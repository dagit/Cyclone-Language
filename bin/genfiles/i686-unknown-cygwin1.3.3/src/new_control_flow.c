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
Cyc_Absyn_Exp* e){ void* _temp24=( void*) e->r; struct Cyc_Absyn_Exp* _temp118;
struct Cyc_Absyn_Exp* _temp120; struct Cyc_Absyn_Exp* _temp122; void* _temp124;
void* _temp126; int _temp128; void* _temp130; void* _temp132; int _temp134; void*
_temp136; void* _temp138; void* _temp140; void* _temp142; struct Cyc_Absyn_Vardecl*
_temp144; void* _temp146; struct Cyc_Absyn_Vardecl* _temp148; void* _temp150;
struct Cyc_Absyn_Vardecl* _temp152; struct Cyc_Absyn_Exp* _temp154; struct Cyc_Core_Opt*
_temp156; struct Cyc_Absyn_Exp* _temp158; struct Cyc_List_List* _temp160; struct
Cyc_Absyn_Exp* _temp162; struct Cyc_Absyn_Exp* _temp164; struct Cyc_Absyn_Exp*
_temp166; struct Cyc_List_List* _temp168; struct Cyc_Absyn_Exp* _temp170; struct
Cyc_Absyn_Exp* _temp172; struct Cyc_Absyn_Exp* _temp174; struct Cyc_Absyn_Exp*
_temp176; struct _tagged_arr* _temp178; struct Cyc_Absyn_Exp* _temp180; struct
_tagged_arr* _temp182; struct Cyc_Absyn_Exp* _temp184; struct Cyc_Absyn_Exp*
_temp186; struct Cyc_Absyn_Exp* _temp188; void* _temp190; struct Cyc_Absyn_Exp*
_temp192; struct Cyc_List_List* _temp194; struct Cyc_List_List* _temp196; struct
Cyc_List_List* _temp198; struct Cyc_List_List* _temp200; struct Cyc_List_List*
_temp202; struct Cyc_Absyn_Exp* _temp204; struct Cyc_Absyn_Exp* _temp206; struct
Cyc_Absyn_Exp* _temp208; struct Cyc_Absyn_Exp* _temp210; struct Cyc_Absyn_Exp*
_temp212; struct Cyc_Absyn_Stmt* _temp214; struct Cyc_Absyn_Exp* _temp216;
struct Cyc_Absyn_Exp* _temp218; struct Cyc_Absyn_Exp* _temp220; struct Cyc_Absyn_Exp*
_temp222; struct Cyc_Absyn_Vardecl* _temp224; void* _temp226; _LL26: if(*(( int*)
_temp24) ==  Cyc_Absyn_Cast_e){ _LL119: _temp118=(( struct Cyc_Absyn_Cast_e_struct*)
_temp24)->f2; goto _LL27;} else{ goto _LL28;} _LL28: if(*(( int*) _temp24) == 
Cyc_Absyn_NoInstantiate_e){ _LL121: _temp120=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp24)->f1; goto _LL29;} else{ goto _LL30;} _LL30: if(*(( int*) _temp24) == 
Cyc_Absyn_Instantiate_e){ _LL123: _temp122=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp24)->f1; goto _LL31;} else{ goto _LL32;} _LL32: if(*(( int*) _temp24) == 
Cyc_Absyn_Const_e){ _LL125: _temp124=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp24)->f1; if( _temp124 == ( void*) Cyc_Absyn_Null_c){ goto _LL33;} else{
goto _LL34;}} else{ goto _LL34;} _LL34: if(*(( int*) _temp24) ==  Cyc_Absyn_Const_e){
_LL127: _temp126=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp24)->f1; if((
unsigned int) _temp126 >  1u?*(( int*) _temp126) ==  Cyc_Absyn_Int_c: 0){ _LL131:
_temp130=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp126)->f1; if( _temp130
== ( void*) Cyc_Absyn_Signed){ goto _LL129;} else{ goto _LL36;} _LL129: _temp128=((
struct Cyc_Absyn_Int_c_struct*) _temp126)->f2; if( _temp128 ==  0){ goto _LL35;}
else{ goto _LL36;}} else{ goto _LL36;}} else{ goto _LL36;} _LL36: if(*(( int*)
_temp24) ==  Cyc_Absyn_Const_e){ _LL133: _temp132=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp24)->f1; if(( unsigned int) _temp132 >  1u?*(( int*) _temp132) ==  Cyc_Absyn_Int_c:
0){ _LL137: _temp136=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp132)->f1;
if( _temp136 == ( void*) Cyc_Absyn_Signed){ goto _LL135;} else{ goto _LL38;}
_LL135: _temp134=(( struct Cyc_Absyn_Int_c_struct*) _temp132)->f2; if( _temp134
==  1){ goto _LL37;} else{ goto _LL38;}} else{ goto _LL38;}} else{ goto _LL38;}
_LL38: if(*(( int*) _temp24) ==  Cyc_Absyn_Const_e){ goto _LL39;} else{ goto
_LL40;} _LL40: if(*(( int*) _temp24) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL41;}
else{ goto _LL42;} _LL42: if(*(( int*) _temp24) ==  Cyc_Absyn_Sizeoftyp_e){ goto
_LL43;} else{ goto _LL44;} _LL44: if(*(( int*) _temp24) ==  Cyc_Absyn_Offsetof_e){
goto _LL45;} else{ goto _LL46;} _LL46: if(*(( int*) _temp24) ==  Cyc_Absyn_Gentyp_e){
goto _LL47;} else{ goto _LL48;} _LL48: if(*(( int*) _temp24) ==  Cyc_Absyn_Enum_e){
goto _LL49;} else{ goto _LL50;} _LL50: if(*(( int*) _temp24) ==  Cyc_Absyn_Var_e){
_LL139: _temp138=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp24)->f2; if((
unsigned int) _temp138 >  1u?*(( int*) _temp138) ==  Cyc_Absyn_Funname_b: 0){
goto _LL51;} else{ goto _LL52;}} else{ goto _LL52;} _LL52: if(*(( int*) _temp24)
==  Cyc_Absyn_Var_e){ _LL141: _temp140=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if(( unsigned int) _temp140 >  1u?*(( int*) _temp140) ==  Cyc_Absyn_Global_b:
0){ goto _LL53;} else{ goto _LL54;}} else{ goto _LL54;} _LL54: if(*(( int*)
_temp24) ==  Cyc_Absyn_Var_e){ _LL143: _temp142=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if(( unsigned int) _temp142 >  1u?*(( int*) _temp142) ==  Cyc_Absyn_Param_b:
0){ _LL145: _temp144=(( struct Cyc_Absyn_Param_b_struct*) _temp142)->f1; goto
_LL55;} else{ goto _LL56;}} else{ goto _LL56;} _LL56: if(*(( int*) _temp24) == 
Cyc_Absyn_Var_e){ _LL147: _temp146=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if(( unsigned int) _temp146 >  1u?*(( int*) _temp146) ==  Cyc_Absyn_Local_b:
0){ _LL149: _temp148=(( struct Cyc_Absyn_Local_b_struct*) _temp146)->f1; goto
_LL57;} else{ goto _LL58;}} else{ goto _LL58;} _LL58: if(*(( int*) _temp24) == 
Cyc_Absyn_Var_e){ _LL151: _temp150=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if(( unsigned int) _temp150 >  1u?*(( int*) _temp150) ==  Cyc_Absyn_Pat_b:
0){ _LL153: _temp152=(( struct Cyc_Absyn_Pat_b_struct*) _temp150)->f1; goto
_LL59;} else{ goto _LL60;}} else{ goto _LL60;} _LL60: if(*(( int*) _temp24) == 
Cyc_Absyn_AssignOp_e){ _LL159: _temp158=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp24)->f1; goto _LL157; _LL157: _temp156=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp24)->f2; if( _temp156 ==  0){ goto _LL155;} else{ goto _LL62;} _LL155:
_temp154=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp24)->f3; goto _LL61;}
else{ goto _LL62;} _LL62: if(*(( int*) _temp24) ==  Cyc_Absyn_FnCall_e){ _LL163:
_temp162=(( struct Cyc_Absyn_FnCall_e_struct*) _temp24)->f1; goto _LL161; _LL161:
_temp160=(( struct Cyc_Absyn_FnCall_e_struct*) _temp24)->f2; goto _LL63;} else{
goto _LL64;} _LL64: if(*(( int*) _temp24) ==  Cyc_Absyn_AssignOp_e){ _LL167:
_temp166=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp24)->f1; goto _LL165;
_LL165: _temp164=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp24)->f3; goto
_LL65;} else{ goto _LL66;} _LL66: if(*(( int*) _temp24) ==  Cyc_Absyn_Primop_e){
_LL169: _temp168=(( struct Cyc_Absyn_Primop_e_struct*) _temp24)->f2; goto _LL67;}
else{ goto _LL68;} _LL68: if(*(( int*) _temp24) ==  Cyc_Absyn_Address_e){ _LL171:
_temp170=(( struct Cyc_Absyn_Address_e_struct*) _temp24)->f1; goto _LL69;} else{
goto _LL70;} _LL70: if(*(( int*) _temp24) ==  Cyc_Absyn_Subscript_e){ _LL175:
_temp174=(( struct Cyc_Absyn_Subscript_e_struct*) _temp24)->f1; goto _LL173;
_LL173: _temp172=(( struct Cyc_Absyn_Subscript_e_struct*) _temp24)->f2; goto
_LL71;} else{ goto _LL72;} _LL72: if(*(( int*) _temp24) ==  Cyc_Absyn_Deref_e){
_LL177: _temp176=(( struct Cyc_Absyn_Deref_e_struct*) _temp24)->f1; goto _LL73;}
else{ goto _LL74;} _LL74: if(*(( int*) _temp24) ==  Cyc_Absyn_StructArrow_e){
_LL181: _temp180=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp24)->f1; goto
_LL179; _LL179: _temp178=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp24)->f2;
goto _LL75;} else{ goto _LL76;} _LL76: if(*(( int*) _temp24) ==  Cyc_Absyn_StructMember_e){
_LL185: _temp184=(( struct Cyc_Absyn_StructMember_e_struct*) _temp24)->f1; goto
_LL183; _LL183: _temp182=(( struct Cyc_Absyn_StructMember_e_struct*) _temp24)->f2;
goto _LL77;} else{ goto _LL78;} _LL78: if(*(( int*) _temp24) ==  Cyc_Absyn_New_e){
_LL189: _temp188=(( struct Cyc_Absyn_New_e_struct*) _temp24)->f1; goto _LL187;
_LL187: _temp186=(( struct Cyc_Absyn_New_e_struct*) _temp24)->f2; goto _LL79;}
else{ goto _LL80;} _LL80: if(*(( int*) _temp24) ==  Cyc_Absyn_Malloc_e){ _LL193:
_temp192=(( struct Cyc_Absyn_Malloc_e_struct*) _temp24)->f1; goto _LL191; _LL191:
_temp190=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp24)->f2; goto _LL81;}
else{ goto _LL82;} _LL82: if(*(( int*) _temp24) ==  Cyc_Absyn_Tunion_e){ _LL195:
_temp194=(( struct Cyc_Absyn_Tunion_e_struct*) _temp24)->f3; goto _LL83;} else{
goto _LL84;} _LL84: if(*(( int*) _temp24) ==  Cyc_Absyn_Tuple_e){ _LL197:
_temp196=(( struct Cyc_Absyn_Tuple_e_struct*) _temp24)->f1; goto _LL85;} else{
goto _LL86;} _LL86: if(*(( int*) _temp24) ==  Cyc_Absyn_AnonStruct_e){ _LL199:
_temp198=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp24)->f2; goto _LL87;}
else{ goto _LL88;} _LL88: if(*(( int*) _temp24) ==  Cyc_Absyn_Struct_e){ _LL201:
_temp200=(( struct Cyc_Absyn_Struct_e_struct*) _temp24)->f3; goto _LL89;} else{
goto _LL90;} _LL90: if(*(( int*) _temp24) ==  Cyc_Absyn_Array_e){ _LL203:
_temp202=(( struct Cyc_Absyn_Array_e_struct*) _temp24)->f1; goto _LL91;} else{
goto _LL92;} _LL92: if(*(( int*) _temp24) ==  Cyc_Absyn_Increment_e){ _LL205:
_temp204=(( struct Cyc_Absyn_Increment_e_struct*) _temp24)->f1; goto _LL93;}
else{ goto _LL94;} _LL94: if(*(( int*) _temp24) ==  Cyc_Absyn_Throw_e){ _LL207:
_temp206=(( struct Cyc_Absyn_Throw_e_struct*) _temp24)->f1; goto _LL95;} else{
goto _LL96;} _LL96: if(*(( int*) _temp24) ==  Cyc_Absyn_Conditional_e){ _LL213:
_temp212=(( struct Cyc_Absyn_Conditional_e_struct*) _temp24)->f1; goto _LL211;
_LL211: _temp210=(( struct Cyc_Absyn_Conditional_e_struct*) _temp24)->f2; goto
_LL209; _LL209: _temp208=(( struct Cyc_Absyn_Conditional_e_struct*) _temp24)->f3;
goto _LL97;} else{ goto _LL98;} _LL98: if(*(( int*) _temp24) ==  Cyc_Absyn_StmtExp_e){
_LL215: _temp214=(( struct Cyc_Absyn_StmtExp_e_struct*) _temp24)->f1; goto _LL99;}
else{ goto _LL100;} _LL100: if(*(( int*) _temp24) ==  Cyc_Absyn_SeqExp_e){
_LL219: _temp218=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp24)->f1; goto _LL217;
_LL217: _temp216=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp24)->f2; goto _LL101;}
else{ goto _LL102;} _LL102: if(*(( int*) _temp24) ==  Cyc_Absyn_Comprehension_e){
_LL225: _temp224=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp24)->f1; goto
_LL223; _LL223: _temp222=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp24)->f2;
goto _LL221; _LL221: _temp220=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp24)->f3; goto _LL103;} else{ goto _LL104;} _LL104: if(*(( int*) _temp24) == 
Cyc_Absyn_Var_e){ _LL227: _temp226=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp24)->f2; if( _temp226 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL105;}
else{ goto _LL106;}} else{ goto _LL106;} _LL106: if(*(( int*) _temp24) ==  Cyc_Absyn_UnknownId_e){
goto _LL107;} else{ goto _LL108;} _LL108: if(*(( int*) _temp24) ==  Cyc_Absyn_UnknownCall_e){
goto _LL109;} else{ goto _LL110;} _LL110: if(*(( int*) _temp24) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL111;} else{ goto _LL112;} _LL112: if(*(( int*) _temp24) ==  Cyc_Absyn_CompoundLit_e){
goto _LL113;} else{ goto _LL114;} _LL114: if(*(( int*) _temp24) ==  Cyc_Absyn_Codegen_e){
goto _LL115;} else{ goto _LL116;} _LL116: if(*(( int*) _temp24) ==  Cyc_Absyn_Fill_e){
goto _LL117;} else{ goto _LL25;} _LL27: _temp120= _temp118; goto _LL29; _LL29:
_temp122= _temp120; goto _LL31; _LL31: return Cyc_NewControlFlow_abstract_exp(
env, _temp122); _LL33: goto _LL35; _LL35: return({ struct Cyc_NewControlFlow_AbsSyn
_temp228; _temp228.when_true_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp228.when_false_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp228.absop=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp228;}); _LL37: return({ struct Cyc_NewControlFlow_AbsSyn _temp229; _temp229.when_true_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp229.when_false_exp=( void*) Cyc_CfAbsexp_mkBottomAE();
_temp229.absop=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp229;}); _LL39: goto
_LL41; _LL41: goto _LL43; _LL43: goto _LL45; _LL45: goto _LL47; _LL47: goto
_LL49; _LL49: goto _LL51; _LL51: goto _LL53; _LL53: return({ struct Cyc_NewControlFlow_AbsSyn
_temp230; _temp230.when_true_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp230.when_false_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp230.absop=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp230;}); _LL55: _temp148= _temp144; goto _LL57; _LL57: _temp152= _temp148;
goto _LL59; _LL59: return({ struct Cyc_NewControlFlow_AbsSyn _temp231; _temp231.when_true_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp231.when_false_exp=( void*) Cyc_CfAbsexp_mkSkipAE();
_temp231.absop=( void*) Cyc_CfAbsexp_mkLocalOp( _temp152); _temp231;}); _LL61: {
struct Cyc_NewControlFlow_AbsSyn _temp232= Cyc_NewControlFlow_abstract_exp( env,
_temp158); struct Cyc_NewControlFlow_AbsSyn _temp233= Cyc_NewControlFlow_abstract_exp(
env, _temp154); void* _temp234= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, Cyc_NewControlFlow_meet_absexp(
_temp232), Cyc_NewControlFlow_meet_absexp( _temp233)), Cyc_CfAbsexp_mkAssignAE((
void*) _temp232.absop,( void*) _temp233.absop)); return({ struct Cyc_NewControlFlow_AbsSyn
_temp235; _temp235.when_true_exp=( void*) _temp234; _temp235.when_false_exp=(
void*) _temp234; _temp235.absop=( void*)(( void*) _temp232.absop); _temp235;});}
_LL63: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp236=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp236->hd=( void*) _temp162; _temp236->tl= _temp160; _temp236;})); _LL65:
_temp168=({ struct Cyc_List_List* _temp237=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp237->hd=( void*) _temp166; _temp237->tl=({
struct Cyc_List_List* _temp238=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp238->hd=( void*) _temp164; _temp238->tl= 0;
_temp238;}); _temp237;}); goto _LL67; _LL67: return Cyc_NewControlFlow_use_exp_unordered(
env, _temp168); _LL69: { void* _temp239=( void*) _temp170->r; _LL241: if(*(( int*)
_temp239) ==  Cyc_Absyn_Struct_e){ goto _LL242;} else{ goto _LL243;} _LL243: if(*((
int*) _temp239) ==  Cyc_Absyn_Tuple_e){ goto _LL244;} else{ goto _LL245;} _LL245:
goto _LL246; _LL242: goto _LL244; _LL244: Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp170->topt))->v);
return Cyc_NewControlFlow_abstract_malloc( env, e, 0,( struct Cyc_Absyn_Exp*)
_temp170); _LL246: { struct Cyc_NewControlFlow_AbsSyn _temp247= Cyc_NewControlFlow_abstract_exp(
env, _temp170); void* _temp248= Cyc_NewControlFlow_meet_absexp( _temp247);
return({ struct Cyc_NewControlFlow_AbsSyn _temp249; _temp249.when_true_exp=(
void*) _temp248; _temp249.when_false_exp=( void*) _temp248; _temp249.absop=(
void*) Cyc_CfAbsexp_mkAddressOp(( void*) _temp247.absop); _temp249;});} _LL240:;}
_LL71: { void* _temp250= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp174->topt))->v); _LL252: if(( unsigned int) _temp250 >  4u?*((
int*) _temp250) ==  Cyc_Absyn_TupleType: 0){ goto _LL253;} else{ goto _LL254;}
_LL254: goto _LL255; _LL253: { struct Cyc_NewControlFlow_AbsSyn _temp256= Cyc_NewControlFlow_abstract_exp(
env, _temp174); void* _temp257= Cyc_NewControlFlow_meet_absexp( _temp256);
struct _tagged_arr* _temp258= Cyc_Absyn_fieldname(( int) Cyc_Evexp_eval_const_uint_exp(
_temp172)); return({ struct Cyc_NewControlFlow_AbsSyn _temp259; _temp259.when_true_exp=(
void*) _temp257; _temp259.when_false_exp=( void*) _temp257; _temp259.absop=(
void*) Cyc_CfAbsexp_mkMemberOp(( void*) _temp256.absop, _temp258); _temp259;});}
_LL255: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp260=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp260->hd=( void*) _temp174; _temp260->tl=({ struct Cyc_List_List* _temp261=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp261->hd=(
void*) _temp172; _temp261->tl= 0; _temp261;}); _temp260;})); _LL251:;} _LL73: {
struct Cyc_NewControlFlow_AbsSyn _temp262= Cyc_NewControlFlow_abstract_exp( env,
_temp176); void* _temp263= Cyc_NewControlFlow_meet_absexp( _temp262); void*
absop;{ void* _temp264= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp176->topt))->v); struct Cyc_Absyn_PtrInfo _temp270; struct Cyc_Absyn_Conref*
_temp272; void* _temp274; _LL266: if(( unsigned int) _temp264 >  4u?*(( int*)
_temp264) ==  Cyc_Absyn_PointerType: 0){ _LL271: _temp270=(( struct Cyc_Absyn_PointerType_struct*)
_temp264)->f1; _LL275: _temp274=( void*) _temp270.elt_typ; goto _LL273; _LL273:
_temp272= _temp270.bounds; goto _LL267;} else{ goto _LL268;} _LL268: goto _LL269;
_LL267: if( Cyc_Tcutil_is_bound_one( _temp272)){ absop= Cyc_CfAbsexp_mkDerefOp((
void*) _temp262.absop);} else{ _temp263= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp263, Cyc_CfAbsexp_mkUseAE(( void*) _temp262.absop)); absop= Cyc_CfAbsexp_mkUnknownOp();}
goto _LL265; _LL269:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp276=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp276[ 0]=({ struct Cyc_Core_Impossible_struct _temp277; _temp277.tag= Cyc_Core_Impossible;
_temp277.f1= _tag_arr("NewControlFlow: bad type in Deref", sizeof( unsigned char),
34u); _temp277;}); _temp276;})); _LL265:;} return({ struct Cyc_NewControlFlow_AbsSyn
_temp278; _temp278.when_true_exp=( void*) _temp263; _temp278.when_false_exp=(
void*) _temp263; _temp278.absop=( void*) absop; _temp278;});} _LL75: { struct
Cyc_NewControlFlow_AbsSyn _temp279= Cyc_NewControlFlow_abstract_exp( env,
_temp180); void* _temp280= Cyc_NewControlFlow_meet_absexp( _temp279); void*
absop;{ void* _temp281= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp180->topt))->v); struct Cyc_Absyn_PtrInfo _temp287; struct Cyc_Absyn_Conref*
_temp289; void* _temp291; _LL283: if(( unsigned int) _temp281 >  4u?*(( int*)
_temp281) ==  Cyc_Absyn_PointerType: 0){ _LL288: _temp287=(( struct Cyc_Absyn_PointerType_struct*)
_temp281)->f1; _LL292: _temp291=( void*) _temp287.elt_typ; goto _LL290; _LL290:
_temp289= _temp287.bounds; goto _LL284;} else{ goto _LL285;} _LL285: goto _LL286;
_LL284: if( Cyc_Tcutil_is_bound_one( _temp289)){ void* _temp293= Cyc_Tcutil_compress(
_temp291); _LL295: if(( unsigned int) _temp293 >  4u?*(( int*) _temp293) ==  Cyc_Absyn_UnionType:
0){ goto _LL296;} else{ goto _LL297;} _LL297: if(( unsigned int) _temp293 >  4u?*((
int*) _temp293) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL298;} else{ goto _LL299;}
_LL299: goto _LL300; _LL296: goto _LL298; _LL298: absop= Cyc_CfAbsexp_mkDerefOp((
void*) _temp279.absop); goto _LL294; _LL300: absop= Cyc_CfAbsexp_mkMemberOp( Cyc_CfAbsexp_mkDerefOp((
void*) _temp279.absop), _temp178); _LL294:;} else{ _temp280= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp280, Cyc_CfAbsexp_mkUseAE(( void*) _temp279.absop));
absop= Cyc_CfAbsexp_mkUnknownOp();} goto _LL282; _LL286:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp301=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp301[ 0]=({ struct
Cyc_Core_Impossible_struct _temp302; _temp302.tag= Cyc_Core_Impossible; _temp302.f1=
_tag_arr("NewControlFlow: bad type in StructArrow", sizeof( unsigned char), 40u);
_temp302;}); _temp301;})); _LL282:;} return({ struct Cyc_NewControlFlow_AbsSyn
_temp303; _temp303.when_true_exp=( void*) _temp280; _temp303.when_false_exp=(
void*) _temp280; _temp303.absop=( void*) absop; _temp303;});} _LL77: { struct
Cyc_NewControlFlow_AbsSyn _temp304= Cyc_NewControlFlow_abstract_exp( env,
_temp184); void* _temp305= Cyc_NewControlFlow_meet_absexp( _temp304); void*
absop;{ void* _temp306= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp184->topt))->v); _LL308: if(( unsigned int) _temp306 >  4u?*((
int*) _temp306) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL309;} else{ goto _LL310;}
_LL310: if(( unsigned int) _temp306 >  4u?*(( int*) _temp306) ==  Cyc_Absyn_UnionType:
0){ goto _LL311;} else{ goto _LL312;} _LL312: goto _LL313; _LL309: goto _LL311;
_LL311: absop=( void*) _temp304.absop; goto _LL307; _LL313: absop= Cyc_CfAbsexp_mkMemberOp((
void*) _temp304.absop, _temp182); _LL307:;} return({ struct Cyc_NewControlFlow_AbsSyn
_temp314; _temp314.when_true_exp=( void*) _temp305; _temp314.when_false_exp=(
void*) _temp305; _temp314.absop=( void*) absop; _temp314;});} _LL79: Cyc_NewControlFlow_add_malloc_root(
env, e,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp186->topt))->v);
return Cyc_NewControlFlow_abstract_malloc( env, e, _temp188,( struct Cyc_Absyn_Exp*)
_temp186); _LL81: Cyc_NewControlFlow_add_malloc_root( env, e, _temp190); return
Cyc_NewControlFlow_abstract_malloc( env, e, _temp192, 0); _LL83: if( _temp194 == 
0){ return({ struct Cyc_NewControlFlow_AbsSyn _temp315; _temp315.when_true_exp=(
void*) Cyc_CfAbsexp_mkSkipAE(); _temp315.when_false_exp=( void*) Cyc_CfAbsexp_mkSkipAE();
_temp315.absop=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp315;});} _temp196=
_temp194; goto _LL85; _LL85: { struct Cyc_List_List* arg_aexps= 0; struct Cyc_Dict_Dict*
absop_dict=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct
_tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{ int j= 0; for( 0;
_temp196 !=  0;( _temp196=(( struct Cyc_List_List*) _check_null( _temp196))->tl,
++ j)){ struct Cyc_NewControlFlow_AbsSyn _temp316= Cyc_NewControlFlow_abstract_exp(
env,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp196))->hd);
arg_aexps=({ struct Cyc_List_List* _temp317=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp317->hd=( void*) Cyc_NewControlFlow_meet_absexp(
_temp316); _temp317->tl= arg_aexps; _temp317;}); absop_dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)(
absop_dict, Cyc_Absyn_fieldname( j),( void*) _temp316.absop);}}{ void* _temp318=
Cyc_CfAbsexp_mkGroupAE_l(( void*) Cyc_CfAbsexp_UnorderedG, arg_aexps); return({
struct Cyc_NewControlFlow_AbsSyn _temp319; _temp319.when_true_exp=( void*)
_temp318; _temp319.when_false_exp=( void*) _temp318; _temp319.absop=( void*) Cyc_CfAbsexp_mkDictOp(
absop_dict); _temp319;});}} _LL87: _temp200= _temp198; goto _LL89; _LL89: {
struct Cyc_List_List* arg_aexps= 0; struct Cyc_Dict_Dict* absop_dict=(( struct
Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp); for( 0; _temp200 !=  0; _temp200=(( struct Cyc_List_List*)
_check_null( _temp200))->tl){ struct Cyc_NewControlFlow_AbsSyn _temp320= Cyc_NewControlFlow_abstract_exp(
env,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp200))->hd)).f2);
arg_aexps=({ struct Cyc_List_List* _temp321=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp321->hd=( void*) Cyc_NewControlFlow_meet_absexp(
_temp320); _temp321->tl= arg_aexps; _temp321;});{ struct Cyc_List_List* ds=(*((
struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp200))->hd)).f1; for(
0; ds !=  0; ds=(( struct Cyc_List_List*) _check_null( ds))->tl){ void* _temp322=(
void*)(( struct Cyc_List_List*) _check_null( ds))->hd; struct _tagged_arr*
_temp328; _LL324: if(*(( int*) _temp322) ==  Cyc_Absyn_ArrayElement){ goto
_LL325;} else{ goto _LL326;} _LL326: if(*(( int*) _temp322) ==  Cyc_Absyn_FieldName){
_LL329: _temp328=(( struct Cyc_Absyn_FieldName_struct*) _temp322)->f1; goto
_LL327;} else{ goto _LL323;} _LL325:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp330=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp330[ 0]=({ struct Cyc_Core_Impossible_struct _temp331; _temp331.tag= Cyc_Core_Impossible;
_temp331.f1= _tag_arr("bad struct designator", sizeof( unsigned char), 22u);
_temp331;}); _temp330;})); _LL327: absop_dict=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)(
absop_dict, _temp328,( void*) _temp320.absop); _LL323:;}}}{ void* _temp332= Cyc_CfAbsexp_mkGroupAE_l((
void*) Cyc_CfAbsexp_UnorderedG, arg_aexps); return({ struct Cyc_NewControlFlow_AbsSyn
_temp333; _temp333.when_true_exp=( void*) _temp332; _temp333.when_false_exp=(
void*) _temp332; _temp333.absop=( void*) Cyc_CfAbsexp_mkDictOp( absop_dict);
_temp333;});}} _LL91: return Cyc_NewControlFlow_use_exp_unordered( env,(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple4*), struct Cyc_List_List*
x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct _tuple4*)) Cyc_Core_snd,
_temp202)); _LL93: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct
Cyc_List_List* _temp334=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp334->hd=( void*) _temp204; _temp334->tl= 0; _temp334;})); _LL95: { void*
_temp335= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_NewControlFlow_use_exp(
env, _temp206), Cyc_CfAbsexp_mkBottomAE()); return({ struct Cyc_NewControlFlow_AbsSyn
_temp336; _temp336.when_true_exp=( void*) _temp335; _temp336.when_false_exp=(
void*) _temp335; _temp336.absop=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp336;});}
_LL97: { struct Cyc_NewControlFlow_AbsSyn _temp337= Cyc_NewControlFlow_abstract_exp(
env, _temp212); struct Cyc_NewControlFlow_AbsSyn _temp338= Cyc_NewControlFlow_abstract_exp(
env, _temp210); struct Cyc_NewControlFlow_AbsSyn _temp339= Cyc_NewControlFlow_abstract_exp(
env, _temp208); void* _temp340= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp337.when_true_exp, Cyc_CfAbsexp_mkUseAE(( void*) _temp337.absop));
void* _temp341= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp337.when_false_exp, Cyc_CfAbsexp_mkUseAE(( void*) _temp337.absop)); void*
_temp342= Cyc_CfAbsexp_mkUseAE(( void*) _temp338.absop); void* _temp343= Cyc_CfAbsexp_mkUseAE((
void*) _temp339.absop); void* _temp344= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp340, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp338.when_true_exp, _temp342)), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp341, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp339.when_true_exp, _temp343))); void* _temp345= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp340, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp338.when_false_exp, _temp342)), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp341, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp339.when_false_exp, _temp343))); return({ struct Cyc_NewControlFlow_AbsSyn
_temp346; _temp346.when_true_exp=( void*) _temp344; _temp346.when_false_exp=(
void*) _temp345; _temp346.absop=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp346;});}
_LL99: Cyc_NewControlFlow_cf_prepass( env, _temp214);{ struct Cyc_Absyn_Stmt*
_temp347= _temp214; while( 1) { void* _temp348=( void*) _temp347->r; struct Cyc_Absyn_Stmt*
_temp358; struct Cyc_Absyn_Stmt* _temp360; struct Cyc_Absyn_Stmt* _temp362;
struct Cyc_Absyn_Decl* _temp364; struct Cyc_Absyn_Exp* _temp366; _LL350: if((
unsigned int) _temp348 >  1u?*(( int*) _temp348) ==  Cyc_Absyn_Seq_s: 0){ _LL361:
_temp360=(( struct Cyc_Absyn_Seq_s_struct*) _temp348)->f1; goto _LL359; _LL359:
_temp358=(( struct Cyc_Absyn_Seq_s_struct*) _temp348)->f2; goto _LL351;} else{
goto _LL352;} _LL352: if(( unsigned int) _temp348 >  1u?*(( int*) _temp348) == 
Cyc_Absyn_Decl_s: 0){ _LL365: _temp364=(( struct Cyc_Absyn_Decl_s_struct*)
_temp348)->f1; goto _LL363; _LL363: _temp362=(( struct Cyc_Absyn_Decl_s_struct*)
_temp348)->f2; goto _LL353;} else{ goto _LL354;} _LL354: if(( unsigned int)
_temp348 >  1u?*(( int*) _temp348) ==  Cyc_Absyn_Exp_s: 0){ _LL367: _temp366=((
struct Cyc_Absyn_Exp_s_struct*) _temp348)->f1; goto _LL355;} else{ goto _LL356;}
_LL356: goto _LL357; _LL351: _temp347= _temp358; continue; _LL353: _temp347=
_temp362; continue; _LL355: { void* _temp368=( void*)( Cyc_NewControlFlow_abstract_exp(
env, _temp366)).absop; void* _temp369= Cyc_CfAbsexp_mkStmtAE( _temp214); return({
struct Cyc_NewControlFlow_AbsSyn _temp370; _temp370.when_true_exp=( void*)
_temp369; _temp370.when_false_exp=( void*) _temp369; _temp370.absop=( void*)
_temp368; _temp370;});} _LL357:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp371=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp371[ 0]=({ struct Cyc_Core_Impossible_struct _temp372; _temp372.tag= Cyc_Core_Impossible;
_temp372.f1= _tag_arr("abstract_exp: ill-formed StmtExp", sizeof( unsigned char),
33u); _temp372;}); _temp371;})); _LL349:;}} _LL101: { struct Cyc_NewControlFlow_AbsSyn
_temp373= Cyc_NewControlFlow_abstract_exp( env, _temp218); struct Cyc_NewControlFlow_AbsSyn
_temp374= Cyc_NewControlFlow_abstract_exp( env, _temp216); void* _temp375= Cyc_NewControlFlow_meet_absexp(
_temp373); return({ struct Cyc_NewControlFlow_AbsSyn _temp376; _temp376.when_true_exp=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp375,( void*)
_temp374.when_true_exp); _temp376.when_false_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp375,( void*) _temp374.when_false_exp);
_temp376.absop=( void*)(( void*) _temp374.absop); _temp376;});} _LL103: Cyc_NewControlFlow_add_var_root(
env, _temp224);{ struct Cyc_NewControlFlow_AbsSyn _temp377= Cyc_NewControlFlow_abstract_exp(
env, _temp222); struct Cyc_NewControlFlow_AbsSyn _temp378= Cyc_NewControlFlow_abstract_exp(
env, _temp220); void* _temp379= Cyc_CfAbsexp_mkComprehensionAE( _temp224,( void*)
_temp377.when_true_exp,( void*) _temp377.when_false_exp,( void*) _temp377.absop,
Cyc_NewControlFlow_meet_absexp( _temp378),( void*) _temp378.absop); return({
struct Cyc_NewControlFlow_AbsSyn _temp380; _temp380.when_true_exp=( void*)
_temp379; _temp380.when_false_exp=( void*) _temp379; _temp380.absop=( void*) Cyc_CfAbsexp_mkUnknownOp();
_temp380;});} _LL105: goto _LL107; _LL107: goto _LL109; _LL109: goto _LL111;
_LL111: goto _LL113; _LL113: goto _LL115; _LL115: goto _LL117; _LL117:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp381=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp381[ 0]=({ struct
Cyc_Core_Impossible_struct _temp382; _temp382.tag= Cyc_Core_Impossible; _temp382.f1=
_tag_arr("abstract_exp, unexpected exp form", sizeof( unsigned char), 34u);
_temp382;}); _temp381;})); _LL25:;} static struct _tuple0 Cyc_NewControlFlow_abstract_guard(
struct Cyc_NewControlFlow_AbsEnv* env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn
_temp383= Cyc_NewControlFlow_abstract_exp( env, e); void* _temp384= Cyc_CfAbsexp_mkUseAE((
void*) _temp383.absop); if(( void*) _temp383.when_true_exp == ( void*) _temp383.when_false_exp){
void* _temp385= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp383.when_true_exp, _temp384); return({ struct _tuple0 _temp386; _temp386.f1=
_temp385; _temp386.f2= _temp385; _temp386;});} return({ struct _tuple0 _temp387;
_temp387.f1= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)
_temp383.when_true_exp, _temp384); _temp387.f2= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_OrderedG,( void*) _temp383.when_false_exp, _temp384); _temp387;});}
struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs( struct Cyc_NewControlFlow_AbsEnv*
env, struct Cyc_List_List* scs){ struct Cyc_List_List* _temp388= 0; for( 0; scs
!=  0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause
_temp391; struct Cyc_Absyn_Stmt* _temp392; struct Cyc_Absyn_Exp* _temp394;
struct Cyc_Core_Opt* _temp396; struct Cyc_Absyn_Switch_clause* _temp389=( struct
Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( scs))->hd;
_temp391=* _temp389; _LL397: _temp396= _temp391.pat_vars; goto _LL395; _LL395:
_temp394= _temp391.where_clause; goto _LL393; _LL393: _temp392= _temp391.body;
goto _LL390; _LL390: if( _temp396 ==  0){( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp398=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp398[ 0]=({ struct Cyc_Core_Impossible_struct _temp399; _temp399.tag= Cyc_Core_Impossible;
_temp399.f1= _tag_arr("switch clause vds not set", sizeof( unsigned char), 26u);
_temp399;}); _temp398;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp396))->v; for( 0; vds !=  0; vds=((
struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd);}}
if( _temp394 !=  0){ void* _temp402; void* _temp404; struct _tuple0 _temp400=
Cyc_NewControlFlow_abstract_guard( env,( struct Cyc_Absyn_Exp*) _check_null(
_temp394)); _LL405: _temp404= _temp400.f1; goto _LL403; _LL403: _temp402=
_temp400.f2; goto _LL401; _LL401: _temp388=({ struct Cyc_List_List* _temp406=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp406->hd=(
void*) _temp404; _temp406->tl=({ struct Cyc_List_List* _temp407=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp407->hd=( void*) _temp402;
_temp407->tl= _temp388; _temp407;}); _temp406;});} Cyc_NewControlFlow_cf_prepass(
env, _temp392);} return Cyc_List_imp_rev( _temp388);} void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv* env, struct Cyc_Absyn_Stmt* s){ void* _temp408=(
void*) s->r; struct Cyc_Absyn_Exp* _temp460; struct Cyc_Absyn_Stmt* _temp462;
struct Cyc_Absyn_Stmt* _temp464; struct Cyc_Absyn_Exp* _temp466; struct Cyc_Absyn_Exp*
_temp468; struct Cyc_Absyn_Stmt* _temp470; struct Cyc_Absyn_Stmt* _temp472;
struct Cyc_Absyn_Exp* _temp474; struct _tuple3 _temp476; struct Cyc_Absyn_Stmt*
_temp478; struct Cyc_Absyn_Exp* _temp480; struct Cyc_Absyn_Stmt* _temp482;
struct Cyc_Absyn_Stmt* _temp484; struct _tuple3 _temp486; struct Cyc_Absyn_Stmt*
_temp488; struct Cyc_Absyn_Exp* _temp490; struct Cyc_Absyn_Stmt* _temp492;
struct Cyc_Absyn_Stmt* _temp494; struct Cyc_Absyn_Stmt* _temp496; struct _tuple3
_temp498; struct Cyc_Absyn_Stmt* _temp500; struct Cyc_Absyn_Exp* _temp502;
struct _tuple3 _temp504; struct Cyc_Absyn_Stmt* _temp506; struct Cyc_Absyn_Exp*
_temp508; struct Cyc_Absyn_Exp* _temp510; struct Cyc_List_List* _temp512; struct
Cyc_Absyn_Exp* _temp514; struct Cyc_List_List* _temp516; struct Cyc_Absyn_Stmt*
_temp518; struct Cyc_Absyn_Decl* _temp520; struct Cyc_Absyn_Decl _temp522; void*
_temp523; struct Cyc_Absyn_Vardecl* _temp525; struct Cyc_Absyn_Stmt* _temp527;
struct Cyc_Absyn_Decl* _temp529; struct Cyc_Absyn_Decl _temp531; void* _temp532;
struct Cyc_Absyn_Exp* _temp534; struct Cyc_Core_Opt* _temp536; struct Cyc_Core_Opt
_temp538; struct Cyc_List_List* _temp539; struct Cyc_Absyn_Stmt* _temp541;
struct Cyc_Absyn_Decl* _temp543; struct Cyc_Absyn_Decl _temp545; void* _temp546;
struct Cyc_List_List* _temp548; struct Cyc_Absyn_Stmt* _temp550; struct Cyc_List_List*
_temp552; struct Cyc_Absyn_Stmt* _temp554; struct Cyc_Absyn_Stmt* _temp556;
struct Cyc_Absyn_Vardecl* _temp558; struct Cyc_Absyn_Tvar* _temp560; _LL410: if(
_temp408 == ( void*) Cyc_Absyn_Skip_s){ goto _LL411;} else{ goto _LL412;} _LL412:
if(( unsigned int) _temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_Exp_s: 0){
_LL461: _temp460=(( struct Cyc_Absyn_Exp_s_struct*) _temp408)->f1; goto _LL413;}
else{ goto _LL414;} _LL414: if(( unsigned int) _temp408 >  1u?*(( int*) _temp408)
==  Cyc_Absyn_Seq_s: 0){ _LL465: _temp464=(( struct Cyc_Absyn_Seq_s_struct*)
_temp408)->f1; goto _LL463; _LL463: _temp462=(( struct Cyc_Absyn_Seq_s_struct*)
_temp408)->f2; goto _LL415;} else{ goto _LL416;} _LL416: if(( unsigned int)
_temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_Return_s: 0){ _LL467: _temp466=((
struct Cyc_Absyn_Return_s_struct*) _temp408)->f1; if( _temp466 ==  0){ goto
_LL417;} else{ goto _LL418;}} else{ goto _LL418;} _LL418: if(( unsigned int)
_temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_Return_s: 0){ _LL469: _temp468=((
struct Cyc_Absyn_Return_s_struct*) _temp408)->f1; goto _LL419;} else{ goto
_LL420;} _LL420: if(( unsigned int) _temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_IfThenElse_s:
0){ _LL475: _temp474=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp408)->f1;
goto _LL473; _LL473: _temp472=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp408)->f2;
goto _LL471; _LL471: _temp470=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp408)->f3;
goto _LL421;} else{ goto _LL422;} _LL422: if(( unsigned int) _temp408 >  1u?*((
int*) _temp408) ==  Cyc_Absyn_Do_s: 0){ _LL483: _temp482=(( struct Cyc_Absyn_Do_s_struct*)
_temp408)->f1; goto _LL477; _LL477: _temp476=(( struct Cyc_Absyn_Do_s_struct*)
_temp408)->f2; _LL481: _temp480= _temp476.f1; goto _LL479; _LL479: _temp478=
_temp476.f2; goto _LL423;} else{ goto _LL424;} _LL424: if(( unsigned int)
_temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_While_s: 0){ _LL487: _temp486=((
struct Cyc_Absyn_While_s_struct*) _temp408)->f1; _LL491: _temp490= _temp486.f1;
goto _LL489; _LL489: _temp488= _temp486.f2; goto _LL485; _LL485: _temp484=((
struct Cyc_Absyn_While_s_struct*) _temp408)->f2; goto _LL425;} else{ goto _LL426;}
_LL426: if(( unsigned int) _temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_Break_s:
0){ goto _LL427;} else{ goto _LL428;} _LL428: if(( unsigned int) _temp408 >  1u?*((
int*) _temp408) ==  Cyc_Absyn_Continue_s: 0){ goto _LL429;} else{ goto _LL430;}
_LL430: if(( unsigned int) _temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_Goto_s:
0){ _LL493: _temp492=(( struct Cyc_Absyn_Goto_s_struct*) _temp408)->f2; if(
_temp492 ==  0){ goto _LL431;} else{ goto _LL432;}} else{ goto _LL432;} _LL432:
if(( unsigned int) _temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_Goto_s: 0){
_LL495: _temp494=(( struct Cyc_Absyn_Goto_s_struct*) _temp408)->f2; goto _LL433;}
else{ goto _LL434;} _LL434: if(( unsigned int) _temp408 >  1u?*(( int*) _temp408)
==  Cyc_Absyn_For_s: 0){ _LL511: _temp510=(( struct Cyc_Absyn_For_s_struct*)
_temp408)->f1; goto _LL505; _LL505: _temp504=(( struct Cyc_Absyn_For_s_struct*)
_temp408)->f2; _LL509: _temp508= _temp504.f1; goto _LL507; _LL507: _temp506=
_temp504.f2; goto _LL499; _LL499: _temp498=(( struct Cyc_Absyn_For_s_struct*)
_temp408)->f3; _LL503: _temp502= _temp498.f1; goto _LL501; _LL501: _temp500=
_temp498.f2; goto _LL497; _LL497: _temp496=(( struct Cyc_Absyn_For_s_struct*)
_temp408)->f4; goto _LL435;} else{ goto _LL436;} _LL436: if(( unsigned int)
_temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_Switch_s: 0){ _LL515: _temp514=((
struct Cyc_Absyn_Switch_s_struct*) _temp408)->f1; goto _LL513; _LL513: _temp512=((
struct Cyc_Absyn_Switch_s_struct*) _temp408)->f2; goto _LL437;} else{ goto
_LL438;} _LL438: if(( unsigned int) _temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_Fallthru_s:
0){ _LL517: _temp516=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp408)->f1; goto
_LL439;} else{ goto _LL440;} _LL440: if(( unsigned int) _temp408 >  1u?*(( int*)
_temp408) ==  Cyc_Absyn_Decl_s: 0){ _LL521: _temp520=(( struct Cyc_Absyn_Decl_s_struct*)
_temp408)->f1; _temp522=* _temp520; _LL524: _temp523=( void*) _temp522.r; if(*((
int*) _temp523) ==  Cyc_Absyn_Var_d){ _LL526: _temp525=(( struct Cyc_Absyn_Var_d_struct*)
_temp523)->f1; goto _LL519;} else{ goto _LL442;} _LL519: _temp518=(( struct Cyc_Absyn_Decl_s_struct*)
_temp408)->f2; goto _LL441;} else{ goto _LL442;} _LL442: if(( unsigned int)
_temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_Decl_s: 0){ _LL530: _temp529=((
struct Cyc_Absyn_Decl_s_struct*) _temp408)->f1; _temp531=* _temp529; _LL533:
_temp532=( void*) _temp531.r; if(*(( int*) _temp532) ==  Cyc_Absyn_Let_d){
_LL537: _temp536=(( struct Cyc_Absyn_Let_d_struct*) _temp532)->f2; if( _temp536
==  0){ goto _LL444;} else{ _temp538=* _temp536; _LL540: _temp539=( struct Cyc_List_List*)
_temp538.v; goto _LL535;} _LL535: _temp534=(( struct Cyc_Absyn_Let_d_struct*)
_temp532)->f4; goto _LL528;} else{ goto _LL444;} _LL528: _temp527=(( struct Cyc_Absyn_Decl_s_struct*)
_temp408)->f2; goto _LL443;} else{ goto _LL444;} _LL444: if(( unsigned int)
_temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_Decl_s: 0){ _LL544: _temp543=((
struct Cyc_Absyn_Decl_s_struct*) _temp408)->f1; _temp545=* _temp543; _LL547:
_temp546=( void*) _temp545.r; if(*(( int*) _temp546) ==  Cyc_Absyn_Letv_d){
_LL549: _temp548=(( struct Cyc_Absyn_Letv_d_struct*) _temp546)->f1; goto _LL542;}
else{ goto _LL446;} _LL542: _temp541=(( struct Cyc_Absyn_Decl_s_struct*)
_temp408)->f2; goto _LL445;} else{ goto _LL446;} _LL446: if(( unsigned int)
_temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_Label_s: 0){ _LL551: _temp550=((
struct Cyc_Absyn_Label_s_struct*) _temp408)->f2; goto _LL447;} else{ goto _LL448;}
_LL448: if(( unsigned int) _temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_TryCatch_s:
0){ _LL555: _temp554=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp408)->f1; goto
_LL553; _LL553: _temp552=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp408)->f2;
goto _LL449;} else{ goto _LL450;} _LL450: if(( unsigned int) _temp408 >  1u?*((
int*) _temp408) ==  Cyc_Absyn_Region_s: 0){ _LL561: _temp560=(( struct Cyc_Absyn_Region_s_struct*)
_temp408)->f1; goto _LL559; _LL559: _temp558=(( struct Cyc_Absyn_Region_s_struct*)
_temp408)->f2; goto _LL557; _LL557: _temp556=(( struct Cyc_Absyn_Region_s_struct*)
_temp408)->f3; goto _LL451;} else{ goto _LL452;} _LL452: if(( unsigned int)
_temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_Decl_s: 0){ goto _LL453;} else{
goto _LL454;} _LL454: if(( unsigned int) _temp408 >  1u?*(( int*) _temp408) == 
Cyc_Absyn_SwitchC_s: 0){ goto _LL455;} else{ goto _LL456;} _LL456: if((
unsigned int) _temp408 >  1u?*(( int*) _temp408) ==  Cyc_Absyn_Cut_s: 0){ goto
_LL457;} else{ goto _LL458;} _LL458: if(( unsigned int) _temp408 >  1u?*(( int*)
_temp408) ==  Cyc_Absyn_Splice_s: 0){ goto _LL459;} else{ goto _LL409;} _LL411:
return; _LL413: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp562=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp562->hd=( void*) Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
env, _temp460)); _temp562->tl= 0; _temp562;})); return; _LL415: Cyc_NewControlFlow_cf_prepass(
env, _temp464); Cyc_NewControlFlow_cf_prepass( env, _temp462); return; _LL417:
return; _LL419: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp563=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp563->hd=( void*) Cyc_NewControlFlow_use_exp( env,( struct Cyc_Absyn_Exp*)
_check_null( _temp468)); _temp563->tl= 0; _temp563;})); return; _LL421: { void*
_temp566; void* _temp568; struct _tuple0 _temp564= Cyc_NewControlFlow_abstract_guard(
env, _temp474); _LL569: _temp568= _temp564.f1; goto _LL567; _LL567: _temp566=
_temp564.f2; goto _LL565; _LL565: Cyc_NewControlFlow_cf_set_absexps( s,({ struct
Cyc_List_List* _temp570=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp570->hd=( void*) _temp568; _temp570->tl=({ struct Cyc_List_List* _temp571=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp571->hd=(
void*) _temp566; _temp571->tl= 0; _temp571;}); _temp570;})); Cyc_NewControlFlow_cf_prepass(
env, _temp472); Cyc_NewControlFlow_cf_prepass( env, _temp470); return;} _LL423:
_temp490= _temp480; _temp488= _temp478; _temp484= _temp482; goto _LL425; _LL425: {
void* _temp574; void* _temp576; struct _tuple0 _temp572= Cyc_NewControlFlow_abstract_guard(
env, _temp490); _LL577: _temp576= _temp572.f1; goto _LL575; _LL575: _temp574=
_temp572.f2; goto _LL573; _LL573: Cyc_NewControlFlow_cf_set_absexps( s,({ struct
Cyc_List_List* _temp578=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp578->hd=( void*) _temp576; _temp578->tl=({ struct Cyc_List_List* _temp579=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp579->hd=(
void*) _temp574; _temp579->tl= 0; _temp579;}); _temp578;})); Cyc_NewControlFlow_cf_prepass(
env, _temp484); return;} _LL427: goto _LL429; _LL429: return; _LL431:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp580=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp580[ 0]=({ struct
Cyc_Core_Impossible_struct _temp581; _temp581.tag= Cyc_Core_Impossible; _temp581.f1=
_tag_arr("cf_prepass: goto w/o dest", sizeof( unsigned char), 26u); _temp581;});
_temp580;})); _LL433: { struct Cyc_Absyn_Stmt* _temp582=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* _temp583=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*) _check_null( _temp494)))->encloser; while( _temp583 != 
_temp582) { struct Cyc_Absyn_Stmt* _temp584=( Cyc_NewControlFlow_get_stmt_annot(
_temp582))->encloser; if( _temp584 ==  _temp582){({ void* _temp585[ 0u]={}; Cyc_Tcutil_terr(
s->loc, _tag_arr("goto enters local scope or exception handler", sizeof(
unsigned char), 45u), _tag_arr( _temp585, sizeof( void*), 0u));}); break;}
_temp582= _temp584;} return;} _LL435: { void* _temp586= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env, _temp510)); void* _temp589; void* _temp591;
struct _tuple0 _temp587= Cyc_NewControlFlow_abstract_guard( env, _temp508);
_LL592: _temp591= _temp587.f1; goto _LL590; _LL590: _temp589= _temp587.f2; goto
_LL588; _LL588: { void* _temp593= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
env, _temp502)); Cyc_NewControlFlow_cf_set_absexps( s,({ void* _temp594[ 4u];
_temp594[ 3u]= _temp593; _temp594[ 2u]= _temp589; _temp594[ 1u]= _temp591;
_temp594[ 0u]= _temp586; Cyc_List_list( _tag_arr( _temp594, sizeof( void*), 4u));}));
Cyc_NewControlFlow_cf_prepass( env, _temp496); return;}} _LL437: Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp595=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp595->hd=( void*) Cyc_NewControlFlow_use_exp( env,
_temp514); _temp595->tl= Cyc_NewControlFlow_cf_prepass_scs( env, _temp512);
_temp595;})); return; _LL439: Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp596=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp596->hd=( void*) Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp516)); _temp596->tl= 0; _temp596;})); return; _LL441: Cyc_NewControlFlow_add_var_root(
env, _temp525); if( _temp525->initializer !=  0){ struct Cyc_NewControlFlow_AbsSyn
_temp597= Cyc_NewControlFlow_abstract_exp( env,( struct Cyc_Absyn_Exp*)
_check_null( _temp525->initializer)); void* _temp598= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_NewControlFlow_meet_absexp( _temp597), Cyc_CfAbsexp_mkAssignAE(
Cyc_CfAbsexp_mkLocalOp( _temp525),( void*) _temp597.absop)); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp599=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp599->hd=( void*) _temp598; _temp599->tl= 0;
_temp599;}));} Cyc_NewControlFlow_cf_prepass( env, _temp518); return; _LL443:
for( 0; _temp539 !=  0; _temp539=(( struct Cyc_List_List*) _check_null( _temp539))->tl){
Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp539))->hd);} Cyc_NewControlFlow_cf_set_absexps( s,({ struct
Cyc_List_List* _temp600=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp600->hd=( void*) Cyc_NewControlFlow_use_exp( env, _temp534); _temp600->tl=
0; _temp600;})); Cyc_NewControlFlow_cf_prepass( env, _temp527); return; _LL445:
for( 0; _temp548 !=  0; _temp548=(( struct Cyc_List_List*) _check_null( _temp548))->tl){
Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp548))->hd);} Cyc_NewControlFlow_cf_prepass( env, _temp541);
return; _LL447: Cyc_NewControlFlow_cf_prepass( env, _temp550); return; _LL449:
Cyc_NewControlFlow_cf_prepass( env, _temp554); Cyc_NewControlFlow_cf_set_absexps(
s, Cyc_NewControlFlow_cf_prepass_scs( env, _temp552)); return; _LL451: Cyc_NewControlFlow_add_var_root(
env, _temp558); Cyc_NewControlFlow_cf_prepass( env, _temp556); return; _LL453:
goto _LL455; _LL455: goto _LL457; _LL457: goto _LL459; _LL459:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp601=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp601[ 0]=({ struct
Cyc_Core_Impossible_struct _temp602; _temp602.tag= Cyc_Core_Impossible; _temp602.f1=
_tag_arr("cf_prepass: bad stmt form", sizeof( unsigned char), 26u); _temp602;});
_temp601;})); _LL409:;} static int Cyc_NewControlFlow_iterate_cf_check= 0;
static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
env->all_changed, new_flow,( void*) env->tryflow));}} static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp603= Cyc_NewControlFlow_get_stmt_annot(
s); void* _temp604= Cyc_CfFlowInfo_join_flow( env->all_changed, flow,( void*)
_temp603->flow); if( ! Cyc_CfFlowInfo_flow_lessthan_approx( _temp604,( void*)
_temp603->flow)){( void*)( _temp603->flow=( void*) _temp604); if( _temp603->visited
==  Cyc_NewControlFlow_iteration_num){ Cyc_NewControlFlow_iterate_cf_check= 1;}}}
static struct Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv* env, void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp605= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( _temp605->flow=( void*) Cyc_CfFlowInfo_join_flow( env->all_changed,
in_flow,( void*) _temp605->flow)); ++ _temp605->visited; _temp605->visited == 
Cyc_NewControlFlow_iteration_num? 0:(((( int(*)( struct _tagged_arr assertion,
struct _tagged_arr file, unsigned int line)) Cyc_Std___assert_fail)( _tag_arr("annot->visited == iteration_num",
sizeof( unsigned char), 32u), _tag_arr("new_control_flow.cyc", sizeof(
unsigned char), 21u), 644), 0)); return _temp605;} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp606= flow; struct Cyc_Dict_Dict* _temp612; _LL608: if( _temp606 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL609;} else{ goto _LL610;} _LL610: if((
unsigned int) _temp606 >  1u?*(( int*) _temp606) ==  Cyc_CfFlowInfo_InitsFL: 0){
_LL613: _temp612=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp606)->f1; goto
_LL611;} else{ goto _LL607;} _LL609: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL611: for( 0; vds !=  0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp614=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp616=( struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp616[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp617; _temp617.tag= Cyc_CfFlowInfo_VarRoot;
_temp617.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
vds))->hd; _temp617;}); _temp616;}); void* _temp615= Cyc_CfFlowInfo_assign_unknown_dict((
void*) Cyc_CfFlowInfo_AllIL, Cyc_Dict_lookup( env->roots,( void*) _temp614));
_temp612= Cyc_Dict_insert( _temp612,( void*) _temp614, _temp615);} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp618=( struct Cyc_CfFlowInfo_InitsFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp618[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp619; _temp619.tag= Cyc_CfFlowInfo_InitsFL;
_temp619.f1= _temp612; _temp619;}); _temp618;}); _LL607:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp620= flow; struct Cyc_Dict_Dict* _temp626; _LL622: if( _temp620 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL623;} else{ goto _LL624;} _LL624: if((
unsigned int) _temp620 >  1u?*(( int*) _temp620) ==  Cyc_CfFlowInfo_InitsFL: 0){
_LL627: _temp626=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp620)->f1; goto
_LL625;} else{ goto _LL621;} _LL623: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL625: for( 0; vds !=  0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp628=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp629=( struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp629[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp630; _temp630.tag= Cyc_CfFlowInfo_VarRoot;
_temp630.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
vds))->hd; _temp630;}); _temp629;}); _temp626= Cyc_Dict_insert( _temp626,( void*)
_temp628, Cyc_Dict_lookup( env->roots,( void*) _temp628));} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp631=( struct Cyc_CfFlowInfo_InitsFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp631[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp632; _temp632.tag= Cyc_CfFlowInfo_InitsFL;
_temp632.f1= _temp626; _temp632;}); _temp631;}); _LL621:;} static struct _tuple0
Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if(( void*)((
struct Cyc_List_List*) _check_null( aes))->hd == ( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd){ void*
_temp633= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); return({ struct _tuple0 _temp634; _temp634.f1=
_temp633; _temp634.f2= _temp633; _temp634;});} return({ struct _tuple0 _temp635;
_temp635.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); _temp635.f2= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->hd, in_flow); _temp635;});} static void* Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs !=  0; scs=(( struct Cyc_List_List*)
_check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause _temp638; struct Cyc_Absyn_Stmt*
_temp639; struct Cyc_Absyn_Exp* _temp641; struct Cyc_Core_Opt* _temp643; struct
Cyc_Absyn_Switch_clause* _temp636=( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( scs))->hd; _temp638=* _temp636; _LL644: _temp643=
_temp638.pat_vars; goto _LL642; _LL642: _temp641= _temp638.where_clause; goto
_LL640; _LL640: _temp639= _temp638.body; goto _LL637; _LL637: { void* _temp645=
Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp643))->v); _temp645= Cyc_NewControlFlow_add_init_vars_flow(
env, _temp645,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp643))->v);{ void* body_outflow; if( _temp641 !=  0){ void* _temp648; void*
_temp650; struct _tuple0 _temp646= Cyc_NewControlFlow_cf_evalguard( env,((
struct Cyc_Absyn_Exp*) _check_null( _temp641))->loc, aes, _temp645); _LL651:
_temp650= _temp646.f1; goto _LL649; _LL649: _temp648= _temp646.f2; goto _LL647;
_LL647: aes=(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->tl; in_flow= _temp648; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp639, _temp650);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp639, _temp645);}{ void* _temp652= body_outflow; _LL654: if( _temp652
== ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL655;} else{ goto _LL656;} _LL656:
goto _LL657; _LL655: goto _LL653; _LL657: if((( struct Cyc_List_List*)
_check_null( scs))->tl ==  0){ return body_outflow;} else{ if(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( scs))->tl))->hd)->pat_vars))->v
!=  0){({ void* _temp658[ 0u]={}; Cyc_Tcutil_terr( _temp639->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp658, sizeof( void*), 0u));});}
else{({ void* _temp659[ 0u]={}; Cyc_Tcutil_warn( _temp639->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp659, sizeof( void*), 0u));});} Cyc_NewControlFlow_update_flow(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( scs))->tl))->hd)->body, body_outflow);} goto
_LL653; _LL653:;}}}} return( void*) Cyc_CfFlowInfo_BottomFL;} void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Absyn_Stmt* s, void* in_flow){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp660= Cyc_NewControlFlow_pre_stmt_check(
env, in_flow, s); in_flow=( void*) _temp660->flow;{ struct Cyc_List_List*
_temp661= _temp660->absexps; void* _temp662=( void*) s->r; struct Cyc_Absyn_Exp*
_temp708; struct Cyc_Absyn_Exp* _temp710; struct Cyc_Absyn_Exp* _temp712; struct
Cyc_Absyn_Stmt* _temp714; struct Cyc_Absyn_Stmt* _temp716; struct Cyc_Absyn_Stmt*
_temp718; struct Cyc_Absyn_Stmt* _temp720; struct Cyc_Absyn_Exp* _temp722;
struct Cyc_Absyn_Stmt* _temp724; struct _tuple3 _temp726; struct Cyc_Absyn_Stmt*
_temp728; struct Cyc_Absyn_Exp* _temp730; struct _tuple3 _temp732; struct Cyc_Absyn_Stmt*
_temp734; struct Cyc_Absyn_Exp* _temp736; struct Cyc_Absyn_Stmt* _temp738;
struct Cyc_Absyn_Stmt* _temp740; struct _tuple3 _temp742; struct Cyc_Absyn_Stmt*
_temp744; struct Cyc_Absyn_Exp* _temp746; struct _tuple3 _temp748; struct Cyc_Absyn_Stmt*
_temp750; struct Cyc_Absyn_Exp* _temp752; struct Cyc_Absyn_Exp* _temp754; struct
Cyc_Absyn_Stmt* _temp756; struct Cyc_Absyn_Switch_clause** _temp758; struct Cyc_Absyn_Switch_clause*
_temp760; struct Cyc_List_List* _temp761; struct Cyc_Absyn_Stmt* _temp763;
struct Cyc_Absyn_Stmt* _temp765; struct Cyc_Absyn_Stmt* _temp767; struct Cyc_List_List*
_temp769; struct Cyc_Absyn_Exp* _temp771; struct Cyc_List_List* _temp773; struct
Cyc_Absyn_Stmt* _temp775; struct Cyc_Absyn_Stmt* _temp777; struct Cyc_Absyn_Decl*
_temp779; struct Cyc_Absyn_Decl _temp781; void* _temp782; struct Cyc_Absyn_Vardecl*
_temp784; struct Cyc_Absyn_Stmt* _temp786; struct Cyc_Absyn_Decl* _temp788;
struct Cyc_Absyn_Decl _temp790; void* _temp791; struct Cyc_Absyn_Exp* _temp793;
struct Cyc_Core_Opt* _temp795; struct Cyc_Core_Opt _temp797; struct Cyc_List_List*
_temp798; struct Cyc_Absyn_Stmt* _temp800; struct Cyc_Absyn_Decl* _temp802;
struct Cyc_Absyn_Decl _temp804; void* _temp805; struct Cyc_List_List* _temp807;
struct Cyc_Absyn_Stmt* _temp809; struct Cyc_Absyn_Stmt* _temp811; struct Cyc_Absyn_Vardecl*
_temp813; struct Cyc_Absyn_Tvar* _temp815; _LL664: if( _temp662 == ( void*) Cyc_Absyn_Skip_s){
goto _LL665;} else{ goto _LL666;} _LL666: if(( unsigned int) _temp662 >  1u?*((
int*) _temp662) ==  Cyc_Absyn_Return_s: 0){ _LL709: _temp708=(( struct Cyc_Absyn_Return_s_struct*)
_temp662)->f1; if( _temp708 ==  0){ goto _LL667;} else{ goto _LL668;}} else{
goto _LL668;} _LL668: if(( unsigned int) _temp662 >  1u?*(( int*) _temp662) == 
Cyc_Absyn_Return_s: 0){ _LL711: _temp710=(( struct Cyc_Absyn_Return_s_struct*)
_temp662)->f1; goto _LL669;} else{ goto _LL670;} _LL670: if(( unsigned int)
_temp662 >  1u?*(( int*) _temp662) ==  Cyc_Absyn_Exp_s: 0){ _LL713: _temp712=((
struct Cyc_Absyn_Exp_s_struct*) _temp662)->f1; goto _LL671;} else{ goto _LL672;}
_LL672: if(( unsigned int) _temp662 >  1u?*(( int*) _temp662) ==  Cyc_Absyn_Seq_s:
0){ _LL717: _temp716=(( struct Cyc_Absyn_Seq_s_struct*) _temp662)->f1; goto
_LL715; _LL715: _temp714=(( struct Cyc_Absyn_Seq_s_struct*) _temp662)->f2; goto
_LL673;} else{ goto _LL674;} _LL674: if(( unsigned int) _temp662 >  1u?*(( int*)
_temp662) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL723: _temp722=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp662)->f1; goto _LL721; _LL721: _temp720=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp662)->f2; goto _LL719; _LL719: _temp718=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp662)->f3; goto _LL675;} else{ goto _LL676;} _LL676: if(( unsigned int)
_temp662 >  1u?*(( int*) _temp662) ==  Cyc_Absyn_While_s: 0){ _LL727: _temp726=((
struct Cyc_Absyn_While_s_struct*) _temp662)->f1; _LL731: _temp730= _temp726.f1;
goto _LL729; _LL729: _temp728= _temp726.f2; goto _LL725; _LL725: _temp724=((
struct Cyc_Absyn_While_s_struct*) _temp662)->f2; goto _LL677;} else{ goto _LL678;}
_LL678: if(( unsigned int) _temp662 >  1u?*(( int*) _temp662) ==  Cyc_Absyn_Do_s:
0){ _LL739: _temp738=(( struct Cyc_Absyn_Do_s_struct*) _temp662)->f1; goto
_LL733; _LL733: _temp732=(( struct Cyc_Absyn_Do_s_struct*) _temp662)->f2; _LL737:
_temp736= _temp732.f1; goto _LL735; _LL735: _temp734= _temp732.f2; goto _LL679;}
else{ goto _LL680;} _LL680: if(( unsigned int) _temp662 >  1u?*(( int*) _temp662)
==  Cyc_Absyn_For_s: 0){ _LL755: _temp754=(( struct Cyc_Absyn_For_s_struct*)
_temp662)->f1; goto _LL749; _LL749: _temp748=(( struct Cyc_Absyn_For_s_struct*)
_temp662)->f2; _LL753: _temp752= _temp748.f1; goto _LL751; _LL751: _temp750=
_temp748.f2; goto _LL743; _LL743: _temp742=(( struct Cyc_Absyn_For_s_struct*)
_temp662)->f3; _LL747: _temp746= _temp742.f1; goto _LL745; _LL745: _temp744=
_temp742.f2; goto _LL741; _LL741: _temp740=(( struct Cyc_Absyn_For_s_struct*)
_temp662)->f4; goto _LL681;} else{ goto _LL682;} _LL682: if(( unsigned int)
_temp662 >  1u?*(( int*) _temp662) ==  Cyc_Absyn_Break_s: 0){ _LL757: _temp756=((
struct Cyc_Absyn_Break_s_struct*) _temp662)->f1; if( _temp756 ==  0){ goto
_LL683;} else{ goto _LL684;}} else{ goto _LL684;} _LL684: if(( unsigned int)
_temp662 >  1u?*(( int*) _temp662) ==  Cyc_Absyn_Fallthru_s: 0){ _LL762:
_temp761=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp662)->f1; goto _LL759;
_LL759: _temp758=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp662)->f2; if(
_temp758 ==  0){ goto _LL686;} else{ _temp760=* _temp758; goto _LL685;}} else{
goto _LL686;} _LL686: if(( unsigned int) _temp662 >  1u?*(( int*) _temp662) == 
Cyc_Absyn_Break_s: 0){ _LL764: _temp763=(( struct Cyc_Absyn_Break_s_struct*)
_temp662)->f1; goto _LL687;} else{ goto _LL688;} _LL688: if(( unsigned int)
_temp662 >  1u?*(( int*) _temp662) ==  Cyc_Absyn_Continue_s: 0){ _LL766:
_temp765=(( struct Cyc_Absyn_Continue_s_struct*) _temp662)->f1; goto _LL689;}
else{ goto _LL690;} _LL690: if(( unsigned int) _temp662 >  1u?*(( int*) _temp662)
==  Cyc_Absyn_Goto_s: 0){ _LL768: _temp767=(( struct Cyc_Absyn_Goto_s_struct*)
_temp662)->f2; goto _LL691;} else{ goto _LL692;} _LL692: if(( unsigned int)
_temp662 >  1u?*(( int*) _temp662) ==  Cyc_Absyn_Switch_s: 0){ _LL772: _temp771=((
struct Cyc_Absyn_Switch_s_struct*) _temp662)->f1; goto _LL770; _LL770: _temp769=((
struct Cyc_Absyn_Switch_s_struct*) _temp662)->f2; goto _LL693;} else{ goto
_LL694;} _LL694: if(( unsigned int) _temp662 >  1u?*(( int*) _temp662) ==  Cyc_Absyn_TryCatch_s:
0){ _LL776: _temp775=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp662)->f1; goto
_LL774; _LL774: _temp773=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp662)->f2;
goto _LL695;} else{ goto _LL696;} _LL696: if(( unsigned int) _temp662 >  1u?*((
int*) _temp662) ==  Cyc_Absyn_Decl_s: 0){ _LL780: _temp779=(( struct Cyc_Absyn_Decl_s_struct*)
_temp662)->f1; _temp781=* _temp779; _LL783: _temp782=( void*) _temp781.r; if(*((
int*) _temp782) ==  Cyc_Absyn_Var_d){ _LL785: _temp784=(( struct Cyc_Absyn_Var_d_struct*)
_temp782)->f1; goto _LL778;} else{ goto _LL698;} _LL778: _temp777=(( struct Cyc_Absyn_Decl_s_struct*)
_temp662)->f2; goto _LL697;} else{ goto _LL698;} _LL698: if(( unsigned int)
_temp662 >  1u?*(( int*) _temp662) ==  Cyc_Absyn_Decl_s: 0){ _LL789: _temp788=((
struct Cyc_Absyn_Decl_s_struct*) _temp662)->f1; _temp790=* _temp788; _LL792:
_temp791=( void*) _temp790.r; if(*(( int*) _temp791) ==  Cyc_Absyn_Let_d){
_LL796: _temp795=(( struct Cyc_Absyn_Let_d_struct*) _temp791)->f2; if( _temp795
==  0){ goto _LL700;} else{ _temp797=* _temp795; _LL799: _temp798=( struct Cyc_List_List*)
_temp797.v; goto _LL794;} _LL794: _temp793=(( struct Cyc_Absyn_Let_d_struct*)
_temp791)->f4; goto _LL787;} else{ goto _LL700;} _LL787: _temp786=(( struct Cyc_Absyn_Decl_s_struct*)
_temp662)->f2; goto _LL699;} else{ goto _LL700;} _LL700: if(( unsigned int)
_temp662 >  1u?*(( int*) _temp662) ==  Cyc_Absyn_Decl_s: 0){ _LL803: _temp802=((
struct Cyc_Absyn_Decl_s_struct*) _temp662)->f1; _temp804=* _temp802; _LL806:
_temp805=( void*) _temp804.r; if(*(( int*) _temp805) ==  Cyc_Absyn_Letv_d){
_LL808: _temp807=(( struct Cyc_Absyn_Letv_d_struct*) _temp805)->f1; goto _LL801;}
else{ goto _LL702;} _LL801: _temp800=(( struct Cyc_Absyn_Decl_s_struct*)
_temp662)->f2; goto _LL701;} else{ goto _LL702;} _LL702: if(( unsigned int)
_temp662 >  1u?*(( int*) _temp662) ==  Cyc_Absyn_Label_s: 0){ _LL810: _temp809=((
struct Cyc_Absyn_Label_s_struct*) _temp662)->f2; goto _LL703;} else{ goto _LL704;}
_LL704: if(( unsigned int) _temp662 >  1u?*(( int*) _temp662) ==  Cyc_Absyn_Region_s:
0){ _LL816: _temp815=(( struct Cyc_Absyn_Region_s_struct*) _temp662)->f1; goto
_LL814; _LL814: _temp813=(( struct Cyc_Absyn_Region_s_struct*) _temp662)->f2;
goto _LL812; _LL812: _temp811=(( struct Cyc_Absyn_Region_s_struct*) _temp662)->f3;
goto _LL705;} else{ goto _LL706;} _LL706: goto _LL707; _LL665: return in_flow;
_LL667: return( void*) Cyc_CfFlowInfo_BottomFL; _LL669: Cyc_CfAbsexp_eval_absexp(
env,(( struct Cyc_Absyn_Exp*) _check_null( _temp710))->loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp661))->hd, in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL671: return Cyc_CfAbsexp_eval_absexp( env, _temp712->loc,( void*)(( struct
Cyc_List_List*) _check_null( _temp661))->hd, in_flow); _LL673: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp714, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp716, in_flow));
_LL675: { void* _temp819; void* _temp821; struct _tuple0 _temp817= Cyc_NewControlFlow_cf_evalguard(
env, _temp722->loc, _temp661, in_flow); _LL822: _temp821= _temp817.f1; goto
_LL820; _LL820: _temp819= _temp817.f2; goto _LL818; _LL818: return Cyc_CfFlowInfo_join_flow(
env->all_changed, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp720, _temp821),
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp718, _temp819));} _LL677: { void*
_temp823=( void*)( Cyc_NewControlFlow_pre_stmt_check( env, in_flow, _temp728))->flow;
void* _temp826; void* _temp828; struct _tuple0 _temp824= Cyc_NewControlFlow_cf_evalguard(
env, _temp730->loc, _temp661, _temp823); _LL829: _temp828= _temp824.f1; goto
_LL827; _LL827: _temp826= _temp824.f2; goto _LL825; _LL825: { void* _temp830=
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp724, _temp828); Cyc_NewControlFlow_update_flow(
env, _temp728, _temp830); return _temp826;}} _LL679: { void* _temp831= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp738, in_flow); void* _temp832=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp831, _temp734))->flow; void* _temp835; void* _temp837; struct _tuple0
_temp833= Cyc_NewControlFlow_cf_evalguard( env, _temp736->loc, _temp661,
_temp832); _LL838: _temp837= _temp833.f1; goto _LL836; _LL836: _temp835=
_temp833.f2; goto _LL834; _LL834: Cyc_NewControlFlow_update_flow( env, _temp738,
_temp837); return _temp835;} _LL681: { void* _temp839= Cyc_CfAbsexp_eval_absexp(
env, _temp754->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp661))->hd,
in_flow); void* _temp840=( void*)( Cyc_NewControlFlow_pre_stmt_check( env,
_temp839, _temp750))->flow; void* _temp843; void* _temp845; struct _tuple0
_temp841= Cyc_NewControlFlow_cf_evalguard( env, _temp752->loc,(( struct Cyc_List_List*)
_check_null( _temp661))->tl, _temp840); _LL846: _temp845= _temp841.f1; goto
_LL844; _LL844: _temp843= _temp841.f2; goto _LL842; _LL842: { void* _temp847=
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp740, _temp845); void* _temp848=(
void*)( Cyc_NewControlFlow_pre_stmt_check( env, _temp847, _temp744))->flow; void*
_temp849= Cyc_CfAbsexp_eval_absexp( env, _temp746->loc,( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp661))->tl))->tl))->tl))->hd,
_temp848); Cyc_NewControlFlow_update_flow( env, _temp750, _temp849); return
_temp843;}} _LL683: return( void*) Cyc_CfFlowInfo_BottomFL; _LL685: { void*
_temp850= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp661))->hd, in_flow); struct Cyc_List_List* _temp851=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp760->pat_vars))->v;
_temp850= Cyc_NewControlFlow_add_vardecls_flow( env, _temp850, _temp851);
_temp850= Cyc_NewControlFlow_add_init_vars_flow( env, _temp850, _temp851); Cyc_NewControlFlow_update_flow(
env,( struct Cyc_Absyn_Stmt*) _temp760->body, _temp850); return( void*) Cyc_CfFlowInfo_BottomFL;}
_LL687: _temp765= _temp763; goto _LL689; _LL689: _temp767= _temp765; goto _LL691;
_LL691: Cyc_NewControlFlow_update_flow( env,( struct Cyc_Absyn_Stmt*)
_check_null( _temp767), in_flow); return( void*) Cyc_CfFlowInfo_BottomFL; _LL693:
return Cyc_NewControlFlow_cf_analyze_switch_clauses( env, _temp769,(( struct Cyc_List_List*)
_check_null( _temp661))->tl, Cyc_CfAbsexp_eval_absexp( env, _temp771->loc,( void*)((
struct Cyc_List_List*) _check_null( _temp661))->hd, in_flow)); _LL695: { struct
Cyc_NewControlFlow_AnalEnv _temp854; void* _temp855; int _temp857; struct Cyc_NewControlFlow_AnalEnv*
_temp852= env; _temp854=* _temp852; _LL858: _temp857= _temp854.in_try; goto
_LL856; _LL856: _temp855=( void*) _temp854.tryflow; goto _LL853; _LL853: env->in_try=
1;( void*)( env->tryflow=( void*) in_flow);{ void* _temp859= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp775, in_flow); void* _temp860=( void*) env->tryflow; env->in_try=
_temp857;( void*)( env->tryflow=( void*) _temp855); Cyc_NewControlFlow_update_tryflow(
env, _temp860);{ void* _temp861= Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp773, _temp661, _temp860); return Cyc_CfFlowInfo_join_flow( env->all_changed,
_temp859, _temp861);}}} _LL697: { struct Cyc_List_List _temp862=({ struct Cyc_List_List
_temp865; _temp865.hd=( void*) _temp784; _temp865.tl= 0; _temp865;}); void*
_temp863= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)&
_temp862); struct Cyc_Absyn_Exp* _temp864= _temp784->initializer; if( _temp864
!=  0){ _temp863= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)
_check_null( _temp864))->loc,( void*)(( struct Cyc_List_List*) _check_null(
_temp661))->hd, _temp863);} return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp777, _temp863);} _LL699: { void* _temp866= Cyc_CfAbsexp_eval_absexp( env,
_temp793->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp661))->hd, Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp798)); _temp866= Cyc_NewControlFlow_add_init_vars_flow( env,
_temp866, _temp798); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp786,
_temp866);} _LL701: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp800,
Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, _temp807)); _LL703: return
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp809, in_flow); _LL705: { struct
Cyc_List_List* _temp867=({ struct Cyc_List_List* _temp869=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp869->hd=( void*) _temp813;
_temp869->tl= 0; _temp869;}); void* _temp868= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp867); _temp868= Cyc_NewControlFlow_add_init_vars_flow( env,
_temp868, _temp867); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp811,
_temp868);} _LL707:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp870=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp870[ 0]=({ struct Cyc_Core_Impossible_struct _temp871; _temp871.tag= Cyc_Core_Impossible;
_temp871.f1= _tag_arr("cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof( unsigned char), 62u); _temp871;}); _temp870;})); _LL663:;}} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{
void* _temp872=( void*) s->r; struct Cyc_Absyn_Stmt* _temp896; struct Cyc_Absyn_Stmt*
_temp898; struct Cyc_Absyn_Stmt* _temp900; struct Cyc_Absyn_Stmt* _temp902;
struct Cyc_Absyn_Stmt* _temp904; struct _tuple3 _temp906; struct Cyc_Absyn_Stmt*
_temp908; struct _tuple3 _temp910; struct Cyc_Absyn_Stmt* _temp912; struct Cyc_Absyn_Stmt*
_temp914; struct _tuple3 _temp916; struct Cyc_Absyn_Stmt* _temp918; struct
_tuple3 _temp920; struct Cyc_Absyn_Stmt* _temp922; struct Cyc_Absyn_Stmt*
_temp924; struct Cyc_Absyn_Stmt* _temp926; struct Cyc_Absyn_Stmt* _temp928;
struct Cyc_Absyn_Stmt* _temp930; struct Cyc_List_List* _temp932; struct Cyc_Absyn_Stmt*
_temp934; struct Cyc_List_List* _temp936; _LL874: if(( unsigned int) _temp872 > 
1u?*(( int*) _temp872) ==  Cyc_Absyn_Seq_s: 0){ _LL899: _temp898=(( struct Cyc_Absyn_Seq_s_struct*)
_temp872)->f1; goto _LL897; _LL897: _temp896=(( struct Cyc_Absyn_Seq_s_struct*)
_temp872)->f2; goto _LL875;} else{ goto _LL876;} _LL876: if(( unsigned int)
_temp872 >  1u?*(( int*) _temp872) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL903:
_temp902=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp872)->f2; goto _LL901;
_LL901: _temp900=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp872)->f3; goto
_LL877;} else{ goto _LL878;} _LL878: if(( unsigned int) _temp872 >  1u?*(( int*)
_temp872) ==  Cyc_Absyn_For_s: 0){ _LL911: _temp910=(( struct Cyc_Absyn_For_s_struct*)
_temp872)->f2; _LL913: _temp912= _temp910.f2; goto _LL907; _LL907: _temp906=((
struct Cyc_Absyn_For_s_struct*) _temp872)->f3; _LL909: _temp908= _temp906.f2;
goto _LL905; _LL905: _temp904=(( struct Cyc_Absyn_For_s_struct*) _temp872)->f4;
goto _LL879;} else{ goto _LL880;} _LL880: if(( unsigned int) _temp872 >  1u?*((
int*) _temp872) ==  Cyc_Absyn_While_s: 0){ _LL917: _temp916=(( struct Cyc_Absyn_While_s_struct*)
_temp872)->f1; _LL919: _temp918= _temp916.f2; goto _LL915; _LL915: _temp914=((
struct Cyc_Absyn_While_s_struct*) _temp872)->f2; goto _LL881;} else{ goto _LL882;}
_LL882: if(( unsigned int) _temp872 >  1u?*(( int*) _temp872) ==  Cyc_Absyn_Do_s:
0){ _LL925: _temp924=(( struct Cyc_Absyn_Do_s_struct*) _temp872)->f1; goto
_LL921; _LL921: _temp920=(( struct Cyc_Absyn_Do_s_struct*) _temp872)->f2; _LL923:
_temp922= _temp920.f2; goto _LL883;} else{ goto _LL884;} _LL884: if((
unsigned int) _temp872 >  1u?*(( int*) _temp872) ==  Cyc_Absyn_Region_s: 0){
_LL927: _temp926=(( struct Cyc_Absyn_Region_s_struct*) _temp872)->f3; goto
_LL885;} else{ goto _LL886;} _LL886: if(( unsigned int) _temp872 >  1u?*(( int*)
_temp872) ==  Cyc_Absyn_Label_s: 0){ _LL929: _temp928=(( struct Cyc_Absyn_Label_s_struct*)
_temp872)->f2; goto _LL887;} else{ goto _LL888;} _LL888: if(( unsigned int)
_temp872 >  1u?*(( int*) _temp872) ==  Cyc_Absyn_Decl_s: 0){ _LL931: _temp930=((
struct Cyc_Absyn_Decl_s_struct*) _temp872)->f2; goto _LL889;} else{ goto _LL890;}
_LL890: if(( unsigned int) _temp872 >  1u?*(( int*) _temp872) ==  Cyc_Absyn_TryCatch_s:
0){ _LL935: _temp934=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp872)->f1; goto
_LL933; _LL933: _temp932=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp872)->f2;
goto _LL891;} else{ goto _LL892;} _LL892: if(( unsigned int) _temp872 >  1u?*((
int*) _temp872) ==  Cyc_Absyn_Switch_s: 0){ _LL937: _temp936=(( struct Cyc_Absyn_Switch_s_struct*)
_temp872)->f2; goto _LL893;} else{ goto _LL894;} _LL894: goto _LL895; _LL875:
_temp902= _temp898; _temp900= _temp896; goto _LL877; _LL877: Cyc_NewControlFlow_cf_postpass(
_temp902); Cyc_NewControlFlow_cf_postpass( _temp900); return; _LL879: Cyc_NewControlFlow_cf_postpass(
_temp908); _temp918= _temp904; _temp914= _temp912; goto _LL881; _LL881: _temp924=
_temp914; _temp922= _temp918; goto _LL883; _LL883: Cyc_NewControlFlow_cf_postpass(
_temp922); _temp926= _temp924; goto _LL885; _LL885: _temp928= _temp926; goto
_LL887; _LL887: _temp930= _temp928; goto _LL889; _LL889: Cyc_NewControlFlow_cf_postpass(
_temp930); return; _LL891: Cyc_NewControlFlow_cf_postpass( _temp934); _temp936=
_temp932; goto _LL893; _LL893: for( 0; _temp936 !=  0; _temp936=(( struct Cyc_List_List*)
_check_null( _temp936))->tl){ Cyc_NewControlFlow_cf_postpass((( struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*) _check_null( _temp936))->hd)->body);} return; _LL895:
return; _LL873:;}} void Cyc_NewControlFlow_cf_check_fun( struct Cyc_Absyn_Fndecl*
fd){ struct Cyc_NewControlFlow_AbsEnv* absenv=({ struct Cyc_NewControlFlow_AbsEnv*
_temp950=( struct Cyc_NewControlFlow_AbsEnv*) _cycalloc( sizeof( struct Cyc_NewControlFlow_AbsEnv));
_temp950->all_roots= Cyc_Dict_empty( Cyc_CfFlowInfo_local_root_cmp); _temp950;});
struct Cyc_List_List* _temp938=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( fd->param_vardecls))->v;{ struct Cyc_List_List* vds= _temp938; for(
0; vds !=  0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root(
absenv,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd);}}
Cyc_NewControlFlow_cf_prepass( absenv, fd->body);{ struct Cyc_NewControlFlow_AnalEnv*
_temp939=({ struct Cyc_NewControlFlow_AnalEnv* _temp949=( struct Cyc_NewControlFlow_AnalEnv*)
_cycalloc( sizeof( struct Cyc_NewControlFlow_AnalEnv)); _temp949->roots= absenv->all_roots;
_temp949->in_try= 0; _temp949->tryflow=( void*)(( void*) Cyc_CfFlowInfo_BottomFL);
_temp949->all_changed= 0; _temp949;}); void* in_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp947=( struct Cyc_CfFlowInfo_InitsFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp947[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp948; _temp948.tag= Cyc_CfFlowInfo_InitsFL;
_temp948.f1= Cyc_Dict_empty( Cyc_CfFlowInfo_local_root_cmp); _temp948;});
_temp947;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow( _temp939, in_flow,
_temp938);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num= 0; Cyc_NewControlFlow_iterate_cf_check=
1; while( Cyc_NewControlFlow_iterate_cf_check) { ++ Cyc_NewControlFlow_iteration_num;
Cyc_NewControlFlow_iterate_cf_check= 0; out_flow= Cyc_NewControlFlow_cf_analyze_stmt(
_temp939, fd->body, in_flow);}{ void* _temp940= out_flow; _LL942: if( _temp940
== ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL943;} else{ goto _LL944;} _LL944:
goto _LL945; _LL943: goto _LL941; _LL945: if(( void*) fd->ret_type != ( void*)
Cyc_Absyn_VoidType){({ void* _temp946[ 0u]={}; Cyc_Tcutil_terr(( fd->body)->loc,
_tag_arr("function may complete without returning a value", sizeof(
unsigned char), 48u), _tag_arr( _temp946, sizeof( void*), 0u));});} goto _LL941;
_LL941:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds !=  0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp951=( void*)(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd)->r; struct Cyc_Absyn_Fndecl*
_temp963; struct Cyc_List_List* _temp965; struct Cyc_List_List* _temp967; struct
Cyc_List_List* _temp969; _LL953: if(*(( int*) _temp951) ==  Cyc_Absyn_Fn_d){
_LL964: _temp963=(( struct Cyc_Absyn_Fn_d_struct*) _temp951)->f1; goto _LL954;}
else{ goto _LL955;} _LL955: if(*(( int*) _temp951) ==  Cyc_Absyn_ExternC_d){
_LL966: _temp965=(( struct Cyc_Absyn_ExternC_d_struct*) _temp951)->f1; goto
_LL956;} else{ goto _LL957;} _LL957: if(*(( int*) _temp951) ==  Cyc_Absyn_Using_d){
_LL968: _temp967=(( struct Cyc_Absyn_Using_d_struct*) _temp951)->f2; goto _LL958;}
else{ goto _LL959;} _LL959: if(*(( int*) _temp951) ==  Cyc_Absyn_Namespace_d){
_LL970: _temp969=(( struct Cyc_Absyn_Namespace_d_struct*) _temp951)->f2; goto
_LL960;} else{ goto _LL961;} _LL961: goto _LL962; _LL954: Cyc_NewControlFlow_cf_check_fun(
_temp963); goto _LL952; _LL956: _temp967= _temp965; goto _LL958; _LL958:
_temp969= _temp967; goto _LL960; _LL960: Cyc_NewControlFlow_cf_check( _temp969);
goto _LL952; _LL962: goto _LL952; _LL952:;}}
