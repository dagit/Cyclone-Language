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
ae; struct Cyc_Core_Opt* le; } ; extern void* Cyc_Tcenv_lookup_ordinary( struct
Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern struct
Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct
_tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_rsubstitute(
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
te, p->loc, _temp10); struct Cyc_Absyn_Tunionfield* _temp37; struct Cyc_Absyn_Tuniondecl*
_temp39; struct Cyc_Absyn_Enumfield* _temp41; struct Cyc_Absyn_Enumdecl* _temp43;
_LL29: if(*(( int*) _temp27) ==  Cyc_Tcenv_StructRes){ goto _LL30;} else{ goto
_LL31;} _LL31: if(*(( int*) _temp27) ==  Cyc_Tcenv_TunionRes){ _LL40: _temp39=((
struct Cyc_Tcenv_TunionRes_struct*) _temp27)->f1; goto _LL38; _LL38: _temp37=((
struct Cyc_Tcenv_TunionRes_struct*) _temp27)->f2; goto _LL32;} else{ goto _LL33;}
_LL33: if(*(( int*) _temp27) ==  Cyc_Tcenv_EnumRes){ _LL44: _temp43=(( struct
Cyc_Tcenv_EnumRes_struct*) _temp27)->f1; goto _LL42; _LL42: _temp41=(( struct
Cyc_Tcenv_EnumRes_struct*) _temp27)->f2; goto _LL34;} else{ goto _LL35;} _LL35:
if(*(( int*) _temp27) ==  Cyc_Tcenv_VarRes){ goto _LL36;} else{ goto _LL28;}
_LL30:({ void* _temp45[ 0u]={}; Cyc_Tcutil_terr( p->loc, _tag_arr("struct tag used without arguments in pattern",
sizeof( unsigned char), 45u), _tag_arr( _temp45, sizeof( void*), 0u));});
_npop_handler( 0u); return; _LL32:( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct*
_temp46=( struct Cyc_Absyn_Tunion_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_p_struct));
_temp46[ 0]=({ struct Cyc_Absyn_Tunion_p_struct _temp47; _temp47.tag= Cyc_Absyn_Tunion_p;
_temp47.f1= _temp39; _temp47.f2= _temp37; _temp47.f3= 0; _temp47.f4= 0; _temp47;});
_temp46;}))); _npop_handler( 0u); return; _LL34:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Enum_p_struct* _temp48=( struct Cyc_Absyn_Enum_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Enum_p_struct)); _temp48[ 0]=({ struct Cyc_Absyn_Enum_p_struct
_temp49; _temp49.tag= Cyc_Absyn_Enum_p; _temp49.f1= _temp43; _temp49.f2= _temp41;
_temp49;}); _temp48;}))); _npop_handler( 0u); return; _LL36: goto _LL28; _LL28:;};
_pop_handler();} else{ void* _temp25=( void*) _exn_thrown; void* _temp51=
_temp25; _LL53: if( _temp51 ==  Cyc_Dict_Absent){ goto _LL54;} else{ goto _LL55;}
_LL55: goto _LL56; _LL54: goto _LL52; _LL56:( void) _throw( _temp51); _LL52:;}}}{
void* _temp57=(* _temp10).f1; struct Cyc_List_List* _temp65; _LL59: if( _temp57
== ( void*) Cyc_Absyn_Loc_n){ goto _LL60;} else{ goto _LL61;} _LL61: if((
unsigned int) _temp57 >  1u?*(( int*) _temp57) ==  Cyc_Absyn_Rel_n: 0){ _LL66:
_temp65=(( struct Cyc_Absyn_Rel_n_struct*) _temp57)->f1; if( _temp65 ==  0){
goto _LL62;} else{ goto _LL63;}} else{ goto _LL63;} _LL63: goto _LL64; _LL60:
goto _LL62; _LL62:(* _temp10).f1=( void*) Cyc_Absyn_Loc_n;( void*)( p->r=( void*)((
void*)({ struct Cyc_Absyn_Var_p_struct* _temp67=( struct Cyc_Absyn_Var_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_p_struct)); _temp67[ 0]=({ struct Cyc_Absyn_Var_p_struct
_temp68; _temp68.tag= Cyc_Absyn_Var_p; _temp68.f1= Cyc_Absyn_new_vardecl(
_temp10,( void*) Cyc_Absyn_VoidType, 0); _temp68;}); _temp67;}))); return; _LL64:({
void* _temp69[ 0u]={}; Cyc_Tcutil_terr( p->loc, _tag_arr("qualified variable in pattern",
sizeof( unsigned char), 30u), _tag_arr( _temp69, sizeof( void*), 0u));});
return; _LL58:;} _LL5:{ struct _handler_cons _temp70; _push_handler(& _temp70);{
int _temp72= 0; if( setjmp( _temp70.handler)){ _temp72= 1;} if( ! _temp72){{
void* _temp73= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp16); struct Cyc_Absyn_Structdecl*
_temp83; struct Cyc_Absyn_Tunionfield* _temp85; struct Cyc_Absyn_Tuniondecl*
_temp87; _LL75: if(*(( int*) _temp73) ==  Cyc_Tcenv_StructRes){ _LL84: _temp83=((
struct Cyc_Tcenv_StructRes_struct*) _temp73)->f1; goto _LL76;} else{ goto _LL77;}
_LL77: if(*(( int*) _temp73) ==  Cyc_Tcenv_TunionRes){ _LL88: _temp87=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp73)->f1; goto _LL86; _LL86: _temp85=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp73)->f2; goto _LL78;} else{ goto _LL79;} _LL79:
if(*(( int*) _temp73) ==  Cyc_Tcenv_EnumRes){ goto _LL80;} else{ goto _LL81;}
_LL81: if(*(( int*) _temp73) ==  Cyc_Tcenv_VarRes){ goto _LL82;} else{ goto
_LL74;} _LL76: { struct Cyc_List_List* _temp89= 0; for( 0; _temp12 !=  0;
_temp12=(( struct Cyc_List_List*) _check_null( _temp12))->tl){ _temp89=({ struct
Cyc_List_List* _temp90=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp90->hd=( void*)({ struct _tuple7* _temp91=( struct _tuple7*) _cycalloc(
sizeof( struct _tuple7)); _temp91->f1= 0; _temp91->f2=( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp12))->hd; _temp91;}); _temp90->tl=
_temp89; _temp90;});}( void*)( p->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_p_struct*
_temp92=( struct Cyc_Absyn_Struct_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_p_struct));
_temp92[ 0]=({ struct Cyc_Absyn_Struct_p_struct _temp93; _temp93.tag= Cyc_Absyn_Struct_p;
_temp93.f1= _temp83; _temp93.f2= 0; _temp93.f3= _temp14; _temp93.f4=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp89);
_temp93;}); _temp92;}))); _npop_handler( 0u); return;} _LL78:( void*)( p->r=(
void*)(( void*)({ struct Cyc_Absyn_Tunion_p_struct* _temp94=( struct Cyc_Absyn_Tunion_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunion_p_struct)); _temp94[ 0]=({ struct Cyc_Absyn_Tunion_p_struct
_temp95; _temp95.tag= Cyc_Absyn_Tunion_p; _temp95.f1= _temp87; _temp95.f2=
_temp85; _temp95.f3= _temp14; _temp95.f4= _temp12; _temp95;}); _temp94;})));
_npop_handler( 0u); return; _LL80:({ struct Cyc_Std_String_pa_struct _temp97;
_temp97.tag= Cyc_Std_String_pa; _temp97.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp16);{ void* _temp96[ 1u]={& _temp97}; Cyc_Tcutil_terr( p->loc, _tag_arr("bad enum constructor %s in pattern",
sizeof( unsigned char), 35u), _tag_arr( _temp96, sizeof( void*), 1u));}});
_npop_handler( 0u); return; _LL82: goto _LL74; _LL74:;}; _pop_handler();} else{
void* _temp71=( void*) _exn_thrown; void* _temp99= _temp71; _LL101: if( _temp99
==  Cyc_Dict_Absent){ goto _LL102;} else{ goto _LL103;} _LL103: goto _LL104;
_LL102: goto _LL100; _LL104:( void) _throw( _temp99); _LL100:;}}}({ struct Cyc_Std_String_pa_struct
_temp106; _temp106.tag= Cyc_Std_String_pa; _temp106.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp16);{ void* _temp105[ 1u]={& _temp106}; Cyc_Tcutil_terr( p->loc, _tag_arr("%s is not a constructor in pattern",
sizeof( unsigned char), 35u), _tag_arr( _temp105, sizeof( void*), 1u));}});
return; _LL7:{ struct _handler_cons _temp107; _push_handler(& _temp107);{ int
_temp109= 0; if( setjmp( _temp107.handler)){ _temp109= 1;} if( ! _temp109){{
void* _temp110= Cyc_Tcenv_lookup_ordinary( te, p->loc, _temp22); struct Cyc_Absyn_Structdecl*
_temp116; _LL112: if(*(( int*) _temp110) ==  Cyc_Tcenv_StructRes){ _LL117:
_temp116=(( struct Cyc_Tcenv_StructRes_struct*) _temp110)->f1; goto _LL113;}
else{ goto _LL114;} _LL114: goto _LL115; _LL113:( void*)( p->r=( void*)(( void*)({
struct Cyc_Absyn_Struct_p_struct* _temp118=( struct Cyc_Absyn_Struct_p_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_p_struct)); _temp118[ 0]=({ struct
Cyc_Absyn_Struct_p_struct _temp119; _temp119.tag= Cyc_Absyn_Struct_p; _temp119.f1=
_temp116; _temp119.f2= 0; _temp119.f3= _temp20; _temp119.f4= _temp18; _temp119;});
_temp118;}))); _npop_handler( 0u); return; _LL115: goto _LL111; _LL111:;};
_pop_handler();} else{ void* _temp108=( void*) _exn_thrown; void* _temp121=
_temp108; _LL123: if( _temp121 ==  Cyc_Dict_Absent){ goto _LL124;} else{ goto
_LL125;} _LL125: goto _LL126; _LL124: goto _LL122; _LL126:( void) _throw(
_temp121); _LL122:;}}}({ struct Cyc_Std_String_pa_struct _temp128; _temp128.tag=
Cyc_Std_String_pa; _temp128.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp22);{ void* _temp127[ 1u]={& _temp128}; Cyc_Tcutil_terr( p->loc, _tag_arr("%s is not a struct name, but pattern has designators",
sizeof( unsigned char), 53u), _tag_arr( _temp127, sizeof( void*), 1u));}});
return; _LL9: return; _LL1:;} static struct _tagged_arr* Cyc_Tcpat_get_name(
struct Cyc_Absyn_Vardecl* vd){ return(* vd->name).f2;} static struct Cyc_Core_Opt
Cyc_Tcpat_mk={( void*)(( void*) 1u)}; static struct Cyc_Core_Opt Cyc_Tcpat_rk={(
void*)(( void*) 3u)}; static struct Cyc_Core_Opt Cyc_Tcpat_ak={( void*)(( void*)
0u)}; static void* Cyc_Tcpat_any_type( struct Cyc_List_List* s, void** topt){
if( topt !=  0){ return*(( void**) _check_null( topt));} return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcpat_mk,({ struct Cyc_Core_Opt* _temp129=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp129->v=( void*) s;
_temp129;}));} static void* Cyc_Tcpat_num_type( void** topt, void* numt){ if(
topt !=  0? Cyc_Tcutil_coerceable(*(( void**) _check_null( topt))): 0){ return*((
void**) _check_null( topt));}{ void* _temp130= Cyc_Tcutil_compress( numt);
_LL132: if(( unsigned int) _temp130 >  4u?*(( int*) _temp130) ==  Cyc_Absyn_EnumType:
0){ goto _LL133;} else{ goto _LL134;} _LL134: goto _LL135; _LL133: if( topt != 
0){ return*(( void**) _check_null( topt));} goto _LL131; _LL135: goto _LL131;
_LL131:;} return numt;} static void Cyc_Tcpat_set_vd( struct Cyc_Absyn_Vardecl*
vd, struct Cyc_List_List** v_result_ptr, void* t){( void*)( vd->type=( void*) t);
vd->tq= Cyc_Absyn_empty_tqual();* v_result_ptr=( struct Cyc_List_List*)({ struct
Cyc_List_List* _temp136=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp136->hd=( void*) vd; _temp136->tl=* v_result_ptr; _temp136;});} struct
_tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; struct _tuple9{ struct Cyc_Absyn_Structfield*
f1; struct Cyc_Absyn_Pat* f2; } ; struct _tuple6 Cyc_Tcpat_tcPatRec( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p, void** topt, void** region_opt){ Cyc_Tcpat_resolve_pat(
te, p);{ void* t; struct Cyc_List_List* tv_result= 0; struct Cyc_List_List*
v_result= 0;{ void* _temp137=( void*) p->r; struct Cyc_Absyn_Vardecl* _temp171;
struct Cyc_Absyn_Vardecl* _temp173; void* _temp175; void* _temp177; struct Cyc_Absyn_Enumfield*
_temp179; struct Cyc_Absyn_Enumdecl* _temp181; struct Cyc_Absyn_Pat* _temp183;
struct Cyc_List_List* _temp185; struct Cyc_List_List* _temp187; struct Cyc_List_List*
_temp189; struct Cyc_Core_Opt* _temp191; struct Cyc_Core_Opt** _temp193; struct
Cyc_Absyn_Structdecl* _temp194; struct Cyc_List_List* _temp196; struct Cyc_List_List*
_temp198; struct Cyc_Absyn_Tunionfield* _temp200; struct Cyc_Absyn_Tuniondecl*
_temp202; _LL139: if( _temp137 == ( void*) Cyc_Absyn_Wild_p){ goto _LL140;}
else{ goto _LL141;} _LL141: if(( unsigned int) _temp137 >  2u?*(( int*) _temp137)
==  Cyc_Absyn_Var_p: 0){ _LL172: _temp171=(( struct Cyc_Absyn_Var_p_struct*)
_temp137)->f1; goto _LL142;} else{ goto _LL143;} _LL143: if(( unsigned int)
_temp137 >  2u?*(( int*) _temp137) ==  Cyc_Absyn_Reference_p: 0){ _LL174:
_temp173=(( struct Cyc_Absyn_Reference_p_struct*) _temp137)->f1; goto _LL144;}
else{ goto _LL145;} _LL145: if(( unsigned int) _temp137 >  2u?*(( int*) _temp137)
==  Cyc_Absyn_Int_p: 0){ _LL176: _temp175=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp137)->f1; if( _temp175 == ( void*) Cyc_Absyn_Unsigned){ goto _LL146;} else{
goto _LL147;}} else{ goto _LL147;} _LL147: if(( unsigned int) _temp137 >  2u?*((
int*) _temp137) ==  Cyc_Absyn_Int_p: 0){ _LL178: _temp177=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp137)->f1; if( _temp177 == ( void*) Cyc_Absyn_Signed){ goto _LL148;} else{
goto _LL149;}} else{ goto _LL149;} _LL149: if(( unsigned int) _temp137 >  2u?*((
int*) _temp137) ==  Cyc_Absyn_Char_p: 0){ goto _LL150;} else{ goto _LL151;}
_LL151: if(( unsigned int) _temp137 >  2u?*(( int*) _temp137) ==  Cyc_Absyn_Float_p:
0){ goto _LL152;} else{ goto _LL153;} _LL153: if(( unsigned int) _temp137 >  2u?*((
int*) _temp137) ==  Cyc_Absyn_Enum_p: 0){ _LL182: _temp181=(( struct Cyc_Absyn_Enum_p_struct*)
_temp137)->f1; goto _LL180; _LL180: _temp179=(( struct Cyc_Absyn_Enum_p_struct*)
_temp137)->f2; goto _LL154;} else{ goto _LL155;} _LL155: if( _temp137 == ( void*)
Cyc_Absyn_Null_p){ goto _LL156;} else{ goto _LL157;} _LL157: if(( unsigned int)
_temp137 >  2u?*(( int*) _temp137) ==  Cyc_Absyn_Pointer_p: 0){ _LL184: _temp183=((
struct Cyc_Absyn_Pointer_p_struct*) _temp137)->f1; goto _LL158;} else{ goto
_LL159;} _LL159: if(( unsigned int) _temp137 >  2u?*(( int*) _temp137) ==  Cyc_Absyn_Tuple_p:
0){ _LL186: _temp185=(( struct Cyc_Absyn_Tuple_p_struct*) _temp137)->f1; goto
_LL160;} else{ goto _LL161;} _LL161: if(( unsigned int) _temp137 >  2u?*(( int*)
_temp137) ==  Cyc_Absyn_Struct_p: 0){ _LL195: _temp194=(( struct Cyc_Absyn_Struct_p_struct*)
_temp137)->f1; goto _LL192; _LL192: _temp191=(( struct Cyc_Absyn_Struct_p_struct*)
_temp137)->f2; _temp193=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Struct_p_struct*)
_temp137)->f2; goto _LL190; _LL190: _temp189=(( struct Cyc_Absyn_Struct_p_struct*)
_temp137)->f3; goto _LL188; _LL188: _temp187=(( struct Cyc_Absyn_Struct_p_struct*)
_temp137)->f4; goto _LL162;} else{ goto _LL163;} _LL163: if(( unsigned int)
_temp137 >  2u?*(( int*) _temp137) ==  Cyc_Absyn_Tunion_p: 0){ _LL203: _temp202=((
struct Cyc_Absyn_Tunion_p_struct*) _temp137)->f1; goto _LL201; _LL201: _temp200=((
struct Cyc_Absyn_Tunion_p_struct*) _temp137)->f2; goto _LL199; _LL199: _temp198=((
struct Cyc_Absyn_Tunion_p_struct*) _temp137)->f3; goto _LL197; _LL197: _temp196=((
struct Cyc_Absyn_Tunion_p_struct*) _temp137)->f4; goto _LL164;} else{ goto
_LL165;} _LL165: if(( unsigned int) _temp137 >  2u?*(( int*) _temp137) ==  Cyc_Absyn_UnknownId_p:
0){ goto _LL166;} else{ goto _LL167;} _LL167: if(( unsigned int) _temp137 >  2u?*((
int*) _temp137) ==  Cyc_Absyn_UnknownCall_p: 0){ goto _LL168;} else{ goto _LL169;}
_LL169: if(( unsigned int) _temp137 >  2u?*(( int*) _temp137) ==  Cyc_Absyn_UnknownFields_p:
0){ goto _LL170;} else{ goto _LL138;} _LL140: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars(
te), topt); goto _LL138; _LL142: t= Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars(
te), topt); Cyc_Tcpat_set_vd( _temp171,& v_result, t); goto _LL138; _LL144: t=
Cyc_Tcpat_any_type( Cyc_Tcenv_lookup_type_vars( te), topt); if( region_opt ==  0){({
void* _temp204[ 0u]={}; Cyc_Tcutil_terr( p->loc, _tag_arr("* pattern would point into an unknown region",
sizeof( unsigned char), 45u), _tag_arr( _temp204, sizeof( void*), 0u));}); goto
_LL138;} Cyc_Tcpat_set_vd( _temp173,& v_result,( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp205=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp205[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp206; _temp206.tag= Cyc_Absyn_PointerType;
_temp206.f1=({ struct Cyc_Absyn_PtrInfo _temp207; _temp207.elt_typ=( void*) t;
_temp207.rgn_typ=( void*)*(( void**) _check_null( region_opt)); _temp207.nullable=((
struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)( 0); _temp207.tq= Cyc_Absyn_empty_tqual();
_temp207.bounds= Cyc_Absyn_empty_conref(); _temp207;}); _temp206;}); _temp205;}));
goto _LL138; _LL146: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_uint_t); goto _LL138;
_LL148: t= Cyc_Tcpat_num_type( topt, Cyc_Absyn_sint_t); goto _LL138; _LL150: t=
Cyc_Tcpat_num_type( topt, Cyc_Absyn_uchar_t); goto _LL138; _LL152: t= Cyc_Tcpat_num_type(
topt, Cyc_Absyn_float_t); goto _LL138; _LL154: t= Cyc_Tcpat_num_type( topt,(
void*)({ struct Cyc_Absyn_EnumType_struct* _temp208=( struct Cyc_Absyn_EnumType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp208[ 0]=({ struct
Cyc_Absyn_EnumType_struct _temp209; _temp209.tag= Cyc_Absyn_EnumType; _temp209.f1=
_temp181->name; _temp209.f2=( struct Cyc_Absyn_Enumdecl*) _temp181; _temp209;});
_temp208;})); goto _LL138; _LL156: if( topt !=  0){ void* _temp210= Cyc_Tcutil_compress(*((
void**) _check_null( topt))); _LL212: if(( unsigned int) _temp210 >  4u?*(( int*)
_temp210) ==  Cyc_Absyn_PointerType: 0){ goto _LL213;} else{ goto _LL214;}
_LL214: goto _LL215; _LL213: t=*(( void**) _check_null( topt)); goto tcpat_end;
_LL215: goto _LL211; _LL211:;}{ struct Cyc_List_List* _temp216= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp217=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp217[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp218; _temp218.tag= Cyc_Absyn_PointerType;
_temp218.f1=({ struct Cyc_Absyn_PtrInfo _temp219; _temp219.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcpat_ak,({ struct Cyc_Core_Opt* _temp221=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp221->v=( void*)
_temp216; _temp221;})); _temp219.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcpat_rk,({ struct Cyc_Core_Opt* _temp220=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp220->v=( void*) _temp216;
_temp220;})); _temp219.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp219.tq= Cyc_Absyn_empty_tqual(); _temp219.bounds= Cyc_Absyn_empty_conref();
_temp219;}); _temp218;}); _temp217;}); goto _LL138;} _LL158: { void* inner_typ=(
void*) Cyc_Absyn_VoidType; void** _temp222= 0; if( topt !=  0){ void* _temp223=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_PtrInfo
_temp229; void* _temp231; _LL225: if(( unsigned int) _temp223 >  4u?*(( int*)
_temp223) ==  Cyc_Absyn_PointerType: 0){ _LL230: _temp229=(( struct Cyc_Absyn_PointerType_struct*)
_temp223)->f1; _LL232: _temp231=( void*) _temp229.elt_typ; goto _LL226;} else{
goto _LL227;} _LL227: goto _LL228; _LL226: inner_typ= _temp231; _temp222=( void**)&
inner_typ; goto _LL224; _LL228: goto _LL224; _LL224:;}{ void* ptr_rgn= Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcpat_rk,({ struct Cyc_Core_Opt* _temp267=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp267->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp267;})); struct Cyc_List_List* _temp235;
struct Cyc_List_List* _temp237; struct _tuple6 _temp233= Cyc_Tcpat_tcPatRec( te,
_temp183, _temp222,( void**)& ptr_rgn); _LL238: _temp237= _temp233.f1; goto
_LL236; _LL236: _temp235= _temp233.f2; goto _LL234; _LL234: tv_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp237); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp235); if( _temp222
==  0){ void* _temp239= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp183->topt))->v); struct Cyc_Absyn_TunionFieldInfo _temp245;
struct Cyc_List_List* _temp247; void* _temp249; struct Cyc_Absyn_Tunionfield*
_temp251; struct Cyc_Absyn_Tuniondecl* _temp253; _LL241: if(( unsigned int)
_temp239 >  4u?*(( int*) _temp239) ==  Cyc_Absyn_TunionFieldType: 0){ _LL246:
_temp245=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp239)->f1; _LL250:
_temp249=( void*) _temp245.field_info; if(*(( int*) _temp249) ==  Cyc_Absyn_KnownTunionfield){
_LL254: _temp253=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp249)->f1;
goto _LL252; _LL252: _temp251=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp249)->f2; goto _LL248;} else{ goto _LL243;} _LL248: _temp247= _temp245.targs;
goto _LL242;} else{ goto _LL243;} _LL243: goto _LL244; _LL242: t=( void*)({
struct Cyc_Absyn_TunionType_struct* _temp255=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp255[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp256; _temp256.tag= Cyc_Absyn_TunionType;
_temp256.f1=({ struct Cyc_Absyn_TunionInfo _temp257; _temp257.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp258=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp258[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp259; _temp259.tag= Cyc_Absyn_KnownTunion;
_temp259.f1=({ struct Cyc_Absyn_Tuniondecl** _temp260=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp260[ 0]= _temp253;
_temp260;}); _temp259;}); _temp258;})); _temp257.targs= _temp247; _temp257.rgn=(
void*) ptr_rgn; _temp257;}); _temp256;}); _temp255;}); goto _LL240; _LL244: t=(
void*)({ struct Cyc_Absyn_PointerType_struct* _temp261=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp261[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp262; _temp262.tag= Cyc_Absyn_PointerType;
_temp262.f1=({ struct Cyc_Absyn_PtrInfo _temp263; _temp263.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp183->topt))->v); _temp263.rgn_typ=(
void*) ptr_rgn; _temp263.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp263.tq= Cyc_Absyn_empty_tqual(); _temp263.bounds= Cyc_Absyn_empty_conref();
_temp263;}); _temp262;}); _temp261;}); goto _LL240; _LL240:;} else{ t=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp264=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp264[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp265; _temp265.tag= Cyc_Absyn_PointerType;
_temp265.f1=({ struct Cyc_Absyn_PtrInfo _temp266; _temp266.elt_typ=( void*)((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp183->topt))->v); _temp266.rgn_typ=(
void*) ptr_rgn; _temp266.nullable=(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)();
_temp266.tq= Cyc_Absyn_empty_tqual(); _temp266.bounds= Cyc_Absyn_empty_conref();
_temp266;}); _temp265;}); _temp264;});} goto _LL138;}} _LL160: { struct Cyc_List_List*
pat_ts= 0; struct Cyc_List_List* topt_ts= 0; if( topt !=  0){ void* _temp268=
Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_List_List*
_temp274; _LL270: if(( unsigned int) _temp268 >  4u?*(( int*) _temp268) ==  Cyc_Absyn_TupleType:
0){ _LL275: _temp274=(( struct Cyc_Absyn_TupleType_struct*) _temp268)->f1; goto
_LL271;} else{ goto _LL272;} _LL272: goto _LL273; _LL271: topt_ts= _temp274;
goto _LL269; _LL273: goto _LL269; _LL269:;} for( 0; _temp185 !=  0; _temp185=((
struct Cyc_List_List*) _check_null( _temp185))->tl){ void** _temp276= 0; if(
topt_ts !=  0){ _temp276=( void**)&(*(( struct _tuple8*)(( struct Cyc_List_List*)
_check_null( topt_ts))->hd)).f2; topt_ts=(( struct Cyc_List_List*) _check_null(
topt_ts))->tl;}{ struct Cyc_List_List* _temp279; struct Cyc_List_List* _temp281;
struct _tuple6 _temp277= Cyc_Tcpat_tcPatRec( te,( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp185))->hd, _temp276, region_opt);
_LL282: _temp281= _temp277.f1; goto _LL280; _LL280: _temp279= _temp277.f2; goto
_LL278; _LL278: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp281); v_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp279); pat_ts=({ struct Cyc_List_List* _temp283=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp283->hd=( void*)({ struct
_tuple8* _temp284=( struct _tuple8*) _cycalloc( sizeof( struct _tuple8));
_temp284->f1= Cyc_Absyn_empty_tqual(); _temp284->f2=( void*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null(
_temp185))->hd)->topt))->v; _temp284;}); _temp283->tl= pat_ts; _temp283;});}} t=(
void*)({ struct Cyc_Absyn_TupleType_struct* _temp285=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp285[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp286; _temp286.tag= Cyc_Absyn_TupleType; _temp286.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( pat_ts);
_temp286;}); _temp285;}); goto _LL138;} _LL162: if( _temp189 !=  0){( int)
_throw(({ void* _temp287[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcPat: struct<...> not implemented",
sizeof( unsigned char), 35u), _tag_arr( _temp287, sizeof( void*), 0u));}));}{
struct _RegionHandle _temp288= _new_region(); struct _RegionHandle* rgn=&
_temp288; _push_region( rgn);{ struct _tuple4 _temp289=({ struct _tuple4
_temp315; _temp315.f1= Cyc_Tcenv_lookup_type_vars( te); _temp315.f2= rgn;
_temp315;}); struct Cyc_List_List* _temp290=(( struct Cyc_List_List*(*)( struct
_RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp289, _temp194->tvs); struct Cyc_List_List* _temp291=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp290); t=( void*)({ struct Cyc_Absyn_StructType_struct*
_temp292=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp292[ 0]=({ struct Cyc_Absyn_StructType_struct _temp293; _temp293.tag= Cyc_Absyn_StructType;
_temp293.f1=( struct _tuple1*)(( struct _tuple1*)(( struct Cyc_Core_Opt*)
_check_null( _temp194->name))->v); _temp293.f2= _temp291; _temp293.f3=({ struct
Cyc_Absyn_Structdecl** _temp294=( struct Cyc_Absyn_Structdecl**) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp294[ 0]= _temp194; _temp294;});
_temp293;}); _temp292;}); if( topt !=  0){ Cyc_Tcutil_unify( t,*(( void**)
_check_null( topt)));}* _temp193=({ struct Cyc_Core_Opt* _temp295=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp295->v=( void*) _temp291;
_temp295;}); if( _temp194->fields ==  0){({ void* _temp296[ 0u]={}; Cyc_Tcutil_terr(
p->loc, _tag_arr("can't destructure an abstract struct", sizeof( unsigned char),
37u), _tag_arr( _temp296, sizeof( void*), 0u));}); _npop_handler( 0u); goto
_LL138;}{ struct Cyc_List_List* fields=(( struct Cyc_List_List*(*)( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)( rgn, p->loc,
_temp187,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp194->fields))->v);
for( 0; fields !=  0; fields=(( struct Cyc_List_List*) _check_null( fields))->tl){
struct _tuple9 _temp299; struct Cyc_Absyn_Pat* _temp300; struct Cyc_Absyn_Structfield*
_temp302; struct _tuple9* _temp297=( struct _tuple9*)(( struct Cyc_List_List*)
_check_null( fields))->hd; _temp299=* _temp297; _LL303: _temp302= _temp299.f1;
goto _LL301; _LL301: _temp300= _temp299.f2; goto _LL298; _LL298: { void*
_temp304= Cyc_Tcutil_rsubstitute( rgn, _temp290,( void*) _temp302->type); struct
Cyc_List_List* _temp307; struct Cyc_List_List* _temp309; struct _tuple6 _temp305=
Cyc_Tcpat_tcPatRec( te, _temp300,( void**)& _temp304, region_opt); _LL310:
_temp309= _temp305.f1; goto _LL308; _LL308: _temp307= _temp305.f2; goto _LL306;
_LL306: tv_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_append)( tv_result, _temp309); v_result=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
v_result, _temp307); if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp300->topt))->v, _temp304)){({ struct Cyc_Std_String_pa_struct
_temp314; _temp314.tag= Cyc_Std_String_pa; _temp314.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp300->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp313; _temp313.tag= Cyc_Std_String_pa; _temp313.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp304);{ struct Cyc_Std_String_pa_struct _temp312; _temp312.tag= Cyc_Std_String_pa;
_temp312.f1=( struct _tagged_arr)* _temp302->name;{ void* _temp311[ 3u]={&
_temp312,& _temp313,& _temp314}; Cyc_Tcutil_terr( p->loc, _tag_arr("field %s of struct pattern expects type %s != %s",
sizeof( unsigned char), 49u), _tag_arr( _temp311, sizeof( void*), 3u));}}}});}}}}};
_pop_region( rgn);} goto _LL138; _LL164: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp198) != (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp200->tvs)){({ void* _temp316[ 0u]={}; Cyc_Tcutil_terr( p->loc, _tag_arr("wrong number of existential type variables in pattern",
sizeof( unsigned char), 54u), _tag_arr( _temp316, sizeof( void*), 0u));});} if(((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp198) !=  0){ region_opt=
0;}{ struct _RegionHandle _temp317= _new_region(); struct _RegionHandle* rgn=&
_temp317; _push_region( rgn);{ struct Cyc_List_List* _temp318= 0;{ struct Cyc_List_List*
t= _temp198; for( 0; t !=  0; t=(( struct Cyc_List_List*) _check_null( t))->tl){
struct Cyc_Absyn_Tvar* tv=( struct Cyc_Absyn_Tvar*)(( struct Cyc_List_List*)
_check_null( t))->hd;{ void* _temp319=( void*)( Cyc_Absyn_compress_conref( tv->kind))->v;
void* _temp329; void* _temp331; _LL321: if(( unsigned int) _temp319 >  1u?*((
int*) _temp319) ==  Cyc_Absyn_Eq_constr: 0){ _LL330: _temp329=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp319)->f1; if( _temp329 == ( void*) Cyc_Absyn_BoxKind){
goto _LL322;} else{ goto _LL323;}} else{ goto _LL323;} _LL323: if(( unsigned int)
_temp319 >  1u?*(( int*) _temp319) ==  Cyc_Absyn_Eq_constr: 0){ _LL332: _temp331=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp319)->f1; goto _LL324;} else{
goto _LL325;} _LL325: if( _temp319 == ( void*) Cyc_Absyn_No_constr){ goto _LL326;}
else{ goto _LL327;} _LL327: if(( unsigned int) _temp319 >  1u?*(( int*) _temp319)
==  Cyc_Absyn_Forward_constr: 0){ goto _LL328;} else{ goto _LL320;} _LL322: goto
_LL320; _LL324:({ struct Cyc_Std_String_pa_struct _temp336; _temp336.tag= Cyc_Std_String_pa;
_temp336.f1=( struct _tagged_arr) Cyc_Absynpp_kind2string( _temp331);{ struct
Cyc_Std_String_pa_struct _temp335; _temp335.tag= Cyc_Std_String_pa; _temp335.f1=(
struct _tagged_arr)* tv->name;{ struct Cyc_Std_String_pa_struct _temp334;
_temp334.tag= Cyc_Std_String_pa; _temp334.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp202->name);{ void* _temp333[ 3u]={& _temp334,& _temp335,& _temp336}; Cyc_Tcutil_terr(
p->loc, _tag_arr("tunion member %s abstracts type variable %s of kind %s != B",
sizeof( unsigned char), 60u), _tag_arr( _temp333, sizeof( void*), 3u));}}}});
goto _LL320; _LL326:( void*)(( tv->kind)->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp337=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp337[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp338; _temp338.tag= Cyc_Absyn_Eq_constr;
_temp338.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp338;}); _temp337;})));
goto _LL320; _LL328:( int) _throw(({ void* _temp339[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("forward after compress in conref", sizeof( unsigned char), 33u),
_tag_arr( _temp339, sizeof( void*), 0u));})); _LL320:;} _temp318=({ struct Cyc_List_List*
_temp340=( struct Cyc_List_List*) _region_malloc( rgn, sizeof( struct Cyc_List_List));
_temp340->hd=( void*)(( void*)({ struct Cyc_Absyn_VarType_struct* _temp341=(
struct Cyc_Absyn_VarType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp341[ 0]=({ struct Cyc_Absyn_VarType_struct _temp342; _temp342.tag= Cyc_Absyn_VarType;
_temp342.f1= tv; _temp342;}); _temp341;})); _temp340->tl= _temp318; _temp340;});}}
_temp318= Cyc_List_imp_rev( _temp318); tv_result=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tv_result,
_temp198);{ struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars( p->loc, te,
_temp198); struct Cyc_List_List* tqts= _temp200->typs; struct Cyc_List_List*
_temp343= Cyc_Tcenv_lookup_type_vars( te); struct _tuple4 _temp344=({ struct
_tuple4 _temp396; _temp396.f1= _temp343; _temp396.f2= rgn; _temp396;}); struct
Cyc_List_List* _temp345=(( struct Cyc_List_List*(*)( struct _RegionHandle*,
struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*), struct _tuple4*
env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp344, _temp202->tvs); struct Cyc_List_List* _temp346=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp200->tvs, _temp318);
struct Cyc_List_List* _temp347=(( struct Cyc_List_List*(*)( void*(* f)( struct
_tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)( struct _tuple5*))
Cyc_Core_snd, _temp345); struct Cyc_List_List* _temp348=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp346); struct Cyc_List_List* _temp349=((
struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp345, _temp346); if( tqts ==  0){
t=( void*)({ struct Cyc_Absyn_TunionType_struct* _temp350=( struct Cyc_Absyn_TunionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct)); _temp350[ 0]=({ struct
Cyc_Absyn_TunionType_struct _temp351; _temp351.tag= Cyc_Absyn_TunionType;
_temp351.f1=({ struct Cyc_Absyn_TunionInfo _temp352; _temp352.tunion_info=( void*)((
void*)({ struct Cyc_Absyn_KnownTunion_struct* _temp354=( struct Cyc_Absyn_KnownTunion_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct)); _temp354[ 0]=({ struct
Cyc_Absyn_KnownTunion_struct _temp355; _temp355.tag= Cyc_Absyn_KnownTunion;
_temp355.f1=({ struct Cyc_Absyn_Tuniondecl** _temp356=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp356[ 0]= _temp202;
_temp356;}); _temp355;}); _temp354;})); _temp352.targs= _temp347; _temp352.rgn=(
void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcpat_rk,({ struct Cyc_Core_Opt*
_temp353=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp353->v=( void*) _temp343; _temp353;})); _temp352;}); _temp351;}); _temp350;});}
else{ t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp357=( struct
Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp357[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp358; _temp358.tag=
Cyc_Absyn_TunionFieldType; _temp358.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp359; _temp359.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp360=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp360[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp361; _temp361.tag= Cyc_Absyn_KnownTunionfield; _temp361.f1= _temp202;
_temp361.f2= _temp200; _temp361;}); _temp360;})); _temp359.targs= _temp347;
_temp359;}); _temp358;}); _temp357;});} if( topt !=  0? tqts ==  0: 0){ void*
_temp362= Cyc_Tcutil_compress(*(( void**) _check_null( topt))); struct Cyc_Absyn_TunionInfo
_temp370; struct Cyc_List_List* _temp372; _LL364: if(( unsigned int) _temp362 > 
4u?*(( int*) _temp362) ==  Cyc_Absyn_TunionFieldType: 0){ goto _LL365;} else{
goto _LL366;} _LL366: if(( unsigned int) _temp362 >  4u?*(( int*) _temp362) == 
Cyc_Absyn_TunionType: 0){ _LL371: _temp370=(( struct Cyc_Absyn_TunionType_struct*)
_temp362)->f1; _LL373: _temp372= _temp370.targs; goto _LL367;} else{ goto _LL368;}
_LL368: goto _LL369; _LL365: t=( void*)({ struct Cyc_Absyn_TunionFieldType_struct*
_temp374=( struct Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_TunionFieldType_struct)); _temp374[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct
_temp375; _temp375.tag= Cyc_Absyn_TunionFieldType; _temp375.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp376; _temp376.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp377=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp377[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp378; _temp378.tag= Cyc_Absyn_KnownTunionfield; _temp378.f1= _temp202;
_temp378.f2= _temp200; _temp378;}); _temp377;})); _temp376.targs= _temp347;
_temp376;}); _temp375;}); _temp374;}); goto _LL363; _LL367: { struct Cyc_List_List*
_temp379= _temp347; for( 0; _temp379 !=  0? _temp372 !=  0: 0;( _temp379=((
struct Cyc_List_List*) _check_null( _temp379))->tl, _temp372=(( struct Cyc_List_List*)
_check_null( _temp372))->tl)){ Cyc_Tcutil_unify(( void*)(( struct Cyc_List_List*)
_check_null( _temp379))->hd,( void*)(( struct Cyc_List_List*) _check_null(
_temp372))->hd);} goto _LL363;} _LL369: goto _LL363; _LL363:;} for( 0; _temp196
!=  0? tqts !=  0: 0;( _temp196=(( struct Cyc_List_List*) _check_null( _temp196))->tl,
tqts=(( struct Cyc_List_List*) _check_null( tqts))->tl)){ struct Cyc_Absyn_Pat*
_temp380=( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*) _check_null( _temp196))->hd;
void* _temp381= Cyc_Tcutil_rsubstitute( rgn, _temp349,(*(( struct _tuple8*)((
struct Cyc_List_List*) _check_null( tqts))->hd)).f2); struct Cyc_List_List*
_temp384; struct Cyc_List_List* _temp386; struct _tuple6 _temp382= Cyc_Tcpat_tcPatRec(
te2, _temp380,( void**)& _temp381, region_opt); _LL387: _temp386= _temp382.f1;
goto _LL385; _LL385: _temp384= _temp382.f2; goto _LL383; _LL383: tv_result=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
tv_result, _temp386); v_result=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( v_result, _temp384); if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp380->topt))->v, _temp381)){({
struct Cyc_Std_String_pa_struct _temp391; _temp391.tag= Cyc_Std_String_pa;
_temp391.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp380->topt))->v);{ struct Cyc_Std_String_pa_struct _temp390;
_temp390.tag= Cyc_Std_String_pa; _temp390.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp381);{ struct Cyc_Std_String_pa_struct _temp389; _temp389.tag= Cyc_Std_String_pa;
_temp389.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp202->name);{
void* _temp388[ 3u]={& _temp389,& _temp390,& _temp391}; Cyc_Tcutil_terr(
_temp380->loc, _tag_arr("%s expects argument type %s, not %s", sizeof(
unsigned char), 36u), _tag_arr( _temp388, sizeof( void*), 3u));}}}});}} if(
_temp196 !=  0){({ struct Cyc_Std_String_pa_struct _temp393; _temp393.tag= Cyc_Std_String_pa;
_temp393.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp202->name);{
void* _temp392[ 1u]={& _temp393}; Cyc_Tcutil_terr( p->loc, _tag_arr("too many arguments for tunion constructor %s",
sizeof( unsigned char), 45u), _tag_arr( _temp392, sizeof( void*), 1u));}});} if(
tqts !=  0){({ struct Cyc_Std_String_pa_struct _temp395; _temp395.tag= Cyc_Std_String_pa;
_temp395.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp202->name);{
void* _temp394[ 1u]={& _temp395}; Cyc_Tcutil_terr( p->loc, _tag_arr("too few arguments for tunion constructor %s",
sizeof( unsigned char), 44u), _tag_arr( _temp394, sizeof( void*), 1u));}});}}};
_pop_region( rgn);} goto _LL138; _LL166: goto _LL168; _LL168: goto _LL170;
_LL170: t= Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp397=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp397->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp397;})); goto _LL138; _LL138:;} tcpat_end: p->topt=({ struct Cyc_Core_Opt*
_temp398=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp398->v=( void*) t; _temp398;}); return({ struct _tuple6 _temp399; _temp399.f1=
tv_result; _temp399.f2= v_result; _temp399;});}} struct _tuple6 Cyc_Tcpat_tcPat(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p, void** topt, void**
region_opt){ struct _tuple6 _temp400= Cyc_Tcpat_tcPatRec( te, p, topt,
region_opt);{ struct _RegionHandle _temp401= _new_region(); struct _RegionHandle*
r=& _temp401; _push_region( r); Cyc_Tcutil_check_unique_vars((( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tagged_arr*(* f)( struct Cyc_Absyn_Vardecl*),
struct Cyc_List_List* x)) Cyc_List_rmap)( r, Cyc_Tcpat_get_name, _temp400.f2), p->loc,
_tag_arr("pattern contains a repeated variable", sizeof( unsigned char), 37u));;
_pop_region( r);} return _temp400;} void Cyc_Tcpat_check_pat_regions( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Pat* p){ void* _temp402=( void*) p->r; struct Cyc_Absyn_Pat*
_temp414; struct Cyc_List_List* _temp416; struct Cyc_List_List* _temp418; struct
Cyc_List_List* _temp420; _LL404: if(( unsigned int) _temp402 >  2u?*(( int*)
_temp402) ==  Cyc_Absyn_Pointer_p: 0){ _LL415: _temp414=(( struct Cyc_Absyn_Pointer_p_struct*)
_temp402)->f1; goto _LL405;} else{ goto _LL406;} _LL406: if(( unsigned int)
_temp402 >  2u?*(( int*) _temp402) ==  Cyc_Absyn_Tunion_p: 0){ _LL417: _temp416=((
struct Cyc_Absyn_Tunion_p_struct*) _temp402)->f4; goto _LL407;} else{ goto
_LL408;} _LL408: if(( unsigned int) _temp402 >  2u?*(( int*) _temp402) ==  Cyc_Absyn_Struct_p:
0){ _LL419: _temp418=(( struct Cyc_Absyn_Struct_p_struct*) _temp402)->f4; goto
_LL409;} else{ goto _LL410;} _LL410: if(( unsigned int) _temp402 >  2u?*(( int*)
_temp402) ==  Cyc_Absyn_Tuple_p: 0){ _LL421: _temp420=(( struct Cyc_Absyn_Tuple_p_struct*)
_temp402)->f1; goto _LL411;} else{ goto _LL412;} _LL412: goto _LL413; _LL405:
Cyc_Tcpat_check_pat_regions( te, _temp414);{ void* _temp422=( void*)(( struct
Cyc_Core_Opt*) _check_null( p->topt))->v; struct Cyc_Absyn_PtrInfo _temp430;
void* _temp432; struct Cyc_Absyn_TunionInfo _temp434; void* _temp436; _LL424:
if(( unsigned int) _temp422 >  4u?*(( int*) _temp422) ==  Cyc_Absyn_PointerType:
0){ _LL431: _temp430=(( struct Cyc_Absyn_PointerType_struct*) _temp422)->f1;
_LL433: _temp432=( void*) _temp430.rgn_typ; goto _LL425;} else{ goto _LL426;}
_LL426: if(( unsigned int) _temp422 >  4u?*(( int*) _temp422) ==  Cyc_Absyn_TunionType:
0){ _LL435: _temp434=(( struct Cyc_Absyn_TunionType_struct*) _temp422)->f1;
_LL437: _temp436=( void*) _temp434.rgn; goto _LL427;} else{ goto _LL428;} _LL428:
goto _LL429; _LL425: _temp436= _temp432; goto _LL427; _LL427: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp436); return; _LL429:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp438=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp438[ 0]=({ struct Cyc_Core_Impossible_struct _temp439; _temp439.tag= Cyc_Core_Impossible;
_temp439.f1= _tag_arr("check_pat_regions: bad pointer type", sizeof(
unsigned char), 36u); _temp439;}); _temp438;})); _LL423:;} _LL407: for( 0;
_temp416 !=  0; _temp416=(( struct Cyc_List_List*) _check_null( _temp416))->tl){
Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)(( struct Cyc_List_List*)
_check_null( _temp416))->hd);}{ void* _temp440=( void*)(( struct Cyc_Core_Opt*)
_check_null( p->topt))->v; struct Cyc_Absyn_TunionInfo _temp448; void* _temp450;
_LL442: if(( unsigned int) _temp440 >  4u?*(( int*) _temp440) ==  Cyc_Absyn_TunionType:
0){ _LL449: _temp448=(( struct Cyc_Absyn_TunionType_struct*) _temp440)->f1;
_LL451: _temp450=( void*) _temp448.rgn; goto _LL443;} else{ goto _LL444;} _LL444:
if(( unsigned int) _temp440 >  4u?*(( int*) _temp440) ==  Cyc_Absyn_TunionFieldType:
0){ goto _LL445;} else{ goto _LL446;} _LL446: goto _LL447; _LL443: Cyc_Tcenv_check_rgn_accessible(
te, p->loc, _temp450); return; _LL445: return; _LL447:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp452=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp452[ 0]=({ struct
Cyc_Core_Impossible_struct _temp453; _temp453.tag= Cyc_Core_Impossible; _temp453.f1=
_tag_arr("check_pat_regions: bad tunion type", sizeof( unsigned char), 35u);
_temp453;}); _temp452;})); _LL441:;} _LL409: _temp420=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Pat*(* f)( struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Pat*(*)( struct _tuple7*)) Cyc_Core_snd, _temp418); goto _LL411;
_LL411: for( 0; _temp420 !=  0; _temp420=(( struct Cyc_List_List*) _check_null(
_temp420))->tl){ Cyc_Tcpat_check_pat_regions( te,( struct Cyc_Absyn_Pat*)((
struct Cyc_List_List*) _check_null( _temp420))->hd);} return; _LL413: return;
_LL403:;} static const int Cyc_Tcpat_Name_v= 0; struct Cyc_Tcpat_Name_v_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Tcpat_Int_v= 1; struct
Cyc_Tcpat_Int_v_struct{ int tag; int f1; } ; struct Cyc_Tcpat_Con_s{ void* name;
int arity; struct Cyc_Core_Opt* span; } ; static const int Cyc_Tcpat_Any= 0;
static const int Cyc_Tcpat_Con= 0; struct Cyc_Tcpat_Con_struct{ int tag; struct
Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static int Cyc_Tcpat_compare_con(
struct Cyc_Tcpat_Con_s* c1, struct Cyc_Tcpat_Con_s* c2){ void* _temp454=( void*)
c1->name; struct _tagged_arr _temp460; int _temp462; _LL456: if(*(( int*)
_temp454) ==  Cyc_Tcpat_Name_v){ _LL461: _temp460=(( struct Cyc_Tcpat_Name_v_struct*)
_temp454)->f1; goto _LL457;} else{ goto _LL458;} _LL458: if(*(( int*) _temp454)
==  Cyc_Tcpat_Int_v){ _LL463: _temp462=(( struct Cyc_Tcpat_Int_v_struct*)
_temp454)->f1; goto _LL459;} else{ goto _LL455;} _LL457: { void* _temp464=( void*)
c2->name; struct _tagged_arr _temp470; _LL466: if(*(( int*) _temp464) ==  Cyc_Tcpat_Name_v){
_LL471: _temp470=(( struct Cyc_Tcpat_Name_v_struct*) _temp464)->f1; goto _LL467;}
else{ goto _LL468;} _LL468: if(*(( int*) _temp464) ==  Cyc_Tcpat_Int_v){ goto
_LL469;} else{ goto _LL465;} _LL467: return Cyc_Std_zstrcmp( _temp460, _temp470);
_LL469: return - 1; _LL465:;} _LL459: { void* _temp472=( void*) c2->name; int
_temp478; _LL474: if(*(( int*) _temp472) ==  Cyc_Tcpat_Name_v){ goto _LL475;}
else{ goto _LL476;} _LL476: if(*(( int*) _temp472) ==  Cyc_Tcpat_Int_v){ _LL479:
_temp478=(( struct Cyc_Tcpat_Int_v_struct*) _temp472)->f1; goto _LL477;} else{
goto _LL473;} _LL475: return 1; _LL477: return _temp462 -  _temp478; _LL473:;}
_LL455:;} static struct Cyc_Set_Set* Cyc_Tcpat_empty_con_set(){ return(( struct
Cyc_Set_Set*(*)( int(* cmp)( struct Cyc_Tcpat_Con_s*, struct Cyc_Tcpat_Con_s*)))
Cyc_Set_empty)( Cyc_Tcpat_compare_con);} static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(
void*) 1}; static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={( void*) 2}; static
struct Cyc_Core_Opt* Cyc_Tcpat_one_opt_ptr=( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt;
static unsigned char _temp481[ 5u]="NULL"; static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_null_name_value={ 0u,{ _temp481, _temp481, _temp481 +  5u}}; static
unsigned char _temp483[ 4u]="ptr"; static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={
0u,{ _temp483, _temp483, _temp483 +  4u}}; static unsigned char _temp485[ 6u]="tuple";
static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={ 0u,{ _temp485,
_temp485, _temp485 +  6u}}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(
void*)(( void*)& Cyc_Tcpat_null_name_value), 0,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={( void*)(( void*)& Cyc_Tcpat_ptr_name_value),
1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt}; static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(
void*)(( void*)& Cyc_Tcpat_ptr_name_value), 1,( struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};
static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_con=& Cyc_Tcpat_null_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0; static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0; static struct
Cyc_Tcpat_Con_s* Cyc_Tcpat_int_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp486=( struct Cyc_Tcpat_Con_s*) _cycalloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp486->name=( void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp487=(
struct Cyc_Tcpat_Int_v_struct*) _cycalloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct));
_temp487[ 0]=({ struct Cyc_Tcpat_Int_v_struct _temp488; _temp488.tag= Cyc_Tcpat_Int_v;
_temp488.f1= i; _temp488;}); _temp487;})); _temp486->arity= 0; _temp486->span= 0;
_temp486;});} static struct Cyc_Tcpat_Con_s* Cyc_Tcpat_float_con( struct
_tagged_arr f){ return({ struct Cyc_Tcpat_Con_s* _temp489=( struct Cyc_Tcpat_Con_s*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp489->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp490=( struct Cyc_Tcpat_Name_v_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp490[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp491; _temp491.tag= Cyc_Tcpat_Name_v; _temp491.f1= f; _temp491;}); _temp490;}));
_temp489->arity= 0; _temp489->span= 0; _temp489;});} static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con( unsigned char c){ return({ struct Cyc_Tcpat_Con_s* _temp492=(
struct Cyc_Tcpat_Con_s*) _cycalloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp492->name=(
void*)(( void*)({ struct Cyc_Tcpat_Int_v_struct* _temp494=( struct Cyc_Tcpat_Int_v_struct*)
_cycalloc_atomic( sizeof( struct Cyc_Tcpat_Int_v_struct)); _temp494[ 0]=({
struct Cyc_Tcpat_Int_v_struct _temp495; _temp495.tag= Cyc_Tcpat_Int_v; _temp495.f1=(
int) c; _temp495;}); _temp494;})); _temp492->arity= 0; _temp492->span=({ struct
Cyc_Core_Opt* _temp493=( struct Cyc_Core_Opt*) _cycalloc_atomic( sizeof( struct
Cyc_Core_Opt)); _temp493->v=( void*) 256; _temp493;}); _temp492;});} static
struct Cyc_Tcpat_Con_s* Cyc_Tcpat_tuple_con( int i){ return({ struct Cyc_Tcpat_Con_s*
_temp496=( struct Cyc_Tcpat_Con_s*) _cycalloc( sizeof( struct Cyc_Tcpat_Con_s));
_temp496->name=( void*)(( void*)& Cyc_Tcpat_tuple_name_value); _temp496->arity=
i; _temp496->span= Cyc_Tcpat_one_opt_ptr; _temp496;});} static void* Cyc_Tcpat_null_pat(){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp497=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp497[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp498; _temp498.tag= Cyc_Tcpat_Con; _temp498.f1= Cyc_Tcpat_null_con; _temp498.f2=
0; _temp498;}); _temp497;});} static void* Cyc_Tcpat_int_pat( int i){ return(
void*)({ struct Cyc_Tcpat_Con_struct* _temp499=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp499[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp500; _temp500.tag= Cyc_Tcpat_Con; _temp500.f1= Cyc_Tcpat_int_con( i);
_temp500.f2= 0; _temp500;}); _temp499;});} static void* Cyc_Tcpat_char_pat(
unsigned char c){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp501=(
struct Cyc_Tcpat_Con_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_Con_struct));
_temp501[ 0]=({ struct Cyc_Tcpat_Con_struct _temp502; _temp502.tag= Cyc_Tcpat_Con;
_temp502.f1= Cyc_Tcpat_char_con( c); _temp502.f2= 0; _temp502;}); _temp501;});}
static void* Cyc_Tcpat_float_pat( struct _tagged_arr f){ return( void*)({ struct
Cyc_Tcpat_Con_struct* _temp503=( struct Cyc_Tcpat_Con_struct*) _cycalloc(
sizeof( struct Cyc_Tcpat_Con_struct)); _temp503[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp504; _temp504.tag= Cyc_Tcpat_Con; _temp504.f1= Cyc_Tcpat_float_con( f);
_temp504.f2= 0; _temp504;}); _temp503;});} static void* Cyc_Tcpat_null_ptr_pat(
void* p){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp505=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp505[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp506; _temp506.tag= Cyc_Tcpat_Con; _temp506.f1= Cyc_Tcpat_null_ptr_con;
_temp506.f2=({ struct Cyc_List_List* _temp507=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp507->hd=( void*) p; _temp507->tl= 0;
_temp507;}); _temp506;}); _temp505;});} static void* Cyc_Tcpat_ptr_pat( void* p){
return( void*)({ struct Cyc_Tcpat_Con_struct* _temp508=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp508[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp509; _temp509.tag= Cyc_Tcpat_Con; _temp509.f1= Cyc_Tcpat_ptr_con; _temp509.f2=({
struct Cyc_List_List* _temp510=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp510->hd=( void*) p; _temp510->tl= 0; _temp510;});
_temp509;}); _temp508;});} static void* Cyc_Tcpat_tuple_pat( struct Cyc_List_List*
ss){ return( void*)({ struct Cyc_Tcpat_Con_struct* _temp511=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp511[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp512; _temp512.tag= Cyc_Tcpat_Con; _temp512.f1= Cyc_Tcpat_tuple_con( Cyc_List_length(
ss)); _temp512.f2= ss; _temp512;}); _temp511;});} static void* Cyc_Tcpat_con_pat(
struct _tagged_arr con_name, struct Cyc_Core_Opt* span, struct Cyc_List_List* ps){
struct Cyc_Tcpat_Con_s* c=({ struct Cyc_Tcpat_Con_s* _temp515=( struct Cyc_Tcpat_Con_s*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_s)); _temp515->name=( void*)(( void*)({
struct Cyc_Tcpat_Name_v_struct* _temp516=( struct Cyc_Tcpat_Name_v_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Name_v_struct)); _temp516[ 0]=({ struct Cyc_Tcpat_Name_v_struct
_temp517; _temp517.tag= Cyc_Tcpat_Name_v; _temp517.f1= con_name; _temp517;});
_temp516;})); _temp515->arity= Cyc_List_length( ps); _temp515->span= span;
_temp515;}); return( void*)({ struct Cyc_Tcpat_Con_struct* _temp513=( struct Cyc_Tcpat_Con_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Con_struct)); _temp513[ 0]=({ struct Cyc_Tcpat_Con_struct
_temp514; _temp514.tag= Cyc_Tcpat_Con; _temp514.f1= c; _temp514.f2= ps; _temp514;});
_temp513;});} static void* Cyc_Tcpat_compile_pat( struct Cyc_Absyn_Pat* p){ void*
s;{ void* _temp518=( void*) p->r; int _temp552; void* _temp554; unsigned char
_temp556; struct _tagged_arr _temp558; struct Cyc_Absyn_Pat* _temp560; struct
Cyc_List_List* _temp562; struct Cyc_Absyn_Tunionfield* _temp564; struct Cyc_Absyn_Tuniondecl*
_temp566; struct Cyc_List_List* _temp568; struct Cyc_List_List* _temp570; struct
Cyc_List_List* _temp572; struct Cyc_Absyn_Structdecl* _temp574; struct Cyc_Absyn_Enumfield*
_temp576; struct Cyc_Absyn_Enumdecl* _temp578; _LL520: if( _temp518 == ( void*)
Cyc_Absyn_Wild_p){ goto _LL521;} else{ goto _LL522;} _LL522: if(( unsigned int)
_temp518 >  2u?*(( int*) _temp518) ==  Cyc_Absyn_Var_p: 0){ goto _LL523;} else{
goto _LL524;} _LL524: if( _temp518 == ( void*) Cyc_Absyn_Null_p){ goto _LL525;}
else{ goto _LL526;} _LL526: if(( unsigned int) _temp518 >  2u?*(( int*) _temp518)
==  Cyc_Absyn_Int_p: 0){ _LL555: _temp554=( void*)(( struct Cyc_Absyn_Int_p_struct*)
_temp518)->f1; goto _LL553; _LL553: _temp552=(( struct Cyc_Absyn_Int_p_struct*)
_temp518)->f2; goto _LL527;} else{ goto _LL528;} _LL528: if(( unsigned int)
_temp518 >  2u?*(( int*) _temp518) ==  Cyc_Absyn_Char_p: 0){ _LL557: _temp556=((
struct Cyc_Absyn_Char_p_struct*) _temp518)->f1; goto _LL529;} else{ goto _LL530;}
_LL530: if(( unsigned int) _temp518 >  2u?*(( int*) _temp518) ==  Cyc_Absyn_Float_p:
0){ _LL559: _temp558=(( struct Cyc_Absyn_Float_p_struct*) _temp518)->f1; goto
_LL531;} else{ goto _LL532;} _LL532: if(( unsigned int) _temp518 >  2u?*(( int*)
_temp518) ==  Cyc_Absyn_Reference_p: 0){ goto _LL533;} else{ goto _LL534;}
_LL534: if(( unsigned int) _temp518 >  2u?*(( int*) _temp518) ==  Cyc_Absyn_Pointer_p:
0){ _LL561: _temp560=(( struct Cyc_Absyn_Pointer_p_struct*) _temp518)->f1; goto
_LL535;} else{ goto _LL536;} _LL536: if(( unsigned int) _temp518 >  2u?*(( int*)
_temp518) ==  Cyc_Absyn_Tunion_p: 0){ _LL567: _temp566=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp518)->f1; goto _LL565; _LL565: _temp564=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp518)->f2; goto _LL563; _LL563: _temp562=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp518)->f4; if( _temp562 ==  0){ goto _LL537;} else{ goto _LL538;}} else{
goto _LL538;} _LL538: if(( unsigned int) _temp518 >  2u?*(( int*) _temp518) == 
Cyc_Absyn_Tunion_p: 0){ _LL569: _temp568=(( struct Cyc_Absyn_Tunion_p_struct*)
_temp518)->f4; goto _LL539;} else{ goto _LL540;} _LL540: if(( unsigned int)
_temp518 >  2u?*(( int*) _temp518) ==  Cyc_Absyn_Tuple_p: 0){ _LL571: _temp570=((
struct Cyc_Absyn_Tuple_p_struct*) _temp518)->f1; goto _LL541;} else{ goto _LL542;}
_LL542: if(( unsigned int) _temp518 >  2u?*(( int*) _temp518) ==  Cyc_Absyn_Struct_p:
0){ _LL575: _temp574=(( struct Cyc_Absyn_Struct_p_struct*) _temp518)->f1; goto
_LL573; _LL573: _temp572=(( struct Cyc_Absyn_Struct_p_struct*) _temp518)->f4;
goto _LL543;} else{ goto _LL544;} _LL544: if(( unsigned int) _temp518 >  2u?*((
int*) _temp518) ==  Cyc_Absyn_Enum_p: 0){ _LL579: _temp578=(( struct Cyc_Absyn_Enum_p_struct*)
_temp518)->f1; goto _LL577; _LL577: _temp576=(( struct Cyc_Absyn_Enum_p_struct*)
_temp518)->f2; goto _LL545;} else{ goto _LL546;} _LL546: if(( unsigned int)
_temp518 >  2u?*(( int*) _temp518) ==  Cyc_Absyn_UnknownId_p: 0){ goto _LL547;}
else{ goto _LL548;} _LL548: if(( unsigned int) _temp518 >  2u?*(( int*) _temp518)
==  Cyc_Absyn_UnknownCall_p: 0){ goto _LL549;} else{ goto _LL550;} _LL550: if((
unsigned int) _temp518 >  2u?*(( int*) _temp518) ==  Cyc_Absyn_UnknownFields_p:
0){ goto _LL551;} else{ goto _LL519;} _LL521: s=( void*) Cyc_Tcpat_Any; goto
_LL519; _LL523: s=( void*) Cyc_Tcpat_Any; goto _LL519; _LL525: s= Cyc_Tcpat_null_pat();
goto _LL519; _LL527: s= Cyc_Tcpat_int_pat( _temp552); goto _LL519; _LL529: s=
Cyc_Tcpat_char_pat( _temp556); goto _LL519; _LL531: s= Cyc_Tcpat_float_pat(
_temp558); goto _LL519; _LL533: s=( void*) Cyc_Tcpat_Any; goto _LL519; _LL535:{
void* _temp580= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
p->topt))->v); struct Cyc_Absyn_PtrInfo _temp588; struct Cyc_Absyn_Conref*
_temp590; _LL582: if(( unsigned int) _temp580 >  4u?*(( int*) _temp580) ==  Cyc_Absyn_PointerType:
0){ _LL589: _temp588=(( struct Cyc_Absyn_PointerType_struct*) _temp580)->f1;
_LL591: _temp590= _temp588.nullable; goto _LL583;} else{ goto _LL584;} _LL584:
if(( unsigned int) _temp580 >  4u?*(( int*) _temp580) ==  Cyc_Absyn_TunionType:
0){ goto _LL585;} else{ goto _LL586;} _LL586: goto _LL587; _LL583: { int
is_nullable= 0; int still_working= 1; while( still_working) { void* _temp592=(
void*) _temp590->v; struct Cyc_Absyn_Conref* _temp600; int _temp602; _LL594: if((
unsigned int) _temp592 >  1u?*(( int*) _temp592) ==  Cyc_Absyn_Forward_constr: 0){
_LL601: _temp600=(( struct Cyc_Absyn_Forward_constr_struct*) _temp592)->f1; goto
_LL595;} else{ goto _LL596;} _LL596: if( _temp592 == ( void*) Cyc_Absyn_No_constr){
goto _LL597;} else{ goto _LL598;} _LL598: if(( unsigned int) _temp592 >  1u?*((
int*) _temp592) ==  Cyc_Absyn_Eq_constr: 0){ _LL603: _temp602=( int)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp592)->f1; goto _LL599;} else{ goto _LL593;}
_LL595:( void*)( _temp590->v=( void*)(( void*) _temp600->v)); continue; _LL597:(
void*)( _temp590->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp604=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp604[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp605; _temp605.tag= Cyc_Absyn_Eq_constr;
_temp605.f1=( void*) 0; _temp605;}); _temp604;}))); is_nullable= 0;
still_working= 0; goto _LL593; _LL599: is_nullable=( int) _temp602;
still_working= 0; goto _LL593; _LL593:;}{ void* ss= Cyc_Tcpat_compile_pat(
_temp560); if( is_nullable){ s= Cyc_Tcpat_null_ptr_pat( ss);} else{ s= Cyc_Tcpat_ptr_pat(
ss);} goto _LL581;}} _LL585:{ void* _temp606=( void*) _temp560->r; struct Cyc_List_List*
_temp612; struct Cyc_Absyn_Tunionfield* _temp614; struct Cyc_Absyn_Tuniondecl*
_temp616; _LL608: if(( unsigned int) _temp606 >  2u?*(( int*) _temp606) ==  Cyc_Absyn_Tunion_p:
0){ _LL617: _temp616=(( struct Cyc_Absyn_Tunion_p_struct*) _temp606)->f1; goto
_LL615; _LL615: _temp614=(( struct Cyc_Absyn_Tunion_p_struct*) _temp606)->f2;
goto _LL613; _LL613: _temp612=(( struct Cyc_Absyn_Tunion_p_struct*) _temp606)->f4;
goto _LL609;} else{ goto _LL610;} _LL610: goto _LL611; _LL609: { struct Cyc_Core_Opt*
span; if( _temp616->is_xtunion){ span= 0;} else{ span=({ struct Cyc_Core_Opt*
_temp618=( struct Cyc_Core_Opt*) _cycalloc_atomic( sizeof( struct Cyc_Core_Opt));
_temp618->v=( void*)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)((
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp616->fields))->v);
_temp618;});} s= Cyc_Tcpat_con_pat(*(* _temp614->name).f2, span,(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_compile_pat,
_temp612)); goto _LL607;} _LL611: s=({ void* _temp619[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("non-[x]tunion pattern has tunion type", sizeof( unsigned char), 38u),
_tag_arr( _temp619, sizeof( void*), 0u));}); goto _LL607; _LL607:;} goto _LL581;
_LL587: s=({ void* _temp620[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("pointer pattern does not have pointer type!",
sizeof( unsigned char), 44u), _tag_arr( _temp620, sizeof( void*), 0u));}); goto
_LL581; _LL581:;} goto _LL519; _LL537: { struct Cyc_Core_Opt* span;{ void*
_temp621= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( p->topt))->v);
_LL623: if(( unsigned int) _temp621 >  4u?*(( int*) _temp621) ==  Cyc_Absyn_TunionType:
0){ goto _LL624;} else{ goto _LL625;} _LL625: if(( unsigned int) _temp621 >  4u?*((
int*) _temp621) ==  Cyc_Absyn_TunionFieldType: 0){ goto _LL626;} else{ goto
_LL627;} _LL627: goto _LL628; _LL624: if( _temp566->is_xtunion){ span= 0;} else{
span=({ struct Cyc_Core_Opt* _temp629=( struct Cyc_Core_Opt*) _cycalloc_atomic(
sizeof( struct Cyc_Core_Opt)); _temp629->v=( void*)(( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( _temp566->fields))->v); _temp629;});} goto _LL622; _LL626: span=({
struct Cyc_Core_Opt* _temp630=( struct Cyc_Core_Opt*) _cycalloc_atomic( sizeof(
struct Cyc_Core_Opt)); _temp630->v=( void*) 1; _temp630;}); goto _LL622; _LL628:
span=({ void* _temp631[ 0u]={};(( struct Cyc_Core_Opt*(*)( struct _tagged_arr
fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("void tunion pattern has bad type",
sizeof( unsigned char), 33u), _tag_arr( _temp631, sizeof( void*), 0u));}); goto
_LL622; _LL622:;} s= Cyc_Tcpat_con_pat(*(* _temp564->name).f2, span, 0); goto
_LL519;} _LL539: _temp570= _temp568; goto _LL541; _LL541: s= Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Pat*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Tcpat_compile_pat, _temp570)); goto _LL519; _LL543: {
struct Cyc_List_List* ps= 0;{ struct Cyc_List_List* fields=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp574->fields))->v; for( 0; fields !=  0;
fields=(( struct Cyc_List_List*) _check_null( fields))->tl){ int found= Cyc_Std_strcmp(*((
struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null( fields))->hd)->name,
_tag_arr("", sizeof( unsigned char), 1u)) ==  0;{ struct Cyc_List_List* dlps0=
_temp572; for( 0; ! found? dlps0 !=  0: 0; dlps0=(( struct Cyc_List_List*)
_check_null( dlps0))->tl){ struct _tuple7 _temp634; struct Cyc_Absyn_Pat*
_temp635; struct Cyc_List_List* _temp637; struct _tuple7* _temp632=( struct
_tuple7*)(( struct Cyc_List_List*) _check_null( dlps0))->hd; _temp634=* _temp632;
_LL638: _temp637= _temp634.f1; goto _LL636; _LL636: _temp635= _temp634.f2; goto
_LL633; _LL633: { struct Cyc_List_List* _temp639= _temp637; struct Cyc_List_List
_temp645; struct Cyc_List_List* _temp646; void* _temp648; struct _tagged_arr*
_temp650; _LL641: if( _temp639 ==  0){ goto _LL643;} else{ _temp645=* _temp639;
_LL649: _temp648=( void*) _temp645.hd; if(*(( int*) _temp648) ==  Cyc_Absyn_FieldName){
_LL651: _temp650=(( struct Cyc_Absyn_FieldName_struct*) _temp648)->f1; goto
_LL647;} else{ goto _LL643;} _LL647: _temp646= _temp645.tl; if( _temp646 ==  0){
goto _LL642;} else{ goto _LL643;}} _LL643: goto _LL644; _LL642: if( Cyc_Std_zstrptrcmp(
_temp650,(( struct Cyc_Absyn_Structfield*)(( struct Cyc_List_List*) _check_null(
fields))->hd)->name) ==  0){ ps=({ struct Cyc_List_List* _temp652=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp652->hd=( void*) Cyc_Tcpat_compile_pat(
_temp635); _temp652->tl= ps; _temp652;}); found= 1;} goto _LL640; _LL644:({ void*
_temp653[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bad designator(s)", sizeof( unsigned char), 18u), _tag_arr( _temp653,
sizeof( void*), 0u));}); goto _LL640; _LL640:;}}} if( ! found){({ void* _temp654[
0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("bad designator", sizeof( unsigned char), 15u), _tag_arr( _temp654,
sizeof( void*), 0u));});}}} s= Cyc_Tcpat_tuple_pat( ps); goto _LL519;} _LL545: {
int span=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp578->fields))->v); s= Cyc_Tcpat_con_pat(*(*
_temp576->name).f2,({ struct Cyc_Core_Opt* _temp655=( struct Cyc_Core_Opt*)
_cycalloc_atomic( sizeof( struct Cyc_Core_Opt)); _temp655->v=( void*) span;
_temp655;}), 0); goto _LL519;} _LL547: s=({ void* _temp656[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("compile_pat: unknownid", sizeof( unsigned char), 23u), _tag_arr(
_temp656, sizeof( void*), 0u));}); goto _LL519; _LL549: s=({ void* _temp657[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("compile_pat: unknowncall", sizeof( unsigned char),
25u), _tag_arr( _temp657, sizeof( void*), 0u));}); goto _LL519; _LL551: s=({
void* _temp658[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("compile_pat: unknownfields",
sizeof( unsigned char), 27u), _tag_arr( _temp658, sizeof( void*), 0u));}); goto
_LL519; _LL519:;} return s;} static const int Cyc_Tcpat_Pos= 0; struct Cyc_Tcpat_Pos_struct{
int tag; struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Tcpat_Neg= 1; struct Cyc_Tcpat_Neg_struct{ int tag; struct Cyc_Set_Set*
f1; } ; static const int Cyc_Tcpat_Failure= 0; static const int Cyc_Tcpat_Success=
0; struct Cyc_Tcpat_Success_struct{ int tag; void* f1; } ; static const int Cyc_Tcpat_IfEq=
1; struct Cyc_Tcpat_IfEq_struct{ int tag; struct Cyc_List_List* f1; struct Cyc_Tcpat_Con_s*
f2; void* f3; void* f4; } ; struct _tuple10{ struct Cyc_List_List* f1; struct
Cyc_List_List* f2; struct Cyc_List_List* f3; } ; static const int Cyc_Tcpat_Yes=
0; static const int Cyc_Tcpat_No= 1; static const int Cyc_Tcpat_Maybe= 2; static
void* Cyc_Tcpat_add_neg( void* td, struct Cyc_Tcpat_Con_s* c){ void* _temp659=
td; struct Cyc_Set_Set* _temp665; _LL661: if(*(( int*) _temp659) ==  Cyc_Tcpat_Neg){
_LL666: _temp665=(( struct Cyc_Tcpat_Neg_struct*) _temp659)->f1; goto _LL662;}
else{ goto _LL663;} _LL663: if(*(( int*) _temp659) ==  Cyc_Tcpat_Pos){ goto
_LL664;} else{ goto _LL660;} _LL662: if((( int(*)( struct Cyc_Set_Set* s, struct
Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp665, c)){({ void* _temp667[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("add_neg called when constructor already in set", sizeof( unsigned char),
47u), _tag_arr( _temp667, sizeof( void*), 0u));});} if( c->span !=  0?(( int(*)(
struct Cyc_Set_Set* s)) Cyc_Set_cardinality)( _temp665) +  1 >= ( int)(( struct
Cyc_Core_Opt*) _check_null( c->span))->v: 0){({ void* _temp668[ 0u]={};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("add_neg called when |cs U {c}| >= span(c)",
sizeof( unsigned char), 42u), _tag_arr( _temp668, sizeof( void*), 0u));});}
return( void*)({ struct Cyc_Tcpat_Neg_struct* _temp669=( struct Cyc_Tcpat_Neg_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp669[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp670; _temp670.tag= Cyc_Tcpat_Neg; _temp670.f1=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_insert)( _temp665,
c); _temp670;}); _temp669;}); _LL664: return({ void* _temp671[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("add_neg called when td is Positive", sizeof( unsigned char), 35u),
_tag_arr( _temp671, sizeof( void*), 0u));}); _LL660:;} static void* Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s* c, void* td){ void* _temp672= td; struct Cyc_Tcpat_Con_s*
_temp678; struct Cyc_Set_Set* _temp680; _LL674: if(*(( int*) _temp672) ==  Cyc_Tcpat_Pos){
_LL679: _temp678=(( struct Cyc_Tcpat_Pos_struct*) _temp672)->f1; goto _LL675;}
else{ goto _LL676;} _LL676: if(*(( int*) _temp672) ==  Cyc_Tcpat_Neg){ _LL681:
_temp680=(( struct Cyc_Tcpat_Neg_struct*) _temp672)->f1; goto _LL677;} else{
goto _LL673;} _LL675: if( Cyc_Tcpat_compare_con( c, _temp678) ==  0){ return(
void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_No;} _LL677: if((( int(*)(
struct Cyc_Set_Set* s, struct Cyc_Tcpat_Con_s* elt)) Cyc_Set_member)( _temp680,
c)){ return( void*) Cyc_Tcpat_No;} else{ if( c->span !=  0?( int)(( struct Cyc_Core_Opt*)
_check_null( c->span))->v == (( int(*)( struct Cyc_Set_Set* s)) Cyc_Set_cardinality)(
_temp680) +  1: 0){ return( void*) Cyc_Tcpat_Yes;} else{ return( void*) Cyc_Tcpat_Maybe;}}
_LL673:;} struct _tuple11{ struct Cyc_Tcpat_Con_s* f1; struct Cyc_List_List* f2;
} ; static struct Cyc_List_List* Cyc_Tcpat_augment( struct Cyc_List_List* ctxt,
void* dsc){ struct Cyc_List_List* _temp682= ctxt; struct Cyc_List_List _temp688;
struct Cyc_List_List* _temp689; struct _tuple11* _temp691; struct _tuple11
_temp693; struct Cyc_List_List* _temp694; struct Cyc_Tcpat_Con_s* _temp696;
_LL684: if( _temp682 ==  0){ goto _LL685;} else{ goto _LL686;} _LL686: if(
_temp682 ==  0){ goto _LL683;} else{ _temp688=* _temp682; _LL692: _temp691=(
struct _tuple11*) _temp688.hd; _temp693=* _temp691; _LL697: _temp696= _temp693.f1;
goto _LL695; _LL695: _temp694= _temp693.f2; goto _LL690; _LL690: _temp689=
_temp688.tl; goto _LL687;} _LL685: return 0; _LL687: return({ struct Cyc_List_List*
_temp698=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp698->hd=( void*)({ struct _tuple11* _temp699=( struct _tuple11*) _cycalloc(
sizeof( struct _tuple11)); _temp699->f1= _temp696; _temp699->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp700=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp700->hd=( void*) dsc; _temp700->tl= _temp694;
_temp700;}); _temp699;}); _temp698->tl= _temp689; _temp698;}); _LL683:;} static
struct Cyc_List_List* Cyc_Tcpat_norm_context( struct Cyc_List_List* ctxt){
struct Cyc_List_List* _temp701= ctxt; struct Cyc_List_List _temp707; struct Cyc_List_List*
_temp708; struct _tuple11* _temp710; struct _tuple11 _temp712; struct Cyc_List_List*
_temp713; struct Cyc_Tcpat_Con_s* _temp715; _LL703: if( _temp701 ==  0){ goto
_LL704;} else{ goto _LL705;} _LL705: if( _temp701 ==  0){ goto _LL702;} else{
_temp707=* _temp701; _LL711: _temp710=( struct _tuple11*) _temp707.hd; _temp712=*
_temp710; _LL716: _temp715= _temp712.f1; goto _LL714; _LL714: _temp713= _temp712.f2;
goto _LL709; _LL709: _temp708= _temp707.tl; goto _LL706;} _LL704: return({ void*
_temp717[ 0u]={};(( struct Cyc_List_List*(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("norm_context: empty context",
sizeof( unsigned char), 28u), _tag_arr( _temp717, sizeof( void*), 0u));});
_LL706: return Cyc_Tcpat_augment( _temp708,( void*)({ struct Cyc_Tcpat_Pos_struct*
_temp718=( struct Cyc_Tcpat_Pos_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_Pos_struct));
_temp718[ 0]=({ struct Cyc_Tcpat_Pos_struct _temp719; _temp719.tag= Cyc_Tcpat_Pos;
_temp719.f1= _temp715; _temp719.f2= Cyc_List_rev( _temp713); _temp719;});
_temp718;})); _LL702:;} static void* Cyc_Tcpat_build_desc( struct Cyc_List_List*
ctxt, void* dsc, struct Cyc_List_List* work){ struct _tuple6 _temp721=({ struct
_tuple6 _temp720; _temp720.f1= ctxt; _temp720.f2= work; _temp720;}); struct Cyc_List_List*
_temp731; struct Cyc_List_List* _temp733; struct Cyc_List_List* _temp735; struct
Cyc_List_List* _temp737; struct Cyc_List_List* _temp739; struct Cyc_List_List
_temp741; struct Cyc_List_List* _temp742; struct _tuple10* _temp744; struct
_tuple10 _temp746; struct Cyc_List_List* _temp747; struct Cyc_List_List*
_temp749; struct Cyc_List_List _temp751; struct Cyc_List_List* _temp752; struct
_tuple11* _temp754; struct _tuple11 _temp756; struct Cyc_List_List* _temp757;
struct Cyc_Tcpat_Con_s* _temp759; _LL723: _LL734: _temp733= _temp721.f1; if(
_temp733 ==  0){ goto _LL732;} else{ goto _LL725;} _LL732: _temp731= _temp721.f2;
if( _temp731 ==  0){ goto _LL724;} else{ goto _LL725;} _LL725: _LL736: _temp735=
_temp721.f1; if( _temp735 ==  0){ goto _LL726;} else{ goto _LL727;} _LL727:
_LL738: _temp737= _temp721.f2; if( _temp737 ==  0){ goto _LL728;} else{ goto
_LL729;} _LL729: _LL750: _temp749= _temp721.f1; if( _temp749 ==  0){ goto _LL722;}
else{ _temp751=* _temp749; _LL755: _temp754=( struct _tuple11*) _temp751.hd;
_temp756=* _temp754; _LL760: _temp759= _temp756.f1; goto _LL758; _LL758:
_temp757= _temp756.f2; goto _LL753; _LL753: _temp752= _temp751.tl; goto _LL740;}
_LL740: _temp739= _temp721.f2; if( _temp739 ==  0){ goto _LL722;} else{ _temp741=*
_temp739; _LL745: _temp744=( struct _tuple10*) _temp741.hd; _temp746=* _temp744;
_LL748: _temp747= _temp746.f3; goto _LL743; _LL743: _temp742= _temp741.tl; goto
_LL730;} _LL724: return dsc; _LL726: return({ void* _temp761[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("build_desc: ctxt and work don't match", sizeof( unsigned char), 38u),
_tag_arr( _temp761, sizeof( void*), 0u));}); _LL728: return({ void* _temp762[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("build_desc: ctxt and work don't match", sizeof(
unsigned char), 38u), _tag_arr( _temp762, sizeof( void*), 0u));}); _LL730: {
struct Cyc_Tcpat_Pos_struct* _temp763=({ struct Cyc_Tcpat_Pos_struct* _temp764=(
struct Cyc_Tcpat_Pos_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_Pos_struct));
_temp764[ 0]=({ struct Cyc_Tcpat_Pos_struct _temp765; _temp765.tag= Cyc_Tcpat_Pos;
_temp765.f1= _temp759; _temp765.f2= Cyc_List_append( Cyc_List_rev( _temp757),({
struct Cyc_List_List* _temp766=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp766->hd=( void*) dsc; _temp766->tl= _temp747;
_temp766;})); _temp765;}); _temp764;}); return Cyc_Tcpat_build_desc( _temp752,(
void*) _temp763, _temp742);} _LL722:;} static void* Cyc_Tcpat_match( void* p,
struct Cyc_List_List* obj, void* dsc, struct Cyc_List_List* ctx, struct Cyc_List_List*
work, void* right_hand_side, struct Cyc_List_List* rules); static void* Cyc_Tcpat_or_match(
void* dsc, struct Cyc_List_List* allmrules){ struct Cyc_List_List* _temp767=
allmrules; struct Cyc_List_List _temp773; struct Cyc_List_List* _temp774; struct
_tuple0* _temp776; struct _tuple0 _temp778; void* _temp779; void* _temp781;
_LL769: if( _temp767 ==  0){ goto _LL770;} else{ goto _LL771;} _LL771: if(
_temp767 ==  0){ goto _LL768;} else{ _temp773=* _temp767; _LL777: _temp776=(
struct _tuple0*) _temp773.hd; _temp778=* _temp776; _LL782: _temp781= _temp778.f1;
goto _LL780; _LL780: _temp779= _temp778.f2; goto _LL775; _LL775: _temp774=
_temp773.tl; goto _LL772;} _LL770: return( void*) Cyc_Tcpat_Failure; _LL772:
return Cyc_Tcpat_match( _temp781, 0, dsc, 0, 0, _temp779, _temp774); _LL768:;}
static void* Cyc_Tcpat_match_compile( struct Cyc_List_List* allmrules){ return
Cyc_Tcpat_or_match(( void*)({ struct Cyc_Tcpat_Neg_struct* _temp783=( struct Cyc_Tcpat_Neg_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Neg_struct)); _temp783[ 0]=({ struct Cyc_Tcpat_Neg_struct
_temp784; _temp784.tag= Cyc_Tcpat_Neg; _temp784.f1= Cyc_Tcpat_empty_con_set();
_temp784;}); _temp783;}), allmrules);} static void* Cyc_Tcpat_and_match( struct
Cyc_List_List* ctx, struct Cyc_List_List* work, void* right_hand_side, struct
Cyc_List_List* rules){ struct Cyc_List_List* _temp785= work; struct Cyc_List_List
_temp793; struct Cyc_List_List* _temp794; struct _tuple10* _temp796; struct
_tuple10 _temp798; struct Cyc_List_List* _temp799; struct Cyc_List_List*
_temp801; struct Cyc_List_List* _temp803; struct Cyc_List_List _temp805; struct
Cyc_List_List* _temp806; struct _tuple10* _temp808; struct _tuple10 _temp810;
struct Cyc_List_List* _temp811; struct Cyc_List_List* _temp813; struct Cyc_List_List*
_temp815; _LL787: if( _temp785 ==  0){ goto _LL788;} else{ goto _LL789;} _LL789:
if( _temp785 ==  0){ goto _LL791;} else{ _temp793=* _temp785; _LL797: _temp796=(
struct _tuple10*) _temp793.hd; _temp798=* _temp796; _LL804: _temp803= _temp798.f1;
if( _temp803 ==  0){ goto _LL802;} else{ goto _LL791;} _LL802: _temp801=
_temp798.f2; if( _temp801 ==  0){ goto _LL800;} else{ goto _LL791;} _LL800:
_temp799= _temp798.f3; if( _temp799 ==  0){ goto _LL795;} else{ goto _LL791;}
_LL795: _temp794= _temp793.tl; goto _LL790;} _LL791: if( _temp785 ==  0){ goto
_LL786;} else{ _temp805=* _temp785; _LL809: _temp808=( struct _tuple10*)
_temp805.hd; _temp810=* _temp808; _LL816: _temp815= _temp810.f1; goto _LL814;
_LL814: _temp813= _temp810.f2; goto _LL812; _LL812: _temp811= _temp810.f3; goto
_LL807; _LL807: _temp806= _temp805.tl; goto _LL792;} _LL788: return( void*)({
struct Cyc_Tcpat_Success_struct* _temp817=( struct Cyc_Tcpat_Success_struct*)
_cycalloc( sizeof( struct Cyc_Tcpat_Success_struct)); _temp817[ 0]=({ struct Cyc_Tcpat_Success_struct
_temp818; _temp818.tag= Cyc_Tcpat_Success; _temp818.f1=( void*) right_hand_side;
_temp818;}); _temp817;}); _LL790: return Cyc_Tcpat_and_match( Cyc_Tcpat_norm_context(
ctx), _temp794, right_hand_side, rules); _LL792: if(( _temp815 ==  0? 1:
_temp813 ==  0)? 1: _temp811 ==  0){ return({ void* _temp819[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("tcpat:and_match: malformed work frame", sizeof( unsigned char), 38u),
_tag_arr( _temp819, sizeof( void*), 0u));});}{ struct Cyc_List_List _temp822;
struct Cyc_List_List* _temp823; void* _temp825; struct Cyc_List_List* _temp820=(
struct Cyc_List_List*) _check_null( _temp815); _temp822=* _temp820; _LL826:
_temp825=( void*) _temp822.hd; goto _LL824; _LL824: _temp823= _temp822.tl; goto
_LL821; _LL821: { struct Cyc_List_List _temp829; struct Cyc_List_List* _temp830;
struct Cyc_List_List* _temp832; struct Cyc_List_List* _temp827=( struct Cyc_List_List*)
_check_null( _temp813); _temp829=* _temp827; _LL833: _temp832=( struct Cyc_List_List*)
_temp829.hd; goto _LL831; _LL831: _temp830= _temp829.tl; goto _LL828; _LL828: {
struct Cyc_List_List _temp836; struct Cyc_List_List* _temp837; void* _temp839;
struct Cyc_List_List* _temp834=( struct Cyc_List_List*) _check_null( _temp811);
_temp836=* _temp834; _LL840: _temp839=( void*) _temp836.hd; goto _LL838; _LL838:
_temp837= _temp836.tl; goto _LL835; _LL835: { struct _tuple10* _temp841=({
struct _tuple10* _temp843=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10));
_temp843->f1= _temp823; _temp843->f2= _temp830; _temp843->f3= _temp837; _temp843;});
return Cyc_Tcpat_match( _temp825, _temp832, _temp839, ctx,({ struct Cyc_List_List*
_temp842=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp842->hd=( void*) _temp841; _temp842->tl= _temp806; _temp842;}),
right_hand_side, rules);}}}} _LL786:;} static struct Cyc_List_List* Cyc_Tcpat_getdargs(
struct Cyc_Tcpat_Con_s* pcon, void* dsc){ void* _temp844= dsc; struct Cyc_Set_Set*
_temp850; struct Cyc_List_List* _temp852; _LL846: if(*(( int*) _temp844) ==  Cyc_Tcpat_Neg){
_LL851: _temp850=(( struct Cyc_Tcpat_Neg_struct*) _temp844)->f1; goto _LL847;}
else{ goto _LL848;} _LL848: if(*(( int*) _temp844) ==  Cyc_Tcpat_Pos){ _LL853:
_temp852=(( struct Cyc_Tcpat_Pos_struct*) _temp844)->f2; goto _LL849;} else{
goto _LL845;} _LL847: { void* any=( void*)({ struct Cyc_Tcpat_Neg_struct*
_temp856=( struct Cyc_Tcpat_Neg_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_Neg_struct));
_temp856[ 0]=({ struct Cyc_Tcpat_Neg_struct _temp857; _temp857.tag= Cyc_Tcpat_Neg;
_temp857.f1= Cyc_Tcpat_empty_con_set(); _temp857;}); _temp856;}); struct Cyc_List_List*
_temp854= 0;{ int i= 0; for( 0; i <  pcon->arity; ++ i){ _temp854=({ struct Cyc_List_List*
_temp855=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp855->hd=( void*) any; _temp855->tl= _temp854; _temp855;});}} return
_temp854;} _LL849: return _temp852; _LL845:;} static struct Cyc_List_List* Cyc_Tcpat_getoarg(
struct Cyc_List_List* obj, int i){ return({ struct Cyc_List_List* _temp858=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp858->hd=(
void*)( i +  1); _temp858->tl= obj; _temp858;});} static struct Cyc_List_List*
Cyc_Tcpat_getoargs( struct Cyc_Tcpat_Con_s* pcon, struct Cyc_List_List* obj){
return(( struct Cyc_List_List*(*)( int n, struct Cyc_List_List*(* f)( struct Cyc_List_List*,
int), struct Cyc_List_List* env)) Cyc_List_tabulate_c)( pcon->arity, Cyc_Tcpat_getoarg,
obj);} static void* Cyc_Tcpat_match( void* p, struct Cyc_List_List* obj, void*
dsc, struct Cyc_List_List* ctx, struct Cyc_List_List* work, void*
right_hand_side, struct Cyc_List_List* rules){ void* _temp859= p; struct Cyc_List_List*
_temp865; struct Cyc_Tcpat_Con_s* _temp867; _LL861: if( _temp859 == ( void*) Cyc_Tcpat_Any){
goto _LL862;} else{ goto _LL863;} _LL863: if(( unsigned int) _temp859 >  1u?*((
int*) _temp859) ==  Cyc_Tcpat_Con: 0){ _LL868: _temp867=(( struct Cyc_Tcpat_Con_struct*)
_temp859)->f1; goto _LL866; _LL866: _temp865=(( struct Cyc_Tcpat_Con_struct*)
_temp859)->f2; goto _LL864;} else{ goto _LL860;} _LL862: return Cyc_Tcpat_and_match(
Cyc_Tcpat_augment( ctx, dsc), work, right_hand_side, rules); _LL864: { void*
_temp869= Cyc_Tcpat_static_match( _temp867, dsc); _LL871: if( _temp869 == ( void*)
Cyc_Tcpat_Yes){ goto _LL872;} else{ goto _LL873;} _LL873: if( _temp869 == ( void*)
Cyc_Tcpat_No){ goto _LL874;} else{ goto _LL875;} _LL875: if( _temp869 == ( void*)
Cyc_Tcpat_Maybe){ goto _LL876;} else{ goto _LL870;} _LL872: { struct Cyc_List_List*
_temp877=({ struct Cyc_List_List* _temp882=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp882->hd=( void*)({ struct _tuple11*
_temp883=( struct _tuple11*) _cycalloc( sizeof( struct _tuple11)); _temp883->f1=
_temp867; _temp883->f2= 0; _temp883;}); _temp882->tl= ctx; _temp882;}); struct
_tuple10* _temp878=({ struct _tuple10* _temp881=( struct _tuple10*) _cycalloc(
sizeof( struct _tuple10)); _temp881->f1= _temp865; _temp881->f2= Cyc_Tcpat_getoargs(
_temp867, obj); _temp881->f3= Cyc_Tcpat_getdargs( _temp867, dsc); _temp881;});
struct Cyc_List_List* _temp879=({ struct Cyc_List_List* _temp880=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp880->hd=( void*) _temp878;
_temp880->tl= work; _temp880;}); return Cyc_Tcpat_and_match( _temp877, _temp879,
right_hand_side, rules);} _LL874: return Cyc_Tcpat_or_match( Cyc_Tcpat_build_desc(
ctx, dsc, work), rules); _LL876: { struct Cyc_List_List* _temp884=({ struct Cyc_List_List*
_temp893=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp893->hd=( void*)({ struct _tuple11* _temp894=( struct _tuple11*) _cycalloc(
sizeof( struct _tuple11)); _temp894->f1= _temp867; _temp894->f2= 0; _temp894;});
_temp893->tl= ctx; _temp893;}); struct _tuple10* _temp885=({ struct _tuple10*
_temp892=( struct _tuple10*) _cycalloc( sizeof( struct _tuple10)); _temp892->f1=
_temp865; _temp892->f2= Cyc_Tcpat_getoargs( _temp867, obj); _temp892->f3= Cyc_Tcpat_getdargs(
_temp867, dsc); _temp892;}); struct Cyc_List_List* _temp886=({ struct Cyc_List_List*
_temp891=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp891->hd=( void*) _temp885; _temp891->tl= work; _temp891;}); void* _temp887=
Cyc_Tcpat_and_match( _temp884, _temp886, right_hand_side, rules); void* _temp888=
Cyc_Tcpat_or_match( Cyc_Tcpat_build_desc( ctx, Cyc_Tcpat_add_neg( dsc, _temp867),
work), rules); return( void*)({ struct Cyc_Tcpat_IfEq_struct* _temp889=( struct
Cyc_Tcpat_IfEq_struct*) _cycalloc( sizeof( struct Cyc_Tcpat_IfEq_struct));
_temp889[ 0]=({ struct Cyc_Tcpat_IfEq_struct _temp890; _temp890.tag= Cyc_Tcpat_IfEq;
_temp890.f1= obj; _temp890.f2= _temp867; _temp890.f3=( void*) _temp887; _temp890.f4=(
void*) _temp888; _temp890;}); _temp889;});} _LL870:;} _LL860:;} static void Cyc_Tcpat_check_exhaust_overlap(
void* d, void(* not_exhaust)( void*), void* env1, void(* rhs_appears)( void*,
void*), void* env2){ void* _temp895= d; void* _temp903; void* _temp905; void*
_temp907; _LL897: if( _temp895 == ( void*) Cyc_Tcpat_Failure){ goto _LL898;}
else{ goto _LL899;} _LL899: if(( unsigned int) _temp895 >  1u?*(( int*) _temp895)
==  Cyc_Tcpat_Success: 0){ _LL904: _temp903=( void*)(( struct Cyc_Tcpat_Success_struct*)
_temp895)->f1; goto _LL900;} else{ goto _LL901;} _LL901: if(( unsigned int)
_temp895 >  1u?*(( int*) _temp895) ==  Cyc_Tcpat_IfEq: 0){ _LL908: _temp907=(
void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp895)->f3; goto _LL906; _LL906:
_temp905=( void*)(( struct Cyc_Tcpat_IfEq_struct*) _temp895)->f4; goto _LL902;}
else{ goto _LL896;} _LL898: not_exhaust( env1); goto _LL896; _LL900: rhs_appears(
env2, _temp903); goto _LL896; _LL902: Cyc_Tcpat_check_exhaust_overlap( _temp907,
not_exhaust, env1, rhs_appears, env2); Cyc_Tcpat_check_exhaust_overlap( _temp905,
not_exhaust, env1, rhs_appears, env2); goto _LL896; _LL896:;} struct _tuple12{
int f1; struct Cyc_Position_Segment* f2; } ; struct _tuple13{ void* f1; struct
_tuple12* f2; } ; static struct _tuple13* Cyc_Tcpat_get_match( int* r, struct
Cyc_Absyn_Switch_clause* swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern);
struct _tuple12* rhs=({ struct _tuple12* _temp914=( struct _tuple12*) _cycalloc(
sizeof( struct _tuple12)); _temp914->f1= 0; _temp914->f2=( swc->pattern)->loc;
_temp914;}); void* sp; if( swc->where_clause !=  0){ sp= Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List* _temp909=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp909->hd=( void*) sp0; _temp909->tl=({ struct Cyc_List_List*
_temp910=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp910->hd=( void*) Cyc_Tcpat_int_pat(* r); _temp910->tl= 0; _temp910;});
_temp909;}));* r=* r +  1;} else{ sp= Cyc_Tcpat_tuple_pat(({ struct Cyc_List_List*
_temp911=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp911->hd=( void*) sp0; _temp911->tl=({ struct Cyc_List_List* _temp912=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp912->hd=(
void*)(( void*) Cyc_Tcpat_Any); _temp912->tl= 0; _temp912;}); _temp911;}));}
return({ struct _tuple13* _temp913=( struct _tuple13*) _cycalloc( sizeof( struct
_tuple13)); _temp913->f1= sp; _temp913->f2= rhs; _temp913;});} static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment* loc){({ void* _temp915[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("patterns may not be exhaustive", sizeof( unsigned char), 31u),
_tag_arr( _temp915, sizeof( void*), 0u));});} static void Cyc_Tcpat_rule_occurs(
int dummy, struct _tuple12* rhs){(* rhs).f1= 1;} void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ int _temp916= 0;
struct Cyc_List_List* _temp917=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)(
int*, struct Cyc_Absyn_Switch_clause*), int* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcpat_get_match,& _temp916, swcs); void* _temp918=(( void*(*)( struct Cyc_List_List*
allmrules)) Cyc_Tcpat_match_compile)( _temp917);(( void(*)( void* d, void(*
not_exhaust)( struct Cyc_Position_Segment*), struct Cyc_Position_Segment* env1,
void(* rhs_appears)( int, struct _tuple12*), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp918, Cyc_Tcpat_not_exhaust_err, loc, Cyc_Tcpat_rule_occurs, 0); for( 0;
_temp917 !=  0; _temp917=(( struct Cyc_List_List*) _check_null( _temp917))->tl){
struct _tuple13 _temp921; struct _tuple12* _temp922; struct _tuple12 _temp924;
struct Cyc_Position_Segment* _temp925; int _temp927; struct _tuple13* _temp919=(
struct _tuple13*)(( struct Cyc_List_List*) _check_null( _temp917))->hd; _temp921=*
_temp919; _LL923: _temp922= _temp921.f2; _temp924=* _temp922; _LL928: _temp927=
_temp924.f1; goto _LL926; _LL926: _temp925= _temp924.f2; goto _LL920; _LL920:
if( ! _temp927){({ void* _temp929[ 0u]={}; Cyc_Tcutil_terr( _temp925, _tag_arr("redundant pattern",
sizeof( unsigned char), 18u), _tag_arr( _temp929, sizeof( void*), 0u));});}}}
struct _tuple14{ struct Cyc_Position_Segment* f1; int f2; } ; static void Cyc_Tcpat_not_exhaust_warn(
struct _tuple14* pr){(* pr).f2= 0;({ void* _temp930[ 0u]={}; Cyc_Tcutil_warn((*
pr).f1, _tag_arr("pattern not exhaustive", sizeof( unsigned char), 23u),
_tag_arr( _temp930, sizeof( void*), 0u));});} static void Cyc_Tcpat_dummy_fn(
int i, int j){ return;} struct _tuple15{ void* f1; int f2; } ; int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Pat* p){ struct Cyc_List_List*
_temp931=({ struct Cyc_List_List* _temp935=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp935->hd=( void*)({ struct _tuple15*
_temp936=( struct _tuple15*) _cycalloc( sizeof( struct _tuple15)); _temp936->f1=
Cyc_Tcpat_compile_pat( p); _temp936->f2= 0; _temp936;}); _temp935->tl= 0;
_temp935;}); void* _temp932=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
_temp931); struct _tuple14* _temp933=({ struct _tuple14* _temp934=( struct
_tuple14*) _cycalloc( sizeof( struct _tuple14)); _temp934->f1= loc; _temp934->f2=
1; _temp934;});(( void(*)( void* d, void(* not_exhaust)( struct _tuple14*),
struct _tuple14* env1, void(* rhs_appears)( int, int), int env2)) Cyc_Tcpat_check_exhaust_overlap)(
_temp932, Cyc_Tcpat_not_exhaust_warn, _temp933, Cyc_Tcpat_dummy_fn, 0); return(*
_temp933).f2;} static struct _tuple13* Cyc_Tcpat_get_match2( struct Cyc_Absyn_Switch_clause*
swc){ void* sp0= Cyc_Tcpat_compile_pat( swc->pattern); struct _tuple12* rhs=({
struct _tuple12* _temp938=( struct _tuple12*) _cycalloc( sizeof( struct _tuple12));
_temp938->f1= 0; _temp938->f2=( swc->pattern)->loc; _temp938;}); return({ struct
_tuple13* _temp937=( struct _tuple13*) _cycalloc( sizeof( struct _tuple13));
_temp937->f1= sp0; _temp937->f2= rhs; _temp937;});} static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment* loc){;} void Cyc_Tcpat_check_catch_overlap( struct
Cyc_Position_Segment* loc, struct Cyc_List_List* swcs){ struct Cyc_List_List*
_temp939=(( struct Cyc_List_List*(*)( struct _tuple13*(* f)( struct Cyc_Absyn_Switch_clause*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcpat_get_match2, swcs); void*
_temp940=(( void*(*)( struct Cyc_List_List* allmrules)) Cyc_Tcpat_match_compile)(
_temp939);(( void(*)( void* d, void(* not_exhaust)( struct Cyc_Position_Segment*),
struct Cyc_Position_Segment* env1, void(* rhs_appears)( int, struct _tuple12*),
int env2)) Cyc_Tcpat_check_exhaust_overlap)( _temp940, Cyc_Tcpat_not_exhaust_err2,
loc, Cyc_Tcpat_rule_occurs, 0); for( 0; _temp939 !=  0; _temp939=(( struct Cyc_List_List*)
_check_null( _temp939))->tl){ struct _tuple13 _temp943; struct _tuple12*
_temp944; struct _tuple12 _temp946; struct Cyc_Position_Segment* _temp947; int
_temp949; struct _tuple13* _temp941=( struct _tuple13*)(( struct Cyc_List_List*)
_check_null( _temp939))->hd; _temp943=* _temp941; _LL945: _temp944= _temp943.f2;
_temp946=* _temp944; _LL950: _temp949= _temp946.f1; goto _LL948; _LL948:
_temp947= _temp946.f2; goto _LL942; _LL942: if( ! _temp949){({ void* _temp951[ 0u]={};
Cyc_Tcutil_terr( _temp947, _tag_arr("redundant pattern", sizeof( unsigned char),
18u), _tag_arr( _temp951, sizeof( void*), 0u));});}}}
