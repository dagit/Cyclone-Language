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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern void*Cyc_List_hd(struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_tl(
struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[18];extern
unsigned char Cyc_List_Nth[8];extern int Cyc_List_mem(int(*compare)(void*,void*),
struct Cyc_List_List*l,void*x);extern int Cyc_List_list_cmp(int(*cmp)(void*,void*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Set_Set;extern struct Cyc_Set_Set*
Cyc_Set_empty(int(*cmp)(void*,void*));extern struct Cyc_Set_Set*Cyc_Set_insert(
struct Cyc_Set_Set*s,void*elt);extern int Cyc_Set_member(struct Cyc_Set_Set*s,void*
elt);extern struct Cyc_List_List*Cyc_Set_elements(struct Cyc_Set_Set*s);extern
unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(
int(*cmp)(void*,void*));extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*
d,void*k,void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);extern
void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*
d,void*accum);extern void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,
struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_map_c(void*(*f)(void*,
void*),void*env,struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_union_two_c(
void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2);extern struct Cyc_Dict_Dict*Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,
void*),void*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);extern int Cyc_Dict_forall_c(
int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);extern int Cyc_Dict_forall_intersect(
int(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2);extern
int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;static const int Cyc_Position_Lex=
0;static const int Cyc_Position_Parse=1;static const int Cyc_Position_Elab=2;struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];static const int
Cyc_Absyn_Loc_n=0;static const int Cyc_Absyn_Rel_n=0;struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_Abs_n=1;struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};
struct Cyc_Absyn_Conref;static const int Cyc_Absyn_Static=0;static const int Cyc_Absyn_Abstract=
1;static const int Cyc_Absyn_Public=2;static const int Cyc_Absyn_Extern=3;static const
int Cyc_Absyn_ExternC=4;struct Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int
q_restrict: 1;};static const int Cyc_Absyn_B1=0;static const int Cyc_Absyn_B2=1;static
const int Cyc_Absyn_B4=2;static const int Cyc_Absyn_B8=3;static const int Cyc_Absyn_AnyKind=
0;static const int Cyc_Absyn_MemKind=1;static const int Cyc_Absyn_BoxKind=2;static
const int Cyc_Absyn_RgnKind=3;static const int Cyc_Absyn_EffKind=4;static const int Cyc_Absyn_Signed=
0;static const int Cyc_Absyn_Unsigned=1;struct Cyc_Absyn_Conref{void*v;};static const
int Cyc_Absyn_Eq_constr=0;struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};
static const int Cyc_Absyn_Forward_constr=1;struct Cyc_Absyn_Forward_constr_struct{
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
struct _tagged_arr*Cyc_Absyn_fieldname(int);struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;
extern struct Cyc_Std___sFILE*Cyc_Std_stderr;extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};static const int Cyc_Std_String_pa=0;struct
Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Std_Int_pa=
1;struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};static const int Cyc_Std_Double_pa=
2;struct Cyc_Std_Double_pa_struct{int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=
3;struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=
4;struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int Cyc_Std_fprintf(
struct Cyc_Std___sFILE*,struct _tagged_arr fmt,struct _tagged_arr);extern struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);static const
int Cyc_Std_ShortPtr_sa=0;struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};
static const int Cyc_Std_UShortPtr_sa=1;struct Cyc_Std_UShortPtr_sa_struct{int tag;
unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=2;struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=3;struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=4;struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};static const int Cyc_Tcenv_VarRes=0;struct Cyc_Tcenv_VarRes_struct{
int tag;void*f1;};static const int Cyc_Tcenv_StructRes=1;struct Cyc_Tcenv_StructRes_struct{
int tag;struct Cyc_Absyn_Structdecl*f1;};static const int Cyc_Tcenv_TunionRes=2;
struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*
f2;};static const int Cyc_Tcenv_EnumRes=3;struct Cyc_Tcenv_EnumRes_struct{int tag;
struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};static const int Cyc_Tcenv_AnonEnumRes=
4;struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;
};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*
structdecls;struct Cyc_Dict_Dict*uniondecls;struct Cyc_Dict_Dict*tuniondecls;
struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;static const int
Cyc_Tcenv_NotLoop_j=0;static const int Cyc_Tcenv_CaseEnd_j=1;static const int Cyc_Tcenv_FnEnd_j=
2;static const int Cyc_Tcenv_Stmt_j=0;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};static const int Cyc_Tcenv_Outermost=0;struct Cyc_Tcenv_Outermost_struct{
int tag;void*f1;};static const int Cyc_Tcenv_Frame=1;struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};static const int Cyc_Tcenv_Hidden=2;struct Cyc_Tcenv_Hidden_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*
ae;struct Cyc_Core_Opt*le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct
_tagged_arr ap);extern void*Cyc_Tcutil_compress(void*t);extern int Cyc_Tcutil_bits_only(
void*t);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*);
static const int Cyc_CfFlowInfo_VarRoot=0;struct Cyc_CfFlowInfo_VarRoot_struct{int
tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_CfFlowInfo_MallocPt=1;struct
Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};static const int Cyc_CfFlowInfo_NoneIL=0;
static const int Cyc_CfFlowInfo_ThisIL=1;static const int Cyc_CfFlowInfo_AllIL=2;
static const int Cyc_CfFlowInfo_EqualConst=0;struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};static const int Cyc_CfFlowInfo_LessVar=1;struct Cyc_CfFlowInfo_LessVar_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};static const int Cyc_CfFlowInfo_LessSize=2;
struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};static
const int Cyc_CfFlowInfo_LessConst=3;struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};static const int Cyc_CfFlowInfo_LessEqSize=4;struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*
vd;void*rop;};unsigned char Cyc_CfFlowInfo_IsZero[11]="\000\000\000\000IsZero";
struct Cyc_CfFlowInfo_IsZero_struct{unsigned char*tag;struct Cyc_List_List*f1;};
unsigned char Cyc_CfFlowInfo_NotZero[12]="\000\000\000\000NotZero";struct Cyc_CfFlowInfo_NotZero_struct{
unsigned char*tag;struct Cyc_List_List*f1;};unsigned char Cyc_CfFlowInfo_UnknownZ[
13]="\000\000\000\000UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_struct{
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
f2;};extern struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();extern void*Cyc_CfFlowInfo_unknown_none;
extern void*Cyc_CfFlowInfo_unknown_this;extern void*Cyc_CfFlowInfo_unknown_all;
extern int Cyc_CfFlowInfo_root_cmp(void*,void*);extern int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);extern void*Cyc_CfFlowInfo_typ_to_absrval(
void*t,void*leafval);extern void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,
void*r);extern void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place);extern int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place);extern int Cyc_CfFlowInfo_flow_lessthan_approx(void*f1,void*f2);extern
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct Cyc_List_List*,struct
Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);extern struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);extern struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_var(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
extern struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);extern void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*);
extern struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(struct Cyc_Dict_Dict*d,
struct Cyc_Set_Set**all_changed,void*r);extern struct Cyc_Dict_Dict*Cyc_CfFlowInfo_assign_place(
struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**
all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);extern void*Cyc_CfFlowInfo_join_flow(
struct Cyc_Set_Set**,void*,void*);extern void*Cyc_CfFlowInfo_after_flow(struct Cyc_Set_Set**,
void*,void*,struct Cyc_Set_Set*,struct Cyc_Set_Set*);static struct Cyc_CfFlowInfo_UnknownR_struct
Cyc_CfFlowInfo_unknown_none_v={0,(void*)((void*)0)};static struct Cyc_CfFlowInfo_UnknownR_struct
Cyc_CfFlowInfo_unknown_this_v={0,(void*)((void*)1)};static struct Cyc_CfFlowInfo_UnknownR_struct
Cyc_CfFlowInfo_unknown_all_v={0,(void*)((void*)2)};static struct Cyc_CfFlowInfo_Esc_struct
Cyc_CfFlowInfo_esc_none_v={1,(void*)((void*)0)};static struct Cyc_CfFlowInfo_Esc_struct
Cyc_CfFlowInfo_esc_this_v={1,(void*)((void*)1)};static struct Cyc_CfFlowInfo_Esc_struct
Cyc_CfFlowInfo_esc_all_v={1,(void*)((void*)2)};void*Cyc_CfFlowInfo_unknown_none=(
void*)& Cyc_CfFlowInfo_unknown_none_v;void*Cyc_CfFlowInfo_unknown_this=(void*)&
Cyc_CfFlowInfo_unknown_this_v;void*Cyc_CfFlowInfo_unknown_all=(void*)& Cyc_CfFlowInfo_unknown_all_v;
void*Cyc_CfFlowInfo_esc_none=(void*)& Cyc_CfFlowInfo_esc_none_v;void*Cyc_CfFlowInfo_esc_this=(
void*)& Cyc_CfFlowInfo_esc_this_v;void*Cyc_CfFlowInfo_esc_all=(void*)& Cyc_CfFlowInfo_esc_all_v;
static struct Cyc_Set_Set**Cyc_CfFlowInfo_mt_place_set_opt=0;struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set(){
if(Cyc_CfFlowInfo_mt_place_set_opt == 0){Cyc_CfFlowInfo_mt_place_set_opt=({struct
Cyc_Set_Set**_tmp6=_cycalloc(sizeof(struct Cyc_Set_Set*));_tmp6[0]=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Set_empty)(
Cyc_CfFlowInfo_place_cmp);_tmp6;});}return*((struct Cyc_Set_Set**)_check_null(Cyc_CfFlowInfo_mt_place_set_opt));}
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){if((int)r1 == (int)r2){return 0;}{int
p1;int p2;{void*_tmp7=r1;struct Cyc_Absyn_Vardecl*_tmpD;struct Cyc_Absyn_Exp*_tmpF;
_LL9: if(*((int*)_tmp7)== Cyc_CfFlowInfo_VarRoot){_LL14: _tmpD=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp7)->f1;goto _LL10;}else{goto _LL11;}_LL11: if(*((int*)_tmp7)== Cyc_CfFlowInfo_MallocPt){
_LL16: _tmpF=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp7)->f1;goto _LL12;}else{
goto _LL8;}_LL10: p1=(int)_tmpD;goto _LL8;_LL12: p1=(int)_tmpF;_LL8:;}{void*_tmp11=
r2;struct Cyc_Absyn_Vardecl*_tmp17;struct Cyc_Absyn_Exp*_tmp19;_LL19: if(*((int*)
_tmp11)== Cyc_CfFlowInfo_VarRoot){_LL24: _tmp17=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmp11)->f1;goto _LL20;}else{goto _LL21;}_LL21: if(*((int*)_tmp11)== Cyc_CfFlowInfo_MallocPt){
_LL26: _tmp19=((struct Cyc_CfFlowInfo_MallocPt_struct*)_tmp11)->f1;goto _LL22;}
else{goto _LL18;}_LL20: p2=(int)_tmp17;goto _LL18;_LL22: p2=(int)_tmp19;_LL18:;}
return p1 - p2;}}int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct
Cyc_CfFlowInfo_Place*p2){if((int)p1 == (int)p2){return 0;}{int i=Cyc_CfFlowInfo_root_cmp((
void*)p1->root,(void*)p2->root);if(i != 0){return i;}return((int(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_Std_zstrptrcmp,p1->fields,p2->fields);}}static struct
_tagged_arr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){struct Cyc_List_List*
sl=0;{void*_tmp1B=(void*)p->root;struct Cyc_Absyn_Vardecl*_tmp21;struct Cyc_Absyn_Exp*
_tmp23;_LL29: if(*((int*)_tmp1B)== Cyc_CfFlowInfo_VarRoot){_LL34: _tmp21=((struct
Cyc_CfFlowInfo_VarRoot_struct*)_tmp1B)->f1;goto _LL30;}else{goto _LL31;}_LL31: if(*((
int*)_tmp1B)== Cyc_CfFlowInfo_MallocPt){_LL36: _tmp23=((struct Cyc_CfFlowInfo_MallocPt_struct*)
_tmp1B)->f1;goto _LL32;}else{goto _LL28;}_LL30: sl=({struct Cyc_List_List*_tmp25=
_cycalloc(sizeof(struct Cyc_List_List));_tmp25->hd=({struct _tagged_arr*_tmp26=
_cycalloc(sizeof(struct _tagged_arr));_tmp26[0]=({struct Cyc_Std_String_pa_struct
_tmp28;_tmp28.tag=Cyc_Std_String_pa;_tmp28.f1=(struct _tagged_arr)*(*_tmp21->name).f2;{
void*_tmp27[1]={& _tmp28};Cyc_Std_aprintf(_tag_arr("%s",sizeof(unsigned char),3),
_tag_arr(_tmp27,sizeof(void*),1));}});_tmp26;});_tmp25->tl=sl;_tmp25;});goto
_LL28;_LL32: sl=({struct Cyc_List_List*_tmp29=_cycalloc(sizeof(struct Cyc_List_List));
_tmp29->hd=({struct _tagged_arr*_tmp2A=_cycalloc(sizeof(struct _tagged_arr));
_tmp2A[0]=({struct Cyc_Std_Int_pa_struct _tmp2C;_tmp2C.tag=Cyc_Std_Int_pa;_tmp2C.f1=(
int)((unsigned int)((int)_tmp23));{void*_tmp2B[1]={& _tmp2C};Cyc_Std_aprintf(
_tag_arr("mpt%d",sizeof(unsigned char),6),_tag_arr(_tmp2B,sizeof(void*),1));}});
_tmp2A;});_tmp29->tl=sl;_tmp29;});_LL28:;}{struct Cyc_List_List*fields=p->fields;
for(0;fields != 0;fields=fields->tl){sl=({struct Cyc_List_List*_tmp2D=_cycalloc(
sizeof(struct Cyc_List_List));_tmp2D->hd=({struct _tagged_arr*_tmp2E=_cycalloc(
sizeof(struct _tagged_arr));_tmp2E[0]=({struct Cyc_Std_String_pa_struct _tmp30;
_tmp30.tag=Cyc_Std_String_pa;_tmp30.f1=(struct _tagged_arr)*((struct _tagged_arr*)
fields->hd);{void*_tmp2F[1]={& _tmp30};Cyc_Std_aprintf(_tag_arr("%s",sizeof(
unsigned char),3),_tag_arr(_tmp2F,sizeof(void*),1));}});_tmp2E;});_tmp2D->tl=sl;
_tmp2D;});}}{struct _tagged_arr*_tmp31=({struct _tagged_arr*_tmp35=_cycalloc(
sizeof(struct _tagged_arr));_tmp35[0]=({struct Cyc_Std_String_pa_struct _tmp37;
_tmp37.tag=Cyc_Std_String_pa;_tmp37.f1=_tag_arr("",sizeof(unsigned char),1);{
void*_tmp36[1]={& _tmp37};Cyc_Std_aprintf(_tag_arr("%s",sizeof(unsigned char),3),
_tag_arr(_tmp36,sizeof(void*),1));}});_tmp35;});for(0;sl != 0;sl=sl->tl){*_tmp31=({
struct Cyc_Std_String_pa_struct _tmp34;_tmp34.tag=Cyc_Std_String_pa;_tmp34.f1=(
struct _tagged_arr)*_tmp31;{struct Cyc_Std_String_pa_struct _tmp33;_tmp33.tag=Cyc_Std_String_pa;
_tmp33.f1=(struct _tagged_arr)*((struct _tagged_arr*)sl->hd);{void*_tmp32[2]={&
_tmp33,& _tmp34};Cyc_Std_aprintf(_tag_arr("%s.%s",sizeof(unsigned char),6),
_tag_arr(_tmp32,sizeof(void*),2));}}});}return _tmp31;}}struct _tuple3{struct Cyc_Absyn_Tqual
f1;void*f2;};void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval){void*_tmp38=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_TunionFieldInfo _tmp44;void*_tmp46;struct
Cyc_Absyn_Tunionfield*_tmp48;struct Cyc_List_List*_tmp4A;struct Cyc_List_List*
_tmp4C;struct Cyc_Absyn_Structdecl**_tmp4E;struct Cyc_Absyn_Structdecl*_tmp50;
struct Cyc_Absyn_Structdecl _tmp51;struct Cyc_Core_Opt*_tmp52;struct Cyc_Core_Opt
_tmp54;struct Cyc_List_List*_tmp55;_LL58: if((unsigned int)_tmp38 > 3?*((int*)
_tmp38)== Cyc_Absyn_TunionFieldType: 0){_LL69: _tmp44=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp38)->f1;_LL71: _tmp46=(void*)_tmp44.field_info;if(*((int*)_tmp46)== Cyc_Absyn_KnownTunionfield){
_LL73: _tmp48=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp46)->f2;goto _LL59;}
else{goto _LL60;}}else{goto _LL60;}_LL60: if((unsigned int)_tmp38 > 3?*((int*)_tmp38)
== Cyc_Absyn_TupleType: 0){_LL75: _tmp4A=((struct Cyc_Absyn_TupleType_struct*)
_tmp38)->f1;goto _LL61;}else{goto _LL62;}_LL62: if((unsigned int)_tmp38 > 3?*((int*)
_tmp38)== Cyc_Absyn_AnonStructType: 0){_LL77: _tmp4C=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp38)->f1;goto _LL63;}else{goto _LL64;}_LL64: if((unsigned int)_tmp38 > 3?*((int*)
_tmp38)== Cyc_Absyn_StructType: 0){_LL79: _tmp4E=((struct Cyc_Absyn_StructType_struct*)
_tmp38)->f3;if(_tmp4E == 0){goto _LL66;}else{_tmp50=*_tmp4E;_tmp51=*_tmp50;_LL83:
_tmp52=_tmp51.fields;if(_tmp52 == 0){goto _LL66;}else{_tmp54=*_tmp52;_LL86: _tmp55=(
struct Cyc_List_List*)_tmp54.v;goto _LL65;}}}else{goto _LL66;}_LL66: goto _LL67;_LL59:
if(_tmp48->typs == 0){return leafval;}_tmp4A=_tmp48->typs;goto _LL61;_LL61: {struct
Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);{int i=0;for(0;_tmp4A != 0;(
_tmp4A=_tmp4A->tl,++ i)){d=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,void*v))Cyc_Dict_insert)(d,Cyc_Absyn_fieldname(i),Cyc_CfFlowInfo_typ_to_absrval((*((
struct _tuple3*)_tmp4A->hd)).f2,leafval));}}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp57=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));_tmp57[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmp58;_tmp58.tag=Cyc_CfFlowInfo_Aggregate;
_tmp58.f1=d;_tmp58;});_tmp57;});}_LL63: _tmp55=_tmp4C;goto _LL65;_LL65: {struct Cyc_Dict_Dict*
d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))
Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);for(0;_tmp55 != 0;_tmp55=_tmp55->tl){struct
Cyc_Absyn_Structfield _tmp5B;void*_tmp5C;struct _tagged_arr*_tmp5E;struct Cyc_Absyn_Structfield*
_tmp59=(struct Cyc_Absyn_Structfield*)_tmp55->hd;_tmp5B=*_tmp59;_LL95: _tmp5E=
_tmp5B.name;goto _LL93;_LL93: _tmp5C=(void*)_tmp5B.type;goto _LL90;_LL90: if(
_get_arr_size(*_tmp5E,sizeof(unsigned char))!= 1){d=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(d,_tmp5E,Cyc_CfFlowInfo_typ_to_absrval(
_tmp5C,leafval));}}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp60=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));_tmp60[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp61;_tmp61.tag=Cyc_CfFlowInfo_Aggregate;_tmp61.f1=d;_tmp61;});_tmp60;});}
_LL67: return Cyc_Tcutil_bits_only(t)? Cyc_CfFlowInfo_unknown_all: leafval;_LL57:;}
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct
Cyc_Set_Set*set){{struct Cyc_List_List*_tmp62=((struct Cyc_List_List*(*)(struct Cyc_Set_Set*
s))Cyc_Set_elements)(set);for(0;_tmp62 != 0;_tmp62=_tmp62->tl){struct Cyc_CfFlowInfo_Place*
_tmp63=(struct Cyc_CfFlowInfo_Place*)_tmp62->hd;if(Cyc_CfFlowInfo_root_cmp((void*)
place->root,(void*)_tmp63->root)!= 0){continue;}{struct Cyc_List_List*_tmp64=
place->fields;struct Cyc_List_List*_tmp65=_tmp63->fields;for(0;_tmp64 != 0? _tmp65
!= 0: 0;(_tmp64=_tmp64->tl,_tmp65=_tmp65->tl)){if(Cyc_Std_zstrptrcmp((struct
_tagged_arr*)_tmp64->hd,(struct _tagged_arr*)_tmp65->hd)!= 0){break;}}if(_tmp64 == 
0){return 1;}}}}return 0;}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;
struct Cyc_List_List*places;};static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*
pile,struct Cyc_CfFlowInfo_Place*place){if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))
Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){pile->places=({
struct Cyc_List_List*_tmp66=_region_malloc(pile->rgn,sizeof(struct Cyc_List_List));
_tmp66->hd=place;_tmp66->tl=pile->places;_tmp66;});}}static void Cyc_CfFlowInfo_add_places(
struct Cyc_CfFlowInfo_EscPile*pile,void*a,void*r){void*_tmp67=r;struct Cyc_CfFlowInfo_Place*
_tmp6F;struct Cyc_Dict_Dict*_tmp71;_LL105: if((unsigned int)_tmp67 > 3?*((int*)
_tmp67)== Cyc_CfFlowInfo_AddressOf: 0){_LL112: _tmp6F=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp67)->f1;goto _LL106;}else{goto _LL107;}_LL107: if((unsigned int)_tmp67 > 3?*((
int*)_tmp67)== Cyc_CfFlowInfo_Aggregate: 0){_LL114: _tmp71=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp67)->f1;goto _LL108;}else{goto _LL109;}_LL109: goto _LL110;_LL106: Cyc_CfFlowInfo_add_place(
pile,_tmp6F);return;_LL108:((void(*)(void(*f)(struct Cyc_CfFlowInfo_EscPile*,
struct _tagged_arr*,void*),struct Cyc_CfFlowInfo_EscPile*env,struct Cyc_Dict_Dict*d))
Cyc_Dict_iter_c)((void(*)(struct Cyc_CfFlowInfo_EscPile*pile,struct _tagged_arr*a,
void*r))Cyc_CfFlowInfo_add_places,pile,_tmp71);return;_LL110: return;_LL104:;}
static void*Cyc_CfFlowInfo_insert_place_inner(void*new_val,void*old_val){void*
_tmp73=old_val;struct Cyc_Dict_Dict*_tmp79;_LL117: if((unsigned int)_tmp73 > 3?*((
int*)_tmp73)== Cyc_CfFlowInfo_Aggregate: 0){_LL122: _tmp79=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp73)->f1;goto _LL118;}else{goto _LL119;}_LL119: goto _LL120;_LL118: return(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp7B=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));
_tmp7B[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp7C;_tmp7C.tag=Cyc_CfFlowInfo_Aggregate;
_tmp7C.f1=((struct Cyc_Dict_Dict*(*)(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict*
d))Cyc_Dict_map_c)(Cyc_CfFlowInfo_insert_place_inner,new_val,_tmp79);_tmp7C;});
_tmp7B;});_LL120: return new_val;_LL116:;}struct _tuple4{struct Cyc_List_List*f1;
void*f2;};static void*Cyc_CfFlowInfo_insert_place_outer(struct Cyc_List_List*fs,
void*old_val,void*new_val){if(fs == 0){return Cyc_CfFlowInfo_insert_place_inner(
new_val,old_val);}{struct _tuple4 _tmp7E=({struct _tuple4 _tmp7D;_tmp7D.f1=fs;_tmp7D.f2=
old_val;_tmp7D;});void*_tmp84;struct Cyc_Dict_Dict*_tmp86;struct Cyc_List_List*
_tmp88;struct Cyc_List_List _tmp8A;struct Cyc_List_List*_tmp8B;struct _tagged_arr*
_tmp8D;_LL128: _LL137: _tmp88=_tmp7E.f1;if(_tmp88 == 0){goto _LL130;}else{_tmp8A=*
_tmp88;_LL142: _tmp8D=(struct _tagged_arr*)_tmp8A.hd;goto _LL140;_LL140: _tmp8B=
_tmp8A.tl;goto _LL133;}_LL133: _tmp84=_tmp7E.f2;if((unsigned int)_tmp84 > 3?*((int*)
_tmp84)== Cyc_CfFlowInfo_Aggregate: 0){_LL135: _tmp86=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp84)->f1;goto _LL129;}else{goto _LL130;}_LL130: goto _LL131;_LL129: {void*_tmp8F=
Cyc_CfFlowInfo_insert_place_outer(_tmp8B,((void*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_lookup)(_tmp86,_tmp8D),new_val);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp90=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));_tmp90[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmp91;_tmp91.tag=Cyc_CfFlowInfo_Aggregate;
_tmp91.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,
void*v))Cyc_Dict_insert)(_tmp86,_tmp8D,_tmp8F);_tmp91;});_tmp90;});}_LL131:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp92=_cycalloc(sizeof(struct
Cyc_Core_Impossible_struct));_tmp92[0]=({struct Cyc_Core_Impossible_struct _tmp93;
_tmp93.tag=Cyc_Core_Impossible;_tmp93.f1=_tag_arr("bad insert place",sizeof(
unsigned char),17);_tmp93;});_tmp92;}));_LL127:;}}static struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_these(
struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Set_Set**all_changed,struct Cyc_Dict_Dict*
d){while(pile->places != 0){struct Cyc_CfFlowInfo_Place*_tmp94=(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(pile->places))->hd;pile->places=((struct Cyc_List_List*)
_check_null(pile->places))->tl;if(all_changed != 0){*all_changed=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*all_changed,
_tmp94);}{void*oldval;void*newval;{struct _handler_cons _tmp95;_push_handler(&
_tmp95);{int _tmp97=0;if(setjmp(_tmp95.handler)){_tmp97=1;}if(! _tmp97){oldval=Cyc_CfFlowInfo_lookup_place(
d,_tmp94);;_pop_handler();}else{void*_tmp96=(void*)_exn_thrown;void*_tmp99=
_tmp96;_LL155: if(_tmp99 == Cyc_Dict_Absent){goto _LL156;}else{goto _LL157;}_LL157:
goto _LL158;_LL156: continue;_LL158:(void)_throw(_tmp99);_LL154:;}}}{void*_tmp9F=
Cyc_CfFlowInfo_initlevel(d,oldval);_LL161: if(_tmp9F == (void*)Cyc_CfFlowInfo_AllIL){
goto _LL162;}else{goto _LL163;}_LL163: if(_tmp9F == (void*)Cyc_CfFlowInfo_ThisIL){
goto _LL164;}else{goto _LL165;}_LL165: if(_tmp9F == (void*)Cyc_CfFlowInfo_NoneIL){
goto _LL166;}else{goto _LL160;}_LL162: newval=Cyc_CfFlowInfo_esc_all;goto _LL160;
_LL164: newval=Cyc_CfFlowInfo_esc_this;goto _LL160;_LL166: newval=Cyc_CfFlowInfo_esc_none;
goto _LL160;_LL160:;}((void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,oldval);d=Cyc_Dict_insert(d,(void*)_tmp94->root,Cyc_CfFlowInfo_insert_place_outer(
_tmp94->fields,Cyc_Dict_lookup(d,(void*)_tmp94->root),newval));}}return d;}struct
Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict*d;struct Cyc_List_List*seen;};
static void*Cyc_CfFlowInfo_initlevel_approx(void*r){void*_tmpA7=r;void*_tmpB5;
void*_tmpB7;_LL169: if((unsigned int)_tmpA7 > 3?*((int*)_tmpA7)== Cyc_CfFlowInfo_UnknownR:
0){_LL182: _tmpB5=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmpA7)->f1;goto
_LL170;}else{goto _LL171;}_LL171: if((unsigned int)_tmpA7 > 3?*((int*)_tmpA7)== Cyc_CfFlowInfo_Esc:
0){_LL184: _tmpB7=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmpA7)->f1;goto
_LL172;}else{goto _LL173;}_LL173: if(_tmpA7 == (void*)Cyc_CfFlowInfo_Zero){goto
_LL174;}else{goto _LL175;}_LL175: if(_tmpA7 == (void*)Cyc_CfFlowInfo_NotZeroAll){
goto _LL176;}else{goto _LL177;}_LL177: if(_tmpA7 == (void*)Cyc_CfFlowInfo_NotZeroThis){
goto _LL178;}else{goto _LL179;}_LL179: goto _LL180;_LL170: return _tmpB5;_LL172: return
_tmpB7;_LL174: goto _LL176;_LL176: return(void*)Cyc_CfFlowInfo_AllIL;_LL178: return(
void*)Cyc_CfFlowInfo_ThisIL;_LL180:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpB9=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmpB9[0]=({struct Cyc_Core_Impossible_struct
_tmpBA;_tmpBA.tag=Cyc_Core_Impossible;_tmpBA.f1=_tag_arr("initlevel_approx",
sizeof(unsigned char),17);_tmpBA;});_tmpB9;}));_LL168:;}static void*Cyc_CfFlowInfo_initlevel_rec(
struct Cyc_CfFlowInfo_InitlevelEnv*env,void*a,void*r,void*acc){void*this_ans;if(
acc == (void*)Cyc_CfFlowInfo_NoneIL){return(void*)Cyc_CfFlowInfo_NoneIL;}{void*
_tmpBB=r;struct Cyc_Dict_Dict*_tmpC3;struct Cyc_CfFlowInfo_Place*_tmpC5;_LL189: if((
unsigned int)_tmpBB > 3?*((int*)_tmpBB)== Cyc_CfFlowInfo_Aggregate: 0){_LL196:
_tmpC3=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpBB)->f1;goto _LL190;}else{
goto _LL191;}_LL191: if((unsigned int)_tmpBB > 3?*((int*)_tmpBB)== Cyc_CfFlowInfo_AddressOf:
0){_LL198: _tmpC5=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpBB)->f1;goto _LL192;}
else{goto _LL193;}_LL193: goto _LL194;_LL190: this_ans=((void*(*)(void*(*f)(struct
Cyc_CfFlowInfo_InitlevelEnv*,struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_InitlevelEnv*
env,struct Cyc_Dict_Dict*d,void*accum))Cyc_Dict_fold_c)((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*
env,struct _tagged_arr*a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec,env,_tmpC3,(
void*)Cyc_CfFlowInfo_AllIL);goto _LL188;_LL192: if(((int(*)(int(*compare)(struct
Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct
Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmpC5)){
this_ans=(void*)Cyc_CfFlowInfo_AllIL;}else{env->seen=({struct Cyc_List_List*
_tmpC7=_cycalloc(sizeof(struct Cyc_List_List));_tmpC7->hd=_tmpC5;_tmpC7->tl=env->seen;
_tmpC7;});this_ans=((void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,int a,void*r,
void*acc))Cyc_CfFlowInfo_initlevel_rec)(env,0,Cyc_CfFlowInfo_lookup_place(env->d,
_tmpC5),(void*)Cyc_CfFlowInfo_AllIL);env->seen=((struct Cyc_List_List*)
_check_null(env->seen))->tl;if(this_ans == (void*)Cyc_CfFlowInfo_NoneIL){this_ans=(
void*)Cyc_CfFlowInfo_ThisIL;}}goto _LL188;_LL194: this_ans=Cyc_CfFlowInfo_initlevel_approx(
r);_LL188:;}if(this_ans == (void*)Cyc_CfFlowInfo_NoneIL){return(void*)Cyc_CfFlowInfo_NoneIL;}
if(this_ans == (void*)Cyc_CfFlowInfo_ThisIL? 1: acc == (void*)Cyc_CfFlowInfo_ThisIL){
return(void*)Cyc_CfFlowInfo_ThisIL;}return(void*)Cyc_CfFlowInfo_AllIL;}void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict*d,void*r){struct Cyc_CfFlowInfo_InitlevelEnv _tmpC8=({struct
Cyc_CfFlowInfo_InitlevelEnv _tmpC9;_tmpC9.d=d;_tmpC9.seen=0;_tmpC9;});return((
void*(*)(struct Cyc_CfFlowInfo_InitlevelEnv*env,int a,void*r,void*acc))Cyc_CfFlowInfo_initlevel_rec)(&
_tmpC8,0,r,(void*)Cyc_CfFlowInfo_AllIL);}void*Cyc_CfFlowInfo_lookup_place(struct
Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*place){struct Cyc_CfFlowInfo_Place
_tmpCC;struct Cyc_List_List*_tmpCD;void*_tmpCF;struct Cyc_CfFlowInfo_Place*_tmpCA=
place;_tmpCC=*_tmpCA;_LL208: _tmpCF=(void*)_tmpCC.root;goto _LL206;_LL206: _tmpCD=
_tmpCC.fields;goto _LL203;_LL203: {void*_tmpD1=Cyc_Dict_lookup(d,_tmpCF);for(0;
_tmpCD != 0;_tmpCD=_tmpCD->tl){struct _tuple0 _tmpD3=({struct _tuple0 _tmpD2;_tmpD2.f1=
_tmpD1;_tmpD2.f2=(struct _tagged_arr*)_tmpCD->hd;_tmpD2;});struct _tagged_arr*
_tmpD9;void*_tmpDB;struct Cyc_Dict_Dict*_tmpDD;_LL213: _LL220: _tmpDB=_tmpD3.f1;if((
unsigned int)_tmpDB > 3?*((int*)_tmpDB)== Cyc_CfFlowInfo_Aggregate: 0){_LL222:
_tmpDD=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmpDB)->f1;goto _LL218;}else{
goto _LL215;}_LL218: _tmpD9=_tmpD3.f2;goto _LL214;_LL215: goto _LL216;_LL214: _tmpD1=((
void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmpDD,
_tmpD9);goto _LL212;_LL216:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpDF=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmpDF[0]=({struct Cyc_Core_Impossible_struct
_tmpE0;_tmpE0.tag=Cyc_Core_Impossible;_tmpE0.f1=_tag_arr("bad lookup_place",
sizeof(unsigned char),17);_tmpE0;});_tmpDF;}));_LL212:;}return _tmpD1;}}static int
Cyc_CfFlowInfo_is_rval_unescaped(void*a,void*b,void*rval){void*_tmpE1=rval;
struct Cyc_Dict_Dict*_tmpE9;_LL227: if((unsigned int)_tmpE1 > 3?*((int*)_tmpE1)== 
Cyc_CfFlowInfo_Esc: 0){goto _LL228;}else{goto _LL229;}_LL229: if((unsigned int)
_tmpE1 > 3?*((int*)_tmpE1)== Cyc_CfFlowInfo_Aggregate: 0){_LL234: _tmpE9=((struct
Cyc_CfFlowInfo_Aggregate_struct*)_tmpE1)->f1;goto _LL230;}else{goto _LL231;}_LL231:
goto _LL232;_LL228: return 0;_LL230: return((int(*)(int(*f)(int,struct _tagged_arr*,
void*),int env,struct Cyc_Dict_Dict*d))Cyc_Dict_forall_c)((int(*)(int a,struct
_tagged_arr*b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped,0,_tmpE9);_LL232:
return 1;_LL226:;}int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*
place){return((int(*)(int a,int b,void*rval))Cyc_CfFlowInfo_is_rval_unescaped)(0,0,
Cyc_CfFlowInfo_lookup_place(d,place));}struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r){struct _RegionHandle
_tmpEB=_new_region("rgn");struct _RegionHandle*rgn=& _tmpEB;_push_region(rgn);{
struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmpED=
_region_malloc(rgn,sizeof(struct Cyc_CfFlowInfo_EscPile));_tmpED->rgn=rgn;_tmpED->places=
0;_tmpED;});((void(*)(struct Cyc_CfFlowInfo_EscPile*pile,int a,void*r))Cyc_CfFlowInfo_add_places)(
pile,0,r);{struct Cyc_Dict_Dict*_tmpEC=Cyc_CfFlowInfo_escape_these(pile,
all_changed,d);_npop_handler(0);return _tmpEC;}};_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{
struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict*d;struct Cyc_Position_Segment*
loc;};struct _tuple5{void*f1;void*f2;};static void*Cyc_CfFlowInfo_assign_place_inner(
struct Cyc_CfFlowInfo_AssignEnv*env,void*a,void*oldval,void*newval){struct _tuple5
_tmpEF=({struct _tuple5 _tmpEE;_tmpEE.f1=oldval;_tmpEE.f2=newval;_tmpEE;});void*
_tmpFB;struct Cyc_CfFlowInfo_Place*_tmpFD;void*_tmpFF;void*_tmp101;void*_tmp103;
struct Cyc_Dict_Dict*_tmp105;void*_tmp107;struct Cyc_Dict_Dict*_tmp109;void*
_tmp10B;void*_tmp10D;_LL241: _LL256: _tmpFF=_tmpEF.f1;if((unsigned int)_tmpFF > 3?*((
int*)_tmpFF)== Cyc_CfFlowInfo_Esc: 0){goto _LL252;}else{goto _LL243;}_LL252: _tmpFB=
_tmpEF.f2;if((unsigned int)_tmpFB > 3?*((int*)_tmpFB)== Cyc_CfFlowInfo_AddressOf:
0){_LL254: _tmpFD=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmpFB)->f1;goto _LL242;}
else{goto _LL243;}_LL243: _LL258: _tmp101=_tmpEF.f1;if((unsigned int)_tmp101 > 3?*((
int*)_tmp101)== Cyc_CfFlowInfo_Esc: 0){goto _LL244;}else{goto _LL245;}_LL245: _LL264:
_tmp107=_tmpEF.f1;if((unsigned int)_tmp107 > 3?*((int*)_tmp107)== Cyc_CfFlowInfo_Aggregate:
0){_LL266: _tmp109=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp107)->f1;goto
_LL260;}else{goto _LL247;}_LL260: _tmp103=_tmpEF.f2;if((unsigned int)_tmp103 > 3?*((
int*)_tmp103)== Cyc_CfFlowInfo_Aggregate: 0){_LL262: _tmp105=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp103)->f1;goto _LL246;}else{goto _LL247;}_LL247: _LL268: _tmp10B=_tmpEF.f2;if((
unsigned int)_tmp10B > 3?*((int*)_tmp10B)== Cyc_CfFlowInfo_Esc: 0){_LL270: _tmp10D=(
void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp10B)->f1;goto _LL248;}else{goto
_LL249;}_LL249: goto _LL250;_LL242: Cyc_CfFlowInfo_add_place(env->pile,_tmpFD);goto
_LL244;_LL244: if(Cyc_CfFlowInfo_initlevel(env->d,newval)!= (void*)Cyc_CfFlowInfo_AllIL){({
void*_tmp10F[0]={};Cyc_Tcutil_terr(env->loc,_tag_arr("assignment puts possibly-uninitialized data in an escaped location",
sizeof(unsigned char),67),_tag_arr(_tmp10F,sizeof(void*),0));});}return Cyc_CfFlowInfo_esc_all;
_LL246: {struct Cyc_Dict_Dict*_tmp110=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct
Cyc_CfFlowInfo_AssignEnv*,struct _tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_AssignEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(
struct Cyc_CfFlowInfo_AssignEnv*env,struct _tagged_arr*a,void*oldval,void*newval))
Cyc_CfFlowInfo_assign_place_inner,env,_tmp109,_tmp105);if(_tmp110 == _tmp109){
return oldval;}if(_tmp110 == _tmp105){return newval;}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp111=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));_tmp111[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmp112;_tmp112.tag=Cyc_CfFlowInfo_Aggregate;
_tmp112.f1=_tmp110;_tmp112;});_tmp111;});}_LL248: {void*_tmp113=_tmp10D;_LL277:
if(_tmp113 == (void*)Cyc_CfFlowInfo_NoneIL){goto _LL278;}else{goto _LL279;}_LL279:
if(_tmp113 == (void*)Cyc_CfFlowInfo_ThisIL){goto _LL280;}else{goto _LL281;}_LL281:
if(_tmp113 == (void*)Cyc_CfFlowInfo_AllIL){goto _LL282;}else{goto _LL276;}_LL278:
return Cyc_CfFlowInfo_unknown_none;_LL280: return Cyc_CfFlowInfo_unknown_this;
_LL282: return Cyc_CfFlowInfo_unknown_all;_LL276:;}_LL250: return newval;_LL240:;}
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,
struct Cyc_List_List*fs,void*oldval,void*newval){if(fs == 0){return((void*(*)(
struct Cyc_CfFlowInfo_AssignEnv*env,int a,void*oldval,void*newval))Cyc_CfFlowInfo_assign_place_inner)(
env,0,oldval,newval);}{struct _tuple4 _tmp11C=({struct _tuple4 _tmp11B;_tmp11B.f1=fs;
_tmp11B.f2=oldval;_tmp11B;});void*_tmp122;struct Cyc_Dict_Dict*_tmp124;struct Cyc_List_List*
_tmp126;struct Cyc_List_List _tmp128;struct Cyc_List_List*_tmp129;struct _tagged_arr*
_tmp12B;_LL286: _LL295: _tmp126=_tmp11C.f1;if(_tmp126 == 0){goto _LL288;}else{
_tmp128=*_tmp126;_LL300: _tmp12B=(struct _tagged_arr*)_tmp128.hd;goto _LL298;_LL298:
_tmp129=_tmp128.tl;goto _LL291;}_LL291: _tmp122=_tmp11C.f2;if((unsigned int)
_tmp122 > 3?*((int*)_tmp122)== Cyc_CfFlowInfo_Aggregate: 0){_LL293: _tmp124=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp122)->f1;goto _LL287;}else{goto _LL288;}
_LL288: goto _LL289;_LL287: {void*_tmp12D=Cyc_CfFlowInfo_assign_place_outer(env,
_tmp129,((void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp124,_tmp12B),newval);return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp12E=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));_tmp12E[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmp12F;_tmp12F.tag=Cyc_CfFlowInfo_Aggregate;
_tmp12F.f1=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,
void*v))Cyc_Dict_insert)(_tmp124,_tmp12B,_tmp12D);_tmp12F;});_tmp12E;});}_LL289:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp130=_cycalloc(sizeof(
struct Cyc_Core_Impossible_struct));_tmp130[0]=({struct Cyc_Core_Impossible_struct
_tmp131;_tmp131.tag=Cyc_Core_Impossible;_tmp131.f1=_tag_arr("bad insert place",
sizeof(unsigned char),17);_tmp131;});_tmp130;}));_LL285:;}}struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,
struct Cyc_Set_Set**all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){if(
all_changed != 0){*all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*all_changed,place);}{struct
_RegionHandle _tmp132=_new_region("rgn");struct _RegionHandle*rgn=& _tmp132;
_push_region(rgn);{struct Cyc_CfFlowInfo_Place _tmp135;struct Cyc_List_List*_tmp136;
void*_tmp138;struct Cyc_CfFlowInfo_Place*_tmp133=place;_tmp135=*_tmp133;_LL313:
_tmp138=(void*)_tmp135.root;goto _LL311;_LL311: _tmp136=_tmp135.fields;goto _LL308;
_LL308: {struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv
_tmp13B;_tmp13B.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp13C=_region_malloc(rgn,
sizeof(struct Cyc_CfFlowInfo_EscPile));_tmp13C->rgn=rgn;_tmp13C->places=0;_tmp13C;});
_tmp13B.d=d;_tmp13B.loc=loc;_tmp13B;});void*newval=Cyc_CfFlowInfo_assign_place_outer(&
env,_tmp136,Cyc_Dict_lookup(d,_tmp138),r);struct Cyc_Dict_Dict*_tmp13A=Cyc_CfFlowInfo_escape_these(
env.pile,all_changed,Cyc_Dict_insert(d,_tmp138,newval));_npop_handler(0);return
_tmp13A;}};_pop_region(rgn);}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_EscPile*
pile;struct Cyc_Dict_Dict*d1;struct Cyc_Dict_Dict*d2;};enum Cyc_CfFlowInfo_WhoIsChanged{
Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct
Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Set_Set*
chg1;struct Cyc_Set_Set*chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**
last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};static void*Cyc_CfFlowInfo_join_absRval(
struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){if(r1 == r2){return r1;}{
struct _tuple5 _tmp13E=({struct _tuple5 _tmp13D;_tmp13D.f1=r1;_tmp13D.f2=r2;_tmp13D;});
void*_tmp14E;struct Cyc_CfFlowInfo_Place*_tmp150;void*_tmp152;struct Cyc_CfFlowInfo_Place*
_tmp154;void*_tmp156;struct Cyc_CfFlowInfo_Place*_tmp158;void*_tmp15A;struct Cyc_CfFlowInfo_Place*
_tmp15C;void*_tmp15E;void*_tmp160;void*_tmp162;void*_tmp164;void*_tmp166;struct
Cyc_Dict_Dict*_tmp168;void*_tmp16A;struct Cyc_Dict_Dict*_tmp16C;_LL320: _LL339:
_tmp152=_tmp13E.f1;if((unsigned int)_tmp152 > 3?*((int*)_tmp152)== Cyc_CfFlowInfo_AddressOf:
0){_LL341: _tmp154=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp152)->f1;goto
_LL335;}else{goto _LL322;}_LL335: _tmp14E=_tmp13E.f2;if((unsigned int)_tmp14E > 3?*((
int*)_tmp14E)== Cyc_CfFlowInfo_AddressOf: 0){_LL337: _tmp150=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp14E)->f1;goto _LL321;}else{goto _LL322;}_LL322: _LL343: _tmp156=_tmp13E.f1;if((
unsigned int)_tmp156 > 3?*((int*)_tmp156)== Cyc_CfFlowInfo_AddressOf: 0){_LL345:
_tmp158=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp156)->f1;goto _LL323;}else{
goto _LL324;}_LL324: _LL347: _tmp15A=_tmp13E.f2;if((unsigned int)_tmp15A > 3?*((int*)
_tmp15A)== Cyc_CfFlowInfo_AddressOf: 0){_LL349: _tmp15C=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp15A)->f1;goto _LL325;}else{goto _LL326;}_LL326: _LL353: _tmp160=_tmp13E.f1;if(
_tmp160 == (void*)Cyc_CfFlowInfo_NotZeroAll){goto _LL351;}else{goto _LL328;}_LL351:
_tmp15E=_tmp13E.f2;if(_tmp15E == (void*)Cyc_CfFlowInfo_NotZeroThis){goto _LL327;}
else{goto _LL328;}_LL328: _LL357: _tmp164=_tmp13E.f1;if(_tmp164 == (void*)Cyc_CfFlowInfo_NotZeroThis){
goto _LL355;}else{goto _LL330;}_LL355: _tmp162=_tmp13E.f2;if(_tmp162 == (void*)Cyc_CfFlowInfo_NotZeroAll){
goto _LL329;}else{goto _LL330;}_LL330: _LL363: _tmp16A=_tmp13E.f1;if((unsigned int)
_tmp16A > 3?*((int*)_tmp16A)== Cyc_CfFlowInfo_Aggregate: 0){_LL365: _tmp16C=((
struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp16A)->f1;goto _LL359;}else{goto _LL332;}
_LL359: _tmp166=_tmp13E.f2;if((unsigned int)_tmp166 > 3?*((int*)_tmp166)== Cyc_CfFlowInfo_Aggregate:
0){_LL361: _tmp168=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp166)->f1;goto
_LL331;}else{goto _LL332;}_LL332: goto _LL333;_LL321: if(Cyc_CfFlowInfo_place_cmp(
_tmp154,_tmp150)== 0){return r1;}Cyc_CfFlowInfo_add_place(env->pile,_tmp154);Cyc_CfFlowInfo_add_place(
env->pile,_tmp150);goto _LL319;_LL323: Cyc_CfFlowInfo_add_place(env->pile,_tmp158);
goto _LL319;_LL325: Cyc_CfFlowInfo_add_place(env->pile,_tmp15C);goto _LL319;_LL327:
goto _LL329;_LL329: return(void*)Cyc_CfFlowInfo_NotZeroThis;_LL331: {struct Cyc_Dict_Dict*
_tmp16E=((struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,struct
_tagged_arr*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict*
d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,struct _tagged_arr*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,env,_tmp16C,
_tmp168);if(_tmp16E == _tmp16C){return r1;}if(_tmp16E == _tmp168){return r2;}return(
void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp16F=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));
_tmp16F[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp170;_tmp170.tag=Cyc_CfFlowInfo_Aggregate;
_tmp170.f1=_tmp16E;_tmp170;});_tmp16F;});}_LL333: goto _LL319;_LL319:;}{void*il1=
Cyc_CfFlowInfo_initlevel(env->d1,r1);void*il2=Cyc_CfFlowInfo_initlevel(env->d2,
r2);struct _tuple5 _tmp172=({struct _tuple5 _tmp171;_tmp171.f1=r1;_tmp171.f2=r2;
_tmp171;});void*_tmp17A;void*_tmp17C;_LL372: _LL379: _tmp17A=_tmp172.f1;if((
unsigned int)_tmp17A > 3?*((int*)_tmp17A)== Cyc_CfFlowInfo_Esc: 0){goto _LL373;}
else{goto _LL374;}_LL374: _LL381: _tmp17C=_tmp172.f2;if((unsigned int)_tmp17C > 3?*((
int*)_tmp17C)== Cyc_CfFlowInfo_Esc: 0){goto _LL375;}else{goto _LL376;}_LL376: goto
_LL377;_LL373: goto _LL375;_LL375: {struct _tuple5 _tmp17F=({struct _tuple5 _tmp17E;
_tmp17E.f1=il1;_tmp17E.f2=il2;_tmp17E;});void*_tmp18B;void*_tmp18D;void*_tmp18F;
void*_tmp191;_LL385: _LL396: _tmp18B=_tmp17F.f2;if(_tmp18B == (void*)Cyc_CfFlowInfo_NoneIL){
goto _LL386;}else{goto _LL387;}_LL387: _LL398: _tmp18D=_tmp17F.f1;if(_tmp18D == (void*)
Cyc_CfFlowInfo_NoneIL){goto _LL388;}else{goto _LL389;}_LL389: _LL400: _tmp18F=
_tmp17F.f2;if(_tmp18F == (void*)Cyc_CfFlowInfo_ThisIL){goto _LL390;}else{goto
_LL391;}_LL391: _LL402: _tmp191=_tmp17F.f1;if(_tmp191 == (void*)Cyc_CfFlowInfo_ThisIL){
goto _LL392;}else{goto _LL393;}_LL393: goto _LL394;_LL386: goto _LL388;_LL388: return
Cyc_CfFlowInfo_esc_none;_LL390: goto _LL392;_LL392: return Cyc_CfFlowInfo_esc_this;
_LL394: return Cyc_CfFlowInfo_esc_all;_LL384:;}_LL377: {struct _tuple5 _tmp194=({
struct _tuple5 _tmp193;_tmp193.f1=il1;_tmp193.f2=il2;_tmp193;});void*_tmp1A0;void*
_tmp1A2;void*_tmp1A4;void*_tmp1A6;_LL406: _LL417: _tmp1A0=_tmp194.f2;if(_tmp1A0 == (
void*)Cyc_CfFlowInfo_NoneIL){goto _LL407;}else{goto _LL408;}_LL408: _LL419: _tmp1A2=
_tmp194.f1;if(_tmp1A2 == (void*)Cyc_CfFlowInfo_NoneIL){goto _LL409;}else{goto
_LL410;}_LL410: _LL421: _tmp1A4=_tmp194.f2;if(_tmp1A4 == (void*)Cyc_CfFlowInfo_ThisIL){
goto _LL411;}else{goto _LL412;}_LL412: _LL423: _tmp1A6=_tmp194.f1;if(_tmp1A6 == (void*)
Cyc_CfFlowInfo_ThisIL){goto _LL413;}else{goto _LL414;}_LL414: goto _LL415;_LL407:
goto _LL409;_LL409: return Cyc_CfFlowInfo_unknown_none;_LL411: goto _LL413;_LL413:
return Cyc_CfFlowInfo_unknown_this;_LL415: return Cyc_CfFlowInfo_unknown_all;_LL405:;}
_LL371:;}}int Cyc_CfFlowInfo_same_relop(void*r1,void*r2){if(r1 == r2){return 1;}{
struct _tuple5 _tmp1A9=({struct _tuple5 _tmp1A8;_tmp1A8.f1=r1;_tmp1A8.f2=r2;_tmp1A8;});
void*_tmp1B7;unsigned int _tmp1B9;void*_tmp1BB;unsigned int _tmp1BD;void*_tmp1BF;
struct Cyc_Absyn_Vardecl*_tmp1C1;void*_tmp1C3;struct Cyc_Absyn_Vardecl*_tmp1C5;
void*_tmp1C7;struct Cyc_Absyn_Vardecl*_tmp1C9;void*_tmp1CB;struct Cyc_Absyn_Vardecl*
_tmp1CD;void*_tmp1CF;unsigned int _tmp1D1;void*_tmp1D3;unsigned int _tmp1D5;void*
_tmp1D7;struct Cyc_Absyn_Vardecl*_tmp1D9;void*_tmp1DB;struct Cyc_Absyn_Vardecl*
_tmp1DD;_LL427: _LL444: _tmp1BB=_tmp1A9.f1;if(*((int*)_tmp1BB)== Cyc_CfFlowInfo_EqualConst){
_LL446: _tmp1BD=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp1BB)->f1;goto _LL440;}
else{goto _LL429;}_LL440: _tmp1B7=_tmp1A9.f2;if(*((int*)_tmp1B7)== Cyc_CfFlowInfo_EqualConst){
_LL442: _tmp1B9=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp1B7)->f1;goto _LL428;}
else{goto _LL429;}_LL429: _LL452: _tmp1C3=_tmp1A9.f1;if(*((int*)_tmp1C3)== Cyc_CfFlowInfo_LessVar){
_LL454: _tmp1C5=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1C3)->f1;goto _LL448;}
else{goto _LL431;}_LL448: _tmp1BF=_tmp1A9.f2;if(*((int*)_tmp1BF)== Cyc_CfFlowInfo_LessVar){
_LL450: _tmp1C1=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp1BF)->f1;goto _LL430;}
else{goto _LL431;}_LL431: _LL460: _tmp1CB=_tmp1A9.f1;if(*((int*)_tmp1CB)== Cyc_CfFlowInfo_LessSize){
_LL462: _tmp1CD=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1CB)->f1;goto _LL456;}
else{goto _LL433;}_LL456: _tmp1C7=_tmp1A9.f2;if(*((int*)_tmp1C7)== Cyc_CfFlowInfo_LessSize){
_LL458: _tmp1C9=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp1C7)->f1;goto _LL432;}
else{goto _LL433;}_LL433: _LL468: _tmp1D3=_tmp1A9.f1;if(*((int*)_tmp1D3)== Cyc_CfFlowInfo_LessConst){
_LL470: _tmp1D5=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp1D3)->f1;goto _LL464;}
else{goto _LL435;}_LL464: _tmp1CF=_tmp1A9.f2;if(*((int*)_tmp1CF)== Cyc_CfFlowInfo_LessConst){
_LL466: _tmp1D1=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp1CF)->f1;goto _LL434;}
else{goto _LL435;}_LL435: _LL476: _tmp1DB=_tmp1A9.f1;if(*((int*)_tmp1DB)== Cyc_CfFlowInfo_LessEqSize){
_LL478: _tmp1DD=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp1DB)->f1;goto _LL472;}
else{goto _LL437;}_LL472: _tmp1D7=_tmp1A9.f2;if(*((int*)_tmp1D7)== Cyc_CfFlowInfo_LessEqSize){
_LL474: _tmp1D9=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp1D7)->f1;goto _LL436;}
else{goto _LL437;}_LL437: goto _LL438;_LL428: return _tmp1BD == _tmp1B9;_LL430: return
_tmp1C5 == _tmp1C1;_LL432: return _tmp1CD == _tmp1C9;_LL434: return _tmp1D5 == _tmp1D1;
_LL436: return _tmp1DD == _tmp1D9;_LL438: return 0;_LL426:;}}struct Cyc_List_List*Cyc_CfFlowInfo_join_relns(
struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){if(r1s == r2s){return r1s;}{
struct Cyc_List_List*res=0;int diff=0;{struct Cyc_List_List*_tmp1DF=r1s;for(0;
_tmp1DF != 0;_tmp1DF=_tmp1DF->tl){struct Cyc_CfFlowInfo_Reln*_tmp1E0=(struct Cyc_CfFlowInfo_Reln*)
_tmp1DF->hd;int found=0;{struct Cyc_List_List*_tmp1E1=r2s;for(0;_tmp1E1 != 0;
_tmp1E1=_tmp1E1->tl){struct Cyc_CfFlowInfo_Reln*_tmp1E2=(struct Cyc_CfFlowInfo_Reln*)
_tmp1E1->hd;if(_tmp1E0 == _tmp1E2? 1:(_tmp1E0->vd == _tmp1E2->vd? Cyc_CfFlowInfo_same_relop((
void*)_tmp1E0->rop,(void*)_tmp1E2->rop): 0)){res=({struct Cyc_List_List*_tmp1E3=
_cycalloc(sizeof(struct Cyc_List_List));_tmp1E3->hd=_tmp1E0;_tmp1E3->tl=res;
_tmp1E3;});found=1;break;}}}if(! found){diff=1;}}}if(! diff){res=r1s;}return res;}}
void*Cyc_CfFlowInfo_join_flow(struct Cyc_Set_Set**all_changed,void*f1,void*f2){
if(f1 == f2){return f1;}{struct _tuple5 _tmp1E5=({struct _tuple5 _tmp1E4;_tmp1E4.f1=f1;
_tmp1E4.f2=f2;_tmp1E4;});void*_tmp1ED;void*_tmp1EF;void*_tmp1F1;struct Cyc_List_List*
_tmp1F3;struct Cyc_Dict_Dict*_tmp1F5;void*_tmp1F7;struct Cyc_List_List*_tmp1F9;
struct Cyc_Dict_Dict*_tmp1FB;_LL487: _LL494: _tmp1ED=_tmp1E5.f1;if(_tmp1ED == (void*)
Cyc_CfFlowInfo_BottomFL){goto _LL488;}else{goto _LL489;}_LL489: _LL496: _tmp1EF=
_tmp1E5.f2;if(_tmp1EF == (void*)Cyc_CfFlowInfo_BottomFL){goto _LL490;}else{goto
_LL491;}_LL491: _LL504: _tmp1F7=_tmp1E5.f1;if((unsigned int)_tmp1F7 > 1?*((int*)
_tmp1F7)== Cyc_CfFlowInfo_ReachableFL: 0){_LL508: _tmp1FB=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1F7)->f1;goto _LL506;_LL506: _tmp1F9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp1F7)->f2;goto _LL498;}else{goto _LL486;}_LL498: _tmp1F1=_tmp1E5.f2;if((
unsigned int)_tmp1F1 > 1?*((int*)_tmp1F1)== Cyc_CfFlowInfo_ReachableFL: 0){_LL502:
_tmp1F5=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F1)->f1;goto _LL500;
_LL500: _tmp1F3=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp1F1)->f2;goto
_LL492;}else{goto _LL486;}_LL488: return f2;_LL490: return f1;_LL492: if(_tmp1FB == 
_tmp1F5? _tmp1F9 == _tmp1F3: 0){return f1;}if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,
f2)){return f2;}if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1)){return f1;}{struct
_RegionHandle _tmp1FD=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1FD;
_push_region(rgn);{struct Cyc_CfFlowInfo_JoinEnv _tmp1FE=({struct Cyc_CfFlowInfo_JoinEnv
_tmp204;_tmp204.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp205=_region_malloc(rgn,
sizeof(struct Cyc_CfFlowInfo_EscPile));_tmp205->rgn=rgn;_tmp205->places=0;_tmp205;});
_tmp204.d1=_tmp1FB;_tmp204.d2=_tmp1F5;_tmp204;});struct Cyc_Dict_Dict*_tmp1FF=((
struct Cyc_Dict_Dict*(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),
struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))
Cyc_Dict_intersect_c)(Cyc_CfFlowInfo_join_absRval,& _tmp1FE,_tmp1FB,_tmp1F5);
struct Cyc_List_List*_tmp200=Cyc_CfFlowInfo_join_relns(_tmp1F9,_tmp1F3);void*
_tmp203=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp201=_cycalloc(
sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp201[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp202;_tmp202.tag=Cyc_CfFlowInfo_ReachableFL;_tmp202.f1=Cyc_CfFlowInfo_escape_these(
_tmp1FE.pile,all_changed,_tmp1FF);_tmp202.f2=_tmp200;_tmp202;});_tmp201;});
_npop_handler(0);return _tmp203;};_pop_region(rgn);}_LL486:;}}static void*Cyc_CfFlowInfo_after_absRval_d(
struct Cyc_CfFlowInfo_AfterEnv*,struct _tagged_arr*,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(
struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){int changed1=env->changed == 
Cyc_CfFlowInfo_One? 1:((int(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))
Cyc_Set_member)(env->chg1,env->curr_place);int changed2=env->changed == Cyc_CfFlowInfo_Two?
1:((int(*)(struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_member)(
env->chg2,env->curr_place);if(changed1? changed2: 0){return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*
env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}
if(changed1){if(! Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2)){
return r1;}env->changed=Cyc_CfFlowInfo_One;}if(changed2){if(! Cyc_CfFlowInfo_prefix_of_member(
env->curr_place,env->chg1)){return r2;}env->changed=Cyc_CfFlowInfo_Two;}{struct
_tuple5 _tmp207=({struct _tuple5 _tmp206;_tmp206.f1=r1;_tmp206.f2=r2;_tmp206;});
void*_tmp20D;struct Cyc_Dict_Dict*_tmp20F;void*_tmp211;struct Cyc_Dict_Dict*
_tmp213;_LL521: _LL530: _tmp211=_tmp207.f1;if((unsigned int)_tmp211 > 3?*((int*)
_tmp211)== Cyc_CfFlowInfo_Aggregate: 0){_LL532: _tmp213=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp211)->f1;goto _LL526;}else{goto _LL523;}_LL526: _tmp20D=_tmp207.f2;if((
unsigned int)_tmp20D > 3?*((int*)_tmp20D)== Cyc_CfFlowInfo_Aggregate: 0){_LL528:
_tmp20F=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp20D)->f1;goto _LL522;}else{
goto _LL523;}_LL523: goto _LL524;_LL522: {struct Cyc_Dict_Dict*_tmp215=((struct Cyc_Dict_Dict*(*)(
void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,struct _tagged_arr*,void*,void*),struct
Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(
Cyc_CfFlowInfo_after_absRval_d,env,_tmp213,_tmp20F);if(_tmp215 == _tmp213){return
r1;}if(_tmp215 == _tmp20F){return r2;}return(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp216=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));_tmp216[0]=({
struct Cyc_CfFlowInfo_Aggregate_struct _tmp217;_tmp217.tag=Cyc_CfFlowInfo_Aggregate;
_tmp217.f1=_tmp215;_tmp217;});_tmp216;});}_LL524:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp218=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp218[0]=({struct
Cyc_Core_Impossible_struct _tmp219;_tmp219.tag=Cyc_Core_Impossible;_tmp219.f1=
_tag_arr("after_pathinfo -- non-aggregates!",sizeof(unsigned char),34);_tmp219;});
_tmp218;}));_LL520:;}}static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*
env,struct _tagged_arr*key,void*r1,void*r2){if(r1 == r2){return r1;}{struct Cyc_List_List**
_tmp21A=env->last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged _tmp21B=env->changed;*
env->last_field_cell=({struct Cyc_List_List*_tmp21C=_cycalloc(sizeof(struct Cyc_List_List));
_tmp21C->hd=key;_tmp21C->tl=0;_tmp21C;});env->last_field_cell=&((struct Cyc_List_List*)
_check_null(*env->last_field_cell))->tl;{void*_tmp21D=Cyc_CfFlowInfo_after_absRval(
env,r1,r2);env->last_field_cell=_tmp21A;((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=
0;env->changed=_tmp21B;return _tmp21D;}}}static void*Cyc_CfFlowInfo_after_root(
struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){if(r1 == r2){return
r1;}*env->curr_place=({struct Cyc_CfFlowInfo_Place _tmp21E;_tmp21E.root=(void*)
root;_tmp21E.fields=0;_tmp21E;});env->last_field_cell=&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
void*Cyc_CfFlowInfo_after_flow(struct Cyc_Set_Set**all_changed,void*f1,void*f2,
struct Cyc_Set_Set*chg1,struct Cyc_Set_Set*chg2){static struct Cyc_Absyn_Const_e_struct
dummy_rawexp={0,(void*)((void*)0)};static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((
void*)& dummy_rawexp),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_CfFlowInfo_MallocPt_struct
dummy_root={1,& dummy_exp};if(f1 == f2){return f1;}{struct _tuple5 _tmp220=({struct
_tuple5 _tmp21F;_tmp21F.f1=f1;_tmp21F.f2=f2;_tmp21F;});void*_tmp228;void*_tmp22A;
void*_tmp22C;struct Cyc_List_List*_tmp22E;struct Cyc_Dict_Dict*_tmp230;void*
_tmp232;struct Cyc_List_List*_tmp234;struct Cyc_Dict_Dict*_tmp236;_LL546: _LL553:
_tmp228=_tmp220.f1;if(_tmp228 == (void*)Cyc_CfFlowInfo_BottomFL){goto _LL547;}
else{goto _LL548;}_LL548: _LL555: _tmp22A=_tmp220.f2;if(_tmp22A == (void*)Cyc_CfFlowInfo_BottomFL){
goto _LL549;}else{goto _LL550;}_LL550: _LL563: _tmp232=_tmp220.f1;if((unsigned int)
_tmp232 > 1?*((int*)_tmp232)== Cyc_CfFlowInfo_ReachableFL: 0){_LL567: _tmp236=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp232)->f1;goto _LL565;_LL565: _tmp234=((
struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp232)->f2;goto _LL557;}else{goto
_LL545;}_LL557: _tmp22C=_tmp220.f2;if((unsigned int)_tmp22C > 1?*((int*)_tmp22C)== 
Cyc_CfFlowInfo_ReachableFL: 0){_LL561: _tmp230=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp22C)->f1;goto _LL559;_LL559: _tmp22E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp22C)->f2;goto _LL551;}else{goto _LL545;}_LL547: goto _LL549;_LL549: return(void*)
Cyc_CfFlowInfo_BottomFL;_LL551: if(_tmp236 == _tmp230? _tmp234 == _tmp22E: 0){return
f1;}{struct _RegionHandle _tmp238=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp238;_push_region(rgn);{struct Cyc_CfFlowInfo_Place*_tmp239=({struct Cyc_CfFlowInfo_Place*
_tmp243=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_tmp243->root=(void*)((
void*)& dummy_root);_tmp243->fields=0;_tmp243;});struct Cyc_CfFlowInfo_AfterEnv
_tmp23A=({struct Cyc_CfFlowInfo_AfterEnv _tmp240;_tmp240.joinenv=({struct Cyc_CfFlowInfo_JoinEnv
_tmp241;_tmp241.pile=({struct Cyc_CfFlowInfo_EscPile*_tmp242=_region_malloc(rgn,
sizeof(struct Cyc_CfFlowInfo_EscPile));_tmp242->rgn=rgn;_tmp242->places=0;_tmp242;});
_tmp241.d1=_tmp236;_tmp241.d2=_tmp230;_tmp241;});_tmp240.chg1=chg1;_tmp240.chg2=
chg2;_tmp240.curr_place=_tmp239;_tmp240.last_field_cell=& _tmp239->fields;_tmp240.changed=
Cyc_CfFlowInfo_Neither;_tmp240;});struct Cyc_Dict_Dict*_tmp23B=((struct Cyc_Dict_Dict*(*)(
void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*
env,struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,&
_tmp23A,_tmp236,_tmp230);struct Cyc_List_List*_tmp23C=Cyc_CfFlowInfo_join_relns(
_tmp234,_tmp22E);void*_tmp23F=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp23D=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp23D[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp23E;_tmp23E.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp23E.f1=Cyc_CfFlowInfo_escape_these((_tmp23A.joinenv).pile,all_changed,
_tmp23B);_tmp23E.f2=_tmp23C;_tmp23E;});_tmp23D;});_npop_handler(0);return _tmp23F;};
_pop_region(rgn);}_LL545:;}}static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*
ignore,void*r1,void*r2){if(r1 == r2){return 1;}{struct _tuple5 _tmp247=({struct
_tuple5 _tmp246;_tmp246.f1=r1;_tmp246.f2=r2;_tmp246;});void*_tmp25D;struct Cyc_CfFlowInfo_Place*
_tmp25F;void*_tmp261;struct Cyc_CfFlowInfo_Place*_tmp263;void*_tmp265;void*
_tmp267;void*_tmp269;struct Cyc_Dict_Dict*_tmp26B;void*_tmp26D;struct Cyc_Dict_Dict*
_tmp26F;void*_tmp271;void*_tmp273;void*_tmp275;void*_tmp277;void*_tmp279;void*
_tmp27B;_LL585: _LL610: _tmp261=_tmp247.f1;if((unsigned int)_tmp261 > 3?*((int*)
_tmp261)== Cyc_CfFlowInfo_AddressOf: 0){_LL612: _tmp263=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp261)->f1;goto _LL606;}else{goto _LL587;}_LL606: _tmp25D=_tmp247.f2;if((
unsigned int)_tmp25D > 3?*((int*)_tmp25D)== Cyc_CfFlowInfo_AddressOf: 0){_LL608:
_tmp25F=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp25D)->f1;goto _LL586;}else{
goto _LL587;}_LL587: _LL614: _tmp265=_tmp247.f1;if((unsigned int)_tmp265 > 3?*((int*)
_tmp265)== Cyc_CfFlowInfo_AddressOf: 0){goto _LL588;}else{goto _LL589;}_LL589:
_LL616: _tmp267=_tmp247.f2;if((unsigned int)_tmp267 > 3?*((int*)_tmp267)== Cyc_CfFlowInfo_AddressOf:
0){goto _LL590;}else{goto _LL591;}_LL591: _LL622: _tmp26D=_tmp247.f1;if((
unsigned int)_tmp26D > 3?*((int*)_tmp26D)== Cyc_CfFlowInfo_Aggregate: 0){_LL624:
_tmp26F=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp26D)->f1;goto _LL618;}else{
goto _LL593;}_LL618: _tmp269=_tmp247.f2;if((unsigned int)_tmp269 > 3?*((int*)
_tmp269)== Cyc_CfFlowInfo_Aggregate: 0){_LL620: _tmp26B=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp269)->f1;goto _LL592;}else{goto _LL593;}_LL593: _LL626: _tmp271=_tmp247.f2;if(
_tmp271 == (void*)Cyc_CfFlowInfo_NotZeroThis){goto _LL594;}else{goto _LL595;}_LL595:
_LL628: _tmp273=_tmp247.f2;if(_tmp273 == (void*)Cyc_CfFlowInfo_Zero){goto _LL596;}
else{goto _LL597;}_LL597: _LL630: _tmp275=_tmp247.f2;if(_tmp275 == (void*)Cyc_CfFlowInfo_NotZeroAll){
goto _LL598;}else{goto _LL599;}_LL599: _LL634: _tmp279=_tmp247.f1;if((unsigned int)
_tmp279 > 3?*((int*)_tmp279)== Cyc_CfFlowInfo_Esc: 0){goto _LL632;}else{goto _LL601;}
_LL632: _tmp277=_tmp247.f2;if((unsigned int)_tmp277 > 3?*((int*)_tmp277)== Cyc_CfFlowInfo_Esc:
0){goto _LL600;}else{goto _LL601;}_LL601: _LL636: _tmp27B=_tmp247.f1;if((
unsigned int)_tmp27B > 3?*((int*)_tmp27B)== Cyc_CfFlowInfo_Esc: 0){goto _LL602;}
else{goto _LL603;}_LL603: goto _LL604;_LL586: return Cyc_CfFlowInfo_place_cmp(_tmp263,
_tmp25F)== 0;_LL588: goto _LL590;_LL590: return 0;_LL592: return _tmp26F == _tmp26B? 1:((
int(*)(int(*f)(struct _tagged_arr*,void*,void*),struct Cyc_Dict_Dict*d1,struct Cyc_Dict_Dict*
d2))Cyc_Dict_forall_intersect)((int(*)(struct _tagged_arr*ignore,void*r1,void*r2))
Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp26F,_tmp26B);_LL594: return r1 == (void*)
Cyc_CfFlowInfo_NotZeroAll;_LL596: goto _LL598;_LL598: return 0;_LL600: goto _LL584;
_LL602: return 0;_LL604: goto _LL584;_LL584:;}{struct _tuple5 _tmp27E=({struct _tuple5
_tmp27D;_tmp27D.f1=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp27D.f2=Cyc_CfFlowInfo_initlevel_approx(
r2);_tmp27D;});void*_tmp28A;void*_tmp28C;void*_tmp28E;void*_tmp290;void*_tmp292;
void*_tmp294;_LL640: _LL653: _tmp28C=_tmp27E.f1;if(_tmp28C == (void*)Cyc_CfFlowInfo_AllIL){
goto _LL651;}else{goto _LL642;}_LL651: _tmp28A=_tmp27E.f2;if(_tmp28A == (void*)Cyc_CfFlowInfo_AllIL){
goto _LL641;}else{goto _LL642;}_LL642: _LL655: _tmp28E=_tmp27E.f2;if(_tmp28E == (void*)
Cyc_CfFlowInfo_NoneIL){goto _LL643;}else{goto _LL644;}_LL644: _LL657: _tmp290=
_tmp27E.f1;if(_tmp290 == (void*)Cyc_CfFlowInfo_NoneIL){goto _LL645;}else{goto
_LL646;}_LL646: _LL659: _tmp292=_tmp27E.f2;if(_tmp292 == (void*)Cyc_CfFlowInfo_ThisIL){
goto _LL647;}else{goto _LL648;}_LL648: _LL661: _tmp294=_tmp27E.f1;if(_tmp294 == (void*)
Cyc_CfFlowInfo_ThisIL){goto _LL649;}else{goto _LL639;}_LL641: return 1;_LL643: return
1;_LL645: return 0;_LL647: return 1;_LL649: return 0;_LL639:;}}int Cyc_CfFlowInfo_relns_approx(
struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){if(r1s == r2s){return 1;}for(0;
r1s != 0;r1s=r1s->tl){struct Cyc_CfFlowInfo_Reln*_tmp296=(struct Cyc_CfFlowInfo_Reln*)
r1s->hd;int found=0;{struct Cyc_List_List*_tmp297=r2s;for(0;_tmp297 != 0;_tmp297=
_tmp297->tl){struct Cyc_CfFlowInfo_Reln*_tmp298=(struct Cyc_CfFlowInfo_Reln*)
_tmp297->hd;if(_tmp296 == _tmp298? 1:(_tmp296->vd == _tmp298->vd? Cyc_CfFlowInfo_same_relop((
void*)_tmp296->rop,(void*)_tmp298->rop): 0)){found=1;break;}}}if(! found){return 0;}}
return 1;}int Cyc_CfFlowInfo_flow_lessthan_approx(void*f1,void*f2){if(f1 == f2){
return 1;}{struct _tuple5 _tmp29A=({struct _tuple5 _tmp299;_tmp299.f1=f1;_tmp299.f2=
f2;_tmp299;});void*_tmp2A2;void*_tmp2A4;void*_tmp2A6;struct Cyc_List_List*_tmp2A8;
struct Cyc_Dict_Dict*_tmp2AA;void*_tmp2AC;struct Cyc_List_List*_tmp2AE;struct Cyc_Dict_Dict*
_tmp2B0;_LL668: _LL675: _tmp2A2=_tmp29A.f1;if(_tmp2A2 == (void*)Cyc_CfFlowInfo_BottomFL){
goto _LL669;}else{goto _LL670;}_LL670: _LL677: _tmp2A4=_tmp29A.f2;if(_tmp2A4 == (void*)
Cyc_CfFlowInfo_BottomFL){goto _LL671;}else{goto _LL672;}_LL672: _LL685: _tmp2AC=
_tmp29A.f1;if((unsigned int)_tmp2AC > 1?*((int*)_tmp2AC)== Cyc_CfFlowInfo_ReachableFL:
0){_LL689: _tmp2B0=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2AC)->f1;goto
_LL687;_LL687: _tmp2AE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp2AC)->f2;
goto _LL679;}else{goto _LL667;}_LL679: _tmp2A6=_tmp29A.f2;if((unsigned int)_tmp2A6 > 
1?*((int*)_tmp2A6)== Cyc_CfFlowInfo_ReachableFL: 0){_LL683: _tmp2AA=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2A6)->f1;goto _LL681;_LL681: _tmp2A8=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp2A6)->f2;goto _LL673;}else{goto _LL667;}_LL669: return 1;_LL671: return 0;_LL673:
if(_tmp2B0 == _tmp2AA? _tmp2AE == _tmp2A8: 0){return 1;}return Cyc_Dict_forall_intersect(
Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp2B0,_tmp2AA)? Cyc_CfFlowInfo_relns_approx(
_tmp2AE,_tmp2A8): 0;_LL667:;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_var(
struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){struct Cyc_List_List*p;int found=
0;for(p=rs;! found? p != 0: 0;p=p->tl){struct Cyc_CfFlowInfo_Reln*_tmp2B2=(struct Cyc_CfFlowInfo_Reln*)
p->hd;if(_tmp2B2->vd == v){found=1;break;}{void*_tmp2B3=(void*)_tmp2B2->rop;
struct Cyc_Absyn_Vardecl*_tmp2BD;struct Cyc_Absyn_Vardecl*_tmp2BF;struct Cyc_Absyn_Vardecl*
_tmp2C1;_LL693: if(*((int*)_tmp2B3)== Cyc_CfFlowInfo_LessVar){_LL702: _tmp2BD=((
struct Cyc_CfFlowInfo_LessVar_struct*)_tmp2B3)->f1;goto _LL694;}else{goto _LL695;}
_LL695: if(*((int*)_tmp2B3)== Cyc_CfFlowInfo_LessSize){_LL704: _tmp2BF=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp2B3)->f1;goto _LL696;}else{goto _LL697;}_LL697: if(*((int*)_tmp2B3)== Cyc_CfFlowInfo_LessEqSize){
_LL706: _tmp2C1=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp2B3)->f1;goto _LL698;}
else{goto _LL699;}_LL699: goto _LL700;_LL694: _tmp2BF=_tmp2BD;goto _LL696;_LL696:
_tmp2C1=_tmp2BF;goto _LL698;_LL698: found=1;continue;_LL700: goto _LL692;_LL692:;}}
if(! found){return rs;}{struct Cyc_List_List*_tmp2C3=0;for(p=rs;p != 0;p=p->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2C4=(struct Cyc_CfFlowInfo_Reln*)p->hd;if(_tmp2C4->vd
!= v){{void*_tmp2C5=(void*)_tmp2C4->rop;struct Cyc_Absyn_Vardecl*_tmp2CF;struct
Cyc_Absyn_Vardecl*_tmp2D1;struct Cyc_Absyn_Vardecl*_tmp2D3;_LL711: if(*((int*)
_tmp2C5)== Cyc_CfFlowInfo_LessVar){_LL720: _tmp2CF=((struct Cyc_CfFlowInfo_LessVar_struct*)
_tmp2C5)->f1;goto _LL712;}else{goto _LL713;}_LL713: if(*((int*)_tmp2C5)== Cyc_CfFlowInfo_LessSize){
_LL722: _tmp2D1=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp2C5)->f1;goto _LL714;}
else{goto _LL715;}_LL715: if(*((int*)_tmp2C5)== Cyc_CfFlowInfo_LessEqSize){_LL724:
_tmp2D3=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp2C5)->f1;goto _LL716;}else{
goto _LL717;}_LL717: goto _LL718;_LL712: _tmp2D1=_tmp2CF;goto _LL714;_LL714: _tmp2D3=
_tmp2D1;goto _LL716;_LL716: if(v == _tmp2D3){continue;}else{goto _LL710;}_LL718: goto
_LL710;;_LL710:;}_tmp2C3=({struct Cyc_List_List*_tmp2D5=_cycalloc(sizeof(struct
Cyc_List_List));_tmp2D5->hd=_tmp2C4;_tmp2D5->tl=_tmp2C3;_tmp2D5;});}}return
_tmp2C3;}}struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*r,
struct Cyc_Absyn_Exp*e){{void*_tmp2D6=(void*)e->r;void*_tmp2E2;struct Cyc_Absyn_Vardecl*
_tmp2E4;void*_tmp2E6;struct Cyc_Absyn_Vardecl*_tmp2E8;void*_tmp2EA;struct Cyc_Absyn_Vardecl*
_tmp2EC;void*_tmp2EE;struct Cyc_Absyn_Vardecl*_tmp2F0;_LL728: if(*((int*)_tmp2D6)
== Cyc_Absyn_Var_e){_LL739: _tmp2E2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D6)->f2;if((unsigned int)_tmp2E2 > 1?*((int*)_tmp2E2)== Cyc_Absyn_Global_b: 0){
_LL741: _tmp2E4=((struct Cyc_Absyn_Global_b_struct*)_tmp2E2)->f1;goto _LL729;}else{
goto _LL730;}}else{goto _LL730;}_LL730: if(*((int*)_tmp2D6)== Cyc_Absyn_Var_e){
_LL743: _tmp2E6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2D6)->f2;if((
unsigned int)_tmp2E6 > 1?*((int*)_tmp2E6)== Cyc_Absyn_Param_b: 0){_LL745: _tmp2E8=((
struct Cyc_Absyn_Param_b_struct*)_tmp2E6)->f1;goto _LL731;}else{goto _LL732;}}else{
goto _LL732;}_LL732: if(*((int*)_tmp2D6)== Cyc_Absyn_Var_e){_LL747: _tmp2EA=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2D6)->f2;if((unsigned int)_tmp2EA > 1?*((int*)
_tmp2EA)== Cyc_Absyn_Local_b: 0){_LL749: _tmp2EC=((struct Cyc_Absyn_Local_b_struct*)
_tmp2EA)->f1;goto _LL733;}else{goto _LL734;}}else{goto _LL734;}_LL734: if(*((int*)
_tmp2D6)== Cyc_Absyn_Var_e){_LL751: _tmp2EE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D6)->f2;if((unsigned int)_tmp2EE > 1?*((int*)_tmp2EE)== Cyc_Absyn_Pat_b: 0){
_LL753: _tmp2F0=((struct Cyc_Absyn_Pat_b_struct*)_tmp2EE)->f1;goto _LL735;}else{
goto _LL736;}}else{goto _LL736;}_LL736: goto _LL737;_LL729: _tmp2E8=_tmp2E4;goto
_LL731;_LL731: _tmp2EC=_tmp2E8;goto _LL733;_LL733: _tmp2F0=_tmp2EC;goto _LL735;
_LL735: if(! _tmp2F0->escapes){return Cyc_CfFlowInfo_reln_kill_var(r,_tmp2F0);}goto
_LL727;_LL737: goto _LL727;_LL727:;}return r;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){if(v->escapes){
return r;}r=Cyc_CfFlowInfo_reln_kill_var(r,v);{void*_tmp2F2=(void*)e->r;struct Cyc_Absyn_MallocInfo
_tmp2F8;int _tmp2FA;struct Cyc_Absyn_Exp*_tmp2FC;_LL756: if(*((int*)_tmp2F2)== Cyc_Absyn_Malloc_e){
_LL761: _tmp2F8=((struct Cyc_Absyn_Malloc_e_struct*)_tmp2F2)->f1;_LL765: _tmp2FC=
_tmp2F8.num_elts;goto _LL763;_LL763: _tmp2FA=_tmp2F8.fat_result;if(_tmp2FA == 1){
goto _LL757;}else{goto _LL758;}}else{goto _LL758;}_LL758: goto _LL759;_LL757:
malloc_loop: {void*_tmp2FE=(void*)_tmp2FC->r;struct Cyc_Absyn_Exp*_tmp30C;void*
_tmp30E;struct Cyc_Absyn_Vardecl*_tmp310;void*_tmp312;struct Cyc_Absyn_Vardecl*
_tmp314;void*_tmp316;struct Cyc_Absyn_Vardecl*_tmp318;void*_tmp31A;struct Cyc_Absyn_Vardecl*
_tmp31C;_LL768: if(*((int*)_tmp2FE)== Cyc_Absyn_Cast_e){_LL781: _tmp30C=((struct
Cyc_Absyn_Cast_e_struct*)_tmp2FE)->f2;goto _LL769;}else{goto _LL770;}_LL770: if(*((
int*)_tmp2FE)== Cyc_Absyn_Var_e){_LL783: _tmp30E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2FE)->f2;if((unsigned int)_tmp30E > 1?*((int*)_tmp30E)== Cyc_Absyn_Pat_b: 0){
_LL785: _tmp310=((struct Cyc_Absyn_Pat_b_struct*)_tmp30E)->f1;goto _LL771;}else{
goto _LL772;}}else{goto _LL772;}_LL772: if(*((int*)_tmp2FE)== Cyc_Absyn_Var_e){
_LL787: _tmp312=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FE)->f2;if((
unsigned int)_tmp312 > 1?*((int*)_tmp312)== Cyc_Absyn_Local_b: 0){_LL789: _tmp314=((
struct Cyc_Absyn_Local_b_struct*)_tmp312)->f1;goto _LL773;}else{goto _LL774;}}else{
goto _LL774;}_LL774: if(*((int*)_tmp2FE)== Cyc_Absyn_Var_e){_LL791: _tmp316=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2FE)->f2;if((unsigned int)_tmp316 > 1?*((int*)
_tmp316)== Cyc_Absyn_Param_b: 0){_LL793: _tmp318=((struct Cyc_Absyn_Param_b_struct*)
_tmp316)->f1;goto _LL775;}else{goto _LL776;}}else{goto _LL776;}_LL776: if(*((int*)
_tmp2FE)== Cyc_Absyn_Var_e){_LL795: _tmp31A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2FE)->f2;if((unsigned int)_tmp31A > 1?*((int*)_tmp31A)== Cyc_Absyn_Global_b: 0){
_LL797: _tmp31C=((struct Cyc_Absyn_Global_b_struct*)_tmp31A)->f1;goto _LL777;}else{
goto _LL778;}}else{goto _LL778;}_LL778: goto _LL779;_LL769: _tmp2FC=_tmp30C;goto
malloc_loop;_LL771: _tmp314=_tmp310;goto _LL773;_LL773: _tmp318=_tmp314;goto _LL775;
_LL775: _tmp31C=_tmp318;goto _LL777;_LL777: if(_tmp31C->escapes){return r;}return({
struct Cyc_List_List*_tmp31E=_cycalloc(sizeof(struct Cyc_List_List));_tmp31E->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp31F=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp31F->vd=_tmp31C;_tmp31F->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp320=_cycalloc(sizeof(struct Cyc_CfFlowInfo_LessEqSize_struct));_tmp320[0]=({
struct Cyc_CfFlowInfo_LessEqSize_struct _tmp321;_tmp321.tag=Cyc_CfFlowInfo_LessEqSize;
_tmp321.f1=v;_tmp321;});_tmp320;}));_tmp31F;});_tmp31E->tl=r;_tmp31E;});_LL779:
return r;_LL767:;}_LL759: goto _LL755;_LL755:;}{void*_tmp322=Cyc_Tcutil_compress((
void*)v->type);_LL804: if((unsigned int)_tmp322 > 3?*((int*)_tmp322)== Cyc_Absyn_IntType:
0){goto _LL805;}else{goto _LL806;}_LL806: goto _LL807;_LL805: goto _LL803;_LL807:
return r;_LL803:;}loop: {void*_tmp328=(void*)e->r;struct Cyc_Absyn_Exp*_tmp334;
void*_tmp336;int _tmp338;struct Cyc_List_List*_tmp33A;struct Cyc_List_List _tmp33C;
struct Cyc_List_List*_tmp33D;struct Cyc_List_List _tmp33F;struct Cyc_Absyn_Exp*
_tmp340;void*_tmp342;struct Cyc_List_List*_tmp344;struct Cyc_List_List _tmp346;
struct Cyc_Absyn_Exp*_tmp347;void*_tmp349;_LL810: if(*((int*)_tmp328)== Cyc_Absyn_Cast_e){
_LL821: _tmp334=((struct Cyc_Absyn_Cast_e_struct*)_tmp328)->f2;goto _LL811;}else{
goto _LL812;}_LL812: if(*((int*)_tmp328)== Cyc_Absyn_Const_e){_LL823: _tmp336=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp328)->f1;if((unsigned int)_tmp336 > 1?*((int*)
_tmp336)== Cyc_Absyn_Int_c: 0){_LL825: _tmp338=((struct Cyc_Absyn_Int_c_struct*)
_tmp336)->f2;goto _LL813;}else{goto _LL814;}}else{goto _LL814;}_LL814: if(*((int*)
_tmp328)== Cyc_Absyn_Primop_e){_LL835: _tmp342=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp328)->f1;if(_tmp342 == (void*)Cyc_Absyn_Mod){goto _LL827;}else{goto _LL816;}
_LL827: _tmp33A=((struct Cyc_Absyn_Primop_e_struct*)_tmp328)->f2;if(_tmp33A == 0){
goto _LL816;}else{_tmp33C=*_tmp33A;_LL830: _tmp33D=_tmp33C.tl;if(_tmp33D == 0){goto
_LL816;}else{_tmp33F=*_tmp33D;_LL833: _tmp340=(struct Cyc_Absyn_Exp*)_tmp33F.hd;
goto _LL815;}}}else{goto _LL816;}_LL816: if(*((int*)_tmp328)== Cyc_Absyn_Primop_e){
_LL842: _tmp349=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp328)->f1;if(_tmp349
== (void*)Cyc_Absyn_Size){goto _LL837;}else{goto _LL818;}_LL837: _tmp344=((struct
Cyc_Absyn_Primop_e_struct*)_tmp328)->f2;if(_tmp344 == 0){goto _LL818;}else{_tmp346=*
_tmp344;_LL840: _tmp347=(struct Cyc_Absyn_Exp*)_tmp346.hd;goto _LL817;}}else{goto
_LL818;}_LL818: goto _LL819;_LL811: e=_tmp334;goto loop;_LL813: return({struct Cyc_List_List*
_tmp34B=_cycalloc(sizeof(struct Cyc_List_List));_tmp34B->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp34C=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));_tmp34C->vd=v;_tmp34C->rop=(
void*)((void*)({struct Cyc_CfFlowInfo_EqualConst_struct*_tmp34D=_cycalloc_atomic(
sizeof(struct Cyc_CfFlowInfo_EqualConst_struct));_tmp34D[0]=({struct Cyc_CfFlowInfo_EqualConst_struct
_tmp34E;_tmp34E.tag=Cyc_CfFlowInfo_EqualConst;_tmp34E.f1=(unsigned int)_tmp338;
_tmp34E;});_tmp34D;}));_tmp34C;});_tmp34B->tl=r;_tmp34B;});_LL815:{void*_tmp34F=(
void*)_tmp340->r;struct Cyc_List_List*_tmp355;struct Cyc_List_List _tmp357;struct
Cyc_Absyn_Exp*_tmp358;void*_tmp35A;_LL849: if(*((int*)_tmp34F)== Cyc_Absyn_Primop_e){
_LL859: _tmp35A=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp34F)->f1;if(_tmp35A
== (void*)Cyc_Absyn_Size){goto _LL854;}else{goto _LL851;}_LL854: _tmp355=((struct
Cyc_Absyn_Primop_e_struct*)_tmp34F)->f2;if(_tmp355 == 0){goto _LL851;}else{_tmp357=*
_tmp355;_LL857: _tmp358=(struct Cyc_Absyn_Exp*)_tmp357.hd;goto _LL850;}}else{goto
_LL851;}_LL851: goto _LL852;_LL850:{void*_tmp35C=(void*)_tmp358->r;void*_tmp368;
struct Cyc_Absyn_Vardecl*_tmp36A;void*_tmp36C;struct Cyc_Absyn_Vardecl*_tmp36E;
void*_tmp370;struct Cyc_Absyn_Vardecl*_tmp372;void*_tmp374;struct Cyc_Absyn_Vardecl*
_tmp376;_LL862: if(*((int*)_tmp35C)== Cyc_Absyn_Var_e){_LL873: _tmp368=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp35C)->f2;if((unsigned int)_tmp368 > 1?*((int*)
_tmp368)== Cyc_Absyn_Global_b: 0){_LL875: _tmp36A=((struct Cyc_Absyn_Global_b_struct*)
_tmp368)->f1;goto _LL863;}else{goto _LL864;}}else{goto _LL864;}_LL864: if(*((int*)
_tmp35C)== Cyc_Absyn_Var_e){_LL877: _tmp36C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp35C)->f2;if((unsigned int)_tmp36C > 1?*((int*)_tmp36C)== Cyc_Absyn_Local_b: 0){
_LL879: _tmp36E=((struct Cyc_Absyn_Local_b_struct*)_tmp36C)->f1;goto _LL865;}else{
goto _LL866;}}else{goto _LL866;}_LL866: if(*((int*)_tmp35C)== Cyc_Absyn_Var_e){
_LL881: _tmp370=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp35C)->f2;if((
unsigned int)_tmp370 > 1?*((int*)_tmp370)== Cyc_Absyn_Param_b: 0){_LL883: _tmp372=((
struct Cyc_Absyn_Param_b_struct*)_tmp370)->f1;goto _LL867;}else{goto _LL868;}}else{
goto _LL868;}_LL868: if(*((int*)_tmp35C)== Cyc_Absyn_Var_e){_LL885: _tmp374=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp35C)->f2;if((unsigned int)_tmp374 > 1?*((int*)
_tmp374)== Cyc_Absyn_Pat_b: 0){_LL887: _tmp376=((struct Cyc_Absyn_Pat_b_struct*)
_tmp374)->f1;goto _LL869;}else{goto _LL870;}}else{goto _LL870;}_LL870: goto _LL871;
_LL863: _tmp36E=_tmp36A;goto _LL865;_LL865: _tmp372=_tmp36E;goto _LL867;_LL867:
_tmp376=_tmp372;goto _LL869;_LL869: if(_tmp376->escapes){return r;}return({struct
Cyc_List_List*_tmp378=_cycalloc(sizeof(struct Cyc_List_List));_tmp378->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp379=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp379->vd=v;_tmp379->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp37A=_cycalloc(sizeof(struct Cyc_CfFlowInfo_LessSize_struct));_tmp37A[0]=({
struct Cyc_CfFlowInfo_LessSize_struct _tmp37B;_tmp37B.tag=Cyc_CfFlowInfo_LessSize;
_tmp37B.f1=_tmp376;_tmp37B;});_tmp37A;}));_tmp379;});_tmp378->tl=r;_tmp378;});
_LL871: goto _LL861;_LL861:;}goto _LL848;_LL852: goto _LL848;_LL848:;}goto _LL809;
_LL817:{void*_tmp37C=(void*)_tmp347->r;void*_tmp388;struct Cyc_Absyn_Vardecl*
_tmp38A;void*_tmp38C;struct Cyc_Absyn_Vardecl*_tmp38E;void*_tmp390;struct Cyc_Absyn_Vardecl*
_tmp392;void*_tmp394;struct Cyc_Absyn_Vardecl*_tmp396;_LL894: if(*((int*)_tmp37C)
== Cyc_Absyn_Var_e){_LL905: _tmp388=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp37C)->f2;if((unsigned int)_tmp388 > 1?*((int*)_tmp388)== Cyc_Absyn_Global_b: 0){
_LL907: _tmp38A=((struct Cyc_Absyn_Global_b_struct*)_tmp388)->f1;goto _LL895;}else{
goto _LL896;}}else{goto _LL896;}_LL896: if(*((int*)_tmp37C)== Cyc_Absyn_Var_e){
_LL909: _tmp38C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp37C)->f2;if((
unsigned int)_tmp38C > 1?*((int*)_tmp38C)== Cyc_Absyn_Local_b: 0){_LL911: _tmp38E=((
struct Cyc_Absyn_Local_b_struct*)_tmp38C)->f1;goto _LL897;}else{goto _LL898;}}else{
goto _LL898;}_LL898: if(*((int*)_tmp37C)== Cyc_Absyn_Var_e){_LL913: _tmp390=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp37C)->f2;if((unsigned int)_tmp390 > 1?*((int*)
_tmp390)== Cyc_Absyn_Param_b: 0){_LL915: _tmp392=((struct Cyc_Absyn_Param_b_struct*)
_tmp390)->f1;goto _LL899;}else{goto _LL900;}}else{goto _LL900;}_LL900: if(*((int*)
_tmp37C)== Cyc_Absyn_Var_e){_LL917: _tmp394=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp37C)->f2;if((unsigned int)_tmp394 > 1?*((int*)_tmp394)== Cyc_Absyn_Pat_b: 0){
_LL919: _tmp396=((struct Cyc_Absyn_Pat_b_struct*)_tmp394)->f1;goto _LL901;}else{
goto _LL902;}}else{goto _LL902;}_LL902: goto _LL903;_LL895: _tmp38E=_tmp38A;goto
_LL897;_LL897: _tmp392=_tmp38E;goto _LL899;_LL899: _tmp396=_tmp392;goto _LL901;
_LL901: if(_tmp396->escapes){return r;}return({struct Cyc_List_List*_tmp398=
_cycalloc(sizeof(struct Cyc_List_List));_tmp398->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp399=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));_tmp399->vd=v;_tmp399->rop=(
void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*_tmp39A=_cycalloc(sizeof(
struct Cyc_CfFlowInfo_LessEqSize_struct));_tmp39A[0]=({struct Cyc_CfFlowInfo_LessEqSize_struct
_tmp39B;_tmp39B.tag=Cyc_CfFlowInfo_LessEqSize;_tmp39B.f1=_tmp396;_tmp39B;});
_tmp39A;}));_tmp399;});_tmp398->tl=r;_tmp398;});_LL903: goto _LL893;_LL893:;}goto
_LL809;_LL819: goto _LL809;_LL809:;}return r;}struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){{void*
_tmp39C=(void*)e1->r;void*_tmp3A8;struct Cyc_Absyn_Vardecl*_tmp3AA;void*_tmp3AC;
struct Cyc_Absyn_Vardecl*_tmp3AE;void*_tmp3B0;struct Cyc_Absyn_Vardecl*_tmp3B2;
void*_tmp3B4;struct Cyc_Absyn_Vardecl*_tmp3B6;_LL926: if(*((int*)_tmp39C)== Cyc_Absyn_Var_e){
_LL937: _tmp3A8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp39C)->f2;if((
unsigned int)_tmp3A8 > 1?*((int*)_tmp3A8)== Cyc_Absyn_Global_b: 0){_LL939: _tmp3AA=((
struct Cyc_Absyn_Global_b_struct*)_tmp3A8)->f1;goto _LL927;}else{goto _LL928;}}
else{goto _LL928;}_LL928: if(*((int*)_tmp39C)== Cyc_Absyn_Var_e){_LL941: _tmp3AC=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp39C)->f2;if((unsigned int)_tmp3AC > 1?*((
int*)_tmp3AC)== Cyc_Absyn_Param_b: 0){_LL943: _tmp3AE=((struct Cyc_Absyn_Param_b_struct*)
_tmp3AC)->f1;goto _LL929;}else{goto _LL930;}}else{goto _LL930;}_LL930: if(*((int*)
_tmp39C)== Cyc_Absyn_Var_e){_LL945: _tmp3B0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp39C)->f2;if((unsigned int)_tmp3B0 > 1?*((int*)_tmp3B0)== Cyc_Absyn_Local_b: 0){
_LL947: _tmp3B2=((struct Cyc_Absyn_Local_b_struct*)_tmp3B0)->f1;goto _LL931;}else{
goto _LL932;}}else{goto _LL932;}_LL932: if(*((int*)_tmp39C)== Cyc_Absyn_Var_e){
_LL949: _tmp3B4=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp39C)->f2;if((
unsigned int)_tmp3B4 > 1?*((int*)_tmp3B4)== Cyc_Absyn_Pat_b: 0){_LL951: _tmp3B6=((
struct Cyc_Absyn_Pat_b_struct*)_tmp3B4)->f1;goto _LL933;}else{goto _LL934;}}else{
goto _LL934;}_LL934: goto _LL935;_LL927: _tmp3AE=_tmp3AA;goto _LL929;_LL929: _tmp3B2=
_tmp3AE;goto _LL931;_LL931: _tmp3B6=_tmp3B2;goto _LL933;_LL933: if(! _tmp3B6->escapes){
return Cyc_CfFlowInfo_reln_assign_var(r,_tmp3B6,e2);}goto _LL925;_LL935: goto _LL925;
_LL925:;}return r;}void Cyc_CfFlowInfo_print_reln(struct Cyc_CfFlowInfo_Reln*r){({
struct Cyc_Std_String_pa_struct _tmp3B9;_tmp3B9.tag=Cyc_Std_String_pa;_tmp3B9.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string((r->vd)->name);{void*_tmp3B8[1]={&
_tmp3B9};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s",sizeof(unsigned char),3),
_tag_arr(_tmp3B8,sizeof(void*),1));}});{void*_tmp3BA=(void*)r->rop;unsigned int
_tmp3C6;struct Cyc_Absyn_Vardecl*_tmp3C8;struct Cyc_Absyn_Vardecl*_tmp3CA;
unsigned int _tmp3CC;struct Cyc_Absyn_Vardecl*_tmp3CE;_LL956: if(*((int*)_tmp3BA)== 
Cyc_CfFlowInfo_EqualConst){_LL967: _tmp3C6=((struct Cyc_CfFlowInfo_EqualConst_struct*)
_tmp3BA)->f1;goto _LL957;}else{goto _LL958;}_LL958: if(*((int*)_tmp3BA)== Cyc_CfFlowInfo_LessVar){
_LL969: _tmp3C8=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp3BA)->f1;goto _LL959;}
else{goto _LL960;}_LL960: if(*((int*)_tmp3BA)== Cyc_CfFlowInfo_LessSize){_LL971:
_tmp3CA=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp3BA)->f1;goto _LL961;}else{
goto _LL962;}_LL962: if(*((int*)_tmp3BA)== Cyc_CfFlowInfo_LessConst){_LL973:
_tmp3CC=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp3BA)->f1;goto _LL963;}else{
goto _LL964;}_LL964: if(*((int*)_tmp3BA)== Cyc_CfFlowInfo_LessEqSize){_LL975:
_tmp3CE=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp3BA)->f1;goto _LL965;}else{
goto _LL955;}_LL957:({struct Cyc_Std_Int_pa_struct _tmp3D1;_tmp3D1.tag=Cyc_Std_Int_pa;
_tmp3D1.f1=(int)_tmp3C6;{void*_tmp3D0[1]={& _tmp3D1};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("==%d",sizeof(unsigned char),5),_tag_arr(_tmp3D0,sizeof(void*),1));}});
goto _LL955;_LL959:({struct Cyc_Std_String_pa_struct _tmp3D3;_tmp3D3.tag=Cyc_Std_String_pa;
_tmp3D3.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp3C8->name);{void*
_tmp3D2[1]={& _tmp3D3};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("<%s",sizeof(
unsigned char),4),_tag_arr(_tmp3D2,sizeof(void*),1));}});goto _LL955;_LL961:({
struct Cyc_Std_String_pa_struct _tmp3D5;_tmp3D5.tag=Cyc_Std_String_pa;_tmp3D5.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp3CA->name);{void*_tmp3D4[1]={&
_tmp3D5};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("<%s.size",sizeof(unsigned char),
9),_tag_arr(_tmp3D4,sizeof(void*),1));}});goto _LL955;_LL963:({struct Cyc_Std_Int_pa_struct
_tmp3D7;_tmp3D7.tag=Cyc_Std_Int_pa;_tmp3D7.f1=(int)_tmp3CC;{void*_tmp3D6[1]={&
_tmp3D7};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("<%d",sizeof(unsigned char),4),
_tag_arr(_tmp3D6,sizeof(void*),1));}});goto _LL955;_LL965:({struct Cyc_Std_String_pa_struct
_tmp3D9;_tmp3D9.tag=Cyc_Std_String_pa;_tmp3D9.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp3CE->name);{void*_tmp3D8[1]={& _tmp3D9};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("<=%s.size",sizeof(unsigned char),10),_tag_arr(_tmp3D8,sizeof(void*),1));}});
goto _LL955;_LL955:;}}void Cyc_CfFlowInfo_print_relns(struct Cyc_List_List*r){for(0;
r != 0;r=r->tl){Cyc_CfFlowInfo_print_reln((struct Cyc_CfFlowInfo_Reln*)r->hd);if(r->tl
!= 0){({void*_tmp3DA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr(",",sizeof(
unsigned char),2),_tag_arr(_tmp3DA,sizeof(void*),0));});}}}
