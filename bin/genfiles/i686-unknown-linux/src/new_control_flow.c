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
struct _tagged_arr); extern struct Cyc_List_List* Cyc_List_tl( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u];
struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[
11u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* k,
void* v); extern struct Cyc_Dict_Dict* Cyc_Dict_insert_new( struct Cyc_Dict_Dict*
d, void* k, void* v); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* k); extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*);
extern void* Cyc_Std___assert_fail( struct _tagged_arr assertion, struct
_tagged_arr file, unsigned int line); struct Cyc_Lineno_Pos{ struct _tagged_arr
logical_file; struct _tagged_arr line; int line_no; int col; } ; extern
unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static const
int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static const
int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr source;
struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; } ;
extern unsigned char Cyc_Position_Nocontext[ 14u]; struct _tuple1{ void* f1;
struct _tagged_arr* f2; } ; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual;
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
15u]; extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*,
struct _tagged_arr*, struct Cyc_Position_Segment*); extern struct _tagged_arr*
Cyc_Absyn_fieldname( int); struct Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[
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
int Cyc_Tcutil_is_tagged_pointer_typ( void*); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); struct Cyc_CfFlowInfo_Place; enum  Cyc_CfFlowInfo_Escaped;
struct Cyc_CfFlowInfo_InitState; static const int Cyc_CfFlowInfo_VarRoot= 0;
struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_CfFlowInfo_Place{ void* root;
struct Cyc_List_List* fields; } ; enum  Cyc_CfFlowInfo_Escaped{ Cyc_CfFlowInfo_Esc
 =  0u, Cyc_CfFlowInfo_Unesc  =  1u}; static const int Cyc_CfFlowInfo_NoneIL= 0;
static const int Cyc_CfFlowInfo_ThisIL= 1; static const int Cyc_CfFlowInfo_AllIL=
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
extern void* Cyc_CfAbsexp_mkAddressOp( void* ao); extern void* Cyc_CfAbsexp_mkLocalOp(
struct Cyc_Absyn_Vardecl* vd); extern void* Cyc_CfAbsexp_mkMemberOp( void* ao,
struct _tagged_arr* f); extern void* Cyc_CfAbsexp_mkMallocOp( struct Cyc_Absyn_Exp*
e); extern void* Cyc_CfAbsexp_mkDerefOp( void* ao); extern void* Cyc_CfAbsexp_mkBottomAE();
extern void* Cyc_CfAbsexp_mkSkipAE(); extern void* Cyc_CfAbsexp_mkAssignAE( void*
l, void* r); extern void* Cyc_CfAbsexp_mkMallocAE( struct Cyc_Absyn_Exp* e);
extern void* Cyc_CfAbsexp_mkStmtAE( struct Cyc_Absyn_Stmt* s); extern void* Cyc_CfAbsexp_mkGroupAE(
void* g, void* ae1, void* ae2); extern void* Cyc_CfAbsexp_mkGroupAE_l( void* g,
struct Cyc_List_List* ael); extern int Cyc_CfAbsexp_isUnknownOp( void*); extern
void* Cyc_CfAbsexp_eval_absexp( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, void* ae, void* in_flow); struct Cyc_PP_Ppstate;
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
unsigned char), 37u); _temp13;}); _temp12;})); _LL4:;} void Cyc_NewControlFlow_cf_set_absexps(
struct Cyc_Absyn_Stmt* s, struct Cyc_List_List* ael){( Cyc_NewControlFlow_get_stmt_annot(
s))->absexps= ael;} struct Cyc_NewControlFlow_SharedAbsEnv{ struct Cyc_Dict_Dict*
all_roots; struct Cyc_List_List* comprehension_vars; } ; struct Cyc_NewControlFlow_AbsEnv{
struct Cyc_NewControlFlow_SharedAbsEnv* shared; struct Cyc_List_List* lhsides;
struct Cyc_List_List* access_path; } ; struct Cyc_NewControlFlow_AbsSynOne{ void*
inner_exp; void* assigns; } ; struct Cyc_NewControlFlow_AbsSyn{ struct Cyc_NewControlFlow_AbsSynOne
when_true; struct Cyc_NewControlFlow_AbsSynOne when_false; struct Cyc_List_List*
lvalues; } ; void Cyc_NewControlFlow_add_var_root( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Vardecl* vd){( env.shared)->all_roots= Cyc_Dict_insert_new((
env.shared)->all_roots,( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp14=(
struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp14[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp15; _temp15.tag= Cyc_CfFlowInfo_VarRoot;
_temp15.f1= vd; _temp15;}); _temp14;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo((
void*) vd->type));} void Cyc_NewControlFlow_add_malloc_root( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e, void* t){( env.shared)->all_roots= Cyc_Dict_insert_new((
env.shared)->all_roots,( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp16=(
struct Cyc_CfFlowInfo_MallocPt_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp16[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp17; _temp17.tag= Cyc_CfFlowInfo_MallocPt;
_temp17.f1= e; _temp17;}); _temp16;}), Cyc_CfFlowInfo_typ_to_unesc_none_pinfo( t));}
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_change_lhs( struct Cyc_NewControlFlow_AbsEnv
src, struct Cyc_List_List* new_lhs){ return({ struct Cyc_NewControlFlow_AbsEnv
_temp18; _temp18.shared= src.shared; _temp18.lhsides= new_lhs; _temp18.access_path=
src.access_path; _temp18;});} static struct Cyc_List_List* Cyc_NewControlFlow_unknown_list=
0; static struct Cyc_List_List* Cyc_NewControlFlow_use_it_list(){ if( Cyc_NewControlFlow_unknown_list
==  0){ Cyc_NewControlFlow_unknown_list=({ struct Cyc_List_List* _temp19=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp19->hd=(
void*) Cyc_CfAbsexp_mkUnknownOp(); _temp19->tl= 0; _temp19;});} return Cyc_NewControlFlow_unknown_list;}
struct Cyc_NewControlFlow_AbsEnv Cyc_NewControlFlow_use_it_env( struct Cyc_NewControlFlow_AbsEnv
src){ struct Cyc_NewControlFlow_AbsEnv _temp20= Cyc_NewControlFlow_change_lhs(
src, Cyc_NewControlFlow_use_it_list()); _temp20.access_path= 0; return _temp20;}
struct Cyc_NewControlFlow_AbsSynOne Cyc_NewControlFlow_meet_absexp( struct Cyc_NewControlFlow_AbsSyn
syn){ struct Cyc_NewControlFlow_AbsSynOne _temp23; void* _temp25; void* _temp27;
struct Cyc_NewControlFlow_AbsSynOne _temp29; void* _temp31; void* _temp33;
struct Cyc_NewControlFlow_AbsSyn _temp21= syn; _LL30: _temp29= _temp21.when_true;
_LL34: _temp33=( void*) _temp29.inner_exp; goto _LL32; _LL32: _temp31=( void*)
_temp29.assigns; goto _LL24; _LL24: _temp23= _temp21.when_false; _LL28: _temp27=(
void*) _temp23.inner_exp; goto _LL26; _LL26: _temp25=( void*) _temp23.assigns;
goto _LL22; _LL22: { void* _temp35= _temp33 ==  _temp27? _temp33: Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OneofG, _temp33, _temp27); void* _temp36= _temp31 == 
_temp25? _temp31: Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, _temp31,
_temp25); return({ struct Cyc_NewControlFlow_AbsSynOne _temp37; _temp37.inner_exp=(
void*) _temp35; _temp37.assigns=( void*) _temp36; _temp37;});}} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_mkSyn_tf( void* ae, struct Cyc_List_List* lvals, void*
assigns){ struct Cyc_NewControlFlow_AbsSynOne _temp38=({ struct Cyc_NewControlFlow_AbsSynOne
_temp40; _temp40.inner_exp=( void*) ae; _temp40.assigns=( void*) assigns;
_temp40;}); return({ struct Cyc_NewControlFlow_AbsSyn _temp39; _temp39.when_true=
_temp38; _temp39.when_false= _temp38; _temp39.lvalues= lvals; _temp39;});} void*
Cyc_NewControlFlow_make_assigns( struct Cyc_NewControlFlow_AbsEnv env, void* ae){
if( env.lhsides ==  0){ return Cyc_CfAbsexp_mkSkipAE();} if((( struct Cyc_List_List*)
_check_null( env.lhsides))->tl ==  0){ return Cyc_CfAbsexp_mkAssignAE(( void*)((
struct Cyc_List_List*) _check_null( env.lhsides))->hd, ae);}{ struct Cyc_List_List*
_temp41= 0;{ struct Cyc_List_List* lhsides= env.lhsides; for( 0; lhsides !=  0;
lhsides=(( struct Cyc_List_List*) _check_null( lhsides))->tl){ _temp41=({ struct
Cyc_List_List* _temp42=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp42->hd=( void*) Cyc_CfAbsexp_mkAssignAE(( void*)(( struct Cyc_List_List*)
_check_null( lhsides))->hd, ae); _temp42->tl= _temp41; _temp42;});}} return Cyc_CfAbsexp_mkGroupAE_l((
void*) Cyc_CfAbsexp_OneofG, _temp41);}} struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e); void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s); struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_use_exp_unordered( struct Cyc_NewControlFlow_AbsEnv env,
struct Cyc_List_List* es, struct Cyc_List_List* lhs){ struct Cyc_NewControlFlow_AbsEnv
elt_env= Cyc_NewControlFlow_use_it_env( env); void* es_exp= Cyc_CfAbsexp_mkSkipAE();
void* uses_exp= Cyc_CfAbsexp_mkSkipAE(); for( 0; es !=  0; es=(( struct Cyc_List_List*)
_check_null( es))->tl){ struct Cyc_NewControlFlow_AbsSyn _temp43= Cyc_NewControlFlow_abstract_exp(
elt_env,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);
void* _temp46; void* _temp48; struct Cyc_NewControlFlow_AbsSynOne _temp44= Cyc_NewControlFlow_meet_absexp(
_temp43); _LL49: _temp48=( void*) _temp44.inner_exp; goto _LL47; _LL47: _temp46=(
void*) _temp44.assigns; goto _LL45; _LL45: es_exp= Cyc_CfAbsexp_mkGroupAE(( void*)
Cyc_CfAbsexp_UnorderedG, _temp48, es_exp); uses_exp= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp46, uses_exp);} return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, es_exp, uses_exp), lhs,
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));} struct Cyc_NewControlFlow_AbsSyn
Cyc_NewControlFlow_descend_path( struct Cyc_NewControlFlow_AbsEnv env, struct
Cyc_Absyn_Exp* path_e, struct Cyc_Absyn_Exp* inner_e){ env.access_path=({ struct
Cyc_List_List* _temp50=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp50->hd=( void*) path_e; _temp50->tl= env.access_path; _temp50;});{ struct
Cyc_NewControlFlow_AbsSyn _temp51= Cyc_NewControlFlow_abstract_exp( env, inner_e);
if( _temp51.lvalues ==  0){ _temp51.lvalues= Cyc_NewControlFlow_use_it_list();}
return _temp51;}} void* Cyc_NewControlFlow_destruct_path( struct Cyc_NewControlFlow_AbsEnv
env, void* absop){{ struct Cyc_List_List* path= env.access_path; for( 0; path != 
0; path=(( struct Cyc_List_List*) _check_null( path))->tl){ void* _temp52=( void*)((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( path))->hd)->r;
struct _tagged_arr* _temp66; struct _tagged_arr* _temp68; struct Cyc_Absyn_Exp*
_temp70; _LL54: if(*(( int*) _temp52) ==  Cyc_Absyn_Address_e){ goto _LL55;}
else{ goto _LL56;} _LL56: if(*(( int*) _temp52) ==  Cyc_Absyn_Deref_e){ goto
_LL57;} else{ goto _LL58;} _LL58: if(*(( int*) _temp52) ==  Cyc_Absyn_StructArrow_e){
_LL67: _temp66=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp52)->f2; goto
_LL59;} else{ goto _LL60;} _LL60: if(*(( int*) _temp52) ==  Cyc_Absyn_StructMember_e){
_LL69: _temp68=(( struct Cyc_Absyn_StructMember_e_struct*) _temp52)->f2; goto
_LL61;} else{ goto _LL62;} _LL62: if(*(( int*) _temp52) ==  Cyc_Absyn_Subscript_e){
_LL71: _temp70=(( struct Cyc_Absyn_Subscript_e_struct*) _temp52)->f2; goto _LL63;}
else{ goto _LL64;} _LL64: goto _LL65; _LL55:(( struct Cyc_List_List*)
_check_null( path))->tl ==  0? 0:(((( int(*)( struct _tagged_arr assertion,
struct _tagged_arr file, unsigned int line)) Cyc_Std___assert_fail)( _tag_arr("path->tl == NULL",
sizeof( unsigned char), 17u), _tag_arr("new_control_flow.cyc", sizeof(
unsigned char), 21u), 236), 0)); return Cyc_CfAbsexp_mkAddressOp( absop); _LL57:
absop= Cyc_CfAbsexp_mkDerefOp( absop); goto _LL53; _LL59: absop= Cyc_CfAbsexp_mkDerefOp(
absop); _temp68= _temp66; goto _LL61; _LL61: absop= Cyc_CfAbsexp_mkMemberOp(
absop, _temp68); goto _LL53; _LL63: { unsigned int _temp72= Cyc_Evexp_eval_const_uint_exp(
_temp70); absop= Cyc_CfAbsexp_mkMemberOp( absop, Cyc_Absyn_fieldname(( int)
_temp72)); goto _LL53;} _LL65:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp73=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp73[ 0]=({ struct Cyc_Core_Impossible_struct _temp74; _temp74.tag= Cyc_Core_Impossible;
_temp74.f1= _tag_arr("abstract_exp: unexpected acces path element", sizeof(
unsigned char), 44u); _temp74;}); _temp73;})); _LL53:;}} return absop;} struct
Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_malloc( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* outer_e, struct Cyc_Absyn_Exp* rgn_exp, struct Cyc_Absyn_Exp*
init_exp){ void* _temp75= Cyc_CfAbsexp_mkMallocOp( outer_e); void* _temp76= Cyc_CfAbsexp_mkSkipAE();
void* _temp77= Cyc_CfAbsexp_mkSkipAE(); if( init_exp !=  0){ struct Cyc_NewControlFlow_AbsEnv
_temp78= Cyc_NewControlFlow_change_lhs( env,({ struct Cyc_List_List* _temp85=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp85->hd=(
void*) _temp75; _temp85->tl= 0; _temp85;})); _temp78.access_path= 0;{ void*
_temp81; void* _temp83; struct Cyc_NewControlFlow_AbsSynOne _temp79= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp78,( struct Cyc_Absyn_Exp*) _check_null(
init_exp))); _LL84: _temp83=( void*) _temp79.inner_exp; goto _LL82; _LL82:
_temp81=( void*) _temp79.assigns; goto _LL80; _LL80: _temp76= _temp83; _temp77=
_temp81;}} if( rgn_exp !=  0){ struct Cyc_NewControlFlow_AbsSyn _temp86= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*) _check_null(
rgn_exp)); void* _temp89; void* _temp91; struct Cyc_NewControlFlow_AbsSynOne
_temp87= Cyc_NewControlFlow_meet_absexp( _temp86); _LL92: _temp91=( void*)
_temp87.inner_exp; goto _LL90; _LL90: _temp89=( void*) _temp87.assigns; goto
_LL88; _LL88: _temp76= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp76, _temp91); _temp77= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp77, _temp89);}{ void* _temp93= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkMallocAE( outer_e), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp76, _temp77)); struct Cyc_NewControlFlow_AbsSyn _temp94= Cyc_NewControlFlow_mkSyn_tf(
_temp93, 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkAddressOp(
_temp75))); if( env.access_path !=  0){ void* _temp95=( void*)(( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( env.access_path))->hd)->r; struct
_tagged_arr* _temp103; struct Cyc_Absyn_Exp* _temp105; _LL97: if(*(( int*)
_temp95) ==  Cyc_Absyn_Deref_e){ goto _LL98;} else{ goto _LL99;} _LL99: if(*((
int*) _temp95) ==  Cyc_Absyn_StructArrow_e){ _LL106: _temp105=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp95)->f1; goto _LL104; _LL104: _temp103=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp95)->f2; goto _LL100;} else{ goto _LL101;} _LL101: goto _LL102; _LL98: env.access_path=((
struct Cyc_List_List*) _check_null( env.access_path))->tl; _temp94.lvalues=({
struct Cyc_List_List* _temp107=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp107->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, _temp75); _temp107->tl= 0; _temp107;}); goto _LL96; _LL100: env.access_path=({
struct Cyc_List_List* _temp108=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp108->hd=( void*) Cyc_Absyn_structmember_exp(
_temp105, _temp103, 0); _temp108->tl=(( struct Cyc_List_List*) _check_null( env.access_path))->tl;
_temp108;}); _temp94.lvalues=({ struct Cyc_List_List* _temp109=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp109->hd=( void*) Cyc_NewControlFlow_destruct_path(
env, _temp75); _temp109->tl= 0; _temp109;}); goto _LL96; _LL102:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp110=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp110[ 0]=({ struct
Cyc_Core_Impossible_struct _temp111; _temp111.tag= Cyc_Core_Impossible; _temp111.f1=
_tag_arr("bad malloc access path", sizeof( unsigned char), 23u); _temp111;});
_temp110;})); _LL96:;} return _temp94;}} struct _tuple4{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; struct Cyc_NewControlFlow_AbsSyn Cyc_NewControlFlow_abstract_exp(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp112=(
void*) e->r; struct Cyc_Absyn_Exp* _temp202; struct Cyc_Absyn_Exp* _temp204;
void* _temp206; int _temp208; void* _temp210; void* _temp212; int _temp214; void*
_temp216; void* _temp218; void* _temp220; void* _temp222; struct Cyc_Absyn_Vardecl*
_temp224; void* _temp226; struct Cyc_Absyn_Vardecl* _temp228; void* _temp230;
struct Cyc_Absyn_Vardecl* _temp232; struct Cyc_Absyn_Exp* _temp234; struct Cyc_Core_Opt*
_temp236; struct Cyc_Absyn_Exp* _temp238; struct Cyc_Absyn_Exp* _temp240; struct
Cyc_Absyn_Exp* _temp242; struct Cyc_List_List* _temp244; struct Cyc_Absyn_Exp*
_temp246; struct Cyc_List_List* _temp248; struct Cyc_Absyn_Exp* _temp250; struct
Cyc_Absyn_Exp* _temp252; struct Cyc_Absyn_Exp* _temp254; struct Cyc_Absyn_Exp*
_temp256; struct Cyc_Absyn_Exp* _temp258; struct Cyc_Absyn_Exp* _temp260; void*
_temp262; struct Cyc_Absyn_Exp* _temp264; struct Cyc_Absyn_Exp* _temp266; struct
Cyc_Absyn_Exp* _temp268; struct Cyc_Absyn_Exp* _temp270; struct Cyc_List_List*
_temp272; struct Cyc_List_List* _temp274; struct Cyc_List_List* _temp276; struct
Cyc_List_List* _temp278; struct Cyc_List_List* _temp280; struct Cyc_Absyn_Exp*
_temp282; struct Cyc_Absyn_Exp* _temp284; struct Cyc_Absyn_Exp* _temp286; struct
Cyc_Absyn_Exp* _temp288; struct Cyc_Absyn_Exp* _temp290; struct Cyc_Absyn_Stmt*
_temp292; struct Cyc_Absyn_Exp* _temp294; struct Cyc_Absyn_Exp* _temp296; struct
Cyc_Absyn_Exp* _temp298; struct Cyc_Absyn_Exp* _temp300; struct Cyc_Absyn_Vardecl*
_temp302; void* _temp304; _LL114: if(*(( int*) _temp112) ==  Cyc_Absyn_NoInstantiate_e){
_LL203: _temp202=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp112)->f1;
goto _LL115;} else{ goto _LL116;} _LL116: if(*(( int*) _temp112) ==  Cyc_Absyn_Instantiate_e){
_LL205: _temp204=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp112)->f1; goto
_LL117;} else{ goto _LL118;} _LL118: if(*(( int*) _temp112) ==  Cyc_Absyn_Const_e){
_LL207: _temp206=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp112)->f1; if((
unsigned int) _temp206 >  1u?*(( int*) _temp206) ==  Cyc_Absyn_Int_c: 0){ _LL211:
_temp210=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp206)->f1; if( _temp210
== ( void*) Cyc_Absyn_Signed){ goto _LL209;} else{ goto _LL120;} _LL209:
_temp208=(( struct Cyc_Absyn_Int_c_struct*) _temp206)->f2; if( _temp208 ==  0){
goto _LL119;} else{ goto _LL120;}} else{ goto _LL120;}} else{ goto _LL120;}
_LL120: if(*(( int*) _temp112) ==  Cyc_Absyn_Const_e){ _LL213: _temp212=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp112)->f1; if(( unsigned int) _temp212 > 
1u?*(( int*) _temp212) ==  Cyc_Absyn_Int_c: 0){ _LL217: _temp216=( void*)((
struct Cyc_Absyn_Int_c_struct*) _temp212)->f1; if( _temp216 == ( void*) Cyc_Absyn_Signed){
goto _LL215;} else{ goto _LL122;} _LL215: _temp214=(( struct Cyc_Absyn_Int_c_struct*)
_temp212)->f2; if( _temp214 ==  1){ goto _LL121;} else{ goto _LL122;}} else{
goto _LL122;}} else{ goto _LL122;} _LL122: if(*(( int*) _temp112) ==  Cyc_Absyn_Sizeofexp_e){
goto _LL123;} else{ goto _LL124;} _LL124: if(*(( int*) _temp112) ==  Cyc_Absyn_Sizeoftyp_e){
goto _LL125;} else{ goto _LL126;} _LL126: if(*(( int*) _temp112) ==  Cyc_Absyn_Offsetof_e){
goto _LL127;} else{ goto _LL128;} _LL128: if(*(( int*) _temp112) ==  Cyc_Absyn_Const_e){
goto _LL129;} else{ goto _LL130;} _LL130: if(*(( int*) _temp112) ==  Cyc_Absyn_Enum_e){
goto _LL131;} else{ goto _LL132;} _LL132: if(*(( int*) _temp112) ==  Cyc_Absyn_Var_e){
_LL219: _temp218=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp112)->f2; if((
unsigned int) _temp218 >  1u?*(( int*) _temp218) ==  Cyc_Absyn_Funname_b: 0){
goto _LL133;} else{ goto _LL134;}} else{ goto _LL134;} _LL134: if(*(( int*)
_temp112) ==  Cyc_Absyn_Var_e){ _LL221: _temp220=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp112)->f2; if(( unsigned int) _temp220 >  1u?*(( int*) _temp220) ==  Cyc_Absyn_Global_b:
0){ goto _LL135;} else{ goto _LL136;}} else{ goto _LL136;} _LL136: if(*(( int*)
_temp112) ==  Cyc_Absyn_Var_e){ _LL223: _temp222=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp112)->f2; if(( unsigned int) _temp222 >  1u?*(( int*) _temp222) ==  Cyc_Absyn_Param_b:
0){ _LL225: _temp224=(( struct Cyc_Absyn_Param_b_struct*) _temp222)->f1; goto
_LL137;} else{ goto _LL138;}} else{ goto _LL138;} _LL138: if(*(( int*) _temp112)
==  Cyc_Absyn_Var_e){ _LL227: _temp226=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp112)->f2; if(( unsigned int) _temp226 >  1u?*(( int*) _temp226) ==  Cyc_Absyn_Local_b:
0){ _LL229: _temp228=(( struct Cyc_Absyn_Local_b_struct*) _temp226)->f1; goto
_LL139;} else{ goto _LL140;}} else{ goto _LL140;} _LL140: if(*(( int*) _temp112)
==  Cyc_Absyn_Var_e){ _LL231: _temp230=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp112)->f2; if(( unsigned int) _temp230 >  1u?*(( int*) _temp230) ==  Cyc_Absyn_Pat_b:
0){ _LL233: _temp232=(( struct Cyc_Absyn_Pat_b_struct*) _temp230)->f1; goto
_LL141;} else{ goto _LL142;}} else{ goto _LL142;} _LL142: if(*(( int*) _temp112)
==  Cyc_Absyn_AssignOp_e){ _LL239: _temp238=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp112)->f1; goto _LL237; _LL237: _temp236=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp112)->f2; if( _temp236 ==  0){ goto _LL235;} else{ goto _LL144;} _LL235:
_temp234=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp112)->f3; goto _LL143;}
else{ goto _LL144;} _LL144: if(*(( int*) _temp112) ==  Cyc_Absyn_AssignOp_e){
_LL243: _temp242=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp112)->f1; goto
_LL241; _LL241: _temp240=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp112)->f3;
goto _LL145;} else{ goto _LL146;} _LL146: if(*(( int*) _temp112) ==  Cyc_Absyn_Primop_e){
_LL245: _temp244=(( struct Cyc_Absyn_Primop_e_struct*) _temp112)->f2; goto
_LL147;} else{ goto _LL148;} _LL148: if(*(( int*) _temp112) ==  Cyc_Absyn_Cast_e){
_LL247: _temp246=(( struct Cyc_Absyn_Cast_e_struct*) _temp112)->f2; goto _LL149;}
else{ goto _LL150;} _LL150: if(*(( int*) _temp112) ==  Cyc_Absyn_FnCall_e){
_LL251: _temp250=(( struct Cyc_Absyn_FnCall_e_struct*) _temp112)->f1; goto
_LL249; _LL249: _temp248=(( struct Cyc_Absyn_FnCall_e_struct*) _temp112)->f2;
goto _LL151;} else{ goto _LL152;} _LL152: if(*(( int*) _temp112) ==  Cyc_Absyn_Subscript_e){
_LL255: _temp254=(( struct Cyc_Absyn_Subscript_e_struct*) _temp112)->f1; goto
_LL253; _LL253: _temp252=(( struct Cyc_Absyn_Subscript_e_struct*) _temp112)->f2;
goto _LL153;} else{ goto _LL154;} _LL154: if(*(( int*) _temp112) ==  Cyc_Absyn_Address_e){
_LL257: _temp256=(( struct Cyc_Absyn_Address_e_struct*) _temp112)->f1; goto
_LL155;} else{ goto _LL156;} _LL156: if(*(( int*) _temp112) ==  Cyc_Absyn_New_e){
_LL261: _temp260=(( struct Cyc_Absyn_New_e_struct*) _temp112)->f1; goto _LL259;
_LL259: _temp258=(( struct Cyc_Absyn_New_e_struct*) _temp112)->f2; goto _LL157;}
else{ goto _LL158;} _LL158: if(*(( int*) _temp112) ==  Cyc_Absyn_Malloc_e){
_LL265: _temp264=(( struct Cyc_Absyn_Malloc_e_struct*) _temp112)->f1; goto
_LL263; _LL263: _temp262=( void*)(( struct Cyc_Absyn_Malloc_e_struct*) _temp112)->f2;
goto _LL159;} else{ goto _LL160;} _LL160: if(*(( int*) _temp112) ==  Cyc_Absyn_Deref_e){
_LL267: _temp266=(( struct Cyc_Absyn_Deref_e_struct*) _temp112)->f1; goto _LL161;}
else{ goto _LL162;} _LL162: if(*(( int*) _temp112) ==  Cyc_Absyn_StructArrow_e){
_LL269: _temp268=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp112)->f1; goto
_LL163;} else{ goto _LL164;} _LL164: if(*(( int*) _temp112) ==  Cyc_Absyn_StructMember_e){
_LL271: _temp270=(( struct Cyc_Absyn_StructMember_e_struct*) _temp112)->f1; goto
_LL165;} else{ goto _LL166;} _LL166: if(*(( int*) _temp112) ==  Cyc_Absyn_Tunion_e){
_LL273: _temp272=(( struct Cyc_Absyn_Tunion_e_struct*) _temp112)->f3; goto
_LL167;} else{ goto _LL168;} _LL168: if(*(( int*) _temp112) ==  Cyc_Absyn_Tuple_e){
_LL275: _temp274=(( struct Cyc_Absyn_Tuple_e_struct*) _temp112)->f1; goto _LL169;}
else{ goto _LL170;} _LL170: if(*(( int*) _temp112) ==  Cyc_Absyn_AnonStruct_e){
_LL277: _temp276=(( struct Cyc_Absyn_AnonStruct_e_struct*) _temp112)->f2; goto
_LL171;} else{ goto _LL172;} _LL172: if(*(( int*) _temp112) ==  Cyc_Absyn_Struct_e){
_LL279: _temp278=(( struct Cyc_Absyn_Struct_e_struct*) _temp112)->f3; goto
_LL173;} else{ goto _LL174;} _LL174: if(*(( int*) _temp112) ==  Cyc_Absyn_Array_e){
_LL281: _temp280=(( struct Cyc_Absyn_Array_e_struct*) _temp112)->f1; goto _LL175;}
else{ goto _LL176;} _LL176: if(*(( int*) _temp112) ==  Cyc_Absyn_Increment_e){
_LL283: _temp282=(( struct Cyc_Absyn_Increment_e_struct*) _temp112)->f1; goto
_LL177;} else{ goto _LL178;} _LL178: if(*(( int*) _temp112) ==  Cyc_Absyn_Throw_e){
_LL285: _temp284=(( struct Cyc_Absyn_Throw_e_struct*) _temp112)->f1; goto _LL179;}
else{ goto _LL180;} _LL180: if(*(( int*) _temp112) ==  Cyc_Absyn_Conditional_e){
_LL291: _temp290=(( struct Cyc_Absyn_Conditional_e_struct*) _temp112)->f1; goto
_LL289; _LL289: _temp288=(( struct Cyc_Absyn_Conditional_e_struct*) _temp112)->f2;
goto _LL287; _LL287: _temp286=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp112)->f3; goto _LL181;} else{ goto _LL182;} _LL182: if(*(( int*) _temp112)
==  Cyc_Absyn_StmtExp_e){ _LL293: _temp292=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp112)->f1; goto _LL183;} else{ goto _LL184;} _LL184: if(*(( int*) _temp112)
==  Cyc_Absyn_SeqExp_e){ _LL297: _temp296=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp112)->f1; goto _LL295; _LL295: _temp294=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp112)->f2; goto _LL185;} else{ goto _LL186;} _LL186: if(*(( int*) _temp112)
==  Cyc_Absyn_Comprehension_e){ _LL303: _temp302=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp112)->f1; goto _LL301; _LL301: _temp300=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp112)->f2; goto _LL299; _LL299: _temp298=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp112)->f3; goto _LL187;} else{ goto _LL188;} _LL188: if(*(( int*) _temp112)
==  Cyc_Absyn_Var_e){ _LL305: _temp304=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp112)->f2; if( _temp304 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL189;}
else{ goto _LL190;}} else{ goto _LL190;} _LL190: if(*(( int*) _temp112) ==  Cyc_Absyn_UnknownId_e){
goto _LL191;} else{ goto _LL192;} _LL192: if(*(( int*) _temp112) ==  Cyc_Absyn_UnknownCall_e){
goto _LL193;} else{ goto _LL194;} _LL194: if(*(( int*) _temp112) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL195;} else{ goto _LL196;} _LL196: if(*(( int*) _temp112) ==  Cyc_Absyn_CompoundLit_e){
goto _LL197;} else{ goto _LL198;} _LL198: if(*(( int*) _temp112) ==  Cyc_Absyn_Codegen_e){
goto _LL199;} else{ goto _LL200;} _LL200: if(*(( int*) _temp112) ==  Cyc_Absyn_Fill_e){
goto _LL201;} else{ goto _LL113;} _LL115: _temp204= _temp202; goto _LL117;
_LL117: return Cyc_NewControlFlow_abstract_exp( env, _temp204); _LL119: env.access_path
==  0? 0:(((( int(*)( struct _tagged_arr assertion, struct _tagged_arr file,
unsigned int line)) Cyc_Std___assert_fail)( _tag_arr("env.access_path==NULL",
sizeof( unsigned char), 22u), _tag_arr("new_control_flow.cyc", sizeof(
unsigned char), 21u), 301), 0)); return({ struct Cyc_NewControlFlow_AbsSyn
_temp306; _temp306.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp308;
_temp308.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp308.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp308;}); _temp306.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp307; _temp307.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE(); _temp307.assigns=(
void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp307;}); _temp306.lvalues= 0; _temp306;}); _LL121: env.access_path ==  0? 0:((((
int(*)( struct _tagged_arr assertion, struct _tagged_arr file, unsigned int line))
Cyc_Std___assert_fail)( _tag_arr("env.access_path==NULL", sizeof( unsigned char),
22u), _tag_arr("new_control_flow.cyc", sizeof( unsigned char), 21u), 306), 0));
return({ struct Cyc_NewControlFlow_AbsSyn _temp309; _temp309.when_true=({ struct
Cyc_NewControlFlow_AbsSynOne _temp311; _temp311.inner_exp=( void*) Cyc_CfAbsexp_mkSkipAE();
_temp311.assigns=( void*) Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp());
_temp311;}); _temp309.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp310;
_temp310.inner_exp=( void*) Cyc_CfAbsexp_mkBottomAE(); _temp310.assigns=( void*)
Cyc_CfAbsexp_mkBottomAE(); _temp310;}); _temp309.lvalues= 0; _temp309;}); _LL123:
goto _LL125; _LL125: goto _LL127; _LL127: goto _LL129; _LL129: goto _LL131;
_LL131: goto _LL133; _LL133: return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),
0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL135:
return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(),
Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp())); _LL137:
_temp228= _temp224; goto _LL139; _LL139: _temp232= _temp228; goto _LL141; _LL141: {
void* absop= Cyc_NewControlFlow_destruct_path( env, Cyc_CfAbsexp_mkLocalOp(
_temp232)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkSkipAE(),({
struct Cyc_List_List* _temp312=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp312->hd=( void*) absop; _temp312->tl= 0; _temp312;}),
Cyc_NewControlFlow_make_assigns( env, absop));} _LL143: { struct Cyc_NewControlFlow_AbsSyn
_temp313= Cyc_NewControlFlow_abstract_exp( env, _temp238); struct Cyc_List_List*
_temp314= _temp313.lvalues ==  0? Cyc_NewControlFlow_use_it_list(): _temp313.lvalues;
struct Cyc_NewControlFlow_AbsSyn _temp315= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs(
env, _temp314), _temp234); void* _temp318; void* _temp320; struct Cyc_NewControlFlow_AbsSynOne
_temp316= Cyc_NewControlFlow_meet_absexp( _temp313); _LL321: _temp320=( void*)
_temp316.inner_exp; goto _LL319; _LL319: _temp318=( void*) _temp316.assigns;
goto _LL317; _LL317: { void* _temp324; void* _temp326; struct Cyc_NewControlFlow_AbsSynOne
_temp322= Cyc_NewControlFlow_meet_absexp( _temp315); _LL327: _temp326=( void*)
_temp322.inner_exp; goto _LL325; _LL325: _temp324=( void*) _temp322.assigns;
goto _LL323; _LL323: { void* _temp328= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp320, _temp326),
_temp324); return Cyc_NewControlFlow_mkSyn_tf( _temp328, 0, _temp318);}}} _LL145:
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp329=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp329->hd=( void*) _temp242; _temp329->tl=({ struct Cyc_List_List* _temp330=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp330->hd=(
void*) _temp240; _temp330->tl= 0; _temp330;}); _temp329;}), 0); _LL147: return
Cyc_NewControlFlow_use_exp_unordered( env, _temp244, 0); _LL149: { int both_leaf=
1;{ void* _temp331=( void*)(( struct Cyc_Core_Opt*) _check_null( _temp246->topt))->v;
_LL333: if(( unsigned int) _temp331 >  4u?*(( int*) _temp331) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL334;} else{ goto _LL335;} _LL335: if(( unsigned int) _temp331 >  4u?*((
int*) _temp331) ==  Cyc_Absyn_StructType: 0){ goto _LL336;} else{ goto _LL337;}
_LL337: if(( unsigned int) _temp331 >  4u?*(( int*) _temp331) ==  Cyc_Absyn_TupleType:
0){ goto _LL338;} else{ goto _LL339;} _LL339: goto _LL340; _LL334: goto _LL336;
_LL336: goto _LL338; _LL338: both_leaf= 0; goto _LL332; _LL340: goto _LL332;
_LL332:;}{ void* _temp341=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
_LL343: if(( unsigned int) _temp341 >  4u?*(( int*) _temp341) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL344;} else{ goto _LL345;} _LL345: if(( unsigned int) _temp341 >  4u?*((
int*) _temp341) ==  Cyc_Absyn_StructType: 0){ goto _LL346;} else{ goto _LL347;}
_LL347: if(( unsigned int) _temp341 >  4u?*(( int*) _temp341) ==  Cyc_Absyn_TupleType:
0){ goto _LL348;} else{ goto _LL349;} _LL349: goto _LL350; _LL344: goto _LL346;
_LL346: goto _LL348; _LL348: both_leaf= 0; goto _LL342; _LL350: goto _LL342;
_LL342:;} if( both_leaf){ return Cyc_NewControlFlow_abstract_exp( env, _temp246);}
return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp351=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp351->hd=( void*) _temp246; _temp351->tl= 0; _temp351;}), env.lhsides);}
_LL151: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp352=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp352->hd=( void*) _temp250; _temp352->tl= _temp248; _temp352;}), 0); _LL153: {
void* _temp353= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp254->topt))->v); _LL355: if(( unsigned int) _temp353 >  4u?*(( int*)
_temp353) ==  Cyc_Absyn_TupleType: 0){ goto _LL356;} else{ goto _LL357;} _LL357:
goto _LL358; _LL356: return Cyc_NewControlFlow_descend_path( env, e, _temp254);
_LL358: return Cyc_NewControlFlow_use_exp_unordered( env,({ struct Cyc_List_List*
_temp359=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp359->hd=( void*) _temp254; _temp359->tl=({ struct Cyc_List_List* _temp360=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp360->hd=(
void*) _temp252; _temp360->tl= 0; _temp360;}); _temp359;}), Cyc_NewControlFlow_use_it_list());
_LL354:;} _LL155: { int is_malloc= 0;{ void* _temp361=( void*) _temp256->r;
_LL363: if(*(( int*) _temp361) ==  Cyc_Absyn_Struct_e){ goto _LL364;} else{ goto
_LL365;} _LL365: if(*(( int*) _temp361) ==  Cyc_Absyn_Tuple_e){ goto _LL366;}
else{ goto _LL367;} _LL367: goto _LL368; _LL364: goto _LL366; _LL366: is_malloc=
1; goto _LL362; _LL368: goto _LL362; _LL362:;} if( is_malloc){ _temp260= 0;
_temp258= _temp256; goto _LL157;}{ struct Cyc_List_List* _temp369= env.access_path;
struct Cyc_List_List _temp381; struct Cyc_List_List* _temp382; struct Cyc_Absyn_Exp*
_temp384; struct Cyc_Absyn_Exp _temp386; void* _temp387; struct Cyc_Absyn_Exp*
_temp389; struct Cyc_List_List _temp391; struct Cyc_List_List* _temp392; struct
Cyc_Absyn_Exp* _temp394; struct Cyc_Absyn_Exp _temp396; void* _temp397; struct
Cyc_List_List _temp399; struct Cyc_List_List* _temp400; struct Cyc_Absyn_Exp*
_temp402; struct Cyc_Absyn_Exp _temp404; void* _temp405; struct _tagged_arr*
_temp407; struct Cyc_Absyn_Exp* _temp409; _LL371: if( _temp369 ==  0){ goto
_LL372;} else{ goto _LL373;} _LL373: if( _temp369 ==  0){ goto _LL375;} else{
_temp381=* _temp369; _LL385: _temp384=( struct Cyc_Absyn_Exp*) _temp381.hd;
_temp386=* _temp384; _LL388: _temp387=( void*) _temp386.r; if(*(( int*) _temp387)
==  Cyc_Absyn_Subscript_e){ _LL390: _temp389=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp387)->f2; goto _LL383;} else{ goto _LL375;} _LL383: _temp382= _temp381.tl;
goto _LL374;} _LL375: if( _temp369 ==  0){ goto _LL377;} else{ _temp391=*
_temp369; _LL395: _temp394=( struct Cyc_Absyn_Exp*) _temp391.hd; _temp396=*
_temp394; _LL398: _temp397=( void*) _temp396.r; if(*(( int*) _temp397) ==  Cyc_Absyn_Deref_e){
goto _LL393;} else{ goto _LL377;} _LL393: _temp392= _temp391.tl; goto _LL376;}
_LL377: if( _temp369 ==  0){ goto _LL379;} else{ _temp399=* _temp369; _LL403:
_temp402=( struct Cyc_Absyn_Exp*) _temp399.hd; _temp404=* _temp402; _LL406:
_temp405=( void*) _temp404.r; if(*(( int*) _temp405) ==  Cyc_Absyn_StructArrow_e){
_LL410: _temp409=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp405)->f1; goto
_LL408; _LL408: _temp407=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp405)->f2;
goto _LL401;} else{ goto _LL379;} _LL401: _temp400= _temp399.tl; goto _LL378;}
_LL379: goto _LL380; _LL372: env.access_path=({ struct Cyc_List_List* _temp411=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp411->hd=(
void*) e; _temp411->tl= env.access_path; _temp411;}); goto _LL370; _LL374: Cyc_Evexp_eval_const_uint_exp(
_temp389) ==  0? 0:(((( int(*)( struct _tagged_arr assertion, struct _tagged_arr
file, unsigned int line)) Cyc_Std___assert_fail)( _tag_arr("Evexp::eval_const_uint_exp(e2) == 0",
sizeof( unsigned char), 36u), _tag_arr("new_control_flow.cyc", sizeof(
unsigned char), 21u), 430), 0)); _temp392= _temp382; goto _LL376; _LL376: env.access_path=
_temp392; goto _LL370; _LL378: env.access_path=({ struct Cyc_List_List* _temp412=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp412->hd=(
void*) Cyc_Absyn_structmember_exp( _temp409, _temp407, 0); _temp412->tl=
_temp400; _temp412;}); goto _LL370; _LL380:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp413=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp413[ 0]=({ struct Cyc_Core_Impossible_struct _temp414; _temp414.tag= Cyc_Core_Impossible;
_temp414.f1= _tag_arr("abstract_exp: bad access path for &", sizeof(
unsigned char), 36u); _temp414;}); _temp413;})); _LL370:;} return Cyc_NewControlFlow_abstract_exp(
env, _temp256);} _LL157: Cyc_NewControlFlow_add_malloc_root( env, e,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp258->topt))->v); return Cyc_NewControlFlow_abstract_malloc(
env, e, _temp260,( struct Cyc_Absyn_Exp*) _temp258); _LL159: Cyc_NewControlFlow_add_malloc_root(
env, e, _temp262); return Cyc_NewControlFlow_abstract_malloc( env, e, _temp264,
0); _LL161: if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp266->topt))->v)){ return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp415=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp415->hd=( void*) _temp266; _temp415->tl= 0;
_temp415;}), Cyc_NewControlFlow_use_it_list());}{ struct Cyc_List_List* _temp416=
env.access_path; struct Cyc_List_List _temp422; struct Cyc_List_List* _temp423;
struct Cyc_Absyn_Exp* _temp425; struct Cyc_Absyn_Exp _temp427; void* _temp428;
_LL418: if( _temp416 ==  0){ goto _LL420;} else{ _temp422=* _temp416; _LL426:
_temp425=( struct Cyc_Absyn_Exp*) _temp422.hd; _temp427=* _temp425; _LL429:
_temp428=( void*) _temp427.r; if(*(( int*) _temp428) ==  Cyc_Absyn_Address_e){
goto _LL424;} else{ goto _LL420;} _LL424: _temp423= _temp422.tl; goto _LL419;}
_LL420: goto _LL421; _LL419: env.access_path= _temp423;{ struct Cyc_NewControlFlow_AbsSyn
_temp430= Cyc_NewControlFlow_abstract_exp( env, _temp266); if( _temp430.lvalues
==  0){ _temp430.lvalues= Cyc_NewControlFlow_use_it_list();} return _temp430;}
_LL421: return Cyc_NewControlFlow_descend_path( env, e, _temp266); _LL417:;}
_LL163: if( Cyc_Tcutil_is_tagged_pointer_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp268->topt))->v)){ return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp431=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp431->hd=( void*) _temp268; _temp431->tl= 0;
_temp431;}), Cyc_NewControlFlow_use_it_list());}{ void* _temp432= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp268->topt))->v); struct Cyc_Absyn_PtrInfo
_temp438; void* _temp440; _LL434: if(( unsigned int) _temp432 >  4u?*(( int*)
_temp432) ==  Cyc_Absyn_PointerType: 0){ _LL439: _temp438=(( struct Cyc_Absyn_PointerType_struct*)
_temp432)->f1; _LL441: _temp440=( void*) _temp438.elt_typ; goto _LL435;} else{
goto _LL436;} _LL436: goto _LL437; _LL435:{ void* _temp442= Cyc_Tcutil_compress(
_temp440); _LL444: if(( unsigned int) _temp442 >  4u?*(( int*) _temp442) ==  Cyc_Absyn_AnonStructType:
0){ goto _LL445;} else{ goto _LL446;} _LL446: if(( unsigned int) _temp442 >  4u?*((
int*) _temp442) ==  Cyc_Absyn_StructType: 0){ goto _LL447;} else{ goto _LL448;}
_LL448: if(( unsigned int) _temp442 >  4u?*(( int*) _temp442) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL449;} else{ goto _LL450;} _LL450: if(( unsigned int) _temp442 >  4u?*((
int*) _temp442) ==  Cyc_Absyn_UnionType: 0){ goto _LL451;} else{ goto _LL452;}
_LL452: goto _LL453; _LL445: goto _LL447; _LL447: return Cyc_NewControlFlow_descend_path(
env, e, _temp268); _LL449: goto _LL451; _LL451: { struct Cyc_NewControlFlow_AbsSyn
_temp454= Cyc_NewControlFlow_abstract_exp( env, _temp268); _temp454.lvalues= Cyc_NewControlFlow_use_it_list();
return _temp454;} _LL453: goto _LL443; _LL443:;} goto _LL437; _LL437:( int)
_throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp455=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp455[ 0]=({ struct
Cyc_Core_Impossible_struct _temp456; _temp456.tag= Cyc_Core_Impossible; _temp456.f1=
_tag_arr("NewControlFlow: bad type in StructArrow", sizeof( unsigned char), 40u);
_temp456;}); _temp455;})); _LL433:;} _LL165: { void* _temp457= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp270->topt))->v); _LL459: if((
unsigned int) _temp457 >  4u?*(( int*) _temp457) ==  Cyc_Absyn_AnonStructType: 0){
goto _LL460;} else{ goto _LL461;} _LL461: if(( unsigned int) _temp457 >  4u?*((
int*) _temp457) ==  Cyc_Absyn_StructType: 0){ goto _LL462;} else{ goto _LL463;}
_LL463: if(( unsigned int) _temp457 >  4u?*(( int*) _temp457) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL464;} else{ goto _LL465;} _LL465: if(( unsigned int) _temp457 >  4u?*((
int*) _temp457) ==  Cyc_Absyn_UnionType: 0){ goto _LL466;} else{ goto _LL467;}
_LL467: goto _LL468; _LL460: goto _LL462; _LL462: return Cyc_NewControlFlow_descend_path(
env, e, _temp270); _LL464: goto _LL466; _LL466: { struct Cyc_NewControlFlow_AbsSyn
_temp469= Cyc_NewControlFlow_abstract_exp( env, _temp270); _temp469.lvalues= Cyc_NewControlFlow_use_it_list();
return _temp469;} _LL468:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp470=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp470[ 0]=({ struct Cyc_Core_Impossible_struct _temp471; _temp471.tag= Cyc_Core_Impossible;
_temp471.f1= _tag_arr("NewControlFlow: bad type in StructMember", sizeof(
unsigned char), 41u); _temp471;}); _temp470;})); _LL458:;} _LL167: env.access_path
==  0? 0:(((( int(*)( struct _tagged_arr assertion, struct _tagged_arr file,
unsigned int line)) Cyc_Std___assert_fail)( _tag_arr("env.access_path == NULL",
sizeof( unsigned char), 24u), _tag_arr("new_control_flow.cyc", sizeof(
unsigned char), 21u), 504), 0)); if( _temp272 ==  0){ return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkSkipAE(), Cyc_NewControlFlow_use_it_list(), Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));} _temp274= _temp272; goto _LL169; _LL169: {
struct Cyc_List_List* _temp472= env.access_path; struct Cyc_List_List _temp480;
struct Cyc_List_List* _temp481; struct Cyc_Absyn_Exp* _temp483; struct Cyc_Absyn_Exp
_temp485; void* _temp486; struct Cyc_Absyn_Exp* _temp488; _LL474: if( _temp472
==  0){ goto _LL475;} else{ goto _LL476;} _LL476: if( _temp472 ==  0){ goto
_LL478;} else{ _temp480=* _temp472; _LL484: _temp483=( struct Cyc_Absyn_Exp*)
_temp480.hd; _temp485=* _temp483; _LL487: _temp486=( void*) _temp485.r; if(*((
int*) _temp486) ==  Cyc_Absyn_Subscript_e){ _LL489: _temp488=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp486)->f2; goto _LL482;} else{ goto _LL478;} _LL482: _temp481= _temp480.tl;
goto _LL477;} _LL478: goto _LL479; _LL475: { void* _temp490= Cyc_CfAbsexp_mkSkipAE();
void* _temp491= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp274 !=  0;(
_temp274=(( struct Cyc_List_List*) _check_null( _temp274))->tl, ++ j)){ struct
Cyc_List_List* _temp492= 0;{ struct Cyc_List_List* ls= env.lhsides; for( 0; ls
!=  0; ls=(( struct Cyc_List_List*) _check_null( ls))->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd)){ _temp492=({ struct Cyc_List_List*
_temp493=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp493->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp493->tl= _temp492;
_temp493;});} else{ _temp492=({ struct Cyc_List_List* _temp494=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp494->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd, Cyc_Absyn_fieldname( j));
_temp494->tl= _temp492; _temp494;});}}}{ struct Cyc_NewControlFlow_AbsSyn
_temp495= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp492),( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp274))->hd); void* _temp498; void* _temp500; struct Cyc_NewControlFlow_AbsSynOne
_temp496= Cyc_NewControlFlow_meet_absexp( _temp495); _LL501: _temp500=( void*)
_temp496.inner_exp; goto _LL499; _LL499: _temp498=( void*) _temp496.assigns;
goto _LL497; _LL497: _temp490= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp490, _temp500); _temp491= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp491, _temp498);}}} return Cyc_NewControlFlow_mkSyn_tf( _temp490, 0,
_temp491);} _LL477: { int i=( int) Cyc_Evexp_eval_const_uint_exp( _temp488);
struct Cyc_NewControlFlow_AbsEnv _temp502= Cyc_NewControlFlow_change_lhs( env, 0);
_temp502.access_path= 0; env.access_path= _temp481;{ void* _temp503= Cyc_CfAbsexp_mkSkipAE();
void* _temp504= Cyc_CfAbsexp_mkSkipAE();{ int j= 0; for( 0; _temp274 !=  0;(
_temp274=(( struct Cyc_List_List*) _check_null( _temp274))->tl, ++ j)){ if( i == 
j){ void* _temp507; void* _temp509; struct Cyc_NewControlFlow_AbsSynOne _temp505=
Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp( env,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp274))->hd)); _LL510:
_temp509=( void*) _temp505.inner_exp; goto _LL508; _LL508: _temp507=( void*)
_temp505.assigns; goto _LL506; _LL506: _temp504= _temp507; _temp503= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp503, _temp509);} else{ void* _temp513; void*
_temp515; struct Cyc_NewControlFlow_AbsSynOne _temp511= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp502,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp274))->hd)); _LL516: _temp515=( void*) _temp511.inner_exp;
goto _LL514; _LL514: _temp513=( void*) _temp511.assigns; goto _LL512; _LL512:
_temp503= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp503,
_temp515);}}} return Cyc_NewControlFlow_mkSyn_tf( _temp503,({ struct Cyc_List_List*
_temp517=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp517->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp517->tl= 0; _temp517;}),
_temp504);}} _LL479:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp518=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp518[ 0]=({ struct Cyc_Core_Impossible_struct _temp519; _temp519.tag= Cyc_Core_Impossible;
_temp519.f1= _tag_arr("abstract_exp: unexpected access path to Tuple", sizeof(
unsigned char), 46u); _temp519;}); _temp518;})); _LL473:;} _LL171: _temp278=
_temp276; goto _LL173; _LL173: { struct Cyc_List_List* _temp520= env.access_path;
struct Cyc_List_List _temp528; struct Cyc_List_List* _temp529; struct Cyc_Absyn_Exp*
_temp531; struct Cyc_Absyn_Exp _temp533; void* _temp534; struct _tagged_arr*
_temp536; _LL522: if( _temp520 ==  0){ goto _LL523;} else{ goto _LL524;} _LL524:
if( _temp520 ==  0){ goto _LL526;} else{ _temp528=* _temp520; _LL532: _temp531=(
struct Cyc_Absyn_Exp*) _temp528.hd; _temp533=* _temp531; _LL535: _temp534=( void*)
_temp533.r; if(*(( int*) _temp534) ==  Cyc_Absyn_StructMember_e){ _LL537:
_temp536=(( struct Cyc_Absyn_StructMember_e_struct*) _temp534)->f2; goto _LL530;}
else{ goto _LL526;} _LL530: _temp529= _temp528.tl; goto _LL525;} _LL526: goto
_LL527; _LL523: { void* _temp538= Cyc_CfAbsexp_mkSkipAE(); void* _temp539= Cyc_CfAbsexp_mkSkipAE();
for( 0; _temp278 !=  0; _temp278=(( struct Cyc_List_List*) _check_null( _temp278))->tl){
struct Cyc_List_List* _temp540= 0;{ struct Cyc_List_List* ls= env.lhsides; for(
0; ls !=  0; ls=(( struct Cyc_List_List*) _check_null( ls))->tl){ if( Cyc_CfAbsexp_isUnknownOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd)){ _temp540=({ struct Cyc_List_List*
_temp541=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp541->hd=( void*) Cyc_CfAbsexp_mkUnknownOp(); _temp541->tl= _temp540;
_temp541;});} else{ struct Cyc_List_List* ds=(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp278))->hd)).f1; for( 0; ds !=  0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp542=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct _tagged_arr* _temp548; _LL544: if(*(( int*)
_temp542) ==  Cyc_Absyn_ArrayElement){ goto _LL545;} else{ goto _LL546;} _LL546:
if(*(( int*) _temp542) ==  Cyc_Absyn_FieldName){ _LL549: _temp548=(( struct Cyc_Absyn_FieldName_struct*)
_temp542)->f1; goto _LL547;} else{ goto _LL543;} _LL545:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp550=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp550[ 0]=({ struct
Cyc_Core_Impossible_struct _temp551; _temp551.tag= Cyc_Core_Impossible; _temp551.f1=
_tag_arr("bad struct designator", sizeof( unsigned char), 22u); _temp551;});
_temp550;})); _LL547: _temp540=({ struct Cyc_List_List* _temp552=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp552->hd=( void*) Cyc_CfAbsexp_mkMemberOp((
void*)(( struct Cyc_List_List*) _check_null( ls))->hd, _temp548); _temp552->tl=
_temp540; _temp552;}); goto _LL543; _LL543:;}}}}{ void** _temp553=( void**)((
void*)&(( struct Cyc_Core_Opt*) _check_null(((*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp278))->hd)).f2)->topt))->v); struct Cyc_NewControlFlow_AbsSyn
_temp554= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_change_lhs( env,
_temp540),(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp278))->hd)).f2);
void* _temp557; void* _temp559; struct Cyc_NewControlFlow_AbsSynOne _temp555=
Cyc_NewControlFlow_meet_absexp( _temp554); _LL560: _temp559=( void*) _temp555.inner_exp;
goto _LL558; _LL558: _temp557=( void*) _temp555.assigns; goto _LL556; _LL556:
_temp538= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp538,
_temp559); _temp539= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG,
_temp539, _temp557);}} return Cyc_NewControlFlow_mkSyn_tf( _temp538, 0, _temp539);}
_LL525: { struct Cyc_NewControlFlow_AbsEnv _temp561= Cyc_NewControlFlow_change_lhs(
env, 0); _temp561.access_path= 0; env.access_path= _temp529;{ void* _temp562=
Cyc_CfAbsexp_mkSkipAE(); void* _temp563= Cyc_CfAbsexp_mkSkipAE(); for( 0;
_temp278 !=  0; _temp278=(( struct Cyc_List_List*) _check_null( _temp278))->tl){
int used= 0;{ struct Cyc_List_List* ds=(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp278))->hd)).f1; for( 0; ds !=  0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp564=( void*)(( struct Cyc_List_List*)
_check_null( ds))->hd; struct _tagged_arr* _temp570; _LL566: if(*(( int*)
_temp564) ==  Cyc_Absyn_ArrayElement){ goto _LL567;} else{ goto _LL568;} _LL568:
if(*(( int*) _temp564) ==  Cyc_Absyn_FieldName){ _LL571: _temp570=(( struct Cyc_Absyn_FieldName_struct*)
_temp564)->f1; goto _LL569;} else{ goto _LL565;} _LL567:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp572=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp572[ 0]=({ struct
Cyc_Core_Impossible_struct _temp573; _temp573.tag= Cyc_Core_Impossible; _temp573.f1=
_tag_arr("bad struct designator", sizeof( unsigned char), 22u); _temp573;});
_temp572;})); _LL569: if( Cyc_Std_zstrptrcmp( _temp536, _temp570) ==  0){ used=
1;} goto _LL565; _LL565:;}} if( used){ void* _temp576; void* _temp578; struct
Cyc_NewControlFlow_AbsSynOne _temp574= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
env,(*(( struct _tuple4*)(( struct Cyc_List_List*) _check_null( _temp278))->hd)).f2));
_LL579: _temp578=( void*) _temp574.inner_exp; goto _LL577; _LL577: _temp576=(
void*) _temp574.assigns; goto _LL575; _LL575: _temp563= _temp576; _temp562= Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_UnorderedG, _temp562, _temp578);} else{ void* _temp582; void*
_temp584; struct Cyc_NewControlFlow_AbsSynOne _temp580= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp561,(*(( struct _tuple4*)(( struct Cyc_List_List*)
_check_null( _temp278))->hd)).f2)); _LL585: _temp584=( void*) _temp580.inner_exp;
goto _LL583; _LL583: _temp582=( void*) _temp580.assigns; goto _LL581; _LL581:
_temp562= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_UnorderedG, _temp562,
_temp584);}} return Cyc_NewControlFlow_mkSyn_tf( _temp562, 0, _temp563);}}
_LL527:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp586=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp586[ 0]=({ struct Cyc_Core_Impossible_struct _temp587; _temp587.tag= Cyc_Core_Impossible;
_temp587.f1= _tag_arr("abstract_exp: unexpected access path to Struct", sizeof(
unsigned char), 47u); _temp587;}); _temp586;})); _LL521:;} _LL175: return Cyc_NewControlFlow_use_exp_unordered(
env,(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple4*),
struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct
_tuple4*)) Cyc_Core_snd, _temp280), 0); _LL177: return Cyc_NewControlFlow_use_exp_unordered(
env,({ struct Cyc_List_List* _temp588=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp588->hd=( void*) _temp282; _temp588->tl= 0;
_temp588;}), 0); _LL179: { struct Cyc_NewControlFlow_AbsSyn _temp589= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp284); return Cyc_NewControlFlow_mkSyn_tf(
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( Cyc_NewControlFlow_meet_absexp(
_temp589)).inner_exp, Cyc_CfAbsexp_mkBottomAE()), 0, Cyc_CfAbsexp_mkSkipAE());}
_LL181: { struct Cyc_NewControlFlow_AbsSynOne _temp592; struct Cyc_NewControlFlow_AbsSynOne
_temp594; struct Cyc_NewControlFlow_AbsSyn _temp590= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp290); _LL595: _temp594= _temp590.when_true;
goto _LL593; _LL593: _temp592= _temp590.when_false; goto _LL591; _LL591: {
struct Cyc_NewControlFlow_AbsSynOne _temp598; struct Cyc_NewControlFlow_AbsSynOne
_temp600; struct Cyc_NewControlFlow_AbsSyn _temp596= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp288); _LL601: _temp600= _temp596.when_true;
goto _LL599; _LL599: _temp598= _temp596.when_false; goto _LL597; _LL597: {
struct Cyc_NewControlFlow_AbsSynOne _temp604; struct Cyc_NewControlFlow_AbsSynOne
_temp606; struct Cyc_NewControlFlow_AbsSyn _temp602= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), _temp286); _LL607: _temp606= _temp602.when_true;
goto _LL605; _LL605: _temp604= _temp602.when_false; goto _LL603; _LL603: { void*
_temp608= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp594.inner_exp,( void*) _temp594.assigns), Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp600.inner_exp,( void*) _temp600.assigns)),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp592.inner_exp,( void*) _temp592.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp606.inner_exp,(
void*) _temp606.assigns))); void* _temp609= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp594.inner_exp,( void*) _temp594.assigns),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp598.inner_exp,(
void*) _temp598.assigns)), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp592.inner_exp,(
void*) _temp592.assigns), Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,(
void*) _temp604.inner_exp,( void*) _temp604.assigns))); return({ struct Cyc_NewControlFlow_AbsSyn
_temp610; _temp610.when_true=({ struct Cyc_NewControlFlow_AbsSynOne _temp612;
_temp612.inner_exp=( void*) _temp608; _temp612.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp612;}); _temp610.when_false=({ struct Cyc_NewControlFlow_AbsSynOne
_temp611; _temp611.inner_exp=( void*) _temp609; _temp611.assigns=( void*) Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()); _temp611;}); _temp610.lvalues= 0; _temp610;});}}}}
_LL183: { struct Cyc_NewControlFlow_AbsEnv _temp613= Cyc_NewControlFlow_change_lhs(
env, 0); _temp613.access_path= 0; Cyc_NewControlFlow_cf_prepass( _temp613,
_temp292);{ struct Cyc_Absyn_Stmt* _temp614= _temp292; while( 1) { void*
_temp615=( void*) _temp614->r; struct Cyc_Absyn_Stmt* _temp625; struct Cyc_Absyn_Stmt*
_temp627; struct Cyc_Absyn_Stmt* _temp629; struct Cyc_Absyn_Decl* _temp631;
struct Cyc_Absyn_Exp* _temp633; _LL617: if(( unsigned int) _temp615 >  1u?*((
int*) _temp615) ==  Cyc_Absyn_Seq_s: 0){ _LL628: _temp627=(( struct Cyc_Absyn_Seq_s_struct*)
_temp615)->f1; goto _LL626; _LL626: _temp625=(( struct Cyc_Absyn_Seq_s_struct*)
_temp615)->f2; goto _LL618;} else{ goto _LL619;} _LL619: if(( unsigned int)
_temp615 >  1u?*(( int*) _temp615) ==  Cyc_Absyn_Decl_s: 0){ _LL632: _temp631=((
struct Cyc_Absyn_Decl_s_struct*) _temp615)->f1; goto _LL630; _LL630: _temp629=((
struct Cyc_Absyn_Decl_s_struct*) _temp615)->f2; goto _LL620;} else{ goto _LL621;}
_LL621: if(( unsigned int) _temp615 >  1u?*(( int*) _temp615) ==  Cyc_Absyn_Exp_s:
0){ _LL634: _temp633=(( struct Cyc_Absyn_Exp_s_struct*) _temp615)->f1; goto
_LL622;} else{ goto _LL623;} _LL623: goto _LL624; _LL618: _temp614= _temp625;
continue; _LL620: _temp614= _temp629; continue; _LL622: { struct Cyc_NewControlFlow_AbsSyn
_temp635= Cyc_NewControlFlow_abstract_exp( Cyc_NewControlFlow_use_it_env( env),
_temp633); void* _temp638; void* _temp640; struct Cyc_NewControlFlow_AbsSynOne
_temp636= Cyc_NewControlFlow_meet_absexp( _temp635); _LL641: _temp640=( void*)
_temp636.inner_exp; goto _LL639; _LL639: _temp638=( void*) _temp636.assigns;
goto _LL637; _LL637:( void*)((( struct Cyc_List_List*) _check_null(( Cyc_NewControlFlow_get_stmt_annot(
_temp614))->absexps))->hd=( void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
_temp640, _temp638)); return Cyc_NewControlFlow_mkSyn_tf( Cyc_CfAbsexp_mkStmtAE(
_temp292), 0, Cyc_NewControlFlow_make_assigns( env, Cyc_CfAbsexp_mkUnknownOp()));}
_LL624:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp642=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp642[ 0]=({ struct Cyc_Core_Impossible_struct _temp643; _temp643.tag= Cyc_Core_Impossible;
_temp643.f1= _tag_arr("abstract_exp: ill-formed StmtExp", sizeof( unsigned char),
33u); _temp643;}); _temp642;})); _LL616:;}}} _LL185: { struct Cyc_NewControlFlow_AbsEnv
_temp644= Cyc_NewControlFlow_change_lhs( env, 0); _temp644.access_path= 0;{
struct Cyc_NewControlFlow_AbsSyn _temp645= Cyc_NewControlFlow_abstract_exp(
_temp644, _temp296); struct Cyc_NewControlFlow_AbsSyn _temp646= Cyc_NewControlFlow_abstract_exp(
env, _temp294); struct Cyc_NewControlFlow_AbsSynOne _temp647= Cyc_NewControlFlow_meet_absexp(
_temp645); return({ struct Cyc_NewControlFlow_AbsSyn _temp648; _temp648.when_true=({
struct Cyc_NewControlFlow_AbsSynOne _temp650; _temp650.inner_exp=( void*) Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG,( void*) _temp647.inner_exp,( void*)( _temp646.when_true).inner_exp);
_temp650.assigns=( void*)(( void*)( _temp646.when_true).assigns); _temp650;});
_temp648.when_false=({ struct Cyc_NewControlFlow_AbsSynOne _temp649; _temp649.inner_exp=(
void*) Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*) _temp647.inner_exp,(
void*)( _temp646.when_false).inner_exp); _temp649.assigns=( void*)(( void*)(
_temp646.when_false).assigns); _temp649;}); _temp648.lvalues= 0; _temp648;});}}
_LL187: env.access_path ==  0? 0:(((( int(*)( struct _tagged_arr assertion,
struct _tagged_arr file, unsigned int line)) Cyc_Std___assert_fail)( _tag_arr("env.access_path==NULL",
sizeof( unsigned char), 22u), _tag_arr("new_control_flow.cyc", sizeof(
unsigned char), 21u), 724), 0)); Cyc_NewControlFlow_add_var_root( env, _temp302);(
env.shared)->comprehension_vars=({ struct Cyc_List_List* _temp651=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp651->hd=( void*) _temp302;
_temp651->tl=( env.shared)->comprehension_vars; _temp651;});{ struct Cyc_NewControlFlow_AbsEnv
_temp652= Cyc_NewControlFlow_use_it_env( env); void* _temp655; void* _temp657;
struct Cyc_NewControlFlow_AbsSynOne _temp653= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( _temp652, _temp300)); _LL658: _temp657=( void*)
_temp653.inner_exp; goto _LL656; _LL656: _temp655=( void*) _temp653.assigns;
goto _LL654; _LL654: { void* _temp661; void* _temp663; struct Cyc_NewControlFlow_AbsSynOne
_temp659= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_abstract_exp(
_temp652, _temp298)); _LL664: _temp663=( void*) _temp659.inner_exp; goto _LL662;
_LL662: _temp661=( void*) _temp659.assigns; goto _LL660; _LL660: { void*
_temp665= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp657, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp655, Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,
Cyc_CfAbsexp_mkAssignAE( Cyc_CfAbsexp_mkLocalOp( _temp302), Cyc_CfAbsexp_mkUnknownOp()),
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OneofG, Cyc_CfAbsexp_mkGroupAE((
void*) Cyc_CfAbsexp_OrderedG, _temp663, _temp661), Cyc_CfAbsexp_mkSkipAE()))));
return Cyc_NewControlFlow_mkSyn_tf( _temp665, 0, Cyc_NewControlFlow_make_assigns(
env, Cyc_CfAbsexp_mkUnknownOp()));}}} _LL189: goto _LL191; _LL191: goto _LL193;
_LL193: goto _LL195; _LL195: goto _LL197; _LL197: goto _LL199; _LL199: goto
_LL201; _LL201:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp666=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp666[ 0]=({ struct Cyc_Core_Impossible_struct _temp667; _temp667.tag= Cyc_Core_Impossible;
_temp667.f1= _tag_arr("abstract_exp, unexpected exp form", sizeof( unsigned char),
34u); _temp667;}); _temp666;})); _LL113:;} void* Cyc_NewControlFlow_abstract_exp_top(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Exp* e){ void* _temp670;
void* _temp672; struct Cyc_NewControlFlow_AbsSynOne _temp668= Cyc_NewControlFlow_meet_absexp(
Cyc_NewControlFlow_abstract_exp( env, e)); _LL673: _temp672=( void*) _temp668.inner_exp;
goto _LL671; _LL671: _temp670=( void*) _temp668.assigns; goto _LL669; _LL669:
return Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp672, _temp670);}
struct _tuple0 Cyc_NewControlFlow_abstract_guard( struct Cyc_NewControlFlow_AbsEnv
env, struct Cyc_Absyn_Exp* e){ struct Cyc_NewControlFlow_AbsSyn _temp674= Cyc_NewControlFlow_abstract_exp(
Cyc_NewControlFlow_use_it_env( env), e); return({ struct _tuple0 _temp675;
_temp675.f1= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)(
_temp674.when_true).inner_exp,( void*)( _temp674.when_true).assigns); _temp675.f2=
Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG,( void*)( _temp674.when_false).inner_exp,(
void*)( _temp674.when_false).assigns); _temp675;});} struct Cyc_List_List* Cyc_NewControlFlow_cf_prepass_scs(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_List_List* scs){ struct Cyc_List_List*
_temp676= 0; for( 0; scs !=  0; scs=(( struct Cyc_List_List*) _check_null( scs))->tl){
struct Cyc_Absyn_Switch_clause _temp679; struct Cyc_Absyn_Stmt* _temp680; struct
Cyc_Absyn_Exp* _temp682; struct Cyc_Core_Opt* _temp684; struct Cyc_Absyn_Switch_clause*
_temp677=( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs))->hd; _temp679=* _temp677; _LL685: _temp684= _temp679.pat_vars; goto _LL683;
_LL683: _temp682= _temp679.where_clause; goto _LL681; _LL681: _temp680= _temp679.body;
goto _LL678; _LL678: if( _temp684 ==  0){( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp686=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp686[ 0]=({ struct Cyc_Core_Impossible_struct _temp687; _temp687.tag= Cyc_Core_Impossible;
_temp687.f1= _tag_arr("switch clause vds not set", sizeof( unsigned char), 26u);
_temp687;}); _temp686;}));}{ struct Cyc_List_List* vds=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp684))->v; for( 0; vds !=  0; vds=((
struct Cyc_List_List*) _check_null( vds))->tl){ Cyc_NewControlFlow_add_var_root(
env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null( vds))->hd);}}
if( _temp682 !=  0){ void* _temp690; void* _temp692; struct _tuple0 _temp688=
Cyc_NewControlFlow_abstract_guard( env,( struct Cyc_Absyn_Exp*) _check_null(
_temp682)); _LL693: _temp692= _temp688.f1; goto _LL691; _LL691: _temp690=
_temp688.f2; goto _LL689; _LL689: _temp676=({ struct Cyc_List_List* _temp694=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp694->hd=(
void*) _temp692; _temp694->tl=({ struct Cyc_List_List* _temp695=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp695->hd=( void*) _temp690;
_temp695->tl= _temp676; _temp695;}); _temp694;});} Cyc_NewControlFlow_cf_prepass(
env, _temp680);} return Cyc_List_imp_rev( _temp676);} void Cyc_NewControlFlow_cf_prepass(
struct Cyc_NewControlFlow_AbsEnv env, struct Cyc_Absyn_Stmt* s){ env.lhsides == 
0? 0:(((( int(*)( struct _tagged_arr assertion, struct _tagged_arr file,
unsigned int line)) Cyc_Std___assert_fail)( _tag_arr("env.lhsides == NULL",
sizeof( unsigned char), 20u), _tag_arr("new_control_flow.cyc", sizeof(
unsigned char), 21u), 776), 0)); env.access_path ==  0? 0:(((( int(*)( struct
_tagged_arr assertion, struct _tagged_arr file, unsigned int line)) Cyc_Std___assert_fail)(
_tag_arr("env.access_path == NULL", sizeof( unsigned char), 24u), _tag_arr("new_control_flow.cyc",
sizeof( unsigned char), 21u), 777), 0));{ void* _temp696=( void*) s->r; struct
Cyc_Absyn_Exp* _temp748; struct Cyc_Absyn_Stmt* _temp750; struct Cyc_Absyn_Stmt*
_temp752; struct Cyc_Absyn_Exp* _temp754; struct Cyc_Absyn_Exp* _temp756; struct
Cyc_Absyn_Stmt* _temp758; struct Cyc_Absyn_Stmt* _temp760; struct Cyc_Absyn_Exp*
_temp762; struct _tuple3 _temp764; struct Cyc_Absyn_Stmt* _temp766; struct Cyc_Absyn_Exp*
_temp768; struct Cyc_Absyn_Stmt* _temp770; struct Cyc_Absyn_Stmt* _temp772;
struct _tuple3 _temp774; struct Cyc_Absyn_Stmt* _temp776; struct Cyc_Absyn_Exp*
_temp778; struct Cyc_Absyn_Stmt* _temp780; struct Cyc_Absyn_Stmt* _temp782;
struct Cyc_Absyn_Stmt* _temp784; struct _tuple3 _temp786; struct Cyc_Absyn_Stmt*
_temp788; struct Cyc_Absyn_Exp* _temp790; struct _tuple3 _temp792; struct Cyc_Absyn_Stmt*
_temp794; struct Cyc_Absyn_Exp* _temp796; struct Cyc_Absyn_Exp* _temp798; struct
Cyc_List_List* _temp800; struct Cyc_Absyn_Exp* _temp802; struct Cyc_List_List*
_temp804; struct Cyc_Absyn_Stmt* _temp806; struct Cyc_Absyn_Decl* _temp808;
struct Cyc_Absyn_Decl _temp810; void* _temp811; struct Cyc_Absyn_Vardecl*
_temp813; struct Cyc_Absyn_Stmt* _temp815; struct Cyc_Absyn_Decl* _temp817;
struct Cyc_Absyn_Decl _temp819; void* _temp820; struct Cyc_Absyn_Exp* _temp822;
struct Cyc_Core_Opt* _temp824; struct Cyc_Core_Opt _temp826; struct Cyc_List_List*
_temp827; struct Cyc_Absyn_Stmt* _temp829; struct Cyc_Absyn_Decl* _temp831;
struct Cyc_Absyn_Decl _temp833; void* _temp834; struct Cyc_List_List* _temp836;
struct Cyc_Absyn_Stmt* _temp838; struct Cyc_List_List* _temp840; struct Cyc_Absyn_Stmt*
_temp842; struct Cyc_Absyn_Stmt* _temp844; struct Cyc_Absyn_Vardecl* _temp846;
struct Cyc_Absyn_Tvar* _temp848; _LL698: if( _temp696 == ( void*) Cyc_Absyn_Skip_s){
goto _LL699;} else{ goto _LL700;} _LL700: if(( unsigned int) _temp696 >  1u?*((
int*) _temp696) ==  Cyc_Absyn_Exp_s: 0){ _LL749: _temp748=(( struct Cyc_Absyn_Exp_s_struct*)
_temp696)->f1; goto _LL701;} else{ goto _LL702;} _LL702: if(( unsigned int)
_temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_Seq_s: 0){ _LL753: _temp752=((
struct Cyc_Absyn_Seq_s_struct*) _temp696)->f1; goto _LL751; _LL751: _temp750=((
struct Cyc_Absyn_Seq_s_struct*) _temp696)->f2; goto _LL703;} else{ goto _LL704;}
_LL704: if(( unsigned int) _temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_Return_s:
0){ _LL755: _temp754=(( struct Cyc_Absyn_Return_s_struct*) _temp696)->f1; if(
_temp754 ==  0){ goto _LL705;} else{ goto _LL706;}} else{ goto _LL706;} _LL706:
if(( unsigned int) _temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_Return_s: 0){
_LL757: _temp756=(( struct Cyc_Absyn_Return_s_struct*) _temp696)->f1; goto
_LL707;} else{ goto _LL708;} _LL708: if(( unsigned int) _temp696 >  1u?*(( int*)
_temp696) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL763: _temp762=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp696)->f1; goto _LL761; _LL761: _temp760=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp696)->f2; goto _LL759; _LL759: _temp758=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp696)->f3; goto _LL709;} else{ goto _LL710;} _LL710: if(( unsigned int)
_temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_Do_s: 0){ _LL771: _temp770=((
struct Cyc_Absyn_Do_s_struct*) _temp696)->f1; goto _LL765; _LL765: _temp764=((
struct Cyc_Absyn_Do_s_struct*) _temp696)->f2; _LL769: _temp768= _temp764.f1;
goto _LL767; _LL767: _temp766= _temp764.f2; goto _LL711;} else{ goto _LL712;}
_LL712: if(( unsigned int) _temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_While_s:
0){ _LL775: _temp774=(( struct Cyc_Absyn_While_s_struct*) _temp696)->f1; _LL779:
_temp778= _temp774.f1; goto _LL777; _LL777: _temp776= _temp774.f2; goto _LL773;
_LL773: _temp772=(( struct Cyc_Absyn_While_s_struct*) _temp696)->f2; goto _LL713;}
else{ goto _LL714;} _LL714: if(( unsigned int) _temp696 >  1u?*(( int*) _temp696)
==  Cyc_Absyn_Break_s: 0){ goto _LL715;} else{ goto _LL716;} _LL716: if((
unsigned int) _temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_Continue_s: 0){
goto _LL717;} else{ goto _LL718;} _LL718: if(( unsigned int) _temp696 >  1u?*((
int*) _temp696) ==  Cyc_Absyn_Goto_s: 0){ _LL781: _temp780=(( struct Cyc_Absyn_Goto_s_struct*)
_temp696)->f2; if( _temp780 ==  0){ goto _LL719;} else{ goto _LL720;}} else{
goto _LL720;} _LL720: if(( unsigned int) _temp696 >  1u?*(( int*) _temp696) == 
Cyc_Absyn_Goto_s: 0){ _LL783: _temp782=(( struct Cyc_Absyn_Goto_s_struct*)
_temp696)->f2; goto _LL721;} else{ goto _LL722;} _LL722: if(( unsigned int)
_temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_For_s: 0){ _LL799: _temp798=((
struct Cyc_Absyn_For_s_struct*) _temp696)->f1; goto _LL793; _LL793: _temp792=((
struct Cyc_Absyn_For_s_struct*) _temp696)->f2; _LL797: _temp796= _temp792.f1;
goto _LL795; _LL795: _temp794= _temp792.f2; goto _LL787; _LL787: _temp786=((
struct Cyc_Absyn_For_s_struct*) _temp696)->f3; _LL791: _temp790= _temp786.f1;
goto _LL789; _LL789: _temp788= _temp786.f2; goto _LL785; _LL785: _temp784=((
struct Cyc_Absyn_For_s_struct*) _temp696)->f4; goto _LL723;} else{ goto _LL724;}
_LL724: if(( unsigned int) _temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_Switch_s:
0){ _LL803: _temp802=(( struct Cyc_Absyn_Switch_s_struct*) _temp696)->f1; goto
_LL801; _LL801: _temp800=(( struct Cyc_Absyn_Switch_s_struct*) _temp696)->f2;
goto _LL725;} else{ goto _LL726;} _LL726: if(( unsigned int) _temp696 >  1u?*((
int*) _temp696) ==  Cyc_Absyn_Fallthru_s: 0){ _LL805: _temp804=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp696)->f1; goto _LL727;} else{ goto _LL728;} _LL728: if(( unsigned int)
_temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_Decl_s: 0){ _LL809: _temp808=((
struct Cyc_Absyn_Decl_s_struct*) _temp696)->f1; _temp810=* _temp808; _LL812:
_temp811=( void*) _temp810.r; if(*(( int*) _temp811) ==  Cyc_Absyn_Var_d){
_LL814: _temp813=(( struct Cyc_Absyn_Var_d_struct*) _temp811)->f1; goto _LL807;}
else{ goto _LL730;} _LL807: _temp806=(( struct Cyc_Absyn_Decl_s_struct*)
_temp696)->f2; goto _LL729;} else{ goto _LL730;} _LL730: if(( unsigned int)
_temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_Decl_s: 0){ _LL818: _temp817=((
struct Cyc_Absyn_Decl_s_struct*) _temp696)->f1; _temp819=* _temp817; _LL821:
_temp820=( void*) _temp819.r; if(*(( int*) _temp820) ==  Cyc_Absyn_Let_d){
_LL825: _temp824=(( struct Cyc_Absyn_Let_d_struct*) _temp820)->f2; if( _temp824
==  0){ goto _LL732;} else{ _temp826=* _temp824; _LL828: _temp827=( struct Cyc_List_List*)
_temp826.v; goto _LL823;} _LL823: _temp822=(( struct Cyc_Absyn_Let_d_struct*)
_temp820)->f4; goto _LL816;} else{ goto _LL732;} _LL816: _temp815=(( struct Cyc_Absyn_Decl_s_struct*)
_temp696)->f2; goto _LL731;} else{ goto _LL732;} _LL732: if(( unsigned int)
_temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_Decl_s: 0){ _LL832: _temp831=((
struct Cyc_Absyn_Decl_s_struct*) _temp696)->f1; _temp833=* _temp831; _LL835:
_temp834=( void*) _temp833.r; if(*(( int*) _temp834) ==  Cyc_Absyn_Letv_d){
_LL837: _temp836=(( struct Cyc_Absyn_Letv_d_struct*) _temp834)->f1; goto _LL830;}
else{ goto _LL734;} _LL830: _temp829=(( struct Cyc_Absyn_Decl_s_struct*)
_temp696)->f2; goto _LL733;} else{ goto _LL734;} _LL734: if(( unsigned int)
_temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_Label_s: 0){ _LL839: _temp838=((
struct Cyc_Absyn_Label_s_struct*) _temp696)->f2; goto _LL735;} else{ goto _LL736;}
_LL736: if(( unsigned int) _temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_TryCatch_s:
0){ _LL843: _temp842=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp696)->f1; goto
_LL841; _LL841: _temp840=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp696)->f2;
goto _LL737;} else{ goto _LL738;} _LL738: if(( unsigned int) _temp696 >  1u?*((
int*) _temp696) ==  Cyc_Absyn_Region_s: 0){ _LL849: _temp848=(( struct Cyc_Absyn_Region_s_struct*)
_temp696)->f1; goto _LL847; _LL847: _temp846=(( struct Cyc_Absyn_Region_s_struct*)
_temp696)->f2; goto _LL845; _LL845: _temp844=(( struct Cyc_Absyn_Region_s_struct*)
_temp696)->f3; goto _LL739;} else{ goto _LL740;} _LL740: if(( unsigned int)
_temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_Decl_s: 0){ goto _LL741;} else{
goto _LL742;} _LL742: if(( unsigned int) _temp696 >  1u?*(( int*) _temp696) == 
Cyc_Absyn_SwitchC_s: 0){ goto _LL743;} else{ goto _LL744;} _LL744: if((
unsigned int) _temp696 >  1u?*(( int*) _temp696) ==  Cyc_Absyn_Cut_s: 0){ goto
_LL745;} else{ goto _LL746;} _LL746: if(( unsigned int) _temp696 >  1u?*(( int*)
_temp696) ==  Cyc_Absyn_Splice_s: 0){ goto _LL747;} else{ goto _LL697;} _LL699:
return; _LL701: { void* _temp850= Cyc_NewControlFlow_abstract_exp_top( env,
_temp748); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp851=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp851->hd=( void*) _temp850; _temp851->tl= 0; _temp851;})); return;} _LL703:
Cyc_NewControlFlow_cf_prepass( env, _temp752); Cyc_NewControlFlow_cf_prepass(
env, _temp750); return; _LL705: return; _LL707: { void* _temp852= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env),( struct Cyc_Absyn_Exp*) _check_null(
_temp756)); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp853=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp853->hd=( void*) _temp852; _temp853->tl= 0; _temp853;})); return;} _LL709: {
void* _temp856; void* _temp858; struct _tuple0 _temp854= Cyc_NewControlFlow_abstract_guard(
env, _temp762); _LL859: _temp858= _temp854.f1; goto _LL857; _LL857: _temp856=
_temp854.f2; goto _LL855; _LL855: Cyc_NewControlFlow_cf_set_absexps( s,({ struct
Cyc_List_List* _temp860=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp860->hd=( void*) _temp858; _temp860->tl=({ struct Cyc_List_List* _temp861=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp861->hd=(
void*) _temp856; _temp861->tl= 0; _temp861;}); _temp860;})); Cyc_NewControlFlow_cf_prepass(
env, _temp760); Cyc_NewControlFlow_cf_prepass( env, _temp758); return;} _LL711:
_temp778= _temp768; _temp776= _temp766; _temp772= _temp770; goto _LL713; _LL713: {
void* _temp864; void* _temp866; struct _tuple0 _temp862= Cyc_NewControlFlow_abstract_guard(
env, _temp778); _LL867: _temp866= _temp862.f1; goto _LL865; _LL865: _temp864=
_temp862.f2; goto _LL863; _LL863: Cyc_NewControlFlow_cf_set_absexps( s,({ struct
Cyc_List_List* _temp868=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp868->hd=( void*) _temp866; _temp868->tl=({ struct Cyc_List_List* _temp869=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp869->hd=(
void*) _temp864; _temp869->tl= 0; _temp869;}); _temp868;})); Cyc_NewControlFlow_cf_prepass(
env, _temp772); return;} _LL715: return; _LL717: return; _LL719:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp870=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp870[ 0]=({ struct
Cyc_Core_Impossible_struct _temp871; _temp871.tag= Cyc_Core_Impossible; _temp871.f1=
_tag_arr("cf_prepass: goto w/o destination", sizeof( unsigned char), 33u);
_temp871;}); _temp870;})); _LL721: { struct Cyc_Absyn_Stmt* _temp872=( Cyc_NewControlFlow_get_stmt_annot(
s))->encloser; struct Cyc_Absyn_Stmt* _temp873=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*) _check_null( _temp782)))->encloser; while( _temp873 != 
_temp872) { struct Cyc_Absyn_Stmt* _temp874=( Cyc_NewControlFlow_get_stmt_annot(
_temp872))->encloser; if( _temp874 ==  _temp872){({ void* _temp875[ 0u]={}; Cyc_Tcutil_terr(
s->loc, _tag_arr("goto enters local scope or exception handler", sizeof(
unsigned char), 45u), _tag_arr( _temp875, sizeof( void*), 0u));}); break;}
_temp872= _temp874;} return;} _LL723: { void* _temp876= Cyc_NewControlFlow_abstract_exp_top(
env, _temp798); void* _temp879; void* _temp881; struct _tuple0 _temp877= Cyc_NewControlFlow_abstract_guard(
env, _temp796); _LL882: _temp881= _temp877.f1; goto _LL880; _LL880: _temp879=
_temp877.f2; goto _LL878; _LL878: { void* _temp883= Cyc_NewControlFlow_abstract_exp_top(
env, _temp790); Cyc_NewControlFlow_cf_set_absexps( s,({ void* _temp884[ 4u];
_temp884[ 3u]= _temp883; _temp884[ 2u]= _temp879; _temp884[ 1u]= _temp881;
_temp884[ 0u]= _temp876; Cyc_List_list( _tag_arr( _temp884, sizeof( void*), 4u));}));
Cyc_NewControlFlow_cf_prepass( env, _temp784); return;}} _LL725: { void*
_temp885= Cyc_NewControlFlow_abstract_exp_top( Cyc_NewControlFlow_use_it_env(
env), _temp802); struct Cyc_List_List* _temp886= Cyc_NewControlFlow_cf_prepass_scs(
env, _temp800); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp887=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp887->hd=( void*) _temp885; _temp887->tl= _temp886; _temp887;})); return;}
_LL727: { void* _temp890; void* _temp892; struct Cyc_NewControlFlow_AbsSynOne
_temp888= Cyc_NewControlFlow_meet_absexp( Cyc_NewControlFlow_use_exp_unordered(
env, _temp804, 0)); _LL893: _temp892=( void*) _temp888.inner_exp; goto _LL891;
_LL891: _temp890=( void*) _temp888.assigns; goto _LL889; _LL889: { void*
_temp894= Cyc_CfAbsexp_mkGroupAE(( void*) Cyc_CfAbsexp_OrderedG, _temp892,
_temp890); Cyc_NewControlFlow_cf_set_absexps( s,({ struct Cyc_List_List*
_temp895=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp895->hd=( void*) _temp894; _temp895->tl= 0; _temp895;})); return;}} _LL729:
Cyc_NewControlFlow_add_var_root( env, _temp813); if( _temp813->initializer !=  0){
struct Cyc_NewControlFlow_AbsEnv _temp896= Cyc_NewControlFlow_change_lhs( env,({
struct Cyc_List_List* _temp899=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp899->hd=( void*) Cyc_CfAbsexp_mkLocalOp( _temp813);
_temp899->tl= 0; _temp899;})); void* _temp897= Cyc_NewControlFlow_abstract_exp_top(
_temp896,( struct Cyc_Absyn_Exp*) _check_null( _temp813->initializer)); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp898=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp898->hd=( void*) _temp897; _temp898->tl= 0;
_temp898;}));} Cyc_NewControlFlow_cf_prepass( env, _temp806); return; _LL731:
for( 0; _temp827 !=  0; _temp827=(( struct Cyc_List_List*) _check_null( _temp827))->tl){
Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp827))->hd);}{ void* _temp900= Cyc_NewControlFlow_abstract_exp_top(
Cyc_NewControlFlow_use_it_env( env), _temp822); Cyc_NewControlFlow_cf_set_absexps(
s,({ struct Cyc_List_List* _temp901=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp901->hd=( void*) _temp900; _temp901->tl= 0;
_temp901;})); Cyc_NewControlFlow_cf_prepass( env, _temp815); return;} _LL733:
for( 0; _temp836 !=  0; _temp836=(( struct Cyc_List_List*) _check_null( _temp836))->tl){
Cyc_NewControlFlow_add_var_root( env,( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*)
_check_null( _temp836))->hd);} Cyc_NewControlFlow_cf_prepass( env, _temp829);
return; _LL735: Cyc_NewControlFlow_cf_prepass( env, _temp838); return; _LL737:
Cyc_NewControlFlow_cf_prepass( env, _temp842);{ struct Cyc_List_List* _temp902=
Cyc_NewControlFlow_cf_prepass_scs( env, _temp840); Cyc_NewControlFlow_cf_set_absexps(
s, _temp902); return;} _LL739: Cyc_NewControlFlow_add_var_root( env, _temp846);
Cyc_NewControlFlow_cf_prepass( env, _temp844); return; _LL741: goto _LL743;
_LL743: goto _LL745; _LL745: goto _LL747; _LL747:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp903=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp903[ 0]=({ struct
Cyc_Core_Impossible_struct _temp904; _temp904.tag= Cyc_Core_Impossible; _temp904.f1=
_tag_arr("cf_prepass: bad stmt form", sizeof( unsigned char), 26u); _temp904;});
_temp903;})); _LL697:;}} static int Cyc_NewControlFlow_iterate_cf_check= 0;
static int Cyc_NewControlFlow_iteration_num= 0; struct Cyc_NewControlFlow_AnalEnv;
void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv* env,
void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
env->all_changed, new_flow,( void*) env->tryflow));}} static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp905= Cyc_NewControlFlow_get_stmt_annot(
s); void* _temp906= Cyc_CfFlowInfo_join_flow( env->all_changed, flow,( void*)
_temp905->flow); if( ! Cyc_CfFlowInfo_flow_lessthan_approx( _temp906,( void*)
_temp905->flow)){( void*)( _temp905->flow=( void*) _temp906); if( _temp905->visited
==  Cyc_NewControlFlow_iteration_num){ Cyc_NewControlFlow_iterate_cf_check= 1;}}}
static struct Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv* env, void* in_flow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp907= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( _temp907->flow=( void*) Cyc_CfFlowInfo_join_flow( env->all_changed,
in_flow,( void*) _temp907->flow)); ++ _temp907->visited; _temp907->visited == 
Cyc_NewControlFlow_iteration_num? 0:(((( int(*)( struct _tagged_arr assertion,
struct _tagged_arr file, unsigned int line)) Cyc_Std___assert_fail)( _tag_arr("annot->visited == iteration_num",
sizeof( unsigned char), 32u), _tag_arr("new_control_flow.cyc", sizeof(
unsigned char), 21u), 936), 0)); return _temp907;} static void* Cyc_NewControlFlow_add_init_vars_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp908= flow; struct Cyc_Dict_Dict* _temp914; _LL910: if( _temp908 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL911;} else{ goto _LL912;} _LL912: if((
unsigned int) _temp908 >  1u?*(( int*) _temp908) ==  Cyc_CfFlowInfo_InitsFL: 0){
_LL915: _temp914=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp908)->f1; goto
_LL913;} else{ goto _LL909;} _LL911: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL913: for( 0; vds !=  0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp916=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp918=( struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp918[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp919; _temp919.tag= Cyc_CfFlowInfo_VarRoot;
_temp919.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
vds))->hd; _temp919;}); _temp918;}); void* _temp917= Cyc_CfFlowInfo_assign_unknown_dict((
void*) Cyc_CfFlowInfo_AllIL, Cyc_Dict_lookup( env->roots,( void*) _temp916));
_temp914= Cyc_Dict_insert( _temp914,( void*) _temp916, _temp917);} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp920=( struct Cyc_CfFlowInfo_InitsFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp920[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp921; _temp921.tag= Cyc_CfFlowInfo_InitsFL;
_temp921.f1= _temp914; _temp921;}); _temp920;}); _LL909:;} static void* Cyc_NewControlFlow_add_vardecls_flow(
struct Cyc_NewControlFlow_AnalEnv* env, void* flow, struct Cyc_List_List* vds){
void* _temp922= flow; struct Cyc_Dict_Dict* _temp928; _LL924: if( _temp922 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL925;} else{ goto _LL926;} _LL926: if((
unsigned int) _temp922 >  1u?*(( int*) _temp922) ==  Cyc_CfFlowInfo_InitsFL: 0){
_LL929: _temp928=(( struct Cyc_CfFlowInfo_InitsFL_struct*) _temp922)->f1; goto
_LL927;} else{ goto _LL923;} _LL925: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL927: for( 0; vds !=  0; vds=(( struct Cyc_List_List*) _check_null( vds))->tl){
struct Cyc_CfFlowInfo_VarRoot_struct* _temp930=({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp931=( struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp931[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp932; _temp932.tag= Cyc_CfFlowInfo_VarRoot;
_temp932.f1=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_List_List*) _check_null(
vds))->hd; _temp932;}); _temp931;}); _temp928= Cyc_Dict_insert( _temp928,( void*)
_temp930, Cyc_Dict_lookup( env->roots,( void*) _temp930));} return( void*)({
struct Cyc_CfFlowInfo_InitsFL_struct* _temp933=( struct Cyc_CfFlowInfo_InitsFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct)); _temp933[ 0]=({
struct Cyc_CfFlowInfo_InitsFL_struct _temp934; _temp934.tag= Cyc_CfFlowInfo_InitsFL;
_temp934.f1= _temp928; _temp934;}); _temp933;}); _LL923:;} static struct _tuple0
Cyc_NewControlFlow_cf_evalguard( struct Cyc_NewControlFlow_AnalEnv* env, struct
Cyc_Position_Segment* loc, struct Cyc_List_List* aes, void* in_flow){ if(( void*)((
struct Cyc_List_List*) _check_null( aes))->hd == ( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( aes))->tl))->hd){ void*
_temp935= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); return({ struct _tuple0 _temp936; _temp936.f1=
_temp935; _temp936.f2= _temp935; _temp936;});} return({ struct _tuple0 _temp937;
_temp937.f1= Cyc_CfAbsexp_eval_absexp( env, loc,( void*)(( struct Cyc_List_List*)
_check_null( aes))->hd, in_flow); _temp937.f2= Cyc_CfAbsexp_eval_absexp( env,
loc,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->hd, in_flow); _temp937;});} static void* Cyc_NewControlFlow_cf_analyze_switch_clauses(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_List_List* scs, struct Cyc_List_List*
aes, void* in_flow){ for( 0; scs !=  0; scs=(( struct Cyc_List_List*)
_check_null( scs))->tl){ struct Cyc_Absyn_Switch_clause _temp940; struct Cyc_Absyn_Stmt*
_temp941; struct Cyc_Absyn_Exp* _temp943; struct Cyc_Core_Opt* _temp945; struct
Cyc_Absyn_Switch_clause* _temp938=( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null( scs))->hd; _temp940=* _temp938; _LL946: _temp945=
_temp940.pat_vars; goto _LL944; _LL944: _temp943= _temp940.where_clause; goto
_LL942; _LL942: _temp941= _temp940.body; goto _LL939; _LL939: { void* _temp947=
Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp945))->v); _temp947= Cyc_NewControlFlow_add_init_vars_flow(
env, _temp947,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp945))->v);{ void* body_outflow; if( _temp943 !=  0){ void* _temp950; void*
_temp952; struct _tuple0 _temp948= Cyc_NewControlFlow_cf_evalguard( env,((
struct Cyc_Absyn_Exp*) _check_null( _temp943))->loc, aes, _temp947); _LL953:
_temp952= _temp948.f1; goto _LL951; _LL951: _temp950= _temp948.f2; goto _LL949;
_LL949: aes=(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( aes))->tl))->tl; in_flow= _temp950; body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp941, _temp952);} else{ body_outflow= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp941, _temp947);}{ void* _temp954= body_outflow; _LL956: if( _temp954
== ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL957;} else{ goto _LL958;} _LL958:
goto _LL959; _LL957: goto _LL955; _LL959: if((( struct Cyc_List_List*)
_check_null( scs))->tl ==  0){ return body_outflow;} else{ if(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Switch_clause*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null( scs))->tl))->hd)->pat_vars))->v
!=  0){({ void* _temp960[ 0u]={}; Cyc_Tcutil_terr( _temp941->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp960, sizeof( void*), 0u));});}
else{({ void* _temp961[ 0u]={}; Cyc_Tcutil_warn( _temp941->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp961, sizeof( void*), 0u));});} Cyc_NewControlFlow_update_flow(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( scs))->tl))->hd)->body, body_outflow);} goto
_LL955; _LL955:;}}}} return( void*) Cyc_CfFlowInfo_BottomFL;} void* Cyc_NewControlFlow_cf_analyze_stmt(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Absyn_Stmt* s, void* in_flow){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp962= Cyc_NewControlFlow_pre_stmt_check(
env, in_flow, s); in_flow=( void*) _temp962->flow;{ struct Cyc_List_List*
_temp963= _temp962->absexps; void* _temp964=( void*) s->r; struct Cyc_Absyn_Exp*
_temp1010; struct Cyc_Absyn_Exp* _temp1012; struct Cyc_Absyn_Exp* _temp1014;
struct Cyc_Absyn_Stmt* _temp1016; struct Cyc_Absyn_Stmt* _temp1018; struct Cyc_Absyn_Stmt*
_temp1020; struct Cyc_Absyn_Stmt* _temp1022; struct Cyc_Absyn_Exp* _temp1024;
struct Cyc_Absyn_Stmt* _temp1026; struct _tuple3 _temp1028; struct Cyc_Absyn_Stmt*
_temp1030; struct Cyc_Absyn_Exp* _temp1032; struct _tuple3 _temp1034; struct Cyc_Absyn_Stmt*
_temp1036; struct Cyc_Absyn_Exp* _temp1038; struct Cyc_Absyn_Stmt* _temp1040;
struct Cyc_Absyn_Stmt* _temp1042; struct _tuple3 _temp1044; struct Cyc_Absyn_Stmt*
_temp1046; struct Cyc_Absyn_Exp* _temp1048; struct _tuple3 _temp1050; struct Cyc_Absyn_Stmt*
_temp1052; struct Cyc_Absyn_Exp* _temp1054; struct Cyc_Absyn_Exp* _temp1056;
struct Cyc_Absyn_Stmt* _temp1058; struct Cyc_Absyn_Switch_clause** _temp1060;
struct Cyc_Absyn_Switch_clause* _temp1062; struct Cyc_List_List* _temp1063;
struct Cyc_Absyn_Stmt* _temp1065; struct Cyc_Absyn_Stmt* _temp1067; struct Cyc_Absyn_Stmt*
_temp1069; struct Cyc_List_List* _temp1071; struct Cyc_Absyn_Exp* _temp1073;
struct Cyc_List_List* _temp1075; struct Cyc_Absyn_Stmt* _temp1077; struct Cyc_Absyn_Stmt*
_temp1079; struct Cyc_Absyn_Decl* _temp1081; struct Cyc_Absyn_Decl _temp1083;
void* _temp1084; struct Cyc_Absyn_Vardecl* _temp1086; struct Cyc_Absyn_Stmt*
_temp1088; struct Cyc_Absyn_Decl* _temp1090; struct Cyc_Absyn_Decl _temp1092;
void* _temp1093; struct Cyc_Absyn_Exp* _temp1095; struct Cyc_Core_Opt* _temp1097;
struct Cyc_Core_Opt _temp1099; struct Cyc_List_List* _temp1100; struct Cyc_Absyn_Stmt*
_temp1102; struct Cyc_Absyn_Decl* _temp1104; struct Cyc_Absyn_Decl _temp1106;
void* _temp1107; struct Cyc_List_List* _temp1109; struct Cyc_Absyn_Stmt*
_temp1111; struct Cyc_Absyn_Stmt* _temp1113; struct Cyc_Absyn_Vardecl* _temp1115;
struct Cyc_Absyn_Tvar* _temp1117; _LL966: if( _temp964 == ( void*) Cyc_Absyn_Skip_s){
goto _LL967;} else{ goto _LL968;} _LL968: if(( unsigned int) _temp964 >  1u?*((
int*) _temp964) ==  Cyc_Absyn_Return_s: 0){ _LL1011: _temp1010=(( struct Cyc_Absyn_Return_s_struct*)
_temp964)->f1; if( _temp1010 ==  0){ goto _LL969;} else{ goto _LL970;}} else{
goto _LL970;} _LL970: if(( unsigned int) _temp964 >  1u?*(( int*) _temp964) == 
Cyc_Absyn_Return_s: 0){ _LL1013: _temp1012=(( struct Cyc_Absyn_Return_s_struct*)
_temp964)->f1; goto _LL971;} else{ goto _LL972;} _LL972: if(( unsigned int)
_temp964 >  1u?*(( int*) _temp964) ==  Cyc_Absyn_Exp_s: 0){ _LL1015: _temp1014=((
struct Cyc_Absyn_Exp_s_struct*) _temp964)->f1; goto _LL973;} else{ goto _LL974;}
_LL974: if(( unsigned int) _temp964 >  1u?*(( int*) _temp964) ==  Cyc_Absyn_Seq_s:
0){ _LL1019: _temp1018=(( struct Cyc_Absyn_Seq_s_struct*) _temp964)->f1; goto
_LL1017; _LL1017: _temp1016=(( struct Cyc_Absyn_Seq_s_struct*) _temp964)->f2;
goto _LL975;} else{ goto _LL976;} _LL976: if(( unsigned int) _temp964 >  1u?*((
int*) _temp964) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1025: _temp1024=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp964)->f1; goto _LL1023; _LL1023: _temp1022=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp964)->f2; goto _LL1021; _LL1021: _temp1020=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp964)->f3; goto _LL977;} else{ goto _LL978;} _LL978: if(( unsigned int)
_temp964 >  1u?*(( int*) _temp964) ==  Cyc_Absyn_While_s: 0){ _LL1029: _temp1028=((
struct Cyc_Absyn_While_s_struct*) _temp964)->f1; _LL1033: _temp1032= _temp1028.f1;
goto _LL1031; _LL1031: _temp1030= _temp1028.f2; goto _LL1027; _LL1027: _temp1026=((
struct Cyc_Absyn_While_s_struct*) _temp964)->f2; goto _LL979;} else{ goto _LL980;}
_LL980: if(( unsigned int) _temp964 >  1u?*(( int*) _temp964) ==  Cyc_Absyn_Do_s:
0){ _LL1041: _temp1040=(( struct Cyc_Absyn_Do_s_struct*) _temp964)->f1; goto
_LL1035; _LL1035: _temp1034=(( struct Cyc_Absyn_Do_s_struct*) _temp964)->f2;
_LL1039: _temp1038= _temp1034.f1; goto _LL1037; _LL1037: _temp1036= _temp1034.f2;
goto _LL981;} else{ goto _LL982;} _LL982: if(( unsigned int) _temp964 >  1u?*((
int*) _temp964) ==  Cyc_Absyn_For_s: 0){ _LL1057: _temp1056=(( struct Cyc_Absyn_For_s_struct*)
_temp964)->f1; goto _LL1051; _LL1051: _temp1050=(( struct Cyc_Absyn_For_s_struct*)
_temp964)->f2; _LL1055: _temp1054= _temp1050.f1; goto _LL1053; _LL1053:
_temp1052= _temp1050.f2; goto _LL1045; _LL1045: _temp1044=(( struct Cyc_Absyn_For_s_struct*)
_temp964)->f3; _LL1049: _temp1048= _temp1044.f1; goto _LL1047; _LL1047:
_temp1046= _temp1044.f2; goto _LL1043; _LL1043: _temp1042=(( struct Cyc_Absyn_For_s_struct*)
_temp964)->f4; goto _LL983;} else{ goto _LL984;} _LL984: if(( unsigned int)
_temp964 >  1u?*(( int*) _temp964) ==  Cyc_Absyn_Break_s: 0){ _LL1059: _temp1058=((
struct Cyc_Absyn_Break_s_struct*) _temp964)->f1; if( _temp1058 ==  0){ goto
_LL985;} else{ goto _LL986;}} else{ goto _LL986;} _LL986: if(( unsigned int)
_temp964 >  1u?*(( int*) _temp964) ==  Cyc_Absyn_Fallthru_s: 0){ _LL1064:
_temp1063=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp964)->f1; goto _LL1061;
_LL1061: _temp1060=(( struct Cyc_Absyn_Fallthru_s_struct*) _temp964)->f2; if(
_temp1060 ==  0){ goto _LL988;} else{ _temp1062=* _temp1060; goto _LL987;}}
else{ goto _LL988;} _LL988: if(( unsigned int) _temp964 >  1u?*(( int*) _temp964)
==  Cyc_Absyn_Break_s: 0){ _LL1066: _temp1065=(( struct Cyc_Absyn_Break_s_struct*)
_temp964)->f1; goto _LL989;} else{ goto _LL990;} _LL990: if(( unsigned int)
_temp964 >  1u?*(( int*) _temp964) ==  Cyc_Absyn_Continue_s: 0){ _LL1068:
_temp1067=(( struct Cyc_Absyn_Continue_s_struct*) _temp964)->f1; goto _LL991;}
else{ goto _LL992;} _LL992: if(( unsigned int) _temp964 >  1u?*(( int*) _temp964)
==  Cyc_Absyn_Goto_s: 0){ _LL1070: _temp1069=(( struct Cyc_Absyn_Goto_s_struct*)
_temp964)->f2; goto _LL993;} else{ goto _LL994;} _LL994: if(( unsigned int)
_temp964 >  1u?*(( int*) _temp964) ==  Cyc_Absyn_Switch_s: 0){ _LL1074:
_temp1073=(( struct Cyc_Absyn_Switch_s_struct*) _temp964)->f1; goto _LL1072;
_LL1072: _temp1071=(( struct Cyc_Absyn_Switch_s_struct*) _temp964)->f2; goto
_LL995;} else{ goto _LL996;} _LL996: if(( unsigned int) _temp964 >  1u?*(( int*)
_temp964) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1078: _temp1077=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp964)->f1; goto _LL1076; _LL1076: _temp1075=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp964)->f2; goto _LL997;} else{ goto _LL998;} _LL998: if(( unsigned int)
_temp964 >  1u?*(( int*) _temp964) ==  Cyc_Absyn_Decl_s: 0){ _LL1082: _temp1081=((
struct Cyc_Absyn_Decl_s_struct*) _temp964)->f1; _temp1083=* _temp1081; _LL1085:
_temp1084=( void*) _temp1083.r; if(*(( int*) _temp1084) ==  Cyc_Absyn_Var_d){
_LL1087: _temp1086=(( struct Cyc_Absyn_Var_d_struct*) _temp1084)->f1; goto
_LL1080;} else{ goto _LL1000;} _LL1080: _temp1079=(( struct Cyc_Absyn_Decl_s_struct*)
_temp964)->f2; goto _LL999;} else{ goto _LL1000;} _LL1000: if(( unsigned int)
_temp964 >  1u?*(( int*) _temp964) ==  Cyc_Absyn_Decl_s: 0){ _LL1091: _temp1090=((
struct Cyc_Absyn_Decl_s_struct*) _temp964)->f1; _temp1092=* _temp1090; _LL1094:
_temp1093=( void*) _temp1092.r; if(*(( int*) _temp1093) ==  Cyc_Absyn_Let_d){
_LL1098: _temp1097=(( struct Cyc_Absyn_Let_d_struct*) _temp1093)->f2; if(
_temp1097 ==  0){ goto _LL1002;} else{ _temp1099=* _temp1097; _LL1101: _temp1100=(
struct Cyc_List_List*) _temp1099.v; goto _LL1096;} _LL1096: _temp1095=(( struct
Cyc_Absyn_Let_d_struct*) _temp1093)->f4; goto _LL1089;} else{ goto _LL1002;}
_LL1089: _temp1088=(( struct Cyc_Absyn_Decl_s_struct*) _temp964)->f2; goto
_LL1001;} else{ goto _LL1002;} _LL1002: if(( unsigned int) _temp964 >  1u?*((
int*) _temp964) ==  Cyc_Absyn_Decl_s: 0){ _LL1105: _temp1104=(( struct Cyc_Absyn_Decl_s_struct*)
_temp964)->f1; _temp1106=* _temp1104; _LL1108: _temp1107=( void*) _temp1106.r;
if(*(( int*) _temp1107) ==  Cyc_Absyn_Letv_d){ _LL1110: _temp1109=(( struct Cyc_Absyn_Letv_d_struct*)
_temp1107)->f1; goto _LL1103;} else{ goto _LL1004;} _LL1103: _temp1102=(( struct
Cyc_Absyn_Decl_s_struct*) _temp964)->f2; goto _LL1003;} else{ goto _LL1004;}
_LL1004: if(( unsigned int) _temp964 >  1u?*(( int*) _temp964) ==  Cyc_Absyn_Label_s:
0){ _LL1112: _temp1111=(( struct Cyc_Absyn_Label_s_struct*) _temp964)->f2; goto
_LL1005;} else{ goto _LL1006;} _LL1006: if(( unsigned int) _temp964 >  1u?*((
int*) _temp964) ==  Cyc_Absyn_Region_s: 0){ _LL1118: _temp1117=(( struct Cyc_Absyn_Region_s_struct*)
_temp964)->f1; goto _LL1116; _LL1116: _temp1115=(( struct Cyc_Absyn_Region_s_struct*)
_temp964)->f2; goto _LL1114; _LL1114: _temp1113=(( struct Cyc_Absyn_Region_s_struct*)
_temp964)->f3; goto _LL1007;} else{ goto _LL1008;} _LL1008: goto _LL1009; _LL967:
return in_flow; _LL969: return( void*) Cyc_CfFlowInfo_BottomFL; _LL971: Cyc_CfAbsexp_eval_absexp(
env,(( struct Cyc_Absyn_Exp*) _check_null( _temp1012))->loc,( void*)(( struct
Cyc_List_List*) _check_null( _temp963))->hd, in_flow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL973: return Cyc_CfAbsexp_eval_absexp( env, _temp1014->loc,( void*)(( struct
Cyc_List_List*) _check_null( _temp963))->hd, in_flow); _LL975: return Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1016, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1018, in_flow));
_LL977: { void* _temp1121; void* _temp1123; struct _tuple0 _temp1119= Cyc_NewControlFlow_cf_evalguard(
env, _temp1024->loc, _temp963, in_flow); _LL1124: _temp1123= _temp1119.f1; goto
_LL1122; _LL1122: _temp1121= _temp1119.f2; goto _LL1120; _LL1120: return Cyc_CfFlowInfo_join_flow(
env->all_changed, Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1022, _temp1123),
Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1020, _temp1121));} _LL979: { void*
_temp1125=( void*)( Cyc_NewControlFlow_pre_stmt_check( env, in_flow, _temp1030))->flow;
void* _temp1128; void* _temp1130; struct _tuple0 _temp1126= Cyc_NewControlFlow_cf_evalguard(
env, _temp1032->loc, _temp963, _temp1125); _LL1131: _temp1130= _temp1126.f1;
goto _LL1129; _LL1129: _temp1128= _temp1126.f2; goto _LL1127; _LL1127: { void*
_temp1132= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1026, _temp1130); Cyc_NewControlFlow_update_flow(
env, _temp1030, _temp1132); return _temp1128;}} _LL981: { void* _temp1133= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1040, in_flow); void* _temp1134=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1133, _temp1036))->flow; void* _temp1137; void* _temp1139; struct
_tuple0 _temp1135= Cyc_NewControlFlow_cf_evalguard( env, _temp1038->loc,
_temp963, _temp1134); _LL1140: _temp1139= _temp1135.f1; goto _LL1138; _LL1138:
_temp1137= _temp1135.f2; goto _LL1136; _LL1136: Cyc_NewControlFlow_update_flow(
env, _temp1040, _temp1139); return _temp1137;} _LL983: { void* _temp1141= Cyc_CfAbsexp_eval_absexp(
env, _temp1056->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp963))->hd,
in_flow); void* _temp1142=( void*)( Cyc_NewControlFlow_pre_stmt_check( env,
_temp1141, _temp1052))->flow; void* _temp1145; void* _temp1147; struct _tuple0
_temp1143= Cyc_NewControlFlow_cf_evalguard( env, _temp1054->loc,(( struct Cyc_List_List*)
_check_null( _temp963))->tl, _temp1142); _LL1148: _temp1147= _temp1143.f1; goto
_LL1146; _LL1146: _temp1145= _temp1143.f2; goto _LL1144; _LL1144: { void*
_temp1149= Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1042, _temp1147); void*
_temp1150=( void*)( Cyc_NewControlFlow_pre_stmt_check( env, _temp1149, _temp1046))->flow;
void* _temp1151= Cyc_CfAbsexp_eval_absexp( env, _temp1048->loc,( void*)(( struct
Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp963))->tl))->tl))->tl))->hd,
_temp1150); Cyc_NewControlFlow_update_flow( env, _temp1052, _temp1151); return
_temp1145;}} _LL985: return( void*) Cyc_CfFlowInfo_BottomFL; _LL987: { void*
_temp1152= Cyc_CfAbsexp_eval_absexp( env, s->loc,( void*)(( struct Cyc_List_List*)
_check_null( _temp963))->hd, in_flow); struct Cyc_List_List* _temp1153=( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1062->pat_vars))->v;
_temp1152= Cyc_NewControlFlow_add_vardecls_flow( env, _temp1152, _temp1153);
_temp1152= Cyc_NewControlFlow_add_init_vars_flow( env, _temp1152, _temp1153);
Cyc_NewControlFlow_update_flow( env,( struct Cyc_Absyn_Stmt*) _temp1062->body,
_temp1152); return( void*) Cyc_CfFlowInfo_BottomFL;} _LL989: _temp1067=
_temp1065; goto _LL991; _LL991: _temp1069= _temp1067; goto _LL993; _LL993: Cyc_NewControlFlow_update_flow(
env,( struct Cyc_Absyn_Stmt*) _check_null( _temp1069), in_flow); return( void*)
Cyc_CfFlowInfo_BottomFL; _LL995: return Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1071,(( struct Cyc_List_List*) _check_null( _temp963))->tl, Cyc_CfAbsexp_eval_absexp(
env, _temp1073->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp963))->hd,
in_flow)); _LL997: { struct Cyc_NewControlFlow_AnalEnv _temp1156; void*
_temp1157; int _temp1159; struct Cyc_NewControlFlow_AnalEnv* _temp1154= env;
_temp1156=* _temp1154; _LL1160: _temp1159= _temp1156.in_try; goto _LL1158;
_LL1158: _temp1157=( void*) _temp1156.tryflow; goto _LL1155; _LL1155: env->in_try=
1;( void*)( env->tryflow=( void*) in_flow);{ void* _temp1161= Cyc_NewControlFlow_cf_analyze_stmt(
env, _temp1077, in_flow); void* _temp1162=( void*) env->tryflow; env->in_try=
_temp1159;( void*)( env->tryflow=( void*) _temp1157); Cyc_NewControlFlow_update_tryflow(
env, _temp1162);{ void* _temp1163= Cyc_NewControlFlow_cf_analyze_switch_clauses(
env, _temp1075, _temp963, _temp1162); return Cyc_CfFlowInfo_join_flow( env->all_changed,
_temp1161, _temp1163);}}} _LL999: { struct Cyc_List_List _temp1164=({ struct Cyc_List_List
_temp1167; _temp1167.hd=( void*) _temp1086; _temp1167.tl= 0; _temp1167;}); void*
_temp1165= Cyc_NewControlFlow_add_vardecls_flow( env, in_flow,( struct Cyc_List_List*)&
_temp1164); struct Cyc_Absyn_Exp* _temp1166= _temp1086->initializer; if(
_temp1166 !=  0){ _temp1165= Cyc_CfAbsexp_eval_absexp( env,(( struct Cyc_Absyn_Exp*)
_check_null( _temp1166))->loc,( void*)(( struct Cyc_List_List*) _check_null(
_temp963))->hd, _temp1165);} return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1079, _temp1165);} _LL1001: { void* _temp1168= Cyc_CfAbsexp_eval_absexp(
env, _temp1095->loc,( void*)(( struct Cyc_List_List*) _check_null( _temp963))->hd,
Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, _temp1100)); _temp1168= Cyc_NewControlFlow_add_init_vars_flow(
env, _temp1168, _temp1100); return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1088, _temp1168);} _LL1003: return Cyc_NewControlFlow_cf_analyze_stmt( env,
_temp1102, Cyc_NewControlFlow_add_vardecls_flow( env, in_flow, _temp1109));
_LL1005: return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1111, in_flow);
_LL1007: { struct Cyc_List_List* _temp1169=({ struct Cyc_List_List* _temp1171=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1171->hd=(
void*) _temp1115; _temp1171->tl= 0; _temp1171;}); void* _temp1170= Cyc_NewControlFlow_add_vardecls_flow(
env, in_flow, _temp1169); _temp1170= Cyc_NewControlFlow_add_init_vars_flow( env,
_temp1170, _temp1169); return Cyc_NewControlFlow_cf_analyze_stmt( env, _temp1113,
_temp1170);} _LL1009:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1172=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1172[ 0]=({ struct Cyc_Core_Impossible_struct _temp1173; _temp1173.tag= Cyc_Core_Impossible;
_temp1173.f1= _tag_arr("cf_analyze_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof( unsigned char), 62u); _temp1173;}); _temp1172;})); _LL965:;}} void Cyc_NewControlFlow_cf_postpass(
struct Cyc_Absyn_Stmt* s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{
void* _temp1174=( void*) s->r; struct Cyc_Absyn_Stmt* _temp1198; struct Cyc_Absyn_Stmt*
_temp1200; struct Cyc_Absyn_Stmt* _temp1202; struct Cyc_Absyn_Stmt* _temp1204;
struct Cyc_Absyn_Stmt* _temp1206; struct _tuple3 _temp1208; struct Cyc_Absyn_Stmt*
_temp1210; struct _tuple3 _temp1212; struct Cyc_Absyn_Stmt* _temp1214; struct
Cyc_Absyn_Stmt* _temp1216; struct _tuple3 _temp1218; struct Cyc_Absyn_Stmt*
_temp1220; struct _tuple3 _temp1222; struct Cyc_Absyn_Stmt* _temp1224; struct
Cyc_Absyn_Stmt* _temp1226; struct Cyc_Absyn_Stmt* _temp1228; struct Cyc_Absyn_Stmt*
_temp1230; struct Cyc_Absyn_Stmt* _temp1232; struct Cyc_List_List* _temp1234;
struct Cyc_Absyn_Stmt* _temp1236; struct Cyc_List_List* _temp1238; _LL1176: if((
unsigned int) _temp1174 >  1u?*(( int*) _temp1174) ==  Cyc_Absyn_Seq_s: 0){
_LL1201: _temp1200=(( struct Cyc_Absyn_Seq_s_struct*) _temp1174)->f1; goto
_LL1199; _LL1199: _temp1198=(( struct Cyc_Absyn_Seq_s_struct*) _temp1174)->f2;
goto _LL1177;} else{ goto _LL1178;} _LL1178: if(( unsigned int) _temp1174 >  1u?*((
int*) _temp1174) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1205: _temp1204=(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp1174)->f2; goto _LL1203; _LL1203: _temp1202=((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1174)->f3; goto _LL1179;} else{ goto
_LL1180;} _LL1180: if(( unsigned int) _temp1174 >  1u?*(( int*) _temp1174) == 
Cyc_Absyn_For_s: 0){ _LL1213: _temp1212=(( struct Cyc_Absyn_For_s_struct*)
_temp1174)->f2; _LL1215: _temp1214= _temp1212.f2; goto _LL1209; _LL1209:
_temp1208=(( struct Cyc_Absyn_For_s_struct*) _temp1174)->f3; _LL1211: _temp1210=
_temp1208.f2; goto _LL1207; _LL1207: _temp1206=(( struct Cyc_Absyn_For_s_struct*)
_temp1174)->f4; goto _LL1181;} else{ goto _LL1182;} _LL1182: if(( unsigned int)
_temp1174 >  1u?*(( int*) _temp1174) ==  Cyc_Absyn_While_s: 0){ _LL1219:
_temp1218=(( struct Cyc_Absyn_While_s_struct*) _temp1174)->f1; _LL1221:
_temp1220= _temp1218.f2; goto _LL1217; _LL1217: _temp1216=(( struct Cyc_Absyn_While_s_struct*)
_temp1174)->f2; goto _LL1183;} else{ goto _LL1184;} _LL1184: if(( unsigned int)
_temp1174 >  1u?*(( int*) _temp1174) ==  Cyc_Absyn_Do_s: 0){ _LL1227: _temp1226=((
struct Cyc_Absyn_Do_s_struct*) _temp1174)->f1; goto _LL1223; _LL1223: _temp1222=((
struct Cyc_Absyn_Do_s_struct*) _temp1174)->f2; _LL1225: _temp1224= _temp1222.f2;
goto _LL1185;} else{ goto _LL1186;} _LL1186: if(( unsigned int) _temp1174 >  1u?*((
int*) _temp1174) ==  Cyc_Absyn_Region_s: 0){ _LL1229: _temp1228=(( struct Cyc_Absyn_Region_s_struct*)
_temp1174)->f3; goto _LL1187;} else{ goto _LL1188;} _LL1188: if(( unsigned int)
_temp1174 >  1u?*(( int*) _temp1174) ==  Cyc_Absyn_Label_s: 0){ _LL1231:
_temp1230=(( struct Cyc_Absyn_Label_s_struct*) _temp1174)->f2; goto _LL1189;}
else{ goto _LL1190;} _LL1190: if(( unsigned int) _temp1174 >  1u?*(( int*)
_temp1174) ==  Cyc_Absyn_Decl_s: 0){ _LL1233: _temp1232=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1174)->f2; goto _LL1191;} else{ goto _LL1192;} _LL1192: if(( unsigned int)
_temp1174 >  1u?*(( int*) _temp1174) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1237:
_temp1236=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1174)->f1; goto _LL1235;
_LL1235: _temp1234=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1174)->f2; goto
_LL1193;} else{ goto _LL1194;} _LL1194: if(( unsigned int) _temp1174 >  1u?*((
int*) _temp1174) ==  Cyc_Absyn_Switch_s: 0){ _LL1239: _temp1238=(( struct Cyc_Absyn_Switch_s_struct*)
_temp1174)->f2; goto _LL1195;} else{ goto _LL1196;} _LL1196: goto _LL1197;
_LL1177: _temp1204= _temp1200; _temp1202= _temp1198; goto _LL1179; _LL1179: Cyc_NewControlFlow_cf_postpass(
_temp1204); Cyc_NewControlFlow_cf_postpass( _temp1202); return; _LL1181: Cyc_NewControlFlow_cf_postpass(
_temp1210); _temp1220= _temp1206; _temp1216= _temp1214; goto _LL1183; _LL1183:
_temp1226= _temp1216; _temp1224= _temp1220; goto _LL1185; _LL1185: Cyc_NewControlFlow_cf_postpass(
_temp1224); _temp1228= _temp1226; goto _LL1187; _LL1187: _temp1230= _temp1228;
goto _LL1189; _LL1189: _temp1232= _temp1230; goto _LL1191; _LL1191: Cyc_NewControlFlow_cf_postpass(
_temp1232); return; _LL1193: Cyc_NewControlFlow_cf_postpass( _temp1236);
_temp1238= _temp1234; goto _LL1195; _LL1195: for( 0; _temp1238 !=  0; _temp1238=((
struct Cyc_List_List*) _check_null( _temp1238))->tl){ Cyc_NewControlFlow_cf_postpass(((
struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null( _temp1238))->hd)->body);}
return; _LL1197: return; _LL1175:;}} void Cyc_NewControlFlow_cf_check_fun(
struct Cyc_Absyn_Fndecl* fd){ struct Cyc_NewControlFlow_AbsEnv absenv=({ struct
Cyc_NewControlFlow_AbsEnv _temp1261; _temp1261.shared=({ struct Cyc_NewControlFlow_SharedAbsEnv*
_temp1262=( struct Cyc_NewControlFlow_SharedAbsEnv*) _cycalloc( sizeof( struct
Cyc_NewControlFlow_SharedAbsEnv)); _temp1262->all_roots= Cyc_Dict_empty( Cyc_CfFlowInfo_local_root_cmp);
_temp1262->comprehension_vars= 0; _temp1262;}); _temp1261.lhsides= 0; _temp1261.access_path=
0; _temp1261;}); struct Cyc_List_List* _temp1240=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( fd->param_vardecls))->v;{ struct Cyc_List_List*
vds= _temp1240; for( 0; vds !=  0; vds=(( struct Cyc_List_List*) _check_null(
vds))->tl){ Cyc_NewControlFlow_add_var_root( absenv,( struct Cyc_Absyn_Vardecl*)((
struct Cyc_List_List*) _check_null( vds))->hd);}} Cyc_NewControlFlow_cf_prepass(
absenv, fd->body);{ struct Cyc_NewControlFlow_SharedAbsEnv* _temp1243; struct
Cyc_NewControlFlow_SharedAbsEnv _temp1245; struct Cyc_List_List* _temp1246;
struct Cyc_Dict_Dict* _temp1248; struct Cyc_NewControlFlow_AbsEnv _temp1241=
absenv; _LL1244: _temp1243= _temp1241.shared; _temp1245=* _temp1243; _LL1249:
_temp1248= _temp1245.all_roots; goto _LL1247; _LL1247: _temp1246= _temp1245.comprehension_vars;
goto _LL1242; _LL1242: { struct Cyc_NewControlFlow_AnalEnv* _temp1250=({ struct
Cyc_NewControlFlow_AnalEnv* _temp1260=( struct Cyc_NewControlFlow_AnalEnv*)
_cycalloc( sizeof( struct Cyc_NewControlFlow_AnalEnv)); _temp1260->roots=
_temp1248; _temp1260->in_try= 0; _temp1260->tryflow=( void*)(( void*) Cyc_CfFlowInfo_BottomFL);
_temp1260->all_changed= 0; _temp1260;}); void* in_flow=( void*)({ struct Cyc_CfFlowInfo_InitsFL_struct*
_temp1258=( struct Cyc_CfFlowInfo_InitsFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_InitsFL_struct));
_temp1258[ 0]=({ struct Cyc_CfFlowInfo_InitsFL_struct _temp1259; _temp1259.tag=
Cyc_CfFlowInfo_InitsFL; _temp1259.f1= Cyc_Dict_empty( Cyc_CfFlowInfo_local_root_cmp);
_temp1259;}); _temp1258;}); in_flow= Cyc_NewControlFlow_add_init_vars_flow(
_temp1250, in_flow, _temp1240); in_flow= Cyc_NewControlFlow_add_vardecls_flow(
_temp1250, in_flow, _temp1246);{ void* out_flow= in_flow; Cyc_NewControlFlow_iteration_num=
0; Cyc_NewControlFlow_iterate_cf_check= 1; while( Cyc_NewControlFlow_iterate_cf_check) {
++ Cyc_NewControlFlow_iteration_num; Cyc_NewControlFlow_iterate_cf_check= 0;
out_flow= Cyc_NewControlFlow_cf_analyze_stmt( _temp1250, fd->body, in_flow);}{
void* _temp1251= out_flow; _LL1253: if( _temp1251 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1254;} else{ goto _LL1255;} _LL1255: goto _LL1256; _LL1254: goto _LL1252;
_LL1256: if(( void*) fd->ret_type != ( void*) Cyc_Absyn_VoidType){({ void*
_temp1257[ 0u]={}; Cyc_Tcutil_terr(( fd->body)->loc, _tag_arr("function may complete without returning a value",
sizeof( unsigned char), 48u), _tag_arr( _temp1257, sizeof( void*), 0u));});}
goto _LL1252; _LL1252:;} Cyc_NewControlFlow_cf_postpass( fd->body);}}}} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds !=  0; ds=(( struct Cyc_List_List*)
_check_null( ds))->tl){ void* _temp1263=( void*)(( struct Cyc_Absyn_Decl*)((
struct Cyc_List_List*) _check_null( ds))->hd)->r; struct Cyc_Absyn_Fndecl*
_temp1275; struct Cyc_List_List* _temp1277; struct Cyc_List_List* _temp1279;
struct Cyc_List_List* _temp1281; _LL1265: if(*(( int*) _temp1263) ==  Cyc_Absyn_Fn_d){
_LL1276: _temp1275=(( struct Cyc_Absyn_Fn_d_struct*) _temp1263)->f1; goto
_LL1266;} else{ goto _LL1267;} _LL1267: if(*(( int*) _temp1263) ==  Cyc_Absyn_ExternC_d){
_LL1278: _temp1277=(( struct Cyc_Absyn_ExternC_d_struct*) _temp1263)->f1; goto
_LL1268;} else{ goto _LL1269;} _LL1269: if(*(( int*) _temp1263) ==  Cyc_Absyn_Using_d){
_LL1280: _temp1279=(( struct Cyc_Absyn_Using_d_struct*) _temp1263)->f2; goto
_LL1270;} else{ goto _LL1271;} _LL1271: if(*(( int*) _temp1263) ==  Cyc_Absyn_Namespace_d){
_LL1282: _temp1281=(( struct Cyc_Absyn_Namespace_d_struct*) _temp1263)->f2; goto
_LL1272;} else{ goto _LL1273;} _LL1273: goto _LL1274; _LL1266: Cyc_NewControlFlow_cf_check_fun(
_temp1275); goto _LL1264; _LL1268: _temp1279= _temp1277; goto _LL1270; _LL1270:
_temp1281= _temp1279; goto _LL1272; _LL1272: Cyc_NewControlFlow_cf_check(
_temp1281); goto _LL1264; _LL1274: goto _LL1264; _LL1264:;}}
