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
extern void* Cyc_List_fold_left( void*(* f)( void*, void*), void* accum, struct
Cyc_List_List* x); extern void* Cyc_List_fold_right_c( void*(* f)( void*, void*,
void*), void*, struct Cyc_List_List* x, void* accum); extern struct Cyc_List_List*
Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List* x); extern
unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_forall( int(* pred)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List*
x, struct Cyc_List_List* y); extern struct Cyc_List_List* Cyc_List_tabulate( int
n, void*(* f)( int)); extern struct Cyc_List_List* Cyc_List_filter( int(* f)(
void*), struct Cyc_List_List* x); struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[
12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* cmp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* k, void* v); extern void* Cyc_Dict_lookup( struct
Cyc_Dict_Dict* d, void* k); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict* d, void* k); extern void Cyc_Dict_iter( void(* f)( void*,
void*), struct Cyc_Dict_Dict* d); extern int Cyc_Std_strcmp( struct _tagged_arr
s1, struct _tagged_arr s2); extern struct _tagged_arr Cyc_Std_strconcat( struct
_tagged_arr, struct _tagged_arr); struct Cyc_Hashtable_Table; extern struct Cyc_Hashtable_Table*
Cyc_Hashtable_create( int sz, int(* cmp)( void*, void*), int(* hash)( void*));
extern void Cyc_Hashtable_insert( struct Cyc_Hashtable_Table* t, void* key, void*
val); extern void* Cyc_Hashtable_lookup( struct Cyc_Hashtable_Table* t, void*
key); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct _tagged_arr
line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[ 9u];
struct Cyc_Position_Segment; extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs(
int, int); static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse=
1; static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; static
const int Cyc_Typerep_Var= 0; struct Cyc_Typerep_Var_struct{ int tag; struct
_tagged_arr* f1; } ; static const int Cyc_Typerep_Int= 1; struct Cyc_Typerep_Int_struct{
int tag; unsigned int f1; } ; static const int Cyc_Typerep_Float= 0; static
const int Cyc_Typerep_Double= 1; static const int Cyc_Typerep_ThinPtr= 2; struct
Cyc_Typerep_ThinPtr_struct{ int tag; unsigned int f1; void* f2; } ; static const
int Cyc_Typerep_FatPtr= 3; struct Cyc_Typerep_FatPtr_struct{ int tag; void* f1;
} ; static const int Cyc_Typerep_Tuple= 4; struct _tuple0{ unsigned int f1; void*
f2; } ; struct Cyc_Typerep_Tuple_struct{ int tag; unsigned int f1; struct
_tagged_arr f2; } ; static const int Cyc_Typerep_TUnion= 5; struct Cyc_Typerep_TUnion_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Typerep_Union= 6;
struct Cyc_Typerep_Union_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Typerep_Forall= 7; struct Cyc_Typerep_Forall_struct{ int tag;
struct _tagged_arr f1; void** f2; } ; static const int Cyc_Typerep_App= 8;
struct Cyc_Typerep_App_struct{ int tag; void* f1; struct _tagged_arr f2; } ;
struct _tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref;
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
15u]; extern int Cyc_Absyn_qvar_cmp( struct _tuple1*, struct _tuple1*); extern
void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_at_typ( void* t, void* rgn,
struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_const_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp(
struct _tuple1*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeoftyp_exp( void* t, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_offsetof_exp( void*, void*, struct Cyc_Position_Segment*);
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
_tagged_arr Cyc_Absynpp_qvar2cstring( struct _tuple1*); extern struct
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
struct _tagged_arr fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_compress(
void* t); extern void* Cyc_Tcutil_substitute( struct Cyc_List_List*, void*);
extern void Cyc_Tcutil_check_valid_toplevel_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, void*); struct Cyc_Tcgenrep_RepInfo{ struct Cyc_List_List*
decls; struct Cyc_Absyn_Exp* exp; struct Cyc_List_List* dependencies; struct Cyc_Core_Opt*
fwd_decl; int emitted; } ; static int Cyc_Tcgenrep_typecmp( void* t1, void* t2){
Cyc_Absynpp_set_params(& Cyc_Absynpp_c_params_r);{ struct _tagged_arr t1str= Cyc_Absynpp_typ2string(
t1); struct _tagged_arr t2str= Cyc_Absynpp_typ2string( t2); return Cyc_Std_strcmp(
t1str, t2str);}} struct Cyc_Dict_Dict* Cyc_Tcgenrep_empty_typerep_dict(){ return((
struct Cyc_Dict_Dict*(*)( int(* cmp)( void*, void*))) Cyc_Dict_empty)( Cyc_Tcgenrep_typecmp);}
void Cyc_Tcgenrep_print_dict_entry( void* type, struct Cyc_Tcgenrep_RepInfo*
info){({ struct Cyc_Std_Int_pa_struct _temp3; _temp3.tag= Cyc_Std_Int_pa; _temp3.f1=(
int)(( unsigned int) info->emitted);{ struct Cyc_Std_Int_pa_struct _temp2;
_temp2.tag= Cyc_Std_Int_pa; _temp2.f1=( unsigned int) info;{ struct Cyc_Std_String_pa_struct
_temp1; _temp1.tag= Cyc_Std_String_pa; _temp1.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
type);{ void* _temp0[ 3u]={& _temp1,& _temp2,& _temp3}; Cyc_Std_printf( _tag_arr("(%s,%x:%d,",
sizeof( unsigned char), 11u), _tag_arr( _temp0, sizeof( void*), 3u));}}}});{
struct Cyc_List_List* p= info->dependencies; for( 0; p !=  0; p=(( struct Cyc_List_List*)
_check_null( p))->tl){({ struct Cyc_Std_Int_pa_struct _temp5; _temp5.tag= Cyc_Std_Int_pa;
_temp5.f1=( unsigned int)(( struct Cyc_Tcgenrep_RepInfo*)(( struct Cyc_List_List*)
_check_null( p))->hd);{ void* _temp4[ 1u]={& _temp5}; Cyc_Std_printf( _tag_arr("%x",
sizeof( unsigned char), 3u), _tag_arr( _temp4, sizeof( void*), 1u));}}); if(((
struct Cyc_List_List*) _check_null( p))->tl !=  0){({ void* _temp6[ 0u]={}; Cyc_Std_printf(
_tag_arr(",", sizeof( unsigned char), 2u), _tag_arr( _temp6, sizeof( void*), 0u));});}}}({
void* _temp7[ 0u]={}; Cyc_Std_printf( _tag_arr(")\n", sizeof( unsigned char), 3u),
_tag_arr( _temp7, sizeof( void*), 0u));});} void Cyc_Tcgenrep_print_typerep_dict(
struct Cyc_Dict_Dict* dict){(( void(*)( void(* f)( void*, struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tcgenrep_print_dict_entry, dict);}
static int Cyc_Tcgenrep_rephash( struct Cyc_Tcgenrep_RepInfo* ri){ return( int)
ri;} static int Cyc_Tcgenrep_repcmp( struct Cyc_Tcgenrep_RepInfo* r1, struct Cyc_Tcgenrep_RepInfo*
r2){ unsigned int r1p; unsigned int r2p; r1p=( unsigned int) r1; r2p=(
unsigned int) r2; if( r1 <  r2){ return - 1;} else{ if( r1 ==  r2){ return 0;}
else{ return 1;}}} static struct Cyc_Absyn_Tunionfield* Cyc_Tcgenrep_getField(
struct Cyc_Absyn_Tuniondecl* td, struct _tuple1* fieldname){ if( td->fields == 
0){({ void* _temp8[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr
ap)) Cyc_Tcutil_impos)( _tag_arr("Could not find field in tuniondecl", sizeof(
unsigned char), 35u), _tag_arr( _temp8, sizeof( void*), 0u));});} else{ struct
Cyc_List_List* l=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
td->fields))->v; for( 0; l !=  0; l=(( struct Cyc_List_List*) _check_null( l))->tl){
if( ! Cyc_Absyn_qvar_cmp((( struct Cyc_Absyn_Tunionfield*)(( struct Cyc_List_List*)
_check_null( l))->hd)->name, fieldname)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*) _check_null( l))->hd;}}}({ void* _temp9[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("Could not find field in tuniondecl",
sizeof( unsigned char), 35u), _tag_arr( _temp9, sizeof( void*), 0u));});( int)
_throw(( void*) Cyc_Tcutil_TypeErr);} static unsigned char _temp10[ 9u]="Tcgenrep";
static struct _tagged_arr Cyc_Tcgenrep_tcgenrep_nm={ _temp10, _temp10, _temp10 + 
9u}; static struct Cyc_List_List Cyc_Tcgenrep_l={( void*)& Cyc_Tcgenrep_tcgenrep_nm,
0}; static struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_tcgenrep_nmspace={ 1u,(
struct Cyc_List_List*)& Cyc_Tcgenrep_l}; static void* Cyc_Tcgenrep_tcgenrep_nmspace2=(
void*)& Cyc_Tcgenrep_tcgenrep_nmspace; static unsigned char _temp12[ 8u]="Typerep";
static struct _tagged_arr Cyc_Tcgenrep_typerep_nm={ _temp12, _temp12, _temp12 + 
8u}; static struct Cyc_List_List Cyc_Tcgenrep_l2={( void*)& Cyc_Tcgenrep_typerep_nm,
0}; static struct Cyc_Absyn_Abs_n_struct Cyc_Tcgenrep_typerep_nmspace={ 1u,(
struct Cyc_List_List*)& Cyc_Tcgenrep_l2}; static struct Cyc_List_List* Cyc_Tcgenrep_dfsvisit(
struct Cyc_Tcgenrep_RepInfo* ri, struct Cyc_Hashtable_Table* visited){ struct
_handler_cons _temp14; _push_handler(& _temp14);{ int _temp16= 0; if( setjmp(
_temp14.handler)){ _temp16= 1;} if( ! _temp16){(( int(*)( struct Cyc_Hashtable_Table*
t, struct Cyc_Tcgenrep_RepInfo* key)) Cyc_Hashtable_lookup)( visited, ri);{
struct Cyc_List_List* _temp17= 0; _npop_handler( 0u); return _temp17;};
_pop_handler();} else{ void* _temp15=( void*) _exn_thrown; void* _temp19=
_temp15; _LL21: if( _temp19 ==  Cyc_Core_Not_found){ goto _LL22;} else{ goto
_LL23;} _LL23: goto _LL24; _LL22:(( void(*)( struct Cyc_Hashtable_Table* t,
struct Cyc_Tcgenrep_RepInfo* key, int val)) Cyc_Hashtable_insert)( visited, ri,
1);{ struct Cyc_List_List* ds= 0;{ struct Cyc_List_List* l= ri->dependencies;
for( 0; l !=  0; l=(( struct Cyc_List_List*) _check_null( l))->tl){ ds=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
ds, Cyc_Tcgenrep_dfsvisit(( struct Cyc_Tcgenrep_RepInfo*)(( struct Cyc_List_List*)
_check_null( l))->hd, visited));}} if( ri->emitted ==  0){ if( ri->fwd_decl != 
0){ ds=({ struct Cyc_List_List* _temp25=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp25->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp26[ 1u]; _temp26[ 0u]=( struct Cyc_Absyn_Decl*)(( struct Cyc_Core_Opt*)
_check_null( ri->fwd_decl))->v;(( struct Cyc_List_List*(*)( struct _tagged_arr))
Cyc_List_list)( _tag_arr( _temp26, sizeof( struct Cyc_Absyn_Decl*), 1u));});
_temp25->tl= ds; _temp25;});}{ struct Cyc_List_List* _temp27=({ struct Cyc_List_List*
_temp28=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp28->hd=( void*) ri->decls; _temp28->tl= 0; _temp28;}); ri->emitted= 1;
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( ds, _temp27);}} else{ return ds;}} _LL24:( void)
_throw( _temp19); _LL20:;}}} static struct Cyc_List_List* Cyc_Tcgenrep_dfs(
struct Cyc_Tcgenrep_RepInfo* ri){ struct Cyc_Hashtable_Table* tab=(( struct Cyc_Hashtable_Table*(*)(
int sz, int(* cmp)( struct Cyc_Tcgenrep_RepInfo*, struct Cyc_Tcgenrep_RepInfo*),
int(* hash)( struct Cyc_Tcgenrep_RepInfo*))) Cyc_Hashtable_create)( 53, Cyc_Tcgenrep_repcmp,
Cyc_Tcgenrep_rephash); struct Cyc_List_List* ds= Cyc_Tcgenrep_dfsvisit( ri, tab);
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)(
ds);} static unsigned char _temp29[ 11u]="Typestruct"; static struct _tagged_arr
Cyc_Tcgenrep_typestruct_str={ _temp29, _temp29, _temp29 +  11u}; static
unsigned char _temp30[ 4u]="Var"; static struct _tagged_arr Cyc_Tcgenrep_var_str={
_temp30, _temp30, _temp30 +  4u}; static unsigned char _temp31[ 4u]="Int";
static struct _tagged_arr Cyc_Tcgenrep_int_str={ _temp31, _temp31, _temp31 +  4u};
static unsigned char _temp32[ 6u]="Float"; static struct _tagged_arr Cyc_Tcgenrep_float_str={
_temp32, _temp32, _temp32 +  6u}; static unsigned char _temp33[ 7u]="Double";
static struct _tagged_arr Cyc_Tcgenrep_double_str={ _temp33, _temp33, _temp33 + 
7u}; static unsigned char _temp34[ 8u]="ThinPtr"; static struct _tagged_arr Cyc_Tcgenrep_thinptr_str={
_temp34, _temp34, _temp34 +  8u}; static unsigned char _temp35[ 7u]="FatPtr";
static struct _tagged_arr Cyc_Tcgenrep_fatptr_str={ _temp35, _temp35, _temp35 + 
7u}; static unsigned char _temp36[ 6u]="Tuple"; static struct _tagged_arr Cyc_Tcgenrep_tuple_str={
_temp36, _temp36, _temp36 +  6u}; static unsigned char _temp37[ 7u]="TUnion";
static struct _tagged_arr Cyc_Tcgenrep_tunion_str={ _temp37, _temp37, _temp37 + 
7u}; static unsigned char _temp38[ 6u]="Union"; static struct _tagged_arr Cyc_Tcgenrep_union_str={
_temp38, _temp38, _temp38 +  6u}; static struct _tuple1 Cyc_Tcgenrep_typestruct_name={(
void*)& Cyc_Tcgenrep_typerep_nmspace,& Cyc_Tcgenrep_typestruct_str}; static
struct Cyc_Absyn_Tuniondecl** Cyc_Tcgenrep_typestruct_decl= 0; static int Cyc_Tcgenrep_gen_id_counter=
0; static struct _tagged_arr* Cyc_Tcgenrep_new_gen_id( struct _tagged_arr name){
return({ struct _tagged_arr* _temp39=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp39[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp42; _temp42.tag= Cyc_Std_Int_pa; _temp42.f1=( int)(( unsigned int) Cyc_Tcgenrep_gen_id_counter
++);{ struct Cyc_Std_String_pa_struct _temp41; _temp41.tag= Cyc_Std_String_pa;
_temp41.f1=( struct _tagged_arr) name;{ void* _temp40[ 2u]={& _temp41,& _temp42};
Cyc_Std_aprintf( _tag_arr("_gen%s_%d", sizeof( unsigned char), 10u), _tag_arr(
_temp40, sizeof( void*), 2u));}}}); _temp39;});} static void Cyc_Tcgenrep_print_params(
struct Cyc_List_List* l){({ void* _temp43[ 0u]={}; Cyc_Std_printf( _tag_arr("<",
sizeof( unsigned char), 2u), _tag_arr( _temp43, sizeof( void*), 0u));});{ struct
Cyc_List_List* p= l; for( 0; p !=  0; p=(( struct Cyc_List_List*) _check_null( p))->tl){({
struct Cyc_Std_Int_pa_struct _temp46; _temp46.tag= Cyc_Std_Int_pa; _temp46.f1=(
int)(( unsigned int)(( int)((( struct Cyc_List_List*) _check_null( p))->tl !=  0?',':' ')));{
struct Cyc_Std_String_pa_struct _temp45; _temp45.tag= Cyc_Std_String_pa; _temp45.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_List_List*)
_check_null( p))->hd);{ void* _temp44[ 2u]={& _temp45,& _temp46}; Cyc_Std_printf(
_tag_arr("%s%c", sizeof( unsigned char), 5u), _tag_arr( _temp44, sizeof( void*),
2u));}}});}}({ void* _temp47[ 0u]={}; Cyc_Std_printf( _tag_arr(">\n", sizeof(
unsigned char), 3u), _tag_arr( _temp47, sizeof( void*), 0u));});} static void
Cyc_Tcgenrep_print_tvars( struct Cyc_List_List* l){({ void* _temp48[ 0u]={}; Cyc_Std_printf(
_tag_arr("<", sizeof( unsigned char), 2u), _tag_arr( _temp48, sizeof( void*), 0u));});{
struct Cyc_List_List* p= l; for( 0; p !=  0; p=(( struct Cyc_List_List*)
_check_null( p))->tl){({ struct Cyc_Std_Int_pa_struct _temp51; _temp51.tag= Cyc_Std_Int_pa;
_temp51.f1=( int)(( unsigned int)(( int)((( struct Cyc_List_List*) _check_null(
p))->tl !=  0?',':' ')));{ struct Cyc_Std_String_pa_struct _temp50; _temp50.tag=
Cyc_Std_String_pa; _temp50.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( p))->hd)->name;{ void* _temp49[ 2u]={&
_temp50,& _temp51}; Cyc_Std_printf( _tag_arr("%s%c", sizeof( unsigned char), 5u),
_tag_arr( _temp49, sizeof( void*), 2u));}}});}}({ void* _temp52[ 0u]={}; Cyc_Std_printf(
_tag_arr(">\n", sizeof( unsigned char), 3u), _tag_arr( _temp52, sizeof( void*),
0u));});} static struct Cyc_Absyn_Rel_n_struct Cyc_Tcgenrep_top_nm_0={ 0u, 0};
static void* Cyc_Tcgenrep_top_nm=( void*)& Cyc_Tcgenrep_top_nm_0; static struct
_tuple1* Cyc_Tcgenrep_toplevel_name( struct _tagged_arr* name){ return({ struct
_tuple1* _temp54=( struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp54->f1=
Cyc_Tcgenrep_top_nm; _temp54->f2=( struct _tagged_arr*) name; _temp54;});}
static struct _tuple1* Cyc_Tcgenrep_relative_name( struct Cyc_List_List* pathl,
struct _tagged_arr* name){ struct Cyc_Absyn_Rel_n_struct* path=({ struct Cyc_Absyn_Rel_n_struct*
_temp56=( struct Cyc_Absyn_Rel_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp56[ 0]=({ struct Cyc_Absyn_Rel_n_struct _temp57; _temp57.tag= Cyc_Absyn_Rel_n;
_temp57.f1= pathl; _temp57;}); _temp56;}); return({ struct _tuple1* _temp55=(
struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp55->f1=( void*) path;
_temp55->f2=( struct _tagged_arr*) name; _temp55;});} static struct Cyc_Absyn_Decl*
Cyc_Tcgenrep_gen_decl( struct _tagged_arr* name, void* type, struct Cyc_Absyn_Exp*
init, struct Cyc_Position_Segment* seg){ struct _tuple1* qvar= Cyc_Tcgenrep_toplevel_name(
name); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( qvar, type, init);(
void*)( vd->sc=( void*)(( void*) Cyc_Absyn_Static));{ struct Cyc_Absyn_Var_d_struct*
r1=({ struct Cyc_Absyn_Var_d_struct* _temp58=( struct Cyc_Absyn_Var_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp58[ 0]=({ struct Cyc_Absyn_Var_d_struct
_temp59; _temp59.tag= Cyc_Absyn_Var_d; _temp59.f1= vd; _temp59;}); _temp58;});
void* r2=( void*) r1; return Cyc_Absyn_new_decl( r2, seg);}} static struct Cyc_Absyn_Decl*
Cyc_Tcgenrep_gen_vardecl_sc( struct _tagged_arr* name, void* type, struct Cyc_Absyn_Exp*
init, void* sc, struct Cyc_Position_Segment* seg){ struct _tuple1* qvar= Cyc_Tcgenrep_toplevel_name(
name); struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl( qvar, type, init);(
void*)( vd->sc=( void*) sc);{ struct Cyc_Absyn_Var_d_struct* r1=({ struct Cyc_Absyn_Var_d_struct*
_temp60=( struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp60[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp61; _temp61.tag= Cyc_Absyn_Var_d;
_temp61.f1= vd; _temp61;}); _temp60;}); void* r2=( void*) r1; return Cyc_Absyn_new_decl(
r2, seg);}} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_cnst_string( struct
_tagged_arr s, struct Cyc_Position_Segment* seg){ return Cyc_Absyn_const_exp((
void*)({ struct Cyc_Absyn_String_c_struct* _temp62=( struct Cyc_Absyn_String_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp62[ 0]=({ struct Cyc_Absyn_String_c_struct
_temp63; _temp63.tag= Cyc_Absyn_String_c; _temp63.f1= s; _temp63;}); _temp62;}),
seg);} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_cnst_int( int i, struct Cyc_Position_Segment*
seg){ return Cyc_Absyn_const_exp(( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp64=( struct Cyc_Absyn_Int_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp64[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp65; _temp65.tag= Cyc_Absyn_Int_c;
_temp65.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp65.f2= i; _temp65;});
_temp64;}), seg);} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_cnst_int_cls(
struct Cyc_Position_Segment* seg, int i){ return Cyc_Absyn_const_exp(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp66=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp66[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp67; _temp67.tag= Cyc_Absyn_Int_c; _temp67.f1=( void*)(( void*) Cyc_Absyn_Signed);
_temp67.f2= i; _temp67;}); _temp66;}), seg);} static int Cyc_Tcgenrep_size_of2int(
void* sz){ void* _temp68= sz; _LL70: if( _temp68 == ( void*) Cyc_Absyn_B1){ goto
_LL71;} else{ goto _LL72;} _LL72: if( _temp68 == ( void*) Cyc_Absyn_B2){ goto
_LL73;} else{ goto _LL74;} _LL74: if( _temp68 == ( void*) Cyc_Absyn_B4){ goto
_LL75;} else{ goto _LL76;} _LL76: if( _temp68 == ( void*) Cyc_Absyn_B8){ goto
_LL77;} else{ goto _LL69;} _LL71: return 8; _LL73: return 16; _LL75: return 32;
_LL77: return 64; _LL69:;} static void* Cyc_Tcgenrep_array_typ( void* ty, struct
Cyc_Absyn_Tqual tq, struct Cyc_Absyn_Exp* e){ return( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp78=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp78[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp79; _temp79.tag= Cyc_Absyn_ArrayType;
_temp79.f1=( void*) ty; _temp79.f2= tq; _temp79.f3= e; _temp79;}); _temp78;});}
static void* Cyc_Tcgenrep_tunion_typ( struct _tagged_arr* name){ return( void*)({
struct Cyc_Absyn_TunionType_struct* _temp80=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp80[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp81; _temp81.tag= Cyc_Absyn_TunionType; _temp81.f1=({
struct Cyc_Absyn_TunionInfo _temp82; _temp82.tunion_info=( void*)(( void*)({
struct Cyc_Absyn_UnknownTunion_struct* _temp83=( struct Cyc_Absyn_UnknownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnknownTunion_struct)); _temp83[ 0]=({
struct Cyc_Absyn_UnknownTunion_struct _temp84; _temp84.tag= Cyc_Absyn_UnknownTunion;
_temp84.f1=({ struct Cyc_Absyn_UnknownTunionInfo _temp85; _temp85.name= Cyc_Tcgenrep_toplevel_name(
name); _temp85.is_xtunion= 0; _temp85;}); _temp84;}); _temp83;})); _temp82.targs=
0; _temp82.rgn=( void*)(( void*) Cyc_Absyn_HeapRgn); _temp82;}); _temp81;});
_temp80;});} static void* Cyc_Tcgenrep_tunionfield_typ( struct _tagged_arr* name,
struct _tagged_arr* fieldname){ return( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp86=( struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp86[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp87; _temp87.tag= Cyc_Absyn_TunionFieldType; _temp87.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp88; _temp88.field_info=( void*)(( void*)({ struct Cyc_Absyn_UnknownTunionfield_struct*
_temp89=( struct Cyc_Absyn_UnknownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnknownTunionfield_struct)); _temp89[ 0]=({ struct Cyc_Absyn_UnknownTunionfield_struct
_temp90; _temp90.tag= Cyc_Absyn_UnknownTunionfield; _temp90.f1=({ struct Cyc_Absyn_UnknownTunionFieldInfo
_temp91; _temp91.tunion_name= Cyc_Tcgenrep_toplevel_name( name); _temp91.field_name=
Cyc_Tcgenrep_toplevel_name( fieldname); _temp91.is_xtunion= 0; _temp91;});
_temp90;}); _temp89;})); _temp88.targs= 0; _temp88;}); _temp87;}); _temp86;});}
static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_call_exp( struct _tuple1* name, struct
Cyc_List_List* args, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_unknowncall_exp(
Cyc_Absyn_unknownid_exp( name, loc), args, loc);} static struct Cyc_Absyn_Decl*
Cyc_Tcgenrep_tunion_constructor_decl( struct _tagged_arr* tunionname, struct
_tagged_arr* fieldname, struct _tagged_arr* varname, struct Cyc_List_List* args,
struct Cyc_Position_Segment* loc){ void* _temp92= Cyc_Tcgenrep_tunionfield_typ(
tunionname, fieldname); struct Cyc_Absyn_Exp* _temp93= Cyc_Tcgenrep_call_exp(
Cyc_Tcgenrep_toplevel_name( fieldname), args, loc); struct Cyc_Absyn_Decl*
_temp94= Cyc_Tcgenrep_gen_decl( varname, _temp92,( struct Cyc_Absyn_Exp*)
_temp93, loc); return _temp94;} struct _tuple4{ struct Cyc_Absyn_Tqual f1; void*
f2; } ; static void* Cyc_Tcgenrep_get_second( struct _tuple4* pair){ return(*
pair).f2;} struct _tuple5{ struct _tagged_arr* f1; struct Cyc_Absyn_Exp* f2; } ;
static struct _tagged_arr* Cyc_Tcgenrep_get_first( struct _tuple5* pair){ return(*
pair).f1;} static unsigned char _temp95[ 5u]="list"; static struct _tagged_arr
Cyc_Tcgenrep_list_str={ _temp95, _temp95, _temp95 +  5u}; static unsigned char
_temp96[ 5u]="List"; static struct _tagged_arr Cyc_Tcgenrep_List_str={ _temp96,
_temp96, _temp96 +  5u}; struct _tuple6{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp*
f2; } ; static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_tuple2_exp_cls( struct Cyc_Position_Segment*
loc, struct _tuple6* es){ struct _tuple6 _temp99; struct Cyc_Absyn_Exp* _temp100;
struct Cyc_Absyn_Exp* _temp102; struct _tuple6* _temp97= es; _temp99=* _temp97;
_LL103: _temp102= _temp99.f1; goto _LL101; _LL101: _temp100= _temp99.f2; goto
_LL98; _LL98: return Cyc_Absyn_tuple_exp(({ struct Cyc_Absyn_Exp* _temp104[ 2u];
_temp104[ 1u]= _temp100; _temp104[ 0u]= _temp102;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp104, sizeof( struct Cyc_Absyn_Exp*),
2u));}), loc);} static unsigned char _temp105[ 5u]="NULL"; static struct
_tagged_arr Cyc_Tcgenrep_null_str={ _temp105, _temp105, _temp105 +  5u}; static
struct Cyc_Absyn_Exp* Cyc_Tcgenrep_list_exp( struct Cyc_List_List* l, struct Cyc_Position_Segment*
loc){ if( l ==  0){ return Cyc_Absyn_null_exp( loc);} return Cyc_Tcgenrep_call_exp(
Cyc_Tcgenrep_toplevel_name(& Cyc_Tcgenrep_list_str), l, loc);} struct _tuple7{
void* f1; struct Cyc_Position_Segment* f2; } ; static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_make_offsetof_exp(
struct _tuple7* typeloc, int index){ struct _tuple7 _temp108; struct Cyc_Position_Segment*
_temp109; void* _temp111; struct _tuple7* _temp106= typeloc; _temp108=* _temp106;
_LL112: _temp111= _temp108.f1; goto _LL110; _LL110: _temp109= _temp108.f2; goto
_LL107; _LL107: return Cyc_Absyn_offsetof_exp( _temp111,( void*)({ struct Cyc_Absyn_TupleIndex_struct*
_temp113=( struct Cyc_Absyn_TupleIndex_struct*) _cycalloc_atomic( sizeof( struct
Cyc_Absyn_TupleIndex_struct)); _temp113[ 0]=({ struct Cyc_Absyn_TupleIndex_struct
_temp114; _temp114.tag= Cyc_Absyn_TupleIndex; _temp114.f1=( unsigned int) index;
_temp114;}); _temp113;}), _temp109);} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_get_and_cast_ri_exp(
struct Cyc_Position_Segment* loc, struct Cyc_Tcgenrep_RepInfo* info){ return Cyc_Absyn_cast_exp(
Cyc_Tcgenrep_tunion_typ(& Cyc_Tcgenrep_typestruct_str),( struct Cyc_Absyn_Exp*)
_check_null( info->exp), loc);} static struct _tuple5* Cyc_Tcgenrep_gen_id_for_exp(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple5* _temp115=( struct _tuple5*)
_cycalloc( sizeof( struct _tuple5)); _temp115->f1= Cyc_Tcgenrep_new_gen_id(
_tag_arr("tuple", sizeof( unsigned char), 6u)); _temp115->f2= e; _temp115;});}
static struct Cyc_Absyn_Decl* Cyc_Tcgenrep_gen_decl_cls( struct _tuple7* env,
struct _tuple5* name_exp){ struct _tuple7 _temp118; struct Cyc_Position_Segment*
_temp119; void* _temp121; struct _tuple7* _temp116= env; _temp118=* _temp116;
_LL122: _temp121= _temp118.f1; goto _LL120; _LL120: _temp119= _temp118.f2; goto
_LL117; _LL117: { struct _tuple5 _temp125; struct Cyc_Absyn_Exp* _temp126;
struct _tagged_arr* _temp128; struct _tuple5* _temp123= name_exp; _temp125=*
_temp123; _LL129: _temp128= _temp125.f1; goto _LL127; _LL127: _temp126= _temp125.f2;
goto _LL124; _LL124: return Cyc_Tcgenrep_gen_decl( _temp128, _temp121,( struct
Cyc_Absyn_Exp*) _temp126, _temp119);}} struct _tuple8{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static struct _tuple8* Cyc_Tcgenrep_null_designator_exp(
struct Cyc_Absyn_Exp* e){ return({ struct _tuple8* _temp130=( struct _tuple8*)
_cycalloc( sizeof( struct _tuple8)); _temp130->f1= 0; _temp130->f2= e; _temp130;});}
static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_arr_init_exp( struct Cyc_List_List* l,
struct Cyc_Position_Segment* loc){ struct Cyc_List_List* _temp131=(( struct Cyc_List_List*(*)(
struct _tuple8*(* f)( struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcgenrep_null_designator_exp, l); return Cyc_Absyn_unresolvedmem_exp( 0,
_temp131, loc);} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_address_exp_cls(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e){ return Cyc_Absyn_address_exp(
e, loc);} static struct Cyc_Absyn_Exp* Cyc_Tcgenrep_unknownid_exp_cls( struct
Cyc_Position_Segment* loc, struct _tuple1* e){ return Cyc_Absyn_unknownid_exp( e,
loc);} static int Cyc_Tcgenrep_has_bitfield( struct Cyc_Absyn_Structfield* sf){
return sf->width !=  0;} static int Cyc_Tcgenrep_add_bitfield_sizes( int total,
struct Cyc_Absyn_Structfield* sf){ return( int)( Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( sf->width)) +  total);} static void* Cyc_Tcgenrep_select_structfield_type(
struct Cyc_Absyn_Structfield* sf){ if( Cyc_Std_strcmp(* sf->name, _tag_arr("",
sizeof( unsigned char), 1u)) ==  0){({ void* _temp132[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): anonymous (padding) structfield not handled yet",
sizeof( unsigned char), 55u), _tag_arr( _temp132, sizeof( void*), 0u));});} if(
Cyc_Tcgenrep_has_bitfield( sf)){({ void* _temp133[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): mixed struct bitfields and union bitfields not handled",
sizeof( unsigned char), 62u), _tag_arr( _temp133, sizeof( void*), 0u));});}
return( void*) sf->type;} static struct Cyc_Dict_Dict* Cyc_Tcgenrep_update_info(
struct Cyc_Dict_Dict* dict, void* type, struct Cyc_List_List* decls, struct Cyc_Absyn_Exp*
exp, struct Cyc_List_List* dependencies, struct Cyc_Core_Opt* fwd_decl){ struct
Cyc_Core_Opt* _temp134=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, void*
k)) Cyc_Dict_lookup_opt)( dict, type); if( _temp134 !=  0){ if((( struct Cyc_Tcgenrep_RepInfo*)((
struct Cyc_Core_Opt*) _check_null( _temp134))->v)->decls !=  0){ Cyc_Tcgenrep_print_typerep_dict(
dict);({ void* _temp135[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("Updating non-forward declaration",
sizeof( unsigned char), 33u), _tag_arr( _temp135, sizeof( void*), 0u));});}((
struct Cyc_Tcgenrep_RepInfo*)(( struct Cyc_Core_Opt*) _check_null( _temp134))->v)->decls=
decls;(( struct Cyc_Tcgenrep_RepInfo*)(( struct Cyc_Core_Opt*) _check_null(
_temp134))->v)->exp= exp;(( struct Cyc_Tcgenrep_RepInfo*)(( struct Cyc_Core_Opt*)
_check_null( _temp134))->v)->dependencies= dependencies; return dict;} else{
return(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* k, struct Cyc_Tcgenrep_RepInfo*
v)) Cyc_Dict_insert)( dict, type,({ struct Cyc_Tcgenrep_RepInfo* _temp136=(
struct Cyc_Tcgenrep_RepInfo*) _cycalloc( sizeof( struct Cyc_Tcgenrep_RepInfo));
_temp136->decls= decls; _temp136->exp= exp; _temp136->dependencies= dependencies;
_temp136->fwd_decl= fwd_decl; _temp136->emitted= 0; _temp136;}));}} static
struct Cyc_Absyn_Tqual Cyc_Tcgenrep_tq_none={ 0, 0, 0}; static struct _tuple4*
Cyc_Tcgenrep_tqual_type( struct Cyc_Absyn_Tqual* tq, void* type){ return({
struct _tuple4* _temp137=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp137->f1=*(( struct Cyc_Absyn_Tqual*) _check_null( tq)); _temp137->f2= type;
_temp137;});} static void* Cyc_Tcgenrep_tuple_typ( struct Cyc_List_List* types){
struct Cyc_List_List* _temp138=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct Cyc_Absyn_Tqual*, void*), struct Cyc_Absyn_Tqual* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcgenrep_tqual_type,( struct Cyc_Absyn_Tqual*)& Cyc_Tcgenrep_tq_none,
types); struct Cyc_Absyn_TupleType_struct* tuple_type_base=({ struct Cyc_Absyn_TupleType_struct*
_temp139=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp139[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp140; _temp140.tag= Cyc_Absyn_TupleType;
_temp140.f1= _temp138; _temp140;}); _temp139;}); void* tuple_type=( void*)
tuple_type_base; return tuple_type;} static struct _tuple8* Cyc_Tcgenrep_array_decls(
void* type, struct Cyc_List_List* exps, struct Cyc_Position_Segment* loc){
struct Cyc_List_List* _temp141=(( struct Cyc_List_List*(*)( struct _tuple5*(* f)(
struct Cyc_Absyn_Exp*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_gen_id_for_exp,
exps); struct Cyc_List_List* _temp142=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(*
f)( struct _tuple7*, struct _tuple5*), struct _tuple7* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcgenrep_gen_decl_cls,({ struct _tuple7* _temp155=(
struct _tuple7*) _cycalloc( sizeof( struct _tuple7)); _temp155->f1= type;
_temp155->f2= loc; _temp155;}), _temp141); struct Cyc_List_List* _temp143=((
struct Cyc_List_List*(*)( struct _tagged_arr*(* f)( struct _tuple5*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcgenrep_get_first, _temp141); struct Cyc_List_List*
_temp144=(( struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tagged_arr*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_toplevel_name, _temp143);
struct Cyc_List_List* _temp145=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(*
f)( struct Cyc_Position_Segment*, struct _tuple1*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_unknownid_exp_cls,
loc, _temp144); struct Cyc_List_List* _temp146=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*, struct Cyc_Absyn_Exp*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_address_exp_cls,
loc, _temp145); struct Cyc_Absyn_Exp* _temp147= Cyc_Tcgenrep_arr_init_exp(
_temp146, loc); struct _tagged_arr* _temp148= Cyc_Tcgenrep_new_gen_id( _tag_arr("arr",
sizeof( unsigned char), 4u)); void* _temp149= Cyc_Absyn_at_typ( type,( void*)
Cyc_Absyn_HeapRgn, Cyc_Tcgenrep_tq_none); void* _temp150= Cyc_Tcgenrep_array_typ(
_temp149, Cyc_Tcgenrep_tq_none, 0); struct Cyc_Absyn_Decl* _temp151= Cyc_Tcgenrep_gen_decl(
_temp148, _temp150,( struct Cyc_Absyn_Exp*) _temp147, loc); struct Cyc_Absyn_Exp*
_temp152= Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( _temp148), loc);
return({ struct _tuple8* _temp153=( struct _tuple8*) _cycalloc( sizeof( struct
_tuple8)); _temp153->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp142,({ struct Cyc_Absyn_Decl*
_temp154[ 1u]; _temp154[ 0u]= _temp151;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp154, sizeof( struct Cyc_Absyn_Decl*),
1u));})); _temp153->f2= _temp152; _temp153;});} static void* Cyc_Tcgenrep_check_tunionfield_and_get_type(
struct Cyc_Absyn_Tunionfield* tuf){ if( tuf->tvs !=  0){({ void* _temp156[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("gen(): tunionfield type varaibles not handled", sizeof( unsigned char),
46u), _tag_arr( _temp156, sizeof( void*), 0u));});} return( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp157=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp157[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp158; _temp158.tag= Cyc_Absyn_TupleType;
_temp158.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)(({ struct _tuple4* _temp159[ 1u]; _temp159[ 0u]=({
struct _tuple4* _temp160=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp160->f1= Cyc_Tcgenrep_tq_none; _temp160->f2= Cyc_Absyn_uint_t; _temp160;});((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp159, sizeof( struct _tuple4*), 1u));}), tuf->typs); _temp158;}); _temp157;});}
static int Cyc_Tcgenrep_filter_empty_tunionfield( struct Cyc_Absyn_Tunionfield*
tuf){ return tuf->typs !=  0;} static struct _tagged_arr* Cyc_Tcgenrep_get_tunionfield_name(
struct Cyc_Absyn_Tunionfield* tuf){ return(* tuf->name).f2;} static void* Cyc_Tcgenrep_id(
void* x){ return x;} struct Cyc_Absyn_Structfield* Cyc_Tcgenrep_substitute_structfield_type(
struct Cyc_List_List* subst, struct Cyc_Absyn_Structfield* sf){ return({ struct
Cyc_Absyn_Structfield* _temp161=( struct Cyc_Absyn_Structfield*) _cycalloc(
sizeof( struct Cyc_Absyn_Structfield)); _temp161->name= sf->name; _temp161->tq=
sf->tq; _temp161->type=( void*) Cyc_Tcutil_substitute( subst,( void*) sf->type);
_temp161->width= sf->width; _temp161->attributes= sf->attributes; _temp161;});}
struct _tuple4* Cyc_Tcgenrep_substitute_tqual_type( struct Cyc_List_List* subst,
struct _tuple4* pair){ struct _tuple4 _temp164; void* _temp165; struct Cyc_Absyn_Tqual
_temp167; struct _tuple4* _temp162= pair; _temp164=* _temp162; _LL168: _temp167=
_temp164.f1; goto _LL166; _LL166: _temp165= _temp164.f2; goto _LL163; _LL163:
return({ struct _tuple4* _temp169=( struct _tuple4*) _cycalloc( sizeof( struct
_tuple4)); _temp169->f1= _temp167; _temp169->f2= Cyc_Tcutil_substitute( subst,
_temp165); _temp169;});} struct Cyc_Absyn_Tunionfield* Cyc_Tcgenrep_substitute_tunionfield_type(
struct Cyc_List_List* subst, struct Cyc_Absyn_Tunionfield* tf){ return({ struct
Cyc_Absyn_Tunionfield* _temp170=( struct Cyc_Absyn_Tunionfield*) _cycalloc(
sizeof( struct Cyc_Absyn_Tunionfield)); _temp170->name= tf->name; _temp170->tvs=
tf->tvs; _temp170->typs=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)(
struct Cyc_List_List*, struct _tuple4*), struct Cyc_List_List* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcgenrep_substitute_tqual_type, subst, tf->typs);
_temp170->loc= tf->loc; _temp170->sc=( void*)(( void*) tf->sc); _temp170;});}
void* Cyc_Tcgenrep_monomorphize_type( void* type){ void* _temp171= Cyc_Tcutil_compress(
type); struct Cyc_Absyn_Structdecl** _temp181; struct Cyc_List_List* _temp183;
struct _tuple1* _temp185; struct Cyc_Absyn_Uniondecl** _temp187; struct Cyc_List_List*
_temp189; struct _tuple1* _temp191; struct Cyc_Absyn_TunionInfo _temp193; void*
_temp195; struct Cyc_List_List* _temp197; void* _temp199; struct Cyc_Absyn_Tuniondecl**
_temp201; struct Cyc_Absyn_Tuniondecl* _temp203; _LL173: if(( unsigned int)
_temp171 >  4u?*(( int*) _temp171) ==  Cyc_Absyn_StructType: 0){ _LL186:
_temp185=(( struct Cyc_Absyn_StructType_struct*) _temp171)->f1; goto _LL184;
_LL184: _temp183=(( struct Cyc_Absyn_StructType_struct*) _temp171)->f2; goto
_LL182; _LL182: _temp181=(( struct Cyc_Absyn_StructType_struct*) _temp171)->f3;
goto _LL174;} else{ goto _LL175;} _LL175: if(( unsigned int) _temp171 >  4u?*((
int*) _temp171) ==  Cyc_Absyn_UnionType: 0){ _LL192: _temp191=(( struct Cyc_Absyn_UnionType_struct*)
_temp171)->f1; goto _LL190; _LL190: _temp189=(( struct Cyc_Absyn_UnionType_struct*)
_temp171)->f2; goto _LL188; _LL188: _temp187=(( struct Cyc_Absyn_UnionType_struct*)
_temp171)->f3; goto _LL176;} else{ goto _LL177;} _LL177: if(( unsigned int)
_temp171 >  4u?*(( int*) _temp171) ==  Cyc_Absyn_TunionType: 0){ _LL194:
_temp193=(( struct Cyc_Absyn_TunionType_struct*) _temp171)->f1; _LL200: _temp199=(
void*) _temp193.tunion_info; if(*(( int*) _temp199) ==  Cyc_Absyn_KnownTunion){
_LL202: _temp201=(( struct Cyc_Absyn_KnownTunion_struct*) _temp199)->f1;
_temp203=* _temp201; goto _LL198;} else{ goto _LL179;} _LL198: _temp197=
_temp193.targs; goto _LL196; _LL196: _temp195=( void*) _temp193.rgn; goto _LL178;}
else{ goto _LL179;} _LL179: goto _LL180; _LL174: if( _temp181 ==  0){({ void*
_temp204[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("struct decl must exist at this point", sizeof( unsigned char), 37u),
_tag_arr( _temp204, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structdecl*
_temp205=*(( struct Cyc_Absyn_Structdecl**) _check_null( _temp181)); struct Cyc_List_List*
_temp206= _temp205->tvs; if( Cyc_List_length( _temp183) != (( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp206)){({ struct Cyc_Std_Int_pa_struct _temp209;
_temp209.tag= Cyc_Std_Int_pa; _temp209.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp206));{ struct Cyc_Std_Int_pa_struct
_temp208; _temp208.tag= Cyc_Std_Int_pa; _temp208.f1=( int)(( unsigned int) Cyc_List_length(
_temp183));{ void* _temp207[ 2u]={& _temp208,& _temp209};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof( unsigned char), 60u), _tag_arr( _temp207, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp210=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp206, _temp183); struct Cyc_Core_Opt*
fields= 0; if( _temp205->fields !=  0){ fields=({ struct Cyc_Core_Opt* _temp211=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp211->v=(
void*)(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_substitute_structfield_type,
_temp210,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp205->fields))->v);
_temp211;});}{ struct Cyc_Absyn_Structdecl* stdecl2=({ struct Cyc_Absyn_Structdecl*
_temp217=( struct Cyc_Absyn_Structdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp217->sc=( void*)(( void*) _temp205->sc); _temp217->name= _temp205->name;
_temp217->tvs= 0; _temp217->fields= fields; _temp217->attributes= _temp205->attributes;
_temp217;}); struct Cyc_Absyn_Structdecl** _temp212=({ struct Cyc_Absyn_Structdecl**
_temp216=( struct Cyc_Absyn_Structdecl**) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp216[ 0]= stdecl2; _temp216;}); struct Cyc_Absyn_StructType_struct* _temp213=({
struct Cyc_Absyn_StructType_struct* _temp214=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp214[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp215; _temp215.tag= Cyc_Absyn_StructType;
_temp215.f1= _temp185; _temp215.f2= _temp183; _temp215.f3= _temp212; _temp215;});
_temp214;}); return( void*) _temp213;}}} _LL176: if( _temp187 ==  0){({ void*
_temp218[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("struct decl must exist at this point", sizeof( unsigned char), 37u),
_tag_arr( _temp218, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Uniondecl*
_temp219=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp187)); struct Cyc_List_List*
_temp220= _temp219->tvs; if( Cyc_List_length( _temp189) != (( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp220)){({ struct Cyc_Std_Int_pa_struct _temp223;
_temp223.tag= Cyc_Std_Int_pa; _temp223.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp220));{ struct Cyc_Std_Int_pa_struct
_temp222; _temp222.tag= Cyc_Std_Int_pa; _temp222.f1=( int)(( unsigned int) Cyc_List_length(
_temp189));{ void* _temp221[ 2u]={& _temp222,& _temp223};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof( unsigned char), 60u), _tag_arr( _temp221, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp224=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp220, _temp189); struct Cyc_Core_Opt*
fields= 0; if( _temp219->fields !=  0){ fields=({ struct Cyc_Core_Opt* _temp225=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp225->v=(
void*)(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Structfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_substitute_structfield_type,
_temp224,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp219->fields))->v);
_temp225;});}{ struct Cyc_Absyn_Structdecl* udecl2=({ struct Cyc_Absyn_Structdecl*
_temp231=( struct Cyc_Absyn_Structdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp231->sc=( void*)(( void*) _temp219->sc); _temp231->name= _temp219->name;
_temp231->tvs= 0; _temp231->fields= fields; _temp231->attributes= _temp219->attributes;
_temp231;}); struct Cyc_Absyn_Structdecl** _temp226=({ struct Cyc_Absyn_Structdecl**
_temp230=( struct Cyc_Absyn_Structdecl**) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp230[ 0]= udecl2; _temp230;}); struct Cyc_Absyn_StructType_struct* _temp227=({
struct Cyc_Absyn_StructType_struct* _temp228=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp228[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp229; _temp229.tag= Cyc_Absyn_StructType;
_temp229.f1= _temp191; _temp229.f2= _temp189; _temp229.f3= _temp226; _temp229;});
_temp228;}); return( void*) _temp227;}}} _LL178: { struct Cyc_List_List*
_temp232= _temp203->tvs; if( Cyc_List_length( _temp197) != (( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( _temp232)){({ struct Cyc_Std_Int_pa_struct _temp235;
_temp235.tag= Cyc_Std_Int_pa; _temp235.f1=( int)(( unsigned int)(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( _temp232));{ struct Cyc_Std_Int_pa_struct
_temp234; _temp234.tag= Cyc_Std_Int_pa; _temp234.f1=( int)(( unsigned int) Cyc_List_length(
_temp197));{ void* _temp233[ 2u]={& _temp234,& _temp235};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): number of params %d differs from number of tyvars %d",
sizeof( unsigned char), 60u), _tag_arr( _temp233, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp236=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp232, _temp197); struct Cyc_Core_Opt*
fields= 0; if( _temp203->fields !=  0){ fields=({ struct Cyc_Core_Opt* _temp237=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp237->v=(
void*)(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tunionfield*(* f)( struct
Cyc_List_List*, struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_substitute_tunionfield_type,
_temp236,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp203->fields))->v);
_temp237;});}{ struct Cyc_Absyn_Tuniondecl* _temp238=({ struct Cyc_Absyn_Tuniondecl*
_temp247=( struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp247->sc=( void*)(( void*) _temp203->sc); _temp247->name= _temp203->name;
_temp247->tvs= 0; _temp247->fields= fields; _temp247->is_xtunion= _temp203->is_xtunion;
_temp247;}); struct Cyc_Absyn_TunionType_struct* _temp239=({ struct Cyc_Absyn_TunionType_struct*
_temp241=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp241[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp242; _temp242.tag= Cyc_Absyn_TunionType;
_temp242.f1=({ struct Cyc_Absyn_TunionInfo _temp243; _temp243.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp244=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp244[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp245; _temp245.tag= Cyc_Absyn_KnownTunion;
_temp245.f1=({ struct Cyc_Absyn_Tuniondecl** _temp246=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp246[ 0]= _temp238;
_temp246;}); _temp245;}); _temp244;})); _temp243.targs= _temp197; _temp243.rgn=(
void*) _temp195; _temp243;}); _temp242;}); _temp241;}); struct Cyc_Position_Segment*
_temp240= Cyc_Position_segment_of_abs( 0, 0); return( void*) _temp239;}}} _LL180:
return type; _LL172:;} struct _tuple9{ struct Cyc_Dict_Dict* f1; struct Cyc_Tcgenrep_RepInfo*
f2; } ; static struct _tuple9* Cyc_Tcgenrep_lookupRep( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment* loc, void* type);
struct _tuple10{ struct Cyc_Dict_Dict* f1; struct Cyc_List_List* f2; } ; struct
_tuple11{ struct Cyc_Tcenv_Tenv* f1; struct Cyc_Position_Segment* f2; } ; static
struct _tuple10* Cyc_Tcgenrep_lookupRepsCls( struct _tuple11* env, void* type,
struct _tuple10* carry){ struct _tuple9 _temp250; struct Cyc_Tcgenrep_RepInfo*
_temp251; struct Cyc_Dict_Dict* _temp253; struct _tuple9* _temp248= Cyc_Tcgenrep_lookupRep((*
env).f1,(* carry).f1,(* env).f2, type); _temp250=* _temp248; _LL254: _temp253=
_temp250.f1; goto _LL252; _LL252: _temp251= _temp250.f2; goto _LL249; _LL249:
return({ struct _tuple10* _temp255=( struct _tuple10*) _cycalloc( sizeof( struct
_tuple10)); _temp255->f1= _temp253; _temp255->f2=({ struct Cyc_List_List*
_temp256=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp256->hd=( void*) _temp251; _temp256->tl=(* carry).f2; _temp256;}); _temp255;});}
static struct Cyc_Dict_Dict* Cyc_Tcgenrep_buildRepTuple( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment* loc, struct
_tagged_arr* varname, void* type, struct Cyc_List_List* types){ struct Cyc_Absyn_Exp*
_temp257= Cyc_Absyn_sizeoftyp_exp( type, loc); struct _tuple11* _temp258=({
struct _tuple11* _temp284=( struct _tuple11*) _cycalloc( sizeof( struct _tuple11));
_temp284->f1= te; _temp284->f2= loc; _temp284;}); struct _tuple10* _temp259=({
struct _tuple10* _temp283=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10));
_temp283->f1= dict; _temp283->f2= 0; _temp283;}); struct _tuple10 _temp262;
struct Cyc_List_List* _temp263; struct Cyc_Dict_Dict* _temp265; struct _tuple10*
_temp260=(( struct _tuple10*(*)( struct _tuple10*(* f)( struct _tuple11*, void*,
struct _tuple10*), struct _tuple11*, struct Cyc_List_List* x, struct _tuple10*
accum)) Cyc_List_fold_right_c)( Cyc_Tcgenrep_lookupRepsCls, _temp258, types,
_temp259); _temp262=* _temp260; _LL266: _temp265= _temp262.f1; goto _LL264;
_LL264: _temp263= _temp262.f2; goto _LL261; _LL261: dict= _temp265;{ struct Cyc_List_List*
_temp267=(( struct Cyc_List_List*(*)( int n, int(* f)( int))) Cyc_List_tabulate)(
Cyc_List_length( types),( int(*)( int x)) Cyc_Tcgenrep_id); struct Cyc_List_List*
_temp268=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple7*,
int), struct _tuple7* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_make_offsetof_exp,({
struct _tuple7* _temp282=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7));
_temp282->f1= type; _temp282->f2= loc; _temp282;}), _temp267); struct Cyc_List_List*
_temp269=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*,
struct Cyc_Tcgenrep_RepInfo*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcgenrep_get_and_cast_ri_exp, loc, _temp263); struct
Cyc_List_List* _temp270=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct Cyc_Position_Segment*, struct _tuple6*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_tuple2_exp_cls, loc,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_zip)(
_temp268, _temp269)); void* tuple_type= Cyc_Tcgenrep_tuple_typ(({ void* _temp281[
2u]; _temp281[ 1u]= Cyc_Tcgenrep_tunion_typ(& Cyc_Tcgenrep_typestruct_str);
_temp281[ 0u]= Cyc_Absyn_uint_t; Cyc_List_list( _tag_arr( _temp281, sizeof( void*),
2u));})); struct _tuple8 _temp273; struct Cyc_Absyn_Exp* _temp274; struct Cyc_List_List*
_temp276; struct _tuple8* _temp271= Cyc_Tcgenrep_array_decls( tuple_type,
_temp270, loc); _temp273=* _temp271; _LL277: _temp276= _temp273.f1; goto _LL275;
_LL275: _temp274= _temp273.f2; goto _LL272; _LL272: { struct Cyc_Absyn_Decl*
_temp278= Cyc_Tcgenrep_tunion_constructor_decl(& Cyc_Tcgenrep_typestruct_str,&
Cyc_Tcgenrep_tuple_str, varname,({ struct Cyc_Absyn_Exp* _temp280[ 2u]; _temp280[
1u]= _temp274; _temp280[ 0u]= _temp257;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp280, sizeof( struct Cyc_Absyn_Exp*),
2u));}), loc); return Cyc_Tcgenrep_update_info( dict, type,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp276,({ struct Cyc_Absyn_Decl* _temp279[ 1u]; _temp279[ 0u]= _temp278;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp279, sizeof( struct Cyc_Absyn_Decl*), 1u));})),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( varname), loc), loc),
_temp263, 0);}}} static struct Cyc_Dict_Dict* Cyc_Tcgenrep_buildRepUnion( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment* loc,
struct _tagged_arr* varname, void* type, struct Cyc_List_List* types){ struct
_tuple11* _temp285=({ struct _tuple11* _temp306=( struct _tuple11*) _cycalloc(
sizeof( struct _tuple11)); _temp306->f1= te; _temp306->f2= loc; _temp306;});
struct _tuple10* _temp286=({ struct _tuple10* _temp305=( struct _tuple10*)
_cycalloc( sizeof( struct _tuple10)); _temp305->f1= dict; _temp305->f2= 0;
_temp305;}); struct _tuple10 _temp289; struct Cyc_List_List* _temp290; struct
Cyc_Dict_Dict* _temp292; struct _tuple10* _temp287=(( struct _tuple10*(*)(
struct _tuple10*(* f)( struct _tuple11*, void*, struct _tuple10*), struct
_tuple11*, struct Cyc_List_List* x, struct _tuple10* accum)) Cyc_List_fold_right_c)(
Cyc_Tcgenrep_lookupRepsCls, _temp285, types, _temp286); _temp289=* _temp287;
_LL293: _temp292= _temp289.f1; goto _LL291; _LL291: _temp290= _temp289.f2; goto
_LL288; _LL288: dict= _temp292;{ struct Cyc_List_List* _temp294=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*, struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcgenrep_get_and_cast_ri_exp,
loc, _temp290); struct _tuple8 _temp297; struct Cyc_Absyn_Exp* _temp298; struct
Cyc_List_List* _temp300; struct _tuple8* _temp295= Cyc_Tcgenrep_array_decls( Cyc_Tcgenrep_tunion_typ(&
Cyc_Tcgenrep_typestruct_str), _temp294, loc); _temp297=* _temp295; _LL301:
_temp300= _temp297.f1; goto _LL299; _LL299: _temp298= _temp297.f2; goto _LL296;
_LL296: { struct Cyc_Absyn_Decl* _temp302= Cyc_Tcgenrep_tunion_constructor_decl(&
Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_union_str, varname,({ struct Cyc_Absyn_Exp*
_temp304[ 1u]; _temp304[ 0u]= _temp298;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp304, sizeof( struct Cyc_Absyn_Exp*),
1u));}), loc); return Cyc_Tcgenrep_update_info( dict, type,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp300,({ struct Cyc_Absyn_Decl* _temp303[ 1u]; _temp303[ 0u]= _temp302;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp303, sizeof( struct Cyc_Absyn_Decl*), 1u));})),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( varname), loc), loc),
_temp290, 0);}}} static struct Cyc_Dict_Dict* Cyc_Tcgenrep_buildRepTunion(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment*
loc, struct _tagged_arr* varname, void* type, struct Cyc_List_List* types){
struct _tuple11* _temp307=({ struct _tuple11* _temp333=( struct _tuple11*)
_cycalloc( sizeof( struct _tuple11)); _temp333->f1= te; _temp333->f2= loc;
_temp333;}); struct _tuple10* _temp308=({ struct _tuple10* _temp332=( struct
_tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp332->f1= dict; _temp332->f2=
0; _temp332;}); struct _tuple10 _temp311; struct Cyc_List_List* _temp312; struct
Cyc_Dict_Dict* _temp314; struct _tuple10* _temp309=(( struct _tuple10*(*)(
struct _tuple10*(* f)( struct _tuple11*, void*, struct _tuple10*), struct
_tuple11*, struct Cyc_List_List* x, struct _tuple10* accum)) Cyc_List_fold_right_c)(
Cyc_Tcgenrep_lookupRepsCls, _temp307, types, _temp308); _temp311=* _temp309;
_LL315: _temp314= _temp311.f1; goto _LL313; _LL313: _temp312= _temp311.f2; goto
_LL310; _LL310: dict= _temp314;{ struct Cyc_List_List* _temp316=(( struct Cyc_List_List*(*)(
int n, int(* f)( int))) Cyc_List_tabulate)((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp312),( int(*)( int x)) Cyc_Tcgenrep_id); struct Cyc_List_List*
_temp317=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*,
int), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcgenrep_cnst_int_cls, loc, _temp316); struct Cyc_List_List* _temp318=((
struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*,
struct Cyc_Tcgenrep_RepInfo*), struct Cyc_Position_Segment* env, struct Cyc_List_List*
x)) Cyc_List_map_c)( Cyc_Tcgenrep_get_and_cast_ri_exp, loc, _temp312); struct
Cyc_List_List* _temp319=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_zip)( _temp317, _temp318); struct Cyc_List_List*
_temp320=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct Cyc_Position_Segment*,
struct _tuple6*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Tcgenrep_tuple2_exp_cls, loc, _temp319); void* tuple_type=
Cyc_Tcgenrep_tuple_typ(({ void* _temp331[ 2u]; _temp331[ 1u]= Cyc_Tcgenrep_tunion_typ(&
Cyc_Tcgenrep_typestruct_str); _temp331[ 0u]= Cyc_Absyn_uint_t; Cyc_List_list(
_tag_arr( _temp331, sizeof( void*), 2u));})); struct _tuple8 _temp323; struct
Cyc_Absyn_Exp* _temp324; struct Cyc_List_List* _temp326; struct _tuple8*
_temp321= Cyc_Tcgenrep_array_decls( tuple_type, _temp320, loc); _temp323=*
_temp321; _LL327: _temp326= _temp323.f1; goto _LL325; _LL325: _temp324= _temp323.f2;
goto _LL322; _LL322: { struct Cyc_Absyn_Decl* _temp328= Cyc_Tcgenrep_tunion_constructor_decl(&
Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_tunion_str, varname,({ struct Cyc_Absyn_Exp*
_temp330[ 1u]; _temp330[ 0u]= _temp324;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp330, sizeof( struct Cyc_Absyn_Exp*),
1u));}), loc); return Cyc_Tcgenrep_update_info( dict, type,(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
_temp326,({ struct Cyc_Absyn_Decl* _temp329[ 1u]; _temp329[ 0u]= _temp328;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp329, sizeof( struct Cyc_Absyn_Decl*), 1u));})),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( varname), loc), loc),
_temp312, 0); return dict;}}} static struct _tuple9* Cyc_Tcgenrep_lookupRep(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Dict_Dict* dict, struct Cyc_Position_Segment*
loc, void* type){ if( Cyc_Tcgenrep_typestruct_decl ==  0){ Cyc_Tcenv_lookup_tuniondecl(
te, loc,& Cyc_Tcgenrep_typestruct_name);}{ struct Cyc_Core_Opt* info=(( struct
Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, void* k)) Cyc_Dict_lookup_opt)( dict,
type); if( info !=  0){ struct _tuple9* _temp334=({ struct _tuple9* _temp335=(
struct _tuple9*) _cycalloc( sizeof( struct _tuple9)); _temp335->f1= dict;
_temp335->f2=( struct Cyc_Tcgenrep_RepInfo*)(( struct Cyc_Core_Opt*) _check_null(
info))->v; _temp335;}); return _temp334;} else{{ void* _temp336= Cyc_Tcutil_compress(
Cyc_Tcgenrep_monomorphize_type( type)); void* _temp386; struct Cyc_Absyn_PtrInfo
_temp388; struct Cyc_Absyn_Exp* _temp390; struct Cyc_Absyn_Tqual _temp392; void*
_temp394; struct Cyc_List_List* _temp396; struct Cyc_Core_Opt* _temp398; struct
Cyc_List_List* _temp400; struct _tuple1* _temp402; struct Cyc_Absyn_Structdecl**
_temp404; struct Cyc_List_List* _temp406; struct _tuple1* _temp408; struct Cyc_List_List*
_temp410; struct Cyc_Absyn_Tvar* _temp412; struct Cyc_Absyn_FnInfo _temp414;
struct Cyc_Absyn_TunionInfo _temp416; void* _temp418; struct Cyc_List_List*
_temp420; void* _temp422; struct Cyc_Absyn_Tuniondecl** _temp424; struct Cyc_Absyn_Tuniondecl*
_temp426; struct Cyc_Absyn_TunionFieldInfo _temp427; struct Cyc_Absyn_Uniondecl**
_temp429; struct Cyc_List_List* _temp431; struct _tuple1* _temp433; struct Cyc_List_List*
_temp435; _LL338: if(( unsigned int) _temp336 >  4u?*(( int*) _temp336) ==  Cyc_Absyn_IntType:
0){ _LL387: _temp386=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp336)->f2;
goto _LL339;} else{ goto _LL340;} _LL340: if( _temp336 == ( void*) Cyc_Absyn_FloatType){
goto _LL341;} else{ goto _LL342;} _LL342: if( _temp336 == ( void*) Cyc_Absyn_DoubleType){
goto _LL343;} else{ goto _LL344;} _LL344: if(( unsigned int) _temp336 >  4u?*((
int*) _temp336) ==  Cyc_Absyn_PointerType: 0){ _LL389: _temp388=(( struct Cyc_Absyn_PointerType_struct*)
_temp336)->f1; goto _LL345;} else{ goto _LL346;} _LL346: if(( unsigned int)
_temp336 >  4u?*(( int*) _temp336) ==  Cyc_Absyn_ArrayType: 0){ _LL395: _temp394=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp336)->f1; goto _LL393; _LL393:
_temp392=(( struct Cyc_Absyn_ArrayType_struct*) _temp336)->f2; goto _LL391;
_LL391: _temp390=(( struct Cyc_Absyn_ArrayType_struct*) _temp336)->f3; goto
_LL347;} else{ goto _LL348;} _LL348: if(( unsigned int) _temp336 >  4u?*(( int*)
_temp336) ==  Cyc_Absyn_TupleType: 0){ _LL397: _temp396=(( struct Cyc_Absyn_TupleType_struct*)
_temp336)->f1; goto _LL349;} else{ goto _LL350;} _LL350: if(( unsigned int)
_temp336 >  4u?*(( int*) _temp336) ==  Cyc_Absyn_TypedefType: 0){ _LL403:
_temp402=(( struct Cyc_Absyn_TypedefType_struct*) _temp336)->f1; goto _LL401;
_LL401: _temp400=(( struct Cyc_Absyn_TypedefType_struct*) _temp336)->f2; goto
_LL399; _LL399: _temp398=(( struct Cyc_Absyn_TypedefType_struct*) _temp336)->f3;
goto _LL351;} else{ goto _LL352;} _LL352: if(( unsigned int) _temp336 >  4u?*((
int*) _temp336) ==  Cyc_Absyn_StructType: 0){ _LL409: _temp408=(( struct Cyc_Absyn_StructType_struct*)
_temp336)->f1; goto _LL407; _LL407: _temp406=(( struct Cyc_Absyn_StructType_struct*)
_temp336)->f2; goto _LL405; _LL405: _temp404=(( struct Cyc_Absyn_StructType_struct*)
_temp336)->f3; goto _LL353;} else{ goto _LL354;} _LL354: if(( unsigned int)
_temp336 >  4u?*(( int*) _temp336) ==  Cyc_Absyn_AnonStructType: 0){ _LL411:
_temp410=(( struct Cyc_Absyn_AnonStructType_struct*) _temp336)->f1; goto _LL355;}
else{ goto _LL356;} _LL356: if( _temp336 == ( void*) Cyc_Absyn_VoidType){ goto
_LL357;} else{ goto _LL358;} _LL358: if(( unsigned int) _temp336 >  4u?*(( int*)
_temp336) ==  Cyc_Absyn_Evar: 0){ goto _LL359;} else{ goto _LL360;} _LL360: if((
unsigned int) _temp336 >  4u?*(( int*) _temp336) ==  Cyc_Absyn_VarType: 0){
_LL413: _temp412=(( struct Cyc_Absyn_VarType_struct*) _temp336)->f1; goto _LL361;}
else{ goto _LL362;} _LL362: if(( unsigned int) _temp336 >  4u?*(( int*) _temp336)
==  Cyc_Absyn_FnType: 0){ _LL415: _temp414=(( struct Cyc_Absyn_FnType_struct*)
_temp336)->f1; goto _LL363;} else{ goto _LL364;} _LL364: if(( unsigned int)
_temp336 >  4u?*(( int*) _temp336) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL365;}
else{ goto _LL366;} _LL366: if( _temp336 == ( void*) Cyc_Absyn_HeapRgn){ goto
_LL367;} else{ goto _LL368;} _LL368: if(( unsigned int) _temp336 >  4u?*(( int*)
_temp336) ==  Cyc_Absyn_AccessEff: 0){ goto _LL369;} else{ goto _LL370;} _LL370:
if(( unsigned int) _temp336 >  4u?*(( int*) _temp336) ==  Cyc_Absyn_JoinEff: 0){
goto _LL371;} else{ goto _LL372;} _LL372: if(( unsigned int) _temp336 >  4u?*((
int*) _temp336) ==  Cyc_Absyn_RgnsEff: 0){ goto _LL373;} else{ goto _LL374;}
_LL374: if(( unsigned int) _temp336 >  4u?*(( int*) _temp336) ==  Cyc_Absyn_TunionType:
0){ _LL417: _temp416=(( struct Cyc_Absyn_TunionType_struct*) _temp336)->f1;
_LL423: _temp422=( void*) _temp416.tunion_info; if(*(( int*) _temp422) ==  Cyc_Absyn_KnownTunion){
_LL425: _temp424=(( struct Cyc_Absyn_KnownTunion_struct*) _temp422)->f1;
_temp426=* _temp424; goto _LL421;} else{ goto _LL376;} _LL421: _temp420=
_temp416.targs; goto _LL419; _LL419: _temp418=( void*) _temp416.rgn; goto _LL375;}
else{ goto _LL376;} _LL376: if(( unsigned int) _temp336 >  4u?*(( int*) _temp336)
==  Cyc_Absyn_TunionType: 0){ goto _LL377;} else{ goto _LL378;} _LL378: if((
unsigned int) _temp336 >  4u?*(( int*) _temp336) ==  Cyc_Absyn_TunionFieldType:
0){ _LL428: _temp427=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp336)->f1;
goto _LL379;} else{ goto _LL380;} _LL380: if(( unsigned int) _temp336 >  4u?*((
int*) _temp336) ==  Cyc_Absyn_UnionType: 0){ _LL434: _temp433=(( struct Cyc_Absyn_UnionType_struct*)
_temp336)->f1; goto _LL432; _LL432: _temp431=(( struct Cyc_Absyn_UnionType_struct*)
_temp336)->f2; goto _LL430; _LL430: _temp429=(( struct Cyc_Absyn_UnionType_struct*)
_temp336)->f3; goto _LL381;} else{ goto _LL382;} _LL382: if(( unsigned int)
_temp336 >  4u?*(( int*) _temp336) ==  Cyc_Absyn_AnonUnionType: 0){ _LL436:
_temp435=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp336)->f1; goto _LL383;}
else{ goto _LL384;} _LL384: if(( unsigned int) _temp336 >  4u?*(( int*) _temp336)
==  Cyc_Absyn_EnumType: 0){ goto _LL385;} else{ goto _LL337;} _LL339: { struct
_tagged_arr* _temp437= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof(
unsigned char), 4u)); struct Cyc_Absyn_Decl* _temp438= Cyc_Tcgenrep_tunion_constructor_decl(&
Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_int_str, _temp437,({ struct Cyc_Absyn_Exp*
_temp440[ 1u]; _temp440[ 0u]= Cyc_Tcgenrep_cnst_int( Cyc_Tcgenrep_size_of2int(
_temp386), loc);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp440, sizeof( struct Cyc_Absyn_Exp*), 1u));}), loc); dict= Cyc_Tcgenrep_update_info(
dict, type,({ struct Cyc_Absyn_Decl* _temp439[ 1u]; _temp439[ 0u]= _temp438;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp439, sizeof( struct Cyc_Absyn_Decl*), 1u));}),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( _temp437), loc), loc), 0, 0);
goto _LL337;} _LL341: dict= Cyc_Tcgenrep_update_info( dict, type, 0,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name(& Cyc_Tcgenrep_float_str),
loc), 0, 0); goto _LL337; _LL343: dict= Cyc_Tcgenrep_update_info( dict, type, 0,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name(& Cyc_Tcgenrep_double_str),
loc), 0, 0); goto _LL337; _LL345:{ void* _temp441= Cyc_Absyn_conref_val(
_temp388.bounds); struct Cyc_Absyn_Exp* _temp447; _LL443: if(( unsigned int)
_temp441 >  1u?*(( int*) _temp441) ==  Cyc_Absyn_Upper_b: 0){ _LL448: _temp447=((
struct Cyc_Absyn_Upper_b_struct*) _temp441)->f1; goto _LL444;} else{ goto _LL445;}
_LL445: if( _temp441 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL446;} else{ goto
_LL442;} _LL444: { struct _tagged_arr* _temp449= Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep", sizeof( unsigned char), 4u)); struct Cyc_Core_Opt* _temp450=({
struct Cyc_Core_Opt* _temp462=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp462->v=( void*) Cyc_Tcgenrep_gen_vardecl_sc( _temp449, Cyc_Tcgenrep_tunionfield_typ(&
Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_thinptr_str), 0,( void*) Cyc_Absyn_Extern,
loc); _temp462;}); dict= Cyc_Tcgenrep_update_info( dict, type, 0,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name(
_temp449), loc), loc), 0, _temp450);{ struct _tuple9 _temp453; struct Cyc_Tcgenrep_RepInfo*
_temp454; struct Cyc_Dict_Dict* _temp456; struct _tuple9* _temp451= Cyc_Tcgenrep_lookupRep(
te, dict, loc,( void*) _temp388.elt_typ); _temp453=* _temp451; _LL457: _temp456=
_temp453.f1; goto _LL455; _LL455: _temp454= _temp453.f2; goto _LL452; _LL452:
dict= _temp456;{ unsigned int bound= Cyc_Evexp_eval_const_uint_exp( _temp447);
struct Cyc_Absyn_Decl* _temp458= Cyc_Tcgenrep_tunion_constructor_decl(& Cyc_Tcgenrep_typestruct_str,&
Cyc_Tcgenrep_thinptr_str, _temp449,({ struct Cyc_Absyn_Exp* _temp461[ 2u];
_temp461[ 1u]=( struct Cyc_Absyn_Exp*) _check_null( _temp454->exp); _temp461[ 0u]=
Cyc_Tcgenrep_cnst_int(( int) bound, loc);(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp461, sizeof( struct Cyc_Absyn_Exp*),
2u));}), loc); dict= Cyc_Tcgenrep_update_info( dict, type,({ struct Cyc_Absyn_Decl*
_temp459[ 1u]; _temp459[ 0u]= _temp458;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp459, sizeof( struct Cyc_Absyn_Decl*),
1u));}),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp(
Cyc_Tcgenrep_toplevel_name( _temp449), loc), loc),({ struct Cyc_Tcgenrep_RepInfo*
_temp460[ 1u]; _temp460[ 0u]= _temp454;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp460, sizeof( struct Cyc_Tcgenrep_RepInfo*),
1u));}), 0); goto _LL442;}}} _LL446: { struct _tagged_arr* _temp463= Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep", sizeof( unsigned char), 4u)); struct Cyc_Core_Opt* _temp464=({
struct Cyc_Core_Opt* _temp476=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp476->v=( void*) Cyc_Tcgenrep_gen_vardecl_sc( _temp463, Cyc_Tcgenrep_tunionfield_typ(&
Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_fatptr_str), 0,( void*) Cyc_Absyn_Extern,
loc); _temp476;}); dict= Cyc_Tcgenrep_update_info( dict, type, 0,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name(
_temp463), loc), loc), 0, _temp464);{ struct _tuple9 _temp467; struct Cyc_Tcgenrep_RepInfo*
_temp468; struct Cyc_Dict_Dict* _temp470; struct _tuple9* _temp465= Cyc_Tcgenrep_lookupRep(
te, dict, loc,( void*) _temp388.elt_typ); _temp467=* _temp465; _LL471: _temp470=
_temp467.f1; goto _LL469; _LL469: _temp468= _temp467.f2; goto _LL466; _LL466:
dict= _temp470;{ struct Cyc_Absyn_Decl* _temp472= Cyc_Tcgenrep_tunion_constructor_decl(&
Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_fatptr_str, _temp463,({ struct Cyc_Absyn_Exp*
_temp475[ 1u]; _temp475[ 0u]=( struct Cyc_Absyn_Exp*) _check_null( _temp468->exp);((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp475, sizeof( struct Cyc_Absyn_Exp*), 1u));}), loc); dict= Cyc_Tcgenrep_update_info(
dict, type,({ struct Cyc_Absyn_Decl* _temp473[ 1u]; _temp473[ 0u]= _temp472;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp473, sizeof( struct Cyc_Absyn_Decl*), 1u));}),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( _temp463), loc), loc),({
struct Cyc_Tcgenrep_RepInfo* _temp474[ 1u]; _temp474[ 0u]= _temp468;(( struct
Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp474,
sizeof( struct Cyc_Tcgenrep_RepInfo*), 1u));}), 0); goto _LL442;}}} _LL442:;}
goto _LL337; _LL347: if( _temp390 ==  0){({ void* _temp477[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("tcgenrep:At this point, array bounds must be constants",
sizeof( unsigned char), 55u), _tag_arr( _temp477, sizeof( void*), 0u));});}{
struct _tuple9 _temp480; struct Cyc_Tcgenrep_RepInfo* _temp481; struct Cyc_Dict_Dict*
_temp483; struct _tuple9* _temp478= Cyc_Tcgenrep_lookupRep( te, dict, loc,
_temp394); _temp480=* _temp478; _LL484: _temp483= _temp480.f1; goto _LL482;
_LL482: _temp481= _temp480.f2; goto _LL479; _LL479: dict= _temp483;{
unsigned int bound= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp390)); struct _tagged_arr* _temp485= Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep", sizeof( unsigned char), 4u)); struct Cyc_Absyn_Decl* _temp486=
Cyc_Tcgenrep_tunion_constructor_decl(& Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_thinptr_str,
_temp485,({ struct Cyc_Absyn_Exp* _temp489[ 2u]; _temp489[ 1u]=( struct Cyc_Absyn_Exp*)
_check_null( _temp481->exp); _temp489[ 0u]= Cyc_Tcgenrep_cnst_int(( int) bound,
loc);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp489, sizeof( struct Cyc_Absyn_Exp*), 2u));}), loc); dict= Cyc_Tcgenrep_update_info(
dict, type,({ struct Cyc_Absyn_Decl* _temp487[ 1u]; _temp487[ 0u]= _temp486;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp487, sizeof( struct Cyc_Absyn_Decl*), 1u));}),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( _temp485), loc), loc),({
struct Cyc_Tcgenrep_RepInfo* _temp488[ 1u]; _temp488[ 0u]= _temp481;(( struct
Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp488,
sizeof( struct Cyc_Tcgenrep_RepInfo*), 1u));}), 0); goto _LL337;}} _LL349: {
struct _tagged_arr* _temp490= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof(
unsigned char), 4u)); struct Cyc_Core_Opt* _temp491=({ struct Cyc_Core_Opt*
_temp493=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp493->v=( void*) Cyc_Tcgenrep_gen_vardecl_sc( _temp490, Cyc_Tcgenrep_tunionfield_typ(&
Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_tuple_str), 0,( void*) Cyc_Absyn_Extern,
loc); _temp493;}); dict= Cyc_Tcgenrep_update_info( dict, type, 0,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name(
_temp490), loc), loc), 0, _temp491);{ struct Cyc_List_List* _temp492=(( struct
Cyc_List_List*(*)( void*(* f)( struct _tuple4*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Tcgenrep_get_second, _temp396); dict= Cyc_Tcgenrep_buildRepTuple( te, dict,
loc, _temp490, type, _temp492); goto _LL337;}} _LL351: if( _temp398 ==  0){({
void* _temp494[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("gen(): typedef definition must be present by now",
sizeof( unsigned char), 49u), _tag_arr( _temp494, sizeof( void*), 0u));});}{
struct _tuple9 _temp497; struct Cyc_Tcgenrep_RepInfo* _temp498; struct Cyc_Dict_Dict*
_temp500; struct _tuple9* _temp495= Cyc_Tcgenrep_lookupRep( te, dict, loc,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp398))->v); _temp497=* _temp495; _LL501:
_temp500= _temp497.f1; goto _LL499; _LL499: _temp498= _temp497.f2; goto _LL496;
_LL496: dict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* k,
struct Cyc_Tcgenrep_RepInfo* v)) Cyc_Dict_insert)( _temp500, type, _temp498);
goto _LL337;} _LL353: if( _temp404 ==  0){({ void* _temp502[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): struct declaration must be present by now",
sizeof( unsigned char), 49u), _tag_arr( _temp502, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Structdecl* _temp503=*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp404)); if( _temp503->fields ==  0){ if( _temp408 ==  0){({
void* _temp504[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("struct must have name by now", sizeof(
unsigned char), 29u), _tag_arr( _temp504, sizeof( void*), 0u));});}{ struct
_tagged_arr s=( struct _tagged_arr) Cyc_Std_strconcat( Cyc_Absynpp_qvar2cstring((
struct _tuple1*) _check_null( _temp408)), _tag_arr("_struct_typerep", sizeof(
unsigned char), 16u)); struct _tagged_arr* _temp505=({ struct _tagged_arr*
_temp508=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp508[ 0]= s; _temp508;}); struct Cyc_Core_Opt* _temp506=({ struct Cyc_Core_Opt*
_temp507=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp507->v=( void*) Cyc_Tcgenrep_gen_vardecl_sc( _temp505, Cyc_Tcgenrep_tunionfield_typ(&
Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_tuple_str), 0,( void*) Cyc_Absyn_Extern,
loc); _temp507;}); dict= Cyc_Tcgenrep_update_info( dict, type, 0,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name(
_temp505), loc), loc), 0, _temp506); goto _LL337;}} if(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp503->fields))->v !=  0?(( int(*)( int(*
pred)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_forall)(
Cyc_Tcgenrep_has_bitfield,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp503->fields))->v): 0){ int _temp509=(( int(*)( int(* f)( int,
struct Cyc_Absyn_Structfield*), int accum, struct Cyc_List_List* x)) Cyc_List_fold_left)(
Cyc_Tcgenrep_add_bitfield_sizes, 0,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp503->fields))->v); int _temp510= _temp509 /  8 + ( _temp509 % 
8 ==  0? 0: 1); void* base_type=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp518=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp518[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp519; _temp519.tag= Cyc_Absyn_ArrayType;
_temp519.f1=( void*) Cyc_Absyn_uchar_t; _temp519.f2= Cyc_Tcgenrep_tq_none;
_temp519.f3=( struct Cyc_Absyn_Exp*) Cyc_Tcgenrep_cnst_int( _temp510, loc);
_temp519;}); _temp518;}); struct _tuple9 _temp513; struct Cyc_Tcgenrep_RepInfo*
_temp514; struct Cyc_Dict_Dict* _temp516; struct _tuple9* _temp511= Cyc_Tcgenrep_lookupRep(
te, dict, loc, base_type); _temp513=* _temp511; _LL517: _temp516= _temp513.f1;
goto _LL515; _LL515: _temp514= _temp513.f2; goto _LL512; _LL512: dict=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* k, struct Cyc_Tcgenrep_RepInfo*
v)) Cyc_Dict_insert)( _temp516, type, _temp514);} else{ struct _tagged_arr*
_temp520= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof( unsigned char), 4u));
struct Cyc_Core_Opt* _temp521=({ struct Cyc_Core_Opt* _temp523=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp523->v=( void*) Cyc_Tcgenrep_gen_vardecl_sc(
_temp520, Cyc_Tcgenrep_tunionfield_typ(& Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_tuple_str),
0,( void*) Cyc_Absyn_Extern, loc); _temp523;}); dict= Cyc_Tcgenrep_update_info(
dict, type, 0,( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp(
Cyc_Tcgenrep_toplevel_name( _temp520), loc), loc), 0, _temp521);{ struct Cyc_List_List*
_temp522=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_select_structfield_type,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp503->fields))->v);
dict= Cyc_Tcgenrep_buildRepTuple( te, dict, loc, _temp520, type, _temp522);}}
goto _LL337;} _LL355: if( _temp410 !=  0?(( int(*)( int(* pred)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_forall)( Cyc_Tcgenrep_has_bitfield, _temp410):
0){ int _temp524=(( int(*)( int(* f)( int, struct Cyc_Absyn_Structfield*), int
accum, struct Cyc_List_List* x)) Cyc_List_fold_left)( Cyc_Tcgenrep_add_bitfield_sizes,
0, _temp410); int _temp525= _temp524 /  8 + ( _temp524 %  8 ==  0? 0: 1); void*
base_type=( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp533=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp533[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp534; _temp534.tag= Cyc_Absyn_ArrayType; _temp534.f1=(
void*) Cyc_Absyn_uchar_t; _temp534.f2= Cyc_Tcgenrep_tq_none; _temp534.f3=(
struct Cyc_Absyn_Exp*) Cyc_Tcgenrep_cnst_int( _temp525, loc); _temp534;});
_temp533;}); struct _tuple9 _temp528; struct Cyc_Tcgenrep_RepInfo* _temp529;
struct Cyc_Dict_Dict* _temp531; struct _tuple9* _temp526= Cyc_Tcgenrep_lookupRep(
te, dict, loc, base_type); _temp528=* _temp526; _LL532: _temp531= _temp528.f1;
goto _LL530; _LL530: _temp529= _temp528.f2; goto _LL527; _LL527: dict=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, void* k, struct Cyc_Tcgenrep_RepInfo*
v)) Cyc_Dict_insert)( _temp531, type, _temp529);} else{ struct Cyc_List_List*
_temp535=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_select_structfield_type,
_temp410); struct _tagged_arr* _temp536= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep",
sizeof( unsigned char), 4u)); dict= Cyc_Tcgenrep_buildRepTuple( te, dict, loc,
_temp536, type, _temp535);} goto _LL337; _LL357:({ void* _temp537[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("found void in gen() expression", sizeof( unsigned char), 31u),
_tag_arr( _temp537, sizeof( void*), 0u));}); goto _LL337; _LL359:({ void*
_temp538[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("found evar in gen() expression",
sizeof( unsigned char), 31u), _tag_arr( _temp538, sizeof( void*), 0u));}); goto
_LL337; _LL361:({ void* _temp539[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("found tyvar in gen() expression",
sizeof( unsigned char), 32u), _tag_arr( _temp539, sizeof( void*), 0u));}); goto
_LL337;({ void* _temp540[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("found variable type in gen() expression",
sizeof( unsigned char), 40u), _tag_arr( _temp540, sizeof( void*), 0u));}); goto
_LL337; _LL363:({ void* _temp541[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("found function type in gen() expression",
sizeof( unsigned char), 40u), _tag_arr( _temp541, sizeof( void*), 0u));}); goto
_LL337; _LL365: goto _LL367; _LL367: goto _LL369; _LL369: goto _LL371; _LL371:
goto _LL373; _LL373:({ void* _temp542[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("gen(): unhandled region, handle or effect type",
sizeof( unsigned char), 47u), _tag_arr( _temp542, sizeof( void*), 0u));}); goto
_LL337; _LL375: if( _temp426->tvs !=  0){({ void* _temp543[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunion type variables not handled yet",
sizeof( unsigned char), 45u), _tag_arr( _temp543, sizeof( void*), 0u));});} if(
_temp426->fields ==  0){({ void* _temp544[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunion fields must be present by now",
sizeof( unsigned char), 44u), _tag_arr( _temp544, sizeof( void*), 0u));});}{
struct _tagged_arr* _temp545= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof(
unsigned char), 4u)); struct Cyc_Core_Opt* _temp546=({ struct Cyc_Core_Opt*
_temp550=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp550->v=( void*) Cyc_Tcgenrep_gen_vardecl_sc( _temp545, Cyc_Tcgenrep_tunionfield_typ(&
Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_tunion_str), 0,( void*) Cyc_Absyn_Extern,
loc); _temp550;}); dict= Cyc_Tcgenrep_update_info( dict, type, 0,( struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp( Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name(
_temp545), loc), loc), 0, _temp546);{ struct Cyc_List_List* _temp547=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp426->fields))->v;
struct Cyc_List_List* _temp548=(( struct Cyc_List_List*(*)( int(* f)( struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List* x)) Cyc_List_filter)( Cyc_Tcgenrep_filter_empty_tunionfield,
_temp547); struct Cyc_List_List* _temp549=(( struct Cyc_List_List*(*)( void*(* f)(
struct Cyc_Absyn_Tunionfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_check_tunionfield_and_get_type,
_temp548); dict= Cyc_Tcgenrep_buildRepTunion( te, dict, loc, _temp545, type,
_temp549); goto _LL337;}} _LL377:({ void* _temp551[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunion must be resolved by now",
sizeof( unsigned char), 38u), _tag_arr( _temp551, sizeof( void*), 0u));}); goto
_LL337; _LL379: if( _temp427.targs !=  0){({ void* _temp552[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunionfield type parameters not handled yet",
sizeof( unsigned char), 51u), _tag_arr( _temp552, sizeof( void*), 0u));});}{
void* _temp553=( void*) _temp427.field_info; struct Cyc_Absyn_Tunionfield*
_temp559; struct Cyc_Absyn_Tuniondecl* _temp561; _LL555: if(*(( int*) _temp553)
==  Cyc_Absyn_KnownTunionfield){ _LL562: _temp561=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp553)->f1; goto _LL560; _LL560: _temp559=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp553)->f2; goto _LL556;} else{ goto _LL557;} _LL557: goto _LL558; _LL556:
if( _temp559->tvs !=  0){({ void* _temp563[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunionfield type parameters not handled yet",
sizeof( unsigned char), 51u), _tag_arr( _temp563, sizeof( void*), 0u));});}{
struct Cyc_List_List* _temp564=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple4*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_get_second,
_temp559->typs); struct Cyc_List_List* _temp565=({ struct Cyc_List_List*
_temp567=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp567->hd=( void*) Cyc_Absyn_uint_t; _temp567->tl= _temp564; _temp567;});
struct _tagged_arr* _temp566= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof(
unsigned char), 4u)); dict= Cyc_Tcgenrep_buildRepTuple( te, dict, loc, _temp566,
type, _temp565); goto _LL554;} _LL558:({ void* _temp568[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): tunionfield must be known at this point",
sizeof( unsigned char), 47u), _tag_arr( _temp568, sizeof( void*), 0u));}); goto
_LL554; _LL554:;} goto _LL337; _LL381: if( _temp429 ==  0){({ void* _temp569[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("gen(): union declaration must be present by now", sizeof(
unsigned char), 48u), _tag_arr( _temp569, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Uniondecl*
_temp570=*(( struct Cyc_Absyn_Uniondecl**) _check_null( _temp429)); if( _temp570->tvs
!=  0){({ void* _temp571[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen: unions with parameters not yet supported",
sizeof( unsigned char), 46u), _tag_arr( _temp571, sizeof( void*), 0u));});} if(
_temp570->fields ==  0){({ void* _temp572[ 0u]={};(( int(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("gen(): union fields must be present by now",
sizeof( unsigned char), 43u), _tag_arr( _temp572, sizeof( void*), 0u));});}{
struct Cyc_List_List* _temp573=(( struct Cyc_List_List*(*)( void*(* f)( struct
Cyc_Absyn_Structfield*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcgenrep_select_structfield_type,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp570->fields))->v);
struct _tagged_arr* _temp574= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof(
unsigned char), 4u)); dict= Cyc_Tcgenrep_buildRepUnion( te, dict, loc, _temp574,
type, _temp573); goto _LL337;}} _LL383: { struct Cyc_List_List* _temp575=((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Structfield*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcgenrep_select_structfield_type, _temp435); struct
_tagged_arr* _temp576= Cyc_Tcgenrep_new_gen_id( _tag_arr("rep", sizeof(
unsigned char), 4u)); dict= Cyc_Tcgenrep_buildRepUnion( te, dict, loc, _temp576,
type, _temp575); goto _LL337;} _LL385: { struct _tagged_arr* _temp577= Cyc_Tcgenrep_new_gen_id(
_tag_arr("rep", sizeof( unsigned char), 4u)); struct Cyc_Absyn_Decl* _temp578=
Cyc_Tcgenrep_tunion_constructor_decl(& Cyc_Tcgenrep_typestruct_str,& Cyc_Tcgenrep_int_str,
_temp577,({ struct Cyc_Absyn_Exp* _temp580[ 1u]; _temp580[ 0u]= Cyc_Tcgenrep_cnst_int(
32, loc);(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp580, sizeof( struct Cyc_Absyn_Exp*), 1u));}), loc); dict= Cyc_Tcgenrep_update_info(
dict, type,({ struct Cyc_Absyn_Decl* _temp579[ 1u]; _temp579[ 0u]= _temp578;((
struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)( _tag_arr(
_temp579, sizeof( struct Cyc_Absyn_Decl*), 1u));}),( struct Cyc_Absyn_Exp*) Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp( Cyc_Tcgenrep_toplevel_name( _temp577), loc), loc), 0, 0);
goto _LL337;} _LL337:;} return({ struct _tuple9* _temp581=( struct _tuple9*)
_cycalloc( sizeof( struct _tuple9)); _temp581->f1= dict; _temp581->f2=(( struct
Cyc_Tcgenrep_RepInfo*(*)( struct Cyc_Dict_Dict* d, void* k)) Cyc_Dict_lookup)(
dict, type); _temp581;});}}} static int Cyc_Tcgenrep_not_emitted_filter( struct
Cyc_Tcgenrep_RepInfo* ri){ return ri->emitted ==  0;} static void Cyc_Tcgenrep_mark_emitted(
struct Cyc_Tcgenrep_RepInfo* ri){ ri->emitted= 1;} struct _tuple12{ struct Cyc_Dict_Dict*
f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Exp* f3; } ; struct _tuple12 Cyc_Tcgenrep_tcGenrep(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, void* type, struct Cyc_Dict_Dict* dict){ Cyc_Tcutil_check_valid_toplevel_type(
loc, te, type); Cyc_Tcutil_check_valid_toplevel_type( loc, te, type);{ struct
_tuple9 _temp584; struct Cyc_Tcgenrep_RepInfo* _temp585; struct Cyc_Dict_Dict*
_temp587; struct _tuple9* _temp582= Cyc_Tcgenrep_lookupRep( te, dict, loc, type);
_temp584=* _temp582; _LL588: _temp587= _temp584.f1; goto _LL586; _LL586:
_temp585= _temp584.f2; goto _LL583; _LL583: { struct Cyc_List_List* _temp589=
Cyc_Tcgenrep_dfs( _temp585);({ struct Cyc_Std_String_pa_struct _temp591;
_temp591.tag= Cyc_Std_String_pa; _temp591.f1=( struct _tagged_arr) Cyc_Absynpp_decllist2string(
_temp589);{ void* _temp590[ 1u]={& _temp591}; Cyc_Std_printf( _tag_arr("Emitting...\n%s\n",
sizeof( unsigned char), 16u), _tag_arr( _temp590, sizeof( void*), 1u));}});
return({ struct _tuple12 _temp592; _temp592.f1= _temp587; _temp592.f2= _temp589;
_temp592.f3=( struct Cyc_Absyn_Exp*) _check_null( _temp585->exp); _temp592;});}}}
