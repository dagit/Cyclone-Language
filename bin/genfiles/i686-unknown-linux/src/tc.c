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
struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[18];extern
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern unsigned char Cyc_List_Nth[8];extern int Cyc_List_mem(int(*compare)(void*,
void*),struct Cyc_List_List*l,void*x);extern struct Cyc_List_List*Cyc_List_filter_c(
int(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);extern int Cyc_Std_strptrcmp(struct
_tagged_arr*s1,struct _tagged_arr*s2);extern int Cyc_Std_zstrptrcmp(struct
_tagged_arr*,struct _tagged_arr*);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;static const int Cyc_Position_Lex=0;static const int Cyc_Position_Parse=
1;static const int Cyc_Position_Elab=2;struct Cyc_Position_Error{struct _tagged_arr
source;struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
unsigned char Cyc_Position_Nocontext[14];static const int Cyc_Absyn_Loc_n=0;static
const int Cyc_Absyn_Rel_n=0;struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
f1;};static const int Cyc_Absyn_Abs_n=1;struct Cyc_Absyn_Abs_n_struct{int tag;struct
Cyc_List_List*f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;
static const int Cyc_Absyn_Static=0;static const int Cyc_Absyn_Abstract=1;static const
int Cyc_Absyn_Public=2;static const int Cyc_Absyn_Extern=3;static const int Cyc_Absyn_ExternC=
4;struct Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int q_restrict: 1;};static
const int Cyc_Absyn_B1=0;static const int Cyc_Absyn_B2=1;static const int Cyc_Absyn_B4=2;
static const int Cyc_Absyn_B8=3;static const int Cyc_Absyn_AnyKind=0;static const int Cyc_Absyn_MemKind=
1;static const int Cyc_Absyn_BoxKind=2;static const int Cyc_Absyn_RgnKind=3;static
const int Cyc_Absyn_EffKind=4;static const int Cyc_Absyn_Signed=0;static const int Cyc_Absyn_Unsigned=
1;struct Cyc_Absyn_Conref{void*v;};static const int Cyc_Absyn_Eq_constr=0;struct Cyc_Absyn_Eq_constr_struct{
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
void*Cyc_Absyn_compress_kb(void*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);extern struct _tagged_arr Cyc_Absyn_attribute2string(
void*);extern int Cyc_Absyn_fntype_att(void*a);struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
static const int Cyc_Std_String_pa=0;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};static const int Cyc_Std_Int_pa=1;struct Cyc_Std_Int_pa_struct{int
tag;unsigned int f1;};static const int Cyc_Std_Double_pa=2;struct Cyc_Std_Double_pa_struct{
int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=3;struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=4;struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};static const int Cyc_Std_ShortPtr_sa=0;struct Cyc_Std_ShortPtr_sa_struct{
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
Cyc_List_List*curr_namespace;};extern void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*
fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;extern struct
_tagged_arr Cyc_Absynpp_typ2string(void*);extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct Cyc_Set_Set;extern struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(
void*,void*));extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*
elt);extern int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern unsigned char
Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];
extern unsigned char Cyc_Dict_Absent[11];extern int Cyc_Dict_member(struct Cyc_Dict_Dict*
d,void*k);extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,
void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);extern struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);extern void Cyc_Dict_iter(void(*
f)(void*,void*),struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_map(
void*(*f)(void*),struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_filter_c(
int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);static const int Cyc_Tcenv_VarRes=
0;struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};static const int Cyc_Tcenv_StructRes=
1;struct Cyc_Tcenv_StructRes_struct{int tag;struct Cyc_Absyn_Structdecl*f1;};static
const int Cyc_Tcenv_TunionRes=2;struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*
f1;struct Cyc_Absyn_Tunionfield*f2;};static const int Cyc_Tcenv_EnumRes=3;struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};static const int
Cyc_Tcenv_AnonEnumRes=4;struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*f1;struct
Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*namespaces;
struct Cyc_Dict_Dict*structdecls;struct Cyc_Dict_Dict*uniondecls;struct Cyc_Dict_Dict*
tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;static const int
Cyc_Tcenv_NotLoop_j=0;static const int Cyc_Tcenv_CaseEnd_j=1;static const int Cyc_Tcenv_FnEnd_j=
2;static const int Cyc_Tcenv_Stmt_j=0;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};static const int Cyc_Tcenv_Outermost=0;struct Cyc_Tcenv_Outermost_struct{
int tag;void*f1;};static const int Cyc_Tcenv_Frame=1;struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};static const int Cyc_Tcenv_Hidden=2;struct Cyc_Tcenv_Hidden_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*
ae;struct Cyc_Core_Opt*le;};extern struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv();
extern struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct Cyc_Position_Segment*,struct
Cyc_Absyn_Fndecl*);extern struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tagged_arr*,struct Cyc_List_List*);
extern struct Cyc_Core_Opt*Cyc_Tcenv_lookup_xtuniondecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple0*);extern int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*);extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(void*t);
extern int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);extern int Cyc_Tcutil_is_function_type(void*t);extern void Cyc_Tcutil_explain_failure();
extern void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);extern void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn);extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,void*);extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);extern void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);extern int Cyc_Tcutil_bits_only(void*t);extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);extern int Cyc_Tcutil_supports_default(
void*);extern void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct
Cyc_Absyn_Exp*);extern void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,int new_block);extern unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);extern void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);extern struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
extern void Cyc_Tc_tcStructdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct
Cyc_Position_Segment*,struct Cyc_Absyn_Structdecl*);extern void Cyc_Tc_tcUniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Uniondecl*);extern void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Tuniondecl*);extern
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*);extern unsigned char Cyc_Tcdecl_Incompatible[17];struct
Cyc_Tcdecl_Xtunionfielddecl{struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*
field;};extern struct Cyc_Absyn_Structdecl*Cyc_Tcdecl_merge_structdecl(struct Cyc_Absyn_Structdecl*
d0,struct Cyc_Absyn_Structdecl*d1,struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg);extern struct Cyc_Absyn_Uniondecl*Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl*d0,struct Cyc_Absyn_Uniondecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Tuniondecl*Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_List_List*
Cyc_Tcdecl_sort_xtunion_fields(struct Cyc_List_List*f,int*res,struct _tagged_arr*v,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Tcgenrep_RepInfo;
extern struct Cyc_Dict_Dict*Cyc_Tcgenrep_empty_typerep_dict();struct _tuple3{struct
Cyc_Dict_Dict*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*f3;};extern struct
_tuple3 Cyc_Tcgenrep_tcGenrep(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,
struct Cyc_Position_Segment*loc,void*type,struct Cyc_Dict_Dict*dict);static
unsigned char _tmp0[1]="";static struct _tagged_arr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,
_tmp0 + 1};static struct _tagged_arr*Cyc_Tc_tc_msg=(struct _tagged_arr*)& Cyc_Tc_tc_msg_c;
static struct Cyc_List_List*Cyc_Tc_transfer_fn_type_atts(void*t,struct Cyc_List_List*
atts){void*_tmp1=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp7;struct Cyc_List_List*
_tmp9;struct Cyc_List_List**_tmpB;_LL3: if((unsigned int)_tmp1 > 3?*((int*)_tmp1)== 
Cyc_Absyn_FnType: 0){_LL8: _tmp7=((struct Cyc_Absyn_FnType_struct*)_tmp1)->f1;_LL10:
_tmp9=_tmp7.attributes;_tmpB=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp1)->f1).attributes;goto _LL4;}else{goto _LL5;}_LL5: goto _LL6;_LL4: {struct Cyc_List_List*
_tmpC=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){*
_tmpB=({struct Cyc_List_List*_tmpD=_cycalloc(sizeof(struct Cyc_List_List));_tmpD->hd=(
void*)((void*)atts->hd);_tmpD->tl=*_tmpB;_tmpD;});}else{_tmpC=({struct Cyc_List_List*
_tmpE=_cycalloc(sizeof(struct Cyc_List_List));_tmpE->hd=(void*)((void*)atts->hd);
_tmpE->tl=_tmpC;_tmpE;});}}return _tmpC;}_LL6: return({void*_tmpF[0]={};((struct
Cyc_List_List*(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("transfer_fn_type_atts",sizeof(unsigned char),22),_tag_arr(_tmpF,
sizeof(void*),0));});_LL2:;}struct _tuple4{void*f1;int f2;};static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Vardecl*vd,int check_var_init){struct _tagged_arr*v=(*vd->name).f2;
void*t=(void*)vd->type;void*sc=(void*)vd->sc;struct Cyc_List_List*atts=vd->attributes;
vd->escapes=1;{void*_tmp10=(*vd->name).f1;struct Cyc_List_List*_tmp18;struct Cyc_List_List*
_tmp1A;_LL18: if((unsigned int)_tmp10 > 1?*((int*)_tmp10)== Cyc_Absyn_Rel_n: 0){
_LL25: _tmp18=((struct Cyc_Absyn_Rel_n_struct*)_tmp10)->f1;if(_tmp18 == 0){goto
_LL19;}else{goto _LL20;}}else{goto _LL20;}_LL20: if((unsigned int)_tmp10 > 1?*((int*)
_tmp10)== Cyc_Absyn_Abs_n: 0){_LL27: _tmp1A=((struct Cyc_Absyn_Abs_n_struct*)_tmp10)->f1;
if(_tmp1A == 0){goto _LL21;}else{goto _LL22;}}else{goto _LL22;}_LL22: goto _LL23;_LL19:
goto _LL17;_LL21: goto _LL17;_LL23:({struct Cyc_Std_String_pa_struct _tmp1D;_tmp1D.tag=
Cyc_Std_String_pa;_tmp1D.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name);{
void*_tmp1C[1]={& _tmp1D};Cyc_Tcutil_terr(loc,_tag_arr("qualified variable declarations are not implemented (%s)",
sizeof(unsigned char),57),_tag_arr(_tmp1C,sizeof(void*),1));}});return;_LL17:;}(*
vd->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp1E=_cycalloc(sizeof(
struct Cyc_Absyn_Abs_n_struct));_tmp1E[0]=({struct Cyc_Absyn_Abs_n_struct _tmp1F;
_tmp1F.tag=Cyc_Absyn_Abs_n;_tmp1F.f1=te->ns;_tmp1F;});_tmp1E;});{void*_tmp20=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Tqual _tmp29;void*_tmp2B;_LL34: if((
unsigned int)_tmp20 > 3?*((int*)_tmp20)== Cyc_Absyn_ArrayType: 0){_LL44: _tmp2B=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp20)->f1;goto _LL42;_LL42: _tmp29=((
struct Cyc_Absyn_ArrayType_struct*)_tmp20)->f2;goto _LL40;_LL40: _tmp27=((struct Cyc_Absyn_ArrayType_struct*)
_tmp20)->f3;if(_tmp27 == 0){goto _LL38;}else{goto _LL36;}}else{goto _LL36;}_LL38: if(
vd->initializer != 0){goto _LL35;}else{goto _LL36;}_LL36: goto _LL37;_LL35:{void*
_tmp2D=(void*)((struct Cyc_Absyn_Exp*)_check_null(vd->initializer))->r;void*
_tmp39;struct _tagged_arr _tmp3B;struct Cyc_Absyn_Exp*_tmp3D;struct Cyc_List_List*
_tmp3F;struct Cyc_List_List*_tmp41;_LL47: if(*((int*)_tmp2D)== Cyc_Absyn_Const_e){
_LL58: _tmp39=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp2D)->f1;if((
unsigned int)_tmp39 > 1?*((int*)_tmp39)== Cyc_Absyn_String_c: 0){_LL60: _tmp3B=((
struct Cyc_Absyn_String_c_struct*)_tmp39)->f1;goto _LL48;}else{goto _LL49;}}else{
goto _LL49;}_LL49: if(*((int*)_tmp2D)== Cyc_Absyn_Comprehension_e){_LL62: _tmp3D=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp2D)->f2;goto _LL50;}else{goto _LL51;}
_LL51: if(*((int*)_tmp2D)== Cyc_Absyn_UnresolvedMem_e){_LL64: _tmp3F=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp2D)->f2;goto _LL52;}else{goto _LL53;}_LL53: if(*((int*)_tmp2D)== Cyc_Absyn_Array_e){
_LL66: _tmp41=((struct Cyc_Absyn_Array_e_struct*)_tmp2D)->f1;goto _LL54;}else{goto
_LL55;}_LL55: goto _LL56;_LL48: t=(void*)(vd->type=(void*)((void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp43=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp43[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp44;_tmp44.tag=Cyc_Absyn_ArrayType;_tmp44.f1=(void*)_tmp2B;_tmp44.f2=_tmp29;
_tmp44.f3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(_get_arr_size(_tmp3B,sizeof(
unsigned char)),0);_tmp44;});_tmp43;})));goto _LL46;_LL50: t=(void*)(vd->type=(
void*)((void*)({struct Cyc_Absyn_ArrayType_struct*_tmp45=_cycalloc(sizeof(struct
Cyc_Absyn_ArrayType_struct));_tmp45[0]=({struct Cyc_Absyn_ArrayType_struct _tmp46;
_tmp46.tag=Cyc_Absyn_ArrayType;_tmp46.f1=(void*)_tmp2B;_tmp46.f2=_tmp29;_tmp46.f3=(
struct Cyc_Absyn_Exp*)_tmp3D;_tmp46;});_tmp45;})));goto _LL46;_LL52: _tmp41=_tmp3F;
goto _LL54;_LL54: t=(void*)(vd->type=(void*)((void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp47=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp47[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp48;_tmp48.tag=Cyc_Absyn_ArrayType;_tmp48.f1=(void*)_tmp2B;_tmp48.f2=_tmp29;
_tmp48.f3=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp41),0);_tmp48;});_tmp47;})));goto _LL46;
_LL56: goto _LL46;_LL46:;}goto _LL33;_LL37: goto _LL33;_LL33:;}Cyc_Tcutil_check_valid_toplevel_type(
loc,te,t);{void*_tmp49=Cyc_Tcutil_compress(t);_LL75: if((unsigned int)_tmp49 > 3?*((
int*)_tmp49)== Cyc_Absyn_ArrayType: 0){goto _LL76;}else{goto _LL77;}_LL77: goto _LL78;
_LL76: vd->escapes=0;goto _LL74;_LL78: goto _LL74;_LL74:;}if(Cyc_Tcutil_is_function_type(
t)){atts=Cyc_Tc_transfer_fn_type_atts(t,atts);}if(sc == (void*)Cyc_Absyn_Extern? 1:
sc == (void*)Cyc_Absyn_ExternC){if(vd->initializer != 0){({void*_tmp4F[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("extern declaration should not have initializer",sizeof(
unsigned char),47),_tag_arr(_tmp4F,sizeof(void*),0));});}}else{if(! Cyc_Tcutil_is_function_type(
t)){for(0;atts != 0;atts=atts->tl){void*_tmp50=(void*)atts->hd;_LL82: if((
unsigned int)_tmp50 > 16?*((int*)_tmp50)== Cyc_Absyn_Aligned_att: 0){goto _LL83;}
else{goto _LL84;}_LL84: if((unsigned int)_tmp50 > 16?*((int*)_tmp50)== Cyc_Absyn_Section_att:
0){goto _LL85;}else{goto _LL86;}_LL86: if(_tmp50 == (void*)Cyc_Absyn_Nocommon_att){
goto _LL87;}else{goto _LL88;}_LL88: if(_tmp50 == (void*)Cyc_Absyn_Shared_att){goto
_LL89;}else{goto _LL90;}_LL90: if(_tmp50 == (void*)Cyc_Absyn_Unused_att){goto _LL91;}
else{goto _LL92;}_LL92: if(_tmp50 == (void*)Cyc_Absyn_Weak_att){goto _LL93;}else{
goto _LL94;}_LL94: if(_tmp50 == (void*)Cyc_Absyn_Dllimport_att){goto _LL95;}else{
goto _LL96;}_LL96: if(_tmp50 == (void*)Cyc_Absyn_Dllexport_att){goto _LL97;}else{
goto _LL98;}_LL98: goto _LL99;_LL83: goto _LL85;_LL85: goto _LL87;_LL87: goto _LL89;_LL89:
goto _LL91;_LL91: goto _LL93;_LL93: goto _LL95;_LL95: goto _LL97;_LL97: continue;_LL99:({
struct Cyc_Std_String_pa_struct _tmp66;_tmp66.tag=Cyc_Std_String_pa;_tmp66.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name);{struct Cyc_Std_String_pa_struct
_tmp65;_tmp65.tag=Cyc_Std_String_pa;_tmp65.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp64[2]={& _tmp65,& _tmp66};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s for variable %s",
sizeof(unsigned char),33),_tag_arr(_tmp64,sizeof(void*),2));}}});goto _LL81;_LL81:;}
if(vd->initializer == 0){if(check_var_init? ! Cyc_Tcutil_supports_default(t): 0){({
struct Cyc_Std_String_pa_struct _tmp69;_tmp69.tag=Cyc_Std_String_pa;_tmp69.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp68;_tmp68.tag=Cyc_Std_String_pa;_tmp68.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
vd->name);{void*_tmp67[2]={& _tmp68,& _tmp69};Cyc_Tcutil_terr(loc,_tag_arr("initializer required for variable %s of type %s",
sizeof(unsigned char),48),_tag_arr(_tmp67,sizeof(void*),2));}}});}}else{struct
Cyc_Absyn_Exp*_tmp6A=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);void*
_tmp6B=Cyc_Tcexp_tcExpInitializer(te,(void**)& t,_tmp6A);if(! Cyc_Tcutil_coerce_assign(
te,_tmp6A,t)){({struct Cyc_Std_String_pa_struct _tmp6F;_tmp6F.tag=Cyc_Std_String_pa;
_tmp6F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp6B);{struct Cyc_Std_String_pa_struct
_tmp6E;_tmp6E.tag=Cyc_Std_String_pa;_tmp6E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{struct Cyc_Std_String_pa_struct _tmp6D;_tmp6D.tag=Cyc_Std_String_pa;_tmp6D.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(vd->name);{void*_tmp6C[3]={& _tmp6D,&
_tmp6E,& _tmp6F};Cyc_Tcutil_terr(loc,_tag_arr("%s is declared with type \n%s\n but initialized with type \n%s",
sizeof(unsigned char),60),_tag_arr(_tmp6C,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}
if(! Cyc_Tcutil_is_const_exp(te,_tmp6A)){({void*_tmp70[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("initializer is not a constant expression",sizeof(unsigned char),41),
_tag_arr(_tmp70,sizeof(void*),0));});}}}else{for(0;atts != 0;atts=atts->tl){void*
_tmp71=(void*)atts->hd;_LL115: if((unsigned int)_tmp71 > 16?*((int*)_tmp71)== Cyc_Absyn_Regparm_att:
0){goto _LL116;}else{goto _LL117;}_LL117: if(_tmp71 == (void*)Cyc_Absyn_Stdcall_att){
goto _LL118;}else{goto _LL119;}_LL119: if(_tmp71 == (void*)Cyc_Absyn_Cdecl_att){goto
_LL120;}else{goto _LL121;}_LL121: if(_tmp71 == (void*)Cyc_Absyn_Fastcall_att){goto
_LL122;}else{goto _LL123;}_LL123: if(_tmp71 == (void*)Cyc_Absyn_Noreturn_att){goto
_LL124;}else{goto _LL125;}_LL125: if((unsigned int)_tmp71 > 16?*((int*)_tmp71)== 
Cyc_Absyn_Format_att: 0){goto _LL126;}else{goto _LL127;}_LL127: if(_tmp71 == (void*)
Cyc_Absyn_Const_att){goto _LL128;}else{goto _LL129;}_LL129: if((unsigned int)_tmp71
> 16?*((int*)_tmp71)== Cyc_Absyn_Aligned_att: 0){goto _LL130;}else{goto _LL131;}
_LL131: if(_tmp71 == (void*)Cyc_Absyn_Packed_att){goto _LL132;}else{goto _LL133;}
_LL133: goto _LL134;_LL116: goto _LL118;_LL118: goto _LL120;_LL120: goto _LL122;_LL122:
goto _LL124;_LL124: goto _LL126;_LL126: goto _LL128;_LL128:({void*_tmp87[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcVardecl: fn type attributes in function var decl",
sizeof(unsigned char),51),_tag_arr(_tmp87,sizeof(void*),0));});goto _LL114;_LL130:
goto _LL132;_LL132:({struct Cyc_Std_String_pa_struct _tmp89;_tmp89.tag=Cyc_Std_String_pa;
_tmp89.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd);{void*
_tmp88[1]={& _tmp89};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s in function declaration",
sizeof(unsigned char),41),_tag_arr(_tmp88,sizeof(void*),1));}});goto _LL114;
_LL134: continue;_LL114:;}}}{struct _handler_cons _tmp8A;_push_handler(& _tmp8A);{
int _tmp8C=0;if(setjmp(_tmp8A.handler)){_tmp8C=1;}if(! _tmp8C){{struct _tuple4*
_tmp8D=((struct _tuple4*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
ge->ordinaries,v);void*_tmp8E=(*_tmp8D).f1;void*_tmp9A;_LL144: if(*((int*)_tmp8E)
== Cyc_Tcenv_VarRes){_LL155: _tmp9A=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp8E)->f1;goto _LL145;}else{goto _LL146;}_LL146: if(*((int*)_tmp8E)== Cyc_Tcenv_StructRes){
goto _LL147;}else{goto _LL148;}_LL148: if(*((int*)_tmp8E)== Cyc_Tcenv_TunionRes){
goto _LL149;}else{goto _LL150;}_LL150: if(*((int*)_tmp8E)== Cyc_Tcenv_AnonEnumRes){
goto _LL151;}else{goto _LL152;}_LL152: if(*((int*)_tmp8E)== Cyc_Tcenv_EnumRes){goto
_LL153;}else{goto _LL143;}_LL145: {struct Cyc_Absyn_Global_b_struct*_tmp9C=({
struct Cyc_Absyn_Global_b_struct*_tmpA1=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_struct));
_tmpA1[0]=({struct Cyc_Absyn_Global_b_struct _tmpA2;_tmpA2.tag=Cyc_Absyn_Global_b;
_tmpA2.f1=vd;_tmpA2;});_tmpA1;});void*_tmp9D=Cyc_Tcdecl_merge_binding(_tmp9A,(
void*)_tmp9C,loc,Cyc_Tc_tc_msg);if(_tmp9D == (void*)Cyc_Absyn_Unresolved_b){goto
_LL143;}if(_tmp9D == _tmp9A?(*_tmp8D).f2: 0){goto _LL143;}ge->ordinaries=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(
ge->ordinaries,v,({struct _tuple4*_tmp9E=_cycalloc(sizeof(struct _tuple4));_tmp9E->f1=(
void*)({struct Cyc_Tcenv_VarRes_struct*_tmp9F=_cycalloc(sizeof(struct Cyc_Tcenv_VarRes_struct));
_tmp9F[0]=({struct Cyc_Tcenv_VarRes_struct _tmpA0;_tmpA0.tag=Cyc_Tcenv_VarRes;
_tmpA0.f1=(void*)_tmp9D;_tmpA0;});_tmp9F;});_tmp9E->f2=1;_tmp9E;}));goto _LL143;}
_LL147: goto _LL143;_LL149:({void*_tmpA3[0]={};Cyc_Tcutil_warn(loc,_tag_arr("variable declaration shadows previous [x]tunion constructor",
sizeof(unsigned char),60),_tag_arr(_tmpA3,sizeof(void*),0));});goto _LL143;_LL151:
goto _LL153;_LL153:({void*_tmpA4[0]={};Cyc_Tcutil_warn(loc,_tag_arr("variable declaration shadows previous enum tag",
sizeof(unsigned char),47),_tag_arr(_tmpA4,sizeof(void*),0));});goto _LL143;_LL143:;};
_pop_handler();}else{void*_tmp8B=(void*)_exn_thrown;void*_tmpA6=_tmp8B;_LL168:
if(_tmpA6 == Cyc_Dict_Absent){goto _LL169;}else{goto _LL170;}_LL170: goto _LL171;
_LL169: ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(ge->ordinaries,v,({struct _tuple4*
_tmpAC=_cycalloc(sizeof(struct _tuple4));_tmpAC->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*
_tmpAD=_cycalloc(sizeof(struct Cyc_Tcenv_VarRes_struct));_tmpAD[0]=({struct Cyc_Tcenv_VarRes_struct
_tmpAE;_tmpAE.tag=Cyc_Tcenv_VarRes;_tmpAE.f1=(void*)((void*)({struct Cyc_Absyn_Global_b_struct*
_tmpAF=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_struct));_tmpAF[0]=({struct Cyc_Absyn_Global_b_struct
_tmpB0;_tmpB0.tag=Cyc_Absyn_Global_b;_tmpB0.f1=vd;_tmpB0;});_tmpAF;}));_tmpAE;});
_tmpAD;});_tmpAC->f2=0;_tmpAC;}));goto _LL167;_LL171:(void)_throw(_tmpA6);_LL167:;}}}}
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct
Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*fd){struct _tagged_arr*v=(*fd->name).f2;
void*sc=(void*)fd->sc;{void*_tmpB1=(*fd->name).f1;struct Cyc_List_List*_tmpB9;
struct Cyc_List_List*_tmpBB;_LL179: if((unsigned int)_tmpB1 > 1?*((int*)_tmpB1)== 
Cyc_Absyn_Rel_n: 0){_LL186: _tmpB9=((struct Cyc_Absyn_Rel_n_struct*)_tmpB1)->f1;if(
_tmpB9 == 0){goto _LL180;}else{goto _LL181;}}else{goto _LL181;}_LL181: if((
unsigned int)_tmpB1 > 1?*((int*)_tmpB1)== Cyc_Absyn_Abs_n: 0){_LL188: _tmpBB=((
struct Cyc_Absyn_Abs_n_struct*)_tmpB1)->f1;goto _LL182;}else{goto _LL183;}_LL183:
goto _LL184;_LL180: goto _LL178;_LL182:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpBD=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmpBD[0]=({struct Cyc_Core_Impossible_struct
_tmpBE;_tmpBE.tag=Cyc_Core_Impossible;_tmpBE.f1=_tag_arr("tc: Abs_n in tcFndecl",
sizeof(unsigned char),22);_tmpBE;});_tmpBD;}));_LL184:({struct Cyc_Std_String_pa_struct
_tmpC0;_tmpC0.tag=Cyc_Std_String_pa;_tmpC0.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
fd->name);{void*_tmpBF[1]={& _tmpC0};Cyc_Tcutil_terr(loc,_tag_arr("qualified function declarations are not implemented (%s)",
sizeof(unsigned char),57),_tag_arr(_tmpBF,sizeof(void*),1));}});return;_LL178:;}(*
fd->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmpC1=_cycalloc(sizeof(
struct Cyc_Absyn_Abs_n_struct));_tmpC1[0]=({struct Cyc_Absyn_Abs_n_struct _tmpC2;
_tmpC2.tag=Cyc_Absyn_Abs_n;_tmpC2.f1=te->ns;_tmpC2;});_tmpC1;});Cyc_Tcutil_check_fndecl_valid_type(
loc,te,fd);{void*t=Cyc_Tcutil_fndecl2typ(fd);fd->attributes=Cyc_Tc_transfer_fn_type_atts(
t,fd->attributes);{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=
atts->tl){void*_tmpC3=(void*)atts->hd;_LL197: if(_tmpC3 == (void*)Cyc_Absyn_Packed_att){
goto _LL198;}else{goto _LL199;}_LL199: if((unsigned int)_tmpC3 > 16?*((int*)_tmpC3)
== Cyc_Absyn_Aligned_att: 0){goto _LL200;}else{goto _LL201;}_LL201: goto _LL202;
_LL198: goto _LL200;_LL200:({struct Cyc_Std_String_pa_struct _tmpCC;_tmpCC.tag=Cyc_Std_String_pa;
_tmpCC.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd);{void*
_tmpCB[1]={& _tmpCC};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s for function",
sizeof(unsigned char),30),_tag_arr(_tmpCB,sizeof(void*),1));}});goto _LL196;
_LL202: goto _LL196;_LL196:;}}{struct _handler_cons _tmpCD;_push_handler(& _tmpCD);{
int _tmpCF=0;if(setjmp(_tmpCD.handler)){_tmpCF=1;}if(! _tmpCF){{struct _tuple4*
_tmpD0=((struct _tuple4*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
ge->ordinaries,v);void*_tmpD1=(*_tmpD0).f1;void*_tmpDD;_LL211: if(*((int*)_tmpD1)
== Cyc_Tcenv_VarRes){_LL222: _tmpDD=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmpD1)->f1;goto _LL212;}else{goto _LL213;}_LL213: if(*((int*)_tmpD1)== Cyc_Tcenv_StructRes){
goto _LL214;}else{goto _LL215;}_LL215: if(*((int*)_tmpD1)== Cyc_Tcenv_TunionRes){
goto _LL216;}else{goto _LL217;}_LL217: if(*((int*)_tmpD1)== Cyc_Tcenv_AnonEnumRes){
goto _LL218;}else{goto _LL219;}_LL219: if(*((int*)_tmpD1)== Cyc_Tcenv_EnumRes){goto
_LL220;}else{goto _LL210;}_LL212: {struct Cyc_Absyn_Funname_b_struct*_tmpDF=({
struct Cyc_Absyn_Funname_b_struct*_tmpE4=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_struct));
_tmpE4[0]=({struct Cyc_Absyn_Funname_b_struct _tmpE5;_tmpE5.tag=Cyc_Absyn_Funname_b;
_tmpE5.f1=fd;_tmpE5;});_tmpE4;});void*_tmpE0=Cyc_Tcdecl_merge_binding(_tmpDD,(
void*)_tmpDF,loc,Cyc_Tc_tc_msg);if(_tmpE0 == (void*)Cyc_Absyn_Unresolved_b){goto
_LL210;}if(_tmpE0 == _tmpDD?(*_tmpD0).f2: 0){goto _LL210;}ge->ordinaries=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(
ge->ordinaries,v,({struct _tuple4*_tmpE1=_cycalloc(sizeof(struct _tuple4));_tmpE1->f1=(
void*)({struct Cyc_Tcenv_VarRes_struct*_tmpE2=_cycalloc(sizeof(struct Cyc_Tcenv_VarRes_struct));
_tmpE2[0]=({struct Cyc_Tcenv_VarRes_struct _tmpE3;_tmpE3.tag=Cyc_Tcenv_VarRes;
_tmpE3.f1=(void*)_tmpE0;_tmpE3;});_tmpE2;});_tmpE1->f2=1;_tmpE1;}));goto _LL210;}
_LL214: goto _LL210;_LL216:({void*_tmpE6[0]={};Cyc_Tcutil_warn(loc,_tag_arr("function declaration shadows previous [x]tunion constructor",
sizeof(unsigned char),60),_tag_arr(_tmpE6,sizeof(void*),0));});goto _LL210;_LL218:
goto _LL220;_LL220:({void*_tmpE7[0]={};Cyc_Tcutil_warn(loc,_tag_arr("function declaration shadows previous enum tag",
sizeof(unsigned char),47),_tag_arr(_tmpE7,sizeof(void*),0));});goto _LL210;_LL210:;};
_pop_handler();}else{void*_tmpCE=(void*)_exn_thrown;void*_tmpE9=_tmpCE;_LL235:
if(_tmpE9 == Cyc_Dict_Absent){goto _LL236;}else{goto _LL237;}_LL237: goto _LL238;
_LL236: ge->ordinaries=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(ge->ordinaries,v,({struct _tuple4*
_tmpEF=_cycalloc(sizeof(struct _tuple4));_tmpEF->f1=(void*)({struct Cyc_Tcenv_VarRes_struct*
_tmpF0=_cycalloc(sizeof(struct Cyc_Tcenv_VarRes_struct));_tmpF0[0]=({struct Cyc_Tcenv_VarRes_struct
_tmpF1;_tmpF1.tag=Cyc_Tcenv_VarRes;_tmpF1.f1=(void*)((void*)({struct Cyc_Absyn_Funname_b_struct*
_tmpF2=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_struct));_tmpF2[0]=({struct Cyc_Absyn_Funname_b_struct
_tmpF3;_tmpF3.tag=Cyc_Absyn_Funname_b;_tmpF3.f1=fd;_tmpF3;});_tmpF2;}));_tmpF1;});
_tmpF0;});_tmpEF->f2=0;_tmpEF;}));goto _LL234;_LL238:(void)_throw(_tmpE9);_LL234:;}}}
te->le=({struct Cyc_Core_Opt*_tmpF4=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpF4->v=(
void*)((void*)({struct Cyc_Tcenv_Outermost_struct*_tmpF5=_cycalloc(sizeof(struct
Cyc_Tcenv_Outermost_struct));_tmpF5[0]=({struct Cyc_Tcenv_Outermost_struct _tmpF6;
_tmpF6.tag=Cyc_Tcenv_Outermost;_tmpF6.f1=(void*)Cyc_Tcenv_new_fenv(loc,fd);
_tmpF6;});_tmpF5;}));_tmpF4;});Cyc_Tcstmt_tcStmt(te,fd->body,0);if(! Cyc_Tcenv_all_labels_resolved(
te)){({void*_tmpF7[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function has goto statements to undefined labels",
sizeof(unsigned char),49),_tag_arr(_tmpF7,sizeof(void*),0));});}te->le=0;}}
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Typedefdecl*td){struct _tagged_arr*
v=(*td->name).f2;{void*_tmpF8=(*td->name).f1;struct Cyc_List_List*_tmp100;struct
Cyc_List_List*_tmp102;_LL250: if((unsigned int)_tmpF8 > 1?*((int*)_tmpF8)== Cyc_Absyn_Rel_n:
0){_LL257: _tmp100=((struct Cyc_Absyn_Rel_n_struct*)_tmpF8)->f1;if(_tmp100 == 0){
goto _LL251;}else{goto _LL252;}}else{goto _LL252;}_LL252: if((unsigned int)_tmpF8 > 1?*((
int*)_tmpF8)== Cyc_Absyn_Abs_n: 0){_LL259: _tmp102=((struct Cyc_Absyn_Abs_n_struct*)
_tmpF8)->f1;if(_tmp102 == 0){goto _LL253;}else{goto _LL254;}}else{goto _LL254;}
_LL254: goto _LL255;_LL251: goto _LL249;_LL253: goto _LL249;_LL255:({struct Cyc_Std_String_pa_struct
_tmp105;_tmp105.tag=Cyc_Std_String_pa;_tmp105.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
td->name);{void*_tmp104[1]={& _tmp105};Cyc_Tcutil_terr(loc,_tag_arr("qualified typedef declarations are not implemented (%s)",
sizeof(unsigned char),56),_tag_arr(_tmp104,sizeof(void*),1));}});return;_LL249:;}
if(((int(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_member)(ge->typedefs,
v)){({struct Cyc_Std_String_pa_struct _tmp107;_tmp107.tag=Cyc_Std_String_pa;
_tmp107.f1=(struct _tagged_arr)*v;{void*_tmp106[1]={& _tmp107};Cyc_Tcutil_terr(loc,
_tag_arr("redeclaration of typedef %s",sizeof(unsigned char),28),_tag_arr(
_tmp106,sizeof(void*),1));}});return;}(*td->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp108=_cycalloc(sizeof(struct Cyc_Absyn_Abs_n_struct));_tmp108[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp109;_tmp109.tag=Cyc_Absyn_Abs_n;_tmp109.f1=te->ns;_tmp109;});_tmp108;});Cyc_Tcutil_check_unique_tvars(
loc,td->tvs);Cyc_Tcutil_add_tvar_identities(td->tvs);Cyc_Tcutil_check_type(loc,
te,td->tvs,(void*)Cyc_Absyn_AnyKind,0,(void*)td->defn);ge->typedefs=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(
ge->typedefs,v,td);}static void Cyc_Tc_tcStructFields(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct _tagged_arr obj,
struct Cyc_List_List*fields,struct Cyc_List_List*tvs){struct _RegionHandle _tmp10A=
_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp10A;_push_region(
uprev_rgn);{struct Cyc_List_List*prev_fields=0;struct Cyc_List_List*_tmp10B=fields;
for(0;_tmp10B != 0;_tmp10B=_tmp10B->tl){struct Cyc_Absyn_Structfield _tmp10E;struct
Cyc_List_List*_tmp10F;struct Cyc_Absyn_Exp*_tmp111;void*_tmp113;struct Cyc_Absyn_Tqual
_tmp115;struct _tagged_arr*_tmp117;struct Cyc_Absyn_Structfield*_tmp10C=(struct Cyc_Absyn_Structfield*)
_tmp10B->hd;_tmp10E=*_tmp10C;_LL280: _tmp117=_tmp10E.name;goto _LL278;_LL278:
_tmp115=_tmp10E.tq;goto _LL276;_LL276: _tmp113=(void*)_tmp10E.type;goto _LL274;
_LL274: _tmp111=_tmp10E.width;goto _LL272;_LL272: _tmp10F=_tmp10E.attributes;goto
_LL269;_LL269: if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),
struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,
prev_fields,_tmp117)){({struct Cyc_Std_String_pa_struct _tmp11B;_tmp11B.tag=Cyc_Std_String_pa;
_tmp11B.f1=(struct _tagged_arr)obj;{struct Cyc_Std_String_pa_struct _tmp11A;_tmp11A.tag=
Cyc_Std_String_pa;_tmp11A.f1=(struct _tagged_arr)*_tmp117;{void*_tmp119[2]={&
_tmp11A,& _tmp11B};Cyc_Tcutil_terr(loc,_tag_arr("duplicate field %s in %s",
sizeof(unsigned char),25),_tag_arr(_tmp119,sizeof(void*),2));}}});}if(Cyc_Std_strcmp(*
_tmp117,_tag_arr("",sizeof(unsigned char),1))!= 0){prev_fields=({struct Cyc_List_List*
_tmp11C=_region_malloc(uprev_rgn,sizeof(struct Cyc_List_List));_tmp11C->hd=
_tmp117;_tmp11C->tl=prev_fields;_tmp11C;});}Cyc_Tcutil_check_type(loc,te,tvs,(
void*)Cyc_Absyn_MemKind,0,_tmp113);Cyc_Tcutil_check_bitfield(loc,te,_tmp113,
_tmp111,_tmp117);}};_pop_region(uprev_rgn);}static void Cyc_Tc_tcStructOrUniondecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct _tagged_arr obj,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Structdecl*sd){struct _tagged_arr*_tmp11D=(*((struct _tuple0*)((
struct Cyc_Core_Opt*)_check_null(sd->name))->v)).f2;if(sd->name == 0){({struct Cyc_Std_String_pa_struct
_tmp11F;_tmp11F.tag=Cyc_Std_String_pa;_tmp11F.f1=(struct _tagged_arr)obj;{void*
_tmp11E[1]={& _tmp11F};Cyc_Tcutil_terr(loc,_tag_arr("anonymous %ss are not allowed at top level",
sizeof(unsigned char),43),_tag_arr(_tmp11E,sizeof(void*),1));}});return;}{struct
Cyc_List_List*atts=sd->attributes;for(0;atts != 0;atts=atts->tl){void*_tmp120=(
void*)atts->hd;_LL290: if(_tmp120 == (void*)Cyc_Absyn_Packed_att){goto _LL291;}
else{goto _LL292;}_LL292: if((unsigned int)_tmp120 > 16?*((int*)_tmp120)== Cyc_Absyn_Aligned_att:
0){goto _LL293;}else{goto _LL294;}_LL294: goto _LL295;_LL291: continue;_LL293:
continue;_LL295:({struct Cyc_Std_String_pa_struct _tmp12B;_tmp12B.tag=Cyc_Std_String_pa;
_tmp12B.f1=(struct _tagged_arr)*_tmp11D;{struct Cyc_Std_String_pa_struct _tmp12A;
_tmp12A.tag=Cyc_Std_String_pa;_tmp12A.f1=(struct _tagged_arr)obj;{struct Cyc_Std_String_pa_struct
_tmp129;_tmp129.tag=Cyc_Std_String_pa;_tmp129.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)atts->hd);{void*_tmp128[3]={& _tmp129,& _tmp12A,& _tmp12B};Cyc_Tcutil_terr(
loc,_tag_arr("bad attribute %s in %s %s definition",sizeof(unsigned char),37),
_tag_arr(_tmp128,sizeof(void*),3));}}}});goto _LL289;_LL289:;}}{struct Cyc_List_List*
tvs=sd->tvs;for(0;tvs != 0;tvs=tvs->tl){void*_tmp12C=Cyc_Absyn_compress_kb((void*)((
struct Cyc_Absyn_Tvar*)tvs->hd)->kind);struct Cyc_Core_Opt*_tmp138;struct Cyc_Core_Opt**
_tmp13A;void*_tmp13B;struct Cyc_Core_Opt*_tmp13D;struct Cyc_Core_Opt**_tmp13F;void*
_tmp140;struct Cyc_Core_Opt*_tmp142;struct Cyc_Core_Opt**_tmp144;void*_tmp145;
_LL302: if(*((int*)_tmp12C)== Cyc_Absyn_Unknown_kb){_LL313: _tmp138=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp12C)->f1;_tmp13A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp12C)->f1;goto _LL303;}else{goto _LL304;}_LL304: if(*((int*)_tmp12C)== Cyc_Absyn_Less_kb){
_LL318: _tmp13D=((struct Cyc_Absyn_Less_kb_struct*)_tmp12C)->f1;_tmp13F=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp12C)->f1;goto _LL316;_LL316: _tmp13B=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp12C)->f2;if(_tmp13B == (void*)Cyc_Absyn_MemKind){
goto _LL305;}else{goto _LL306;}}else{goto _LL306;}_LL306: if(*((int*)_tmp12C)== Cyc_Absyn_Less_kb){
_LL323: _tmp142=((struct Cyc_Absyn_Less_kb_struct*)_tmp12C)->f1;_tmp144=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp12C)->f1;goto _LL321;_LL321: _tmp140=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp12C)->f2;if(_tmp140 == (void*)Cyc_Absyn_AnyKind){
goto _LL307;}else{goto _LL308;}}else{goto _LL308;}_LL308: if(*((int*)_tmp12C)== Cyc_Absyn_Eq_kb){
_LL326: _tmp145=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp12C)->f1;if(_tmp145 == (
void*)Cyc_Absyn_MemKind){goto _LL309;}else{goto _LL310;}}else{goto _LL310;}_LL310:
goto _LL311;_LL303: _tmp13F=_tmp13A;goto _LL305;_LL305: _tmp144=_tmp13F;goto _LL307;
_LL307:*_tmp144=({struct Cyc_Core_Opt*_tmp147=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp147->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp148=_cycalloc(
sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp148[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp149;_tmp149.tag=Cyc_Absyn_Eq_kb;_tmp149.f1=(void*)((void*)Cyc_Absyn_BoxKind);
_tmp149;});_tmp148;}));_tmp147;});continue;_LL309:({struct Cyc_Std_String_pa_struct
_tmp14D;_tmp14D.tag=Cyc_Std_String_pa;_tmp14D.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)
tvs->hd)->name;{struct Cyc_Std_String_pa_struct _tmp14C;_tmp14C.tag=Cyc_Std_String_pa;
_tmp14C.f1=(struct _tagged_arr)*_tmp11D;{struct Cyc_Std_String_pa_struct _tmp14B;
_tmp14B.tag=Cyc_Std_String_pa;_tmp14B.f1=(struct _tagged_arr)obj;{void*_tmp14A[3]={&
_tmp14B,& _tmp14C,& _tmp14D};Cyc_Tcutil_terr(loc,_tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof(unsigned char),54),_tag_arr(_tmp14A,sizeof(void*),3));}}}});continue;
_LL311: continue;_LL301:;}}{void*_tmp14E=(*((struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(sd->name))->v)).f1;struct Cyc_List_List*_tmp156;struct Cyc_List_List*
_tmp158;_LL336: if((unsigned int)_tmp14E > 1?*((int*)_tmp14E)== Cyc_Absyn_Rel_n: 0){
_LL343: _tmp156=((struct Cyc_Absyn_Rel_n_struct*)_tmp14E)->f1;if(_tmp156 == 0){goto
_LL337;}else{goto _LL338;}}else{goto _LL338;}_LL338: if((unsigned int)_tmp14E > 1?*((
int*)_tmp14E)== Cyc_Absyn_Abs_n: 0){_LL345: _tmp158=((struct Cyc_Absyn_Abs_n_struct*)
_tmp14E)->f1;if(_tmp158 == 0){goto _LL339;}else{goto _LL340;}}else{goto _LL340;}
_LL340: goto _LL341;_LL337: goto _LL335;_LL339: goto _LL335;_LL341:({struct Cyc_Std_String_pa_struct
_tmp15B;_tmp15B.tag=Cyc_Std_String_pa;_tmp15B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((
struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(sd->name))->v);{void*_tmp15A[1]={&
_tmp15B};Cyc_Tcutil_terr(loc,_tag_arr("qualified struct declarations are not implemented (%s)",
sizeof(unsigned char),55),_tag_arr(_tmp15A,sizeof(void*),1));}});return;_LL335:;}(*((
struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(sd->name))->v)).f1=(void*)({
struct Cyc_Absyn_Abs_n_struct*_tmp15C=_cycalloc(sizeof(struct Cyc_Absyn_Abs_n_struct));
_tmp15C[0]=({struct Cyc_Absyn_Abs_n_struct _tmp15D;_tmp15D.tag=Cyc_Absyn_Abs_n;
_tmp15D.f1=te->ns;_tmp15D;});_tmp15C;});Cyc_Tcutil_check_unique_tvars(loc,sd->tvs);
Cyc_Tcutil_add_tvar_identities(sd->tvs);}struct _tuple5{struct Cyc_Core_Opt*f1;
struct Cyc_Core_Opt*f2;};void Cyc_Tc_tcStructdecl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Structdecl*sd){
struct _tagged_arr*_tmp15E=(*((struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(
sd->name))->v)).f2;const unsigned char*_tmp15F="struct";Cyc_Tc_tcStructOrUniondecl(
te,ge,_tag_arr(_tmp15F,sizeof(unsigned char),7),loc,sd);{struct Cyc_List_List*
_tmp160=sd->tvs;{struct _tuple5 _tmp162=({struct _tuple5 _tmp161;_tmp161.f1=sd->fields;
_tmp161.f2=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))
Cyc_Dict_lookup_opt)(ge->structdecls,_tmp15E);_tmp161;});struct Cyc_Core_Opt*
_tmp16C;struct Cyc_Core_Opt*_tmp16E;struct Cyc_Core_Opt*_tmp170;struct Cyc_Core_Opt*
_tmp172;struct Cyc_Core_Opt _tmp174;struct Cyc_List_List*_tmp175;struct Cyc_Core_Opt*
_tmp177;struct Cyc_Core_Opt _tmp179;struct Cyc_Absyn_Structdecl**_tmp17A;struct Cyc_Core_Opt*
_tmp17C;struct Cyc_Core_Opt _tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_Core_Opt*
_tmp181;struct Cyc_Core_Opt _tmp183;struct Cyc_Absyn_Structdecl**_tmp184;struct Cyc_Core_Opt*
_tmp186;_LL356: _LL367: _tmp16E=_tmp162.f1;if(_tmp16E == 0){goto _LL365;}else{goto
_LL358;}_LL365: _tmp16C=_tmp162.f2;if(_tmp16C == 0){goto _LL357;}else{goto _LL358;}
_LL358: _LL371: _tmp172=_tmp162.f1;if(_tmp172 == 0){goto _LL360;}else{_tmp174=*
_tmp172;_LL374: _tmp175=(struct Cyc_List_List*)_tmp174.v;goto _LL369;}_LL369:
_tmp170=_tmp162.f2;if(_tmp170 == 0){goto _LL359;}else{goto _LL360;}_LL360: _LL381:
_tmp17C=_tmp162.f1;if(_tmp17C == 0){goto _LL362;}else{_tmp17E=*_tmp17C;_LL384:
_tmp17F=(struct Cyc_List_List*)_tmp17E.v;goto _LL376;}_LL376: _tmp177=_tmp162.f2;
if(_tmp177 == 0){goto _LL362;}else{_tmp179=*_tmp177;_LL379: _tmp17A=(struct Cyc_Absyn_Structdecl**)
_tmp179.v;goto _LL361;}_LL362: _LL391: _tmp186=_tmp162.f1;if(_tmp186 == 0){goto
_LL386;}else{goto _LL355;}_LL386: _tmp181=_tmp162.f2;if(_tmp181 == 0){goto _LL355;}
else{_tmp183=*_tmp181;_LL389: _tmp184=(struct Cyc_Absyn_Structdecl**)_tmp183.v;
goto _LL363;}_LL357: ge->structdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Structdecl**v))Cyc_Dict_insert)(ge->structdecls,
_tmp15E,({struct Cyc_Absyn_Structdecl**_tmp188=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl*));
_tmp188[0]=sd;_tmp188;}));goto _LL355;_LL359: {struct Cyc_Absyn_Structdecl**
_tmp189=({struct Cyc_Absyn_Structdecl**_tmp18A=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl*));
_tmp18A[0]=({struct Cyc_Absyn_Structdecl*_tmp18B=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));
_tmp18B->sc=(void*)((void*)Cyc_Absyn_Extern);_tmp18B->name=sd->name;_tmp18B->tvs=
_tmp160;_tmp18B->fields=0;_tmp18B->attributes=0;_tmp18B;});_tmp18A;});ge->structdecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Structdecl**
v))Cyc_Dict_insert)(ge->structdecls,_tmp15E,_tmp189);Cyc_Tc_tcStructFields(te,ge,
loc,_tag_arr(_tmp15F,sizeof(unsigned char),7),_tmp175,_tmp160);*_tmp189=sd;goto
_LL355;}_LL361: {struct Cyc_Absyn_Structdecl*_tmp18C=*_tmp17A;*_tmp17A=({struct
Cyc_Absyn_Structdecl*_tmp18D=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));
_tmp18D->sc=(void*)((void*)Cyc_Absyn_Extern);_tmp18D->name=sd->name;_tmp18D->tvs=
_tmp160;_tmp18D->fields=0;_tmp18D->attributes=0;_tmp18D;});Cyc_Tc_tcStructFields(
te,ge,loc,_tag_arr(_tmp15F,sizeof(unsigned char),7),_tmp17F,_tmp160);*_tmp17A=
_tmp18C;_tmp184=_tmp17A;goto _LL363;}_LL363: {struct Cyc_Absyn_Structdecl*_tmp18E=
Cyc_Tcdecl_merge_structdecl(*_tmp184,sd,loc,Cyc_Tc_tc_msg);if(_tmp18E == 0){
return;}else{*_tmp184=(struct Cyc_Absyn_Structdecl*)_check_null(_tmp18E);sd=(
struct Cyc_Absyn_Structdecl*)_check_null(_tmp18E);goto _LL355;}}_LL355:;}ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*
v))Cyc_Dict_insert)(ge->ordinaries,_tmp15E,({struct _tuple4*_tmp18F=_cycalloc(
sizeof(struct _tuple4));_tmp18F->f1=(void*)({struct Cyc_Tcenv_StructRes_struct*
_tmp190=_cycalloc(sizeof(struct Cyc_Tcenv_StructRes_struct));_tmp190[0]=({struct
Cyc_Tcenv_StructRes_struct _tmp191;_tmp191.tag=Cyc_Tcenv_StructRes;_tmp191.f1=sd;
_tmp191;});_tmp190;});_tmp18F->f2=1;_tmp18F;}));}}void Cyc_Tc_tcUniondecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct
Cyc_Absyn_Uniondecl*ud){struct _tagged_arr*_tmp192=(*((struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(ud->name))->v)).f2;const unsigned char*_tmp193="union";Cyc_Tc_tcStructOrUniondecl(
te,ge,_tag_arr(_tmp193,sizeof(unsigned char),6),loc,(struct Cyc_Absyn_Structdecl*)
ud);{struct Cyc_List_List*_tmp194=ud->tvs;struct _tuple5 _tmp196=({struct _tuple5
_tmp195;_tmp195.f1=ud->fields;_tmp195.f2=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)(ge->uniondecls,_tmp192);_tmp195;});
struct Cyc_Core_Opt*_tmp1A0;struct Cyc_Core_Opt*_tmp1A2;struct Cyc_Core_Opt*_tmp1A4;
struct Cyc_Core_Opt*_tmp1A6;struct Cyc_Core_Opt _tmp1A8;struct Cyc_List_List*_tmp1A9;
struct Cyc_Core_Opt*_tmp1AB;struct Cyc_Core_Opt _tmp1AD;struct Cyc_Absyn_Uniondecl**
_tmp1AE;struct Cyc_Core_Opt*_tmp1B0;struct Cyc_Core_Opt _tmp1B2;struct Cyc_List_List*
_tmp1B3;struct Cyc_Core_Opt*_tmp1B5;struct Cyc_Core_Opt _tmp1B7;struct Cyc_Absyn_Uniondecl**
_tmp1B8;struct Cyc_Core_Opt*_tmp1BA;_LL408: _LL419: _tmp1A2=_tmp196.f1;if(_tmp1A2 == 
0){goto _LL417;}else{goto _LL410;}_LL417: _tmp1A0=_tmp196.f2;if(_tmp1A0 == 0){goto
_LL409;}else{goto _LL410;}_LL410: _LL423: _tmp1A6=_tmp196.f1;if(_tmp1A6 == 0){goto
_LL412;}else{_tmp1A8=*_tmp1A6;_LL426: _tmp1A9=(struct Cyc_List_List*)_tmp1A8.v;
goto _LL421;}_LL421: _tmp1A4=_tmp196.f2;if(_tmp1A4 == 0){goto _LL411;}else{goto
_LL412;}_LL412: _LL433: _tmp1B0=_tmp196.f1;if(_tmp1B0 == 0){goto _LL414;}else{
_tmp1B2=*_tmp1B0;_LL436: _tmp1B3=(struct Cyc_List_List*)_tmp1B2.v;goto _LL428;}
_LL428: _tmp1AB=_tmp196.f2;if(_tmp1AB == 0){goto _LL414;}else{_tmp1AD=*_tmp1AB;
_LL431: _tmp1AE=(struct Cyc_Absyn_Uniondecl**)_tmp1AD.v;goto _LL413;}_LL414: _LL443:
_tmp1BA=_tmp196.f1;if(_tmp1BA == 0){goto _LL438;}else{goto _LL407;}_LL438: _tmp1B5=
_tmp196.f2;if(_tmp1B5 == 0){goto _LL407;}else{_tmp1B7=*_tmp1B5;_LL441: _tmp1B8=(
struct Cyc_Absyn_Uniondecl**)_tmp1B7.v;goto _LL415;}_LL409: ge->uniondecls=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Uniondecl**
v))Cyc_Dict_insert)(ge->uniondecls,_tmp192,({struct Cyc_Absyn_Uniondecl**_tmp1BC=
_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl*));_tmp1BC[0]=ud;_tmp1BC;}));goto
_LL407;_LL411: {struct Cyc_Absyn_Uniondecl**_tmp1BD=({struct Cyc_Absyn_Uniondecl**
_tmp1C2=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl*));_tmp1C2[0]=({struct Cyc_Absyn_Uniondecl*
_tmp1C3=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl));_tmp1C3->sc=(void*)((void*)
Cyc_Absyn_Extern);_tmp1C3->name=ud->name;_tmp1C3->tvs=_tmp194;_tmp1C3->fields=0;
_tmp1C3->attributes=ud->attributes;_tmp1C3;});_tmp1C2;});ge->uniondecls=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Uniondecl**
v))Cyc_Dict_insert)(ge->uniondecls,_tmp192,_tmp1BD);Cyc_Tc_tcStructFields(te,ge,
loc,_tag_arr(_tmp193,sizeof(unsigned char),6),_tmp1A9,_tmp194);{struct Cyc_List_List*
f=_tmp1A9;for(0;f != 0;f=f->tl){if(! Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Structfield*)
f->hd)->type)){({struct Cyc_Std_String_pa_struct _tmp1C1;_tmp1C1.tag=Cyc_Std_String_pa;
_tmp1C1.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Absyn_Structfield*)
f->hd)->type);{struct Cyc_Std_String_pa_struct _tmp1C0;_tmp1C0.tag=Cyc_Std_String_pa;
_tmp1C0.f1=(struct _tagged_arr)*_tmp192;{struct Cyc_Std_String_pa_struct _tmp1BF;
_tmp1BF.tag=Cyc_Std_String_pa;_tmp1BF.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Structfield*)
f->hd)->name;{void*_tmp1BE[3]={& _tmp1BF,& _tmp1C0,& _tmp1C1};Cyc_Tcutil_terr(loc,
_tag_arr("field %s of union %s has type %s which is not `bits-only'",sizeof(
unsigned char),58),_tag_arr(_tmp1BE,sizeof(void*),3));}}}});}}}*_tmp1BD=ud;goto
_LL407;}_LL413: {struct Cyc_Absyn_Uniondecl*_tmp1C4=*_tmp1AE;*_tmp1AE=({struct Cyc_Absyn_Uniondecl*
_tmp1C5=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl));_tmp1C5->sc=(void*)((void*)
Cyc_Absyn_Extern);_tmp1C5->name=ud->name;_tmp1C5->tvs=_tmp194;_tmp1C5->fields=0;
_tmp1C5->attributes=ud->attributes;_tmp1C5;});Cyc_Tc_tcStructFields(te,ge,loc,
_tag_arr(_tmp193,sizeof(unsigned char),6),_tmp1B3,_tmp194);*_tmp1AE=_tmp1C4;
_tmp1B8=_tmp1AE;goto _LL415;}_LL415: {struct Cyc_Absyn_Uniondecl*_tmp1C6=Cyc_Tcdecl_merge_uniondecl(*
_tmp1B8,ud,loc,Cyc_Tc_tc_msg);if(_tmp1C6 == 0){return;}else{*_tmp1B8=(struct Cyc_Absyn_Uniondecl*)
_check_null(_tmp1C6);goto _LL407;}}_LL407:;}}struct _tuple6{struct Cyc_Absyn_Tqual
f1;void*f2;};static struct Cyc_List_List*Cyc_Tc_tcTunionFields(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct _tagged_arr obj,
int is_xtunion,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*
tvs,struct Cyc_Absyn_Tuniondecl*tudres){{struct Cyc_List_List*_tmp1C7=fields;for(0;
_tmp1C7 != 0;_tmp1C7=_tmp1C7->tl){struct Cyc_Absyn_Tunionfield*_tmp1C8=(struct Cyc_Absyn_Tunionfield*)
_tmp1C7->hd;{struct Cyc_List_List*tvs=_tmp1C8->tvs;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp1C9=(struct Cyc_Absyn_Tvar*)tvs->hd;void*_tmp1CA=Cyc_Absyn_compress_kb((
void*)_tmp1C9->kind);void*_tmp1D6;struct Cyc_Core_Opt*_tmp1D8;struct Cyc_Core_Opt**
_tmp1DA;void*_tmp1DB;struct Cyc_Core_Opt*_tmp1DD;struct Cyc_Core_Opt**_tmp1DF;void*
_tmp1E0;struct Cyc_Core_Opt*_tmp1E2;struct Cyc_Core_Opt**_tmp1E4;_LL460: if(*((int*)
_tmp1CA)== Cyc_Absyn_Eq_kb){_LL471: _tmp1D6=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp1CA)->f1;if(_tmp1D6 == (void*)Cyc_Absyn_MemKind){goto _LL461;}else{goto _LL462;}}
else{goto _LL462;}_LL462: if(*((int*)_tmp1CA)== Cyc_Absyn_Unknown_kb){_LL473:
_tmp1D8=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp1CA)->f1;_tmp1DA=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp1CA)->f1;goto _LL463;}else{goto _LL464;}
_LL464: if(*((int*)_tmp1CA)== Cyc_Absyn_Less_kb){_LL478: _tmp1DD=((struct Cyc_Absyn_Less_kb_struct*)
_tmp1CA)->f1;_tmp1DF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp1CA)->f1;goto _LL476;_LL476: _tmp1DB=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp1CA)->f2;if(_tmp1DB == (void*)Cyc_Absyn_MemKind){goto _LL465;}else{goto _LL466;}}
else{goto _LL466;}_LL466: if(*((int*)_tmp1CA)== Cyc_Absyn_Less_kb){_LL483: _tmp1E2=((
struct Cyc_Absyn_Less_kb_struct*)_tmp1CA)->f1;_tmp1E4=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp1CA)->f1;goto _LL481;_LL481: _tmp1E0=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp1CA)->f2;if(_tmp1E0 == (void*)Cyc_Absyn_AnyKind){
goto _LL467;}else{goto _LL468;}}else{goto _LL468;}_LL468: goto _LL469;_LL461:({struct
Cyc_Std_String_pa_struct _tmp1E6;_tmp1E6.tag=Cyc_Std_String_pa;_tmp1E6.f1=(struct
_tagged_arr)*(*_tmp1C8->name).f2;{void*_tmp1E5[1]={& _tmp1E6};Cyc_Tcutil_terr(
_tmp1C8->loc,_tag_arr("field %s abstracts type variable of kind M",sizeof(
unsigned char),43),_tag_arr(_tmp1E5,sizeof(void*),1));}});goto _LL459;_LL463:
_tmp1DF=_tmp1DA;goto _LL465;_LL465: _tmp1E4=_tmp1DF;goto _LL467;_LL467:*_tmp1E4=({
struct Cyc_Core_Opt*_tmp1E7=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1E7->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp1E8=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));
_tmp1E8[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp1E9;_tmp1E9.tag=Cyc_Absyn_Eq_kb;
_tmp1E9.f1=(void*)((void*)Cyc_Absyn_BoxKind);_tmp1E9;});_tmp1E8;}));_tmp1E7;});
goto _LL459;_LL469: goto _LL459;_LL459:;}}{struct Cyc_List_List*alltvs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmp1C8->tvs);
Cyc_Tcutil_check_unique_tvars(loc,alltvs);Cyc_Tcutil_add_tvar_identities(_tmp1C8->tvs);{
struct Cyc_List_List*typs=_tmp1C8->typs;for(0;typs != 0;typs=typs->tl){Cyc_Tcutil_check_type(
_tmp1C8->loc,te,alltvs,(void*)Cyc_Absyn_MemKind,0,(*((struct _tuple6*)typs->hd)).f2);}}{
void*_tmp1EA=(*_tmp1C8->name).f1;struct Cyc_List_List*_tmp1F4;_LL492: if((
unsigned int)_tmp1EA > 1?*((int*)_tmp1EA)== Cyc_Absyn_Rel_n: 0){_LL501: _tmp1F4=((
struct Cyc_Absyn_Rel_n_struct*)_tmp1EA)->f1;if(_tmp1F4 == 0){goto _LL493;}else{goto
_LL494;}}else{goto _LL494;}_LL494: if((unsigned int)_tmp1EA > 1?*((int*)_tmp1EA)== 
Cyc_Absyn_Rel_n: 0){goto _LL495;}else{goto _LL496;}_LL496: if((unsigned int)_tmp1EA > 
1?*((int*)_tmp1EA)== Cyc_Absyn_Abs_n: 0){goto _LL497;}else{goto _LL498;}_LL498: if(
_tmp1EA == (void*)Cyc_Absyn_Loc_n){goto _LL499;}else{goto _LL491;}_LL493: if(
is_xtunion){(*_tmp1C8->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp1F6=
_cycalloc(sizeof(struct Cyc_Absyn_Abs_n_struct));_tmp1F6[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp1F7;_tmp1F7.tag=Cyc_Absyn_Abs_n;_tmp1F7.f1=te->ns;_tmp1F7;});_tmp1F6;});}
else{(*_tmp1C8->name).f1=(*name).f1;}goto _LL491;_LL495:({struct Cyc_Std_String_pa_struct
_tmp1F9;_tmp1F9.tag=Cyc_Std_String_pa;_tmp1F9.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp1C8->name);{void*_tmp1F8[1]={& _tmp1F9};Cyc_Tcutil_terr(_tmp1C8->loc,_tag_arr("qualified tunionfield declarations are not allowed (%s)",
sizeof(unsigned char),56),_tag_arr(_tmp1F8,sizeof(void*),1));}});goto _LL491;
_LL497: goto _LL491;_LL499:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1FA=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp1FA[0]=({struct
Cyc_Core_Impossible_struct _tmp1FB;_tmp1FB.tag=Cyc_Core_Impossible;_tmp1FB.f1=
_tag_arr("tcTunionFields: Loc_n",sizeof(unsigned char),22);_tmp1FB;});_tmp1FA;}));
goto _LL491;_LL491:;}}}}{struct Cyc_List_List*fields2;if(is_xtunion){int _tmp1FC=1;
struct Cyc_List_List*_tmp1FD=Cyc_Tcdecl_sort_xtunion_fields(fields,& _tmp1FC,(*
name).f2,loc,Cyc_Tc_tc_msg);if(_tmp1FC){fields2=_tmp1FD;}else{fields2=0;}}else{
struct _RegionHandle _tmp1FE=_new_region("uprev_rgn");struct _RegionHandle*
uprev_rgn=& _tmp1FE;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;{
struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Tunionfield*
_tmp1FF=(struct Cyc_Absyn_Tunionfield*)fs->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,(*_tmp1FF->name).f2)){({struct Cyc_Std_String_pa_struct
_tmp202;_tmp202.tag=Cyc_Std_String_pa;_tmp202.f1=(struct _tagged_arr)obj;{struct
Cyc_Std_String_pa_struct _tmp201;_tmp201.tag=Cyc_Std_String_pa;_tmp201.f1=(struct
_tagged_arr)*(*_tmp1FF->name).f2;{void*_tmp200[2]={& _tmp201,& _tmp202};Cyc_Tcutil_terr(
_tmp1FF->loc,_tag_arr("duplicate field name %s in %s",sizeof(unsigned char),30),
_tag_arr(_tmp200,sizeof(void*),2));}}});}else{prev_fields=({struct Cyc_List_List*
_tmp203=_region_malloc(uprev_rgn,sizeof(struct Cyc_List_List));_tmp203->hd=(*
_tmp1FF->name).f2;_tmp203->tl=prev_fields;_tmp203;});}if((void*)_tmp1FF->sc != (
void*)Cyc_Absyn_Public){({struct Cyc_Std_String_pa_struct _tmp205;_tmp205.tag=Cyc_Std_String_pa;
_tmp205.f1=(struct _tagged_arr)*(*_tmp1FF->name).f2;{void*_tmp204[1]={& _tmp205};
Cyc_Tcutil_warn(loc,_tag_arr("ignoring scope of field %s",sizeof(unsigned char),
27),_tag_arr(_tmp204,sizeof(void*),1));}});(void*)(_tmp1FF->sc=(void*)((void*)
Cyc_Absyn_Public));}}}fields2=fields;};_pop_region(uprev_rgn);}{struct Cyc_List_List*
_tmp206=fields;for(0;_tmp206 != 0;_tmp206=_tmp206->tl){struct Cyc_Absyn_Tunionfield*
_tmp207=(struct Cyc_Absyn_Tunionfield*)_tmp206->hd;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp207->name).f2,({struct _tuple4*_tmp208=_cycalloc(sizeof(struct _tuple4));
_tmp208->f1=(void*)({struct Cyc_Tcenv_TunionRes_struct*_tmp209=_cycalloc(sizeof(
struct Cyc_Tcenv_TunionRes_struct));_tmp209[0]=({struct Cyc_Tcenv_TunionRes_struct
_tmp20A;_tmp20A.tag=Cyc_Tcenv_TunionRes;_tmp20A.f1=tudres;_tmp20A.f2=_tmp207;
_tmp20A;});_tmp209;});_tmp208->f2=1;_tmp208;}));}}return fields2;}}void Cyc_Tc_tcTuniondecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Tuniondecl*tud){struct _tagged_arr*v=(*tud->name).f2;struct
_tagged_arr obj;if(tud->is_xtunion){obj=_tag_arr("xtunion",sizeof(unsigned char),
8);}else{obj=_tag_arr("tunion",sizeof(unsigned char),7);}{struct Cyc_List_List*
tvs=tud->tvs;{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){void*
_tmp20B=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);
struct Cyc_Core_Opt*_tmp217;struct Cyc_Core_Opt**_tmp219;void*_tmp21A;struct Cyc_Core_Opt*
_tmp21C;struct Cyc_Core_Opt**_tmp21E;void*_tmp21F;struct Cyc_Core_Opt*_tmp221;
struct Cyc_Core_Opt**_tmp223;void*_tmp224;_LL525: if(*((int*)_tmp20B)== Cyc_Absyn_Unknown_kb){
_LL536: _tmp217=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp20B)->f1;_tmp219=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp20B)->f1;goto _LL526;}
else{goto _LL527;}_LL527: if(*((int*)_tmp20B)== Cyc_Absyn_Less_kb){_LL541: _tmp21C=((
struct Cyc_Absyn_Less_kb_struct*)_tmp20B)->f1;_tmp21E=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp20B)->f1;goto _LL539;_LL539: _tmp21A=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp20B)->f2;if(_tmp21A == (void*)Cyc_Absyn_MemKind){
goto _LL528;}else{goto _LL529;}}else{goto _LL529;}_LL529: if(*((int*)_tmp20B)== Cyc_Absyn_Less_kb){
_LL546: _tmp221=((struct Cyc_Absyn_Less_kb_struct*)_tmp20B)->f1;_tmp223=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp20B)->f1;goto _LL544;_LL544: _tmp21F=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp20B)->f2;if(_tmp21F == (void*)Cyc_Absyn_AnyKind){
goto _LL530;}else{goto _LL531;}}else{goto _LL531;}_LL531: if(*((int*)_tmp20B)== Cyc_Absyn_Eq_kb){
_LL549: _tmp224=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp20B)->f1;if(_tmp224 == (
void*)Cyc_Absyn_MemKind){goto _LL532;}else{goto _LL533;}}else{goto _LL533;}_LL533:
goto _LL534;_LL526: _tmp21E=_tmp219;goto _LL528;_LL528: _tmp223=_tmp21E;goto _LL530;
_LL530:*_tmp223=({struct Cyc_Core_Opt*_tmp226=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp226->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp227=_cycalloc(
sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp227[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp228;_tmp228.tag=Cyc_Absyn_Eq_kb;_tmp228.f1=(void*)((void*)Cyc_Absyn_BoxKind);
_tmp228;});_tmp227;}));_tmp226;});goto _LL524;_LL532:({struct Cyc_Std_String_pa_struct
_tmp22C;_tmp22C.tag=Cyc_Std_String_pa;_tmp22C.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)
tvs2->hd)->name;{struct Cyc_Std_String_pa_struct _tmp22B;_tmp22B.tag=Cyc_Std_String_pa;
_tmp22B.f1=(struct _tagged_arr)*v;{struct Cyc_Std_String_pa_struct _tmp22A;_tmp22A.tag=
Cyc_Std_String_pa;_tmp22A.f1=(struct _tagged_arr)obj;{void*_tmp229[3]={& _tmp22A,&
_tmp22B,& _tmp22C};Cyc_Tcutil_terr(loc,_tag_arr("%s %s attempts to abstract type variable %s of kind M",
sizeof(unsigned char),54),_tag_arr(_tmp229,sizeof(void*),3));}}}});goto _LL524;
_LL534: goto _LL524;_LL524:;}}Cyc_Tcutil_check_unique_tvars(loc,tvs);Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_Core_Opt*tud_opt;if(tud->is_xtunion){{struct _handler_cons _tmp22D;
_push_handler(& _tmp22D);{int _tmp22F=0;if(setjmp(_tmp22D.handler)){_tmp22F=1;}if(
! _tmp22F){tud_opt=Cyc_Tcenv_lookup_xtuniondecl(te,loc,tud->name);;_pop_handler();}
else{void*_tmp22E=(void*)_exn_thrown;void*_tmp231=_tmp22E;_LL563: if(_tmp231 == 
Cyc_Dict_Absent){goto _LL564;}else{goto _LL565;}_LL565: goto _LL566;_LL564:({struct
Cyc_Std_String_pa_struct _tmp238;_tmp238.tag=Cyc_Std_String_pa;_tmp238.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(tud->name);{void*_tmp237[1]={& _tmp238};Cyc_Tcutil_terr(
loc,_tag_arr("qualified xtunion declaration %s is not an existing xtunion",
sizeof(unsigned char),60),_tag_arr(_tmp237,sizeof(void*),1));}});return;_LL566:(
void)_throw(_tmp231);_LL562:;}}}if(tud_opt != 0){tud->name=(*((struct Cyc_Absyn_Tuniondecl**)
tud_opt->v))->name;}else{(*tud->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp239=_cycalloc(sizeof(struct Cyc_Absyn_Abs_n_struct));_tmp239[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp23A;_tmp23A.tag=Cyc_Absyn_Abs_n;_tmp23A.f1=te->ns;_tmp23A;});_tmp239;});}}
else{{void*_tmp23B=(*tud->name).f1;struct Cyc_List_List*_tmp243;_LL573: if((
unsigned int)_tmp23B > 1?*((int*)_tmp23B)== Cyc_Absyn_Rel_n: 0){_LL580: _tmp243=((
struct Cyc_Absyn_Rel_n_struct*)_tmp23B)->f1;if(_tmp243 == 0){goto _LL574;}else{goto
_LL575;}}else{goto _LL575;}_LL575: if((unsigned int)_tmp23B > 1?*((int*)_tmp23B)== 
Cyc_Absyn_Abs_n: 0){goto _LL576;}else{goto _LL577;}_LL577: goto _LL578;_LL574:(*tud->name).f1=(
void*)({struct Cyc_Absyn_Abs_n_struct*_tmp245=_cycalloc(sizeof(struct Cyc_Absyn_Abs_n_struct));
_tmp245[0]=({struct Cyc_Absyn_Abs_n_struct _tmp246;_tmp246.tag=Cyc_Absyn_Abs_n;
_tmp246.f1=te->ns;_tmp246;});_tmp245;});goto _LL572;_LL576: goto _LL578;_LL578:({
struct Cyc_Std_String_pa_struct _tmp248;_tmp248.tag=Cyc_Std_String_pa;_tmp248.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(tud->name);{void*_tmp247[1]={& _tmp248};
Cyc_Tcutil_terr(loc,_tag_arr("qualified tunion declarations are not implemented (%s)",
sizeof(unsigned char),55),_tag_arr(_tmp247,sizeof(void*),1));}});return;_LL572:;}
tud_opt=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)(
ge->tuniondecls,v);}{struct _tuple5 _tmp24A=({struct _tuple5 _tmp249;_tmp249.f1=tud->fields;
_tmp249.f2=tud_opt;_tmp249;});struct Cyc_Core_Opt*_tmp254;struct Cyc_Core_Opt*
_tmp256;struct Cyc_Core_Opt*_tmp258;struct Cyc_Core_Opt*_tmp25A;struct Cyc_Core_Opt
_tmp25C;struct Cyc_List_List*_tmp25D;struct Cyc_List_List**_tmp25F;struct Cyc_Core_Opt*
_tmp260;struct Cyc_Core_Opt _tmp262;struct Cyc_Absyn_Tuniondecl**_tmp263;struct Cyc_Core_Opt*
_tmp265;struct Cyc_Core_Opt _tmp267;struct Cyc_List_List*_tmp268;struct Cyc_List_List**
_tmp26A;struct Cyc_Core_Opt*_tmp26B;struct Cyc_Core_Opt _tmp26D;struct Cyc_Absyn_Tuniondecl**
_tmp26E;struct Cyc_Core_Opt*_tmp270;_LL588: _LL599: _tmp256=_tmp24A.f1;if(_tmp256 == 
0){goto _LL597;}else{goto _LL590;}_LL597: _tmp254=_tmp24A.f2;if(_tmp254 == 0){goto
_LL589;}else{goto _LL590;}_LL590: _LL603: _tmp25A=_tmp24A.f1;if(_tmp25A == 0){goto
_LL592;}else{_tmp25C=*_tmp25A;_LL606: _tmp25D=(struct Cyc_List_List*)_tmp25C.v;
_tmp25F=(struct Cyc_List_List**)&(*_tmp24A.f1).v;goto _LL601;}_LL601: _tmp258=
_tmp24A.f2;if(_tmp258 == 0){goto _LL591;}else{goto _LL592;}_LL592: _LL614: _tmp265=
_tmp24A.f1;if(_tmp265 == 0){goto _LL594;}else{_tmp267=*_tmp265;_LL617: _tmp268=(
struct Cyc_List_List*)_tmp267.v;_tmp26A=(struct Cyc_List_List**)&(*_tmp24A.f1).v;
goto _LL609;}_LL609: _tmp260=_tmp24A.f2;if(_tmp260 == 0){goto _LL594;}else{_tmp262=*
_tmp260;_LL612: _tmp263=(struct Cyc_Absyn_Tuniondecl**)_tmp262.v;goto _LL593;}
_LL594: _LL625: _tmp270=_tmp24A.f1;if(_tmp270 == 0){goto _LL620;}else{goto _LL587;}
_LL620: _tmp26B=_tmp24A.f2;if(_tmp26B == 0){goto _LL587;}else{_tmp26D=*_tmp26B;
_LL623: _tmp26E=(struct Cyc_Absyn_Tuniondecl**)_tmp26D.v;goto _LL595;}_LL589: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl**
v))Cyc_Dict_insert)(ge->tuniondecls,v,({struct Cyc_Absyn_Tuniondecl**_tmp272=
_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl*));_tmp272[0]=tud;_tmp272;}));goto
_LL587;_LL591: {struct Cyc_Absyn_Tuniondecl**_tmp273=({struct Cyc_Absyn_Tuniondecl**
_tmp274=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl*));_tmp274[0]=({struct Cyc_Absyn_Tuniondecl*
_tmp275=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));_tmp275->sc=(void*)((void*)
Cyc_Absyn_Extern);_tmp275->name=tud->name;_tmp275->tvs=tvs;_tmp275->fields=0;
_tmp275->is_xtunion=tud->is_xtunion;_tmp275;});_tmp274;});ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl**
v))Cyc_Dict_insert)(ge->tuniondecls,v,_tmp273);*_tmp25F=Cyc_Tc_tcTunionFields(te,
ge,loc,obj,tud->is_xtunion,tud->name,*_tmp25F,tvs,tud);*_tmp273=tud;goto _LL587;}
_LL593: {struct Cyc_Absyn_Tuniondecl*_tmp276=*_tmp263;*_tmp263=({struct Cyc_Absyn_Tuniondecl*
_tmp277=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));_tmp277->sc=(void*)((void*)
Cyc_Absyn_Extern);_tmp277->name=tud->name;_tmp277->tvs=tvs;_tmp277->fields=0;
_tmp277->is_xtunion=tud->is_xtunion;_tmp277;});*_tmp26A=Cyc_Tc_tcTunionFields(te,
ge,loc,obj,tud->is_xtunion,tud->name,*_tmp26A,tvs,tud);*_tmp263=_tmp276;_tmp26E=
_tmp263;goto _LL595;}_LL595: {struct Cyc_Absyn_Tuniondecl*_tmp278=Cyc_Tcdecl_merge_tuniondecl(*
_tmp26E,tud,loc,Cyc_Tc_tc_msg);if(_tmp278 == 0){return;}else{*_tmp26E=(struct Cyc_Absyn_Tuniondecl*)
_check_null(_tmp278);goto _LL587;}}_LL587:;}}}}void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Genv*ge,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl*
ed){struct _tagged_arr*v=(*ed->name).f2;{void*_tmp279=(*ed->name).f1;struct Cyc_List_List*
_tmp281;struct Cyc_List_List*_tmp283;_LL635: if((unsigned int)_tmp279 > 1?*((int*)
_tmp279)== Cyc_Absyn_Rel_n: 0){_LL642: _tmp281=((struct Cyc_Absyn_Rel_n_struct*)
_tmp279)->f1;if(_tmp281 == 0){goto _LL636;}else{goto _LL637;}}else{goto _LL637;}
_LL637: if((unsigned int)_tmp279 > 1?*((int*)_tmp279)== Cyc_Absyn_Abs_n: 0){_LL644:
_tmp283=((struct Cyc_Absyn_Abs_n_struct*)_tmp279)->f1;if(_tmp283 == 0){goto _LL638;}
else{goto _LL639;}}else{goto _LL639;}_LL639: goto _LL640;_LL636: goto _LL634;_LL638:
goto _LL634;_LL640:({struct Cyc_Std_String_pa_struct _tmp286;_tmp286.tag=Cyc_Std_String_pa;
_tmp286.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(ed->name);{void*_tmp285[1]={&
_tmp286};Cyc_Tcutil_terr(loc,_tag_arr("qualified enum declarations are not implemented (%s)",
sizeof(unsigned char),53),_tag_arr(_tmp285,sizeof(void*),1));}});return;_LL634:;}(*
ed->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*_tmp287=_cycalloc(sizeof(
struct Cyc_Absyn_Abs_n_struct));_tmp287[0]=({struct Cyc_Absyn_Abs_n_struct _tmp288;
_tmp288.tag=Cyc_Absyn_Abs_n;_tmp288.f1=te->ns;_tmp288;});_tmp287;});if(ed->fields
!= 0){struct _RegionHandle _tmp289=_new_region("uprev_rgn");struct _RegionHandle*
uprev_rgn=& _tmp289;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*
_tmp28A=(struct Cyc_Absyn_Enumfield*)fs->hd;if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,(*_tmp28A->name).f2)){({struct Cyc_Std_String_pa_struct
_tmp28C;_tmp28C.tag=Cyc_Std_String_pa;_tmp28C.f1=(struct _tagged_arr)*(*_tmp28A->name).f2;{
void*_tmp28B[1]={& _tmp28C};Cyc_Tcutil_terr(_tmp28A->loc,_tag_arr("duplicate field name %s",
sizeof(unsigned char),24),_tag_arr(_tmp28B,sizeof(void*),1));}});}else{
prev_fields=({struct Cyc_List_List*_tmp28D=_region_malloc(uprev_rgn,sizeof(struct
Cyc_List_List));_tmp28D->hd=(*_tmp28A->name).f2;_tmp28D->tl=prev_fields;_tmp28D;});}
if(_tmp28A->tag == 0){_tmp28A->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmp28A->loc);}else{if(! Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp28A->tag))){({struct Cyc_Std_String_pa_struct _tmp290;_tmp290.tag=
Cyc_Std_String_pa;_tmp290.f1=(struct _tagged_arr)*(*_tmp28A->name).f2;{struct Cyc_Std_String_pa_struct
_tmp28F;_tmp28F.tag=Cyc_Std_String_pa;_tmp28F.f1=(struct _tagged_arr)*v;{void*
_tmp28E[2]={& _tmp28F,& _tmp290};Cyc_Tcutil_terr(loc,_tag_arr("enum %s, field %s: expression is not constant",
sizeof(unsigned char),46),_tag_arr(_tmp28E,sizeof(void*),2));}}});}}{
unsigned int t1=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp28A->tag));tag_count=t1 + 1;(*_tmp28A->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp291=_cycalloc(sizeof(struct Cyc_Absyn_Abs_n_struct));_tmp291[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp292;_tmp292.tag=Cyc_Absyn_Abs_n;_tmp292.f1=te->ns;_tmp292;});_tmp291;});}}};
_pop_region(uprev_rgn);}{struct _handler_cons _tmp293;_push_handler(& _tmp293);{int
_tmp295=0;if(setjmp(_tmp293.handler)){_tmp295=1;}if(! _tmp295){{struct Cyc_Absyn_Enumdecl**
_tmp296=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*
k))Cyc_Dict_lookup)(ge->enumdecls,v);struct Cyc_Absyn_Enumdecl*_tmp297=Cyc_Tcdecl_merge_enumdecl(*
_tmp296,ed,loc,Cyc_Tc_tc_msg);if(_tmp297 == 0){_npop_handler(0);return;}*_tmp296=(
struct Cyc_Absyn_Enumdecl*)_check_null(_tmp297);};_pop_handler();}else{void*
_tmp294=(void*)_exn_thrown;void*_tmp299=_tmp294;_LL667: if(_tmp299 == Cyc_Dict_Absent){
goto _LL668;}else{goto _LL669;}_LL669: goto _LL670;_LL668: {struct Cyc_Absyn_Enumdecl**
_tmp29F=({struct Cyc_Absyn_Enumdecl**_tmp2A0=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl*));
_tmp2A0[0]=ed;_tmp2A0;});ge->enumdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,
v,_tmp29F);goto _LL666;}_LL670:(void)_throw(_tmp299);_LL666:;}}}if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
ed->fields))->v;for(0;fs != 0;fs=fs->tl){struct Cyc_Absyn_Enumfield*_tmp2A1=(
struct Cyc_Absyn_Enumfield*)fs->hd;ge->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(ge->ordinaries,(*
_tmp2A1->name).f2,({struct _tuple4*_tmp2A2=_cycalloc(sizeof(struct _tuple4));
_tmp2A2->f1=(void*)({struct Cyc_Tcenv_EnumRes_struct*_tmp2A3=_cycalloc(sizeof(
struct Cyc_Tcenv_EnumRes_struct));_tmp2A3[0]=({struct Cyc_Tcenv_EnumRes_struct
_tmp2A4;_tmp2A4.tag=Cyc_Tcenv_EnumRes;_tmp2A4.f1=ed;_tmp2A4.f2=_tmp2A1;_tmp2A4;});
_tmp2A3;});_tmp2A2->f2=1;_tmp2A2;}));}}}static int Cyc_Tc_okay_externC(struct Cyc_Position_Segment*
loc,void*sc){void*_tmp2A5=sc;_LL679: if(_tmp2A5 == (void*)Cyc_Absyn_Static){goto
_LL680;}else{goto _LL681;}_LL681: if(_tmp2A5 == (void*)Cyc_Absyn_Abstract){goto
_LL682;}else{goto _LL683;}_LL683: if(_tmp2A5 == (void*)Cyc_Absyn_Public){goto _LL684;}
else{goto _LL685;}_LL685: if(_tmp2A5 == (void*)Cyc_Absyn_Extern){goto _LL686;}else{
goto _LL687;}_LL687: if(_tmp2A5 == (void*)Cyc_Absyn_ExternC){goto _LL688;}else{goto
_LL678;}_LL680:({void*_tmp2B1[0]={};Cyc_Tcutil_warn(loc,_tag_arr("static declaration nested within extern \"C\"",
sizeof(unsigned char),44),_tag_arr(_tmp2B1,sizeof(void*),0));});return 0;_LL682:({
void*_tmp2B2[0]={};Cyc_Tcutil_warn(loc,_tag_arr("abstract declaration nested within extern \"C\"",
sizeof(unsigned char),46),_tag_arr(_tmp2B2,sizeof(void*),0));});return 0;_LL684:
return 1;_LL686: return 1;_LL688:({void*_tmp2B3[0]={};Cyc_Tcutil_warn(loc,_tag_arr("nested extern \"C\" declaration",
sizeof(unsigned char),30),_tag_arr(_tmp2B3,sizeof(void*),0));});return 1;_LL678:;}
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int
in_externC,int check_var_init){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;struct Cyc_Dict_Dict*dict=Cyc_Tcgenrep_empty_typerep_dict();
struct Cyc_List_List*_tmp2B4=ds0;for(0;_tmp2B4 != 0;(last=_tmp2B4,_tmp2B4=_tmp2B4->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp2B4->hd;struct Cyc_Position_Segment*
loc=d->loc;void*_tmp2B5=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp2CF;struct Cyc_Absyn_Fndecl*
_tmp2D1;struct Cyc_Absyn_Typedefdecl*_tmp2D3;struct Cyc_Absyn_Structdecl*_tmp2D5;
struct Cyc_Absyn_Uniondecl*_tmp2D7;struct Cyc_Absyn_Tuniondecl*_tmp2D9;struct Cyc_Absyn_Enumdecl*
_tmp2DB;struct Cyc_List_List*_tmp2DD;struct _tagged_arr*_tmp2DF;struct Cyc_List_List*
_tmp2E1;struct _tuple0*_tmp2E3;struct _tuple0 _tmp2E5;struct _tagged_arr*_tmp2E6;
void*_tmp2E8;struct Cyc_List_List*_tmp2EA;_LL695: if(*((int*)_tmp2B5)== Cyc_Absyn_Let_d){
goto _LL696;}else{goto _LL697;}_LL697: if(*((int*)_tmp2B5)== Cyc_Absyn_Letv_d){goto
_LL698;}else{goto _LL699;}_LL699: if(*((int*)_tmp2B5)== Cyc_Absyn_Var_d){_LL720:
_tmp2CF=((struct Cyc_Absyn_Var_d_struct*)_tmp2B5)->f1;goto _LL700;}else{goto _LL701;}
_LL701: if(*((int*)_tmp2B5)== Cyc_Absyn_Fn_d){_LL722: _tmp2D1=((struct Cyc_Absyn_Fn_d_struct*)
_tmp2B5)->f1;goto _LL702;}else{goto _LL703;}_LL703: if(*((int*)_tmp2B5)== Cyc_Absyn_Typedef_d){
_LL724: _tmp2D3=((struct Cyc_Absyn_Typedef_d_struct*)_tmp2B5)->f1;goto _LL704;}
else{goto _LL705;}_LL705: if(*((int*)_tmp2B5)== Cyc_Absyn_Struct_d){_LL726: _tmp2D5=((
struct Cyc_Absyn_Struct_d_struct*)_tmp2B5)->f1;goto _LL706;}else{goto _LL707;}
_LL707: if(*((int*)_tmp2B5)== Cyc_Absyn_Union_d){_LL728: _tmp2D7=((struct Cyc_Absyn_Union_d_struct*)
_tmp2B5)->f1;goto _LL708;}else{goto _LL709;}_LL709: if(*((int*)_tmp2B5)== Cyc_Absyn_Tunion_d){
_LL730: _tmp2D9=((struct Cyc_Absyn_Tunion_d_struct*)_tmp2B5)->f1;goto _LL710;}else{
goto _LL711;}_LL711: if(*((int*)_tmp2B5)== Cyc_Absyn_Enum_d){_LL732: _tmp2DB=((
struct Cyc_Absyn_Enum_d_struct*)_tmp2B5)->f1;goto _LL712;}else{goto _LL713;}_LL713:
if(*((int*)_tmp2B5)== Cyc_Absyn_Namespace_d){_LL736: _tmp2DF=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp2B5)->f1;goto _LL734;_LL734: _tmp2DD=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp2B5)->f2;goto _LL714;}else{goto _LL715;}_LL715: if(*((int*)_tmp2B5)== Cyc_Absyn_Using_d){
_LL740: _tmp2E3=((struct Cyc_Absyn_Using_d_struct*)_tmp2B5)->f1;_tmp2E5=*_tmp2E3;
_LL745: _tmp2E8=_tmp2E5.f1;goto _LL743;_LL743: _tmp2E6=_tmp2E5.f2;goto _LL738;_LL738:
_tmp2E1=((struct Cyc_Absyn_Using_d_struct*)_tmp2B5)->f2;goto _LL716;}else{goto
_LL717;}_LL717: if(*((int*)_tmp2B5)== Cyc_Absyn_ExternC_d){_LL747: _tmp2EA=((
struct Cyc_Absyn_ExternC_d_struct*)_tmp2B5)->f1;goto _LL718;}else{goto _LL694;}
_LL696: goto _LL698;_LL698:({void*_tmp2EC[0]={};Cyc_Tcutil_terr(loc,_tag_arr("top level let-declarations are not implemented",
sizeof(unsigned char),47),_tag_arr(_tmp2EC,sizeof(void*),0));});goto _LL694;
_LL700: if(in_externC? Cyc_Tc_okay_externC(d->loc,(void*)_tmp2CF->sc): 0){(void*)(
_tmp2CF->sc=(void*)((void*)Cyc_Absyn_ExternC));}if(_tmp2CF->initializer != 0){
void*_tmp2ED=(void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp2CF->initializer))->r;
void*_tmp2F3;_LL751: if(*((int*)_tmp2ED)== Cyc_Absyn_Gentyp_e){_LL756: _tmp2F3=(
void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp2ED)->f2;goto _LL752;}else{goto
_LL753;}_LL753: goto _LL754;_LL752: {struct Cyc_Absyn_Exp*_tmp2F7;struct Cyc_List_List*
_tmp2F9;struct Cyc_Dict_Dict*_tmp2FB;struct _tuple3 _tmp2F5=Cyc_Tcgenrep_tcGenrep(
te,ge,loc,_tmp2F3,dict);_LL764: _tmp2FB=_tmp2F5.f1;goto _LL762;_LL762: _tmp2F9=
_tmp2F5.f2;goto _LL760;_LL760: _tmp2F7=_tmp2F5.f3;goto _LL758;_LL758: dict=_tmp2FB;
Cyc_Tc_tc_decls(te,_tmp2F9,in_externC,check_var_init);_tmp2CF->initializer=(
struct Cyc_Absyn_Exp*)_tmp2F7;Cyc_Tc_tcVardecl(te,ge,loc,_tmp2CF,check_var_init);
if(_tmp2F9 != 0){if(last != 0){((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp2F9,_tmp2B4);((struct Cyc_List_List*)
_check_null(last))->tl=_tmp2F9;}else{struct Cyc_List_List tmp=({struct Cyc_List_List
_tmp2FD;_tmp2FD.hd=(struct Cyc_Absyn_Decl*)_tmp2B4->hd;_tmp2FD.tl=_tmp2B4->tl;
_tmp2FD;});(struct Cyc_Absyn_Decl*)(_tmp2B4->hd=(void*)((struct Cyc_Absyn_Decl*)
_tmp2F9->hd));_tmp2B4->tl=_tmp2F9->tl;(struct Cyc_Absyn_Decl*)(_tmp2F9->hd=(void*)((
struct Cyc_Absyn_Decl*)tmp.hd));_tmp2F9->tl=tmp.tl;((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp2B4,
_tmp2F9);}}continue;}_LL754: goto _LL750;_LL750:;}Cyc_Tc_tcVardecl(te,ge,loc,
_tmp2CF,check_var_init);goto _LL694;_LL702: if(in_externC? Cyc_Tc_okay_externC(d->loc,(
void*)_tmp2D1->sc): 0){(void*)(_tmp2D1->sc=(void*)((void*)Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl(te,ge,loc,_tmp2D1);goto _LL694;_LL704: Cyc_Tc_tcTypedefdecl(te,ge,
loc,_tmp2D3);goto _LL694;_LL706: if(in_externC? Cyc_Tc_okay_externC(d->loc,(void*)
_tmp2D5->sc): 0){(void*)(_tmp2D5->sc=(void*)((void*)Cyc_Absyn_ExternC));}Cyc_Tc_tcStructdecl(
te,ge,loc,_tmp2D5);goto _LL694;_LL708: if(in_externC? Cyc_Tc_okay_externC(d->loc,(
void*)_tmp2D7->sc): 0){(void*)(_tmp2D7->sc=(void*)((void*)Cyc_Absyn_ExternC));}
Cyc_Tc_tcUniondecl(te,ge,loc,_tmp2D7);goto _LL694;_LL710: if(in_externC? Cyc_Tc_okay_externC(
d->loc,(void*)_tmp2D9->sc): 0){(void*)(_tmp2D9->sc=(void*)((void*)Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl(te,ge,loc,_tmp2D9);goto _LL694;_LL712: if(in_externC? Cyc_Tc_okay_externC(
d->loc,(void*)_tmp2DB->sc): 0){(void*)(_tmp2DB->sc=(void*)((void*)Cyc_Absyn_ExternC));}
Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp2DB);goto _LL694;_LL714: {struct Cyc_List_List*
_tmp2FE=te->ns;struct Cyc_List_List*_tmp2FF=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2FE,({struct Cyc_List_List*_tmp300=
_cycalloc(sizeof(struct Cyc_List_List));_tmp300->hd=_tmp2DF;_tmp300->tl=0;_tmp300;}));
if(!((int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(ge->namespaces,
_tmp2DF)){ge->namespaces=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_tagged_arr*elt))Cyc_Set_insert)(ge->namespaces,_tmp2DF);te->ae=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(
te->ae,_tmp2FF,Cyc_Tcenv_empty_genv());}te->ns=_tmp2FF;Cyc_Tc_tc_decls(te,
_tmp2DD,in_externC,check_var_init);te->ns=_tmp2FE;goto _LL694;}_LL716: {struct
_tagged_arr*first;struct Cyc_List_List*rest;{void*_tmp301=_tmp2E8;struct Cyc_List_List*
_tmp30D;struct Cyc_List_List*_tmp30F;struct Cyc_List_List*_tmp311;struct Cyc_List_List
_tmp313;struct Cyc_List_List*_tmp314;struct _tagged_arr*_tmp316;struct Cyc_List_List*
_tmp318;struct Cyc_List_List _tmp31A;struct Cyc_List_List*_tmp31B;struct _tagged_arr*
_tmp31D;_LL771: if(_tmp301 == (void*)Cyc_Absyn_Loc_n){goto _LL772;}else{goto _LL773;}
_LL773: if((unsigned int)_tmp301 > 1?*((int*)_tmp301)== Cyc_Absyn_Rel_n: 0){_LL782:
_tmp30D=((struct Cyc_Absyn_Rel_n_struct*)_tmp301)->f1;if(_tmp30D == 0){goto _LL774;}
else{goto _LL775;}}else{goto _LL775;}_LL775: if((unsigned int)_tmp301 > 1?*((int*)
_tmp301)== Cyc_Absyn_Abs_n: 0){_LL784: _tmp30F=((struct Cyc_Absyn_Abs_n_struct*)
_tmp301)->f1;if(_tmp30F == 0){goto _LL776;}else{goto _LL777;}}else{goto _LL777;}
_LL777: if((unsigned int)_tmp301 > 1?*((int*)_tmp301)== Cyc_Absyn_Rel_n: 0){_LL786:
_tmp311=((struct Cyc_Absyn_Rel_n_struct*)_tmp301)->f1;if(_tmp311 == 0){goto _LL779;}
else{_tmp313=*_tmp311;_LL791: _tmp316=(struct _tagged_arr*)_tmp313.hd;goto _LL789;
_LL789: _tmp314=_tmp313.tl;goto _LL778;}}else{goto _LL779;}_LL779: if((unsigned int)
_tmp301 > 1?*((int*)_tmp301)== Cyc_Absyn_Abs_n: 0){_LL793: _tmp318=((struct Cyc_Absyn_Abs_n_struct*)
_tmp301)->f1;if(_tmp318 == 0){goto _LL770;}else{_tmp31A=*_tmp318;_LL798: _tmp31D=(
struct _tagged_arr*)_tmp31A.hd;goto _LL796;_LL796: _tmp31B=_tmp31A.tl;goto _LL780;}}
else{goto _LL770;}_LL772: goto _LL774;_LL774: goto _LL776;_LL776: first=_tmp2E6;rest=0;
goto _LL770;_LL778: _tmp31D=_tmp316;_tmp31B=_tmp314;goto _LL780;_LL780: first=
_tmp31D;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp31B,({struct Cyc_List_List*_tmp31F=_cycalloc(sizeof(
struct Cyc_List_List));_tmp31F->hd=_tmp2E6;_tmp31F->tl=0;_tmp31F;}));goto _LL770;
_LL770:;}{struct Cyc_List_List*_tmp320=Cyc_Tcenv_resolve_namespace(te,loc,first,
rest);ge->availables=({struct Cyc_List_List*_tmp321=_cycalloc(sizeof(struct Cyc_List_List));
_tmp321->hd=_tmp320;_tmp321->tl=ge->availables;_tmp321;});Cyc_Tc_tc_decls(te,
_tmp2E1,in_externC,check_var_init);ge->availables=((struct Cyc_List_List*)
_check_null(ge->availables))->tl;goto _LL694;}}_LL718: Cyc_Tc_tc_decls(te,_tmp2EA,
1,check_var_init);goto _LL694;_LL694:;}}void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int
check_var_init,struct Cyc_List_List*ds){Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,check_var_init);}static int Cyc_Tc_vardecl_needed(struct
Cyc_Dict_Dict*env,struct Cyc_Absyn_Decl*d);static struct Cyc_List_List*Cyc_Tc_treeshake_f(
struct Cyc_Dict_Dict*env,struct Cyc_List_List*ds){return((struct Cyc_List_List*(*)(
int(*f)(struct Cyc_Dict_Dict*,struct Cyc_Absyn_Decl*),struct Cyc_Dict_Dict*env,
struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}struct
_tuple7{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};static int Cyc_Tc_vardecl_needed(
struct Cyc_Dict_Dict*env,struct Cyc_Absyn_Decl*d){void*_tmp322=(void*)d->r;struct
Cyc_Absyn_Vardecl*_tmp32C;struct Cyc_List_List*_tmp32E;struct Cyc_List_List**
_tmp330;struct Cyc_List_List*_tmp331;struct Cyc_List_List**_tmp333;_LL804: if(*((
int*)_tmp322)== Cyc_Absyn_Var_d){_LL813: _tmp32C=((struct Cyc_Absyn_Var_d_struct*)
_tmp322)->f1;goto _LL805;}else{goto _LL806;}_LL806: if(*((int*)_tmp322)== Cyc_Absyn_Using_d){
_LL815: _tmp32E=((struct Cyc_Absyn_Using_d_struct*)_tmp322)->f2;_tmp330=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Using_d_struct*)_tmp322)->f2;goto _LL807;}else{goto _LL808;}_LL808:
if(*((int*)_tmp322)== Cyc_Absyn_Namespace_d){_LL818: _tmp331=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp322)->f2;_tmp333=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_struct*)
_tmp322)->f2;goto _LL809;}else{goto _LL810;}_LL810: goto _LL811;_LL805: if((void*)
_tmp32C->sc != (void*)Cyc_Absyn_Extern?(void*)_tmp32C->sc != (void*)Cyc_Absyn_ExternC:
0){return 1;}{struct _tuple0 _tmp336;struct _tagged_arr*_tmp337;void*_tmp339;struct
_tuple0*_tmp334=_tmp32C->name;_tmp336=*_tmp334;_LL826: _tmp339=_tmp336.f1;goto
_LL824;_LL824: _tmp337=_tmp336.f2;goto _LL821;_LL821: {struct Cyc_List_List*ns;{
void*_tmp33B=_tmp339;struct Cyc_List_List*_tmp343;struct Cyc_List_List*_tmp345;
_LL829: if(_tmp33B == (void*)Cyc_Absyn_Loc_n){goto _LL830;}else{goto _LL831;}_LL831:
if((unsigned int)_tmp33B > 1?*((int*)_tmp33B)== Cyc_Absyn_Rel_n: 0){_LL836: _tmp343=((
struct Cyc_Absyn_Rel_n_struct*)_tmp33B)->f1;goto _LL832;}else{goto _LL833;}_LL833:
if((unsigned int)_tmp33B > 1?*((int*)_tmp33B)== Cyc_Absyn_Abs_n: 0){_LL838: _tmp345=((
struct Cyc_Absyn_Abs_n_struct*)_tmp33B)->f1;goto _LL834;}else{goto _LL828;}_LL830:
ns=0;goto _LL828;_LL832: ns=_tmp343;goto _LL828;_LL834: ns=_tmp345;goto _LL828;_LL828:;}{
struct _tuple7*_tmp347=((struct _tuple7*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(env,ns);struct Cyc_Tcenv_Genv*_tmp348=(*_tmp347).f1;int
_tmp349=(*((struct _tuple4*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp348->ordinaries,_tmp337)).f2;if(! _tmp349){(*_tmp347).f2=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_insert)((*_tmp347).f2,_tmp337);}
return _tmp349;}}}_LL807: _tmp333=_tmp330;goto _LL809;_LL809:*_tmp333=Cyc_Tc_treeshake_f(
env,*_tmp333);return 1;_LL811: return 1;_LL803:;}static int Cyc_Tc_treeshake_remove_f(
struct Cyc_Set_Set*set,struct _tagged_arr*x,void*y){return !((int(*)(struct Cyc_Set_Set*
s,struct _tagged_arr*elt))Cyc_Set_member)(set,x);}static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List*ns,struct _tuple7*nsenv){struct _tuple7 _tmp34C;struct Cyc_Set_Set*
_tmp34D;struct Cyc_Tcenv_Genv*_tmp34F;struct _tuple7*_tmp34A=nsenv;_tmp34C=*
_tmp34A;_LL848: _tmp34F=_tmp34C.f1;goto _LL846;_LL846: _tmp34D=_tmp34C.f2;goto
_LL843;_LL843: _tmp34F->ordinaries=((struct Cyc_Dict_Dict*(*)(int(*f)(struct Cyc_Set_Set*,
struct _tagged_arr*,struct _tuple4*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict*d))
Cyc_Dict_filter_c)((int(*)(struct Cyc_Set_Set*set,struct _tagged_arr*x,struct
_tuple4*y))Cyc_Tc_treeshake_remove_f,_tmp34D,_tmp34F->ordinaries);}static struct
_tuple7*Cyc_Tc_treeshake_make_env_f(struct Cyc_Tcenv_Genv*ge){return({struct
_tuple7*_tmp351=_cycalloc(sizeof(struct _tuple7));_tmp351->f1=ge;_tmp351->f2=((
struct Cyc_Set_Set*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(
Cyc_Std_strptrcmp);_tmp351;});}struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*ds){struct Cyc_Dict_Dict*_tmp352=((struct Cyc_Dict_Dict*(*)(
struct _tuple7*(*f)(struct Cyc_Tcenv_Genv*),struct Cyc_Dict_Dict*d))Cyc_Dict_map)(
Cyc_Tc_treeshake_make_env_f,te->ae);struct Cyc_List_List*_tmp353=Cyc_Tc_treeshake_f(
_tmp352,ds);((void(*)(void(*f)(struct Cyc_List_List*,struct _tuple7*),struct Cyc_Dict_Dict*
d))Cyc_Dict_iter)(Cyc_Tc_treeshake_remove,_tmp352);return _tmp353;}
