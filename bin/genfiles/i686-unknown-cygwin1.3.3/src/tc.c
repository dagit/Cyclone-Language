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
tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x); extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List* Cyc_List_append(
struct Cyc_List_List* x, struct Cyc_List_List* y); extern struct Cyc_List_List*
Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_List_mem( int(* compare)( void*,
void*), struct Cyc_List_List* l, void* x); extern struct Cyc_List_List* Cyc_List_filter_c(
int(* f)( void*, void*), void* env, struct Cyc_List_List* x); extern int Cyc_Std_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern int Cyc_Std_strptrcmp(
struct _tagged_arr* s1, struct _tagged_arr* s2); extern int Cyc_Std_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); struct Cyc_Lineno_Pos{ struct
_tagged_arr logical_file; struct _tagged_arr line; int line_no; int col; } ;
extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment; static
const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1; static
const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct _tagged_arr
source; struct Cyc_Position_Segment* seg; void* kind; struct _tagged_arr desc; }
; extern unsigned char Cyc_Position_Nocontext[ 14u]; static const int Cyc_Absyn_Loc_n=
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
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern void* Cyc_Absyn_compress_kb( void*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*); extern struct
_tagged_arr Cyc_Absyn_attribute2string( void*); extern int Cyc_Absyn_fntype_att(
void* a); struct Cyc_Cstdio___sFILE; struct Cyc_Std___sFILE; extern
unsigned char Cyc_Std_FileCloseError[ 19u]; extern unsigned char Cyc_Std_FileOpenError[
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
int tag; float* f1; } ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{ int expand_typedefs: 1; int qvar_to_Cids: 1; int
add_cyc_prefix: 1; int to_VC: 1; int decls_first: 1; int rewrite_temp_tvars: 1;
int print_all_tvars: 1; int print_all_kinds: 1; int print_using_stmts: 1; int
print_externC_stmts: 1; int print_full_evars: 1; int generate_line_directives: 1;
int use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern void
Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params* fs); extern struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r; extern struct _tagged_arr Cyc_Absynpp_typ2string( void*);
extern struct _tagged_arr Cyc_Absynpp_qvar2string( struct _tuple0*); struct Cyc_Set_Set;
extern struct Cyc_Set_Set* Cyc_Set_empty( int(* cmp)( void*, void*)); extern
struct Cyc_Set_Set* Cyc_Set_insert( struct Cyc_Set_Set* s, void* elt); extern
int Cyc_Set_member( struct Cyc_Set_Set* s, void* elt); extern unsigned char Cyc_Set_Absent[
11u]; struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
unsigned char Cyc_Dict_Absent[ 11u]; extern int Cyc_Dict_member( struct Cyc_Dict_Dict*
d, void* k); extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict*
d, void* k, void* v); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d,
void* k); extern struct Cyc_Core_Opt* Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict*
d, void* k); extern void Cyc_Dict_iter( void(* f)( void*, void*), struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict*
d); extern struct Cyc_Dict_Dict* Cyc_Dict_filter_c( int(* f)( void*, void*, void*),
void* env, struct Cyc_Dict_Dict* d); static const int Cyc_Tcenv_VarRes= 0;
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
extern struct Cyc_Tcenv_Genv* Cyc_Tcenv_empty_genv(); extern struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_new_fenv( struct Cyc_Position_Segment*, struct Cyc_Absyn_Fndecl*);
extern struct Cyc_List_List* Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tagged_arr*, struct Cyc_List_List*);
extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple0*); extern int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*); extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern
void* Cyc_Tcutil_impos( struct _tagged_arr fmt, struct _tagged_arr ap); extern
void Cyc_Tcutil_terr( struct Cyc_Position_Segment*, struct _tagged_arr fmt,
struct _tagged_arr ap); extern void Cyc_Tcutil_warn( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_is_function_type( void* t); extern
void Cyc_Tcutil_explain_failure(); extern void* Cyc_Tcutil_fndecl2typ( struct
Cyc_Absyn_Fndecl*); extern void Cyc_Tcutil_check_bitfield( struct Cyc_Position_Segment*
loc, struct Cyc_Tcenv_Tenv* te, void* field_typ, struct Cyc_Absyn_Exp* width,
struct _tagged_arr* fn); extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, int allow_evars, void*); extern void
Cyc_Tcutil_check_unique_tvars( struct Cyc_Position_Segment*, struct Cyc_List_List*);
extern void Cyc_Tcutil_add_tvar_identities( struct Cyc_List_List*); extern int
Cyc_Tcutil_bits_only( void* t); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_supports_default( void*);
extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*, void**, struct
Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int
var_default_init, struct Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*); extern void Cyc_Tc_tcStructdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Tcenv_Genv*, struct Cyc_Position_Segment*,
struct Cyc_Absyn_Structdecl*); extern void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Tcenv_Genv*, struct Cyc_Position_Segment*, struct Cyc_Absyn_Uniondecl*);
extern void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*, struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*, struct Cyc_Absyn_Tuniondecl*); extern void Cyc_Tc_tcEnumdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Tcenv_Genv*, struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*); extern unsigned char Cyc_Tcdecl_Incompatible[ 17u];
struct Cyc_Tcdecl_Xtunionfielddecl{ struct Cyc_Absyn_Tuniondecl* base; struct
Cyc_Absyn_Tunionfield* field; } ; extern struct Cyc_Absyn_Structdecl* Cyc_Tcdecl_merge_structdecl(
struct Cyc_Absyn_Structdecl* d0, struct Cyc_Absyn_Structdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Uniondecl* Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl* d0, struct Cyc_Absyn_Uniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Tuniondecl* Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl* d0, struct Cyc_Absyn_Tuniondecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern struct Cyc_Absyn_Enumdecl* Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl* d0, struct Cyc_Absyn_Enumdecl* d1, struct Cyc_Position_Segment*
loc, struct _tagged_arr* msg); extern void* Cyc_Tcdecl_merge_binding( void* d0,
void* d1, struct Cyc_Position_Segment* loc, struct _tagged_arr* msg); extern
struct Cyc_List_List* Cyc_Tcdecl_sort_xtunion_fields( struct Cyc_List_List* f,
int* res, struct _tagged_arr* v, struct Cyc_Position_Segment* loc, struct
_tagged_arr* msg); struct Cyc_Tcgenrep_RepInfo; extern struct Cyc_Dict_Dict* Cyc_Tcgenrep_empty_typerep_dict();
struct _tuple3{ struct Cyc_Dict_Dict* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Exp*
f3; } ; extern struct _tuple3 Cyc_Tcgenrep_tcGenrep( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, void* type, struct
Cyc_Dict_Dict* dict); static unsigned char _temp0[ 1u]=""; static struct
_tagged_arr Cyc_Tc_tc_msg_c={ _temp0, _temp0, _temp0 +  1u}; static struct
_tagged_arr* Cyc_Tc_tc_msg=( struct _tagged_arr*)& Cyc_Tc_tc_msg_c; static
struct Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List*
atts){ void* _temp1= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp7;
struct Cyc_List_List* _temp9; struct Cyc_List_List** _temp11; _LL3: if((
unsigned int) _temp1 >  3u?*(( int*) _temp1) ==  Cyc_Absyn_FnType: 0){ _LL8:
_temp7=(( struct Cyc_Absyn_FnType_struct*) _temp1)->f1; _LL10: _temp9= _temp7.attributes;
_temp11=( struct Cyc_List_List**)&((( struct Cyc_Absyn_FnType_struct*) _temp1)->f1).attributes;
goto _LL4;} else{ goto _LL5;} _LL5: goto _LL6; _LL4: { struct Cyc_List_List*
_temp12= 0; for( 0; atts !=  0; atts= atts->tl){ if( Cyc_Absyn_fntype_att(( void*)
atts->hd)){* _temp11=({ struct Cyc_List_List* _temp13=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp13->hd=( void*)(( void*) atts->hd);
_temp13->tl=* _temp11; _temp13;});} else{ _temp12=({ struct Cyc_List_List*
_temp14=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp14->hd=( void*)(( void*) atts->hd); _temp14->tl= _temp12; _temp14;});}}
return _temp12;} _LL6: return({ void* _temp15[ 0u]={};(( struct Cyc_List_List*(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("transfer_fn_type_atts",
sizeof( unsigned char), 22u), _tag_arr( _temp15, sizeof( void*), 0u));}); _LL2:;}
struct _tuple4{ void* f1; int f2; } ; static void Cyc_Tc_tcVardecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Vardecl*
vd, int check_var_init){ struct _tagged_arr* v=(* vd->name).f2; void* t=( void*)
vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List* atts= vd->attributes;
vd->escapes= 1;{ void* _temp16=(* vd->name).f1; struct Cyc_List_List* _temp24;
struct Cyc_List_List* _temp26; _LL18: if(( unsigned int) _temp16 >  1u?*(( int*)
_temp16) ==  Cyc_Absyn_Rel_n: 0){ _LL25: _temp24=(( struct Cyc_Absyn_Rel_n_struct*)
_temp16)->f1; if( _temp24 ==  0){ goto _LL19;} else{ goto _LL20;}} else{ goto
_LL20;} _LL20: if(( unsigned int) _temp16 >  1u?*(( int*) _temp16) ==  Cyc_Absyn_Abs_n:
0){ _LL27: _temp26=(( struct Cyc_Absyn_Abs_n_struct*) _temp16)->f1; if( _temp26
==  0){ goto _LL21;} else{ goto _LL22;}} else{ goto _LL22;} _LL22: goto _LL23;
_LL19: goto _LL17; _LL21: goto _LL17; _LL23:({ struct Cyc_Std_String_pa_struct
_temp29; _temp29.tag= Cyc_Std_String_pa; _temp29.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp28[ 1u]={& _temp29}; Cyc_Tcutil_terr( loc, _tag_arr("qualified variable declarations are not implemented (%s)",
sizeof( unsigned char), 57u), _tag_arr( _temp28, sizeof( void*), 1u));}});
return; _LL17:;}(* vd->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp30=( struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp30[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp31; _temp31.tag= Cyc_Absyn_Abs_n;
_temp31.f1= te->ns; _temp31;}); _temp30;});{ void* _temp32= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Exp* _temp39; struct Cyc_Absyn_Tqual _temp41; void* _temp43;
_LL34: if(( unsigned int) _temp32 >  3u?*(( int*) _temp32) ==  Cyc_Absyn_ArrayType:
0){ _LL44: _temp43=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp32)->f1;
goto _LL42; _LL42: _temp41=(( struct Cyc_Absyn_ArrayType_struct*) _temp32)->f2;
goto _LL40; _LL40: _temp39=(( struct Cyc_Absyn_ArrayType_struct*) _temp32)->f3;
if( _temp39 ==  0){ goto _LL38;} else{ goto _LL36;}} else{ goto _LL36;} _LL38:
if( vd->initializer !=  0){ goto _LL35;} else{ goto _LL36;} _LL36: goto _LL37;
_LL35:{ void* _temp45=( void*)(( struct Cyc_Absyn_Exp*) _check_null( vd->initializer))->r;
void* _temp57; struct _tagged_arr _temp59; struct Cyc_Absyn_Exp* _temp61; struct
Cyc_List_List* _temp63; struct Cyc_List_List* _temp65; _LL47: if(*(( int*)
_temp45) ==  Cyc_Absyn_Const_e){ _LL58: _temp57=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp45)->f1; if(( unsigned int) _temp57 >  1u?*(( int*) _temp57) ==  Cyc_Absyn_String_c:
0){ _LL60: _temp59=(( struct Cyc_Absyn_String_c_struct*) _temp57)->f1; goto
_LL48;} else{ goto _LL49;}} else{ goto _LL49;} _LL49: if(*(( int*) _temp45) == 
Cyc_Absyn_Comprehension_e){ _LL62: _temp61=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp45)->f2; goto _LL50;} else{ goto _LL51;} _LL51: if(*(( int*) _temp45) == 
Cyc_Absyn_UnresolvedMem_e){ _LL64: _temp63=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp45)->f2; goto _LL52;} else{ goto _LL53;} _LL53: if(*(( int*) _temp45) == 
Cyc_Absyn_Array_e){ _LL66: _temp65=(( struct Cyc_Absyn_Array_e_struct*) _temp45)->f1;
goto _LL54;} else{ goto _LL55;} _LL55: goto _LL56; _LL48: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp67=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp67[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp68; _temp68.tag= Cyc_Absyn_ArrayType; _temp68.f1=(
void*) _temp43; _temp68.f2= _temp41; _temp68.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp(
_get_arr_size( _temp59, sizeof( unsigned char)), 0); _temp68;}); _temp67;})));
goto _LL46; _LL50: t=( void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp69=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp69[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp70; _temp70.tag= Cyc_Absyn_ArrayType;
_temp70.f1=( void*) _temp43; _temp70.f2= _temp41; _temp70.f3=( struct Cyc_Absyn_Exp*)
_temp61; _temp70;}); _temp69;}))); goto _LL46; _LL52: _temp65= _temp63; goto
_LL54; _LL54: t=( void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp71=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp71[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp72; _temp72.tag= Cyc_Absyn_ArrayType;
_temp72.f1=( void*) _temp43; _temp72.f2= _temp41; _temp72.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp65), 0); _temp72;}); _temp71;}))); goto _LL46; _LL56: goto _LL46; _LL46:;}
goto _LL33; _LL37: goto _LL33; _LL33:;} Cyc_Tcutil_check_valid_toplevel_type(
loc, te, t);{ void* _temp73= Cyc_Tcutil_compress( t); _LL75: if(( unsigned int)
_temp73 >  3u?*(( int*) _temp73) ==  Cyc_Absyn_ArrayType: 0){ goto _LL76;} else{
goto _LL77;} _LL77: goto _LL78; _LL76: vd->escapes= 0; goto _LL74; _LL78: goto
_LL74; _LL74:;} if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);} if( sc == ( void*) Cyc_Absyn_Extern? 1: sc == ( void*) Cyc_Absyn_ExternC){
if( vd->initializer !=  0){({ void* _temp79[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("extern declaration should not have initializer", sizeof( unsigned char),
47u), _tag_arr( _temp79, sizeof( void*), 0u));});}} else{ if( ! Cyc_Tcutil_is_function_type(
t)){ for( 0; atts !=  0; atts= atts->tl){ void* _temp80=( void*) atts->hd; _LL82:
if(( unsigned int) _temp80 >  16u?*(( int*) _temp80) ==  Cyc_Absyn_Aligned_att:
0){ goto _LL83;} else{ goto _LL84;} _LL84: if(( unsigned int) _temp80 >  16u?*((
int*) _temp80) ==  Cyc_Absyn_Section_att: 0){ goto _LL85;} else{ goto _LL86;}
_LL86: if( _temp80 == ( void*) Cyc_Absyn_Nocommon_att){ goto _LL87;} else{ goto
_LL88;} _LL88: if( _temp80 == ( void*) Cyc_Absyn_Shared_att){ goto _LL89;} else{
goto _LL90;} _LL90: if( _temp80 == ( void*) Cyc_Absyn_Unused_att){ goto _LL91;}
else{ goto _LL92;} _LL92: if( _temp80 == ( void*) Cyc_Absyn_Weak_att){ goto
_LL93;} else{ goto _LL94;} _LL94: if( _temp80 == ( void*) Cyc_Absyn_Dllimport_att){
goto _LL95;} else{ goto _LL96;} _LL96: if( _temp80 == ( void*) Cyc_Absyn_Dllexport_att){
goto _LL97;} else{ goto _LL98;} _LL98: goto _LL99; _LL83: goto _LL85; _LL85:
goto _LL87; _LL87: goto _LL89; _LL89: goto _LL91; _LL91: goto _LL93; _LL93: goto
_LL95; _LL95: goto _LL97; _LL97: continue; _LL99:({ struct Cyc_Std_String_pa_struct
_temp102; _temp102.tag= Cyc_Std_String_pa; _temp102.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ struct Cyc_Std_String_pa_struct _temp101; _temp101.tag= Cyc_Std_String_pa;
_temp101.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*) atts->hd);{
void* _temp100[ 2u]={& _temp101,& _temp102}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s for variable %s",
sizeof( unsigned char), 33u), _tag_arr( _temp100, sizeof( void*), 2u));}}});
goto _LL81; _LL81:;} if( vd->initializer ==  0){ if( check_var_init? ! Cyc_Tcutil_supports_default(
t): 0){({ struct Cyc_Std_String_pa_struct _temp105; _temp105.tag= Cyc_Std_String_pa;
_temp105.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct
_temp104; _temp104.tag= Cyc_Std_String_pa; _temp104.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp103[ 2u]={& _temp104,& _temp105}; Cyc_Tcutil_terr( loc,
_tag_arr("initializer required for variable %s of type %s", sizeof(
unsigned char), 48u), _tag_arr( _temp103, sizeof( void*), 2u));}}});}} else{
struct Cyc_Absyn_Exp* _temp106=( struct Cyc_Absyn_Exp*) _check_null( vd->initializer);
void* _temp107= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, _temp106); if( !
Cyc_Tcutil_coerce_assign( te, _temp106, t)){({ struct Cyc_Std_String_pa_struct
_temp111; _temp111.tag= Cyc_Std_String_pa; _temp111.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp107);{ struct Cyc_Std_String_pa_struct _temp110; _temp110.tag= Cyc_Std_String_pa;
_temp110.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct
_temp109; _temp109.tag= Cyc_Std_String_pa; _temp109.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp108[ 3u]={& _temp109,& _temp110,& _temp111}; Cyc_Tcutil_terr(
loc, _tag_arr("%s is declared with type \n%s\n but initialized with type \n%s",
sizeof( unsigned char), 60u), _tag_arr( _temp108, sizeof( void*), 3u));}}}});
Cyc_Tcutil_explain_failure();} if( ! Cyc_Tcutil_is_const_exp( te, _temp106)){({
void* _temp112[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("initializer is not a constant expression",
sizeof( unsigned char), 41u), _tag_arr( _temp112, sizeof( void*), 0u));});}}}
else{ for( 0; atts !=  0; atts= atts->tl){ void* _temp113=( void*) atts->hd;
_LL115: if(( unsigned int) _temp113 >  16u?*(( int*) _temp113) ==  Cyc_Absyn_Regparm_att:
0){ goto _LL116;} else{ goto _LL117;} _LL117: if( _temp113 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL118;} else{ goto _LL119;} _LL119: if( _temp113 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL120;} else{ goto _LL121;} _LL121: if( _temp113 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL122;} else{ goto _LL123;} _LL123: if( _temp113 == ( void*) Cyc_Absyn_Noreturn_att){
goto _LL124;} else{ goto _LL125;} _LL125: if(( unsigned int) _temp113 >  16u?*((
int*) _temp113) ==  Cyc_Absyn_Format_att: 0){ goto _LL126;} else{ goto _LL127;}
_LL127: if( _temp113 == ( void*) Cyc_Absyn_Const_att){ goto _LL128;} else{ goto
_LL129;} _LL129: if(( unsigned int) _temp113 >  16u?*(( int*) _temp113) ==  Cyc_Absyn_Aligned_att:
0){ goto _LL130;} else{ goto _LL131;} _LL131: if( _temp113 == ( void*) Cyc_Absyn_Packed_att){
goto _LL132;} else{ goto _LL133;} _LL133: goto _LL134; _LL116: goto _LL118;
_LL118: goto _LL120; _LL120: goto _LL122; _LL122: goto _LL124; _LL124: goto
_LL126; _LL126: goto _LL128; _LL128:({ void* _temp135[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("tcVardecl: fn type attributes in function var decl",
sizeof( unsigned char), 51u), _tag_arr( _temp135, sizeof( void*), 0u));}); goto
_LL114; _LL130: goto _LL132; _LL132:({ struct Cyc_Std_String_pa_struct _temp137;
_temp137.tag= Cyc_Std_String_pa; _temp137.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) atts->hd);{ void* _temp136[ 1u]={& _temp137}; Cyc_Tcutil_terr( loc,
_tag_arr("bad attribute %s in function declaration", sizeof( unsigned char), 41u),
_tag_arr( _temp136, sizeof( void*), 1u));}}); goto _LL114; _LL134: continue;
_LL114:;}}}{ struct _handler_cons _temp138; _push_handler(& _temp138);{ int
_temp140= 0; if( setjmp( _temp138.handler)){ _temp140= 1;} if( ! _temp140){{
struct _tuple4* _temp141=(( struct _tuple4*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k)) Cyc_Dict_lookup)( ge->ordinaries, v); void* _temp142=(*
_temp141).f1; void* _temp154; _LL144: if(*(( int*) _temp142) ==  Cyc_Tcenv_VarRes){
_LL155: _temp154=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp142)->f1; goto
_LL145;} else{ goto _LL146;} _LL146: if(*(( int*) _temp142) ==  Cyc_Tcenv_StructRes){
goto _LL147;} else{ goto _LL148;} _LL148: if(*(( int*) _temp142) ==  Cyc_Tcenv_TunionRes){
goto _LL149;} else{ goto _LL150;} _LL150: if(*(( int*) _temp142) ==  Cyc_Tcenv_AnonEnumRes){
goto _LL151;} else{ goto _LL152;} _LL152: if(*(( int*) _temp142) ==  Cyc_Tcenv_EnumRes){
goto _LL153;} else{ goto _LL143;} _LL145: { struct Cyc_Absyn_Global_b_struct*
_temp156=({ struct Cyc_Absyn_Global_b_struct* _temp161=( struct Cyc_Absyn_Global_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp161[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp162; _temp162.tag= Cyc_Absyn_Global_b; _temp162.f1=
vd; _temp162;}); _temp161;}); void* _temp157= Cyc_Tcdecl_merge_binding( _temp154,(
void*) _temp156, loc, Cyc_Tc_tc_msg); if( _temp157 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL143;} if( _temp157 ==  _temp154?(* _temp141).f2: 0){ goto _LL143;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
_tuple4* v)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple4* _temp158=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp158->f1=( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp159=( struct Cyc_Tcenv_VarRes_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp159[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp160; _temp160.tag= Cyc_Tcenv_VarRes; _temp160.f1=( void*) _temp157;
_temp160;}); _temp159;}); _temp158->f2= 1; _temp158;})); goto _LL143;} _LL147:
goto _LL143; _LL149:({ void* _temp163[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous [x]tunion constructor",
sizeof( unsigned char), 60u), _tag_arr( _temp163, sizeof( void*), 0u));}); goto
_LL143; _LL151: goto _LL153; _LL153:({ void* _temp164[ 0u]={}; Cyc_Tcutil_warn(
loc, _tag_arr("variable declaration shadows previous enum tag", sizeof(
unsigned char), 47u), _tag_arr( _temp164, sizeof( void*), 0u));}); goto _LL143;
_LL143:;}; _pop_handler();} else{ void* _temp139=( void*) _exn_thrown; void*
_temp166= _temp139; _LL168: if( _temp166 ==  Cyc_Dict_Absent){ goto _LL169;}
else{ goto _LL170;} _LL170: goto _LL171; _LL169: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct _tuple4* v)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple4* _temp172=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp172->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp173=( struct Cyc_Tcenv_VarRes_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp173[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp174; _temp174.tag= Cyc_Tcenv_VarRes;
_temp174.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp175=(
struct Cyc_Absyn_Global_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp175[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp176; _temp176.tag= Cyc_Absyn_Global_b;
_temp176.f1= vd; _temp176;}); _temp175;})); _temp174;}); _temp173;}); _temp172->f2=
0; _temp172;})); goto _LL167; _LL171:( void) _throw( _temp166); _LL167:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_arr* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp177=(* fd->name).f1;
struct Cyc_List_List* _temp185; struct Cyc_List_List* _temp187; _LL179: if((
unsigned int) _temp177 >  1u?*(( int*) _temp177) ==  Cyc_Absyn_Rel_n: 0){ _LL186:
_temp185=(( struct Cyc_Absyn_Rel_n_struct*) _temp177)->f1; if( _temp185 ==  0){
goto _LL180;} else{ goto _LL181;}} else{ goto _LL181;} _LL181: if(( unsigned int)
_temp177 >  1u?*(( int*) _temp177) ==  Cyc_Absyn_Abs_n: 0){ _LL188: _temp187=((
struct Cyc_Absyn_Abs_n_struct*) _temp177)->f1; goto _LL182;} else{ goto _LL183;}
_LL183: goto _LL184; _LL180: goto _LL178; _LL182:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp189=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp189[ 0]=({ struct
Cyc_Core_Impossible_struct _temp190; _temp190.tag= Cyc_Core_Impossible; _temp190.f1=
_tag_arr("tc: Abs_n in tcFndecl", sizeof( unsigned char), 22u); _temp190;});
_temp189;})); _LL184:({ struct Cyc_Std_String_pa_struct _temp192; _temp192.tag=
Cyc_Std_String_pa; _temp192.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( fd->name);{
void* _temp191[ 1u]={& _temp192}; Cyc_Tcutil_terr( loc, _tag_arr("qualified function declarations are not implemented (%s)",
sizeof( unsigned char), 57u), _tag_arr( _temp191, sizeof( void*), 1u));}});
return; _LL178:;}(* fd->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp193=( struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp193[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp194; _temp194.tag= Cyc_Absyn_Abs_n;
_temp194.f1= te->ns; _temp194;}); _temp193;}); Cyc_Tcutil_check_fndecl_valid_type(
loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ( fd); fd->attributes= Cyc_Tc_transfer_fn_type_atts(
t, fd->attributes);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 
0; atts= atts->tl){ void* _temp195=( void*) atts->hd; _LL197: if( _temp195 == (
void*) Cyc_Absyn_Packed_att){ goto _LL198;} else{ goto _LL199;} _LL199: if((
unsigned int) _temp195 >  16u?*(( int*) _temp195) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL200;} else{ goto _LL201;} _LL201: goto _LL202; _LL198: goto _LL200;
_LL200:({ struct Cyc_Std_String_pa_struct _temp204; _temp204.tag= Cyc_Std_String_pa;
_temp204.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*) atts->hd);{
void* _temp203[ 1u]={& _temp204}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s for function",
sizeof( unsigned char), 30u), _tag_arr( _temp203, sizeof( void*), 1u));}}); goto
_LL196; _LL202: goto _LL196; _LL196:;}}{ struct _handler_cons _temp205;
_push_handler(& _temp205);{ int _temp207= 0; if( setjmp( _temp205.handler)){
_temp207= 1;} if( ! _temp207){{ struct _tuple4* _temp208=(( struct _tuple4*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( ge->ordinaries,
v); void* _temp209=(* _temp208).f1; void* _temp221; _LL211: if(*(( int*)
_temp209) ==  Cyc_Tcenv_VarRes){ _LL222: _temp221=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp209)->f1; goto _LL212;} else{ goto _LL213;} _LL213: if(*(( int*) _temp209)
==  Cyc_Tcenv_StructRes){ goto _LL214;} else{ goto _LL215;} _LL215: if(*(( int*)
_temp209) ==  Cyc_Tcenv_TunionRes){ goto _LL216;} else{ goto _LL217;} _LL217:
if(*(( int*) _temp209) ==  Cyc_Tcenv_AnonEnumRes){ goto _LL218;} else{ goto
_LL219;} _LL219: if(*(( int*) _temp209) ==  Cyc_Tcenv_EnumRes){ goto _LL220;}
else{ goto _LL210;} _LL212: { struct Cyc_Absyn_Funname_b_struct* _temp223=({
struct Cyc_Absyn_Funname_b_struct* _temp228=( struct Cyc_Absyn_Funname_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp228[ 0]=({ struct
Cyc_Absyn_Funname_b_struct _temp229; _temp229.tag= Cyc_Absyn_Funname_b; _temp229.f1=
fd; _temp229;}); _temp228;}); void* _temp224= Cyc_Tcdecl_merge_binding( _temp221,(
void*) _temp223, loc, Cyc_Tc_tc_msg); if( _temp224 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL210;} if( _temp224 ==  _temp221?(* _temp208).f2: 0){ goto _LL210;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
_tuple4* v)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple4* _temp225=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp225->f1=( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp226=( struct Cyc_Tcenv_VarRes_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp226[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp227; _temp227.tag= Cyc_Tcenv_VarRes; _temp227.f1=( void*) _temp224;
_temp227;}); _temp226;}); _temp225->f2= 1; _temp225;})); goto _LL210;} _LL214:
goto _LL210; _LL216:({ void* _temp230[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous [x]tunion constructor",
sizeof( unsigned char), 60u), _tag_arr( _temp230, sizeof( void*), 0u));}); goto
_LL210; _LL218: goto _LL220; _LL220:({ void* _temp231[ 0u]={}; Cyc_Tcutil_warn(
loc, _tag_arr("function declaration shadows previous enum tag", sizeof(
unsigned char), 47u), _tag_arr( _temp231, sizeof( void*), 0u));}); goto _LL210;
_LL210:;}; _pop_handler();} else{ void* _temp206=( void*) _exn_thrown; void*
_temp233= _temp206; _LL235: if( _temp233 ==  Cyc_Dict_Absent){ goto _LL236;}
else{ goto _LL237;} _LL237: goto _LL238; _LL236: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct _tuple4* v)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple4* _temp239=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp239->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp240=( struct Cyc_Tcenv_VarRes_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp240[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp241; _temp241.tag= Cyc_Tcenv_VarRes;
_temp241.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp242=(
struct Cyc_Absyn_Funname_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp242[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp243; _temp243.tag= Cyc_Absyn_Funname_b;
_temp243.f1= fd; _temp243;}); _temp242;})); _temp241;}); _temp240;}); _temp239->f2=
0; _temp239;})); goto _LL234; _LL238:( void) _throw( _temp233); _LL234:;}}} te->le=({
struct Cyc_Core_Opt* _temp244=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp244->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp245=( struct Cyc_Tcenv_Outermost_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp245[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp246; _temp246.tag= Cyc_Tcenv_Outermost;
_temp246.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp246;}); _temp245;}));
_temp244;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){({ void* _temp247[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("function has goto statements to undefined labels",
sizeof( unsigned char), 49u), _tag_arr( _temp247, sizeof( void*), 0u));});} te->le=
0;}} static void Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_arr* v=(* td->name).f2;{ void* _temp248=(* td->name).f1; struct Cyc_List_List*
_temp256; struct Cyc_List_List* _temp258; _LL250: if(( unsigned int) _temp248 > 
1u?*(( int*) _temp248) ==  Cyc_Absyn_Rel_n: 0){ _LL257: _temp256=(( struct Cyc_Absyn_Rel_n_struct*)
_temp248)->f1; if( _temp256 ==  0){ goto _LL251;} else{ goto _LL252;}} else{
goto _LL252;} _LL252: if(( unsigned int) _temp248 >  1u?*(( int*) _temp248) == 
Cyc_Absyn_Abs_n: 0){ _LL259: _temp258=(( struct Cyc_Absyn_Abs_n_struct*)
_temp248)->f1; if( _temp258 ==  0){ goto _LL253;} else{ goto _LL254;}} else{
goto _LL254;} _LL254: goto _LL255; _LL251: goto _LL249; _LL253: goto _LL249;
_LL255:({ struct Cyc_Std_String_pa_struct _temp261; _temp261.tag= Cyc_Std_String_pa;
_temp261.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( td->name);{ void*
_temp260[ 1u]={& _temp261}; Cyc_Tcutil_terr( loc, _tag_arr("qualified typedef declarations are not implemented (%s)",
sizeof( unsigned char), 56u), _tag_arr( _temp260, sizeof( void*), 1u));}});
return; _LL249:;} if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k))
Cyc_Dict_member)( ge->typedefs, v)){({ struct Cyc_Std_String_pa_struct _temp263;
_temp263.tag= Cyc_Std_String_pa; _temp263.f1=( struct _tagged_arr)* v;{ void*
_temp262[ 1u]={& _temp263}; Cyc_Tcutil_terr( loc, _tag_arr("redeclaration of typedef %s",
sizeof( unsigned char), 28u), _tag_arr( _temp262, sizeof( void*), 1u));}});
return;}(* td->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp264=(
struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp264[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp265; _temp265.tag= Cyc_Absyn_Abs_n;
_temp265.f1= te->ns; _temp265;}); _temp264;}); Cyc_Tcutil_check_unique_tvars(
loc, td->tvs); Cyc_Tcutil_add_tvar_identities( td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind, 0,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Typedefdecl* v)) Cyc_Dict_insert)( ge->typedefs, v, td);} static void
Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct _tagged_arr obj, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs){ struct _RegionHandle _temp266= _new_region("uprev_rgn");
struct _RegionHandle* uprev_rgn=& _temp266; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; struct Cyc_List_List* _temp267= fields; for( 0;
_temp267 !=  0; _temp267= _temp267->tl){ struct Cyc_Absyn_Structfield _temp270;
struct Cyc_List_List* _temp271; struct Cyc_Absyn_Exp* _temp273; void* _temp275;
struct Cyc_Absyn_Tqual _temp277; struct _tagged_arr* _temp279; struct Cyc_Absyn_Structfield*
_temp268=( struct Cyc_Absyn_Structfield*) _temp267->hd; _temp270=* _temp268;
_LL280: _temp279= _temp270.name; goto _LL278; _LL278: _temp277= _temp270.tq;
goto _LL276; _LL276: _temp275=( void*) _temp270.type; goto _LL274; _LL274:
_temp273= _temp270.width; goto _LL272; _LL272: _temp271= _temp270.attributes;
goto _LL269; _LL269: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_zstrptrcmp, prev_fields, _temp279)){({ struct Cyc_Std_String_pa_struct
_temp283; _temp283.tag= Cyc_Std_String_pa; _temp283.f1=( struct _tagged_arr) obj;{
struct Cyc_Std_String_pa_struct _temp282; _temp282.tag= Cyc_Std_String_pa;
_temp282.f1=( struct _tagged_arr)* _temp279;{ void* _temp281[ 2u]={& _temp282,&
_temp283}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in %s", sizeof(
unsigned char), 25u), _tag_arr( _temp281, sizeof( void*), 2u));}}});} if( Cyc_Std_strcmp(*
_temp279, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){ prev_fields=({
struct Cyc_List_List* _temp284=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp284->hd=( void*) _temp279;
_temp284->tl= prev_fields; _temp284;});} Cyc_Tcutil_check_type( loc, te, tvs,(
void*) Cyc_Absyn_MemKind, 0, _temp275); Cyc_Tcutil_check_bitfield( loc, te,
_temp275, _temp273, _temp279);}}; _pop_region( uprev_rgn);} static void Cyc_Tc_tcStructOrUniondecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct _tagged_arr obj,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_arr* _temp285=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
sd->name))->v)).f2; if( sd->name ==  0){({ struct Cyc_Std_String_pa_struct
_temp287; _temp287.tag= Cyc_Std_String_pa; _temp287.f1=( struct _tagged_arr) obj;{
void* _temp286[ 1u]={& _temp287}; Cyc_Tcutil_terr( loc, _tag_arr("anonymous %ss are not allowed at top level",
sizeof( unsigned char), 43u), _tag_arr( _temp286, sizeof( void*), 1u));}});
return;}{ struct Cyc_List_List* atts= sd->attributes; for( 0; atts !=  0; atts=
atts->tl){ void* _temp288=( void*) atts->hd; _LL290: if( _temp288 == ( void*)
Cyc_Absyn_Packed_att){ goto _LL291;} else{ goto _LL292;} _LL292: if((
unsigned int) _temp288 >  16u?*(( int*) _temp288) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL293;} else{ goto _LL294;} _LL294: goto _LL295; _LL291: continue; _LL293:
continue; _LL295:({ struct Cyc_Std_String_pa_struct _temp299; _temp299.tag= Cyc_Std_String_pa;
_temp299.f1=( struct _tagged_arr)* _temp285;{ struct Cyc_Std_String_pa_struct
_temp298; _temp298.tag= Cyc_Std_String_pa; _temp298.f1=( struct _tagged_arr) obj;{
struct Cyc_Std_String_pa_struct _temp297; _temp297.tag= Cyc_Std_String_pa;
_temp297.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*) atts->hd);{
void* _temp296[ 3u]={& _temp297,& _temp298,& _temp299}; Cyc_Tcutil_terr( loc,
_tag_arr("bad attribute %s in %s %s definition", sizeof( unsigned char), 37u),
_tag_arr( _temp296, sizeof( void*), 3u));}}}}); goto _LL289; _LL289:;}}{ struct
Cyc_List_List* tvs= sd->tvs; for( 0; tvs !=  0; tvs= tvs->tl){ void* _temp300=
Cyc_Absyn_compress_kb(( void*)(( struct Cyc_Absyn_Tvar*) tvs->hd)->kind); struct
Cyc_Core_Opt* _temp312; struct Cyc_Core_Opt** _temp314; void* _temp315; struct
Cyc_Core_Opt* _temp317; struct Cyc_Core_Opt** _temp319; void* _temp320; struct
Cyc_Core_Opt* _temp322; struct Cyc_Core_Opt** _temp324; void* _temp325; _LL302:
if(*(( int*) _temp300) ==  Cyc_Absyn_Unknown_kb){ _LL313: _temp312=(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp300)->f1; _temp314=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp300)->f1; goto _LL303;} else{ goto _LL304;} _LL304: if(*(( int*) _temp300)
==  Cyc_Absyn_Less_kb){ _LL318: _temp317=(( struct Cyc_Absyn_Less_kb_struct*)
_temp300)->f1; _temp319=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp300)->f1; goto _LL316; _LL316: _temp315=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp300)->f2; if( _temp315 == ( void*) Cyc_Absyn_MemKind){ goto _LL305;} else{
goto _LL306;}} else{ goto _LL306;} _LL306: if(*(( int*) _temp300) ==  Cyc_Absyn_Less_kb){
_LL323: _temp322=(( struct Cyc_Absyn_Less_kb_struct*) _temp300)->f1; _temp324=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*) _temp300)->f1; goto
_LL321; _LL321: _temp320=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp300)->f2;
if( _temp320 == ( void*) Cyc_Absyn_AnyKind){ goto _LL307;} else{ goto _LL308;}}
else{ goto _LL308;} _LL308: if(*(( int*) _temp300) ==  Cyc_Absyn_Eq_kb){ _LL326:
_temp325=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp300)->f1; if( _temp325
== ( void*) Cyc_Absyn_MemKind){ goto _LL309;} else{ goto _LL310;}} else{ goto
_LL310;} _LL310: goto _LL311; _LL303: _temp319= _temp314; goto _LL305; _LL305:
_temp324= _temp319; goto _LL307; _LL307:* _temp324=({ struct Cyc_Core_Opt*
_temp327=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp327->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct* _temp328=( struct
Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp328[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp329; _temp329.tag= Cyc_Absyn_Eq_kb;
_temp329.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp329;}); _temp328;}));
_temp327;}); continue; _LL309:({ struct Cyc_Std_String_pa_struct _temp333;
_temp333.tag= Cyc_Std_String_pa; _temp333.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)
tvs->hd)->name;{ struct Cyc_Std_String_pa_struct _temp332; _temp332.tag= Cyc_Std_String_pa;
_temp332.f1=( struct _tagged_arr)* _temp285;{ struct Cyc_Std_String_pa_struct
_temp331; _temp331.tag= Cyc_Std_String_pa; _temp331.f1=( struct _tagged_arr) obj;{
void* _temp330[ 3u]={& _temp331,& _temp332,& _temp333}; Cyc_Tcutil_terr( loc,
_tag_arr("%s %s attempts to abstract type variable %s of kind M", sizeof(
unsigned char), 54u), _tag_arr( _temp330, sizeof( void*), 3u));}}}}); continue;
_LL311: continue; _LL301:;}}{ void* _temp334=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1; struct Cyc_List_List* _temp342; struct Cyc_List_List*
_temp344; _LL336: if(( unsigned int) _temp334 >  1u?*(( int*) _temp334) ==  Cyc_Absyn_Rel_n:
0){ _LL343: _temp342=(( struct Cyc_Absyn_Rel_n_struct*) _temp334)->f1; if(
_temp342 ==  0){ goto _LL337;} else{ goto _LL338;}} else{ goto _LL338;} _LL338:
if(( unsigned int) _temp334 >  1u?*(( int*) _temp334) ==  Cyc_Absyn_Abs_n: 0){
_LL345: _temp344=(( struct Cyc_Absyn_Abs_n_struct*) _temp334)->f1; if( _temp344
==  0){ goto _LL339;} else{ goto _LL340;}} else{ goto _LL340;} _LL340: goto
_LL341; _LL337: goto _LL335; _LL339: goto _LL335; _LL341:({ struct Cyc_Std_String_pa_struct
_temp347; _temp347.tag= Cyc_Std_String_pa; _temp347.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v);{ void*
_temp346[ 1u]={& _temp347}; Cyc_Tcutil_terr( loc, _tag_arr("qualified struct declarations are not implemented (%s)",
sizeof( unsigned char), 55u), _tag_arr( _temp346, sizeof( void*), 1u));}});
return; _LL335:;}(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp348=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp348[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp349; _temp349.tag= Cyc_Absyn_Abs_n; _temp349.f1= te->ns; _temp349;});
_temp348;}); Cyc_Tcutil_check_unique_tvars( loc, sd->tvs); Cyc_Tcutil_add_tvar_identities(
sd->tvs);} struct _tuple5{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_arr* _temp350=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
sd->name))->v)).f2; const unsigned char* _temp351="struct"; Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp351, sizeof( unsigned char), 7u), loc, sd);{ struct Cyc_List_List*
_temp352= sd->tvs;{ struct _tuple5 _temp354=({ struct _tuple5 _temp353; _temp353.f1=
sd->fields; _temp353.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp350);
_temp353;}); struct Cyc_Core_Opt* _temp364; struct Cyc_Core_Opt* _temp366;
struct Cyc_Core_Opt* _temp368; struct Cyc_Core_Opt* _temp370; struct Cyc_Core_Opt
_temp372; struct Cyc_List_List* _temp373; struct Cyc_Core_Opt* _temp375; struct
Cyc_Core_Opt _temp377; struct Cyc_Absyn_Structdecl** _temp378; struct Cyc_Core_Opt*
_temp380; struct Cyc_Core_Opt _temp382; struct Cyc_List_List* _temp383; struct
Cyc_Core_Opt* _temp385; struct Cyc_Core_Opt _temp387; struct Cyc_Absyn_Structdecl**
_temp388; struct Cyc_Core_Opt* _temp390; _LL356: _LL367: _temp366= _temp354.f1;
if( _temp366 ==  0){ goto _LL365;} else{ goto _LL358;} _LL365: _temp364=
_temp354.f2; if( _temp364 ==  0){ goto _LL357;} else{ goto _LL358;} _LL358:
_LL371: _temp370= _temp354.f1; if( _temp370 ==  0){ goto _LL360;} else{ _temp372=*
_temp370; _LL374: _temp373=( struct Cyc_List_List*) _temp372.v; goto _LL369;}
_LL369: _temp368= _temp354.f2; if( _temp368 ==  0){ goto _LL359;} else{ goto
_LL360;} _LL360: _LL381: _temp380= _temp354.f1; if( _temp380 ==  0){ goto _LL362;}
else{ _temp382=* _temp380; _LL384: _temp383=( struct Cyc_List_List*) _temp382.v;
goto _LL376;} _LL376: _temp375= _temp354.f2; if( _temp375 ==  0){ goto _LL362;}
else{ _temp377=* _temp375; _LL379: _temp378=( struct Cyc_Absyn_Structdecl**)
_temp377.v; goto _LL361;} _LL362: _LL391: _temp390= _temp354.f1; if( _temp390 == 
0){ goto _LL386;} else{ goto _LL355;} _LL386: _temp385= _temp354.f2; if(
_temp385 ==  0){ goto _LL355;} else{ _temp387=* _temp385; _LL389: _temp388=(
struct Cyc_Absyn_Structdecl**) _temp387.v; goto _LL363;} _LL357: ge->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Structdecl** v)) Cyc_Dict_insert)( ge->structdecls, _temp350,({ struct
Cyc_Absyn_Structdecl** _temp392=( struct Cyc_Absyn_Structdecl**) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp392[ 0]= sd; _temp392;})); goto
_LL355; _LL359: { struct Cyc_Absyn_Structdecl** _temp393=({ struct Cyc_Absyn_Structdecl**
_temp394=( struct Cyc_Absyn_Structdecl**) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp394[ 0]=({ struct Cyc_Absyn_Structdecl* _temp395=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp395->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp395->name= sd->name; _temp395->tvs= _temp352; _temp395->fields=
0; _temp395->attributes= 0; _temp395;}); _temp394;}); ge->structdecls=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl**
v)) Cyc_Dict_insert)( ge->structdecls, _temp350, _temp393); Cyc_Tc_tcStructFields(
te, ge, loc, _tag_arr( _temp351, sizeof( unsigned char), 7u), _temp373, _temp352);*
_temp393= sd; goto _LL355;} _LL361: { struct Cyc_Absyn_Structdecl* _temp396=*
_temp378;* _temp378=({ struct Cyc_Absyn_Structdecl* _temp397=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp397->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp397->name= sd->name; _temp397->tvs= _temp352; _temp397->fields=
0; _temp397->attributes= 0; _temp397;}); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp351, sizeof( unsigned char), 7u), _temp383, _temp352);* _temp378=
_temp396; _temp388= _temp378; goto _LL363;} _LL363: { struct Cyc_Absyn_Structdecl*
_temp398= Cyc_Tcdecl_merge_structdecl(* _temp388, sd, loc, Cyc_Tc_tc_msg); if(
_temp398 ==  0){ return;} else{* _temp388=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp398); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp398);
goto _LL355;}} _LL355:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, struct _tuple4* v)) Cyc_Dict_insert)( ge->ordinaries,
_temp350,({ struct _tuple4* _temp399=( struct _tuple4*) _cycalloc( sizeof(
struct _tuple4)); _temp399->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp400=( struct Cyc_Tcenv_StructRes_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp400[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp401; _temp401.tag= Cyc_Tcenv_StructRes;
_temp401.f1= sd; _temp401;}); _temp400;}); _temp399->f2= 1; _temp399;}));}} void
Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct
Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct _tagged_arr*
_temp402=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)).f2;
const unsigned char* _temp403="union"; Cyc_Tc_tcStructOrUniondecl( te, ge,
_tag_arr( _temp403, sizeof( unsigned char), 6u), loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp404= ud->tvs; struct _tuple5 _temp406=({ struct
_tuple5 _temp405; _temp405.f1= ud->fields; _temp405.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)( ge->uniondecls,
_temp402); _temp405;}); struct Cyc_Core_Opt* _temp416; struct Cyc_Core_Opt*
_temp418; struct Cyc_Core_Opt* _temp420; struct Cyc_Core_Opt* _temp422; struct
Cyc_Core_Opt _temp424; struct Cyc_List_List* _temp425; struct Cyc_Core_Opt*
_temp427; struct Cyc_Core_Opt _temp429; struct Cyc_Absyn_Uniondecl** _temp430;
struct Cyc_Core_Opt* _temp432; struct Cyc_Core_Opt _temp434; struct Cyc_List_List*
_temp435; struct Cyc_Core_Opt* _temp437; struct Cyc_Core_Opt _temp439; struct
Cyc_Absyn_Uniondecl** _temp440; struct Cyc_Core_Opt* _temp442; _LL408: _LL419:
_temp418= _temp406.f1; if( _temp418 ==  0){ goto _LL417;} else{ goto _LL410;}
_LL417: _temp416= _temp406.f2; if( _temp416 ==  0){ goto _LL409;} else{ goto
_LL410;} _LL410: _LL423: _temp422= _temp406.f1; if( _temp422 ==  0){ goto _LL412;}
else{ _temp424=* _temp422; _LL426: _temp425=( struct Cyc_List_List*) _temp424.v;
goto _LL421;} _LL421: _temp420= _temp406.f2; if( _temp420 ==  0){ goto _LL411;}
else{ goto _LL412;} _LL412: _LL433: _temp432= _temp406.f1; if( _temp432 ==  0){
goto _LL414;} else{ _temp434=* _temp432; _LL436: _temp435=( struct Cyc_List_List*)
_temp434.v; goto _LL428;} _LL428: _temp427= _temp406.f2; if( _temp427 ==  0){
goto _LL414;} else{ _temp429=* _temp427; _LL431: _temp430=( struct Cyc_Absyn_Uniondecl**)
_temp429.v; goto _LL413;} _LL414: _LL443: _temp442= _temp406.f1; if( _temp442 == 
0){ goto _LL438;} else{ goto _LL407;} _LL438: _temp437= _temp406.f2; if(
_temp437 ==  0){ goto _LL407;} else{ _temp439=* _temp437; _LL441: _temp440=(
struct Cyc_Absyn_Uniondecl**) _temp439.v; goto _LL415;} _LL409: ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Uniondecl** v)) Cyc_Dict_insert)( ge->uniondecls, _temp402,({ struct
Cyc_Absyn_Uniondecl** _temp444=( struct Cyc_Absyn_Uniondecl**) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl*)); _temp444[ 0]= ud; _temp444;})); goto
_LL407; _LL411: { struct Cyc_Absyn_Uniondecl** _temp445=({ struct Cyc_Absyn_Uniondecl**
_temp450=( struct Cyc_Absyn_Uniondecl**) _cycalloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp450[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp451=( struct Cyc_Absyn_Uniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp451->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp451->name= ud->name; _temp451->tvs= _temp404; _temp451->fields=
0; _temp451->attributes= ud->attributes; _temp451;}); _temp450;}); ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Uniondecl** v)) Cyc_Dict_insert)( ge->uniondecls, _temp402, _temp445);
Cyc_Tc_tcStructFields( te, ge, loc, _tag_arr( _temp403, sizeof( unsigned char),
6u), _temp425, _temp404);{ struct Cyc_List_List* f= _temp425; for( 0; f !=  0; f=
f->tl){ if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*) f->hd)->type)){({
struct Cyc_Std_String_pa_struct _temp449; _temp449.tag= Cyc_Std_String_pa;
_temp449.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Absyn_Structfield*)
f->hd)->type);{ struct Cyc_Std_String_pa_struct _temp448; _temp448.tag= Cyc_Std_String_pa;
_temp448.f1=( struct _tagged_arr)* _temp402;{ struct Cyc_Std_String_pa_struct
_temp447; _temp447.tag= Cyc_Std_String_pa; _temp447.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Structfield*) f->hd)->name;{ void* _temp446[ 3u]={& _temp447,&
_temp448,& _temp449}; Cyc_Tcutil_terr( loc, _tag_arr("field %s of union %s has type %s which is not `bits-only'",
sizeof( unsigned char), 58u), _tag_arr( _temp446, sizeof( void*), 3u));}}}});}}}*
_temp445= ud; goto _LL407;} _LL413: { struct Cyc_Absyn_Uniondecl* _temp452=*
_temp430;* _temp430=({ struct Cyc_Absyn_Uniondecl* _temp453=( struct Cyc_Absyn_Uniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp453->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp453->name= ud->name; _temp453->tvs= _temp404; _temp453->fields=
0; _temp453->attributes= ud->attributes; _temp453;}); Cyc_Tc_tcStructFields( te,
ge, loc, _tag_arr( _temp403, sizeof( unsigned char), 6u), _temp435, _temp404);*
_temp430= _temp452; _temp440= _temp430; goto _LL415;} _LL415: { struct Cyc_Absyn_Uniondecl*
_temp454= Cyc_Tcdecl_merge_uniondecl(* _temp440, ud, loc, Cyc_Tc_tc_msg); if(
_temp454 ==  0){ return;} else{* _temp440=( struct Cyc_Absyn_Uniondecl*)
_check_null( _temp454); goto _LL407;}} _LL407:;}} struct _tuple6{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp455= fields; for( 0; _temp455 !=  0; _temp455= _temp455->tl){
struct Cyc_Absyn_Tunionfield* _temp456=( struct Cyc_Absyn_Tunionfield*) _temp455->hd;{
struct Cyc_List_List* tvs= _temp456->tvs; for( 0; tvs !=  0; tvs= tvs->tl){
struct Cyc_Absyn_Tvar* _temp457=( struct Cyc_Absyn_Tvar*) tvs->hd; void*
_temp458= Cyc_Absyn_compress_kb(( void*) _temp457->kind); void* _temp470; struct
Cyc_Core_Opt* _temp472; struct Cyc_Core_Opt** _temp474; void* _temp475; struct
Cyc_Core_Opt* _temp477; struct Cyc_Core_Opt** _temp479; void* _temp480; struct
Cyc_Core_Opt* _temp482; struct Cyc_Core_Opt** _temp484; _LL460: if(*(( int*)
_temp458) ==  Cyc_Absyn_Eq_kb){ _LL471: _temp470=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp458)->f1; if( _temp470 == ( void*) Cyc_Absyn_MemKind){ goto _LL461;} else{
goto _LL462;}} else{ goto _LL462;} _LL462: if(*(( int*) _temp458) ==  Cyc_Absyn_Unknown_kb){
_LL473: _temp472=(( struct Cyc_Absyn_Unknown_kb_struct*) _temp458)->f1; _temp474=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*) _temp458)->f1;
goto _LL463;} else{ goto _LL464;} _LL464: if(*(( int*) _temp458) ==  Cyc_Absyn_Less_kb){
_LL478: _temp477=(( struct Cyc_Absyn_Less_kb_struct*) _temp458)->f1; _temp479=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*) _temp458)->f1; goto
_LL476; _LL476: _temp475=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp458)->f2;
if( _temp475 == ( void*) Cyc_Absyn_MemKind){ goto _LL465;} else{ goto _LL466;}}
else{ goto _LL466;} _LL466: if(*(( int*) _temp458) ==  Cyc_Absyn_Less_kb){
_LL483: _temp482=(( struct Cyc_Absyn_Less_kb_struct*) _temp458)->f1; _temp484=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*) _temp458)->f1; goto
_LL481; _LL481: _temp480=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp458)->f2;
if( _temp480 == ( void*) Cyc_Absyn_AnyKind){ goto _LL467;} else{ goto _LL468;}}
else{ goto _LL468;} _LL468: goto _LL469; _LL461:({ struct Cyc_Std_String_pa_struct
_temp486; _temp486.tag= Cyc_Std_String_pa; _temp486.f1=( struct _tagged_arr)*(*
_temp456->name).f2;{ void* _temp485[ 1u]={& _temp486}; Cyc_Tcutil_terr( _temp456->loc,
_tag_arr("field %s abstracts type variable of kind M", sizeof( unsigned char),
43u), _tag_arr( _temp485, sizeof( void*), 1u));}}); goto _LL459; _LL463:
_temp479= _temp474; goto _LL465; _LL465: _temp484= _temp479; goto _LL467; _LL467:*
_temp484=({ struct Cyc_Core_Opt* _temp487=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp487->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp488=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp488[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp489; _temp489.tag= Cyc_Absyn_Eq_kb;
_temp489.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp489;}); _temp488;}));
_temp487;}); goto _LL459; _LL469: goto _LL459; _LL459:;}}{ struct Cyc_List_List*
alltvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tvs, _temp456->tvs); Cyc_Tcutil_check_unique_tvars( loc,
alltvs); Cyc_Tcutil_add_tvar_identities( _temp456->tvs);{ struct Cyc_List_List*
typs= _temp456->typs; for( 0; typs !=  0; typs= typs->tl){ Cyc_Tcutil_check_type(
_temp456->loc, te, alltvs,( void*) Cyc_Absyn_MemKind, 0,(*(( struct _tuple6*)
typs->hd)).f2);}}{ void* _temp490=(* _temp456->name).f1; struct Cyc_List_List*
_temp500; _LL492: if(( unsigned int) _temp490 >  1u?*(( int*) _temp490) ==  Cyc_Absyn_Rel_n:
0){ _LL501: _temp500=(( struct Cyc_Absyn_Rel_n_struct*) _temp490)->f1; if(
_temp500 ==  0){ goto _LL493;} else{ goto _LL494;}} else{ goto _LL494;} _LL494:
if(( unsigned int) _temp490 >  1u?*(( int*) _temp490) ==  Cyc_Absyn_Rel_n: 0){
goto _LL495;} else{ goto _LL496;} _LL496: if(( unsigned int) _temp490 >  1u?*((
int*) _temp490) ==  Cyc_Absyn_Abs_n: 0){ goto _LL497;} else{ goto _LL498;}
_LL498: if( _temp490 == ( void*) Cyc_Absyn_Loc_n){ goto _LL499;} else{ goto
_LL491;} _LL493: if( is_xtunion){(* _temp456->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp502=( struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp502[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp503; _temp503.tag= Cyc_Absyn_Abs_n;
_temp503.f1= te->ns; _temp503;}); _temp502;});} else{(* _temp456->name).f1=(*
name).f1;} goto _LL491; _LL495:({ struct Cyc_Std_String_pa_struct _temp505;
_temp505.tag= Cyc_Std_String_pa; _temp505.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp456->name);{ void* _temp504[ 1u]={& _temp505}; Cyc_Tcutil_terr( _temp456->loc,
_tag_arr("qualified tunionfield declarations are not allowed (%s)", sizeof(
unsigned char), 56u), _tag_arr( _temp504, sizeof( void*), 1u));}}); goto _LL491;
_LL497: goto _LL491; _LL499:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp506=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp506[ 0]=({ struct Cyc_Core_Impossible_struct _temp507; _temp507.tag= Cyc_Core_Impossible;
_temp507.f1= _tag_arr("tcTunionFields: Loc_n", sizeof( unsigned char), 22u);
_temp507;}); _temp506;})); goto _LL491; _LL491:;}}}}{ struct Cyc_List_List*
fields2; if( is_xtunion){ int _temp508= 1; struct Cyc_List_List* _temp509= Cyc_Tcdecl_sort_xtunion_fields(
fields,& _temp508,(* name).f2, loc, Cyc_Tc_tc_msg); if( _temp508){ fields2=
_temp509;} else{ fields2= 0;}} else{ struct _RegionHandle _temp510= _new_region("uprev_rgn");
struct _RegionHandle* uprev_rgn=& _temp510; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs= fields; for( 0; fs != 
0; fs= fs->tl){ struct Cyc_Absyn_Tunionfield* _temp511=( struct Cyc_Absyn_Tunionfield*)
fs->hd; if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*),
struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp,
prev_fields,(* _temp511->name).f2)){({ struct Cyc_Std_String_pa_struct _temp514;
_temp514.tag= Cyc_Std_String_pa; _temp514.f1=( struct _tagged_arr) obj;{ struct
Cyc_Std_String_pa_struct _temp513; _temp513.tag= Cyc_Std_String_pa; _temp513.f1=(
struct _tagged_arr)*(* _temp511->name).f2;{ void* _temp512[ 2u]={& _temp513,&
_temp514}; Cyc_Tcutil_terr( _temp511->loc, _tag_arr("duplicate field name %s in %s",
sizeof( unsigned char), 30u), _tag_arr( _temp512, sizeof( void*), 2u));}}});}
else{ prev_fields=({ struct Cyc_List_List* _temp515=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp515->hd=( void*)(*
_temp511->name).f2; _temp515->tl= prev_fields; _temp515;});} if(( void*)
_temp511->sc != ( void*) Cyc_Absyn_Public){({ struct Cyc_Std_String_pa_struct
_temp517; _temp517.tag= Cyc_Std_String_pa; _temp517.f1=( struct _tagged_arr)*(*
_temp511->name).f2;{ void* _temp516[ 1u]={& _temp517}; Cyc_Tcutil_warn( loc,
_tag_arr("ignoring scope of field %s", sizeof( unsigned char), 27u), _tag_arr(
_temp516, sizeof( void*), 1u));}});( void*)( _temp511->sc=( void*)(( void*) Cyc_Absyn_Public));}}}
fields2= fields;}; _pop_region( uprev_rgn);}{ struct Cyc_List_List* _temp518=
fields; for( 0; _temp518 !=  0; _temp518= _temp518->tl){ struct Cyc_Absyn_Tunionfield*
_temp519=( struct Cyc_Absyn_Tunionfield*) _temp518->hd; ge->ordinaries=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
_tuple4* v)) Cyc_Dict_insert)( ge->ordinaries,(* _temp519->name).f2,({ struct
_tuple4* _temp520=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp520->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct* _temp521=( struct Cyc_Tcenv_TunionRes_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp521[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp522; _temp522.tag= Cyc_Tcenv_TunionRes; _temp522.f1=
tudres; _temp522.f2= _temp519; _temp522;}); _temp521;}); _temp520->f2= 1;
_temp520;}));}} return fields2;}} void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_arr* v=(* tud->name).f2; struct _tagged_arr obj; if( tud->is_xtunion){
obj= _tag_arr("xtunion", sizeof( unsigned char), 8u);} else{ obj= _tag_arr("tunion",
sizeof( unsigned char), 7u);}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2= tvs2->tl){ void* _temp523= Cyc_Absyn_compress_kb((
void*)(( struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); struct Cyc_Core_Opt* _temp535;
struct Cyc_Core_Opt** _temp537; void* _temp538; struct Cyc_Core_Opt* _temp540;
struct Cyc_Core_Opt** _temp542; void* _temp543; struct Cyc_Core_Opt* _temp545;
struct Cyc_Core_Opt** _temp547; void* _temp548; _LL525: if(*(( int*) _temp523)
==  Cyc_Absyn_Unknown_kb){ _LL536: _temp535=(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp523)->f1; _temp537=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp523)->f1; goto _LL526;} else{ goto _LL527;} _LL527: if(*(( int*) _temp523)
==  Cyc_Absyn_Less_kb){ _LL541: _temp540=(( struct Cyc_Absyn_Less_kb_struct*)
_temp523)->f1; _temp542=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp523)->f1; goto _LL539; _LL539: _temp538=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp523)->f2; if( _temp538 == ( void*) Cyc_Absyn_MemKind){ goto _LL528;} else{
goto _LL529;}} else{ goto _LL529;} _LL529: if(*(( int*) _temp523) ==  Cyc_Absyn_Less_kb){
_LL546: _temp545=(( struct Cyc_Absyn_Less_kb_struct*) _temp523)->f1; _temp547=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*) _temp523)->f1; goto
_LL544; _LL544: _temp543=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp523)->f2;
if( _temp543 == ( void*) Cyc_Absyn_AnyKind){ goto _LL530;} else{ goto _LL531;}}
else{ goto _LL531;} _LL531: if(*(( int*) _temp523) ==  Cyc_Absyn_Eq_kb){ _LL549:
_temp548=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp523)->f1; if( _temp548
== ( void*) Cyc_Absyn_MemKind){ goto _LL532;} else{ goto _LL533;}} else{ goto
_LL533;} _LL533: goto _LL534; _LL526: _temp542= _temp537; goto _LL528; _LL528:
_temp547= _temp542; goto _LL530; _LL530:* _temp547=({ struct Cyc_Core_Opt*
_temp550=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp550->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct* _temp551=( struct
Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp551[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp552; _temp552.tag= Cyc_Absyn_Eq_kb;
_temp552.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp552;}); _temp551;}));
_temp550;}); goto _LL524; _LL532:({ struct Cyc_Std_String_pa_struct _temp556;
_temp556.tag= Cyc_Std_String_pa; _temp556.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)
tvs2->hd)->name;{ struct Cyc_Std_String_pa_struct _temp555; _temp555.tag= Cyc_Std_String_pa;
_temp555.f1=( struct _tagged_arr)* v;{ struct Cyc_Std_String_pa_struct _temp554;
_temp554.tag= Cyc_Std_String_pa; _temp554.f1=( struct _tagged_arr) obj;{ void*
_temp553[ 3u]={& _temp554,& _temp555,& _temp556}; Cyc_Tcutil_terr( loc, _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u), _tag_arr( _temp553, sizeof( void*), 3u));}}}});
goto _LL524; _LL534: goto _LL524; _LL524:;}} Cyc_Tcutil_check_unique_tvars( loc,
tvs); Cyc_Tcutil_add_tvar_identities( tvs);{ struct Cyc_Core_Opt* tud_opt; if(
tud->is_xtunion){{ struct _handler_cons _temp557; _push_handler(& _temp557);{
int _temp559= 0; if( setjmp( _temp557.handler)){ _temp559= 1;} if( ! _temp559){
tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc, tud->name);; _pop_handler();}
else{ void* _temp558=( void*) _exn_thrown; void* _temp561= _temp558; _LL563: if(
_temp561 ==  Cyc_Dict_Absent){ goto _LL564;} else{ goto _LL565;} _LL565: goto
_LL566; _LL564:({ struct Cyc_Std_String_pa_struct _temp568; _temp568.tag= Cyc_Std_String_pa;
_temp568.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tud->name);{ void*
_temp567[ 1u]={& _temp568}; Cyc_Tcutil_terr( loc, _tag_arr("qualified xtunion declaration %s is not an existing xtunion",
sizeof( unsigned char), 60u), _tag_arr( _temp567, sizeof( void*), 1u));}});
return; _LL566:( void) _throw( _temp561); _LL562:;}}} if( tud_opt !=  0){ tud->name=(*((
struct Cyc_Absyn_Tuniondecl**) tud_opt->v))->name;} else{(* tud->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp569=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp569[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp570; _temp570.tag= Cyc_Absyn_Abs_n; _temp570.f1= te->ns; _temp570;});
_temp569;});}} else{{ void* _temp571=(* tud->name).f1; struct Cyc_List_List*
_temp579; _LL573: if(( unsigned int) _temp571 >  1u?*(( int*) _temp571) ==  Cyc_Absyn_Rel_n:
0){ _LL580: _temp579=(( struct Cyc_Absyn_Rel_n_struct*) _temp571)->f1; if(
_temp579 ==  0){ goto _LL574;} else{ goto _LL575;}} else{ goto _LL575;} _LL575:
if(( unsigned int) _temp571 >  1u?*(( int*) _temp571) ==  Cyc_Absyn_Abs_n: 0){
goto _LL576;} else{ goto _LL577;} _LL577: goto _LL578; _LL574:(* tud->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp581=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp581[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp582; _temp582.tag= Cyc_Absyn_Abs_n; _temp582.f1= te->ns; _temp582;});
_temp581;}); goto _LL572; _LL576: goto _LL578; _LL578:({ struct Cyc_Std_String_pa_struct
_temp584; _temp584.tag= Cyc_Std_String_pa; _temp584.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tud->name);{ void* _temp583[ 1u]={& _temp584}; Cyc_Tcutil_terr( loc, _tag_arr("qualified tunion declarations are not implemented (%s)",
sizeof( unsigned char), 55u), _tag_arr( _temp583, sizeof( void*), 1u));}});
return; _LL572:;} tud_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct
_tuple5 _temp586=({ struct _tuple5 _temp585; _temp585.f1= tud->fields; _temp585.f2=
tud_opt; _temp585;}); struct Cyc_Core_Opt* _temp596; struct Cyc_Core_Opt*
_temp598; struct Cyc_Core_Opt* _temp600; struct Cyc_Core_Opt* _temp602; struct
Cyc_Core_Opt _temp604; struct Cyc_List_List* _temp605; struct Cyc_List_List**
_temp607; struct Cyc_Core_Opt* _temp608; struct Cyc_Core_Opt _temp610; struct
Cyc_Absyn_Tuniondecl** _temp611; struct Cyc_Core_Opt* _temp613; struct Cyc_Core_Opt
_temp615; struct Cyc_List_List* _temp616; struct Cyc_List_List** _temp618;
struct Cyc_Core_Opt* _temp619; struct Cyc_Core_Opt _temp621; struct Cyc_Absyn_Tuniondecl**
_temp622; struct Cyc_Core_Opt* _temp624; _LL588: _LL599: _temp598= _temp586.f1;
if( _temp598 ==  0){ goto _LL597;} else{ goto _LL590;} _LL597: _temp596=
_temp586.f2; if( _temp596 ==  0){ goto _LL589;} else{ goto _LL590;} _LL590:
_LL603: _temp602= _temp586.f1; if( _temp602 ==  0){ goto _LL592;} else{ _temp604=*
_temp602; _LL606: _temp605=( struct Cyc_List_List*) _temp604.v; _temp607=(
struct Cyc_List_List**)&(* _temp586.f1).v; goto _LL601;} _LL601: _temp600=
_temp586.f2; if( _temp600 ==  0){ goto _LL591;} else{ goto _LL592;} _LL592:
_LL614: _temp613= _temp586.f1; if( _temp613 ==  0){ goto _LL594;} else{ _temp615=*
_temp613; _LL617: _temp616=( struct Cyc_List_List*) _temp615.v; _temp618=(
struct Cyc_List_List**)&(* _temp586.f1).v; goto _LL609;} _LL609: _temp608=
_temp586.f2; if( _temp608 ==  0){ goto _LL594;} else{ _temp610=* _temp608;
_LL612: _temp611=( struct Cyc_Absyn_Tuniondecl**) _temp610.v; goto _LL593;}
_LL594: _LL625: _temp624= _temp586.f1; if( _temp624 ==  0){ goto _LL620;} else{
goto _LL587;} _LL620: _temp619= _temp586.f2; if( _temp619 ==  0){ goto _LL587;}
else{ _temp621=* _temp619; _LL623: _temp622=( struct Cyc_Absyn_Tuniondecl**)
_temp621.v; goto _LL595;} _LL589: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Tuniondecl** v))
Cyc_Dict_insert)( ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp626=(
struct Cyc_Absyn_Tuniondecl**) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp626[ 0]= tud; _temp626;})); goto _LL587; _LL591: { struct Cyc_Absyn_Tuniondecl**
_temp627=({ struct Cyc_Absyn_Tuniondecl** _temp628=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp628[ 0]=({ struct Cyc_Absyn_Tuniondecl*
_temp629=( struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp629->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp629->name= tud->name;
_temp629->tvs= tvs; _temp629->fields= 0; _temp629->is_xtunion= tud->is_xtunion;
_temp629;}); _temp628;}); ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Tuniondecl** v)) Cyc_Dict_insert)(
ge->tuniondecls, v, _temp627);* _temp607= Cyc_Tc_tcTunionFields( te, ge, loc,
obj, tud->is_xtunion, tud->name,* _temp607, tvs, tud);* _temp627= tud; goto
_LL587;} _LL593: { struct Cyc_Absyn_Tuniondecl* _temp630=* _temp611;* _temp611=({
struct Cyc_Absyn_Tuniondecl* _temp631=( struct Cyc_Absyn_Tuniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp631->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp631->name= tud->name; _temp631->tvs= tvs; _temp631->fields= 0; _temp631->is_xtunion=
tud->is_xtunion; _temp631;});* _temp618= Cyc_Tc_tcTunionFields( te, ge, loc, obj,
tud->is_xtunion, tud->name,* _temp618, tvs, tud);* _temp611= _temp630; _temp622=
_temp611; goto _LL595;} _LL595: { struct Cyc_Absyn_Tuniondecl* _temp632= Cyc_Tcdecl_merge_tuniondecl(*
_temp622, tud, loc, Cyc_Tc_tc_msg); if( _temp632 ==  0){ return;} else{*
_temp622=( struct Cyc_Absyn_Tuniondecl*) _check_null( _temp632); goto _LL587;}}
_LL587:;}}}} void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl* ed){ struct
_tagged_arr* v=(* ed->name).f2;{ void* _temp633=(* ed->name).f1; struct Cyc_List_List*
_temp641; struct Cyc_List_List* _temp643; _LL635: if(( unsigned int) _temp633 > 
1u?*(( int*) _temp633) ==  Cyc_Absyn_Rel_n: 0){ _LL642: _temp641=(( struct Cyc_Absyn_Rel_n_struct*)
_temp633)->f1; if( _temp641 ==  0){ goto _LL636;} else{ goto _LL637;}} else{
goto _LL637;} _LL637: if(( unsigned int) _temp633 >  1u?*(( int*) _temp633) == 
Cyc_Absyn_Abs_n: 0){ _LL644: _temp643=(( struct Cyc_Absyn_Abs_n_struct*)
_temp633)->f1; if( _temp643 ==  0){ goto _LL638;} else{ goto _LL639;}} else{
goto _LL639;} _LL639: goto _LL640; _LL636: goto _LL634; _LL638: goto _LL634;
_LL640:({ struct Cyc_Std_String_pa_struct _temp646; _temp646.tag= Cyc_Std_String_pa;
_temp646.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( ed->name);{ void*
_temp645[ 1u]={& _temp646}; Cyc_Tcutil_terr( loc, _tag_arr("qualified enum declarations are not implemented (%s)",
sizeof( unsigned char), 53u), _tag_arr( _temp645, sizeof( void*), 1u));}});
return; _LL634:;}(* ed->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp647=( struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp647[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp648; _temp648.tag= Cyc_Absyn_Abs_n;
_temp648.f1= te->ns; _temp648;}); _temp647;}); if( ed->fields !=  0){ struct
_RegionHandle _temp649= _new_region("uprev_rgn"); struct _RegionHandle*
uprev_rgn=& _temp649; _push_region( uprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; unsigned int tag_count= 0; struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs !=  0; fs= fs->tl){
struct Cyc_Absyn_Enumfield* _temp650=( struct Cyc_Absyn_Enumfield*) fs->hd; if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,(*
_temp650->name).f2)){({ struct Cyc_Std_String_pa_struct _temp652; _temp652.tag=
Cyc_Std_String_pa; _temp652.f1=( struct _tagged_arr)*(* _temp650->name).f2;{
void* _temp651[ 1u]={& _temp652}; Cyc_Tcutil_terr( _temp650->loc, _tag_arr("duplicate field name %s",
sizeof( unsigned char), 24u), _tag_arr( _temp651, sizeof( void*), 1u));}});}
else{ prev_fields=({ struct Cyc_List_List* _temp653=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp653->hd=( void*)(*
_temp650->name).f2; _temp653->tl= prev_fields; _temp653;});} if( _temp650->tag
==  0){ _temp650->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count,
_temp650->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp650->tag))){({ struct Cyc_Std_String_pa_struct _temp656;
_temp656.tag= Cyc_Std_String_pa; _temp656.f1=( struct _tagged_arr)*(* _temp650->name).f2;{
struct Cyc_Std_String_pa_struct _temp655; _temp655.tag= Cyc_Std_String_pa;
_temp655.f1=( struct _tagged_arr)* v;{ void* _temp654[ 2u]={& _temp655,&
_temp656}; Cyc_Tcutil_terr( loc, _tag_arr("enum %s, field %s: expression is not constant",
sizeof( unsigned char), 46u), _tag_arr( _temp654, sizeof( void*), 2u));}}});}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp650->tag)); tag_count= t1 +  1;(* _temp650->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp657=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp657[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp658; _temp658.tag= Cyc_Absyn_Abs_n; _temp658.f1= te->ns; _temp658;});
_temp657;});}}}; _pop_region( uprev_rgn);}{ struct _handler_cons _temp659;
_push_handler(& _temp659);{ int _temp661= 0; if( setjmp( _temp659.handler)){
_temp661= 1;} if( ! _temp661){{ struct Cyc_Absyn_Enumdecl** _temp662=(( struct
Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)(
ge->enumdecls, v); struct Cyc_Absyn_Enumdecl* _temp663= Cyc_Tcdecl_merge_enumdecl(*
_temp662, ed, loc, Cyc_Tc_tc_msg); if( _temp663 ==  0){ _npop_handler( 0u);
return;}* _temp662=( struct Cyc_Absyn_Enumdecl*) _check_null( _temp663);};
_pop_handler();} else{ void* _temp660=( void*) _exn_thrown; void* _temp665=
_temp660; _LL667: if( _temp665 ==  Cyc_Dict_Absent){ goto _LL668;} else{ goto
_LL669;} _LL669: goto _LL670; _LL668: { struct Cyc_Absyn_Enumdecl** _temp671=({
struct Cyc_Absyn_Enumdecl** _temp672=( struct Cyc_Absyn_Enumdecl**) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl*)); _temp672[ 0]= ed; _temp672;}); ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Enumdecl** v)) Cyc_Dict_insert)( ge->enumdecls, v, _temp671); goto
_LL666;} _LL670:( void) _throw( _temp665); _LL666:;}}} if( ed->fields !=  0){
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; fs !=  0; fs= fs->tl){ struct Cyc_Absyn_Enumfield*
_temp673=( struct Cyc_Absyn_Enumfield*) fs->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct _tuple4* v)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp673->name).f2,({ struct _tuple4* _temp674=( struct
_tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp674->f1=( void*)({ struct
Cyc_Tcenv_EnumRes_struct* _temp675=( struct Cyc_Tcenv_EnumRes_struct*) _cycalloc(
sizeof( struct Cyc_Tcenv_EnumRes_struct)); _temp675[ 0]=({ struct Cyc_Tcenv_EnumRes_struct
_temp676; _temp676.tag= Cyc_Tcenv_EnumRes; _temp676.f1= ed; _temp676.f2=
_temp673; _temp676;}); _temp675;}); _temp674->f2= 1; _temp674;}));}}} static int
Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){ void* _temp677=
sc; _LL679: if( _temp677 == ( void*) Cyc_Absyn_Static){ goto _LL680;} else{ goto
_LL681;} _LL681: if( _temp677 == ( void*) Cyc_Absyn_Abstract){ goto _LL682;}
else{ goto _LL683;} _LL683: if( _temp677 == ( void*) Cyc_Absyn_Public){ goto
_LL684;} else{ goto _LL685;} _LL685: if( _temp677 == ( void*) Cyc_Absyn_Extern){
goto _LL686;} else{ goto _LL687;} _LL687: if( _temp677 == ( void*) Cyc_Absyn_ExternC){
goto _LL688;} else{ goto _LL678;} _LL680:({ void* _temp689[ 0u]={}; Cyc_Tcutil_warn(
loc, _tag_arr("static declaration nested within extern \"C\"", sizeof(
unsigned char), 44u), _tag_arr( _temp689, sizeof( void*), 0u));}); return 0;
_LL682:({ void* _temp690[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("abstract declaration nested within extern \"C\"",
sizeof( unsigned char), 46u), _tag_arr( _temp690, sizeof( void*), 0u));});
return 0; _LL684: return 1; _LL686: return 1; _LL688:({ void* _temp691[ 0u]={};
Cyc_Tcutil_warn( loc, _tag_arr("nested extern \"C\" declaration", sizeof(
unsigned char), 30u), _tag_arr( _temp691, sizeof( void*), 0u));}); return 1;
_LL678:;} static void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
ds0, int in_externC, int check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)(
te->ae, te->ns); struct Cyc_List_List* last= 0; struct Cyc_Dict_Dict* dict= Cyc_Tcgenrep_empty_typerep_dict();
struct Cyc_List_List* _temp692= ds0; for( 0; _temp692 !=  0;( last= _temp692,
_temp692= _temp692->tl)){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
_temp692->hd; struct Cyc_Position_Segment* loc= d->loc; void* _temp693=( void*)
d->r; struct Cyc_Absyn_Vardecl* _temp719; struct Cyc_Absyn_Fndecl* _temp721;
struct Cyc_Absyn_Typedefdecl* _temp723; struct Cyc_Absyn_Structdecl* _temp725;
struct Cyc_Absyn_Uniondecl* _temp727; struct Cyc_Absyn_Tuniondecl* _temp729;
struct Cyc_Absyn_Enumdecl* _temp731; struct Cyc_List_List* _temp733; struct
_tagged_arr* _temp735; struct Cyc_List_List* _temp737; struct _tuple0* _temp739;
struct _tuple0 _temp741; struct _tagged_arr* _temp742; void* _temp744; struct
Cyc_List_List* _temp746; _LL695: if(*(( int*) _temp693) ==  Cyc_Absyn_Let_d){
goto _LL696;} else{ goto _LL697;} _LL697: if(*(( int*) _temp693) ==  Cyc_Absyn_Letv_d){
goto _LL698;} else{ goto _LL699;} _LL699: if(*(( int*) _temp693) ==  Cyc_Absyn_Var_d){
_LL720: _temp719=(( struct Cyc_Absyn_Var_d_struct*) _temp693)->f1; goto _LL700;}
else{ goto _LL701;} _LL701: if(*(( int*) _temp693) ==  Cyc_Absyn_Fn_d){ _LL722:
_temp721=(( struct Cyc_Absyn_Fn_d_struct*) _temp693)->f1; goto _LL702;} else{
goto _LL703;} _LL703: if(*(( int*) _temp693) ==  Cyc_Absyn_Typedef_d){ _LL724:
_temp723=(( struct Cyc_Absyn_Typedef_d_struct*) _temp693)->f1; goto _LL704;}
else{ goto _LL705;} _LL705: if(*(( int*) _temp693) ==  Cyc_Absyn_Struct_d){
_LL726: _temp725=(( struct Cyc_Absyn_Struct_d_struct*) _temp693)->f1; goto
_LL706;} else{ goto _LL707;} _LL707: if(*(( int*) _temp693) ==  Cyc_Absyn_Union_d){
_LL728: _temp727=(( struct Cyc_Absyn_Union_d_struct*) _temp693)->f1; goto _LL708;}
else{ goto _LL709;} _LL709: if(*(( int*) _temp693) ==  Cyc_Absyn_Tunion_d){
_LL730: _temp729=(( struct Cyc_Absyn_Tunion_d_struct*) _temp693)->f1; goto
_LL710;} else{ goto _LL711;} _LL711: if(*(( int*) _temp693) ==  Cyc_Absyn_Enum_d){
_LL732: _temp731=(( struct Cyc_Absyn_Enum_d_struct*) _temp693)->f1; goto _LL712;}
else{ goto _LL713;} _LL713: if(*(( int*) _temp693) ==  Cyc_Absyn_Namespace_d){
_LL736: _temp735=(( struct Cyc_Absyn_Namespace_d_struct*) _temp693)->f1; goto
_LL734; _LL734: _temp733=(( struct Cyc_Absyn_Namespace_d_struct*) _temp693)->f2;
goto _LL714;} else{ goto _LL715;} _LL715: if(*(( int*) _temp693) ==  Cyc_Absyn_Using_d){
_LL740: _temp739=(( struct Cyc_Absyn_Using_d_struct*) _temp693)->f1; _temp741=*
_temp739; _LL745: _temp744= _temp741.f1; goto _LL743; _LL743: _temp742= _temp741.f2;
goto _LL738; _LL738: _temp737=(( struct Cyc_Absyn_Using_d_struct*) _temp693)->f2;
goto _LL716;} else{ goto _LL717;} _LL717: if(*(( int*) _temp693) ==  Cyc_Absyn_ExternC_d){
_LL747: _temp746=(( struct Cyc_Absyn_ExternC_d_struct*) _temp693)->f1; goto
_LL718;} else{ goto _LL694;} _LL696: goto _LL698; _LL698:({ void* _temp748[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("top level let-declarations are not implemented",
sizeof( unsigned char), 47u), _tag_arr( _temp748, sizeof( void*), 0u));}); goto
_LL694; _LL700: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp719->sc):
0){( void*)( _temp719->sc=( void*)(( void*) Cyc_Absyn_ExternC));} if( _temp719->initializer
!=  0){ void* _temp749=( void*)(( struct Cyc_Absyn_Exp*) _check_null( _temp719->initializer))->r;
void* _temp755; _LL751: if(*(( int*) _temp749) ==  Cyc_Absyn_Gentyp_e){ _LL756:
_temp755=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*) _temp749)->f2; goto
_LL752;} else{ goto _LL753;} _LL753: goto _LL754; _LL752: { struct Cyc_Absyn_Exp*
_temp759; struct Cyc_List_List* _temp761; struct Cyc_Dict_Dict* _temp763; struct
_tuple3 _temp757= Cyc_Tcgenrep_tcGenrep( te, ge, loc, _temp755, dict); _LL764:
_temp763= _temp757.f1; goto _LL762; _LL762: _temp761= _temp757.f2; goto _LL760;
_LL760: _temp759= _temp757.f3; goto _LL758; _LL758: dict= _temp763; Cyc_Tc_tc_decls(
te, _temp761, in_externC, check_var_init); _temp719->initializer=( struct Cyc_Absyn_Exp*)
_temp759; Cyc_Tc_tcVardecl( te, ge, loc, _temp719, check_var_init); if( _temp761
!=  0){ if( last !=  0){(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp761, _temp692);(( struct
Cyc_List_List*) _check_null( last))->tl= _temp761;} else{ struct Cyc_List_List
tmp=({ struct Cyc_List_List _temp765; _temp765.hd=( void*)(( struct Cyc_Absyn_Decl*)
_temp692->hd); _temp765.tl= _temp692->tl; _temp765;});( struct Cyc_Absyn_Decl*)(
_temp692->hd=( void*)(( struct Cyc_Absyn_Decl*) _temp761->hd)); _temp692->tl=
_temp761->tl;( struct Cyc_Absyn_Decl*)( _temp761->hd=( void*)(( struct Cyc_Absyn_Decl*)
tmp.hd)); _temp761->tl= tmp.tl;(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp692, _temp761);}}
continue;} _LL754: goto _LL750; _LL750:;} Cyc_Tc_tcVardecl( te, ge, loc,
_temp719, check_var_init); goto _LL694; _LL702: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp721->sc): 0){( void*)( _temp721->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp721); goto _LL694; _LL704: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp723); goto _LL694; _LL706: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp725->sc): 0){( void*)( _temp725->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp725); goto _LL694; _LL708: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp727->sc): 0){( void*)( _temp727->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp727);
goto _LL694; _LL710: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp729->sc): 0){( void*)( _temp729->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp729); goto _LL694; _LL712: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp731->sc): 0){( void*)( _temp731->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp731);
goto _LL694; _LL714: { struct Cyc_List_List* _temp766= te->ns; struct Cyc_List_List*
_temp767=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp766,({ struct Cyc_List_List* _temp768=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp768->hd=( void*) _temp735;
_temp768->tl= 0; _temp768;})); if( !(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge->namespaces, _temp735)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
ge->namespaces, _temp735); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k, struct Cyc_Tcenv_Genv* v)) Cyc_Dict_insert)( te->ae,
_temp767, Cyc_Tcenv_empty_genv());} te->ns= _temp767; Cyc_Tc_tc_decls( te,
_temp733, in_externC, check_var_init); te->ns= _temp766; goto _LL694;} _LL716: {
struct _tagged_arr* first; struct Cyc_List_List* rest;{ void* _temp769= _temp744;
struct Cyc_List_List* _temp781; struct Cyc_List_List* _temp783; struct Cyc_List_List*
_temp785; struct Cyc_List_List _temp787; struct Cyc_List_List* _temp788; struct
_tagged_arr* _temp790; struct Cyc_List_List* _temp792; struct Cyc_List_List
_temp794; struct Cyc_List_List* _temp795; struct _tagged_arr* _temp797; _LL771:
if( _temp769 == ( void*) Cyc_Absyn_Loc_n){ goto _LL772;} else{ goto _LL773;}
_LL773: if(( unsigned int) _temp769 >  1u?*(( int*) _temp769) ==  Cyc_Absyn_Rel_n:
0){ _LL782: _temp781=(( struct Cyc_Absyn_Rel_n_struct*) _temp769)->f1; if(
_temp781 ==  0){ goto _LL774;} else{ goto _LL775;}} else{ goto _LL775;} _LL775:
if(( unsigned int) _temp769 >  1u?*(( int*) _temp769) ==  Cyc_Absyn_Abs_n: 0){
_LL784: _temp783=(( struct Cyc_Absyn_Abs_n_struct*) _temp769)->f1; if( _temp783
==  0){ goto _LL776;} else{ goto _LL777;}} else{ goto _LL777;} _LL777: if((
unsigned int) _temp769 >  1u?*(( int*) _temp769) ==  Cyc_Absyn_Rel_n: 0){ _LL786:
_temp785=(( struct Cyc_Absyn_Rel_n_struct*) _temp769)->f1; if( _temp785 ==  0){
goto _LL779;} else{ _temp787=* _temp785; _LL791: _temp790=( struct _tagged_arr*)
_temp787.hd; goto _LL789; _LL789: _temp788= _temp787.tl; goto _LL778;}} else{
goto _LL779;} _LL779: if(( unsigned int) _temp769 >  1u?*(( int*) _temp769) == 
Cyc_Absyn_Abs_n: 0){ _LL793: _temp792=(( struct Cyc_Absyn_Abs_n_struct*)
_temp769)->f1; if( _temp792 ==  0){ goto _LL770;} else{ _temp794=* _temp792;
_LL798: _temp797=( struct _tagged_arr*) _temp794.hd; goto _LL796; _LL796:
_temp795= _temp794.tl; goto _LL780;}} else{ goto _LL770;} _LL772: goto _LL774;
_LL774: goto _LL776; _LL776: first= _temp742; rest= 0; goto _LL770; _LL778:
_temp797= _temp790; _temp795= _temp788; goto _LL780; _LL780: first= _temp797;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp795,({ struct Cyc_List_List* _temp799=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp799->hd=( void*) _temp742;
_temp799->tl= 0; _temp799;})); goto _LL770; _LL770:;}{ struct Cyc_List_List*
_temp800= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp801=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp801->hd=( void*) _temp800; _temp801->tl= ge->availables;
_temp801;}); Cyc_Tc_tc_decls( te, _temp737, in_externC, check_var_init); ge->availables=((
struct Cyc_List_List*) _check_null( ge->availables))->tl; goto _LL694;}} _LL718:
Cyc_Tc_tc_decls( te, _temp746, 1, check_var_init); goto _LL694; _LL694:;}} void
Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int check_var_init, struct Cyc_List_List*
ds){ Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r); Cyc_Tc_tc_decls( te, ds,
0, check_var_init);} static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env,
struct Cyc_Absyn_Decl* d); static struct Cyc_List_List* Cyc_Tc_treeshake_f(
struct Cyc_Dict_Dict* env, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict*
env, struct Cyc_List_List* x)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env,
ds);} struct _tuple7{ struct Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d){ void* _temp802=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp812; struct Cyc_List_List*
_temp814; struct Cyc_List_List** _temp816; struct Cyc_List_List* _temp817;
struct Cyc_List_List** _temp819; _LL804: if(*(( int*) _temp802) ==  Cyc_Absyn_Var_d){
_LL813: _temp812=(( struct Cyc_Absyn_Var_d_struct*) _temp802)->f1; goto _LL805;}
else{ goto _LL806;} _LL806: if(*(( int*) _temp802) ==  Cyc_Absyn_Using_d){
_LL815: _temp814=(( struct Cyc_Absyn_Using_d_struct*) _temp802)->f2; _temp816=(
struct Cyc_List_List**)&(( struct Cyc_Absyn_Using_d_struct*) _temp802)->f2; goto
_LL807;} else{ goto _LL808;} _LL808: if(*(( int*) _temp802) ==  Cyc_Absyn_Namespace_d){
_LL818: _temp817=(( struct Cyc_Absyn_Namespace_d_struct*) _temp802)->f2;
_temp819=( struct Cyc_List_List**)&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp802)->f2; goto _LL809;} else{ goto _LL810;} _LL810: goto _LL811; _LL805:
if(( void*) _temp812->sc != ( void*) Cyc_Absyn_Extern){ return 1;}{ struct
_tuple0 _temp822; struct _tagged_arr* _temp823; void* _temp825; struct _tuple0*
_temp820= _temp812->name; _temp822=* _temp820; _LL826: _temp825= _temp822.f1;
goto _LL824; _LL824: _temp823= _temp822.f2; goto _LL821; _LL821: { struct Cyc_List_List*
ns;{ void* _temp827= _temp825; struct Cyc_List_List* _temp835; struct Cyc_List_List*
_temp837; _LL829: if( _temp827 == ( void*) Cyc_Absyn_Loc_n){ goto _LL830;} else{
goto _LL831;} _LL831: if(( unsigned int) _temp827 >  1u?*(( int*) _temp827) == 
Cyc_Absyn_Rel_n: 0){ _LL836: _temp835=(( struct Cyc_Absyn_Rel_n_struct*)
_temp827)->f1; goto _LL832;} else{ goto _LL833;} _LL833: if(( unsigned int)
_temp827 >  1u?*(( int*) _temp827) ==  Cyc_Absyn_Abs_n: 0){ _LL838: _temp837=((
struct Cyc_Absyn_Abs_n_struct*) _temp827)->f1; goto _LL834;} else{ goto _LL828;}
_LL830: ns= 0; goto _LL828; _LL832: ns= _temp835; goto _LL828; _LL834: ns=
_temp837; goto _LL828; _LL828:;}{ struct _tuple7* _temp839=(( struct _tuple7*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( env, ns);
struct Cyc_Tcenv_Genv* _temp840=(* _temp839).f1; int _temp841=(*(( struct
_tuple4*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)(
_temp840->ordinaries, _temp823)).f2; if( ! _temp841){(* _temp839).f2=(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)((*
_temp839).f2, _temp823);} return _temp841;}}} _LL807: _temp819= _temp816; goto
_LL809; _LL809:* _temp819= Cyc_Tc_treeshake_f( env,* _temp819); return 1; _LL811:
return 1; _LL803:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_arr* x, void* y){ return !(( int(*)( struct Cyc_Set_Set* s,
struct _tagged_arr* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple7* nsenv){ struct _tuple7 _temp844;
struct Cyc_Set_Set* _temp845; struct Cyc_Tcenv_Genv* _temp847; struct _tuple7*
_temp842= nsenv; _temp844=* _temp842; _LL848: _temp847= _temp844.f1; goto _LL846;
_LL846: _temp845= _temp844.f2; goto _LL843; _LL843: _temp847->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* f)( struct Cyc_Set_Set*, struct _tagged_arr*,
struct _tuple4*), struct Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)((
int(*)( struct Cyc_Set_Set* set, struct _tagged_arr* x, struct _tuple4* y)) Cyc_Tc_treeshake_remove_f,
_temp845, _temp847->ordinaries);} static struct _tuple7* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple7* _temp849=( struct _tuple7*)
_cycalloc( sizeof( struct _tuple7)); _temp849->f1= ge; _temp849->f2=(( struct
Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)(
Cyc_Std_strptrcmp); _temp849;});} struct Cyc_List_List* Cyc_Tc_treeshake( struct
Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* _temp850=((
struct Cyc_Dict_Dict*(*)( struct _tuple7*(* f)( struct Cyc_Tcenv_Genv*), struct
Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae); struct
Cyc_List_List* _temp851= Cyc_Tc_treeshake_f( _temp850, ds);(( void(*)( void(* f)(
struct Cyc_List_List*, struct _tuple7*), struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)(
Cyc_Tc_treeshake_remove, _temp850); return _temp851;}
