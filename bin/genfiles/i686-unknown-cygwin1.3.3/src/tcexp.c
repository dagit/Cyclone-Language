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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};struct
_tuple0{void*f1;void*f2;};extern void*Cyc_Core_snd(struct _tuple0*);extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern int Cyc_List_length(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern
unsigned char Cyc_List_List_mismatch[18];extern struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_rappend(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern unsigned char
Cyc_List_Nth[8];extern void*Cyc_List_nth(struct Cyc_List_List*x,int n);extern struct
Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);extern int Cyc_List_exists_c(
int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);extern struct Cyc_List_List*
Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;
struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;static const int Cyc_Position_Lex=0;static const int Cyc_Position_Parse=
1;static const int Cyc_Position_Elab=2;struct Cyc_Position_Error{struct _tagged_arr
source;struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
unsigned char Cyc_Position_Nocontext[14];static const int Cyc_Absyn_Loc_n=0;static
const int Cyc_Absyn_Rel_n=0;struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
f1;};static const int Cyc_Absyn_Abs_n=1;struct Cyc_Absyn_Abs_n_struct{int tag;struct
Cyc_List_List*f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;
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
_tuple1*name;int is_xtunion;};static const int Cyc_Absyn_UnknownTunion=0;struct Cyc_Absyn_UnknownTunion_struct{
int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};static const int Cyc_Absyn_KnownTunion=
1;struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};
struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*targs;void*rgn;}
;struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*tunion_name;struct _tuple1*
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
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};
static const int Cyc_Absyn_UnionType=11;struct Cyc_Absyn_UnionType_struct{int tag;
struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};static
const int Cyc_Absyn_EnumType=12;struct Cyc_Absyn_EnumType_struct{int tag;struct
_tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};static const int Cyc_Absyn_SizeofType=13;
struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};static const int Cyc_Absyn_AnonStructType=
14;struct Cyc_Absyn_AnonStructType_struct{int tag;struct Cyc_List_List*f1;};static
const int Cyc_Absyn_AnonUnionType=15;struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};static const int Cyc_Absyn_AnonEnumType=16;struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_RgnHandleType=17;struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};static const int Cyc_Absyn_TypedefType=
18;struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
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
int Cyc_Absyn_Var_e=1;struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*f1;void*
f2;};static const int Cyc_Absyn_UnknownId_e=2;struct Cyc_Absyn_UnknownId_e_struct{
int tag;struct _tuple1*f1;};static const int Cyc_Absyn_Primop_e=3;struct Cyc_Absyn_Primop_e_struct{
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
struct Cyc_List_List*f1;};static const int Cyc_Absyn_CompoundLit_e=25;struct _tuple2{
struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Array_e=
26;struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};static const int
Cyc_Absyn_Comprehension_e=27;struct Cyc_Absyn_Comprehension_e_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
static const int Cyc_Absyn_Struct_e=28;struct Cyc_Absyn_Struct_e_struct{int tag;
struct _tuple1*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};static const int Cyc_Absyn_AnonStruct_e=29;struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_Tunion_e=30;
struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};static const int Cyc_Absyn_Enum_e=31;struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
static const int Cyc_Absyn_AnonEnum_e=32;struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static const int Cyc_Absyn_Malloc_e=
33;struct Cyc_Absyn_Malloc_e_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};static
const int Cyc_Absyn_UnresolvedMem_e=34;struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};static const int Cyc_Absyn_StmtExp_e=
35;struct Cyc_Absyn_StmtExp_e_struct{int tag;struct Cyc_Absyn_Stmt*f1;};static const
int Cyc_Absyn_Codegen_e=36;struct Cyc_Absyn_Codegen_e_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};static const int Cyc_Absyn_Fill_e=37;struct Cyc_Absyn_Fill_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;
struct Cyc_Position_Segment*loc;void*annot;};struct _tuple3{struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;
struct _tuple3 condition;struct _tuple3 delta;struct Cyc_Absyn_Stmt*body;};static
const int Cyc_Absyn_Skip_s=0;static const int Cyc_Absyn_Exp_s=0;struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_Seq_s=1;struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_Return_s=
2;struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};static const int
Cyc_Absyn_IfThenElse_s=3;struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};static const int Cyc_Absyn_While_s=
4;struct Cyc_Absyn_While_s_struct{int tag;struct _tuple3 f1;struct Cyc_Absyn_Stmt*f2;
};static const int Cyc_Absyn_Break_s=5;struct Cyc_Absyn_Break_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};static const int Cyc_Absyn_Continue_s=6;struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};static const int Cyc_Absyn_Goto_s=7;struct Cyc_Absyn_Goto_s_struct{
int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};static const int Cyc_Absyn_For_s=
8;struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple3 f2;
struct _tuple3 f3;struct Cyc_Absyn_Stmt*f4;};static const int Cyc_Absyn_Switch_s=9;
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
struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple3 f2;};
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
int tag;struct _tuple1*f1;};static const int Cyc_Absyn_UnknownCall_p=12;struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};static
const int Cyc_Absyn_UnknownFields_p=13;struct Cyc_Absyn_UnknownFields_p_struct{int
tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{
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
_tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Structfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{
void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple1*name;struct Cyc_List_List*tvs;void*defn;};static const int Cyc_Absyn_Var_d=0;
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
10;struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;
};static const int Cyc_Absyn_ExternC_d=11;struct Cyc_Absyn_ExternC_d_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};static const int Cyc_Absyn_ArrayElement=0;struct Cyc_Absyn_ArrayElement_struct{
int tag;struct Cyc_Absyn_Exp*f1;};static const int Cyc_Absyn_FieldName=1;struct Cyc_Absyn_FieldName_struct{
int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[15];extern
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual();extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_empty_conref();extern struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*
x);extern void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern void*
Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_wildtyp(
struct Cyc_Core_Opt*);extern void*Cyc_Absyn_uchar_t;extern void*Cyc_Absyn_ushort_t;
extern void*Cyc_Absyn_uint_t;extern void*Cyc_Absyn_ulonglong_t;extern void*Cyc_Absyn_schar_t;
extern void*Cyc_Absyn_sshort_t;extern void*Cyc_Absyn_sint_t;extern void*Cyc_Absyn_slonglong_t;
extern void*Cyc_Absyn_float_t;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_atb_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b);extern void*Cyc_Absyn_star_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern void*Cyc_Absyn_at_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq);extern void*Cyc_Absyn_tagged_typ(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq);extern struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct
Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct
Cyc_Position_Segment*);extern void*Cyc_Absyn_pointer_expand(void*);extern int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Structfield*Cyc_Absyn_lookup_field(
struct Cyc_List_List*,struct _tagged_arr*);extern struct Cyc_Absyn_Structfield*Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*,struct _tagged_arr*);extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field(struct Cyc_Absyn_Uniondecl*,struct _tagged_arr*);
extern int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);extern int Cyc_Std_zstrcmp(
struct _tagged_arr,struct _tagged_arr);struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
static const int Cyc_Std_String_pa=0;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};static const int Cyc_Std_Int_pa=1;struct Cyc_Std_Int_pa_struct{int
tag;unsigned int f1;};static const int Cyc_Std_Double_pa=2;struct Cyc_Std_Double_pa_struct{
int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=3;struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=4;struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};extern struct _tagged_arr Cyc_Std_vrprintf(struct
_RegionHandle*r1,struct _tagged_arr fmt,struct _tagged_arr ap);static const int Cyc_Std_ShortPtr_sa=
0;struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};static const int Cyc_Std_UShortPtr_sa=
1;struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=
2;struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=
3;struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=
4;struct Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};static const int
Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};
static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{int tag;
float*f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(void*);extern
struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);extern struct
_tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Set_Set;extern
unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];static const int Cyc_Tcenv_VarRes=0;
struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};static const int Cyc_Tcenv_StructRes=
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
ae;struct Cyc_Core_Opt*le;};extern void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);extern struct Cyc_Absyn_Structdecl**
Cyc_Tcenv_lookup_structdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);extern struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);extern struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*);extern struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);extern struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct
Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*);extern void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
extern void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*rgn);extern void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*eff);extern void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(struct
_tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern int Cyc_Tcutil_kind_leq(void*k1,
void*k2);extern void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);extern void*Cyc_Tcutil_typ_kind(
void*t);extern void*Cyc_Tcutil_compress(void*t);extern void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);extern int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);extern int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);extern int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);extern int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);extern int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);extern int Cyc_Tcutil_coerce_sint_typ(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);extern int Cyc_Tcutil_coerceable(void*);
extern int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);extern int Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*,void*);extern int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);extern int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt
Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
extern int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*
e);extern void*Cyc_Tcutil_max_arithmetic_type(void*,void*);extern void Cyc_Tcutil_explain_failure();
extern int Cyc_Tcutil_unify(void*,void*);extern void*Cyc_Tcutil_substitute(struct
Cyc_List_List*,void*);extern void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);extern void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
struct _tuple4{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple5{
struct Cyc_Absyn_Tvar*f1;void*f2;};extern struct _tuple5*Cyc_Tcutil_r_make_inst_var(
struct _tuple4*,struct Cyc_Absyn_Tvar*);extern void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);extern void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);
extern void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);
extern void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,unsigned int i,
struct Cyc_Absyn_Conref*);extern int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*
b);extern struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields);extern int Cyc_Tcutil_is_tagged_pointer_typ(void*);extern int
Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);struct _tuple6{
int f1;void*f2;};extern struct _tuple6 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);extern int Cyc_Tcutil_bits_only(void*t);extern int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);extern int Cyc_Evexp_okay_szofarg(void*t);extern void Cyc_Tcstmt_tcStmt(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);extern struct Cyc_List_List*
Cyc_Formatstr_get_format_typs(struct Cyc_Tcenv_Tenv*,struct _tagged_arr,struct Cyc_Position_Segment*);
extern struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct Cyc_Tcenv_Tenv*,
struct _tagged_arr,struct Cyc_Position_Segment*);extern void*Cyc_Tcexp_tcExp(struct
Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);extern void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);extern void Cyc_Tcexp_tcTest(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _tagged_arr msg_part);static
const int Cyc_CfFlowInfo_VarRoot=0;struct Cyc_CfFlowInfo_VarRoot_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_CfFlowInfo_MallocPt=1;struct Cyc_CfFlowInfo_MallocPt_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};static const int Cyc_CfFlowInfo_NoneIL=0;static const int Cyc_CfFlowInfo_ThisIL=
1;static const int Cyc_CfFlowInfo_AllIL=2;static const int Cyc_CfFlowInfo_EqualConst=0;
struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};static const int Cyc_CfFlowInfo_LessVar=
1;struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};static
const int Cyc_CfFlowInfo_LessSize=2;struct Cyc_CfFlowInfo_LessSize_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_CfFlowInfo_LessConst=3;struct Cyc_CfFlowInfo_LessConst_struct{
int tag;unsigned int f1;};static const int Cyc_CfFlowInfo_LessEqSize=4;struct Cyc_CfFlowInfo_LessEqSize_struct{
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
struct Cyc_Absyn_Stmt*encloser);static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tagged_arr msg,struct _tagged_arr ap){({
void*_tmp0[0]={};Cyc_Tcutil_terr(loc,(struct _tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,
msg,ap),_tag_arr(_tmp0,sizeof(void*),0));});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp1;}));}static void Cyc_Tcexp_resolve_unknown_id(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,struct _tuple1*q){struct _handler_cons _tmp2;_push_handler(&
_tmp2);{int _tmp4=0;if(setjmp(_tmp2.handler)){_tmp4=1;}if(! _tmp4){{void*_tmp5=Cyc_Tcenv_lookup_ordinary(
te,e->loc,q);void*_tmp11;struct Cyc_Absyn_Enumfield*_tmp13;struct Cyc_Absyn_Enumdecl*
_tmp15;struct Cyc_Absyn_Enumfield*_tmp17;void*_tmp19;struct Cyc_Absyn_Tunionfield*
_tmp1B;struct Cyc_Absyn_Tuniondecl*_tmp1D;_LL7: if(*((int*)_tmp5)== Cyc_Tcenv_VarRes){
_LL18: _tmp11=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmp5)->f1;goto _LL8;}else{
goto _LL9;}_LL9: if(*((int*)_tmp5)== Cyc_Tcenv_EnumRes){_LL22: _tmp15=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp5)->f1;goto _LL20;_LL20: _tmp13=((struct Cyc_Tcenv_EnumRes_struct*)_tmp5)->f2;
goto _LL10;}else{goto _LL11;}_LL11: if(*((int*)_tmp5)== Cyc_Tcenv_AnonEnumRes){
_LL26: _tmp19=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f1;goto _LL24;
_LL24: _tmp17=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp5)->f2;goto _LL12;}else{
goto _LL13;}_LL13: if(*((int*)_tmp5)== Cyc_Tcenv_TunionRes){_LL30: _tmp1D=((struct
Cyc_Tcenv_TunionRes_struct*)_tmp5)->f1;goto _LL28;_LL28: _tmp1B=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp5)->f2;goto _LL14;}else{goto _LL15;}_LL15: if(*((int*)_tmp5)== Cyc_Tcenv_StructRes){
goto _LL16;}else{goto _LL6;}_LL8:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*
_tmp1F=_cycalloc(sizeof(struct Cyc_Absyn_Var_e_struct));_tmp1F[0]=({struct Cyc_Absyn_Var_e_struct
_tmp20;_tmp20.tag=Cyc_Absyn_Var_e;_tmp20.f1=q;_tmp20.f2=(void*)_tmp11;_tmp20;});
_tmp1F;})));goto _LL6;_LL10:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Enum_e_struct*
_tmp21=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_struct));_tmp21[0]=({struct Cyc_Absyn_Enum_e_struct
_tmp22;_tmp22.tag=Cyc_Absyn_Enum_e;_tmp22.f1=q;_tmp22.f2=(struct Cyc_Absyn_Enumdecl*)
_tmp15;_tmp22.f3=(struct Cyc_Absyn_Enumfield*)_tmp13;_tmp22;});_tmp21;})));goto
_LL6;_LL12:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_e_struct*
_tmp23=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_struct));_tmp23[0]=({struct
Cyc_Absyn_AnonEnum_e_struct _tmp24;_tmp24.tag=Cyc_Absyn_AnonEnum_e;_tmp24.f1=q;
_tmp24.f2=(void*)_tmp19;_tmp24.f3=(struct Cyc_Absyn_Enumfield*)_tmp17;_tmp24;});
_tmp23;})));goto _LL6;_LL14:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Tunion_e_struct*
_tmp25=_cycalloc(sizeof(struct Cyc_Absyn_Tunion_e_struct));_tmp25[0]=({struct Cyc_Absyn_Tunion_e_struct
_tmp26;_tmp26.tag=Cyc_Absyn_Tunion_e;_tmp26.f1=0;_tmp26.f2=0;_tmp26.f3=0;_tmp26.f4=
_tmp1D;_tmp26.f5=_tmp1B;_tmp26;});_tmp25;})));goto _LL6;_LL16:({struct Cyc_Std_String_pa_struct
_tmp28;_tmp28.tag=Cyc_Std_String_pa;_tmp28.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
q);{void*_tmp27[1]={& _tmp28};Cyc_Tcutil_terr(e->loc,_tag_arr("bad occurrence of struct name %s",
sizeof(unsigned char),33),_tag_arr(_tmp27,sizeof(void*),1));}});(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmp29=_cycalloc(sizeof(struct Cyc_Absyn_Var_e_struct));
_tmp29[0]=({struct Cyc_Absyn_Var_e_struct _tmp2A;_tmp2A.tag=Cyc_Absyn_Var_e;_tmp2A.f1=
q;_tmp2A.f2=(void*)((void*)Cyc_Absyn_Unresolved_b);_tmp2A;});_tmp29;})));goto
_LL6;_LL6:;};_pop_handler();}else{void*_tmp3=(void*)_exn_thrown;void*_tmp2C=
_tmp3;_LL46: if(_tmp2C == Cyc_Dict_Absent){goto _LL47;}else{goto _LL48;}_LL48: goto
_LL49;_LL47:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Var_e_struct*_tmp32=
_cycalloc(sizeof(struct Cyc_Absyn_Var_e_struct));_tmp32[0]=({struct Cyc_Absyn_Var_e_struct
_tmp33;_tmp33.tag=Cyc_Absyn_Var_e;_tmp33.f1=q;_tmp33.f2=(void*)((void*)Cyc_Absyn_Unresolved_b);
_tmp33;});_tmp32;})));goto _LL45;_LL49:(void)_throw(_tmp2C);_LL45:;}}}struct
_tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple7*Cyc_Tcexp_make_struct_arg(
struct Cyc_Absyn_Exp*e){return({struct _tuple7*_tmp34=_cycalloc(sizeof(struct
_tuple7));_tmp34->f1=0;_tmp34->f2=e;_tmp34;});}static void Cyc_Tcexp_resolve_unknown_fn(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
es){void*_tmp35=(void*)e1->r;struct _tuple1*_tmp3B;_LL55: if(*((int*)_tmp35)== Cyc_Absyn_UnknownId_e){
_LL60: _tmp3B=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp35)->f1;goto _LL56;}else{
goto _LL57;}_LL57: goto _LL58;_LL56: {struct _handler_cons _tmp3D;_push_handler(&
_tmp3D);{int _tmp3F=0;if(setjmp(_tmp3D.handler)){_tmp3F=1;}if(! _tmp3F){{void*
_tmp40=Cyc_Tcenv_lookup_ordinary(te,e1->loc,_tmp3B);void*_tmp4C;struct Cyc_Absyn_Tunionfield*
_tmp4E;struct Cyc_Absyn_Tuniondecl*_tmp50;struct Cyc_Absyn_Structdecl*_tmp52;_LL66:
if(*((int*)_tmp40)== Cyc_Tcenv_VarRes){_LL77: _tmp4C=(void*)((struct Cyc_Tcenv_VarRes_struct*)
_tmp40)->f1;goto _LL67;}else{goto _LL68;}_LL68: if(*((int*)_tmp40)== Cyc_Tcenv_TunionRes){
_LL81: _tmp50=((struct Cyc_Tcenv_TunionRes_struct*)_tmp40)->f1;goto _LL79;_LL79:
_tmp4E=((struct Cyc_Tcenv_TunionRes_struct*)_tmp40)->f2;goto _LL69;}else{goto _LL70;}
_LL70: if(*((int*)_tmp40)== Cyc_Tcenv_StructRes){_LL83: _tmp52=((struct Cyc_Tcenv_StructRes_struct*)
_tmp40)->f1;goto _LL71;}else{goto _LL72;}_LL72: if(*((int*)_tmp40)== Cyc_Tcenv_AnonEnumRes){
goto _LL73;}else{goto _LL74;}_LL74: if(*((int*)_tmp40)== Cyc_Tcenv_EnumRes){goto
_LL75;}else{goto _LL65;}_LL67:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp54=_cycalloc(sizeof(struct Cyc_Absyn_FnCall_e_struct));_tmp54[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp55;_tmp55.tag=Cyc_Absyn_FnCall_e;_tmp55.f1=e1;_tmp55.f2=es;_tmp55.f3=0;
_tmp55;});_tmp54;})));_npop_handler(0);return;_LL69: if(_tmp4E->typs == 0){({
struct Cyc_Std_String_pa_struct _tmp57;_tmp57.tag=Cyc_Std_String_pa;_tmp57.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp4E->name);{void*_tmp56[1]={& _tmp57};
Cyc_Tcutil_terr(e->loc,_tag_arr("%s is a constant, not a function",sizeof(
unsigned char),33),_tag_arr(_tmp56,sizeof(void*),1));}});}(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Tunion_e_struct*_tmp58=_cycalloc(sizeof(struct Cyc_Absyn_Tunion_e_struct));
_tmp58[0]=({struct Cyc_Absyn_Tunion_e_struct _tmp59;_tmp59.tag=Cyc_Absyn_Tunion_e;
_tmp59.f1=0;_tmp59.f2=0;_tmp59.f3=es;_tmp59.f4=_tmp50;_tmp59.f5=_tmp4E;_tmp59;});
_tmp58;})));_npop_handler(0);return;_npop_handler(0);return;_LL71: {struct Cyc_List_List*
_tmp5A=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct Cyc_Absyn_Exp*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);if(_tmp52->name != 0){(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*_tmp5B=_cycalloc(
sizeof(struct Cyc_Absyn_Struct_e_struct));_tmp5B[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp5C;_tmp5C.tag=Cyc_Absyn_Struct_e;_tmp5C.f1=(struct _tuple1*)((struct Cyc_Core_Opt*)
_check_null(_tmp52->name))->v;_tmp5C.f2=0;_tmp5C.f3=_tmp5A;_tmp5C.f4=(struct Cyc_Absyn_Structdecl*)
_tmp52;_tmp5C;});_tmp5B;})));}else{({void*_tmp5D[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("missing struct name",sizeof(unsigned char),20),_tag_arr(_tmp5D,sizeof(
void*),0));});(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Struct_e_struct*
_tmp5E=_cycalloc(sizeof(struct Cyc_Absyn_Struct_e_struct));_tmp5E[0]=({struct Cyc_Absyn_Struct_e_struct
_tmp5F;_tmp5F.tag=Cyc_Absyn_Struct_e;_tmp5F.f1=_tmp3B;_tmp5F.f2=0;_tmp5F.f3=
_tmp5A;_tmp5F.f4=(struct Cyc_Absyn_Structdecl*)_tmp52;_tmp5F;});_tmp5E;})));}
_npop_handler(0);return;}_LL73: goto _LL75;_LL75:({struct Cyc_Std_String_pa_struct
_tmp61;_tmp61.tag=Cyc_Std_String_pa;_tmp61.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp3B);{void*_tmp60[1]={& _tmp61};Cyc_Tcutil_terr(e->loc,_tag_arr("%s is an enum constructor, not a function",
sizeof(unsigned char),42),_tag_arr(_tmp60,sizeof(void*),1));}});_npop_handler(0);
return;_LL65:;};_pop_handler();}else{void*_tmp3E=(void*)_exn_thrown;void*_tmp63=
_tmp3E;_LL101: if(_tmp63 == Cyc_Dict_Absent){goto _LL102;}else{goto _LL103;}_LL103:
goto _LL104;_LL102:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmp69=_cycalloc(sizeof(struct Cyc_Absyn_FnCall_e_struct));_tmp69[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp6A;_tmp6A.tag=Cyc_Absyn_FnCall_e;_tmp6A.f1=e1;_tmp6A.f2=es;_tmp6A.f3=0;
_tmp6A;});_tmp69;})));return;_LL104:(void)_throw(_tmp63);_LL100:;}}}_LL58:(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_FnCall_e_struct*_tmp6B=_cycalloc(sizeof(
struct Cyc_Absyn_FnCall_e_struct));_tmp6B[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmp6C;_tmp6C.tag=Cyc_Absyn_FnCall_e;_tmp6C.f1=e1;_tmp6C.f2=es;_tmp6C.f3=0;
_tmp6C;});_tmp6B;})));return;_LL54:;}static void Cyc_Tcexp_resolve_unresolved_mem(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct Cyc_List_List*des){if(topt == 0){(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*
_tmp6D=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_struct));_tmp6D[0]=({struct Cyc_Absyn_Array_e_struct
_tmp6E;_tmp6E.tag=Cyc_Absyn_Array_e;_tmp6E.f1=des;_tmp6E;});_tmp6D;})));return;}{
void*t=*topt;void*_tmp6F=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tqual _tmp79;void*
_tmp7B;struct Cyc_Absyn_Structdecl**_tmp7D;struct Cyc_List_List*_tmp7F;struct
_tuple1*_tmp81;_LL113: if((unsigned int)_tmp6F > 3?*((int*)_tmp6F)== Cyc_Absyn_ArrayType:
0){_LL124: _tmp7B=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp6F)->f1;goto
_LL122;_LL122: _tmp79=((struct Cyc_Absyn_ArrayType_struct*)_tmp6F)->f2;goto _LL114;}
else{goto _LL115;}_LL115: if((unsigned int)_tmp6F > 3?*((int*)_tmp6F)== Cyc_Absyn_StructType:
0){_LL130: _tmp81=((struct Cyc_Absyn_StructType_struct*)_tmp6F)->f1;goto _LL128;
_LL128: _tmp7F=((struct Cyc_Absyn_StructType_struct*)_tmp6F)->f2;goto _LL126;_LL126:
_tmp7D=((struct Cyc_Absyn_StructType_struct*)_tmp6F)->f3;goto _LL116;}else{goto
_LL117;}_LL117: if((unsigned int)_tmp6F > 3?*((int*)_tmp6F)== Cyc_Absyn_AnonStructType:
0){goto _LL118;}else{goto _LL119;}_LL119: goto _LL120;_LL114:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Array_e_struct*_tmp83=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_struct));
_tmp83[0]=({struct Cyc_Absyn_Array_e_struct _tmp84;_tmp84.tag=Cyc_Absyn_Array_e;
_tmp84.f1=des;_tmp84;});_tmp83;})));goto _LL112;_LL116: if(_tmp7D == 0? 1: _tmp81 == 0){({
void*_tmp85[0]={};Cyc_Tcutil_terr(loc,_tag_arr("Compiler bug: struct type not properly set",
sizeof(unsigned char),43),_tag_arr(_tmp85,sizeof(void*),0));});}(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Struct_e_struct*_tmp86=_cycalloc(sizeof(struct
Cyc_Absyn_Struct_e_struct));_tmp86[0]=({struct Cyc_Absyn_Struct_e_struct _tmp87;
_tmp87.tag=Cyc_Absyn_Struct_e;_tmp87.f1=(struct _tuple1*)_check_null(_tmp81);
_tmp87.f2=0;_tmp87.f3=des;_tmp87.f4=(struct Cyc_Absyn_Structdecl*)*((struct Cyc_Absyn_Structdecl**)
_check_null(_tmp7D));_tmp87;});_tmp86;})));goto _LL112;_LL118:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_AnonStruct_e_struct*_tmp88=_cycalloc(sizeof(struct Cyc_Absyn_AnonStruct_e_struct));
_tmp88[0]=({struct Cyc_Absyn_AnonStruct_e_struct _tmp89;_tmp89.tag=Cyc_Absyn_AnonStruct_e;
_tmp89.f1=(void*)t;_tmp89.f2=des;_tmp89;});_tmp88;})));goto _LL112;_LL120:(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp8A=_cycalloc(sizeof(
struct Cyc_Absyn_Array_e_struct));_tmp8A[0]=({struct Cyc_Absyn_Array_e_struct
_tmp8B;_tmp8B.tag=Cyc_Absyn_Array_e;_tmp8B.f1=des;_tmp8B;});_tmp8A;})));goto
_LL112;_LL112:;}}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**
topt,struct Cyc_Absyn_Exp*e);static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e);static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*es){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExp(te,0,(struct
Cyc_Absyn_Exp*)es->hd);}}void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e,struct _tagged_arr msg_part){Cyc_Tcutil_check_contains_assign(e);Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_sint_t,e);if(! Cyc_Tcutil_coerce_to_bool(te,e)){({struct Cyc_Std_String_pa_struct
_tmp8E;_tmp8E.tag=Cyc_Std_String_pa;_tmp8E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp8D;_tmp8D.tag=Cyc_Std_String_pa;_tmp8D.f1=(struct _tagged_arr)msg_part;{void*
_tmp8C[2]={& _tmp8D,& _tmp8E};Cyc_Tcutil_terr(e->loc,_tag_arr("test of %s has type %s instead of integral or * type",
sizeof(unsigned char),53),_tag_arr(_tmp8C,sizeof(void*),2));}}});}}static void*
Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,void*c,struct Cyc_Absyn_Exp*e){void*t;{void*_tmp8F=c;void*_tmp9F;void*_tmpA1;
void*_tmpA3;int _tmpA5;void*_tmpA7;struct _tagged_arr _tmpA9;_LL145: if((
unsigned int)_tmp8F > 1?*((int*)_tmp8F)== Cyc_Absyn_Char_c: 0){_LL160: _tmp9F=(void*)((
struct Cyc_Absyn_Char_c_struct*)_tmp8F)->f1;goto _LL146;}else{goto _LL147;}_LL147:
if((unsigned int)_tmp8F > 1?*((int*)_tmp8F)== Cyc_Absyn_Short_c: 0){_LL162: _tmpA1=(
void*)((struct Cyc_Absyn_Short_c_struct*)_tmp8F)->f1;goto _LL148;}else{goto _LL149;}
_LL149: if((unsigned int)_tmp8F > 1?*((int*)_tmp8F)== Cyc_Absyn_LongLong_c: 0){
_LL164: _tmpA3=(void*)((struct Cyc_Absyn_LongLong_c_struct*)_tmp8F)->f1;goto _LL150;}
else{goto _LL151;}_LL151: if((unsigned int)_tmp8F > 1?*((int*)_tmp8F)== Cyc_Absyn_Float_c:
0){goto _LL152;}else{goto _LL153;}_LL153: if((unsigned int)_tmp8F > 1?*((int*)_tmp8F)
== Cyc_Absyn_Int_c: 0){_LL168: _tmpA7=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp8F)->f1;goto _LL166;_LL166: _tmpA5=((struct Cyc_Absyn_Int_c_struct*)_tmp8F)->f2;
goto _LL154;}else{goto _LL155;}_LL155: if((unsigned int)_tmp8F > 1?*((int*)_tmp8F)== 
Cyc_Absyn_String_c: 0){_LL170: _tmpA9=((struct Cyc_Absyn_String_c_struct*)_tmp8F)->f1;
goto _LL156;}else{goto _LL157;}_LL157: if(_tmp8F == (void*)Cyc_Absyn_Null_c){goto
_LL158;}else{goto _LL144;}_LL146: t=_tmp9F == (void*)Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t:
Cyc_Absyn_schar_t;goto _LL144;_LL148: t=_tmpA1 == (void*)Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t:
Cyc_Absyn_sshort_t;goto _LL144;_LL150: t=_tmpA3 == (void*)Cyc_Absyn_Unsigned? Cyc_Absyn_ulonglong_t:
Cyc_Absyn_slonglong_t;goto _LL144;_LL152: t=Cyc_Absyn_float_t;goto _LL144;_LL154:
if(topt == 0){t=_tmpA7 == (void*)Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;}
else{void*_tmpAB=Cyc_Tcutil_compress(*topt);void*_tmpB7;void*_tmpB9;void*_tmpBB;
void*_tmpBD;void*_tmpBF;_LL173: if((unsigned int)_tmpAB > 3?*((int*)_tmpAB)== Cyc_Absyn_IntType:
0){_LL186: _tmpB9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpAB)->f1;goto _LL184;
_LL184: _tmpB7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpAB)->f2;if(_tmpB7 == (
void*)Cyc_Absyn_B1){goto _LL174;}else{goto _LL175;}}else{goto _LL175;}_LL175: if((
unsigned int)_tmpAB > 3?*((int*)_tmpAB)== Cyc_Absyn_IntType: 0){_LL190: _tmpBD=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpAB)->f1;goto _LL188;_LL188: _tmpBB=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpAB)->f2;if(_tmpBB == (void*)Cyc_Absyn_B2){
goto _LL176;}else{goto _LL177;}}else{goto _LL177;}_LL177: if((unsigned int)_tmpAB > 3?*((
int*)_tmpAB)== Cyc_Absyn_IntType: 0){_LL192: _tmpBF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpAB)->f1;goto _LL178;}else{goto _LL179;}_LL179: if((unsigned int)_tmpAB > 3?*((
int*)_tmpAB)== Cyc_Absyn_PointerType: 0){goto _LL193;}else{goto _LL181;}_LL193: if(
_tmpA5 == 0){goto _LL180;}else{goto _LL181;}_LL181: goto _LL182;_LL174: t=_tmpB9 == (
void*)Cyc_Absyn_Unsigned? Cyc_Absyn_uchar_t: Cyc_Absyn_schar_t;(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Const_e_struct*_tmpC2=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_struct));
_tmpC2[0]=({struct Cyc_Absyn_Const_e_struct _tmpC3;_tmpC3.tag=Cyc_Absyn_Const_e;
_tmpC3.f1=(void*)((void*)({struct Cyc_Absyn_Char_c_struct*_tmpC4=_cycalloc(
sizeof(struct Cyc_Absyn_Char_c_struct));_tmpC4[0]=({struct Cyc_Absyn_Char_c_struct
_tmpC5;_tmpC5.tag=Cyc_Absyn_Char_c;_tmpC5.f1=(void*)_tmpB9;_tmpC5.f2=(
unsigned char)_tmpA5;_tmpC5;});_tmpC4;}));_tmpC3;});_tmpC2;})));goto _LL172;
_LL176: t=_tmpBD == (void*)Cyc_Absyn_Unsigned? Cyc_Absyn_ushort_t: Cyc_Absyn_sshort_t;(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmpC6=_cycalloc(
sizeof(struct Cyc_Absyn_Const_e_struct));_tmpC6[0]=({struct Cyc_Absyn_Const_e_struct
_tmpC7;_tmpC7.tag=Cyc_Absyn_Const_e;_tmpC7.f1=(void*)((void*)({struct Cyc_Absyn_Short_c_struct*
_tmpC8=_cycalloc(sizeof(struct Cyc_Absyn_Short_c_struct));_tmpC8[0]=({struct Cyc_Absyn_Short_c_struct
_tmpC9;_tmpC9.tag=Cyc_Absyn_Short_c;_tmpC9.f1=(void*)_tmpBD;_tmpC9.f2=(short)
_tmpA5;_tmpC9;});_tmpC8;}));_tmpC7;});_tmpC6;})));goto _LL172;_LL178: t=_tmpBF == (
void*)Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;goto _LL172;_LL180:(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmpCA=_cycalloc(
sizeof(struct Cyc_Absyn_Const_e_struct));_tmpCA[0]=({struct Cyc_Absyn_Const_e_struct
_tmpCB;_tmpCB.tag=Cyc_Absyn_Const_e;_tmpCB.f1=(void*)((void*)Cyc_Absyn_Null_c);
_tmpCB;});_tmpCA;})));{struct Cyc_List_List*_tmpCC=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmpCD=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_struct));
_tmpCD[0]=({struct Cyc_Absyn_PointerType_struct _tmpCE;_tmpCE.tag=Cyc_Absyn_PointerType;
_tmpCE.f1=({struct Cyc_Absyn_PtrInfo _tmpCF;_tmpCF.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmpD1=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmpD1->v=_tmpCC;_tmpD1;}));_tmpCF.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmpD0=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmpD0->v=_tmpCC;_tmpD0;}));_tmpCF.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(1);_tmpCF.tq=Cyc_Absyn_empty_tqual();_tmpCF.bounds=
Cyc_Absyn_empty_conref();_tmpCF;});_tmpCE;});_tmpCD;});goto _LL172;}_LL182: t=
_tmpA7 == (void*)Cyc_Absyn_Unsigned? Cyc_Absyn_uint_t: Cyc_Absyn_sint_t;goto _LL172;
_LL172:;}goto _LL144;_LL156: {int len=(int)_get_arr_size(_tmpA9,sizeof(
unsigned char));struct Cyc_Absyn_Const_e_struct*_tmpD2=({struct Cyc_Absyn_Const_e_struct*
_tmpF7=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_struct));_tmpF7[0]=({struct Cyc_Absyn_Const_e_struct
_tmpF8;_tmpF8.tag=Cyc_Absyn_Const_e;_tmpF8.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*
_tmpF9=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));_tmpF9[0]=({struct Cyc_Absyn_Int_c_struct
_tmpFA;_tmpFA.tag=Cyc_Absyn_Int_c;_tmpFA.f1=(void*)((void*)Cyc_Absyn_Unsigned);
_tmpFA.f2=len;_tmpFA;});_tmpF9;}));_tmpF8;});_tmpF7;});struct Cyc_Absyn_Exp*elen=
Cyc_Absyn_new_exp((void*)_tmpD2,loc);elen->topt=({struct Cyc_Core_Opt*_tmpD3=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpD3->v=(void*)Cyc_Absyn_uint_t;_tmpD3;});
t=Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_t,(void*)Cyc_Absyn_HeapRgn,Cyc_Absyn_const_tqual(),(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmpD4=_cycalloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
_tmpD4[0]=({struct Cyc_Absyn_Upper_b_struct _tmpD5;_tmpD5.tag=Cyc_Absyn_Upper_b;
_tmpD5.f1=elen;_tmpD5;});_tmpD4;}));if(topt != 0){void*_tmpD6=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Tqual _tmpE0;void*_tmpE2;struct
Cyc_Absyn_PtrInfo _tmpE4;struct Cyc_Absyn_Conref*_tmpE6;struct Cyc_Absyn_Tqual
_tmpE8;struct Cyc_Absyn_Conref*_tmpEA;void*_tmpEC;void*_tmpEE;_LL216: if((
unsigned int)_tmpD6 > 3?*((int*)_tmpD6)== Cyc_Absyn_ArrayType: 0){_LL227: _tmpE2=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpD6)->f1;goto _LL225;_LL225: _tmpE0=((
struct Cyc_Absyn_ArrayType_struct*)_tmpD6)->f2;goto _LL223;_LL223: _tmpDE=((struct
Cyc_Absyn_ArrayType_struct*)_tmpD6)->f3;goto _LL217;}else{goto _LL218;}_LL218: if((
unsigned int)_tmpD6 > 3?*((int*)_tmpD6)== Cyc_Absyn_PointerType: 0){_LL229: _tmpE4=((
struct Cyc_Absyn_PointerType_struct*)_tmpD6)->f1;_LL239: _tmpEE=(void*)_tmpE4.elt_typ;
goto _LL237;_LL237: _tmpEC=(void*)_tmpE4.rgn_typ;goto _LL235;_LL235: _tmpEA=_tmpE4.nullable;
goto _LL233;_LL233: _tmpE8=_tmpE4.tq;goto _LL231;_LL231: _tmpE6=_tmpE4.bounds;goto
_LL219;}else{goto _LL220;}_LL220: goto _LL221;_LL217: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpF0=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmpF0[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpF1;_tmpF1.tag=Cyc_Absyn_ArrayType;_tmpF1.f1=(void*)Cyc_Absyn_uchar_t;_tmpF1.f2=
_tmpE0;_tmpF1.f3=(struct Cyc_Absyn_Exp*)elen;_tmpF1;});_tmpF0;});_LL219: if(! Cyc_Tcutil_unify(*
topt,t)? Cyc_Tcutil_silent_castable(te,loc,t,*topt): 0){e->topt=({struct Cyc_Core_Opt*
_tmpF2=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpF2->v=(void*)t;_tmpF2;});Cyc_Tcutil_unchecked_cast(
te,e,*topt);t=*topt;}else{t=Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_t,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmpF3=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmpF3->v=Cyc_Tcenv_lookup_type_vars(te);_tmpF3;})),Cyc_Absyn_const_tqual(),(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmpF4=_cycalloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
_tmpF4[0]=({struct Cyc_Absyn_Upper_b_struct _tmpF5;_tmpF5.tag=Cyc_Absyn_Upper_b;
_tmpF5.f1=elen;_tmpF5;});_tmpF4;}));if(! Cyc_Tcutil_unify(*topt,t)? Cyc_Tcutil_silent_castable(
te,loc,t,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmpF6=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmpF6->v=(void*)t;_tmpF6;});Cyc_Tcutil_unchecked_cast(te,e,*topt);
t=*topt;}}goto _LL215;_LL221: goto _LL215;_LL215:;}return t;}_LL158: {struct Cyc_List_List*
_tmpFB=Cyc_Tcenv_lookup_type_vars(te);t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpFC=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_struct));_tmpFC[0]=({struct
Cyc_Absyn_PointerType_struct _tmpFD;_tmpFD.tag=Cyc_Absyn_PointerType;_tmpFD.f1=({
struct Cyc_Absyn_PtrInfo _tmpFE;_tmpFE.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp100=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmp100->v=_tmpFB;_tmp100;}));_tmpFE.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmpFF=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmpFF->v=_tmpFB;_tmpFF;}));_tmpFE.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(1);_tmpFE.tq=Cyc_Absyn_empty_tqual();_tmpFE.bounds=
Cyc_Absyn_empty_conref();_tmpFE;});_tmpFD;});_tmpFC;});goto _LL144;}_LL144:;}
return t;}static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q,void*b){void*_tmp101=b;struct Cyc_Absyn_Vardecl*_tmp10F;
struct Cyc_Absyn_Fndecl*_tmp111;struct Cyc_Absyn_Vardecl*_tmp113;struct Cyc_Absyn_Vardecl*
_tmp115;struct Cyc_Absyn_Vardecl*_tmp117;_LL259: if(_tmp101 == (void*)Cyc_Absyn_Unresolved_b){
goto _LL260;}else{goto _LL261;}_LL261: if((unsigned int)_tmp101 > 1?*((int*)_tmp101)
== Cyc_Absyn_Global_b: 0){_LL272: _tmp10F=((struct Cyc_Absyn_Global_b_struct*)
_tmp101)->f1;goto _LL262;}else{goto _LL263;}_LL263: if((unsigned int)_tmp101 > 1?*((
int*)_tmp101)== Cyc_Absyn_Funname_b: 0){_LL274: _tmp111=((struct Cyc_Absyn_Funname_b_struct*)
_tmp101)->f1;goto _LL264;}else{goto _LL265;}_LL265: if((unsigned int)_tmp101 > 1?*((
int*)_tmp101)== Cyc_Absyn_Pat_b: 0){_LL276: _tmp113=((struct Cyc_Absyn_Pat_b_struct*)
_tmp101)->f1;goto _LL266;}else{goto _LL267;}_LL267: if((unsigned int)_tmp101 > 1?*((
int*)_tmp101)== Cyc_Absyn_Local_b: 0){_LL278: _tmp115=((struct Cyc_Absyn_Local_b_struct*)
_tmp101)->f1;goto _LL268;}else{goto _LL269;}_LL269: if((unsigned int)_tmp101 > 1?*((
int*)_tmp101)== Cyc_Absyn_Param_b: 0){_LL280: _tmp117=((struct Cyc_Absyn_Param_b_struct*)
_tmp101)->f1;goto _LL270;}else{goto _LL258;}_LL260: return({struct Cyc_Std_String_pa_struct
_tmp11A;_tmp11A.tag=Cyc_Std_String_pa;_tmp11A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
q);{void*_tmp119[1]={& _tmp11A};Cyc_Tcexp_expr_err(te,loc,_tag_arr("undeclared identifier: %s",
sizeof(unsigned char),26),_tag_arr(_tmp119,sizeof(void*),1));}});_LL262: q[0]=(
_tmp10F->name)[_check_known_subscript_notnull(1,0)];return(void*)_tmp10F->type;
_LL264: q[0]=(_tmp111->name)[_check_known_subscript_notnull(1,0)];return Cyc_Tcutil_fndecl2typ(
_tmp111);_LL266: _tmp115=_tmp113;goto _LL268;_LL268: _tmp117=_tmp115;goto _LL270;
_LL270:(*q).f1=(void*)Cyc_Absyn_Loc_n;return(void*)_tmp117->type;_LL258:;}static
void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,
struct Cyc_Core_Opt*opt_args,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,
struct _tagged_arr,struct Cyc_Position_Segment*)){struct Cyc_List_List*desc_types;{
void*_tmp11B=(void*)fmt->r;void*_tmp123;struct _tagged_arr _tmp125;struct Cyc_Absyn_Exp*
_tmp127;struct Cyc_Absyn_Exp _tmp129;void*_tmp12A;void*_tmp12C;struct _tagged_arr
_tmp12E;_LL285: if(*((int*)_tmp11B)== Cyc_Absyn_Const_e){_LL292: _tmp123=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp11B)->f1;if((unsigned int)_tmp123 > 1?*((int*)
_tmp123)== Cyc_Absyn_String_c: 0){_LL294: _tmp125=((struct Cyc_Absyn_String_c_struct*)
_tmp123)->f1;goto _LL286;}else{goto _LL287;}}else{goto _LL287;}_LL287: if(*((int*)
_tmp11B)== Cyc_Absyn_Cast_e){_LL296: _tmp127=((struct Cyc_Absyn_Cast_e_struct*)
_tmp11B)->f2;_tmp129=*_tmp127;_LL299: _tmp12A=(void*)_tmp129.r;if(*((int*)_tmp12A)
== Cyc_Absyn_Const_e){_LL301: _tmp12C=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp12A)->f1;if((unsigned int)_tmp12C > 1?*((int*)_tmp12C)== Cyc_Absyn_String_c: 0){
_LL303: _tmp12E=((struct Cyc_Absyn_String_c_struct*)_tmp12C)->f1;goto _LL288;}else{
goto _LL289;}}else{goto _LL289;}}else{goto _LL289;}_LL289: goto _LL290;_LL286: _tmp12E=
_tmp125;goto _LL288;_LL288: desc_types=type_getter(te,(struct _tagged_arr)_tmp12E,
fmt->loc);goto _LL284;_LL290: return;_LL284:;}if(opt_args != 0){struct Cyc_List_List*
_tmp130=(struct Cyc_List_List*)opt_args->v;for(0;desc_types != 0? _tmp130 != 0: 0;(
desc_types=desc_types->tl,_tmp130=_tmp130->tl)){void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp130->hd;if(! Cyc_Tcutil_coerce_arg(
te,e,t)){({struct Cyc_Std_String_pa_struct _tmp133;_tmp133.tag=Cyc_Std_String_pa;
_tmp133.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);{struct Cyc_Std_String_pa_struct _tmp132;_tmp132.tag=Cyc_Std_String_pa;
_tmp132.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp131[2]={&
_tmp132,& _tmp133};Cyc_Tcutil_terr(e->loc,_tag_arr("descriptor has type \n%s\n but argument has type \n%s",
sizeof(unsigned char),51),_tag_arr(_tmp131,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}
if(desc_types != 0){({void*_tmp134[0]={};Cyc_Tcutil_terr(fmt->loc,_tag_arr("too few arguments",
sizeof(unsigned char),18),_tag_arr(_tmp134,sizeof(void*),0));});}if(_tmp130 != 0){({
void*_tmp135[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp130->hd)->loc,
_tag_arr("too many arguments",sizeof(unsigned char),19),_tag_arr(_tmp135,sizeof(
void*),0));});}}}static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void**topt,void*p,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp136=p;_LL312: if(
_tmp136 == (void*)Cyc_Absyn_Plus){goto _LL313;}else{goto _LL314;}_LL314: if(_tmp136
== (void*)Cyc_Absyn_Minus){goto _LL315;}else{goto _LL316;}_LL316: if(_tmp136 == (
void*)Cyc_Absyn_Not){goto _LL317;}else{goto _LL318;}_LL318: if(_tmp136 == (void*)Cyc_Absyn_Bitnot){
goto _LL319;}else{goto _LL320;}_LL320: if(_tmp136 == (void*)Cyc_Absyn_Size){goto
_LL321;}else{goto _LL322;}_LL322: goto _LL323;_LL313: goto _LL315;_LL315: if(! Cyc_Tcutil_is_numeric(
e)){({struct Cyc_Std_String_pa_struct _tmp145;_tmp145.tag=Cyc_Std_String_pa;
_tmp145.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp144[1]={&
_tmp145};Cyc_Tcutil_terr(loc,_tag_arr("expecting arithmetic type but found %s",
sizeof(unsigned char),39),_tag_arr(_tmp144,sizeof(void*),1));}});}return(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL317: Cyc_Tcutil_check_contains_assign(
e);if(! Cyc_Tcutil_coerce_to_bool(te,e)){({struct Cyc_Std_String_pa_struct _tmp147;
_tmp147.tag=Cyc_Std_String_pa;_tmp147.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{void*_tmp146[1]={& _tmp147};Cyc_Tcutil_terr(loc,_tag_arr("expecting integral or * type but found %s",
sizeof(unsigned char),42),_tag_arr(_tmp146,sizeof(void*),1));}});}return Cyc_Absyn_sint_t;
_LL319: if(! Cyc_Tcutil_is_integral(e)){({struct Cyc_Std_String_pa_struct _tmp149;
_tmp149.tag=Cyc_Std_String_pa;_tmp149.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{void*_tmp148[1]={& _tmp149};Cyc_Tcutil_terr(loc,_tag_arr("expecting integral type but found %s",
sizeof(unsigned char),37),_tag_arr(_tmp148,sizeof(void*),1));}});}return(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;_LL321:{void*_tmp14A=t;struct Cyc_Absyn_PtrInfo
_tmp152;_LL332: if((unsigned int)_tmp14A > 3?*((int*)_tmp14A)== Cyc_Absyn_ArrayType:
0){goto _LL333;}else{goto _LL334;}_LL334: if((unsigned int)_tmp14A > 3?*((int*)
_tmp14A)== Cyc_Absyn_PointerType: 0){_LL339: _tmp152=((struct Cyc_Absyn_PointerType_struct*)
_tmp14A)->f1;goto _LL335;}else{goto _LL336;}_LL336: goto _LL337;_LL333: goto _LL331;
_LL335: goto _LL331;_LL337:({struct Cyc_Std_String_pa_struct _tmp155;_tmp155.tag=Cyc_Std_String_pa;
_tmp155.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp154[1]={&
_tmp155};Cyc_Tcutil_terr(loc,_tag_arr("size requires pointer or array type, not %s",
sizeof(unsigned char),44),_tag_arr(_tmp154,sizeof(void*),1));}});goto _LL331;
_LL331:;}return Cyc_Absyn_uint_t;_LL323: return({void*_tmp156[0]={};Cyc_Tcutil_impos(
_tag_arr("Non-unary primop",sizeof(unsigned char),17),_tag_arr(_tmp156,sizeof(
void*),0));});_LL311:;}static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
if(! checker(e1)){({struct Cyc_Std_String_pa_struct _tmp158;_tmp158.tag=Cyc_Std_String_pa;
_tmp158.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);{void*_tmp157[1]={& _tmp158};Cyc_Tcutil_terr(e1->loc,
_tag_arr("type %s cannot be used here",sizeof(unsigned char),28),_tag_arr(
_tmp157,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp159=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp159->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp159;}));}if(! checker(e2)){({struct Cyc_Std_String_pa_struct _tmp15B;
_tmp15B.tag=Cyc_Std_String_pa;_tmp15B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{void*_tmp15A[1]={&
_tmp15B};Cyc_Tcutil_terr(e2->loc,_tag_arr("type %s cannot be used here",sizeof(
unsigned char),28),_tag_arr(_tmp15A,sizeof(void*),1));}});return Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp15C=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp15C->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp15C;}));}{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);return Cyc_Tcutil_max_arithmetic_type(t1,t2);}}static
void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);
void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
void*_tmp15D=t1;struct Cyc_Absyn_PtrInfo _tmp163;struct Cyc_Absyn_Conref*_tmp165;
struct Cyc_Absyn_Tqual _tmp167;struct Cyc_Absyn_Conref*_tmp169;void*_tmp16B;void*
_tmp16D;_LL351: if((unsigned int)_tmp15D > 3?*((int*)_tmp15D)== Cyc_Absyn_PointerType:
0){_LL356: _tmp163=((struct Cyc_Absyn_PointerType_struct*)_tmp15D)->f1;_LL366:
_tmp16D=(void*)_tmp163.elt_typ;goto _LL364;_LL364: _tmp16B=(void*)_tmp163.rgn_typ;
goto _LL362;_LL362: _tmp169=_tmp163.nullable;goto _LL360;_LL360: _tmp167=_tmp163.tq;
goto _LL358;_LL358: _tmp165=_tmp163.bounds;goto _LL352;}else{goto _LL353;}_LL353:
goto _LL354;_LL352: if(! Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp16D),(void*)
Cyc_Absyn_MemKind)){({void*_tmp16F[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("can't perform arithmetic on abstract pointer type",
sizeof(unsigned char),50),_tag_arr(_tmp16F,sizeof(void*),0));});}if(! Cyc_Tcutil_coerce_sint_typ(
te,e2)){({struct Cyc_Std_String_pa_struct _tmp171;_tmp171.tag=Cyc_Std_String_pa;
_tmp171.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmp170[1]={&
_tmp171};Cyc_Tcutil_terr(e2->loc,_tag_arr("expecting int but found %s",sizeof(
unsigned char),27),_tag_arr(_tmp170,sizeof(void*),1));}});}_tmp165=Cyc_Absyn_compress_conref(
_tmp165);{void*_tmp172=(void*)_tmp165->v;void*_tmp17A;void*_tmp17C;struct Cyc_Absyn_Exp*
_tmp17E;_LL372: if((unsigned int)_tmp172 > 1?*((int*)_tmp172)== Cyc_Absyn_Eq_constr:
0){_LL379: _tmp17A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp172)->f1;if(
_tmp17A == (void*)Cyc_Absyn_Unknown_b){goto _LL373;}else{goto _LL374;}}else{goto
_LL374;}_LL374: if((unsigned int)_tmp172 > 1?*((int*)_tmp172)== Cyc_Absyn_Eq_constr:
0){_LL381: _tmp17C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp172)->f1;if((
unsigned int)_tmp17C > 1?*((int*)_tmp17C)== Cyc_Absyn_Upper_b: 0){_LL383: _tmp17E=((
struct Cyc_Absyn_Upper_b_struct*)_tmp17C)->f1;goto _LL375;}else{goto _LL376;}}else{
goto _LL376;}_LL376: goto _LL377;_LL373: return t1;_LL375: {struct Cyc_Absyn_PointerType_struct*
_tmp180=({struct Cyc_Absyn_PointerType_struct*_tmp181=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_struct));
_tmp181[0]=({struct Cyc_Absyn_PointerType_struct _tmp182;_tmp182.tag=Cyc_Absyn_PointerType;
_tmp182.f1=({struct Cyc_Absyn_PtrInfo _tmp183;_tmp183.elt_typ=(void*)_tmp16D;
_tmp183.rgn_typ=(void*)_tmp16B;_tmp183.nullable=((struct Cyc_Absyn_Conref*(*)(int
x))Cyc_Absyn_new_conref)(1);_tmp183.tq=_tmp167;_tmp183.bounds=Cyc_Absyn_new_conref((
void*)Cyc_Absyn_Unknown_b);_tmp183;});_tmp182;});_tmp181;});Cyc_Tcutil_unchecked_cast(
te,e1,(void*)_tmp180);return(void*)_tmp180;}_LL377:(void*)(_tmp165->v=(void*)((
void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp184=_cycalloc(sizeof(struct Cyc_Absyn_Eq_constr_struct));
_tmp184[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp185;_tmp185.tag=Cyc_Absyn_Eq_constr;
_tmp185.f1=(void*)((void*)Cyc_Absyn_Unknown_b);_tmp185;});_tmp184;})));return t1;
_LL371:;}_LL354: return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
_LL350:;}static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*t1=(void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v;
void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;void*t1_elt=(void*)
Cyc_Absyn_VoidType;void*t2_elt=(void*)Cyc_Absyn_VoidType;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t1,& t1_elt)){if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){if(! Cyc_Tcutil_unify(t1,t2)){({
struct Cyc_Std_String_pa_struct _tmp188;_tmp188.tag=Cyc_Std_String_pa;_tmp188.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);{struct Cyc_Std_String_pa_struct _tmp187;_tmp187.tag=
Cyc_Std_String_pa;_tmp187.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{void*_tmp186[2]={& _tmp187,&
_tmp188};Cyc_Tcutil_terr(e1->loc,_tag_arr("pointer arithmetic on values of different types (%s != %s)",
sizeof(unsigned char),59),_tag_arr(_tmp186,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return Cyc_Absyn_sint_t;}else{if(! Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),(
void*)Cyc_Absyn_MemKind)){({void*_tmp189[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("can't perform arithmetic on abstract pointer type",
sizeof(unsigned char),50),_tag_arr(_tmp189,sizeof(void*),0));});}if(! Cyc_Tcutil_coerce_sint_typ(
te,e2)){({struct Cyc_Std_String_pa_struct _tmp18C;_tmp18C.tag=Cyc_Std_String_pa;
_tmp18C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct
_tmp18B;_tmp18B.tag=Cyc_Std_String_pa;_tmp18B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t1);{void*_tmp18A[2]={& _tmp18B,& _tmp18C};Cyc_Tcutil_terr(e2->loc,_tag_arr("expecting either %s or int but found %s",
sizeof(unsigned char),40),_tag_arr(_tmp18A,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t1;}}else{return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}}
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int e1_is_num=Cyc_Tcutil_is_numeric(
e1);int e2_is_num=Cyc_Tcutil_is_numeric(e2);void*t1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(e1_is_num? e2_is_num: 0){return
Cyc_Absyn_sint_t;}else{if(Cyc_Tcutil_unify(t1,t2)? Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_bk,({struct Cyc_Core_Opt*_tmp18D=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp18D->v=Cyc_Tcenv_lookup_type_vars(te);_tmp18D;}))):
0){return Cyc_Absyn_sint_t;}else{if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){Cyc_Tcutil_unchecked_cast(
te,e2,t1);return Cyc_Absyn_sint_t;}else{if(Cyc_Tcutil_silent_castable(te,loc,t1,
t2)){Cyc_Tcutil_unchecked_cast(te,e1,t2);return Cyc_Absyn_sint_t;}else{if(Cyc_Tcutil_zero_to_null(
te,t2,e1)? 1: Cyc_Tcutil_zero_to_null(te,t1,e2)){return Cyc_Absyn_sint_t;}else{{
struct _tuple0 _tmp18F=({struct _tuple0 _tmp18E;_tmp18E.f1=Cyc_Tcutil_compress(t1);
_tmp18E.f2=Cyc_Tcutil_compress(t2);_tmp18E;});void*_tmp195;struct Cyc_Absyn_PtrInfo
_tmp197;void*_tmp199;void*_tmp19B;struct Cyc_Absyn_PtrInfo _tmp19D;void*_tmp19F;
_LL401: _LL412: _tmp19B=_tmp18F.f1;if((unsigned int)_tmp19B > 3?*((int*)_tmp19B)== 
Cyc_Absyn_PointerType: 0){_LL414: _tmp19D=((struct Cyc_Absyn_PointerType_struct*)
_tmp19B)->f1;_LL416: _tmp19F=(void*)_tmp19D.elt_typ;goto _LL406;}else{goto _LL403;}
_LL406: _tmp195=_tmp18F.f2;if((unsigned int)_tmp195 > 3?*((int*)_tmp195)== Cyc_Absyn_PointerType:
0){_LL408: _tmp197=((struct Cyc_Absyn_PointerType_struct*)_tmp195)->f1;_LL410:
_tmp199=(void*)_tmp197.elt_typ;goto _LL402;}else{goto _LL403;}_LL403: goto _LL404;
_LL402: if(Cyc_Tcutil_unify(_tmp19F,_tmp199)){return Cyc_Absyn_sint_t;}goto _LL400;
_LL404: goto _LL400;_LL400:;}FAIL:({struct Cyc_Std_String_pa_struct _tmp1A3;_tmp1A3.tag=
Cyc_Std_String_pa;_tmp1A3.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp1A2;_tmp1A2.tag=Cyc_Std_String_pa;_tmp1A2.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp1A1[2]={& _tmp1A2,& _tmp1A3};
Cyc_Tcutil_terr(loc,_tag_arr("comparison not allowed between %s and %s",sizeof(
unsigned char),41),_tag_arr(_tmp1A1,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*_tmp1A4=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp1A4->v=Cyc_Tcenv_lookup_type_vars(te);_tmp1A4;}));}}}}}}static void*Cyc_Tcexp_tcBinPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp1A5=p;_LL423: if(_tmp1A5 == (
void*)Cyc_Absyn_Plus){goto _LL424;}else{goto _LL425;}_LL425: if(_tmp1A5 == (void*)
Cyc_Absyn_Minus){goto _LL426;}else{goto _LL427;}_LL427: if(_tmp1A5 == (void*)Cyc_Absyn_Times){
goto _LL428;}else{goto _LL429;}_LL429: if(_tmp1A5 == (void*)Cyc_Absyn_Div){goto
_LL430;}else{goto _LL431;}_LL431: if(_tmp1A5 == (void*)Cyc_Absyn_Mod){goto _LL432;}
else{goto _LL433;}_LL433: if(_tmp1A5 == (void*)Cyc_Absyn_Bitand){goto _LL434;}else{
goto _LL435;}_LL435: if(_tmp1A5 == (void*)Cyc_Absyn_Bitor){goto _LL436;}else{goto
_LL437;}_LL437: if(_tmp1A5 == (void*)Cyc_Absyn_Bitxor){goto _LL438;}else{goto _LL439;}
_LL439: if(_tmp1A5 == (void*)Cyc_Absyn_Bitlshift){goto _LL440;}else{goto _LL441;}
_LL441: if(_tmp1A5 == (void*)Cyc_Absyn_Bitlrshift){goto _LL442;}else{goto _LL443;}
_LL443: if(_tmp1A5 == (void*)Cyc_Absyn_Bitarshift){goto _LL444;}else{goto _LL445;}
_LL445: if(_tmp1A5 == (void*)Cyc_Absyn_Eq){goto _LL446;}else{goto _LL447;}_LL447: if(
_tmp1A5 == (void*)Cyc_Absyn_Neq){goto _LL448;}else{goto _LL449;}_LL449: if(_tmp1A5 == (
void*)Cyc_Absyn_Gt){goto _LL450;}else{goto _LL451;}_LL451: if(_tmp1A5 == (void*)Cyc_Absyn_Lt){
goto _LL452;}else{goto _LL453;}_LL453: if(_tmp1A5 == (void*)Cyc_Absyn_Gte){goto
_LL454;}else{goto _LL455;}_LL455: if(_tmp1A5 == (void*)Cyc_Absyn_Lte){goto _LL456;}
else{goto _LL457;}_LL457: goto _LL458;_LL424: return Cyc_Tcexp_tcPlus(te,e1,e2);
_LL426: return Cyc_Tcexp_tcMinus(te,e1,e2);_LL428: goto _LL430;_LL430: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_numeric);_LL432: goto _LL434;_LL434: goto _LL436;_LL436: goto
_LL438;_LL438: goto _LL440;_LL440: goto _LL442;_LL442: goto _LL444;_LL444: return Cyc_Tcexp_tcArithBinop(
te,e1,e2,Cyc_Tcutil_is_integral);_LL446: goto _LL448;_LL448: goto _LL450;_LL450: goto
_LL452;_LL452: goto _LL454;_LL454: goto _LL456;_LL456: return Cyc_Tcexp_tcAnyBinop(te,
loc,e1,e2);_LL458:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp1CB=
_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmp1CB[0]=({struct Cyc_Core_Failure_struct
_tmp1CC;_tmp1CC.tag=Cyc_Core_Failure;_tmp1CC.f1=_tag_arr("bad binary primop",
sizeof(unsigned char),18);_tmp1CC;});_tmp1CB;}));_LL422:;}static void*Cyc_Tcexp_tcPrimop(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,void*p,struct
Cyc_List_List*es){if(p == (void*)Cyc_Absyn_Minus?((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(es)== 1: 0){return Cyc_Tcexp_tcExp(te,topt,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd);}Cyc_Tcexp_tcExpList(te,es);{void*t;
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){case 0: _LL461: return({
void*_tmp1CF[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("primitive operator has 0 arguments",
sizeof(unsigned char),35),_tag_arr(_tmp1CF,sizeof(void*),0));});case 1: _LL462: t=
Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd);break;case 2: _LL464: t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);
break;default: _LL465: return({void*_tmp1D3[0]={};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("primitive operator has > 2 arguments",sizeof(unsigned char),37),
_tag_arr(_tmp1D3,sizeof(void*),0));});}return t;}}struct _tuple8{struct Cyc_Absyn_Tqual
f1;void*f2;};static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){{void*_tmp1D4=(void*)e->r;void*_tmp1EC;struct Cyc_Absyn_Vardecl*
_tmp1EE;void*_tmp1F0;struct Cyc_Absyn_Vardecl*_tmp1F2;void*_tmp1F4;struct Cyc_Absyn_Vardecl*
_tmp1F6;void*_tmp1F8;struct Cyc_Absyn_Vardecl*_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FC;
struct Cyc_Absyn_Exp*_tmp1FE;struct _tagged_arr*_tmp200;struct Cyc_Absyn_Exp*
_tmp202;struct _tagged_arr*_tmp204;struct Cyc_Absyn_Exp*_tmp206;struct Cyc_Absyn_Exp*
_tmp208;struct Cyc_Absyn_Exp*_tmp20A;struct Cyc_Absyn_Exp*_tmp20C;_LL470: if(*((int*)
_tmp1D4)== Cyc_Absyn_Var_e){_LL493: _tmp1EC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1D4)->f2;if((unsigned int)_tmp1EC > 1?*((int*)_tmp1EC)== Cyc_Absyn_Param_b: 0){
_LL495: _tmp1EE=((struct Cyc_Absyn_Param_b_struct*)_tmp1EC)->f1;goto _LL471;}else{
goto _LL472;}}else{goto _LL472;}_LL472: if(*((int*)_tmp1D4)== Cyc_Absyn_Var_e){
_LL497: _tmp1F0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp1D4)->f2;if((
unsigned int)_tmp1F0 > 1?*((int*)_tmp1F0)== Cyc_Absyn_Local_b: 0){_LL499: _tmp1F2=((
struct Cyc_Absyn_Local_b_struct*)_tmp1F0)->f1;goto _LL473;}else{goto _LL474;}}else{
goto _LL474;}_LL474: if(*((int*)_tmp1D4)== Cyc_Absyn_Var_e){_LL501: _tmp1F4=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp1D4)->f2;if((unsigned int)_tmp1F4 > 1?*((int*)
_tmp1F4)== Cyc_Absyn_Pat_b: 0){_LL503: _tmp1F6=((struct Cyc_Absyn_Pat_b_struct*)
_tmp1F4)->f1;goto _LL475;}else{goto _LL476;}}else{goto _LL476;}_LL476: if(*((int*)
_tmp1D4)== Cyc_Absyn_Var_e){_LL505: _tmp1F8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp1D4)->f2;if((unsigned int)_tmp1F8 > 1?*((int*)_tmp1F8)== Cyc_Absyn_Global_b: 0){
_LL507: _tmp1FA=((struct Cyc_Absyn_Global_b_struct*)_tmp1F8)->f1;goto _LL477;}else{
goto _LL478;}}else{goto _LL478;}_LL478: if(*((int*)_tmp1D4)== Cyc_Absyn_Subscript_e){
_LL511: _tmp1FE=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1D4)->f1;goto _LL509;
_LL509: _tmp1FC=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1D4)->f2;goto _LL479;}
else{goto _LL480;}_LL480: if(*((int*)_tmp1D4)== Cyc_Absyn_StructMember_e){_LL515:
_tmp202=((struct Cyc_Absyn_StructMember_e_struct*)_tmp1D4)->f1;goto _LL513;_LL513:
_tmp200=((struct Cyc_Absyn_StructMember_e_struct*)_tmp1D4)->f2;goto _LL481;}else{
goto _LL482;}_LL482: if(*((int*)_tmp1D4)== Cyc_Absyn_StructArrow_e){_LL519: _tmp206=((
struct Cyc_Absyn_StructArrow_e_struct*)_tmp1D4)->f1;goto _LL517;_LL517: _tmp204=((
struct Cyc_Absyn_StructArrow_e_struct*)_tmp1D4)->f2;goto _LL483;}else{goto _LL484;}
_LL484: if(*((int*)_tmp1D4)== Cyc_Absyn_Deref_e){_LL521: _tmp208=((struct Cyc_Absyn_Deref_e_struct*)
_tmp1D4)->f1;goto _LL485;}else{goto _LL486;}_LL486: if(*((int*)_tmp1D4)== Cyc_Absyn_NoInstantiate_e){
_LL523: _tmp20A=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D4)->f1;goto _LL487;}
else{goto _LL488;}_LL488: if(*((int*)_tmp1D4)== Cyc_Absyn_Instantiate_e){_LL525:
_tmp20C=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp1D4)->f1;goto _LL489;}else{
goto _LL490;}_LL490: goto _LL491;_LL471: _tmp1F2=_tmp1EE;goto _LL473;_LL473: _tmp1F6=
_tmp1F2;goto _LL475;_LL475: _tmp1FA=_tmp1F6;goto _LL477;_LL477: if(!(_tmp1FA->tq).q_const){
return;}goto _LL469;_LL479:{void*_tmp20E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1FE->topt))->v);struct Cyc_Absyn_PtrInfo _tmp218;struct Cyc_Absyn_Tqual
_tmp21A;struct Cyc_Absyn_Tqual _tmp21C;struct Cyc_List_List*_tmp21E;_LL528: if((
unsigned int)_tmp20E > 3?*((int*)_tmp20E)== Cyc_Absyn_PointerType: 0){_LL537:
_tmp218=((struct Cyc_Absyn_PointerType_struct*)_tmp20E)->f1;_LL539: _tmp21A=
_tmp218.tq;goto _LL529;}else{goto _LL530;}_LL530: if((unsigned int)_tmp20E > 3?*((
int*)_tmp20E)== Cyc_Absyn_ArrayType: 0){_LL541: _tmp21C=((struct Cyc_Absyn_ArrayType_struct*)
_tmp20E)->f2;goto _LL531;}else{goto _LL532;}_LL532: if((unsigned int)_tmp20E > 3?*((
int*)_tmp20E)== Cyc_Absyn_TupleType: 0){_LL543: _tmp21E=((struct Cyc_Absyn_TupleType_struct*)
_tmp20E)->f1;goto _LL533;}else{goto _LL534;}_LL534: goto _LL535;_LL529: _tmp21C=
_tmp21A;goto _LL531;_LL531: if(! _tmp21C.q_const){return;}goto _LL527;_LL533: {
unsigned int i=Cyc_Evexp_eval_const_uint_exp(_tmp1FC);{struct _handler_cons _tmp220;
_push_handler(& _tmp220);{int _tmp222=0;if(setjmp(_tmp220.handler)){_tmp222=1;}if(
! _tmp222){{struct _tuple8 _tmp225;struct Cyc_Absyn_Tqual _tmp226;struct _tuple8*
_tmp223=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp21E,(
int)i);_tmp225=*_tmp223;_LL551: _tmp226=_tmp225.f1;goto _LL548;_LL548: if(! _tmp226.q_const){
_npop_handler(0);return;}};_pop_handler();}else{void*_tmp221=(void*)_exn_thrown;
void*_tmp229=_tmp221;_LL555: if(_tmp229 == Cyc_List_Nth){goto _LL556;}else{goto
_LL557;}_LL557: goto _LL558;_LL556: return;_LL558:(void)_throw(_tmp229);_LL554:;}}}
goto _LL527;}_LL535: goto _LL527;_LL527:;}goto _LL469;_LL481:{void*_tmp22F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp202->topt))->v);struct Cyc_Absyn_Structdecl**
_tmp23B;struct Cyc_List_List*_tmp23D;struct Cyc_List_List*_tmp23F;struct Cyc_Absyn_Uniondecl**
_tmp241;_LL561: if((unsigned int)_tmp22F > 3?*((int*)_tmp22F)== Cyc_Absyn_StructType:
0){_LL572: _tmp23B=((struct Cyc_Absyn_StructType_struct*)_tmp22F)->f3;goto _LL562;}
else{goto _LL563;}_LL563: if((unsigned int)_tmp22F > 3?*((int*)_tmp22F)== Cyc_Absyn_AnonUnionType:
0){_LL574: _tmp23D=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp22F)->f1;goto
_LL564;}else{goto _LL565;}_LL565: if((unsigned int)_tmp22F > 3?*((int*)_tmp22F)== 
Cyc_Absyn_AnonStructType: 0){_LL576: _tmp23F=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp22F)->f1;goto _LL566;}else{goto _LL567;}_LL567: if((unsigned int)_tmp22F > 3?*((
int*)_tmp22F)== Cyc_Absyn_UnionType: 0){_LL578: _tmp241=((struct Cyc_Absyn_UnionType_struct*)
_tmp22F)->f3;goto _LL568;}else{goto _LL569;}_LL569: goto _LL570;_LL562: {struct Cyc_Absyn_Structfield*
sf=_tmp23B == 0? 0: Cyc_Absyn_lookup_struct_field((struct Cyc_Absyn_Structdecl*)*
_tmp23B,_tmp200);if(sf == 0? 1: !(sf->tq).q_const){return;}goto _LL560;}_LL564:
_tmp23F=_tmp23D;goto _LL566;_LL566: {struct Cyc_Absyn_Structfield*sf=Cyc_Absyn_lookup_field(
_tmp23F,_tmp200);if(sf == 0? 1: !(sf->tq).q_const){return;}goto _LL560;}_LL568: {
struct Cyc_Absyn_Structfield*sf=_tmp241 == 0? 0: Cyc_Absyn_lookup_union_field((
struct Cyc_Absyn_Uniondecl*)*_tmp241,_tmp200);if(sf == 0? 1: !(sf->tq).q_const){
return;}goto _LL560;}_LL570: goto _LL560;_LL560:;}goto _LL469;_LL483:{void*_tmp243=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp206->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp249;void*_tmp24B;_LL581: if((unsigned int)_tmp243 > 3?*((
int*)_tmp243)== Cyc_Absyn_PointerType: 0){_LL586: _tmp249=((struct Cyc_Absyn_PointerType_struct*)
_tmp243)->f1;_LL588: _tmp24B=(void*)_tmp249.elt_typ;goto _LL582;}else{goto _LL583;}
_LL583: goto _LL584;_LL582:{void*_tmp24D=Cyc_Tcutil_compress(_tmp24B);struct Cyc_Absyn_Structdecl**
_tmp259;struct Cyc_Absyn_Uniondecl**_tmp25B;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*
_tmp25F;_LL591: if((unsigned int)_tmp24D > 3?*((int*)_tmp24D)== Cyc_Absyn_StructType:
0){_LL602: _tmp259=((struct Cyc_Absyn_StructType_struct*)_tmp24D)->f3;goto _LL592;}
else{goto _LL593;}_LL593: if((unsigned int)_tmp24D > 3?*((int*)_tmp24D)== Cyc_Absyn_UnionType:
0){_LL604: _tmp25B=((struct Cyc_Absyn_UnionType_struct*)_tmp24D)->f3;goto _LL594;}
else{goto _LL595;}_LL595: if((unsigned int)_tmp24D > 3?*((int*)_tmp24D)== Cyc_Absyn_AnonStructType:
0){_LL606: _tmp25D=((struct Cyc_Absyn_AnonStructType_struct*)_tmp24D)->f1;goto
_LL596;}else{goto _LL597;}_LL597: if((unsigned int)_tmp24D > 3?*((int*)_tmp24D)== 
Cyc_Absyn_AnonUnionType: 0){_LL608: _tmp25F=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp24D)->f1;goto _LL598;}else{goto _LL599;}_LL599: goto _LL600;_LL592: {struct Cyc_Absyn_Structfield*
sf=_tmp259 == 0? 0: Cyc_Absyn_lookup_struct_field((struct Cyc_Absyn_Structdecl*)*
_tmp259,_tmp204);if(sf == 0? 1: !(sf->tq).q_const){return;}goto _LL590;}_LL594: {
struct Cyc_Absyn_Structfield*sf=_tmp25B == 0? 0: Cyc_Absyn_lookup_union_field((
struct Cyc_Absyn_Uniondecl*)*_tmp25B,_tmp204);if(sf == 0? 1: !(sf->tq).q_const){
return;}goto _LL590;}_LL596: _tmp25F=_tmp25D;goto _LL598;_LL598: {struct Cyc_Absyn_Structfield*
sf=Cyc_Absyn_lookup_field(_tmp25F,_tmp204);if(sf == 0? 1: !(sf->tq).q_const){
return;}goto _LL590;}_LL600: goto _LL590;_LL590:;}goto _LL580;_LL584: goto _LL580;
_LL580:;}goto _LL469;_LL485:{void*_tmp261=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp208->topt))->v);struct Cyc_Absyn_PtrInfo _tmp269;struct Cyc_Absyn_Tqual
_tmp26B;struct Cyc_Absyn_Tqual _tmp26D;_LL611: if((unsigned int)_tmp261 > 3?*((int*)
_tmp261)== Cyc_Absyn_PointerType: 0){_LL618: _tmp269=((struct Cyc_Absyn_PointerType_struct*)
_tmp261)->f1;_LL620: _tmp26B=_tmp269.tq;goto _LL612;}else{goto _LL613;}_LL613: if((
unsigned int)_tmp261 > 3?*((int*)_tmp261)== Cyc_Absyn_ArrayType: 0){_LL622: _tmp26D=((
struct Cyc_Absyn_ArrayType_struct*)_tmp261)->f2;goto _LL614;}else{goto _LL615;}
_LL615: goto _LL616;_LL612: _tmp26D=_tmp26B;goto _LL614;_LL614: if(! _tmp26D.q_const){
return;}goto _LL610;_LL616: goto _LL610;_LL610:;}goto _LL469;_LL487: _tmp20C=_tmp20A;
goto _LL489;_LL489: Cyc_Tcexp_check_writable(te,_tmp20C);return;_LL491: goto _LL469;
_LL469:;}({struct Cyc_Std_String_pa_struct _tmp270;_tmp270.tag=Cyc_Std_String_pa;
_tmp270.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp26F[1]={&
_tmp270};Cyc_Tcutil_terr(e->loc,_tag_arr("attempt to write a const location: %s",
sizeof(unsigned char),38),_tag_arr(_tmp26F,sizeof(void*),1));}});}static void*Cyc_Tcexp_tcIncrement(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,void*i){Cyc_Tcexp_tcExpNoPromote(te,0,e);if(! Cyc_Absyn_is_lvalue(e)){return({
void*_tmp271[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("increment/decrement of non-lvalue",
sizeof(unsigned char),34),_tag_arr(_tmp271,sizeof(void*),0));});}Cyc_Tcexp_check_writable(
te,e);{void*t=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;if(! Cyc_Tcutil_is_numeric(
e)){void*telt=(void*)Cyc_Absyn_VoidType;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& telt)){if(! Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),(void*)Cyc_Absyn_MemKind)){({
void*_tmp272[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("can't perform arithmetic on abstract pointer type",
sizeof(unsigned char),50),_tag_arr(_tmp272,sizeof(void*),0));});}}else{({struct
Cyc_Std_String_pa_struct _tmp274;_tmp274.tag=Cyc_Std_String_pa;_tmp274.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp273[1]={& _tmp274};Cyc_Tcutil_terr(
e->loc,_tag_arr("expecting arithmetic or ? type but found %s",sizeof(
unsigned char),44),_tag_arr(_tmp273,sizeof(void*),1));}});}}return t;}}static void*
Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
Cyc_Tcexp_tcTest(te,e1,_tag_arr("conditional expression",sizeof(unsigned char),
23));Cyc_Tcexp_tcExp(te,topt,e2);Cyc_Tcexp_tcExp(te,topt,e3);{void*t=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp27C=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp27C->v=Cyc_Tcenv_lookup_type_vars(te);_tmp27C;}));
struct Cyc_List_List*_tmp275=({struct Cyc_List_List*_tmp27B=_cycalloc(sizeof(
struct Cyc_List_List));_tmp27B->hd=e3;_tmp27B->tl=0;_tmp27B;});struct Cyc_List_List*
_tmp276=({struct Cyc_List_List*_tmp27A=_cycalloc(sizeof(struct Cyc_List_List));
_tmp27A->hd=e2;_tmp27A->tl=_tmp275;_tmp27A;});if(! Cyc_Tcutil_coerce_list(te,t,
_tmp276)){({struct Cyc_Std_String_pa_struct _tmp279;_tmp279.tag=Cyc_Std_String_pa;
_tmp279.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e3->topt))->v);{struct Cyc_Std_String_pa_struct _tmp278;_tmp278.tag=
Cyc_Std_String_pa;_tmp278.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(e2->topt))->v);{void*_tmp277[2]={& _tmp278,&
_tmp279};Cyc_Tcutil_terr(loc,_tag_arr("conditional clause types do not match: %s != %s",
sizeof(unsigned char),48),_tag_arr(_tmp277,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
return t;}}static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*
e2){Cyc_Tcexp_tcExpNoPromote(te,0,e1);Cyc_Tcexp_tcExp(te,(void**)((void**)((void*)&((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v)),e2);{void*t1=(void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v;void*t2=(void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v;{
void*_tmp27D=Cyc_Tcutil_compress(t1);_LL639: if((unsigned int)_tmp27D > 3?*((int*)
_tmp27D)== Cyc_Absyn_ArrayType: 0){goto _LL640;}else{goto _LL641;}_LL641: goto _LL642;
_LL640:({void*_tmp283[0]={};Cyc_Tcutil_terr(loc,_tag_arr("cannot assign to an array",
sizeof(unsigned char),26),_tag_arr(_tmp283,sizeof(void*),0));});goto _LL638;
_LL642: goto _LL638;_LL638:;}if(! Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),(void*)
Cyc_Absyn_MemKind)){({void*_tmp284[0]={};Cyc_Tcutil_terr(loc,_tag_arr("type is abstract (can't determine size).",
sizeof(unsigned char),41),_tag_arr(_tmp284,sizeof(void*),0));});}if(! Cyc_Absyn_is_lvalue(
e1)){return({void*_tmp285[0]={};Cyc_Tcexp_expr_err(te,loc,_tag_arr("assignment to non-lvalue",
sizeof(unsigned char),25),_tag_arr(_tmp285,sizeof(void*),0));});}Cyc_Tcexp_check_writable(
te,e1);if(po == 0){if(! Cyc_Tcutil_coerce_assign(te,e2,t1)){void*_tmp286=({struct
Cyc_Std_String_pa_struct _tmp289;_tmp289.tag=Cyc_Std_String_pa;_tmp289.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct _tmp288;
_tmp288.tag=Cyc_Std_String_pa;_tmp288.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t1);{void*_tmp287[2]={& _tmp288,& _tmp289};Cyc_Tcexp_expr_err(te,loc,_tag_arr("type mismatch: %s != %s",
sizeof(unsigned char),24),_tag_arr(_tmp287,sizeof(void*),2));}}});Cyc_Tcutil_unify(
t1,t2);Cyc_Tcutil_explain_failure();return _tmp286;}}else{void*_tmp28A=(void*)po->v;
void*_tmp28B=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp28A,e1,e2);if(!(Cyc_Tcutil_unify(
_tmp28B,t1)? 1:(Cyc_Tcutil_coerceable(_tmp28B)? Cyc_Tcutil_coerceable(t1): 0))){
void*_tmp28C=({struct Cyc_Std_String_pa_struct _tmp28F;_tmp28F.tag=Cyc_Std_String_pa;
_tmp28F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct
_tmp28E;_tmp28E.tag=Cyc_Std_String_pa;_tmp28E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t1);{void*_tmp28D[2]={& _tmp28E,& _tmp28F};Cyc_Tcexp_expr_err(te,loc,_tag_arr("Cannot use this operator in an assignment when the arguments have types %s and %s",
sizeof(unsigned char),82),_tag_arr(_tmp28D,sizeof(void*),2));}}});Cyc_Tcutil_unify(
_tmp28B,t1);Cyc_Tcutil_explain_failure();return _tmp28C;}return _tmp28B;}return t1;}}
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,topt,e2);return(void*)((struct Cyc_Core_Opt*)_check_null(
e2->topt))->v;}static struct Cyc_Absyn_DoubleType_struct Cyc_Tcexp_dbl={6,0};static
void*Cyc_Tcexp_dbl_t=(void*)& Cyc_Tcexp_dbl;static struct Cyc_Absyn_Tunionfield*Cyc_Tcexp_tcInjection(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,
struct Cyc_List_List*inst,struct Cyc_List_List*fs){struct Cyc_List_List*fields;void*
t1=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;{void*_tmp291=Cyc_Tcutil_compress(
t1);void*_tmp29B;void*_tmp29D;_LL659: if(_tmp291 == (void*)Cyc_Absyn_FloatType){
goto _LL660;}else{goto _LL661;}_LL661: if((unsigned int)_tmp291 > 3?*((int*)_tmp291)
== Cyc_Absyn_IntType: 0){_LL668: _tmp29B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp291)->f2;if(_tmp29B == (void*)Cyc_Absyn_B1){goto _LL662;}else{goto _LL663;}}
else{goto _LL663;}_LL663: if((unsigned int)_tmp291 > 3?*((int*)_tmp291)== Cyc_Absyn_IntType:
0){_LL670: _tmp29D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp291)->f2;if(
_tmp29D == (void*)Cyc_Absyn_B2){goto _LL664;}else{goto _LL665;}}else{goto _LL665;}
_LL665: goto _LL666;_LL660: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Tcexp_dbl_t);t1=Cyc_Tcexp_dbl_t;
goto _LL658;_LL662: goto _LL664;_LL664: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_t);
t1=Cyc_Absyn_sint_t;goto _LL658;_LL666: goto _LL658;_LL658:;}for(fields=fs;fields != 
0;fields=fields->tl){void*_tmp2A1;struct Cyc_Position_Segment*_tmp2A3;struct Cyc_List_List*
_tmp2A5;struct Cyc_List_List*_tmp2A7;struct _tuple1*_tmp2A9;struct Cyc_Absyn_Tunionfield
_tmp29F=*((struct Cyc_Absyn_Tunionfield*)fields->hd);_LL682: _tmp2A9=_tmp29F.name;
goto _LL680;_LL680: _tmp2A7=_tmp29F.tvs;goto _LL678;_LL678: _tmp2A5=_tmp29F.typs;
goto _LL676;_LL676: _tmp2A3=_tmp29F.loc;goto _LL674;_LL674: _tmp2A1=(void*)_tmp29F.sc;
goto _LL672;_LL672: if((_tmp2A5 == 0? 1: _tmp2A5->tl != 0)? 1: _tmp2A7 != 0){continue;}{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple8*)_tmp2A5->hd)).f2);if(
Cyc_Tcutil_unify(t1,t2)){return(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)
fields->hd);}}}for(fields=fs;fields != 0;fields=fields->tl){void*_tmp2AD;struct
Cyc_Position_Segment*_tmp2AF;struct Cyc_List_List*_tmp2B1;struct Cyc_List_List*
_tmp2B3;struct _tuple1*_tmp2B5;struct Cyc_Absyn_Tunionfield _tmp2AB=*((struct Cyc_Absyn_Tunionfield*)
fields->hd);_LL694: _tmp2B5=_tmp2AB.name;goto _LL692;_LL692: _tmp2B3=_tmp2AB.tvs;
goto _LL690;_LL690: _tmp2B1=_tmp2AB.typs;goto _LL688;_LL688: _tmp2AF=_tmp2AB.loc;
goto _LL686;_LL686: _tmp2AD=(void*)_tmp2AB.sc;goto _LL684;_LL684: if((_tmp2B1 == 0? 1:
_tmp2B1->tl != 0)? 1: _tmp2B3 != 0){continue;}{void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((
struct _tuple8*)_tmp2B1->hd)).f2);if(Cyc_Tcutil_coerce_arg(te,e,t2)){return(
struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)fields->hd);}}}({
struct Cyc_Std_String_pa_struct _tmp2B9;_tmp2B9.tag=Cyc_Std_String_pa;_tmp2B9.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{struct Cyc_Std_String_pa_struct
_tmp2B8;_tmp2B8.tag=Cyc_Std_String_pa;_tmp2B8.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
tu);{void*_tmp2B7[2]={& _tmp2B8,& _tmp2B9};Cyc_Tcutil_terr(e->loc,_tag_arr("can't find a field in %s to inject a value of type %s",
sizeof(unsigned char),54),_tag_arr(_tmp2B7,sizeof(void*),2));}}});return 0;}
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**
vararg_call_info){struct Cyc_List_List*_tmp2BA=args;te=Cyc_Tcenv_new_block(loc,te);
Cyc_Tcexp_tcExp(te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);void*_tmp2BB=t;struct Cyc_Absyn_PtrInfo _tmp2C1;struct
Cyc_Absyn_Conref*_tmp2C3;struct Cyc_Absyn_Tqual _tmp2C5;struct Cyc_Absyn_Conref*
_tmp2C7;void*_tmp2C9;void*_tmp2CB;_LL701: if((unsigned int)_tmp2BB > 3?*((int*)
_tmp2BB)== Cyc_Absyn_PointerType: 0){_LL706: _tmp2C1=((struct Cyc_Absyn_PointerType_struct*)
_tmp2BB)->f1;_LL716: _tmp2CB=(void*)_tmp2C1.elt_typ;goto _LL714;_LL714: _tmp2C9=(
void*)_tmp2C1.rgn_typ;goto _LL712;_LL712: _tmp2C7=_tmp2C1.nullable;goto _LL710;
_LL710: _tmp2C5=_tmp2C1.tq;goto _LL708;_LL708: _tmp2C3=_tmp2C1.bounds;goto _LL702;}
else{goto _LL703;}_LL703: goto _LL704;_LL702: Cyc_Tcenv_check_rgn_accessible(te,loc,
_tmp2C9);Cyc_Tcutil_check_nonzero_bound(loc,_tmp2C3);{void*_tmp2CD=Cyc_Tcutil_compress(
_tmp2CB);struct Cyc_Absyn_FnInfo _tmp2D3;struct Cyc_List_List*_tmp2D5;struct Cyc_List_List*
_tmp2D7;struct Cyc_Absyn_VarargInfo*_tmp2D9;int _tmp2DB;struct Cyc_List_List*
_tmp2DD;void*_tmp2DF;struct Cyc_Core_Opt*_tmp2E1;struct Cyc_List_List*_tmp2E3;
_LL719: if((unsigned int)_tmp2CD > 3?*((int*)_tmp2CD)== Cyc_Absyn_FnType: 0){_LL724:
_tmp2D3=((struct Cyc_Absyn_FnType_struct*)_tmp2CD)->f1;_LL740: _tmp2E3=_tmp2D3.tvars;
goto _LL738;_LL738: _tmp2E1=_tmp2D3.effect;goto _LL736;_LL736: _tmp2DF=(void*)
_tmp2D3.ret_typ;goto _LL734;_LL734: _tmp2DD=_tmp2D3.args;goto _LL732;_LL732: _tmp2DB=
_tmp2D3.c_varargs;goto _LL730;_LL730: _tmp2D9=_tmp2D3.cyc_varargs;goto _LL728;
_LL728: _tmp2D7=_tmp2D3.rgn_po;goto _LL726;_LL726: _tmp2D5=_tmp2D3.attributes;goto
_LL720;}else{goto _LL721;}_LL721: goto _LL722;_LL720: if(topt != 0){Cyc_Tcutil_unify(
_tmp2DF,*topt);}while(_tmp2BA != 0? _tmp2DD != 0: 0){struct Cyc_Absyn_Exp*e1=(struct
Cyc_Absyn_Exp*)_tmp2BA->hd;void*t2=(*((struct _tuple2*)_tmp2DD->hd)).f3;Cyc_Tcexp_tcExp(
te,(void**)& t2,e1);if(! Cyc_Tcutil_coerce_arg(te,e1,t2)){({struct Cyc_Std_String_pa_struct
_tmp2E7;_tmp2E7.tag=Cyc_Std_String_pa;_tmp2E7.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t2);{struct Cyc_Std_String_pa_struct _tmp2E6;_tmp2E6.tag=Cyc_Std_String_pa;_tmp2E6.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);{void*_tmp2E5[2]={& _tmp2E6,& _tmp2E7};Cyc_Tcutil_terr(
e1->loc,_tag_arr("actual argument has type \n%s\n but formal has type \n%s",
sizeof(unsigned char),54),_tag_arr(_tmp2E5,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}
_tmp2BA=_tmp2BA->tl;_tmp2DD=_tmp2DD->tl;}if(_tmp2DD != 0){({void*_tmp2E8[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too few arguments for function",sizeof(
unsigned char),31),_tag_arr(_tmp2E8,sizeof(void*),0));});}else{if((_tmp2BA != 0? 1:
_tmp2DB)? 1: _tmp2D9 != 0){if(_tmp2DB){for(0;_tmp2BA != 0;_tmp2BA=_tmp2BA->tl){Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmp2BA->hd);}}else{if(_tmp2D9 == 0){({void*_tmp2E9[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too many arguments for function",sizeof(
unsigned char),32),_tag_arr(_tmp2E9,sizeof(void*),0));});}else{int _tmp2EC;void*
_tmp2EE;struct Cyc_Absyn_VarargInfo _tmp2EA=*_tmp2D9;_LL751: _tmp2EE=(void*)_tmp2EA.type;
goto _LL749;_LL749: _tmp2EC=_tmp2EA.inject;goto _LL747;_LL747: {struct Cyc_Absyn_VarargCallInfo*
_tmp2F0=({struct Cyc_Absyn_VarargCallInfo*_tmp30E=_cycalloc(sizeof(struct Cyc_Absyn_VarargCallInfo));
_tmp30E->num_varargs=0;_tmp30E->injectors=0;_tmp30E->vai=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmp2D9);_tmp30E;});*vararg_call_info=(struct Cyc_Absyn_VarargCallInfo*)
_tmp2F0;if(! _tmp2EC){for(0;_tmp2BA != 0;_tmp2BA=_tmp2BA->tl){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)_tmp2BA->hd;_tmp2F0->num_varargs ++;Cyc_Tcexp_tcExp(te,(
void**)& _tmp2EE,e1);if(! Cyc_Tcutil_coerce_arg(te,e1,_tmp2EE)){({struct Cyc_Std_String_pa_struct
_tmp2F3;_tmp2F3.tag=Cyc_Std_String_pa;_tmp2F3.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e1->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp2F2;_tmp2F2.tag=Cyc_Std_String_pa;_tmp2F2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp2EE);{void*_tmp2F1[2]={& _tmp2F2,& _tmp2F3};Cyc_Tcutil_terr(loc,_tag_arr("vararg requires type %s but argument has type %s",
sizeof(unsigned char),49),_tag_arr(_tmp2F1,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();}}}
else{void*_tmp2F4=Cyc_Tcutil_compress(_tmp2EE);struct Cyc_Absyn_TunionInfo _tmp2FA;
void*_tmp2FC;struct Cyc_List_List*_tmp2FE;void*_tmp300;struct Cyc_Absyn_Tuniondecl**
_tmp302;struct Cyc_Absyn_Tuniondecl*_tmp304;_LL758: if((unsigned int)_tmp2F4 > 3?*((
int*)_tmp2F4)== Cyc_Absyn_TunionType: 0){_LL763: _tmp2FA=((struct Cyc_Absyn_TunionType_struct*)
_tmp2F4)->f1;_LL769: _tmp300=(void*)_tmp2FA.tunion_info;if(*((int*)_tmp300)== Cyc_Absyn_KnownTunion){
_LL771: _tmp302=((struct Cyc_Absyn_KnownTunion_struct*)_tmp300)->f1;_tmp304=*
_tmp302;goto _LL767;}else{goto _LL760;}_LL767: _tmp2FE=_tmp2FA.targs;goto _LL765;
_LL765: _tmp2FC=(void*)_tmp2FA.rgn;goto _LL759;}else{goto _LL760;}_LL760: goto _LL761;
_LL759: {struct Cyc_Absyn_Tuniondecl*_tmp305=*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmp304->name);struct Cyc_List_List*fields=0;if(_tmp305->fields == 0){({struct Cyc_Std_String_pa_struct
_tmp307;_tmp307.tag=Cyc_Std_String_pa;_tmp307.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp2EE);{void*_tmp306[1]={& _tmp307};Cyc_Tcutil_terr(loc,_tag_arr("can't inject into %s",
sizeof(unsigned char),21),_tag_arr(_tmp306,sizeof(void*),1));}});}else{fields=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp305->fields))->v;}if(
! Cyc_Tcutil_unify(_tmp2FC,Cyc_Tcenv_curr_rgn(te))){({void*_tmp308[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("bad region for injected varargs",sizeof(unsigned char),32),
_tag_arr(_tmp308,sizeof(void*),0));});}{struct _RegionHandle _tmp309=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp309;_push_region(rgn);{struct Cyc_List_List*_tmp30A=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp305->tvs,_tmp2FE);for(0;
_tmp2BA != 0;_tmp2BA=_tmp2BA->tl){_tmp2F0->num_varargs ++;{struct Cyc_Absyn_Exp*e1=(
struct Cyc_Absyn_Exp*)_tmp2BA->hd;Cyc_Tcexp_tcExp(te,0,e1);{struct Cyc_Absyn_Tunionfield*
_tmp30B=Cyc_Tcexp_tcInjection(te,e1,_tmp2EE,rgn,_tmp30A,fields);if(_tmp30B != 0){
_tmp2F0->injectors=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp2F0->injectors,({struct Cyc_List_List*_tmp30C=_cycalloc(
sizeof(struct Cyc_List_List));_tmp30C->hd=(struct Cyc_Absyn_Tunionfield*)
_check_null(_tmp30B);_tmp30C->tl=0;_tmp30C;}));}}}}};_pop_region(rgn);}goto
_LL757;}_LL761:({void*_tmp30D[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad inject vararg type",
sizeof(unsigned char),23),_tag_arr(_tmp30D,sizeof(void*),0));});goto _LL757;
_LL757:;}}}}}}{struct Cyc_List_List*a=_tmp2D5;for(0;a != 0;a=a->tl){void*_tmp30F=(
void*)a->hd;int _tmp315;int _tmp317;void*_tmp319;_LL785: if((unsigned int)_tmp30F > 
16?*((int*)_tmp30F)== Cyc_Absyn_Format_att: 0){_LL794: _tmp319=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp30F)->f1;goto _LL792;_LL792: _tmp317=((struct Cyc_Absyn_Format_att_struct*)
_tmp30F)->f2;goto _LL790;_LL790: _tmp315=((struct Cyc_Absyn_Format_att_struct*)
_tmp30F)->f3;goto _LL786;}else{goto _LL787;}_LL787: goto _LL788;_LL786:{struct
_handler_cons _tmp31B;_push_handler(& _tmp31B);{int _tmp31D=0;if(setjmp(_tmp31B.handler)){
_tmp31D=1;}if(! _tmp31D){{struct Cyc_Absyn_Exp*_tmp31E=((struct Cyc_Absyn_Exp*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp317 - 1);struct Cyc_Core_Opt*
fmt_args;if(_tmp315 == 0){fmt_args=0;}else{fmt_args=({struct Cyc_Core_Opt*_tmp31F=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp31F->v=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp315 - 1);_tmp31F;});}{
void*_tmp320=_tmp319;_LL802: if(_tmp320 == (void*)Cyc_Absyn_Printf_ft){goto _LL803;}
else{goto _LL804;}_LL804: if(_tmp320 == (void*)Cyc_Absyn_Scanf_ft){goto _LL805;}
else{goto _LL801;}_LL803: Cyc_Tcexp_check_format_args(te,_tmp31E,fmt_args,Cyc_Formatstr_get_format_typs);
goto _LL801;_LL805: Cyc_Tcexp_check_format_args(te,_tmp31E,fmt_args,Cyc_Formatstr_get_scanf_typs);
goto _LL801;_LL801:;}};_pop_handler();}else{void*_tmp31C=(void*)_exn_thrown;void*
_tmp327=_tmp31C;_LL809: if(_tmp327 == Cyc_List_Nth){goto _LL810;}else{goto _LL811;}
_LL811: goto _LL812;_LL810:({void*_tmp32D[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad format arguments",
sizeof(unsigned char),21),_tag_arr(_tmp32D,sizeof(void*),0));});goto _LL808;
_LL812:(void)_throw(_tmp327);_LL808:;}}}goto _LL784;_LL788: goto _LL784;_LL784:;}}
Cyc_Tcenv_check_effect_accessible(te,loc,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp2E1))->v);Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp2D7);
return _tmp2DF;_LL722: return({void*_tmp32E[0]={};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("expected pointer to function",sizeof(unsigned char),29),_tag_arr(
_tmp32E,sizeof(void*),0));});_LL718:;}_LL704: return({void*_tmp32F[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expected pointer to function",sizeof(unsigned char),29),
_tag_arr(_tmp32F,sizeof(void*),0));});_LL700:;}}static void*Cyc_Tcexp_tcThrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e){Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_exn_typ,e);if(! Cyc_Tcutil_coerce_arg(te,
e,Cyc_Absyn_exn_typ)){({struct Cyc_Std_String_pa_struct _tmp331;_tmp331.tag=Cyc_Std_String_pa;
_tmp331.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);{void*_tmp330[1]={& _tmp331};Cyc_Tcutil_terr(loc,
_tag_arr("expected xtunion exn but found %s",sizeof(unsigned char),34),_tag_arr(
_tmp330,sizeof(void*),1));}});}return Cyc_Absyn_wildtyp(({struct Cyc_Core_Opt*
_tmp332=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp332->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp332;}));}static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
ts){Cyc_Tcexp_tcExpNoInst(te,0,e);(void*)(((struct Cyc_Core_Opt*)_check_null(e->topt))->v=(
void*)Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp333=t1;struct Cyc_Absyn_PtrInfo _tmp339;struct Cyc_Absyn_Conref*_tmp33B;
struct Cyc_Absyn_Tqual _tmp33D;struct Cyc_Absyn_Conref*_tmp33F;void*_tmp341;void*
_tmp343;_LL821: if((unsigned int)_tmp333 > 3?*((int*)_tmp333)== Cyc_Absyn_PointerType:
0){_LL826: _tmp339=((struct Cyc_Absyn_PointerType_struct*)_tmp333)->f1;_LL836:
_tmp343=(void*)_tmp339.elt_typ;goto _LL834;_LL834: _tmp341=(void*)_tmp339.rgn_typ;
goto _LL832;_LL832: _tmp33F=_tmp339.nullable;goto _LL830;_LL830: _tmp33D=_tmp339.tq;
goto _LL828;_LL828: _tmp33B=_tmp339.bounds;goto _LL822;}else{goto _LL823;}_LL823:
goto _LL824;_LL822:{void*_tmp345=Cyc_Tcutil_compress(_tmp343);struct Cyc_Absyn_FnInfo
_tmp34B;struct Cyc_List_List*_tmp34D;struct Cyc_List_List*_tmp34F;struct Cyc_Absyn_VarargInfo*
_tmp351;int _tmp353;struct Cyc_List_List*_tmp355;void*_tmp357;struct Cyc_Core_Opt*
_tmp359;struct Cyc_List_List*_tmp35B;_LL839: if((unsigned int)_tmp345 > 3?*((int*)
_tmp345)== Cyc_Absyn_FnType: 0){_LL844: _tmp34B=((struct Cyc_Absyn_FnType_struct*)
_tmp345)->f1;_LL860: _tmp35B=_tmp34B.tvars;goto _LL858;_LL858: _tmp359=_tmp34B.effect;
goto _LL856;_LL856: _tmp357=(void*)_tmp34B.ret_typ;goto _LL854;_LL854: _tmp355=
_tmp34B.args;goto _LL852;_LL852: _tmp353=_tmp34B.c_varargs;goto _LL850;_LL850:
_tmp351=_tmp34B.cyc_varargs;goto _LL848;_LL848: _tmp34F=_tmp34B.rgn_po;goto _LL846;
_LL846: _tmp34D=_tmp34B.attributes;goto _LL840;}else{goto _LL841;}_LL841: goto _LL842;
_LL840: {struct Cyc_List_List*instantiation=0;if(Cyc_List_length(ts)!= ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp35B)){return({void*_tmp35D[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("type instantiation mismatch",sizeof(unsigned char),28),_tag_arr(
_tmp35D,sizeof(void*),0));});}for(0;ts != 0;(ts=ts->tl,_tmp35B=_tmp35B->tl)){void*
k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp35B))->hd);Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(
void*)ts->hd);instantiation=({struct Cyc_List_List*_tmp35E=_cycalloc(sizeof(
struct Cyc_List_List));_tmp35E->hd=({struct _tuple5*_tmp35F=_cycalloc(sizeof(
struct _tuple5));_tmp35F->f1=(struct Cyc_Absyn_Tvar*)_tmp35B->hd;_tmp35F->f2=(void*)
ts->hd;_tmp35F;});_tmp35E->tl=instantiation;_tmp35E;});}{void*new_fn_typ=Cyc_Tcutil_substitute(
instantiation,(void*)({struct Cyc_Absyn_FnType_struct*_tmp363=_cycalloc(sizeof(
struct Cyc_Absyn_FnType_struct));_tmp363[0]=({struct Cyc_Absyn_FnType_struct
_tmp364;_tmp364.tag=Cyc_Absyn_FnType;_tmp364.f1=({struct Cyc_Absyn_FnInfo _tmp365;
_tmp365.tvars=0;_tmp365.effect=_tmp359;_tmp365.ret_typ=(void*)_tmp357;_tmp365.args=
_tmp355;_tmp365.c_varargs=_tmp353;_tmp365.cyc_varargs=_tmp351;_tmp365.rgn_po=
_tmp34F;_tmp365.attributes=_tmp34D;_tmp365;});_tmp364;});_tmp363;}));void*
new_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp360=_cycalloc(sizeof(
struct Cyc_Absyn_PointerType_struct));_tmp360[0]=({struct Cyc_Absyn_PointerType_struct
_tmp361;_tmp361.tag=Cyc_Absyn_PointerType;_tmp361.f1=({struct Cyc_Absyn_PtrInfo
_tmp362;_tmp362.elt_typ=(void*)new_fn_typ;_tmp362.rgn_typ=(void*)_tmp341;_tmp362.nullable=
_tmp33F;_tmp362.tq=_tmp33D;_tmp362.bounds=_tmp33B;_tmp362;});_tmp361;});_tmp360;});
return new_typ;}}_LL842: goto _LL838;_LL838:;}goto _LL820;_LL824: goto _LL820;_LL820:;}
return({struct Cyc_Std_String_pa_struct _tmp367;_tmp367.tag=Cyc_Std_String_pa;
_tmp367.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp366[1]={&
_tmp367};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting polymorphic type but found %s",
sizeof(unsigned char),40),_tag_arr(_tmp366,sizeof(void*),1));}});}}static void*
Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,void*t,struct Cyc_Absyn_Exp*e){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)Cyc_Absyn_MemKind,1,t);Cyc_Tcexp_tcExp(te,0,e);{void*t2=(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;if((! Cyc_Tcutil_silent_castable(te,
loc,t2,t)? ! Cyc_Tcutil_castable(te,loc,t2,t): 0)? ! Cyc_Tcutil_zero_to_null(te,t,e):
0){Cyc_Tcutil_unify(t2,t);{void*_tmp368=({struct Cyc_Std_String_pa_struct _tmp36B;
_tmp36B.tag=Cyc_Std_String_pa;_tmp36B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{struct Cyc_Std_String_pa_struct _tmp36A;_tmp36A.tag=Cyc_Std_String_pa;_tmp36A.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmp369[2]={& _tmp36A,& _tmp36B};
Cyc_Tcexp_expr_err(te,loc,_tag_arr("cannot cast %s to %s",sizeof(unsigned char),
21),_tag_arr(_tmp369,sizeof(void*),2));}}});Cyc_Tcutil_explain_failure();return
_tmp368;}}return t;}}static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*
e){void**_tmp36C=0;struct Cyc_Absyn_Tqual _tmp36D=Cyc_Absyn_empty_tqual();if(topt
!= 0){void*_tmp36E=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp374;
struct Cyc_Absyn_Tqual _tmp376;void*_tmp378;_LL880: if((unsigned int)_tmp36E > 3?*((
int*)_tmp36E)== Cyc_Absyn_PointerType: 0){_LL885: _tmp374=((struct Cyc_Absyn_PointerType_struct*)
_tmp36E)->f1;_LL889: _tmp378=(void*)_tmp374.elt_typ;goto _LL887;_LL887: _tmp376=
_tmp374.tq;goto _LL881;}else{goto _LL882;}_LL882: goto _LL883;_LL881: _tmp36C=({void**
_tmp37A=_cycalloc(sizeof(void*));_tmp37A[0]=_tmp378;_tmp37A;});_tmp36D=_tmp376;
goto _LL879;_LL883: goto _LL879;_LL879:;}Cyc_Tcexp_tcExpNoInst(te,_tmp36C,e);{void*
_tmp37B=(void*)e->r;struct Cyc_Absyn_Exp*_tmp385;struct Cyc_Absyn_Exp*_tmp387;
_LL893: if(*((int*)_tmp37B)== Cyc_Absyn_Struct_e){goto _LL894;}else{goto _LL895;}
_LL895: if(*((int*)_tmp37B)== Cyc_Absyn_Tuple_e){goto _LL896;}else{goto _LL897;}
_LL897: if(*((int*)_tmp37B)== Cyc_Absyn_Subscript_e){_LL904: _tmp387=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp37B)->f1;goto _LL902;_LL902: _tmp385=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp37B)->f2;goto _LL898;}else{goto _LL899;}_LL899: goto _LL900;_LL894: goto _LL896;
_LL896:({void*_tmp389[0]={};Cyc_Tcutil_warn(loc,_tag_arr("& used to allocate",
sizeof(unsigned char),19),_tag_arr(_tmp389,sizeof(void*),0));});{void*(*_tmp38A)(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq)=Cyc_Absyn_at_typ;if(topt != 0){void*
_tmp38B=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp391;struct Cyc_Absyn_Conref*
_tmp393;_LL909: if((unsigned int)_tmp38B > 3?*((int*)_tmp38B)== Cyc_Absyn_PointerType:
0){_LL914: _tmp391=((struct Cyc_Absyn_PointerType_struct*)_tmp38B)->f1;_LL916:
_tmp393=_tmp391.nullable;goto _LL910;}else{goto _LL911;}_LL911: goto _LL912;_LL910:
if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp393)){_tmp38A=
Cyc_Absyn_star_typ;}goto _LL908;_LL912: goto _LL908;_LL908:;}return _tmp38A((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)Cyc_Absyn_HeapRgn,_tmp36D);}
_LL898:{void*_tmp395=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp387->topt))->v);_LL919: if((unsigned int)_tmp395 > 3?*((int*)
_tmp395)== Cyc_Absyn_TupleType: 0){goto _LL920;}else{goto _LL921;}_LL921: goto _LL922;
_LL920: goto _LL918;_LL922:(void*)(e0->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmp39B=_cycalloc(sizeof(struct Cyc_Absyn_Primop_e_struct));_tmp39B[0]=({struct
Cyc_Absyn_Primop_e_struct _tmp39C;_tmp39C.tag=Cyc_Absyn_Primop_e;_tmp39C.f1=(void*)((
void*)Cyc_Absyn_Plus);_tmp39C.f2=({struct Cyc_List_List*_tmp39D=_cycalloc(sizeof(
struct Cyc_List_List));_tmp39D->hd=_tmp387;_tmp39D->tl=({struct Cyc_List_List*
_tmp39E=_cycalloc(sizeof(struct Cyc_List_List));_tmp39E->hd=_tmp385;_tmp39E->tl=0;
_tmp39E;});_tmp39D;});_tmp39C;});_tmp39B;})));return Cyc_Tcexp_tcPlus(te,_tmp387,
_tmp385);_LL918:;}goto _LL892;_LL900: goto _LL892;_LL892:;}{void*_tmp3A1;int _tmp3A3;
struct _tuple6 _tmp39F=Cyc_Tcutil_addressof_props(te,e);_LL932: _tmp3A3=_tmp39F.f1;
goto _LL930;_LL930: _tmp3A1=_tmp39F.f2;goto _LL928;_LL928: {struct Cyc_Absyn_Tqual tq=
Cyc_Absyn_empty_tqual();if(_tmp3A3){tq.q_const=1;}{void*t=Cyc_Absyn_at_typ((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,_tmp3A1,tq);return t;}}}}static void*
Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,void*t){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),(void*)
Cyc_Absyn_MemKind,1,t);if(! Cyc_Evexp_okay_szofarg(t)){({struct Cyc_Std_String_pa_struct
_tmp3A6;_tmp3A6.tag=Cyc_Std_String_pa;_tmp3A6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{void*_tmp3A5[1]={& _tmp3A6};Cyc_Tcutil_terr(loc,_tag_arr("sizeof applied to type %s, which has unknown size here",
sizeof(unsigned char),55),_tag_arr(_tmp3A5,sizeof(void*),1));}});}if(topt != 0){
void*_tmp3A7=Cyc_Tcutil_compress(*topt);_LL937: if((unsigned int)_tmp3A7 > 3?*((
int*)_tmp3A7)== Cyc_Absyn_SizeofType: 0){goto _LL938;}else{goto _LL939;}_LL939: goto
_LL940;_LL938: return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp3AD=_cycalloc(
sizeof(struct Cyc_Absyn_SizeofType_struct));_tmp3AD[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp3AE;_tmp3AE.tag=Cyc_Absyn_SizeofType;_tmp3AE.f1=(void*)t;_tmp3AE;});_tmp3AD;});
_LL940: goto _LL936;_LL936:;}return Cyc_Absyn_uint_t;}int Cyc_Tcexp_structfield_has_name(
struct _tagged_arr*n,struct Cyc_Absyn_Structfield*sf){return Cyc_Std_strcmp(*n,*sf->name)
== 0;}static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,void*t,void*n){Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)Cyc_Absyn_MemKind,1,t);{void*_tmp3AF=n;struct _tagged_arr*_tmp3B5;
unsigned int _tmp3B7;_LL945: if(*((int*)_tmp3AF)== Cyc_Absyn_StructField){_LL950:
_tmp3B5=((struct Cyc_Absyn_StructField_struct*)_tmp3AF)->f1;goto _LL946;}else{goto
_LL947;}_LL947: if(*((int*)_tmp3AF)== Cyc_Absyn_TupleIndex){_LL952: _tmp3B7=((
struct Cyc_Absyn_TupleIndex_struct*)_tmp3AF)->f1;goto _LL948;}else{goto _LL944;}
_LL946: {int bad_type=1;{void*_tmp3B9=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Structdecl**
_tmp3C5;struct _tuple1*_tmp3C7;struct Cyc_List_List*_tmp3C9;struct Cyc_Absyn_Uniondecl**
_tmp3CB;struct _tuple1*_tmp3CD;struct Cyc_List_List*_tmp3CF;_LL955: if((
unsigned int)_tmp3B9 > 3?*((int*)_tmp3B9)== Cyc_Absyn_StructType: 0){_LL968:
_tmp3C7=((struct Cyc_Absyn_StructType_struct*)_tmp3B9)->f1;goto _LL966;_LL966:
_tmp3C5=((struct Cyc_Absyn_StructType_struct*)_tmp3B9)->f3;goto _LL956;}else{goto
_LL957;}_LL957: if((unsigned int)_tmp3B9 > 3?*((int*)_tmp3B9)== Cyc_Absyn_AnonStructType:
0){_LL970: _tmp3C9=((struct Cyc_Absyn_AnonStructType_struct*)_tmp3B9)->f1;goto
_LL958;}else{goto _LL959;}_LL959: if((unsigned int)_tmp3B9 > 3?*((int*)_tmp3B9)== 
Cyc_Absyn_UnionType: 0){_LL974: _tmp3CD=((struct Cyc_Absyn_UnionType_struct*)
_tmp3B9)->f1;goto _LL972;_LL972: _tmp3CB=((struct Cyc_Absyn_UnionType_struct*)
_tmp3B9)->f3;goto _LL960;}else{goto _LL961;}_LL961: if((unsigned int)_tmp3B9 > 3?*((
int*)_tmp3B9)== Cyc_Absyn_AnonUnionType: 0){_LL976: _tmp3CF=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp3B9)->f1;goto _LL962;}else{goto _LL963;}_LL963: goto _LL964;_LL956: if(_tmp3C5 == 
0){return({void*_tmp3D1[0]={};Cyc_Tcutil_impos(_tag_arr("offsetof on unchecked StructType",
sizeof(unsigned char),33),_tag_arr(_tmp3D1,sizeof(void*),0));});}if((*_tmp3C5)->fields
== 0){goto _LL954;}if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Structfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp3B5,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((*_tmp3C5)->fields))->v)){({
struct Cyc_Std_String_pa_struct _tmp3D3;_tmp3D3.tag=Cyc_Std_String_pa;_tmp3D3.f1=(
struct _tagged_arr)*_tmp3B5;{void*_tmp3D2[1]={& _tmp3D3};Cyc_Tcutil_terr(loc,
_tag_arr("no field of struct has name %s",sizeof(unsigned char),31),_tag_arr(
_tmp3D2,sizeof(void*),1));}});}bad_type=0;goto _LL954;_LL958: if(!((int(*)(int(*
pred)(struct _tagged_arr*,struct Cyc_Absyn_Structfield*),struct _tagged_arr*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp3B5,
_tmp3C9)){({struct Cyc_Std_String_pa_struct _tmp3D5;_tmp3D5.tag=Cyc_Std_String_pa;
_tmp3D5.f1=(struct _tagged_arr)*_tmp3B5;{void*_tmp3D4[1]={& _tmp3D5};Cyc_Tcutil_terr(
loc,_tag_arr("no field of struct has name %s",sizeof(unsigned char),31),_tag_arr(
_tmp3D4,sizeof(void*),1));}});}bad_type=0;goto _LL954;_LL960: if(_tmp3CB == 0){
return({void*_tmp3D6[0]={};Cyc_Tcutil_impos(_tag_arr("offsetof on unchecked UnionType",
sizeof(unsigned char),32),_tag_arr(_tmp3D6,sizeof(void*),0));});}if((*_tmp3CB)->fields
== 0){goto _LL954;}if(!((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Structfield*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,
_tmp3B5,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((*_tmp3CB)->fields))->v)){({
struct Cyc_Std_String_pa_struct _tmp3D8;_tmp3D8.tag=Cyc_Std_String_pa;_tmp3D8.f1=(
struct _tagged_arr)*_tmp3B5;{void*_tmp3D7[1]={& _tmp3D8};Cyc_Tcutil_terr(loc,
_tag_arr("no field of union has name %s",sizeof(unsigned char),30),_tag_arr(
_tmp3D7,sizeof(void*),1));}});}bad_type=0;goto _LL954;_LL962: if(!((int(*)(int(*
pred)(struct _tagged_arr*,struct Cyc_Absyn_Structfield*),struct _tagged_arr*env,
struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcexp_structfield_has_name,_tmp3B5,
_tmp3CF)){({struct Cyc_Std_String_pa_struct _tmp3DA;_tmp3DA.tag=Cyc_Std_String_pa;
_tmp3DA.f1=(struct _tagged_arr)*_tmp3B5;{void*_tmp3D9[1]={& _tmp3DA};Cyc_Tcutil_terr(
loc,_tag_arr("no field of anonymous union has name %s",sizeof(unsigned char),40),
_tag_arr(_tmp3D9,sizeof(void*),1));}});}bad_type=0;goto _LL954;_LL964: goto _LL954;
_LL954:;}if(bad_type){({struct Cyc_Std_String_pa_struct _tmp3DC;_tmp3DC.tag=Cyc_Std_String_pa;
_tmp3DC.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp3DB[1]={&
_tmp3DC};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known struct/union type",
sizeof(unsigned char),36),_tag_arr(_tmp3DB,sizeof(void*),1));}});}goto _LL944;}
_LL948: {int bad_type=1;{void*_tmp3DD=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Structdecl**
_tmp3ED;struct Cyc_List_List*_tmp3EF;struct Cyc_Absyn_Uniondecl**_tmp3F1;struct Cyc_List_List*
_tmp3F3;struct Cyc_List_List*_tmp3F5;struct Cyc_Absyn_TunionFieldInfo _tmp3F7;void*
_tmp3F9;struct Cyc_Absyn_Tunionfield*_tmp3FB;struct Cyc_Absyn_Tuniondecl*_tmp3FD;
_LL991: if((unsigned int)_tmp3DD > 3?*((int*)_tmp3DD)== Cyc_Absyn_StructType: 0){
_LL1006: _tmp3ED=((struct Cyc_Absyn_StructType_struct*)_tmp3DD)->f3;goto _LL992;}
else{goto _LL993;}_LL993: if((unsigned int)_tmp3DD > 3?*((int*)_tmp3DD)== Cyc_Absyn_AnonStructType:
0){_LL1008: _tmp3EF=((struct Cyc_Absyn_AnonStructType_struct*)_tmp3DD)->f1;goto
_LL994;}else{goto _LL995;}_LL995: if((unsigned int)_tmp3DD > 3?*((int*)_tmp3DD)== 
Cyc_Absyn_UnionType: 0){_LL1010: _tmp3F1=((struct Cyc_Absyn_UnionType_struct*)
_tmp3DD)->f3;goto _LL996;}else{goto _LL997;}_LL997: if((unsigned int)_tmp3DD > 3?*((
int*)_tmp3DD)== Cyc_Absyn_AnonUnionType: 0){_LL1012: _tmp3F3=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp3DD)->f1;goto _LL998;}else{goto _LL999;}_LL999: if((unsigned int)_tmp3DD > 3?*((
int*)_tmp3DD)== Cyc_Absyn_TupleType: 0){_LL1014: _tmp3F5=((struct Cyc_Absyn_TupleType_struct*)
_tmp3DD)->f1;goto _LL1000;}else{goto _LL1001;}_LL1001: if((unsigned int)_tmp3DD > 3?*((
int*)_tmp3DD)== Cyc_Absyn_TunionFieldType: 0){_LL1016: _tmp3F7=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3DD)->f1;_LL1018: _tmp3F9=(void*)_tmp3F7.field_info;if(*((int*)_tmp3F9)== Cyc_Absyn_KnownTunionfield){
_LL1022: _tmp3FD=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3F9)->f1;goto
_LL1020;_LL1020: _tmp3FB=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3F9)->f2;
goto _LL1002;}else{goto _LL1003;}}else{goto _LL1003;}_LL1003: goto _LL1004;_LL992: if(
_tmp3ED == 0){return({void*_tmp3FF[0]={};Cyc_Tcutil_impos(_tag_arr("offsetof on unchecked StructType",
sizeof(unsigned char),33),_tag_arr(_tmp3FF,sizeof(void*),0));});}if((*_tmp3ED)->fields
== 0){goto _LL990;}if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null((*_tmp3ED)->fields))->v)<= _tmp3B7){({struct Cyc_Std_Int_pa_struct
_tmp402;_tmp402.tag=Cyc_Std_Int_pa;_tmp402.f1=(int)_tmp3B7;{struct Cyc_Std_Int_pa_struct
_tmp401;_tmp401.tag=Cyc_Std_Int_pa;_tmp401.f1=(int)((unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null((*_tmp3ED)->fields))->v));{void*_tmp400[2]={& _tmp401,& _tmp402};Cyc_Tcutil_terr(
loc,_tag_arr("struct has too few components: %d <= %d",sizeof(unsigned char),40),
_tag_arr(_tmp400,sizeof(void*),2));}}});}bad_type=0;goto _LL990;_LL994: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp3EF)<= _tmp3B7){({struct Cyc_Std_Int_pa_struct
_tmp405;_tmp405.tag=Cyc_Std_Int_pa;_tmp405.f1=(int)_tmp3B7;{struct Cyc_Std_Int_pa_struct
_tmp404;_tmp404.tag=Cyc_Std_Int_pa;_tmp404.f1=(int)((unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp3EF));{void*_tmp403[2]={& _tmp404,&
_tmp405};Cyc_Tcutil_terr(loc,_tag_arr("struct has too few components: %d <= %d",
sizeof(unsigned char),40),_tag_arr(_tmp403,sizeof(void*),2));}}});}bad_type=0;
goto _LL990;_LL996: if(_tmp3F1 == 0){return({void*_tmp406[0]={};Cyc_Tcutil_impos(
_tag_arr("offsetof on unchecked UnionType",sizeof(unsigned char),32),_tag_arr(
_tmp406,sizeof(void*),0));});}if((*_tmp3F1)->fields == 0){goto _LL990;}if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null((*_tmp3F1)->fields))->v)<= _tmp3B7){({struct Cyc_Std_Int_pa_struct
_tmp409;_tmp409.tag=Cyc_Std_Int_pa;_tmp409.f1=(int)_tmp3B7;{struct Cyc_Std_Int_pa_struct
_tmp408;_tmp408.tag=Cyc_Std_Int_pa;_tmp408.f1=(int)((unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null((*_tmp3F1)->fields))->v));{void*_tmp407[2]={& _tmp408,& _tmp409};Cyc_Tcutil_terr(
loc,_tag_arr("union has too few components: %d <= %d",sizeof(unsigned char),39),
_tag_arr(_tmp407,sizeof(void*),2));}}});}bad_type=0;goto _LL990;_LL998: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp3F3)<= _tmp3B7){({struct Cyc_Std_Int_pa_struct
_tmp40C;_tmp40C.tag=Cyc_Std_Int_pa;_tmp40C.f1=(int)_tmp3B7;{struct Cyc_Std_Int_pa_struct
_tmp40B;_tmp40B.tag=Cyc_Std_Int_pa;_tmp40B.f1=(int)((unsigned int)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp3F3));{void*_tmp40A[2]={& _tmp40B,&
_tmp40C};Cyc_Tcutil_terr(loc,_tag_arr("union has too few components: %d <= %d",
sizeof(unsigned char),39),_tag_arr(_tmp40A,sizeof(void*),2));}}});}bad_type=0;
goto _LL990;_LL1000: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3F5)
<= _tmp3B7){({struct Cyc_Std_Int_pa_struct _tmp40F;_tmp40F.tag=Cyc_Std_Int_pa;
_tmp40F.f1=(int)_tmp3B7;{struct Cyc_Std_Int_pa_struct _tmp40E;_tmp40E.tag=Cyc_Std_Int_pa;
_tmp40E.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp3F5));{void*_tmp40D[2]={& _tmp40E,& _tmp40F};Cyc_Tcutil_terr(loc,_tag_arr("tuple has too few components: %d <= %d",
sizeof(unsigned char),39),_tag_arr(_tmp40D,sizeof(void*),2));}}});}bad_type=0;
goto _LL990;_LL1002: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp3FB->typs)
< _tmp3B7){({struct Cyc_Std_Int_pa_struct _tmp412;_tmp412.tag=Cyc_Std_Int_pa;
_tmp412.f1=(int)_tmp3B7;{struct Cyc_Std_Int_pa_struct _tmp411;_tmp411.tag=Cyc_Std_Int_pa;
_tmp411.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp3FB->typs));{void*_tmp410[2]={& _tmp411,& _tmp412};Cyc_Tcutil_terr(loc,
_tag_arr("tunionfield has too few components: %d < %d",sizeof(unsigned char),44),
_tag_arr(_tmp410,sizeof(void*),2));}}});}bad_type=0;goto _LL990;_LL1004: goto
_LL990;_LL990:;}if(bad_type){({struct Cyc_Std_String_pa_struct _tmp414;_tmp414.tag=
Cyc_Std_String_pa;_tmp414.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmp413[1]={& _tmp414};Cyc_Tcutil_terr(loc,_tag_arr("%s is not a known struct/union/tuple/tunion type",
sizeof(unsigned char),49),_tag_arr(_tmp413,sizeof(void*),1));}});}goto _LL944;}
_LL944:;}return Cyc_Absyn_uint_t;}static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e){Cyc_Tcexp_tcExp(
te,0,e);{void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*_tmp415=t;struct Cyc_Absyn_PtrInfo _tmp41B;struct Cyc_Absyn_Conref*_tmp41D;
void*_tmp41F;void*_tmp421;_LL1047: if((unsigned int)_tmp415 > 3?*((int*)_tmp415)== 
Cyc_Absyn_PointerType: 0){_LL1052: _tmp41B=((struct Cyc_Absyn_PointerType_struct*)
_tmp415)->f1;_LL1058: _tmp421=(void*)_tmp41B.elt_typ;goto _LL1056;_LL1056: _tmp41F=(
void*)_tmp41B.rgn_typ;goto _LL1054;_LL1054: _tmp41D=_tmp41B.bounds;goto _LL1048;}
else{goto _LL1049;}_LL1049: goto _LL1050;_LL1048: Cyc_Tcenv_check_rgn_accessible(te,
loc,_tmp41F);Cyc_Tcutil_check_nonzero_bound(loc,_tmp41D);if(! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp421),(void*)Cyc_Absyn_MemKind)){({void*_tmp423[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("can't dereference abstract pointer type",sizeof(unsigned char),40),
_tag_arr(_tmp423,sizeof(void*),0));});}return _tmp421;_LL1050: return({struct Cyc_Std_String_pa_struct
_tmp425;_tmp425.tag=Cyc_Std_String_pa;_tmp425.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{void*_tmp424[1]={& _tmp425};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting * or @ type but found %s",
sizeof(unsigned char),35),_tag_arr(_tmp424,sizeof(void*),1));}});_LL1046:;}}
static void*Cyc_Tcexp_tcStructMember(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct
_tagged_arr*f){Cyc_Tcexp_tcExpNoPromote(te,0,e);{void*t=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp426=t;struct Cyc_Absyn_Structdecl**
_tmp436;struct Cyc_Absyn_Structdecl*_tmp438;struct Cyc_List_List*_tmp439;struct
_tuple1*_tmp43B;struct Cyc_List_List*_tmp43D;struct Cyc_List_List*_tmp43F;struct
Cyc_Absyn_Uniondecl**_tmp441;struct Cyc_Absyn_Uniondecl*_tmp443;struct Cyc_List_List*
_tmp444;struct _tuple1*_tmp446;_LL1064: if((unsigned int)_tmp426 > 3?*((int*)
_tmp426)== Cyc_Absyn_StructType: 0){_LL1084: _tmp43B=((struct Cyc_Absyn_StructType_struct*)
_tmp426)->f1;goto _LL1082;_LL1082: _tmp439=((struct Cyc_Absyn_StructType_struct*)
_tmp426)->f2;goto _LL1079;_LL1079: _tmp436=((struct Cyc_Absyn_StructType_struct*)
_tmp426)->f3;if(_tmp436 == 0){goto _LL1066;}else{_tmp438=*_tmp436;goto _LL1065;}}
else{goto _LL1066;}_LL1066: if((unsigned int)_tmp426 > 3?*((int*)_tmp426)== Cyc_Absyn_AnonStructType:
0){_LL1086: _tmp43D=((struct Cyc_Absyn_AnonStructType_struct*)_tmp426)->f1;goto
_LL1067;}else{goto _LL1068;}_LL1068: if((unsigned int)_tmp426 > 3?*((int*)_tmp426)
== Cyc_Absyn_AnonUnionType: 0){_LL1088: _tmp43F=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp426)->f1;goto _LL1069;}else{goto _LL1070;}_LL1070: if((unsigned int)_tmp426 > 3?*((
int*)_tmp426)== Cyc_Absyn_UnionType: 0){_LL1095: _tmp446=((struct Cyc_Absyn_UnionType_struct*)
_tmp426)->f1;goto _LL1093;_LL1093: _tmp444=((struct Cyc_Absyn_UnionType_struct*)
_tmp426)->f2;goto _LL1090;_LL1090: _tmp441=((struct Cyc_Absyn_UnionType_struct*)
_tmp426)->f3;if(_tmp441 == 0){goto _LL1072;}else{_tmp443=*_tmp441;goto _LL1071;}}
else{goto _LL1072;}_LL1072: if((unsigned int)_tmp426 > 3?*((int*)_tmp426)== Cyc_Absyn_ArrayType:
0){goto _LL1096;}else{goto _LL1074;}_LL1096: if(Cyc_Std_zstrcmp(*f,_tag_arr("size",
sizeof(unsigned char),5))== 0){goto _LL1073;}else{goto _LL1074;}_LL1074: if((
unsigned int)_tmp426 > 3?*((int*)_tmp426)== Cyc_Absyn_PointerType: 0){goto _LL1097;}
else{goto _LL1076;}_LL1097: if(Cyc_Std_zstrcmp(*f,_tag_arr("size",sizeof(
unsigned char),5))== 0){goto _LL1075;}else{goto _LL1076;}_LL1076: goto _LL1077;
_LL1065: if(_tmp43B == 0){return({void*_tmp44A[0]={};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("unresolved struct type (COMPILER ERROR)",sizeof(unsigned char),40),
_tag_arr(_tmp44A,sizeof(void*),0));});}{struct Cyc_Absyn_Structfield*_tmp44B=Cyc_Absyn_lookup_struct_field(
_tmp438,f);if(_tmp44B == 0){return({struct Cyc_Std_String_pa_struct _tmp44E;_tmp44E.tag=
Cyc_Std_String_pa;_tmp44E.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct
_tmp44D;_tmp44D.tag=Cyc_Std_String_pa;_tmp44D.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((
struct _tuple1*)_check_null(_tmp43B));{void*_tmp44C[2]={& _tmp44D,& _tmp44E};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("struct %s has no %s field",sizeof(unsigned char),26),_tag_arr(
_tmp44C,sizeof(void*),2));}}});}{void*t2;{struct _RegionHandle _tmp44F=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp44F;_push_region(rgn);{struct Cyc_List_List*_tmp450=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp438->tvs,_tmp439);t2=Cyc_Tcutil_rsubstitute(
rgn,_tmp450,(void*)_tmp44B->type);};_pop_region(rgn);}return t2;}}_LL1067: {
struct Cyc_Absyn_Structfield*_tmp451=Cyc_Absyn_lookup_field(_tmp43D,f);if(_tmp451
== 0){return({struct Cyc_Std_String_pa_struct _tmp453;_tmp453.tag=Cyc_Std_String_pa;
_tmp453.f1=(struct _tagged_arr)*f;{void*_tmp452[1]={& _tmp453};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("struct has no %s field",sizeof(unsigned char),23),_tag_arr(
_tmp452,sizeof(void*),1));}});}return(void*)_tmp451->type;}_LL1069: {struct Cyc_Absyn_Structfield*
_tmp454=Cyc_Absyn_lookup_field(_tmp43F,f);if(_tmp454 == 0){return({struct Cyc_Std_String_pa_struct
_tmp456;_tmp456.tag=Cyc_Std_String_pa;_tmp456.f1=(struct _tagged_arr)*f;{void*
_tmp455[1]={& _tmp456};Cyc_Tcexp_expr_err(te,loc,_tag_arr("union has no %s field",
sizeof(unsigned char),22),_tag_arr(_tmp455,sizeof(void*),1));}});}return(void*)
_tmp454->type;}_LL1071: if(_tmp446 == 0){return({void*_tmp457[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("unresolved union type (COMPILER ERROR)",sizeof(unsigned char),
39),_tag_arr(_tmp457,sizeof(void*),0));});}{struct Cyc_Absyn_Structfield*_tmp458=
Cyc_Absyn_lookup_union_field(_tmp443,f);if(_tmp458 == 0){return({struct Cyc_Std_String_pa_struct
_tmp45B;_tmp45B.tag=Cyc_Std_String_pa;_tmp45B.f1=(struct _tagged_arr)*f;{struct
Cyc_Std_String_pa_struct _tmp45A;_tmp45A.tag=Cyc_Std_String_pa;_tmp45A.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string((struct _tuple1*)_check_null(_tmp446));{void*
_tmp459[2]={& _tmp45A,& _tmp45B};Cyc_Tcexp_expr_err(te,loc,_tag_arr("union %s has no %s field",
sizeof(unsigned char),25),_tag_arr(_tmp459,sizeof(void*),2));}}});}{struct Cyc_List_List*
_tmp45C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmp443->tvs,_tmp444);void*_tmp45D=Cyc_Tcutil_substitute(_tmp45C,(
void*)_tmp458->type);return _tmp45D;}}_LL1073:(void*)(outer_e->r=(void*)((void*)({
struct Cyc_Absyn_Primop_e_struct*_tmp45E=_cycalloc(sizeof(struct Cyc_Absyn_Primop_e_struct));
_tmp45E[0]=({struct Cyc_Absyn_Primop_e_struct _tmp45F;_tmp45F.tag=Cyc_Absyn_Primop_e;
_tmp45F.f1=(void*)((void*)Cyc_Absyn_Size);_tmp45F.f2=({struct Cyc_List_List*
_tmp460=_cycalloc(sizeof(struct Cyc_List_List));_tmp460->hd=e;_tmp460->tl=0;
_tmp460;});_tmp45F;});_tmp45E;})));return Cyc_Absyn_uint_t;_LL1075:(void*)(
outer_e->r=(void*)((void*)({struct Cyc_Absyn_Primop_e_struct*_tmp461=_cycalloc(
sizeof(struct Cyc_Absyn_Primop_e_struct));_tmp461[0]=({struct Cyc_Absyn_Primop_e_struct
_tmp462;_tmp462.tag=Cyc_Absyn_Primop_e;_tmp462.f1=(void*)((void*)Cyc_Absyn_Size);
_tmp462.f2=({struct Cyc_List_List*_tmp463=_cycalloc(sizeof(struct Cyc_List_List));
_tmp463->hd=e;_tmp463->tl=0;_tmp463;});_tmp462;});_tmp461;})));return Cyc_Absyn_uint_t;
_LL1077: if(Cyc_Std_zstrcmp(*f,_tag_arr("size",sizeof(unsigned char),5))== 0){
return({struct Cyc_Std_String_pa_struct _tmp465;_tmp465.tag=Cyc_Std_String_pa;
_tmp465.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp464[1]={&
_tmp465};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting struct, union or array, found %s",
sizeof(unsigned char),43),_tag_arr(_tmp464,sizeof(void*),1));}});}else{return({
struct Cyc_Std_String_pa_struct _tmp467;_tmp467.tag=Cyc_Std_String_pa;_tmp467.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp466[1]={& _tmp467};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("expecting struct or union, found %s",sizeof(unsigned char),36),
_tag_arr(_tmp466,sizeof(void*),1));}});}_LL1063:;}}static void*Cyc_Tcexp_tcStructArrow(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
e,struct _tagged_arr*f){Cyc_Tcexp_tcExp(te,0,e);{void*t=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);void*_tmp468=t;struct Cyc_Absyn_PtrInfo
_tmp46E;struct Cyc_Absyn_Conref*_tmp470;void*_tmp472;void*_tmp474;_LL1130: if((
unsigned int)_tmp468 > 3?*((int*)_tmp468)== Cyc_Absyn_PointerType: 0){_LL1135:
_tmp46E=((struct Cyc_Absyn_PointerType_struct*)_tmp468)->f1;_LL1141: _tmp474=(void*)
_tmp46E.elt_typ;goto _LL1139;_LL1139: _tmp472=(void*)_tmp46E.rgn_typ;goto _LL1137;
_LL1137: _tmp470=_tmp46E.bounds;goto _LL1131;}else{goto _LL1132;}_LL1132: goto
_LL1133;_LL1131: Cyc_Tcutil_check_nonzero_bound(loc,_tmp470);{void*_tmp476=Cyc_Tcutil_compress(
_tmp474);struct Cyc_Absyn_Structdecl**_tmp482;struct Cyc_Absyn_Structdecl*_tmp484;
struct Cyc_List_List*_tmp485;struct _tuple1*_tmp487;struct Cyc_Absyn_Uniondecl**
_tmp489;struct Cyc_Absyn_Uniondecl*_tmp48B;struct Cyc_List_List*_tmp48C;struct
_tuple1*_tmp48E;struct Cyc_List_List*_tmp490;struct Cyc_List_List*_tmp492;_LL1144:
if((unsigned int)_tmp476 > 3?*((int*)_tmp476)== Cyc_Absyn_StructType: 0){_LL1160:
_tmp487=((struct Cyc_Absyn_StructType_struct*)_tmp476)->f1;goto _LL1158;_LL1158:
_tmp485=((struct Cyc_Absyn_StructType_struct*)_tmp476)->f2;goto _LL1155;_LL1155:
_tmp482=((struct Cyc_Absyn_StructType_struct*)_tmp476)->f3;if(_tmp482 == 0){goto
_LL1146;}else{_tmp484=*_tmp482;goto _LL1145;}}else{goto _LL1146;}_LL1146: if((
unsigned int)_tmp476 > 3?*((int*)_tmp476)== Cyc_Absyn_UnionType: 0){_LL1167:
_tmp48E=((struct Cyc_Absyn_UnionType_struct*)_tmp476)->f1;goto _LL1165;_LL1165:
_tmp48C=((struct Cyc_Absyn_UnionType_struct*)_tmp476)->f2;goto _LL1162;_LL1162:
_tmp489=((struct Cyc_Absyn_UnionType_struct*)_tmp476)->f3;if(_tmp489 == 0){goto
_LL1148;}else{_tmp48B=*_tmp489;goto _LL1147;}}else{goto _LL1148;}_LL1148: if((
unsigned int)_tmp476 > 3?*((int*)_tmp476)== Cyc_Absyn_AnonStructType: 0){_LL1169:
_tmp490=((struct Cyc_Absyn_AnonStructType_struct*)_tmp476)->f1;goto _LL1149;}else{
goto _LL1150;}_LL1150: if((unsigned int)_tmp476 > 3?*((int*)_tmp476)== Cyc_Absyn_AnonUnionType:
0){_LL1171: _tmp492=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp476)->f1;goto
_LL1151;}else{goto _LL1152;}_LL1152: goto _LL1153;_LL1145: {struct Cyc_Absyn_Structfield*
_tmp494=Cyc_Absyn_lookup_struct_field(_tmp484,f);if(_tmp494 == 0){return({struct
Cyc_Std_String_pa_struct _tmp497;_tmp497.tag=Cyc_Std_String_pa;_tmp497.f1=(struct
_tagged_arr)*f;{struct Cyc_Std_String_pa_struct _tmp496;_tmp496.tag=Cyc_Std_String_pa;
_tmp496.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((struct _tuple1*)
_check_null(_tmp487));{void*_tmp495[2]={& _tmp496,& _tmp497};Cyc_Tcexp_expr_err(te,
loc,_tag_arr("struct %s has no %s field",sizeof(unsigned char),26),_tag_arr(
_tmp495,sizeof(void*),2));}}});}{void*t3;{struct _RegionHandle _tmp498=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp498;_push_region(rgn);{struct Cyc_List_List*_tmp499=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp484->tvs,_tmp485);t3=Cyc_Tcutil_rsubstitute(
rgn,_tmp499,(void*)_tmp494->type);};_pop_region(rgn);}Cyc_Tcenv_check_rgn_accessible(
te,loc,_tmp472);return t3;}}_LL1147: {struct Cyc_Absyn_Structfield*_tmp49A=Cyc_Absyn_lookup_union_field(
_tmp48B,f);if(_tmp49A == 0){return({struct Cyc_Std_String_pa_struct _tmp49D;_tmp49D.tag=
Cyc_Std_String_pa;_tmp49D.f1=(struct _tagged_arr)*f;{struct Cyc_Std_String_pa_struct
_tmp49C;_tmp49C.tag=Cyc_Std_String_pa;_tmp49C.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((
struct _tuple1*)_check_null(_tmp48E));{void*_tmp49B[2]={& _tmp49C,& _tmp49D};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("union %s has no %s field",sizeof(unsigned char),25),_tag_arr(
_tmp49B,sizeof(void*),2));}}});}{struct Cyc_List_List*_tmp49E=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp48B->tvs,_tmp48C);
void*_tmp49F=Cyc_Tcutil_substitute(_tmp49E,(void*)_tmp49A->type);Cyc_Tcenv_check_rgn_accessible(
te,loc,_tmp472);return _tmp49F;}}_LL1149: {struct Cyc_Absyn_Structfield*_tmp4A0=
Cyc_Absyn_lookup_field(_tmp490,f);if(_tmp4A0 == 0){return({struct Cyc_Std_String_pa_struct
_tmp4A2;_tmp4A2.tag=Cyc_Std_String_pa;_tmp4A2.f1=(struct _tagged_arr)*f;{void*
_tmp4A1[1]={& _tmp4A2};Cyc_Tcexp_expr_err(te,loc,_tag_arr("struct has no %s field",
sizeof(unsigned char),23),_tag_arr(_tmp4A1,sizeof(void*),1));}});}return(void*)
_tmp4A0->type;}_LL1151: {struct Cyc_Absyn_Structfield*_tmp4A3=Cyc_Absyn_lookup_field(
_tmp492,f);if(_tmp4A3 == 0){return({struct Cyc_Std_String_pa_struct _tmp4A5;_tmp4A5.tag=
Cyc_Std_String_pa;_tmp4A5.f1=(struct _tagged_arr)*f;{void*_tmp4A4[1]={& _tmp4A5};
Cyc_Tcexp_expr_err(te,loc,_tag_arr("union has no %s field",sizeof(unsigned char),
22),_tag_arr(_tmp4A4,sizeof(void*),1));}});}return(void*)_tmp4A3->type;}_LL1153:
return({struct Cyc_Std_String_pa_struct _tmp4A7;_tmp4A7.tag=Cyc_Std_String_pa;
_tmp4A7.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp4A6[1]={&
_tmp4A7};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting struct pointer, found %s",
sizeof(unsigned char),35),_tag_arr(_tmp4A6,sizeof(void*),1));}});_LL1143:;}
_LL1133: return({struct Cyc_Std_String_pa_struct _tmp4A9;_tmp4A9.tag=Cyc_Std_String_pa;
_tmp4A9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp4A8[1]={&
_tmp4A9};Cyc_Tcexp_expr_err(te,loc,_tag_arr("expecting struct pointer, found %s",
sizeof(unsigned char),35),_tag_arr(_tmp4A8,sizeof(void*),1));}});_LL1129:;}}
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){unsigned int i=Cyc_Evexp_eval_const_uint_exp(
index);struct _handler_cons _tmp4AA;_push_handler(& _tmp4AA);{int _tmp4AC=0;if(
setjmp(_tmp4AA.handler)){_tmp4AC=1;}if(! _tmp4AC){{void*_tmp4AD=(*((struct _tuple8*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)i)).f2;_npop_handler(0);return
_tmp4AD;};_pop_handler();}else{void*_tmp4AB=(void*)_exn_thrown;void*_tmp4AF=
_tmp4AB;_LL1201: if(_tmp4AF == Cyc_List_Nth){goto _LL1202;}else{goto _LL1203;}
_LL1203: goto _LL1204;_LL1202: return({struct Cyc_Std_Int_pa_struct _tmp4B7;_tmp4B7.tag=
Cyc_Std_Int_pa;_tmp4B7.f1=(int)((unsigned int)((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(ts));{struct Cyc_Std_Int_pa_struct _tmp4B6;_tmp4B6.tag=Cyc_Std_Int_pa;
_tmp4B6.f1=(int)i;{void*_tmp4B5[2]={& _tmp4B6,& _tmp4B7};Cyc_Tcexp_expr_err(te,loc,
_tag_arr("index is %d but tuple has only %d fields",sizeof(unsigned char),41),
_tag_arr(_tmp4B5,sizeof(void*),2));}}});_LL1204:(void)_throw(_tmp4AF);_LL1200:;}}}
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){Cyc_Tcexp_tcExp(te,
0,e1);Cyc_Tcexp_tcExp(te,0,e2);{void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v);void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e2->topt))->v);if(! Cyc_Tcutil_coerce_sint_typ(te,e2)){return({struct
Cyc_Std_String_pa_struct _tmp4B9;_tmp4B9.tag=Cyc_Std_String_pa;_tmp4B9.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{void*_tmp4B8[1]={& _tmp4B9};Cyc_Tcexp_expr_err(
te,e2->loc,_tag_arr("expecting int subscript, found %s",sizeof(unsigned char),34),
_tag_arr(_tmp4B8,sizeof(void*),1));}});}{void*_tmp4BA=t1;struct Cyc_Absyn_PtrInfo
_tmp4C2;struct Cyc_Absyn_Conref*_tmp4C4;struct Cyc_Absyn_Tqual _tmp4C6;void*_tmp4C8;
void*_tmp4CA;struct Cyc_List_List*_tmp4CC;_LL1212: if((unsigned int)_tmp4BA > 3?*((
int*)_tmp4BA)== Cyc_Absyn_PointerType: 0){_LL1219: _tmp4C2=((struct Cyc_Absyn_PointerType_struct*)
_tmp4BA)->f1;_LL1227: _tmp4CA=(void*)_tmp4C2.elt_typ;goto _LL1225;_LL1225: _tmp4C8=(
void*)_tmp4C2.rgn_typ;goto _LL1223;_LL1223: _tmp4C6=_tmp4C2.tq;goto _LL1221;_LL1221:
_tmp4C4=_tmp4C2.bounds;goto _LL1213;}else{goto _LL1214;}_LL1214: if((unsigned int)
_tmp4BA > 3?*((int*)_tmp4BA)== Cyc_Absyn_TupleType: 0){_LL1229: _tmp4CC=((struct Cyc_Absyn_TupleType_struct*)
_tmp4BA)->f1;goto _LL1215;}else{goto _LL1216;}_LL1216: goto _LL1217;_LL1213: if(Cyc_Tcutil_is_const_exp(
te,e2)){Cyc_Tcutil_check_bound(loc,Cyc_Evexp_eval_const_uint_exp(e2),_tmp4C4);}
else{if(Cyc_Tcutil_is_bound_one(_tmp4C4)){({void*_tmp4CE[0]={};Cyc_Tcutil_warn(
e1->loc,_tag_arr("subscript applied to pointer to one object",sizeof(
unsigned char),43),_tag_arr(_tmp4CE,sizeof(void*),0));});}Cyc_Tcutil_check_nonzero_bound(
loc,_tmp4C4);}Cyc_Tcenv_check_rgn_accessible(te,loc,_tmp4C8);if(! Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(_tmp4CA),(void*)Cyc_Absyn_MemKind)){({void*_tmp4CF[0]={};Cyc_Tcutil_terr(
e1->loc,_tag_arr("can't subscript an abstract pointer",sizeof(unsigned char),36),
_tag_arr(_tmp4CF,sizeof(void*),0));});}return _tmp4CA;_LL1215: return Cyc_Tcexp_ithTupleType(
te,loc,_tmp4CC,e2);_LL1217: return({struct Cyc_Std_String_pa_struct _tmp4D1;_tmp4D1.tag=
Cyc_Std_String_pa;_tmp4D1.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmp4D0[1]={& _tmp4D1};Cyc_Tcexp_expr_err(te,loc,_tag_arr("subscript applied to %s",
sizeof(unsigned char),24),_tag_arr(_tmp4D0,sizeof(void*),1));}});_LL1211:;}}}
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_List_List*es){int done=0;struct Cyc_List_List*fields=0;
if(topt != 0){void*_tmp4D2=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmp4D8;
_LL1236: if((unsigned int)_tmp4D2 > 3?*((int*)_tmp4D2)== Cyc_Absyn_TupleType: 0){
_LL1241: _tmp4D8=((struct Cyc_Absyn_TupleType_struct*)_tmp4D2)->f1;goto _LL1237;}
else{goto _LL1238;}_LL1238: goto _LL1239;_LL1237: if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp4D8)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)){
goto _LL1235;}for(0;es != 0;(es=es->tl,_tmp4D8=_tmp4D8->tl)){void*_tmp4DA=(*((
struct _tuple8*)((struct Cyc_List_List*)_check_null(_tmp4D8))->hd)).f2;Cyc_Tcexp_tcExpInitializer(
te,(void**)& _tmp4DA,(struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*
_tmp4DB=_cycalloc(sizeof(struct Cyc_List_List));_tmp4DB->hd=({struct _tuple8*
_tmp4DC=_cycalloc(sizeof(struct _tuple8));_tmp4DC->f1=(*((struct _tuple8*)_tmp4D8->hd)).f1;
_tmp4DC->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp4DC;});_tmp4DB->tl=fields;_tmp4DB;});}done=1;goto _LL1235;_LL1239: goto _LL1235;
_LL1235:;}if(! done){for(0;es != 0;es=es->tl){Cyc_Tcexp_tcExpInitializer(te,0,(
struct Cyc_Absyn_Exp*)es->hd);fields=({struct Cyc_List_List*_tmp4DD=_cycalloc(
sizeof(struct Cyc_List_List));_tmp4DD->hd=({struct _tuple8*_tmp4DE=_cycalloc(
sizeof(struct _tuple8));_tmp4DE->f1=Cyc_Absyn_empty_tqual();_tmp4DE->f2=(void*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt))->v;
_tmp4DE;});_tmp4DD->tl=fields;_tmp4DD;});}}return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp4DF=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_struct));_tmp4DF[0]=({struct
Cyc_Absyn_TupleType_struct _tmp4E0;_tmp4E0.tag=Cyc_Absyn_TupleType;_tmp4E0.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp4E0;});
_tmp4DF;});}static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct _tuple2*t,struct Cyc_List_List*des){return({void*_tmp4E1[0]={};
Cyc_Tcexp_expr_err(te,loc,_tag_arr("tcCompoundLit",sizeof(unsigned char),14),
_tag_arr(_tmp4E1,sizeof(void*),0));});}static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**elt_topt,struct Cyc_List_List*des){struct
Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct
_tuple7*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct
_tuple7*))Cyc_Core_snd,des);void*res=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp4F8=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp4F8->v=Cyc_Tcenv_lookup_type_vars(te);_tmp4F8;}));struct Cyc_Absyn_Const_e_struct*
_tmp4E2=({struct Cyc_Absyn_Const_e_struct*_tmp4F4=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_struct));
_tmp4F4[0]=({struct Cyc_Absyn_Const_e_struct _tmp4F5;_tmp4F5.tag=Cyc_Absyn_Const_e;
_tmp4F5.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*_tmp4F6=_cycalloc(
sizeof(struct Cyc_Absyn_Int_c_struct));_tmp4F6[0]=({struct Cyc_Absyn_Int_c_struct
_tmp4F7;_tmp4F7.tag=Cyc_Absyn_Int_c;_tmp4F7.f1=(void*)((void*)Cyc_Absyn_Unsigned);
_tmp4F7.f2=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);_tmp4F7;});
_tmp4F6;}));_tmp4F5;});_tmp4F4;});struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((
void*)_tmp4E2,loc);sz_exp->topt=({struct Cyc_Core_Opt*_tmp4E3=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmp4E3->v=(void*)Cyc_Absyn_uint_t;_tmp4E3;});{void*res_t2=(
void*)({struct Cyc_Absyn_ArrayType_struct*_tmp4F2=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));
_tmp4F2[0]=({struct Cyc_Absyn_ArrayType_struct _tmp4F3;_tmp4F3.tag=Cyc_Absyn_ArrayType;
_tmp4F3.f1=(void*)res;_tmp4F3.f2=Cyc_Absyn_empty_tqual();_tmp4F3.f3=(struct Cyc_Absyn_Exp*)
sz_exp;_tmp4F3;});_tmp4F2;});{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}if(! Cyc_Tcutil_coerce_list(
te,res,es)){({struct Cyc_Std_String_pa_struct _tmp4E5;_tmp4E5.tag=Cyc_Std_String_pa;
_tmp4E5.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(res);{void*_tmp4E4[1]={&
_tmp4E5};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(es))->hd)->loc,_tag_arr("elements of array do not all have the same type (%s)",
sizeof(unsigned char),53),_tag_arr(_tmp4E4,sizeof(void*),1));}});}{int offset=0;
for(0;des != 0;(offset ++,des=des->tl)){struct Cyc_List_List*ds=(*((struct _tuple7*)
des->hd)).f1;if(ds != 0){void*_tmp4E6=(void*)ds->hd;struct Cyc_Absyn_Exp*_tmp4EC;
_LL1256: if(*((int*)_tmp4E6)== Cyc_Absyn_FieldName){goto _LL1257;}else{goto _LL1258;}
_LL1258: if(*((int*)_tmp4E6)== Cyc_Absyn_ArrayElement){_LL1261: _tmp4EC=((struct
Cyc_Absyn_ArrayElement_struct*)_tmp4E6)->f1;goto _LL1259;}else{goto _LL1255;}
_LL1257:({void*_tmp4EE[0]={};Cyc_Tcutil_terr(loc,_tag_arr("only array index designators are supported",
sizeof(unsigned char),43),_tag_arr(_tmp4EE,sizeof(void*),0));});goto _LL1255;
_LL1259: Cyc_Tcexp_tcExpInitializer(te,0,_tmp4EC);{unsigned int i=Cyc_Evexp_eval_const_uint_exp(
_tmp4EC);if(i != offset){({struct Cyc_Std_Int_pa_struct _tmp4F1;_tmp4F1.tag=Cyc_Std_Int_pa;
_tmp4F1.f1=(int)i;{struct Cyc_Std_Int_pa_struct _tmp4F0;_tmp4F0.tag=Cyc_Std_Int_pa;
_tmp4F0.f1=(int)((unsigned int)offset);{void*_tmp4EF[2]={& _tmp4F0,& _tmp4F1};Cyc_Tcutil_terr(
_tmp4EC->loc,_tag_arr("expecting index designator %d but found %d",sizeof(
unsigned char),43),_tag_arr(_tmp4EF,sizeof(void*),2));}}});}goto _LL1255;}_LL1255:;}}}
return res_t2;}}static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*
bound,struct Cyc_Absyn_Exp*body){Cyc_Tcexp_tcExp(te,0,bound);if(! Cyc_Tcutil_coerce_uint_typ(
te,bound)){({struct Cyc_Std_String_pa_struct _tmp4FA;_tmp4FA.tag=Cyc_Std_String_pa;
_tmp4FA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(bound->topt))->v);{void*_tmp4F9[1]={& _tmp4FA};Cyc_Tcutil_terr(bound->loc,
_tag_arr("expecting unsigned int, found %s",sizeof(unsigned char),33),_tag_arr(
_tmp4F9,sizeof(void*),1));}});}if(!(vd->tq).q_const){({void*_tmp4FB[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("comprehension index variable is not declared const!",
sizeof(unsigned char),52),_tag_arr(_tmp4FB,sizeof(void*),0));});}if(te->le != 0){
te=Cyc_Tcenv_new_block(loc,te);te=Cyc_Tcenv_add_local_var(loc,te,vd);}{struct Cyc_Absyn_PtrInfo
pinfo;void**_tmp4FC=0;struct Cyc_Absyn_Tqual*_tmp4FD=0;if(topt != 0){void*_tmp4FE=
Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp506;struct Cyc_Absyn_Exp*
_tmp508;struct Cyc_Absyn_Tqual _tmp50A;struct Cyc_Absyn_Tqual*_tmp50C;void*_tmp50D;
void**_tmp50F;_LL1280: if((unsigned int)_tmp4FE > 3?*((int*)_tmp4FE)== Cyc_Absyn_PointerType:
0){_LL1287: _tmp506=((struct Cyc_Absyn_PointerType_struct*)_tmp4FE)->f1;goto
_LL1281;}else{goto _LL1282;}_LL1282: if((unsigned int)_tmp4FE > 3?*((int*)_tmp4FE)
== Cyc_Absyn_ArrayType: 0){_LL1294: _tmp50D=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp4FE)->f1;_tmp50F=(void**)&((void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp4FE)->f1);goto _LL1291;_LL1291: _tmp50A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4FE)->f2;_tmp50C=(struct Cyc_Absyn_Tqual*)&((struct Cyc_Absyn_ArrayType_struct*)
_tmp4FE)->f2;goto _LL1289;_LL1289: _tmp508=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4FE)->f3;goto _LL1283;}else{goto _LL1284;}_LL1284: goto _LL1285;_LL1281: pinfo=
_tmp506;_tmp4FC=(void**)((void**)((void*)& pinfo.elt_typ));_tmp4FD=(struct Cyc_Absyn_Tqual*)&
pinfo.tq;goto _LL1279;_LL1283: _tmp4FC=(void**)_tmp50F;_tmp4FD=(struct Cyc_Absyn_Tqual*)
_tmp50C;goto _LL1279;_LL1285: goto _LL1279;_LL1279:;}{void*t=Cyc_Tcexp_tcExp(te,
_tmp4FC,body);if(te->le == 0){if(! Cyc_Tcutil_is_const_exp(te,bound)){({void*
_tmp510[0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("bound is not constant",
sizeof(unsigned char),22),_tag_arr(_tmp510,sizeof(void*),0));});}if(! Cyc_Tcutil_is_const_exp(
te,body)){({void*_tmp511[0]={};Cyc_Tcutil_terr(bound->loc,_tag_arr("body is not constant",
sizeof(unsigned char),21),_tag_arr(_tmp511,sizeof(void*),0));});}}return(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp512=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));
_tmp512[0]=({struct Cyc_Absyn_ArrayType_struct _tmp513;_tmp513.tag=Cyc_Absyn_ArrayType;
_tmp513.f1=(void*)t;_tmp513.f2=_tmp4FD == 0? Cyc_Absyn_empty_tqual():*_tmp4FD;
_tmp513.f3=(struct Cyc_Absyn_Exp*)bound;_tmp513;});_tmp512;});}}}struct _tuple9{
struct Cyc_Absyn_Structfield*f1;struct Cyc_Absyn_Exp*f2;};static void*Cyc_Tcexp_tcStruct(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct _tuple1**
tn,struct Cyc_Core_Opt**otyps,struct Cyc_List_List*args,struct Cyc_Absyn_Structdecl**
sd_opt){struct Cyc_Absyn_Structdecl*sd;if(*sd_opt != 0){sd=(struct Cyc_Absyn_Structdecl*)
_check_null(*sd_opt);}else{{struct _handler_cons _tmp514;_push_handler(& _tmp514);{
int _tmp516=0;if(setjmp(_tmp514.handler)){_tmp516=1;}if(! _tmp516){sd=*Cyc_Tcenv_lookup_structdecl(
te,loc,*tn);;_pop_handler();}else{void*_tmp515=(void*)_exn_thrown;void*_tmp518=
_tmp515;_LL1306: if(_tmp518 == Cyc_Dict_Absent){goto _LL1307;}else{goto _LL1308;}
_LL1308: goto _LL1309;_LL1307:({struct Cyc_Std_String_pa_struct _tmp51F;_tmp51F.tag=
Cyc_Std_String_pa;_tmp51F.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(*tn);{
void*_tmp51E[1]={& _tmp51F};Cyc_Tcutil_terr(loc,_tag_arr("unbound struct name %s",
sizeof(unsigned char),23),_tag_arr(_tmp51E,sizeof(void*),1));}});return topt != 0?*
topt:(void*)Cyc_Absyn_VoidType;_LL1309:(void)_throw(_tmp518);_LL1305:;}}}*sd_opt=(
struct Cyc_Absyn_Structdecl*)sd;if(sd->name != 0){*tn=(struct _tuple1*)((struct Cyc_Core_Opt*)
_check_null(sd->name))->v;}}{struct _RegionHandle _tmp520=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp520;_push_region(rgn);{struct _tuple4 _tmp521=({
struct _tuple4 _tmp539;_tmp539.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp539.f2=rgn;
_tmp539;});struct Cyc_List_List*_tmp522=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp521,sd->tvs);struct Cyc_List_List*_tmp523=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp522);struct Cyc_Absyn_StructType_struct*_tmp524=({struct Cyc_Absyn_StructType_struct*
_tmp536=_cycalloc(sizeof(struct Cyc_Absyn_StructType_struct));_tmp536[0]=({struct
Cyc_Absyn_StructType_struct _tmp537;_tmp537.tag=Cyc_Absyn_StructType;_tmp537.f1=(
struct _tuple1*)*tn;_tmp537.f2=_tmp523;_tmp537.f3=({struct Cyc_Absyn_Structdecl**
_tmp538=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl*));_tmp538[0]=sd;_tmp538;});
_tmp537;});_tmp536;});if(topt != 0){Cyc_Tcutil_unify((void*)_tmp524,*topt);}*
otyps=({struct Cyc_Core_Opt*_tmp525=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp525->v=
_tmp523;_tmp525;});if(sd->fields == 0){({void*_tmp526[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("can't build abstract struct",sizeof(unsigned char),28),_tag_arr(
_tmp526,sizeof(void*),0));});{void*_tmp527=(void*)_tmp524;_npop_handler(0);
return _tmp527;}}{struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(rgn,loc,args,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sd->fields))->v);for(0;
fields != 0;fields=fields->tl){struct _tuple9 _tmp52A;struct Cyc_Absyn_Exp*_tmp52B;
struct Cyc_Absyn_Structfield*_tmp52D;struct _tuple9*_tmp528=(struct _tuple9*)fields->hd;
_tmp52A=*_tmp528;_LL1326: _tmp52D=_tmp52A.f1;goto _LL1324;_LL1324: _tmp52B=_tmp52A.f2;
goto _LL1321;_LL1321: {void*_tmp52F=Cyc_Tcutil_rsubstitute(rgn,_tmp522,(void*)
_tmp52D->type);Cyc_Tcexp_tcExpInitializer(te,(void**)& _tmp52F,_tmp52B);if(! Cyc_Tcutil_coerce_arg(
te,_tmp52B,_tmp52F)){({struct Cyc_Std_String_pa_struct _tmp534;_tmp534.tag=Cyc_Std_String_pa;
_tmp534.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp52B->topt))->v);{struct Cyc_Std_String_pa_struct _tmp533;_tmp533.tag=
Cyc_Std_String_pa;_tmp533.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp52F);{
struct Cyc_Std_String_pa_struct _tmp532;_tmp532.tag=Cyc_Std_String_pa;_tmp532.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(*tn);{struct Cyc_Std_String_pa_struct
_tmp531;_tmp531.tag=Cyc_Std_String_pa;_tmp531.f1=(struct _tagged_arr)*_tmp52D->name;{
void*_tmp530[4]={& _tmp531,& _tmp532,& _tmp533,& _tmp534};Cyc_Tcutil_terr(_tmp52B->loc,
_tag_arr("field %s of struct %s expects type %s != %s",sizeof(unsigned char),44),
_tag_arr(_tmp530,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();}}}{void*
_tmp535=(void*)_tmp524;_npop_handler(0);return _tmp535;}}};_pop_region(rgn);}}
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*ts,struct Cyc_List_List*args){{struct _RegionHandle _tmp53A=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp53A;_push_region(rgn);{void*_tmp53B=Cyc_Tcutil_compress(
ts);struct Cyc_List_List*_tmp541;_LL1341: if((unsigned int)_tmp53B > 3?*((int*)
_tmp53B)== Cyc_Absyn_AnonStructType: 0){_LL1346: _tmp541=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp53B)->f1;goto _LL1342;}else{goto _LL1343;}_LL1343: goto _LL1344;_LL1342: {struct
Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,loc,args,_tmp541);for(0;fields != 0;fields=fields->tl){struct _tuple9 _tmp545;
struct Cyc_Absyn_Exp*_tmp546;struct Cyc_Absyn_Structfield*_tmp548;struct _tuple9*
_tmp543=(struct _tuple9*)fields->hd;_tmp545=*_tmp543;_LL1353: _tmp548=_tmp545.f1;
goto _LL1351;_LL1351: _tmp546=_tmp545.f2;goto _LL1348;_LL1348: Cyc_Tcexp_tcExpInitializer(
te,(void**)((void**)((void*)& _tmp548->type)),_tmp546);if(! Cyc_Tcutil_coerce_arg(
te,_tmp546,(void*)_tmp548->type)){({struct Cyc_Std_String_pa_struct _tmp54D;
_tmp54D.tag=Cyc_Std_String_pa;_tmp54D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp546->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp54C;_tmp54C.tag=Cyc_Std_String_pa;_tmp54C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)_tmp548->type);{struct Cyc_Std_String_pa_struct _tmp54B;_tmp54B.tag=Cyc_Std_String_pa;
_tmp54B.f1=(struct _tagged_arr)*_tmp548->name;{void*_tmp54A[3]={& _tmp54B,& _tmp54C,&
_tmp54D};Cyc_Tcutil_terr(_tmp546->loc,_tag_arr("field %s of struct expects type %s != %s",
sizeof(unsigned char),41),_tag_arr(_tmp54A,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
goto _LL1340;}_LL1344:({void*_tmp54E[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("tcAnonStruct:  not an AnonStructType",
sizeof(unsigned char),37),_tag_arr(_tmp54E,sizeof(void*),0));});goto _LL1340;
_LL1340:;};_pop_region(rgn);}return ts;}static void*Cyc_Tcexp_tcTunion(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_Core_Opt**
all_ref,struct Cyc_Core_Opt**exist_ref,struct Cyc_List_List*es,struct Cyc_Absyn_Tuniondecl*
tud,struct Cyc_Absyn_Tunionfield*tuf){struct _RegionHandle _tmp54F=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp54F;_push_region(rgn);{struct _tuple4 _tmp550=({
struct _tuple4 _tmp57E;_tmp57E.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp57E.f2=rgn;
_tmp57E;});struct Cyc_List_List*_tmp551=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp550,tud->tvs);struct Cyc_List_List*_tmp552=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp550,tuf->tvs);struct Cyc_List_List*_tmp553=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp551);struct Cyc_List_List*_tmp554=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp552);struct Cyc_List_List*_tmp555=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
rgn,_tmp551,_tmp552);*all_ref=({struct Cyc_Core_Opt*_tmp556=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmp556->v=_tmp553;_tmp556;});*exist_ref=({struct Cyc_Core_Opt*
_tmp557=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp557->v=_tmp554;_tmp557;});{
void*res=(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp579=_cycalloc(
sizeof(struct Cyc_Absyn_TunionFieldType_struct));_tmp579[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp57A;_tmp57A.tag=Cyc_Absyn_TunionFieldType;_tmp57A.f1=({struct Cyc_Absyn_TunionFieldInfo
_tmp57B;_tmp57B.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp57C=_cycalloc(sizeof(struct Cyc_Absyn_KnownTunionfield_struct));_tmp57C[0]=({
struct Cyc_Absyn_KnownTunionfield_struct _tmp57D;_tmp57D.tag=Cyc_Absyn_KnownTunionfield;
_tmp57D.f1=tud;_tmp57D.f2=tuf;_tmp57D;});_tmp57C;}));_tmp57B.targs=_tmp553;
_tmp57B;});_tmp57A;});_tmp579;});if(topt != 0){void*_tmp558=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_TunionInfo _tmp560;void*_tmp562;struct Cyc_List_List*_tmp564;
_LL1370: if((unsigned int)_tmp558 > 3?*((int*)_tmp558)== Cyc_Absyn_TunionFieldType:
0){goto _LL1371;}else{goto _LL1372;}_LL1372: if((unsigned int)_tmp558 > 3?*((int*)
_tmp558)== Cyc_Absyn_TunionType: 0){_LL1377: _tmp560=((struct Cyc_Absyn_TunionType_struct*)
_tmp558)->f1;_LL1381: _tmp564=_tmp560.targs;goto _LL1379;_LL1379: _tmp562=(void*)
_tmp560.rgn;goto _LL1373;}else{goto _LL1374;}_LL1374: goto _LL1375;_LL1371: Cyc_Tcutil_unify(*
topt,res);goto _LL1369;_LL1373:{struct Cyc_List_List*a=_tmp553;for(0;a != 0? _tmp564
!= 0: 0;(a=a->tl,_tmp564=_tmp564->tl)){Cyc_Tcutil_unify((void*)a->hd,(void*)
_tmp564->hd);}}if(tuf->typs == 0? es == 0: 0){e->topt=({struct Cyc_Core_Opt*_tmp566=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp566->v=(void*)res;_tmp566;});res=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp567=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));
_tmp567[0]=({struct Cyc_Absyn_TunionType_struct _tmp568;_tmp568.tag=Cyc_Absyn_TunionType;
_tmp568.f1=({struct Cyc_Absyn_TunionInfo _tmp569;_tmp569.tunion_info=(void*)((void*)({
struct Cyc_Absyn_KnownTunion_struct*_tmp56A=_cycalloc(sizeof(struct Cyc_Absyn_KnownTunion_struct));
_tmp56A[0]=({struct Cyc_Absyn_KnownTunion_struct _tmp56B;_tmp56B.tag=Cyc_Absyn_KnownTunion;
_tmp56B.f1=({struct Cyc_Absyn_Tuniondecl**_tmp56C=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl*));
_tmp56C[0]=tud;_tmp56C;});_tmp56B;});_tmp56A;}));_tmp569.targs=_tmp553;_tmp569.rgn=(
void*)_tmp562;_tmp569;});_tmp568;});_tmp567;});Cyc_Tcutil_unchecked_cast(te,e,
res);{void*_tmp56D=res;_npop_handler(0);return _tmp56D;}}goto _LL1369;_LL1375: goto
_LL1369;_LL1369:;}{struct Cyc_List_List*ts=tuf->typs;for(0;es != 0? ts != 0: 0;(es=es->tl,
ts=ts->tl)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp555,(*((struct _tuple8*)ts->hd)).f2);Cyc_Tcexp_tcExpInitializer(te,(void**)&
t,e);if(! Cyc_Tcutil_coerce_arg(te,e,t)){({struct Cyc_Std_String_pa_struct _tmp571;
_tmp571.tag=Cyc_Std_String_pa;_tmp571.f1=(struct _tagged_arr)(e->topt == 0?
_tag_arr("?",sizeof(unsigned char),2): Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v));{struct Cyc_Std_String_pa_struct _tmp570;_tmp570.tag=
Cyc_Std_String_pa;_tmp570.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
struct Cyc_Std_String_pa_struct _tmp56F;_tmp56F.tag=Cyc_Std_String_pa;_tmp56F.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp56E[3]={& _tmp56F,&
_tmp570,& _tmp571};Cyc_Tcutil_terr(e->loc,_tag_arr("tunion constructor %s expects argument of type %s but this argument has type %s",
sizeof(unsigned char),80),_tag_arr(_tmp56E,sizeof(void*),3));}}}});Cyc_Tcutil_explain_failure();}}
if(es != 0){void*_tmp574=({struct Cyc_Std_String_pa_struct _tmp573;_tmp573.tag=Cyc_Std_String_pa;
_tmp573.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp572[1]={&
_tmp573};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,_tag_arr("too many arguments for tunion constructor %s",
sizeof(unsigned char),45),_tag_arr(_tmp572,sizeof(void*),1));}});_npop_handler(0);
return _tmp574;}if(ts != 0){void*_tmp577=({struct Cyc_Std_String_pa_struct _tmp576;
_tmp576.tag=Cyc_Std_String_pa;_tmp576.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
tuf->name);{void*_tmp575[1]={& _tmp576};Cyc_Tcexp_expr_err(te,loc,_tag_arr("too few arguments for tunion constructor %s",
sizeof(unsigned char),44),_tag_arr(_tmp575,sizeof(void*),1));}});_npop_handler(0);
return _tmp577;}{void*_tmp578=res;_npop_handler(0);return _tmp578;}}}};_pop_region(
rgn);}static int Cyc_Tcexp_zeroable_type(void*t){void*_tmp57F=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp5B3;struct Cyc_Absyn_Conref*_tmp5B5;void*_tmp5B7;
struct Cyc_List_List*_tmp5B9;struct Cyc_Absyn_Structdecl**_tmp5BB;struct Cyc_List_List*
_tmp5BD;struct Cyc_List_List*_tmp5BF;_LL1409: if(_tmp57F == (void*)Cyc_Absyn_VoidType){
goto _LL1410;}else{goto _LL1411;}_LL1411: if((unsigned int)_tmp57F > 3?*((int*)
_tmp57F)== Cyc_Absyn_Evar: 0){goto _LL1412;}else{goto _LL1413;}_LL1413: if((
unsigned int)_tmp57F > 3?*((int*)_tmp57F)== Cyc_Absyn_VarType: 0){goto _LL1414;}
else{goto _LL1415;}_LL1415: if((unsigned int)_tmp57F > 3?*((int*)_tmp57F)== Cyc_Absyn_TunionType:
0){goto _LL1416;}else{goto _LL1417;}_LL1417: if((unsigned int)_tmp57F > 3?*((int*)
_tmp57F)== Cyc_Absyn_TunionFieldType: 0){goto _LL1418;}else{goto _LL1419;}_LL1419:
if((unsigned int)_tmp57F > 3?*((int*)_tmp57F)== Cyc_Absyn_PointerType: 0){_LL1460:
_tmp5B3=((struct Cyc_Absyn_PointerType_struct*)_tmp57F)->f1;_LL1462: _tmp5B5=
_tmp5B3.nullable;goto _LL1420;}else{goto _LL1421;}_LL1421: if((unsigned int)_tmp57F
> 3?*((int*)_tmp57F)== Cyc_Absyn_IntType: 0){goto _LL1422;}else{goto _LL1423;}
_LL1423: if(_tmp57F == (void*)Cyc_Absyn_FloatType){goto _LL1424;}else{goto _LL1425;}
_LL1425: if((unsigned int)_tmp57F > 3?*((int*)_tmp57F)== Cyc_Absyn_DoubleType: 0){
goto _LL1426;}else{goto _LL1427;}_LL1427: if((unsigned int)_tmp57F > 3?*((int*)
_tmp57F)== Cyc_Absyn_ArrayType: 0){_LL1464: _tmp5B7=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp57F)->f1;goto _LL1428;}else{goto _LL1429;}_LL1429: if((unsigned int)_tmp57F > 3?*((
int*)_tmp57F)== Cyc_Absyn_FnType: 0){goto _LL1430;}else{goto _LL1431;}_LL1431: if((
unsigned int)_tmp57F > 3?*((int*)_tmp57F)== Cyc_Absyn_TupleType: 0){_LL1466:
_tmp5B9=((struct Cyc_Absyn_TupleType_struct*)_tmp57F)->f1;goto _LL1432;}else{goto
_LL1433;}_LL1433: if((unsigned int)_tmp57F > 3?*((int*)_tmp57F)== Cyc_Absyn_StructType:
0){_LL1470: _tmp5BD=((struct Cyc_Absyn_StructType_struct*)_tmp57F)->f2;goto _LL1468;
_LL1468: _tmp5BB=((struct Cyc_Absyn_StructType_struct*)_tmp57F)->f3;goto _LL1434;}
else{goto _LL1435;}_LL1435: if((unsigned int)_tmp57F > 3?*((int*)_tmp57F)== Cyc_Absyn_UnionType:
0){goto _LL1436;}else{goto _LL1437;}_LL1437: if((unsigned int)_tmp57F > 3?*((int*)
_tmp57F)== Cyc_Absyn_EnumType: 0){goto _LL1438;}else{goto _LL1439;}_LL1439: if((
unsigned int)_tmp57F > 3?*((int*)_tmp57F)== Cyc_Absyn_SizeofType: 0){goto _LL1440;}
else{goto _LL1441;}_LL1441: if((unsigned int)_tmp57F > 3?*((int*)_tmp57F)== Cyc_Absyn_AnonStructType:
0){_LL1472: _tmp5BF=((struct Cyc_Absyn_AnonStructType_struct*)_tmp57F)->f1;goto
_LL1442;}else{goto _LL1443;}_LL1443: if((unsigned int)_tmp57F > 3?*((int*)_tmp57F)
== Cyc_Absyn_AnonUnionType: 0){goto _LL1444;}else{goto _LL1445;}_LL1445: if((
unsigned int)_tmp57F > 3?*((int*)_tmp57F)== Cyc_Absyn_AnonEnumType: 0){goto _LL1446;}
else{goto _LL1447;}_LL1447: if((unsigned int)_tmp57F > 3?*((int*)_tmp57F)== Cyc_Absyn_RgnHandleType:
0){goto _LL1448;}else{goto _LL1449;}_LL1449: if((unsigned int)_tmp57F > 3?*((int*)
_tmp57F)== Cyc_Absyn_TypedefType: 0){goto _LL1450;}else{goto _LL1451;}_LL1451: if(
_tmp57F == (void*)Cyc_Absyn_HeapRgn){goto _LL1452;}else{goto _LL1453;}_LL1453: if((
unsigned int)_tmp57F > 3?*((int*)_tmp57F)== Cyc_Absyn_AccessEff: 0){goto _LL1454;}
else{goto _LL1455;}_LL1455: if((unsigned int)_tmp57F > 3?*((int*)_tmp57F)== Cyc_Absyn_JoinEff:
0){goto _LL1456;}else{goto _LL1457;}_LL1457: if((unsigned int)_tmp57F > 3?*((int*)
_tmp57F)== Cyc_Absyn_RgnsEff: 0){goto _LL1458;}else{goto _LL1408;}_LL1410: return 1;
_LL1412: goto _LL1414;_LL1414: goto _LL1416;_LL1416: goto _LL1418;_LL1418: return 0;
_LL1420: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,
_tmp5B5);_LL1422: goto _LL1424;_LL1424: goto _LL1426;_LL1426: return 1;_LL1428: return
Cyc_Tcexp_zeroable_type(_tmp5B7);_LL1430: return 0;_LL1432: for(0;(unsigned int)
_tmp5B9;_tmp5B9=_tmp5B9->tl){if(! Cyc_Tcexp_zeroable_type((*((struct _tuple8*)
_tmp5B9->hd)).f2)){return 0;}}return 1;_LL1434: if(_tmp5BB == 0){return 0;}{struct Cyc_Absyn_Structdecl*
_tmp5C1=*_tmp5BB;if(_tmp5C1->fields == 0){return 0;}{struct _RegionHandle _tmp5C2=
_new_region("r");struct _RegionHandle*r=& _tmp5C2;_push_region(r);{struct Cyc_List_List*
_tmp5C3=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp5C1->tvs,
_tmp5BD);{struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp5C1->fields))->v;for(0;fs != 0;fs=fs->tl){if(! Cyc_Tcexp_zeroable_type(
Cyc_Tcutil_rsubstitute(r,_tmp5C3,(void*)((struct Cyc_Absyn_Structfield*)fs->hd)->type))){
int _tmp5C4=0;_npop_handler(0);return _tmp5C4;}}}{int _tmp5C5=1;_npop_handler(0);
return _tmp5C5;}};_pop_region(r);}}_LL1436: return 1;_LL1438: return 1;_LL1440: return
1;_LL1442: for(0;_tmp5BF != 0;_tmp5BF=_tmp5BF->tl){if(! Cyc_Tcexp_zeroable_type((
void*)((struct Cyc_Absyn_Structfield*)_tmp5BF->hd)->type)){return 0;}}return 1;
_LL1444: return 1;_LL1446: return 1;_LL1448: return 0;_LL1450: goto _LL1452;_LL1452: goto
_LL1454;_LL1454: goto _LL1456;_LL1456: goto _LL1458;_LL1458: return({struct Cyc_Std_String_pa_struct
_tmp5C7;_tmp5C7.tag=Cyc_Std_String_pa;_tmp5C7.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{void*_tmp5C6[1]={& _tmp5C7};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("bad type `%s' in zeroable type",sizeof(unsigned char),
31),_tag_arr(_tmp5C6,sizeof(void*),1));}});_LL1408:;}static void Cyc_Tcexp_check_malloc_type(
int allow_zero,struct Cyc_Position_Segment*loc,void**topt,void*t){if(Cyc_Tcutil_bits_only(
t)? 1:(allow_zero? Cyc_Tcexp_zeroable_type(t): 0)){return;}if(topt != 0){void*
_tmp5C8=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp5CE;void*_tmp5D0;
_LL1482: if((unsigned int)_tmp5C8 > 3?*((int*)_tmp5C8)== Cyc_Absyn_PointerType: 0){
_LL1487: _tmp5CE=((struct Cyc_Absyn_PointerType_struct*)_tmp5C8)->f1;_LL1489:
_tmp5D0=(void*)_tmp5CE.elt_typ;goto _LL1483;}else{goto _LL1484;}_LL1484: goto
_LL1485;_LL1483: Cyc_Tcutil_unify(_tmp5D0,t);if(Cyc_Tcutil_bits_only(t)? 1:(
allow_zero? Cyc_Tcexp_zeroable_type(t): 0)){return;}goto _LL1481;_LL1485: goto
_LL1481;_LL1481:;}({struct Cyc_Std_String_pa_struct _tmp5D4;_tmp5D4.tag=Cyc_Std_String_pa;
_tmp5D4.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp5D3;_tmp5D3.tag=Cyc_Std_String_pa;_tmp5D3.f1=(struct _tagged_arr)(allow_zero?
_tag_arr("calloc",sizeof(unsigned char),7): _tag_arr("malloc",sizeof(
unsigned char),7));{void*_tmp5D2[2]={& _tmp5D3,& _tmp5D4};Cyc_Tcutil_terr(loc,
_tag_arr("%s cannot be used with type %s\n\t(type needs initialization)",sizeof(
unsigned char),60),_tag_arr(_tmp5D2,sizeof(void*),2));}}});}static void*Cyc_Tcexp_tcMalloc(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Exp*
ropt,void***t,struct Cyc_Absyn_Exp**e,int is_calloc,int*is_fat){void*rgn=(void*)
Cyc_Absyn_HeapRgn;if(ropt != 0){void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp5DF=_cycalloc(sizeof(struct Cyc_Absyn_RgnHandleType_struct));_tmp5DF[0]=({
struct Cyc_Absyn_RgnHandleType_struct _tmp5E0;_tmp5E0.tag=Cyc_Absyn_RgnHandleType;
_tmp5E0.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp5E1=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp5E1->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp5E1;}));_tmp5E0;});_tmp5DF;});void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)_check_null(ropt));void*_tmp5D5=Cyc_Tcutil_compress(
handle_type);void*_tmp5DB;_LL1495: if((unsigned int)_tmp5D5 > 3?*((int*)_tmp5D5)== 
Cyc_Absyn_RgnHandleType: 0){_LL1500: _tmp5DB=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp5D5)->f1;goto _LL1496;}else{goto _LL1497;}_LL1497: goto _LL1498;_LL1496: rgn=
_tmp5DB;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL1494;_LL1498:({struct
Cyc_Std_String_pa_struct _tmp5DE;_tmp5DE.tag=Cyc_Std_String_pa;_tmp5DE.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(handle_type);{void*_tmp5DD[1]={& _tmp5DE};Cyc_Tcutil_terr(
ropt->loc,_tag_arr("expecting region_t type but found %s",sizeof(unsigned char),
37),_tag_arr(_tmp5DD,sizeof(void*),1));}});goto _LL1494;_LL1494:;}Cyc_Tcexp_tcExp(
te,(void**)& Cyc_Absyn_uint_t,*e);{void*elt_type;struct Cyc_Absyn_Exp*num_elts;int
one_elt;if(is_calloc){if(*t == 0){({void*_tmp5E2[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("calloc with empty type",
sizeof(unsigned char),23),_tag_arr(_tmp5E2,sizeof(void*),0));});}elt_type=*((
void**)_check_null(*t));Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(
te),(void*)Cyc_Absyn_MemKind,1,elt_type);Cyc_Tcexp_check_malloc_type(1,loc,topt,
elt_type);num_elts=*e;one_elt=0;}else{void*_tmp5E3=(void*)(*e)->r;void*_tmp5EB;
struct Cyc_List_List*_tmp5ED;struct Cyc_List_List _tmp5EF;struct Cyc_List_List*
_tmp5F0;struct Cyc_List_List _tmp5F2;struct Cyc_List_List*_tmp5F3;struct Cyc_Absyn_Exp*
_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F7;void*_tmp5F9;_LL1509: if(*((int*)_tmp5E3)== 
Cyc_Absyn_Sizeoftyp_e){_LL1516: _tmp5EB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp5E3)->f1;goto _LL1510;}else{goto _LL1511;}_LL1511: if(*((int*)_tmp5E3)== Cyc_Absyn_Primop_e){
_LL1530: _tmp5F9=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp5E3)->f1;if(
_tmp5F9 == (void*)Cyc_Absyn_Times){goto _LL1518;}else{goto _LL1513;}_LL1518: _tmp5ED=((
struct Cyc_Absyn_Primop_e_struct*)_tmp5E3)->f2;if(_tmp5ED == 0){goto _LL1513;}else{
_tmp5EF=*_tmp5ED;_LL1528: _tmp5F7=(struct Cyc_Absyn_Exp*)_tmp5EF.hd;goto _LL1521;
_LL1521: _tmp5F0=_tmp5EF.tl;if(_tmp5F0 == 0){goto _LL1513;}else{_tmp5F2=*_tmp5F0;
_LL1526: _tmp5F5=(struct Cyc_Absyn_Exp*)_tmp5F2.hd;goto _LL1524;_LL1524: _tmp5F3=
_tmp5F2.tl;if(_tmp5F3 == 0){goto _LL1512;}else{goto _LL1513;}}}}else{goto _LL1513;}
_LL1513: goto _LL1514;_LL1510: elt_type=_tmp5EB;*t=(void**)({void**_tmp5FB=
_cycalloc(sizeof(void*));_tmp5FB[0]=elt_type;_tmp5FB;});num_elts=Cyc_Absyn_uint_exp(
1,0);Cyc_Tcexp_tcExp(te,(void**)& Cyc_Absyn_uint_t,num_elts);one_elt=1;goto
_LL1508;_LL1512:{struct _tuple0 _tmp5FD=({struct _tuple0 _tmp5FC;_tmp5FC.f1=(void*)
_tmp5F7->r;_tmp5FC.f2=(void*)_tmp5F5->r;_tmp5FC;});void*_tmp605;void*_tmp607;
void*_tmp609;void*_tmp60B;_LL1535: _LL1542: _tmp605=_tmp5FD.f1;if(*((int*)_tmp605)
== Cyc_Absyn_Sizeoftyp_e){_LL1544: _tmp607=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp605)->f1;goto _LL1536;}else{goto _LL1537;}_LL1537: _LL1546: _tmp609=_tmp5FD.f2;
if(*((int*)_tmp609)== Cyc_Absyn_Sizeoftyp_e){_LL1548: _tmp60B=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp609)->f1;goto _LL1538;}else{goto _LL1539;}_LL1539: goto _LL1540;_LL1536: Cyc_Tcexp_check_malloc_type(
0,loc,topt,_tmp607);elt_type=_tmp607;*t=(void**)({void**_tmp60D=_cycalloc(
sizeof(void*));_tmp60D[0]=elt_type;_tmp60D;});num_elts=_tmp5F5;one_elt=0;goto
_LL1534;_LL1538: Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp60B);elt_type=_tmp60B;*
t=(void**)({void**_tmp60E=_cycalloc(sizeof(void*));_tmp60E[0]=elt_type;_tmp60E;});
num_elts=_tmp5F7;one_elt=0;goto _LL1534;_LL1540: goto No_sizeof;_LL1534:;}goto
_LL1508;_LL1514: No_sizeof: elt_type=Cyc_Absyn_uchar_t;*t=(void**)({void**_tmp60F=
_cycalloc(sizeof(void*));_tmp60F[0]=elt_type;_tmp60F;});num_elts=*e;one_elt=0;
goto _LL1508;_LL1508:;}*e=num_elts;*is_fat=! one_elt;{void*_tmp610=elt_type;struct
Cyc_Absyn_TunionFieldInfo _tmp616;void*_tmp618;struct Cyc_Absyn_Tunionfield*
_tmp61A;_LL1554: if((unsigned int)_tmp610 > 3?*((int*)_tmp610)== Cyc_Absyn_TunionFieldType:
0){_LL1559: _tmp616=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp610)->f1;
_LL1561: _tmp618=(void*)_tmp616.field_info;if(*((int*)_tmp618)== Cyc_Absyn_KnownTunionfield){
_LL1563: _tmp61A=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp618)->f2;goto
_LL1555;}else{goto _LL1556;}}else{goto _LL1556;}_LL1556: goto _LL1557;_LL1555: if(
_tmp61A->tvs != 0){({void*_tmp61C[0]={};Cyc_Tcutil_terr(loc,_tag_arr("malloc with existential types not yet implemented",
sizeof(unsigned char),50),_tag_arr(_tmp61C,sizeof(void*),0));});}goto _LL1553;
_LL1557: goto _LL1553;_LL1553:;}{void*(*_tmp61D)(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq)=Cyc_Absyn_at_typ;if(topt != 0){void*_tmp61E=Cyc_Tcutil_compress(*topt);struct
Cyc_Absyn_PtrInfo _tmp624;struct Cyc_Absyn_Conref*_tmp626;_LL1568: if((unsigned int)
_tmp61E > 3?*((int*)_tmp61E)== Cyc_Absyn_PointerType: 0){_LL1573: _tmp624=((struct
Cyc_Absyn_PointerType_struct*)_tmp61E)->f1;_LL1575: _tmp626=_tmp624.nullable;goto
_LL1569;}else{goto _LL1570;}_LL1570: goto _LL1571;_LL1569: if(((int(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_val)(_tmp626)){_tmp61D=Cyc_Absyn_star_typ;}goto _LL1567;
_LL1571: goto _LL1567;_LL1567:;}if(! one_elt){_tmp61D=Cyc_Absyn_tagged_typ;}return
_tmp61D(elt_type,rgn,Cyc_Absyn_empty_tqual());}}}static void*Cyc_Tcexp_tcStmtExp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**topt,struct Cyc_Absyn_Stmt*
s){Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(te,s),s,1);Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));while(1){void*_tmp628=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp632;struct Cyc_Absyn_Stmt*_tmp634;struct Cyc_Absyn_Stmt*_tmp636;struct Cyc_Absyn_Stmt*
_tmp638;struct Cyc_Absyn_Decl*_tmp63A;_LL1578: if((unsigned int)_tmp628 > 1?*((int*)
_tmp628)== Cyc_Absyn_Exp_s: 0){_LL1587: _tmp632=((struct Cyc_Absyn_Exp_s_struct*)
_tmp628)->f1;goto _LL1579;}else{goto _LL1580;}_LL1580: if((unsigned int)_tmp628 > 1?*((
int*)_tmp628)== Cyc_Absyn_Seq_s: 0){_LL1591: _tmp636=((struct Cyc_Absyn_Seq_s_struct*)
_tmp628)->f1;goto _LL1589;_LL1589: _tmp634=((struct Cyc_Absyn_Seq_s_struct*)_tmp628)->f2;
goto _LL1581;}else{goto _LL1582;}_LL1582: if((unsigned int)_tmp628 > 1?*((int*)
_tmp628)== Cyc_Absyn_Decl_s: 0){_LL1595: _tmp63A=((struct Cyc_Absyn_Decl_s_struct*)
_tmp628)->f1;goto _LL1593;_LL1593: _tmp638=((struct Cyc_Absyn_Decl_s_struct*)
_tmp628)->f2;goto _LL1583;}else{goto _LL1584;}_LL1584: goto _LL1585;_LL1579: return(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp632->topt))->v;_LL1581: s=_tmp634;
continue;_LL1583: s=_tmp638;continue;_LL1585: return({void*_tmp63C[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("statement expression must end with expression",sizeof(
unsigned char),46),_tag_arr(_tmp63C,sizeof(void*),0));});_LL1577:;}}static void*
Cyc_Tcexp_tcCodegen(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void**
topt,struct Cyc_Absyn_Fndecl*fd){return({void*_tmp63D[0]={};Cyc_Tcexp_expr_err(te,
loc,_tag_arr("tcCodegen",sizeof(unsigned char),10),_tag_arr(_tmp63D,sizeof(void*),
0));});}static void*Cyc_Tcexp_tcFill(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*e){return({void*_tmp63E[0]={};Cyc_Tcexp_expr_err(
te,loc,_tag_arr("tcFill",sizeof(unsigned char),7),_tag_arr(_tmp63E,sizeof(void*),
0));});}static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void**topt,struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*
e1){void*rgn=(void*)Cyc_Absyn_HeapRgn;if(rgn_handle != 0){void*expected_type=(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp649=_cycalloc(sizeof(struct Cyc_Absyn_RgnHandleType_struct));
_tmp649[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp64A;_tmp64A.tag=Cyc_Absyn_RgnHandleType;
_tmp64A.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp64B=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp64B->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp64B;}));_tmp64A;});_tmp649;});void*handle_type=Cyc_Tcexp_tcExp(te,(void**)&
expected_type,(struct Cyc_Absyn_Exp*)_check_null(rgn_handle));void*_tmp63F=Cyc_Tcutil_compress(
handle_type);void*_tmp645;_LL1601: if((unsigned int)_tmp63F > 3?*((int*)_tmp63F)== 
Cyc_Absyn_RgnHandleType: 0){_LL1606: _tmp645=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp63F)->f1;goto _LL1602;}else{goto _LL1603;}_LL1603: goto _LL1604;_LL1602: rgn=
_tmp645;Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);goto _LL1600;_LL1604:({struct
Cyc_Std_String_pa_struct _tmp648;_tmp648.tag=Cyc_Std_String_pa;_tmp648.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(handle_type);{void*_tmp647[1]={& _tmp648};Cyc_Tcutil_terr(
rgn_handle->loc,_tag_arr("expecting region_t type but found %s",sizeof(
unsigned char),37),_tag_arr(_tmp647,sizeof(void*),1));}});goto _LL1600;_LL1600:;}{
void*_tmp64C=(void*)e1->r;struct Cyc_Absyn_Exp*_tmp658;struct Cyc_Absyn_Exp*
_tmp65A;struct Cyc_Absyn_Vardecl*_tmp65C;struct Cyc_List_List*_tmp65E;struct Cyc_Core_Opt*
_tmp660;struct Cyc_List_List*_tmp662;void*_tmp664;struct _tagged_arr _tmp666;
_LL1614: if(*((int*)_tmp64C)== Cyc_Absyn_Comprehension_e){_LL1629: _tmp65C=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp64C)->f1;goto _LL1627;_LL1627: _tmp65A=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp64C)->f2;goto _LL1625;_LL1625: _tmp658=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp64C)->f3;goto _LL1615;}else{goto
_LL1616;}_LL1616: if(*((int*)_tmp64C)== Cyc_Absyn_UnresolvedMem_e){_LL1633:
_tmp660=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp64C)->f1;goto _LL1631;
_LL1631: _tmp65E=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp64C)->f2;goto
_LL1617;}else{goto _LL1618;}_LL1618: if(*((int*)_tmp64C)== Cyc_Absyn_Array_e){
_LL1635: _tmp662=((struct Cyc_Absyn_Array_e_struct*)_tmp64C)->f1;goto _LL1619;}
else{goto _LL1620;}_LL1620: if(*((int*)_tmp64C)== Cyc_Absyn_Const_e){_LL1637:
_tmp664=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp64C)->f1;if((unsigned int)
_tmp664 > 1?*((int*)_tmp664)== Cyc_Absyn_String_c: 0){_LL1639: _tmp666=((struct Cyc_Absyn_String_c_struct*)
_tmp664)->f1;goto _LL1621;}else{goto _LL1622;}}else{goto _LL1622;}_LL1622: goto
_LL1623;_LL1615: {void*_tmp668=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);void*_tmp669=
Cyc_Tcutil_compress(_tmp668);struct Cyc_Absyn_Tqual _tmp66F;void*_tmp671;_LL1643:
if((unsigned int)_tmp669 > 3?*((int*)_tmp669)== Cyc_Absyn_ArrayType: 0){_LL1650:
_tmp671=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp669)->f1;goto _LL1648;
_LL1648: _tmp66F=((struct Cyc_Absyn_ArrayType_struct*)_tmp669)->f2;goto _LL1644;}
else{goto _LL1645;}_LL1645: goto _LL1646;_LL1644: {void*b=Cyc_Tcutil_is_const_exp(
te,_tmp65A)?(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp677=_cycalloc(sizeof(
struct Cyc_Absyn_Upper_b_struct));_tmp677[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp678;_tmp678.tag=Cyc_Absyn_Upper_b;_tmp678.f1=_tmp65A;_tmp678;});_tmp677;}):(
void*)Cyc_Absyn_Unknown_b;void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp674=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_struct));_tmp674[0]=({
struct Cyc_Absyn_PointerType_struct _tmp675;_tmp675.tag=Cyc_Absyn_PointerType;
_tmp675.f1=({struct Cyc_Absyn_PtrInfo _tmp676;_tmp676.elt_typ=(void*)_tmp671;
_tmp676.rgn_typ=(void*)rgn;_tmp676.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp676.tq=_tmp66F;_tmp676.bounds=Cyc_Absyn_new_conref(b);_tmp676;});_tmp675;});
_tmp674;});if(topt != 0){if(! Cyc_Tcutil_unify(*topt,res_typ)? Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp673=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmp673->v=(void*)res_typ;_tmp673;});Cyc_Tcutil_unchecked_cast(
te,e,*topt);res_typ=*topt;}}return res_typ;}_LL1646: return({void*_tmp679[0]={};
Cyc_Tcutil_impos(_tag_arr("tcNew: comprehension returned non-array type",sizeof(
unsigned char),45),_tag_arr(_tmp679,sizeof(void*),0));});_LL1642:;}_LL1617:(void*)(
e1->r=(void*)((void*)({struct Cyc_Absyn_Array_e_struct*_tmp67A=_cycalloc(sizeof(
struct Cyc_Absyn_Array_e_struct));_tmp67A[0]=({struct Cyc_Absyn_Array_e_struct
_tmp67B;_tmp67B.tag=Cyc_Absyn_Array_e;_tmp67B.f1=_tmp65E;_tmp67B;});_tmp67A;})));
_tmp662=_tmp65E;goto _LL1619;_LL1619: {void**elt_typ_opt=0;if(topt != 0){void*
_tmp67C=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_PtrInfo _tmp682;struct Cyc_Absyn_Tqual
_tmp684;void*_tmp686;void**_tmp688;_LL1662: if((unsigned int)_tmp67C > 3?*((int*)
_tmp67C)== Cyc_Absyn_PointerType: 0){_LL1667: _tmp682=((struct Cyc_Absyn_PointerType_struct*)
_tmp67C)->f1;_LL1671: _tmp686=(void*)_tmp682.elt_typ;_tmp688=(void**)&(((struct
Cyc_Absyn_PointerType_struct*)_tmp67C)->f1).elt_typ;goto _LL1669;_LL1669: _tmp684=
_tmp682.tq;goto _LL1663;}else{goto _LL1664;}_LL1664: goto _LL1665;_LL1663:
elt_typ_opt=(void**)_tmp688;goto _LL1661;_LL1665: goto _LL1661;_LL1661:;}{void*
_tmp689=Cyc_Tcexp_tcExpNoPromote(te,elt_typ_opt,e1);void*res_typ;{void*_tmp68A=
Cyc_Tcutil_compress(_tmp689);struct Cyc_Absyn_Exp*_tmp690;struct Cyc_Absyn_Tqual
_tmp692;void*_tmp694;_LL1676: if((unsigned int)_tmp68A > 3?*((int*)_tmp68A)== Cyc_Absyn_ArrayType:
0){_LL1685: _tmp694=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp68A)->f1;goto
_LL1683;_LL1683: _tmp692=((struct Cyc_Absyn_ArrayType_struct*)_tmp68A)->f2;goto
_LL1681;_LL1681: _tmp690=((struct Cyc_Absyn_ArrayType_struct*)_tmp68A)->f3;goto
_LL1677;}else{goto _LL1678;}_LL1678: goto _LL1679;_LL1677: res_typ=(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp696=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_struct));
_tmp696[0]=({struct Cyc_Absyn_PointerType_struct _tmp697;_tmp697.tag=Cyc_Absyn_PointerType;
_tmp697.f1=({struct Cyc_Absyn_PtrInfo _tmp698;_tmp698.elt_typ=(void*)_tmp694;
_tmp698.rgn_typ=(void*)rgn;_tmp698.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp698.tq=_tmp692;_tmp698.bounds=Cyc_Absyn_new_conref((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp699=_cycalloc(sizeof(struct Cyc_Absyn_Upper_b_struct));_tmp699[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp69A;_tmp69A.tag=Cyc_Absyn_Upper_b;_tmp69A.f1=(struct Cyc_Absyn_Exp*)
_check_null(_tmp690);_tmp69A;});_tmp699;}));_tmp698;});_tmp697;});_tmp696;});if(
topt != 0){if(! Cyc_Tcutil_unify(*topt,res_typ)? Cyc_Tcutil_silent_castable(te,loc,
res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp69B=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmp69B->v=(void*)res_typ;_tmp69B;});Cyc_Tcutil_unchecked_cast(te,
e,*topt);res_typ=*topt;}}goto _LL1675;_LL1679: return({void*_tmp69C[0]={};Cyc_Tcutil_impos(
_tag_arr("tcExpNoPromote on Array_e returned non-array type",sizeof(
unsigned char),50),_tag_arr(_tmp69C,sizeof(void*),0));});_LL1675:;}return res_typ;}}
_LL1621: {void*_tmp69D=Cyc_Absyn_atb_typ(Cyc_Absyn_uchar_t,rgn,Cyc_Absyn_const_tqual(),(
void*)Cyc_Absyn_Unknown_b);void*_tmp69E=Cyc_Tcexp_tcExp(te,(void**)& _tmp69D,e1);
return Cyc_Absyn_atb_typ(_tmp69E,rgn,Cyc_Absyn_empty_tqual(),(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp69F=_cycalloc(sizeof(struct Cyc_Absyn_Upper_b_struct));_tmp69F[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp6A0;_tmp6A0.tag=Cyc_Absyn_Upper_b;_tmp6A0.f1=Cyc_Absyn_uint_exp(1,0);_tmp6A0;});
_tmp69F;}));}_LL1623: {void**topt2=0;if(topt != 0){void*_tmp6A1=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmp6A9;struct Cyc_Absyn_Conref*_tmp6AB;void*_tmp6AD;
void**_tmp6AF;_LL1699: if((unsigned int)_tmp6A1 > 3?*((int*)_tmp6A1)== Cyc_Absyn_PointerType:
0){_LL1706: _tmp6A9=((struct Cyc_Absyn_PointerType_struct*)_tmp6A1)->f1;_LL1710:
_tmp6AD=(void*)_tmp6A9.elt_typ;_tmp6AF=(void**)&(((struct Cyc_Absyn_PointerType_struct*)
_tmp6A1)->f1).elt_typ;goto _LL1708;_LL1708: _tmp6AB=_tmp6A9.nullable;goto _LL1700;}
else{goto _LL1701;}_LL1701: if((unsigned int)_tmp6A1 > 3?*((int*)_tmp6A1)== Cyc_Absyn_TunionType:
0){goto _LL1702;}else{goto _LL1703;}_LL1703: goto _LL1704;_LL1700: topt2=(void**)
_tmp6AF;goto _LL1698;_LL1702: topt2=({void**_tmp6B0=_cycalloc(sizeof(void*));
_tmp6B0[0]=*topt;_tmp6B0;});goto _LL1698;_LL1704: goto _LL1698;_LL1698:;}{void*telt=
Cyc_Tcexp_tcExp(te,topt2,e1);void*res_typ=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp6B2=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_struct));_tmp6B2[0]=({
struct Cyc_Absyn_PointerType_struct _tmp6B3;_tmp6B3.tag=Cyc_Absyn_PointerType;
_tmp6B3.f1=({struct Cyc_Absyn_PtrInfo _tmp6B4;_tmp6B4.elt_typ=(void*)telt;_tmp6B4.rgn_typ=(
void*)rgn;_tmp6B4.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp6B4.tq=Cyc_Absyn_empty_tqual();_tmp6B4.bounds=Cyc_Absyn_new_conref((void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp6B5=_cycalloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
_tmp6B5[0]=({struct Cyc_Absyn_Upper_b_struct _tmp6B6;_tmp6B6.tag=Cyc_Absyn_Upper_b;
_tmp6B6.f1=Cyc_Absyn_uint_exp(1,0);_tmp6B6;});_tmp6B5;}));_tmp6B4;});_tmp6B3;});
_tmp6B2;});if(topt != 0){if(! Cyc_Tcutil_unify(*topt,res_typ)? Cyc_Tcutil_silent_castable(
te,loc,res_typ,*topt): 0){e->topt=({struct Cyc_Core_Opt*_tmp6B1=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmp6B1->v=(void*)res_typ;_tmp6B1;});Cyc_Tcutil_unchecked_cast(
te,e,*topt);res_typ=*topt;}}return res_typ;}}_LL1613:;}}void*Cyc_Tcexp_tcExp(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcutil_compress(
Cyc_Tcexp_tcExpNoPromote(te,topt,e));void*_tmp6B7=t;struct Cyc_Absyn_Exp*_tmp6BD;
struct Cyc_Absyn_Tqual _tmp6BF;void*_tmp6C1;_LL1721: if((unsigned int)_tmp6B7 > 3?*((
int*)_tmp6B7)== Cyc_Absyn_ArrayType: 0){_LL1730: _tmp6C1=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp6B7)->f1;goto _LL1728;_LL1728: _tmp6BF=((struct Cyc_Absyn_ArrayType_struct*)
_tmp6B7)->f2;goto _LL1726;_LL1726: _tmp6BD=((struct Cyc_Absyn_ArrayType_struct*)
_tmp6B7)->f3;goto _LL1722;}else{goto _LL1723;}_LL1723: goto _LL1724;_LL1722: {void*
_tmp6C5;struct _tuple6 _tmp6C3=Cyc_Tcutil_addressof_props(te,e);_LL1734: _tmp6C5=
_tmp6C3.f2;goto _LL1732;_LL1732: {void*_tmp6C7=_tmp6BD == 0?(void*)Cyc_Absyn_Unknown_b:(
void*)({struct Cyc_Absyn_Upper_b_struct*_tmp6C8=_cycalloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
_tmp6C8[0]=({struct Cyc_Absyn_Upper_b_struct _tmp6C9;_tmp6C9.tag=Cyc_Absyn_Upper_b;
_tmp6C9.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp6BD);_tmp6C9;});_tmp6C8;});t=
Cyc_Absyn_atb_typ(_tmp6C1,_tmp6C5,_tmp6BF,_tmp6C7);(void*)(((struct Cyc_Core_Opt*)
_check_null(e->topt))->v=(void*)t);return t;}}_LL1724: return t;_LL1720:;}void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){void*t=Cyc_Tcexp_tcExpNoPromote(
te,topt,e);void*_tmp6CA=(void*)e->r;void*_tmp6D4;_LL1740: if(*((int*)_tmp6CA)== 
Cyc_Absyn_Array_e){goto _LL1741;}else{goto _LL1742;}_LL1742: if(*((int*)_tmp6CA)== 
Cyc_Absyn_Comprehension_e){goto _LL1743;}else{goto _LL1744;}_LL1744: if(*((int*)
_tmp6CA)== Cyc_Absyn_Const_e){_LL1749: _tmp6D4=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp6CA)->f1;if((unsigned int)_tmp6D4 > 1?*((int*)_tmp6D4)== Cyc_Absyn_String_c: 0){
goto _LL1745;}else{goto _LL1746;}}else{goto _LL1746;}_LL1746: goto _LL1747;_LL1741:
goto _LL1743;_LL1743: goto _LL1745;_LL1745: return t;_LL1747: t=Cyc_Tcutil_compress(t);{
void*_tmp6D6=t;struct Cyc_Absyn_Exp*_tmp6DC;struct Cyc_Absyn_Tqual _tmp6DE;void*
_tmp6E0;_LL1752: if((unsigned int)_tmp6D6 > 3?*((int*)_tmp6D6)== Cyc_Absyn_ArrayType:
0){_LL1761: _tmp6E0=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp6D6)->f1;goto
_LL1759;_LL1759: _tmp6DE=((struct Cyc_Absyn_ArrayType_struct*)_tmp6D6)->f2;goto
_LL1757;_LL1757: _tmp6DC=((struct Cyc_Absyn_ArrayType_struct*)_tmp6D6)->f3;goto
_LL1753;}else{goto _LL1754;}_LL1754: goto _LL1755;_LL1753: {void*_tmp6E4;struct
_tuple6 _tmp6E2=Cyc_Tcutil_addressof_props(te,e);_LL1765: _tmp6E4=_tmp6E2.f2;goto
_LL1763;_LL1763: {void*b=_tmp6DC == 0?(void*)Cyc_Absyn_Unknown_b:(void*)({struct
Cyc_Absyn_Upper_b_struct*_tmp6E6=_cycalloc(sizeof(struct Cyc_Absyn_Upper_b_struct));
_tmp6E6[0]=({struct Cyc_Absyn_Upper_b_struct _tmp6E7;_tmp6E7.tag=Cyc_Absyn_Upper_b;
_tmp6E7.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp6DC);_tmp6E7;});_tmp6E6;});t=
Cyc_Absyn_atb_typ(_tmp6E0,_tmp6E4,_tmp6DE,b);Cyc_Tcutil_unchecked_cast(te,e,t);
return t;}}_LL1755: return t;_LL1751:;}_LL1739:;}static void*Cyc_Tcexp_tcExpNoPromote(
struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){{void*_tmp6E8=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp6EE;_LL1770: if(*((int*)_tmp6E8)== Cyc_Absyn_NoInstantiate_e){
_LL1775: _tmp6EE=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp6E8)->f1;goto
_LL1771;}else{goto _LL1772;}_LL1772: goto _LL1773;_LL1771: Cyc_Tcexp_tcExpNoInst(te,
topt,_tmp6EE);(void*)(((struct Cyc_Core_Opt*)_check_null(_tmp6EE->topt))->v=(void*)
Cyc_Absyn_pointer_expand((void*)((struct Cyc_Core_Opt*)_check_null(_tmp6EE->topt))->v));
e->topt=_tmp6EE->topt;goto _LL1769;_LL1773: Cyc_Tcexp_tcExpNoInst(te,topt,e);(void*)(((
struct Cyc_Core_Opt*)_check_null(e->topt))->v=(void*)Cyc_Absyn_pointer_expand((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v));{void*_tmp6F0=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp6F6;struct Cyc_Absyn_Conref*_tmp6F8;struct Cyc_Absyn_Tqual _tmp6FA;struct Cyc_Absyn_Conref*
_tmp6FC;void*_tmp6FE;void*_tmp700;_LL1778: if((unsigned int)_tmp6F0 > 3?*((int*)
_tmp6F0)== Cyc_Absyn_PointerType: 0){_LL1783: _tmp6F6=((struct Cyc_Absyn_PointerType_struct*)
_tmp6F0)->f1;_LL1793: _tmp700=(void*)_tmp6F6.elt_typ;goto _LL1791;_LL1791: _tmp6FE=(
void*)_tmp6F6.rgn_typ;goto _LL1789;_LL1789: _tmp6FC=_tmp6F6.nullable;goto _LL1787;
_LL1787: _tmp6FA=_tmp6F6.tq;goto _LL1785;_LL1785: _tmp6F8=_tmp6F6.bounds;goto
_LL1779;}else{goto _LL1780;}_LL1780: goto _LL1781;_LL1779:{void*_tmp702=Cyc_Tcutil_compress(
_tmp700);struct Cyc_Absyn_FnInfo _tmp708;struct Cyc_List_List*_tmp70A;struct Cyc_List_List*
_tmp70C;struct Cyc_Absyn_VarargInfo*_tmp70E;int _tmp710;struct Cyc_List_List*
_tmp712;void*_tmp714;struct Cyc_Core_Opt*_tmp716;struct Cyc_List_List*_tmp718;
_LL1796: if((unsigned int)_tmp702 > 3?*((int*)_tmp702)== Cyc_Absyn_FnType: 0){
_LL1801: _tmp708=((struct Cyc_Absyn_FnType_struct*)_tmp702)->f1;_LL1817: _tmp718=
_tmp708.tvars;goto _LL1815;_LL1815: _tmp716=_tmp708.effect;goto _LL1813;_LL1813:
_tmp714=(void*)_tmp708.ret_typ;goto _LL1811;_LL1811: _tmp712=_tmp708.args;goto
_LL1809;_LL1809: _tmp710=_tmp708.c_varargs;goto _LL1807;_LL1807: _tmp70E=_tmp708.cyc_varargs;
goto _LL1805;_LL1805: _tmp70C=_tmp708.rgn_po;goto _LL1803;_LL1803: _tmp70A=_tmp708.attributes;
goto _LL1797;}else{goto _LL1798;}_LL1798: goto _LL1799;_LL1797: if(_tmp718 != 0){
struct _RegionHandle _tmp71A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp71A;
_push_region(rgn);{struct _tuple4 _tmp71B=({struct _tuple4 _tmp725;_tmp725.f1=Cyc_Tcenv_lookup_type_vars(
te);_tmp725.f2=rgn;_tmp725;});struct Cyc_List_List*inst=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),
struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp71B,_tmp718);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(
struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,inst);void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp722=_cycalloc(sizeof(struct Cyc_Absyn_FnType_struct));_tmp722[0]=({struct Cyc_Absyn_FnType_struct
_tmp723;_tmp723.tag=Cyc_Absyn_FnType;_tmp723.f1=({struct Cyc_Absyn_FnInfo _tmp724;
_tmp724.tvars=0;_tmp724.effect=_tmp716;_tmp724.ret_typ=(void*)_tmp714;_tmp724.args=
_tmp712;_tmp724.c_varargs=_tmp710;_tmp724.cyc_varargs=_tmp70E;_tmp724.rgn_po=
_tmp70C;_tmp724.attributes=_tmp70A;_tmp724;});_tmp723;});_tmp722;}));void*
new_typ=(void*)({struct Cyc_Absyn_PointerType_struct*_tmp71F=_cycalloc(sizeof(
struct Cyc_Absyn_PointerType_struct));_tmp71F[0]=({struct Cyc_Absyn_PointerType_struct
_tmp720;_tmp720.tag=Cyc_Absyn_PointerType;_tmp720.f1=({struct Cyc_Absyn_PtrInfo
_tmp721;_tmp721.elt_typ=(void*)ftyp;_tmp721.rgn_typ=(void*)_tmp6FE;_tmp721.nullable=
_tmp6FC;_tmp721.tq=_tmp6FA;_tmp721.bounds=_tmp6F8;_tmp721;});_tmp720;});_tmp71F;});
struct Cyc_Absyn_Exp*inner=Cyc_Absyn_copy_exp(e);(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Instantiate_e_struct*_tmp71C=_cycalloc(sizeof(struct Cyc_Absyn_Instantiate_e_struct));
_tmp71C[0]=({struct Cyc_Absyn_Instantiate_e_struct _tmp71D;_tmp71D.tag=Cyc_Absyn_Instantiate_e;
_tmp71D.f1=inner;_tmp71D.f2=ts;_tmp71D;});_tmp71C;})));e->topt=({struct Cyc_Core_Opt*
_tmp71E=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp71E->v=(void*)new_typ;_tmp71E;});};
_pop_region(rgn);}goto _LL1795;_LL1799: goto _LL1795;_LL1795:;}goto _LL1777;_LL1781:
goto _LL1777;_LL1777:;}goto _LL1769;_LL1769:;}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*
te,void**topt,struct Cyc_Absyn_Exp*e){struct Cyc_Position_Segment*loc=e->loc;void*
t;{void*_tmp726=(void*)e->r;struct Cyc_Absyn_Exp*_tmp774;struct _tuple1*_tmp776;
struct Cyc_List_List*_tmp778;struct Cyc_Absyn_Exp*_tmp77A;struct Cyc_List_List*
_tmp77C;struct Cyc_Core_Opt*_tmp77E;void*_tmp780;void*_tmp782;struct _tuple1*
_tmp784;struct Cyc_List_List*_tmp786;void*_tmp788;void*_tmp78A;struct Cyc_Absyn_Exp*
_tmp78C;struct Cyc_Absyn_Exp*_tmp78E;struct Cyc_Core_Opt*_tmp790;struct Cyc_Absyn_Exp*
_tmp792;struct Cyc_Absyn_Exp*_tmp794;struct Cyc_Absyn_Exp*_tmp796;struct Cyc_Absyn_Exp*
_tmp798;struct Cyc_Absyn_Exp*_tmp79A;struct Cyc_Absyn_Exp*_tmp79C;struct Cyc_Absyn_VarargCallInfo*
_tmp79E;struct Cyc_Absyn_VarargCallInfo**_tmp7A0;struct Cyc_List_List*_tmp7A1;
struct Cyc_Absyn_Exp*_tmp7A3;struct Cyc_Absyn_Exp*_tmp7A5;struct Cyc_List_List*
_tmp7A7;struct Cyc_Absyn_Exp*_tmp7A9;struct Cyc_Absyn_Exp*_tmp7AB;void*_tmp7AD;
struct Cyc_Absyn_Exp*_tmp7AF;struct Cyc_Absyn_Exp*_tmp7B1;struct Cyc_Absyn_Exp*
_tmp7B3;struct Cyc_Absyn_Exp*_tmp7B5;void*_tmp7B7;void*_tmp7B9;void*_tmp7BB;
struct Cyc_Absyn_Exp*_tmp7BD;struct _tagged_arr*_tmp7BF;struct Cyc_Absyn_Exp*
_tmp7C1;struct _tagged_arr*_tmp7C3;struct Cyc_Absyn_Exp*_tmp7C5;struct Cyc_Absyn_Exp*
_tmp7C7;struct Cyc_Absyn_Exp*_tmp7C9;struct Cyc_List_List*_tmp7CB;struct Cyc_List_List*
_tmp7CD;struct _tuple2*_tmp7CF;struct Cyc_List_List*_tmp7D1;struct Cyc_Absyn_Stmt*
_tmp7D3;struct Cyc_Absyn_Fndecl*_tmp7D5;struct Cyc_Absyn_Exp*_tmp7D7;struct Cyc_Absyn_Exp*
_tmp7D9;struct Cyc_Absyn_Exp*_tmp7DB;struct Cyc_Absyn_Vardecl*_tmp7DD;struct Cyc_Absyn_Structdecl*
_tmp7DF;struct Cyc_Absyn_Structdecl**_tmp7E1;struct Cyc_List_List*_tmp7E2;struct
Cyc_Core_Opt*_tmp7E4;struct Cyc_Core_Opt**_tmp7E6;struct _tuple1*_tmp7E7;struct
_tuple1**_tmp7E9;struct Cyc_List_List*_tmp7EA;void*_tmp7EC;struct Cyc_Absyn_Tunionfield*
_tmp7EE;struct Cyc_Absyn_Tuniondecl*_tmp7F0;struct Cyc_List_List*_tmp7F2;struct Cyc_Core_Opt*
_tmp7F4;struct Cyc_Core_Opt**_tmp7F6;struct Cyc_Core_Opt*_tmp7F7;struct Cyc_Core_Opt**
_tmp7F9;struct Cyc_Absyn_Enumfield*_tmp7FA;struct Cyc_Absyn_Enumdecl*_tmp7FC;
struct _tuple1*_tmp7FE;struct _tuple1**_tmp800;struct Cyc_Absyn_Enumfield*_tmp801;
void*_tmp803;struct _tuple1*_tmp805;struct _tuple1**_tmp807;struct Cyc_Absyn_MallocInfo
_tmp808;int _tmp80A;int*_tmp80C;struct Cyc_Absyn_Exp*_tmp80D;struct Cyc_Absyn_Exp**
_tmp80F;void**_tmp810;void***_tmp812;struct Cyc_Absyn_Exp*_tmp813;int _tmp815;
_LL1832: if(*((int*)_tmp726)== Cyc_Absyn_NoInstantiate_e){_LL1909: _tmp774=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp726)->f1;goto _LL1833;}else{goto
_LL1834;}_LL1834: if(*((int*)_tmp726)== Cyc_Absyn_UnknownId_e){_LL1911: _tmp776=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp726)->f1;goto _LL1835;}else{goto _LL1836;}
_LL1836: if(*((int*)_tmp726)== Cyc_Absyn_UnknownCall_e){_LL1915: _tmp77A=((struct
Cyc_Absyn_UnknownCall_e_struct*)_tmp726)->f1;goto _LL1913;_LL1913: _tmp778=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp726)->f2;goto _LL1837;}else{goto _LL1838;}
_LL1838: if(*((int*)_tmp726)== Cyc_Absyn_UnresolvedMem_e){_LL1919: _tmp77E=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp726)->f1;goto _LL1917;_LL1917: _tmp77C=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp726)->f2;goto _LL1839;}else{goto
_LL1840;}_LL1840: if(*((int*)_tmp726)== Cyc_Absyn_Const_e){_LL1921: _tmp780=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp726)->f1;goto _LL1841;}else{goto _LL1842;}
_LL1842: if(*((int*)_tmp726)== Cyc_Absyn_Var_e){_LL1925: _tmp784=((struct Cyc_Absyn_Var_e_struct*)
_tmp726)->f1;goto _LL1923;_LL1923: _tmp782=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp726)->f2;goto _LL1843;}else{goto _LL1844;}_LL1844: if(*((int*)_tmp726)== Cyc_Absyn_Primop_e){
_LL1929: _tmp788=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp726)->f1;goto
_LL1927;_LL1927: _tmp786=((struct Cyc_Absyn_Primop_e_struct*)_tmp726)->f2;goto
_LL1845;}else{goto _LL1846;}_LL1846: if(*((int*)_tmp726)== Cyc_Absyn_Increment_e){
_LL1933: _tmp78C=((struct Cyc_Absyn_Increment_e_struct*)_tmp726)->f1;goto _LL1931;
_LL1931: _tmp78A=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp726)->f2;goto
_LL1847;}else{goto _LL1848;}_LL1848: if(*((int*)_tmp726)== Cyc_Absyn_AssignOp_e){
_LL1939: _tmp792=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp726)->f1;goto _LL1937;
_LL1937: _tmp790=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp726)->f2;goto _LL1935;
_LL1935: _tmp78E=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp726)->f3;goto _LL1849;}
else{goto _LL1850;}_LL1850: if(*((int*)_tmp726)== Cyc_Absyn_Conditional_e){_LL1945:
_tmp798=((struct Cyc_Absyn_Conditional_e_struct*)_tmp726)->f1;goto _LL1943;_LL1943:
_tmp796=((struct Cyc_Absyn_Conditional_e_struct*)_tmp726)->f2;goto _LL1941;_LL1941:
_tmp794=((struct Cyc_Absyn_Conditional_e_struct*)_tmp726)->f3;goto _LL1851;}else{
goto _LL1852;}_LL1852: if(*((int*)_tmp726)== Cyc_Absyn_SeqExp_e){_LL1949: _tmp79C=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp726)->f1;goto _LL1947;_LL1947: _tmp79A=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp726)->f2;goto _LL1853;}else{goto _LL1854;}
_LL1854: if(*((int*)_tmp726)== Cyc_Absyn_FnCall_e){_LL1956: _tmp7A3=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp726)->f1;goto _LL1954;_LL1954: _tmp7A1=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp726)->f2;goto _LL1951;_LL1951: _tmp79E=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp726)->f3;_tmp7A0=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_struct*)
_tmp726)->f3;goto _LL1855;}else{goto _LL1856;}_LL1856: if(*((int*)_tmp726)== Cyc_Absyn_Throw_e){
_LL1958: _tmp7A5=((struct Cyc_Absyn_Throw_e_struct*)_tmp726)->f1;goto _LL1857;}
else{goto _LL1858;}_LL1858: if(*((int*)_tmp726)== Cyc_Absyn_Instantiate_e){_LL1962:
_tmp7A9=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp726)->f1;goto _LL1960;_LL1960:
_tmp7A7=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp726)->f2;goto _LL1859;}else{
goto _LL1860;}_LL1860: if(*((int*)_tmp726)== Cyc_Absyn_Cast_e){_LL1966: _tmp7AD=(
void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp726)->f1;goto _LL1964;_LL1964: _tmp7AB=((
struct Cyc_Absyn_Cast_e_struct*)_tmp726)->f2;goto _LL1861;}else{goto _LL1862;}
_LL1862: if(*((int*)_tmp726)== Cyc_Absyn_Address_e){_LL1968: _tmp7AF=((struct Cyc_Absyn_Address_e_struct*)
_tmp726)->f1;goto _LL1863;}else{goto _LL1864;}_LL1864: if(*((int*)_tmp726)== Cyc_Absyn_New_e){
_LL1972: _tmp7B3=((struct Cyc_Absyn_New_e_struct*)_tmp726)->f1;goto _LL1970;_LL1970:
_tmp7B1=((struct Cyc_Absyn_New_e_struct*)_tmp726)->f2;goto _LL1865;}else{goto
_LL1866;}_LL1866: if(*((int*)_tmp726)== Cyc_Absyn_Sizeofexp_e){_LL1974: _tmp7B5=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp726)->f1;goto _LL1867;}else{goto _LL1868;}
_LL1868: if(*((int*)_tmp726)== Cyc_Absyn_Sizeoftyp_e){_LL1976: _tmp7B7=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp726)->f1;goto _LL1869;}else{goto _LL1870;}
_LL1870: if(*((int*)_tmp726)== Cyc_Absyn_Offsetof_e){_LL1980: _tmp7BB=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp726)->f1;goto _LL1978;_LL1978: _tmp7B9=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp726)->f2;goto _LL1871;}else{goto
_LL1872;}_LL1872: if(*((int*)_tmp726)== Cyc_Absyn_Gentyp_e){goto _LL1873;}else{
goto _LL1874;}_LL1874: if(*((int*)_tmp726)== Cyc_Absyn_Deref_e){_LL1982: _tmp7BD=((
struct Cyc_Absyn_Deref_e_struct*)_tmp726)->f1;goto _LL1875;}else{goto _LL1876;}
_LL1876: if(*((int*)_tmp726)== Cyc_Absyn_StructMember_e){_LL1986: _tmp7C1=((struct
Cyc_Absyn_StructMember_e_struct*)_tmp726)->f1;goto _LL1984;_LL1984: _tmp7BF=((
struct Cyc_Absyn_StructMember_e_struct*)_tmp726)->f2;goto _LL1877;}else{goto
_LL1878;}_LL1878: if(*((int*)_tmp726)== Cyc_Absyn_StructArrow_e){_LL1990: _tmp7C5=((
struct Cyc_Absyn_StructArrow_e_struct*)_tmp726)->f1;goto _LL1988;_LL1988: _tmp7C3=((
struct Cyc_Absyn_StructArrow_e_struct*)_tmp726)->f2;goto _LL1879;}else{goto _LL1880;}
_LL1880: if(*((int*)_tmp726)== Cyc_Absyn_Subscript_e){_LL1994: _tmp7C9=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp726)->f1;goto _LL1992;_LL1992: _tmp7C7=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp726)->f2;goto _LL1881;}else{goto _LL1882;}_LL1882: if(*((int*)_tmp726)== Cyc_Absyn_Tuple_e){
_LL1996: _tmp7CB=((struct Cyc_Absyn_Tuple_e_struct*)_tmp726)->f1;goto _LL1883;}
else{goto _LL1884;}_LL1884: if(*((int*)_tmp726)== Cyc_Absyn_CompoundLit_e){_LL2000:
_tmp7CF=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp726)->f1;goto _LL1998;_LL1998:
_tmp7CD=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp726)->f2;goto _LL1885;}else{
goto _LL1886;}_LL1886: if(*((int*)_tmp726)== Cyc_Absyn_Array_e){_LL2002: _tmp7D1=((
struct Cyc_Absyn_Array_e_struct*)_tmp726)->f1;goto _LL1887;}else{goto _LL1888;}
_LL1888: if(*((int*)_tmp726)== Cyc_Absyn_StmtExp_e){_LL2004: _tmp7D3=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp726)->f1;goto _LL1889;}else{goto _LL1890;}_LL1890: if(*((int*)_tmp726)== Cyc_Absyn_Codegen_e){
_LL2006: _tmp7D5=((struct Cyc_Absyn_Codegen_e_struct*)_tmp726)->f1;goto _LL1891;}
else{goto _LL1892;}_LL1892: if(*((int*)_tmp726)== Cyc_Absyn_Fill_e){_LL2008:
_tmp7D7=((struct Cyc_Absyn_Fill_e_struct*)_tmp726)->f1;goto _LL1893;}else{goto
_LL1894;}_LL1894: if(*((int*)_tmp726)== Cyc_Absyn_Comprehension_e){_LL2014:
_tmp7DD=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp726)->f1;goto _LL2012;
_LL2012: _tmp7DB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp726)->f2;goto
_LL2010;_LL2010: _tmp7D9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp726)->f3;
goto _LL1895;}else{goto _LL1896;}_LL1896: if(*((int*)_tmp726)== Cyc_Absyn_Struct_e){
_LL2024: _tmp7E7=((struct Cyc_Absyn_Struct_e_struct*)_tmp726)->f1;_tmp7E9=(struct
_tuple1**)&((struct Cyc_Absyn_Struct_e_struct*)_tmp726)->f1;goto _LL2021;_LL2021:
_tmp7E4=((struct Cyc_Absyn_Struct_e_struct*)_tmp726)->f2;_tmp7E6=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Struct_e_struct*)_tmp726)->f2;goto _LL2019;_LL2019: _tmp7E2=((
struct Cyc_Absyn_Struct_e_struct*)_tmp726)->f3;goto _LL2016;_LL2016: _tmp7DF=((
struct Cyc_Absyn_Struct_e_struct*)_tmp726)->f4;_tmp7E1=(struct Cyc_Absyn_Structdecl**)&((
struct Cyc_Absyn_Struct_e_struct*)_tmp726)->f4;goto _LL1897;}else{goto _LL1898;}
_LL1898: if(*((int*)_tmp726)== Cyc_Absyn_AnonStruct_e){_LL2029: _tmp7EC=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp726)->f1;goto _LL2027;_LL2027: _tmp7EA=((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp726)->f2;goto _LL1899;}else{goto _LL1900;}
_LL1900: if(*((int*)_tmp726)== Cyc_Absyn_Tunion_e){_LL2040: _tmp7F7=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp726)->f1;_tmp7F9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Tunion_e_struct*)
_tmp726)->f1;goto _LL2037;_LL2037: _tmp7F4=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp726)->f2;_tmp7F6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Tunion_e_struct*)
_tmp726)->f2;goto _LL2035;_LL2035: _tmp7F2=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp726)->f3;goto _LL2033;_LL2033: _tmp7F0=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp726)->f4;goto _LL2031;_LL2031: _tmp7EE=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp726)->f5;goto _LL1901;}else{goto _LL1902;}_LL1902: if(*((int*)_tmp726)== Cyc_Absyn_Enum_e){
_LL2047: _tmp7FE=((struct Cyc_Absyn_Enum_e_struct*)_tmp726)->f1;_tmp800=(struct
_tuple1**)&((struct Cyc_Absyn_Enum_e_struct*)_tmp726)->f1;goto _LL2045;_LL2045:
_tmp7FC=((struct Cyc_Absyn_Enum_e_struct*)_tmp726)->f2;goto _LL2043;_LL2043:
_tmp7FA=((struct Cyc_Absyn_Enum_e_struct*)_tmp726)->f3;goto _LL1903;}else{goto
_LL1904;}_LL1904: if(*((int*)_tmp726)== Cyc_Absyn_AnonEnum_e){_LL2054: _tmp805=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp726)->f1;_tmp807=(struct _tuple1**)&((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp726)->f1;goto _LL2052;_LL2052: _tmp803=(
void*)((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp726)->f2;goto _LL2050;_LL2050:
_tmp801=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp726)->f3;goto _LL1905;}else{goto
_LL1906;}_LL1906: if(*((int*)_tmp726)== Cyc_Absyn_Malloc_e){_LL2057: _tmp808=((
struct Cyc_Absyn_Malloc_e_struct*)_tmp726)->f1;_LL2070: _tmp815=_tmp808.is_calloc;
goto _LL2068;_LL2068: _tmp813=_tmp808.rgn;goto _LL2065;_LL2065: _tmp810=_tmp808.elt_type;
_tmp812=(void***)&(((struct Cyc_Absyn_Malloc_e_struct*)_tmp726)->f1).elt_type;
goto _LL2062;_LL2062: _tmp80D=_tmp808.num_elts;_tmp80F=(struct Cyc_Absyn_Exp**)&(((
struct Cyc_Absyn_Malloc_e_struct*)_tmp726)->f1).num_elts;goto _LL2059;_LL2059:
_tmp80A=_tmp808.fat_result;_tmp80C=(int*)&(((struct Cyc_Absyn_Malloc_e_struct*)
_tmp726)->f1).fat_result;goto _LL1907;}else{goto _LL1831;}_LL1833: Cyc_Tcexp_tcExpNoInst(
te,0,_tmp774);return;_LL1835: Cyc_Tcexp_resolve_unknown_id(te,e,_tmp776);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL1837: Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp77A,_tmp778);Cyc_Tcexp_tcExpNoInst(
te,topt,e);return;_LL1839: Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp77C);
Cyc_Tcexp_tcExpNoInst(te,topt,e);return;_LL1841: t=Cyc_Tcexp_tcConst(te,loc,topt,
_tmp780,e);goto _LL1831;_LL1843: t=Cyc_Tcexp_tcVar(te,loc,_tmp784,_tmp782);goto
_LL1831;_LL1845: t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp788,_tmp786);goto _LL1831;
_LL1847: t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp78C,_tmp78A);goto _LL1831;_LL1849:
t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp792,_tmp790,_tmp78E);goto _LL1831;_LL1851:
t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp798,_tmp796,_tmp794);goto _LL1831;
_LL1853: t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp79C,_tmp79A);goto _LL1831;_LL1855: t=
Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp7A3,_tmp7A1,_tmp7A0);goto _LL1831;_LL1857: t=
Cyc_Tcexp_tcThrow(te,loc,topt,_tmp7A5);goto _LL1831;_LL1859: t=Cyc_Tcexp_tcInstantiate(
te,loc,topt,_tmp7A9,_tmp7A7);goto _LL1831;_LL1861: t=Cyc_Tcexp_tcCast(te,loc,topt,
_tmp7AD,_tmp7AB);goto _LL1831;_LL1863: t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp7AF);
goto _LL1831;_LL1865: t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp7B3,e,_tmp7B1);goto _LL1831;
_LL1867: {void*_tmp817=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp7B5);t=Cyc_Tcexp_tcSizeof(
te,loc,topt,_tmp817);goto _LL1831;}_LL1869: t=Cyc_Tcexp_tcSizeof(te,loc,topt,
_tmp7B7);goto _LL1831;_LL1871: t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp7BB,_tmp7B9);
goto _LL1831;_LL1873:({void*_tmp818[0]={};Cyc_Tcutil_terr(loc,_tag_arr("gen() not in top-level initializer",
sizeof(unsigned char),35),_tag_arr(_tmp818,sizeof(void*),0));});return;_LL1875: t=
Cyc_Tcexp_tcDeref(te,loc,topt,_tmp7BD);goto _LL1831;_LL1877: t=Cyc_Tcexp_tcStructMember(
te,loc,topt,e,_tmp7C1,_tmp7BF);goto _LL1831;_LL1879: t=Cyc_Tcexp_tcStructArrow(te,
loc,topt,_tmp7C5,_tmp7C3);goto _LL1831;_LL1881: t=Cyc_Tcexp_tcSubscript(te,loc,
topt,_tmp7C9,_tmp7C7);goto _LL1831;_LL1883: t=Cyc_Tcexp_tcTuple(te,loc,topt,
_tmp7CB);goto _LL1831;_LL1885: t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp7CF,
_tmp7CD);goto _LL1831;_LL1887: {void**elt_topt=0;if(topt != 0){void*_tmp819=Cyc_Tcutil_compress(*
topt);void*_tmp81F;void**_tmp821;_LL2075: if((unsigned int)_tmp819 > 3?*((int*)
_tmp819)== Cyc_Absyn_ArrayType: 0){_LL2080: _tmp81F=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp819)->f1;_tmp821=(void**)&((void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp819)->f1);goto _LL2076;}else{goto _LL2077;}_LL2077: goto _LL2078;_LL2076:
elt_topt=(void**)_tmp821;goto _LL2074;_LL2078: goto _LL2074;_LL2074:;}t=Cyc_Tcexp_tcArray(
te,loc,elt_topt,_tmp7D1);goto _LL1831;}_LL1889: t=Cyc_Tcexp_tcStmtExp(te,loc,topt,
_tmp7D3);goto _LL1831;_LL1891: t=Cyc_Tcexp_tcCodegen(te,loc,topt,_tmp7D5);goto
_LL1831;_LL1893: t=Cyc_Tcexp_tcFill(te,loc,topt,_tmp7D7);goto _LL1831;_LL1895: t=
Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp7DD,_tmp7DB,_tmp7D9);goto _LL1831;
_LL1897: t=Cyc_Tcexp_tcStruct(te,loc,topt,_tmp7E9,_tmp7E6,_tmp7E2,_tmp7E1);goto
_LL1831;_LL1899: t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp7EC,_tmp7EA);goto _LL1831;
_LL1901: t=Cyc_Tcexp_tcTunion(te,loc,topt,e,_tmp7F9,_tmp7F6,_tmp7F2,_tmp7F0,
_tmp7EE);goto _LL1831;_LL1903:*_tmp800=((struct Cyc_Absyn_Enumfield*)_check_null(
_tmp7FA))->name;t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmp822=_cycalloc(
sizeof(struct Cyc_Absyn_EnumType_struct));_tmp822[0]=({struct Cyc_Absyn_EnumType_struct
_tmp823;_tmp823.tag=Cyc_Absyn_EnumType;_tmp823.f1=((struct Cyc_Absyn_Enumdecl*)
_check_null(_tmp7FC))->name;_tmp823.f2=_tmp7FC;_tmp823;});_tmp822;});goto _LL1831;
_LL1905:*_tmp807=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp801))->name;t=
_tmp803;goto _LL1831;_LL1907: t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp813,_tmp812,
_tmp80F,_tmp815,_tmp80C);goto _LL1831;_LL1831:;}e->topt=({struct Cyc_Core_Opt*
_tmp824=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp824->v=(void*)t;_tmp824;});}
