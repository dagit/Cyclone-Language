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
void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*); extern int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref* b); extern int Cyc_Tcutil_is_noreturn( void*); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); static
const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List* fields; } ;
static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_EqualConst=
0; struct Cyc_CfFlowInfo_EqualConst_struct{ int tag; unsigned int f1; } ; static
const int Cyc_CfFlowInfo_LessVar= 1; struct Cyc_CfFlowInfo_LessVar_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_LessSize=
2; struct Cyc_CfFlowInfo_LessSize_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_LessConst= 3; struct Cyc_CfFlowInfo_LessConst_struct{
int tag; unsigned int f1; } ; static const int Cyc_CfFlowInfo_LessEqSize= 4;
struct Cyc_CfFlowInfo_LessEqSize_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; struct Cyc_CfFlowInfo_Reln{ struct Cyc_Absyn_Vardecl* vd; void* rop; } ;
extern unsigned char Cyc_CfFlowInfo_IsZero[ 11u]; struct Cyc_CfFlowInfo_IsZero_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_CfFlowInfo_NotZero[
12u]; struct Cyc_CfFlowInfo_NotZero_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_CfFlowInfo_UnknownZ[ 13u]; struct Cyc_CfFlowInfo_UnknownZ_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; static const int Cyc_CfFlowInfo_PlaceL=
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
all_changed; int noreturn; } ; static void* Cyc_NewControlFlow_anal_stmt( struct
Cyc_NewControlFlow_AnalEnv*, void*, struct Cyc_Absyn_Stmt*); static void* Cyc_NewControlFlow_anal_decl(
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
_temp623,( void*) _temp621->hd);} if( Cyc_Tcutil_is_noreturn(( void*)(( struct
Cyc_Core_Opt*) _check_null( _temp444->topt))->v)){ return({ struct _tuple0
_temp625; _temp625.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp625.f2= Cyc_CfFlowInfo_typ_to_absrval((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all);
_temp625;});} else{ return({ struct _tuple0 _temp626; _temp626.f1= _temp623;
_temp626.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp626;});}} _LL340: {
void* root=( void*)({ struct Cyc_CfFlowInfo_MallocPt_struct* _temp655=( struct
Cyc_CfFlowInfo_MallocPt_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct));
_temp655[ 0]=({ struct Cyc_CfFlowInfo_MallocPt_struct _temp656; _temp656.tag=
Cyc_CfFlowInfo_MallocPt; _temp656.f1= _temp450; _temp656;}); _temp655;}); struct
Cyc_CfFlowInfo_Place* place=({ struct Cyc_CfFlowInfo_Place* _temp654=( struct
Cyc_CfFlowInfo_Place*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Place));
_temp654->root=( void*) root; _temp654->fields= 0; _temp654;}); void* rval=(
void*)({ struct Cyc_CfFlowInfo_AddressOf_struct* _temp652=( struct Cyc_CfFlowInfo_AddressOf_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_AddressOf_struct)); _temp652[ 0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _temp653; _temp653.tag= Cyc_CfFlowInfo_AddressOf;
_temp653.f1= place; _temp653;}); _temp652;}); void* place_val= _temp448?( void*)
Cyc_CfFlowInfo_NotZeroAll: Cyc_CfFlowInfo_typ_to_absrval(*(( void**) _check_null(
_temp452)), Cyc_CfFlowInfo_unknown_none); void* outflow; if( env->all_changed != 
0){*(( struct Cyc_Set_Set**) _check_null( env->all_changed))=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(*((
struct Cyc_Set_Set**) _check_null( env->all_changed)), place);} if( _temp454 != 
0){ struct Cyc_List_List* _temp630; void* _temp632; struct _tuple4 _temp628= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp627[ 2u]; _temp627[ 1u]= _temp450;
_temp627[ 0u]=( struct Cyc_Absyn_Exp*) _check_null( _temp454);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp627, sizeof( struct Cyc_Absyn_Exp*),
2u));})); _LL633: _temp632= _temp628.f1; goto _LL631; _LL631: _temp630= _temp628.f2;
goto _LL629; _LL629: outflow= _temp632;} else{ void* _temp636; struct _tuple0
_temp634= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp450); _LL637: _temp636=
_temp634.f1; goto _LL635; _LL635: outflow= _temp636;}{ void* _temp638= outflow;
struct Cyc_List_List* _temp644; struct Cyc_Dict_Dict* _temp646; _LL640: if(
_temp638 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL641;} else{ goto _LL642;}
_LL642: if(( unsigned int) _temp638 >  1u?*(( int*) _temp638) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL647: _temp646=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp638)->f1;
goto _LL645; _LL645: _temp644=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp638)->f2; goto _LL643;} else{ goto _LL639;} _LL641: return({ struct _tuple0
_temp648; _temp648.f1= outflow; _temp648.f2= rval; _temp648;}); _LL643: return({
struct _tuple0 _temp649; _temp649.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp650=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp650[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp651; _temp651.tag= Cyc_CfFlowInfo_ReachableFL; _temp651.f1= Cyc_Dict_insert(
_temp646, root, place_val); _temp651.f2= _temp644; _temp651;}); _temp650;});
_temp649.f2= rval; _temp649;}); _LL639:;}} _LL342: { void* root=( void*)({
struct Cyc_CfFlowInfo_MallocPt_struct* _temp687=( struct Cyc_CfFlowInfo_MallocPt_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_MallocPt_struct)); _temp687[ 0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _temp688; _temp688.tag= Cyc_CfFlowInfo_MallocPt;
_temp688.f1= _temp458; _temp688;}); _temp687;}); struct Cyc_CfFlowInfo_Place*
place=({ struct Cyc_CfFlowInfo_Place* _temp686=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp686->root=( void*) root;
_temp686->fields= 0; _temp686;}); void* rval=( void*)({ struct Cyc_CfFlowInfo_AddressOf_struct*
_temp684=( struct Cyc_CfFlowInfo_AddressOf_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_AddressOf_struct)); _temp684[ 0]=({ struct Cyc_CfFlowInfo_AddressOf_struct
_temp685; _temp685.tag= Cyc_CfFlowInfo_AddressOf; _temp685.f1= place; _temp685;});
_temp684;}); if( env->all_changed !=  0){*(( struct Cyc_Set_Set**) _check_null(
env->all_changed))=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place*
elt)) Cyc_Set_insert)(*(( struct Cyc_Set_Set**) _check_null( env->all_changed)),
place);}{ void* outflow; void* place_val; if( _temp460 !=  0){ struct Cyc_List_List*
_temp660; void* _temp662; struct _tuple4 _temp658= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp657[ 2u]; _temp657[ 1u]= _temp458;
_temp657[ 0u]=( struct Cyc_Absyn_Exp*) _check_null( _temp460);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp657, sizeof( struct Cyc_Absyn_Exp*),
2u));})); _LL663: _temp662= _temp658.f1; goto _LL661; _LL661: _temp660= _temp658.f2;
goto _LL659; _LL659: outflow= _temp662; place_val=( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp660))->tl))->hd;} else{
void* _temp666; void* _temp668; struct _tuple0 _temp664= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp458); _LL669: _temp668= _temp664.f1; goto _LL667; _LL667:
_temp666= _temp664.f2; goto _LL665; _LL665: outflow= _temp668; place_val=
_temp666;}{ void* _temp670= outflow; struct Cyc_List_List* _temp676; struct Cyc_Dict_Dict*
_temp678; _LL672: if( _temp670 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL673;}
else{ goto _LL674;} _LL674: if(( unsigned int) _temp670 >  1u?*(( int*) _temp670)
==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL679: _temp678=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp670)->f1; goto _LL677; _LL677: _temp676=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp670)->f2; goto _LL675;} else{ goto _LL671;} _LL673: return({ struct _tuple0
_temp680; _temp680.f1= outflow; _temp680.f2= rval; _temp680;}); _LL675: return({
struct _tuple0 _temp681; _temp681.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp682=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp682[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp683; _temp683.tag= Cyc_CfFlowInfo_ReachableFL; _temp683.f1= Cyc_Dict_insert(
_temp678, root, place_val); _temp683.f2= _temp676; _temp683;}); _temp682;});
_temp681.f2= rval; _temp681;}); _LL671:;}}} _LL344: { void* _temp691; void*
_temp693; struct _tuple0 _temp689= Cyc_NewControlFlow_anal_Lexp( env, inflow,
_temp462); _LL694: _temp693= _temp689.f1; goto _LL692; _LL692: _temp691=
_temp689.f2; goto _LL690; _LL690: { void* _temp695= _temp691; struct Cyc_CfFlowInfo_Place*
_temp701; _LL697: if( _temp695 == ( void*) Cyc_CfFlowInfo_UnknownL){ goto _LL698;}
else{ goto _LL699;} _LL699: if(( unsigned int) _temp695 >  1u?*(( int*) _temp695)
==  Cyc_CfFlowInfo_PlaceL: 0){ _LL702: _temp701=(( struct Cyc_CfFlowInfo_PlaceL_struct*)
_temp695)->f1; goto _LL700;} else{ goto _LL696;} _LL698: return({ struct _tuple0
_temp703; _temp703.f1= _temp693; _temp703.f2=( void*) Cyc_CfFlowInfo_NotZeroAll;
_temp703;}); _LL700: return({ struct _tuple0 _temp704; _temp704.f1= _temp693;
_temp704.f2=( void*)({ struct Cyc_CfFlowInfo_AddressOf_struct* _temp705=( struct
Cyc_CfFlowInfo_AddressOf_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_AddressOf_struct));
_temp705[ 0]=({ struct Cyc_CfFlowInfo_AddressOf_struct _temp706; _temp706.tag=
Cyc_CfFlowInfo_AddressOf; _temp706.f1= _temp701; _temp706;}); _temp705;});
_temp704;}); _LL696:;}} _LL346: { void* _temp709; void* _temp711; struct _tuple0
_temp707= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp464); _LL712: _temp711=
_temp707.f1; goto _LL710; _LL710: _temp709= _temp707.f2; goto _LL708; _LL708:
return Cyc_NewControlFlow_anal_derefR( env, inflow, _temp711, _temp464, _temp709);}
_LL348: { void* _temp715; void* _temp717; struct _tuple0 _temp713= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp468); _LL718: _temp717= _temp713.f1; goto _LL716; _LL716:
_temp715= _temp713.f2; goto _LL714; _LL714:{ void* _temp719= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp468->topt))->v); _LL721: if((
unsigned int) _temp719 >  3u?*(( int*) _temp719) ==  Cyc_Absyn_AnonUnionType: 0){
goto _LL722;} else{ goto _LL723;} _LL723: if(( unsigned int) _temp719 >  3u?*((
int*) _temp719) ==  Cyc_Absyn_UnionType: 0){ goto _LL724;} else{ goto _LL725;}
_LL725: goto _LL726; _LL722: goto _LL724; _LL724: return({ struct _tuple0
_temp727; _temp727.f1= _temp717; _temp727.f2= Cyc_CfFlowInfo_typ_to_absrval((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all);
_temp727;}); _LL726: goto _LL720; _LL720:;}{ void* _temp728= _temp715; struct
Cyc_Dict_Dict* _temp734; _LL730: if(( unsigned int) _temp728 >  3u?*(( int*)
_temp728) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL735: _temp734=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp728)->f1; goto _LL731;} else{ goto _LL732;} _LL732: goto _LL733; _LL731:
return({ struct _tuple0 _temp736; _temp736.f1= _temp717; _temp736.f2=(( void*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp734,
_temp466); _temp736;}); _LL733:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp737=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp737[ 0]=({ struct Cyc_Core_Impossible_struct _temp738; _temp738.tag= Cyc_Core_Impossible;
_temp738.f1= _tag_arr("anal_Rexp: StructMember", sizeof( unsigned char), 24u);
_temp738;}); _temp737;})); _LL729:;}} _LL350: { void* _temp741; void* _temp743;
struct _tuple0 _temp739= Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp472);
_LL744: _temp743= _temp739.f1; goto _LL742; _LL742: _temp741= _temp739.f2; goto
_LL740; _LL740: { void* _temp747; void* _temp749; struct _tuple0 _temp745= Cyc_NewControlFlow_anal_derefR(
env, inflow, _temp743, _temp472, _temp741); _LL750: _temp749= _temp745.f1; goto
_LL748; _LL748: _temp747= _temp745.f2; goto _LL746; _LL746:{ void* _temp751= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp472->topt))->v); struct Cyc_Absyn_PtrInfo
_temp757; void* _temp759; _LL753: if(( unsigned int) _temp751 >  3u?*(( int*)
_temp751) ==  Cyc_Absyn_PointerType: 0){ _LL758: _temp757=(( struct Cyc_Absyn_PointerType_struct*)
_temp751)->f1; _LL760: _temp759=( void*) _temp757.elt_typ; goto _LL754;} else{
goto _LL755;} _LL755: goto _LL756; _LL754:{ void* _temp761= Cyc_Tcutil_compress(
_temp759); _LL763: if(( unsigned int) _temp761 >  3u?*(( int*) _temp761) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL764;} else{ goto _LL765;} _LL765: if(( unsigned int) _temp761 >  3u?*((
int*) _temp761) ==  Cyc_Absyn_UnionType: 0){ goto _LL766;} else{ goto _LL767;}
_LL767: goto _LL768; _LL764: goto _LL766; _LL766: return({ struct _tuple0
_temp769; _temp769.f1= _temp749; _temp769.f2= Cyc_CfFlowInfo_typ_to_absrval((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all);
_temp769;}); _LL768: goto _LL762; _LL762:;} goto _LL752; _LL756:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp770=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp770[ 0]=({ struct
Cyc_Core_Impossible_struct _temp771; _temp771.tag= Cyc_Core_Impossible; _temp771.f1=
_tag_arr("anal_Rexp: StructArrow ptr", sizeof( unsigned char), 27u); _temp771;});
_temp770;})); _LL752:;}{ void* _temp772= _temp747; struct Cyc_Dict_Dict*
_temp778; _LL774: if(( unsigned int) _temp772 >  3u?*(( int*) _temp772) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL779: _temp778=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp772)->f1;
goto _LL775;} else{ goto _LL776;} _LL776: goto _LL777; _LL775: return({ struct
_tuple0 _temp780; _temp780.f1= _temp749; _temp780.f2=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp778, _temp470); _temp780;});
_LL777:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp781=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp781[ 0]=({ struct Cyc_Core_Impossible_struct _temp782; _temp782.tag= Cyc_Core_Impossible;
_temp782.f1= _tag_arr("anal_Rexp: StructArrow", sizeof( unsigned char), 23u);
_temp782;}); _temp781;})); _LL773:;}}} _LL352: { void* _temp785; void* _temp787;
struct _tuple0 _temp783= Cyc_NewControlFlow_anal_test( env, inflow, _temp478);
_LL788: _temp787= _temp783.f1; goto _LL786; _LL786: _temp785= _temp783.f2; goto
_LL784; _LL784: { void* _temp791; void* _temp793; struct _tuple0 _temp789= Cyc_NewControlFlow_anal_Rexp(
env, _temp787, _temp476); _LL794: _temp793= _temp789.f1; goto _LL792; _LL792:
_temp791= _temp789.f2; goto _LL790; _LL790: { void* _temp797; void* _temp799;
struct _tuple0 _temp795= Cyc_NewControlFlow_anal_Rexp( env, _temp785, _temp474);
_LL800: _temp799= _temp795.f1; goto _LL798; _LL798: _temp797= _temp795.f2; goto
_LL796; _LL796: return({ struct _tuple0 _temp801; _temp801.f1= Cyc_CfFlowInfo_join_flow(
env->all_changed, Cyc_NewControlFlow_use_Rval( env, _temp476->loc, _temp793,
_temp791), Cyc_NewControlFlow_use_Rval( env, _temp474->loc, _temp799, _temp797));
_temp801.f2= Cyc_CfFlowInfo_unknown_all; _temp801;});}}} _LL354: { struct Cyc_List_List*
_temp805; void* _temp807; struct _tuple4 _temp803= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp802[ 2u]; _temp802[ 1u]= _temp480;
_temp802[ 0u]= _temp482;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp802, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL808: _temp807=
_temp803.f1; goto _LL806; _LL806: _temp805= _temp803.f2; goto _LL804; _LL804:{
void* _temp809= _temp807; struct Cyc_List_List* _temp815; struct Cyc_Dict_Dict*
_temp817; _LL811: if(( unsigned int) _temp809 >  1u?*(( int*) _temp809) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL818: _temp817=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp809)->f1;
goto _LL816; _LL816: _temp815=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp809)->f2; goto _LL812;} else{ goto _LL813;} _LL813: goto _LL814; _LL812:
if( Cyc_CfFlowInfo_initlevel( _temp817,( void*)(( struct Cyc_List_List*)
_check_null((( struct Cyc_List_List*) _check_null( _temp805))->tl))->hd) == (
void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp819[ 0u]={}; Cyc_Tcutil_terr(
_temp480->loc, _tag_arr("expression may not be initialized", sizeof(
unsigned char), 34u), _tag_arr( _temp819, sizeof( void*), 0u));});}{ struct Cyc_List_List*
_temp820= Cyc_NewControlFlow_add_subscript_reln( _temp815, _temp482, _temp480);
if( _temp815 !=  _temp820){ _temp807=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp821=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp821[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp822; _temp822.tag= Cyc_CfFlowInfo_ReachableFL; _temp822.f1= _temp817;
_temp822.f2= _temp820; _temp822;}); _temp821;});} goto _LL810;} _LL814: goto
_LL810; _LL810:;}{ void* _temp823= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp482->topt))->v); _LL825: if(( unsigned int) _temp823 >  3u?*((
int*) _temp823) ==  Cyc_Absyn_TupleType: 0){ goto _LL826;} else{ goto _LL827;}
_LL827: goto _LL828; _LL826: { void* _temp829=( void*)(( struct Cyc_List_List*)
_check_null( _temp805))->hd; struct Cyc_Dict_Dict* _temp835; _LL831: if((
unsigned int) _temp829 >  3u?*(( int*) _temp829) ==  Cyc_CfFlowInfo_Aggregate: 0){
_LL836: _temp835=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp829)->f1; goto
_LL832;} else{ goto _LL833;} _LL833: goto _LL834; _LL832: return({ struct
_tuple0 _temp837; _temp837.f1= _temp807; _temp837.f2=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp835, Cyc_Absyn_fieldname(( int)
Cyc_Evexp_eval_const_uint_exp( _temp480))); _temp837;}); _LL834:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp838=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp838[ 0]=({ struct
Cyc_Core_Impossible_struct _temp839; _temp839.tag= Cyc_Core_Impossible; _temp839.f1=
_tag_arr("anal_Rexp: Subscript", sizeof( unsigned char), 21u); _temp839;});
_temp838;})); _LL830:;} _LL828: goto _LL824; _LL824:;} return Cyc_NewControlFlow_anal_derefR(
env, inflow, _temp807, _temp482,( void*)(( struct Cyc_List_List*) _check_null(
_temp805))->hd);} _LL356: _temp486= _temp484; goto _LL358; _LL358: { struct Cyc_List_List*
_temp842; void* _temp844; struct _tuple4 _temp840= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, _temp486); _LL845: _temp844= _temp840.f1; goto _LL843; _LL843:
_temp842= _temp840.f2; goto _LL841; _LL841: { struct Cyc_Dict_Dict* aggrdict=((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{ int i= 0; for( 0; _temp842 !=  0;(
_temp842= _temp842->tl, ++ i)){ aggrdict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( aggrdict, Cyc_Absyn_fieldname(
i),( void*) _temp842->hd);}} return({ struct _tuple0 _temp846; _temp846.f1=
_temp844; _temp846.f2=( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp847=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp847[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp848; _temp848.tag= Cyc_CfFlowInfo_Aggregate; _temp848.f1= aggrdict;
_temp848;}); _temp847;}); _temp846;});}} _LL360: _temp490= _temp488; goto _LL362;
_LL362: { struct Cyc_List_List* _temp851; void* _temp853; struct _tuple4
_temp849= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow,(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)( struct _tuple5*)) Cyc_Core_snd, _temp490)); _LL854:
_temp853= _temp849.f1; goto _LL852; _LL852: _temp851= _temp849.f2; goto _LL850;
_LL850: { struct Cyc_Dict_Dict* aggrdict=(( struct Cyc_Dict_Dict*(*)( int(* cmp)(
struct _tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{
int i= 0; for( 0; _temp851 !=  0;((( _temp851= _temp851->tl, _temp490= _temp490->tl)),
++ i)){ struct Cyc_List_List* ds=(*(( struct _tuple5*)(( struct Cyc_List_List*)
_check_null( _temp490))->hd)).f1; for( 0; ds !=  0; ds= ds->tl){ void* _temp855=(
void*) ds->hd; struct _tagged_arr* _temp861; _LL857: if(*(( int*) _temp855) == 
Cyc_Absyn_ArrayElement){ goto _LL858;} else{ goto _LL859;} _LL859: if(*(( int*)
_temp855) ==  Cyc_Absyn_FieldName){ _LL862: _temp861=(( struct Cyc_Absyn_FieldName_struct*)
_temp855)->f1; goto _LL860;} else{ goto _LL856;} _LL858:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp863=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp863[ 0]=({ struct
Cyc_Core_Impossible_struct _temp864; _temp864.tag= Cyc_Core_Impossible; _temp864.f1=
_tag_arr("anal_Rexp:Struct_e", sizeof( unsigned char), 19u); _temp864;});
_temp863;})); _LL860: aggrdict=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( aggrdict, _temp861,( void*)
_temp851->hd); _LL856:;}}} return({ struct _tuple0 _temp865; _temp865.f1=
_temp853; _temp865.f2=( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp866=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp866[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp867; _temp867.tag= Cyc_CfFlowInfo_Aggregate; _temp867.f1= aggrdict;
_temp867;}); _temp866;}); _temp865;});}} _LL364: { struct Cyc_List_List*
_temp868=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)( struct _tuple5*),
struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)( struct
_tuple5*)) Cyc_Core_snd, _temp492); struct Cyc_List_List* _temp871; void*
_temp873; struct _tuple4 _temp869= Cyc_NewControlFlow_anal_unordered_Rexps( env,
inflow, _temp868); _LL874: _temp873= _temp869.f1; goto _LL872; _LL872: _temp871=
_temp869.f2; goto _LL870; _LL870: for( 0; _temp871 !=  0;( _temp871= _temp871->tl,
_temp868= _temp868->tl)){ _temp873= Cyc_NewControlFlow_use_Rval( env,(( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp868))->hd)->loc,
_temp873,( void*) _temp871->hd);} return({ struct _tuple0 _temp875; _temp875.f1=
_temp873; _temp875.f2= Cyc_CfFlowInfo_typ_to_absrval(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, Cyc_CfFlowInfo_unknown_all); _temp875;});} _LL366: {
void* _temp878; void* _temp880; struct _tuple0 _temp876= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp496); _LL881: _temp880= _temp876.f1; goto _LL879; _LL879:
_temp878= _temp876.f2; goto _LL877; _LL877: { void* _temp882= _temp880; struct
Cyc_List_List* _temp888; struct Cyc_Dict_Dict* _temp890; _LL884: if( _temp882 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL885;} else{ goto _LL886;} _LL886: if((
unsigned int) _temp882 >  1u?*(( int*) _temp882) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL891: _temp890=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp882)->f1;
goto _LL889; _LL889: _temp888=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp882)->f2; goto _LL887;} else{ goto _LL883;} _LL885: return({ struct _tuple0
_temp892; _temp892.f1= _temp880; _temp892.f2= Cyc_CfFlowInfo_unknown_all;
_temp892;}); _LL887: if( Cyc_CfFlowInfo_initlevel( _temp890, _temp878) == ( void*)
Cyc_CfFlowInfo_NoneIL){({ void* _temp893[ 0u]={}; Cyc_Tcutil_terr( _temp496->loc,
_tag_arr("expression may not be initialized", sizeof( unsigned char), 34u),
_tag_arr( _temp893, sizeof( void*), 0u));});}{ struct Cyc_List_List* new_relns=
_temp888; comp_loop: { void* _temp894=( void*) _temp496->r; struct Cyc_Absyn_Exp*
_temp912; void* _temp915; struct Cyc_Absyn_Vardecl* _temp917; void* _temp920;
struct Cyc_Absyn_Vardecl* _temp922; void* _temp925; struct Cyc_Absyn_Vardecl*
_temp927; void* _temp930; struct Cyc_Absyn_Vardecl* _temp932; void* _temp934;
int _temp936; struct Cyc_List_List* _temp938; struct Cyc_List_List _temp940;
struct Cyc_Absyn_Exp* _temp941; void* _temp943; _LL896: if(*(( int*) _temp894)
==  Cyc_Absyn_Cast_e){ _LL913: _temp912=(( struct Cyc_Absyn_Cast_e_struct*)
_temp894)->f2; goto _LL897;} else{ goto _LL898;} _LL898: if(*(( int*) _temp894)
==  Cyc_Absyn_Var_e){ _LL916: _temp915=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp894)->f2; if(( unsigned int) _temp915 >  1u?*(( int*) _temp915) ==  Cyc_Absyn_Global_b:
0){ _LL918: _temp917=(( struct Cyc_Absyn_Global_b_struct*) _temp915)->f1; goto
_LL914;} else{ goto _LL900;}} else{ goto _LL900;} _LL914: if( ! _temp917->escapes){
goto _LL899;} else{ goto _LL900;} _LL900: if(*(( int*) _temp894) ==  Cyc_Absyn_Var_e){
_LL921: _temp920=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp894)->f2; if((
unsigned int) _temp920 >  1u?*(( int*) _temp920) ==  Cyc_Absyn_Local_b: 0){
_LL923: _temp922=(( struct Cyc_Absyn_Local_b_struct*) _temp920)->f1; goto _LL919;}
else{ goto _LL902;}} else{ goto _LL902;} _LL919: if( ! _temp922->escapes){ goto
_LL901;} else{ goto _LL902;} _LL902: if(*(( int*) _temp894) ==  Cyc_Absyn_Var_e){
_LL926: _temp925=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp894)->f2; if((
unsigned int) _temp925 >  1u?*(( int*) _temp925) ==  Cyc_Absyn_Pat_b: 0){ _LL928:
_temp927=(( struct Cyc_Absyn_Pat_b_struct*) _temp925)->f1; goto _LL924;} else{
goto _LL904;}} else{ goto _LL904;} _LL924: if( ! _temp927->escapes){ goto _LL903;}
else{ goto _LL904;} _LL904: if(*(( int*) _temp894) ==  Cyc_Absyn_Var_e){ _LL931:
_temp930=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp894)->f2; if((
unsigned int) _temp930 >  1u?*(( int*) _temp930) ==  Cyc_Absyn_Param_b: 0){
_LL933: _temp932=(( struct Cyc_Absyn_Param_b_struct*) _temp930)->f1; goto _LL929;}
else{ goto _LL906;}} else{ goto _LL906;} _LL929: if( ! _temp932->escapes){ goto
_LL905;} else{ goto _LL906;} _LL906: if(*(( int*) _temp894) ==  Cyc_Absyn_Const_e){
_LL935: _temp934=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp894)->f1; if((
unsigned int) _temp934 >  1u?*(( int*) _temp934) ==  Cyc_Absyn_Int_c: 0){ _LL937:
_temp936=(( struct Cyc_Absyn_Int_c_struct*) _temp934)->f2; goto _LL907;} else{
goto _LL908;}} else{ goto _LL908;} _LL908: if(*(( int*) _temp894) ==  Cyc_Absyn_Primop_e){
_LL944: _temp943=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp894)->f1;
if( _temp943 == ( void*) Cyc_Absyn_Size){ goto _LL939;} else{ goto _LL910;}
_LL939: _temp938=(( struct Cyc_Absyn_Primop_e_struct*) _temp894)->f2; if(
_temp938 ==  0){ goto _LL910;} else{ _temp940=* _temp938; _LL942: _temp941=(
struct Cyc_Absyn_Exp*) _temp940.hd; goto _LL909;}} else{ goto _LL910;} _LL910:
goto _LL911; _LL897: _temp496= _temp912; goto comp_loop; _LL899: _temp922=
_temp917; goto _LL901; _LL901: _temp927= _temp922; goto _LL903; _LL903: _temp932=
_temp927; goto _LL905; _LL905: new_relns=({ struct Cyc_List_List* _temp945=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp945->hd=(
void*)({ struct Cyc_CfFlowInfo_Reln* _temp946=( struct Cyc_CfFlowInfo_Reln*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln)); _temp946->vd= _temp498;
_temp946->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessVar_struct* _temp947=(
struct Cyc_CfFlowInfo_LessVar_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_LessVar_struct));
_temp947[ 0]=({ struct Cyc_CfFlowInfo_LessVar_struct _temp948; _temp948.tag= Cyc_CfFlowInfo_LessVar;
_temp948.f1= _temp932; _temp948;}); _temp947;})); _temp946;}); _temp945->tl=
_temp888; _temp945;}); goto _LL895; _LL907: new_relns=({ struct Cyc_List_List*
_temp949=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp949->hd=( void*)({ struct Cyc_CfFlowInfo_Reln* _temp950=( struct Cyc_CfFlowInfo_Reln*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln)); _temp950->vd= _temp498;
_temp950->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessConst_struct*
_temp951=( struct Cyc_CfFlowInfo_LessConst_struct*) _cycalloc_atomic( sizeof(
struct Cyc_CfFlowInfo_LessConst_struct)); _temp951[ 0]=({ struct Cyc_CfFlowInfo_LessConst_struct
_temp952; _temp952.tag= Cyc_CfFlowInfo_LessConst; _temp952.f1=( unsigned int)
_temp936; _temp952;}); _temp951;})); _temp950;}); _temp949->tl= _temp888;
_temp949;}); goto _LL895; _LL909:{ void* _temp953=( void*) _temp941->r; void*
_temp966; struct Cyc_Absyn_Vardecl* _temp968; void* _temp971; struct Cyc_Absyn_Vardecl*
_temp973; void* _temp976; struct Cyc_Absyn_Vardecl* _temp978; void* _temp981;
struct Cyc_Absyn_Vardecl* _temp983; _LL955: if(*(( int*) _temp953) ==  Cyc_Absyn_Var_e){
_LL967: _temp966=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp953)->f2; if((
unsigned int) _temp966 >  1u?*(( int*) _temp966) ==  Cyc_Absyn_Global_b: 0){
_LL969: _temp968=(( struct Cyc_Absyn_Global_b_struct*) _temp966)->f1; goto
_LL965;} else{ goto _LL957;}} else{ goto _LL957;} _LL965: if( ! _temp968->escapes){
goto _LL956;} else{ goto _LL957;} _LL957: if(*(( int*) _temp953) ==  Cyc_Absyn_Var_e){
_LL972: _temp971=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp953)->f2; if((
unsigned int) _temp971 >  1u?*(( int*) _temp971) ==  Cyc_Absyn_Local_b: 0){
_LL974: _temp973=(( struct Cyc_Absyn_Local_b_struct*) _temp971)->f1; goto _LL970;}
else{ goto _LL959;}} else{ goto _LL959;} _LL970: if( ! _temp973->escapes){ goto
_LL958;} else{ goto _LL959;} _LL959: if(*(( int*) _temp953) ==  Cyc_Absyn_Var_e){
_LL977: _temp976=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp953)->f2; if((
unsigned int) _temp976 >  1u?*(( int*) _temp976) ==  Cyc_Absyn_Pat_b: 0){ _LL979:
_temp978=(( struct Cyc_Absyn_Pat_b_struct*) _temp976)->f1; goto _LL975;} else{
goto _LL961;}} else{ goto _LL961;} _LL975: if( ! _temp978->escapes){ goto _LL960;}
else{ goto _LL961;} _LL961: if(*(( int*) _temp953) ==  Cyc_Absyn_Var_e){ _LL982:
_temp981=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp953)->f2; if((
unsigned int) _temp981 >  1u?*(( int*) _temp981) ==  Cyc_Absyn_Param_b: 0){
_LL984: _temp983=(( struct Cyc_Absyn_Param_b_struct*) _temp981)->f1; goto _LL980;}
else{ goto _LL963;}} else{ goto _LL963;} _LL980: if( ! _temp983->escapes){ goto
_LL962;} else{ goto _LL963;} _LL963: goto _LL964; _LL956: _temp973= _temp968;
goto _LL958; _LL958: _temp978= _temp973; goto _LL960; _LL960: _temp983= _temp978;
goto _LL962; _LL962: new_relns=({ struct Cyc_List_List* _temp985=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp985->hd=( void*)({ struct Cyc_CfFlowInfo_Reln*
_temp986=( struct Cyc_CfFlowInfo_Reln*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln));
_temp986->vd= _temp498; _temp986->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessSize_struct*
_temp987=( struct Cyc_CfFlowInfo_LessSize_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_LessSize_struct));
_temp987[ 0]=({ struct Cyc_CfFlowInfo_LessSize_struct _temp988; _temp988.tag=
Cyc_CfFlowInfo_LessSize; _temp988.f1= _temp983; _temp988;}); _temp987;}));
_temp986;}); _temp985->tl= _temp888; _temp985;}); goto _LL954; _LL964: goto
_LL954; _LL954:;} goto _LL895; _LL911: goto _LL895; _LL895:;} if( _temp888 != 
new_relns){ _temp880=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp989=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp989[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp990; _temp990.tag= Cyc_CfFlowInfo_ReachableFL; _temp990.f1= _temp890;
_temp990.f2= new_relns; _temp990;}); _temp989;});}{ void* _temp991= _temp878;
_LL993: if( _temp991 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL994;} else{ goto
_LL995;} _LL995: if( _temp991 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto
_LL996;} else{ goto _LL997;} _LL997: if( _temp991 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL998;} else{ goto _LL999;} _LL999: if(( unsigned int) _temp991 >  3u?*((
int*) _temp991) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL1000;} else{ goto
_LL1001;} _LL1001: goto _LL1002; _LL994: return({ struct _tuple0 _temp1003;
_temp1003.f1= _temp880; _temp1003.f2= Cyc_CfFlowInfo_unknown_all; _temp1003;});
_LL996: goto _LL998; _LL998: goto _LL1000; _LL1000: { struct Cyc_List_List
_temp1004=({ struct Cyc_List_List _temp1022; _temp1022.hd=( void*) _temp498;
_temp1022.tl= 0; _temp1022;}); _temp880= Cyc_NewControlFlow_add_vars( _temp880,(
struct Cyc_List_List*)& _temp1004, Cyc_CfFlowInfo_unknown_all);{ void* _temp1007;
void* _temp1009; struct _tuple0 _temp1005= Cyc_NewControlFlow_anal_Rexp( env,
_temp880, _temp494); _LL1010: _temp1009= _temp1005.f1; goto _LL1008; _LL1008:
_temp1007= _temp1005.f2; goto _LL1006; _LL1006:{ void* _temp1011= _temp1009;
struct Cyc_Dict_Dict* _temp1017; _LL1013: if( _temp1011 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1014;} else{ goto _LL1015;} _LL1015: if(( unsigned int) _temp1011 >  1u?*((
int*) _temp1011) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1018: _temp1017=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1011)->f1; goto _LL1016;} else{
goto _LL1012;} _LL1014: return({ struct _tuple0 _temp1019; _temp1019.f1=
_temp1009; _temp1019.f2= Cyc_CfFlowInfo_unknown_all; _temp1019;}); _LL1016: if(
Cyc_CfFlowInfo_initlevel( _temp1017, _temp1007) != ( void*) Cyc_CfFlowInfo_AllIL){({
void* _temp1020[ 0u]={}; Cyc_Tcutil_terr( _temp496->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1020, sizeof( void*), 0u));});
return({ struct _tuple0 _temp1021; _temp1021.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1021.f2= Cyc_CfFlowInfo_unknown_all; _temp1021;});} _LL1012:;} _temp880=
_temp1009; goto _LL1002;}} _LL1002: while( 1) { struct Cyc_List_List _temp1023=({
struct Cyc_List_List _temp1041; _temp1041.hd=( void*) _temp498; _temp1041.tl= 0;
_temp1041;}); _temp880= Cyc_NewControlFlow_add_vars( _temp880,( struct Cyc_List_List*)&
_temp1023, Cyc_CfFlowInfo_unknown_all);{ void* _temp1026; void* _temp1028;
struct _tuple0 _temp1024= Cyc_NewControlFlow_anal_Rexp( env, _temp880, _temp494);
_LL1029: _temp1028= _temp1024.f1; goto _LL1027; _LL1027: _temp1026= _temp1024.f2;
goto _LL1025; _LL1025:{ void* _temp1030= _temp1028; struct Cyc_Dict_Dict*
_temp1036; _LL1032: if( _temp1030 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1033;} else{ goto _LL1034;} _LL1034: if(( unsigned int) _temp1030 >  1u?*((
int*) _temp1030) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1037: _temp1036=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1030)->f1; goto _LL1035;} else{
goto _LL1031;} _LL1033: goto _LL1031; _LL1035: if( Cyc_CfFlowInfo_initlevel(
_temp1036, _temp1026) != ( void*) Cyc_CfFlowInfo_AllIL){({ void* _temp1038[ 0u]={};
Cyc_Tcutil_terr( _temp496->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1038, sizeof( void*), 0u));});
return({ struct _tuple0 _temp1039; _temp1039.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1039.f2= Cyc_CfFlowInfo_unknown_all; _temp1039;});} _LL1031:;}{ void*
_temp1040= Cyc_CfFlowInfo_join_flow( env->all_changed, _temp880, _temp1028); if(
Cyc_CfFlowInfo_flow_lessthan_approx( _temp1040, _temp880)){ break;} _temp880=
_temp1040;}}} return({ struct _tuple0 _temp1042; _temp1042.f1= _temp880;
_temp1042.f2= Cyc_CfFlowInfo_unknown_all; _temp1042;}); _LL992:;}} _LL883:;}}
_LL368: while( 1) { inflow=( void*)( Cyc_NewControlFlow_pre_stmt_check( env,
inflow, _temp500))->flow;{ void* _temp1043=( void*) _temp500->r; struct Cyc_Absyn_Stmt*
_temp1053; struct Cyc_Absyn_Stmt* _temp1055; struct Cyc_Absyn_Stmt* _temp1057;
struct Cyc_Absyn_Decl* _temp1059; struct Cyc_Absyn_Exp* _temp1061; _LL1045: if((
unsigned int) _temp1043 >  1u?*(( int*) _temp1043) ==  Cyc_Absyn_Seq_s: 0){
_LL1056: _temp1055=(( struct Cyc_Absyn_Seq_s_struct*) _temp1043)->f1; goto
_LL1054; _LL1054: _temp1053=(( struct Cyc_Absyn_Seq_s_struct*) _temp1043)->f2;
goto _LL1046;} else{ goto _LL1047;} _LL1047: if(( unsigned int) _temp1043 >  1u?*((
int*) _temp1043) ==  Cyc_Absyn_Decl_s: 0){ _LL1060: _temp1059=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1043)->f1; goto _LL1058; _LL1058: _temp1057=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1043)->f2; goto _LL1048;} else{ goto _LL1049;} _LL1049: if(( unsigned int)
_temp1043 >  1u?*(( int*) _temp1043) ==  Cyc_Absyn_Exp_s: 0){ _LL1062: _temp1061=((
struct Cyc_Absyn_Exp_s_struct*) _temp1043)->f1; goto _LL1050;} else{ goto
_LL1051;} _LL1051: goto _LL1052; _LL1046: inflow= Cyc_NewControlFlow_anal_stmt(
env, inflow, _temp1055); _temp500= _temp1053; goto _LL1044; _LL1048: inflow= Cyc_NewControlFlow_anal_decl(
env, inflow, _temp1059); _temp500= _temp1057; goto _LL1044; _LL1050: return Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1061); _LL1052:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1063=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1063[ 0]=({ struct Cyc_Core_Impossible_struct _temp1064; _temp1064.tag= Cyc_Core_Impossible;
_temp1064.f1= _tag_arr("analyze_Rexp: ill-formed StmtExp", sizeof( unsigned char),
33u); _temp1064;}); _temp1063;})); _LL1044:;}} _LL370: goto _LL372; _LL372: goto
_LL374; _LL374: goto _LL376; _LL376: goto _LL378; _LL378: goto _LL380; _LL380:
goto _LL382; _LL382:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1065=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1065[ 0]=({ struct Cyc_Core_Impossible_struct _temp1066; _temp1066.tag= Cyc_Core_Impossible;
_temp1066.f1= _tag_arr("anal_Rexp, unexpected exp form", sizeof( unsigned char),
31u); _temp1066;}); _temp1065;})); _LL286:;}} static struct _tuple0 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, void* f, struct Cyc_Absyn_Exp*
e, void* r, struct Cyc_List_List* flds){ void* _temp1067= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); struct Cyc_Absyn_PtrInfo
_temp1073; struct Cyc_Absyn_Conref* _temp1075; void* _temp1077; _LL1069: if((
unsigned int) _temp1067 >  3u?*(( int*) _temp1067) ==  Cyc_Absyn_PointerType: 0){
_LL1074: _temp1073=(( struct Cyc_Absyn_PointerType_struct*) _temp1067)->f1;
_LL1078: _temp1077=( void*) _temp1073.elt_typ; goto _LL1076; _LL1076: _temp1075=
_temp1073.bounds; goto _LL1070;} else{ goto _LL1071;} _LL1071: goto _LL1072;
_LL1070: { void* _temp1079= f; struct Cyc_List_List* _temp1085; struct Cyc_Dict_Dict*
_temp1087; _LL1081: if( _temp1079 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto
_LL1082;} else{ goto _LL1083;} _LL1083: if(( unsigned int) _temp1079 >  1u?*((
int*) _temp1079) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1088: _temp1087=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1079)->f1; goto _LL1086; _LL1086:
_temp1085=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1079)->f2; goto
_LL1084;} else{ goto _LL1080;} _LL1082: return({ struct _tuple0 _temp1089;
_temp1089.f1= f; _temp1089.f2=( void*) Cyc_CfFlowInfo_UnknownL; _temp1089;});
_LL1084: if( Cyc_Tcutil_is_bound_one( _temp1075)){ void* _temp1090= r; struct
Cyc_CfFlowInfo_Place* _temp1104; struct Cyc_CfFlowInfo_Place _temp1106; struct
Cyc_List_List* _temp1107; void* _temp1109; void* _temp1111; _LL1092: if(
_temp1090 == ( void*) Cyc_CfFlowInfo_NotZeroAll){ goto _LL1093;} else{ goto
_LL1094;} _LL1094: if( _temp1090 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto
_LL1095;} else{ goto _LL1096;} _LL1096: if(( unsigned int) _temp1090 >  3u?*((
int*) _temp1090) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL1105: _temp1104=(( struct
Cyc_CfFlowInfo_AddressOf_struct*) _temp1090)->f1; _temp1106=* _temp1104; _LL1110:
_temp1109=( void*) _temp1106.root; goto _LL1108; _LL1108: _temp1107= _temp1106.fields;
goto _LL1097;} else{ goto _LL1098;} _LL1098: if( _temp1090 == ( void*) Cyc_CfFlowInfo_Zero){
goto _LL1099;} else{ goto _LL1100;} _LL1100: if(( unsigned int) _temp1090 >  3u?*((
int*) _temp1090) ==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1112: _temp1111=( void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*) _temp1090)->f1; goto _LL1101;} else{
goto _LL1102;} _LL1102: goto _LL1103; _LL1093: goto _LL1095; _LL1095:( void*)( e->annot=(
void*)(( void*)({ struct Cyc_CfFlowInfo_NotZero_struct* _temp1113=( struct Cyc_CfFlowInfo_NotZero_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_NotZero_struct)); _temp1113[ 0]=({
struct Cyc_CfFlowInfo_NotZero_struct _temp1114; _temp1114.tag= Cyc_CfFlowInfo_NotZero;
_temp1114.f1= _temp1085; _temp1114;}); _temp1113;}))); goto _LL1091; _LL1097:(
void*)( e->annot=( void*)(( void*)({ struct Cyc_CfFlowInfo_NotZero_struct*
_temp1115=( struct Cyc_CfFlowInfo_NotZero_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_NotZero_struct));
_temp1115[ 0]=({ struct Cyc_CfFlowInfo_NotZero_struct _temp1116; _temp1116.tag=
Cyc_CfFlowInfo_NotZero; _temp1116.f1= _temp1085; _temp1116;}); _temp1115;})));
return({ struct _tuple0 _temp1117; _temp1117.f1= f; _temp1117.f2=( void*)({
struct Cyc_CfFlowInfo_PlaceL_struct* _temp1118=( struct Cyc_CfFlowInfo_PlaceL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_PlaceL_struct)); _temp1118[ 0]=({
struct Cyc_CfFlowInfo_PlaceL_struct _temp1119; _temp1119.tag= Cyc_CfFlowInfo_PlaceL;
_temp1119.f1=({ struct Cyc_CfFlowInfo_Place* _temp1120=( struct Cyc_CfFlowInfo_Place*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Place)); _temp1120->root=( void*)
_temp1109; _temp1120->fields=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1107, flds); _temp1120;});
_temp1119;}); _temp1118;}); _temp1117;}); _LL1099:( void*)( e->annot=( void*)((
void*)({ struct Cyc_CfFlowInfo_IsZero_struct* _temp1121=( struct Cyc_CfFlowInfo_IsZero_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_IsZero_struct)); _temp1121[ 0]=({
struct Cyc_CfFlowInfo_IsZero_struct _temp1122; _temp1122.tag= Cyc_CfFlowInfo_IsZero;
_temp1122.f1= _temp1085; _temp1122;}); _temp1121;}))); return({ struct _tuple0
_temp1123; _temp1123.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp1123.f2=( void*)
Cyc_CfFlowInfo_UnknownL; _temp1123;}); _LL1101: f= Cyc_NewControlFlow_notzero(
env, inflow, f, e, _temp1111); goto _LL1103; _LL1103:( void*)( e->annot=( void*)((
void*)({ struct Cyc_CfFlowInfo_UnknownZ_struct* _temp1124=( struct Cyc_CfFlowInfo_UnknownZ_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_UnknownZ_struct)); _temp1124[ 0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _temp1125; _temp1125.tag= Cyc_CfFlowInfo_UnknownZ;
_temp1125.f1= _temp1085; _temp1125;}); _temp1124;}))); _LL1091:;} else{( void*)(
e->annot=( void*)(( void*)({ struct Cyc_CfFlowInfo_UnknownZ_struct* _temp1126=(
struct Cyc_CfFlowInfo_UnknownZ_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_UnknownZ_struct));
_temp1126[ 0]=({ struct Cyc_CfFlowInfo_UnknownZ_struct _temp1127; _temp1127.tag=
Cyc_CfFlowInfo_UnknownZ; _temp1127.f1= _temp1085; _temp1127;}); _temp1126;})));}
if( Cyc_CfFlowInfo_initlevel( _temp1087, r) == ( void*) Cyc_CfFlowInfo_NoneIL){({
void* _temp1128[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("dereference of possibly uninitialized pointer",
sizeof( unsigned char), 46u), _tag_arr( _temp1128, sizeof( void*), 0u));});}
return({ struct _tuple0 _temp1129; _temp1129.f1= f; _temp1129.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp1129;}); _LL1080:;} _LL1072:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1130=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1130[ 0]=({ struct Cyc_Core_Impossible_struct _temp1131; _temp1131.tag= Cyc_Core_Impossible;
_temp1131.f1= _tag_arr("left deref of non-pointer-type", sizeof( unsigned char),
31u); _temp1131;}); _temp1130;})); _LL1068:;} static struct _tuple0 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* flds){ struct Cyc_Dict_Dict* d;{ void* _temp1132= inflow;
struct Cyc_List_List* _temp1138; struct Cyc_Dict_Dict* _temp1140; _LL1134: if(
_temp1132 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1135;} else{ goto
_LL1136;} _LL1136: if(( unsigned int) _temp1132 >  1u?*(( int*) _temp1132) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL1141: _temp1140=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1132)->f1; goto _LL1139; _LL1139: _temp1138=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1132)->f2; goto _LL1137;} else{ goto _LL1133;} _LL1135: return({ struct
_tuple0 _temp1142; _temp1142.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp1142.f2=(
void*) Cyc_CfFlowInfo_UnknownL; _temp1142;}); _LL1137: d= _temp1140; _LL1133:;}{
void* _temp1143=( void*) e->r; struct Cyc_Absyn_Exp* _temp1169; struct Cyc_Absyn_Exp*
_temp1171; struct Cyc_Absyn_Exp* _temp1173; void* _temp1175; struct Cyc_Absyn_Vardecl*
_temp1177; void* _temp1179; struct Cyc_Absyn_Vardecl* _temp1181; void* _temp1183;
struct Cyc_Absyn_Vardecl* _temp1185; void* _temp1187; struct Cyc_Absyn_Vardecl*
_temp1189; struct _tagged_arr* _temp1191; struct Cyc_Absyn_Exp* _temp1193;
struct Cyc_Absyn_Exp* _temp1195; struct Cyc_Absyn_Exp* _temp1197; struct Cyc_Absyn_Exp*
_temp1199; struct _tagged_arr* _temp1201; struct Cyc_Absyn_Exp* _temp1203;
_LL1145: if(*(( int*) _temp1143) ==  Cyc_Absyn_Cast_e){ _LL1170: _temp1169=((
struct Cyc_Absyn_Cast_e_struct*) _temp1143)->f2; goto _LL1146;} else{ goto
_LL1147;} _LL1147: if(*(( int*) _temp1143) ==  Cyc_Absyn_NoInstantiate_e){
_LL1172: _temp1171=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1143)->f1;
goto _LL1148;} else{ goto _LL1149;} _LL1149: if(*(( int*) _temp1143) ==  Cyc_Absyn_Instantiate_e){
_LL1174: _temp1173=(( struct Cyc_Absyn_Instantiate_e_struct*) _temp1143)->f1;
goto _LL1150;} else{ goto _LL1151;} _LL1151: if(*(( int*) _temp1143) ==  Cyc_Absyn_Var_e){
_LL1176: _temp1175=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1143)->f2;
if(( unsigned int) _temp1175 >  1u?*(( int*) _temp1175) ==  Cyc_Absyn_Param_b: 0){
_LL1178: _temp1177=(( struct Cyc_Absyn_Param_b_struct*) _temp1175)->f1; goto
_LL1152;} else{ goto _LL1153;}} else{ goto _LL1153;} _LL1153: if(*(( int*)
_temp1143) ==  Cyc_Absyn_Var_e){ _LL1180: _temp1179=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1143)->f2; if(( unsigned int) _temp1179 >  1u?*(( int*) _temp1179) ==  Cyc_Absyn_Local_b:
0){ _LL1182: _temp1181=(( struct Cyc_Absyn_Local_b_struct*) _temp1179)->f1; goto
_LL1154;} else{ goto _LL1155;}} else{ goto _LL1155;} _LL1155: if(*(( int*)
_temp1143) ==  Cyc_Absyn_Var_e){ _LL1184: _temp1183=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1143)->f2; if(( unsigned int) _temp1183 >  1u?*(( int*) _temp1183) ==  Cyc_Absyn_Pat_b:
0){ _LL1186: _temp1185=(( struct Cyc_Absyn_Pat_b_struct*) _temp1183)->f1; goto
_LL1156;} else{ goto _LL1157;}} else{ goto _LL1157;} _LL1157: if(*(( int*)
_temp1143) ==  Cyc_Absyn_Var_e){ _LL1188: _temp1187=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1143)->f2; if(( unsigned int) _temp1187 >  1u?*(( int*) _temp1187) ==  Cyc_Absyn_Global_b:
0){ _LL1190: _temp1189=(( struct Cyc_Absyn_Global_b_struct*) _temp1187)->f1;
goto _LL1158;} else{ goto _LL1159;}} else{ goto _LL1159;} _LL1159: if(*(( int*)
_temp1143) ==  Cyc_Absyn_StructArrow_e){ _LL1194: _temp1193=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1143)->f1; goto _LL1192; _LL1192: _temp1191=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1143)->f2; goto _LL1160;} else{ goto _LL1161;} _LL1161: if(*(( int*)
_temp1143) ==  Cyc_Absyn_Deref_e){ _LL1196: _temp1195=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1143)->f1; goto _LL1162;} else{ goto _LL1163;} _LL1163: if(*(( int*)
_temp1143) ==  Cyc_Absyn_Subscript_e){ _LL1200: _temp1199=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1143)->f1; goto _LL1198; _LL1198: _temp1197=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1143)->f2; goto _LL1164;} else{ goto _LL1165;} _LL1165: if(*(( int*)
_temp1143) ==  Cyc_Absyn_StructMember_e){ _LL1204: _temp1203=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1143)->f1; goto _LL1202; _LL1202: _temp1201=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1143)->f2; goto _LL1166;} else{ goto _LL1167;} _LL1167: goto _LL1168;
_LL1146: _temp1171= _temp1169; goto _LL1148; _LL1148: _temp1173= _temp1171; goto
_LL1150; _LL1150: return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow,
_temp1173, flds); _LL1152: _temp1181= _temp1177; goto _LL1154; _LL1154:
_temp1185= _temp1181; goto _LL1156; _LL1156: return({ struct _tuple0 _temp1205;
_temp1205.f1= inflow; _temp1205.f2=( void*)({ struct Cyc_CfFlowInfo_PlaceL_struct*
_temp1206=( struct Cyc_CfFlowInfo_PlaceL_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_PlaceL_struct));
_temp1206[ 0]=({ struct Cyc_CfFlowInfo_PlaceL_struct _temp1207; _temp1207.tag=
Cyc_CfFlowInfo_PlaceL; _temp1207.f1=({ struct Cyc_CfFlowInfo_Place* _temp1208=(
struct Cyc_CfFlowInfo_Place*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Place));
_temp1208->root=( void*)(( void*)({ struct Cyc_CfFlowInfo_VarRoot_struct*
_temp1209=( struct Cyc_CfFlowInfo_VarRoot_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct));
_temp1209[ 0]=({ struct Cyc_CfFlowInfo_VarRoot_struct _temp1210; _temp1210.tag=
Cyc_CfFlowInfo_VarRoot; _temp1210.f1= _temp1185; _temp1210;}); _temp1209;}));
_temp1208->fields= flds; _temp1208;}); _temp1207;}); _temp1206;}); _temp1205;});
_LL1158: return({ struct _tuple0 _temp1211; _temp1211.f1= inflow; _temp1211.f2=(
void*) Cyc_CfFlowInfo_UnknownL; _temp1211;}); _LL1160:{ void* _temp1212= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1193->topt))->v); _LL1214: if((
unsigned int) _temp1212 >  3u?*(( int*) _temp1212) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL1215;} else{ goto _LL1216;} _LL1216: if(( unsigned int) _temp1212 > 
3u?*(( int*) _temp1212) ==  Cyc_Absyn_UnionType: 0){ goto _LL1217;} else{ goto
_LL1218;} _LL1218: goto _LL1219; _LL1215: goto _LL1217; _LL1217: goto _LL1213;
_LL1219: flds=({ struct Cyc_List_List* _temp1220=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1220->hd=( void*) _temp1191;
_temp1220->tl= flds; _temp1220;}); _LL1213:;} _temp1195= _temp1193; goto _LL1162;
_LL1162: { void* _temp1223; void* _temp1225; struct _tuple0 _temp1221= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1195); _LL1226: _temp1225= _temp1221.f1; goto _LL1224; _LL1224:
_temp1223= _temp1221.f2; goto _LL1222; _LL1222: return Cyc_NewControlFlow_anal_derefL(
env, inflow, _temp1225, _temp1195, _temp1223, flds);} _LL1164:{ void* _temp1227=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1199->topt))->v);
_LL1229: if(( unsigned int) _temp1227 >  3u?*(( int*) _temp1227) ==  Cyc_Absyn_TupleType:
0){ goto _LL1230;} else{ goto _LL1231;} _LL1231: goto _LL1232; _LL1230: { struct
_tagged_arr* _temp1233= Cyc_Absyn_fieldname(( int) Cyc_Evexp_eval_const_uint_exp(
_temp1197)); return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, _temp1199,({
struct Cyc_List_List* _temp1234=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1234->hd=( void*) _temp1233; _temp1234->tl= flds;
_temp1234;}));} _LL1232: goto _LL1228; _LL1228:;}{ struct Cyc_List_List*
_temp1238; void* _temp1240; struct _tuple4 _temp1236= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow,({ struct Cyc_Absyn_Exp* _temp1235[ 2u]; _temp1235[ 1u]= _temp1197;
_temp1235[ 0u]= _temp1199;(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp1235, sizeof( struct Cyc_Absyn_Exp*), 2u));})); _LL1241:
_temp1240= _temp1236.f1; goto _LL1239; _LL1239: _temp1238= _temp1236.f2; goto
_LL1237; _LL1237:{ void* _temp1242= _temp1240; struct Cyc_List_List* _temp1248;
struct Cyc_Dict_Dict* _temp1250; _LL1244: if(( unsigned int) _temp1242 >  1u?*((
int*) _temp1242) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1251: _temp1250=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1242)->f1; goto _LL1249; _LL1249:
_temp1248=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1242)->f2; goto
_LL1245;} else{ goto _LL1246;} _LL1246: goto _LL1247; _LL1245: if( Cyc_CfFlowInfo_initlevel(
_temp1250,( void*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1238))->tl))->hd) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void*
_temp1252[ 0u]={}; Cyc_Tcutil_terr( _temp1197->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1252, sizeof( void*), 0u));});}{
struct Cyc_List_List* _temp1253= Cyc_NewControlFlow_add_subscript_reln(
_temp1248, _temp1199, _temp1197); if( _temp1248 !=  _temp1253){ _temp1240=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp1254=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp1254[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp1255; _temp1255.tag= Cyc_CfFlowInfo_ReachableFL;
_temp1255.f1= _temp1250; _temp1255.f2= _temp1253; _temp1255;}); _temp1254;});}
goto _LL1243;} _LL1247: goto _LL1243; _LL1243:;} return Cyc_NewControlFlow_anal_derefL(
env, inflow, _temp1240, _temp1199,( void*)(( struct Cyc_List_List*) _check_null(
_temp1238))->hd, flds);} _LL1166:{ void* _temp1256= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1203->topt))->v); _LL1258: if((
unsigned int) _temp1256 >  3u?*(( int*) _temp1256) ==  Cyc_Absyn_AnonUnionType:
0){ goto _LL1259;} else{ goto _LL1260;} _LL1260: if(( unsigned int) _temp1256 > 
3u?*(( int*) _temp1256) ==  Cyc_Absyn_UnionType: 0){ goto _LL1261;} else{ goto
_LL1262;} _LL1262: goto _LL1263; _LL1259: goto _LL1261; _LL1261: return({ struct
_tuple0 _temp1264; _temp1264.f1= inflow; _temp1264.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp1264;}); _LL1263: flds=({ struct Cyc_List_List* _temp1265=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp1265->hd=( void*) _temp1201;
_temp1265->tl= flds; _temp1265;}); _LL1257:;} return Cyc_NewControlFlow_anal_Lexp_rec(
env, inflow, _temp1203, flds); _LL1168: return({ struct _tuple0 _temp1266;
_temp1266.f1=( void*) Cyc_CfFlowInfo_BottomFL; _temp1266.f2=( void*) Cyc_CfFlowInfo_UnknownL;
_temp1266;}); _LL1144:;}} static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_Absyn_Exp* e){
return Cyc_NewControlFlow_anal_Lexp_rec( env, inflow, e, 0);} static struct
_tuple0 Cyc_NewControlFlow_anal_test( struct Cyc_NewControlFlow_AnalEnv* env,
void* inflow, struct Cyc_Absyn_Exp* e){{ void* _temp1267=( void*) e->r; struct
Cyc_Absyn_Exp* _temp1285; struct Cyc_Absyn_Exp* _temp1287; struct Cyc_Absyn_Exp*
_temp1289; struct Cyc_Absyn_Exp* _temp1291; struct Cyc_Absyn_Exp* _temp1293;
struct Cyc_List_List* _temp1295; struct Cyc_List_List _temp1297; struct Cyc_List_List*
_temp1298; struct Cyc_Absyn_Exp* _temp1300; void* _temp1302; struct Cyc_List_List*
_temp1304; void* _temp1306; struct Cyc_List_List* _temp1308; void* _temp1310;
struct Cyc_List_List* _temp1312; void* _temp1314; struct Cyc_List_List*
_temp1316; void* _temp1318; _LL1269: if(*(( int*) _temp1267) ==  Cyc_Absyn_Conditional_e){
_LL1290: _temp1289=(( struct Cyc_Absyn_Conditional_e_struct*) _temp1267)->f1;
goto _LL1288; _LL1288: _temp1287=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1267)->f2; goto _LL1286; _LL1286: _temp1285=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1267)->f3; goto _LL1270;} else{ goto _LL1271;} _LL1271: if(*(( int*)
_temp1267) ==  Cyc_Absyn_SeqExp_e){ _LL1294: _temp1293=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1267)->f1; goto _LL1292; _LL1292: _temp1291=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1267)->f2; goto _LL1272;} else{ goto _LL1273;} _LL1273: if(*(( int*)
_temp1267) ==  Cyc_Absyn_Primop_e){ _LL1303: _temp1302=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1267)->f1; if( _temp1302 == ( void*) Cyc_Absyn_Not){ goto _LL1296;} else{
goto _LL1275;} _LL1296: _temp1295=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1267)->f2; if( _temp1295 ==  0){ goto _LL1275;} else{ _temp1297=* _temp1295;
_LL1301: _temp1300=( struct Cyc_Absyn_Exp*) _temp1297.hd; goto _LL1299; _LL1299:
_temp1298= _temp1297.tl; if( _temp1298 ==  0){ goto _LL1274;} else{ goto _LL1275;}}}
else{ goto _LL1275;} _LL1275: if(*(( int*) _temp1267) ==  Cyc_Absyn_Primop_e){
_LL1307: _temp1306=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1267)->f1;
if( _temp1306 == ( void*) Cyc_Absyn_Eq){ goto _LL1305;} else{ goto _LL1277;}
_LL1305: _temp1304=(( struct Cyc_Absyn_Primop_e_struct*) _temp1267)->f2; goto
_LL1276;} else{ goto _LL1277;} _LL1277: if(*(( int*) _temp1267) ==  Cyc_Absyn_Primop_e){
_LL1311: _temp1310=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1267)->f1;
if( _temp1310 == ( void*) Cyc_Absyn_Neq){ goto _LL1309;} else{ goto _LL1279;}
_LL1309: _temp1308=(( struct Cyc_Absyn_Primop_e_struct*) _temp1267)->f2; goto
_LL1278;} else{ goto _LL1279;} _LL1279: if(*(( int*) _temp1267) ==  Cyc_Absyn_Primop_e){
_LL1315: _temp1314=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1267)->f1;
if( _temp1314 == ( void*) Cyc_Absyn_Lt){ goto _LL1313;} else{ goto _LL1281;}
_LL1313: _temp1312=(( struct Cyc_Absyn_Primop_e_struct*) _temp1267)->f2; goto
_LL1280;} else{ goto _LL1281;} _LL1281: if(*(( int*) _temp1267) ==  Cyc_Absyn_Primop_e){
_LL1319: _temp1318=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp1267)->f1;
if( _temp1318 == ( void*) Cyc_Absyn_Lte){ goto _LL1317;} else{ goto _LL1283;}
_LL1317: _temp1316=(( struct Cyc_Absyn_Primop_e_struct*) _temp1267)->f2; goto
_LL1282;} else{ goto _LL1283;} _LL1283: goto _LL1284; _LL1270: { void* _temp1322;
void* _temp1324; struct _tuple0 _temp1320= Cyc_NewControlFlow_anal_test( env,
inflow, _temp1289); _LL1325: _temp1324= _temp1320.f1; goto _LL1323; _LL1323:
_temp1322= _temp1320.f2; goto _LL1321; _LL1321: { void* _temp1328; void*
_temp1330; struct _tuple0 _temp1326= Cyc_NewControlFlow_anal_test( env,
_temp1324, _temp1287); _LL1331: _temp1330= _temp1326.f1; goto _LL1329; _LL1329:
_temp1328= _temp1326.f2; goto _LL1327; _LL1327: { void* _temp1334; void*
_temp1336; struct _tuple0 _temp1332= Cyc_NewControlFlow_anal_test( env,
_temp1322, _temp1285); _LL1337: _temp1336= _temp1332.f1; goto _LL1335; _LL1335:
_temp1334= _temp1332.f2; goto _LL1333; _LL1333: return({ struct _tuple0
_temp1338; _temp1338.f1= Cyc_CfFlowInfo_join_flow( env->all_changed, _temp1330,
_temp1336); _temp1338.f2= Cyc_CfFlowInfo_join_flow( env->all_changed, _temp1328,
_temp1334); _temp1338;});}}} _LL1272: return Cyc_NewControlFlow_anal_test( env,(
Cyc_NewControlFlow_anal_Rexp( env, inflow, _temp1293)).f1, _temp1291); _LL1274: {
void* _temp1341; void* _temp1343; struct _tuple0 _temp1339= Cyc_NewControlFlow_anal_test(
env, inflow, _temp1300); _LL1344: _temp1343= _temp1339.f1; goto _LL1342; _LL1342:
_temp1341= _temp1339.f2; goto _LL1340; _LL1340: return({ struct _tuple0
_temp1345; _temp1345.f1= _temp1341; _temp1345.f2= _temp1343; _temp1345;});}
_LL1276: { struct Cyc_List_List* _temp1348; void* _temp1350; struct _tuple4
_temp1346= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1304);
_LL1351: _temp1350= _temp1346.f1; goto _LL1349; _LL1349: _temp1348= _temp1346.f2;
goto _LL1347; _LL1347: { void* _temp1352= _temp1350; struct Cyc_List_List*
_temp1358; struct Cyc_Dict_Dict* _temp1360; _LL1354: if( _temp1352 == ( void*)
Cyc_CfFlowInfo_BottomFL){ goto _LL1355;} else{ goto _LL1356;} _LL1356: if((
unsigned int) _temp1352 >  1u?*(( int*) _temp1352) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL1361: _temp1360=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1352)->f1;
goto _LL1359; _LL1359: _temp1358=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1352)->f2; goto _LL1357;} else{ goto _LL1353;} _LL1355: return({ struct
_tuple0 _temp1362; _temp1362.f1= _temp1350; _temp1362.f2= _temp1350; _temp1362;});
_LL1357: { void* _temp1363=( void*)(( struct Cyc_List_List*) _check_null(
_temp1348))->hd; void* _temp1364=( void*)(( struct Cyc_List_List*) _check_null(
_temp1348->tl))->hd; if( Cyc_CfFlowInfo_initlevel( _temp1360, _temp1363) == (
void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1365[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1304))->hd)->loc,
_tag_arr("expression may not be initialized", sizeof( unsigned char), 34u),
_tag_arr( _temp1365, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1360, _temp1364) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1366[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1304))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1366, sizeof( void*), 0u));});}{
struct _tuple0 _temp1368=({ struct _tuple0 _temp1367; _temp1367.f1= _temp1363;
_temp1367.f2= _temp1364; _temp1367;}); void* _temp1376; void* _temp1378; void*
_temp1380; void* _temp1382; void* _temp1384; void* _temp1386; _LL1370: _LL1379:
_temp1378= _temp1368.f1; if(( unsigned int) _temp1378 >  3u?*(( int*) _temp1378)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1381: _temp1380=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1378)->f1; goto _LL1377;} else{ goto _LL1372;} _LL1377: _temp1376=
_temp1368.f2; if( _temp1376 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1371;}
else{ goto _LL1372;} _LL1372: _LL1387: _temp1386= _temp1368.f1; if( _temp1386 == (
void*) Cyc_CfFlowInfo_Zero){ goto _LL1383;} else{ goto _LL1374;} _LL1383:
_temp1382= _temp1368.f2; if(( unsigned int) _temp1382 >  3u?*(( int*) _temp1382)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1385: _temp1384=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1382)->f1; goto _LL1373;} else{ goto _LL1374;} _LL1374: goto _LL1375;
_LL1371: { void* _temp1390; void* _temp1392; struct _tuple0 _temp1388= Cyc_NewControlFlow_splitzero(
env, _temp1350, _temp1350,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1304))->hd, _temp1380); _LL1393: _temp1392= _temp1388.f1; goto
_LL1391; _LL1391: _temp1390= _temp1388.f2; goto _LL1389; _LL1389: return({
struct _tuple0 _temp1394; _temp1394.f1= _temp1390; _temp1394.f2= _temp1392;
_temp1394;});} _LL1373: { void* _temp1397; void* _temp1399; struct _tuple0
_temp1395= Cyc_NewControlFlow_splitzero( env, _temp1350, _temp1350,( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null((( struct Cyc_List_List*) _check_null(
_temp1304))->tl))->hd, _temp1384); _LL1400: _temp1399= _temp1395.f1; goto
_LL1398; _LL1398: _temp1397= _temp1395.f2; goto _LL1396; _LL1396: return({
struct _tuple0 _temp1401; _temp1401.f1= _temp1397; _temp1401.f2= _temp1399;
_temp1401;});} _LL1375: return({ struct _tuple0 _temp1402; _temp1402.f1=
_temp1350; _temp1402.f2= _temp1350; _temp1402;}); _LL1369:;}} _LL1353:;}}
_LL1278: { struct Cyc_List_List* _temp1405; void* _temp1407; struct _tuple4
_temp1403= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1308);
_LL1408: _temp1407= _temp1403.f1; goto _LL1406; _LL1406: _temp1405= _temp1403.f2;
goto _LL1404; _LL1404: { void* _temp1409= _temp1407; struct Cyc_List_List*
_temp1415; struct Cyc_Dict_Dict* _temp1417; _LL1411: if( _temp1409 == ( void*)
Cyc_CfFlowInfo_BottomFL){ goto _LL1412;} else{ goto _LL1413;} _LL1413: if((
unsigned int) _temp1409 >  1u?*(( int*) _temp1409) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL1418: _temp1417=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1409)->f1;
goto _LL1416; _LL1416: _temp1415=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1409)->f2; goto _LL1414;} else{ goto _LL1410;} _LL1412: return({ struct
_tuple0 _temp1419; _temp1419.f1= _temp1407; _temp1419.f2= _temp1407; _temp1419;});
_LL1414: { void* _temp1420=( void*)(( struct Cyc_List_List*) _check_null(
_temp1405))->hd; void* _temp1421=( void*)(( struct Cyc_List_List*) _check_null(
_temp1405->tl))->hd; if( Cyc_CfFlowInfo_initlevel( _temp1417, _temp1420) == (
void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1422[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1308))->hd)->loc,
_tag_arr("expression may not be initialized", sizeof( unsigned char), 34u),
_tag_arr( _temp1422, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1417, _temp1421) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1423[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1308))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1423, sizeof( void*), 0u));});}{
struct _tuple0 _temp1425=({ struct _tuple0 _temp1424; _temp1424.f1= _temp1420;
_temp1424.f2= _temp1421; _temp1424;}); void* _temp1433; void* _temp1435; void*
_temp1437; void* _temp1439; void* _temp1441; void* _temp1443; _LL1427: _LL1436:
_temp1435= _temp1425.f1; if(( unsigned int) _temp1435 >  3u?*(( int*) _temp1435)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1438: _temp1437=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1435)->f1; goto _LL1434;} else{ goto _LL1429;} _LL1434: _temp1433=
_temp1425.f2; if( _temp1433 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1428;}
else{ goto _LL1429;} _LL1429: _LL1444: _temp1443= _temp1425.f1; if( _temp1443 == (
void*) Cyc_CfFlowInfo_Zero){ goto _LL1440;} else{ goto _LL1431;} _LL1440:
_temp1439= _temp1425.f2; if(( unsigned int) _temp1439 >  3u?*(( int*) _temp1439)
==  Cyc_CfFlowInfo_UnknownR: 0){ _LL1442: _temp1441=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1439)->f1; goto _LL1430;} else{ goto _LL1431;} _LL1431: goto _LL1432;
_LL1428: return Cyc_NewControlFlow_splitzero( env, _temp1407, _temp1407,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1308))->hd, _temp1437);
_LL1430: return Cyc_NewControlFlow_splitzero( env, _temp1407, _temp1407,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( _temp1308))->tl))->hd, _temp1441); _LL1432: return({ struct _tuple0
_temp1445; _temp1445.f1= _temp1407; _temp1445.f2= _temp1407; _temp1445;});
_LL1426:;}} _LL1410:;}} _LL1280: { struct Cyc_List_List* _temp1448; void*
_temp1450; struct _tuple4 _temp1446= Cyc_NewControlFlow_anal_unordered_Rexps(
env, inflow, _temp1312); _LL1451: _temp1450= _temp1446.f1; goto _LL1449; _LL1449:
_temp1448= _temp1446.f2; goto _LL1447; _LL1447: { void* _temp1452= _temp1450;
struct Cyc_List_List* _temp1458; struct Cyc_Dict_Dict* _temp1460; _LL1454: if(
_temp1452 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1455;} else{ goto
_LL1456;} _LL1456: if(( unsigned int) _temp1452 >  1u?*(( int*) _temp1452) == 
Cyc_CfFlowInfo_ReachableFL: 0){ _LL1461: _temp1460=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1452)->f1; goto _LL1459; _LL1459: _temp1458=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1452)->f2; goto _LL1457;} else{ goto _LL1453;} _LL1455: return({ struct
_tuple0 _temp1462; _temp1462.f1= _temp1450; _temp1462.f2= _temp1450; _temp1462;});
_LL1457: { void* _temp1463=( void*)(( struct Cyc_List_List*) _check_null(
_temp1448))->hd; void* _temp1464=( void*)(( struct Cyc_List_List*) _check_null(
_temp1448->tl))->hd; if( Cyc_CfFlowInfo_initlevel( _temp1460, _temp1463) == (
void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1465[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1312))->hd)->loc,
_tag_arr("expression may not be initialized", sizeof( unsigned char), 34u),
_tag_arr( _temp1465, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1460, _temp1464) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1466[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1312))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1466, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Exp* _temp1467=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1312))->hd; struct Cyc_Absyn_Exp* _temp1468=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1312->tl))->hd; void* _temp1469=( void*)
_temp1467->r; void* _temp1482; struct Cyc_Absyn_Vardecl* _temp1484; void*
_temp1487; struct Cyc_Absyn_Vardecl* _temp1489; void* _temp1492; struct Cyc_Absyn_Vardecl*
_temp1494; void* _temp1497; struct Cyc_Absyn_Vardecl* _temp1499; _LL1471: if(*((
int*) _temp1469) ==  Cyc_Absyn_Var_e){ _LL1483: _temp1482=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1469)->f2; if(( unsigned int) _temp1482 >  1u?*(( int*) _temp1482) ==  Cyc_Absyn_Global_b:
0){ _LL1485: _temp1484=(( struct Cyc_Absyn_Global_b_struct*) _temp1482)->f1;
goto _LL1481;} else{ goto _LL1473;}} else{ goto _LL1473;} _LL1481: if( !
_temp1484->escapes){ goto _LL1472;} else{ goto _LL1473;} _LL1473: if(*(( int*)
_temp1469) ==  Cyc_Absyn_Var_e){ _LL1488: _temp1487=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1469)->f2; if(( unsigned int) _temp1487 >  1u?*(( int*) _temp1487) ==  Cyc_Absyn_Local_b:
0){ _LL1490: _temp1489=(( struct Cyc_Absyn_Local_b_struct*) _temp1487)->f1; goto
_LL1486;} else{ goto _LL1475;}} else{ goto _LL1475;} _LL1486: if( ! _temp1489->escapes){
goto _LL1474;} else{ goto _LL1475;} _LL1475: if(*(( int*) _temp1469) ==  Cyc_Absyn_Var_e){
_LL1493: _temp1492=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1469)->f2;
if(( unsigned int) _temp1492 >  1u?*(( int*) _temp1492) ==  Cyc_Absyn_Pat_b: 0){
_LL1495: _temp1494=(( struct Cyc_Absyn_Pat_b_struct*) _temp1492)->f1; goto
_LL1491;} else{ goto _LL1477;}} else{ goto _LL1477;} _LL1491: if( ! _temp1494->escapes){
goto _LL1476;} else{ goto _LL1477;} _LL1477: if(*(( int*) _temp1469) ==  Cyc_Absyn_Var_e){
_LL1498: _temp1497=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1469)->f2;
if(( unsigned int) _temp1497 >  1u?*(( int*) _temp1497) ==  Cyc_Absyn_Param_b: 0){
_LL1500: _temp1499=(( struct Cyc_Absyn_Param_b_struct*) _temp1497)->f1; goto
_LL1496;} else{ goto _LL1479;}} else{ goto _LL1479;} _LL1496: if( ! _temp1499->escapes){
goto _LL1478;} else{ goto _LL1479;} _LL1479: goto _LL1480; _LL1472: _temp1489=
_temp1484; goto _LL1474; _LL1474: _temp1494= _temp1489; goto _LL1476; _LL1476:
_temp1499= _temp1494; goto _LL1478; _LL1478: { void* _temp1501=( void*)
_temp1468->r; void* _temp1518; struct Cyc_Absyn_Vardecl* _temp1520; void*
_temp1523; struct Cyc_Absyn_Vardecl* _temp1525; void* _temp1528; struct Cyc_Absyn_Vardecl*
_temp1530; void* _temp1533; struct Cyc_Absyn_Vardecl* _temp1535; void* _temp1537;
int _temp1539; struct Cyc_List_List* _temp1541; struct Cyc_List_List _temp1543;
struct Cyc_Absyn_Exp* _temp1544; void* _temp1546; _LL1503: if(*(( int*)
_temp1501) ==  Cyc_Absyn_Var_e){ _LL1519: _temp1518=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1501)->f2; if(( unsigned int) _temp1518 >  1u?*(( int*) _temp1518) ==  Cyc_Absyn_Global_b:
0){ _LL1521: _temp1520=(( struct Cyc_Absyn_Global_b_struct*) _temp1518)->f1;
goto _LL1517;} else{ goto _LL1505;}} else{ goto _LL1505;} _LL1517: if( !
_temp1520->escapes){ goto _LL1504;} else{ goto _LL1505;} _LL1505: if(*(( int*)
_temp1501) ==  Cyc_Absyn_Var_e){ _LL1524: _temp1523=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1501)->f2; if(( unsigned int) _temp1523 >  1u?*(( int*) _temp1523) ==  Cyc_Absyn_Local_b:
0){ _LL1526: _temp1525=(( struct Cyc_Absyn_Local_b_struct*) _temp1523)->f1; goto
_LL1522;} else{ goto _LL1507;}} else{ goto _LL1507;} _LL1522: if( ! _temp1525->escapes){
goto _LL1506;} else{ goto _LL1507;} _LL1507: if(*(( int*) _temp1501) ==  Cyc_Absyn_Var_e){
_LL1529: _temp1528=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1501)->f2;
if(( unsigned int) _temp1528 >  1u?*(( int*) _temp1528) ==  Cyc_Absyn_Pat_b: 0){
_LL1531: _temp1530=(( struct Cyc_Absyn_Pat_b_struct*) _temp1528)->f1; goto
_LL1527;} else{ goto _LL1509;}} else{ goto _LL1509;} _LL1527: if( ! _temp1530->escapes){
goto _LL1508;} else{ goto _LL1509;} _LL1509: if(*(( int*) _temp1501) ==  Cyc_Absyn_Var_e){
_LL1534: _temp1533=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1501)->f2;
if(( unsigned int) _temp1533 >  1u?*(( int*) _temp1533) ==  Cyc_Absyn_Param_b: 0){
_LL1536: _temp1535=(( struct Cyc_Absyn_Param_b_struct*) _temp1533)->f1; goto
_LL1532;} else{ goto _LL1511;}} else{ goto _LL1511;} _LL1532: if( ! _temp1535->escapes){
goto _LL1510;} else{ goto _LL1511;} _LL1511: if(*(( int*) _temp1501) ==  Cyc_Absyn_Const_e){
_LL1538: _temp1537=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1501)->f1;
if(( unsigned int) _temp1537 >  1u?*(( int*) _temp1537) ==  Cyc_Absyn_Int_c: 0){
_LL1540: _temp1539=(( struct Cyc_Absyn_Int_c_struct*) _temp1537)->f2; goto
_LL1512;} else{ goto _LL1513;}} else{ goto _LL1513;} _LL1513: if(*(( int*)
_temp1501) ==  Cyc_Absyn_Primop_e){ _LL1547: _temp1546=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1501)->f1; if( _temp1546 == ( void*) Cyc_Absyn_Size){ goto _LL1542;} else{
goto _LL1515;} _LL1542: _temp1541=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1501)->f2; if( _temp1541 ==  0){ goto _LL1515;} else{ _temp1543=* _temp1541;
_LL1545: _temp1544=( struct Cyc_Absyn_Exp*) _temp1543.hd; goto _LL1514;}} else{
goto _LL1515;} _LL1515: goto _LL1516; _LL1504: _temp1525= _temp1520; goto
_LL1506; _LL1506: _temp1530= _temp1525; goto _LL1508; _LL1508: _temp1535=
_temp1530; goto _LL1510; _LL1510:{ struct _tuple0 _temp1549=({ struct _tuple0
_temp1548; _temp1548.f1= Cyc_Tcutil_compress(( void*) _temp1499->type);
_temp1548.f2= Cyc_Tcutil_compress(( void*) _temp1535->type); _temp1548;}); void*
_temp1557; void* _temp1559; void* _temp1561; void* _temp1563; void* _temp1565;
void* _temp1567; _LL1551: _LL1558: _temp1557= _temp1549.f1; if(( unsigned int)
_temp1557 >  3u?*(( int*) _temp1557) ==  Cyc_Absyn_IntType: 0){ _LL1562:
_temp1561=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1557)->f1; if(
_temp1561 == ( void*) Cyc_Absyn_Unsigned){ goto _LL1560;} else{ goto _LL1553;}
_LL1560: _temp1559=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1557)->f2;
if( _temp1559 == ( void*) Cyc_Absyn_B4){ goto _LL1552;} else{ goto _LL1553;}}
else{ goto _LL1553;} _LL1553: _LL1564: _temp1563= _temp1549.f2; if((
unsigned int) _temp1563 >  3u?*(( int*) _temp1563) ==  Cyc_Absyn_IntType: 0){
_LL1568: _temp1567=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1563)->f1;
if( _temp1567 == ( void*) Cyc_Absyn_Unsigned){ goto _LL1566;} else{ goto _LL1555;}
_LL1566: _temp1565=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1563)->f2;
if( _temp1565 == ( void*) Cyc_Absyn_B4){ goto _LL1554;} else{ goto _LL1555;}}
else{ goto _LL1555;} _LL1555: goto _LL1556; _LL1552: goto _LL1550; _LL1554: goto
_LL1550; _LL1556: return({ struct _tuple0 _temp1569; _temp1569.f1= _temp1450;
_temp1569.f2= _temp1450; _temp1569;}); _LL1550:;}{ struct Cyc_List_List*
_temp1570=({ struct Cyc_List_List* _temp1574=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1574->hd=( void*)({ struct Cyc_CfFlowInfo_Reln*
_temp1575=( struct Cyc_CfFlowInfo_Reln*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln));
_temp1575->vd= _temp1499; _temp1575->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessVar_struct*
_temp1576=( struct Cyc_CfFlowInfo_LessVar_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_LessVar_struct));
_temp1576[ 0]=({ struct Cyc_CfFlowInfo_LessVar_struct _temp1577; _temp1577.tag=
Cyc_CfFlowInfo_LessVar; _temp1577.f1= _temp1535; _temp1577;}); _temp1576;}));
_temp1575;}); _temp1574->tl= _temp1458; _temp1574;}); return({ struct _tuple0
_temp1571; _temp1571.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp1572=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp1572[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp1573; _temp1573.tag= Cyc_CfFlowInfo_ReachableFL; _temp1573.f1= _temp1460;
_temp1573.f2= _temp1570; _temp1573;}); _temp1572;}); _temp1571.f2= _temp1450;
_temp1571;});} _LL1512:{ struct _tuple0 _temp1579=({ struct _tuple0 _temp1578;
_temp1578.f1= Cyc_Tcutil_compress(( void*) _temp1499->type); _temp1578.f2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp1468->topt))->v); _temp1578;});
void* _temp1587; void* _temp1589; void* _temp1591; void* _temp1593; void*
_temp1595; void* _temp1597; _LL1581: _LL1588: _temp1587= _temp1579.f1; if((
unsigned int) _temp1587 >  3u?*(( int*) _temp1587) ==  Cyc_Absyn_IntType: 0){
_LL1592: _temp1591=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1587)->f1;
if( _temp1591 == ( void*) Cyc_Absyn_Unsigned){ goto _LL1590;} else{ goto _LL1583;}
_LL1590: _temp1589=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1587)->f2;
if( _temp1589 == ( void*) Cyc_Absyn_B4){ goto _LL1582;} else{ goto _LL1583;}}
else{ goto _LL1583;} _LL1583: _LL1594: _temp1593= _temp1579.f2; if((
unsigned int) _temp1593 >  3u?*(( int*) _temp1593) ==  Cyc_Absyn_IntType: 0){
_LL1598: _temp1597=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1593)->f1;
if( _temp1597 == ( void*) Cyc_Absyn_Unsigned){ goto _LL1596;} else{ goto _LL1585;}
_LL1596: _temp1595=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp1593)->f2;
if( _temp1595 == ( void*) Cyc_Absyn_B4){ goto _LL1584;} else{ goto _LL1585;}}
else{ goto _LL1585;} _LL1585: goto _LL1586; _LL1582: goto _LL1580; _LL1584: goto
_LL1580; _LL1586: return({ struct _tuple0 _temp1599; _temp1599.f1= _temp1450;
_temp1599.f2= _temp1450; _temp1599;}); _LL1580:;}{ struct Cyc_List_List*
_temp1600=({ struct Cyc_List_List* _temp1604=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp1604->hd=( void*)({ struct Cyc_CfFlowInfo_Reln*
_temp1605=( struct Cyc_CfFlowInfo_Reln*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln));
_temp1605->vd= _temp1499; _temp1605->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessConst_struct*
_temp1606=( struct Cyc_CfFlowInfo_LessConst_struct*) _cycalloc_atomic( sizeof(
struct Cyc_CfFlowInfo_LessConst_struct)); _temp1606[ 0]=({ struct Cyc_CfFlowInfo_LessConst_struct
_temp1607; _temp1607.tag= Cyc_CfFlowInfo_LessConst; _temp1607.f1=( unsigned int)
_temp1539; _temp1607;}); _temp1606;})); _temp1605;}); _temp1604->tl= _temp1458;
_temp1604;}); return({ struct _tuple0 _temp1601; _temp1601.f1=( void*)({ struct
Cyc_CfFlowInfo_ReachableFL_struct* _temp1602=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp1602[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp1603; _temp1603.tag= Cyc_CfFlowInfo_ReachableFL;
_temp1603.f1= _temp1460; _temp1603.f2= _temp1600; _temp1603;}); _temp1602;});
_temp1601.f2= _temp1450; _temp1601;});} _LL1514: { void* _temp1608=( void*)
_temp1544->r; void* _temp1621; struct Cyc_Absyn_Vardecl* _temp1623; void*
_temp1626; struct Cyc_Absyn_Vardecl* _temp1628; void* _temp1631; struct Cyc_Absyn_Vardecl*
_temp1633; void* _temp1636; struct Cyc_Absyn_Vardecl* _temp1638; _LL1610: if(*((
int*) _temp1608) ==  Cyc_Absyn_Var_e){ _LL1622: _temp1621=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1608)->f2; if(( unsigned int) _temp1621 >  1u?*(( int*) _temp1621) ==  Cyc_Absyn_Global_b:
0){ _LL1624: _temp1623=(( struct Cyc_Absyn_Global_b_struct*) _temp1621)->f1;
goto _LL1620;} else{ goto _LL1612;}} else{ goto _LL1612;} _LL1620: if( !
_temp1623->escapes){ goto _LL1611;} else{ goto _LL1612;} _LL1612: if(*(( int*)
_temp1608) ==  Cyc_Absyn_Var_e){ _LL1627: _temp1626=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1608)->f2; if(( unsigned int) _temp1626 >  1u?*(( int*) _temp1626) ==  Cyc_Absyn_Local_b:
0){ _LL1629: _temp1628=(( struct Cyc_Absyn_Local_b_struct*) _temp1626)->f1; goto
_LL1625;} else{ goto _LL1614;}} else{ goto _LL1614;} _LL1625: if( ! _temp1628->escapes){
goto _LL1613;} else{ goto _LL1614;} _LL1614: if(*(( int*) _temp1608) ==  Cyc_Absyn_Var_e){
_LL1632: _temp1631=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1608)->f2;
if(( unsigned int) _temp1631 >  1u?*(( int*) _temp1631) ==  Cyc_Absyn_Pat_b: 0){
_LL1634: _temp1633=(( struct Cyc_Absyn_Pat_b_struct*) _temp1631)->f1; goto
_LL1630;} else{ goto _LL1616;}} else{ goto _LL1616;} _LL1630: if( ! _temp1633->escapes){
goto _LL1615;} else{ goto _LL1616;} _LL1616: if(*(( int*) _temp1608) ==  Cyc_Absyn_Var_e){
_LL1637: _temp1636=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1608)->f2;
if(( unsigned int) _temp1636 >  1u?*(( int*) _temp1636) ==  Cyc_Absyn_Param_b: 0){
_LL1639: _temp1638=(( struct Cyc_Absyn_Param_b_struct*) _temp1636)->f1; goto
_LL1635;} else{ goto _LL1618;}} else{ goto _LL1618;} _LL1635: if( ! _temp1638->escapes){
goto _LL1617;} else{ goto _LL1618;} _LL1618: goto _LL1619; _LL1611: _temp1628=
_temp1623; goto _LL1613; _LL1613: _temp1633= _temp1628; goto _LL1615; _LL1615:
_temp1638= _temp1633; goto _LL1617; _LL1617: { struct Cyc_List_List* _temp1640=({
struct Cyc_List_List* _temp1644=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1644->hd=( void*)({ struct Cyc_CfFlowInfo_Reln*
_temp1645=( struct Cyc_CfFlowInfo_Reln*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln));
_temp1645->vd= _temp1499; _temp1645->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessSize_struct*
_temp1646=( struct Cyc_CfFlowInfo_LessSize_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_LessSize_struct)); _temp1646[ 0]=({ struct Cyc_CfFlowInfo_LessSize_struct
_temp1647; _temp1647.tag= Cyc_CfFlowInfo_LessSize; _temp1647.f1= _temp1638;
_temp1647;}); _temp1646;})); _temp1645;}); _temp1644->tl= _temp1458; _temp1644;});
return({ struct _tuple0 _temp1641; _temp1641.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp1642=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp1642[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp1643; _temp1643.tag= Cyc_CfFlowInfo_ReachableFL; _temp1643.f1= _temp1460;
_temp1643.f2= _temp1640; _temp1643;}); _temp1642;}); _temp1641.f2= _temp1450;
_temp1641;});} _LL1619: return({ struct _tuple0 _temp1648; _temp1648.f1=
_temp1450; _temp1648.f2= _temp1450; _temp1648;}); _LL1609:;} _LL1516: return({
struct _tuple0 _temp1649; _temp1649.f1= _temp1450; _temp1649.f2= _temp1450;
_temp1649;}); _LL1502:;} _LL1480: return({ struct _tuple0 _temp1650; _temp1650.f1=
_temp1450; _temp1650.f2= _temp1450; _temp1650;}); _LL1470:;}} _LL1453:;}}
_LL1282: { struct Cyc_List_List* _temp1653; void* _temp1655; struct _tuple4
_temp1651= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1316);
_LL1656: _temp1655= _temp1651.f1; goto _LL1654; _LL1654: _temp1653= _temp1651.f2;
goto _LL1652; _LL1652: { void* _temp1657= _temp1655; struct Cyc_List_List*
_temp1663; struct Cyc_Dict_Dict* _temp1665; _LL1659: if( _temp1657 == ( void*)
Cyc_CfFlowInfo_BottomFL){ goto _LL1660;} else{ goto _LL1661;} _LL1661: if((
unsigned int) _temp1657 >  1u?*(( int*) _temp1657) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL1666: _temp1665=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1657)->f1;
goto _LL1664; _LL1664: _temp1663=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp1657)->f2; goto _LL1662;} else{ goto _LL1658;} _LL1660: return({ struct
_tuple0 _temp1667; _temp1667.f1= _temp1655; _temp1667.f2= _temp1655; _temp1667;});
_LL1662: { void* _temp1668=( void*)(( struct Cyc_List_List*) _check_null(
_temp1653))->hd; void* _temp1669=( void*)(( struct Cyc_List_List*) _check_null(
_temp1653->tl))->hd; if( Cyc_CfFlowInfo_initlevel( _temp1665, _temp1668) == (
void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1670[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1316))->hd)->loc,
_tag_arr("expression may not be initialized", sizeof( unsigned char), 34u),
_tag_arr( _temp1670, sizeof( void*), 0u));});} if( Cyc_CfFlowInfo_initlevel(
_temp1665, _temp1669) == ( void*) Cyc_CfFlowInfo_NoneIL){({ void* _temp1671[ 0u]={};
Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(((
struct Cyc_List_List*) _check_null( _temp1316))->tl))->hd)->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1671, sizeof( void*), 0u));});}{
struct Cyc_Absyn_Exp* _temp1672=( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*)
_check_null( _temp1316))->hd; struct Cyc_Absyn_Exp* _temp1673=( struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*) _check_null( _temp1316->tl))->hd; void* _temp1674=( void*)
_temp1672->r; void* _temp1687; struct Cyc_Absyn_Vardecl* _temp1689; void*
_temp1692; struct Cyc_Absyn_Vardecl* _temp1694; void* _temp1697; struct Cyc_Absyn_Vardecl*
_temp1699; void* _temp1702; struct Cyc_Absyn_Vardecl* _temp1704; _LL1676: if(*((
int*) _temp1674) ==  Cyc_Absyn_Var_e){ _LL1688: _temp1687=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1674)->f2; if(( unsigned int) _temp1687 >  1u?*(( int*) _temp1687) ==  Cyc_Absyn_Global_b:
0){ _LL1690: _temp1689=(( struct Cyc_Absyn_Global_b_struct*) _temp1687)->f1;
goto _LL1686;} else{ goto _LL1678;}} else{ goto _LL1678;} _LL1686: if( !
_temp1689->escapes){ goto _LL1677;} else{ goto _LL1678;} _LL1678: if(*(( int*)
_temp1674) ==  Cyc_Absyn_Var_e){ _LL1693: _temp1692=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1674)->f2; if(( unsigned int) _temp1692 >  1u?*(( int*) _temp1692) ==  Cyc_Absyn_Local_b:
0){ _LL1695: _temp1694=(( struct Cyc_Absyn_Local_b_struct*) _temp1692)->f1; goto
_LL1691;} else{ goto _LL1680;}} else{ goto _LL1680;} _LL1691: if( ! _temp1694->escapes){
goto _LL1679;} else{ goto _LL1680;} _LL1680: if(*(( int*) _temp1674) ==  Cyc_Absyn_Var_e){
_LL1698: _temp1697=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1674)->f2;
if(( unsigned int) _temp1697 >  1u?*(( int*) _temp1697) ==  Cyc_Absyn_Pat_b: 0){
_LL1700: _temp1699=(( struct Cyc_Absyn_Pat_b_struct*) _temp1697)->f1; goto
_LL1696;} else{ goto _LL1682;}} else{ goto _LL1682;} _LL1696: if( ! _temp1699->escapes){
goto _LL1681;} else{ goto _LL1682;} _LL1682: if(*(( int*) _temp1674) ==  Cyc_Absyn_Var_e){
_LL1703: _temp1702=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1674)->f2;
if(( unsigned int) _temp1702 >  1u?*(( int*) _temp1702) ==  Cyc_Absyn_Param_b: 0){
_LL1705: _temp1704=(( struct Cyc_Absyn_Param_b_struct*) _temp1702)->f1; goto
_LL1701;} else{ goto _LL1684;}} else{ goto _LL1684;} _LL1701: if( ! _temp1704->escapes){
goto _LL1683;} else{ goto _LL1684;} _LL1684: goto _LL1685; _LL1677: _temp1694=
_temp1689; goto _LL1679; _LL1679: _temp1699= _temp1694; goto _LL1681; _LL1681:
_temp1704= _temp1699; goto _LL1683; _LL1683: { void* _temp1706=( void*)
_temp1673->r; struct Cyc_List_List* _temp1712; struct Cyc_List_List _temp1714;
struct Cyc_Absyn_Exp* _temp1715; void* _temp1717; _LL1708: if(*(( int*)
_temp1706) ==  Cyc_Absyn_Primop_e){ _LL1718: _temp1717=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1706)->f1; if( _temp1717 == ( void*) Cyc_Absyn_Size){ goto _LL1713;} else{
goto _LL1710;} _LL1713: _temp1712=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1706)->f2; if( _temp1712 ==  0){ goto _LL1710;} else{ _temp1714=* _temp1712;
_LL1716: _temp1715=( struct Cyc_Absyn_Exp*) _temp1714.hd; goto _LL1709;}} else{
goto _LL1710;} _LL1710: goto _LL1711; _LL1709: { void* _temp1719=( void*)
_temp1715->r; void* _temp1732; struct Cyc_Absyn_Vardecl* _temp1734; void*
_temp1737; struct Cyc_Absyn_Vardecl* _temp1739; void* _temp1742; struct Cyc_Absyn_Vardecl*
_temp1744; void* _temp1747; struct Cyc_Absyn_Vardecl* _temp1749; _LL1721: if(*((
int*) _temp1719) ==  Cyc_Absyn_Var_e){ _LL1733: _temp1732=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1719)->f2; if(( unsigned int) _temp1732 >  1u?*(( int*) _temp1732) ==  Cyc_Absyn_Global_b:
0){ _LL1735: _temp1734=(( struct Cyc_Absyn_Global_b_struct*) _temp1732)->f1;
goto _LL1731;} else{ goto _LL1723;}} else{ goto _LL1723;} _LL1731: if( !
_temp1734->escapes){ goto _LL1722;} else{ goto _LL1723;} _LL1723: if(*(( int*)
_temp1719) ==  Cyc_Absyn_Var_e){ _LL1738: _temp1737=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1719)->f2; if(( unsigned int) _temp1737 >  1u?*(( int*) _temp1737) ==  Cyc_Absyn_Local_b:
0){ _LL1740: _temp1739=(( struct Cyc_Absyn_Local_b_struct*) _temp1737)->f1; goto
_LL1736;} else{ goto _LL1725;}} else{ goto _LL1725;} _LL1736: if( ! _temp1739->escapes){
goto _LL1724;} else{ goto _LL1725;} _LL1725: if(*(( int*) _temp1719) ==  Cyc_Absyn_Var_e){
_LL1743: _temp1742=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1719)->f2;
if(( unsigned int) _temp1742 >  1u?*(( int*) _temp1742) ==  Cyc_Absyn_Pat_b: 0){
_LL1745: _temp1744=(( struct Cyc_Absyn_Pat_b_struct*) _temp1742)->f1; goto
_LL1741;} else{ goto _LL1727;}} else{ goto _LL1727;} _LL1741: if( ! _temp1744->escapes){
goto _LL1726;} else{ goto _LL1727;} _LL1727: if(*(( int*) _temp1719) ==  Cyc_Absyn_Var_e){
_LL1748: _temp1747=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp1719)->f2;
if(( unsigned int) _temp1747 >  1u?*(( int*) _temp1747) ==  Cyc_Absyn_Param_b: 0){
_LL1750: _temp1749=(( struct Cyc_Absyn_Param_b_struct*) _temp1747)->f1; goto
_LL1746;} else{ goto _LL1729;}} else{ goto _LL1729;} _LL1746: if( ! _temp1749->escapes){
goto _LL1728;} else{ goto _LL1729;} _LL1729: goto _LL1730; _LL1722: _temp1739=
_temp1734; goto _LL1724; _LL1724: _temp1744= _temp1739; goto _LL1726; _LL1726:
_temp1749= _temp1744; goto _LL1728; _LL1728: { struct Cyc_List_List* _temp1751=({
struct Cyc_List_List* _temp1755=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1755->hd=( void*)({ struct Cyc_CfFlowInfo_Reln*
_temp1756=( struct Cyc_CfFlowInfo_Reln*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln));
_temp1756->vd= _temp1704; _temp1756->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessEqSize_struct*
_temp1757=( struct Cyc_CfFlowInfo_LessEqSize_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_LessEqSize_struct)); _temp1757[ 0]=({ struct Cyc_CfFlowInfo_LessEqSize_struct
_temp1758; _temp1758.tag= Cyc_CfFlowInfo_LessEqSize; _temp1758.f1= _temp1749;
_temp1758;}); _temp1757;})); _temp1756;}); _temp1755->tl= _temp1663; _temp1755;});
return({ struct _tuple0 _temp1752; _temp1752.f1=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp1753=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp1753[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp1754; _temp1754.tag= Cyc_CfFlowInfo_ReachableFL; _temp1754.f1= _temp1665;
_temp1754.f2= _temp1751; _temp1754;}); _temp1753;}); _temp1752.f2= _temp1655;
_temp1752;});} _LL1730: return({ struct _tuple0 _temp1759; _temp1759.f1=
_temp1655; _temp1759.f2= _temp1655; _temp1759;}); _LL1720:;} _LL1711: return({
struct _tuple0 _temp1760; _temp1760.f1= _temp1655; _temp1760.f2= _temp1655;
_temp1760;}); _LL1707:;} _LL1685: return({ struct _tuple0 _temp1761; _temp1761.f1=
_temp1655; _temp1761.f2= _temp1655; _temp1761;}); _LL1675:;}} _LL1658:;}}
_LL1284: goto _LL1268; _LL1268:;}{ void* _temp1764; void* _temp1766; struct
_tuple0 _temp1762= Cyc_NewControlFlow_anal_Rexp( env, inflow, e); _LL1767:
_temp1766= _temp1762.f1; goto _LL1765; _LL1765: _temp1764= _temp1762.f2; goto
_LL1763; _LL1763: { void* _temp1768= _temp1766; struct Cyc_List_List* _temp1774;
struct Cyc_Dict_Dict* _temp1776; _LL1770: if( _temp1768 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL1771;} else{ goto _LL1772;} _LL1772: if(( unsigned int) _temp1768 >  1u?*((
int*) _temp1768) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL1777: _temp1776=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1768)->f1; goto _LL1775; _LL1775:
_temp1774=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp1768)->f2; goto
_LL1773;} else{ goto _LL1769;} _LL1771: return({ struct _tuple0 _temp1778;
_temp1778.f1= _temp1766; _temp1778.f2= _temp1766; _temp1778;}); _LL1773: { void*
_temp1779= _temp1764; void* _temp1799; void* _temp1801; void* _temp1803; _LL1781:
if( _temp1779 == ( void*) Cyc_CfFlowInfo_Zero){ goto _LL1782;} else{ goto
_LL1783;} _LL1783: if( _temp1779 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto
_LL1784;} else{ goto _LL1785;} _LL1785: if( _temp1779 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL1786;} else{ goto _LL1787;} _LL1787: if(( unsigned int) _temp1779 >  3u?*((
int*) _temp1779) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL1788;} else{ goto
_LL1789;} _LL1789: if(( unsigned int) _temp1779 >  3u?*(( int*) _temp1779) == 
Cyc_CfFlowInfo_UnknownR: 0){ _LL1800: _temp1799=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1779)->f1; if( _temp1799 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL1790;}
else{ goto _LL1791;}} else{ goto _LL1791;} _LL1791: if(( unsigned int) _temp1779
>  3u?*(( int*) _temp1779) ==  Cyc_CfFlowInfo_Esc: 0){ _LL1802: _temp1801=( void*)((
struct Cyc_CfFlowInfo_Esc_struct*) _temp1779)->f1; if( _temp1801 == ( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL1792;} else{ goto _LL1793;}} else{ goto _LL1793;} _LL1793: if((
unsigned int) _temp1779 >  3u?*(( int*) _temp1779) ==  Cyc_CfFlowInfo_UnknownR:
0){ _LL1804: _temp1803=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*)
_temp1779)->f1; goto _LL1794;} else{ goto _LL1795;} _LL1795: if(( unsigned int)
_temp1779 >  3u?*(( int*) _temp1779) ==  Cyc_CfFlowInfo_Esc: 0){ goto _LL1796;}
else{ goto _LL1797;} _LL1797: if(( unsigned int) _temp1779 >  3u?*(( int*)
_temp1779) ==  Cyc_CfFlowInfo_Aggregate: 0){ goto _LL1798;} else{ goto _LL1780;}
_LL1782: return({ struct _tuple0 _temp1805; _temp1805.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1805.f2= _temp1766; _temp1805;}); _LL1784: goto _LL1786; _LL1786: goto
_LL1788; _LL1788: return({ struct _tuple0 _temp1806; _temp1806.f1= _temp1766;
_temp1806.f2=( void*) Cyc_CfFlowInfo_BottomFL; _temp1806;}); _LL1790: goto
_LL1792; _LL1792:({ void* _temp1807[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("expression may not be initialized",
sizeof( unsigned char), 34u), _tag_arr( _temp1807, sizeof( void*), 0u));});
return({ struct _tuple0 _temp1808; _temp1808.f1=( void*) Cyc_CfFlowInfo_BottomFL;
_temp1808.f2=( void*) Cyc_CfFlowInfo_BottomFL; _temp1808;}); _LL1794: return Cyc_NewControlFlow_splitzero(
env, inflow, _temp1766, e, _temp1803); _LL1796: return({ struct _tuple0
_temp1809; _temp1809.f1= _temp1766; _temp1809.f2= _temp1766; _temp1809;});
_LL1798:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp1810=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1810[ 0]=({ struct Cyc_Core_Impossible_struct _temp1811; _temp1811.tag= Cyc_Core_Impossible;
_temp1811.f1= _tag_arr("anal_test", sizeof( unsigned char), 10u); _temp1811;});
_temp1810;})); _LL1780:;} _LL1769:;}}} static void* Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv* env, void* inflow, struct Cyc_List_List* scs){
for( 0; scs !=  0; scs= scs->tl){ struct Cyc_Absyn_Switch_clause _temp1814;
struct Cyc_Absyn_Stmt* _temp1815; struct Cyc_Absyn_Exp* _temp1817; struct Cyc_Core_Opt*
_temp1819; struct Cyc_Absyn_Switch_clause* _temp1812=( struct Cyc_Absyn_Switch_clause*)
scs->hd; _temp1814=* _temp1812; _LL1820: _temp1819= _temp1814.pat_vars; goto
_LL1818; _LL1818: _temp1817= _temp1814.where_clause; goto _LL1816; _LL1816:
_temp1815= _temp1814.body; goto _LL1813; _LL1813: { void* clause_inflow= Cyc_NewControlFlow_add_vars(
inflow,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( _temp1819))->v,
Cyc_CfFlowInfo_unknown_all); void* clause_outflow; if( _temp1817 !=  0){ void*
_temp1823; void* _temp1825; struct _tuple0 _temp1821= Cyc_NewControlFlow_anal_test(
env, clause_inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp1817)); _LL1826:
_temp1825= _temp1821.f1; goto _LL1824; _LL1824: _temp1823= _temp1821.f2; goto
_LL1822; _LL1822: inflow= _temp1823; clause_outflow= Cyc_NewControlFlow_anal_stmt(
env, _temp1825, _temp1815);} else{ clause_outflow= Cyc_NewControlFlow_anal_stmt(
env, clause_inflow, _temp1815);}{ void* _temp1827= clause_outflow; _LL1829: if(
_temp1827 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL1830;} else{ goto
_LL1831;} _LL1831: goto _LL1832; _LL1830: goto _LL1828; _LL1832: if( scs->tl == 
0){ return clause_outflow;} else{ if(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null((( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*)
_check_null( scs->tl))->hd)->pat_vars))->v !=  0){({ void* _temp1833[ 0u]={};
Cyc_Tcutil_terr( _temp1815->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp1833, sizeof( void*), 0u));});}
else{({ void* _temp1834[ 0u]={}; Cyc_Tcutil_warn( _temp1815->loc, _tag_arr("switch clause may implicitly fallthru",
sizeof( unsigned char), 38u), _tag_arr( _temp1834, sizeof( void*), 0u));});} Cyc_NewControlFlow_update_flow(
env,(( struct Cyc_Absyn_Switch_clause*)(( struct Cyc_List_List*) _check_null(
scs->tl))->hd)->body, clause_outflow);} goto _LL1828; _LL1828:;}}} return( void*)
Cyc_CfFlowInfo_BottomFL;} static void* Cyc_NewControlFlow_anal_stmt( struct Cyc_NewControlFlow_AnalEnv*
env, void* inflow, struct Cyc_Absyn_Stmt* s){ struct Cyc_NewControlFlow_CFStmtAnnot*
_temp1835= Cyc_NewControlFlow_pre_stmt_check( env, inflow, s); inflow=( void*)
_temp1835->flow;{ void* _temp1836=( void*) s->r; struct Cyc_Absyn_Exp* _temp1878;
struct Cyc_Absyn_Exp* _temp1880; struct Cyc_Absyn_Exp* _temp1882; struct Cyc_Absyn_Stmt*
_temp1884; struct Cyc_Absyn_Stmt* _temp1886; struct Cyc_Absyn_Stmt* _temp1888;
struct Cyc_Absyn_Stmt* _temp1890; struct Cyc_Absyn_Exp* _temp1892; struct Cyc_Absyn_Stmt*
_temp1894; struct _tuple3 _temp1896; struct Cyc_Absyn_Stmt* _temp1898; struct
Cyc_Absyn_Exp* _temp1900; struct _tuple3 _temp1902; struct Cyc_Absyn_Stmt*
_temp1904; struct Cyc_Absyn_Exp* _temp1906; struct Cyc_Absyn_Stmt* _temp1908;
struct Cyc_Absyn_Stmt* _temp1910; struct _tuple3 _temp1912; struct Cyc_Absyn_Stmt*
_temp1914; struct Cyc_Absyn_Exp* _temp1916; struct _tuple3 _temp1918; struct Cyc_Absyn_Stmt*
_temp1920; struct Cyc_Absyn_Exp* _temp1922; struct Cyc_Absyn_Exp* _temp1924;
struct Cyc_Absyn_Stmt* _temp1926; struct Cyc_Absyn_Switch_clause** _temp1928;
struct Cyc_Absyn_Switch_clause* _temp1930; struct Cyc_List_List* _temp1931;
struct Cyc_Absyn_Stmt* _temp1933; struct Cyc_Absyn_Stmt* _temp1935; struct Cyc_Absyn_Stmt*
_temp1937; struct Cyc_List_List* _temp1939; struct Cyc_Absyn_Exp* _temp1941;
struct Cyc_List_List* _temp1943; struct Cyc_Absyn_Stmt* _temp1945; struct Cyc_Absyn_Stmt*
_temp1947; struct Cyc_Absyn_Decl* _temp1949; struct Cyc_Absyn_Stmt* _temp1951;
struct Cyc_Absyn_Stmt* _temp1953; struct Cyc_Absyn_Vardecl* _temp1955; struct
Cyc_Absyn_Tvar* _temp1957; _LL1838: if( _temp1836 == ( void*) Cyc_Absyn_Skip_s){
goto _LL1839;} else{ goto _LL1840;} _LL1840: if(( unsigned int) _temp1836 >  1u?*((
int*) _temp1836) ==  Cyc_Absyn_Return_s: 0){ _LL1879: _temp1878=(( struct Cyc_Absyn_Return_s_struct*)
_temp1836)->f1; if( _temp1878 ==  0){ goto _LL1841;} else{ goto _LL1842;}} else{
goto _LL1842;} _LL1842: if(( unsigned int) _temp1836 >  1u?*(( int*) _temp1836)
==  Cyc_Absyn_Return_s: 0){ _LL1881: _temp1880=(( struct Cyc_Absyn_Return_s_struct*)
_temp1836)->f1; goto _LL1843;} else{ goto _LL1844;} _LL1844: if(( unsigned int)
_temp1836 >  1u?*(( int*) _temp1836) ==  Cyc_Absyn_Exp_s: 0){ _LL1883: _temp1882=((
struct Cyc_Absyn_Exp_s_struct*) _temp1836)->f1; goto _LL1845;} else{ goto
_LL1846;} _LL1846: if(( unsigned int) _temp1836 >  1u?*(( int*) _temp1836) == 
Cyc_Absyn_Seq_s: 0){ _LL1887: _temp1886=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1836)->f1; goto _LL1885; _LL1885: _temp1884=(( struct Cyc_Absyn_Seq_s_struct*)
_temp1836)->f2; goto _LL1847;} else{ goto _LL1848;} _LL1848: if(( unsigned int)
_temp1836 >  1u?*(( int*) _temp1836) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL1893:
_temp1892=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1836)->f1; goto _LL1891;
_LL1891: _temp1890=(( struct Cyc_Absyn_IfThenElse_s_struct*) _temp1836)->f2;
goto _LL1889; _LL1889: _temp1888=(( struct Cyc_Absyn_IfThenElse_s_struct*)
_temp1836)->f3; goto _LL1849;} else{ goto _LL1850;} _LL1850: if(( unsigned int)
_temp1836 >  1u?*(( int*) _temp1836) ==  Cyc_Absyn_While_s: 0){ _LL1897:
_temp1896=(( struct Cyc_Absyn_While_s_struct*) _temp1836)->f1; _LL1901:
_temp1900= _temp1896.f1; goto _LL1899; _LL1899: _temp1898= _temp1896.f2; goto
_LL1895; _LL1895: _temp1894=(( struct Cyc_Absyn_While_s_struct*) _temp1836)->f2;
goto _LL1851;} else{ goto _LL1852;} _LL1852: if(( unsigned int) _temp1836 >  1u?*((
int*) _temp1836) ==  Cyc_Absyn_Do_s: 0){ _LL1909: _temp1908=(( struct Cyc_Absyn_Do_s_struct*)
_temp1836)->f1; goto _LL1903; _LL1903: _temp1902=(( struct Cyc_Absyn_Do_s_struct*)
_temp1836)->f2; _LL1907: _temp1906= _temp1902.f1; goto _LL1905; _LL1905:
_temp1904= _temp1902.f2; goto _LL1853;} else{ goto _LL1854;} _LL1854: if((
unsigned int) _temp1836 >  1u?*(( int*) _temp1836) ==  Cyc_Absyn_For_s: 0){
_LL1925: _temp1924=(( struct Cyc_Absyn_For_s_struct*) _temp1836)->f1; goto
_LL1919; _LL1919: _temp1918=(( struct Cyc_Absyn_For_s_struct*) _temp1836)->f2;
_LL1923: _temp1922= _temp1918.f1; goto _LL1921; _LL1921: _temp1920= _temp1918.f2;
goto _LL1913; _LL1913: _temp1912=(( struct Cyc_Absyn_For_s_struct*) _temp1836)->f3;
_LL1917: _temp1916= _temp1912.f1; goto _LL1915; _LL1915: _temp1914= _temp1912.f2;
goto _LL1911; _LL1911: _temp1910=(( struct Cyc_Absyn_For_s_struct*) _temp1836)->f4;
goto _LL1855;} else{ goto _LL1856;} _LL1856: if(( unsigned int) _temp1836 >  1u?*((
int*) _temp1836) ==  Cyc_Absyn_Break_s: 0){ _LL1927: _temp1926=(( struct Cyc_Absyn_Break_s_struct*)
_temp1836)->f1; if( _temp1926 ==  0){ goto _LL1857;} else{ goto _LL1858;}} else{
goto _LL1858;} _LL1858: if(( unsigned int) _temp1836 >  1u?*(( int*) _temp1836)
==  Cyc_Absyn_Fallthru_s: 0){ _LL1932: _temp1931=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1836)->f1; goto _LL1929; _LL1929: _temp1928=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp1836)->f2; if( _temp1928 ==  0){ goto _LL1860;} else{ _temp1930=* _temp1928;
goto _LL1859;}} else{ goto _LL1860;} _LL1860: if(( unsigned int) _temp1836 >  1u?*((
int*) _temp1836) ==  Cyc_Absyn_Break_s: 0){ _LL1934: _temp1933=(( struct Cyc_Absyn_Break_s_struct*)
_temp1836)->f1; goto _LL1861;} else{ goto _LL1862;} _LL1862: if(( unsigned int)
_temp1836 >  1u?*(( int*) _temp1836) ==  Cyc_Absyn_Continue_s: 0){ _LL1936:
_temp1935=(( struct Cyc_Absyn_Continue_s_struct*) _temp1836)->f1; goto _LL1863;}
else{ goto _LL1864;} _LL1864: if(( unsigned int) _temp1836 >  1u?*(( int*)
_temp1836) ==  Cyc_Absyn_Goto_s: 0){ _LL1938: _temp1937=(( struct Cyc_Absyn_Goto_s_struct*)
_temp1836)->f2; goto _LL1865;} else{ goto _LL1866;} _LL1866: if(( unsigned int)
_temp1836 >  1u?*(( int*) _temp1836) ==  Cyc_Absyn_Switch_s: 0){ _LL1942:
_temp1941=(( struct Cyc_Absyn_Switch_s_struct*) _temp1836)->f1; goto _LL1940;
_LL1940: _temp1939=(( struct Cyc_Absyn_Switch_s_struct*) _temp1836)->f2; goto
_LL1867;} else{ goto _LL1868;} _LL1868: if(( unsigned int) _temp1836 >  1u?*((
int*) _temp1836) ==  Cyc_Absyn_TryCatch_s: 0){ _LL1946: _temp1945=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1836)->f1; goto _LL1944; _LL1944: _temp1943=(( struct Cyc_Absyn_TryCatch_s_struct*)
_temp1836)->f2; goto _LL1869;} else{ goto _LL1870;} _LL1870: if(( unsigned int)
_temp1836 >  1u?*(( int*) _temp1836) ==  Cyc_Absyn_Decl_s: 0){ _LL1950:
_temp1949=(( struct Cyc_Absyn_Decl_s_struct*) _temp1836)->f1; goto _LL1948;
_LL1948: _temp1947=(( struct Cyc_Absyn_Decl_s_struct*) _temp1836)->f2; goto
_LL1871;} else{ goto _LL1872;} _LL1872: if(( unsigned int) _temp1836 >  1u?*((
int*) _temp1836) ==  Cyc_Absyn_Label_s: 0){ _LL1952: _temp1951=(( struct Cyc_Absyn_Label_s_struct*)
_temp1836)->f2; goto _LL1873;} else{ goto _LL1874;} _LL1874: if(( unsigned int)
_temp1836 >  1u?*(( int*) _temp1836) ==  Cyc_Absyn_Region_s: 0){ _LL1958:
_temp1957=(( struct Cyc_Absyn_Region_s_struct*) _temp1836)->f1; goto _LL1956;
_LL1956: _temp1955=(( struct Cyc_Absyn_Region_s_struct*) _temp1836)->f2; goto
_LL1954; _LL1954: _temp1953=(( struct Cyc_Absyn_Region_s_struct*) _temp1836)->f3;
goto _LL1875;} else{ goto _LL1876;} _LL1876: goto _LL1877; _LL1839: return
inflow; _LL1841: if( env->noreturn){({ void* _temp1959[ 0u]={}; Cyc_Tcutil_terr(
s->loc, _tag_arr("`noreturn' function might return", sizeof( unsigned char), 33u),
_tag_arr( _temp1959, sizeof( void*), 0u));});} return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1843: if( env->noreturn){({ void* _temp1960[ 0u]={}; Cyc_Tcutil_terr( s->loc,
_tag_arr("`noreturn' function might return", sizeof( unsigned char), 33u),
_tag_arr( _temp1960, sizeof( void*), 0u));});}{ void* _temp1963; void* _temp1965;
struct _tuple0 _temp1961= Cyc_NewControlFlow_anal_Rexp( env, inflow,( struct Cyc_Absyn_Exp*)
_check_null( _temp1880)); _LL1966: _temp1965= _temp1961.f1; goto _LL1964;
_LL1964: _temp1963= _temp1961.f2; goto _LL1962; _LL1962: Cyc_NewControlFlow_use_Rval(
env,(( struct Cyc_Absyn_Exp*) _check_null( _temp1880))->loc, _temp1965,
_temp1963); return( void*) Cyc_CfFlowInfo_BottomFL;} _LL1845: return( Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1882)).f1; _LL1847: return Cyc_NewControlFlow_anal_stmt( env,
Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1886), _temp1884); _LL1849: {
void* _temp1969; void* _temp1971; struct _tuple0 _temp1967= Cyc_NewControlFlow_anal_test(
env, inflow, _temp1892); _LL1972: _temp1971= _temp1967.f1; goto _LL1970; _LL1970:
_temp1969= _temp1967.f2; goto _LL1968; _LL1968: return Cyc_CfFlowInfo_join_flow(
env->all_changed, Cyc_NewControlFlow_anal_stmt( env, _temp1971, _temp1890), Cyc_NewControlFlow_anal_stmt(
env, _temp1969, _temp1888));} _LL1851: { void* _temp1973=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, inflow, _temp1898))->flow; void* _temp1976; void* _temp1978; struct _tuple0
_temp1974= Cyc_NewControlFlow_anal_test( env, _temp1973, _temp1900); _LL1979:
_temp1978= _temp1974.f1; goto _LL1977; _LL1977: _temp1976= _temp1974.f2; goto
_LL1975; _LL1975: { void* _temp1980= Cyc_NewControlFlow_anal_stmt( env,
_temp1978, _temp1894); Cyc_NewControlFlow_update_flow( env, _temp1898, _temp1980);
return _temp1976;}} _LL1853: { void* _temp1981= Cyc_NewControlFlow_anal_stmt(
env, inflow, _temp1908); void* _temp1982=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1981, _temp1904))->flow; void* _temp1985; void* _temp1987; struct
_tuple0 _temp1983= Cyc_NewControlFlow_anal_test( env, _temp1982, _temp1906);
_LL1988: _temp1987= _temp1983.f1; goto _LL1986; _LL1986: _temp1985= _temp1983.f2;
goto _LL1984; _LL1984: Cyc_NewControlFlow_update_flow( env, _temp1908, _temp1987);
return _temp1985;} _LL1855: { void* _temp1989=( Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1924)).f1; void* _temp1990=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1989, _temp1920))->flow; void* _temp1993; void* _temp1995; struct
_tuple0 _temp1991= Cyc_NewControlFlow_anal_test( env, _temp1990, _temp1922);
_LL1996: _temp1995= _temp1991.f1; goto _LL1994; _LL1994: _temp1993= _temp1991.f2;
goto _LL1992; _LL1992: { void* _temp1997= Cyc_NewControlFlow_anal_stmt( env,
_temp1995, _temp1910); void* _temp1998=( void*)( Cyc_NewControlFlow_pre_stmt_check(
env, _temp1997, _temp1914))->flow; void* _temp1999=( Cyc_NewControlFlow_anal_Rexp(
env, _temp1998, _temp1916)).f1; Cyc_NewControlFlow_update_flow( env, _temp1920,
_temp1999); return _temp1993;}} _LL1857: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1859: { struct Cyc_List_List* _temp2002; void* _temp2004; struct _tuple4
_temp2000= Cyc_NewControlFlow_anal_unordered_Rexps( env, inflow, _temp1931);
_LL2005: _temp2004= _temp2000.f1; goto _LL2003; _LL2003: _temp2002= _temp2000.f2;
goto _LL2001; _LL2001: for( 0; _temp2002 !=  0;( _temp2002= _temp2002->tl,
_temp1931= _temp1931->tl)){ _temp2004= Cyc_NewControlFlow_use_Rval( env,((
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( _temp1931))->hd)->loc,
_temp2004,( void*) _temp2002->hd);} _temp2004= Cyc_NewControlFlow_add_vars(
_temp2004,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp1930->pat_vars))->v, Cyc_CfFlowInfo_unknown_all); Cyc_NewControlFlow_update_flow(
env,( struct Cyc_Absyn_Stmt*) _temp1930->body, _temp2004); return( void*) Cyc_CfFlowInfo_BottomFL;}
_LL1861: _temp1935= _temp1933; goto _LL1863; _LL1863: _temp1937= _temp1935; goto
_LL1865; _LL1865: if( env->iteration_num ==  1){ struct Cyc_Absyn_Stmt*
_temp2006= _temp1835->encloser; struct Cyc_Absyn_Stmt* _temp2007=( Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*) _check_null( _temp1937)))->encloser; while( _temp2007 != 
_temp2006) { struct Cyc_Absyn_Stmt* _temp2008=( Cyc_NewControlFlow_get_stmt_annot(
_temp2006))->encloser; if( _temp2008 ==  _temp2006){({ void* _temp2009[ 0u]={};
Cyc_Tcutil_terr( s->loc, _tag_arr("goto enters local scope or exception handler",
sizeof( unsigned char), 45u), _tag_arr( _temp2009, sizeof( void*), 0u));});
break;} _temp2006= _temp2008;}} Cyc_NewControlFlow_update_flow( env,( struct Cyc_Absyn_Stmt*)
_check_null( _temp1937), inflow); return( void*) Cyc_CfFlowInfo_BottomFL;
_LL1867: { void* _temp2012; void* _temp2014; struct _tuple0 _temp2010= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp1941); _LL2015: _temp2014= _temp2010.f1; goto _LL2013; _LL2013:
_temp2012= _temp2010.f2; goto _LL2011; _LL2011: _temp2014= Cyc_NewControlFlow_use_Rval(
env, _temp1941->loc, _temp2014, _temp2012); return Cyc_NewControlFlow_anal_scs(
env, _temp2014, _temp1939);} _LL1869: { int old_in_try= env->in_try; void*
old_tryflow=( void*) env->tryflow; env->in_try= 1;( void*)( env->tryflow=( void*)
inflow);{ void* s1_outflow= Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1945);
void* scs_inflow=( void*) env->tryflow; env->in_try= old_in_try;( void*)( env->tryflow=(
void*) old_tryflow); Cyc_NewControlFlow_update_tryflow( env, scs_inflow);{ void*
scs_outflow= Cyc_NewControlFlow_anal_scs( env, scs_inflow, _temp1943);{ void*
_temp2016= scs_outflow; _LL2018: if( _temp2016 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL2019;} else{ goto _LL2020;} _LL2020: goto _LL2021; _LL2019: goto _LL2017;
_LL2021:({ void* _temp2022[ 0u]={}; Cyc_Tcutil_terr( s->loc, _tag_arr("last catch clause may implicitly fallthru",
sizeof( unsigned char), 42u), _tag_arr( _temp2022, sizeof( void*), 0u));});
_LL2017:;} return s1_outflow;}}} _LL1871: return Cyc_NewControlFlow_anal_stmt(
env, Cyc_NewControlFlow_anal_decl( env, inflow, _temp1949), _temp1947); _LL1873:
return Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1951); _LL1875: { struct
Cyc_List_List _temp2023=({ struct Cyc_List_List _temp2024; _temp2024.hd=( void*)
_temp1955; _temp2024.tl= 0; _temp2024;}); inflow= Cyc_NewControlFlow_add_vars(
inflow,( struct Cyc_List_List*)& _temp2023, Cyc_CfFlowInfo_unknown_all); return
Cyc_NewControlFlow_anal_stmt( env, inflow, _temp1953);} _LL1877:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp2025=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp2025[ 0]=({ struct
Cyc_Core_Impossible_struct _temp2026; _temp2026.tag= Cyc_Core_Impossible;
_temp2026.f1= _tag_arr("anal_stmt -- bad stmt syntax or unimplemented stmt form",
sizeof( unsigned char), 56u); _temp2026;}); _temp2025;})); _LL1837:;}} static
void* Cyc_NewControlFlow_anal_decl( struct Cyc_NewControlFlow_AnalEnv* env, void*
inflow, struct Cyc_Absyn_Decl* decl){ void* _temp2027=( void*) decl->r; struct
Cyc_Absyn_Vardecl* _temp2037; struct Cyc_Absyn_Exp* _temp2039; struct Cyc_Core_Opt*
_temp2041; struct Cyc_Core_Opt _temp2043; struct Cyc_List_List* _temp2044;
struct Cyc_List_List* _temp2046; _LL2029: if(*(( int*) _temp2027) ==  Cyc_Absyn_Var_d){
_LL2038: _temp2037=(( struct Cyc_Absyn_Var_d_struct*) _temp2027)->f1; goto
_LL2030;} else{ goto _LL2031;} _LL2031: if(*(( int*) _temp2027) ==  Cyc_Absyn_Let_d){
_LL2042: _temp2041=(( struct Cyc_Absyn_Let_d_struct*) _temp2027)->f2; if(
_temp2041 ==  0){ goto _LL2033;} else{ _temp2043=* _temp2041; _LL2045: _temp2044=(
struct Cyc_List_List*) _temp2043.v; goto _LL2040;} _LL2040: _temp2039=(( struct
Cyc_Absyn_Let_d_struct*) _temp2027)->f4; goto _LL2032;} else{ goto _LL2033;}
_LL2033: if(*(( int*) _temp2027) ==  Cyc_Absyn_Letv_d){ _LL2047: _temp2046=((
struct Cyc_Absyn_Letv_d_struct*) _temp2027)->f1; goto _LL2034;} else{ goto
_LL2035;} _LL2035: goto _LL2036; _LL2030: { struct Cyc_List_List _temp2048=({
struct Cyc_List_List _temp2072; _temp2072.hd=( void*) _temp2037; _temp2072.tl= 0;
_temp2072;}); inflow= Cyc_NewControlFlow_add_vars( inflow,( struct Cyc_List_List*)&
_temp2048, Cyc_CfFlowInfo_unknown_none);{ struct Cyc_Absyn_Exp* _temp2049=
_temp2037->initializer; if( _temp2049 ==  0){ return inflow;}{ void* _temp2052;
void* _temp2054; struct _tuple0 _temp2050= Cyc_NewControlFlow_anal_Rexp( env,
inflow,( struct Cyc_Absyn_Exp*) _check_null( _temp2049)); _LL2055: _temp2054=
_temp2050.f1; goto _LL2053; _LL2053: _temp2052= _temp2050.f2; goto _LL2051;
_LL2051: { void* _temp2056= _temp2054; struct Cyc_List_List* _temp2062; struct
Cyc_Dict_Dict* _temp2064; _LL2058: if( _temp2056 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL2059;} else{ goto _LL2060;} _LL2060: if(( unsigned int) _temp2056 >  1u?*((
int*) _temp2056) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL2065: _temp2064=((
struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp2056)->f1; goto _LL2063; _LL2063:
_temp2062=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp2056)->f2; goto
_LL2061;} else{ goto _LL2057;} _LL2059: return( void*) Cyc_CfFlowInfo_BottomFL;
_LL2061: _temp2064= Cyc_CfFlowInfo_assign_place( decl->loc, _temp2064, env->all_changed,({
struct Cyc_CfFlowInfo_Place* _temp2066=( struct Cyc_CfFlowInfo_Place*) _cycalloc(
sizeof( struct Cyc_CfFlowInfo_Place)); _temp2066->root=( void*)(( void*)({
struct Cyc_CfFlowInfo_VarRoot_struct* _temp2067=( struct Cyc_CfFlowInfo_VarRoot_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_VarRoot_struct)); _temp2067[ 0]=({
struct Cyc_CfFlowInfo_VarRoot_struct _temp2068; _temp2068.tag= Cyc_CfFlowInfo_VarRoot;
_temp2068.f1= _temp2037; _temp2068;}); _temp2067;})); _temp2066->fields= 0;
_temp2066;}), _temp2052); _temp2062= Cyc_CfFlowInfo_reln_assign_var( _temp2062,
_temp2037,( struct Cyc_Absyn_Exp*) _check_null( _temp2037->initializer));{
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp2069=({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp2070=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp2070[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp2071; _temp2071.tag= Cyc_CfFlowInfo_ReachableFL; _temp2071.f1= _temp2064;
_temp2071.f2= _temp2062; _temp2071;}); _temp2070;}); Cyc_NewControlFlow_update_tryflow(
env,( void*) _temp2069); return( void*) _temp2069;} _LL2057:;}}}} _LL2032: {
void* _temp2075; void* _temp2077; struct _tuple0 _temp2073= Cyc_NewControlFlow_anal_Rexp(
env, inflow, _temp2039); _LL2078: _temp2077= _temp2073.f1; goto _LL2076; _LL2076:
_temp2075= _temp2073.f2; goto _LL2074; _LL2074: _temp2077= Cyc_NewControlFlow_use_Rval(
env, _temp2039->loc, _temp2077, _temp2075); return Cyc_NewControlFlow_add_vars(
_temp2077, _temp2044, Cyc_CfFlowInfo_unknown_all);} _LL2034: return Cyc_NewControlFlow_add_vars(
inflow, _temp2046, Cyc_CfFlowInfo_unknown_none); _LL2036:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp2079=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp2079[ 0]=({ struct
Cyc_Core_Impossible_struct _temp2080; _temp2080.tag= Cyc_Core_Impossible;
_temp2080.f1= _tag_arr("anal_decl: unexpected decl variant", sizeof(
unsigned char), 35u); _temp2080;}); _temp2079;})); _LL2028:;} static void Cyc_NewControlFlow_postpass(
struct Cyc_Absyn_Stmt* s){( void*)( s->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot));{
void* _temp2081=( void*) s->r; struct Cyc_Absyn_Stmt* _temp2105; struct _tuple3
_temp2107; struct Cyc_Absyn_Stmt* _temp2109; struct _tuple3 _temp2111; struct
Cyc_Absyn_Stmt* _temp2113; struct Cyc_Absyn_Stmt* _temp2115; struct Cyc_Absyn_Stmt*
_temp2117; struct Cyc_Absyn_Stmt* _temp2119; struct _tuple3 _temp2121; struct
Cyc_Absyn_Stmt* _temp2123; struct _tuple3 _temp2125; struct Cyc_Absyn_Stmt*
_temp2127; struct Cyc_Absyn_Stmt* _temp2129; struct Cyc_Absyn_Stmt* _temp2131;
struct Cyc_Absyn_Stmt* _temp2133; struct Cyc_Absyn_Stmt* _temp2135; struct Cyc_Absyn_Stmt*
_temp2137; struct Cyc_Absyn_Stmt* _temp2139; struct Cyc_List_List* _temp2141;
struct Cyc_Absyn_Stmt* _temp2143; struct Cyc_List_List* _temp2145; _LL2083: if((
unsigned int) _temp2081 >  1u?*(( int*) _temp2081) ==  Cyc_Absyn_For_s: 0){
_LL2112: _temp2111=(( struct Cyc_Absyn_For_s_struct*) _temp2081)->f2; _LL2114:
_temp2113= _temp2111.f2; goto _LL2108; _LL2108: _temp2107=(( struct Cyc_Absyn_For_s_struct*)
_temp2081)->f3; _LL2110: _temp2109= _temp2107.f2; goto _LL2106; _LL2106:
_temp2105=(( struct Cyc_Absyn_For_s_struct*) _temp2081)->f4; goto _LL2084;}
else{ goto _LL2085;} _LL2085: if(( unsigned int) _temp2081 >  1u?*(( int*)
_temp2081) ==  Cyc_Absyn_Seq_s: 0){ _LL2118: _temp2117=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2081)->f1; goto _LL2116; _LL2116: _temp2115=(( struct Cyc_Absyn_Seq_s_struct*)
_temp2081)->f2; goto _LL2086;} else{ goto _LL2087;} _LL2087: if(( unsigned int)
_temp2081 >  1u?*(( int*) _temp2081) ==  Cyc_Absyn_While_s: 0){ _LL2122:
_temp2121=(( struct Cyc_Absyn_While_s_struct*) _temp2081)->f1; _LL2124:
_temp2123= _temp2121.f2; goto _LL2120; _LL2120: _temp2119=(( struct Cyc_Absyn_While_s_struct*)
_temp2081)->f2; goto _LL2088;} else{ goto _LL2089;} _LL2089: if(( unsigned int)
_temp2081 >  1u?*(( int*) _temp2081) ==  Cyc_Absyn_Do_s: 0){ _LL2130: _temp2129=((
struct Cyc_Absyn_Do_s_struct*) _temp2081)->f1; goto _LL2126; _LL2126: _temp2125=((
struct Cyc_Absyn_Do_s_struct*) _temp2081)->f2; _LL2128: _temp2127= _temp2125.f2;
goto _LL2090;} else{ goto _LL2091;} _LL2091: if(( unsigned int) _temp2081 >  1u?*((
int*) _temp2081) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL2134: _temp2133=(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp2081)->f2; goto _LL2132; _LL2132: _temp2131=((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp2081)->f3; goto _LL2092;} else{ goto
_LL2093;} _LL2093: if(( unsigned int) _temp2081 >  1u?*(( int*) _temp2081) == 
Cyc_Absyn_Region_s: 0){ _LL2136: _temp2135=(( struct Cyc_Absyn_Region_s_struct*)
_temp2081)->f3; goto _LL2094;} else{ goto _LL2095;} _LL2095: if(( unsigned int)
_temp2081 >  1u?*(( int*) _temp2081) ==  Cyc_Absyn_Label_s: 0){ _LL2138:
_temp2137=(( struct Cyc_Absyn_Label_s_struct*) _temp2081)->f2; goto _LL2096;}
else{ goto _LL2097;} _LL2097: if(( unsigned int) _temp2081 >  1u?*(( int*)
_temp2081) ==  Cyc_Absyn_Decl_s: 0){ _LL2140: _temp2139=(( struct Cyc_Absyn_Decl_s_struct*)
_temp2081)->f2; goto _LL2098;} else{ goto _LL2099;} _LL2099: if(( unsigned int)
_temp2081 >  1u?*(( int*) _temp2081) ==  Cyc_Absyn_TryCatch_s: 0){ _LL2144:
_temp2143=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2081)->f1; goto _LL2142;
_LL2142: _temp2141=(( struct Cyc_Absyn_TryCatch_s_struct*) _temp2081)->f2; goto
_LL2100;} else{ goto _LL2101;} _LL2101: if(( unsigned int) _temp2081 >  1u?*((
int*) _temp2081) ==  Cyc_Absyn_Switch_s: 0){ _LL2146: _temp2145=(( struct Cyc_Absyn_Switch_s_struct*)
_temp2081)->f2; goto _LL2102;} else{ goto _LL2103;} _LL2103: goto _LL2104;
_LL2084: Cyc_NewControlFlow_postpass( _temp2109); _temp2117= _temp2105;
_temp2115= _temp2113; goto _LL2086; _LL2086: _temp2123= _temp2117; _temp2119=
_temp2115; goto _LL2088; _LL2088: _temp2129= _temp2119; _temp2127= _temp2123;
goto _LL2090; _LL2090: _temp2133= _temp2129; _temp2131= _temp2127; goto _LL2092;
_LL2092: Cyc_NewControlFlow_postpass( _temp2133); _temp2135= _temp2131; goto
_LL2094; _LL2094: _temp2137= _temp2135; goto _LL2096; _LL2096: _temp2139=
_temp2137; goto _LL2098; _LL2098: Cyc_NewControlFlow_postpass( _temp2139);
return; _LL2100: Cyc_NewControlFlow_postpass( _temp2143); _temp2145= _temp2141;
goto _LL2102; _LL2102: for( 0; _temp2145 !=  0; _temp2145= _temp2145->tl){ Cyc_NewControlFlow_postpass(((
struct Cyc_Absyn_Switch_clause*) _temp2145->hd)->body);} return; _LL2104:
return; _LL2082:;}} static void Cyc_NewControlFlow_check_fun( struct Cyc_Absyn_Fndecl*
fd){ void* inflow= Cyc_NewControlFlow_add_vars(( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp2169=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp2169[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp2170; _temp2170.tag= Cyc_CfFlowInfo_ReachableFL; _temp2170.f1= Cyc_Dict_empty(
Cyc_CfFlowInfo_root_cmp); _temp2170.f2= 0; _temp2170;}); _temp2169;}),( struct
Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( fd->param_vardecls))->v,
Cyc_CfFlowInfo_unknown_all); int noreturn= Cyc_Tcutil_is_noreturn( Cyc_Tcutil_fndecl2typ(
fd)); struct Cyc_NewControlFlow_AnalEnv* env=({ struct Cyc_NewControlFlow_AnalEnv*
_temp2168=( struct Cyc_NewControlFlow_AnalEnv*) _cycalloc( sizeof( struct Cyc_NewControlFlow_AnalEnv));
_temp2168->iterate_again= 1; _temp2168->iteration_num= 0; _temp2168->in_try= 0;
_temp2168->tryflow=( void*) inflow; _temp2168->all_changed= 0; _temp2168->noreturn=
noreturn; _temp2168;}); void* outflow= inflow; while( env->iterate_again) { ++
env->iteration_num; env->iterate_again= 0; outflow= Cyc_NewControlFlow_anal_stmt(
env, inflow, fd->body);}{ void* _temp2147= outflow; _LL2149: if( _temp2147 == (
void*) Cyc_CfFlowInfo_BottomFL){ goto _LL2150;} else{ goto _LL2151;} _LL2151:
goto _LL2152; _LL2150: goto _LL2148; _LL2152: if( noreturn){({ void* _temp2153[
0u]={}; Cyc_Tcutil_terr(( fd->body)->loc, _tag_arr("`noreturn' function might (implicitly) return",
sizeof( unsigned char), 46u), _tag_arr( _temp2153, sizeof( void*), 0u));});}
else{{ void* _temp2154= Cyc_Tcutil_compress(( void*) fd->ret_type); _LL2156: if(
_temp2154 == ( void*) Cyc_Absyn_VoidType){ goto _LL2157;} else{ goto _LL2158;}
_LL2158: if(( unsigned int) _temp2154 >  3u?*(( int*) _temp2154) ==  Cyc_Absyn_DoubleType:
0){ goto _LL2159;} else{ goto _LL2160;} _LL2160: if( _temp2154 == ( void*) Cyc_Absyn_FloatType){
goto _LL2161;} else{ goto _LL2162;} _LL2162: if(( unsigned int) _temp2154 >  3u?*((
int*) _temp2154) ==  Cyc_Absyn_IntType: 0){ goto _LL2163;} else{ goto _LL2164;}
_LL2164: goto _LL2165; _LL2157: goto _LL2155; _LL2159: goto _LL2161; _LL2161:
goto _LL2163; _LL2163:({ void* _temp2166[ 0u]={}; Cyc_Tcutil_warn(( fd->body)->loc,
_tag_arr("function may not return a value", sizeof( unsigned char), 32u),
_tag_arr( _temp2166, sizeof( void*), 0u));}); goto _LL2155; _LL2165:({ void*
_temp2167[ 0u]={}; Cyc_Tcutil_terr(( fd->body)->loc, _tag_arr("function may not return a value",
sizeof( unsigned char), 32u), _tag_arr( _temp2167, sizeof( void*), 0u));}); goto
_LL2155; _LL2155:;} goto _LL2148;} _LL2148:;} Cyc_NewControlFlow_postpass( fd->body);}
void Cyc_NewControlFlow_cf_check( struct Cyc_List_List* ds){ for( 0; ds !=  0;
ds= ds->tl){ void* _temp2171=( void*)(( struct Cyc_Absyn_Decl*) ds->hd)->r;
struct Cyc_Absyn_Fndecl* _temp2183; struct Cyc_List_List* _temp2185; struct Cyc_List_List*
_temp2187; struct Cyc_List_List* _temp2189; _LL2173: if(*(( int*) _temp2171) == 
Cyc_Absyn_Fn_d){ _LL2184: _temp2183=(( struct Cyc_Absyn_Fn_d_struct*) _temp2171)->f1;
goto _LL2174;} else{ goto _LL2175;} _LL2175: if(*(( int*) _temp2171) ==  Cyc_Absyn_ExternC_d){
_LL2186: _temp2185=(( struct Cyc_Absyn_ExternC_d_struct*) _temp2171)->f1; goto
_LL2176;} else{ goto _LL2177;} _LL2177: if(*(( int*) _temp2171) ==  Cyc_Absyn_Using_d){
_LL2188: _temp2187=(( struct Cyc_Absyn_Using_d_struct*) _temp2171)->f2; goto
_LL2178;} else{ goto _LL2179;} _LL2179: if(*(( int*) _temp2171) ==  Cyc_Absyn_Namespace_d){
_LL2190: _temp2189=(( struct Cyc_Absyn_Namespace_d_struct*) _temp2171)->f2; goto
_LL2180;} else{ goto _LL2181;} _LL2181: goto _LL2182; _LL2174: Cyc_NewControlFlow_check_fun(
_temp2183); goto _LL2172; _LL2176: _temp2187= _temp2185; goto _LL2178; _LL2178:
_temp2189= _temp2187; goto _LL2180; _LL2180: Cyc_NewControlFlow_cf_check(
_temp2189); goto _LL2172; _LL2182: goto _LL2172; _LL2172:;}}
