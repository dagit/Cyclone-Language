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
void* hd; struct Cyc_List_List* tl; } ; extern struct Cyc_List_List* Cyc_List_list(
struct _tagged_arr); extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x);
extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Set_Set; extern unsigned char
Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[
12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern struct Cyc_Dict_Dict*
Cyc_Dict_empty( int(* cmp)( void*, void*)); extern struct Cyc_Dict_Dict* Cyc_Dict_insert(
struct Cyc_Dict_Dict* d, void* k, void* v); extern struct Cyc_Dict_Dict* Cyc_Dict_insert_new(
struct Cyc_Dict_Dict* d, void* k, void* v); extern void* Cyc_Dict_lookup( struct
Cyc_Dict_Dict* d, void* k); extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); extern void* Cyc_Std___assert_fail( struct _tagged_arr
assertion, struct _tagged_arr file, unsigned int line); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int
Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e=
22; struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Tuple_e= 23; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_CompoundLit_e=
24; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
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
15u]; extern struct _tagged_arr* Cyc_Absyn_fieldname( int); struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; static const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_compress( void* t); extern
int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b); extern unsigned int
Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); struct Cyc_CfFlowInfo_Place;
enum  Cyc_CfFlowInfo_Escaped; struct Cyc_CfFlowInfo_InitState; static const int
Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct
Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct
Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List* fields; } ; enum  Cyc_CfFlowInfo_Escaped{
Cyc_CfFlowInfo_Esc  =  0u, Cyc_CfFlowInfo_Unesc  =  1u}; static const int Cyc_CfFlowInfo_NoneIL=
0; static const int Cyc_CfFlowInfo_ThisIL= 1; static const int Cyc_CfFlowInfo_AllIL=
2; static const int Cyc_CfFlowInfo_MustPointTo= 0; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; struct Cyc_CfFlowInfo_InitState{
enum  Cyc_CfFlowInfo_Escaped esc; void* level; } ; static const int Cyc_CfFlowInfo_LeafPI=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; struct Cyc_CfFlowInfo_InitState
f1; } ; static const int Cyc_CfFlowInfo_DictPI= 1; struct Cyc_CfFlowInfo_DictPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_BottomFL=
0; static const int Cyc_CfFlowInfo_InitsFL= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; extern int Cyc_CfFlowInfo_local_root_cmp(
void*, void*); extern void* Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( void* t);
extern void* Cyc_CfFlowInfo_assign_unknown_dict( void*, void*); extern void* Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**, void*, void*); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt*
enclosee, struct Cyc_Absyn_Stmt* encloser); struct Cyc_NewControlFlow_AnalEnv{
struct Cyc_Dict_Dict* roots; int in_try; void* tryflow; struct Cyc_Set_Set**
all_changed; } ; extern void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv*
env, void* new_flow); extern void* Cyc_NewControlFlow_cf_analyze_stmt( struct
Cyc_NewControlFlow_AnalEnv*, struct Cyc_Absyn_Stmt*, void*); extern void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); extern unsigned char Cyc_CfAbsexp_BadAbsexp[ 14u];
static const int Cyc_CfAbsexp_OrderedG= 0; static const int Cyc_CfAbsexp_UnorderedG=
1; static const int Cyc_CfAbsexp_OneofG= 2; extern void* Cyc_CfAbsexp_mkUnknownOp();
extern void* Cyc_CfAbsexp_mkLocalOp( struct Cyc_Absyn_Vardecl*); extern void*
Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp*); extern void* Cyc_CfAbsexp_mkDictOp(
struct Cyc_Dict_Dict*); extern void* Cyc_CfAbsexp_mkAddressOp( void*); extern
void* Cyc_CfAbsexp_mkMemberOp( void*, struct _tagged_arr* f); extern void* Cyc_CfAbsexp_mkDerefOp(
void*); extern void* Cyc_CfAbsexp_mkBottomAE(); extern void* Cyc_CfAbsexp_mkSkipAE();
extern void* Cyc_CfAbsexp_mkUseAE( void* ao); extern void* Cyc_CfAbsexp_mkAssignAE(
void* l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e);
extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s); extern void* Cyc_CfAbsexp_mkComprehensionAE(
struct Cyc_Absyn_Vardecl* vd, void* size_true_exp, void* size_false_exp, void*
size_aop, void* body_exp, void* body_op); extern void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2); extern void* Cyc_CfAbsexp_mkGroupAE_l( void* g,
struct Cyc_List_List* ael); extern void* Cyc_CfAbsexp_eval_absexp( struct Cyc_NewControlFlow_AnalEnv*
env, struct Cyc_Position_Segment* loc, void* ae, void* in_flow); struct Cyc_PP_Ppstate;
struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int
expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; struct Cyc_NewControlFlow_CFStmtAnnot{ struct Cyc_Absyn_Stmt*
encloser; int visited; void* flow; struct Cyc_List_List* absexps; } ;
unsigned char Cyc_NewControlFlow_CFAnnot[ 12u]="\000\000\000\000CFAnnot"; struct
Cyc_NewControlFlow_CFAnnot_struct{ unsigned char* tag; struct Cyc_NewControlFlow_CFStmtAnnot
f1; } ; void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser){( void*)( enclosee->annot=( void*)(( void*)({
struct Cyc_NewControlFlow_CFAnnot_struct* _temp0=( struct Cyc_NewControlFlow_CFAnnot_struct*)
_cycalloc( sizeof( struct Cyc_NewControlFlow_CFAnnot_struct)); _temp0[ 0]=({
struct Cyc_NewControlFlow_CFAnnot_struct _temp1; _temp1.tag= Cyc_NewControlFlow_CFAnnot;
_temp1.f1=({ struct Cyc_NewControlFlow_CFStmtAnnot _temp2; _temp2.encloser=
encloser; _temp2.visited= 0; _temp2.flow=( void*)(( void*) Cyc_CfFlowInfo_BottomFL);
_temp2.absexps= 0; _temp2;}); _temp1;}); _temp0;})));} static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_get_stmt_annot( struct Cyc_Absyn_Stmt* s){ void* _temp3=(
void*) s->annot; struct Cyc_NewControlFlow_CFStmtAnnot _temp9; struct Cyc_NewControlFlow_CFStmtAnnot*
_temp11; _LL5: if(*(( void**) _temp3) ==  Cyc_NewControlFlow_CFAnnot){ _LL10:
_temp9=(( struct Cyc_NewControlFlow_CFAnnot_struct*) _temp3)->f1; _temp11=(
struct Cyc_NewControlFlow_CFStmtAnnot*)&(( struct Cyc_NewControlFlow_CFAnnot_struct*)
_temp3)->f1; goto _LL6;} else{ goto _LL7;} _LL7: goto _LL8; _LL6: return _temp11;
_LL8:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp12=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp12[ 0]=({ struct Cyc_Core_Impossible_struct _temp13; _temp13.tag= Cyc_Core_Impossible;
_temp13.f1= _tag_arr("ControlFlow -- wrong stmt annotation", sizeof(
unsigned char), 37u); _temp13;}); _temp12;})); _LL4:;} static void Cyc_NewControlFlow_cf_set_absexps(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* ael){( Cyc_NewControlFlow_get_stmt_annot(
s))->absexps= ael;} struct Cyc_NewControlFlow_AbsEnv{ struct Cyc_Dict_Dict*
all_roots; } ; struct Cyc_NewControlFlow_AbsSyn{ void* when_true_exp; void*
when_false_exp; void* absop; } ; static void Cyc_NewControlFlow_add_var_root(
struct Cyc_NewControlFlow_AbsEnv* env, struct Cyc_Absyn_Vardecl* vd){ env->all_roots=
Cyc_Dict_insert_new( env->all_roots,( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp14=( struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp14[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp15; _temp15.tag= Cyc_CfFlowInfo_VarRoot;
_temp15.f1= vd; _temp15;}); _temp14;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo((
void*) vd->type));} static void Cyc_NewControlFlow_add_malloc_root( struct Cyc_NewControlFlow_AbsEnv*
env, struct Cyc_Absyn_Exp* e, void* t){ env->all_roots= Cyc_Dict_insert_new( env->all_roots,(
void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp16=( struct Cyc_CfFlowInfo_MallocPt_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp16[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp17; _temp17.tag= Cyc_CfFlowInfo_MallocPt;
_temp17.f1= e; _temp17;}); _temp16;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( t));}
static void* Cyc_NewControlFlow_meet_absexp( struct Cyc_NewControlFlow_AbsSyn
syn){ if(( void*) syn.when_true_exp == ( void*) syn.when_false_exp){ return(
void*) syn.when_true_exp;} return Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,(
void*) syn.when_true_exp,( void*) syn.when_false_exp);} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_abstract_exp( struct Cyc_NewControlFlow_AbsEnv* env, struct
Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass( struct Cyc_NewControlFlow_AbsEnv*
env, struct Cyc_Absyn_Stmt* s); static void* Cyc_NewControlFlow_use_exp( struct
Cyc_NewControlFlow_AbsEnv* env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
_temp18= Cyc_NewControlFlow_abstract_exp( env, e); return Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_NewControlFlow_meet_absexp( _temp18), Cyc_CfAbsexp_mkUseAE((
void*) _temp18.absop));} static struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_use_exp_unordered(
struct Cyc_NewControlFlow_AbsEnv* env, struct Cyc_List_List* es){ void* _temp19=
Cyc_CfAbsexp_mkGroupAE_l(( void*) Cyc_CfAbsexp_UnorderedG,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_NewControlFlow_AbsEnv*, struct Cyc_Absyn_Exp*), struct
Cyc_NewControlFlow_AbsEnv* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_NewControlFlow_use_exp,
env, es)); return({ struct Cyc_NewControlFlow_AbsSyn _temp20; _temp20.when_true_exp=(
void*) _temp19; _temp20.when_false_exp=( void*) _temp19; _temp20.absop=( void*)
Cyc_CfAbsexp_mkUnknownOp(); _temp20;});} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_malloc(
struct Cyc_NewControlFlow_AbsEnv* env, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp*
rgn_exp, struct Cyc_Absyn_Exp* init_exp){ void* _temp21= Cyc_CfAbsexp_mkMallocOp(
outer_e); void* ans_aexp= Cyc_CfAbsexp_mkSkipAE(); if( init_exp !=  0){ struct
Cyc_NewControlFlow_AbsSyn _temp22= Cyc_NewControlFlow_abstract_exp( env,( struct
Cyc_Absyn_Exp*) _check_null( init_exp)); ans_aexp= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_OrderedG, Cyc_NewControlFlow_meet_absexp( _temp22), Cyc_CfAbsexp_mkAssignAE(
_temp21,( void*) _temp22.absop));} ans_aexp= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkMallocAE( outer_e), ans_aexp); if( rgn_exp !=  0){ ans_aexp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
env,( struct Cyc_Absyn_Exp*) _check_null( rgn_exp))), ans_aexp);} return({
struct Cyc_NewControlFlow_AbsSyn _temp23; _temp23.when_true_exp=( void*)
ans_aexp; _temp23.when_false_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp23.absop=(
void*) Cyc_CfAbsexp_mkAddressOp( _temp21); _temp23;});} struct _tuple4{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_abstract_exp( struct Cyc_NewControlFlow_AbsEnv* env, struct
Cyc_Absyn_Exp* e){ void* _temp24=( void*) e->r; struct Cyc_Absyn_Exp* _temp116;
struct Cyc_Absyn_Exp* _temp118; struct Cyc_Absyn_Exp* _temp120; void* _temp122;
void* _temp124; int _temp126; void* _temp128; void* _temp130; int _temp132; void*
_temp134; void* _temp136; void* _temp138; void* _temp140; struct Cyc_Absyn_Vardecl*
_temp142; void* _temp144; struct Cyc_Absyn_Vardecl* _temp146; void* _temp148;
struct Cyc_Absyn_Vardecl* _temp150; struct Cyc_Absyn_Exp* _temp152; struct Cyc_Core_Opt*
_temp154; struct Cyc_Absyn_Exp* _temp156; struct Cyc_List_List* _temp158; struct
Cyc_Absyn_Exp* _temp160; struct Cyc_Absyn_Exp* _temp162; struct Cyc_Absyn_Exp*
_temp164; struct Cyc_List_List* _temp166; struct Cyc_Absyn_Exp* _temp168; struct
Cyc_Absyn_Exp* _temp170; struct Cyc_Absyn_Exp* _temp172; struct Cyc_Absyn_Exp*
_temp174; struct _tagged_arr* _temp176; struct Cyc_Absyn_Exp* _temp178; struct
_tagged_arr* _temp180; struct Cyc_Absyn_Exp* _temp182; struct Cyc_Absyn_Exp*
_temp184; struct Cyc_Absyn_Exp* _temp186; void* _temp188; struct Cyc_Absyn_Exp*
_temp190; struct Cyc_List_List* _temp192; struct Cyc_List_List* _temp194; struct
Cyc_List_List* _temp196; struct Cyc_List_List* _temp198; struct Cyc_List_List*
_temp200; struct Cyc_Absyn_Exp* _temp202; struct Cyc_Absyn_Exp* _temp204; struct
Cyc_Absyn_Exp* _temp206; struct Cyc_Absyn_Exp* _temp208; struct Cyc_Absyn_Exp*
_temp210; struct Cyc_Absyn_Stmt* _temp212; struct Cyc_Absyn_Exp* _temp214;
struct Cyc_Absyn_Exp* _temp216; struct Cyc_Absyn_Exp* _temp218; struct Cyc_Absyn_Exp*
_temp220; struct Cyc_Absyn_Vardecl* _temp222; void* _temp224; _LL26: if(*(( int*)
_temp24) ==  Cyc_Absyn_Cast_e){ _LL117: _temp116=(( struct Cyc_Absyn_Cast_e_struct*)
_temp24)->f2; goto _LL27;} else{ goto _LL28;} _LL28: if(*(( int*) _temp24) == 
Cyc_Absyn_NoInstantiate_e){ _LL119: _temp118=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp24)->f1; goto _LL29;} else{ goto _LL30;} _LL30: if(*(( int*) _temp24) == 
Cyc_Absyn_Instantiate_e){ _LL121: _temp120=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp24)->f1; goto _LL31;} else{ goto _LL32;} _LL32: if(*(( int*) _temp24) == 
Cyc_Absyn_Const_e){ _LL123: _temp122=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp24)->f1; if( _temp122 == ( void*) Cyc_Absyn_Null_c){ goto _LL33;} else{
goto _LL34;}} else{ goto _LL34;} _LL34: if(*(( int*) _temp24) ==  Cyc_Absyn_Const_e){
_LL125: _temp124=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp24)->f1; if((
unsigned int) _temp124 >  1u?*(( int*) _temp124) ==  Cyc_Absyn_Int_c: 0){ _LL129:
_temp128=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp124)->f1; if( _temp128
== ( void*) Cyc_Absyn_Signed){ goto _LL127;} else{ goto _LL36;} _LL127: _temp126=((
struct Cyc_Absyn_Int_c_struct*) _temp124)->f2; if( _temp126 ==  0){ goto _LL35;}
else{ goto _LL36;}} else{ goto _LL36;}} else{ goto _LL36;} _LL36: if(*(( int*)
_temp24) ==  Cyc_Absyn_Const_e){ _LL131: _temp130=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp24)->f1; if(( unsigned int) _temp130 >  1u?*(( int*) _temp130) ==  Cyc_Absyn_Int_c:
0){ _LL135: _temp134=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp130)->f1;
if( _temp134 == ( void*) Cyc_Absyn_Signed){ goto _LL133;} else{ goto _LL38;}
_LL133: _temp132=(( struct Cyc_Absyn_Int_c_struct*) _temp130)->f2; if( _temp132
==  1){ goto _LL37;} else{ goto _LL38;}} else{ goto _LL38;}} else{ goto _LL38;}
_LL38: if(*(( int*) _temp24) ==  Cyc_Absyn_Const_e){ goto _LL39;} else{ goto
_LL40;} _LL40: if(*(( int*) _temp24) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL41;}
else{ goto _LL42;} _LL42: if(*(( int*) _temp24) ==  Cyc_Absyn_Sizeoftyp_e){ goto
_LL43;} else{ goto _LL44;} _LL44: if(*(( int*) _temp24) ==  Cyc_Absyn_Offsetof_e){
goto _LL45;} else{ goto _LL46;} _LL46: if(*(( int*) _temp24) ==  Cyc_Absyn_Enum_e){
goto _LL47;} else{ goto _LL48;} _LL48: if(*(( int*) _temp24) ==  Cyc_Absyn_Var_e){
_LL137: _temp136=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp24)->f2; if((
unsigned int) _temp136 >  1u?*(( int*) _temp136) ==  Cyc_Absyn_Funname_b: 0){
goto _LL49;} else{ goto _LL50;}} else{ goto _LL50;} _LL50: if(*(( int*) _temp24)
==  Cyc_Absyn_Var_e){ _LL139: _temp138=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if(( unsigned int) _temp138 >  1u?*(( int*) _temp138) ==  Cyc_Absyn_Global_b:
0){ goto _LL51;} else{ goto _LL52;}} else{ goto _LL52;} _LL52: if(*(( int*)
_temp24) ==  Cyc_Absyn_Var_e){ _LL141: _temp140=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if(( unsigned int) _temp140 >  1u?*(( int*) _temp140) ==  Cyc_Absyn_Param_b:
0){ _LL143: _temp142=(( struct Cyc_Absyn_Param_b_struct*) _temp140)->f1; goto
_LL53;} else{ goto _LL54;}} else{ goto _LL54;} _LL54: if(*(( int*) _temp24) == 
Cyc_Absyn_Var_e){ _LL145: _temp144=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if(( unsigned int) _temp144 >  1u?*(( int*) _temp144) ==  Cyc_Absyn_Local_b:
0){ _LL147: _temp146=(( struct Cyc_Absyn_Local_b_struct*) _temp144)->f1; goto
_LL55;} else{ goto _LL56;}} else{ goto _LL56;} _LL56: if(*(( int*) _temp24) == 
Cyc_Absyn_Var_e){ _LL149: _temp148=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if(( unsigned int) _temp148 >  1u?*(( int*) _temp148) ==  Cyc_Absyn_Pat_b:
0){ _LL151: _temp150=(( struct Cyc_Absyn_Pat_b_struct*) _temp148)->f1; goto
_LL57;} else{ goto _LL58;}} else{ goto _LL58;} _LL58: if(*(( int*) _temp24) == 
Cyc_Absyn_AssignOp_e){ _LL157: _temp156=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp24)->f1; goto _LL155; _LL155: _temp154=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp24)->f2; if( _temp154 ==  0){ goto _LL153;} else{ goto _LL60;} _LL153:
_temp152=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp24)->f3; goto _LL59;}
else{ goto _LL60;} _LL60: if(*(( int*) _temp24) ==  Cyc_Absyn_FnCall_e){ _LL161:
_temp160=(( struct Cyc_Absyn_FnCall_e_struct*) _temp24)->f1; goto _LL159; _LL159:
_temp158=(( struct Cyc_Absyn_FnCall_e_struct*) _temp24)->f2; goto _LL61;} else{
goto _LL62;} _LL62: if(*(( int*) _temp24) ==  Cyc_Absyn_AssignOp_e){ _LL165:
_temp164=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp24)->f1; goto _LL163;
_LL163: _temp162=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp24)->f3; goto
_LL63;} else{ goto _LL64;} _LL64: if(*(( int*) _temp24) ==  Cyc_Absyn_Primop_e){
_LL167: _temp166=(( struct Cyc_Absyn_Primop_e_struct*) _temp24)->f2; goto _LL65;}
else{ goto _LL66;} _LL66: if(*(( int*) _temp24) ==  Cyc_Absyn_Address_e){ _LL169:
_temp168=(( struct Cyc_Absyn_Address_e_struct*) _temp24)->f1; goto _LL67;} else{
goto _LL68;} _LL68: if(*(( int*) _temp24) ==  Cyc_Absyn_Subscript_e){ _LL173:
_temp172=(( struct Cyc_Absyn_Subscript_e_struct*) _temp24)->f1; goto _LL171;
_LL171: _temp170=(( struct Cyc_Absyn_Subscript_e_struct*) _temp24)->f2; goto
_LL69;} else{ goto _LL70;} _LL70: if(*(( int*) _temp24) ==  Cyc_Absyn_Deref_e){
_LL175: _temp174=(( struct Cyc_Absyn_Deref_e_struct*) _temp24)->f1; goto _LL71;}
else{ goto _LL72;} _LL72: if(*(( int*) _temp24) ==  Cyc_Absyn_StructArrow_e){
_LL179: _temp178=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp24)->f1; goto
_LL177; _LL177: _temp176=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp24)->f2;
goto _LL73;} else{ goto _LL74;} _LL74: if(*(( int*) _temp24) ==  Cyc_Absyn_StructMember_e){
_LL183: _temp182=(( struct Cyc_Absyn_StructMember_e_struct*) _temp24)->f1; goto
_LL181; _LL181: _temp180=(( struct Cyc_Absyn_StructMember_e_struct*) _temp24)->f2;
goto _LL75;} else{ goto _LL76;} _LL76: if(*(( int*) _temp24) ==  Cyc_Absyn_New_e){
_LL187: _temp186=(( struct Cyc_Absyn_New_e_struct*) _temp24)->f1; goto _LL185;
_LL185: _temp184=(( struct Cyc_Absyn_New_e_struct*) _temp24)->f2; goto _LL77;}
else{ goto _LL78;} _LL78: if(*(( int*) _temp24) ==  Cyc_Absyn_Malloc_e){ _LL191:
_temp190=(( struct Cyc_Absyn_Malloc_e_struct*) _temp24)->f1; goto _LL189; _LL189:
_temp188=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp24)->f2; goto _LL79;}
else{ goto _LL80;} _LL80: if(*(( int*) _temp24) ==  Cyc_Absyn_Tunion_e){ _LL193:
_temp192=(( struct Cyc_Absyn_Tunion_e_struct*) _temp24)->f3; goto _LL81;} else{
goto _LL82;} _LL82: if(*(( int*) _temp24) ==  Cyc_Absyn_Tuple_e){ _LL195:
_temp194=(( struct Cyc_Absyn_Tuple_e_struct*) _temp24)->f1; goto _LL83;} else{
goto _LL84;} _LL84: if(*(( int*) _temp24) ==  Cyc_Absyn_AnonStruct_e){ _LL197:
_temp196=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp24)->f2; goto _LL85;}
else{ goto _LL86;} _LL86: if(*(( int*) _temp24) ==  Cyc_Absyn_Struct_e){ _LL199:
_temp198=(( struct Cyc_Absyn_Struct_e_struct*) _temp24)->f3; goto _LL87;} else{
goto _LL88;} _LL88: if(*(( int*) _temp24) ==  Cyc_Absyn_Array_e){ _LL201:
_temp200=(( struct Cyc_Absyn_Array_e_struct*) _temp24)->f1; goto _LL89;} else{
goto _LL90;} _LL90: if(*(( int*) _temp24) ==  Cyc_Absyn_Increment_e){ _LL203:
_temp202=(( struct Cyc_Absyn_Increment_e_struct*) _temp24)->f1; goto _LL91;}
else{ goto _LL92;} _LL92: if(*(( int*) _temp24) ==  Cyc_Absyn_Throw_e){ _LL205:
_temp204=(( struct Cyc_Absyn_Throw_e_struct*) _temp24)->f1; goto _LL93;} else{
goto _LL94;} _LL94: if(*(( int*) _temp24) ==  Cyc_Absyn_Conditional_e){ _LL211:
_temp210=(( struct Cyc_Absyn_Conditional_e_struct*) _temp24)->f1; goto _LL209;
_LL209: _temp208=(( struct Cyc_Absyn_Conditional_e_struct*) _temp24)->f2; goto
_LL207; _LL207: _temp206=(( struct Cyc_Absyn_Conditional_e_struct*) _temp24)->f3;
goto _LL95;} else{ goto _LL96;} _LL96: if(*(( int*) _temp24) ==  Cyc_Absyn_StmtExp_e){
_LL213: _temp212=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp24)->f1; goto _LL97;}
else{ goto _LL98;} _LL98: if(*(( int*) _temp24) ==  Cyc_Absyn_SeqExp_e){ _LL217:
_temp216=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp24)->f1; goto _LL215; _LL215:
_temp214=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp24)->f2; goto _LL99;} else{
goto _LL100;} _LL100: if(*(( int*) _temp24) ==  Cyc_Absyn_Comprehension_e){
_LL223: _temp222=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp24)->f1; goto
_LL221; _LL221: _temp220=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp24)->f2;
goto _LL219; _LL219: _temp218=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp24)->f3; goto _LL101;} else{ goto _LL102;} _LL102: if(*(( int*) _temp24) == 
Cyc_Absyn_Var_e){ _LL225: _temp224=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if( _temp224 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL103;}
else{ goto _LL104;}} else{ goto _LL104;} _LL104: if(*(( int*) _temp24) ==  Cyc_Absyn_UnknownId_e){
goto _LL105;} else{ goto _LL106;} _LL106: if(*(( int*) _temp24) ==  Cyc_Absyn_UnknownCall_e){
goto _LL107;} else{ goto _LL108;} _LL108: if(*(( int*) _temp24) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL109;} else{ goto _LL110;} _LL110: if(*(( int*) _temp24) ==  Cyc_Absyn_CompoundLit_e){
goto _LL111;} else{ goto _LL112;} _LL112: if(*(( int*) _temp24) ==  Cyc_Absyn_Codegen_e){
goto _LL113;} else{ goto _LL114;} _LL114: if(*(( int*) _temp24) ==  Cyc_Absyn_Fill_e){
goto _LL115;} else{ goto _LL25;} _LL27: _temp118= _temp116; goto _LL29; _LL29:
_temp120= _temp118; goto _LL31; _LL31: return Cyc_NewControlFlow_abstract_exp(
env, _temp120); _LL33: goto _LL35; _LL35: return({ struct Cyc_NewControlFlow_AbsSyn
_temp226; _temp226.when_true_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp226.when_false_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp226.absop=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp226;}); _LL37: return({ struct Cyc_NewControlFlow_AbsSyn _temp227; _temp227.when_true_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp227.when_false_exp=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp227.absop=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp227;}); _LL39: goto
_LL41; _LL41: goto _LL43; _LL43: goto _LL45; _LL45: goto _LL47; _LL47: goto
_LL49; _LL49: goto _LL51; _LL51: return({ struct Cyc_NewControlFlow_AbsSyn
_temp228; _temp228.when_true_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp228.when_false_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp228.absop=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp228;}); _LL53: _temp146= _temp142; goto _LL55; _LL55: _temp150= _temp146;
goto _LL57; _LL57: return({ struct Cyc_NewControlFlow_AbsSyn _temp229; _temp229.when_true_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp229.when_false_exp=( void*) Cyc_CfAbsexp_mkSkipAE();
_temp229.absop=( void*) Cyc_CfAbsexp_mkLocalOp( _temp150); _temp229;}); _LL59: {
struct Cyc_NewControlFlow_AbsSyn _temp230= Cyc_NewControlFlow_abstract_exp( env,
_temp156); struct Cyc_NewControlFlow_AbsSyn _temp231= Cyc_NewControlFlow_abstract_exp(
env, _temp152); void* _temp232= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, Cyc_NewControlFlow_meet_absexp(
_temp230), Cyc_NewControlFlow_meet_absexp( _temp231)), Cyc_CfAbsexp_mkAssignAE((
void*) _temp230.absop,( void*) _temp231.absop)); return({ struct Cyc_NewControlFlow_AbsSyn
_temp233; _temp233.when_true_exp=( void*) _temp232; _temp233.when_false_exp=(
void*) _temp232; _temp233.absop=( void*)(( void*) _temp230.absop); _temp233;});}
_LL61: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp234=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp234->hd=( void*) _temp160; _temp234->tl= _temp158; _temp234;})); _LL63:
_temp166=({ struct Cyc_List_List* _temp235=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp235->hd=( void*) _temp164; _temp235->tl=({
struct Cyc_List_List* _temp236=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp236->hd=( void*) _temp162; _temp236->tl= 0;
_temp236;}); _temp235;}); goto _LL65; _LL65: return Cyc_NewControlFlow_use_exp_unordered(
env, _temp166); _LL67: { void* _temp237=( void*) _temp168->r; _LL239: if(*(( int*)
_temp237) ==  Cyc_Absyn_Struct_e){ goto _LL240;} else{ goto _LL241;} _LL241: if(*((
int*) _temp237) ==  Cyc_Absyn_Tuple_e){ goto _LL242;} else{ goto _LL243;} _LL243:
goto _LL244; _LL240: goto _LL242; _LL242: Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp168->topt))->v);
return Cyc_NewControlFlow_abstract_malloc( env, e, 0,( struct Cyc_Absyn_Exp*)
_temp168); _LL244: { struct Cyc_NewControlFlow_AbsSyn _temp245= Cyc_NewControlFlow_abstract_exp(
env, _temp168); void* _temp246= Cyc_NewControlFlow_meet_absexp( _temp245);
return({ struct Cyc_NewControlFlow_AbsSyn _temp247; _temp247.when_true_exp=(
void*) _temp246; _temp247.when_false_exp=( void*) _temp246; _temp247.absop=(
void*) Cyc_CfAbsexp_mkAddressOp(( void*) _temp245.absop); _temp247;});} _LL238:;}
_LL69: { void* _temp248= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp172->topt))->v); _LL250: if(( unsigned int) _temp248 >  4u?*((
int*) _temp248) ==  Cyc_Absyn_TupleType: 0){ goto _LL251;} else{ goto _LL252;}
_LL252: goto _LL253; _LL251: { struct Cyc_NewControlFlow_AbsSyn _temp254= Cyc_NewControlFlow_abstract_exp(
env, _temp172); void* _temp255= Cyc_NewControlFlow_meet_absexp( _temp254);
struct _tagged_arr* _temp256= Cyc_Absyn_fieldname(( int) Cyc_Evexp_eval_const_uint_exp(
_temp170)); return({ struct Cyc_NewControlFlow_AbsSyn _temp257; _temp257.when_true_exp=(
void*) _temp255; _temp257.when_false_exp=( void*) _temp255; _temp257.absop=(
void*) Cyc_CfAbsexp_mkMemberOp(( void*) _temp254.absop, _temp256); _temp257;});}
_LL253: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp258=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp258->hd=( void*) _temp172; _temp258->tl=({ struct Cyc_List_List* _temp259=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp259->hd=(
void*) _temp170; _temp259->tl= 0; _temp259;}); _temp258;})); _LL249:;} _LL71: {
struct Cyc_NewControlFlow_AbsSyn _temp260= Cyc_NewControlFlow_abstract_exp( env,
_temp174); void* _temp261= Cyc_NewControlFlow_meet_absexp( _temp260); void*
absop;{ void* _temp262= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp174->topt))->v); struct Cyc_Absyn_PtrInfo _temp268; struct Cyc_Absyn_Conref*
_temp270; void* _temp272; _LL264: if(( unsigned int) _temp262 >  4u?*(( int*)
_temp262) ==  Cyc_Absyn_PointerType: 0){ _LL269: _temp268=(( struct Cyc_Absyn_PointerType_struct*)
_temp262)->f1; _LL273: _temp272=( void*) _temp268.elt_typ; goto _LL271; _LL271:
_temp270= _temp268.bounds; goto _LL265;} else{ goto _LL266;} _LL266: goto _LL267;
_LL265: if( Cyc_Tcutil_is_bound_one( _temp270)){ absop= Cyc_CfAbsexp_mkDerefOp((
void*) _temp260.absop);} else{ _temp261= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp261, Cyc_CfAbsexp_mkUseAE(( void*) _temp260.absop)); absop= Cyc_CfAbsexp_mkUnknownOp();}
goto _LL263; _LL267:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp274=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp274[ 0]=({ struct Cyc_Core_Impossible_struct _temp275; _temp275.tag= Cyc_Core_Impossible;
_temp275.f1= _tag_arr("NewControlFlow: bad type in Deref", sizeof( unsigned char),
34u); _temp275;}); _temp274;})); _LL263:;} return({ struct Cyc_NewControlFlow_AbsSyn
_temp276; _temp276.when_true_exp=( void*) _temp261; _temp276.when_false_exp=(
void*) _temp261; _temp276.absop=( void*) absop; _temp276;});} _LL73: { struct
Cyc_NewControlFlow_AbsSyn _temp277= Cyc_NewControlFlow_abstract_exp( env,
_temp178); void* _temp278= Cyc_NewControlFlow_meet_absexp( _temp277); void*
absop;{ void* _temp279= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp178->topt))->v); struct Cyc_Absyn_PtrInfo _temp285; struct Cyc_Absyn_Conref*
_temp287; void* _temp289; _LL281: if(( unsigned int) _temp279 >  4u?*(( int*)
_temp279) ==  Cyc_Absyn_PointerType: 0){ _LL286: _temp285=(( struct Cyc_Absyn_PointerType_struct*)
_temp279)->f1; _LL290: _temp289=( void*) _temp285.elt_typ; goto _LL288; _LL288:
_temp287= _temp285.bounds; goto _LL282;} else{ goto _LL283;} _LL283: goto _LL284;
_LL282: if( Cyc_Tcutil_is_bound_one( _temp287)){ void* _temp291= Cyc_Tcutil_compress(
_temp289); _LL293: if(( unsigned int) _temp291 >  4u?*(( int*) _temp291) ==  Cyc_Absyn_UnionType:
0){ goto _LL294;} else{ goto _LL295;} _LL295: if(( unsigned int) _temp291 >  4u?*((
int*) _temp291) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL296;} else{ goto _LL297;}
_LL297: goto _LL298; _LL294: goto _LL296; _LL296: absop= Cyc_CfAbsexp_mkDerefOp((
void*) _temp277.absop); goto _LL292; _LL298: absop= Cyc_CfAbsexp_mkMemberOp( Cyc_CfAbsexp_mkDerefOp((
void*) _temp277.absop), _temp176); _LL292:;} else{ _temp278= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp278, Cyc_CfAbsexp_mkUseAE(( void*) _temp277.absop));
absop= Cyc_CfAbsexp_mkUnknownOp();} goto _LL280; _LL284:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp299=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp299[ 0]=({ struct
Cyc_Core_Impossible_struct _temp300; _temp300.tag= Cyc_Core_Impossible; _temp300.f1=
_tag_arr("NewControlFlow: bad type in StructArrow", sizeof( unsigned char), 40u);
_temp300;}); _temp299;})); _LL280:;} return({ struct Cyc_NewControlFlow_AbsSyn
_temp301; _temp301.when_true_exp=( void*) _temp278; _temp301.when_false_exp=(
void*) _temp278; _temp301.absop=( void*) absop; _temp301;});} _LL75: { struct
Cyc_NewControlFlow_AbsSyn _temp302= Cyc_NewControlFlow_abstract_exp( env,
_temp182); void* _temp303= Cyc_NewControlFlow_meet_absexp( _temp302); void*
absop;{ void* _temp304= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp182->topt))->v); _LL306: if(( unsigned int) _temp304 >  4u?*((
int*) _temp304) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL307;} else{ goto _LL308;}
_LL308: if(( unsigned int) _temp304 >  4u?*(( int*) _temp304) ==  Cyc_Absyn_UnionType:
0){ goto _LL309;} else{ goto _LL310;} _LL310: goto _LL311; _LL307: goto _LL309;
_LL309: absop=( void*) _temp302.absop; goto _LL305; _LL311: absop= Cyc_CfAbsexp_mkMemberOp((
void*) _temp302.absop, _temp180); _LL305:;} return({ struct Cyc_NewControlFlow_AbsSyn
_temp312; _temp312.when_true_exp=( void*) _temp303; _temp312.when_false_exp=(
void*) _temp303; _temp312.absop=( void*) absop; _temp312;});} _LL77: Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp184->topt))->v);
return Cyc_NewControlFlow_abstract_malloc( env, e, _temp186,( struct Cyc_Absyn_Exp*)
_temp184); _LL79: Cyc_NewControlFlow_add_malloc_root( env, e, _temp188); return
Cyc_NewControlFlow_abstract_malloc( env, e, _temp190, 0); _LL81: if( _temp192 == 
0){ return({ struct Cyc_NewControlFlow_AbsSyn _temp313; _temp313.when_true_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp313.when_false_exp=( void*) Cyc_CfAbsexp_mkSkipAE();
_temp313.absop=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp313;});} _temp194=
_temp192; goto _LL83; _LL83: { struct Cyc_List_List* arg_aexps= 0; struct Cyc_Dict_Dict*
absop_dict=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{ int j= 0; for( 0;
_temp194 !=  0;( _temp194=(( struct Cyc_List_List*) _check_null( _temp194))->tl,
++ j)){ struct Cyc_NewControlFlow_AbsSyn _temp314= Cyc_NewControlFlow_abstract_exp(
env,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp194))->hd);
arg_aexps=({ struct Cyc_List_List* _temp315=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp315->hd=( void*) Cyc_NewControlFlow_meet_absexp(
_temp314); _temp315->tl= arg_aexps; _temp315;}); absop_dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)(
absop_dict, Cyc_Absyn_fieldname( j),( void*) _temp314.absop);}}{ void* _temp316=
Cyc_CfAbsexp_mkGroupAE_l(( void*) Cyc_CfAbsexp_UnorderedG, arg_aexps); return({
struct Cyc_NewControlFlow_AbsSyn _temp317; _temp317.when_true_exp=( void*)
_temp316; _temp317.when_false_exp=( void*) _temp316; _temp317.absop=( void*) Cyc_CfAbsexp_mkDictOp(
absop_dict); _temp317;});}} _LL85: _temp198= _temp196; goto _LL87; _LL87: {
struct Cyc_List_List* arg_aexps= 0; struct Cyc_Dict_Dict* absop_dict=(( struct
Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp); for( 0; _temp198 !=  0; _temp198=(( struct Cyc_List_List*)
_check_null( _temp198))->tl){ struct Cyc_NewControlFlow_AbsSyn _temp318= Cyc_NewControlFlow_abstract_exp(
env,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp198))->hd)).f2);
arg_aexps=({ struct Cyc_List_List* _temp319=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp319->hd=( void*) Cyc_NewControlFlow_meet_absexp(
_temp318); _temp319->tl= arg_aexps; _temp319;});{ struct Cyc_List_List* ds=(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp198))->hd)).f1; for(
0; ds !=  0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){ void* _temp320=(
void*)(( struct Cyc_List_List*) _check_null( ds))->hd; struct _tagged_arr*
_temp326; _LL322: if(*(( int*) _temp320) ==  Cyc_Absyn_ArrayElement){ goto
_LL323;} else{ goto _LL324;} _LL324: if(*(( int*) _temp320) ==  Cyc_Absyn_FieldName){
_LL327: _temp326=(( struct Cyc_Absyn_FieldName_struct*) _temp320)->f1; goto
_LL325;} else{ goto _LL321;} _LL323:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp328=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp328[ 0]=({ struct Cyc_Core_Impossible_struct _temp329; _temp329.tag= Cyc_Core_Impossible;
_temp329.f1= _tag_arr("bad struct designator", sizeof( unsigned char), 22u);
_temp329;}); _temp328;})); _LL325: absop_dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)(
absop_dict, _temp326,( void*) _temp318.absop); _LL321:;}}}{ void* _temp330= Cyc_CfAbsexp_mkGroupAE_l((
void*) Cyc_CfAbsexp_UnorderedG, arg_aexps); return({ struct Cyc_NewControlFlow_AbsSyn
_temp331; _temp331.when_true_exp=( void*) _temp330; _temp331.when_false_exp=(
void*) _temp330; _temp331.absop=( void*) Cyc_CfAbsexp_mkDictOp( absop_dict);
_temp331;});}} _LL89: return Cyc_NewControlFlow_use_exp_unordered( env,(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct _tuple4*)) Cyc_Core_snd,
_temp200)); _LL91: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct
Cyc_List_List* _temp332=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp332->hd=( void*) _temp202; _temp332->tl= 0; _temp332;})); _LL93: { void*
_temp333= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_NewControlFlow_use_exp(
env, _temp204), Cyc_CfAbsexp_mkBottomAE()); return({ struct Cyc_NewControlFlow_AbsSyn
_temp334; _temp334.when_true_exp=( void*) _temp333; _temp334.when_false_exp=(
void*) _temp333; _temp334.absop=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp334;});}
_LL95: { struct Cyc_NewControlFlow_AbsSyn _temp335= Cyc_NewControlFlow_abstract_exp(
env, _temp210); struct Cyc_NewControlFlow_AbsSyn _temp336= Cyc_NewControlFlow_abstract_exp(
env, _temp208); struct Cyc_NewControlFlow_AbsSyn _temp337= Cyc_NewControlFlow_abstract_exp(
env, _temp206); void* _temp338= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp335.when_true_exp, Cyc_CfAbsexp_mkUseAE(( void*) _temp335.absop));
void* _temp339= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp335.when_false_exp, Cyc_CfAbsexp_mkUseAE(( void*) _temp335.absop)); void*
_temp340= Cyc_CfAbsexp_mkUseAE(( void*) _temp336.absop); void* _temp341= Cyc_CfAbsexp_mkUseAE((
void*) _temp337.absop); void* _temp342= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp338, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp336.when_true_exp, _temp340)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp339, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp337.when_true_exp, _temp341))); void* _temp343= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp338, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp336.when_false_exp, _temp340)), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp339, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp337.when_false_exp, _temp341))); return({ struct Cyc_NewControlFlow_AbsSyn
_temp344; _temp344.when_true_exp=( void*) _temp342; _temp344.when_false_exp=(
void*) _temp343; _temp344.absop=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp344;});}
_LL97: Cyc_NewControlFlow_cf_prepass( env, _temp212);{ struct Cyc_Absyn_Stmt*
_temp345= _temp212; while( 1) { void* _temp346=( void*) _temp345->r; struct Cyc_Absyn_Stmt*
_temp356; struct Cyc_Absyn_Stmt* _temp358; struct Cyc_Absyn_Stmt* _temp360;
struct Cyc_Absyn_Decl* _temp362; struct Cyc_Absyn_Exp* _temp364; _LL348: if((
unsigned int) _temp346 >  1u?*(( int*) _temp346) ==  Cyc_Absyn_Seq_s: 0){ _LL359:
_temp358=(( struct Cyc_Absyn_Seq_s_struct*) _temp346)->f1; goto _LL357; _LL357:
_temp356=(( struct Cyc_Absyn_Seq_s_struct*) _temp346)->f2; goto _LL349;} else{
goto _LL350;} _LL350: if(( unsigned int) _temp346 >  1u?*(( int*) _temp346) == 
Cyc_Absyn_Decl_s: 0){ _LL363: _temp362=(( struct Cyc_Absyn_Decl_s_struct*)
_temp346)->f1; goto _LL361; _LL361: _temp360=(( struct Cyc_Absyn_Decl_s_struct*)
_temp346)->f2; goto _LL351;} else{ goto _LL352;} _LL352: if(( unsigned int)
_temp346 >  1u?*(( int*) _temp346) ==  Cyc_Absyn_Exp_s: 0){ _LL365: _temp364=((
struct Cyc_Absyn_Exp_s_struct*) _temp346)->f1; goto _LL353;} else{ goto _LL354;}
_LL354: goto _LL355; _LL349: _temp345= _temp356; continue; _LL351: _temp345=
_temp360; continue; _LL353: { void* _temp366=( void*)( Cyc_NewControlFlow_abstract_exp(
env, _temp364)).absop; void* _temp367= Cyc_CfAbsexp_mkStmtAE( _temp212); return({
struct Cyc_NewControlFlow_AbsSyn _temp368; _temp368.when_true_exp=( void*)
_temp367; _temp368.when_false_exp=( void*) _temp367; _temp368.absop=( void*)
_temp366; _temp368;});} _LL355:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp369=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp369[ 0]=({ struct Cyc_Core_Impossible_struct _temp370; _temp370.tag= Cyc_Core_Impossible;
_temp370.f1= _tag_arr("abstract_exp: ill-formed StmtExp", sizeof( unsigned char),
33u); _temp370;}); _temp369;})); _LL347:;}} _LL99: { struct Cyc_NewControlFlow_AbsSyn
_temp371= Cyc_NewControlFlow_abstract_exp( env, _temp216); struct Cyc_NewControlFlow_AbsSyn
_temp372= Cyc_NewControlFlow_abstract_exp( env, _temp214); void* _temp373= Cyc_NewControlFlow_meet_absexp(
_temp371); return({ struct Cyc_NewControlFlow_AbsSyn _temp374; _temp374.when_true_exp=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp373,( void*)
_temp372.when_true_exp); _temp374.when_false_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp373,( void*) _temp372.when_false_exp);
_temp374.absop=( void*)(( void*) _temp372.absop); _temp374;});} _LL101: Cyc_NewControlFlow_add_var_root(
env, _temp222);{ struct Cyc_NewControlFlow_AbsSyn _temp375= Cyc_NewControlFlow_abstract_exp(
env, _temp220); struct Cyc_NewControlFlow_AbsSyn _temp376= Cyc_NewControlFlow_abstract_exp(
env, _temp218); void* _temp377= Cyc_CfAbsexp_mkComprehensionAE( _temp222,( void*)
_temp375.when_true_exp,( void*) _temp375.when_false_exp,( void*) _temp375.absop,
Cyc_NewControlFlow_meet_absexp( _temp376),( void*) _temp376.absop); return({
struct Cyc_NewControlFlow_AbsSyn _temp378; _temp378.when_true_exp=( void*)
_temp377; _temp378.when_false_exp=( void*) _temp377; _temp378.absop=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp378;});} _LL103: goto _LL105; _LL105: goto _LL107; _LL107: goto _LL109;
_LL109: goto _LL111; _LL111: goto _LL113; _LL113: goto _LL115; _LL115:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp379=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp379[ 0]=({ struct
Cyc_Core_Impossible_struct _temp380; _temp380.tag= Cyc_Core_Impossible; _temp380.f1=
_tag_arr("abstract_exp, unexpected exp form", sizeof( unsigned char), 34u);
_temp380;}); _temp379;})); _LL25:;} static struct _tuple0 Cyc_NewControlFlow_abstract_guard(
struct Cyc_NewControlFlow_AbsEnv* env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
_temp381= Cyc_NewControlFlow_abstract_exp( env, e); void* _temp382= Cyc_CfAbsexp_mkUseAE((
void*) _temp381.absop); if(( void*) _temp381.when_true_exp == ( void*) _temp381.when_false_exp){
void* _temp383= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp381.when_true_exp, _temp382); return({ struct _tuple0 _temp384; _temp384.f1=
_temp383; _temp384.f2= _temp383; _temp384;});} return({ struct _tuple0 _temp385;
_temp385.f1= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp381.when_true_exp, _temp382); _temp385.f2= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_OrderedG,( void*) _temp381.when_false_exp, _temp382); _temp385;});}
struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs( struct Cyc_NewControlFlow_AbsEnv*
env, struct Cyc_List_List* scs){ struct Cyc_List_List* _temp386= 0; for( 0; scs
!=  0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause
_temp389; struct Cyc_Absyn_Stmt* _temp390; struct Cyc_Absyn_Exp* _temp392;
struct Cyc_Core_Opt* _temp394; struct Cyc_Absyn_Switch_clause* _temp387=( struct
Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( scs))->hd;
_temp389=* _temp387; _LL395: _temp394= _temp389.pat_vars; goto _LL393; _LL393:
_temp392= _temp389.where_clause; goto _LL391; _LL391: _temp390= _temp389.body;
goto _LL388; _LL388: if( _temp394 ==  0){( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp396=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp396[ 0]=({ struct Cyc_Core_Impossible_struct _temp397; _temp397.tag= Cyc_Core_Impossible;
_temp397.f1= _tag_arr("switch clause vds not set", sizeof( unsigned char), 26u);
_temp397;}); _temp396;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp394))->v; for( 0; vds !=  0; vds=((
struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd);}}
if( _temp392 !=  0){ void* _temp400; void* _temp402; struct _tuple0 _temp398=
Cyc_NewControlFlow_abstract_guard( env,( struct Cyc_Absyn_Exp*) _check_null(
_temp392)); _LL403: _temp402= _temp398.f1; goto _LL401; _LL401: _temp400=
_temp398.f2; goto _LL399; _LL399: _temp386=({ struct Cyc_List_List* _temp404=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp404->hd=(
void*) _temp402; _temp404->tl=({ struct Cyc_List_List* _temp405=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp405->hd=( void*) _temp400;
_temp405->tl= _temp386; _temp405;}); _temp404;});} Cyc_NewControlFlow_cf_prepass(
env, _temp390);} return Cyc_List_imp_rev( _temp386);} void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv* env, struct Cyc_Absyn_Stmt* s){ void* _temp406=(
void*) s->r; struct Cyc_Absyn_Exp* _temp458; struct Cyc_Absyn_Stmt* _temp460;
struct Cyc_Absyn_Stmt* _temp462; struct Cyc_Absyn_Exp* _temp464; struct Cyc_Absyn_Exp*
_temp466; struct Cyc_Absyn_Stmt* _temp468; struct Cyc_Absyn_Stmt* _temp470;
struct Cyc_Absyn_Exp* _temp472; struct _tuple3 _temp474; struct Cyc_Absyn_Stmt*
_temp476; struct Cyc_Absyn_Exp* _temp478; struct Cyc_Absyn_Stmt* _temp480;
struct Cyc_Absyn_Stmt* _temp482; struct _tuple3 _temp484; struct Cyc_Absyn_Stmt*
_temp486; struct Cyc_Absyn_Exp* _temp488; struct Cyc_Absyn_Stmt* _temp490;
struct Cyc_Absyn_Stmt* _temp492; struct Cyc_Absyn_Stmt* _temp494; struct _tuple3
_temp496; struct Cyc_Absyn_Stmt* _temp498; struct Cyc_Absyn_Exp* _temp500;
struct _tuple3 _temp502; struct Cyc_Absyn_Stmt* _temp504; struct Cyc_Absyn_Exp*
_temp506; struct Cyc_Absyn_Exp* _temp508; struct Cyc_List_List* _temp510; struct
Cyc_Absyn_Exp* _temp512; struct Cyc_List_List* _temp514; struct Cyc_Absyn_Stmt*
_temp516; struct Cyc_Absyn_Decl* _temp518; struct Cyc_Absyn_Decl _temp520; void*
_temp521; struct Cyc_Absyn_Vardecl* _temp523; struct Cyc_Absyn_Stmt* _temp525;
struct Cyc_Absyn_Decl* _temp527; struct Cyc_Absyn_Decl _temp529; void* _temp530;
struct Cyc_Absyn_Exp* _temp532; struct Cyc_Core_Opt* _temp534; struct Cyc_Core_Opt
_temp536; struct Cyc_List_List* _temp537; struct Cyc_Absyn_Stmt* _temp539;
struct Cyc_Absyn_Decl* _temp541; struct Cyc_Absyn_Decl _temp543; void* _temp544;
struct Cyc_List_List* _temp546; struct Cyc_Absyn_Stmt* _temp548; struct Cyc_List_List*
_temp550; struct Cyc_Absyn_Stmt* _temp552; struct Cyc_Absyn_Stmt* _temp554;
struct Cyc_Absyn_Vardecl* _temp556; struct Cyc_Absyn_Tvar* _temp558; _LL408: if(
_temp406 == ( void*) Cyc_Absyn_Skip_s){ goto _LL409;} else{ goto _LL410;} _LL410:
if(( unsigned int) _temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_Exp_s: 0){
_LL459: _temp458=(( struct Cyc_Absyn_Exp_s_struct*) _temp406)->f1; goto _LL411;}
else{ goto _LL412;} _LL412: if(( unsigned int) _temp406 >  1u?*(( int*) _temp406)
==  Cyc_Absyn_Seq_s: 0){ _LL463: _temp462=(( struct Cyc_Absyn_Seq_s_struct*)
_temp406)->f1; goto _LL461; _LL461: _temp460=(( struct Cyc_Absyn_Seq_s_struct*)
_temp406)->f2; goto _LL413;} else{ goto _LL414;} _LL414: if(( unsigned int)
_temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_Return_s: 0){ _LL465: _temp464=((
struct Cyc_Absyn_Return_s_struct*) _temp406)->f1; if( _temp464 ==  0){ goto
_LL415;} else{ goto _LL416;}} else{ goto _LL416;} _LL416: if(( unsigned int)
_temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_Return_s: 0){ _LL467: _temp466=((
struct Cyc_Absyn_Return_s_struct*) _temp406)->f1; goto _LL417;} else{ goto
_LL418;} _LL418: if(( unsigned int) _temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_IfThenElse_s:
0){ _LL473: _temp472=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp406)->f1;
goto _LL471; _LL471: _temp470=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp406)->f2;
goto _LL469; _LL469: _temp468=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp406)->f3;
goto _LL419;} else{ goto _LL420;} _LL420: if(( unsigned int) _temp406 >  1u?*((
int*) _temp406) ==  Cyc_Absyn_Do_s: 0){ _LL481: _temp480=(( struct Cyc_Absyn_Do_s_struct*)
_temp406)->f1; goto _LL475; _LL475: _temp474=(( struct Cyc_Absyn_Do_s_struct*)
_temp406)->f2; _LL479: _temp478= _temp474.f1; goto _LL477; _LL477: _temp476=
_temp474.f2; goto _LL421;} else{ goto _LL422;} _LL422: if(( unsigned int)
_temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_While_s: 0){ _LL485: _temp484=((
struct Cyc_Absyn_While_s_struct*) _temp406)->f1; _LL489: _temp488= _temp484.f1;
goto _LL487; _LL487: _temp486= _temp484.f2; goto _LL483; _LL483: _temp482=((
struct Cyc_Absyn_While_s_struct*) _temp406)->f2; goto _LL423;} else{ goto _LL424;}
_LL424: if(( unsigned int) _temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_Break_s:
0){ goto _LL425;} else{ goto _LL426;} _LL426: if(( unsigned int) _temp406 >  1u?*((
int*) _temp406) ==  Cyc_Absyn_Continue_s: 0){ goto _LL427;} else{ goto _LL428;}
_LL428: if(( unsigned int) _temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_Goto_s:
0){ _LL491: _temp490=(( struct Cyc_Absyn_Goto_s_struct*) _temp406)->f2; if(
_temp490 ==  0){ goto _LL429;} else{ goto _LL430;}} else{ goto _LL430;} _LL430:
if(( unsigned int) _temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_Goto_s: 0){
_LL493: _temp492=(( struct Cyc_Absyn_Goto_s_struct*) _temp406)->f2; goto _LL431;}
else{ goto _LL432;} _LL432: if(( unsigned int) _temp406 >  1u?*(( int*) _temp406)
==  Cyc_Absyn_For_s: 0){ _LL509: _temp508=(( struct Cyc_Absyn_For_s_struct*)
_temp406)->f1; goto _LL503; _LL503: _temp502=(( struct Cyc_Absyn_For_s_struct*)
_temp406)->f2; _LL507: _temp506= _temp502.f1; goto _LL505; _LL505: _temp504=
_temp502.f2; goto _LL497; _LL497: _temp496=(( struct Cyc_Absyn_For_s_struct*)
_temp406)->f3; _LL501: _temp500= _temp496.f1; goto _LL499; _LL499: _temp498=
_temp496.f2; goto _LL495; _LL495: _temp494=(( struct Cyc_Absyn_For_s_struct*)
_temp406)->f4; goto _LL433;} else{ goto _LL434;} _LL434: if(( unsigned int)
_temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_Switch_s: 0){ _LL513: _temp512=((
struct Cyc_Absyn_Switch_s_struct*) _temp406)->f1; goto _LL511; _LL511: _temp510=((
struct Cyc_Absyn_Switch_s_struct*) _temp406)->f2; goto _LL435;} else{ goto
_LL436;} _LL436: if(( unsigned int) _temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_Fallthru_s:
0){ _LL515: _temp514=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp406)->f1; goto
_LL437;} else{ goto _LL438;} _LL438: if(( unsigned int) _temp406 >  1u?*(( int*)
_temp406) ==  Cyc_Absyn_Decl_s: 0){ _LL519: _temp518=(( struct Cyc_Absyn_Decl_s_struct*)
_temp406)->f1; _temp520=* _temp518; _LL522: _temp521=( void*) _temp520.r; if(*((
int*) _temp521) ==  Cyc_Absyn_Var_d){ _LL524: _temp523=(( struct Cyc_Absyn_Var_d_struct*)
_temp521)->f1; goto _LL517;} else{ goto _LL440;} _LL517: _temp516=(( struct Cyc_Absyn_Decl_s_struct*)
_temp406)->f2; goto _LL439;} else{ goto _LL440;} _LL440: if(( unsigned int)
_temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_Decl_s: 0){ _LL528: _temp527=((
struct Cyc_Absyn_Decl_s_struct*) _temp406)->f1; _temp529=* _temp527; _LL531:
_temp530=( void*) _temp529.r; if(*(( int*) _temp530) ==  Cyc_Absyn_Let_d){
_LL535: _temp534=(( struct Cyc_Absyn_Let_d_struct*) _temp530)->f2; if( _temp534
==  0){ goto _LL442;} else{ _temp536=* _temp534; _LL538: _temp537=( struct Cyc_List_List*)
_temp536.v; goto _LL533;} _LL533: _temp532=(( struct Cyc_Absyn_Let_d_struct*)
_temp530)->f4; goto _LL526;} else{ goto _LL442;} _LL526: _temp525=(( struct Cyc_Absyn_Decl_s_struct*)
_temp406)->f2; goto _LL441;} else{ goto _LL442;} _LL442: if(( unsigned int)
_temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_Decl_s: 0){ _LL542: _temp541=((
struct Cyc_Absyn_Decl_s_struct*) _temp406)->f1; _temp543=* _temp541; _LL545:
_temp544=( void*) _temp543.r; if(*(( int*) _temp544) ==  Cyc_Absyn_Letv_d){
_LL547: _temp546=(( struct Cyc_Absyn_Letv_d_struct*) _temp544)->f1; goto _LL540;}
else{ goto _LL444;} _LL540: _temp539=(( struct Cyc_Absyn_Decl_s_struct*)
_temp406)->f2; goto _LL443;} else{ goto _LL444;} _LL444: if(( unsigned int)
_temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_Label_s: 0){ _LL549: _temp548=((
struct Cyc_Absyn_Label_s_struct*) _temp406)->f2; goto _LL445;} else{ goto _LL446;}
_LL446: if(( unsigned int) _temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_TryCatch_s:
0){ _LL553: _temp552=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp406)->f1; goto
_LL551; _LL551: _temp550=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp406)->f2;
goto _LL447;} else{ goto _LL448;} _LL448: if(( unsigned int) _temp406 >  1u?*((
int*) _temp406) ==  Cyc_Absyn_Region_s: 0){ _LL559: _temp558=(( struct Cyc_Absyn_Region_s_struct*)
_temp406)->f1; goto _LL557; _LL557: _temp556=(( struct Cyc_Absyn_Region_s_struct*)
_temp406)->f2; goto _LL555; _LL555: _temp554=(( struct Cyc_Absyn_Region_s_struct*)
_temp406)->f3; goto _LL449;} else{ goto _LL450;} _LL450: if(( unsigned int)
_temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_Decl_s: 0){ goto _LL451;} else{
goto _LL452;} _LL452: if(( unsigned int) _temp406 >  1u?*(( int*) _temp406) == 
Cyc_Absyn_SwitchC_s: 0){ goto _LL453;} else{ goto _LL454;} _LL454: if((
unsigned int) _temp406 >  1u?*(( int*) _temp406) ==  Cyc_Absyn_Cut_s: 0){ goto
_LL455;} else{ goto _LL456;} _LL456: if(( unsigned int) _temp406 >  1u?*(( int*)
_temp406) ==  Cyc_Absyn_Splice_s: 0){ goto _LL457;} else{ goto _LL407;} _LL409:
return; _LL411: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp560=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp560->hd=( void*) Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
env, _temp458)); _temp560->tl= 0; _temp560;})); return; _LL413: Cyc_NewControlFlow_cf_prepass(
env, _temp462); Cyc_NewControlFlow_cf_prepass( env, _temp460); return; _LL415:
return; _LL417: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp561=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp561->hd=( void*) Cyc_NewControlFlow_use_exp( env,( struct Cyc_Absyn_Exp*)
_check_null( _temp466)); _temp561->tl= 0; _temp561;})); return; _LL419: { void*
_temp564; void* _temp566; struct _tuple0 _temp562= Cyc_NewControlFlow_abstract_guard(
env, _temp472); _LL567: _temp566= _temp562.f1; goto _LL565; _LL565: _temp564=
_temp562.f2; goto _LL563; _LL563: Cyc_NewControlFlow_cf_set_absexps( s,({ struct
Cyc_List_List* _temp568=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp568->hd=( void*) _temp566; _temp568->tl=({ struct Cyc_List_List* _temp569=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp569->hd=(
void*) _temp564; _temp569->tl= 0; _temp569;}); _temp568;})); Cyc_NewControlFlow_cf_prepass(
env, _temp470); Cyc_NewControlFlow_cf_prepass( env, _temp468); return;} _LL421:
_temp488= _temp478; _temp486= _temp476; _temp482= _temp480; goto _LL423; _LL423: {
void* _temp572; void* _temp574; struct _tuple0 _temp570= Cyc_NewControlFlow_abstract_guard(
env, _temp488); _LL575: _temp574= _temp570.f1; goto _LL573; _LL573: _temp572=
_temp570.f2; goto _LL571; _LL571: Cyc_NewControlFlow_cf_set_absexps( s,({ struct
Cyc_List_List* _temp576=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp576->hd=( void*) _temp574; _temp576->tl=({ struct Cyc_List_List* _temp577=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp577->hd=(
void*) _temp572; _temp577->tl= 0; _temp577;}); _temp576;})); Cyc_NewControlFlow_cf_prepass(
env, _temp482); return;} _LL425: goto _LL427; _LL427: return; _LL429:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp578=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp578[ 0]=({ struct
Cyc_Core_Impossible_struct _temp579; _temp579.tag= Cyc_Core_Impossible; _temp579.f1=
_tag_arr("cf_prepass: goto w/o dest", sizeof( unsigned char), 26u); _temp579;});
_temp578;})); _LL431: { struct Cyc_Absyn_Stmt* _temp580=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* _temp581=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*) _check_null( _temp492)))->encloser; while( _temp581 != 
_temp580) { struct Cyc_Absyn_Stmt* _temp582=( Cyc_NewControlFlow_get_stmt_annot(
_temp580))->encloser; if( _temp582 ==  _temp580){({ void* _temp583[ 0u]={}; Cyc_Tcutil_terr(
s->loc, _tag_arr("goto enters local scope or exception handler", sizeof(
unsigned char), 45u), _tag_arr( _temp583, sizeof( void*), 0u));}); break;}
_temp580= _temp582;} return;} _LL433: { void* _temp584= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env, _temp508)); void* _temp587; void* _temp589;
struct _tuple0 _temp585= Cyc_NewControlFlow_abstract_guard( env, _temp506);
_LL590: _temp589= _temp585.f1; goto _LL588; _LL588: _temp587= _temp585.f2; goto
_LL586; _LL586: { void* _temp591= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
env, _temp500)); Cyc_NewControlFlow_cf_set_absexps( s,({ void* _temp592[ 4u];
_temp592[ 3u]= _temp591; _temp592[ 2u]= _temp587; _temp592[ 1u]= _temp589;
_temp592[ 0u]= _temp584; Cyc_List_list( _tag_arr( _temp592, sizeof( void*), 4u));}));
Cyc_NewControlFlow_cf_prepass( env, _temp494); return;}} _LL435: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp593=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp593->hd=( void*) Cyc_NewControlFlow_use_exp( env,
_temp512); _temp593->tl= Cyc_NewControlFlow_cf_prepass_scs( env, _temp510);
_temp593;})); return; _LL437: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp594=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp594->hd=( void*) Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp514)); _temp594->tl= 0; _temp594;})); return; _LL439: Cyc_NewControlFlow_add_var_root(
env, _temp523); if( _temp523->initializer !=  0){ struct Cyc_NewControlFlow_AbsSyn
_temp595= Cyc_NewControlFlow_abstract_exp( env,( struct Cyc_Absyn_Exp*)
_check_null( _temp523->initializer)); void* _temp596= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_NewControlFlow_meet_absexp( _temp595), Cyc_CfAbsexp_mkAssignAE(
Cyc_CfAbsexp_mkLocalOp( _temp523),( void*) _temp595.absop)); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp597=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp597->hd=( void*) _temp596; _temp597->tl= 0;
_temp597;}));} Cyc_NewControlFlow_cf_prepass( env, _temp516); return; _LL441:
for( 0; _temp537 !=  0; _temp537=(( struct Cyc_List_List*) _check_null( _temp537))->tl){
Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp537))->hd);} Cyc_NewControlFlow_cf_set_absexps( s,({ struct
Cyc_List_List* _temp598=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp598->hd=( void*) Cyc_NewControlFlow_use_exp( env, _temp532); _temp598->tl=
0; _temp598;})); Cyc_NewControlFlow_cf_prepass( env, _temp525); return; _LL443:
for( 0; _temp546 !=  0; _temp546=(( struct Cyc_List_List*) _check_null( _temp546))->tl){
Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp546))->hd);} Cyc_NewControlFlow_cf_prepass( env, _temp539);
return; _LL445: Cyc_NewControlFlow_cf_prepass( env, _temp548); return; _LL447:
Cyc_NewControlFlow_cf_prepass( env, _temp552); Cyc_NewControlFlow_cf_set_absexps(
s, Cyc_NewControlFlow_cf_prepass_scs( env, _temp550)); return; _LL449: Cyc_NewControlFlow_add_var_root(
env, _temp556); Cyc_NewControlFlow_cf_prepass( env, _temp554); return; _LL451:
goto _LL453; _LL453: goto _LL455; _LL455: goto _LL457; _LL457:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp599=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp599[ 0]=({ struct
Cyc_Core_Impossible_struct _temp600; _temp600.tag= Cyc_Core_Impossible; _temp600.f1=
_tag_arr("cf_prepass: bad stmt form", sizeof( unsigned char), 26u); _temp600;});
_temp599;})); _LL407:;} static int Cyc_NewControlFlow_iterate_cf_check= 0;
static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
env->all_changed, new_flow,( void*) env->tryflow));}} static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp601= Cyc_NewControlFlow_get_stmt_annot(
s); void* _temp602= Cyc_CfFlowInfo_join_flow( env->all_changed, flow,( void*)
_temp601->flow); if( ! Cyc_CfFlowInfo_flow_lessthan_approx( _temp602,( void*)
_temp601->flow)){( void*)( _temp601->flow=( void*) _temp602); if( _temp601->visited
==  Cyc_NewControlFlow_iteration_num){ Cyc_NewControlFlow_iterate_cf_check= 1;}}}
static struct Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv* env, void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp603= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( _temp603->flow=( void*) Cyc_CfFlowInfo_join_flow( env->all_changed,
in_flow,( void*) _temp603->flow)); ++ _temp603->visited; _temp603->visited == 
Cyc_NewControlFlow_iteration_num? 0:(((( int(*)( struct _tagged_arr assertion,
struct _tagged_arr file, unsigned int line)) Cyc_Std___assert_fail)( _tag_arr("annot->visited == iteration_num",
sizeof( unsigned char), 32u), _tag_arr("new_control_flow.cyc", sizeof(
unsigned char), 21u), 643), 0)); return _temp603;} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp604= flow; struct Cyc_Dict_Dict* _temp610; _LL606: if( _temp604 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL607;} else{ goto _LL608;} _LL608: if((
unsigned int) _temp604 >  1u?*(( int*) _temp604) ==  Cyc_CfFlowInfo_InitsFL: 0){
_LL611: _temp610=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp604)->f1; goto
_LL609;} else{ goto _LL605;} _LL607: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL609: for( 0; vds !=  0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp612=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp614=( struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp614[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp615; _temp615.tag= Cyc_CfFlowInfo_VarRoot;
_temp615.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
vds))->hd; _temp615;}); _temp614;}); void* _temp613= Cyc_CfFlowInfo_assign_unknown_dict((
void*) Cyc_CfFlowInfo_AllIL, Cyc_Dict_lookup( env->roots,( void*) _temp612));
_temp610= Cyc_Dict_insert( _temp610,( void*) _temp612, _temp613);} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp616=( struct Cyc_CfFlowInfo_InitsFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp616[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp617; _temp617.tag= Cyc_CfFlowInfo_InitsFL;
_temp617.f1= _temp610; _temp617;}); _temp616;}); _LL605:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp618= flow; struct Cyc_Dict_Dict* _temp624; _LL620: if( _temp618 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL621;} else{ goto _LL622;} _LL622: if((
unsigned int) _temp618 >  1u?*(( int*) _temp618) ==  Cyc_CfFlowInfo_InitsFL: 0){
_LL625: _temp624=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp618)->f1; goto
_LL623;} else{ goto _LL619;} _LL621: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL623: for( 0; vds !=  0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp626=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp627=( struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp627[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp628; _temp628.tag= Cyc_CfFlowInfo_VarRoot;
_temp628.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
vds))->hd; _temp628;}); _temp627;}); _temp624= Cyc_Dict_insert( _temp624,( void*)
_temp626, Cyc_Dict_lookup( env->roots,( void*) _temp626));} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp629=( struct Cyc_CfFlowInfo_InitsFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp629[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp630; _temp630.tag= Cyc_CfFlowInfo_InitsFL;
_temp630.f1= _temp624; _temp630;}); _temp629;}); _LL619:;} static struct _tuple0
Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if(( void*)((
struct Cyc_List_List*) _check_null( aes))->hd == ( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd){ void*
_temp631= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); return({ struct _tuple0 _temp632; _temp632.f1=
_temp631; _temp632.f2= _temp631; _temp632;});} return({ struct _tuple0 _temp633;
_temp633.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); _temp633.f2= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->hd, in_flow); _temp633;});} static void* Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs !=  0; scs=(( struct Cyc_List_List*)
_check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause _temp636; struct Cyc_Absyn_Stmt*
_temp637; struct Cyc_Absyn_Exp* _temp639; struct Cyc_Core_Opt* _temp641; struct
Cyc_Absyn_Switch_clause* _temp634=( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( scs))->hd; _temp636=* _temp634; _LL642: _temp641=
_temp636.pat_vars; goto _LL640; _LL640: _temp639= _temp636.where_clause; goto
_LL638; _LL638: _temp637= _temp636.body; goto _LL635; _LL635: { void* _temp643=
Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp641))->v); _temp643= Cyc_NewControlFlow_add_init_vars_flow(
env, _temp643,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp641))->v);{ void* body_outflow; if( _temp639 !=  0){ void* _temp646; void*
_temp648; struct _tuple0 _temp644= Cyc_NewControlFlow_cf_evalguard( env,((
struct Cyc_Absyn_Exp*) _check_null( _temp639))->loc, aes, _temp643); _LL649:
_temp648= _temp644.f1; goto _LL647; _LL647: _temp646= _temp644.f2; goto _LL645;
_LL645: aes=(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->tl; in_flow= _temp646; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp637, _temp648);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp637, _temp643);}{ void* _temp650= body_outflow; _LL652: if( _temp650
== ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL653;} else{ goto _LL654;} _LL654:
goto _LL655; _LL653: goto _LL651; _LL655: if((( struct Cyc_List_List*)
_check_null( scs))->tl ==  0){ return body_outflow;} else{ if(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( scs))->tl))->hd)->pat_vars))->v
!=  0){({ void* _temp656[ 0u]={}; Cyc_Tcutil_terr( _temp637->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp656, sizeof( void*), 0u));});}
else{({ void* _temp657[ 0u]={}; Cyc_Tcutil_warn( _temp637->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp657, sizeof( void*), 0u));});} Cyc_NewControlFlow_update_flow(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( scs))->tl))->hd)->body, body_outflow);} goto
_LL651; _LL651:;}}}} return( void*) Cyc_CfFlowInfo_BottomFL;} void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Absyn_Stmt* s, void* in_flow){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp658= Cyc_NewControlFlow_pre_stmt_check(
env, in_flow, s); in_flow=( void*) _temp658->flow;{ struct Cyc_List_List*
_temp659= _temp658->absexps; void* _temp660=( void*) s->r; struct Cyc_Absyn_Exp*
_temp706; struct Cyc_Absyn_Exp* _temp708; struct Cyc_Absyn_Exp* _temp710; struct
Cyc_Absyn_Stmt* _temp712; struct Cyc_Absyn_Stmt* _temp714; struct Cyc_Absyn_Stmt*
_temp716; struct Cyc_Absyn_Stmt* _temp718; struct Cyc_Absyn_Exp* _temp720;
struct Cyc_Absyn_Stmt* _temp722; struct _tuple3 _temp724; struct Cyc_Absyn_Stmt*
_temp726; struct Cyc_Absyn_Exp* _temp728; struct _tuple3 _temp730; struct Cyc_Absyn_Stmt*
_temp732; struct Cyc_Absyn_Exp* _temp734; struct Cyc_Absyn_Stmt* _temp736;
struct Cyc_Absyn_Stmt* _temp738; struct _tuple3 _temp740; struct Cyc_Absyn_Stmt*
_temp742; struct Cyc_Absyn_Exp* _temp744; struct _tuple3 _temp746; struct Cyc_Absyn_Stmt*
_temp748; struct Cyc_Absyn_Exp* _temp750; struct Cyc_Absyn_Exp* _temp752; struct
Cyc_Absyn_Stmt* _temp754; struct Cyc_Absyn_Switch_clause** _temp756; struct Cyc_Absyn_Switch_clause*
_temp758; struct Cyc_List_List* _temp759; struct Cyc_Absyn_Stmt* _temp761;
struct Cyc_Absyn_Stmt* _temp763; struct Cyc_Absyn_Stmt* _temp765; struct Cyc_List_List*
_temp767; struct Cyc_Absyn_Exp* _temp769; struct Cyc_List_List* _temp771; struct
Cyc_Absyn_Stmt* _temp773; struct Cyc_Absyn_Stmt* _temp775; struct Cyc_Absyn_Decl*
_temp777; struct Cyc_Absyn_Decl _temp779; void* _temp780; struct Cyc_Absyn_Vardecl*
_temp782; struct Cyc_Absyn_Stmt* _temp784; struct Cyc_Absyn_Decl* _temp786;
struct Cyc_Absyn_Decl _temp788; void* _temp789; struct Cyc_Absyn_Exp* _temp791;
struct Cyc_Core_Opt* _temp793; struct Cyc_Core_Opt _temp795; struct Cyc_List_List*
_temp796; struct Cyc_Absyn_Stmt* _temp798; struct Cyc_Absyn_Decl* _temp800;
struct Cyc_Absyn_Decl _temp802; void* _temp803; struct Cyc_List_List* _temp805;
struct Cyc_Absyn_Stmt* _temp807; struct Cyc_Absyn_Stmt* _temp809; struct Cyc_Absyn_Vardecl*
_temp811; struct Cyc_Absyn_Tvar* _temp813; _LL662: if( _temp660 == ( void*) Cyc_Absyn_Skip_s){
goto _LL663;} else{ goto _LL664;} _LL664: if(( unsigned int) _temp660 >  1u?*((
int*) _temp660) ==  Cyc_Absyn_Return_s: 0){ _LL707: _temp706=(( struct Cyc_Absyn_Return_s_struct*)
_temp660)->f1; if( _temp706 ==  0){ goto _LL665;} else{ goto _LL666;}} else{
goto _LL666;} _LL666: if(( unsigned int) _temp660 >  1u?*(( int*) _temp660) == 
Cyc_Absyn_Return_s: 0){ _LL709: _temp708=(( struct Cyc_Absyn_Return_s_struct*)
_temp660)->f1; goto _LL667;} else{ goto _LL668;} _LL668: if(( unsigned int)
_temp660 >  1u?*(( int*) _temp660) ==  Cyc_Absyn_Exp_s: 0){ _LL711: _temp710=((
struct Cyc_Absyn_Exp_s_struct*) _temp660)->f1; goto _LL669;} else{ goto _LL670;}
_LL670: if(( unsigned int) _temp660 >  1u?*(( int*) _temp660) ==  Cyc_Absyn_Seq_s:
0){ _LL715: _temp714=(( struct Cyc_Absyn_Seq_s_struct*) _temp660)->f1; goto
_LL713; _LL713: _temp712=(( struct Cyc_Absyn_Seq_s_struct*) _temp660)->f2; goto
_LL671;} else{ goto _LL672;} _LL672: if(( unsigned int) _temp660 >  1u?*(( int*)
_temp660) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL721: _temp720=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp660)->f1; goto _LL719; _LL719: _temp718=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp660)->f2; goto _LL717; _LL717: _temp716=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp660)->f3; goto _LL673;} else{ goto _LL674;} _LL674: if(( unsigned int)
_temp660 >  1u?*(( int*) _temp660) ==  Cyc_Absyn_While_s: 0){ _LL725: _temp724=((
struct Cyc_Absyn_While_s_struct*) _temp660)->f1; _LL729: _temp728= _temp724.f1;
goto _LL727; _LL727: _temp726= _temp724.f2; goto _LL723; _LL723: _temp722=((
struct Cyc_Absyn_While_s_struct*) _temp660)->f2; goto _LL675;} else{ goto _LL676;}
_LL676: if(( unsigned int) _temp660 >  1u?*(( int*) _temp660) ==  Cyc_Absyn_Do_s:
0){ _LL737: _temp736=(( struct Cyc_Absyn_Do_s_struct*) _temp660)->f1; goto
_LL731; _LL731: _temp730=(( struct Cyc_Absyn_Do_s_struct*) _temp660)->f2; _LL735:
_temp734= _temp730.f1; goto _LL733; _LL733: _temp732= _temp730.f2; goto _LL677;}
else{ goto _LL678;} _LL678: if(( unsigned int) _temp660 >  1u?*(( int*) _temp660)
==  Cyc_Absyn_For_s: 0){ _LL753: _temp752=(( struct Cyc_Absyn_For_s_struct*)
_temp660)->f1; goto _LL747; _LL747: _temp746=(( struct Cyc_Absyn_For_s_struct*)
_temp660)->f2; _LL751: _temp750= _temp746.f1; goto _LL749; _LL749: _temp748=
_temp746.f2; goto _LL741; _LL741: _temp740=(( struct Cyc_Absyn_For_s_struct*)
_temp660)->f3; _LL745: _temp744= _temp740.f1; goto _LL743; _LL743: _temp742=
_temp740.f2; goto _LL739; _LL739: _temp738=(( struct Cyc_Absyn_For_s_struct*)
_temp660)->f4; goto _LL679;} else{ goto _LL680;} _LL680: if(( unsigned int)
_temp660 >  1u?*(( int*) _temp660) ==  Cyc_Absyn_Break_s: 0){ _LL755: _temp754=((
struct Cyc_Absyn_Break_s_struct*) _temp660)->f1; if( _temp754 ==  0){ goto
_LL681;} else{ goto _LL682;}} else{ goto _LL682;} _LL682: if(( unsigned int)
_temp660 >  1u?*(( int*) _temp660) ==  Cyc_Absyn_Fallthru_s: 0){ _LL760:
_temp759=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp660)->f1; goto _LL757;
_LL757: _temp756=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp660)->f2; if(
_temp756 ==  0){ goto _LL684;} else{ _temp758=* _temp756; goto _LL683;}} else{
goto _LL684;} _LL684: if(( unsigned int) _temp660 >  1u?*(( int*) _temp660) == 
Cyc_Absyn_Break_s: 0){ _LL762: _temp761=(( struct Cyc_Absyn_Break_s_struct*)
_temp660)->f1; goto _LL685;} else{ goto _LL686;} _LL686: if(( unsigned int)
_temp660 >  1u?*(( int*) _temp660) ==  Cyc_Absyn_Continue_s: 0){ _LL764:
_temp763=(( struct Cyc_Absyn_Continue_s_struct*) _temp660)->f1; goto _LL687;}
else{ goto _LL688;} _LL688: if(( unsigned int) _temp660 >  1u?*(( int*) _temp660)
==  Cyc_Absyn_Goto_s: 0){ _LL766: _temp765=(( struct Cyc_Absyn_Goto_s_struct*)
_temp660)->f2; goto _LL689;} else{ goto _LL690;} _LL690: if(( unsigned int)
_temp660 >  1u?*(( int*) _temp660) ==  Cyc_Absyn_Switch_s: 0){ _LL770: _temp769=((
struct Cyc_Absyn_Switch_s_struct*) _temp660)->f1; goto _LL768; _LL768: _temp767=((
struct Cyc_Absyn_Switch_s_struct*) _temp660)->f2; goto _LL691;} else{ goto
_LL692;} _LL692: if(( unsigned int) _temp660 >  1u?*(( int*) _temp660) ==  Cyc_Absyn_TryCatch_s:
0){ _LL774: _temp773=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp660)->f1; goto
_LL772; _LL772: _temp771=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp660)->f2;
goto _LL693;} else{ goto _LL694;} _LL694: if(( unsigned int) _temp660 >  1u?*((
int*) _temp660) ==  Cyc_Absyn_Decl_s: 0){ _LL778: _temp777=(( struct Cyc_Absyn_Decl_s_struct*)
_temp660)->f1; _temp779=* _temp777; _LL781: _temp780=( void*) _temp779.r; if(*((
int*) _temp780) ==  Cyc_Absyn_Var_d){ _LL783: _temp782=(( struct Cyc_Absyn_Var_d_struct*)
_temp780)->f1; goto _LL776;} else{ goto _LL696;} _LL776: _temp775=(( struct Cyc_Absyn_Decl_s_struct*)
_temp660)->f2; goto _LL695;} else{ goto _LL696;} _LL696: if(( unsigned int)
_temp660 >  1u?*(( int*) _temp660) ==  Cyc_Absyn_Decl_s: 0){ _LL787: _temp786=((
struct Cyc_Absyn_Decl_s_struct*) _temp660)->f1; _temp788=* _temp786; _LL790:
_temp789=( void*) _temp788.r; if(*(( int*) _temp789) ==  Cyc_Absyn_Let_d){
_LL794: _temp793=(( struct Cyc_Absyn_Let_d_struct*) _temp789)->f2; if( _temp793
==  0){ goto _LL698;} else{ _temp795=* _temp793; _LL797: _temp796=( struct Cyc_List_List*)
_temp795.v; goto _LL792;} _LL792: _temp791=(( struct Cyc_Absyn_Let_d_struct*)
_temp789)->f4; goto _LL785;} else{ goto _LL698;} _LL785: _temp784=(( struct Cyc_Absyn_Decl_s_struct*)
_temp660)->f2; goto _LL697;} else{ goto _LL698;} _LL698: if(( unsigned int)
_temp660 >  1u?*(( int*) _temp660) ==  Cyc_Absyn_Decl_s: 0){ _LL801: _temp800=((
struct Cyc_Absyn_Decl_s_struct*) _temp660)->f1; _temp802=* _temp800; _LL804:
_temp803=( void*) _temp802.r; if(*(( int*) _temp803) ==  Cyc_Absyn_Letv_d){
_LL806: _temp805=(( struct Cyc_Absyn_Letv_d_struct*) _temp803)->f1; goto _LL799;}
else{ goto _LL700;} _LL799: _temp798=(( struct Cyc_Absyn_Decl_s_struct*)
_temp660)->f2; goto _LL699;} else{ goto _LL700;} _LL700: if(( unsigned int)
_temp660 >  1u?*(( int*) _temp660) ==  Cyc_Absyn_Label_s: 0){ _LL808: _temp807=((
struct Cyc_Absyn_Label_s_struct*) _temp660)->f2; goto _LL701;} else{ goto _LL702;}
_LL702: if(( unsigned int) _temp660 >  1u?*(( int*) _temp660) ==  Cyc_Absyn_Region_s:
0){ _LL814: _temp813=(( struct Cyc_Absyn_Region_s_struct*) _temp660)->f1; goto
_LL812; _LL812: _temp811=(( struct Cyc_Absyn_Region_s_struct*) _temp660)->f2;
goto _LL810; _LL810: _temp809=(( struct Cyc_Absyn_Region_s_struct*) _temp660)->f3;
goto _LL703;} else{ goto _LL704;} _LL704: goto _LL705; _LL663: return in_flow;
_LL665: return( void*) Cyc_CfFlowInfo_BottomFL; _LL667: Cyc_CfAbsexp_eval_absexp(
env,(( struct Cyc_Absyn_Exp*) _check_null( _temp708))->loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp659))->hd, in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL669: return Cyc_CfAbsexp_eval_absexp( env, _temp710->loc,( void*)(( struct
Cyc_List_List*) _check_null( _temp659))->hd, in_flow); _LL671: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp712, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp714, in_flow));
_LL673: { void* _temp817; void* _temp819; struct _tuple0 _temp815= Cyc_NewControlFlow_cf_evalguard(
env, _temp720->loc, _temp659, in_flow); _LL820: _temp819= _temp815.f1; goto
_LL818; _LL818: _temp817= _temp815.f2; goto _LL816; _LL816: return Cyc_CfFlowInfo_join_flow(
env->all_changed, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp718, _temp819),
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp716, _temp817));} _LL675: { void*
_temp821=( void*)( Cyc_NewControlFlow_pre_stmt_check( env, in_flow, _temp726))->flow;
void* _temp824; void* _temp826; struct _tuple0 _temp822= Cyc_NewControlFlow_cf_evalguard(
env, _temp728->loc, _temp659, _temp821); _LL827: _temp826= _temp822.f1; goto
_LL825; _LL825: _temp824= _temp822.f2; goto _LL823; _LL823: { void* _temp828=
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp722, _temp826); Cyc_NewControlFlow_update_flow(
env, _temp726, _temp828); return _temp824;}} _LL677: { void* _temp829= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp736, in_flow); void* _temp830=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp829, _temp732))->flow; void* _temp833; void* _temp835; struct _tuple0
_temp831= Cyc_NewControlFlow_cf_evalguard( env, _temp734->loc, _temp659,
_temp830); _LL836: _temp835= _temp831.f1; goto _LL834; _LL834: _temp833=
_temp831.f2; goto _LL832; _LL832: Cyc_NewControlFlow_update_flow( env, _temp736,
_temp835); return _temp833;} _LL679: { void* _temp837= Cyc_CfAbsexp_eval_absexp(
env, _temp752->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp659))->hd,
in_flow); void* _temp838=( void*)( Cyc_NewControlFlow_pre_stmt_check( env,
_temp837, _temp748))->flow; void* _temp841; void* _temp843; struct _tuple0
_temp839= Cyc_NewControlFlow_cf_evalguard( env, _temp750->loc,(( struct Cyc_List_List*)
_check_null( _temp659))->tl, _temp838); _LL844: _temp843= _temp839.f1; goto
_LL842; _LL842: _temp841= _temp839.f2; goto _LL840; _LL840: { void* _temp845=
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp738, _temp843); void* _temp846=(
void*)( Cyc_NewControlFlow_pre_stmt_check( env, _temp845, _temp742))->flow; void*
_temp847= Cyc_CfAbsexp_eval_absexp( env, _temp744->loc,( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp659))->tl))->tl))->tl))->hd,
_temp846); Cyc_NewControlFlow_update_flow( env, _temp748, _temp847); return
_temp841;}} _LL681: return( void*) Cyc_CfFlowInfo_BottomFL; _LL683: { void*
_temp848= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp659))->hd, in_flow); struct Cyc_List_List* _temp849=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp758->pat_vars))->v;
_temp848= Cyc_NewControlFlow_add_vardecls_flow( env, _temp848, _temp849);
_temp848= Cyc_NewControlFlow_add_init_vars_flow( env, _temp848, _temp849); Cyc_NewControlFlow_update_flow(
env,( struct Cyc_Absyn_Stmt*) _temp758->body, _temp848); return( void*) Cyc_CfFlowInfo_BottomFL;}
_LL685: _temp763= _temp761; goto _LL687; _LL687: _temp765= _temp763; goto _LL689;
_LL689: Cyc_NewControlFlow_update_flow( env,( struct Cyc_Absyn_Stmt*)
_check_null( _temp765), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL691:
return Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp767,(( struct Cyc_List_List*)
_check_null( _temp659))->tl, Cyc_CfAbsexp_eval_absexp( env, _temp769->loc,( void*)((
struct Cyc_List_List*) _check_null( _temp659))->hd, in_flow)); _LL693: { struct
Cyc_NewControlFlow_AnalEnv _temp852; void* _temp853; int _temp855; struct Cyc_NewControlFlow_AnalEnv*
_temp850= env; _temp852=* _temp850; _LL856: _temp855= _temp852.in_try; goto
_LL854; _LL854: _temp853=( void*) _temp852.tryflow; goto _LL851; _LL851: env->in_try=
1;( void*)( env->tryflow=( void*) in_flow);{ void* _temp857= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp773, in_flow); void* _temp858=( void*) env->tryflow; env->in_try=
_temp855;( void*)( env->tryflow=( void*) _temp853); Cyc_NewControlFlow_update_tryflow(
env, _temp858);{ void* _temp859= Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp771, _temp659, _temp858); return Cyc_CfFlowInfo_join_flow( env->all_changed,
_temp857, _temp859);}}} _LL695: { struct Cyc_List_List _temp860=({ struct Cyc_List_List
_temp863; _temp863.hd=( void*) _temp782; _temp863.tl= 0; _temp863;}); void*
_temp861= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)&
_temp860); struct Cyc_Absyn_Exp* _temp862= _temp782->initializer; if( _temp862
!=  0){ _temp861= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)
_check_null( _temp862))->loc,( void*)(( struct Cyc_List_List*) _check_null(
_temp659))->hd, _temp861);} return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp775, _temp861);} _LL697: { void* _temp864= Cyc_CfAbsexp_eval_absexp( env,
_temp791->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp659))->hd, Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp796)); _temp864= Cyc_NewControlFlow_add_init_vars_flow( env,
_temp864, _temp796); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp784,
_temp864);} _LL699: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp798,
Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, _temp805)); _LL701: return
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp807, in_flow); _LL703: { struct
Cyc_List_List* _temp865=({ struct Cyc_List_List* _temp867=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp867->hd=( void*) _temp811;
_temp867->tl= 0; _temp867;}); void* _temp866= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp865); _temp866= Cyc_NewControlFlow_add_init_vars_flow( env,
_temp866, _temp865); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp809,
_temp866);} _LL705:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp868=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp868[ 0]=({ struct Cyc_Core_Impossible_struct _temp869; _temp869.tag= Cyc_Core_Impossible;
_temp869.f1= _tag_arr("cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof( unsigned char), 62u); _temp869;}); _temp868;})); _LL661:;}} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{
void* _temp870=( void*) s->r; struct Cyc_Absyn_Stmt* _temp894; struct Cyc_Absyn_Stmt*
_temp896; struct Cyc_Absyn_Stmt* _temp898; struct Cyc_Absyn_Stmt* _temp900;
struct Cyc_Absyn_Stmt* _temp902; struct _tuple3 _temp904; struct Cyc_Absyn_Stmt*
_temp906; struct _tuple3 _temp908; struct Cyc_Absyn_Stmt* _temp910; struct Cyc_Absyn_Stmt*
_temp912; struct _tuple3 _temp914; struct Cyc_Absyn_Stmt* _temp916; struct
_tuple3 _temp918; struct Cyc_Absyn_Stmt* _temp920; struct Cyc_Absyn_Stmt*
_temp922; struct Cyc_Absyn_Stmt* _temp924; struct Cyc_Absyn_Stmt* _temp926;
struct Cyc_Absyn_Stmt* _temp928; struct Cyc_List_List* _temp930; struct Cyc_Absyn_Stmt*
_temp932; struct Cyc_List_List* _temp934; _LL872: if(( unsigned int) _temp870 > 
1u?*(( int*) _temp870) ==  Cyc_Absyn_Seq_s: 0){ _LL897: _temp896=(( struct Cyc_Absyn_Seq_s_struct*)
_temp870)->f1; goto _LL895; _LL895: _temp894=(( struct Cyc_Absyn_Seq_s_struct*)
_temp870)->f2; goto _LL873;} else{ goto _LL874;} _LL874: if(( unsigned int)
_temp870 >  1u?*(( int*) _temp870) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL901:
_temp900=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp870)->f2; goto _LL899;
_LL899: _temp898=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp870)->f3; goto
_LL875;} else{ goto _LL876;} _LL876: if(( unsigned int) _temp870 >  1u?*(( int*)
_temp870) ==  Cyc_Absyn_For_s: 0){ _LL909: _temp908=(( struct Cyc_Absyn_For_s_struct*)
_temp870)->f2; _LL911: _temp910= _temp908.f2; goto _LL905; _LL905: _temp904=((
struct Cyc_Absyn_For_s_struct*) _temp870)->f3; _LL907: _temp906= _temp904.f2;
goto _LL903; _LL903: _temp902=(( struct Cyc_Absyn_For_s_struct*) _temp870)->f4;
goto _LL877;} else{ goto _LL878;} _LL878: if(( unsigned int) _temp870 >  1u?*((
int*) _temp870) ==  Cyc_Absyn_While_s: 0){ _LL915: _temp914=(( struct Cyc_Absyn_While_s_struct*)
_temp870)->f1; _LL917: _temp916= _temp914.f2; goto _LL913; _LL913: _temp912=((
struct Cyc_Absyn_While_s_struct*) _temp870)->f2; goto _LL879;} else{ goto _LL880;}
_LL880: if(( unsigned int) _temp870 >  1u?*(( int*) _temp870) ==  Cyc_Absyn_Do_s:
0){ _LL923: _temp922=(( struct Cyc_Absyn_Do_s_struct*) _temp870)->f1; goto
_LL919; _LL919: _temp918=(( struct Cyc_Absyn_Do_s_struct*) _temp870)->f2; _LL921:
_temp920= _temp918.f2; goto _LL881;} else{ goto _LL882;} _LL882: if((
unsigned int) _temp870 >  1u?*(( int*) _temp870) ==  Cyc_Absyn_Region_s: 0){
_LL925: _temp924=(( struct Cyc_Absyn_Region_s_struct*) _temp870)->f3; goto
_LL883;} else{ goto _LL884;} _LL884: if(( unsigned int) _temp870 >  1u?*(( int*)
_temp870) ==  Cyc_Absyn_Label_s: 0){ _LL927: _temp926=(( struct Cyc_Absyn_Label_s_struct*)
_temp870)->f2; goto _LL885;} else{ goto _LL886;} _LL886: if(( unsigned int)
_temp870 >  1u?*(( int*) _temp870) ==  Cyc_Absyn_Decl_s: 0){ _LL929: _temp928=((
struct Cyc_Absyn_Decl_s_struct*) _temp870)->f2; goto _LL887;} else{ goto _LL888;}
_LL888: if(( unsigned int) _temp870 >  1u?*(( int*) _temp870) ==  Cyc_Absyn_TryCatch_s:
0){ _LL933: _temp932=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp870)->f1; goto
_LL931; _LL931: _temp930=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp870)->f2;
goto _LL889;} else{ goto _LL890;} _LL890: if(( unsigned int) _temp870 >  1u?*((
int*) _temp870) ==  Cyc_Absyn_Switch_s: 0){ _LL935: _temp934=(( struct Cyc_Absyn_Switch_s_struct*)
_temp870)->f2; goto _LL891;} else{ goto _LL892;} _LL892: goto _LL893; _LL873:
_temp900= _temp896; _temp898= _temp894; goto _LL875; _LL875: Cyc_NewControlFlow_cf_postpass(
_temp900); Cyc_NewControlFlow_cf_postpass( _temp898); return; _LL877: Cyc_NewControlFlow_cf_postpass(
_temp906); _temp916= _temp902; _temp912= _temp910; goto _LL879; _LL879: _temp922=
_temp912; _temp920= _temp916; goto _LL881; _LL881: Cyc_NewControlFlow_cf_postpass(
_temp920); _temp924= _temp922; goto _LL883; _LL883: _temp926= _temp924; goto
_LL885; _LL885: _temp928= _temp926; goto _LL887; _LL887: Cyc_NewControlFlow_cf_postpass(
_temp928); return; _LL889: Cyc_NewControlFlow_cf_postpass( _temp932); _temp934=
_temp930; goto _LL891; _LL891: for( 0; _temp934 !=  0; _temp934=(( struct Cyc_List_List*)
_check_null( _temp934))->tl){ Cyc_NewControlFlow_cf_postpass((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp934))->hd)->body);} return; _LL893:
return; _LL871:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv* absenv=({ struct Cyc_NewControlFlow_AbsEnv*
_temp948=( struct Cyc_NewControlFlow_AbsEnv*) _cycalloc( sizeof( struct Cyc_NewControlFlow_AbsEnv));
_temp948->all_roots= Cyc_Dict_empty( Cyc_CfFlowInfo_local_root_cmp); _temp948;});
struct Cyc_List_List* _temp936=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( fd->param_vardecls))->v;{ struct Cyc_List_List* vds= _temp936; for(
0; vds !=  0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root(
absenv,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd);}}
Cyc_NewControlFlow_cf_prepass( absenv, fd->body);{ struct Cyc_NewControlFlow_AnalEnv*
_temp937=({ struct Cyc_NewControlFlow_AnalEnv* _temp947=( struct Cyc_NewControlFlow_AnalEnv*)
_cycalloc( sizeof( struct Cyc_NewControlFlow_AnalEnv)); _temp947->roots= absenv->all_roots;
_temp947->in_try= 0; _temp947->tryflow=( void*)(( void*) Cyc_CfFlowInfo_BottomFL);
_temp947->all_changed= 0; _temp947;}); void* in_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp945=( struct Cyc_CfFlowInfo_InitsFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp945[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp946; _temp946.tag= Cyc_CfFlowInfo_InitsFL;
_temp946.f1= Cyc_Dict_empty( Cyc_CfFlowInfo_local_root_cmp); _temp946;});
_temp945;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow( _temp937, in_flow,
_temp936);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num= 0; Cyc_NewControlFlow_iterate_cf_check=
1; while( Cyc_NewControlFlow_iterate_cf_check) { ++ Cyc_NewControlFlow_iteration_num;
Cyc_NewControlFlow_iterate_cf_check= 0; out_flow= Cyc_NewControlFlow_cf_analyze_stmt(
_temp937, fd->body, in_flow);}{ void* _temp938= out_flow; _LL940: if( _temp938
== ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL941;} else{ goto _LL942;} _LL942:
goto _LL943; _LL941: goto _LL939; _LL943: if(( void*) fd->ret_type != ( void*)
Cyc_Absyn_VoidType){({ void* _temp944[ 0u]={}; Cyc_Tcutil_terr(( fd->body)->loc,
_tag_arr("function may complete without returning a value", sizeof(
unsigned char), 48u), _tag_arr( _temp944, sizeof( void*), 0u));});} goto _LL939;
_LL939:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds !=  0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp949=( void*)(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd)->r; struct Cyc_Absyn_Fndecl*
_temp961; struct Cyc_List_List* _temp963; struct Cyc_List_List* _temp965; struct
Cyc_List_List* _temp967; _LL951: if(*(( int*) _temp949) ==  Cyc_Absyn_Fn_d){
_LL962: _temp961=(( struct Cyc_Absyn_Fn_d_struct*) _temp949)->f1; goto _LL952;}
else{ goto _LL953;} _LL953: if(*(( int*) _temp949) ==  Cyc_Absyn_ExternC_d){
_LL964: _temp963=(( struct Cyc_Absyn_ExternC_d_struct*) _temp949)->f1; goto
_LL954;} else{ goto _LL955;} _LL955: if(*(( int*) _temp949) ==  Cyc_Absyn_Using_d){
_LL966: _temp965=(( struct Cyc_Absyn_Using_d_struct*) _temp949)->f2; goto _LL956;}
else{ goto _LL957;} _LL957: if(*(( int*) _temp949) ==  Cyc_Absyn_Namespace_d){
_LL968: _temp967=(( struct Cyc_Absyn_Namespace_d_struct*) _temp949)->f2; goto
_LL958;} else{ goto _LL959;} _LL959: goto _LL960; _LL952: Cyc_NewControlFlow_cf_check_fun(
_temp961); goto _LL950; _LL954: _temp965= _temp963; goto _LL956; _LL956:
_temp967= _temp965; goto _LL958; _LL958: Cyc_NewControlFlow_cf_check( _temp967);
goto _LL950; _LL960: goto _LL950; _LL950:;}}
