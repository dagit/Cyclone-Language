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
//  extern struct _RegionHandle _new_region();
extern void * _region_malloc(struct _RegionHandle *, unsigned int);
extern void * _region_calloc(struct _RegionHandle *, unsigned int t,
                             unsigned int n);
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
static inline void * _cyccalloc(unsigned int s, unsigned int n) {
  void * ans = (void *)GC_calloc(s,n);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc_atomic(unsigned int s, unsigned int n) {
  void * ans = (void *)GC_calloc_atomic(s,n);
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
#define _new_region(n) _profile_new_region(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ## ":" ## __FUNCTION__,__LINE__)
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
topt; void* r; struct Cyc_Position_Segment* loc; void* annot; } ; static const
int Cyc_Absyn_Skip_s= 0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1;
struct Cyc_Absyn_Seq_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_IfThenElse_s=
3; struct Cyc_Absyn_IfThenElse_s_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt* f3; } ; static const int Cyc_Absyn_While_s=
4; struct _tuple2{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ;
struct Cyc_Absyn_While_s_struct{ int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Break_s= 5; struct Cyc_Absyn_Break_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Continue_s= 6;
struct Cyc_Absyn_Continue_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Goto_s= 7; struct Cyc_Absyn_Goto_s_struct{ int tag;
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_For_s=
8; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt* f4; } ; static const int
Cyc_Absyn_Switch_s= 9; struct Cyc_Absyn_Switch_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_SwitchC_s= 10;
struct Cyc_Absyn_SwitchC_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List*
f2; } ; static const int Cyc_Absyn_Fallthru_s= 11; struct Cyc_Absyn_Fallthru_s_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2; } ;
static const int Cyc_Absyn_Decl_s= 12; struct Cyc_Absyn_Decl_s_struct{ int tag;
struct Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Cut_s=
13; struct Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Splice_s= 14; struct Cyc_Absyn_Splice_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Label_s= 15;
struct Cyc_Absyn_Label_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Do_s= 16; struct Cyc_Absyn_Do_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; struct _tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s=
17; struct Cyc_Absyn_TryCatch_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Region_s= 18; struct
Cyc_Absyn_Region_s_struct{ int tag; struct Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl*
f2; struct Cyc_Absyn_Stmt* f3; } ; struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment*
loc; struct Cyc_List_List* non_local_preds; int try_depth; void* annot; } ;
static const int Cyc_Absyn_Wild_p= 0; static const int Cyc_Absyn_Var_p= 0;
struct Cyc_Absyn_Var_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Null_p= 1; static const int Cyc_Absyn_Int_p= 1; struct Cyc_Absyn_Int_p_struct{
int tag; void* f1; int f2; } ; static const int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{
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
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
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
vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List* atts= vd->attributes;{
void* _temp16=(* vd->name).f1; struct Cyc_List_List* _temp24; struct Cyc_List_List*
_temp26; _LL18: if(( unsigned int) _temp16 >  1u?*(( int*) _temp16) ==  Cyc_Absyn_Rel_n:
0){ _LL25: _temp24=(( struct Cyc_Absyn_Rel_n_struct*) _temp16)->f1; if( _temp24
==  0){ goto _LL19;} else{ goto _LL20;}} else{ goto _LL20;} _LL20: if((
unsigned int) _temp16 >  1u?*(( int*) _temp16) ==  Cyc_Absyn_Abs_n: 0){ _LL27:
_temp26=(( struct Cyc_Absyn_Abs_n_struct*) _temp16)->f1; if( _temp26 ==  0){
goto _LL21;} else{ goto _LL22;}} else{ goto _LL22;} _LL22: goto _LL23; _LL19:
goto _LL17; _LL21: goto _LL17; _LL23:({ struct Cyc_Std_String_pa_struct _temp29;
_temp29.tag= Cyc_Std_String_pa; _temp29.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
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
loc, te, t); if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);} if( sc == ( void*) Cyc_Absyn_Extern? 1: sc == ( void*) Cyc_Absyn_ExternC){
if( vd->initializer !=  0){({ void* _temp73[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("extern declaration should not have initializer", sizeof( unsigned char),
47u), _tag_arr( _temp73, sizeof( void*), 0u));});}} else{ if( ! Cyc_Tcutil_is_function_type(
t)){ for( 0; atts !=  0; atts= atts->tl){ void* _temp74=( void*) atts->hd; _LL76:
if(( unsigned int) _temp74 >  16u?*(( int*) _temp74) ==  Cyc_Absyn_Aligned_att:
0){ goto _LL77;} else{ goto _LL78;} _LL78: if(( unsigned int) _temp74 >  16u?*((
int*) _temp74) ==  Cyc_Absyn_Section_att: 0){ goto _LL79;} else{ goto _LL80;}
_LL80: if( _temp74 == ( void*) Cyc_Absyn_Nocommon_att){ goto _LL81;} else{ goto
_LL82;} _LL82: if( _temp74 == ( void*) Cyc_Absyn_Shared_att){ goto _LL83;} else{
goto _LL84;} _LL84: if( _temp74 == ( void*) Cyc_Absyn_Unused_att){ goto _LL85;}
else{ goto _LL86;} _LL86: if( _temp74 == ( void*) Cyc_Absyn_Weak_att){ goto
_LL87;} else{ goto _LL88;} _LL88: if( _temp74 == ( void*) Cyc_Absyn_Dllimport_att){
goto _LL89;} else{ goto _LL90;} _LL90: if( _temp74 == ( void*) Cyc_Absyn_Dllexport_att){
goto _LL91;} else{ goto _LL92;} _LL92: goto _LL93; _LL77: continue; _LL79:
continue; _LL81: continue; _LL83: continue; _LL85: continue; _LL87: continue;
_LL89: continue; _LL91: continue; _LL93:({ struct Cyc_Std_String_pa_struct
_temp96; _temp96.tag= Cyc_Std_String_pa; _temp96.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ struct Cyc_Std_String_pa_struct _temp95; _temp95.tag= Cyc_Std_String_pa;
_temp95.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*) atts->hd);{
void* _temp94[ 2u]={& _temp95,& _temp96}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s for variable %s",
sizeof( unsigned char), 33u), _tag_arr( _temp94, sizeof( void*), 2u));}}}); goto
_LL75; _LL75:;} if( vd->initializer ==  0){ if( check_var_init? ! Cyc_Tcutil_supports_default(
t): 0){({ struct Cyc_Std_String_pa_struct _temp99; _temp99.tag= Cyc_Std_String_pa;
_temp99.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct
_temp98; _temp98.tag= Cyc_Std_String_pa; _temp98.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp97[ 2u]={& _temp98,& _temp99}; Cyc_Tcutil_terr( loc,
_tag_arr("initializer required for variable %s of type %s", sizeof(
unsigned char), 48u), _tag_arr( _temp97, sizeof( void*), 2u));}}});}} else{
struct Cyc_Absyn_Exp* _temp100=( struct Cyc_Absyn_Exp*) _check_null( vd->initializer);
void* _temp101= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, _temp100); if( !
Cyc_Tcutil_coerce_assign( te, _temp100, t)){({ struct Cyc_Std_String_pa_struct
_temp105; _temp105.tag= Cyc_Std_String_pa; _temp105.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp101);{ struct Cyc_Std_String_pa_struct _temp104; _temp104.tag= Cyc_Std_String_pa;
_temp104.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct
_temp103; _temp103.tag= Cyc_Std_String_pa; _temp103.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
vd->name);{ void* _temp102[ 3u]={& _temp103,& _temp104,& _temp105}; Cyc_Tcutil_terr(
loc, _tag_arr("%s is declared with type \n%s\n but initialized with type \n%s",
sizeof( unsigned char), 60u), _tag_arr( _temp102, sizeof( void*), 3u));}}}});
Cyc_Tcutil_explain_failure();} if( ! Cyc_Tcutil_is_const_exp( te, _temp100)){({
void* _temp106[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("initializer is not a constant expression",
sizeof( unsigned char), 41u), _tag_arr( _temp106, sizeof( void*), 0u));});}}}
else{ for( 0; atts !=  0; atts= atts->tl){ void* _temp107=( void*) atts->hd;
_LL109: if(( unsigned int) _temp107 >  16u?*(( int*) _temp107) ==  Cyc_Absyn_Regparm_att:
0){ goto _LL110;} else{ goto _LL111;} _LL111: if( _temp107 == ( void*) Cyc_Absyn_Stdcall_att){
goto _LL112;} else{ goto _LL113;} _LL113: if( _temp107 == ( void*) Cyc_Absyn_Cdecl_att){
goto _LL114;} else{ goto _LL115;} _LL115: if( _temp107 == ( void*) Cyc_Absyn_Fastcall_att){
goto _LL116;} else{ goto _LL117;} _LL117: if( _temp107 == ( void*) Cyc_Absyn_Noreturn_att){
goto _LL118;} else{ goto _LL119;} _LL119: if(( unsigned int) _temp107 >  16u?*((
int*) _temp107) ==  Cyc_Absyn_Format_att: 0){ goto _LL120;} else{ goto _LL121;}
_LL121: if( _temp107 == ( void*) Cyc_Absyn_Const_att){ goto _LL122;} else{ goto
_LL123;} _LL123: if(( unsigned int) _temp107 >  16u?*(( int*) _temp107) ==  Cyc_Absyn_Aligned_att:
0){ goto _LL124;} else{ goto _LL125;} _LL125: if( _temp107 == ( void*) Cyc_Absyn_Packed_att){
goto _LL126;} else{ goto _LL127;} _LL127: goto _LL128; _LL110: goto _LL112;
_LL112: goto _LL114; _LL114: goto _LL116; _LL116: goto _LL118; _LL118: goto
_LL120; _LL120: goto _LL122; _LL122:({ void* _temp129[ 0u]={};(( int(*)( struct
_tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("tcVardecl: fn type attributes in function var decl",
sizeof( unsigned char), 51u), _tag_arr( _temp129, sizeof( void*), 0u));}); goto
_LL108; _LL124: goto _LL126; _LL126:({ struct Cyc_Std_String_pa_struct _temp131;
_temp131.tag= Cyc_Std_String_pa; _temp131.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string((
void*) atts->hd);{ void* _temp130[ 1u]={& _temp131}; Cyc_Tcutil_terr( loc,
_tag_arr("bad attribute %s in function declaration", sizeof( unsigned char), 41u),
_tag_arr( _temp130, sizeof( void*), 1u));}}); goto _LL108; _LL128: continue;
_LL108:;}}}{ struct _handler_cons _temp132; _push_handler(& _temp132);{ int
_temp134= 0; if( setjmp( _temp132.handler)){ _temp134= 1;} if( ! _temp134){{
struct _tuple4* _temp135=(( struct _tuple4*(*)( struct Cyc_Dict_Dict* d, struct
_tagged_arr* k)) Cyc_Dict_lookup)( ge->ordinaries, v); void* _temp136=(*
_temp135).f1; void* _temp148; _LL138: if(*(( int*) _temp136) ==  Cyc_Tcenv_VarRes){
_LL149: _temp148=( void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp136)->f1; goto
_LL139;} else{ goto _LL140;} _LL140: if(*(( int*) _temp136) ==  Cyc_Tcenv_StructRes){
goto _LL141;} else{ goto _LL142;} _LL142: if(*(( int*) _temp136) ==  Cyc_Tcenv_TunionRes){
goto _LL143;} else{ goto _LL144;} _LL144: if(*(( int*) _temp136) ==  Cyc_Tcenv_AnonEnumRes){
goto _LL145;} else{ goto _LL146;} _LL146: if(*(( int*) _temp136) ==  Cyc_Tcenv_EnumRes){
goto _LL147;} else{ goto _LL137;} _LL139: { struct Cyc_Absyn_Global_b_struct*
_temp150=({ struct Cyc_Absyn_Global_b_struct* _temp155=( struct Cyc_Absyn_Global_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct)); _temp155[ 0]=({ struct
Cyc_Absyn_Global_b_struct _temp156; _temp156.tag= Cyc_Absyn_Global_b; _temp156.f1=
vd; _temp156;}); _temp155;}); void* _temp151= Cyc_Tcdecl_merge_binding( _temp148,(
void*) _temp150, loc, Cyc_Tc_tc_msg); if( _temp151 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL137;} if( _temp151 ==  _temp148?(* _temp135).f2: 0){ goto _LL137;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
_tuple4* v)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple4* _temp152=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp152->f1=( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp153=( struct Cyc_Tcenv_VarRes_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp153[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp154; _temp154.tag= Cyc_Tcenv_VarRes; _temp154.f1=( void*) _temp151;
_temp154;}); _temp153;}); _temp152->f2= 1; _temp152;})); goto _LL137;} _LL141:
goto _LL137; _LL143:({ void* _temp157[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("variable declaration shadows previous [x]tunion constructor",
sizeof( unsigned char), 60u), _tag_arr( _temp157, sizeof( void*), 0u));}); goto
_LL137; _LL145: goto _LL147; _LL147:({ void* _temp158[ 0u]={}; Cyc_Tcutil_warn(
loc, _tag_arr("variable declaration shadows previous enum tag", sizeof(
unsigned char), 47u), _tag_arr( _temp158, sizeof( void*), 0u));}); goto _LL137;
_LL137:;}; _pop_handler();} else{ void* _temp133=( void*) _exn_thrown; void*
_temp160= _temp133; _LL162: if( _temp160 ==  Cyc_Dict_Absent){ goto _LL163;}
else{ goto _LL164;} _LL164: goto _LL165; _LL163: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct _tuple4* v)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple4* _temp166=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp166->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp167=( struct Cyc_Tcenv_VarRes_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp167[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp168; _temp168.tag= Cyc_Tcenv_VarRes;
_temp168.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp169=(
struct Cyc_Absyn_Global_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp169[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp170; _temp170.tag= Cyc_Absyn_Global_b;
_temp170.f1= vd; _temp170;}); _temp169;})); _temp168;}); _temp167;}); _temp166->f2=
0; _temp166;})); goto _LL161; _LL165:( void) _throw( _temp160); _LL161:;}}}}
static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl* fd){ struct
_tagged_arr* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void* _temp171=(* fd->name).f1;
struct Cyc_List_List* _temp179; struct Cyc_List_List* _temp181; _LL173: if((
unsigned int) _temp171 >  1u?*(( int*) _temp171) ==  Cyc_Absyn_Rel_n: 0){ _LL180:
_temp179=(( struct Cyc_Absyn_Rel_n_struct*) _temp171)->f1; if( _temp179 ==  0){
goto _LL174;} else{ goto _LL175;}} else{ goto _LL175;} _LL175: if(( unsigned int)
_temp171 >  1u?*(( int*) _temp171) ==  Cyc_Absyn_Abs_n: 0){ _LL182: _temp181=((
struct Cyc_Absyn_Abs_n_struct*) _temp171)->f1; goto _LL176;} else{ goto _LL177;}
_LL177: goto _LL178; _LL174: goto _LL172; _LL176:( int) _throw(( void*)({ struct
Cyc_Core_Impossible_struct* _temp183=( struct Cyc_Core_Impossible_struct*)
_cycalloc( sizeof( struct Cyc_Core_Impossible_struct)); _temp183[ 0]=({ struct
Cyc_Core_Impossible_struct _temp184; _temp184.tag= Cyc_Core_Impossible; _temp184.f1=
_tag_arr("tc: Abs_n in tcFndecl", sizeof( unsigned char), 22u); _temp184;});
_temp183;})); _LL178:({ struct Cyc_Std_String_pa_struct _temp186; _temp186.tag=
Cyc_Std_String_pa; _temp186.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( fd->name);{
void* _temp185[ 1u]={& _temp186}; Cyc_Tcutil_terr( loc, _tag_arr("qualified function declarations are not implemented (%s)",
sizeof( unsigned char), 57u), _tag_arr( _temp185, sizeof( void*), 1u));}});
return; _LL172:;}(* fd->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp187=( struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp187[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp188; _temp188.tag= Cyc_Absyn_Abs_n;
_temp188.f1= te->ns; _temp188;}); _temp187;}); Cyc_Tcutil_check_fndecl_valid_type(
loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ( fd); fd->attributes= Cyc_Tc_transfer_fn_type_atts(
t, fd->attributes);{ struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 
0; atts= atts->tl){ void* _temp189=( void*) atts->hd; _LL191: if( _temp189 == (
void*) Cyc_Absyn_Packed_att){ goto _LL192;} else{ goto _LL193;} _LL193: if((
unsigned int) _temp189 >  16u?*(( int*) _temp189) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL194;} else{ goto _LL195;} _LL195: goto _LL196; _LL192: goto _LL194;
_LL194:({ struct Cyc_Std_String_pa_struct _temp198; _temp198.tag= Cyc_Std_String_pa;
_temp198.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*) atts->hd);{
void* _temp197[ 1u]={& _temp198}; Cyc_Tcutil_terr( loc, _tag_arr("bad attribute %s for function",
sizeof( unsigned char), 30u), _tag_arr( _temp197, sizeof( void*), 1u));}}); goto
_LL190; _LL196: goto _LL190; _LL190:;}}{ struct _handler_cons _temp199;
_push_handler(& _temp199);{ int _temp201= 0; if( setjmp( _temp199.handler)){
_temp201= 1;} if( ! _temp201){{ struct _tuple4* _temp202=(( struct _tuple4*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)( ge->ordinaries,
v); void* _temp203=(* _temp202).f1; void* _temp215; _LL205: if(*(( int*)
_temp203) ==  Cyc_Tcenv_VarRes){ _LL216: _temp215=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp203)->f1; goto _LL206;} else{ goto _LL207;} _LL207: if(*(( int*) _temp203)
==  Cyc_Tcenv_StructRes){ goto _LL208;} else{ goto _LL209;} _LL209: if(*(( int*)
_temp203) ==  Cyc_Tcenv_TunionRes){ goto _LL210;} else{ goto _LL211;} _LL211:
if(*(( int*) _temp203) ==  Cyc_Tcenv_AnonEnumRes){ goto _LL212;} else{ goto
_LL213;} _LL213: if(*(( int*) _temp203) ==  Cyc_Tcenv_EnumRes){ goto _LL214;}
else{ goto _LL204;} _LL206: { struct Cyc_Absyn_Funname_b_struct* _temp217=({
struct Cyc_Absyn_Funname_b_struct* _temp222=( struct Cyc_Absyn_Funname_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Funname_b_struct)); _temp222[ 0]=({ struct
Cyc_Absyn_Funname_b_struct _temp223; _temp223.tag= Cyc_Absyn_Funname_b; _temp223.f1=
fd; _temp223;}); _temp222;}); void* _temp218= Cyc_Tcdecl_merge_binding( _temp215,(
void*) _temp217, loc, Cyc_Tc_tc_msg); if( _temp218 == ( void*) Cyc_Absyn_Unresolved_b){
goto _LL204;} if( _temp218 ==  _temp215?(* _temp202).f2: 0){ goto _LL204;} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
_tuple4* v)) Cyc_Dict_insert)( ge->ordinaries, v,({ struct _tuple4* _temp219=(
struct _tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp219->f1=( void*)({
struct Cyc_Tcenv_VarRes_struct* _temp220=( struct Cyc_Tcenv_VarRes_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_VarRes_struct)); _temp220[ 0]=({ struct Cyc_Tcenv_VarRes_struct
_temp221; _temp221.tag= Cyc_Tcenv_VarRes; _temp221.f1=( void*) _temp218;
_temp221;}); _temp220;}); _temp219->f2= 1; _temp219;})); goto _LL204;} _LL208:
goto _LL204; _LL210:({ void* _temp224[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("function declaration shadows previous [x]tunion constructor",
sizeof( unsigned char), 60u), _tag_arr( _temp224, sizeof( void*), 0u));}); goto
_LL204; _LL212: goto _LL214; _LL214:({ void* _temp225[ 0u]={}; Cyc_Tcutil_warn(
loc, _tag_arr("function declaration shadows previous enum tag", sizeof(
unsigned char), 47u), _tag_arr( _temp225, sizeof( void*), 0u));}); goto _LL204;
_LL204:;}; _pop_handler();} else{ void* _temp200=( void*) _exn_thrown; void*
_temp227= _temp200; _LL229: if( _temp227 ==  Cyc_Dict_Absent){ goto _LL230;}
else{ goto _LL231;} _LL231: goto _LL232; _LL230: ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct _tuple4* v)) Cyc_Dict_insert)(
ge->ordinaries, v,({ struct _tuple4* _temp233=( struct _tuple4*) _cycalloc(
sizeof( struct _tuple4)); _temp233->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct*
_temp234=( struct Cyc_Tcenv_VarRes_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp234[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp235; _temp235.tag= Cyc_Tcenv_VarRes;
_temp235.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp236=(
struct Cyc_Absyn_Funname_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp236[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp237; _temp237.tag= Cyc_Absyn_Funname_b;
_temp237.f1= fd; _temp237;}); _temp236;})); _temp235;}); _temp234;}); _temp233->f2=
0; _temp233;})); goto _LL228; _LL232:( void) _throw( _temp227); _LL228:;}}} te->le=({
struct Cyc_Core_Opt* _temp238=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp238->v=( void*)(( void*)({ struct Cyc_Tcenv_Outermost_struct*
_temp239=( struct Cyc_Tcenv_Outermost_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_Outermost_struct));
_temp239[ 0]=({ struct Cyc_Tcenv_Outermost_struct _temp240; _temp240.tag= Cyc_Tcenv_Outermost;
_temp240.f1=( void*) Cyc_Tcenv_new_fenv( loc, fd); _temp240;}); _temp239;}));
_temp238;}); Cyc_Tcstmt_tcStmt( te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved(
te)){({ void* _temp241[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("function has goto statements to undefined labels",
sizeof( unsigned char), 49u), _tag_arr( _temp241, sizeof( void*), 0u));});} te->le=
0;}} static void Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_arr* v=(* td->name).f2;{ void* _temp242=(* td->name).f1; struct Cyc_List_List*
_temp250; struct Cyc_List_List* _temp252; _LL244: if(( unsigned int) _temp242 > 
1u?*(( int*) _temp242) ==  Cyc_Absyn_Rel_n: 0){ _LL251: _temp250=(( struct Cyc_Absyn_Rel_n_struct*)
_temp242)->f1; if( _temp250 ==  0){ goto _LL245;} else{ goto _LL246;}} else{
goto _LL246;} _LL246: if(( unsigned int) _temp242 >  1u?*(( int*) _temp242) == 
Cyc_Absyn_Abs_n: 0){ _LL253: _temp252=(( struct Cyc_Absyn_Abs_n_struct*)
_temp242)->f1; if( _temp252 ==  0){ goto _LL247;} else{ goto _LL248;}} else{
goto _LL248;} _LL248: goto _LL249; _LL245: goto _LL243; _LL247: goto _LL243;
_LL249:({ struct Cyc_Std_String_pa_struct _temp255; _temp255.tag= Cyc_Std_String_pa;
_temp255.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( td->name);{ void*
_temp254[ 1u]={& _temp255}; Cyc_Tcutil_terr( loc, _tag_arr("qualified typedef declarations are not implemented (%s)",
sizeof( unsigned char), 56u), _tag_arr( _temp254, sizeof( void*), 1u));}});
return; _LL243:;} if((( int(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k))
Cyc_Dict_member)( ge->typedefs, v)){({ struct Cyc_Std_String_pa_struct _temp257;
_temp257.tag= Cyc_Std_String_pa; _temp257.f1=( struct _tagged_arr)* v;{ void*
_temp256[ 1u]={& _temp257}; Cyc_Tcutil_terr( loc, _tag_arr("redeclaration of typedef %s",
sizeof( unsigned char), 28u), _tag_arr( _temp256, sizeof( void*), 1u));}});
return;}(* td->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp258=(
struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp258[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp259; _temp259.tag= Cyc_Absyn_Abs_n;
_temp259.f1= te->ns; _temp259;}); _temp258;}); Cyc_Tcutil_check_unique_tvars(
loc, td->tvs); Cyc_Tcutil_add_tvar_identities( td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind, 0,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Typedefdecl* v)) Cyc_Dict_insert)( ge->typedefs, v, td);} static void
Cyc_Tc_tcStructFields( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct _tagged_arr obj, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs){ struct _RegionHandle _temp260= _new_region("uprev_rgn");
struct _RegionHandle* uprev_rgn=& _temp260; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0; struct Cyc_List_List* _temp261= fields; for( 0;
_temp261 !=  0; _temp261= _temp261->tl){ struct Cyc_Absyn_Structfield _temp264;
struct Cyc_List_List* _temp265; struct Cyc_Absyn_Exp* _temp267; void* _temp269;
struct Cyc_Absyn_Tqual _temp271; struct _tagged_arr* _temp273; struct Cyc_Absyn_Structfield*
_temp262=( struct Cyc_Absyn_Structfield*) _temp261->hd; _temp264=* _temp262;
_LL274: _temp273= _temp264.name; goto _LL272; _LL272: _temp271= _temp264.tq;
goto _LL270; _LL270: _temp269=( void*) _temp264.type; goto _LL268; _LL268:
_temp267= _temp264.width; goto _LL266; _LL266: _temp265= _temp264.attributes;
goto _LL263; _LL263: if((( int(*)( int(* compare)( struct _tagged_arr*, struct
_tagged_arr*), struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)(
Cyc_Std_zstrptrcmp, prev_fields, _temp273)){({ struct Cyc_Std_String_pa_struct
_temp277; _temp277.tag= Cyc_Std_String_pa; _temp277.f1=( struct _tagged_arr) obj;{
struct Cyc_Std_String_pa_struct _temp276; _temp276.tag= Cyc_Std_String_pa;
_temp276.f1=( struct _tagged_arr)* _temp273;{ void* _temp275[ 2u]={& _temp276,&
_temp277}; Cyc_Tcutil_terr( loc, _tag_arr("duplicate field %s in %s", sizeof(
unsigned char), 25u), _tag_arr( _temp275, sizeof( void*), 2u));}}});} if( Cyc_Std_strcmp(*
_temp273, _tag_arr("", sizeof( unsigned char), 1u)) !=  0){ prev_fields=({
struct Cyc_List_List* _temp278=( struct Cyc_List_List*) _region_malloc(
uprev_rgn, sizeof( struct Cyc_List_List)); _temp278->hd=( void*) _temp273;
_temp278->tl= prev_fields; _temp278;});} Cyc_Tcutil_check_type( loc, te, tvs,(
void*) Cyc_Absyn_MemKind, 0, _temp269); Cyc_Tcutil_check_bitfield( loc, te,
_temp269, _temp267, _temp273);}}; _pop_region( uprev_rgn);} static void Cyc_Tc_tcStructOrUniondecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct _tagged_arr obj,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_arr* _temp279=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
sd->name))->v)).f2; if( sd->name ==  0){({ struct Cyc_Std_String_pa_struct
_temp281; _temp281.tag= Cyc_Std_String_pa; _temp281.f1=( struct _tagged_arr) obj;{
void* _temp280[ 1u]={& _temp281}; Cyc_Tcutil_terr( loc, _tag_arr("anonymous %ss are not allowed at top level",
sizeof( unsigned char), 43u), _tag_arr( _temp280, sizeof( void*), 1u));}});
return;}{ struct Cyc_List_List* atts= sd->attributes; for( 0; atts !=  0; atts=
atts->tl){ void* _temp282=( void*) atts->hd; _LL284: if( _temp282 == ( void*)
Cyc_Absyn_Packed_att){ goto _LL285;} else{ goto _LL286;} _LL286: if((
unsigned int) _temp282 >  16u?*(( int*) _temp282) ==  Cyc_Absyn_Aligned_att: 0){
goto _LL287;} else{ goto _LL288;} _LL288: goto _LL289; _LL285: continue; _LL287:
continue; _LL289:({ struct Cyc_Std_String_pa_struct _temp293; _temp293.tag= Cyc_Std_String_pa;
_temp293.f1=( struct _tagged_arr)* _temp279;{ struct Cyc_Std_String_pa_struct
_temp292; _temp292.tag= Cyc_Std_String_pa; _temp292.f1=( struct _tagged_arr) obj;{
struct Cyc_Std_String_pa_struct _temp291; _temp291.tag= Cyc_Std_String_pa;
_temp291.f1=( struct _tagged_arr) Cyc_Absyn_attribute2string(( void*) atts->hd);{
void* _temp290[ 3u]={& _temp291,& _temp292,& _temp293}; Cyc_Tcutil_terr( loc,
_tag_arr("bad attribute %s in %s %s definition", sizeof( unsigned char), 37u),
_tag_arr( _temp290, sizeof( void*), 3u));}}}}); goto _LL283; _LL283:;}}{ struct
Cyc_List_List* tvs= sd->tvs; for( 0; tvs !=  0; tvs= tvs->tl){ void* _temp294=
Cyc_Absyn_compress_kb(( void*)(( struct Cyc_Absyn_Tvar*) tvs->hd)->kind); struct
Cyc_Core_Opt* _temp306; struct Cyc_Core_Opt** _temp308; void* _temp309; struct
Cyc_Core_Opt* _temp311; struct Cyc_Core_Opt** _temp313; void* _temp314; struct
Cyc_Core_Opt* _temp316; struct Cyc_Core_Opt** _temp318; void* _temp319; _LL296:
if(*(( int*) _temp294) ==  Cyc_Absyn_Unknown_kb){ _LL307: _temp306=(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp294)->f1; _temp308=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp294)->f1; goto _LL297;} else{ goto _LL298;} _LL298: if(*(( int*) _temp294)
==  Cyc_Absyn_Less_kb){ _LL312: _temp311=(( struct Cyc_Absyn_Less_kb_struct*)
_temp294)->f1; _temp313=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp294)->f1; goto _LL310; _LL310: _temp309=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp294)->f2; if( _temp309 == ( void*) Cyc_Absyn_MemKind){ goto _LL299;} else{
goto _LL300;}} else{ goto _LL300;} _LL300: if(*(( int*) _temp294) ==  Cyc_Absyn_Less_kb){
_LL317: _temp316=(( struct Cyc_Absyn_Less_kb_struct*) _temp294)->f1; _temp318=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*) _temp294)->f1; goto
_LL315; _LL315: _temp314=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp294)->f2;
if( _temp314 == ( void*) Cyc_Absyn_AnyKind){ goto _LL301;} else{ goto _LL302;}}
else{ goto _LL302;} _LL302: if(*(( int*) _temp294) ==  Cyc_Absyn_Eq_kb){ _LL320:
_temp319=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp294)->f1; if( _temp319
== ( void*) Cyc_Absyn_MemKind){ goto _LL303;} else{ goto _LL304;}} else{ goto
_LL304;} _LL304: goto _LL305; _LL297: _temp313= _temp308; goto _LL299; _LL299:
_temp318= _temp313; goto _LL301; _LL301:* _temp318=({ struct Cyc_Core_Opt*
_temp321=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp321->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct* _temp322=( struct
Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp322[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp323; _temp323.tag= Cyc_Absyn_Eq_kb;
_temp323.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp323;}); _temp322;}));
_temp321;}); continue; _LL303:({ struct Cyc_Std_String_pa_struct _temp327;
_temp327.tag= Cyc_Std_String_pa; _temp327.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)
tvs->hd)->name;{ struct Cyc_Std_String_pa_struct _temp326; _temp326.tag= Cyc_Std_String_pa;
_temp326.f1=( struct _tagged_arr)* _temp279;{ struct Cyc_Std_String_pa_struct
_temp325; _temp325.tag= Cyc_Std_String_pa; _temp325.f1=( struct _tagged_arr) obj;{
void* _temp324[ 3u]={& _temp325,& _temp326,& _temp327}; Cyc_Tcutil_terr( loc,
_tag_arr("%s %s attempts to abstract type variable %s of kind M", sizeof(
unsigned char), 54u), _tag_arr( _temp324, sizeof( void*), 3u));}}}}); continue;
_LL305: continue; _LL295:;}}{ void* _temp328=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*)
_check_null( sd->name))->v)).f1; struct Cyc_List_List* _temp336; struct Cyc_List_List*
_temp338; _LL330: if(( unsigned int) _temp328 >  1u?*(( int*) _temp328) ==  Cyc_Absyn_Rel_n:
0){ _LL337: _temp336=(( struct Cyc_Absyn_Rel_n_struct*) _temp328)->f1; if(
_temp336 ==  0){ goto _LL331;} else{ goto _LL332;}} else{ goto _LL332;} _LL332:
if(( unsigned int) _temp328 >  1u?*(( int*) _temp328) ==  Cyc_Absyn_Abs_n: 0){
_LL339: _temp338=(( struct Cyc_Absyn_Abs_n_struct*) _temp328)->f1; if( _temp338
==  0){ goto _LL333;} else{ goto _LL334;}} else{ goto _LL334;} _LL334: goto
_LL335; _LL331: goto _LL329; _LL333: goto _LL329; _LL335:({ struct Cyc_Std_String_pa_struct
_temp341; _temp341.tag= Cyc_Std_String_pa; _temp341.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string((
struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v);{ void*
_temp340[ 1u]={& _temp341}; Cyc_Tcutil_terr( loc, _tag_arr("qualified struct declarations are not implemented (%s)",
sizeof( unsigned char), 55u), _tag_arr( _temp340, sizeof( void*), 1u));}});
return; _LL329:;}(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v)).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp342=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp342[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp343; _temp343.tag= Cyc_Absyn_Abs_n; _temp343.f1= te->ns; _temp343;});
_temp342;}); Cyc_Tcutil_check_unique_tvars( loc, sd->tvs); Cyc_Tcutil_add_tvar_identities(
sd->tvs);} struct _tuple5{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ;
void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ struct
_tagged_arr* _temp344=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null(
sd->name))->v)).f2; const unsigned char* _temp345="struct"; Cyc_Tc_tcStructOrUniondecl(
te, ge, _tag_arr( _temp345, sizeof( unsigned char), 7u), loc, sd);{ struct Cyc_List_List*
_temp346= sd->tvs;{ struct _tuple5 _temp348=({ struct _tuple5 _temp347; _temp347.f1=
sd->fields; _temp347.f2=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k)) Cyc_Dict_lookup_opt)( ge->structdecls, _temp344);
_temp347;}); struct Cyc_Core_Opt* _temp358; struct Cyc_Core_Opt* _temp360;
struct Cyc_Core_Opt* _temp362; struct Cyc_Core_Opt* _temp364; struct Cyc_Core_Opt
_temp366; struct Cyc_List_List* _temp367; struct Cyc_Core_Opt* _temp369; struct
Cyc_Core_Opt _temp371; struct Cyc_Absyn_Structdecl** _temp372; struct Cyc_Core_Opt*
_temp374; struct Cyc_Core_Opt _temp376; struct Cyc_List_List* _temp377; struct
Cyc_Core_Opt* _temp379; struct Cyc_Core_Opt _temp381; struct Cyc_Absyn_Structdecl**
_temp382; struct Cyc_Core_Opt* _temp384; _LL350: _LL361: _temp360= _temp348.f1;
if( _temp360 ==  0){ goto _LL359;} else{ goto _LL352;} _LL359: _temp358=
_temp348.f2; if( _temp358 ==  0){ goto _LL351;} else{ goto _LL352;} _LL352:
_LL365: _temp364= _temp348.f1; if( _temp364 ==  0){ goto _LL354;} else{ _temp366=*
_temp364; _LL368: _temp367=( struct Cyc_List_List*) _temp366.v; goto _LL363;}
_LL363: _temp362= _temp348.f2; if( _temp362 ==  0){ goto _LL353;} else{ goto
_LL354;} _LL354: _LL375: _temp374= _temp348.f1; if( _temp374 ==  0){ goto _LL356;}
else{ _temp376=* _temp374; _LL378: _temp377=( struct Cyc_List_List*) _temp376.v;
goto _LL370;} _LL370: _temp369= _temp348.f2; if( _temp369 ==  0){ goto _LL356;}
else{ _temp371=* _temp369; _LL373: _temp372=( struct Cyc_Absyn_Structdecl**)
_temp371.v; goto _LL355;} _LL356: _LL385: _temp384= _temp348.f1; if( _temp384 == 
0){ goto _LL380;} else{ goto _LL349;} _LL380: _temp379= _temp348.f2; if(
_temp379 ==  0){ goto _LL349;} else{ _temp381=* _temp379; _LL383: _temp382=(
struct Cyc_Absyn_Structdecl**) _temp381.v; goto _LL357;} _LL351: ge->structdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Structdecl** v)) Cyc_Dict_insert)( ge->structdecls, _temp344,({ struct
Cyc_Absyn_Structdecl** _temp386=( struct Cyc_Absyn_Structdecl**) _cycalloc(
sizeof( struct Cyc_Absyn_Structdecl*)); _temp386[ 0]= sd; _temp386;})); goto
_LL349; _LL353: { struct Cyc_Absyn_Structdecl** _temp387=({ struct Cyc_Absyn_Structdecl**
_temp388=( struct Cyc_Absyn_Structdecl**) _cycalloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp388[ 0]=({ struct Cyc_Absyn_Structdecl* _temp389=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp389->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp389->name= sd->name; _temp389->tvs= _temp346; _temp389->fields=
0; _temp389->attributes= 0; _temp389;}); _temp388;}); ge->structdecls=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Structdecl**
v)) Cyc_Dict_insert)( ge->structdecls, _temp344, _temp387); Cyc_Tc_tcStructFields(
te, ge, loc, _tag_arr( _temp345, sizeof( unsigned char), 7u), _temp367, _temp346);*
_temp387= sd; goto _LL349;} _LL355: { struct Cyc_Absyn_Structdecl* _temp390=*
_temp372;* _temp372=({ struct Cyc_Absyn_Structdecl* _temp391=( struct Cyc_Absyn_Structdecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp391->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp391->name= sd->name; _temp391->tvs= _temp346; _temp391->fields=
0; _temp391->attributes= 0; _temp391;}); Cyc_Tc_tcStructFields( te, ge, loc,
_tag_arr( _temp345, sizeof( unsigned char), 7u), _temp377, _temp346);* _temp372=
_temp390; _temp382= _temp372; goto _LL357;} _LL357: { struct Cyc_Absyn_Structdecl*
_temp392= Cyc_Tcdecl_merge_structdecl(* _temp382, sd, loc, Cyc_Tc_tc_msg); if(
_temp392 ==  0){ return;} else{* _temp382=( struct Cyc_Absyn_Structdecl*)
_check_null( _temp392); sd=( struct Cyc_Absyn_Structdecl*) _check_null( _temp392);
goto _LL349;}} _LL349:;} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_arr* k, struct _tuple4* v)) Cyc_Dict_insert)( ge->ordinaries,
_temp344,({ struct _tuple4* _temp393=( struct _tuple4*) _cycalloc( sizeof(
struct _tuple4)); _temp393->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct*
_temp394=( struct Cyc_Tcenv_StructRes_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp394[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp395; _temp395.tag= Cyc_Tcenv_StructRes;
_temp395.f1= sd; _temp395;}); _temp394;}); _temp393->f2= 1; _temp393;}));}} void
Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct
Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ struct _tagged_arr*
_temp396=(*(( struct _tuple0*)(( struct Cyc_Core_Opt*) _check_null( ud->name))->v)).f2;
const unsigned char* _temp397="union"; Cyc_Tc_tcStructOrUniondecl( te, ge,
_tag_arr( _temp397, sizeof( unsigned char), 6u), loc,( struct Cyc_Absyn_Structdecl*)
ud);{ struct Cyc_List_List* _temp398= ud->tvs; struct _tuple5 _temp400=({ struct
_tuple5 _temp399; _temp399.f1= ud->fields; _temp399.f2=(( struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup_opt)( ge->uniondecls,
_temp396); _temp399;}); struct Cyc_Core_Opt* _temp410; struct Cyc_Core_Opt*
_temp412; struct Cyc_Core_Opt* _temp414; struct Cyc_Core_Opt* _temp416; struct
Cyc_Core_Opt _temp418; struct Cyc_List_List* _temp419; struct Cyc_Core_Opt*
_temp421; struct Cyc_Core_Opt _temp423; struct Cyc_Absyn_Uniondecl** _temp424;
struct Cyc_Core_Opt* _temp426; struct Cyc_Core_Opt _temp428; struct Cyc_List_List*
_temp429; struct Cyc_Core_Opt* _temp431; struct Cyc_Core_Opt _temp433; struct
Cyc_Absyn_Uniondecl** _temp434; struct Cyc_Core_Opt* _temp436; _LL402: _LL413:
_temp412= _temp400.f1; if( _temp412 ==  0){ goto _LL411;} else{ goto _LL404;}
_LL411: _temp410= _temp400.f2; if( _temp410 ==  0){ goto _LL403;} else{ goto
_LL404;} _LL404: _LL417: _temp416= _temp400.f1; if( _temp416 ==  0){ goto _LL406;}
else{ _temp418=* _temp416; _LL420: _temp419=( struct Cyc_List_List*) _temp418.v;
goto _LL415;} _LL415: _temp414= _temp400.f2; if( _temp414 ==  0){ goto _LL405;}
else{ goto _LL406;} _LL406: _LL427: _temp426= _temp400.f1; if( _temp426 ==  0){
goto _LL408;} else{ _temp428=* _temp426; _LL430: _temp429=( struct Cyc_List_List*)
_temp428.v; goto _LL422;} _LL422: _temp421= _temp400.f2; if( _temp421 ==  0){
goto _LL408;} else{ _temp423=* _temp421; _LL425: _temp424=( struct Cyc_Absyn_Uniondecl**)
_temp423.v; goto _LL407;} _LL408: _LL437: _temp436= _temp400.f1; if( _temp436 == 
0){ goto _LL432;} else{ goto _LL401;} _LL432: _temp431= _temp400.f2; if(
_temp431 ==  0){ goto _LL401;} else{ _temp433=* _temp431; _LL435: _temp434=(
struct Cyc_Absyn_Uniondecl**) _temp433.v; goto _LL409;} _LL403: ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Uniondecl** v)) Cyc_Dict_insert)( ge->uniondecls, _temp396,({ struct
Cyc_Absyn_Uniondecl** _temp438=( struct Cyc_Absyn_Uniondecl**) _cycalloc(
sizeof( struct Cyc_Absyn_Uniondecl*)); _temp438[ 0]= ud; _temp438;})); goto
_LL401; _LL405: { struct Cyc_Absyn_Uniondecl** _temp439=({ struct Cyc_Absyn_Uniondecl**
_temp444=( struct Cyc_Absyn_Uniondecl**) _cycalloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp444[ 0]=({ struct Cyc_Absyn_Uniondecl* _temp445=( struct Cyc_Absyn_Uniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp445->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp445->name= ud->name; _temp445->tvs= _temp398; _temp445->fields=
0; _temp445->attributes= ud->attributes; _temp445;}); _temp444;}); ge->uniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Uniondecl** v)) Cyc_Dict_insert)( ge->uniondecls, _temp396, _temp439);
Cyc_Tc_tcStructFields( te, ge, loc, _tag_arr( _temp397, sizeof( unsigned char),
6u), _temp419, _temp398);{ struct Cyc_List_List* f= _temp419; for( 0; f !=  0; f=
f->tl){ if( ! Cyc_Tcutil_bits_only(( void*)(( struct Cyc_Absyn_Structfield*) f->hd)->type)){({
struct Cyc_Std_String_pa_struct _temp443; _temp443.tag= Cyc_Std_String_pa;
_temp443.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Absyn_Structfield*)
f->hd)->type);{ struct Cyc_Std_String_pa_struct _temp442; _temp442.tag= Cyc_Std_String_pa;
_temp442.f1=( struct _tagged_arr)* _temp396;{ struct Cyc_Std_String_pa_struct
_temp441; _temp441.tag= Cyc_Std_String_pa; _temp441.f1=( struct _tagged_arr)*((
struct Cyc_Absyn_Structfield*) f->hd)->name;{ void* _temp440[ 3u]={& _temp441,&
_temp442,& _temp443}; Cyc_Tcutil_terr( loc, _tag_arr("field %s of union %s has type %s which is not `bits-only'",
sizeof( unsigned char), 58u), _tag_arr( _temp440, sizeof( void*), 3u));}}}});}}}*
_temp439= ud; goto _LL401;} _LL407: { struct Cyc_Absyn_Uniondecl* _temp446=*
_temp424;* _temp424=({ struct Cyc_Absyn_Uniondecl* _temp447=( struct Cyc_Absyn_Uniondecl*)
_cycalloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp447->sc=( void*)(( void*)
Cyc_Absyn_Extern); _temp447->name= ud->name; _temp447->tvs= _temp398; _temp447->fields=
0; _temp447->attributes= ud->attributes; _temp447;}); Cyc_Tc_tcStructFields( te,
ge, loc, _tag_arr( _temp397, sizeof( unsigned char), 6u), _temp429, _temp398);*
_temp424= _temp446; _temp434= _temp424; goto _LL409;} _LL409: { struct Cyc_Absyn_Uniondecl*
_temp448= Cyc_Tcdecl_merge_uniondecl(* _temp434, ud, loc, Cyc_Tc_tc_msg); if(
_temp448 ==  0){ return;} else{* _temp434=( struct Cyc_Absyn_Uniondecl*)
_check_null( _temp448); goto _LL401;}} _LL401:;}} struct _tuple6{ struct Cyc_Absyn_Tqual
f1; void* f2; } ; static struct Cyc_List_List* Cyc_Tc_tcTunionFields( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct
_tagged_arr obj, int is_xtunion, struct _tuple0* name, struct Cyc_List_List*
fields, struct Cyc_List_List* tvs, struct Cyc_Absyn_Tuniondecl* tudres){{ struct
Cyc_List_List* _temp449= fields; for( 0; _temp449 !=  0; _temp449= _temp449->tl){
struct Cyc_Absyn_Tunionfield* _temp450=( struct Cyc_Absyn_Tunionfield*) _temp449->hd;{
struct Cyc_List_List* tvs= _temp450->tvs; for( 0; tvs !=  0; tvs= tvs->tl){
struct Cyc_Absyn_Tvar* _temp451=( struct Cyc_Absyn_Tvar*) tvs->hd; void*
_temp452= Cyc_Absyn_compress_kb(( void*) _temp451->kind); void* _temp464; struct
Cyc_Core_Opt* _temp466; struct Cyc_Core_Opt** _temp468; void* _temp469; struct
Cyc_Core_Opt* _temp471; struct Cyc_Core_Opt** _temp473; void* _temp474; struct
Cyc_Core_Opt* _temp476; struct Cyc_Core_Opt** _temp478; _LL454: if(*(( int*)
_temp452) ==  Cyc_Absyn_Eq_kb){ _LL465: _temp464=( void*)(( struct Cyc_Absyn_Eq_kb_struct*)
_temp452)->f1; if( _temp464 == ( void*) Cyc_Absyn_MemKind){ goto _LL455;} else{
goto _LL456;}} else{ goto _LL456;} _LL456: if(*(( int*) _temp452) ==  Cyc_Absyn_Unknown_kb){
_LL467: _temp466=(( struct Cyc_Absyn_Unknown_kb_struct*) _temp452)->f1; _temp468=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*) _temp452)->f1;
goto _LL457;} else{ goto _LL458;} _LL458: if(*(( int*) _temp452) ==  Cyc_Absyn_Less_kb){
_LL472: _temp471=(( struct Cyc_Absyn_Less_kb_struct*) _temp452)->f1; _temp473=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*) _temp452)->f1; goto
_LL470; _LL470: _temp469=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp452)->f2;
if( _temp469 == ( void*) Cyc_Absyn_MemKind){ goto _LL459;} else{ goto _LL460;}}
else{ goto _LL460;} _LL460: if(*(( int*) _temp452) ==  Cyc_Absyn_Less_kb){
_LL477: _temp476=(( struct Cyc_Absyn_Less_kb_struct*) _temp452)->f1; _temp478=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*) _temp452)->f1; goto
_LL475; _LL475: _temp474=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp452)->f2;
if( _temp474 == ( void*) Cyc_Absyn_AnyKind){ goto _LL461;} else{ goto _LL462;}}
else{ goto _LL462;} _LL462: goto _LL463; _LL455:({ struct Cyc_Std_String_pa_struct
_temp480; _temp480.tag= Cyc_Std_String_pa; _temp480.f1=( struct _tagged_arr)*(*
_temp450->name).f2;{ void* _temp479[ 1u]={& _temp480}; Cyc_Tcutil_terr( _temp450->loc,
_tag_arr("field %s abstracts type variable of kind M", sizeof( unsigned char),
43u), _tag_arr( _temp479, sizeof( void*), 1u));}}); goto _LL453; _LL457:
_temp473= _temp468; goto _LL459; _LL459: _temp478= _temp473; goto _LL461; _LL461:*
_temp478=({ struct Cyc_Core_Opt* _temp481=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp481->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct*
_temp482=( struct Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp482[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp483; _temp483.tag= Cyc_Absyn_Eq_kb;
_temp483.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp483;}); _temp482;}));
_temp481;}); goto _LL453; _LL463: goto _LL453; _LL453:;}}{ struct Cyc_List_List*
alltvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tvs, _temp450->tvs); Cyc_Tcutil_check_unique_tvars( loc,
alltvs); Cyc_Tcutil_add_tvar_identities( _temp450->tvs);{ struct Cyc_List_List*
typs= _temp450->typs; for( 0; typs !=  0; typs= typs->tl){ Cyc_Tcutil_check_type(
_temp450->loc, te, alltvs,( void*) Cyc_Absyn_MemKind, 0,(*(( struct _tuple6*)
typs->hd)).f2);}}{ void* _temp484=(* _temp450->name).f1; struct Cyc_List_List*
_temp494; _LL486: if(( unsigned int) _temp484 >  1u?*(( int*) _temp484) ==  Cyc_Absyn_Rel_n:
0){ _LL495: _temp494=(( struct Cyc_Absyn_Rel_n_struct*) _temp484)->f1; if(
_temp494 ==  0){ goto _LL487;} else{ goto _LL488;}} else{ goto _LL488;} _LL488:
if(( unsigned int) _temp484 >  1u?*(( int*) _temp484) ==  Cyc_Absyn_Rel_n: 0){
goto _LL489;} else{ goto _LL490;} _LL490: if(( unsigned int) _temp484 >  1u?*((
int*) _temp484) ==  Cyc_Absyn_Abs_n: 0){ goto _LL491;} else{ goto _LL492;}
_LL492: if( _temp484 == ( void*) Cyc_Absyn_Loc_n){ goto _LL493;} else{ goto
_LL485;} _LL487: if( is_xtunion){(* _temp450->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp496=( struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp496[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp497; _temp497.tag= Cyc_Absyn_Abs_n;
_temp497.f1= te->ns; _temp497;}); _temp496;});} else{(* _temp450->name).f1=(*
name).f1;} goto _LL485; _LL489:({ struct Cyc_Std_String_pa_struct _temp499;
_temp499.tag= Cyc_Std_String_pa; _temp499.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp450->name);{ void* _temp498[ 1u]={& _temp499}; Cyc_Tcutil_terr( _temp450->loc,
_tag_arr("qualified tunionfield declarations are not allowed (%s)", sizeof(
unsigned char), 56u), _tag_arr( _temp498, sizeof( void*), 1u));}}); goto _LL485;
_LL491: goto _LL485; _LL493:( int) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp500=( struct Cyc_Core_Impossible_struct*) _cycalloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp500[ 0]=({ struct Cyc_Core_Impossible_struct _temp501; _temp501.tag= Cyc_Core_Impossible;
_temp501.f1= _tag_arr("tcTunionFields: Loc_n", sizeof( unsigned char), 22u);
_temp501;}); _temp500;})); goto _LL485; _LL485:;}}}}{ struct Cyc_List_List*
fields2; if( is_xtunion){ int _temp502= 1; struct Cyc_List_List* _temp503= Cyc_Tcdecl_sort_xtunion_fields(
fields,& _temp502,(* name).f2, loc, Cyc_Tc_tc_msg); if( _temp502){ fields2=
_temp503;} else{ fields2= 0;}} else{ struct _RegionHandle _temp504= _new_region("uprev_rgn");
struct _RegionHandle* uprev_rgn=& _temp504; _push_region( uprev_rgn);{ struct
Cyc_List_List* prev_fields= 0;{ struct Cyc_List_List* fs= fields; for( 0; fs != 
0; fs= fs->tl){ struct Cyc_Absyn_Tunionfield* _temp505=( struct Cyc_Absyn_Tunionfield*)
fs->hd; if((( int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*),
struct Cyc_List_List* l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp,
prev_fields,(* _temp505->name).f2)){({ struct Cyc_Std_String_pa_struct _temp508;
_temp508.tag= Cyc_Std_String_pa; _temp508.f1=( struct _tagged_arr) obj;{ struct
Cyc_Std_String_pa_struct _temp507; _temp507.tag= Cyc_Std_String_pa; _temp507.f1=(
struct _tagged_arr)*(* _temp505->name).f2;{ void* _temp506[ 2u]={& _temp507,&
_temp508}; Cyc_Tcutil_terr( _temp505->loc, _tag_arr("duplicate field name %s in %s",
sizeof( unsigned char), 30u), _tag_arr( _temp506, sizeof( void*), 2u));}}});}
else{ prev_fields=({ struct Cyc_List_List* _temp509=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp509->hd=( void*)(*
_temp505->name).f2; _temp509->tl= prev_fields; _temp509;});} if(( void*)
_temp505->sc != ( void*) Cyc_Absyn_Public){({ struct Cyc_Std_String_pa_struct
_temp511; _temp511.tag= Cyc_Std_String_pa; _temp511.f1=( struct _tagged_arr)*(*
_temp505->name).f2;{ void* _temp510[ 1u]={& _temp511}; Cyc_Tcutil_warn( loc,
_tag_arr("ignoring scope of field %s", sizeof( unsigned char), 27u), _tag_arr(
_temp510, sizeof( void*), 1u));}});( void*)( _temp505->sc=( void*)(( void*) Cyc_Absyn_Public));}}}
fields2= fields;}; _pop_region( uprev_rgn);}{ struct Cyc_List_List* _temp512=
fields; for( 0; _temp512 !=  0; _temp512= _temp512->tl){ struct Cyc_Absyn_Tunionfield*
_temp513=( struct Cyc_Absyn_Tunionfield*) _temp512->hd; ge->ordinaries=(( struct
Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
_tuple4* v)) Cyc_Dict_insert)( ge->ordinaries,(* _temp513->name).f2,({ struct
_tuple4* _temp514=( struct _tuple4*) _cycalloc( sizeof( struct _tuple4));
_temp514->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct* _temp515=( struct Cyc_Tcenv_TunionRes_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_TunionRes_struct)); _temp515[ 0]=({ struct
Cyc_Tcenv_TunionRes_struct _temp516; _temp516.tag= Cyc_Tcenv_TunionRes; _temp516.f1=
tudres; _temp516.f2= _temp513; _temp516;}); _temp515;}); _temp514->f2= 1;
_temp514;}));}} return fields2;}} void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_arr* v=(* tud->name).f2; struct _tagged_arr obj; if( tud->is_xtunion){
obj= _tag_arr("xtunion", sizeof( unsigned char), 8u);} else{ obj= _tag_arr("tunion",
sizeof( unsigned char), 7u);}{ struct Cyc_List_List* tvs= tud->tvs;{ struct Cyc_List_List*
tvs2= tvs; for( 0; tvs2 !=  0; tvs2= tvs2->tl){ void* _temp517= Cyc_Absyn_compress_kb((
void*)(( struct Cyc_Absyn_Tvar*) tvs2->hd)->kind); struct Cyc_Core_Opt* _temp529;
struct Cyc_Core_Opt** _temp531; void* _temp532; struct Cyc_Core_Opt* _temp534;
struct Cyc_Core_Opt** _temp536; void* _temp537; struct Cyc_Core_Opt* _temp539;
struct Cyc_Core_Opt** _temp541; void* _temp542; _LL519: if(*(( int*) _temp517)
==  Cyc_Absyn_Unknown_kb){ _LL530: _temp529=(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp517)->f1; _temp531=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Unknown_kb_struct*)
_temp517)->f1; goto _LL520;} else{ goto _LL521;} _LL521: if(*(( int*) _temp517)
==  Cyc_Absyn_Less_kb){ _LL535: _temp534=(( struct Cyc_Absyn_Less_kb_struct*)
_temp517)->f1; _temp536=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*)
_temp517)->f1; goto _LL533; _LL533: _temp532=( void*)(( struct Cyc_Absyn_Less_kb_struct*)
_temp517)->f2; if( _temp532 == ( void*) Cyc_Absyn_MemKind){ goto _LL522;} else{
goto _LL523;}} else{ goto _LL523;} _LL523: if(*(( int*) _temp517) ==  Cyc_Absyn_Less_kb){
_LL540: _temp539=(( struct Cyc_Absyn_Less_kb_struct*) _temp517)->f1; _temp541=(
struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Less_kb_struct*) _temp517)->f1; goto
_LL538; _LL538: _temp537=( void*)(( struct Cyc_Absyn_Less_kb_struct*) _temp517)->f2;
if( _temp537 == ( void*) Cyc_Absyn_AnyKind){ goto _LL524;} else{ goto _LL525;}}
else{ goto _LL525;} _LL525: if(*(( int*) _temp517) ==  Cyc_Absyn_Eq_kb){ _LL543:
_temp542=( void*)(( struct Cyc_Absyn_Eq_kb_struct*) _temp517)->f1; if( _temp542
== ( void*) Cyc_Absyn_MemKind){ goto _LL526;} else{ goto _LL527;}} else{ goto
_LL527;} _LL527: goto _LL528; _LL520: _temp536= _temp531; goto _LL522; _LL522:
_temp541= _temp536; goto _LL524; _LL524:* _temp541=({ struct Cyc_Core_Opt*
_temp544=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp544->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_kb_struct* _temp545=( struct
Cyc_Absyn_Eq_kb_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct));
_temp545[ 0]=({ struct Cyc_Absyn_Eq_kb_struct _temp546; _temp546.tag= Cyc_Absyn_Eq_kb;
_temp546.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp546;}); _temp545;}));
_temp544;}); goto _LL518; _LL526:({ struct Cyc_Std_String_pa_struct _temp550;
_temp550.tag= Cyc_Std_String_pa; _temp550.f1=( struct _tagged_arr)*(( struct Cyc_Absyn_Tvar*)
tvs2->hd)->name;{ struct Cyc_Std_String_pa_struct _temp549; _temp549.tag= Cyc_Std_String_pa;
_temp549.f1=( struct _tagged_arr)* v;{ struct Cyc_Std_String_pa_struct _temp548;
_temp548.tag= Cyc_Std_String_pa; _temp548.f1=( struct _tagged_arr) obj;{ void*
_temp547[ 3u]={& _temp548,& _temp549,& _temp550}; Cyc_Tcutil_terr( loc, _tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof( unsigned char), 54u), _tag_arr( _temp547, sizeof( void*), 3u));}}}});
goto _LL518; _LL528: goto _LL518; _LL518:;}} Cyc_Tcutil_check_unique_tvars( loc,
tvs); Cyc_Tcutil_add_tvar_identities( tvs);{ struct Cyc_Core_Opt* tud_opt; if(
tud->is_xtunion){{ struct _handler_cons _temp551; _push_handler(& _temp551);{
int _temp553= 0; if( setjmp( _temp551.handler)){ _temp553= 1;} if( ! _temp553){
tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc, tud->name);; _pop_handler();}
else{ void* _temp552=( void*) _exn_thrown; void* _temp555= _temp552; _LL557: if(
_temp555 ==  Cyc_Dict_Absent){ goto _LL558;} else{ goto _LL559;} _LL559: goto
_LL560; _LL558:({ struct Cyc_Std_String_pa_struct _temp562; _temp562.tag= Cyc_Std_String_pa;
_temp562.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tud->name);{ void*
_temp561[ 1u]={& _temp562}; Cyc_Tcutil_terr( loc, _tag_arr("qualified xtunion declaration %s is not an existing xtunion",
sizeof( unsigned char), 60u), _tag_arr( _temp561, sizeof( void*), 1u));}});
return; _LL560:( void) _throw( _temp555); _LL556:;}}} if( tud_opt !=  0){ tud->name=(*((
struct Cyc_Absyn_Tuniondecl**) tud_opt->v))->name;} else{(* tud->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp563=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp563[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp564; _temp564.tag= Cyc_Absyn_Abs_n; _temp564.f1= te->ns; _temp564;});
_temp563;});}} else{{ void* _temp565=(* tud->name).f1; struct Cyc_List_List*
_temp573; _LL567: if(( unsigned int) _temp565 >  1u?*(( int*) _temp565) ==  Cyc_Absyn_Rel_n:
0){ _LL574: _temp573=(( struct Cyc_Absyn_Rel_n_struct*) _temp565)->f1; if(
_temp573 ==  0){ goto _LL568;} else{ goto _LL569;}} else{ goto _LL569;} _LL569:
if(( unsigned int) _temp565 >  1u?*(( int*) _temp565) ==  Cyc_Absyn_Abs_n: 0){
goto _LL570;} else{ goto _LL571;} _LL571: goto _LL572; _LL568:(* tud->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp575=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp575[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp576; _temp576.tag= Cyc_Absyn_Abs_n; _temp576.f1= te->ns; _temp576;});
_temp575;}); goto _LL566; _LL570: goto _LL572; _LL572:({ struct Cyc_Std_String_pa_struct
_temp578; _temp578.tag= Cyc_Std_String_pa; _temp578.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
tud->name);{ void* _temp577[ 1u]={& _temp578}; Cyc_Tcutil_terr( loc, _tag_arr("qualified tunion declarations are not implemented (%s)",
sizeof( unsigned char), 55u), _tag_arr( _temp577, sizeof( void*), 1u));}});
return; _LL566:;} tud_opt=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_arr* k)) Cyc_Dict_lookup_opt)( ge->tuniondecls, v);}{ struct
_tuple5 _temp580=({ struct _tuple5 _temp579; _temp579.f1= tud->fields; _temp579.f2=
tud_opt; _temp579;}); struct Cyc_Core_Opt* _temp590; struct Cyc_Core_Opt*
_temp592; struct Cyc_Core_Opt* _temp594; struct Cyc_Core_Opt* _temp596; struct
Cyc_Core_Opt _temp598; struct Cyc_List_List* _temp599; struct Cyc_List_List**
_temp601; struct Cyc_Core_Opt* _temp602; struct Cyc_Core_Opt _temp604; struct
Cyc_Absyn_Tuniondecl** _temp605; struct Cyc_Core_Opt* _temp607; struct Cyc_Core_Opt
_temp609; struct Cyc_List_List* _temp610; struct Cyc_List_List** _temp612;
struct Cyc_Core_Opt* _temp613; struct Cyc_Core_Opt _temp615; struct Cyc_Absyn_Tuniondecl**
_temp616; struct Cyc_Core_Opt* _temp618; _LL582: _LL593: _temp592= _temp580.f1;
if( _temp592 ==  0){ goto _LL591;} else{ goto _LL584;} _LL591: _temp590=
_temp580.f2; if( _temp590 ==  0){ goto _LL583;} else{ goto _LL584;} _LL584:
_LL597: _temp596= _temp580.f1; if( _temp596 ==  0){ goto _LL586;} else{ _temp598=*
_temp596; _LL600: _temp599=( struct Cyc_List_List*) _temp598.v; _temp601=(
struct Cyc_List_List**)&(* _temp580.f1).v; goto _LL595;} _LL595: _temp594=
_temp580.f2; if( _temp594 ==  0){ goto _LL585;} else{ goto _LL586;} _LL586:
_LL608: _temp607= _temp580.f1; if( _temp607 ==  0){ goto _LL588;} else{ _temp609=*
_temp607; _LL611: _temp610=( struct Cyc_List_List*) _temp609.v; _temp612=(
struct Cyc_List_List**)&(* _temp580.f1).v; goto _LL603;} _LL603: _temp602=
_temp580.f2; if( _temp602 ==  0){ goto _LL588;} else{ _temp604=* _temp602;
_LL606: _temp605=( struct Cyc_Absyn_Tuniondecl**) _temp604.v; goto _LL587;}
_LL588: _LL619: _temp618= _temp580.f1; if( _temp618 ==  0){ goto _LL614;} else{
goto _LL581;} _LL614: _temp613= _temp580.f2; if( _temp613 ==  0){ goto _LL581;}
else{ _temp615=* _temp613; _LL617: _temp616=( struct Cyc_Absyn_Tuniondecl**)
_temp615.v; goto _LL589;} _LL583: ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Tuniondecl** v))
Cyc_Dict_insert)( ge->tuniondecls, v,({ struct Cyc_Absyn_Tuniondecl** _temp620=(
struct Cyc_Absyn_Tuniondecl**) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp620[ 0]= tud; _temp620;})); goto _LL581; _LL585: { struct Cyc_Absyn_Tuniondecl**
_temp621=({ struct Cyc_Absyn_Tuniondecl** _temp622=( struct Cyc_Absyn_Tuniondecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp622[ 0]=({ struct Cyc_Absyn_Tuniondecl*
_temp623=( struct Cyc_Absyn_Tuniondecl*) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl));
_temp623->sc=( void*)(( void*) Cyc_Absyn_Extern); _temp623->name= tud->name;
_temp623->tvs= tvs; _temp623->fields= 0; _temp623->is_xtunion= tud->is_xtunion;
_temp623;}); _temp622;}); ge->tuniondecls=(( struct Cyc_Dict_Dict*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_arr* k, struct Cyc_Absyn_Tuniondecl** v)) Cyc_Dict_insert)(
ge->tuniondecls, v, _temp621);* _temp601= Cyc_Tc_tcTunionFields( te, ge, loc,
obj, tud->is_xtunion, tud->name,* _temp601, tvs, tud);* _temp621= tud; goto
_LL581;} _LL587: { struct Cyc_Absyn_Tuniondecl* _temp624=* _temp605;* _temp605=({
struct Cyc_Absyn_Tuniondecl* _temp625=( struct Cyc_Absyn_Tuniondecl*) _cycalloc(
sizeof( struct Cyc_Absyn_Tuniondecl)); _temp625->sc=( void*)(( void*) Cyc_Absyn_Extern);
_temp625->name= tud->name; _temp625->tvs= tvs; _temp625->fields= 0; _temp625->is_xtunion=
tud->is_xtunion; _temp625;});* _temp612= Cyc_Tc_tcTunionFields( te, ge, loc, obj,
tud->is_xtunion, tud->name,* _temp612, tvs, tud);* _temp605= _temp624; _temp616=
_temp605; goto _LL589;} _LL589: { struct Cyc_Absyn_Tuniondecl* _temp626= Cyc_Tcdecl_merge_tuniondecl(*
_temp616, tud, loc, Cyc_Tc_tc_msg); if( _temp626 ==  0){ return;} else{*
_temp616=( struct Cyc_Absyn_Tuniondecl*) _check_null( _temp626); goto _LL581;}}
_LL581:;}}}} void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl* ed){ struct
_tagged_arr* v=(* ed->name).f2;{ void* _temp627=(* ed->name).f1; struct Cyc_List_List*
_temp635; struct Cyc_List_List* _temp637; _LL629: if(( unsigned int) _temp627 > 
1u?*(( int*) _temp627) ==  Cyc_Absyn_Rel_n: 0){ _LL636: _temp635=(( struct Cyc_Absyn_Rel_n_struct*)
_temp627)->f1; if( _temp635 ==  0){ goto _LL630;} else{ goto _LL631;}} else{
goto _LL631;} _LL631: if(( unsigned int) _temp627 >  1u?*(( int*) _temp627) == 
Cyc_Absyn_Abs_n: 0){ _LL638: _temp637=(( struct Cyc_Absyn_Abs_n_struct*)
_temp627)->f1; if( _temp637 ==  0){ goto _LL632;} else{ goto _LL633;}} else{
goto _LL633;} _LL633: goto _LL634; _LL630: goto _LL628; _LL632: goto _LL628;
_LL634:({ struct Cyc_Std_String_pa_struct _temp640; _temp640.tag= Cyc_Std_String_pa;
_temp640.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( ed->name);{ void*
_temp639[ 1u]={& _temp640}; Cyc_Tcutil_terr( loc, _tag_arr("qualified enum declarations are not implemented (%s)",
sizeof( unsigned char), 53u), _tag_arr( _temp639, sizeof( void*), 1u));}});
return; _LL628:;}(* ed->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp641=( struct Cyc_Absyn_Abs_n_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp641[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp642; _temp642.tag= Cyc_Absyn_Abs_n;
_temp642.f1= te->ns; _temp642;}); _temp641;}); if( ed->fields !=  0){ struct
_RegionHandle _temp643= _new_region("uprev_rgn"); struct _RegionHandle*
uprev_rgn=& _temp643; _push_region( uprev_rgn);{ struct Cyc_List_List*
prev_fields= 0; unsigned int tag_count= 0; struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( ed->fields))->v; for( 0; fs !=  0; fs= fs->tl){
struct Cyc_Absyn_Enumfield* _temp644=( struct Cyc_Absyn_Enumfield*) fs->hd; if(((
int(*)( int(* compare)( struct _tagged_arr*, struct _tagged_arr*), struct Cyc_List_List*
l, struct _tagged_arr* x)) Cyc_List_mem)( Cyc_Std_zstrptrcmp, prev_fields,(*
_temp644->name).f2)){({ struct Cyc_Std_String_pa_struct _temp646; _temp646.tag=
Cyc_Std_String_pa; _temp646.f1=( struct _tagged_arr)*(* _temp644->name).f2;{
void* _temp645[ 1u]={& _temp646}; Cyc_Tcutil_terr( _temp644->loc, _tag_arr("duplicate field name %s",
sizeof( unsigned char), 24u), _tag_arr( _temp645, sizeof( void*), 1u));}});}
else{ prev_fields=({ struct Cyc_List_List* _temp647=( struct Cyc_List_List*)
_region_malloc( uprev_rgn, sizeof( struct Cyc_List_List)); _temp647->hd=( void*)(*
_temp644->name).f2; _temp647->tl= prev_fields; _temp647;});} if( _temp644->tag
==  0){ _temp644->tag=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count,
_temp644->loc);} else{ if( ! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)
_check_null( _temp644->tag))){({ struct Cyc_Std_String_pa_struct _temp650;
_temp650.tag= Cyc_Std_String_pa; _temp650.f1=( struct _tagged_arr)*(* _temp644->name).f2;{
struct Cyc_Std_String_pa_struct _temp649; _temp649.tag= Cyc_Std_String_pa;
_temp649.f1=( struct _tagged_arr)* v;{ void* _temp648[ 2u]={& _temp649,&
_temp650}; Cyc_Tcutil_terr( loc, _tag_arr("enum %s, field %s: expression is not constant",
sizeof( unsigned char), 46u), _tag_arr( _temp648, sizeof( void*), 2u));}}});}}{
unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)
_check_null( _temp644->tag)); tag_count= t1 +  1;(* _temp644->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp651=( struct Cyc_Absyn_Abs_n_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp651[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp652; _temp652.tag= Cyc_Absyn_Abs_n; _temp652.f1= te->ns; _temp652;});
_temp651;});}}}; _pop_region( uprev_rgn);}{ struct _handler_cons _temp653;
_push_handler(& _temp653);{ int _temp655= 0; if( setjmp( _temp653.handler)){
_temp655= 1;} if( ! _temp655){{ struct Cyc_Absyn_Enumdecl** _temp656=(( struct
Cyc_Absyn_Enumdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)(
ge->enumdecls, v); struct Cyc_Absyn_Enumdecl* _temp657= Cyc_Tcdecl_merge_enumdecl(*
_temp656, ed, loc, Cyc_Tc_tc_msg); if( _temp657 ==  0){ _npop_handler( 0u);
return;}* _temp656=( struct Cyc_Absyn_Enumdecl*) _check_null( _temp657);};
_pop_handler();} else{ void* _temp654=( void*) _exn_thrown; void* _temp659=
_temp654; _LL661: if( _temp659 ==  Cyc_Dict_Absent){ goto _LL662;} else{ goto
_LL663;} _LL663: goto _LL664; _LL662: { struct Cyc_Absyn_Enumdecl** _temp665=({
struct Cyc_Absyn_Enumdecl** _temp666=( struct Cyc_Absyn_Enumdecl**) _cycalloc(
sizeof( struct Cyc_Absyn_Enumdecl*)); _temp666[ 0]= ed; _temp666;}); ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct
Cyc_Absyn_Enumdecl** v)) Cyc_Dict_insert)( ge->enumdecls, v, _temp665); goto
_LL660;} _LL664:( void) _throw( _temp659); _LL660:;}}} if( ed->fields !=  0){
struct Cyc_List_List* fs=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*)
_check_null( ed->fields))->v; for( 0; fs !=  0; fs= fs->tl){ struct Cyc_Absyn_Enumfield*
_temp667=( struct Cyc_Absyn_Enumfield*) fs->hd; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_arr* k, struct _tuple4* v)) Cyc_Dict_insert)(
ge->ordinaries,(* _temp667->name).f2,({ struct _tuple4* _temp668=( struct
_tuple4*) _cycalloc( sizeof( struct _tuple4)); _temp668->f1=( void*)({ struct
Cyc_Tcenv_EnumRes_struct* _temp669=( struct Cyc_Tcenv_EnumRes_struct*) _cycalloc(
sizeof( struct Cyc_Tcenv_EnumRes_struct)); _temp669[ 0]=({ struct Cyc_Tcenv_EnumRes_struct
_temp670; _temp670.tag= Cyc_Tcenv_EnumRes; _temp670.f1= ed; _temp670.f2=
_temp667; _temp670;}); _temp669;}); _temp668->f2= 1; _temp668;}));}}} static int
Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){ void* _temp671=
sc; _LL673: if( _temp671 == ( void*) Cyc_Absyn_Static){ goto _LL674;} else{ goto
_LL675;} _LL675: if( _temp671 == ( void*) Cyc_Absyn_Abstract){ goto _LL676;}
else{ goto _LL677;} _LL677: if( _temp671 == ( void*) Cyc_Absyn_Public){ goto
_LL678;} else{ goto _LL679;} _LL679: if( _temp671 == ( void*) Cyc_Absyn_Extern){
goto _LL680;} else{ goto _LL681;} _LL681: if( _temp671 == ( void*) Cyc_Absyn_ExternC){
goto _LL682;} else{ goto _LL672;} _LL674:({ void* _temp683[ 0u]={}; Cyc_Tcutil_warn(
loc, _tag_arr("static declaration nested within extern \"C\"", sizeof(
unsigned char), 44u), _tag_arr( _temp683, sizeof( void*), 0u));}); return 0;
_LL676:({ void* _temp684[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("abstract declaration nested within extern \"C\"",
sizeof( unsigned char), 46u), _tag_arr( _temp684, sizeof( void*), 0u));});
return 0; _LL678: return 1; _LL680: return 1; _LL682:({ void* _temp685[ 0u]={};
Cyc_Tcutil_warn( loc, _tag_arr("nested extern \"C\" declaration", sizeof(
unsigned char), 30u), _tag_arr( _temp685, sizeof( void*), 0u));}); return 1;
_LL672:;} static void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
ds0, int in_externC, int check_var_init){ struct Cyc_Tcenv_Genv* ge=(( struct
Cyc_Tcenv_Genv*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)(
te->ae, te->ns); struct Cyc_List_List* last= 0; struct Cyc_Dict_Dict* dict= Cyc_Tcgenrep_empty_typerep_dict();
struct Cyc_List_List* _temp686= ds0; for( 0; _temp686 !=  0;( last= _temp686,
_temp686= _temp686->tl)){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)
_temp686->hd; struct Cyc_Position_Segment* loc= d->loc; void* _temp687=( void*)
d->r; struct Cyc_Absyn_Vardecl* _temp713; struct Cyc_Absyn_Fndecl* _temp715;
struct Cyc_Absyn_Typedefdecl* _temp717; struct Cyc_Absyn_Structdecl* _temp719;
struct Cyc_Absyn_Uniondecl* _temp721; struct Cyc_Absyn_Tuniondecl* _temp723;
struct Cyc_Absyn_Enumdecl* _temp725; struct Cyc_List_List* _temp727; struct
_tagged_arr* _temp729; struct Cyc_List_List* _temp731; struct _tuple0* _temp733;
struct _tuple0 _temp735; struct _tagged_arr* _temp736; void* _temp738; struct
Cyc_List_List* _temp740; _LL689: if(*(( int*) _temp687) ==  Cyc_Absyn_Let_d){
goto _LL690;} else{ goto _LL691;} _LL691: if(*(( int*) _temp687) ==  Cyc_Absyn_Letv_d){
goto _LL692;} else{ goto _LL693;} _LL693: if(*(( int*) _temp687) ==  Cyc_Absyn_Var_d){
_LL714: _temp713=(( struct Cyc_Absyn_Var_d_struct*) _temp687)->f1; goto _LL694;}
else{ goto _LL695;} _LL695: if(*(( int*) _temp687) ==  Cyc_Absyn_Fn_d){ _LL716:
_temp715=(( struct Cyc_Absyn_Fn_d_struct*) _temp687)->f1; goto _LL696;} else{
goto _LL697;} _LL697: if(*(( int*) _temp687) ==  Cyc_Absyn_Typedef_d){ _LL718:
_temp717=(( struct Cyc_Absyn_Typedef_d_struct*) _temp687)->f1; goto _LL698;}
else{ goto _LL699;} _LL699: if(*(( int*) _temp687) ==  Cyc_Absyn_Struct_d){
_LL720: _temp719=(( struct Cyc_Absyn_Struct_d_struct*) _temp687)->f1; goto
_LL700;} else{ goto _LL701;} _LL701: if(*(( int*) _temp687) ==  Cyc_Absyn_Union_d){
_LL722: _temp721=(( struct Cyc_Absyn_Union_d_struct*) _temp687)->f1; goto _LL702;}
else{ goto _LL703;} _LL703: if(*(( int*) _temp687) ==  Cyc_Absyn_Tunion_d){
_LL724: _temp723=(( struct Cyc_Absyn_Tunion_d_struct*) _temp687)->f1; goto
_LL704;} else{ goto _LL705;} _LL705: if(*(( int*) _temp687) ==  Cyc_Absyn_Enum_d){
_LL726: _temp725=(( struct Cyc_Absyn_Enum_d_struct*) _temp687)->f1; goto _LL706;}
else{ goto _LL707;} _LL707: if(*(( int*) _temp687) ==  Cyc_Absyn_Namespace_d){
_LL730: _temp729=(( struct Cyc_Absyn_Namespace_d_struct*) _temp687)->f1; goto
_LL728; _LL728: _temp727=(( struct Cyc_Absyn_Namespace_d_struct*) _temp687)->f2;
goto _LL708;} else{ goto _LL709;} _LL709: if(*(( int*) _temp687) ==  Cyc_Absyn_Using_d){
_LL734: _temp733=(( struct Cyc_Absyn_Using_d_struct*) _temp687)->f1; _temp735=*
_temp733; _LL739: _temp738= _temp735.f1; goto _LL737; _LL737: _temp736= _temp735.f2;
goto _LL732; _LL732: _temp731=(( struct Cyc_Absyn_Using_d_struct*) _temp687)->f2;
goto _LL710;} else{ goto _LL711;} _LL711: if(*(( int*) _temp687) ==  Cyc_Absyn_ExternC_d){
_LL741: _temp740=(( struct Cyc_Absyn_ExternC_d_struct*) _temp687)->f1; goto
_LL712;} else{ goto _LL688;} _LL690: goto _LL692; _LL692:({ void* _temp742[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("top level let-declarations are not implemented",
sizeof( unsigned char), 47u), _tag_arr( _temp742, sizeof( void*), 0u));}); goto
_LL688; _LL694: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp713->sc):
0){( void*)( _temp713->sc=( void*)(( void*) Cyc_Absyn_ExternC));} if( _temp713->initializer
!=  0){ void* _temp743=( void*)(( struct Cyc_Absyn_Exp*) _check_null( _temp713->initializer))->r;
void* _temp749; _LL745: if(*(( int*) _temp743) ==  Cyc_Absyn_Gentyp_e){ _LL750:
_temp749=( void*)(( struct Cyc_Absyn_Gentyp_e_struct*) _temp743)->f2; goto
_LL746;} else{ goto _LL747;} _LL747: goto _LL748; _LL746: { struct Cyc_Absyn_Exp*
_temp753; struct Cyc_List_List* _temp755; struct Cyc_Dict_Dict* _temp757; struct
_tuple3 _temp751= Cyc_Tcgenrep_tcGenrep( te, ge, loc, _temp749, dict); _LL758:
_temp757= _temp751.f1; goto _LL756; _LL756: _temp755= _temp751.f2; goto _LL754;
_LL754: _temp753= _temp751.f3; goto _LL752; _LL752: dict= _temp757; Cyc_Tc_tc_decls(
te, _temp755, in_externC, check_var_init); _temp713->initializer=( struct Cyc_Absyn_Exp*)
_temp753; Cyc_Tc_tcVardecl( te, ge, loc, _temp713, check_var_init); if( _temp755
!=  0){ if( last !=  0){(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp755, _temp686);(( struct
Cyc_List_List*) _check_null( last))->tl= _temp755;} else{ struct Cyc_List_List
tmp=({ struct Cyc_List_List _temp759; _temp759.hd=( void*)(( struct Cyc_Absyn_Decl*)
_temp686->hd); _temp759.tl= _temp686->tl; _temp759;});( struct Cyc_Absyn_Decl*)(
_temp686->hd=( void*)(( struct Cyc_Absyn_Decl*) _temp755->hd)); _temp686->tl=
_temp755->tl;( struct Cyc_Absyn_Decl*)( _temp755->hd=( void*)(( struct Cyc_Absyn_Decl*)
tmp.hd)); _temp755->tl= tmp.tl;(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( _temp686, _temp755);}}
continue;} _LL748: goto _LL744; _LL744:;} Cyc_Tc_tcVardecl( te, ge, loc,
_temp713, check_var_init); goto _LL688; _LL696: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp715->sc): 0){( void*)( _temp715->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp715); goto _LL688; _LL698: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp717); goto _LL688; _LL700: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp719->sc): 0){( void*)( _temp719->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp719); goto _LL688; _LL702: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp721->sc): 0){( void*)( _temp721->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge, loc, _temp721);
goto _LL688; _LL704: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*)
_temp723->sc): 0){( void*)( _temp723->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp723); goto _LL688; _LL706: if( in_externC?
Cyc_Tc_okay_externC( d->loc,( void*) _temp725->sc): 0){( void*)( _temp725->sc=(
void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge, loc, _temp725);
goto _LL688; _LL708: { struct Cyc_List_List* _temp760= te->ns; struct Cyc_List_List*
_temp761=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp760,({ struct Cyc_List_List* _temp762=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp762->hd=( void*) _temp729;
_temp762->tl= 0; _temp762;})); if( !(( int(*)( struct Cyc_Set_Set* s, struct
_tagged_arr* elt)) Cyc_Set_member)( ge->namespaces, _temp729)){ ge->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)(
ge->namespaces, _temp729); te->ae=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* k, struct Cyc_Tcenv_Genv* v)) Cyc_Dict_insert)( te->ae,
_temp761, Cyc_Tcenv_empty_genv());} te->ns= _temp761; Cyc_Tc_tc_decls( te,
_temp727, in_externC, check_var_init); te->ns= _temp760; goto _LL688;} _LL710: {
struct _tagged_arr* first; struct Cyc_List_List* rest;{ void* _temp763= _temp738;
struct Cyc_List_List* _temp775; struct Cyc_List_List* _temp777; struct Cyc_List_List*
_temp779; struct Cyc_List_List _temp781; struct Cyc_List_List* _temp782; struct
_tagged_arr* _temp784; struct Cyc_List_List* _temp786; struct Cyc_List_List
_temp788; struct Cyc_List_List* _temp789; struct _tagged_arr* _temp791; _LL765:
if( _temp763 == ( void*) Cyc_Absyn_Loc_n){ goto _LL766;} else{ goto _LL767;}
_LL767: if(( unsigned int) _temp763 >  1u?*(( int*) _temp763) ==  Cyc_Absyn_Rel_n:
0){ _LL776: _temp775=(( struct Cyc_Absyn_Rel_n_struct*) _temp763)->f1; if(
_temp775 ==  0){ goto _LL768;} else{ goto _LL769;}} else{ goto _LL769;} _LL769:
if(( unsigned int) _temp763 >  1u?*(( int*) _temp763) ==  Cyc_Absyn_Abs_n: 0){
_LL778: _temp777=(( struct Cyc_Absyn_Abs_n_struct*) _temp763)->f1; if( _temp777
==  0){ goto _LL770;} else{ goto _LL771;}} else{ goto _LL771;} _LL771: if((
unsigned int) _temp763 >  1u?*(( int*) _temp763) ==  Cyc_Absyn_Rel_n: 0){ _LL780:
_temp779=(( struct Cyc_Absyn_Rel_n_struct*) _temp763)->f1; if( _temp779 ==  0){
goto _LL773;} else{ _temp781=* _temp779; _LL785: _temp784=( struct _tagged_arr*)
_temp781.hd; goto _LL783; _LL783: _temp782= _temp781.tl; goto _LL772;}} else{
goto _LL773;} _LL773: if(( unsigned int) _temp763 >  1u?*(( int*) _temp763) == 
Cyc_Absyn_Abs_n: 0){ _LL787: _temp786=(( struct Cyc_Absyn_Abs_n_struct*)
_temp763)->f1; if( _temp786 ==  0){ goto _LL764;} else{ _temp788=* _temp786;
_LL792: _temp791=( struct _tagged_arr*) _temp788.hd; goto _LL790; _LL790:
_temp789= _temp788.tl; goto _LL774;}} else{ goto _LL764;} _LL766: goto _LL768;
_LL768: goto _LL770; _LL770: first= _temp736; rest= 0; goto _LL764; _LL772:
_temp791= _temp784; _temp789= _temp782; goto _LL774; _LL774: first= _temp791;
rest=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( _temp789,({ struct Cyc_List_List* _temp793=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp793->hd=( void*) _temp736;
_temp793->tl= 0; _temp793;})); goto _LL764; _LL764:;}{ struct Cyc_List_List*
_temp794= Cyc_Tcenv_resolve_namespace( te, loc, first, rest); ge->availables=({
struct Cyc_List_List* _temp795=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp795->hd=( void*) _temp794; _temp795->tl= ge->availables;
_temp795;}); Cyc_Tc_tc_decls( te, _temp731, in_externC, check_var_init); ge->availables=((
struct Cyc_List_List*) _check_null( ge->availables))->tl; goto _LL688;}} _LL712:
Cyc_Tc_tc_decls( te, _temp740, 1, check_var_init); goto _LL688; _LL688:;}} void
Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int check_var_init, struct Cyc_List_List*
ds){ Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r); Cyc_Tc_tc_decls( te, ds,
0, check_var_init);} static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env,
struct Cyc_Absyn_Decl* d); static struct Cyc_List_List* Cyc_Tc_treeshake_f(
struct Cyc_Dict_Dict* env, struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)(
int(* f)( struct Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict*
env, struct Cyc_List_List* x)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env,
ds);} struct _tuple7{ struct Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d){ void* _temp796=( void*) d->r; struct Cyc_Absyn_Vardecl* _temp806; struct Cyc_List_List*
_temp808; struct Cyc_List_List** _temp810; struct Cyc_List_List* _temp811;
struct Cyc_List_List** _temp813; _LL798: if(*(( int*) _temp796) ==  Cyc_Absyn_Var_d){
_LL807: _temp806=(( struct Cyc_Absyn_Var_d_struct*) _temp796)->f1; goto _LL799;}
else{ goto _LL800;} _LL800: if(*(( int*) _temp796) ==  Cyc_Absyn_Using_d){
_LL809: _temp808=(( struct Cyc_Absyn_Using_d_struct*) _temp796)->f2; _temp810=(
struct Cyc_List_List**)&(( struct Cyc_Absyn_Using_d_struct*) _temp796)->f2; goto
_LL801;} else{ goto _LL802;} _LL802: if(*(( int*) _temp796) ==  Cyc_Absyn_Namespace_d){
_LL812: _temp811=(( struct Cyc_Absyn_Namespace_d_struct*) _temp796)->f2;
_temp813=( struct Cyc_List_List**)&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp796)->f2; goto _LL803;} else{ goto _LL804;} _LL804: goto _LL805; _LL799:
if(( void*) _temp806->sc != ( void*) Cyc_Absyn_Extern){ return 1;}{ struct
_tuple0 _temp816; struct _tagged_arr* _temp817; void* _temp819; struct _tuple0*
_temp814= _temp806->name; _temp816=* _temp814; _LL820: _temp819= _temp816.f1;
goto _LL818; _LL818: _temp817= _temp816.f2; goto _LL815; _LL815: { struct Cyc_List_List*
ns;{ void* _temp821= _temp819; struct Cyc_List_List* _temp829; struct Cyc_List_List*
_temp831; _LL823: if( _temp821 == ( void*) Cyc_Absyn_Loc_n){ goto _LL824;} else{
goto _LL825;} _LL825: if(( unsigned int) _temp821 >  1u?*(( int*) _temp821) == 
Cyc_Absyn_Rel_n: 0){ _LL830: _temp829=(( struct Cyc_Absyn_Rel_n_struct*)
_temp821)->f1; goto _LL826;} else{ goto _LL827;} _LL827: if(( unsigned int)
_temp821 >  1u?*(( int*) _temp821) ==  Cyc_Absyn_Abs_n: 0){ _LL832: _temp831=((
struct Cyc_Absyn_Abs_n_struct*) _temp821)->f1; goto _LL828;} else{ goto _LL822;}
_LL824: ns= 0; goto _LL822; _LL826: ns= _temp829; goto _LL822; _LL828: ns=
_temp831; goto _LL822; _LL822:;}{ struct _tuple7* _temp833=(( struct _tuple7*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* k)) Cyc_Dict_lookup)( env, ns);
struct Cyc_Tcenv_Genv* _temp834=(* _temp833).f1; int _temp835=(*(( struct
_tuple4*(*)( struct Cyc_Dict_Dict* d, struct _tagged_arr* k)) Cyc_Dict_lookup)(
_temp834->ordinaries, _temp817)).f2; if( ! _temp835){(* _temp833).f2=(( struct
Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_arr* elt)) Cyc_Set_insert)((*
_temp833).f2, _temp817);} return _temp835;}}} _LL801: _temp813= _temp810; goto
_LL803; _LL803:* _temp813= Cyc_Tc_treeshake_f( env,* _temp813); return 1; _LL805:
return 1; _LL797:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_arr* x, void* y){ return !(( int(*)( struct Cyc_Set_Set* s,
struct _tagged_arr* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple7* nsenv){ struct _tuple7 _temp838;
struct Cyc_Set_Set* _temp839; struct Cyc_Tcenv_Genv* _temp841; struct _tuple7*
_temp836= nsenv; _temp838=* _temp836; _LL842: _temp841= _temp838.f1; goto _LL840;
_LL840: _temp839= _temp838.f2; goto _LL837; _LL837: _temp841->ordinaries=((
struct Cyc_Dict_Dict*(*)( int(* f)( struct Cyc_Set_Set*, struct _tagged_arr*,
struct _tuple4*), struct Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)((
int(*)( struct Cyc_Set_Set* set, struct _tagged_arr* x, struct _tuple4* y)) Cyc_Tc_treeshake_remove_f,
_temp839, _temp841->ordinaries);} static struct _tuple7* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple7* _temp843=( struct _tuple7*)
_cycalloc( sizeof( struct _tuple7)); _temp843->f1= ge; _temp843->f2=(( struct
Cyc_Set_Set*(*)( int(* cmp)( struct _tagged_arr*, struct _tagged_arr*))) Cyc_Set_empty)(
Cyc_Std_strptrcmp); _temp843;});} struct Cyc_List_List* Cyc_Tc_treeshake( struct
Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* _temp844=((
struct Cyc_Dict_Dict*(*)( struct _tuple7*(* f)( struct Cyc_Tcenv_Genv*), struct
Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae); struct
Cyc_List_List* _temp845= Cyc_Tc_treeshake_f( _temp844, ds);(( void(*)( void(* f)(
struct Cyc_List_List*, struct _tuple7*), struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)(
Cyc_Tc_treeshake_remove, _temp844); return _temp845;}
