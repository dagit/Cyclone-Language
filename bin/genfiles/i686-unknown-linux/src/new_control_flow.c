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
2; extern unsigned char Cyc_CfFlowInfo_IsZero[ 11u]; extern unsigned char Cyc_CfFlowInfo_NotZero[
12u]; extern unsigned char Cyc_CfFlowInfo_UnknownZ[ 13u]; static const int Cyc_CfFlowInfo_PlaceL=
0; struct Cyc_CfFlowInfo_PlaceL_struct{ int tag; struct Cyc_CfFlowInfo_Place* f1;
} ; static const int Cyc_CfFlowInfo_UnknownL= 0; static const int Cyc_CfFlowInfo_Zero=
0; static const int Cyc_CfFlowInfo_NotZeroAll= 1; static const int Cyc_CfFlowInfo_NotZeroThis=
2; static const int Cyc_CfFlowInfo_UnknownR= 0; struct Cyc_CfFlowInfo_UnknownR_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_Esc= 1; struct Cyc_CfFlowInfo_Esc_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_AddressOf= 2; struct Cyc_CfFlowInfo_AddressOf_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_Aggregate=
3; struct Cyc_CfFlowInfo_Aggregate_struct{ int tag; struct Cyc_Dict_Dict* f1; }
; static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_ReachableFL=
0; struct Cyc_CfFlowInfo_ReachableFL_struct{ int tag; struct Cyc_Dict_Dict* f1;
} ; extern struct Cyc_Set_Set* Cyc_CfFlowInfo_mt_place_set(); extern void* Cyc_CfFlowInfo_unknown_none;
extern void* Cyc_CfFlowInfo_unknown_all; extern int Cyc_CfFlowInfo_root_cmp(
void*, void*); extern void* Cyc_CfFlowInfo_typ_to_absrval( void* t, void*
leafval); extern void* Cyc_CfFlowInfo_initlevel( struct Cyc_Dict_Dict* d, void*
r); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d, struct
Cyc_CfFlowInfo_Place* place); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_deref(
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
struct Cyc_Dict_Dict* _temp23; _LL19: if( _temp17 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp17 >  1u?*(( int*)
_temp17) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL24: _temp23=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp17)->f1; goto _LL22;} else{ goto _LL18;} _LL20: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL22: for( 0; vds !=  0; vds= vds->tl){ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp25=({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp26=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp26[ 0]=({ struct
Cyc_CfFlowInfo_VarRoot_struct _temp27; _temp27.tag= Cyc_CfFlowInfo_VarRoot;
_temp27.f1=( struct Cyc_Absyn_Vardecl*) vds->hd; _temp27;}); _temp26;}); _temp23=
Cyc_Dict_insert( _temp23,( void*) _temp25, Cyc_CfFlowInfo_typ_to_absrval(( void*)((
struct Cyc_Absyn_Vardecl*) vds->hd)->type, leafval));} return( void*)({ struct
Cyc_CfFlowInfo_ReachableFL_struct* _temp28=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp28[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp29; _temp29.tag= Cyc_CfFlowInfo_ReachableFL;
_temp29.f1= _temp23; _temp29;}); _temp28;}); _LL18:;} static void* Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv* env, struct Cyc_Position_Segment* loc, void*
inflow, void* r){ void* _temp30= inflow; struct Cyc_Dict_Dict* _temp36; _LL32:
if( _temp30 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL33;} else{ goto _LL34;}
_LL34: if(( unsigned int) _temp30 >  1u?*(( int*) _temp30) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL37: _temp36=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp30)->f1;
goto _LL35;} else{ goto _LL31;} _LL33: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL35: if( Cyc_CfFlowInfo_initlevel( _temp36, r) != ( void*) Cyc_CfFlowInfo_AllIL){({
void* _temp38[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("expression may not be fully initialized",
sizeof( unsigned char), 40u), _tag_arr( _temp38, sizeof( void*), 0u));});}{
struct Cyc_Dict_Dict* _temp39= Cyc_CfFlowInfo_escape_deref( _temp36, env->all_changed,
r); if( _temp36 ==  _temp39){ return inflow;}{ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp40=({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp41=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp41[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp42; _temp42.tag= Cyc_CfFlowInfo_ReachableFL;
_temp42.f1= _temp39; _temp42;}); _temp41;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp40); return( void*) _temp40;}} _LL31:;} struct _tuple4{ void*
f1; struct Cyc_List_List* f2; } ; static struct _tuple4 Cyc_NewControlFlow_anal_unordered_Rexps(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_List_List* es){
if( es ==  0){ return({ struct _tuple4 _temp43; _temp43.f1= inflow; _temp43.f2=
0; _temp43;});} if( es->tl ==  0){ void* _temp46; void* _temp48; struct _tuple0
_temp44= Cyc_NewControlFlow_anal_Rexp( env, inflow,( struct Cyc_Absyn_Exp*) es->hd);
_LL49: _temp48= _temp44.f1; goto _LL47; _LL47: _temp46= _temp44.f2; goto _LL45;
_LL45: return({ struct _tuple4 _temp50; _temp50.f1= _temp48; _temp50.f2=({
struct Cyc_List_List* _temp51=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp51->hd=( void*) _temp46; _temp51->tl= 0; _temp51;});
_temp50;});}{ struct Cyc_Set_Set** outer_all_changed= env->all_changed; struct
Cyc_Set_Set* this_all_changed; void* old_inflow; void* outflow; struct Cyc_List_List*
rvals; do { this_all_changed= Cyc_CfFlowInfo_mt_place_set(); env->all_changed=({
struct Cyc_Set_Set** _temp52=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp52[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp52;});{ void*
_temp55; void* _temp57; struct _tuple0 _temp53= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*) es->hd); _LL58: _temp57= _temp53.f1; goto
_LL56; _LL56: _temp55= _temp53.f2; goto _LL54; _LL54: outflow= _temp57; rvals=({
struct Cyc_List_List* _temp59=( struct Cyc_List_List*) _cycalloc( sizeof( struct
Cyc_List_List)); _temp59->hd=( void*) _temp55; _temp59->tl= 0; _temp59;});
this_all_changed=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set*
s2)) Cyc_Set_union_two)( this_all_changed,*(( struct Cyc_Set_Set**) _check_null(
env->all_changed)));{ struct Cyc_List_List* es2= es->tl; for( 0; es2 !=  0; es2=
es2->tl){ env->all_changed=({ struct Cyc_Set_Set** _temp60=( struct Cyc_Set_Set**)
_cycalloc( sizeof( struct Cyc_Set_Set*)); _temp60[ 0]= Cyc_CfFlowInfo_mt_place_set();
_temp60;});{ void* _temp63; void* _temp65; struct _tuple0 _temp61= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*) es2->hd); _LL66: _temp65= _temp61.f1; goto
_LL64; _LL64: _temp63= _temp61.f2; goto _LL62; _LL62: rvals=({ struct Cyc_List_List*
_temp67=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp67->hd=( void*) _temp63; _temp67->tl= rvals; _temp67;}); outflow= Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& this_all_changed, outflow, _temp65, this_all_changed,*((
struct Cyc_Set_Set**) _check_null( env->all_changed))); this_all_changed=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(
this_all_changed,*(( struct Cyc_Set_Set**) _check_null( env->all_changed)));}}}
old_inflow= inflow; inflow= Cyc_CfFlowInfo_join_flow( outer_all_changed, inflow,
outflow);}} while ( ! Cyc_CfFlowInfo_flow_lessthan_approx( inflow, old_inflow));
if( outer_all_changed ==  0){ env->all_changed= 0;} else{ env->all_changed=({
struct Cyc_Set_Set** _temp68=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp68[ 0]=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1,
struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(* outer_all_changed,
this_all_changed); _temp68;});} Cyc_NewControlFlow_update_tryflow( env, outflow);
return({ struct _tuple4 _temp69; _temp69.f1= outflow; _temp69.f2= Cyc_List_imp_rev(
rvals); _temp69;});}} static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_List_List* es){
struct Cyc_List_List* _temp72; void* _temp74; struct _tuple4 _temp70= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, es); _LL75: _temp74= _temp70.f1; goto _LL73; _LL73: _temp72=
_temp70.f2; goto _LL71; _LL71:{ void* _temp76= _temp74; struct Cyc_Dict_Dict*
_temp82; _LL78: if(( unsigned int) _temp76 >  1u?*(( int*) _temp76) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL83: _temp82=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp76)->f1;
goto _LL79;} else{ goto _LL80;} _LL80: if( _temp76 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL81;} else{ goto _LL77;} _LL79: for( 0; _temp72 !=  0;( _temp72= _temp72->tl,
es=(( struct Cyc_List_List*) _check_null( es))->tl)){ if( Cyc_CfFlowInfo_initlevel(
_temp82,( void*) _temp72->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void*
_temp84[ 0u]={}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( es))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp84, sizeof( void*), 0u));});}} goto
_LL77; _LL81: goto _LL77; _LL77:;} return({ struct _tuple0 _temp85; _temp85.f1=
_temp74; _temp85.f2= Cyc_CfFlowInfo_unknown_all; _temp85;});} static void* Cyc_NewControlFlow_notzero(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, void* outflow, struct Cyc_Absyn_Exp*
e, void* il){ void* _temp86= outflow; struct Cyc_Dict_Dict* _temp92; _LL88: if(
_temp86 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL89;} else{ goto _LL90;}
_LL90: if(( unsigned int) _temp86 >  1u?*(( int*) _temp86) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL93: _temp92=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp86)->f1;
goto _LL91;} else{ goto _LL87;} _LL89: return outflow; _LL91: { void* _temp94=(
Cyc_NewControlFlow_anal_Lexp( env, inflow, e)).f2; struct Cyc_CfFlowInfo_Place*
_temp100; _LL96: if( _temp94 == ( void*) Cyc_CfFlowInfo_UnknownL){ goto _LL97;}
else{ goto _LL98;} _LL98: if(( unsigned int) _temp94 >  1u?*(( int*) _temp94) == 
Cyc_CfFlowInfo_PlaceL: 0){ _LL101: _temp100=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp94)->f1; goto _LL99;} else{ goto _LL95;} _LL97: return outflow; _LL99: {
void* nzval= il == ( void*) Cyc_CfFlowInfo_AllIL?( void*) Cyc_CfFlowInfo_NotZeroAll:(
void*) Cyc_CfFlowInfo_NotZeroThis; return( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp102=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp102[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp103; _temp103.tag= Cyc_CfFlowInfo_ReachableFL; _temp103.f1= Cyc_CfFlowInfo_assign_place(
e->loc, _temp92, env->all_changed, _temp100, nzval); _temp103;}); _temp102;});}
_LL95:;} _LL87:;} static struct _tuple0 Cyc_NewControlFlow_splitzero( struct Cyc_NewControlFlow_AnalEnv*
env, void* inflow, void* outflow, struct Cyc_Absyn_Exp* e, void* il){ void*
_temp104= outflow; struct Cyc_Dict_Dict* _temp110; _LL106: if( _temp104 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL107;} else{ goto _LL108;} _LL108: if((
unsigned int) _temp104 >  1u?*(( int*) _temp104) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL111: _temp110=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp104)->f1;
goto _LL109;} else{ goto _LL105;} _LL107: return({ struct _tuple0 _temp112;
_temp112.f1= outflow; _temp112.f2= outflow; _temp112;}); _LL109: { void*
_temp113=( Cyc_NewControlFlow_anal_Lexp( env, inflow, e)).f2; struct Cyc_CfFlowInfo_Place*
_temp119; _LL115: if( _temp113 == ( void*) Cyc_CfFlowInfo_UnknownL){ goto _LL116;}
else{ goto _LL117;} _LL117: if(( unsigned int) _temp113 >  1u?*(( int*) _temp113)
==  Cyc_CfFlowInfo_PlaceL: 0){ _LL120: _temp119=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp113)->f1; goto _LL118;} else{ goto _LL114;} _LL116: return({ struct _tuple0
_temp121; _temp121.f1= outflow; _temp121.f2= outflow; _temp121;}); _LL118: {
void* nzval= il == ( void*) Cyc_CfFlowInfo_AllIL?( void*) Cyc_CfFlowInfo_NotZeroAll:(
void*) Cyc_CfFlowInfo_NotZeroThis; return({ struct _tuple0 _temp122; _temp122.f1=(
void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp125=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp125[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp126; _temp126.tag= Cyc_CfFlowInfo_ReachableFL;
_temp126.f1= Cyc_CfFlowInfo_assign_place( e->loc, _temp110, env->all_changed,
_temp119, nzval); _temp126;}); _temp125;}); _temp122.f2=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp123=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp123[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp124; _temp124.tag= Cyc_CfFlowInfo_ReachableFL; _temp124.f1= Cyc_CfFlowInfo_assign_place(
e->loc, _temp110, env->all_changed, _temp119,( void*) Cyc_CfFlowInfo_Zero);
_temp124;}); _temp123;}); _temp122;});} _LL114:;} _LL105:;} static struct
_tuple0 Cyc_NewControlFlow_anal_derefR( struct Cyc_NewControlFlow_AnalEnv* env,
void* inflow, void* f, struct Cyc_Absyn_Exp* e, void* r){ void* _temp127= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo
_temp133; struct Cyc_Absyn_Conref* _temp135; void* _temp137; _LL129: if((
unsigned int) _temp127 >  3u?*(( int*) _temp127) ==  Cyc_Absyn_PointerType: 0){
_LL134: _temp133=(( struct Cyc_Absyn_PointerType_struct*) _temp127)->f1; _LL138:
_temp137=( void*) _temp133.elt_typ; goto _LL136; _LL136: _temp135= _temp133.bounds;
goto _LL130;} else{ goto _LL131;} _LL131: goto _LL132; _LL130: { void* _temp139=
f; struct Cyc_Dict_Dict* _temp145; _LL141: if( _temp139 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL142;} else{ goto _LL143;} _LL143: if(( unsigned int) _temp139 >  1u?*((
int*) _temp139) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL146: _temp145=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp139)->f1; goto _LL144;} else{ goto
_LL140;} _LL142: return({ struct _tuple0 _temp147; _temp147.f1= f; _temp147.f2=
Cyc_CfFlowInfo_typ_to_absrval( _temp137, Cyc_CfFlowInfo_unknown_all); _temp147;});
_LL144: if( Cyc_Tcutil_is_bound_one( _temp135)){ void* _temp148= r; struct Cyc_CfFlowInfo_Place*
_temp162; void* _temp164; _LL150: if( _temp148 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL151;} else{ goto _LL152;} _LL152: if( _temp148 == ( void*) Cyc_CfFlowInfo_NotZeroThis){
goto _LL153;} else{ goto _LL154;} _LL154: if(( unsigned int) _temp148 >  3u?*((
int*) _temp148) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL163: _temp162=(( struct Cyc_CfFlowInfo_AddressOf_struct*)
_temp148)->f1; goto _LL155;} else{ goto _LL156;} _LL156: if( _temp148 == ( void*)
Cyc_CfFlowInfo_Zero){ goto _LL157;} else{ goto _LL158;} _LL158: if((
unsigned int) _temp148 >  3u?*(( int*) _temp148) ==  Cyc_CfFlowInfo_UnknownR: 0){
_LL165: _temp164=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*) _temp148)->f1;
goto _LL159;} else{ goto _LL160;} _LL160: goto _LL161; _LL151: goto _LL153;
_LL153:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_NotZero)); goto
_LL149; _LL155:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_NotZero));
return({ struct _tuple0 _temp166; _temp166.f1= f; _temp166.f2= Cyc_CfFlowInfo_lookup_place(
_temp145, _temp162); _temp166;}); _LL157:( void*)( e->annot=( void*)(( void*)
Cyc_CfFlowInfo_IsZero)); return({ struct _tuple0 _temp167; _temp167.f1=( void*)
Cyc_CfFlowInfo_BottomFL; _temp167.f2= Cyc_CfFlowInfo_typ_to_absrval( _temp137,
Cyc_CfFlowInfo_unknown_all); _temp167;}); _LL159: f= Cyc_NewControlFlow_notzero(
env, inflow, f, e, _temp164); goto _LL161; _LL161:( void*)( e->annot=( void*)((
void*) Cyc_CfFlowInfo_UnknownZ)); _LL149:;} else{( void*)( e->annot=( void*)((
void*) Cyc_CfFlowInfo_UnknownZ));}{ void* _temp168= Cyc_CfFlowInfo_initlevel(
_temp145, r); _LL170: if( _temp168 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto
_LL171;} else{ goto _LL172;} _LL172: if( _temp168 == ( void*) Cyc_CfFlowInfo_AllIL){
goto _LL173;} else{ goto _LL174;} _LL174: if( _temp168 == ( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL175;} else{ goto _LL169;} _LL171:({ void* _temp176[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("dereference of possibly uninitialized pointer", sizeof(
unsigned char), 46u), _tag_arr( _temp176, sizeof( void*), 0u));}); goto _LL173;
_LL173: return({ struct _tuple0 _temp177; _temp177.f1= f; _temp177.f2= Cyc_CfFlowInfo_typ_to_absrval(
_temp137, Cyc_CfFlowInfo_unknown_all); _temp177;}); _LL175: return({ struct
_tuple0 _temp178; _temp178.f1= f; _temp178.f2= Cyc_CfFlowInfo_typ_to_absrval(
_temp137, Cyc_CfFlowInfo_unknown_none); _temp178;}); _LL169:;} _LL140:;} _LL132:(
int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp179=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp179[ 0]=({ struct
Cyc_Core_Impossible_struct _temp180; _temp180.tag= Cyc_Core_Impossible; _temp180.f1=
_tag_arr("right deref of non-pointer-type", sizeof( unsigned char), 32u);
_temp180;}); _temp179;})); _LL128:;} struct _tuple5{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Exp* f2; } ; static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e){
struct Cyc_Dict_Dict* d;{ void* _temp181= inflow; struct Cyc_Dict_Dict* _temp187;
_LL183: if( _temp181 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL184;} else{
goto _LL185;} _LL185: if(( unsigned int) _temp181 >  1u?*(( int*) _temp181) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL188: _temp187=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp181)->f1; goto _LL186;} else{ goto _LL182;} _LL184: return({ struct _tuple0
_temp189; _temp189.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp189.f2= Cyc_CfFlowInfo_unknown_all;
_temp189;}); _LL186: d= _temp187; _LL182:;}{ void* _temp190=( void*) e->r;
struct Cyc_Absyn_Exp* _temp288; struct Cyc_Absyn_Exp* _temp290; struct Cyc_Absyn_Exp*
_temp292; void* _temp294; void* _temp296; int _temp298; void* _temp300; void*
_temp302; void* _temp304; void* _temp306; struct Cyc_List_List* _temp308; void*
_temp310; void* _temp312; struct Cyc_Absyn_Vardecl* _temp314; void* _temp316;
struct Cyc_Absyn_Vardecl* _temp318; void* _temp320; struct Cyc_Absyn_Vardecl*
_temp322; struct Cyc_List_List* _temp324; struct Cyc_Absyn_Exp* _temp326; struct
Cyc_Absyn_Exp* _temp328; struct Cyc_Core_Opt* _temp330; struct Cyc_Core_Opt
_temp332; struct Cyc_Absyn_Exp* _temp333; struct Cyc_Absyn_Exp* _temp335; struct
Cyc_Core_Opt* _temp337; struct Cyc_Absyn_Exp* _temp339; struct Cyc_Absyn_Exp*
_temp341; struct Cyc_Absyn_Exp* _temp343; struct Cyc_Absyn_Exp* _temp345; struct
Cyc_List_List* _temp347; struct Cyc_Absyn_Exp* _temp349; struct Cyc_Absyn_MallocInfo
_temp351; int _temp353; struct Cyc_Absyn_Exp* _temp355; void** _temp357; struct
Cyc_Absyn_Exp* _temp359; int _temp361; struct Cyc_Absyn_Exp* _temp363; struct
Cyc_Absyn_Exp* _temp365; struct Cyc_Absyn_Exp* _temp367; struct Cyc_Absyn_Exp*
_temp369; struct _tagged_arr* _temp371; struct Cyc_Absyn_Exp* _temp373; struct
_tagged_arr* _temp375; struct Cyc_Absyn_Exp* _temp377; struct Cyc_Absyn_Exp*
_temp379; struct Cyc_Absyn_Exp* _temp381; struct Cyc_Absyn_Exp* _temp383; struct
Cyc_Absyn_Exp* _temp385; struct Cyc_Absyn_Exp* _temp387; struct Cyc_List_List*
_temp389; struct Cyc_List_List* _temp391; struct Cyc_List_List* _temp393; struct
Cyc_List_List* _temp395; struct Cyc_List_List* _temp397; struct Cyc_Absyn_Exp*
_temp399; struct Cyc_Absyn_Exp* _temp401; struct Cyc_Absyn_Vardecl* _temp403;
struct Cyc_Absyn_Stmt* _temp405; void* _temp407; _LL192: if(*(( int*) _temp190)
==  Cyc_Absyn_Cast_e){ _LL289: _temp288=(( struct Cyc_Absyn_Cast_e_struct*)
_temp190)->f2; goto _LL193;} else{ goto _LL194;} _LL194: if(*(( int*) _temp190)
==  Cyc_Absyn_NoInstantiate_e){ _LL291: _temp290=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp190)->f1; goto _LL195;} else{ goto _LL196;} _LL196: if(*(( int*) _temp190)
==  Cyc_Absyn_Instantiate_e){ _LL293: _temp292=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp190)->f1; goto _LL197;} else{ goto _LL198;} _LL198: if(*(( int*) _temp190)
==  Cyc_Absyn_Const_e){ _LL295: _temp294=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp190)->f1; if( _temp294 == ( void*) Cyc_Absyn_Null_c){ goto _LL199;} else{
goto _LL200;}} else{ goto _LL200;} _LL200: if(*(( int*) _temp190) ==  Cyc_Absyn_Const_e){
_LL297: _temp296=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp190)->f1; if((
unsigned int) _temp296 >  1u?*(( int*) _temp296) ==  Cyc_Absyn_Int_c: 0){ _LL301:
_temp300=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp296)->f1; if( _temp300
== ( void*) Cyc_Absyn_Signed){ goto _LL299;} else{ goto _LL202;} _LL299:
_temp298=(( struct Cyc_Absyn_Int_c_struct*) _temp296)->f2; if( _temp298 ==  0){
goto _LL201;} else{ goto _LL202;}} else{ goto _LL202;}} else{ goto _LL202;}
_LL202: if(*(( int*) _temp190) ==  Cyc_Absyn_Const_e){ _LL303: _temp302=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp190)->f1; if(( unsigned int) _temp302 > 
1u?*(( int*) _temp302) ==  Cyc_Absyn_Int_c: 0){ _LL305: _temp304=( void*)((
struct Cyc_Absyn_Int_c_struct*) _temp302)->f1; if( _temp304 == ( void*) Cyc_Absyn_Signed){
goto _LL203;} else{ goto _LL204;}} else{ goto _LL204;}} else{ goto _LL204;}
_LL204: if(*(( int*) _temp190) ==  Cyc_Absyn_Var_e){ _LL307: _temp306=( void*)((
struct Cyc_Absyn_Var_e_struct*) _temp190)->f2; if(( unsigned int) _temp306 >  1u?*((
int*) _temp306) ==  Cyc_Absyn_Funname_b: 0){ goto _LL205;} else{ goto _LL206;}}
else{ goto _LL206;} _LL206: if(*(( int*) _temp190) ==  Cyc_Absyn_Tunion_e){
_LL309: _temp308=(( struct Cyc_Absyn_Tunion_e_struct*) _temp190)->f3; if(
_temp308 ==  0){ goto _LL207;} else{ goto _LL208;}} else{ goto _LL208;} _LL208:
if(*(( int*) _temp190) ==  Cyc_Absyn_Const_e){ goto _LL209;} else{ goto _LL210;}
_LL210: if(*(( int*) _temp190) ==  Cyc_Absyn_Sizeofexp_e){ goto _LL211;} else{
goto _LL212;} _LL212: if(*(( int*) _temp190) ==  Cyc_Absyn_Sizeoftyp_e){ goto
_LL213;} else{ goto _LL214;} _LL214: if(*(( int*) _temp190) ==  Cyc_Absyn_Offsetof_e){
goto _LL215;} else{ goto _LL216;} _LL216: if(*(( int*) _temp190) ==  Cyc_Absyn_Gentyp_e){
goto _LL217;} else{ goto _LL218;} _LL218: if(*(( int*) _temp190) ==  Cyc_Absyn_AnonEnum_e){
goto _LL219;} else{ goto _LL220;} _LL220: if(*(( int*) _temp190) ==  Cyc_Absyn_Enum_e){
goto _LL221;} else{ goto _LL222;} _LL222: if(*(( int*) _temp190) ==  Cyc_Absyn_Var_e){
_LL311: _temp310=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp190)->f2; if((
unsigned int) _temp310 >  1u?*(( int*) _temp310) ==  Cyc_Absyn_Global_b: 0){
goto _LL223;} else{ goto _LL224;}} else{ goto _LL224;} _LL224: if(*(( int*)
_temp190) ==  Cyc_Absyn_Var_e){ _LL313: _temp312=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp190)->f2; if(( unsigned int) _temp312 >  1u?*(( int*) _temp312) ==  Cyc_Absyn_Param_b:
0){ _LL315: _temp314=(( struct Cyc_Absyn_Param_b_struct*) _temp312)->f1; goto
_LL225;} else{ goto _LL226;}} else{ goto _LL226;} _LL226: if(*(( int*) _temp190)
==  Cyc_Absyn_Var_e){ _LL317: _temp316=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp190)->f2; if(( unsigned int) _temp316 >  1u?*(( int*) _temp316) ==  Cyc_Absyn_Local_b:
0){ _LL319: _temp318=(( struct Cyc_Absyn_Local_b_struct*) _temp316)->f1; goto
_LL227;} else{ goto _LL228;}} else{ goto _LL228;} _LL228: if(*(( int*) _temp190)
==  Cyc_Absyn_Var_e){ _LL321: _temp320=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp190)->f2; if(( unsigned int) _temp320 >  1u?*(( int*) _temp320) ==  Cyc_Absyn_Pat_b:
0){ _LL323: _temp322=(( struct Cyc_Absyn_Pat_b_struct*) _temp320)->f1; goto
_LL229;} else{ goto _LL230;}} else{ goto _LL230;} _LL230: if(*(( int*) _temp190)
==  Cyc_Absyn_Primop_e){ _LL325: _temp324=(( struct Cyc_Absyn_Primop_e_struct*)
_temp190)->f2; goto _LL231;} else{ goto _LL232;} _LL232: if(*(( int*) _temp190)
==  Cyc_Absyn_Increment_e){ _LL327: _temp326=(( struct Cyc_Absyn_Increment_e_struct*)
_temp190)->f1; goto _LL233;} else{ goto _LL234;} _LL234: if(*(( int*) _temp190)
==  Cyc_Absyn_AssignOp_e){ _LL334: _temp333=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp190)->f1; goto _LL331; _LL331: _temp330=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp190)->f2; if( _temp330 ==  0){ goto _LL236;} else{ _temp332=* _temp330;
goto _LL329;} _LL329: _temp328=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp190)->f3;
goto _LL235;} else{ goto _LL236;} _LL236: if(*(( int*) _temp190) ==  Cyc_Absyn_AssignOp_e){
_LL340: _temp339=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp190)->f1; goto
_LL338; _LL338: _temp337=(( struct Cyc_Absyn_AssignOp_e_struct*) _temp190)->f2;
if( _temp337 ==  0){ goto _LL336;} else{ goto _LL238;} _LL336: _temp335=((
struct Cyc_Absyn_AssignOp_e_struct*) _temp190)->f3; goto _LL237;} else{ goto
_LL238;} _LL238: if(*(( int*) _temp190) ==  Cyc_Absyn_SeqExp_e){ _LL344:
_temp343=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp190)->f1; goto _LL342;
_LL342: _temp341=(( struct Cyc_Absyn_SeqExp_e_struct*) _temp190)->f2; goto
_LL239;} else{ goto _LL240;} _LL240: if(*(( int*) _temp190) ==  Cyc_Absyn_Throw_e){
_LL346: _temp345=(( struct Cyc_Absyn_Throw_e_struct*) _temp190)->f1; goto _LL241;}
else{ goto _LL242;} _LL242: if(*(( int*) _temp190) ==  Cyc_Absyn_FnCall_e){
_LL350: _temp349=(( struct Cyc_Absyn_FnCall_e_struct*) _temp190)->f1; goto
_LL348; _LL348: _temp347=(( struct Cyc_Absyn_FnCall_e_struct*) _temp190)->f2;
goto _LL243;} else{ goto _LL244;} _LL244: if(*(( int*) _temp190) ==  Cyc_Absyn_Malloc_e){
_LL352: _temp351=(( struct Cyc_Absyn_Malloc_e_struct*) _temp190)->f1; _LL362:
_temp361= _temp351.is_calloc; goto _LL360; _LL360: _temp359= _temp351.rgn; goto
_LL358; _LL358: _temp357= _temp351.elt_type; goto _LL356; _LL356: _temp355=
_temp351.num_elts; goto _LL354; _LL354: _temp353= _temp351.fat_result; goto
_LL245;} else{ goto _LL246;} _LL246: if(*(( int*) _temp190) ==  Cyc_Absyn_New_e){
_LL366: _temp365=(( struct Cyc_Absyn_New_e_struct*) _temp190)->f1; goto _LL364;
_LL364: _temp363=(( struct Cyc_Absyn_New_e_struct*) _temp190)->f2; goto _LL247;}
else{ goto _LL248;} _LL248: if(*(( int*) _temp190) ==  Cyc_Absyn_Address_e){
_LL368: _temp367=(( struct Cyc_Absyn_Address_e_struct*) _temp190)->f1; goto
_LL249;} else{ goto _LL250;} _LL250: if(*(( int*) _temp190) ==  Cyc_Absyn_Deref_e){
_LL370: _temp369=(( struct Cyc_Absyn_Deref_e_struct*) _temp190)->f1; goto _LL251;}
else{ goto _LL252;} _LL252: if(*(( int*) _temp190) ==  Cyc_Absyn_StructMember_e){
_LL374: _temp373=(( struct Cyc_Absyn_StructMember_e_struct*) _temp190)->f1; goto
_LL372; _LL372: _temp371=(( struct Cyc_Absyn_StructMember_e_struct*) _temp190)->f2;
goto _LL253;} else{ goto _LL254;} _LL254: if(*(( int*) _temp190) ==  Cyc_Absyn_StructArrow_e){
_LL378: _temp377=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp190)->f1; goto
_LL376; _LL376: _temp375=(( struct Cyc_Absyn_StructArrow_e_struct*) _temp190)->f2;
goto _LL255;} else{ goto _LL256;} _LL256: if(*(( int*) _temp190) ==  Cyc_Absyn_Conditional_e){
_LL384: _temp383=(( struct Cyc_Absyn_Conditional_e_struct*) _temp190)->f1; goto
_LL382; _LL382: _temp381=(( struct Cyc_Absyn_Conditional_e_struct*) _temp190)->f2;
goto _LL380; _LL380: _temp379=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp190)->f3; goto _LL257;} else{ goto _LL258;} _LL258: if(*(( int*) _temp190)
==  Cyc_Absyn_Subscript_e){ _LL388: _temp387=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp190)->f1; goto _LL386; _LL386: _temp385=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp190)->f2; goto _LL259;} else{ goto _LL260;} _LL260: if(*(( int*) _temp190)
==  Cyc_Absyn_Tunion_e){ _LL390: _temp389=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp190)->f3; goto _LL261;} else{ goto _LL262;} _LL262: if(*(( int*) _temp190)
==  Cyc_Absyn_Tuple_e){ _LL392: _temp391=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp190)->f1; goto _LL263;} else{ goto _LL264;} _LL264: if(*(( int*) _temp190)
==  Cyc_Absyn_AnonStruct_e){ _LL394: _temp393=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp190)->f2; goto _LL265;} else{ goto _LL266;} _LL266: if(*(( int*) _temp190)
==  Cyc_Absyn_Struct_e){ _LL396: _temp395=(( struct Cyc_Absyn_Struct_e_struct*)
_temp190)->f3; goto _LL267;} else{ goto _LL268;} _LL268: if(*(( int*) _temp190)
==  Cyc_Absyn_Array_e){ _LL398: _temp397=(( struct Cyc_Absyn_Array_e_struct*)
_temp190)->f1; goto _LL269;} else{ goto _LL270;} _LL270: if(*(( int*) _temp190)
==  Cyc_Absyn_Comprehension_e){ _LL404: _temp403=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp190)->f1; goto _LL402; _LL402: _temp401=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp190)->f2; goto _LL400; _LL400: _temp399=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp190)->f3; goto _LL271;} else{ goto _LL272;} _LL272: if(*(( int*) _temp190)
==  Cyc_Absyn_StmtExp_e){ _LL406: _temp405=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp190)->f1; goto _LL273;} else{ goto _LL274;} _LL274: if(*(( int*) _temp190)
==  Cyc_Absyn_Var_e){ _LL408: _temp407=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp190)->f2; if( _temp407 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL275;}
else{ goto _LL276;}} else{ goto _LL276;} _LL276: if(*(( int*) _temp190) ==  Cyc_Absyn_UnknownId_e){
goto _LL277;} else{ goto _LL278;} _LL278: if(*(( int*) _temp190) ==  Cyc_Absyn_UnknownCall_e){
goto _LL279;} else{ goto _LL280;} _LL280: if(*(( int*) _temp190) ==  Cyc_Absyn_UnresolvedMem_e){
goto _LL281;} else{ goto _LL282;} _LL282: if(*(( int*) _temp190) ==  Cyc_Absyn_CompoundLit_e){
goto _LL283;} else{ goto _LL284;} _LL284: if(*(( int*) _temp190) ==  Cyc_Absyn_Codegen_e){
goto _LL285;} else{ goto _LL286;} _LL286: if(*(( int*) _temp190) ==  Cyc_Absyn_Fill_e){
goto _LL287;} else{ goto _LL191;} _LL193: _temp290= _temp288; goto _LL195;
_LL195: _temp292= _temp290; goto _LL197; _LL197: return Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp292); _LL199: goto _LL201; _LL201: return({ struct _tuple0
_temp409; _temp409.f1= inflow; _temp409.f2=( void*) Cyc_CfFlowInfo_Zero;
_temp409;}); _LL203: goto _LL205; _LL205: return({ struct _tuple0 _temp410;
_temp410.f1= inflow; _temp410.f2=( void*) Cyc_CfFlowInfo_NotZeroAll; _temp410;});
_LL207: goto _LL209; _LL209: goto _LL211; _LL211: goto _LL213; _LL213: goto
_LL215; _LL215: goto _LL217; _LL217: goto _LL219; _LL219: goto _LL221; _LL221:
return({ struct _tuple0 _temp411; _temp411.f1= inflow; _temp411.f2= Cyc_CfFlowInfo_unknown_all;
_temp411;}); _LL223: return({ struct _tuple0 _temp412; _temp412.f1= inflow;
_temp412.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp412;}); _LL225:
_temp318= _temp314; goto _LL227; _LL227: _temp322= _temp318; goto _LL229; _LL229:
return({ struct _tuple0 _temp413; _temp413.f1= inflow; _temp413.f2= Cyc_Dict_lookup(
d,( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct* _temp414=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp414[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp415; _temp415.tag= Cyc_CfFlowInfo_VarRoot;
_temp415.f1= _temp322; _temp415;}); _temp414;})); _temp413;}); _LL231: return
Cyc_NewControlFlow_anal_use_ints( env, inflow, _temp324); _LL233: { void*
_temp419; struct _tuple0 _temp417= Cyc_NewControlFlow_anal_use_ints( env, inflow,({
struct Cyc_Absyn_Exp* _temp416[ 1u]; _temp416[ 0u]= _temp326;(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp416, sizeof( struct Cyc_Absyn_Exp*),
1u));})); _LL420: _temp419= _temp417.f1; goto _LL418; _LL418: { void* _temp423;
struct _tuple0 _temp421= Cyc_NewControlFlow_anal_Lexp( env, _temp419, _temp326);
_LL424: _temp423= _temp421.f2; goto _LL422; _LL422:{ struct _tuple0 _temp426=({
struct _tuple0 _temp425; _temp425.f1= _temp423; _temp425.f2= _temp419; _temp425;});
void* _temp432; struct Cyc_Dict_Dict* _temp434; void* _temp436; struct Cyc_CfFlowInfo_Place*
_temp438; _LL428: _LL437: _temp436= _temp426.f1; if(( unsigned int) _temp436 > 
1u?*(( int*) _temp436) ==  Cyc_CfFlowInfo_PlaceL: 0){ _LL439: _temp438=(( struct
Cyc_CfFlowInfo_PlaceL_struct*) _temp436)->f1; goto _LL433;} else{ goto _LL430;}
_LL433: _temp432= _temp426.f2; if(( unsigned int) _temp432 >  1u?*(( int*)
_temp432) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL435: _temp434=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp432)->f1; goto _LL429;} else{ goto _LL430;} _LL430: goto _LL431; _LL429:
_temp419=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp440=( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct));
_temp440[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct _temp441; _temp441.tag=
Cyc_CfFlowInfo_ReachableFL; _temp441.f1= Cyc_CfFlowInfo_assign_place( _temp326->loc,
_temp434, env->all_changed, _temp438, Cyc_CfFlowInfo_unknown_all); _temp441;});
_temp440;}); goto _LL427; _LL431: goto _LL427; _LL427:;} return({ struct _tuple0
_temp442; _temp442.f1= _temp419; _temp442.f2= Cyc_CfFlowInfo_unknown_all;
_temp442;});}} _LL235: { void* _temp446; struct _tuple0 _temp444= Cyc_NewControlFlow_anal_use_ints(
env, inflow,({ struct Cyc_Absyn_Exp* _temp443[ 2u]; _temp443[ 1u]= _temp328;
_temp443[ 0u]= _temp333;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp443, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL447: _temp446=
_temp444.f1; goto _LL445; _LL445: { void* _temp450; struct _tuple0 _temp448= Cyc_NewControlFlow_anal_Lexp(
env, _temp446, e); _LL451: _temp450= _temp448.f2; goto _LL449; _LL449:{ struct
_tuple0 _temp453=({ struct _tuple0 _temp452; _temp452.f1= _temp450; _temp452.f2=
_temp446; _temp452;}); void* _temp459; struct Cyc_Dict_Dict* _temp461; void*
_temp463; struct Cyc_CfFlowInfo_Place* _temp465; _LL455: _LL464: _temp463=
_temp453.f1; if(( unsigned int) _temp463 >  1u?*(( int*) _temp463) ==  Cyc_CfFlowInfo_PlaceL:
0){ _LL466: _temp465=(( struct Cyc_CfFlowInfo_PlaceL_struct*) _temp463)->f1;
goto _LL460;} else{ goto _LL457;} _LL460: _temp459= _temp453.f2; if((
unsigned int) _temp459 >  1u?*(( int*) _temp459) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL462: _temp461=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp459)->f1;
goto _LL456;} else{ goto _LL457;} _LL457: goto _LL458; _LL456: _temp446=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp467=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp467[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp468; _temp468.tag= Cyc_CfFlowInfo_ReachableFL;
_temp468.f1= Cyc_CfFlowInfo_assign_place( _temp333->loc, _temp461, env->all_changed,
_temp465, Cyc_CfFlowInfo_unknown_all); _temp468;}); _temp467;}); goto _LL454;
_LL458: goto _LL454; _LL454:;} return({ struct _tuple0 _temp469; _temp469.f1=
_temp446; _temp469.f2= Cyc_CfFlowInfo_unknown_all; _temp469;});}} _LL237: {
struct Cyc_Set_Set** _temp470= env->all_changed; while( 1) { env->all_changed=({
struct Cyc_Set_Set** _temp471=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp471[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp471;});{ void*
_temp474; void* _temp476; struct _tuple0 _temp472= Cyc_NewControlFlow_anal_Lexp(
env, inflow, _temp339); _LL477: _temp476= _temp472.f1; goto _LL475; _LL475:
_temp474= _temp472.f2; goto _LL473; _LL473: { struct Cyc_Set_Set* _temp478=*((
struct Cyc_Set_Set**) _check_null( env->all_changed)); env->all_changed=({
struct Cyc_Set_Set** _temp479=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp479[ 0]= Cyc_CfFlowInfo_mt_place_set(); _temp479;});{ void*
_temp482; void* _temp484; struct _tuple0 _temp480= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp335); _LL485: _temp484= _temp480.f1; goto _LL483; _LL483:
_temp482= _temp480.f2; goto _LL481; _LL481: { struct Cyc_Set_Set* _temp486=*((
struct Cyc_Set_Set**) _check_null( env->all_changed)); void* _temp487= Cyc_CfFlowInfo_after_flow((
struct Cyc_Set_Set**)& _temp478, _temp476, _temp484, _temp478, _temp486); void*
_temp488= Cyc_CfFlowInfo_join_flow( _temp470, inflow, _temp487); if( Cyc_CfFlowInfo_flow_lessthan_approx(
_temp488, inflow)){ if( _temp470 ==  0){ env->all_changed= 0;} else{*(( struct
Cyc_Set_Set**) _check_null( env->all_changed))=(( struct Cyc_Set_Set*(*)( struct
Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(* _temp470,((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s1, struct Cyc_Set_Set* s2)) Cyc_Set_union_two)(
_temp478, _temp486));}{ void* _temp489= _temp487; struct Cyc_Dict_Dict* _temp495;
_LL491: if( _temp489 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL492;} else{
goto _LL493;} _LL493: if(( unsigned int) _temp489 >  1u?*(( int*) _temp489) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL496: _temp495=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp489)->f1; goto _LL494;} else{ goto _LL490;} _LL492: return({ struct _tuple0
_temp497; _temp497.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp497.f2= _temp482;
_temp497;}); _LL494: { void* _temp498= _temp474; struct Cyc_CfFlowInfo_Place*
_temp504; _LL500: if(( unsigned int) _temp498 >  1u?*(( int*) _temp498) ==  Cyc_CfFlowInfo_PlaceL:
0){ _LL505: _temp504=(( struct Cyc_CfFlowInfo_PlaceL_struct*) _temp498)->f1;
goto _LL501;} else{ goto _LL502;} _LL502: if( _temp498 == ( void*) Cyc_CfFlowInfo_UnknownL){
goto _LL503;} else{ goto _LL499;} _LL501: _temp495= Cyc_CfFlowInfo_assign_place(
e->loc, _temp495, env->all_changed, _temp504, _temp482); _temp487=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp506=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp506[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp507; _temp507.tag= Cyc_CfFlowInfo_ReachableFL;
_temp507.f1= _temp495; _temp507;}); _temp506;}); Cyc_NewControlFlow_update_tryflow(
env, _temp487); return({ struct _tuple0 _temp508; _temp508.f1= _temp487;
_temp508.f2= _temp482; _temp508;}); _LL503: return({ struct _tuple0 _temp509;
_temp509.f1= Cyc_NewControlFlow_use_Rval( env, _temp335->loc, _temp487, _temp482);
_temp509.f2= _temp482; _temp509;}); _LL499:;} _LL490:;}} inflow= _temp488;}}}}}}
_LL239: return Cyc_NewControlFlow_anal_Rexp( env,( Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp343)).f1, _temp341); _LL241: { void* _temp512; void* _temp514;
struct _tuple0 _temp510= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp345);
_LL515: _temp514= _temp510.f1; goto _LL513; _LL513: _temp512= _temp510.f2; goto
_LL511; _LL511: Cyc_NewControlFlow_use_Rval( env, _temp345->loc, _temp514,
_temp512); return({ struct _tuple0 _temp516; _temp516.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp516.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp516;});} _LL243:
_temp347=({ struct Cyc_List_List* _temp517=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp517->hd=( void*) _temp349; _temp517->tl=
_temp347; _temp517;});{ struct Cyc_List_List* _temp520; void* _temp522; struct
_tuple4 _temp518= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp347);
_LL523: _temp522= _temp518.f1; goto _LL521; _LL521: _temp520= _temp518.f2; goto
_LL519; _LL519: for( 0; _temp520 !=  0;( _temp520= _temp520->tl, _temp347=((
struct Cyc_List_List*) _check_null( _temp347))->tl)){ _temp522= Cyc_NewControlFlow_use_Rval(
env,(( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp347))->hd)->loc,
_temp522,( void*) _temp520->hd);} return({ struct _tuple0 _temp524; _temp524.f1=
_temp522; _temp524.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp524;});} _LL245: {
void* root=( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp551=( struct
Cyc_CfFlowInfo_MallocPt_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp551[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp552; _temp552.tag=
Cyc_CfFlowInfo_MallocPt; _temp552.f1= _temp355; _temp552;}); _temp551;}); struct
Cyc_CfFlowInfo_Place* place=({ struct Cyc_CfFlowInfo_Place* _temp550=( struct
Cyc_CfFlowInfo_Place*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Place));
_temp550->root=( void*) root; _temp550->fields= 0; _temp550;}); void* rval=(
void*)({ struct Cyc_CfFlowInfo_AddressOf_struct* _temp548=( struct Cyc_CfFlowInfo_AddressOf_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_AddressOf_struct)); _temp548[ 0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _temp549; _temp549.tag= Cyc_CfFlowInfo_AddressOf;
_temp549.f1= place; _temp549;}); _temp548;}); void* place_val= _temp353?( void*)
Cyc_CfFlowInfo_NotZeroAll: Cyc_CfFlowInfo_typ_to_absrval(*(( void**) _check_null(
_temp357)), Cyc_CfFlowInfo_unknown_none); void* outflow; if( env->all_changed != 
0){*(( struct Cyc_Set_Set**) _check_null( env->all_changed))=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(*((
struct Cyc_Set_Set**) _check_null( env->all_changed)), place);} if( _temp359 != 
0){ struct Cyc_List_List* _temp528; void* _temp530; struct _tuple4 _temp526= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp525[ 2u]; _temp525[ 1u]= _temp355;
_temp525[ 0u]=( struct Cyc_Absyn_Exp*) _check_null( _temp359);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp525, sizeof( struct Cyc_Absyn_Exp*),
2u));})); _LL531: _temp530= _temp526.f1; goto _LL529; _LL529: _temp528= _temp526.f2;
goto _LL527; _LL527: outflow= _temp530;} else{ void* _temp534; struct _tuple0
_temp532= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp355); _LL535: _temp534=
_temp532.f1; goto _LL533; _LL533: outflow= _temp534;}{ void* _temp536= outflow;
struct Cyc_Dict_Dict* _temp542; _LL538: if( _temp536 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL539;} else{ goto _LL540;} _LL540: if(( unsigned int) _temp536 >  1u?*((
int*) _temp536) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL543: _temp542=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp536)->f1; goto _LL541;} else{ goto
_LL537;} _LL539: return({ struct _tuple0 _temp544; _temp544.f1= outflow;
_temp544.f2= rval; _temp544;}); _LL541: return({ struct _tuple0 _temp545;
_temp545.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp546=(
struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct));
_temp546[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct _temp547; _temp547.tag=
Cyc_CfFlowInfo_ReachableFL; _temp547.f1= Cyc_Dict_insert( _temp542, root,
place_val); _temp547;}); _temp546;}); _temp545.f2= rval; _temp545;}); _LL537:;}}
_LL247: { void* root=( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp581=(
struct Cyc_CfFlowInfo_MallocPt_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp581[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp582; _temp582.tag=
Cyc_CfFlowInfo_MallocPt; _temp582.f1= _temp363; _temp582;}); _temp581;}); struct
Cyc_CfFlowInfo_Place* place=({ struct Cyc_CfFlowInfo_Place* _temp580=( struct
Cyc_CfFlowInfo_Place*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Place));
_temp580->root=( void*) root; _temp580->fields= 0; _temp580;}); void* rval=(
void*)({ struct Cyc_CfFlowInfo_AddressOf_struct* _temp578=( struct Cyc_CfFlowInfo_AddressOf_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_AddressOf_struct)); _temp578[ 0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _temp579; _temp579.tag= Cyc_CfFlowInfo_AddressOf;
_temp579.f1= place; _temp579;}); _temp578;}); if( env->all_changed !=  0){*((
struct Cyc_Set_Set**) _check_null( env->all_changed))=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(*((
struct Cyc_Set_Set**) _check_null( env->all_changed)), place);}{ void* outflow;
void* place_val; if( _temp365 !=  0){ struct Cyc_List_List* _temp556; void*
_temp558; struct _tuple4 _temp554= Cyc_NewControlFlow_anal_unordered_Rexps( env,
inflow,({ struct Cyc_Absyn_Exp* _temp553[ 2u]; _temp553[ 1u]= _temp363; _temp553[
0u]=( struct Cyc_Absyn_Exp*) _check_null( _temp365);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp553, sizeof( struct Cyc_Absyn_Exp*),
2u));})); _LL559: _temp558= _temp554.f1; goto _LL557; _LL557: _temp556= _temp554.f2;
goto _LL555; _LL555: outflow= _temp558; place_val=( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp556))->tl))->hd;} else{
void* _temp562; void* _temp564; struct _tuple0 _temp560= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp363); _LL565: _temp564= _temp560.f1; goto _LL563; _LL563:
_temp562= _temp560.f2; goto _LL561; _LL561: outflow= _temp564; place_val=
_temp562;}{ void* _temp566= outflow; struct Cyc_Dict_Dict* _temp572; _LL568: if(
_temp566 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL569;} else{ goto _LL570;}
_LL570: if(( unsigned int) _temp566 >  1u?*(( int*) _temp566) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL573: _temp572=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp566)->f1;
goto _LL571;} else{ goto _LL567;} _LL569: return({ struct _tuple0 _temp574;
_temp574.f1= outflow; _temp574.f2= rval; _temp574;}); _LL571: return({ struct
_tuple0 _temp575; _temp575.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp576=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp576[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp577; _temp577.tag= Cyc_CfFlowInfo_ReachableFL; _temp577.f1= Cyc_Dict_insert(
_temp572, root, place_val); _temp577;}); _temp576;}); _temp575.f2= rval;
_temp575;}); _LL567:;}}} _LL249: { void* _temp585; void* _temp587; struct
_tuple0 _temp583= Cyc_NewControlFlow_anal_Lexp( env, inflow, _temp367); _LL588:
_temp587= _temp583.f1; goto _LL586; _LL586: _temp585= _temp583.f2; goto _LL584;
_LL584: { void* _temp589= _temp585; struct Cyc_CfFlowInfo_Place* _temp595;
_LL591: if( _temp589 == ( void*) Cyc_CfFlowInfo_UnknownL){ goto _LL592;} else{
goto _LL593;} _LL593: if(( unsigned int) _temp589 >  1u?*(( int*) _temp589) == 
Cyc_CfFlowInfo_PlaceL: 0){ _LL596: _temp595=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp589)->f1; goto _LL594;} else{ goto _LL590;} _LL592: return({ struct _tuple0
_temp597; _temp597.f1= _temp587; _temp597.f2=( void*) Cyc_CfFlowInfo_NotZeroAll;
_temp597;}); _LL594: return({ struct _tuple0 _temp598; _temp598.f1= _temp587;
_temp598.f2=( void*)({ struct Cyc_CfFlowInfo_AddressOf_struct* _temp599=( struct
Cyc_CfFlowInfo_AddressOf_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_AddressOf_struct));
_temp599[ 0]=({ struct Cyc_CfFlowInfo_AddressOf_struct _temp600; _temp600.tag=
Cyc_CfFlowInfo_AddressOf; _temp600.f1= _temp595; _temp600;}); _temp599;});
_temp598;}); _LL590:;}} _LL251: { void* _temp603; void* _temp605; struct _tuple0
_temp601= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp369); _LL606: _temp605=
_temp601.f1; goto _LL604; _LL604: _temp603= _temp601.f2; goto _LL602; _LL602:
return Cyc_NewControlFlow_anal_derefR( env, inflow, _temp605, _temp369, _temp603);}
_LL253: { void* _temp609; void* _temp611; struct _tuple0 _temp607= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp373); _LL612: _temp611= _temp607.f1; goto _LL610; _LL610:
_temp609= _temp607.f2; goto _LL608; _LL608:{ void* _temp613= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp373->topt))->v); _LL615: if((
unsigned int) _temp613 >  3u?*(( int*) _temp613) ==  Cyc_Absyn_AnonUnionType: 0){
goto _LL616;} else{ goto _LL617;} _LL617: if(( unsigned int) _temp613 >  3u?*((
int*) _temp613) ==  Cyc_Absyn_UnionType: 0){ goto _LL618;} else{ goto _LL619;}
_LL619: goto _LL620; _LL616: goto _LL618; _LL618: return({ struct _tuple0
_temp621; _temp621.f1= _temp611; _temp621.f2= Cyc_CfFlowInfo_typ_to_absrval((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all);
_temp621;}); _LL620: goto _LL614; _LL614:;}{ void* _temp622= _temp609; struct
Cyc_Dict_Dict* _temp628; _LL624: if(( unsigned int) _temp622 >  3u?*(( int*)
_temp622) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL629: _temp628=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp622)->f1; goto _LL625;} else{ goto _LL626;} _LL626: goto _LL627; _LL625:
return({ struct _tuple0 _temp630; _temp630.f1= _temp611; _temp630.f2=(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp628,
_temp371); _temp630;}); _LL627:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp631=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp631[ 0]=({ struct Cyc_Core_Impossible_struct _temp632; _temp632.tag= Cyc_Core_Impossible;
_temp632.f1= _tag_arr("anal_Rexp: StructMember", sizeof( unsigned char), 24u);
_temp632;}); _temp631;})); _LL623:;}} _LL255: { void* _temp635; void* _temp637;
struct _tuple0 _temp633= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp377);
_LL638: _temp637= _temp633.f1; goto _LL636; _LL636: _temp635= _temp633.f2; goto
_LL634; _LL634: { void* _temp641; void* _temp643; struct _tuple0 _temp639= Cyc_NewControlFlow_anal_derefR(
env, inflow, _temp637, _temp377, _temp635); _LL644: _temp643= _temp639.f1; goto
_LL642; _LL642: _temp641= _temp639.f2; goto _LL640; _LL640:{ void* _temp645= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp377->topt))->v); struct Cyc_Absyn_PtrInfo
_temp651; void* _temp653; _LL647: if(( unsigned int) _temp645 >  3u?*(( int*)
_temp645) ==  Cyc_Absyn_PointerType: 0){ _LL652: _temp651=(( struct Cyc_Absyn_PointerType_struct*)
_temp645)->f1; _LL654: _temp653=( void*) _temp651.elt_typ; goto _LL648;} else{
goto _LL649;} _LL649: goto _LL650; _LL648:{ void* _temp655= Cyc_Tcutil_compress(
_temp653); _LL657: if(( unsigned int) _temp655 >  3u?*(( int*) _temp655) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL658;} else{ goto _LL659;} _LL659: if(( unsigned int) _temp655 >  3u?*((
int*) _temp655) ==  Cyc_Absyn_UnionType: 0){ goto _LL660;} else{ goto _LL661;}
_LL661: goto _LL662; _LL658: goto _LL660; _LL660: return({ struct _tuple0
_temp663; _temp663.f1= _temp643; _temp663.f2= Cyc_CfFlowInfo_typ_to_absrval((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all);
_temp663;}); _LL662: goto _LL656; _LL656:;} goto _LL646; _LL650:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp664=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp664[ 0]=({ struct
Cyc_Core_Impossible_struct _temp665; _temp665.tag= Cyc_Core_Impossible; _temp665.f1=
_tag_arr("anal_Rexp: StructArrow ptr", sizeof( unsigned char), 27u); _temp665;});
_temp664;})); _LL646:;}{ void* _temp666= _temp641; struct Cyc_Dict_Dict*
_temp672; _LL668: if(( unsigned int) _temp666 >  3u?*(( int*) _temp666) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL673: _temp672=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp666)->f1;
goto _LL669;} else{ goto _LL670;} _LL670: goto _LL671; _LL669: return({ struct
_tuple0 _temp674; _temp674.f1= _temp643; _temp674.f2=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp672, _temp375); _temp674;});
_LL671:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp675=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp675[ 0]=({ struct Cyc_Core_Impossible_struct _temp676; _temp676.tag= Cyc_Core_Impossible;
_temp676.f1= _tag_arr("anal_Rexp: StructArrow", sizeof( unsigned char), 23u);
_temp676;}); _temp675;})); _LL667:;}}} _LL257: { void* _temp679; void* _temp681;
struct _tuple0 _temp677= Cyc_NewControlFlow_anal_test( env, inflow, _temp383);
_LL682: _temp681= _temp677.f1; goto _LL680; _LL680: _temp679= _temp677.f2; goto
_LL678; _LL678: { void* _temp685; void* _temp687; struct _tuple0 _temp683= Cyc_NewControlFlow_anal_Rexp(
env, _temp681, _temp381); _LL688: _temp687= _temp683.f1; goto _LL686; _LL686:
_temp685= _temp683.f2; goto _LL684; _LL684: { void* _temp691; void* _temp693;
struct _tuple0 _temp689= Cyc_NewControlFlow_anal_Rexp( env, _temp679, _temp379);
_LL694: _temp693= _temp689.f1; goto _LL692; _LL692: _temp691= _temp689.f2; goto
_LL690; _LL690: return({ struct _tuple0 _temp695; _temp695.f1= Cyc_CfFlowInfo_join_flow(
env->all_changed, Cyc_NewControlFlow_use_Rval( env, _temp381->loc, _temp687,
_temp685), Cyc_NewControlFlow_use_Rval( env, _temp379->loc, _temp693, _temp691));
_temp695.f2= Cyc_CfFlowInfo_unknown_all; _temp695;});}}} _LL259: { struct Cyc_List_List*
_temp699; void* _temp701; struct _tuple4 _temp697= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp696[ 2u]; _temp696[ 1u]= _temp385;
_temp696[ 0u]= _temp387;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp696, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL702: _temp701=
_temp697.f1; goto _LL700; _LL700: _temp699= _temp697.f2; goto _LL698; _LL698:{
void* _temp703= _temp701; struct Cyc_Dict_Dict* _temp709; _LL705: if((
unsigned int) _temp703 >  1u?*(( int*) _temp703) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL710: _temp709=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp703)->f1;
goto _LL706;} else{ goto _LL707;} _LL707: goto _LL708; _LL706: if( Cyc_CfFlowInfo_initlevel(
_temp709,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp699))->tl))->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void*
_temp711[ 0u]={}; Cyc_Tcutil_terr( _temp385->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp711, sizeof( void*), 0u));});} goto
_LL704; _LL708: goto _LL704; _LL704:;}{ void* _temp712= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp387->topt))->v); _LL714: if((
unsigned int) _temp712 >  3u?*(( int*) _temp712) ==  Cyc_Absyn_TupleType: 0){
goto _LL715;} else{ goto _LL716;} _LL716: goto _LL717; _LL715: { void* _temp718=(
void*)(( struct Cyc_List_List*) _check_null( _temp699))->hd; struct Cyc_Dict_Dict*
_temp724; _LL720: if(( unsigned int) _temp718 >  3u?*(( int*) _temp718) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL725: _temp724=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp718)->f1;
goto _LL721;} else{ goto _LL722;} _LL722: goto _LL723; _LL721: return({ struct
_tuple0 _temp726; _temp726.f1= _temp701; _temp726.f2=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp724, Cyc_Absyn_fieldname(( int)
Cyc_Evexp_eval_const_uint_exp( _temp385))); _temp726;}); _LL723:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp727=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp727[ 0]=({ struct
Cyc_Core_Impossible_struct _temp728; _temp728.tag= Cyc_Core_Impossible; _temp728.f1=
_tag_arr("anal_Rexp: Subscript", sizeof( unsigned char), 21u); _temp728;});
_temp727;})); _LL719:;} _LL717: goto _LL713; _LL713:;} return Cyc_NewControlFlow_anal_derefR(
env, inflow, _temp701, _temp387,( void*)(( struct Cyc_List_List*) _check_null(
_temp699))->hd);} _LL261: _temp391= _temp389; goto _LL263; _LL263: { struct Cyc_List_List*
_temp731; void* _temp733; struct _tuple4 _temp729= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, _temp391); _LL734: _temp733= _temp729.f1; goto _LL732; _LL732:
_temp731= _temp729.f2; goto _LL730; _LL730: { struct Cyc_Dict_Dict* aggrdict=((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{ int i= 0; for( 0; _temp731 !=  0;(
_temp731= _temp731->tl, ++ i)){ aggrdict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( aggrdict, Cyc_Absyn_fieldname(
i),( void*) _temp731->hd);}} return({ struct _tuple0 _temp735; _temp735.f1=
_temp733; _temp735.f2=( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp736=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp736[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp737; _temp737.tag= Cyc_CfFlowInfo_Aggregate; _temp737.f1= aggrdict;
_temp737;}); _temp736;}); _temp735;});}} _LL265: _temp395= _temp393; goto _LL267;
_LL267: { struct Cyc_List_List* _temp740; void* _temp742; struct _tuple4
_temp738= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple5*)) Cyc_Core_snd, _temp395)); _LL743:
_temp742= _temp738.f1; goto _LL741; _LL741: _temp740= _temp738.f2; goto _LL739;
_LL739: { struct Cyc_Dict_Dict* aggrdict=(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{
int i= 0; for( 0; _temp740 !=  0;((( _temp740= _temp740->tl, _temp395= _temp395->tl)),
++ i)){ struct Cyc_List_List* ds=(*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp395))->hd)).f1; for( 0; ds !=  0; ds= ds->tl){ void* _temp744=(
void*) ds->hd; struct _tagged_arr* _temp750; _LL746: if(*(( int*) _temp744) == 
Cyc_Absyn_ArrayElement){ goto _LL747;} else{ goto _LL748;} _LL748: if(*(( int*)
_temp744) ==  Cyc_Absyn_FieldName){ _LL751: _temp750=(( struct Cyc_Absyn_FieldName_struct*)
_temp744)->f1; goto _LL749;} else{ goto _LL745;} _LL747:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp752=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp752[ 0]=({ struct
Cyc_Core_Impossible_struct _temp753; _temp753.tag= Cyc_Core_Impossible; _temp753.f1=
_tag_arr("anal_Rexp:Struct_e", sizeof( unsigned char), 19u); _temp753;});
_temp752;})); _LL749: aggrdict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( aggrdict, _temp750,( void*)
_temp740->hd); _LL745:;}}} return({ struct _tuple0 _temp754; _temp754.f1=
_temp742; _temp754.f2=( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp755=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp755[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp756; _temp756.tag= Cyc_CfFlowInfo_Aggregate; _temp756.f1= aggrdict;
_temp756;}); _temp755;}); _temp754;});}} _LL269: { struct Cyc_List_List*
_temp757=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple5*),
struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct
_tuple5*)) Cyc_Core_snd, _temp397); struct Cyc_List_List* _temp760; void*
_temp762; struct _tuple4 _temp758= Cyc_NewControlFlow_anal_unordered_Rexps( env,
inflow, _temp757); _LL763: _temp762= _temp758.f1; goto _LL761; _LL761: _temp760=
_temp758.f2; goto _LL759; _LL759: for( 0; _temp760 !=  0;( _temp760= _temp760->tl,
_temp757= _temp757->tl)){ _temp762= Cyc_NewControlFlow_use_Rval( env,(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp757))->hd)->loc,
_temp762,( void*) _temp760->hd);} return({ struct _tuple0 _temp764; _temp764.f1=
_temp762; _temp764.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp764;});} _LL271: {
void* _temp767; void* _temp769; struct _tuple0 _temp765= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp401); _LL770: _temp769= _temp765.f1; goto _LL768; _LL768:
_temp767= _temp765.f2; goto _LL766; _LL766: { void* _temp771= _temp769; struct
Cyc_Dict_Dict* _temp777; _LL773: if( _temp771 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL774;} else{ goto _LL775;} _LL775: if(( unsigned int) _temp771 >  1u?*((
int*) _temp771) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL778: _temp777=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp771)->f1; goto _LL776;} else{ goto
_LL772;} _LL774: return({ struct _tuple0 _temp779; _temp779.f1= _temp769;
_temp779.f2= Cyc_CfFlowInfo_unknown_all; _temp779;}); _LL776: if( Cyc_CfFlowInfo_initlevel(
_temp777, _temp767) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp780[ 0u]={};
Cyc_Tcutil_terr( _temp401->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp780, sizeof( void*), 0u));});}{
void* _temp781= _temp767; _LL783: if( _temp781 == ( void*) Cyc_CfFlowInfo_Zero){
goto _LL784;} else{ goto _LL785;} _LL785: if( _temp781 == ( void*) Cyc_CfFlowInfo_NotZeroThis){
goto _LL786;} else{ goto _LL787;} _LL787: if( _temp781 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL788;} else{ goto _LL789;} _LL789: if(( unsigned int) _temp781 >  3u?*((
int*) _temp781) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL790;} else{ goto
_LL791;} _LL791: goto _LL792; _LL784: return({ struct _tuple0 _temp793; _temp793.f1=
_temp769; _temp793.f2= Cyc_CfFlowInfo_unknown_all; _temp793;}); _LL786: goto
_LL788; _LL788: goto _LL790; _LL790: { struct Cyc_List_List _temp794=({ struct
Cyc_List_List _temp812; _temp812.hd=( void*) _temp403; _temp812.tl= 0; _temp812;});
_temp769= Cyc_NewControlFlow_add_vars( _temp769,( struct Cyc_List_List*)&
_temp794, Cyc_CfFlowInfo_unknown_all);{ void* _temp797; void* _temp799; struct
_tuple0 _temp795= Cyc_NewControlFlow_anal_Rexp( env, _temp769, _temp399); _LL800:
_temp799= _temp795.f1; goto _LL798; _LL798: _temp797= _temp795.f2; goto _LL796;
_LL796:{ void* _temp801= _temp799; struct Cyc_Dict_Dict* _temp807; _LL803: if(
_temp801 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL804;} else{ goto _LL805;}
_LL805: if(( unsigned int) _temp801 >  1u?*(( int*) _temp801) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL808: _temp807=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp801)->f1;
goto _LL806;} else{ goto _LL802;} _LL804: return({ struct _tuple0 _temp809;
_temp809.f1= _temp799; _temp809.f2= Cyc_CfFlowInfo_unknown_all; _temp809;});
_LL806: if( Cyc_CfFlowInfo_initlevel( _temp807, _temp797) != ( void*) Cyc_CfFlowInfo_AllIL){({
void* _temp810[ 0u]={}; Cyc_Tcutil_terr( _temp401->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp810, sizeof( void*), 0u));});
return({ struct _tuple0 _temp811; _temp811.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp811.f2= Cyc_CfFlowInfo_unknown_all; _temp811;});} _LL802:;} _temp769=
_temp799; goto _LL792;}} _LL792: while( 1) { struct Cyc_List_List _temp813=({
struct Cyc_List_List _temp831; _temp831.hd=( void*) _temp403; _temp831.tl= 0;
_temp831;}); _temp769= Cyc_NewControlFlow_add_vars( _temp769,( struct Cyc_List_List*)&
_temp813, Cyc_CfFlowInfo_unknown_all);{ void* _temp816; void* _temp818; struct
_tuple0 _temp814= Cyc_NewControlFlow_anal_Rexp( env, _temp769, _temp399); _LL819:
_temp818= _temp814.f1; goto _LL817; _LL817: _temp816= _temp814.f2; goto _LL815;
_LL815:{ void* _temp820= _temp818; struct Cyc_Dict_Dict* _temp826; _LL822: if(
_temp820 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL823;} else{ goto _LL824;}
_LL824: if(( unsigned int) _temp820 >  1u?*(( int*) _temp820) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL827: _temp826=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp820)->f1;
goto _LL825;} else{ goto _LL821;} _LL823: goto _LL821; _LL825: if( Cyc_CfFlowInfo_initlevel(
_temp826, _temp816) != ( void*) Cyc_CfFlowInfo_AllIL){({ void* _temp828[ 0u]={};
Cyc_Tcutil_terr( _temp401->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp828, sizeof( void*), 0u));});
return({ struct _tuple0 _temp829; _temp829.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp829.f2= Cyc_CfFlowInfo_unknown_all; _temp829;});} _LL821:;}{ void* _temp830=
Cyc_CfFlowInfo_join_flow( env->all_changed, _temp769, _temp818); if( Cyc_CfFlowInfo_flow_lessthan_approx(
_temp830, _temp769)){ break;} _temp769= _temp830;}}} return({ struct _tuple0
_temp832; _temp832.f1= _temp769; _temp832.f2= Cyc_CfFlowInfo_unknown_all;
_temp832;}); _LL782:;} _LL772:;}} _LL273: while( 1) { inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, inflow, _temp405))->flow;{ void* _temp833=( void*) _temp405->r; struct Cyc_Absyn_Stmt*
_temp843; struct Cyc_Absyn_Stmt* _temp845; struct Cyc_Absyn_Stmt* _temp847;
struct Cyc_Absyn_Decl* _temp849; struct Cyc_Absyn_Exp* _temp851; _LL835: if((
unsigned int) _temp833 >  1u?*(( int*) _temp833) ==  Cyc_Absyn_Seq_s: 0){ _LL846:
_temp845=(( struct Cyc_Absyn_Seq_s_struct*) _temp833)->f1; goto _LL844; _LL844:
_temp843=(( struct Cyc_Absyn_Seq_s_struct*) _temp833)->f2; goto _LL836;} else{
goto _LL837;} _LL837: if(( unsigned int) _temp833 >  1u?*(( int*) _temp833) == 
Cyc_Absyn_Decl_s: 0){ _LL850: _temp849=(( struct Cyc_Absyn_Decl_s_struct*)
_temp833)->f1; goto _LL848; _LL848: _temp847=(( struct Cyc_Absyn_Decl_s_struct*)
_temp833)->f2; goto _LL838;} else{ goto _LL839;} _LL839: if(( unsigned int)
_temp833 >  1u?*(( int*) _temp833) ==  Cyc_Absyn_Exp_s: 0){ _LL852: _temp851=((
struct Cyc_Absyn_Exp_s_struct*) _temp833)->f1; goto _LL840;} else{ goto _LL841;}
_LL841: goto _LL842; _LL836: inflow= Cyc_NewControlFlow_anal_stmt( env, inflow,
_temp845); _temp405= _temp843; goto _LL834; _LL838: inflow= Cyc_NewControlFlow_anal_decl(
env, inflow, _temp849); _temp405= _temp847; goto _LL834; _LL840: return Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp851); _LL842:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp853=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp853[ 0]=({ struct Cyc_Core_Impossible_struct _temp854; _temp854.tag= Cyc_Core_Impossible;
_temp854.f1= _tag_arr("analyze_Rexp: ill-formed StmtExp", sizeof( unsigned char),
33u); _temp854;}); _temp853;})); _LL834:;}} _LL275: goto _LL277; _LL277: goto
_LL279; _LL279: goto _LL281; _LL281: goto _LL283; _LL283: goto _LL285; _LL285:
goto _LL287; _LL287:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp855=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp855[ 0]=({ struct Cyc_Core_Impossible_struct _temp856; _temp856.tag= Cyc_Core_Impossible;
_temp856.f1= _tag_arr("anal_Rexp, unexpected exp form", sizeof( unsigned char),
31u); _temp856;}); _temp855;})); _LL191:;}} static struct _tuple0 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, void* f, struct Cyc_Absyn_Exp*
e, void* r, struct Cyc_List_List* flds){ void* _temp857= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo
_temp863; struct Cyc_Absyn_Conref* _temp865; void* _temp867; _LL859: if((
unsigned int) _temp857 >  3u?*(( int*) _temp857) ==  Cyc_Absyn_PointerType: 0){
_LL864: _temp863=(( struct Cyc_Absyn_PointerType_struct*) _temp857)->f1; _LL868:
_temp867=( void*) _temp863.elt_typ; goto _LL866; _LL866: _temp865= _temp863.bounds;
goto _LL860;} else{ goto _LL861;} _LL861: goto _LL862; _LL860: { void* _temp869=
f; struct Cyc_Dict_Dict* _temp875; _LL871: if( _temp869 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL872;} else{ goto _LL873;} _LL873: if(( unsigned int) _temp869 >  1u?*((
int*) _temp869) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL876: _temp875=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp869)->f1; goto _LL874;} else{ goto
_LL870;} _LL872: return({ struct _tuple0 _temp877; _temp877.f1= f; _temp877.f2=(
void*) Cyc_CfFlowInfo_UnknownL; _temp877;}); _LL874: if( Cyc_Tcutil_is_bound_one(
_temp865)){ void* _temp878= r; struct Cyc_CfFlowInfo_Place* _temp892; struct Cyc_CfFlowInfo_Place
_temp894; struct Cyc_List_List* _temp895; void* _temp897; void* _temp899; _LL880:
if( _temp878 == ( void*) Cyc_CfFlowInfo_NotZeroAll){ goto _LL881;} else{ goto
_LL882;} _LL882: if( _temp878 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto
_LL883;} else{ goto _LL884;} _LL884: if(( unsigned int) _temp878 >  3u?*(( int*)
_temp878) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL893: _temp892=(( struct Cyc_CfFlowInfo_AddressOf_struct*)
_temp878)->f1; _temp894=* _temp892; _LL898: _temp897=( void*) _temp894.root;
goto _LL896; _LL896: _temp895= _temp894.fields; goto _LL885;} else{ goto _LL886;}
_LL886: if( _temp878 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL887;} else{ goto
_LL888;} _LL888: if(( unsigned int) _temp878 >  3u?*(( int*) _temp878) ==  Cyc_CfFlowInfo_UnknownR:
0){ _LL900: _temp899=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*) _temp878)->f1;
goto _LL889;} else{ goto _LL890;} _LL890: goto _LL891; _LL881: goto _LL883;
_LL883:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_NotZero)); goto
_LL879; _LL885:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_NotZero));
return({ struct _tuple0 _temp901; _temp901.f1= f; _temp901.f2=( void*)({ struct
Cyc_CfFlowInfo_PlaceL_struct* _temp902=( struct Cyc_CfFlowInfo_PlaceL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_PlaceL_struct)); _temp902[ 0]=({ struct
Cyc_CfFlowInfo_PlaceL_struct _temp903; _temp903.tag= Cyc_CfFlowInfo_PlaceL;
_temp903.f1=({ struct Cyc_CfFlowInfo_Place* _temp904=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp904->root=( void*)
_temp897; _temp904->fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( _temp895, flds); _temp904;});
_temp903;}); _temp902;}); _temp901;}); _LL887:( void*)( e->annot=( void*)(( void*)
Cyc_CfFlowInfo_IsZero)); return({ struct _tuple0 _temp905; _temp905.f1=( void*)
Cyc_CfFlowInfo_BottomFL; _temp905.f2=( void*) Cyc_CfFlowInfo_UnknownL; _temp905;});
_LL889: f= Cyc_NewControlFlow_notzero( env, inflow, f, e, _temp899); goto _LL891;
_LL891:( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_UnknownZ)); _LL879:;}
else{( void*)( e->annot=( void*)(( void*) Cyc_CfFlowInfo_UnknownZ));} if( Cyc_CfFlowInfo_initlevel(
_temp875, r) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp906[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("dereference of possibly uninitialized pointer", sizeof(
unsigned char), 46u), _tag_arr( _temp906, sizeof( void*), 0u));});} return({
struct _tuple0 _temp907; _temp907.f1= f; _temp907.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp907;}); _LL870:;} _LL862:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp908=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp908[ 0]=({ struct Cyc_Core_Impossible_struct _temp909; _temp909.tag= Cyc_Core_Impossible;
_temp909.f1= _tag_arr("left deref of non-pointer-type", sizeof( unsigned char),
31u); _temp909;}); _temp908;})); _LL858:;} static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* flds){ struct Cyc_Dict_Dict* d;{ void* _temp910= inflow;
struct Cyc_Dict_Dict* _temp916; _LL912: if( _temp910 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL913;} else{ goto _LL914;} _LL914: if(( unsigned int) _temp910 >  1u?*((
int*) _temp910) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL917: _temp916=(( struct
Cyc_CfFlowInfo_ReachableFL_struct*) _temp910)->f1; goto _LL915;} else{ goto
_LL911;} _LL913: return({ struct _tuple0 _temp918; _temp918.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp918.f2=( void*) Cyc_CfFlowInfo_UnknownL; _temp918;}); _LL915: d= _temp916;
_LL911:;}{ void* _temp919=( void*) e->r; struct Cyc_Absyn_Exp* _temp945; struct
Cyc_Absyn_Exp* _temp947; struct Cyc_Absyn_Exp* _temp949; void* _temp951; struct
Cyc_Absyn_Vardecl* _temp953; void* _temp955; struct Cyc_Absyn_Vardecl* _temp957;
void* _temp959; struct Cyc_Absyn_Vardecl* _temp961; void* _temp963; struct Cyc_Absyn_Vardecl*
_temp965; struct _tagged_arr* _temp967; struct Cyc_Absyn_Exp* _temp969; struct
Cyc_Absyn_Exp* _temp971; struct Cyc_Absyn_Exp* _temp973; struct Cyc_Absyn_Exp*
_temp975; struct _tagged_arr* _temp977; struct Cyc_Absyn_Exp* _temp979; _LL921:
if(*(( int*) _temp919) ==  Cyc_Absyn_Cast_e){ _LL946: _temp945=(( struct Cyc_Absyn_Cast_e_struct*)
_temp919)->f2; goto _LL922;} else{ goto _LL923;} _LL923: if(*(( int*) _temp919)
==  Cyc_Absyn_NoInstantiate_e){ _LL948: _temp947=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp919)->f1; goto _LL924;} else{ goto _LL925;} _LL925: if(*(( int*) _temp919)
==  Cyc_Absyn_Instantiate_e){ _LL950: _temp949=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp919)->f1; goto _LL926;} else{ goto _LL927;} _LL927: if(*(( int*) _temp919)
==  Cyc_Absyn_Var_e){ _LL952: _temp951=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp919)->f2; if(( unsigned int) _temp951 >  1u?*(( int*) _temp951) ==  Cyc_Absyn_Param_b:
0){ _LL954: _temp953=(( struct Cyc_Absyn_Param_b_struct*) _temp951)->f1; goto
_LL928;} else{ goto _LL929;}} else{ goto _LL929;} _LL929: if(*(( int*) _temp919)
==  Cyc_Absyn_Var_e){ _LL956: _temp955=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp919)->f2; if(( unsigned int) _temp955 >  1u?*(( int*) _temp955) ==  Cyc_Absyn_Local_b:
0){ _LL958: _temp957=(( struct Cyc_Absyn_Local_b_struct*) _temp955)->f1; goto
_LL930;} else{ goto _LL931;}} else{ goto _LL931;} _LL931: if(*(( int*) _temp919)
==  Cyc_Absyn_Var_e){ _LL960: _temp959=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp919)->f2; if(( unsigned int) _temp959 >  1u?*(( int*) _temp959) ==  Cyc_Absyn_Pat_b:
0){ _LL962: _temp961=(( struct Cyc_Absyn_Pat_b_struct*) _temp959)->f1; goto
_LL932;} else{ goto _LL933;}} else{ goto _LL933;} _LL933: if(*(( int*) _temp919)
==  Cyc_Absyn_Var_e){ _LL964: _temp963=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp919)->f2; if(( unsigned int) _temp963 >  1u?*(( int*) _temp963) ==  Cyc_Absyn_Global_b:
0){ _LL966: _temp965=(( struct Cyc_Absyn_Global_b_struct*) _temp963)->f1; goto
_LL934;} else{ goto _LL935;}} else{ goto _LL935;} _LL935: if(*(( int*) _temp919)
==  Cyc_Absyn_StructArrow_e){ _LL970: _temp969=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp919)->f1; goto _LL968; _LL968: _temp967=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp919)->f2; goto _LL936;} else{ goto _LL937;} _LL937: if(*(( int*) _temp919)
==  Cyc_Absyn_Deref_e){ _LL972: _temp971=(( struct Cyc_Absyn_Deref_e_struct*)
_temp919)->f1; goto _LL938;} else{ goto _LL939;} _LL939: if(*(( int*) _temp919)
==  Cyc_Absyn_Subscript_e){ _LL976: _temp975=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp919)->f1; goto _LL974; _LL974: _temp973=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp919)->f2; goto _LL940;} else{ goto _LL941;} _LL941: if(*(( int*) _temp919)
==  Cyc_Absyn_StructMember_e){ _LL980: _temp979=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp919)->f1; goto _LL978; _LL978: _temp977=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp919)->f2; goto _LL942;} else{ goto _LL943;} _LL943: goto _LL944; _LL922:
_temp947= _temp945; goto _LL924; _LL924: _temp949= _temp947; goto _LL926; _LL926:
return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, _temp949, flds); _LL928:
_temp957= _temp953; goto _LL930; _LL930: _temp961= _temp957; goto _LL932; _LL932:
return({ struct _tuple0 _temp981; _temp981.f1= inflow; _temp981.f2=( void*)({
struct Cyc_CfFlowInfo_PlaceL_struct* _temp982=( struct Cyc_CfFlowInfo_PlaceL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_PlaceL_struct)); _temp982[ 0]=({ struct
Cyc_CfFlowInfo_PlaceL_struct _temp983; _temp983.tag= Cyc_CfFlowInfo_PlaceL;
_temp983.f1=({ struct Cyc_CfFlowInfo_Place* _temp984=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp984->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp985=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp985[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp986; _temp986.tag= Cyc_CfFlowInfo_VarRoot;
_temp986.f1= _temp961; _temp986;}); _temp985;})); _temp984->fields= flds;
_temp984;}); _temp983;}); _temp982;}); _temp981;}); _LL934: return({ struct
_tuple0 _temp987; _temp987.f1= inflow; _temp987.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp987;}); _LL936:{ void* _temp988= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp969->topt))->v); _LL990: if(( unsigned int) _temp988 >  3u?*((
int*) _temp988) ==  Cyc_Absyn_AnonUnionType: 0){ goto _LL991;} else{ goto _LL992;}
_LL992: if(( unsigned int) _temp988 >  3u?*(( int*) _temp988) ==  Cyc_Absyn_UnionType:
0){ goto _LL993;} else{ goto _LL994;} _LL994: goto _LL995; _LL991: goto _LL993;
_LL993: goto _LL989; _LL995: flds=({ struct Cyc_List_List* _temp996=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp996->hd=( void*) _temp967;
_temp996->tl= flds; _temp996;}); _LL989:;} _temp971= _temp969; goto _LL938;
_LL938: { void* _temp999; void* _temp1001; struct _tuple0 _temp997= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp971); _LL1002: _temp1001= _temp997.f1; goto _LL1000; _LL1000:
_temp999= _temp997.f2; goto _LL998; _LL998: return Cyc_NewControlFlow_anal_derefL(
env, inflow, _temp1001, _temp971, _temp999, flds);} _LL940:{ void* _temp1003=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp975->topt))->v);
_LL1005: if(( unsigned int) _temp1003 >  3u?*(( int*) _temp1003) ==  Cyc_Absyn_TupleType:
0){ goto _LL1006;} else{ goto _LL1007;} _LL1007: goto _LL1008; _LL1006: { struct
_tagged_arr* _temp1009= Cyc_Absyn_fieldname(( int) Cyc_Evexp_eval_const_uint_exp(
_temp973)); return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, _temp975,({
struct Cyc_List_List* _temp1010=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1010->hd=( void*) _temp1009; _temp1010->tl= flds;
_temp1010;}));} _LL1008: goto _LL1004; _LL1004:;}{ struct Cyc_List_List*
_temp1014; void* _temp1016; struct _tuple4 _temp1012= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp1011[ 2u]; _temp1011[ 1u]= _temp973;
_temp1011[ 0u]= _temp975;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1011, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL1017:
_temp1016= _temp1012.f1; goto _LL1015; _LL1015: _temp1014= _temp1012.f2; goto
_LL1013; _LL1013:{ void* _temp1018= _temp1016; struct Cyc_Dict_Dict* _temp1024;
_LL1020: if(( unsigned int) _temp1018 >  1u?*(( int*) _temp1018) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL1025: _temp1024=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1018)->f1;
goto _LL1021;} else{ goto _LL1022;} _LL1022: goto _LL1023; _LL1021: if( Cyc_CfFlowInfo_initlevel(
_temp1024,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1014))->tl))->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void*
_temp1026[ 0u]={}; Cyc_Tcutil_terr( _temp973->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1026, sizeof( void*), 0u));});}
goto _LL1019; _LL1023: goto _LL1019; _LL1019:;} return Cyc_NewControlFlow_anal_derefL(
env, inflow, _temp1016, _temp975,( void*)(( struct Cyc_List_List*) _check_null(
_temp1014))->hd, flds);} _LL942:{ void* _temp1027= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp979->topt))->v); _LL1029: if((
unsigned int) _temp1027 >  3u?*(( int*) _temp1027) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL1030;} else{ goto _LL1031;} _LL1031: if(( unsigned int) _temp1027 > 
3u?*(( int*) _temp1027) ==  Cyc_Absyn_UnionType: 0){ goto _LL1032;} else{ goto
_LL1033;} _LL1033: goto _LL1034; _LL1030: goto _LL1032; _LL1032: return({ struct
_tuple0 _temp1035; _temp1035.f1= inflow; _temp1035.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp1035;}); _LL1034: flds=({ struct Cyc_List_List* _temp1036=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1036->hd=( void*) _temp977;
_temp1036->tl= flds; _temp1036;}); _LL1028:;} return Cyc_NewControlFlow_anal_Lexp_rec(
env, inflow, _temp979, flds); _LL944: return({ struct _tuple0 _temp1037;
_temp1037.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp1037.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp1037;}); _LL920:;}} static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e){
return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, e, 0);} static struct
_tuple0 Cyc_NewControlFlow_anal_test( struct Cyc_NewControlFlow_AnalEnv* env,
void* inflow, struct Cyc_Absyn_Exp* e){{ void* _temp1038=( void*) e->r; struct
Cyc_Absyn_Exp* _temp1050; struct Cyc_Absyn_Exp* _temp1052; struct Cyc_Absyn_Exp*
_temp1054; struct Cyc_List_List* _temp1056; struct Cyc_List_List _temp1058;
struct Cyc_List_List* _temp1059; struct Cyc_Absyn_Exp* _temp1061; void*
_temp1063; struct Cyc_List_List* _temp1065; void* _temp1067; struct Cyc_List_List*
_temp1069; void* _temp1071; _LL1040: if(*(( int*) _temp1038) ==  Cyc_Absyn_Conditional_e){
_LL1055: _temp1054=(( struct Cyc_Absyn_Conditional_e_struct*) _temp1038)->f1;
goto _LL1053; _LL1053: _temp1052=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1038)->f2; goto _LL1051; _LL1051: _temp1050=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1038)->f3; goto _LL1041;} else{ goto _LL1042;} _LL1042: if(*(( int*)
_temp1038) ==  Cyc_Absyn_Primop_e){ _LL1064: _temp1063=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1038)->f1; if( _temp1063 == ( void*) Cyc_Absyn_Not){ goto _LL1057;} else{
goto _LL1044;} _LL1057: _temp1056=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1038)->f2; if( _temp1056 ==  0){ goto _LL1044;} else{ _temp1058=* _temp1056;
_LL1062: _temp1061=( struct Cyc_Absyn_Exp*) _temp1058.hd; goto _LL1060; _LL1060:
_temp1059= _temp1058.tl; if( _temp1059 ==  0){ goto _LL1043;} else{ goto _LL1044;}}}
else{ goto _LL1044;} _LL1044: if(*(( int*) _temp1038) ==  Cyc_Absyn_Primop_e){
_LL1068: _temp1067=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1038)->f1;
if( _temp1067 == ( void*) Cyc_Absyn_Eq){ goto _LL1066;} else{ goto _LL1046;}
_LL1066: _temp1065=(( struct Cyc_Absyn_Primop_e_struct*) _temp1038)->f2; goto
_LL1045;} else{ goto _LL1046;} _LL1046: if(*(( int*) _temp1038) ==  Cyc_Absyn_Primop_e){
_LL1072: _temp1071=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1038)->f1;
if( _temp1071 == ( void*) Cyc_Absyn_Neq){ goto _LL1070;} else{ goto _LL1048;}
_LL1070: _temp1069=(( struct Cyc_Absyn_Primop_e_struct*) _temp1038)->f2; goto
_LL1047;} else{ goto _LL1048;} _LL1048: goto _LL1049; _LL1041: { void* _temp1075;
void* _temp1077; struct _tuple0 _temp1073= Cyc_NewControlFlow_anal_test( env,
inflow, _temp1054); _LL1078: _temp1077= _temp1073.f1; goto _LL1076; _LL1076:
_temp1075= _temp1073.f2; goto _LL1074; _LL1074: { void* _temp1081; void*
_temp1083; struct _tuple0 _temp1079= Cyc_NewControlFlow_anal_test( env,
_temp1077, _temp1052); _LL1084: _temp1083= _temp1079.f1; goto _LL1082; _LL1082:
_temp1081= _temp1079.f2; goto _LL1080; _LL1080: { void* _temp1087; void*
_temp1089; struct _tuple0 _temp1085= Cyc_NewControlFlow_anal_test( env,
_temp1075, _temp1050); _LL1090: _temp1089= _temp1085.f1; goto _LL1088; _LL1088:
_temp1087= _temp1085.f2; goto _LL1086; _LL1086: return({ struct _tuple0
_temp1091; _temp1091.f1= Cyc_CfFlowInfo_join_flow( env->all_changed, _temp1083,
_temp1089); _temp1091.f2= Cyc_CfFlowInfo_join_flow( env->all_changed, _temp1081,
_temp1087); _temp1091;});}}} _LL1043: { void* _temp1094; void* _temp1096; struct
_tuple0 _temp1092= Cyc_NewControlFlow_anal_test( env, inflow, _temp1061);
_LL1097: _temp1096= _temp1092.f1; goto _LL1095; _LL1095: _temp1094= _temp1092.f2;
goto _LL1093; _LL1093: return({ struct _tuple0 _temp1098; _temp1098.f1=
_temp1094; _temp1098.f2= _temp1096; _temp1098;});} _LL1045: { struct Cyc_List_List*
_temp1101; void* _temp1103; struct _tuple4 _temp1099= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, _temp1065); _LL1104: _temp1103= _temp1099.f1; goto _LL1102; _LL1102:
_temp1101= _temp1099.f2; goto _LL1100; _LL1100: { void* _temp1105= _temp1103;
struct Cyc_Dict_Dict* _temp1111; _LL1107: if( _temp1105 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1108;} else{ goto _LL1109;} _LL1109: if(( unsigned int) _temp1105 >  1u?*((
int*) _temp1105) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1112: _temp1111=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1105)->f1; goto _LL1110;} else{
goto _LL1106;} _LL1108: return({ struct _tuple0 _temp1113; _temp1113.f1=
_temp1103; _temp1113.f2= _temp1103; _temp1113;}); _LL1110: { void* _temp1114=(
void*)(( struct Cyc_List_List*) _check_null( _temp1101))->hd; void* _temp1115=(
void*)(( struct Cyc_List_List*) _check_null( _temp1101->tl))->hd; if( Cyc_CfFlowInfo_initlevel(
_temp1111, _temp1114) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1116[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1065))->hd)->loc, _tag_arr("expression may not be initialized", sizeof(
unsigned char), 34u), _tag_arr( _temp1116, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1111, _temp1115) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1117[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1065))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1117, sizeof( void*), 0u));});}{
struct _tuple0 _temp1119=({ struct _tuple0 _temp1118; _temp1118.f1= _temp1114;
_temp1118.f2= _temp1115; _temp1118;}); void* _temp1127; void* _temp1129; void*
_temp1131; void* _temp1133; void* _temp1135; void* _temp1137; _LL1121: _LL1130:
_temp1129= _temp1119.f1; if(( unsigned int) _temp1129 >  3u?*(( int*) _temp1129)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1132: _temp1131=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1129)->f1; goto _LL1128;} else{ goto _LL1123;} _LL1128: _temp1127=
_temp1119.f2; if( _temp1127 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1122;}
else{ goto _LL1123;} _LL1123: _LL1138: _temp1137= _temp1119.f1; if( _temp1137 == (
void*) Cyc_CfFlowInfo_Zero){ goto _LL1134;} else{ goto _LL1125;} _LL1134:
_temp1133= _temp1119.f2; if(( unsigned int) _temp1133 >  3u?*(( int*) _temp1133)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1136: _temp1135=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1133)->f1; goto _LL1124;} else{ goto _LL1125;} _LL1125: goto _LL1126;
_LL1122: { void* _temp1141; void* _temp1143; struct _tuple0 _temp1139= Cyc_NewControlFlow_splitzero(
env, _temp1103, _temp1103,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1065))->hd, _temp1131); _LL1144: _temp1143= _temp1139.f1; goto
_LL1142; _LL1142: _temp1141= _temp1139.f2; goto _LL1140; _LL1140: return({
struct _tuple0 _temp1145; _temp1145.f1= _temp1141; _temp1145.f2= _temp1143;
_temp1145;});} _LL1124: { void* _temp1148; void* _temp1150; struct _tuple0
_temp1146= Cyc_NewControlFlow_splitzero( env, _temp1103, _temp1103,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1065))->tl))->hd, _temp1135); _LL1151: _temp1150= _temp1146.f1; goto
_LL1149; _LL1149: _temp1148= _temp1146.f2; goto _LL1147; _LL1147: return({
struct _tuple0 _temp1152; _temp1152.f1= _temp1148; _temp1152.f2= _temp1150;
_temp1152;});} _LL1126: return({ struct _tuple0 _temp1153; _temp1153.f1=
_temp1103; _temp1153.f2= _temp1103; _temp1153;}); _LL1120:;}} _LL1106:;}}
_LL1047: { struct Cyc_List_List* _temp1156; void* _temp1158; struct _tuple4
_temp1154= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1069);
_LL1159: _temp1158= _temp1154.f1; goto _LL1157; _LL1157: _temp1156= _temp1154.f2;
goto _LL1155; _LL1155: { void* _temp1160= _temp1158; struct Cyc_Dict_Dict*
_temp1166; _LL1162: if( _temp1160 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1163;} else{ goto _LL1164;} _LL1164: if(( unsigned int) _temp1160 >  1u?*((
int*) _temp1160) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1167: _temp1166=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1160)->f1; goto _LL1165;} else{
goto _LL1161;} _LL1163: return({ struct _tuple0 _temp1168; _temp1168.f1=
_temp1158; _temp1168.f2= _temp1158; _temp1168;}); _LL1165: { void* _temp1169=(
void*)(( struct Cyc_List_List*) _check_null( _temp1156))->hd; void* _temp1170=(
void*)(( struct Cyc_List_List*) _check_null( _temp1156->tl))->hd; if( Cyc_CfFlowInfo_initlevel(
_temp1166, _temp1169) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1171[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
_temp1069))->hd)->loc, _tag_arr("expression may not be initialized", sizeof(
unsigned char), 34u), _tag_arr( _temp1171, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1166, _temp1170) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1172[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1069))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1172, sizeof( void*), 0u));});}{
struct _tuple0 _temp1174=({ struct _tuple0 _temp1173; _temp1173.f1= _temp1169;
_temp1173.f2= _temp1170; _temp1173;}); void* _temp1182; void* _temp1184; void*
_temp1186; void* _temp1188; void* _temp1190; void* _temp1192; _LL1176: _LL1185:
_temp1184= _temp1174.f1; if(( unsigned int) _temp1184 >  3u?*(( int*) _temp1184)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1187: _temp1186=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1184)->f1; goto _LL1183;} else{ goto _LL1178;} _LL1183: _temp1182=
_temp1174.f2; if( _temp1182 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1177;}
else{ goto _LL1178;} _LL1178: _LL1193: _temp1192= _temp1174.f1; if( _temp1192 == (
void*) Cyc_CfFlowInfo_Zero){ goto _LL1189;} else{ goto _LL1180;} _LL1189:
_temp1188= _temp1174.f2; if(( unsigned int) _temp1188 >  3u?*(( int*) _temp1188)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1191: _temp1190=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1188)->f1; goto _LL1179;} else{ goto _LL1180;} _LL1180: goto _LL1181;
_LL1177: return Cyc_NewControlFlow_splitzero( env, _temp1158, _temp1158,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1069))->hd, _temp1186);
_LL1179: return Cyc_NewControlFlow_splitzero( env, _temp1158, _temp1158,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1069))->tl))->hd, _temp1190); _LL1181: return({ struct _tuple0
_temp1194; _temp1194.f1= _temp1158; _temp1194.f2= _temp1158; _temp1194;});
_LL1175:;}} _LL1161:;}} _LL1049: goto _LL1039; _LL1039:;}{ void* _temp1197; void*
_temp1199; struct _tuple0 _temp1195= Cyc_NewControlFlow_anal_Rexp( env, inflow,
e); _LL1200: _temp1199= _temp1195.f1; goto _LL1198; _LL1198: _temp1197=
_temp1195.f2; goto _LL1196; _LL1196: { void* _temp1201= _temp1199; struct Cyc_Dict_Dict*
_temp1207; _LL1203: if( _temp1201 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1204;} else{ goto _LL1205;} _LL1205: if(( unsigned int) _temp1201 >  1u?*((
int*) _temp1201) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1208: _temp1207=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1201)->f1; goto _LL1206;} else{
goto _LL1202;} _LL1204: return({ struct _tuple0 _temp1209; _temp1209.f1=
_temp1199; _temp1209.f2= _temp1199; _temp1209;}); _LL1206: { void* _temp1210=
_temp1197; void* _temp1230; void* _temp1232; void* _temp1234; _LL1212: if(
_temp1210 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1213;} else{ goto _LL1214;}
_LL1214: if( _temp1210 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto _LL1215;}
else{ goto _LL1216;} _LL1216: if( _temp1210 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL1217;} else{ goto _LL1218;} _LL1218: if(( unsigned int) _temp1210 >  3u?*((
int*) _temp1210) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL1219;} else{ goto
_LL1220;} _LL1220: if(( unsigned int) _temp1210 >  3u?*(( int*) _temp1210) == 
Cyc_CfFlowInfo_UnknownR: 0){ _LL1231: _temp1230=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1210)->f1; if( _temp1230 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL1221;}
else{ goto _LL1222;}} else{ goto _LL1222;} _LL1222: if(( unsigned int) _temp1210
>  3u?*(( int*) _temp1210) ==  Cyc_CfFlowInfo_Esc: 0){ _LL1233: _temp1232=( void*)((
struct Cyc_CfFlowInfo_Esc_struct*) _temp1210)->f1; if( _temp1232 == ( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL1223;} else{ goto _LL1224;}} else{ goto _LL1224;} _LL1224: if((
unsigned int) _temp1210 >  3u?*(( int*) _temp1210) ==  Cyc_CfFlowInfo_UnknownR:
0){ _LL1235: _temp1234=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1210)->f1; goto _LL1225;} else{ goto _LL1226;} _LL1226: if(( unsigned int)
_temp1210 >  3u?*(( int*) _temp1210) ==  Cyc_CfFlowInfo_Esc: 0){ goto _LL1227;}
else{ goto _LL1228;} _LL1228: if(( unsigned int) _temp1210 >  3u?*(( int*)
_temp1210) ==  Cyc_CfFlowInfo_Aggregate: 0){ goto _LL1229;} else{ goto _LL1211;}
_LL1213: return({ struct _tuple0 _temp1236; _temp1236.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1236.f2= _temp1199; _temp1236;}); _LL1215: goto _LL1217; _LL1217: goto
_LL1219; _LL1219: return({ struct _tuple0 _temp1237; _temp1237.f1= _temp1199;
_temp1237.f2=( void*) Cyc_CfFlowInfo_BottomFL; _temp1237;}); _LL1221: goto
_LL1223; _LL1223:({ void* _temp1238[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1238, sizeof( void*), 0u));});
return({ struct _tuple0 _temp1239; _temp1239.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1239.f2=( void*) Cyc_CfFlowInfo_BottomFL; _temp1239;}); _LL1225: return Cyc_NewControlFlow_splitzero(
env, inflow, _temp1199, e, _temp1234); _LL1227: return({ struct _tuple0
_temp1240; _temp1240.f1= _temp1199; _temp1240.f2= _temp1199; _temp1240;});
_LL1229:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1241=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1241[ 0]=({ struct Cyc_Core_Impossible_struct _temp1242; _temp1242.tag= Cyc_Core_Impossible;
_temp1242.f1= _tag_arr("anal_test", sizeof( unsigned char), 10u); _temp1242;});
_temp1241;})); _LL1211:;} _LL1202:;}}} static void* Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_List_List* scs){
for( 0; scs !=  0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause _temp1245;
struct Cyc_Absyn_Stmt* _temp1246; struct Cyc_Absyn_Exp* _temp1248; struct Cyc_Core_Opt*
_temp1250; struct Cyc_Absyn_Switch_clause* _temp1243=( struct Cyc_Absyn_Switch_clause*)
scs->hd; _temp1245=* _temp1243; _LL1251: _temp1250= _temp1245.pat_vars; goto
_LL1249; _LL1249: _temp1248= _temp1245.where_clause; goto _LL1247; _LL1247:
_temp1246= _temp1245.body; goto _LL1244; _LL1244: { void* clause_inflow= Cyc_NewControlFlow_add_vars(
inflow,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1250))->v,
Cyc_CfFlowInfo_unknown_all); void* clause_outflow; if( _temp1248 !=  0){ void*
_temp1254; void* _temp1256; struct _tuple0 _temp1252= Cyc_NewControlFlow_anal_test(
env, clause_inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp1248)); _LL1257:
_temp1256= _temp1252.f1; goto _LL1255; _LL1255: _temp1254= _temp1252.f2; goto
_LL1253; _LL1253: inflow= _temp1254; clause_outflow= Cyc_NewControlFlow_anal_stmt(
env, _temp1256, _temp1246);} else{ clause_outflow= Cyc_NewControlFlow_anal_stmt(
env, clause_inflow, _temp1246);}{ void* _temp1258= clause_outflow; _LL1260: if(
_temp1258 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1261;} else{ goto
_LL1262;} _LL1262: goto _LL1263; _LL1261: goto _LL1259; _LL1263: if( scs->tl == 
0){ return clause_outflow;} else{ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs->tl))->hd)->pat_vars))->v !=  0){({ void* _temp1264[ 0u]={};
Cyc_Tcutil_terr( _temp1246->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp1264, sizeof( void*), 0u));});}
else{({ void* _temp1265[ 0u]={}; Cyc_Tcutil_warn( _temp1246->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp1265, sizeof( void*), 0u));});} Cyc_NewControlFlow_update_flow(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs->tl))->hd)->body, clause_outflow);} goto _LL1259; _LL1259:;}}} return( void*)
Cyc_CfFlowInfo_BottomFL;} static void* Cyc_NewControlFlow_anal_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, void* inflow, struct Cyc_Absyn_Stmt* s){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1266= Cyc_NewControlFlow_pre_stmt_check( env, inflow, s); inflow=( void*)
_temp1266->flow;{ void* _temp1267=( void*) s->r; struct Cyc_Absyn_Exp* _temp1309;
struct Cyc_Absyn_Exp* _temp1311; struct Cyc_Absyn_Exp* _temp1313; struct Cyc_Absyn_Stmt*
_temp1315; struct Cyc_Absyn_Stmt* _temp1317; struct Cyc_Absyn_Stmt* _temp1319;
struct Cyc_Absyn_Stmt* _temp1321; struct Cyc_Absyn_Exp* _temp1323; struct Cyc_Absyn_Stmt*
_temp1325; struct _tuple3 _temp1327; struct Cyc_Absyn_Stmt* _temp1329; struct
Cyc_Absyn_Exp* _temp1331; struct _tuple3 _temp1333; struct Cyc_Absyn_Stmt*
_temp1335; struct Cyc_Absyn_Exp* _temp1337; struct Cyc_Absyn_Stmt* _temp1339;
struct Cyc_Absyn_Stmt* _temp1341; struct _tuple3 _temp1343; struct Cyc_Absyn_Stmt*
_temp1345; struct Cyc_Absyn_Exp* _temp1347; struct _tuple3 _temp1349; struct Cyc_Absyn_Stmt*
_temp1351; struct Cyc_Absyn_Exp* _temp1353; struct Cyc_Absyn_Exp* _temp1355;
struct Cyc_Absyn_Stmt* _temp1357; struct Cyc_Absyn_Switch_clause** _temp1359;
struct Cyc_Absyn_Switch_clause* _temp1361; struct Cyc_List_List* _temp1362;
struct Cyc_Absyn_Stmt* _temp1364; struct Cyc_Absyn_Stmt* _temp1366; struct Cyc_Absyn_Stmt*
_temp1368; struct Cyc_List_List* _temp1370; struct Cyc_Absyn_Exp* _temp1372;
struct Cyc_List_List* _temp1374; struct Cyc_Absyn_Stmt* _temp1376; struct Cyc_Absyn_Stmt*
_temp1378; struct Cyc_Absyn_Decl* _temp1380; struct Cyc_Absyn_Stmt* _temp1382;
struct Cyc_Absyn_Stmt* _temp1384; struct Cyc_Absyn_Vardecl* _temp1386; struct
Cyc_Absyn_Tvar* _temp1388; _LL1269: if( _temp1267 == ( void*) Cyc_Absyn_Skip_s){
goto _LL1270;} else{ goto _LL1271;} _LL1271: if(( unsigned int) _temp1267 >  1u?*((
int*) _temp1267) ==  Cyc_Absyn_Return_s: 0){ _LL1310: _temp1309=(( struct Cyc_Absyn_Return_s_struct*)
_temp1267)->f1; if( _temp1309 ==  0){ goto _LL1272;} else{ goto _LL1273;}} else{
goto _LL1273;} _LL1273: if(( unsigned int) _temp1267 >  1u?*(( int*) _temp1267)
==  Cyc_Absyn_Return_s: 0){ _LL1312: _temp1311=(( struct Cyc_Absyn_Return_s_struct*)
_temp1267)->f1; goto _LL1274;} else{ goto _LL1275;} _LL1275: if(( unsigned int)
_temp1267 >  1u?*(( int*) _temp1267) ==  Cyc_Absyn_Exp_s: 0){ _LL1314: _temp1313=((
struct Cyc_Absyn_Exp_s_struct*) _temp1267)->f1; goto _LL1276;} else{ goto
_LL1277;} _LL1277: if(( unsigned int) _temp1267 >  1u?*(( int*) _temp1267) == 
Cyc_Absyn_Seq_s: 0){ _LL1318: _temp1317=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1267)->f1; goto _LL1316; _LL1316: _temp1315=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1267)->f2; goto _LL1278;} else{ goto _LL1279;} _LL1279: if(( unsigned int)
_temp1267 >  1u?*(( int*) _temp1267) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1324:
_temp1323=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1267)->f1; goto _LL1322;
_LL1322: _temp1321=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1267)->f2;
goto _LL1320; _LL1320: _temp1319=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1267)->f3; goto _LL1280;} else{ goto _LL1281;} _LL1281: if(( unsigned int)
_temp1267 >  1u?*(( int*) _temp1267) ==  Cyc_Absyn_While_s: 0){ _LL1328:
_temp1327=(( struct Cyc_Absyn_While_s_struct*) _temp1267)->f1; _LL1332:
_temp1331= _temp1327.f1; goto _LL1330; _LL1330: _temp1329= _temp1327.f2; goto
_LL1326; _LL1326: _temp1325=(( struct Cyc_Absyn_While_s_struct*) _temp1267)->f2;
goto _LL1282;} else{ goto _LL1283;} _LL1283: if(( unsigned int) _temp1267 >  1u?*((
int*) _temp1267) ==  Cyc_Absyn_Do_s: 0){ _LL1340: _temp1339=(( struct Cyc_Absyn_Do_s_struct*)
_temp1267)->f1; goto _LL1334; _LL1334: _temp1333=(( struct Cyc_Absyn_Do_s_struct*)
_temp1267)->f2; _LL1338: _temp1337= _temp1333.f1; goto _LL1336; _LL1336:
_temp1335= _temp1333.f2; goto _LL1284;} else{ goto _LL1285;} _LL1285: if((
unsigned int) _temp1267 >  1u?*(( int*) _temp1267) ==  Cyc_Absyn_For_s: 0){
_LL1356: _temp1355=(( struct Cyc_Absyn_For_s_struct*) _temp1267)->f1; goto
_LL1350; _LL1350: _temp1349=(( struct Cyc_Absyn_For_s_struct*) _temp1267)->f2;
_LL1354: _temp1353= _temp1349.f1; goto _LL1352; _LL1352: _temp1351= _temp1349.f2;
goto _LL1344; _LL1344: _temp1343=(( struct Cyc_Absyn_For_s_struct*) _temp1267)->f3;
_LL1348: _temp1347= _temp1343.f1; goto _LL1346; _LL1346: _temp1345= _temp1343.f2;
goto _LL1342; _LL1342: _temp1341=(( struct Cyc_Absyn_For_s_struct*) _temp1267)->f4;
goto _LL1286;} else{ goto _LL1287;} _LL1287: if(( unsigned int) _temp1267 >  1u?*((
int*) _temp1267) ==  Cyc_Absyn_Break_s: 0){ _LL1358: _temp1357=(( struct Cyc_Absyn_Break_s_struct*)
_temp1267)->f1; if( _temp1357 ==  0){ goto _LL1288;} else{ goto _LL1289;}} else{
goto _LL1289;} _LL1289: if(( unsigned int) _temp1267 >  1u?*(( int*) _temp1267)
==  Cyc_Absyn_Fallthru_s: 0){ _LL1363: _temp1362=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1267)->f1; goto _LL1360; _LL1360: _temp1359=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1267)->f2; if( _temp1359 ==  0){ goto _LL1291;} else{ _temp1361=* _temp1359;
goto _LL1290;}} else{ goto _LL1291;} _LL1291: if(( unsigned int) _temp1267 >  1u?*((
int*) _temp1267) ==  Cyc_Absyn_Break_s: 0){ _LL1365: _temp1364=(( struct Cyc_Absyn_Break_s_struct*)
_temp1267)->f1; goto _LL1292;} else{ goto _LL1293;} _LL1293: if(( unsigned int)
_temp1267 >  1u?*(( int*) _temp1267) ==  Cyc_Absyn_Continue_s: 0){ _LL1367:
_temp1366=(( struct Cyc_Absyn_Continue_s_struct*) _temp1267)->f1; goto _LL1294;}
else{ goto _LL1295;} _LL1295: if(( unsigned int) _temp1267 >  1u?*(( int*)
_temp1267) ==  Cyc_Absyn_Goto_s: 0){ _LL1369: _temp1368=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1267)->f2; goto _LL1296;} else{ goto _LL1297;} _LL1297: if(( unsigned int)
_temp1267 >  1u?*(( int*) _temp1267) ==  Cyc_Absyn_Switch_s: 0){ _LL1373:
_temp1372=(( struct Cyc_Absyn_Switch_s_struct*) _temp1267)->f1; goto _LL1371;
_LL1371: _temp1370=(( struct Cyc_Absyn_Switch_s_struct*) _temp1267)->f2; goto
_LL1298;} else{ goto _LL1299;} _LL1299: if(( unsigned int) _temp1267 >  1u?*((
int*) _temp1267) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1377: _temp1376=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1267)->f1; goto _LL1375; _LL1375: _temp1374=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1267)->f2; goto _LL1300;} else{ goto _LL1301;} _LL1301: if(( unsigned int)
_temp1267 >  1u?*(( int*) _temp1267) ==  Cyc_Absyn_Decl_s: 0){ _LL1381:
_temp1380=(( struct Cyc_Absyn_Decl_s_struct*) _temp1267)->f1; goto _LL1379;
_LL1379: _temp1378=(( struct Cyc_Absyn_Decl_s_struct*) _temp1267)->f2; goto
_LL1302;} else{ goto _LL1303;} _LL1303: if(( unsigned int) _temp1267 >  1u?*((
int*) _temp1267) ==  Cyc_Absyn_Label_s: 0){ _LL1383: _temp1382=(( struct Cyc_Absyn_Label_s_struct*)
_temp1267)->f2; goto _LL1304;} else{ goto _LL1305;} _LL1305: if(( unsigned int)
_temp1267 >  1u?*(( int*) _temp1267) ==  Cyc_Absyn_Region_s: 0){ _LL1389:
_temp1388=(( struct Cyc_Absyn_Region_s_struct*) _temp1267)->f1; goto _LL1387;
_LL1387: _temp1386=(( struct Cyc_Absyn_Region_s_struct*) _temp1267)->f2; goto
_LL1385; _LL1385: _temp1384=(( struct Cyc_Absyn_Region_s_struct*) _temp1267)->f3;
goto _LL1306;} else{ goto _LL1307;} _LL1307: goto _LL1308; _LL1270: return
inflow; _LL1272: return( void*) Cyc_CfFlowInfo_BottomFL; _LL1274: { void*
_temp1392; void* _temp1394; struct _tuple0 _temp1390= Cyc_NewControlFlow_anal_Rexp(
env, inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp1311)); _LL1395:
_temp1394= _temp1390.f1; goto _LL1393; _LL1393: _temp1392= _temp1390.f2; goto
_LL1391; _LL1391: Cyc_NewControlFlow_use_Rval( env,(( struct Cyc_Absyn_Exp*)
_check_null( _temp1311))->loc, _temp1394, _temp1392); return( void*) Cyc_CfFlowInfo_BottomFL;}
_LL1276: return( Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp1313)).f1;
_LL1278: return Cyc_NewControlFlow_anal_stmt( env, Cyc_NewControlFlow_anal_stmt(
env, inflow, _temp1317), _temp1315); _LL1280: { void* _temp1398; void* _temp1400;
struct _tuple0 _temp1396= Cyc_NewControlFlow_anal_test( env, inflow, _temp1323);
_LL1401: _temp1400= _temp1396.f1; goto _LL1399; _LL1399: _temp1398= _temp1396.f2;
goto _LL1397; _LL1397: return Cyc_CfFlowInfo_join_flow( env->all_changed, Cyc_NewControlFlow_anal_stmt(
env, _temp1400, _temp1321), Cyc_NewControlFlow_anal_stmt( env, _temp1398,
_temp1319));} _LL1282: { void* _temp1402=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, inflow, _temp1329))->flow; void* _temp1405; void* _temp1407; struct _tuple0
_temp1403= Cyc_NewControlFlow_anal_test( env, _temp1402, _temp1331); _LL1408:
_temp1407= _temp1403.f1; goto _LL1406; _LL1406: _temp1405= _temp1403.f2; goto
_LL1404; _LL1404: { void* _temp1409= Cyc_NewControlFlow_anal_stmt( env,
_temp1407, _temp1325); Cyc_NewControlFlow_update_flow( env, _temp1329, _temp1409);
return _temp1405;}} _LL1284: { void* _temp1410= Cyc_NewControlFlow_anal_stmt(
env, inflow, _temp1339); void* _temp1411=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1410, _temp1335))->flow; void* _temp1414; void* _temp1416; struct
_tuple0 _temp1412= Cyc_NewControlFlow_anal_test( env, _temp1411, _temp1337);
_LL1417: _temp1416= _temp1412.f1; goto _LL1415; _LL1415: _temp1414= _temp1412.f2;
goto _LL1413; _LL1413: Cyc_NewControlFlow_update_flow( env, _temp1339, _temp1416);
return _temp1414;} _LL1286: { void* _temp1418=( Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1355)).f1; void* _temp1419=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1418, _temp1351))->flow; void* _temp1422; void* _temp1424; struct
_tuple0 _temp1420= Cyc_NewControlFlow_anal_test( env, _temp1419, _temp1353);
_LL1425: _temp1424= _temp1420.f1; goto _LL1423; _LL1423: _temp1422= _temp1420.f2;
goto _LL1421; _LL1421: { void* _temp1426= Cyc_NewControlFlow_anal_stmt( env,
_temp1424, _temp1341); void* _temp1427=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1426, _temp1345))->flow; void* _temp1428=( Cyc_NewControlFlow_anal_Rexp(
env, _temp1427, _temp1347)).f1; Cyc_NewControlFlow_update_flow( env, _temp1351,
_temp1428); return _temp1422;}} _LL1288: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1290: { struct Cyc_List_List* _temp1431; void* _temp1433; struct _tuple4
_temp1429= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1362);
_LL1434: _temp1433= _temp1429.f1; goto _LL1432; _LL1432: _temp1431= _temp1429.f2;
goto _LL1430; _LL1430: for( 0; _temp1431 !=  0;( _temp1431= _temp1431->tl,
_temp1362= _temp1362->tl)){ _temp1433= Cyc_NewControlFlow_use_Rval( env,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1362))->hd)->loc,
_temp1433,( void*) _temp1431->hd);} _temp1433= Cyc_NewControlFlow_add_vars(
_temp1433,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1361->pat_vars))->v, Cyc_CfFlowInfo_unknown_all); Cyc_NewControlFlow_update_flow(
env,( struct Cyc_Absyn_Stmt*) _temp1361->body, _temp1433); return( void*) Cyc_CfFlowInfo_BottomFL;}
_LL1292: _temp1366= _temp1364; goto _LL1294; _LL1294: _temp1368= _temp1366; goto
_LL1296; _LL1296: if( env->iteration_num ==  1){ struct Cyc_Absyn_Stmt*
_temp1435= _temp1266->encloser; struct Cyc_Absyn_Stmt* _temp1436=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*) _check_null( _temp1368)))->encloser; while( _temp1436 != 
_temp1435) { struct Cyc_Absyn_Stmt* _temp1437=( Cyc_NewControlFlow_get_stmt_annot(
_temp1435))->encloser; if( _temp1437 ==  _temp1435){({ void* _temp1438[ 0u]={};
Cyc_Tcutil_terr( s->loc, _tag_arr("goto enters local scope or exception handler",
sizeof( unsigned char), 45u), _tag_arr( _temp1438, sizeof( void*), 0u));});
break;} _temp1435= _temp1437;}} Cyc_NewControlFlow_update_flow( env,( struct Cyc_Absyn_Stmt*)
_check_null( _temp1368), inflow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1298: { void* _temp1441; void* _temp1443; struct _tuple0 _temp1439= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1372); _LL1444: _temp1443= _temp1439.f1; goto _LL1442; _LL1442:
_temp1441= _temp1439.f2; goto _LL1440; _LL1440: _temp1443= Cyc_NewControlFlow_use_Rval(
env, _temp1372->loc, _temp1443, _temp1441); return Cyc_NewControlFlow_anal_scs(
env, _temp1443, _temp1370);} _LL1300: { int old_in_try= env->in_try; void*
old_tryflow=( void*) env->tryflow; env->in_try= 1;( void*)( env->tryflow=( void*)
inflow);{ void* s1_outflow= Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1376);
void* scs_inflow=( void*) env->tryflow; env->in_try= old_in_try;( void*)( env->tryflow=(
void*) old_tryflow); Cyc_NewControlFlow_update_tryflow( env, scs_inflow);{ void*
scs_outflow= Cyc_NewControlFlow_anal_scs( env, scs_inflow, _temp1374);{ void*
_temp1445= scs_outflow; _LL1447: if( _temp1445 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1448;} else{ goto _LL1449;} _LL1449: goto _LL1450; _LL1448: goto _LL1446;
_LL1450:({ void* _temp1451[ 0u]={}; Cyc_Tcutil_terr( s->loc, _tag_arr("last catch clause may implicitly fallthru",
sizeof( unsigned char), 42u), _tag_arr( _temp1451, sizeof( void*), 0u));});
_LL1446:;} return s1_outflow;}}} _LL1302: return Cyc_NewControlFlow_anal_stmt(
env, Cyc_NewControlFlow_anal_decl( env, inflow, _temp1380), _temp1378); _LL1304:
return Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1382); _LL1306: { struct
Cyc_List_List _temp1452=({ struct Cyc_List_List _temp1453; _temp1453.hd=( void*)
_temp1386; _temp1453.tl= 0; _temp1453;}); inflow= Cyc_NewControlFlow_add_vars(
inflow,( struct Cyc_List_List*)& _temp1452, Cyc_CfFlowInfo_unknown_all); return
Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1384);} _LL1308:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp1454=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1454[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1455; _temp1455.tag= Cyc_Core_Impossible;
_temp1455.f1= _tag_arr("anal_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof( unsigned char), 56u); _temp1455;}); _temp1454;})); _LL1268:;}} static
void* Cyc_NewControlFlow_anal_decl( struct Cyc_NewControlFlow_AnalEnv* env, void*
inflow, struct Cyc_Absyn_Decl* decl){ void* _temp1456=( void*) decl->r; struct
Cyc_Absyn_Vardecl* _temp1466; struct Cyc_Absyn_Exp* _temp1468; struct Cyc_Core_Opt*
_temp1470; struct Cyc_Core_Opt _temp1472; struct Cyc_List_List* _temp1473;
struct Cyc_List_List* _temp1475; _LL1458: if(*(( int*) _temp1456) ==  Cyc_Absyn_Var_d){
_LL1467: _temp1466=(( struct Cyc_Absyn_Var_d_struct*) _temp1456)->f1; goto
_LL1459;} else{ goto _LL1460;} _LL1460: if(*(( int*) _temp1456) ==  Cyc_Absyn_Let_d){
_LL1471: _temp1470=(( struct Cyc_Absyn_Let_d_struct*) _temp1456)->f2; if(
_temp1470 ==  0){ goto _LL1462;} else{ _temp1472=* _temp1470; _LL1474: _temp1473=(
struct Cyc_List_List*) _temp1472.v; goto _LL1469;} _LL1469: _temp1468=(( struct
Cyc_Absyn_Let_d_struct*) _temp1456)->f4; goto _LL1461;} else{ goto _LL1462;}
_LL1462: if(*(( int*) _temp1456) ==  Cyc_Absyn_Letv_d){ _LL1476: _temp1475=((
struct Cyc_Absyn_Letv_d_struct*) _temp1456)->f1; goto _LL1463;} else{ goto
_LL1464;} _LL1464: goto _LL1465; _LL1459: { struct Cyc_List_List _temp1477=({
struct Cyc_List_List _temp1499; _temp1499.hd=( void*) _temp1466; _temp1499.tl= 0;
_temp1499;}); inflow= Cyc_NewControlFlow_add_vars( inflow,( struct Cyc_List_List*)&
_temp1477, Cyc_CfFlowInfo_unknown_none);{ struct Cyc_Absyn_Exp* _temp1478=
_temp1466->initializer; if( _temp1478 ==  0){ return inflow;}{ void* _temp1481;
void* _temp1483; struct _tuple0 _temp1479= Cyc_NewControlFlow_anal_Rexp( env,
inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp1478)); _LL1484: _temp1483=
_temp1479.f1; goto _LL1482; _LL1482: _temp1481= _temp1479.f2; goto _LL1480;
_LL1480: { void* _temp1485= _temp1483; struct Cyc_Dict_Dict* _temp1491; _LL1487:
if( _temp1485 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1488;} else{ goto
_LL1489;} _LL1489: if(( unsigned int) _temp1485 >  1u?*(( int*) _temp1485) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL1492: _temp1491=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1485)->f1; goto _LL1490;} else{ goto _LL1486;} _LL1488: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1490: _temp1491= Cyc_CfFlowInfo_assign_place( decl->loc, _temp1491, env->all_changed,({
struct Cyc_CfFlowInfo_Place* _temp1493=( struct Cyc_CfFlowInfo_Place*) _cycalloc(
sizeof( struct Cyc_CfFlowInfo_Place)); _temp1493->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp1494=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp1494[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp1495; _temp1495.tag= Cyc_CfFlowInfo_VarRoot;
_temp1495.f1= _temp1466; _temp1495;}); _temp1494;})); _temp1493->fields= 0;
_temp1493;}), _temp1481);{ struct Cyc_CfFlowInfo_ReachableFL_struct* _temp1496=({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp1497=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp1497[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp1498; _temp1498.tag= Cyc_CfFlowInfo_ReachableFL;
_temp1498.f1= _temp1491; _temp1498;}); _temp1497;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp1496); return( void*) _temp1496;} _LL1486:;}}}} _LL1461: {
void* _temp1502; void* _temp1504; struct _tuple0 _temp1500= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1468); _LL1505: _temp1504= _temp1500.f1; goto _LL1503; _LL1503:
_temp1502= _temp1500.f2; goto _LL1501; _LL1501: _temp1504= Cyc_NewControlFlow_use_Rval(
env, _temp1468->loc, _temp1504, _temp1502); return Cyc_NewControlFlow_add_vars(
_temp1504, _temp1473, Cyc_CfFlowInfo_unknown_all);} _LL1463: return Cyc_NewControlFlow_add_vars(
inflow, _temp1475, Cyc_CfFlowInfo_unknown_none); _LL1465:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp1506=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp1506[ 0]=({ struct
Cyc_Core_Impossible_struct _temp1507; _temp1507.tag= Cyc_Core_Impossible;
_temp1507.f1= _tag_arr("anal_decl: unexpected decl variant", sizeof(
unsigned char), 35u); _temp1507;}); _temp1506;})); _LL1457:;} static void Cyc_NewControlFlow_postpass(
struct Cyc_Absyn_Stmt* s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{
void* _temp1508=( void*) s->r; struct Cyc_Absyn_Stmt* _temp1532; struct _tuple3
_temp1534; struct Cyc_Absyn_Stmt* _temp1536; struct _tuple3 _temp1538; struct
Cyc_Absyn_Stmt* _temp1540; struct Cyc_Absyn_Stmt* _temp1542; struct Cyc_Absyn_Stmt*
_temp1544; struct Cyc_Absyn_Stmt* _temp1546; struct _tuple3 _temp1548; struct
Cyc_Absyn_Stmt* _temp1550; struct _tuple3 _temp1552; struct Cyc_Absyn_Stmt*
_temp1554; struct Cyc_Absyn_Stmt* _temp1556; struct Cyc_Absyn_Stmt* _temp1558;
struct Cyc_Absyn_Stmt* _temp1560; struct Cyc_Absyn_Stmt* _temp1562; struct Cyc_Absyn_Stmt*
_temp1564; struct Cyc_Absyn_Stmt* _temp1566; struct Cyc_List_List* _temp1568;
struct Cyc_Absyn_Stmt* _temp1570; struct Cyc_List_List* _temp1572; _LL1510: if((
unsigned int) _temp1508 >  1u?*(( int*) _temp1508) ==  Cyc_Absyn_For_s: 0){
_LL1539: _temp1538=(( struct Cyc_Absyn_For_s_struct*) _temp1508)->f2; _LL1541:
_temp1540= _temp1538.f2; goto _LL1535; _LL1535: _temp1534=(( struct Cyc_Absyn_For_s_struct*)
_temp1508)->f3; _LL1537: _temp1536= _temp1534.f2; goto _LL1533; _LL1533:
_temp1532=(( struct Cyc_Absyn_For_s_struct*) _temp1508)->f4; goto _LL1511;}
else{ goto _LL1512;} _LL1512: if(( unsigned int) _temp1508 >  1u?*(( int*)
_temp1508) ==  Cyc_Absyn_Seq_s: 0){ _LL1545: _temp1544=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1508)->f1; goto _LL1543; _LL1543: _temp1542=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1508)->f2; goto _LL1513;} else{ goto _LL1514;} _LL1514: if(( unsigned int)
_temp1508 >  1u?*(( int*) _temp1508) ==  Cyc_Absyn_While_s: 0){ _LL1549:
_temp1548=(( struct Cyc_Absyn_While_s_struct*) _temp1508)->f1; _LL1551:
_temp1550= _temp1548.f2; goto _LL1547; _LL1547: _temp1546=(( struct Cyc_Absyn_While_s_struct*)
_temp1508)->f2; goto _LL1515;} else{ goto _LL1516;} _LL1516: if(( unsigned int)
_temp1508 >  1u?*(( int*) _temp1508) ==  Cyc_Absyn_Do_s: 0){ _LL1557: _temp1556=((
struct Cyc_Absyn_Do_s_struct*) _temp1508)->f1; goto _LL1553; _LL1553: _temp1552=((
struct Cyc_Absyn_Do_s_struct*) _temp1508)->f2; _LL1555: _temp1554= _temp1552.f2;
goto _LL1517;} else{ goto _LL1518;} _LL1518: if(( unsigned int) _temp1508 >  1u?*((
int*) _temp1508) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1561: _temp1560=(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp1508)->f2; goto _LL1559; _LL1559: _temp1558=((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp1508)->f3; goto _LL1519;} else{ goto
_LL1520;} _LL1520: if(( unsigned int) _temp1508 >  1u?*(( int*) _temp1508) == 
Cyc_Absyn_Region_s: 0){ _LL1563: _temp1562=(( struct Cyc_Absyn_Region_s_struct*)
_temp1508)->f3; goto _LL1521;} else{ goto _LL1522;} _LL1522: if(( unsigned int)
_temp1508 >  1u?*(( int*) _temp1508) ==  Cyc_Absyn_Label_s: 0){ _LL1565:
_temp1564=(( struct Cyc_Absyn_Label_s_struct*) _temp1508)->f2; goto _LL1523;}
else{ goto _LL1524;} _LL1524: if(( unsigned int) _temp1508 >  1u?*(( int*)
_temp1508) ==  Cyc_Absyn_Decl_s: 0){ _LL1567: _temp1566=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1508)->f2; goto _LL1525;} else{ goto _LL1526;} _LL1526: if(( unsigned int)
_temp1508 >  1u?*(( int*) _temp1508) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1571:
_temp1570=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1508)->f1; goto _LL1569;
_LL1569: _temp1568=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp1508)->f2; goto
_LL1527;} else{ goto _LL1528;} _LL1528: if(( unsigned int) _temp1508 >  1u?*((
int*) _temp1508) ==  Cyc_Absyn_Switch_s: 0){ _LL1573: _temp1572=(( struct Cyc_Absyn_Switch_s_struct*)
_temp1508)->f2; goto _LL1529;} else{ goto _LL1530;} _LL1530: goto _LL1531;
_LL1511: Cyc_NewControlFlow_postpass( _temp1536); _temp1544= _temp1532;
_temp1542= _temp1540; goto _LL1513; _LL1513: _temp1550= _temp1544; _temp1546=
_temp1542; goto _LL1515; _LL1515: _temp1556= _temp1546; _temp1554= _temp1550;
goto _LL1517; _LL1517: _temp1560= _temp1556; _temp1558= _temp1554; goto _LL1519;
_LL1519: Cyc_NewControlFlow_postpass( _temp1560); _temp1562= _temp1558; goto
_LL1521; _LL1521: _temp1564= _temp1562; goto _LL1523; _LL1523: _temp1566=
_temp1564; goto _LL1525; _LL1525: Cyc_NewControlFlow_postpass( _temp1566);
return; _LL1527: Cyc_NewControlFlow_postpass( _temp1570); _temp1572= _temp1568;
goto _LL1529; _LL1529: for( 0; _temp1572 !=  0; _temp1572= _temp1572->tl){ Cyc_NewControlFlow_postpass(((
struct Cyc_Absyn_Switch_clause*) _temp1572->hd)->body);} return; _LL1531:
return; _LL1509:;}} static void Cyc_NewControlFlow_check_fun( struct Cyc_Absyn_Fndecl*
fd){ void* inflow= Cyc_NewControlFlow_add_vars(( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp1582=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp1582[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp1583; _temp1583.tag= Cyc_CfFlowInfo_ReachableFL; _temp1583.f1= Cyc_Dict_empty(
Cyc_CfFlowInfo_root_cmp); _temp1583;}); _temp1582;}),( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( fd->param_vardecls))->v, Cyc_CfFlowInfo_unknown_all);
struct Cyc_NewControlFlow_AnalEnv* env=({ struct Cyc_NewControlFlow_AnalEnv*
_temp1581=( struct Cyc_NewControlFlow_AnalEnv*) _cycalloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp1581->iterate_again= 1; _temp1581->iteration_num= 0; _temp1581->in_try= 0;
_temp1581->tryflow=( void*) inflow; _temp1581->all_changed= 0; _temp1581;});
void* outflow= inflow; while( env->iterate_again) { ++ env->iteration_num; env->iterate_again=
0; outflow= Cyc_NewControlFlow_anal_stmt( env, inflow, fd->body);}{ void*
_temp1574= outflow; _LL1576: if( _temp1574 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1577;} else{ goto _LL1578;} _LL1578: goto _LL1579; _LL1577: goto _LL1575;
_LL1579: if(( void*) fd->ret_type != ( void*) Cyc_Absyn_VoidType){({ void*
_temp1580[ 0u]={}; Cyc_Tcutil_terr(( fd->body)->loc, _tag_arr("function may not return a value",
sizeof( unsigned char), 32u), _tag_arr( _temp1580, sizeof( void*), 0u));});}
_LL1575:;} Cyc_NewControlFlow_postpass( fd->body);} void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds){ for( 0; ds !=  0; ds= ds->tl){ void* _temp1584=( void*)((
struct Cyc_Absyn_Decl*) ds->hd)->r; struct Cyc_Absyn_Fndecl* _temp1596; struct
Cyc_List_List* _temp1598; struct Cyc_List_List* _temp1600; struct Cyc_List_List*
_temp1602; _LL1586: if(*(( int*) _temp1584) ==  Cyc_Absyn_Fn_d){ _LL1597:
_temp1596=(( struct Cyc_Absyn_Fn_d_struct*) _temp1584)->f1; goto _LL1587;} else{
goto _LL1588;} _LL1588: if(*(( int*) _temp1584) ==  Cyc_Absyn_ExternC_d){
_LL1599: _temp1598=(( struct Cyc_Absyn_ExternC_d_struct*) _temp1584)->f1; goto
_LL1589;} else{ goto _LL1590;} _LL1590: if(*(( int*) _temp1584) ==  Cyc_Absyn_Using_d){
_LL1601: _temp1600=(( struct Cyc_Absyn_Using_d_struct*) _temp1584)->f2; goto
_LL1591;} else{ goto _LL1592;} _LL1592: if(*(( int*) _temp1584) ==  Cyc_Absyn_Namespace_d){
_LL1603: _temp1602=(( struct Cyc_Absyn_Namespace_d_struct*) _temp1584)->f2; goto
_LL1593;} else{ goto _LL1594;} _LL1594: goto _LL1595; _LL1587: Cyc_NewControlFlow_check_fun(
_temp1596); goto _LL1585; _LL1589: _temp1600= _temp1598; goto _LL1591; _LL1591:
_temp1602= _temp1600; goto _LL1593; _LL1593: Cyc_NewControlFlow_cf_check(
_temp1602); goto _LL1585; _LL1595: goto _LL1585; _LL1585:;}}
