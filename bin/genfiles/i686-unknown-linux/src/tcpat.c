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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};extern void*Cyc_Core_snd(
struct _tuple0*);extern unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern int Cyc_List_length(struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct
Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);extern struct
Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);extern unsigned char Cyc_List_Nth[8];extern struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);extern struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*
env);struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern
unsigned char Cyc_Dict_Absent[11];struct Cyc_Set_Set;extern struct Cyc_Set_Set*Cyc_Set_empty(
int(*cmp)(void*,void*));extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*
s,void*elt);extern int Cyc_Set_cardinality(struct Cyc_Set_Set*s);extern int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern unsigned char Cyc_Set_Absent[11];extern int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);extern int Cyc_Std_zstrcmp(struct
_tagged_arr,struct _tagged_arr);extern int Cyc_Std_zstrptrcmp(struct _tagged_arr*,
struct _tagged_arr*);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[9];
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
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();extern struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(
void*x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();extern void*Cyc_Absyn_compress_kb(
void*);extern void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*
tenv);extern void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_uchar_t;
extern void*Cyc_Absyn_uint_t;extern void*Cyc_Absyn_sint_t;extern void*Cyc_Absyn_float_t;
extern struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,
struct Cyc_Absyn_Exp*init);struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern
unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
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
Cyc_List_List*curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*);extern struct _tagged_arr Cyc_Absynpp_kind2string(void*);extern struct
_tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);static const int Cyc_Tcenv_VarRes=
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
ae;struct Cyc_Core_Opt*le;};extern void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);extern struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);extern unsigned char
Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(void*t);
extern int Cyc_Tcutil_coerceable(void*);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
extern int Cyc_Tcutil_unify(void*,void*);extern void*Cyc_Tcutil_rsubstitute(struct
_RegionHandle*,struct Cyc_List_List*,void*);struct _tuple4{struct Cyc_List_List*f1;
struct _RegionHandle*f2;};struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};extern
struct _tuple5*Cyc_Tcutil_r_make_inst_var(struct _tuple4*,struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr err_msg);extern struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};extern struct _tuple6 Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p,void**topt,void**region_opt);extern void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);extern void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_List_List*);extern int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Pat*p);extern void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct _tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p){void*_tmp0=(void*)p->r;struct _tuple1*_tmpA;struct Cyc_List_List*_tmpC;struct
Cyc_List_List*_tmpE;struct _tuple1*_tmp10;struct Cyc_List_List*_tmp12;struct Cyc_List_List*
_tmp14;struct _tuple1*_tmp16;_LL2: if((unsigned int)_tmp0 > 2?*((int*)_tmp0)== Cyc_Absyn_UnknownId_p:
0){_LL11: _tmpA=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;goto _LL3;}else{
goto _LL4;}_LL4: if((unsigned int)_tmp0 > 2?*((int*)_tmp0)== Cyc_Absyn_UnknownCall_p:
0){_LL17: _tmp10=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f1;goto _LL15;
_LL15: _tmpE=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f2;goto _LL13;_LL13:
_tmpC=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f3;goto _LL5;}else{goto _LL6;}
_LL6: if((unsigned int)_tmp0 > 2?*((int*)_tmp0)== Cyc_Absyn_UnknownFields_p: 0){
_LL23: _tmp16=((struct Cyc_Absyn_UnknownFields_p_struct*)_tmp0)->f1;goto _LL21;
_LL21: _tmp14=((struct Cyc_Absyn_UnknownFields_p_struct*)_tmp0)->f2;goto _LL19;
_LL19: _tmp12=((struct Cyc_Absyn_UnknownFields_p_struct*)_tmp0)->f3;goto _LL7;}
else{goto _LL8;}_LL8: goto _LL9;_LL3:{struct _handler_cons _tmp18;_push_handler(&
_tmp18);{int _tmp1A=0;if(setjmp(_tmp18.handler)){_tmp1A=1;}if(! _tmp1A){{void*
_tmp1B=Cyc_Tcenv_lookup_ordinary(te,p->loc,_tmpA);struct Cyc_Absyn_Tunionfield*
_tmp27;struct Cyc_Absyn_Tuniondecl*_tmp29;struct Cyc_Absyn_Enumfield*_tmp2B;struct
Cyc_Absyn_Enumdecl*_tmp2D;struct Cyc_Absyn_Enumfield*_tmp2F;void*_tmp31;_LL29: if(*((
int*)_tmp1B)== Cyc_Tcenv_StructRes){goto _LL30;}else{goto _LL31;}_LL31: if(*((int*)
_tmp1B)== Cyc_Tcenv_TunionRes){_LL42: _tmp29=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp1B)->f1;goto _LL40;_LL40: _tmp27=((struct Cyc_Tcenv_TunionRes_struct*)_tmp1B)->f2;
goto _LL32;}else{goto _LL33;}_LL33: if(*((int*)_tmp1B)== Cyc_Tcenv_EnumRes){_LL46:
_tmp2D=((struct Cyc_Tcenv_EnumRes_struct*)_tmp1B)->f1;goto _LL44;_LL44: _tmp2B=((
struct Cyc_Tcenv_EnumRes_struct*)_tmp1B)->f2;goto _LL34;}else{goto _LL35;}_LL35: if(*((
int*)_tmp1B)== Cyc_Tcenv_AnonEnumRes){_LL50: _tmp31=(void*)((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp1B)->f1;goto _LL48;_LL48: _tmp2F=((struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp1B)->f2;
goto _LL36;}else{goto _LL37;}_LL37: if(*((int*)_tmp1B)== Cyc_Tcenv_VarRes){goto
_LL38;}else{goto _LL28;}_LL30:({void*_tmp33[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("struct tag used without arguments in pattern",
sizeof(unsigned char),45),_tag_arr(_tmp33,sizeof(void*),0));});_npop_handler(0);
return;_LL32:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*
_tmp34=_cycalloc(sizeof(struct Cyc_Absyn_Tunion_p_struct));_tmp34[0]=({struct Cyc_Absyn_Tunion_p_struct
_tmp35;_tmp35.tag=Cyc_Absyn_Tunion_p;_tmp35.f1=_tmp29;_tmp35.f2=_tmp27;_tmp35.f3=
0;_tmp35.f4=0;_tmp35;});_tmp34;})));_npop_handler(0);return;_LL34:(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Enum_p_struct*_tmp36=_cycalloc(sizeof(struct Cyc_Absyn_Enum_p_struct));
_tmp36[0]=({struct Cyc_Absyn_Enum_p_struct _tmp37;_tmp37.tag=Cyc_Absyn_Enum_p;
_tmp37.f1=_tmp2D;_tmp37.f2=_tmp2B;_tmp37;});_tmp36;})));_npop_handler(0);return;
_LL36:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_p_struct*_tmp38=
_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_p_struct));_tmp38[0]=({struct Cyc_Absyn_AnonEnum_p_struct
_tmp39;_tmp39.tag=Cyc_Absyn_AnonEnum_p;_tmp39.f1=(void*)_tmp31;_tmp39.f2=_tmp2F;
_tmp39;});_tmp38;})));_npop_handler(0);return;_LL38: goto _LL28;_LL28:;};
_pop_handler();}else{void*_tmp19=(void*)_exn_thrown;void*_tmp3B=_tmp19;_LL61: if(
_tmp3B == Cyc_Dict_Absent){goto _LL62;}else{goto _LL63;}_LL63: goto _LL64;_LL62: goto
_LL60;_LL64:(void)_throw(_tmp3B);_LL60:;}}}{void*_tmp41=(*_tmpA).f1;struct Cyc_List_List*
_tmp49;_LL67: if(_tmp41 == (void*)Cyc_Absyn_Loc_n){goto _LL68;}else{goto _LL69;}
_LL69: if((unsigned int)_tmp41 > 1?*((int*)_tmp41)== Cyc_Absyn_Rel_n: 0){_LL74:
_tmp49=((struct Cyc_Absyn_Rel_n_struct*)_tmp41)->f1;if(_tmp49 == 0){goto _LL70;}
else{goto _LL71;}}else{goto _LL71;}_LL71: goto _LL72;_LL68: goto _LL70;_LL70:(*_tmpA).f1=(
void*)Cyc_Absyn_Loc_n;(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*
_tmp4B=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_struct));_tmp4B[0]=({struct Cyc_Absyn_Var_p_struct
_tmp4C;_tmp4C.tag=Cyc_Absyn_Var_p;_tmp4C.f1=Cyc_Absyn_new_vardecl(_tmpA,(void*)
Cyc_Absyn_VoidType,0);_tmp4C;});_tmp4B;})));return;_LL72:({void*_tmp4D[0]={};Cyc_Tcutil_terr(
p->loc,_tag_arr("qualified variable in pattern",sizeof(unsigned char),30),
_tag_arr(_tmp4D,sizeof(void*),0));});return;_LL66:;}_LL5:{struct _handler_cons
_tmp4E;_push_handler(& _tmp4E);{int _tmp50=0;if(setjmp(_tmp4E.handler)){_tmp50=1;}
if(! _tmp50){{void*_tmp51=Cyc_Tcenv_lookup_ordinary(te,p->loc,_tmp10);struct Cyc_Absyn_Structdecl*
_tmp5D;struct Cyc_Absyn_Tunionfield*_tmp5F;struct Cyc_Absyn_Tuniondecl*_tmp61;
_LL83: if(*((int*)_tmp51)== Cyc_Tcenv_StructRes){_LL94: _tmp5D=((struct Cyc_Tcenv_StructRes_struct*)
_tmp51)->f1;goto _LL84;}else{goto _LL85;}_LL85: if(*((int*)_tmp51)== Cyc_Tcenv_TunionRes){
_LL98: _tmp61=((struct Cyc_Tcenv_TunionRes_struct*)_tmp51)->f1;goto _LL96;_LL96:
_tmp5F=((struct Cyc_Tcenv_TunionRes_struct*)_tmp51)->f2;goto _LL86;}else{goto _LL87;}
_LL87: if(*((int*)_tmp51)== Cyc_Tcenv_EnumRes){goto _LL88;}else{goto _LL89;}_LL89:
if(*((int*)_tmp51)== Cyc_Tcenv_AnonEnumRes){goto _LL90;}else{goto _LL91;}_LL91: if(*((
int*)_tmp51)== Cyc_Tcenv_VarRes){goto _LL92;}else{goto _LL82;}_LL84: {struct Cyc_List_List*
_tmp63=0;for(0;_tmpC != 0;_tmpC=_tmpC->tl){_tmp63=({struct Cyc_List_List*_tmp64=
_cycalloc(sizeof(struct Cyc_List_List));_tmp64->hd=({struct _tuple7*_tmp65=
_cycalloc(sizeof(struct _tuple7));_tmp65->f1=0;_tmp65->f2=(struct Cyc_Absyn_Pat*)
_tmpC->hd;_tmp65;});_tmp64->tl=_tmp63;_tmp64;});}(void*)(p->r=(void*)((void*)({
struct Cyc_Absyn_Struct_p_struct*_tmp66=_cycalloc(sizeof(struct Cyc_Absyn_Struct_p_struct));
_tmp66[0]=({struct Cyc_Absyn_Struct_p_struct _tmp67;_tmp67.tag=Cyc_Absyn_Struct_p;
_tmp67.f1=_tmp5D;_tmp67.f2=0;_tmp67.f3=_tmpE;_tmp67.f4=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp63);_tmp67;});_tmp66;})));
_npop_handler(0);return;}_LL86:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*
_tmp68=_cycalloc(sizeof(struct Cyc_Absyn_Tunion_p_struct));_tmp68[0]=({struct Cyc_Absyn_Tunion_p_struct
_tmp69;_tmp69.tag=Cyc_Absyn_Tunion_p;_tmp69.f1=_tmp61;_tmp69.f2=_tmp5F;_tmp69.f3=
_tmpE;_tmp69.f4=_tmpC;_tmp69;});_tmp68;})));_npop_handler(0);return;_LL88: goto
_LL90;_LL90:({struct Cyc_Std_String_pa_struct _tmp6B;_tmp6B.tag=Cyc_Std_String_pa;
_tmp6B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp10);{void*_tmp6A[1]={&
_tmp6B};Cyc_Tcutil_terr(p->loc,_tag_arr("bad enum constructor %s in pattern",
sizeof(unsigned char),35),_tag_arr(_tmp6A,sizeof(void*),1));}});_npop_handler(0);
return;_LL92: goto _LL82;_LL82:;};_pop_handler();}else{void*_tmp4F=(void*)
_exn_thrown;void*_tmp6D=_tmp4F;_LL111: if(_tmp6D == Cyc_Dict_Absent){goto _LL112;}
else{goto _LL113;}_LL113: goto _LL114;_LL112: goto _LL110;_LL114:(void)_throw(_tmp6D);
_LL110:;}}}({struct Cyc_Std_String_pa_struct _tmp74;_tmp74.tag=Cyc_Std_String_pa;
_tmp74.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp10);{void*_tmp73[1]={&
_tmp74};Cyc_Tcutil_terr(p->loc,_tag_arr("%s is not a constructor in pattern",
sizeof(unsigned char),35),_tag_arr(_tmp73,sizeof(void*),1));}});return;_LL7:{
struct _handler_cons _tmp75;_push_handler(& _tmp75);{int _tmp77=0;if(setjmp(_tmp75.handler)){
_tmp77=1;}if(! _tmp77){{void*_tmp78=Cyc_Tcenv_lookup_ordinary(te,p->loc,_tmp16);
struct Cyc_Absyn_Structdecl*_tmp7E;_LL122: if(*((int*)_tmp78)== Cyc_Tcenv_StructRes){
_LL127: _tmp7E=((struct Cyc_Tcenv_StructRes_struct*)_tmp78)->f1;goto _LL123;}else{
goto _LL124;}_LL124: goto _LL125;_LL123:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Struct_p_struct*
_tmp80=_cycalloc(sizeof(struct Cyc_Absyn_Struct_p_struct));_tmp80[0]=({struct Cyc_Absyn_Struct_p_struct
_tmp81;_tmp81.tag=Cyc_Absyn_Struct_p;_tmp81.f1=_tmp7E;_tmp81.f2=0;_tmp81.f3=
_tmp14;_tmp81.f4=_tmp12;_tmp81;});_tmp80;})));_npop_handler(0);return;_LL125:
goto _LL121;_LL121:;};_pop_handler();}else{void*_tmp76=(void*)_exn_thrown;void*
_tmp83=_tmp76;_LL133: if(_tmp83 == Cyc_Dict_Absent){goto _LL134;}else{goto _LL135;}
_LL135: goto _LL136;_LL134: goto _LL132;_LL136:(void)_throw(_tmp83);_LL132:;}}}({
struct Cyc_Std_String_pa_struct _tmp8A;_tmp8A.tag=Cyc_Std_String_pa;_tmp8A.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp16);{void*_tmp89[1]={& _tmp8A};Cyc_Tcutil_terr(
p->loc,_tag_arr("%s is not a struct name, but pattern has designators",sizeof(
unsigned char),53),_tag_arr(_tmp89,sizeof(void*),1));}});return;_LL9: return;_LL1:;}
static struct _tagged_arr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){return(*
vd->name).f2;}static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0){return*topt;}return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_mk,({
struct Cyc_Core_Opt*_tmp8B=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp8B->v=s;
_tmp8B;}));}static void*Cyc_Tcpat_num_type(void**topt,void*numt){if(topt != 0? Cyc_Tcutil_coerceable(*
topt): 0){return*topt;}{void*_tmp8C=Cyc_Tcutil_compress(numt);_LL142: if((
unsigned int)_tmp8C > 3?*((int*)_tmp8C)== Cyc_Absyn_EnumType: 0){goto _LL143;}else{
goto _LL144;}_LL144: goto _LL145;_LL143: if(topt != 0){return*topt;}goto _LL141;_LL145:
goto _LL141;_LL141:;}return numt;}static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*
vd,struct Cyc_List_List**v_result_ptr,void*t){(void*)(vd->type=(void*)t);vd->tq=
Cyc_Absyn_empty_tqual();*v_result_ptr=(struct Cyc_List_List*)({struct Cyc_List_List*
_tmp92=_cycalloc(sizeof(struct Cyc_List_List));_tmp92->hd=vd;_tmp92->tl=*
v_result_ptr;_tmp92;});}struct _tuple8{struct Cyc_Absyn_Tqual f1;void*f2;};struct
_tuple9{struct Cyc_Absyn_Structfield*f1;struct Cyc_Absyn_Pat*f2;};struct _tuple6 Cyc_Tcpat_tcPatRec(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void**region_opt){Cyc_Tcpat_resolve_pat(
te,p);{void*t;struct Cyc_List_List*tv_result=0;struct Cyc_List_List*v_result=0;{
void*_tmp93=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmpB7;struct Cyc_Absyn_Vardecl*
_tmpB9;void*_tmpBB;void*_tmpBD;struct Cyc_Absyn_Enumfield*_tmpBF;struct Cyc_Absyn_Enumdecl*
_tmpC1;struct Cyc_Absyn_Enumfield*_tmpC3;void*_tmpC5;struct Cyc_Absyn_Pat*_tmpC7;
struct Cyc_List_List*_tmpC9;struct Cyc_List_List*_tmpCB;struct Cyc_List_List*_tmpCD;
struct Cyc_Core_Opt*_tmpCF;struct Cyc_Core_Opt**_tmpD1;struct Cyc_Absyn_Structdecl*
_tmpD2;struct Cyc_List_List*_tmpD4;struct Cyc_List_List*_tmpD6;struct Cyc_Absyn_Tunionfield*
_tmpD8;struct Cyc_Absyn_Tuniondecl*_tmpDA;_LL149: if(_tmp93 == (void*)Cyc_Absyn_Wild_p){
goto _LL150;}else{goto _LL151;}_LL151: if((unsigned int)_tmp93 > 2?*((int*)_tmp93)== 
Cyc_Absyn_Var_p: 0){_LL184: _tmpB7=((struct Cyc_Absyn_Var_p_struct*)_tmp93)->f1;
goto _LL152;}else{goto _LL153;}_LL153: if((unsigned int)_tmp93 > 2?*((int*)_tmp93)== 
Cyc_Absyn_Reference_p: 0){_LL186: _tmpB9=((struct Cyc_Absyn_Reference_p_struct*)
_tmp93)->f1;goto _LL154;}else{goto _LL155;}_LL155: if((unsigned int)_tmp93 > 2?*((
int*)_tmp93)== Cyc_Absyn_Int_p: 0){_LL188: _tmpBB=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp93)->f1;if(_tmpBB == (void*)Cyc_Absyn_Unsigned){goto _LL156;}else{goto _LL157;}}
else{goto _LL157;}_LL157: if((unsigned int)_tmp93 > 2?*((int*)_tmp93)== Cyc_Absyn_Int_p:
0){_LL190: _tmpBD=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp93)->f1;if(_tmpBD == (
void*)Cyc_Absyn_Signed){goto _LL158;}else{goto _LL159;}}else{goto _LL159;}_LL159:
if((unsigned int)_tmp93 > 2?*((int*)_tmp93)== Cyc_Absyn_Char_p: 0){goto _LL160;}
else{goto _LL161;}_LL161: if((unsigned int)_tmp93 > 2?*((int*)_tmp93)== Cyc_Absyn_Float_p:
0){goto _LL162;}else{goto _LL163;}_LL163: if((unsigned int)_tmp93 > 2?*((int*)_tmp93)
== Cyc_Absyn_Enum_p: 0){_LL194: _tmpC1=((struct Cyc_Absyn_Enum_p_struct*)_tmp93)->f1;
goto _LL192;_LL192: _tmpBF=((struct Cyc_Absyn_Enum_p_struct*)_tmp93)->f2;goto _LL164;}
else{goto _LL165;}_LL165: if((unsigned int)_tmp93 > 2?*((int*)_tmp93)== Cyc_Absyn_AnonEnum_p:
0){_LL198: _tmpC5=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp93)->f1;goto
_LL196;_LL196: _tmpC3=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp93)->f2;goto _LL166;}
else{goto _LL167;}_LL167: if(_tmp93 == (void*)Cyc_Absyn_Null_p){goto _LL168;}else{
goto _LL169;}_LL169: if((unsigned int)_tmp93 > 2?*((int*)_tmp93)== Cyc_Absyn_Pointer_p:
0){_LL200: _tmpC7=((struct Cyc_Absyn_Pointer_p_struct*)_tmp93)->f1;goto _LL170;}
else{goto _LL171;}_LL171: if((unsigned int)_tmp93 > 2?*((int*)_tmp93)== Cyc_Absyn_Tuple_p:
0){_LL202: _tmpC9=((struct Cyc_Absyn_Tuple_p_struct*)_tmp93)->f1;goto _LL172;}else{
goto _LL173;}_LL173: if((unsigned int)_tmp93 > 2?*((int*)_tmp93)== Cyc_Absyn_Struct_p:
0){_LL211: _tmpD2=((struct Cyc_Absyn_Struct_p_struct*)_tmp93)->f1;goto _LL208;
_LL208: _tmpCF=((struct Cyc_Absyn_Struct_p_struct*)_tmp93)->f2;_tmpD1=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Struct_p_struct*)_tmp93)->f2;goto _LL206;_LL206: _tmpCD=((struct
Cyc_Absyn_Struct_p_struct*)_tmp93)->f3;goto _LL204;_LL204: _tmpCB=((struct Cyc_Absyn_Struct_p_struct*)
_tmp93)->f4;goto _LL174;}else{goto _LL175;}_LL175: if((unsigned int)_tmp93 > 2?*((
int*)_tmp93)== Cyc_Absyn_Tunion_p: 0){_LL219: _tmpDA=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp93)->f1;goto _LL217;_LL217: _tmpD8=((struct Cyc_Absyn_Tunion_p_struct*)_tmp93)->f2;
goto _LL215;_LL215: _tmpD6=((struct Cyc_Absyn_Tunion_p_struct*)_tmp93)->f3;goto
_LL213;_LL213: _tmpD4=((struct Cyc_Absyn_Tunion_p_struct*)_tmp93)->f4;goto _LL176;}
else{goto _LL177;}_LL177: if((unsigned int)_tmp93 > 2?*((int*)_tmp93)== Cyc_Absyn_UnknownId_p:
0){goto _LL178;}else{goto _LL179;}_LL179: if((unsigned int)_tmp93 > 2?*((int*)_tmp93)
== Cyc_Absyn_UnknownCall_p: 0){goto _LL180;}else{goto _LL181;}_LL181: if((
unsigned int)_tmp93 > 2?*((int*)_tmp93)== Cyc_Absyn_UnknownFields_p: 0){goto _LL182;}
else{goto _LL148;}_LL150: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);
goto _LL148;_LL152: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);Cyc_Tcpat_set_vd(
_tmpB7,& v_result,t);goto _LL148;_LL154: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);if(region_opt == 0){({void*_tmpDC[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("* pattern would point into an unknown region",
sizeof(unsigned char),45),_tag_arr(_tmpDC,sizeof(void*),0));});goto _LL148;}Cyc_Tcpat_set_vd(
_tmpB9,& v_result,(void*)({struct Cyc_Absyn_PointerType_struct*_tmpDD=_cycalloc(
sizeof(struct Cyc_Absyn_PointerType_struct));_tmpDD[0]=({struct Cyc_Absyn_PointerType_struct
_tmpDE;_tmpDE.tag=Cyc_Absyn_PointerType;_tmpDE.f1=({struct Cyc_Absyn_PtrInfo
_tmpDF;_tmpDF.elt_typ=(void*)t;_tmpDF.rgn_typ=(void*)*region_opt;_tmpDF.nullable=((
struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(0);_tmpDF.tq=Cyc_Absyn_empty_tqual();
_tmpDF.bounds=Cyc_Absyn_empty_conref();_tmpDF;});_tmpDE;});_tmpDD;}));goto _LL148;
_LL156: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_t);goto _LL148;_LL158: t=Cyc_Tcpat_num_type(
topt,Cyc_Absyn_sint_t);goto _LL148;_LL160: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uchar_t);
goto _LL148;_LL162: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_t);goto _LL148;_LL164:
t=Cyc_Tcpat_num_type(topt,(void*)({struct Cyc_Absyn_EnumType_struct*_tmpE0=
_cycalloc(sizeof(struct Cyc_Absyn_EnumType_struct));_tmpE0[0]=({struct Cyc_Absyn_EnumType_struct
_tmpE1;_tmpE1.tag=Cyc_Absyn_EnumType;_tmpE1.f1=_tmpC1->name;_tmpE1.f2=(struct Cyc_Absyn_Enumdecl*)
_tmpC1;_tmpE1;});_tmpE0;}));goto _LL148;_LL166: t=Cyc_Tcpat_num_type(topt,_tmpC5);
goto _LL148;_LL168: if(topt != 0){void*_tmpE2=Cyc_Tcutil_compress(*topt);_LL228: if((
unsigned int)_tmpE2 > 3?*((int*)_tmpE2)== Cyc_Absyn_PointerType: 0){goto _LL229;}
else{goto _LL230;}_LL230: goto _LL231;_LL229: t=*topt;goto tcpat_end;_LL231: goto
_LL227;_LL227:;}{struct Cyc_List_List*_tmpE8=Cyc_Tcenv_lookup_type_vars(te);t=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmpE9=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_struct));
_tmpE9[0]=({struct Cyc_Absyn_PointerType_struct _tmpEA;_tmpEA.tag=Cyc_Absyn_PointerType;
_tmpEA.f1=({struct Cyc_Absyn_PtrInfo _tmpEB;_tmpEB.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmpED=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmpED->v=_tmpE8;_tmpED;}));_tmpEB.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmpEC=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmpEC->v=_tmpE8;_tmpEC;}));_tmpEB.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(1);_tmpEB.tq=Cyc_Absyn_empty_tqual();_tmpEB.bounds=
Cyc_Absyn_empty_conref();_tmpEB;});_tmpEA;});_tmpE9;});goto _LL148;}_LL170: {void*
inner_typ=(void*)Cyc_Absyn_VoidType;void**_tmpEE=0;if(topt != 0){void*_tmpEF=Cyc_Tcutil_compress(*
topt);struct Cyc_Absyn_PtrInfo _tmpF5;void*_tmpF7;_LL241: if((unsigned int)_tmpEF > 
3?*((int*)_tmpEF)== Cyc_Absyn_PointerType: 0){_LL246: _tmpF5=((struct Cyc_Absyn_PointerType_struct*)
_tmpEF)->f1;_LL248: _tmpF7=(void*)_tmpF5.elt_typ;goto _LL242;}else{goto _LL243;}
_LL243: goto _LL244;_LL242: inner_typ=_tmpF7;_tmpEE=(void**)& inner_typ;goto _LL240;
_LL244: goto _LL240;_LL240:;}{void*ptr_rgn=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp11B=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp11B->v=Cyc_Tcenv_lookup_type_vars(te);_tmp11B;}));struct Cyc_List_List*_tmpFB;
struct Cyc_List_List*_tmpFD;struct _tuple6 _tmpF9=Cyc_Tcpat_tcPatRec(te,_tmpC7,
_tmpEE,(void**)& ptr_rgn);_LL254: _tmpFD=_tmpF9.f1;goto _LL252;_LL252: _tmpFB=_tmpF9.f2;
goto _LL250;_LL250: tv_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(tv_result,_tmpFD);v_result=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(v_result,_tmpFB);
if(_tmpEE == 0){void*_tmpFF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpC7->topt))->v);struct Cyc_Absyn_TunionFieldInfo _tmp105;struct Cyc_List_List*
_tmp107;void*_tmp109;struct Cyc_Absyn_Tunionfield*_tmp10B;struct Cyc_Absyn_Tuniondecl*
_tmp10D;_LL257: if((unsigned int)_tmpFF > 3?*((int*)_tmpFF)== Cyc_Absyn_TunionFieldType:
0){_LL262: _tmp105=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpFF)->f1;_LL266:
_tmp109=(void*)_tmp105.field_info;if(*((int*)_tmp109)== Cyc_Absyn_KnownTunionfield){
_LL270: _tmp10D=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp109)->f1;goto
_LL268;_LL268: _tmp10B=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp109)->f2;
goto _LL264;}else{goto _LL259;}_LL264: _tmp107=_tmp105.targs;goto _LL258;}else{goto
_LL259;}_LL259: goto _LL260;_LL258: t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp10F=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));_tmp10F[0]=({struct
Cyc_Absyn_TunionType_struct _tmp110;_tmp110.tag=Cyc_Absyn_TunionType;_tmp110.f1=({
struct Cyc_Absyn_TunionInfo _tmp111;_tmp111.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp112=_cycalloc(sizeof(struct Cyc_Absyn_KnownTunion_struct));_tmp112[0]=({
struct Cyc_Absyn_KnownTunion_struct _tmp113;_tmp113.tag=Cyc_Absyn_KnownTunion;
_tmp113.f1=({struct Cyc_Absyn_Tuniondecl**_tmp114=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl*));
_tmp114[0]=_tmp10D;_tmp114;});_tmp113;});_tmp112;}));_tmp111.targs=_tmp107;
_tmp111.rgn=(void*)ptr_rgn;_tmp111;});_tmp110;});_tmp10F;});goto _LL256;_LL260: t=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmp115=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_struct));
_tmp115[0]=({struct Cyc_Absyn_PointerType_struct _tmp116;_tmp116.tag=Cyc_Absyn_PointerType;
_tmp116.f1=({struct Cyc_Absyn_PtrInfo _tmp117;_tmp117.elt_typ=(void*)((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpC7->topt))->v);_tmp117.rgn_typ=(void*)
ptr_rgn;_tmp117.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp117.tq=Cyc_Absyn_empty_tqual();_tmp117.bounds=Cyc_Absyn_empty_conref();
_tmp117;});_tmp116;});_tmp115;});goto _LL256;_LL256:;}else{t=(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp118=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_struct));_tmp118[0]=({
struct Cyc_Absyn_PointerType_struct _tmp119;_tmp119.tag=Cyc_Absyn_PointerType;
_tmp119.f1=({struct Cyc_Absyn_PtrInfo _tmp11A;_tmp11A.elt_typ=(void*)((void*)((
struct Cyc_Core_Opt*)_check_null(_tmpC7->topt))->v);_tmp11A.rgn_typ=(void*)
ptr_rgn;_tmp11A.nullable=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();
_tmp11A.tq=Cyc_Absyn_empty_tqual();_tmp11A.bounds=Cyc_Absyn_empty_conref();
_tmp11A;});_tmp119;});_tmp118;});}goto _LL148;}}_LL172: {struct Cyc_List_List*
pat_ts=0;struct Cyc_List_List*topt_ts=0;if(topt != 0){void*_tmp11C=Cyc_Tcutil_compress(*
topt);struct Cyc_List_List*_tmp122;_LL286: if((unsigned int)_tmp11C > 3?*((int*)
_tmp11C)== Cyc_Absyn_TupleType: 0){_LL291: _tmp122=((struct Cyc_Absyn_TupleType_struct*)
_tmp11C)->f1;goto _LL287;}else{goto _LL288;}_LL288: goto _LL289;_LL287: topt_ts=
_tmp122;goto _LL285;_LL289: goto _LL285;_LL285:;}for(0;_tmpC9 != 0;_tmpC9=_tmpC9->tl){
void**_tmp124=0;if(topt_ts != 0){_tmp124=(void**)&(*((struct _tuple8*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{struct Cyc_List_List*_tmp127;struct Cyc_List_List*_tmp129;
struct _tuple6 _tmp125=Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpC9->hd,
_tmp124,region_opt);_LL298: _tmp129=_tmp125.f1;goto _LL296;_LL296: _tmp127=_tmp125.f2;
goto _LL294;_LL294: tv_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(tv_result,_tmp129);v_result=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(v_result,_tmp127);
pat_ts=({struct Cyc_List_List*_tmp12B=_cycalloc(sizeof(struct Cyc_List_List));
_tmp12B->hd=({struct _tuple8*_tmp12C=_cycalloc(sizeof(struct _tuple8));_tmp12C->f1=
Cyc_Absyn_empty_tqual();_tmp12C->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Pat*)_tmpC9->hd)->topt))->v;_tmp12C;});_tmp12B->tl=pat_ts;
_tmp12B;});}}t=(void*)({struct Cyc_Absyn_TupleType_struct*_tmp12D=_cycalloc(
sizeof(struct Cyc_Absyn_TupleType_struct));_tmp12D[0]=({struct Cyc_Absyn_TupleType_struct
_tmp12E;_tmp12E.tag=Cyc_Absyn_TupleType;_tmp12E.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmp12E;});_tmp12D;});goto
_LL148;}_LL174: if(_tmpCD != 0){(int)_throw(({void*_tmp12F[0]={};Cyc_Tcutil_impos(
_tag_arr("tcPat: struct<...> not implemented",sizeof(unsigned char),35),_tag_arr(
_tmp12F,sizeof(void*),0));}));}{struct _RegionHandle _tmp130=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp130;_push_region(rgn);{struct _tuple4 _tmp131=({
struct _tuple4 _tmp14B;_tmp14B.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp14B.f2=rgn;
_tmp14B;});struct Cyc_List_List*_tmp132=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp131,_tmpD2->tvs);struct Cyc_List_List*_tmp133=((struct Cyc_List_List*(*)(void*(*
f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp132);t=(void*)({struct Cyc_Absyn_StructType_struct*_tmp134=
_cycalloc(sizeof(struct Cyc_Absyn_StructType_struct));_tmp134[0]=({struct Cyc_Absyn_StructType_struct
_tmp135;_tmp135.tag=Cyc_Absyn_StructType;_tmp135.f1=(struct _tuple1*)((struct
_tuple1*)((struct Cyc_Core_Opt*)_check_null(_tmpD2->name))->v);_tmp135.f2=_tmp133;
_tmp135.f3=({struct Cyc_Absyn_Structdecl**_tmp136=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl*));
_tmp136[0]=_tmpD2;_tmp136;});_tmp135;});_tmp134;});if(topt != 0){Cyc_Tcutil_unify(
t,*topt);}*_tmpD1=({struct Cyc_Core_Opt*_tmp137=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp137->v=_tmp133;_tmp137;});if(_tmpD2->fields == 0){({void*_tmp138[0]={};Cyc_Tcutil_terr(
p->loc,_tag_arr("can't destructure an abstract struct",sizeof(unsigned char),37),
_tag_arr(_tmp138,sizeof(void*),0));});_npop_handler(0);goto _LL148;}{struct Cyc_List_List*
fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,p->loc,_tmpCB,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpD2->fields))->v);
for(0;fields != 0;fields=fields->tl){struct _tuple9 _tmp13B;struct Cyc_Absyn_Pat*
_tmp13C;struct Cyc_Absyn_Structfield*_tmp13E;struct _tuple9*_tmp139=(struct _tuple9*)
fields->hd;_tmp13B=*_tmp139;_LL319: _tmp13E=_tmp13B.f1;goto _LL317;_LL317: _tmp13C=
_tmp13B.f2;goto _LL314;_LL314: {void*_tmp140=Cyc_Tcutil_rsubstitute(rgn,_tmp132,(
void*)_tmp13E->type);struct Cyc_List_List*_tmp143;struct Cyc_List_List*_tmp145;
struct _tuple6 _tmp141=Cyc_Tcpat_tcPatRec(te,_tmp13C,(void**)& _tmp140,region_opt);
_LL326: _tmp145=_tmp141.f1;goto _LL324;_LL324: _tmp143=_tmp141.f2;goto _LL322;_LL322:
tv_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(tv_result,_tmp145);v_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(v_result,_tmp143);if(! Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp13C->topt))->v,_tmp140)){({struct Cyc_Std_String_pa_struct
_tmp14A;_tmp14A.tag=Cyc_Std_String_pa;_tmp14A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp13C->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp149;_tmp149.tag=Cyc_Std_String_pa;_tmp149.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp140);{struct Cyc_Std_String_pa_struct _tmp148;_tmp148.tag=Cyc_Std_String_pa;
_tmp148.f1=(struct _tagged_arr)*_tmp13E->name;{void*_tmp147[3]={& _tmp148,& _tmp149,&
_tmp14A};Cyc_Tcutil_terr(p->loc,_tag_arr("field %s of struct pattern expects type %s != %s",
sizeof(unsigned char),49),_tag_arr(_tmp147,sizeof(void*),3));}}}});}}}}};
_pop_region(rgn);}goto _LL148;_LL176: if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpD6)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpD8->tvs)){({void*
_tmp14C[0]={};Cyc_Tcutil_terr(p->loc,_tag_arr("wrong number of existential type variables in pattern",
sizeof(unsigned char),54),_tag_arr(_tmp14C,sizeof(void*),0));});}if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpD6)!= 0){region_opt=0;}{struct
_RegionHandle _tmp14D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp14D;
_push_region(rgn);{struct Cyc_List_List*_tmp14E=0;{struct Cyc_List_List*t=_tmpD6;
for(0;t != 0;t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;{void*
_tmp14F=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp15D;struct Cyc_Core_Opt*
_tmp15F;struct Cyc_Core_Opt**_tmp161;void*_tmp162;struct Cyc_Core_Opt*_tmp164;
struct Cyc_Core_Opt**_tmp166;void*_tmp167;struct Cyc_Core_Opt*_tmp169;struct Cyc_Core_Opt**
_tmp16B;void*_tmp16C;void*_tmp16E;_LL337: if(*((int*)_tmp14F)== Cyc_Absyn_Eq_kb){
_LL350: _tmp15D=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp14F)->f1;if(_tmp15D == (
void*)Cyc_Absyn_BoxKind){goto _LL338;}else{goto _LL339;}}else{goto _LL339;}_LL339:
if(*((int*)_tmp14F)== Cyc_Absyn_Unknown_kb){_LL352: _tmp15F=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp14F)->f1;_tmp161=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp14F)->f1;goto _LL340;}else{goto _LL341;}_LL341: if(*((int*)_tmp14F)== Cyc_Absyn_Less_kb){
_LL357: _tmp164=((struct Cyc_Absyn_Less_kb_struct*)_tmp14F)->f1;_tmp166=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp14F)->f1;goto _LL355;_LL355: _tmp162=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp14F)->f2;if(_tmp162 == (void*)Cyc_Absyn_MemKind){
goto _LL342;}else{goto _LL343;}}else{goto _LL343;}_LL343: if(*((int*)_tmp14F)== Cyc_Absyn_Less_kb){
_LL362: _tmp169=((struct Cyc_Absyn_Less_kb_struct*)_tmp14F)->f1;_tmp16B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp14F)->f1;goto _LL360;_LL360: _tmp167=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp14F)->f2;if(_tmp167 == (void*)Cyc_Absyn_AnyKind){
goto _LL344;}else{goto _LL345;}}else{goto _LL345;}_LL345: if(*((int*)_tmp14F)== Cyc_Absyn_Eq_kb){
_LL365: _tmp16C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp14F)->f1;goto _LL346;}
else{goto _LL347;}_LL347: if(*((int*)_tmp14F)== Cyc_Absyn_Less_kb){_LL367: _tmp16E=(
void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp14F)->f2;goto _LL348;}else{goto _LL336;}
_LL338: goto _LL336;_LL340: _tmp166=_tmp161;goto _LL342;_LL342: _tmp16B=_tmp166;goto
_LL344;_LL344:*_tmp16B=({struct Cyc_Core_Opt*_tmp170=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp170->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp171=_cycalloc(
sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp171[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp172;_tmp172.tag=Cyc_Absyn_Eq_kb;_tmp172.f1=(void*)((void*)Cyc_Absyn_BoxKind);
_tmp172;});_tmp171;}));_tmp170;});goto _LL336;_LL346: _tmp16E=_tmp16C;goto _LL348;
_LL348:({struct Cyc_Std_String_pa_struct _tmp176;_tmp176.tag=Cyc_Std_String_pa;
_tmp176.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp16E);{struct Cyc_Std_String_pa_struct
_tmp175;_tmp175.tag=Cyc_Std_String_pa;_tmp175.f1=(struct _tagged_arr)*tv->name;{
struct Cyc_Std_String_pa_struct _tmp174;_tmp174.tag=Cyc_Std_String_pa;_tmp174.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpDA->name);{void*_tmp173[3]={&
_tmp174,& _tmp175,& _tmp176};Cyc_Tcutil_terr(p->loc,_tag_arr("tunion member %s abstracts type variable %s of kind %s != B",
sizeof(unsigned char),60),_tag_arr(_tmp173,sizeof(void*),3));}}}});goto _LL336;
_LL336:;}_tmp14E=({struct Cyc_List_List*_tmp177=_region_malloc(rgn,sizeof(struct
Cyc_List_List));_tmp177->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp178=_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));_tmp178[0]=({struct Cyc_Absyn_VarType_struct
_tmp179;_tmp179.tag=Cyc_Absyn_VarType;_tmp179.f1=tv;_tmp179;});_tmp178;}));
_tmp177->tl=_tmp14E;_tmp177;});}}_tmp14E=Cyc_List_imp_rev(_tmp14E);tv_result=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
tv_result,_tmpD6);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,
_tmpD6);struct Cyc_List_List*tqts=_tmpD8->typs;struct Cyc_List_List*_tmp17A=Cyc_Tcenv_lookup_type_vars(
te);struct _tuple4 _tmp17B=({struct _tuple4 _tmp1AF;_tmp1AF.f1=_tmp17A;_tmp1AF.f2=
rgn;_tmp1AF;});struct Cyc_List_List*_tmp17C=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmp17B,_tmpDA->tvs);struct Cyc_List_List*_tmp17D=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpD8->tvs,_tmp14E);struct Cyc_List_List*_tmp17E=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmp17C);struct Cyc_List_List*_tmp17F=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmp17D);struct Cyc_List_List*_tmp180=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rappend)(rgn,_tmp17C,_tmp17D);if(tqts == 0){t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp181=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));_tmp181[0]=({struct
Cyc_Absyn_TunionType_struct _tmp182;_tmp182.tag=Cyc_Absyn_TunionType;_tmp182.f1=({
struct Cyc_Absyn_TunionInfo _tmp183;_tmp183.tunion_info=(void*)((void*)({struct Cyc_Absyn_KnownTunion_struct*
_tmp185=_cycalloc(sizeof(struct Cyc_Absyn_KnownTunion_struct));_tmp185[0]=({
struct Cyc_Absyn_KnownTunion_struct _tmp186;_tmp186.tag=Cyc_Absyn_KnownTunion;
_tmp186.f1=({struct Cyc_Absyn_Tuniondecl**_tmp187=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl*));
_tmp187[0]=_tmpDA;_tmp187;});_tmp186;});_tmp185;}));_tmp183.targs=_tmp17E;
_tmp183.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({
struct Cyc_Core_Opt*_tmp184=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp184->v=
_tmp17A;_tmp184;}));_tmp183;});_tmp182;});_tmp181;});}else{t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp188=_cycalloc(sizeof(struct Cyc_Absyn_TunionFieldType_struct));_tmp188[0]=({
struct Cyc_Absyn_TunionFieldType_struct _tmp189;_tmp189.tag=Cyc_Absyn_TunionFieldType;
_tmp189.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp18A;_tmp18A.field_info=(void*)((
void*)({struct Cyc_Absyn_KnownTunionfield_struct*_tmp18B=_cycalloc(sizeof(struct
Cyc_Absyn_KnownTunionfield_struct));_tmp18B[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp18C;_tmp18C.tag=Cyc_Absyn_KnownTunionfield;_tmp18C.f1=_tmpDA;_tmp18C.f2=
_tmpD8;_tmp18C;});_tmp18B;}));_tmp18A.targs=_tmp17E;_tmp18A;});_tmp189;});
_tmp188;});}if(topt != 0? tqts == 0: 0){void*_tmp18D=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_TunionInfo _tmp195;struct Cyc_List_List*_tmp197;_LL399: if((
unsigned int)_tmp18D > 3?*((int*)_tmp18D)== Cyc_Absyn_TunionFieldType: 0){goto
_LL400;}else{goto _LL401;}_LL401: if((unsigned int)_tmp18D > 3?*((int*)_tmp18D)== 
Cyc_Absyn_TunionType: 0){_LL406: _tmp195=((struct Cyc_Absyn_TunionType_struct*)
_tmp18D)->f1;_LL408: _tmp197=_tmp195.targs;goto _LL402;}else{goto _LL403;}_LL403:
goto _LL404;_LL400: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp199=
_cycalloc(sizeof(struct Cyc_Absyn_TunionFieldType_struct));_tmp199[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp19A;_tmp19A.tag=Cyc_Absyn_TunionFieldType;_tmp19A.f1=({struct Cyc_Absyn_TunionFieldInfo
_tmp19B;_tmp19B.field_info=(void*)((void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp19C=_cycalloc(sizeof(struct Cyc_Absyn_KnownTunionfield_struct));_tmp19C[0]=({
struct Cyc_Absyn_KnownTunionfield_struct _tmp19D;_tmp19D.tag=Cyc_Absyn_KnownTunionfield;
_tmp19D.f1=_tmpDA;_tmp19D.f2=_tmpD8;_tmp19D;});_tmp19C;}));_tmp19B.targs=_tmp17E;
_tmp19B;});_tmp19A;});_tmp199;});goto _LL398;_LL402: {struct Cyc_List_List*_tmp19E=
_tmp17E;for(0;_tmp19E != 0? _tmp197 != 0: 0;(_tmp19E=_tmp19E->tl,_tmp197=_tmp197->tl)){
Cyc_Tcutil_unify((void*)_tmp19E->hd,(void*)_tmp197->hd);}goto _LL398;}_LL404: goto
_LL398;_LL398:;}for(0;_tmpD4 != 0? tqts != 0: 0;(_tmpD4=_tmpD4->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmp19F=(struct Cyc_Absyn_Pat*)_tmpD4->hd;void*_tmp1A0=Cyc_Tcutil_rsubstitute(
rgn,_tmp180,(*((struct _tuple8*)tqts->hd)).f2);struct Cyc_List_List*_tmp1A3;struct
Cyc_List_List*_tmp1A5;struct _tuple6 _tmp1A1=Cyc_Tcpat_tcPatRec(te2,_tmp19F,(void**)&
_tmp1A0,region_opt);_LL422: _tmp1A5=_tmp1A1.f1;goto _LL420;_LL420: _tmp1A3=_tmp1A1.f2;
goto _LL418;_LL418: tv_result=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(tv_result,_tmp1A5);v_result=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(v_result,_tmp1A3);
if(! Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp19F->topt))->v,
_tmp1A0)){({struct Cyc_Std_String_pa_struct _tmp1AA;_tmp1AA.tag=Cyc_Std_String_pa;
_tmp1AA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp19F->topt))->v);{struct Cyc_Std_String_pa_struct _tmp1A9;_tmp1A9.tag=
Cyc_Std_String_pa;_tmp1A9.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp1A0);{
struct Cyc_Std_String_pa_struct _tmp1A8;_tmp1A8.tag=Cyc_Std_String_pa;_tmp1A8.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpDA->name);{void*_tmp1A7[3]={&
_tmp1A8,& _tmp1A9,& _tmp1AA};Cyc_Tcutil_terr(_tmp19F->loc,_tag_arr("%s expects argument type %s, not %s",
sizeof(unsigned char),36),_tag_arr(_tmp1A7,sizeof(void*),3));}}}});}}if(_tmpD4 != 
0){({struct Cyc_Std_String_pa_struct _tmp1AC;_tmp1AC.tag=Cyc_Std_String_pa;_tmp1AC.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpDA->name);{void*_tmp1AB[1]={&
_tmp1AC};Cyc_Tcutil_terr(p->loc,_tag_arr("too many arguments for tunion constructor %s",
sizeof(unsigned char),45),_tag_arr(_tmp1AB,sizeof(void*),1));}});}if(tqts != 0){({
struct Cyc_Std_String_pa_struct _tmp1AE;_tmp1AE.tag=Cyc_Std_String_pa;_tmp1AE.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpDA->name);{void*_tmp1AD[1]={&
_tmp1AE};Cyc_Tcutil_terr(p->loc,_tag_arr("too few arguments for tunion constructor %s",
sizeof(unsigned char),44),_tag_arr(_tmp1AD,sizeof(void*),1));}});}}};_pop_region(
rgn);}goto _LL148;_LL178: goto _LL180;_LL180: goto _LL182;_LL182: t=Cyc_Absyn_wildtyp(({
struct Cyc_Core_Opt*_tmp1B0=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1B0->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp1B0;}));goto _LL148;_LL148:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*
_tmp1B1=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1B1->v=(void*)t;_tmp1B1;});
return({struct _tuple6 _tmp1B2;_tmp1B2.f1=tv_result;_tmp1B2.f2=v_result;_tmp1B2;});}}
struct _tuple6 Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**
topt,void**region_opt){struct _tuple6 _tmp1B3=Cyc_Tcpat_tcPatRec(te,p,topt,
region_opt);{struct _RegionHandle _tmp1B4=_new_region("r");struct _RegionHandle*r=&
_tmp1B4;_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tagged_arr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp1B3.f2),p->loc,_tag_arr("pattern contains a repeated variable",
sizeof(unsigned char),37));;_pop_region(r);}return _tmp1B3;}void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmp1B5=(void*)p->r;struct
Cyc_Absyn_Pat*_tmp1C1;struct Cyc_List_List*_tmp1C3;struct Cyc_List_List*_tmp1C5;
struct Cyc_List_List*_tmp1C7;_LL439: if((unsigned int)_tmp1B5 > 2?*((int*)_tmp1B5)
== Cyc_Absyn_Pointer_p: 0){_LL450: _tmp1C1=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp1B5)->f1;goto _LL440;}else{goto _LL441;}_LL441: if((unsigned int)_tmp1B5 > 2?*((
int*)_tmp1B5)== Cyc_Absyn_Tunion_p: 0){_LL452: _tmp1C3=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1B5)->f4;goto _LL442;}else{goto _LL443;}_LL443: if((unsigned int)_tmp1B5 > 2?*((
int*)_tmp1B5)== Cyc_Absyn_Struct_p: 0){_LL454: _tmp1C5=((struct Cyc_Absyn_Struct_p_struct*)
_tmp1B5)->f4;goto _LL444;}else{goto _LL445;}_LL445: if((unsigned int)_tmp1B5 > 2?*((
int*)_tmp1B5)== Cyc_Absyn_Tuple_p: 0){_LL456: _tmp1C7=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp1B5)->f1;goto _LL446;}else{goto _LL447;}_LL447: goto _LL448;_LL440: Cyc_Tcpat_check_pat_regions(
te,_tmp1C1);{void*_tmp1C9=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_PtrInfo _tmp1D1;void*_tmp1D3;struct Cyc_Absyn_TunionInfo _tmp1D5;
void*_tmp1D7;_LL459: if((unsigned int)_tmp1C9 > 3?*((int*)_tmp1C9)== Cyc_Absyn_PointerType:
0){_LL466: _tmp1D1=((struct Cyc_Absyn_PointerType_struct*)_tmp1C9)->f1;_LL468:
_tmp1D3=(void*)_tmp1D1.rgn_typ;goto _LL460;}else{goto _LL461;}_LL461: if((
unsigned int)_tmp1C9 > 3?*((int*)_tmp1C9)== Cyc_Absyn_TunionType: 0){_LL470:
_tmp1D5=((struct Cyc_Absyn_TunionType_struct*)_tmp1C9)->f1;_LL472: _tmp1D7=(void*)
_tmp1D5.rgn;goto _LL462;}else{goto _LL463;}_LL463: goto _LL464;_LL460: _tmp1D7=
_tmp1D3;goto _LL462;_LL462: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp1D7);
return;_LL464:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1D9=
_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp1D9[0]=({struct Cyc_Core_Impossible_struct
_tmp1DA;_tmp1DA.tag=Cyc_Core_Impossible;_tmp1DA.f1=_tag_arr("check_pat_regions: bad pointer type",
sizeof(unsigned char),36);_tmp1DA;});_tmp1D9;}));_LL458:;}_LL442: for(0;_tmp1C3 != 
0;_tmp1C3=_tmp1C3->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp1C3->hd);}{void*_tmp1DB=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;
struct Cyc_Absyn_TunionInfo _tmp1E3;void*_tmp1E5;_LL477: if((unsigned int)_tmp1DB > 
3?*((int*)_tmp1DB)== Cyc_Absyn_TunionType: 0){_LL484: _tmp1E3=((struct Cyc_Absyn_TunionType_struct*)
_tmp1DB)->f1;_LL486: _tmp1E5=(void*)_tmp1E3.rgn;goto _LL478;}else{goto _LL479;}
_LL479: if((unsigned int)_tmp1DB > 3?*((int*)_tmp1DB)== Cyc_Absyn_TunionFieldType:
0){goto _LL480;}else{goto _LL481;}_LL481: goto _LL482;_LL478: Cyc_Tcenv_check_rgn_accessible(
te,p->loc,_tmp1E5);return;_LL480: return;_LL482:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp1E7=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp1E7[0]=({struct
Cyc_Core_Impossible_struct _tmp1E8;_tmp1E8.tag=Cyc_Core_Impossible;_tmp1E8.f1=
_tag_arr("check_pat_regions: bad tunion type",sizeof(unsigned char),35);_tmp1E8;});
_tmp1E7;}));_LL476:;}_LL444: _tmp1C7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Pat*(*
f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Pat*(*)(
struct _tuple7*))Cyc_Core_snd,_tmp1C5);goto _LL446;_LL446: for(0;_tmp1C7 != 0;
_tmp1C7=_tmp1C7->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp1C7->hd);}return;_LL448: return;_LL438:;}static const int Cyc_Tcpat_Name_v=0;
struct Cyc_Tcpat_Name_v_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Tcpat_Int_v=
1;struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};struct Cyc_Tcpat_Con_s{void*name;
int arity;struct Cyc_Core_Opt*span;};static const int Cyc_Tcpat_Any=0;static const int
Cyc_Tcpat_Con=0;struct Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*f1;
struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,
struct Cyc_Tcpat_Con_s*c2){void*_tmp1E9=(void*)c1->name;struct _tagged_arr _tmp1EF;
int _tmp1F1;_LL491: if(*((int*)_tmp1E9)== Cyc_Tcpat_Name_v){_LL496: _tmp1EF=((
struct Cyc_Tcpat_Name_v_struct*)_tmp1E9)->f1;goto _LL492;}else{goto _LL493;}_LL493:
if(*((int*)_tmp1E9)== Cyc_Tcpat_Int_v){_LL498: _tmp1F1=((struct Cyc_Tcpat_Int_v_struct*)
_tmp1E9)->f1;goto _LL494;}else{goto _LL490;}_LL492: {void*_tmp1F3=(void*)c2->name;
struct _tagged_arr _tmp1F9;_LL501: if(*((int*)_tmp1F3)== Cyc_Tcpat_Name_v){_LL506:
_tmp1F9=((struct Cyc_Tcpat_Name_v_struct*)_tmp1F3)->f1;goto _LL502;}else{goto
_LL503;}_LL503: if(*((int*)_tmp1F3)== Cyc_Tcpat_Int_v){goto _LL504;}else{goto
_LL500;}_LL502: return Cyc_Std_zstrcmp(_tmp1EF,_tmp1F9);_LL504: return - 1;_LL500:;}
_LL494: {void*_tmp1FB=(void*)c2->name;int _tmp201;_LL509: if(*((int*)_tmp1FB)== 
Cyc_Tcpat_Name_v){goto _LL510;}else{goto _LL511;}_LL511: if(*((int*)_tmp1FB)== Cyc_Tcpat_Int_v){
_LL514: _tmp201=((struct Cyc_Tcpat_Int_v_struct*)_tmp1FB)->f1;goto _LL512;}else{
goto _LL508;}_LL510: return 1;_LL512: return _tmp1F1 - _tmp201;_LL508:;}_LL490:;}
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(){return((struct Cyc_Set_Set*(*)(
int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_empty)(Cyc_Tcpat_compare_con);}
static struct Cyc_Core_Opt Cyc_Tcpat_one_opt={(void*)1};static struct Cyc_Core_Opt Cyc_Tcpat_two_opt={(
void*)2};static struct Cyc_Core_Opt*Cyc_Tcpat_one_opt_ptr=(struct Cyc_Core_Opt*)&
Cyc_Tcpat_one_opt;static unsigned char _tmp204[5]="NULL";static struct Cyc_Tcpat_Name_v_struct
Cyc_Tcpat_null_name_value={0,{_tmp204,_tmp204,_tmp204 + 5}};static unsigned char
_tmp206[4]="ptr";static struct Cyc_Tcpat_Name_v_struct Cyc_Tcpat_ptr_name_value={0,{
_tmp206,_tmp206,_tmp206 + 4}};static unsigned char _tmp208[6]="tuple";static struct
Cyc_Tcpat_Name_v_struct Cyc_Tcpat_tuple_name_value={0,{_tmp208,_tmp208,_tmp208 + 6}};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_con0={(void*)((void*)& Cyc_Tcpat_null_name_value),
0,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};static struct Cyc_Tcpat_Con_s Cyc_Tcpat_null_ptr_con0={(
void*)((void*)& Cyc_Tcpat_ptr_name_value),1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_two_opt};
static struct Cyc_Tcpat_Con_s Cyc_Tcpat_ptr_con0={(void*)((void*)& Cyc_Tcpat_ptr_name_value),
1,(struct Cyc_Core_Opt*)& Cyc_Tcpat_one_opt};static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con=&
Cyc_Tcpat_null_con0;static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con=& Cyc_Tcpat_null_ptr_con0;
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con=& Cyc_Tcpat_ptr_con0;static struct
Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i){return({struct Cyc_Tcpat_Con_s*_tmp209=
_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));_tmp209->name=(void*)((void*)({struct
Cyc_Tcpat_Int_v_struct*_tmp20A=_cycalloc_atomic(sizeof(struct Cyc_Tcpat_Int_v_struct));
_tmp20A[0]=({struct Cyc_Tcpat_Int_v_struct _tmp20B;_tmp20B.tag=Cyc_Tcpat_Int_v;
_tmp20B.f1=i;_tmp20B;});_tmp20A;}));_tmp209->arity=0;_tmp209->span=0;_tmp209;});}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _tagged_arr f){return({
struct Cyc_Tcpat_Con_s*_tmp20C=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));_tmp20C->name=(
void*)((void*)({struct Cyc_Tcpat_Name_v_struct*_tmp20D=_cycalloc(sizeof(struct Cyc_Tcpat_Name_v_struct));
_tmp20D[0]=({struct Cyc_Tcpat_Name_v_struct _tmp20E;_tmp20E.tag=Cyc_Tcpat_Name_v;
_tmp20E.f1=f;_tmp20E;});_tmp20D;}));_tmp20C->arity=0;_tmp20C->span=0;_tmp20C;});}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(unsigned char c){return({struct Cyc_Tcpat_Con_s*
_tmp20F=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));_tmp20F->name=(void*)((void*)({
struct Cyc_Tcpat_Int_v_struct*_tmp211=_cycalloc_atomic(sizeof(struct Cyc_Tcpat_Int_v_struct));
_tmp211[0]=({struct Cyc_Tcpat_Int_v_struct _tmp212;_tmp212.tag=Cyc_Tcpat_Int_v;
_tmp212.f1=(int)c;_tmp212;});_tmp211;}));_tmp20F->arity=0;_tmp20F->span=({struct
Cyc_Core_Opt*_tmp210=_cycalloc_atomic(sizeof(struct Cyc_Core_Opt));_tmp210->v=(
void*)256;_tmp210;});_tmp20F;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(
int i){return({struct Cyc_Tcpat_Con_s*_tmp213=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));
_tmp213->name=(void*)((void*)& Cyc_Tcpat_tuple_name_value);_tmp213->arity=i;
_tmp213->span=Cyc_Tcpat_one_opt_ptr;_tmp213;});}static void*Cyc_Tcpat_null_pat(){
return(void*)({struct Cyc_Tcpat_Con_struct*_tmp214=_cycalloc(sizeof(struct Cyc_Tcpat_Con_struct));
_tmp214[0]=({struct Cyc_Tcpat_Con_struct _tmp215;_tmp215.tag=Cyc_Tcpat_Con;_tmp215.f1=
Cyc_Tcpat_null_con;_tmp215.f2=0;_tmp215;});_tmp214;});}static void*Cyc_Tcpat_int_pat(
int i){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp216=_cycalloc(sizeof(struct
Cyc_Tcpat_Con_struct));_tmp216[0]=({struct Cyc_Tcpat_Con_struct _tmp217;_tmp217.tag=
Cyc_Tcpat_Con;_tmp217.f1=Cyc_Tcpat_int_con(i);_tmp217.f2=0;_tmp217;});_tmp216;});}
static void*Cyc_Tcpat_char_pat(unsigned char c){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp218=_cycalloc(sizeof(struct Cyc_Tcpat_Con_struct));_tmp218[0]=({struct Cyc_Tcpat_Con_struct
_tmp219;_tmp219.tag=Cyc_Tcpat_Con;_tmp219.f1=Cyc_Tcpat_char_con(c);_tmp219.f2=0;
_tmp219;});_tmp218;});}static void*Cyc_Tcpat_float_pat(struct _tagged_arr f){return(
void*)({struct Cyc_Tcpat_Con_struct*_tmp21A=_cycalloc(sizeof(struct Cyc_Tcpat_Con_struct));
_tmp21A[0]=({struct Cyc_Tcpat_Con_struct _tmp21B;_tmp21B.tag=Cyc_Tcpat_Con;_tmp21B.f1=
Cyc_Tcpat_float_con(f);_tmp21B.f2=0;_tmp21B;});_tmp21A;});}static void*Cyc_Tcpat_null_ptr_pat(
void*p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp21C=_cycalloc(sizeof(
struct Cyc_Tcpat_Con_struct));_tmp21C[0]=({struct Cyc_Tcpat_Con_struct _tmp21D;
_tmp21D.tag=Cyc_Tcpat_Con;_tmp21D.f1=Cyc_Tcpat_null_ptr_con;_tmp21D.f2=({struct
Cyc_List_List*_tmp21E=_cycalloc(sizeof(struct Cyc_List_List));_tmp21E->hd=(void*)
p;_tmp21E->tl=0;_tmp21E;});_tmp21D;});_tmp21C;});}static void*Cyc_Tcpat_ptr_pat(
void*p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp21F=_cycalloc(sizeof(
struct Cyc_Tcpat_Con_struct));_tmp21F[0]=({struct Cyc_Tcpat_Con_struct _tmp220;
_tmp220.tag=Cyc_Tcpat_Con;_tmp220.f1=Cyc_Tcpat_ptr_con;_tmp220.f2=({struct Cyc_List_List*
_tmp221=_cycalloc(sizeof(struct Cyc_List_List));_tmp221->hd=(void*)p;_tmp221->tl=
0;_tmp221;});_tmp220;});_tmp21F;});}static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*
ss){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp222=_cycalloc(sizeof(struct Cyc_Tcpat_Con_struct));
_tmp222[0]=({struct Cyc_Tcpat_Con_struct _tmp223;_tmp223.tag=Cyc_Tcpat_Con;_tmp223.f1=
Cyc_Tcpat_tuple_con(Cyc_List_length(ss));_tmp223.f2=ss;_tmp223;});_tmp222;});}
static void*Cyc_Tcpat_con_pat(struct _tagged_arr con_name,struct Cyc_Core_Opt*span,
struct Cyc_List_List*ps){struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp226=
_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));_tmp226->name=(void*)((void*)({struct
Cyc_Tcpat_Name_v_struct*_tmp227=_cycalloc(sizeof(struct Cyc_Tcpat_Name_v_struct));
_tmp227[0]=({struct Cyc_Tcpat_Name_v_struct _tmp228;_tmp228.tag=Cyc_Tcpat_Name_v;
_tmp228.f1=con_name;_tmp228;});_tmp227;}));_tmp226->arity=Cyc_List_length(ps);
_tmp226->span=span;_tmp226;});return(void*)({struct Cyc_Tcpat_Con_struct*_tmp224=
_cycalloc(sizeof(struct Cyc_Tcpat_Con_struct));_tmp224[0]=({struct Cyc_Tcpat_Con_struct
_tmp225;_tmp225.tag=Cyc_Tcpat_Con;_tmp225.f1=c;_tmp225.f2=ps;_tmp225;});_tmp224;});}
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){void*s;{void*_tmp229=(
void*)p->r;int _tmp24D;void*_tmp24F;unsigned char _tmp251;struct _tagged_arr _tmp253;
struct Cyc_Absyn_Pat*_tmp255;struct Cyc_List_List*_tmp257;struct Cyc_Absyn_Tunionfield*
_tmp259;struct Cyc_Absyn_Tuniondecl*_tmp25B;struct Cyc_List_List*_tmp25D;struct Cyc_List_List*
_tmp25F;struct Cyc_List_List*_tmp261;struct Cyc_Absyn_Structdecl*_tmp263;struct Cyc_Absyn_Enumfield*
_tmp265;struct Cyc_Absyn_Enumdecl*_tmp267;struct Cyc_Absyn_Enumfield*_tmp269;void*
_tmp26B;_LL555: if(_tmp229 == (void*)Cyc_Absyn_Wild_p){goto _LL556;}else{goto _LL557;}
_LL557: if((unsigned int)_tmp229 > 2?*((int*)_tmp229)== Cyc_Absyn_Var_p: 0){goto
_LL558;}else{goto _LL559;}_LL559: if(_tmp229 == (void*)Cyc_Absyn_Null_p){goto _LL560;}
else{goto _LL561;}_LL561: if((unsigned int)_tmp229 > 2?*((int*)_tmp229)== Cyc_Absyn_Int_p:
0){_LL592: _tmp24F=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp229)->f1;goto _LL590;
_LL590: _tmp24D=((struct Cyc_Absyn_Int_p_struct*)_tmp229)->f2;goto _LL562;}else{
goto _LL563;}_LL563: if((unsigned int)_tmp229 > 2?*((int*)_tmp229)== Cyc_Absyn_Char_p:
0){_LL594: _tmp251=((struct Cyc_Absyn_Char_p_struct*)_tmp229)->f1;goto _LL564;}
else{goto _LL565;}_LL565: if((unsigned int)_tmp229 > 2?*((int*)_tmp229)== Cyc_Absyn_Float_p:
0){_LL596: _tmp253=((struct Cyc_Absyn_Float_p_struct*)_tmp229)->f1;goto _LL566;}
else{goto _LL567;}_LL567: if((unsigned int)_tmp229 > 2?*((int*)_tmp229)== Cyc_Absyn_Reference_p:
0){goto _LL568;}else{goto _LL569;}_LL569: if((unsigned int)_tmp229 > 2?*((int*)
_tmp229)== Cyc_Absyn_Pointer_p: 0){_LL598: _tmp255=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp229)->f1;goto _LL570;}else{goto _LL571;}_LL571: if((unsigned int)_tmp229 > 2?*((
int*)_tmp229)== Cyc_Absyn_Tunion_p: 0){_LL604: _tmp25B=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp229)->f1;goto _LL602;_LL602: _tmp259=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp229)->f2;goto _LL600;_LL600: _tmp257=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp229)->f4;if(_tmp257 == 0){goto _LL572;}else{goto _LL573;}}else{goto _LL573;}
_LL573: if((unsigned int)_tmp229 > 2?*((int*)_tmp229)== Cyc_Absyn_Tunion_p: 0){
_LL606: _tmp25D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp229)->f4;goto _LL574;}else{
goto _LL575;}_LL575: if((unsigned int)_tmp229 > 2?*((int*)_tmp229)== Cyc_Absyn_Tuple_p:
0){_LL608: _tmp25F=((struct Cyc_Absyn_Tuple_p_struct*)_tmp229)->f1;goto _LL576;}
else{goto _LL577;}_LL577: if((unsigned int)_tmp229 > 2?*((int*)_tmp229)== Cyc_Absyn_Struct_p:
0){_LL612: _tmp263=((struct Cyc_Absyn_Struct_p_struct*)_tmp229)->f1;goto _LL610;
_LL610: _tmp261=((struct Cyc_Absyn_Struct_p_struct*)_tmp229)->f4;goto _LL578;}else{
goto _LL579;}_LL579: if((unsigned int)_tmp229 > 2?*((int*)_tmp229)== Cyc_Absyn_Enum_p:
0){_LL616: _tmp267=((struct Cyc_Absyn_Enum_p_struct*)_tmp229)->f1;goto _LL614;
_LL614: _tmp265=((struct Cyc_Absyn_Enum_p_struct*)_tmp229)->f2;goto _LL580;}else{
goto _LL581;}_LL581: if((unsigned int)_tmp229 > 2?*((int*)_tmp229)== Cyc_Absyn_AnonEnum_p:
0){_LL620: _tmp26B=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp229)->f1;goto
_LL618;_LL618: _tmp269=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp229)->f2;goto
_LL582;}else{goto _LL583;}_LL583: if((unsigned int)_tmp229 > 2?*((int*)_tmp229)== 
Cyc_Absyn_UnknownId_p: 0){goto _LL584;}else{goto _LL585;}_LL585: if((unsigned int)
_tmp229 > 2?*((int*)_tmp229)== Cyc_Absyn_UnknownCall_p: 0){goto _LL586;}else{goto
_LL587;}_LL587: if((unsigned int)_tmp229 > 2?*((int*)_tmp229)== Cyc_Absyn_UnknownFields_p:
0){goto _LL588;}else{goto _LL554;}_LL556: s=(void*)Cyc_Tcpat_Any;goto _LL554;_LL558:
s=(void*)Cyc_Tcpat_Any;goto _LL554;_LL560: s=Cyc_Tcpat_null_pat();goto _LL554;
_LL562: s=Cyc_Tcpat_int_pat(_tmp24D);goto _LL554;_LL564: s=Cyc_Tcpat_char_pat(
_tmp251);goto _LL554;_LL566: s=Cyc_Tcpat_float_pat(_tmp253);goto _LL554;_LL568: s=(
void*)Cyc_Tcpat_Any;goto _LL554;_LL570:{void*_tmp26D=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo _tmp275;
struct Cyc_Absyn_Conref*_tmp277;_LL623: if((unsigned int)_tmp26D > 3?*((int*)
_tmp26D)== Cyc_Absyn_PointerType: 0){_LL630: _tmp275=((struct Cyc_Absyn_PointerType_struct*)
_tmp26D)->f1;_LL632: _tmp277=_tmp275.nullable;goto _LL624;}else{goto _LL625;}_LL625:
if((unsigned int)_tmp26D > 3?*((int*)_tmp26D)== Cyc_Absyn_TunionType: 0){goto
_LL626;}else{goto _LL627;}_LL627: goto _LL628;_LL624: {int is_nullable=0;int
still_working=1;while(still_working){void*_tmp279=(void*)_tmp277->v;struct Cyc_Absyn_Conref*
_tmp281;int _tmp283;_LL635: if((unsigned int)_tmp279 > 1?*((int*)_tmp279)== Cyc_Absyn_Forward_constr:
0){_LL642: _tmp281=((struct Cyc_Absyn_Forward_constr_struct*)_tmp279)->f1;goto
_LL636;}else{goto _LL637;}_LL637: if(_tmp279 == (void*)Cyc_Absyn_No_constr){goto
_LL638;}else{goto _LL639;}_LL639: if((unsigned int)_tmp279 > 1?*((int*)_tmp279)== 
Cyc_Absyn_Eq_constr: 0){_LL644: _tmp283=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp279)->f1;goto _LL640;}else{goto _LL634;}_LL636:(void*)(_tmp277->v=(void*)((
void*)_tmp281->v));continue;_LL638:(void*)(_tmp277->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp285=_cycalloc(sizeof(struct Cyc_Absyn_Eq_constr_struct));_tmp285[0]=({struct
Cyc_Absyn_Eq_constr_struct _tmp286;_tmp286.tag=Cyc_Absyn_Eq_constr;_tmp286.f1=(
void*)0;_tmp286;});_tmp285;})));is_nullable=0;still_working=0;goto _LL634;_LL640:
is_nullable=(int)_tmp283;still_working=0;goto _LL634;_LL634:;}{void*ss=Cyc_Tcpat_compile_pat(
_tmp255);if(is_nullable){s=Cyc_Tcpat_null_ptr_pat(ss);}else{s=Cyc_Tcpat_ptr_pat(
ss);}goto _LL622;}}_LL626:{void*_tmp287=(void*)_tmp255->r;struct Cyc_List_List*
_tmp28D;struct Cyc_Absyn_Tunionfield*_tmp28F;struct Cyc_Absyn_Tuniondecl*_tmp291;
_LL649: if((unsigned int)_tmp287 > 2?*((int*)_tmp287)== Cyc_Absyn_Tunion_p: 0){
_LL658: _tmp291=((struct Cyc_Absyn_Tunion_p_struct*)_tmp287)->f1;goto _LL656;_LL656:
_tmp28F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp287)->f2;goto _LL654;_LL654:
_tmp28D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp287)->f4;goto _LL650;}else{goto
_LL651;}_LL651: goto _LL652;_LL650: {struct Cyc_Core_Opt*span;if(_tmp291->is_xtunion){
span=0;}else{span=({struct Cyc_Core_Opt*_tmp293=_cycalloc_atomic(sizeof(struct Cyc_Core_Opt));
_tmp293->v=(void*)((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp291->fields))->v);_tmp293;});}s=Cyc_Tcpat_con_pat(*(*
_tmp28F->name).f2,span,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp28D));goto _LL648;}
_LL652: s=({void*_tmp294[0]={};Cyc_Tcutil_impos(_tag_arr("non-[x]tunion pattern has tunion type",
sizeof(unsigned char),38),_tag_arr(_tmp294,sizeof(void*),0));});goto _LL648;
_LL648:;}goto _LL622;_LL628: s=({void*_tmp295[0]={};Cyc_Tcutil_impos(_tag_arr("pointer pattern does not have pointer type!",
sizeof(unsigned char),44),_tag_arr(_tmp295,sizeof(void*),0));});goto _LL622;
_LL622:;}goto _LL554;_LL572: {struct Cyc_Core_Opt*span;{void*_tmp296=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v);_LL664: if((unsigned int)
_tmp296 > 3?*((int*)_tmp296)== Cyc_Absyn_TunionType: 0){goto _LL665;}else{goto
_LL666;}_LL666: if((unsigned int)_tmp296 > 3?*((int*)_tmp296)== Cyc_Absyn_TunionFieldType:
0){goto _LL667;}else{goto _LL668;}_LL668: goto _LL669;_LL665: if(_tmp25B->is_xtunion){
span=0;}else{span=({struct Cyc_Core_Opt*_tmp29E=_cycalloc_atomic(sizeof(struct Cyc_Core_Opt));
_tmp29E->v=(void*)((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp25B->fields))->v);_tmp29E;});}goto _LL663;
_LL667: span=({struct Cyc_Core_Opt*_tmp29F=_cycalloc_atomic(sizeof(struct Cyc_Core_Opt));
_tmp29F->v=(void*)1;_tmp29F;});goto _LL663;_LL669: span=({void*_tmp2A0[0]={};((
struct Cyc_Core_Opt*(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("void tunion pattern has bad type",sizeof(unsigned char),33),_tag_arr(
_tmp2A0,sizeof(void*),0));});goto _LL663;_LL663:;}s=Cyc_Tcpat_con_pat(*(*_tmp259->name).f2,
span,0);goto _LL554;}_LL574: _tmp25F=_tmp25D;goto _LL576;_LL576: s=Cyc_Tcpat_tuple_pat(((
struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp25F));goto _LL554;_LL578: {struct Cyc_List_List*
ps=0;{struct Cyc_List_List*fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp263->fields))->v;for(0;fields != 0;fields=fields->tl){int found=
Cyc_Std_strcmp(*((struct Cyc_Absyn_Structfield*)fields->hd)->name,_tag_arr("",
sizeof(unsigned char),1))== 0;{struct Cyc_List_List*dlps0=_tmp261;for(0;! found?
dlps0 != 0: 0;dlps0=dlps0->tl){struct _tuple7 _tmp2A3;struct Cyc_Absyn_Pat*_tmp2A4;
struct Cyc_List_List*_tmp2A6;struct _tuple7*_tmp2A1=(struct _tuple7*)dlps0->hd;
_tmp2A3=*_tmp2A1;_LL679: _tmp2A6=_tmp2A3.f1;goto _LL677;_LL677: _tmp2A4=_tmp2A3.f2;
goto _LL674;_LL674: {struct Cyc_List_List*_tmp2A8=_tmp2A6;struct Cyc_List_List
_tmp2AE;struct Cyc_List_List*_tmp2AF;void*_tmp2B1;struct _tagged_arr*_tmp2B3;
_LL682: if(_tmp2A8 == 0){goto _LL684;}else{_tmp2AE=*_tmp2A8;_LL690: _tmp2B1=(void*)
_tmp2AE.hd;if(*((int*)_tmp2B1)== Cyc_Absyn_FieldName){_LL692: _tmp2B3=((struct Cyc_Absyn_FieldName_struct*)
_tmp2B1)->f1;goto _LL688;}else{goto _LL684;}_LL688: _tmp2AF=_tmp2AE.tl;if(_tmp2AF == 
0){goto _LL683;}else{goto _LL684;}}_LL684: goto _LL685;_LL683: if(Cyc_Std_zstrptrcmp(
_tmp2B3,((struct Cyc_Absyn_Structfield*)fields->hd)->name)== 0){ps=({struct Cyc_List_List*
_tmp2B5=_cycalloc(sizeof(struct Cyc_List_List));_tmp2B5->hd=(void*)Cyc_Tcpat_compile_pat(
_tmp2A4);_tmp2B5->tl=ps;_tmp2B5;});found=1;}goto _LL681;_LL685:({void*_tmp2B6[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad designator(s)",
sizeof(unsigned char),18),_tag_arr(_tmp2B6,sizeof(void*),0));});goto _LL681;
_LL681:;}}}if(! found){({void*_tmp2B7[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad designator",sizeof(unsigned char),
15),_tag_arr(_tmp2B7,sizeof(void*),0));});}}}s=Cyc_Tcpat_tuple_pat(ps);goto
_LL554;}_LL580: {int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp267->fields))->v);s=
Cyc_Tcpat_con_pat(*(*_tmp265->name).f2,({struct Cyc_Core_Opt*_tmp2B8=
_cycalloc_atomic(sizeof(struct Cyc_Core_Opt));_tmp2B8->v=(void*)span;_tmp2B8;}),0);
goto _LL554;}_LL582: {struct Cyc_List_List*fields;{void*_tmp2B9=Cyc_Tcutil_compress(
_tmp26B);struct Cyc_List_List*_tmp2BF;_LL699: if((unsigned int)_tmp2B9 > 3?*((int*)
_tmp2B9)== Cyc_Absyn_AnonEnumType: 0){_LL704: _tmp2BF=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp2B9)->f1;goto _LL700;}else{goto _LL701;}_LL701: goto _LL702;_LL700: fields=
_tmp2BF;goto _LL698;_LL702: fields=({void*_tmp2C1[0]={};((struct Cyc_List_List*(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad type in AnonEnum_p",
sizeof(unsigned char),23),_tag_arr(_tmp2C1,sizeof(void*),0));});goto _LL698;
_LL698:;}{int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);s=Cyc_Tcpat_con_pat(*(*
_tmp269->name).f2,({struct Cyc_Core_Opt*_tmp2C2=_cycalloc_atomic(sizeof(struct Cyc_Core_Opt));
_tmp2C2->v=(void*)span;_tmp2C2;}),0);goto _LL554;}}_LL584: s=({void*_tmp2C3[0]={};
Cyc_Tcutil_impos(_tag_arr("compile_pat: unknownid",sizeof(unsigned char),23),
_tag_arr(_tmp2C3,sizeof(void*),0));});goto _LL554;_LL586: s=({void*_tmp2C4[0]={};
Cyc_Tcutil_impos(_tag_arr("compile_pat: unknowncall",sizeof(unsigned char),25),
_tag_arr(_tmp2C4,sizeof(void*),0));});goto _LL554;_LL588: s=({void*_tmp2C5[0]={};
Cyc_Tcutil_impos(_tag_arr("compile_pat: unknownfields",sizeof(unsigned char),27),
_tag_arr(_tmp2C5,sizeof(void*),0));});goto _LL554;_LL554:;}return s;}static const
int Cyc_Tcpat_Pos=0;struct Cyc_Tcpat_Pos_struct{int tag;struct Cyc_Tcpat_Con_s*f1;
struct Cyc_List_List*f2;};static const int Cyc_Tcpat_Neg=1;struct Cyc_Tcpat_Neg_struct{
int tag;struct Cyc_Set_Set*f1;};static const int Cyc_Tcpat_Failure=0;static const int
Cyc_Tcpat_Success=0;struct Cyc_Tcpat_Success_struct{int tag;void*f1;};static const
int Cyc_Tcpat_IfEq=1;struct Cyc_Tcpat_IfEq_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct _tuple10{struct Cyc_List_List*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};static const int Cyc_Tcpat_Yes=0;
static const int Cyc_Tcpat_No=1;static const int Cyc_Tcpat_Maybe=2;static void*Cyc_Tcpat_add_neg(
void*td,struct Cyc_Tcpat_Con_s*c){void*_tmp2C6=td;struct Cyc_Set_Set*_tmp2CC;
_LL712: if(*((int*)_tmp2C6)== Cyc_Tcpat_Neg){_LL717: _tmp2CC=((struct Cyc_Tcpat_Neg_struct*)
_tmp2C6)->f1;goto _LL713;}else{goto _LL714;}_LL714: if(*((int*)_tmp2C6)== Cyc_Tcpat_Pos){
goto _LL715;}else{goto _LL711;}_LL713: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp2CC,c)){({void*_tmp2CE[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("add_neg called when constructor already in set",
sizeof(unsigned char),47),_tag_arr(_tmp2CE,sizeof(void*),0));});}if(c->span != 0?((
int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp2CC)+ 1 >= (int)((struct Cyc_Core_Opt*)
_check_null(c->span))->v: 0){({void*_tmp2CF[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("add_neg called when |cs U {c}| >= span(c)",
sizeof(unsigned char),42),_tag_arr(_tmp2CF,sizeof(void*),0));});}return(void*)({
struct Cyc_Tcpat_Neg_struct*_tmp2D0=_cycalloc(sizeof(struct Cyc_Tcpat_Neg_struct));
_tmp2D0[0]=({struct Cyc_Tcpat_Neg_struct _tmp2D1;_tmp2D1.tag=Cyc_Tcpat_Neg;_tmp2D1.f1=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(
_tmp2CC,c);_tmp2D1;});_tmp2D0;});_LL715: return({void*_tmp2D2[0]={};Cyc_Tcutil_impos(
_tag_arr("add_neg called when td is Positive",sizeof(unsigned char),35),_tag_arr(
_tmp2D2,sizeof(void*),0));});_LL711:;}static void*Cyc_Tcpat_static_match(struct
Cyc_Tcpat_Con_s*c,void*td){void*_tmp2D3=td;struct Cyc_Tcpat_Con_s*_tmp2D9;struct
Cyc_Set_Set*_tmp2DB;_LL725: if(*((int*)_tmp2D3)== Cyc_Tcpat_Pos){_LL730: _tmp2D9=((
struct Cyc_Tcpat_Pos_struct*)_tmp2D3)->f1;goto _LL726;}else{goto _LL727;}_LL727: if(*((
int*)_tmp2D3)== Cyc_Tcpat_Neg){_LL732: _tmp2DB=((struct Cyc_Tcpat_Neg_struct*)
_tmp2D3)->f1;goto _LL728;}else{goto _LL724;}_LL726: if(Cyc_Tcpat_compare_con(c,
_tmp2D9)== 0){return(void*)Cyc_Tcpat_Yes;}else{return(void*)Cyc_Tcpat_No;}_LL728:
if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(
_tmp2DB,c)){return(void*)Cyc_Tcpat_No;}else{if(c->span != 0?(int)((struct Cyc_Core_Opt*)
_check_null(c->span))->v == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp2DB)+ 1: 0){return(void*)Cyc_Tcpat_Yes;}else{return(void*)Cyc_Tcpat_Maybe;}}
_LL724:;}struct _tuple11{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};static
struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){struct
Cyc_List_List*_tmp2DD=ctxt;struct Cyc_List_List _tmp2E3;struct Cyc_List_List*
_tmp2E4;struct _tuple11*_tmp2E6;struct _tuple11 _tmp2E8;struct Cyc_List_List*_tmp2E9;
struct Cyc_Tcpat_Con_s*_tmp2EB;_LL735: if(_tmp2DD == 0){goto _LL736;}else{goto _LL737;}
_LL737: if(_tmp2DD == 0){goto _LL734;}else{_tmp2E3=*_tmp2DD;_LL743: _tmp2E6=(struct
_tuple11*)_tmp2E3.hd;_tmp2E8=*_tmp2E6;_LL748: _tmp2EB=_tmp2E8.f1;goto _LL746;
_LL746: _tmp2E9=_tmp2E8.f2;goto _LL741;_LL741: _tmp2E4=_tmp2E3.tl;goto _LL738;}
_LL736: return 0;_LL738: return({struct Cyc_List_List*_tmp2ED=_cycalloc(sizeof(
struct Cyc_List_List));_tmp2ED->hd=({struct _tuple11*_tmp2EE=_cycalloc(sizeof(
struct _tuple11));_tmp2EE->f1=_tmp2EB;_tmp2EE->f2=(struct Cyc_List_List*)({struct
Cyc_List_List*_tmp2EF=_cycalloc(sizeof(struct Cyc_List_List));_tmp2EF->hd=(void*)
dsc;_tmp2EF->tl=_tmp2E9;_tmp2EF;});_tmp2EE;});_tmp2ED->tl=_tmp2E4;_tmp2ED;});
_LL734:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*
ctxt){struct Cyc_List_List*_tmp2F0=ctxt;struct Cyc_List_List _tmp2F6;struct Cyc_List_List*
_tmp2F7;struct _tuple11*_tmp2F9;struct _tuple11 _tmp2FB;struct Cyc_List_List*_tmp2FC;
struct Cyc_Tcpat_Con_s*_tmp2FE;_LL754: if(_tmp2F0 == 0){goto _LL755;}else{goto _LL756;}
_LL756: if(_tmp2F0 == 0){goto _LL753;}else{_tmp2F6=*_tmp2F0;_LL762: _tmp2F9=(struct
_tuple11*)_tmp2F6.hd;_tmp2FB=*_tmp2F9;_LL767: _tmp2FE=_tmp2FB.f1;goto _LL765;
_LL765: _tmp2FC=_tmp2FB.f2;goto _LL760;_LL760: _tmp2F7=_tmp2F6.tl;goto _LL757;}
_LL755: return({void*_tmp300[0]={};((struct Cyc_List_List*(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("norm_context: empty context",
sizeof(unsigned char),28),_tag_arr(_tmp300,sizeof(void*),0));});_LL757: return Cyc_Tcpat_augment(
_tmp2F7,(void*)({struct Cyc_Tcpat_Pos_struct*_tmp301=_cycalloc(sizeof(struct Cyc_Tcpat_Pos_struct));
_tmp301[0]=({struct Cyc_Tcpat_Pos_struct _tmp302;_tmp302.tag=Cyc_Tcpat_Pos;_tmp302.f1=
_tmp2FE;_tmp302.f2=Cyc_List_rev(_tmp2FC);_tmp302;});_tmp301;}));_LL753:;}static
void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*
work){struct _tuple6 _tmp304=({struct _tuple6 _tmp303;_tmp303.f1=ctxt;_tmp303.f2=
work;_tmp303;});struct Cyc_List_List*_tmp30E;struct Cyc_List_List*_tmp310;struct
Cyc_List_List*_tmp312;struct Cyc_List_List*_tmp314;struct Cyc_List_List*_tmp316;
struct Cyc_List_List _tmp318;struct Cyc_List_List*_tmp319;struct _tuple10*_tmp31B;
struct _tuple10 _tmp31D;struct Cyc_List_List*_tmp31E;struct Cyc_List_List*_tmp320;
struct Cyc_List_List _tmp322;struct Cyc_List_List*_tmp323;struct _tuple11*_tmp325;
struct _tuple11 _tmp327;struct Cyc_List_List*_tmp328;struct Cyc_Tcpat_Con_s*_tmp32A;
_LL774: _LL785: _tmp310=_tmp304.f1;if(_tmp310 == 0){goto _LL783;}else{goto _LL776;}
_LL783: _tmp30E=_tmp304.f2;if(_tmp30E == 0){goto _LL775;}else{goto _LL776;}_LL776:
_LL787: _tmp312=_tmp304.f1;if(_tmp312 == 0){goto _LL777;}else{goto _LL778;}_LL778:
_LL789: _tmp314=_tmp304.f2;if(_tmp314 == 0){goto _LL779;}else{goto _LL780;}_LL780:
_LL801: _tmp320=_tmp304.f1;if(_tmp320 == 0){goto _LL773;}else{_tmp322=*_tmp320;
_LL806: _tmp325=(struct _tuple11*)_tmp322.hd;_tmp327=*_tmp325;_LL811: _tmp32A=
_tmp327.f1;goto _LL809;_LL809: _tmp328=_tmp327.f2;goto _LL804;_LL804: _tmp323=
_tmp322.tl;goto _LL791;}_LL791: _tmp316=_tmp304.f2;if(_tmp316 == 0){goto _LL773;}
else{_tmp318=*_tmp316;_LL796: _tmp31B=(struct _tuple10*)_tmp318.hd;_tmp31D=*
_tmp31B;_LL799: _tmp31E=_tmp31D.f3;goto _LL794;_LL794: _tmp319=_tmp318.tl;goto
_LL781;}_LL775: return dsc;_LL777: return({void*_tmp32C[0]={};Cyc_Tcutil_impos(
_tag_arr("build_desc: ctxt and work don't match",sizeof(unsigned char),38),
_tag_arr(_tmp32C,sizeof(void*),0));});_LL779: return({void*_tmp32D[0]={};Cyc_Tcutil_impos(
_tag_arr("build_desc: ctxt and work don't match",sizeof(unsigned char),38),
_tag_arr(_tmp32D,sizeof(void*),0));});_LL781: {struct Cyc_Tcpat_Pos_struct*
_tmp32E=({struct Cyc_Tcpat_Pos_struct*_tmp32F=_cycalloc(sizeof(struct Cyc_Tcpat_Pos_struct));
_tmp32F[0]=({struct Cyc_Tcpat_Pos_struct _tmp330;_tmp330.tag=Cyc_Tcpat_Pos;_tmp330.f1=
_tmp32A;_tmp330.f2=Cyc_List_append(Cyc_List_rev(_tmp328),({struct Cyc_List_List*
_tmp331=_cycalloc(sizeof(struct Cyc_List_List));_tmp331->hd=(void*)dsc;_tmp331->tl=
_tmp31E;_tmp331;}));_tmp330;});_tmp32F;});return Cyc_Tcpat_build_desc(_tmp323,(
void*)_tmp32E,_tmp319);}_LL773:;}static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*
obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules);static void*Cyc_Tcpat_or_match(void*
dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*_tmp332=allmrules;struct
Cyc_List_List _tmp338;struct Cyc_List_List*_tmp339;struct _tuple0*_tmp33B;struct
_tuple0 _tmp33D;void*_tmp33E;void*_tmp340;_LL820: if(_tmp332 == 0){goto _LL821;}
else{goto _LL822;}_LL822: if(_tmp332 == 0){goto _LL819;}else{_tmp338=*_tmp332;_LL828:
_tmp33B=(struct _tuple0*)_tmp338.hd;_tmp33D=*_tmp33B;_LL833: _tmp340=_tmp33D.f1;
goto _LL831;_LL831: _tmp33E=_tmp33D.f2;goto _LL826;_LL826: _tmp339=_tmp338.tl;goto
_LL823;}_LL821: return(void*)Cyc_Tcpat_Failure;_LL823: return Cyc_Tcpat_match(
_tmp340,0,dsc,0,0,_tmp33E,_tmp339);_LL819:;}static void*Cyc_Tcpat_match_compile(
struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match((void*)({struct Cyc_Tcpat_Neg_struct*
_tmp342=_cycalloc(sizeof(struct Cyc_Tcpat_Neg_struct));_tmp342[0]=({struct Cyc_Tcpat_Neg_struct
_tmp343;_tmp343.tag=Cyc_Tcpat_Neg;_tmp343.f1=Cyc_Tcpat_empty_con_set();_tmp343;});
_tmp342;}),allmrules);}static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,
struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){struct
Cyc_List_List*_tmp344=work;struct Cyc_List_List _tmp34C;struct Cyc_List_List*
_tmp34D;struct _tuple10*_tmp34F;struct _tuple10 _tmp351;struct Cyc_List_List*_tmp352;
struct Cyc_List_List*_tmp354;struct Cyc_List_List*_tmp356;struct Cyc_List_List
_tmp358;struct Cyc_List_List*_tmp359;struct _tuple10*_tmp35B;struct _tuple10 _tmp35D;
struct Cyc_List_List*_tmp35E;struct Cyc_List_List*_tmp360;struct Cyc_List_List*
_tmp362;_LL838: if(_tmp344 == 0){goto _LL839;}else{goto _LL840;}_LL840: if(_tmp344 == 
0){goto _LL842;}else{_tmp34C=*_tmp344;_LL848: _tmp34F=(struct _tuple10*)_tmp34C.hd;
_tmp351=*_tmp34F;_LL855: _tmp356=_tmp351.f1;if(_tmp356 == 0){goto _LL853;}else{goto
_LL842;}_LL853: _tmp354=_tmp351.f2;if(_tmp354 == 0){goto _LL851;}else{goto _LL842;}
_LL851: _tmp352=_tmp351.f3;if(_tmp352 == 0){goto _LL846;}else{goto _LL842;}_LL846:
_tmp34D=_tmp34C.tl;goto _LL841;}_LL842: if(_tmp344 == 0){goto _LL837;}else{_tmp358=*
_tmp344;_LL860: _tmp35B=(struct _tuple10*)_tmp358.hd;_tmp35D=*_tmp35B;_LL867:
_tmp362=_tmp35D.f1;goto _LL865;_LL865: _tmp360=_tmp35D.f2;goto _LL863;_LL863:
_tmp35E=_tmp35D.f3;goto _LL858;_LL858: _tmp359=_tmp358.tl;goto _LL843;}_LL839:
return(void*)({struct Cyc_Tcpat_Success_struct*_tmp364=_cycalloc(sizeof(struct Cyc_Tcpat_Success_struct));
_tmp364[0]=({struct Cyc_Tcpat_Success_struct _tmp365;_tmp365.tag=Cyc_Tcpat_Success;
_tmp365.f1=(void*)right_hand_side;_tmp365;});_tmp364;});_LL841: return Cyc_Tcpat_and_match(
Cyc_Tcpat_norm_context(ctx),_tmp34D,right_hand_side,rules);_LL843: if((_tmp362 == 
0? 1: _tmp360 == 0)? 1: _tmp35E == 0){return({void*_tmp366[0]={};Cyc_Tcutil_impos(
_tag_arr("tcpat:and_match: malformed work frame",sizeof(unsigned char),38),
_tag_arr(_tmp366,sizeof(void*),0));});}{struct Cyc_List_List _tmp369;struct Cyc_List_List*
_tmp36A;void*_tmp36C;struct Cyc_List_List*_tmp367=(struct Cyc_List_List*)
_check_null(_tmp362);_tmp369=*_tmp367;_LL877: _tmp36C=(void*)_tmp369.hd;goto
_LL875;_LL875: _tmp36A=_tmp369.tl;goto _LL872;_LL872: {struct Cyc_List_List _tmp370;
struct Cyc_List_List*_tmp371;struct Cyc_List_List*_tmp373;struct Cyc_List_List*
_tmp36E=(struct Cyc_List_List*)_check_null(_tmp360);_tmp370=*_tmp36E;_LL884:
_tmp373=(struct Cyc_List_List*)_tmp370.hd;goto _LL882;_LL882: _tmp371=_tmp370.tl;
goto _LL879;_LL879: {struct Cyc_List_List _tmp377;struct Cyc_List_List*_tmp378;void*
_tmp37A;struct Cyc_List_List*_tmp375=(struct Cyc_List_List*)_check_null(_tmp35E);
_tmp377=*_tmp375;_LL891: _tmp37A=(void*)_tmp377.hd;goto _LL889;_LL889: _tmp378=
_tmp377.tl;goto _LL886;_LL886: {struct _tuple10*_tmp37C=({struct _tuple10*_tmp37E=
_cycalloc(sizeof(struct _tuple10));_tmp37E->f1=_tmp36A;_tmp37E->f2=_tmp371;
_tmp37E->f3=_tmp378;_tmp37E;});return Cyc_Tcpat_match(_tmp36C,_tmp373,_tmp37A,ctx,({
struct Cyc_List_List*_tmp37D=_cycalloc(sizeof(struct Cyc_List_List));_tmp37D->hd=
_tmp37C;_tmp37D->tl=_tmp359;_tmp37D;}),right_hand_side,rules);}}}}_LL837:;}
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp37F=dsc;struct Cyc_Set_Set*_tmp385;struct Cyc_List_List*_tmp387;_LL897:
if(*((int*)_tmp37F)== Cyc_Tcpat_Neg){_LL902: _tmp385=((struct Cyc_Tcpat_Neg_struct*)
_tmp37F)->f1;goto _LL898;}else{goto _LL899;}_LL899: if(*((int*)_tmp37F)== Cyc_Tcpat_Pos){
_LL904: _tmp387=((struct Cyc_Tcpat_Pos_struct*)_tmp37F)->f2;goto _LL900;}else{goto
_LL896;}_LL898: {void*any=(void*)({struct Cyc_Tcpat_Neg_struct*_tmp38B=_cycalloc(
sizeof(struct Cyc_Tcpat_Neg_struct));_tmp38B[0]=({struct Cyc_Tcpat_Neg_struct
_tmp38C;_tmp38C.tag=Cyc_Tcpat_Neg;_tmp38C.f1=Cyc_Tcpat_empty_con_set();_tmp38C;});
_tmp38B;});struct Cyc_List_List*_tmp389=0;{int i=0;for(0;i < pcon->arity;++ i){
_tmp389=({struct Cyc_List_List*_tmp38A=_cycalloc(sizeof(struct Cyc_List_List));
_tmp38A->hd=(void*)any;_tmp38A->tl=_tmp389;_tmp38A;});}}return _tmp389;}_LL900:
return _tmp387;_LL896:;}static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct Cyc_List_List*
obj,int i){return({struct Cyc_List_List*_tmp38D=_cycalloc(sizeof(struct Cyc_List_List));
_tmp38D->hd=(void*)(i + 1);_tmp38D->tl=obj;_tmp38D;});}static struct Cyc_List_List*
Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){return((
struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*f)(struct Cyc_List_List*,int),
struct Cyc_List_List*env))Cyc_List_tabulate_c)(pcon->arity,Cyc_Tcpat_getoarg,obj);}
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*
ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
void*_tmp38E=p;struct Cyc_List_List*_tmp394;struct Cyc_Tcpat_Con_s*_tmp396;_LL912:
if(_tmp38E == (void*)Cyc_Tcpat_Any){goto _LL913;}else{goto _LL914;}_LL914: if((
unsigned int)_tmp38E > 1?*((int*)_tmp38E)== Cyc_Tcpat_Con: 0){_LL919: _tmp396=((
struct Cyc_Tcpat_Con_struct*)_tmp38E)->f1;goto _LL917;_LL917: _tmp394=((struct Cyc_Tcpat_Con_struct*)
_tmp38E)->f2;goto _LL915;}else{goto _LL911;}_LL913: return Cyc_Tcpat_and_match(Cyc_Tcpat_augment(
ctx,dsc),work,right_hand_side,rules);_LL915: {void*_tmp398=Cyc_Tcpat_static_match(
_tmp396,dsc);_LL922: if(_tmp398 == (void*)Cyc_Tcpat_Yes){goto _LL923;}else{goto
_LL924;}_LL924: if(_tmp398 == (void*)Cyc_Tcpat_No){goto _LL925;}else{goto _LL926;}
_LL926: if(_tmp398 == (void*)Cyc_Tcpat_Maybe){goto _LL927;}else{goto _LL921;}_LL923: {
struct Cyc_List_List*_tmp3A0=({struct Cyc_List_List*_tmp3A5=_cycalloc(sizeof(
struct Cyc_List_List));_tmp3A5->hd=({struct _tuple11*_tmp3A6=_cycalloc(sizeof(
struct _tuple11));_tmp3A6->f1=_tmp396;_tmp3A6->f2=0;_tmp3A6;});_tmp3A5->tl=ctx;
_tmp3A5;});struct _tuple10*_tmp3A1=({struct _tuple10*_tmp3A4=_cycalloc(sizeof(
struct _tuple10));_tmp3A4->f1=_tmp394;_tmp3A4->f2=Cyc_Tcpat_getoargs(_tmp396,obj);
_tmp3A4->f3=Cyc_Tcpat_getdargs(_tmp396,dsc);_tmp3A4;});struct Cyc_List_List*
_tmp3A2=({struct Cyc_List_List*_tmp3A3=_cycalloc(sizeof(struct Cyc_List_List));
_tmp3A3->hd=_tmp3A1;_tmp3A3->tl=work;_tmp3A3;});return Cyc_Tcpat_and_match(
_tmp3A0,_tmp3A2,right_hand_side,rules);}_LL925: return Cyc_Tcpat_or_match(Cyc_Tcpat_build_desc(
ctx,dsc,work),rules);_LL927: {struct Cyc_List_List*_tmp3A7=({struct Cyc_List_List*
_tmp3B0=_cycalloc(sizeof(struct Cyc_List_List));_tmp3B0->hd=({struct _tuple11*
_tmp3B1=_cycalloc(sizeof(struct _tuple11));_tmp3B1->f1=_tmp396;_tmp3B1->f2=0;
_tmp3B1;});_tmp3B0->tl=ctx;_tmp3B0;});struct _tuple10*_tmp3A8=({struct _tuple10*
_tmp3AF=_cycalloc(sizeof(struct _tuple10));_tmp3AF->f1=_tmp394;_tmp3AF->f2=Cyc_Tcpat_getoargs(
_tmp396,obj);_tmp3AF->f3=Cyc_Tcpat_getdargs(_tmp396,dsc);_tmp3AF;});struct Cyc_List_List*
_tmp3A9=({struct Cyc_List_List*_tmp3AE=_cycalloc(sizeof(struct Cyc_List_List));
_tmp3AE->hd=_tmp3A8;_tmp3AE->tl=work;_tmp3AE;});void*_tmp3AA=Cyc_Tcpat_and_match(
_tmp3A7,_tmp3A9,right_hand_side,rules);void*_tmp3AB=Cyc_Tcpat_or_match(Cyc_Tcpat_build_desc(
ctx,Cyc_Tcpat_add_neg(dsc,_tmp396),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*
_tmp3AC=_cycalloc(sizeof(struct Cyc_Tcpat_IfEq_struct));_tmp3AC[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp3AD;_tmp3AD.tag=Cyc_Tcpat_IfEq;_tmp3AD.f1=obj;_tmp3AD.f2=_tmp396;_tmp3AD.f3=(
void*)_tmp3AA;_tmp3AD.f4=(void*)_tmp3AB;_tmp3AD;});_tmp3AC;});}_LL921:;}_LL911:;}
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*),void*
env1,void(*rhs_appears)(void*,void*),void*env2){void*_tmp3B2=d;void*_tmp3BA;void*
_tmp3BC;void*_tmp3BE;_LL948: if(_tmp3B2 == (void*)Cyc_Tcpat_Failure){goto _LL949;}
else{goto _LL950;}_LL950: if((unsigned int)_tmp3B2 > 1?*((int*)_tmp3B2)== Cyc_Tcpat_Success:
0){_LL955: _tmp3BA=(void*)((struct Cyc_Tcpat_Success_struct*)_tmp3B2)->f1;goto
_LL951;}else{goto _LL952;}_LL952: if((unsigned int)_tmp3B2 > 1?*((int*)_tmp3B2)== 
Cyc_Tcpat_IfEq: 0){_LL959: _tmp3BE=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp3B2)->f3;
goto _LL957;_LL957: _tmp3BC=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp3B2)->f4;
goto _LL953;}else{goto _LL947;}_LL949: not_exhaust(env1);goto _LL947;_LL951:
rhs_appears(env2,_tmp3BA);goto _LL947;_LL953: Cyc_Tcpat_check_exhaust_overlap(
_tmp3BE,not_exhaust,env1,rhs_appears,env2);Cyc_Tcpat_check_exhaust_overlap(
_tmp3BC,not_exhaust,env1,rhs_appears,env2);goto _LL947;_LL947:;}struct _tuple12{
int f1;struct Cyc_Position_Segment*f2;};struct _tuple13{void*f1;struct _tuple12*f2;}
;static struct _tuple13*Cyc_Tcpat_get_match(int*r,struct Cyc_Absyn_Switch_clause*
swc){void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);struct _tuple12*rhs=({struct
_tuple12*_tmp3C5=_cycalloc(sizeof(struct _tuple12));_tmp3C5->f1=0;_tmp3C5->f2=(
swc->pattern)->loc;_tmp3C5;});void*sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(({
struct Cyc_List_List*_tmp3C0=_cycalloc(sizeof(struct Cyc_List_List));_tmp3C0->hd=(
void*)sp0;_tmp3C0->tl=({struct Cyc_List_List*_tmp3C1=_cycalloc(sizeof(struct Cyc_List_List));
_tmp3C1->hd=(void*)Cyc_Tcpat_int_pat(*r);_tmp3C1->tl=0;_tmp3C1;});_tmp3C0;}));*r=*
r + 1;}else{sp=Cyc_Tcpat_tuple_pat(({struct Cyc_List_List*_tmp3C2=_cycalloc(
sizeof(struct Cyc_List_List));_tmp3C2->hd=(void*)sp0;_tmp3C2->tl=({struct Cyc_List_List*
_tmp3C3=_cycalloc(sizeof(struct Cyc_List_List));_tmp3C3->hd=(void*)((void*)Cyc_Tcpat_Any);
_tmp3C3->tl=0;_tmp3C3;});_tmp3C2;}));}return({struct _tuple13*_tmp3C4=_cycalloc(
sizeof(struct _tuple13));_tmp3C4->f1=sp;_tmp3C4->f2=rhs;_tmp3C4;});}static void Cyc_Tcpat_not_exhaust_err(
struct Cyc_Position_Segment*loc){({void*_tmp3C6[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("patterns may not be exhaustive",sizeof(unsigned char),31),_tag_arr(
_tmp3C6,sizeof(void*),0));});}static void Cyc_Tcpat_rule_occurs(int dummy,struct
_tuple12*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*swcs){int _tmp3C7=0;struct Cyc_List_List*_tmp3C8=((struct
Cyc_List_List*(*)(struct _tuple13*(*f)(int*,struct Cyc_Absyn_Switch_clause*),int*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcpat_get_match,& _tmp3C7,swcs);
void*_tmp3C9=((void*(*)(struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(
_tmp3C8);((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*),struct
Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple12*),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp3C9,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0);for(0;_tmp3C8 != 0;
_tmp3C8=_tmp3C8->tl){struct _tuple13 _tmp3CC;struct _tuple12*_tmp3CD;struct _tuple12
_tmp3CF;struct Cyc_Position_Segment*_tmp3D0;int _tmp3D2;struct _tuple13*_tmp3CA=(
struct _tuple13*)_tmp3C8->hd;_tmp3CC=*_tmp3CA;_LL974: _tmp3CD=_tmp3CC.f2;_tmp3CF=*
_tmp3CD;_LL979: _tmp3D2=_tmp3CF.f1;goto _LL977;_LL977: _tmp3D0=_tmp3CF.f2;goto
_LL971;_LL971: if(! _tmp3D2){({void*_tmp3D4[0]={};Cyc_Tcutil_terr(_tmp3D0,_tag_arr("redundant pattern",
sizeof(unsigned char),18),_tag_arr(_tmp3D4,sizeof(void*),0));});}}}struct
_tuple14{struct Cyc_Position_Segment*f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(
struct _tuple14*pr){(*pr).f2=0;({void*_tmp3D5[0]={};Cyc_Tcutil_warn((*pr).f1,
_tag_arr("pattern not exhaustive",sizeof(unsigned char),23),_tag_arr(_tmp3D5,
sizeof(void*),0));});}static void Cyc_Tcpat_dummy_fn(int i,int j){return;}struct
_tuple15{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Pat*p){struct Cyc_List_List*_tmp3D6=({struct Cyc_List_List*
_tmp3DA=_cycalloc(sizeof(struct Cyc_List_List));_tmp3DA->hd=({struct _tuple15*
_tmp3DB=_cycalloc(sizeof(struct _tuple15));_tmp3DB->f1=Cyc_Tcpat_compile_pat(p);
_tmp3DB->f2=0;_tmp3DB;});_tmp3DA->tl=0;_tmp3DA;});void*_tmp3D7=((void*(*)(struct
Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(_tmp3D6);struct _tuple14*
_tmp3D8=({struct _tuple14*_tmp3D9=_cycalloc(sizeof(struct _tuple14));_tmp3D9->f1=
loc;_tmp3D9->f2=1;_tmp3D9;});((void(*)(void*d,void(*not_exhaust)(struct _tuple14*),
struct _tuple14*env1,void(*rhs_appears)(int,int),int env2))Cyc_Tcpat_check_exhaust_overlap)(
_tmp3D7,Cyc_Tcpat_not_exhaust_warn,_tmp3D8,Cyc_Tcpat_dummy_fn,0);return(*_tmp3D8).f2;}
static struct _tuple13*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);struct _tuple12*rhs=({struct _tuple12*
_tmp3DD=_cycalloc(sizeof(struct _tuple12));_tmp3DD->f1=0;_tmp3DD->f2=(swc->pattern)->loc;
_tmp3DD;});return({struct _tuple13*_tmp3DC=_cycalloc(sizeof(struct _tuple13));
_tmp3DC->f1=sp0;_tmp3DC->f2=rhs;_tmp3DC;});}static void Cyc_Tcpat_not_exhaust_err2(
struct Cyc_Position_Segment*loc){;}void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*swcs){struct Cyc_List_List*_tmp3DE=((struct Cyc_List_List*(*)(
struct _tuple13*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcpat_get_match2,swcs);void*_tmp3DF=((void*(*)(struct Cyc_List_List*allmrules))
Cyc_Tcpat_match_compile)(_tmp3DE);((void(*)(void*d,void(*not_exhaust)(struct Cyc_Position_Segment*),
struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple12*),int env2))
Cyc_Tcpat_check_exhaust_overlap)(_tmp3DF,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,
0);for(0;_tmp3DE != 0;_tmp3DE=_tmp3DE->tl){struct _tuple13 _tmp3E2;struct _tuple12*
_tmp3E3;struct _tuple12 _tmp3E5;struct Cyc_Position_Segment*_tmp3E6;int _tmp3E8;
struct _tuple13*_tmp3E0=(struct _tuple13*)_tmp3DE->hd;_tmp3E2=*_tmp3E0;_LL996:
_tmp3E3=_tmp3E2.f2;_tmp3E5=*_tmp3E3;_LL1001: _tmp3E8=_tmp3E5.f1;goto _LL999;_LL999:
_tmp3E6=_tmp3E5.f2;goto _LL993;_LL993: if(! _tmp3E8){({void*_tmp3EA[0]={};Cyc_Tcutil_terr(
_tmp3E6,_tag_arr("redundant pattern",sizeof(unsigned char),18),_tag_arr(_tmp3EA,
sizeof(void*),0));});}}}
