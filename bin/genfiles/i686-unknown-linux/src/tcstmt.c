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
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
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
x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)( void*, void*), void*
env, struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern struct Cyc_List_List* Cyc_List_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
unsigned char Cyc_List_Nth[ 8u]; struct Cyc_Lineno_Pos{ struct _tagged_arr
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
15u]; extern void* Cyc_Absyn_exn_typ; extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt(
void* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_List_List*, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment* s); struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[ 19u];
extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_Std_aprintf( struct
_tagged_arr fmt, struct _tagged_arr); static const int Cyc_Std_ShortPtr_sa= 0;
struct Cyc_Std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Std_UShortPtr_sa=
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
print_externC_stmts: 1; int print_full_evars: 1; int generate_line_directives: 1;
int use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern
struct _tagged_arr Cyc_Absynpp_typ2string( void*); struct Cyc_Set_Set; extern
unsigned char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; extern unsigned char
Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u]; static const
int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ;
static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{ int
tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
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
extern void* Cyc_Tcenv_return_typ( struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Vardecl*);
extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_type_vars( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_loop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt*
continue_dest); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_in_switch( struct
Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_fallthru( struct
Cyc_Tcenv_Tenv* te, struct Cyc_List_List* new_tvs, struct Cyc_List_List* vds,
struct Cyc_Absyn_Switch_clause* clause); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_next(
struct Cyc_Tcenv_Tenv*, void*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_enter_try(
struct Cyc_Tcenv_Tenv* te); extern int Cyc_Tcenv_get_try_depth( struct Cyc_Tcenv_Tenv*
te); extern void Cyc_Tcenv_process_continue( struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_break( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Stmt**); extern void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct _tagged_arr*, struct Cyc_Absyn_Stmt**);
struct _tuple3{ struct Cyc_Absyn_Switch_clause* f1; struct Cyc_List_List* f2;
struct Cyc_List_List* f3; } ; extern struct _tuple3* Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*, struct Cyc_Absyn_Switch_clause***);
extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser( struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser( struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_add_label(
struct Cyc_Tcenv_Tenv*, struct _tagged_arr*, struct Cyc_Absyn_Stmt*); extern
struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct
Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_new_named_block(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Tvar*
name); extern void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv*); extern struct
Cyc_Tcenv_Tenv* Cyc_Tcenv_add_region( struct Cyc_Tcenv_Tenv* te, void* r);
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos(
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern void* Cyc_Tcutil_compress( void* t); extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern void Cyc_Tcutil_explain_failure();
extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void*
f2; } ; extern struct _tuple4* Cyc_Tcutil_make_inst_var( struct Cyc_List_List*,
struct Cyc_Absyn_Tvar*); extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*, struct Cyc_List_List* bound_tvars, void* k, int
allow_evars, void*); extern int* Cyc_Tcutil_new_tvar_id(); extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part);
struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; extern
struct _tuple5 Cyc_Tcpat_tcPat( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat*
p, void** topt, void** region_opt); extern void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*, struct Cyc_Absyn_Pat* p); extern void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*, struct Cyc_List_List*); static const int Cyc_CfFlowInfo_VarRoot=
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
} ; extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); static void Cyc_Tcstmt_decorate_stmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s){ Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser(
te)); s->try_depth= Cyc_Tcenv_get_try_depth( te);} static void Cyc_Tcstmt_pattern_synth(
struct Cyc_Position_Segment* loc, struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
tvs, struct Cyc_List_List* vs, struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp*
where_opt, int new_block){ struct Cyc_Tcenv_Tenv* te2= Cyc_Tcenv_add_type_vars(
loc, te, tvs); if( new_block){ te2= Cyc_Tcenv_new_block( loc, te2);}{ struct Cyc_Core_Opt*
_temp0=({ struct Cyc_Core_Opt* _temp2=( struct Cyc_Core_Opt*) _cycalloc( sizeof(
struct Cyc_Core_Opt)); _temp2->v=( void*) Cyc_Tcenv_curr_rgn( te2); _temp2;});{
struct Cyc_List_List* _temp1= vs; for( 0; _temp1 !=  0; _temp1= _temp1->tl){ te2=
Cyc_Tcenv_add_pat_var( loc, te2,( struct Cyc_Absyn_Vardecl*) _temp1->hd);((
struct Cyc_Absyn_Vardecl*) _temp1->hd)->rgn= _temp0;}} if( where_opt !=  0){ Cyc_Tcexp_tcTest(
te2,( struct Cyc_Absyn_Exp*) _check_null( where_opt), _tag_arr("switch clause guard",
sizeof( unsigned char), 20u));} if( vs !=  0){ te2= Cyc_Tcenv_set_encloser( te2,
s);} Cyc_Tcstmt_tcStmt( te2, s, 0); if( vs !=  0){ Cyc_NewControlFlow_set_encloser(
s, Cyc_Tcenv_get_encloser( te));}}} struct _tuple6{ struct Cyc_Tcenv_Tenv* f1;
struct Cyc_List_List* f2; } ; struct _tuple6 Cyc_Tcstmt_check_forarray_decls(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, struct Cyc_List_List*
decls){({ void* _temp3[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct
_tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("check_forarray_decls", sizeof(
unsigned char), 21u), _tag_arr( _temp3, sizeof( void*), 0u));}); return({ struct
_tuple6 _temp4; _temp4.f1= te; _temp4.f2= 0; _temp4;});} void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s0, int new_block){ Cyc_Tcstmt_decorate_stmt(
te, s0);{ void* _temp5=( void*) s0->r; struct Cyc_Absyn_Exp* _temp49; struct Cyc_Absyn_Stmt*
_temp51; struct Cyc_Absyn_Stmt* _temp53; struct Cyc_Absyn_Exp* _temp55; struct
Cyc_Absyn_Stmt* _temp57; struct Cyc_Absyn_Stmt* _temp59; struct Cyc_Absyn_Exp*
_temp61; struct Cyc_Absyn_Stmt* _temp63; struct _tuple2 _temp65; struct Cyc_Absyn_Stmt*
_temp67; struct Cyc_Absyn_Exp* _temp69; struct Cyc_Absyn_Stmt* _temp71; struct
_tuple2 _temp73; struct Cyc_Absyn_Stmt* _temp75; struct Cyc_Absyn_Exp* _temp77;
struct _tuple2 _temp79; struct Cyc_Absyn_Stmt* _temp81; struct Cyc_Absyn_Exp*
_temp83; struct Cyc_Absyn_Exp* _temp85; struct Cyc_Absyn_ForArrayInfo _temp87;
struct Cyc_Absyn_Stmt* _temp89; struct _tuple2 _temp91; struct Cyc_Absyn_Stmt*
_temp93; struct Cyc_Absyn_Exp* _temp95; struct _tuple2 _temp97; struct Cyc_Absyn_Stmt*
_temp99; struct Cyc_Absyn_Exp* _temp101; struct Cyc_List_List* _temp103; struct
_tuple2 _temp105; struct Cyc_Absyn_Stmt* _temp107; struct Cyc_Absyn_Exp*
_temp109; struct Cyc_Absyn_Stmt* _temp111; struct Cyc_Absyn_Stmt* _temp113;
struct Cyc_Absyn_Stmt** _temp115; struct Cyc_Absyn_Stmt* _temp116; struct Cyc_Absyn_Stmt**
_temp118; struct Cyc_Absyn_Stmt* _temp119; struct Cyc_Absyn_Stmt** _temp121;
struct _tagged_arr* _temp122; struct Cyc_Absyn_Switch_clause** _temp124; struct
Cyc_Absyn_Switch_clause*** _temp126; struct Cyc_List_List* _temp127; struct Cyc_Absyn_Stmt*
_temp129; struct _tagged_arr* _temp131; struct Cyc_List_List* _temp133; struct
Cyc_Absyn_Exp* _temp135; struct Cyc_List_List* _temp137; struct Cyc_Absyn_Exp*
_temp139; struct Cyc_List_List* _temp141; struct Cyc_Absyn_Stmt* _temp143;
struct Cyc_Absyn_Stmt* _temp145; struct Cyc_Absyn_Decl* _temp147; struct Cyc_Absyn_Stmt*
_temp149; struct Cyc_Absyn_Vardecl* _temp151; struct Cyc_Absyn_Tvar* _temp153;
struct Cyc_Absyn_Stmt* _temp155; struct Cyc_Absyn_Stmt* _temp157; _LL7: if(
_temp5 == ( void*) Cyc_Absyn_Skip_s){ goto _LL8;} else{ goto _LL9;} _LL9: if((
unsigned int) _temp5 >  1u?*(( int*) _temp5) ==  Cyc_Absyn_Exp_s: 0){ _LL50:
_temp49=(( struct Cyc_Absyn_Exp_s_struct*) _temp5)->f1; goto _LL10;} else{ goto
_LL11;} _LL11: if(( unsigned int) _temp5 >  1u?*(( int*) _temp5) ==  Cyc_Absyn_Seq_s:
0){ _LL54: _temp53=(( struct Cyc_Absyn_Seq_s_struct*) _temp5)->f1; goto _LL52;
_LL52: _temp51=(( struct Cyc_Absyn_Seq_s_struct*) _temp5)->f2; goto _LL12;}
else{ goto _LL13;} _LL13: if(( unsigned int) _temp5 >  1u?*(( int*) _temp5) == 
Cyc_Absyn_Return_s: 0){ _LL56: _temp55=(( struct Cyc_Absyn_Return_s_struct*)
_temp5)->f1; goto _LL14;} else{ goto _LL15;} _LL15: if(( unsigned int) _temp5 > 
1u?*(( int*) _temp5) ==  Cyc_Absyn_IfThenElse_s: 0){ _LL62: _temp61=(( struct
Cyc_Absyn_IfThenElse_s_struct*) _temp5)->f1; goto _LL60; _LL60: _temp59=((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp5)->f2; goto _LL58; _LL58: _temp57=((
struct Cyc_Absyn_IfThenElse_s_struct*) _temp5)->f3; goto _LL16;} else{ goto
_LL17;} _LL17: if(( unsigned int) _temp5 >  1u?*(( int*) _temp5) ==  Cyc_Absyn_While_s:
0){ _LL66: _temp65=(( struct Cyc_Absyn_While_s_struct*) _temp5)->f1; _LL70:
_temp69= _temp65.f1; goto _LL68; _LL68: _temp67= _temp65.f2; goto _LL64; _LL64:
_temp63=(( struct Cyc_Absyn_While_s_struct*) _temp5)->f2; goto _LL18;} else{
goto _LL19;} _LL19: if(( unsigned int) _temp5 >  1u?*(( int*) _temp5) ==  Cyc_Absyn_For_s:
0){ _LL86: _temp85=(( struct Cyc_Absyn_For_s_struct*) _temp5)->f1; goto _LL80;
_LL80: _temp79=(( struct Cyc_Absyn_For_s_struct*) _temp5)->f2; _LL84: _temp83=
_temp79.f1; goto _LL82; _LL82: _temp81= _temp79.f2; goto _LL74; _LL74: _temp73=((
struct Cyc_Absyn_For_s_struct*) _temp5)->f3; _LL78: _temp77= _temp73.f1; goto
_LL76; _LL76: _temp75= _temp73.f2; goto _LL72; _LL72: _temp71=(( struct Cyc_Absyn_For_s_struct*)
_temp5)->f4; goto _LL20;} else{ goto _LL21;} _LL21: if(( unsigned int) _temp5 > 
1u?*(( int*) _temp5) ==  Cyc_Absyn_ForArray_s: 0){ _LL88: _temp87=(( struct Cyc_Absyn_ForArray_s_struct*)
_temp5)->f1; _LL104: _temp103= _temp87.defns; goto _LL98; _LL98: _temp97=
_temp87.condition; _LL102: _temp101= _temp97.f1; goto _LL100; _LL100: _temp99=
_temp97.f2; goto _LL92; _LL92: _temp91= _temp87.delta; _LL96: _temp95= _temp91.f1;
goto _LL94; _LL94: _temp93= _temp91.f2; goto _LL90; _LL90: _temp89= _temp87.body;
goto _LL22;} else{ goto _LL23;} _LL23: if(( unsigned int) _temp5 >  1u?*(( int*)
_temp5) ==  Cyc_Absyn_Do_s: 0){ _LL112: _temp111=(( struct Cyc_Absyn_Do_s_struct*)
_temp5)->f1; goto _LL106; _LL106: _temp105=(( struct Cyc_Absyn_Do_s_struct*)
_temp5)->f2; _LL110: _temp109= _temp105.f1; goto _LL108; _LL108: _temp107=
_temp105.f2; goto _LL24;} else{ goto _LL25;} _LL25: if(( unsigned int) _temp5 > 
1u?*(( int*) _temp5) ==  Cyc_Absyn_Break_s: 0){ _LL114: _temp113=(( struct Cyc_Absyn_Break_s_struct*)
_temp5)->f1; _temp115=( struct Cyc_Absyn_Stmt**)&(( struct Cyc_Absyn_Break_s_struct*)
_temp5)->f1; goto _LL26;} else{ goto _LL27;} _LL27: if(( unsigned int) _temp5 > 
1u?*(( int*) _temp5) ==  Cyc_Absyn_Continue_s: 0){ _LL117: _temp116=(( struct
Cyc_Absyn_Continue_s_struct*) _temp5)->f1; _temp118=( struct Cyc_Absyn_Stmt**)&((
struct Cyc_Absyn_Continue_s_struct*) _temp5)->f1; goto _LL28;} else{ goto _LL29;}
_LL29: if(( unsigned int) _temp5 >  1u?*(( int*) _temp5) ==  Cyc_Absyn_Goto_s: 0){
_LL123: _temp122=(( struct Cyc_Absyn_Goto_s_struct*) _temp5)->f1; goto _LL120;
_LL120: _temp119=(( struct Cyc_Absyn_Goto_s_struct*) _temp5)->f2; _temp121=(
struct Cyc_Absyn_Stmt**)&(( struct Cyc_Absyn_Goto_s_struct*) _temp5)->f2; goto
_LL30;} else{ goto _LL31;} _LL31: if(( unsigned int) _temp5 >  1u?*(( int*)
_temp5) ==  Cyc_Absyn_Fallthru_s: 0){ _LL128: _temp127=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp5)->f1; goto _LL125; _LL125: _temp124=(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp5)->f2; _temp126=( struct Cyc_Absyn_Switch_clause***)&(( struct Cyc_Absyn_Fallthru_s_struct*)
_temp5)->f2; goto _LL32;} else{ goto _LL33;} _LL33: if(( unsigned int) _temp5 > 
1u?*(( int*) _temp5) ==  Cyc_Absyn_Label_s: 0){ _LL132: _temp131=(( struct Cyc_Absyn_Label_s_struct*)
_temp5)->f1; goto _LL130; _LL130: _temp129=(( struct Cyc_Absyn_Label_s_struct*)
_temp5)->f2; goto _LL34;} else{ goto _LL35;} _LL35: if(( unsigned int) _temp5 > 
1u?*(( int*) _temp5) ==  Cyc_Absyn_Switch_s: 0){ _LL136: _temp135=(( struct Cyc_Absyn_Switch_s_struct*)
_temp5)->f1; goto _LL134; _LL134: _temp133=(( struct Cyc_Absyn_Switch_s_struct*)
_temp5)->f2; goto _LL36;} else{ goto _LL37;} _LL37: if(( unsigned int) _temp5 > 
1u?*(( int*) _temp5) ==  Cyc_Absyn_SwitchC_s: 0){ _LL140: _temp139=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp5)->f1; goto _LL138; _LL138: _temp137=(( struct Cyc_Absyn_SwitchC_s_struct*)
_temp5)->f2; goto _LL38;} else{ goto _LL39;} _LL39: if(( unsigned int) _temp5 > 
1u?*(( int*) _temp5) ==  Cyc_Absyn_TryCatch_s: 0){ _LL144: _temp143=(( struct
Cyc_Absyn_TryCatch_s_struct*) _temp5)->f1; goto _LL142; _LL142: _temp141=((
struct Cyc_Absyn_TryCatch_s_struct*) _temp5)->f2; goto _LL40;} else{ goto _LL41;}
_LL41: if(( unsigned int) _temp5 >  1u?*(( int*) _temp5) ==  Cyc_Absyn_Decl_s: 0){
_LL148: _temp147=(( struct Cyc_Absyn_Decl_s_struct*) _temp5)->f1; goto _LL146;
_LL146: _temp145=(( struct Cyc_Absyn_Decl_s_struct*) _temp5)->f2; goto _LL42;}
else{ goto _LL43;} _LL43: if(( unsigned int) _temp5 >  1u?*(( int*) _temp5) == 
Cyc_Absyn_Region_s: 0){ _LL154: _temp153=(( struct Cyc_Absyn_Region_s_struct*)
_temp5)->f1; goto _LL152; _LL152: _temp151=(( struct Cyc_Absyn_Region_s_struct*)
_temp5)->f2; goto _LL150; _LL150: _temp149=(( struct Cyc_Absyn_Region_s_struct*)
_temp5)->f3; goto _LL44;} else{ goto _LL45;} _LL45: if(( unsigned int) _temp5 > 
1u?*(( int*) _temp5) ==  Cyc_Absyn_Cut_s: 0){ _LL156: _temp155=(( struct Cyc_Absyn_Cut_s_struct*)
_temp5)->f1; goto _LL46;} else{ goto _LL47;} _LL47: if(( unsigned int) _temp5 > 
1u?*(( int*) _temp5) ==  Cyc_Absyn_Splice_s: 0){ _LL158: _temp157=(( struct Cyc_Absyn_Splice_s_struct*)
_temp5)->f1; goto _LL48;} else{ goto _LL6;} _LL8: return; _LL10: Cyc_Tcexp_tcExp(
te, 0, _temp49); return; _LL12: { struct Cyc_Tcenv_Tenv* _temp159= Cyc_Tcenv_set_next(
te,( void*)({ struct Cyc_Tcenv_Stmt_j_struct* _temp172=( struct Cyc_Tcenv_Stmt_j_struct*)
_cycalloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct)); _temp172[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct
_temp173; _temp173.tag= Cyc_Tcenv_Stmt_j; _temp173.f1= _temp51; _temp173;});
_temp172;})); Cyc_Tcstmt_tcStmt( _temp159, _temp53, 1); while( 1) { void*
_temp160=( void*) _temp51->r; struct Cyc_Absyn_Stmt* _temp166; struct Cyc_Absyn_Stmt*
_temp168; _LL162: if(( unsigned int) _temp160 >  1u?*(( int*) _temp160) ==  Cyc_Absyn_Seq_s:
0){ _LL169: _temp168=(( struct Cyc_Absyn_Seq_s_struct*) _temp160)->f1; goto
_LL167; _LL167: _temp166=(( struct Cyc_Absyn_Seq_s_struct*) _temp160)->f2; goto
_LL163;} else{ goto _LL164;} _LL164: goto _LL165; _LL163: Cyc_Tcstmt_decorate_stmt(
te, _temp51); _temp159= Cyc_Tcenv_set_next( te,( void*)({ struct Cyc_Tcenv_Stmt_j_struct*
_temp170=( struct Cyc_Tcenv_Stmt_j_struct*) _cycalloc( sizeof( struct Cyc_Tcenv_Stmt_j_struct));
_temp170[ 0]=({ struct Cyc_Tcenv_Stmt_j_struct _temp171; _temp171.tag= Cyc_Tcenv_Stmt_j;
_temp171.f1= _temp166; _temp171;}); _temp170;})); Cyc_Tcstmt_tcStmt( _temp159,
_temp168, 1); _temp51= _temp166; continue; _LL165: goto seq_end; _LL161:;}
seq_end: Cyc_Tcstmt_tcStmt( te, _temp51, 1); return;} _LL14: { void* t= Cyc_Tcenv_return_typ(
te); if( _temp55 ==  0){ void* _temp174= Cyc_Tcutil_compress( t); _LL176: if(
_temp174 == ( void*) Cyc_Absyn_VoidType){ goto _LL177;} else{ goto _LL178;}
_LL178: goto _LL179; _LL177: goto _LL175; _LL179:({ struct Cyc_Std_String_pa_struct
_temp181; _temp181.tag= Cyc_Std_String_pa; _temp181.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp180[ 1u]={& _temp181}; Cyc_Tcutil_terr( s0->loc, _tag_arr("must return a value of type %s",
sizeof( unsigned char), 31u), _tag_arr( _temp180, sizeof( void*), 1u));}}); goto
_LL175; _LL175:;} else{ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*)
_check_null( _temp55); Cyc_Tcexp_tcExp( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg(
te, e, t)){({ struct Cyc_Std_String_pa_struct _temp184; _temp184.tag= Cyc_Std_String_pa;
_temp184.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ struct Cyc_Std_String_pa_struct
_temp183; _temp183.tag= Cyc_Std_String_pa; _temp183.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp182[ 2u]={&
_temp183,& _temp184}; Cyc_Tcutil_terr( s0->loc, _tag_arr("returns value of type %s but requires %s",
sizeof( unsigned char), 41u), _tag_arr( _temp182, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}}
return;} _LL16: Cyc_Tcexp_tcTest( te, _temp61, _tag_arr("if statement", sizeof(
unsigned char), 13u)); Cyc_Tcstmt_tcStmt( te, _temp59, 1); Cyc_Tcstmt_tcStmt( te,
_temp57, 1); return; _LL18: Cyc_Tcstmt_decorate_stmt( te, _temp67); Cyc_Tcexp_tcTest(
te, _temp69, _tag_arr("while loop", sizeof( unsigned char), 11u)); Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_in_loop( te, _temp67), _temp63, 1); return; _LL20: Cyc_Tcstmt_decorate_stmt(
te, _temp81); Cyc_Tcstmt_decorate_stmt( te, _temp75); Cyc_Tcexp_tcExp( te, 0,
_temp85); Cyc_Tcexp_tcTest( te, _temp83, _tag_arr("for loop", sizeof(
unsigned char), 9u)); te= Cyc_Tcenv_set_in_loop( te, _temp75); Cyc_Tcstmt_tcStmt(
te, _temp71, 1); Cyc_Tcexp_tcExp( te, 0, _temp77); return; _LL22:({ void*
_temp185[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("ForArray_s", sizeof( unsigned char), 11u), _tag_arr( _temp185, sizeof(
void*), 0u));}); return; _LL24: Cyc_Tcstmt_decorate_stmt( te, _temp107); Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_in_loop( te, _temp107), _temp111, 1); Cyc_Tcexp_tcTest( te,
_temp109, _tag_arr("do loop", sizeof( unsigned char), 8u)); return; _LL26: Cyc_Tcenv_process_break(
te, s0, _temp115); return; _LL28: Cyc_Tcenv_process_continue( te, s0, _temp118);
return; _LL30: Cyc_Tcenv_process_goto( te, s0, _temp122, _temp121); return;
_LL32: { struct _tuple3* _temp186= Cyc_Tcenv_process_fallthru( te, s0, _temp126);
if( _temp186 ==  0){({ void* _temp187[ 0u]={}; Cyc_Tcutil_terr( s0->loc,
_tag_arr("fallthru not in a non-last case", sizeof( unsigned char), 32u),
_tag_arr( _temp187, sizeof( void*), 0u));}); return;}{ struct _tuple3 _temp190;
struct Cyc_List_List* _temp191; struct Cyc_List_List* _temp193; struct _tuple3*
_temp188=( struct _tuple3*) _check_null( _temp186); _temp190=* _temp188; _LL194:
_temp193= _temp190.f2; goto _LL192; _LL192: _temp191= _temp190.f3; goto _LL189;
_LL189: { struct Cyc_List_List* instantiation=(( struct Cyc_List_List*(*)(
struct _tuple4*(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Tvar*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Tcutil_make_inst_var, Cyc_Tcenv_lookup_type_vars(
te), _temp193); _temp191=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_List_List*,
void*), struct Cyc_List_List* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Tcutil_substitute, instantiation, _temp191); for( 0; _temp191 !=  0?
_temp127 !=  0: 0;( _temp191= _temp191->tl, _temp127= _temp127->tl)){ Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*) _temp127->hd); if( ! Cyc_Tcutil_coerce_arg( te,(
struct Cyc_Absyn_Exp*) _temp127->hd,( void*) _temp191->hd)){({ struct Cyc_Std_String_pa_struct
_temp197; _temp197.tag= Cyc_Std_String_pa; _temp197.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*) _temp191->hd);{ struct Cyc_Std_String_pa_struct _temp196; _temp196.tag=
Cyc_Std_String_pa; _temp196.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*) _temp127->hd)->topt))->v);{
void* _temp195[ 2u]={& _temp196,& _temp197}; Cyc_Tcutil_terr( s0->loc, _tag_arr("fallthru argument has type %s but pattern variable has type %s",
sizeof( unsigned char), 63u), _tag_arr( _temp195, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}}
if( _temp127 !=  0){({ void* _temp198[ 0u]={}; Cyc_Tcutil_terr( s0->loc,
_tag_arr("too many arguments to explicit fallthru", sizeof( unsigned char), 40u),
_tag_arr( _temp198, sizeof( void*), 0u));});} if( _temp191 !=  0){({ void*
_temp199[ 0u]={}; Cyc_Tcutil_terr( s0->loc, _tag_arr("too few arguments to explicit fallthru",
sizeof( unsigned char), 39u), _tag_arr( _temp199, sizeof( void*), 0u));});}
return;}}} _LL34: Cyc_Tcstmt_tcStmt( Cyc_Tcenv_new_named_block( s0->loc, Cyc_Tcenv_add_label(
te, _temp131, _temp129),({ struct Cyc_Absyn_Tvar* _temp200=( struct Cyc_Absyn_Tvar*)
_cycalloc( sizeof( struct Cyc_Absyn_Tvar)); _temp200->name=({ struct _tagged_arr*
_temp203=( struct _tagged_arr*) _cycalloc( sizeof( struct _tagged_arr));
_temp203[ 0]=( struct _tagged_arr)({ struct Cyc_Std_String_pa_struct _temp205;
_temp205.tag= Cyc_Std_String_pa; _temp205.f1=( struct _tagged_arr)* _temp131;{
void* _temp204[ 1u]={& _temp205}; Cyc_Std_aprintf( _tag_arr("`%s", sizeof(
unsigned char), 4u), _tag_arr( _temp204, sizeof( void*), 1u));}}); _temp203;});
_temp200->identity= Cyc_Tcutil_new_tvar_id(); _temp200->kind=( void*)(( void*)({
struct Cyc_Absyn_Eq_kb_struct* _temp201=( struct Cyc_Absyn_Eq_kb_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_kb_struct)); _temp201[ 0]=({ struct Cyc_Absyn_Eq_kb_struct
_temp202; _temp202.tag= Cyc_Absyn_Eq_kb; _temp202.f1=( void*)(( void*) Cyc_Absyn_RgnKind);
_temp202;}); _temp201;})); _temp200;})), _temp129, 0); return; _LL36: Cyc_Tcexp_tcExp(
te, 0, _temp135);{ void* _temp206=( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp135->topt))->v; te= Cyc_Tcenv_set_in_switch( te); te= Cyc_Tcenv_clear_fallthru(
te);{ struct Cyc_List_List* scs=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp133); for( 0; scs !=  0; scs= scs->tl){ struct Cyc_Absyn_Pat*
_temp207=(( struct Cyc_Absyn_Switch_clause*) scs->hd)->pattern; struct Cyc_List_List*
_temp210; struct Cyc_List_List* _temp212; struct _tuple5 _temp208= Cyc_Tcpat_tcPat(
te, _temp207,( void**)& _temp206, 0); _LL213: _temp212= _temp208.f1; goto _LL211;
_LL211: _temp210= _temp208.f2; goto _LL209; _LL209: if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp207->topt))->v, _temp206)){({
struct Cyc_Std_String_pa_struct _temp216; _temp216.tag= Cyc_Std_String_pa;
_temp216.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp207->topt))->v);{ struct Cyc_Std_String_pa_struct _temp215;
_temp215.tag= Cyc_Std_String_pa; _temp215.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp206);{ void* _temp214[ 2u]={& _temp215,& _temp216}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Switch_clause*) scs->hd)->loc, _tag_arr("switch on type %s, but case expects type %s",
sizeof( unsigned char), 44u), _tag_arr( _temp214, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
else{ Cyc_Tcpat_check_pat_regions( te, _temp207);}(( struct Cyc_Absyn_Switch_clause*)
scs->hd)->pat_vars=({ struct Cyc_Core_Opt* _temp217=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp217->v=( void*) _temp210;
_temp217;}); Cyc_Tcstmt_pattern_synth((( struct Cyc_Absyn_Switch_clause*) scs->hd)->loc,
te, _temp212, _temp210,(( struct Cyc_Absyn_Switch_clause*) scs->hd)->body,((
struct Cyc_Absyn_Switch_clause*) scs->hd)->where_clause, 1); te= Cyc_Tcenv_set_fallthru(
te, _temp212, _temp210,( struct Cyc_Absyn_Switch_clause*) scs->hd);}} Cyc_Tcpat_check_switch_exhaustive(
s0->loc, _temp133); return;} _LL38: { struct Cyc_List_List* _temp218= 0; for( 0;
_temp137 !=  0; _temp137= _temp137->tl){ struct Cyc_Absyn_SwitchC_clause
_temp221; struct Cyc_Position_Segment* _temp222; struct Cyc_Absyn_Stmt* _temp224;
struct Cyc_Absyn_Exp* _temp226; struct Cyc_Absyn_SwitchC_clause* _temp219=(
struct Cyc_Absyn_SwitchC_clause*) _temp137->hd; _temp221=* _temp219; _LL227:
_temp226= _temp221.cnst_exp; goto _LL225; _LL225: _temp224= _temp221.body; goto
_LL223; _LL223: _temp222= _temp221.loc; goto _LL220; _LL220: { struct Cyc_Absyn_Pat*
pat; if( _temp226 !=  0){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_check_null( _temp226)); pat= Cyc_Absyn_new_pat(( void*)({ struct Cyc_Absyn_Int_p_struct*
_temp228=( struct Cyc_Absyn_Int_p_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_p_struct));
_temp228[ 0]=({ struct Cyc_Absyn_Int_p_struct _temp229; _temp229.tag= Cyc_Absyn_Int_p;
_temp229.f1=( void*)(( void*) Cyc_Absyn_Signed); _temp229.f2=( int) Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*) _check_null( _temp226)); _temp229;}); _temp228;}),
_temp226->loc);} else{ pat= Cyc_Absyn_new_pat(( void*) Cyc_Absyn_Wild_p,
_temp222);} _temp218=({ struct Cyc_List_List* _temp230=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp230->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp231=( struct Cyc_Absyn_Switch_clause*) _cycalloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp231->pattern= pat; _temp231->pat_vars= 0; _temp231->where_clause= 0;
_temp231->body= _temp224; _temp231->loc= _temp222; _temp231;}); _temp230->tl=
_temp218; _temp230;});}}( void*)( s0->r=( void*)(( void*)( Cyc_Absyn_switch_stmt(
_temp139,(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
_temp218), 0))->r)); Cyc_Tcstmt_tcStmt( te, s0, new_block); return;} _LL40:(
void*)( _temp143->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Absyn_new_stmt((
void*) _temp143->r, _temp143->loc), Cyc_Absyn_skip_stmt( _temp143->loc),
_temp143->loc))->r)); Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( Cyc_Tcenv_enter_try(
te), s0), _temp143, 1); te= Cyc_Tcenv_set_in_switch( te); te= Cyc_Tcenv_clear_fallthru(
te);{ struct Cyc_List_List* _temp232=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_rev)( _temp141); for( 0; _temp232 !=  0; _temp232= _temp232->tl){
struct Cyc_Absyn_Pat* _temp233=(( struct Cyc_Absyn_Switch_clause*) _temp232->hd)->pattern;
struct Cyc_List_List* _temp236; struct Cyc_List_List* _temp238; struct _tuple5
_temp234= Cyc_Tcpat_tcPat( te, _temp233,( void**)& Cyc_Absyn_exn_typ, 0); _LL239:
_temp238= _temp234.f1; goto _LL237; _LL237: _temp236= _temp234.f2; goto _LL235;
_LL235: if( ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp233->topt))->v, Cyc_Absyn_exn_typ)){({ struct Cyc_Std_String_pa_struct
_temp241; _temp241.tag= Cyc_Std_String_pa; _temp241.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp233->topt))->v);{ void*
_temp240[ 1u]={& _temp241}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Switch_clause*)
_temp232->hd)->loc, _tag_arr("expected xtunion exn but found %s", sizeof(
unsigned char), 34u), _tag_arr( _temp240, sizeof( void*), 1u));}});} else{ Cyc_Tcpat_check_pat_regions(
te, _temp233);}(( struct Cyc_Absyn_Switch_clause*) _temp232->hd)->pat_vars=({
struct Cyc_Core_Opt* _temp242=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp242->v=( void*) _temp236; _temp242;}); Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*) _temp232->hd)->loc, te, _temp238, _temp236,((
struct Cyc_Absyn_Switch_clause*) _temp232->hd)->body,(( struct Cyc_Absyn_Switch_clause*)
_temp232->hd)->where_clause, 1); te= Cyc_Tcenv_set_fallthru( te, _temp238,
_temp236,( struct Cyc_Absyn_Switch_clause*) _temp232->hd);}} Cyc_Tcpat_check_catch_overlap(
s0->loc, _temp141); return; _LL42: { struct _tagged_arr unimp_msg_part; if(
new_block){ te= Cyc_Tcenv_new_block( s0->loc, te);}{ void* _temp243=( void*)
_temp147->r; struct Cyc_Absyn_Vardecl* _temp269; int _temp271; int* _temp273;
struct Cyc_Absyn_Exp* _temp274; struct Cyc_Core_Opt* _temp276; struct Cyc_Core_Opt**
_temp278; struct Cyc_Core_Opt* _temp279; struct Cyc_Core_Opt** _temp281; struct
Cyc_Absyn_Pat* _temp282; struct Cyc_List_List* _temp284; struct Cyc_List_List*
_temp286; struct _tagged_arr* _temp288; struct Cyc_List_List* _temp290; struct
_tuple0* _temp292; _LL245: if(*(( int*) _temp243) ==  Cyc_Absyn_Var_d){ _LL270:
_temp269=(( struct Cyc_Absyn_Var_d_struct*) _temp243)->f1; goto _LL246;} else{
goto _LL247;} _LL247: if(*(( int*) _temp243) ==  Cyc_Absyn_Let_d){ _LL283:
_temp282=(( struct Cyc_Absyn_Let_d_struct*) _temp243)->f1; goto _LL280; _LL280:
_temp279=(( struct Cyc_Absyn_Let_d_struct*) _temp243)->f2; _temp281=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Let_d_struct*) _temp243)->f2; goto _LL277; _LL277: _temp276=((
struct Cyc_Absyn_Let_d_struct*) _temp243)->f3; _temp278=( struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Let_d_struct*) _temp243)->f3; goto _LL275; _LL275: _temp274=((
struct Cyc_Absyn_Let_d_struct*) _temp243)->f4; goto _LL272; _LL272: _temp271=((
struct Cyc_Absyn_Let_d_struct*) _temp243)->f5; _temp273=( int*)&(( struct Cyc_Absyn_Let_d_struct*)
_temp243)->f5; goto _LL248;} else{ goto _LL249;} _LL249: if(*(( int*) _temp243)
==  Cyc_Absyn_Letv_d){ _LL285: _temp284=(( struct Cyc_Absyn_Letv_d_struct*)
_temp243)->f1; goto _LL250;} else{ goto _LL251;} _LL251: if(*(( int*) _temp243)
==  Cyc_Absyn_Namespace_d){ _LL289: _temp288=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp243)->f1; goto _LL287; _LL287: _temp286=(( struct Cyc_Absyn_Namespace_d_struct*)
_temp243)->f2; goto _LL252;} else{ goto _LL253;} _LL253: if(*(( int*) _temp243)
==  Cyc_Absyn_Using_d){ _LL293: _temp292=(( struct Cyc_Absyn_Using_d_struct*)
_temp243)->f1; goto _LL291; _LL291: _temp290=(( struct Cyc_Absyn_Using_d_struct*)
_temp243)->f2; goto _LL254;} else{ goto _LL255;} _LL255: if(*(( int*) _temp243)
==  Cyc_Absyn_Fn_d){ goto _LL256;} else{ goto _LL257;} _LL257: if(*(( int*)
_temp243) ==  Cyc_Absyn_Struct_d){ goto _LL258;} else{ goto _LL259;} _LL259: if(*((
int*) _temp243) ==  Cyc_Absyn_Union_d){ goto _LL260;} else{ goto _LL261;} _LL261:
if(*(( int*) _temp243) ==  Cyc_Absyn_Tunion_d){ goto _LL262;} else{ goto _LL263;}
_LL263: if(*(( int*) _temp243) ==  Cyc_Absyn_Enum_d){ goto _LL264;} else{ goto
_LL265;} _LL265: if(*(( int*) _temp243) ==  Cyc_Absyn_Typedef_d){ goto _LL266;}
else{ goto _LL267;} _LL267: if(*(( int*) _temp243) ==  Cyc_Absyn_ExternC_d){
goto _LL268;} else{ goto _LL244;} _LL246: { struct Cyc_Absyn_Vardecl _temp296;
struct Cyc_List_List* _temp297; struct Cyc_Core_Opt* _temp299; struct Cyc_Core_Opt**
_temp301; struct Cyc_Absyn_Exp* _temp302; void* _temp304; struct Cyc_Absyn_Tqual
_temp306; struct _tuple0* _temp308; struct _tuple0 _temp310; struct _tagged_arr*
_temp311; void* _temp313; void* _temp315; struct Cyc_Absyn_Vardecl* _temp294=
_temp269; _temp296=* _temp294; _LL316: _temp315=( void*) _temp296.sc; goto
_LL309; _LL309: _temp308= _temp296.name; _temp310=* _temp308; _LL314: _temp313=
_temp310.f1; goto _LL312; _LL312: _temp311= _temp310.f2; goto _LL307; _LL307:
_temp306= _temp296.tq; goto _LL305; _LL305: _temp304=( void*) _temp296.type;
goto _LL303; _LL303: _temp302= _temp296.initializer; goto _LL300; _LL300:
_temp299= _temp296.rgn; _temp301=( struct Cyc_Core_Opt**)&(* _temp294).rgn; goto
_LL298; _LL298: _temp297= _temp296.attributes; goto _LL295; _LL295: { void*
_temp317= Cyc_Tcenv_curr_rgn( te); int is_local;{ void* _temp318= _temp315;
_LL320: if( _temp318 == ( void*) Cyc_Absyn_Static){ goto _LL321;} else{ goto
_LL322;} _LL322: if( _temp318 == ( void*) Cyc_Absyn_Extern){ goto _LL323;} else{
goto _LL324;} _LL324: if( _temp318 == ( void*) Cyc_Absyn_ExternC){ goto _LL325;}
else{ goto _LL326;} _LL326: if( _temp318 == ( void*) Cyc_Absyn_Abstract){ goto
_LL327;} else{ goto _LL328;} _LL328: if( _temp318 == ( void*) Cyc_Absyn_Public){
goto _LL329;} else{ goto _LL319;} _LL321: goto _LL323; _LL323: goto _LL325;
_LL325: is_local= 0; goto _LL319; _LL327:({ void* _temp330[ 0u]={}; Cyc_Tcutil_terr(
_temp147->loc, _tag_arr("bad abstract scope for local variable", sizeof(
unsigned char), 38u), _tag_arr( _temp330, sizeof( void*), 0u));}); goto _LL329;
_LL329: is_local= 1; goto _LL319; _LL319:;}* _temp301= is_local?({ struct Cyc_Core_Opt*
_temp331=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp331->v=( void*) _temp317; _temp331;}):({ struct Cyc_Core_Opt* _temp332=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp332->v=(
void*)(( void*) Cyc_Absyn_HeapRgn); _temp332;});{ void* _temp333= _temp313;
struct Cyc_List_List* _temp343; struct Cyc_List_List* _temp345; _LL335: if(
_temp333 == ( void*) Cyc_Absyn_Loc_n){ goto _LL336;} else{ goto _LL337;} _LL337:
if(( unsigned int) _temp333 >  1u?*(( int*) _temp333) ==  Cyc_Absyn_Rel_n: 0){
_LL344: _temp343=(( struct Cyc_Absyn_Rel_n_struct*) _temp333)->f1; if( _temp343
==  0){ goto _LL338;} else{ goto _LL339;}} else{ goto _LL339;} _LL339: if((
unsigned int) _temp333 >  1u?*(( int*) _temp333) ==  Cyc_Absyn_Abs_n: 0){ _LL346:
_temp345=(( struct Cyc_Absyn_Abs_n_struct*) _temp333)->f1; goto _LL340;} else{
goto _LL341;} _LL341: goto _LL342; _LL336: goto _LL334; _LL338:(* _temp269->name).f1=(
void*) Cyc_Absyn_Loc_n; goto _LL334; _LL340:( int) _throw(({ void* _temp347[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("tcstmt: Abs_n declaration", sizeof( unsigned char),
26u), _tag_arr( _temp347, sizeof( void*), 0u));})); _LL342:({ void* _temp348[ 0u]={};
Cyc_Tcutil_terr( _temp147->loc, _tag_arr("cannot declare a qualified local variable",
sizeof( unsigned char), 42u), _tag_arr( _temp348, sizeof( void*), 0u));}); goto
_LL334; _LL334:;}{ void* _temp349= Cyc_Tcutil_compress( _temp304); struct Cyc_Absyn_Exp*
_temp356; struct Cyc_Absyn_Tqual _temp358; void* _temp360; _LL351: if((
unsigned int) _temp349 >  3u?*(( int*) _temp349) ==  Cyc_Absyn_ArrayType: 0){
_LL361: _temp360=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp349)->f1;
goto _LL359; _LL359: _temp358=(( struct Cyc_Absyn_ArrayType_struct*) _temp349)->f2;
goto _LL357; _LL357: _temp356=(( struct Cyc_Absyn_ArrayType_struct*) _temp349)->f3;
if( _temp356 ==  0){ goto _LL355;} else{ goto _LL353;}} else{ goto _LL353;}
_LL355: if( _temp269->initializer !=  0){ goto _LL352;} else{ goto _LL353;}
_LL353: goto _LL354; _LL352:{ void* _temp362=( void*)(( struct Cyc_Absyn_Exp*)
_check_null( _temp269->initializer))->r; void* _temp374; struct _tagged_arr
_temp376; struct Cyc_Absyn_Exp* _temp378; struct Cyc_List_List* _temp380; struct
Cyc_List_List* _temp382; _LL364: if(*(( int*) _temp362) ==  Cyc_Absyn_Const_e){
_LL375: _temp374=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp362)->f1; if((
unsigned int) _temp374 >  1u?*(( int*) _temp374) ==  Cyc_Absyn_String_c: 0){
_LL377: _temp376=(( struct Cyc_Absyn_String_c_struct*) _temp374)->f1; goto
_LL365;} else{ goto _LL366;}} else{ goto _LL366;} _LL366: if(*(( int*) _temp362)
==  Cyc_Absyn_Comprehension_e){ _LL379: _temp378=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp362)->f2; goto _LL367;} else{ goto _LL368;} _LL368: if(*(( int*) _temp362)
==  Cyc_Absyn_UnresolvedMem_e){ _LL381: _temp380=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp362)->f2; goto _LL369;} else{ goto _LL370;} _LL370: if(*(( int*) _temp362)
==  Cyc_Absyn_Array_e){ _LL383: _temp382=(( struct Cyc_Absyn_Array_e_struct*)
_temp362)->f1; goto _LL371;} else{ goto _LL372;} _LL372: goto _LL373; _LL365:
_temp304=( void*)( _temp269->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp384=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp384[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp385; _temp385.tag= Cyc_Absyn_ArrayType;
_temp385.f1=( void*) _temp360; _temp385.f2= _temp358; _temp385.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( _get_arr_size( _temp376, sizeof( unsigned char)), 0);
_temp385;}); _temp384;}))); goto _LL363; _LL367: _temp304=( void*)( _temp269->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp386=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp386[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp387; _temp387.tag= Cyc_Absyn_ArrayType; _temp387.f1=(
void*) _temp360; _temp387.f2= _temp358; _temp387.f3=( struct Cyc_Absyn_Exp*)
_temp378; _temp387;}); _temp386;}))); goto _LL363; _LL369: _temp382= _temp380;
goto _LL371; _LL371: _temp304=( void*)( _temp269->type=( void*)(( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp388=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp388[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp389; _temp389.tag= Cyc_Absyn_ArrayType; _temp389.f1=(
void*) _temp360; _temp389.f2= _temp358; _temp389.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp((
unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp382), 0);
_temp389;}); _temp388;}))); goto _LL363; _LL373: goto _LL363; _LL363:;} goto
_LL350; _LL354: goto _LL350; _LL350:;}{ struct Cyc_List_List* _temp390= !
is_local? 0: Cyc_Tcenv_lookup_type_vars( te); int _temp391= ! is_local? 0: 1;
Cyc_Tcutil_check_type( s0->loc, te, _temp390,( void*) Cyc_Absyn_MemKind,
_temp391, _temp304);{ struct Cyc_Tcenv_Tenv* _temp392= Cyc_Tcenv_add_local_var(
_temp145->loc, te, _temp269); if( _temp315 == ( void*) Cyc_Absyn_Extern? 1:
_temp315 == ( void*) Cyc_Absyn_ExternC){({ void* _temp393[ 0u]={}; Cyc_Tcutil_terr(
_temp147->loc, _tag_arr("local extern declarations not yet supported", sizeof(
unsigned char), 44u), _tag_arr( _temp393, sizeof( void*), 0u));});} if( _temp302
!=  0){ Cyc_Tcexp_tcExpInitializer( _temp392,( void**)& _temp304,( struct Cyc_Absyn_Exp*)
_check_null( _temp302)); if( ! is_local? ! Cyc_Tcutil_is_const_exp( te,( struct
Cyc_Absyn_Exp*) _check_null( _temp302)): 0){({ void* _temp394[ 0u]={}; Cyc_Tcutil_terr(
_temp147->loc, _tag_arr("initializer needs to be a constant expression", sizeof(
unsigned char), 46u), _tag_arr( _temp394, sizeof( void*), 0u));});} if( ! Cyc_Tcutil_coerce_assign(
_temp392,( struct Cyc_Absyn_Exp*) _check_null( _temp302), _temp304)){({ struct
Cyc_Std_String_pa_struct _temp398; _temp398.tag= Cyc_Std_String_pa; _temp398.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp302->topt))->v);{ struct Cyc_Std_String_pa_struct _temp397;
_temp397.tag= Cyc_Std_String_pa; _temp397.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp304);{ struct Cyc_Std_String_pa_struct _temp396; _temp396.tag= Cyc_Std_String_pa;
_temp396.f1=( struct _tagged_arr)* _temp311;{ void* _temp395[ 3u]={& _temp396,&
_temp397,& _temp398}; Cyc_Tcutil_terr( _temp147->loc, _tag_arr("%s declared with type \n%s\n but initialized with type \n%s",
sizeof( unsigned char), 57u), _tag_arr( _temp395, sizeof( void*), 3u));}}}});
Cyc_Tcutil_explain_failure();}} Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
_temp392, s0), _temp145, 0); return;}}}} _LL248: { struct Cyc_Core_Opt* _temp399=*
_temp278; Cyc_Tcexp_tcExpInitializer( te, 0, _temp274);* _temp278= _temp274->topt;{
struct Cyc_List_List* _temp402; struct Cyc_List_List* _temp404; struct _tuple5
_temp400= Cyc_Tcpat_tcPat( te, _temp282,( void**)(( void**)(( void*)&(( struct
Cyc_Core_Opt*) _check_null( _temp274->topt))->v)), 0); _LL405: _temp404=
_temp400.f1; goto _LL403; _LL403: _temp402= _temp400.f2; goto _LL401; _LL401:*
_temp281=({ struct Cyc_Core_Opt* _temp406=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp406->v=( void*) _temp402; _temp406;}); if(
_temp399 !=  0? ! Cyc_Tcutil_unify(( void*)(( struct Cyc_Core_Opt*) _check_null(
_temp282->topt))->v,( void*) _temp399->v): 0){({ void* _temp407[ 0u]={}; Cyc_Tcutil_terr(
_temp147->loc, _tag_arr("type of pattern does not match declared type", sizeof(
unsigned char), 45u), _tag_arr( _temp407, sizeof( void*), 0u));});} if( ! Cyc_Tcutil_unify((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp282->topt))->v,( void*)((
struct Cyc_Core_Opt*) _check_null( _temp274->topt))->v)){({ struct Cyc_Std_String_pa_struct
_temp410; _temp410.tag= Cyc_Std_String_pa; _temp410.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp274->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp409; _temp409.tag= Cyc_Std_String_pa; _temp409.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp282->topt))->v);{ void*
_temp408[ 2u]={& _temp409,& _temp410}; Cyc_Tcutil_terr( _temp147->loc, _tag_arr("pattern type %s does not match definition type %s",
sizeof( unsigned char), 50u), _tag_arr( _temp408, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
else{ Cyc_Tcpat_check_pat_regions( te, _temp282);}* _temp273= Cyc_Tcpat_check_let_pat_exhaustive(
_temp282->loc, _temp282); Cyc_Tcstmt_pattern_synth( s0->loc, te, _temp404,
_temp402, _temp145, 0, 0); return;}} _LL250: { void* _temp411= Cyc_Tcenv_curr_rgn(
te); struct Cyc_Tcenv_Tenv* _temp412= te; for( 0; _temp284 !=  0; _temp284=
_temp284->tl){ struct Cyc_Absyn_Vardecl* _temp413=( struct Cyc_Absyn_Vardecl*)
_temp284->hd; struct Cyc_Absyn_Vardecl _temp416; struct Cyc_Core_Opt* _temp417;
struct Cyc_Core_Opt** _temp419; void* _temp420; struct _tuple0* _temp422; struct
_tuple0 _temp424; void* _temp425; struct Cyc_Absyn_Vardecl* _temp414= _temp413;
_temp416=* _temp414; _LL423: _temp422= _temp416.name; _temp424=* _temp422;
_LL426: _temp425= _temp424.f1; goto _LL421; _LL421: _temp420=( void*) _temp416.type;
goto _LL418; _LL418: _temp417= _temp416.rgn; _temp419=( struct Cyc_Core_Opt**)&(*
_temp414).rgn; goto _LL415; _LL415:* _temp419=({ struct Cyc_Core_Opt* _temp427=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp427->v=(
void*) _temp411; _temp427;});{ void* _temp428= _temp425; struct Cyc_List_List*
_temp438; struct Cyc_List_List* _temp440; _LL430: if( _temp428 == ( void*) Cyc_Absyn_Loc_n){
goto _LL431;} else{ goto _LL432;} _LL432: if(( unsigned int) _temp428 >  1u?*((
int*) _temp428) ==  Cyc_Absyn_Rel_n: 0){ _LL439: _temp438=(( struct Cyc_Absyn_Rel_n_struct*)
_temp428)->f1; if( _temp438 ==  0){ goto _LL433;} else{ goto _LL434;}} else{
goto _LL434;} _LL434: if(( unsigned int) _temp428 >  1u?*(( int*) _temp428) == 
Cyc_Absyn_Abs_n: 0){ _LL441: _temp440=(( struct Cyc_Absyn_Abs_n_struct*)
_temp428)->f1; goto _LL435;} else{ goto _LL436;} _LL436: goto _LL437; _LL431:
goto _LL429; _LL433:(* _temp413->name).f1=( void*) Cyc_Absyn_Loc_n; goto _LL429;
_LL435:( int) _throw(({ void* _temp442[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcstmt: Abs_n declaration",
sizeof( unsigned char), 26u), _tag_arr( _temp442, sizeof( void*), 0u));}));
_LL437:({ void* _temp443[ 0u]={}; Cyc_Tcutil_terr( _temp147->loc, _tag_arr("cannot declare a qualified local variable",
sizeof( unsigned char), 42u), _tag_arr( _temp443, sizeof( void*), 0u));}); goto
_LL429; _LL429:;} Cyc_Tcutil_check_type( s0->loc, _temp412, Cyc_Tcenv_lookup_type_vars(
_temp412),( void*) Cyc_Absyn_MemKind, 1, _temp420); _temp412= Cyc_Tcenv_add_local_var(
_temp145->loc, _temp412, _temp413);} Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
_temp412, s0), _temp145, 0); return;} _LL252: unimp_msg_part= _tag_arr("namespace",
sizeof( unsigned char), 10u); goto _LL244; _LL254: unimp_msg_part= _tag_arr("using",
sizeof( unsigned char), 6u); goto _LL244; _LL256: unimp_msg_part= _tag_arr("function",
sizeof( unsigned char), 9u); goto _LL244; _LL258: unimp_msg_part= _tag_arr("struct",
sizeof( unsigned char), 7u); goto _LL244; _LL260: unimp_msg_part= _tag_arr("union",
sizeof( unsigned char), 6u); goto _LL244; _LL262: unimp_msg_part= _tag_arr("[x]tunion",
sizeof( unsigned char), 10u); goto _LL244; _LL264: unimp_msg_part= _tag_arr("enum",
sizeof( unsigned char), 5u); goto _LL244; _LL266: unimp_msg_part= _tag_arr("typedef",
sizeof( unsigned char), 8u); goto _LL244; _LL268: unimp_msg_part= _tag_arr("extern \"C\"",
sizeof( unsigned char), 11u); goto _LL244; _LL244:;}( int) _throw(({ struct Cyc_Std_String_pa_struct
_temp445; _temp445.tag= Cyc_Std_String_pa; _temp445.f1=( struct _tagged_arr)
unimp_msg_part;{ void* _temp444[ 1u]={& _temp445}; Cyc_Tcutil_impos( _tag_arr("tcStmt: nested %s declarations unimplemented",
sizeof( unsigned char), 45u), _tag_arr( _temp444, sizeof( void*), 1u));}}));}
_LL44:( void*)( _temp149->r=( void*)(( void*)( Cyc_Absyn_seq_stmt( Cyc_Absyn_new_stmt((
void*) _temp149->r, _temp149->loc), Cyc_Absyn_skip_stmt( _temp149->loc),
_temp149->loc))->r)); if( new_block){ te= Cyc_Tcenv_new_block( s0->loc, te);}{
struct Cyc_Absyn_Vardecl _temp448; struct Cyc_Core_Opt* _temp449; struct Cyc_Core_Opt**
_temp451; void* _temp452; struct Cyc_Absyn_Vardecl* _temp446= _temp151; _temp448=*
_temp446; _LL453: _temp452=( void*) _temp448.type; goto _LL450; _LL450: _temp449=
_temp448.rgn; _temp451=( struct Cyc_Core_Opt**)&(* _temp446).rgn; goto _LL447;
_LL447: { void* _temp454= Cyc_Tcenv_curr_rgn( te); void* rgn_typ=( void*)({
struct Cyc_Absyn_VarType_struct* _temp460=( struct Cyc_Absyn_VarType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp460[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp461; _temp461.tag= Cyc_Absyn_VarType; _temp461.f1= _temp153; _temp461;});
_temp460;});* _temp451=({ struct Cyc_Core_Opt* _temp455=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp455->v=( void*) _temp454;
_temp455;}); te= Cyc_Tcenv_add_type_vars( s0->loc, te,({ struct Cyc_List_List*
_temp456=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp456->hd=( void*) _temp153; _temp456->tl= 0; _temp456;})); Cyc_Tcutil_check_type(
s0->loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_BoxKind, 1,
_temp452); te= Cyc_Tcenv_add_region( te, rgn_typ); if( ! Cyc_Tcutil_unify(
_temp452,( void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp457=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp457[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp458; _temp458.tag= Cyc_Absyn_RgnHandleType;
_temp458.f1=( void*) rgn_typ; _temp458;}); _temp457;}))){({ void* _temp459[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("region stmt: type of region handle is wrong!", sizeof( unsigned char),
45u), _tag_arr( _temp459, sizeof( void*), 0u));});} Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser(
Cyc_Tcenv_add_local_var( _temp149->loc, Cyc_Tcenv_enter_try( te), _temp151), s0),
_temp149, 0); return;}} _LL46:( int) _throw(({ void* _temp462[ 0u]={}; Cyc_Tcutil_impos(
_tag_arr("tcStmt: cut is not implemented", sizeof( unsigned char), 31u),
_tag_arr( _temp462, sizeof( void*), 0u));})); _LL48:( int) _throw(({ void*
_temp463[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcStmt: splice is not implemented",
sizeof( unsigned char), 34u), _tag_arr( _temp463, sizeof( void*), 0u));})); _LL6:;}}
