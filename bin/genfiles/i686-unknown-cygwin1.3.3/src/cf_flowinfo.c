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
 extern void exit( int); extern void* abort(); struct Cyc_Std__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; extern unsigned char Cyc_Core_Invalid_argument[
21u]; struct Cyc_Core_Invalid_argument_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern void* Cyc_List_hd( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_tl( struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_mem( int(*
compare)( void*, void*), struct Cyc_List_List* l, void* x); extern int Cyc_List_list_cmp(
int(* cmp)( void*, void*), struct Cyc_List_List* l1, struct Cyc_List_List* l2);
struct Cyc_Set_Set; extern struct Cyc_Set_Set* Cyc_Set_empty( int(* cmp)( void*,
void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void*
elt); extern int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern
struct Cyc_List_List* Cyc_Set_elements( struct Cyc_Set_Set* s); extern
unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern unsigned char
Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; extern
struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* cmp)( void*, void*)); extern struct
Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* k, void* v);
extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* k); extern void*
Cyc_Dict_fold_c( void*(* f)( void*, void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d, void* accum); extern void Cyc_Dict_iter_c( void(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict* Cyc_Dict_map_c(
void*(* f)( void*, void*), void* env, struct Cyc_Dict_Dict* d); extern struct
Cyc_Dict_Dict* Cyc_Dict_union_two_c( void*(* f)( void*, void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern struct
Cyc_Dict_Dict* Cyc_Dict_intersect_c( void*(* f)( void*, void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2); extern int Cyc_Dict_forall_c(
int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict* d); extern int
Cyc_Dict_forall_intersect( int(* f)( void*, void*, void*), struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2); extern int Cyc_Std_zstrptrcmp( struct _tagged_arr*,
struct _tagged_arr*); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file;
struct _tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; static const int Cyc_Position_Lex= 0; static
const int Cyc_Position_Parse= 1; static const int Cyc_Position_Elab= 2; struct
Cyc_Position_Error{ struct _tagged_arr source; struct Cyc_Position_Segment* seg;
void* kind; struct _tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[
14u]; static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0;
struct Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; struct _tuple0{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref;
static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract= 1;
static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
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
0; static const int Cyc_Absyn_Eq_kb= 0; struct Cyc_Absyn_Eq_kb_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_Unknown_kb= 1; struct Cyc_Absyn_Unknown_kb_struct{
int tag; struct Cyc_Core_Opt* f1; } ; static const int Cyc_Absyn_Less_kb= 2;
struct Cyc_Absyn_Less_kb_struct{ int tag; struct Cyc_Core_Opt* f1; void* f2; } ;
struct Cyc_Absyn_Tvar{ struct _tagged_arr* name; int* identity; void* kind; } ;
static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b= 0;
struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars;
struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args; int
c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
struct _tuple0* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl** f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple0* tunion_name; struct _tuple0* field_name; int is_xtunion; } ;
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
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 11; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_EnumType= 12; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int
Cyc_Absyn_SizeofType= 13; struct Cyc_Absyn_SizeofType_struct{ int tag; void* f1;
} ; static const int Cyc_Absyn_AnonStructType= 14; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
15; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_AnonEnumType= 16; struct Cyc_Absyn_AnonEnumType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_RgnHandleType=
17; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 18; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
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
int tag; struct _tuple0* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple0* f1; } ; static
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
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 25; struct _tuple1{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
26; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 27; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 28; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_AnonStruct_e= 29;
struct Cyc_Absyn_AnonStruct_e_struct{ int tag; void* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Tunion_e= 30; struct Cyc_Absyn_Tunion_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ;
static const int Cyc_Absyn_Enum_e= 31; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_AnonEnum_e= 32; struct Cyc_Absyn_AnonEnum_e_struct{
int tag; struct _tuple0* f1; void* f2; struct Cyc_Absyn_Enumfield* f3; } ;
static const int Cyc_Absyn_Malloc_e= 33; struct Cyc_Absyn_Malloc_e_struct{ int
tag; struct Cyc_Absyn_MallocInfo f1; } ; static const int Cyc_Absyn_UnresolvedMem_e=
34; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 35; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 36; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 37; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; void* annot; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_ForArrayInfo{
struct Cyc_List_List* defns; struct _tuple2 condition; struct _tuple2 delta;
struct Cyc_Absyn_Stmt* body; } ; static const int Cyc_Absyn_Skip_s= 0; static
const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int
Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct Cyc_Absyn_While_s_struct{
int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt*
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
_tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
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
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_UnknownCall_p= 12;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; static const int Cyc_Absyn_UnknownFields_p= 13;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
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
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; int escapes; } ; struct Cyc_Absyn_Fndecl{ void*
sc; int is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_Absyn_Stmt* body; struct
Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_arr* name; struct
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple0* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple0* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple0* name; struct Cyc_List_List*
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
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple0*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern struct _tagged_arr* Cyc_Absyn_fieldname( int); struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE; extern struct Cyc_Std___sFILE* Cyc_Std_stderr; extern
unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
18u]; struct Cyc_Std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Std_String_pa= 0; struct Cyc_Std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Std_Int_pa= 1; struct
Cyc_Std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Std_Double_pa=
2; struct Cyc_Std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa=
3; struct Cyc_Std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_Std_fprintf( struct Cyc_Std___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr); extern struct _tagged_arr Cyc_Std_aprintf( struct
_tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0;
struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
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
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_bits_only(
void* t); struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct
Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int generate_line_directives: 1;
int use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern
struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); static const int
Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct
Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt= 1; struct
Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List* fields; } ; static const
int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL= 1; static
const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_EqualConst= 0;
struct Cyc_CfFlowInfo_EqualConst_struct{ int tag; unsigned int f1; } ; static
const int Cyc_CfFlowInfo_LessVar= 1; struct Cyc_CfFlowInfo_LessVar_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_LessSize=
2; struct Cyc_CfFlowInfo_LessSize_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; static const int Cyc_CfFlowInfo_LessConst= 3; struct Cyc_CfFlowInfo_LessConst_struct{
int tag; unsigned int f1; } ; static const int Cyc_CfFlowInfo_LessEqSize= 4;
struct Cyc_CfFlowInfo_LessEqSize_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
} ; struct Cyc_CfFlowInfo_Reln{ struct Cyc_Absyn_Vardecl* vd; void* rop; } ;
unsigned char Cyc_CfFlowInfo_IsZero[ 11u]="\000\000\000\000IsZero"; struct Cyc_CfFlowInfo_IsZero_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_CfFlowInfo_NotZero[
12u]="\000\000\000\000NotZero"; struct Cyc_CfFlowInfo_NotZero_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; unsigned char Cyc_CfFlowInfo_UnknownZ[
13u]="\000\000\000\000UnknownZ"; struct Cyc_CfFlowInfo_UnknownZ_struct{
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
extern void* Cyc_CfFlowInfo_unknown_none; extern void* Cyc_CfFlowInfo_unknown_this;
extern void* Cyc_CfFlowInfo_unknown_all; extern int Cyc_CfFlowInfo_root_cmp(
void*, void*); extern int Cyc_CfFlowInfo_place_cmp( struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*); extern void* Cyc_CfFlowInfo_typ_to_absrval( void*
t, void* leafval); extern void* Cyc_CfFlowInfo_initlevel( struct Cyc_Dict_Dict*
d, void* r); extern void* Cyc_CfFlowInfo_lookup_place( struct Cyc_Dict_Dict* d,
struct Cyc_CfFlowInfo_Place* place); extern int Cyc_CfFlowInfo_is_unescaped(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place); extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void* f1, void* f2); extern struct Cyc_List_List* Cyc_CfFlowInfo_reln_assign_var(
struct Cyc_List_List*, struct Cyc_Absyn_Vardecl*, struct Cyc_Absyn_Exp*); extern
struct Cyc_List_List* Cyc_CfFlowInfo_reln_assign_exp( struct Cyc_List_List*,
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*); extern struct Cyc_List_List* Cyc_CfFlowInfo_reln_kill_var(
struct Cyc_List_List*, struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp( struct Cyc_List_List*, struct Cyc_Absyn_Exp*);
extern void Cyc_CfFlowInfo_print_relns( struct Cyc_List_List*); extern struct
Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_deref( struct Cyc_Dict_Dict* d, struct Cyc_Set_Set**
all_changed, void* r); extern struct Cyc_Dict_Dict* Cyc_CfFlowInfo_assign_place(
struct Cyc_Position_Segment* loc, struct Cyc_Dict_Dict* d, struct Cyc_Set_Set**
all_changed, struct Cyc_CfFlowInfo_Place* place, void* r); extern void* Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**, void*, void*); extern void* Cyc_CfFlowInfo_after_flow(
struct Cyc_Set_Set**, void*, void*, struct Cyc_Set_Set*, struct Cyc_Set_Set*);
static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_none_v={ 0u,(
void*)(( void*) 0u)}; static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_this_v={
0u,( void*)(( void*) 1u)}; static struct Cyc_CfFlowInfo_UnknownR_struct Cyc_CfFlowInfo_unknown_all_v={
0u,( void*)(( void*) 2u)}; static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_none_v={
1u,( void*)(( void*) 0u)}; static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_this_v={
1u,( void*)(( void*) 1u)}; static struct Cyc_CfFlowInfo_Esc_struct Cyc_CfFlowInfo_esc_all_v={
1u,( void*)(( void*) 2u)}; void* Cyc_CfFlowInfo_unknown_none=( void*)& Cyc_CfFlowInfo_unknown_none_v;
void* Cyc_CfFlowInfo_unknown_this=( void*)& Cyc_CfFlowInfo_unknown_this_v; void*
Cyc_CfFlowInfo_unknown_all=( void*)& Cyc_CfFlowInfo_unknown_all_v; void* Cyc_CfFlowInfo_esc_none=(
void*)& Cyc_CfFlowInfo_esc_none_v; void* Cyc_CfFlowInfo_esc_this=( void*)& Cyc_CfFlowInfo_esc_this_v;
void* Cyc_CfFlowInfo_esc_all=( void*)& Cyc_CfFlowInfo_esc_all_v; static struct
Cyc_Set_Set** Cyc_CfFlowInfo_mt_place_set_opt= 0; struct Cyc_Set_Set* Cyc_CfFlowInfo_mt_place_set(){
if( Cyc_CfFlowInfo_mt_place_set_opt ==  0){ Cyc_CfFlowInfo_mt_place_set_opt=({
struct Cyc_Set_Set** _temp6=( struct Cyc_Set_Set**) _cycalloc( sizeof( struct
Cyc_Set_Set*)); _temp6[ 0]=(( struct Cyc_Set_Set*(*)( int(* cmp)( struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*))) Cyc_Set_empty)( Cyc_CfFlowInfo_place_cmp);
_temp6;});} return*(( struct Cyc_Set_Set**) _check_null( Cyc_CfFlowInfo_mt_place_set_opt));}
int Cyc_CfFlowInfo_root_cmp( void* r1, void* r2){ if(( int) r1 == ( int) r2){
return 0;}{ int p1; int p2;{ void* _temp7= r1; struct Cyc_Absyn_Vardecl* _temp13;
struct Cyc_Absyn_Exp* _temp15; _LL9: if(*(( int*) _temp7) ==  Cyc_CfFlowInfo_VarRoot){
_LL14: _temp13=(( struct Cyc_CfFlowInfo_VarRoot_struct*) _temp7)->f1; goto _LL10;}
else{ goto _LL11;} _LL11: if(*(( int*) _temp7) ==  Cyc_CfFlowInfo_MallocPt){
_LL16: _temp15=(( struct Cyc_CfFlowInfo_MallocPt_struct*) _temp7)->f1; goto
_LL12;} else{ goto _LL8;} _LL10: p1=( int) _temp13; goto _LL8; _LL12: p1=( int)
_temp15; _LL8:;}{ void* _temp17= r2; struct Cyc_Absyn_Vardecl* _temp23; struct
Cyc_Absyn_Exp* _temp25; _LL19: if(*(( int*) _temp17) ==  Cyc_CfFlowInfo_VarRoot){
_LL24: _temp23=(( struct Cyc_CfFlowInfo_VarRoot_struct*) _temp17)->f1; goto
_LL20;} else{ goto _LL21;} _LL21: if(*(( int*) _temp17) ==  Cyc_CfFlowInfo_MallocPt){
_LL26: _temp25=(( struct Cyc_CfFlowInfo_MallocPt_struct*) _temp17)->f1; goto
_LL22;} else{ goto _LL18;} _LL20: p2=( int) _temp23; goto _LL18; _LL22: p2=( int)
_temp25; _LL18:;} return p1 -  p2;}} int Cyc_CfFlowInfo_place_cmp( struct Cyc_CfFlowInfo_Place*
p1, struct Cyc_CfFlowInfo_Place* p2){ if(( int) p1 == ( int) p2){ return 0;}{
int i= Cyc_CfFlowInfo_root_cmp(( void*) p1->root,( void*) p2->root); if( i !=  0){
return i;} return(( int(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*),
struct Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_Std_zstrptrcmp,
p1->fields, p2->fields);}} static struct _tagged_arr* Cyc_CfFlowInfo_place2string(
struct Cyc_CfFlowInfo_Place* p){ struct Cyc_List_List* sl= 0;{ void* _temp27=(
void*) p->root; struct Cyc_Absyn_Vardecl* _temp33; struct Cyc_Absyn_Exp* _temp35;
_LL29: if(*(( int*) _temp27) ==  Cyc_CfFlowInfo_VarRoot){ _LL34: _temp33=((
struct Cyc_CfFlowInfo_VarRoot_struct*) _temp27)->f1; goto _LL30;} else{ goto
_LL31;} _LL31: if(*(( int*) _temp27) ==  Cyc_CfFlowInfo_MallocPt){ _LL36:
_temp35=(( struct Cyc_CfFlowInfo_MallocPt_struct*) _temp27)->f1; goto _LL32;}
else{ goto _LL28;} _LL30: sl=({ struct Cyc_List_List* _temp37=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp37->hd=( void*)({ struct
_tagged_arr* _temp38=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp38[ 0]=({ struct Cyc_Std_String_pa_struct _temp40; _temp40.tag=
Cyc_Std_String_pa; _temp40.f1=( struct _tagged_arr)*(* _temp33->name).f2;{ void*
_temp39[ 1u]={& _temp40}; Cyc_Std_aprintf( _tag_arr("%s", sizeof( unsigned char),
3u), _tag_arr( _temp39, sizeof( void*), 1u));}}); _temp38;}); _temp37->tl= sl;
_temp37;}); goto _LL28; _LL32: sl=({ struct Cyc_List_List* _temp41=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp41->hd=( void*)({ struct
_tagged_arr* _temp42=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp42[ 0]=({ struct Cyc_Std_Int_pa_struct _temp44; _temp44.tag=
Cyc_Std_Int_pa; _temp44.f1=( int)(( unsigned int)(( int) _temp35));{ void*
_temp43[ 1u]={& _temp44}; Cyc_Std_aprintf( _tag_arr("mpt%d", sizeof(
unsigned char), 6u), _tag_arr( _temp43, sizeof( void*), 1u));}}); _temp42;});
_temp41->tl= sl; _temp41;}); _LL28:;}{ struct Cyc_List_List* fields= p->fields;
for( 0; fields !=  0; fields= fields->tl){ sl=({ struct Cyc_List_List* _temp45=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp45->hd=(
void*)({ struct _tagged_arr* _temp46=( struct _tagged_arr*) _cycalloc( sizeof(
struct _tagged_arr)); _temp46[ 0]=({ struct Cyc_Std_String_pa_struct _temp48;
_temp48.tag= Cyc_Std_String_pa; _temp48.f1=( struct _tagged_arr)*(( struct
_tagged_arr*) fields->hd);{ void* _temp47[ 1u]={& _temp48}; Cyc_Std_aprintf(
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp47, sizeof( void*), 1u));}});
_temp46;}); _temp45->tl= sl; _temp45;});}}{ struct _tagged_arr* _temp49=({
struct _tagged_arr* _temp53=( struct _tagged_arr*) _cycalloc( sizeof( struct
_tagged_arr)); _temp53[ 0]=({ struct Cyc_Std_String_pa_struct _temp55; _temp55.tag=
Cyc_Std_String_pa; _temp55.f1= _tag_arr("", sizeof( unsigned char), 1u);{ void*
_temp54[ 1u]={& _temp55}; Cyc_Std_aprintf( _tag_arr("%s", sizeof( unsigned char),
3u), _tag_arr( _temp54, sizeof( void*), 1u));}}); _temp53;}); for( 0; sl !=  0;
sl= sl->tl){* _temp49=({ struct Cyc_Std_String_pa_struct _temp52; _temp52.tag=
Cyc_Std_String_pa; _temp52.f1=( struct _tagged_arr)* _temp49;{ struct Cyc_Std_String_pa_struct
_temp51; _temp51.tag= Cyc_Std_String_pa; _temp51.f1=( struct _tagged_arr)*((
struct _tagged_arr*) sl->hd);{ void* _temp50[ 2u]={& _temp51,& _temp52}; Cyc_Std_aprintf(
_tag_arr("%s.%s", sizeof( unsigned char), 6u), _tag_arr( _temp50, sizeof( void*),
2u));}}});} return _temp49;}} struct _tuple3{ struct Cyc_Absyn_Tqual f1; void*
f2; } ; void* Cyc_CfFlowInfo_typ_to_absrval( void* t, void* leafval){ void*
_temp56= Cyc_Tcutil_compress( t); struct Cyc_Absyn_TunionFieldInfo _temp68; void*
_temp70; struct Cyc_Absyn_Tunionfield* _temp72; struct Cyc_List_List* _temp74;
struct Cyc_List_List* _temp76; struct Cyc_Absyn_Structdecl** _temp78; struct Cyc_Absyn_Structdecl*
_temp80; struct Cyc_Absyn_Structdecl _temp81; struct Cyc_Core_Opt* _temp82;
struct Cyc_Core_Opt _temp84; struct Cyc_List_List* _temp85; _LL58: if((
unsigned int) _temp56 >  3u?*(( int*) _temp56) ==  Cyc_Absyn_TunionFieldType: 0){
_LL69: _temp68=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp56)->f1; _LL71:
_temp70=( void*) _temp68.field_info; if(*(( int*) _temp70) ==  Cyc_Absyn_KnownTunionfield){
_LL73: _temp72=(( struct Cyc_Absyn_KnownTunionfield_struct*) _temp70)->f2; goto
_LL59;} else{ goto _LL60;}} else{ goto _LL60;} _LL60: if(( unsigned int) _temp56
>  3u?*(( int*) _temp56) ==  Cyc_Absyn_TupleType: 0){ _LL75: _temp74=(( struct
Cyc_Absyn_TupleType_struct*) _temp56)->f1; goto _LL61;} else{ goto _LL62;} _LL62:
if(( unsigned int) _temp56 >  3u?*(( int*) _temp56) ==  Cyc_Absyn_AnonStructType:
0){ _LL77: _temp76=(( struct Cyc_Absyn_AnonStructType_struct*) _temp56)->f1;
goto _LL63;} else{ goto _LL64;} _LL64: if(( unsigned int) _temp56 >  3u?*(( int*)
_temp56) ==  Cyc_Absyn_StructType: 0){ _LL79: _temp78=(( struct Cyc_Absyn_StructType_struct*)
_temp56)->f3; if( _temp78 ==  0){ goto _LL66;} else{ _temp80=* _temp78; _temp81=*
_temp80; _LL83: _temp82= _temp81.fields; if( _temp82 ==  0){ goto _LL66;} else{
_temp84=* _temp82; _LL86: _temp85=( struct Cyc_List_List*) _temp84.v; goto _LL65;}}}
else{ goto _LL66;} _LL66: goto _LL67; _LL59: if( _temp72->typs ==  0){ return
leafval;} _temp74= _temp72->typs; goto _LL61; _LL61: { struct Cyc_Dict_Dict* d=((
struct Cyc_Dict_Dict*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*)))
Cyc_Dict_empty)( Cyc_Std_zstrptrcmp);{ int i= 0; for( 0; _temp74 !=  0;( _temp74=
_temp74->tl, ++ i)){ d=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( d, Cyc_Absyn_fieldname( i),
Cyc_CfFlowInfo_typ_to_absrval((*(( struct _tuple3*) _temp74->hd)).f2, leafval));}}
return( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct* _temp87=( struct Cyc_CfFlowInfo_Aggregate_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Aggregate_struct)); _temp87[ 0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _temp88; _temp88.tag= Cyc_CfFlowInfo_Aggregate;
_temp88.f1= d; _temp88;}); _temp87;});} _LL63: _temp85= _temp76; goto _LL65;
_LL65: { struct Cyc_Dict_Dict* d=(( struct Cyc_Dict_Dict*(*)( int(* cmp)( struct
_tagged_arr*, struct _tagged_arr*))) Cyc_Dict_empty)( Cyc_Std_zstrptrcmp); for(
0; _temp85 !=  0; _temp85= _temp85->tl){ struct Cyc_Absyn_Structfield _temp91;
void* _temp92; struct _tagged_arr* _temp94; struct Cyc_Absyn_Structfield*
_temp89=( struct Cyc_Absyn_Structfield*) _temp85->hd; _temp91=* _temp89; _LL95:
_temp94= _temp91.name; goto _LL93; _LL93: _temp92=( void*) _temp91.type; goto
_LL90; _LL90: if( _get_arr_size(* _temp94, sizeof( unsigned char)) !=  1){ d=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void*
v)) Cyc_Dict_insert)( d, _temp94, Cyc_CfFlowInfo_typ_to_absrval( _temp92,
leafval));}} return( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct* _temp96=(
struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Aggregate_struct));
_temp96[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct _temp97; _temp97.tag= Cyc_CfFlowInfo_Aggregate;
_temp97.f1= d; _temp97;}); _temp96;});} _LL67: return Cyc_Tcutil_bits_only( t)?
Cyc_CfFlowInfo_unknown_all: leafval; _LL57:;} static int Cyc_CfFlowInfo_prefix_of_member(
struct Cyc_CfFlowInfo_Place* place, struct Cyc_Set_Set* set){{ struct Cyc_List_List*
_temp98=(( struct Cyc_List_List*(*)( struct Cyc_Set_Set* s)) Cyc_Set_elements)(
set); for( 0; _temp98 !=  0; _temp98= _temp98->tl){ struct Cyc_CfFlowInfo_Place*
_temp99=( struct Cyc_CfFlowInfo_Place*) _temp98->hd; if( Cyc_CfFlowInfo_root_cmp((
void*) place->root,( void*) _temp99->root) !=  0){ continue;}{ struct Cyc_List_List*
_temp100= place->fields; struct Cyc_List_List* _temp101= _temp99->fields; for( 0;
_temp100 !=  0? _temp101 !=  0: 0;( _temp100= _temp100->tl, _temp101= _temp101->tl)){
if( Cyc_Std_zstrptrcmp(( struct _tagged_arr*) _temp100->hd,( struct _tagged_arr*)
_temp101->hd) !=  0){ break;}} if( _temp100 ==  0){ return 1;}}}} return 0;}
struct Cyc_CfFlowInfo_EscPile{ struct _RegionHandle* rgn; struct Cyc_List_List*
places; } ; static void Cyc_CfFlowInfo_add_place( struct Cyc_CfFlowInfo_EscPile*
pile, struct Cyc_CfFlowInfo_Place* place){ if( !(( int(*)( int(* compare)(
struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*), struct Cyc_List_List*
l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,
pile->places, place)){ pile->places=({ struct Cyc_List_List* _temp102=( struct
Cyc_List_List*) _region_malloc( pile->rgn, sizeof( struct Cyc_List_List));
_temp102->hd=( void*) place; _temp102->tl= pile->places; _temp102;});}} static
void Cyc_CfFlowInfo_add_places( struct Cyc_CfFlowInfo_EscPile* pile, void* a,
void* r){ void* _temp103= r; struct Cyc_CfFlowInfo_Place* _temp111; struct Cyc_Dict_Dict*
_temp113; _LL105: if(( unsigned int) _temp103 >  3u?*(( int*) _temp103) ==  Cyc_CfFlowInfo_AddressOf:
0){ _LL112: _temp111=(( struct Cyc_CfFlowInfo_AddressOf_struct*) _temp103)->f1;
goto _LL106;} else{ goto _LL107;} _LL107: if(( unsigned int) _temp103 >  3u?*((
int*) _temp103) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL114: _temp113=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp103)->f1; goto _LL108;} else{ goto _LL109;} _LL109: goto _LL110; _LL106:
Cyc_CfFlowInfo_add_place( pile, _temp111); return; _LL108:(( void(*)( void(* f)(
struct Cyc_CfFlowInfo_EscPile*, struct _tagged_arr*, void*), struct Cyc_CfFlowInfo_EscPile*
env, struct Cyc_Dict_Dict* d)) Cyc_Dict_iter_c)(( void(*)( struct Cyc_CfFlowInfo_EscPile*
pile, struct _tagged_arr* a, void* r)) Cyc_CfFlowInfo_add_places, pile, _temp113);
return; _LL110: return; _LL104:;} static void* Cyc_CfFlowInfo_insert_place_inner(
void* new_val, void* old_val){ void* _temp115= old_val; struct Cyc_Dict_Dict*
_temp121; _LL117: if(( unsigned int) _temp115 >  3u?*(( int*) _temp115) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL122: _temp121=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp115)->f1;
goto _LL118;} else{ goto _LL119;} _LL119: goto _LL120; _LL118: return( void*)({
struct Cyc_CfFlowInfo_Aggregate_struct* _temp123=( struct Cyc_CfFlowInfo_Aggregate_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Aggregate_struct)); _temp123[ 0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _temp124; _temp124.tag= Cyc_CfFlowInfo_Aggregate;
_temp124.f1=(( struct Cyc_Dict_Dict*(*)( void*(* f)( void*, void*), void* env,
struct Cyc_Dict_Dict* d)) Cyc_Dict_map_c)( Cyc_CfFlowInfo_insert_place_inner,
new_val, _temp121); _temp124;}); _temp123;}); _LL120: return new_val; _LL116:;}
struct _tuple4{ struct Cyc_List_List* f1; void* f2; } ; static void* Cyc_CfFlowInfo_insert_place_outer(
struct Cyc_List_List* fs, void* old_val, void* new_val){ if( fs ==  0){ return
Cyc_CfFlowInfo_insert_place_inner( new_val, old_val);}{ struct _tuple4 _temp126=({
struct _tuple4 _temp125; _temp125.f1= fs; _temp125.f2= old_val; _temp125;});
void* _temp132; struct Cyc_Dict_Dict* _temp134; struct Cyc_List_List* _temp136;
struct Cyc_List_List _temp138; struct Cyc_List_List* _temp139; struct
_tagged_arr* _temp141; _LL128: _LL137: _temp136= _temp126.f1; if( _temp136 ==  0){
goto _LL130;} else{ _temp138=* _temp136; _LL142: _temp141=( struct _tagged_arr*)
_temp138.hd; goto _LL140; _LL140: _temp139= _temp138.tl; goto _LL133;} _LL133:
_temp132= _temp126.f2; if(( unsigned int) _temp132 >  3u?*(( int*) _temp132) == 
Cyc_CfFlowInfo_Aggregate: 0){ _LL135: _temp134=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp132)->f1; goto _LL129;} else{ goto _LL130;} _LL130: goto _LL131; _LL129: {
void* _temp143= Cyc_CfFlowInfo_insert_place_outer( _temp139,(( void*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp134, _temp141),
new_val); return( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct* _temp144=(
struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Aggregate_struct));
_temp144[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct _temp145; _temp145.tag=
Cyc_CfFlowInfo_Aggregate; _temp145.f1=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)( _temp134, _temp141,
_temp143); _temp145;}); _temp144;});} _LL131:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp146=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp146[ 0]=({ struct Cyc_Core_Impossible_struct _temp147; _temp147.tag= Cyc_Core_Impossible;
_temp147.f1= _tag_arr("bad insert place", sizeof( unsigned char), 17u); _temp147;});
_temp146;})); _LL127:;}} static struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_EscPile* pile, struct Cyc_Set_Set** all_changed, struct
Cyc_Dict_Dict* d){ while( pile->places !=  0) { struct Cyc_CfFlowInfo_Place*
_temp148=( struct Cyc_CfFlowInfo_Place*)(( struct Cyc_List_List*) _check_null(
pile->places))->hd; pile->places=(( struct Cyc_List_List*) _check_null( pile->places))->tl;
if( all_changed !=  0){* all_changed=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set*
s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(* all_changed, _temp148);}{
void* oldval; void* newval;{ struct _handler_cons _temp149; _push_handler(&
_temp149);{ int _temp151= 0; if( setjmp( _temp149.handler)){ _temp151= 1;} if( !
_temp151){ oldval= Cyc_CfFlowInfo_lookup_place( d, _temp148);; _pop_handler();}
else{ void* _temp150=( void*) _exn_thrown; void* _temp153= _temp150; _LL155: if(
_temp153 ==  Cyc_Dict_Absent){ goto _LL156;} else{ goto _LL157;} _LL157: goto
_LL158; _LL156: continue; _LL158:( void) _throw( _temp153); _LL154:;}}}{ void*
_temp159= Cyc_CfFlowInfo_initlevel( d, oldval); _LL161: if( _temp159 == ( void*)
Cyc_CfFlowInfo_AllIL){ goto _LL162;} else{ goto _LL163;} _LL163: if( _temp159 == (
void*) Cyc_CfFlowInfo_ThisIL){ goto _LL164;} else{ goto _LL165;} _LL165: if(
_temp159 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL166;} else{ goto _LL160;}
_LL162: newval= Cyc_CfFlowInfo_esc_all; goto _LL160; _LL164: newval= Cyc_CfFlowInfo_esc_this;
goto _LL160; _LL166: newval= Cyc_CfFlowInfo_esc_none; goto _LL160; _LL160:;}((
void(*)( struct Cyc_CfFlowInfo_EscPile* pile, int a, void* r)) Cyc_CfFlowInfo_add_places)(
pile, 0, oldval); d= Cyc_Dict_insert( d,( void*) _temp148->root, Cyc_CfFlowInfo_insert_place_outer(
_temp148->fields, Cyc_Dict_lookup( d,( void*) _temp148->root), newval));}}
return d;} struct Cyc_CfFlowInfo_InitlevelEnv{ struct Cyc_Dict_Dict* d; struct
Cyc_List_List* seen; } ; static void* Cyc_CfFlowInfo_initlevel_approx( void* r){
void* _temp167= r; void* _temp181; void* _temp183; _LL169: if(( unsigned int)
_temp167 >  3u?*(( int*) _temp167) ==  Cyc_CfFlowInfo_UnknownR: 0){ _LL182:
_temp181=( void*)(( struct Cyc_CfFlowInfo_UnknownR_struct*) _temp167)->f1; goto
_LL170;} else{ goto _LL171;} _LL171: if(( unsigned int) _temp167 >  3u?*(( int*)
_temp167) ==  Cyc_CfFlowInfo_Esc: 0){ _LL184: _temp183=( void*)(( struct Cyc_CfFlowInfo_Esc_struct*)
_temp167)->f1; goto _LL172;} else{ goto _LL173;} _LL173: if( _temp167 == ( void*)
Cyc_CfFlowInfo_Zero){ goto _LL174;} else{ goto _LL175;} _LL175: if( _temp167 == (
void*) Cyc_CfFlowInfo_NotZeroAll){ goto _LL176;} else{ goto _LL177;} _LL177: if(
_temp167 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto _LL178;} else{ goto
_LL179;} _LL179: goto _LL180; _LL170: return _temp181; _LL172: return _temp183;
_LL174: goto _LL176; _LL176: return( void*) Cyc_CfFlowInfo_AllIL; _LL178: return(
void*) Cyc_CfFlowInfo_ThisIL; _LL180:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp185=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp185[ 0]=({ struct Cyc_Core_Impossible_struct _temp186; _temp186.tag= Cyc_Core_Impossible;
_temp186.f1= _tag_arr("initlevel_approx", sizeof( unsigned char), 17u); _temp186;});
_temp185;})); _LL168:;} static void* Cyc_CfFlowInfo_initlevel_rec( struct Cyc_CfFlowInfo_InitlevelEnv*
env, void* a, void* r, void* acc){ void* this_ans; if( acc == ( void*) Cyc_CfFlowInfo_NoneIL){
return( void*) Cyc_CfFlowInfo_NoneIL;}{ void* _temp187= r; struct Cyc_Dict_Dict*
_temp195; struct Cyc_CfFlowInfo_Place* _temp197; _LL189: if(( unsigned int)
_temp187 >  3u?*(( int*) _temp187) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL196:
_temp195=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp187)->f1; goto _LL190;}
else{ goto _LL191;} _LL191: if(( unsigned int) _temp187 >  3u?*(( int*) _temp187)
==  Cyc_CfFlowInfo_AddressOf: 0){ _LL198: _temp197=(( struct Cyc_CfFlowInfo_AddressOf_struct*)
_temp187)->f1; goto _LL192;} else{ goto _LL193;} _LL193: goto _LL194; _LL190:
this_ans=(( void*(*)( void*(* f)( struct Cyc_CfFlowInfo_InitlevelEnv*, struct
_tagged_arr*, void*, void*), struct Cyc_CfFlowInfo_InitlevelEnv* env, struct Cyc_Dict_Dict*
d, void* accum)) Cyc_Dict_fold_c)(( void*(*)( struct Cyc_CfFlowInfo_InitlevelEnv*
env, struct _tagged_arr* a, void* r, void* acc)) Cyc_CfFlowInfo_initlevel_rec,
env, _temp195,( void*) Cyc_CfFlowInfo_AllIL); goto _LL188; _LL192: if((( int(*)(
int(* compare)( struct Cyc_CfFlowInfo_Place*, struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List* l, struct Cyc_CfFlowInfo_Place* x)) Cyc_List_mem)( Cyc_CfFlowInfo_place_cmp,
env->seen, _temp197)){ this_ans=( void*) Cyc_CfFlowInfo_AllIL;} else{ env->seen=({
struct Cyc_List_List* _temp199=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp199->hd=( void*) _temp197; _temp199->tl= env->seen;
_temp199;}); this_ans=(( void*(*)( struct Cyc_CfFlowInfo_InitlevelEnv* env, int
a, void* r, void* acc)) Cyc_CfFlowInfo_initlevel_rec)( env, 0, Cyc_CfFlowInfo_lookup_place(
env->d, _temp197),( void*) Cyc_CfFlowInfo_AllIL); env->seen=(( struct Cyc_List_List*)
_check_null( env->seen))->tl; if( this_ans == ( void*) Cyc_CfFlowInfo_NoneIL){
this_ans=( void*) Cyc_CfFlowInfo_ThisIL;}} goto _LL188; _LL194: this_ans= Cyc_CfFlowInfo_initlevel_approx(
r); _LL188:;} if( this_ans == ( void*) Cyc_CfFlowInfo_NoneIL){ return( void*)
Cyc_CfFlowInfo_NoneIL;} if( this_ans == ( void*) Cyc_CfFlowInfo_ThisIL? 1: acc
== ( void*) Cyc_CfFlowInfo_ThisIL){ return( void*) Cyc_CfFlowInfo_ThisIL;}
return( void*) Cyc_CfFlowInfo_AllIL;} void* Cyc_CfFlowInfo_initlevel( struct Cyc_Dict_Dict*
d, void* r){ struct Cyc_CfFlowInfo_InitlevelEnv _temp200=({ struct Cyc_CfFlowInfo_InitlevelEnv
_temp201; _temp201.d= d; _temp201.seen= 0; _temp201;}); return(( void*(*)(
struct Cyc_CfFlowInfo_InitlevelEnv* env, int a, void* r, void* acc)) Cyc_CfFlowInfo_initlevel_rec)(&
_temp200, 0, r,( void*) Cyc_CfFlowInfo_AllIL);} void* Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict* d, struct Cyc_CfFlowInfo_Place* place){ struct Cyc_CfFlowInfo_Place
_temp204; struct Cyc_List_List* _temp205; void* _temp207; struct Cyc_CfFlowInfo_Place*
_temp202= place; _temp204=* _temp202; _LL208: _temp207=( void*) _temp204.root;
goto _LL206; _LL206: _temp205= _temp204.fields; goto _LL203; _LL203: { void*
_temp209= Cyc_Dict_lookup( d, _temp207); for( 0; _temp205 !=  0; _temp205=
_temp205->tl){ struct _tuple0 _temp211=({ struct _tuple0 _temp210; _temp210.f1=
_temp209; _temp210.f2=( struct _tagged_arr*) _temp205->hd; _temp210;}); struct
_tagged_arr* _temp217; void* _temp219; struct Cyc_Dict_Dict* _temp221; _LL213:
_LL220: _temp219= _temp211.f1; if(( unsigned int) _temp219 >  3u?*(( int*)
_temp219) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL222: _temp221=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp219)->f1; goto _LL218;} else{ goto _LL215;} _LL218: _temp217= _temp211.f2;
goto _LL214; _LL215: goto _LL216; _LL214: _temp209=(( void*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k)) Cyc_Dict_lookup)( _temp221, _temp217); goto _LL212;
_LL216:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct* _temp223=(
struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp223[ 0]=({ struct Cyc_Core_Impossible_struct _temp224; _temp224.tag= Cyc_Core_Impossible;
_temp224.f1= _tag_arr("bad lookup_place", sizeof( unsigned char), 17u); _temp224;});
_temp223;})); _LL212:;} return _temp209;}} static int Cyc_CfFlowInfo_is_rval_unescaped(
void* a, void* b, void* rval){ void* _temp225= rval; struct Cyc_Dict_Dict*
_temp233; _LL227: if(( unsigned int) _temp225 >  3u?*(( int*) _temp225) ==  Cyc_CfFlowInfo_Esc:
0){ goto _LL228;} else{ goto _LL229;} _LL229: if(( unsigned int) _temp225 >  3u?*((
int*) _temp225) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL234: _temp233=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp225)->f1; goto _LL230;} else{ goto _LL231;} _LL231: goto _LL232; _LL228:
return 0; _LL230: return(( int(*)( int(* f)( int, struct _tagged_arr*, void*),
int env, struct Cyc_Dict_Dict* d)) Cyc_Dict_forall_c)(( int(*)( int a, struct
_tagged_arr* b, void* rval)) Cyc_CfFlowInfo_is_rval_unescaped, 0, _temp233);
_LL232: return 1; _LL226:;} int Cyc_CfFlowInfo_is_unescaped( struct Cyc_Dict_Dict*
d, struct Cyc_CfFlowInfo_Place* place){ return(( int(*)( int a, int b, void*
rval)) Cyc_CfFlowInfo_is_rval_unescaped)( 0, 0, Cyc_CfFlowInfo_lookup_place( d,
place));} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_escape_deref( struct Cyc_Dict_Dict*
d, struct Cyc_Set_Set** all_changed, void* r){ struct _RegionHandle _temp235=
_new_region("rgn"); struct _RegionHandle* rgn=& _temp235; _push_region( rgn);{
struct Cyc_CfFlowInfo_EscPile* pile=({ struct Cyc_CfFlowInfo_EscPile* _temp237=(
struct Cyc_CfFlowInfo_EscPile*) _region_malloc( rgn, sizeof( struct Cyc_CfFlowInfo_EscPile));
_temp237->rgn= rgn; _temp237->places= 0; _temp237;});(( void(*)( struct Cyc_CfFlowInfo_EscPile*
pile, int a, void* r)) Cyc_CfFlowInfo_add_places)( pile, 0, r);{ struct Cyc_Dict_Dict*
_temp236= Cyc_CfFlowInfo_escape_these( pile, all_changed, d); _npop_handler( 0u);
return _temp236;}}; _pop_region( rgn);} struct Cyc_CfFlowInfo_AssignEnv{ struct
Cyc_CfFlowInfo_EscPile* pile; struct Cyc_Dict_Dict* d; struct Cyc_Position_Segment*
loc; } ; struct _tuple5{ void* f1; void* f2; } ; static void* Cyc_CfFlowInfo_assign_place_inner(
struct Cyc_CfFlowInfo_AssignEnv* env, void* a, void* oldval, void* newval){
struct _tuple5 _temp239=({ struct _tuple5 _temp238; _temp238.f1= oldval;
_temp238.f2= newval; _temp238;}); void* _temp251; struct Cyc_CfFlowInfo_Place*
_temp253; void* _temp255; void* _temp257; void* _temp259; struct Cyc_Dict_Dict*
_temp261; void* _temp263; struct Cyc_Dict_Dict* _temp265; void* _temp267; void*
_temp269; _LL241: _LL256: _temp255= _temp239.f1; if(( unsigned int) _temp255 > 
3u?*(( int*) _temp255) ==  Cyc_CfFlowInfo_Esc: 0){ goto _LL252;} else{ goto
_LL243;} _LL252: _temp251= _temp239.f2; if(( unsigned int) _temp251 >  3u?*((
int*) _temp251) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL254: _temp253=(( struct Cyc_CfFlowInfo_AddressOf_struct*)
_temp251)->f1; goto _LL242;} else{ goto _LL243;} _LL243: _LL258: _temp257=
_temp239.f1; if(( unsigned int) _temp257 >  3u?*(( int*) _temp257) ==  Cyc_CfFlowInfo_Esc:
0){ goto _LL244;} else{ goto _LL245;} _LL245: _LL264: _temp263= _temp239.f1; if((
unsigned int) _temp263 >  3u?*(( int*) _temp263) ==  Cyc_CfFlowInfo_Aggregate: 0){
_LL266: _temp265=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp263)->f1; goto
_LL260;} else{ goto _LL247;} _LL260: _temp259= _temp239.f2; if(( unsigned int)
_temp259 >  3u?*(( int*) _temp259) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL262:
_temp261=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp259)->f1; goto _LL246;}
else{ goto _LL247;} _LL247: _LL268: _temp267= _temp239.f2; if(( unsigned int)
_temp267 >  3u?*(( int*) _temp267) ==  Cyc_CfFlowInfo_Esc: 0){ _LL270: _temp269=(
void*)(( struct Cyc_CfFlowInfo_Esc_struct*) _temp267)->f1; goto _LL248;} else{
goto _LL249;} _LL249: goto _LL250; _LL242: Cyc_CfFlowInfo_add_place( env->pile,
_temp253); goto _LL244; _LL244: if( Cyc_CfFlowInfo_initlevel( env->d, newval) != (
void*) Cyc_CfFlowInfo_AllIL){({ void* _temp271[ 0u]={}; Cyc_Tcutil_terr( env->loc,
_tag_arr("assignment puts possibly-uninitialized data in an escaped location",
sizeof( unsigned char), 67u), _tag_arr( _temp271, sizeof( void*), 0u));});}
return Cyc_CfFlowInfo_esc_all; _LL246: { struct Cyc_Dict_Dict* _temp272=((
struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_AssignEnv*, struct
_tagged_arr*, void*, void*), struct Cyc_CfFlowInfo_AssignEnv* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_union_two_c)(( void*(*)( struct Cyc_CfFlowInfo_AssignEnv*
env, struct _tagged_arr* a, void* oldval, void* newval)) Cyc_CfFlowInfo_assign_place_inner,
env, _temp265, _temp261); if( _temp272 ==  _temp265){ return oldval;} if(
_temp272 ==  _temp261){ return newval;} return( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp273=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp273[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp274; _temp274.tag= Cyc_CfFlowInfo_Aggregate; _temp274.f1= _temp272;
_temp274;}); _temp273;});} _LL248: { void* _temp275= _temp269; _LL277: if(
_temp275 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL278;} else{ goto _LL279;}
_LL279: if( _temp275 == ( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL280;} else{
goto _LL281;} _LL281: if( _temp275 == ( void*) Cyc_CfFlowInfo_AllIL){ goto
_LL282;} else{ goto _LL276;} _LL278: return Cyc_CfFlowInfo_unknown_none; _LL280:
return Cyc_CfFlowInfo_unknown_this; _LL282: return Cyc_CfFlowInfo_unknown_all;
_LL276:;} _LL250: return newval; _LL240:;} static void* Cyc_CfFlowInfo_assign_place_outer(
struct Cyc_CfFlowInfo_AssignEnv* env, struct Cyc_List_List* fs, void* oldval,
void* newval){ if( fs ==  0){ return(( void*(*)( struct Cyc_CfFlowInfo_AssignEnv*
env, int a, void* oldval, void* newval)) Cyc_CfFlowInfo_assign_place_inner)( env,
0, oldval, newval);}{ struct _tuple4 _temp284=({ struct _tuple4 _temp283;
_temp283.f1= fs; _temp283.f2= oldval; _temp283;}); void* _temp290; struct Cyc_Dict_Dict*
_temp292; struct Cyc_List_List* _temp294; struct Cyc_List_List _temp296; struct
Cyc_List_List* _temp297; struct _tagged_arr* _temp299; _LL286: _LL295: _temp294=
_temp284.f1; if( _temp294 ==  0){ goto _LL288;} else{ _temp296=* _temp294;
_LL300: _temp299=( struct _tagged_arr*) _temp296.hd; goto _LL298; _LL298:
_temp297= _temp296.tl; goto _LL291;} _LL291: _temp290= _temp284.f2; if((
unsigned int) _temp290 >  3u?*(( int*) _temp290) ==  Cyc_CfFlowInfo_Aggregate: 0){
_LL293: _temp292=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp290)->f1; goto
_LL287;} else{ goto _LL288;} _LL288: goto _LL289; _LL287: { void* _temp301= Cyc_CfFlowInfo_assign_place_outer(
env, _temp297,(( void*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)(
_temp292, _temp299), newval); return( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp302=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp302[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp303; _temp303.tag= Cyc_CfFlowInfo_Aggregate; _temp303.f1=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, void* v)) Cyc_Dict_insert)(
_temp292, _temp299, _temp301); _temp303;}); _temp302;});} _LL289:( int) _throw((
void*)({ struct Cyc_Core_Impossible_struct* _temp304=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp304[ 0]=({ struct
Cyc_Core_Impossible_struct _temp305; _temp305.tag= Cyc_Core_Impossible; _temp305.f1=
_tag_arr("bad insert place", sizeof( unsigned char), 17u); _temp305;}); _temp304;}));
_LL285:;}} struct Cyc_Dict_Dict* Cyc_CfFlowInfo_assign_place( struct Cyc_Position_Segment*
loc, struct Cyc_Dict_Dict* d, struct Cyc_Set_Set** all_changed, struct Cyc_CfFlowInfo_Place*
place, void* r){ if( all_changed !=  0){* all_changed=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place* elt)) Cyc_Set_insert)(*
all_changed, place);}{ struct _RegionHandle _temp306= _new_region("rgn"); struct
_RegionHandle* rgn=& _temp306; _push_region( rgn);{ struct Cyc_CfFlowInfo_Place
_temp309; struct Cyc_List_List* _temp310; void* _temp312; struct Cyc_CfFlowInfo_Place*
_temp307= place; _temp309=* _temp307; _LL313: _temp312=( void*) _temp309.root;
goto _LL311; _LL311: _temp310= _temp309.fields; goto _LL308; _LL308: { struct
Cyc_CfFlowInfo_AssignEnv env=({ struct Cyc_CfFlowInfo_AssignEnv _temp315;
_temp315.pile=({ struct Cyc_CfFlowInfo_EscPile* _temp316=( struct Cyc_CfFlowInfo_EscPile*)
_region_malloc( rgn, sizeof( struct Cyc_CfFlowInfo_EscPile)); _temp316->rgn= rgn;
_temp316->places= 0; _temp316;}); _temp315.d= d; _temp315.loc= loc; _temp315;});
void* newval= Cyc_CfFlowInfo_assign_place_outer(& env, _temp310, Cyc_Dict_lookup(
d, _temp312), r); struct Cyc_Dict_Dict* _temp314= Cyc_CfFlowInfo_escape_these(
env.pile, all_changed, Cyc_Dict_insert( d, _temp312, newval)); _npop_handler( 0u);
return _temp314;}}; _pop_region( rgn);}} struct Cyc_CfFlowInfo_JoinEnv{ struct
Cyc_CfFlowInfo_EscPile* pile; struct Cyc_Dict_Dict* d1; struct Cyc_Dict_Dict* d2;
} ; enum  Cyc_CfFlowInfo_WhoIsChanged{ Cyc_CfFlowInfo_Neither  =  0u, Cyc_CfFlowInfo_One
 =  1u, Cyc_CfFlowInfo_Two  =  2u}; struct Cyc_CfFlowInfo_AfterEnv{ struct Cyc_CfFlowInfo_JoinEnv
joinenv; struct Cyc_Set_Set* chg1; struct Cyc_Set_Set* chg2; struct Cyc_CfFlowInfo_Place*
curr_place; struct Cyc_List_List** last_field_cell; enum  Cyc_CfFlowInfo_WhoIsChanged
changed; } ; static void* Cyc_CfFlowInfo_join_absRval( struct Cyc_CfFlowInfo_JoinEnv*
env, void* a, void* r1, void* r2){ if( r1 ==  r2){ return r1;}{ struct _tuple5
_temp318=({ struct _tuple5 _temp317; _temp317.f1= r1; _temp317.f2= r2; _temp317;});
void* _temp334; struct Cyc_CfFlowInfo_Place* _temp336; void* _temp338; struct
Cyc_CfFlowInfo_Place* _temp340; void* _temp342; struct Cyc_CfFlowInfo_Place*
_temp344; void* _temp346; struct Cyc_CfFlowInfo_Place* _temp348; void* _temp350;
void* _temp352; void* _temp354; void* _temp356; void* _temp358; struct Cyc_Dict_Dict*
_temp360; void* _temp362; struct Cyc_Dict_Dict* _temp364; _LL320: _LL339:
_temp338= _temp318.f1; if(( unsigned int) _temp338 >  3u?*(( int*) _temp338) == 
Cyc_CfFlowInfo_AddressOf: 0){ _LL341: _temp340=(( struct Cyc_CfFlowInfo_AddressOf_struct*)
_temp338)->f1; goto _LL335;} else{ goto _LL322;} _LL335: _temp334= _temp318.f2;
if(( unsigned int) _temp334 >  3u?*(( int*) _temp334) ==  Cyc_CfFlowInfo_AddressOf:
0){ _LL337: _temp336=(( struct Cyc_CfFlowInfo_AddressOf_struct*) _temp334)->f1;
goto _LL321;} else{ goto _LL322;} _LL322: _LL343: _temp342= _temp318.f1; if((
unsigned int) _temp342 >  3u?*(( int*) _temp342) ==  Cyc_CfFlowInfo_AddressOf: 0){
_LL345: _temp344=(( struct Cyc_CfFlowInfo_AddressOf_struct*) _temp342)->f1; goto
_LL323;} else{ goto _LL324;} _LL324: _LL347: _temp346= _temp318.f2; if((
unsigned int) _temp346 >  3u?*(( int*) _temp346) ==  Cyc_CfFlowInfo_AddressOf: 0){
_LL349: _temp348=(( struct Cyc_CfFlowInfo_AddressOf_struct*) _temp346)->f1; goto
_LL325;} else{ goto _LL326;} _LL326: _LL353: _temp352= _temp318.f1; if( _temp352
== ( void*) Cyc_CfFlowInfo_NotZeroAll){ goto _LL351;} else{ goto _LL328;} _LL351:
_temp350= _temp318.f2; if( _temp350 == ( void*) Cyc_CfFlowInfo_NotZeroThis){
goto _LL327;} else{ goto _LL328;} _LL328: _LL357: _temp356= _temp318.f1; if(
_temp356 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto _LL355;} else{ goto
_LL330;} _LL355: _temp354= _temp318.f2; if( _temp354 == ( void*) Cyc_CfFlowInfo_NotZeroAll){
goto _LL329;} else{ goto _LL330;} _LL330: _LL363: _temp362= _temp318.f1; if((
unsigned int) _temp362 >  3u?*(( int*) _temp362) ==  Cyc_CfFlowInfo_Aggregate: 0){
_LL365: _temp364=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp362)->f1; goto
_LL359;} else{ goto _LL332;} _LL359: _temp358= _temp318.f2; if(( unsigned int)
_temp358 >  3u?*(( int*) _temp358) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL361:
_temp360=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp358)->f1; goto _LL331;}
else{ goto _LL332;} _LL332: goto _LL333; _LL321: if( Cyc_CfFlowInfo_place_cmp(
_temp340, _temp336) ==  0){ return r1;} Cyc_CfFlowInfo_add_place( env->pile,
_temp340); Cyc_CfFlowInfo_add_place( env->pile, _temp336); goto _LL319; _LL323:
Cyc_CfFlowInfo_add_place( env->pile, _temp344); goto _LL319; _LL325: Cyc_CfFlowInfo_add_place(
env->pile, _temp348); goto _LL319; _LL327: goto _LL329; _LL329: return( void*)
Cyc_CfFlowInfo_NotZeroThis; _LL331: { struct Cyc_Dict_Dict* _temp366=(( struct
Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*, struct
_tagged_arr*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_union_two_c)(( void*(*)( struct Cyc_CfFlowInfo_JoinEnv*
env, struct _tagged_arr* a, void* r1, void* r2)) Cyc_CfFlowInfo_join_absRval,
env, _temp364, _temp360); if( _temp366 ==  _temp364){ return r1;} if( _temp366
==  _temp360){ return r2;} return( void*)({ struct Cyc_CfFlowInfo_Aggregate_struct*
_temp367=( struct Cyc_CfFlowInfo_Aggregate_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_Aggregate_struct)); _temp367[ 0]=({ struct Cyc_CfFlowInfo_Aggregate_struct
_temp368; _temp368.tag= Cyc_CfFlowInfo_Aggregate; _temp368.f1= _temp366;
_temp368;}); _temp367;});} _LL333: goto _LL319; _LL319:;}{ void* il1= Cyc_CfFlowInfo_initlevel(
env->d1, r1); void* il2= Cyc_CfFlowInfo_initlevel( env->d2, r2); struct _tuple5
_temp370=({ struct _tuple5 _temp369; _temp369.f1= r1; _temp369.f2= r2; _temp369;});
void* _temp378; void* _temp380; _LL372: _LL379: _temp378= _temp370.f1; if((
unsigned int) _temp378 >  3u?*(( int*) _temp378) ==  Cyc_CfFlowInfo_Esc: 0){
goto _LL373;} else{ goto _LL374;} _LL374: _LL381: _temp380= _temp370.f2; if((
unsigned int) _temp380 >  3u?*(( int*) _temp380) ==  Cyc_CfFlowInfo_Esc: 0){
goto _LL375;} else{ goto _LL376;} _LL376: goto _LL377; _LL373: goto _LL375;
_LL375: { struct _tuple5 _temp383=({ struct _tuple5 _temp382; _temp382.f1= il1;
_temp382.f2= il2; _temp382;}); void* _temp395; void* _temp397; void* _temp399;
void* _temp401; _LL385: _LL396: _temp395= _temp383.f2; if( _temp395 == ( void*)
Cyc_CfFlowInfo_NoneIL){ goto _LL386;} else{ goto _LL387;} _LL387: _LL398:
_temp397= _temp383.f1; if( _temp397 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto
_LL388;} else{ goto _LL389;} _LL389: _LL400: _temp399= _temp383.f2; if( _temp399
== ( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL390;} else{ goto _LL391;} _LL391:
_LL402: _temp401= _temp383.f1; if( _temp401 == ( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL392;} else{ goto _LL393;} _LL393: goto _LL394; _LL386: goto _LL388;
_LL388: return Cyc_CfFlowInfo_esc_none; _LL390: goto _LL392; _LL392: return Cyc_CfFlowInfo_esc_this;
_LL394: return Cyc_CfFlowInfo_esc_all; _LL384:;} _LL377: { struct _tuple5
_temp404=({ struct _tuple5 _temp403; _temp403.f1= il1; _temp403.f2= il2;
_temp403;}); void* _temp416; void* _temp418; void* _temp420; void* _temp422;
_LL406: _LL417: _temp416= _temp404.f2; if( _temp416 == ( void*) Cyc_CfFlowInfo_NoneIL){
goto _LL407;} else{ goto _LL408;} _LL408: _LL419: _temp418= _temp404.f1; if(
_temp418 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL409;} else{ goto _LL410;}
_LL410: _LL421: _temp420= _temp404.f2; if( _temp420 == ( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL411;} else{ goto _LL412;} _LL412: _LL423: _temp422= _temp404.f1; if(
_temp422 == ( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL413;} else{ goto _LL414;}
_LL414: goto _LL415; _LL407: goto _LL409; _LL409: return Cyc_CfFlowInfo_unknown_none;
_LL411: goto _LL413; _LL413: return Cyc_CfFlowInfo_unknown_this; _LL415: return
Cyc_CfFlowInfo_unknown_all; _LL405:;} _LL371:;}} int Cyc_CfFlowInfo_same_relop(
void* r1, void* r2){ if( r1 ==  r2){ return 1;}{ struct _tuple5 _temp425=({
struct _tuple5 _temp424; _temp424.f1= r1; _temp424.f2= r2; _temp424;}); void*
_temp439; unsigned int _temp441; void* _temp443; unsigned int _temp445; void*
_temp447; struct Cyc_Absyn_Vardecl* _temp449; void* _temp451; struct Cyc_Absyn_Vardecl*
_temp453; void* _temp455; struct Cyc_Absyn_Vardecl* _temp457; void* _temp459;
struct Cyc_Absyn_Vardecl* _temp461; void* _temp463; unsigned int _temp465; void*
_temp467; unsigned int _temp469; void* _temp471; struct Cyc_Absyn_Vardecl*
_temp473; void* _temp475; struct Cyc_Absyn_Vardecl* _temp477; _LL427: _LL444:
_temp443= _temp425.f1; if(*(( int*) _temp443) ==  Cyc_CfFlowInfo_EqualConst){
_LL446: _temp445=(( struct Cyc_CfFlowInfo_EqualConst_struct*) _temp443)->f1;
goto _LL440;} else{ goto _LL429;} _LL440: _temp439= _temp425.f2; if(*(( int*)
_temp439) ==  Cyc_CfFlowInfo_EqualConst){ _LL442: _temp441=(( struct Cyc_CfFlowInfo_EqualConst_struct*)
_temp439)->f1; goto _LL428;} else{ goto _LL429;} _LL429: _LL452: _temp451=
_temp425.f1; if(*(( int*) _temp451) ==  Cyc_CfFlowInfo_LessVar){ _LL454:
_temp453=(( struct Cyc_CfFlowInfo_LessVar_struct*) _temp451)->f1; goto _LL448;}
else{ goto _LL431;} _LL448: _temp447= _temp425.f2; if(*(( int*) _temp447) == 
Cyc_CfFlowInfo_LessVar){ _LL450: _temp449=(( struct Cyc_CfFlowInfo_LessVar_struct*)
_temp447)->f1; goto _LL430;} else{ goto _LL431;} _LL431: _LL460: _temp459=
_temp425.f1; if(*(( int*) _temp459) ==  Cyc_CfFlowInfo_LessSize){ _LL462:
_temp461=(( struct Cyc_CfFlowInfo_LessSize_struct*) _temp459)->f1; goto _LL456;}
else{ goto _LL433;} _LL456: _temp455= _temp425.f2; if(*(( int*) _temp455) == 
Cyc_CfFlowInfo_LessSize){ _LL458: _temp457=(( struct Cyc_CfFlowInfo_LessSize_struct*)
_temp455)->f1; goto _LL432;} else{ goto _LL433;} _LL433: _LL468: _temp467=
_temp425.f1; if(*(( int*) _temp467) ==  Cyc_CfFlowInfo_LessConst){ _LL470:
_temp469=(( struct Cyc_CfFlowInfo_LessConst_struct*) _temp467)->f1; goto _LL464;}
else{ goto _LL435;} _LL464: _temp463= _temp425.f2; if(*(( int*) _temp463) == 
Cyc_CfFlowInfo_LessConst){ _LL466: _temp465=(( struct Cyc_CfFlowInfo_LessConst_struct*)
_temp463)->f1; goto _LL434;} else{ goto _LL435;} _LL435: _LL476: _temp475=
_temp425.f1; if(*(( int*) _temp475) ==  Cyc_CfFlowInfo_LessEqSize){ _LL478:
_temp477=(( struct Cyc_CfFlowInfo_LessEqSize_struct*) _temp475)->f1; goto _LL472;}
else{ goto _LL437;} _LL472: _temp471= _temp425.f2; if(*(( int*) _temp471) == 
Cyc_CfFlowInfo_LessEqSize){ _LL474: _temp473=(( struct Cyc_CfFlowInfo_LessEqSize_struct*)
_temp471)->f1; goto _LL436;} else{ goto _LL437;} _LL437: goto _LL438; _LL428:
return _temp445 ==  _temp441; _LL430: return _temp453 ==  _temp449; _LL432:
return _temp461 ==  _temp457; _LL434: return _temp469 ==  _temp465; _LL436:
return _temp477 ==  _temp473; _LL438: return 0; _LL426:;}} struct Cyc_List_List*
Cyc_CfFlowInfo_join_relns( struct Cyc_List_List* r1s, struct Cyc_List_List* r2s){
if( r1s ==  r2s){ return r1s;}{ struct Cyc_List_List* res= 0; int diff= 0;{
struct Cyc_List_List* _temp479= r1s; for( 0; _temp479 !=  0; _temp479= _temp479->tl){
struct Cyc_CfFlowInfo_Reln* _temp480=( struct Cyc_CfFlowInfo_Reln*) _temp479->hd;
int found= 0;{ struct Cyc_List_List* _temp481= r2s; for( 0; _temp481 !=  0;
_temp481= _temp481->tl){ struct Cyc_CfFlowInfo_Reln* _temp482=( struct Cyc_CfFlowInfo_Reln*)
_temp481->hd; if( _temp480 ==  _temp482? 1:( _temp480->vd ==  _temp482->vd? Cyc_CfFlowInfo_same_relop((
void*) _temp480->rop,( void*) _temp482->rop): 0)){ res=({ struct Cyc_List_List*
_temp483=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp483->hd=( void*) _temp480; _temp483->tl= res; _temp483;}); found= 1; break;}}}
if( ! found){ diff= 1;}}} if( ! diff){ res= r1s;} return res;}} void* Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set** all_changed, void* f1, void* f2){ if( f1 ==  f2){ return f1;}{
struct _tuple5 _temp485=({ struct _tuple5 _temp484; _temp484.f1= f1; _temp484.f2=
f2; _temp484;}); void* _temp493; void* _temp495; void* _temp497; struct Cyc_List_List*
_temp499; struct Cyc_Dict_Dict* _temp501; void* _temp503; struct Cyc_List_List*
_temp505; struct Cyc_Dict_Dict* _temp507; _LL487: _LL494: _temp493= _temp485.f1;
if( _temp493 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL488;} else{ goto
_LL489;} _LL489: _LL496: _temp495= _temp485.f2; if( _temp495 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL490;} else{ goto _LL491;} _LL491: _LL504: _temp503= _temp485.f1; if((
unsigned int) _temp503 >  1u?*(( int*) _temp503) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL508: _temp507=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp503)->f1;
goto _LL506; _LL506: _temp505=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp503)->f2; goto _LL498;} else{ goto _LL486;} _LL498: _temp497= _temp485.f2;
if(( unsigned int) _temp497 >  1u?*(( int*) _temp497) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL502: _temp501=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp497)->f1;
goto _LL500; _LL500: _temp499=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp497)->f2; goto _LL492;} else{ goto _LL486;} _LL488: return f2; _LL490:
return f1; _LL492: if( _temp507 ==  _temp501? _temp505 ==  _temp499: 0){ return
f1;} if( Cyc_CfFlowInfo_flow_lessthan_approx( f1, f2)){ return f2;} if( Cyc_CfFlowInfo_flow_lessthan_approx(
f2, f1)){ return f1;}{ struct _RegionHandle _temp509= _new_region("rgn"); struct
_RegionHandle* rgn=& _temp509; _push_region( rgn);{ struct Cyc_CfFlowInfo_JoinEnv
_temp510=({ struct Cyc_CfFlowInfo_JoinEnv _temp516; _temp516.pile=({ struct Cyc_CfFlowInfo_EscPile*
_temp517=( struct Cyc_CfFlowInfo_EscPile*) _region_malloc( rgn, sizeof( struct
Cyc_CfFlowInfo_EscPile)); _temp517->rgn= rgn; _temp517->places= 0; _temp517;});
_temp516.d1= _temp507; _temp516.d2= _temp501; _temp516;}); struct Cyc_Dict_Dict*
_temp511=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct Cyc_CfFlowInfo_JoinEnv*,
void*, void*, void*), struct Cyc_CfFlowInfo_JoinEnv* env, struct Cyc_Dict_Dict*
d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_intersect_c)( Cyc_CfFlowInfo_join_absRval,&
_temp510, _temp507, _temp501); struct Cyc_List_List* _temp512= Cyc_CfFlowInfo_join_relns(
_temp505, _temp499); void* _temp515=( void*)({ struct Cyc_CfFlowInfo_ReachableFL_struct*
_temp513=( struct Cyc_CfFlowInfo_ReachableFL_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_ReachableFL_struct)); _temp513[ 0]=({ struct Cyc_CfFlowInfo_ReachableFL_struct
_temp514; _temp514.tag= Cyc_CfFlowInfo_ReachableFL; _temp514.f1= Cyc_CfFlowInfo_escape_these(
_temp510.pile, all_changed, _temp511); _temp514.f2= _temp512; _temp514;});
_temp513;}); _npop_handler( 0u); return _temp515;}; _pop_region( rgn);} _LL486:;}}
static void* Cyc_CfFlowInfo_after_absRval_d( struct Cyc_CfFlowInfo_AfterEnv*,
struct _tagged_arr*, void*, void*); static void* Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv* env, void* r1, void* r2){ int changed1= env->changed
==  Cyc_CfFlowInfo_One? 1:(( int(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place*
elt)) Cyc_Set_member)( env->chg1, env->curr_place); int changed2= env->changed
==  Cyc_CfFlowInfo_Two? 1:(( int(*)( struct Cyc_Set_Set* s, struct Cyc_CfFlowInfo_Place*
elt)) Cyc_Set_member)( env->chg2, env->curr_place); if( changed1? changed2: 0){
return(( void*(*)( struct Cyc_CfFlowInfo_JoinEnv* env, int a, void* r1, void* r2))
Cyc_CfFlowInfo_join_absRval)(& env->joinenv, 0, r1, r2);} if( changed1){ if( !
Cyc_CfFlowInfo_prefix_of_member( env->curr_place, env->chg2)){ return r1;} env->changed=
Cyc_CfFlowInfo_One;} if( changed2){ if( ! Cyc_CfFlowInfo_prefix_of_member( env->curr_place,
env->chg1)){ return r2;} env->changed= Cyc_CfFlowInfo_Two;}{ struct _tuple5
_temp519=({ struct _tuple5 _temp518; _temp518.f1= r1; _temp518.f2= r2; _temp518;});
void* _temp525; struct Cyc_Dict_Dict* _temp527; void* _temp529; struct Cyc_Dict_Dict*
_temp531; _LL521: _LL530: _temp529= _temp519.f1; if(( unsigned int) _temp529 > 
3u?*(( int*) _temp529) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL532: _temp531=((
struct Cyc_CfFlowInfo_Aggregate_struct*) _temp529)->f1; goto _LL526;} else{ goto
_LL523;} _LL526: _temp525= _temp519.f2; if(( unsigned int) _temp525 >  3u?*((
int*) _temp525) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL528: _temp527=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp525)->f1; goto _LL522;} else{ goto _LL523;} _LL523: goto _LL524; _LL522: {
struct Cyc_Dict_Dict* _temp533=(( struct Cyc_Dict_Dict*(*)( void*(* f)( struct
Cyc_CfFlowInfo_AfterEnv*, struct _tagged_arr*, void*, void*), struct Cyc_CfFlowInfo_AfterEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_union_two_c)(
Cyc_CfFlowInfo_after_absRval_d, env, _temp531, _temp527); if( _temp533 == 
_temp531){ return r1;} if( _temp533 ==  _temp527){ return r2;} return( void*)({
struct Cyc_CfFlowInfo_Aggregate_struct* _temp534=( struct Cyc_CfFlowInfo_Aggregate_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_Aggregate_struct)); _temp534[ 0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _temp535; _temp535.tag= Cyc_CfFlowInfo_Aggregate;
_temp535.f1= _temp533; _temp535;}); _temp534;});} _LL524:( int) _throw(( void*)({
struct Cyc_Core_Impossible_struct* _temp536=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp536[ 0]=({ struct
Cyc_Core_Impossible_struct _temp537; _temp537.tag= Cyc_Core_Impossible; _temp537.f1=
_tag_arr("after_pathinfo -- non-aggregates!", sizeof( unsigned char), 34u);
_temp537;}); _temp536;})); _LL520:;}} static void* Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv* env, struct _tagged_arr* key, void* r1, void* r2){
if( r1 ==  r2){ return r1;}{ struct Cyc_List_List** _temp538= env->last_field_cell;
enum  Cyc_CfFlowInfo_WhoIsChanged _temp539= env->changed;* env->last_field_cell=({
struct Cyc_List_List* _temp540=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp540->hd=( void*) key; _temp540->tl= 0; _temp540;});
env->last_field_cell=&(( struct Cyc_List_List*) _check_null(* env->last_field_cell))->tl;{
void* _temp541= Cyc_CfFlowInfo_after_absRval( env, r1, r2); env->last_field_cell=
_temp538;(( struct Cyc_List_List*) _check_null(* env->last_field_cell))->tl= 0;
env->changed= _temp539; return _temp541;}}} static void* Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv* env, void* root, void* r1, void* r2){ if( r1 == 
r2){ return r1;}* env->curr_place=({ struct Cyc_CfFlowInfo_Place _temp542;
_temp542.root=( void*) root; _temp542.fields= 0; _temp542;}); env->last_field_cell=&(
env->curr_place)->fields; env->changed= Cyc_CfFlowInfo_Neither; return Cyc_CfFlowInfo_after_absRval(
env, r1, r2);} void* Cyc_CfFlowInfo_after_flow( struct Cyc_Set_Set** all_changed,
void* f1, void* f2, struct Cyc_Set_Set* chg1, struct Cyc_Set_Set* chg2){ static
struct Cyc_Absyn_Const_e_struct dummy_rawexp={ 0u,( void*)(( void*) 0u)}; static
struct Cyc_Absyn_Exp dummy_exp={ 0,( void*)(( void*)& dummy_rawexp), 0,( void*)((
void*) Cyc_Absyn_EmptyAnnot)}; static struct Cyc_CfFlowInfo_MallocPt_struct
dummy_root={ 1u,& dummy_exp}; if( f1 ==  f2){ return f1;}{ struct _tuple5
_temp544=({ struct _tuple5 _temp543; _temp543.f1= f1; _temp543.f2= f2; _temp543;});
void* _temp552; void* _temp554; void* _temp556; struct Cyc_List_List* _temp558;
struct Cyc_Dict_Dict* _temp560; void* _temp562; struct Cyc_List_List* _temp564;
struct Cyc_Dict_Dict* _temp566; _LL546: _LL553: _temp552= _temp544.f1; if(
_temp552 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL547;} else{ goto _LL548;}
_LL548: _LL555: _temp554= _temp544.f2; if( _temp554 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL549;} else{ goto _LL550;} _LL550: _LL563: _temp562= _temp544.f1; if((
unsigned int) _temp562 >  1u?*(( int*) _temp562) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL567: _temp566=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp562)->f1;
goto _LL565; _LL565: _temp564=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp562)->f2; goto _LL557;} else{ goto _LL545;} _LL557: _temp556= _temp544.f2;
if(( unsigned int) _temp556 >  1u?*(( int*) _temp556) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL561: _temp560=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp556)->f1;
goto _LL559; _LL559: _temp558=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp556)->f2; goto _LL551;} else{ goto _LL545;} _LL547: goto _LL549; _LL549:
return( void*) Cyc_CfFlowInfo_BottomFL; _LL551: if( _temp566 ==  _temp560?
_temp564 ==  _temp558: 0){ return f1;}{ struct _RegionHandle _temp568=
_new_region("rgn"); struct _RegionHandle* rgn=& _temp568; _push_region( rgn);{
struct Cyc_CfFlowInfo_Place* _temp569=({ struct Cyc_CfFlowInfo_Place* _temp579=(
struct Cyc_CfFlowInfo_Place*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Place));
_temp579->root=( void*)(( void*)& dummy_root); _temp579->fields= 0; _temp579;});
struct Cyc_CfFlowInfo_AfterEnv _temp570=({ struct Cyc_CfFlowInfo_AfterEnv
_temp576; _temp576.joinenv=({ struct Cyc_CfFlowInfo_JoinEnv _temp577; _temp577.pile=({
struct Cyc_CfFlowInfo_EscPile* _temp578=( struct Cyc_CfFlowInfo_EscPile*)
_region_malloc( rgn, sizeof( struct Cyc_CfFlowInfo_EscPile)); _temp578->rgn= rgn;
_temp578->places= 0; _temp578;}); _temp577.d1= _temp566; _temp577.d2= _temp560;
_temp577;}); _temp576.chg1= chg1; _temp576.chg2= chg2; _temp576.curr_place=
_temp569; _temp576.last_field_cell=& _temp569->fields; _temp576.changed= Cyc_CfFlowInfo_Neither;
_temp576;}); struct Cyc_Dict_Dict* _temp571=(( struct Cyc_Dict_Dict*(*)( void*(*
f)( struct Cyc_CfFlowInfo_AfterEnv*, void*, void*, void*), struct Cyc_CfFlowInfo_AfterEnv*
env, struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict* d2)) Cyc_Dict_union_two_c)(
Cyc_CfFlowInfo_after_root,& _temp570, _temp566, _temp560); struct Cyc_List_List*
_temp572= Cyc_CfFlowInfo_join_relns( _temp564, _temp558); void* _temp575=( void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct* _temp573=( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_cycalloc( sizeof( struct Cyc_CfFlowInfo_ReachableFL_struct)); _temp573[ 0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _temp574; _temp574.tag= Cyc_CfFlowInfo_ReachableFL;
_temp574.f1= Cyc_CfFlowInfo_escape_these(( _temp570.joinenv).pile, all_changed,
_temp571); _temp574.f2= _temp572; _temp574;}); _temp573;}); _npop_handler( 0u);
return _temp575;}; _pop_region( rgn);} _LL545:;}} static int Cyc_CfFlowInfo_absRval_lessthan_approx(
void* ignore, void* r1, void* r2){ if( r1 ==  r2){ return 1;}{ struct _tuple5
_temp583=({ struct _tuple5 _temp582; _temp582.f1= r1; _temp582.f2= r2; _temp582;});
void* _temp605; struct Cyc_CfFlowInfo_Place* _temp607; void* _temp609; struct
Cyc_CfFlowInfo_Place* _temp611; void* _temp613; void* _temp615; void* _temp617;
struct Cyc_Dict_Dict* _temp619; void* _temp621; struct Cyc_Dict_Dict* _temp623;
void* _temp625; void* _temp627; void* _temp629; void* _temp631; void* _temp633;
void* _temp635; _LL585: _LL610: _temp609= _temp583.f1; if(( unsigned int)
_temp609 >  3u?*(( int*) _temp609) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL612:
_temp611=(( struct Cyc_CfFlowInfo_AddressOf_struct*) _temp609)->f1; goto _LL606;}
else{ goto _LL587;} _LL606: _temp605= _temp583.f2; if(( unsigned int) _temp605 > 
3u?*(( int*) _temp605) ==  Cyc_CfFlowInfo_AddressOf: 0){ _LL608: _temp607=((
struct Cyc_CfFlowInfo_AddressOf_struct*) _temp605)->f1; goto _LL586;} else{ goto
_LL587;} _LL587: _LL614: _temp613= _temp583.f1; if(( unsigned int) _temp613 >  3u?*((
int*) _temp613) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL588;} else{ goto
_LL589;} _LL589: _LL616: _temp615= _temp583.f2; if(( unsigned int) _temp615 >  3u?*((
int*) _temp615) ==  Cyc_CfFlowInfo_AddressOf: 0){ goto _LL590;} else{ goto
_LL591;} _LL591: _LL622: _temp621= _temp583.f1; if(( unsigned int) _temp621 >  3u?*((
int*) _temp621) ==  Cyc_CfFlowInfo_Aggregate: 0){ _LL624: _temp623=(( struct Cyc_CfFlowInfo_Aggregate_struct*)
_temp621)->f1; goto _LL618;} else{ goto _LL593;} _LL618: _temp617= _temp583.f2;
if(( unsigned int) _temp617 >  3u?*(( int*) _temp617) ==  Cyc_CfFlowInfo_Aggregate:
0){ _LL620: _temp619=(( struct Cyc_CfFlowInfo_Aggregate_struct*) _temp617)->f1;
goto _LL592;} else{ goto _LL593;} _LL593: _LL626: _temp625= _temp583.f2; if(
_temp625 == ( void*) Cyc_CfFlowInfo_NotZeroThis){ goto _LL594;} else{ goto
_LL595;} _LL595: _LL628: _temp627= _temp583.f2; if( _temp627 == ( void*) Cyc_CfFlowInfo_Zero){
goto _LL596;} else{ goto _LL597;} _LL597: _LL630: _temp629= _temp583.f2; if(
_temp629 == ( void*) Cyc_CfFlowInfo_NotZeroAll){ goto _LL598;} else{ goto _LL599;}
_LL599: _LL634: _temp633= _temp583.f1; if(( unsigned int) _temp633 >  3u?*(( int*)
_temp633) ==  Cyc_CfFlowInfo_Esc: 0){ goto _LL632;} else{ goto _LL601;} _LL632:
_temp631= _temp583.f2; if(( unsigned int) _temp631 >  3u?*(( int*) _temp631) == 
Cyc_CfFlowInfo_Esc: 0){ goto _LL600;} else{ goto _LL601;} _LL601: _LL636:
_temp635= _temp583.f1; if(( unsigned int) _temp635 >  3u?*(( int*) _temp635) == 
Cyc_CfFlowInfo_Esc: 0){ goto _LL602;} else{ goto _LL603;} _LL603: goto _LL604;
_LL586: return Cyc_CfFlowInfo_place_cmp( _temp611, _temp607) ==  0; _LL588: goto
_LL590; _LL590: return 0; _LL592: return _temp623 ==  _temp619? 1:(( int(*)( int(*
f)( struct _tagged_arr*, void*, void*), struct Cyc_Dict_Dict* d1, struct Cyc_Dict_Dict*
d2)) Cyc_Dict_forall_intersect)(( int(*)( struct _tagged_arr* ignore, void* r1,
void* r2)) Cyc_CfFlowInfo_absRval_lessthan_approx, _temp623, _temp619); _LL594:
return r1 == ( void*) Cyc_CfFlowInfo_NotZeroAll; _LL596: goto _LL598; _LL598:
return 0; _LL600: goto _LL584; _LL602: return 0; _LL604: goto _LL584; _LL584:;}{
struct _tuple5 _temp638=({ struct _tuple5 _temp637; _temp637.f1= Cyc_CfFlowInfo_initlevel_approx(
r1); _temp637.f2= Cyc_CfFlowInfo_initlevel_approx( r2); _temp637;}); void*
_temp650; void* _temp652; void* _temp654; void* _temp656; void* _temp658; void*
_temp660; _LL640: _LL653: _temp652= _temp638.f1; if( _temp652 == ( void*) Cyc_CfFlowInfo_AllIL){
goto _LL651;} else{ goto _LL642;} _LL651: _temp650= _temp638.f2; if( _temp650 == (
void*) Cyc_CfFlowInfo_AllIL){ goto _LL641;} else{ goto _LL642;} _LL642: _LL655:
_temp654= _temp638.f2; if( _temp654 == ( void*) Cyc_CfFlowInfo_NoneIL){ goto
_LL643;} else{ goto _LL644;} _LL644: _LL657: _temp656= _temp638.f1; if( _temp656
== ( void*) Cyc_CfFlowInfo_NoneIL){ goto _LL645;} else{ goto _LL646;} _LL646:
_LL659: _temp658= _temp638.f2; if( _temp658 == ( void*) Cyc_CfFlowInfo_ThisIL){
goto _LL647;} else{ goto _LL648;} _LL648: _LL661: _temp660= _temp638.f1; if(
_temp660 == ( void*) Cyc_CfFlowInfo_ThisIL){ goto _LL649;} else{ goto _LL639;}
_LL641: return 1; _LL643: return 1; _LL645: return 0; _LL647: return 1; _LL649:
return 0; _LL639:;}} int Cyc_CfFlowInfo_relns_approx( struct Cyc_List_List* r2s,
struct Cyc_List_List* r1s){ if( r1s ==  r2s){ return 1;} for( 0; r1s !=  0; r1s=
r1s->tl){ struct Cyc_CfFlowInfo_Reln* _temp662=( struct Cyc_CfFlowInfo_Reln*)
r1s->hd; int found= 0;{ struct Cyc_List_List* _temp663= r2s; for( 0; _temp663 != 
0; _temp663= _temp663->tl){ struct Cyc_CfFlowInfo_Reln* _temp664=( struct Cyc_CfFlowInfo_Reln*)
_temp663->hd; if( _temp662 ==  _temp664? 1:( _temp662->vd ==  _temp664->vd? Cyc_CfFlowInfo_same_relop((
void*) _temp662->rop,( void*) _temp664->rop): 0)){ found= 1; break;}}} if( !
found){ return 0;}} return 1;} int Cyc_CfFlowInfo_flow_lessthan_approx( void* f1,
void* f2){ if( f1 ==  f2){ return 1;}{ struct _tuple5 _temp666=({ struct _tuple5
_temp665; _temp665.f1= f1; _temp665.f2= f2; _temp665;}); void* _temp674; void*
_temp676; void* _temp678; struct Cyc_List_List* _temp680; struct Cyc_Dict_Dict*
_temp682; void* _temp684; struct Cyc_List_List* _temp686; struct Cyc_Dict_Dict*
_temp688; _LL668: _LL675: _temp674= _temp666.f1; if( _temp674 == ( void*) Cyc_CfFlowInfo_BottomFL){
goto _LL669;} else{ goto _LL670;} _LL670: _LL677: _temp676= _temp666.f2; if(
_temp676 == ( void*) Cyc_CfFlowInfo_BottomFL){ goto _LL671;} else{ goto _LL672;}
_LL672: _LL685: _temp684= _temp666.f1; if(( unsigned int) _temp684 >  1u?*(( int*)
_temp684) ==  Cyc_CfFlowInfo_ReachableFL: 0){ _LL689: _temp688=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp684)->f1; goto _LL687; _LL687: _temp686=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp684)->f2; goto _LL679;} else{ goto _LL667;} _LL679: _temp678= _temp666.f2;
if(( unsigned int) _temp678 >  1u?*(( int*) _temp678) ==  Cyc_CfFlowInfo_ReachableFL:
0){ _LL683: _temp682=(( struct Cyc_CfFlowInfo_ReachableFL_struct*) _temp678)->f1;
goto _LL681; _LL681: _temp680=(( struct Cyc_CfFlowInfo_ReachableFL_struct*)
_temp678)->f2; goto _LL673;} else{ goto _LL667;} _LL669: return 1; _LL671:
return 0; _LL673: if( _temp688 ==  _temp682? _temp686 ==  _temp680: 0){ return 1;}
return Cyc_Dict_forall_intersect( Cyc_CfFlowInfo_absRval_lessthan_approx,
_temp688, _temp682)? Cyc_CfFlowInfo_relns_approx( _temp686, _temp680): 0; _LL667:;}}
struct Cyc_List_List* Cyc_CfFlowInfo_reln_kill_var( struct Cyc_List_List* rs,
struct Cyc_Absyn_Vardecl* v){ struct Cyc_List_List* p; int found= 0; for( p= rs;
! found? p !=  0: 0; p= p->tl){ struct Cyc_CfFlowInfo_Reln* _temp690=( struct
Cyc_CfFlowInfo_Reln*) p->hd; if( _temp690->vd ==  v){ found= 1; break;}{ void*
_temp691=( void*) _temp690->rop; struct Cyc_Absyn_Vardecl* _temp701; struct Cyc_Absyn_Vardecl*
_temp703; struct Cyc_Absyn_Vardecl* _temp705; _LL693: if(*(( int*) _temp691) == 
Cyc_CfFlowInfo_LessVar){ _LL702: _temp701=(( struct Cyc_CfFlowInfo_LessVar_struct*)
_temp691)->f1; goto _LL694;} else{ goto _LL695;} _LL695: if(*(( int*) _temp691)
==  Cyc_CfFlowInfo_LessSize){ _LL704: _temp703=(( struct Cyc_CfFlowInfo_LessSize_struct*)
_temp691)->f1; goto _LL696;} else{ goto _LL697;} _LL697: if(*(( int*) _temp691)
==  Cyc_CfFlowInfo_LessEqSize){ _LL706: _temp705=(( struct Cyc_CfFlowInfo_LessEqSize_struct*)
_temp691)->f1; goto _LL698;} else{ goto _LL699;} _LL699: goto _LL700; _LL694:
_temp703= _temp701; goto _LL696; _LL696: _temp705= _temp703; goto _LL698; _LL698:
found= 1; continue; _LL700: goto _LL692; _LL692:;}} if( ! found){ return rs;}{
struct Cyc_List_List* _temp707= 0; for( p= rs; p !=  0; p= p->tl){ struct Cyc_CfFlowInfo_Reln*
_temp708=( struct Cyc_CfFlowInfo_Reln*) p->hd; if( _temp708->vd !=  v){{ void*
_temp709=( void*) _temp708->rop; struct Cyc_Absyn_Vardecl* _temp719; struct Cyc_Absyn_Vardecl*
_temp721; struct Cyc_Absyn_Vardecl* _temp723; _LL711: if(*(( int*) _temp709) == 
Cyc_CfFlowInfo_LessVar){ _LL720: _temp719=(( struct Cyc_CfFlowInfo_LessVar_struct*)
_temp709)->f1; goto _LL712;} else{ goto _LL713;} _LL713: if(*(( int*) _temp709)
==  Cyc_CfFlowInfo_LessSize){ _LL722: _temp721=(( struct Cyc_CfFlowInfo_LessSize_struct*)
_temp709)->f1; goto _LL714;} else{ goto _LL715;} _LL715: if(*(( int*) _temp709)
==  Cyc_CfFlowInfo_LessEqSize){ _LL724: _temp723=(( struct Cyc_CfFlowInfo_LessEqSize_struct*)
_temp709)->f1; goto _LL716;} else{ goto _LL717;} _LL717: goto _LL718; _LL712:
_temp721= _temp719; goto _LL714; _LL714: _temp723= _temp721; goto _LL716; _LL716:
if( v ==  _temp723){ continue;} else{ goto _LL710;} _LL718: goto _LL710;; _LL710:;}
_temp707=({ struct Cyc_List_List* _temp725=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp725->hd=( void*) _temp708; _temp725->tl=
_temp707; _temp725;});}} return _temp707;}} struct Cyc_List_List* Cyc_CfFlowInfo_reln_kill_exp(
struct Cyc_List_List* r, struct Cyc_Absyn_Exp* e){{ void* _temp726=( void*) e->r;
void* _temp738; struct Cyc_Absyn_Vardecl* _temp740; void* _temp742; struct Cyc_Absyn_Vardecl*
_temp744; void* _temp746; struct Cyc_Absyn_Vardecl* _temp748; void* _temp750;
struct Cyc_Absyn_Vardecl* _temp752; _LL728: if(*(( int*) _temp726) ==  Cyc_Absyn_Var_e){
_LL739: _temp738=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp726)->f2; if((
unsigned int) _temp738 >  1u?*(( int*) _temp738) ==  Cyc_Absyn_Global_b: 0){
_LL741: _temp740=(( struct Cyc_Absyn_Global_b_struct*) _temp738)->f1; goto
_LL729;} else{ goto _LL730;}} else{ goto _LL730;} _LL730: if(*(( int*) _temp726)
==  Cyc_Absyn_Var_e){ _LL743: _temp742=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp726)->f2; if(( unsigned int) _temp742 >  1u?*(( int*) _temp742) ==  Cyc_Absyn_Param_b:
0){ _LL745: _temp744=(( struct Cyc_Absyn_Param_b_struct*) _temp742)->f1; goto
_LL731;} else{ goto _LL732;}} else{ goto _LL732;} _LL732: if(*(( int*) _temp726)
==  Cyc_Absyn_Var_e){ _LL747: _temp746=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp726)->f2; if(( unsigned int) _temp746 >  1u?*(( int*) _temp746) ==  Cyc_Absyn_Local_b:
0){ _LL749: _temp748=(( struct Cyc_Absyn_Local_b_struct*) _temp746)->f1; goto
_LL733;} else{ goto _LL734;}} else{ goto _LL734;} _LL734: if(*(( int*) _temp726)
==  Cyc_Absyn_Var_e){ _LL751: _temp750=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp726)->f2; if(( unsigned int) _temp750 >  1u?*(( int*) _temp750) ==  Cyc_Absyn_Pat_b:
0){ _LL753: _temp752=(( struct Cyc_Absyn_Pat_b_struct*) _temp750)->f1; goto
_LL735;} else{ goto _LL736;}} else{ goto _LL736;} _LL736: goto _LL737; _LL729:
_temp744= _temp740; goto _LL731; _LL731: _temp748= _temp744; goto _LL733; _LL733:
_temp752= _temp748; goto _LL735; _LL735: if( ! _temp752->escapes){ return Cyc_CfFlowInfo_reln_kill_var(
r, _temp752);} goto _LL727; _LL737: goto _LL727; _LL727:;} return r;} struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var( struct Cyc_List_List* r, struct Cyc_Absyn_Vardecl*
v, struct Cyc_Absyn_Exp* e){ if( v->escapes){ return r;} r= Cyc_CfFlowInfo_reln_kill_var(
r, v);{ void* _temp754=( void*) e->r; struct Cyc_Absyn_MallocInfo _temp760; int
_temp762; struct Cyc_Absyn_Exp* _temp764; _LL756: if(*(( int*) _temp754) ==  Cyc_Absyn_Malloc_e){
_LL761: _temp760=(( struct Cyc_Absyn_Malloc_e_struct*) _temp754)->f1; _LL765:
_temp764= _temp760.num_elts; goto _LL763; _LL763: _temp762= _temp760.fat_result;
if( _temp762 ==  1){ goto _LL757;} else{ goto _LL758;}} else{ goto _LL758;}
_LL758: goto _LL759; _LL757: malloc_loop: { void* _temp766=( void*) _temp764->r;
struct Cyc_Absyn_Exp* _temp780; void* _temp782; struct Cyc_Absyn_Vardecl*
_temp784; void* _temp786; struct Cyc_Absyn_Vardecl* _temp788; void* _temp790;
struct Cyc_Absyn_Vardecl* _temp792; void* _temp794; struct Cyc_Absyn_Vardecl*
_temp796; _LL768: if(*(( int*) _temp766) ==  Cyc_Absyn_Cast_e){ _LL781: _temp780=((
struct Cyc_Absyn_Cast_e_struct*) _temp766)->f2; goto _LL769;} else{ goto _LL770;}
_LL770: if(*(( int*) _temp766) ==  Cyc_Absyn_Var_e){ _LL783: _temp782=( void*)((
struct Cyc_Absyn_Var_e_struct*) _temp766)->f2; if(( unsigned int) _temp782 >  1u?*((
int*) _temp782) ==  Cyc_Absyn_Pat_b: 0){ _LL785: _temp784=(( struct Cyc_Absyn_Pat_b_struct*)
_temp782)->f1; goto _LL771;} else{ goto _LL772;}} else{ goto _LL772;} _LL772:
if(*(( int*) _temp766) ==  Cyc_Absyn_Var_e){ _LL787: _temp786=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp766)->f2; if(( unsigned int) _temp786 >  1u?*((
int*) _temp786) ==  Cyc_Absyn_Local_b: 0){ _LL789: _temp788=(( struct Cyc_Absyn_Local_b_struct*)
_temp786)->f1; goto _LL773;} else{ goto _LL774;}} else{ goto _LL774;} _LL774:
if(*(( int*) _temp766) ==  Cyc_Absyn_Var_e){ _LL791: _temp790=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp766)->f2; if(( unsigned int) _temp790 >  1u?*((
int*) _temp790) ==  Cyc_Absyn_Param_b: 0){ _LL793: _temp792=(( struct Cyc_Absyn_Param_b_struct*)
_temp790)->f1; goto _LL775;} else{ goto _LL776;}} else{ goto _LL776;} _LL776:
if(*(( int*) _temp766) ==  Cyc_Absyn_Var_e){ _LL795: _temp794=( void*)(( struct
Cyc_Absyn_Var_e_struct*) _temp766)->f2; if(( unsigned int) _temp794 >  1u?*((
int*) _temp794) ==  Cyc_Absyn_Global_b: 0){ _LL797: _temp796=(( struct Cyc_Absyn_Global_b_struct*)
_temp794)->f1; goto _LL777;} else{ goto _LL778;}} else{ goto _LL778;} _LL778:
goto _LL779; _LL769: _temp764= _temp780; goto malloc_loop; _LL771: _temp788=
_temp784; goto _LL773; _LL773: _temp792= _temp788; goto _LL775; _LL775: _temp796=
_temp792; goto _LL777; _LL777: if( _temp796->escapes){ return r;} return({
struct Cyc_List_List* _temp798=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp798->hd=( void*)({ struct Cyc_CfFlowInfo_Reln*
_temp799=( struct Cyc_CfFlowInfo_Reln*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln));
_temp799->vd= _temp796; _temp799->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessEqSize_struct*
_temp800=( struct Cyc_CfFlowInfo_LessEqSize_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_LessEqSize_struct)); _temp800[ 0]=({ struct Cyc_CfFlowInfo_LessEqSize_struct
_temp801; _temp801.tag= Cyc_CfFlowInfo_LessEqSize; _temp801.f1= v; _temp801;});
_temp800;})); _temp799;}); _temp798->tl= r; _temp798;}); _LL779: return r;
_LL767:;} _LL759: goto _LL755; _LL755:;}{ void* _temp802= Cyc_Tcutil_compress((
void*) v->type); _LL804: if(( unsigned int) _temp802 >  3u?*(( int*) _temp802)
==  Cyc_Absyn_IntType: 0){ goto _LL805;} else{ goto _LL806;} _LL806: goto _LL807;
_LL805: goto _LL803; _LL807: return r; _LL803:;} loop: { void* _temp808=( void*)
e->r; struct Cyc_Absyn_Exp* _temp820; void* _temp822; int _temp824; struct Cyc_List_List*
_temp826; struct Cyc_List_List _temp828; struct Cyc_List_List* _temp829; struct
Cyc_List_List _temp831; struct Cyc_Absyn_Exp* _temp832; void* _temp834; struct
Cyc_List_List* _temp836; struct Cyc_List_List _temp838; struct Cyc_Absyn_Exp*
_temp839; void* _temp841; _LL810: if(*(( int*) _temp808) ==  Cyc_Absyn_Cast_e){
_LL821: _temp820=(( struct Cyc_Absyn_Cast_e_struct*) _temp808)->f2; goto _LL811;}
else{ goto _LL812;} _LL812: if(*(( int*) _temp808) ==  Cyc_Absyn_Const_e){
_LL823: _temp822=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp808)->f1; if((
unsigned int) _temp822 >  1u?*(( int*) _temp822) ==  Cyc_Absyn_Int_c: 0){ _LL825:
_temp824=(( struct Cyc_Absyn_Int_c_struct*) _temp822)->f2; goto _LL813;} else{
goto _LL814;}} else{ goto _LL814;} _LL814: if(*(( int*) _temp808) ==  Cyc_Absyn_Primop_e){
_LL835: _temp834=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp808)->f1;
if( _temp834 == ( void*) Cyc_Absyn_Mod){ goto _LL827;} else{ goto _LL816;}
_LL827: _temp826=(( struct Cyc_Absyn_Primop_e_struct*) _temp808)->f2; if(
_temp826 ==  0){ goto _LL816;} else{ _temp828=* _temp826; _LL830: _temp829=
_temp828.tl; if( _temp829 ==  0){ goto _LL816;} else{ _temp831=* _temp829;
_LL833: _temp832=( struct Cyc_Absyn_Exp*) _temp831.hd; goto _LL815;}}} else{
goto _LL816;} _LL816: if(*(( int*) _temp808) ==  Cyc_Absyn_Primop_e){ _LL842:
_temp841=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp808)->f1; if(
_temp841 == ( void*) Cyc_Absyn_Size){ goto _LL837;} else{ goto _LL818;} _LL837:
_temp836=(( struct Cyc_Absyn_Primop_e_struct*) _temp808)->f2; if( _temp836 ==  0){
goto _LL818;} else{ _temp838=* _temp836; _LL840: _temp839=( struct Cyc_Absyn_Exp*)
_temp838.hd; goto _LL817;}} else{ goto _LL818;} _LL818: goto _LL819; _LL811: e=
_temp820; goto loop; _LL813: return({ struct Cyc_List_List* _temp843=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp843->hd=( void*)({
struct Cyc_CfFlowInfo_Reln* _temp844=( struct Cyc_CfFlowInfo_Reln*) _cycalloc(
sizeof( struct Cyc_CfFlowInfo_Reln)); _temp844->vd= v; _temp844->rop=( void*)((
void*)({ struct Cyc_CfFlowInfo_EqualConst_struct* _temp845=( struct Cyc_CfFlowInfo_EqualConst_struct*)
_cycalloc_atomic( sizeof( struct Cyc_CfFlowInfo_EqualConst_struct)); _temp845[ 0]=({
struct Cyc_CfFlowInfo_EqualConst_struct _temp846; _temp846.tag= Cyc_CfFlowInfo_EqualConst;
_temp846.f1=( unsigned int) _temp824; _temp846;}); _temp845;})); _temp844;});
_temp843->tl= r; _temp843;}); _LL815:{ void* _temp847=( void*) _temp832->r;
struct Cyc_List_List* _temp853; struct Cyc_List_List _temp855; struct Cyc_Absyn_Exp*
_temp856; void* _temp858; _LL849: if(*(( int*) _temp847) ==  Cyc_Absyn_Primop_e){
_LL859: _temp858=( void*)(( struct Cyc_Absyn_Primop_e_struct*) _temp847)->f1;
if( _temp858 == ( void*) Cyc_Absyn_Size){ goto _LL854;} else{ goto _LL851;}
_LL854: _temp853=(( struct Cyc_Absyn_Primop_e_struct*) _temp847)->f2; if(
_temp853 ==  0){ goto _LL851;} else{ _temp855=* _temp853; _LL857: _temp856=(
struct Cyc_Absyn_Exp*) _temp855.hd; goto _LL850;}} else{ goto _LL851;} _LL851:
goto _LL852; _LL850:{ void* _temp860=( void*) _temp856->r; void* _temp872;
struct Cyc_Absyn_Vardecl* _temp874; void* _temp876; struct Cyc_Absyn_Vardecl*
_temp878; void* _temp880; struct Cyc_Absyn_Vardecl* _temp882; void* _temp884;
struct Cyc_Absyn_Vardecl* _temp886; _LL862: if(*(( int*) _temp860) ==  Cyc_Absyn_Var_e){
_LL873: _temp872=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp860)->f2; if((
unsigned int) _temp872 >  1u?*(( int*) _temp872) ==  Cyc_Absyn_Global_b: 0){
_LL875: _temp874=(( struct Cyc_Absyn_Global_b_struct*) _temp872)->f1; goto
_LL863;} else{ goto _LL864;}} else{ goto _LL864;} _LL864: if(*(( int*) _temp860)
==  Cyc_Absyn_Var_e){ _LL877: _temp876=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp860)->f2; if(( unsigned int) _temp876 >  1u?*(( int*) _temp876) ==  Cyc_Absyn_Local_b:
0){ _LL879: _temp878=(( struct Cyc_Absyn_Local_b_struct*) _temp876)->f1; goto
_LL865;} else{ goto _LL866;}} else{ goto _LL866;} _LL866: if(*(( int*) _temp860)
==  Cyc_Absyn_Var_e){ _LL881: _temp880=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp860)->f2; if(( unsigned int) _temp880 >  1u?*(( int*) _temp880) ==  Cyc_Absyn_Param_b:
0){ _LL883: _temp882=(( struct Cyc_Absyn_Param_b_struct*) _temp880)->f1; goto
_LL867;} else{ goto _LL868;}} else{ goto _LL868;} _LL868: if(*(( int*) _temp860)
==  Cyc_Absyn_Var_e){ _LL885: _temp884=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp860)->f2; if(( unsigned int) _temp884 >  1u?*(( int*) _temp884) ==  Cyc_Absyn_Pat_b:
0){ _LL887: _temp886=(( struct Cyc_Absyn_Pat_b_struct*) _temp884)->f1; goto
_LL869;} else{ goto _LL870;}} else{ goto _LL870;} _LL870: goto _LL871; _LL863:
_temp878= _temp874; goto _LL865; _LL865: _temp882= _temp878; goto _LL867; _LL867:
_temp886= _temp882; goto _LL869; _LL869: if( _temp886->escapes){ return r;}
return({ struct Cyc_List_List* _temp888=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp888->hd=( void*)({ struct Cyc_CfFlowInfo_Reln*
_temp889=( struct Cyc_CfFlowInfo_Reln*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln));
_temp889->vd= v; _temp889->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessSize_struct*
_temp890=( struct Cyc_CfFlowInfo_LessSize_struct*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_LessSize_struct));
_temp890[ 0]=({ struct Cyc_CfFlowInfo_LessSize_struct _temp891; _temp891.tag=
Cyc_CfFlowInfo_LessSize; _temp891.f1= _temp886; _temp891;}); _temp890;}));
_temp889;}); _temp888->tl= r; _temp888;}); _LL871: goto _LL861; _LL861:;} goto
_LL848; _LL852: goto _LL848; _LL848:;} goto _LL809; _LL817:{ void* _temp892=(
void*) _temp839->r; void* _temp904; struct Cyc_Absyn_Vardecl* _temp906; void*
_temp908; struct Cyc_Absyn_Vardecl* _temp910; void* _temp912; struct Cyc_Absyn_Vardecl*
_temp914; void* _temp916; struct Cyc_Absyn_Vardecl* _temp918; _LL894: if(*(( int*)
_temp892) ==  Cyc_Absyn_Var_e){ _LL905: _temp904=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp892)->f2; if(( unsigned int) _temp904 >  1u?*(( int*) _temp904) ==  Cyc_Absyn_Global_b:
0){ _LL907: _temp906=(( struct Cyc_Absyn_Global_b_struct*) _temp904)->f1; goto
_LL895;} else{ goto _LL896;}} else{ goto _LL896;} _LL896: if(*(( int*) _temp892)
==  Cyc_Absyn_Var_e){ _LL909: _temp908=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp892)->f2; if(( unsigned int) _temp908 >  1u?*(( int*) _temp908) ==  Cyc_Absyn_Local_b:
0){ _LL911: _temp910=(( struct Cyc_Absyn_Local_b_struct*) _temp908)->f1; goto
_LL897;} else{ goto _LL898;}} else{ goto _LL898;} _LL898: if(*(( int*) _temp892)
==  Cyc_Absyn_Var_e){ _LL913: _temp912=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp892)->f2; if(( unsigned int) _temp912 >  1u?*(( int*) _temp912) ==  Cyc_Absyn_Param_b:
0){ _LL915: _temp914=(( struct Cyc_Absyn_Param_b_struct*) _temp912)->f1; goto
_LL899;} else{ goto _LL900;}} else{ goto _LL900;} _LL900: if(*(( int*) _temp892)
==  Cyc_Absyn_Var_e){ _LL917: _temp916=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp892)->f2; if(( unsigned int) _temp916 >  1u?*(( int*) _temp916) ==  Cyc_Absyn_Pat_b:
0){ _LL919: _temp918=(( struct Cyc_Absyn_Pat_b_struct*) _temp916)->f1; goto
_LL901;} else{ goto _LL902;}} else{ goto _LL902;} _LL902: goto _LL903; _LL895:
_temp910= _temp906; goto _LL897; _LL897: _temp914= _temp910; goto _LL899; _LL899:
_temp918= _temp914; goto _LL901; _LL901: if( _temp918->escapes){ return r;}
return({ struct Cyc_List_List* _temp920=( struct Cyc_List_List*) _cycalloc(
sizeof( struct Cyc_List_List)); _temp920->hd=( void*)({ struct Cyc_CfFlowInfo_Reln*
_temp921=( struct Cyc_CfFlowInfo_Reln*) _cycalloc( sizeof( struct Cyc_CfFlowInfo_Reln));
_temp921->vd= v; _temp921->rop=( void*)(( void*)({ struct Cyc_CfFlowInfo_LessEqSize_struct*
_temp922=( struct Cyc_CfFlowInfo_LessEqSize_struct*) _cycalloc( sizeof( struct
Cyc_CfFlowInfo_LessEqSize_struct)); _temp922[ 0]=({ struct Cyc_CfFlowInfo_LessEqSize_struct
_temp923; _temp923.tag= Cyc_CfFlowInfo_LessEqSize; _temp923.f1= _temp918;
_temp923;}); _temp922;})); _temp921;}); _temp920->tl= r; _temp920;}); _LL903:
goto _LL893; _LL893:;} goto _LL809; _LL819: goto _LL809; _LL809:;} return r;}
struct Cyc_List_List* Cyc_CfFlowInfo_reln_assign_exp( struct Cyc_List_List* r,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){{ void* _temp924=( void*) e1->r;
void* _temp936; struct Cyc_Absyn_Vardecl* _temp938; void* _temp940; struct Cyc_Absyn_Vardecl*
_temp942; void* _temp944; struct Cyc_Absyn_Vardecl* _temp946; void* _temp948;
struct Cyc_Absyn_Vardecl* _temp950; _LL926: if(*(( int*) _temp924) ==  Cyc_Absyn_Var_e){
_LL937: _temp936=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp924)->f2; if((
unsigned int) _temp936 >  1u?*(( int*) _temp936) ==  Cyc_Absyn_Global_b: 0){
_LL939: _temp938=(( struct Cyc_Absyn_Global_b_struct*) _temp936)->f1; goto
_LL927;} else{ goto _LL928;}} else{ goto _LL928;} _LL928: if(*(( int*) _temp924)
==  Cyc_Absyn_Var_e){ _LL941: _temp940=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp924)->f2; if(( unsigned int) _temp940 >  1u?*(( int*) _temp940) ==  Cyc_Absyn_Param_b:
0){ _LL943: _temp942=(( struct Cyc_Absyn_Param_b_struct*) _temp940)->f1; goto
_LL929;} else{ goto _LL930;}} else{ goto _LL930;} _LL930: if(*(( int*) _temp924)
==  Cyc_Absyn_Var_e){ _LL945: _temp944=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp924)->f2; if(( unsigned int) _temp944 >  1u?*(( int*) _temp944) ==  Cyc_Absyn_Local_b:
0){ _LL947: _temp946=(( struct Cyc_Absyn_Local_b_struct*) _temp944)->f1; goto
_LL931;} else{ goto _LL932;}} else{ goto _LL932;} _LL932: if(*(( int*) _temp924)
==  Cyc_Absyn_Var_e){ _LL949: _temp948=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp924)->f2; if(( unsigned int) _temp948 >  1u?*(( int*) _temp948) ==  Cyc_Absyn_Pat_b:
0){ _LL951: _temp950=(( struct Cyc_Absyn_Pat_b_struct*) _temp948)->f1; goto
_LL933;} else{ goto _LL934;}} else{ goto _LL934;} _LL934: goto _LL935; _LL927:
_temp942= _temp938; goto _LL929; _LL929: _temp946= _temp942; goto _LL931; _LL931:
_temp950= _temp946; goto _LL933; _LL933: if( ! _temp950->escapes){ return Cyc_CfFlowInfo_reln_assign_var(
r, _temp950, e2);} goto _LL925; _LL935: goto _LL925; _LL925:;} return r;} void
Cyc_CfFlowInfo_print_reln( struct Cyc_CfFlowInfo_Reln* r){({ struct Cyc_Std_String_pa_struct
_temp953; _temp953.tag= Cyc_Std_String_pa; _temp953.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
r->vd)->name);{ void* _temp952[ 1u]={& _temp953}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("%s", sizeof( unsigned char), 3u), _tag_arr( _temp952, sizeof( void*),
1u));}});{ void* _temp954=( void*) r->rop; unsigned int _temp966; struct Cyc_Absyn_Vardecl*
_temp968; struct Cyc_Absyn_Vardecl* _temp970; unsigned int _temp972; struct Cyc_Absyn_Vardecl*
_temp974; _LL956: if(*(( int*) _temp954) ==  Cyc_CfFlowInfo_EqualConst){ _LL967:
_temp966=(( struct Cyc_CfFlowInfo_EqualConst_struct*) _temp954)->f1; goto _LL957;}
else{ goto _LL958;} _LL958: if(*(( int*) _temp954) ==  Cyc_CfFlowInfo_LessVar){
_LL969: _temp968=(( struct Cyc_CfFlowInfo_LessVar_struct*) _temp954)->f1; goto
_LL959;} else{ goto _LL960;} _LL960: if(*(( int*) _temp954) ==  Cyc_CfFlowInfo_LessSize){
_LL971: _temp970=(( struct Cyc_CfFlowInfo_LessSize_struct*) _temp954)->f1; goto
_LL961;} else{ goto _LL962;} _LL962: if(*(( int*) _temp954) ==  Cyc_CfFlowInfo_LessConst){
_LL973: _temp972=(( struct Cyc_CfFlowInfo_LessConst_struct*) _temp954)->f1; goto
_LL963;} else{ goto _LL964;} _LL964: if(*(( int*) _temp954) ==  Cyc_CfFlowInfo_LessEqSize){
_LL975: _temp974=(( struct Cyc_CfFlowInfo_LessEqSize_struct*) _temp954)->f1;
goto _LL965;} else{ goto _LL955;} _LL957:({ struct Cyc_Std_Int_pa_struct
_temp977; _temp977.tag= Cyc_Std_Int_pa; _temp977.f1=( int) _temp966;{ void*
_temp976[ 1u]={& _temp977}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("==%d",
sizeof( unsigned char), 5u), _tag_arr( _temp976, sizeof( void*), 1u));}}); goto
_LL955; _LL959:({ struct Cyc_Std_String_pa_struct _temp979; _temp979.tag= Cyc_Std_String_pa;
_temp979.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp968->name);{
void* _temp978[ 1u]={& _temp979}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("<%s",
sizeof( unsigned char), 4u), _tag_arr( _temp978, sizeof( void*), 1u));}}); goto
_LL955; _LL961:({ struct Cyc_Std_String_pa_struct _temp981; _temp981.tag= Cyc_Std_String_pa;
_temp981.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp970->name);{
void* _temp980[ 1u]={& _temp981}; Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr("<%s.size",
sizeof( unsigned char), 9u), _tag_arr( _temp980, sizeof( void*), 1u));}}); goto
_LL955; _LL963:({ struct Cyc_Std_Int_pa_struct _temp983; _temp983.tag= Cyc_Std_Int_pa;
_temp983.f1=( int) _temp972;{ void* _temp982[ 1u]={& _temp983}; Cyc_Std_fprintf(
Cyc_Std_stderr, _tag_arr("<%d", sizeof( unsigned char), 4u), _tag_arr( _temp982,
sizeof( void*), 1u));}}); goto _LL955; _LL965:({ struct Cyc_Std_String_pa_struct
_temp985; _temp985.tag= Cyc_Std_String_pa; _temp985.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp974->name);{ void* _temp984[ 1u]={& _temp985}; Cyc_Std_fprintf( Cyc_Std_stderr,
_tag_arr("<=%s.size", sizeof( unsigned char), 10u), _tag_arr( _temp984, sizeof(
void*), 1u));}}); goto _LL955; _LL955:;}} void Cyc_CfFlowInfo_print_relns(
struct Cyc_List_List* r){ for( 0; r !=  0; r= r->tl){ Cyc_CfFlowInfo_print_reln((
struct Cyc_CfFlowInfo_Reln*) r->hd); if( r->tl !=  0){({ void* _temp986[ 0u]={};
Cyc_Std_fprintf( Cyc_Std_stderr, _tag_arr(",", sizeof( unsigned char), 2u),
_tag_arr( _temp986, sizeof( void*), 0u));});}}}
