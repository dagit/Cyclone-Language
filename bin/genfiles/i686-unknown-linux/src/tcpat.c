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
void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_rmap( struct _RegionHandle*, void*(* f)(
void*), struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*, void*),
void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x);
extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_rappend( struct _RegionHandle*, struct
Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u];
extern struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r1, struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_tabulate_c( int n, void*(* f)( void*, int), void*
env); struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; struct Cyc_Set_Set; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* cmp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_cardinality( struct Cyc_Set_Set*
s); extern int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern
unsigned char Cyc_Set_Absent[ 11u]; extern int Cyc_Std_strcmp( struct
_tagged_arr s1, struct _tagged_arr s2); extern int Cyc_Std_zstrcmp( struct
_tagged_arr, struct _tagged_arr); extern int Cyc_Std_zstrptrcmp( struct
_tagged_arr*, struct _tagged_arr*); struct Cyc_Lineno_Pos{ struct _tagged_arr
logical_file; struct _tagged_arr line; int line_no; int col; } ; extern
unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static const
int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static const
int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr source;
struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; } ;
extern unsigned char Cyc_Position_Nocontext[ 14u]; static const int Cyc_Absyn_Loc_n=
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt*
tenv); extern void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t;
extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple1* x, void*
t, struct Cyc_Absyn_Exp* init); struct Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[
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
struct Cyc_List_List* curr_namespace; } ; extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*); extern struct _tagged_arr Cyc_Absynpp_kind2string( void*); extern struct
_tagged_arr Cyc_Absynpp_qvar2string( struct _tuple1*); static const int Cyc_Tcenv_VarRes=
0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes= 3; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; static const
int Cyc_Tcenv_AnonEnumRes= 4; struct Cyc_Tcenv_AnonEnumRes_struct{ int tag; void*
f1; struct Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set*
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
struct _tuple1*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*);
extern void Cyc_Tcenv_check_rgn_accessible( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
void* rgn); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_compress( void* t); extern
int Cyc_Tcutil_coerceable( void*); extern struct Cyc_Core_Opt Cyc_Tcutil_rk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ak; extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_rsubstitute(
struct _RegionHandle*, struct Cyc_List_List*, void*); struct _tuple4{ struct Cyc_List_List*
f1; struct _RegionHandle* f2; } ; struct _tuple5{ struct Cyc_Absyn_Tvar* f1;
void* f2; } ; extern struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple4*,
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_unique_vars( struct Cyc_List_List*
vs, struct Cyc_Position_Segment* loc, struct _tagged_arr err_msg); extern struct
Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct _RegionHandle* rgn,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_List_List*
fields); struct _tuple6{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ;
extern struct _tuple6 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); struct _tuple7{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Pat* f2; } ; static void Cyc_Tcpat_resolve_pat( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p){ void* _temp0=( void*) p->r; struct _tuple1*
_temp10; struct Cyc_List_List* _temp12; struct Cyc_List_List* _temp14; struct
_tuple1* _temp16; struct Cyc_List_List* _temp18; struct Cyc_List_List* _temp20;
struct _tuple1* _temp22; _LL2: if(( unsigned int) _temp0 >  2u?*(( int*) _temp0)
==  Cyc_Absyn_UnknownId_p: 0){ _LL11: _temp10=(( struct Cyc_Absyn_UnknownId_p_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: if(( unsigned int) _temp0 >  2u?*((
int*) _temp0) ==  Cyc_Absyn_UnknownCall_p: 0){ _LL17: _temp16=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f1; goto _LL15; _LL15: _temp14=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f2; goto _LL13; _LL13: _temp12=(( struct Cyc_Absyn_UnknownCall_p_struct*)
_temp0)->f3; goto _LL5;} else{ goto _LL6;} _LL6: if(( unsigned int) _temp0 >  2u?*((
int*) _temp0) ==  Cyc_Absyn_UnknownFields_p: 0){ _LL23: _temp22=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f1; goto _LL21; _LL21: _temp20=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f2; goto _LL19; _LL19: _temp18=(( struct Cyc_Absyn_UnknownFields_p_struct*)
_temp0)->f3; goto _LL7;} else{ goto _LL8;} _LL8: goto _LL9; _LL3:{ struct
_handler_cons _temp24; _push_handler(& _temp24);{ int _temp26= 0; if( setjmp(
_temp24.handler)){ _temp26= 1;} if( ! _temp26){{ void* _temp27= Cyc_Tcenv_lookup_ordinary(
te, p->loc, _temp10); struct Cyc_Absyn_Tunionfield* _temp39; struct Cyc_Absyn_Tuniondecl*
_temp41; struct Cyc_Absyn_Enumfield* _temp43; struct Cyc_Absyn_Enumdecl* _temp45;
struct Cyc_Absyn_Enumfield* _temp47; void* _temp49; _LL29: if(*(( int*) _temp27)
==  Cyc_Tcenv_StructRes){ goto _LL30;} else{ goto _LL31;} _LL31: if(*(( int*)
_temp27) ==  Cyc_Tcenv_TunionRes){ _LL42: _temp41=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp27)->f1; goto _LL40; _LL40: _temp39=(( struct Cyc_Tcenv_TunionRes_struct*)
_temp27)->f2; goto _LL32;} else{ goto _LL33;} _LL33: if(*(( int*) _temp27) == 
Cyc_Tcenv_EnumRes){ _LL46: _temp45=(( struct Cyc_Tcenv_EnumRes_struct*) _temp27)->f1;
goto _LL44; _LL44: _temp43=(( struct Cyc_Tcenv_EnumRes_struct*) _temp27)->f2;
goto _LL34;} else{ goto _LL35;} _LL35: if(*(( int*) _temp27) ==  Cyc_Tcenv_AnonEnumRes){
_LL50: _temp49=( void*)(( struct Cyc_Tcenv_AnonEnumRes_struct*) _temp27)->f1;
goto _LL48; _LL48: _temp47=(( struct Cyc_Tcenv_AnonEnumRes_struct*) _temp27)->f2;
goto _LL36;} else{ goto _LL37;} _LL37: if(*(( int*) _temp27) ==  Cyc_Tcenv_VarRes){
goto _LL38;} else{ goto _LL28;} _LL30:({ void* _temp51[ 0u]={}; Cyc_Tcutil_terr(
p->loc, _tag_arr("struct tag used without arguments in pattern", sizeof(
unsigned char), 45u), _tag_arr( _temp51, sizeof( void*), 0u));}); _npop_handler(
0u); return; _LL32:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp52=( struct Cyc_Absyn_Tunion_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp52[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp53; _temp53.tag= Cyc_Absyn_Tunion_p;
_temp53.f1= _temp41; _temp53.f2= _temp39; _temp53.f3= 0; _temp53.f4= 0; _temp53;});
_temp52;}))); _npop_handler( 0u); return; _LL34:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_p_struct* _temp54=( struct Cyc_Absyn_Enum_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_p_struct)); _temp54[ 0]=({ struct Cyc_Absyn_Enum_p_struct
_temp55; _temp55.tag= Cyc_Absyn_Enum_p; _temp55.f1= _temp45; _temp55.f2= _temp43;
_temp55;}); _temp54;}))); _npop_handler( 0u); return; _LL36:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_AnonEnum_p_struct* _temp56=( struct Cyc_Absyn_AnonEnum_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AnonEnum_p_struct)); _temp56[ 0]=({ struct
Cyc_Absyn_AnonEnum_p_struct _temp57; _temp57.tag= Cyc_Absyn_AnonEnum_p; _temp57.f1=(
void*) _temp49; _temp57.f2= _temp47; _temp57;}); _temp56;}))); _npop_handler( 0u);
return; _LL38: goto _LL28; _LL28:;}; _pop_handler();} else{ void* _temp25=( void*)
_exn_thrown; void* _temp59= _temp25; _LL61: if( _temp59 ==  Cyc_Dict_Absent){
goto _LL62;} else{ goto _LL63;} _LL63: goto _LL64; _LL62: goto _LL60; _LL64:(
void) _throw( _temp59); _LL60:;}}}{ void* _temp65=(* _temp10).f1; struct Cyc_List_List*
_temp73; _LL67: if( _temp65 == ( void*) Cyc_Absyn_Loc_n){ goto _LL68;} else{
goto _LL69;} _LL69: if(( unsigned int) _temp65 >  1u?*(( int*) _temp65) ==  Cyc_Absyn_Rel_n:
0){ _LL74: _temp73=(( struct Cyc_Absyn_Rel_n_struct*) _temp65)->f1; if( _temp73
==  0){ goto _LL70;} else{ goto _LL71;}} else{ goto _LL71;} _LL71: goto _LL72;
_LL68: goto _LL70; _LL70:(* _temp10).f1=( void*) Cyc_Absyn_Loc_n;( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Var_p_struct* _temp75=( struct Cyc_Absyn_Var_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp75[ 0]=({ struct Cyc_Absyn_Var_p_struct
_temp76; _temp76.tag= Cyc_Absyn_Var_p; _temp76.f1= Cyc_Absyn_new_vardecl(
_temp10,( void*) Cyc_Absyn_VoidType, 0); _temp76;}); _temp75;}))); return; _LL72:({
void* _temp77[ 0u]={}; Cyc_Tcutil_terr( p->loc, _tag_arr("qualified variable in pattern",
sizeof( unsigned char), 30u), _tag_arr( _temp77, sizeof( void*), 0u));});
return; _LL66:;} _LL5:{ struct _handler_cons _temp78; _push_handler(& _temp78);{
int _temp80= 0; if( setjmp( _temp78.handler)){ _temp80= 1;} if( ! _temp80){{
void* _temp81= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp16); struct Cyc_Absyn_Structdecl*
_temp93; struct Cyc_Absyn_Tunionfield* _temp95; struct Cyc_Absyn_Tuniondecl*
_temp97; _LL83: if(*(( int*) _temp81) ==  Cyc_Tcenv_StructRes){ _LL94: _temp93=((
struct Cyc_Tcenv_StructRes_struct*) _temp81)->f1; goto _LL84;} else{ goto _LL85;}
_LL85: if(*(( int*) _temp81) ==  Cyc_Tcenv_TunionRes){ _LL98: _temp97=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp81)->f1; goto _LL96; _LL96: _temp95=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp81)->f2; goto _LL86;} else{ goto _LL87;} _LL87:
if(*(( int*) _temp81) ==  Cyc_Tcenv_EnumRes){ goto _LL88;} else{ goto _LL89;}
_LL89: if(*(( int*) _temp81) ==  Cyc_Tcenv_AnonEnumRes){ goto _LL90;} else{ goto
_LL91;} _LL91: if(*(( int*) _temp81) ==  Cyc_Tcenv_VarRes){ goto _LL92;} else{
goto _LL82;} _LL84: { struct Cyc_List_List* _temp99= 0; for( 0; _temp12 !=  0;
_temp12= _temp12->tl){ _temp99=({ struct Cyc_List_List* _temp100=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp100->hd=( void*)({ struct
_tuple7* _temp101=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7));
_temp101->f1= 0; _temp101->f2=( struct Cyc_Absyn_Pat*) _temp12->hd; _temp101;});
_temp100->tl= _temp99; _temp100;});}( void*)( p->r=( void*)(( void*)({ struct
Cyc_Absyn_Struct_p_struct* _temp102=( struct Cyc_Absyn_Struct_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp102[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp103; _temp103.tag= Cyc_Absyn_Struct_p; _temp103.f1=
_temp93; _temp103.f2= 0; _temp103.f3= _temp14; _temp103.f4=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp99); _temp103;}); _temp102;})));
_npop_handler( 0u); return;} _LL86:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp104=( struct Cyc_Absyn_Tunion_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp104[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp105; _temp105.tag= Cyc_Absyn_Tunion_p;
_temp105.f1= _temp97; _temp105.f2= _temp95; _temp105.f3= _temp14; _temp105.f4=
_temp12; _temp105;}); _temp104;}))); _npop_handler( 0u); return; _LL88: goto
_LL90; _LL90:({ struct Cyc_Std_String_pa_struct _temp107; _temp107.tag= Cyc_Std_String_pa;
_temp107.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp16);{ void*
_temp106[ 1u]={& _temp107}; Cyc_Tcutil_terr( p->loc, _tag_arr("bad enum constructor %s in pattern",
sizeof( unsigned char), 35u), _tag_arr( _temp106, sizeof( void*), 1u));}});
_npop_handler( 0u); return; _LL92: goto _LL82; _LL82:;}; _pop_handler();} else{
void* _temp79=( void*) _exn_thrown; void* _temp109= _temp79; _LL111: if(
_temp109 ==  Cyc_Dict_Absent){ goto _LL112;} else{ goto _LL113;} _LL113: goto
_LL114; _LL112: goto _LL110; _LL114:( void) _throw( _temp109); _LL110:;}}}({
struct Cyc_Std_String_pa_struct _temp116; _temp116.tag= Cyc_Std_String_pa;
_temp116.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp16);{ void*
_temp115[ 1u]={& _temp116}; Cyc_Tcutil_terr( p->loc, _tag_arr("%s is not a constructor in pattern",
sizeof( unsigned char), 35u), _tag_arr( _temp115, sizeof( void*), 1u));}});
return; _LL7:{ struct _handler_cons _temp117; _push_handler(& _temp117);{ int
_temp119= 0; if( setjmp( _temp117.handler)){ _temp119= 1;} if( ! _temp119){{
void* _temp120= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp22); struct Cyc_Absyn_Structdecl*
_temp126; _LL122: if(*(( int*) _temp120) ==  Cyc_Tcenv_StructRes){ _LL127:
_temp126=(( struct Cyc_Tcenv_StructRes_struct*) _temp120)->f1; goto _LL123;}
else{ goto _LL124;} _LL124: goto _LL125; _LL123:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Struct_p_struct* _temp128=( struct Cyc_Absyn_Struct_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp128[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp129; _temp129.tag= Cyc_Absyn_Struct_p; _temp129.f1=
_temp126; _temp129.f2= 0; _temp129.f3= _temp20; _temp129.f4= _temp18; _temp129;});
_temp128;}))); _npop_handler( 0u); return; _LL125: goto _LL121; _LL121:;};
_pop_handler();} else{ void* _temp118=( void*) _exn_thrown; void* _temp131=
_temp118; _LL133: if( _temp131 ==  Cyc_Dict_Absent){ goto _LL134;} else{ goto
_LL135;} _LL135: goto _LL136; _LL134: goto _LL132; _LL136:( void) _throw(
_temp131); _LL132:;}}}({ struct Cyc_Std_String_pa_struct _temp138; _temp138.tag=
Cyc_Std_String_pa; _temp138.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp22);{ void* _temp137[ 1u]={& _temp138}; Cyc_Tcutil_terr( p->loc, _tag_arr("%s is not a struct name, but pattern has designators",
sizeof( unsigned char), 53u), _tag_arr( _temp137, sizeof( void*), 1u));}});
return; _LL9: return; _LL1:;} static struct _tagged_arr* Cyc_Tcpat_get_name(
struct Cyc_Absyn_Vardecl* vd){ return(* vd->name).f2;} static void* Cyc_Tcpat_any_type(
struct Cyc_List_List* s, void** topt){ if( topt !=  0){ return* topt;} return
Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({ struct Cyc_Core_Opt*
_temp139=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp139->v=( void*) s; _temp139;}));} static void* Cyc_Tcpat_num_type( void**
topt, void* numt){ if( topt !=  0? Cyc_Tcutil_coerceable(* topt): 0){ return*
topt;}{ void* _temp140= Cyc_Tcutil_compress( numt); _LL142: if(( unsigned int)
_temp140 >  4u?*(( int*) _temp140) ==  Cyc_Absyn_EnumType: 0){ goto _LL143;}
else{ goto _LL144;} _LL144: goto _LL145; _LL143: if( topt !=  0){ return* topt;}
goto _LL141; _LL145: goto _LL141; _LL141:;} return numt;} static void Cyc_Tcpat_set_vd(
struct Cyc_Absyn_Vardecl* vd, struct Cyc_List_List** v_result_ptr, void* t){(
void*)( vd->type=( void*) t); vd->tq= Cyc_Absyn_empty_tqual();* v_result_ptr=(
struct Cyc_List_List*)({ struct Cyc_List_List* _temp146=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp146->hd=( void*) vd; _temp146->tl=*
v_result_ptr; _temp146;});} struct _tuple8{ struct Cyc_Absyn_Tqual f1; void* f2;
} ; struct _tuple9{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Pat* f2;
} ; struct _tuple6 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat( te, p);{ void* t;
struct Cyc_List_List* tv_result= 0; struct Cyc_List_List* v_result= 0;{ void*
_temp147=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp183; struct Cyc_Absyn_Vardecl*
_temp185; void* _temp187; void* _temp189; struct Cyc_Absyn_Enumfield* _temp191;
struct Cyc_Absyn_Enumdecl* _temp193; struct Cyc_Absyn_Enumfield* _temp195; void*
_temp197; struct Cyc_Absyn_Pat* _temp199; struct Cyc_List_List* _temp201; struct
Cyc_List_List* _temp203; struct Cyc_List_List* _temp205; struct Cyc_Core_Opt*
_temp207; struct Cyc_Core_Opt** _temp209; struct Cyc_Absyn_Structdecl* _temp210;
struct Cyc_List_List* _temp212; struct Cyc_List_List* _temp214; struct Cyc_Absyn_Tunionfield*
_temp216; struct Cyc_Absyn_Tuniondecl* _temp218; _LL149: if( _temp147 == ( void*)
Cyc_Absyn_Wild_p){ goto _LL150;} else{ goto _LL151;} _LL151: if(( unsigned int)
_temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_Var_p: 0){ _LL184: _temp183=((
struct Cyc_Absyn_Var_p_struct*) _temp147)->f1; goto _LL152;} else{ goto _LL153;}
_LL153: if(( unsigned int) _temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_Reference_p:
0){ _LL186: _temp185=(( struct Cyc_Absyn_Reference_p_struct*) _temp147)->f1;
goto _LL154;} else{ goto _LL155;} _LL155: if(( unsigned int) _temp147 >  2u?*((
int*) _temp147) ==  Cyc_Absyn_Int_p: 0){ _LL188: _temp187=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp147)->f1; if( _temp187 == ( void*) Cyc_Absyn_Unsigned){ goto _LL156;} else{
goto _LL157;}} else{ goto _LL157;} _LL157: if(( unsigned int) _temp147 >  2u?*((
int*) _temp147) ==  Cyc_Absyn_Int_p: 0){ _LL190: _temp189=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp147)->f1; if( _temp189 == ( void*) Cyc_Absyn_Signed){ goto _LL158;} else{
goto _LL159;}} else{ goto _LL159;} _LL159: if(( unsigned int) _temp147 >  2u?*((
int*) _temp147) ==  Cyc_Absyn_Char_p: 0){ goto _LL160;} else{ goto _LL161;}
_LL161: if(( unsigned int) _temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_Float_p:
0){ goto _LL162;} else{ goto _LL163;} _LL163: if(( unsigned int) _temp147 >  2u?*((
int*) _temp147) ==  Cyc_Absyn_Enum_p: 0){ _LL194: _temp193=(( struct Cyc_Absyn_Enum_p_struct*)
_temp147)->f1; goto _LL192; _LL192: _temp191=(( struct Cyc_Absyn_Enum_p_struct*)
_temp147)->f2; goto _LL164;} else{ goto _LL165;} _LL165: if(( unsigned int)
_temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_AnonEnum_p: 0){ _LL198:
_temp197=( void*)(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp147)->f1; goto
_LL196; _LL196: _temp195=(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp147)->f2;
goto _LL166;} else{ goto _LL167;} _LL167: if( _temp147 == ( void*) Cyc_Absyn_Null_p){
goto _LL168;} else{ goto _LL169;} _LL169: if(( unsigned int) _temp147 >  2u?*((
int*) _temp147) ==  Cyc_Absyn_Pointer_p: 0){ _LL200: _temp199=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp147)->f1; goto _LL170;} else{ goto _LL171;} _LL171: if(( unsigned int)
_temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_Tuple_p: 0){ _LL202: _temp201=((
struct Cyc_Absyn_Tuple_p_struct*) _temp147)->f1; goto _LL172;} else{ goto _LL173;}
_LL173: if(( unsigned int) _temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_Struct_p:
0){ _LL211: _temp210=(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f1; goto
_LL208; _LL208: _temp207=(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f2;
_temp209=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f2;
goto _LL206; _LL206: _temp205=(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f3;
goto _LL204; _LL204: _temp203=(( struct Cyc_Absyn_Struct_p_struct*) _temp147)->f4;
goto _LL174;} else{ goto _LL175;} _LL175: if(( unsigned int) _temp147 >  2u?*((
int*) _temp147) ==  Cyc_Absyn_Tunion_p: 0){ _LL219: _temp218=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f1; goto _LL217; _LL217: _temp216=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f2; goto _LL215; _LL215: _temp214=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f3; goto _LL213; _LL213: _temp212=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp147)->f4; goto _LL176;} else{ goto _LL177;} _LL177: if(( unsigned int)
_temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_UnknownId_p: 0){ goto _LL178;}
else{ goto _LL179;} _LL179: if(( unsigned int) _temp147 >  2u?*(( int*) _temp147)
==  Cyc_Absyn_UnknownCall_p: 0){ goto _LL180;} else{ goto _LL181;} _LL181: if((
unsigned int) _temp147 >  2u?*(( int*) _temp147) ==  Cyc_Absyn_UnknownFields_p:
0){ goto _LL182;} else{ goto _LL148;} _LL150: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars(
te), topt); goto _LL148; _LL152: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars(
te), topt); Cyc_Tcpat_set_vd( _temp183,& v_result, t); goto _LL148; _LL154: t=
Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars( te), topt); if( region_opt ==  0){({
void* _temp220[ 0u]={}; Cyc_Tcutil_terr( p->loc, _tag_arr("* pattern would point into an unknown region",
sizeof( unsigned char), 45u), _tag_arr( _temp220, sizeof( void*), 0u));}); goto
_LL148;} Cyc_Tcpat_set_vd( _temp185,& v_result,( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp221=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp221[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp222; _temp222.tag= Cyc_Absyn_PointerType;
_temp222.f1=({ struct Cyc_Absyn_PtrInfo _temp223; _temp223.elt_typ=( void*) t;
_temp223.rgn_typ=( void*)* region_opt; _temp223.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 0); _temp223.tq= Cyc_Absyn_empty_tqual();
_temp223.bounds= Cyc_Absyn_empty_conref(); _temp223;}); _temp222;}); _temp221;}));
goto _LL148; _LL156: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uint_t); goto _LL148;
_LL158: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t); goto _LL148; _LL160: t=
Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto _LL148; _LL162: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_float_t); goto _LL148; _LL164: t= Cyc_Tcpat_num_type( topt,(
void*)({ struct Cyc_Absyn_EnumType_struct* _temp224=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp224[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp225; _temp225.tag= Cyc_Absyn_EnumType; _temp225.f1=
_temp193->name; _temp225.f2=( struct Cyc_Absyn_Enumdecl*) _temp193; _temp225;});
_temp224;})); goto _LL148; _LL166: t= Cyc_Tcpat_num_type( topt, _temp197); goto
_LL148; _LL168: if( topt !=  0){ void* _temp226= Cyc_Tcutil_compress(* topt);
_LL228: if(( unsigned int) _temp226 >  4u?*(( int*) _temp226) ==  Cyc_Absyn_PointerType:
0){ goto _LL229;} else{ goto _LL230;} _LL230: goto _LL231; _LL229: t=* topt;
goto tcpat_end; _LL231: goto _LL227; _LL227:;}{ struct Cyc_List_List* _temp232=
Cyc_Tcenv_lookup_type_vars( te); t=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp233=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp233[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp234; _temp234.tag= Cyc_Absyn_PointerType;
_temp234.f1=({ struct Cyc_Absyn_PtrInfo _temp235; _temp235.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({ struct Cyc_Core_Opt* _temp237=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp237->v=( void*)
_temp232; _temp237;})); _temp235.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt* _temp236=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp236->v=( void*) _temp232;
_temp236;})); _temp235.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp235.tq= Cyc_Absyn_empty_tqual(); _temp235.bounds= Cyc_Absyn_empty_conref();
_temp235;}); _temp234;}); _temp233;}); goto _LL148;} _LL170: { void* inner_typ=(
void*) Cyc_Absyn_VoidType; void** _temp238= 0; if( topt !=  0){ void* _temp239=
Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp245; void* _temp247;
_LL241: if(( unsigned int) _temp239 >  4u?*(( int*) _temp239) ==  Cyc_Absyn_PointerType:
0){ _LL246: _temp245=(( struct Cyc_Absyn_PointerType_struct*) _temp239)->f1;
_LL248: _temp247=( void*) _temp245.elt_typ; goto _LL242;} else{ goto _LL243;}
_LL243: goto _LL244; _LL242: inner_typ= _temp247; _temp238=( void**)& inner_typ;
goto _LL240; _LL244: goto _LL240; _LL240:;}{ void* ptr_rgn= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt* _temp283=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp283->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp283;})); struct Cyc_List_List* _temp251;
struct Cyc_List_List* _temp253; struct _tuple6 _temp249= Cyc_Tcpat_tcPatRec( te,
_temp199, _temp238,( void**)& ptr_rgn); _LL254: _temp253= _temp249.f1; goto
_LL252; _LL252: _temp251= _temp249.f2; goto _LL250; _LL250: tv_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp253); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp251); if( _temp238
==  0){ void* _temp255= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp199->topt))->v); struct Cyc_Absyn_TunionFieldInfo _temp261;
struct Cyc_List_List* _temp263; void* _temp265; struct Cyc_Absyn_Tunionfield*
_temp267; struct Cyc_Absyn_Tuniondecl* _temp269; _LL257: if(( unsigned int)
_temp255 >  4u?*(( int*) _temp255) ==  Cyc_Absyn_TunionFieldType: 0){ _LL262:
_temp261=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp255)->f1; _LL266:
_temp265=( void*) _temp261.field_info; if(*(( int*) _temp265) ==  Cyc_Absyn_KnownTunionfield){
_LL270: _temp269=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp265)->f1;
goto _LL268; _LL268: _temp267=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp265)->f2; goto _LL264;} else{ goto _LL259;} _LL264: _temp263= _temp261.targs;
goto _LL258;} else{ goto _LL259;} _LL259: goto _LL260; _LL258: t=( void*)({
struct Cyc_Absyn_TunionType_struct* _temp271=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp271[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp272; _temp272.tag= Cyc_Absyn_TunionType;
_temp272.f1=({ struct Cyc_Absyn_TunionInfo _temp273; _temp273.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp274=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp274[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp275; _temp275.tag= Cyc_Absyn_KnownTunion;
_temp275.f1=({ struct Cyc_Absyn_Tuniondecl** _temp276=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp276[ 0]= _temp269;
_temp276;}); _temp275;}); _temp274;})); _temp273.targs= _temp263; _temp273.rgn=(
void*) ptr_rgn; _temp273;}); _temp272;}); _temp271;}); goto _LL256; _LL260: t=(
void*)({ struct Cyc_Absyn_PointerType_struct* _temp277=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp277[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp278; _temp278.tag= Cyc_Absyn_PointerType;
_temp278.f1=({ struct Cyc_Absyn_PtrInfo _temp279; _temp279.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp199->topt))->v); _temp279.rgn_typ=(
void*) ptr_rgn; _temp279.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp279.tq= Cyc_Absyn_empty_tqual(); _temp279.bounds= Cyc_Absyn_empty_conref();
_temp279;}); _temp278;}); _temp277;}); goto _LL256; _LL256:;} else{ t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp280=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp280[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp281; _temp281.tag= Cyc_Absyn_PointerType;
_temp281.f1=({ struct Cyc_Absyn_PtrInfo _temp282; _temp282.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp199->topt))->v); _temp282.rgn_typ=(
void*) ptr_rgn; _temp282.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp282.tq= Cyc_Absyn_empty_tqual(); _temp282.bounds= Cyc_Absyn_empty_conref();
_temp282;}); _temp281;}); _temp280;});} goto _LL148;}} _LL172: { struct Cyc_List_List*
pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if( topt !=  0){ void* _temp284=
Cyc_Tcutil_compress(* topt); struct Cyc_List_List* _temp290; _LL286: if((
unsigned int) _temp284 >  4u?*(( int*) _temp284) ==  Cyc_Absyn_TupleType: 0){
_LL291: _temp290=(( struct Cyc_Absyn_TupleType_struct*) _temp284)->f1; goto
_LL287;} else{ goto _LL288;} _LL288: goto _LL289; _LL287: topt_ts= _temp290;
goto _LL285; _LL289: goto _LL285; _LL285:;} for( 0; _temp201 !=  0; _temp201=
_temp201->tl){ void** _temp292= 0; if( topt_ts !=  0){ _temp292=( void**)&(*((
struct _tuple8*) topt_ts->hd)).f2; topt_ts= topt_ts->tl;}{ struct Cyc_List_List*
_temp295; struct Cyc_List_List* _temp297; struct _tuple6 _temp293= Cyc_Tcpat_tcPatRec(
te,( struct Cyc_Absyn_Pat*) _temp201->hd, _temp292, region_opt); _LL298:
_temp297= _temp293.f1; goto _LL296; _LL296: _temp295= _temp293.f2; goto _LL294;
_LL294: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp297); v_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp295); pat_ts=({ struct Cyc_List_List* _temp299=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp299->hd=( void*)({ struct
_tuple8* _temp300=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8));
_temp300->f1= Cyc_Absyn_empty_tqual(); _temp300->f2=( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Pat*) _temp201->hd)->topt))->v; _temp300;});
_temp299->tl= pat_ts; _temp299;});}} t=( void*)({ struct Cyc_Absyn_TupleType_struct*
_temp301=( struct Cyc_Absyn_TupleType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct));
_temp301[ 0]=({ struct Cyc_Absyn_TupleType_struct _temp302; _temp302.tag= Cyc_Absyn_TupleType;
_temp302.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
pat_ts); _temp302;}); _temp301;}); goto _LL148;} _LL174: if( _temp205 !=  0){(
int) _throw(({ void* _temp303[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcPat: struct<...> not implemented",
sizeof( unsigned char), 35u), _tag_arr( _temp303, sizeof( void*), 0u));}));}{
struct _RegionHandle _temp304= _new_region(); struct _RegionHandle* rgn=&
_temp304; _push_region( rgn);{ struct _tuple4 _temp305=({ struct _tuple4
_temp331; _temp331.f1= Cyc_Tcenv_lookup_type_vars( te); _temp331.f2= rgn;
_temp331;}); struct Cyc_List_List* _temp306=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp305, _temp210->tvs); struct Cyc_List_List* _temp307=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp306); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp308=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp308[ 0]=({ struct Cyc_Absyn_StructType_struct _temp309; _temp309.tag= Cyc_Absyn_StructType;
_temp309.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp210->name))->v); _temp309.f2= _temp307; _temp309.f3=({ struct
Cyc_Absyn_Structdecl** _temp310=( struct Cyc_Absyn_Structdecl**) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp310[ 0]= _temp210; _temp310;});
_temp309;}); _temp308;}); if( topt !=  0){ Cyc_Tcutil_unify( t,* topt);}*
_temp209=({ struct Cyc_Core_Opt* _temp311=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp311->v=( void*) _temp307; _temp311;}); if(
_temp210->fields ==  0){({ void* _temp312[ 0u]={}; Cyc_Tcutil_terr( p->loc,
_tag_arr("can't destructure an abstract struct", sizeof( unsigned char), 37u),
_tag_arr( _temp312, sizeof( void*), 0u));}); _npop_handler( 0u); goto _LL148;}{
struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct _RegionHandle*
rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des, struct Cyc_List_List*
fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, p->loc, _temp203,( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp210->fields))->v);
for( 0; fields !=  0; fields= fields->tl){ struct _tuple9 _temp315; struct Cyc_Absyn_Pat*
_temp316; struct Cyc_Absyn_Structfield* _temp318; struct _tuple9* _temp313=(
struct _tuple9*) fields->hd; _temp315=* _temp313; _LL319: _temp318= _temp315.f1;
goto _LL317; _LL317: _temp316= _temp315.f2; goto _LL314; _LL314: { void*
_temp320= Cyc_Tcutil_rsubstitute( rgn, _temp306,( void*) _temp318->type); struct
Cyc_List_List* _temp323; struct Cyc_List_List* _temp325; struct _tuple6 _temp321=
Cyc_Tcpat_tcPatRec( te, _temp316,( void**)& _temp320, region_opt); _LL326:
_temp325= _temp321.f1; goto _LL324; _LL324: _temp323= _temp321.f2; goto _LL322;
_LL322: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp325); v_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp323); if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp316->topt))->v, _temp320)){({ struct Cyc_Std_String_pa_struct
_temp330; _temp330.tag= Cyc_Std_String_pa; _temp330.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp316->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp329; _temp329.tag= Cyc_Std_String_pa; _temp329.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp320);{ struct Cyc_Std_String_pa_struct _temp328; _temp328.tag= Cyc_Std_String_pa;
_temp328.f1=( struct _tagged_arr)* _temp318->name;{ void* _temp327[ 3u]={&
_temp328,& _temp329,& _temp330}; Cyc_Tcutil_terr( p->loc, _tag_arr("field %s of struct pattern expects type %s != %s",
sizeof( unsigned char), 49u), _tag_arr( _temp327, sizeof( void*), 3u));}}}});}}}}};
_pop_region( rgn);} goto _LL148; _LL176: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp214) != (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp216->tvs)){({ void* _temp332[ 0u]={}; Cyc_Tcutil_terr( p->loc, _tag_arr("wrong number of existential type variables in pattern",
sizeof( unsigned char), 54u), _tag_arr( _temp332, sizeof( void*), 0u));});} if(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp214) !=  0){ region_opt=
0;}{ struct _RegionHandle _temp333= _new_region(); struct _RegionHandle* rgn=&
_temp333; _push_region( rgn);{ struct Cyc_List_List* _temp334= 0;{ struct Cyc_List_List*
t= _temp214; for( 0; t !=  0; t= t->tl){ struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)
t->hd;{ void* _temp335=( void*)( Cyc_Absyn_compress_conref( tv->kind))->v; void*
_temp345; void* _temp347; _LL337: if(( unsigned int) _temp335 >  1u?*(( int*)
_temp335) ==  Cyc_Absyn_Eq_constr: 0){ _LL346: _temp345=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp335)->f1; if( _temp345 == ( void*) Cyc_Absyn_BoxKind){ goto _LL338;} else{
goto _LL339;}} else{ goto _LL339;} _LL339: if(( unsigned int) _temp335 >  1u?*((
int*) _temp335) ==  Cyc_Absyn_Eq_constr: 0){ _LL348: _temp347=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp335)->f1; goto _LL340;} else{ goto _LL341;}
_LL341: if( _temp335 == ( void*) Cyc_Absyn_No_constr){ goto _LL342;} else{ goto
_LL343;} _LL343: if(( unsigned int) _temp335 >  1u?*(( int*) _temp335) ==  Cyc_Absyn_Forward_constr:
0){ goto _LL344;} else{ goto _LL336;} _LL338: goto _LL336; _LL340:({ struct Cyc_Std_String_pa_struct
_temp352; _temp352.tag= Cyc_Std_String_pa; _temp352.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string(
_temp347);{ struct Cyc_Std_String_pa_struct _temp351; _temp351.tag= Cyc_Std_String_pa;
_temp351.f1=( struct _tagged_arr)* tv->name;{ struct Cyc_Std_String_pa_struct
_temp350; _temp350.tag= Cyc_Std_String_pa; _temp350.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp218->name);{ void* _temp349[ 3u]={& _temp350,& _temp351,& _temp352}; Cyc_Tcutil_terr(
p->loc, _tag_arr("tunion member %s abstracts type variable %s of kind %s != B",
sizeof( unsigned char), 60u), _tag_arr( _temp349, sizeof( void*), 3u));}}}});
goto _LL336; _LL342:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp353=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp353[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp354; _temp354.tag= Cyc_Absyn_Eq_constr;
_temp354.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp354;}); _temp353;})));
goto _LL336; _LL344:( int) _throw(({ void* _temp355[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("forward after compress in conref", sizeof( unsigned char), 33u),
_tag_arr( _temp355, sizeof( void*), 0u));})); _LL336:;} _temp334=({ struct Cyc_List_List*
_temp356=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp356->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp357=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp357[ 0]=({ struct Cyc_Absyn_VarType_struct _temp358; _temp358.tag= Cyc_Absyn_VarType;
_temp358.f1= tv; _temp358;}); _temp357;})); _temp356->tl= _temp334; _temp356;});}}
_temp334= Cyc_List_imp_rev( _temp334); tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp214);{ struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te,
_temp214); struct Cyc_List_List* tqts= _temp216->typs; struct Cyc_List_List*
_temp359= Cyc_Tcenv_lookup_type_vars( te); struct _tuple4 _temp360=({ struct
_tuple4 _temp412; _temp412.f1= _temp359; _temp412.f2= rgn; _temp412;}); struct
Cyc_List_List* _temp361=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp360, _temp218->tvs); struct Cyc_List_List* _temp362=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp216->tvs, _temp334);
struct Cyc_List_List* _temp363=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp361); struct Cyc_List_List* _temp364=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp362); struct Cyc_List_List* _temp365=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp361, _temp362); if( tqts ==  0){
t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp366=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp366[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp367; _temp367.tag= Cyc_Absyn_TunionType;
_temp367.f1=({ struct Cyc_Absyn_TunionInfo _temp368; _temp368.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp370=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp370[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp371; _temp371.tag= Cyc_Absyn_KnownTunion;
_temp371.f1=({ struct Cyc_Absyn_Tuniondecl** _temp372=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp372[ 0]= _temp218;
_temp372;}); _temp371;}); _temp370;})); _temp368.targs= _temp363; _temp368.rgn=(
void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt*
_temp369=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp369->v=( void*) _temp359; _temp369;})); _temp368;}); _temp367;}); _temp366;});}
else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp373=( struct
Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp373[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp374; _temp374.tag=
Cyc_Absyn_TunionFieldType; _temp374.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp375; _temp375.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp376=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp376[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp377; _temp377.tag= Cyc_Absyn_KnownTunionfield; _temp377.f1= _temp218;
_temp377.f2= _temp216; _temp377;}); _temp376;})); _temp375.targs= _temp363;
_temp375;}); _temp374;}); _temp373;});} if( topt !=  0? tqts ==  0: 0){ void*
_temp378= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_TunionInfo _temp386;
struct Cyc_List_List* _temp388; _LL380: if(( unsigned int) _temp378 >  4u?*((
int*) _temp378) ==  Cyc_Absyn_TunionFieldType: 0){ goto _LL381;} else{ goto
_LL382;} _LL382: if(( unsigned int) _temp378 >  4u?*(( int*) _temp378) ==  Cyc_Absyn_TunionType:
0){ _LL387: _temp386=(( struct Cyc_Absyn_TunionType_struct*) _temp378)->f1;
_LL389: _temp388= _temp386.targs; goto _LL383;} else{ goto _LL384;} _LL384: goto
_LL385; _LL381: t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp390=(
struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp390[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp391; _temp391.tag=
Cyc_Absyn_TunionFieldType; _temp391.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp392; _temp392.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp393=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp393[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp394; _temp394.tag= Cyc_Absyn_KnownTunionfield; _temp394.f1= _temp218;
_temp394.f2= _temp216; _temp394;}); _temp393;})); _temp392.targs= _temp363;
_temp392;}); _temp391;}); _temp390;}); goto _LL379; _LL383: { struct Cyc_List_List*
_temp395= _temp363; for( 0; _temp395 !=  0? _temp388 !=  0: 0;( _temp395=
_temp395->tl, _temp388= _temp388->tl)){ Cyc_Tcutil_unify(( void*) _temp395->hd,(
void*) _temp388->hd);} goto _LL379;} _LL385: goto _LL379; _LL379:;} for( 0;
_temp212 !=  0? tqts !=  0: 0;( _temp212= _temp212->tl, tqts= tqts->tl)){ struct
Cyc_Absyn_Pat* _temp396=( struct Cyc_Absyn_Pat*) _temp212->hd; void* _temp397=
Cyc_Tcutil_rsubstitute( rgn, _temp365,(*(( struct _tuple8*) tqts->hd)).f2);
struct Cyc_List_List* _temp400; struct Cyc_List_List* _temp402; struct _tuple6
_temp398= Cyc_Tcpat_tcPatRec( te2, _temp396,( void**)& _temp397, region_opt);
_LL403: _temp402= _temp398.f1; goto _LL401; _LL401: _temp400= _temp398.f2; goto
_LL399; _LL399: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp402); v_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp400); if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp396->topt))->v, _temp397)){({ struct Cyc_Std_String_pa_struct
_temp407; _temp407.tag= Cyc_Std_String_pa; _temp407.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp396->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp406; _temp406.tag= Cyc_Std_String_pa; _temp406.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp397);{ struct Cyc_Std_String_pa_struct _temp405; _temp405.tag= Cyc_Std_String_pa;
_temp405.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp218->name);{
void* _temp404[ 3u]={& _temp405,& _temp406,& _temp407}; Cyc_Tcutil_terr(
_temp396->loc, _tag_arr("%s expects argument type %s, not %s", sizeof(
unsigned char), 36u), _tag_arr( _temp404, sizeof( void*), 3u));}}}});}} if(
_temp212 !=  0){({ struct Cyc_Std_String_pa_struct _temp409; _temp409.tag= Cyc_Std_String_pa;
_temp409.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp218->name);{
void* _temp408[ 1u]={& _temp409}; Cyc_Tcutil_terr( p->loc, _tag_arr("too many arguments for tunion constructor %s",
sizeof( unsigned char), 45u), _tag_arr( _temp408, sizeof( void*), 1u));}});} if(
tqts !=  0){({ struct Cyc_Std_String_pa_struct _temp411; _temp411.tag= Cyc_Std_String_pa;
_temp411.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp218->name);{
void* _temp410[ 1u]={& _temp411}; Cyc_Tcutil_terr( p->loc, _tag_arr("too few arguments for tunion constructor %s",
sizeof( unsigned char), 44u), _tag_arr( _temp410, sizeof( void*), 1u));}});}}};
_pop_region( rgn);} goto _LL148; _LL178: goto _LL180; _LL180: goto _LL182;
_LL182: t= Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp413=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp413->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp413;})); goto _LL148; _LL148:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt*
_temp414=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp414->v=( void*) t; _temp414;}); return({ struct _tuple6 _temp415; _temp415.f1=
tv_result; _temp415.f2= v_result; _temp415;});}} struct _tuple6 Cyc_Tcpat_tcPat(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p, void** topt, void**
region_opt){ struct _tuple6 _temp416= Cyc_Tcpat_tcPatRec( te, p, topt,
region_opt);{ struct _RegionHandle _temp417= _new_region(); struct _RegionHandle*
r=& _temp417; _push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_arr*(* f)( struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Tcpat_get_name, _temp416.f2), p->loc,
_tag_arr("pattern contains a repeated variable", sizeof( unsigned char), 37u));;
_pop_region( r);} return _temp416;} void Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p){ void* _temp418=( void*) p->r; struct Cyc_Absyn_Pat*
_temp430; struct Cyc_List_List* _temp432; struct Cyc_List_List* _temp434; struct
Cyc_List_List* _temp436; _LL420: if(( unsigned int) _temp418 >  2u?*(( int*)
_temp418) ==  Cyc_Absyn_Pointer_p: 0){ _LL431: _temp430=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp418)->f1; goto _LL421;} else{ goto _LL422;} _LL422: if(( unsigned int)
_temp418 >  2u?*(( int*) _temp418) ==  Cyc_Absyn_Tunion_p: 0){ _LL433: _temp432=((
struct Cyc_Absyn_Tunion_p_struct*) _temp418)->f4; goto _LL423;} else{ goto
_LL424;} _LL424: if(( unsigned int) _temp418 >  2u?*(( int*) _temp418) ==  Cyc_Absyn_Struct_p:
0){ _LL435: _temp434=(( struct Cyc_Absyn_Struct_p_struct*) _temp418)->f4; goto
_LL425;} else{ goto _LL426;} _LL426: if(( unsigned int) _temp418 >  2u?*(( int*)
_temp418) ==  Cyc_Absyn_Tuple_p: 0){ _LL437: _temp436=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp418)->f1; goto _LL427;} else{ goto _LL428;} _LL428: goto _LL429; _LL421:
Cyc_Tcpat_check_pat_regions( te, _temp430);{ void* _temp438=( void*)(( struct
Cyc_Core_Opt*) _check_null( p->topt))->v; struct Cyc_Absyn_PtrInfo _temp446;
void* _temp448; struct Cyc_Absyn_TunionInfo _temp450; void* _temp452; _LL440:
if(( unsigned int) _temp438 >  4u?*(( int*) _temp438) ==  Cyc_Absyn_PointerType:
0){ _LL447: _temp446=(( struct Cyc_Absyn_PointerType_struct*) _temp438)->f1;
_LL449: _temp448=( void*) _temp446.rgn_typ; goto _LL441;} else{ goto _LL442;}
_LL442: if(( unsigned int) _temp438 >  4u?*(( int*) _temp438) ==  Cyc_Absyn_TunionType:
0){ _LL451: _temp450=(( struct Cyc_Absyn_TunionType_struct*) _temp438)->f1;
_LL453: _temp452=( void*) _temp450.rgn; goto _LL443;} else{ goto _LL444;} _LL444:
goto _LL445; _LL441: _temp452= _temp448; goto _LL443; _LL443: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp452); return; _LL445:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp454=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp454[ 0]=({ struct Cyc_Core_Impossible_struct _temp455; _temp455.tag= Cyc_Core_Impossible;
_temp455.f1= _tag_arr("check_pat_regions: bad pointer type", sizeof(
unsigned char), 36u); _temp455;}); _temp454;})); _LL439:;} _LL423: for( 0;
_temp432 !=  0; _temp432= _temp432->tl){ Cyc_Tcpat_check_pat_regions( te,(
struct Cyc_Absyn_Pat*) _temp432->hd);}{ void* _temp456=( void*)(( struct Cyc_Core_Opt*)
_check_null( p->topt))->v; struct Cyc_Absyn_TunionInfo _temp464; void* _temp466;
_LL458: if(( unsigned int) _temp456 >  4u?*(( int*) _temp456) ==  Cyc_Absyn_TunionType:
0){ _LL465: _temp464=(( struct Cyc_Absyn_TunionType_struct*) _temp456)->f1;
_LL467: _temp466=( void*) _temp464.rgn; goto _LL459;} else{ goto _LL460;} _LL460:
if(( unsigned int) _temp456 >  4u?*(( int*) _temp456) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL461;} else{ goto _LL462;} _LL462: goto _LL463; _LL459: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp466); return; _LL461: return; _LL463:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp468=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp468[ 0]=({ struct
Cyc_Core_Impossible_struct _temp469; _temp469.tag= Cyc_Core_Impossible; _temp469.f1=
_tag_arr("check_pat_regions: bad tunion type", sizeof( unsigned char), 35u);
_temp469;}); _temp468;})); _LL457:;} _LL425: _temp436=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Pat*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Pat*(*)( struct _tuple7*)) Cyc_Core_snd, _temp434); goto _LL427;
_LL427: for( 0; _temp436 !=  0; _temp436= _temp436->tl){ Cyc_Tcpat_check_pat_regions(
te,( struct Cyc_Absyn_Pat*) _temp436->hd);} return; _LL429: return; _LL419:;}
static const int Cyc_Tcpat_Name_v= 0; struct Cyc_Tcpat_Name_v_struct{ int tag;
struct _tagged_arr f1; } ; static const int Cyc_Tcpat_Int_v= 1; struct Cyc_Tcpat_Int_v_struct{
int tag; int f1; } ; struct Cyc_Tcpat_Con_s{ void* name; int arity; struct Cyc_Core_Opt*
span; } ; static const int Cyc_Tcpat_Any= 0; static const int Cyc_Tcpat_Con= 0;
struct Cyc_Tcpat_Con_struct{ int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List*
f2; } ; static int Cyc_Tcpat_compare_con( struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s*
c2){ void* _temp470=( void*) c1->name; struct _tagged_arr _temp476; int _temp478;
_LL472: if(*(( int*) _temp470) ==  Cyc_Tcpat_Name_v){ _LL477: _temp476=(( struct
Cyc_Tcpat_Name_v_struct*) _temp470)->f1; goto _LL473;} else{ goto _LL474;}
_LL474: if(*(( int*) _temp470) ==  Cyc_Tcpat_Int_v){ _LL479: _temp478=(( struct
Cyc_Tcpat_Int_v_struct*) _temp470)->f1; goto _LL475;} else{ goto _LL471;} _LL473: {
void* _temp480=( void*) c2->name; struct _tagged_arr _temp486; _LL482: if(*((
int*) _temp480) ==  Cyc_Tcpat_Name_v){ _LL487: _temp486=(( struct Cyc_Tcpat_Name_v_struct*)
_temp480)->f1; goto _LL483;} else{ goto _LL484;} _LL484: if(*(( int*) _temp480)
==  Cyc_Tcpat_Int_v){ goto _LL485;} else{ goto _LL481;} _LL483: return Cyc_Std_zstrcmp(
_temp476, _temp486); _LL485: return - 1; _LL481:;} _LL475: { void* _temp488=(
void*) c2->name; int _temp494; _LL490: if(*(( int*) _temp488) ==  Cyc_Tcpat_Name_v){
goto _LL491;} else{ goto _LL492;} _LL492: if(*(( int*) _temp488) ==  Cyc_Tcpat_Int_v){
_LL495: _temp494=(( struct Cyc_Tcpat_Int_v_struct*) _temp488)->f1; goto _LL493;}
else{ goto _LL489;} _LL491: return 1; _LL493: return _temp478 -  _temp494;
_LL489:;} _LL471:;} static struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return((
struct Cyc_Set_Set*(*)( int(* cmp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*)))
Cyc_Set_empty)( Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(
void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={( void*) 2}; static
struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;
static unsigned char _temp497[ 5u]="NULL"; static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_null_name_value={ 0u,{ _temp497, _temp497, _temp497 +  5u}}; static
unsigned char _temp499[ 4u]="ptr"; static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={
0u,{ _temp499, _temp499, _temp499 +  4u}}; static unsigned char _temp501[ 6u]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,{ _temp501,
_temp501, _temp501 +  6u}}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(
void*)(( void*)& Cyc_Tcpat_null_name_value), 0,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={( void*)(( void*)& Cyc_Tcpat_ptr_name_value),
1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)(( void*)& Cyc_Tcpat_ptr_name_value), 1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0; static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp502=( struct Cyc_Tcpat_Con_s*) _cycalloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp502->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp503=(
struct Cyc_Tcpat_Int_v_struct*) _cycalloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp503[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp504; _temp504.tag= Cyc_Tcpat_Int_v;
_temp504.f1= i; _temp504;}); _temp503;})); _temp502->arity= 0; _temp502->span= 0;
_temp502;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_arr f){ return({ struct Cyc_Tcpat_Con_s* _temp505=( struct Cyc_Tcpat_Con_s*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp505->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp506=( struct Cyc_Tcpat_Name_v_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp506[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp507; _temp507.tag= Cyc_Tcpat_Name_v; _temp507.f1= f; _temp507;}); _temp506;}));
_temp505->arity= 0; _temp505->span= 0; _temp505;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con( unsigned char c){ return({ struct Cyc_Tcpat_Con_s* _temp508=(
struct Cyc_Tcpat_Con_s*) _cycalloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp508->name=(
void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp510=( struct Cyc_Tcpat_Int_v_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp510[ 0]=({
struct Cyc_Tcpat_Int_v_struct _temp511; _temp511.tag= Cyc_Tcpat_Int_v; _temp511.f1=(
int) c; _temp511;}); _temp510;})); _temp508->arity= 0; _temp508->span=({ struct
Cyc_Core_Opt* _temp509=( struct Cyc_Core_Opt*) _cycalloc_atomic( sizeof( struct
Cyc_Core_Opt)); _temp509->v=( void*) 256; _temp509;}); _temp508;});} static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp512=( struct Cyc_Tcpat_Con_s*) _cycalloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp512->name=( void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp512->arity=
i; _temp512->span= Cyc_Tcpat_one_opt_ptr; _temp512;});} static void* Cyc_Tcpat_null_pat(){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp513=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp513[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp514; _temp514.tag= Cyc_Tcpat_Con; _temp514.f1= Cyc_Tcpat_null_con; _temp514.f2=
0; _temp514;}); _temp513;});} static void* Cyc_Tcpat_int_pat( int i){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp515=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp515[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp516; _temp516.tag= Cyc_Tcpat_Con; _temp516.f1= Cyc_Tcpat_int_con( i);
_temp516.f2= 0; _temp516;}); _temp515;});} static void* Cyc_Tcpat_char_pat(
unsigned char c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp517=(
struct Cyc_Tcpat_Con_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp517[ 0]=({ struct Cyc_Tcpat_Con_struct _temp518; _temp518.tag= Cyc_Tcpat_Con;
_temp518.f1= Cyc_Tcpat_char_con( c); _temp518.f2= 0; _temp518;}); _temp517;});}
static void* Cyc_Tcpat_float_pat( struct _tagged_arr f){ return( void*)({ struct
Cyc_Tcpat_Con_struct* _temp519=( struct Cyc_Tcpat_Con_struct*) _cycalloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp519[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp520; _temp520.tag= Cyc_Tcpat_Con; _temp520.f1= Cyc_Tcpat_float_con( f);
_temp520.f2= 0; _temp520;}); _temp519;});} static void* Cyc_Tcpat_null_ptr_pat(
void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp521=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp521[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp522; _temp522.tag= Cyc_Tcpat_Con; _temp522.f1= Cyc_Tcpat_null_ptr_con;
_temp522.f2=({ struct Cyc_List_List* _temp523=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp523->hd=( void*) p; _temp523->tl= 0;
_temp523;}); _temp522;}); _temp521;});} static void* Cyc_Tcpat_ptr_pat( void* p){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp524=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp524[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp525; _temp525.tag= Cyc_Tcpat_Con; _temp525.f1= Cyc_Tcpat_ptr_con; _temp525.f2=({
struct Cyc_List_List* _temp526=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp526->hd=( void*) p; _temp526->tl= 0; _temp526;});
_temp525;}); _temp524;});} static void* Cyc_Tcpat_tuple_pat( struct Cyc_List_List*
ss){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp527=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp527[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp528; _temp528.tag= Cyc_Tcpat_Con; _temp528.f1= Cyc_Tcpat_tuple_con( Cyc_List_length(
ss)); _temp528.f2= ss; _temp528;}); _temp527;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_arr con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List* ps){
struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp531=( struct Cyc_Tcpat_Con_s*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp531->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp532=( struct Cyc_Tcpat_Name_v_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp532[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp533; _temp533.tag= Cyc_Tcpat_Name_v; _temp533.f1= con_name; _temp533;});
_temp532;})); _temp531->arity= Cyc_List_length( ps); _temp531->span= span;
_temp531;}); return( void*)({ struct Cyc_Tcpat_Con_struct* _temp529=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp529[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp530; _temp530.tag= Cyc_Tcpat_Con; _temp530.f1= c; _temp530.f2= ps; _temp530;});
_temp529;});} static void* Cyc_Tcpat_compile_pat( struct Cyc_Absyn_Pat* p){ void*
s;{ void* _temp534=( void*) p->r; int _temp570; void* _temp572; unsigned char
_temp574; struct _tagged_arr _temp576; struct Cyc_Absyn_Pat* _temp578; struct
Cyc_List_List* _temp580; struct Cyc_Absyn_Tunionfield* _temp582; struct Cyc_Absyn_Tuniondecl*
_temp584; struct Cyc_List_List* _temp586; struct Cyc_List_List* _temp588; struct
Cyc_List_List* _temp590; struct Cyc_Absyn_Structdecl* _temp592; struct Cyc_Absyn_Enumfield*
_temp594; struct Cyc_Absyn_Enumdecl* _temp596; struct Cyc_Absyn_Enumfield*
_temp598; void* _temp600; _LL536: if( _temp534 == ( void*) Cyc_Absyn_Wild_p){
goto _LL537;} else{ goto _LL538;} _LL538: if(( unsigned int) _temp534 >  2u?*((
int*) _temp534) ==  Cyc_Absyn_Var_p: 0){ goto _LL539;} else{ goto _LL540;}
_LL540: if( _temp534 == ( void*) Cyc_Absyn_Null_p){ goto _LL541;} else{ goto
_LL542;} _LL542: if(( unsigned int) _temp534 >  2u?*(( int*) _temp534) ==  Cyc_Absyn_Int_p:
0){ _LL573: _temp572=( void*)(( struct Cyc_Absyn_Int_p_struct*) _temp534)->f1;
goto _LL571; _LL571: _temp570=(( struct Cyc_Absyn_Int_p_struct*) _temp534)->f2;
goto _LL543;} else{ goto _LL544;} _LL544: if(( unsigned int) _temp534 >  2u?*((
int*) _temp534) ==  Cyc_Absyn_Char_p: 0){ _LL575: _temp574=(( struct Cyc_Absyn_Char_p_struct*)
_temp534)->f1; goto _LL545;} else{ goto _LL546;} _LL546: if(( unsigned int)
_temp534 >  2u?*(( int*) _temp534) ==  Cyc_Absyn_Float_p: 0){ _LL577: _temp576=((
struct Cyc_Absyn_Float_p_struct*) _temp534)->f1; goto _LL547;} else{ goto _LL548;}
_LL548: if(( unsigned int) _temp534 >  2u?*(( int*) _temp534) ==  Cyc_Absyn_Reference_p:
0){ goto _LL549;} else{ goto _LL550;} _LL550: if(( unsigned int) _temp534 >  2u?*((
int*) _temp534) ==  Cyc_Absyn_Pointer_p: 0){ _LL579: _temp578=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp534)->f1; goto _LL551;} else{ goto _LL552;} _LL552: if(( unsigned int)
_temp534 >  2u?*(( int*) _temp534) ==  Cyc_Absyn_Tunion_p: 0){ _LL585: _temp584=((
struct Cyc_Absyn_Tunion_p_struct*) _temp534)->f1; goto _LL583; _LL583: _temp582=((
struct Cyc_Absyn_Tunion_p_struct*) _temp534)->f2; goto _LL581; _LL581: _temp580=((
struct Cyc_Absyn_Tunion_p_struct*) _temp534)->f4; if( _temp580 ==  0){ goto
_LL553;} else{ goto _LL554;}} else{ goto _LL554;} _LL554: if(( unsigned int)
_temp534 >  2u?*(( int*) _temp534) ==  Cyc_Absyn_Tunion_p: 0){ _LL587: _temp586=((
struct Cyc_Absyn_Tunion_p_struct*) _temp534)->f4; goto _LL555;} else{ goto
_LL556;} _LL556: if(( unsigned int) _temp534 >  2u?*(( int*) _temp534) ==  Cyc_Absyn_Tuple_p:
0){ _LL589: _temp588=(( struct Cyc_Absyn_Tuple_p_struct*) _temp534)->f1; goto
_LL557;} else{ goto _LL558;} _LL558: if(( unsigned int) _temp534 >  2u?*(( int*)
_temp534) ==  Cyc_Absyn_Struct_p: 0){ _LL593: _temp592=(( struct Cyc_Absyn_Struct_p_struct*)
_temp534)->f1; goto _LL591; _LL591: _temp590=(( struct Cyc_Absyn_Struct_p_struct*)
_temp534)->f4; goto _LL559;} else{ goto _LL560;} _LL560: if(( unsigned int)
_temp534 >  2u?*(( int*) _temp534) ==  Cyc_Absyn_Enum_p: 0){ _LL597: _temp596=((
struct Cyc_Absyn_Enum_p_struct*) _temp534)->f1; goto _LL595; _LL595: _temp594=((
struct Cyc_Absyn_Enum_p_struct*) _temp534)->f2; goto _LL561;} else{ goto _LL562;}
_LL562: if(( unsigned int) _temp534 >  2u?*(( int*) _temp534) ==  Cyc_Absyn_AnonEnum_p:
0){ _LL601: _temp600=( void*)(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp534)->f1;
goto _LL599; _LL599: _temp598=(( struct Cyc_Absyn_AnonEnum_p_struct*) _temp534)->f2;
goto _LL563;} else{ goto _LL564;} _LL564: if(( unsigned int) _temp534 >  2u?*((
int*) _temp534) ==  Cyc_Absyn_UnknownId_p: 0){ goto _LL565;} else{ goto _LL566;}
_LL566: if(( unsigned int) _temp534 >  2u?*(( int*) _temp534) ==  Cyc_Absyn_UnknownCall_p:
0){ goto _LL567;} else{ goto _LL568;} _LL568: if(( unsigned int) _temp534 >  2u?*((
int*) _temp534) ==  Cyc_Absyn_UnknownFields_p: 0){ goto _LL569;} else{ goto
_LL535;} _LL537: s=( void*) Cyc_Tcpat_Any; goto _LL535; _LL539: s=( void*) Cyc_Tcpat_Any;
goto _LL535; _LL541: s= Cyc_Tcpat_null_pat(); goto _LL535; _LL543: s= Cyc_Tcpat_int_pat(
_temp570); goto _LL535; _LL545: s= Cyc_Tcpat_char_pat( _temp574); goto _LL535;
_LL547: s= Cyc_Tcpat_float_pat( _temp576); goto _LL535; _LL549: s=( void*) Cyc_Tcpat_Any;
goto _LL535; _LL551:{ void* _temp602= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( p->topt))->v); struct Cyc_Absyn_PtrInfo _temp610; struct Cyc_Absyn_Conref*
_temp612; _LL604: if(( unsigned int) _temp602 >  4u?*(( int*) _temp602) ==  Cyc_Absyn_PointerType:
0){ _LL611: _temp610=(( struct Cyc_Absyn_PointerType_struct*) _temp602)->f1;
_LL613: _temp612= _temp610.nullable; goto _LL605;} else{ goto _LL606;} _LL606:
if(( unsigned int) _temp602 >  4u?*(( int*) _temp602) ==  Cyc_Absyn_TunionType:
0){ goto _LL607;} else{ goto _LL608;} _LL608: goto _LL609; _LL605: { int
is_nullable= 0; int still_working= 1; while( still_working) { void* _temp614=(
void*) _temp612->v; struct Cyc_Absyn_Conref* _temp622; int _temp624; _LL616: if((
unsigned int) _temp614 >  1u?*(( int*) _temp614) ==  Cyc_Absyn_Forward_constr: 0){
_LL623: _temp622=(( struct Cyc_Absyn_Forward_constr_struct*) _temp614)->f1; goto
_LL617;} else{ goto _LL618;} _LL618: if( _temp614 == ( void*) Cyc_Absyn_No_constr){
goto _LL619;} else{ goto _LL620;} _LL620: if(( unsigned int) _temp614 >  1u?*((
int*) _temp614) ==  Cyc_Absyn_Eq_constr: 0){ _LL625: _temp624=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp614)->f1; goto _LL621;} else{ goto _LL615;}
_LL617:( void*)( _temp612->v=( void*)(( void*) _temp622->v)); continue; _LL619:(
void*)( _temp612->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp626=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp626[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp627; _temp627.tag= Cyc_Absyn_Eq_constr;
_temp627.f1=( void*) 0; _temp627;}); _temp626;}))); is_nullable= 0;
still_working= 0; goto _LL615; _LL621: is_nullable=( int) _temp624;
still_working= 0; goto _LL615; _LL615:;}{ void* ss= Cyc_Tcpat_compile_pat(
_temp578); if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat( ss);} else{ s= Cyc_Tcpat_ptr_pat(
ss);} goto _LL603;}} _LL607:{ void* _temp628=( void*) _temp578->r; struct Cyc_List_List*
_temp634; struct Cyc_Absyn_Tunionfield* _temp636; struct Cyc_Absyn_Tuniondecl*
_temp638; _LL630: if(( unsigned int) _temp628 >  2u?*(( int*) _temp628) ==  Cyc_Absyn_Tunion_p:
0){ _LL639: _temp638=(( struct Cyc_Absyn_Tunion_p_struct*) _temp628)->f1; goto
_LL637; _LL637: _temp636=(( struct Cyc_Absyn_Tunion_p_struct*) _temp628)->f2;
goto _LL635; _LL635: _temp634=(( struct Cyc_Absyn_Tunion_p_struct*) _temp628)->f4;
goto _LL631;} else{ goto _LL632;} _LL632: goto _LL633; _LL631: { struct Cyc_Core_Opt*
span; if( _temp638->is_xtunion){ span= 0;} else{ span=({ struct Cyc_Core_Opt*
_temp640=( struct Cyc_Core_Opt*) _cycalloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp640->v=( void*)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp638->fields))->v);
_temp640;});} s= Cyc_Tcpat_con_pat(*(* _temp636->name).f2, span,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp634)); goto _LL629;} _LL633: s=({ void* _temp641[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("non-[x]tunion pattern has tunion type", sizeof( unsigned char), 38u),
_tag_arr( _temp641, sizeof( void*), 0u));}); goto _LL629; _LL629:;} goto _LL603;
_LL609: s=({ void* _temp642[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("pointer pattern does not have pointer type!",
sizeof( unsigned char), 44u), _tag_arr( _temp642, sizeof( void*), 0u));}); goto
_LL603; _LL603:;} goto _LL535; _LL553: { struct Cyc_Core_Opt* span;{ void*
_temp643= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( p->topt))->v);
_LL645: if(( unsigned int) _temp643 >  4u?*(( int*) _temp643) ==  Cyc_Absyn_TunionType:
0){ goto _LL646;} else{ goto _LL647;} _LL647: if(( unsigned int) _temp643 >  4u?*((
int*) _temp643) ==  Cyc_Absyn_TunionFieldType: 0){ goto _LL648;} else{ goto
_LL649;} _LL649: goto _LL650; _LL646: if( _temp584->is_xtunion){ span= 0;} else{
span=({ struct Cyc_Core_Opt* _temp651=( struct Cyc_Core_Opt*) _cycalloc_atomic(
sizeof( struct Cyc_Core_Opt)); _temp651->v=( void*)(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp584->fields))->v); _temp651;});} goto _LL644; _LL648: span=({
struct Cyc_Core_Opt* _temp652=( struct Cyc_Core_Opt*) _cycalloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp652->v=( void*) 1; _temp652;}); goto _LL644; _LL650:
span=({ void* _temp653[ 0u]={};(( struct Cyc_Core_Opt*(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("void tunion pattern has bad type",
sizeof( unsigned char), 33u), _tag_arr( _temp653, sizeof( void*), 0u));}); goto
_LL644; _LL644:;} s= Cyc_Tcpat_con_pat(*(* _temp582->name).f2, span, 0); goto
_LL535;} _LL555: _temp588= _temp586; goto _LL557; _LL557: s= Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp588)); goto _LL535; _LL559: {
struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp592->fields))->v; for( 0; fields !=  0;
fields= fields->tl){ int found= Cyc_Std_strcmp(*(( struct Cyc_Absyn_Structfield*)
fields->hd)->name, _tag_arr("", sizeof( unsigned char), 1u)) ==  0;{ struct Cyc_List_List*
dlps0= _temp590; for( 0; ! found? dlps0 !=  0: 0; dlps0= dlps0->tl){ struct
_tuple7 _temp656; struct Cyc_Absyn_Pat* _temp657; struct Cyc_List_List* _temp659;
struct _tuple7* _temp654=( struct _tuple7*) dlps0->hd; _temp656=* _temp654;
_LL660: _temp659= _temp656.f1; goto _LL658; _LL658: _temp657= _temp656.f2; goto
_LL655; _LL655: { struct Cyc_List_List* _temp661= _temp659; struct Cyc_List_List
_temp667; struct Cyc_List_List* _temp668; void* _temp670; struct _tagged_arr*
_temp672; _LL663: if( _temp661 ==  0){ goto _LL665;} else{ _temp667=* _temp661;
_LL671: _temp670=( void*) _temp667.hd; if(*(( int*) _temp670) ==  Cyc_Absyn_FieldName){
_LL673: _temp672=(( struct Cyc_Absyn_FieldName_struct*) _temp670)->f1; goto
_LL669;} else{ goto _LL665;} _LL669: _temp668= _temp667.tl; if( _temp668 ==  0){
goto _LL664;} else{ goto _LL665;}} _LL665: goto _LL666; _LL664: if( Cyc_Std_zstrptrcmp(
_temp672,(( struct Cyc_Absyn_Structfield*) fields->hd)->name) ==  0){ ps=({
struct Cyc_List_List* _temp674=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp674->hd=( void*) Cyc_Tcpat_compile_pat( _temp657);
_temp674->tl= ps; _temp674;}); found= 1;} goto _LL662; _LL666:({ void* _temp675[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bad designator(s)", sizeof( unsigned char), 18u), _tag_arr( _temp675,
sizeof( void*), 0u));}); goto _LL662; _LL662:;}}} if( ! found){({ void* _temp676[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bad designator", sizeof( unsigned char), 15u), _tag_arr( _temp676,
sizeof( void*), 0u));});}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL535;} _LL561: {
int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp596->fields))->v); s= Cyc_Tcpat_con_pat(*(*
_temp594->name).f2,({ struct Cyc_Core_Opt* _temp677=( struct Cyc_Core_Opt*)
_cycalloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp677->v=( void*) span;
_temp677;}), 0); goto _LL535;} _LL563: { struct Cyc_List_List* fields;{ void*
_temp678= Cyc_Tcutil_compress( _temp600); struct Cyc_List_List* _temp684; _LL680:
if(( unsigned int) _temp678 >  4u?*(( int*) _temp678) ==  Cyc_Absyn_AnonEnumType:
0){ _LL685: _temp684=(( struct Cyc_Absyn_AnonEnumType_struct*) _temp678)->f1;
goto _LL681;} else{ goto _LL682;} _LL682: goto _LL683; _LL681: fields= _temp684;
goto _LL679; _LL683: fields=({ void* _temp686[ 0u]={};(( struct Cyc_List_List*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("bad type in AnonEnum_p",
sizeof( unsigned char), 23u), _tag_arr( _temp686, sizeof( void*), 0u));}); goto
_LL679; _LL679:;}{ int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
fields); s= Cyc_Tcpat_con_pat(*(* _temp598->name).f2,({ struct Cyc_Core_Opt*
_temp687=( struct Cyc_Core_Opt*) _cycalloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp687->v=( void*) span; _temp687;}), 0); goto _LL535;}} _LL565: s=({ void*
_temp688[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("compile_pat: unknownid", sizeof(
unsigned char), 23u), _tag_arr( _temp688, sizeof( void*), 0u));}); goto _LL535;
_LL567: s=({ void* _temp689[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("compile_pat: unknowncall",
sizeof( unsigned char), 25u), _tag_arr( _temp689, sizeof( void*), 0u));}); goto
_LL535; _LL569: s=({ void* _temp690[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("compile_pat: unknownfields",
sizeof( unsigned char), 27u), _tag_arr( _temp690, sizeof( void*), 0u));}); goto
_LL535; _LL535:;} return s;} static const int Cyc_Tcpat_Pos= 0; struct Cyc_Tcpat_Pos_struct{
int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Tcpat_Neg= 1; struct Cyc_Tcpat_Neg_struct{ int tag; struct Cyc_Set_Set*
f1; } ; static const int Cyc_Tcpat_Failure= 0; static const int Cyc_Tcpat_Success=
0; struct Cyc_Tcpat_Success_struct{ int tag; void* f1; } ; static const int Cyc_Tcpat_IfEq=
1; struct Cyc_Tcpat_IfEq_struct{ int tag; struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s*
f2; void* f3; void* f4; } ; struct _tuple10{ struct Cyc_List_List* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; static const int Cyc_Tcpat_Yes=
0; static const int Cyc_Tcpat_No= 1; static const int Cyc_Tcpat_Maybe= 2; static
void* Cyc_Tcpat_add_neg( void* td, struct Cyc_Tcpat_Con_s* c){ void* _temp691=
td; struct Cyc_Set_Set* _temp697; _LL693: if(*(( int*) _temp691) ==  Cyc_Tcpat_Neg){
_LL698: _temp697=(( struct Cyc_Tcpat_Neg_struct*) _temp691)->f1; goto _LL694;}
else{ goto _LL695;} _LL695: if(*(( int*) _temp691) ==  Cyc_Tcpat_Pos){ goto
_LL696;} else{ goto _LL692;} _LL694: if((( int(*)( struct Cyc_Set_Set* s, struct
Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp697, c)){({ void* _temp699[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("add_neg called when constructor already in set", sizeof( unsigned char),
47u), _tag_arr( _temp699, sizeof( void*), 0u));});} if( c->span !=  0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp697) +  1 >= ( int)(( struct
Cyc_Core_Opt*) _check_null( c->span))->v: 0){({ void* _temp700[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("add_neg called when |cs U {c}| >= span(c)",
sizeof( unsigned char), 42u), _tag_arr( _temp700, sizeof( void*), 0u));});}
return( void*)({ struct Cyc_Tcpat_Neg_struct* _temp701=( struct Cyc_Tcpat_Neg_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp701[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp702; _temp702.tag= Cyc_Tcpat_Neg; _temp702.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp697,
c); _temp702;}); _temp701;}); _LL696: return({ void* _temp703[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("add_neg called when td is Positive", sizeof( unsigned char), 35u),
_tag_arr( _temp703, sizeof( void*), 0u));}); _LL692:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp704= td; struct Cyc_Tcpat_Con_s*
_temp710; struct Cyc_Set_Set* _temp712; _LL706: if(*(( int*) _temp704) ==  Cyc_Tcpat_Pos){
_LL711: _temp710=(( struct Cyc_Tcpat_Pos_struct*) _temp704)->f1; goto _LL707;}
else{ goto _LL708;} _LL708: if(*(( int*) _temp704) ==  Cyc_Tcpat_Neg){ _LL713:
_temp712=(( struct Cyc_Tcpat_Neg_struct*) _temp704)->f1; goto _LL709;} else{
goto _LL705;} _LL707: if( Cyc_Tcpat_compare_con( c, _temp710) ==  0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL709: if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp712,
c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span !=  0?( int)(( struct Cyc_Core_Opt*)
_check_null( c->span))->v == (( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)(
_temp712) +  1: 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}}
_LL705:;} struct _tuple11{ struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2;
} ; static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt,
void* dsc){ struct Cyc_List_List* _temp714= ctxt; struct Cyc_List_List _temp720;
struct Cyc_List_List* _temp721; struct _tuple11* _temp723; struct _tuple11
_temp725; struct Cyc_List_List* _temp726; struct Cyc_Tcpat_Con_s* _temp728;
_LL716: if( _temp714 ==  0){ goto _LL717;} else{ goto _LL718;} _LL718: if(
_temp714 ==  0){ goto _LL715;} else{ _temp720=* _temp714; _LL724: _temp723=(
struct _tuple11*) _temp720.hd; _temp725=* _temp723; _LL729: _temp728= _temp725.f1;
goto _LL727; _LL727: _temp726= _temp725.f2; goto _LL722; _LL722: _temp721=
_temp720.tl; goto _LL719;} _LL717: return 0; _LL719: return({ struct Cyc_List_List*
_temp730=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp730->hd=( void*)({ struct _tuple11* _temp731=( struct _tuple11*) _cycalloc(
sizeof( struct _tuple11)); _temp731->f1= _temp728; _temp731->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp732=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp732->hd=( void*) dsc; _temp732->tl= _temp726;
_temp732;}); _temp731;}); _temp730->tl= _temp721; _temp730;}); _LL715:;} static
struct Cyc_List_List* Cyc_Tcpat_norm_context( struct Cyc_List_List* ctxt){
struct Cyc_List_List* _temp733= ctxt; struct Cyc_List_List _temp739; struct Cyc_List_List*
_temp740; struct _tuple11* _temp742; struct _tuple11 _temp744; struct Cyc_List_List*
_temp745; struct Cyc_Tcpat_Con_s* _temp747; _LL735: if( _temp733 ==  0){ goto
_LL736;} else{ goto _LL737;} _LL737: if( _temp733 ==  0){ goto _LL734;} else{
_temp739=* _temp733; _LL743: _temp742=( struct _tuple11*) _temp739.hd; _temp744=*
_temp742; _LL748: _temp747= _temp744.f1; goto _LL746; _LL746: _temp745= _temp744.f2;
goto _LL741; _LL741: _temp740= _temp739.tl; goto _LL738;} _LL736: return({ void*
_temp749[ 0u]={};(( struct Cyc_List_List*(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("norm_context: empty context",
sizeof( unsigned char), 28u), _tag_arr( _temp749, sizeof( void*), 0u));});
_LL738: return Cyc_Tcpat_augment( _temp740,( void*)({ struct Cyc_Tcpat_Pos_struct*
_temp750=( struct Cyc_Tcpat_Pos_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_Pos_struct));
_temp750[ 0]=({ struct Cyc_Tcpat_Pos_struct _temp751; _temp751.tag= Cyc_Tcpat_Pos;
_temp751.f1= _temp747; _temp751.f2= Cyc_List_rev( _temp745); _temp751;});
_temp750;})); _LL734:;} static void* Cyc_Tcpat_build_desc( struct Cyc_List_List*
ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple6 _temp753=({ struct
_tuple6 _temp752; _temp752.f1= ctxt; _temp752.f2= work; _temp752;}); struct Cyc_List_List*
_temp763; struct Cyc_List_List* _temp765; struct Cyc_List_List* _temp767; struct
Cyc_List_List* _temp769; struct Cyc_List_List* _temp771; struct Cyc_List_List
_temp773; struct Cyc_List_List* _temp774; struct _tuple10* _temp776; struct
_tuple10 _temp778; struct Cyc_List_List* _temp779; struct Cyc_List_List*
_temp781; struct Cyc_List_List _temp783; struct Cyc_List_List* _temp784; struct
_tuple11* _temp786; struct _tuple11 _temp788; struct Cyc_List_List* _temp789;
struct Cyc_Tcpat_Con_s* _temp791; _LL755: _LL766: _temp765= _temp753.f1; if(
_temp765 ==  0){ goto _LL764;} else{ goto _LL757;} _LL764: _temp763= _temp753.f2;
if( _temp763 ==  0){ goto _LL756;} else{ goto _LL757;} _LL757: _LL768: _temp767=
_temp753.f1; if( _temp767 ==  0){ goto _LL758;} else{ goto _LL759;} _LL759:
_LL770: _temp769= _temp753.f2; if( _temp769 ==  0){ goto _LL760;} else{ goto
_LL761;} _LL761: _LL782: _temp781= _temp753.f1; if( _temp781 ==  0){ goto _LL754;}
else{ _temp783=* _temp781; _LL787: _temp786=( struct _tuple11*) _temp783.hd;
_temp788=* _temp786; _LL792: _temp791= _temp788.f1; goto _LL790; _LL790:
_temp789= _temp788.f2; goto _LL785; _LL785: _temp784= _temp783.tl; goto _LL772;}
_LL772: _temp771= _temp753.f2; if( _temp771 ==  0){ goto _LL754;} else{ _temp773=*
_temp771; _LL777: _temp776=( struct _tuple10*) _temp773.hd; _temp778=* _temp776;
_LL780: _temp779= _temp778.f3; goto _LL775; _LL775: _temp774= _temp773.tl; goto
_LL762;} _LL756: return dsc; _LL758: return({ void* _temp793[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("build_desc: ctxt and work don't match", sizeof( unsigned char), 38u),
_tag_arr( _temp793, sizeof( void*), 0u));}); _LL760: return({ void* _temp794[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("build_desc: ctxt and work don't match", sizeof(
unsigned char), 38u), _tag_arr( _temp794, sizeof( void*), 0u));}); _LL762: {
struct Cyc_Tcpat_Pos_struct* _temp795=({ struct Cyc_Tcpat_Pos_struct* _temp796=(
struct Cyc_Tcpat_Pos_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_Pos_struct));
_temp796[ 0]=({ struct Cyc_Tcpat_Pos_struct _temp797; _temp797.tag= Cyc_Tcpat_Pos;
_temp797.f1= _temp791; _temp797.f2= Cyc_List_append( Cyc_List_rev( _temp789),({
struct Cyc_List_List* _temp798=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp798->hd=( void*) dsc; _temp798->tl= _temp779;
_temp798;})); _temp797;}); _temp796;}); return Cyc_Tcpat_build_desc( _temp784,(
void*) _temp795, _temp774);} _LL754:;} static void* Cyc_Tcpat_match( void* p,
struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp799=
allmrules; struct Cyc_List_List _temp805; struct Cyc_List_List* _temp806; struct
_tuple0* _temp808; struct _tuple0 _temp810; void* _temp811; void* _temp813;
_LL801: if( _temp799 ==  0){ goto _LL802;} else{ goto _LL803;} _LL803: if(
_temp799 ==  0){ goto _LL800;} else{ _temp805=* _temp799; _LL809: _temp808=(
struct _tuple0*) _temp805.hd; _temp810=* _temp808; _LL814: _temp813= _temp810.f1;
goto _LL812; _LL812: _temp811= _temp810.f2; goto _LL807; _LL807: _temp806=
_temp805.tl; goto _LL804;} _LL802: return( void*) Cyc_Tcpat_Failure; _LL804:
return Cyc_Tcpat_match( _temp813, 0, dsc, 0, 0, _temp811, _temp806); _LL800:;}
static void* Cyc_Tcpat_match_compile( struct Cyc_List_List* allmrules){ return
Cyc_Tcpat_or_match(( void*)({ struct Cyc_Tcpat_Neg_struct* _temp815=( struct Cyc_Tcpat_Neg_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp815[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp816; _temp816.tag= Cyc_Tcpat_Neg; _temp816.f1= Cyc_Tcpat_empty_con_set();
_temp816;}); _temp815;}), allmrules);} static void* Cyc_Tcpat_and_match( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules){ struct Cyc_List_List* _temp817= work; struct Cyc_List_List
_temp825; struct Cyc_List_List* _temp826; struct _tuple10* _temp828; struct
_tuple10 _temp830; struct Cyc_List_List* _temp831; struct Cyc_List_List*
_temp833; struct Cyc_List_List* _temp835; struct Cyc_List_List _temp837; struct
Cyc_List_List* _temp838; struct _tuple10* _temp840; struct _tuple10 _temp842;
struct Cyc_List_List* _temp843; struct Cyc_List_List* _temp845; struct Cyc_List_List*
_temp847; _LL819: if( _temp817 ==  0){ goto _LL820;} else{ goto _LL821;} _LL821:
if( _temp817 ==  0){ goto _LL823;} else{ _temp825=* _temp817; _LL829: _temp828=(
struct _tuple10*) _temp825.hd; _temp830=* _temp828; _LL836: _temp835= _temp830.f1;
if( _temp835 ==  0){ goto _LL834;} else{ goto _LL823;} _LL834: _temp833=
_temp830.f2; if( _temp833 ==  0){ goto _LL832;} else{ goto _LL823;} _LL832:
_temp831= _temp830.f3; if( _temp831 ==  0){ goto _LL827;} else{ goto _LL823;}
_LL827: _temp826= _temp825.tl; goto _LL822;} _LL823: if( _temp817 ==  0){ goto
_LL818;} else{ _temp837=* _temp817; _LL841: _temp840=( struct _tuple10*)
_temp837.hd; _temp842=* _temp840; _LL848: _temp847= _temp842.f1; goto _LL846;
_LL846: _temp845= _temp842.f2; goto _LL844; _LL844: _temp843= _temp842.f3; goto
_LL839; _LL839: _temp838= _temp837.tl; goto _LL824;} _LL820: return( void*)({
struct Cyc_Tcpat_Success_struct* _temp849=( struct Cyc_Tcpat_Success_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Success_struct)); _temp849[ 0]=({ struct Cyc_Tcpat_Success_struct
_temp850; _temp850.tag= Cyc_Tcpat_Success; _temp850.f1=( void*) right_hand_side;
_temp850;}); _temp849;}); _LL822: return Cyc_Tcpat_and_match( Cyc_Tcpat_norm_context(
ctx), _temp826, right_hand_side, rules); _LL824: if(( _temp847 ==  0? 1:
_temp845 ==  0)? 1: _temp843 ==  0){ return({ void* _temp851[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("tcpat:and_match: malformed work frame", sizeof( unsigned char), 38u),
_tag_arr( _temp851, sizeof( void*), 0u));});}{ struct Cyc_List_List _temp854;
struct Cyc_List_List* _temp855; void* _temp857; struct Cyc_List_List* _temp852=(
struct Cyc_List_List*) _check_null( _temp847); _temp854=* _temp852; _LL858:
_temp857=( void*) _temp854.hd; goto _LL856; _LL856: _temp855= _temp854.tl; goto
_LL853; _LL853: { struct Cyc_List_List _temp861; struct Cyc_List_List* _temp862;
struct Cyc_List_List* _temp864; struct Cyc_List_List* _temp859=( struct Cyc_List_List*)
_check_null( _temp845); _temp861=* _temp859; _LL865: _temp864=( struct Cyc_List_List*)
_temp861.hd; goto _LL863; _LL863: _temp862= _temp861.tl; goto _LL860; _LL860: {
struct Cyc_List_List _temp868; struct Cyc_List_List* _temp869; void* _temp871;
struct Cyc_List_List* _temp866=( struct Cyc_List_List*) _check_null( _temp843);
_temp868=* _temp866; _LL872: _temp871=( void*) _temp868.hd; goto _LL870; _LL870:
_temp869= _temp868.tl; goto _LL867; _LL867: { struct _tuple10* _temp873=({
struct _tuple10* _temp875=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10));
_temp875->f1= _temp855; _temp875->f2= _temp862; _temp875->f3= _temp869; _temp875;});
return Cyc_Tcpat_match( _temp857, _temp864, _temp871, ctx,({ struct Cyc_List_List*
_temp874=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp874->hd=( void*) _temp873; _temp874->tl= _temp838; _temp874;}),
right_hand_side, rules);}}}} _LL818:;} static struct Cyc_List_List* Cyc_Tcpat_getdargs(
struct Cyc_Tcpat_Con_s* pcon, void* dsc){ void* _temp876= dsc; struct Cyc_Set_Set*
_temp882; struct Cyc_List_List* _temp884; _LL878: if(*(( int*) _temp876) ==  Cyc_Tcpat_Neg){
_LL883: _temp882=(( struct Cyc_Tcpat_Neg_struct*) _temp876)->f1; goto _LL879;}
else{ goto _LL880;} _LL880: if(*(( int*) _temp876) ==  Cyc_Tcpat_Pos){ _LL885:
_temp884=(( struct Cyc_Tcpat_Pos_struct*) _temp876)->f2; goto _LL881;} else{
goto _LL877;} _LL879: { void* any=( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp888=( struct Cyc_Tcpat_Neg_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp888[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp889; _temp889.tag= Cyc_Tcpat_Neg;
_temp889.f1= Cyc_Tcpat_empty_con_set(); _temp889;}); _temp888;}); struct Cyc_List_List*
_temp886= 0;{ int i= 0; for( 0; i <  pcon->arity; ++ i){ _temp886=({ struct Cyc_List_List*
_temp887=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp887->hd=( void*) any; _temp887->tl= _temp886; _temp887;});}} return
_temp886;} _LL881: return _temp884; _LL877:;} static struct Cyc_List_List* Cyc_Tcpat_getoarg(
struct Cyc_List_List* obj, int i){ return({ struct Cyc_List_List* _temp890=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp890->hd=(
void*)( i +  1); _temp890->tl= obj; _temp890;});} static struct Cyc_List_List*
Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s* pcon, struct Cyc_List_List* obj){
return(( struct Cyc_List_List*(*)( int n, struct Cyc_List_List*(* f)( struct Cyc_List_List*,
int), struct Cyc_List_List* env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg,
obj);} static void* Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj, void*
dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules){ void* _temp891= p; struct Cyc_List_List*
_temp897; struct Cyc_Tcpat_Con_s* _temp899; _LL893: if( _temp891 == ( void*) Cyc_Tcpat_Any){
goto _LL894;} else{ goto _LL895;} _LL895: if(( unsigned int) _temp891 >  1u?*((
int*) _temp891) ==  Cyc_Tcpat_Con: 0){ _LL900: _temp899=(( struct Cyc_Tcpat_Con_struct*)
_temp891)->f1; goto _LL898; _LL898: _temp897=(( struct Cyc_Tcpat_Con_struct*)
_temp891)->f2; goto _LL896;} else{ goto _LL892;} _LL894: return Cyc_Tcpat_and_match(
Cyc_Tcpat_augment( ctx, dsc), work, right_hand_side, rules); _LL896: { void*
_temp901= Cyc_Tcpat_static_match( _temp899, dsc); _LL903: if( _temp901 == ( void*)
Cyc_Tcpat_Yes){ goto _LL904;} else{ goto _LL905;} _LL905: if( _temp901 == ( void*)
Cyc_Tcpat_No){ goto _LL906;} else{ goto _LL907;} _LL907: if( _temp901 == ( void*)
Cyc_Tcpat_Maybe){ goto _LL908;} else{ goto _LL902;} _LL904: { struct Cyc_List_List*
_temp909=({ struct Cyc_List_List* _temp914=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp914->hd=( void*)({ struct _tuple11*
_temp915=( struct _tuple11*) _cycalloc( sizeof( struct _tuple11)); _temp915->f1=
_temp899; _temp915->f2= 0; _temp915;}); _temp914->tl= ctx; _temp914;}); struct
_tuple10* _temp910=({ struct _tuple10* _temp913=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp913->f1= _temp897; _temp913->f2= Cyc_Tcpat_getoargs(
_temp899, obj); _temp913->f3= Cyc_Tcpat_getdargs( _temp899, dsc); _temp913;});
struct Cyc_List_List* _temp911=({ struct Cyc_List_List* _temp912=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp912->hd=( void*) _temp910;
_temp912->tl= work; _temp912;}); return Cyc_Tcpat_and_match( _temp909, _temp911,
right_hand_side, rules);} _LL906: return Cyc_Tcpat_or_match( Cyc_Tcpat_build_desc(
ctx, dsc, work), rules); _LL908: { struct Cyc_List_List* _temp916=({ struct Cyc_List_List*
_temp925=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp925->hd=( void*)({ struct _tuple11* _temp926=( struct _tuple11*) _cycalloc(
sizeof( struct _tuple11)); _temp926->f1= _temp899; _temp926->f2= 0; _temp926;});
_temp925->tl= ctx; _temp925;}); struct _tuple10* _temp917=({ struct _tuple10*
_temp924=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp924->f1=
_temp897; _temp924->f2= Cyc_Tcpat_getoargs( _temp899, obj); _temp924->f3= Cyc_Tcpat_getdargs(
_temp899, dsc); _temp924;}); struct Cyc_List_List* _temp918=({ struct Cyc_List_List*
_temp923=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp923->hd=( void*) _temp917; _temp923->tl= work; _temp923;}); void* _temp919=
Cyc_Tcpat_and_match( _temp916, _temp918, right_hand_side, rules); void* _temp920=
Cyc_Tcpat_or_match( Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg( dsc, _temp899),
work), rules); return( void*)({ struct Cyc_Tcpat_IfEq_struct* _temp921=( struct
Cyc_Tcpat_IfEq_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp921[ 0]=({ struct Cyc_Tcpat_IfEq_struct _temp922; _temp922.tag= Cyc_Tcpat_IfEq;
_temp922.f1= obj; _temp922.f2= _temp899; _temp922.f3=( void*) _temp919; _temp922.f4=(
void*) _temp920; _temp922;}); _temp921;});} _LL902:;} _LL892:;} static void Cyc_Tcpat_check_exhaust_overlap(
void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)( void*,
void*), void* env2){ void* _temp927= d; void* _temp935; void* _temp937; void*
_temp939; _LL929: if( _temp927 == ( void*) Cyc_Tcpat_Failure){ goto _LL930;}
else{ goto _LL931;} _LL931: if(( unsigned int) _temp927 >  1u?*(( int*) _temp927)
==  Cyc_Tcpat_Success: 0){ _LL936: _temp935=( void*)(( struct Cyc_Tcpat_Success_struct*)
_temp927)->f1; goto _LL932;} else{ goto _LL933;} _LL933: if(( unsigned int)
_temp927 >  1u?*(( int*) _temp927) ==  Cyc_Tcpat_IfEq: 0){ _LL940: _temp939=(
void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp927)->f3; goto _LL938; _LL938:
_temp937=( void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp927)->f4; goto _LL934;}
else{ goto _LL928;} _LL930: not_exhaust( env1); goto _LL928; _LL932: rhs_appears(
env2, _temp935); goto _LL928; _LL934: Cyc_Tcpat_check_exhaust_overlap( _temp939,
not_exhaust, env1, rhs_appears, env2); Cyc_Tcpat_check_exhaust_overlap( _temp937,
not_exhaust, env1, rhs_appears, env2); goto _LL928; _LL928:;} struct _tuple12{
int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple13{ void* f1; struct
_tuple12* f2; } ; static struct _tuple13* Cyc_Tcpat_get_match( int* r, struct
Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern);
struct _tuple12* rhs=({ struct _tuple12* _temp946=( struct _tuple12*) _cycalloc(
sizeof( struct _tuple12)); _temp946->f1= 0; _temp946->f2=( swc->pattern)->loc;
_temp946;}); void* sp; if( swc->where_clause !=  0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp941=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp941->hd=( void*) sp0; _temp941->tl=({ struct Cyc_List_List*
_temp942=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp942->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp942->tl= 0; _temp942;});
_temp941;}));* r=* r +  1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp943=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp943->hd=( void*) sp0; _temp943->tl=({ struct Cyc_List_List* _temp944=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp944->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp944->tl= 0; _temp944;}); _temp943;}));}
return({ struct _tuple13* _temp945=( struct _tuple13*) _cycalloc( sizeof( struct
_tuple13)); _temp945->f1= sp; _temp945->f2= rhs; _temp945;});} static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment* loc){({ void* _temp947[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("patterns may not be exhaustive", sizeof( unsigned char), 31u),
_tag_arr( _temp947, sizeof( void*), 0u));});} static void Cyc_Tcpat_rule_occurs(
int dummy, struct _tuple12* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int _temp948= 0;
struct Cyc_List_List* _temp949=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)(
int*, struct Cyc_Absyn_Switch_clause*), int* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcpat_get_match,& _temp948, swcs); void* _temp950=(( void*(*)( struct Cyc_List_List*
allmrules)) Cyc_Tcpat_match_compile)( _temp949);(( void(*)( void* d, void(*
not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment* env1,
void(* rhs_appears)( int, struct _tuple12*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp950, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp949 !=  0; _temp949= _temp949->tl){ struct _tuple13 _temp953; struct
_tuple12* _temp954; struct _tuple12 _temp956; struct Cyc_Position_Segment*
_temp957; int _temp959; struct _tuple13* _temp951=( struct _tuple13*) _temp949->hd;
_temp953=* _temp951; _LL955: _temp954= _temp953.f2; _temp956=* _temp954; _LL960:
_temp959= _temp956.f1; goto _LL958; _LL958: _temp957= _temp956.f2; goto _LL952;
_LL952: if( ! _temp959){({ void* _temp961[ 0u]={}; Cyc_Tcutil_terr( _temp957,
_tag_arr("redundant pattern", sizeof( unsigned char), 18u), _tag_arr( _temp961,
sizeof( void*), 0u));});}}} struct _tuple14{ struct Cyc_Position_Segment* f1;
int f2; } ; static void Cyc_Tcpat_not_exhaust_warn( struct _tuple14* pr){(* pr).f2=
0;({ void* _temp962[ 0u]={}; Cyc_Tcutil_warn((* pr).f1, _tag_arr("pattern not exhaustive",
sizeof( unsigned char), 23u), _tag_arr( _temp962, sizeof( void*), 0u));});}
static void Cyc_Tcpat_dummy_fn( int i, int j){ return;} struct _tuple15{ void*
f1; int f2; } ; int Cyc_Tcpat_check_let_pat_exhaustive( struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List* _temp963=({ struct Cyc_List_List*
_temp967=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp967->hd=( void*)({ struct _tuple15* _temp968=( struct _tuple15*) _cycalloc(
sizeof( struct _tuple15)); _temp968->f1= Cyc_Tcpat_compile_pat( p); _temp968->f2=
0; _temp968;}); _temp967->tl= 0; _temp967;}); void* _temp964=(( void*(*)( struct
Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)( _temp963); struct _tuple14*
_temp965=({ struct _tuple14* _temp966=( struct _tuple14*) _cycalloc( sizeof(
struct _tuple14)); _temp966->f1= loc; _temp966->f2= 1; _temp966;});(( void(*)(
void* d, void(* not_exhaust)( struct _tuple14*), struct _tuple14* env1, void(*
rhs_appears)( int, int), int env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp964,
Cyc_Tcpat_not_exhaust_warn, _temp965, Cyc_Tcpat_dummy_fn, 0); return(* _temp965).f2;}
static struct _tuple13* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple12* rhs=({
struct _tuple12* _temp970=( struct _tuple12*) _cycalloc( sizeof( struct _tuple12));
_temp970->f1= 0; _temp970->f2=( swc->pattern)->loc; _temp970;}); return({ struct
_tuple13* _temp969=( struct _tuple13*) _cycalloc( sizeof( struct _tuple13));
_temp969->f1= sp0; _temp969->f2= rhs; _temp969;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
_temp971=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
_temp972=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
_temp971);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple12*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp972, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; _temp971 !=  0; _temp971= _temp971->tl){
struct _tuple13 _temp975; struct _tuple12* _temp976; struct _tuple12 _temp978;
struct Cyc_Position_Segment* _temp979; int _temp981; struct _tuple13* _temp973=(
struct _tuple13*) _temp971->hd; _temp975=* _temp973; _LL977: _temp976= _temp975.f2;
_temp978=* _temp976; _LL982: _temp981= _temp978.f1; goto _LL980; _LL980:
_temp979= _temp978.f2; goto _LL974; _LL974: if( ! _temp981){({ void* _temp983[ 0u]={};
Cyc_Tcutil_terr( _temp979, _tag_arr("redundant pattern", sizeof( unsigned char),
18u), _tag_arr( _temp983, sizeof( void*), 0u));});}}}
