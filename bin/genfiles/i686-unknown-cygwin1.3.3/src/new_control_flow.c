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
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);extern struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
extern unsigned char Cyc_List_Nth[8];struct Cyc_Set_Set;extern struct Cyc_Set_Set*Cyc_Set_insert(
struct Cyc_Set_Set*s,void*elt);extern struct Cyc_Set_Set*Cyc_Set_union_two(struct
Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);extern unsigned char Cyc_Set_Absent[11];
struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern unsigned char
Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,
void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);extern int Cyc_Std_zstrptrcmp(
struct _tagged_arr*,struct _tagged_arr*);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;static const int Cyc_Position_Lex=0;static const int Cyc_Position_Parse=
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
struct _tagged_arr*Cyc_Absyn_fieldname(int);struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;
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
_tagged_arr ap);extern void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(void*t);
extern void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);extern int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);extern int Cyc_Tcutil_is_noreturn(void*);extern
unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);static const int
Cyc_CfFlowInfo_VarRoot=0;struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};static const int Cyc_CfFlowInfo_MallocPt=1;struct Cyc_CfFlowInfo_MallocPt_struct{
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
f2;};extern struct Cyc_Set_Set*Cyc_CfFlowInfo_mt_place_set();extern void*Cyc_CfFlowInfo_unknown_none;
extern void*Cyc_CfFlowInfo_unknown_all;extern int Cyc_CfFlowInfo_root_cmp(void*,
void*);extern void*Cyc_CfFlowInfo_typ_to_absrval(void*t,void*leafval);extern void*
Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict*d,void*r);extern void*Cyc_CfFlowInfo_lookup_place(
struct Cyc_Dict_Dict*d,struct Cyc_CfFlowInfo_Place*place);extern int Cyc_CfFlowInfo_flow_lessthan_approx(
void*f1,void*f2);extern struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(struct
Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);extern struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_exp(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct
Cyc_Absyn_Exp*);extern struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);extern struct Cyc_Dict_Dict*Cyc_CfFlowInfo_escape_deref(
struct Cyc_Dict_Dict*d,struct Cyc_Set_Set**all_changed,void*r);extern struct Cyc_Dict_Dict*
Cyc_CfFlowInfo_assign_place(struct Cyc_Position_Segment*loc,struct Cyc_Dict_Dict*d,
struct Cyc_Set_Set**all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);extern
void*Cyc_CfFlowInfo_join_flow(struct Cyc_Set_Set**,void*,void*);extern void*Cyc_CfFlowInfo_after_flow(
struct Cyc_Set_Set**,void*,void*,struct Cyc_Set_Set*,struct Cyc_Set_Set*);extern
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);extern void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*
encloser;int visited;void*flow;};static unsigned char Cyc_NewControlFlow_CFAnnot[12]="\000\000\000\000CFAnnot";
struct Cyc_NewControlFlow_CFAnnot_struct{unsigned char*tag;struct Cyc_NewControlFlow_CFStmtAnnot
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser){(void*)(enclosee->annot=(void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp0=_cycalloc(sizeof(struct Cyc_NewControlFlow_CFAnnot_struct));_tmp0[0]=({
struct Cyc_NewControlFlow_CFAnnot_struct _tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;
_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp2;_tmp2.encloser=encloser;
_tmp2.visited=0;_tmp2.flow=(void*)((void*)Cyc_CfFlowInfo_BottomFL);_tmp2;});
_tmp1;});_tmp0;})));}static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp9;struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB;_LL5: if(*((void**)_tmp3)== Cyc_NewControlFlow_CFAnnot){
_LL10: _tmp9=((struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmpB=(struct
Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp3)->f1;goto _LL6;}else{goto _LL7;}_LL7: goto _LL8;_LL6: return _tmpB;_LL8:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpC=_cycalloc(sizeof(struct
Cyc_Core_Impossible_struct));_tmpC[0]=({struct Cyc_Core_Impossible_struct _tmpD;
_tmpD.tag=Cyc_Core_Impossible;_tmpD.f1=_tag_arr("ControlFlow -- wrong stmt annotation",
sizeof(unsigned char),37);_tmpD;});_tmpC;}));_LL4:;}struct Cyc_NewControlFlow_AnalEnv{
int iterate_again;int iteration_num;int in_try;void*tryflow;struct Cyc_Set_Set**
all_changed;int noreturn;};static void*Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Stmt*);static void*Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Decl*);static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,void*,struct Cyc_Absyn_Exp*);static struct
_tuple0 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,void*,
struct Cyc_Absyn_Exp*);static struct _tuple0 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,
void*,struct Cyc_Absyn_Exp*);static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Stmt*s){struct
Cyc_NewControlFlow_CFStmtAnnot*_tmpE=Cyc_NewControlFlow_get_stmt_annot(s);(void*)(
_tmpE->flow=(void*)Cyc_CfFlowInfo_join_flow(env->all_changed,inflow,(void*)_tmpE->flow));
_tmpE->visited=env->iteration_num;return _tmpE;}static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,void*new_flow){if(env->in_try){(void*)(env->tryflow=(
void*)Cyc_CfFlowInfo_join_flow(env->all_changed,new_flow,(void*)env->tryflow));}}
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_Absyn_Stmt*s,void*flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpF=Cyc_NewControlFlow_get_stmt_annot(
s);void*_tmp10=Cyc_CfFlowInfo_join_flow(env->all_changed,flow,(void*)_tmpF->flow);
if(! Cyc_CfFlowInfo_flow_lessthan_approx(_tmp10,(void*)_tmpF->flow)){(void*)(
_tmpF->flow=(void*)_tmp10);if(_tmpF->visited == env->iteration_num){env->iterate_again=
1;}}}static void*Cyc_NewControlFlow_add_vars(void*inflow,struct Cyc_List_List*vds,
void*leafval){void*_tmp11=inflow;struct Cyc_List_List*_tmp17;struct Cyc_Dict_Dict*
_tmp19;_LL19: if(_tmp11 == (void*)Cyc_CfFlowInfo_BottomFL){goto _LL20;}else{goto
_LL21;}_LL21: if((unsigned int)_tmp11 > 1?*((int*)_tmp11)== Cyc_CfFlowInfo_ReachableFL:
0){_LL26: _tmp19=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11)->f1;goto _LL24;
_LL24: _tmp17=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp11)->f2;goto _LL22;}
else{goto _LL18;}_LL20: return(void*)Cyc_CfFlowInfo_BottomFL;_LL22: for(0;vds != 0;
vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp1B=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp1C=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_struct));_tmp1C[0]=({struct
Cyc_CfFlowInfo_VarRoot_struct _tmp1D;_tmp1D.tag=Cyc_CfFlowInfo_VarRoot;_tmp1D.f1=(
struct Cyc_Absyn_Vardecl*)vds->hd;_tmp1D;});_tmp1C;});_tmp19=Cyc_Dict_insert(
_tmp19,(void*)_tmp1B,Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Absyn_Vardecl*)
vds->hd)->type,leafval));}return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp1E=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp1E[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp1F;_tmp1F.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp1F.f1=_tmp19;_tmp1F.f2=_tmp17;_tmp1F;});_tmp1E;});_LL18:;}static void*Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,void*inflow,
void*r){void*_tmp20=inflow;struct Cyc_List_List*_tmp26;struct Cyc_Dict_Dict*_tmp28;
_LL34: if(_tmp20 == (void*)Cyc_CfFlowInfo_BottomFL){goto _LL35;}else{goto _LL36;}
_LL36: if((unsigned int)_tmp20 > 1?*((int*)_tmp20)== Cyc_CfFlowInfo_ReachableFL: 0){
_LL41: _tmp28=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp20)->f1;goto _LL39;
_LL39: _tmp26=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp20)->f2;goto _LL37;}
else{goto _LL33;}_LL35: return(void*)Cyc_CfFlowInfo_BottomFL;_LL37: if(Cyc_CfFlowInfo_initlevel(
_tmp28,r)!= (void*)Cyc_CfFlowInfo_AllIL){({void*_tmp2A[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("expression may not be fully initialized",sizeof(unsigned char),40),
_tag_arr(_tmp2A,sizeof(void*),0));});}{struct Cyc_Dict_Dict*_tmp2B=Cyc_CfFlowInfo_escape_deref(
_tmp28,env->all_changed,r);if(_tmp28 == _tmp2B){return inflow;}{struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp2C=({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2D=_cycalloc(sizeof(struct
Cyc_CfFlowInfo_ReachableFL_struct));_tmp2D[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp2E;_tmp2E.tag=Cyc_CfFlowInfo_ReachableFL;_tmp2E.f1=_tmp2B;_tmp2E.f2=_tmp26;
_tmp2E;});_tmp2D;});Cyc_NewControlFlow_update_tryflow(env,(void*)_tmp2C);return(
void*)_tmp2C;}}_LL33:;}struct _tuple4{void*f1;struct Cyc_List_List*f2;};static
struct _tuple4 Cyc_NewControlFlow_anal_unordered_Rexps(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es){if(es == 0){return({struct _tuple4 _tmp2F;
_tmp2F.f1=inflow;_tmp2F.f2=0;_tmp2F;});}if(es->tl == 0){void*_tmp32;void*_tmp34;
struct _tuple0 _tmp30=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
es->hd);_LL53: _tmp34=_tmp30.f1;goto _LL51;_LL51: _tmp32=_tmp30.f2;goto _LL49;_LL49:
return({struct _tuple4 _tmp36;_tmp36.f1=_tmp34;_tmp36.f2=({struct Cyc_List_List*
_tmp37=_cycalloc(sizeof(struct Cyc_List_List));_tmp37->hd=(void*)_tmp32;_tmp37->tl=
0;_tmp37;});_tmp36;});}{struct Cyc_Set_Set**outer_all_changed=env->all_changed;
struct Cyc_Set_Set*this_all_changed;void*old_inflow;void*outflow;struct Cyc_List_List*
rvals;do {this_all_changed=Cyc_CfFlowInfo_mt_place_set();env->all_changed=({
struct Cyc_Set_Set**_tmp38=_cycalloc(sizeof(struct Cyc_Set_Set*));_tmp38[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp38;});{void*_tmp3B;void*_tmp3D;struct _tuple0 _tmp39=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)es->hd);_LL62: _tmp3D=_tmp39.f1;goto _LL60;_LL60:
_tmp3B=_tmp39.f2;goto _LL58;_LL58: outflow=_tmp3D;rvals=({struct Cyc_List_List*
_tmp3F=_cycalloc(sizeof(struct Cyc_List_List));_tmp3F->hd=(void*)_tmp3B;_tmp3F->tl=
0;_tmp3F;});this_all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct
Cyc_Set_Set*s2))Cyc_Set_union_two)(this_all_changed,*((struct Cyc_Set_Set**)
_check_null(env->all_changed)));{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;
es2=es2->tl){env->all_changed=({struct Cyc_Set_Set**_tmp40=_cycalloc(sizeof(
struct Cyc_Set_Set*));_tmp40[0]=Cyc_CfFlowInfo_mt_place_set();_tmp40;});{void*
_tmp43;void*_tmp45;struct _tuple0 _tmp41=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)es2->hd);_LL70: _tmp45=_tmp41.f1;goto _LL68;_LL68: _tmp43=
_tmp41.f2;goto _LL66;_LL66: rvals=({struct Cyc_List_List*_tmp47=_cycalloc(sizeof(
struct Cyc_List_List));_tmp47->hd=(void*)_tmp43;_tmp47->tl=rvals;_tmp47;});
outflow=Cyc_CfFlowInfo_after_flow((struct Cyc_Set_Set**)& this_all_changed,outflow,
_tmp45,this_all_changed,*((struct Cyc_Set_Set**)_check_null(env->all_changed)));
this_all_changed=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_union_two)(this_all_changed,*((struct Cyc_Set_Set**)_check_null(env->all_changed)));}}}
old_inflow=inflow;inflow=Cyc_CfFlowInfo_join_flow(outer_all_changed,inflow,
outflow);}} while (! Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));if(
outer_all_changed == 0){env->all_changed=0;}else{env->all_changed=({struct Cyc_Set_Set**
_tmp48=_cycalloc(sizeof(struct Cyc_Set_Set*));_tmp48[0]=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*outer_all_changed,
this_all_changed);_tmp48;});}Cyc_NewControlFlow_update_tryflow(env,outflow);
return({struct _tuple4 _tmp49;_tmp49.f1=outflow;_tmp49.f2=Cyc_List_imp_rev(rvals);
_tmp49;});}}static struct _tuple0 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*es){struct Cyc_List_List*_tmp4C;void*_tmp4E;
struct _tuple4 _tmp4A=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,es);_LL79:
_tmp4E=_tmp4A.f1;goto _LL77;_LL77: _tmp4C=_tmp4A.f2;goto _LL75;_LL75:{void*_tmp50=
_tmp4E;struct Cyc_List_List*_tmp56;struct Cyc_Dict_Dict*_tmp58;_LL82: if((
unsigned int)_tmp50 > 1?*((int*)_tmp50)== Cyc_CfFlowInfo_ReachableFL: 0){_LL89:
_tmp58=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp50)->f1;goto _LL87;_LL87:
_tmp56=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp50)->f2;goto _LL83;}else{
goto _LL84;}_LL84: if(_tmp50 == (void*)Cyc_CfFlowInfo_BottomFL){goto _LL85;}else{
goto _LL81;}_LL83: for(0;_tmp4C != 0;(_tmp4C=_tmp4C->tl,es=((struct Cyc_List_List*)
_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(_tmp58,(void*)_tmp4C->hd)== (
void*)Cyc_CfFlowInfo_NoneIL){({void*_tmp5A[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp5A,sizeof(void*),0));});}}goto _LL81;_LL85:
goto _LL81;_LL81:;}return({struct _tuple0 _tmp5B;_tmp5B.f1=_tmp4E;_tmp5B.f2=Cyc_CfFlowInfo_unknown_all;
_tmp5B;});}static void*Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp5C=outflow;
struct Cyc_List_List*_tmp62;struct Cyc_Dict_Dict*_tmp64;_LL94: if(_tmp5C == (void*)
Cyc_CfFlowInfo_BottomFL){goto _LL95;}else{goto _LL96;}_LL96: if((unsigned int)
_tmp5C > 1?*((int*)_tmp5C)== Cyc_CfFlowInfo_ReachableFL: 0){_LL101: _tmp64=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp5C)->f1;goto _LL99;_LL99: _tmp62=((struct
Cyc_CfFlowInfo_ReachableFL_struct*)_tmp5C)->f2;goto _LL97;}else{goto _LL93;}_LL95:
return outflow;_LL97: {void*_tmp66=(Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;
struct Cyc_CfFlowInfo_Place*_tmp6C;_LL104: if(_tmp66 == (void*)Cyc_CfFlowInfo_UnknownL){
goto _LL105;}else{goto _LL106;}_LL106: if((unsigned int)_tmp66 > 1?*((int*)_tmp66)== 
Cyc_CfFlowInfo_PlaceL: 0){_LL109: _tmp6C=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp66)->f1;goto _LL107;}else{goto _LL103;}_LL105: return outflow;_LL107: {void*
nzval=il == (void*)Cyc_CfFlowInfo_AllIL?(void*)Cyc_CfFlowInfo_NotZeroAll:(void*)
Cyc_CfFlowInfo_NotZeroThis;return(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp6E=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp6E[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp6F;_tmp6F.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp6F.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp64,env->all_changed,_tmp6C,
nzval);_tmp6F.f2=_tmp62;_tmp6F;});_tmp6E;});}_LL103:;}_LL93:;}static struct
_tuple0 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,void*outflow,struct Cyc_Absyn_Exp*e,void*il){void*_tmp70=outflow;struct Cyc_List_List*
_tmp76;struct Cyc_Dict_Dict*_tmp78;_LL114: if(_tmp70 == (void*)Cyc_CfFlowInfo_BottomFL){
goto _LL115;}else{goto _LL116;}_LL116: if((unsigned int)_tmp70 > 1?*((int*)_tmp70)== 
Cyc_CfFlowInfo_ReachableFL: 0){_LL121: _tmp78=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp70)->f1;goto _LL119;_LL119: _tmp76=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp70)->f2;goto _LL117;}else{goto _LL113;}_LL115: return({struct _tuple0 _tmp7A;
_tmp7A.f1=outflow;_tmp7A.f2=outflow;_tmp7A;});_LL117: {void*_tmp7B=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp81;_LL125: if(_tmp7B == (void*)
Cyc_CfFlowInfo_UnknownL){goto _LL126;}else{goto _LL127;}_LL127: if((unsigned int)
_tmp7B > 1?*((int*)_tmp7B)== Cyc_CfFlowInfo_PlaceL: 0){_LL130: _tmp81=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp7B)->f1;goto _LL128;}else{goto _LL124;}_LL126: return({struct _tuple0 _tmp83;
_tmp83.f1=outflow;_tmp83.f2=outflow;_tmp83;});_LL128: {void*nzval=il == (void*)
Cyc_CfFlowInfo_AllIL?(void*)Cyc_CfFlowInfo_NotZeroAll:(void*)Cyc_CfFlowInfo_NotZeroThis;
return({struct _tuple0 _tmp84;_tmp84.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp87=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp87[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp88;_tmp88.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp88.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp78,env->all_changed,_tmp81,
nzval);_tmp88.f2=_tmp76;_tmp88;});_tmp87;});_tmp84.f2=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp85=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp85[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp86;_tmp86.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp86.f1=Cyc_CfFlowInfo_assign_place(e->loc,_tmp78,env->all_changed,_tmp81,(
void*)Cyc_CfFlowInfo_Zero);_tmp86.f2=_tmp76;_tmp86;});_tmp85;});_tmp84;});}
_LL124:;}_LL113:;}static struct _tuple0 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,void*f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp89=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp8F;struct Cyc_Absyn_Conref*_tmp91;void*_tmp93;_LL139: if((unsigned int)_tmp89 > 
3?*((int*)_tmp89)== Cyc_Absyn_PointerType: 0){_LL144: _tmp8F=((struct Cyc_Absyn_PointerType_struct*)
_tmp89)->f1;_LL148: _tmp93=(void*)_tmp8F.elt_typ;goto _LL146;_LL146: _tmp91=_tmp8F.bounds;
goto _LL140;}else{goto _LL141;}_LL141: goto _LL142;_LL140: {void*_tmp95=f;struct Cyc_List_List*
_tmp9B;struct Cyc_Dict_Dict*_tmp9D;_LL151: if(_tmp95 == (void*)Cyc_CfFlowInfo_BottomFL){
goto _LL152;}else{goto _LL153;}_LL153: if((unsigned int)_tmp95 > 1?*((int*)_tmp95)== 
Cyc_CfFlowInfo_ReachableFL: 0){_LL158: _tmp9D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp95)->f1;goto _LL156;_LL156: _tmp9B=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp95)->f2;goto _LL154;}else{goto _LL150;}_LL152: return({struct _tuple0 _tmp9F;
_tmp9F.f1=f;_tmp9F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp93,Cyc_CfFlowInfo_unknown_all);
_tmp9F;});_LL154: if(Cyc_Tcutil_is_bound_one(_tmp91)){void*_tmpA0=r;struct Cyc_CfFlowInfo_Place*
_tmpAE;void*_tmpB0;_LL162: if(_tmpA0 == (void*)Cyc_CfFlowInfo_NotZeroAll){goto
_LL163;}else{goto _LL164;}_LL164: if(_tmpA0 == (void*)Cyc_CfFlowInfo_NotZeroThis){
goto _LL165;}else{goto _LL166;}_LL166: if((unsigned int)_tmpA0 > 3?*((int*)_tmpA0)== 
Cyc_CfFlowInfo_AddressOf: 0){_LL175: _tmpAE=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpA0)->f1;goto _LL167;}else{goto _LL168;}_LL168: if(_tmpA0 == (void*)Cyc_CfFlowInfo_Zero){
goto _LL169;}else{goto _LL170;}_LL170: if((unsigned int)_tmpA0 > 3?*((int*)_tmpA0)== 
Cyc_CfFlowInfo_UnknownR: 0){_LL177: _tmpB0=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpA0)->f1;goto _LL171;}else{goto _LL172;}_LL172: goto _LL173;_LL163: goto _LL165;
_LL165:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmpB2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NotZero_struct));_tmpB2[0]=({struct
Cyc_CfFlowInfo_NotZero_struct _tmpB3;_tmpB3.tag=Cyc_CfFlowInfo_NotZero;_tmpB3.f1=
_tmp9B;_tmpB3;});_tmpB2;})));goto _LL161;_LL167:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_NotZero_struct*_tmpB4=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NotZero_struct));
_tmpB4[0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmpB5;_tmpB5.tag=Cyc_CfFlowInfo_NotZero;
_tmpB5.f1=_tmp9B;_tmpB5;});_tmpB4;})));return({struct _tuple0 _tmpB6;_tmpB6.f1=f;
_tmpB6.f2=Cyc_CfFlowInfo_lookup_place(_tmp9D,_tmpAE);_tmpB6;});_LL169:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_IsZero_struct*_tmpB7=_cycalloc(sizeof(
struct Cyc_CfFlowInfo_IsZero_struct));_tmpB7[0]=({struct Cyc_CfFlowInfo_IsZero_struct
_tmpB8;_tmpB8.tag=Cyc_CfFlowInfo_IsZero;_tmpB8.f1=_tmp9B;_tmpB8;});_tmpB7;})));
return({struct _tuple0 _tmpB9;_tmpB9.f1=(void*)Cyc_CfFlowInfo_BottomFL;_tmpB9.f2=
Cyc_CfFlowInfo_typ_to_absrval(_tmp93,Cyc_CfFlowInfo_unknown_all);_tmpB9;});
_LL171: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpB0);goto _LL173;_LL173:(
void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmpBA=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownZ_struct));_tmpBA[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmpBB;_tmpBB.tag=Cyc_CfFlowInfo_UnknownZ;_tmpBB.f1=_tmp9B;_tmpBB;});_tmpBA;})));
_LL161:;}else{(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmpBC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownZ_struct));_tmpBC[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmpBD;_tmpBD.tag=Cyc_CfFlowInfo_UnknownZ;
_tmpBD.f1=_tmp9B;_tmpBD;});_tmpBC;})));}{void*_tmpBE=Cyc_CfFlowInfo_initlevel(
_tmp9D,r);_LL192: if(_tmpBE == (void*)Cyc_CfFlowInfo_NoneIL){goto _LL193;}else{goto
_LL194;}_LL194: if(_tmpBE == (void*)Cyc_CfFlowInfo_AllIL){goto _LL195;}else{goto
_LL196;}_LL196: if(_tmpBE == (void*)Cyc_CfFlowInfo_ThisIL){goto _LL197;}else{goto
_LL191;}_LL193:({void*_tmpC6[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of possibly uninitialized pointer",
sizeof(unsigned char),46),_tag_arr(_tmpC6,sizeof(void*),0));});goto _LL195;_LL195:
return({struct _tuple0 _tmpC7;_tmpC7.f1=f;_tmpC7.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp93,Cyc_CfFlowInfo_unknown_all);_tmpC7;});_LL197: return({struct _tuple0 _tmpC8;
_tmpC8.f1=f;_tmpC8.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp93,Cyc_CfFlowInfo_unknown_none);
_tmpC8;});_LL191:;}_LL150:;}_LL142:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpC9=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmpC9[0]=({struct Cyc_Core_Impossible_struct
_tmpCA;_tmpCA.tag=Cyc_Core_Impossible;_tmpCA.f1=_tag_arr("right deref of non-pointer-type",
sizeof(unsigned char),32);_tmpCA;});_tmpC9;}));_LL138:;}static struct Cyc_List_List*
Cyc_NewControlFlow_add_subscript_reln(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*_tmpCB=(void*)e1->r;void*_tmpD7;struct Cyc_Absyn_Vardecl*
_tmpD9;void*_tmpDB;struct Cyc_Absyn_Vardecl*_tmpDD;void*_tmpDF;struct Cyc_Absyn_Vardecl*
_tmpE1;void*_tmpE3;struct Cyc_Absyn_Vardecl*_tmpE5;_LL205: if(*((int*)_tmpCB)== 
Cyc_Absyn_Var_e){_LL216: _tmpD7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpCB)->f2;
if((unsigned int)_tmpD7 > 1?*((int*)_tmpD7)== Cyc_Absyn_Pat_b: 0){_LL218: _tmpD9=((
struct Cyc_Absyn_Pat_b_struct*)_tmpD7)->f1;goto _LL206;}else{goto _LL207;}}else{
goto _LL207;}_LL207: if(*((int*)_tmpCB)== Cyc_Absyn_Var_e){_LL220: _tmpDB=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpCB)->f2;if((unsigned int)_tmpDB > 1?*((int*)
_tmpDB)== Cyc_Absyn_Local_b: 0){_LL222: _tmpDD=((struct Cyc_Absyn_Local_b_struct*)
_tmpDB)->f1;goto _LL208;}else{goto _LL209;}}else{goto _LL209;}_LL209: if(*((int*)
_tmpCB)== Cyc_Absyn_Var_e){_LL224: _tmpDF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpCB)->f2;if((unsigned int)_tmpDF > 1?*((int*)_tmpDF)== Cyc_Absyn_Param_b: 0){
_LL226: _tmpE1=((struct Cyc_Absyn_Param_b_struct*)_tmpDF)->f1;goto _LL210;}else{
goto _LL211;}}else{goto _LL211;}_LL211: if(*((int*)_tmpCB)== Cyc_Absyn_Var_e){
_LL228: _tmpE3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpCB)->f2;if((
unsigned int)_tmpE3 > 1?*((int*)_tmpE3)== Cyc_Absyn_Global_b: 0){_LL230: _tmpE5=((
struct Cyc_Absyn_Global_b_struct*)_tmpE3)->f1;goto _LL212;}else{goto _LL213;}}else{
goto _LL213;}_LL213: goto _LL214;_LL206: _tmpDD=_tmpD9;goto _LL208;_LL208: _tmpE1=
_tmpDD;goto _LL210;_LL210: _tmpE5=_tmpE1;goto _LL212;_LL212: if(! _tmpE5->escapes){
void*_tmpE7=(void*)e2->r;void*_tmpF3;struct Cyc_Absyn_Vardecl*_tmpF5;void*_tmpF7;
struct Cyc_Absyn_Vardecl*_tmpF9;void*_tmpFB;struct Cyc_Absyn_Vardecl*_tmpFD;void*
_tmpFF;struct Cyc_Absyn_Vardecl*_tmp101;_LL233: if(*((int*)_tmpE7)== Cyc_Absyn_Var_e){
_LL244: _tmpF3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpE7)->f2;if((
unsigned int)_tmpF3 > 1?*((int*)_tmpF3)== Cyc_Absyn_Pat_b: 0){_LL246: _tmpF5=((
struct Cyc_Absyn_Pat_b_struct*)_tmpF3)->f1;goto _LL234;}else{goto _LL235;}}else{
goto _LL235;}_LL235: if(*((int*)_tmpE7)== Cyc_Absyn_Var_e){_LL248: _tmpF7=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpE7)->f2;if((unsigned int)_tmpF7 > 1?*((int*)
_tmpF7)== Cyc_Absyn_Local_b: 0){_LL250: _tmpF9=((struct Cyc_Absyn_Local_b_struct*)
_tmpF7)->f1;goto _LL236;}else{goto _LL237;}}else{goto _LL237;}_LL237: if(*((int*)
_tmpE7)== Cyc_Absyn_Var_e){_LL252: _tmpFB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpE7)->f2;if((unsigned int)_tmpFB > 1?*((int*)_tmpFB)== Cyc_Absyn_Param_b: 0){
_LL254: _tmpFD=((struct Cyc_Absyn_Param_b_struct*)_tmpFB)->f1;goto _LL238;}else{
goto _LL239;}}else{goto _LL239;}_LL239: if(*((int*)_tmpE7)== Cyc_Absyn_Var_e){
_LL256: _tmpFF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpE7)->f2;if((
unsigned int)_tmpFF > 1?*((int*)_tmpFF)== Cyc_Absyn_Global_b: 0){_LL258: _tmp101=((
struct Cyc_Absyn_Global_b_struct*)_tmpFF)->f1;goto _LL240;}else{goto _LL241;}}else{
goto _LL241;}_LL241: goto _LL242;_LL234: _tmpF9=_tmpF5;goto _LL236;_LL236: _tmpFD=
_tmpF9;goto _LL238;_LL238: _tmp101=_tmpFD;goto _LL240;_LL240: if(! _tmp101->escapes){
int found=0;{struct Cyc_List_List*_tmp103=relns;for(0;_tmp103 != 0;_tmp103=_tmp103->tl){
struct Cyc_CfFlowInfo_Reln*_tmp104=(struct Cyc_CfFlowInfo_Reln*)_tmp103->hd;if(
_tmp104->vd == _tmp101){void*_tmp105=(void*)_tmp104->rop;struct Cyc_Absyn_Vardecl*
_tmp10C;_LL263: if(*((int*)_tmp105)== Cyc_CfFlowInfo_LessSize){_LL269: _tmp10C=((
struct Cyc_CfFlowInfo_LessSize_struct*)_tmp105)->f1;goto _LL267;}else{goto _LL265;}
_LL267: if(_tmp10C == _tmpE5){goto _LL264;}else{goto _LL265;}_LL265: goto _LL266;
_LL264: return relns;_LL266: continue;_LL262:;}}}if(! found){return({struct Cyc_List_List*
_tmp10E=_cycalloc(sizeof(struct Cyc_List_List));_tmp10E->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp10F=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));_tmp10F->vd=_tmp101;_tmp10F->rop=(
void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*_tmp110=_cycalloc(sizeof(
struct Cyc_CfFlowInfo_LessSize_struct));_tmp110[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp111;_tmp111.tag=Cyc_CfFlowInfo_LessSize;_tmp111.f1=_tmpE5;_tmp111;});_tmp110;}));
_tmp10F;});_tmp10E->tl=relns;_tmp10E;});}}return relns;_LL242: return relns;_LL232:;}
else{return relns;}_LL214: return relns;_LL204:;}struct _tuple5{struct Cyc_List_List*
f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple0 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Exp*e){struct
Cyc_Dict_Dict*d;struct Cyc_List_List*relns;{void*_tmp112=inflow;struct Cyc_List_List*
_tmp118;struct Cyc_Dict_Dict*_tmp11A;_LL276: if(_tmp112 == (void*)Cyc_CfFlowInfo_BottomFL){
goto _LL277;}else{goto _LL278;}_LL278: if((unsigned int)_tmp112 > 1?*((int*)_tmp112)
== Cyc_CfFlowInfo_ReachableFL: 0){_LL283: _tmp11A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp112)->f1;goto _LL281;_LL281: _tmp118=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp112)->f2;goto _LL279;}else{goto _LL275;}_LL277: return({struct _tuple0 _tmp11C;
_tmp11C.f1=(void*)Cyc_CfFlowInfo_BottomFL;_tmp11C.f2=Cyc_CfFlowInfo_unknown_all;
_tmp11C;});_LL279: d=_tmp11A;relns=_tmp118;_LL275:;}{void*_tmp11D=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Absyn_Exp*
_tmp183;void*_tmp185;void*_tmp187;int _tmp189;void*_tmp18B;void*_tmp18D;void*
_tmp18F;void*_tmp191;struct Cyc_List_List*_tmp193;void*_tmp195;void*_tmp197;
struct Cyc_Absyn_Vardecl*_tmp199;void*_tmp19B;struct Cyc_Absyn_Vardecl*_tmp19D;
void*_tmp19F;struct Cyc_Absyn_Vardecl*_tmp1A1;struct Cyc_List_List*_tmp1A3;struct
Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A7;struct Cyc_Core_Opt*_tmp1A9;
struct Cyc_Core_Opt _tmp1AB;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_Exp*
_tmp1AE;struct Cyc_Core_Opt*_tmp1B0;struct Cyc_Absyn_Exp*_tmp1B2;struct Cyc_Absyn_Exp*
_tmp1B4;struct Cyc_Absyn_Exp*_tmp1B6;struct Cyc_Absyn_Exp*_tmp1B8;struct Cyc_List_List*
_tmp1BA;struct Cyc_Absyn_Exp*_tmp1BC;struct Cyc_Absyn_MallocInfo _tmp1BE;int _tmp1C0;
struct Cyc_Absyn_Exp*_tmp1C2;void**_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C6;int _tmp1C8;
struct Cyc_Absyn_Exp*_tmp1CA;struct Cyc_Absyn_Exp*_tmp1CC;struct Cyc_Absyn_Exp*
_tmp1CE;struct Cyc_Absyn_Exp*_tmp1D0;struct _tagged_arr*_tmp1D2;struct Cyc_Absyn_Exp*
_tmp1D4;struct _tagged_arr*_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_Absyn_Exp*
_tmp1DA;struct Cyc_Absyn_Exp*_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DE;struct Cyc_Absyn_Exp*
_tmp1E0;struct Cyc_Absyn_Exp*_tmp1E2;struct Cyc_List_List*_tmp1E4;struct Cyc_List_List*
_tmp1E6;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1EA;struct Cyc_List_List*
_tmp1EC;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1F0;struct Cyc_Absyn_Vardecl*
_tmp1F2;struct Cyc_Absyn_Stmt*_tmp1F4;void*_tmp1F6;_LL287: if(*((int*)_tmp11D)== 
Cyc_Absyn_Cast_e){_LL384: _tmp17F=((struct Cyc_Absyn_Cast_e_struct*)_tmp11D)->f2;
goto _LL288;}else{goto _LL289;}_LL289: if(*((int*)_tmp11D)== Cyc_Absyn_NoInstantiate_e){
_LL386: _tmp181=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp11D)->f1;goto _LL290;}
else{goto _LL291;}_LL291: if(*((int*)_tmp11D)== Cyc_Absyn_Instantiate_e){_LL388:
_tmp183=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp11D)->f1;goto _LL292;}else{
goto _LL293;}_LL293: if(*((int*)_tmp11D)== Cyc_Absyn_Const_e){_LL390: _tmp185=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp11D)->f1;if(_tmp185 == (void*)Cyc_Absyn_Null_c){
goto _LL294;}else{goto _LL295;}}else{goto _LL295;}_LL295: if(*((int*)_tmp11D)== Cyc_Absyn_Const_e){
_LL392: _tmp187=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp11D)->f1;if((
unsigned int)_tmp187 > 1?*((int*)_tmp187)== Cyc_Absyn_Int_c: 0){_LL396: _tmp18B=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp187)->f1;if(_tmp18B == (void*)Cyc_Absyn_Signed){
goto _LL394;}else{goto _LL297;}_LL394: _tmp189=((struct Cyc_Absyn_Int_c_struct*)
_tmp187)->f2;if(_tmp189 == 0){goto _LL296;}else{goto _LL297;}}else{goto _LL297;}}
else{goto _LL297;}_LL297: if(*((int*)_tmp11D)== Cyc_Absyn_Const_e){_LL398: _tmp18D=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp11D)->f1;if((unsigned int)_tmp18D > 1?*((
int*)_tmp18D)== Cyc_Absyn_Int_c: 0){_LL400: _tmp18F=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp18D)->f1;if(_tmp18F == (void*)Cyc_Absyn_Signed){goto _LL298;}else{goto _LL299;}}
else{goto _LL299;}}else{goto _LL299;}_LL299: if(*((int*)_tmp11D)== Cyc_Absyn_Var_e){
_LL402: _tmp191=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp11D)->f2;if((
unsigned int)_tmp191 > 1?*((int*)_tmp191)== Cyc_Absyn_Funname_b: 0){goto _LL300;}
else{goto _LL301;}}else{goto _LL301;}_LL301: if(*((int*)_tmp11D)== Cyc_Absyn_Tunion_e){
_LL404: _tmp193=((struct Cyc_Absyn_Tunion_e_struct*)_tmp11D)->f3;if(_tmp193 == 0){
goto _LL302;}else{goto _LL303;}}else{goto _LL303;}_LL303: if(*((int*)_tmp11D)== Cyc_Absyn_Const_e){
goto _LL304;}else{goto _LL305;}_LL305: if(*((int*)_tmp11D)== Cyc_Absyn_Sizeofexp_e){
goto _LL306;}else{goto _LL307;}_LL307: if(*((int*)_tmp11D)== Cyc_Absyn_Sizeoftyp_e){
goto _LL308;}else{goto _LL309;}_LL309: if(*((int*)_tmp11D)== Cyc_Absyn_Offsetof_e){
goto _LL310;}else{goto _LL311;}_LL311: if(*((int*)_tmp11D)== Cyc_Absyn_Gentyp_e){
goto _LL312;}else{goto _LL313;}_LL313: if(*((int*)_tmp11D)== Cyc_Absyn_AnonEnum_e){
goto _LL314;}else{goto _LL315;}_LL315: if(*((int*)_tmp11D)== Cyc_Absyn_Enum_e){goto
_LL316;}else{goto _LL317;}_LL317: if(*((int*)_tmp11D)== Cyc_Absyn_Var_e){_LL406:
_tmp195=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp11D)->f2;if((unsigned int)
_tmp195 > 1?*((int*)_tmp195)== Cyc_Absyn_Global_b: 0){goto _LL318;}else{goto _LL319;}}
else{goto _LL319;}_LL319: if(*((int*)_tmp11D)== Cyc_Absyn_Var_e){_LL408: _tmp197=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp11D)->f2;if((unsigned int)_tmp197 > 1?*((
int*)_tmp197)== Cyc_Absyn_Param_b: 0){_LL410: _tmp199=((struct Cyc_Absyn_Param_b_struct*)
_tmp197)->f1;goto _LL320;}else{goto _LL321;}}else{goto _LL321;}_LL321: if(*((int*)
_tmp11D)== Cyc_Absyn_Var_e){_LL412: _tmp19B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp11D)->f2;if((unsigned int)_tmp19B > 1?*((int*)_tmp19B)== Cyc_Absyn_Local_b: 0){
_LL414: _tmp19D=((struct Cyc_Absyn_Local_b_struct*)_tmp19B)->f1;goto _LL322;}else{
goto _LL323;}}else{goto _LL323;}_LL323: if(*((int*)_tmp11D)== Cyc_Absyn_Var_e){
_LL416: _tmp19F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp11D)->f2;if((
unsigned int)_tmp19F > 1?*((int*)_tmp19F)== Cyc_Absyn_Pat_b: 0){_LL418: _tmp1A1=((
struct Cyc_Absyn_Pat_b_struct*)_tmp19F)->f1;goto _LL324;}else{goto _LL325;}}else{
goto _LL325;}_LL325: if(*((int*)_tmp11D)== Cyc_Absyn_Primop_e){_LL420: _tmp1A3=((
struct Cyc_Absyn_Primop_e_struct*)_tmp11D)->f2;goto _LL326;}else{goto _LL327;}
_LL327: if(*((int*)_tmp11D)== Cyc_Absyn_Increment_e){_LL422: _tmp1A5=((struct Cyc_Absyn_Increment_e_struct*)
_tmp11D)->f1;goto _LL328;}else{goto _LL329;}_LL329: if(*((int*)_tmp11D)== Cyc_Absyn_AssignOp_e){
_LL429: _tmp1AC=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp11D)->f1;goto _LL426;
_LL426: _tmp1A9=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp11D)->f2;if(_tmp1A9 == 0){
goto _LL331;}else{_tmp1AB=*_tmp1A9;goto _LL424;}_LL424: _tmp1A7=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp11D)->f3;goto _LL330;}else{goto _LL331;}_LL331: if(*((int*)_tmp11D)== Cyc_Absyn_AssignOp_e){
_LL435: _tmp1B2=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp11D)->f1;goto _LL433;
_LL433: _tmp1B0=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp11D)->f2;if(_tmp1B0 == 0){
goto _LL431;}else{goto _LL333;}_LL431: _tmp1AE=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp11D)->f3;goto _LL332;}else{goto _LL333;}_LL333: if(*((int*)_tmp11D)== Cyc_Absyn_SeqExp_e){
_LL439: _tmp1B6=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp11D)->f1;goto _LL437;_LL437:
_tmp1B4=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp11D)->f2;goto _LL334;}else{goto
_LL335;}_LL335: if(*((int*)_tmp11D)== Cyc_Absyn_Throw_e){_LL441: _tmp1B8=((struct
Cyc_Absyn_Throw_e_struct*)_tmp11D)->f1;goto _LL336;}else{goto _LL337;}_LL337: if(*((
int*)_tmp11D)== Cyc_Absyn_FnCall_e){_LL445: _tmp1BC=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp11D)->f1;goto _LL443;_LL443: _tmp1BA=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp11D)->f2;goto _LL338;}else{goto _LL339;}_LL339: if(*((int*)_tmp11D)== Cyc_Absyn_Malloc_e){
_LL447: _tmp1BE=((struct Cyc_Absyn_Malloc_e_struct*)_tmp11D)->f1;_LL457: _tmp1C8=
_tmp1BE.is_calloc;goto _LL455;_LL455: _tmp1C6=_tmp1BE.rgn;goto _LL453;_LL453:
_tmp1C4=_tmp1BE.elt_type;goto _LL451;_LL451: _tmp1C2=_tmp1BE.num_elts;goto _LL449;
_LL449: _tmp1C0=_tmp1BE.fat_result;goto _LL340;}else{goto _LL341;}_LL341: if(*((int*)
_tmp11D)== Cyc_Absyn_New_e){_LL461: _tmp1CC=((struct Cyc_Absyn_New_e_struct*)
_tmp11D)->f1;goto _LL459;_LL459: _tmp1CA=((struct Cyc_Absyn_New_e_struct*)_tmp11D)->f2;
goto _LL342;}else{goto _LL343;}_LL343: if(*((int*)_tmp11D)== Cyc_Absyn_Address_e){
_LL463: _tmp1CE=((struct Cyc_Absyn_Address_e_struct*)_tmp11D)->f1;goto _LL344;}
else{goto _LL345;}_LL345: if(*((int*)_tmp11D)== Cyc_Absyn_Deref_e){_LL465: _tmp1D0=((
struct Cyc_Absyn_Deref_e_struct*)_tmp11D)->f1;goto _LL346;}else{goto _LL347;}_LL347:
if(*((int*)_tmp11D)== Cyc_Absyn_StructMember_e){_LL469: _tmp1D4=((struct Cyc_Absyn_StructMember_e_struct*)
_tmp11D)->f1;goto _LL467;_LL467: _tmp1D2=((struct Cyc_Absyn_StructMember_e_struct*)
_tmp11D)->f2;goto _LL348;}else{goto _LL349;}_LL349: if(*((int*)_tmp11D)== Cyc_Absyn_StructArrow_e){
_LL473: _tmp1D8=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp11D)->f1;goto _LL471;
_LL471: _tmp1D6=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp11D)->f2;goto _LL350;}
else{goto _LL351;}_LL351: if(*((int*)_tmp11D)== Cyc_Absyn_Conditional_e){_LL479:
_tmp1DE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp11D)->f1;goto _LL477;_LL477:
_tmp1DC=((struct Cyc_Absyn_Conditional_e_struct*)_tmp11D)->f2;goto _LL475;_LL475:
_tmp1DA=((struct Cyc_Absyn_Conditional_e_struct*)_tmp11D)->f3;goto _LL352;}else{
goto _LL353;}_LL353: if(*((int*)_tmp11D)== Cyc_Absyn_Subscript_e){_LL483: _tmp1E2=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp11D)->f1;goto _LL481;_LL481: _tmp1E0=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp11D)->f2;goto _LL354;}else{goto _LL355;}
_LL355: if(*((int*)_tmp11D)== Cyc_Absyn_Tunion_e){_LL485: _tmp1E4=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp11D)->f3;goto _LL356;}else{goto _LL357;}_LL357: if(*((int*)_tmp11D)== Cyc_Absyn_Tuple_e){
_LL487: _tmp1E6=((struct Cyc_Absyn_Tuple_e_struct*)_tmp11D)->f1;goto _LL358;}else{
goto _LL359;}_LL359: if(*((int*)_tmp11D)== Cyc_Absyn_AnonStruct_e){_LL489: _tmp1E8=((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp11D)->f2;goto _LL360;}else{goto _LL361;}
_LL361: if(*((int*)_tmp11D)== Cyc_Absyn_Struct_e){_LL491: _tmp1EA=((struct Cyc_Absyn_Struct_e_struct*)
_tmp11D)->f3;goto _LL362;}else{goto _LL363;}_LL363: if(*((int*)_tmp11D)== Cyc_Absyn_Array_e){
_LL493: _tmp1EC=((struct Cyc_Absyn_Array_e_struct*)_tmp11D)->f1;goto _LL364;}else{
goto _LL365;}_LL365: if(*((int*)_tmp11D)== Cyc_Absyn_Comprehension_e){_LL499:
_tmp1F2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp11D)->f1;goto _LL497;_LL497:
_tmp1F0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp11D)->f2;goto _LL495;_LL495:
_tmp1EE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp11D)->f3;goto _LL366;}else{
goto _LL367;}_LL367: if(*((int*)_tmp11D)== Cyc_Absyn_StmtExp_e){_LL501: _tmp1F4=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp11D)->f1;goto _LL368;}else{goto _LL369;}
_LL369: if(*((int*)_tmp11D)== Cyc_Absyn_Var_e){_LL503: _tmp1F6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp11D)->f2;if(_tmp1F6 == (void*)Cyc_Absyn_Unresolved_b){goto _LL370;}else{goto
_LL371;}}else{goto _LL371;}_LL371: if(*((int*)_tmp11D)== Cyc_Absyn_UnknownId_e){
goto _LL372;}else{goto _LL373;}_LL373: if(*((int*)_tmp11D)== Cyc_Absyn_UnknownCall_e){
goto _LL374;}else{goto _LL375;}_LL375: if(*((int*)_tmp11D)== Cyc_Absyn_UnresolvedMem_e){
goto _LL376;}else{goto _LL377;}_LL377: if(*((int*)_tmp11D)== Cyc_Absyn_CompoundLit_e){
goto _LL378;}else{goto _LL379;}_LL379: if(*((int*)_tmp11D)== Cyc_Absyn_Codegen_e){
goto _LL380;}else{goto _LL381;}_LL381: if(*((int*)_tmp11D)== Cyc_Absyn_Fill_e){goto
_LL382;}else{goto _LL286;}_LL288: _tmp181=_tmp17F;goto _LL290;_LL290: _tmp183=
_tmp181;goto _LL292;_LL292: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp183);
_LL294: goto _LL296;_LL296: return({struct _tuple0 _tmp1F8;_tmp1F8.f1=inflow;_tmp1F8.f2=(
void*)Cyc_CfFlowInfo_Zero;_tmp1F8;});_LL298: goto _LL300;_LL300: return({struct
_tuple0 _tmp1F9;_tmp1F9.f1=inflow;_tmp1F9.f2=(void*)Cyc_CfFlowInfo_NotZeroAll;
_tmp1F9;});_LL302: goto _LL304;_LL304: goto _LL306;_LL306: goto _LL308;_LL308: goto
_LL310;_LL310: goto _LL312;_LL312: goto _LL314;_LL314: goto _LL316;_LL316: return({
struct _tuple0 _tmp1FA;_tmp1FA.f1=inflow;_tmp1FA.f2=Cyc_CfFlowInfo_unknown_all;
_tmp1FA;});_LL318: return({struct _tuple0 _tmp1FB;_tmp1FB.f1=inflow;_tmp1FB.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp1FB;});_LL320: _tmp19D=_tmp199;goto _LL322;_LL322: _tmp1A1=_tmp19D;goto _LL324;
_LL324: return({struct _tuple0 _tmp1FC;_tmp1FC.f1=inflow;_tmp1FC.f2=Cyc_Dict_lookup(
d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp1FD=_cycalloc(sizeof(struct
Cyc_CfFlowInfo_VarRoot_struct));_tmp1FD[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp1FE;_tmp1FE.tag=Cyc_CfFlowInfo_VarRoot;_tmp1FE.f1=_tmp1A1;_tmp1FE;});_tmp1FD;}));
_tmp1FC;});_LL326: return Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp1A3);
_LL328: {void*_tmp202;struct _tuple0 _tmp200=Cyc_NewControlFlow_anal_use_ints(env,
inflow,({struct Cyc_Absyn_Exp*_tmp1FF[1];_tmp1FF[0]=_tmp1A5;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp1FF,sizeof(struct Cyc_Absyn_Exp*),
1));}));_LL515: _tmp202=_tmp200.f1;goto _LL513;_LL513: {void*_tmp206;struct _tuple0
_tmp204=Cyc_NewControlFlow_anal_Lexp(env,_tmp202,_tmp1A5);_LL519: _tmp206=_tmp204.f2;
goto _LL517;_LL517:{struct _tuple0 _tmp209=({struct _tuple0 _tmp208;_tmp208.f1=
_tmp206;_tmp208.f2=_tmp202;_tmp208;});void*_tmp20F;struct Cyc_List_List*_tmp211;
struct Cyc_Dict_Dict*_tmp213;void*_tmp215;struct Cyc_CfFlowInfo_Place*_tmp217;
_LL523: _LL534: _tmp215=_tmp209.f1;if((unsigned int)_tmp215 > 1?*((int*)_tmp215)== 
Cyc_CfFlowInfo_PlaceL: 0){_LL536: _tmp217=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp215)->f1;goto _LL528;}else{goto _LL525;}_LL528: _tmp20F=_tmp209.f2;if((
unsigned int)_tmp20F > 1?*((int*)_tmp20F)== Cyc_CfFlowInfo_ReachableFL: 0){_LL532:
_tmp213=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp20F)->f1;goto _LL530;
_LL530: _tmp211=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp20F)->f2;goto
_LL524;}else{goto _LL525;}_LL525: goto _LL526;_LL524: _tmp211=Cyc_CfFlowInfo_reln_kill_exp(
_tmp211,_tmp1A5);_tmp202=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp219=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp219[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp21A;_tmp21A.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp21A.f1=Cyc_CfFlowInfo_assign_place(_tmp1A5->loc,_tmp213,env->all_changed,
_tmp217,Cyc_CfFlowInfo_unknown_all);_tmp21A.f2=_tmp211;_tmp21A;});_tmp219;});
goto _LL522;_LL526: goto _LL522;_LL522:;}return({struct _tuple0 _tmp21B;_tmp21B.f1=
_tmp202;_tmp21B.f2=Cyc_CfFlowInfo_unknown_all;_tmp21B;});}}_LL330: {void*_tmp21F;
struct _tuple0 _tmp21D=Cyc_NewControlFlow_anal_use_ints(env,inflow,({struct Cyc_Absyn_Exp*
_tmp21C[2];_tmp21C[1]=_tmp1A7;_tmp21C[0]=_tmp1AC;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp21C,sizeof(struct Cyc_Absyn_Exp*),
2));}));_LL544: _tmp21F=_tmp21D.f1;goto _LL542;_LL542: {void*_tmp223;struct _tuple0
_tmp221=Cyc_NewControlFlow_anal_Lexp(env,_tmp21F,e);_LL548: _tmp223=_tmp221.f2;
goto _LL546;_LL546:{struct _tuple0 _tmp226=({struct _tuple0 _tmp225;_tmp225.f1=
_tmp223;_tmp225.f2=_tmp21F;_tmp225;});void*_tmp22C;struct Cyc_List_List*_tmp22E;
struct Cyc_Dict_Dict*_tmp230;void*_tmp232;struct Cyc_CfFlowInfo_Place*_tmp234;
_LL552: _LL563: _tmp232=_tmp226.f1;if((unsigned int)_tmp232 > 1?*((int*)_tmp232)== 
Cyc_CfFlowInfo_PlaceL: 0){_LL565: _tmp234=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp232)->f1;goto _LL557;}else{goto _LL554;}_LL557: _tmp22C=_tmp226.f2;if((
unsigned int)_tmp22C > 1?*((int*)_tmp22C)== Cyc_CfFlowInfo_ReachableFL: 0){_LL561:
_tmp230=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp22C)->f1;goto _LL559;
_LL559: _tmp22E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp22C)->f2;goto
_LL553;}else{goto _LL554;}_LL554: goto _LL555;_LL553: _tmp22E=Cyc_CfFlowInfo_reln_kill_exp(
_tmp22E,_tmp1AC);_tmp21F=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp236=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp236[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp237;_tmp237.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp237.f1=Cyc_CfFlowInfo_assign_place(_tmp1AC->loc,_tmp230,env->all_changed,
_tmp234,Cyc_CfFlowInfo_unknown_all);_tmp237.f2=_tmp22E;_tmp237;});_tmp236;});
goto _LL551;_LL555: goto _LL551;_LL551:;}return({struct _tuple0 _tmp238;_tmp238.f1=
_tmp21F;_tmp238.f2=Cyc_CfFlowInfo_unknown_all;_tmp238;});}}_LL332: {struct Cyc_Set_Set**
_tmp239=env->all_changed;while(1){env->all_changed=({struct Cyc_Set_Set**_tmp23A=
_cycalloc(sizeof(struct Cyc_Set_Set*));_tmp23A[0]=Cyc_CfFlowInfo_mt_place_set();
_tmp23A;});{void*_tmp23D;void*_tmp23F;struct _tuple0 _tmp23B=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmp1B2);_LL576: _tmp23F=_tmp23B.f1;goto _LL574;_LL574: _tmp23D=_tmp23B.f2;
goto _LL572;_LL572: {struct Cyc_Set_Set*_tmp241=*((struct Cyc_Set_Set**)_check_null(
env->all_changed));env->all_changed=({struct Cyc_Set_Set**_tmp242=_cycalloc(
sizeof(struct Cyc_Set_Set*));_tmp242[0]=Cyc_CfFlowInfo_mt_place_set();_tmp242;});{
void*_tmp245;void*_tmp247;struct _tuple0 _tmp243=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1AE);_LL584: _tmp247=_tmp243.f1;goto _LL582;_LL582: _tmp245=_tmp243.f2;
goto _LL580;_LL580: {struct Cyc_Set_Set*_tmp249=*((struct Cyc_Set_Set**)_check_null(
env->all_changed));void*_tmp24A=Cyc_CfFlowInfo_after_flow((struct Cyc_Set_Set**)&
_tmp241,_tmp23F,_tmp247,_tmp241,_tmp249);void*_tmp24B=Cyc_CfFlowInfo_join_flow(
_tmp239,inflow,_tmp24A);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp24B,inflow)){
if(_tmp239 == 0){env->all_changed=0;}else{*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*
_tmp239,((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp241,_tmp249));}{void*_tmp24C=_tmp24A;struct Cyc_List_List*_tmp252;struct Cyc_Dict_Dict*
_tmp254;_LL590: if(_tmp24C == (void*)Cyc_CfFlowInfo_BottomFL){goto _LL591;}else{
goto _LL592;}_LL592: if((unsigned int)_tmp24C > 1?*((int*)_tmp24C)== Cyc_CfFlowInfo_ReachableFL:
0){_LL597: _tmp254=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp24C)->f1;goto
_LL595;_LL595: _tmp252=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp24C)->f2;
goto _LL593;}else{goto _LL589;}_LL591: return({struct _tuple0 _tmp256;_tmp256.f1=(
void*)Cyc_CfFlowInfo_BottomFL;_tmp256.f2=_tmp245;_tmp256;});_LL593: {void*
_tmp257=_tmp23D;struct Cyc_CfFlowInfo_Place*_tmp25D;_LL601: if((unsigned int)
_tmp257 > 1?*((int*)_tmp257)== Cyc_CfFlowInfo_PlaceL: 0){_LL606: _tmp25D=((struct
Cyc_CfFlowInfo_PlaceL_struct*)_tmp257)->f1;goto _LL602;}else{goto _LL603;}_LL603:
if(_tmp257 == (void*)Cyc_CfFlowInfo_UnknownL){goto _LL604;}else{goto _LL600;}_LL602:
_tmp254=Cyc_CfFlowInfo_assign_place(e->loc,_tmp254,env->all_changed,_tmp25D,
_tmp245);_tmp252=Cyc_CfFlowInfo_reln_assign_exp(_tmp252,_tmp1B2,_tmp1AE);_tmp24A=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp25F=_cycalloc(sizeof(struct
Cyc_CfFlowInfo_ReachableFL_struct));_tmp25F[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp260;_tmp260.tag=Cyc_CfFlowInfo_ReachableFL;_tmp260.f1=_tmp254;_tmp260.f2=
_tmp252;_tmp260;});_tmp25F;});Cyc_NewControlFlow_update_tryflow(env,_tmp24A);
return({struct _tuple0 _tmp261;_tmp261.f1=_tmp24A;_tmp261.f2=_tmp245;_tmp261;});
_LL604: return({struct _tuple0 _tmp262;_tmp262.f1=Cyc_NewControlFlow_use_Rval(env,
_tmp1AE->loc,_tmp24A,_tmp245);_tmp262.f2=_tmp245;_tmp262;});_LL600:;}_LL589:;}}
inflow=_tmp24B;}}}}}}_LL334: return Cyc_NewControlFlow_anal_Rexp(env,(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1B6)).f1,_tmp1B4);_LL336: {void*_tmp265;void*_tmp267;struct
_tuple0 _tmp263=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp1B8);_LL616: _tmp267=
_tmp263.f1;goto _LL614;_LL614: _tmp265=_tmp263.f2;goto _LL612;_LL612: Cyc_NewControlFlow_use_Rval(
env,_tmp1B8->loc,_tmp267,_tmp265);return({struct _tuple0 _tmp269;_tmp269.f1=(void*)
Cyc_CfFlowInfo_BottomFL;_tmp269.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp269;});}
_LL338: _tmp1BA=({struct Cyc_List_List*_tmp26A=_cycalloc(sizeof(struct Cyc_List_List));
_tmp26A->hd=_tmp1BC;_tmp26A->tl=_tmp1BA;_tmp26A;});{struct Cyc_List_List*_tmp26D;
void*_tmp26F;struct _tuple4 _tmp26B=Cyc_NewControlFlow_anal_unordered_Rexps(env,
inflow,_tmp1BA);_LL624: _tmp26F=_tmp26B.f1;goto _LL622;_LL622: _tmp26D=_tmp26B.f2;
goto _LL620;_LL620: for(0;_tmp26D != 0;(_tmp26D=_tmp26D->tl,_tmp1BA=((struct Cyc_List_List*)
_check_null(_tmp1BA))->tl)){_tmp26F=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp1BA))->hd)->loc,_tmp26F,(void*)_tmp26D->hd);}
if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1BC->topt))->v)){
return({struct _tuple0 _tmp271;_tmp271.f1=(void*)Cyc_CfFlowInfo_BottomFL;_tmp271.f2=
Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,
Cyc_CfFlowInfo_unknown_all);_tmp271;});}else{return({struct _tuple0 _tmp272;
_tmp272.f1=_tmp26F;_tmp272.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp272;});}}_LL340: {void*
root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*_tmp28F=_cycalloc(sizeof(
struct Cyc_CfFlowInfo_MallocPt_struct));_tmp28F[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp290;_tmp290.tag=Cyc_CfFlowInfo_MallocPt;_tmp290.f1=_tmp1C2;_tmp290;});
_tmp28F;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*_tmp28E=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_tmp28E->root=(void*)root;_tmp28E->fields=
0;_tmp28E;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*_tmp28C=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_struct));_tmp28C[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp28D;_tmp28D.tag=Cyc_CfFlowInfo_AddressOf;_tmp28D.f1=place;_tmp28D;});_tmp28C;});
void*place_val=_tmp1C0?(void*)Cyc_CfFlowInfo_NotZeroAll: Cyc_CfFlowInfo_typ_to_absrval(*((
void**)_check_null(_tmp1C4)),Cyc_CfFlowInfo_unknown_none);void*outflow;if(env->all_changed
!= 0){*((struct Cyc_Set_Set**)_check_null(env->all_changed))=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s,struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(env->all_changed)),place);}if(_tmp1C6 != 0){struct Cyc_List_List*
_tmp276;void*_tmp278;struct _tuple4 _tmp274=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp273[2];_tmp273[1]=_tmp1C2;_tmp273[0]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp1C6);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp273,sizeof(struct Cyc_Absyn_Exp*),2));}));
_LL633: _tmp278=_tmp274.f1;goto _LL631;_LL631: _tmp276=_tmp274.f2;goto _LL629;_LL629:
outflow=_tmp278;}else{void*_tmp27C;struct _tuple0 _tmp27A=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1C2);_LL637: _tmp27C=_tmp27A.f1;goto _LL635;_LL635: outflow=_tmp27C;}{
void*_tmp27E=outflow;struct Cyc_List_List*_tmp284;struct Cyc_Dict_Dict*_tmp286;
_LL640: if(_tmp27E == (void*)Cyc_CfFlowInfo_BottomFL){goto _LL641;}else{goto _LL642;}
_LL642: if((unsigned int)_tmp27E > 1?*((int*)_tmp27E)== Cyc_CfFlowInfo_ReachableFL:
0){_LL647: _tmp286=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp27E)->f1;goto
_LL645;_LL645: _tmp284=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp27E)->f2;
goto _LL643;}else{goto _LL639;}_LL641: return({struct _tuple0 _tmp288;_tmp288.f1=
outflow;_tmp288.f2=rval;_tmp288;});_LL643: return({struct _tuple0 _tmp289;_tmp289.f1=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp28A=_cycalloc(sizeof(struct
Cyc_CfFlowInfo_ReachableFL_struct));_tmp28A[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp28B;_tmp28B.tag=Cyc_CfFlowInfo_ReachableFL;_tmp28B.f1=Cyc_Dict_insert(
_tmp286,root,place_val);_tmp28B.f2=_tmp284;_tmp28B;});_tmp28A;});_tmp289.f2=rval;
_tmp289;});_LL639:;}}_LL342: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp2AF=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_struct));_tmp2AF[0]=({
struct Cyc_CfFlowInfo_MallocPt_struct _tmp2B0;_tmp2B0.tag=Cyc_CfFlowInfo_MallocPt;
_tmp2B0.f1=_tmp1CA;_tmp2B0;});_tmp2AF;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp2AE=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));
_tmp2AE->root=(void*)root;_tmp2AE->fields=0;_tmp2AE;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp2AC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_struct));
_tmp2AC[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp2AD;_tmp2AD.tag=Cyc_CfFlowInfo_AddressOf;
_tmp2AD.f1=place;_tmp2AD;});_tmp2AC;});if(env->all_changed != 0){*((struct Cyc_Set_Set**)
_check_null(env->all_changed))=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,
struct Cyc_CfFlowInfo_Place*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)
_check_null(env->all_changed)),place);}{void*outflow;void*place_val;if(_tmp1CC != 
0){struct Cyc_List_List*_tmp294;void*_tmp296;struct _tuple4 _tmp292=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,({struct Cyc_Absyn_Exp*_tmp291[2];_tmp291[1]=_tmp1CA;_tmp291[0]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp1CC);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp291,sizeof(struct Cyc_Absyn_Exp*),2));}));
_LL663: _tmp296=_tmp292.f1;goto _LL661;_LL661: _tmp294=_tmp292.f2;goto _LL659;_LL659:
outflow=_tmp296;place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp294))->tl))->hd;}else{void*_tmp29A;void*_tmp29C;struct _tuple0
_tmp298=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp1CA);_LL669: _tmp29C=_tmp298.f1;
goto _LL667;_LL667: _tmp29A=_tmp298.f2;goto _LL665;_LL665: outflow=_tmp29C;place_val=
_tmp29A;}{void*_tmp29E=outflow;struct Cyc_List_List*_tmp2A4;struct Cyc_Dict_Dict*
_tmp2A6;_LL672: if(_tmp29E == (void*)Cyc_CfFlowInfo_BottomFL){goto _LL673;}else{
goto _LL674;}_LL674: if((unsigned int)_tmp29E > 1?*((int*)_tmp29E)== Cyc_CfFlowInfo_ReachableFL:
0){_LL679: _tmp2A6=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp29E)->f1;goto
_LL677;_LL677: _tmp2A4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp29E)->f2;
goto _LL675;}else{goto _LL671;}_LL673: return({struct _tuple0 _tmp2A8;_tmp2A8.f1=
outflow;_tmp2A8.f2=rval;_tmp2A8;});_LL675: return({struct _tuple0 _tmp2A9;_tmp2A9.f1=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp2AA=_cycalloc(sizeof(struct
Cyc_CfFlowInfo_ReachableFL_struct));_tmp2AA[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp2AB;_tmp2AB.tag=Cyc_CfFlowInfo_ReachableFL;_tmp2AB.f1=Cyc_Dict_insert(
_tmp2A6,root,place_val);_tmp2AB.f2=_tmp2A4;_tmp2AB;});_tmp2AA;});_tmp2A9.f2=rval;
_tmp2A9;});_LL671:;}}}_LL344: {void*_tmp2B3;void*_tmp2B5;struct _tuple0 _tmp2B1=
Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmp1CE);_LL694: _tmp2B5=_tmp2B1.f1;goto
_LL692;_LL692: _tmp2B3=_tmp2B1.f2;goto _LL690;_LL690: {void*_tmp2B7=_tmp2B3;struct
Cyc_CfFlowInfo_Place*_tmp2BD;_LL697: if(_tmp2B7 == (void*)Cyc_CfFlowInfo_UnknownL){
goto _LL698;}else{goto _LL699;}_LL699: if((unsigned int)_tmp2B7 > 1?*((int*)_tmp2B7)
== Cyc_CfFlowInfo_PlaceL: 0){_LL702: _tmp2BD=((struct Cyc_CfFlowInfo_PlaceL_struct*)
_tmp2B7)->f1;goto _LL700;}else{goto _LL696;}_LL698: return({struct _tuple0 _tmp2BF;
_tmp2BF.f1=_tmp2B5;_tmp2BF.f2=(void*)Cyc_CfFlowInfo_NotZeroAll;_tmp2BF;});_LL700:
return({struct _tuple0 _tmp2C0;_tmp2C0.f1=_tmp2B5;_tmp2C0.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp2C1=_cycalloc(sizeof(struct Cyc_CfFlowInfo_AddressOf_struct));_tmp2C1[0]=({
struct Cyc_CfFlowInfo_AddressOf_struct _tmp2C2;_tmp2C2.tag=Cyc_CfFlowInfo_AddressOf;
_tmp2C2.f1=_tmp2BD;_tmp2C2;});_tmp2C1;});_tmp2C0;});_LL696:;}}_LL346: {void*
_tmp2C5;void*_tmp2C7;struct _tuple0 _tmp2C3=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1D0);_LL712: _tmp2C7=_tmp2C3.f1;goto _LL710;_LL710: _tmp2C5=_tmp2C3.f2;
goto _LL708;_LL708: return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2C7,
_tmp1D0,_tmp2C5);}_LL348: {void*_tmp2CB;void*_tmp2CD;struct _tuple0 _tmp2C9=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1D4);_LL718: _tmp2CD=_tmp2C9.f1;goto _LL716;_LL716: _tmp2CB=_tmp2C9.f2;
goto _LL714;_LL714:{void*_tmp2CF=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1D4->topt))->v);_LL721: if((unsigned int)_tmp2CF > 3?*((int*)
_tmp2CF)== Cyc_Absyn_AnonUnionType: 0){goto _LL722;}else{goto _LL723;}_LL723: if((
unsigned int)_tmp2CF > 3?*((int*)_tmp2CF)== Cyc_Absyn_UnionType: 0){goto _LL724;}
else{goto _LL725;}_LL725: goto _LL726;_LL722: goto _LL724;_LL724: return({struct
_tuple0 _tmp2D7;_tmp2D7.f1=_tmp2CD;_tmp2D7.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp2D7;});
_LL726: goto _LL720;_LL720:;}{void*_tmp2D8=_tmp2CB;struct Cyc_Dict_Dict*_tmp2DE;
_LL730: if((unsigned int)_tmp2D8 > 3?*((int*)_tmp2D8)== Cyc_CfFlowInfo_Aggregate: 0){
_LL735: _tmp2DE=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp2D8)->f1;goto _LL731;}
else{goto _LL732;}_LL732: goto _LL733;_LL731: return({struct _tuple0 _tmp2E0;_tmp2E0.f1=
_tmp2CD;_tmp2E0.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp2DE,_tmp1D2);_tmp2E0;});_LL733:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2E1=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp2E1[0]=({struct
Cyc_Core_Impossible_struct _tmp2E2;_tmp2E2.tag=Cyc_Core_Impossible;_tmp2E2.f1=
_tag_arr("anal_Rexp: StructMember",sizeof(unsigned char),24);_tmp2E2;});_tmp2E1;}));
_LL729:;}}_LL350: {void*_tmp2E5;void*_tmp2E7;struct _tuple0 _tmp2E3=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp1D8);_LL744: _tmp2E7=_tmp2E3.f1;goto _LL742;_LL742: _tmp2E5=_tmp2E3.f2;
goto _LL740;_LL740: {void*_tmp2EB;void*_tmp2ED;struct _tuple0 _tmp2E9=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp2E7,_tmp1D8,_tmp2E5);_LL750: _tmp2ED=_tmp2E9.f1;goto _LL748;_LL748:
_tmp2EB=_tmp2E9.f2;goto _LL746;_LL746:{void*_tmp2EF=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1D8->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp2F5;void*_tmp2F7;_LL753: if((unsigned int)_tmp2EF > 3?*((int*)_tmp2EF)== Cyc_Absyn_PointerType:
0){_LL758: _tmp2F5=((struct Cyc_Absyn_PointerType_struct*)_tmp2EF)->f1;_LL760:
_tmp2F7=(void*)_tmp2F5.elt_typ;goto _LL754;}else{goto _LL755;}_LL755: goto _LL756;
_LL754:{void*_tmp2F9=Cyc_Tcutil_compress(_tmp2F7);_LL763: if((unsigned int)
_tmp2F9 > 3?*((int*)_tmp2F9)== Cyc_Absyn_AnonUnionType: 0){goto _LL764;}else{goto
_LL765;}_LL765: if((unsigned int)_tmp2F9 > 3?*((int*)_tmp2F9)== Cyc_Absyn_UnionType:
0){goto _LL766;}else{goto _LL767;}_LL767: goto _LL768;_LL764: goto _LL766;_LL766:
return({struct _tuple0 _tmp301;_tmp301.f1=_tmp2ED;_tmp301.f2=Cyc_CfFlowInfo_typ_to_absrval((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);
_tmp301;});_LL768: goto _LL762;_LL762:;}goto _LL752;_LL756:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp302=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));
_tmp302[0]=({struct Cyc_Core_Impossible_struct _tmp303;_tmp303.tag=Cyc_Core_Impossible;
_tmp303.f1=_tag_arr("anal_Rexp: StructArrow ptr",sizeof(unsigned char),27);
_tmp303;});_tmp302;}));_LL752:;}{void*_tmp304=_tmp2EB;struct Cyc_Dict_Dict*
_tmp30A;_LL774: if((unsigned int)_tmp304 > 3?*((int*)_tmp304)== Cyc_CfFlowInfo_Aggregate:
0){_LL779: _tmp30A=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp304)->f1;goto
_LL775;}else{goto _LL776;}_LL776: goto _LL777;_LL775: return({struct _tuple0 _tmp30C;
_tmp30C.f1=_tmp2ED;_tmp30C.f2=((void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*
k))Cyc_Dict_lookup)(_tmp30A,_tmp1D6);_tmp30C;});_LL777:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp30D=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));
_tmp30D[0]=({struct Cyc_Core_Impossible_struct _tmp30E;_tmp30E.tag=Cyc_Core_Impossible;
_tmp30E.f1=_tag_arr("anal_Rexp: StructArrow",sizeof(unsigned char),23);_tmp30E;});
_tmp30D;}));_LL773:;}}}_LL352: {void*_tmp311;void*_tmp313;struct _tuple0 _tmp30F=
Cyc_NewControlFlow_anal_test(env,inflow,_tmp1DE);_LL788: _tmp313=_tmp30F.f1;goto
_LL786;_LL786: _tmp311=_tmp30F.f2;goto _LL784;_LL784: {void*_tmp317;void*_tmp319;
struct _tuple0 _tmp315=Cyc_NewControlFlow_anal_Rexp(env,_tmp313,_tmp1DC);_LL794:
_tmp319=_tmp315.f1;goto _LL792;_LL792: _tmp317=_tmp315.f2;goto _LL790;_LL790: {void*
_tmp31D;void*_tmp31F;struct _tuple0 _tmp31B=Cyc_NewControlFlow_anal_Rexp(env,
_tmp311,_tmp1DA);_LL800: _tmp31F=_tmp31B.f1;goto _LL798;_LL798: _tmp31D=_tmp31B.f2;
goto _LL796;_LL796: return({struct _tuple0 _tmp321;_tmp321.f1=Cyc_CfFlowInfo_join_flow(
env->all_changed,Cyc_NewControlFlow_use_Rval(env,_tmp1DC->loc,_tmp319,_tmp317),
Cyc_NewControlFlow_use_Rval(env,_tmp1DA->loc,_tmp31F,_tmp31D));_tmp321.f2=Cyc_CfFlowInfo_unknown_all;
_tmp321;});}}}_LL354: {struct Cyc_List_List*_tmp325;void*_tmp327;struct _tuple4
_tmp323=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,({struct Cyc_Absyn_Exp*
_tmp322[2];_tmp322[1]=_tmp1E0;_tmp322[0]=_tmp1E2;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp322,sizeof(struct Cyc_Absyn_Exp*),
2));}));_LL808: _tmp327=_tmp323.f1;goto _LL806;_LL806: _tmp325=_tmp323.f2;goto
_LL804;_LL804:{void*_tmp329=_tmp327;struct Cyc_List_List*_tmp32F;struct Cyc_Dict_Dict*
_tmp331;_LL811: if((unsigned int)_tmp329 > 1?*((int*)_tmp329)== Cyc_CfFlowInfo_ReachableFL:
0){_LL818: _tmp331=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp329)->f1;goto
_LL816;_LL816: _tmp32F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp329)->f2;
goto _LL812;}else{goto _LL813;}_LL813: goto _LL814;_LL812: if(Cyc_CfFlowInfo_initlevel(
_tmp331,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp325))->tl))->hd)== (void*)Cyc_CfFlowInfo_NoneIL){({void*_tmp333[
0]={};Cyc_Tcutil_terr(_tmp1E0->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp333,sizeof(void*),0));});}{struct Cyc_List_List*
_tmp334=Cyc_NewControlFlow_add_subscript_reln(_tmp32F,_tmp1E2,_tmp1E0);if(
_tmp32F != _tmp334){_tmp327=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp335=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp335[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp336;_tmp336.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp336.f1=_tmp331;_tmp336.f2=_tmp334;_tmp336;});_tmp335;});}goto _LL810;}_LL814:
goto _LL810;_LL810:;}{void*_tmp337=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1E2->topt))->v);_LL825: if((unsigned int)_tmp337 > 3?*((int*)
_tmp337)== Cyc_Absyn_TupleType: 0){goto _LL826;}else{goto _LL827;}_LL827: goto _LL828;
_LL826: {void*_tmp33D=(void*)((struct Cyc_List_List*)_check_null(_tmp325))->hd;
struct Cyc_Dict_Dict*_tmp343;_LL831: if((unsigned int)_tmp33D > 3?*((int*)_tmp33D)
== Cyc_CfFlowInfo_Aggregate: 0){_LL836: _tmp343=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp33D)->f1;goto _LL832;}else{goto _LL833;}_LL833: goto _LL834;_LL832: return({
struct _tuple0 _tmp345;_tmp345.f1=_tmp327;_tmp345.f2=((void*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp343,Cyc_Absyn_fieldname((int)Cyc_Evexp_eval_const_uint_exp(
_tmp1E0)));_tmp345;});_LL834:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp346=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp346[0]=({struct
Cyc_Core_Impossible_struct _tmp347;_tmp347.tag=Cyc_Core_Impossible;_tmp347.f1=
_tag_arr("anal_Rexp: Subscript",sizeof(unsigned char),21);_tmp347;});_tmp346;}));
_LL830:;}_LL828: goto _LL824;_LL824:;}return Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp327,_tmp1E2,(void*)((struct Cyc_List_List*)_check_null(_tmp325))->hd);}
_LL356: _tmp1E6=_tmp1E4;goto _LL358;_LL358: {struct Cyc_List_List*_tmp34A;void*
_tmp34C;struct _tuple4 _tmp348=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,
_tmp1E6);_LL845: _tmp34C=_tmp348.f1;goto _LL843;_LL843: _tmp34A=_tmp348.f2;goto
_LL841;_LL841: {struct Cyc_Dict_Dict*aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);{int
i=0;for(0;_tmp34A != 0;(_tmp34A=_tmp34A->tl,++ i)){aggrdict=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,Cyc_Absyn_fieldname(
i),(void*)_tmp34A->hd);}}return({struct _tuple0 _tmp34E;_tmp34E.f1=_tmp34C;_tmp34E.f2=(
void*)({struct Cyc_CfFlowInfo_Aggregate_struct*_tmp34F=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));
_tmp34F[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp350;_tmp350.tag=Cyc_CfFlowInfo_Aggregate;
_tmp350.f1=aggrdict;_tmp350;});_tmp34F;});_tmp34E;});}}_LL360: _tmp1EA=_tmp1E8;
goto _LL362;_LL362: {struct Cyc_List_List*_tmp353;void*_tmp355;struct _tuple4
_tmp351=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
struct Cyc_Absyn_Exp*(*)(struct _tuple5*))Cyc_Core_snd,_tmp1EA));_LL854: _tmp355=
_tmp351.f1;goto _LL852;_LL852: _tmp353=_tmp351.f2;goto _LL850;_LL850: {struct Cyc_Dict_Dict*
aggrdict=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);{int i=0;for(0;_tmp353 != 0;(((
_tmp353=_tmp353->tl,_tmp1EA=_tmp1EA->tl)),++ i)){struct Cyc_List_List*ds=(*((
struct _tuple5*)((struct Cyc_List_List*)_check_null(_tmp1EA))->hd)).f1;for(0;ds != 
0;ds=ds->tl){void*_tmp357=(void*)ds->hd;struct _tagged_arr*_tmp35D;_LL857: if(*((
int*)_tmp357)== Cyc_Absyn_ArrayElement){goto _LL858;}else{goto _LL859;}_LL859: if(*((
int*)_tmp357)== Cyc_Absyn_FieldName){_LL862: _tmp35D=((struct Cyc_Absyn_FieldName_struct*)
_tmp357)->f1;goto _LL860;}else{goto _LL856;}_LL858:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp35F=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp35F[0]=({struct
Cyc_Core_Impossible_struct _tmp360;_tmp360.tag=Cyc_Core_Impossible;_tmp360.f1=
_tag_arr("anal_Rexp:Struct_e",sizeof(unsigned char),19);_tmp360;});_tmp35F;}));
_LL860: aggrdict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,void*v))Cyc_Dict_insert)(aggrdict,_tmp35D,(void*)_tmp353->hd);
_LL856:;}}}return({struct _tuple0 _tmp361;_tmp361.f1=_tmp355;_tmp361.f2=(void*)({
struct Cyc_CfFlowInfo_Aggregate_struct*_tmp362=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_struct));
_tmp362[0]=({struct Cyc_CfFlowInfo_Aggregate_struct _tmp363;_tmp363.tag=Cyc_CfFlowInfo_Aggregate;
_tmp363.f1=aggrdict;_tmp363;});_tmp362;});_tmp361;});}}_LL364: {struct Cyc_List_List*
_tmp364=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple5*))
Cyc_Core_snd,_tmp1EC);struct Cyc_List_List*_tmp367;void*_tmp369;struct _tuple4
_tmp365=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp364);_LL874:
_tmp369=_tmp365.f1;goto _LL872;_LL872: _tmp367=_tmp365.f2;goto _LL870;_LL870: for(0;
_tmp367 != 0;(_tmp367=_tmp367->tl,_tmp364=_tmp364->tl)){_tmp369=Cyc_NewControlFlow_use_Rval(
env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp364))->hd)->loc,
_tmp369,(void*)_tmp367->hd);}return({struct _tuple0 _tmp36B;_tmp36B.f1=_tmp369;
_tmp36B.f2=Cyc_CfFlowInfo_typ_to_absrval((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_CfFlowInfo_unknown_all);_tmp36B;});}_LL366: {void*
_tmp36E;void*_tmp370;struct _tuple0 _tmp36C=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp1F0);_LL881: _tmp370=_tmp36C.f1;goto _LL879;_LL879: _tmp36E=_tmp36C.f2;
goto _LL877;_LL877: {void*_tmp372=_tmp370;struct Cyc_List_List*_tmp378;struct Cyc_Dict_Dict*
_tmp37A;_LL884: if(_tmp372 == (void*)Cyc_CfFlowInfo_BottomFL){goto _LL885;}else{
goto _LL886;}_LL886: if((unsigned int)_tmp372 > 1?*((int*)_tmp372)== Cyc_CfFlowInfo_ReachableFL:
0){_LL891: _tmp37A=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp372)->f1;goto
_LL889;_LL889: _tmp378=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp372)->f2;
goto _LL887;}else{goto _LL883;}_LL885: return({struct _tuple0 _tmp37C;_tmp37C.f1=
_tmp370;_tmp37C.f2=Cyc_CfFlowInfo_unknown_all;_tmp37C;});_LL887: if(Cyc_CfFlowInfo_initlevel(
_tmp37A,_tmp36E)== (void*)Cyc_CfFlowInfo_NoneIL){({void*_tmp37D[0]={};Cyc_Tcutil_terr(
_tmp1F0->loc,_tag_arr("expression may not be initialized",sizeof(unsigned char),
34),_tag_arr(_tmp37D,sizeof(void*),0));});}{struct Cyc_List_List*new_relns=
_tmp378;comp_loop: {void*_tmp37E=(void*)_tmp1F0->r;struct Cyc_Absyn_Exp*_tmp390;
void*_tmp393;struct Cyc_Absyn_Vardecl*_tmp395;void*_tmp398;struct Cyc_Absyn_Vardecl*
_tmp39A;void*_tmp39D;struct Cyc_Absyn_Vardecl*_tmp39F;void*_tmp3A2;struct Cyc_Absyn_Vardecl*
_tmp3A4;void*_tmp3A6;int _tmp3A8;struct Cyc_List_List*_tmp3AA;struct Cyc_List_List
_tmp3AC;struct Cyc_Absyn_Exp*_tmp3AD;void*_tmp3AF;_LL896: if(*((int*)_tmp37E)== 
Cyc_Absyn_Cast_e){_LL913: _tmp390=((struct Cyc_Absyn_Cast_e_struct*)_tmp37E)->f2;
goto _LL897;}else{goto _LL898;}_LL898: if(*((int*)_tmp37E)== Cyc_Absyn_Var_e){
_LL916: _tmp393=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp37E)->f2;if((
unsigned int)_tmp393 > 1?*((int*)_tmp393)== Cyc_Absyn_Global_b: 0){_LL918: _tmp395=((
struct Cyc_Absyn_Global_b_struct*)_tmp393)->f1;goto _LL914;}else{goto _LL900;}}
else{goto _LL900;}_LL914: if(! _tmp395->escapes){goto _LL899;}else{goto _LL900;}
_LL900: if(*((int*)_tmp37E)== Cyc_Absyn_Var_e){_LL921: _tmp398=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp37E)->f2;if((unsigned int)_tmp398 > 1?*((int*)_tmp398)== Cyc_Absyn_Local_b: 0){
_LL923: _tmp39A=((struct Cyc_Absyn_Local_b_struct*)_tmp398)->f1;goto _LL919;}else{
goto _LL902;}}else{goto _LL902;}_LL919: if(! _tmp39A->escapes){goto _LL901;}else{goto
_LL902;}_LL902: if(*((int*)_tmp37E)== Cyc_Absyn_Var_e){_LL926: _tmp39D=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp37E)->f2;if((unsigned int)_tmp39D > 1?*((int*)
_tmp39D)== Cyc_Absyn_Pat_b: 0){_LL928: _tmp39F=((struct Cyc_Absyn_Pat_b_struct*)
_tmp39D)->f1;goto _LL924;}else{goto _LL904;}}else{goto _LL904;}_LL924: if(! _tmp39F->escapes){
goto _LL903;}else{goto _LL904;}_LL904: if(*((int*)_tmp37E)== Cyc_Absyn_Var_e){
_LL931: _tmp3A2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp37E)->f2;if((
unsigned int)_tmp3A2 > 1?*((int*)_tmp3A2)== Cyc_Absyn_Param_b: 0){_LL933: _tmp3A4=((
struct Cyc_Absyn_Param_b_struct*)_tmp3A2)->f1;goto _LL929;}else{goto _LL906;}}else{
goto _LL906;}_LL929: if(! _tmp3A4->escapes){goto _LL905;}else{goto _LL906;}_LL906: if(*((
int*)_tmp37E)== Cyc_Absyn_Const_e){_LL935: _tmp3A6=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp37E)->f1;if((unsigned int)_tmp3A6 > 1?*((int*)_tmp3A6)== Cyc_Absyn_Int_c: 0){
_LL937: _tmp3A8=((struct Cyc_Absyn_Int_c_struct*)_tmp3A6)->f2;goto _LL907;}else{
goto _LL908;}}else{goto _LL908;}_LL908: if(*((int*)_tmp37E)== Cyc_Absyn_Primop_e){
_LL944: _tmp3AF=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp37E)->f1;if(_tmp3AF
== (void*)Cyc_Absyn_Size){goto _LL939;}else{goto _LL910;}_LL939: _tmp3AA=((struct
Cyc_Absyn_Primop_e_struct*)_tmp37E)->f2;if(_tmp3AA == 0){goto _LL910;}else{_tmp3AC=*
_tmp3AA;_LL942: _tmp3AD=(struct Cyc_Absyn_Exp*)_tmp3AC.hd;goto _LL909;}}else{goto
_LL910;}_LL910: goto _LL911;_LL897: _tmp1F0=_tmp390;goto comp_loop;_LL899: _tmp39A=
_tmp395;goto _LL901;_LL901: _tmp39F=_tmp39A;goto _LL903;_LL903: _tmp3A4=_tmp39F;goto
_LL905;_LL905: new_relns=({struct Cyc_List_List*_tmp3B1=_cycalloc(sizeof(struct Cyc_List_List));
_tmp3B1->hd=({struct Cyc_CfFlowInfo_Reln*_tmp3B2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp3B2->vd=_tmp1F2;_tmp3B2->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*
_tmp3B3=_cycalloc(sizeof(struct Cyc_CfFlowInfo_LessVar_struct));_tmp3B3[0]=({
struct Cyc_CfFlowInfo_LessVar_struct _tmp3B4;_tmp3B4.tag=Cyc_CfFlowInfo_LessVar;
_tmp3B4.f1=_tmp3A4;_tmp3B4;});_tmp3B3;}));_tmp3B2;});_tmp3B1->tl=_tmp378;_tmp3B1;});
goto _LL895;_LL907: new_relns=({struct Cyc_List_List*_tmp3B5=_cycalloc(sizeof(
struct Cyc_List_List));_tmp3B5->hd=({struct Cyc_CfFlowInfo_Reln*_tmp3B6=_cycalloc(
sizeof(struct Cyc_CfFlowInfo_Reln));_tmp3B6->vd=_tmp1F2;_tmp3B6->rop=(void*)((
void*)({struct Cyc_CfFlowInfo_LessConst_struct*_tmp3B7=_cycalloc_atomic(sizeof(
struct Cyc_CfFlowInfo_LessConst_struct));_tmp3B7[0]=({struct Cyc_CfFlowInfo_LessConst_struct
_tmp3B8;_tmp3B8.tag=Cyc_CfFlowInfo_LessConst;_tmp3B8.f1=(unsigned int)_tmp3A8;
_tmp3B8;});_tmp3B7;}));_tmp3B6;});_tmp3B5->tl=_tmp378;_tmp3B5;});goto _LL895;
_LL909:{void*_tmp3B9=(void*)_tmp3AD->r;void*_tmp3C6;struct Cyc_Absyn_Vardecl*
_tmp3C8;void*_tmp3CB;struct Cyc_Absyn_Vardecl*_tmp3CD;void*_tmp3D0;struct Cyc_Absyn_Vardecl*
_tmp3D2;void*_tmp3D5;struct Cyc_Absyn_Vardecl*_tmp3D7;_LL955: if(*((int*)_tmp3B9)
== Cyc_Absyn_Var_e){_LL967: _tmp3C6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3B9)->f2;if((unsigned int)_tmp3C6 > 1?*((int*)_tmp3C6)== Cyc_Absyn_Global_b: 0){
_LL969: _tmp3C8=((struct Cyc_Absyn_Global_b_struct*)_tmp3C6)->f1;goto _LL965;}else{
goto _LL957;}}else{goto _LL957;}_LL965: if(! _tmp3C8->escapes){goto _LL956;}else{goto
_LL957;}_LL957: if(*((int*)_tmp3B9)== Cyc_Absyn_Var_e){_LL972: _tmp3CB=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp3B9)->f2;if((unsigned int)_tmp3CB > 1?*((int*)
_tmp3CB)== Cyc_Absyn_Local_b: 0){_LL974: _tmp3CD=((struct Cyc_Absyn_Local_b_struct*)
_tmp3CB)->f1;goto _LL970;}else{goto _LL959;}}else{goto _LL959;}_LL970: if(! _tmp3CD->escapes){
goto _LL958;}else{goto _LL959;}_LL959: if(*((int*)_tmp3B9)== Cyc_Absyn_Var_e){
_LL977: _tmp3D0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp3B9)->f2;if((
unsigned int)_tmp3D0 > 1?*((int*)_tmp3D0)== Cyc_Absyn_Pat_b: 0){_LL979: _tmp3D2=((
struct Cyc_Absyn_Pat_b_struct*)_tmp3D0)->f1;goto _LL975;}else{goto _LL961;}}else{
goto _LL961;}_LL975: if(! _tmp3D2->escapes){goto _LL960;}else{goto _LL961;}_LL961: if(*((
int*)_tmp3B9)== Cyc_Absyn_Var_e){_LL982: _tmp3D5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3B9)->f2;if((unsigned int)_tmp3D5 > 1?*((int*)_tmp3D5)== Cyc_Absyn_Param_b: 0){
_LL984: _tmp3D7=((struct Cyc_Absyn_Param_b_struct*)_tmp3D5)->f1;goto _LL980;}else{
goto _LL963;}}else{goto _LL963;}_LL980: if(! _tmp3D7->escapes){goto _LL962;}else{goto
_LL963;}_LL963: goto _LL964;_LL956: _tmp3CD=_tmp3C8;goto _LL958;_LL958: _tmp3D2=
_tmp3CD;goto _LL960;_LL960: _tmp3D7=_tmp3D2;goto _LL962;_LL962: new_relns=({struct
Cyc_List_List*_tmp3D9=_cycalloc(sizeof(struct Cyc_List_List));_tmp3D9->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp3DA=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp3DA->vd=_tmp1F2;_tmp3DA->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*
_tmp3DB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_LessSize_struct));_tmp3DB[0]=({
struct Cyc_CfFlowInfo_LessSize_struct _tmp3DC;_tmp3DC.tag=Cyc_CfFlowInfo_LessSize;
_tmp3DC.f1=_tmp3D7;_tmp3DC;});_tmp3DB;}));_tmp3DA;});_tmp3D9->tl=_tmp378;_tmp3D9;});
goto _LL954;_LL964: goto _LL954;_LL954:;}goto _LL895;_LL911: goto _LL895;_LL895:;}if(
_tmp378 != new_relns){_tmp370=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp3DD=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp3DD[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp3DE;_tmp3DE.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp3DE.f1=_tmp37A;_tmp3DE.f2=new_relns;_tmp3DE;});_tmp3DD;});}{void*_tmp3DF=
_tmp36E;_LL993: if(_tmp3DF == (void*)Cyc_CfFlowInfo_Zero){goto _LL994;}else{goto
_LL995;}_LL995: if(_tmp3DF == (void*)Cyc_CfFlowInfo_NotZeroThis){goto _LL996;}else{
goto _LL997;}_LL997: if(_tmp3DF == (void*)Cyc_CfFlowInfo_NotZeroAll){goto _LL998;}
else{goto _LL999;}_LL999: if((unsigned int)_tmp3DF > 3?*((int*)_tmp3DF)== Cyc_CfFlowInfo_AddressOf:
0){goto _LL1000;}else{goto _LL1001;}_LL1001: goto _LL1002;_LL994: return({struct
_tuple0 _tmp3EB;_tmp3EB.f1=_tmp370;_tmp3EB.f2=Cyc_CfFlowInfo_unknown_all;_tmp3EB;});
_LL996: goto _LL998;_LL998: goto _LL1000;_LL1000: {struct Cyc_List_List _tmp3EC=({
struct Cyc_List_List _tmp3FE;_tmp3FE.hd=_tmp1F2;_tmp3FE.tl=0;_tmp3FE;});_tmp370=
Cyc_NewControlFlow_add_vars(_tmp370,(struct Cyc_List_List*)& _tmp3EC,Cyc_CfFlowInfo_unknown_all);{
void*_tmp3EF;void*_tmp3F1;struct _tuple0 _tmp3ED=Cyc_NewControlFlow_anal_Rexp(env,
_tmp370,_tmp1EE);_LL1010: _tmp3F1=_tmp3ED.f1;goto _LL1008;_LL1008: _tmp3EF=_tmp3ED.f2;
goto _LL1006;_LL1006:{void*_tmp3F3=_tmp3F1;struct Cyc_Dict_Dict*_tmp3F9;_LL1013:
if(_tmp3F3 == (void*)Cyc_CfFlowInfo_BottomFL){goto _LL1014;}else{goto _LL1015;}
_LL1015: if((unsigned int)_tmp3F3 > 1?*((int*)_tmp3F3)== Cyc_CfFlowInfo_ReachableFL:
0){_LL1018: _tmp3F9=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp3F3)->f1;goto
_LL1016;}else{goto _LL1012;}_LL1014: return({struct _tuple0 _tmp3FB;_tmp3FB.f1=
_tmp3F1;_tmp3FB.f2=Cyc_CfFlowInfo_unknown_all;_tmp3FB;});_LL1016: if(Cyc_CfFlowInfo_initlevel(
_tmp3F9,_tmp3EF)!= (void*)Cyc_CfFlowInfo_AllIL){({void*_tmp3FC[0]={};Cyc_Tcutil_terr(
_tmp1F0->loc,_tag_arr("expression may not be initialized",sizeof(unsigned char),
34),_tag_arr(_tmp3FC,sizeof(void*),0));});return({struct _tuple0 _tmp3FD;_tmp3FD.f1=(
void*)Cyc_CfFlowInfo_BottomFL;_tmp3FD.f2=Cyc_CfFlowInfo_unknown_all;_tmp3FD;});}
_LL1012:;}_tmp370=_tmp3F1;goto _LL1002;}}_LL1002: while(1){struct Cyc_List_List
_tmp3FF=({struct Cyc_List_List _tmp411;_tmp411.hd=_tmp1F2;_tmp411.tl=0;_tmp411;});
_tmp370=Cyc_NewControlFlow_add_vars(_tmp370,(struct Cyc_List_List*)& _tmp3FF,Cyc_CfFlowInfo_unknown_all);{
void*_tmp402;void*_tmp404;struct _tuple0 _tmp400=Cyc_NewControlFlow_anal_Rexp(env,
_tmp370,_tmp1EE);_LL1029: _tmp404=_tmp400.f1;goto _LL1027;_LL1027: _tmp402=_tmp400.f2;
goto _LL1025;_LL1025:{void*_tmp406=_tmp404;struct Cyc_Dict_Dict*_tmp40C;_LL1032:
if(_tmp406 == (void*)Cyc_CfFlowInfo_BottomFL){goto _LL1033;}else{goto _LL1034;}
_LL1034: if((unsigned int)_tmp406 > 1?*((int*)_tmp406)== Cyc_CfFlowInfo_ReachableFL:
0){_LL1037: _tmp40C=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp406)->f1;goto
_LL1035;}else{goto _LL1031;}_LL1033: goto _LL1031;_LL1035: if(Cyc_CfFlowInfo_initlevel(
_tmp40C,_tmp402)!= (void*)Cyc_CfFlowInfo_AllIL){({void*_tmp40E[0]={};Cyc_Tcutil_terr(
_tmp1F0->loc,_tag_arr("expression may not be initialized",sizeof(unsigned char),
34),_tag_arr(_tmp40E,sizeof(void*),0));});return({struct _tuple0 _tmp40F;_tmp40F.f1=(
void*)Cyc_CfFlowInfo_BottomFL;_tmp40F.f2=Cyc_CfFlowInfo_unknown_all;_tmp40F;});}
_LL1031:;}{void*_tmp410=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp370,
_tmp404);if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp410,_tmp370)){break;}_tmp370=
_tmp410;}}}return({struct _tuple0 _tmp412;_tmp412.f1=_tmp370;_tmp412.f2=Cyc_CfFlowInfo_unknown_all;
_tmp412;});_LL992:;}}_LL883:;}}_LL368: while(1){inflow=(void*)(Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp1F4))->flow;{void*_tmp413=(void*)_tmp1F4->r;struct Cyc_Absyn_Stmt*
_tmp41D;struct Cyc_Absyn_Stmt*_tmp41F;struct Cyc_Absyn_Stmt*_tmp421;struct Cyc_Absyn_Decl*
_tmp423;struct Cyc_Absyn_Exp*_tmp425;_LL1045: if((unsigned int)_tmp413 > 1?*((int*)
_tmp413)== Cyc_Absyn_Seq_s: 0){_LL1056: _tmp41F=((struct Cyc_Absyn_Seq_s_struct*)
_tmp413)->f1;goto _LL1054;_LL1054: _tmp41D=((struct Cyc_Absyn_Seq_s_struct*)_tmp413)->f2;
goto _LL1046;}else{goto _LL1047;}_LL1047: if((unsigned int)_tmp413 > 1?*((int*)
_tmp413)== Cyc_Absyn_Decl_s: 0){_LL1060: _tmp423=((struct Cyc_Absyn_Decl_s_struct*)
_tmp413)->f1;goto _LL1058;_LL1058: _tmp421=((struct Cyc_Absyn_Decl_s_struct*)
_tmp413)->f2;goto _LL1048;}else{goto _LL1049;}_LL1049: if((unsigned int)_tmp413 > 1?*((
int*)_tmp413)== Cyc_Absyn_Exp_s: 0){_LL1062: _tmp425=((struct Cyc_Absyn_Exp_s_struct*)
_tmp413)->f1;goto _LL1050;}else{goto _LL1051;}_LL1051: goto _LL1052;_LL1046: inflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp41F);_tmp1F4=_tmp41D;goto _LL1044;
_LL1048: inflow=Cyc_NewControlFlow_anal_decl(env,inflow,_tmp423);_tmp1F4=_tmp421;
goto _LL1044;_LL1050: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp425);
_LL1052:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp427=_cycalloc(
sizeof(struct Cyc_Core_Impossible_struct));_tmp427[0]=({struct Cyc_Core_Impossible_struct
_tmp428;_tmp428.tag=Cyc_Core_Impossible;_tmp428.f1=_tag_arr("analyze_Rexp: ill-formed StmtExp",
sizeof(unsigned char),33);_tmp428;});_tmp427;}));_LL1044:;}}_LL370: goto _LL372;
_LL372: goto _LL374;_LL374: goto _LL376;_LL376: goto _LL378;_LL378: goto _LL380;_LL380:
goto _LL382;_LL382:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp429=
_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp429[0]=({struct Cyc_Core_Impossible_struct
_tmp42A;_tmp42A.tag=Cyc_Core_Impossible;_tmp42A.f1=_tag_arr("anal_Rexp, unexpected exp form",
sizeof(unsigned char),31);_tmp42A;});_tmp429;}));_LL286:;}}static struct _tuple0
Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,
void*f,struct Cyc_Absyn_Exp*e,void*r,struct Cyc_List_List*flds){void*_tmp42B=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp431;struct Cyc_Absyn_Conref*_tmp433;void*_tmp435;_LL1069: if((unsigned int)
_tmp42B > 3?*((int*)_tmp42B)== Cyc_Absyn_PointerType: 0){_LL1074: _tmp431=((struct
Cyc_Absyn_PointerType_struct*)_tmp42B)->f1;_LL1078: _tmp435=(void*)_tmp431.elt_typ;
goto _LL1076;_LL1076: _tmp433=_tmp431.bounds;goto _LL1070;}else{goto _LL1071;}
_LL1071: goto _LL1072;_LL1070: {void*_tmp437=f;struct Cyc_List_List*_tmp43D;struct
Cyc_Dict_Dict*_tmp43F;_LL1081: if(_tmp437 == (void*)Cyc_CfFlowInfo_BottomFL){goto
_LL1082;}else{goto _LL1083;}_LL1083: if((unsigned int)_tmp437 > 1?*((int*)_tmp437)
== Cyc_CfFlowInfo_ReachableFL: 0){_LL1088: _tmp43F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp437)->f1;goto _LL1086;_LL1086: _tmp43D=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp437)->f2;goto _LL1084;}else{goto _LL1080;}_LL1082: return({struct _tuple0 _tmp441;
_tmp441.f1=f;_tmp441.f2=(void*)Cyc_CfFlowInfo_UnknownL;_tmp441;});_LL1084: if(Cyc_Tcutil_is_bound_one(
_tmp433)){void*_tmp442=r;struct Cyc_CfFlowInfo_Place*_tmp450;struct Cyc_CfFlowInfo_Place
_tmp452;struct Cyc_List_List*_tmp453;void*_tmp455;void*_tmp457;_LL1092: if(_tmp442
== (void*)Cyc_CfFlowInfo_NotZeroAll){goto _LL1093;}else{goto _LL1094;}_LL1094: if(
_tmp442 == (void*)Cyc_CfFlowInfo_NotZeroThis){goto _LL1095;}else{goto _LL1096;}
_LL1096: if((unsigned int)_tmp442 > 3?*((int*)_tmp442)== Cyc_CfFlowInfo_AddressOf:
0){_LL1105: _tmp450=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp442)->f1;_tmp452=*
_tmp450;_LL1110: _tmp455=(void*)_tmp452.root;goto _LL1108;_LL1108: _tmp453=_tmp452.fields;
goto _LL1097;}else{goto _LL1098;}_LL1098: if(_tmp442 == (void*)Cyc_CfFlowInfo_Zero){
goto _LL1099;}else{goto _LL1100;}_LL1100: if((unsigned int)_tmp442 > 3?*((int*)
_tmp442)== Cyc_CfFlowInfo_UnknownR: 0){_LL1112: _tmp457=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp442)->f1;goto _LL1101;}else{goto _LL1102;}_LL1102: goto _LL1103;_LL1093: goto
_LL1095;_LL1095:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp459=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NotZero_struct));_tmp459[0]=({
struct Cyc_CfFlowInfo_NotZero_struct _tmp45A;_tmp45A.tag=Cyc_CfFlowInfo_NotZero;
_tmp45A.f1=_tmp43D;_tmp45A;});_tmp459;})));goto _LL1091;_LL1097:(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*_tmp45B=_cycalloc(sizeof(
struct Cyc_CfFlowInfo_NotZero_struct));_tmp45B[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp45C;_tmp45C.tag=Cyc_CfFlowInfo_NotZero;_tmp45C.f1=_tmp43D;_tmp45C;});_tmp45B;})));
return({struct _tuple0 _tmp45D;_tmp45D.f1=f;_tmp45D.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*
_tmp45E=_cycalloc(sizeof(struct Cyc_CfFlowInfo_PlaceL_struct));_tmp45E[0]=({
struct Cyc_CfFlowInfo_PlaceL_struct _tmp45F;_tmp45F.tag=Cyc_CfFlowInfo_PlaceL;
_tmp45F.f1=({struct Cyc_CfFlowInfo_Place*_tmp460=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));
_tmp460->root=(void*)_tmp455;_tmp460->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp453,flds);_tmp460;});_tmp45F;});
_tmp45E;});_tmp45D;});_LL1099:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_IsZero_struct*
_tmp461=_cycalloc(sizeof(struct Cyc_CfFlowInfo_IsZero_struct));_tmp461[0]=({
struct Cyc_CfFlowInfo_IsZero_struct _tmp462;_tmp462.tag=Cyc_CfFlowInfo_IsZero;
_tmp462.f1=_tmp43D;_tmp462;});_tmp461;})));return({struct _tuple0 _tmp463;_tmp463.f1=(
void*)Cyc_CfFlowInfo_BottomFL;_tmp463.f2=(void*)Cyc_CfFlowInfo_UnknownL;_tmp463;});
_LL1101: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp457);goto _LL1103;_LL1103:(
void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp464=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownZ_struct));_tmp464[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp465;_tmp465.tag=Cyc_CfFlowInfo_UnknownZ;_tmp465.f1=_tmp43D;_tmp465;});
_tmp464;})));_LL1091:;}else{(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp466=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownZ_struct));_tmp466[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmp467;_tmp467.tag=Cyc_CfFlowInfo_UnknownZ;
_tmp467.f1=_tmp43D;_tmp467;});_tmp466;})));}if(Cyc_CfFlowInfo_initlevel(_tmp43F,
r)== (void*)Cyc_CfFlowInfo_NoneIL){({void*_tmp468[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("dereference of possibly uninitialized pointer",sizeof(unsigned char),
46),_tag_arr(_tmp468,sizeof(void*),0));});}return({struct _tuple0 _tmp469;_tmp469.f1=
f;_tmp469.f2=(void*)Cyc_CfFlowInfo_UnknownL;_tmp469;});_LL1080:;}_LL1072:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp46A=_cycalloc(sizeof(struct
Cyc_Core_Impossible_struct));_tmp46A[0]=({struct Cyc_Core_Impossible_struct
_tmp46B;_tmp46B.tag=Cyc_Core_Impossible;_tmp46B.f1=_tag_arr("left deref of non-pointer-type",
sizeof(unsigned char),31);_tmp46B;});_tmp46A;}));_LL1068:;}static struct _tuple0
Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,void*
inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){struct Cyc_Dict_Dict*d;{
void*_tmp46C=inflow;struct Cyc_List_List*_tmp472;struct Cyc_Dict_Dict*_tmp474;
_LL1134: if(_tmp46C == (void*)Cyc_CfFlowInfo_BottomFL){goto _LL1135;}else{goto
_LL1136;}_LL1136: if((unsigned int)_tmp46C > 1?*((int*)_tmp46C)== Cyc_CfFlowInfo_ReachableFL:
0){_LL1141: _tmp474=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp46C)->f1;goto
_LL1139;_LL1139: _tmp472=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp46C)->f2;
goto _LL1137;}else{goto _LL1133;}_LL1135: return({struct _tuple0 _tmp476;_tmp476.f1=(
void*)Cyc_CfFlowInfo_BottomFL;_tmp476.f2=(void*)Cyc_CfFlowInfo_UnknownL;_tmp476;});
_LL1137: d=_tmp474;_LL1133:;}{void*_tmp477=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp491;struct Cyc_Absyn_Exp*_tmp493;struct Cyc_Absyn_Exp*_tmp495;void*_tmp497;
struct Cyc_Absyn_Vardecl*_tmp499;void*_tmp49B;struct Cyc_Absyn_Vardecl*_tmp49D;
void*_tmp49F;struct Cyc_Absyn_Vardecl*_tmp4A1;void*_tmp4A3;struct Cyc_Absyn_Vardecl*
_tmp4A5;struct _tagged_arr*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A9;struct Cyc_Absyn_Exp*
_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp*_tmp4AF;struct
_tagged_arr*_tmp4B1;struct Cyc_Absyn_Exp*_tmp4B3;_LL1145: if(*((int*)_tmp477)== 
Cyc_Absyn_Cast_e){_LL1170: _tmp491=((struct Cyc_Absyn_Cast_e_struct*)_tmp477)->f2;
goto _LL1146;}else{goto _LL1147;}_LL1147: if(*((int*)_tmp477)== Cyc_Absyn_NoInstantiate_e){
_LL1172: _tmp493=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp477)->f1;goto
_LL1148;}else{goto _LL1149;}_LL1149: if(*((int*)_tmp477)== Cyc_Absyn_Instantiate_e){
_LL1174: _tmp495=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp477)->f1;goto _LL1150;}
else{goto _LL1151;}_LL1151: if(*((int*)_tmp477)== Cyc_Absyn_Var_e){_LL1176: _tmp497=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp477)->f2;if((unsigned int)_tmp497 > 1?*((
int*)_tmp497)== Cyc_Absyn_Param_b: 0){_LL1178: _tmp499=((struct Cyc_Absyn_Param_b_struct*)
_tmp497)->f1;goto _LL1152;}else{goto _LL1153;}}else{goto _LL1153;}_LL1153: if(*((int*)
_tmp477)== Cyc_Absyn_Var_e){_LL1180: _tmp49B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp477)->f2;if((unsigned int)_tmp49B > 1?*((int*)_tmp49B)== Cyc_Absyn_Local_b: 0){
_LL1182: _tmp49D=((struct Cyc_Absyn_Local_b_struct*)_tmp49B)->f1;goto _LL1154;}
else{goto _LL1155;}}else{goto _LL1155;}_LL1155: if(*((int*)_tmp477)== Cyc_Absyn_Var_e){
_LL1184: _tmp49F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp477)->f2;if((
unsigned int)_tmp49F > 1?*((int*)_tmp49F)== Cyc_Absyn_Pat_b: 0){_LL1186: _tmp4A1=((
struct Cyc_Absyn_Pat_b_struct*)_tmp49F)->f1;goto _LL1156;}else{goto _LL1157;}}else{
goto _LL1157;}_LL1157: if(*((int*)_tmp477)== Cyc_Absyn_Var_e){_LL1188: _tmp4A3=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp477)->f2;if((unsigned int)_tmp4A3 > 1?*((
int*)_tmp4A3)== Cyc_Absyn_Global_b: 0){_LL1190: _tmp4A5=((struct Cyc_Absyn_Global_b_struct*)
_tmp4A3)->f1;goto _LL1158;}else{goto _LL1159;}}else{goto _LL1159;}_LL1159: if(*((int*)
_tmp477)== Cyc_Absyn_StructArrow_e){_LL1194: _tmp4A9=((struct Cyc_Absyn_StructArrow_e_struct*)
_tmp477)->f1;goto _LL1192;_LL1192: _tmp4A7=((struct Cyc_Absyn_StructArrow_e_struct*)
_tmp477)->f2;goto _LL1160;}else{goto _LL1161;}_LL1161: if(*((int*)_tmp477)== Cyc_Absyn_Deref_e){
_LL1196: _tmp4AB=((struct Cyc_Absyn_Deref_e_struct*)_tmp477)->f1;goto _LL1162;}
else{goto _LL1163;}_LL1163: if(*((int*)_tmp477)== Cyc_Absyn_Subscript_e){_LL1200:
_tmp4AF=((struct Cyc_Absyn_Subscript_e_struct*)_tmp477)->f1;goto _LL1198;_LL1198:
_tmp4AD=((struct Cyc_Absyn_Subscript_e_struct*)_tmp477)->f2;goto _LL1164;}else{
goto _LL1165;}_LL1165: if(*((int*)_tmp477)== Cyc_Absyn_StructMember_e){_LL1204:
_tmp4B3=((struct Cyc_Absyn_StructMember_e_struct*)_tmp477)->f1;goto _LL1202;
_LL1202: _tmp4B1=((struct Cyc_Absyn_StructMember_e_struct*)_tmp477)->f2;goto
_LL1166;}else{goto _LL1167;}_LL1167: goto _LL1168;_LL1146: _tmp493=_tmp491;goto
_LL1148;_LL1148: _tmp495=_tmp493;goto _LL1150;_LL1150: return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp495,flds);_LL1152: _tmp49D=_tmp499;goto _LL1154;_LL1154: _tmp4A1=
_tmp49D;goto _LL1156;_LL1156: return({struct _tuple0 _tmp4B5;_tmp4B5.f1=inflow;
_tmp4B5.f2=(void*)({struct Cyc_CfFlowInfo_PlaceL_struct*_tmp4B6=_cycalloc(sizeof(
struct Cyc_CfFlowInfo_PlaceL_struct));_tmp4B6[0]=({struct Cyc_CfFlowInfo_PlaceL_struct
_tmp4B7;_tmp4B7.tag=Cyc_CfFlowInfo_PlaceL;_tmp4B7.f1=({struct Cyc_CfFlowInfo_Place*
_tmp4B8=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_tmp4B8->root=(void*)((
void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp4B9=_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_struct));
_tmp4B9[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp4BA;_tmp4BA.tag=Cyc_CfFlowInfo_VarRoot;
_tmp4BA.f1=_tmp4A1;_tmp4BA;});_tmp4B9;}));_tmp4B8->fields=flds;_tmp4B8;});
_tmp4B7;});_tmp4B6;});_tmp4B5;});_LL1158: return({struct _tuple0 _tmp4BB;_tmp4BB.f1=
inflow;_tmp4BB.f2=(void*)Cyc_CfFlowInfo_UnknownL;_tmp4BB;});_LL1160:{void*
_tmp4BC=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4A9->topt))->v);
_LL1214: if((unsigned int)_tmp4BC > 3?*((int*)_tmp4BC)== Cyc_Absyn_AnonUnionType: 0){
goto _LL1215;}else{goto _LL1216;}_LL1216: if((unsigned int)_tmp4BC > 3?*((int*)
_tmp4BC)== Cyc_Absyn_UnionType: 0){goto _LL1217;}else{goto _LL1218;}_LL1218: goto
_LL1219;_LL1215: goto _LL1217;_LL1217: goto _LL1213;_LL1219: flds=({struct Cyc_List_List*
_tmp4C4=_cycalloc(sizeof(struct Cyc_List_List));_tmp4C4->hd=_tmp4A7;_tmp4C4->tl=
flds;_tmp4C4;});_LL1213:;}_tmp4AB=_tmp4A9;goto _LL1162;_LL1162: {void*_tmp4C7;
void*_tmp4C9;struct _tuple0 _tmp4C5=Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp4AB);_LL1226: _tmp4C9=_tmp4C5.f1;goto _LL1224;_LL1224: _tmp4C7=_tmp4C5.f2;goto
_LL1222;_LL1222: return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp4C9,_tmp4AB,
_tmp4C7,flds);}_LL1164:{void*_tmp4CB=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4AF->topt))->v);_LL1229: if((unsigned int)_tmp4CB > 3?*((int*)
_tmp4CB)== Cyc_Absyn_TupleType: 0){goto _LL1230;}else{goto _LL1231;}_LL1231: goto
_LL1232;_LL1230: {struct _tagged_arr*_tmp4D1=Cyc_Absyn_fieldname((int)Cyc_Evexp_eval_const_uint_exp(
_tmp4AD));return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp4AF,({struct Cyc_List_List*
_tmp4D2=_cycalloc(sizeof(struct Cyc_List_List));_tmp4D2->hd=_tmp4D1;_tmp4D2->tl=
flds;_tmp4D2;}));}_LL1232: goto _LL1228;_LL1228:;}{struct Cyc_List_List*_tmp4D6;
void*_tmp4D8;struct _tuple4 _tmp4D4=Cyc_NewControlFlow_anal_unordered_Rexps(env,
inflow,({struct Cyc_Absyn_Exp*_tmp4D3[2];_tmp4D3[1]=_tmp4AD;_tmp4D3[0]=_tmp4AF;((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp4D3,
sizeof(struct Cyc_Absyn_Exp*),2));}));_LL1241: _tmp4D8=_tmp4D4.f1;goto _LL1239;
_LL1239: _tmp4D6=_tmp4D4.f2;goto _LL1237;_LL1237:{void*_tmp4DA=_tmp4D8;struct Cyc_List_List*
_tmp4E0;struct Cyc_Dict_Dict*_tmp4E2;_LL1244: if((unsigned int)_tmp4DA > 1?*((int*)
_tmp4DA)== Cyc_CfFlowInfo_ReachableFL: 0){_LL1251: _tmp4E2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp4DA)->f1;goto _LL1249;_LL1249: _tmp4E0=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp4DA)->f2;goto _LL1245;}else{goto _LL1246;}_LL1246: goto _LL1247;_LL1245: if(Cyc_CfFlowInfo_initlevel(
_tmp4E2,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp4D6))->tl))->hd)== (void*)Cyc_CfFlowInfo_NoneIL){({void*_tmp4E4[
0]={};Cyc_Tcutil_terr(_tmp4AD->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp4E4,sizeof(void*),0));});}{struct Cyc_List_List*
_tmp4E5=Cyc_NewControlFlow_add_subscript_reln(_tmp4E0,_tmp4AF,_tmp4AD);if(
_tmp4E0 != _tmp4E5){_tmp4D8=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp4E6=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp4E6[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp4E7;_tmp4E7.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp4E7.f1=_tmp4E2;_tmp4E7.f2=_tmp4E5;_tmp4E7;});_tmp4E6;});}goto _LL1243;}
_LL1247: goto _LL1243;_LL1243:;}return Cyc_NewControlFlow_anal_derefL(env,inflow,
_tmp4D8,_tmp4AF,(void*)((struct Cyc_List_List*)_check_null(_tmp4D6))->hd,flds);}
_LL1166:{void*_tmp4E8=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4B3->topt))->v);_LL1258: if((unsigned int)_tmp4E8 > 3?*((int*)
_tmp4E8)== Cyc_Absyn_AnonUnionType: 0){goto _LL1259;}else{goto _LL1260;}_LL1260: if((
unsigned int)_tmp4E8 > 3?*((int*)_tmp4E8)== Cyc_Absyn_UnionType: 0){goto _LL1261;}
else{goto _LL1262;}_LL1262: goto _LL1263;_LL1259: goto _LL1261;_LL1261: return({struct
_tuple0 _tmp4F0;_tmp4F0.f1=inflow;_tmp4F0.f2=(void*)Cyc_CfFlowInfo_UnknownL;
_tmp4F0;});_LL1263: flds=({struct Cyc_List_List*_tmp4F1=_cycalloc(sizeof(struct Cyc_List_List));
_tmp4F1->hd=_tmp4B1;_tmp4F1->tl=flds;_tmp4F1;});_LL1257:;}return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp4B3,flds);_LL1168: return({struct _tuple0 _tmp4F2;_tmp4F2.f1=(void*)
Cyc_CfFlowInfo_BottomFL;_tmp4F2.f2=(void*)Cyc_CfFlowInfo_UnknownL;_tmp4F2;});
_LL1144:;}}static struct _tuple0 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);}static struct _tuple0 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_Absyn_Exp*e){{void*_tmp4F3=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp505;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Exp*_tmp509;struct Cyc_Absyn_Exp*
_tmp50B;struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_List_List*_tmp50F;struct Cyc_List_List
_tmp511;struct Cyc_List_List*_tmp512;struct Cyc_Absyn_Exp*_tmp514;void*_tmp516;
struct Cyc_List_List*_tmp518;void*_tmp51A;struct Cyc_List_List*_tmp51C;void*
_tmp51E;struct Cyc_List_List*_tmp520;void*_tmp522;struct Cyc_List_List*_tmp524;
void*_tmp526;_LL1269: if(*((int*)_tmp4F3)== Cyc_Absyn_Conditional_e){_LL1290:
_tmp509=((struct Cyc_Absyn_Conditional_e_struct*)_tmp4F3)->f1;goto _LL1288;_LL1288:
_tmp507=((struct Cyc_Absyn_Conditional_e_struct*)_tmp4F3)->f2;goto _LL1286;_LL1286:
_tmp505=((struct Cyc_Absyn_Conditional_e_struct*)_tmp4F3)->f3;goto _LL1270;}else{
goto _LL1271;}_LL1271: if(*((int*)_tmp4F3)== Cyc_Absyn_SeqExp_e){_LL1294: _tmp50D=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp4F3)->f1;goto _LL1292;_LL1292: _tmp50B=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp4F3)->f2;goto _LL1272;}else{goto _LL1273;}
_LL1273: if(*((int*)_tmp4F3)== Cyc_Absyn_Primop_e){_LL1303: _tmp516=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp4F3)->f1;if(_tmp516 == (void*)Cyc_Absyn_Not){
goto _LL1296;}else{goto _LL1275;}_LL1296: _tmp50F=((struct Cyc_Absyn_Primop_e_struct*)
_tmp4F3)->f2;if(_tmp50F == 0){goto _LL1275;}else{_tmp511=*_tmp50F;_LL1301: _tmp514=(
struct Cyc_Absyn_Exp*)_tmp511.hd;goto _LL1299;_LL1299: _tmp512=_tmp511.tl;if(
_tmp512 == 0){goto _LL1274;}else{goto _LL1275;}}}else{goto _LL1275;}_LL1275: if(*((
int*)_tmp4F3)== Cyc_Absyn_Primop_e){_LL1307: _tmp51A=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp4F3)->f1;if(_tmp51A == (void*)Cyc_Absyn_Eq){goto _LL1305;}else{goto _LL1277;}
_LL1305: _tmp518=((struct Cyc_Absyn_Primop_e_struct*)_tmp4F3)->f2;goto _LL1276;}
else{goto _LL1277;}_LL1277: if(*((int*)_tmp4F3)== Cyc_Absyn_Primop_e){_LL1311:
_tmp51E=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp4F3)->f1;if(_tmp51E == (
void*)Cyc_Absyn_Neq){goto _LL1309;}else{goto _LL1279;}_LL1309: _tmp51C=((struct Cyc_Absyn_Primop_e_struct*)
_tmp4F3)->f2;goto _LL1278;}else{goto _LL1279;}_LL1279: if(*((int*)_tmp4F3)== Cyc_Absyn_Primop_e){
_LL1315: _tmp522=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp4F3)->f1;if(
_tmp522 == (void*)Cyc_Absyn_Lt){goto _LL1313;}else{goto _LL1281;}_LL1313: _tmp520=((
struct Cyc_Absyn_Primop_e_struct*)_tmp4F3)->f2;goto _LL1280;}else{goto _LL1281;}
_LL1281: if(*((int*)_tmp4F3)== Cyc_Absyn_Primop_e){_LL1319: _tmp526=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp4F3)->f1;if(_tmp526 == (void*)Cyc_Absyn_Lte){
goto _LL1317;}else{goto _LL1283;}_LL1317: _tmp524=((struct Cyc_Absyn_Primop_e_struct*)
_tmp4F3)->f2;goto _LL1282;}else{goto _LL1283;}_LL1283: goto _LL1284;_LL1270: {void*
_tmp52A;void*_tmp52C;struct _tuple0 _tmp528=Cyc_NewControlFlow_anal_test(env,
inflow,_tmp509);_LL1325: _tmp52C=_tmp528.f1;goto _LL1323;_LL1323: _tmp52A=_tmp528.f2;
goto _LL1321;_LL1321: {void*_tmp530;void*_tmp532;struct _tuple0 _tmp52E=Cyc_NewControlFlow_anal_test(
env,_tmp52C,_tmp507);_LL1331: _tmp532=_tmp52E.f1;goto _LL1329;_LL1329: _tmp530=
_tmp52E.f2;goto _LL1327;_LL1327: {void*_tmp536;void*_tmp538;struct _tuple0 _tmp534=
Cyc_NewControlFlow_anal_test(env,_tmp52A,_tmp505);_LL1337: _tmp538=_tmp534.f1;
goto _LL1335;_LL1335: _tmp536=_tmp534.f2;goto _LL1333;_LL1333: return({struct _tuple0
_tmp53A;_tmp53A.f1=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp532,_tmp538);
_tmp53A.f2=Cyc_CfFlowInfo_join_flow(env->all_changed,_tmp530,_tmp536);_tmp53A;});}}}
_LL1272: return Cyc_NewControlFlow_anal_test(env,(Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp50D)).f1,_tmp50B);_LL1274: {void*_tmp53D;void*_tmp53F;struct _tuple0
_tmp53B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp514);_LL1344: _tmp53F=_tmp53B.f1;
goto _LL1342;_LL1342: _tmp53D=_tmp53B.f2;goto _LL1340;_LL1340: return({struct _tuple0
_tmp541;_tmp541.f1=_tmp53D;_tmp541.f2=_tmp53F;_tmp541;});}_LL1276: {struct Cyc_List_List*
_tmp544;void*_tmp546;struct _tuple4 _tmp542=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp518);_LL1351: _tmp546=_tmp542.f1;goto _LL1349;_LL1349: _tmp544=
_tmp542.f2;goto _LL1347;_LL1347: {void*_tmp548=_tmp546;struct Cyc_List_List*
_tmp54E;struct Cyc_Dict_Dict*_tmp550;_LL1354: if(_tmp548 == (void*)Cyc_CfFlowInfo_BottomFL){
goto _LL1355;}else{goto _LL1356;}_LL1356: if((unsigned int)_tmp548 > 1?*((int*)
_tmp548)== Cyc_CfFlowInfo_ReachableFL: 0){_LL1361: _tmp550=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp548)->f1;goto _LL1359;_LL1359: _tmp54E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp548)->f2;goto _LL1357;}else{goto _LL1353;}_LL1355: return({struct _tuple0 _tmp552;
_tmp552.f1=_tmp546;_tmp552.f2=_tmp546;_tmp552;});_LL1357: {void*_tmp553=(void*)((
struct Cyc_List_List*)_check_null(_tmp544))->hd;void*_tmp554=(void*)((struct Cyc_List_List*)
_check_null(_tmp544->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp550,_tmp553)== (
void*)Cyc_CfFlowInfo_NoneIL){({void*_tmp555[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp518))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp555,sizeof(void*),0));});}if(Cyc_CfFlowInfo_initlevel(
_tmp550,_tmp554)== (void*)Cyc_CfFlowInfo_NoneIL){({void*_tmp556[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp518))->tl))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp556,sizeof(void*),0));});}{struct _tuple0
_tmp558=({struct _tuple0 _tmp557;_tmp557.f1=_tmp553;_tmp557.f2=_tmp554;_tmp557;});
void*_tmp560;void*_tmp562;void*_tmp564;void*_tmp566;void*_tmp568;void*_tmp56A;
_LL1370: _LL1379: _tmp562=_tmp558.f1;if((unsigned int)_tmp562 > 3?*((int*)_tmp562)
== Cyc_CfFlowInfo_UnknownR: 0){_LL1381: _tmp564=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp562)->f1;goto _LL1377;}else{goto _LL1372;}_LL1377: _tmp560=_tmp558.f2;if(
_tmp560 == (void*)Cyc_CfFlowInfo_Zero){goto _LL1371;}else{goto _LL1372;}_LL1372:
_LL1387: _tmp56A=_tmp558.f1;if(_tmp56A == (void*)Cyc_CfFlowInfo_Zero){goto _LL1383;}
else{goto _LL1374;}_LL1383: _tmp566=_tmp558.f2;if((unsigned int)_tmp566 > 3?*((int*)
_tmp566)== Cyc_CfFlowInfo_UnknownR: 0){_LL1385: _tmp568=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp566)->f1;goto _LL1373;}else{goto _LL1374;}_LL1374: goto _LL1375;_LL1371: {void*
_tmp56E;void*_tmp570;struct _tuple0 _tmp56C=Cyc_NewControlFlow_splitzero(env,
_tmp546,_tmp546,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp518))->hd,
_tmp564);_LL1393: _tmp570=_tmp56C.f1;goto _LL1391;_LL1391: _tmp56E=_tmp56C.f2;goto
_LL1389;_LL1389: return({struct _tuple0 _tmp572;_tmp572.f1=_tmp56E;_tmp572.f2=
_tmp570;_tmp572;});}_LL1373: {void*_tmp575;void*_tmp577;struct _tuple0 _tmp573=Cyc_NewControlFlow_splitzero(
env,_tmp546,_tmp546,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((
struct Cyc_List_List*)_check_null(_tmp518))->tl))->hd,_tmp568);_LL1400: _tmp577=
_tmp573.f1;goto _LL1398;_LL1398: _tmp575=_tmp573.f2;goto _LL1396;_LL1396: return({
struct _tuple0 _tmp579;_tmp579.f1=_tmp575;_tmp579.f2=_tmp577;_tmp579;});}_LL1375:
return({struct _tuple0 _tmp57A;_tmp57A.f1=_tmp546;_tmp57A.f2=_tmp546;_tmp57A;});
_LL1369:;}}_LL1353:;}}_LL1278: {struct Cyc_List_List*_tmp57D;void*_tmp57F;struct
_tuple4 _tmp57B=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp51C);
_LL1408: _tmp57F=_tmp57B.f1;goto _LL1406;_LL1406: _tmp57D=_tmp57B.f2;goto _LL1404;
_LL1404: {void*_tmp581=_tmp57F;struct Cyc_List_List*_tmp587;struct Cyc_Dict_Dict*
_tmp589;_LL1411: if(_tmp581 == (void*)Cyc_CfFlowInfo_BottomFL){goto _LL1412;}else{
goto _LL1413;}_LL1413: if((unsigned int)_tmp581 > 1?*((int*)_tmp581)== Cyc_CfFlowInfo_ReachableFL:
0){_LL1418: _tmp589=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp581)->f1;goto
_LL1416;_LL1416: _tmp587=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp581)->f2;
goto _LL1414;}else{goto _LL1410;}_LL1412: return({struct _tuple0 _tmp58B;_tmp58B.f1=
_tmp57F;_tmp58B.f2=_tmp57F;_tmp58B;});_LL1414: {void*_tmp58C=(void*)((struct Cyc_List_List*)
_check_null(_tmp57D))->hd;void*_tmp58D=(void*)((struct Cyc_List_List*)_check_null(
_tmp57D->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp589,_tmp58C)== (void*)Cyc_CfFlowInfo_NoneIL){({
void*_tmp58E[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp51C))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp58E,sizeof(void*),0));});}if(Cyc_CfFlowInfo_initlevel(
_tmp589,_tmp58D)== (void*)Cyc_CfFlowInfo_NoneIL){({void*_tmp58F[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp51C))->tl))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp58F,sizeof(void*),0));});}{struct _tuple0
_tmp591=({struct _tuple0 _tmp590;_tmp590.f1=_tmp58C;_tmp590.f2=_tmp58D;_tmp590;});
void*_tmp599;void*_tmp59B;void*_tmp59D;void*_tmp59F;void*_tmp5A1;void*_tmp5A3;
_LL1427: _LL1436: _tmp59B=_tmp591.f1;if((unsigned int)_tmp59B > 3?*((int*)_tmp59B)
== Cyc_CfFlowInfo_UnknownR: 0){_LL1438: _tmp59D=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp59B)->f1;goto _LL1434;}else{goto _LL1429;}_LL1434: _tmp599=_tmp591.f2;if(
_tmp599 == (void*)Cyc_CfFlowInfo_Zero){goto _LL1428;}else{goto _LL1429;}_LL1429:
_LL1444: _tmp5A3=_tmp591.f1;if(_tmp5A3 == (void*)Cyc_CfFlowInfo_Zero){goto _LL1440;}
else{goto _LL1431;}_LL1440: _tmp59F=_tmp591.f2;if((unsigned int)_tmp59F > 3?*((int*)
_tmp59F)== Cyc_CfFlowInfo_UnknownR: 0){_LL1442: _tmp5A1=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp59F)->f1;goto _LL1430;}else{goto _LL1431;}_LL1431: goto _LL1432;_LL1428: return
Cyc_NewControlFlow_splitzero(env,_tmp57F,_tmp57F,(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp51C))->hd,_tmp59D);_LL1430: return Cyc_NewControlFlow_splitzero(
env,_tmp57F,_tmp57F,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((
struct Cyc_List_List*)_check_null(_tmp51C))->tl))->hd,_tmp5A1);_LL1432: return({
struct _tuple0 _tmp5A5;_tmp5A5.f1=_tmp57F;_tmp5A5.f2=_tmp57F;_tmp5A5;});_LL1426:;}}
_LL1410:;}}_LL1280: {struct Cyc_List_List*_tmp5A8;void*_tmp5AA;struct _tuple4
_tmp5A6=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,_tmp520);_LL1451:
_tmp5AA=_tmp5A6.f1;goto _LL1449;_LL1449: _tmp5A8=_tmp5A6.f2;goto _LL1447;_LL1447: {
void*_tmp5AC=_tmp5AA;struct Cyc_List_List*_tmp5B2;struct Cyc_Dict_Dict*_tmp5B4;
_LL1454: if(_tmp5AC == (void*)Cyc_CfFlowInfo_BottomFL){goto _LL1455;}else{goto
_LL1456;}_LL1456: if((unsigned int)_tmp5AC > 1?*((int*)_tmp5AC)== Cyc_CfFlowInfo_ReachableFL:
0){_LL1461: _tmp5B4=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp5AC)->f1;goto
_LL1459;_LL1459: _tmp5B2=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp5AC)->f2;
goto _LL1457;}else{goto _LL1453;}_LL1455: return({struct _tuple0 _tmp5B6;_tmp5B6.f1=
_tmp5AA;_tmp5B6.f2=_tmp5AA;_tmp5B6;});_LL1457: {void*_tmp5B7=(void*)((struct Cyc_List_List*)
_check_null(_tmp5A8))->hd;void*_tmp5B8=(void*)((struct Cyc_List_List*)_check_null(
_tmp5A8->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp5B4,_tmp5B7)== (void*)Cyc_CfFlowInfo_NoneIL){({
void*_tmp5B9[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp520))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp5B9,sizeof(void*),0));});}if(Cyc_CfFlowInfo_initlevel(
_tmp5B4,_tmp5B8)== (void*)Cyc_CfFlowInfo_NoneIL){({void*_tmp5BA[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp520))->tl))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp5BA,sizeof(void*),0));});}{struct Cyc_Absyn_Exp*
_tmp5BB=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp520))->hd;
struct Cyc_Absyn_Exp*_tmp5BC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp520->tl))->hd;void*_tmp5BD=(void*)_tmp5BB->r;void*_tmp5CA;struct
Cyc_Absyn_Vardecl*_tmp5CC;void*_tmp5CF;struct Cyc_Absyn_Vardecl*_tmp5D1;void*
_tmp5D4;struct Cyc_Absyn_Vardecl*_tmp5D6;void*_tmp5D9;struct Cyc_Absyn_Vardecl*
_tmp5DB;_LL1471: if(*((int*)_tmp5BD)== Cyc_Absyn_Var_e){_LL1483: _tmp5CA=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp5BD)->f2;if((unsigned int)_tmp5CA > 1?*((int*)
_tmp5CA)== Cyc_Absyn_Global_b: 0){_LL1485: _tmp5CC=((struct Cyc_Absyn_Global_b_struct*)
_tmp5CA)->f1;goto _LL1481;}else{goto _LL1473;}}else{goto _LL1473;}_LL1481: if(!
_tmp5CC->escapes){goto _LL1472;}else{goto _LL1473;}_LL1473: if(*((int*)_tmp5BD)== 
Cyc_Absyn_Var_e){_LL1488: _tmp5CF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp5BD)->f2;
if((unsigned int)_tmp5CF > 1?*((int*)_tmp5CF)== Cyc_Absyn_Local_b: 0){_LL1490:
_tmp5D1=((struct Cyc_Absyn_Local_b_struct*)_tmp5CF)->f1;goto _LL1486;}else{goto
_LL1475;}}else{goto _LL1475;}_LL1486: if(! _tmp5D1->escapes){goto _LL1474;}else{goto
_LL1475;}_LL1475: if(*((int*)_tmp5BD)== Cyc_Absyn_Var_e){_LL1493: _tmp5D4=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp5BD)->f2;if((unsigned int)_tmp5D4 > 1?*((int*)
_tmp5D4)== Cyc_Absyn_Pat_b: 0){_LL1495: _tmp5D6=((struct Cyc_Absyn_Pat_b_struct*)
_tmp5D4)->f1;goto _LL1491;}else{goto _LL1477;}}else{goto _LL1477;}_LL1491: if(!
_tmp5D6->escapes){goto _LL1476;}else{goto _LL1477;}_LL1477: if(*((int*)_tmp5BD)== 
Cyc_Absyn_Var_e){_LL1498: _tmp5D9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp5BD)->f2;
if((unsigned int)_tmp5D9 > 1?*((int*)_tmp5D9)== Cyc_Absyn_Param_b: 0){_LL1500:
_tmp5DB=((struct Cyc_Absyn_Param_b_struct*)_tmp5D9)->f1;goto _LL1496;}else{goto
_LL1479;}}else{goto _LL1479;}_LL1496: if(! _tmp5DB->escapes){goto _LL1478;}else{goto
_LL1479;}_LL1479: goto _LL1480;_LL1472: _tmp5D1=_tmp5CC;goto _LL1474;_LL1474: _tmp5D6=
_tmp5D1;goto _LL1476;_LL1476: _tmp5DB=_tmp5D6;goto _LL1478;_LL1478: {void*_tmp5DD=(
void*)_tmp5BC->r;void*_tmp5EE;struct Cyc_Absyn_Vardecl*_tmp5F0;void*_tmp5F3;
struct Cyc_Absyn_Vardecl*_tmp5F5;void*_tmp5F8;struct Cyc_Absyn_Vardecl*_tmp5FA;
void*_tmp5FD;struct Cyc_Absyn_Vardecl*_tmp5FF;void*_tmp601;int _tmp603;struct Cyc_List_List*
_tmp605;struct Cyc_List_List _tmp607;struct Cyc_Absyn_Exp*_tmp608;void*_tmp60A;
_LL1503: if(*((int*)_tmp5DD)== Cyc_Absyn_Var_e){_LL1519: _tmp5EE=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp5DD)->f2;if((unsigned int)_tmp5EE > 1?*((int*)_tmp5EE)
== Cyc_Absyn_Global_b: 0){_LL1521: _tmp5F0=((struct Cyc_Absyn_Global_b_struct*)
_tmp5EE)->f1;goto _LL1517;}else{goto _LL1505;}}else{goto _LL1505;}_LL1517: if(!
_tmp5F0->escapes){goto _LL1504;}else{goto _LL1505;}_LL1505: if(*((int*)_tmp5DD)== 
Cyc_Absyn_Var_e){_LL1524: _tmp5F3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp5DD)->f2;
if((unsigned int)_tmp5F3 > 1?*((int*)_tmp5F3)== Cyc_Absyn_Local_b: 0){_LL1526:
_tmp5F5=((struct Cyc_Absyn_Local_b_struct*)_tmp5F3)->f1;goto _LL1522;}else{goto
_LL1507;}}else{goto _LL1507;}_LL1522: if(! _tmp5F5->escapes){goto _LL1506;}else{goto
_LL1507;}_LL1507: if(*((int*)_tmp5DD)== Cyc_Absyn_Var_e){_LL1529: _tmp5F8=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp5DD)->f2;if((unsigned int)_tmp5F8 > 1?*((int*)
_tmp5F8)== Cyc_Absyn_Pat_b: 0){_LL1531: _tmp5FA=((struct Cyc_Absyn_Pat_b_struct*)
_tmp5F8)->f1;goto _LL1527;}else{goto _LL1509;}}else{goto _LL1509;}_LL1527: if(!
_tmp5FA->escapes){goto _LL1508;}else{goto _LL1509;}_LL1509: if(*((int*)_tmp5DD)== 
Cyc_Absyn_Var_e){_LL1534: _tmp5FD=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp5DD)->f2;
if((unsigned int)_tmp5FD > 1?*((int*)_tmp5FD)== Cyc_Absyn_Param_b: 0){_LL1536:
_tmp5FF=((struct Cyc_Absyn_Param_b_struct*)_tmp5FD)->f1;goto _LL1532;}else{goto
_LL1511;}}else{goto _LL1511;}_LL1532: if(! _tmp5FF->escapes){goto _LL1510;}else{goto
_LL1511;}_LL1511: if(*((int*)_tmp5DD)== Cyc_Absyn_Const_e){_LL1538: _tmp601=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp5DD)->f1;if((unsigned int)_tmp601 > 1?*((int*)
_tmp601)== Cyc_Absyn_Int_c: 0){_LL1540: _tmp603=((struct Cyc_Absyn_Int_c_struct*)
_tmp601)->f2;goto _LL1512;}else{goto _LL1513;}}else{goto _LL1513;}_LL1513: if(*((int*)
_tmp5DD)== Cyc_Absyn_Primop_e){_LL1547: _tmp60A=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp5DD)->f1;if(_tmp60A == (void*)Cyc_Absyn_Size){goto _LL1542;}else{goto _LL1515;}
_LL1542: _tmp605=((struct Cyc_Absyn_Primop_e_struct*)_tmp5DD)->f2;if(_tmp605 == 0){
goto _LL1515;}else{_tmp607=*_tmp605;_LL1545: _tmp608=(struct Cyc_Absyn_Exp*)_tmp607.hd;
goto _LL1514;}}else{goto _LL1515;}_LL1515: goto _LL1516;_LL1504: _tmp5F5=_tmp5F0;goto
_LL1506;_LL1506: _tmp5FA=_tmp5F5;goto _LL1508;_LL1508: _tmp5FF=_tmp5FA;goto _LL1510;
_LL1510:{struct _tuple0 _tmp60D=({struct _tuple0 _tmp60C;_tmp60C.f1=Cyc_Tcutil_compress((
void*)_tmp5DB->type);_tmp60C.f2=Cyc_Tcutil_compress((void*)_tmp5FF->type);
_tmp60C;});void*_tmp615;void*_tmp617;void*_tmp619;void*_tmp61B;void*_tmp61D;void*
_tmp61F;_LL1551: _LL1558: _tmp615=_tmp60D.f1;if((unsigned int)_tmp615 > 3?*((int*)
_tmp615)== Cyc_Absyn_IntType: 0){_LL1562: _tmp619=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp615)->f1;if(_tmp619 == (void*)Cyc_Absyn_Unsigned){goto _LL1560;}else{goto
_LL1553;}_LL1560: _tmp617=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp615)->f2;
if(_tmp617 == (void*)Cyc_Absyn_B4){goto _LL1552;}else{goto _LL1553;}}else{goto
_LL1553;}_LL1553: _LL1564: _tmp61B=_tmp60D.f2;if((unsigned int)_tmp61B > 3?*((int*)
_tmp61B)== Cyc_Absyn_IntType: 0){_LL1568: _tmp61F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp61B)->f1;if(_tmp61F == (void*)Cyc_Absyn_Unsigned){goto _LL1566;}else{goto
_LL1555;}_LL1566: _tmp61D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp61B)->f2;
if(_tmp61D == (void*)Cyc_Absyn_B4){goto _LL1554;}else{goto _LL1555;}}else{goto
_LL1555;}_LL1555: goto _LL1556;_LL1552: goto _LL1550;_LL1554: goto _LL1550;_LL1556:
return({struct _tuple0 _tmp621;_tmp621.f1=_tmp5AA;_tmp621.f2=_tmp5AA;_tmp621;});
_LL1550:;}{struct Cyc_List_List*_tmp622=({struct Cyc_List_List*_tmp626=_cycalloc(
sizeof(struct Cyc_List_List));_tmp626->hd=({struct Cyc_CfFlowInfo_Reln*_tmp627=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));_tmp627->vd=_tmp5DB;_tmp627->rop=(
void*)((void*)({struct Cyc_CfFlowInfo_LessVar_struct*_tmp628=_cycalloc(sizeof(
struct Cyc_CfFlowInfo_LessVar_struct));_tmp628[0]=({struct Cyc_CfFlowInfo_LessVar_struct
_tmp629;_tmp629.tag=Cyc_CfFlowInfo_LessVar;_tmp629.f1=_tmp5FF;_tmp629;});_tmp628;}));
_tmp627;});_tmp626->tl=_tmp5B2;_tmp626;});return({struct _tuple0 _tmp623;_tmp623.f1=(
void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp624=_cycalloc(sizeof(struct
Cyc_CfFlowInfo_ReachableFL_struct));_tmp624[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct
_tmp625;_tmp625.tag=Cyc_CfFlowInfo_ReachableFL;_tmp625.f1=_tmp5B4;_tmp625.f2=
_tmp622;_tmp625;});_tmp624;});_tmp623.f2=_tmp5AA;_tmp623;});}_LL1512:{struct
_tuple0 _tmp62B=({struct _tuple0 _tmp62A;_tmp62A.f1=Cyc_Tcutil_compress((void*)
_tmp5DB->type);_tmp62A.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp5BC->topt))->v);_tmp62A;});void*_tmp633;void*_tmp635;void*
_tmp637;void*_tmp639;void*_tmp63B;void*_tmp63D;_LL1581: _LL1588: _tmp633=_tmp62B.f1;
if((unsigned int)_tmp633 > 3?*((int*)_tmp633)== Cyc_Absyn_IntType: 0){_LL1592:
_tmp637=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp633)->f1;if(_tmp637 == (void*)
Cyc_Absyn_Unsigned){goto _LL1590;}else{goto _LL1583;}_LL1590: _tmp635=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp633)->f2;if(_tmp635 == (void*)Cyc_Absyn_B4){
goto _LL1582;}else{goto _LL1583;}}else{goto _LL1583;}_LL1583: _LL1594: _tmp639=
_tmp62B.f2;if((unsigned int)_tmp639 > 3?*((int*)_tmp639)== Cyc_Absyn_IntType: 0){
_LL1598: _tmp63D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp639)->f1;if(_tmp63D
== (void*)Cyc_Absyn_Unsigned){goto _LL1596;}else{goto _LL1585;}_LL1596: _tmp63B=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp639)->f2;if(_tmp63B == (void*)Cyc_Absyn_B4){
goto _LL1584;}else{goto _LL1585;}}else{goto _LL1585;}_LL1585: goto _LL1586;_LL1582:
goto _LL1580;_LL1584: goto _LL1580;_LL1586: return({struct _tuple0 _tmp63F;_tmp63F.f1=
_tmp5AA;_tmp63F.f2=_tmp5AA;_tmp63F;});_LL1580:;}{struct Cyc_List_List*_tmp640=({
struct Cyc_List_List*_tmp644=_cycalloc(sizeof(struct Cyc_List_List));_tmp644->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp645=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp645->vd=_tmp5DB;_tmp645->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessConst_struct*
_tmp646=_cycalloc_atomic(sizeof(struct Cyc_CfFlowInfo_LessConst_struct));_tmp646[
0]=({struct Cyc_CfFlowInfo_LessConst_struct _tmp647;_tmp647.tag=Cyc_CfFlowInfo_LessConst;
_tmp647.f1=(unsigned int)_tmp603;_tmp647;});_tmp646;}));_tmp645;});_tmp644->tl=
_tmp5B2;_tmp644;});return({struct _tuple0 _tmp641;_tmp641.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp642=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp642[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp643;_tmp643.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp643.f1=_tmp5B4;_tmp643.f2=_tmp640;_tmp643;});_tmp642;});_tmp641.f2=_tmp5AA;
_tmp641;});}_LL1514: {void*_tmp648=(void*)_tmp608->r;void*_tmp655;struct Cyc_Absyn_Vardecl*
_tmp657;void*_tmp65A;struct Cyc_Absyn_Vardecl*_tmp65C;void*_tmp65F;struct Cyc_Absyn_Vardecl*
_tmp661;void*_tmp664;struct Cyc_Absyn_Vardecl*_tmp666;_LL1610: if(*((int*)_tmp648)
== Cyc_Absyn_Var_e){_LL1622: _tmp655=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp648)->f2;if((unsigned int)_tmp655 > 1?*((int*)_tmp655)== Cyc_Absyn_Global_b: 0){
_LL1624: _tmp657=((struct Cyc_Absyn_Global_b_struct*)_tmp655)->f1;goto _LL1620;}
else{goto _LL1612;}}else{goto _LL1612;}_LL1620: if(! _tmp657->escapes){goto _LL1611;}
else{goto _LL1612;}_LL1612: if(*((int*)_tmp648)== Cyc_Absyn_Var_e){_LL1627: _tmp65A=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp648)->f2;if((unsigned int)_tmp65A > 1?*((
int*)_tmp65A)== Cyc_Absyn_Local_b: 0){_LL1629: _tmp65C=((struct Cyc_Absyn_Local_b_struct*)
_tmp65A)->f1;goto _LL1625;}else{goto _LL1614;}}else{goto _LL1614;}_LL1625: if(!
_tmp65C->escapes){goto _LL1613;}else{goto _LL1614;}_LL1614: if(*((int*)_tmp648)== 
Cyc_Absyn_Var_e){_LL1632: _tmp65F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp648)->f2;
if((unsigned int)_tmp65F > 1?*((int*)_tmp65F)== Cyc_Absyn_Pat_b: 0){_LL1634:
_tmp661=((struct Cyc_Absyn_Pat_b_struct*)_tmp65F)->f1;goto _LL1630;}else{goto
_LL1616;}}else{goto _LL1616;}_LL1630: if(! _tmp661->escapes){goto _LL1615;}else{goto
_LL1616;}_LL1616: if(*((int*)_tmp648)== Cyc_Absyn_Var_e){_LL1637: _tmp664=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp648)->f2;if((unsigned int)_tmp664 > 1?*((int*)
_tmp664)== Cyc_Absyn_Param_b: 0){_LL1639: _tmp666=((struct Cyc_Absyn_Param_b_struct*)
_tmp664)->f1;goto _LL1635;}else{goto _LL1618;}}else{goto _LL1618;}_LL1635: if(!
_tmp666->escapes){goto _LL1617;}else{goto _LL1618;}_LL1618: goto _LL1619;_LL1611:
_tmp65C=_tmp657;goto _LL1613;_LL1613: _tmp661=_tmp65C;goto _LL1615;_LL1615: _tmp666=
_tmp661;goto _LL1617;_LL1617: {struct Cyc_List_List*_tmp668=({struct Cyc_List_List*
_tmp66C=_cycalloc(sizeof(struct Cyc_List_List));_tmp66C->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp66D=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));_tmp66D->vd=_tmp5DB;_tmp66D->rop=(
void*)((void*)({struct Cyc_CfFlowInfo_LessSize_struct*_tmp66E=_cycalloc(sizeof(
struct Cyc_CfFlowInfo_LessSize_struct));_tmp66E[0]=({struct Cyc_CfFlowInfo_LessSize_struct
_tmp66F;_tmp66F.tag=Cyc_CfFlowInfo_LessSize;_tmp66F.f1=_tmp666;_tmp66F;});
_tmp66E;}));_tmp66D;});_tmp66C->tl=_tmp5B2;_tmp66C;});return({struct _tuple0
_tmp669;_tmp669.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp66A=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp66A[0]=({struct
Cyc_CfFlowInfo_ReachableFL_struct _tmp66B;_tmp66B.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp66B.f1=_tmp5B4;_tmp66B.f2=_tmp668;_tmp66B;});_tmp66A;});_tmp669.f2=_tmp5AA;
_tmp669;});}_LL1619: return({struct _tuple0 _tmp670;_tmp670.f1=_tmp5AA;_tmp670.f2=
_tmp5AA;_tmp670;});_LL1609:;}_LL1516: return({struct _tuple0 _tmp671;_tmp671.f1=
_tmp5AA;_tmp671.f2=_tmp5AA;_tmp671;});_LL1502:;}_LL1480: return({struct _tuple0
_tmp672;_tmp672.f1=_tmp5AA;_tmp672.f2=_tmp5AA;_tmp672;});_LL1470:;}}_LL1453:;}}
_LL1282: {struct Cyc_List_List*_tmp675;void*_tmp677;struct _tuple4 _tmp673=Cyc_NewControlFlow_anal_unordered_Rexps(
env,inflow,_tmp524);_LL1656: _tmp677=_tmp673.f1;goto _LL1654;_LL1654: _tmp675=
_tmp673.f2;goto _LL1652;_LL1652: {void*_tmp679=_tmp677;struct Cyc_List_List*
_tmp67F;struct Cyc_Dict_Dict*_tmp681;_LL1659: if(_tmp679 == (void*)Cyc_CfFlowInfo_BottomFL){
goto _LL1660;}else{goto _LL1661;}_LL1661: if((unsigned int)_tmp679 > 1?*((int*)
_tmp679)== Cyc_CfFlowInfo_ReachableFL: 0){_LL1666: _tmp681=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp679)->f1;goto _LL1664;_LL1664: _tmp67F=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp679)->f2;goto _LL1662;}else{goto _LL1658;}_LL1660: return({struct _tuple0 _tmp683;
_tmp683.f1=_tmp677;_tmp683.f2=_tmp677;_tmp683;});_LL1662: {void*_tmp684=(void*)((
struct Cyc_List_List*)_check_null(_tmp675))->hd;void*_tmp685=(void*)((struct Cyc_List_List*)
_check_null(_tmp675->tl))->hd;if(Cyc_CfFlowInfo_initlevel(_tmp681,_tmp684)== (
void*)Cyc_CfFlowInfo_NoneIL){({void*_tmp686[0]={};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp524))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp686,sizeof(void*),0));});}if(Cyc_CfFlowInfo_initlevel(
_tmp681,_tmp685)== (void*)Cyc_CfFlowInfo_NoneIL){({void*_tmp687[0]={};Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp524))->tl))->hd)->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp687,sizeof(void*),0));});}{struct Cyc_Absyn_Exp*
_tmp688=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp524))->hd;
struct Cyc_Absyn_Exp*_tmp689=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp524->tl))->hd;void*_tmp68A=(void*)_tmp688->r;void*_tmp697;struct
Cyc_Absyn_Vardecl*_tmp699;void*_tmp69C;struct Cyc_Absyn_Vardecl*_tmp69E;void*
_tmp6A1;struct Cyc_Absyn_Vardecl*_tmp6A3;void*_tmp6A6;struct Cyc_Absyn_Vardecl*
_tmp6A8;_LL1676: if(*((int*)_tmp68A)== Cyc_Absyn_Var_e){_LL1688: _tmp697=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp68A)->f2;if((unsigned int)_tmp697 > 1?*((int*)
_tmp697)== Cyc_Absyn_Global_b: 0){_LL1690: _tmp699=((struct Cyc_Absyn_Global_b_struct*)
_tmp697)->f1;goto _LL1686;}else{goto _LL1678;}}else{goto _LL1678;}_LL1686: if(!
_tmp699->escapes){goto _LL1677;}else{goto _LL1678;}_LL1678: if(*((int*)_tmp68A)== 
Cyc_Absyn_Var_e){_LL1693: _tmp69C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp68A)->f2;
if((unsigned int)_tmp69C > 1?*((int*)_tmp69C)== Cyc_Absyn_Local_b: 0){_LL1695:
_tmp69E=((struct Cyc_Absyn_Local_b_struct*)_tmp69C)->f1;goto _LL1691;}else{goto
_LL1680;}}else{goto _LL1680;}_LL1691: if(! _tmp69E->escapes){goto _LL1679;}else{goto
_LL1680;}_LL1680: if(*((int*)_tmp68A)== Cyc_Absyn_Var_e){_LL1698: _tmp6A1=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp68A)->f2;if((unsigned int)_tmp6A1 > 1?*((int*)
_tmp6A1)== Cyc_Absyn_Pat_b: 0){_LL1700: _tmp6A3=((struct Cyc_Absyn_Pat_b_struct*)
_tmp6A1)->f1;goto _LL1696;}else{goto _LL1682;}}else{goto _LL1682;}_LL1696: if(!
_tmp6A3->escapes){goto _LL1681;}else{goto _LL1682;}_LL1682: if(*((int*)_tmp68A)== 
Cyc_Absyn_Var_e){_LL1703: _tmp6A6=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp68A)->f2;
if((unsigned int)_tmp6A6 > 1?*((int*)_tmp6A6)== Cyc_Absyn_Param_b: 0){_LL1705:
_tmp6A8=((struct Cyc_Absyn_Param_b_struct*)_tmp6A6)->f1;goto _LL1701;}else{goto
_LL1684;}}else{goto _LL1684;}_LL1701: if(! _tmp6A8->escapes){goto _LL1683;}else{goto
_LL1684;}_LL1684: goto _LL1685;_LL1677: _tmp69E=_tmp699;goto _LL1679;_LL1679: _tmp6A3=
_tmp69E;goto _LL1681;_LL1681: _tmp6A8=_tmp6A3;goto _LL1683;_LL1683: {void*_tmp6AA=(
void*)_tmp689->r;struct Cyc_List_List*_tmp6B0;struct Cyc_List_List _tmp6B2;struct
Cyc_Absyn_Exp*_tmp6B3;void*_tmp6B5;_LL1708: if(*((int*)_tmp6AA)== Cyc_Absyn_Primop_e){
_LL1718: _tmp6B5=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp6AA)->f1;if(
_tmp6B5 == (void*)Cyc_Absyn_Size){goto _LL1713;}else{goto _LL1710;}_LL1713: _tmp6B0=((
struct Cyc_Absyn_Primop_e_struct*)_tmp6AA)->f2;if(_tmp6B0 == 0){goto _LL1710;}else{
_tmp6B2=*_tmp6B0;_LL1716: _tmp6B3=(struct Cyc_Absyn_Exp*)_tmp6B2.hd;goto _LL1709;}}
else{goto _LL1710;}_LL1710: goto _LL1711;_LL1709: {void*_tmp6B7=(void*)_tmp6B3->r;
void*_tmp6C4;struct Cyc_Absyn_Vardecl*_tmp6C6;void*_tmp6C9;struct Cyc_Absyn_Vardecl*
_tmp6CB;void*_tmp6CE;struct Cyc_Absyn_Vardecl*_tmp6D0;void*_tmp6D3;struct Cyc_Absyn_Vardecl*
_tmp6D5;_LL1721: if(*((int*)_tmp6B7)== Cyc_Absyn_Var_e){_LL1733: _tmp6C4=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp6B7)->f2;if((unsigned int)_tmp6C4 > 1?*((int*)
_tmp6C4)== Cyc_Absyn_Global_b: 0){_LL1735: _tmp6C6=((struct Cyc_Absyn_Global_b_struct*)
_tmp6C4)->f1;goto _LL1731;}else{goto _LL1723;}}else{goto _LL1723;}_LL1731: if(!
_tmp6C6->escapes){goto _LL1722;}else{goto _LL1723;}_LL1723: if(*((int*)_tmp6B7)== 
Cyc_Absyn_Var_e){_LL1738: _tmp6C9=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6B7)->f2;
if((unsigned int)_tmp6C9 > 1?*((int*)_tmp6C9)== Cyc_Absyn_Local_b: 0){_LL1740:
_tmp6CB=((struct Cyc_Absyn_Local_b_struct*)_tmp6C9)->f1;goto _LL1736;}else{goto
_LL1725;}}else{goto _LL1725;}_LL1736: if(! _tmp6CB->escapes){goto _LL1724;}else{goto
_LL1725;}_LL1725: if(*((int*)_tmp6B7)== Cyc_Absyn_Var_e){_LL1743: _tmp6CE=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp6B7)->f2;if((unsigned int)_tmp6CE > 1?*((int*)
_tmp6CE)== Cyc_Absyn_Pat_b: 0){_LL1745: _tmp6D0=((struct Cyc_Absyn_Pat_b_struct*)
_tmp6CE)->f1;goto _LL1741;}else{goto _LL1727;}}else{goto _LL1727;}_LL1741: if(!
_tmp6D0->escapes){goto _LL1726;}else{goto _LL1727;}_LL1727: if(*((int*)_tmp6B7)== 
Cyc_Absyn_Var_e){_LL1748: _tmp6D3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp6B7)->f2;
if((unsigned int)_tmp6D3 > 1?*((int*)_tmp6D3)== Cyc_Absyn_Param_b: 0){_LL1750:
_tmp6D5=((struct Cyc_Absyn_Param_b_struct*)_tmp6D3)->f1;goto _LL1746;}else{goto
_LL1729;}}else{goto _LL1729;}_LL1746: if(! _tmp6D5->escapes){goto _LL1728;}else{goto
_LL1729;}_LL1729: goto _LL1730;_LL1722: _tmp6CB=_tmp6C6;goto _LL1724;_LL1724: _tmp6D0=
_tmp6CB;goto _LL1726;_LL1726: _tmp6D5=_tmp6D0;goto _LL1728;_LL1728: {struct Cyc_List_List*
_tmp6D7=({struct Cyc_List_List*_tmp6DB=_cycalloc(sizeof(struct Cyc_List_List));
_tmp6DB->hd=({struct Cyc_CfFlowInfo_Reln*_tmp6DC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Reln));
_tmp6DC->vd=_tmp6A8;_tmp6DC->rop=(void*)((void*)({struct Cyc_CfFlowInfo_LessEqSize_struct*
_tmp6DD=_cycalloc(sizeof(struct Cyc_CfFlowInfo_LessEqSize_struct));_tmp6DD[0]=({
struct Cyc_CfFlowInfo_LessEqSize_struct _tmp6DE;_tmp6DE.tag=Cyc_CfFlowInfo_LessEqSize;
_tmp6DE.f1=_tmp6D5;_tmp6DE;});_tmp6DD;}));_tmp6DC;});_tmp6DB->tl=_tmp67F;_tmp6DB;});
return({struct _tuple0 _tmp6D8;_tmp6D8.f1=(void*)({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp6D9=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp6D9[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp6DA;_tmp6DA.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp6DA.f1=_tmp681;_tmp6DA.f2=_tmp6D7;_tmp6DA;});_tmp6D9;});_tmp6D8.f2=_tmp677;
_tmp6D8;});}_LL1730: return({struct _tuple0 _tmp6DF;_tmp6DF.f1=_tmp677;_tmp6DF.f2=
_tmp677;_tmp6DF;});_LL1720:;}_LL1711: return({struct _tuple0 _tmp6E0;_tmp6E0.f1=
_tmp677;_tmp6E0.f2=_tmp677;_tmp6E0;});_LL1707:;}_LL1685: return({struct _tuple0
_tmp6E1;_tmp6E1.f1=_tmp677;_tmp6E1.f2=_tmp677;_tmp6E1;});_LL1675:;}}_LL1658:;}}
_LL1284: goto _LL1268;_LL1268:;}{void*_tmp6E4;void*_tmp6E6;struct _tuple0 _tmp6E2=
Cyc_NewControlFlow_anal_Rexp(env,inflow,e);_LL1767: _tmp6E6=_tmp6E2.f1;goto
_LL1765;_LL1765: _tmp6E4=_tmp6E2.f2;goto _LL1763;_LL1763: {void*_tmp6E8=_tmp6E6;
struct Cyc_List_List*_tmp6EE;struct Cyc_Dict_Dict*_tmp6F0;_LL1770: if(_tmp6E8 == (
void*)Cyc_CfFlowInfo_BottomFL){goto _LL1771;}else{goto _LL1772;}_LL1772: if((
unsigned int)_tmp6E8 > 1?*((int*)_tmp6E8)== Cyc_CfFlowInfo_ReachableFL: 0){_LL1777:
_tmp6F0=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp6E8)->f1;goto _LL1775;
_LL1775: _tmp6EE=((struct Cyc_CfFlowInfo_ReachableFL_struct*)_tmp6E8)->f2;goto
_LL1773;}else{goto _LL1769;}_LL1771: return({struct _tuple0 _tmp6F2;_tmp6F2.f1=
_tmp6E6;_tmp6F2.f2=_tmp6E6;_tmp6F2;});_LL1773: {void*_tmp6F3=_tmp6E4;void*
_tmp707;void*_tmp709;void*_tmp70B;_LL1781: if(_tmp6F3 == (void*)Cyc_CfFlowInfo_Zero){
goto _LL1782;}else{goto _LL1783;}_LL1783: if(_tmp6F3 == (void*)Cyc_CfFlowInfo_NotZeroThis){
goto _LL1784;}else{goto _LL1785;}_LL1785: if(_tmp6F3 == (void*)Cyc_CfFlowInfo_NotZeroAll){
goto _LL1786;}else{goto _LL1787;}_LL1787: if((unsigned int)_tmp6F3 > 3?*((int*)
_tmp6F3)== Cyc_CfFlowInfo_AddressOf: 0){goto _LL1788;}else{goto _LL1789;}_LL1789:
if((unsigned int)_tmp6F3 > 3?*((int*)_tmp6F3)== Cyc_CfFlowInfo_UnknownR: 0){
_LL1800: _tmp707=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp6F3)->f1;if(
_tmp707 == (void*)Cyc_CfFlowInfo_NoneIL){goto _LL1790;}else{goto _LL1791;}}else{
goto _LL1791;}_LL1791: if((unsigned int)_tmp6F3 > 3?*((int*)_tmp6F3)== Cyc_CfFlowInfo_Esc:
0){_LL1802: _tmp709=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)_tmp6F3)->f1;if(
_tmp709 == (void*)Cyc_CfFlowInfo_NoneIL){goto _LL1792;}else{goto _LL1793;}}else{
goto _LL1793;}_LL1793: if((unsigned int)_tmp6F3 > 3?*((int*)_tmp6F3)== Cyc_CfFlowInfo_UnknownR:
0){_LL1804: _tmp70B=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp6F3)->f1;
goto _LL1794;}else{goto _LL1795;}_LL1795: if((unsigned int)_tmp6F3 > 3?*((int*)
_tmp6F3)== Cyc_CfFlowInfo_Esc: 0){goto _LL1796;}else{goto _LL1797;}_LL1797: if((
unsigned int)_tmp6F3 > 3?*((int*)_tmp6F3)== Cyc_CfFlowInfo_Aggregate: 0){goto
_LL1798;}else{goto _LL1780;}_LL1782: return({struct _tuple0 _tmp70D;_tmp70D.f1=(void*)
Cyc_CfFlowInfo_BottomFL;_tmp70D.f2=_tmp6E6;_tmp70D;});_LL1784: goto _LL1786;
_LL1786: goto _LL1788;_LL1788: return({struct _tuple0 _tmp70E;_tmp70E.f1=_tmp6E6;
_tmp70E.f2=(void*)Cyc_CfFlowInfo_BottomFL;_tmp70E;});_LL1790: goto _LL1792;_LL1792:({
void*_tmp70F[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("expression may not be initialized",
sizeof(unsigned char),34),_tag_arr(_tmp70F,sizeof(void*),0));});return({struct
_tuple0 _tmp710;_tmp710.f1=(void*)Cyc_CfFlowInfo_BottomFL;_tmp710.f2=(void*)Cyc_CfFlowInfo_BottomFL;
_tmp710;});_LL1794: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp6E6,e,
_tmp70B);_LL1796: return({struct _tuple0 _tmp711;_tmp711.f1=_tmp6E6;_tmp711.f2=
_tmp6E6;_tmp711;});_LL1798:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp712=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp712[0]=({struct
Cyc_Core_Impossible_struct _tmp713;_tmp713.tag=Cyc_Core_Impossible;_tmp713.f1=
_tag_arr("anal_test",sizeof(unsigned char),10);_tmp713;});_tmp712;}));_LL1780:;}
_LL1769:;}}}static void*Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*
env,void*inflow,struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause
_tmp716;struct Cyc_Absyn_Stmt*_tmp717;struct Cyc_Absyn_Exp*_tmp719;struct Cyc_Core_Opt*
_tmp71B;struct Cyc_Absyn_Switch_clause*_tmp714=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp716=*_tmp714;_LL1820: _tmp71B=_tmp716.pat_vars;goto _LL1818;_LL1818:
_tmp719=_tmp716.where_clause;goto _LL1816;_LL1816: _tmp717=_tmp716.body;goto
_LL1813;_LL1813: {void*clause_inflow=Cyc_NewControlFlow_add_vars(inflow,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp71B))->v,Cyc_CfFlowInfo_unknown_all);
void*clause_outflow;if(_tmp719 != 0){void*_tmp71F;void*_tmp721;struct _tuple0
_tmp71D=Cyc_NewControlFlow_anal_test(env,clause_inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp719));_LL1826: _tmp721=_tmp71D.f1;goto _LL1824;_LL1824: _tmp71F=
_tmp71D.f2;goto _LL1822;_LL1822: inflow=_tmp71F;clause_outflow=Cyc_NewControlFlow_anal_stmt(
env,_tmp721,_tmp717);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,
clause_inflow,_tmp717);}{void*_tmp723=clause_outflow;_LL1829: if(_tmp723 == (void*)
Cyc_CfFlowInfo_BottomFL){goto _LL1830;}else{goto _LL1831;}_LL1831: goto _LL1832;
_LL1830: goto _LL1828;_LL1832: if(scs->tl == 0){return clause_outflow;}else{if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){({void*
_tmp729[0]={};Cyc_Tcutil_terr(_tmp717->loc,_tag_arr("switch clause may implicitly fallthru",
sizeof(unsigned char),38),_tag_arr(_tmp729,sizeof(void*),0));});}else{({void*
_tmp72A[0]={};Cyc_Tcutil_warn(_tmp717->loc,_tag_arr("switch clause may implicitly fallthru",
sizeof(unsigned char),38),_tag_arr(_tmp72A,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(
env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,
clause_outflow);}goto _LL1828;_LL1828:;}}}return(void*)Cyc_CfFlowInfo_BottomFL;}
static void*Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,
void*inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp72B=
Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);inflow=(void*)_tmp72B->flow;{
void*_tmp72C=(void*)s->r;struct Cyc_Absyn_Exp*_tmp756;struct Cyc_Absyn_Exp*_tmp758;
struct Cyc_Absyn_Exp*_tmp75A;struct Cyc_Absyn_Stmt*_tmp75C;struct Cyc_Absyn_Stmt*
_tmp75E;struct Cyc_Absyn_Stmt*_tmp760;struct Cyc_Absyn_Stmt*_tmp762;struct Cyc_Absyn_Exp*
_tmp764;struct Cyc_Absyn_Stmt*_tmp766;struct _tuple3 _tmp768;struct Cyc_Absyn_Stmt*
_tmp76A;struct Cyc_Absyn_Exp*_tmp76C;struct _tuple3 _tmp76E;struct Cyc_Absyn_Stmt*
_tmp770;struct Cyc_Absyn_Exp*_tmp772;struct Cyc_Absyn_Stmt*_tmp774;struct Cyc_Absyn_Stmt*
_tmp776;struct _tuple3 _tmp778;struct Cyc_Absyn_Stmt*_tmp77A;struct Cyc_Absyn_Exp*
_tmp77C;struct _tuple3 _tmp77E;struct Cyc_Absyn_Stmt*_tmp780;struct Cyc_Absyn_Exp*
_tmp782;struct Cyc_Absyn_Exp*_tmp784;struct Cyc_Absyn_Stmt*_tmp786;struct Cyc_Absyn_Switch_clause**
_tmp788;struct Cyc_Absyn_Switch_clause*_tmp78A;struct Cyc_List_List*_tmp78B;struct
Cyc_Absyn_Stmt*_tmp78D;struct Cyc_Absyn_Stmt*_tmp78F;struct Cyc_Absyn_Stmt*_tmp791;
struct Cyc_List_List*_tmp793;struct Cyc_Absyn_Exp*_tmp795;struct Cyc_List_List*
_tmp797;struct Cyc_Absyn_Stmt*_tmp799;struct Cyc_Absyn_Stmt*_tmp79B;struct Cyc_Absyn_Decl*
_tmp79D;struct Cyc_Absyn_Stmt*_tmp79F;struct Cyc_Absyn_Stmt*_tmp7A1;struct Cyc_Absyn_Vardecl*
_tmp7A3;struct Cyc_Absyn_Tvar*_tmp7A5;_LL1838: if(_tmp72C == (void*)Cyc_Absyn_Skip_s){
goto _LL1839;}else{goto _LL1840;}_LL1840: if((unsigned int)_tmp72C > 1?*((int*)
_tmp72C)== Cyc_Absyn_Return_s: 0){_LL1879: _tmp756=((struct Cyc_Absyn_Return_s_struct*)
_tmp72C)->f1;if(_tmp756 == 0){goto _LL1841;}else{goto _LL1842;}}else{goto _LL1842;}
_LL1842: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)== Cyc_Absyn_Return_s: 0){
_LL1881: _tmp758=((struct Cyc_Absyn_Return_s_struct*)_tmp72C)->f1;goto _LL1843;}
else{goto _LL1844;}_LL1844: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)== Cyc_Absyn_Exp_s:
0){_LL1883: _tmp75A=((struct Cyc_Absyn_Exp_s_struct*)_tmp72C)->f1;goto _LL1845;}
else{goto _LL1846;}_LL1846: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)== Cyc_Absyn_Seq_s:
0){_LL1887: _tmp75E=((struct Cyc_Absyn_Seq_s_struct*)_tmp72C)->f1;goto _LL1885;
_LL1885: _tmp75C=((struct Cyc_Absyn_Seq_s_struct*)_tmp72C)->f2;goto _LL1847;}else{
goto _LL1848;}_LL1848: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)== Cyc_Absyn_IfThenElse_s:
0){_LL1893: _tmp764=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp72C)->f1;goto
_LL1891;_LL1891: _tmp762=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp72C)->f2;goto
_LL1889;_LL1889: _tmp760=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp72C)->f3;goto
_LL1849;}else{goto _LL1850;}_LL1850: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)
== Cyc_Absyn_While_s: 0){_LL1897: _tmp768=((struct Cyc_Absyn_While_s_struct*)
_tmp72C)->f1;_LL1901: _tmp76C=_tmp768.f1;goto _LL1899;_LL1899: _tmp76A=_tmp768.f2;
goto _LL1895;_LL1895: _tmp766=((struct Cyc_Absyn_While_s_struct*)_tmp72C)->f2;goto
_LL1851;}else{goto _LL1852;}_LL1852: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)
== Cyc_Absyn_Do_s: 0){_LL1909: _tmp774=((struct Cyc_Absyn_Do_s_struct*)_tmp72C)->f1;
goto _LL1903;_LL1903: _tmp76E=((struct Cyc_Absyn_Do_s_struct*)_tmp72C)->f2;_LL1907:
_tmp772=_tmp76E.f1;goto _LL1905;_LL1905: _tmp770=_tmp76E.f2;goto _LL1853;}else{goto
_LL1854;}_LL1854: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)== Cyc_Absyn_For_s:
0){_LL1925: _tmp784=((struct Cyc_Absyn_For_s_struct*)_tmp72C)->f1;goto _LL1919;
_LL1919: _tmp77E=((struct Cyc_Absyn_For_s_struct*)_tmp72C)->f2;_LL1923: _tmp782=
_tmp77E.f1;goto _LL1921;_LL1921: _tmp780=_tmp77E.f2;goto _LL1913;_LL1913: _tmp778=((
struct Cyc_Absyn_For_s_struct*)_tmp72C)->f3;_LL1917: _tmp77C=_tmp778.f1;goto
_LL1915;_LL1915: _tmp77A=_tmp778.f2;goto _LL1911;_LL1911: _tmp776=((struct Cyc_Absyn_For_s_struct*)
_tmp72C)->f4;goto _LL1855;}else{goto _LL1856;}_LL1856: if((unsigned int)_tmp72C > 1?*((
int*)_tmp72C)== Cyc_Absyn_Break_s: 0){_LL1927: _tmp786=((struct Cyc_Absyn_Break_s_struct*)
_tmp72C)->f1;if(_tmp786 == 0){goto _LL1857;}else{goto _LL1858;}}else{goto _LL1858;}
_LL1858: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)== Cyc_Absyn_Fallthru_s: 0){
_LL1932: _tmp78B=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp72C)->f1;goto _LL1929;
_LL1929: _tmp788=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp72C)->f2;if(_tmp788 == 0){
goto _LL1860;}else{_tmp78A=*_tmp788;goto _LL1859;}}else{goto _LL1860;}_LL1860: if((
unsigned int)_tmp72C > 1?*((int*)_tmp72C)== Cyc_Absyn_Break_s: 0){_LL1934: _tmp78D=((
struct Cyc_Absyn_Break_s_struct*)_tmp72C)->f1;goto _LL1861;}else{goto _LL1862;}
_LL1862: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)== Cyc_Absyn_Continue_s: 0){
_LL1936: _tmp78F=((struct Cyc_Absyn_Continue_s_struct*)_tmp72C)->f1;goto _LL1863;}
else{goto _LL1864;}_LL1864: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)== Cyc_Absyn_Goto_s:
0){_LL1938: _tmp791=((struct Cyc_Absyn_Goto_s_struct*)_tmp72C)->f2;goto _LL1865;}
else{goto _LL1866;}_LL1866: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)== Cyc_Absyn_Switch_s:
0){_LL1942: _tmp795=((struct Cyc_Absyn_Switch_s_struct*)_tmp72C)->f1;goto _LL1940;
_LL1940: _tmp793=((struct Cyc_Absyn_Switch_s_struct*)_tmp72C)->f2;goto _LL1867;}
else{goto _LL1868;}_LL1868: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)== Cyc_Absyn_TryCatch_s:
0){_LL1946: _tmp799=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp72C)->f1;goto _LL1944;
_LL1944: _tmp797=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp72C)->f2;goto _LL1869;}
else{goto _LL1870;}_LL1870: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)== Cyc_Absyn_Decl_s:
0){_LL1950: _tmp79D=((struct Cyc_Absyn_Decl_s_struct*)_tmp72C)->f1;goto _LL1948;
_LL1948: _tmp79B=((struct Cyc_Absyn_Decl_s_struct*)_tmp72C)->f2;goto _LL1871;}else{
goto _LL1872;}_LL1872: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)== Cyc_Absyn_Label_s:
0){_LL1952: _tmp79F=((struct Cyc_Absyn_Label_s_struct*)_tmp72C)->f2;goto _LL1873;}
else{goto _LL1874;}_LL1874: if((unsigned int)_tmp72C > 1?*((int*)_tmp72C)== Cyc_Absyn_Region_s:
0){_LL1958: _tmp7A5=((struct Cyc_Absyn_Region_s_struct*)_tmp72C)->f1;goto _LL1956;
_LL1956: _tmp7A3=((struct Cyc_Absyn_Region_s_struct*)_tmp72C)->f2;goto _LL1954;
_LL1954: _tmp7A1=((struct Cyc_Absyn_Region_s_struct*)_tmp72C)->f3;goto _LL1875;}
else{goto _LL1876;}_LL1876: goto _LL1877;_LL1839: return inflow;_LL1841: if(env->noreturn){({
void*_tmp7A7[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("`noreturn' function might return",
sizeof(unsigned char),33),_tag_arr(_tmp7A7,sizeof(void*),0));});}return(void*)
Cyc_CfFlowInfo_BottomFL;_LL1843: if(env->noreturn){({void*_tmp7A8[0]={};Cyc_Tcutil_terr(
s->loc,_tag_arr("`noreturn' function might return",sizeof(unsigned char),33),
_tag_arr(_tmp7A8,sizeof(void*),0));});}{void*_tmp7AB;void*_tmp7AD;struct _tuple0
_tmp7A9=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp758));_LL1966: _tmp7AD=_tmp7A9.f1;goto _LL1964;_LL1964: _tmp7AB=
_tmp7A9.f2;goto _LL1962;_LL1962: Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)
_check_null(_tmp758))->loc,_tmp7AD,_tmp7AB);return(void*)Cyc_CfFlowInfo_BottomFL;}
_LL1845: return(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp75A)).f1;_LL1847:
return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,
_tmp75E),_tmp75C);_LL1849: {void*_tmp7B1;void*_tmp7B3;struct _tuple0 _tmp7AF=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp764);_LL1972: _tmp7B3=_tmp7AF.f1;goto _LL1970;_LL1970: _tmp7B1=
_tmp7AF.f2;goto _LL1968;_LL1968: return Cyc_CfFlowInfo_join_flow(env->all_changed,
Cyc_NewControlFlow_anal_stmt(env,_tmp7B3,_tmp762),Cyc_NewControlFlow_anal_stmt(
env,_tmp7B1,_tmp760));}_LL1851: {void*_tmp7B5=(void*)(Cyc_NewControlFlow_pre_stmt_check(
env,inflow,_tmp76A))->flow;void*_tmp7B8;void*_tmp7BA;struct _tuple0 _tmp7B6=Cyc_NewControlFlow_anal_test(
env,_tmp7B5,_tmp76C);_LL1979: _tmp7BA=_tmp7B6.f1;goto _LL1977;_LL1977: _tmp7B8=
_tmp7B6.f2;goto _LL1975;_LL1975: {void*_tmp7BC=Cyc_NewControlFlow_anal_stmt(env,
_tmp7BA,_tmp766);Cyc_NewControlFlow_update_flow(env,_tmp76A,_tmp7BC);return
_tmp7B8;}}_LL1853: {void*_tmp7BD=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp774);
void*_tmp7BE=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp7BD,_tmp770))->flow;
void*_tmp7C1;void*_tmp7C3;struct _tuple0 _tmp7BF=Cyc_NewControlFlow_anal_test(env,
_tmp7BE,_tmp772);_LL1988: _tmp7C3=_tmp7BF.f1;goto _LL1986;_LL1986: _tmp7C1=_tmp7BF.f2;
goto _LL1984;_LL1984: Cyc_NewControlFlow_update_flow(env,_tmp774,_tmp7C3);return
_tmp7C1;}_LL1855: {void*_tmp7C5=(Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp784)).f1;
void*_tmp7C6=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp7C5,_tmp780))->flow;
void*_tmp7C9;void*_tmp7CB;struct _tuple0 _tmp7C7=Cyc_NewControlFlow_anal_test(env,
_tmp7C6,_tmp782);_LL1996: _tmp7CB=_tmp7C7.f1;goto _LL1994;_LL1994: _tmp7C9=_tmp7C7.f2;
goto _LL1992;_LL1992: {void*_tmp7CD=Cyc_NewControlFlow_anal_stmt(env,_tmp7CB,
_tmp776);void*_tmp7CE=(void*)(Cyc_NewControlFlow_pre_stmt_check(env,_tmp7CD,
_tmp77A))->flow;void*_tmp7CF=(Cyc_NewControlFlow_anal_Rexp(env,_tmp7CE,_tmp77C)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp780,_tmp7CF);return _tmp7C9;}}_LL1857:
return(void*)Cyc_CfFlowInfo_BottomFL;_LL1859: {struct Cyc_List_List*_tmp7D2;void*
_tmp7D4;struct _tuple4 _tmp7D0=Cyc_NewControlFlow_anal_unordered_Rexps(env,inflow,
_tmp78B);_LL2005: _tmp7D4=_tmp7D0.f1;goto _LL2003;_LL2003: _tmp7D2=_tmp7D0.f2;goto
_LL2001;_LL2001: for(0;_tmp7D2 != 0;(_tmp7D2=_tmp7D2->tl,_tmp78B=_tmp78B->tl)){
_tmp7D4=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp78B))->hd)->loc,_tmp7D4,(void*)_tmp7D2->hd);}_tmp7D4=Cyc_NewControlFlow_add_vars(
_tmp7D4,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp78A->pat_vars))->v,
Cyc_CfFlowInfo_unknown_all);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp78A->body,_tmp7D4);return(void*)Cyc_CfFlowInfo_BottomFL;}_LL1861: _tmp78F=
_tmp78D;goto _LL1863;_LL1863: _tmp791=_tmp78F;goto _LL1865;_LL1865: if(env->iteration_num
== 1){struct Cyc_Absyn_Stmt*_tmp7D6=_tmp72B->encloser;struct Cyc_Absyn_Stmt*
_tmp7D7=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(
_tmp791)))->encloser;while(_tmp7D7 != _tmp7D6){struct Cyc_Absyn_Stmt*_tmp7D8=(Cyc_NewControlFlow_get_stmt_annot(
_tmp7D6))->encloser;if(_tmp7D8 == _tmp7D6){({void*_tmp7D9[0]={};Cyc_Tcutil_terr(s->loc,
_tag_arr("goto enters local scope or exception handler",sizeof(unsigned char),45),
_tag_arr(_tmp7D9,sizeof(void*),0));});break;}_tmp7D6=_tmp7D8;}}Cyc_NewControlFlow_update_flow(
env,(struct Cyc_Absyn_Stmt*)_check_null(_tmp791),inflow);return(void*)Cyc_CfFlowInfo_BottomFL;
_LL1867: {void*_tmp7DC;void*_tmp7DE;struct _tuple0 _tmp7DA=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp795);_LL2015: _tmp7DE=_tmp7DA.f1;goto _LL2013;_LL2013: _tmp7DC=
_tmp7DA.f2;goto _LL2011;_LL2011: _tmp7DE=Cyc_NewControlFlow_use_Rval(env,_tmp795->loc,
_tmp7DE,_tmp7DC);return Cyc_NewControlFlow_anal_scs(env,_tmp7DE,_tmp793);}_LL1869: {
int old_in_try=env->in_try;void*old_tryflow=(void*)env->tryflow;env->in_try=1;(
void*)(env->tryflow=(void*)inflow);{void*s1_outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp799);void*scs_inflow=(void*)env->tryflow;env->in_try=old_in_try;(
void*)(env->tryflow=(void*)old_tryflow);Cyc_NewControlFlow_update_tryflow(env,
scs_inflow);{void*scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp797);{
void*_tmp7E0=scs_outflow;_LL2018: if(_tmp7E0 == (void*)Cyc_CfFlowInfo_BottomFL){
goto _LL2019;}else{goto _LL2020;}_LL2020: goto _LL2021;_LL2019: goto _LL2017;_LL2021:({
void*_tmp7E6[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("last catch clause may implicitly fallthru",
sizeof(unsigned char),42),_tag_arr(_tmp7E6,sizeof(void*),0));});_LL2017:;}return
s1_outflow;}}}_LL1871: return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp79D),_tmp79B);_LL1873: return Cyc_NewControlFlow_anal_stmt(env,
inflow,_tmp79F);_LL1875: {struct Cyc_List_List _tmp7E7=({struct Cyc_List_List
_tmp7E8;_tmp7E8.hd=_tmp7A3;_tmp7E8.tl=0;_tmp7E8;});inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)& _tmp7E7,Cyc_CfFlowInfo_unknown_all);return Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp7A1);}_LL1877:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp7E9=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp7E9[0]=({struct
Cyc_Core_Impossible_struct _tmp7EA;_tmp7EA.tag=Cyc_Core_Impossible;_tmp7EA.f1=
_tag_arr("anal_stmt -- bad stmt syntax or unimplemented stmt form",sizeof(
unsigned char),56);_tmp7EA;});_tmp7E9;}));_LL1837:;}}static void*Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,void*inflow,struct Cyc_Absyn_Decl*decl){void*
_tmp7EB=(void*)decl->r;struct Cyc_Absyn_Vardecl*_tmp7F5;struct Cyc_Absyn_Exp*
_tmp7F7;struct Cyc_Core_Opt*_tmp7F9;struct Cyc_Core_Opt _tmp7FB;struct Cyc_List_List*
_tmp7FC;struct Cyc_List_List*_tmp7FE;_LL2029: if(*((int*)_tmp7EB)== Cyc_Absyn_Var_d){
_LL2038: _tmp7F5=((struct Cyc_Absyn_Var_d_struct*)_tmp7EB)->f1;goto _LL2030;}else{
goto _LL2031;}_LL2031: if(*((int*)_tmp7EB)== Cyc_Absyn_Let_d){_LL2042: _tmp7F9=((
struct Cyc_Absyn_Let_d_struct*)_tmp7EB)->f2;if(_tmp7F9 == 0){goto _LL2033;}else{
_tmp7FB=*_tmp7F9;_LL2045: _tmp7FC=(struct Cyc_List_List*)_tmp7FB.v;goto _LL2040;}
_LL2040: _tmp7F7=((struct Cyc_Absyn_Let_d_struct*)_tmp7EB)->f4;goto _LL2032;}else{
goto _LL2033;}_LL2033: if(*((int*)_tmp7EB)== Cyc_Absyn_Letv_d){_LL2047: _tmp7FE=((
struct Cyc_Absyn_Letv_d_struct*)_tmp7EB)->f1;goto _LL2034;}else{goto _LL2035;}
_LL2035: goto _LL2036;_LL2030: {struct Cyc_List_List _tmp800=({struct Cyc_List_List
_tmp818;_tmp818.hd=_tmp7F5;_tmp818.tl=0;_tmp818;});inflow=Cyc_NewControlFlow_add_vars(
inflow,(struct Cyc_List_List*)& _tmp800,Cyc_CfFlowInfo_unknown_none);{struct Cyc_Absyn_Exp*
_tmp801=_tmp7F5->initializer;if(_tmp801 == 0){return inflow;}{void*_tmp804;void*
_tmp806;struct _tuple0 _tmp802=Cyc_NewControlFlow_anal_Rexp(env,inflow,(struct Cyc_Absyn_Exp*)
_check_null(_tmp801));_LL2055: _tmp806=_tmp802.f1;goto _LL2053;_LL2053: _tmp804=
_tmp802.f2;goto _LL2051;_LL2051: {void*_tmp808=_tmp806;struct Cyc_List_List*
_tmp80E;struct Cyc_Dict_Dict*_tmp810;_LL2058: if(_tmp808 == (void*)Cyc_CfFlowInfo_BottomFL){
goto _LL2059;}else{goto _LL2060;}_LL2060: if((unsigned int)_tmp808 > 1?*((int*)
_tmp808)== Cyc_CfFlowInfo_ReachableFL: 0){_LL2065: _tmp810=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp808)->f1;goto _LL2063;_LL2063: _tmp80E=((struct Cyc_CfFlowInfo_ReachableFL_struct*)
_tmp808)->f2;goto _LL2061;}else{goto _LL2057;}_LL2059: return(void*)Cyc_CfFlowInfo_BottomFL;
_LL2061: _tmp810=Cyc_CfFlowInfo_assign_place(decl->loc,_tmp810,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp812=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));
_tmp812->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp813=
_cycalloc(sizeof(struct Cyc_CfFlowInfo_VarRoot_struct));_tmp813[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp814;_tmp814.tag=Cyc_CfFlowInfo_VarRoot;_tmp814.f1=_tmp7F5;_tmp814;});_tmp813;}));
_tmp812->fields=0;_tmp812;}),_tmp804);_tmp80E=Cyc_CfFlowInfo_reln_assign_var(
_tmp80E,_tmp7F5,(struct Cyc_Absyn_Exp*)_check_null(_tmp7F5->initializer));{struct
Cyc_CfFlowInfo_ReachableFL_struct*_tmp815=({struct Cyc_CfFlowInfo_ReachableFL_struct*
_tmp816=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));_tmp816[0]=({
struct Cyc_CfFlowInfo_ReachableFL_struct _tmp817;_tmp817.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp817.f1=_tmp810;_tmp817.f2=_tmp80E;_tmp817;});_tmp816;});Cyc_NewControlFlow_update_tryflow(
env,(void*)_tmp815);return(void*)_tmp815;}_LL2057:;}}}}_LL2032: {void*_tmp81B;
void*_tmp81D;struct _tuple0 _tmp819=Cyc_NewControlFlow_anal_Rexp(env,inflow,
_tmp7F7);_LL2078: _tmp81D=_tmp819.f1;goto _LL2076;_LL2076: _tmp81B=_tmp819.f2;goto
_LL2074;_LL2074: _tmp81D=Cyc_NewControlFlow_use_Rval(env,_tmp7F7->loc,_tmp81D,
_tmp81B);return Cyc_NewControlFlow_add_vars(_tmp81D,_tmp7FC,Cyc_CfFlowInfo_unknown_all);}
_LL2034: return Cyc_NewControlFlow_add_vars(inflow,_tmp7FE,Cyc_CfFlowInfo_unknown_none);
_LL2036:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp81F=_cycalloc(
sizeof(struct Cyc_Core_Impossible_struct));_tmp81F[0]=({struct Cyc_Core_Impossible_struct
_tmp820;_tmp820.tag=Cyc_Core_Impossible;_tmp820.f1=_tag_arr("anal_decl: unexpected decl variant",
sizeof(unsigned char),35);_tmp820;});_tmp81F;}));_LL2028:;}static void Cyc_NewControlFlow_postpass(
struct Cyc_Absyn_Stmt*s){(void*)(s->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot));{
void*_tmp821=(void*)s->r;struct Cyc_Absyn_Stmt*_tmp839;struct _tuple3 _tmp83B;
struct Cyc_Absyn_Stmt*_tmp83D;struct _tuple3 _tmp83F;struct Cyc_Absyn_Stmt*_tmp841;
struct Cyc_Absyn_Stmt*_tmp843;struct Cyc_Absyn_Stmt*_tmp845;struct Cyc_Absyn_Stmt*
_tmp847;struct _tuple3 _tmp849;struct Cyc_Absyn_Stmt*_tmp84B;struct _tuple3 _tmp84D;
struct Cyc_Absyn_Stmt*_tmp84F;struct Cyc_Absyn_Stmt*_tmp851;struct Cyc_Absyn_Stmt*
_tmp853;struct Cyc_Absyn_Stmt*_tmp855;struct Cyc_Absyn_Stmt*_tmp857;struct Cyc_Absyn_Stmt*
_tmp859;struct Cyc_Absyn_Stmt*_tmp85B;struct Cyc_List_List*_tmp85D;struct Cyc_Absyn_Stmt*
_tmp85F;struct Cyc_List_List*_tmp861;_LL2083: if((unsigned int)_tmp821 > 1?*((int*)
_tmp821)== Cyc_Absyn_For_s: 0){_LL2112: _tmp83F=((struct Cyc_Absyn_For_s_struct*)
_tmp821)->f2;_LL2114: _tmp841=_tmp83F.f2;goto _LL2108;_LL2108: _tmp83B=((struct Cyc_Absyn_For_s_struct*)
_tmp821)->f3;_LL2110: _tmp83D=_tmp83B.f2;goto _LL2106;_LL2106: _tmp839=((struct Cyc_Absyn_For_s_struct*)
_tmp821)->f4;goto _LL2084;}else{goto _LL2085;}_LL2085: if((unsigned int)_tmp821 > 1?*((
int*)_tmp821)== Cyc_Absyn_Seq_s: 0){_LL2118: _tmp845=((struct Cyc_Absyn_Seq_s_struct*)
_tmp821)->f1;goto _LL2116;_LL2116: _tmp843=((struct Cyc_Absyn_Seq_s_struct*)_tmp821)->f2;
goto _LL2086;}else{goto _LL2087;}_LL2087: if((unsigned int)_tmp821 > 1?*((int*)
_tmp821)== Cyc_Absyn_While_s: 0){_LL2122: _tmp849=((struct Cyc_Absyn_While_s_struct*)
_tmp821)->f1;_LL2124: _tmp84B=_tmp849.f2;goto _LL2120;_LL2120: _tmp847=((struct Cyc_Absyn_While_s_struct*)
_tmp821)->f2;goto _LL2088;}else{goto _LL2089;}_LL2089: if((unsigned int)_tmp821 > 1?*((
int*)_tmp821)== Cyc_Absyn_Do_s: 0){_LL2130: _tmp851=((struct Cyc_Absyn_Do_s_struct*)
_tmp821)->f1;goto _LL2126;_LL2126: _tmp84D=((struct Cyc_Absyn_Do_s_struct*)_tmp821)->f2;
_LL2128: _tmp84F=_tmp84D.f2;goto _LL2090;}else{goto _LL2091;}_LL2091: if((
unsigned int)_tmp821 > 1?*((int*)_tmp821)== Cyc_Absyn_IfThenElse_s: 0){_LL2134:
_tmp855=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp821)->f2;goto _LL2132;_LL2132:
_tmp853=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp821)->f3;goto _LL2092;}else{
goto _LL2093;}_LL2093: if((unsigned int)_tmp821 > 1?*((int*)_tmp821)== Cyc_Absyn_Region_s:
0){_LL2136: _tmp857=((struct Cyc_Absyn_Region_s_struct*)_tmp821)->f3;goto _LL2094;}
else{goto _LL2095;}_LL2095: if((unsigned int)_tmp821 > 1?*((int*)_tmp821)== Cyc_Absyn_Label_s:
0){_LL2138: _tmp859=((struct Cyc_Absyn_Label_s_struct*)_tmp821)->f2;goto _LL2096;}
else{goto _LL2097;}_LL2097: if((unsigned int)_tmp821 > 1?*((int*)_tmp821)== Cyc_Absyn_Decl_s:
0){_LL2140: _tmp85B=((struct Cyc_Absyn_Decl_s_struct*)_tmp821)->f2;goto _LL2098;}
else{goto _LL2099;}_LL2099: if((unsigned int)_tmp821 > 1?*((int*)_tmp821)== Cyc_Absyn_TryCatch_s:
0){_LL2144: _tmp85F=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp821)->f1;goto _LL2142;
_LL2142: _tmp85D=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp821)->f2;goto _LL2100;}
else{goto _LL2101;}_LL2101: if((unsigned int)_tmp821 > 1?*((int*)_tmp821)== Cyc_Absyn_Switch_s:
0){_LL2146: _tmp861=((struct Cyc_Absyn_Switch_s_struct*)_tmp821)->f2;goto _LL2102;}
else{goto _LL2103;}_LL2103: goto _LL2104;_LL2084: Cyc_NewControlFlow_postpass(
_tmp83D);_tmp845=_tmp839;_tmp843=_tmp841;goto _LL2086;_LL2086: _tmp84B=_tmp845;
_tmp847=_tmp843;goto _LL2088;_LL2088: _tmp851=_tmp847;_tmp84F=_tmp84B;goto _LL2090;
_LL2090: _tmp855=_tmp851;_tmp853=_tmp84F;goto _LL2092;_LL2092: Cyc_NewControlFlow_postpass(
_tmp855);_tmp857=_tmp853;goto _LL2094;_LL2094: _tmp859=_tmp857;goto _LL2096;_LL2096:
_tmp85B=_tmp859;goto _LL2098;_LL2098: Cyc_NewControlFlow_postpass(_tmp85B);return;
_LL2100: Cyc_NewControlFlow_postpass(_tmp85F);_tmp861=_tmp85D;goto _LL2102;_LL2102:
for(0;_tmp861 != 0;_tmp861=_tmp861->tl){Cyc_NewControlFlow_postpass(((struct Cyc_Absyn_Switch_clause*)
_tmp861->hd)->body);}return;_LL2104: return;_LL2082:;}}static void Cyc_NewControlFlow_check_fun(
struct Cyc_Absyn_Fndecl*fd){void*inflow=Cyc_NewControlFlow_add_vars((void*)({
struct Cyc_CfFlowInfo_ReachableFL_struct*_tmp879=_cycalloc(sizeof(struct Cyc_CfFlowInfo_ReachableFL_struct));
_tmp879[0]=({struct Cyc_CfFlowInfo_ReachableFL_struct _tmp87A;_tmp87A.tag=Cyc_CfFlowInfo_ReachableFL;
_tmp87A.f1=Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_tmp87A.f2=0;_tmp87A;});
_tmp879;}),(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,
Cyc_CfFlowInfo_unknown_all);int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(
fd));struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp878=_cycalloc(sizeof(struct Cyc_NewControlFlow_AnalEnv));_tmp878->iterate_again=
1;_tmp878->iteration_num=0;_tmp878->in_try=0;_tmp878->tryflow=(void*)inflow;
_tmp878->all_changed=0;_tmp878->noreturn=noreturn;_tmp878;});void*outflow=inflow;
while(env->iterate_again){++ env->iteration_num;env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,fd->body);}{void*_tmp863=outflow;_LL2149: if(_tmp863 == (void*)Cyc_CfFlowInfo_BottomFL){
goto _LL2150;}else{goto _LL2151;}_LL2151: goto _LL2152;_LL2150: goto _LL2148;_LL2152:
if(noreturn){({void*_tmp869[0]={};Cyc_Tcutil_terr((fd->body)->loc,_tag_arr("`noreturn' function might (implicitly) return",
sizeof(unsigned char),46),_tag_arr(_tmp869,sizeof(void*),0));});}else{{void*
_tmp86A=Cyc_Tcutil_compress((void*)fd->ret_type);_LL2156: if(_tmp86A == (void*)Cyc_Absyn_VoidType){
goto _LL2157;}else{goto _LL2158;}_LL2158: if((unsigned int)_tmp86A > 3?*((int*)
_tmp86A)== Cyc_Absyn_DoubleType: 0){goto _LL2159;}else{goto _LL2160;}_LL2160: if(
_tmp86A == (void*)Cyc_Absyn_FloatType){goto _LL2161;}else{goto _LL2162;}_LL2162: if((
unsigned int)_tmp86A > 3?*((int*)_tmp86A)== Cyc_Absyn_IntType: 0){goto _LL2163;}
else{goto _LL2164;}_LL2164: goto _LL2165;_LL2157: goto _LL2155;_LL2159: goto _LL2161;
_LL2161: goto _LL2163;_LL2163:({void*_tmp876[0]={};Cyc_Tcutil_warn((fd->body)->loc,
_tag_arr("function may not return a value",sizeof(unsigned char),32),_tag_arr(
_tmp876,sizeof(void*),0));});goto _LL2155;_LL2165:({void*_tmp877[0]={};Cyc_Tcutil_terr((
fd->body)->loc,_tag_arr("function may not return a value",sizeof(unsigned char),
32),_tag_arr(_tmp877,sizeof(void*),0));});goto _LL2155;_LL2155:;}goto _LL2148;}
_LL2148:;}Cyc_NewControlFlow_postpass(fd->body);}void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*_tmp87B=(void*)((struct Cyc_Absyn_Decl*)
ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp887;struct Cyc_List_List*_tmp889;struct Cyc_List_List*
_tmp88B;struct Cyc_List_List*_tmp88D;_LL2173: if(*((int*)_tmp87B)== Cyc_Absyn_Fn_d){
_LL2184: _tmp887=((struct Cyc_Absyn_Fn_d_struct*)_tmp87B)->f1;goto _LL2174;}else{
goto _LL2175;}_LL2175: if(*((int*)_tmp87B)== Cyc_Absyn_ExternC_d){_LL2186: _tmp889=((
struct Cyc_Absyn_ExternC_d_struct*)_tmp87B)->f1;goto _LL2176;}else{goto _LL2177;}
_LL2177: if(*((int*)_tmp87B)== Cyc_Absyn_Using_d){_LL2188: _tmp88B=((struct Cyc_Absyn_Using_d_struct*)
_tmp87B)->f2;goto _LL2178;}else{goto _LL2179;}_LL2179: if(*((int*)_tmp87B)== Cyc_Absyn_Namespace_d){
_LL2190: _tmp88D=((struct Cyc_Absyn_Namespace_d_struct*)_tmp87B)->f2;goto _LL2180;}
else{goto _LL2181;}_LL2181: goto _LL2182;_LL2174: Cyc_NewControlFlow_check_fun(
_tmp887);goto _LL2172;_LL2176: _tmp88B=_tmp889;goto _LL2178;_LL2178: _tmp88D=_tmp88B;
goto _LL2180;_LL2180: Cyc_NewControlFlow_cf_check(_tmp88D);goto _LL2172;_LL2182:
goto _LL2172;_LL2172:;}}
