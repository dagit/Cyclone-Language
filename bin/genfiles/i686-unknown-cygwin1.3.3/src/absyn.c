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
tl; } ; extern struct Cyc_List_List* Cyc_List_list( struct _tagged_arr); extern
struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_imp_rev( struct Cyc_List_List* x); extern unsigned char Cyc_List_Nth[ 8u];
extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct Cyc_List_List*
l1, struct Cyc_List_List* l2); struct Cyc_Lineno_Pos{ struct _tagged_arr
logical_file; struct _tagged_arr line; int line_no; int col; } ; extern
unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static const
int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static const
int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr source;
struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; } ;
extern unsigned char Cyc_Position_Nocontext[ 14u]; static const int Cyc_Absyn_Loc_n=
0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{ int tag;
struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{
int tag; struct Cyc_List_List* f1; } ; struct _tuple0{ void* f1; struct
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
int tag; struct _tagged_arr* f1; } ; unsigned char Cyc_Absyn_EmptyAnnot[ 15u]="\000\000\000\000EmptyAnnot";
extern int Cyc_Absyn_qvar_cmp( struct _tuple0*, struct _tuple0*); extern int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*, struct Cyc_List_List*); extern int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*, struct Cyc_Absyn_Tvar*); extern struct Cyc_Absyn_Rel_n_struct
Cyc_Absyn_rel_ns_null_value; extern void* Cyc_Absyn_rel_ns_null; extern int Cyc_Absyn_is_qvar_qualified(
struct _tuple0*); extern struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(); extern
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual( struct Cyc_Absyn_Tqual x, struct
Cyc_Absyn_Tqual y); extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_def( void*, struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_compress_kb( void*); extern void* Cyc_Absyn_force_kb(
void* kb); extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt* k, struct Cyc_Core_Opt*
tenv); extern void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*); extern void* Cyc_Absyn_uchar_t;
extern void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_ulonglong_t; extern void* Cyc_Absyn_schar_t; extern void*
Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t;
extern void* Cyc_Absyn_slonglong_t; extern void* Cyc_Absyn_float_t; extern void*
Cyc_Absyn_double_t( int); extern void* Cyc_Absyn_empty_effect; extern struct
_tuple0* Cyc_Absyn_exn_name; extern struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud;
extern void* Cyc_Absyn_exn_typ; extern struct _tuple0* Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple0* Cyc_Absyn_tunion_scanf_arg_qvar; extern void* Cyc_Absyn_string_typ(
void* rgn); extern void* Cyc_Absyn_const_string_typ( void* rgn); extern void*
Cyc_Absyn_file_typ(); extern struct Cyc_Absyn_Exp* Cyc_Absyn_exp_unsigned_one;
extern void* Cyc_Absyn_bounds_one; extern void* Cyc_Absyn_starb_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_at_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_tagged_typ( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_void_star_typ();
extern struct Cyc_Core_Opt* Cyc_Absyn_void_star_typ_opt(); extern void* Cyc_Absyn_strct(
struct _tagged_arr* name); extern void* Cyc_Absyn_strctq( struct _tuple0* name);
extern void* Cyc_Absyn_unionq_typ( struct _tuple0* name); extern void* Cyc_Absyn_union_typ(
struct _tagged_arr* name); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_const_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_bool_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( unsigned char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_arr f, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_arr s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp( struct _tuple0*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*,
struct Cyc_List_List* es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*, void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_offsetof_exp( void*, void*, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_gentyp_exp( struct Cyc_List_List*, void*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp( struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp(
struct Cyc_Absyn_Exp*, struct _tagged_arr*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp*,
struct _tagged_arr*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_tuple_exp( struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_match_exn_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_array_exp( struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp( struct Cyc_Core_Opt*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts( struct Cyc_List_List*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt( struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt( struct Cyc_List_List* el,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple0*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt( struct _tagged_arr* v, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_arr* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_forarray_stmt( struct Cyc_List_List*, struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment*
s); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p,
struct Cyc_Core_Opt* t_opt, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_letv_decl( struct Cyc_List_List*,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_struct_decl( void* s, struct Cyc_Core_Opt*
n, struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl( void* s, struct _tuple0* n, struct Cyc_List_List*
ts, struct Cyc_Core_Opt* fs, int is_xtunion, struct Cyc_Position_Segment* loc);
extern void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct Cyc_Core_Opt*
eff_typ, void* ret_typ, struct Cyc_List_List* args, int c_varargs, struct Cyc_Absyn_VarargInfo*
cyc_varargs, struct Cyc_List_List* rgn_po, struct Cyc_List_List*); extern void*
Cyc_Absyn_pointer_expand( void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*);
extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*,
struct _tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_arr*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*);
struct _tuple3{ struct Cyc_Absyn_Tqual f1; void* f2; } ; extern struct _tuple3*
Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List*, int); extern struct
_tagged_arr Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att(
void* a); extern struct _tagged_arr* Cyc_Absyn_fieldname( int); extern void Cyc_Absyn_print_decls(
struct Cyc_List_List*); static const int Cyc_Typerep_Var= 0; struct Cyc_Typerep_Var_struct{
int tag; struct _tagged_arr* f1; } ; static const int Cyc_Typerep_Int= 1; struct
Cyc_Typerep_Int_struct{ int tag; unsigned int f1; } ; static const int Cyc_Typerep_Float=
0; static const int Cyc_Typerep_Double= 1; static const int Cyc_Typerep_ThinPtr=
2; struct Cyc_Typerep_ThinPtr_struct{ int tag; unsigned int f1; void* f2; } ;
static const int Cyc_Typerep_FatPtr= 3; struct Cyc_Typerep_FatPtr_struct{ int
tag; void* f1; } ; static const int Cyc_Typerep_Tuple= 4; struct _tuple4{
unsigned int f1; void* f2; } ; struct Cyc_Typerep_Tuple_struct{ int tag;
unsigned int f1; struct _tagged_arr f2; } ; static const int Cyc_Typerep_TUnion=
5; struct Cyc_Typerep_TUnion_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Typerep_XTUnion= 6; struct _tuple5{ struct _tagged_arr f1; void*
f2; } ; struct Cyc_Typerep_XTUnion_struct{ int tag; struct _tagged_arr f1; } ;
static const int Cyc_Typerep_Union= 7; struct Cyc_Typerep_Union_struct{ int tag;
struct _tagged_arr f1; } ; static const int Cyc_Typerep_Forall= 8; struct Cyc_Typerep_Forall_struct{
int tag; struct _tagged_arr f1; void** f2; } ; static const int Cyc_Typerep_App=
9; struct Cyc_Typerep_App_struct{ int tag; void* f1; struct _tagged_arr f2; } ;
unsigned int Cyc_Typerep_size_type( void* rep); extern void* Cyc_decls_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Position_Segment_rep; extern
int Cyc_Std_zstrptrcmp( struct _tagged_arr*, struct _tagged_arr*); struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[ 19u];
extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_Std_printf( struct _tagged_arr fmt,
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
int tag; float* f1; } ; struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
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
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_compress(
void* t); extern void Cyc_Marshal_print_type( void* rep, void* val); static int
Cyc_Absyn_zstrlist_cmp( struct Cyc_List_List* ss1, struct Cyc_List_List* ss2){
return(( int(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*), struct
Cyc_List_List* l1, struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_Std_zstrptrcmp,
ss1, ss2);} int Cyc_Absyn_varlist_cmp( struct Cyc_List_List* vs1, struct Cyc_List_List*
vs2){ return Cyc_Absyn_zstrlist_cmp( vs1, vs2);} struct _tuple6{ void* f1; void*
f2; } ; int Cyc_Absyn_qvar_cmp( struct _tuple0* q1, struct _tuple0* q2){ void*
_temp0=(* q1).f1; void* _temp1=(* q2).f1;{ struct _tuple6 _temp3=({ struct
_tuple6 _temp2; _temp2.f1= _temp0; _temp2.f2= _temp1; _temp2;}); void* _temp19;
void* _temp21; void* _temp23; struct Cyc_List_List* _temp25; void* _temp27;
struct Cyc_List_List* _temp29; void* _temp31; struct Cyc_List_List* _temp33;
void* _temp35; struct Cyc_List_List* _temp37; void* _temp39; void* _temp41; void*
_temp43; void* _temp45; _LL5: _LL22: _temp21= _temp3.f1; if( _temp21 == ( void*)
Cyc_Absyn_Loc_n){ goto _LL20;} else{ goto _LL7;} _LL20: _temp19= _temp3.f2; if(
_temp19 == ( void*) Cyc_Absyn_Loc_n){ goto _LL6;} else{ goto _LL7;} _LL7: _LL28:
_temp27= _temp3.f1; if(( unsigned int) _temp27 >  1u?*(( int*) _temp27) ==  Cyc_Absyn_Rel_n:
0){ _LL30: _temp29=(( struct Cyc_Absyn_Rel_n_struct*) _temp27)->f1; goto _LL24;}
else{ goto _LL9;} _LL24: _temp23= _temp3.f2; if(( unsigned int) _temp23 >  1u?*((
int*) _temp23) ==  Cyc_Absyn_Rel_n: 0){ _LL26: _temp25=(( struct Cyc_Absyn_Rel_n_struct*)
_temp23)->f1; goto _LL8;} else{ goto _LL9;} _LL9: _LL36: _temp35= _temp3.f1; if((
unsigned int) _temp35 >  1u?*(( int*) _temp35) ==  Cyc_Absyn_Abs_n: 0){ _LL38:
_temp37=(( struct Cyc_Absyn_Abs_n_struct*) _temp35)->f1; goto _LL32;} else{ goto
_LL11;} _LL32: _temp31= _temp3.f2; if(( unsigned int) _temp31 >  1u?*(( int*)
_temp31) ==  Cyc_Absyn_Abs_n: 0){ _LL34: _temp33=(( struct Cyc_Absyn_Abs_n_struct*)
_temp31)->f1; goto _LL10;} else{ goto _LL11;} _LL11: _LL40: _temp39= _temp3.f1;
if( _temp39 == ( void*) Cyc_Absyn_Loc_n){ goto _LL12;} else{ goto _LL13;} _LL13:
_LL42: _temp41= _temp3.f2; if( _temp41 == ( void*) Cyc_Absyn_Loc_n){ goto _LL14;}
else{ goto _LL15;} _LL15: _LL44: _temp43= _temp3.f1; if(( unsigned int) _temp43
>  1u?*(( int*) _temp43) ==  Cyc_Absyn_Rel_n: 0){ goto _LL16;} else{ goto _LL17;}
_LL17: _LL46: _temp45= _temp3.f2; if(( unsigned int) _temp45 >  1u?*(( int*)
_temp45) ==  Cyc_Absyn_Rel_n: 0){ goto _LL18;} else{ goto _LL4;} _LL6: goto _LL4;
_LL8: _temp37= _temp29; _temp33= _temp25; goto _LL10; _LL10: { int i= Cyc_Absyn_zstrlist_cmp(
_temp37, _temp33); if( i !=  0){ return i;} goto _LL4;} _LL12: return - 1; _LL14:
return 1; _LL16: return - 1; _LL18: return 1; _LL4:;} return Cyc_Std_zstrptrcmp((*
q1).f2,(* q2).f2);} int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct
Cyc_Absyn_Tvar* tv2){ int i= Cyc_Std_zstrptrcmp( tv1->name, tv2->name); if( i != 
0){ return i;} if( tv1->identity ==  tv2->identity){ return 0;} if( tv1->identity
!=  0? tv2->identity !=  0: 0){ return*(( int*) _check_null( tv1->identity)) - *((
int*) _check_null( tv2->identity));} else{ if( tv1->identity ==  0){ return - 1;}
else{ return 1;}}} struct Cyc_Absyn_Rel_n_struct Cyc_Absyn_rel_ns_null_value={ 0u,
0}; void* Cyc_Absyn_rel_ns_null=( void*)& Cyc_Absyn_rel_ns_null_value; int Cyc_Absyn_is_qvar_qualified(
struct _tuple0* qv){ void* _temp48=(* qv).f1; struct Cyc_List_List* _temp58;
struct Cyc_List_List* _temp60; _LL50: if(( unsigned int) _temp48 >  1u?*(( int*)
_temp48) ==  Cyc_Absyn_Rel_n: 0){ _LL59: _temp58=(( struct Cyc_Absyn_Rel_n_struct*)
_temp48)->f1; if( _temp58 ==  0){ goto _LL51;} else{ goto _LL52;}} else{ goto
_LL52;} _LL52: if(( unsigned int) _temp48 >  1u?*(( int*) _temp48) ==  Cyc_Absyn_Abs_n:
0){ _LL61: _temp60=(( struct Cyc_Absyn_Abs_n_struct*) _temp48)->f1; if( _temp60
==  0){ goto _LL53;} else{ goto _LL54;}} else{ goto _LL54;} _LL54: if( _temp48
== ( void*) Cyc_Absyn_Loc_n){ goto _LL55;} else{ goto _LL56;} _LL56: goto _LL57;
_LL51: goto _LL53; _LL53: goto _LL55; _LL55: return 0; _LL57: return 1; _LL49:;}
static int Cyc_Absyn_new_type_counter= 0; void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt*
k, struct Cyc_Core_Opt* env){ return( void*)({ struct Cyc_Absyn_Evar_struct*
_temp62=( struct Cyc_Absyn_Evar_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Evar_struct));
_temp62[ 0]=({ struct Cyc_Absyn_Evar_struct _temp63; _temp63.tag= Cyc_Absyn_Evar;
_temp63.f1= k; _temp63.f2= 0; _temp63.f3= Cyc_Absyn_new_type_counter ++; _temp63.f4=
env; _temp63;}); _temp62;});} static struct Cyc_Core_Opt Cyc_Absyn_mk={( void*)((
void*) 1u)}; void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt* tenv){ return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Absyn_mk, tenv);} struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x, struct Cyc_Absyn_Tqual y){ return({ struct Cyc_Absyn_Tqual
_temp64; _temp64.q_const= x.q_const? 1: y.q_const; _temp64.q_volatile= x.q_volatile?
1: y.q_volatile; _temp64.q_restrict= x.q_restrict? 1: y.q_restrict; _temp64;});}
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(){ return({ struct Cyc_Absyn_Tqual
_temp65; _temp65.q_const= 0; _temp65.q_volatile= 0; _temp65.q_restrict= 0;
_temp65;});} struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(){ return({ struct Cyc_Absyn_Tqual
_temp66; _temp66.q_const= 1; _temp66.q_volatile= 0; _temp66.q_restrict= 0;
_temp66;});} struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x){ return({
struct Cyc_Absyn_Conref* _temp67=( struct Cyc_Absyn_Conref*) _cycalloc( sizeof(
struct Cyc_Absyn_Conref)); _temp67->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp68=( struct Cyc_Absyn_Eq_constr_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp68[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp69; _temp69.tag= Cyc_Absyn_Eq_constr;
_temp69.f1=( void*) x; _temp69;}); _temp68;})); _temp67;});} struct Cyc_Absyn_Conref*
Cyc_Absyn_empty_conref(){ return({ struct Cyc_Absyn_Conref* _temp70=( struct Cyc_Absyn_Conref*)
_cycalloc( sizeof( struct Cyc_Absyn_Conref)); _temp70->v=( void*)(( void*) Cyc_Absyn_No_constr);
_temp70;});} static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_true_constraint={
0u,( void*) 1}; static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_false_constraint={
0u,( void*) 0}; struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v={( void*)(( void*)&
Cyc_Absyn_true_constraint)}; struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v={(
void*)(( void*)& Cyc_Absyn_false_constraint)}; struct Cyc_Absyn_Conref* Cyc_Absyn_true_conref=&
Cyc_Absyn_true_conref_v; struct Cyc_Absyn_Conref* Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x){
void* _temp73=( void*) x->v; struct Cyc_Absyn_Conref* _temp81; struct Cyc_Absyn_Conref**
_temp83; _LL75: if( _temp73 == ( void*) Cyc_Absyn_No_constr){ goto _LL76;} else{
goto _LL77;} _LL77: if(( unsigned int) _temp73 >  1u?*(( int*) _temp73) ==  Cyc_Absyn_Eq_constr:
0){ goto _LL78;} else{ goto _LL79;} _LL79: if(( unsigned int) _temp73 >  1u?*((
int*) _temp73) ==  Cyc_Absyn_Forward_constr: 0){ _LL82: _temp81=(( struct Cyc_Absyn_Forward_constr_struct*)
_temp73)->f1; _temp83=( struct Cyc_Absyn_Conref**)&(( struct Cyc_Absyn_Forward_constr_struct*)
_temp73)->f1; goto _LL80;} else{ goto _LL74;} _LL76: return x; _LL78: return x;
_LL80: { struct Cyc_Absyn_Conref* _temp84= Cyc_Absyn_compress_conref(* _temp83);*
_temp83= _temp84; return _temp84;} _LL74:;} void* Cyc_Absyn_conref_val( struct
Cyc_Absyn_Conref* x){ void* _temp85=( void*)( Cyc_Absyn_compress_conref( x))->v;
void* _temp91; _LL87: if(( unsigned int) _temp85 >  1u?*(( int*) _temp85) == 
Cyc_Absyn_Eq_constr: 0){ _LL92: _temp91=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp85)->f1; goto _LL88;} else{ goto _LL89;} _LL89: goto _LL90; _LL88: return
_temp91; _LL90: return({ void* _temp93[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("conref_val",
sizeof( unsigned char), 11u), _tag_arr( _temp93, sizeof( void*), 0u));}); _LL86:;}
void* Cyc_Absyn_conref_def( void* y, struct Cyc_Absyn_Conref* x){ void* _temp94=(
void*)( Cyc_Absyn_compress_conref( x))->v; void* _temp100; _LL96: if((
unsigned int) _temp94 >  1u?*(( int*) _temp94) ==  Cyc_Absyn_Eq_constr: 0){
_LL101: _temp100=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp94)->f1;
goto _LL97;} else{ goto _LL98;} _LL98: goto _LL99; _LL97: return _temp100; _LL99:
return y; _LL95:;} void* Cyc_Absyn_compress_kb( void* k){ void* _temp102= k;
struct Cyc_Core_Opt* _temp114; struct Cyc_Core_Opt* _temp116; struct Cyc_Core_Opt*
_temp118; struct Cyc_Core_Opt _temp120; void* _temp121; void** _temp123; struct
Cyc_Core_Opt* _temp124; struct Cyc_Core_Opt _temp126; void* _temp127; void**
_temp129; _LL104: if(*(( int*) _temp102) ==  Cyc_Absyn_Eq_kb){ goto _LL105;}
else{ goto _LL106;} _LL106: if(*(( int*) _temp102) ==  Cyc_Absyn_Unknown_kb){
_LL115: _temp114=(( struct Cyc_Absyn_Unknown_kb_struct*) _temp102)->f1; if(
_temp114 ==  0){ goto _LL107;} else{ goto _LL108;}} else{ goto _LL108;} _LL108:
if(*(( int*) _temp102) ==  Cyc_Absyn_Less_kb){ _LL117: _temp116=(( struct Cyc_Absyn_Less_kb_struct*)
_temp102)->f1; if( _temp116 ==  0){ goto _LL109;} else{ goto _LL110;}} else{
goto _LL110;} _LL110: if(*(( int*) _temp102) ==  Cyc_Absyn_Unknown_kb){ _LL119:
_temp118=(( struct Cyc_Absyn_Unknown_kb_struct*) _temp102)->f1; if( _temp118 == 
0){ goto _LL112;} else{ _temp120=* _temp118; _LL122: _temp121=( void*) _temp120.v;
_temp123=( void**)&(*(( struct Cyc_Absyn_Unknown_kb_struct*) _temp102)->f1).v;
goto _LL111;}} else{ goto _LL112;} _LL112: if(*(( int*) _temp102) ==  Cyc_Absyn_Less_kb){
_LL125: _temp124=(( struct Cyc_Absyn_Less_kb_struct*) _temp102)->f1; if(
_temp124 ==  0){ goto _LL103;} else{ _temp126=* _temp124; _LL128: _temp127=(
void*) _temp126.v; _temp129=( void**)&(*(( struct Cyc_Absyn_Less_kb_struct*)
_temp102)->f1).v; goto _LL113;}} else{ goto _LL103;} _LL105: return k; _LL107:
return k; _LL109: return k; _LL111: _temp129= _temp123; goto _LL113; _LL113:*
_temp129= Cyc_Absyn_compress_kb(* _temp129); return* _temp129; _LL103:;} void*
Cyc_Absyn_force_kb( void* kb){ void* _temp130= Cyc_Absyn_compress_kb( kb); void*
_temp138; struct Cyc_Core_Opt* _temp140; struct Cyc_Core_Opt** _temp142; void*
_temp143; struct Cyc_Core_Opt* _temp145; struct Cyc_Core_Opt** _temp147; _LL132:
if(*(( int*) _temp130) ==  Cyc_Absyn_Eq_kb){ _LL139: _temp138=( void*)(( struct
Cyc_Absyn_Eq_kb_struct*) _temp130)->f1; goto _LL133;} else{ goto _LL134;} _LL134:
if(*(( int*) _temp130) ==  Cyc_Absyn_Unknown_kb){ _LL141: _temp140=(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp130)->f1; _temp142=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp130)->f1; goto _LL135;} else{ goto _LL136;} _LL136: if(*(( int*) _temp130)
==  Cyc_Absyn_Less_kb){ _LL146: _temp145=(( struct Cyc_Absyn_Less_kb_struct*)
_temp130)->f1; _temp147=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp130)->f1; goto _LL144; _LL144: _temp143=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp130)->f2; goto _LL137;} else{ goto _LL131;} _LL133: return _temp138; _LL135:
_temp147= _temp142; _temp143=( void*) Cyc_Absyn_BoxKind; goto _LL137; _LL137:*
_temp147=({ struct Cyc_Core_Opt* _temp148=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp148->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp149=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp149[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp150; _temp150.tag= Cyc_Absyn_Eq_kb;
_temp150.f1=( void*) _temp143; _temp150;}); _temp149;})); _temp148;}); return
_temp143; _LL131:;} static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uchar_tt={
5u,( void*)(( void*) 1u),( void*)(( void*) 0u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ushort_tt={ 5u,( void*)(( void*) 1u),( void*)(( void*) 1u)}; static
struct Cyc_Absyn_IntType_struct Cyc_Absyn_uint_tt={ 5u,( void*)(( void*) 1u),(
void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulong_tt={
5u,( void*)(( void*) 1u),( void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ulonglong_tt={ 5u,( void*)(( void*) 1u),( void*)(( void*) 3u)}; void*
Cyc_Absyn_uchar_t=( void*)& Cyc_Absyn_uchar_tt; void* Cyc_Absyn_ushort_t=( void*)&
Cyc_Absyn_ushort_tt; void* Cyc_Absyn_uint_t=( void*)& Cyc_Absyn_uint_tt; void*
Cyc_Absyn_ulong_t=( void*)& Cyc_Absyn_ulong_tt; void* Cyc_Absyn_ulonglong_t=(
void*)& Cyc_Absyn_ulonglong_tt; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_schar_tt={
5u,( void*)(( void*) 0u),( void*)(( void*) 0u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sshort_tt={ 5u,( void*)(( void*) 0u),( void*)(( void*) 1u)}; static
struct Cyc_Absyn_IntType_struct Cyc_Absyn_sint_tt={ 5u,( void*)(( void*) 0u),(
void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slong_tt={
5u,( void*)(( void*) 0u),( void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_slonglong_tt={ 5u,( void*)(( void*) 0u),( void*)(( void*) 3u)}; void*
Cyc_Absyn_schar_t=( void*)& Cyc_Absyn_schar_tt; void* Cyc_Absyn_sshort_t=( void*)&
Cyc_Absyn_sshort_tt; void* Cyc_Absyn_sint_t=( void*)& Cyc_Absyn_sint_tt; void*
Cyc_Absyn_slong_t=( void*)& Cyc_Absyn_slong_tt; void* Cyc_Absyn_slonglong_t=(
void*)& Cyc_Absyn_slonglong_tt; void* Cyc_Absyn_float_t=( void*) 1u; void* Cyc_Absyn_double_t(
int b){ return( void*)({ struct Cyc_Absyn_DoubleType_struct* _temp161=( struct
Cyc_Absyn_DoubleType_struct*) _cycalloc_atomic( sizeof( struct Cyc_Absyn_DoubleType_struct));
_temp161[ 0]=({ struct Cyc_Absyn_DoubleType_struct _temp162; _temp162.tag= Cyc_Absyn_DoubleType;
_temp162.f1= b; _temp162;}); _temp161;});} static struct Cyc_Absyn_Abs_n_struct
Cyc_Absyn_abs_null={ 1u, 0}; static unsigned char _temp164[ 4u]="exn"; static
struct _tagged_arr Cyc_Absyn_exn_str={ _temp164, _temp164, _temp164 +  4u};
static struct _tuple0 Cyc_Absyn_exn_name_v={( void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_exn_str};
struct _tuple0* Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v; static unsigned char
_temp165[ 15u]="Null_Exception"; static struct _tagged_arr Cyc_Absyn_Null_Exception_str={
_temp165, _temp165, _temp165 +  15u}; static struct _tuple0 Cyc_Absyn_Null_Exception_pr={(
void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Null_Exception_str}; struct _tuple0* Cyc_Absyn_Null_Exception_name=&
Cyc_Absyn_Null_Exception_pr; static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Null_Exception_tuf_v={&
Cyc_Absyn_Null_Exception_pr, 0, 0, 0,( void*)(( void*) 3u)}; struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v; static
unsigned char _temp166[ 13u]="Array_bounds"; static struct _tagged_arr Cyc_Absyn_Array_bounds_str={
_temp166, _temp166, _temp166 +  13u}; static struct _tuple0 Cyc_Absyn_Array_bounds_pr={(
void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Array_bounds_str}; struct _tuple0* Cyc_Absyn_Array_bounds_name=&
Cyc_Absyn_Array_bounds_pr; static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Array_bounds_tuf_v={&
Cyc_Absyn_Array_bounds_pr, 0, 0, 0,( void*)(( void*) 3u)}; struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v; static unsigned char
_temp167[ 16u]="Match_Exception"; static struct _tagged_arr Cyc_Absyn_Match_Exception_str={
_temp167, _temp167, _temp167 +  16u}; static struct _tuple0 Cyc_Absyn_Match_Exception_pr={(
void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Match_Exception_str}; struct _tuple0* Cyc_Absyn_Match_Exception_name=&
Cyc_Absyn_Match_Exception_pr; static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Match_Exception_tuf_v={&
Cyc_Absyn_Match_Exception_pr, 0, 0, 0,( void*)(( void*) 3u)}; struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Match_Exception_tuf=& Cyc_Absyn_Match_Exception_tuf_v; static
unsigned char _temp168[ 10u]="Bad_alloc"; static struct _tagged_arr Cyc_Absyn_Bad_alloc_str={
_temp168, _temp168, _temp168 +  10u}; static struct _tuple0 Cyc_Absyn_Bad_alloc_pr={(
void*)& Cyc_Absyn_abs_null,& Cyc_Absyn_Bad_alloc_str}; struct _tuple0* Cyc_Absyn_Bad_alloc_name=&
Cyc_Absyn_Bad_alloc_pr; static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Bad_alloc_tuf_v={&
Cyc_Absyn_Bad_alloc_pr, 0, 0, 0,( void*)(( void*) 3u)}; struct Cyc_Absyn_Tunionfield*
Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v; static struct Cyc_List_List
Cyc_Absyn_exn_l0={( void*)& Cyc_Absyn_Null_Exception_tuf_v, 0}; static struct
Cyc_List_List Cyc_Absyn_exn_l1={( void*)& Cyc_Absyn_Array_bounds_tuf_v,( struct
Cyc_List_List*)& Cyc_Absyn_exn_l0}; static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,( struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={( void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2}; static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)(( struct Cyc_List_List*)& Cyc_Absyn_exn_l3)}; static struct Cyc_Absyn_Tuniondecl
Cyc_Absyn_exn_tud_v={( void*)(( void*) 3u),& Cyc_Absyn_exn_name_v, 0,( struct
Cyc_Core_Opt*)& Cyc_Absyn_exn_ol, 1}; struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_exn_tud=&
Cyc_Absyn_exn_tud_v; static struct Cyc_Absyn_KnownTunion_struct Cyc_Absyn_exn_tinfou={
1u,& Cyc_Absyn_exn_tud}; static struct Cyc_Absyn_TunionType_struct Cyc_Absyn_exn_typ_tt={
2u,{( void*)(( void*)& Cyc_Absyn_exn_tinfou), 0,( void*)(( void*) 2u)}}; void*
Cyc_Absyn_exn_typ=( void*)& Cyc_Absyn_exn_typ_tt; static unsigned char _temp171[
9u]="PrintArg"; static struct _tagged_arr Cyc_Absyn_printarg_str={ _temp171,
_temp171, _temp171 +  9u}; static unsigned char _temp172[ 9u]="ScanfArg"; static
struct _tagged_arr Cyc_Absyn_scanfarg_str={ _temp172, _temp172, _temp172 +  9u};
static unsigned char _temp173[ 4u]="Cyc"; static struct _tagged_arr Cyc_Absyn_cyc_str={
_temp173, _temp173, _temp173 +  4u}; static unsigned char _temp174[ 4u]="Std";
static struct _tagged_arr Cyc_Absyn_std_str={ _temp174, _temp174, _temp174 +  4u};
static struct Cyc_List_List Cyc_Absyn_std_list={( void*)& Cyc_Absyn_std_str, 0};
static struct Cyc_Absyn_Abs_n_struct Cyc_Absyn_std_namespace={ 1u,( struct Cyc_List_List*)&
Cyc_Absyn_std_list}; static struct _tuple0 Cyc_Absyn_tunion_print_arg_qvar_p={(
void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_printarg_str}; static struct _tuple0
Cyc_Absyn_tunion_scanf_arg_qvar_p={( void*)& Cyc_Absyn_std_namespace,& Cyc_Absyn_scanfarg_str};
struct _tuple0* Cyc_Absyn_tunion_print_arg_qvar=& Cyc_Absyn_tunion_print_arg_qvar_p;
struct _tuple0* Cyc_Absyn_tunion_scanf_arg_qvar=& Cyc_Absyn_tunion_scanf_arg_qvar_p;
static struct Cyc_Core_Opt* Cyc_Absyn_string_t_opt= 0; void* Cyc_Absyn_string_typ(
void* rgn){ if( rgn != ( void*) Cyc_Absyn_HeapRgn){ return Cyc_Absyn_starb_typ(
Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b);}
else{ if( Cyc_Absyn_string_t_opt ==  0){ void* t= Cyc_Absyn_starb_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual(),( void*) Cyc_Absyn_Unknown_b);
Cyc_Absyn_string_t_opt=({ struct Cyc_Core_Opt* _temp176=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp176->v=( void*) t; _temp176;});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Absyn_string_t_opt))->v;}}
static struct Cyc_Core_Opt* Cyc_Absyn_const_string_t_opt= 0; void* Cyc_Absyn_const_string_typ(
void* rgn){ if( rgn != ( void*) Cyc_Absyn_HeapRgn){ return Cyc_Absyn_starb_typ(
Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b);}
else{ if( Cyc_Absyn_const_string_t_opt ==  0){ void* t= Cyc_Absyn_starb_typ( Cyc_Absyn_uchar_t,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b);
Cyc_Absyn_const_string_t_opt=({ struct Cyc_Core_Opt* _temp177=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp177->v=( void*) t; _temp177;});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( Cyc_Absyn_const_string_t_opt))->v;}}
static struct Cyc_Absyn_Int_c_struct Cyc_Absyn_one_intc={ 2u,( void*)(( void*) 1u),
1}; static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={ 0u,( void*)((
void*)& Cyc_Absyn_one_intc)}; struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={
0,( void*)(( void*)& Cyc_Absyn_one_b_raw), 0,( void*)(( void*) Cyc_Absyn_EmptyAnnot)};
struct Cyc_Absyn_Exp* Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;
static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={ 0u,& Cyc_Absyn_exp_unsigned_one_v};
void* Cyc_Absyn_bounds_one=( void*)& Cyc_Absyn_one_bt; void* Cyc_Absyn_starb_typ(
void* t, void* r, struct Cyc_Absyn_Tqual tq, void* b){ return( void*)({ struct
Cyc_Absyn_PointerType_struct* _temp181=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp181[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp182; _temp182.tag= Cyc_Absyn_PointerType;
_temp182.f1=({ struct Cyc_Absyn_PtrInfo _temp183; _temp183.elt_typ=( void*) t;
_temp183.rgn_typ=( void*) r; _temp183.nullable= Cyc_Absyn_true_conref; _temp183.tq=
tq; _temp183.bounds= Cyc_Absyn_new_conref( b); _temp183;}); _temp182;});
_temp181;});} void* Cyc_Absyn_atb_typ( void* t, void* r, struct Cyc_Absyn_Tqual
tq, void* b){ return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp184=(
struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp184[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp185; _temp185.tag= Cyc_Absyn_PointerType;
_temp185.f1=({ struct Cyc_Absyn_PtrInfo _temp186; _temp186.elt_typ=( void*) t;
_temp186.rgn_typ=( void*) r; _temp186.nullable= Cyc_Absyn_false_conref; _temp186.tq=
tq; _temp186.bounds= Cyc_Absyn_new_conref( b); _temp186;}); _temp185;});
_temp184;});} void* Cyc_Absyn_star_typ( void* t, void* r, struct Cyc_Absyn_Tqual
tq){ return Cyc_Absyn_starb_typ( t, r, tq, Cyc_Absyn_bounds_one);} void* Cyc_Absyn_cstar_typ(
void* t, struct Cyc_Absyn_Tqual tq){ return Cyc_Absyn_starb_typ( t,( void*) Cyc_Absyn_HeapRgn,
tq, Cyc_Absyn_bounds_one);} void* Cyc_Absyn_at_typ( void* t, void* r, struct Cyc_Absyn_Tqual
tq){ return Cyc_Absyn_atb_typ( t, r, tq, Cyc_Absyn_bounds_one);} void* Cyc_Absyn_tagged_typ(
void* t, void* r, struct Cyc_Absyn_Tqual tq){ return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp187=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp187[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp188; _temp188.tag= Cyc_Absyn_PointerType;
_temp188.f1=({ struct Cyc_Absyn_PtrInfo _temp189; _temp189.elt_typ=( void*) t;
_temp189.rgn_typ=( void*) r; _temp189.nullable= Cyc_Absyn_true_conref; _temp189.tq=
tq; _temp189.bounds= Cyc_Absyn_new_conref(( void*) Cyc_Absyn_Unknown_b);
_temp189;}); _temp188;}); _temp187;});} static struct Cyc_Core_Opt* Cyc_Absyn_file_t_opt=
0; static unsigned char _temp190[ 8u]="__sFILE"; static struct _tagged_arr Cyc_Absyn_sf_str={
_temp190, _temp190, _temp190 +  8u}; static struct _tagged_arr* Cyc_Absyn_sf=&
Cyc_Absyn_sf_str; static unsigned char _temp191[ 4u]="Std"; static struct
_tagged_arr Cyc_Absyn_st_str={ _temp191, _temp191, _temp191 +  4u}; static
struct _tagged_arr* Cyc_Absyn_st=& Cyc_Absyn_st_str; void* Cyc_Absyn_file_typ(){
if( Cyc_Absyn_file_t_opt ==  0){ struct _tuple0* file_t_name=({ struct _tuple0*
_temp198=( struct _tuple0*) _cycalloc( sizeof( struct _tuple0)); _temp198->f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp199=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp199[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp200; _temp200.tag= Cyc_Absyn_Abs_n; _temp200.f1=({ struct _tagged_arr*
_temp201[ 1u]; _temp201[ 0u]= Cyc_Absyn_st;(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp201, sizeof( struct _tagged_arr*),
1u));}); _temp200;}); _temp199;}); _temp198->f2= Cyc_Absyn_sf; _temp198;});
struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl* _temp196=(
struct Cyc_Absyn_Structdecl*) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp196->sc=( void*)(( void*) Cyc_Absyn_Abstract); _temp196->name=({ struct Cyc_Core_Opt*
_temp197=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp197->v=( void*) file_t_name; _temp197;}); _temp196->tvs= 0; _temp196->fields=
0; _temp196->attributes= 0; _temp196;}); void* file_struct_typ=( void*)({ struct
Cyc_Absyn_StructType_struct* _temp193=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp193[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp194; _temp194.tag= Cyc_Absyn_StructType;
_temp194.f1=( struct _tuple0*) file_t_name; _temp194.f2= 0; _temp194.f3=({
struct Cyc_Absyn_Structdecl** _temp195=( struct Cyc_Absyn_Structdecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl*) *  1); _temp195[ 0]= sd;
_temp195;}); _temp194;}); _temp193;}); Cyc_Absyn_file_t_opt=({ struct Cyc_Core_Opt*
_temp192=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp192->v=( void*) Cyc_Absyn_at_typ( file_struct_typ,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp192;});} return( void*)(( struct Cyc_Core_Opt*)
_check_null( Cyc_Absyn_file_t_opt))->v;} static struct Cyc_Core_Opt* Cyc_Absyn_void_star_t_opt=
0; void* Cyc_Absyn_void_star_typ(){ if( Cyc_Absyn_void_star_t_opt ==  0){ Cyc_Absyn_void_star_t_opt=({
struct Cyc_Core_Opt* _temp202=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp202->v=( void*) Cyc_Absyn_star_typ(( void*) Cyc_Absyn_VoidType,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _temp202;});} return( void*)((
struct Cyc_Core_Opt*) _check_null( Cyc_Absyn_void_star_t_opt))->v;} struct Cyc_Core_Opt*
Cyc_Absyn_void_star_typ_opt(){ if( Cyc_Absyn_void_star_t_opt ==  0){ Cyc_Absyn_void_star_t_opt=({
struct Cyc_Core_Opt* _temp203=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp203->v=( void*) Cyc_Absyn_star_typ(( void*) Cyc_Absyn_VoidType,(
void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _temp203;});} return Cyc_Absyn_void_star_t_opt;}
static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={ 20u, 0}; void* Cyc_Absyn_empty_effect=(
void*)& Cyc_Absyn_empty_eff; void* Cyc_Absyn_strct( struct _tagged_arr* name){
return( void*)({ struct Cyc_Absyn_StructType_struct* _temp205=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp205[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp206; _temp206.tag= Cyc_Absyn_StructType;
_temp206.f1=({ struct _tuple0* _temp207=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp207->f1= Cyc_Absyn_rel_ns_null; _temp207->f2= name;
_temp207;}); _temp206.f2= 0; _temp206.f3= 0; _temp206;}); _temp205;});} void*
Cyc_Absyn_union_typ( struct _tagged_arr* name){ return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp208=( struct Cyc_Absyn_UnionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp208[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp209; _temp209.tag= Cyc_Absyn_UnionType;
_temp209.f1=({ struct _tuple0* _temp210=( struct _tuple0*) _cycalloc( sizeof(
struct _tuple0)); _temp210->f1= Cyc_Absyn_rel_ns_null; _temp210->f2= name;
_temp210;}); _temp209.f2= 0; _temp209.f3= 0; _temp209;}); _temp208;});} void*
Cyc_Absyn_strctq( struct _tuple0* name){ return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp211=( struct Cyc_Absyn_StructType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp211[ 0]=({ struct Cyc_Absyn_StructType_struct _temp212; _temp212.tag= Cyc_Absyn_StructType;
_temp212.f1=( struct _tuple0*) name; _temp212.f2= 0; _temp212.f3= 0; _temp212;});
_temp211;});} void* Cyc_Absyn_unionq_typ( struct _tuple0* name){ return( void*)({
struct Cyc_Absyn_UnionType_struct* _temp213=( struct Cyc_Absyn_UnionType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp213[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp214; _temp214.tag= Cyc_Absyn_UnionType; _temp214.f1=(
struct _tuple0*) name; _temp214.f2= 0; _temp214.f3= 0; _temp214;}); _temp213;});}
struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void* r, struct Cyc_Position_Segment*
loc){ return({ struct Cyc_Absyn_Exp* _temp215=( struct Cyc_Absyn_Exp*) _cycalloc(
sizeof( struct Cyc_Absyn_Exp)); _temp215->topt= 0; _temp215->r=( void*) r;
_temp215->loc= loc; _temp215->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot);
_temp215;});} struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp( struct Cyc_Absyn_Exp*
rgn_handle, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return
Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_New_e_struct* _temp216=( struct
Cyc_Absyn_New_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_New_e_struct));
_temp216[ 0]=({ struct Cyc_Absyn_New_e_struct _temp217; _temp217.tag= Cyc_Absyn_New_e;
_temp217.f1= rgn_handle; _temp217.f2= e; _temp217;}); _temp216;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp* e){ return({ struct Cyc_Absyn_Exp*
_temp218=( struct Cyc_Absyn_Exp*) _cycalloc( sizeof( struct Cyc_Absyn_Exp));
_temp218->topt= e->topt; _temp218->r=( void*)(( void*) e->r); _temp218->loc= e->loc;
_temp218->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot); _temp218;});} struct
Cyc_Absyn_Exp* Cyc_Absyn_const_exp( void* c, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp219=(
struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp219[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp220; _temp220.tag= Cyc_Absyn_Const_e;
_temp220.f1=( void*) c; _temp220;}); _temp219;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp( struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Const_e_struct* _temp221=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp221[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp222; _temp222.tag= Cyc_Absyn_Const_e; _temp222.f1=( void*)(( void*) Cyc_Absyn_Null_c);
_temp222;}); _temp221;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*
s, int i, struct Cyc_Position_Segment* seg){ return Cyc_Absyn_const_exp(( void*)({
struct Cyc_Absyn_Int_c_struct* _temp223=( struct Cyc_Absyn_Int_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp223[ 0]=({ struct Cyc_Absyn_Int_c_struct
_temp224; _temp224.tag= Cyc_Absyn_Int_c; _temp224.f1=( void*) s; _temp224.f2= i;
_temp224;}); _temp223;}), seg);} struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int i, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed,
i, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int i, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Unsigned,( int) i, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_bool_exp( int b, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_signed_int_exp( b? 1: 0, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_bool_exp( 1, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment* loc){ return
Cyc_Absyn_bool_exp( 0, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp(
unsigned char c, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp((
void*)({ struct Cyc_Absyn_Char_c_struct* _temp225=( struct Cyc_Absyn_Char_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct)); _temp225[ 0]=({ struct Cyc_Absyn_Char_c_struct
_temp226; _temp226.tag= Cyc_Absyn_Char_c; _temp226.f1=( void*)(( void*) Cyc_Absyn_Unsigned);
_temp226.f2= c; _temp226;}); _temp225;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp(
struct _tagged_arr f, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp((
void*)({ struct Cyc_Absyn_Float_c_struct* _temp227=( struct Cyc_Absyn_Float_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp227[ 0]=({ struct Cyc_Absyn_Float_c_struct
_temp228; _temp228.tag= Cyc_Absyn_Float_c; _temp228.f1= f; _temp228;}); _temp227;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp( struct _tagged_arr s, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp(( void*)({ struct Cyc_Absyn_String_c_struct*
_temp229=( struct Cyc_Absyn_String_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_String_c_struct));
_temp229[ 0]=({ struct Cyc_Absyn_String_c_struct _temp230; _temp230.tag= Cyc_Absyn_String_c;
_temp230.f1= s; _temp230;}); _temp229;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp(
struct _tuple0* q, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp231=( struct Cyc_Absyn_Var_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp231[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp232; _temp232.tag= Cyc_Absyn_Var_e; _temp232.f1= q; _temp232.f2=( void*)((
void*) Cyc_Absyn_Unresolved_b); _temp232;}); _temp231;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp( struct _tuple0* q, void* b, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp233=(
struct Cyc_Absyn_Var_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp233[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp234; _temp234.tag= Cyc_Absyn_Var_e;
_temp234.f1= q; _temp234.f2=( void*) b; _temp234;}); _temp233;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp( struct _tuple0* q, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp235=( struct Cyc_Absyn_UnknownId_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp235[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp236; _temp236.tag= Cyc_Absyn_UnknownId_e;
_temp236.f1= q; _temp236;}); _temp235;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp(
void* p, struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp237=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp237[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp238; _temp238.tag= Cyc_Absyn_Primop_e; _temp238.f1=(
void*) p; _temp238.f2= es; _temp238;}); _temp237;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp( void* p, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp239=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp239->hd=( void*)
e; _temp239->tl= 0; _temp239;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp(
void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp240=( struct
Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp240->hd=( void*)
e1; _temp240->tl=({ struct Cyc_List_List* _temp241=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp241->hd=( void*) e2; _temp241->tl=
0; _temp241;}); _temp240;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Times,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Div, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Eq, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Neq,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Gt, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Lt, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gte,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Lte, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt* popt, struct Cyc_Absyn_Exp* e2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({ struct
Cyc_Absyn_AssignOp_e_struct* _temp242=( struct Cyc_Absyn_AssignOp_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_AssignOp_e_struct)); _temp242[ 0]=({ struct
Cyc_Absyn_AssignOp_e_struct _temp243; _temp243.tag= Cyc_Absyn_AssignOp_e;
_temp243.f1= e1; _temp243.f2= popt; _temp243.f3= e2; _temp243;}); _temp242;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_assignop_exp(
e1, 0, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp( struct Cyc_Absyn_Exp*
e, void* i, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp244=( struct Cyc_Absyn_Increment_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp244[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp245; _temp245.tag= Cyc_Absyn_Increment_e;
_temp245.f1= e; _temp245.f2=( void*) i; _temp245;}); _temp244;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PostInc, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PreInc, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PreDec, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PostDec, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Conditional_e_struct*
_temp246=( struct Cyc_Absyn_Conditional_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Conditional_e_struct));
_temp246[ 0]=({ struct Cyc_Absyn_Conditional_e_struct _temp247; _temp247.tag=
Cyc_Absyn_Conditional_e; _temp247.f1= e1; _temp247.f2= e2; _temp247.f3= e3;
_temp247;}); _temp246;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_conditional_exp( e1, e2, Cyc_Absyn_false_exp( loc), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_conditional_exp( e1, Cyc_Absyn_true_exp(
loc), e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_SeqExp_e_struct* _temp248=( struct Cyc_Absyn_SeqExp_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SeqExp_e_struct)); _temp248[ 0]=({ struct
Cyc_Absyn_SeqExp_e_struct _temp249; _temp249.tag= Cyc_Absyn_SeqExp_e; _temp249.f1=
e1; _temp249.f2= e2; _temp249;}); _temp248;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownCall_e_struct*
_temp250=( struct Cyc_Absyn_UnknownCall_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_UnknownCall_e_struct));
_temp250[ 0]=({ struct Cyc_Absyn_UnknownCall_e_struct _temp251; _temp251.tag=
Cyc_Absyn_UnknownCall_e; _temp251.f1= e; _temp251.f2= es; _temp251;}); _temp250;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp252=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp252[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp253; _temp253.tag= Cyc_Absyn_FnCall_e; _temp253.f1=
e; _temp253.f2= es; _temp253.f3= 0; _temp253;}); _temp252;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_NoInstantiate_e_struct*
_temp254=( struct Cyc_Absyn_NoInstantiate_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_NoInstantiate_e_struct)); _temp254[ 0]=({ struct Cyc_Absyn_NoInstantiate_e_struct
_temp255; _temp255.tag= Cyc_Absyn_NoInstantiate_e; _temp255.f1= e; _temp255;});
_temp254;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*
e, struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp256=( struct Cyc_Absyn_Instantiate_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp256[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp257; _temp257.tag= Cyc_Absyn_Instantiate_e;
_temp257.f1= e; _temp257.f2= ts; _temp257;}); _temp256;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp258=( struct Cyc_Absyn_Cast_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp258[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp259; _temp259.tag= Cyc_Absyn_Cast_e;
_temp259.f1=( void*) t; _temp259.f2= e; _temp259;}); _temp258;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_throw_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Throw_e_struct*
_temp260=( struct Cyc_Absyn_Throw_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Throw_e_struct));
_temp260[ 0]=({ struct Cyc_Absyn_Throw_e_struct _temp261; _temp261.tag= Cyc_Absyn_Throw_e;
_temp261.f1= e; _temp261;}); _temp260;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Address_e_struct* _temp262=( struct Cyc_Absyn_Address_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Address_e_struct)); _temp262[ 0]=({ struct
Cyc_Absyn_Address_e_struct _temp263; _temp263.tag= Cyc_Absyn_Address_e; _temp263.f1=
e; _temp263;}); _temp262;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Sizeoftyp_e_struct* _temp264=( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct)); _temp264[ 0]=({ struct
Cyc_Absyn_Sizeoftyp_e_struct _temp265; _temp265.tag= Cyc_Absyn_Sizeoftyp_e;
_temp265.f1=( void*) t; _temp265;}); _temp264;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Sizeofexp_e_struct*
_temp266=( struct Cyc_Absyn_Sizeofexp_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Sizeofexp_e_struct));
_temp266[ 0]=({ struct Cyc_Absyn_Sizeofexp_e_struct _temp267; _temp267.tag= Cyc_Absyn_Sizeofexp_e;
_temp267.f1= e; _temp267;}); _temp266;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_offsetof_exp(
void* t, void* of, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Offsetof_e_struct* _temp268=( struct Cyc_Absyn_Offsetof_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Offsetof_e_struct)); _temp268[ 0]=({ struct
Cyc_Absyn_Offsetof_e_struct _temp269; _temp269.tag= Cyc_Absyn_Offsetof_e;
_temp269.f1=( void*) t; _temp269.f2=( void*) of; _temp269;}); _temp268;}), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_gentyp_exp( struct Cyc_List_List* tvs, void* t,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({ struct
Cyc_Absyn_Gentyp_e_struct* _temp270=( struct Cyc_Absyn_Gentyp_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Gentyp_e_struct)); _temp270[ 0]=({ struct
Cyc_Absyn_Gentyp_e_struct _temp271; _temp271.tag= Cyc_Absyn_Gentyp_e; _temp271.f1=
tvs; _temp271.f2=( void*) t; _temp271;}); _temp270;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Deref_e_struct* _temp272=(
struct Cyc_Absyn_Deref_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Deref_e_struct));
_temp272[ 0]=({ struct Cyc_Absyn_Deref_e_struct _temp273; _temp273.tag= Cyc_Absyn_Deref_e;
_temp273.f1= e; _temp273;}); _temp272;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp(
struct Cyc_Absyn_Exp* e, struct _tagged_arr* n, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_StructMember_e_struct*
_temp274=( struct Cyc_Absyn_StructMember_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_StructMember_e_struct)); _temp274[ 0]=({ struct Cyc_Absyn_StructMember_e_struct
_temp275; _temp275.tag= Cyc_Absyn_StructMember_e; _temp275.f1= e; _temp275.f2= n;
_temp275;}); _temp274;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp* e, struct _tagged_arr* n, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_StructArrow_e_struct*
_temp276=( struct Cyc_Absyn_StructArrow_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_StructArrow_e_struct));
_temp276[ 0]=({ struct Cyc_Absyn_StructArrow_e_struct _temp277; _temp277.tag=
Cyc_Absyn_StructArrow_e; _temp277.f1= e; _temp277.f2= n; _temp277;}); _temp276;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_arrow_exp( struct Cyc_Absyn_Exp* e,
struct _tagged_arr* n, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_deref_exp(
Cyc_Absyn_structmember_exp( e, n, loc), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Subscript_e_struct*
_temp278=( struct Cyc_Absyn_Subscript_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Subscript_e_struct));
_temp278[ 0]=({ struct Cyc_Absyn_Subscript_e_struct _temp279; _temp279.tag= Cyc_Absyn_Subscript_e;
_temp279.f1= e1; _temp279.f2= e2; _temp279;}); _temp278;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp( struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Tuple_e_struct* _temp280=(
struct Cyc_Absyn_Tuple_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tuple_e_struct));
_temp280[ 0]=({ struct Cyc_Absyn_Tuple_e_struct _temp281; _temp281.tag= Cyc_Absyn_Tuple_e;
_temp281.f1= es; _temp281;}); _temp280;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_StmtExp_e_struct* _temp282=( struct Cyc_Absyn_StmtExp_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StmtExp_e_struct)); _temp282[ 0]=({ struct
Cyc_Absyn_StmtExp_e_struct _temp283; _temp283.tag= Cyc_Absyn_StmtExp_e; _temp283.f1=
s; _temp283;}); _temp282;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_match_exn_exp(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_var_exp( Cyc_Absyn_Match_Exception_name,
loc);} struct _tuple7{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_Absyn_Exp* Cyc_Absyn_array_exp( struct Cyc_List_List* es, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* dles= 0; for( 0; es !=  0; es= es->tl){ dles=({
struct Cyc_List_List* _temp284=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp284->hd=( void*)({ struct _tuple7* _temp285=(
struct _tuple7*) _cycalloc( sizeof( struct _tuple7)); _temp285->f1= 0; _temp285->f2=(
struct Cyc_Absyn_Exp*) es->hd; _temp285;}); _temp284->tl= dles; _temp284;});}
dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
dles); return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp286=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp286[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp287; _temp287.tag= Cyc_Absyn_Array_e;
_temp287.f1= dles; _temp287;}); _temp286;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt* n, struct Cyc_List_List* dles, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp288=( struct Cyc_Absyn_UnresolvedMem_e_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp288[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp289; _temp289.tag= Cyc_Absyn_UnresolvedMem_e; _temp289.f1= n; _temp289.f2=
dles; _temp289;}); _temp288;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt(
void* s, struct Cyc_Position_Segment* loc){ return({ struct Cyc_Absyn_Stmt*
_temp290=( struct Cyc_Absyn_Stmt*) _cycalloc( sizeof( struct Cyc_Absyn_Stmt));
_temp290->r=( void*) s; _temp290->loc= loc; _temp290->non_local_preds= 0;
_temp290->try_depth= 0; _temp290->annot=( void*)(( void*) Cyc_Absyn_EmptyAnnot);
_temp290;});} struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*) Cyc_Absyn_Skip_s, loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Exp_s_struct* _temp291=(
struct Cyc_Absyn_Exp_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Exp_s_struct));
_temp291[ 0]=({ struct Cyc_Absyn_Exp_s_struct _temp292; _temp292.tag= Cyc_Absyn_Exp_s;
_temp292.f1= e; _temp292;}); _temp291;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts(
struct Cyc_List_List* ss, struct Cyc_Position_Segment* loc){ if( ss ==  0){
return Cyc_Absyn_skip_stmt( loc);} else{ if( ss->tl ==  0){ return( struct Cyc_Absyn_Stmt*)
ss->hd;} else{ return Cyc_Absyn_seq_stmt(( struct Cyc_Absyn_Stmt*) ss->hd, Cyc_Absyn_seq_stmts(
ss->tl, loc), loc);}}} struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Return_s_struct* _temp293=( struct Cyc_Absyn_Return_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Return_s_struct)); _temp293[ 0]=({ struct
Cyc_Absyn_Return_s_struct _temp294; _temp294.tag= Cyc_Absyn_Return_s; _temp294.f1=
e; _temp294;}); _temp293;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_IfThenElse_s_struct* _temp295=( struct Cyc_Absyn_IfThenElse_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_IfThenElse_s_struct)); _temp295[ 0]=({
struct Cyc_Absyn_IfThenElse_s_struct _temp296; _temp296.tag= Cyc_Absyn_IfThenElse_s;
_temp296.f1= e; _temp296.f2= s1; _temp296.f3= s2; _temp296;}); _temp295;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt*
s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_While_s_struct* _temp297=( struct Cyc_Absyn_While_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_While_s_struct)); _temp297[ 0]=({ struct Cyc_Absyn_While_s_struct
_temp298; _temp298.tag= Cyc_Absyn_While_s; _temp298.f1=({ struct _tuple2
_temp299; _temp299.f1= e; _temp299.f2= Cyc_Absyn_skip_stmt( e->loc); _temp299;});
_temp298.f2= s; _temp298;}); _temp297;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_Break_s_struct* _temp300=( struct Cyc_Absyn_Break_s_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Break_s_struct)); _temp300[ 0]=({ struct Cyc_Absyn_Break_s_struct
_temp301; _temp301.tag= Cyc_Absyn_Break_s; _temp301.f1= 0; _temp301;}); _temp300;}),
loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Continue_s_struct*
_temp302=( struct Cyc_Absyn_Continue_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Continue_s_struct));
_temp302[ 0]=({ struct Cyc_Absyn_Continue_s_struct _temp303; _temp303.tag= Cyc_Absyn_Continue_s;
_temp303.f1= 0; _temp303;}); _temp302;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_For_s_struct* _temp304=( struct Cyc_Absyn_For_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_For_s_struct)); _temp304[ 0]=({ struct Cyc_Absyn_For_s_struct
_temp305; _temp305.tag= Cyc_Absyn_For_s; _temp305.f1= e1; _temp305.f2=({ struct
_tuple2 _temp306; _temp306.f1= e2; _temp306.f2= Cyc_Absyn_skip_stmt( e3->loc);
_temp306;}); _temp305.f3=({ struct _tuple2 _temp307; _temp307.f1= e3; _temp307.f2=
Cyc_Absyn_skip_stmt( e3->loc); _temp307;}); _temp305.f4= s; _temp305;});
_temp304;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Switch_s_struct* _temp308=( struct Cyc_Absyn_Switch_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Switch_s_struct)); _temp308[ 0]=({ struct
Cyc_Absyn_Switch_s_struct _temp309; _temp309.tag= Cyc_Absyn_Switch_s; _temp309.f1=
e; _temp309.f2= scs; _temp309;}); _temp308;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc){ void* _temp310=( void*) s1->r; _LL312: if( _temp310 == ( void*) Cyc_Absyn_Skip_s){
goto _LL313;} else{ goto _LL314;} _LL314: goto _LL315; _LL313: return s2; _LL315:
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Seq_s_struct* _temp316=(
struct Cyc_Absyn_Seq_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Seq_s_struct));
_temp316[ 0]=({ struct Cyc_Absyn_Seq_s_struct _temp317; _temp317.tag= Cyc_Absyn_Seq_s;
_temp317.f1= s1; _temp317.f2= s2; _temp317;}); _temp316;}), loc); _LL311:;}
struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt( struct Cyc_List_List* el, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Fallthru_s_struct*
_temp318=( struct Cyc_Absyn_Fallthru_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Fallthru_s_struct));
_temp318[ 0]=({ struct Cyc_Absyn_Fallthru_s_struct _temp319; _temp319.tag= Cyc_Absyn_Fallthru_s;
_temp319.f1= el; _temp319.f2= 0; _temp319;}); _temp318;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp320=( struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp320[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp321; _temp321.tag= Cyc_Absyn_Decl_s;
_temp321.f1= d; _temp321.f2= s; _temp321;}); _temp320;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Decl*
d= Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp324=(
struct Cyc_Absyn_Var_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp324[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp325; _temp325.tag= Cyc_Absyn_Var_d;
_temp325.f1= Cyc_Absyn_new_vardecl( x, t, init); _temp325;}); _temp324;}), loc);
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp322=(
struct Cyc_Absyn_Decl_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp322[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp323; _temp323.tag= Cyc_Absyn_Decl_s;
_temp323.f1= d; _temp323.f2= s; _temp323;}); _temp322;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct* _temp326=(
struct Cyc_Absyn_Cut_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp326[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp327; _temp327.tag= Cyc_Absyn_Cut_s;
_temp327.f1= s; _temp327;}); _temp326;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Splice_s_struct* _temp328=( struct Cyc_Absyn_Splice_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp328[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp329; _temp329.tag= Cyc_Absyn_Splice_s; _temp329.f1=
s; _temp329;}); _temp328;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt(
struct _tagged_arr* v, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp330=( struct Cyc_Absyn_Label_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp330[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp331; _temp331.tag= Cyc_Absyn_Label_s;
_temp331.f1= v; _temp331.f2= s; _temp331;}); _temp330;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Do_s_struct*
_temp332=( struct Cyc_Absyn_Do_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Do_s_struct));
_temp332[ 0]=({ struct Cyc_Absyn_Do_s_struct _temp333; _temp333.tag= Cyc_Absyn_Do_s;
_temp333.f1= s; _temp333.f2=({ struct _tuple2 _temp334; _temp334.f1= e; _temp334.f2=
Cyc_Absyn_skip_stmt( e->loc); _temp334;}); _temp333;}); _temp332;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt* s, struct
Cyc_List_List* scs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_TryCatch_s_struct* _temp335=( struct Cyc_Absyn_TryCatch_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TryCatch_s_struct)); _temp335[ 0]=({ struct
Cyc_Absyn_TryCatch_s_struct _temp336; _temp336.tag= Cyc_Absyn_TryCatch_s;
_temp336.f1= s; _temp336.f2= scs; _temp336;}); _temp335;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt( struct _tagged_arr* lab, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Goto_s_struct* _temp337=(
struct Cyc_Absyn_Goto_s_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Goto_s_struct));
_temp337[ 0]=({ struct Cyc_Absyn_Goto_s_struct _temp338; _temp338.tag= Cyc_Absyn_Goto_s;
_temp338.f1= lab; _temp338.f2= 0; _temp338;}); _temp337;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
e1, e2, loc), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_forarray_stmt( struct Cyc_List_List*
d, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_ForArray_s_struct* _temp339=( struct Cyc_Absyn_ForArray_s_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ForArray_s_struct)); _temp339[ 0]=({ struct
Cyc_Absyn_ForArray_s_struct _temp340; _temp340.tag= Cyc_Absyn_ForArray_s;
_temp340.f1=({ struct Cyc_Absyn_ForArrayInfo _temp341; _temp341.defns= d;
_temp341.condition=({ struct _tuple2 _temp343; _temp343.f1= e1; _temp343.f2= Cyc_Absyn_skip_stmt(
e1->loc); _temp343;}); _temp341.delta=({ struct _tuple2 _temp342; _temp342.f1=
e2; _temp342.f2= Cyc_Absyn_skip_stmt( e2->loc); _temp342;}); _temp341.body= s;
_temp341;}); _temp340;}); _temp339;}), loc);} struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat(
void* p, struct Cyc_Position_Segment* s){ return({ struct Cyc_Absyn_Pat*
_temp344=( struct Cyc_Absyn_Pat*) _cycalloc( sizeof( struct Cyc_Absyn_Pat));
_temp344->r=( void*) p; _temp344->topt= 0; _temp344->loc= s; _temp344;});}
struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc){ return({ struct Cyc_Absyn_Decl* _temp345=( struct Cyc_Absyn_Decl*)
_cycalloc( sizeof( struct Cyc_Absyn_Decl)); _temp345->r=( void*) r; _temp345->loc=
loc; _temp345;});} struct Cyc_Absyn_Decl* Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat*
p, struct Cyc_Core_Opt* t_opt, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Let_d_struct*
_temp346=( struct Cyc_Absyn_Let_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Let_d_struct));
_temp346[ 0]=({ struct Cyc_Absyn_Let_d_struct _temp347; _temp347.tag= Cyc_Absyn_Let_d;
_temp347.f1= p; _temp347.f2= 0; _temp347.f3= t_opt; _temp347.f4= e; _temp347.f5=
0; _temp347;}); _temp346;}), loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_letv_decl(
struct Cyc_List_List* vds, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Letv_d_struct* _temp348=( struct Cyc_Absyn_Letv_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Letv_d_struct)); _temp348[ 0]=({ struct Cyc_Absyn_Letv_d_struct
_temp349; _temp349.tag= Cyc_Absyn_Letv_d; _temp349.f1= vds; _temp349;});
_temp348;}), loc);} struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct
_tuple0* x, void* t, struct Cyc_Absyn_Exp* init){ return({ struct Cyc_Absyn_Vardecl*
_temp350=( struct Cyc_Absyn_Vardecl*) _cycalloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp350->sc=( void*)(( void*) Cyc_Absyn_Public); _temp350->name= x; _temp350->tq=
Cyc_Absyn_empty_tqual(); _temp350->type=( void*) t; _temp350->initializer= init;
_temp350->rgn= 0; _temp350->attributes= 0; _temp350->escapes= 0; _temp350;});}
struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl( struct _tuple0* x, void* t,
struct Cyc_Absyn_Exp* init){ return({ struct Cyc_Absyn_Vardecl* _temp351=(
struct Cyc_Absyn_Vardecl*) _cycalloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp351->sc=( void*)(( void*) Cyc_Absyn_Static); _temp351->name= x; _temp351->tq=
Cyc_Absyn_empty_tqual(); _temp351->type=( void*) t; _temp351->initializer= init;
_temp351->rgn= 0; _temp351->attributes= 0; _temp351->escapes= 0; _temp351;});}
struct Cyc_Absyn_Decl* Cyc_Absyn_struct_decl( void* s, struct Cyc_Core_Opt* n,
struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl(( void*)({ struct
Cyc_Absyn_Struct_d_struct* _temp352=( struct Cyc_Absyn_Struct_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp352[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp353; _temp353.tag= Cyc_Absyn_Struct_d; _temp353.f1=({
struct Cyc_Absyn_Structdecl* _temp354=( struct Cyc_Absyn_Structdecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl)); _temp354->sc=( void*) s; _temp354->name=
n; _temp354->tvs= ts; _temp354->fields= fs; _temp354->attributes= atts; _temp354;});
_temp353;}); _temp352;}), loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Union_d_struct* _temp355=( struct Cyc_Absyn_Union_d_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp355[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp356; _temp356.tag= Cyc_Absyn_Union_d; _temp356.f1=({ struct Cyc_Absyn_Uniondecl*
_temp357=( struct Cyc_Absyn_Uniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp357->sc=( void*) s; _temp357->name= n; _temp357->tvs= ts; _temp357->fields=
fs; _temp357->attributes= atts; _temp357;}); _temp356;}); _temp355;}), loc);}
struct Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl( void* s, struct _tuple0* n, struct
Cyc_List_List* ts, struct Cyc_Core_Opt* fs, int is_xtunion, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp358=( struct Cyc_Absyn_Tunion_d_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp358[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp359; _temp359.tag= Cyc_Absyn_Tunion_d;
_temp359.f1=({ struct Cyc_Absyn_Tuniondecl* _temp360=( struct Cyc_Absyn_Tuniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp360->sc=( void*) s;
_temp360->name= n; _temp360->tvs= ts; _temp360->fields= fs; _temp360->is_xtunion=
is_xtunion; _temp360;}); _temp359;}); _temp358;}), loc);} static struct _tuple1*
Cyc_Absyn_expand_arg( struct _tuple1* a){ return({ struct _tuple1* _temp361=(
struct _tuple1*) _cycalloc( sizeof( struct _tuple1)); _temp361->f1=(* a).f1;
_temp361->f2=(* a).f2; _temp361->f3= Cyc_Absyn_pointer_expand((* a).f3);
_temp361;});} void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct
Cyc_Core_Opt* eff_typ, void* ret_typ, struct Cyc_List_List* args, int c_varargs,
struct Cyc_Absyn_VarargInfo* cyc_varargs, struct Cyc_List_List* rgn_po, struct
Cyc_List_List* atts){ return( void*)({ struct Cyc_Absyn_FnType_struct* _temp362=(
struct Cyc_Absyn_FnType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp362[ 0]=({ struct Cyc_Absyn_FnType_struct _temp363; _temp363.tag= Cyc_Absyn_FnType;
_temp363.f1=({ struct Cyc_Absyn_FnInfo _temp364; _temp364.tvars= tvs; _temp364.ret_typ=(
void*) Cyc_Absyn_pointer_expand( ret_typ); _temp364.effect= eff_typ; _temp364.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absyn_expand_arg, args); _temp364.c_varargs= c_varargs;
_temp364.cyc_varargs= cyc_varargs; _temp364.rgn_po= rgn_po; _temp364.attributes=
atts; _temp364;}); _temp363;}); _temp362;});} void* Cyc_Absyn_pointer_expand(
void* t){ void* _temp365= Cyc_Tcutil_compress( t); _LL367: if(( unsigned int)
_temp365 >  3u?*(( int*) _temp365) ==  Cyc_Absyn_FnType: 0){ goto _LL368;} else{
goto _LL369;} _LL369: goto _LL370; _LL368: return Cyc_Absyn_at_typ( t,( void*)
Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual()); _LL370: return t; _LL366:;} int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp* e){ void* _temp371=( void*) e->r; void* _temp395; void*
_temp397; struct Cyc_Absyn_Vardecl* _temp399; void* _temp401; struct Cyc_Absyn_Vardecl*
_temp403; struct Cyc_Absyn_Exp* _temp405; struct Cyc_Absyn_Exp* _temp407; struct
_tagged_arr* _temp409; struct Cyc_Absyn_Exp* _temp411; struct _tagged_arr*
_temp413; struct Cyc_Absyn_Exp* _temp415; struct Cyc_Absyn_Exp* _temp417; struct
Cyc_Absyn_Exp* _temp419; struct Cyc_Absyn_Exp* _temp421; _LL373: if(*(( int*)
_temp371) ==  Cyc_Absyn_Var_e){ _LL396: _temp395=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp371)->f2; if(( unsigned int) _temp395 >  1u?*(( int*) _temp395) ==  Cyc_Absyn_Funname_b:
0){ goto _LL374;} else{ goto _LL375;}} else{ goto _LL375;} _LL375: if(*(( int*)
_temp371) ==  Cyc_Absyn_Var_e){ _LL398: _temp397=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp371)->f2; if(( unsigned int) _temp397 >  1u?*(( int*) _temp397) ==  Cyc_Absyn_Global_b:
0){ _LL400: _temp399=(( struct Cyc_Absyn_Global_b_struct*) _temp397)->f1; goto
_LL376;} else{ goto _LL377;}} else{ goto _LL377;} _LL377: if(*(( int*) _temp371)
==  Cyc_Absyn_Var_e){ _LL402: _temp401=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp371)->f2; if(( unsigned int) _temp401 >  1u?*(( int*) _temp401) ==  Cyc_Absyn_Local_b:
0){ _LL404: _temp403=(( struct Cyc_Absyn_Local_b_struct*) _temp401)->f1; goto
_LL378;} else{ goto _LL379;}} else{ goto _LL379;} _LL379: if(*(( int*) _temp371)
==  Cyc_Absyn_Var_e){ goto _LL380;} else{ goto _LL381;} _LL381: if(*(( int*)
_temp371) ==  Cyc_Absyn_Subscript_e){ _LL408: _temp407=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp371)->f1; goto _LL406; _LL406: _temp405=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp371)->f2; goto _LL382;} else{ goto _LL383;} _LL383: if(*(( int*) _temp371)
==  Cyc_Absyn_StructMember_e){ _LL412: _temp411=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp371)->f1; goto _LL410; _LL410: _temp409=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp371)->f2; goto _LL384;} else{ goto _LL385;} _LL385: if(*(( int*) _temp371)
==  Cyc_Absyn_StructArrow_e){ _LL416: _temp415=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp371)->f1; goto _LL414; _LL414: _temp413=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp371)->f2; goto _LL386;} else{ goto _LL387;} _LL387: if(*(( int*) _temp371)
==  Cyc_Absyn_Deref_e){ _LL418: _temp417=(( struct Cyc_Absyn_Deref_e_struct*)
_temp371)->f1; goto _LL388;} else{ goto _LL389;} _LL389: if(*(( int*) _temp371)
==  Cyc_Absyn_Instantiate_e){ _LL420: _temp419=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp371)->f1; goto _LL390;} else{ goto _LL391;} _LL391: if(*(( int*) _temp371)
==  Cyc_Absyn_NoInstantiate_e){ _LL422: _temp421=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp371)->f1; goto _LL392;} else{ goto _LL393;} _LL393: goto _LL394; _LL374:
return 0; _LL376: _temp403= _temp399; goto _LL378; _LL378: { void* _temp423= Cyc_Tcutil_compress((
void*) _temp403->type); _LL425: if(( unsigned int) _temp423 >  3u?*(( int*)
_temp423) ==  Cyc_Absyn_ArrayType: 0){ goto _LL426;} else{ goto _LL427;} _LL427:
goto _LL428; _LL426: return 0; _LL428: return 1; _LL424:;} _LL380: return 1;
_LL382: return 1; _LL384: return Cyc_Absyn_is_lvalue( _temp411); _LL386: return
1; _LL388: return 1; _LL390: return Cyc_Absyn_is_lvalue( _temp419); _LL392:
return Cyc_Absyn_is_lvalue( _temp421); _LL394: return 0; _LL372:;} struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_field( struct Cyc_List_List* fields, struct _tagged_arr* v){{
struct Cyc_List_List* fs= fields; for( 0; fs !=  0; fs= fs->tl){ struct Cyc_Absyn_Structfield*
_temp429=( struct Cyc_Absyn_Structfield*) fs->hd; if( Cyc_Std_zstrptrcmp(
_temp429->name, v) ==  0){ return( struct Cyc_Absyn_Structfield*) _temp429;}}}
return 0;} struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field( struct
Cyc_Absyn_Structdecl* sd, struct _tagged_arr* v){ return sd->fields ==  0? 0:
Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( sd->fields))->v, v);} struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_union_field(
struct Cyc_Absyn_Uniondecl* ud, struct _tagged_arr* v){ return ud->fields ==  0?
0: Cyc_Absyn_lookup_field(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ud->fields))->v, v);} struct _tuple3* Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List* ts, int i){ for( 0; i !=  0; -- i){ if( ts ==  0){ return
0;} ts= ts->tl;} if( ts ==  0){ return 0;} return( struct _tuple3*)(( struct
_tuple3*) ts->hd);} struct _tagged_arr Cyc_Absyn_attribute2string( void* a){
void* _temp430= a; int _temp474; int _temp476; struct _tagged_arr _temp478; int
_temp480; int _temp482; void* _temp484; int _temp486; int _temp488; void*
_temp490; _LL432: if(( unsigned int) _temp430 >  16u?*(( int*) _temp430) ==  Cyc_Absyn_Regparm_att:
0){ _LL475: _temp474=(( struct Cyc_Absyn_Regparm_att_struct*) _temp430)->f1;
goto _LL433;} else{ goto _LL434;} _LL434: if( _temp430 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL435;} else{ goto _LL436;} _LL436: if( _temp430 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL437;} else{ goto _LL438;} _LL438: if( _temp430 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL439;} else{ goto _LL440;} _LL440: if( _temp430 == ( void*) Cyc_Absyn_Noreturn_att){
goto _LL441;} else{ goto _LL442;} _LL442: if( _temp430 == ( void*) Cyc_Absyn_Const_att){
goto _LL443;} else{ goto _LL444;} _LL444: if(( unsigned int) _temp430 >  16u?*((
int*) _temp430) ==  Cyc_Absyn_Aligned_att: 0){ _LL477: _temp476=(( struct Cyc_Absyn_Aligned_att_struct*)
_temp430)->f1; goto _LL445;} else{ goto _LL446;} _LL446: if( _temp430 == ( void*)
Cyc_Absyn_Packed_att){ goto _LL447;} else{ goto _LL448;} _LL448: if((
unsigned int) _temp430 >  16u?*(( int*) _temp430) ==  Cyc_Absyn_Section_att: 0){
_LL479: _temp478=(( struct Cyc_Absyn_Section_att_struct*) _temp430)->f1; goto
_LL449;} else{ goto _LL450;} _LL450: if( _temp430 == ( void*) Cyc_Absyn_Nocommon_att){
goto _LL451;} else{ goto _LL452;} _LL452: if( _temp430 == ( void*) Cyc_Absyn_Shared_att){
goto _LL453;} else{ goto _LL454;} _LL454: if( _temp430 == ( void*) Cyc_Absyn_Unused_att){
goto _LL455;} else{ goto _LL456;} _LL456: if( _temp430 == ( void*) Cyc_Absyn_Weak_att){
goto _LL457;} else{ goto _LL458;} _LL458: if( _temp430 == ( void*) Cyc_Absyn_Dllimport_att){
goto _LL459;} else{ goto _LL460;} _LL460: if( _temp430 == ( void*) Cyc_Absyn_Dllexport_att){
goto _LL461;} else{ goto _LL462;} _LL462: if( _temp430 == ( void*) Cyc_Absyn_No_instrument_function_att){
goto _LL463;} else{ goto _LL464;} _LL464: if( _temp430 == ( void*) Cyc_Absyn_Constructor_att){
goto _LL465;} else{ goto _LL466;} _LL466: if( _temp430 == ( void*) Cyc_Absyn_Destructor_att){
goto _LL467;} else{ goto _LL468;} _LL468: if( _temp430 == ( void*) Cyc_Absyn_No_check_memory_usage_att){
goto _LL469;} else{ goto _LL470;} _LL470: if(( unsigned int) _temp430 >  16u?*((
int*) _temp430) ==  Cyc_Absyn_Format_att: 0){ _LL485: _temp484=( void*)(( struct
Cyc_Absyn_Format_att_struct*) _temp430)->f1; if( _temp484 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL483;} else{ goto _LL472;} _LL483: _temp482=(( struct Cyc_Absyn_Format_att_struct*)
_temp430)->f2; goto _LL481; _LL481: _temp480=(( struct Cyc_Absyn_Format_att_struct*)
_temp430)->f3; goto _LL471;} else{ goto _LL472;} _LL472: if(( unsigned int)
_temp430 >  16u?*(( int*) _temp430) ==  Cyc_Absyn_Format_att: 0){ _LL491:
_temp490=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp430)->f1; if(
_temp490 == ( void*) Cyc_Absyn_Scanf_ft){ goto _LL489;} else{ goto _LL431;}
_LL489: _temp488=(( struct Cyc_Absyn_Format_att_struct*) _temp430)->f2; goto
_LL487; _LL487: _temp486=(( struct Cyc_Absyn_Format_att_struct*) _temp430)->f3;
goto _LL473;} else{ goto _LL431;} _LL433: return( struct _tagged_arr)({ struct
Cyc_Std_Int_pa_struct _temp493; _temp493.tag= Cyc_Std_Int_pa; _temp493.f1=( int)((
unsigned int) _temp474);{ void* _temp492[ 1u]={& _temp493}; Cyc_Std_aprintf(
_tag_arr("regparm(%d)", sizeof( unsigned char), 12u), _tag_arr( _temp492,
sizeof( void*), 1u));}}); _LL435: return _tag_arr("stdcall", sizeof(
unsigned char), 8u); _LL437: return _tag_arr("cdecl", sizeof( unsigned char), 6u);
_LL439: return _tag_arr("fastcall", sizeof( unsigned char), 9u); _LL441: return
_tag_arr("noreturn", sizeof( unsigned char), 9u); _LL443: return _tag_arr("const",
sizeof( unsigned char), 6u); _LL445: if( _temp476 ==  - 1){ return _tag_arr("aligned",
sizeof( unsigned char), 8u);} else{ return( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp495; _temp495.tag= Cyc_Std_Int_pa; _temp495.f1=( int)(( unsigned int)
_temp476);{ void* _temp494[ 1u]={& _temp495}; Cyc_Std_aprintf( _tag_arr("aligned(%d)",
sizeof( unsigned char), 12u), _tag_arr( _temp494, sizeof( void*), 1u));}});}
_LL447: return _tag_arr("packed", sizeof( unsigned char), 7u); _LL449: return(
struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp497; _temp497.tag=
Cyc_Std_String_pa; _temp497.f1=( struct _tagged_arr) _temp478;{ void* _temp496[
1u]={& _temp497}; Cyc_Std_aprintf( _tag_arr("section(\"%s\")", sizeof(
unsigned char), 14u), _tag_arr( _temp496, sizeof( void*), 1u));}}); _LL451:
return _tag_arr("nocommon", sizeof( unsigned char), 9u); _LL453: return _tag_arr("shared",
sizeof( unsigned char), 7u); _LL455: return _tag_arr("unused", sizeof(
unsigned char), 7u); _LL457: return _tag_arr("weak", sizeof( unsigned char), 5u);
_LL459: return _tag_arr("dllimport", sizeof( unsigned char), 10u); _LL461:
return _tag_arr("dllexport", sizeof( unsigned char), 10u); _LL463: return
_tag_arr("no_instrument_function", sizeof( unsigned char), 23u); _LL465: return
_tag_arr("constructor", sizeof( unsigned char), 12u); _LL467: return _tag_arr("destructor",
sizeof( unsigned char), 11u); _LL469: return _tag_arr("no_check_memory_usage",
sizeof( unsigned char), 22u); _LL471: return( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp500; _temp500.tag= Cyc_Std_Int_pa; _temp500.f1=( unsigned int) _temp480;{
struct Cyc_Std_Int_pa_struct _temp499; _temp499.tag= Cyc_Std_Int_pa; _temp499.f1=(
unsigned int) _temp482;{ void* _temp498[ 2u]={& _temp499,& _temp500}; Cyc_Std_aprintf(
_tag_arr("format(printf,%u,%u)", sizeof( unsigned char), 21u), _tag_arr(
_temp498, sizeof( void*), 2u));}}}); _LL473: return( struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _temp503; _temp503.tag= Cyc_Std_Int_pa; _temp503.f1=(
unsigned int) _temp486;{ struct Cyc_Std_Int_pa_struct _temp502; _temp502.tag=
Cyc_Std_Int_pa; _temp502.f1=( unsigned int) _temp488;{ void* _temp501[ 2u]={&
_temp502,& _temp503}; Cyc_Std_aprintf( _tag_arr("format(scanf,%u,%u)", sizeof(
unsigned char), 20u), _tag_arr( _temp501, sizeof( void*), 2u));}}}); _LL431:;}
int Cyc_Absyn_fntype_att( void* a){ void* _temp504= a; _LL506: if(( unsigned int)
_temp504 >  16u?*(( int*) _temp504) ==  Cyc_Absyn_Regparm_att: 0){ goto _LL507;}
else{ goto _LL508;} _LL508: if( _temp504 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL509;} else{ goto _LL510;} _LL510: if( _temp504 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL511;} else{ goto _LL512;} _LL512: if( _temp504 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL513;} else{ goto _LL514;} _LL514: if( _temp504 == ( void*) Cyc_Absyn_Noreturn_att){
goto _LL515;} else{ goto _LL516;} _LL516: if(( unsigned int) _temp504 >  16u?*((
int*) _temp504) ==  Cyc_Absyn_Format_att: 0){ goto _LL517;} else{ goto _LL518;}
_LL518: if( _temp504 == ( void*) Cyc_Absyn_Const_att){ goto _LL519;} else{ goto
_LL520;} _LL520: goto _LL521; _LL507: goto _LL509; _LL509: goto _LL511; _LL511:
goto _LL513; _LL513: goto _LL515; _LL515: goto _LL517; _LL517: goto _LL519;
_LL519: return 1; _LL521: return 0; _LL505:;} static unsigned char _temp522[ 3u]="f0";
static struct _tagged_arr Cyc_Absyn_f0={ _temp522, _temp522, _temp522 +  3u};
static struct _tagged_arr* Cyc_Absyn_field_names_v[ 1u]={& Cyc_Absyn_f0}; static
struct _tagged_arr Cyc_Absyn_field_names={( void*)(( struct _tagged_arr**) Cyc_Absyn_field_names_v),(
void*)(( struct _tagged_arr**) Cyc_Absyn_field_names_v),( void*)(( struct
_tagged_arr**) Cyc_Absyn_field_names_v +  1u)}; struct _tagged_arr* Cyc_Absyn_fieldname(
int i){ unsigned int fsz= _get_arr_size( Cyc_Absyn_field_names, sizeof( struct
_tagged_arr*)); if( i >=  fsz){ Cyc_Absyn_field_names=({ unsigned int _temp523=(
unsigned int)( i +  1); struct _tagged_arr** _temp524=( struct _tagged_arr**)
_cycalloc( _check_times( sizeof( struct _tagged_arr*), _temp523)); struct
_tagged_arr _temp529= _tag_arr( _temp524, sizeof( struct _tagged_arr*),(
unsigned int)( i +  1));{ unsigned int _temp525= _temp523; unsigned int j; for(
j= 0; j <  _temp525; j ++){ _temp524[ j]= j <  fsz?*(( struct _tagged_arr**)
_check_unknown_subscript( Cyc_Absyn_field_names, sizeof( struct _tagged_arr*),(
int) j)):({ struct _tagged_arr* _temp526=( struct _tagged_arr*) _cycalloc(
sizeof( struct _tagged_arr)); _temp526[ 0]=( struct _tagged_arr)({ struct Cyc_Std_Int_pa_struct
_temp528; _temp528.tag= Cyc_Std_Int_pa; _temp528.f1=( int) j;{ void* _temp527[ 1u]={&
_temp528}; Cyc_Std_aprintf( _tag_arr("f%d", sizeof( unsigned char), 4u),
_tag_arr( _temp527, sizeof( void*), 1u));}}); _temp526;});}}; _temp529;});}
return*(( struct _tagged_arr**) _check_unknown_subscript( Cyc_Absyn_field_names,
sizeof( struct _tagged_arr*), i));} void Cyc_Absyn_print_decls( struct Cyc_List_List*
decls){(( void(*)( void* rep, struct Cyc_List_List** val)) Cyc_Marshal_print_type)(
Cyc_decls_rep,& decls);({ void* _temp530[ 0u]={}; Cyc_Std_printf( _tag_arr("\n",
sizeof( unsigned char), 2u), _tag_arr( _temp530, sizeof( void*), 0u));});}
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0Absyn_decl_t46H2_rep; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Decl_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep; extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_306; static struct Cyc_Typerep_Int_struct
Cyc__genrep_4={ 1u, 32}; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_99;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Vardecl_rep; extern
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep; static struct _tuple4*
Cyc__genarr_103[ 0u]={}; struct Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_103),( void*)(( struct _tuple4**)
Cyc__genarr_103),( void*)(( struct _tuple4**) Cyc__genarr_103 +  0u)}}; extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_9; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_10; extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_14; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_15; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_var_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11; extern struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_12; static struct Cyc_Typerep_Int_struct Cyc__genrep_13={ 1u, 8};
static struct Cyc_Typerep_FatPtr_struct Cyc__genrep_12={ 3u,( void*)(( void*)&
Cyc__genrep_13)}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11={ 2u,
1,( void*)(( void*)& Cyc__genrep_12)}; static struct _tuple4 Cyc__gentuple_16={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_11}; static struct
_tuple4 Cyc__gentuple_17={ offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_15};
static struct _tuple4* Cyc__genarr_18[ 2u]={& Cyc__gentuple_16,& Cyc__gentuple_17};
struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_var_t46H2_rep={ 4u,
sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_18),(
void*)(( struct _tuple4**) Cyc__genarr_18),( void*)(( struct _tuple4**) Cyc__genarr_18
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_15={ 2u, 1,( void*)((
void*)& Cyc_struct_List_List0Absyn_var_t46H2_rep)}; struct _tuple8{ unsigned int
f1; struct Cyc_List_List* f2; } ; static struct _tuple4 Cyc__gentuple_19={
offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_20={
offsetof( struct _tuple8,f2),( void*)& Cyc__genrep_15}; static struct _tuple4*
Cyc__genarr_21[ 2u]={& Cyc__gentuple_19,& Cyc__gentuple_20}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_14={ 4u, sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_21),(
void*)(( struct _tuple4**) Cyc__genarr_21),( void*)(( struct _tuple4**) Cyc__genarr_21
+  2u)}}; static struct _tuple4 Cyc__gentuple_22={ 0,( void*)& Cyc__genrep_14};
static struct _tuple4 Cyc__gentuple_23={ 1,( void*)& Cyc__genrep_14}; static
struct _tuple4* Cyc__genarr_24[ 2u]={& Cyc__gentuple_22,& Cyc__gentuple_23};
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep={ 5u,{( void*)(( struct
_tuple4**) Cyc__genarr_24),( void*)(( struct _tuple4**) Cyc__genarr_24),( void*)((
struct _tuple4**) Cyc__genarr_24 +  2u)}}; static struct _tuple4 Cyc__gentuple_25={
offsetof( struct _tuple0,f1),( void*)& Cyc_Absyn_nmspace_t_rep}; static struct
_tuple4 Cyc__gentuple_26={ offsetof( struct _tuple0,f2),( void*)& Cyc__genrep_11};
static struct _tuple4* Cyc__genarr_27[ 2u]={& Cyc__gentuple_25,& Cyc__gentuple_26};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_10={ 4u, sizeof( struct
_tuple0),{( void*)(( struct _tuple4**) Cyc__genarr_27),( void*)(( struct _tuple4**)
Cyc__genarr_27),( void*)(( struct _tuple4**) Cyc__genarr_27 +  2u)}}; static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_9={ 2u, 1,( void*)(( void*)& Cyc__genrep_10)};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_100; struct _tuple9{
unsigned char f1; } ; static struct _tuple4 Cyc__gentuple_101={ offsetof( struct
_tuple9,f1),( void*)(( void*)& Cyc__genrep_13)}; static struct _tuple4* Cyc__genarr_102[
1u]={& Cyc__gentuple_101}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_100={
4u, sizeof( struct _tuple9),{( void*)(( struct _tuple4**) Cyc__genarr_102),(
void*)(( struct _tuple4**) Cyc__genarr_102),( void*)(( struct _tuple4**) Cyc__genarr_102
+  1u)}}; extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep; extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_925; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_929; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep; static struct
_tuple4* Cyc__genarr_139[ 0u]={}; struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_139),( void*)(( struct _tuple4**)
Cyc__genarr_139),( void*)(( struct _tuple4**) Cyc__genarr_139 +  0u)}}; static
struct _tuple4 Cyc__gentuple_930={ offsetof( struct Cyc_Core_Opt,v),( void*)&
Cyc_Absyn_kind_t_rep}; static struct _tuple4* Cyc__genarr_931[ 1u]={& Cyc__gentuple_930};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep={ 4u,
sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_931),(
void*)(( struct _tuple4**) Cyc__genarr_931),( void*)(( struct _tuple4**) Cyc__genarr_931
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_929={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt0Absyn_kind_t2_rep)}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_52; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0Absyn_type_t2_rep;
static struct _tuple4 Cyc__gentuple_53={ offsetof( struct Cyc_Core_Opt,v),( void*)((
void*)& Cyc_Absyn_type_t_rep)}; static struct _tuple4* Cyc__genarr_54[ 1u]={&
Cyc__gentuple_53}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0Absyn_type_t2_rep={
4u, sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_54),(
void*)(( struct _tuple4**) Cyc__genarr_54),( void*)(( struct _tuple4**) Cyc__genarr_54
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_52={ 2u, 1,( void*)((
void*)& Cyc_struct_Core_Opt0Absyn_type_t2_rep)}; static struct Cyc_Typerep_Int_struct
Cyc__genrep_74={ 1u, 32}; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_926;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_194; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_137; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Tvar_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_159; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_159={ 2u, 1,( void*)(( void*)& Cyc__genrep_74)}; extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kindbound_t_rep; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_151;
static struct _tuple4 Cyc__gentuple_152={ offsetof( struct _tuple4,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_153={ offsetof( struct
_tuple4,f2),( void*)& Cyc_Absyn_kind_t_rep}; static struct _tuple4* Cyc__genarr_154[
2u]={& Cyc__gentuple_152,& Cyc__gentuple_153}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_151={ 4u, sizeof( struct _tuple4),{( void*)(( struct _tuple4**) Cyc__genarr_154),(
void*)(( struct _tuple4**) Cyc__genarr_154),( void*)(( struct _tuple4**) Cyc__genarr_154
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_147; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_140; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep; static struct _tuple4 Cyc__gentuple_141={
offsetof( struct Cyc_Core_Opt,v),( void*)& Cyc_Absyn_kindbound_t_rep}; static
struct _tuple4* Cyc__genarr_142[ 1u]={& Cyc__gentuple_141}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep={ 4u, sizeof( struct Cyc_Core_Opt),{(
void*)(( struct _tuple4**) Cyc__genarr_142),( void*)(( struct _tuple4**) Cyc__genarr_142),(
void*)(( struct _tuple4**) Cyc__genarr_142 +  1u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_140={ 2u, 1,( void*)(( void*)& Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep)};
struct _tuple10{ unsigned int f1; struct Cyc_Core_Opt* f2; } ; static struct
_tuple4 Cyc__gentuple_148={ offsetof( struct _tuple10,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_149={ offsetof( struct _tuple10,f2),( void*)&
Cyc__genrep_140}; static struct _tuple4* Cyc__genarr_150[ 2u]={& Cyc__gentuple_148,&
Cyc__gentuple_149}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_147={ 4u,
sizeof( struct _tuple10),{( void*)(( struct _tuple4**) Cyc__genarr_150),( void*)((
struct _tuple4**) Cyc__genarr_150),( void*)(( struct _tuple4**) Cyc__genarr_150
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_138; struct
_tuple11{ unsigned int f1; struct Cyc_Core_Opt* f2; void* f3; } ; static struct
_tuple4 Cyc__gentuple_143={ offsetof( struct _tuple11,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_144={ offsetof( struct _tuple11,f2),( void*)&
Cyc__genrep_140}; static struct _tuple4 Cyc__gentuple_145={ offsetof( struct
_tuple11,f3),( void*)& Cyc_Absyn_kind_t_rep}; static struct _tuple4* Cyc__genarr_146[
3u]={& Cyc__gentuple_143,& Cyc__gentuple_144,& Cyc__gentuple_145}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_138={ 4u, sizeof( struct _tuple11),{( void*)((
struct _tuple4**) Cyc__genarr_146),( void*)(( struct _tuple4**) Cyc__genarr_146),(
void*)(( struct _tuple4**) Cyc__genarr_146 +  3u)}}; static struct _tuple4 Cyc__gentuple_155={
0,( void*)& Cyc__genrep_151}; static struct _tuple4 Cyc__gentuple_156={ 1,( void*)&
Cyc__genrep_147}; static struct _tuple4 Cyc__gentuple_157={ 2,( void*)& Cyc__genrep_138};
static struct _tuple4* Cyc__genarr_158[ 3u]={& Cyc__gentuple_155,& Cyc__gentuple_156,&
Cyc__gentuple_157}; struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_158),( void*)(( struct _tuple4**)
Cyc__genarr_158),( void*)(( struct _tuple4**) Cyc__genarr_158 +  3u)}}; static
struct _tuple4 Cyc__gentuple_160={ offsetof( struct Cyc_Absyn_Tvar,name),( void*)&
Cyc__genrep_11}; static struct _tuple4 Cyc__gentuple_161={ offsetof( struct Cyc_Absyn_Tvar,identity),(
void*)& Cyc__genrep_159}; static struct _tuple4 Cyc__gentuple_162={ offsetof(
struct Cyc_Absyn_Tvar,kind),( void*)& Cyc_Absyn_kindbound_t_rep}; static struct
_tuple4* Cyc__genarr_163[ 3u]={& Cyc__gentuple_160,& Cyc__gentuple_161,& Cyc__gentuple_162};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Tvar_rep={ 4u, sizeof( struct
Cyc_Absyn_Tvar),{( void*)(( struct _tuple4**) Cyc__genarr_163),( void*)(( struct
_tuple4**) Cyc__genarr_163),( void*)(( struct _tuple4**) Cyc__genarr_163 +  3u)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_137={ 2u, 1,( void*)(( void*)&
Cyc_struct_Absyn_Tvar_rep)}; static struct _tuple4 Cyc__gentuple_195={ offsetof(
struct Cyc_List_List,hd),( void*)& Cyc__genrep_137}; static struct _tuple4 Cyc__gentuple_196={
offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_194}; static struct
_tuple4* Cyc__genarr_197[ 2u]={& Cyc__gentuple_195,& Cyc__gentuple_196}; struct
Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_tvar_t46H2_rep={ 4u, sizeof(
struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_197),( void*)((
struct _tuple4**) Cyc__genarr_197),( void*)(( struct _tuple4**) Cyc__genarr_197
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_194={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List0Absyn_tvar_t46H2_rep)}; static struct
_tuple4 Cyc__gentuple_927={ offsetof( struct Cyc_Core_Opt,v),( void*)& Cyc__genrep_194};
static struct _tuple4* Cyc__genarr_928[ 1u]={& Cyc__gentuple_927}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep={ 4u, sizeof( struct Cyc_Core_Opt),{(
void*)(( struct _tuple4**) Cyc__genarr_928),( void*)(( struct _tuple4**) Cyc__genarr_928),(
void*)(( struct _tuple4**) Cyc__genarr_928 +  1u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_926={ 2u, 1,( void*)(( void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep)};
struct _tuple12{ unsigned int f1; struct Cyc_Core_Opt* f2; struct Cyc_Core_Opt*
f3; int f4; struct Cyc_Core_Opt* f5; } ; static struct _tuple4 Cyc__gentuple_932={
offsetof( struct _tuple12,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_933={ offsetof( struct _tuple12,f2),( void*)& Cyc__genrep_929};
static struct _tuple4 Cyc__gentuple_934={ offsetof( struct _tuple12,f3),( void*)&
Cyc__genrep_52}; static struct _tuple4 Cyc__gentuple_935={ offsetof( struct
_tuple12,f4),( void*)(( void*)& Cyc__genrep_74)}; static struct _tuple4 Cyc__gentuple_936={
offsetof( struct _tuple12,f5),( void*)& Cyc__genrep_926}; static struct _tuple4*
Cyc__genarr_937[ 5u]={& Cyc__gentuple_932,& Cyc__gentuple_933,& Cyc__gentuple_934,&
Cyc__gentuple_935,& Cyc__gentuple_936}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_925={ 4u, sizeof( struct _tuple12),{( void*)(( struct _tuple4**) Cyc__genarr_937),(
void*)(( struct _tuple4**) Cyc__genarr_937),( void*)(( struct _tuple4**) Cyc__genarr_937
+  5u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_921; struct
_tuple13{ unsigned int f1; struct Cyc_Absyn_Tvar* f2; } ; static struct _tuple4
Cyc__gentuple_922={ offsetof( struct _tuple13,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_923={ offsetof( struct _tuple13,f2),( void*)&
Cyc__genrep_137}; static struct _tuple4* Cyc__genarr_924[ 2u]={& Cyc__gentuple_922,&
Cyc__gentuple_923}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_921={ 4u,
sizeof( struct _tuple13),{( void*)(( struct _tuple4**) Cyc__genarr_924),( void*)((
struct _tuple4**) Cyc__genarr_924),( void*)(( struct _tuple4**) Cyc__genarr_924
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_898; extern struct
Cyc_Typerep_Tuple_struct Cyc_Absyn_tunion_info_t_rep; extern struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_TunionInfoU_rep; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_904;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_UnknownTunionInfo_rep;
static struct _tuple4 Cyc__gentuple_905={ offsetof( struct Cyc_Absyn_UnknownTunionInfo,name),(
void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_906={ offsetof(
struct Cyc_Absyn_UnknownTunionInfo,is_xtunion),( void*)(( void*)& Cyc__genrep_74)};
static struct _tuple4* Cyc__genarr_907[ 2u]={& Cyc__gentuple_905,& Cyc__gentuple_906};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_UnknownTunionInfo_rep={ 4u,
sizeof( struct Cyc_Absyn_UnknownTunionInfo),{( void*)(( struct _tuple4**) Cyc__genarr_907),(
void*)(( struct _tuple4**) Cyc__genarr_907),( void*)(( struct _tuple4**) Cyc__genarr_907
+  2u)}}; struct _tuple14{ unsigned int f1; struct Cyc_Absyn_UnknownTunionInfo
f2; } ; static struct _tuple4 Cyc__gentuple_908={ offsetof( struct _tuple14,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_909={ offsetof(
struct _tuple14,f2),( void*)& Cyc_struct_Absyn_UnknownTunionInfo_rep}; static
struct _tuple4* Cyc__genarr_910[ 2u]={& Cyc__gentuple_908,& Cyc__gentuple_909};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_904={ 4u, sizeof( struct
_tuple14),{( void*)(( struct _tuple4**) Cyc__genarr_910),( void*)(( struct
_tuple4**) Cyc__genarr_910),( void*)(( struct _tuple4**) Cyc__genarr_910 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_899; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_900; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_252;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Tuniondecl_rep; extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_253; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_254; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_236; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Tunionfield_rep; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_237;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_238; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_239; static struct _tuple4 Cyc__gentuple_240={ offsetof( struct
_tuple3,f1),( void*)& Cyc__genrep_100}; static struct _tuple4 Cyc__gentuple_241={
offsetof( struct _tuple3,f2),( void*)(( void*)& Cyc_Absyn_type_t_rep)}; static
struct _tuple4* Cyc__genarr_242[ 2u]={& Cyc__gentuple_240,& Cyc__gentuple_241};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_239={ 4u, sizeof( struct
_tuple3),{( void*)(( struct _tuple4**) Cyc__genarr_242),( void*)(( struct
_tuple4**) Cyc__genarr_242),( void*)(( struct _tuple4**) Cyc__genarr_242 +  2u)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_238={ 2u, 1,( void*)(( void*)&
Cyc__genrep_239)}; static struct _tuple4 Cyc__gentuple_243={ offsetof( struct
Cyc_List_List,hd),( void*)& Cyc__genrep_238}; static struct _tuple4 Cyc__gentuple_244={
offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_237}; static struct
_tuple4* Cyc__genarr_245[ 2u]={& Cyc__gentuple_243,& Cyc__gentuple_244}; struct
Cyc_Typerep_Tuple_struct Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_245),(
void*)(( struct _tuple4**) Cyc__genarr_245),( void*)(( struct _tuple4**) Cyc__genarr_245
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_237={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_2={ 2u, 1,( void*)(( void*)& Cyc_struct_Position_Segment_rep)};
static struct _tuple4 Cyc__gentuple_246={ offsetof( struct Cyc_Absyn_Tunionfield,name),(
void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_247={ offsetof(
struct Cyc_Absyn_Tunionfield,tvs),( void*)& Cyc__genrep_194}; static struct
_tuple4 Cyc__gentuple_248={ offsetof( struct Cyc_Absyn_Tunionfield,typs),( void*)&
Cyc__genrep_237}; static struct _tuple4 Cyc__gentuple_249={ offsetof( struct Cyc_Absyn_Tunionfield,loc),(
void*)& Cyc__genrep_2}; static struct _tuple4 Cyc__gentuple_250={ offsetof(
struct Cyc_Absyn_Tunionfield,sc),( void*)& Cyc_Absyn_scope_t_rep}; static struct
_tuple4* Cyc__genarr_251[ 5u]={& Cyc__gentuple_246,& Cyc__gentuple_247,& Cyc__gentuple_248,&
Cyc__gentuple_249,& Cyc__gentuple_250}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Tunionfield_rep={
4u, sizeof( struct Cyc_Absyn_Tunionfield),{( void*)(( struct _tuple4**) Cyc__genarr_251),(
void*)(( struct _tuple4**) Cyc__genarr_251),( void*)(( struct _tuple4**) Cyc__genarr_251
+  5u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_236={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Tunionfield_rep)}; static struct _tuple4 Cyc__gentuple_255={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_236}; static struct
_tuple4 Cyc__gentuple_256={ offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_254};
static struct _tuple4* Cyc__genarr_257[ 2u]={& Cyc__gentuple_255,& Cyc__gentuple_256};
struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_257),(
void*)(( struct _tuple4**) Cyc__genarr_257),( void*)(( struct _tuple4**) Cyc__genarr_257
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_254={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep)}; static
struct _tuple4 Cyc__gentuple_258={ offsetof( struct Cyc_Core_Opt,v),( void*)&
Cyc__genrep_254}; static struct _tuple4* Cyc__genarr_259[ 1u]={& Cyc__gentuple_258};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep={
4u, sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_259),(
void*)(( struct _tuple4**) Cyc__genarr_259),( void*)(( struct _tuple4**) Cyc__genarr_259
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_253={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep)};
static struct _tuple4 Cyc__gentuple_260={ offsetof( struct Cyc_Absyn_Tuniondecl,sc),(
void*)& Cyc_Absyn_scope_t_rep}; static struct _tuple4 Cyc__gentuple_261={
offsetof( struct Cyc_Absyn_Tuniondecl,name),( void*)& Cyc__genrep_9}; static
struct _tuple4 Cyc__gentuple_262={ offsetof( struct Cyc_Absyn_Tuniondecl,tvs),(
void*)& Cyc__genrep_194}; static struct _tuple4 Cyc__gentuple_263={ offsetof(
struct Cyc_Absyn_Tuniondecl,fields),( void*)& Cyc__genrep_253}; static struct
_tuple4 Cyc__gentuple_264={ offsetof( struct Cyc_Absyn_Tuniondecl,is_xtunion),(
void*)(( void*)& Cyc__genrep_74)}; static struct _tuple4* Cyc__genarr_265[ 5u]={&
Cyc__gentuple_260,& Cyc__gentuple_261,& Cyc__gentuple_262,& Cyc__gentuple_263,&
Cyc__gentuple_264}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Tuniondecl_rep={
4u, sizeof( struct Cyc_Absyn_Tuniondecl),{( void*)(( struct _tuple4**) Cyc__genarr_265),(
void*)(( struct _tuple4**) Cyc__genarr_265),( void*)(( struct _tuple4**) Cyc__genarr_265
+  5u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_252={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Tuniondecl_rep)}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_900={ 2u, 1,( void*)(( void*)& Cyc__genrep_252)}; struct _tuple15{
unsigned int f1; struct Cyc_Absyn_Tuniondecl** f2; } ; static struct _tuple4 Cyc__gentuple_901={
offsetof( struct _tuple15,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_902={ offsetof( struct _tuple15,f2),( void*)& Cyc__genrep_900};
static struct _tuple4* Cyc__genarr_903[ 2u]={& Cyc__gentuple_901,& Cyc__gentuple_902};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_899={ 4u, sizeof( struct
_tuple15),{( void*)(( struct _tuple4**) Cyc__genarr_903),( void*)(( struct
_tuple4**) Cyc__genarr_903),( void*)(( struct _tuple4**) Cyc__genarr_903 +  2u)}};
static struct _tuple4 Cyc__gentuple_911={ 0,( void*)& Cyc__genrep_904}; static
struct _tuple4 Cyc__gentuple_912={ 1,( void*)& Cyc__genrep_899}; static struct
_tuple4* Cyc__genarr_913[ 2u]={& Cyc__gentuple_911,& Cyc__gentuple_912}; struct
Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep={ 5u,{( void*)((
struct _tuple4**) Cyc__genarr_913),( void*)(( struct _tuple4**) Cyc__genarr_913),(
void*)(( struct _tuple4**) Cyc__genarr_913 +  2u)}}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_44; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_type_t46H2_rep;
static struct _tuple4 Cyc__gentuple_45={ offsetof( struct Cyc_List_List,hd),(
void*)(( void*)& Cyc_Absyn_type_t_rep)}; static struct _tuple4 Cyc__gentuple_46={
offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_44}; static struct
_tuple4* Cyc__genarr_47[ 2u]={& Cyc__gentuple_45,& Cyc__gentuple_46}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0Absyn_type_t46H2_rep={ 4u, sizeof( struct Cyc_List_List),{(
void*)(( struct _tuple4**) Cyc__genarr_47),( void*)(( struct _tuple4**) Cyc__genarr_47),(
void*)(( struct _tuple4**) Cyc__genarr_47 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_44={ 2u, 1,( void*)(( void*)& Cyc_struct_List_List0Absyn_type_t46H2_rep)};
static struct _tuple4 Cyc__gentuple_914={ offsetof( struct Cyc_Absyn_TunionInfo,tunion_info),(
void*)& Cyc_tunion_Absyn_TunionInfoU_rep}; static struct _tuple4 Cyc__gentuple_915={
offsetof( struct Cyc_Absyn_TunionInfo,targs),( void*)& Cyc__genrep_44}; static
struct _tuple4 Cyc__gentuple_916={ offsetof( struct Cyc_Absyn_TunionInfo,rgn),(
void*)(( void*)& Cyc_Absyn_type_t_rep)}; static struct _tuple4* Cyc__genarr_917[
3u]={& Cyc__gentuple_914,& Cyc__gentuple_915,& Cyc__gentuple_916}; struct Cyc_Typerep_Tuple_struct
Cyc_Absyn_tunion_info_t_rep={ 4u, sizeof( struct Cyc_Absyn_TunionInfo),{( void*)((
struct _tuple4**) Cyc__genarr_917),( void*)(( struct _tuple4**) Cyc__genarr_917),(
void*)(( struct _tuple4**) Cyc__genarr_917 +  3u)}}; struct _tuple16{
unsigned int f1; struct Cyc_Absyn_TunionInfo f2; } ; static struct _tuple4 Cyc__gentuple_918={
offsetof( struct _tuple16,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_919={ offsetof( struct _tuple16,f2),( void*)& Cyc_Absyn_tunion_info_t_rep};
static struct _tuple4* Cyc__genarr_920[ 2u]={& Cyc__gentuple_918,& Cyc__gentuple_919};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_898={ 4u, sizeof( struct
_tuple16),{( void*)(( struct _tuple4**) Cyc__genarr_920),( void*)(( struct
_tuple4**) Cyc__genarr_920),( void*)(( struct _tuple4**) Cyc__genarr_920 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_875; extern struct Cyc_Typerep_Tuple_struct
Cyc_Absyn_tunion_field_info_t_rep; extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_881; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep; static struct _tuple4 Cyc__gentuple_882={
offsetof( struct Cyc_Absyn_UnknownTunionFieldInfo,tunion_name),( void*)& Cyc__genrep_9};
static struct _tuple4 Cyc__gentuple_883={ offsetof( struct Cyc_Absyn_UnknownTunionFieldInfo,field_name),(
void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_884={ offsetof(
struct Cyc_Absyn_UnknownTunionFieldInfo,is_xtunion),( void*)(( void*)& Cyc__genrep_74)};
static struct _tuple4* Cyc__genarr_885[ 3u]={& Cyc__gentuple_882,& Cyc__gentuple_883,&
Cyc__gentuple_884}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_UnknownTunionFieldInfo_rep={
4u, sizeof( struct Cyc_Absyn_UnknownTunionFieldInfo),{( void*)(( struct _tuple4**)
Cyc__genarr_885),( void*)(( struct _tuple4**) Cyc__genarr_885),( void*)(( struct
_tuple4**) Cyc__genarr_885 +  3u)}}; struct _tuple17{ unsigned int f1; struct
Cyc_Absyn_UnknownTunionFieldInfo f2; } ; static struct _tuple4 Cyc__gentuple_886={
offsetof( struct _tuple17,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_887={ offsetof( struct _tuple17,f2),( void*)& Cyc_struct_Absyn_UnknownTunionFieldInfo_rep};
static struct _tuple4* Cyc__genarr_888[ 2u]={& Cyc__gentuple_886,& Cyc__gentuple_887};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_881={ 4u, sizeof( struct
_tuple17),{( void*)(( struct _tuple4**) Cyc__genarr_888),( void*)(( struct
_tuple4**) Cyc__genarr_888),( void*)(( struct _tuple4**) Cyc__genarr_888 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_876; struct _tuple18{
unsigned int f1; struct Cyc_Absyn_Tuniondecl* f2; struct Cyc_Absyn_Tunionfield*
f3; } ; static struct _tuple4 Cyc__gentuple_877={ offsetof( struct _tuple18,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_878={ offsetof(
struct _tuple18,f2),( void*)(( void*)& Cyc__genrep_252)}; static struct _tuple4
Cyc__gentuple_879={ offsetof( struct _tuple18,f3),( void*)& Cyc__genrep_236};
static struct _tuple4* Cyc__genarr_880[ 3u]={& Cyc__gentuple_877,& Cyc__gentuple_878,&
Cyc__gentuple_879}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_876={ 4u,
sizeof( struct _tuple18),{( void*)(( struct _tuple4**) Cyc__genarr_880),( void*)((
struct _tuple4**) Cyc__genarr_880),( void*)(( struct _tuple4**) Cyc__genarr_880
+  3u)}}; static struct _tuple4 Cyc__gentuple_889={ 0,( void*)& Cyc__genrep_881};
static struct _tuple4 Cyc__gentuple_890={ 1,( void*)& Cyc__genrep_876}; static
struct _tuple4* Cyc__genarr_891[ 2u]={& Cyc__gentuple_889,& Cyc__gentuple_890};
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep={ 5u,{(
void*)(( struct _tuple4**) Cyc__genarr_891),( void*)(( struct _tuple4**) Cyc__genarr_891),(
void*)(( struct _tuple4**) Cyc__genarr_891 +  2u)}}; static struct _tuple4 Cyc__gentuple_892={
offsetof( struct Cyc_Absyn_TunionFieldInfo,field_info),( void*)& Cyc_tunion_Absyn_TunionFieldInfoU_rep};
static struct _tuple4 Cyc__gentuple_893={ offsetof( struct Cyc_Absyn_TunionFieldInfo,targs),(
void*)& Cyc__genrep_44}; static struct _tuple4* Cyc__genarr_894[ 2u]={& Cyc__gentuple_892,&
Cyc__gentuple_893}; struct Cyc_Typerep_Tuple_struct Cyc_Absyn_tunion_field_info_t_rep={
4u, sizeof( struct Cyc_Absyn_TunionFieldInfo),{( void*)(( struct _tuple4**) Cyc__genarr_894),(
void*)(( struct _tuple4**) Cyc__genarr_894),( void*)(( struct _tuple4**) Cyc__genarr_894
+  2u)}}; struct _tuple19{ unsigned int f1; struct Cyc_Absyn_TunionFieldInfo f2;
} ; static struct _tuple4 Cyc__gentuple_895={ offsetof( struct _tuple19,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_896={ offsetof(
struct _tuple19,f2),( void*)& Cyc_Absyn_tunion_field_info_t_rep}; static struct
_tuple4* Cyc__genarr_897[ 2u]={& Cyc__gentuple_895,& Cyc__gentuple_896}; static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_875={ 4u, sizeof( struct _tuple19),{(
void*)(( struct _tuple4**) Cyc__genarr_897),( void*)(( struct _tuple4**) Cyc__genarr_897),(
void*)(( struct _tuple4**) Cyc__genarr_897 +  2u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_846; extern struct Cyc_Typerep_Tuple_struct Cyc_Absyn_ptr_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_863; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Conref0bool2_rep; extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_852; extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_bounds_t_rep; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_65;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_66; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Exp_rep; extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_700; extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_cnst_t_rep; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_713;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep; static struct
_tuple4* Cyc__genarr_323[ 0u]={}; struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_323),( void*)(( struct _tuple4**)
Cyc__genarr_323),( void*)(( struct _tuple4**) Cyc__genarr_323 +  0u)}}; struct
_tuple20{ unsigned int f1; void* f2; unsigned char f3; } ; static struct _tuple4
Cyc__gentuple_714={ offsetof( struct _tuple20,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_715={ offsetof( struct _tuple20,f2),( void*)&
Cyc_Absyn_sign_t_rep}; static struct _tuple4 Cyc__gentuple_716={ offsetof(
struct _tuple20,f3),( void*)(( void*)& Cyc__genrep_13)}; static struct _tuple4*
Cyc__genarr_717[ 3u]={& Cyc__gentuple_714,& Cyc__gentuple_715,& Cyc__gentuple_716};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_713={ 4u, sizeof( struct
_tuple20),{( void*)(( struct _tuple4**) Cyc__genarr_717),( void*)(( struct
_tuple4**) Cyc__genarr_717),( void*)(( struct _tuple4**) Cyc__genarr_717 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_707; static struct Cyc_Typerep_Int_struct
Cyc__genrep_708={ 1u, 16}; struct _tuple21{ unsigned int f1; void* f2; short f3;
} ; static struct _tuple4 Cyc__gentuple_709={ offsetof( struct _tuple21,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_710={ offsetof(
struct _tuple21,f2),( void*)& Cyc_Absyn_sign_t_rep}; static struct _tuple4 Cyc__gentuple_711={
offsetof( struct _tuple21,f3),( void*)& Cyc__genrep_708}; static struct _tuple4*
Cyc__genarr_712[ 3u]={& Cyc__gentuple_709,& Cyc__gentuple_710,& Cyc__gentuple_711};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_707={ 4u, sizeof( struct
_tuple21),{( void*)(( struct _tuple4**) Cyc__genarr_712),( void*)(( struct
_tuple4**) Cyc__genarr_712),( void*)(( struct _tuple4**) Cyc__genarr_712 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_322; struct _tuple22{
unsigned int f1; void* f2; int f3; } ; static struct _tuple4 Cyc__gentuple_324={
offsetof( struct _tuple22,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_325={ offsetof( struct _tuple22,f2),( void*)& Cyc_Absyn_sign_t_rep};
static struct _tuple4 Cyc__gentuple_326={ offsetof( struct _tuple22,f3),( void*)((
void*)& Cyc__genrep_74)}; static struct _tuple4* Cyc__genarr_327[ 3u]={& Cyc__gentuple_324,&
Cyc__gentuple_325,& Cyc__gentuple_326}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_322={ 4u, sizeof( struct _tuple22),{( void*)(( struct _tuple4**) Cyc__genarr_327),(
void*)(( struct _tuple4**) Cyc__genarr_327),( void*)(( struct _tuple4**) Cyc__genarr_327
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_701; static struct
Cyc_Typerep_Int_struct Cyc__genrep_702={ 1u, 64}; struct _tuple23{ unsigned int
f1; void* f2; long long f3; } ; static struct _tuple4 Cyc__gentuple_703={
offsetof( struct _tuple23,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_704={ offsetof( struct _tuple23,f2),( void*)& Cyc_Absyn_sign_t_rep};
static struct _tuple4 Cyc__gentuple_705={ offsetof( struct _tuple23,f3),( void*)&
Cyc__genrep_702}; static struct _tuple4* Cyc__genarr_706[ 3u]={& Cyc__gentuple_703,&
Cyc__gentuple_704,& Cyc__gentuple_705}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_701={ 4u, sizeof( struct _tuple23),{( void*)(( struct _tuple4**) Cyc__genarr_706),(
void*)(( struct _tuple4**) Cyc__genarr_706),( void*)(( struct _tuple4**) Cyc__genarr_706
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_81; struct _tuple24{
unsigned int f1; struct _tagged_arr f2; } ; static struct _tuple4 Cyc__gentuple_82={
offsetof( struct _tuple24,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_83={ offsetof( struct _tuple24,f2),( void*)(( void*)& Cyc__genrep_12)};
static struct _tuple4* Cyc__genarr_84[ 2u]={& Cyc__gentuple_82,& Cyc__gentuple_83};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_81={ 4u, sizeof( struct
_tuple24),{( void*)(( struct _tuple4**) Cyc__genarr_84),( void*)(( struct
_tuple4**) Cyc__genarr_84),( void*)(( struct _tuple4**) Cyc__genarr_84 +  2u)}};
static struct _tuple4 Cyc__gentuple_718={ 0,( void*)& Cyc__genrep_713}; static
struct _tuple4 Cyc__gentuple_719={ 1,( void*)& Cyc__genrep_707}; static struct
_tuple4 Cyc__gentuple_720={ 2,( void*)& Cyc__genrep_322}; static struct _tuple4
Cyc__gentuple_721={ 3,( void*)& Cyc__genrep_701}; static struct _tuple4 Cyc__gentuple_722={
4,( void*)& Cyc__genrep_81}; static struct _tuple4 Cyc__gentuple_723={ 5,( void*)&
Cyc__genrep_81}; static struct _tuple4* Cyc__genarr_724[ 6u]={& Cyc__gentuple_718,&
Cyc__gentuple_719,& Cyc__gentuple_720,& Cyc__gentuple_721,& Cyc__gentuple_722,&
Cyc__gentuple_723}; struct Cyc_Typerep_TUnion_struct Cyc_Absyn_cnst_t_rep={ 5u,{(
void*)(( struct _tuple4**) Cyc__genarr_724),( void*)(( struct _tuple4**) Cyc__genarr_724),(
void*)(( struct _tuple4**) Cyc__genarr_724 +  6u)}}; static struct _tuple4 Cyc__gentuple_725={
offsetof( struct _tuple4,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_726={
offsetof( struct _tuple4,f2),( void*)& Cyc_Absyn_cnst_t_rep}; static struct
_tuple4* Cyc__genarr_727[ 2u]={& Cyc__gentuple_725,& Cyc__gentuple_726}; static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_700={ 4u, sizeof( struct _tuple4),{(
void*)(( struct _tuple4**) Cyc__genarr_727),( void*)(( struct _tuple4**) Cyc__genarr_727),(
void*)(( struct _tuple4**) Cyc__genarr_727 +  2u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_689; extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_binding_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_70; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_71; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Fndecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_490; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep; extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_491; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_492; struct _tuple25{ struct _tagged_arr* f1; struct Cyc_Absyn_Tqual
f2; void* f3; } ; static struct _tuple4 Cyc__gentuple_493={ offsetof( struct
_tuple25,f1),( void*)& Cyc__genrep_11}; static struct _tuple4 Cyc__gentuple_494={
offsetof( struct _tuple25,f2),( void*)& Cyc__genrep_100}; static struct _tuple4
Cyc__gentuple_495={ offsetof( struct _tuple25,f3),( void*)(( void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple4* Cyc__genarr_496[ 3u]={& Cyc__gentuple_493,& Cyc__gentuple_494,&
Cyc__gentuple_495}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_492={ 4u,
sizeof( struct _tuple25),{( void*)(( struct _tuple4**) Cyc__genarr_496),( void*)((
struct _tuple4**) Cyc__genarr_496),( void*)(( struct _tuple4**) Cyc__genarr_496
+  3u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_491={ 2u, 1,(
void*)(( void*)& Cyc__genrep_492)}; static struct _tuple4 Cyc__gentuple_497={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_491}; static struct
_tuple4 Cyc__gentuple_498={ offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_490};
static struct _tuple4* Cyc__genarr_499[ 2u]={& Cyc__gentuple_497,& Cyc__gentuple_498};
struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_499),(
void*)(( struct _tuple4**) Cyc__genarr_499),( void*)(( struct _tuple4**) Cyc__genarr_499
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_490={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_481; extern struct Cyc_Typerep_Tuple_struct
Cyc_Absyn_vararg_info_t_rep; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_482;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep;
static struct _tuple4 Cyc__gentuple_483={ offsetof( struct Cyc_Core_Opt,v),(
void*)& Cyc__genrep_11}; static struct _tuple4* Cyc__genarr_484[ 1u]={& Cyc__gentuple_483};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep={ 4u,
sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_484),(
void*)(( struct _tuple4**) Cyc__genarr_484),( void*)(( struct _tuple4**) Cyc__genarr_484
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_482={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt0Absyn_var_t2_rep)}; static struct _tuple4
Cyc__gentuple_485={ offsetof( struct Cyc_Absyn_VarargInfo,name),( void*)& Cyc__genrep_482};
static struct _tuple4 Cyc__gentuple_486={ offsetof( struct Cyc_Absyn_VarargInfo,tq),(
void*)& Cyc__genrep_100}; static struct _tuple4 Cyc__gentuple_487={ offsetof(
struct Cyc_Absyn_VarargInfo,type),( void*)(( void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple4 Cyc__gentuple_488={ offsetof( struct Cyc_Absyn_VarargInfo,inject),(
void*)(( void*)& Cyc__genrep_74)}; static struct _tuple4* Cyc__genarr_489[ 4u]={&
Cyc__gentuple_485,& Cyc__gentuple_486,& Cyc__gentuple_487,& Cyc__gentuple_488};
struct Cyc_Typerep_Tuple_struct Cyc_Absyn_vararg_info_t_rep={ 4u, sizeof( struct
Cyc_Absyn_VarargInfo),{( void*)(( struct _tuple4**) Cyc__genarr_489),( void*)((
struct _tuple4**) Cyc__genarr_489),( void*)(( struct _tuple4**) Cyc__genarr_489
+  4u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_481={ 2u, 1,(
void*)(( void*)& Cyc_Absyn_vararg_info_t_rep)}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_472; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_473; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_474; static struct _tuple4 Cyc__gentuple_475={ offsetof( struct
_tuple6,f1),( void*)(( void*)& Cyc_Absyn_type_t_rep)}; static struct _tuple4 Cyc__gentuple_476={
offsetof( struct _tuple6,f2),( void*)(( void*)& Cyc_Absyn_type_t_rep)}; static
struct _tuple4* Cyc__genarr_477[ 2u]={& Cyc__gentuple_475,& Cyc__gentuple_476};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_474={ 4u, sizeof( struct
_tuple6),{( void*)(( struct _tuple4**) Cyc__genarr_477),( void*)(( struct
_tuple4**) Cyc__genarr_477),( void*)(( struct _tuple4**) Cyc__genarr_477 +  2u)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_473={ 2u, 1,( void*)(( void*)&
Cyc__genrep_474)}; static struct _tuple4 Cyc__gentuple_478={ offsetof( struct
Cyc_List_List,hd),( void*)& Cyc__genrep_473}; static struct _tuple4 Cyc__gentuple_479={
offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_472}; static struct
_tuple4* Cyc__genarr_480[ 2u]={& Cyc__gentuple_478,& Cyc__gentuple_479}; struct
Cyc_Typerep_Tuple_struct Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_480),(
void*)(( struct _tuple4**) Cyc__genarr_480),( void*)(( struct _tuple4**) Cyc__genarr_480
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_472={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_118; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Stmt_rep; extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_440; struct _tuple26{
unsigned int f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt* f3; } ;
static struct _tuple4 Cyc__gentuple_441={ offsetof( struct _tuple26,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_442={ offsetof( struct
_tuple26,f2),( void*)& Cyc__genrep_118}; static struct _tuple4 Cyc__gentuple_443={
offsetof( struct _tuple26,f3),( void*)& Cyc__genrep_118}; static struct _tuple4*
Cyc__genarr_444[ 3u]={& Cyc__gentuple_441,& Cyc__gentuple_442,& Cyc__gentuple_443};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_440={ 4u, sizeof( struct
_tuple26),{( void*)(( struct _tuple4**) Cyc__genarr_444),( void*)(( struct
_tuple4**) Cyc__genarr_444),( void*)(( struct _tuple4**) Cyc__genarr_444 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_436; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_63; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_63={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Exp_rep)}; struct _tuple27{ unsigned int f1;
struct Cyc_Absyn_Exp* f2; } ; static struct _tuple4 Cyc__gentuple_437={
offsetof( struct _tuple27,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_438={ offsetof( struct _tuple27,f2),( void*)& Cyc__genrep_63};
static struct _tuple4* Cyc__genarr_439[ 2u]={& Cyc__gentuple_437,& Cyc__gentuple_438};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_436={ 4u, sizeof( struct
_tuple27),{( void*)(( struct _tuple4**) Cyc__genarr_439),( void*)(( struct
_tuple4**) Cyc__genarr_439),( void*)(( struct _tuple4**) Cyc__genarr_439 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_430; struct _tuple28{
unsigned int f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Stmt* f3; struct Cyc_Absyn_Stmt*
f4; } ; static struct _tuple4 Cyc__gentuple_431={ offsetof( struct _tuple28,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_432={ offsetof(
struct _tuple28,f2),( void*)& Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_433={
offsetof( struct _tuple28,f3),( void*)& Cyc__genrep_118}; static struct _tuple4
Cyc__gentuple_434={ offsetof( struct _tuple28,f4),( void*)& Cyc__genrep_118};
static struct _tuple4* Cyc__genarr_435[ 4u]={& Cyc__gentuple_431,& Cyc__gentuple_432,&
Cyc__gentuple_433,& Cyc__gentuple_434}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_430={ 4u, sizeof( struct _tuple28),{( void*)(( struct _tuple4**) Cyc__genarr_435),(
void*)(( struct _tuple4**) Cyc__genarr_435),( void*)(( struct _tuple4**) Cyc__genarr_435
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_425; extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_124; static struct _tuple4 Cyc__gentuple_125={
offsetof( struct _tuple2,f1),( void*)& Cyc__genrep_66}; static struct _tuple4
Cyc__gentuple_126={ offsetof( struct _tuple2,f2),( void*)& Cyc__genrep_118};
static struct _tuple4* Cyc__genarr_127[ 2u]={& Cyc__gentuple_125,& Cyc__gentuple_126};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_124={ 4u, sizeof( struct
_tuple2),{( void*)(( struct _tuple4**) Cyc__genarr_127),( void*)(( struct
_tuple4**) Cyc__genarr_127),( void*)(( struct _tuple4**) Cyc__genarr_127 +  2u)}};
struct _tuple29{ unsigned int f1; struct _tuple2 f2; struct Cyc_Absyn_Stmt* f3;
} ; static struct _tuple4 Cyc__gentuple_426={ offsetof( struct _tuple29,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_427={ offsetof(
struct _tuple29,f2),( void*)& Cyc__genrep_124}; static struct _tuple4 Cyc__gentuple_428={
offsetof( struct _tuple29,f3),( void*)& Cyc__genrep_118}; static struct _tuple4*
Cyc__genarr_429[ 3u]={& Cyc__gentuple_426,& Cyc__gentuple_427,& Cyc__gentuple_428};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_425={ 4u, sizeof( struct
_tuple29),{( void*)(( struct _tuple4**) Cyc__genarr_429),( void*)(( struct
_tuple4**) Cyc__genarr_429),( void*)(( struct _tuple4**) Cyc__genarr_429 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_421; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_416; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_416={ 2u,
1,( void*)(( void*)& Cyc_struct_Absyn_Stmt_rep)}; struct _tuple30{ unsigned int
f1; struct Cyc_Absyn_Stmt* f2; } ; static struct _tuple4 Cyc__gentuple_422={
offsetof( struct _tuple30,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_423={ offsetof( struct _tuple30,f2),( void*)& Cyc__genrep_416};
static struct _tuple4* Cyc__genarr_424[ 2u]={& Cyc__gentuple_422,& Cyc__gentuple_423};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_421={ 4u, sizeof( struct
_tuple30),{( void*)(( struct _tuple4**) Cyc__genarr_424),( void*)(( struct
_tuple4**) Cyc__genarr_424),( void*)(( struct _tuple4**) Cyc__genarr_424 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_415; struct _tuple31{
unsigned int f1; struct _tagged_arr* f2; struct Cyc_Absyn_Stmt* f3; } ; static
struct _tuple4 Cyc__gentuple_417={ offsetof( struct _tuple31,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_418={ offsetof( struct _tuple31,f2),( void*)&
Cyc__genrep_11}; static struct _tuple4 Cyc__gentuple_419={ offsetof( struct
_tuple31,f3),( void*)& Cyc__genrep_416}; static struct _tuple4* Cyc__genarr_420[
3u]={& Cyc__gentuple_417,& Cyc__gentuple_418,& Cyc__gentuple_419}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_415={ 4u, sizeof( struct _tuple31),{( void*)((
struct _tuple4**) Cyc__genarr_420),( void*)(( struct _tuple4**) Cyc__genarr_420),(
void*)(( struct _tuple4**) Cyc__genarr_420 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_408; struct _tuple32{ unsigned int f1; struct Cyc_Absyn_Exp* f2;
struct _tuple2 f3; struct _tuple2 f4; struct Cyc_Absyn_Stmt* f5; } ; static
struct _tuple4 Cyc__gentuple_409={ offsetof( struct _tuple32,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_410={ offsetof( struct _tuple32,f2),( void*)&
Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_411={ offsetof( struct
_tuple32,f3),( void*)& Cyc__genrep_124}; static struct _tuple4 Cyc__gentuple_412={
offsetof( struct _tuple32,f4),( void*)& Cyc__genrep_124}; static struct _tuple4
Cyc__gentuple_413={ offsetof( struct _tuple32,f5),( void*)& Cyc__genrep_118};
static struct _tuple4* Cyc__genarr_414[ 5u]={& Cyc__gentuple_409,& Cyc__gentuple_410,&
Cyc__gentuple_411,& Cyc__gentuple_412,& Cyc__gentuple_413}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_408={ 4u, sizeof( struct _tuple32),{( void*)(( struct _tuple4**) Cyc__genarr_414),(
void*)(( struct _tuple4**) Cyc__genarr_414),( void*)(( struct _tuple4**) Cyc__genarr_414
+  5u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_403; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_170; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_171; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Switch_clause_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_172; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Pat_rep; extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_pat_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_318; struct _tuple33{
unsigned int f1; unsigned char f2; } ; static struct _tuple4 Cyc__gentuple_319={
offsetof( struct _tuple33,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_320={ offsetof( struct _tuple33,f2),( void*)(( void*)& Cyc__genrep_13)};
static struct _tuple4* Cyc__genarr_321[ 2u]={& Cyc__gentuple_319,& Cyc__gentuple_320};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_318={ 4u, sizeof( struct
_tuple33),{( void*)(( struct _tuple4**) Cyc__genarr_321),( void*)(( struct
_tuple4**) Cyc__genarr_321),( void*)(( struct _tuple4**) Cyc__genarr_321 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_314; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_204; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep;
static struct _tuple4 Cyc__gentuple_205={ offsetof( struct Cyc_List_List,hd),(
void*)& Cyc__genrep_172}; static struct _tuple4 Cyc__gentuple_206={ offsetof(
struct Cyc_List_List,tl),( void*)& Cyc__genrep_204}; static struct _tuple4* Cyc__genarr_207[
2u]={& Cyc__gentuple_205,& Cyc__gentuple_206}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0Absyn_pat_t46H2_rep={ 4u, sizeof( struct Cyc_List_List),{(
void*)(( struct _tuple4**) Cyc__genarr_207),( void*)(( struct _tuple4**) Cyc__genarr_207),(
void*)(( struct _tuple4**) Cyc__genarr_207 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_204={ 2u, 1,( void*)(( void*)& Cyc_struct_List_List0Absyn_pat_t46H2_rep)};
static struct _tuple4 Cyc__gentuple_315={ offsetof( struct _tuple8,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_316={ offsetof( struct
_tuple8,f2),( void*)& Cyc__genrep_204}; static struct _tuple4* Cyc__genarr_317[
2u]={& Cyc__gentuple_315,& Cyc__gentuple_316}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_314={ 4u, sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_317),(
void*)(( struct _tuple4**) Cyc__genarr_317),( void*)(( struct _tuple4**) Cyc__genarr_317
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_310; struct
_tuple34{ unsigned int f1; struct Cyc_Absyn_Pat* f2; } ; static struct _tuple4
Cyc__gentuple_311={ offsetof( struct _tuple34,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_312={ offsetof( struct _tuple34,f2),( void*)&
Cyc__genrep_172}; static struct _tuple4* Cyc__genarr_313[ 2u]={& Cyc__gentuple_311,&
Cyc__gentuple_312}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_310={ 4u,
sizeof( struct _tuple34),{( void*)(( struct _tuple4**) Cyc__genarr_313),( void*)((
struct _tuple4**) Cyc__genarr_313),( void*)(( struct _tuple4**) Cyc__genarr_313
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_272; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_276; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Structdecl_rep; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_291;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep;
static struct _tuple4 Cyc__gentuple_292={ offsetof( struct Cyc_Core_Opt,v),(
void*)& Cyc__genrep_9}; static struct _tuple4* Cyc__genarr_293[ 1u]={& Cyc__gentuple_292};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep={
4u, sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_293),(
void*)(( struct _tuple4**) Cyc__genarr_293),( void*)(( struct _tuple4**) Cyc__genarr_293
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_291={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep)}; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_277; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_structfield_t46H22_rep; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_278; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_structfield_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_279; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Structfield_rep; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_72;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep; extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_85; struct _tuple35{ unsigned int f1; int
f2; } ; static struct _tuple4 Cyc__gentuple_86={ offsetof( struct _tuple35,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_87={ offsetof(
struct _tuple35,f2),( void*)(( void*)& Cyc__genrep_74)}; static struct _tuple4*
Cyc__genarr_88[ 2u]={& Cyc__gentuple_86,& Cyc__gentuple_87}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_85={ 4u, sizeof( struct _tuple35),{( void*)(( struct _tuple4**) Cyc__genarr_88),(
void*)(( struct _tuple4**) Cyc__genarr_88),( void*)(( struct _tuple4**) Cyc__genarr_88
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_73; extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_format_type_t_rep; static struct _tuple4*
Cyc__genarr_75[ 0u]={}; struct Cyc_Typerep_TUnion_struct Cyc_Absyn_format_type_t_rep={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_75),( void*)(( struct _tuple4**)
Cyc__genarr_75),( void*)(( struct _tuple4**) Cyc__genarr_75 +  0u)}}; struct
_tuple36{ unsigned int f1; void* f2; int f3; int f4; } ; static struct _tuple4
Cyc__gentuple_76={ offsetof( struct _tuple36,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_77={ offsetof( struct _tuple36,f2),( void*)&
Cyc_Absyn_format_type_t_rep}; static struct _tuple4 Cyc__gentuple_78={ offsetof(
struct _tuple36,f3),( void*)(( void*)& Cyc__genrep_74)}; static struct _tuple4
Cyc__gentuple_79={ offsetof( struct _tuple36,f4),( void*)(( void*)& Cyc__genrep_74)};
static struct _tuple4* Cyc__genarr_80[ 4u]={& Cyc__gentuple_76,& Cyc__gentuple_77,&
Cyc__gentuple_78,& Cyc__gentuple_79}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_73={
4u, sizeof( struct _tuple36),{( void*)(( struct _tuple4**) Cyc__genarr_80),(
void*)(( struct _tuple4**) Cyc__genarr_80),( void*)(( struct _tuple4**) Cyc__genarr_80
+  4u)}}; static struct _tuple4 Cyc__gentuple_89={ 0,( void*)& Cyc__genrep_85};
static struct _tuple4 Cyc__gentuple_90={ 1,( void*)& Cyc__genrep_85}; static
struct _tuple4 Cyc__gentuple_91={ 2,( void*)& Cyc__genrep_81}; static struct
_tuple4 Cyc__gentuple_92={ 3,( void*)& Cyc__genrep_73}; static struct _tuple4*
Cyc__genarr_93[ 4u]={& Cyc__gentuple_89,& Cyc__gentuple_90,& Cyc__gentuple_91,&
Cyc__gentuple_92}; struct Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_93),( void*)(( struct _tuple4**)
Cyc__genarr_93),( void*)(( struct _tuple4**) Cyc__genarr_93 +  4u)}}; static
struct _tuple4 Cyc__gentuple_94={ offsetof( struct Cyc_List_List,hd),( void*)&
Cyc_Absyn_attribute_t_rep}; static struct _tuple4 Cyc__gentuple_95={ offsetof(
struct Cyc_List_List,tl),( void*)& Cyc__genrep_72}; static struct _tuple4* Cyc__genarr_96[
2u]={& Cyc__gentuple_94,& Cyc__gentuple_95}; struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_96),(
void*)(( struct _tuple4**) Cyc__genarr_96),( void*)(( struct _tuple4**) Cyc__genarr_96
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_72={ 2u, 1,( void*)((
void*)& Cyc_struct_List_List0Absyn_attribute_t46H2_rep)}; static struct _tuple4
Cyc__gentuple_280={ offsetof( struct Cyc_Absyn_Structfield,name),( void*)& Cyc__genrep_11};
static struct _tuple4 Cyc__gentuple_281={ offsetof( struct Cyc_Absyn_Structfield,tq),(
void*)& Cyc__genrep_100}; static struct _tuple4 Cyc__gentuple_282={ offsetof(
struct Cyc_Absyn_Structfield,type),( void*)(( void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple4 Cyc__gentuple_283={ offsetof( struct Cyc_Absyn_Structfield,width),(
void*)& Cyc__genrep_63}; static struct _tuple4 Cyc__gentuple_284={ offsetof(
struct Cyc_Absyn_Structfield,attributes),( void*)& Cyc__genrep_72}; static
struct _tuple4* Cyc__genarr_285[ 5u]={& Cyc__gentuple_280,& Cyc__gentuple_281,&
Cyc__gentuple_282,& Cyc__gentuple_283,& Cyc__gentuple_284}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Structfield_rep={ 4u, sizeof( struct Cyc_Absyn_Structfield),{(
void*)(( struct _tuple4**) Cyc__genarr_285),( void*)(( struct _tuple4**) Cyc__genarr_285),(
void*)(( struct _tuple4**) Cyc__genarr_285 +  5u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_279={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_Structfield_rep)};
static struct _tuple4 Cyc__gentuple_286={ offsetof( struct Cyc_List_List,hd),(
void*)& Cyc__genrep_279}; static struct _tuple4 Cyc__gentuple_287={ offsetof(
struct Cyc_List_List,tl),( void*)& Cyc__genrep_278}; static struct _tuple4* Cyc__genarr_288[
2u]={& Cyc__gentuple_286,& Cyc__gentuple_287}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0Absyn_structfield_t46H2_rep={ 4u, sizeof( struct Cyc_List_List),{(
void*)(( struct _tuple4**) Cyc__genarr_288),( void*)(( struct _tuple4**) Cyc__genarr_288),(
void*)(( struct _tuple4**) Cyc__genarr_288 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_278={ 2u, 1,( void*)(( void*)& Cyc_struct_List_List0Absyn_structfield_t46H2_rep)};
static struct _tuple4 Cyc__gentuple_289={ offsetof( struct Cyc_Core_Opt,v),(
void*)& Cyc__genrep_278}; static struct _tuple4* Cyc__genarr_290[ 1u]={& Cyc__gentuple_289};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0List_list_t0Absyn_structfield_t46H22_rep={
4u, sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_290),(
void*)(( struct _tuple4**) Cyc__genarr_290),( void*)(( struct _tuple4**) Cyc__genarr_290
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_277={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_structfield_t46H22_rep)};
static struct _tuple4 Cyc__gentuple_294={ offsetof( struct Cyc_Absyn_Structdecl,sc),(
void*)& Cyc_Absyn_scope_t_rep}; static struct _tuple4 Cyc__gentuple_295={
offsetof( struct Cyc_Absyn_Structdecl,name),( void*)& Cyc__genrep_291}; static
struct _tuple4 Cyc__gentuple_296={ offsetof( struct Cyc_Absyn_Structdecl,tvs),(
void*)& Cyc__genrep_194}; static struct _tuple4 Cyc__gentuple_297={ offsetof(
struct Cyc_Absyn_Structdecl,fields),( void*)& Cyc__genrep_277}; static struct
_tuple4 Cyc__gentuple_298={ offsetof( struct Cyc_Absyn_Structdecl,attributes),(
void*)& Cyc__genrep_72}; static struct _tuple4* Cyc__genarr_299[ 5u]={& Cyc__gentuple_294,&
Cyc__gentuple_295,& Cyc__gentuple_296,& Cyc__gentuple_297,& Cyc__gentuple_298};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Structdecl_rep={ 4u, sizeof(
struct Cyc_Absyn_Structdecl),{( void*)(( struct _tuple4**) Cyc__genarr_299),(
void*)(( struct _tuple4**) Cyc__genarr_299),( void*)(( struct _tuple4**) Cyc__genarr_299
+  5u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_276={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Structdecl_rep)}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_273; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0List_list_t0Absyn_type_t46H22_rep;
static struct _tuple4 Cyc__gentuple_274={ offsetof( struct Cyc_Core_Opt,v),(
void*)& Cyc__genrep_44}; static struct _tuple4* Cyc__genarr_275[ 1u]={& Cyc__gentuple_274};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0List_list_t0Absyn_type_t46H22_rep={
4u, sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_275),(
void*)(( struct _tuple4**) Cyc__genarr_275),( void*)(( struct _tuple4**) Cyc__genarr_275
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_273={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_type_t46H22_rep)}; extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_174; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_175; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_176; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_177;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep; extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_178; struct _tuple37{ unsigned int
f1; struct _tagged_arr* f2; } ; static struct _tuple4 Cyc__gentuple_179={
offsetof( struct _tuple37,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_180={ offsetof( struct _tuple37,f2),( void*)& Cyc__genrep_11};
static struct _tuple4* Cyc__genarr_181[ 2u]={& Cyc__gentuple_179,& Cyc__gentuple_180};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_178={ 4u, sizeof( struct
_tuple37),{( void*)(( struct _tuple4**) Cyc__genarr_181),( void*)(( struct
_tuple4**) Cyc__genarr_181),( void*)(( struct _tuple4**) Cyc__genarr_181 +  2u)}};
static struct _tuple4 Cyc__gentuple_182={ 0,( void*)& Cyc__genrep_65}; static
struct _tuple4 Cyc__gentuple_183={ 1,( void*)& Cyc__genrep_178}; static struct
_tuple4* Cyc__genarr_184[ 2u]={& Cyc__gentuple_182,& Cyc__gentuple_183}; struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep={ 5u,{( void*)(( struct
_tuple4**) Cyc__genarr_184),( void*)(( struct _tuple4**) Cyc__genarr_184),( void*)((
struct _tuple4**) Cyc__genarr_184 +  2u)}}; static struct _tuple4 Cyc__gentuple_185={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc_Absyn_designator_t_rep}; static
struct _tuple4 Cyc__gentuple_186={ offsetof( struct Cyc_List_List,tl),( void*)&
Cyc__genrep_177}; static struct _tuple4* Cyc__genarr_187[ 2u]={& Cyc__gentuple_185,&
Cyc__gentuple_186}; struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_187),(
void*)(( struct _tuple4**) Cyc__genarr_187),( void*)(( struct _tuple4**) Cyc__genarr_187
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_177={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List0Absyn_designator_t46H2_rep)}; struct
_tuple38{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; static struct
_tuple4 Cyc__gentuple_188={ offsetof( struct _tuple38,f1),( void*)& Cyc__genrep_177};
static struct _tuple4 Cyc__gentuple_189={ offsetof( struct _tuple38,f2),( void*)&
Cyc__genrep_172}; static struct _tuple4* Cyc__genarr_190[ 2u]={& Cyc__gentuple_188,&
Cyc__gentuple_189}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_176={ 4u,
sizeof( struct _tuple38),{( void*)(( struct _tuple4**) Cyc__genarr_190),( void*)((
struct _tuple4**) Cyc__genarr_190),( void*)(( struct _tuple4**) Cyc__genarr_190
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_175={ 2u, 1,(
void*)(( void*)& Cyc__genrep_176)}; static struct _tuple4 Cyc__gentuple_191={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_175}; static struct
_tuple4 Cyc__gentuple_192={ offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_174};
static struct _tuple4* Cyc__genarr_193[ 2u]={& Cyc__gentuple_191,& Cyc__gentuple_192};
struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_193),(
void*)(( struct _tuple4**) Cyc__genarr_193),( void*)(( struct _tuple4**) Cyc__genarr_193
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_174={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep)};
struct _tuple39{ unsigned int f1; struct Cyc_Absyn_Structdecl* f2; struct Cyc_Core_Opt*
f3; struct Cyc_List_List* f4; struct Cyc_List_List* f5; } ; static struct
_tuple4 Cyc__gentuple_300={ offsetof( struct _tuple39,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_301={ offsetof( struct _tuple39,f2),( void*)((
void*)& Cyc__genrep_276)}; static struct _tuple4 Cyc__gentuple_302={ offsetof(
struct _tuple39,f3),( void*)& Cyc__genrep_273}; static struct _tuple4 Cyc__gentuple_303={
offsetof( struct _tuple39,f4),( void*)& Cyc__genrep_194}; static struct _tuple4
Cyc__gentuple_304={ offsetof( struct _tuple39,f5),( void*)& Cyc__genrep_174};
static struct _tuple4* Cyc__genarr_305[ 5u]={& Cyc__gentuple_300,& Cyc__gentuple_301,&
Cyc__gentuple_302,& Cyc__gentuple_303,& Cyc__gentuple_304}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_272={ 4u, sizeof( struct _tuple39),{( void*)(( struct _tuple4**) Cyc__genarr_305),(
void*)(( struct _tuple4**) Cyc__genarr_305),( void*)(( struct _tuple4**) Cyc__genarr_305
+  5u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_235; struct
_tuple40{ unsigned int f1; struct Cyc_Absyn_Tuniondecl* f2; struct Cyc_Absyn_Tunionfield*
f3; struct Cyc_List_List* f4; struct Cyc_List_List* f5; } ; static struct
_tuple4 Cyc__gentuple_266={ offsetof( struct _tuple40,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_267={ offsetof( struct _tuple40,f2),( void*)((
void*)& Cyc__genrep_252)}; static struct _tuple4 Cyc__gentuple_268={ offsetof(
struct _tuple40,f3),( void*)& Cyc__genrep_236}; static struct _tuple4 Cyc__gentuple_269={
offsetof( struct _tuple40,f4),( void*)& Cyc__genrep_194}; static struct _tuple4
Cyc__gentuple_270={ offsetof( struct _tuple40,f5),( void*)& Cyc__genrep_204};
static struct _tuple4* Cyc__genarr_271[ 5u]={& Cyc__gentuple_266,& Cyc__gentuple_267,&
Cyc__gentuple_268,& Cyc__gentuple_269,& Cyc__gentuple_270}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_235={ 4u, sizeof( struct _tuple40),{( void*)(( struct _tuple4**) Cyc__genarr_271),(
void*)(( struct _tuple4**) Cyc__genarr_271),( void*)(( struct _tuple4**) Cyc__genarr_271
+  5u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_222; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_223; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Enumdecl_rep; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_224;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0Absyn_enumfield_t46H2_rep; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_62; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Enumfield_rep;
static struct _tuple4 Cyc__gentuple_772={ offsetof( struct Cyc_Absyn_Enumfield,name),(
void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_773={ offsetof(
struct Cyc_Absyn_Enumfield,tag),( void*)& Cyc__genrep_63}; static struct _tuple4
Cyc__gentuple_774={ offsetof( struct Cyc_Absyn_Enumfield,loc),( void*)& Cyc__genrep_2};
static struct _tuple4* Cyc__genarr_775[ 3u]={& Cyc__gentuple_772,& Cyc__gentuple_773,&
Cyc__gentuple_774}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Enumfield_rep={
4u, sizeof( struct Cyc_Absyn_Enumfield),{( void*)(( struct _tuple4**) Cyc__genarr_775),(
void*)(( struct _tuple4**) Cyc__genarr_775),( void*)(( struct _tuple4**) Cyc__genarr_775
+  3u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62={ 2u, 1,( void*)((
void*)& Cyc_struct_Absyn_Enumfield_rep)}; static struct _tuple4 Cyc__gentuple_776={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_62}; static struct
_tuple4 Cyc__gentuple_777={ offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_61};
static struct _tuple4* Cyc__genarr_778[ 2u]={& Cyc__gentuple_776,& Cyc__gentuple_777};
struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_778),(
void*)(( struct _tuple4**) Cyc__genarr_778),( void*)(( struct _tuple4**) Cyc__genarr_778
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61={ 2u, 1,( void*)((
void*)& Cyc_struct_List_List0Absyn_enumfield_t46H2_rep)}; static struct _tuple4
Cyc__gentuple_225={ offsetof( struct Cyc_Core_Opt,v),( void*)& Cyc__genrep_61};
static struct _tuple4* Cyc__genarr_226[ 1u]={& Cyc__gentuple_225}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep={ 4u, sizeof( struct
Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_226),( void*)(( struct
_tuple4**) Cyc__genarr_226),( void*)(( struct _tuple4**) Cyc__genarr_226 +  1u)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_224={ 2u, 1,( void*)(( void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep)}; static struct
_tuple4 Cyc__gentuple_227={ offsetof( struct Cyc_Absyn_Enumdecl,sc),( void*)&
Cyc_Absyn_scope_t_rep}; static struct _tuple4 Cyc__gentuple_228={ offsetof(
struct Cyc_Absyn_Enumdecl,name),( void*)& Cyc__genrep_9}; static struct _tuple4
Cyc__gentuple_229={ offsetof( struct Cyc_Absyn_Enumdecl,fields),( void*)& Cyc__genrep_224};
static struct _tuple4* Cyc__genarr_230[ 3u]={& Cyc__gentuple_227,& Cyc__gentuple_228,&
Cyc__gentuple_229}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Enumdecl_rep={
4u, sizeof( struct Cyc_Absyn_Enumdecl),{( void*)(( struct _tuple4**) Cyc__genarr_230),(
void*)(( struct _tuple4**) Cyc__genarr_230),( void*)(( struct _tuple4**) Cyc__genarr_230
+  3u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_223={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Enumdecl_rep)}; struct _tuple41{ unsigned int
f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield* f3; } ; static
struct _tuple4 Cyc__gentuple_231={ offsetof( struct _tuple41,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_232={ offsetof( struct _tuple41,f2),( void*)&
Cyc__genrep_223}; static struct _tuple4 Cyc__gentuple_233={ offsetof( struct
_tuple41,f3),( void*)& Cyc__genrep_62}; static struct _tuple4* Cyc__genarr_234[
3u]={& Cyc__gentuple_231,& Cyc__gentuple_232,& Cyc__gentuple_233}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_222={ 4u, sizeof( struct _tuple41),{( void*)((
struct _tuple4**) Cyc__genarr_234),( void*)(( struct _tuple4**) Cyc__genarr_234),(
void*)(( struct _tuple4**) Cyc__genarr_234 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_217; struct _tuple42{ unsigned int f1; void* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static struct _tuple4 Cyc__gentuple_218={ offsetof( struct _tuple42,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_219={ offsetof(
struct _tuple42,f2),( void*)(( void*)& Cyc_Absyn_type_t_rep)}; static struct
_tuple4 Cyc__gentuple_220={ offsetof( struct _tuple42,f3),( void*)& Cyc__genrep_62};
static struct _tuple4* Cyc__genarr_221[ 3u]={& Cyc__gentuple_218,& Cyc__gentuple_219,&
Cyc__gentuple_220}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_217={ 4u,
sizeof( struct _tuple42),{( void*)(( struct _tuple4**) Cyc__genarr_221),( void*)((
struct _tuple4**) Cyc__genarr_221),( void*)(( struct _tuple4**) Cyc__genarr_221
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_213; struct
_tuple43{ unsigned int f1; struct _tuple0* f2; } ; static struct _tuple4 Cyc__gentuple_214={
offsetof( struct _tuple43,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_215={ offsetof( struct _tuple43,f2),( void*)& Cyc__genrep_9};
static struct _tuple4* Cyc__genarr_216[ 2u]={& Cyc__gentuple_214,& Cyc__gentuple_215};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_213={ 4u, sizeof( struct
_tuple43),{( void*)(( struct _tuple4**) Cyc__genarr_216),( void*)(( struct
_tuple4**) Cyc__genarr_216),( void*)(( struct _tuple4**) Cyc__genarr_216 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_203; struct _tuple44{
unsigned int f1; struct _tuple0* f2; struct Cyc_List_List* f3; struct Cyc_List_List*
f4; } ; static struct _tuple4 Cyc__gentuple_208={ offsetof( struct _tuple44,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_209={ offsetof(
struct _tuple44,f2),( void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_210={
offsetof( struct _tuple44,f3),( void*)& Cyc__genrep_194}; static struct _tuple4
Cyc__gentuple_211={ offsetof( struct _tuple44,f4),( void*)& Cyc__genrep_204};
static struct _tuple4* Cyc__genarr_212[ 4u]={& Cyc__gentuple_208,& Cyc__gentuple_209,&
Cyc__gentuple_210,& Cyc__gentuple_211}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_203={ 4u, sizeof( struct _tuple44),{( void*)(( struct _tuple4**) Cyc__genarr_212),(
void*)(( struct _tuple4**) Cyc__genarr_212),( void*)(( struct _tuple4**) Cyc__genarr_212
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_173; static struct
_tuple4 Cyc__gentuple_198={ offsetof( struct _tuple44,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_199={ offsetof( struct _tuple44,f2),( void*)&
Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_200={ offsetof( struct
_tuple44,f3),( void*)& Cyc__genrep_194}; static struct _tuple4 Cyc__gentuple_201={
offsetof( struct _tuple44,f4),( void*)& Cyc__genrep_174}; static struct _tuple4*
Cyc__genarr_202[ 4u]={& Cyc__gentuple_198,& Cyc__gentuple_199,& Cyc__gentuple_200,&
Cyc__gentuple_201}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_173={ 4u,
sizeof( struct _tuple44),{( void*)(( struct _tuple4**) Cyc__genarr_202),( void*)((
struct _tuple4**) Cyc__genarr_202),( void*)(( struct _tuple4**) Cyc__genarr_202
+  4u)}}; static struct _tuple4 Cyc__gentuple_328={ 0,( void*)& Cyc__genrep_306};
static struct _tuple4 Cyc__gentuple_329={ 1,( void*)& Cyc__genrep_322}; static
struct _tuple4 Cyc__gentuple_330={ 2,( void*)& Cyc__genrep_318}; static struct
_tuple4 Cyc__gentuple_331={ 3,( void*)& Cyc__genrep_81}; static struct _tuple4
Cyc__gentuple_332={ 4,( void*)& Cyc__genrep_314}; static struct _tuple4 Cyc__gentuple_333={
5,( void*)& Cyc__genrep_310}; static struct _tuple4 Cyc__gentuple_334={ 6,( void*)&
Cyc__genrep_306}; static struct _tuple4 Cyc__gentuple_335={ 7,( void*)& Cyc__genrep_272};
static struct _tuple4 Cyc__gentuple_336={ 8,( void*)& Cyc__genrep_235}; static
struct _tuple4 Cyc__gentuple_337={ 9,( void*)& Cyc__genrep_222}; static struct
_tuple4 Cyc__gentuple_338={ 10,( void*)& Cyc__genrep_217}; static struct _tuple4
Cyc__gentuple_339={ 11,( void*)& Cyc__genrep_213}; static struct _tuple4 Cyc__gentuple_340={
12,( void*)& Cyc__genrep_203}; static struct _tuple4 Cyc__gentuple_341={ 13,(
void*)& Cyc__genrep_173}; static struct _tuple4* Cyc__genarr_342[ 14u]={& Cyc__gentuple_328,&
Cyc__gentuple_329,& Cyc__gentuple_330,& Cyc__gentuple_331,& Cyc__gentuple_332,&
Cyc__gentuple_333,& Cyc__gentuple_334,& Cyc__gentuple_335,& Cyc__gentuple_336,&
Cyc__gentuple_337,& Cyc__gentuple_338,& Cyc__gentuple_339,& Cyc__gentuple_340,&
Cyc__gentuple_341}; struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_pat_t_rep={ 5u,{(
void*)(( struct _tuple4**) Cyc__genarr_342),( void*)(( struct _tuple4**) Cyc__genarr_342),(
void*)(( struct _tuple4**) Cyc__genarr_342 +  14u)}}; static struct _tuple4 Cyc__gentuple_343={
offsetof( struct Cyc_Absyn_Pat,r),( void*)& Cyc_Absyn_raw_pat_t_rep}; static
struct _tuple4 Cyc__gentuple_344={ offsetof( struct Cyc_Absyn_Pat,topt),( void*)&
Cyc__genrep_52}; static struct _tuple4 Cyc__gentuple_345={ offsetof( struct Cyc_Absyn_Pat,loc),(
void*)& Cyc__genrep_2}; static struct _tuple4* Cyc__genarr_346[ 3u]={& Cyc__gentuple_343,&
Cyc__gentuple_344,& Cyc__gentuple_345}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Pat_rep={
4u, sizeof( struct Cyc_Absyn_Pat),{( void*)(( struct _tuple4**) Cyc__genarr_346),(
void*)(( struct _tuple4**) Cyc__genarr_346),( void*)(( struct _tuple4**) Cyc__genarr_346
+  3u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_172={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Pat_rep)}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_97; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_98; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0Absyn_vardecl_t46H2_rep; static struct _tuple4 Cyc__gentuple_113={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_99}; static struct
_tuple4 Cyc__gentuple_114={ offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_98};
static struct _tuple4* Cyc__genarr_115[ 2u]={& Cyc__gentuple_113,& Cyc__gentuple_114};
struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_vardecl_t46H2_rep={ 4u,
sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_115),(
void*)(( struct _tuple4**) Cyc__genarr_115),( void*)(( struct _tuple4**) Cyc__genarr_115
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_98={ 2u, 1,( void*)((
void*)& Cyc_struct_List_List0Absyn_vardecl_t46H2_rep)}; static struct _tuple4
Cyc__gentuple_116={ offsetof( struct Cyc_Core_Opt,v),( void*)& Cyc__genrep_98};
static struct _tuple4* Cyc__genarr_117[ 1u]={& Cyc__gentuple_116}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep={ 4u, sizeof( struct
Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_117),( void*)(( struct
_tuple4**) Cyc__genarr_117),( void*)(( struct _tuple4**) Cyc__genarr_117 +  1u)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_97={ 2u, 1,( void*)(( void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep)}; static struct
_tuple4 Cyc__gentuple_347={ offsetof( struct Cyc_Absyn_Switch_clause,pattern),(
void*)& Cyc__genrep_172}; static struct _tuple4 Cyc__gentuple_348={ offsetof(
struct Cyc_Absyn_Switch_clause,pat_vars),( void*)& Cyc__genrep_97}; static
struct _tuple4 Cyc__gentuple_349={ offsetof( struct Cyc_Absyn_Switch_clause,where_clause),(
void*)& Cyc__genrep_63}; static struct _tuple4 Cyc__gentuple_350={ offsetof(
struct Cyc_Absyn_Switch_clause,body),( void*)& Cyc__genrep_118}; static struct
_tuple4 Cyc__gentuple_351={ offsetof( struct Cyc_Absyn_Switch_clause,loc),( void*)&
Cyc__genrep_2}; static struct _tuple4* Cyc__genarr_352[ 5u]={& Cyc__gentuple_347,&
Cyc__gentuple_348,& Cyc__gentuple_349,& Cyc__gentuple_350,& Cyc__gentuple_351};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Switch_clause_rep={ 4u, sizeof(
struct Cyc_Absyn_Switch_clause),{( void*)(( struct _tuple4**) Cyc__genarr_352),(
void*)(( struct _tuple4**) Cyc__genarr_352),( void*)(( struct _tuple4**) Cyc__genarr_352
+  5u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_171={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Switch_clause_rep)}; static struct _tuple4 Cyc__gentuple_353={
offsetof( struct Cyc_List_List,hd),( void*)(( void*)& Cyc__genrep_171)}; static
struct _tuple4 Cyc__gentuple_354={ offsetof( struct Cyc_List_List,tl),( void*)&
Cyc__genrep_170}; static struct _tuple4* Cyc__genarr_355[ 2u]={& Cyc__gentuple_353,&
Cyc__gentuple_354}; struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_355),(
void*)(( struct _tuple4**) Cyc__genarr_355),( void*)(( struct _tuple4**) Cyc__genarr_355
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_170={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep)}; struct
_tuple45{ unsigned int f1; struct Cyc_Absyn_Exp* f2; struct Cyc_List_List* f3; }
; static struct _tuple4 Cyc__gentuple_404={ offsetof( struct _tuple45,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_405={ offsetof( struct
_tuple45,f2),( void*)& Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_406={
offsetof( struct _tuple45,f3),( void*)& Cyc__genrep_170}; static struct _tuple4*
Cyc__genarr_407[ 3u]={& Cyc__gentuple_404,& Cyc__gentuple_405,& Cyc__gentuple_406};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_403={ 4u, sizeof( struct
_tuple45),{( void*)(( struct _tuple4**) Cyc__genarr_407),( void*)(( struct
_tuple4**) Cyc__genarr_407),( void*)(( struct _tuple4**) Cyc__genarr_407 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_389; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_390; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_391; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_SwitchC_clause_rep; static struct _tuple4 Cyc__gentuple_392={
offsetof( struct Cyc_Absyn_SwitchC_clause,cnst_exp),( void*)& Cyc__genrep_63};
static struct _tuple4 Cyc__gentuple_393={ offsetof( struct Cyc_Absyn_SwitchC_clause,body),(
void*)& Cyc__genrep_118}; static struct _tuple4 Cyc__gentuple_394={ offsetof(
struct Cyc_Absyn_SwitchC_clause,loc),( void*)& Cyc__genrep_2}; static struct
_tuple4* Cyc__genarr_395[ 3u]={& Cyc__gentuple_392,& Cyc__gentuple_393,& Cyc__gentuple_394};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_SwitchC_clause_rep={ 4u,
sizeof( struct Cyc_Absyn_SwitchC_clause),{( void*)(( struct _tuple4**) Cyc__genarr_395),(
void*)(( struct _tuple4**) Cyc__genarr_395),( void*)(( struct _tuple4**) Cyc__genarr_395
+  3u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_391={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_SwitchC_clause_rep)}; static struct _tuple4
Cyc__gentuple_396={ offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_391};
static struct _tuple4 Cyc__gentuple_397={ offsetof( struct Cyc_List_List,tl),(
void*)& Cyc__genrep_390}; static struct _tuple4* Cyc__genarr_398[ 2u]={& Cyc__gentuple_396,&
Cyc__gentuple_397}; struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_398),(
void*)(( struct _tuple4**) Cyc__genarr_398),( void*)(( struct _tuple4**) Cyc__genarr_398
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_390={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List0Absyn_switchC_clause_t46H2_rep)}; static
struct _tuple4 Cyc__gentuple_399={ offsetof( struct _tuple45,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_400={ offsetof( struct _tuple45,f2),( void*)&
Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_401={ offsetof( struct
_tuple45,f3),( void*)& Cyc__genrep_390}; static struct _tuple4* Cyc__genarr_402[
3u]={& Cyc__gentuple_399,& Cyc__gentuple_400,& Cyc__gentuple_401}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_389={ 4u, sizeof( struct _tuple45),{( void*)((
struct _tuple4**) Cyc__genarr_402),( void*)(( struct _tuple4**) Cyc__genarr_402),(
void*)(( struct _tuple4**) Cyc__genarr_402 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_379; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_381;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep;
static struct _tuple4 Cyc__gentuple_382={ offsetof( struct Cyc_List_List,hd),(
void*)& Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_383={ offsetof(
struct Cyc_List_List,tl),( void*)& Cyc__genrep_381}; static struct _tuple4* Cyc__genarr_384[
2u]={& Cyc__gentuple_382,& Cyc__gentuple_383}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0Absyn_exp_t46H2_rep={ 4u, sizeof( struct Cyc_List_List),{(
void*)(( struct _tuple4**) Cyc__genarr_384),( void*)(( struct _tuple4**) Cyc__genarr_384),(
void*)(( struct _tuple4**) Cyc__genarr_384 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_381={ 2u, 1,( void*)(( void*)& Cyc_struct_List_List0Absyn_exp_t46H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_380; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_380={ 2u, 1,( void*)(( void*)& Cyc__genrep_171)}; struct _tuple46{
unsigned int f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Switch_clause** f3;
} ; static struct _tuple4 Cyc__gentuple_385={ offsetof( struct _tuple46,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_386={ offsetof(
struct _tuple46,f2),( void*)& Cyc__genrep_381}; static struct _tuple4 Cyc__gentuple_387={
offsetof( struct _tuple46,f3),( void*)& Cyc__genrep_380}; static struct _tuple4*
Cyc__genarr_388[ 3u]={& Cyc__gentuple_385,& Cyc__gentuple_386,& Cyc__gentuple_387};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_379={ 4u, sizeof( struct
_tuple46),{( void*)(( struct _tuple4**) Cyc__genarr_388),( void*)(( struct
_tuple4**) Cyc__genarr_388),( void*)(( struct _tuple4**) Cyc__genarr_388 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_374; struct _tuple47{
unsigned int f1; struct Cyc_Absyn_Decl* f2; struct Cyc_Absyn_Stmt* f3; } ;
static struct _tuple4 Cyc__gentuple_375={ offsetof( struct _tuple47,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_376={ offsetof( struct
_tuple47,f2),( void*)& Cyc__genrep_1}; static struct _tuple4 Cyc__gentuple_377={
offsetof( struct _tuple47,f3),( void*)& Cyc__genrep_118}; static struct _tuple4*
Cyc__genarr_378[ 3u]={& Cyc__gentuple_375,& Cyc__gentuple_376,& Cyc__gentuple_377};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_374={ 4u, sizeof( struct
_tuple47),{( void*)(( struct _tuple4**) Cyc__genarr_378),( void*)(( struct
_tuple4**) Cyc__genarr_378),( void*)(( struct _tuple4**) Cyc__genarr_378 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_370; static struct _tuple4
Cyc__gentuple_371={ offsetof( struct _tuple30,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_372={ offsetof( struct _tuple30,f2),( void*)&
Cyc__genrep_118}; static struct _tuple4* Cyc__genarr_373[ 2u]={& Cyc__gentuple_371,&
Cyc__gentuple_372}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_370={ 4u,
sizeof( struct _tuple30),{( void*)(( struct _tuple4**) Cyc__genarr_373),( void*)((
struct _tuple4**) Cyc__genarr_373),( void*)(( struct _tuple4**) Cyc__genarr_373
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_365; static struct
_tuple4 Cyc__gentuple_366={ offsetof( struct _tuple31,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_367={ offsetof( struct _tuple31,f2),( void*)&
Cyc__genrep_11}; static struct _tuple4 Cyc__gentuple_368={ offsetof( struct
_tuple31,f3),( void*)& Cyc__genrep_118}; static struct _tuple4* Cyc__genarr_369[
3u]={& Cyc__gentuple_366,& Cyc__gentuple_367,& Cyc__gentuple_368}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_365={ 4u, sizeof( struct _tuple31),{( void*)((
struct _tuple4**) Cyc__genarr_369),( void*)(( struct _tuple4**) Cyc__genarr_369),(
void*)(( struct _tuple4**) Cyc__genarr_369 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_360; struct _tuple48{ unsigned int f1; struct Cyc_Absyn_Stmt* f2;
struct _tuple2 f3; } ; static struct _tuple4 Cyc__gentuple_361={ offsetof(
struct _tuple48,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_362={
offsetof( struct _tuple48,f2),( void*)& Cyc__genrep_118}; static struct _tuple4
Cyc__gentuple_363={ offsetof( struct _tuple48,f3),( void*)& Cyc__genrep_124};
static struct _tuple4* Cyc__genarr_364[ 3u]={& Cyc__gentuple_361,& Cyc__gentuple_362,&
Cyc__gentuple_363}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_360={ 4u,
sizeof( struct _tuple48),{( void*)(( struct _tuple4**) Cyc__genarr_364),( void*)((
struct _tuple4**) Cyc__genarr_364),( void*)(( struct _tuple4**) Cyc__genarr_364
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_169; struct
_tuple49{ unsigned int f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_List_List* f3;
} ; static struct _tuple4 Cyc__gentuple_356={ offsetof( struct _tuple49,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_357={ offsetof(
struct _tuple49,f2),( void*)& Cyc__genrep_118}; static struct _tuple4 Cyc__gentuple_358={
offsetof( struct _tuple49,f3),( void*)& Cyc__genrep_170}; static struct _tuple4*
Cyc__genarr_359[ 3u]={& Cyc__gentuple_356,& Cyc__gentuple_357,& Cyc__gentuple_358};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_169={ 4u, sizeof( struct
_tuple49),{( void*)(( struct _tuple4**) Cyc__genarr_359),( void*)(( struct
_tuple4**) Cyc__genarr_359),( void*)(( struct _tuple4**) Cyc__genarr_359 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_136; struct _tuple50{
unsigned int f1; struct Cyc_Absyn_Tvar* f2; struct Cyc_Absyn_Vardecl* f3; struct
Cyc_Absyn_Stmt* f4; } ; static struct _tuple4 Cyc__gentuple_164={ offsetof(
struct _tuple50,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_165={
offsetof( struct _tuple50,f2),( void*)& Cyc__genrep_137}; static struct _tuple4
Cyc__gentuple_166={ offsetof( struct _tuple50,f3),( void*)& Cyc__genrep_99};
static struct _tuple4 Cyc__gentuple_167={ offsetof( struct _tuple50,f4),( void*)&
Cyc__genrep_118}; static struct _tuple4* Cyc__genarr_168[ 4u]={& Cyc__gentuple_164,&
Cyc__gentuple_165,& Cyc__gentuple_166,& Cyc__gentuple_167}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_136={ 4u, sizeof( struct _tuple50),{( void*)(( struct _tuple4**) Cyc__genarr_168),(
void*)(( struct _tuple4**) Cyc__genarr_168),( void*)(( struct _tuple4**) Cyc__genarr_168
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_123; extern struct
Cyc_Typerep_Tuple_struct Cyc_Absyn_forarray_info_t_rep; static struct _tuple4
Cyc__gentuple_128={ offsetof( struct Cyc_Absyn_ForArrayInfo,defns),( void*)& Cyc__genrep_98};
static struct _tuple4 Cyc__gentuple_129={ offsetof( struct Cyc_Absyn_ForArrayInfo,condition),(
void*)& Cyc__genrep_124}; static struct _tuple4 Cyc__gentuple_130={ offsetof(
struct Cyc_Absyn_ForArrayInfo,delta),( void*)& Cyc__genrep_124}; static struct
_tuple4 Cyc__gentuple_131={ offsetof( struct Cyc_Absyn_ForArrayInfo,body),( void*)&
Cyc__genrep_118}; static struct _tuple4* Cyc__genarr_132[ 4u]={& Cyc__gentuple_128,&
Cyc__gentuple_129,& Cyc__gentuple_130,& Cyc__gentuple_131}; struct Cyc_Typerep_Tuple_struct
Cyc_Absyn_forarray_info_t_rep={ 4u, sizeof( struct Cyc_Absyn_ForArrayInfo),{(
void*)(( struct _tuple4**) Cyc__genarr_132),( void*)(( struct _tuple4**) Cyc__genarr_132),(
void*)(( struct _tuple4**) Cyc__genarr_132 +  4u)}}; struct _tuple51{
unsigned int f1; struct Cyc_Absyn_ForArrayInfo f2; } ; static struct _tuple4 Cyc__gentuple_133={
offsetof( struct _tuple51,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_134={ offsetof( struct _tuple51,f2),( void*)& Cyc_Absyn_forarray_info_t_rep};
static struct _tuple4* Cyc__genarr_135[ 2u]={& Cyc__gentuple_133,& Cyc__gentuple_134};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_123={ 4u, sizeof( struct
_tuple51),{( void*)(( struct _tuple4**) Cyc__genarr_135),( void*)(( struct
_tuple4**) Cyc__genarr_135),( void*)(( struct _tuple4**) Cyc__genarr_135 +  2u)}};
static struct _tuple4 Cyc__gentuple_445={ 0,( void*)& Cyc__genrep_65}; static
struct _tuple4 Cyc__gentuple_446={ 1,( void*)& Cyc__genrep_440}; static struct
_tuple4 Cyc__gentuple_447={ 2,( void*)& Cyc__genrep_436}; static struct _tuple4
Cyc__gentuple_448={ 3,( void*)& Cyc__genrep_430}; static struct _tuple4 Cyc__gentuple_449={
4,( void*)& Cyc__genrep_425}; static struct _tuple4 Cyc__gentuple_450={ 5,( void*)&
Cyc__genrep_421}; static struct _tuple4 Cyc__gentuple_451={ 6,( void*)& Cyc__genrep_421};
static struct _tuple4 Cyc__gentuple_452={ 7,( void*)& Cyc__genrep_415}; static
struct _tuple4 Cyc__gentuple_453={ 8,( void*)& Cyc__genrep_408}; static struct
_tuple4 Cyc__gentuple_454={ 9,( void*)& Cyc__genrep_403}; static struct _tuple4
Cyc__gentuple_455={ 10,( void*)& Cyc__genrep_389}; static struct _tuple4 Cyc__gentuple_456={
11,( void*)& Cyc__genrep_379}; static struct _tuple4 Cyc__gentuple_457={ 12,(
void*)& Cyc__genrep_374}; static struct _tuple4 Cyc__gentuple_458={ 13,( void*)&
Cyc__genrep_370}; static struct _tuple4 Cyc__gentuple_459={ 14,( void*)& Cyc__genrep_370};
static struct _tuple4 Cyc__gentuple_460={ 15,( void*)& Cyc__genrep_365}; static
struct _tuple4 Cyc__gentuple_461={ 16,( void*)& Cyc__genrep_360}; static struct
_tuple4 Cyc__gentuple_462={ 17,( void*)& Cyc__genrep_169}; static struct _tuple4
Cyc__gentuple_463={ 18,( void*)& Cyc__genrep_136}; static struct _tuple4 Cyc__gentuple_464={
19,( void*)& Cyc__genrep_123}; static struct _tuple4* Cyc__genarr_465[ 20u]={&
Cyc__gentuple_445,& Cyc__gentuple_446,& Cyc__gentuple_447,& Cyc__gentuple_448,&
Cyc__gentuple_449,& Cyc__gentuple_450,& Cyc__gentuple_451,& Cyc__gentuple_452,&
Cyc__gentuple_453,& Cyc__gentuple_454,& Cyc__gentuple_455,& Cyc__gentuple_456,&
Cyc__gentuple_457,& Cyc__gentuple_458,& Cyc__gentuple_459,& Cyc__gentuple_460,&
Cyc__gentuple_461,& Cyc__gentuple_462,& Cyc__gentuple_463,& Cyc__gentuple_464};
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep={ 5u,{( void*)((
struct _tuple4**) Cyc__genarr_465),( void*)(( struct _tuple4**) Cyc__genarr_465),(
void*)(( struct _tuple4**) Cyc__genarr_465 +  20u)}}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_119; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep;
static struct _tuple4 Cyc__gentuple_120={ offsetof( struct Cyc_List_List,hd),(
void*)& Cyc__genrep_118}; static struct _tuple4 Cyc__gentuple_121={ offsetof(
struct Cyc_List_List,tl),( void*)& Cyc__genrep_119}; static struct _tuple4* Cyc__genarr_122[
2u]={& Cyc__gentuple_120,& Cyc__gentuple_121}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List0Absyn_stmt_t46H2_rep={ 4u, sizeof( struct Cyc_List_List),{(
void*)(( struct _tuple4**) Cyc__genarr_122),( void*)(( struct _tuple4**) Cyc__genarr_122),(
void*)(( struct _tuple4**) Cyc__genarr_122 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_119={ 2u, 1,( void*)(( void*)& Cyc_struct_List_List0Absyn_stmt_t46H2_rep)};
extern struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep; static
struct _tuple5* Cyc__genarr_64[ 0u]={}; struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep={
6u,{( void*)(( struct _tuple5**) Cyc__genarr_64),( void*)(( struct _tuple5**)
Cyc__genarr_64),( void*)(( struct _tuple5**) Cyc__genarr_64 +  0u)}}; static
struct _tuple4 Cyc__gentuple_466={ offsetof( struct Cyc_Absyn_Stmt,r),( void*)&
Cyc_Absyn_raw_stmt_t_rep}; static struct _tuple4 Cyc__gentuple_467={ offsetof(
struct Cyc_Absyn_Stmt,loc),( void*)& Cyc__genrep_2}; static struct _tuple4 Cyc__gentuple_468={
offsetof( struct Cyc_Absyn_Stmt,non_local_preds),( void*)& Cyc__genrep_119};
static struct _tuple4 Cyc__gentuple_469={ offsetof( struct Cyc_Absyn_Stmt,try_depth),(
void*)(( void*)& Cyc__genrep_74)}; static struct _tuple4 Cyc__gentuple_470={
offsetof( struct Cyc_Absyn_Stmt,annot),( void*)& Cyc_Absyn_absyn_annot_t_rep};
static struct _tuple4* Cyc__genarr_471[ 5u]={& Cyc__gentuple_466,& Cyc__gentuple_467,&
Cyc__gentuple_468,& Cyc__gentuple_469,& Cyc__gentuple_470}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Stmt_rep={ 4u, sizeof( struct Cyc_Absyn_Stmt),{( void*)((
struct _tuple4**) Cyc__genarr_471),( void*)(( struct _tuple4**) Cyc__genarr_471),(
void*)(( struct _tuple4**) Cyc__genarr_471 +  5u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_118={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_Stmt_rep)}; static
struct _tuple4 Cyc__gentuple_500={ offsetof( struct Cyc_Absyn_Fndecl,sc),( void*)&
Cyc_Absyn_scope_t_rep}; static struct _tuple4 Cyc__gentuple_501={ offsetof(
struct Cyc_Absyn_Fndecl,is_inline),( void*)(( void*)& Cyc__genrep_74)}; static
struct _tuple4 Cyc__gentuple_502={ offsetof( struct Cyc_Absyn_Fndecl,name),(
void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_503={ offsetof(
struct Cyc_Absyn_Fndecl,tvs),( void*)& Cyc__genrep_194}; static struct _tuple4
Cyc__gentuple_504={ offsetof( struct Cyc_Absyn_Fndecl,effect),( void*)& Cyc__genrep_52};
static struct _tuple4 Cyc__gentuple_505={ offsetof( struct Cyc_Absyn_Fndecl,ret_type),(
void*)(( void*)& Cyc_Absyn_type_t_rep)}; static struct _tuple4 Cyc__gentuple_506={
offsetof( struct Cyc_Absyn_Fndecl,args),( void*)& Cyc__genrep_490}; static
struct _tuple4 Cyc__gentuple_507={ offsetof( struct Cyc_Absyn_Fndecl,c_varargs),(
void*)(( void*)& Cyc__genrep_74)}; static struct _tuple4 Cyc__gentuple_508={
offsetof( struct Cyc_Absyn_Fndecl,cyc_varargs),( void*)& Cyc__genrep_481};
static struct _tuple4 Cyc__gentuple_509={ offsetof( struct Cyc_Absyn_Fndecl,rgn_po),(
void*)& Cyc__genrep_472}; static struct _tuple4 Cyc__gentuple_510={ offsetof(
struct Cyc_Absyn_Fndecl,body),( void*)& Cyc__genrep_118}; static struct _tuple4
Cyc__gentuple_511={ offsetof( struct Cyc_Absyn_Fndecl,cached_typ),( void*)& Cyc__genrep_52};
static struct _tuple4 Cyc__gentuple_512={ offsetof( struct Cyc_Absyn_Fndecl,param_vardecls),(
void*)& Cyc__genrep_97}; static struct _tuple4 Cyc__gentuple_513={ offsetof(
struct Cyc_Absyn_Fndecl,attributes),( void*)& Cyc__genrep_72}; static struct
_tuple4* Cyc__genarr_514[ 14u]={& Cyc__gentuple_500,& Cyc__gentuple_501,& Cyc__gentuple_502,&
Cyc__gentuple_503,& Cyc__gentuple_504,& Cyc__gentuple_505,& Cyc__gentuple_506,&
Cyc__gentuple_507,& Cyc__gentuple_508,& Cyc__gentuple_509,& Cyc__gentuple_510,&
Cyc__gentuple_511,& Cyc__gentuple_512,& Cyc__gentuple_513}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Fndecl_rep={ 4u, sizeof( struct Cyc_Absyn_Fndecl),{( void*)((
struct _tuple4**) Cyc__genarr_514),( void*)(( struct _tuple4**) Cyc__genarr_514),(
void*)(( struct _tuple4**) Cyc__genarr_514 +  14u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_71={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_Fndecl_rep)}; struct
_tuple52{ unsigned int f1; struct Cyc_Absyn_Fndecl* f2; } ; static struct
_tuple4 Cyc__gentuple_515={ offsetof( struct _tuple52,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_516={ offsetof( struct _tuple52,f2),( void*)&
Cyc__genrep_71}; static struct _tuple4* Cyc__genarr_517[ 2u]={& Cyc__gentuple_515,&
Cyc__gentuple_516}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_70={ 4u,
sizeof( struct _tuple52),{( void*)(( struct _tuple4**) Cyc__genarr_517),( void*)((
struct _tuple4**) Cyc__genarr_517),( void*)(( struct _tuple4**) Cyc__genarr_517
+  2u)}}; static struct _tuple4 Cyc__gentuple_690={ 0,( void*)& Cyc__genrep_306};
static struct _tuple4 Cyc__gentuple_691={ 1,( void*)& Cyc__genrep_70}; static
struct _tuple4 Cyc__gentuple_692={ 2,( void*)& Cyc__genrep_306}; static struct
_tuple4 Cyc__gentuple_693={ 3,( void*)& Cyc__genrep_306}; static struct _tuple4
Cyc__gentuple_694={ 4,( void*)& Cyc__genrep_306}; static struct _tuple4* Cyc__genarr_695[
5u]={& Cyc__gentuple_690,& Cyc__gentuple_691,& Cyc__gentuple_692,& Cyc__gentuple_693,&
Cyc__gentuple_694}; struct Cyc_Typerep_TUnion_struct Cyc_Absyn_binding_t_rep={ 5u,{(
void*)(( struct _tuple4**) Cyc__genarr_695),( void*)(( struct _tuple4**) Cyc__genarr_695),(
void*)(( struct _tuple4**) Cyc__genarr_695 +  5u)}}; struct _tuple53{
unsigned int f1; struct _tuple0* f2; void* f3; } ; static struct _tuple4 Cyc__gentuple_696={
offsetof( struct _tuple53,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_697={ offsetof( struct _tuple53,f2),( void*)& Cyc__genrep_9};
static struct _tuple4 Cyc__gentuple_698={ offsetof( struct _tuple53,f3),( void*)&
Cyc_Absyn_binding_t_rep}; static struct _tuple4* Cyc__genarr_699[ 3u]={& Cyc__gentuple_696,&
Cyc__gentuple_697,& Cyc__gentuple_698}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_689={ 4u, sizeof( struct _tuple53),{( void*)(( struct _tuple4**) Cyc__genarr_699),(
void*)(( struct _tuple4**) Cyc__genarr_699),( void*)(( struct _tuple4**) Cyc__genarr_699
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_684; extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep; static struct _tuple4* Cyc__genarr_676[
0u]={}; struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep={ 5u,{( void*)((
struct _tuple4**) Cyc__genarr_676),( void*)(( struct _tuple4**) Cyc__genarr_676),(
void*)(( struct _tuple4**) Cyc__genarr_676 +  0u)}}; struct _tuple54{
unsigned int f1; void* f2; struct Cyc_List_List* f3; } ; static struct _tuple4
Cyc__gentuple_685={ offsetof( struct _tuple54,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_686={ offsetof( struct _tuple54,f2),( void*)&
Cyc_Absyn_primop_t_rep}; static struct _tuple4 Cyc__gentuple_687={ offsetof(
struct _tuple54,f3),( void*)& Cyc__genrep_381}; static struct _tuple4* Cyc__genarr_688[
3u]={& Cyc__gentuple_685,& Cyc__gentuple_686,& Cyc__gentuple_687}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_684={ 4u, sizeof( struct _tuple54),{( void*)((
struct _tuple4**) Cyc__genarr_688),( void*)(( struct _tuple4**) Cyc__genarr_688),(
void*)(( struct _tuple4**) Cyc__genarr_688 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_674; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_675;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep;
static struct _tuple4 Cyc__gentuple_677={ offsetof( struct Cyc_Core_Opt,v),(
void*)& Cyc_Absyn_primop_t_rep}; static struct _tuple4* Cyc__genarr_678[ 1u]={&
Cyc__gentuple_677}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep={
4u, sizeof( struct Cyc_Core_Opt),{( void*)(( struct _tuple4**) Cyc__genarr_678),(
void*)(( struct _tuple4**) Cyc__genarr_678),( void*)(( struct _tuple4**) Cyc__genarr_678
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_675={ 2u, 1,(
void*)(( void*)& Cyc_struct_Core_Opt0Absyn_primop_t2_rep)}; struct _tuple55{
unsigned int f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp*
f4; } ; static struct _tuple4 Cyc__gentuple_679={ offsetof( struct _tuple55,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_680={ offsetof(
struct _tuple55,f2),( void*)& Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_681={
offsetof( struct _tuple55,f3),( void*)& Cyc__genrep_675}; static struct _tuple4
Cyc__gentuple_682={ offsetof( struct _tuple55,f4),( void*)& Cyc__genrep_66};
static struct _tuple4* Cyc__genarr_683[ 4u]={& Cyc__gentuple_679,& Cyc__gentuple_680,&
Cyc__gentuple_681,& Cyc__gentuple_682}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_674={ 4u, sizeof( struct _tuple55),{( void*)(( struct _tuple4**) Cyc__genarr_683),(
void*)(( struct _tuple4**) Cyc__genarr_683),( void*)(( struct _tuple4**) Cyc__genarr_683
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_668; extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep; static struct _tuple4*
Cyc__genarr_669[ 0u]={}; struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_669),( void*)(( struct _tuple4**)
Cyc__genarr_669),( void*)(( struct _tuple4**) Cyc__genarr_669 +  0u)}}; struct
_tuple56{ unsigned int f1; struct Cyc_Absyn_Exp* f2; void* f3; } ; static struct
_tuple4 Cyc__gentuple_670={ offsetof( struct _tuple56,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_671={ offsetof( struct _tuple56,f2),( void*)&
Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_672={ offsetof( struct
_tuple56,f3),( void*)& Cyc_Absyn_incrementor_t_rep}; static struct _tuple4* Cyc__genarr_673[
3u]={& Cyc__gentuple_670,& Cyc__gentuple_671,& Cyc__gentuple_672}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_668={ 4u, sizeof( struct _tuple56),{( void*)((
struct _tuple4**) Cyc__genarr_673),( void*)(( struct _tuple4**) Cyc__genarr_673),(
void*)(( struct _tuple4**) Cyc__genarr_673 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_662; struct _tuple57{ unsigned int f1; struct Cyc_Absyn_Exp* f2;
struct Cyc_Absyn_Exp* f3; struct Cyc_Absyn_Exp* f4; } ; static struct _tuple4
Cyc__gentuple_663={ offsetof( struct _tuple57,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_664={ offsetof( struct _tuple57,f2),( void*)&
Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_665={ offsetof( struct
_tuple57,f3),( void*)& Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_666={
offsetof( struct _tuple57,f4),( void*)& Cyc__genrep_66}; static struct _tuple4*
Cyc__genarr_667[ 4u]={& Cyc__gentuple_663,& Cyc__gentuple_664,& Cyc__gentuple_665,&
Cyc__gentuple_666}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_662={ 4u,
sizeof( struct _tuple57),{( void*)(( struct _tuple4**) Cyc__genarr_667),( void*)((
struct _tuple4**) Cyc__genarr_667),( void*)(( struct _tuple4**) Cyc__genarr_667
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_603; struct
_tuple58{ unsigned int f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; }
; static struct _tuple4 Cyc__gentuple_604={ offsetof( struct _tuple58,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_605={ offsetof( struct
_tuple58,f2),( void*)& Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_606={
offsetof( struct _tuple58,f3),( void*)& Cyc__genrep_66}; static struct _tuple4*
Cyc__genarr_607[ 3u]={& Cyc__gentuple_604,& Cyc__gentuple_605,& Cyc__gentuple_606};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_603={ 4u, sizeof( struct
_tuple58),{( void*)(( struct _tuple4**) Cyc__genarr_607),( void*)(( struct
_tuple4**) Cyc__genarr_607),( void*)(( struct _tuple4**) Cyc__genarr_607 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_657; static struct _tuple4
Cyc__gentuple_658={ offsetof( struct _tuple45,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_659={ offsetof( struct _tuple45,f2),( void*)&
Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_660={ offsetof( struct
_tuple45,f3),( void*)& Cyc__genrep_381}; static struct _tuple4* Cyc__genarr_661[
3u]={& Cyc__gentuple_658,& Cyc__gentuple_659,& Cyc__gentuple_660}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_657={ 4u, sizeof( struct _tuple45),{( void*)((
struct _tuple4**) Cyc__genarr_661),( void*)(( struct _tuple4**) Cyc__genarr_661),(
void*)(( struct _tuple4**) Cyc__genarr_661 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_645; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_646;
extern struct Cyc_Typerep_Tuple_struct Cyc_Absyn_vararg_call_info_t_rep; extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_647; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_647={ 2u, 1,( void*)(( void*)& Cyc_Absyn_vararg_info_t_rep)}; static
struct _tuple4 Cyc__gentuple_648={ offsetof( struct Cyc_Absyn_VarargCallInfo,num_varargs),(
void*)(( void*)& Cyc__genrep_74)}; static struct _tuple4 Cyc__gentuple_649={
offsetof( struct Cyc_Absyn_VarargCallInfo,injectors),( void*)& Cyc__genrep_254};
static struct _tuple4 Cyc__gentuple_650={ offsetof( struct Cyc_Absyn_VarargCallInfo,vai),(
void*)& Cyc__genrep_647}; static struct _tuple4* Cyc__genarr_651[ 3u]={& Cyc__gentuple_648,&
Cyc__gentuple_649,& Cyc__gentuple_650}; struct Cyc_Typerep_Tuple_struct Cyc_Absyn_vararg_call_info_t_rep={
4u, sizeof( struct Cyc_Absyn_VarargCallInfo),{( void*)(( struct _tuple4**) Cyc__genarr_651),(
void*)(( struct _tuple4**) Cyc__genarr_651),( void*)(( struct _tuple4**) Cyc__genarr_651
+  3u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_646={ 2u, 1,(
void*)(( void*)& Cyc_Absyn_vararg_call_info_t_rep)}; struct _tuple59{
unsigned int f1; struct Cyc_Absyn_Exp* f2; struct Cyc_List_List* f3; struct Cyc_Absyn_VarargCallInfo*
f4; } ; static struct _tuple4 Cyc__gentuple_652={ offsetof( struct _tuple59,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_653={ offsetof(
struct _tuple59,f2),( void*)& Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_654={
offsetof( struct _tuple59,f3),( void*)& Cyc__genrep_381}; static struct _tuple4
Cyc__gentuple_655={ offsetof( struct _tuple59,f4),( void*)& Cyc__genrep_646};
static struct _tuple4* Cyc__genarr_656[ 4u]={& Cyc__gentuple_652,& Cyc__gentuple_653,&
Cyc__gentuple_654,& Cyc__gentuple_655}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_645={ 4u, sizeof( struct _tuple59),{( void*)(( struct _tuple4**) Cyc__genarr_656),(
void*)(( struct _tuple4**) Cyc__genarr_656),( void*)(( struct _tuple4**) Cyc__genarr_656
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_640; static struct
_tuple4 Cyc__gentuple_641={ offsetof( struct _tuple45,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_642={ offsetof( struct _tuple45,f2),( void*)&
Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_643={ offsetof( struct
_tuple45,f3),( void*)& Cyc__genrep_44}; static struct _tuple4* Cyc__genarr_644[
3u]={& Cyc__gentuple_641,& Cyc__gentuple_642,& Cyc__gentuple_643}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_640={ 4u, sizeof( struct _tuple45),{( void*)((
struct _tuple4**) Cyc__genarr_644),( void*)(( struct _tuple4**) Cyc__genarr_644),(
void*)(( struct _tuple4**) Cyc__genarr_644 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_635; struct _tuple60{ unsigned int f1; void* f2; struct Cyc_Absyn_Exp*
f3; } ; static struct _tuple4 Cyc__gentuple_636={ offsetof( struct _tuple60,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_637={ offsetof(
struct _tuple60,f2),( void*)(( void*)& Cyc_Absyn_type_t_rep)}; static struct
_tuple4 Cyc__gentuple_638={ offsetof( struct _tuple60,f3),( void*)& Cyc__genrep_66};
static struct _tuple4* Cyc__genarr_639[ 3u]={& Cyc__gentuple_636,& Cyc__gentuple_637,&
Cyc__gentuple_638}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_635={ 4u,
sizeof( struct _tuple60),{( void*)(( struct _tuple4**) Cyc__genarr_639),( void*)((
struct _tuple4**) Cyc__genarr_639),( void*)(( struct _tuple4**) Cyc__genarr_639
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_630; static struct
_tuple4 Cyc__gentuple_631={ offsetof( struct _tuple58,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_632={ offsetof( struct _tuple58,f2),( void*)&
Cyc__genrep_63}; static struct _tuple4 Cyc__gentuple_633={ offsetof( struct
_tuple58,f3),( void*)& Cyc__genrep_66}; static struct _tuple4* Cyc__genarr_634[
3u]={& Cyc__gentuple_631,& Cyc__gentuple_632,& Cyc__gentuple_633}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_630={ 4u, sizeof( struct _tuple58),{( void*)((
struct _tuple4**) Cyc__genarr_634),( void*)(( struct _tuple4**) Cyc__genarr_634),(
void*)(( struct _tuple4**) Cyc__genarr_634 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_39; static struct _tuple4 Cyc__gentuple_40={ offsetof( struct
_tuple4,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_41={
offsetof( struct _tuple4,f2),( void*)(( void*)& Cyc_Absyn_type_t_rep)}; static
struct _tuple4* Cyc__genarr_42[ 2u]={& Cyc__gentuple_40,& Cyc__gentuple_41};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_39={ 4u, sizeof( struct
_tuple4),{( void*)(( struct _tuple4**) Cyc__genarr_42),( void*)(( struct _tuple4**)
Cyc__genarr_42),( void*)(( struct _tuple4**) Cyc__genarr_42 +  2u)}}; extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_618; extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_offsetof_field_t_rep; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_619;
struct _tuple61{ unsigned int f1; unsigned int f2; } ; static struct _tuple4 Cyc__gentuple_620={
offsetof( struct _tuple61,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_621={ offsetof( struct _tuple61,f2),( void*)& Cyc__genrep_4};
static struct _tuple4* Cyc__genarr_622[ 2u]={& Cyc__gentuple_620,& Cyc__gentuple_621};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_619={ 4u, sizeof( struct
_tuple61),{( void*)(( struct _tuple4**) Cyc__genarr_622),( void*)(( struct
_tuple4**) Cyc__genarr_622),( void*)(( struct _tuple4**) Cyc__genarr_622 +  2u)}};
static struct _tuple4 Cyc__gentuple_623={ 0,( void*)& Cyc__genrep_178}; static
struct _tuple4 Cyc__gentuple_624={ 1,( void*)& Cyc__genrep_619}; static struct
_tuple4* Cyc__genarr_625[ 2u]={& Cyc__gentuple_623,& Cyc__gentuple_624}; struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep={ 5u,{( void*)(( struct
_tuple4**) Cyc__genarr_625),( void*)(( struct _tuple4**) Cyc__genarr_625),( void*)((
struct _tuple4**) Cyc__genarr_625 +  2u)}}; struct _tuple62{ unsigned int f1;
void* f2; void* f3; } ; static struct _tuple4 Cyc__gentuple_626={ offsetof(
struct _tuple62,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_627={
offsetof( struct _tuple62,f2),( void*)(( void*)& Cyc_Absyn_type_t_rep)}; static
struct _tuple4 Cyc__gentuple_628={ offsetof( struct _tuple62,f3),( void*)& Cyc_Absyn_offsetof_field_t_rep};
static struct _tuple4* Cyc__genarr_629[ 3u]={& Cyc__gentuple_626,& Cyc__gentuple_627,&
Cyc__gentuple_628}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_618={ 4u,
sizeof( struct _tuple62),{( void*)(( struct _tuple4**) Cyc__genarr_629),( void*)((
struct _tuple4**) Cyc__genarr_629),( void*)(( struct _tuple4**) Cyc__genarr_629
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_613; struct
_tuple63{ unsigned int f1; struct Cyc_List_List* f2; void* f3; } ; static struct
_tuple4 Cyc__gentuple_614={ offsetof( struct _tuple63,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_615={ offsetof( struct _tuple63,f2),( void*)&
Cyc__genrep_194}; static struct _tuple4 Cyc__gentuple_616={ offsetof( struct
_tuple63,f3),( void*)(( void*)& Cyc_Absyn_type_t_rep)}; static struct _tuple4*
Cyc__genarr_617[ 3u]={& Cyc__gentuple_614,& Cyc__gentuple_615,& Cyc__gentuple_616};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_613={ 4u, sizeof( struct
_tuple63),{( void*)(( struct _tuple4**) Cyc__genarr_617),( void*)(( struct
_tuple4**) Cyc__genarr_617),( void*)(( struct _tuple4**) Cyc__genarr_617 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_608; struct _tuple64{
unsigned int f1; struct Cyc_Absyn_Exp* f2; struct _tagged_arr* f3; } ; static
struct _tuple4 Cyc__gentuple_609={ offsetof( struct _tuple64,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_610={ offsetof( struct _tuple64,f2),( void*)&
Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_611={ offsetof( struct
_tuple64,f3),( void*)& Cyc__genrep_11}; static struct _tuple4* Cyc__genarr_612[
3u]={& Cyc__gentuple_609,& Cyc__gentuple_610,& Cyc__gentuple_611}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_608={ 4u, sizeof( struct _tuple64),{( void*)((
struct _tuple4**) Cyc__genarr_612),( void*)(( struct _tuple4**) Cyc__genarr_612),(
void*)(( struct _tuple4**) Cyc__genarr_612 +  3u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_599; static struct _tuple4 Cyc__gentuple_600={ offsetof( struct
_tuple8,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_601={
offsetof( struct _tuple8,f2),( void*)& Cyc__genrep_381}; static struct _tuple4*
Cyc__genarr_602[ 2u]={& Cyc__gentuple_600,& Cyc__gentuple_601}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_599={ 4u, sizeof( struct _tuple8),{( void*)((
struct _tuple4**) Cyc__genarr_602),( void*)(( struct _tuple4**) Cyc__genarr_602),(
void*)(( struct _tuple4**) Cyc__genarr_602 +  2u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_588; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_589;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_590; static struct _tuple4
Cyc__gentuple_591={ offsetof( struct _tuple1,f1),( void*)& Cyc__genrep_482};
static struct _tuple4 Cyc__gentuple_592={ offsetof( struct _tuple1,f2),( void*)&
Cyc__genrep_100}; static struct _tuple4 Cyc__gentuple_593={ offsetof( struct
_tuple1,f3),( void*)(( void*)& Cyc_Absyn_type_t_rep)}; static struct _tuple4*
Cyc__genarr_594[ 3u]={& Cyc__gentuple_591,& Cyc__gentuple_592,& Cyc__gentuple_593};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_590={ 4u, sizeof( struct
_tuple1),{( void*)(( struct _tuple4**) Cyc__genarr_594),( void*)(( struct
_tuple4**) Cyc__genarr_594),( void*)(( struct _tuple4**) Cyc__genarr_594 +  3u)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_589={ 2u, 1,( void*)(( void*)&
Cyc__genrep_590)}; extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_519;
extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_520; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_521; static struct _tuple4 Cyc__gentuple_522={ offsetof( struct
_tuple7,f1),( void*)& Cyc__genrep_177}; static struct _tuple4 Cyc__gentuple_523={
offsetof( struct _tuple7,f2),( void*)& Cyc__genrep_66}; static struct _tuple4*
Cyc__genarr_524[ 2u]={& Cyc__gentuple_522,& Cyc__gentuple_523}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_521={ 4u, sizeof( struct _tuple7),{( void*)((
struct _tuple4**) Cyc__genarr_524),( void*)(( struct _tuple4**) Cyc__genarr_524),(
void*)(( struct _tuple4**) Cyc__genarr_524 +  2u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_520={ 2u, 1,( void*)(( void*)& Cyc__genrep_521)}; static struct
_tuple4 Cyc__gentuple_525={ offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_520};
static struct _tuple4 Cyc__gentuple_526={ offsetof( struct Cyc_List_List,tl),(
void*)& Cyc__genrep_519}; static struct _tuple4* Cyc__genarr_527[ 2u]={& Cyc__gentuple_525,&
Cyc__gentuple_526}; struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_527),(
void*)(( struct _tuple4**) Cyc__genarr_527),( void*)(( struct _tuple4**) Cyc__genarr_527
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_519={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep)};
struct _tuple65{ unsigned int f1; struct _tuple1* f2; struct Cyc_List_List* f3;
} ; static struct _tuple4 Cyc__gentuple_595={ offsetof( struct _tuple65,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_596={ offsetof(
struct _tuple65,f2),( void*)& Cyc__genrep_589}; static struct _tuple4 Cyc__gentuple_597={
offsetof( struct _tuple65,f3),( void*)& Cyc__genrep_519}; static struct _tuple4*
Cyc__genarr_598[ 3u]={& Cyc__gentuple_595,& Cyc__gentuple_596,& Cyc__gentuple_597};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_588={ 4u, sizeof( struct
_tuple65),{( void*)(( struct _tuple4**) Cyc__genarr_598),( void*)(( struct
_tuple4**) Cyc__genarr_598),( void*)(( struct _tuple4**) Cyc__genarr_598 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_584; static struct _tuple4
Cyc__gentuple_585={ offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_586={ offsetof( struct _tuple8,f2),( void*)&
Cyc__genrep_519}; static struct _tuple4* Cyc__genarr_587[ 2u]={& Cyc__gentuple_585,&
Cyc__gentuple_586}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_584={ 4u,
sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_587),( void*)((
struct _tuple4**) Cyc__genarr_587),( void*)(( struct _tuple4**) Cyc__genarr_587
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_578; struct
_tuple66{ unsigned int f1; struct Cyc_Absyn_Vardecl* f2; struct Cyc_Absyn_Exp*
f3; struct Cyc_Absyn_Exp* f4; } ; static struct _tuple4 Cyc__gentuple_579={
offsetof( struct _tuple66,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_580={ offsetof( struct _tuple66,f2),( void*)& Cyc__genrep_99};
static struct _tuple4 Cyc__gentuple_581={ offsetof( struct _tuple66,f3),( void*)&
Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_582={ offsetof( struct
_tuple66,f4),( void*)& Cyc__genrep_66}; static struct _tuple4* Cyc__genarr_583[
4u]={& Cyc__gentuple_579,& Cyc__gentuple_580,& Cyc__gentuple_581,& Cyc__gentuple_582};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_578={ 4u, sizeof( struct
_tuple66),{( void*)(( struct _tuple4**) Cyc__genarr_583),( void*)(( struct
_tuple4**) Cyc__genarr_583),( void*)(( struct _tuple4**) Cyc__genarr_583 +  4u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_570; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_571; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_571={ 2u,
1,( void*)(( void*)& Cyc_struct_Absyn_Structdecl_rep)}; struct _tuple67{
unsigned int f1; struct _tuple0* f2; struct Cyc_Core_Opt* f3; struct Cyc_List_List*
f4; struct Cyc_Absyn_Structdecl* f5; } ; static struct _tuple4 Cyc__gentuple_572={
offsetof( struct _tuple67,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_573={ offsetof( struct _tuple67,f2),( void*)& Cyc__genrep_9};
static struct _tuple4 Cyc__gentuple_574={ offsetof( struct _tuple67,f3),( void*)&
Cyc__genrep_273}; static struct _tuple4 Cyc__gentuple_575={ offsetof( struct
_tuple67,f4),( void*)& Cyc__genrep_519}; static struct _tuple4 Cyc__gentuple_576={
offsetof( struct _tuple67,f5),( void*)& Cyc__genrep_571}; static struct _tuple4*
Cyc__genarr_577[ 5u]={& Cyc__gentuple_572,& Cyc__gentuple_573,& Cyc__gentuple_574,&
Cyc__gentuple_575,& Cyc__gentuple_576}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_570={ 4u, sizeof( struct _tuple67),{( void*)(( struct _tuple4**) Cyc__genarr_577),(
void*)(( struct _tuple4**) Cyc__genarr_577),( void*)(( struct _tuple4**) Cyc__genarr_577
+  5u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_565; static struct
_tuple4 Cyc__gentuple_566={ offsetof( struct _tuple54,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_567={ offsetof( struct _tuple54,f2),( void*)((
void*)& Cyc_Absyn_type_t_rep)}; static struct _tuple4 Cyc__gentuple_568={
offsetof( struct _tuple54,f3),( void*)& Cyc__genrep_519}; static struct _tuple4*
Cyc__genarr_569[ 3u]={& Cyc__gentuple_566,& Cyc__gentuple_567,& Cyc__gentuple_568};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_565={ 4u, sizeof( struct
_tuple54),{( void*)(( struct _tuple4**) Cyc__genarr_569),( void*)(( struct
_tuple4**) Cyc__genarr_569),( void*)(( struct _tuple4**) Cyc__genarr_569 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_557; struct _tuple68{
unsigned int f1; struct Cyc_Core_Opt* f2; struct Cyc_Core_Opt* f3; struct Cyc_List_List*
f4; struct Cyc_Absyn_Tuniondecl* f5; struct Cyc_Absyn_Tunionfield* f6; } ;
static struct _tuple4 Cyc__gentuple_558={ offsetof( struct _tuple68,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_559={ offsetof( struct
_tuple68,f2),( void*)& Cyc__genrep_273}; static struct _tuple4 Cyc__gentuple_560={
offsetof( struct _tuple68,f3),( void*)& Cyc__genrep_273}; static struct _tuple4
Cyc__gentuple_561={ offsetof( struct _tuple68,f4),( void*)& Cyc__genrep_381};
static struct _tuple4 Cyc__gentuple_562={ offsetof( struct _tuple68,f5),( void*)((
void*)& Cyc__genrep_252)}; static struct _tuple4 Cyc__gentuple_563={ offsetof(
struct _tuple68,f6),( void*)& Cyc__genrep_236}; static struct _tuple4* Cyc__genarr_564[
6u]={& Cyc__gentuple_558,& Cyc__gentuple_559,& Cyc__gentuple_560,& Cyc__gentuple_561,&
Cyc__gentuple_562,& Cyc__gentuple_563}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_557={ 4u, sizeof( struct _tuple68),{( void*)(( struct _tuple4**) Cyc__genarr_564),(
void*)(( struct _tuple4**) Cyc__genarr_564),( void*)(( struct _tuple4**) Cyc__genarr_564
+  6u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_550; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_551; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_551={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_Enumdecl_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_544; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_544={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_Enumfield_rep)};
struct _tuple69{ unsigned int f1; struct _tuple0* f2; struct Cyc_Absyn_Enumdecl*
f3; struct Cyc_Absyn_Enumfield* f4; } ; static struct _tuple4 Cyc__gentuple_552={
offsetof( struct _tuple69,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_553={ offsetof( struct _tuple69,f2),( void*)& Cyc__genrep_9};
static struct _tuple4 Cyc__gentuple_554={ offsetof( struct _tuple69,f3),( void*)&
Cyc__genrep_551}; static struct _tuple4 Cyc__gentuple_555={ offsetof( struct
_tuple69,f4),( void*)& Cyc__genrep_544}; static struct _tuple4* Cyc__genarr_556[
4u]={& Cyc__gentuple_552,& Cyc__gentuple_553,& Cyc__gentuple_554,& Cyc__gentuple_555};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_550={ 4u, sizeof( struct
_tuple69),{( void*)(( struct _tuple4**) Cyc__genarr_556),( void*)(( struct
_tuple4**) Cyc__genarr_556),( void*)(( struct _tuple4**) Cyc__genarr_556 +  4u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_543; struct _tuple70{
unsigned int f1; struct _tuple0* f2; void* f3; struct Cyc_Absyn_Enumfield* f4; }
; static struct _tuple4 Cyc__gentuple_545={ offsetof( struct _tuple70,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_546={ offsetof( struct
_tuple70,f2),( void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_547={
offsetof( struct _tuple70,f3),( void*)(( void*)& Cyc_Absyn_type_t_rep)}; static
struct _tuple4 Cyc__gentuple_548={ offsetof( struct _tuple70,f4),( void*)& Cyc__genrep_544};
static struct _tuple4* Cyc__genarr_549[ 4u]={& Cyc__gentuple_545,& Cyc__gentuple_546,&
Cyc__gentuple_547,& Cyc__gentuple_548}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_543={ 4u, sizeof( struct _tuple70),{( void*)(( struct _tuple4**) Cyc__genarr_549),(
void*)(( struct _tuple4**) Cyc__genarr_549),( void*)(( struct _tuple4**) Cyc__genarr_549
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_532; extern struct
Cyc_Typerep_Tuple_struct Cyc_Absyn_malloc_info_t_rep; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_533; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_533={ 2u,
1,( void*)(( void*)& Cyc_Absyn_type_t_rep)}; static struct _tuple4 Cyc__gentuple_534={
offsetof( struct Cyc_Absyn_MallocInfo,is_calloc),( void*)(( void*)& Cyc__genrep_74)};
static struct _tuple4 Cyc__gentuple_535={ offsetof( struct Cyc_Absyn_MallocInfo,rgn),(
void*)& Cyc__genrep_63}; static struct _tuple4 Cyc__gentuple_536={ offsetof(
struct Cyc_Absyn_MallocInfo,elt_type),( void*)& Cyc__genrep_533}; static struct
_tuple4 Cyc__gentuple_537={ offsetof( struct Cyc_Absyn_MallocInfo,num_elts),(
void*)& Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_538={ offsetof(
struct Cyc_Absyn_MallocInfo,fat_result),( void*)(( void*)& Cyc__genrep_74)};
static struct _tuple4* Cyc__genarr_539[ 5u]={& Cyc__gentuple_534,& Cyc__gentuple_535,&
Cyc__gentuple_536,& Cyc__gentuple_537,& Cyc__gentuple_538}; struct Cyc_Typerep_Tuple_struct
Cyc_Absyn_malloc_info_t_rep={ 4u, sizeof( struct Cyc_Absyn_MallocInfo),{( void*)((
struct _tuple4**) Cyc__genarr_539),( void*)(( struct _tuple4**) Cyc__genarr_539),(
void*)(( struct _tuple4**) Cyc__genarr_539 +  5u)}}; struct _tuple71{
unsigned int f1; struct Cyc_Absyn_MallocInfo f2; } ; static struct _tuple4 Cyc__gentuple_540={
offsetof( struct _tuple71,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_541={ offsetof( struct _tuple71,f2),( void*)& Cyc_Absyn_malloc_info_t_rep};
static struct _tuple4* Cyc__genarr_542[ 2u]={& Cyc__gentuple_540,& Cyc__gentuple_541};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_532={ 4u, sizeof( struct
_tuple71),{( void*)(( struct _tuple4**) Cyc__genarr_542),( void*)(( struct
_tuple4**) Cyc__genarr_542),( void*)(( struct _tuple4**) Cyc__genarr_542 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_518; struct _tuple72{
unsigned int f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3; } ; static
struct _tuple4 Cyc__gentuple_528={ offsetof( struct _tuple72,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_529={ offsetof( struct _tuple72,f2),( void*)&
Cyc__genrep_291}; static struct _tuple4 Cyc__gentuple_530={ offsetof( struct
_tuple72,f3),( void*)& Cyc__genrep_519}; static struct _tuple4* Cyc__genarr_531[
3u]={& Cyc__gentuple_528,& Cyc__gentuple_529,& Cyc__gentuple_530}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_518={ 4u, sizeof( struct _tuple72),{( void*)((
struct _tuple4**) Cyc__genarr_531),( void*)(( struct _tuple4**) Cyc__genarr_531),(
void*)(( struct _tuple4**) Cyc__genarr_531 +  3u)}}; static struct _tuple4 Cyc__gentuple_728={
0,( void*)& Cyc__genrep_700}; static struct _tuple4 Cyc__gentuple_729={ 1,( void*)&
Cyc__genrep_689}; static struct _tuple4 Cyc__gentuple_730={ 2,( void*)& Cyc__genrep_213};
static struct _tuple4 Cyc__gentuple_731={ 3,( void*)& Cyc__genrep_684}; static
struct _tuple4 Cyc__gentuple_732={ 4,( void*)& Cyc__genrep_674}; static struct
_tuple4 Cyc__gentuple_733={ 5,( void*)& Cyc__genrep_668}; static struct _tuple4
Cyc__gentuple_734={ 6,( void*)& Cyc__genrep_662}; static struct _tuple4 Cyc__gentuple_735={
7,( void*)& Cyc__genrep_603}; static struct _tuple4 Cyc__gentuple_736={ 8,( void*)&
Cyc__genrep_657}; static struct _tuple4 Cyc__gentuple_737={ 9,( void*)& Cyc__genrep_645};
static struct _tuple4 Cyc__gentuple_738={ 10,( void*)& Cyc__genrep_65}; static
struct _tuple4 Cyc__gentuple_739={ 11,( void*)& Cyc__genrep_65}; static struct
_tuple4 Cyc__gentuple_740={ 12,( void*)& Cyc__genrep_640}; static struct _tuple4
Cyc__gentuple_741={ 13,( void*)& Cyc__genrep_635}; static struct _tuple4 Cyc__gentuple_742={
14,( void*)& Cyc__genrep_65}; static struct _tuple4 Cyc__gentuple_743={ 15,(
void*)& Cyc__genrep_630}; static struct _tuple4 Cyc__gentuple_744={ 16,( void*)&
Cyc__genrep_39}; static struct _tuple4 Cyc__gentuple_745={ 17,( void*)& Cyc__genrep_65};
static struct _tuple4 Cyc__gentuple_746={ 18,( void*)& Cyc__genrep_618}; static
struct _tuple4 Cyc__gentuple_747={ 19,( void*)& Cyc__genrep_613}; static struct
_tuple4 Cyc__gentuple_748={ 20,( void*)& Cyc__genrep_65}; static struct _tuple4
Cyc__gentuple_749={ 21,( void*)& Cyc__genrep_608}; static struct _tuple4 Cyc__gentuple_750={
22,( void*)& Cyc__genrep_608}; static struct _tuple4 Cyc__gentuple_751={ 23,(
void*)& Cyc__genrep_603}; static struct _tuple4 Cyc__gentuple_752={ 24,( void*)&
Cyc__genrep_599}; static struct _tuple4 Cyc__gentuple_753={ 25,( void*)& Cyc__genrep_588};
static struct _tuple4 Cyc__gentuple_754={ 26,( void*)& Cyc__genrep_584}; static
struct _tuple4 Cyc__gentuple_755={ 27,( void*)& Cyc__genrep_578}; static struct
_tuple4 Cyc__gentuple_756={ 28,( void*)& Cyc__genrep_570}; static struct _tuple4
Cyc__gentuple_757={ 29,( void*)& Cyc__genrep_565}; static struct _tuple4 Cyc__gentuple_758={
30,( void*)& Cyc__genrep_557}; static struct _tuple4 Cyc__gentuple_759={ 31,(
void*)& Cyc__genrep_550}; static struct _tuple4 Cyc__gentuple_760={ 32,( void*)&
Cyc__genrep_543}; static struct _tuple4 Cyc__gentuple_761={ 33,( void*)& Cyc__genrep_532};
static struct _tuple4 Cyc__gentuple_762={ 34,( void*)& Cyc__genrep_518}; static
struct _tuple4 Cyc__gentuple_763={ 35,( void*)& Cyc__genrep_370}; static struct
_tuple4 Cyc__gentuple_764={ 36,( void*)& Cyc__genrep_70}; static struct _tuple4
Cyc__gentuple_765={ 37,( void*)& Cyc__genrep_65}; static struct _tuple4* Cyc__genarr_766[
38u]={& Cyc__gentuple_728,& Cyc__gentuple_729,& Cyc__gentuple_730,& Cyc__gentuple_731,&
Cyc__gentuple_732,& Cyc__gentuple_733,& Cyc__gentuple_734,& Cyc__gentuple_735,&
Cyc__gentuple_736,& Cyc__gentuple_737,& Cyc__gentuple_738,& Cyc__gentuple_739,&
Cyc__gentuple_740,& Cyc__gentuple_741,& Cyc__gentuple_742,& Cyc__gentuple_743,&
Cyc__gentuple_744,& Cyc__gentuple_745,& Cyc__gentuple_746,& Cyc__gentuple_747,&
Cyc__gentuple_748,& Cyc__gentuple_749,& Cyc__gentuple_750,& Cyc__gentuple_751,&
Cyc__gentuple_752,& Cyc__gentuple_753,& Cyc__gentuple_754,& Cyc__gentuple_755,&
Cyc__gentuple_756,& Cyc__gentuple_757,& Cyc__gentuple_758,& Cyc__gentuple_759,&
Cyc__gentuple_760,& Cyc__gentuple_761,& Cyc__gentuple_762,& Cyc__gentuple_763,&
Cyc__gentuple_764,& Cyc__gentuple_765}; struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_766),( void*)(( struct _tuple4**)
Cyc__genarr_766),( void*)(( struct _tuple4**) Cyc__genarr_766 +  38u)}}; static
struct _tuple4 Cyc__gentuple_767={ offsetof( struct Cyc_Absyn_Exp,topt),( void*)&
Cyc__genrep_52}; static struct _tuple4 Cyc__gentuple_768={ offsetof( struct Cyc_Absyn_Exp,r),(
void*)& Cyc_Absyn_raw_exp_t_rep}; static struct _tuple4 Cyc__gentuple_769={
offsetof( struct Cyc_Absyn_Exp,loc),( void*)& Cyc__genrep_2}; static struct
_tuple4 Cyc__gentuple_770={ offsetof( struct Cyc_Absyn_Exp,annot),( void*)& Cyc_Absyn_absyn_annot_t_rep};
static struct _tuple4* Cyc__genarr_771[ 4u]={& Cyc__gentuple_767,& Cyc__gentuple_768,&
Cyc__gentuple_769,& Cyc__gentuple_770}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Exp_rep={
4u, sizeof( struct Cyc_Absyn_Exp),{( void*)(( struct _tuple4**) Cyc__genarr_771),(
void*)(( struct _tuple4**) Cyc__genarr_771),( void*)(( struct _tuple4**) Cyc__genarr_771
+  4u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_66={ 2u, 1,( void*)((
void*)& Cyc_struct_Absyn_Exp_rep)}; static struct _tuple4 Cyc__gentuple_67={
offsetof( struct _tuple27,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_68={ offsetof( struct _tuple27,f2),( void*)& Cyc__genrep_66};
static struct _tuple4* Cyc__genarr_69[ 2u]={& Cyc__gentuple_67,& Cyc__gentuple_68};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_65={ 4u, sizeof( struct
_tuple27),{( void*)(( struct _tuple4**) Cyc__genarr_69),( void*)(( struct
_tuple4**) Cyc__genarr_69),( void*)(( struct _tuple4**) Cyc__genarr_69 +  2u)}};
static struct _tuple4 Cyc__gentuple_853={ 0,( void*)& Cyc__genrep_65}; static
struct _tuple4* Cyc__genarr_854[ 1u]={& Cyc__gentuple_853}; struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_bounds_t_rep={ 5u,{( void*)(( struct _tuple4**) Cyc__genarr_854),(
void*)(( struct _tuple4**) Cyc__genarr_854),( void*)(( struct _tuple4**) Cyc__genarr_854
+  1u)}}; static struct _tuple4 Cyc__gentuple_855={ offsetof( struct _tuple4,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_856={ offsetof(
struct _tuple4,f2),( void*)& Cyc_Absyn_bounds_t_rep}; static struct _tuple4* Cyc__genarr_857[
2u]={& Cyc__gentuple_855,& Cyc__gentuple_856}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_852={ 4u, sizeof( struct _tuple4),{( void*)(( struct _tuple4**) Cyc__genarr_857),(
void*)(( struct _tuple4**) Cyc__genarr_857),( void*)(( struct _tuple4**) Cyc__genarr_857
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_848; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_847; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep; static struct _tuple4 Cyc__gentuple_861={
offsetof( struct Cyc_Absyn_Conref,v),( void*)& Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep};
static struct _tuple4* Cyc__genarr_862[ 1u]={& Cyc__gentuple_861}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep={ 4u, sizeof( struct Cyc_Absyn_Conref),{(
void*)(( struct _tuple4**) Cyc__genarr_862),( void*)(( struct _tuple4**) Cyc__genarr_862),(
void*)(( struct _tuple4**) Cyc__genarr_862 +  1u)}}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_847={ 2u, 1,( void*)(( void*)& Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep)};
struct _tuple73{ unsigned int f1; struct Cyc_Absyn_Conref* f2; } ; static struct
_tuple4 Cyc__gentuple_849={ offsetof( struct _tuple73,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_850={ offsetof( struct _tuple73,f2),( void*)&
Cyc__genrep_847}; static struct _tuple4* Cyc__genarr_851[ 2u]={& Cyc__gentuple_849,&
Cyc__gentuple_850}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_848={ 4u,
sizeof( struct _tuple73),{( void*)(( struct _tuple4**) Cyc__genarr_851),( void*)((
struct _tuple4**) Cyc__genarr_851),( void*)(( struct _tuple4**) Cyc__genarr_851
+  2u)}}; static struct _tuple4 Cyc__gentuple_858={ 0,( void*)& Cyc__genrep_852};
static struct _tuple4 Cyc__gentuple_859={ 1,( void*)& Cyc__genrep_848}; static
struct _tuple4* Cyc__genarr_860[ 2u]={& Cyc__gentuple_858,& Cyc__gentuple_859};
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep={
5u,{( void*)(( struct _tuple4**) Cyc__genarr_860),( void*)(( struct _tuple4**)
Cyc__genarr_860),( void*)(( struct _tuple4**) Cyc__genarr_860 +  2u)}}; static
struct _tuple4 Cyc__gentuple_864={ offsetof( struct Cyc_Absyn_Conref,v),( void*)&
Cyc_tunion_Absyn_Constraint0Absyn_bounds_t2_rep}; static struct _tuple4* Cyc__genarr_865[
1u]={& Cyc__gentuple_864}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Conref0bool2_rep={
4u, sizeof( struct Cyc_Absyn_Conref),{( void*)(( struct _tuple4**) Cyc__genarr_865),(
void*)(( struct _tuple4**) Cyc__genarr_865),( void*)(( struct _tuple4**) Cyc__genarr_865
+  1u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_863={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Conref0bool2_rep)}; static struct _tuple4 Cyc__gentuple_866={
offsetof( struct Cyc_Absyn_PtrInfo,elt_typ),( void*)(( void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple4 Cyc__gentuple_867={ offsetof( struct Cyc_Absyn_PtrInfo,rgn_typ),(
void*)(( void*)& Cyc_Absyn_type_t_rep)}; static struct _tuple4 Cyc__gentuple_868={
offsetof( struct Cyc_Absyn_PtrInfo,nullable),( void*)& Cyc__genrep_863}; static
struct _tuple4 Cyc__gentuple_869={ offsetof( struct Cyc_Absyn_PtrInfo,tq),( void*)&
Cyc__genrep_100}; static struct _tuple4 Cyc__gentuple_870={ offsetof( struct Cyc_Absyn_PtrInfo,bounds),(
void*)& Cyc__genrep_847}; static struct _tuple4* Cyc__genarr_871[ 5u]={& Cyc__gentuple_866,&
Cyc__gentuple_867,& Cyc__gentuple_868,& Cyc__gentuple_869,& Cyc__gentuple_870};
struct Cyc_Typerep_Tuple_struct Cyc_Absyn_ptr_info_t_rep={ 4u, sizeof( struct
Cyc_Absyn_PtrInfo),{( void*)(( struct _tuple4**) Cyc__genarr_871),( void*)((
struct _tuple4**) Cyc__genarr_871),( void*)(( struct _tuple4**) Cyc__genarr_871
+  5u)}}; struct _tuple74{ unsigned int f1; struct Cyc_Absyn_PtrInfo f2; } ;
static struct _tuple4 Cyc__gentuple_872={ offsetof( struct _tuple74,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_873={ offsetof( struct
_tuple74,f2),( void*)& Cyc_Absyn_ptr_info_t_rep}; static struct _tuple4* Cyc__genarr_874[
2u]={& Cyc__gentuple_872,& Cyc__gentuple_873}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_846={ 4u, sizeof( struct _tuple74),{( void*)(( struct _tuple4**) Cyc__genarr_874),(
void*)(( struct _tuple4**) Cyc__genarr_874),( void*)(( struct _tuple4**) Cyc__genarr_874
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_840; extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep; static struct _tuple4* Cyc__genarr_841[
0u]={}; struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep={ 5u,{( void*)((
struct _tuple4**) Cyc__genarr_841),( void*)(( struct _tuple4**) Cyc__genarr_841),(
void*)(( struct _tuple4**) Cyc__genarr_841 +  0u)}}; static struct _tuple4 Cyc__gentuple_842={
offsetof( struct _tuple62,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_843={ offsetof( struct _tuple62,f2),( void*)& Cyc_Absyn_sign_t_rep};
static struct _tuple4 Cyc__gentuple_844={ offsetof( struct _tuple62,f3),( void*)&
Cyc_Absyn_size_of_t_rep}; static struct _tuple4* Cyc__genarr_845[ 3u]={& Cyc__gentuple_842,&
Cyc__gentuple_843,& Cyc__gentuple_844}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_840={ 4u, sizeof( struct _tuple62),{( void*)(( struct _tuple4**) Cyc__genarr_845),(
void*)(( struct _tuple4**) Cyc__genarr_845),( void*)(( struct _tuple4**) Cyc__genarr_845
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_834; struct
_tuple75{ unsigned int f1; void* f2; struct Cyc_Absyn_Tqual f3; struct Cyc_Absyn_Exp*
f4; } ; static struct _tuple4 Cyc__gentuple_835={ offsetof( struct _tuple75,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_836={ offsetof(
struct _tuple75,f2),( void*)(( void*)& Cyc_Absyn_type_t_rep)}; static struct
_tuple4 Cyc__gentuple_837={ offsetof( struct _tuple75,f3),( void*)& Cyc__genrep_100};
static struct _tuple4 Cyc__gentuple_838={ offsetof( struct _tuple75,f4),( void*)&
Cyc__genrep_63}; static struct _tuple4* Cyc__genarr_839[ 4u]={& Cyc__gentuple_835,&
Cyc__gentuple_836,& Cyc__gentuple_837,& Cyc__gentuple_838}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_834={ 4u, sizeof( struct _tuple75),{( void*)(( struct _tuple4**) Cyc__genarr_839),(
void*)(( struct _tuple4**) Cyc__genarr_839),( void*)(( struct _tuple4**) Cyc__genarr_839
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_817; extern struct
Cyc_Typerep_Tuple_struct Cyc_Absyn_fn_info_t_rep; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_818; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep;
static struct _tuple4 Cyc__gentuple_819={ offsetof( struct Cyc_List_List,hd),(
void*)& Cyc__genrep_589}; static struct _tuple4 Cyc__gentuple_820={ offsetof(
struct Cyc_List_List,tl),( void*)& Cyc__genrep_818}; static struct _tuple4* Cyc__genarr_821[
2u]={& Cyc__gentuple_819,& Cyc__gentuple_820}; struct Cyc_Typerep_Tuple_struct
Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep={
4u, sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_821),(
void*)(( struct _tuple4**) Cyc__genarr_821),( void*)(( struct _tuple4**) Cyc__genarr_821
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_818={ 2u, 1,(
void*)(( void*)& Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep)};
static struct _tuple4 Cyc__gentuple_822={ offsetof( struct Cyc_Absyn_FnInfo,tvars),(
void*)& Cyc__genrep_194}; static struct _tuple4 Cyc__gentuple_823={ offsetof(
struct Cyc_Absyn_FnInfo,effect),( void*)& Cyc__genrep_52}; static struct _tuple4
Cyc__gentuple_824={ offsetof( struct Cyc_Absyn_FnInfo,ret_typ),( void*)(( void*)&
Cyc_Absyn_type_t_rep)}; static struct _tuple4 Cyc__gentuple_825={ offsetof(
struct Cyc_Absyn_FnInfo,args),( void*)& Cyc__genrep_818}; static struct _tuple4
Cyc__gentuple_826={ offsetof( struct Cyc_Absyn_FnInfo,c_varargs),( void*)(( void*)&
Cyc__genrep_74)}; static struct _tuple4 Cyc__gentuple_827={ offsetof( struct Cyc_Absyn_FnInfo,cyc_varargs),(
void*)& Cyc__genrep_481}; static struct _tuple4 Cyc__gentuple_828={ offsetof(
struct Cyc_Absyn_FnInfo,rgn_po),( void*)& Cyc__genrep_472}; static struct
_tuple4 Cyc__gentuple_829={ offsetof( struct Cyc_Absyn_FnInfo,attributes),( void*)&
Cyc__genrep_72}; static struct _tuple4* Cyc__genarr_830[ 8u]={& Cyc__gentuple_822,&
Cyc__gentuple_823,& Cyc__gentuple_824,& Cyc__gentuple_825,& Cyc__gentuple_826,&
Cyc__gentuple_827,& Cyc__gentuple_828,& Cyc__gentuple_829}; struct Cyc_Typerep_Tuple_struct
Cyc_Absyn_fn_info_t_rep={ 4u, sizeof( struct Cyc_Absyn_FnInfo),{( void*)((
struct _tuple4**) Cyc__genarr_830),( void*)(( struct _tuple4**) Cyc__genarr_830),(
void*)(( struct _tuple4**) Cyc__genarr_830 +  8u)}}; struct _tuple76{
unsigned int f1; struct Cyc_Absyn_FnInfo f2; } ; static struct _tuple4 Cyc__gentuple_831={
offsetof( struct _tuple76,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_832={ offsetof( struct _tuple76,f2),( void*)& Cyc_Absyn_fn_info_t_rep};
static struct _tuple4* Cyc__genarr_833[ 2u]={& Cyc__gentuple_831,& Cyc__gentuple_832};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_817={ 4u, sizeof( struct
_tuple76),{( void*)(( struct _tuple4**) Cyc__genarr_833),( void*)(( struct
_tuple4**) Cyc__genarr_833),( void*)(( struct _tuple4**) Cyc__genarr_833 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_813; static struct _tuple4
Cyc__gentuple_814={ offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_815={ offsetof( struct _tuple8,f2),( void*)&
Cyc__genrep_237}; static struct _tuple4* Cyc__genarr_816[ 2u]={& Cyc__gentuple_814,&
Cyc__gentuple_815}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_813={ 4u,
sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_816),( void*)((
struct _tuple4**) Cyc__genarr_816),( void*)(( struct _tuple4**) Cyc__genarr_816
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_806; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_800; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_800={ 2u, 1,( void*)(( void*)& Cyc__genrep_10)}; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_807; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_807={ 2u,
1,( void*)(( void*)& Cyc__genrep_276)}; struct _tuple77{ unsigned int f1; struct
_tuple0* f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl** f4; } ;
static struct _tuple4 Cyc__gentuple_808={ offsetof( struct _tuple77,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_809={ offsetof( struct
_tuple77,f2),( void*)& Cyc__genrep_800}; static struct _tuple4 Cyc__gentuple_810={
offsetof( struct _tuple77,f3),( void*)& Cyc__genrep_44}; static struct _tuple4
Cyc__gentuple_811={ offsetof( struct _tuple77,f4),( void*)& Cyc__genrep_807};
static struct _tuple4* Cyc__genarr_812[ 4u]={& Cyc__gentuple_808,& Cyc__gentuple_809,&
Cyc__gentuple_810,& Cyc__gentuple_811}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_806={ 4u, sizeof( struct _tuple77),{( void*)(( struct _tuple4**) Cyc__genarr_812),(
void*)(( struct _tuple4**) Cyc__genarr_812),( void*)(( struct _tuple4**) Cyc__genarr_812
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_791; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_792; extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_793; extern struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Uniondecl_rep;
static struct _tuple4 Cyc__gentuple_794={ offsetof( struct Cyc_Absyn_Uniondecl,sc),(
void*)& Cyc_Absyn_scope_t_rep}; static struct _tuple4 Cyc__gentuple_795={
offsetof( struct Cyc_Absyn_Uniondecl,name),( void*)& Cyc__genrep_291}; static
struct _tuple4 Cyc__gentuple_796={ offsetof( struct Cyc_Absyn_Uniondecl,tvs),(
void*)& Cyc__genrep_194}; static struct _tuple4 Cyc__gentuple_797={ offsetof(
struct Cyc_Absyn_Uniondecl,fields),( void*)& Cyc__genrep_277}; static struct
_tuple4 Cyc__gentuple_798={ offsetof( struct Cyc_Absyn_Uniondecl,attributes),(
void*)& Cyc__genrep_72}; static struct _tuple4* Cyc__genarr_799[ 5u]={& Cyc__gentuple_794,&
Cyc__gentuple_795,& Cyc__gentuple_796,& Cyc__gentuple_797,& Cyc__gentuple_798};
struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Uniondecl_rep={ 4u, sizeof(
struct Cyc_Absyn_Uniondecl),{( void*)(( struct _tuple4**) Cyc__genarr_799),(
void*)(( struct _tuple4**) Cyc__genarr_799),( void*)(( struct _tuple4**) Cyc__genarr_799
+  5u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_793={ 2u, 1,(
void*)(( void*)& Cyc_struct_Absyn_Uniondecl_rep)}; static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_792={ 2u, 1,( void*)(( void*)& Cyc__genrep_793)}; struct _tuple78{
unsigned int f1; struct _tuple0* f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Uniondecl**
f4; } ; static struct _tuple4 Cyc__gentuple_801={ offsetof( struct _tuple78,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_802={ offsetof(
struct _tuple78,f2),( void*)& Cyc__genrep_800}; static struct _tuple4 Cyc__gentuple_803={
offsetof( struct _tuple78,f3),( void*)& Cyc__genrep_44}; static struct _tuple4
Cyc__gentuple_804={ offsetof( struct _tuple78,f4),( void*)& Cyc__genrep_792};
static struct _tuple4* Cyc__genarr_805[ 4u]={& Cyc__gentuple_801,& Cyc__gentuple_802,&
Cyc__gentuple_803,& Cyc__gentuple_804}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_791={ 4u, sizeof( struct _tuple78),{( void*)(( struct _tuple4**) Cyc__genarr_805),(
void*)(( struct _tuple4**) Cyc__genarr_805),( void*)(( struct _tuple4**) Cyc__genarr_805
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_786; struct
_tuple79{ unsigned int f1; struct _tuple0* f2; struct Cyc_Absyn_Enumdecl* f3; }
; static struct _tuple4 Cyc__gentuple_787={ offsetof( struct _tuple79,f1),( void*)&
Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_788={ offsetof( struct
_tuple79,f2),( void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_789={
offsetof( struct _tuple79,f3),( void*)& Cyc__genrep_551}; static struct _tuple4*
Cyc__genarr_790[ 3u]={& Cyc__gentuple_787,& Cyc__gentuple_788,& Cyc__gentuple_789};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_786={ 4u, sizeof( struct
_tuple79),{( void*)(( struct _tuple4**) Cyc__genarr_790),( void*)(( struct
_tuple4**) Cyc__genarr_790),( void*)(( struct _tuple4**) Cyc__genarr_790 +  3u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_782; static struct _tuple4
Cyc__gentuple_783={ offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_784={ offsetof( struct _tuple8,f2),( void*)&
Cyc__genrep_278}; static struct _tuple4* Cyc__genarr_785[ 2u]={& Cyc__gentuple_783,&
Cyc__gentuple_784}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_782={ 4u,
sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_785),( void*)((
struct _tuple4**) Cyc__genarr_785),( void*)(( struct _tuple4**) Cyc__genarr_785
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_60; static struct
_tuple4 Cyc__gentuple_779={ offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_780={ offsetof( struct _tuple8,f2),( void*)&
Cyc__genrep_61}; static struct _tuple4* Cyc__genarr_781[ 2u]={& Cyc__gentuple_779,&
Cyc__gentuple_780}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_60={ 4u,
sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_781),( void*)((
struct _tuple4**) Cyc__genarr_781),( void*)(( struct _tuple4**) Cyc__genarr_781
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_51; struct _tuple80{
unsigned int f1; struct _tuple0* f2; struct Cyc_List_List* f3; struct Cyc_Core_Opt*
f4; } ; static struct _tuple4 Cyc__gentuple_55={ offsetof( struct _tuple80,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_56={ offsetof(
struct _tuple80,f2),( void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_57={
offsetof( struct _tuple80,f3),( void*)& Cyc__genrep_44}; static struct _tuple4
Cyc__gentuple_58={ offsetof( struct _tuple80,f4),( void*)& Cyc__genrep_52};
static struct _tuple4* Cyc__genarr_59[ 4u]={& Cyc__gentuple_55,& Cyc__gentuple_56,&
Cyc__gentuple_57,& Cyc__gentuple_58}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_51={
4u, sizeof( struct _tuple80),{( void*)(( struct _tuple4**) Cyc__genarr_59),(
void*)(( struct _tuple4**) Cyc__genarr_59),( void*)(( struct _tuple4**) Cyc__genarr_59
+  4u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_43; static struct
_tuple4 Cyc__gentuple_48={ offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_49={ offsetof( struct _tuple8,f2),( void*)&
Cyc__genrep_44}; static struct _tuple4* Cyc__genarr_50[ 2u]={& Cyc__gentuple_48,&
Cyc__gentuple_49}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_43={ 4u,
sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_50),( void*)((
struct _tuple4**) Cyc__genarr_50),( void*)(( struct _tuple4**) Cyc__genarr_50 + 
2u)}}; static struct _tuple4 Cyc__gentuple_938={ 0,( void*)& Cyc__genrep_925};
static struct _tuple4 Cyc__gentuple_939={ 1,( void*)& Cyc__genrep_921}; static
struct _tuple4 Cyc__gentuple_940={ 2,( void*)& Cyc__genrep_898}; static struct
_tuple4 Cyc__gentuple_941={ 3,( void*)& Cyc__genrep_875}; static struct _tuple4
Cyc__gentuple_942={ 4,( void*)& Cyc__genrep_846}; static struct _tuple4 Cyc__gentuple_943={
5,( void*)& Cyc__genrep_840}; static struct _tuple4 Cyc__gentuple_944={ 6,( void*)&
Cyc__genrep_85}; static struct _tuple4 Cyc__gentuple_945={ 7,( void*)& Cyc__genrep_834};
static struct _tuple4 Cyc__gentuple_946={ 8,( void*)& Cyc__genrep_817}; static
struct _tuple4 Cyc__gentuple_947={ 9,( void*)& Cyc__genrep_813}; static struct
_tuple4 Cyc__gentuple_948={ 10,( void*)& Cyc__genrep_806}; static struct _tuple4
Cyc__gentuple_949={ 11,( void*)& Cyc__genrep_791}; static struct _tuple4 Cyc__gentuple_950={
12,( void*)& Cyc__genrep_786}; static struct _tuple4 Cyc__gentuple_951={ 13,(
void*)& Cyc__genrep_39}; static struct _tuple4 Cyc__gentuple_952={ 14,( void*)&
Cyc__genrep_782}; static struct _tuple4 Cyc__gentuple_953={ 15,( void*)& Cyc__genrep_782};
static struct _tuple4 Cyc__gentuple_954={ 16,( void*)& Cyc__genrep_60}; static
struct _tuple4 Cyc__gentuple_955={ 17,( void*)& Cyc__genrep_39}; static struct
_tuple4 Cyc__gentuple_956={ 18,( void*)& Cyc__genrep_51}; static struct _tuple4
Cyc__gentuple_957={ 19,( void*)& Cyc__genrep_39}; static struct _tuple4 Cyc__gentuple_958={
20,( void*)& Cyc__genrep_43}; static struct _tuple4 Cyc__gentuple_959={ 21,(
void*)& Cyc__genrep_39}; static struct _tuple4* Cyc__genarr_960[ 22u]={& Cyc__gentuple_938,&
Cyc__gentuple_939,& Cyc__gentuple_940,& Cyc__gentuple_941,& Cyc__gentuple_942,&
Cyc__gentuple_943,& Cyc__gentuple_944,& Cyc__gentuple_945,& Cyc__gentuple_946,&
Cyc__gentuple_947,& Cyc__gentuple_948,& Cyc__gentuple_949,& Cyc__gentuple_950,&
Cyc__gentuple_951,& Cyc__gentuple_952,& Cyc__gentuple_953,& Cyc__gentuple_954,&
Cyc__gentuple_955,& Cyc__gentuple_956,& Cyc__gentuple_957,& Cyc__gentuple_958,&
Cyc__gentuple_959}; struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep={ 5u,{(
void*)(( struct _tuple4**) Cyc__genarr_960),( void*)(( struct _tuple4**) Cyc__genarr_960),(
void*)(( struct _tuple4**) Cyc__genarr_960 +  22u)}}; static struct _tuple4 Cyc__gentuple_104={
offsetof( struct Cyc_Absyn_Vardecl,sc),( void*)& Cyc_Absyn_scope_t_rep}; static
struct _tuple4 Cyc__gentuple_105={ offsetof( struct Cyc_Absyn_Vardecl,name),(
void*)& Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_106={ offsetof(
struct Cyc_Absyn_Vardecl,tq),( void*)& Cyc__genrep_100}; static struct _tuple4
Cyc__gentuple_107={ offsetof( struct Cyc_Absyn_Vardecl,type),( void*)(( void*)&
Cyc_Absyn_type_t_rep)}; static struct _tuple4 Cyc__gentuple_108={ offsetof(
struct Cyc_Absyn_Vardecl,initializer),( void*)& Cyc__genrep_63}; static struct
_tuple4 Cyc__gentuple_109={ offsetof( struct Cyc_Absyn_Vardecl,rgn),( void*)&
Cyc__genrep_52}; static struct _tuple4 Cyc__gentuple_110={ offsetof( struct Cyc_Absyn_Vardecl,attributes),(
void*)& Cyc__genrep_72}; static struct _tuple4 Cyc__gentuple_111={ offsetof(
struct Cyc_Absyn_Vardecl,escapes),( void*)(( void*)& Cyc__genrep_74)}; static
struct _tuple4* Cyc__genarr_112[ 8u]={& Cyc__gentuple_104,& Cyc__gentuple_105,&
Cyc__gentuple_106,& Cyc__gentuple_107,& Cyc__gentuple_108,& Cyc__gentuple_109,&
Cyc__gentuple_110,& Cyc__gentuple_111}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Vardecl_rep={
4u, sizeof( struct Cyc_Absyn_Vardecl),{( void*)(( struct _tuple4**) Cyc__genarr_112),(
void*)(( struct _tuple4**) Cyc__genarr_112),( void*)(( struct _tuple4**) Cyc__genarr_112
+  8u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_99={ 2u, 1,( void*)((
void*)& Cyc_struct_Absyn_Vardecl_rep)}; struct _tuple81{ unsigned int f1; struct
Cyc_Absyn_Vardecl* f2; } ; static struct _tuple4 Cyc__gentuple_307={ offsetof(
struct _tuple81,f1),( void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_308={
offsetof( struct _tuple81,f2),( void*)& Cyc__genrep_99}; static struct _tuple4*
Cyc__genarr_309[ 2u]={& Cyc__gentuple_307,& Cyc__gentuple_308}; static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_306={ 4u, sizeof( struct _tuple81),{( void*)((
struct _tuple4**) Cyc__genarr_309),( void*)(( struct _tuple4**) Cyc__genarr_309),(
void*)(( struct _tuple4**) Cyc__genarr_309 +  2u)}}; extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_988; struct _tuple82{ unsigned int f1; struct Cyc_Absyn_Pat* f2;
struct Cyc_Core_Opt* f3; struct Cyc_Core_Opt* f4; struct Cyc_Absyn_Exp* f5; int
f6; } ; static struct _tuple4 Cyc__gentuple_989={ offsetof( struct _tuple82,f1),(
void*)& Cyc__genrep_4}; static struct _tuple4 Cyc__gentuple_990={ offsetof(
struct _tuple82,f2),( void*)& Cyc__genrep_172}; static struct _tuple4 Cyc__gentuple_991={
offsetof( struct _tuple82,f3),( void*)& Cyc__genrep_97}; static struct _tuple4
Cyc__gentuple_992={ offsetof( struct _tuple82,f4),( void*)& Cyc__genrep_52};
static struct _tuple4 Cyc__gentuple_993={ offsetof( struct _tuple82,f5),( void*)&
Cyc__genrep_66}; static struct _tuple4 Cyc__gentuple_994={ offsetof( struct
_tuple82,f6),( void*)(( void*)& Cyc__genrep_74)}; static struct _tuple4* Cyc__genarr_995[
6u]={& Cyc__gentuple_989,& Cyc__gentuple_990,& Cyc__gentuple_991,& Cyc__gentuple_992,&
Cyc__gentuple_993,& Cyc__gentuple_994}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_988={ 4u, sizeof( struct _tuple82),{( void*)(( struct _tuple4**) Cyc__genarr_995),(
void*)(( struct _tuple4**) Cyc__genarr_995),( void*)(( struct _tuple4**) Cyc__genarr_995
+  6u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_984; static struct
_tuple4 Cyc__gentuple_985={ offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_986={ offsetof( struct _tuple8,f2),( void*)&
Cyc__genrep_98}; static struct _tuple4* Cyc__genarr_987[ 2u]={& Cyc__gentuple_985,&
Cyc__gentuple_986}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_984={ 4u,
sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_987),( void*)((
struct _tuple4**) Cyc__genarr_987),( void*)(( struct _tuple4**) Cyc__genarr_987
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_980; struct
_tuple83{ unsigned int f1; struct Cyc_Absyn_Structdecl* f2; } ; static struct
_tuple4 Cyc__gentuple_981={ offsetof( struct _tuple83,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_982={ offsetof( struct _tuple83,f2),( void*)((
void*)& Cyc__genrep_276)}; static struct _tuple4* Cyc__genarr_983[ 2u]={& Cyc__gentuple_981,&
Cyc__gentuple_982}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_980={ 4u,
sizeof( struct _tuple83),{( void*)(( struct _tuple4**) Cyc__genarr_983),( void*)((
struct _tuple4**) Cyc__genarr_983),( void*)(( struct _tuple4**) Cyc__genarr_983
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_976; struct
_tuple84{ unsigned int f1; struct Cyc_Absyn_Uniondecl* f2; } ; static struct
_tuple4 Cyc__gentuple_977={ offsetof( struct _tuple84,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_978={ offsetof( struct _tuple84,f2),( void*)((
void*)& Cyc__genrep_793)}; static struct _tuple4* Cyc__genarr_979[ 2u]={& Cyc__gentuple_977,&
Cyc__gentuple_978}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_976={ 4u,
sizeof( struct _tuple84),{( void*)(( struct _tuple4**) Cyc__genarr_979),( void*)((
struct _tuple4**) Cyc__genarr_979),( void*)(( struct _tuple4**) Cyc__genarr_979
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_972; struct
_tuple85{ unsigned int f1; struct Cyc_Absyn_Tuniondecl* f2; } ; static struct
_tuple4 Cyc__gentuple_973={ offsetof( struct _tuple85,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_974={ offsetof( struct _tuple85,f2),( void*)((
void*)& Cyc__genrep_252)}; static struct _tuple4* Cyc__genarr_975[ 2u]={& Cyc__gentuple_973,&
Cyc__gentuple_974}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_972={ 4u,
sizeof( struct _tuple85),{( void*)(( struct _tuple4**) Cyc__genarr_975),( void*)((
struct _tuple4**) Cyc__genarr_975),( void*)(( struct _tuple4**) Cyc__genarr_975
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_968; struct
_tuple86{ unsigned int f1; struct Cyc_Absyn_Enumdecl* f2; } ; static struct
_tuple4 Cyc__gentuple_969={ offsetof( struct _tuple86,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_970={ offsetof( struct _tuple86,f2),( void*)&
Cyc__genrep_223}; static struct _tuple4* Cyc__genarr_971[ 2u]={& Cyc__gentuple_969,&
Cyc__gentuple_970}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_968={ 4u,
sizeof( struct _tuple86),{( void*)(( struct _tuple4**) Cyc__genarr_971),( void*)((
struct _tuple4**) Cyc__genarr_971),( void*)(( struct _tuple4**) Cyc__genarr_971
+  2u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_37; extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_38; extern struct Cyc_Typerep_Tuple_struct
Cyc_struct_Absyn_Typedefdecl_rep; static struct _tuple4 Cyc__gentuple_961={
offsetof( struct Cyc_Absyn_Typedefdecl,name),( void*)& Cyc__genrep_9}; static
struct _tuple4 Cyc__gentuple_962={ offsetof( struct Cyc_Absyn_Typedefdecl,tvs),(
void*)& Cyc__genrep_194}; static struct _tuple4 Cyc__gentuple_963={ offsetof(
struct Cyc_Absyn_Typedefdecl,defn),( void*)& Cyc_Absyn_type_t_rep}; static
struct _tuple4* Cyc__genarr_964[ 3u]={& Cyc__gentuple_961,& Cyc__gentuple_962,&
Cyc__gentuple_963}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Typedefdecl_rep={
4u, sizeof( struct Cyc_Absyn_Typedefdecl),{( void*)(( struct _tuple4**) Cyc__genarr_964),(
void*)(( struct _tuple4**) Cyc__genarr_964),( void*)(( struct _tuple4**) Cyc__genarr_964
+  3u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_38={ 2u, 1,( void*)((
void*)& Cyc_struct_Absyn_Typedefdecl_rep)}; struct _tuple87{ unsigned int f1;
struct Cyc_Absyn_Typedefdecl* f2; } ; static struct _tuple4 Cyc__gentuple_965={
offsetof( struct _tuple87,f1),( void*)& Cyc__genrep_4}; static struct _tuple4
Cyc__gentuple_966={ offsetof( struct _tuple87,f2),( void*)& Cyc__genrep_38};
static struct _tuple4* Cyc__genarr_967[ 2u]={& Cyc__gentuple_965,& Cyc__gentuple_966};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_37={ 4u, sizeof( struct
_tuple87),{( void*)(( struct _tuple4**) Cyc__genarr_967),( void*)(( struct
_tuple4**) Cyc__genarr_967),( void*)(( struct _tuple4**) Cyc__genarr_967 +  2u)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_32; struct _tuple88{
unsigned int f1; struct _tagged_arr* f2; struct Cyc_List_List* f3; } ; static
struct _tuple4 Cyc__gentuple_33={ offsetof( struct _tuple88,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_34={ offsetof( struct _tuple88,f2),( void*)&
Cyc__genrep_11}; static struct _tuple4 Cyc__gentuple_35={ offsetof( struct
_tuple88,f3),( void*)& Cyc__genrep_0}; static struct _tuple4* Cyc__genarr_36[ 3u]={&
Cyc__gentuple_33,& Cyc__gentuple_34,& Cyc__gentuple_35}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_32={ 4u, sizeof( struct _tuple88),{( void*)(( struct _tuple4**) Cyc__genarr_36),(
void*)(( struct _tuple4**) Cyc__genarr_36),( void*)(( struct _tuple4**) Cyc__genarr_36
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_8; struct _tuple89{
unsigned int f1; struct _tuple0* f2; struct Cyc_List_List* f3; } ; static struct
_tuple4 Cyc__gentuple_28={ offsetof( struct _tuple89,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_29={ offsetof( struct _tuple89,f2),( void*)&
Cyc__genrep_9}; static struct _tuple4 Cyc__gentuple_30={ offsetof( struct
_tuple89,f3),( void*)& Cyc__genrep_0}; static struct _tuple4* Cyc__genarr_31[ 3u]={&
Cyc__gentuple_28,& Cyc__gentuple_29,& Cyc__gentuple_30}; static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_8={ 4u, sizeof( struct _tuple89),{( void*)(( struct _tuple4**) Cyc__genarr_31),(
void*)(( struct _tuple4**) Cyc__genarr_31),( void*)(( struct _tuple4**) Cyc__genarr_31
+  3u)}}; extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_3; static struct
_tuple4 Cyc__gentuple_5={ offsetof( struct _tuple8,f1),( void*)& Cyc__genrep_4};
static struct _tuple4 Cyc__gentuple_6={ offsetof( struct _tuple8,f2),( void*)&
Cyc__genrep_0}; static struct _tuple4* Cyc__genarr_7[ 2u]={& Cyc__gentuple_5,&
Cyc__gentuple_6}; static struct Cyc_Typerep_Tuple_struct Cyc__genrep_3={ 4u,
sizeof( struct _tuple8),{( void*)(( struct _tuple4**) Cyc__genarr_7),( void*)((
struct _tuple4**) Cyc__genarr_7),( void*)(( struct _tuple4**) Cyc__genarr_7 +  2u)}};
static struct _tuple4 Cyc__gentuple_996={ 0,( void*)& Cyc__genrep_306}; static
struct _tuple4 Cyc__gentuple_997={ 1,( void*)& Cyc__genrep_70}; static struct
_tuple4 Cyc__gentuple_998={ 2,( void*)& Cyc__genrep_988}; static struct _tuple4
Cyc__gentuple_999={ 3,( void*)& Cyc__genrep_984}; static struct _tuple4 Cyc__gentuple_1000={
4,( void*)& Cyc__genrep_980}; static struct _tuple4 Cyc__gentuple_1001={ 5,(
void*)& Cyc__genrep_976}; static struct _tuple4 Cyc__gentuple_1002={ 6,( void*)&
Cyc__genrep_972}; static struct _tuple4 Cyc__gentuple_1003={ 7,( void*)& Cyc__genrep_968};
static struct _tuple4 Cyc__gentuple_1004={ 8,( void*)& Cyc__genrep_37}; static
struct _tuple4 Cyc__gentuple_1005={ 9,( void*)& Cyc__genrep_32}; static struct
_tuple4 Cyc__gentuple_1006={ 10,( void*)& Cyc__genrep_8}; static struct _tuple4
Cyc__gentuple_1007={ 11,( void*)& Cyc__genrep_3}; static struct _tuple4* Cyc__genarr_1008[
12u]={& Cyc__gentuple_996,& Cyc__gentuple_997,& Cyc__gentuple_998,& Cyc__gentuple_999,&
Cyc__gentuple_1000,& Cyc__gentuple_1001,& Cyc__gentuple_1002,& Cyc__gentuple_1003,&
Cyc__gentuple_1004,& Cyc__gentuple_1005,& Cyc__gentuple_1006,& Cyc__gentuple_1007};
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep={ 5u,{( void*)((
struct _tuple4**) Cyc__genarr_1008),( void*)(( struct _tuple4**) Cyc__genarr_1008),(
void*)(( struct _tuple4**) Cyc__genarr_1008 +  12u)}}; static struct _tuple4 Cyc__gentuple_1009={
offsetof( struct Cyc_Absyn_Decl,r),( void*)& Cyc_Absyn_raw_decl_t_rep}; static
struct _tuple4 Cyc__gentuple_1010={ offsetof( struct Cyc_Absyn_Decl,loc),( void*)&
Cyc__genrep_2}; static struct _tuple4* Cyc__genarr_1011[ 2u]={& Cyc__gentuple_1009,&
Cyc__gentuple_1010}; struct Cyc_Typerep_Tuple_struct Cyc_struct_Absyn_Decl_rep={
4u, sizeof( struct Cyc_Absyn_Decl),{( void*)(( struct _tuple4**) Cyc__genarr_1011),(
void*)(( struct _tuple4**) Cyc__genarr_1011),( void*)(( struct _tuple4**) Cyc__genarr_1011
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1={ 2u, 1,( void*)((
void*)& Cyc_struct_Absyn_Decl_rep)}; static struct _tuple4 Cyc__gentuple_1012={
offsetof( struct Cyc_List_List,hd),( void*)& Cyc__genrep_1}; static struct
_tuple4 Cyc__gentuple_1013={ offsetof( struct Cyc_List_List,tl),( void*)& Cyc__genrep_0};
static struct _tuple4* Cyc__genarr_1014[ 2u]={& Cyc__gentuple_1012,& Cyc__gentuple_1013};
struct Cyc_Typerep_Tuple_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep={ 4u,
sizeof( struct Cyc_List_List),{( void*)(( struct _tuple4**) Cyc__genarr_1014),(
void*)(( struct _tuple4**) Cyc__genarr_1014),( void*)(( struct _tuple4**) Cyc__genarr_1014
+  2u)}}; static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_0={ 2u, 1,( void*)((
void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)}; void* Cyc_decls_rep=( void*)&
Cyc__genrep_0;
