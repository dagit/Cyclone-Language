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
fds_bits[ 2u]; } ; struct Cyc_Core_Opt{ void* v; } ; struct _tuple0{ void* f1;
void* f2; } ; extern void* Cyc_Core_snd( struct _tuple0*); extern unsigned char
Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region;
extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, unsigned int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{ void* hd; struct Cyc_List_List*
tl; } ; extern int Cyc_List_length( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_rmap_c( struct _RegionHandle*, void*(* f)( void*, void*), void* env,
struct Cyc_List_List* x); extern unsigned char Cyc_List_List_mismatch[ 18u];
extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_rappend( struct _RegionHandle*, struct
Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u];
extern void* Cyc_List_nth( struct Cyc_List_List* x, int n); extern struct Cyc_List_List*
Cyc_List_nth_tail( struct Cyc_List_List* x, int i); extern int Cyc_List_exists_c(
int(* pred)( void*, void*), void* env, struct Cyc_List_List* x); extern struct
Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List* y);
extern struct Cyc_List_List* Cyc_List_rzip( struct _RegionHandle* r1, struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y); struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file; struct _tagged_arr line; int line_no; int col;
} ; extern unsigned char Cyc_Position_Exit[ 9u]; struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; static
const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; struct
_tuple1{ void* f1; struct _tagged_arr* f2; } ; struct Cyc_Absyn_Conref; static
const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract= 1; static
const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3; static
const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1; int
q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0; static
const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const int
Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
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
topt; void* r; struct Cyc_Position_Segment* loc; void* annot; } ; static const
int Cyc_Absyn_Skip_s= 0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1;
struct Cyc_Absyn_Seq_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_IfThenElse_s=
3; struct Cyc_Absyn_IfThenElse_s_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt* f3; } ; static const int Cyc_Absyn_While_s=
4; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ;
struct Cyc_Absyn_While_s_struct{ int tag; struct _tuple3 f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Break_s= 5; struct Cyc_Absyn_Break_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Continue_s= 6;
struct Cyc_Absyn_Continue_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Goto_s= 7; struct Cyc_Absyn_Goto_s_struct{ int tag;
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_For_s=
8; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt* f4; } ; static const int
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
tag; struct Cyc_Absyn_Stmt* f1; struct _tuple3 f2; } ; static const int Cyc_Absyn_TryCatch_s=
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
15u]; extern struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(); extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_def(
void*, struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar( struct Cyc_Core_Opt*
k, struct Cyc_Core_Opt* tenv); extern void* Cyc_Absyn_wildtyp( struct Cyc_Core_Opt*);
extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_ushort_t; extern void*
Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulonglong_t; extern void* Cyc_Absyn_schar_t;
extern void* Cyc_Absyn_sshort_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slonglong_t;
extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_exn_typ; extern void* Cyc_Absyn_atb_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq, void* b); extern void* Cyc_Absyn_star_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_at_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp(
void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_List_List*, struct
_tagged_arr*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_arr*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_arr*);
extern int Cyc_Std_strcmp( struct _tagged_arr s1, struct _tagged_arr s2); extern
int Cyc_Std_zstrcmp( struct _tagged_arr, struct _tagged_arr); struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE; extern unsigned char Cyc_Std_FileCloseError[ 19u];
extern unsigned char Cyc_Std_FileOpenError[ 18u]; struct Cyc_Std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_Std_String_pa=
0; struct Cyc_Std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Std_Int_pa= 1; struct Cyc_Std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_Std_Double_pa= 2; struct Cyc_Std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_Std_ShortPtr_pa= 3; struct Cyc_Std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_Std_IntPtr_pa= 4; struct Cyc_Std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern struct _tagged_arr Cyc_Std_vrprintf(
struct _RegionHandle* r1, struct _tagged_arr fmt, struct _tagged_arr ap); static
const int Cyc_Std_ShortPtr_sa= 0; struct Cyc_Std_ShortPtr_sa_struct{ int tag;
short* f1; } ; static const int Cyc_Std_UShortPtr_sa= 1; struct Cyc_Std_UShortPtr_sa_struct{
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
int use_curr_namespace: 1; struct Cyc_List_List* curr_namespace; } ; extern
struct _tagged_arr Cyc_Absynpp_typ2string( void*); extern struct _tagged_arr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*); extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple1*); struct Cyc_Set_Set; extern unsigned char Cyc_Set_Absent[ 11u];
struct Cyc_Dict_Dict; extern unsigned char Cyc_Dict_Present[ 12u]; extern
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
extern void* Cyc_Tcenv_lookup_ordinary( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tuple1*); extern struct Cyc_Absyn_Structdecl** Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple1*); extern
struct Cyc_Absyn_Tuniondecl** Cyc_Tcenv_lookup_tuniondecl( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, struct _tuple1*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*); extern struct Cyc_List_List* Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Absyn_Stmt* Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*); extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_set_encloser(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Stmt*); extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_block( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*);
extern void* Cyc_Tcenv_curr_rgn( struct Cyc_Tcenv_Tenv*); extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, void* rgn); extern void
Cyc_Tcenv_check_effect_accessible( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void* eff); extern void Cyc_Tcenv_check_rgn_partial_order( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_List_List* po); extern
unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void* Cyc_Tcutil_impos( struct
_tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_arr fmt, struct _tagged_arr ap); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_arr fmt, struct _tagged_arr ap);
extern int Cyc_Tcutil_kind_leq( void* k1, void* k2); extern void* Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar* t); extern void* Cyc_Tcutil_typ_kind( void* t); extern
void* Cyc_Tcutil_compress( void* t); extern void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*, void*, struct Cyc_List_List*); extern int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_coerceable(
void*); extern int Cyc_Tcutil_silent_castable( struct Cyc_Tcenv_Tenv*, struct
Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_castable( struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*, void*, void*); extern int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*); extern int Cyc_Tcutil_is_numeric( struct Cyc_Absyn_Exp*);
extern struct Cyc_Core_Opt Cyc_Tcutil_rk; extern struct Cyc_Core_Opt Cyc_Tcutil_ak;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk; extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
extern int Cyc_Tcutil_zero_to_null( struct Cyc_Tcenv_Tenv*, void* t, struct Cyc_Absyn_Exp*
e); extern void* Cyc_Tcutil_max_arithmetic_type( void*, void*); extern void Cyc_Tcutil_explain_failure();
extern int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute(
struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_rsubstitute( struct
_RegionHandle*, struct Cyc_List_List*, void*); extern void* Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*); struct _tuple4{ struct Cyc_List_List* f1; struct
_RegionHandle* f2; } ; struct _tuple5{ struct Cyc_Absyn_Tvar* f1; void* f2; } ;
extern struct _tuple5* Cyc_Tcutil_r_make_inst_var( struct _tuple4*, struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_check_contains_assign( struct Cyc_Absyn_Exp*); extern
void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, int allow_evars, void*); extern void
Cyc_Tcutil_check_nonzero_bound( struct Cyc_Position_Segment*, struct Cyc_Absyn_Conref*);
extern void Cyc_Tcutil_check_bound( struct Cyc_Position_Segment*, unsigned int i,
struct Cyc_Absyn_Conref*); extern int Cyc_Tcutil_is_bound_one( struct Cyc_Absyn_Conref*
b); extern struct Cyc_List_List* Cyc_Tcutil_resolve_struct_designators( struct
_RegionHandle* rgn, struct Cyc_Position_Segment* loc, struct Cyc_List_List* des,
struct Cyc_List_List* fields); extern int Cyc_Tcutil_is_tagged_pointer_typ( void*);
extern int Cyc_Tcutil_is_tagged_pointer_typ_elt( void* t, void** elt_typ_dest);
struct _tuple6{ int f1; void* f2; } ; extern struct _tuple6 Cyc_Tcutil_addressof_props(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e); extern int Cyc_Tcutil_bits_only(
void* t); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e); extern unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp*
e); extern int Cyc_Evexp_okay_szofarg( void* t); extern void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Stmt* s, int new_block); extern
struct Cyc_List_List* Cyc_Formatstr_get_format_typs( struct Cyc_Tcenv_Tenv*,
struct _tagged_arr, struct Cyc_Position_Segment*); extern struct Cyc_List_List*
Cyc_Formatstr_get_scanf_typs( struct Cyc_Tcenv_Tenv*, struct _tagged_arr, struct
Cyc_Position_Segment*); extern void* Cyc_Tcexp_tcExp( struct Cyc_Tcenv_Tenv*,
void**, struct Cyc_Absyn_Exp*); extern void* Cyc_Tcexp_tcExpInitializer( struct
Cyc_Tcenv_Tenv*, void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part);
static const int Cyc_CfFlowInfo_VarRoot= 0; struct Cyc_CfFlowInfo_VarRoot_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_CfFlowInfo_MallocPt=
1; struct Cyc_CfFlowInfo_MallocPt_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List* fields; } ;
static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; extern unsigned char Cyc_CfFlowInfo_IsZero[
11u]; extern unsigned char Cyc_CfFlowInfo_NotZero[ 12u]; extern unsigned char
Cyc_CfFlowInfo_UnknownZ[ 13u]; static const int Cyc_CfFlowInfo_PlaceL= 0; struct
Cyc_CfFlowInfo_PlaceL_struct{ int tag; struct Cyc_CfFlowInfo_Place* f1; } ;
static const int Cyc_CfFlowInfo_UnknownL= 0; static const int Cyc_CfFlowInfo_Zero=
0; static const int Cyc_CfFlowInfo_NotZeroAll= 1; static const int Cyc_CfFlowInfo_NotZeroThis=
2; static const int Cyc_CfFlowInfo_UnknownR= 0; struct Cyc_CfFlowInfo_UnknownR_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_Esc= 1; struct Cyc_CfFlowInfo_Esc_struct{
int tag; void* f1; } ; static const int Cyc_CfFlowInfo_AddressOf= 2; struct Cyc_CfFlowInfo_AddressOf_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_Aggregate=
3; struct Cyc_CfFlowInfo_Aggregate_struct{ int tag; struct Cyc_Dict_Dict* f1; }
; static const int Cyc_CfFlowInfo_BottomFL= 0; static const int Cyc_CfFlowInfo_ReachableFL=
0; struct Cyc_CfFlowInfo_ReachableFL_struct{ int tag; struct Cyc_Dict_Dict* f1;
} ; extern void Cyc_NewControlFlow_set_encloser( struct Cyc_Absyn_Stmt* enclosee,
struct Cyc_Absyn_Stmt* encloser); static void* Cyc_Tcexp_expr_err( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct _tagged_arr msg, struct _tagged_arr
ap){({ void* _temp0[ 0u]={}; Cyc_Tcutil_terr( loc,( struct _tagged_arr) Cyc_Std_vrprintf(
Cyc_Core_heap_region, msg, ap), _tag_arr( _temp0, sizeof( void*), 0u));});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp1=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp1;}));} static void Cyc_Tcexp_resolve_unknown_id( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tuple1* q){ struct _handler_cons _temp2;
_push_handler(& _temp2);{ int _temp4= 0; if( setjmp( _temp2.handler)){ _temp4= 1;}
if( ! _temp4){{ void* _temp5= Cyc_Tcenv_lookup_ordinary( te, e->loc, q); void*
_temp17; struct Cyc_Absyn_Enumfield* _temp19; struct Cyc_Absyn_Enumdecl* _temp21;
struct Cyc_Absyn_Enumfield* _temp23; void* _temp25; struct Cyc_Absyn_Tunionfield*
_temp27; struct Cyc_Absyn_Tuniondecl* _temp29; _LL7: if(*(( int*) _temp5) == 
Cyc_Tcenv_VarRes){ _LL18: _temp17=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp5)->f1; goto _LL8;} else{ goto _LL9;} _LL9: if(*(( int*) _temp5) ==  Cyc_Tcenv_EnumRes){
_LL22: _temp21=(( struct Cyc_Tcenv_EnumRes_struct*) _temp5)->f1; goto _LL20;
_LL20: _temp19=(( struct Cyc_Tcenv_EnumRes_struct*) _temp5)->f2; goto _LL10;}
else{ goto _LL11;} _LL11: if(*(( int*) _temp5) ==  Cyc_Tcenv_AnonEnumRes){ _LL26:
_temp25=( void*)(( struct Cyc_Tcenv_AnonEnumRes_struct*) _temp5)->f1; goto _LL24;
_LL24: _temp23=(( struct Cyc_Tcenv_AnonEnumRes_struct*) _temp5)->f2; goto _LL12;}
else{ goto _LL13;} _LL13: if(*(( int*) _temp5) ==  Cyc_Tcenv_TunionRes){ _LL30:
_temp29=(( struct Cyc_Tcenv_TunionRes_struct*) _temp5)->f1; goto _LL28; _LL28:
_temp27=(( struct Cyc_Tcenv_TunionRes_struct*) _temp5)->f2; goto _LL14;} else{
goto _LL15;} _LL15: if(*(( int*) _temp5) ==  Cyc_Tcenv_StructRes){ goto _LL16;}
else{ goto _LL6;} _LL8:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp31=( struct Cyc_Absyn_Var_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp31[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp32; _temp32.tag= Cyc_Absyn_Var_e;
_temp32.f1= q; _temp32.f2=( void*) _temp17; _temp32;}); _temp31;}))); goto _LL6;
_LL10:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Enum_e_struct* _temp33=(
struct Cyc_Absyn_Enum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Enum_e_struct));
_temp33[ 0]=({ struct Cyc_Absyn_Enum_e_struct _temp34; _temp34.tag= Cyc_Absyn_Enum_e;
_temp34.f1= q; _temp34.f2=( struct Cyc_Absyn_Enumdecl*) _temp21; _temp34.f3=(
struct Cyc_Absyn_Enumfield*) _temp19; _temp34;}); _temp33;}))); goto _LL6; _LL12:(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonEnum_e_struct* _temp35=(
struct Cyc_Absyn_AnonEnum_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonEnum_e_struct));
_temp35[ 0]=({ struct Cyc_Absyn_AnonEnum_e_struct _temp36; _temp36.tag= Cyc_Absyn_AnonEnum_e;
_temp36.f1= q; _temp36.f2=( void*) _temp25; _temp36.f3=( struct Cyc_Absyn_Enumfield*)
_temp23; _temp36;}); _temp35;}))); goto _LL6; _LL14:( void*)( e->r=( void*)((
void*)({ struct Cyc_Absyn_Tunion_e_struct* _temp37=( struct Cyc_Absyn_Tunion_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp37[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp38; _temp38.tag= Cyc_Absyn_Tunion_e; _temp38.f1= 0; _temp38.f2= 0; _temp38.f3=
0; _temp38.f4= _temp29; _temp38.f5= _temp27; _temp38;}); _temp37;}))); goto _LL6;
_LL16:({ struct Cyc_Std_String_pa_struct _temp40; _temp40.tag= Cyc_Std_String_pa;
_temp40.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( q);{ void* _temp39[ 1u]={&
_temp40}; Cyc_Tcutil_terr( e->loc, _tag_arr("bad occurrence of struct name %s",
sizeof( unsigned char), 33u), _tag_arr( _temp39, sizeof( void*), 1u));}});( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp41=( struct Cyc_Absyn_Var_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp41[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp42; _temp42.tag= Cyc_Absyn_Var_e; _temp42.f1= q; _temp42.f2=( void*)(( void*)
Cyc_Absyn_Unresolved_b); _temp42;}); _temp41;}))); goto _LL6; _LL6:;};
_pop_handler();} else{ void* _temp3=( void*) _exn_thrown; void* _temp44= _temp3;
_LL46: if( _temp44 ==  Cyc_Dict_Absent){ goto _LL47;} else{ goto _LL48;} _LL48:
goto _LL49; _LL47:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Var_e_struct*
_temp50=( struct Cyc_Absyn_Var_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp50[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp51; _temp51.tag= Cyc_Absyn_Var_e;
_temp51.f1= q; _temp51.f2=( void*)(( void*) Cyc_Absyn_Unresolved_b); _temp51;});
_temp50;}))); goto _LL45; _LL49:( void) _throw( _temp44); _LL45:;}}} struct
_tuple7{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; static struct
_tuple7* Cyc_Tcexp_make_struct_arg( struct Cyc_Absyn_Exp* e){ return({ struct
_tuple7* _temp52=( struct _tuple7*) _cycalloc( sizeof( struct _tuple7)); _temp52->f1=
0; _temp52->f2= e; _temp52;});} static void Cyc_Tcexp_resolve_unknown_fn( struct
Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1, struct
Cyc_List_List* es){ void* _temp53=( void*) e1->r; struct _tuple1* _temp59; _LL55:
if(*(( int*) _temp53) ==  Cyc_Absyn_UnknownId_e){ _LL60: _temp59=(( struct Cyc_Absyn_UnknownId_e_struct*)
_temp53)->f1; goto _LL56;} else{ goto _LL57;} _LL57: goto _LL58; _LL56: { struct
_handler_cons _temp61; _push_handler(& _temp61);{ int _temp63= 0; if( setjmp(
_temp61.handler)){ _temp63= 1;} if( ! _temp63){{ void* _temp64= Cyc_Tcenv_lookup_ordinary(
te, e1->loc, _temp59); void* _temp76; struct Cyc_Absyn_Tunionfield* _temp78;
struct Cyc_Absyn_Tuniondecl* _temp80; struct Cyc_Absyn_Structdecl* _temp82;
_LL66: if(*(( int*) _temp64) ==  Cyc_Tcenv_VarRes){ _LL77: _temp76=( void*)((
struct Cyc_Tcenv_VarRes_struct*) _temp64)->f1; goto _LL67;} else{ goto _LL68;}
_LL68: if(*(( int*) _temp64) ==  Cyc_Tcenv_TunionRes){ _LL81: _temp80=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp64)->f1; goto _LL79; _LL79: _temp78=(( struct
Cyc_Tcenv_TunionRes_struct*) _temp64)->f2; goto _LL69;} else{ goto _LL70;} _LL70:
if(*(( int*) _temp64) ==  Cyc_Tcenv_StructRes){ _LL83: _temp82=(( struct Cyc_Tcenv_StructRes_struct*)
_temp64)->f1; goto _LL71;} else{ goto _LL72;} _LL72: if(*(( int*) _temp64) == 
Cyc_Tcenv_AnonEnumRes){ goto _LL73;} else{ goto _LL74;} _LL74: if(*(( int*)
_temp64) ==  Cyc_Tcenv_EnumRes){ goto _LL75;} else{ goto _LL65;} _LL67:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp84=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp84[ 0]=({ struct Cyc_Absyn_FnCall_e_struct
_temp85; _temp85.tag= Cyc_Absyn_FnCall_e; _temp85.f1= e1; _temp85.f2= es;
_temp85.f3= 0; _temp85;}); _temp84;}))); _npop_handler( 0u); return; _LL69: if(
_temp78->typs ==  0){({ struct Cyc_Std_String_pa_struct _temp87; _temp87.tag=
Cyc_Std_String_pa; _temp87.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
_temp78->name);{ void* _temp86[ 1u]={& _temp87}; Cyc_Tcutil_terr( e->loc,
_tag_arr("%s is a constant, not a function", sizeof( unsigned char), 33u),
_tag_arr( _temp86, sizeof( void*), 1u));}});}( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_Tunion_e_struct* _temp88=( struct Cyc_Absyn_Tunion_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Tunion_e_struct)); _temp88[ 0]=({ struct Cyc_Absyn_Tunion_e_struct
_temp89; _temp89.tag= Cyc_Absyn_Tunion_e; _temp89.f1= 0; _temp89.f2= 0; _temp89.f3=
es; _temp89.f4= _temp80; _temp89.f5= _temp78; _temp89;}); _temp88;})));
_npop_handler( 0u); return; _npop_handler( 0u); return; _LL71: { struct Cyc_List_List*
_temp90=(( struct Cyc_List_List*(*)( struct _tuple7*(* f)( struct Cyc_Absyn_Exp*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Tcexp_make_struct_arg, es); if(
_temp82->name !=  0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp91=( struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp91[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp92; _temp92.tag= Cyc_Absyn_Struct_e;
_temp92.f1=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( _temp82->name))->v;
_temp92.f2= 0; _temp92.f3= _temp90; _temp92.f4=( struct Cyc_Absyn_Structdecl*)
_temp82; _temp92;}); _temp91;})));} else{({ void* _temp93[ 0u]={}; Cyc_Tcutil_terr(
e->loc, _tag_arr("missing struct name", sizeof( unsigned char), 20u), _tag_arr(
_temp93, sizeof( void*), 0u));});( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct*
_temp94=( struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp94[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp95; _temp95.tag= Cyc_Absyn_Struct_e;
_temp95.f1= _temp59; _temp95.f2= 0; _temp95.f3= _temp90; _temp95.f4=( struct Cyc_Absyn_Structdecl*)
_temp82; _temp95;}); _temp94;})));} _npop_handler( 0u); return;} _LL73: goto
_LL75; _LL75:({ struct Cyc_Std_String_pa_struct _temp97; _temp97.tag= Cyc_Std_String_pa;
_temp97.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( _temp59);{ void*
_temp96[ 1u]={& _temp97}; Cyc_Tcutil_terr( e->loc, _tag_arr("%s is an enum constructor, not a function",
sizeof( unsigned char), 42u), _tag_arr( _temp96, sizeof( void*), 1u));}});
_npop_handler( 0u); return; _LL65:;}; _pop_handler();} else{ void* _temp62=(
void*) _exn_thrown; void* _temp99= _temp62; _LL101: if( _temp99 ==  Cyc_Dict_Absent){
goto _LL102;} else{ goto _LL103;} _LL103: goto _LL104; _LL102:( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_FnCall_e_struct* _temp105=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp105[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp106; _temp106.tag= Cyc_Absyn_FnCall_e; _temp106.f1=
e1; _temp106.f2= es; _temp106.f3= 0; _temp106;}); _temp105;}))); return; _LL104:(
void) _throw( _temp99); _LL100:;}}} _LL58:( void*)( e->r=( void*)(( void*)({
struct Cyc_Absyn_FnCall_e_struct* _temp107=( struct Cyc_Absyn_FnCall_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnCall_e_struct)); _temp107[ 0]=({ struct
Cyc_Absyn_FnCall_e_struct _temp108; _temp108.tag= Cyc_Absyn_FnCall_e; _temp108.f1=
e1; _temp108.f2= es; _temp108.f3= 0; _temp108;}); _temp107;}))); return; _LL54:;}
static void Cyc_Tcexp_resolve_unresolved_mem( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
des){ if( topt ==  0){( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp109=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp109[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp110; _temp110.tag= Cyc_Absyn_Array_e;
_temp110.f1= des; _temp110;}); _temp109;}))); return;}{ void* t=* topt; void*
_temp111= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Tqual _temp121; void*
_temp123; struct Cyc_Absyn_Structdecl** _temp125; struct Cyc_List_List* _temp127;
struct _tuple1* _temp129; _LL113: if(( unsigned int) _temp111 >  3u?*(( int*)
_temp111) ==  Cyc_Absyn_ArrayType: 0){ _LL124: _temp123=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp111)->f1; goto _LL122; _LL122: _temp121=(( struct Cyc_Absyn_ArrayType_struct*)
_temp111)->f2; goto _LL114;} else{ goto _LL115;} _LL115: if(( unsigned int)
_temp111 >  3u?*(( int*) _temp111) ==  Cyc_Absyn_StructType: 0){ _LL130:
_temp129=(( struct Cyc_Absyn_StructType_struct*) _temp111)->f1; goto _LL128;
_LL128: _temp127=(( struct Cyc_Absyn_StructType_struct*) _temp111)->f2; goto
_LL126; _LL126: _temp125=(( struct Cyc_Absyn_StructType_struct*) _temp111)->f3;
goto _LL116;} else{ goto _LL117;} _LL117: if(( unsigned int) _temp111 >  3u?*((
int*) _temp111) ==  Cyc_Absyn_AnonStructType: 0){ goto _LL118;} else{ goto
_LL119;} _LL119: goto _LL120; _LL114:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Array_e_struct* _temp131=( struct Cyc_Absyn_Array_e_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Array_e_struct)); _temp131[ 0]=({ struct Cyc_Absyn_Array_e_struct
_temp132; _temp132.tag= Cyc_Absyn_Array_e; _temp132.f1= des; _temp132;});
_temp131;}))); goto _LL112; _LL116: if( _temp125 ==  0? 1: _temp129 ==  0){({
void* _temp133[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("Compiler bug: struct type not properly set",
sizeof( unsigned char), 43u), _tag_arr( _temp133, sizeof( void*), 0u));});}(
void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Struct_e_struct* _temp134=(
struct Cyc_Absyn_Struct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp134[ 0]=({ struct Cyc_Absyn_Struct_e_struct _temp135; _temp135.tag= Cyc_Absyn_Struct_e;
_temp135.f1=( struct _tuple1*) _check_null( _temp129); _temp135.f2= 0; _temp135.f3=
des; _temp135.f4=( struct Cyc_Absyn_Structdecl*)*(( struct Cyc_Absyn_Structdecl**)
_check_null( _temp125)); _temp135;}); _temp134;}))); goto _LL112; _LL118:( void*)(
e->r=( void*)(( void*)({ struct Cyc_Absyn_AnonStruct_e_struct* _temp136=( struct
Cyc_Absyn_AnonStruct_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_AnonStruct_e_struct));
_temp136[ 0]=({ struct Cyc_Absyn_AnonStruct_e_struct _temp137; _temp137.tag= Cyc_Absyn_AnonStruct_e;
_temp137.f1=( void*) t; _temp137.f2= des; _temp137;}); _temp136;}))); goto
_LL112; _LL120:( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp138=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp138[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp139; _temp139.tag= Cyc_Absyn_Array_e;
_temp139.f1= des; _temp139;}); _temp138;}))); goto _LL112; _LL112:;}} static
void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp*
e); static void* Cyc_Tcexp_tcExpNoPromote( struct Cyc_Tcenv_Tenv* te, void**
topt, struct Cyc_Absyn_Exp* e); static void Cyc_Tcexp_tcExpList( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List* es){ for( 0; es !=  0; es= es->tl){ Cyc_Tcexp_tcExp(
te, 0,( struct Cyc_Absyn_Exp*) es->hd);}} void Cyc_Tcexp_tcTest( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, struct _tagged_arr msg_part){ Cyc_Tcutil_check_contains_assign(
e); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_sint_t, e); if( ! Cyc_Tcutil_coerce_to_bool(
te, e)){({ struct Cyc_Std_String_pa_struct _temp142; _temp142.tag= Cyc_Std_String_pa;
_temp142.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v);{ struct Cyc_Std_String_pa_struct _temp141; _temp141.tag=
Cyc_Std_String_pa; _temp141.f1=( struct _tagged_arr) msg_part;{ void* _temp140[
2u]={& _temp141,& _temp142}; Cyc_Tcutil_terr( e->loc, _tag_arr("test of %s has type %s instead of integral or * type",
sizeof( unsigned char), 53u), _tag_arr( _temp140, sizeof( void*), 2u));}}});}}
static void* Cyc_Tcexp_tcConst( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* c, struct Cyc_Absyn_Exp* e){ void* t;{ void* _temp143= c;
void* _temp159; void* _temp161; void* _temp163; int _temp165; void* _temp167;
struct _tagged_arr _temp169; _LL145: if(( unsigned int) _temp143 >  1u?*(( int*)
_temp143) ==  Cyc_Absyn_Char_c: 0){ _LL160: _temp159=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp143)->f1; goto _LL146;} else{ goto _LL147;} _LL147: if(( unsigned int)
_temp143 >  1u?*(( int*) _temp143) ==  Cyc_Absyn_Short_c: 0){ _LL162: _temp161=(
void*)(( struct Cyc_Absyn_Short_c_struct*) _temp143)->f1; goto _LL148;} else{
goto _LL149;} _LL149: if(( unsigned int) _temp143 >  1u?*(( int*) _temp143) == 
Cyc_Absyn_LongLong_c: 0){ _LL164: _temp163=( void*)(( struct Cyc_Absyn_LongLong_c_struct*)
_temp143)->f1; goto _LL150;} else{ goto _LL151;} _LL151: if(( unsigned int)
_temp143 >  1u?*(( int*) _temp143) ==  Cyc_Absyn_Float_c: 0){ goto _LL152;}
else{ goto _LL153;} _LL153: if(( unsigned int) _temp143 >  1u?*(( int*) _temp143)
==  Cyc_Absyn_Int_c: 0){ _LL168: _temp167=( void*)(( struct Cyc_Absyn_Int_c_struct*)
_temp143)->f1; goto _LL166; _LL166: _temp165=(( struct Cyc_Absyn_Int_c_struct*)
_temp143)->f2; goto _LL154;} else{ goto _LL155;} _LL155: if(( unsigned int)
_temp143 >  1u?*(( int*) _temp143) ==  Cyc_Absyn_String_c: 0){ _LL170: _temp169=((
struct Cyc_Absyn_String_c_struct*) _temp143)->f1; goto _LL156;} else{ goto
_LL157;} _LL157: if( _temp143 == ( void*) Cyc_Absyn_Null_c){ goto _LL158;} else{
goto _LL144;} _LL146: t= _temp159 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t; goto _LL144; _LL148: t= _temp161 == ( void*) Cyc_Absyn_Unsigned?
Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t; goto _LL144; _LL150: t= _temp163 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t: Cyc_Absyn_slonglong_t; goto
_LL144; _LL152: t= Cyc_Absyn_float_t; goto _LL144; _LL154: if( topt ==  0){ t=
_temp167 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{ void* _temp171= Cyc_Tcutil_compress(* topt); void* _temp183; void*
_temp185; void* _temp187; void* _temp189; void* _temp191; _LL173: if((
unsigned int) _temp171 >  3u?*(( int*) _temp171) ==  Cyc_Absyn_IntType: 0){
_LL186: _temp185=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp171)->f1;
goto _LL184; _LL184: _temp183=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f2; if( _temp183 == ( void*) Cyc_Absyn_B1){ goto _LL174;} else{ goto
_LL175;}} else{ goto _LL175;} _LL175: if(( unsigned int) _temp171 >  3u?*(( int*)
_temp171) ==  Cyc_Absyn_IntType: 0){ _LL190: _temp189=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f1; goto _LL188; _LL188: _temp187=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f2; if( _temp187 == ( void*) Cyc_Absyn_B2){ goto _LL176;} else{ goto
_LL177;}} else{ goto _LL177;} _LL177: if(( unsigned int) _temp171 >  3u?*(( int*)
_temp171) ==  Cyc_Absyn_IntType: 0){ _LL192: _temp191=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp171)->f1; goto _LL178;} else{ goto _LL179;} _LL179: if(( unsigned int)
_temp171 >  3u?*(( int*) _temp171) ==  Cyc_Absyn_PointerType: 0){ goto _LL193;}
else{ goto _LL181;} _LL193: if( _temp165 ==  0){ goto _LL180;} else{ goto _LL181;}
_LL181: goto _LL182; _LL174: t= _temp185 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t;( void*)( e->r=( void*)(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp194=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp194[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp195; _temp195.tag= Cyc_Absyn_Const_e;
_temp195.f1=( void*)(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp196=(
struct Cyc_Absyn_Char_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp196[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp197; _temp197.tag= Cyc_Absyn_Char_c;
_temp197.f1=( void*) _temp185; _temp197.f2=( unsigned char) _temp165; _temp197;});
_temp196;})); _temp195;}); _temp194;}))); goto _LL172; _LL176: t= _temp189 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Const_e_struct* _temp198=( struct Cyc_Absyn_Const_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp198[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp199; _temp199.tag= Cyc_Absyn_Const_e; _temp199.f1=( void*)(( void*)({
struct Cyc_Absyn_Short_c_struct* _temp200=( struct Cyc_Absyn_Short_c_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Short_c_struct)); _temp200[ 0]=({ struct Cyc_Absyn_Short_c_struct
_temp201; _temp201.tag= Cyc_Absyn_Short_c; _temp201.f1=( void*) _temp189;
_temp201.f2=( short) _temp165; _temp201;}); _temp200;})); _temp199;}); _temp198;})));
goto _LL172; _LL178: t= _temp191 == ( void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t:
Cyc_Absyn_sint_t; goto _LL172; _LL180:( void*)( e->r=( void*)(( void*)({ struct
Cyc_Absyn_Const_e_struct* _temp202=( struct Cyc_Absyn_Const_e_struct*) _cycalloc(
sizeof( struct Cyc_Absyn_Const_e_struct)); _temp202[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp203; _temp203.tag= Cyc_Absyn_Const_e; _temp203.f1=( void*)(( void*) Cyc_Absyn_Null_c);
_temp203;}); _temp202;})));{ struct Cyc_List_List* _temp204= Cyc_Tcenv_lookup_type_vars(
te); t=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp205=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp205[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp206; _temp206.tag= Cyc_Absyn_PointerType;
_temp206.f1=({ struct Cyc_Absyn_PtrInfo _temp207; _temp207.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({ struct Cyc_Core_Opt* _temp209=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp209->v=( void*)
_temp204; _temp209;})); _temp207.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt* _temp208=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp208->v=( void*) _temp204;
_temp208;})); _temp207.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp207.tq= Cyc_Absyn_empty_tqual(); _temp207.bounds= Cyc_Absyn_empty_conref();
_temp207;}); _temp206;}); _temp205;}); goto _LL172;} _LL182: t= _temp167 == (
void*) Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t; goto _LL172;
_LL172:;} goto _LL144; _LL156: { int len=( int) _get_arr_size( _temp169, sizeof(
unsigned char)); struct Cyc_Absyn_Const_e_struct* _temp210=({ struct Cyc_Absyn_Const_e_struct*
_temp247=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp247[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp248; _temp248.tag= Cyc_Absyn_Const_e;
_temp248.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp249=( struct
Cyc_Absyn_Int_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp249[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp250; _temp250.tag= Cyc_Absyn_Int_c;
_temp250.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp250.f2= len; _temp250;});
_temp249;})); _temp248;}); _temp247;}); struct Cyc_Absyn_Exp* elen= Cyc_Absyn_new_exp((
void*) _temp210, loc); elen->topt=({ struct Cyc_Core_Opt* _temp211=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp211->v=( void*) Cyc_Absyn_uint_t;
_temp211;}); t= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_const_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp212=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp212[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp213; _temp213.tag= Cyc_Absyn_Upper_b;
_temp213.f1= elen; _temp213;}); _temp212;})); if( topt !=  0){ void* _temp214=
Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_Exp* _temp222; struct Cyc_Absyn_Tqual
_temp224; void* _temp226; struct Cyc_Absyn_PtrInfo _temp228; struct Cyc_Absyn_Conref*
_temp230; struct Cyc_Absyn_Tqual _temp232; struct Cyc_Absyn_Conref* _temp234;
void* _temp236; void* _temp238; _LL216: if(( unsigned int) _temp214 >  3u?*((
int*) _temp214) ==  Cyc_Absyn_ArrayType: 0){ _LL227: _temp226=( void*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp214)->f1; goto _LL225; _LL225: _temp224=((
struct Cyc_Absyn_ArrayType_struct*) _temp214)->f2; goto _LL223; _LL223: _temp222=((
struct Cyc_Absyn_ArrayType_struct*) _temp214)->f3; goto _LL217;} else{ goto
_LL218;} _LL218: if(( unsigned int) _temp214 >  3u?*(( int*) _temp214) ==  Cyc_Absyn_PointerType:
0){ _LL229: _temp228=(( struct Cyc_Absyn_PointerType_struct*) _temp214)->f1;
_LL239: _temp238=( void*) _temp228.elt_typ; goto _LL237; _LL237: _temp236=( void*)
_temp228.rgn_typ; goto _LL235; _LL235: _temp234= _temp228.nullable; goto _LL233;
_LL233: _temp232= _temp228.tq; goto _LL231; _LL231: _temp230= _temp228.bounds;
goto _LL219;} else{ goto _LL220;} _LL220: goto _LL221; _LL217: return( void*)({
struct Cyc_Absyn_ArrayType_struct* _temp240=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp240[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp241; _temp241.tag= Cyc_Absyn_ArrayType; _temp241.f1=(
void*) Cyc_Absyn_uchar_t; _temp241.f2= _temp224; _temp241.f3=( struct Cyc_Absyn_Exp*)
elen; _temp241;}); _temp240;}); _LL219: if( ! Cyc_Tcutil_unify(* topt, t)? Cyc_Tcutil_silent_castable(
te, loc, t,* topt): 0){ e->topt=({ struct Cyc_Core_Opt* _temp242=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp242->v=( void*) t; _temp242;});
Cyc_Tcutil_unchecked_cast( te, e,* topt); t=* topt;} else{ t= Cyc_Absyn_atb_typ(
Cyc_Absyn_uchar_t, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt* _temp243=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp243->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp243;})),
Cyc_Absyn_const_tqual(),( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp244=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp244[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp245; _temp245.tag= Cyc_Absyn_Upper_b;
_temp245.f1= elen; _temp245;}); _temp244;})); if( ! Cyc_Tcutil_unify(* topt, t)?
Cyc_Tcutil_silent_castable( te, loc, t,* topt): 0){ e->topt=({ struct Cyc_Core_Opt*
_temp246=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt));
_temp246->v=( void*) t; _temp246;}); Cyc_Tcutil_unchecked_cast( te, e,* topt); t=*
topt;}} goto _LL215; _LL221: goto _LL215; _LL215:;} return t;} _LL158: { struct
Cyc_List_List* _temp251= Cyc_Tcenv_lookup_type_vars( te); t=( void*)({ struct
Cyc_Absyn_PointerType_struct* _temp252=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp252[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp253; _temp253.tag= Cyc_Absyn_PointerType;
_temp253.f1=({ struct Cyc_Absyn_PtrInfo _temp254; _temp254.elt_typ=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({ struct Cyc_Core_Opt* _temp256=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp256->v=( void*)
_temp251; _temp256;})); _temp254.rgn_typ=( void*) Cyc_Absyn_new_evar(( struct
Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt* _temp255=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp255->v=( void*) _temp251;
_temp255;})); _temp254.nullable=(( struct Cyc_Absyn_Conref*(*)( int x)) Cyc_Absyn_new_conref)(
1); _temp254.tq= Cyc_Absyn_empty_tqual(); _temp254.bounds= Cyc_Absyn_empty_conref();
_temp254;}); _temp253;}); _temp252;}); goto _LL144;} _LL144:;} return t;} static
void* Cyc_Tcexp_tcVar( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct _tuple1* q, void* b){ void* _temp257= b; struct Cyc_Absyn_Vardecl*
_temp271; struct Cyc_Absyn_Fndecl* _temp273; struct Cyc_Absyn_Vardecl* _temp275;
struct Cyc_Absyn_Vardecl* _temp277; struct Cyc_Absyn_Vardecl* _temp279; _LL259:
if( _temp257 == ( void*) Cyc_Absyn_Unresolved_b){ goto _LL260;} else{ goto
_LL261;} _LL261: if(( unsigned int) _temp257 >  1u?*(( int*) _temp257) ==  Cyc_Absyn_Global_b:
0){ _LL272: _temp271=(( struct Cyc_Absyn_Global_b_struct*) _temp257)->f1; goto
_LL262;} else{ goto _LL263;} _LL263: if(( unsigned int) _temp257 >  1u?*(( int*)
_temp257) ==  Cyc_Absyn_Funname_b: 0){ _LL274: _temp273=(( struct Cyc_Absyn_Funname_b_struct*)
_temp257)->f1; goto _LL264;} else{ goto _LL265;} _LL265: if(( unsigned int)
_temp257 >  1u?*(( int*) _temp257) ==  Cyc_Absyn_Pat_b: 0){ _LL276: _temp275=((
struct Cyc_Absyn_Pat_b_struct*) _temp257)->f1; goto _LL266;} else{ goto _LL267;}
_LL267: if(( unsigned int) _temp257 >  1u?*(( int*) _temp257) ==  Cyc_Absyn_Local_b:
0){ _LL278: _temp277=(( struct Cyc_Absyn_Local_b_struct*) _temp257)->f1; goto
_LL268;} else{ goto _LL269;} _LL269: if(( unsigned int) _temp257 >  1u?*(( int*)
_temp257) ==  Cyc_Absyn_Param_b: 0){ _LL280: _temp279=(( struct Cyc_Absyn_Param_b_struct*)
_temp257)->f1; goto _LL270;} else{ goto _LL258;} _LL260: return({ struct Cyc_Std_String_pa_struct
_temp282; _temp282.tag= Cyc_Std_String_pa; _temp282.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(
q);{ void* _temp281[ 1u]={& _temp282}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("undeclared identifier: %s",
sizeof( unsigned char), 26u), _tag_arr( _temp281, sizeof( void*), 1u));}});
_LL262: q[ _check_known_subscript_notnull( 1u, 0)]=( _temp271->name)[
_check_known_subscript_notnull( 1u, 0)]; return( void*) _temp271->type; _LL264:
q[ _check_known_subscript_notnull( 1u, 0)]=( _temp273->name)[
_check_known_subscript_notnull( 1u, 0)]; return Cyc_Tcutil_fndecl2typ( _temp273);
_LL266: _temp277= _temp275; goto _LL268; _LL268: _temp279= _temp277; goto _LL270;
_LL270:(* q).f1=( void*) Cyc_Absyn_Loc_n; return( void*) _temp279->type; _LL258:;}
static void Cyc_Tcexp_check_format_args( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
fmt, struct Cyc_Core_Opt* opt_args, struct Cyc_List_List*(* type_getter)( struct
Cyc_Tcenv_Tenv*, struct _tagged_arr, struct Cyc_Position_Segment*)){ struct Cyc_List_List*
desc_types;{ void* _temp283=( void*) fmt->r; void* _temp291; struct _tagged_arr
_temp293; struct Cyc_Absyn_Exp* _temp295; struct Cyc_Absyn_Exp _temp297; void*
_temp298; void* _temp300; struct _tagged_arr _temp302; _LL285: if(*(( int*)
_temp283) ==  Cyc_Absyn_Const_e){ _LL292: _temp291=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp283)->f1; if(( unsigned int) _temp291 >  1u?*(( int*) _temp291) ==  Cyc_Absyn_String_c:
0){ _LL294: _temp293=(( struct Cyc_Absyn_String_c_struct*) _temp291)->f1; goto
_LL286;} else{ goto _LL287;}} else{ goto _LL287;} _LL287: if(*(( int*) _temp283)
==  Cyc_Absyn_Cast_e){ _LL296: _temp295=(( struct Cyc_Absyn_Cast_e_struct*)
_temp283)->f2; _temp297=* _temp295; _LL299: _temp298=( void*) _temp297.r; if(*((
int*) _temp298) ==  Cyc_Absyn_Const_e){ _LL301: _temp300=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp298)->f1; if(( unsigned int) _temp300 >  1u?*(( int*) _temp300) ==  Cyc_Absyn_String_c:
0){ _LL303: _temp302=(( struct Cyc_Absyn_String_c_struct*) _temp300)->f1; goto
_LL288;} else{ goto _LL289;}} else{ goto _LL289;}} else{ goto _LL289;} _LL289:
goto _LL290; _LL286: _temp302= _temp293; goto _LL288; _LL288: desc_types=
type_getter( te,( struct _tagged_arr) _temp302, fmt->loc); goto _LL284; _LL290:
return; _LL284:;} if( opt_args !=  0){ struct Cyc_List_List* _temp304=( struct
Cyc_List_List*) opt_args->v; for( 0; desc_types !=  0? _temp304 !=  0: 0;(
desc_types= desc_types->tl, _temp304= _temp304->tl)){ void* t=( void*)
desc_types->hd; struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) _temp304->hd;
if( ! Cyc_Tcutil_coerce_arg( te, e, t)){({ struct Cyc_Std_String_pa_struct
_temp307; _temp307.tag= Cyc_Std_String_pa; _temp307.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp306; _temp306.tag= Cyc_Std_String_pa; _temp306.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp305[ 2u]={& _temp306,& _temp307}; Cyc_Tcutil_terr( e->loc,
_tag_arr("descriptor has type \n%s\n but argument has type \n%s", sizeof(
unsigned char), 51u), _tag_arr( _temp305, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}}
if( desc_types !=  0){({ void* _temp308[ 0u]={}; Cyc_Tcutil_terr( fmt->loc,
_tag_arr("too few arguments", sizeof( unsigned char), 18u), _tag_arr( _temp308,
sizeof( void*), 0u));});} if( _temp304 !=  0){({ void* _temp309[ 0u]={}; Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*) _temp304->hd)->loc, _tag_arr("too many arguments",
sizeof( unsigned char), 19u), _tag_arr( _temp309, sizeof( void*), 0u));});}}}
static void* Cyc_Tcexp_tcUnPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* p, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp310= p;
_LL312: if( _temp310 == ( void*) Cyc_Absyn_Plus){ goto _LL313;} else{ goto
_LL314;} _LL314: if( _temp310 == ( void*) Cyc_Absyn_Minus){ goto _LL315;} else{
goto _LL316;} _LL316: if( _temp310 == ( void*) Cyc_Absyn_Not){ goto _LL317;}
else{ goto _LL318;} _LL318: if( _temp310 == ( void*) Cyc_Absyn_Bitnot){ goto
_LL319;} else{ goto _LL320;} _LL320: if( _temp310 == ( void*) Cyc_Absyn_Size){
goto _LL321;} else{ goto _LL322;} _LL322: goto _LL323; _LL313: goto _LL315;
_LL315: if( ! Cyc_Tcutil_is_numeric( e)){({ struct Cyc_Std_String_pa_struct
_temp325; _temp325.tag= Cyc_Std_String_pa; _temp325.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp324[ 1u]={& _temp325}; Cyc_Tcutil_terr( loc, _tag_arr("expecting arithmetic type but found %s",
sizeof( unsigned char), 39u), _tag_arr( _temp324, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL317: Cyc_Tcutil_check_contains_assign(
e); if( ! Cyc_Tcutil_coerce_to_bool( te, e)){({ struct Cyc_Std_String_pa_struct
_temp327; _temp327.tag= Cyc_Std_String_pa; _temp327.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp326[ 1u]={& _temp327}; Cyc_Tcutil_terr( loc, _tag_arr("expecting integral or * type but found %s",
sizeof( unsigned char), 42u), _tag_arr( _temp326, sizeof( void*), 1u));}});}
return Cyc_Absyn_sint_t; _LL319: if( ! Cyc_Tcutil_is_integral( e)){({ struct Cyc_Std_String_pa_struct
_temp329; _temp329.tag= Cyc_Std_String_pa; _temp329.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp328[ 1u]={& _temp329}; Cyc_Tcutil_terr( loc, _tag_arr("expecting integral type but found %s",
sizeof( unsigned char), 37u), _tag_arr( _temp328, sizeof( void*), 1u));}});}
return( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; _LL321:{ void*
_temp330= t; struct Cyc_Absyn_PtrInfo _temp338; _LL332: if(( unsigned int)
_temp330 >  3u?*(( int*) _temp330) ==  Cyc_Absyn_ArrayType: 0){ goto _LL333;}
else{ goto _LL334;} _LL334: if(( unsigned int) _temp330 >  3u?*(( int*) _temp330)
==  Cyc_Absyn_PointerType: 0){ _LL339: _temp338=(( struct Cyc_Absyn_PointerType_struct*)
_temp330)->f1; goto _LL335;} else{ goto _LL336;} _LL336: goto _LL337; _LL333:
goto _LL331; _LL335: goto _LL331; _LL337:({ struct Cyc_Std_String_pa_struct
_temp341; _temp341.tag= Cyc_Std_String_pa; _temp341.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp340[ 1u]={& _temp341}; Cyc_Tcutil_terr( loc, _tag_arr("size requires pointer or array type, not %s",
sizeof( unsigned char), 44u), _tag_arr( _temp340, sizeof( void*), 1u));}}); goto
_LL331; _LL331:;} return Cyc_Absyn_uint_t; _LL323: return({ void* _temp342[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("Non-unary primop", sizeof( unsigned char), 17u),
_tag_arr( _temp342, sizeof( void*), 0u));}); _LL311:;} static void* Cyc_Tcexp_tcArithBinop(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
int(* checker)( struct Cyc_Absyn_Exp*)){ if( ! checker( e1)){({ struct Cyc_Std_String_pa_struct
_temp344; _temp344.tag= Cyc_Std_String_pa; _temp344.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp343[ 1u]={&
_temp344}; Cyc_Tcutil_terr( e1->loc, _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u), _tag_arr( _temp343, sizeof( void*), 1u));}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp345=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp345->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp345;}));} if( ! checker( e2)){({ struct Cyc_Std_String_pa_struct
_temp347; _temp347.tag= Cyc_Std_String_pa; _temp347.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp346[ 1u]={&
_temp347}; Cyc_Tcutil_terr( e2->loc, _tag_arr("type %s cannot be used here",
sizeof( unsigned char), 28u), _tag_arr( _temp346, sizeof( void*), 1u));}});
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp348=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp348->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp348;}));}{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); return Cyc_Tcutil_max_arithmetic_type( t1, t2);}}
static void* Cyc_Tcexp_tcPlus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1= Cyc_Tcutil_compress(( void*)(( struct
Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v); void* _temp349= t1; struct
Cyc_Absyn_PtrInfo _temp355; struct Cyc_Absyn_Conref* _temp357; struct Cyc_Absyn_Tqual
_temp359; struct Cyc_Absyn_Conref* _temp361; void* _temp363; void* _temp365;
_LL351: if(( unsigned int) _temp349 >  3u?*(( int*) _temp349) ==  Cyc_Absyn_PointerType:
0){ _LL356: _temp355=(( struct Cyc_Absyn_PointerType_struct*) _temp349)->f1;
_LL366: _temp365=( void*) _temp355.elt_typ; goto _LL364; _LL364: _temp363=( void*)
_temp355.rgn_typ; goto _LL362; _LL362: _temp361= _temp355.nullable; goto _LL360;
_LL360: _temp359= _temp355.tq; goto _LL358; _LL358: _temp357= _temp355.bounds;
goto _LL352;} else{ goto _LL353;} _LL353: goto _LL354; _LL352: if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( _temp365),( void*) Cyc_Absyn_MemKind)){({ void* _temp367[ 0u]={};
Cyc_Tcutil_terr( e1->loc, _tag_arr("can't perform arithmetic on abstract pointer type",
sizeof( unsigned char), 50u), _tag_arr( _temp367, sizeof( void*), 0u));});} if(
! Cyc_Tcutil_coerce_sint_typ( te, e2)){({ struct Cyc_Std_String_pa_struct
_temp369; _temp369.tag= Cyc_Std_String_pa; _temp369.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ void* _temp368[ 1u]={& _temp369}; Cyc_Tcutil_terr( e2->loc, _tag_arr("expecting int but found %s",
sizeof( unsigned char), 27u), _tag_arr( _temp368, sizeof( void*), 1u));}});}
_temp357= Cyc_Absyn_compress_conref( _temp357);{ void* _temp370=( void*)
_temp357->v; void* _temp378; void* _temp380; struct Cyc_Absyn_Exp* _temp382;
_LL372: if(( unsigned int) _temp370 >  1u?*(( int*) _temp370) ==  Cyc_Absyn_Eq_constr:
0){ _LL379: _temp378=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp370)->f1;
if( _temp378 == ( void*) Cyc_Absyn_Unknown_b){ goto _LL373;} else{ goto _LL374;}}
else{ goto _LL374;} _LL374: if(( unsigned int) _temp370 >  1u?*(( int*) _temp370)
==  Cyc_Absyn_Eq_constr: 0){ _LL381: _temp380=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp370)->f1; if(( unsigned int) _temp380 >  1u?*(( int*) _temp380) ==  Cyc_Absyn_Upper_b:
0){ _LL383: _temp382=(( struct Cyc_Absyn_Upper_b_struct*) _temp380)->f1; goto
_LL375;} else{ goto _LL376;}} else{ goto _LL376;} _LL376: goto _LL377; _LL373:
return t1; _LL375: { struct Cyc_Absyn_PointerType_struct* _temp384=({ struct Cyc_Absyn_PointerType_struct*
_temp385=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp385[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp386; _temp386.tag= Cyc_Absyn_PointerType;
_temp386.f1=({ struct Cyc_Absyn_PtrInfo _temp387; _temp387.elt_typ=( void*)
_temp365; _temp387.rgn_typ=( void*) _temp363; _temp387.nullable=(( struct Cyc_Absyn_Conref*(*)(
int x)) Cyc_Absyn_new_conref)( 1); _temp387.tq= _temp359; _temp387.bounds= Cyc_Absyn_new_conref((
void*) Cyc_Absyn_Unknown_b); _temp387;}); _temp386;}); _temp385;}); Cyc_Tcutil_unchecked_cast(
te, e1,( void*) _temp384); return( void*) _temp384;} _LL377:( void*)( _temp357->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp388=( struct Cyc_Absyn_Eq_constr_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp388[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp389; _temp389.tag= Cyc_Absyn_Eq_constr; _temp389.f1=(
void*)(( void*) Cyc_Absyn_Unknown_b); _temp389;}); _temp388;}))); return t1;
_LL371:;} _LL354: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL350:;} static void* Cyc_Tcexp_tcMinus( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* t1=( void*)(( struct Cyc_Core_Opt*)
_check_null( e1->topt))->v; void* t2=( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v; void* t1_elt=( void*) Cyc_Absyn_VoidType; void*
t2_elt=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt( t1,&
t1_elt)){ if( Cyc_Tcutil_is_tagged_pointer_typ( t2)){ if( ! Cyc_Tcutil_unify( t1,
t2)){({ struct Cyc_Std_String_pa_struct _temp392; _temp392.tag= Cyc_Std_String_pa;
_temp392.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v);{ struct Cyc_Std_String_pa_struct _temp391; _temp391.tag=
Cyc_Std_String_pa; _temp391.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp390[ 2u]={&
_temp391,& _temp392}; Cyc_Tcutil_terr( e1->loc, _tag_arr("pointer arithmetic on values of different types (%s != %s)",
sizeof( unsigned char), 59u), _tag_arr( _temp390, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_t;} else{ if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind(
t1_elt),( void*) Cyc_Absyn_MemKind)){({ void* _temp393[ 0u]={}; Cyc_Tcutil_terr(
e1->loc, _tag_arr("can't perform arithmetic on abstract pointer type", sizeof(
unsigned char), 50u), _tag_arr( _temp393, sizeof( void*), 0u));});} if( ! Cyc_Tcutil_coerce_sint_typ(
te, e2)){({ struct Cyc_Std_String_pa_struct _temp396; _temp396.tag= Cyc_Std_String_pa;
_temp396.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp395; _temp395.tag= Cyc_Std_String_pa; _temp395.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp394[ 2u]={& _temp395,& _temp396}; Cyc_Tcutil_terr( e2->loc,
_tag_arr("expecting either %s or int but found %s", sizeof( unsigned char), 40u),
_tag_arr( _temp394, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
return t1;}} else{ return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);}}
static void* Cyc_Tcexp_tcAnyBinop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ int e1_is_num= Cyc_Tcutil_is_numeric(
e1); int e2_is_num= Cyc_Tcutil_is_numeric( e2); void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v); void* t2= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v); if( e1_is_num?
e2_is_num: 0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_unify( t1, t2)?
Cyc_Tcutil_unify( t1, Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({
struct Cyc_Core_Opt* _temp397=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp397->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp397;}))):
0){ return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_silent_castable( te, loc, t2,
t1)){ Cyc_Tcutil_unchecked_cast( te, e2, t1); return Cyc_Absyn_sint_t;} else{
if( Cyc_Tcutil_silent_castable( te, loc, t1, t2)){ Cyc_Tcutil_unchecked_cast( te,
e1, t2); return Cyc_Absyn_sint_t;} else{ if( Cyc_Tcutil_zero_to_null( te, t2, e1)?
1: Cyc_Tcutil_zero_to_null( te, t1, e2)){ return Cyc_Absyn_sint_t;} else{{
struct _tuple0 _temp399=({ struct _tuple0 _temp398; _temp398.f1= Cyc_Tcutil_compress(
t1); _temp398.f2= Cyc_Tcutil_compress( t2); _temp398;}); void* _temp405; struct
Cyc_Absyn_PtrInfo _temp407; void* _temp409; void* _temp411; struct Cyc_Absyn_PtrInfo
_temp413; void* _temp415; _LL401: _LL412: _temp411= _temp399.f1; if((
unsigned int) _temp411 >  3u?*(( int*) _temp411) ==  Cyc_Absyn_PointerType: 0){
_LL414: _temp413=(( struct Cyc_Absyn_PointerType_struct*) _temp411)->f1; _LL416:
_temp415=( void*) _temp413.elt_typ; goto _LL406;} else{ goto _LL403;} _LL406:
_temp405= _temp399.f2; if(( unsigned int) _temp405 >  3u?*(( int*) _temp405) == 
Cyc_Absyn_PointerType: 0){ _LL408: _temp407=(( struct Cyc_Absyn_PointerType_struct*)
_temp405)->f1; _LL410: _temp409=( void*) _temp407.elt_typ; goto _LL402;} else{
goto _LL403;} _LL403: goto _LL404; _LL402: if( Cyc_Tcutil_unify( _temp415,
_temp409)){ return Cyc_Absyn_sint_t;} goto _LL400; _LL404: goto _LL400; _LL400:;}
FAIL:({ struct Cyc_Std_String_pa_struct _temp419; _temp419.tag= Cyc_Std_String_pa;
_temp419.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp418; _temp418.tag= Cyc_Std_String_pa; _temp418.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp417[ 2u]={& _temp418,& _temp419}; Cyc_Tcutil_terr( loc,
_tag_arr("comparison not allowed between %s and %s", sizeof( unsigned char), 41u),
_tag_arr( _temp417, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp420=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp420->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp420;}));}}}}}} static void* Cyc_Tcexp_tcBinPrimop( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, void* p, struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2){ void* _temp421= p; _LL423: if( _temp421 == ( void*)
Cyc_Absyn_Plus){ goto _LL424;} else{ goto _LL425;} _LL425: if( _temp421 == (
void*) Cyc_Absyn_Minus){ goto _LL426;} else{ goto _LL427;} _LL427: if( _temp421
== ( void*) Cyc_Absyn_Times){ goto _LL428;} else{ goto _LL429;} _LL429: if(
_temp421 == ( void*) Cyc_Absyn_Div){ goto _LL430;} else{ goto _LL431;} _LL431:
if( _temp421 == ( void*) Cyc_Absyn_Mod){ goto _LL432;} else{ goto _LL433;}
_LL433: if( _temp421 == ( void*) Cyc_Absyn_Bitand){ goto _LL434;} else{ goto
_LL435;} _LL435: if( _temp421 == ( void*) Cyc_Absyn_Bitor){ goto _LL436;} else{
goto _LL437;} _LL437: if( _temp421 == ( void*) Cyc_Absyn_Bitxor){ goto _LL438;}
else{ goto _LL439;} _LL439: if( _temp421 == ( void*) Cyc_Absyn_Bitlshift){ goto
_LL440;} else{ goto _LL441;} _LL441: if( _temp421 == ( void*) Cyc_Absyn_Bitlrshift){
goto _LL442;} else{ goto _LL443;} _LL443: if( _temp421 == ( void*) Cyc_Absyn_Bitarshift){
goto _LL444;} else{ goto _LL445;} _LL445: if( _temp421 == ( void*) Cyc_Absyn_Eq){
goto _LL446;} else{ goto _LL447;} _LL447: if( _temp421 == ( void*) Cyc_Absyn_Neq){
goto _LL448;} else{ goto _LL449;} _LL449: if( _temp421 == ( void*) Cyc_Absyn_Gt){
goto _LL450;} else{ goto _LL451;} _LL451: if( _temp421 == ( void*) Cyc_Absyn_Lt){
goto _LL452;} else{ goto _LL453;} _LL453: if( _temp421 == ( void*) Cyc_Absyn_Gte){
goto _LL454;} else{ goto _LL455;} _LL455: if( _temp421 == ( void*) Cyc_Absyn_Lte){
goto _LL456;} else{ goto _LL457;} _LL457: goto _LL458; _LL424: return Cyc_Tcexp_tcPlus(
te, e1, e2); _LL426: return Cyc_Tcexp_tcMinus( te, e1, e2); _LL428: goto _LL430;
_LL430: return Cyc_Tcexp_tcArithBinop( te, e1, e2, Cyc_Tcutil_is_numeric);
_LL432: goto _LL434; _LL434: goto _LL436; _LL436: goto _LL438; _LL438: goto
_LL440; _LL440: goto _LL442; _LL442: goto _LL444; _LL444: return Cyc_Tcexp_tcArithBinop(
te, e1, e2, Cyc_Tcutil_is_integral); _LL446: goto _LL448; _LL448: goto _LL450;
_LL450: goto _LL452; _LL452: goto _LL454; _LL454: goto _LL456; _LL456: return
Cyc_Tcexp_tcAnyBinop( te, loc, e1, e2); _LL458:( int) _throw(( void*)({ struct
Cyc_Core_Failure_struct* _temp459=( struct Cyc_Core_Failure_struct*) _cycalloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp459[ 0]=({ struct Cyc_Core_Failure_struct
_temp460; _temp460.tag= Cyc_Core_Failure; _temp460.f1= _tag_arr("bad binary primop",
sizeof( unsigned char), 18u); _temp460;}); _temp459;})); _LL422:;} static void*
Cyc_Tcexp_tcPrimop( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc,
void** topt, void* p, struct Cyc_List_List* es){ if( p == ( void*) Cyc_Absyn_Minus?((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( es) ==  1: 0){ return Cyc_Tcexp_tcExp(
te, topt,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd);}
Cyc_Tcexp_tcExpList( te, es);{ void* t; switch((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( es)){ case 0: _LL461: return({ void* _temp463[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("primitive operator has 0 arguments", sizeof( unsigned char),
35u), _tag_arr( _temp463, sizeof( void*), 0u));}); case 1: _LL462: t= Cyc_Tcexp_tcUnPrimop(
te, loc, topt, p,( struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null(
es))->hd); break; case 2: _LL464: t= Cyc_Tcexp_tcBinPrimop( te, loc, topt, p,(
struct Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null( es))->hd,( struct
Cyc_Absyn_Exp*)(( struct Cyc_List_List*) _check_null((( struct Cyc_List_List*)
_check_null( es))->tl))->hd); break; default: _LL465: return({ void* _temp467[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("primitive operator has > 2 arguments",
sizeof( unsigned char), 37u), _tag_arr( _temp467, sizeof( void*), 0u));});}
return t;}} struct _tuple8{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void
Cyc_Tcexp_check_writable( struct Cyc_Tcenv_Tenv* te, struct Cyc_Absyn_Exp* e){{
void* _temp468=( void*) e->r; void* _temp492; struct Cyc_Absyn_Vardecl* _temp494;
void* _temp496; struct Cyc_Absyn_Vardecl* _temp498; void* _temp500; struct Cyc_Absyn_Vardecl*
_temp502; void* _temp504; struct Cyc_Absyn_Vardecl* _temp506; struct Cyc_Absyn_Exp*
_temp508; struct Cyc_Absyn_Exp* _temp510; struct _tagged_arr* _temp512; struct
Cyc_Absyn_Exp* _temp514; struct _tagged_arr* _temp516; struct Cyc_Absyn_Exp*
_temp518; struct Cyc_Absyn_Exp* _temp520; struct Cyc_Absyn_Exp* _temp522; struct
Cyc_Absyn_Exp* _temp524; _LL470: if(*(( int*) _temp468) ==  Cyc_Absyn_Var_e){
_LL493: _temp492=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp468)->f2; if((
unsigned int) _temp492 >  1u?*(( int*) _temp492) ==  Cyc_Absyn_Param_b: 0){
_LL495: _temp494=(( struct Cyc_Absyn_Param_b_struct*) _temp492)->f1; goto _LL471;}
else{ goto _LL472;}} else{ goto _LL472;} _LL472: if(*(( int*) _temp468) ==  Cyc_Absyn_Var_e){
_LL497: _temp496=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp468)->f2; if((
unsigned int) _temp496 >  1u?*(( int*) _temp496) ==  Cyc_Absyn_Local_b: 0){
_LL499: _temp498=(( struct Cyc_Absyn_Local_b_struct*) _temp496)->f1; goto _LL473;}
else{ goto _LL474;}} else{ goto _LL474;} _LL474: if(*(( int*) _temp468) ==  Cyc_Absyn_Var_e){
_LL501: _temp500=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp468)->f2; if((
unsigned int) _temp500 >  1u?*(( int*) _temp500) ==  Cyc_Absyn_Pat_b: 0){ _LL503:
_temp502=(( struct Cyc_Absyn_Pat_b_struct*) _temp500)->f1; goto _LL475;} else{
goto _LL476;}} else{ goto _LL476;} _LL476: if(*(( int*) _temp468) ==  Cyc_Absyn_Var_e){
_LL505: _temp504=( void*)(( struct Cyc_Absyn_Var_e_struct*) _temp468)->f2; if((
unsigned int) _temp504 >  1u?*(( int*) _temp504) ==  Cyc_Absyn_Global_b: 0){
_LL507: _temp506=(( struct Cyc_Absyn_Global_b_struct*) _temp504)->f1; goto
_LL477;} else{ goto _LL478;}} else{ goto _LL478;} _LL478: if(*(( int*) _temp468)
==  Cyc_Absyn_Subscript_e){ _LL511: _temp510=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp468)->f1; goto _LL509; _LL509: _temp508=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp468)->f2; goto _LL479;} else{ goto _LL480;} _LL480: if(*(( int*) _temp468)
==  Cyc_Absyn_StructMember_e){ _LL515: _temp514=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp468)->f1; goto _LL513; _LL513: _temp512=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp468)->f2; goto _LL481;} else{ goto _LL482;} _LL482: if(*(( int*) _temp468)
==  Cyc_Absyn_StructArrow_e){ _LL519: _temp518=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp468)->f1; goto _LL517; _LL517: _temp516=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp468)->f2; goto _LL483;} else{ goto _LL484;} _LL484: if(*(( int*) _temp468)
==  Cyc_Absyn_Deref_e){ _LL521: _temp520=(( struct Cyc_Absyn_Deref_e_struct*)
_temp468)->f1; goto _LL485;} else{ goto _LL486;} _LL486: if(*(( int*) _temp468)
==  Cyc_Absyn_NoInstantiate_e){ _LL523: _temp522=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp468)->f1; goto _LL487;} else{ goto _LL488;} _LL488: if(*(( int*) _temp468)
==  Cyc_Absyn_Instantiate_e){ _LL525: _temp524=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp468)->f1; goto _LL489;} else{ goto _LL490;} _LL490: goto _LL491; _LL471:
_temp498= _temp494; goto _LL473; _LL473: _temp502= _temp498; goto _LL475; _LL475:
_temp506= _temp502; goto _LL477; _LL477: if( !( _temp506->tq).q_const){ return;}
goto _LL469; _LL479:{ void* _temp526= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp510->topt))->v); struct Cyc_Absyn_PtrInfo _temp536; struct Cyc_Absyn_Tqual
_temp538; struct Cyc_Absyn_Tqual _temp540; struct Cyc_List_List* _temp542;
_LL528: if(( unsigned int) _temp526 >  3u?*(( int*) _temp526) ==  Cyc_Absyn_PointerType:
0){ _LL537: _temp536=(( struct Cyc_Absyn_PointerType_struct*) _temp526)->f1;
_LL539: _temp538= _temp536.tq; goto _LL529;} else{ goto _LL530;} _LL530: if((
unsigned int) _temp526 >  3u?*(( int*) _temp526) ==  Cyc_Absyn_ArrayType: 0){
_LL541: _temp540=(( struct Cyc_Absyn_ArrayType_struct*) _temp526)->f2; goto
_LL531;} else{ goto _LL532;} _LL532: if(( unsigned int) _temp526 >  3u?*(( int*)
_temp526) ==  Cyc_Absyn_TupleType: 0){ _LL543: _temp542=(( struct Cyc_Absyn_TupleType_struct*)
_temp526)->f1; goto _LL533;} else{ goto _LL534;} _LL534: goto _LL535; _LL529:
_temp540= _temp538; goto _LL531; _LL531: if( ! _temp540.q_const){ return;} goto
_LL527; _LL533: { unsigned int i= Cyc_Evexp_eval_const_uint_exp( _temp508);{
struct _handler_cons _temp544; _push_handler(& _temp544);{ int _temp546= 0; if(
setjmp( _temp544.handler)){ _temp546= 1;} if( ! _temp546){{ struct _tuple8
_temp549; struct Cyc_Absyn_Tqual _temp550; struct _tuple8* _temp547=(( struct
_tuple8*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)( _temp542,( int) i);
_temp549=* _temp547; _LL551: _temp550= _temp549.f1; goto _LL548; _LL548: if( !
_temp550.q_const){ _npop_handler( 0u); return;}}; _pop_handler();} else{ void*
_temp545=( void*) _exn_thrown; void* _temp553= _temp545; _LL555: if( _temp553 == 
Cyc_List_Nth){ goto _LL556;} else{ goto _LL557;} _LL557: goto _LL558; _LL556:
return; _LL558:( void) _throw( _temp553); _LL554:;}}} goto _LL527;} _LL535: goto
_LL527; _LL527:;} goto _LL469; _LL481:{ void* _temp559= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp514->topt))->v); struct Cyc_Absyn_Structdecl**
_temp571; struct Cyc_List_List* _temp573; struct Cyc_List_List* _temp575; struct
Cyc_Absyn_Uniondecl** _temp577; _LL561: if(( unsigned int) _temp559 >  3u?*((
int*) _temp559) ==  Cyc_Absyn_StructType: 0){ _LL572: _temp571=(( struct Cyc_Absyn_StructType_struct*)
_temp559)->f3; goto _LL562;} else{ goto _LL563;} _LL563: if(( unsigned int)
_temp559 >  3u?*(( int*) _temp559) ==  Cyc_Absyn_AnonUnionType: 0){ _LL574:
_temp573=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp559)->f1; goto _LL564;}
else{ goto _LL565;} _LL565: if(( unsigned int) _temp559 >  3u?*(( int*) _temp559)
==  Cyc_Absyn_AnonStructType: 0){ _LL576: _temp575=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp559)->f1; goto _LL566;} else{ goto _LL567;} _LL567: if(( unsigned int)
_temp559 >  3u?*(( int*) _temp559) ==  Cyc_Absyn_UnionType: 0){ _LL578: _temp577=((
struct Cyc_Absyn_UnionType_struct*) _temp559)->f3; goto _LL568;} else{ goto
_LL569;} _LL569: goto _LL570; _LL562: { struct Cyc_Absyn_Structfield* sf=
_temp571 ==  0? 0: Cyc_Absyn_lookup_struct_field(( struct Cyc_Absyn_Structdecl*)*
_temp571, _temp512); if( sf ==  0? 1: !( sf->tq).q_const){ return;} goto _LL560;}
_LL564: _temp575= _temp573; goto _LL566; _LL566: { struct Cyc_Absyn_Structfield*
sf= Cyc_Absyn_lookup_field( _temp575, _temp512); if( sf ==  0? 1: !( sf->tq).q_const){
return;} goto _LL560;} _LL568: { struct Cyc_Absyn_Structfield* sf= _temp577 == 
0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)* _temp577,
_temp512); if( sf ==  0? 1: !( sf->tq).q_const){ return;} goto _LL560;} _LL570:
goto _LL560; _LL560:;} goto _LL469; _LL483:{ void* _temp579= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( _temp518->topt))->v); struct Cyc_Absyn_PtrInfo
_temp585; void* _temp587; _LL581: if(( unsigned int) _temp579 >  3u?*(( int*)
_temp579) ==  Cyc_Absyn_PointerType: 0){ _LL586: _temp585=(( struct Cyc_Absyn_PointerType_struct*)
_temp579)->f1; _LL588: _temp587=( void*) _temp585.elt_typ; goto _LL582;} else{
goto _LL583;} _LL583: goto _LL584; _LL582:{ void* _temp589= Cyc_Tcutil_compress(
_temp587); struct Cyc_Absyn_Structdecl** _temp601; struct Cyc_Absyn_Uniondecl**
_temp603; struct Cyc_List_List* _temp605; struct Cyc_List_List* _temp607; _LL591:
if(( unsigned int) _temp589 >  3u?*(( int*) _temp589) ==  Cyc_Absyn_StructType:
0){ _LL602: _temp601=(( struct Cyc_Absyn_StructType_struct*) _temp589)->f3; goto
_LL592;} else{ goto _LL593;} _LL593: if(( unsigned int) _temp589 >  3u?*(( int*)
_temp589) ==  Cyc_Absyn_UnionType: 0){ _LL604: _temp603=(( struct Cyc_Absyn_UnionType_struct*)
_temp589)->f3; goto _LL594;} else{ goto _LL595;} _LL595: if(( unsigned int)
_temp589 >  3u?*(( int*) _temp589) ==  Cyc_Absyn_AnonStructType: 0){ _LL606:
_temp605=(( struct Cyc_Absyn_AnonStructType_struct*) _temp589)->f1; goto _LL596;}
else{ goto _LL597;} _LL597: if(( unsigned int) _temp589 >  3u?*(( int*) _temp589)
==  Cyc_Absyn_AnonUnionType: 0){ _LL608: _temp607=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp589)->f1; goto _LL598;} else{ goto _LL599;} _LL599: goto _LL600; _LL592: {
struct Cyc_Absyn_Structfield* sf= _temp601 ==  0? 0: Cyc_Absyn_lookup_struct_field((
struct Cyc_Absyn_Structdecl*)* _temp601, _temp516); if( sf ==  0? 1: !( sf->tq).q_const){
return;} goto _LL590;} _LL594: { struct Cyc_Absyn_Structfield* sf= _temp603 == 
0? 0: Cyc_Absyn_lookup_union_field(( struct Cyc_Absyn_Uniondecl*)* _temp603,
_temp516); if( sf ==  0? 1: !( sf->tq).q_const){ return;} goto _LL590;} _LL596:
_temp607= _temp605; goto _LL598; _LL598: { struct Cyc_Absyn_Structfield* sf= Cyc_Absyn_lookup_field(
_temp607, _temp516); if( sf ==  0? 1: !( sf->tq).q_const){ return;} goto _LL590;}
_LL600: goto _LL590; _LL590:;} goto _LL580; _LL584: goto _LL580; _LL580:;} goto
_LL469; _LL485:{ void* _temp609= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp520->topt))->v); struct Cyc_Absyn_PtrInfo _temp617; struct Cyc_Absyn_Tqual
_temp619; struct Cyc_Absyn_Tqual _temp621; _LL611: if(( unsigned int) _temp609 > 
3u?*(( int*) _temp609) ==  Cyc_Absyn_PointerType: 0){ _LL618: _temp617=(( struct
Cyc_Absyn_PointerType_struct*) _temp609)->f1; _LL620: _temp619= _temp617.tq;
goto _LL612;} else{ goto _LL613;} _LL613: if(( unsigned int) _temp609 >  3u?*((
int*) _temp609) ==  Cyc_Absyn_ArrayType: 0){ _LL622: _temp621=(( struct Cyc_Absyn_ArrayType_struct*)
_temp609)->f2; goto _LL614;} else{ goto _LL615;} _LL615: goto _LL616; _LL612:
_temp621= _temp619; goto _LL614; _LL614: if( ! _temp621.q_const){ return;} goto
_LL610; _LL616: goto _LL610; _LL610:;} goto _LL469; _LL487: _temp524= _temp522;
goto _LL489; _LL489: Cyc_Tcexp_check_writable( te, _temp524); return; _LL491:
goto _LL469; _LL469:;}({ struct Cyc_Std_String_pa_struct _temp624; _temp624.tag=
Cyc_Std_String_pa; _temp624.f1=( struct _tagged_arr) Cyc_Absynpp_exp2string( e);{
void* _temp623[ 1u]={& _temp624}; Cyc_Tcutil_terr( e->loc, _tag_arr("attempt to write a const location: %s",
sizeof( unsigned char), 38u), _tag_arr( _temp623, sizeof( void*), 1u));}});}
static void* Cyc_Tcexp_tcIncrement( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e, void* i){ Cyc_Tcexp_tcExpNoPromote(
te, 0, e); if( ! Cyc_Absyn_is_lvalue( e)){ return({ void* _temp625[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("increment/decrement of non-lvalue", sizeof( unsigned char),
34u), _tag_arr( _temp625, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable( te,
e);{ void* t=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v; if( !
Cyc_Tcutil_is_numeric( e)){ void* telt=( void*) Cyc_Absyn_VoidType; if( Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt)){ if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( telt),( void*) Cyc_Absyn_MemKind)){({
void* _temp626[ 0u]={}; Cyc_Tcutil_terr( e->loc, _tag_arr("can't perform arithmetic on abstract pointer type",
sizeof( unsigned char), 50u), _tag_arr( _temp626, sizeof( void*), 0u));});}}
else{({ struct Cyc_Std_String_pa_struct _temp628; _temp628.tag= Cyc_Std_String_pa;
_temp628.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp627[ 1u]={&
_temp628}; Cyc_Tcutil_terr( e->loc, _tag_arr("expecting arithmetic or ? type but found %s",
sizeof( unsigned char), 44u), _tag_arr( _temp627, sizeof( void*), 1u));}});}}
return t;}} static void* Cyc_Tcexp_tcConditional( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct
Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3){ Cyc_Tcexp_tcTest( te, e1, _tag_arr("conditional expression",
sizeof( unsigned char), 23u)); Cyc_Tcexp_tcExp( te, topt, e2); Cyc_Tcexp_tcExp(
te, topt, e3);{ void* t= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt* _temp636=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp636->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp636;}));
struct Cyc_List_List* _temp629=({ struct Cyc_List_List* _temp635=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp635->hd=( void*) e3; _temp635->tl=
0; _temp635;}); struct Cyc_List_List* _temp630=({ struct Cyc_List_List* _temp634=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp634->hd=(
void*) e2; _temp634->tl= _temp629; _temp634;}); if( ! Cyc_Tcutil_coerce_list( te,
t, _temp630)){({ struct Cyc_Std_String_pa_struct _temp633; _temp633.tag= Cyc_Std_String_pa;
_temp633.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e3->topt))->v);{ struct Cyc_Std_String_pa_struct _temp632; _temp632.tag=
Cyc_Std_String_pa; _temp632.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v);{ void* _temp631[ 2u]={&
_temp632,& _temp633}; Cyc_Tcutil_terr( loc, _tag_arr("conditional clause types do not match: %s != %s",
sizeof( unsigned char), 48u), _tag_arr( _temp631, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
return t;}} static void* Cyc_Tcexp_tcAssignOp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt*
po, struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExpNoPromote( te, 0, e1); Cyc_Tcexp_tcExp(
te,( void**)(( void**)(( void*)&(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v)),
e2);{ void* t1=( void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v; void*
t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e2->topt))->v;{ void* _temp637=
Cyc_Tcutil_compress( t1); _LL639: if(( unsigned int) _temp637 >  3u?*(( int*)
_temp637) ==  Cyc_Absyn_ArrayType: 0){ goto _LL640;} else{ goto _LL641;} _LL641:
goto _LL642; _LL640:({ void* _temp643[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("cannot assign to an array",
sizeof( unsigned char), 26u), _tag_arr( _temp643, sizeof( void*), 0u));}); goto
_LL638; _LL642: goto _LL638; _LL638:;} if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind(
t1),( void*) Cyc_Absyn_MemKind)){({ void* _temp644[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("type is abstract (can't determine size).", sizeof( unsigned char), 41u),
_tag_arr( _temp644, sizeof( void*), 0u));});} if( ! Cyc_Absyn_is_lvalue( e1)){
return({ void* _temp645[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("assignment to non-lvalue",
sizeof( unsigned char), 25u), _tag_arr( _temp645, sizeof( void*), 0u));});} Cyc_Tcexp_check_writable(
te, e1); if( po ==  0){ if( ! Cyc_Tcutil_coerce_assign( te, e2, t1)){ void*
_temp646=({ struct Cyc_Std_String_pa_struct _temp649; _temp649.tag= Cyc_Std_String_pa;
_temp649.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp648; _temp648.tag= Cyc_Std_String_pa; _temp648.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp647[ 2u]={& _temp648,& _temp649}; Cyc_Tcexp_expr_err( te, loc,
_tag_arr("type mismatch: %s != %s", sizeof( unsigned char), 24u), _tag_arr(
_temp647, sizeof( void*), 2u));}}}); Cyc_Tcutil_unify( t1, t2); Cyc_Tcutil_explain_failure();
return _temp646;}} else{ void* _temp650=( void*) po->v; void* _temp651= Cyc_Tcexp_tcBinPrimop(
te, loc, 0, _temp650, e1, e2); if( !( Cyc_Tcutil_unify( _temp651, t1)? 1:( Cyc_Tcutil_coerceable(
_temp651)? Cyc_Tcutil_coerceable( t1): 0))){ void* _temp652=({ struct Cyc_Std_String_pa_struct
_temp655; _temp655.tag= Cyc_Std_String_pa; _temp655.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t2);{ struct Cyc_Std_String_pa_struct _temp654; _temp654.tag= Cyc_Std_String_pa;
_temp654.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp653[
2u]={& _temp654,& _temp655}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("Cannot use this operator in an assignment when the arguments have types %s and %s",
sizeof( unsigned char), 82u), _tag_arr( _temp653, sizeof( void*), 2u));}}}); Cyc_Tcutil_unify(
_temp651, t1); Cyc_Tcutil_explain_failure(); return _temp652;} return _temp651;}
return t1;}} static void* Cyc_Tcexp_tcSeqExp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, topt, e2); return( void*)((
struct Cyc_Core_Opt*) _check_null( e2->topt))->v;} static struct Cyc_Absyn_DoubleType_struct
Cyc_Tcexp_dbl={ 6u, 0}; static void* Cyc_Tcexp_dbl_t=( void*)& Cyc_Tcexp_dbl;
static struct Cyc_Absyn_Tunionfield* Cyc_Tcexp_tcInjection( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Exp* e, void* tu, struct _RegionHandle* r, struct Cyc_List_List*
inst, struct Cyc_List_List* fs){ struct Cyc_List_List* fields; void* t1=( void*)((
struct Cyc_Core_Opt*) _check_null( e->topt))->v;{ void* _temp657= Cyc_Tcutil_compress(
t1); void* _temp667; void* _temp669; _LL659: if( _temp657 == ( void*) Cyc_Absyn_FloatType){
goto _LL660;} else{ goto _LL661;} _LL661: if(( unsigned int) _temp657 >  3u?*((
int*) _temp657) ==  Cyc_Absyn_IntType: 0){ _LL668: _temp667=( void*)(( struct
Cyc_Absyn_IntType_struct*) _temp657)->f2; if( _temp667 == ( void*) Cyc_Absyn_B1){
goto _LL662;} else{ goto _LL663;}} else{ goto _LL663;} _LL663: if(( unsigned int)
_temp657 >  3u?*(( int*) _temp657) ==  Cyc_Absyn_IntType: 0){ _LL670: _temp669=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp657)->f2; if( _temp669 == ( void*)
Cyc_Absyn_B2){ goto _LL664;} else{ goto _LL665;}} else{ goto _LL665;} _LL665:
goto _LL666; _LL660: Cyc_Tcutil_unchecked_cast( te, e, Cyc_Tcexp_dbl_t); t1= Cyc_Tcexp_dbl_t;
goto _LL658; _LL662: goto _LL664; _LL664: Cyc_Tcutil_unchecked_cast( te, e, Cyc_Absyn_sint_t);
t1= Cyc_Absyn_sint_t; goto _LL658; _LL666: goto _LL658; _LL658:;} for( fields=
fs; fields !=  0; fields= fields->tl){ void* _temp673; struct Cyc_Position_Segment*
_temp675; struct Cyc_List_List* _temp677; struct Cyc_List_List* _temp679; struct
_tuple1* _temp681; struct Cyc_Absyn_Tunionfield _temp671=*(( struct Cyc_Absyn_Tunionfield*)
fields->hd); _LL682: _temp681= _temp671.name; goto _LL680; _LL680: _temp679=
_temp671.tvs; goto _LL678; _LL678: _temp677= _temp671.typs; goto _LL676; _LL676:
_temp675= _temp671.loc; goto _LL674; _LL674: _temp673=( void*) _temp671.sc; goto
_LL672; _LL672: if(( _temp677 ==  0? 1: _temp677->tl !=  0)? 1: _temp679 !=  0){
continue;}{ void* t2= Cyc_Tcutil_rsubstitute( r, inst,(*(( struct _tuple8*)
_temp677->hd)).f2); if( Cyc_Tcutil_unify( t1, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*) fields->hd);}}} for( fields= fs; fields !=  0;
fields= fields->tl){ void* _temp685; struct Cyc_Position_Segment* _temp687;
struct Cyc_List_List* _temp689; struct Cyc_List_List* _temp691; struct _tuple1*
_temp693; struct Cyc_Absyn_Tunionfield _temp683=*(( struct Cyc_Absyn_Tunionfield*)
fields->hd); _LL694: _temp693= _temp683.name; goto _LL692; _LL692: _temp691=
_temp683.tvs; goto _LL690; _LL690: _temp689= _temp683.typs; goto _LL688; _LL688:
_temp687= _temp683.loc; goto _LL686; _LL686: _temp685=( void*) _temp683.sc; goto
_LL684; _LL684: if(( _temp689 ==  0? 1: _temp689->tl !=  0)? 1: _temp691 !=  0){
continue;}{ void* t2= Cyc_Tcutil_rsubstitute( r, inst,(*(( struct _tuple8*)
_temp689->hd)).f2); if( Cyc_Tcutil_coerce_arg( te, e, t2)){ return( struct Cyc_Absyn_Tunionfield*)((
struct Cyc_Absyn_Tunionfield*) fields->hd);}}}({ struct Cyc_Std_String_pa_struct
_temp697; _temp697.tag= Cyc_Std_String_pa; _temp697.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ struct Cyc_Std_String_pa_struct _temp696; _temp696.tag= Cyc_Std_String_pa;
_temp696.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( tu);{ void* _temp695[
2u]={& _temp696,& _temp697}; Cyc_Tcutil_terr( e->loc, _tag_arr("can't find a field in %s to inject a value of type %s",
sizeof( unsigned char), 54u), _tag_arr( _temp695, sizeof( void*), 2u));}}});
return 0;} static void* Cyc_Tcexp_tcFnCall( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct Cyc_List_List*
args, struct Cyc_Absyn_VarargCallInfo** vararg_call_info){ struct Cyc_List_List*
_temp698= args; te= Cyc_Tcenv_new_block( loc, te); Cyc_Tcexp_tcExp( te, 0, e);{
void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
void* _temp699= t; struct Cyc_Absyn_PtrInfo _temp705; struct Cyc_Absyn_Conref*
_temp707; struct Cyc_Absyn_Tqual _temp709; struct Cyc_Absyn_Conref* _temp711;
void* _temp713; void* _temp715; _LL701: if(( unsigned int) _temp699 >  3u?*((
int*) _temp699) ==  Cyc_Absyn_PointerType: 0){ _LL706: _temp705=(( struct Cyc_Absyn_PointerType_struct*)
_temp699)->f1; _LL716: _temp715=( void*) _temp705.elt_typ; goto _LL714; _LL714:
_temp713=( void*) _temp705.rgn_typ; goto _LL712; _LL712: _temp711= _temp705.nullable;
goto _LL710; _LL710: _temp709= _temp705.tq; goto _LL708; _LL708: _temp707=
_temp705.bounds; goto _LL702;} else{ goto _LL703;} _LL703: goto _LL704; _LL702:
Cyc_Tcenv_check_rgn_accessible( te, loc, _temp713); Cyc_Tcutil_check_nonzero_bound(
loc, _temp707);{ void* _temp717= Cyc_Tcutil_compress( _temp715); struct Cyc_Absyn_FnInfo
_temp723; struct Cyc_List_List* _temp725; struct Cyc_List_List* _temp727; struct
Cyc_Absyn_VarargInfo* _temp729; int _temp731; struct Cyc_List_List* _temp733;
void* _temp735; struct Cyc_Core_Opt* _temp737; struct Cyc_List_List* _temp739;
_LL719: if(( unsigned int) _temp717 >  3u?*(( int*) _temp717) ==  Cyc_Absyn_FnType:
0){ _LL724: _temp723=(( struct Cyc_Absyn_FnType_struct*) _temp717)->f1; _LL740:
_temp739= _temp723.tvars; goto _LL738; _LL738: _temp737= _temp723.effect; goto
_LL736; _LL736: _temp735=( void*) _temp723.ret_typ; goto _LL734; _LL734:
_temp733= _temp723.args; goto _LL732; _LL732: _temp731= _temp723.c_varargs; goto
_LL730; _LL730: _temp729= _temp723.cyc_varargs; goto _LL728; _LL728: _temp727=
_temp723.rgn_po; goto _LL726; _LL726: _temp725= _temp723.attributes; goto _LL720;}
else{ goto _LL721;} _LL721: goto _LL722; _LL720: if( topt !=  0){ Cyc_Tcutil_unify(
_temp735,* topt);} while( _temp698 !=  0? _temp733 !=  0: 0) { struct Cyc_Absyn_Exp*
e1=( struct Cyc_Absyn_Exp*) _temp698->hd; void* t2=(*(( struct _tuple2*)
_temp733->hd)).f3; Cyc_Tcexp_tcExp( te,( void**)& t2, e1); if( ! Cyc_Tcutil_coerce_arg(
te, e1, t2)){({ struct Cyc_Std_String_pa_struct _temp743; _temp743.tag= Cyc_Std_String_pa;
_temp743.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ struct Cyc_Std_String_pa_struct
_temp742; _temp742.tag= Cyc_Std_String_pa; _temp742.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ void* _temp741[ 2u]={&
_temp742,& _temp743}; Cyc_Tcutil_terr( e1->loc, _tag_arr("actual argument has type \n%s\n but formal has type \n%s",
sizeof( unsigned char), 54u), _tag_arr( _temp741, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}
_temp698= _temp698->tl; _temp733= _temp733->tl;} if( _temp733 !=  0){({ void*
_temp744[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("too few arguments for function",
sizeof( unsigned char), 31u), _tag_arr( _temp744, sizeof( void*), 0u));});}
else{ if(( _temp698 !=  0? 1: _temp731)? 1: _temp729 !=  0){ if( _temp731){ for(
0; _temp698 !=  0; _temp698= _temp698->tl){ Cyc_Tcexp_tcExp( te, 0,( struct Cyc_Absyn_Exp*)
_temp698->hd);}} else{ if( _temp729 ==  0){({ void* _temp745[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("too many arguments for function", sizeof( unsigned char), 32u),
_tag_arr( _temp745, sizeof( void*), 0u));});} else{ int _temp748; void* _temp750;
struct Cyc_Absyn_VarargInfo _temp746=* _temp729; _LL751: _temp750=( void*)
_temp746.type; goto _LL749; _LL749: _temp748= _temp746.inject; goto _LL747;
_LL747: { struct Cyc_Absyn_VarargCallInfo* _temp752=({ struct Cyc_Absyn_VarargCallInfo*
_temp782=( struct Cyc_Absyn_VarargCallInfo*) _cycalloc( sizeof( struct Cyc_Absyn_VarargCallInfo));
_temp782->num_varargs= 0; _temp782->injectors= 0; _temp782->vai=( struct Cyc_Absyn_VarargInfo*)
_check_null( _temp729); _temp782;});* vararg_call_info=( struct Cyc_Absyn_VarargCallInfo*)
_temp752; if( ! _temp748){ for( 0; _temp698 !=  0; _temp698= _temp698->tl){
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp698->hd; _temp752->num_varargs
++; Cyc_Tcexp_tcExp( te,( void**)& _temp750, e1); if( ! Cyc_Tcutil_coerce_arg(
te, e1, _temp750)){({ struct Cyc_Std_String_pa_struct _temp755; _temp755.tag=
Cyc_Std_String_pa; _temp755.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e1->topt))->v);{ struct Cyc_Std_String_pa_struct
_temp754; _temp754.tag= Cyc_Std_String_pa; _temp754.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp750);{ void* _temp753[ 2u]={& _temp754,& _temp755}; Cyc_Tcutil_terr( loc,
_tag_arr("vararg requires type %s but argument has type %s", sizeof(
unsigned char), 49u), _tag_arr( _temp753, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();}}}
else{ void* _temp756= Cyc_Tcutil_compress( _temp750); struct Cyc_Absyn_TunionInfo
_temp762; void* _temp764; struct Cyc_List_List* _temp766; void* _temp768; struct
Cyc_Absyn_Tuniondecl** _temp770; struct Cyc_Absyn_Tuniondecl* _temp772; _LL758:
if(( unsigned int) _temp756 >  3u?*(( int*) _temp756) ==  Cyc_Absyn_TunionType:
0){ _LL763: _temp762=(( struct Cyc_Absyn_TunionType_struct*) _temp756)->f1;
_LL769: _temp768=( void*) _temp762.tunion_info; if(*(( int*) _temp768) ==  Cyc_Absyn_KnownTunion){
_LL771: _temp770=(( struct Cyc_Absyn_KnownTunion_struct*) _temp768)->f1;
_temp772=* _temp770; goto _LL767;} else{ goto _LL760;} _LL767: _temp766=
_temp762.targs; goto _LL765; _LL765: _temp764=( void*) _temp762.rgn; goto _LL759;}
else{ goto _LL760;} _LL760: goto _LL761; _LL759: { struct Cyc_Absyn_Tuniondecl*
_temp773=* Cyc_Tcenv_lookup_tuniondecl( te, loc, _temp772->name); struct Cyc_List_List*
fields= 0; if( _temp773->fields ==  0){({ struct Cyc_Std_String_pa_struct
_temp775; _temp775.tag= Cyc_Std_String_pa; _temp775.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
_temp750);{ void* _temp774[ 1u]={& _temp775}; Cyc_Tcutil_terr( loc, _tag_arr("can't inject into %s",
sizeof( unsigned char), 21u), _tag_arr( _temp774, sizeof( void*), 1u));}});}
else{ fields=( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null(
_temp773->fields))->v;} if( ! Cyc_Tcutil_unify( _temp764, Cyc_Tcenv_curr_rgn( te))){({
void* _temp776[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad region for injected varargs",
sizeof( unsigned char), 32u), _tag_arr( _temp776, sizeof( void*), 0u));});}{
struct _RegionHandle _temp777= _new_region("rgn"); struct _RegionHandle* rgn=&
_temp777; _push_region( rgn);{ struct Cyc_List_List* _temp778=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp773->tvs, _temp766);
for( 0; _temp698 !=  0; _temp698= _temp698->tl){ _temp752->num_varargs ++;{
struct Cyc_Absyn_Exp* e1=( struct Cyc_Absyn_Exp*) _temp698->hd; Cyc_Tcexp_tcExp(
te, 0, e1);{ struct Cyc_Absyn_Tunionfield* _temp779= Cyc_Tcexp_tcInjection( te,
e1, _temp750, rgn, _temp778, fields); if( _temp779 !=  0){ _temp752->injectors=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp752->injectors,({ struct Cyc_List_List* _temp780=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp780->hd=( void*)(( struct Cyc_Absyn_Tunionfield*)
_check_null( _temp779)); _temp780->tl= 0; _temp780;}));}}}}}; _pop_region( rgn);}
goto _LL757;} _LL761:({ void* _temp781[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("bad inject vararg type",
sizeof( unsigned char), 23u), _tag_arr( _temp781, sizeof( void*), 0u));}); goto
_LL757; _LL757:;}}}}}}{ struct Cyc_List_List* a= _temp725; for( 0; a !=  0; a= a->tl){
void* _temp783=( void*) a->hd; int _temp789; int _temp791; void* _temp793;
_LL785: if(( unsigned int) _temp783 >  16u?*(( int*) _temp783) ==  Cyc_Absyn_Format_att:
0){ _LL794: _temp793=( void*)(( struct Cyc_Absyn_Format_att_struct*) _temp783)->f1;
goto _LL792; _LL792: _temp791=(( struct Cyc_Absyn_Format_att_struct*) _temp783)->f2;
goto _LL790; _LL790: _temp789=(( struct Cyc_Absyn_Format_att_struct*) _temp783)->f3;
goto _LL786;} else{ goto _LL787;} _LL787: goto _LL788; _LL786:{ struct
_handler_cons _temp795; _push_handler(& _temp795);{ int _temp797= 0; if( setjmp(
_temp795.handler)){ _temp797= 1;} if( ! _temp797){{ struct Cyc_Absyn_Exp*
_temp798=(( struct Cyc_Absyn_Exp*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)(
args, _temp791 -  1); struct Cyc_Core_Opt* fmt_args; if( _temp789 ==  0){
fmt_args= 0;} else{ fmt_args=({ struct Cyc_Core_Opt* _temp799=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp799->v=( void*)(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, int i)) Cyc_List_nth_tail)( args, _temp789 -  1);
_temp799;});}{ void* _temp800= _temp793; _LL802: if( _temp800 == ( void*) Cyc_Absyn_Printf_ft){
goto _LL803;} else{ goto _LL804;} _LL804: if( _temp800 == ( void*) Cyc_Absyn_Scanf_ft){
goto _LL805;} else{ goto _LL801;} _LL803: Cyc_Tcexp_check_format_args( te,
_temp798, fmt_args, Cyc_Formatstr_get_format_typs); goto _LL801; _LL805: Cyc_Tcexp_check_format_args(
te, _temp798, fmt_args, Cyc_Formatstr_get_scanf_typs); goto _LL801; _LL801:;}};
_pop_handler();} else{ void* _temp796=( void*) _exn_thrown; void* _temp807=
_temp796; _LL809: if( _temp807 ==  Cyc_List_Nth){ goto _LL810;} else{ goto
_LL811;} _LL811: goto _LL812; _LL810:({ void* _temp813[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("bad format arguments", sizeof( unsigned char), 21u), _tag_arr(
_temp813, sizeof( void*), 0u));}); goto _LL808; _LL812:( void) _throw( _temp807);
_LL808:;}}} goto _LL784; _LL788: goto _LL784; _LL784:;}} Cyc_Tcenv_check_effect_accessible(
te, loc,( void*)(( struct Cyc_Core_Opt*) _check_null( _temp737))->v); Cyc_Tcenv_check_rgn_partial_order(
te, loc, _temp727); return _temp735; _LL722: return({ void* _temp814[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function", sizeof(
unsigned char), 29u), _tag_arr( _temp814, sizeof( void*), 0u));}); _LL718:;}
_LL704: return({ void* _temp815[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expected pointer to function",
sizeof( unsigned char), 29u), _tag_arr( _temp815, sizeof( void*), 0u));});
_LL700:;}} static void* Cyc_Tcexp_tcThrow( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_exn_typ,
e); if( ! Cyc_Tcutil_coerce_arg( te, e, Cyc_Absyn_exn_typ)){({ struct Cyc_Std_String_pa_struct
_temp817; _temp817.tag= Cyc_Std_String_pa; _temp817.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp816[ 1u]={&
_temp817}; Cyc_Tcutil_terr( loc, _tag_arr("expected xtunion exn but found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp816, sizeof( void*), 1u));}});}
return Cyc_Absyn_wildtyp(({ struct Cyc_Core_Opt* _temp818=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp818->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp818;}));} static void* Cyc_Tcexp_tcInstantiate( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_List_List* ts){ Cyc_Tcexp_tcExpNoInst( te, 0, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* t1= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);{ void* _temp819= t1;
struct Cyc_Absyn_PtrInfo _temp825; struct Cyc_Absyn_Conref* _temp827; struct Cyc_Absyn_Tqual
_temp829; struct Cyc_Absyn_Conref* _temp831; void* _temp833; void* _temp835;
_LL821: if(( unsigned int) _temp819 >  3u?*(( int*) _temp819) ==  Cyc_Absyn_PointerType:
0){ _LL826: _temp825=(( struct Cyc_Absyn_PointerType_struct*) _temp819)->f1;
_LL836: _temp835=( void*) _temp825.elt_typ; goto _LL834; _LL834: _temp833=( void*)
_temp825.rgn_typ; goto _LL832; _LL832: _temp831= _temp825.nullable; goto _LL830;
_LL830: _temp829= _temp825.tq; goto _LL828; _LL828: _temp827= _temp825.bounds;
goto _LL822;} else{ goto _LL823;} _LL823: goto _LL824; _LL822:{ void* _temp837=
Cyc_Tcutil_compress( _temp835); struct Cyc_Absyn_FnInfo _temp843; struct Cyc_List_List*
_temp845; struct Cyc_List_List* _temp847; struct Cyc_Absyn_VarargInfo* _temp849;
int _temp851; struct Cyc_List_List* _temp853; void* _temp855; struct Cyc_Core_Opt*
_temp857; struct Cyc_List_List* _temp859; _LL839: if(( unsigned int) _temp837 > 
3u?*(( int*) _temp837) ==  Cyc_Absyn_FnType: 0){ _LL844: _temp843=(( struct Cyc_Absyn_FnType_struct*)
_temp837)->f1; _LL860: _temp859= _temp843.tvars; goto _LL858; _LL858: _temp857=
_temp843.effect; goto _LL856; _LL856: _temp855=( void*) _temp843.ret_typ; goto
_LL854; _LL854: _temp853= _temp843.args; goto _LL852; _LL852: _temp851= _temp843.c_varargs;
goto _LL850; _LL850: _temp849= _temp843.cyc_varargs; goto _LL848; _LL848:
_temp847= _temp843.rgn_po; goto _LL846; _LL846: _temp845= _temp843.attributes;
goto _LL840;} else{ goto _LL841;} _LL841: goto _LL842; _LL840: { struct Cyc_List_List*
instantiation= 0; if( Cyc_List_length( ts) != (( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp859)){ return({ void* _temp861[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("type instantiation mismatch", sizeof( unsigned char), 28u),
_tag_arr( _temp861, sizeof( void*), 0u));});} for( 0; ts !=  0;( ts= ts->tl,
_temp859= _temp859->tl)){ void* k= Cyc_Tcutil_tvar_kind(( struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*) _check_null( _temp859))->hd); Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te), k, 1,( void*) ts->hd); instantiation=({
struct Cyc_List_List* _temp862=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp862->hd=( void*)({ struct _tuple5* _temp863=(
struct _tuple5*) _cycalloc( sizeof( struct _tuple5)); _temp863->f1=( struct Cyc_Absyn_Tvar*)
_temp859->hd; _temp863->f2=( void*) ts->hd; _temp863;}); _temp862->tl=
instantiation; _temp862;});}{ void* new_fn_typ= Cyc_Tcutil_substitute(
instantiation,( void*)({ struct Cyc_Absyn_FnType_struct* _temp867=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp867[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp868; _temp868.tag= Cyc_Absyn_FnType; _temp868.f1=({ struct Cyc_Absyn_FnInfo
_temp869; _temp869.tvars= 0; _temp869.effect= _temp857; _temp869.ret_typ=( void*)
_temp855; _temp869.args= _temp853; _temp869.c_varargs= _temp851; _temp869.cyc_varargs=
_temp849; _temp869.rgn_po= _temp847; _temp869.attributes= _temp845; _temp869;});
_temp868;}); _temp867;})); void* new_typ=( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp864=( struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp864[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp865; _temp865.tag= Cyc_Absyn_PointerType;
_temp865.f1=({ struct Cyc_Absyn_PtrInfo _temp866; _temp866.elt_typ=( void*)
new_fn_typ; _temp866.rgn_typ=( void*) _temp833; _temp866.nullable= _temp831;
_temp866.tq= _temp829; _temp866.bounds= _temp827; _temp866;}); _temp865;});
_temp864;}); return new_typ;}} _LL842: goto _LL838; _LL838:;} goto _LL820;
_LL824: goto _LL820; _LL820:;} return({ struct Cyc_Std_String_pa_struct _temp871;
_temp871.tag= Cyc_Std_String_pa; _temp871.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t1);{ void* _temp870[ 1u]={& _temp871}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting polymorphic type but found %s",
sizeof( unsigned char), 40u), _tag_arr( _temp870, sizeof( void*), 1u));}});}}
static void* Cyc_Tcexp_tcCast( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, struct Cyc_Absyn_Exp* e){ Cyc_Tcutil_check_type( loc,
te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, 1, t); Cyc_Tcexp_tcExp(
te, 0, e);{ void* t2=( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v;
if(( ! Cyc_Tcutil_silent_castable( te, loc, t2, t)? ! Cyc_Tcutil_castable( te,
loc, t2, t): 0)? ! Cyc_Tcutil_zero_to_null( te, t, e): 0){ Cyc_Tcutil_unify( t2,
t);{ void* _temp872=({ struct Cyc_Std_String_pa_struct _temp875; _temp875.tag=
Cyc_Std_String_pa; _temp875.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{
struct Cyc_Std_String_pa_struct _temp874; _temp874.tag= Cyc_Std_String_pa;
_temp874.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp873[
2u]={& _temp874,& _temp875}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("cannot cast %s to %s",
sizeof( unsigned char), 21u), _tag_arr( _temp873, sizeof( void*), 2u));}}}); Cyc_Tcutil_explain_failure();
return _temp872;}} return t;}} static void* Cyc_Tcexp_tcAddress( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Exp* e0, void** topt,
struct Cyc_Absyn_Exp* e){ void** _temp876= 0; struct Cyc_Absyn_Tqual _temp877=
Cyc_Absyn_empty_tqual(); if( topt !=  0){ void* _temp878= Cyc_Tcutil_compress(*
topt); struct Cyc_Absyn_PtrInfo _temp884; struct Cyc_Absyn_Tqual _temp886; void*
_temp888; _LL880: if(( unsigned int) _temp878 >  3u?*(( int*) _temp878) ==  Cyc_Absyn_PointerType:
0){ _LL885: _temp884=(( struct Cyc_Absyn_PointerType_struct*) _temp878)->f1;
_LL889: _temp888=( void*) _temp884.elt_typ; goto _LL887; _LL887: _temp886=
_temp884.tq; goto _LL881;} else{ goto _LL882;} _LL882: goto _LL883; _LL881:
_temp876=({ void** _temp890=( void**) _cycalloc( sizeof( void*)); _temp890[ 0]=
_temp888; _temp890;}); _temp877= _temp886; goto _LL879; _LL883: goto _LL879;
_LL879:;} Cyc_Tcexp_tcExpNoInst( te, _temp876, e);{ void* _temp891=( void*) e->r;
struct Cyc_Absyn_Exp* _temp901; struct Cyc_Absyn_Exp* _temp903; _LL893: if(*((
int*) _temp891) ==  Cyc_Absyn_Struct_e){ goto _LL894;} else{ goto _LL895;}
_LL895: if(*(( int*) _temp891) ==  Cyc_Absyn_Tuple_e){ goto _LL896;} else{ goto
_LL897;} _LL897: if(*(( int*) _temp891) ==  Cyc_Absyn_Subscript_e){ _LL904:
_temp903=(( struct Cyc_Absyn_Subscript_e_struct*) _temp891)->f1; goto _LL902;
_LL902: _temp901=(( struct Cyc_Absyn_Subscript_e_struct*) _temp891)->f2; goto
_LL898;} else{ goto _LL899;} _LL899: goto _LL900; _LL894: goto _LL896; _LL896:({
void* _temp905[ 0u]={}; Cyc_Tcutil_warn( loc, _tag_arr("& used to allocate",
sizeof( unsigned char), 19u), _tag_arr( _temp905, sizeof( void*), 0u));});{ void*(*
_temp906)( void* t, void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if(
topt !=  0){ void* _temp907= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo
_temp913; struct Cyc_Absyn_Conref* _temp915; _LL909: if(( unsigned int) _temp907
>  3u?*(( int*) _temp907) ==  Cyc_Absyn_PointerType: 0){ _LL914: _temp913=((
struct Cyc_Absyn_PointerType_struct*) _temp907)->f1; _LL916: _temp915= _temp913.nullable;
goto _LL910;} else{ goto _LL911;} _LL911: goto _LL912; _LL910: if((( int(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( _temp915)){ _temp906= Cyc_Absyn_star_typ;}
goto _LL908; _LL912: goto _LL908; _LL908:;} return _temp906(( void*)(( struct
Cyc_Core_Opt*) _check_null( e->topt))->v,( void*) Cyc_Absyn_HeapRgn, _temp877);}
_LL898:{ void* _temp917= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( _temp903->topt))->v); _LL919: if(( unsigned int) _temp917 >  3u?*((
int*) _temp917) ==  Cyc_Absyn_TupleType: 0){ goto _LL920;} else{ goto _LL921;}
_LL921: goto _LL922; _LL920: goto _LL918; _LL922:( void*)( e0->r=( void*)(( void*)({
struct Cyc_Absyn_Primop_e_struct* _temp923=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp923[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp924; _temp924.tag= Cyc_Absyn_Primop_e; _temp924.f1=(
void*)(( void*) Cyc_Absyn_Plus); _temp924.f2=({ struct Cyc_List_List* _temp925=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp925->hd=(
void*) _temp903; _temp925->tl=({ struct Cyc_List_List* _temp926=( struct Cyc_List_List*)
_cycalloc( sizeof( struct Cyc_List_List)); _temp926->hd=( void*) _temp901;
_temp926->tl= 0; _temp926;}); _temp925;}); _temp924;}); _temp923;}))); return
Cyc_Tcexp_tcPlus( te, _temp903, _temp901); _LL918:;} goto _LL892; _LL900: goto
_LL892; _LL892:;}{ void* _temp929; int _temp931; struct _tuple6 _temp927= Cyc_Tcutil_addressof_props(
te, e); _LL932: _temp931= _temp927.f1; goto _LL930; _LL930: _temp929= _temp927.f2;
goto _LL928; _LL928: { struct Cyc_Absyn_Tqual tq= Cyc_Absyn_empty_tqual(); if(
_temp931){ tq.q_const= 1;}{ void* t= Cyc_Absyn_at_typ(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v, _temp929, tq); return t;}}}} static void* Cyc_Tcexp_tcSizeof(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, void*
t){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind,
1, t); if( ! Cyc_Evexp_okay_szofarg( t)){({ struct Cyc_Std_String_pa_struct
_temp934; _temp934.tag= Cyc_Std_String_pa; _temp934.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ void* _temp933[ 1u]={& _temp934}; Cyc_Tcutil_terr( loc, _tag_arr("sizeof applied to type %s, which has unknown size here",
sizeof( unsigned char), 55u), _tag_arr( _temp933, sizeof( void*), 1u));}});} if(
topt !=  0){ void* _temp935= Cyc_Tcutil_compress(* topt); _LL937: if((
unsigned int) _temp935 >  3u?*(( int*) _temp935) ==  Cyc_Absyn_SizeofType: 0){
goto _LL938;} else{ goto _LL939;} _LL939: goto _LL940; _LL938: return( void*)({
struct Cyc_Absyn_SizeofType_struct* _temp941=( struct Cyc_Absyn_SizeofType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_SizeofType_struct)); _temp941[ 0]=({ struct
Cyc_Absyn_SizeofType_struct _temp942; _temp942.tag= Cyc_Absyn_SizeofType;
_temp942.f1=( void*) t; _temp942;}); _temp941;}); _LL940: goto _LL936; _LL936:;}
return Cyc_Absyn_uint_t;} int Cyc_Tcexp_structfield_has_name( struct _tagged_arr*
n, struct Cyc_Absyn_Structfield* sf){ return Cyc_Std_strcmp(* n,* sf->name) == 
0;} static void* Cyc_Tcexp_tcOffsetof( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, void* t, void* n){ Cyc_Tcutil_check_type( loc, te, Cyc_Tcenv_lookup_type_vars(
te),( void*) Cyc_Absyn_MemKind, 1, t);{ void* _temp943= n; struct _tagged_arr*
_temp949; unsigned int _temp951; _LL945: if(*(( int*) _temp943) ==  Cyc_Absyn_StructField){
_LL950: _temp949=(( struct Cyc_Absyn_StructField_struct*) _temp943)->f1; goto
_LL946;} else{ goto _LL947;} _LL947: if(*(( int*) _temp943) ==  Cyc_Absyn_TupleIndex){
_LL952: _temp951=(( struct Cyc_Absyn_TupleIndex_struct*) _temp943)->f1; goto
_LL948;} else{ goto _LL944;} _LL946: { int bad_type= 1;{ void* _temp953= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Structdecl** _temp965; struct _tuple1* _temp967; struct Cyc_List_List*
_temp969; struct Cyc_Absyn_Uniondecl** _temp971; struct _tuple1* _temp973;
struct Cyc_List_List* _temp975; _LL955: if(( unsigned int) _temp953 >  3u?*((
int*) _temp953) ==  Cyc_Absyn_StructType: 0){ _LL968: _temp967=(( struct Cyc_Absyn_StructType_struct*)
_temp953)->f1; goto _LL966; _LL966: _temp965=(( struct Cyc_Absyn_StructType_struct*)
_temp953)->f3; goto _LL956;} else{ goto _LL957;} _LL957: if(( unsigned int)
_temp953 >  3u?*(( int*) _temp953) ==  Cyc_Absyn_AnonStructType: 0){ _LL970:
_temp969=(( struct Cyc_Absyn_AnonStructType_struct*) _temp953)->f1; goto _LL958;}
else{ goto _LL959;} _LL959: if(( unsigned int) _temp953 >  3u?*(( int*) _temp953)
==  Cyc_Absyn_UnionType: 0){ _LL974: _temp973=(( struct Cyc_Absyn_UnionType_struct*)
_temp953)->f1; goto _LL972; _LL972: _temp971=(( struct Cyc_Absyn_UnionType_struct*)
_temp953)->f3; goto _LL960;} else{ goto _LL961;} _LL961: if(( unsigned int)
_temp953 >  3u?*(( int*) _temp953) ==  Cyc_Absyn_AnonUnionType: 0){ _LL976:
_temp975=(( struct Cyc_Absyn_AnonUnionType_struct*) _temp953)->f1; goto _LL962;}
else{ goto _LL963;} _LL963: goto _LL964; _LL956: if( _temp965 ==  0){ return({
void* _temp977[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked StructType",
sizeof( unsigned char), 33u), _tag_arr( _temp977, sizeof( void*), 0u));});} if((*
_temp965)->fields ==  0){ goto _LL954;} if( !(( int(*)( int(* pred)( struct
_tagged_arr*, struct Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name, _temp949,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp965)->fields))->v)){({
struct Cyc_Std_String_pa_struct _temp979; _temp979.tag= Cyc_Std_String_pa;
_temp979.f1=( struct _tagged_arr)* _temp949;{ void* _temp978[ 1u]={& _temp979};
Cyc_Tcutil_terr( loc, _tag_arr("no field of struct has name %s", sizeof(
unsigned char), 31u), _tag_arr( _temp978, sizeof( void*), 1u));}});} bad_type= 0;
goto _LL954; _LL958: if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct
Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcexp_structfield_has_name, _temp949, _temp969)){({ struct Cyc_Std_String_pa_struct
_temp981; _temp981.tag= Cyc_Std_String_pa; _temp981.f1=( struct _tagged_arr)*
_temp949;{ void* _temp980[ 1u]={& _temp981}; Cyc_Tcutil_terr( loc, _tag_arr("no field of struct has name %s",
sizeof( unsigned char), 31u), _tag_arr( _temp980, sizeof( void*), 1u));}});}
bad_type= 0; goto _LL954; _LL960: if( _temp971 ==  0){ return({ void* _temp982[
0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked UnionType", sizeof(
unsigned char), 32u), _tag_arr( _temp982, sizeof( void*), 0u));});} if((*
_temp971)->fields ==  0){ goto _LL954;} if( !(( int(*)( int(* pred)( struct
_tagged_arr*, struct Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct
Cyc_List_List* x)) Cyc_List_exists_c)( Cyc_Tcexp_structfield_has_name, _temp949,(
struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((* _temp971)->fields))->v)){({
struct Cyc_Std_String_pa_struct _temp984; _temp984.tag= Cyc_Std_String_pa;
_temp984.f1=( struct _tagged_arr)* _temp949;{ void* _temp983[ 1u]={& _temp984};
Cyc_Tcutil_terr( loc, _tag_arr("no field of union has name %s", sizeof(
unsigned char), 30u), _tag_arr( _temp983, sizeof( void*), 1u));}});} bad_type= 0;
goto _LL954; _LL962: if( !(( int(*)( int(* pred)( struct _tagged_arr*, struct
Cyc_Absyn_Structfield*), struct _tagged_arr* env, struct Cyc_List_List* x)) Cyc_List_exists_c)(
Cyc_Tcexp_structfield_has_name, _temp949, _temp975)){({ struct Cyc_Std_String_pa_struct
_temp986; _temp986.tag= Cyc_Std_String_pa; _temp986.f1=( struct _tagged_arr)*
_temp949;{ void* _temp985[ 1u]={& _temp986}; Cyc_Tcutil_terr( loc, _tag_arr("no field of anonymous union has name %s",
sizeof( unsigned char), 40u), _tag_arr( _temp985, sizeof( void*), 1u));}});}
bad_type= 0; goto _LL954; _LL964: goto _LL954; _LL954:;} if( bad_type){({ struct
Cyc_Std_String_pa_struct _temp988; _temp988.tag= Cyc_Std_String_pa; _temp988.f1=(
struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp987[ 1u]={& _temp988};
Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct/union type", sizeof(
unsigned char), 36u), _tag_arr( _temp987, sizeof( void*), 1u));}});} goto _LL944;}
_LL948: { int bad_type= 1;{ void* _temp989= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Structdecl**
_temp1005; struct Cyc_List_List* _temp1007; struct Cyc_Absyn_Uniondecl**
_temp1009; struct Cyc_List_List* _temp1011; struct Cyc_List_List* _temp1013;
struct Cyc_Absyn_TunionFieldInfo _temp1015; void* _temp1017; struct Cyc_Absyn_Tunionfield*
_temp1019; struct Cyc_Absyn_Tuniondecl* _temp1021; _LL991: if(( unsigned int)
_temp989 >  3u?*(( int*) _temp989) ==  Cyc_Absyn_StructType: 0){ _LL1006:
_temp1005=(( struct Cyc_Absyn_StructType_struct*) _temp989)->f3; goto _LL992;}
else{ goto _LL993;} _LL993: if(( unsigned int) _temp989 >  3u?*(( int*) _temp989)
==  Cyc_Absyn_AnonStructType: 0){ _LL1008: _temp1007=(( struct Cyc_Absyn_AnonStructType_struct*)
_temp989)->f1; goto _LL994;} else{ goto _LL995;} _LL995: if(( unsigned int)
_temp989 >  3u?*(( int*) _temp989) ==  Cyc_Absyn_UnionType: 0){ _LL1010:
_temp1009=(( struct Cyc_Absyn_UnionType_struct*) _temp989)->f3; goto _LL996;}
else{ goto _LL997;} _LL997: if(( unsigned int) _temp989 >  3u?*(( int*) _temp989)
==  Cyc_Absyn_AnonUnionType: 0){ _LL1012: _temp1011=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp989)->f1; goto _LL998;} else{ goto _LL999;} _LL999: if(( unsigned int)
_temp989 >  3u?*(( int*) _temp989) ==  Cyc_Absyn_TupleType: 0){ _LL1014:
_temp1013=(( struct Cyc_Absyn_TupleType_struct*) _temp989)->f1; goto _LL1000;}
else{ goto _LL1001;} _LL1001: if(( unsigned int) _temp989 >  3u?*(( int*)
_temp989) ==  Cyc_Absyn_TunionFieldType: 0){ _LL1016: _temp1015=(( struct Cyc_Absyn_TunionFieldType_struct*)
_temp989)->f1; _LL1018: _temp1017=( void*) _temp1015.field_info; if(*(( int*)
_temp1017) ==  Cyc_Absyn_KnownTunionfield){ _LL1022: _temp1021=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1017)->f1; goto _LL1020; _LL1020: _temp1019=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1017)->f2; goto _LL1002;} else{ goto _LL1003;}} else{ goto _LL1003;}
_LL1003: goto _LL1004; _LL992: if( _temp1005 ==  0){ return({ void* _temp1023[ 0u]={};
Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked StructType", sizeof(
unsigned char), 33u), _tag_arr( _temp1023, sizeof( void*), 0u));});} if((*
_temp1005)->fields ==  0){ goto _LL990;} if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((*
_temp1005)->fields))->v) <=  _temp951){({ struct Cyc_Std_Int_pa_struct _temp1026;
_temp1026.tag= Cyc_Std_Int_pa; _temp1026.f1=( int) _temp951;{ struct Cyc_Std_Int_pa_struct
_temp1025; _temp1025.tag= Cyc_Std_Int_pa; _temp1025.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((* _temp1005)->fields))->v));{ void* _temp1024[
2u]={& _temp1025,& _temp1026}; Cyc_Tcutil_terr( loc, _tag_arr("struct has too few components: %d <= %d",
sizeof( unsigned char), 40u), _tag_arr( _temp1024, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL990; _LL994: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1007) <=  _temp951){({ struct Cyc_Std_Int_pa_struct _temp1029; _temp1029.tag=
Cyc_Std_Int_pa; _temp1029.f1=( int) _temp951;{ struct Cyc_Std_Int_pa_struct
_temp1028; _temp1028.tag= Cyc_Std_Int_pa; _temp1028.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1007));{ void*
_temp1027[ 2u]={& _temp1028,& _temp1029}; Cyc_Tcutil_terr( loc, _tag_arr("struct has too few components: %d <= %d",
sizeof( unsigned char), 40u), _tag_arr( _temp1027, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL990; _LL996: if( _temp1009 ==  0){ return({ void* _temp1030[
0u]={}; Cyc_Tcutil_impos( _tag_arr("offsetof on unchecked UnionType", sizeof(
unsigned char), 32u), _tag_arr( _temp1030, sizeof( void*), 0u));});} if((*
_temp1009)->fields ==  0){ goto _LL990;} if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)(( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null((*
_temp1009)->fields))->v) <=  _temp951){({ struct Cyc_Std_Int_pa_struct _temp1033;
_temp1033.tag= Cyc_Std_Int_pa; _temp1033.f1=( int) _temp951;{ struct Cyc_Std_Int_pa_struct
_temp1032; _temp1032.tag= Cyc_Std_Int_pa; _temp1032.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)(( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null((* _temp1009)->fields))->v));{ void* _temp1031[
2u]={& _temp1032,& _temp1033}; Cyc_Tcutil_terr( loc, _tag_arr("union has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp1031, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL990; _LL998: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1011) <=  _temp951){({ struct Cyc_Std_Int_pa_struct _temp1036; _temp1036.tag=
Cyc_Std_Int_pa; _temp1036.f1=( int) _temp951;{ struct Cyc_Std_Int_pa_struct
_temp1035; _temp1035.tag= Cyc_Std_Int_pa; _temp1035.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1011));{ void*
_temp1034[ 2u]={& _temp1035,& _temp1036}; Cyc_Tcutil_terr( loc, _tag_arr("union has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp1034, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL990; _LL1000: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1013) <=  _temp951){({ struct Cyc_Std_Int_pa_struct _temp1039; _temp1039.tag=
Cyc_Std_Int_pa; _temp1039.f1=( int) _temp951;{ struct Cyc_Std_Int_pa_struct
_temp1038; _temp1038.tag= Cyc_Std_Int_pa; _temp1038.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1013));{ void*
_temp1037[ 2u]={& _temp1038,& _temp1039}; Cyc_Tcutil_terr( loc, _tag_arr("tuple has too few components: %d <= %d",
sizeof( unsigned char), 39u), _tag_arr( _temp1037, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL990; _LL1002: if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
_temp1019->typs) <  _temp951){({ struct Cyc_Std_Int_pa_struct _temp1042;
_temp1042.tag= Cyc_Std_Int_pa; _temp1042.f1=( int) _temp951;{ struct Cyc_Std_Int_pa_struct
_temp1041; _temp1041.tag= Cyc_Std_Int_pa; _temp1041.f1=( int)(( unsigned int)((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp1019->typs));{ void*
_temp1040[ 2u]={& _temp1041,& _temp1042}; Cyc_Tcutil_terr( loc, _tag_arr("tunionfield has too few components: %d < %d",
sizeof( unsigned char), 44u), _tag_arr( _temp1040, sizeof( void*), 2u));}}});}
bad_type= 0; goto _LL990; _LL1004: goto _LL990; _LL990:;} if( bad_type){({
struct Cyc_Std_String_pa_struct _temp1044; _temp1044.tag= Cyc_Std_String_pa;
_temp1044.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1043[
1u]={& _temp1044}; Cyc_Tcutil_terr( loc, _tag_arr("%s is not a known struct/union/tuple/tunion type",
sizeof( unsigned char), 49u), _tag_arr( _temp1043, sizeof( void*), 1u));}});}
goto _LL944;} _LL944:;} return Cyc_Absyn_uint_t;} static void* Cyc_Tcexp_tcDeref(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* e){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1045= t;
struct Cyc_Absyn_PtrInfo _temp1051; struct Cyc_Absyn_Conref* _temp1053; void*
_temp1055; void* _temp1057; _LL1047: if(( unsigned int) _temp1045 >  3u?*(( int*)
_temp1045) ==  Cyc_Absyn_PointerType: 0){ _LL1052: _temp1051=(( struct Cyc_Absyn_PointerType_struct*)
_temp1045)->f1; _LL1058: _temp1057=( void*) _temp1051.elt_typ; goto _LL1056;
_LL1056: _temp1055=( void*) _temp1051.rgn_typ; goto _LL1054; _LL1054: _temp1053=
_temp1051.bounds; goto _LL1048;} else{ goto _LL1049;} _LL1049: goto _LL1050;
_LL1048: Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1055); Cyc_Tcutil_check_nonzero_bound(
loc, _temp1053); if( ! Cyc_Tcutil_kind_leq( Cyc_Tcutil_typ_kind( _temp1057),(
void*) Cyc_Absyn_MemKind)){({ void* _temp1059[ 0u]={}; Cyc_Tcutil_terr( loc,
_tag_arr("can't dereference abstract pointer type", sizeof( unsigned char), 40u),
_tag_arr( _temp1059, sizeof( void*), 0u));});} return _temp1057; _LL1050: return({
struct Cyc_Std_String_pa_struct _temp1061; _temp1061.tag= Cyc_Std_String_pa;
_temp1061.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1060[
1u]={& _temp1061}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting * or @ type but found %s",
sizeof( unsigned char), 35u), _tag_arr( _temp1060, sizeof( void*), 1u));}});
_LL1046:;}} static void* Cyc_Tcexp_tcStructMember( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* outer_e,
struct Cyc_Absyn_Exp* e, struct _tagged_arr* f){ Cyc_Tcexp_tcExpNoPromote( te, 0,
e);{ void* t= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e->topt))->v); void* _temp1062= t; struct Cyc_Absyn_Structdecl** _temp1078;
struct Cyc_Absyn_Structdecl* _temp1080; struct Cyc_List_List* _temp1081; struct
_tuple1* _temp1083; struct Cyc_List_List* _temp1085; struct Cyc_List_List*
_temp1087; struct Cyc_Absyn_Uniondecl** _temp1089; struct Cyc_Absyn_Uniondecl*
_temp1091; struct Cyc_List_List* _temp1092; struct _tuple1* _temp1094; _LL1064:
if(( unsigned int) _temp1062 >  3u?*(( int*) _temp1062) ==  Cyc_Absyn_StructType:
0){ _LL1084: _temp1083=(( struct Cyc_Absyn_StructType_struct*) _temp1062)->f1;
goto _LL1082; _LL1082: _temp1081=(( struct Cyc_Absyn_StructType_struct*)
_temp1062)->f2; goto _LL1079; _LL1079: _temp1078=(( struct Cyc_Absyn_StructType_struct*)
_temp1062)->f3; if( _temp1078 ==  0){ goto _LL1066;} else{ _temp1080=* _temp1078;
goto _LL1065;}} else{ goto _LL1066;} _LL1066: if(( unsigned int) _temp1062 >  3u?*((
int*) _temp1062) ==  Cyc_Absyn_AnonStructType: 0){ _LL1086: _temp1085=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1062)->f1; goto _LL1067;} else{ goto
_LL1068;} _LL1068: if(( unsigned int) _temp1062 >  3u?*(( int*) _temp1062) == 
Cyc_Absyn_AnonUnionType: 0){ _LL1088: _temp1087=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1062)->f1; goto _LL1069;} else{ goto _LL1070;} _LL1070: if(( unsigned int)
_temp1062 >  3u?*(( int*) _temp1062) ==  Cyc_Absyn_UnionType: 0){ _LL1095:
_temp1094=(( struct Cyc_Absyn_UnionType_struct*) _temp1062)->f1; goto _LL1093;
_LL1093: _temp1092=(( struct Cyc_Absyn_UnionType_struct*) _temp1062)->f2; goto
_LL1090; _LL1090: _temp1089=(( struct Cyc_Absyn_UnionType_struct*) _temp1062)->f3;
if( _temp1089 ==  0){ goto _LL1072;} else{ _temp1091=* _temp1089; goto _LL1071;}}
else{ goto _LL1072;} _LL1072: if(( unsigned int) _temp1062 >  3u?*(( int*)
_temp1062) ==  Cyc_Absyn_ArrayType: 0){ goto _LL1096;} else{ goto _LL1074;}
_LL1096: if( Cyc_Std_zstrcmp(* f, _tag_arr("size", sizeof( unsigned char), 5u))
==  0){ goto _LL1073;} else{ goto _LL1074;} _LL1074: if(( unsigned int)
_temp1062 >  3u?*(( int*) _temp1062) ==  Cyc_Absyn_PointerType: 0){ goto _LL1097;}
else{ goto _LL1076;} _LL1097: if( Cyc_Std_zstrcmp(* f, _tag_arr("size", sizeof(
unsigned char), 5u)) ==  0){ goto _LL1075;} else{ goto _LL1076;} _LL1076: goto
_LL1077; _LL1065: if( _temp1083 ==  0){ return({ void* _temp1098[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved struct type (COMPILER ERROR)", sizeof(
unsigned char), 40u), _tag_arr( _temp1098, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp1099= Cyc_Absyn_lookup_struct_field( _temp1080, f); if( _temp1099 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp1102; _temp1102.tag= Cyc_Std_String_pa;
_temp1102.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct
_temp1101; _temp1101.tag= Cyc_Std_String_pa; _temp1101.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1083));{ void*
_temp1100[ 2u]={& _temp1101,& _temp1102}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct %s has no %s field",
sizeof( unsigned char), 26u), _tag_arr( _temp1100, sizeof( void*), 2u));}}});}{
void* t2;{ struct _RegionHandle _temp1103= _new_region("rgn"); struct
_RegionHandle* rgn=& _temp1103; _push_region( rgn);{ struct Cyc_List_List*
_temp1104=(( struct Cyc_List_List*(*)( struct _RegionHandle* r1, struct
_RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
rgn, rgn, _temp1080->tvs, _temp1081); t2= Cyc_Tcutil_rsubstitute( rgn, _temp1104,(
void*) _temp1099->type);}; _pop_region( rgn);} return t2;}} _LL1067: { struct
Cyc_Absyn_Structfield* _temp1105= Cyc_Absyn_lookup_field( _temp1085, f); if(
_temp1105 ==  0){ return({ struct Cyc_Std_String_pa_struct _temp1107; _temp1107.tag=
Cyc_Std_String_pa; _temp1107.f1=( struct _tagged_arr)* f;{ void* _temp1106[ 1u]={&
_temp1107}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("struct has no %s field",
sizeof( unsigned char), 23u), _tag_arr( _temp1106, sizeof( void*), 1u));}});}
return( void*) _temp1105->type;} _LL1069: { struct Cyc_Absyn_Structfield*
_temp1108= Cyc_Absyn_lookup_field( _temp1087, f); if( _temp1108 ==  0){ return({
struct Cyc_Std_String_pa_struct _temp1110; _temp1110.tag= Cyc_Std_String_pa;
_temp1110.f1=( struct _tagged_arr)* f;{ void* _temp1109[ 1u]={& _temp1110}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("union has no %s field", sizeof( unsigned char), 22u),
_tag_arr( _temp1109, sizeof( void*), 1u));}});} return( void*) _temp1108->type;}
_LL1071: if( _temp1094 ==  0){ return({ void* _temp1111[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("unresolved union type (COMPILER ERROR)", sizeof(
unsigned char), 39u), _tag_arr( _temp1111, sizeof( void*), 0u));});}{ struct Cyc_Absyn_Structfield*
_temp1112= Cyc_Absyn_lookup_union_field( _temp1091, f); if( _temp1112 ==  0){
return({ struct Cyc_Std_String_pa_struct _temp1115; _temp1115.tag= Cyc_Std_String_pa;
_temp1115.f1=( struct _tagged_arr)* f;{ struct Cyc_Std_String_pa_struct
_temp1114; _temp1114.tag= Cyc_Std_String_pa; _temp1114.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string(( struct _tuple1*) _check_null( _temp1094));{ void*
_temp1113[ 2u]={& _temp1114,& _temp1115}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union %s has no %s field",
sizeof( unsigned char), 25u), _tag_arr( _temp1113, sizeof( void*), 2u));}}});}{
struct Cyc_List_List* _temp1116=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_zip)( _temp1091->tvs, _temp1092); void*
_temp1117= Cyc_Tcutil_substitute( _temp1116,( void*) _temp1112->type); return
_temp1117;}} _LL1073:( void*)( outer_e->r=( void*)(( void*)({ struct Cyc_Absyn_Primop_e_struct*
_temp1118=( struct Cyc_Absyn_Primop_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct));
_temp1118[ 0]=({ struct Cyc_Absyn_Primop_e_struct _temp1119; _temp1119.tag= Cyc_Absyn_Primop_e;
_temp1119.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1119.f2=({ struct Cyc_List_List*
_temp1120=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1120->hd=( void*) e; _temp1120->tl= 0; _temp1120;}); _temp1119;});
_temp1118;}))); return Cyc_Absyn_uint_t; _LL1075:( void*)( outer_e->r=( void*)((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp1121=( struct Cyc_Absyn_Primop_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp1121[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp1122; _temp1122.tag= Cyc_Absyn_Primop_e;
_temp1122.f1=( void*)(( void*) Cyc_Absyn_Size); _temp1122.f2=({ struct Cyc_List_List*
_temp1123=( struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List));
_temp1123->hd=( void*) e; _temp1123->tl= 0; _temp1123;}); _temp1122;});
_temp1121;}))); return Cyc_Absyn_uint_t; _LL1077: if( Cyc_Std_zstrcmp(* f,
_tag_arr("size", sizeof( unsigned char), 5u)) ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1125; _temp1125.tag= Cyc_Std_String_pa; _temp1125.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1124[ 1u]={& _temp1125}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct, union or array, found %s", sizeof(
unsigned char), 43u), _tag_arr( _temp1124, sizeof( void*), 1u));}});} else{
return({ struct Cyc_Std_String_pa_struct _temp1127; _temp1127.tag= Cyc_Std_String_pa;
_temp1127.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1126[
1u]={& _temp1127}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct or union, found %s",
sizeof( unsigned char), 36u), _tag_arr( _temp1126, sizeof( void*), 1u));}});}
_LL1063:;}} static void* Cyc_Tcexp_tcStructArrow( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e, struct
_tagged_arr* f){ Cyc_Tcexp_tcExp( te, 0, e);{ void* t= Cyc_Tcutil_compress((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v); void* _temp1128= t;
struct Cyc_Absyn_PtrInfo _temp1134; struct Cyc_Absyn_Conref* _temp1136; void*
_temp1138; void* _temp1140; _LL1130: if(( unsigned int) _temp1128 >  3u?*(( int*)
_temp1128) ==  Cyc_Absyn_PointerType: 0){ _LL1135: _temp1134=(( struct Cyc_Absyn_PointerType_struct*)
_temp1128)->f1; _LL1141: _temp1140=( void*) _temp1134.elt_typ; goto _LL1139;
_LL1139: _temp1138=( void*) _temp1134.rgn_typ; goto _LL1137; _LL1137: _temp1136=
_temp1134.bounds; goto _LL1131;} else{ goto _LL1132;} _LL1132: goto _LL1133;
_LL1131: Cyc_Tcutil_check_nonzero_bound( loc, _temp1136);{ void* _temp1142= Cyc_Tcutil_compress(
_temp1140); struct Cyc_Absyn_Structdecl** _temp1154; struct Cyc_Absyn_Structdecl*
_temp1156; struct Cyc_List_List* _temp1157; struct _tuple1* _temp1159; struct
Cyc_Absyn_Uniondecl** _temp1161; struct Cyc_Absyn_Uniondecl* _temp1163; struct
Cyc_List_List* _temp1164; struct _tuple1* _temp1166; struct Cyc_List_List*
_temp1168; struct Cyc_List_List* _temp1170; _LL1144: if(( unsigned int)
_temp1142 >  3u?*(( int*) _temp1142) ==  Cyc_Absyn_StructType: 0){ _LL1160:
_temp1159=(( struct Cyc_Absyn_StructType_struct*) _temp1142)->f1; goto _LL1158;
_LL1158: _temp1157=(( struct Cyc_Absyn_StructType_struct*) _temp1142)->f2; goto
_LL1155; _LL1155: _temp1154=(( struct Cyc_Absyn_StructType_struct*) _temp1142)->f3;
if( _temp1154 ==  0){ goto _LL1146;} else{ _temp1156=* _temp1154; goto _LL1145;}}
else{ goto _LL1146;} _LL1146: if(( unsigned int) _temp1142 >  3u?*(( int*)
_temp1142) ==  Cyc_Absyn_UnionType: 0){ _LL1167: _temp1166=(( struct Cyc_Absyn_UnionType_struct*)
_temp1142)->f1; goto _LL1165; _LL1165: _temp1164=(( struct Cyc_Absyn_UnionType_struct*)
_temp1142)->f2; goto _LL1162; _LL1162: _temp1161=(( struct Cyc_Absyn_UnionType_struct*)
_temp1142)->f3; if( _temp1161 ==  0){ goto _LL1148;} else{ _temp1163=* _temp1161;
goto _LL1147;}} else{ goto _LL1148;} _LL1148: if(( unsigned int) _temp1142 >  3u?*((
int*) _temp1142) ==  Cyc_Absyn_AnonStructType: 0){ _LL1169: _temp1168=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1142)->f1; goto _LL1149;} else{ goto
_LL1150;} _LL1150: if(( unsigned int) _temp1142 >  3u?*(( int*) _temp1142) == 
Cyc_Absyn_AnonUnionType: 0){ _LL1171: _temp1170=(( struct Cyc_Absyn_AnonUnionType_struct*)
_temp1142)->f1; goto _LL1151;} else{ goto _LL1152;} _LL1152: goto _LL1153;
_LL1145: { struct Cyc_Absyn_Structfield* _temp1172= Cyc_Absyn_lookup_struct_field(
_temp1156, f); if( _temp1172 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1175; _temp1175.tag= Cyc_Std_String_pa; _temp1175.f1=( struct _tagged_arr)*
f;{ struct Cyc_Std_String_pa_struct _temp1174; _temp1174.tag= Cyc_Std_String_pa;
_temp1174.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1159));{ void* _temp1173[ 2u]={& _temp1174,& _temp1175}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct %s has no %s field", sizeof( unsigned char), 26u),
_tag_arr( _temp1173, sizeof( void*), 2u));}}});}{ void* t3;{ struct
_RegionHandle _temp1176= _new_region("rgn"); struct _RegionHandle* rgn=&
_temp1176; _push_region( rgn);{ struct Cyc_List_List* _temp1177=(( struct Cyc_List_List*(*)(
struct _RegionHandle* r1, struct _RegionHandle* r2, struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_rzip)( rgn, rgn, _temp1156->tvs, _temp1157);
t3= Cyc_Tcutil_rsubstitute( rgn, _temp1177,( void*) _temp1172->type);};
_pop_region( rgn);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1138); return
t3;}} _LL1147: { struct Cyc_Absyn_Structfield* _temp1178= Cyc_Absyn_lookup_union_field(
_temp1163, f); if( _temp1178 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1181; _temp1181.tag= Cyc_Std_String_pa; _temp1181.f1=( struct _tagged_arr)*
f;{ struct Cyc_Std_String_pa_struct _temp1180; _temp1180.tag= Cyc_Std_String_pa;
_temp1180.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(( struct _tuple1*)
_check_null( _temp1166));{ void* _temp1179[ 2u]={& _temp1180,& _temp1181}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("union %s has no %s field", sizeof( unsigned char), 25u),
_tag_arr( _temp1179, sizeof( void*), 2u));}}});}{ struct Cyc_List_List*
_temp1182=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_zip)( _temp1163->tvs, _temp1164); void* _temp1183= Cyc_Tcutil_substitute(
_temp1182,( void*) _temp1178->type); Cyc_Tcenv_check_rgn_accessible( te, loc,
_temp1138); return _temp1183;}} _LL1149: { struct Cyc_Absyn_Structfield*
_temp1184= Cyc_Absyn_lookup_field( _temp1168, f); if( _temp1184 ==  0){ return({
struct Cyc_Std_String_pa_struct _temp1186; _temp1186.tag= Cyc_Std_String_pa;
_temp1186.f1=( struct _tagged_arr)* f;{ void* _temp1185[ 1u]={& _temp1186}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("struct has no %s field", sizeof( unsigned char), 23u),
_tag_arr( _temp1185, sizeof( void*), 1u));}});} return( void*) _temp1184->type;}
_LL1151: { struct Cyc_Absyn_Structfield* _temp1187= Cyc_Absyn_lookup_field(
_temp1170, f); if( _temp1187 ==  0){ return({ struct Cyc_Std_String_pa_struct
_temp1189; _temp1189.tag= Cyc_Std_String_pa; _temp1189.f1=( struct _tagged_arr)*
f;{ void* _temp1188[ 1u]={& _temp1189}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("union has no %s field",
sizeof( unsigned char), 22u), _tag_arr( _temp1188, sizeof( void*), 1u));}});}
return( void*) _temp1187->type;} _LL1153: return({ struct Cyc_Std_String_pa_struct
_temp1191; _temp1191.tag= Cyc_Std_String_pa; _temp1191.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1190[ 1u]={& _temp1191}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("expecting struct pointer, found %s", sizeof( unsigned char),
35u), _tag_arr( _temp1190, sizeof( void*), 1u));}}); _LL1143:;} _LL1133: return({
struct Cyc_Std_String_pa_struct _temp1193; _temp1193.tag= Cyc_Std_String_pa;
_temp1193.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t);{ void* _temp1192[
1u]={& _temp1193}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("expecting struct pointer, found %s",
sizeof( unsigned char), 35u), _tag_arr( _temp1192, sizeof( void*), 1u));}});
_LL1129:;}} static void* Cyc_Tcexp_ithTupleType( struct Cyc_Tcenv_Tenv* te,
struct Cyc_Position_Segment* loc, struct Cyc_List_List* ts, struct Cyc_Absyn_Exp*
index){ unsigned int i= Cyc_Evexp_eval_const_uint_exp( index); struct
_handler_cons _temp1194; _push_handler(& _temp1194);{ int _temp1196= 0; if(
setjmp( _temp1194.handler)){ _temp1196= 1;} if( ! _temp1196){{ void* _temp1197=(*((
struct _tuple8*(*)( struct Cyc_List_List* x, int n)) Cyc_List_nth)( ts,( int) i)).f2;
_npop_handler( 0u); return _temp1197;}; _pop_handler();} else{ void* _temp1195=(
void*) _exn_thrown; void* _temp1199= _temp1195; _LL1201: if( _temp1199 ==  Cyc_List_Nth){
goto _LL1202;} else{ goto _LL1203;} _LL1203: goto _LL1204; _LL1202: return({
struct Cyc_Std_Int_pa_struct _temp1207; _temp1207.tag= Cyc_Std_Int_pa; _temp1207.f1=(
int)(( unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( ts));{
struct Cyc_Std_Int_pa_struct _temp1206; _temp1206.tag= Cyc_Std_Int_pa; _temp1206.f1=(
int) i;{ void* _temp1205[ 2u]={& _temp1206,& _temp1207}; Cyc_Tcexp_expr_err( te,
loc, _tag_arr("index is %d but tuple has only %d fields", sizeof( unsigned char),
41u), _tag_arr( _temp1205, sizeof( void*), 2u));}}}); _LL1204:( void) _throw(
_temp1199); _LL1200:;}}} static void* Cyc_Tcexp_tcSubscript( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2){ Cyc_Tcexp_tcExp( te, 0, e1); Cyc_Tcexp_tcExp( te, 0,
e2);{ void* t1= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null(
e1->topt))->v); void* t2= Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*)
_check_null( e2->topt))->v); if( ! Cyc_Tcutil_coerce_sint_typ( te, e2)){ return({
struct Cyc_Std_String_pa_struct _temp1209; _temp1209.tag= Cyc_Std_String_pa;
_temp1209.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t2);{ void* _temp1208[
1u]={& _temp1209}; Cyc_Tcexp_expr_err( te, e2->loc, _tag_arr("expecting int subscript, found %s",
sizeof( unsigned char), 34u), _tag_arr( _temp1208, sizeof( void*), 1u));}});}{
void* _temp1210= t1; struct Cyc_Absyn_PtrInfo _temp1218; struct Cyc_Absyn_Conref*
_temp1220; struct Cyc_Absyn_Tqual _temp1222; void* _temp1224; void* _temp1226;
struct Cyc_List_List* _temp1228; _LL1212: if(( unsigned int) _temp1210 >  3u?*((
int*) _temp1210) ==  Cyc_Absyn_PointerType: 0){ _LL1219: _temp1218=(( struct Cyc_Absyn_PointerType_struct*)
_temp1210)->f1; _LL1227: _temp1226=( void*) _temp1218.elt_typ; goto _LL1225;
_LL1225: _temp1224=( void*) _temp1218.rgn_typ; goto _LL1223; _LL1223: _temp1222=
_temp1218.tq; goto _LL1221; _LL1221: _temp1220= _temp1218.bounds; goto _LL1213;}
else{ goto _LL1214;} _LL1214: if(( unsigned int) _temp1210 >  3u?*(( int*)
_temp1210) ==  Cyc_Absyn_TupleType: 0){ _LL1229: _temp1228=(( struct Cyc_Absyn_TupleType_struct*)
_temp1210)->f1; goto _LL1215;} else{ goto _LL1216;} _LL1216: goto _LL1217;
_LL1213: if( Cyc_Tcutil_is_const_exp( te, e2)){ Cyc_Tcutil_check_bound( loc, Cyc_Evexp_eval_const_uint_exp(
e2), _temp1220);} else{ if( Cyc_Tcutil_is_bound_one( _temp1220)){({ void*
_temp1230[ 0u]={}; Cyc_Tcutil_warn( e1->loc, _tag_arr("subscript applied to pointer to one object",
sizeof( unsigned char), 43u), _tag_arr( _temp1230, sizeof( void*), 0u));});} Cyc_Tcutil_check_nonzero_bound(
loc, _temp1220);} Cyc_Tcenv_check_rgn_accessible( te, loc, _temp1224); if( ! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind( _temp1226),( void*) Cyc_Absyn_MemKind)){({ void* _temp1231[
0u]={}; Cyc_Tcutil_terr( e1->loc, _tag_arr("can't subscript an abstract pointer",
sizeof( unsigned char), 36u), _tag_arr( _temp1231, sizeof( void*), 0u));});}
return _temp1226; _LL1215: return Cyc_Tcexp_ithTupleType( te, loc, _temp1228, e2);
_LL1217: return({ struct Cyc_Std_String_pa_struct _temp1233; _temp1233.tag= Cyc_Std_String_pa;
_temp1233.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( t1);{ void* _temp1232[
1u]={& _temp1233}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("subscript applied to %s",
sizeof( unsigned char), 24u), _tag_arr( _temp1232, sizeof( void*), 1u));}});
_LL1211:;}}} static void* Cyc_Tcexp_tcTuple( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_List_List* es){ int done= 0;
struct Cyc_List_List* fields= 0; if( topt !=  0){ void* _temp1234= Cyc_Tcutil_compress(*
topt); struct Cyc_List_List* _temp1240; _LL1236: if(( unsigned int) _temp1234 > 
3u?*(( int*) _temp1234) ==  Cyc_Absyn_TupleType: 0){ _LL1241: _temp1240=((
struct Cyc_Absyn_TupleType_struct*) _temp1234)->f1; goto _LL1237;} else{ goto
_LL1238;} _LL1238: goto _LL1239; _LL1237: if((( int(*)( struct Cyc_List_List* x))
Cyc_List_length)( _temp1240) != (( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
es)){ goto _LL1235;} for( 0; es !=  0;( es= es->tl, _temp1240= _temp1240->tl)){
void* _temp1242=(*(( struct _tuple8*)(( struct Cyc_List_List*) _check_null(
_temp1240))->hd)).f2; Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1242,(
struct Cyc_Absyn_Exp*) es->hd); fields=({ struct Cyc_List_List* _temp1243=(
struct Cyc_List_List*) _cycalloc( sizeof( struct Cyc_List_List)); _temp1243->hd=(
void*)({ struct _tuple8* _temp1244=( struct _tuple8*) _cycalloc( sizeof( struct
_tuple8)); _temp1244->f1=(*(( struct _tuple8*) _temp1240->hd)).f1; _temp1244->f2=(
void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*) es->hd)->topt))->v;
_temp1244;}); _temp1243->tl= fields; _temp1243;});} done= 1; goto _LL1235;
_LL1239: goto _LL1235; _LL1235:;} if( ! done){ for( 0; es !=  0; es= es->tl){
Cyc_Tcexp_tcExpInitializer( te, 0,( struct Cyc_Absyn_Exp*) es->hd); fields=({
struct Cyc_List_List* _temp1245=( struct Cyc_List_List*) _cycalloc( sizeof(
struct Cyc_List_List)); _temp1245->hd=( void*)({ struct _tuple8* _temp1246=(
struct _tuple8*) _cycalloc( sizeof( struct _tuple8)); _temp1246->f1= Cyc_Absyn_empty_tqual();
_temp1246->f2=( void*)(( struct Cyc_Core_Opt*) _check_null((( struct Cyc_Absyn_Exp*)
es->hd)->topt))->v; _temp1246;}); _temp1245->tl= fields; _temp1245;});}} return(
void*)({ struct Cyc_Absyn_TupleType_struct* _temp1247=( struct Cyc_Absyn_TupleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp1247[ 0]=({ struct
Cyc_Absyn_TupleType_struct _temp1248; _temp1248.tag= Cyc_Absyn_TupleType;
_temp1248.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
fields); _temp1248;}); _temp1247;});} static void* Cyc_Tcexp_tcCompoundLit(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
_tuple2* t, struct Cyc_List_List* des){ return({ void* _temp1249[ 0u]={}; Cyc_Tcexp_expr_err(
te, loc, _tag_arr("tcCompoundLit", sizeof( unsigned char), 14u), _tag_arr(
_temp1249, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcArray( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** elt_topt, struct Cyc_List_List* des){
struct Cyc_List_List* es=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Exp*(* f)(
struct _tuple7*), struct Cyc_List_List* x)) Cyc_List_map)(( struct Cyc_Absyn_Exp*(*)(
struct _tuple7*)) Cyc_Core_snd, des); void* res= Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,({ struct Cyc_Core_Opt* _temp1272=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1272->v=( void*) Cyc_Tcenv_lookup_type_vars(
te); _temp1272;})); struct Cyc_Absyn_Const_e_struct* _temp1250=({ struct Cyc_Absyn_Const_e_struct*
_temp1268=( struct Cyc_Absyn_Const_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp1268[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp1269; _temp1269.tag= Cyc_Absyn_Const_e;
_temp1269.f1=( void*)(( void*)({ struct Cyc_Absyn_Int_c_struct* _temp1270=(
struct Cyc_Absyn_Int_c_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp1270[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp1271; _temp1271.tag= Cyc_Absyn_Int_c;
_temp1271.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp1271.f2=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( es); _temp1271;}); _temp1270;}));
_temp1269;}); _temp1268;}); struct Cyc_Absyn_Exp* sz_exp= Cyc_Absyn_new_exp((
void*) _temp1250, loc); sz_exp->topt=({ struct Cyc_Core_Opt* _temp1251=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1251->v=( void*)
Cyc_Absyn_uint_t; _temp1251;});{ void* res_t2=( void*)({ struct Cyc_Absyn_ArrayType_struct*
_temp1266=( struct Cyc_Absyn_ArrayType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp1266[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp1267; _temp1267.tag= Cyc_Absyn_ArrayType;
_temp1267.f1=( void*) res; _temp1267.f2= Cyc_Absyn_empty_tqual(); _temp1267.f3=(
struct Cyc_Absyn_Exp*) sz_exp; _temp1267;}); _temp1266;});{ struct Cyc_List_List*
es2= es; for( 0; es2 !=  0; es2= es2->tl){ Cyc_Tcexp_tcExpInitializer( te,
elt_topt,( struct Cyc_Absyn_Exp*) es2->hd);}} if( ! Cyc_Tcutil_coerce_list( te,
res, es)){({ struct Cyc_Std_String_pa_struct _temp1253; _temp1253.tag= Cyc_Std_String_pa;
_temp1253.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( res);{ void*
_temp1252[ 1u]={& _temp1253}; Cyc_Tcutil_terr((( struct Cyc_Absyn_Exp*)(( struct
Cyc_List_List*) _check_null( es))->hd)->loc, _tag_arr("elements of array do not all have the same type (%s)",
sizeof( unsigned char), 53u), _tag_arr( _temp1252, sizeof( void*), 1u));}});}{
int offset= 0; for( 0; des !=  0;( offset ++, des= des->tl)){ struct Cyc_List_List*
ds=(*(( struct _tuple7*) des->hd)).f1; if( ds !=  0){ void* _temp1254=( void*)
ds->hd; struct Cyc_Absyn_Exp* _temp1260; _LL1256: if(*(( int*) _temp1254) == 
Cyc_Absyn_FieldName){ goto _LL1257;} else{ goto _LL1258;} _LL1258: if(*(( int*)
_temp1254) ==  Cyc_Absyn_ArrayElement){ _LL1261: _temp1260=(( struct Cyc_Absyn_ArrayElement_struct*)
_temp1254)->f1; goto _LL1259;} else{ goto _LL1255;} _LL1257:({ void* _temp1262[
0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("only array index designators are supported",
sizeof( unsigned char), 43u), _tag_arr( _temp1262, sizeof( void*), 0u));}); goto
_LL1255; _LL1259: Cyc_Tcexp_tcExpInitializer( te, 0, _temp1260);{ unsigned int i=
Cyc_Evexp_eval_const_uint_exp( _temp1260); if( i !=  offset){({ struct Cyc_Std_Int_pa_struct
_temp1265; _temp1265.tag= Cyc_Std_Int_pa; _temp1265.f1=( int) i;{ struct Cyc_Std_Int_pa_struct
_temp1264; _temp1264.tag= Cyc_Std_Int_pa; _temp1264.f1=( int)(( unsigned int)
offset);{ void* _temp1263[ 2u]={& _temp1264,& _temp1265}; Cyc_Tcutil_terr(
_temp1260->loc, _tag_arr("expecting index designator %d but found %d", sizeof(
unsigned char), 43u), _tag_arr( _temp1263, sizeof( void*), 2u));}}});} goto
_LL1255;} _LL1255:;}}} return res_t2;}} static void* Cyc_Tcexp_tcComprehension(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Vardecl* vd, struct Cyc_Absyn_Exp* bound, struct Cyc_Absyn_Exp* body){
Cyc_Tcexp_tcExp( te, 0, bound); if( ! Cyc_Tcutil_coerce_uint_typ( te, bound)){({
struct Cyc_Std_String_pa_struct _temp1274; _temp1274.tag= Cyc_Std_String_pa;
_temp1274.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( bound->topt))->v);{ void* _temp1273[ 1u]={& _temp1274}; Cyc_Tcutil_terr(
bound->loc, _tag_arr("expecting unsigned int, found %s", sizeof( unsigned char),
33u), _tag_arr( _temp1273, sizeof( void*), 1u));}});} if( !( vd->tq).q_const){({
void* _temp1275[ 0u]={};(( int(*)( struct _tagged_arr fmt, struct _tagged_arr ap))
Cyc_Tcutil_impos)( _tag_arr("comprehension index variable is not declared const!",
sizeof( unsigned char), 52u), _tag_arr( _temp1275, sizeof( void*), 0u));});} if(
te->le !=  0){ te= Cyc_Tcenv_new_block( loc, te); te= Cyc_Tcenv_add_local_var(
loc, te, vd);}{ struct Cyc_Absyn_PtrInfo pinfo; void** _temp1276= 0; struct Cyc_Absyn_Tqual*
_temp1277= 0; if( topt !=  0){ void* _temp1278= Cyc_Tcutil_compress(* topt);
struct Cyc_Absyn_PtrInfo _temp1286; struct Cyc_Absyn_Exp* _temp1288; struct Cyc_Absyn_Tqual
_temp1290; struct Cyc_Absyn_Tqual* _temp1292; void* _temp1293; void** _temp1295;
_LL1280: if(( unsigned int) _temp1278 >  3u?*(( int*) _temp1278) ==  Cyc_Absyn_PointerType:
0){ _LL1287: _temp1286=(( struct Cyc_Absyn_PointerType_struct*) _temp1278)->f1;
goto _LL1281;} else{ goto _LL1282;} _LL1282: if(( unsigned int) _temp1278 >  3u?*((
int*) _temp1278) ==  Cyc_Absyn_ArrayType: 0){ _LL1294: _temp1293=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1278)->f1; _temp1295=( void**)&(( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1278)->f1); goto _LL1291; _LL1291:
_temp1290=(( struct Cyc_Absyn_ArrayType_struct*) _temp1278)->f2; _temp1292=(
struct Cyc_Absyn_Tqual*)&(( struct Cyc_Absyn_ArrayType_struct*) _temp1278)->f2;
goto _LL1289; _LL1289: _temp1288=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1278)->f3; goto _LL1283;} else{ goto _LL1284;} _LL1284: goto _LL1285;
_LL1281: pinfo= _temp1286; _temp1276=( void**)(( void**)(( void*)& pinfo.elt_typ));
_temp1277=( struct Cyc_Absyn_Tqual*)& pinfo.tq; goto _LL1279; _LL1283: _temp1276=(
void**) _temp1295; _temp1277=( struct Cyc_Absyn_Tqual*) _temp1292; goto _LL1279;
_LL1285: goto _LL1279; _LL1279:;}{ void* t= Cyc_Tcexp_tcExp( te, _temp1276, body);
if( te->le ==  0){ if( ! Cyc_Tcutil_is_const_exp( te, bound)){({ void* _temp1296[
0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("bound is not constant", sizeof(
unsigned char), 22u), _tag_arr( _temp1296, sizeof( void*), 0u));});} if( ! Cyc_Tcutil_is_const_exp(
te, body)){({ void* _temp1297[ 0u]={}; Cyc_Tcutil_terr( bound->loc, _tag_arr("body is not constant",
sizeof( unsigned char), 21u), _tag_arr( _temp1297, sizeof( void*), 0u));});}}
return( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp1298=( struct Cyc_Absyn_ArrayType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp1298[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp1299; _temp1299.tag= Cyc_Absyn_ArrayType;
_temp1299.f1=( void*) t; _temp1299.f2= _temp1277 ==  0? Cyc_Absyn_empty_tqual():*
_temp1277; _temp1299.f3=( struct Cyc_Absyn_Exp*) bound; _temp1299;}); _temp1298;});}}}
struct _tuple9{ struct Cyc_Absyn_Structfield* f1; struct Cyc_Absyn_Exp* f2; } ;
static void* Cyc_Tcexp_tcStruct( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct _tuple1** tn, struct Cyc_Core_Opt** otyps, struct Cyc_List_List*
args, struct Cyc_Absyn_Structdecl** sd_opt){ struct Cyc_Absyn_Structdecl* sd;
if(* sd_opt !=  0){ sd=( struct Cyc_Absyn_Structdecl*) _check_null(* sd_opt);}
else{{ struct _handler_cons _temp1300; _push_handler(& _temp1300);{ int
_temp1302= 0; if( setjmp( _temp1300.handler)){ _temp1302= 1;} if( ! _temp1302){
sd=* Cyc_Tcenv_lookup_structdecl( te, loc,* tn);; _pop_handler();} else{ void*
_temp1301=( void*) _exn_thrown; void* _temp1304= _temp1301; _LL1306: if(
_temp1304 ==  Cyc_Dict_Absent){ goto _LL1307;} else{ goto _LL1308;} _LL1308:
goto _LL1309; _LL1307:({ struct Cyc_Std_String_pa_struct _temp1311; _temp1311.tag=
Cyc_Std_String_pa; _temp1311.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string(*
tn);{ void* _temp1310[ 1u]={& _temp1311}; Cyc_Tcutil_terr( loc, _tag_arr("unbound struct name %s",
sizeof( unsigned char), 23u), _tag_arr( _temp1310, sizeof( void*), 1u));}});
return topt !=  0?* topt:( void*) Cyc_Absyn_VoidType; _LL1309:( void) _throw(
_temp1304); _LL1305:;}}}* sd_opt=( struct Cyc_Absyn_Structdecl*) sd; if( sd->name
!=  0){* tn=( struct _tuple1*)(( struct Cyc_Core_Opt*) _check_null( sd->name))->v;}}{
struct _RegionHandle _temp1312= _new_region("rgn"); struct _RegionHandle* rgn=&
_temp1312; _push_region( rgn);{ struct _tuple4 _temp1313=({ struct _tuple4
_temp1337; _temp1337.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1337.f2= rgn;
_temp1337;}); struct Cyc_List_List* _temp1314=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1313, sd->tvs); struct Cyc_List_List* _temp1315=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1314); struct Cyc_Absyn_StructType_struct*
_temp1316=({ struct Cyc_Absyn_StructType_struct* _temp1334=( struct Cyc_Absyn_StructType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp1334[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp1335; _temp1335.tag= Cyc_Absyn_StructType;
_temp1335.f1=( struct _tuple1*)* tn; _temp1335.f2= _temp1315; _temp1335.f3=({
struct Cyc_Absyn_Structdecl** _temp1336=( struct Cyc_Absyn_Structdecl**)
_cycalloc( sizeof( struct Cyc_Absyn_Structdecl*)); _temp1336[ 0]= sd; _temp1336;});
_temp1335;}); _temp1334;}); if( topt !=  0){ Cyc_Tcutil_unify(( void*) _temp1316,*
topt);}* otyps=({ struct Cyc_Core_Opt* _temp1317=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1317->v=( void*) _temp1315;
_temp1317;}); if( sd->fields ==  0){({ void* _temp1318[ 0u]={}; Cyc_Tcutil_terr(
loc, _tag_arr("can't build abstract struct", sizeof( unsigned char), 28u),
_tag_arr( _temp1318, sizeof( void*), 0u));});{ void* _temp1319=( void*)
_temp1316; _npop_handler( 0u); return _temp1319;}}{ struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args,( struct Cyc_List_List*)(( struct Cyc_Core_Opt*) _check_null( sd->fields))->v);
for( 0; fields !=  0; fields= fields->tl){ struct _tuple9 _temp1322; struct Cyc_Absyn_Exp*
_temp1323; struct Cyc_Absyn_Structfield* _temp1325; struct _tuple9* _temp1320=(
struct _tuple9*) fields->hd; _temp1322=* _temp1320; _LL1326: _temp1325=
_temp1322.f1; goto _LL1324; _LL1324: _temp1323= _temp1322.f2; goto _LL1321;
_LL1321: { void* _temp1327= Cyc_Tcutil_rsubstitute( rgn, _temp1314,( void*)
_temp1325->type); Cyc_Tcexp_tcExpInitializer( te,( void**)& _temp1327, _temp1323);
if( ! Cyc_Tcutil_coerce_arg( te, _temp1323, _temp1327)){({ struct Cyc_Std_String_pa_struct
_temp1332; _temp1332.tag= Cyc_Std_String_pa; _temp1332.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1323->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1331; _temp1331.tag= Cyc_Std_String_pa;
_temp1331.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string( _temp1327);{ struct
Cyc_Std_String_pa_struct _temp1330; _temp1330.tag= Cyc_Std_String_pa; _temp1330.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string(* tn);{ struct Cyc_Std_String_pa_struct
_temp1329; _temp1329.tag= Cyc_Std_String_pa; _temp1329.f1=( struct _tagged_arr)*
_temp1325->name;{ void* _temp1328[ 4u]={& _temp1329,& _temp1330,& _temp1331,&
_temp1332}; Cyc_Tcutil_terr( _temp1323->loc, _tag_arr("field %s of struct %s expects type %s != %s",
sizeof( unsigned char), 44u), _tag_arr( _temp1328, sizeof( void*), 4u));}}}}});
Cyc_Tcutil_explain_failure();}}}{ void* _temp1333=( void*) _temp1316;
_npop_handler( 0u); return _temp1333;}}}; _pop_region( rgn);}} static void* Cyc_Tcexp_tcAnonStruct(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void* ts, struct
Cyc_List_List* args){{ struct _RegionHandle _temp1338= _new_region("rgn");
struct _RegionHandle* rgn=& _temp1338; _push_region( rgn);{ void* _temp1339= Cyc_Tcutil_compress(
ts); struct Cyc_List_List* _temp1345; _LL1341: if(( unsigned int) _temp1339 >  3u?*((
int*) _temp1339) ==  Cyc_Absyn_AnonStructType: 0){ _LL1346: _temp1345=(( struct
Cyc_Absyn_AnonStructType_struct*) _temp1339)->f1; goto _LL1342;} else{ goto
_LL1343;} _LL1343: goto _LL1344; _LL1342: { struct Cyc_List_List* fields=((
struct Cyc_List_List*(*)( struct _RegionHandle* rgn, struct Cyc_Position_Segment*
loc, struct Cyc_List_List* des, struct Cyc_List_List* fields)) Cyc_Tcutil_resolve_struct_designators)(
rgn, loc, args, _temp1345); for( 0; fields !=  0; fields= fields->tl){ struct
_tuple9 _temp1349; struct Cyc_Absyn_Exp* _temp1350; struct Cyc_Absyn_Structfield*
_temp1352; struct _tuple9* _temp1347=( struct _tuple9*) fields->hd; _temp1349=*
_temp1347; _LL1353: _temp1352= _temp1349.f1; goto _LL1351; _LL1351: _temp1350=
_temp1349.f2; goto _LL1348; _LL1348: Cyc_Tcexp_tcExpInitializer( te,( void**)((
void**)(( void*)& _temp1352->type)), _temp1350); if( ! Cyc_Tcutil_coerce_arg( te,
_temp1350,( void*) _temp1352->type)){({ struct Cyc_Std_String_pa_struct
_temp1357; _temp1357.tag= Cyc_Std_String_pa; _temp1357.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1350->topt))->v);{
struct Cyc_Std_String_pa_struct _temp1356; _temp1356.tag= Cyc_Std_String_pa;
_temp1356.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(( void*) _temp1352->type);{
struct Cyc_Std_String_pa_struct _temp1355; _temp1355.tag= Cyc_Std_String_pa;
_temp1355.f1=( struct _tagged_arr)* _temp1352->name;{ void* _temp1354[ 3u]={&
_temp1355,& _temp1356,& _temp1357}; Cyc_Tcutil_terr( _temp1350->loc, _tag_arr("field %s of struct expects type %s != %s",
sizeof( unsigned char), 41u), _tag_arr( _temp1354, sizeof( void*), 3u));}}}});
Cyc_Tcutil_explain_failure();}} goto _LL1340;} _LL1344:({ void* _temp1358[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("tcAnonStruct:  not an AnonStructType", sizeof( unsigned char), 37u),
_tag_arr( _temp1358, sizeof( void*), 0u));}); goto _LL1340; _LL1340:;};
_pop_region( rgn);} return ts;} static void* Cyc_Tcexp_tcTunion( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Exp* e,
struct Cyc_Core_Opt** all_ref, struct Cyc_Core_Opt** exist_ref, struct Cyc_List_List*
es, struct Cyc_Absyn_Tuniondecl* tud, struct Cyc_Absyn_Tunionfield* tuf){ struct
_RegionHandle _temp1359= _new_region("rgn"); struct _RegionHandle* rgn=&
_temp1359; _push_region( rgn);{ struct _tuple4 _temp1360=({ struct _tuple4
_temp1406; _temp1406.f1= Cyc_Tcenv_lookup_type_vars( te); _temp1406.f2= rgn;
_temp1406;}); struct Cyc_List_List* _temp1361=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1360, tud->tvs); struct Cyc_List_List* _temp1362=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1360, tuf->tvs); struct Cyc_List_List* _temp1363=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, _temp1361); struct Cyc_List_List* _temp1364=((
struct Cyc_List_List*(*)( void*(* f)( struct _tuple5*), struct Cyc_List_List* x))
Cyc_List_map)(( void*(*)( struct _tuple5*)) Cyc_Core_snd, _temp1362); struct Cyc_List_List*
_temp1365=(( struct Cyc_List_List*(*)( struct _RegionHandle*, struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_rappend)( rgn, _temp1361, _temp1362);*
all_ref=({ struct Cyc_Core_Opt* _temp1366=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1366->v=( void*) _temp1363; _temp1366;});*
exist_ref=({ struct Cyc_Core_Opt* _temp1367=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp1367->v=( void*) _temp1364; _temp1367;});{
void* res=( void*)({ struct Cyc_Absyn_TunionFieldType_struct* _temp1401=( struct
Cyc_Absyn_TunionFieldType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionFieldType_struct));
_temp1401[ 0]=({ struct Cyc_Absyn_TunionFieldType_struct _temp1402; _temp1402.tag=
Cyc_Absyn_TunionFieldType; _temp1402.f1=({ struct Cyc_Absyn_TunionFieldInfo
_temp1403; _temp1403.field_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunionfield_struct*
_temp1404=( struct Cyc_Absyn_KnownTunionfield_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_KnownTunionfield_struct)); _temp1404[ 0]=({ struct Cyc_Absyn_KnownTunionfield_struct
_temp1405; _temp1405.tag= Cyc_Absyn_KnownTunionfield; _temp1405.f1= tud;
_temp1405.f2= tuf; _temp1405;}); _temp1404;})); _temp1403.targs= _temp1363;
_temp1403;}); _temp1402;}); _temp1401;}); if( topt !=  0){ void* _temp1368= Cyc_Tcutil_compress(*
topt); struct Cyc_Absyn_TunionInfo _temp1376; void* _temp1378; struct Cyc_List_List*
_temp1380; _LL1370: if(( unsigned int) _temp1368 >  3u?*(( int*) _temp1368) == 
Cyc_Absyn_TunionFieldType: 0){ goto _LL1371;} else{ goto _LL1372;} _LL1372: if((
unsigned int) _temp1368 >  3u?*(( int*) _temp1368) ==  Cyc_Absyn_TunionType: 0){
_LL1377: _temp1376=(( struct Cyc_Absyn_TunionType_struct*) _temp1368)->f1;
_LL1381: _temp1380= _temp1376.targs; goto _LL1379; _LL1379: _temp1378=( void*)
_temp1376.rgn; goto _LL1373;} else{ goto _LL1374;} _LL1374: goto _LL1375;
_LL1371: Cyc_Tcutil_unify(* topt, res); goto _LL1369; _LL1373:{ struct Cyc_List_List*
a= _temp1363; for( 0; a !=  0? _temp1380 !=  0: 0;( a= a->tl, _temp1380=
_temp1380->tl)){ Cyc_Tcutil_unify(( void*) a->hd,( void*) _temp1380->hd);}} if(
tuf->typs ==  0? es ==  0: 0){ e->topt=({ struct Cyc_Core_Opt* _temp1382=(
struct Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1382->v=(
void*) res; _temp1382;}); res=( void*)({ struct Cyc_Absyn_TunionType_struct*
_temp1383=( struct Cyc_Absyn_TunionType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_TunionType_struct));
_temp1383[ 0]=({ struct Cyc_Absyn_TunionType_struct _temp1384; _temp1384.tag=
Cyc_Absyn_TunionType; _temp1384.f1=({ struct Cyc_Absyn_TunionInfo _temp1385;
_temp1385.tunion_info=( void*)(( void*)({ struct Cyc_Absyn_KnownTunion_struct*
_temp1386=( struct Cyc_Absyn_KnownTunion_struct*) _cycalloc( sizeof( struct Cyc_Absyn_KnownTunion_struct));
_temp1386[ 0]=({ struct Cyc_Absyn_KnownTunion_struct _temp1387; _temp1387.tag=
Cyc_Absyn_KnownTunion; _temp1387.f1=({ struct Cyc_Absyn_Tuniondecl** _temp1388=(
struct Cyc_Absyn_Tuniondecl**) _cycalloc( sizeof( struct Cyc_Absyn_Tuniondecl*));
_temp1388[ 0]= tud; _temp1388;}); _temp1387;}); _temp1386;})); _temp1385.targs=
_temp1363; _temp1385.rgn=( void*) _temp1378; _temp1385;}); _temp1384;});
_temp1383;}); Cyc_Tcutil_unchecked_cast( te, e, res);{ void* _temp1389= res;
_npop_handler( 0u); return _temp1389;}} goto _LL1369; _LL1375: goto _LL1369;
_LL1369:;}{ struct Cyc_List_List* ts= tuf->typs; for( 0; es !=  0? ts !=  0: 0;(
es= es->tl, ts= ts->tl)){ struct Cyc_Absyn_Exp* e=( struct Cyc_Absyn_Exp*) es->hd;
void* t= Cyc_Tcutil_rsubstitute( rgn, _temp1365,(*(( struct _tuple8*) ts->hd)).f2);
Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( ! Cyc_Tcutil_coerce_arg( te,
e, t)){({ struct Cyc_Std_String_pa_struct _temp1393; _temp1393.tag= Cyc_Std_String_pa;
_temp1393.f1=( struct _tagged_arr)( e->topt ==  0? _tag_arr("?", sizeof(
unsigned char), 2u): Cyc_Absynpp_typ2string(( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v));{ struct Cyc_Std_String_pa_struct _temp1392;
_temp1392.tag= Cyc_Std_String_pa; _temp1392.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_Std_String_pa_struct _temp1391; _temp1391.tag= Cyc_Std_String_pa;
_temp1391.f1=( struct _tagged_arr) Cyc_Absynpp_qvar2string( tuf->name);{ void*
_temp1390[ 3u]={& _temp1391,& _temp1392,& _temp1393}; Cyc_Tcutil_terr( e->loc,
_tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof( unsigned char), 80u), _tag_arr( _temp1390, sizeof( void*), 3u));}}}});
Cyc_Tcutil_explain_failure();}} if( es !=  0){ void* _temp1396=({ struct Cyc_Std_String_pa_struct
_temp1395; _temp1395.tag= Cyc_Std_String_pa; _temp1395.f1=( struct _tagged_arr)
Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp1394[ 1u]={& _temp1395}; Cyc_Tcexp_expr_err(
te,(( struct Cyc_Absyn_Exp*) es->hd)->loc, _tag_arr("too many arguments for tunion constructor %s",
sizeof( unsigned char), 45u), _tag_arr( _temp1394, sizeof( void*), 1u));}});
_npop_handler( 0u); return _temp1396;} if( ts !=  0){ void* _temp1399=({ struct
Cyc_Std_String_pa_struct _temp1398; _temp1398.tag= Cyc_Std_String_pa; _temp1398.f1=(
struct _tagged_arr) Cyc_Absynpp_qvar2string( tuf->name);{ void* _temp1397[ 1u]={&
_temp1398}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("too few arguments for tunion constructor %s",
sizeof( unsigned char), 44u), _tag_arr( _temp1397, sizeof( void*), 1u));}});
_npop_handler( 0u); return _temp1399;}{ void* _temp1400= res; _npop_handler( 0u);
return _temp1400;}}}}; _pop_region( rgn);} static int Cyc_Tcexp_zeroable_type(
void* t){ void* _temp1407= Cyc_Tcutil_compress( t); struct Cyc_Absyn_PtrInfo
_temp1459; struct Cyc_Absyn_Conref* _temp1461; void* _temp1463; struct Cyc_List_List*
_temp1465; struct Cyc_Absyn_Structdecl** _temp1467; struct Cyc_List_List*
_temp1469; struct Cyc_List_List* _temp1471; _LL1409: if( _temp1407 == ( void*)
Cyc_Absyn_VoidType){ goto _LL1410;} else{ goto _LL1411;} _LL1411: if((
unsigned int) _temp1407 >  3u?*(( int*) _temp1407) ==  Cyc_Absyn_Evar: 0){ goto
_LL1412;} else{ goto _LL1413;} _LL1413: if(( unsigned int) _temp1407 >  3u?*((
int*) _temp1407) ==  Cyc_Absyn_VarType: 0){ goto _LL1414;} else{ goto _LL1415;}
_LL1415: if(( unsigned int) _temp1407 >  3u?*(( int*) _temp1407) ==  Cyc_Absyn_TunionType:
0){ goto _LL1416;} else{ goto _LL1417;} _LL1417: if(( unsigned int) _temp1407 > 
3u?*(( int*) _temp1407) ==  Cyc_Absyn_TunionFieldType: 0){ goto _LL1418;} else{
goto _LL1419;} _LL1419: if(( unsigned int) _temp1407 >  3u?*(( int*) _temp1407)
==  Cyc_Absyn_PointerType: 0){ _LL1460: _temp1459=(( struct Cyc_Absyn_PointerType_struct*)
_temp1407)->f1; _LL1462: _temp1461= _temp1459.nullable; goto _LL1420;} else{
goto _LL1421;} _LL1421: if(( unsigned int) _temp1407 >  3u?*(( int*) _temp1407)
==  Cyc_Absyn_IntType: 0){ goto _LL1422;} else{ goto _LL1423;} _LL1423: if(
_temp1407 == ( void*) Cyc_Absyn_FloatType){ goto _LL1424;} else{ goto _LL1425;}
_LL1425: if(( unsigned int) _temp1407 >  3u?*(( int*) _temp1407) ==  Cyc_Absyn_DoubleType:
0){ goto _LL1426;} else{ goto _LL1427;} _LL1427: if(( unsigned int) _temp1407 > 
3u?*(( int*) _temp1407) ==  Cyc_Absyn_ArrayType: 0){ _LL1464: _temp1463=( void*)((
struct Cyc_Absyn_ArrayType_struct*) _temp1407)->f1; goto _LL1428;} else{ goto
_LL1429;} _LL1429: if(( unsigned int) _temp1407 >  3u?*(( int*) _temp1407) == 
Cyc_Absyn_FnType: 0){ goto _LL1430;} else{ goto _LL1431;} _LL1431: if((
unsigned int) _temp1407 >  3u?*(( int*) _temp1407) ==  Cyc_Absyn_TupleType: 0){
_LL1466: _temp1465=(( struct Cyc_Absyn_TupleType_struct*) _temp1407)->f1; goto
_LL1432;} else{ goto _LL1433;} _LL1433: if(( unsigned int) _temp1407 >  3u?*((
int*) _temp1407) ==  Cyc_Absyn_StructType: 0){ _LL1470: _temp1469=(( struct Cyc_Absyn_StructType_struct*)
_temp1407)->f2; goto _LL1468; _LL1468: _temp1467=(( struct Cyc_Absyn_StructType_struct*)
_temp1407)->f3; goto _LL1434;} else{ goto _LL1435;} _LL1435: if(( unsigned int)
_temp1407 >  3u?*(( int*) _temp1407) ==  Cyc_Absyn_UnionType: 0){ goto _LL1436;}
else{ goto _LL1437;} _LL1437: if(( unsigned int) _temp1407 >  3u?*(( int*)
_temp1407) ==  Cyc_Absyn_EnumType: 0){ goto _LL1438;} else{ goto _LL1439;}
_LL1439: if(( unsigned int) _temp1407 >  3u?*(( int*) _temp1407) ==  Cyc_Absyn_SizeofType:
0){ goto _LL1440;} else{ goto _LL1441;} _LL1441: if(( unsigned int) _temp1407 > 
3u?*(( int*) _temp1407) ==  Cyc_Absyn_AnonStructType: 0){ _LL1472: _temp1471=((
struct Cyc_Absyn_AnonStructType_struct*) _temp1407)->f1; goto _LL1442;} else{
goto _LL1443;} _LL1443: if(( unsigned int) _temp1407 >  3u?*(( int*) _temp1407)
==  Cyc_Absyn_AnonUnionType: 0){ goto _LL1444;} else{ goto _LL1445;} _LL1445:
if(( unsigned int) _temp1407 >  3u?*(( int*) _temp1407) ==  Cyc_Absyn_AnonEnumType:
0){ goto _LL1446;} else{ goto _LL1447;} _LL1447: if(( unsigned int) _temp1407 > 
3u?*(( int*) _temp1407) ==  Cyc_Absyn_RgnHandleType: 0){ goto _LL1448;} else{
goto _LL1449;} _LL1449: if(( unsigned int) _temp1407 >  3u?*(( int*) _temp1407)
==  Cyc_Absyn_TypedefType: 0){ goto _LL1450;} else{ goto _LL1451;} _LL1451: if(
_temp1407 == ( void*) Cyc_Absyn_HeapRgn){ goto _LL1452;} else{ goto _LL1453;}
_LL1453: if(( unsigned int) _temp1407 >  3u?*(( int*) _temp1407) ==  Cyc_Absyn_AccessEff:
0){ goto _LL1454;} else{ goto _LL1455;} _LL1455: if(( unsigned int) _temp1407 > 
3u?*(( int*) _temp1407) ==  Cyc_Absyn_JoinEff: 0){ goto _LL1456;} else{ goto
_LL1457;} _LL1457: if(( unsigned int) _temp1407 >  3u?*(( int*) _temp1407) == 
Cyc_Absyn_RgnsEff: 0){ goto _LL1458;} else{ goto _LL1408;} _LL1410: return 1;
_LL1412: goto _LL1414; _LL1414: goto _LL1416; _LL1416: goto _LL1418; _LL1418:
return 0; _LL1420: return(( int(*)( int, struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_def)(
1, _temp1461); _LL1422: goto _LL1424; _LL1424: goto _LL1426; _LL1426: return 1;
_LL1428: return Cyc_Tcexp_zeroable_type( _temp1463); _LL1430: return 0; _LL1432:
for( 0;( unsigned int) _temp1465; _temp1465= _temp1465->tl){ if( ! Cyc_Tcexp_zeroable_type((*((
struct _tuple8*) _temp1465->hd)).f2)){ return 0;}} return 1; _LL1434: if(
_temp1467 ==  0){ return 0;}{ struct Cyc_Absyn_Structdecl* _temp1473=* _temp1467;
if( _temp1473->fields ==  0){ return 0;}{ struct _RegionHandle _temp1474=
_new_region("r"); struct _RegionHandle* r=& _temp1474; _push_region( r);{ struct
Cyc_List_List* _temp1475=(( struct Cyc_List_List*(*)( struct _RegionHandle* r1,
struct _RegionHandle* r2, struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_rzip)(
r, r, _temp1473->tvs, _temp1469);{ struct Cyc_List_List* fs=( struct Cyc_List_List*)((
struct Cyc_Core_Opt*) _check_null( _temp1473->fields))->v; for( 0; fs !=  0; fs=
fs->tl){ if( ! Cyc_Tcexp_zeroable_type( Cyc_Tcutil_rsubstitute( r, _temp1475,(
void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type))){ int _temp1476= 0;
_npop_handler( 0u); return _temp1476;}}}{ int _temp1477= 1; _npop_handler( 0u);
return _temp1477;}}; _pop_region( r);}} _LL1436: return 1; _LL1438: return 1;
_LL1440: return 1; _LL1442: for( 0; _temp1471 !=  0; _temp1471= _temp1471->tl){
if( ! Cyc_Tcexp_zeroable_type(( void*)(( struct Cyc_Absyn_Structfield*)
_temp1471->hd)->type)){ return 0;}} return 1; _LL1444: return 1; _LL1446: return
1; _LL1448: return 0; _LL1450: goto _LL1452; _LL1452: goto _LL1454; _LL1454:
goto _LL1456; _LL1456: goto _LL1458; _LL1458: return({ struct Cyc_Std_String_pa_struct
_temp1479; _temp1479.tag= Cyc_Std_String_pa; _temp1479.f1=( struct _tagged_arr)
Cyc_Absynpp_typ2string( t);{ void* _temp1478[ 1u]={& _temp1479};(( int(*)(
struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)( _tag_arr("bad type `%s' in zeroable type",
sizeof( unsigned char), 31u), _tag_arr( _temp1478, sizeof( void*), 1u));}});
_LL1408:;} static void Cyc_Tcexp_check_malloc_type( int allow_zero, struct Cyc_Position_Segment*
loc, void** topt, void* t){ if( Cyc_Tcutil_bits_only( t)? 1:( allow_zero? Cyc_Tcexp_zeroable_type(
t): 0)){ return;} if( topt !=  0){ void* _temp1480= Cyc_Tcutil_compress(* topt);
struct Cyc_Absyn_PtrInfo _temp1486; void* _temp1488; _LL1482: if(( unsigned int)
_temp1480 >  3u?*(( int*) _temp1480) ==  Cyc_Absyn_PointerType: 0){ _LL1487:
_temp1486=(( struct Cyc_Absyn_PointerType_struct*) _temp1480)->f1; _LL1489:
_temp1488=( void*) _temp1486.elt_typ; goto _LL1483;} else{ goto _LL1484;}
_LL1484: goto _LL1485; _LL1483: Cyc_Tcutil_unify( _temp1488, t); if( Cyc_Tcutil_bits_only(
t)? 1:( allow_zero? Cyc_Tcexp_zeroable_type( t): 0)){ return;} goto _LL1481;
_LL1485: goto _LL1481; _LL1481:;}({ struct Cyc_Std_String_pa_struct _temp1492;
_temp1492.tag= Cyc_Std_String_pa; _temp1492.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
t);{ struct Cyc_Std_String_pa_struct _temp1491; _temp1491.tag= Cyc_Std_String_pa;
_temp1491.f1=( struct _tagged_arr)( allow_zero? _tag_arr("calloc", sizeof(
unsigned char), 7u): _tag_arr("malloc", sizeof( unsigned char), 7u));{ void*
_temp1490[ 2u]={& _temp1491,& _temp1492}; Cyc_Tcutil_terr( loc, _tag_arr("%s cannot be used with type %s\n\t(type needs initialization)",
sizeof( unsigned char), 60u), _tag_arr( _temp1490, sizeof( void*), 2u));}}});}
static void* Cyc_Tcexp_tcMalloc( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* ropt, void*** t, struct Cyc_Absyn_Exp**
e, int is_calloc, int* is_fat){ void* rgn=( void*) Cyc_Absyn_HeapRgn; if( ropt
!=  0){ void* expected_type=( void*)({ struct Cyc_Absyn_RgnHandleType_struct*
_temp1503=( struct Cyc_Absyn_RgnHandleType_struct*) _cycalloc( sizeof( struct
Cyc_Absyn_RgnHandleType_struct)); _temp1503[ 0]=({ struct Cyc_Absyn_RgnHandleType_struct
_temp1504; _temp1504.tag= Cyc_Absyn_RgnHandleType; _temp1504.f1=( void*) Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({ struct Cyc_Core_Opt* _temp1505=( struct
Cyc_Core_Opt*) _cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1505->v=( void*)
Cyc_Tcenv_lookup_type_vars( te); _temp1505;})); _temp1504;}); _temp1503;}); void*
handle_type= Cyc_Tcexp_tcExp( te,( void**)& expected_type,( struct Cyc_Absyn_Exp*)
_check_null( ropt)); void* _temp1493= Cyc_Tcutil_compress( handle_type); void*
_temp1499; _LL1495: if(( unsigned int) _temp1493 >  3u?*(( int*) _temp1493) == 
Cyc_Absyn_RgnHandleType: 0){ _LL1500: _temp1499=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1493)->f1; goto _LL1496;} else{ goto _LL1497;} _LL1497: goto _LL1498;
_LL1496: rgn= _temp1499; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1494; _LL1498:({ struct Cyc_Std_String_pa_struct _temp1502; _temp1502.tag=
Cyc_Std_String_pa; _temp1502.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1501[ 1u]={& _temp1502}; Cyc_Tcutil_terr( ropt->loc,
_tag_arr("expecting region_t type but found %s", sizeof( unsigned char), 37u),
_tag_arr( _temp1501, sizeof( void*), 1u));}}); goto _LL1494; _LL1494:;} Cyc_Tcexp_tcExp(
te,( void**)& Cyc_Absyn_uint_t,* e);{ void* elt_type; struct Cyc_Absyn_Exp*
num_elts; int one_elt; if( is_calloc){ if(* t ==  0){({ void* _temp1506[ 0u]={};((
int(*)( struct _tagged_arr fmt, struct _tagged_arr ap)) Cyc_Tcutil_impos)(
_tag_arr("calloc with empty type", sizeof( unsigned char), 23u), _tag_arr(
_temp1506, sizeof( void*), 0u));});} elt_type=*(( void**) _check_null(* t)); Cyc_Tcutil_check_type(
loc, te, Cyc_Tcenv_lookup_type_vars( te),( void*) Cyc_Absyn_MemKind, 1, elt_type);
Cyc_Tcexp_check_malloc_type( 1, loc, topt, elt_type); num_elts=* e; one_elt= 0;}
else{ void* _temp1507=( void*)(* e)->r; void* _temp1515; struct Cyc_List_List*
_temp1517; struct Cyc_List_List _temp1519; struct Cyc_List_List* _temp1520;
struct Cyc_List_List _temp1522; struct Cyc_List_List* _temp1523; struct Cyc_Absyn_Exp*
_temp1525; struct Cyc_Absyn_Exp* _temp1527; void* _temp1529; _LL1509: if(*(( int*)
_temp1507) ==  Cyc_Absyn_Sizeoftyp_e){ _LL1516: _temp1515=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1507)->f1; goto _LL1510;} else{ goto _LL1511;} _LL1511: if(*(( int*)
_temp1507) ==  Cyc_Absyn_Primop_e){ _LL1530: _temp1529=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1507)->f1; if( _temp1529 == ( void*) Cyc_Absyn_Times){ goto _LL1518;} else{
goto _LL1513;} _LL1518: _temp1517=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1507)->f2; if( _temp1517 ==  0){ goto _LL1513;} else{ _temp1519=* _temp1517;
_LL1528: _temp1527=( struct Cyc_Absyn_Exp*) _temp1519.hd; goto _LL1521; _LL1521:
_temp1520= _temp1519.tl; if( _temp1520 ==  0){ goto _LL1513;} else{ _temp1522=*
_temp1520; _LL1526: _temp1525=( struct Cyc_Absyn_Exp*) _temp1522.hd; goto
_LL1524; _LL1524: _temp1523= _temp1522.tl; if( _temp1523 ==  0){ goto _LL1512;}
else{ goto _LL1513;}}}} else{ goto _LL1513;} _LL1513: goto _LL1514; _LL1510:
elt_type= _temp1515;* t=( void**)({ void** _temp1531=( void**) _cycalloc(
sizeof( void*)); _temp1531[ 0]= elt_type; _temp1531;}); num_elts= Cyc_Absyn_uint_exp(
1, 0); Cyc_Tcexp_tcExp( te,( void**)& Cyc_Absyn_uint_t, num_elts); one_elt= 1;
goto _LL1508; _LL1512:{ struct _tuple0 _temp1533=({ struct _tuple0 _temp1532;
_temp1532.f1=( void*) _temp1527->r; _temp1532.f2=( void*) _temp1525->r;
_temp1532;}); void* _temp1541; void* _temp1543; void* _temp1545; void* _temp1547;
_LL1535: _LL1542: _temp1541= _temp1533.f1; if(*(( int*) _temp1541) ==  Cyc_Absyn_Sizeoftyp_e){
_LL1544: _temp1543=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1541)->f1;
goto _LL1536;} else{ goto _LL1537;} _LL1537: _LL1546: _temp1545= _temp1533.f2;
if(*(( int*) _temp1545) ==  Cyc_Absyn_Sizeoftyp_e){ _LL1548: _temp1547=( void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*) _temp1545)->f1; goto _LL1538;} else{ goto
_LL1539;} _LL1539: goto _LL1540; _LL1536: Cyc_Tcexp_check_malloc_type( 0, loc,
topt, _temp1543); elt_type= _temp1543;* t=( void**)({ void** _temp1549=( void**)
_cycalloc( sizeof( void*)); _temp1549[ 0]= elt_type; _temp1549;}); num_elts=
_temp1525; one_elt= 0; goto _LL1534; _LL1538: Cyc_Tcexp_check_malloc_type( 0,
loc, topt, _temp1547); elt_type= _temp1547;* t=( void**)({ void** _temp1550=(
void**) _cycalloc( sizeof( void*)); _temp1550[ 0]= elt_type; _temp1550;});
num_elts= _temp1527; one_elt= 0; goto _LL1534; _LL1540: goto No_sizeof; _LL1534:;}
goto _LL1508; _LL1514: No_sizeof: elt_type= Cyc_Absyn_uchar_t;* t=( void**)({
void** _temp1551=( void**) _cycalloc( sizeof( void*)); _temp1551[ 0]= elt_type;
_temp1551;}); num_elts=* e; one_elt= 0; goto _LL1508; _LL1508:;}* e= num_elts;*
is_fat= ! one_elt;{ void* _temp1552= elt_type; struct Cyc_Absyn_TunionFieldInfo
_temp1558; void* _temp1560; struct Cyc_Absyn_Tunionfield* _temp1562; _LL1554:
if(( unsigned int) _temp1552 >  3u?*(( int*) _temp1552) ==  Cyc_Absyn_TunionFieldType:
0){ _LL1559: _temp1558=(( struct Cyc_Absyn_TunionFieldType_struct*) _temp1552)->f1;
_LL1561: _temp1560=( void*) _temp1558.field_info; if(*(( int*) _temp1560) == 
Cyc_Absyn_KnownTunionfield){ _LL1563: _temp1562=(( struct Cyc_Absyn_KnownTunionfield_struct*)
_temp1560)->f2; goto _LL1555;} else{ goto _LL1556;}} else{ goto _LL1556;}
_LL1556: goto _LL1557; _LL1555: if( _temp1562->tvs !=  0){({ void* _temp1564[ 0u]={};
Cyc_Tcutil_terr( loc, _tag_arr("malloc with existential types not yet implemented",
sizeof( unsigned char), 50u), _tag_arr( _temp1564, sizeof( void*), 0u));});}
goto _LL1553; _LL1557: goto _LL1553; _LL1553:;}{ void*(* _temp1565)( void* t,
void* rgn, struct Cyc_Absyn_Tqual tq)= Cyc_Absyn_at_typ; if( topt !=  0){ void*
_temp1566= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp1572;
struct Cyc_Absyn_Conref* _temp1574; _LL1568: if(( unsigned int) _temp1566 >  3u?*((
int*) _temp1566) ==  Cyc_Absyn_PointerType: 0){ _LL1573: _temp1572=(( struct Cyc_Absyn_PointerType_struct*)
_temp1566)->f1; _LL1575: _temp1574= _temp1572.nullable; goto _LL1569;} else{
goto _LL1570;} _LL1570: goto _LL1571; _LL1569: if((( int(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_conref_val)( _temp1574)){ _temp1565= Cyc_Absyn_star_typ;} goto
_LL1567; _LL1571: goto _LL1567; _LL1567:;} if( ! one_elt){ _temp1565= Cyc_Absyn_tagged_typ;}
return _temp1565( elt_type, rgn, Cyc_Absyn_empty_tqual());}}} static void* Cyc_Tcexp_tcStmtExp(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Stmt* s){ Cyc_Tcstmt_tcStmt( Cyc_Tcenv_set_encloser( te, s), s, 1);
Cyc_NewControlFlow_set_encloser( s, Cyc_Tcenv_get_encloser( te)); while( 1) {
void* _temp1576=( void*) s->r; struct Cyc_Absyn_Exp* _temp1586; struct Cyc_Absyn_Stmt*
_temp1588; struct Cyc_Absyn_Stmt* _temp1590; struct Cyc_Absyn_Stmt* _temp1592;
struct Cyc_Absyn_Decl* _temp1594; _LL1578: if(( unsigned int) _temp1576 >  1u?*((
int*) _temp1576) ==  Cyc_Absyn_Exp_s: 0){ _LL1587: _temp1586=(( struct Cyc_Absyn_Exp_s_struct*)
_temp1576)->f1; goto _LL1579;} else{ goto _LL1580;} _LL1580: if(( unsigned int)
_temp1576 >  1u?*(( int*) _temp1576) ==  Cyc_Absyn_Seq_s: 0){ _LL1591: _temp1590=((
struct Cyc_Absyn_Seq_s_struct*) _temp1576)->f1; goto _LL1589; _LL1589: _temp1588=((
struct Cyc_Absyn_Seq_s_struct*) _temp1576)->f2; goto _LL1581;} else{ goto
_LL1582;} _LL1582: if(( unsigned int) _temp1576 >  1u?*(( int*) _temp1576) == 
Cyc_Absyn_Decl_s: 0){ _LL1595: _temp1594=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1576)->f1; goto _LL1593; _LL1593: _temp1592=(( struct Cyc_Absyn_Decl_s_struct*)
_temp1576)->f2; goto _LL1583;} else{ goto _LL1584;} _LL1584: goto _LL1585;
_LL1579: return( void*)(( struct Cyc_Core_Opt*) _check_null( _temp1586->topt))->v;
_LL1581: s= _temp1588; continue; _LL1583: s= _temp1592; continue; _LL1585:
return({ void* _temp1596[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("statement expression must end with expression",
sizeof( unsigned char), 46u), _tag_arr( _temp1596, sizeof( void*), 0u));});
_LL1577:;}} static void* Cyc_Tcexp_tcCodegen( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Position_Segment* loc, void** topt, struct Cyc_Absyn_Fndecl* fd){ return({
void* _temp1597[ 0u]={}; Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcCodegen",
sizeof( unsigned char), 10u), _tag_arr( _temp1597, sizeof( void*), 0u));});}
static void* Cyc_Tcexp_tcFill( struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment*
loc, void** topt, struct Cyc_Absyn_Exp* e){ return({ void* _temp1598[ 0u]={};
Cyc_Tcexp_expr_err( te, loc, _tag_arr("tcFill", sizeof( unsigned char), 7u),
_tag_arr( _temp1598, sizeof( void*), 0u));});} static void* Cyc_Tcexp_tcNew(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Position_Segment* loc, void** topt, struct
Cyc_Absyn_Exp* rgn_handle, struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Exp* e1){
void* rgn=( void*) Cyc_Absyn_HeapRgn; if( rgn_handle !=  0){ void* expected_type=(
void*)({ struct Cyc_Absyn_RgnHandleType_struct* _temp1609=( struct Cyc_Absyn_RgnHandleType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_RgnHandleType_struct)); _temp1609[ 0]=({
struct Cyc_Absyn_RgnHandleType_struct _temp1610; _temp1610.tag= Cyc_Absyn_RgnHandleType;
_temp1610.f1=( void*) Cyc_Absyn_new_evar(( struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt* _temp1611=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1611->v=( void*) Cyc_Tcenv_lookup_type_vars( te); _temp1611;}));
_temp1610;}); _temp1609;}); void* handle_type= Cyc_Tcexp_tcExp( te,( void**)&
expected_type,( struct Cyc_Absyn_Exp*) _check_null( rgn_handle)); void*
_temp1599= Cyc_Tcutil_compress( handle_type); void* _temp1605; _LL1601: if((
unsigned int) _temp1599 >  3u?*(( int*) _temp1599) ==  Cyc_Absyn_RgnHandleType:
0){ _LL1606: _temp1605=( void*)(( struct Cyc_Absyn_RgnHandleType_struct*)
_temp1599)->f1; goto _LL1602;} else{ goto _LL1603;} _LL1603: goto _LL1604;
_LL1602: rgn= _temp1605; Cyc_Tcenv_check_rgn_accessible( te, loc, rgn); goto
_LL1600; _LL1604:({ struct Cyc_Std_String_pa_struct _temp1608; _temp1608.tag=
Cyc_Std_String_pa; _temp1608.f1=( struct _tagged_arr) Cyc_Absynpp_typ2string(
handle_type);{ void* _temp1607[ 1u]={& _temp1608}; Cyc_Tcutil_terr( rgn_handle->loc,
_tag_arr("expecting region_t type but found %s", sizeof( unsigned char), 37u),
_tag_arr( _temp1607, sizeof( void*), 1u));}}); goto _LL1600; _LL1600:;}{ void*
_temp1612=( void*) e1->r; struct Cyc_Absyn_Exp* _temp1624; struct Cyc_Absyn_Exp*
_temp1626; struct Cyc_Absyn_Vardecl* _temp1628; struct Cyc_List_List* _temp1630;
struct Cyc_Core_Opt* _temp1632; struct Cyc_List_List* _temp1634; void* _temp1636;
struct _tagged_arr _temp1638; _LL1614: if(*(( int*) _temp1612) ==  Cyc_Absyn_Comprehension_e){
_LL1629: _temp1628=(( struct Cyc_Absyn_Comprehension_e_struct*) _temp1612)->f1;
goto _LL1627; _LL1627: _temp1626=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1612)->f2; goto _LL1625; _LL1625: _temp1624=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1612)->f3; goto _LL1615;} else{ goto _LL1616;} _LL1616: if(*(( int*)
_temp1612) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1633: _temp1632=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1612)->f1; goto _LL1631; _LL1631: _temp1630=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1612)->f2; goto _LL1617;} else{ goto _LL1618;} _LL1618: if(*(( int*)
_temp1612) ==  Cyc_Absyn_Array_e){ _LL1635: _temp1634=(( struct Cyc_Absyn_Array_e_struct*)
_temp1612)->f1; goto _LL1619;} else{ goto _LL1620;} _LL1620: if(*(( int*)
_temp1612) ==  Cyc_Absyn_Const_e){ _LL1637: _temp1636=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1612)->f1; if(( unsigned int) _temp1636 >  1u?*(( int*) _temp1636) ==  Cyc_Absyn_String_c:
0){ _LL1639: _temp1638=(( struct Cyc_Absyn_String_c_struct*) _temp1636)->f1;
goto _LL1621;} else{ goto _LL1622;}} else{ goto _LL1622;} _LL1622: goto _LL1623;
_LL1615: { void* _temp1640= Cyc_Tcexp_tcExpNoPromote( te, topt, e1); void*
_temp1641= Cyc_Tcutil_compress( _temp1640); struct Cyc_Absyn_Tqual _temp1647;
void* _temp1649; _LL1643: if(( unsigned int) _temp1641 >  3u?*(( int*) _temp1641)
==  Cyc_Absyn_ArrayType: 0){ _LL1650: _temp1649=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1641)->f1; goto _LL1648; _LL1648: _temp1647=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1641)->f2; goto _LL1644;} else{ goto _LL1645;} _LL1645: goto _LL1646;
_LL1644: { void* b= Cyc_Tcutil_is_const_exp( te, _temp1626)?( void*)({ struct
Cyc_Absyn_Upper_b_struct* _temp1655=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1655[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1656; _temp1656.tag= Cyc_Absyn_Upper_b; _temp1656.f1=
_temp1626; _temp1656;}); _temp1655;}):( void*) Cyc_Absyn_Unknown_b; void*
res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1652=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1652[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1653; _temp1653.tag= Cyc_Absyn_PointerType;
_temp1653.f1=({ struct Cyc_Absyn_PtrInfo _temp1654; _temp1654.elt_typ=( void*)
_temp1649; _temp1654.rgn_typ=( void*) rgn; _temp1654.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1654.tq= _temp1647; _temp1654.bounds= Cyc_Absyn_new_conref(
b); _temp1654;}); _temp1653;}); _temp1652;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(*
topt, res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,* topt): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1651=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1651->v=( void*) res_typ; _temp1651;}); Cyc_Tcutil_unchecked_cast(
te, e,* topt); res_typ=* topt;}} return res_typ;} _LL1646: return({ void*
_temp1657[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcNew: comprehension returned non-array type",
sizeof( unsigned char), 45u), _tag_arr( _temp1657, sizeof( void*), 0u));});
_LL1642:;} _LL1617:( void*)( e1->r=( void*)(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp1658=( struct Cyc_Absyn_Array_e_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp1658[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp1659; _temp1659.tag= Cyc_Absyn_Array_e;
_temp1659.f1= _temp1630; _temp1659;}); _temp1658;}))); _temp1634= _temp1630;
goto _LL1619; _LL1619: { void** elt_typ_opt= 0; if( topt !=  0){ void* _temp1660=
Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo _temp1666; struct Cyc_Absyn_Tqual
_temp1668; void* _temp1670; void** _temp1672; _LL1662: if(( unsigned int)
_temp1660 >  3u?*(( int*) _temp1660) ==  Cyc_Absyn_PointerType: 0){ _LL1667:
_temp1666=(( struct Cyc_Absyn_PointerType_struct*) _temp1660)->f1; _LL1671:
_temp1670=( void*) _temp1666.elt_typ; _temp1672=( void**)&((( struct Cyc_Absyn_PointerType_struct*)
_temp1660)->f1).elt_typ; goto _LL1669; _LL1669: _temp1668= _temp1666.tq; goto
_LL1663;} else{ goto _LL1664;} _LL1664: goto _LL1665; _LL1663: elt_typ_opt=(
void**) _temp1672; goto _LL1661; _LL1665: goto _LL1661; _LL1661:;}{ void*
_temp1673= Cyc_Tcexp_tcExpNoPromote( te, elt_typ_opt, e1); void* res_typ;{ void*
_temp1674= Cyc_Tcutil_compress( _temp1673); struct Cyc_Absyn_Exp* _temp1680;
struct Cyc_Absyn_Tqual _temp1682; void* _temp1684; _LL1676: if(( unsigned int)
_temp1674 >  3u?*(( int*) _temp1674) ==  Cyc_Absyn_ArrayType: 0){ _LL1685:
_temp1684=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1674)->f1; goto
_LL1683; _LL1683: _temp1682=(( struct Cyc_Absyn_ArrayType_struct*) _temp1674)->f2;
goto _LL1681; _LL1681: _temp1680=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1674)->f3; goto _LL1677;} else{ goto _LL1678;} _LL1678: goto _LL1679;
_LL1677: res_typ=( void*)({ struct Cyc_Absyn_PointerType_struct* _temp1686=(
struct Cyc_Absyn_PointerType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp1686[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp1687; _temp1687.tag=
Cyc_Absyn_PointerType; _temp1687.f1=({ struct Cyc_Absyn_PtrInfo _temp1688;
_temp1688.elt_typ=( void*) _temp1684; _temp1688.rgn_typ=( void*) rgn; _temp1688.nullable=((
struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)(); _temp1688.tq=
_temp1682; _temp1688.bounds= Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1689=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1689[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1690; _temp1690.tag= Cyc_Absyn_Upper_b;
_temp1690.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1680); _temp1690;});
_temp1689;})); _temp1688;}); _temp1687;}); _temp1686;}); if( topt !=  0){ if( !
Cyc_Tcutil_unify(* topt, res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,*
topt): 0){ e->topt=({ struct Cyc_Core_Opt* _temp1691=( struct Cyc_Core_Opt*)
_cycalloc( sizeof( struct Cyc_Core_Opt)); _temp1691->v=( void*) res_typ;
_temp1691;}); Cyc_Tcutil_unchecked_cast( te, e,* topt); res_typ=* topt;}} goto
_LL1675; _LL1679: return({ void* _temp1692[ 0u]={}; Cyc_Tcutil_impos( _tag_arr("tcExpNoPromote on Array_e returned non-array type",
sizeof( unsigned char), 50u), _tag_arr( _temp1692, sizeof( void*), 0u));});
_LL1675:;} return res_typ;}} _LL1621: { void* _temp1693= Cyc_Absyn_atb_typ( Cyc_Absyn_uchar_t,
rgn, Cyc_Absyn_const_tqual(),( void*) Cyc_Absyn_Unknown_b); void* _temp1694= Cyc_Tcexp_tcExp(
te,( void**)& _temp1693, e1); return Cyc_Absyn_atb_typ( _temp1694, rgn, Cyc_Absyn_empty_tqual(),(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1695=( struct Cyc_Absyn_Upper_b_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp1695[ 0]=({ struct
Cyc_Absyn_Upper_b_struct _temp1696; _temp1696.tag= Cyc_Absyn_Upper_b; _temp1696.f1=
Cyc_Absyn_uint_exp( 1, 0); _temp1696;}); _temp1695;}));} _LL1623: { void** topt2=
0; if( topt !=  0){ void* _temp1697= Cyc_Tcutil_compress(* topt); struct Cyc_Absyn_PtrInfo
_temp1705; struct Cyc_Absyn_Conref* _temp1707; void* _temp1709; void** _temp1711;
_LL1699: if(( unsigned int) _temp1697 >  3u?*(( int*) _temp1697) ==  Cyc_Absyn_PointerType:
0){ _LL1706: _temp1705=(( struct Cyc_Absyn_PointerType_struct*) _temp1697)->f1;
_LL1710: _temp1709=( void*) _temp1705.elt_typ; _temp1711=( void**)&((( struct
Cyc_Absyn_PointerType_struct*) _temp1697)->f1).elt_typ; goto _LL1708; _LL1708:
_temp1707= _temp1705.nullable; goto _LL1700;} else{ goto _LL1701;} _LL1701: if((
unsigned int) _temp1697 >  3u?*(( int*) _temp1697) ==  Cyc_Absyn_TunionType: 0){
goto _LL1702;} else{ goto _LL1703;} _LL1703: goto _LL1704; _LL1700: topt2=( void**)
_temp1711; goto _LL1698; _LL1702: topt2=({ void** _temp1712=( void**) _cycalloc(
sizeof( void*)); _temp1712[ 0]=* topt; _temp1712;}); goto _LL1698; _LL1704: goto
_LL1698; _LL1698:;}{ void* telt= Cyc_Tcexp_tcExp( te, topt2, e1); void* res_typ=(
void*)({ struct Cyc_Absyn_PointerType_struct* _temp1714=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1714[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1715; _temp1715.tag= Cyc_Absyn_PointerType;
_temp1715.f1=({ struct Cyc_Absyn_PtrInfo _temp1716; _temp1716.elt_typ=( void*)
telt; _temp1716.rgn_typ=( void*) rgn; _temp1716.nullable=(( struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)(); _temp1716.tq= Cyc_Absyn_empty_tqual(); _temp1716.bounds=
Cyc_Absyn_new_conref(( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp1717=(
struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1717[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1718; _temp1718.tag= Cyc_Absyn_Upper_b;
_temp1718.f1= Cyc_Absyn_uint_exp( 1, 0); _temp1718;}); _temp1717;})); _temp1716;});
_temp1715;}); _temp1714;}); if( topt !=  0){ if( ! Cyc_Tcutil_unify(* topt,
res_typ)? Cyc_Tcutil_silent_castable( te, loc, res_typ,* topt): 0){ e->topt=({
struct Cyc_Core_Opt* _temp1713=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1713->v=( void*) res_typ; _temp1713;}); Cyc_Tcutil_unchecked_cast(
te, e,* topt); res_typ=* topt;}} return res_typ;}} _LL1613:;}} void* Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcutil_compress(
Cyc_Tcexp_tcExpNoPromote( te, topt, e)); void* _temp1719= t; struct Cyc_Absyn_Exp*
_temp1725; struct Cyc_Absyn_Tqual _temp1727; void* _temp1729; _LL1721: if((
unsigned int) _temp1719 >  3u?*(( int*) _temp1719) ==  Cyc_Absyn_ArrayType: 0){
_LL1730: _temp1729=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp1719)->f1;
goto _LL1728; _LL1728: _temp1727=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1719)->f2; goto _LL1726; _LL1726: _temp1725=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1719)->f3; goto _LL1722;} else{ goto _LL1723;} _LL1723: goto _LL1724;
_LL1722: { void* _temp1733; struct _tuple6 _temp1731= Cyc_Tcutil_addressof_props(
te, e); _LL1734: _temp1733= _temp1731.f2; goto _LL1732; _LL1732: { void*
_temp1735= _temp1725 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1736=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1736[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1737; _temp1737.tag= Cyc_Absyn_Upper_b;
_temp1737.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1725); _temp1737;});
_temp1736;}); t= Cyc_Absyn_atb_typ( _temp1729, _temp1733, _temp1727, _temp1735);(
void*)((( struct Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) t); return t;}}
_LL1724: return t; _LL1720:;} void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ void* t= Cyc_Tcexp_tcExpNoPromote( te,
topt, e); void* _temp1738=( void*) e->r; void* _temp1748; _LL1740: if(*(( int*)
_temp1738) ==  Cyc_Absyn_Array_e){ goto _LL1741;} else{ goto _LL1742;} _LL1742:
if(*(( int*) _temp1738) ==  Cyc_Absyn_Comprehension_e){ goto _LL1743;} else{
goto _LL1744;} _LL1744: if(*(( int*) _temp1738) ==  Cyc_Absyn_Const_e){ _LL1749:
_temp1748=( void*)(( struct Cyc_Absyn_Const_e_struct*) _temp1738)->f1; if((
unsigned int) _temp1748 >  1u?*(( int*) _temp1748) ==  Cyc_Absyn_String_c: 0){
goto _LL1745;} else{ goto _LL1746;}} else{ goto _LL1746;} _LL1746: goto _LL1747;
_LL1741: return t; _LL1743: return t; _LL1745: return t; _LL1747: t= Cyc_Tcutil_compress(
t);{ void* _temp1750= t; struct Cyc_Absyn_Exp* _temp1756; struct Cyc_Absyn_Tqual
_temp1758; void* _temp1760; _LL1752: if(( unsigned int) _temp1750 >  3u?*(( int*)
_temp1750) ==  Cyc_Absyn_ArrayType: 0){ _LL1761: _temp1760=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp1750)->f1; goto _LL1759; _LL1759: _temp1758=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1750)->f2; goto _LL1757; _LL1757: _temp1756=(( struct Cyc_Absyn_ArrayType_struct*)
_temp1750)->f3; goto _LL1753;} else{ goto _LL1754;} _LL1754: goto _LL1755;
_LL1753: { void* _temp1764; struct _tuple6 _temp1762= Cyc_Tcutil_addressof_props(
te, e); _LL1765: _temp1764= _temp1762.f2; goto _LL1763; _LL1763: { void* b=
_temp1756 ==  0?( void*) Cyc_Absyn_Unknown_b:( void*)({ struct Cyc_Absyn_Upper_b_struct*
_temp1766=( struct Cyc_Absyn_Upper_b_struct*) _cycalloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp1766[ 0]=({ struct Cyc_Absyn_Upper_b_struct _temp1767; _temp1767.tag= Cyc_Absyn_Upper_b;
_temp1767.f1=( struct Cyc_Absyn_Exp*) _check_null( _temp1756); _temp1767;});
_temp1766;}); t= Cyc_Absyn_atb_typ( _temp1760, _temp1764, _temp1758, b); Cyc_Tcutil_unchecked_cast(
te, e, t); return t;}} _LL1755: return t; _LL1751:;} _LL1739:;} static void* Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv* te, void** topt, struct Cyc_Absyn_Exp* e){{ void*
_temp1768=( void*) e->r; struct Cyc_Absyn_Exp* _temp1774; _LL1770: if(*(( int*)
_temp1768) ==  Cyc_Absyn_NoInstantiate_e){ _LL1775: _temp1774=(( struct Cyc_Absyn_NoInstantiate_e_struct*)
_temp1768)->f1; goto _LL1771;} else{ goto _LL1772;} _LL1772: goto _LL1773;
_LL1771: Cyc_Tcexp_tcExpNoInst( te, topt, _temp1774);( void*)((( struct Cyc_Core_Opt*)
_check_null( _temp1774->topt))->v=( void*) Cyc_Absyn_pointer_expand(( void*)((
struct Cyc_Core_Opt*) _check_null( _temp1774->topt))->v)); e->topt= _temp1774->topt;
goto _LL1769; _LL1773: Cyc_Tcexp_tcExpNoInst( te, topt, e);( void*)((( struct
Cyc_Core_Opt*) _check_null( e->topt))->v=( void*) Cyc_Absyn_pointer_expand((
void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v));{ void* _temp1776=
Cyc_Tcutil_compress(( void*)(( struct Cyc_Core_Opt*) _check_null( e->topt))->v);
struct Cyc_Absyn_PtrInfo _temp1782; struct Cyc_Absyn_Conref* _temp1784; struct
Cyc_Absyn_Tqual _temp1786; struct Cyc_Absyn_Conref* _temp1788; void* _temp1790;
void* _temp1792; _LL1778: if(( unsigned int) _temp1776 >  3u?*(( int*) _temp1776)
==  Cyc_Absyn_PointerType: 0){ _LL1783: _temp1782=(( struct Cyc_Absyn_PointerType_struct*)
_temp1776)->f1; _LL1793: _temp1792=( void*) _temp1782.elt_typ; goto _LL1791;
_LL1791: _temp1790=( void*) _temp1782.rgn_typ; goto _LL1789; _LL1789: _temp1788=
_temp1782.nullable; goto _LL1787; _LL1787: _temp1786= _temp1782.tq; goto _LL1785;
_LL1785: _temp1784= _temp1782.bounds; goto _LL1779;} else{ goto _LL1780;}
_LL1780: goto _LL1781; _LL1779:{ void* _temp1794= Cyc_Tcutil_compress( _temp1792);
struct Cyc_Absyn_FnInfo _temp1800; struct Cyc_List_List* _temp1802; struct Cyc_List_List*
_temp1804; struct Cyc_Absyn_VarargInfo* _temp1806; int _temp1808; struct Cyc_List_List*
_temp1810; void* _temp1812; struct Cyc_Core_Opt* _temp1814; struct Cyc_List_List*
_temp1816; _LL1796: if(( unsigned int) _temp1794 >  3u?*(( int*) _temp1794) == 
Cyc_Absyn_FnType: 0){ _LL1801: _temp1800=(( struct Cyc_Absyn_FnType_struct*)
_temp1794)->f1; _LL1817: _temp1816= _temp1800.tvars; goto _LL1815; _LL1815:
_temp1814= _temp1800.effect; goto _LL1813; _LL1813: _temp1812=( void*) _temp1800.ret_typ;
goto _LL1811; _LL1811: _temp1810= _temp1800.args; goto _LL1809; _LL1809:
_temp1808= _temp1800.c_varargs; goto _LL1807; _LL1807: _temp1806= _temp1800.cyc_varargs;
goto _LL1805; _LL1805: _temp1804= _temp1800.rgn_po; goto _LL1803; _LL1803:
_temp1802= _temp1800.attributes; goto _LL1797;} else{ goto _LL1798;} _LL1798:
goto _LL1799; _LL1797: if( _temp1816 !=  0){ struct _RegionHandle _temp1818=
_new_region("rgn"); struct _RegionHandle* rgn=& _temp1818; _push_region( rgn);{
struct _tuple4 _temp1819=({ struct _tuple4 _temp1829; _temp1829.f1= Cyc_Tcenv_lookup_type_vars(
te); _temp1829.f2= rgn; _temp1829;}); struct Cyc_List_List* inst=(( struct Cyc_List_List*(*)(
struct _RegionHandle*, struct _tuple5*(* f)( struct _tuple4*, struct Cyc_Absyn_Tvar*),
struct _tuple4* env, struct Cyc_List_List* x)) Cyc_List_rmap_c)( rgn, Cyc_Tcutil_r_make_inst_var,&
_temp1819, _temp1816); struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
void*(* f)( struct _tuple5*), struct Cyc_List_List* x)) Cyc_List_map)(( void*(*)(
struct _tuple5*)) Cyc_Core_snd, inst); void* ftyp= Cyc_Tcutil_rsubstitute( rgn,
inst,( void*)({ struct Cyc_Absyn_FnType_struct* _temp1826=( struct Cyc_Absyn_FnType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_FnType_struct)); _temp1826[ 0]=({ struct Cyc_Absyn_FnType_struct
_temp1827; _temp1827.tag= Cyc_Absyn_FnType; _temp1827.f1=({ struct Cyc_Absyn_FnInfo
_temp1828; _temp1828.tvars= 0; _temp1828.effect= _temp1814; _temp1828.ret_typ=(
void*) _temp1812; _temp1828.args= _temp1810; _temp1828.c_varargs= _temp1808;
_temp1828.cyc_varargs= _temp1806; _temp1828.rgn_po= _temp1804; _temp1828.attributes=
_temp1802; _temp1828;}); _temp1827;}); _temp1826;})); void* new_typ=( void*)({
struct Cyc_Absyn_PointerType_struct* _temp1823=( struct Cyc_Absyn_PointerType_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp1823[ 0]=({
struct Cyc_Absyn_PointerType_struct _temp1824; _temp1824.tag= Cyc_Absyn_PointerType;
_temp1824.f1=({ struct Cyc_Absyn_PtrInfo _temp1825; _temp1825.elt_typ=( void*)
ftyp; _temp1825.rgn_typ=( void*) _temp1790; _temp1825.nullable= _temp1788;
_temp1825.tq= _temp1786; _temp1825.bounds= _temp1784; _temp1825;}); _temp1824;});
_temp1823;}); struct Cyc_Absyn_Exp* inner= Cyc_Absyn_copy_exp( e);( void*)( e->r=(
void*)(( void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp1820=( struct Cyc_Absyn_Instantiate_e_struct*)
_cycalloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp1820[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp1821; _temp1821.tag= Cyc_Absyn_Instantiate_e;
_temp1821.f1= inner; _temp1821.f2= ts; _temp1821;}); _temp1820;}))); e->topt=({
struct Cyc_Core_Opt* _temp1822=( struct Cyc_Core_Opt*) _cycalloc( sizeof( struct
Cyc_Core_Opt)); _temp1822->v=( void*) new_typ; _temp1822;});}; _pop_region( rgn);}
goto _LL1795; _LL1799: goto _LL1795; _LL1795:;} goto _LL1777; _LL1781: goto
_LL1777; _LL1777:;} goto _LL1769; _LL1769:;} return( void*)(( struct Cyc_Core_Opt*)
_check_null( e->topt))->v;} static void Cyc_Tcexp_tcExpNoInst( struct Cyc_Tcenv_Tenv*
te, void** topt, struct Cyc_Absyn_Exp* e){ struct Cyc_Position_Segment* loc= e->loc;
void* t;{ void* _temp1830=( void*) e->r; struct Cyc_Absyn_Exp* _temp1908; struct
_tuple1* _temp1910; struct Cyc_List_List* _temp1912; struct Cyc_Absyn_Exp*
_temp1914; struct Cyc_List_List* _temp1916; struct Cyc_Core_Opt* _temp1918; void*
_temp1920; void* _temp1922; struct _tuple1* _temp1924; struct Cyc_List_List*
_temp1926; void* _temp1928; void* _temp1930; struct Cyc_Absyn_Exp* _temp1932;
struct Cyc_Absyn_Exp* _temp1934; struct Cyc_Core_Opt* _temp1936; struct Cyc_Absyn_Exp*
_temp1938; struct Cyc_Absyn_Exp* _temp1940; struct Cyc_Absyn_Exp* _temp1942;
struct Cyc_Absyn_Exp* _temp1944; struct Cyc_Absyn_Exp* _temp1946; struct Cyc_Absyn_Exp*
_temp1948; struct Cyc_Absyn_VarargCallInfo* _temp1950; struct Cyc_Absyn_VarargCallInfo**
_temp1952; struct Cyc_List_List* _temp1953; struct Cyc_Absyn_Exp* _temp1955;
struct Cyc_Absyn_Exp* _temp1957; struct Cyc_List_List* _temp1959; struct Cyc_Absyn_Exp*
_temp1961; struct Cyc_Absyn_Exp* _temp1963; void* _temp1965; struct Cyc_Absyn_Exp*
_temp1967; struct Cyc_Absyn_Exp* _temp1969; struct Cyc_Absyn_Exp* _temp1971;
struct Cyc_Absyn_Exp* _temp1973; void* _temp1975; void* _temp1977; void*
_temp1979; struct Cyc_Absyn_Exp* _temp1981; struct _tagged_arr* _temp1983;
struct Cyc_Absyn_Exp* _temp1985; struct _tagged_arr* _temp1987; struct Cyc_Absyn_Exp*
_temp1989; struct Cyc_Absyn_Exp* _temp1991; struct Cyc_Absyn_Exp* _temp1993;
struct Cyc_List_List* _temp1995; struct Cyc_List_List* _temp1997; struct _tuple2*
_temp1999; struct Cyc_List_List* _temp2001; struct Cyc_Absyn_Stmt* _temp2003;
struct Cyc_Absyn_Fndecl* _temp2005; struct Cyc_Absyn_Exp* _temp2007; struct Cyc_Absyn_Exp*
_temp2009; struct Cyc_Absyn_Exp* _temp2011; struct Cyc_Absyn_Vardecl* _temp2013;
struct Cyc_Absyn_Structdecl* _temp2015; struct Cyc_Absyn_Structdecl** _temp2017;
struct Cyc_List_List* _temp2018; struct Cyc_Core_Opt* _temp2020; struct Cyc_Core_Opt**
_temp2022; struct _tuple1* _temp2023; struct _tuple1** _temp2025; struct Cyc_List_List*
_temp2026; void* _temp2028; struct Cyc_Absyn_Tunionfield* _temp2030; struct Cyc_Absyn_Tuniondecl*
_temp2032; struct Cyc_List_List* _temp2034; struct Cyc_Core_Opt* _temp2036;
struct Cyc_Core_Opt** _temp2038; struct Cyc_Core_Opt* _temp2039; struct Cyc_Core_Opt**
_temp2041; struct Cyc_Absyn_Enumfield* _temp2042; struct Cyc_Absyn_Enumdecl*
_temp2044; struct _tuple1* _temp2046; struct _tuple1** _temp2048; struct Cyc_Absyn_Enumfield*
_temp2049; void* _temp2051; struct _tuple1* _temp2053; struct _tuple1**
_temp2055; struct Cyc_Absyn_MallocInfo _temp2056; int _temp2058; int* _temp2060;
struct Cyc_Absyn_Exp* _temp2061; struct Cyc_Absyn_Exp** _temp2063; void**
_temp2064; void*** _temp2066; struct Cyc_Absyn_Exp* _temp2067; int _temp2069;
_LL1832: if(*(( int*) _temp1830) ==  Cyc_Absyn_NoInstantiate_e){ _LL1909:
_temp1908=(( struct Cyc_Absyn_NoInstantiate_e_struct*) _temp1830)->f1; goto
_LL1833;} else{ goto _LL1834;} _LL1834: if(*(( int*) _temp1830) ==  Cyc_Absyn_UnknownId_e){
_LL1911: _temp1910=(( struct Cyc_Absyn_UnknownId_e_struct*) _temp1830)->f1; goto
_LL1835;} else{ goto _LL1836;} _LL1836: if(*(( int*) _temp1830) ==  Cyc_Absyn_UnknownCall_e){
_LL1915: _temp1914=(( struct Cyc_Absyn_UnknownCall_e_struct*) _temp1830)->f1;
goto _LL1913; _LL1913: _temp1912=(( struct Cyc_Absyn_UnknownCall_e_struct*)
_temp1830)->f2; goto _LL1837;} else{ goto _LL1838;} _LL1838: if(*(( int*)
_temp1830) ==  Cyc_Absyn_UnresolvedMem_e){ _LL1919: _temp1918=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1830)->f1; goto _LL1917; _LL1917: _temp1916=(( struct Cyc_Absyn_UnresolvedMem_e_struct*)
_temp1830)->f2; goto _LL1839;} else{ goto _LL1840;} _LL1840: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Const_e){ _LL1921: _temp1920=( void*)(( struct Cyc_Absyn_Const_e_struct*)
_temp1830)->f1; goto _LL1841;} else{ goto _LL1842;} _LL1842: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Var_e){ _LL1925: _temp1924=(( struct Cyc_Absyn_Var_e_struct*)
_temp1830)->f1; goto _LL1923; _LL1923: _temp1922=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp1830)->f2; goto _LL1843;} else{ goto _LL1844;} _LL1844: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Primop_e){ _LL1929: _temp1928=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp1830)->f1; goto _LL1927; _LL1927: _temp1926=(( struct Cyc_Absyn_Primop_e_struct*)
_temp1830)->f2; goto _LL1845;} else{ goto _LL1846;} _LL1846: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Increment_e){ _LL1933: _temp1932=(( struct Cyc_Absyn_Increment_e_struct*)
_temp1830)->f1; goto _LL1931; _LL1931: _temp1930=( void*)(( struct Cyc_Absyn_Increment_e_struct*)
_temp1830)->f2; goto _LL1847;} else{ goto _LL1848;} _LL1848: if(*(( int*)
_temp1830) ==  Cyc_Absyn_AssignOp_e){ _LL1939: _temp1938=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1830)->f1; goto _LL1937; _LL1937: _temp1936=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1830)->f2; goto _LL1935; _LL1935: _temp1934=(( struct Cyc_Absyn_AssignOp_e_struct*)
_temp1830)->f3; goto _LL1849;} else{ goto _LL1850;} _LL1850: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Conditional_e){ _LL1945: _temp1944=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1830)->f1; goto _LL1943; _LL1943: _temp1942=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1830)->f2; goto _LL1941; _LL1941: _temp1940=(( struct Cyc_Absyn_Conditional_e_struct*)
_temp1830)->f3; goto _LL1851;} else{ goto _LL1852;} _LL1852: if(*(( int*)
_temp1830) ==  Cyc_Absyn_SeqExp_e){ _LL1949: _temp1948=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1830)->f1; goto _LL1947; _LL1947: _temp1946=(( struct Cyc_Absyn_SeqExp_e_struct*)
_temp1830)->f2; goto _LL1853;} else{ goto _LL1854;} _LL1854: if(*(( int*)
_temp1830) ==  Cyc_Absyn_FnCall_e){ _LL1956: _temp1955=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1830)->f1; goto _LL1954; _LL1954: _temp1953=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1830)->f2; goto _LL1951; _LL1951: _temp1950=(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1830)->f3; _temp1952=( struct Cyc_Absyn_VarargCallInfo**)&(( struct Cyc_Absyn_FnCall_e_struct*)
_temp1830)->f3; goto _LL1855;} else{ goto _LL1856;} _LL1856: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Throw_e){ _LL1958: _temp1957=(( struct Cyc_Absyn_Throw_e_struct*)
_temp1830)->f1; goto _LL1857;} else{ goto _LL1858;} _LL1858: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Instantiate_e){ _LL1962: _temp1961=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1830)->f1; goto _LL1960; _LL1960: _temp1959=(( struct Cyc_Absyn_Instantiate_e_struct*)
_temp1830)->f2; goto _LL1859;} else{ goto _LL1860;} _LL1860: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Cast_e){ _LL1966: _temp1965=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp1830)->f1; goto _LL1964; _LL1964: _temp1963=(( struct Cyc_Absyn_Cast_e_struct*)
_temp1830)->f2; goto _LL1861;} else{ goto _LL1862;} _LL1862: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Address_e){ _LL1968: _temp1967=(( struct Cyc_Absyn_Address_e_struct*)
_temp1830)->f1; goto _LL1863;} else{ goto _LL1864;} _LL1864: if(*(( int*)
_temp1830) ==  Cyc_Absyn_New_e){ _LL1972: _temp1971=(( struct Cyc_Absyn_New_e_struct*)
_temp1830)->f1; goto _LL1970; _LL1970: _temp1969=(( struct Cyc_Absyn_New_e_struct*)
_temp1830)->f2; goto _LL1865;} else{ goto _LL1866;} _LL1866: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Sizeofexp_e){ _LL1974: _temp1973=(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp1830)->f1; goto _LL1867;} else{ goto _LL1868;} _LL1868: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Sizeoftyp_e){ _LL1976: _temp1975=( void*)(( struct Cyc_Absyn_Sizeoftyp_e_struct*)
_temp1830)->f1; goto _LL1869;} else{ goto _LL1870;} _LL1870: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Offsetof_e){ _LL1980: _temp1979=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1830)->f1; goto _LL1978; _LL1978: _temp1977=( void*)(( struct Cyc_Absyn_Offsetof_e_struct*)
_temp1830)->f2; goto _LL1871;} else{ goto _LL1872;} _LL1872: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Gentyp_e){ goto _LL1873;} else{ goto _LL1874;} _LL1874:
if(*(( int*) _temp1830) ==  Cyc_Absyn_Deref_e){ _LL1982: _temp1981=(( struct Cyc_Absyn_Deref_e_struct*)
_temp1830)->f1; goto _LL1875;} else{ goto _LL1876;} _LL1876: if(*(( int*)
_temp1830) ==  Cyc_Absyn_StructMember_e){ _LL1986: _temp1985=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1830)->f1; goto _LL1984; _LL1984: _temp1983=(( struct Cyc_Absyn_StructMember_e_struct*)
_temp1830)->f2; goto _LL1877;} else{ goto _LL1878;} _LL1878: if(*(( int*)
_temp1830) ==  Cyc_Absyn_StructArrow_e){ _LL1990: _temp1989=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1830)->f1; goto _LL1988; _LL1988: _temp1987=(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp1830)->f2; goto _LL1879;} else{ goto _LL1880;} _LL1880: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Subscript_e){ _LL1994: _temp1993=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1830)->f1; goto _LL1992; _LL1992: _temp1991=(( struct Cyc_Absyn_Subscript_e_struct*)
_temp1830)->f2; goto _LL1881;} else{ goto _LL1882;} _LL1882: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Tuple_e){ _LL1996: _temp1995=(( struct Cyc_Absyn_Tuple_e_struct*)
_temp1830)->f1; goto _LL1883;} else{ goto _LL1884;} _LL1884: if(*(( int*)
_temp1830) ==  Cyc_Absyn_CompoundLit_e){ _LL2000: _temp1999=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1830)->f1; goto _LL1998; _LL1998: _temp1997=(( struct Cyc_Absyn_CompoundLit_e_struct*)
_temp1830)->f2; goto _LL1885;} else{ goto _LL1886;} _LL1886: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Array_e){ _LL2002: _temp2001=(( struct Cyc_Absyn_Array_e_struct*)
_temp1830)->f1; goto _LL1887;} else{ goto _LL1888;} _LL1888: if(*(( int*)
_temp1830) ==  Cyc_Absyn_StmtExp_e){ _LL2004: _temp2003=(( struct Cyc_Absyn_StmtExp_e_struct*)
_temp1830)->f1; goto _LL1889;} else{ goto _LL1890;} _LL1890: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Codegen_e){ _LL2006: _temp2005=(( struct Cyc_Absyn_Codegen_e_struct*)
_temp1830)->f1; goto _LL1891;} else{ goto _LL1892;} _LL1892: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Fill_e){ _LL2008: _temp2007=(( struct Cyc_Absyn_Fill_e_struct*)
_temp1830)->f1; goto _LL1893;} else{ goto _LL1894;} _LL1894: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Comprehension_e){ _LL2014: _temp2013=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1830)->f1; goto _LL2012; _LL2012: _temp2011=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1830)->f2; goto _LL2010; _LL2010: _temp2009=(( struct Cyc_Absyn_Comprehension_e_struct*)
_temp1830)->f3; goto _LL1895;} else{ goto _LL1896;} _LL1896: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Struct_e){ _LL2024: _temp2023=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1830)->f1; _temp2025=( struct _tuple1**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1830)->f1; goto _LL2021; _LL2021: _temp2020=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1830)->f2; _temp2022=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1830)->f2; goto _LL2019; _LL2019: _temp2018=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1830)->f3; goto _LL2016; _LL2016: _temp2015=(( struct Cyc_Absyn_Struct_e_struct*)
_temp1830)->f4; _temp2017=( struct Cyc_Absyn_Structdecl**)&(( struct Cyc_Absyn_Struct_e_struct*)
_temp1830)->f4; goto _LL1897;} else{ goto _LL1898;} _LL1898: if(*(( int*)
_temp1830) ==  Cyc_Absyn_AnonStruct_e){ _LL2029: _temp2028=( void*)(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1830)->f1; goto _LL2027; _LL2027: _temp2026=(( struct Cyc_Absyn_AnonStruct_e_struct*)
_temp1830)->f2; goto _LL1899;} else{ goto _LL1900;} _LL1900: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Tunion_e){ _LL2040: _temp2039=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1830)->f1; _temp2041=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1830)->f1; goto _LL2037; _LL2037: _temp2036=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1830)->f2; _temp2038=( struct Cyc_Core_Opt**)&(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1830)->f2; goto _LL2035; _LL2035: _temp2034=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1830)->f3; goto _LL2033; _LL2033: _temp2032=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1830)->f4; goto _LL2031; _LL2031: _temp2030=(( struct Cyc_Absyn_Tunion_e_struct*)
_temp1830)->f5; goto _LL1901;} else{ goto _LL1902;} _LL1902: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Enum_e){ _LL2047: _temp2046=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1830)->f1; _temp2048=( struct _tuple1**)&(( struct Cyc_Absyn_Enum_e_struct*)
_temp1830)->f1; goto _LL2045; _LL2045: _temp2044=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1830)->f2; goto _LL2043; _LL2043: _temp2042=(( struct Cyc_Absyn_Enum_e_struct*)
_temp1830)->f3; goto _LL1903;} else{ goto _LL1904;} _LL1904: if(*(( int*)
_temp1830) ==  Cyc_Absyn_AnonEnum_e){ _LL2054: _temp2053=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1830)->f1; _temp2055=( struct _tuple1**)&(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1830)->f1; goto _LL2052; _LL2052: _temp2051=( void*)(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1830)->f2; goto _LL2050; _LL2050: _temp2049=(( struct Cyc_Absyn_AnonEnum_e_struct*)
_temp1830)->f3; goto _LL1905;} else{ goto _LL1906;} _LL1906: if(*(( int*)
_temp1830) ==  Cyc_Absyn_Malloc_e){ _LL2057: _temp2056=(( struct Cyc_Absyn_Malloc_e_struct*)
_temp1830)->f1; _LL2070: _temp2069= _temp2056.is_calloc; goto _LL2068; _LL2068:
_temp2067= _temp2056.rgn; goto _LL2065; _LL2065: _temp2064= _temp2056.elt_type;
_temp2066=( void***)&((( struct Cyc_Absyn_Malloc_e_struct*) _temp1830)->f1).elt_type;
goto _LL2062; _LL2062: _temp2061= _temp2056.num_elts; _temp2063=( struct Cyc_Absyn_Exp**)&(((
struct Cyc_Absyn_Malloc_e_struct*) _temp1830)->f1).num_elts; goto _LL2059;
_LL2059: _temp2058= _temp2056.fat_result; _temp2060=( int*)&((( struct Cyc_Absyn_Malloc_e_struct*)
_temp1830)->f1).fat_result; goto _LL1907;} else{ goto _LL1831;} _LL1833: Cyc_Tcexp_tcExpNoInst(
te, 0, _temp1908); return; _LL1835: Cyc_Tcexp_resolve_unknown_id( te, e,
_temp1910); Cyc_Tcexp_tcExpNoInst( te, topt, e); return; _LL1837: Cyc_Tcexp_resolve_unknown_fn(
te, e, _temp1914, _temp1912); Cyc_Tcexp_tcExpNoInst( te, topt, e); return;
_LL1839: Cyc_Tcexp_resolve_unresolved_mem( te, loc, topt, e, _temp1916); Cyc_Tcexp_tcExpNoInst(
te, topt, e); return; _LL1841: t= Cyc_Tcexp_tcConst( te, loc, topt, _temp1920, e);
goto _LL1831; _LL1843: t= Cyc_Tcexp_tcVar( te, loc, _temp1924, _temp1922); goto
_LL1831; _LL1845: t= Cyc_Tcexp_tcPrimop( te, loc, topt, _temp1928, _temp1926);
goto _LL1831; _LL1847: t= Cyc_Tcexp_tcIncrement( te, loc, topt, _temp1932,
_temp1930); goto _LL1831; _LL1849: t= Cyc_Tcexp_tcAssignOp( te, loc, topt,
_temp1938, _temp1936, _temp1934); goto _LL1831; _LL1851: t= Cyc_Tcexp_tcConditional(
te, loc, topt, _temp1944, _temp1942, _temp1940); goto _LL1831; _LL1853: t= Cyc_Tcexp_tcSeqExp(
te, loc, topt, _temp1948, _temp1946); goto _LL1831; _LL1855: t= Cyc_Tcexp_tcFnCall(
te, loc, topt, _temp1955, _temp1953, _temp1952); goto _LL1831; _LL1857: t= Cyc_Tcexp_tcThrow(
te, loc, topt, _temp1957); goto _LL1831; _LL1859: t= Cyc_Tcexp_tcInstantiate( te,
loc, topt, _temp1961, _temp1959); goto _LL1831; _LL1861: t= Cyc_Tcexp_tcCast( te,
loc, topt, _temp1965, _temp1963); goto _LL1831; _LL1863: t= Cyc_Tcexp_tcAddress(
te, loc, e, topt, _temp1967); goto _LL1831; _LL1865: t= Cyc_Tcexp_tcNew( te, loc,
topt, _temp1971, e, _temp1969); goto _LL1831; _LL1867: { void* _temp2071= Cyc_Tcexp_tcExpNoPromote(
te, 0, _temp1973); t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp2071); goto
_LL1831;} _LL1869: t= Cyc_Tcexp_tcSizeof( te, loc, topt, _temp1975); goto
_LL1831; _LL1871: t= Cyc_Tcexp_tcOffsetof( te, loc, topt, _temp1979, _temp1977);
goto _LL1831; _LL1873:({ void* _temp2072[ 0u]={}; Cyc_Tcutil_terr( loc, _tag_arr("gen() not in top-level initializer",
sizeof( unsigned char), 35u), _tag_arr( _temp2072, sizeof( void*), 0u));});
return; _LL1875: t= Cyc_Tcexp_tcDeref( te, loc, topt, _temp1981); goto _LL1831;
_LL1877: t= Cyc_Tcexp_tcStructMember( te, loc, topt, e, _temp1985, _temp1983);
goto _LL1831; _LL1879: t= Cyc_Tcexp_tcStructArrow( te, loc, topt, _temp1989,
_temp1987); goto _LL1831; _LL1881: t= Cyc_Tcexp_tcSubscript( te, loc, topt,
_temp1993, _temp1991); goto _LL1831; _LL1883: t= Cyc_Tcexp_tcTuple( te, loc,
topt, _temp1995); goto _LL1831; _LL1885: t= Cyc_Tcexp_tcCompoundLit( te, loc,
topt, _temp1999, _temp1997); goto _LL1831; _LL1887: { void** elt_topt= 0; if(
topt !=  0){ void* _temp2073= Cyc_Tcutil_compress(* topt); void* _temp2079; void**
_temp2081; _LL2075: if(( unsigned int) _temp2073 >  3u?*(( int*) _temp2073) == 
Cyc_Absyn_ArrayType: 0){ _LL2080: _temp2079=( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2073)->f1; _temp2081=( void**)&(( void*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp2073)->f1); goto _LL2076;} else{ goto _LL2077;} _LL2077: goto _LL2078;
_LL2076: elt_topt=( void**) _temp2081; goto _LL2074; _LL2078: goto _LL2074;
_LL2074:;} t= Cyc_Tcexp_tcArray( te, loc, elt_topt, _temp2001); goto _LL1831;}
_LL1889: t= Cyc_Tcexp_tcStmtExp( te, loc, topt, _temp2003); goto _LL1831;
_LL1891: t= Cyc_Tcexp_tcCodegen( te, loc, topt, _temp2005); goto _LL1831;
_LL1893: t= Cyc_Tcexp_tcFill( te, loc, topt, _temp2007); goto _LL1831; _LL1895:
t= Cyc_Tcexp_tcComprehension( te, loc, topt, _temp2013, _temp2011, _temp2009);
goto _LL1831; _LL1897: t= Cyc_Tcexp_tcStruct( te, loc, topt, _temp2025,
_temp2022, _temp2018, _temp2017); goto _LL1831; _LL1899: t= Cyc_Tcexp_tcAnonStruct(
te, loc, _temp2028, _temp2026); goto _LL1831; _LL1901: t= Cyc_Tcexp_tcTunion( te,
loc, topt, e, _temp2041, _temp2038, _temp2034, _temp2032, _temp2030); goto
_LL1831; _LL1903:* _temp2048=(( struct Cyc_Absyn_Enumfield*) _check_null(
_temp2042))->name; t=( void*)({ struct Cyc_Absyn_EnumType_struct* _temp2082=(
struct Cyc_Absyn_EnumType_struct*) _cycalloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp2082[ 0]=({ struct Cyc_Absyn_EnumType_struct _temp2083; _temp2083.tag= Cyc_Absyn_EnumType;
_temp2083.f1=(( struct Cyc_Absyn_Enumdecl*) _check_null( _temp2044))->name;
_temp2083.f2= _temp2044; _temp2083;}); _temp2082;}); goto _LL1831; _LL1905:*
_temp2055=(( struct Cyc_Absyn_Enumfield*) _check_null( _temp2049))->name; t=
_temp2051; goto _LL1831; _LL1907: t= Cyc_Tcexp_tcMalloc( te, loc, topt,
_temp2067, _temp2066, _temp2063, _temp2069, _temp2060); goto _LL1831; _LL1831:;}
e->topt=({ struct Cyc_Core_Opt* _temp2084=( struct Cyc_Core_Opt*) _cycalloc(
sizeof( struct Cyc_Core_Opt)); _temp2084->v=( void*) t; _temp2084;});}
