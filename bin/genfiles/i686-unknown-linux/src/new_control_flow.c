// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in file lib/runtime_cyc.c
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

//// Discriminated Unions
struct _xtunion_struct { char *tag; };

// Need one of these per thread (we don't have threads)
// The runtime maintains a stack that contains either _handler_cons
// structs or _RegionHandle structs.  The tag is 0 for a handler_cons
// and 1 for a region handle.  
struct _RuntimeStack {
  int tag; // 0 for an exception handler, 1 for a region handle
  struct _RuntimeStack *next;
};

//// Regions
struct _RegionPage {
#ifdef CYC_REGION_PROFILE
  unsigned int total_bytes;
  unsigned int free_bytes;
#endif
  struct _RegionPage *next;
  char data[0];
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
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
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
  return ptr;
}
static inline 
char * _check_known_subscript_null(void * ptr, unsigned bound, 
				   unsigned elt_sz, unsigned index) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return ((char *)ptr) + elt_sz*index;
}
static inline 
unsigned _check_known_subscript_notnull(unsigned bound, unsigned index) {
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
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
#ifndef NO_CYC_NULL_CHECKS
  if(!arr.base) 
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(ans < arr.base || ans >= arr.last_plus_one)
    _throw_arraybounds();
#endif
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
#ifndef NO_CYC_BOUNDS_CHECKS
  if(arr.curr < arr.base || arr.curr + elt_sz * num_elts > arr.last_plus_one)
    _throw_arraybounds();
#endif
  return arr.curr;
}
static inline 
unsigned _get_arr_size(struct _tagged_arr arr, unsigned elt_sz) {
  return (arr.curr<arr.base || arr.curr>=arr.last_plus_one) ? 0 : ((arr.last_plus_one - arr.curr) / elt_sz);
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
extern void * GC_calloc(unsigned int,unsigned int);
extern void * GC_calloc_atomic(unsigned int,unsigned int);

static inline void * _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc_atomic(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc_atomic(n,s);
  if (!ans)
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

#if defined(CYC_REGION_PROFILE) 
extern void * _profile_GC_malloc(int,char *file,int lineno);
extern void * _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void * _profile_region_malloc(struct _RegionHandle *, unsigned int,
                                     char *file,int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						char *file,int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 char *file,int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ":" __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ":" __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ":" __FUNCTION__,__LINE__)
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
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Set_Set; extern struct
Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern struct
Cyc_Set_Set* Cyc_Set_union_two( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2);
extern unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*)); extern
struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* k, void* v);
extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* k); extern int Cyc_Std_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); struct Cyc_Lineno_Pos{ struct
_tagged_arr logical_file; struct _tagged_arr line; int line_no; int col; } ;
extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static
const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static
const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr
source; struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; }
; extern unsigned char Cyc_Position_Nocontext[ 14u]; static const int Cyc_Absyn_Loc_n=
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
} ; static const int Cyc_Absyn_No_constr= 0; static const int Cyc_Absyn_Eq_kb= 0;
struct Cyc_Absyn_Eq_kb_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Unknown_kb=
1; struct Cyc_Absyn_Unknown_kb_struct{ int tag; struct Cyc_Core_Opt* f1; } ;
static const int Cyc_Absyn_Less_kb= 2; struct Cyc_Absyn_Less_kb_struct{ int tag;
struct Cyc_Core_Opt* f1; void* f2; } ; struct Cyc_Absyn_Tvar{ struct _tagged_arr*
name; int* identity; void* kind; } ; static const int Cyc_Absyn_Unknown_b= 0;
static const int Cyc_Absyn_Upper_b= 0; struct Cyc_Absyn_Upper_b_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{ void* elt_typ; void*
rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual tq; struct
Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt*
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
Cyc_Absyn_SizeofType= 13; struct Cyc_Absyn_SizeofType_struct{ int tag; void* f1;
} ; static const int Cyc_Absyn_AnonStructType= 14; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
15; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonEnumType= 16; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
17; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 18; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 2; static const int Cyc_Absyn_AccessEff= 19;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
20; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 21; struct Cyc_Absyn_RgnsEff_struct{ int tag;
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
int tag; unsigned int f1; } ; struct Cyc_Absyn_MallocInfo{ int is_calloc; struct
Cyc_Absyn_Exp* rgn; void** elt_type; struct Cyc_Absyn_Exp* num_elts; int
fat_result; } ; static const int Cyc_Absyn_Const_e= 0; struct Cyc_Absyn_Const_e_struct{
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
tag; struct Cyc_Absyn_MallocInfo f1; } ; static const int Cyc_Absyn_UnresolvedMem_e=
34; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 35; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 36; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 37; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; void* annot; } ; struct _tuple3{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_ForArrayInfo{
struct Cyc_List_List* defns; struct _tuple3 condition; struct _tuple3 delta;
struct Cyc_Absyn_Stmt* body; } ; static const int Cyc_Absyn_Skip_s= 0; static
const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int
Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct Cyc_Absyn_While_s_struct{
int tag; struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
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
static const int Cyc_Absyn_ForArray_s= 19; struct Cyc_Absyn_ForArray_s_struct{
int tag; struct Cyc_Absyn_ForArrayInfo f1; } ; struct Cyc_Absyn_Stmt{ void* r;
struct Cyc_Position_Segment* loc; struct Cyc_List_List* non_local_preds; int
try_depth; void* annot; } ; static const int Cyc_Absyn_Wild_p= 0; static const
int Cyc_Absyn_Var_p= 0; struct Cyc_Absyn_Var_p_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Null_p= 1; static const int Cyc_Absyn_Int_p=
1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1; int f2; } ; static const
int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{ int tag; unsigned char
f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
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
struct Cyc_List_List* attributes; int escapes; } ; struct Cyc_Absyn_Fndecl{ void*
sc; int is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
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
15u]; extern struct _tagged_arr* Cyc_Absyn_fieldname( int); struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[ 19u];
extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
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
extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*, struct _tagged_arr
fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_compress( void* t); extern
int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref* b); extern unsigned int
Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); static const int Cyc_CfFlowInfo_VarRoot=
0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_CfFlowInfo_Place{ void* root;
struct Cyc_List_List* fields; } ; static const int Cyc_CfFlowInfo_NoneIL= 0;
static const int Cyc_CfFlowInfo_ThisIL= 1; static const int Cyc_CfFlowInfo_AllIL=
2; static const int Cyc_CfFlowInfo_EqualConst= 0; struct Cyc_CfFlowInfo_EqualConst_struct{
int tag; unsigned int f1; } ; static const int Cyc_CfFlowInfo_LessVar= 1; struct
Cyc_CfFlowInfo_LessVar_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_CfFlowInfo_LessSize= 2; struct Cyc_CfFlowInfo_LessSize_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_LessConst=
3; struct Cyc_CfFlowInfo_LessConst_struct{ int tag; unsigned int f1; } ; static
const int Cyc_CfFlowInfo_LessEqSize= 4; struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; struct Cyc_CfFlowInfo_Reln{ struct
Cyc_Absyn_Vardecl* vd; void* rop; } ; extern unsigned char Cyc_CfFlowInfo_IsZero[
11u]; struct Cyc_CfFlowInfo_IsZero_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_CfFlowInfo_NotZero[ 12u]; struct Cyc_CfFlowInfo_NotZero_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_CfFlowInfo_UnknownZ[
13u]; struct Cyc_CfFlowInfo_UnknownZ_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; static const int Cyc_CfFlowInfo_PlaceL= 0; struct Cyc_CfFlowInfo_PlaceL_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_UnknownL=
0; static const int Cyc_CfFlowInfo_Zero= 0; static const int Cyc_CfFlowInfo_NotZeroAll=
1; static const int Cyc_CfFlowInfo_NotZeroThis= 2; static const int Cyc_CfFlowInfo_UnknownR=
0; struct Cyc_CfFlowInfo_UnknownR_struct{ int tag; void* f1; } ; static const
int Cyc_CfFlowInfo_Esc= 1; struct Cyc_CfFlowInfo_Esc_struct{ int tag; void* f1;
} ; static const int Cyc_CfFlowInfo_AddressOf= 2; struct Cyc_CfFlowInfo_AddressOf_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_Aggregate=
3; struct Cyc_CfFlowInfo_Aggregate_struct{ int tag; struct Cyc_Dict_Dict* f1; }
; static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_ReachableFL=
0; struct Cyc_CfFlowInfo_ReachableFL_struct{ int tag; struct Cyc_Dict_Dict* f1;
struct Cyc_List_List* f2; } ; extern struct Cyc_Set_Set* Cyc_CfFlowInfo_mt_place_set();
extern void* Cyc_CfFlowInfo_unknown_none; extern void* Cyc_CfFlowInfo_unknown_all;
extern int Cyc_CfFlowInfo_root_cmp( void*, void*); extern void* Cyc_CfFlowInfo_typ_to_absrval(
void* t, void* leafval); extern void* Cyc_CfFlowInfo_initlevel( struct Cyc_Dict_Dict*
d, void* r); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d,
struct Cyc_CfFlowInfo_Place* place); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); extern struct Cyc_List_List* Cyc_CfFlowInfo_reln_assign_var(
struct Cyc_List_List*, struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Exp*); extern
struct Cyc_List_List* Cyc_CfFlowInfo_reln_assign_exp( struct Cyc_List_List*,
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*); extern struct Cyc_List_List* Cyc_CfFlowInfo_reln_kill_exp(
struct Cyc_List_List*, struct Cyc_Absyn_Exp*); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict* d, struct Cyc_Set_Set** all_changed, void* r); extern
struct Cyc_Dict_Dict* Cyc_CfFlowInfo_assign_place( struct Cyc_Position_Segment*
loc, struct Cyc_Dict_Dict* d, struct Cyc_Set_Set** all_changed, struct Cyc_CfFlowInfo_Place*
place, void* r); extern void* Cyc_CfFlowInfo_join_flow( struct Cyc_Set_Set**,
void*, void*); extern void* Cyc_CfFlowInfo_after_flow( struct Cyc_Set_Set**,
void*, void*, struct Cyc_Set_Set*, struct Cyc_Set_Set*); extern void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt* enclosee, struct Cyc_Absyn_Stmt* encloser); extern void
Cyc_NewControlFlow_cf_check( struct Cyc_List_List* ds); struct Cyc_PP_Ppstate;
struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{ int
expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int generate_line_directives: 1; int use_curr_namespace: 1;
struct Cyc_List_List* curr_namespace; } ; struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt* encloser; int visited; void* flow; } ; static
unsigned char Cyc_NewControlFlow_CFAnnot[ 12u]="\000\000\000\000CFAnnot"; struct
Cyc_NewControlFlow_CFAnnot_struct{ unsigned char* tag; struct Cyc_NewControlFlow_CFStmtAnnot
f1; } ; void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser){( void*)( enclosee->annot=( void*)(( void*)({
struct Cyc_NewControlFlow_CFAnnot_struct* _temp0=( struct Cyc_NewControlFlow_CFAnnot_struct*)
_cycalloc( sizeof( struct Cyc_NewControlFlow_CFAnnot_struct)); _temp0[ 0]=({
struct Cyc_NewControlFlow_CFAnnot_struct _temp1; _temp1.tag= Cyc_NewControlFlow_CFAnnot;
_temp1.f1=({ struct Cyc_NewControlFlow_CFStmtAnnot _temp2; _temp2.encloser=
encloser; _temp2.visited= 0; _temp2.flow=( void*)(( void*) Cyc_CfFlowInfo_BottomFL);
_temp2;}); _temp1;}); _temp0;})));} static struct Cyc_NewControlFlow_CFStmtAnnot*
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
unsigned char), 37u); _temp13;}); _temp12;})); _LL4:;} struct Cyc_NewControlFlow_AnalEnv{
int iterate_again; int iteration_num; int in_try; void* tryflow; struct Cyc_Set_Set**
all_changed; } ; static void* Cyc_NewControlFlow_anal_stmt( struct Cyc_NewControlFlow_AnalEnv*,
void*, struct Cyc_Absyn_Stmt*); static void* Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*, void*, struct Cyc_Absyn_Decl*); static
struct _tuple0 Cyc_NewControlFlow_anal_Lexp( struct Cyc_NewControlFlow_AnalEnv*,
void*, struct Cyc_Absyn_Exp*); static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*, void*, struct Cyc_Absyn_Exp*); static struct
_tuple0 Cyc_NewControlFlow_anal_test( struct Cyc_NewControlFlow_AnalEnv*, void*,
struct Cyc_Absyn_Exp*); static struct Cyc_NewControlFlow_CFStmtAnnot* Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Stmt* s){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp14= Cyc_NewControlFlow_get_stmt_annot(
s);( void*)( _temp14->flow=( void*) Cyc_CfFlowInfo_join_flow( env->all_changed,
inflow,( void*) _temp14->flow)); _temp14->visited= env->iteration_num; return
_temp14;} static void Cyc_NewControlFlow_update_tryflow( struct Cyc_NewControlFlow_AnalEnv*
env, void* new_flow){ if( env->in_try){( void*)( env->tryflow=( void*) Cyc_CfFlowInfo_join_flow(
env->all_changed, new_flow,( void*) env->tryflow));}} static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Absyn_Stmt* s, void* flow){
struct Cyc_NewControlFlow_CFStmtAnnot* _temp15= Cyc_NewControlFlow_get_stmt_annot(
s); void* _temp16= Cyc_CfFlowInfo_join_flow( env->all_changed, flow,( void*)
_temp15->flow); if( ! Cyc_CfFlowInfo_flow_lessthan_approx( _temp16,( void*)
_temp15->flow)){( void*)( _temp15->flow=( void*) _temp16); if( _temp15->visited
==  env->iteration_num){ env->iterate_again= 1;}}} static void* Cyc_NewControlFlow_add_vars(
void* inflow, struct Cyc_List_List* vds, void* leafval){ void* _temp17= inflow;
struct Cyc_List_List* _temp23; struct Cyc_Dict_Dict* _temp25; _LL19: if( _temp17
== ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL20;} else{ goto _LL21;} _LL21: if((
unsigned int) _temp17 >  1u?*(( int*) _temp17) ==  Cyc_CfFlowInfo_ReachableFL: 0){
_LL26: _temp25=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp17)->f1; goto
_LL24; _LL24: _temp23=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp17)->f2;
goto _LL22;} else{ goto _LL18;} _LL20: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL22: for( 0; vds !=  0; vds= vds->tl){ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp27=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp28=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp28[ 0]=({ struct
Cyc_CfFlowInfo_VarRoot_struct _temp29; _temp29.tag= Cyc_CfFlowInfo_VarRoot;
_temp29.f1=( struct Cyc_Absyn_Vardecl*) vds->hd; _temp29;}); _temp28;}); _temp25=
Cyc_Dict_insert( _temp25,( void*) _temp27, Cyc_CfFlowInfo_typ_to_absrval(( void*)((
struct Cyc_Absyn_Vardecl*) vds->hd)->type, leafval));} return( void*)({ struct
Cyc_CfFlowInfo_ReachableFL_struct* _temp30=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp30[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp31; _temp31.tag= Cyc_CfFlowInfo_ReachableFL;
_temp31.f1= _temp25; _temp31.f2= _temp23; _temp31;}); _temp30;}); _LL18:;}
static void* Cyc_NewControlFlow_use_Rval( struct Cyc_NewControlFlow_AnalEnv* env,
struct Cyc_Position_Segment* loc, void* inflow, void* r){ void* _temp32= inflow;
struct Cyc_List_List* _temp38; struct Cyc_Dict_Dict* _temp40; _LL34: if( _temp32
== ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL35;} else{ goto _LL36;} _LL36: if((
unsigned int) _temp32 >  1u?*(( int*) _temp32) ==  Cyc_CfFlowInfo_ReachableFL: 0){
_LL41: _temp40=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp32)->f1; goto
_LL39; _LL39: _temp38=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp32)->f2;
goto _LL37;} else{ goto _LL33;} _LL35: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL37: if( Cyc_CfFlowInfo_initlevel( _temp40, r) != ( void*) Cyc_CfFlowInfo_AllIL){({
void* _temp42[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("expression may not be fully initialized",
sizeof( unsigned char), 40u), _tag_arr( _temp42, sizeof( void*), 0u));});}{
struct Cyc_Dict_Dict* _temp43= Cyc_CfFlowInfo_escape_deref( _temp40, env->all_changed,
r); if( _temp40 ==  _temp43){ return inflow;}{ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp44=({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp45=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp45[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp46; _temp46.tag= Cyc_CfFlowInfo_ReachableFL;
_temp46.f1= _temp43; _temp46.f2= _temp38; _temp46;}); _temp45;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp44); return( void*) _temp44;}} _LL33:;} struct _tuple4{ void*
f1; struct Cyc_List_List* f2; } ; static struct _tuple4 Cyc_NewControlFlow_anal_unordered_Rexps(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_List_List* es){
if( es ==  0){ return({ struct _tuple4 _temp47; _temp47.f1= inflow; _temp47.f2=
0; _temp47;});} if( es->tl ==  0){ void* _temp50; void* _temp52; struct _tuple0
_temp48= Cyc_NewControlFlow_anal_Rexp( env, inflow,( struct Cyc_Absyn_Exp*) es->hd);
_LL53: _temp52= _temp48.f1; goto _LL51; _LL51: _temp50= _temp48.f2; goto _LL49;
_LL49: return({ struct _tuple4 _temp54; _temp54.f1= _temp52; _temp54.f2=({
struct Cyc_List_List* _temp55=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp55->hd=( void*) _temp50; _temp55->tl= 0; _temp55;});
_temp54;});}{ struct Cyc_Set_Set** outer_all_changed= env->all_changed; struct
Cyc_Set_Set* this_all_changed; void* old_inflow; void* outflow; struct Cyc_List_List*
rvals; do { this_all_changed= Cyc_CfFlowInfo_mt_place_set(); env->all_changed=({
struct Cyc_Set_Set** _temp56=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp56[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp56;});{ void*
_temp59; void* _temp61; struct _tuple0 _temp57= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*) es->hd); _LL62: _temp61= _temp57.f1; goto
_LL60; _LL60: _temp59= _temp57.f2; goto _LL58; _LL58: outflow= _temp61; rvals=({
struct Cyc_List_List* _temp63=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp63->hd=( void*) _temp59; _temp63->tl= 0; _temp63;});
this_all_changed=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2)) Cyc_Set_union_two)( this_all_changed,*(( struct Cyc_Set_Set**) _check_null(
env->all_changed)));{ struct Cyc_List_List* es2= es->tl; for( 0; es2 !=  0; es2=
es2->tl){ env->all_changed=({ struct Cyc_Set_Set** _temp64=( struct Cyc_Set_Set**)
_cycalloc( sizeof( struct Cyc_Set_Set*)); _temp64[ 0]= Cyc_CfFlowInfo_mt_place_set();
_temp64;});{ void* _temp67; void* _temp69; struct _tuple0 _temp65= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*) es2->hd); _LL70: _temp69= _temp65.f1; goto
_LL68; _LL68: _temp67= _temp65.f2; goto _LL66; _LL66: rvals=({ struct Cyc_List_List*
_temp71=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp71->hd=( void*) _temp67; _temp71->tl= rvals; _temp71;}); outflow= Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& this_all_changed, outflow, _temp69, this_all_changed,*((
struct Cyc_Set_Set**) _check_null( env->all_changed))); this_all_changed=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(
this_all_changed,*(( struct Cyc_Set_Set**) _check_null( env->all_changed)));}}}
old_inflow= inflow; inflow= Cyc_CfFlowInfo_join_flow( outer_all_changed, inflow,
outflow);}} while ( ! Cyc_CfFlowInfo_flow_lessthan_approx( inflow, old_inflow));
if( outer_all_changed ==  0){ env->all_changed= 0;} else{ env->all_changed=({
struct Cyc_Set_Set** _temp72=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp72[ 0]=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1,
struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(* outer_all_changed,
this_all_changed); _temp72;});} Cyc_NewControlFlow_update_tryflow( env, outflow);
return({ struct _tuple4 _temp73; _temp73.f1= outflow; _temp73.f2= Cyc_List_imp_rev(
rvals); _temp73;});}} static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_List_List* es){
struct Cyc_List_List* _temp76; void* _temp78; struct _tuple4 _temp74= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, es); _LL79: _temp78= _temp74.f1; goto _LL77; _LL77: _temp76=
_temp74.f2; goto _LL75; _LL75:{ void* _temp80= _temp78; struct Cyc_List_List*
_temp86; struct Cyc_Dict_Dict* _temp88; _LL82: if(( unsigned int) _temp80 >  1u?*((
int*) _temp80) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL89: _temp88=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp80)->f1; goto _LL87; _LL87: _temp86=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp80)->f2; goto _LL83;} else{ goto _LL84;} _LL84: if( _temp80 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL85;} else{ goto _LL81;} _LL83: for( 0; _temp76 !=  0;( _temp76= _temp76->tl,
es=(( struct Cyc_List_List*) _check_null( es))->tl)){ if( Cyc_CfFlowInfo_initlevel(
_temp88,( void*) _temp76->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void*
_temp90[ 0u]={}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp90, sizeof( void*), 0u));});}} goto
_LL81; _LL85: goto _LL81; _LL81:;} return({ struct _tuple0 _temp91; _temp91.f1=
_temp78; _temp91.f2= Cyc_CfFlowInfo_unknown_all; _temp91;});} static void* Cyc_NewControlFlow_notzero(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, void* outflow, struct Cyc_Absyn_Exp*
e, void* il){ void* _temp92= outflow; struct Cyc_List_List* _temp98; struct Cyc_Dict_Dict*
_temp100; _LL94: if( _temp92 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL95;}
else{ goto _LL96;} _LL96: if(( unsigned int) _temp92 >  1u?*(( int*) _temp92) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL101: _temp100=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp92)->f1; goto _LL99; _LL99: _temp98=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp92)->f2; goto _LL97;} else{ goto _LL93;} _LL95: return outflow; _LL97: {
void* _temp102=( Cyc_NewControlFlow_anal_Lexp( env, inflow, e)).f2; struct Cyc_CfFlowInfo_Place*
_temp108; _LL104: if( _temp102 == ( void*) Cyc_CfFlowInfo_UnknownL){ goto _LL105;}
else{ goto _LL106;} _LL106: if(( unsigned int) _temp102 >  1u?*(( int*) _temp102)
==  Cyc_CfFlowInfo_PlaceL: 0){ _LL109: _temp108=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp102)->f1; goto _LL107;} else{ goto _LL103;} _LL105: return outflow; _LL107: {
void* nzval= il == ( void*) Cyc_CfFlowInfo_AllIL?( void*) Cyc_CfFlowInfo_NotZeroAll:(
void*) Cyc_CfFlowInfo_NotZeroThis; return( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp110=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp110[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp111; _temp111.tag= Cyc_CfFlowInfo_ReachableFL; _temp111.f1= Cyc_CfFlowInfo_assign_place(
e->loc, _temp100, env->all_changed, _temp108, nzval); _temp111.f2= _temp98;
_temp111;}); _temp110;});} _LL103:;} _LL93:;} static struct _tuple0 Cyc_NewControlFlow_splitzero(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, void* outflow, struct Cyc_Absyn_Exp*
e, void* il){ void* _temp112= outflow; struct Cyc_List_List* _temp118; struct
Cyc_Dict_Dict* _temp120; _LL114: if( _temp112 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL115;} else{ goto _LL116;} _LL116: if(( unsigned int) _temp112 >  1u?*((
int*) _temp112) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL121: _temp120=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp112)->f1; goto _LL119; _LL119: _temp118=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp112)->f2; goto _LL117;} else{
goto _LL113;} _LL115: return({ struct _tuple0 _temp122; _temp122.f1= outflow;
_temp122.f2= outflow; _temp122;}); _LL117: { void* _temp123=( Cyc_NewControlFlow_anal_Lexp(
env, inflow, e)).f2; struct Cyc_CfFlowInfo_Place* _temp129; _LL125: if( _temp123
== ( void*) Cyc_CfFlowInfo_UnknownL){ goto _LL126;} else{ goto _LL127;} _LL127:
if(( unsigned int) _temp123 >  1u?*(( int*) _temp123) ==  Cyc_CfFlowInfo_PlaceL:
0){ _LL130: _temp129=(( struct Cyc_CfFlowInfo_PlaceL_struct*) _temp123)->f1;
goto _LL128;} else{ goto _LL124;} _LL126: return({ struct _tuple0 _temp131;
_temp131.f1= outflow; _temp131.f2= outflow; _temp131;}); _LL128: { void* nzval=
il == ( void*) Cyc_CfFlowInfo_AllIL?( void*) Cyc_CfFlowInfo_NotZeroAll:( void*)
Cyc_CfFlowInfo_NotZeroThis; return({ struct _tuple0 _temp132; _temp132.f1=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp135=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp135[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp136; _temp136.tag= Cyc_CfFlowInfo_ReachableFL;
_temp136.f1= Cyc_CfFlowInfo_assign_place( e->loc, _temp120, env->all_changed,
_temp129, nzval); _temp136.f2= _temp118; _temp136;}); _temp135;}); _temp132.f2=(
void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp133=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp133[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp134; _temp134.tag= Cyc_CfFlowInfo_ReachableFL;
_temp134.f1= Cyc_CfFlowInfo_assign_place( e->loc, _temp120, env->all_changed,
_temp129,( void*) Cyc_CfFlowInfo_Zero); _temp134.f2= _temp118; _temp134;});
_temp133;}); _temp132;});} _LL124:;} _LL113:;} static struct _tuple0 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, void* f, struct Cyc_Absyn_Exp*
e, void* r){ void* _temp137= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo _temp143; struct Cyc_Absyn_Conref*
_temp145; void* _temp147; _LL139: if(( unsigned int) _temp137 >  3u?*(( int*)
_temp137) ==  Cyc_Absyn_PointerType: 0){ _LL144: _temp143=(( struct Cyc_Absyn_PointerType_struct*)
_temp137)->f1; _LL148: _temp147=( void*) _temp143.elt_typ; goto _LL146; _LL146:
_temp145= _temp143.bounds; goto _LL140;} else{ goto _LL141;} _LL141: goto _LL142;
_LL140: { void* _temp149= f; struct Cyc_List_List* _temp155; struct Cyc_Dict_Dict*
_temp157; _LL151: if( _temp149 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL152;}
else{ goto _LL153;} _LL153: if(( unsigned int) _temp149 >  1u?*(( int*) _temp149)
==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL158: _temp157=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp149)->f1; goto _LL156; _LL156: _temp155=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp149)->f2; goto _LL154;} else{ goto _LL150;} _LL152: return({ struct _tuple0
_temp159; _temp159.f1= f; _temp159.f2= Cyc_CfFlowInfo_typ_to_absrval( _temp147,
Cyc_CfFlowInfo_unknown_all); _temp159;}); _LL154: if( Cyc_Tcutil_is_bound_one(
_temp145)){ void* _temp160= r; struct Cyc_CfFlowInfo_Place* _temp174; void*
_temp176; _LL162: if( _temp160 == ( void*) Cyc_CfFlowInfo_NotZeroAll){ goto
_LL163;} else{ goto _LL164;} _LL164: if( _temp160 == ( void*) Cyc_CfFlowInfo_NotZeroThis){
goto _LL165;} else{ goto _LL166;} _LL166: if(( unsigned int) _temp160 >  3u?*((
int*) _temp160) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL175: _temp174=(( struct Cyc_CfFlowInfo_AddressOf_struct*)
_temp160)->f1; goto _LL167;} else{ goto _LL168;} _LL168: if( _temp160 == ( void*)
Cyc_CfFlowInfo_Zero){ goto _LL169;} else{ goto _LL170;} _LL170: if((
unsigned int) _temp160 >  3u?*(( int*) _temp160) ==  Cyc_CfFlowInfo_UnknownR: 0){
_LL177: _temp176=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*) _temp160)->f1;
goto _LL171;} else{ goto _LL172;} _LL172: goto _LL173; _LL163: goto _LL165;
_LL165:( void*)( e->annot=( void*)(( void*)({ struct Cyc_CfFlowInfo_NotZero_struct*
_temp178=( struct Cyc_CfFlowInfo_NotZero_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_NotZero_struct));
_temp178[ 0]=({ struct Cyc_CfFlowInfo_NotZero_struct _temp179; _temp179.tag= Cyc_CfFlowInfo_NotZero;
_temp179.f1= _temp155; _temp179;}); _temp178;}))); goto _LL161; _LL167:( void*)(
e->annot=( void*)(( void*)({ struct Cyc_CfFlowInfo_NotZero_struct* _temp180=(
struct Cyc_CfFlowInfo_NotZero_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_NotZero_struct));
_temp180[ 0]=({ struct Cyc_CfFlowInfo_NotZero_struct _temp181; _temp181.tag= Cyc_CfFlowInfo_NotZero;
_temp181.f1= _temp155; _temp181;}); _temp180;}))); return({ struct _tuple0
_temp182; _temp182.f1= f; _temp182.f2= Cyc_CfFlowInfo_lookup_place( _temp157,
_temp174); _temp182;}); _LL169:( void*)( e->annot=( void*)(( void*)({ struct Cyc_CfFlowInfo_IsZero_struct*
_temp183=( struct Cyc_CfFlowInfo_IsZero_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_IsZero_struct));
_temp183[ 0]=({ struct Cyc_CfFlowInfo_IsZero_struct _temp184; _temp184.tag= Cyc_CfFlowInfo_IsZero;
_temp184.f1= _temp155; _temp184;}); _temp183;}))); return({ struct _tuple0
_temp185; _temp185.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp185.f2= Cyc_CfFlowInfo_typ_to_absrval(
_temp147, Cyc_CfFlowInfo_unknown_all); _temp185;}); _LL171: f= Cyc_NewControlFlow_notzero(
env, inflow, f, e, _temp176); goto _LL173; _LL173:( void*)( e->annot=( void*)((
void*)({ struct Cyc_CfFlowInfo_UnknownZ_struct* _temp186=( struct Cyc_CfFlowInfo_UnknownZ_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_UnknownZ_struct)); _temp186[ 0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _temp187; _temp187.tag= Cyc_CfFlowInfo_UnknownZ;
_temp187.f1= _temp155; _temp187;}); _temp186;}))); _LL161:;} else{( void*)( e->annot=(
void*)(( void*)({ struct Cyc_CfFlowInfo_UnknownZ_struct* _temp188=( struct Cyc_CfFlowInfo_UnknownZ_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_UnknownZ_struct)); _temp188[ 0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _temp189; _temp189.tag= Cyc_CfFlowInfo_UnknownZ;
_temp189.f1= _temp155; _temp189;}); _temp188;})));}{ void* _temp190= Cyc_CfFlowInfo_initlevel(
_temp157, r); _LL192: if( _temp190 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto
_LL193;} else{ goto _LL194;} _LL194: if( _temp190 == ( void*) Cyc_CfFlowInfo_AllIL){
goto _LL195;} else{ goto _LL196;} _LL196: if( _temp190 == ( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL197;} else{ goto _LL191;} _LL193:({ void* _temp198[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("dereference of possibly uninitialized pointer", sizeof(
unsigned char), 46u), _tag_arr( _temp198, sizeof( void*), 0u));}); goto _LL195;
_LL195: return({ struct _tuple0 _temp199; _temp199.f1= f; _temp199.f2= Cyc_CfFlowInfo_typ_to_absrval(
_temp147, Cyc_CfFlowInfo_unknown_all); _temp199;}); _LL197: return({ struct
_tuple0 _temp200; _temp200.f1= f; _temp200.f2= Cyc_CfFlowInfo_typ_to_absrval(
_temp147, Cyc_CfFlowInfo_unknown_none); _temp200;}); _LL191:;} _LL150:;} _LL142:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp201=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp201[ 0]=({ struct
Cyc_Core_Impossible_struct _temp202; _temp202.tag= Cyc_Core_Impossible; _temp202.f1=
_tag_arr("right deref of non-pointer-type", sizeof( unsigned char), 32u);
_temp202;}); _temp201;})); _LL138:;} static struct Cyc_List_List* Cyc_NewControlFlow_add_subscript_reln(
struct Cyc_List_List* relns, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){
void* _temp203=( void*) e1->r; void* _temp215; struct Cyc_Absyn_Vardecl*
_temp217; void* _temp219; struct Cyc_Absyn_Vardecl* _temp221; void* _temp223;
struct Cyc_Absyn_Vardecl* _temp225; void* _temp227; struct Cyc_Absyn_Vardecl*
_temp229; _LL205: if(*(( int*) _temp203) ==  Cyc_Absyn_Var_e){ _LL216: _temp215=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp203)->f2; if(( unsigned int)
_temp215 >  1u?*(( int*) _temp215) ==  Cyc_Absyn_Pat_b: 0){ _LL218: _temp217=((
struct Cyc_Absyn_Pat_b_struct*) _temp215)->f1; goto _LL206;} else{ goto _LL207;}}
else{ goto _LL207;} _LL207: if(*(( int*) _temp203) ==  Cyc_Absyn_Var_e){ _LL220:
_temp219=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp203)->f2; if((
unsigned int) _temp219 >  1u?*(( int*) _temp219) ==  Cyc_Absyn_Local_b: 0){
_LL222: _temp221=(( struct Cyc_Absyn_Local_b_struct*) _temp219)->f1; goto _LL208;}
else{ goto _LL209;}} else{ goto _LL209;} _LL209: if(*(( int*) _temp203) ==  Cyc_Absyn_Var_e){
_LL224: _temp223=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp203)->f2; if((
unsigned int) _temp223 >  1u?*(( int*) _temp223) ==  Cyc_Absyn_Param_b: 0){
_LL226: _temp225=(( struct Cyc_Absyn_Param_b_struct*) _temp223)->f1; goto _LL210;}
else{ goto _LL211;}} else{ goto _LL211;} _LL211: if(*(( int*) _temp203) ==  Cyc_Absyn_Var_e){
_LL228: _temp227=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp203)->f2; if((
unsigned int) _temp227 >  1u?*(( int*) _temp227) ==  Cyc_Absyn_Global_b: 0){
_LL230: _temp229=(( struct Cyc_Absyn_Global_b_struct*) _temp227)->f1; goto
_LL212;} else{ goto _LL213;}} else{ goto _LL213;} _LL213: goto _LL214; _LL206:
_temp221= _temp217; goto _LL208; _LL208: _temp225= _temp221; goto _LL210; _LL210:
_temp229= _temp225; goto _LL212; _LL212: if( ! _temp229->escapes){ void*
_temp231=( void*) e2->r; void* _temp243; struct Cyc_Absyn_Vardecl* _temp245;
void* _temp247; struct Cyc_Absyn_Vardecl* _temp249; void* _temp251; struct Cyc_Absyn_Vardecl*
_temp253; void* _temp255; struct Cyc_Absyn_Vardecl* _temp257; _LL233: if(*(( int*)
_temp231) ==  Cyc_Absyn_Var_e){ _LL244: _temp243=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp231)->f2; if(( unsigned int) _temp243 >  1u?*(( int*) _temp243) ==  Cyc_Absyn_Pat_b:
0){ _LL246: _temp245=(( struct Cyc_Absyn_Pat_b_struct*) _temp243)->f1; goto
_LL234;} else{ goto _LL235;}} else{ goto _LL235;} _LL235: if(*(( int*) _temp231)
==  Cyc_Absyn_Var_e){ _LL248: _temp247=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp231)->f2; if(( unsigned int) _temp247 >  1u?*(( int*) _temp247) ==  Cyc_Absyn_Local_b:
0){ _LL250: _temp249=(( struct Cyc_Absyn_Local_b_struct*) _temp247)->f1; goto
_LL236;} else{ goto _LL237;}} else{ goto _LL237;} _LL237: if(*(( int*) _temp231)
==  Cyc_Absyn_Var_e){ _LL252: _temp251=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp231)->f2; if(( unsigned int) _temp251 >  1u?*(( int*) _temp251) ==  Cyc_Absyn_Param_b:
0){ _LL254: _temp253=(( struct Cyc_Absyn_Param_b_struct*) _temp251)->f1; goto
_LL238;} else{ goto _LL239;}} else{ goto _LL239;} _LL239: if(*(( int*) _temp231)
==  Cyc_Absyn_Var_e){ _LL256: _temp255=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp231)->f2; if(( unsigned int) _temp255 >  1u?*(( int*) _temp255) ==  Cyc_Absyn_Global_b:
0){ _LL258: _temp257=(( struct Cyc_Absyn_Global_b_struct*) _temp255)->f1; goto
_LL240;} else{ goto _LL241;}} else{ goto _LL241;} _LL241: goto _LL242; _LL234:
_temp249= _temp245; goto _LL236; _LL236: _temp253= _temp249; goto _LL238; _LL238:
_temp257= _temp253; goto _LL240; _LL240: if( ! _temp257->escapes){ int found= 0;{
struct Cyc_List_List* _temp259= relns; for( 0; _temp259 !=  0; _temp259=
_temp259->tl){ struct Cyc_CfFlowInfo_Reln* _temp260=( struct Cyc_CfFlowInfo_Reln*)
_temp259->hd; if( _temp260->vd ==  _temp257){ void* _temp261=( void*) _temp260->rop;
struct Cyc_Absyn_Vardecl* _temp268; _LL263: if(*(( int*) _temp261) ==  Cyc_CfFlowInfo_LessSize){
_LL269: _temp268=(( struct Cyc_CfFlowInfo_LessSize_struct*) _temp261)->f1; goto
_LL267;} else{ goto _LL265;} _LL267: if( _temp268 ==  _temp229){ goto _LL264;}
else{ goto _LL265;} _LL265: goto _LL266; _LL264: return relns; _LL266: continue;
_LL262:;}}} if( ! found){ return({ struct Cyc_List_List* _temp270=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp270->hd=( void*)({ struct Cyc_CfFlowInfo_Reln*
_temp271=( struct Cyc_CfFlowInfo_Reln*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln));
_temp271->vd= _temp257; _temp271->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessSize_struct*
_temp272=( struct Cyc_CfFlowInfo_LessSize_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_LessSize_struct));
_temp272[ 0]=({ struct Cyc_CfFlowInfo_LessSize_struct _temp273; _temp273.tag=
Cyc_CfFlowInfo_LessSize; _temp273.f1= _temp229; _temp273;}); _temp272;}));
_temp271;}); _temp270->tl= relns; _temp270;});}} return relns; _LL242: return
relns; _LL232:;} else{ return relns;} _LL214: return relns; _LL204:;} struct
_tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static struct
_tuple0 Cyc_NewControlFlow_anal_Rexp( struct Cyc_NewControlFlow_AnalEnv* env,
void* inflow, struct Cyc_Absyn_Exp* e){ struct Cyc_Dict_Dict* d; struct Cyc_List_List*
relns;{ void* _temp274= inflow; struct Cyc_List_List* _temp280; struct Cyc_Dict_Dict*
_temp282; _LL276: if( _temp274 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL277;}
else{ goto _LL278;} _LL278: if(( unsigned int) _temp274 >  1u?*(( int*) _temp274)
==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL283: _temp282=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp274)->f1; goto _LL281; _LL281: _temp280=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp274)->f2; goto _LL279;} else{ goto _LL275;} _LL277: return({ struct _tuple0
_temp284; _temp284.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp284.f2= Cyc_CfFlowInfo_unknown_all;
_temp284;}); _LL279: d= _temp282; relns= _temp280; _LL275:;}{ void* _temp285=(
void*) e->r; struct Cyc_Absyn_Exp* _temp383; struct Cyc_Absyn_Exp* _temp385;
struct Cyc_Absyn_Exp* _temp387; void* _temp389; void* _temp391; int _temp393;
void* _temp395; void* _temp397; void* _temp399; void* _temp401; struct Cyc_List_List*
_temp403; void* _temp405; void* _temp407; struct Cyc_Absyn_Vardecl* _temp409;
void* _temp411; struct Cyc_Absyn_Vardecl* _temp413; void* _temp415; struct Cyc_Absyn_Vardecl*
_temp417; struct Cyc_List_List* _temp419; struct Cyc_Absyn_Exp* _temp421; struct
Cyc_Absyn_Exp* _temp423; struct Cyc_Core_Opt* _temp425; struct Cyc_Core_Opt
_temp427; struct Cyc_Absyn_Exp* _temp428; struct Cyc_Absyn_Exp* _temp430; struct
Cyc_Core_Opt* _temp432; struct Cyc_Absyn_Exp* _temp434; struct Cyc_Absyn_Exp*
_temp436; struct Cyc_Absyn_Exp* _temp438; struct Cyc_Absyn_Exp* _temp440; struct
Cyc_List_List* _temp442; struct Cyc_Absyn_Exp* _temp444; struct Cyc_Absyn_MallocInfo
_temp446; int _temp448; struct Cyc_Absyn_Exp* _temp450; void** _temp452; struct
Cyc_Absyn_Exp* _temp454; int _temp456; struct Cyc_Absyn_Exp* _temp458; struct
Cyc_Absyn_Exp* _temp460; struct Cyc_Absyn_Exp* _temp462; struct Cyc_Absyn_Exp*
_temp464; struct _tagged_arr* _temp466; struct Cyc_Absyn_Exp* _temp468; struct
_tagged_arr* _temp470; struct Cyc_Absyn_Exp* _temp472; struct Cyc_Absyn_Exp*
_temp474; struct Cyc_Absyn_Exp* _temp476; struct Cyc_Absyn_Exp* _temp478; struct
Cyc_Absyn_Exp* _temp480; struct Cyc_Absyn_Exp* _temp482; struct Cyc_List_List*
_temp484; struct Cyc_List_List* _temp486; struct Cyc_List_List* _temp488; struct
Cyc_List_List* _temp490; struct Cyc_List_List* _temp492; struct Cyc_Absyn_Exp*
_temp494; struct Cyc_Absyn_Exp* _temp496; struct Cyc_Absyn_Vardecl* _temp498;
struct Cyc_Absyn_Stmt* _temp500; void* _temp502; _LL287: if(*(( int*) _temp285)
==  Cyc_Absyn_Cast_e){ _LL384: _temp383=(( struct Cyc_Absyn_Cast_e_struct*)
_temp285)->f2; goto _LL288;} else{ goto _LL289;} _LL289: if(*(( int*) _temp285)
==  Cyc_Absyn_NoInstantiate_e){ _LL386: _temp385=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp285)->f1; goto _LL290;} else{ goto _LL291;} _LL291: if(*(( int*) _temp285)
==  Cyc_Absyn_Instantiate_e){ _LL388: _temp387=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp285)->f1; goto _LL292;} else{ goto _LL293;} _LL293: if(*(( int*) _temp285)
==  Cyc_Absyn_Const_e){ _LL390: _temp389=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp285)->f1; if( _temp389 == ( void*) Cyc_Absyn_Null_c){ goto _LL294;} else{
goto _LL295;}} else{ goto _LL295;} _LL295: if(*(( int*) _temp285) ==  Cyc_Absyn_Const_e){
_LL392: _temp391=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp285)->f1; if((
unsigned int) _temp391 >  1u?*(( int*) _temp391) ==  Cyc_Absyn_Int_c: 0){ _LL396:
_temp395=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp391)->f1; if( _temp395
== ( void*) Cyc_Absyn_Signed){ goto _LL394;} else{ goto _LL297;} _LL394:
_temp393=(( struct Cyc_Absyn_Int_c_struct*) _temp391)->f2; if( _temp393 ==  0){
goto _LL296;} else{ goto _LL297;}} else{ goto _LL297;}} else{ goto _LL297;}
_LL297: if(*(( int*) _temp285) ==  Cyc_Absyn_Const_e){ _LL398: _temp397=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp285)->f1; if(( unsigned int) _temp397 > 
1u?*(( int*) _temp397) ==  Cyc_Absyn_Int_c: 0){ _LL400: _temp399=( void*)((
struct Cyc_Absyn_Int_c_struct*) _temp397)->f1; if( _temp399 == ( void*) Cyc_Absyn_Signed){
goto _LL298;} else{ goto _LL299;}} else{ goto _LL299;}} else{ goto _LL299;}
_LL299: if(*(( int*) _temp285) ==  Cyc_Absyn_Var_e){ _LL402: _temp401=( void*)((
struct Cyc_Absyn_Var_e_struct*) _temp285)->f2; if(( unsigned int) _temp401 >  1u?*((
int*) _temp401) ==  Cyc_Absyn_Funname_b: 0){ goto _LL300;} else{ goto _LL301;}}
else{ goto _LL301;} _LL301: if(*(( int*) _temp285) ==  Cyc_Absyn_Tunion_e){
_LL404: _temp403=(( struct Cyc_Absyn_Tunion_e_struct*) _temp285)->f3; if(
_temp403 ==  0){ goto _LL302;} else{ goto _LL303;}} else{ goto _LL303;} _LL303:
if(*(( int*) _temp285) ==  Cyc_Absyn_Const_e){ goto _LL304;} else{ goto _LL305;}
_LL305: if(*(( int*) _temp285) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL306;} else{
goto _LL307;} _LL307: if(*(( int*) _temp285) ==  Cyc_Absyn_Sizeoftyp_e){ goto
_LL308;} else{ goto _LL309;} _LL309: if(*(( int*) _temp285) ==  Cyc_Absyn_Offsetof_e){
goto _LL310;} else{ goto _LL311;} _LL311: if(*(( int*) _temp285) ==  Cyc_Absyn_Gentyp_e){
goto _LL312;} else{ goto _LL313;} _LL313: if(*(( int*) _temp285) ==  Cyc_Absyn_AnonEnum_e){
goto _LL314;} else{ goto _LL315;} _LL315: if(*(( int*) _temp285) ==  Cyc_Absyn_Enum_e){
goto _LL316;} else{ goto _LL317;} _LL317: if(*(( int*) _temp285) ==  Cyc_Absyn_Var_e){
_LL406: _temp405=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp285)->f2; if((
unsigned int) _temp405 >  1u?*(( int*) _temp405) ==  Cyc_Absyn_Global_b: 0){
goto _LL318;} else{ goto _LL319;}} else{ goto _LL319;} _LL319: if(*(( int*)
_temp285) ==  Cyc_Absyn_Var_e){ _LL408: _temp407=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp285)->f2; if(( unsigned int) _temp407 >  1u?*(( int*) _temp407) ==  Cyc_Absyn_Param_b:
0){ _LL410: _temp409=(( struct Cyc_Absyn_Param_b_struct*) _temp407)->f1; goto
_LL320;} else{ goto _LL321;}} else{ goto _LL321;} _LL321: if(*(( int*) _temp285)
==  Cyc_Absyn_Var_e){ _LL412: _temp411=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp285)->f2; if(( unsigned int) _temp411 >  1u?*(( int*) _temp411) ==  Cyc_Absyn_Local_b:
0){ _LL414: _temp413=(( struct Cyc_Absyn_Local_b_struct*) _temp411)->f1; goto
_LL322;} else{ goto _LL323;}} else{ goto _LL323;} _LL323: if(*(( int*) _temp285)
==  Cyc_Absyn_Var_e){ _LL416: _temp415=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp285)->f2; if(( unsigned int) _temp415 >  1u?*(( int*) _temp415) ==  Cyc_Absyn_Pat_b:
0){ _LL418: _temp417=(( struct Cyc_Absyn_Pat_b_struct*) _temp415)->f1; goto
_LL324;} else{ goto _LL325;}} else{ goto _LL325;} _LL325: if(*(( int*) _temp285)
==  Cyc_Absyn_Primop_e){ _LL420: _temp419=(( struct Cyc_Absyn_Primop_e_struct*)
_temp285)->f2; goto _LL326;} else{ goto _LL327;} _LL327: if(*(( int*) _temp285)
==  Cyc_Absyn_Increment_e){ _LL422: _temp421=(( struct Cyc_Absyn_Increment_e_struct*)
_temp285)->f1; goto _LL328;} else{ goto _LL329;} _LL329: if(*(( int*) _temp285)
==  Cyc_Absyn_AssignOp_e){ _LL429: _temp428=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp285)->f1; goto _LL426; _LL426: _temp425=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp285)->f2; if( _temp425 ==  0){ goto _LL331;} else{ _temp427=* _temp425;
goto _LL424;} _LL424: _temp423=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp285)->f3;
goto _LL330;} else{ goto _LL331;} _LL331: if(*(( int*) _temp285) ==  Cyc_Absyn_AssignOp_e){
_LL435: _temp434=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp285)->f1; goto
_LL433; _LL433: _temp432=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp285)->f2;
if( _temp432 ==  0){ goto _LL431;} else{ goto _LL333;} _LL431: _temp430=((
struct Cyc_Absyn_AssignOp_e_struct*) _temp285)->f3; goto _LL332;} else{ goto
_LL333;} _LL333: if(*(( int*) _temp285) ==  Cyc_Absyn_SeqExp_e){ _LL439:
_temp438=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp285)->f1; goto _LL437;
_LL437: _temp436=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp285)->f2; goto
_LL334;} else{ goto _LL335;} _LL335: if(*(( int*) _temp285) ==  Cyc_Absyn_Throw_e){
_LL441: _temp440=(( struct Cyc_Absyn_Throw_e_struct*) _temp285)->f1; goto _LL336;}
else{ goto _LL337;} _LL337: if(*(( int*) _temp285) ==  Cyc_Absyn_FnCall_e){
_LL445: _temp444=(( struct Cyc_Absyn_FnCall_e_struct*) _temp285)->f1; goto
_LL443; _LL443: _temp442=(( struct Cyc_Absyn_FnCall_e_struct*) _temp285)->f2;
goto _LL338;} else{ goto _LL339;} _LL339: if(*(( int*) _temp285) ==  Cyc_Absyn_Malloc_e){
_LL447: _temp446=(( struct Cyc_Absyn_Malloc_e_struct*) _temp285)->f1; _LL457:
_temp456= _temp446.is_calloc; goto _LL455; _LL455: _temp454= _temp446.rgn; goto
_LL453; _LL453: _temp452= _temp446.elt_type; goto _LL451; _LL451: _temp450=
_temp446.num_elts; goto _LL449; _LL449: _temp448= _temp446.fat_result; goto
_LL340;} else{ goto _LL341;} _LL341: if(*(( int*) _temp285) ==  Cyc_Absyn_New_e){
_LL461: _temp460=(( struct Cyc_Absyn_New_e_struct*) _temp285)->f1; goto _LL459;
_LL459: _temp458=(( struct Cyc_Absyn_New_e_struct*) _temp285)->f2; goto _LL342;}
else{ goto _LL343;} _LL343: if(*(( int*) _temp285) ==  Cyc_Absyn_Address_e){
_LL463: _temp462=(( struct Cyc_Absyn_Address_e_struct*) _temp285)->f1; goto
_LL344;} else{ goto _LL345;} _LL345: if(*(( int*) _temp285) ==  Cyc_Absyn_Deref_e){
_LL465: _temp464=(( struct Cyc_Absyn_Deref_e_struct*) _temp285)->f1; goto _LL346;}
else{ goto _LL347;} _LL347: if(*(( int*) _temp285) ==  Cyc_Absyn_StructMember_e){
_LL469: _temp468=(( struct Cyc_Absyn_StructMember_e_struct*) _temp285)->f1; goto
_LL467; _LL467: _temp466=(( struct Cyc_Absyn_StructMember_e_struct*) _temp285)->f2;
goto _LL348;} else{ goto _LL349;} _LL349: if(*(( int*) _temp285) ==  Cyc_Absyn_StructArrow_e){
_LL473: _temp472=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp285)->f1; goto
_LL471; _LL471: _temp470=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp285)->f2;
goto _LL350;} else{ goto _LL351;} _LL351: if(*(( int*) _temp285) ==  Cyc_Absyn_Conditional_e){
_LL479: _temp478=(( struct Cyc_Absyn_Conditional_e_struct*) _temp285)->f1; goto
_LL477; _LL477: _temp476=(( struct Cyc_Absyn_Conditional_e_struct*) _temp285)->f2;
goto _LL475; _LL475: _temp474=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp285)->f3; goto _LL352;} else{ goto _LL353;} _LL353: if(*(( int*) _temp285)
==  Cyc_Absyn_Subscript_e){ _LL483: _temp482=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp285)->f1; goto _LL481; _LL481: _temp480=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp285)->f2; goto _LL354;} else{ goto _LL355;} _LL355: if(*(( int*) _temp285)
==  Cyc_Absyn_Tunion_e){ _LL485: _temp484=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp285)->f3; goto _LL356;} else{ goto _LL357;} _LL357: if(*(( int*) _temp285)
==  Cyc_Absyn_Tuple_e){ _LL487: _temp486=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp285)->f1; goto _LL358;} else{ goto _LL359;} _LL359: if(*(( int*) _temp285)
==  Cyc_Absyn_AnonStruct_e){ _LL489: _temp488=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp285)->f2; goto _LL360;} else{ goto _LL361;} _LL361: if(*(( int*) _temp285)
==  Cyc_Absyn_Struct_e){ _LL491: _temp490=(( struct Cyc_Absyn_Struct_e_struct*)
_temp285)->f3; goto _LL362;} else{ goto _LL363;} _LL363: if(*(( int*) _temp285)
==  Cyc_Absyn_Array_e){ _LL493: _temp492=(( struct Cyc_Absyn_Array_e_struct*)
_temp285)->f1; goto _LL364;} else{ goto _LL365;} _LL365: if(*(( int*) _temp285)
==  Cyc_Absyn_Comprehension_e){ _LL499: _temp498=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp285)->f1; goto _LL497; _LL497: _temp496=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp285)->f2; goto _LL495; _LL495: _temp494=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp285)->f3; goto _LL366;} else{ goto _LL367;} _LL367: if(*(( int*) _temp285)
==  Cyc_Absyn_StmtExp_e){ _LL501: _temp500=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp285)->f1; goto _LL368;} else{ goto _LL369;} _LL369: if(*(( int*) _temp285)
==  Cyc_Absyn_Var_e){ _LL503: _temp502=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp285)->f2; if( _temp502 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL370;}
else{ goto _LL371;}} else{ goto _LL371;} _LL371: if(*(( int*) _temp285) ==  Cyc_Absyn_UnknownId_e){
goto _LL372;} else{ goto _LL373;} _LL373: if(*(( int*) _temp285) ==  Cyc_Absyn_UnknownCall_e){
goto _LL374;} else{ goto _LL375;} _LL375: if(*(( int*) _temp285) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL376;} else{ goto _LL377;} _LL377: if(*(( int*) _temp285) ==  Cyc_Absyn_CompoundLit_e){
goto _LL378;} else{ goto _LL379;} _LL379: if(*(( int*) _temp285) ==  Cyc_Absyn_Codegen_e){
goto _LL380;} else{ goto _LL381;} _LL381: if(*(( int*) _temp285) ==  Cyc_Absyn_Fill_e){
goto _LL382;} else{ goto _LL286;} _LL288: _temp385= _temp383; goto _LL290;
_LL290: _temp387= _temp385; goto _LL292; _LL292: return Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp387); _LL294: goto _LL296; _LL296: return({ struct _tuple0
_temp504; _temp504.f1= inflow; _temp504.f2=( void*) Cyc_CfFlowInfo_Zero;
_temp504;}); _LL298: goto _LL300; _LL300: return({ struct _tuple0 _temp505;
_temp505.f1= inflow; _temp505.f2=( void*) Cyc_CfFlowInfo_NotZeroAll; _temp505;});
_LL302: goto _LL304; _LL304: goto _LL306; _LL306: goto _LL308; _LL308: goto
_LL310; _LL310: goto _LL312; _LL312: goto _LL314; _LL314: goto _LL316; _LL316:
return({ struct _tuple0 _temp506; _temp506.f1= inflow; _temp506.f2= Cyc_CfFlowInfo_unknown_all;
_temp506;}); _LL318: return({ struct _tuple0 _temp507; _temp507.f1= inflow;
_temp507.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp507;}); _LL320:
_temp413= _temp409; goto _LL322; _LL322: _temp417= _temp413; goto _LL324; _LL324:
return({ struct _tuple0 _temp508; _temp508.f1= inflow; _temp508.f2= Cyc_Dict_lookup(
d,( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp509=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp509[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp510; _temp510.tag= Cyc_CfFlowInfo_VarRoot;
_temp510.f1= _temp417; _temp510;}); _temp509;})); _temp508;}); _LL326: return
Cyc_NewControlFlow_anal_use_ints( env, inflow, _temp419); _LL328: { void*
_temp514; struct _tuple0 _temp512= Cyc_NewControlFlow_anal_use_ints( env, inflow,({
struct Cyc_Absyn_Exp* _temp511[ 1u]; _temp511[ 0u]= _temp421;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp511, sizeof( struct Cyc_Absyn_Exp*),
1u));})); _LL515: _temp514= _temp512.f1; goto _LL513; _LL513: { void* _temp518;
struct _tuple0 _temp516= Cyc_NewControlFlow_anal_Lexp( env, _temp514, _temp421);
_LL519: _temp518= _temp516.f2; goto _LL517; _LL517:{ struct _tuple0 _temp521=({
struct _tuple0 _temp520; _temp520.f1= _temp518; _temp520.f2= _temp514; _temp520;});
void* _temp527; struct Cyc_List_List* _temp529; struct Cyc_Dict_Dict* _temp531;
void* _temp533; struct Cyc_CfFlowInfo_Place* _temp535; _LL523: _LL534: _temp533=
_temp521.f1; if(( unsigned int) _temp533 >  1u?*(( int*) _temp533) ==  Cyc_CfFlowInfo_PlaceL:
0){ _LL536: _temp535=(( struct Cyc_CfFlowInfo_PlaceL_struct*) _temp533)->f1;
goto _LL528;} else{ goto _LL525;} _LL528: _temp527= _temp521.f2; if((
unsigned int) _temp527 >  1u?*(( int*) _temp527) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL532: _temp531=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp527)->f1;
goto _LL530; _LL530: _temp529=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp527)->f2; goto _LL524;} else{ goto _LL525;} _LL525: goto _LL526; _LL524:
_temp529= Cyc_CfFlowInfo_reln_kill_exp( _temp529, _temp421); _temp514=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp537=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp537[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp538; _temp538.tag= Cyc_CfFlowInfo_ReachableFL;
_temp538.f1= Cyc_CfFlowInfo_assign_place( _temp421->loc, _temp531, env->all_changed,
_temp535, Cyc_CfFlowInfo_unknown_all); _temp538.f2= _temp529; _temp538;});
_temp537;}); goto _LL522; _LL526: goto _LL522; _LL522:;} return({ struct _tuple0
_temp539; _temp539.f1= _temp514; _temp539.f2= Cyc_CfFlowInfo_unknown_all;
_temp539;});}} _LL330: { void* _temp543; struct _tuple0 _temp541= Cyc_NewControlFlow_anal_use_ints(
env, inflow,({ struct Cyc_Absyn_Exp* _temp540[ 2u]; _temp540[ 1u]= _temp423;
_temp540[ 0u]= _temp428;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp540, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL544: _temp543=
_temp541.f1; goto _LL542; _LL542: { void* _temp547; struct _tuple0 _temp545= Cyc_NewControlFlow_anal_Lexp(
env, _temp543, e); _LL548: _temp547= _temp545.f2; goto _LL546; _LL546:{ struct
_tuple0 _temp550=({ struct _tuple0 _temp549; _temp549.f1= _temp547; _temp549.f2=
_temp543; _temp549;}); void* _temp556; struct Cyc_List_List* _temp558; struct
Cyc_Dict_Dict* _temp560; void* _temp562; struct Cyc_CfFlowInfo_Place* _temp564;
_LL552: _LL563: _temp562= _temp550.f1; if(( unsigned int) _temp562 >  1u?*(( int*)
_temp562) ==  Cyc_CfFlowInfo_PlaceL: 0){ _LL565: _temp564=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp562)->f1; goto _LL557;} else{ goto _LL554;} _LL557: _temp556= _temp550.f2;
if(( unsigned int) _temp556 >  1u?*(( int*) _temp556) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL561: _temp560=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp556)->f1;
goto _LL559; _LL559: _temp558=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp556)->f2; goto _LL553;} else{ goto _LL554;} _LL554: goto _LL555; _LL553:
_temp558= Cyc_CfFlowInfo_reln_kill_exp( _temp558, _temp428); _temp543=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp566=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp566[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp567; _temp567.tag= Cyc_CfFlowInfo_ReachableFL;
_temp567.f1= Cyc_CfFlowInfo_assign_place( _temp428->loc, _temp560, env->all_changed,
_temp564, Cyc_CfFlowInfo_unknown_all); _temp567.f2= _temp558; _temp567;});
_temp566;}); goto _LL551; _LL555: goto _LL551; _LL551:;} return({ struct _tuple0
_temp568; _temp568.f1= _temp543; _temp568.f2= Cyc_CfFlowInfo_unknown_all;
_temp568;});}} _LL332: { struct Cyc_Set_Set** _temp569= env->all_changed; while(
1) { env->all_changed=({ struct Cyc_Set_Set** _temp570=( struct Cyc_Set_Set**)
_cycalloc( sizeof( struct Cyc_Set_Set*)); _temp570[ 0]= Cyc_CfFlowInfo_mt_place_set();
_temp570;});{ void* _temp573; void* _temp575; struct _tuple0 _temp571= Cyc_NewControlFlow_anal_Lexp(
env, inflow, _temp434); _LL576: _temp575= _temp571.f1; goto _LL574; _LL574:
_temp573= _temp571.f2; goto _LL572; _LL572: { struct Cyc_Set_Set* _temp577=*((
struct Cyc_Set_Set**) _check_null( env->all_changed)); env->all_changed=({
struct Cyc_Set_Set** _temp578=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp578[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp578;});{ void*
_temp581; void* _temp583; struct _tuple0 _temp579= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp430); _LL584: _temp583= _temp579.f1; goto _LL582; _LL582:
_temp581= _temp579.f2; goto _LL580; _LL580: { struct Cyc_Set_Set* _temp585=*((
struct Cyc_Set_Set**) _check_null( env->all_changed)); void* _temp586= Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& _temp577, _temp575, _temp583, _temp577, _temp585); void*
_temp587= Cyc_CfFlowInfo_join_flow( _temp569, inflow, _temp586); if( Cyc_CfFlowInfo_flow_lessthan_approx(
_temp587, inflow)){ if( _temp569 ==  0){ env->all_changed= 0;} else{*(( struct
Cyc_Set_Set**) _check_null( env->all_changed))=(( struct Cyc_Set_Set*(*)( struct
Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(* _temp569,((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(
_temp577, _temp585));}{ void* _temp588= _temp586; struct Cyc_List_List* _temp594;
struct Cyc_Dict_Dict* _temp596; _LL590: if( _temp588 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL591;} else{ goto _LL592;} _LL592: if(( unsigned int) _temp588 >  1u?*((
int*) _temp588) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL597: _temp596=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp588)->f1; goto _LL595; _LL595: _temp594=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp588)->f2; goto _LL593;} else{
goto _LL589;} _LL591: return({ struct _tuple0 _temp598; _temp598.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp598.f2= _temp581; _temp598;}); _LL593: { void* _temp599= _temp573; struct
Cyc_CfFlowInfo_Place* _temp605; _LL601: if(( unsigned int) _temp599 >  1u?*((
int*) _temp599) ==  Cyc_CfFlowInfo_PlaceL: 0){ _LL606: _temp605=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp599)->f1; goto _LL602;} else{ goto _LL603;} _LL603: if( _temp599 == ( void*)
Cyc_CfFlowInfo_UnknownL){ goto _LL604;} else{ goto _LL600;} _LL602: _temp596=
Cyc_CfFlowInfo_assign_place( e->loc, _temp596, env->all_changed, _temp605,
_temp581); _temp594= Cyc_CfFlowInfo_reln_assign_exp( _temp594, _temp434,
_temp430); _temp586=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp607=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp607[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp608; _temp608.tag= Cyc_CfFlowInfo_ReachableFL; _temp608.f1= _temp596;
_temp608.f2= _temp594; _temp608;}); _temp607;}); Cyc_NewControlFlow_update_tryflow(
env, _temp586); return({ struct _tuple0 _temp609; _temp609.f1= _temp586;
_temp609.f2= _temp581; _temp609;}); _LL604: return({ struct _tuple0 _temp610;
_temp610.f1= Cyc_NewControlFlow_use_Rval( env, _temp430->loc, _temp586, _temp581);
_temp610.f2= _temp581; _temp610;}); _LL600:;} _LL589:;}} inflow= _temp587;}}}}}}
_LL334: return Cyc_NewControlFlow_anal_Rexp( env,( Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp438)).f1, _temp436); _LL336: { void* _temp613; void* _temp615;
struct _tuple0 _temp611= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp440);
_LL616: _temp615= _temp611.f1; goto _LL614; _LL614: _temp613= _temp611.f2; goto
_LL612; _LL612: Cyc_NewControlFlow_use_Rval( env, _temp440->loc, _temp615,
_temp613); return({ struct _tuple0 _temp617; _temp617.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp617.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp617;});} _LL338:
_temp442=({ struct Cyc_List_List* _temp618=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp618->hd=( void*) _temp444; _temp618->tl=
_temp442; _temp618;});{ struct Cyc_List_List* _temp621; void* _temp623; struct
_tuple4 _temp619= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp442);
_LL624: _temp623= _temp619.f1; goto _LL622; _LL622: _temp621= _temp619.f2; goto
_LL620; _LL620: for( 0; _temp621 !=  0;( _temp621= _temp621->tl, _temp442=((
struct Cyc_List_List*) _check_null( _temp442))->tl)){ _temp623= Cyc_NewControlFlow_use_Rval(
env,(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp442))->hd)->loc,
_temp623,( void*) _temp621->hd);} return({ struct _tuple0 _temp625; _temp625.f1=
_temp623; _temp625.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp625;});} _LL340: {
void* root=( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp654=( struct
Cyc_CfFlowInfo_MallocPt_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp654[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp655; _temp655.tag=
Cyc_CfFlowInfo_MallocPt; _temp655.f1= _temp450; _temp655;}); _temp654;}); struct
Cyc_CfFlowInfo_Place* place=({ struct Cyc_CfFlowInfo_Place* _temp653=( struct
Cyc_CfFlowInfo_Place*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Place));
_temp653->root=( void*) root; _temp653->fields= 0; _temp653;}); void* rval=(
void*)({ struct Cyc_CfFlowInfo_AddressOf_struct* _temp651=( struct Cyc_CfFlowInfo_AddressOf_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_AddressOf_struct)); _temp651[ 0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _temp652; _temp652.tag= Cyc_CfFlowInfo_AddressOf;
_temp652.f1= place; _temp652;}); _temp651;}); void* place_val= _temp448?( void*)
Cyc_CfFlowInfo_NotZeroAll: Cyc_CfFlowInfo_typ_to_absrval(*(( void**) _check_null(
_temp452)), Cyc_CfFlowInfo_unknown_none); void* outflow; if( env->all_changed != 
0){*(( struct Cyc_Set_Set**) _check_null( env->all_changed))=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(*((
struct Cyc_Set_Set**) _check_null( env->all_changed)), place);} if( _temp454 != 
0){ struct Cyc_List_List* _temp629; void* _temp631; struct _tuple4 _temp627= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp626[ 2u]; _temp626[ 1u]= _temp450;
_temp626[ 0u]=( struct Cyc_Absyn_Exp*) _check_null( _temp454);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp626, sizeof( struct Cyc_Absyn_Exp*),
2u));})); _LL632: _temp631= _temp627.f1; goto _LL630; _LL630: _temp629= _temp627.f2;
goto _LL628; _LL628: outflow= _temp631;} else{ void* _temp635; struct _tuple0
_temp633= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp450); _LL636: _temp635=
_temp633.f1; goto _LL634; _LL634: outflow= _temp635;}{ void* _temp637= outflow;
struct Cyc_List_List* _temp643; struct Cyc_Dict_Dict* _temp645; _LL639: if(
_temp637 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL640;} else{ goto _LL641;}
_LL641: if(( unsigned int) _temp637 >  1u?*(( int*) _temp637) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL646: _temp645=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp637)->f1;
goto _LL644; _LL644: _temp643=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp637)->f2; goto _LL642;} else{ goto _LL638;} _LL640: return({ struct _tuple0
_temp647; _temp647.f1= outflow; _temp647.f2= rval; _temp647;}); _LL642: return({
struct _tuple0 _temp648; _temp648.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp649=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp649[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp650; _temp650.tag= Cyc_CfFlowInfo_ReachableFL; _temp650.f1= Cyc_Dict_insert(
_temp645, root, place_val); _temp650.f2= _temp643; _temp650;}); _temp649;});
_temp648.f2= rval; _temp648;}); _LL638:;}} _LL342: { void* root=( void*)({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp686=( struct Cyc_CfFlowInfo_MallocPt_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp686[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp687; _temp687.tag= Cyc_CfFlowInfo_MallocPt;
_temp687.f1= _temp458; _temp687;}); _temp686;}); struct Cyc_CfFlowInfo_Place*
place=({ struct Cyc_CfFlowInfo_Place* _temp685=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp685->root=( void*) root;
_temp685->fields= 0; _temp685;}); void* rval=( void*)({ struct Cyc_CfFlowInfo_AddressOf_struct*
_temp683=( struct Cyc_CfFlowInfo_AddressOf_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_AddressOf_struct)); _temp683[ 0]=({ struct Cyc_CfFlowInfo_AddressOf_struct
_temp684; _temp684.tag= Cyc_CfFlowInfo_AddressOf; _temp684.f1= place; _temp684;});
_temp683;}); if( env->all_changed !=  0){*(( struct Cyc_Set_Set**) _check_null(
env->all_changed))=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place*
elt)) Cyc_Set_insert)(*(( struct Cyc_Set_Set**) _check_null( env->all_changed)),
place);}{ void* outflow; void* place_val; if( _temp460 !=  0){ struct Cyc_List_List*
_temp659; void* _temp661; struct _tuple4 _temp657= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp656[ 2u]; _temp656[ 1u]= _temp458;
_temp656[ 0u]=( struct Cyc_Absyn_Exp*) _check_null( _temp460);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp656, sizeof( struct Cyc_Absyn_Exp*),
2u));})); _LL662: _temp661= _temp657.f1; goto _LL660; _LL660: _temp659= _temp657.f2;
goto _LL658; _LL658: outflow= _temp661; place_val=( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp659))->tl))->hd;} else{
void* _temp665; void* _temp667; struct _tuple0 _temp663= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp458); _LL668: _temp667= _temp663.f1; goto _LL666; _LL666:
_temp665= _temp663.f2; goto _LL664; _LL664: outflow= _temp667; place_val=
_temp665;}{ void* _temp669= outflow; struct Cyc_List_List* _temp675; struct Cyc_Dict_Dict*
_temp677; _LL671: if( _temp669 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL672;}
else{ goto _LL673;} _LL673: if(( unsigned int) _temp669 >  1u?*(( int*) _temp669)
==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL678: _temp677=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp669)->f1; goto _LL676; _LL676: _temp675=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp669)->f2; goto _LL674;} else{ goto _LL670;} _LL672: return({ struct _tuple0
_temp679; _temp679.f1= outflow; _temp679.f2= rval; _temp679;}); _LL674: return({
struct _tuple0 _temp680; _temp680.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp681=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp681[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp682; _temp682.tag= Cyc_CfFlowInfo_ReachableFL; _temp682.f1= Cyc_Dict_insert(
_temp677, root, place_val); _temp682.f2= _temp675; _temp682;}); _temp681;});
_temp680.f2= rval; _temp680;}); _LL670:;}}} _LL344: { void* _temp690; void*
_temp692; struct _tuple0 _temp688= Cyc_NewControlFlow_anal_Lexp( env, inflow,
_temp462); _LL693: _temp692= _temp688.f1; goto _LL691; _LL691: _temp690=
_temp688.f2; goto _LL689; _LL689: { void* _temp694= _temp690; struct Cyc_CfFlowInfo_Place*
_temp700; _LL696: if( _temp694 == ( void*) Cyc_CfFlowInfo_UnknownL){ goto _LL697;}
else{ goto _LL698;} _LL698: if(( unsigned int) _temp694 >  1u?*(( int*) _temp694)
==  Cyc_CfFlowInfo_PlaceL: 0){ _LL701: _temp700=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp694)->f1; goto _LL699;} else{ goto _LL695;} _LL697: return({ struct _tuple0
_temp702; _temp702.f1= _temp692; _temp702.f2=( void*) Cyc_CfFlowInfo_NotZeroAll;
_temp702;}); _LL699: return({ struct _tuple0 _temp703; _temp703.f1= _temp692;
_temp703.f2=( void*)({ struct Cyc_CfFlowInfo_AddressOf_struct* _temp704=( struct
Cyc_CfFlowInfo_AddressOf_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_AddressOf_struct));
_temp704[ 0]=({ struct Cyc_CfFlowInfo_AddressOf_struct _temp705; _temp705.tag=
Cyc_CfFlowInfo_AddressOf; _temp705.f1= _temp700; _temp705;}); _temp704;});
_temp703;}); _LL695:;}} _LL346: { void* _temp708; void* _temp710; struct _tuple0
_temp706= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp464); _LL711: _temp710=
_temp706.f1; goto _LL709; _LL709: _temp708= _temp706.f2; goto _LL707; _LL707:
return Cyc_NewControlFlow_anal_derefR( env, inflow, _temp710, _temp464, _temp708);}
_LL348: { void* _temp714; void* _temp716; struct _tuple0 _temp712= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp468); _LL717: _temp716= _temp712.f1; goto _LL715; _LL715:
_temp714= _temp712.f2; goto _LL713; _LL713:{ void* _temp718= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp468->topt))->v); _LL720: if((
unsigned int) _temp718 >  3u?*(( int*) _temp718) ==  Cyc_Absyn_AnonUnionType: 0){
goto _LL721;} else{ goto _LL722;} _LL722: if(( unsigned int) _temp718 >  3u?*((
int*) _temp718) ==  Cyc_Absyn_UnionType: 0){ goto _LL723;} else{ goto _LL724;}
_LL724: goto _LL725; _LL721: goto _LL723; _LL723: return({ struct _tuple0
_temp726; _temp726.f1= _temp716; _temp726.f2= Cyc_CfFlowInfo_typ_to_absrval((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all);
_temp726;}); _LL725: goto _LL719; _LL719:;}{ void* _temp727= _temp714; struct
Cyc_Dict_Dict* _temp733; _LL729: if(( unsigned int) _temp727 >  3u?*(( int*)
_temp727) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL734: _temp733=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp727)->f1; goto _LL730;} else{ goto _LL731;} _LL731: goto _LL732; _LL730:
return({ struct _tuple0 _temp735; _temp735.f1= _temp716; _temp735.f2=(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp733,
_temp466); _temp735;}); _LL732:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp736=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp736[ 0]=({ struct Cyc_Core_Impossible_struct _temp737; _temp737.tag= Cyc_Core_Impossible;
_temp737.f1= _tag_arr("anal_Rexp: StructMember", sizeof( unsigned char), 24u);
_temp737;}); _temp736;})); _LL728:;}} _LL350: { void* _temp740; void* _temp742;
struct _tuple0 _temp738= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp472);
_LL743: _temp742= _temp738.f1; goto _LL741; _LL741: _temp740= _temp738.f2; goto
_LL739; _LL739: { void* _temp746; void* _temp748; struct _tuple0 _temp744= Cyc_NewControlFlow_anal_derefR(
env, inflow, _temp742, _temp472, _temp740); _LL749: _temp748= _temp744.f1; goto
_LL747; _LL747: _temp746= _temp744.f2; goto _LL745; _LL745:{ void* _temp750= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp472->topt))->v); struct Cyc_Absyn_PtrInfo
_temp756; void* _temp758; _LL752: if(( unsigned int) _temp750 >  3u?*(( int*)
_temp750) ==  Cyc_Absyn_PointerType: 0){ _LL757: _temp756=(( struct Cyc_Absyn_PointerType_struct*)
_temp750)->f1; _LL759: _temp758=( void*) _temp756.elt_typ; goto _LL753;} else{
goto _LL754;} _LL754: goto _LL755; _LL753:{ void* _temp760= Cyc_Tcutil_compress(
_temp758); _LL762: if(( unsigned int) _temp760 >  3u?*(( int*) _temp760) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL763;} else{ goto _LL764;} _LL764: if(( unsigned int) _temp760 >  3u?*((
int*) _temp760) ==  Cyc_Absyn_UnionType: 0){ goto _LL765;} else{ goto _LL766;}
_LL766: goto _LL767; _LL763: goto _LL765; _LL765: return({ struct _tuple0
_temp768; _temp768.f1= _temp748; _temp768.f2= Cyc_CfFlowInfo_typ_to_absrval((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all);
_temp768;}); _LL767: goto _LL761; _LL761:;} goto _LL751; _LL755:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp769=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp769[ 0]=({ struct
Cyc_Core_Impossible_struct _temp770; _temp770.tag= Cyc_Core_Impossible; _temp770.f1=
_tag_arr("anal_Rexp: StructArrow ptr", sizeof( unsigned char), 27u); _temp770;});
_temp769;})); _LL751:;}{ void* _temp771= _temp746; struct Cyc_Dict_Dict*
_temp777; _LL773: if(( unsigned int) _temp771 >  3u?*(( int*) _temp771) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL778: _temp777=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp771)->f1;
goto _LL774;} else{ goto _LL775;} _LL775: goto _LL776; _LL774: return({ struct
_tuple0 _temp779; _temp779.f1= _temp748; _temp779.f2=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp777, _temp470); _temp779;});
_LL776:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp780=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp780[ 0]=({ struct Cyc_Core_Impossible_struct _temp781; _temp781.tag= Cyc_Core_Impossible;
_temp781.f1= _tag_arr("anal_Rexp: StructArrow", sizeof( unsigned char), 23u);
_temp781;}); _temp780;})); _LL772:;}}} _LL352: { void* _temp784; void* _temp786;
struct _tuple0 _temp782= Cyc_NewControlFlow_anal_test( env, inflow, _temp478);
_LL787: _temp786= _temp782.f1; goto _LL785; _LL785: _temp784= _temp782.f2; goto
_LL783; _LL783: { void* _temp790; void* _temp792; struct _tuple0 _temp788= Cyc_NewControlFlow_anal_Rexp(
env, _temp786, _temp476); _LL793: _temp792= _temp788.f1; goto _LL791; _LL791:
_temp790= _temp788.f2; goto _LL789; _LL789: { void* _temp796; void* _temp798;
struct _tuple0 _temp794= Cyc_NewControlFlow_anal_Rexp( env, _temp784, _temp474);
_LL799: _temp798= _temp794.f1; goto _LL797; _LL797: _temp796= _temp794.f2; goto
_LL795; _LL795: return({ struct _tuple0 _temp800; _temp800.f1= Cyc_CfFlowInfo_join_flow(
env->all_changed, Cyc_NewControlFlow_use_Rval( env, _temp476->loc, _temp792,
_temp790), Cyc_NewControlFlow_use_Rval( env, _temp474->loc, _temp798, _temp796));
_temp800.f2= Cyc_CfFlowInfo_unknown_all; _temp800;});}}} _LL354: { struct Cyc_List_List*
_temp804; void* _temp806; struct _tuple4 _temp802= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp801[ 2u]; _temp801[ 1u]= _temp480;
_temp801[ 0u]= _temp482;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp801, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL807: _temp806=
_temp802.f1; goto _LL805; _LL805: _temp804= _temp802.f2; goto _LL803; _LL803:{
void* _temp808= _temp806; struct Cyc_List_List* _temp814; struct Cyc_Dict_Dict*
_temp816; _LL810: if(( unsigned int) _temp808 >  1u?*(( int*) _temp808) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL817: _temp816=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp808)->f1;
goto _LL815; _LL815: _temp814=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp808)->f2; goto _LL811;} else{ goto _LL812;} _LL812: goto _LL813; _LL811:
if( Cyc_CfFlowInfo_initlevel( _temp816,( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp804))->tl))->hd) == (
void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp818[ 0u]={}; Cyc_Tcutil_terr(
_temp480->loc, _tag_arr("expression may not be initialized", sizeof(
unsigned char), 34u), _tag_arr( _temp818, sizeof( void*), 0u));});}{ struct Cyc_List_List*
_temp819= Cyc_NewControlFlow_add_subscript_reln( _temp814, _temp482, _temp480);
if( _temp814 !=  _temp819){ _temp806=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp820=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp820[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp821; _temp821.tag= Cyc_CfFlowInfo_ReachableFL; _temp821.f1= _temp816;
_temp821.f2= _temp819; _temp821;}); _temp820;});} goto _LL809;} _LL813: goto
_LL809; _LL809:;}{ void* _temp822= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp482->topt))->v); _LL824: if(( unsigned int) _temp822 >  3u?*((
int*) _temp822) ==  Cyc_Absyn_TupleType: 0){ goto _LL825;} else{ goto _LL826;}
_LL826: goto _LL827; _LL825: { void* _temp828=( void*)(( struct Cyc_List_List*)
_check_null( _temp804))->hd; struct Cyc_Dict_Dict* _temp834; _LL830: if((
unsigned int) _temp828 >  3u?*(( int*) _temp828) ==  Cyc_CfFlowInfo_Aggregate: 0){
_LL835: _temp834=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp828)->f1; goto
_LL831;} else{ goto _LL832;} _LL832: goto _LL833; _LL831: return({ struct
_tuple0 _temp836; _temp836.f1= _temp806; _temp836.f2=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp834, Cyc_Absyn_fieldname(( int)
Cyc_Evexp_eval_const_uint_exp( _temp480))); _temp836;}); _LL833:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp837=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp837[ 0]=({ struct
Cyc_Core_Impossible_struct _temp838; _temp838.tag= Cyc_Core_Impossible; _temp838.f1=
_tag_arr("anal_Rexp: Subscript", sizeof( unsigned char), 21u); _temp838;});
_temp837;})); _LL829:;} _LL827: goto _LL823; _LL823:;} return Cyc_NewControlFlow_anal_derefR(
env, inflow, _temp806, _temp482,( void*)(( struct Cyc_List_List*) _check_null(
_temp804))->hd);} _LL356: _temp486= _temp484; goto _LL358; _LL358: { struct Cyc_List_List*
_temp841; void* _temp843; struct _tuple4 _temp839= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, _temp486); _LL844: _temp843= _temp839.f1; goto _LL842; _LL842:
_temp841= _temp839.f2; goto _LL840; _LL840: { struct Cyc_Dict_Dict* aggrdict=((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{ int i= 0; for( 0; _temp841 !=  0;(
_temp841= _temp841->tl, ++ i)){ aggrdict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( aggrdict, Cyc_Absyn_fieldname(
i),( void*) _temp841->hd);}} return({ struct _tuple0 _temp845; _temp845.f1=
_temp843; _temp845.f2=( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp846=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp846[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp847; _temp847.tag= Cyc_CfFlowInfo_Aggregate; _temp847.f1= aggrdict;
_temp847;}); _temp846;}); _temp845;});}} _LL360: _temp490= _temp488; goto _LL362;
_LL362: { struct Cyc_List_List* _temp850; void* _temp852; struct _tuple4
_temp848= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple5*)) Cyc_Core_snd, _temp490)); _LL853:
_temp852= _temp848.f1; goto _LL851; _LL851: _temp850= _temp848.f2; goto _LL849;
_LL849: { struct Cyc_Dict_Dict* aggrdict=(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{
int i= 0; for( 0; _temp850 !=  0;((( _temp850= _temp850->tl, _temp490= _temp490->tl)),
++ i)){ struct Cyc_List_List* ds=(*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp490))->hd)).f1; for( 0; ds !=  0; ds= ds->tl){ void* _temp854=(
void*) ds->hd; struct _tagged_arr* _temp860; _LL856: if(*(( int*) _temp854) == 
Cyc_Absyn_ArrayElement){ goto _LL857;} else{ goto _LL858;} _LL858: if(*(( int*)
_temp854) ==  Cyc_Absyn_FieldName){ _LL861: _temp860=(( struct Cyc_Absyn_FieldName_struct*)
_temp854)->f1; goto _LL859;} else{ goto _LL855;} _LL857:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp862=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp862[ 0]=({ struct
Cyc_Core_Impossible_struct _temp863; _temp863.tag= Cyc_Core_Impossible; _temp863.f1=
_tag_arr("anal_Rexp:Struct_e", sizeof( unsigned char), 19u); _temp863;});
_temp862;})); _LL859: aggrdict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( aggrdict, _temp860,( void*)
_temp850->hd); _LL855:;}}} return({ struct _tuple0 _temp864; _temp864.f1=
_temp852; _temp864.f2=( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp865=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp865[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp866; _temp866.tag= Cyc_CfFlowInfo_Aggregate; _temp866.f1= aggrdict;
_temp866;}); _temp865;}); _temp864;});}} _LL364: { struct Cyc_List_List*
_temp867=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple5*),
struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct
_tuple5*)) Cyc_Core_snd, _temp492); struct Cyc_List_List* _temp870; void*
_temp872; struct _tuple4 _temp868= Cyc_NewControlFlow_anal_unordered_Rexps( env,
inflow, _temp867); _LL873: _temp872= _temp868.f1; goto _LL871; _LL871: _temp870=
_temp868.f2; goto _LL869; _LL869: for( 0; _temp870 !=  0;( _temp870= _temp870->tl,
_temp867= _temp867->tl)){ _temp872= Cyc_NewControlFlow_use_Rval( env,(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp867))->hd)->loc,
_temp872,( void*) _temp870->hd);} return({ struct _tuple0 _temp874; _temp874.f1=
_temp872; _temp874.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp874;});} _LL366: {
void* _temp877; void* _temp879; struct _tuple0 _temp875= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp496); _LL880: _temp879= _temp875.f1; goto _LL878; _LL878:
_temp877= _temp875.f2; goto _LL876; _LL876: { void* _temp881= _temp879; struct
Cyc_List_List* _temp887; struct Cyc_Dict_Dict* _temp889; _LL883: if( _temp881 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL884;} else{ goto _LL885;} _LL885: if((
unsigned int) _temp881 >  1u?*(( int*) _temp881) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL890: _temp889=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp881)->f1;
goto _LL888; _LL888: _temp887=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp881)->f2; goto _LL886;} else{ goto _LL882;} _LL884: return({ struct _tuple0
_temp891; _temp891.f1= _temp879; _temp891.f2= Cyc_CfFlowInfo_unknown_all;
_temp891;}); _LL886: if( Cyc_CfFlowInfo_initlevel( _temp889, _temp877) == ( void*)
Cyc_CfFlowInfo_NoneIL){({ void* _temp892[ 0u]={}; Cyc_Tcutil_terr( _temp496->loc,
_tag_arr("expression may not be initialized", sizeof( unsigned char), 34u),
_tag_arr( _temp892, sizeof( void*), 0u));});}{ struct Cyc_List_List* new_relns=
_temp887; comp_loop: { void* _temp893=( void*) _temp496->r; struct Cyc_Absyn_Exp*
_temp911; void* _temp914; struct Cyc_Absyn_Vardecl* _temp916; void* _temp919;
struct Cyc_Absyn_Vardecl* _temp921; void* _temp924; struct Cyc_Absyn_Vardecl*
_temp926; void* _temp929; struct Cyc_Absyn_Vardecl* _temp931; void* _temp933;
int _temp935; struct Cyc_List_List* _temp937; struct Cyc_List_List _temp939;
struct Cyc_Absyn_Exp* _temp940; void* _temp942; _LL895: if(*(( int*) _temp893)
==  Cyc_Absyn_Cast_e){ _LL912: _temp911=(( struct Cyc_Absyn_Cast_e_struct*)
_temp893)->f2; goto _LL896;} else{ goto _LL897;} _LL897: if(*(( int*) _temp893)
==  Cyc_Absyn_Var_e){ _LL915: _temp914=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp893)->f2; if(( unsigned int) _temp914 >  1u?*(( int*) _temp914) ==  Cyc_Absyn_Global_b:
0){ _LL917: _temp916=(( struct Cyc_Absyn_Global_b_struct*) _temp914)->f1; goto
_LL913;} else{ goto _LL899;}} else{ goto _LL899;} _LL913: if( ! _temp916->escapes){
goto _LL898;} else{ goto _LL899;} _LL899: if(*(( int*) _temp893) ==  Cyc_Absyn_Var_e){
_LL920: _temp919=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp893)->f2; if((
unsigned int) _temp919 >  1u?*(( int*) _temp919) ==  Cyc_Absyn_Local_b: 0){
_LL922: _temp921=(( struct Cyc_Absyn_Local_b_struct*) _temp919)->f1; goto _LL918;}
else{ goto _LL901;}} else{ goto _LL901;} _LL918: if( ! _temp921->escapes){ goto
_LL900;} else{ goto _LL901;} _LL901: if(*(( int*) _temp893) ==  Cyc_Absyn_Var_e){
_LL925: _temp924=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp893)->f2; if((
unsigned int) _temp924 >  1u?*(( int*) _temp924) ==  Cyc_Absyn_Pat_b: 0){ _LL927:
_temp926=(( struct Cyc_Absyn_Pat_b_struct*) _temp924)->f1; goto _LL923;} else{
goto _LL903;}} else{ goto _LL903;} _LL923: if( ! _temp926->escapes){ goto _LL902;}
else{ goto _LL903;} _LL903: if(*(( int*) _temp893) ==  Cyc_Absyn_Var_e){ _LL930:
_temp929=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp893)->f2; if((
unsigned int) _temp929 >  1u?*(( int*) _temp929) ==  Cyc_Absyn_Param_b: 0){
_LL932: _temp931=(( struct Cyc_Absyn_Param_b_struct*) _temp929)->f1; goto _LL928;}
else{ goto _LL905;}} else{ goto _LL905;} _LL928: if( ! _temp931->escapes){ goto
_LL904;} else{ goto _LL905;} _LL905: if(*(( int*) _temp893) ==  Cyc_Absyn_Const_e){
_LL934: _temp933=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp893)->f1; if((
unsigned int) _temp933 >  1u?*(( int*) _temp933) ==  Cyc_Absyn_Int_c: 0){ _LL936:
_temp935=(( struct Cyc_Absyn_Int_c_struct*) _temp933)->f2; goto _LL906;} else{
goto _LL907;}} else{ goto _LL907;} _LL907: if(*(( int*) _temp893) ==  Cyc_Absyn_Primop_e){
_LL943: _temp942=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp893)->f1;
if( _temp942 == ( void*) Cyc_Absyn_Size){ goto _LL938;} else{ goto _LL909;}
_LL938: _temp937=(( struct Cyc_Absyn_Primop_e_struct*) _temp893)->f2; if(
_temp937 ==  0){ goto _LL909;} else{ _temp939=* _temp937; _LL941: _temp940=(
struct Cyc_Absyn_Exp*) _temp939.hd; goto _LL908;}} else{ goto _LL909;} _LL909:
goto _LL910; _LL896: _temp496= _temp911; goto comp_loop; _LL898: _temp921=
_temp916; goto _LL900; _LL900: _temp926= _temp921; goto _LL902; _LL902: _temp931=
_temp926; goto _LL904; _LL904: new_relns=({ struct Cyc_List_List* _temp944=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp944->hd=(
void*)({ struct Cyc_CfFlowInfo_Reln* _temp945=( struct Cyc_CfFlowInfo_Reln*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln)); _temp945->vd= _temp498;
_temp945->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessVar_struct* _temp946=(
struct Cyc_CfFlowInfo_LessVar_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_LessVar_struct));
_temp946[ 0]=({ struct Cyc_CfFlowInfo_LessVar_struct _temp947; _temp947.tag= Cyc_CfFlowInfo_LessVar;
_temp947.f1= _temp931; _temp947;}); _temp946;})); _temp945;}); _temp944->tl=
_temp887; _temp944;}); goto _LL894; _LL906: new_relns=({ struct Cyc_List_List*
_temp948=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp948->hd=( void*)({ struct Cyc_CfFlowInfo_Reln* _temp949=( struct Cyc_CfFlowInfo_Reln*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln)); _temp949->vd= _temp498;
_temp949->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessConst_struct*
_temp950=( struct Cyc_CfFlowInfo_LessConst_struct*) _cycalloc_atomic( sizeof(
struct Cyc_CfFlowInfo_LessConst_struct)); _temp950[ 0]=({ struct Cyc_CfFlowInfo_LessConst_struct
_temp951; _temp951.tag= Cyc_CfFlowInfo_LessConst; _temp951.f1=( unsigned int)
_temp935; _temp951;}); _temp950;})); _temp949;}); _temp948->tl= _temp887;
_temp948;}); goto _LL894; _LL908:{ void* _temp952=( void*) _temp940->r; void*
_temp965; struct Cyc_Absyn_Vardecl* _temp967; void* _temp970; struct Cyc_Absyn_Vardecl*
_temp972; void* _temp975; struct Cyc_Absyn_Vardecl* _temp977; void* _temp980;
struct Cyc_Absyn_Vardecl* _temp982; _LL954: if(*(( int*) _temp952) ==  Cyc_Absyn_Var_e){
_LL966: _temp965=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp952)->f2; if((
unsigned int) _temp965 >  1u?*(( int*) _temp965) ==  Cyc_Absyn_Global_b: 0){
_LL968: _temp967=(( struct Cyc_Absyn_Global_b_struct*) _temp965)->f1; goto
_LL964;} else{ goto _LL956;}} else{ goto _LL956;} _LL964: if( ! _temp967->escapes){
goto _LL955;} else{ goto _LL956;} _LL956: if(*(( int*) _temp952) ==  Cyc_Absyn_Var_e){
_LL971: _temp970=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp952)->f2; if((
unsigned int) _temp970 >  1u?*(( int*) _temp970) ==  Cyc_Absyn_Local_b: 0){
_LL973: _temp972=(( struct Cyc_Absyn_Local_b_struct*) _temp970)->f1; goto _LL969;}
else{ goto _LL958;}} else{ goto _LL958;} _LL969: if( ! _temp972->escapes){ goto
_LL957;} else{ goto _LL958;} _LL958: if(*(( int*) _temp952) ==  Cyc_Absyn_Var_e){
_LL976: _temp975=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp952)->f2; if((
unsigned int) _temp975 >  1u?*(( int*) _temp975) ==  Cyc_Absyn_Pat_b: 0){ _LL978:
_temp977=(( struct Cyc_Absyn_Pat_b_struct*) _temp975)->f1; goto _LL974;} else{
goto _LL960;}} else{ goto _LL960;} _LL974: if( ! _temp977->escapes){ goto _LL959;}
else{ goto _LL960;} _LL960: if(*(( int*) _temp952) ==  Cyc_Absyn_Var_e){ _LL981:
_temp980=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp952)->f2; if((
unsigned int) _temp980 >  1u?*(( int*) _temp980) ==  Cyc_Absyn_Param_b: 0){
_LL983: _temp982=(( struct Cyc_Absyn_Param_b_struct*) _temp980)->f1; goto _LL979;}
else{ goto _LL962;}} else{ goto _LL962;} _LL979: if( ! _temp982->escapes){ goto
_LL961;} else{ goto _LL962;} _LL962: goto _LL963; _LL955: _temp972= _temp967;
goto _LL957; _LL957: _temp977= _temp972; goto _LL959; _LL959: _temp982= _temp977;
goto _LL961; _LL961: new_relns=({ struct Cyc_List_List* _temp984=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp984->hd=( void*)({ struct Cyc_CfFlowInfo_Reln*
_temp985=( struct Cyc_CfFlowInfo_Reln*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln));
_temp985->vd= _temp498; _temp985->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessSize_struct*
_temp986=( struct Cyc_CfFlowInfo_LessSize_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_LessSize_struct));
_temp986[ 0]=({ struct Cyc_CfFlowInfo_LessSize_struct _temp987; _temp987.tag=
Cyc_CfFlowInfo_LessSize; _temp987.f1= _temp982; _temp987;}); _temp986;}));
_temp985;}); _temp984->tl= _temp887; _temp984;}); goto _LL953; _LL963: goto
_LL953; _LL953:;} goto _LL894; _LL910: goto _LL894; _LL894:;} if( _temp887 != 
new_relns){ _temp879=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp988=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp988[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp989; _temp989.tag= Cyc_CfFlowInfo_ReachableFL; _temp989.f1= _temp889;
_temp989.f2= new_relns; _temp989;}); _temp988;});}{ void* _temp990= _temp877;
_LL992: if( _temp990 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL993;} else{ goto
_LL994;} _LL994: if( _temp990 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto
_LL995;} else{ goto _LL996;} _LL996: if( _temp990 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL997;} else{ goto _LL998;} _LL998: if(( unsigned int) _temp990 >  3u?*((
int*) _temp990) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL999;} else{ goto
_LL1000;} _LL1000: goto _LL1001; _LL993: return({ struct _tuple0 _temp1002;
_temp1002.f1= _temp879; _temp1002.f2= Cyc_CfFlowInfo_unknown_all; _temp1002;});
_LL995: goto _LL997; _LL997: goto _LL999; _LL999: { struct Cyc_List_List
_temp1003=({ struct Cyc_List_List _temp1021; _temp1021.hd=( void*) _temp498;
_temp1021.tl= 0; _temp1021;}); _temp879= Cyc_NewControlFlow_add_vars( _temp879,(
struct Cyc_List_List*)& _temp1003, Cyc_CfFlowInfo_unknown_all);{ void* _temp1006;
void* _temp1008; struct _tuple0 _temp1004= Cyc_NewControlFlow_anal_Rexp( env,
_temp879, _temp494); _LL1009: _temp1008= _temp1004.f1; goto _LL1007; _LL1007:
_temp1006= _temp1004.f2; goto _LL1005; _LL1005:{ void* _temp1010= _temp1008;
struct Cyc_Dict_Dict* _temp1016; _LL1012: if( _temp1010 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1013;} else{ goto _LL1014;} _LL1014: if(( unsigned int) _temp1010 >  1u?*((
int*) _temp1010) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1017: _temp1016=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1010)->f1; goto _LL1015;} else{
goto _LL1011;} _LL1013: return({ struct _tuple0 _temp1018; _temp1018.f1=
_temp1008; _temp1018.f2= Cyc_CfFlowInfo_unknown_all; _temp1018;}); _LL1015: if(
Cyc_CfFlowInfo_initlevel( _temp1016, _temp1006) != ( void*) Cyc_CfFlowInfo_AllIL){({
void* _temp1019[ 0u]={}; Cyc_Tcutil_terr( _temp496->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1019, sizeof( void*), 0u));});
return({ struct _tuple0 _temp1020; _temp1020.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1020.f2= Cyc_CfFlowInfo_unknown_all; _temp1020;});} _LL1011:;} _temp879=
_temp1008; goto _LL1001;}} _LL1001: while( 1) { struct Cyc_List_List _temp1022=({
struct Cyc_List_List _temp1040; _temp1040.hd=( void*) _temp498; _temp1040.tl= 0;
_temp1040;}); _temp879= Cyc_NewControlFlow_add_vars( _temp879,( struct Cyc_List_List*)&
_temp1022, Cyc_CfFlowInfo_unknown_all);{ void* _temp1025; void* _temp1027;
struct _tuple0 _temp1023= Cyc_NewControlFlow_anal_Rexp( env, _temp879, _temp494);
_LL1028: _temp1027= _temp1023.f1; goto _LL1026; _LL1026: _temp1025= _temp1023.f2;
goto _LL1024; _LL1024:{ void* _temp1029= _temp1027; struct Cyc_Dict_Dict*
_temp1035; _LL1031: if( _temp1029 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1032;} else{ goto _LL1033;} _LL1033: if(( unsigned int) _temp1029 >  1u?*((
int*) _temp1029) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1036: _temp1035=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1029)->f1; goto _LL1034;} else{
goto _LL1030;} _LL1032: goto _LL1030; _LL1034: if( Cyc_CfFlowInfo_initlevel(
_temp1035, _temp1025) != ( void*) Cyc_CfFlowInfo_AllIL){({ void* _temp1037[ 0u]={};
Cyc_Tcutil_terr( _temp496->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1037, sizeof( void*), 0u));});
return({ struct _tuple0 _temp1038; _temp1038.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1038.f2= Cyc_CfFlowInfo_unknown_all; _temp1038;});} _LL1030:;}{ void*
_temp1039= Cyc_CfFlowInfo_join_flow( env->all_changed, _temp879, _temp1027); if(
Cyc_CfFlowInfo_flow_lessthan_approx( _temp1039, _temp879)){ break;} _temp879=
_temp1039;}}} return({ struct _tuple0 _temp1041; _temp1041.f1= _temp879;
_temp1041.f2= Cyc_CfFlowInfo_unknown_all; _temp1041;}); _LL991:;}} _LL882:;}}
_LL368: while( 1) { inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check( env,
inflow, _temp500))->flow;{ void* _temp1042=( void*) _temp500->r; struct Cyc_Absyn_Stmt*
_temp1052; struct Cyc_Absyn_Stmt* _temp1054; struct Cyc_Absyn_Stmt* _temp1056;
struct Cyc_Absyn_Decl* _temp1058; struct Cyc_Absyn_Exp* _temp1060; _LL1044: if((
unsigned int) _temp1042 >  1u?*(( int*) _temp1042) ==  Cyc_Absyn_Seq_s: 0){
_LL1055: _temp1054=(( struct Cyc_Absyn_Seq_s_struct*) _temp1042)->f1; goto
_LL1053; _LL1053: _temp1052=(( struct Cyc_Absyn_Seq_s_struct*) _temp1042)->f2;
goto _LL1045;} else{ goto _LL1046;} _LL1046: if(( unsigned int) _temp1042 >  1u?*((
int*) _temp1042) ==  Cyc_Absyn_Decl_s: 0){ _LL1059: _temp1058=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1042)->f1; goto _LL1057; _LL1057: _temp1056=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1042)->f2; goto _LL1047;} else{ goto _LL1048;} _LL1048: if(( unsigned int)
_temp1042 >  1u?*(( int*) _temp1042) ==  Cyc_Absyn_Exp_s: 0){ _LL1061: _temp1060=((
struct Cyc_Absyn_Exp_s_struct*) _temp1042)->f1; goto _LL1049;} else{ goto
_LL1050;} _LL1050: goto _LL1051; _LL1045: inflow= Cyc_NewControlFlow_anal_stmt(
env, inflow, _temp1054); _temp500= _temp1052; goto _LL1043; _LL1047: inflow= Cyc_NewControlFlow_anal_decl(
env, inflow, _temp1058); _temp500= _temp1056; goto _LL1043; _LL1049: return Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1060); _LL1051:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1062=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1062[ 0]=({ struct Cyc_Core_Impossible_struct _temp1063; _temp1063.tag= Cyc_Core_Impossible;
_temp1063.f1= _tag_arr("analyze_Rexp: ill-formed StmtExp", sizeof( unsigned char),
33u); _temp1063;}); _temp1062;})); _LL1043:;}} _LL370: goto _LL372; _LL372: goto
_LL374; _LL374: goto _LL376; _LL376: goto _LL378; _LL378: goto _LL380; _LL380:
goto _LL382; _LL382:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1064=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1064[ 0]=({ struct Cyc_Core_Impossible_struct _temp1065; _temp1065.tag= Cyc_Core_Impossible;
_temp1065.f1= _tag_arr("anal_Rexp, unexpected exp form", sizeof( unsigned char),
31u); _temp1065;}); _temp1064;})); _LL286:;}} static struct _tuple0 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, void* f, struct Cyc_Absyn_Exp*
e, void* r, struct Cyc_List_List* flds){ void* _temp1066= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo
_temp1072; struct Cyc_Absyn_Conref* _temp1074; void* _temp1076; _LL1068: if((
unsigned int) _temp1066 >  3u?*(( int*) _temp1066) ==  Cyc_Absyn_PointerType: 0){
_LL1073: _temp1072=(( struct Cyc_Absyn_PointerType_struct*) _temp1066)->f1;
_LL1077: _temp1076=( void*) _temp1072.elt_typ; goto _LL1075; _LL1075: _temp1074=
_temp1072.bounds; goto _LL1069;} else{ goto _LL1070;} _LL1070: goto _LL1071;
_LL1069: { void* _temp1078= f; struct Cyc_List_List* _temp1084; struct Cyc_Dict_Dict*
_temp1086; _LL1080: if( _temp1078 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1081;} else{ goto _LL1082;} _LL1082: if(( unsigned int) _temp1078 >  1u?*((
int*) _temp1078) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1087: _temp1086=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1078)->f1; goto _LL1085; _LL1085:
_temp1084=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1078)->f2; goto
_LL1083;} else{ goto _LL1079;} _LL1081: return({ struct _tuple0 _temp1088;
_temp1088.f1= f; _temp1088.f2=( void*) Cyc_CfFlowInfo_UnknownL; _temp1088;});
_LL1083: if( Cyc_Tcutil_is_bound_one( _temp1074)){ void* _temp1089= r; struct
Cyc_CfFlowInfo_Place* _temp1103; struct Cyc_CfFlowInfo_Place _temp1105; struct
Cyc_List_List* _temp1106; void* _temp1108; void* _temp1110; _LL1091: if(
_temp1089 == ( void*) Cyc_CfFlowInfo_NotZeroAll){ goto _LL1092;} else{ goto
_LL1093;} _LL1093: if( _temp1089 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto
_LL1094;} else{ goto _LL1095;} _LL1095: if(( unsigned int) _temp1089 >  3u?*((
int*) _temp1089) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL1104: _temp1103=(( struct
Cyc_CfFlowInfo_AddressOf_struct*) _temp1089)->f1; _temp1105=* _temp1103; _LL1109:
_temp1108=( void*) _temp1105.root; goto _LL1107; _LL1107: _temp1106= _temp1105.fields;
goto _LL1096;} else{ goto _LL1097;} _LL1097: if( _temp1089 == ( void*) Cyc_CfFlowInfo_Zero){
goto _LL1098;} else{ goto _LL1099;} _LL1099: if(( unsigned int) _temp1089 >  3u?*((
int*) _temp1089) ==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1111: _temp1110=( void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*) _temp1089)->f1; goto _LL1100;} else{
goto _LL1101;} _LL1101: goto _LL1102; _LL1092: goto _LL1094; _LL1094:( void*)( e->annot=(
void*)(( void*)({ struct Cyc_CfFlowInfo_NotZero_struct* _temp1112=( struct Cyc_CfFlowInfo_NotZero_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_NotZero_struct)); _temp1112[ 0]=({
struct Cyc_CfFlowInfo_NotZero_struct _temp1113; _temp1113.tag= Cyc_CfFlowInfo_NotZero;
_temp1113.f1= _temp1084; _temp1113;}); _temp1112;}))); goto _LL1090; _LL1096:(
void*)( e->annot=( void*)(( void*)({ struct Cyc_CfFlowInfo_NotZero_struct*
_temp1114=( struct Cyc_CfFlowInfo_NotZero_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_NotZero_struct));
_temp1114[ 0]=({ struct Cyc_CfFlowInfo_NotZero_struct _temp1115; _temp1115.tag=
Cyc_CfFlowInfo_NotZero; _temp1115.f1= _temp1084; _temp1115;}); _temp1114;})));
return({ struct _tuple0 _temp1116; _temp1116.f1= f; _temp1116.f2=( void*)({
struct Cyc_CfFlowInfo_PlaceL_struct* _temp1117=( struct Cyc_CfFlowInfo_PlaceL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_PlaceL_struct)); _temp1117[ 0]=({
struct Cyc_CfFlowInfo_PlaceL_struct _temp1118; _temp1118.tag= Cyc_CfFlowInfo_PlaceL;
_temp1118.f1=({ struct Cyc_CfFlowInfo_Place* _temp1119=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp1119->root=( void*)
_temp1108; _temp1119->fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1106, flds); _temp1119;});
_temp1118;}); _temp1117;}); _temp1116;}); _LL1098:( void*)( e->annot=( void*)((
void*)({ struct Cyc_CfFlowInfo_IsZero_struct* _temp1120=( struct Cyc_CfFlowInfo_IsZero_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_IsZero_struct)); _temp1120[ 0]=({
struct Cyc_CfFlowInfo_IsZero_struct _temp1121; _temp1121.tag= Cyc_CfFlowInfo_IsZero;
_temp1121.f1= _temp1084; _temp1121;}); _temp1120;}))); return({ struct _tuple0
_temp1122; _temp1122.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp1122.f2=( void*)
Cyc_CfFlowInfo_UnknownL; _temp1122;}); _LL1100: f= Cyc_NewControlFlow_notzero(
env, inflow, f, e, _temp1110); goto _LL1102; _LL1102:( void*)( e->annot=( void*)((
void*)({ struct Cyc_CfFlowInfo_UnknownZ_struct* _temp1123=( struct Cyc_CfFlowInfo_UnknownZ_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_UnknownZ_struct)); _temp1123[ 0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _temp1124; _temp1124.tag= Cyc_CfFlowInfo_UnknownZ;
_temp1124.f1= _temp1084; _temp1124;}); _temp1123;}))); _LL1090:;} else{( void*)(
e->annot=( void*)(( void*)({ struct Cyc_CfFlowInfo_UnknownZ_struct* _temp1125=(
struct Cyc_CfFlowInfo_UnknownZ_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_UnknownZ_struct));
_temp1125[ 0]=({ struct Cyc_CfFlowInfo_UnknownZ_struct _temp1126; _temp1126.tag=
Cyc_CfFlowInfo_UnknownZ; _temp1126.f1= _temp1084; _temp1126;}); _temp1125;})));}
if( Cyc_CfFlowInfo_initlevel( _temp1086, r) == ( void*) Cyc_CfFlowInfo_NoneIL){({
void* _temp1127[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("dereference of possibly uninitialized pointer",
sizeof( unsigned char), 46u), _tag_arr( _temp1127, sizeof( void*), 0u));});}
return({ struct _tuple0 _temp1128; _temp1128.f1= f; _temp1128.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp1128;}); _LL1079:;} _LL1071:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1129=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1129[ 0]=({ struct Cyc_Core_Impossible_struct _temp1130; _temp1130.tag= Cyc_Core_Impossible;
_temp1130.f1= _tag_arr("left deref of non-pointer-type", sizeof( unsigned char),
31u); _temp1130;}); _temp1129;})); _LL1067:;} static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* flds){ struct Cyc_Dict_Dict* d;{ void* _temp1131= inflow;
struct Cyc_List_List* _temp1137; struct Cyc_Dict_Dict* _temp1139; _LL1133: if(
_temp1131 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1134;} else{ goto
_LL1135;} _LL1135: if(( unsigned int) _temp1131 >  1u?*(( int*) _temp1131) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL1140: _temp1139=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1131)->f1; goto _LL1138; _LL1138: _temp1137=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1131)->f2; goto _LL1136;} else{ goto _LL1132;} _LL1134: return({ struct
_tuple0 _temp1141; _temp1141.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp1141.f2=(
void*) Cyc_CfFlowInfo_UnknownL; _temp1141;}); _LL1136: d= _temp1139; _LL1132:;}{
void* _temp1142=( void*) e->r; struct Cyc_Absyn_Exp* _temp1168; struct Cyc_Absyn_Exp*
_temp1170; struct Cyc_Absyn_Exp* _temp1172; void* _temp1174; struct Cyc_Absyn_Vardecl*
_temp1176; void* _temp1178; struct Cyc_Absyn_Vardecl* _temp1180; void* _temp1182;
struct Cyc_Absyn_Vardecl* _temp1184; void* _temp1186; struct Cyc_Absyn_Vardecl*
_temp1188; struct _tagged_arr* _temp1190; struct Cyc_Absyn_Exp* _temp1192;
struct Cyc_Absyn_Exp* _temp1194; struct Cyc_Absyn_Exp* _temp1196; struct Cyc_Absyn_Exp*
_temp1198; struct _tagged_arr* _temp1200; struct Cyc_Absyn_Exp* _temp1202;
_LL1144: if(*(( int*) _temp1142) ==  Cyc_Absyn_Cast_e){ _LL1169: _temp1168=((
struct Cyc_Absyn_Cast_e_struct*) _temp1142)->f2; goto _LL1145;} else{ goto
_LL1146;} _LL1146: if(*(( int*) _temp1142) ==  Cyc_Absyn_NoInstantiate_e){
_LL1171: _temp1170=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1142)->f1;
goto _LL1147;} else{ goto _LL1148;} _LL1148: if(*(( int*) _temp1142) ==  Cyc_Absyn_Instantiate_e){
_LL1173: _temp1172=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1142)->f1;
goto _LL1149;} else{ goto _LL1150;} _LL1150: if(*(( int*) _temp1142) ==  Cyc_Absyn_Var_e){
_LL1175: _temp1174=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1142)->f2;
if(( unsigned int) _temp1174 >  1u?*(( int*) _temp1174) ==  Cyc_Absyn_Param_b: 0){
_LL1177: _temp1176=(( struct Cyc_Absyn_Param_b_struct*) _temp1174)->f1; goto
_LL1151;} else{ goto _LL1152;}} else{ goto _LL1152;} _LL1152: if(*(( int*)
_temp1142) ==  Cyc_Absyn_Var_e){ _LL1179: _temp1178=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1142)->f2; if(( unsigned int) _temp1178 >  1u?*(( int*) _temp1178) ==  Cyc_Absyn_Local_b:
0){ _LL1181: _temp1180=(( struct Cyc_Absyn_Local_b_struct*) _temp1178)->f1; goto
_LL1153;} else{ goto _LL1154;}} else{ goto _LL1154;} _LL1154: if(*(( int*)
_temp1142) ==  Cyc_Absyn_Var_e){ _LL1183: _temp1182=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1142)->f2; if(( unsigned int) _temp1182 >  1u?*(( int*) _temp1182) ==  Cyc_Absyn_Pat_b:
0){ _LL1185: _temp1184=(( struct Cyc_Absyn_Pat_b_struct*) _temp1182)->f1; goto
_LL1155;} else{ goto _LL1156;}} else{ goto _LL1156;} _LL1156: if(*(( int*)
_temp1142) ==  Cyc_Absyn_Var_e){ _LL1187: _temp1186=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1142)->f2; if(( unsigned int) _temp1186 >  1u?*(( int*) _temp1186) ==  Cyc_Absyn_Global_b:
0){ _LL1189: _temp1188=(( struct Cyc_Absyn_Global_b_struct*) _temp1186)->f1;
goto _LL1157;} else{ goto _LL1158;}} else{ goto _LL1158;} _LL1158: if(*(( int*)
_temp1142) ==  Cyc_Absyn_StructArrow_e){ _LL1193: _temp1192=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1142)->f1; goto _LL1191; _LL1191: _temp1190=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1142)->f2; goto _LL1159;} else{ goto _LL1160;} _LL1160: if(*(( int*)
_temp1142) ==  Cyc_Absyn_Deref_e){ _LL1195: _temp1194=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1142)->f1; goto _LL1161;} else{ goto _LL1162;} _LL1162: if(*(( int*)
_temp1142) ==  Cyc_Absyn_Subscript_e){ _LL1199: _temp1198=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1142)->f1; goto _LL1197; _LL1197: _temp1196=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1142)->f2; goto _LL1163;} else{ goto _LL1164;} _LL1164: if(*(( int*)
_temp1142) ==  Cyc_Absyn_StructMember_e){ _LL1203: _temp1202=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1142)->f1; goto _LL1201; _LL1201: _temp1200=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1142)->f2; goto _LL1165;} else{ goto _LL1166;} _LL1166: goto _LL1167;
_LL1145: _temp1170= _temp1168; goto _LL1147; _LL1147: _temp1172= _temp1170; goto
_LL1149; _LL1149: return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow,
_temp1172, flds); _LL1151: _temp1180= _temp1176; goto _LL1153; _LL1153:
_temp1184= _temp1180; goto _LL1155; _LL1155: return({ struct _tuple0 _temp1204;
_temp1204.f1= inflow; _temp1204.f2=( void*)({ struct Cyc_CfFlowInfo_PlaceL_struct*
_temp1205=( struct Cyc_CfFlowInfo_PlaceL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_PlaceL_struct));
_temp1205[ 0]=({ struct Cyc_CfFlowInfo_PlaceL_struct _temp1206; _temp1206.tag=
Cyc_CfFlowInfo_PlaceL; _temp1206.f1=({ struct Cyc_CfFlowInfo_Place* _temp1207=(
struct Cyc_CfFlowInfo_Place*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Place));
_temp1207->root=( void*)(( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1208=( struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1208[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp1209; _temp1209.tag=
Cyc_CfFlowInfo_VarRoot; _temp1209.f1= _temp1184; _temp1209;}); _temp1208;}));
_temp1207->fields= flds; _temp1207;}); _temp1206;}); _temp1205;}); _temp1204;});
_LL1157: return({ struct _tuple0 _temp1210; _temp1210.f1= inflow; _temp1210.f2=(
void*) Cyc_CfFlowInfo_UnknownL; _temp1210;}); _LL1159:{ void* _temp1211= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1192->topt))->v); _LL1213: if((
unsigned int) _temp1211 >  3u?*(( int*) _temp1211) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL1214;} else{ goto _LL1215;} _LL1215: if(( unsigned int) _temp1211 > 
3u?*(( int*) _temp1211) ==  Cyc_Absyn_UnionType: 0){ goto _LL1216;} else{ goto
_LL1217;} _LL1217: goto _LL1218; _LL1214: goto _LL1216; _LL1216: goto _LL1212;
_LL1218: flds=({ struct Cyc_List_List* _temp1219=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1219->hd=( void*) _temp1190;
_temp1219->tl= flds; _temp1219;}); _LL1212:;} _temp1194= _temp1192; goto _LL1161;
_LL1161: { void* _temp1222; void* _temp1224; struct _tuple0 _temp1220= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1194); _LL1225: _temp1224= _temp1220.f1; goto _LL1223; _LL1223:
_temp1222= _temp1220.f2; goto _LL1221; _LL1221: return Cyc_NewControlFlow_anal_derefL(
env, inflow, _temp1224, _temp1194, _temp1222, flds);} _LL1163:{ void* _temp1226=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1198->topt))->v);
_LL1228: if(( unsigned int) _temp1226 >  3u?*(( int*) _temp1226) ==  Cyc_Absyn_TupleType:
0){ goto _LL1229;} else{ goto _LL1230;} _LL1230: goto _LL1231; _LL1229: { struct
_tagged_arr* _temp1232= Cyc_Absyn_fieldname(( int) Cyc_Evexp_eval_const_uint_exp(
_temp1196)); return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, _temp1198,({
struct Cyc_List_List* _temp1233=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1233->hd=( void*) _temp1232; _temp1233->tl= flds;
_temp1233;}));} _LL1231: goto _LL1227; _LL1227:;}{ struct Cyc_List_List*
_temp1237; void* _temp1239; struct _tuple4 _temp1235= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp1234[ 2u]; _temp1234[ 1u]= _temp1196;
_temp1234[ 0u]= _temp1198;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1234, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL1240:
_temp1239= _temp1235.f1; goto _LL1238; _LL1238: _temp1237= _temp1235.f2; goto
_LL1236; _LL1236:{ void* _temp1241= _temp1239; struct Cyc_List_List* _temp1247;
struct Cyc_Dict_Dict* _temp1249; _LL1243: if(( unsigned int) _temp1241 >  1u?*((
int*) _temp1241) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1250: _temp1249=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1241)->f1; goto _LL1248; _LL1248:
_temp1247=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1241)->f2; goto
_LL1244;} else{ goto _LL1245;} _LL1245: goto _LL1246; _LL1244: if( Cyc_CfFlowInfo_initlevel(
_temp1249,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1237))->tl))->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void*
_temp1251[ 0u]={}; Cyc_Tcutil_terr( _temp1196->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1251, sizeof( void*), 0u));});}{
struct Cyc_List_List* _temp1252= Cyc_NewControlFlow_add_subscript_reln(
_temp1247, _temp1198, _temp1196); if( _temp1247 !=  _temp1252){ _temp1239=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp1253=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp1253[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp1254; _temp1254.tag= Cyc_CfFlowInfo_ReachableFL;
_temp1254.f1= _temp1249; _temp1254.f2= _temp1252; _temp1254;}); _temp1253;});}
goto _LL1242;} _LL1246: goto _LL1242; _LL1242:;} return Cyc_NewControlFlow_anal_derefL(
env, inflow, _temp1239, _temp1198,( void*)(( struct Cyc_List_List*) _check_null(
_temp1237))->hd, flds);} _LL1165:{ void* _temp1255= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1202->topt))->v); _LL1257: if((
unsigned int) _temp1255 >  3u?*(( int*) _temp1255) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL1258;} else{ goto _LL1259;} _LL1259: if(( unsigned int) _temp1255 > 
3u?*(( int*) _temp1255) ==  Cyc_Absyn_UnionType: 0){ goto _LL1260;} else{ goto
_LL1261;} _LL1261: goto _LL1262; _LL1258: goto _LL1260; _LL1260: return({ struct
_tuple0 _temp1263; _temp1263.f1= inflow; _temp1263.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp1263;}); _LL1262: flds=({ struct Cyc_List_List* _temp1264=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1264->hd=( void*) _temp1200;
_temp1264->tl= flds; _temp1264;}); _LL1256:;} return Cyc_NewControlFlow_anal_Lexp_rec(
env, inflow, _temp1202, flds); _LL1167: return({ struct _tuple0 _temp1265;
_temp1265.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp1265.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp1265;}); _LL1143:;}} static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e){
return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, e, 0);} static struct
_tuple0 Cyc_NewControlFlow_anal_test( struct Cyc_NewControlFlow_AnalEnv* env,
void* inflow, struct Cyc_Absyn_Exp* e){{ void* _temp1266=( void*) e->r; struct
Cyc_Absyn_Exp* _temp1284; struct Cyc_Absyn_Exp* _temp1286; struct Cyc_Absyn_Exp*
_temp1288; struct Cyc_Absyn_Exp* _temp1290; struct Cyc_Absyn_Exp* _temp1292;
struct Cyc_List_List* _temp1294; struct Cyc_List_List _temp1296; struct Cyc_List_List*
_temp1297; struct Cyc_Absyn_Exp* _temp1299; void* _temp1301; struct Cyc_List_List*
_temp1303; void* _temp1305; struct Cyc_List_List* _temp1307; void* _temp1309;
struct Cyc_List_List* _temp1311; void* _temp1313; struct Cyc_List_List*
_temp1315; void* _temp1317; _LL1268: if(*(( int*) _temp1266) ==  Cyc_Absyn_Conditional_e){
_LL1289: _temp1288=(( struct Cyc_Absyn_Conditional_e_struct*) _temp1266)->f1;
goto _LL1287; _LL1287: _temp1286=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1266)->f2; goto _LL1285; _LL1285: _temp1284=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1266)->f3; goto _LL1269;} else{ goto _LL1270;} _LL1270: if(*(( int*)
_temp1266) ==  Cyc_Absyn_SeqExp_e){ _LL1293: _temp1292=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1266)->f1; goto _LL1291; _LL1291: _temp1290=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1266)->f2; goto _LL1271;} else{ goto _LL1272;} _LL1272: if(*(( int*)
_temp1266) ==  Cyc_Absyn_Primop_e){ _LL1302: _temp1301=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1266)->f1; if( _temp1301 == ( void*) Cyc_Absyn_Not){ goto _LL1295;} else{
goto _LL1274;} _LL1295: _temp1294=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1266)->f2; if( _temp1294 ==  0){ goto _LL1274;} else{ _temp1296=* _temp1294;
_LL1300: _temp1299=( struct Cyc_Absyn_Exp*) _temp1296.hd; goto _LL1298; _LL1298:
_temp1297= _temp1296.tl; if( _temp1297 ==  0){ goto _LL1273;} else{ goto _LL1274;}}}
else{ goto _LL1274;} _LL1274: if(*(( int*) _temp1266) ==  Cyc_Absyn_Primop_e){
_LL1306: _temp1305=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1266)->f1;
if( _temp1305 == ( void*) Cyc_Absyn_Eq){ goto _LL1304;} else{ goto _LL1276;}
_LL1304: _temp1303=(( struct Cyc_Absyn_Primop_e_struct*) _temp1266)->f2; goto
_LL1275;} else{ goto _LL1276;} _LL1276: if(*(( int*) _temp1266) ==  Cyc_Absyn_Primop_e){
_LL1310: _temp1309=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1266)->f1;
if( _temp1309 == ( void*) Cyc_Absyn_Neq){ goto _LL1308;} else{ goto _LL1278;}
_LL1308: _temp1307=(( struct Cyc_Absyn_Primop_e_struct*) _temp1266)->f2; goto
_LL1277;} else{ goto _LL1278;} _LL1278: if(*(( int*) _temp1266) ==  Cyc_Absyn_Primop_e){
_LL1314: _temp1313=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1266)->f1;
if( _temp1313 == ( void*) Cyc_Absyn_Lt){ goto _LL1312;} else{ goto _LL1280;}
_LL1312: _temp1311=(( struct Cyc_Absyn_Primop_e_struct*) _temp1266)->f2; goto
_LL1279;} else{ goto _LL1280;} _LL1280: if(*(( int*) _temp1266) ==  Cyc_Absyn_Primop_e){
_LL1318: _temp1317=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1266)->f1;
if( _temp1317 == ( void*) Cyc_Absyn_Lte){ goto _LL1316;} else{ goto _LL1282;}
_LL1316: _temp1315=(( struct Cyc_Absyn_Primop_e_struct*) _temp1266)->f2; goto
_LL1281;} else{ goto _LL1282;} _LL1282: goto _LL1283; _LL1269: { void* _temp1321;
void* _temp1323; struct _tuple0 _temp1319= Cyc_NewControlFlow_anal_test( env,
inflow, _temp1288); _LL1324: _temp1323= _temp1319.f1; goto _LL1322; _LL1322:
_temp1321= _temp1319.f2; goto _LL1320; _LL1320: { void* _temp1327; void*
_temp1329; struct _tuple0 _temp1325= Cyc_NewControlFlow_anal_test( env,
_temp1323, _temp1286); _LL1330: _temp1329= _temp1325.f1; goto _LL1328; _LL1328:
_temp1327= _temp1325.f2; goto _LL1326; _LL1326: { void* _temp1333; void*
_temp1335; struct _tuple0 _temp1331= Cyc_NewControlFlow_anal_test( env,
_temp1321, _temp1284); _LL1336: _temp1335= _temp1331.f1; goto _LL1334; _LL1334:
_temp1333= _temp1331.f2; goto _LL1332; _LL1332: return({ struct _tuple0
_temp1337; _temp1337.f1= Cyc_CfFlowInfo_join_flow( env->all_changed, _temp1329,
_temp1335); _temp1337.f2= Cyc_CfFlowInfo_join_flow( env->all_changed, _temp1327,
_temp1333); _temp1337;});}}} _LL1271: return Cyc_NewControlFlow_anal_test( env,(
Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp1292)).f1, _temp1290); _LL1273: {
void* _temp1340; void* _temp1342; struct _tuple0 _temp1338= Cyc_NewControlFlow_anal_test(
env, inflow, _temp1299); _LL1343: _temp1342= _temp1338.f1; goto _LL1341; _LL1341:
_temp1340= _temp1338.f2; goto _LL1339; _LL1339: return({ struct _tuple0
_temp1344; _temp1344.f1= _temp1340; _temp1344.f2= _temp1342; _temp1344;});}
_LL1275: { struct Cyc_List_List* _temp1347; void* _temp1349; struct _tuple4
_temp1345= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1303);
_LL1350: _temp1349= _temp1345.f1; goto _LL1348; _LL1348: _temp1347= _temp1345.f2;
goto _LL1346; _LL1346: { void* _temp1351= _temp1349; struct Cyc_List_List*
_temp1357; struct Cyc_Dict_Dict* _temp1359; _LL1353: if( _temp1351 == ( void*)
Cyc_CfFlowInfo_BottomFL){ goto _LL1354;} else{ goto _LL1355;} _LL1355: if((
unsigned int) _temp1351 >  1u?*(( int*) _temp1351) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL1360: _temp1359=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1351)->f1;
goto _LL1358; _LL1358: _temp1357=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1351)->f2; goto _LL1356;} else{ goto _LL1352;} _LL1354: return({ struct
_tuple0 _temp1361; _temp1361.f1= _temp1349; _temp1361.f2= _temp1349; _temp1361;});
_LL1356: { void* _temp1362=( void*)(( struct Cyc_List_List*) _check_null(
_temp1347))->hd; void* _temp1363=( void*)(( struct Cyc_List_List*) _check_null(
_temp1347->tl))->hd; if( Cyc_CfFlowInfo_initlevel( _temp1359, _temp1362) == (
void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1364[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1303))->hd)->loc,
_tag_arr("expression may not be initialized", sizeof( unsigned char), 34u),
_tag_arr( _temp1364, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1359, _temp1363) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1365[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1303))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1365, sizeof( void*), 0u));});}{
struct _tuple0 _temp1367=({ struct _tuple0 _temp1366; _temp1366.f1= _temp1362;
_temp1366.f2= _temp1363; _temp1366;}); void* _temp1375; void* _temp1377; void*
_temp1379; void* _temp1381; void* _temp1383; void* _temp1385; _LL1369: _LL1378:
_temp1377= _temp1367.f1; if(( unsigned int) _temp1377 >  3u?*(( int*) _temp1377)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1380: _temp1379=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1377)->f1; goto _LL1376;} else{ goto _LL1371;} _LL1376: _temp1375=
_temp1367.f2; if( _temp1375 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1370;}
else{ goto _LL1371;} _LL1371: _LL1386: _temp1385= _temp1367.f1; if( _temp1385 == (
void*) Cyc_CfFlowInfo_Zero){ goto _LL1382;} else{ goto _LL1373;} _LL1382:
_temp1381= _temp1367.f2; if(( unsigned int) _temp1381 >  3u?*(( int*) _temp1381)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1384: _temp1383=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1381)->f1; goto _LL1372;} else{ goto _LL1373;} _LL1373: goto _LL1374;
_LL1370: { void* _temp1389; void* _temp1391; struct _tuple0 _temp1387= Cyc_NewControlFlow_splitzero(
env, _temp1349, _temp1349,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1303))->hd, _temp1379); _LL1392: _temp1391= _temp1387.f1; goto
_LL1390; _LL1390: _temp1389= _temp1387.f2; goto _LL1388; _LL1388: return({
struct _tuple0 _temp1393; _temp1393.f1= _temp1389; _temp1393.f2= _temp1391;
_temp1393;});} _LL1372: { void* _temp1396; void* _temp1398; struct _tuple0
_temp1394= Cyc_NewControlFlow_splitzero( env, _temp1349, _temp1349,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1303))->tl))->hd, _temp1383); _LL1399: _temp1398= _temp1394.f1; goto
_LL1397; _LL1397: _temp1396= _temp1394.f2; goto _LL1395; _LL1395: return({
struct _tuple0 _temp1400; _temp1400.f1= _temp1396; _temp1400.f2= _temp1398;
_temp1400;});} _LL1374: return({ struct _tuple0 _temp1401; _temp1401.f1=
_temp1349; _temp1401.f2= _temp1349; _temp1401;}); _LL1368:;}} _LL1352:;}}
_LL1277: { struct Cyc_List_List* _temp1404; void* _temp1406; struct _tuple4
_temp1402= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1307);
_LL1407: _temp1406= _temp1402.f1; goto _LL1405; _LL1405: _temp1404= _temp1402.f2;
goto _LL1403; _LL1403: { void* _temp1408= _temp1406; struct Cyc_List_List*
_temp1414; struct Cyc_Dict_Dict* _temp1416; _LL1410: if( _temp1408 == ( void*)
Cyc_CfFlowInfo_BottomFL){ goto _LL1411;} else{ goto _LL1412;} _LL1412: if((
unsigned int) _temp1408 >  1u?*(( int*) _temp1408) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL1417: _temp1416=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1408)->f1;
goto _LL1415; _LL1415: _temp1414=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1408)->f2; goto _LL1413;} else{ goto _LL1409;} _LL1411: return({ struct
_tuple0 _temp1418; _temp1418.f1= _temp1406; _temp1418.f2= _temp1406; _temp1418;});
_LL1413: { void* _temp1419=( void*)(( struct Cyc_List_List*) _check_null(
_temp1404))->hd; void* _temp1420=( void*)(( struct Cyc_List_List*) _check_null(
_temp1404->tl))->hd; if( Cyc_CfFlowInfo_initlevel( _temp1416, _temp1419) == (
void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1421[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1307))->hd)->loc,
_tag_arr("expression may not be initialized", sizeof( unsigned char), 34u),
_tag_arr( _temp1421, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1416, _temp1420) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1422[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1307))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1422, sizeof( void*), 0u));});}{
struct _tuple0 _temp1424=({ struct _tuple0 _temp1423; _temp1423.f1= _temp1419;
_temp1423.f2= _temp1420; _temp1423;}); void* _temp1432; void* _temp1434; void*
_temp1436; void* _temp1438; void* _temp1440; void* _temp1442; _LL1426: _LL1435:
_temp1434= _temp1424.f1; if(( unsigned int) _temp1434 >  3u?*(( int*) _temp1434)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1437: _temp1436=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1434)->f1; goto _LL1433;} else{ goto _LL1428;} _LL1433: _temp1432=
_temp1424.f2; if( _temp1432 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1427;}
else{ goto _LL1428;} _LL1428: _LL1443: _temp1442= _temp1424.f1; if( _temp1442 == (
void*) Cyc_CfFlowInfo_Zero){ goto _LL1439;} else{ goto _LL1430;} _LL1439:
_temp1438= _temp1424.f2; if(( unsigned int) _temp1438 >  3u?*(( int*) _temp1438)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1441: _temp1440=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1438)->f1; goto _LL1429;} else{ goto _LL1430;} _LL1430: goto _LL1431;
_LL1427: return Cyc_NewControlFlow_splitzero( env, _temp1406, _temp1406,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1307))->hd, _temp1436);
_LL1429: return Cyc_NewControlFlow_splitzero( env, _temp1406, _temp1406,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1307))->tl))->hd, _temp1440); _LL1431: return({ struct _tuple0
_temp1444; _temp1444.f1= _temp1406; _temp1444.f2= _temp1406; _temp1444;});
_LL1425:;}} _LL1409:;}} _LL1279: { struct Cyc_List_List* _temp1447; void*
_temp1449; struct _tuple4 _temp1445= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, _temp1311); _LL1450: _temp1449= _temp1445.f1; goto _LL1448; _LL1448:
_temp1447= _temp1445.f2; goto _LL1446; _LL1446: { void* _temp1451= _temp1449;
struct Cyc_List_List* _temp1457; struct Cyc_Dict_Dict* _temp1459; _LL1453: if(
_temp1451 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1454;} else{ goto
_LL1455;} _LL1455: if(( unsigned int) _temp1451 >  1u?*(( int*) _temp1451) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL1460: _temp1459=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1451)->f1; goto _LL1458; _LL1458: _temp1457=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1451)->f2; goto _LL1456;} else{ goto _LL1452;} _LL1454: return({ struct
_tuple0 _temp1461; _temp1461.f1= _temp1449; _temp1461.f2= _temp1449; _temp1461;});
_LL1456: { void* _temp1462=( void*)(( struct Cyc_List_List*) _check_null(
_temp1447))->hd; void* _temp1463=( void*)(( struct Cyc_List_List*) _check_null(
_temp1447->tl))->hd; if( Cyc_CfFlowInfo_initlevel( _temp1459, _temp1462) == (
void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1464[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1311))->hd)->loc,
_tag_arr("expression may not be initialized", sizeof( unsigned char), 34u),
_tag_arr( _temp1464, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1459, _temp1463) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1465[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1311))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1465, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Exp* _temp1466=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1311))->hd; struct Cyc_Absyn_Exp* _temp1467=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1311->tl))->hd; void* _temp1468=( void*)
_temp1466->r; void* _temp1481; struct Cyc_Absyn_Vardecl* _temp1483; void*
_temp1486; struct Cyc_Absyn_Vardecl* _temp1488; void* _temp1491; struct Cyc_Absyn_Vardecl*
_temp1493; void* _temp1496; struct Cyc_Absyn_Vardecl* _temp1498; _LL1470: if(*((
int*) _temp1468) ==  Cyc_Absyn_Var_e){ _LL1482: _temp1481=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1468)->f2; if(( unsigned int) _temp1481 >  1u?*(( int*) _temp1481) ==  Cyc_Absyn_Global_b:
0){ _LL1484: _temp1483=(( struct Cyc_Absyn_Global_b_struct*) _temp1481)->f1;
goto _LL1480;} else{ goto _LL1472;}} else{ goto _LL1472;} _LL1480: if( !
_temp1483->escapes){ goto _LL1471;} else{ goto _LL1472;} _LL1472: if(*(( int*)
_temp1468) ==  Cyc_Absyn_Var_e){ _LL1487: _temp1486=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1468)->f2; if(( unsigned int) _temp1486 >  1u?*(( int*) _temp1486) ==  Cyc_Absyn_Local_b:
0){ _LL1489: _temp1488=(( struct Cyc_Absyn_Local_b_struct*) _temp1486)->f1; goto
_LL1485;} else{ goto _LL1474;}} else{ goto _LL1474;} _LL1485: if( ! _temp1488->escapes){
goto _LL1473;} else{ goto _LL1474;} _LL1474: if(*(( int*) _temp1468) ==  Cyc_Absyn_Var_e){
_LL1492: _temp1491=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1468)->f2;
if(( unsigned int) _temp1491 >  1u?*(( int*) _temp1491) ==  Cyc_Absyn_Pat_b: 0){
_LL1494: _temp1493=(( struct Cyc_Absyn_Pat_b_struct*) _temp1491)->f1; goto
_LL1490;} else{ goto _LL1476;}} else{ goto _LL1476;} _LL1490: if( ! _temp1493->escapes){
goto _LL1475;} else{ goto _LL1476;} _LL1476: if(*(( int*) _temp1468) ==  Cyc_Absyn_Var_e){
_LL1497: _temp1496=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1468)->f2;
if(( unsigned int) _temp1496 >  1u?*(( int*) _temp1496) ==  Cyc_Absyn_Param_b: 0){
_LL1499: _temp1498=(( struct Cyc_Absyn_Param_b_struct*) _temp1496)->f1; goto
_LL1495;} else{ goto _LL1478;}} else{ goto _LL1478;} _LL1495: if( ! _temp1498->escapes){
goto _LL1477;} else{ goto _LL1478;} _LL1478: goto _LL1479; _LL1471: _temp1488=
_temp1483; goto _LL1473; _LL1473: _temp1493= _temp1488; goto _LL1475; _LL1475:
_temp1498= _temp1493; goto _LL1477; _LL1477: { void* _temp1500=( void*)
_temp1467->r; void* _temp1517; struct Cyc_Absyn_Vardecl* _temp1519; void*
_temp1522; struct Cyc_Absyn_Vardecl* _temp1524; void* _temp1527; struct Cyc_Absyn_Vardecl*
_temp1529; void* _temp1532; struct Cyc_Absyn_Vardecl* _temp1534; void* _temp1536;
int _temp1538; struct Cyc_List_List* _temp1540; struct Cyc_List_List _temp1542;
struct Cyc_Absyn_Exp* _temp1543; void* _temp1545; _LL1502: if(*(( int*)
_temp1500) ==  Cyc_Absyn_Var_e){ _LL1518: _temp1517=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1500)->f2; if(( unsigned int) _temp1517 >  1u?*(( int*) _temp1517) ==  Cyc_Absyn_Global_b:
0){ _LL1520: _temp1519=(( struct Cyc_Absyn_Global_b_struct*) _temp1517)->f1;
goto _LL1516;} else{ goto _LL1504;}} else{ goto _LL1504;} _LL1516: if( !
_temp1519->escapes){ goto _LL1503;} else{ goto _LL1504;} _LL1504: if(*(( int*)
_temp1500) ==  Cyc_Absyn_Var_e){ _LL1523: _temp1522=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1500)->f2; if(( unsigned int) _temp1522 >  1u?*(( int*) _temp1522) ==  Cyc_Absyn_Local_b:
0){ _LL1525: _temp1524=(( struct Cyc_Absyn_Local_b_struct*) _temp1522)->f1; goto
_LL1521;} else{ goto _LL1506;}} else{ goto _LL1506;} _LL1521: if( ! _temp1524->escapes){
goto _LL1505;} else{ goto _LL1506;} _LL1506: if(*(( int*) _temp1500) ==  Cyc_Absyn_Var_e){
_LL1528: _temp1527=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1500)->f2;
if(( unsigned int) _temp1527 >  1u?*(( int*) _temp1527) ==  Cyc_Absyn_Pat_b: 0){
_LL1530: _temp1529=(( struct Cyc_Absyn_Pat_b_struct*) _temp1527)->f1; goto
_LL1526;} else{ goto _LL1508;}} else{ goto _LL1508;} _LL1526: if( ! _temp1529->escapes){
goto _LL1507;} else{ goto _LL1508;} _LL1508: if(*(( int*) _temp1500) ==  Cyc_Absyn_Var_e){
_LL1533: _temp1532=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1500)->f2;
if(( unsigned int) _temp1532 >  1u?*(( int*) _temp1532) ==  Cyc_Absyn_Param_b: 0){
_LL1535: _temp1534=(( struct Cyc_Absyn_Param_b_struct*) _temp1532)->f1; goto
_LL1531;} else{ goto _LL1510;}} else{ goto _LL1510;} _LL1531: if( ! _temp1534->escapes){
goto _LL1509;} else{ goto _LL1510;} _LL1510: if(*(( int*) _temp1500) ==  Cyc_Absyn_Const_e){
_LL1537: _temp1536=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1500)->f1;
if(( unsigned int) _temp1536 >  1u?*(( int*) _temp1536) ==  Cyc_Absyn_Int_c: 0){
_LL1539: _temp1538=(( struct Cyc_Absyn_Int_c_struct*) _temp1536)->f2; goto
_LL1511;} else{ goto _LL1512;}} else{ goto _LL1512;} _LL1512: if(*(( int*)
_temp1500) ==  Cyc_Absyn_Primop_e){ _LL1546: _temp1545=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1500)->f1; if( _temp1545 == ( void*) Cyc_Absyn_Size){ goto _LL1541;} else{
goto _LL1514;} _LL1541: _temp1540=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1500)->f2; if( _temp1540 ==  0){ goto _LL1514;} else{ _temp1542=* _temp1540;
_LL1544: _temp1543=( struct Cyc_Absyn_Exp*) _temp1542.hd; goto _LL1513;}} else{
goto _LL1514;} _LL1514: goto _LL1515; _LL1503: _temp1524= _temp1519; goto
_LL1505; _LL1505: _temp1529= _temp1524; goto _LL1507; _LL1507: _temp1534=
_temp1529; goto _LL1509; _LL1509:{ struct _tuple0 _temp1548=({ struct _tuple0
_temp1547; _temp1547.f1= Cyc_Tcutil_compress(( void*) _temp1498->type);
_temp1547.f2= Cyc_Tcutil_compress(( void*) _temp1534->type); _temp1547;}); void*
_temp1556; void* _temp1558; void* _temp1560; void* _temp1562; void* _temp1564;
void* _temp1566; _LL1550: _LL1557: _temp1556= _temp1548.f1; if(( unsigned int)
_temp1556 >  3u?*(( int*) _temp1556) ==  Cyc_Absyn_IntType: 0){ _LL1561:
_temp1560=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1556)->f1; if(
_temp1560 == ( void*) Cyc_Absyn_Unsigned){ goto _LL1559;} else{ goto _LL1552;}
_LL1559: _temp1558=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1556)->f2;
if( _temp1558 == ( void*) Cyc_Absyn_B4){ goto _LL1551;} else{ goto _LL1552;}}
else{ goto _LL1552;} _LL1552: _LL1563: _temp1562= _temp1548.f2; if((
unsigned int) _temp1562 >  3u?*(( int*) _temp1562) ==  Cyc_Absyn_IntType: 0){
_LL1567: _temp1566=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1562)->f1;
if( _temp1566 == ( void*) Cyc_Absyn_Unsigned){ goto _LL1565;} else{ goto _LL1554;}
_LL1565: _temp1564=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1562)->f2;
if( _temp1564 == ( void*) Cyc_Absyn_B4){ goto _LL1553;} else{ goto _LL1554;}}
else{ goto _LL1554;} _LL1554: goto _LL1555; _LL1551: goto _LL1549; _LL1553: goto
_LL1549; _LL1555: return({ struct _tuple0 _temp1568; _temp1568.f1= _temp1449;
_temp1568.f2= _temp1449; _temp1568;}); _LL1549:;}{ struct Cyc_List_List*
_temp1569=({ struct Cyc_List_List* _temp1573=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1573->hd=( void*)({ struct Cyc_CfFlowInfo_Reln*
_temp1574=( struct Cyc_CfFlowInfo_Reln*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln));
_temp1574->vd= _temp1498; _temp1574->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessVar_struct*
_temp1575=( struct Cyc_CfFlowInfo_LessVar_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_LessVar_struct));
_temp1575[ 0]=({ struct Cyc_CfFlowInfo_LessVar_struct _temp1576; _temp1576.tag=
Cyc_CfFlowInfo_LessVar; _temp1576.f1= _temp1534; _temp1576;}); _temp1575;}));
_temp1574;}); _temp1573->tl= _temp1457; _temp1573;}); return({ struct _tuple0
_temp1570; _temp1570.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp1571=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp1571[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp1572; _temp1572.tag= Cyc_CfFlowInfo_ReachableFL; _temp1572.f1= _temp1459;
_temp1572.f2= _temp1569; _temp1572;}); _temp1571;}); _temp1570.f2= _temp1449;
_temp1570;});} _LL1511:{ struct _tuple0 _temp1578=({ struct _tuple0 _temp1577;
_temp1577.f1= Cyc_Tcutil_compress(( void*) _temp1498->type); _temp1577.f2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1467->topt))->v); _temp1577;});
void* _temp1586; void* _temp1588; void* _temp1590; void* _temp1592; void*
_temp1594; void* _temp1596; _LL1580: _LL1587: _temp1586= _temp1578.f1; if((
unsigned int) _temp1586 >  3u?*(( int*) _temp1586) ==  Cyc_Absyn_IntType: 0){
_LL1591: _temp1590=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1586)->f1;
if( _temp1590 == ( void*) Cyc_Absyn_Unsigned){ goto _LL1589;} else{ goto _LL1582;}
_LL1589: _temp1588=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1586)->f2;
if( _temp1588 == ( void*) Cyc_Absyn_B4){ goto _LL1581;} else{ goto _LL1582;}}
else{ goto _LL1582;} _LL1582: _LL1593: _temp1592= _temp1578.f2; if((
unsigned int) _temp1592 >  3u?*(( int*) _temp1592) ==  Cyc_Absyn_IntType: 0){
_LL1597: _temp1596=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1592)->f1;
if( _temp1596 == ( void*) Cyc_Absyn_Unsigned){ goto _LL1595;} else{ goto _LL1584;}
_LL1595: _temp1594=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1592)->f2;
if( _temp1594 == ( void*) Cyc_Absyn_B4){ goto _LL1583;} else{ goto _LL1584;}}
else{ goto _LL1584;} _LL1584: goto _LL1585; _LL1581: goto _LL1579; _LL1583: goto
_LL1579; _LL1585: return({ struct _tuple0 _temp1598; _temp1598.f1= _temp1449;
_temp1598.f2= _temp1449; _temp1598;}); _LL1579:;}{ struct Cyc_List_List*
_temp1599=({ struct Cyc_List_List* _temp1603=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1603->hd=( void*)({ struct Cyc_CfFlowInfo_Reln*
_temp1604=( struct Cyc_CfFlowInfo_Reln*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln));
_temp1604->vd= _temp1498; _temp1604->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessConst_struct*
_temp1605=( struct Cyc_CfFlowInfo_LessConst_struct*) _cycalloc_atomic( sizeof(
struct Cyc_CfFlowInfo_LessConst_struct)); _temp1605[ 0]=({ struct Cyc_CfFlowInfo_LessConst_struct
_temp1606; _temp1606.tag= Cyc_CfFlowInfo_LessConst; _temp1606.f1=( unsigned int)
_temp1538; _temp1606;}); _temp1605;})); _temp1604;}); _temp1603->tl= _temp1457;
_temp1603;}); return({ struct _tuple0 _temp1600; _temp1600.f1=( void*)({ struct
Cyc_CfFlowInfo_ReachableFL_struct* _temp1601=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp1601[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp1602; _temp1602.tag= Cyc_CfFlowInfo_ReachableFL;
_temp1602.f1= _temp1459; _temp1602.f2= _temp1599; _temp1602;}); _temp1601;});
_temp1600.f2= _temp1449; _temp1600;});} _LL1513: { void* _temp1607=( void*)
_temp1543->r; void* _temp1620; struct Cyc_Absyn_Vardecl* _temp1622; void*
_temp1625; struct Cyc_Absyn_Vardecl* _temp1627; void* _temp1630; struct Cyc_Absyn_Vardecl*
_temp1632; void* _temp1635; struct Cyc_Absyn_Vardecl* _temp1637; _LL1609: if(*((
int*) _temp1607) ==  Cyc_Absyn_Var_e){ _LL1621: _temp1620=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1607)->f2; if(( unsigned int) _temp1620 >  1u?*(( int*) _temp1620) ==  Cyc_Absyn_Global_b:
0){ _LL1623: _temp1622=(( struct Cyc_Absyn_Global_b_struct*) _temp1620)->f1;
goto _LL1619;} else{ goto _LL1611;}} else{ goto _LL1611;} _LL1619: if( !
_temp1622->escapes){ goto _LL1610;} else{ goto _LL1611;} _LL1611: if(*(( int*)
_temp1607) ==  Cyc_Absyn_Var_e){ _LL1626: _temp1625=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1607)->f2; if(( unsigned int) _temp1625 >  1u?*(( int*) _temp1625) ==  Cyc_Absyn_Local_b:
0){ _LL1628: _temp1627=(( struct Cyc_Absyn_Local_b_struct*) _temp1625)->f1; goto
_LL1624;} else{ goto _LL1613;}} else{ goto _LL1613;} _LL1624: if( ! _temp1627->escapes){
goto _LL1612;} else{ goto _LL1613;} _LL1613: if(*(( int*) _temp1607) ==  Cyc_Absyn_Var_e){
_LL1631: _temp1630=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1607)->f2;
if(( unsigned int) _temp1630 >  1u?*(( int*) _temp1630) ==  Cyc_Absyn_Pat_b: 0){
_LL1633: _temp1632=(( struct Cyc_Absyn_Pat_b_struct*) _temp1630)->f1; goto
_LL1629;} else{ goto _LL1615;}} else{ goto _LL1615;} _LL1629: if( ! _temp1632->escapes){
goto _LL1614;} else{ goto _LL1615;} _LL1615: if(*(( int*) _temp1607) ==  Cyc_Absyn_Var_e){
_LL1636: _temp1635=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1607)->f2;
if(( unsigned int) _temp1635 >  1u?*(( int*) _temp1635) ==  Cyc_Absyn_Param_b: 0){
_LL1638: _temp1637=(( struct Cyc_Absyn_Param_b_struct*) _temp1635)->f1; goto
_LL1634;} else{ goto _LL1617;}} else{ goto _LL1617;} _LL1634: if( ! _temp1637->escapes){
goto _LL1616;} else{ goto _LL1617;} _LL1617: goto _LL1618; _LL1610: _temp1627=
_temp1622; goto _LL1612; _LL1612: _temp1632= _temp1627; goto _LL1614; _LL1614:
_temp1637= _temp1632; goto _LL1616; _LL1616: { struct Cyc_List_List* _temp1639=({
struct Cyc_List_List* _temp1643=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1643->hd=( void*)({ struct Cyc_CfFlowInfo_Reln*
_temp1644=( struct Cyc_CfFlowInfo_Reln*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln));
_temp1644->vd= _temp1498; _temp1644->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessSize_struct*
_temp1645=( struct Cyc_CfFlowInfo_LessSize_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_LessSize_struct)); _temp1645[ 0]=({ struct Cyc_CfFlowInfo_LessSize_struct
_temp1646; _temp1646.tag= Cyc_CfFlowInfo_LessSize; _temp1646.f1= _temp1637;
_temp1646;}); _temp1645;})); _temp1644;}); _temp1643->tl= _temp1457; _temp1643;});
return({ struct _tuple0 _temp1640; _temp1640.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp1641=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp1641[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp1642; _temp1642.tag= Cyc_CfFlowInfo_ReachableFL; _temp1642.f1= _temp1459;
_temp1642.f2= _temp1639; _temp1642;}); _temp1641;}); _temp1640.f2= _temp1449;
_temp1640;});} _LL1618: return({ struct _tuple0 _temp1647; _temp1647.f1=
_temp1449; _temp1647.f2= _temp1449; _temp1647;}); _LL1608:;} _LL1515: return({
struct _tuple0 _temp1648; _temp1648.f1= _temp1449; _temp1648.f2= _temp1449;
_temp1648;}); _LL1501:;} _LL1479: return({ struct _tuple0 _temp1649; _temp1649.f1=
_temp1449; _temp1649.f2= _temp1449; _temp1649;}); _LL1469:;}} _LL1452:;}}
_LL1281: { struct Cyc_List_List* _temp1652; void* _temp1654; struct _tuple4
_temp1650= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1315);
_LL1655: _temp1654= _temp1650.f1; goto _LL1653; _LL1653: _temp1652= _temp1650.f2;
goto _LL1651; _LL1651: { void* _temp1656= _temp1654; struct Cyc_List_List*
_temp1662; struct Cyc_Dict_Dict* _temp1664; _LL1658: if( _temp1656 == ( void*)
Cyc_CfFlowInfo_BottomFL){ goto _LL1659;} else{ goto _LL1660;} _LL1660: if((
unsigned int) _temp1656 >  1u?*(( int*) _temp1656) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL1665: _temp1664=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1656)->f1;
goto _LL1663; _LL1663: _temp1662=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1656)->f2; goto _LL1661;} else{ goto _LL1657;} _LL1659: return({ struct
_tuple0 _temp1666; _temp1666.f1= _temp1654; _temp1666.f2= _temp1654; _temp1666;});
_LL1661: { void* _temp1667=( void*)(( struct Cyc_List_List*) _check_null(
_temp1652))->hd; void* _temp1668=( void*)(( struct Cyc_List_List*) _check_null(
_temp1652->tl))->hd; if( Cyc_CfFlowInfo_initlevel( _temp1664, _temp1667) == (
void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1669[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1315))->hd)->loc,
_tag_arr("expression may not be initialized", sizeof( unsigned char), 34u),
_tag_arr( _temp1669, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1664, _temp1668) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1670[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1315))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1670, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Exp* _temp1671=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1315))->hd; struct Cyc_Absyn_Exp* _temp1672=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1315->tl))->hd; void* _temp1673=( void*)
_temp1671->r; void* _temp1686; struct Cyc_Absyn_Vardecl* _temp1688; void*
_temp1691; struct Cyc_Absyn_Vardecl* _temp1693; void* _temp1696; struct Cyc_Absyn_Vardecl*
_temp1698; void* _temp1701; struct Cyc_Absyn_Vardecl* _temp1703; _LL1675: if(*((
int*) _temp1673) ==  Cyc_Absyn_Var_e){ _LL1687: _temp1686=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1673)->f2; if(( unsigned int) _temp1686 >  1u?*(( int*) _temp1686) ==  Cyc_Absyn_Global_b:
0){ _LL1689: _temp1688=(( struct Cyc_Absyn_Global_b_struct*) _temp1686)->f1;
goto _LL1685;} else{ goto _LL1677;}} else{ goto _LL1677;} _LL1685: if( !
_temp1688->escapes){ goto _LL1676;} else{ goto _LL1677;} _LL1677: if(*(( int*)
_temp1673) ==  Cyc_Absyn_Var_e){ _LL1692: _temp1691=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1673)->f2; if(( unsigned int) _temp1691 >  1u?*(( int*) _temp1691) ==  Cyc_Absyn_Local_b:
0){ _LL1694: _temp1693=(( struct Cyc_Absyn_Local_b_struct*) _temp1691)->f1; goto
_LL1690;} else{ goto _LL1679;}} else{ goto _LL1679;} _LL1690: if( ! _temp1693->escapes){
goto _LL1678;} else{ goto _LL1679;} _LL1679: if(*(( int*) _temp1673) ==  Cyc_Absyn_Var_e){
_LL1697: _temp1696=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1673)->f2;
if(( unsigned int) _temp1696 >  1u?*(( int*) _temp1696) ==  Cyc_Absyn_Pat_b: 0){
_LL1699: _temp1698=(( struct Cyc_Absyn_Pat_b_struct*) _temp1696)->f1; goto
_LL1695;} else{ goto _LL1681;}} else{ goto _LL1681;} _LL1695: if( ! _temp1698->escapes){
goto _LL1680;} else{ goto _LL1681;} _LL1681: if(*(( int*) _temp1673) ==  Cyc_Absyn_Var_e){
_LL1702: _temp1701=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1673)->f2;
if(( unsigned int) _temp1701 >  1u?*(( int*) _temp1701) ==  Cyc_Absyn_Param_b: 0){
_LL1704: _temp1703=(( struct Cyc_Absyn_Param_b_struct*) _temp1701)->f1; goto
_LL1700;} else{ goto _LL1683;}} else{ goto _LL1683;} _LL1700: if( ! _temp1703->escapes){
goto _LL1682;} else{ goto _LL1683;} _LL1683: goto _LL1684; _LL1676: _temp1693=
_temp1688; goto _LL1678; _LL1678: _temp1698= _temp1693; goto _LL1680; _LL1680:
_temp1703= _temp1698; goto _LL1682; _LL1682: { void* _temp1705=( void*)
_temp1672->r; struct Cyc_List_List* _temp1711; struct Cyc_List_List _temp1713;
struct Cyc_Absyn_Exp* _temp1714; void* _temp1716; _LL1707: if(*(( int*)
_temp1705) ==  Cyc_Absyn_Primop_e){ _LL1717: _temp1716=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1705)->f1; if( _temp1716 == ( void*) Cyc_Absyn_Size){ goto _LL1712;} else{
goto _LL1709;} _LL1712: _temp1711=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1705)->f2; if( _temp1711 ==  0){ goto _LL1709;} else{ _temp1713=* _temp1711;
_LL1715: _temp1714=( struct Cyc_Absyn_Exp*) _temp1713.hd; goto _LL1708;}} else{
goto _LL1709;} _LL1709: goto _LL1710; _LL1708: { void* _temp1718=( void*)
_temp1714->r; void* _temp1731; struct Cyc_Absyn_Vardecl* _temp1733; void*
_temp1736; struct Cyc_Absyn_Vardecl* _temp1738; void* _temp1741; struct Cyc_Absyn_Vardecl*
_temp1743; void* _temp1746; struct Cyc_Absyn_Vardecl* _temp1748; _LL1720: if(*((
int*) _temp1718) ==  Cyc_Absyn_Var_e){ _LL1732: _temp1731=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1718)->f2; if(( unsigned int) _temp1731 >  1u?*(( int*) _temp1731) ==  Cyc_Absyn_Global_b:
0){ _LL1734: _temp1733=(( struct Cyc_Absyn_Global_b_struct*) _temp1731)->f1;
goto _LL1730;} else{ goto _LL1722;}} else{ goto _LL1722;} _LL1730: if( !
_temp1733->escapes){ goto _LL1721;} else{ goto _LL1722;} _LL1722: if(*(( int*)
_temp1718) ==  Cyc_Absyn_Var_e){ _LL1737: _temp1736=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1718)->f2; if(( unsigned int) _temp1736 >  1u?*(( int*) _temp1736) ==  Cyc_Absyn_Local_b:
0){ _LL1739: _temp1738=(( struct Cyc_Absyn_Local_b_struct*) _temp1736)->f1; goto
_LL1735;} else{ goto _LL1724;}} else{ goto _LL1724;} _LL1735: if( ! _temp1738->escapes){
goto _LL1723;} else{ goto _LL1724;} _LL1724: if(*(( int*) _temp1718) ==  Cyc_Absyn_Var_e){
_LL1742: _temp1741=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1718)->f2;
if(( unsigned int) _temp1741 >  1u?*(( int*) _temp1741) ==  Cyc_Absyn_Pat_b: 0){
_LL1744: _temp1743=(( struct Cyc_Absyn_Pat_b_struct*) _temp1741)->f1; goto
_LL1740;} else{ goto _LL1726;}} else{ goto _LL1726;} _LL1740: if( ! _temp1743->escapes){
goto _LL1725;} else{ goto _LL1726;} _LL1726: if(*(( int*) _temp1718) ==  Cyc_Absyn_Var_e){
_LL1747: _temp1746=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1718)->f2;
if(( unsigned int) _temp1746 >  1u?*(( int*) _temp1746) ==  Cyc_Absyn_Param_b: 0){
_LL1749: _temp1748=(( struct Cyc_Absyn_Param_b_struct*) _temp1746)->f1; goto
_LL1745;} else{ goto _LL1728;}} else{ goto _LL1728;} _LL1745: if( ! _temp1748->escapes){
goto _LL1727;} else{ goto _LL1728;} _LL1728: goto _LL1729; _LL1721: _temp1738=
_temp1733; goto _LL1723; _LL1723: _temp1743= _temp1738; goto _LL1725; _LL1725:
_temp1748= _temp1743; goto _LL1727; _LL1727: { struct Cyc_List_List* _temp1750=({
struct Cyc_List_List* _temp1754=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1754->hd=( void*)({ struct Cyc_CfFlowInfo_Reln*
_temp1755=( struct Cyc_CfFlowInfo_Reln*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln));
_temp1755->vd= _temp1703; _temp1755->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessEqSize_struct*
_temp1756=( struct Cyc_CfFlowInfo_LessEqSize_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_LessEqSize_struct)); _temp1756[ 0]=({ struct Cyc_CfFlowInfo_LessEqSize_struct
_temp1757; _temp1757.tag= Cyc_CfFlowInfo_LessEqSize; _temp1757.f1= _temp1748;
_temp1757;}); _temp1756;})); _temp1755;}); _temp1754->tl= _temp1662; _temp1754;});
return({ struct _tuple0 _temp1751; _temp1751.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp1752=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp1752[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp1753; _temp1753.tag= Cyc_CfFlowInfo_ReachableFL; _temp1753.f1= _temp1664;
_temp1753.f2= _temp1750; _temp1753;}); _temp1752;}); _temp1751.f2= _temp1654;
_temp1751;});} _LL1729: return({ struct _tuple0 _temp1758; _temp1758.f1=
_temp1654; _temp1758.f2= _temp1654; _temp1758;}); _LL1719:;} _LL1710: return({
struct _tuple0 _temp1759; _temp1759.f1= _temp1654; _temp1759.f2= _temp1654;
_temp1759;}); _LL1706:;} _LL1684: return({ struct _tuple0 _temp1760; _temp1760.f1=
_temp1654; _temp1760.f2= _temp1654; _temp1760;}); _LL1674:;}} _LL1657:;}}
_LL1283: goto _LL1267; _LL1267:;}{ void* _temp1763; void* _temp1765; struct
_tuple0 _temp1761= Cyc_NewControlFlow_anal_Rexp( env, inflow, e); _LL1766:
_temp1765= _temp1761.f1; goto _LL1764; _LL1764: _temp1763= _temp1761.f2; goto
_LL1762; _LL1762: { void* _temp1767= _temp1765; struct Cyc_List_List* _temp1773;
struct Cyc_Dict_Dict* _temp1775; _LL1769: if( _temp1767 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1770;} else{ goto _LL1771;} _LL1771: if(( unsigned int) _temp1767 >  1u?*((
int*) _temp1767) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1776: _temp1775=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1767)->f1; goto _LL1774; _LL1774:
_temp1773=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1767)->f2; goto
_LL1772;} else{ goto _LL1768;} _LL1770: return({ struct _tuple0 _temp1777;
_temp1777.f1= _temp1765; _temp1777.f2= _temp1765; _temp1777;}); _LL1772: { void*
_temp1778= _temp1763; void* _temp1798; void* _temp1800; void* _temp1802; _LL1780:
if( _temp1778 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1781;} else{ goto
_LL1782;} _LL1782: if( _temp1778 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto
_LL1783;} else{ goto _LL1784;} _LL1784: if( _temp1778 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL1785;} else{ goto _LL1786;} _LL1786: if(( unsigned int) _temp1778 >  3u?*((
int*) _temp1778) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL1787;} else{ goto
_LL1788;} _LL1788: if(( unsigned int) _temp1778 >  3u?*(( int*) _temp1778) == 
Cyc_CfFlowInfo_UnknownR: 0){ _LL1799: _temp1798=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1778)->f1; if( _temp1798 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL1789;}
else{ goto _LL1790;}} else{ goto _LL1790;} _LL1790: if(( unsigned int) _temp1778
>  3u?*(( int*) _temp1778) ==  Cyc_CfFlowInfo_Esc: 0){ _LL1801: _temp1800=( void*)((
struct Cyc_CfFlowInfo_Esc_struct*) _temp1778)->f1; if( _temp1800 == ( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL1791;} else{ goto _LL1792;}} else{ goto _LL1792;} _LL1792: if((
unsigned int) _temp1778 >  3u?*(( int*) _temp1778) ==  Cyc_CfFlowInfo_UnknownR:
0){ _LL1803: _temp1802=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1778)->f1; goto _LL1793;} else{ goto _LL1794;} _LL1794: if(( unsigned int)
_temp1778 >  3u?*(( int*) _temp1778) ==  Cyc_CfFlowInfo_Esc: 0){ goto _LL1795;}
else{ goto _LL1796;} _LL1796: if(( unsigned int) _temp1778 >  3u?*(( int*)
_temp1778) ==  Cyc_CfFlowInfo_Aggregate: 0){ goto _LL1797;} else{ goto _LL1779;}
_LL1781: return({ struct _tuple0 _temp1804; _temp1804.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1804.f2= _temp1765; _temp1804;}); _LL1783: goto _LL1785; _LL1785: goto
_LL1787; _LL1787: return({ struct _tuple0 _temp1805; _temp1805.f1= _temp1765;
_temp1805.f2=( void*) Cyc_CfFlowInfo_BottomFL; _temp1805;}); _LL1789: goto
_LL1791; _LL1791:({ void* _temp1806[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1806, sizeof( void*), 0u));});
return({ struct _tuple0 _temp1807; _temp1807.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1807.f2=( void*) Cyc_CfFlowInfo_BottomFL; _temp1807;}); _LL1793: return Cyc_NewControlFlow_splitzero(
env, inflow, _temp1765, e, _temp1802); _LL1795: return({ struct _tuple0
_temp1808; _temp1808.f1= _temp1765; _temp1808.f2= _temp1765; _temp1808;});
_LL1797:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1809=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1809[ 0]=({ struct Cyc_Core_Impossible_struct _temp1810; _temp1810.tag= Cyc_Core_Impossible;
_temp1810.f1= _tag_arr("anal_test", sizeof( unsigned char), 10u); _temp1810;});
_temp1809;})); _LL1779:;} _LL1768:;}}} static void* Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_List_List* scs){
for( 0; scs !=  0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause _temp1813;
struct Cyc_Absyn_Stmt* _temp1814; struct Cyc_Absyn_Exp* _temp1816; struct Cyc_Core_Opt*
_temp1818; struct Cyc_Absyn_Switch_clause* _temp1811=( struct Cyc_Absyn_Switch_clause*)
scs->hd; _temp1813=* _temp1811; _LL1819: _temp1818= _temp1813.pat_vars; goto
_LL1817; _LL1817: _temp1816= _temp1813.where_clause; goto _LL1815; _LL1815:
_temp1814= _temp1813.body; goto _LL1812; _LL1812: { void* clause_inflow= Cyc_NewControlFlow_add_vars(
inflow,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1818))->v,
Cyc_CfFlowInfo_unknown_all); void* clause_outflow; if( _temp1816 !=  0){ void*
_temp1822; void* _temp1824; struct _tuple0 _temp1820= Cyc_NewControlFlow_anal_test(
env, clause_inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp1816)); _LL1825:
_temp1824= _temp1820.f1; goto _LL1823; _LL1823: _temp1822= _temp1820.f2; goto
_LL1821; _LL1821: inflow= _temp1822; clause_outflow= Cyc_NewControlFlow_anal_stmt(
env, _temp1824, _temp1814);} else{ clause_outflow= Cyc_NewControlFlow_anal_stmt(
env, clause_inflow, _temp1814);}{ void* _temp1826= clause_outflow; _LL1828: if(
_temp1826 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1829;} else{ goto
_LL1830;} _LL1830: goto _LL1831; _LL1829: goto _LL1827; _LL1831: if( scs->tl == 
0){ return clause_outflow;} else{ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs->tl))->hd)->pat_vars))->v !=  0){({ void* _temp1832[ 0u]={};
Cyc_Tcutil_terr( _temp1814->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp1832, sizeof( void*), 0u));});}
else{({ void* _temp1833[ 0u]={}; Cyc_Tcutil_warn( _temp1814->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp1833, sizeof( void*), 0u));});} Cyc_NewControlFlow_update_flow(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs->tl))->hd)->body, clause_outflow);} goto _LL1827; _LL1827:;}}} return( void*)
Cyc_CfFlowInfo_BottomFL;} static void* Cyc_NewControlFlow_anal_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, void* inflow, struct Cyc_Absyn_Stmt* s){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1834= Cyc_NewControlFlow_pre_stmt_check( env, inflow, s); inflow=( void*)
_temp1834->flow;{ void* _temp1835=( void*) s->r; struct Cyc_Absyn_Exp* _temp1877;
struct Cyc_Absyn_Exp* _temp1879; struct Cyc_Absyn_Exp* _temp1881; struct Cyc_Absyn_Stmt*
_temp1883; struct Cyc_Absyn_Stmt* _temp1885; struct Cyc_Absyn_Stmt* _temp1887;
struct Cyc_Absyn_Stmt* _temp1889; struct Cyc_Absyn_Exp* _temp1891; struct Cyc_Absyn_Stmt*
_temp1893; struct _tuple3 _temp1895; struct Cyc_Absyn_Stmt* _temp1897; struct
Cyc_Absyn_Exp* _temp1899; struct _tuple3 _temp1901; struct Cyc_Absyn_Stmt*
_temp1903; struct Cyc_Absyn_Exp* _temp1905; struct Cyc_Absyn_Stmt* _temp1907;
struct Cyc_Absyn_Stmt* _temp1909; struct _tuple3 _temp1911; struct Cyc_Absyn_Stmt*
_temp1913; struct Cyc_Absyn_Exp* _temp1915; struct _tuple3 _temp1917; struct Cyc_Absyn_Stmt*
_temp1919; struct Cyc_Absyn_Exp* _temp1921; struct Cyc_Absyn_Exp* _temp1923;
struct Cyc_Absyn_Stmt* _temp1925; struct Cyc_Absyn_Switch_clause** _temp1927;
struct Cyc_Absyn_Switch_clause* _temp1929; struct Cyc_List_List* _temp1930;
struct Cyc_Absyn_Stmt* _temp1932; struct Cyc_Absyn_Stmt* _temp1934; struct Cyc_Absyn_Stmt*
_temp1936; struct Cyc_List_List* _temp1938; struct Cyc_Absyn_Exp* _temp1940;
struct Cyc_List_List* _temp1942; struct Cyc_Absyn_Stmt* _temp1944; struct Cyc_Absyn_Stmt*
_temp1946; struct Cyc_Absyn_Decl* _temp1948; struct Cyc_Absyn_Stmt* _temp1950;
struct Cyc_Absyn_Stmt* _temp1952; struct Cyc_Absyn_Vardecl* _temp1954; struct
Cyc_Absyn_Tvar* _temp1956; _LL1837: if( _temp1835 == ( void*) Cyc_Absyn_Skip_s){
goto _LL1838;} else{ goto _LL1839;} _LL1839: if(( unsigned int) _temp1835 >  1u?*((
int*) _temp1835) ==  Cyc_Absyn_Return_s: 0){ _LL1878: _temp1877=(( struct Cyc_Absyn_Return_s_struct*)
_temp1835)->f1; if( _temp1877 ==  0){ goto _LL1840;} else{ goto _LL1841;}} else{
goto _LL1841;} _LL1841: if(( unsigned int) _temp1835 >  1u?*(( int*) _temp1835)
==  Cyc_Absyn_Return_s: 0){ _LL1880: _temp1879=(( struct Cyc_Absyn_Return_s_struct*)
_temp1835)->f1; goto _LL1842;} else{ goto _LL1843;} _LL1843: if(( unsigned int)
_temp1835 >  1u?*(( int*) _temp1835) ==  Cyc_Absyn_Exp_s: 0){ _LL1882: _temp1881=((
struct Cyc_Absyn_Exp_s_struct*) _temp1835)->f1; goto _LL1844;} else{ goto
_LL1845;} _LL1845: if(( unsigned int) _temp1835 >  1u?*(( int*) _temp1835) == 
Cyc_Absyn_Seq_s: 0){ _LL1886: _temp1885=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1835)->f1; goto _LL1884; _LL1884: _temp1883=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1835)->f2; goto _LL1846;} else{ goto _LL1847;} _LL1847: if(( unsigned int)
_temp1835 >  1u?*(( int*) _temp1835) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1892:
_temp1891=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1835)->f1; goto _LL1890;
_LL1890: _temp1889=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1835)->f2;
goto _LL1888; _LL1888: _temp1887=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1835)->f3; goto _LL1848;} else{ goto _LL1849;} _LL1849: if(( unsigned int)
_temp1835 >  1u?*(( int*) _temp1835) ==  Cyc_Absyn_While_s: 0){ _LL1896:
_temp1895=(( struct Cyc_Absyn_While_s_struct*) _temp1835)->f1; _LL1900:
_temp1899= _temp1895.f1; goto _LL1898; _LL1898: _temp1897= _temp1895.f2; goto
_LL1894; _LL1894: _temp1893=(( struct Cyc_Absyn_While_s_struct*) _temp1835)->f2;
goto _LL1850;} else{ goto _LL1851;} _LL1851: if(( unsigned int) _temp1835 >  1u?*((
int*) _temp1835) ==  Cyc_Absyn_Do_s: 0){ _LL1908: _temp1907=(( struct Cyc_Absyn_Do_s_struct*)
_temp1835)->f1; goto _LL1902; _LL1902: _temp1901=(( struct Cyc_Absyn_Do_s_struct*)
_temp1835)->f2; _LL1906: _temp1905= _temp1901.f1; goto _LL1904; _LL1904:
_temp1903= _temp1901.f2; goto _LL1852;} else{ goto _LL1853;} _LL1853: if((
unsigned int) _temp1835 >  1u?*(( int*) _temp1835) ==  Cyc_Absyn_For_s: 0){
_LL1924: _temp1923=(( struct Cyc_Absyn_For_s_struct*) _temp1835)->f1; goto
_LL1918; _LL1918: _temp1917=(( struct Cyc_Absyn_For_s_struct*) _temp1835)->f2;
_LL1922: _temp1921= _temp1917.f1; goto _LL1920; _LL1920: _temp1919= _temp1917.f2;
goto _LL1912; _LL1912: _temp1911=(( struct Cyc_Absyn_For_s_struct*) _temp1835)->f3;
_LL1916: _temp1915= _temp1911.f1; goto _LL1914; _LL1914: _temp1913= _temp1911.f2;
goto _LL1910; _LL1910: _temp1909=(( struct Cyc_Absyn_For_s_struct*) _temp1835)->f4;
goto _LL1854;} else{ goto _LL1855;} _LL1855: if(( unsigned int) _temp1835 >  1u?*((
int*) _temp1835) ==  Cyc_Absyn_Break_s: 0){ _LL1926: _temp1925=(( struct Cyc_Absyn_Break_s_struct*)
_temp1835)->f1; if( _temp1925 ==  0){ goto _LL1856;} else{ goto _LL1857;}} else{
goto _LL1857;} _LL1857: if(( unsigned int) _temp1835 >  1u?*(( int*) _temp1835)
==  Cyc_Absyn_Fallthru_s: 0){ _LL1931: _temp1930=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1835)->f1; goto _LL1928; _LL1928: _temp1927=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1835)->f2; if( _temp1927 ==  0){ goto _LL1859;} else{ _temp1929=* _temp1927;
goto _LL1858;}} else{ goto _LL1859;} _LL1859: if(( unsigned int) _temp1835 >  1u?*((
int*) _temp1835) ==  Cyc_Absyn_Break_s: 0){ _LL1933: _temp1932=(( struct Cyc_Absyn_Break_s_struct*)
_temp1835)->f1; goto _LL1860;} else{ goto _LL1861;} _LL1861: if(( unsigned int)
_temp1835 >  1u?*(( int*) _temp1835) ==  Cyc_Absyn_Continue_s: 0){ _LL1935:
_temp1934=(( struct Cyc_Absyn_Continue_s_struct*) _temp1835)->f1; goto _LL1862;}
else{ goto _LL1863;} _LL1863: if(( unsigned int) _temp1835 >  1u?*(( int*)
_temp1835) ==  Cyc_Absyn_Goto_s: 0){ _LL1937: _temp1936=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1835)->f2; goto _LL1864;} else{ goto _LL1865;} _LL1865: if(( unsigned int)
_temp1835 >  1u?*(( int*) _temp1835) ==  Cyc_Absyn_Switch_s: 0){ _LL1941:
_temp1940=(( struct Cyc_Absyn_Switch_s_struct*) _temp1835)->f1; goto _LL1939;
_LL1939: _temp1938=(( struct Cyc_Absyn_Switch_s_struct*) _temp1835)->f2; goto
_LL1866;} else{ goto _LL1867;} _LL1867: if(( unsigned int) _temp1835 >  1u?*((
int*) _temp1835) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1945: _temp1944=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1835)->f1; goto _LL1943; _LL1943: _temp1942=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1835)->f2; goto _LL1868;} else{ goto _LL1869;} _LL1869: if(( unsigned int)
_temp1835 >  1u?*(( int*) _temp1835) ==  Cyc_Absyn_Decl_s: 0){ _LL1949:
_temp1948=(( struct Cyc_Absyn_Decl_s_struct*) _temp1835)->f1; goto _LL1947;
_LL1947: _temp1946=(( struct Cyc_Absyn_Decl_s_struct*) _temp1835)->f2; goto
_LL1870;} else{ goto _LL1871;} _LL1871: if(( unsigned int) _temp1835 >  1u?*((
int*) _temp1835) ==  Cyc_Absyn_Label_s: 0){ _LL1951: _temp1950=(( struct Cyc_Absyn_Label_s_struct*)
_temp1835)->f2; goto _LL1872;} else{ goto _LL1873;} _LL1873: if(( unsigned int)
_temp1835 >  1u?*(( int*) _temp1835) ==  Cyc_Absyn_Region_s: 0){ _LL1957:
_temp1956=(( struct Cyc_Absyn_Region_s_struct*) _temp1835)->f1; goto _LL1955;
_LL1955: _temp1954=(( struct Cyc_Absyn_Region_s_struct*) _temp1835)->f2; goto
_LL1953; _LL1953: _temp1952=(( struct Cyc_Absyn_Region_s_struct*) _temp1835)->f3;
goto _LL1874;} else{ goto _LL1875;} _LL1875: goto _LL1876; _LL1838: return
inflow; _LL1840: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1842: { void*
_temp1960; void* _temp1962; struct _tuple0 _temp1958= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp1879)); _LL1963:
_temp1962= _temp1958.f1; goto _LL1961; _LL1961: _temp1960= _temp1958.f2; goto
_LL1959; _LL1959: Cyc_NewControlFlow_use_Rval( env,(( struct Cyc_Absyn_Exp*)
_check_null( _temp1879))->loc, _temp1962, _temp1960); return( void*) Cyc_CfFlowInfo_BottomFL;}
_LL1844: return( Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp1881)).f1;
_LL1846: return Cyc_NewControlFlow_anal_stmt( env, Cyc_NewControlFlow_anal_stmt(
env, inflow, _temp1885), _temp1883); _LL1848: { void* _temp1966; void* _temp1968;
struct _tuple0 _temp1964= Cyc_NewControlFlow_anal_test( env, inflow, _temp1891);
_LL1969: _temp1968= _temp1964.f1; goto _LL1967; _LL1967: _temp1966= _temp1964.f2;
goto _LL1965; _LL1965: return Cyc_CfFlowInfo_join_flow( env->all_changed, Cyc_NewControlFlow_anal_stmt(
env, _temp1968, _temp1889), Cyc_NewControlFlow_anal_stmt( env, _temp1966,
_temp1887));} _LL1850: { void* _temp1970=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, inflow, _temp1897))->flow; void* _temp1973; void* _temp1975; struct _tuple0
_temp1971= Cyc_NewControlFlow_anal_test( env, _temp1970, _temp1899); _LL1976:
_temp1975= _temp1971.f1; goto _LL1974; _LL1974: _temp1973= _temp1971.f2; goto
_LL1972; _LL1972: { void* _temp1977= Cyc_NewControlFlow_anal_stmt( env,
_temp1975, _temp1893); Cyc_NewControlFlow_update_flow( env, _temp1897, _temp1977);
return _temp1973;}} _LL1852: { void* _temp1978= Cyc_NewControlFlow_anal_stmt(
env, inflow, _temp1907); void* _temp1979=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1978, _temp1903))->flow; void* _temp1982; void* _temp1984; struct
_tuple0 _temp1980= Cyc_NewControlFlow_anal_test( env, _temp1979, _temp1905);
_LL1985: _temp1984= _temp1980.f1; goto _LL1983; _LL1983: _temp1982= _temp1980.f2;
goto _LL1981; _LL1981: Cyc_NewControlFlow_update_flow( env, _temp1907, _temp1984);
return _temp1982;} _LL1854: { void* _temp1986=( Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1923)).f1; void* _temp1987=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1986, _temp1919))->flow; void* _temp1990; void* _temp1992; struct
_tuple0 _temp1988= Cyc_NewControlFlow_anal_test( env, _temp1987, _temp1921);
_LL1993: _temp1992= _temp1988.f1; goto _LL1991; _LL1991: _temp1990= _temp1988.f2;
goto _LL1989; _LL1989: { void* _temp1994= Cyc_NewControlFlow_anal_stmt( env,
_temp1992, _temp1909); void* _temp1995=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1994, _temp1913))->flow; void* _temp1996=( Cyc_NewControlFlow_anal_Rexp(
env, _temp1995, _temp1915)).f1; Cyc_NewControlFlow_update_flow( env, _temp1919,
_temp1996); return _temp1990;}} _LL1856: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1858: { struct Cyc_List_List* _temp1999; void* _temp2001; struct _tuple4
_temp1997= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1930);
_LL2002: _temp2001= _temp1997.f1; goto _LL2000; _LL2000: _temp1999= _temp1997.f2;
goto _LL1998; _LL1998: for( 0; _temp1999 !=  0;( _temp1999= _temp1999->tl,
_temp1930= _temp1930->tl)){ _temp2001= Cyc_NewControlFlow_use_Rval( env,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1930))->hd)->loc,
_temp2001,( void*) _temp1999->hd);} _temp2001= Cyc_NewControlFlow_add_vars(
_temp2001,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1929->pat_vars))->v, Cyc_CfFlowInfo_unknown_all); Cyc_NewControlFlow_update_flow(
env,( struct Cyc_Absyn_Stmt*) _temp1929->body, _temp2001); return( void*) Cyc_CfFlowInfo_BottomFL;}
_LL1860: _temp1934= _temp1932; goto _LL1862; _LL1862: _temp1936= _temp1934; goto
_LL1864; _LL1864: if( env->iteration_num ==  1){ struct Cyc_Absyn_Stmt*
_temp2003= _temp1834->encloser; struct Cyc_Absyn_Stmt* _temp2004=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*) _check_null( _temp1936)))->encloser; while( _temp2004 != 
_temp2003) { struct Cyc_Absyn_Stmt* _temp2005=( Cyc_NewControlFlow_get_stmt_annot(
_temp2003))->encloser; if( _temp2005 ==  _temp2003){({ void* _temp2006[ 0u]={};
Cyc_Tcutil_terr( s->loc, _tag_arr("goto enters local scope or exception handler",
sizeof( unsigned char), 45u), _tag_arr( _temp2006, sizeof( void*), 0u));});
break;} _temp2003= _temp2005;}} Cyc_NewControlFlow_update_flow( env,( struct Cyc_Absyn_Stmt*)
_check_null( _temp1936), inflow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1866: { void* _temp2009; void* _temp2011; struct _tuple0 _temp2007= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1940); _LL2012: _temp2011= _temp2007.f1; goto _LL2010; _LL2010:
_temp2009= _temp2007.f2; goto _LL2008; _LL2008: _temp2011= Cyc_NewControlFlow_use_Rval(
env, _temp1940->loc, _temp2011, _temp2009); return Cyc_NewControlFlow_anal_scs(
env, _temp2011, _temp1938);} _LL1868: { int old_in_try= env->in_try; void*
old_tryflow=( void*) env->tryflow; env->in_try= 1;( void*)( env->tryflow=( void*)
inflow);{ void* s1_outflow= Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1944);
void* scs_inflow=( void*) env->tryflow; env->in_try= old_in_try;( void*)( env->tryflow=(
void*) old_tryflow); Cyc_NewControlFlow_update_tryflow( env, scs_inflow);{ void*
scs_outflow= Cyc_NewControlFlow_anal_scs( env, scs_inflow, _temp1942);{ void*
_temp2013= scs_outflow; _LL2015: if( _temp2013 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL2016;} else{ goto _LL2017;} _LL2017: goto _LL2018; _LL2016: goto _LL2014;
_LL2018:({ void* _temp2019[ 0u]={}; Cyc_Tcutil_terr( s->loc, _tag_arr("last catch clause may implicitly fallthru",
sizeof( unsigned char), 42u), _tag_arr( _temp2019, sizeof( void*), 0u));});
_LL2014:;} return s1_outflow;}}} _LL1870: return Cyc_NewControlFlow_anal_stmt(
env, Cyc_NewControlFlow_anal_decl( env, inflow, _temp1948), _temp1946); _LL1872:
return Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1950); _LL1874: { struct
Cyc_List_List _temp2020=({ struct Cyc_List_List _temp2021; _temp2021.hd=( void*)
_temp1954; _temp2021.tl= 0; _temp2021;}); inflow= Cyc_NewControlFlow_add_vars(
inflow,( struct Cyc_List_List*)& _temp2020, Cyc_CfFlowInfo_unknown_all); return
Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1952);} _LL1876:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp2022=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp2022[ 0]=({ struct
Cyc_Core_Impossible_struct _temp2023; _temp2023.tag= Cyc_Core_Impossible;
_temp2023.f1= _tag_arr("anal_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof( unsigned char), 56u); _temp2023;}); _temp2022;})); _LL1836:;}} static
void* Cyc_NewControlFlow_anal_decl( struct Cyc_NewControlFlow_AnalEnv* env, void*
inflow, struct Cyc_Absyn_Decl* decl){ void* _temp2024=( void*) decl->r; struct
Cyc_Absyn_Vardecl* _temp2034; struct Cyc_Absyn_Exp* _temp2036; struct Cyc_Core_Opt*
_temp2038; struct Cyc_Core_Opt _temp2040; struct Cyc_List_List* _temp2041;
struct Cyc_List_List* _temp2043; _LL2026: if(*(( int*) _temp2024) ==  Cyc_Absyn_Var_d){
_LL2035: _temp2034=(( struct Cyc_Absyn_Var_d_struct*) _temp2024)->f1; goto
_LL2027;} else{ goto _LL2028;} _LL2028: if(*(( int*) _temp2024) ==  Cyc_Absyn_Let_d){
_LL2039: _temp2038=(( struct Cyc_Absyn_Let_d_struct*) _temp2024)->f2; if(
_temp2038 ==  0){ goto _LL2030;} else{ _temp2040=* _temp2038; _LL2042: _temp2041=(
struct Cyc_List_List*) _temp2040.v; goto _LL2037;} _LL2037: _temp2036=(( struct
Cyc_Absyn_Let_d_struct*) _temp2024)->f4; goto _LL2029;} else{ goto _LL2030;}
_LL2030: if(*(( int*) _temp2024) ==  Cyc_Absyn_Letv_d){ _LL2044: _temp2043=((
struct Cyc_Absyn_Letv_d_struct*) _temp2024)->f1; goto _LL2031;} else{ goto
_LL2032;} _LL2032: goto _LL2033; _LL2027: { struct Cyc_List_List _temp2045=({
struct Cyc_List_List _temp2069; _temp2069.hd=( void*) _temp2034; _temp2069.tl= 0;
_temp2069;}); inflow= Cyc_NewControlFlow_add_vars( inflow,( struct Cyc_List_List*)&
_temp2045, Cyc_CfFlowInfo_unknown_none);{ struct Cyc_Absyn_Exp* _temp2046=
_temp2034->initializer; if( _temp2046 ==  0){ return inflow;}{ void* _temp2049;
void* _temp2051; struct _tuple0 _temp2047= Cyc_NewControlFlow_anal_Rexp( env,
inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp2046)); _LL2052: _temp2051=
_temp2047.f1; goto _LL2050; _LL2050: _temp2049= _temp2047.f2; goto _LL2048;
_LL2048: { void* _temp2053= _temp2051; struct Cyc_List_List* _temp2059; struct
Cyc_Dict_Dict* _temp2061; _LL2055: if( _temp2053 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL2056;} else{ goto _LL2057;} _LL2057: if(( unsigned int) _temp2053 >  1u?*((
int*) _temp2053) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL2062: _temp2061=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp2053)->f1; goto _LL2060; _LL2060:
_temp2059=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp2053)->f2; goto
_LL2058;} else{ goto _LL2054;} _LL2056: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL2058: _temp2061= Cyc_CfFlowInfo_assign_place( decl->loc, _temp2061, env->all_changed,({
struct Cyc_CfFlowInfo_Place* _temp2063=( struct Cyc_CfFlowInfo_Place*) _cycalloc(
sizeof( struct Cyc_CfFlowInfo_Place)); _temp2063->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp2064=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp2064[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp2065; _temp2065.tag= Cyc_CfFlowInfo_VarRoot;
_temp2065.f1= _temp2034; _temp2065;}); _temp2064;})); _temp2063->fields= 0;
_temp2063;}), _temp2049); _temp2059= Cyc_CfFlowInfo_reln_assign_var( _temp2059,
_temp2034,( struct Cyc_Absyn_Exp*) _check_null( _temp2034->initializer));{
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp2066=({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp2067=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp2067[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp2068; _temp2068.tag= Cyc_CfFlowInfo_ReachableFL; _temp2068.f1= _temp2061;
_temp2068.f2= _temp2059; _temp2068;}); _temp2067;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp2066); return( void*) _temp2066;} _LL2054:;}}}} _LL2029: {
void* _temp2072; void* _temp2074; struct _tuple0 _temp2070= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp2036); _LL2075: _temp2074= _temp2070.f1; goto _LL2073; _LL2073:
_temp2072= _temp2070.f2; goto _LL2071; _LL2071: _temp2074= Cyc_NewControlFlow_use_Rval(
env, _temp2036->loc, _temp2074, _temp2072); return Cyc_NewControlFlow_add_vars(
_temp2074, _temp2041, Cyc_CfFlowInfo_unknown_all);} _LL2031: return Cyc_NewControlFlow_add_vars(
inflow, _temp2043, Cyc_CfFlowInfo_unknown_none); _LL2033:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp2076=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp2076[ 0]=({ struct
Cyc_Core_Impossible_struct _temp2077; _temp2077.tag= Cyc_Core_Impossible;
_temp2077.f1= _tag_arr("anal_decl: unexpected decl variant", sizeof(
unsigned char), 35u); _temp2077;}); _temp2076;})); _LL2025:;} static void Cyc_NewControlFlow_postpass(
struct Cyc_Absyn_Stmt* s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{
void* _temp2078=( void*) s->r; struct Cyc_Absyn_Stmt* _temp2102; struct _tuple3
_temp2104; struct Cyc_Absyn_Stmt* _temp2106; struct _tuple3 _temp2108; struct
Cyc_Absyn_Stmt* _temp2110; struct Cyc_Absyn_Stmt* _temp2112; struct Cyc_Absyn_Stmt*
_temp2114; struct Cyc_Absyn_Stmt* _temp2116; struct _tuple3 _temp2118; struct
Cyc_Absyn_Stmt* _temp2120; struct _tuple3 _temp2122; struct Cyc_Absyn_Stmt*
_temp2124; struct Cyc_Absyn_Stmt* _temp2126; struct Cyc_Absyn_Stmt* _temp2128;
struct Cyc_Absyn_Stmt* _temp2130; struct Cyc_Absyn_Stmt* _temp2132; struct Cyc_Absyn_Stmt*
_temp2134; struct Cyc_Absyn_Stmt* _temp2136; struct Cyc_List_List* _temp2138;
struct Cyc_Absyn_Stmt* _temp2140; struct Cyc_List_List* _temp2142; _LL2080: if((
unsigned int) _temp2078 >  1u?*(( int*) _temp2078) ==  Cyc_Absyn_For_s: 0){
_LL2109: _temp2108=(( struct Cyc_Absyn_For_s_struct*) _temp2078)->f2; _LL2111:
_temp2110= _temp2108.f2; goto _LL2105; _LL2105: _temp2104=(( struct Cyc_Absyn_For_s_struct*)
_temp2078)->f3; _LL2107: _temp2106= _temp2104.f2; goto _LL2103; _LL2103:
_temp2102=(( struct Cyc_Absyn_For_s_struct*) _temp2078)->f4; goto _LL2081;}
else{ goto _LL2082;} _LL2082: if(( unsigned int) _temp2078 >  1u?*(( int*)
_temp2078) ==  Cyc_Absyn_Seq_s: 0){ _LL2115: _temp2114=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2078)->f1; goto _LL2113; _LL2113: _temp2112=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2078)->f2; goto _LL2083;} else{ goto _LL2084;} _LL2084: if(( unsigned int)
_temp2078 >  1u?*(( int*) _temp2078) ==  Cyc_Absyn_While_s: 0){ _LL2119:
_temp2118=(( struct Cyc_Absyn_While_s_struct*) _temp2078)->f1; _LL2121:
_temp2120= _temp2118.f2; goto _LL2117; _LL2117: _temp2116=(( struct Cyc_Absyn_While_s_struct*)
_temp2078)->f2; goto _LL2085;} else{ goto _LL2086;} _LL2086: if(( unsigned int)
_temp2078 >  1u?*(( int*) _temp2078) ==  Cyc_Absyn_Do_s: 0){ _LL2127: _temp2126=((
struct Cyc_Absyn_Do_s_struct*) _temp2078)->f1; goto _LL2123; _LL2123: _temp2122=((
struct Cyc_Absyn_Do_s_struct*) _temp2078)->f2; _LL2125: _temp2124= _temp2122.f2;
goto _LL2087;} else{ goto _LL2088;} _LL2088: if(( unsigned int) _temp2078 >  1u?*((
int*) _temp2078) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL2131: _temp2130=(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp2078)->f2; goto _LL2129; _LL2129: _temp2128=((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2078)->f3; goto _LL2089;} else{ goto
_LL2090;} _LL2090: if(( unsigned int) _temp2078 >  1u?*(( int*) _temp2078) == 
Cyc_Absyn_Region_s: 0){ _LL2133: _temp2132=(( struct Cyc_Absyn_Region_s_struct*)
_temp2078)->f3; goto _LL2091;} else{ goto _LL2092;} _LL2092: if(( unsigned int)
_temp2078 >  1u?*(( int*) _temp2078) ==  Cyc_Absyn_Label_s: 0){ _LL2135:
_temp2134=(( struct Cyc_Absyn_Label_s_struct*) _temp2078)->f2; goto _LL2093;}
else{ goto _LL2094;} _LL2094: if(( unsigned int) _temp2078 >  1u?*(( int*)
_temp2078) ==  Cyc_Absyn_Decl_s: 0){ _LL2137: _temp2136=(( struct Cyc_Absyn_Decl_s_struct*)
_temp2078)->f2; goto _LL2095;} else{ goto _LL2096;} _LL2096: if(( unsigned int)
_temp2078 >  1u?*(( int*) _temp2078) ==  Cyc_Absyn_TryCatch_s: 0){ _LL2141:
_temp2140=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2078)->f1; goto _LL2139;
_LL2139: _temp2138=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2078)->f2; goto
_LL2097;} else{ goto _LL2098;} _LL2098: if(( unsigned int) _temp2078 >  1u?*((
int*) _temp2078) ==  Cyc_Absyn_Switch_s: 0){ _LL2143: _temp2142=(( struct Cyc_Absyn_Switch_s_struct*)
_temp2078)->f2; goto _LL2099;} else{ goto _LL2100;} _LL2100: goto _LL2101;
_LL2081: Cyc_NewControlFlow_postpass( _temp2106); _temp2114= _temp2102;
_temp2112= _temp2110; goto _LL2083; _LL2083: _temp2120= _temp2114; _temp2116=
_temp2112; goto _LL2085; _LL2085: _temp2126= _temp2116; _temp2124= _temp2120;
goto _LL2087; _LL2087: _temp2130= _temp2126; _temp2128= _temp2124; goto _LL2089;
_LL2089: Cyc_NewControlFlow_postpass( _temp2130); _temp2132= _temp2128; goto
_LL2091; _LL2091: _temp2134= _temp2132; goto _LL2093; _LL2093: _temp2136=
_temp2134; goto _LL2095; _LL2095: Cyc_NewControlFlow_postpass( _temp2136);
return; _LL2097: Cyc_NewControlFlow_postpass( _temp2140); _temp2142= _temp2138;
goto _LL2099; _LL2099: for( 0; _temp2142 !=  0; _temp2142= _temp2142->tl){ Cyc_NewControlFlow_postpass(((
struct Cyc_Absyn_Switch_clause*) _temp2142->hd)->body);} return; _LL2101:
return; _LL2079:;}} static void Cyc_NewControlFlow_check_fun( struct Cyc_Absyn_Fndecl*
fd){ void* inflow= Cyc_NewControlFlow_add_vars(( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp2152=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp2152[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp2153; _temp2153.tag= Cyc_CfFlowInfo_ReachableFL; _temp2153.f1= Cyc_Dict_empty(
Cyc_CfFlowInfo_root_cmp); _temp2153.f2= 0; _temp2153;}); _temp2152;}),( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( fd->param_vardecls))->v,
Cyc_CfFlowInfo_unknown_all); struct Cyc_NewControlFlow_AnalEnv* env=({ struct
Cyc_NewControlFlow_AnalEnv* _temp2151=( struct Cyc_NewControlFlow_AnalEnv*)
_cycalloc( sizeof( struct Cyc_NewControlFlow_AnalEnv)); _temp2151->iterate_again=
1; _temp2151->iteration_num= 0; _temp2151->in_try= 0; _temp2151->tryflow=( void*)
inflow; _temp2151->all_changed= 0; _temp2151;}); void* outflow= inflow; while(
env->iterate_again) { ++ env->iteration_num; env->iterate_again= 0; outflow= Cyc_NewControlFlow_anal_stmt(
env, inflow, fd->body);}{ void* _temp2144= outflow; _LL2146: if( _temp2144 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL2147;} else{ goto _LL2148;} _LL2148:
goto _LL2149; _LL2147: goto _LL2145; _LL2149: if(( void*) fd->ret_type != ( void*)
Cyc_Absyn_VoidType){({ void* _temp2150[ 0u]={}; Cyc_Tcutil_terr(( fd->body)->loc,
_tag_arr("function may not return a value", sizeof( unsigned char), 32u),
_tag_arr( _temp2150, sizeof( void*), 0u));});} _LL2145:;} Cyc_NewControlFlow_postpass(
fd->body);} void Cyc_NewControlFlow_cf_check( struct Cyc_List_List* ds){ for( 0;
ds !=  0; ds= ds->tl){ void* _temp2154=( void*)(( struct Cyc_Absyn_Decl*) ds->hd)->r;
struct Cyc_Absyn_Fndecl* _temp2166; struct Cyc_List_List* _temp2168; struct Cyc_List_List*
_temp2170; struct Cyc_List_List* _temp2172; _LL2156: if(*(( int*) _temp2154) == 
Cyc_Absyn_Fn_d){ _LL2167: _temp2166=(( struct Cyc_Absyn_Fn_d_struct*) _temp2154)->f1;
goto _LL2157;} else{ goto _LL2158;} _LL2158: if(*(( int*) _temp2154) ==  Cyc_Absyn_ExternC_d){
_LL2169: _temp2168=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2154)->f1; goto
_LL2159;} else{ goto _LL2160;} _LL2160: if(*(( int*) _temp2154) ==  Cyc_Absyn_Using_d){
_LL2171: _temp2170=(( struct Cyc_Absyn_Using_d_struct*) _temp2154)->f2; goto
_LL2161;} else{ goto _LL2162;} _LL2162: if(*(( int*) _temp2154) ==  Cyc_Absyn_Namespace_d){
_LL2173: _temp2172=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2154)->f2; goto
_LL2163;} else{ goto _LL2164;} _LL2164: goto _LL2165; _LL2157: Cyc_NewControlFlow_check_fun(
_temp2166); goto _LL2155; _LL2159: _temp2170= _temp2168; goto _LL2161; _LL2161:
_temp2172= _temp2170; goto _LL2163; _LL2163: Cyc_NewControlFlow_cf_check(
_temp2172); goto _LL2155; _LL2165: goto _LL2155; _LL2155:;}}
