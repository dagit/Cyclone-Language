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
 struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern int Cyc_List_length(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,
void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);extern struct
Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern unsigned char Cyc_List_Nth[
8];struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};extern unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
static const int Cyc_Position_Lex=0;static const int Cyc_Position_Parse=1;static const
int Cyc_Position_Elab=2;struct Cyc_Position_Error{struct _tagged_arr source;struct
Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern unsigned char
Cyc_Position_Nocontext[14];static const int Cyc_Absyn_Loc_n=0;static const int Cyc_Absyn_Rel_n=
0;struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_Abs_n=
1;struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple0{
void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;static const int Cyc_Absyn_Static=
0;static const int Cyc_Absyn_Abstract=1;static const int Cyc_Absyn_Public=2;static
const int Cyc_Absyn_Extern=3;static const int Cyc_Absyn_ExternC=4;struct Cyc_Absyn_Tqual{
int q_const: 1;int q_volatile: 1;int q_restrict: 1;};static const int Cyc_Absyn_B1=0;
static const int Cyc_Absyn_B2=1;static const int Cyc_Absyn_B4=2;static const int Cyc_Absyn_B8=
3;static const int Cyc_Absyn_AnyKind=0;static const int Cyc_Absyn_MemKind=1;static
const int Cyc_Absyn_BoxKind=2;static const int Cyc_Absyn_RgnKind=3;static const int Cyc_Absyn_EffKind=
4;static const int Cyc_Absyn_Signed=0;static const int Cyc_Absyn_Unsigned=1;struct Cyc_Absyn_Conref{
void*v;};static const int Cyc_Absyn_Eq_constr=0;struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};static const int Cyc_Absyn_Forward_constr=1;struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};static const int Cyc_Absyn_No_constr=0;static
const int Cyc_Absyn_Eq_kb=0;struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};static
const int Cyc_Absyn_Unknown_kb=1;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct
Cyc_Core_Opt*f1;};static const int Cyc_Absyn_Less_kb=2;struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};static const int Cyc_Absyn_Unknown_b=0;static const int
Cyc_Absyn_Upper_b=0;struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_Absyn_PtrInfo{void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*
nullable;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Conref*bounds;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple0*name;int is_xtunion;};static const int Cyc_Absyn_UnknownTunion=0;struct Cyc_Absyn_UnknownTunion_struct{
int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};static const int Cyc_Absyn_KnownTunion=
1;struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};
struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*targs;void*rgn;}
;struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*tunion_name;struct _tuple0*
field_name;int is_xtunion;};static const int Cyc_Absyn_UnknownTunionfield=0;struct
Cyc_Absyn_UnknownTunionfield_struct{int tag;struct Cyc_Absyn_UnknownTunionFieldInfo
f1;};static const int Cyc_Absyn_KnownTunionfield=1;struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};static const int Cyc_Absyn_VoidType=0;
static const int Cyc_Absyn_Evar=0;struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*
f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};static const int Cyc_Absyn_VarType=
1;struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};static const int
Cyc_Absyn_TunionType=2;struct Cyc_Absyn_TunionType_struct{int tag;struct Cyc_Absyn_TunionInfo
f1;};static const int Cyc_Absyn_TunionFieldType=3;struct Cyc_Absyn_TunionFieldType_struct{
int tag;struct Cyc_Absyn_TunionFieldInfo f1;};static const int Cyc_Absyn_PointerType=4;
struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};static
const int Cyc_Absyn_IntType=5;struct Cyc_Absyn_IntType_struct{int tag;void*f1;void*
f2;};static const int Cyc_Absyn_FloatType=1;static const int Cyc_Absyn_DoubleType=6;
struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};static const int Cyc_Absyn_ArrayType=
7;struct Cyc_Absyn_ArrayType_struct{int tag;void*f1;struct Cyc_Absyn_Tqual f2;struct
Cyc_Absyn_Exp*f3;};static const int Cyc_Absyn_FnType=8;struct Cyc_Absyn_FnType_struct{
int tag;struct Cyc_Absyn_FnInfo f1;};static const int Cyc_Absyn_TupleType=9;struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_StructType=10;struct Cyc_Absyn_StructType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};
static const int Cyc_Absyn_UnionType=11;struct Cyc_Absyn_UnionType_struct{int tag;
struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};static
const int Cyc_Absyn_EnumType=12;struct Cyc_Absyn_EnumType_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};static const int Cyc_Absyn_SizeofType=13;
struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};static const int Cyc_Absyn_AnonStructType=
14;struct Cyc_Absyn_AnonStructType_struct{int tag;struct Cyc_List_List*f1;};static
const int Cyc_Absyn_AnonUnionType=15;struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};static const int Cyc_Absyn_AnonEnumType=16;struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_RgnHandleType=17;struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};static const int Cyc_Absyn_TypedefType=
18;struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;struct Cyc_Core_Opt*f3;};static const int Cyc_Absyn_HeapRgn=2;static const int Cyc_Absyn_AccessEff=
19;struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};static const int Cyc_Absyn_JoinEff=
20;struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};static const int
Cyc_Absyn_RgnsEff=21;struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};static const
int Cyc_Absyn_NoTypes=0;struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*
f1;struct Cyc_Position_Segment*f2;};static const int Cyc_Absyn_WithTypes=1;struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};static const int Cyc_Absyn_NonNullable_ps=0;struct Cyc_Absyn_NonNullable_ps_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Nullable_ps=1;struct Cyc_Absyn_Nullable_ps_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_TaggedArray_ps=0;static
const int Cyc_Absyn_Printf_ft=0;static const int Cyc_Absyn_Scanf_ft=1;static const int
Cyc_Absyn_Regparm_att=0;struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};static
const int Cyc_Absyn_Stdcall_att=0;static const int Cyc_Absyn_Cdecl_att=1;static const
int Cyc_Absyn_Fastcall_att=2;static const int Cyc_Absyn_Noreturn_att=3;static const
int Cyc_Absyn_Const_att=4;static const int Cyc_Absyn_Aligned_att=1;struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};static const int Cyc_Absyn_Packed_att=5;static const int Cyc_Absyn_Section_att=
2;struct Cyc_Absyn_Section_att_struct{int tag;struct _tagged_arr f1;};static const int
Cyc_Absyn_Nocommon_att=6;static const int Cyc_Absyn_Shared_att=7;static const int Cyc_Absyn_Unused_att=
8;static const int Cyc_Absyn_Weak_att=9;static const int Cyc_Absyn_Dllimport_att=10;
static const int Cyc_Absyn_Dllexport_att=11;static const int Cyc_Absyn_No_instrument_function_att=
12;static const int Cyc_Absyn_Constructor_att=13;static const int Cyc_Absyn_Destructor_att=
14;static const int Cyc_Absyn_No_check_memory_usage_att=15;static const int Cyc_Absyn_Format_att=
3;struct Cyc_Absyn_Format_att_struct{int tag;void*f1;int f2;int f3;};static const int
Cyc_Absyn_Carray_mod=0;static const int Cyc_Absyn_ConstArray_mod=0;struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Pointer_mod=1;struct Cyc_Absyn_Pointer_mod_struct{
int tag;void*f1;void*f2;struct Cyc_Absyn_Tqual f3;};static const int Cyc_Absyn_Function_mod=
2;struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};static const int Cyc_Absyn_TypeParams_mod=
3;struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};static const int Cyc_Absyn_Attributes_mod=4;struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Char_c=
0;struct Cyc_Absyn_Char_c_struct{int tag;void*f1;unsigned char f2;};static const int
Cyc_Absyn_Short_c=1;struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};
static const int Cyc_Absyn_Int_c=2;struct Cyc_Absyn_Int_c_struct{int tag;void*f1;int
f2;};static const int Cyc_Absyn_LongLong_c=3;struct Cyc_Absyn_LongLong_c_struct{int
tag;void*f1;long long f2;};static const int Cyc_Absyn_Float_c=4;struct Cyc_Absyn_Float_c_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Absyn_String_c=5;struct Cyc_Absyn_String_c_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Absyn_Null_c=0;static const int Cyc_Absyn_Plus=
0;static const int Cyc_Absyn_Times=1;static const int Cyc_Absyn_Minus=2;static const int
Cyc_Absyn_Div=3;static const int Cyc_Absyn_Mod=4;static const int Cyc_Absyn_Eq=5;
static const int Cyc_Absyn_Neq=6;static const int Cyc_Absyn_Gt=7;static const int Cyc_Absyn_Lt=
8;static const int Cyc_Absyn_Gte=9;static const int Cyc_Absyn_Lte=10;static const int Cyc_Absyn_Not=
11;static const int Cyc_Absyn_Bitnot=12;static const int Cyc_Absyn_Bitand=13;static
const int Cyc_Absyn_Bitor=14;static const int Cyc_Absyn_Bitxor=15;static const int Cyc_Absyn_Bitlshift=
16;static const int Cyc_Absyn_Bitlrshift=17;static const int Cyc_Absyn_Bitarshift=18;
static const int Cyc_Absyn_Size=19;static const int Cyc_Absyn_PreInc=0;static const int
Cyc_Absyn_PostInc=1;static const int Cyc_Absyn_PreDec=2;static const int Cyc_Absyn_PostDec=
3;struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;
struct Cyc_Absyn_VarargInfo*vai;};static const int Cyc_Absyn_StructField=0;struct Cyc_Absyn_StructField_struct{
int tag;struct _tagged_arr*f1;};static const int Cyc_Absyn_TupleIndex=1;struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*
rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};static const int
Cyc_Absyn_Const_e=0;struct Cyc_Absyn_Const_e_struct{int tag;void*f1;};static const
int Cyc_Absyn_Var_e=1;struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*
f2;};static const int Cyc_Absyn_UnknownId_e=2;struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple0*f1;};static const int Cyc_Absyn_Primop_e=3;struct Cyc_Absyn_Primop_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_AssignOp_e=4;
struct Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};static const int Cyc_Absyn_Increment_e=5;struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};static const int Cyc_Absyn_Conditional_e=6;
struct Cyc_Absyn_Conditional_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;struct Cyc_Absyn_Exp*f3;};static const int Cyc_Absyn_SeqExp_e=7;struct Cyc_Absyn_SeqExp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};static const int Cyc_Absyn_UnknownCall_e=
8;struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};static const int Cyc_Absyn_FnCall_e=9;struct Cyc_Absyn_FnCall_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;
};static const int Cyc_Absyn_Throw_e=10;struct Cyc_Absyn_Throw_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_NoInstantiate_e=11;struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Instantiate_e=12;struct
Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};static const int Cyc_Absyn_Cast_e=13;struct Cyc_Absyn_Cast_e_struct{int tag;void*
f1;struct Cyc_Absyn_Exp*f2;};static const int Cyc_Absyn_Address_e=14;struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_New_e=15;struct Cyc_Absyn_New_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};static const int Cyc_Absyn_Sizeoftyp_e=
16;struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};static const int Cyc_Absyn_Sizeofexp_e=
17;struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};static
const int Cyc_Absyn_Offsetof_e=18;struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;
void*f2;};static const int Cyc_Absyn_Gentyp_e=19;struct Cyc_Absyn_Gentyp_e_struct{
int tag;struct Cyc_List_List*f1;void*f2;};static const int Cyc_Absyn_Deref_e=20;
struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_StructMember_e=
21;struct Cyc_Absyn_StructMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_tagged_arr*f2;};static const int Cyc_Absyn_StructArrow_e=22;struct Cyc_Absyn_StructArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};static const int Cyc_Absyn_Subscript_e=
23;struct Cyc_Absyn_Subscript_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};static const int Cyc_Absyn_Tuple_e=24;struct Cyc_Absyn_Tuple_e_struct{int tag;
struct Cyc_List_List*f1;};static const int Cyc_Absyn_CompoundLit_e=25;struct _tuple1{
struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Array_e=
26;struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};static const int
Cyc_Absyn_Comprehension_e=27;struct Cyc_Absyn_Comprehension_e_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
static const int Cyc_Absyn_Struct_e=28;struct Cyc_Absyn_Struct_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};static const int Cyc_Absyn_AnonStruct_e=29;struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Tunion_e=30;
struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};static const int Cyc_Absyn_Enum_e=31;struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
static const int Cyc_Absyn_AnonEnum_e=32;struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static const int Cyc_Absyn_Malloc_e=
33;struct Cyc_Absyn_Malloc_e_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};static
const int Cyc_Absyn_UnresolvedMem_e=34;struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_StmtExp_e=
35;struct Cyc_Absyn_StmtExp_e_struct{int tag;struct Cyc_Absyn_Stmt*f1;};static const
int Cyc_Absyn_Codegen_e=36;struct Cyc_Absyn_Codegen_e_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};static const int Cyc_Absyn_Fill_e=37;struct Cyc_Absyn_Fill_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;
struct Cyc_Position_Segment*loc;void*annot;};struct _tuple2{struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;
struct _tuple2 condition;struct _tuple2 delta;struct Cyc_Absyn_Stmt*body;};static
const int Cyc_Absyn_Skip_s=0;static const int Cyc_Absyn_Exp_s=0;struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Seq_s=1;struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_Return_s=
2;struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};static const int
Cyc_Absyn_IfThenElse_s=3;struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};static const int Cyc_Absyn_While_s=
4;struct Cyc_Absyn_While_s_struct{int tag;struct _tuple2 f1;struct Cyc_Absyn_Stmt*f2;
};static const int Cyc_Absyn_Break_s=5;struct Cyc_Absyn_Break_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};static const int Cyc_Absyn_Continue_s=6;struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};static const int Cyc_Absyn_Goto_s=7;struct Cyc_Absyn_Goto_s_struct{
int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_For_s=
8;struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple2 f2;
struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;};static const int Cyc_Absyn_Switch_s=9;
struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};static const int Cyc_Absyn_SwitchC_s=10;struct Cyc_Absyn_SwitchC_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Fallthru_s=
11;struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};static const int Cyc_Absyn_Decl_s=12;struct Cyc_Absyn_Decl_s_struct{int tag;
struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_Cut_s=
13;struct Cyc_Absyn_Cut_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};static const int
Cyc_Absyn_Splice_s=14;struct Cyc_Absyn_Splice_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};static const int Cyc_Absyn_Label_s=15;struct Cyc_Absyn_Label_s_struct{int tag;
struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_Do_s=16;
struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple2 f2;};
static const int Cyc_Absyn_TryCatch_s=17;struct Cyc_Absyn_TryCatch_s_struct{int tag;
struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Region_s=
18;struct Cyc_Absyn_Region_s_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;struct Cyc_Absyn_Stmt*f3;};static const int Cyc_Absyn_ForArray_s=19;struct Cyc_Absyn_ForArray_s_struct{
int tag;struct Cyc_Absyn_ForArrayInfo f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};static const int
Cyc_Absyn_Wild_p=0;static const int Cyc_Absyn_Var_p=0;struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_Absyn_Null_p=1;static const
int Cyc_Absyn_Int_p=1;struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};static
const int Cyc_Absyn_Char_p=2;struct Cyc_Absyn_Char_p_struct{int tag;unsigned char f1;
};static const int Cyc_Absyn_Float_p=3;struct Cyc_Absyn_Float_p_struct{int tag;struct
_tagged_arr f1;};static const int Cyc_Absyn_Tuple_p=4;struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_Pointer_p=5;struct Cyc_Absyn_Pointer_p_struct{
int tag;struct Cyc_Absyn_Pat*f1;};static const int Cyc_Absyn_Reference_p=6;struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_Absyn_Struct_p=7;struct Cyc_Absyn_Struct_p_struct{
int tag;struct Cyc_Absyn_Structdecl*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;};static const int Cyc_Absyn_Tunion_p=8;struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;struct Cyc_List_List*f4;};static const int Cyc_Absyn_Enum_p=9;struct Cyc_Absyn_Enum_p_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};static const int
Cyc_Absyn_AnonEnum_p=10;struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct
Cyc_Absyn_Enumfield*f2;};static const int Cyc_Absyn_UnknownId_p=11;struct Cyc_Absyn_UnknownId_p_struct{
int tag;struct _tuple0*f1;};static const int Cyc_Absyn_UnknownCall_p=12;struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};static
const int Cyc_Absyn_UnknownFields_p=13;struct Cyc_Absyn_UnknownFields_p_struct{int
tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{
void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*body;
struct Cyc_Position_Segment*loc;};static const int Cyc_Absyn_Unresolved_b=0;static
const int Cyc_Absyn_Global_b=0;struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};static const int Cyc_Absyn_Funname_b=1;struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};static const int Cyc_Absyn_Param_b=2;struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_Absyn_Local_b=3;struct Cyc_Absyn_Local_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_Absyn_Pat_b=4;struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Structfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{
void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_List_List*tvs;void*defn;};static const int Cyc_Absyn_Var_d=0;
struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};static const int
Cyc_Absyn_Fn_d=1;struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};
static const int Cyc_Absyn_Let_d=2;struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*
f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};
static const int Cyc_Absyn_Letv_d=3;struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};static const int Cyc_Absyn_Struct_d=4;struct Cyc_Absyn_Struct_d_struct{int tag;
struct Cyc_Absyn_Structdecl*f1;};static const int Cyc_Absyn_Union_d=5;struct Cyc_Absyn_Union_d_struct{
int tag;struct Cyc_Absyn_Uniondecl*f1;};static const int Cyc_Absyn_Tunion_d=6;struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};static const int
Cyc_Absyn_Enum_d=7;struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};static const int Cyc_Absyn_Typedef_d=8;struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};static const int Cyc_Absyn_Namespace_d=9;struct Cyc_Absyn_Namespace_d_struct{
int tag;struct _tagged_arr*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Using_d=
10;struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;
};static const int Cyc_Absyn_ExternC_d=11;struct Cyc_Absyn_ExternC_d_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};static const int Cyc_Absyn_ArrayElement=0;struct Cyc_Absyn_ArrayElement_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_FieldName=1;struct Cyc_Absyn_FieldName_struct{
int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[15];extern
void*Cyc_Absyn_exn_typ;extern struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*
s,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*
p,struct Cyc_Position_Segment*s);struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
static const int Cyc_Std_String_pa=0;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};static const int Cyc_Std_Int_pa=1;struct Cyc_Std_Int_pa_struct{int
tag;unsigned int f1;};static const int Cyc_Std_Double_pa=2;struct Cyc_Std_Double_pa_struct{
int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=3;struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=4;struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};extern struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);static const int Cyc_Std_ShortPtr_sa=0;struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};static const int Cyc_Std_UShortPtr_sa=1;struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=2;struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=3;struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=4;struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct
Cyc_List_List*curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];
static const int Cyc_Tcenv_VarRes=0;struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};
static const int Cyc_Tcenv_StructRes=1;struct Cyc_Tcenv_StructRes_struct{int tag;
struct Cyc_Absyn_Structdecl*f1;};static const int Cyc_Tcenv_TunionRes=2;struct Cyc_Tcenv_TunionRes_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};static
const int Cyc_Tcenv_EnumRes=3;struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};static const int Cyc_Tcenv_AnonEnumRes=4;struct
Cyc_Tcenv_AnonEnumRes_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*
structdecls;struct Cyc_Dict_Dict*uniondecls;struct Cyc_Dict_Dict*tuniondecls;
struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;static const int
Cyc_Tcenv_NotLoop_j=0;static const int Cyc_Tcenv_CaseEnd_j=1;static const int Cyc_Tcenv_FnEnd_j=
2;static const int Cyc_Tcenv_Stmt_j=0;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};static const int Cyc_Tcenv_Outermost=0;struct Cyc_Tcenv_Outermost_struct{
int tag;void*f1;};static const int Cyc_Tcenv_Frame=1;struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};static const int Cyc_Tcenv_Hidden=2;struct Cyc_Tcenv_Hidden_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*
ae;struct Cyc_Core_Opt*le;};extern void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
extern struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_switch(struct Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,
struct Cyc_Absyn_Switch_clause*clause);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct Cyc_Tcenv_Tenv*,
void*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct Cyc_Tcenv_Tenv*te);
extern int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);extern void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);extern void
Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
extern void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct _tagged_arr*,struct Cyc_Absyn_Stmt**);struct _tuple3{struct Cyc_Absyn_Switch_clause*
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};extern struct _tuple3*Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
extern struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);extern
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct
_tagged_arr*,struct Cyc_Absyn_Stmt*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_named_block(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Tvar*name);extern void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*r);
extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(struct
_tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(void*t);
extern int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
extern int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);extern void Cyc_Tcutil_explain_failure();extern int Cyc_Tcutil_unify(void*,
void*);extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);struct
_tuple4{struct Cyc_Absyn_Tvar*f1;void*f2;};extern struct _tuple4*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);extern void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*
bound_tvars,void*k,int allow_evars,void*);extern int*Cyc_Tcutil_new_tvar_id();
extern int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
extern void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
extern void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
extern void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct
_tagged_arr msg_part);struct _tuple5{struct Cyc_List_List*f1;struct Cyc_List_List*f2;
};extern struct _tuple5 Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p,void**topt,void**region_opt);extern void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);extern void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_List_List*);extern int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Pat*p);extern void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,
struct Cyc_List_List*);static const int Cyc_CfFlowInfo_VarRoot=0;struct Cyc_CfFlowInfo_VarRoot_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_CfFlowInfo_MallocPt=1;
struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};static const int Cyc_CfFlowInfo_NoneIL=0;
static const int Cyc_CfFlowInfo_ThisIL=1;static const int Cyc_CfFlowInfo_AllIL=2;
static const int Cyc_CfFlowInfo_EqualConst=0;struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};static const int Cyc_CfFlowInfo_LessVar=1;struct Cyc_CfFlowInfo_LessVar_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_CfFlowInfo_LessSize=2;
struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};static
const int Cyc_CfFlowInfo_LessConst=3;struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};static const int Cyc_CfFlowInfo_LessEqSize=4;struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};extern unsigned char Cyc_CfFlowInfo_IsZero[11];struct Cyc_CfFlowInfo_IsZero_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_CfFlowInfo_NotZero[
12];struct Cyc_CfFlowInfo_NotZero_struct{unsigned char*tag;struct Cyc_List_List*f1;
};extern unsigned char Cyc_CfFlowInfo_UnknownZ[13];struct Cyc_CfFlowInfo_UnknownZ_struct{
unsigned char*tag;struct Cyc_List_List*f1;};static const int Cyc_CfFlowInfo_PlaceL=0;
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};static
const int Cyc_CfFlowInfo_UnknownL=0;static const int Cyc_CfFlowInfo_Zero=0;static
const int Cyc_CfFlowInfo_NotZeroAll=1;static const int Cyc_CfFlowInfo_NotZeroThis=2;
static const int Cyc_CfFlowInfo_UnknownR=0;struct Cyc_CfFlowInfo_UnknownR_struct{int
tag;void*f1;};static const int Cyc_CfFlowInfo_Esc=1;struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};static const int Cyc_CfFlowInfo_AddressOf=2;struct Cyc_CfFlowInfo_AddressOf_struct{
int tag;struct Cyc_CfFlowInfo_Place*f1;};static const int Cyc_CfFlowInfo_Aggregate=3;
struct Cyc_CfFlowInfo_Aggregate_struct{int tag;struct Cyc_Dict_Dict*f1;};static
const int Cyc_CfFlowInfo_BottomFL=0;static const int Cyc_CfFlowInfo_ReachableFL=0;
struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict*f1;struct Cyc_List_List*
f2;};extern void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,
struct Cyc_Absyn_Stmt*encloser);extern void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s,int new_block);extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s){Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(
te));s->try_depth=Cyc_Tcenv_get_try_depth(te);}static void Cyc_Tcstmt_pattern_synth(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs,
struct Cyc_List_List*vs,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int
new_block){struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(loc,te,tvs);if(
new_block){te2=Cyc_Tcenv_new_block(loc,te2);}{struct Cyc_Core_Opt*_tmp0=({struct
Cyc_Core_Opt*_tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp2->v=(void*)Cyc_Tcenv_curr_rgn(
te2);_tmp2;});{struct Cyc_List_List*_tmp1=vs;for(0;_tmp1 != 0;_tmp1=_tmp1->tl){te2=
Cyc_Tcenv_add_pat_var(loc,te2,(struct Cyc_Absyn_Vardecl*)_tmp1->hd);((struct Cyc_Absyn_Vardecl*)
_tmp1->hd)->rgn=_tmp0;}}if(where_opt != 0){Cyc_Tcexp_tcTest(te2,(struct Cyc_Absyn_Exp*)
_check_null(where_opt),_tag_arr("switch clause guard",sizeof(unsigned char),20));}
if(vs != 0){te2=Cyc_Tcenv_set_encloser(te2,s);}Cyc_Tcstmt_tcStmt(te2,s,0);if(vs != 
0){Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));}}}struct
_tuple6{struct Cyc_Tcenv_Tenv*f1;struct Cyc_List_List*f2;};struct _tuple6 Cyc_Tcstmt_check_forarray_decls(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*decls){({
void*_tmp3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("check_forarray_decls",sizeof(unsigned char),21),_tag_arr(_tmp3,sizeof(
void*),0));});return({struct _tuple6 _tmp4;_tmp4.f1=te;_tmp4.f2=0;_tmp4;});}void
Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
Cyc_Tcstmt_decorate_stmt(te,s0);{void*_tmp5=(void*)s0->r;struct Cyc_Absyn_Exp*
_tmp31;struct Cyc_Absyn_Stmt*_tmp33;struct Cyc_Absyn_Stmt*_tmp35;struct Cyc_Absyn_Exp*
_tmp37;struct Cyc_Absyn_Stmt*_tmp39;struct Cyc_Absyn_Stmt*_tmp3B;struct Cyc_Absyn_Exp*
_tmp3D;struct Cyc_Absyn_Stmt*_tmp3F;struct _tuple2 _tmp41;struct Cyc_Absyn_Stmt*
_tmp43;struct Cyc_Absyn_Exp*_tmp45;struct Cyc_Absyn_Stmt*_tmp47;struct _tuple2
_tmp49;struct Cyc_Absyn_Stmt*_tmp4B;struct Cyc_Absyn_Exp*_tmp4D;struct _tuple2
_tmp4F;struct Cyc_Absyn_Stmt*_tmp51;struct Cyc_Absyn_Exp*_tmp53;struct Cyc_Absyn_Exp*
_tmp55;struct Cyc_Absyn_ForArrayInfo _tmp57;struct Cyc_Absyn_Stmt*_tmp59;struct
_tuple2 _tmp5B;struct Cyc_Absyn_Stmt*_tmp5D;struct Cyc_Absyn_Exp*_tmp5F;struct
_tuple2 _tmp61;struct Cyc_Absyn_Stmt*_tmp63;struct Cyc_Absyn_Exp*_tmp65;struct Cyc_List_List*
_tmp67;struct _tuple2 _tmp69;struct Cyc_Absyn_Stmt*_tmp6B;struct Cyc_Absyn_Exp*
_tmp6D;struct Cyc_Absyn_Stmt*_tmp6F;struct Cyc_Absyn_Stmt*_tmp71;struct Cyc_Absyn_Stmt**
_tmp73;struct Cyc_Absyn_Stmt*_tmp74;struct Cyc_Absyn_Stmt**_tmp76;struct Cyc_Absyn_Stmt*
_tmp77;struct Cyc_Absyn_Stmt**_tmp79;struct _tagged_arr*_tmp7A;struct Cyc_Absyn_Switch_clause**
_tmp7C;struct Cyc_Absyn_Switch_clause***_tmp7E;struct Cyc_List_List*_tmp7F;struct
Cyc_Absyn_Stmt*_tmp81;struct _tagged_arr*_tmp83;struct Cyc_List_List*_tmp85;struct
Cyc_Absyn_Exp*_tmp87;struct Cyc_List_List*_tmp89;struct Cyc_Absyn_Exp*_tmp8B;
struct Cyc_List_List*_tmp8D;struct Cyc_Absyn_Stmt*_tmp8F;struct Cyc_Absyn_Stmt*
_tmp91;struct Cyc_Absyn_Decl*_tmp93;struct Cyc_Absyn_Stmt*_tmp95;struct Cyc_Absyn_Vardecl*
_tmp97;struct Cyc_Absyn_Tvar*_tmp99;struct Cyc_Absyn_Stmt*_tmp9B;struct Cyc_Absyn_Stmt*
_tmp9D;_LL7: if(_tmp5 == (void*)Cyc_Absyn_Skip_s){goto _LL8;}else{goto _LL9;}_LL9:
if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== Cyc_Absyn_Exp_s: 0){_LL50: _tmp31=((
struct Cyc_Absyn_Exp_s_struct*)_tmp5)->f1;goto _LL10;}else{goto _LL11;}_LL11: if((
unsigned int)_tmp5 > 1?*((int*)_tmp5)== Cyc_Absyn_Seq_s: 0){_LL54: _tmp35=((struct
Cyc_Absyn_Seq_s_struct*)_tmp5)->f1;goto _LL52;_LL52: _tmp33=((struct Cyc_Absyn_Seq_s_struct*)
_tmp5)->f2;goto _LL12;}else{goto _LL13;}_LL13: if((unsigned int)_tmp5 > 1?*((int*)
_tmp5)== Cyc_Absyn_Return_s: 0){_LL56: _tmp37=((struct Cyc_Absyn_Return_s_struct*)
_tmp5)->f1;goto _LL14;}else{goto _LL15;}_LL15: if((unsigned int)_tmp5 > 1?*((int*)
_tmp5)== Cyc_Absyn_IfThenElse_s: 0){_LL62: _tmp3D=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp5)->f1;goto _LL60;_LL60: _tmp3B=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp5)->f2;
goto _LL58;_LL58: _tmp39=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp5)->f3;goto
_LL16;}else{goto _LL17;}_LL17: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== Cyc_Absyn_While_s:
0){_LL66: _tmp41=((struct Cyc_Absyn_While_s_struct*)_tmp5)->f1;_LL70: _tmp45=_tmp41.f1;
goto _LL68;_LL68: _tmp43=_tmp41.f2;goto _LL64;_LL64: _tmp3F=((struct Cyc_Absyn_While_s_struct*)
_tmp5)->f2;goto _LL18;}else{goto _LL19;}_LL19: if((unsigned int)_tmp5 > 1?*((int*)
_tmp5)== Cyc_Absyn_For_s: 0){_LL86: _tmp55=((struct Cyc_Absyn_For_s_struct*)_tmp5)->f1;
goto _LL80;_LL80: _tmp4F=((struct Cyc_Absyn_For_s_struct*)_tmp5)->f2;_LL84: _tmp53=
_tmp4F.f1;goto _LL82;_LL82: _tmp51=_tmp4F.f2;goto _LL74;_LL74: _tmp49=((struct Cyc_Absyn_For_s_struct*)
_tmp5)->f3;_LL78: _tmp4D=_tmp49.f1;goto _LL76;_LL76: _tmp4B=_tmp49.f2;goto _LL72;
_LL72: _tmp47=((struct Cyc_Absyn_For_s_struct*)_tmp5)->f4;goto _LL20;}else{goto
_LL21;}_LL21: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== Cyc_Absyn_ForArray_s: 0){
_LL88: _tmp57=((struct Cyc_Absyn_ForArray_s_struct*)_tmp5)->f1;_LL104: _tmp67=
_tmp57.defns;goto _LL98;_LL98: _tmp61=_tmp57.condition;_LL102: _tmp65=_tmp61.f1;
goto _LL100;_LL100: _tmp63=_tmp61.f2;goto _LL92;_LL92: _tmp5B=_tmp57.delta;_LL96:
_tmp5F=_tmp5B.f1;goto _LL94;_LL94: _tmp5D=_tmp5B.f2;goto _LL90;_LL90: _tmp59=_tmp57.body;
goto _LL22;}else{goto _LL23;}_LL23: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== Cyc_Absyn_Do_s:
0){_LL112: _tmp6F=((struct Cyc_Absyn_Do_s_struct*)_tmp5)->f1;goto _LL106;_LL106:
_tmp69=((struct Cyc_Absyn_Do_s_struct*)_tmp5)->f2;_LL110: _tmp6D=_tmp69.f1;goto
_LL108;_LL108: _tmp6B=_tmp69.f2;goto _LL24;}else{goto _LL25;}_LL25: if((unsigned int)
_tmp5 > 1?*((int*)_tmp5)== Cyc_Absyn_Break_s: 0){_LL114: _tmp71=((struct Cyc_Absyn_Break_s_struct*)
_tmp5)->f1;_tmp73=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Break_s_struct*)
_tmp5)->f1;goto _LL26;}else{goto _LL27;}_LL27: if((unsigned int)_tmp5 > 1?*((int*)
_tmp5)== Cyc_Absyn_Continue_s: 0){_LL117: _tmp74=((struct Cyc_Absyn_Continue_s_struct*)
_tmp5)->f1;_tmp76=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Continue_s_struct*)
_tmp5)->f1;goto _LL28;}else{goto _LL29;}_LL29: if((unsigned int)_tmp5 > 1?*((int*)
_tmp5)== Cyc_Absyn_Goto_s: 0){_LL123: _tmp7A=((struct Cyc_Absyn_Goto_s_struct*)
_tmp5)->f1;goto _LL120;_LL120: _tmp77=((struct Cyc_Absyn_Goto_s_struct*)_tmp5)->f2;
_tmp79=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_struct*)_tmp5)->f2;goto
_LL30;}else{goto _LL31;}_LL31: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== Cyc_Absyn_Fallthru_s:
0){_LL128: _tmp7F=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp5)->f1;goto _LL125;
_LL125: _tmp7C=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp5)->f2;_tmp7E=(struct Cyc_Absyn_Switch_clause***)&((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp5)->f2;goto _LL32;}else{goto _LL33;}_LL33:
if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== Cyc_Absyn_Label_s: 0){_LL132: _tmp83=((
struct Cyc_Absyn_Label_s_struct*)_tmp5)->f1;goto _LL130;_LL130: _tmp81=((struct Cyc_Absyn_Label_s_struct*)
_tmp5)->f2;goto _LL34;}else{goto _LL35;}_LL35: if((unsigned int)_tmp5 > 1?*((int*)
_tmp5)== Cyc_Absyn_Switch_s: 0){_LL136: _tmp87=((struct Cyc_Absyn_Switch_s_struct*)
_tmp5)->f1;goto _LL134;_LL134: _tmp85=((struct Cyc_Absyn_Switch_s_struct*)_tmp5)->f2;
goto _LL36;}else{goto _LL37;}_LL37: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== Cyc_Absyn_SwitchC_s:
0){_LL140: _tmp8B=((struct Cyc_Absyn_SwitchC_s_struct*)_tmp5)->f1;goto _LL138;
_LL138: _tmp89=((struct Cyc_Absyn_SwitchC_s_struct*)_tmp5)->f2;goto _LL38;}else{
goto _LL39;}_LL39: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== Cyc_Absyn_TryCatch_s:
0){_LL144: _tmp8F=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp5)->f1;goto _LL142;
_LL142: _tmp8D=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp5)->f2;goto _LL40;}else{
goto _LL41;}_LL41: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== Cyc_Absyn_Decl_s: 0){
_LL148: _tmp93=((struct Cyc_Absyn_Decl_s_struct*)_tmp5)->f1;goto _LL146;_LL146:
_tmp91=((struct Cyc_Absyn_Decl_s_struct*)_tmp5)->f2;goto _LL42;}else{goto _LL43;}
_LL43: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== Cyc_Absyn_Region_s: 0){_LL154:
_tmp99=((struct Cyc_Absyn_Region_s_struct*)_tmp5)->f1;goto _LL152;_LL152: _tmp97=((
struct Cyc_Absyn_Region_s_struct*)_tmp5)->f2;goto _LL150;_LL150: _tmp95=((struct Cyc_Absyn_Region_s_struct*)
_tmp5)->f3;goto _LL44;}else{goto _LL45;}_LL45: if((unsigned int)_tmp5 > 1?*((int*)
_tmp5)== Cyc_Absyn_Cut_s: 0){_LL156: _tmp9B=((struct Cyc_Absyn_Cut_s_struct*)_tmp5)->f1;
goto _LL46;}else{goto _LL47;}_LL47: if((unsigned int)_tmp5 > 1?*((int*)_tmp5)== Cyc_Absyn_Splice_s:
0){_LL158: _tmp9D=((struct Cyc_Absyn_Splice_s_struct*)_tmp5)->f1;goto _LL48;}else{
goto _LL6;}_LL8: return;_LL10: Cyc_Tcexp_tcExp(te,0,_tmp31);return;_LL12: {struct
Cyc_Tcenv_Tenv*_tmp9F=Cyc_Tcenv_set_next(te,(void*)({struct Cyc_Tcenv_Stmt_j_struct*
_tmpAC=_cycalloc(sizeof(struct Cyc_Tcenv_Stmt_j_struct));_tmpAC[0]=({struct Cyc_Tcenv_Stmt_j_struct
_tmpAD;_tmpAD.tag=Cyc_Tcenv_Stmt_j;_tmpAD.f1=_tmp33;_tmpAD;});_tmpAC;}));Cyc_Tcstmt_tcStmt(
_tmp9F,_tmp35,1);while(1){void*_tmpA0=(void*)_tmp33->r;struct Cyc_Absyn_Stmt*
_tmpA6;struct Cyc_Absyn_Stmt*_tmpA8;_LL162: if((unsigned int)_tmpA0 > 1?*((int*)
_tmpA0)== Cyc_Absyn_Seq_s: 0){_LL169: _tmpA8=((struct Cyc_Absyn_Seq_s_struct*)
_tmpA0)->f1;goto _LL167;_LL167: _tmpA6=((struct Cyc_Absyn_Seq_s_struct*)_tmpA0)->f2;
goto _LL163;}else{goto _LL164;}_LL164: goto _LL165;_LL163: Cyc_Tcstmt_decorate_stmt(
te,_tmp33);_tmp9F=Cyc_Tcenv_set_next(te,(void*)({struct Cyc_Tcenv_Stmt_j_struct*
_tmpAA=_cycalloc(sizeof(struct Cyc_Tcenv_Stmt_j_struct));_tmpAA[0]=({struct Cyc_Tcenv_Stmt_j_struct
_tmpAB;_tmpAB.tag=Cyc_Tcenv_Stmt_j;_tmpAB.f1=_tmpA6;_tmpAB;});_tmpAA;}));Cyc_Tcstmt_tcStmt(
_tmp9F,_tmpA8,1);_tmp33=_tmpA6;continue;_LL165: goto seq_end;_LL161:;}seq_end: Cyc_Tcstmt_tcStmt(
te,_tmp33,1);return;}_LL14: {void*t=Cyc_Tcenv_return_typ(te);if(_tmp37 == 0){void*
_tmpAE=Cyc_Tcutil_compress(t);_LL176: if(_tmpAE == (void*)Cyc_Absyn_VoidType){goto
_LL177;}else{goto _LL178;}_LL178: goto _LL179;_LL177: goto _LL175;_LL179:({struct Cyc_Std_String_pa_struct
_tmpB5;_tmpB5.tag=Cyc_Std_String_pa;_tmpB5.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{void*_tmpB4[1]={& _tmpB5};Cyc_Tcutil_terr(s0->loc,_tag_arr("must return a value of type %s",
sizeof(unsigned char),31),_tag_arr(_tmpB4,sizeof(void*),1));}});goto _LL175;
_LL175:;}else{struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp37);
Cyc_Tcexp_tcExp(te,(void**)& t,e);if(! Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_Std_String_pa_struct
_tmpB8;_tmpB8.tag=Cyc_Std_String_pa;_tmpB8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{struct Cyc_Std_String_pa_struct _tmpB7;_tmpB7.tag=Cyc_Std_String_pa;_tmpB7.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);{void*_tmpB6[2]={& _tmpB7,& _tmpB8};Cyc_Tcutil_terr(s0->loc,
_tag_arr("returns value of type %s but requires %s",sizeof(unsigned char),41),
_tag_arr(_tmpB6,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}return;}
_LL16: Cyc_Tcexp_tcTest(te,_tmp3D,_tag_arr("if statement",sizeof(unsigned char),
13));Cyc_Tcstmt_tcStmt(te,_tmp3B,1);Cyc_Tcstmt_tcStmt(te,_tmp39,1);return;_LL18:
Cyc_Tcstmt_decorate_stmt(te,_tmp43);Cyc_Tcexp_tcTest(te,_tmp45,_tag_arr("while loop",
sizeof(unsigned char),11));Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(te,_tmp43),
_tmp3F,1);return;_LL20: Cyc_Tcstmt_decorate_stmt(te,_tmp51);Cyc_Tcstmt_decorate_stmt(
te,_tmp4B);Cyc_Tcexp_tcExp(te,0,_tmp55);Cyc_Tcexp_tcTest(te,_tmp53,_tag_arr("for loop",
sizeof(unsigned char),9));te=Cyc_Tcenv_set_in_loop(te,_tmp4B);Cyc_Tcstmt_tcStmt(
te,_tmp47,1);Cyc_Tcexp_tcExp(te,0,_tmp4D);return;_LL22:({void*_tmpB9[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("ForArray_s",
sizeof(unsigned char),11),_tag_arr(_tmpB9,sizeof(void*),0));});return;_LL24: Cyc_Tcstmt_decorate_stmt(
te,_tmp6B);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(te,_tmp6B),_tmp6F,1);Cyc_Tcexp_tcTest(
te,_tmp6D,_tag_arr("do loop",sizeof(unsigned char),8));return;_LL26: Cyc_Tcenv_process_break(
te,s0,_tmp73);return;_LL28: Cyc_Tcenv_process_continue(te,s0,_tmp76);return;_LL30:
Cyc_Tcenv_process_goto(te,s0,_tmp7A,_tmp79);return;_LL32: {struct _tuple3*_tmpBA=
Cyc_Tcenv_process_fallthru(te,s0,_tmp7E);if(_tmpBA == 0){({void*_tmpBB[0]={};Cyc_Tcutil_terr(
s0->loc,_tag_arr("fallthru not in a non-last case",sizeof(unsigned char),32),
_tag_arr(_tmpBB,sizeof(void*),0));});return;}{struct _tuple3 _tmpBE;struct Cyc_List_List*
_tmpBF;struct Cyc_List_List*_tmpC1;struct _tuple3*_tmpBC=(struct _tuple3*)
_check_null(_tmpBA);_tmpBE=*_tmpBC;_LL194: _tmpC1=_tmpBE.f2;goto _LL192;_LL192:
_tmpBF=_tmpBE.f3;goto _LL189;_LL189: {struct Cyc_List_List*instantiation=((struct
Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,
Cyc_Tcenv_lookup_type_vars(te),_tmpC1);_tmpBF=((struct Cyc_List_List*(*)(void*(*f)(
struct Cyc_List_List*,void*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_substitute,instantiation,_tmpBF);for(0;_tmpBF != 0? _tmp7F != 0: 0;(
_tmpBF=_tmpBF->tl,_tmp7F=_tmp7F->tl)){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)
_tmp7F->hd);if(! Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp7F->hd,(void*)
_tmpBF->hd)){({struct Cyc_Std_String_pa_struct _tmpC5;_tmpC5.tag=Cyc_Std_String_pa;
_tmpC5.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)_tmpBF->hd);{struct
Cyc_Std_String_pa_struct _tmpC4;_tmpC4.tag=Cyc_Std_String_pa;_tmpC4.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)_tmp7F->hd)->topt))->v);{void*_tmpC3[2]={& _tmpC4,& _tmpC5};
Cyc_Tcutil_terr(s0->loc,_tag_arr("fallthru argument has type %s but pattern variable has type %s",
sizeof(unsigned char),63),_tag_arr(_tmpC3,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
if(_tmp7F != 0){({void*_tmpC6[0]={};Cyc_Tcutil_terr(s0->loc,_tag_arr("too many arguments to explicit fallthru",
sizeof(unsigned char),40),_tag_arr(_tmpC6,sizeof(void*),0));});}if(_tmpBF != 0){({
void*_tmpC7[0]={};Cyc_Tcutil_terr(s0->loc,_tag_arr("too few arguments to explicit fallthru",
sizeof(unsigned char),39),_tag_arr(_tmpC7,sizeof(void*),0));});}return;}}}_LL34:
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(s0->loc,Cyc_Tcenv_add_label(te,
_tmp83,_tmp81),({struct Cyc_Absyn_Tvar*_tmpC8=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_tmpC8->name=({struct _tagged_arr*_tmpCB=_cycalloc(sizeof(struct _tagged_arr));
_tmpCB[0]=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpCD;_tmpCD.tag=
Cyc_Std_String_pa;_tmpCD.f1=(struct _tagged_arr)*_tmp83;{void*_tmpCC[1]={& _tmpCD};
Cyc_Std_aprintf(_tag_arr("`%s",sizeof(unsigned char),4),_tag_arr(_tmpCC,sizeof(
void*),1));}});_tmpCB;});_tmpC8->identity=Cyc_Tcutil_new_tvar_id();_tmpC8->kind=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmpC9=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));
_tmpC9[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpCA;_tmpCA.tag=Cyc_Absyn_Eq_kb;_tmpCA.f1=(
void*)((void*)Cyc_Absyn_RgnKind);_tmpCA;});_tmpC9;}));_tmpC8;})),_tmp81,0);
return;_LL36: Cyc_Tcexp_tcExp(te,0,_tmp87);{void*_tmpCE=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp87->topt))->v;te=Cyc_Tcenv_set_in_switch(te);te=Cyc_Tcenv_clear_fallthru(
te);{struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp85);for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Pat*_tmpCF=((
struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct Cyc_List_List*_tmpD2;
struct Cyc_List_List*_tmpD4;struct _tuple5 _tmpD0=Cyc_Tcpat_tcPat(te,_tmpCF,(void**)&
_tmpCE,0);_LL213: _tmpD4=_tmpD0.f1;goto _LL211;_LL211: _tmpD2=_tmpD0.f2;goto _LL209;
_LL209: if(! Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmpCF->topt))->v,
_tmpCE)){({struct Cyc_Std_String_pa_struct _tmpD8;_tmpD8.tag=Cyc_Std_String_pa;
_tmpD8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpCF->topt))->v);{struct Cyc_Std_String_pa_struct _tmpD7;_tmpD7.tag=
Cyc_Std_String_pa;_tmpD7.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmpCE);{
void*_tmpD6[2]={& _tmpD7,& _tmpD8};Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,_tag_arr("switch on type %s, but case expects type %s",sizeof(
unsigned char),44),_tag_arr(_tmpD6,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
else{Cyc_Tcpat_check_pat_regions(te,_tmpCF);}((struct Cyc_Absyn_Switch_clause*)
scs->hd)->pat_vars=({struct Cyc_Core_Opt*_tmpD9=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmpD9->v=_tmpD2;_tmpD9;});Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,te,_tmpD4,_tmpD2,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((
struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);te=Cyc_Tcenv_set_fallthru(
te,_tmpD4,_tmpD2,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}Cyc_Tcpat_check_switch_exhaustive(
s0->loc,_tmp85);return;}_LL38: {struct Cyc_List_List*_tmpDA=0;for(0;_tmp89 != 0;
_tmp89=_tmp89->tl){struct Cyc_Absyn_SwitchC_clause _tmpDD;struct Cyc_Position_Segment*
_tmpDE;struct Cyc_Absyn_Stmt*_tmpE0;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_SwitchC_clause*
_tmpDB=(struct Cyc_Absyn_SwitchC_clause*)_tmp89->hd;_tmpDD=*_tmpDB;_LL227: _tmpE2=
_tmpDD.cnst_exp;goto _LL225;_LL225: _tmpE0=_tmpDD.body;goto _LL223;_LL223: _tmpDE=
_tmpDD.loc;goto _LL220;_LL220: {struct Cyc_Absyn_Pat*pat;if(_tmpE2 != 0){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmpE2));pat=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Int_p_struct*_tmpE4=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_struct));
_tmpE4[0]=({struct Cyc_Absyn_Int_p_struct _tmpE5;_tmpE5.tag=Cyc_Absyn_Int_p;_tmpE5.f1=(
void*)((void*)Cyc_Absyn_Signed);_tmpE5.f2=(int)Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmpE2));_tmpE5;});_tmpE4;}),_tmpE2->loc);}
else{pat=Cyc_Absyn_new_pat((void*)Cyc_Absyn_Wild_p,_tmpDE);}_tmpDA=({struct Cyc_List_List*
_tmpE6=_cycalloc(sizeof(struct Cyc_List_List));_tmpE6->hd=({struct Cyc_Absyn_Switch_clause*
_tmpE7=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_tmpE7->pattern=pat;
_tmpE7->pat_vars=0;_tmpE7->where_clause=0;_tmpE7->body=_tmpE0;_tmpE7->loc=_tmpDE;
_tmpE7;});_tmpE6->tl=_tmpDA;_tmpE6;});}}(void*)(s0->r=(void*)((void*)(Cyc_Absyn_switch_stmt(
_tmp8B,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpDA),
0))->r));Cyc_Tcstmt_tcStmt(te,s0,new_block);return;}_LL40:(void*)(_tmp8F->r=(
void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Absyn_new_stmt((void*)_tmp8F->r,_tmp8F->loc),
Cyc_Absyn_skip_stmt(_tmp8F->loc),_tmp8F->loc))->r));Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
Cyc_Tcenv_enter_try(te),s0),_tmp8F,1);te=Cyc_Tcenv_set_in_switch(te);te=Cyc_Tcenv_clear_fallthru(
te);{struct Cyc_List_List*_tmpE8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp8D);for(0;_tmpE8 != 0;_tmpE8=_tmpE8->tl){struct Cyc_Absyn_Pat*
_tmpE9=((struct Cyc_Absyn_Switch_clause*)_tmpE8->hd)->pattern;struct Cyc_List_List*
_tmpEC;struct Cyc_List_List*_tmpEE;struct _tuple5 _tmpEA=Cyc_Tcpat_tcPat(te,_tmpE9,(
void**)& Cyc_Absyn_exn_typ,0);_LL239: _tmpEE=_tmpEA.f1;goto _LL237;_LL237: _tmpEC=
_tmpEA.f2;goto _LL235;_LL235: if(! Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpE9->topt))->v,Cyc_Absyn_exn_typ)){({struct Cyc_Std_String_pa_struct
_tmpF1;_tmpF1.tag=Cyc_Std_String_pa;_tmpF1.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpE9->topt))->v);{void*_tmpF0[1]={&
_tmpF1};Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmpE8->hd)->loc,
_tag_arr("expected xtunion exn but found %s",sizeof(unsigned char),34),_tag_arr(
_tmpF0,sizeof(void*),1));}});}else{Cyc_Tcpat_check_pat_regions(te,_tmpE9);}((
struct Cyc_Absyn_Switch_clause*)_tmpE8->hd)->pat_vars=({struct Cyc_Core_Opt*_tmpF2=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpF2->v=_tmpEC;_tmpF2;});Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)_tmpE8->hd)->loc,te,_tmpEE,_tmpEC,((struct Cyc_Absyn_Switch_clause*)
_tmpE8->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmpE8->hd)->where_clause,1);
te=Cyc_Tcenv_set_fallthru(te,_tmpEE,_tmpEC,(struct Cyc_Absyn_Switch_clause*)
_tmpE8->hd);}}Cyc_Tcpat_check_catch_overlap(s0->loc,_tmp8D);return;_LL42: {
struct _tagged_arr unimp_msg_part;if(new_block){te=Cyc_Tcenv_new_block(s0->loc,te);}{
void*_tmpF3=(void*)_tmp93->r;struct Cyc_Absyn_Vardecl*_tmp10D;int _tmp10F;int*
_tmp111;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Core_Opt*_tmp114;struct Cyc_Core_Opt**
_tmp116;struct Cyc_Core_Opt*_tmp117;struct Cyc_Core_Opt**_tmp119;struct Cyc_Absyn_Pat*
_tmp11A;struct Cyc_List_List*_tmp11C;struct Cyc_List_List*_tmp11E;struct
_tagged_arr*_tmp120;struct Cyc_List_List*_tmp122;struct _tuple0*_tmp124;_LL245: if(*((
int*)_tmpF3)== Cyc_Absyn_Var_d){_LL270: _tmp10D=((struct Cyc_Absyn_Var_d_struct*)
_tmpF3)->f1;goto _LL246;}else{goto _LL247;}_LL247: if(*((int*)_tmpF3)== Cyc_Absyn_Let_d){
_LL283: _tmp11A=((struct Cyc_Absyn_Let_d_struct*)_tmpF3)->f1;goto _LL280;_LL280:
_tmp117=((struct Cyc_Absyn_Let_d_struct*)_tmpF3)->f2;_tmp119=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Let_d_struct*)_tmpF3)->f2;goto _LL277;_LL277: _tmp114=((struct Cyc_Absyn_Let_d_struct*)
_tmpF3)->f3;_tmp116=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_struct*)
_tmpF3)->f3;goto _LL275;_LL275: _tmp112=((struct Cyc_Absyn_Let_d_struct*)_tmpF3)->f4;
goto _LL272;_LL272: _tmp10F=((struct Cyc_Absyn_Let_d_struct*)_tmpF3)->f5;_tmp111=(
int*)&((struct Cyc_Absyn_Let_d_struct*)_tmpF3)->f5;goto _LL248;}else{goto _LL249;}
_LL249: if(*((int*)_tmpF3)== Cyc_Absyn_Letv_d){_LL285: _tmp11C=((struct Cyc_Absyn_Letv_d_struct*)
_tmpF3)->f1;goto _LL250;}else{goto _LL251;}_LL251: if(*((int*)_tmpF3)== Cyc_Absyn_Namespace_d){
_LL289: _tmp120=((struct Cyc_Absyn_Namespace_d_struct*)_tmpF3)->f1;goto _LL287;
_LL287: _tmp11E=((struct Cyc_Absyn_Namespace_d_struct*)_tmpF3)->f2;goto _LL252;}
else{goto _LL253;}_LL253: if(*((int*)_tmpF3)== Cyc_Absyn_Using_d){_LL293: _tmp124=((
struct Cyc_Absyn_Using_d_struct*)_tmpF3)->f1;goto _LL291;_LL291: _tmp122=((struct
Cyc_Absyn_Using_d_struct*)_tmpF3)->f2;goto _LL254;}else{goto _LL255;}_LL255: if(*((
int*)_tmpF3)== Cyc_Absyn_Fn_d){goto _LL256;}else{goto _LL257;}_LL257: if(*((int*)
_tmpF3)== Cyc_Absyn_Struct_d){goto _LL258;}else{goto _LL259;}_LL259: if(*((int*)
_tmpF3)== Cyc_Absyn_Union_d){goto _LL260;}else{goto _LL261;}_LL261: if(*((int*)
_tmpF3)== Cyc_Absyn_Tunion_d){goto _LL262;}else{goto _LL263;}_LL263: if(*((int*)
_tmpF3)== Cyc_Absyn_Enum_d){goto _LL264;}else{goto _LL265;}_LL265: if(*((int*)
_tmpF3)== Cyc_Absyn_Typedef_d){goto _LL266;}else{goto _LL267;}_LL267: if(*((int*)
_tmpF3)== Cyc_Absyn_ExternC_d){goto _LL268;}else{goto _LL244;}_LL246: {struct Cyc_Absyn_Vardecl
_tmp128;struct Cyc_List_List*_tmp129;struct Cyc_Core_Opt*_tmp12B;struct Cyc_Core_Opt**
_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;void*_tmp130;struct Cyc_Absyn_Tqual _tmp132;
struct _tuple0*_tmp134;struct _tuple0 _tmp136;struct _tagged_arr*_tmp137;void*
_tmp139;void*_tmp13B;struct Cyc_Absyn_Vardecl*_tmp126=_tmp10D;_tmp128=*_tmp126;
_LL316: _tmp13B=(void*)_tmp128.sc;goto _LL309;_LL309: _tmp134=_tmp128.name;_tmp136=*
_tmp134;_LL314: _tmp139=_tmp136.f1;goto _LL312;_LL312: _tmp137=_tmp136.f2;goto
_LL307;_LL307: _tmp132=_tmp128.tq;goto _LL305;_LL305: _tmp130=(void*)_tmp128.type;
goto _LL303;_LL303: _tmp12E=_tmp128.initializer;goto _LL300;_LL300: _tmp12B=_tmp128.rgn;
_tmp12D=(struct Cyc_Core_Opt**)&(*_tmp126).rgn;goto _LL298;_LL298: _tmp129=_tmp128.attributes;
goto _LL295;_LL295: {void*_tmp13D=Cyc_Tcenv_curr_rgn(te);int is_local;{void*
_tmp13E=_tmp13B;_LL320: if(_tmp13E == (void*)Cyc_Absyn_Static){goto _LL321;}else{
goto _LL322;}_LL322: if(_tmp13E == (void*)Cyc_Absyn_Extern){goto _LL323;}else{goto
_LL324;}_LL324: if(_tmp13E == (void*)Cyc_Absyn_ExternC){goto _LL325;}else{goto
_LL326;}_LL326: if(_tmp13E == (void*)Cyc_Absyn_Abstract){goto _LL327;}else{goto
_LL328;}_LL328: if(_tmp13E == (void*)Cyc_Absyn_Public){goto _LL329;}else{goto _LL319;}
_LL321: goto _LL323;_LL323: goto _LL325;_LL325: is_local=0;goto _LL319;_LL327:({void*
_tmp14A[0]={};Cyc_Tcutil_terr(_tmp93->loc,_tag_arr("bad abstract scope for local variable",
sizeof(unsigned char),38),_tag_arr(_tmp14A,sizeof(void*),0));});goto _LL329;
_LL329: is_local=1;goto _LL319;_LL319:;}*_tmp12D=is_local?({struct Cyc_Core_Opt*
_tmp14B=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp14B->v=(void*)_tmp13D;_tmp14B;}):({
struct Cyc_Core_Opt*_tmp14C=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp14C->v=(
void*)((void*)Cyc_Absyn_HeapRgn);_tmp14C;});{void*_tmp14D=_tmp139;struct Cyc_List_List*
_tmp157;struct Cyc_List_List*_tmp159;_LL335: if(_tmp14D == (void*)Cyc_Absyn_Loc_n){
goto _LL336;}else{goto _LL337;}_LL337: if((unsigned int)_tmp14D > 1?*((int*)_tmp14D)
== Cyc_Absyn_Rel_n: 0){_LL344: _tmp157=((struct Cyc_Absyn_Rel_n_struct*)_tmp14D)->f1;
if(_tmp157 == 0){goto _LL338;}else{goto _LL339;}}else{goto _LL339;}_LL339: if((
unsigned int)_tmp14D > 1?*((int*)_tmp14D)== Cyc_Absyn_Abs_n: 0){_LL346: _tmp159=((
struct Cyc_Absyn_Abs_n_struct*)_tmp14D)->f1;goto _LL340;}else{goto _LL341;}_LL341:
goto _LL342;_LL336: goto _LL334;_LL338:(*_tmp10D->name).f1=(void*)Cyc_Absyn_Loc_n;
goto _LL334;_LL340:(int)_throw(({void*_tmp15B[0]={};Cyc_Tcutil_impos(_tag_arr("tcstmt: Abs_n declaration",
sizeof(unsigned char),26),_tag_arr(_tmp15B,sizeof(void*),0));}));_LL342:({void*
_tmp15C[0]={};Cyc_Tcutil_terr(_tmp93->loc,_tag_arr("cannot declare a qualified local variable",
sizeof(unsigned char),42),_tag_arr(_tmp15C,sizeof(void*),0));});goto _LL334;
_LL334:;}{void*_tmp15D=Cyc_Tcutil_compress(_tmp130);struct Cyc_Absyn_Exp*_tmp164;
struct Cyc_Absyn_Tqual _tmp166;void*_tmp168;_LL351: if((unsigned int)_tmp15D > 3?*((
int*)_tmp15D)== Cyc_Absyn_ArrayType: 0){_LL361: _tmp168=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp15D)->f1;goto _LL359;_LL359: _tmp166=((struct Cyc_Absyn_ArrayType_struct*)
_tmp15D)->f2;goto _LL357;_LL357: _tmp164=((struct Cyc_Absyn_ArrayType_struct*)
_tmp15D)->f3;if(_tmp164 == 0){goto _LL355;}else{goto _LL353;}}else{goto _LL353;}
_LL355: if(_tmp10D->initializer != 0){goto _LL352;}else{goto _LL353;}_LL353: goto
_LL354;_LL352:{void*_tmp16A=(void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp10D->initializer))->r;
void*_tmp176;struct _tagged_arr _tmp178;struct Cyc_Absyn_Exp*_tmp17A;struct Cyc_List_List*
_tmp17C;struct Cyc_List_List*_tmp17E;_LL364: if(*((int*)_tmp16A)== Cyc_Absyn_Const_e){
_LL375: _tmp176=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp16A)->f1;if((
unsigned int)_tmp176 > 1?*((int*)_tmp176)== Cyc_Absyn_String_c: 0){_LL377: _tmp178=((
struct Cyc_Absyn_String_c_struct*)_tmp176)->f1;goto _LL365;}else{goto _LL366;}}
else{goto _LL366;}_LL366: if(*((int*)_tmp16A)== Cyc_Absyn_Comprehension_e){_LL379:
_tmp17A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp16A)->f2;goto _LL367;}else{
goto _LL368;}_LL368: if(*((int*)_tmp16A)== Cyc_Absyn_UnresolvedMem_e){_LL381:
_tmp17C=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp16A)->f2;goto _LL369;}else{
goto _LL370;}_LL370: if(*((int*)_tmp16A)== Cyc_Absyn_Array_e){_LL383: _tmp17E=((
struct Cyc_Absyn_Array_e_struct*)_tmp16A)->f1;goto _LL371;}else{goto _LL372;}_LL372:
goto _LL373;_LL365: _tmp130=(void*)(_tmp10D->type=(void*)((void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp180=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp180[0]=({struct
Cyc_Absyn_ArrayType_struct _tmp181;_tmp181.tag=Cyc_Absyn_ArrayType;_tmp181.f1=(
void*)_tmp168;_tmp181.f2=_tmp166;_tmp181.f3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
_get_arr_size(_tmp178,sizeof(unsigned char)),0);_tmp181;});_tmp180;})));goto
_LL363;_LL367: _tmp130=(void*)(_tmp10D->type=(void*)((void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp182=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp182[0]=({struct
Cyc_Absyn_ArrayType_struct _tmp183;_tmp183.tag=Cyc_Absyn_ArrayType;_tmp183.f1=(
void*)_tmp168;_tmp183.f2=_tmp166;_tmp183.f3=(struct Cyc_Absyn_Exp*)_tmp17A;
_tmp183;});_tmp182;})));goto _LL363;_LL369: _tmp17E=_tmp17C;goto _LL371;_LL371:
_tmp130=(void*)(_tmp10D->type=(void*)((void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp184=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp184[0]=({struct
Cyc_Absyn_ArrayType_struct _tmp185;_tmp185.tag=Cyc_Absyn_ArrayType;_tmp185.f1=(
void*)_tmp168;_tmp185.f2=_tmp166;_tmp185.f3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp17E),0);
_tmp185;});_tmp184;})));goto _LL363;_LL373: goto _LL363;_LL363:;}goto _LL350;_LL354:
goto _LL350;_LL350:;}{struct Cyc_List_List*_tmp186=! is_local? 0: Cyc_Tcenv_lookup_type_vars(
te);int _tmp187=! is_local? 0: 1;Cyc_Tcutil_check_type(s0->loc,te,_tmp186,(void*)Cyc_Absyn_MemKind,
_tmp187,_tmp130);{struct Cyc_Tcenv_Tenv*_tmp188=Cyc_Tcenv_add_local_var(_tmp91->loc,
te,_tmp10D);if(_tmp13B == (void*)Cyc_Absyn_Extern? 1: _tmp13B == (void*)Cyc_Absyn_ExternC){({
void*_tmp189[0]={};Cyc_Tcutil_terr(_tmp93->loc,_tag_arr("local extern declarations not yet supported",
sizeof(unsigned char),44),_tag_arr(_tmp189,sizeof(void*),0));});}if(_tmp12E != 0){
Cyc_Tcexp_tcExpInitializer(_tmp188,(void**)& _tmp130,(struct Cyc_Absyn_Exp*)
_check_null(_tmp12E));if(! is_local? ! Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp12E)): 0){({void*_tmp18A[0]={};Cyc_Tcutil_terr(_tmp93->loc,
_tag_arr("initializer needs to be a constant expression",sizeof(unsigned char),
46),_tag_arr(_tmp18A,sizeof(void*),0));});}if(! Cyc_Tcutil_coerce_assign(_tmp188,(
struct Cyc_Absyn_Exp*)_check_null(_tmp12E),_tmp130)){({struct Cyc_Std_String_pa_struct
_tmp18E;_tmp18E.tag=Cyc_Std_String_pa;_tmp18E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp12E->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp18D;_tmp18D.tag=Cyc_Std_String_pa;_tmp18D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp130);{struct Cyc_Std_String_pa_struct _tmp18C;_tmp18C.tag=Cyc_Std_String_pa;
_tmp18C.f1=(struct _tagged_arr)*_tmp137;{void*_tmp18B[3]={& _tmp18C,& _tmp18D,&
_tmp18E};Cyc_Tcutil_terr(_tmp93->loc,_tag_arr("%s declared with type \n%s\n but initialized with type \n%s",
sizeof(unsigned char),57),_tag_arr(_tmp18B,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmp188,s0),_tmp91,0);return;}}}}_LL248: {
struct Cyc_Core_Opt*_tmp18F=*_tmp116;Cyc_Tcexp_tcExpInitializer(te,0,_tmp112);*
_tmp116=_tmp112->topt;{struct Cyc_List_List*_tmp192;struct Cyc_List_List*_tmp194;
struct _tuple5 _tmp190=Cyc_Tcpat_tcPat(te,_tmp11A,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(_tmp112->topt))->v)),0);_LL405: _tmp194=_tmp190.f1;
goto _LL403;_LL403: _tmp192=_tmp190.f2;goto _LL401;_LL401:*_tmp119=({struct Cyc_Core_Opt*
_tmp196=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp196->v=_tmp192;_tmp196;});if(
_tmp18F != 0? ! Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp11A->topt))->v,(
void*)_tmp18F->v): 0){({void*_tmp197[0]={};Cyc_Tcutil_terr(_tmp93->loc,_tag_arr("type of pattern does not match declared type",
sizeof(unsigned char),45),_tag_arr(_tmp197,sizeof(void*),0));});}if(! Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp11A->topt))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp112->topt))->v)){({struct Cyc_Std_String_pa_struct _tmp19A;_tmp19A.tag=
Cyc_Std_String_pa;_tmp19A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp112->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp199;_tmp199.tag=Cyc_Std_String_pa;_tmp199.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp11A->topt))->v);{void*_tmp198[2]={&
_tmp199,& _tmp19A};Cyc_Tcutil_terr(_tmp93->loc,_tag_arr("pattern type %s does not match definition type %s",
sizeof(unsigned char),50),_tag_arr(_tmp198,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
else{Cyc_Tcpat_check_pat_regions(te,_tmp11A);}*_tmp111=Cyc_Tcpat_check_let_pat_exhaustive(
_tmp11A->loc,_tmp11A);Cyc_Tcstmt_pattern_synth(s0->loc,te,_tmp194,_tmp192,_tmp91,
0,0);return;}}_LL250: {void*_tmp19B=Cyc_Tcenv_curr_rgn(te);struct Cyc_Tcenv_Tenv*
_tmp19C=te;for(0;_tmp11C != 0;_tmp11C=_tmp11C->tl){struct Cyc_Absyn_Vardecl*
_tmp19D=(struct Cyc_Absyn_Vardecl*)_tmp11C->hd;struct Cyc_Absyn_Vardecl _tmp1A0;
struct Cyc_Core_Opt*_tmp1A1;struct Cyc_Core_Opt**_tmp1A3;void*_tmp1A4;struct
_tuple0*_tmp1A6;struct _tuple0 _tmp1A8;void*_tmp1A9;struct Cyc_Absyn_Vardecl*
_tmp19E=_tmp19D;_tmp1A0=*_tmp19E;_LL423: _tmp1A6=_tmp1A0.name;_tmp1A8=*_tmp1A6;
_LL426: _tmp1A9=_tmp1A8.f1;goto _LL421;_LL421: _tmp1A4=(void*)_tmp1A0.type;goto
_LL418;_LL418: _tmp1A1=_tmp1A0.rgn;_tmp1A3=(struct Cyc_Core_Opt**)&(*_tmp19E).rgn;
goto _LL415;_LL415:*_tmp1A3=({struct Cyc_Core_Opt*_tmp1AB=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmp1AB->v=(void*)_tmp19B;_tmp1AB;});{void*_tmp1AC=_tmp1A9;struct
Cyc_List_List*_tmp1B6;struct Cyc_List_List*_tmp1B8;_LL430: if(_tmp1AC == (void*)Cyc_Absyn_Loc_n){
goto _LL431;}else{goto _LL432;}_LL432: if((unsigned int)_tmp1AC > 1?*((int*)_tmp1AC)
== Cyc_Absyn_Rel_n: 0){_LL439: _tmp1B6=((struct Cyc_Absyn_Rel_n_struct*)_tmp1AC)->f1;
if(_tmp1B6 == 0){goto _LL433;}else{goto _LL434;}}else{goto _LL434;}_LL434: if((
unsigned int)_tmp1AC > 1?*((int*)_tmp1AC)== Cyc_Absyn_Abs_n: 0){_LL441: _tmp1B8=((
struct Cyc_Absyn_Abs_n_struct*)_tmp1AC)->f1;goto _LL435;}else{goto _LL436;}_LL436:
goto _LL437;_LL431: goto _LL429;_LL433:(*_tmp19D->name).f1=(void*)Cyc_Absyn_Loc_n;
goto _LL429;_LL435:(int)_throw(({void*_tmp1BA[0]={};Cyc_Tcutil_impos(_tag_arr("tcstmt: Abs_n declaration",
sizeof(unsigned char),26),_tag_arr(_tmp1BA,sizeof(void*),0));}));_LL437:({void*
_tmp1BB[0]={};Cyc_Tcutil_terr(_tmp93->loc,_tag_arr("cannot declare a qualified local variable",
sizeof(unsigned char),42),_tag_arr(_tmp1BB,sizeof(void*),0));});goto _LL429;
_LL429:;}Cyc_Tcutil_check_type(s0->loc,_tmp19C,Cyc_Tcenv_lookup_type_vars(
_tmp19C),(void*)Cyc_Absyn_MemKind,1,_tmp1A4);_tmp19C=Cyc_Tcenv_add_local_var(
_tmp91->loc,_tmp19C,_tmp19D);}Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(_tmp19C,
s0),_tmp91,0);return;}_LL252: unimp_msg_part=_tag_arr("namespace",sizeof(
unsigned char),10);goto _LL244;_LL254: unimp_msg_part=_tag_arr("using",sizeof(
unsigned char),6);goto _LL244;_LL256: unimp_msg_part=_tag_arr("function",sizeof(
unsigned char),9);goto _LL244;_LL258: unimp_msg_part=_tag_arr("struct",sizeof(
unsigned char),7);goto _LL244;_LL260: unimp_msg_part=_tag_arr("union",sizeof(
unsigned char),6);goto _LL244;_LL262: unimp_msg_part=_tag_arr("[x]tunion",sizeof(
unsigned char),10);goto _LL244;_LL264: unimp_msg_part=_tag_arr("enum",sizeof(
unsigned char),5);goto _LL244;_LL266: unimp_msg_part=_tag_arr("typedef",sizeof(
unsigned char),8);goto _LL244;_LL268: unimp_msg_part=_tag_arr("extern \"C\"",
sizeof(unsigned char),11);goto _LL244;_LL244:;}(int)_throw(({struct Cyc_Std_String_pa_struct
_tmp1BD;_tmp1BD.tag=Cyc_Std_String_pa;_tmp1BD.f1=(struct _tagged_arr)
unimp_msg_part;{void*_tmp1BC[1]={& _tmp1BD};Cyc_Tcutil_impos(_tag_arr("tcStmt: nested %s declarations unimplemented",
sizeof(unsigned char),45),_tag_arr(_tmp1BC,sizeof(void*),1));}}));}_LL44:(void*)(
_tmp95->r=(void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Absyn_new_stmt((void*)_tmp95->r,
_tmp95->loc),Cyc_Absyn_skip_stmt(_tmp95->loc),_tmp95->loc))->r));if(new_block){
te=Cyc_Tcenv_new_block(s0->loc,te);}{struct Cyc_Absyn_Vardecl _tmp1C0;struct Cyc_Core_Opt*
_tmp1C1;struct Cyc_Core_Opt**_tmp1C3;void*_tmp1C4;struct Cyc_Absyn_Vardecl*_tmp1BE=
_tmp97;_tmp1C0=*_tmp1BE;_LL453: _tmp1C4=(void*)_tmp1C0.type;goto _LL450;_LL450:
_tmp1C1=_tmp1C0.rgn;_tmp1C3=(struct Cyc_Core_Opt**)&(*_tmp1BE).rgn;goto _LL447;
_LL447: {void*_tmp1C6=Cyc_Tcenv_curr_rgn(te);void*rgn_typ=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp1CC=_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));_tmp1CC[0]=({struct Cyc_Absyn_VarType_struct
_tmp1CD;_tmp1CD.tag=Cyc_Absyn_VarType;_tmp1CD.f1=_tmp99;_tmp1CD;});_tmp1CC;});*
_tmp1C3=({struct Cyc_Core_Opt*_tmp1C7=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp1C7->v=(void*)_tmp1C6;_tmp1C7;});te=Cyc_Tcenv_add_type_vars(s0->loc,te,({
struct Cyc_List_List*_tmp1C8=_cycalloc(sizeof(struct Cyc_List_List));_tmp1C8->hd=
_tmp99;_tmp1C8->tl=0;_tmp1C8;}));Cyc_Tcutil_check_type(s0->loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)Cyc_Absyn_BoxKind,1,_tmp1C4);te=Cyc_Tcenv_add_region(te,rgn_typ);if(!
Cyc_Tcutil_unify(_tmp1C4,(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp1C9=
_cycalloc(sizeof(struct Cyc_Absyn_RgnHandleType_struct));_tmp1C9[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp1CA;_tmp1CA.tag=Cyc_Absyn_RgnHandleType;_tmp1CA.f1=(void*)rgn_typ;_tmp1CA;});
_tmp1C9;}))){({void*_tmp1CB[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("region stmt: type of region handle is wrong!",
sizeof(unsigned char),45),_tag_arr(_tmp1CB,sizeof(void*),0));});}Cyc_Tcstmt_tcStmt(
Cyc_Tcenv_set_encloser(Cyc_Tcenv_add_local_var(_tmp95->loc,Cyc_Tcenv_enter_try(
te),_tmp97),s0),_tmp95,0);return;}}_LL46:(int)_throw(({void*_tmp1CE[0]={};Cyc_Tcutil_impos(
_tag_arr("tcStmt: cut is not implemented",sizeof(unsigned char),31),_tag_arr(
_tmp1CE,sizeof(void*),0));}));_LL48:(int)_throw(({void*_tmp1CF[0]={};Cyc_Tcutil_impos(
_tag_arr("tcStmt: splice is not implemented",sizeof(unsigned char),34),_tag_arr(
_tmp1CF,sizeof(void*),0));}));_LL6:;}}
