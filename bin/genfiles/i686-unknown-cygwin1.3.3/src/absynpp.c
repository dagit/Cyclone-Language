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
struct _tagged_arr Cyc_Core_new_string(unsigned int);extern unsigned char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;
};extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern unsigned char Cyc_Std_FileCloseError[
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
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern unsigned char Cyc_List_List_mismatch[18];extern void Cyc_List_iter(void(*f)(
void*),struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_imp_rev(struct
Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern unsigned char Cyc_List_Nth[8];extern int Cyc_List_exists(
int(*pred)(void*),struct Cyc_List_List*x);extern int Cyc_List_list_cmp(int(*cmp)(
void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);extern int Cyc_List_list_prefix(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;extern void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*
d,int w,struct Cyc_Std___sFILE*f);extern struct _tagged_arr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc*d,int w);extern struct Cyc_PP_Doc*Cyc_PP_nil_doc();extern struct Cyc_PP_Doc*
Cyc_PP_line_doc();extern struct Cyc_PP_Doc*Cyc_PP_text(struct _tagged_arr s);extern
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _tagged_arr*p);extern struct Cyc_PP_Doc*Cyc_PP_nest(
int k,struct Cyc_PP_Doc*d);extern struct Cyc_PP_Doc*Cyc_PP_cat(struct _tagged_arr);
extern struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct
_tagged_arr sep,struct Cyc_List_List*l);extern struct Cyc_PP_Doc*Cyc_PP_seql(struct
_tagged_arr sep,struct Cyc_List_List*l0);extern struct Cyc_PP_Doc*Cyc_PP_ppseql(
struct Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*l);extern
struct Cyc_PP_Doc*Cyc_PP_group(struct _tagged_arr start,struct _tagged_arr stop,
struct _tagged_arr sep,struct Cyc_List_List*l);extern struct Cyc_PP_Doc*Cyc_PP_egroup(
struct _tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,struct Cyc_List_List*
l);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
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
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();extern struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_compress_kb(void*);extern struct
Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);extern struct _tagged_arr Cyc_Absyn_attribute2string(
void*);struct Cyc_Buffer_t;extern int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct
_tagged_arr*s2);extern struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,
struct _tagged_arr);extern unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
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
ae;struct Cyc_Core_Opt*le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*
Cyc_Tcutil_compress(void*t);extern int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc_Std___sFILE*f);extern struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d);extern struct _tagged_arr Cyc_Absynpp_typ2string(void*);
extern struct _tagged_arr Cyc_Absynpp_typ2cstring(void*);extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*);extern struct _tagged_arr Cyc_Absynpp_kindbound2string(void*);extern struct
_tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);extern struct _tagged_arr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*);extern struct _tagged_arr Cyc_Absynpp_decllist2string(struct Cyc_List_List*
tdl);extern struct _tagged_arr Cyc_Absynpp_prim2string(void*p);extern struct
_tagged_arr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);extern struct _tagged_arr
Cyc_Absynpp_scope2string(void*sc);extern int Cyc_Absynpp_is_anon_structtype(void*t);
extern struct _tagged_arr Cyc_Absynpp_cyc_string;extern struct _tagged_arr*Cyc_Absynpp_cyc_stringptr;
extern int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);extern struct _tagged_arr Cyc_Absynpp_char_escape(
unsigned char);extern struct _tagged_arr Cyc_Absynpp_string_escape(struct
_tagged_arr);extern struct _tagged_arr Cyc_Absynpp_prim2str(void*p);extern int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);struct _tuple3{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};extern struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple3*arg);
struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};extern
struct _tuple4 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);struct _tuple5{
struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};extern struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(
struct _tuple5*dp);extern struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*
cs);extern struct Cyc_PP_Doc*Cyc_Absynpp_switchCclauses2doc(struct Cyc_List_List*cs);
extern struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);extern struct Cyc_PP_Doc*Cyc_Absynpp_structfields2doc(
struct Cyc_List_List*fields);extern struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*);
extern struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*);extern struct
Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);extern struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);extern struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(
int inprec,struct Cyc_List_List*es);extern struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*);extern struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct
_tuple0*);extern struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*);extern struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc(void*);extern struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int
inprec,void*p,struct Cyc_List_List*es);struct _tuple6{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};extern struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple6*
de);extern struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,
struct Cyc_Core_Opt*dopt);extern struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct
Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po);extern struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields);extern struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs);extern struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd);static int Cyc_Absynpp_expand_typedefs;static int Cyc_Absynpp_qvar_to_Cids;
static unsigned char _tmp0[4]="Cyc";struct _tagged_arr Cyc_Absynpp_cyc_string={_tmp0,
_tmp0,_tmp0 + 4};struct _tagged_arr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
static int Cyc_Absynpp_add_cyc_prefix;static int Cyc_Absynpp_to_VC;static int Cyc_Absynpp_decls_first;
static int Cyc_Absynpp_rewrite_temp_tvars;static int Cyc_Absynpp_print_all_tvars;
static int Cyc_Absynpp_print_all_kinds;static int Cyc_Absynpp_print_using_stmts;
static int Cyc_Absynpp_print_externC_stmts;static int Cyc_Absynpp_print_full_evars;
static int Cyc_Absynpp_generate_line_directives;static int Cyc_Absynpp_use_curr_namespace;
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;struct Cyc_Absynpp_Params;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){Cyc_Absynpp_expand_typedefs=
fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
fs->add_cyc_prefix;Cyc_Absynpp_to_VC=fs->to_VC;Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=
fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;Cyc_Absynpp_print_using_stmts=
fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_generate_line_directives=
fs->generate_line_directives;Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;
Cyc_Absynpp_curr_namespace=fs->curr_namespace;}struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={
0,0,0,0,0,1,0,0,1,1,0,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={
1,0,0,0,0,1,0,0,1,1,0,0,1,0};struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,
1,0,1,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,
0,0,0,0,0,1,1,0,0,0,0};static void Cyc_Absynpp_curr_namespace_add(struct
_tagged_arr*v){Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,({
struct Cyc_List_List*_tmp1=_cycalloc(sizeof(struct Cyc_List_List));_tmp1->hd=v;
_tmp1->tl=0;_tmp1;}));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if(((struct Cyc_List_List*)_check_null(*l))->tl == 0){*l=0;}else{Cyc_Absynpp_suppr_last(&((
struct Cyc_List_List*)_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _tagged_arr Cyc_Absynpp_char_escape(unsigned char c){switch(c){case '\a': _LL2:
return _tag_arr("\\a",sizeof(unsigned char),3);case '\b': _LL3: return _tag_arr("\\b",
sizeof(unsigned char),3);case '\f': _LL4: return _tag_arr("\\f",sizeof(unsigned char),
3);case '\n': _LL5: return _tag_arr("\\n",sizeof(unsigned char),3);case '\r': _LL6:
return _tag_arr("\\r",sizeof(unsigned char),3);case '\t': _LL7: return _tag_arr("\\t",
sizeof(unsigned char),3);case '\v': _LL8: return _tag_arr("\\v",sizeof(unsigned char),
3);case '\\': _LL9: return _tag_arr("\\\\",sizeof(unsigned char),3);case '"': _LL10:
return _tag_arr("\"",sizeof(unsigned char),2);case '\'': _LL11: return _tag_arr("\\'",
sizeof(unsigned char),3);default: _LL12: if(c >= ' '? c <= '~': 0){struct _tagged_arr
_tmpE=Cyc_Core_new_string(1);*((unsigned char*)_check_unknown_subscript(_tmpE,
sizeof(unsigned char),0))=c;return(struct _tagged_arr)_tmpE;}else{struct
_tagged_arr _tmpF=Cyc_Core_new_string(4);int j=0;*((unsigned char*)
_check_unknown_subscript(_tmpF,sizeof(unsigned char),j ++))='\\';*((unsigned char*)
_check_unknown_subscript(_tmpF,sizeof(unsigned char),j ++))=(unsigned char)('0' + ((
unsigned char)c >> 6 & 7));*((unsigned char*)_check_unknown_subscript(_tmpF,
sizeof(unsigned char),j ++))=(unsigned char)('0' + (c >> 3 & 7));*((unsigned char*)
_check_unknown_subscript(_tmpF,sizeof(unsigned char),j ++))=(unsigned char)('0' + (
c & 7));return(struct _tagged_arr)_tmpF;}}}static int Cyc_Absynpp_special(struct
_tagged_arr s){int sz=(int)(_get_arr_size(s,sizeof(unsigned char))- 1);{int i=0;
for(0;i < sz;i ++){unsigned char c=((const unsigned char*)s.curr)[i];if(((c <= ' '? 1:
c >= '~')? 1: c == '"')? 1: c == '\\'){return 1;}}}return 0;}struct _tagged_arr Cyc_Absynpp_string_escape(
struct _tagged_arr s){if(! Cyc_Absynpp_special(s)){return s;}{int n=(int)(
_get_arr_size(s,sizeof(unsigned char))- 1);if(n > 0?((const unsigned char*)s.curr)[
n]== '\000': 0){n --;}{int len=0;{int i=0;for(0;i <= n;i ++){unsigned char _tmp10=((
const unsigned char*)s.curr)[i];_LL18: if(_tmp10 == '\a'){goto _LL19;}else{goto _LL20;}
_LL20: if(_tmp10 == '\b'){goto _LL21;}else{goto _LL22;}_LL22: if(_tmp10 == '\f'){goto
_LL23;}else{goto _LL24;}_LL24: if(_tmp10 == '\n'){goto _LL25;}else{goto _LL26;}_LL26:
if(_tmp10 == '\r'){goto _LL27;}else{goto _LL28;}_LL28: if(_tmp10 == '\t'){goto _LL29;}
else{goto _LL30;}_LL30: if(_tmp10 == '\v'){goto _LL31;}else{goto _LL32;}_LL32: if(
_tmp10 == '\\'){goto _LL33;}else{goto _LL34;}_LL34: if(_tmp10 == '"'){goto _LL35;}
else{goto _LL36;}_LL36: goto _LL37;_LL19: goto _LL21;_LL21: goto _LL23;_LL23: goto _LL25;
_LL25: goto _LL27;_LL27: goto _LL29;_LL29: goto _LL31;_LL31: goto _LL33;_LL33: goto _LL35;
_LL35: len +=2;goto _LL17;_LL37: if(_tmp10 >= ' '? _tmp10 <= '~': 0){len ++;}else{len +=4;}
goto _LL17;_LL17:;}}{struct _tagged_arr t=Cyc_Core_new_string((unsigned int)len);
int j=0;{int i=0;for(0;i <= n;i ++){unsigned char _tmp26=((const unsigned char*)s.curr)[
i];_LL40: if(_tmp26 == '\a'){goto _LL41;}else{goto _LL42;}_LL42: if(_tmp26 == '\b'){
goto _LL43;}else{goto _LL44;}_LL44: if(_tmp26 == '\f'){goto _LL45;}else{goto _LL46;}
_LL46: if(_tmp26 == '\n'){goto _LL47;}else{goto _LL48;}_LL48: if(_tmp26 == '\r'){goto
_LL49;}else{goto _LL50;}_LL50: if(_tmp26 == '\t'){goto _LL51;}else{goto _LL52;}_LL52:
if(_tmp26 == '\v'){goto _LL53;}else{goto _LL54;}_LL54: if(_tmp26 == '\\'){goto _LL55;}
else{goto _LL56;}_LL56: if(_tmp26 == '"'){goto _LL57;}else{goto _LL58;}_LL58: goto
_LL59;_LL41:*((unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j
++))='\\';*((unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='a';
goto _LL39;_LL43:*((unsigned char*)_check_unknown_subscript(t,sizeof(
unsigned char),j ++))='\\';*((unsigned char*)_check_unknown_subscript(t,sizeof(
unsigned char),j ++))='b';goto _LL39;_LL45:*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='f';goto _LL39;_LL47:*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='n';goto
_LL39;_LL49:*((unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j
++))='\\';*((unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='r';
goto _LL39;_LL51:*((unsigned char*)_check_unknown_subscript(t,sizeof(
unsigned char),j ++))='\\';*((unsigned char*)_check_unknown_subscript(t,sizeof(
unsigned char),j ++))='t';goto _LL39;_LL53:*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='v';goto _LL39;_LL55:*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';goto
_LL39;_LL57:*((unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j
++))='\\';*((unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='"';
goto _LL39;_LL59: if(_tmp26 >= ' '? _tmp26 <= '~': 0){*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))=_tmp26;}else{*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))=(
unsigned char)('0' + (_tmp26 >> 6 & 7));*((unsigned char*)_check_unknown_subscript(
t,sizeof(unsigned char),j ++))=(unsigned char)('0' + (_tmp26 >> 3 & 7));*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))=(
unsigned char)('0' + (_tmp26 & 7));}goto _LL39;_LL39:;}}return(struct _tagged_arr)t;}}}}
static unsigned char _tmp3C[9]="restrict";static struct _tagged_arr Cyc_Absynpp_restrict_string={
_tmp3C,_tmp3C,_tmp3C + 9};static unsigned char _tmp3D[9]="volatile";static struct
_tagged_arr Cyc_Absynpp_volatile_string={_tmp3D,_tmp3D,_tmp3D + 9};static
unsigned char _tmp3E[6]="const";static struct _tagged_arr Cyc_Absynpp_const_str={
_tmp3E,_tmp3E,_tmp3E + 6};static struct _tagged_arr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _tagged_arr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _tagged_arr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict){
l=({struct Cyc_List_List*_tmp3F=_cycalloc(sizeof(struct Cyc_List_List));_tmp3F->hd=
Cyc_Absynpp_restrict_sp;_tmp3F->tl=l;_tmp3F;});}if(tq.q_volatile){l=({struct Cyc_List_List*
_tmp40=_cycalloc(sizeof(struct Cyc_List_List));_tmp40->hd=Cyc_Absynpp_volatile_sp;
_tmp40->tl=l;_tmp40;});}if(tq.q_const){l=({struct Cyc_List_List*_tmp41=_cycalloc(
sizeof(struct Cyc_List_List));_tmp41->hd=Cyc_Absynpp_const_sp;_tmp41->tl=l;_tmp41;});}
return Cyc_PP_egroup(_tag_arr("",sizeof(unsigned char),1),_tag_arr(" ",sizeof(
unsigned char),2),_tag_arr(" ",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,l));}struct _tagged_arr Cyc_Absynpp_kind2string(void*k){void*_tmp42=
k;_LL68: if(_tmp42 == (void*)Cyc_Absyn_AnyKind){goto _LL69;}else{goto _LL70;}_LL70:
if(_tmp42 == (void*)Cyc_Absyn_MemKind){goto _LL71;}else{goto _LL72;}_LL72: if(_tmp42
== (void*)Cyc_Absyn_BoxKind){goto _LL73;}else{goto _LL74;}_LL74: if(_tmp42 == (void*)
Cyc_Absyn_RgnKind){goto _LL75;}else{goto _LL76;}_LL76: if(_tmp42 == (void*)Cyc_Absyn_EffKind){
goto _LL77;}else{goto _LL67;}_LL69: return _tag_arr("A",sizeof(unsigned char),2);
_LL71: return _tag_arr("M",sizeof(unsigned char),2);_LL73: return _tag_arr("B",
sizeof(unsigned char),2);_LL75: return _tag_arr("R",sizeof(unsigned char),2);_LL77:
return _tag_arr("E",sizeof(unsigned char),2);_LL67:;}struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*c){void*_tmp4E=Cyc_Absyn_compress_kb(c);void*_tmp56;void*_tmp58;_LL80: if(*((
int*)_tmp4E)== Cyc_Absyn_Eq_kb){_LL87: _tmp56=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp4E)->f1;goto _LL81;}else{goto _LL82;}_LL82: if(*((int*)_tmp4E)== Cyc_Absyn_Unknown_kb){
goto _LL83;}else{goto _LL84;}_LL84: if(*((int*)_tmp4E)== Cyc_Absyn_Less_kb){_LL89:
_tmp58=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4E)->f2;goto _LL85;}else{goto
_LL79;}_LL81: return Cyc_Absynpp_kind2string(_tmp56);_LL83: return _tag_arr("?",
sizeof(unsigned char),2);_LL85: return Cyc_Absynpp_kind2string(_tmp58);_LL79:;}
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(void*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(
k));}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){return Cyc_PP_text(Cyc_Absynpp_kindbound2string(
c));}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){return Cyc_PP_egroup(
_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}struct
Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*_tmp5A=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmp64;void*_tmp66;void*_tmp68;_LL92: if(*((int*)_tmp5A)== 
Cyc_Absyn_Unknown_kb){goto _LL93;}else{goto _LL94;}_LL94: if(*((int*)_tmp5A)== Cyc_Absyn_Eq_kb){
_LL101: _tmp64=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5A)->f1;if(_tmp64 == (
void*)Cyc_Absyn_BoxKind){goto _LL95;}else{goto _LL96;}}else{goto _LL96;}_LL96: if(*((
int*)_tmp5A)== Cyc_Absyn_Less_kb){_LL103: _tmp66=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp5A)->f2;goto _LL97;}else{goto _LL98;}_LL98: if(*((int*)_tmp5A)== Cyc_Absyn_Eq_kb){
_LL105: _tmp68=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp5A)->f1;goto _LL99;}
else{goto _LL91;}_LL93: goto _LL95;_LL95: return Cyc_PP_textptr(tv->name);_LL97:
_tmp68=_tmp66;goto _LL99;_LL99: return({struct Cyc_PP_Doc*_tmp6A[3];_tmp6A[2]=Cyc_Absynpp_kind2doc(
_tmp68);_tmp6A[1]=Cyc_PP_text(_tag_arr("::",sizeof(unsigned char),3));_tmp6A[0]=
Cyc_PP_textptr(tv->name);Cyc_PP_cat(_tag_arr(_tmp6A,sizeof(struct Cyc_PP_Doc*),3));});
_LL91:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){return
Cyc_PP_egroup(_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",sizeof(
unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_ktvar2doc,tvs));}static struct _tagged_arr*Cyc_Absynpp_get_name(struct
Cyc_Absyn_Tvar*tv){return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct
Cyc_List_List*tvs){if(Cyc_Absynpp_print_all_kinds){return Cyc_Absynpp_ktvars2doc(
tvs);}return Cyc_PP_egroup(_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));}struct _tuple7{
struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct
_tuple7*t){return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(
struct Cyc_List_List*ts){return Cyc_PP_group(_tag_arr("(",sizeof(unsigned char),2),
_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(
void*att){void*_tmp6B=att;_LL109: if(_tmp6B == (void*)Cyc_Absyn_Stdcall_att){goto
_LL110;}else{goto _LL111;}_LL111: if(_tmp6B == (void*)Cyc_Absyn_Cdecl_att){goto
_LL112;}else{goto _LL113;}_LL113: if(_tmp6B == (void*)Cyc_Absyn_Fastcall_att){goto
_LL114;}else{goto _LL115;}_LL115: goto _LL116;_LL110: return Cyc_PP_nil_doc();_LL112:
return Cyc_PP_nil_doc();_LL114: return Cyc_PP_nil_doc();_LL116: return Cyc_PP_text(
Cyc_Absyn_attribute2string(att));_LL108:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(
struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmp75=(void*)atts->hd;
_LL119: if(_tmp75 == (void*)Cyc_Absyn_Stdcall_att){goto _LL120;}else{goto _LL121;}
_LL121: if(_tmp75 == (void*)Cyc_Absyn_Cdecl_att){goto _LL122;}else{goto _LL123;}
_LL123: if(_tmp75 == (void*)Cyc_Absyn_Fastcall_att){goto _LL124;}else{goto _LL125;}
_LL125: goto _LL126;_LL120: return Cyc_PP_text(_tag_arr(" _stdcall ",sizeof(
unsigned char),11));_LL122: return Cyc_PP_text(_tag_arr(" _cdecl ",sizeof(
unsigned char),9));_LL124: return Cyc_PP_text(_tag_arr(" _fastcall ",sizeof(
unsigned char),12));_LL126: goto _LL118;_LL118:;}return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*
Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*
atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmp7F=(void*)atts2->hd;_LL129:
if(_tmp7F == (void*)Cyc_Absyn_Stdcall_att){goto _LL130;}else{goto _LL131;}_LL131:
if(_tmp7F == (void*)Cyc_Absyn_Cdecl_att){goto _LL132;}else{goto _LL133;}_LL133: if(
_tmp7F == (void*)Cyc_Absyn_Fastcall_att){goto _LL134;}else{goto _LL135;}_LL135: goto
_LL136;_LL130: goto _LL128;_LL132: goto _LL128;_LL134: goto _LL128;_LL136: hasatt=1;
goto _LL128;_LL128:;}}if(! hasatt){return Cyc_PP_nil_doc();}return({struct Cyc_PP_Doc*
_tmp89[3];_tmp89[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp89[1]=
Cyc_PP_group(_tag_arr("",sizeof(unsigned char),1),_tag_arr("",sizeof(
unsigned char),1),_tag_arr(" ",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,
atts));_tmp89[0]=Cyc_PP_text(_tag_arr("__declspec(",sizeof(unsigned char),12));
Cyc_PP_cat(_tag_arr(_tmp89,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*
Cyc_Absynpp_att2doc(void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){if(atts == 0){
return Cyc_PP_nil_doc();}if(Cyc_Absynpp_to_VC){return Cyc_Absynpp_noncallconv2doc(
atts);}return({struct Cyc_PP_Doc*_tmp8A[2];_tmp8A[1]=Cyc_PP_group(_tag_arr("((",
sizeof(unsigned char),3),_tag_arr("))",sizeof(unsigned char),3),_tag_arr(",",
sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));_tmp8A[0]=Cyc_PP_text(
_tag_arr(" __attribute__",sizeof(unsigned char),15));Cyc_PP_cat(_tag_arr(_tmp8A,
sizeof(struct Cyc_PP_Doc*),2));});}int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*
tms){if(tms == 0){return 0;}{void*_tmp8B=(void*)tms->hd;_LL141: if((unsigned int)
_tmp8B > 1?*((int*)_tmp8B)== Cyc_Absyn_Pointer_mod: 0){goto _LL142;}else{goto _LL143;}
_LL143: if((unsigned int)_tmp8B > 1?*((int*)_tmp8B)== Cyc_Absyn_Attributes_mod: 0){
goto _LL144;}else{goto _LL145;}_LL145: goto _LL146;_LL142: return 1;_LL144: if(! Cyc_Absynpp_to_VC){
return 0;}return Cyc_Absynpp_next_is_pointer(tms->tl);_LL146: return 0;_LL140:;}}
extern struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(
struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0){return d;}{struct Cyc_PP_Doc*
rest=Cyc_Absynpp_dtms2doc(d,tms->tl);struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*
_tmpE5[3];_tmpE5[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmpE5[1]=
rest;_tmpE5[0]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmpE5,sizeof(struct Cyc_PP_Doc*),3));});void*_tmp93=(void*)tms->hd;
struct Cyc_Absyn_Exp*_tmpA1;void*_tmpA3;struct Cyc_List_List*_tmpA5;int _tmpA7;
struct Cyc_Position_Segment*_tmpA9;struct Cyc_List_List*_tmpAB;struct Cyc_Absyn_Tqual
_tmpAD;void*_tmpAF;void*_tmpB1;_LL149: if(_tmp93 == (void*)Cyc_Absyn_Carray_mod){
goto _LL150;}else{goto _LL151;}_LL151: if((unsigned int)_tmp93 > 1?*((int*)_tmp93)== 
Cyc_Absyn_ConstArray_mod: 0){_LL162: _tmpA1=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp93)->f1;goto _LL152;}else{goto _LL153;}_LL153: if((unsigned int)_tmp93 > 1?*((
int*)_tmp93)== Cyc_Absyn_Function_mod: 0){_LL164: _tmpA3=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp93)->f1;goto _LL154;}else{goto _LL155;}_LL155: if((unsigned int)_tmp93 > 1?*((
int*)_tmp93)== Cyc_Absyn_Attributes_mod: 0){_LL166: _tmpA5=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmp93)->f2;goto _LL156;}else{goto _LL157;}_LL157: if((unsigned int)_tmp93 > 1?*((
int*)_tmp93)== Cyc_Absyn_TypeParams_mod: 0){_LL172: _tmpAB=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp93)->f1;goto _LL170;_LL170: _tmpA9=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp93)->f2;goto _LL168;_LL168: _tmpA7=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp93)->f3;goto _LL158;}else{goto _LL159;}_LL159: if((unsigned int)_tmp93 > 1?*((
int*)_tmp93)== Cyc_Absyn_Pointer_mod: 0){_LL178: _tmpB1=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp93)->f1;goto _LL176;_LL176: _tmpAF=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp93)->f2;goto _LL174;_LL174: _tmpAD=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp93)->f3;goto _LL160;}else{goto _LL148;}_LL150: if(Cyc_Absynpp_next_is_pointer(
tms->tl)){rest=p_rest;}return({struct Cyc_PP_Doc*_tmpB3[2];_tmpB3[1]=Cyc_PP_text(
_tag_arr("[]",sizeof(unsigned char),3));_tmpB3[0]=rest;Cyc_PP_cat(_tag_arr(
_tmpB3,sizeof(struct Cyc_PP_Doc*),2));});_LL152: if(Cyc_Absynpp_next_is_pointer(
tms->tl)){rest=p_rest;}return({struct Cyc_PP_Doc*_tmpB4[4];_tmpB4[3]=Cyc_PP_text(
_tag_arr("]",sizeof(unsigned char),2));_tmpB4[2]=Cyc_Absynpp_exp2doc(_tmpA1);
_tmpB4[1]=Cyc_PP_text(_tag_arr("[",sizeof(unsigned char),2));_tmpB4[0]=rest;Cyc_PP_cat(
_tag_arr(_tmpB4,sizeof(struct Cyc_PP_Doc*),4));});_LL154: if(Cyc_Absynpp_next_is_pointer(
tms->tl)){rest=p_rest;}{void*_tmpB5=_tmpA3;struct Cyc_List_List*_tmpBB;struct Cyc_Core_Opt*
_tmpBD;struct Cyc_Absyn_VarargInfo*_tmpBF;int _tmpC1;struct Cyc_List_List*_tmpC3;
struct Cyc_Position_Segment*_tmpC5;struct Cyc_List_List*_tmpC7;_LL183: if(*((int*)
_tmpB5)== Cyc_Absyn_WithTypes){_LL196: _tmpC3=((struct Cyc_Absyn_WithTypes_struct*)
_tmpB5)->f1;goto _LL194;_LL194: _tmpC1=((struct Cyc_Absyn_WithTypes_struct*)_tmpB5)->f2;
goto _LL192;_LL192: _tmpBF=((struct Cyc_Absyn_WithTypes_struct*)_tmpB5)->f3;goto
_LL190;_LL190: _tmpBD=((struct Cyc_Absyn_WithTypes_struct*)_tmpB5)->f4;goto _LL188;
_LL188: _tmpBB=((struct Cyc_Absyn_WithTypes_struct*)_tmpB5)->f5;goto _LL184;}else{
goto _LL185;}_LL185: if(*((int*)_tmpB5)== Cyc_Absyn_NoTypes){_LL200: _tmpC7=((
struct Cyc_Absyn_NoTypes_struct*)_tmpB5)->f1;goto _LL198;_LL198: _tmpC5=((struct Cyc_Absyn_NoTypes_struct*)
_tmpB5)->f2;goto _LL186;}else{goto _LL182;}_LL184: return({struct Cyc_PP_Doc*_tmpC9[
2];_tmpC9[1]=Cyc_Absynpp_funargs2doc(_tmpC3,_tmpC1,_tmpBF,_tmpBD,_tmpBB);_tmpC9[
0]=rest;Cyc_PP_cat(_tag_arr(_tmpC9,sizeof(struct Cyc_PP_Doc*),2));});_LL186:
return({struct Cyc_PP_Doc*_tmpCA[2];_tmpCA[1]=Cyc_PP_group(_tag_arr("(",sizeof(
unsigned char),2),_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmpC7));
_tmpCA[0]=rest;Cyc_PP_cat(_tag_arr(_tmpCA,sizeof(struct Cyc_PP_Doc*),2));});
_LL182:;}_LL156: if(! Cyc_Absynpp_to_VC){if(Cyc_Absynpp_next_is_pointer(tms->tl)){
rest=p_rest;}return({struct Cyc_PP_Doc*_tmpCB[2];_tmpCB[1]=Cyc_Absynpp_atts2doc(
_tmpA5);_tmpCB[0]=rest;Cyc_PP_cat(_tag_arr(_tmpCB,sizeof(struct Cyc_PP_Doc*),2));});}
else{if(Cyc_Absynpp_next_is_pointer(tms->tl)){return({struct Cyc_PP_Doc*_tmpCC[2];
_tmpCC[1]=rest;_tmpCC[0]=Cyc_Absynpp_callconv2doc(_tmpA5);Cyc_PP_cat(_tag_arr(
_tmpCC,sizeof(struct Cyc_PP_Doc*),2));});}return rest;}_LL158: if(Cyc_Absynpp_next_is_pointer(
tms->tl)){rest=p_rest;}if(_tmpA7){return({struct Cyc_PP_Doc*_tmpCD[2];_tmpCD[1]=
Cyc_Absynpp_ktvars2doc(_tmpAB);_tmpCD[0]=rest;Cyc_PP_cat(_tag_arr(_tmpCD,sizeof(
struct Cyc_PP_Doc*),2));});}else{return({struct Cyc_PP_Doc*_tmpCE[2];_tmpCE[1]=Cyc_Absynpp_tvars2doc(
_tmpAB);_tmpCE[0]=rest;Cyc_PP_cat(_tag_arr(_tmpCE,sizeof(struct Cyc_PP_Doc*),2));});}
_LL160: {struct Cyc_PP_Doc*ptr;{void*_tmpCF=_tmpB1;struct Cyc_Absyn_Exp*_tmpD7;
struct Cyc_Absyn_Exp*_tmpD9;_LL209: if((unsigned int)_tmpCF > 1?*((int*)_tmpCF)== 
Cyc_Absyn_Nullable_ps: 0){_LL216: _tmpD7=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmpCF)->f1;goto _LL210;}else{goto _LL211;}_LL211: if((unsigned int)_tmpCF > 1?*((
int*)_tmpCF)== Cyc_Absyn_NonNullable_ps: 0){_LL218: _tmpD9=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmpCF)->f1;goto _LL212;}else{goto _LL213;}_LL213: if(_tmpCF == (void*)Cyc_Absyn_TaggedArray_ps){
goto _LL214;}else{goto _LL208;}_LL210: if(Cyc_Evexp_eval_const_uint_exp(_tmpD7)== 1){
ptr=Cyc_PP_text(_tag_arr("*",sizeof(unsigned char),2));}else{ptr=({struct Cyc_PP_Doc*
_tmpDB[4];_tmpDB[3]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmpDB[2]=
Cyc_Absynpp_exp2doc(_tmpD7);_tmpDB[1]=Cyc_PP_text(_tag_arr("{",sizeof(
unsigned char),2));_tmpDB[0]=Cyc_PP_text(_tag_arr("*",sizeof(unsigned char),2));
Cyc_PP_cat(_tag_arr(_tmpDB,sizeof(struct Cyc_PP_Doc*),4));});}goto _LL208;_LL212:
if(Cyc_Evexp_eval_const_uint_exp(_tmpD9)== 1){ptr=Cyc_PP_text(_tag_arr("@",
sizeof(unsigned char),2));}else{ptr=({struct Cyc_PP_Doc*_tmpDC[4];_tmpDC[3]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmpDC[2]=Cyc_Absynpp_exp2doc(_tmpD9);
_tmpDC[1]=Cyc_PP_text(_tag_arr("{",sizeof(unsigned char),2));_tmpDC[0]=Cyc_PP_text(
_tag_arr("@",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmpDC,sizeof(struct
Cyc_PP_Doc*),4));});}goto _LL208;_LL214: ptr=Cyc_PP_text(_tag_arr("?",sizeof(
unsigned char),2));goto _LL208;_LL208:;}{void*_tmpDD=Cyc_Tcutil_compress(_tmpAF);
_LL223: if(_tmpDD == (void*)Cyc_Absyn_HeapRgn){goto _LL224;}else{goto _LL225;}_LL225:
goto _LL226;_LL224: return({struct Cyc_PP_Doc*_tmpE3[2];_tmpE3[1]=rest;_tmpE3[0]=
ptr;Cyc_PP_cat(_tag_arr(_tmpE3,sizeof(struct Cyc_PP_Doc*),2));});_LL226: return({
struct Cyc_PP_Doc*_tmpE4[4];_tmpE4[3]=rest;_tmpE4[2]=Cyc_PP_text(_tag_arr(" ",
sizeof(unsigned char),2));_tmpE4[1]=Cyc_Absynpp_typ2doc(_tmpAF);_tmpE4[0]=ptr;
Cyc_PP_cat(_tag_arr(_tmpE4,sizeof(struct Cyc_PP_Doc*),4));});_LL222:;}}_LL148:;}}
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmpE6=Cyc_Tcutil_compress(t);
_LL232: if(_tmpE6 == (void*)Cyc_Absyn_HeapRgn){goto _LL233;}else{goto _LL234;}_LL234:
goto _LL235;_LL233: return Cyc_PP_text(_tag_arr("`H",sizeof(unsigned char),3));
_LL235: return Cyc_Absynpp_ntyp2doc(t);_LL231:;}static void Cyc_Absynpp_effects2docs(
struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){void*_tmpEC=Cyc_Tcutil_compress(
t);void*_tmpF4;struct Cyc_List_List*_tmpF6;_LL238: if((unsigned int)_tmpEC > 3?*((
int*)_tmpEC)== Cyc_Absyn_AccessEff: 0){_LL245: _tmpF4=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmpEC)->f1;goto _LL239;}else{goto _LL240;}_LL240: if((unsigned int)_tmpEC > 3?*((
int*)_tmpEC)== Cyc_Absyn_JoinEff: 0){_LL247: _tmpF6=((struct Cyc_Absyn_JoinEff_struct*)
_tmpEC)->f1;goto _LL241;}else{goto _LL242;}_LL242: goto _LL243;_LL239:*rgions=({
struct Cyc_List_List*_tmpF8=_cycalloc(sizeof(struct Cyc_List_List));_tmpF8->hd=Cyc_Absynpp_rgn2doc(
_tmpF4);_tmpF8->tl=*rgions;_tmpF8;});goto _LL237;_LL241: for(0;_tmpF6 != 0;_tmpF6=
_tmpF6->tl){Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmpF6->hd);}goto
_LL237;_LL243:*effects=({struct Cyc_List_List*_tmpF9=_cycalloc(sizeof(struct Cyc_List_List));
_tmpF9->hd=Cyc_Absynpp_typ2doc(t);_tmpF9->tl=*effects;_tmpF9;});goto _LL237;
_LL237:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=
0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0? effects != 0: 0){return Cyc_PP_group(_tag_arr("",sizeof(
unsigned char),1),_tag_arr("",sizeof(unsigned char),1),_tag_arr("+",sizeof(
unsigned char),2),effects);}else{struct Cyc_PP_Doc*_tmpFA=Cyc_PP_group(_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2),rgions);return Cyc_PP_group(_tag_arr("",sizeof(
unsigned char),1),_tag_arr("",sizeof(unsigned char),1),_tag_arr("+",sizeof(
unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(effects,({struct Cyc_List_List*_tmpFB=_cycalloc(sizeof(
struct Cyc_List_List));_tmpFB->hd=_tmpFA;_tmpFB->tl=0;_tmpFB;})));}}struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmpFC=t;struct Cyc_Core_Opt*
_tmp130;int _tmp132;struct Cyc_Core_Opt*_tmp134;struct Cyc_Core_Opt*_tmp136;struct
Cyc_Absyn_Tvar*_tmp138;struct Cyc_Absyn_TunionInfo _tmp13A;void*_tmp13C;struct Cyc_List_List*
_tmp13E;void*_tmp140;struct Cyc_Absyn_TunionFieldInfo _tmp142;struct Cyc_List_List*
_tmp144;void*_tmp146;void*_tmp148;void*_tmp14A;int _tmp14C;struct Cyc_List_List*
_tmp14E;struct Cyc_List_List*_tmp150;struct _tuple0*_tmp152;struct Cyc_List_List*
_tmp154;struct _tuple0*_tmp156;struct Cyc_List_List*_tmp158;struct Cyc_List_List*
_tmp15A;struct Cyc_List_List*_tmp15C;struct _tuple0*_tmp15E;void*_tmp160;struct Cyc_Core_Opt*
_tmp162;struct Cyc_List_List*_tmp164;struct _tuple0*_tmp166;void*_tmp168;void*
_tmp16A;_LL254: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_ArrayType:
0){goto _LL255;}else{goto _LL256;}_LL256: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)
== Cyc_Absyn_FnType: 0){goto _LL257;}else{goto _LL258;}_LL258: if((unsigned int)
_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_PointerType: 0){goto _LL259;}else{goto _LL260;}
_LL260: if(_tmpFC == (void*)Cyc_Absyn_VoidType){goto _LL261;}else{goto _LL262;}
_LL262: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_Evar: 0){_LL311:
_tmp136=((struct Cyc_Absyn_Evar_struct*)_tmpFC)->f1;goto _LL309;_LL309: _tmp134=((
struct Cyc_Absyn_Evar_struct*)_tmpFC)->f2;goto _LL307;_LL307: _tmp132=((struct Cyc_Absyn_Evar_struct*)
_tmpFC)->f3;goto _LL305;_LL305: _tmp130=((struct Cyc_Absyn_Evar_struct*)_tmpFC)->f4;
goto _LL263;}else{goto _LL264;}_LL264: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== 
Cyc_Absyn_VarType: 0){_LL313: _tmp138=((struct Cyc_Absyn_VarType_struct*)_tmpFC)->f1;
goto _LL265;}else{goto _LL266;}_LL266: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== 
Cyc_Absyn_TunionType: 0){_LL315: _tmp13A=((struct Cyc_Absyn_TunionType_struct*)
_tmpFC)->f1;_LL321: _tmp140=(void*)_tmp13A.tunion_info;goto _LL319;_LL319: _tmp13E=
_tmp13A.targs;goto _LL317;_LL317: _tmp13C=(void*)_tmp13A.rgn;goto _LL267;}else{goto
_LL268;}_LL268: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_TunionFieldType:
0){_LL323: _tmp142=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpFC)->f1;_LL327:
_tmp146=(void*)_tmp142.field_info;goto _LL325;_LL325: _tmp144=_tmp142.targs;goto
_LL269;}else{goto _LL270;}_LL270: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_IntType:
0){_LL331: _tmp14A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpFC)->f1;goto
_LL329;_LL329: _tmp148=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpFC)->f2;goto
_LL271;}else{goto _LL272;}_LL272: if(_tmpFC == (void*)Cyc_Absyn_FloatType){goto
_LL273;}else{goto _LL274;}_LL274: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_DoubleType:
0){_LL333: _tmp14C=((struct Cyc_Absyn_DoubleType_struct*)_tmpFC)->f1;goto _LL275;}
else{goto _LL276;}_LL276: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_TupleType:
0){_LL335: _tmp14E=((struct Cyc_Absyn_TupleType_struct*)_tmpFC)->f1;goto _LL277;}
else{goto _LL278;}_LL278: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_StructType:
0){_LL339: _tmp152=((struct Cyc_Absyn_StructType_struct*)_tmpFC)->f1;goto _LL337;
_LL337: _tmp150=((struct Cyc_Absyn_StructType_struct*)_tmpFC)->f2;goto _LL279;}
else{goto _LL280;}_LL280: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_UnionType:
0){_LL343: _tmp156=((struct Cyc_Absyn_UnionType_struct*)_tmpFC)->f1;goto _LL341;
_LL341: _tmp154=((struct Cyc_Absyn_UnionType_struct*)_tmpFC)->f2;goto _LL281;}else{
goto _LL282;}_LL282: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_AnonStructType:
0){_LL345: _tmp158=((struct Cyc_Absyn_AnonStructType_struct*)_tmpFC)->f1;goto
_LL283;}else{goto _LL284;}_LL284: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_AnonUnionType:
0){_LL347: _tmp15A=((struct Cyc_Absyn_AnonUnionType_struct*)_tmpFC)->f1;goto _LL285;}
else{goto _LL286;}_LL286: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_AnonEnumType:
0){_LL349: _tmp15C=((struct Cyc_Absyn_AnonEnumType_struct*)_tmpFC)->f1;goto _LL287;}
else{goto _LL288;}_LL288: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_EnumType:
0){_LL351: _tmp15E=((struct Cyc_Absyn_EnumType_struct*)_tmpFC)->f1;goto _LL289;}
else{goto _LL290;}_LL290: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_SizeofType:
0){_LL353: _tmp160=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmpFC)->f1;goto
_LL291;}else{goto _LL292;}_LL292: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_TypedefType:
0){_LL359: _tmp166=((struct Cyc_Absyn_TypedefType_struct*)_tmpFC)->f1;goto _LL357;
_LL357: _tmp164=((struct Cyc_Absyn_TypedefType_struct*)_tmpFC)->f2;goto _LL355;
_LL355: _tmp162=((struct Cyc_Absyn_TypedefType_struct*)_tmpFC)->f3;goto _LL293;}
else{goto _LL294;}_LL294: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_RgnHandleType:
0){_LL361: _tmp168=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmpFC)->f1;
goto _LL295;}else{goto _LL296;}_LL296: if(_tmpFC == (void*)Cyc_Absyn_HeapRgn){goto
_LL297;}else{goto _LL298;}_LL298: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_RgnsEff:
0){_LL363: _tmp16A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmpFC)->f1;goto
_LL299;}else{goto _LL300;}_LL300: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)== Cyc_Absyn_AccessEff:
0){goto _LL301;}else{goto _LL302;}_LL302: if((unsigned int)_tmpFC > 3?*((int*)_tmpFC)
== Cyc_Absyn_JoinEff: 0){goto _LL303;}else{goto _LL253;}_LL255: return Cyc_PP_text(
_tag_arr("[[[array]]]",sizeof(unsigned char),12));_LL257: return Cyc_PP_nil_doc();
_LL259: return Cyc_PP_nil_doc();_LL261: s=Cyc_PP_text(_tag_arr("void",sizeof(
unsigned char),5));goto _LL253;_LL263: if(_tmp134 != 0){return Cyc_Absynpp_ntyp2doc((
void*)_tmp134->v);}else{s=({struct Cyc_PP_Doc*_tmp16C[6];_tmp16C[5]=_tmp136 == 0?
Cyc_PP_text(_tag_arr("?",sizeof(unsigned char),2)): Cyc_Absynpp_kind2doc((void*)
_tmp136->v);_tmp16C[4]=Cyc_PP_text(_tag_arr(")::",sizeof(unsigned char),4));
_tmp16C[3]=(! Cyc_Absynpp_print_full_evars? 1: _tmp130 == 0)? Cyc_PP_text(_tag_arr("",
sizeof(unsigned char),1)): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp130->v);
_tmp16C[2]=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp16E;
_tmp16E.tag=Cyc_Std_Int_pa;_tmp16E.f1=(int)((unsigned int)_tmp132);{void*_tmp16D[
1]={& _tmp16E};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(
_tmp16D,sizeof(void*),1));}}));_tmp16C[1]=Cyc_PP_text(_tag_arr("(",sizeof(
unsigned char),2));_tmp16C[0]=Cyc_PP_text(_tag_arr("%",sizeof(unsigned char),2));
Cyc_PP_cat(_tag_arr(_tmp16C,sizeof(struct Cyc_PP_Doc*),6));});}goto _LL253;_LL265:
s=Cyc_PP_textptr(_tmp138->name);if(Cyc_Absynpp_print_all_kinds){s=({struct Cyc_PP_Doc*
_tmp16F[3];_tmp16F[2]=Cyc_Absynpp_kindbound2doc((void*)_tmp138->kind);_tmp16F[1]=
Cyc_PP_text(_tag_arr("::",sizeof(unsigned char),3));_tmp16F[0]=s;Cyc_PP_cat(
_tag_arr(_tmp16F,sizeof(struct Cyc_PP_Doc*),3));});}if(Cyc_Absynpp_rewrite_temp_tvars?
Cyc_Tcutil_is_temp_tvar(_tmp138): 0){s=({struct Cyc_PP_Doc*_tmp170[3];_tmp170[2]=
Cyc_PP_text(_tag_arr(" */",sizeof(unsigned char),4));_tmp170[1]=s;_tmp170[0]=Cyc_PP_text(
_tag_arr("_ /* ",sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp170,sizeof(
struct Cyc_PP_Doc*),3));});}goto _LL253;_LL267:{void*_tmp171=_tmp140;struct Cyc_Absyn_UnknownTunionInfo
_tmp177;int _tmp179;struct _tuple0*_tmp17B;struct Cyc_Absyn_Tuniondecl**_tmp17D;
struct Cyc_Absyn_Tuniondecl*_tmp17F;struct Cyc_Absyn_Tuniondecl _tmp180;int _tmp181;
struct _tuple0*_tmp183;_LL371: if(*((int*)_tmp171)== Cyc_Absyn_UnknownTunion){
_LL376: _tmp177=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp171)->f1;_LL380:
_tmp17B=_tmp177.name;goto _LL378;_LL378: _tmp179=_tmp177.is_xtunion;goto _LL372;}
else{goto _LL373;}_LL373: if(*((int*)_tmp171)== Cyc_Absyn_KnownTunion){_LL382:
_tmp17D=((struct Cyc_Absyn_KnownTunion_struct*)_tmp171)->f1;_tmp17F=*_tmp17D;
_tmp180=*_tmp17F;_LL388: _tmp183=_tmp180.name;goto _LL386;_LL386: _tmp181=_tmp180.is_xtunion;
goto _LL374;}else{goto _LL370;}_LL372: _tmp183=_tmp17B;_tmp181=_tmp179;goto _LL374;
_LL374: {struct Cyc_PP_Doc*_tmp185=Cyc_PP_text(_tmp181? _tag_arr("xtunion ",
sizeof(unsigned char),9): _tag_arr("tunion ",sizeof(unsigned char),8));{void*
_tmp186=Cyc_Tcutil_compress(_tmp13C);_LL392: if(_tmp186 == (void*)Cyc_Absyn_HeapRgn){
goto _LL393;}else{goto _LL394;}_LL394: goto _LL395;_LL393: s=({struct Cyc_PP_Doc*
_tmp18C[3];_tmp18C[2]=Cyc_Absynpp_tps2doc(_tmp13E);_tmp18C[1]=Cyc_Absynpp_qvar2doc(
_tmp183);_tmp18C[0]=_tmp185;Cyc_PP_cat(_tag_arr(_tmp18C,sizeof(struct Cyc_PP_Doc*),
3));});goto _LL391;_LL395: s=({struct Cyc_PP_Doc*_tmp18D[5];_tmp18D[4]=Cyc_Absynpp_tps2doc(
_tmp13E);_tmp18D[3]=Cyc_Absynpp_qvar2doc(_tmp183);_tmp18D[2]=Cyc_PP_text(
_tag_arr(" ",sizeof(unsigned char),2));_tmp18D[1]=Cyc_Absynpp_typ2doc(_tmp13C);
_tmp18D[0]=_tmp185;Cyc_PP_cat(_tag_arr(_tmp18D,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL391;_LL391:;}goto _LL370;}_LL370:;}goto _LL253;_LL269:{void*_tmp18E=_tmp146;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp194;int _tmp196;struct _tuple0*_tmp198;
struct _tuple0*_tmp19A;struct Cyc_Absyn_Tunionfield*_tmp19C;struct Cyc_Absyn_Tunionfield
_tmp19E;struct _tuple0*_tmp19F;struct Cyc_Absyn_Tuniondecl*_tmp1A1;struct Cyc_Absyn_Tuniondecl
_tmp1A3;int _tmp1A4;struct _tuple0*_tmp1A6;_LL400: if(*((int*)_tmp18E)== Cyc_Absyn_UnknownTunionfield){
_LL405: _tmp194=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp18E)->f1;_LL411:
_tmp19A=_tmp194.tunion_name;goto _LL409;_LL409: _tmp198=_tmp194.field_name;goto
_LL407;_LL407: _tmp196=_tmp194.is_xtunion;goto _LL401;}else{goto _LL402;}_LL402: if(*((
int*)_tmp18E)== Cyc_Absyn_KnownTunionfield){_LL418: _tmp1A1=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp18E)->f1;_tmp1A3=*_tmp1A1;_LL423: _tmp1A6=_tmp1A3.name;goto _LL421;_LL421:
_tmp1A4=_tmp1A3.is_xtunion;goto _LL413;_LL413: _tmp19C=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp18E)->f2;_tmp19E=*_tmp19C;_LL416: _tmp19F=_tmp19E.name;goto _LL403;}else{goto
_LL399;}_LL401: _tmp1A6=_tmp19A;_tmp1A4=_tmp196;_tmp19F=_tmp198;goto _LL403;_LL403: {
struct Cyc_PP_Doc*_tmp1A8=Cyc_PP_text(_tmp1A4? _tag_arr("xtunion ",sizeof(
unsigned char),9): _tag_arr("tunion ",sizeof(unsigned char),8));s=({struct Cyc_PP_Doc*
_tmp1A9[4];_tmp1A9[3]=Cyc_Absynpp_qvar2doc(_tmp19F);_tmp1A9[2]=Cyc_PP_text(
_tag_arr(".",sizeof(unsigned char),2));_tmp1A9[1]=Cyc_Absynpp_qvar2doc(_tmp1A6);
_tmp1A9[0]=_tmp1A8;Cyc_PP_cat(_tag_arr(_tmp1A9,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL399;}_LL399:;}goto _LL253;_LL271: {struct _tagged_arr sns;struct _tagged_arr
ts;{void*_tmp1AA=_tmp14A;_LL428: if(_tmp1AA == (void*)Cyc_Absyn_Signed){goto _LL429;}
else{goto _LL430;}_LL430: if(_tmp1AA == (void*)Cyc_Absyn_Unsigned){goto _LL431;}
else{goto _LL427;}_LL429: sns=_tag_arr("",sizeof(unsigned char),1);goto _LL427;
_LL431: sns=_tag_arr("unsigned ",sizeof(unsigned char),10);goto _LL427;_LL427:;}{
void*_tmp1B0=_tmp148;_LL434: if(_tmp1B0 == (void*)Cyc_Absyn_B1){goto _LL435;}else{
goto _LL436;}_LL436: if(_tmp1B0 == (void*)Cyc_Absyn_B2){goto _LL437;}else{goto _LL438;}
_LL438: if(_tmp1B0 == (void*)Cyc_Absyn_B4){goto _LL439;}else{goto _LL440;}_LL440: if(
_tmp1B0 == (void*)Cyc_Absyn_B8){goto _LL441;}else{goto _LL433;}_LL435:{void*_tmp1BA=
_tmp14A;_LL444: if(_tmp1BA == (void*)Cyc_Absyn_Signed){goto _LL445;}else{goto _LL446;}
_LL446: if(_tmp1BA == (void*)Cyc_Absyn_Unsigned){goto _LL447;}else{goto _LL443;}
_LL445: sns=_tag_arr("signed ",sizeof(unsigned char),8);goto _LL443;_LL447: goto
_LL443;_LL443:;}ts=_tag_arr("char",sizeof(unsigned char),5);goto _LL433;_LL437: ts=
_tag_arr("short",sizeof(unsigned char),6);goto _LL433;_LL439: ts=_tag_arr("int",
sizeof(unsigned char),4);goto _LL433;_LL441: ts=Cyc_Absynpp_to_VC? _tag_arr("__int64",
sizeof(unsigned char),8): _tag_arr("long long",sizeof(unsigned char),10);goto
_LL433;_LL433:;}s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp1C2;_tmp1C2.tag=Cyc_Std_String_pa;_tmp1C2.f1=(struct _tagged_arr)ts;{struct
Cyc_Std_String_pa_struct _tmp1C1;_tmp1C1.tag=Cyc_Std_String_pa;_tmp1C1.f1=(struct
_tagged_arr)sns;{void*_tmp1C0[2]={& _tmp1C1,& _tmp1C2};Cyc_Std_aprintf(_tag_arr("%s%s",
sizeof(unsigned char),5),_tag_arr(_tmp1C0,sizeof(void*),2));}}}));goto _LL253;}
_LL273: s=Cyc_PP_text(_tag_arr("float",sizeof(unsigned char),6));goto _LL253;
_LL275: if(_tmp14C){s=Cyc_PP_text(_tag_arr("long double",sizeof(unsigned char),12));}
else{s=Cyc_PP_text(_tag_arr("double",sizeof(unsigned char),7));}goto _LL253;
_LL277: s=({struct Cyc_PP_Doc*_tmp1C3[2];_tmp1C3[1]=Cyc_Absynpp_args2doc(_tmp14E);
_tmp1C3[0]=Cyc_PP_text(_tag_arr("$",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp1C3,sizeof(struct Cyc_PP_Doc*),2));});goto _LL253;_LL279: if(_tmp152 == 
0){s=({struct Cyc_PP_Doc*_tmp1C4[2];_tmp1C4[1]=Cyc_Absynpp_tps2doc(_tmp150);
_tmp1C4[0]=Cyc_PP_text(_tag_arr("struct ",sizeof(unsigned char),8));Cyc_PP_cat(
_tag_arr(_tmp1C4,sizeof(struct Cyc_PP_Doc*),2));});}else{s=({struct Cyc_PP_Doc*
_tmp1C5[3];_tmp1C5[2]=Cyc_Absynpp_tps2doc(_tmp150);_tmp1C5[1]=_tmp152 == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc((struct _tuple0*)_check_null(_tmp152));_tmp1C5[0]=Cyc_PP_text(
_tag_arr("struct ",sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp1C5,sizeof(
struct Cyc_PP_Doc*),3));});}goto _LL253;_LL281: if(_tmp156 == 0){s=({struct Cyc_PP_Doc*
_tmp1C6[2];_tmp1C6[1]=Cyc_Absynpp_tps2doc(_tmp154);_tmp1C6[0]=Cyc_PP_text(
_tag_arr("union ",sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(_tmp1C6,sizeof(
struct Cyc_PP_Doc*),2));});}else{s=({struct Cyc_PP_Doc*_tmp1C7[3];_tmp1C7[2]=Cyc_Absynpp_tps2doc(
_tmp154);_tmp1C7[1]=_tmp156 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((struct
_tuple0*)_check_null(_tmp156));_tmp1C7[0]=Cyc_PP_text(_tag_arr("union ",sizeof(
unsigned char),7));Cyc_PP_cat(_tag_arr(_tmp1C7,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL253;_LL283: s=({struct Cyc_PP_Doc*_tmp1C8[3];_tmp1C8[2]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp1C8[1]=Cyc_PP_nest(2,Cyc_Absynpp_structfields2doc(
_tmp158));_tmp1C8[0]=Cyc_PP_text(_tag_arr("struct {",sizeof(unsigned char),9));
Cyc_PP_cat(_tag_arr(_tmp1C8,sizeof(struct Cyc_PP_Doc*),3));});goto _LL253;_LL285: s=({
struct Cyc_PP_Doc*_tmp1C9[3];_tmp1C9[2]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp1C9[1]=Cyc_PP_nest(2,Cyc_Absynpp_structfields2doc(_tmp15A));
_tmp1C9[0]=Cyc_PP_text(_tag_arr("union {",sizeof(unsigned char),8));Cyc_PP_cat(
_tag_arr(_tmp1C9,sizeof(struct Cyc_PP_Doc*),3));});goto _LL253;_LL287: s=({struct
Cyc_PP_Doc*_tmp1CA[3];_tmp1CA[2]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),
2));_tmp1CA[1]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp15C));_tmp1CA[0]=Cyc_PP_text(
_tag_arr("enum {",sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(_tmp1CA,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL253;_LL289: s=({struct Cyc_PP_Doc*_tmp1CB[2];
_tmp1CB[1]=Cyc_Absynpp_qvar2doc(_tmp15E);_tmp1CB[0]=Cyc_PP_text(_tag_arr("enum ",
sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp1CB,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL253;_LL291: s=({struct Cyc_PP_Doc*_tmp1CC[3];_tmp1CC[2]=Cyc_PP_text(
_tag_arr(">",sizeof(unsigned char),2));_tmp1CC[1]=Cyc_Absynpp_typ2doc(_tmp160);
_tmp1CC[0]=Cyc_PP_text(_tag_arr("sizeof_t<",sizeof(unsigned char),10));Cyc_PP_cat(
_tag_arr(_tmp1CC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL253;_LL293: s=({struct
Cyc_PP_Doc*_tmp1CD[2];_tmp1CD[1]=Cyc_Absynpp_tps2doc(_tmp164);_tmp1CD[0]=Cyc_Absynpp_qvar2doc(
_tmp166);Cyc_PP_cat(_tag_arr(_tmp1CD,sizeof(struct Cyc_PP_Doc*),2));});goto _LL253;
_LL295: s=({struct Cyc_PP_Doc*_tmp1CE[3];_tmp1CE[2]=Cyc_PP_text(_tag_arr(">",
sizeof(unsigned char),2));_tmp1CE[1]=Cyc_Absynpp_rgn2doc(_tmp168);_tmp1CE[0]=Cyc_PP_text(
_tag_arr("region_t<",sizeof(unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp1CE,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL253;_LL297: s=Cyc_Absynpp_rgn2doc(t);goto
_LL253;_LL299: s=({struct Cyc_PP_Doc*_tmp1CF[3];_tmp1CF[2]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp1CF[1]=Cyc_Absynpp_typ2doc(_tmp16A);_tmp1CF[0]=Cyc_PP_text(
_tag_arr("regions(",sizeof(unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp1CF,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL253;_LL301: goto _LL303;_LL303: s=Cyc_Absynpp_eff2doc(
t);goto _LL253;_LL253:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*
vo){return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _tagged_arr*)vo->v));}
struct _tuple8{void*f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct
_tuple8*cmp){struct _tuple8 _tmp1D2;void*_tmp1D3;void*_tmp1D5;struct _tuple8*
_tmp1D0=cmp;_tmp1D2=*_tmp1D0;_LL470: _tmp1D5=_tmp1D2.f1;goto _LL468;_LL468: _tmp1D3=
_tmp1D2.f2;goto _LL465;_LL465: return({struct Cyc_PP_Doc*_tmp1D7[3];_tmp1D7[2]=Cyc_Absynpp_rgn2doc(
_tmp1D3);_tmp1D7[1]=Cyc_PP_text(_tag_arr(" < ",sizeof(unsigned char),4));_tmp1D7[
0]=Cyc_Absynpp_rgn2doc(_tmp1D5);Cyc_PP_cat(_tag_arr(_tmp1D7,sizeof(struct Cyc_PP_Doc*),
3));});}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){return Cyc_PP_group(
_tag_arr("",sizeof(unsigned char),1),_tag_arr("",sizeof(unsigned char),1),
_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,
po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple1*t){struct Cyc_Core_Opt*
dopt=(*t).f1 == 0? 0:({struct Cyc_Core_Opt*_tmp1D8=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp1D8->v=Cyc_PP_text(*((struct _tagged_arr*)((struct Cyc_Core_Opt*)_check_null((*
t).f1))->v));_tmp1D8;});return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}struct
Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct
Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*effopt,struct Cyc_List_List*
rgn_po){struct Cyc_List_List*_tmp1D9=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs){_tmp1D9=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1D9,({struct
Cyc_List_List*_tmp1DA=_cycalloc(sizeof(struct Cyc_List_List));_tmp1DA->hd=Cyc_PP_text(
_tag_arr("...",sizeof(unsigned char),4));_tmp1DA->tl=0;_tmp1DA;}));}else{if(
cyc_varargs != 0){struct Cyc_PP_Doc*_tmp1DB=({struct Cyc_PP_Doc*_tmp1DD[3];_tmp1DD[
2]=Cyc_Absynpp_funarg2doc(({struct _tuple1*_tmp1DE=_cycalloc(sizeof(struct _tuple1));
_tmp1DE->f1=cyc_varargs->name;_tmp1DE->f2=cyc_varargs->tq;_tmp1DE->f3=(void*)
cyc_varargs->type;_tmp1DE;}));_tmp1DD[1]=cyc_varargs->inject? Cyc_PP_text(
_tag_arr(" inject ",sizeof(unsigned char),9)): Cyc_PP_text(_tag_arr(" ",sizeof(
unsigned char),2));_tmp1DD[0]=Cyc_PP_text(_tag_arr("...",sizeof(unsigned char),4));
Cyc_PP_cat(_tag_arr(_tmp1DD,sizeof(struct Cyc_PP_Doc*),3));});_tmp1D9=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1D9,({struct
Cyc_List_List*_tmp1DC=_cycalloc(sizeof(struct Cyc_List_List));_tmp1DC->hd=_tmp1DB;
_tmp1DC->tl=0;_tmp1DC;}));}}{struct Cyc_PP_Doc*_tmp1DF=Cyc_PP_group(_tag_arr("",
sizeof(unsigned char),1),_tag_arr("",sizeof(unsigned char),1),_tag_arr(",",
sizeof(unsigned char),2),_tmp1D9);if(effopt != 0){_tmp1DF=({struct Cyc_PP_Doc*
_tmp1E0[3];_tmp1E0[2]=Cyc_Absynpp_eff2doc((void*)effopt->v);_tmp1E0[1]=Cyc_PP_text(
_tag_arr(";",sizeof(unsigned char),2));_tmp1E0[0]=_tmp1DF;Cyc_PP_cat(_tag_arr(
_tmp1E0,sizeof(struct Cyc_PP_Doc*),3));});}if(rgn_po != 0){_tmp1DF=({struct Cyc_PP_Doc*
_tmp1E1[3];_tmp1E1[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);_tmp1E1[1]=Cyc_PP_text(
_tag_arr(":",sizeof(unsigned char),2));_tmp1E1[0]=_tmp1DF;Cyc_PP_cat(_tag_arr(
_tmp1E1,sizeof(struct Cyc_PP_Doc*),3));});}return({struct Cyc_PP_Doc*_tmp1E2[3];
_tmp1E2[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp1E2[1]=_tmp1DF;
_tmp1E2[0]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp1E2,sizeof(struct Cyc_PP_Doc*),3));});}}struct _tuple1*Cyc_Absynpp_arg_mk_opt(
struct _tuple3*arg){return({struct _tuple1*_tmp1E3=_cycalloc(sizeof(struct _tuple1));
_tmp1E3->f1=({struct Cyc_Core_Opt*_tmp1E4=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp1E4->v=(*arg).f1;_tmp1E4;});_tmp1E3->f2=(*arg).f2;_tmp1E3->f3=(*arg).f3;
_tmp1E3;});}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _tagged_arr*v){return Cyc_PP_text(*
v);}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){struct Cyc_List_List*
_tmp1E5=0;int match;{void*_tmp1E6=(*q).f1;struct Cyc_List_List*_tmp1EE;struct Cyc_List_List*
_tmp1F0;_LL488: if(_tmp1E6 == (void*)Cyc_Absyn_Loc_n){goto _LL489;}else{goto _LL490;}
_LL490: if((unsigned int)_tmp1E6 > 1?*((int*)_tmp1E6)== Cyc_Absyn_Rel_n: 0){_LL495:
_tmp1EE=((struct Cyc_Absyn_Rel_n_struct*)_tmp1E6)->f1;goto _LL491;}else{goto _LL492;}
_LL492: if((unsigned int)_tmp1E6 > 1?*((int*)_tmp1E6)== Cyc_Absyn_Abs_n: 0){_LL497:
_tmp1F0=((struct Cyc_Absyn_Abs_n_struct*)_tmp1E6)->f1;goto _LL493;}else{goto _LL487;}
_LL489: _tmp1EE=0;goto _LL491;_LL491: match=0;_tmp1E5=_tmp1EE;goto _LL487;_LL493:
match=Cyc_Absynpp_use_curr_namespace?((int(*)(int(*cmp)(struct _tagged_arr*,
struct _tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(
Cyc_Std_strptrcmp,_tmp1F0,Cyc_Absynpp_curr_namespace): 0;_tmp1E5=(Cyc_Absynpp_qvar_to_Cids?
Cyc_Absynpp_add_cyc_prefix: 0)?({struct Cyc_List_List*_tmp1F2=_cycalloc(sizeof(
struct Cyc_List_List));_tmp1F2->hd=Cyc_Absynpp_cyc_stringptr;_tmp1F2->tl=_tmp1F0;
_tmp1F2;}): _tmp1F0;goto _LL487;_LL487:;}if(Cyc_Absynpp_qvar_to_Cids){return Cyc_PP_text((
struct _tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1E5,({struct Cyc_List_List*_tmp1F3=
_cycalloc(sizeof(struct Cyc_List_List));_tmp1F3->hd=(*q).f2;_tmp1F3->tl=0;_tmp1F3;})),
_tag_arr("_",sizeof(unsigned char),2)));}else{if(match){return Cyc_Absynpp_var2doc((*
q).f2);}else{return Cyc_PP_text((struct _tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1E5,({struct
Cyc_List_List*_tmp1F4=_cycalloc(sizeof(struct Cyc_List_List));_tmp1F4->hd=(*q).f2;
_tmp1F4->tl=0;_tmp1F4;})),_tag_arr("::",sizeof(unsigned char),3)));}}}struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids){
return Cyc_Absynpp_qvar2doc(v);}if(Cyc_Absynpp_use_curr_namespace){void*_tmp1F5=(*
v).f1;struct Cyc_List_List*_tmp1FF;struct Cyc_List_List*_tmp201;_LL503: if(_tmp1F5
== (void*)Cyc_Absyn_Loc_n){goto _LL504;}else{goto _LL505;}_LL505: if((unsigned int)
_tmp1F5 > 1?*((int*)_tmp1F5)== Cyc_Absyn_Rel_n: 0){_LL512: _tmp1FF=((struct Cyc_Absyn_Rel_n_struct*)
_tmp1F5)->f1;if(_tmp1FF == 0){goto _LL506;}else{goto _LL507;}}else{goto _LL507;}
_LL507: if((unsigned int)_tmp1F5 > 1?*((int*)_tmp1F5)== Cyc_Absyn_Abs_n: 0){_LL514:
_tmp201=((struct Cyc_Absyn_Abs_n_struct*)_tmp1F5)->f1;goto _LL508;}else{goto _LL509;}
_LL509: goto _LL510;_LL504: goto _LL506;_LL506: return Cyc_Absynpp_var2doc((*v).f2);
_LL508: if(((int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Std_strptrcmp,_tmp201,Cyc_Absynpp_curr_namespace)
== 0){return Cyc_Absynpp_var2doc((*v).f2);}else{goto _LL510;}_LL510: return({struct
Cyc_PP_Doc*_tmp203[2];_tmp203[1]=Cyc_Absynpp_qvar2doc(v);_tmp203[0]=Cyc_PP_text(
_tag_arr("/* bad namespace : */ ",sizeof(unsigned char),23));Cyc_PP_cat(_tag_arr(
_tmp203,sizeof(struct Cyc_PP_Doc*),2));});_LL502:;}else{return Cyc_Absynpp_var2doc((*
v).f2);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(),t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp204=(void*)e->r;void*_tmp252;struct Cyc_Absyn_Exp*_tmp254;struct Cyc_Absyn_Exp*
_tmp256;_LL518: if(*((int*)_tmp204)== Cyc_Absyn_Const_e){goto _LL519;}else{goto
_LL520;}_LL520: if(*((int*)_tmp204)== Cyc_Absyn_Var_e){goto _LL521;}else{goto
_LL522;}_LL522: if(*((int*)_tmp204)== Cyc_Absyn_UnknownId_e){goto _LL523;}else{
goto _LL524;}_LL524: if(*((int*)_tmp204)== Cyc_Absyn_Primop_e){_LL595: _tmp252=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp204)->f1;goto _LL525;}else{goto
_LL526;}_LL526: if(*((int*)_tmp204)== Cyc_Absyn_AssignOp_e){goto _LL527;}else{goto
_LL528;}_LL528: if(*((int*)_tmp204)== Cyc_Absyn_Increment_e){goto _LL529;}else{
goto _LL530;}_LL530: if(*((int*)_tmp204)== Cyc_Absyn_Conditional_e){goto _LL531;}
else{goto _LL532;}_LL532: if(*((int*)_tmp204)== Cyc_Absyn_SeqExp_e){goto _LL533;}
else{goto _LL534;}_LL534: if(*((int*)_tmp204)== Cyc_Absyn_UnknownCall_e){goto
_LL535;}else{goto _LL536;}_LL536: if(*((int*)_tmp204)== Cyc_Absyn_FnCall_e){goto
_LL537;}else{goto _LL538;}_LL538: if(*((int*)_tmp204)== Cyc_Absyn_Throw_e){goto
_LL539;}else{goto _LL540;}_LL540: if(*((int*)_tmp204)== Cyc_Absyn_NoInstantiate_e){
_LL597: _tmp254=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp204)->f1;goto _LL541;}
else{goto _LL542;}_LL542: if(*((int*)_tmp204)== Cyc_Absyn_Instantiate_e){_LL599:
_tmp256=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp204)->f1;goto _LL543;}else{
goto _LL544;}_LL544: if(*((int*)_tmp204)== Cyc_Absyn_Cast_e){goto _LL545;}else{goto
_LL546;}_LL546: if(*((int*)_tmp204)== Cyc_Absyn_New_e){goto _LL547;}else{goto
_LL548;}_LL548: if(*((int*)_tmp204)== Cyc_Absyn_Address_e){goto _LL549;}else{goto
_LL550;}_LL550: if(*((int*)_tmp204)== Cyc_Absyn_Sizeoftyp_e){goto _LL551;}else{
goto _LL552;}_LL552: if(*((int*)_tmp204)== Cyc_Absyn_Sizeofexp_e){goto _LL553;}
else{goto _LL554;}_LL554: if(*((int*)_tmp204)== Cyc_Absyn_Offsetof_e){goto _LL555;}
else{goto _LL556;}_LL556: if(*((int*)_tmp204)== Cyc_Absyn_Gentyp_e){goto _LL557;}
else{goto _LL558;}_LL558: if(*((int*)_tmp204)== Cyc_Absyn_Deref_e){goto _LL559;}
else{goto _LL560;}_LL560: if(*((int*)_tmp204)== Cyc_Absyn_StructMember_e){goto
_LL561;}else{goto _LL562;}_LL562: if(*((int*)_tmp204)== Cyc_Absyn_StructArrow_e){
goto _LL563;}else{goto _LL564;}_LL564: if(*((int*)_tmp204)== Cyc_Absyn_Subscript_e){
goto _LL565;}else{goto _LL566;}_LL566: if(*((int*)_tmp204)== Cyc_Absyn_Tuple_e){
goto _LL567;}else{goto _LL568;}_LL568: if(*((int*)_tmp204)== Cyc_Absyn_CompoundLit_e){
goto _LL569;}else{goto _LL570;}_LL570: if(*((int*)_tmp204)== Cyc_Absyn_Array_e){
goto _LL571;}else{goto _LL572;}_LL572: if(*((int*)_tmp204)== Cyc_Absyn_Comprehension_e){
goto _LL573;}else{goto _LL574;}_LL574: if(*((int*)_tmp204)== Cyc_Absyn_Struct_e){
goto _LL575;}else{goto _LL576;}_LL576: if(*((int*)_tmp204)== Cyc_Absyn_AnonStruct_e){
goto _LL577;}else{goto _LL578;}_LL578: if(*((int*)_tmp204)== Cyc_Absyn_Tunion_e){
goto _LL579;}else{goto _LL580;}_LL580: if(*((int*)_tmp204)== Cyc_Absyn_Enum_e){goto
_LL581;}else{goto _LL582;}_LL582: if(*((int*)_tmp204)== Cyc_Absyn_AnonEnum_e){goto
_LL583;}else{goto _LL584;}_LL584: if(*((int*)_tmp204)== Cyc_Absyn_Malloc_e){goto
_LL585;}else{goto _LL586;}_LL586: if(*((int*)_tmp204)== Cyc_Absyn_UnresolvedMem_e){
goto _LL587;}else{goto _LL588;}_LL588: if(*((int*)_tmp204)== Cyc_Absyn_StmtExp_e){
goto _LL589;}else{goto _LL590;}_LL590: if(*((int*)_tmp204)== Cyc_Absyn_Codegen_e){
goto _LL591;}else{goto _LL592;}_LL592: if(*((int*)_tmp204)== Cyc_Absyn_Fill_e){goto
_LL593;}else{goto _LL517;}_LL519: return 10000;_LL521: return 10000;_LL523: return
10000;_LL525: {void*_tmp258=_tmp252;_LL602: if(_tmp258 == (void*)Cyc_Absyn_Plus){
goto _LL603;}else{goto _LL604;}_LL604: if(_tmp258 == (void*)Cyc_Absyn_Times){goto
_LL605;}else{goto _LL606;}_LL606: if(_tmp258 == (void*)Cyc_Absyn_Minus){goto _LL607;}
else{goto _LL608;}_LL608: if(_tmp258 == (void*)Cyc_Absyn_Div){goto _LL609;}else{goto
_LL610;}_LL610: if(_tmp258 == (void*)Cyc_Absyn_Mod){goto _LL611;}else{goto _LL612;}
_LL612: if(_tmp258 == (void*)Cyc_Absyn_Eq){goto _LL613;}else{goto _LL614;}_LL614: if(
_tmp258 == (void*)Cyc_Absyn_Neq){goto _LL615;}else{goto _LL616;}_LL616: if(_tmp258 == (
void*)Cyc_Absyn_Gt){goto _LL617;}else{goto _LL618;}_LL618: if(_tmp258 == (void*)Cyc_Absyn_Lt){
goto _LL619;}else{goto _LL620;}_LL620: if(_tmp258 == (void*)Cyc_Absyn_Gte){goto
_LL621;}else{goto _LL622;}_LL622: if(_tmp258 == (void*)Cyc_Absyn_Lte){goto _LL623;}
else{goto _LL624;}_LL624: if(_tmp258 == (void*)Cyc_Absyn_Not){goto _LL625;}else{goto
_LL626;}_LL626: if(_tmp258 == (void*)Cyc_Absyn_Bitnot){goto _LL627;}else{goto _LL628;}
_LL628: if(_tmp258 == (void*)Cyc_Absyn_Bitand){goto _LL629;}else{goto _LL630;}_LL630:
if(_tmp258 == (void*)Cyc_Absyn_Bitor){goto _LL631;}else{goto _LL632;}_LL632: if(
_tmp258 == (void*)Cyc_Absyn_Bitxor){goto _LL633;}else{goto _LL634;}_LL634: if(
_tmp258 == (void*)Cyc_Absyn_Bitlshift){goto _LL635;}else{goto _LL636;}_LL636: if(
_tmp258 == (void*)Cyc_Absyn_Bitlrshift){goto _LL637;}else{goto _LL638;}_LL638: if(
_tmp258 == (void*)Cyc_Absyn_Bitarshift){goto _LL639;}else{goto _LL640;}_LL640: if(
_tmp258 == (void*)Cyc_Absyn_Size){goto _LL641;}else{goto _LL601;}_LL603: return 100;
_LL605: return 110;_LL607: return 100;_LL609: return 110;_LL611: return 110;_LL613:
return 70;_LL615: return 70;_LL617: return 80;_LL619: return 80;_LL621: return 80;_LL623:
return 80;_LL625: return 130;_LL627: return 130;_LL629: return 60;_LL631: return 40;
_LL633: return 50;_LL635: return 90;_LL637: return 80;_LL639: return 80;_LL641: return 140;
_LL601:;}_LL527: return 20;_LL529: return 130;_LL531: return 30;_LL533: return 10;_LL535:
return 140;_LL537: return 140;_LL539: return 130;_LL541: return Cyc_Absynpp_exp_prec(
_tmp254);_LL543: return Cyc_Absynpp_exp_prec(_tmp256);_LL545: return 120;_LL547:
return 15;_LL549: return 130;_LL551: return 130;_LL553: return 130;_LL555: return 130;
_LL557: return 130;_LL559: return 130;_LL561: return 140;_LL563: return 140;_LL565:
return 140;_LL567: return 150;_LL569: goto _LL571;_LL571: goto _LL573;_LL573: goto _LL575;
_LL575: goto _LL577;_LL577: goto _LL579;_LL579: goto _LL581;_LL581: goto _LL583;_LL583:
goto _LL585;_LL585: return 140;_LL587: return 140;_LL589: return 10000;_LL591: return 140;
_LL593: return 140;_LL517:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*
e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmp282=(void*)e->r;void*_tmp2D2;struct _tuple0*_tmp2D4;struct _tuple0*
_tmp2D6;struct Cyc_List_List*_tmp2D8;void*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2DC;
struct Cyc_Core_Opt*_tmp2DE;struct Cyc_Absyn_Exp*_tmp2E0;void*_tmp2E2;struct Cyc_Absyn_Exp*
_tmp2E4;struct Cyc_Absyn_Exp*_tmp2E6;struct Cyc_Absyn_Exp*_tmp2E8;struct Cyc_Absyn_Exp*
_tmp2EA;struct Cyc_Absyn_Exp*_tmp2EC;struct Cyc_Absyn_Exp*_tmp2EE;struct Cyc_List_List*
_tmp2F0;struct Cyc_Absyn_Exp*_tmp2F2;struct Cyc_List_List*_tmp2F4;struct Cyc_Absyn_Exp*
_tmp2F6;struct Cyc_Absyn_Exp*_tmp2F8;struct Cyc_Absyn_Exp*_tmp2FA;struct Cyc_Absyn_Exp*
_tmp2FC;struct Cyc_Absyn_Exp*_tmp2FE;void*_tmp300;struct Cyc_Absyn_Exp*_tmp302;
struct Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Exp*_tmp306;void*_tmp308;struct Cyc_Absyn_Exp*
_tmp30A;void*_tmp30C;struct _tagged_arr*_tmp30E;void*_tmp310;void*_tmp312;
unsigned int _tmp314;void*_tmp316;void*_tmp318;struct Cyc_List_List*_tmp31A;struct
Cyc_Absyn_Exp*_tmp31C;struct _tagged_arr*_tmp31E;struct Cyc_Absyn_Exp*_tmp320;
struct _tagged_arr*_tmp322;struct Cyc_Absyn_Exp*_tmp324;struct Cyc_Absyn_Exp*
_tmp326;struct Cyc_Absyn_Exp*_tmp328;struct Cyc_List_List*_tmp32A;struct Cyc_List_List*
_tmp32C;struct _tuple1*_tmp32E;struct Cyc_List_List*_tmp330;struct Cyc_Absyn_Exp*
_tmp332;struct Cyc_Absyn_Exp*_tmp334;struct Cyc_Absyn_Vardecl*_tmp336;struct Cyc_List_List*
_tmp338;struct _tuple0*_tmp33A;struct Cyc_List_List*_tmp33C;struct Cyc_Absyn_Tunionfield*
_tmp33E;struct Cyc_List_List*_tmp340;struct _tuple0*_tmp342;struct _tuple0*_tmp344;
struct Cyc_Absyn_MallocInfo _tmp346;struct Cyc_Absyn_Exp*_tmp348;void**_tmp34A;
struct Cyc_Absyn_Exp*_tmp34C;int _tmp34E;struct Cyc_List_List*_tmp350;struct Cyc_Core_Opt*
_tmp352;struct Cyc_Absyn_Stmt*_tmp354;struct Cyc_Absyn_Fndecl*_tmp356;struct Cyc_Absyn_Exp*
_tmp358;_LL644: if(*((int*)_tmp282)== Cyc_Absyn_Const_e){_LL723: _tmp2D2=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp282)->f1;goto _LL645;}else{goto _LL646;}_LL646:
if(*((int*)_tmp282)== Cyc_Absyn_Var_e){_LL725: _tmp2D4=((struct Cyc_Absyn_Var_e_struct*)
_tmp282)->f1;goto _LL647;}else{goto _LL648;}_LL648: if(*((int*)_tmp282)== Cyc_Absyn_UnknownId_e){
_LL727: _tmp2D6=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp282)->f1;goto _LL649;}
else{goto _LL650;}_LL650: if(*((int*)_tmp282)== Cyc_Absyn_Primop_e){_LL731: _tmp2DA=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp282)->f1;goto _LL729;_LL729: _tmp2D8=((
struct Cyc_Absyn_Primop_e_struct*)_tmp282)->f2;goto _LL651;}else{goto _LL652;}
_LL652: if(*((int*)_tmp282)== Cyc_Absyn_AssignOp_e){_LL737: _tmp2E0=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp282)->f1;goto _LL735;_LL735: _tmp2DE=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp282)->f2;goto _LL733;_LL733: _tmp2DC=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp282)->f3;goto _LL653;}else{goto _LL654;}_LL654: if(*((int*)_tmp282)== Cyc_Absyn_Increment_e){
_LL741: _tmp2E4=((struct Cyc_Absyn_Increment_e_struct*)_tmp282)->f1;goto _LL739;
_LL739: _tmp2E2=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp282)->f2;goto
_LL655;}else{goto _LL656;}_LL656: if(*((int*)_tmp282)== Cyc_Absyn_Conditional_e){
_LL747: _tmp2EA=((struct Cyc_Absyn_Conditional_e_struct*)_tmp282)->f1;goto _LL745;
_LL745: _tmp2E8=((struct Cyc_Absyn_Conditional_e_struct*)_tmp282)->f2;goto _LL743;
_LL743: _tmp2E6=((struct Cyc_Absyn_Conditional_e_struct*)_tmp282)->f3;goto _LL657;}
else{goto _LL658;}_LL658: if(*((int*)_tmp282)== Cyc_Absyn_SeqExp_e){_LL751: _tmp2EE=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp282)->f1;goto _LL749;_LL749: _tmp2EC=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp282)->f2;goto _LL659;}else{goto _LL660;}_LL660: if(*((
int*)_tmp282)== Cyc_Absyn_UnknownCall_e){_LL755: _tmp2F2=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp282)->f1;goto _LL753;_LL753: _tmp2F0=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp282)->f2;goto _LL661;}else{goto _LL662;}_LL662: if(*((int*)_tmp282)== Cyc_Absyn_FnCall_e){
_LL759: _tmp2F6=((struct Cyc_Absyn_FnCall_e_struct*)_tmp282)->f1;goto _LL757;_LL757:
_tmp2F4=((struct Cyc_Absyn_FnCall_e_struct*)_tmp282)->f2;goto _LL663;}else{goto
_LL664;}_LL664: if(*((int*)_tmp282)== Cyc_Absyn_Throw_e){_LL761: _tmp2F8=((struct
Cyc_Absyn_Throw_e_struct*)_tmp282)->f1;goto _LL665;}else{goto _LL666;}_LL666: if(*((
int*)_tmp282)== Cyc_Absyn_NoInstantiate_e){_LL763: _tmp2FA=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp282)->f1;goto _LL667;}else{goto _LL668;}_LL668: if(*((int*)_tmp282)== Cyc_Absyn_Instantiate_e){
_LL765: _tmp2FC=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp282)->f1;goto _LL669;}
else{goto _LL670;}_LL670: if(*((int*)_tmp282)== Cyc_Absyn_Cast_e){_LL769: _tmp300=(
void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp282)->f1;goto _LL767;_LL767: _tmp2FE=((
struct Cyc_Absyn_Cast_e_struct*)_tmp282)->f2;goto _LL671;}else{goto _LL672;}_LL672:
if(*((int*)_tmp282)== Cyc_Absyn_Address_e){_LL771: _tmp302=((struct Cyc_Absyn_Address_e_struct*)
_tmp282)->f1;goto _LL673;}else{goto _LL674;}_LL674: if(*((int*)_tmp282)== Cyc_Absyn_New_e){
_LL775: _tmp306=((struct Cyc_Absyn_New_e_struct*)_tmp282)->f1;goto _LL773;_LL773:
_tmp304=((struct Cyc_Absyn_New_e_struct*)_tmp282)->f2;goto _LL675;}else{goto _LL676;}
_LL676: if(*((int*)_tmp282)== Cyc_Absyn_Sizeoftyp_e){_LL777: _tmp308=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp282)->f1;goto _LL677;}else{goto _LL678;}
_LL678: if(*((int*)_tmp282)== Cyc_Absyn_Sizeofexp_e){_LL779: _tmp30A=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp282)->f1;goto _LL679;}else{goto _LL680;}_LL680: if(*((int*)_tmp282)== Cyc_Absyn_Offsetof_e){
_LL785: _tmp310=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp282)->f1;goto
_LL781;_LL781: _tmp30C=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp282)->f2;
if(*((int*)_tmp30C)== Cyc_Absyn_StructField){_LL783: _tmp30E=((struct Cyc_Absyn_StructField_struct*)
_tmp30C)->f1;goto _LL681;}else{goto _LL682;}}else{goto _LL682;}_LL682: if(*((int*)
_tmp282)== Cyc_Absyn_Offsetof_e){_LL791: _tmp316=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp282)->f1;goto _LL787;_LL787: _tmp312=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp282)->f2;if(*((int*)_tmp312)== Cyc_Absyn_TupleIndex){_LL789: _tmp314=((struct
Cyc_Absyn_TupleIndex_struct*)_tmp312)->f1;goto _LL683;}else{goto _LL684;}}else{
goto _LL684;}_LL684: if(*((int*)_tmp282)== Cyc_Absyn_Gentyp_e){_LL795: _tmp31A=((
struct Cyc_Absyn_Gentyp_e_struct*)_tmp282)->f1;goto _LL793;_LL793: _tmp318=(void*)((
struct Cyc_Absyn_Gentyp_e_struct*)_tmp282)->f2;goto _LL685;}else{goto _LL686;}
_LL686: if(*((int*)_tmp282)== Cyc_Absyn_Deref_e){_LL797: _tmp31C=((struct Cyc_Absyn_Deref_e_struct*)
_tmp282)->f1;goto _LL687;}else{goto _LL688;}_LL688: if(*((int*)_tmp282)== Cyc_Absyn_StructMember_e){
_LL801: _tmp320=((struct Cyc_Absyn_StructMember_e_struct*)_tmp282)->f1;goto _LL799;
_LL799: _tmp31E=((struct Cyc_Absyn_StructMember_e_struct*)_tmp282)->f2;goto _LL689;}
else{goto _LL690;}_LL690: if(*((int*)_tmp282)== Cyc_Absyn_StructArrow_e){_LL805:
_tmp324=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp282)->f1;goto _LL803;_LL803:
_tmp322=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp282)->f2;goto _LL691;}else{
goto _LL692;}_LL692: if(*((int*)_tmp282)== Cyc_Absyn_Subscript_e){_LL809: _tmp328=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp282)->f1;goto _LL807;_LL807: _tmp326=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp282)->f2;goto _LL693;}else{goto _LL694;}
_LL694: if(*((int*)_tmp282)== Cyc_Absyn_Tuple_e){_LL811: _tmp32A=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp282)->f1;goto _LL695;}else{goto _LL696;}_LL696: if(*((int*)_tmp282)== Cyc_Absyn_CompoundLit_e){
_LL815: _tmp32E=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp282)->f1;goto _LL813;
_LL813: _tmp32C=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp282)->f2;goto _LL697;}
else{goto _LL698;}_LL698: if(*((int*)_tmp282)== Cyc_Absyn_Array_e){_LL817: _tmp330=((
struct Cyc_Absyn_Array_e_struct*)_tmp282)->f1;goto _LL699;}else{goto _LL700;}_LL700:
if(*((int*)_tmp282)== Cyc_Absyn_Comprehension_e){_LL823: _tmp336=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp282)->f1;goto _LL821;_LL821: _tmp334=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp282)->f2;goto _LL819;_LL819: _tmp332=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp282)->f3;goto _LL701;}else{goto _LL702;}_LL702: if(*((int*)_tmp282)== Cyc_Absyn_Struct_e){
_LL827: _tmp33A=((struct Cyc_Absyn_Struct_e_struct*)_tmp282)->f1;goto _LL825;_LL825:
_tmp338=((struct Cyc_Absyn_Struct_e_struct*)_tmp282)->f3;goto _LL703;}else{goto
_LL704;}_LL704: if(*((int*)_tmp282)== Cyc_Absyn_AnonStruct_e){_LL829: _tmp33C=((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp282)->f2;goto _LL705;}else{goto _LL706;}
_LL706: if(*((int*)_tmp282)== Cyc_Absyn_Tunion_e){_LL833: _tmp340=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp282)->f3;goto _LL831;_LL831: _tmp33E=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp282)->f5;goto _LL707;}else{goto _LL708;}_LL708: if(*((int*)_tmp282)== Cyc_Absyn_Enum_e){
_LL835: _tmp342=((struct Cyc_Absyn_Enum_e_struct*)_tmp282)->f1;goto _LL709;}else{
goto _LL710;}_LL710: if(*((int*)_tmp282)== Cyc_Absyn_AnonEnum_e){_LL837: _tmp344=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp282)->f1;goto _LL711;}else{goto _LL712;}
_LL712: if(*((int*)_tmp282)== Cyc_Absyn_Malloc_e){_LL839: _tmp346=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp282)->f1;_LL847: _tmp34E=_tmp346.is_calloc;goto _LL845;_LL845: _tmp34C=_tmp346.rgn;
goto _LL843;_LL843: _tmp34A=_tmp346.elt_type;goto _LL841;_LL841: _tmp348=_tmp346.num_elts;
goto _LL713;}else{goto _LL714;}_LL714: if(*((int*)_tmp282)== Cyc_Absyn_UnresolvedMem_e){
_LL851: _tmp352=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp282)->f1;goto _LL849;
_LL849: _tmp350=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp282)->f2;goto _LL715;}
else{goto _LL716;}_LL716: if(*((int*)_tmp282)== Cyc_Absyn_StmtExp_e){_LL853:
_tmp354=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp282)->f1;goto _LL717;}else{goto
_LL718;}_LL718: if(*((int*)_tmp282)== Cyc_Absyn_Codegen_e){_LL855: _tmp356=((
struct Cyc_Absyn_Codegen_e_struct*)_tmp282)->f1;goto _LL719;}else{goto _LL720;}
_LL720: if(*((int*)_tmp282)== Cyc_Absyn_Fill_e){_LL857: _tmp358=((struct Cyc_Absyn_Fill_e_struct*)
_tmp282)->f1;goto _LL721;}else{goto _LL643;}_LL645: s=Cyc_Absynpp_cnst2doc(_tmp2D2);
goto _LL643;_LL647: _tmp2D6=_tmp2D4;goto _LL649;_LL649: s=Cyc_Absynpp_qvar2doc(
_tmp2D6);goto _LL643;_LL651: s=Cyc_Absynpp_primapp2doc(myprec,_tmp2DA,_tmp2D8);
goto _LL643;_LL653: s=({struct Cyc_PP_Doc*_tmp35A[5];_tmp35A[4]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp2DC);_tmp35A[3]=Cyc_PP_text(_tag_arr("= ",sizeof(unsigned char),3));
_tmp35A[2]=_tmp2DE == 0? Cyc_PP_text(_tag_arr("",sizeof(unsigned char),1)): Cyc_Absynpp_prim2doc((
void*)_tmp2DE->v);_tmp35A[1]=Cyc_PP_text(_tag_arr(" ",sizeof(unsigned char),2));
_tmp35A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2E0);Cyc_PP_cat(_tag_arr(_tmp35A,
sizeof(struct Cyc_PP_Doc*),5));});goto _LL643;_LL655: {struct Cyc_PP_Doc*_tmp35B=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2E4);{void*_tmp35C=_tmp2E2;_LL862: if(_tmp35C
== (void*)Cyc_Absyn_PreInc){goto _LL863;}else{goto _LL864;}_LL864: if(_tmp35C == (
void*)Cyc_Absyn_PreDec){goto _LL865;}else{goto _LL866;}_LL866: if(_tmp35C == (void*)
Cyc_Absyn_PostInc){goto _LL867;}else{goto _LL868;}_LL868: if(_tmp35C == (void*)Cyc_Absyn_PostDec){
goto _LL869;}else{goto _LL861;}_LL863: s=({struct Cyc_PP_Doc*_tmp366[2];_tmp366[1]=
_tmp35B;_tmp366[0]=Cyc_PP_text(_tag_arr("++",sizeof(unsigned char),3));Cyc_PP_cat(
_tag_arr(_tmp366,sizeof(struct Cyc_PP_Doc*),2));});goto _LL861;_LL865: s=({struct
Cyc_PP_Doc*_tmp367[2];_tmp367[1]=_tmp35B;_tmp367[0]=Cyc_PP_text(_tag_arr("--",
sizeof(unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp367,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL861;_LL867: s=({struct Cyc_PP_Doc*_tmp368[2];_tmp368[1]=Cyc_PP_text(
_tag_arr("++",sizeof(unsigned char),3));_tmp368[0]=_tmp35B;Cyc_PP_cat(_tag_arr(
_tmp368,sizeof(struct Cyc_PP_Doc*),2));});goto _LL861;_LL869: s=({struct Cyc_PP_Doc*
_tmp369[2];_tmp369[1]=Cyc_PP_text(_tag_arr("--",sizeof(unsigned char),3));
_tmp369[0]=_tmp35B;Cyc_PP_cat(_tag_arr(_tmp369,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL861;_LL861:;}goto _LL643;}_LL657:{struct _tuple8 _tmp36B=({struct _tuple8
_tmp36A;_tmp36A.f1=(void*)_tmp2E8->r;_tmp36A.f2=(void*)_tmp2E6->r;_tmp36A;});
_LL877: goto _LL878;_LL878: s=({struct Cyc_PP_Doc*_tmp36F[5];_tmp36F[4]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp2E6);_tmp36F[3]=Cyc_PP_text(_tag_arr(" : ",sizeof(unsigned char),4));
_tmp36F[2]=Cyc_Absynpp_exp2doc_prec(0,_tmp2E8);_tmp36F[1]=Cyc_PP_text(_tag_arr(" ? ",
sizeof(unsigned char),4));_tmp36F[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2EA);
Cyc_PP_cat(_tag_arr(_tmp36F,sizeof(struct Cyc_PP_Doc*),5));});goto _LL876;_LL876:;}
goto _LL643;_LL659: s=({struct Cyc_PP_Doc*_tmp370[5];_tmp370[4]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp370[3]=Cyc_Absynpp_exp2doc(_tmp2EC);
_tmp370[2]=Cyc_PP_text(_tag_arr(", ",sizeof(unsigned char),3));_tmp370[1]=Cyc_Absynpp_exp2doc(
_tmp2EE);_tmp370[0]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp370,sizeof(struct Cyc_PP_Doc*),5));});goto _LL643;_LL661: s=({struct
Cyc_PP_Doc*_tmp371[4];_tmp371[3]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),
2));_tmp371[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp2F0);_tmp371[1]=Cyc_PP_text(
_tag_arr("(",sizeof(unsigned char),2));_tmp371[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp2F2);Cyc_PP_cat(_tag_arr(_tmp371,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL643;_LL663: s=({struct Cyc_PP_Doc*_tmp372[4];_tmp372[3]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp372[2]=Cyc_Absynpp_exps2doc_prec(20,
_tmp2F4);_tmp372[1]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));_tmp372[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2F6);Cyc_PP_cat(_tag_arr(_tmp372,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL643;_LL665: s=({struct Cyc_PP_Doc*_tmp373[2];
_tmp373[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp2F8);_tmp373[0]=Cyc_PP_text(
_tag_arr("throw ",sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(_tmp373,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL643;_LL667: s=Cyc_Absynpp_exp2doc_prec(inprec,
_tmp2FA);goto _LL643;_LL669: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp2FC);goto _LL643;
_LL671: s=({struct Cyc_PP_Doc*_tmp374[4];_tmp374[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp2FE);_tmp374[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));
_tmp374[1]=Cyc_Absynpp_typ2doc(_tmp300);_tmp374[0]=Cyc_PP_text(_tag_arr("(",
sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp374,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL643;_LL673: s=({struct Cyc_PP_Doc*_tmp375[2];_tmp375[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp302);_tmp375[0]=Cyc_PP_text(_tag_arr("&",sizeof(unsigned char),2));
Cyc_PP_cat(_tag_arr(_tmp375,sizeof(struct Cyc_PP_Doc*),2));});goto _LL643;_LL675:
if(_tmp306 == 0){s=({struct Cyc_PP_Doc*_tmp376[2];_tmp376[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp304);_tmp376[0]=Cyc_PP_text(_tag_arr("new ",sizeof(unsigned char),5));
Cyc_PP_cat(_tag_arr(_tmp376,sizeof(struct Cyc_PP_Doc*),2));});}else{s=({struct Cyc_PP_Doc*
_tmp377[4];_tmp377[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp304);_tmp377[2]=Cyc_PP_text(
_tag_arr(") ",sizeof(unsigned char),3));_tmp377[1]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(_tmp306));_tmp377[0]=Cyc_PP_text(_tag_arr("rnew(",
sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp377,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL643;_LL677: s=({struct Cyc_PP_Doc*_tmp378[3];_tmp378[2]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp378[1]=Cyc_Absynpp_typ2doc(_tmp308);
_tmp378[0]=Cyc_PP_text(_tag_arr("sizeof(",sizeof(unsigned char),8));Cyc_PP_cat(
_tag_arr(_tmp378,sizeof(struct Cyc_PP_Doc*),3));});goto _LL643;_LL679: s=({struct
Cyc_PP_Doc*_tmp379[3];_tmp379[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),
2));_tmp379[1]=Cyc_Absynpp_exp2doc(_tmp30A);_tmp379[0]=Cyc_PP_text(_tag_arr("sizeof(",
sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp379,sizeof(struct Cyc_PP_Doc*),
3));});goto _LL643;_LL681: s=({struct Cyc_PP_Doc*_tmp37A[5];_tmp37A[4]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp37A[3]=Cyc_PP_textptr(_tmp30E);
_tmp37A[2]=Cyc_PP_text(_tag_arr(",",sizeof(unsigned char),2));_tmp37A[1]=Cyc_Absynpp_typ2doc(
_tmp310);_tmp37A[0]=Cyc_PP_text(_tag_arr("offsetof(",sizeof(unsigned char),10));
Cyc_PP_cat(_tag_arr(_tmp37A,sizeof(struct Cyc_PP_Doc*),5));});goto _LL643;_LL683: s=({
struct Cyc_PP_Doc*_tmp37B[5];_tmp37B[4]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp37B[3]=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp37D;_tmp37D.tag=Cyc_Std_Int_pa;_tmp37D.f1=(int)_tmp314;{void*_tmp37C[1]={&
_tmp37D};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp37C,
sizeof(void*),1));}}));_tmp37B[2]=Cyc_PP_text(_tag_arr(",",sizeof(unsigned char),
2));_tmp37B[1]=Cyc_Absynpp_typ2doc(_tmp316);_tmp37B[0]=Cyc_PP_text(_tag_arr("offsetof(",
sizeof(unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp37B,sizeof(struct Cyc_PP_Doc*),
5));});goto _LL643;_LL685: s=({struct Cyc_PP_Doc*_tmp37E[4];_tmp37E[3]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp37E[2]=Cyc_Absynpp_typ2doc(_tmp318);
_tmp37E[1]=Cyc_Absynpp_tvars2doc(_tmp31A);_tmp37E[0]=Cyc_PP_text(_tag_arr("__gen(",
sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(_tmp37E,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL643;_LL687: s=({struct Cyc_PP_Doc*_tmp37F[2];_tmp37F[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp31C);_tmp37F[0]=Cyc_PP_text(_tag_arr("*",sizeof(unsigned char),2));
Cyc_PP_cat(_tag_arr(_tmp37F,sizeof(struct Cyc_PP_Doc*),2));});goto _LL643;_LL689: s=({
struct Cyc_PP_Doc*_tmp380[3];_tmp380[2]=Cyc_PP_textptr(_tmp31E);_tmp380[1]=Cyc_PP_text(
_tag_arr(".",sizeof(unsigned char),2));_tmp380[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp320);Cyc_PP_cat(_tag_arr(_tmp380,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL643;_LL691: s=({struct Cyc_PP_Doc*_tmp381[3];_tmp381[2]=Cyc_PP_textptr(
_tmp322);_tmp381[1]=Cyc_PP_text(_tag_arr("->",sizeof(unsigned char),3));_tmp381[
0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp324);Cyc_PP_cat(_tag_arr(_tmp381,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL643;_LL693: s=({struct Cyc_PP_Doc*_tmp382[4];
_tmp382[3]=Cyc_PP_text(_tag_arr("]",sizeof(unsigned char),2));_tmp382[2]=Cyc_Absynpp_exp2doc(
_tmp326);_tmp382[1]=Cyc_PP_text(_tag_arr("[",sizeof(unsigned char),2));_tmp382[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp328);Cyc_PP_cat(_tag_arr(_tmp382,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL643;_LL695: s=({struct Cyc_PP_Doc*_tmp383[3];
_tmp383[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp383[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp32A);_tmp383[0]=Cyc_PP_text(_tag_arr("$(",sizeof(unsigned char),3));Cyc_PP_cat(
_tag_arr(_tmp383,sizeof(struct Cyc_PP_Doc*),3));});goto _LL643;_LL697: s=({struct
Cyc_PP_Doc*_tmp384[4];_tmp384[3]=Cyc_PP_group(_tag_arr("{",sizeof(unsigned char),
2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple6*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp32C));_tmp384[2]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp384[1]=Cyc_Absynpp_typ2doc((*_tmp32E).f3);_tmp384[
0]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(
_tmp384,sizeof(struct Cyc_PP_Doc*),4));});goto _LL643;_LL699: s=Cyc_PP_group(
_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp330));goto _LL643;_LL701: s=({struct Cyc_PP_Doc*_tmp385[7];_tmp385[6]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp385[5]=Cyc_Absynpp_exp2doc(_tmp332);
_tmp385[4]=Cyc_PP_text(_tag_arr(" : ",sizeof(unsigned char),4));_tmp385[3]=Cyc_Absynpp_exp2doc(
_tmp334);_tmp385[2]=Cyc_PP_text(_tag_arr(" < ",sizeof(unsigned char),4));_tmp385[
1]=Cyc_PP_text(*(*_tmp336->name).f2);_tmp385[0]=Cyc_PP_text(_tag_arr("{for ",
sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp385,sizeof(struct Cyc_PP_Doc*),
7));});goto _LL643;_LL703: s=({struct Cyc_PP_Doc*_tmp386[2];_tmp386[1]=Cyc_PP_group(
_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp338));_tmp386[0]=Cyc_Absynpp_qvar2doc(_tmp33A);Cyc_PP_cat(_tag_arr(_tmp386,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL643;_LL705: s=Cyc_PP_group(_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_tuple6*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp33C));goto
_LL643;_LL707: if(_tmp340 == 0){s=Cyc_Absynpp_qvar2doc(_tmp33E->name);}else{s=({
struct Cyc_PP_Doc*_tmp387[2];_tmp387[1]=Cyc_PP_egroup(_tag_arr("(",sizeof(
unsigned char),2),_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp340));_tmp387[0]=
Cyc_Absynpp_qvar2doc(_tmp33E->name);Cyc_PP_cat(_tag_arr(_tmp387,sizeof(struct Cyc_PP_Doc*),
2));});}goto _LL643;_LL709: s=Cyc_Absynpp_qvar2doc(_tmp342);goto _LL643;_LL711: s=
Cyc_Absynpp_qvar2doc(_tmp344);goto _LL643;_LL713: if(_tmp34E){struct Cyc_Absyn_Exp*
st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp34A)),0);if(_tmp34C == 0){s=({
struct Cyc_PP_Doc*_tmp388[5];_tmp388[4]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp388[3]=Cyc_Absynpp_exp2doc(st);_tmp388[2]=Cyc_PP_text(
_tag_arr(",",sizeof(unsigned char),2));_tmp388[1]=Cyc_Absynpp_exp2doc(_tmp348);
_tmp388[0]=Cyc_PP_text(_tag_arr("calloc(",sizeof(unsigned char),8));Cyc_PP_cat(
_tag_arr(_tmp388,sizeof(struct Cyc_PP_Doc*),5));});}else{s=({struct Cyc_PP_Doc*
_tmp389[7];_tmp389[6]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp389[
5]=Cyc_Absynpp_exp2doc(st);_tmp389[4]=Cyc_PP_text(_tag_arr(",",sizeof(
unsigned char),2));_tmp389[3]=Cyc_Absynpp_exp2doc(_tmp348);_tmp389[2]=Cyc_PP_text(
_tag_arr(",",sizeof(unsigned char),2));_tmp389[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmp34C));_tmp389[0]=Cyc_PP_text(_tag_arr("rcalloc(",sizeof(
unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp389,sizeof(struct Cyc_PP_Doc*),7));});}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp34A == 0){new_e=_tmp348;}else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp34A,0),_tmp348,0);}if(_tmp34C == 0){s=({struct Cyc_PP_Doc*
_tmp38A[3];_tmp38A[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp38A[
1]=Cyc_Absynpp_exp2doc(new_e);_tmp38A[0]=Cyc_PP_text(_tag_arr("malloc(",sizeof(
unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp38A,sizeof(struct Cyc_PP_Doc*),3));});}
else{s=({struct Cyc_PP_Doc*_tmp38B[5];_tmp38B[4]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp38B[3]=Cyc_Absynpp_exp2doc(new_e);_tmp38B[2]=Cyc_PP_text(
_tag_arr(",",sizeof(unsigned char),2));_tmp38B[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmp34C));_tmp38B[0]=Cyc_PP_text(_tag_arr("rmalloc(",sizeof(
unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp38B,sizeof(struct Cyc_PP_Doc*),5));});}}
goto _LL643;_LL715: s=Cyc_PP_group(_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp350));goto _LL643;_LL717: s=({struct Cyc_PP_Doc*_tmp38C[3];_tmp38C[2]=Cyc_PP_text(
_tag_arr(" })",sizeof(unsigned char),4));_tmp38C[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp354));_tmp38C[0]=Cyc_PP_text(_tag_arr("({ ",sizeof(unsigned char),4));Cyc_PP_cat(
_tag_arr(_tmp38C,sizeof(struct Cyc_PP_Doc*),3));});goto _LL643;_LL719: s=({struct
Cyc_PP_Doc*_tmp38D[3];_tmp38D[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),
2));_tmp38D[1]=Cyc_PP_nest(2,Cyc_Absynpp_decl2doc(({struct Cyc_Absyn_Decl*_tmp38E=
_cycalloc(sizeof(struct Cyc_Absyn_Decl));_tmp38E->r=(void*)((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp38F=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_struct));_tmp38F[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp390;_tmp390.tag=Cyc_Absyn_Fn_d;_tmp390.f1=_tmp356;_tmp390;});_tmp38F;}));
_tmp38E->loc=e->loc;_tmp38E;})));_tmp38D[0]=Cyc_PP_text(_tag_arr("codegen(",
sizeof(unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp38D,sizeof(struct Cyc_PP_Doc*),
3));});goto _LL643;_LL721: s=({struct Cyc_PP_Doc*_tmp391[3];_tmp391[2]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp391[1]=Cyc_PP_nest(2,Cyc_Absynpp_exp2doc(
_tmp358));_tmp391[0]=Cyc_PP_text(_tag_arr("fill(",sizeof(unsigned char),6));Cyc_PP_cat(
_tag_arr(_tmp391,sizeof(struct Cyc_PP_Doc*),3));});goto _LL643;_LL643:;}if(inprec
>= myprec){s=({struct Cyc_PP_Doc*_tmp392[3];_tmp392[2]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp392[1]=s;_tmp392[0]=Cyc_PP_text(_tag_arr("(",
sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp392,sizeof(struct Cyc_PP_Doc*),
3));});}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){void*
_tmp393=d;struct Cyc_Absyn_Exp*_tmp399;struct _tagged_arr*_tmp39B;_LL917: if(*((int*)
_tmp393)== Cyc_Absyn_ArrayElement){_LL922: _tmp399=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp393)->f1;goto _LL918;}else{goto _LL919;}_LL919: if(*((int*)_tmp393)== Cyc_Absyn_FieldName){
_LL924: _tmp39B=((struct Cyc_Absyn_FieldName_struct*)_tmp393)->f1;goto _LL920;}
else{goto _LL916;}_LL918: return({struct Cyc_PP_Doc*_tmp39D[3];_tmp39D[2]=Cyc_PP_text(
_tag_arr("]",sizeof(unsigned char),2));_tmp39D[1]=Cyc_Absynpp_exp2doc(_tmp399);
_tmp39D[0]=Cyc_PP_text(_tag_arr(".[",sizeof(unsigned char),3));Cyc_PP_cat(
_tag_arr(_tmp39D,sizeof(struct Cyc_PP_Doc*),3));});_LL920: return({struct Cyc_PP_Doc*
_tmp39E[2];_tmp39E[1]=Cyc_PP_textptr(_tmp39B);_tmp39E[0]=Cyc_PP_text(_tag_arr(".",
sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp39E,sizeof(struct Cyc_PP_Doc*),
2));});_LL916:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple6*de){if((*de).f1
== 0){return Cyc_Absynpp_exp2doc((*de).f2);}else{return({struct Cyc_PP_Doc*_tmp39F[
2];_tmp39F[1]=Cyc_Absynpp_exp2doc((*de).f2);_tmp39F[0]=Cyc_PP_egroup(_tag_arr("",
sizeof(unsigned char),1),_tag_arr("=",sizeof(unsigned char),2),_tag_arr("=",
sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));Cyc_PP_cat(
_tag_arr(_tmp39F,sizeof(struct Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(
int inprec,struct Cyc_List_List*es){return Cyc_PP_group(_tag_arr("",sizeof(
unsigned char),1),_tag_arr("",sizeof(unsigned char),1),_tag_arr(",",sizeof(
unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),
int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*c){void*_tmp3A0=c;unsigned char
_tmp3B2;void*_tmp3B4;short _tmp3B6;void*_tmp3B8;int _tmp3BA;void*_tmp3BC;int
_tmp3BE;void*_tmp3C0;long long _tmp3C2;void*_tmp3C4;struct _tagged_arr _tmp3C6;
struct _tagged_arr _tmp3C8;_LL930: if((unsigned int)_tmp3A0 > 1?*((int*)_tmp3A0)== 
Cyc_Absyn_Char_c: 0){_LL949: _tmp3B4=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp3A0)->f1;goto _LL947;_LL947: _tmp3B2=((struct Cyc_Absyn_Char_c_struct*)_tmp3A0)->f2;
goto _LL931;}else{goto _LL932;}_LL932: if((unsigned int)_tmp3A0 > 1?*((int*)_tmp3A0)
== Cyc_Absyn_Short_c: 0){_LL953: _tmp3B8=(void*)((struct Cyc_Absyn_Short_c_struct*)
_tmp3A0)->f1;goto _LL951;_LL951: _tmp3B6=((struct Cyc_Absyn_Short_c_struct*)_tmp3A0)->f2;
goto _LL933;}else{goto _LL934;}_LL934: if((unsigned int)_tmp3A0 > 1?*((int*)_tmp3A0)
== Cyc_Absyn_Int_c: 0){_LL957: _tmp3BC=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp3A0)->f1;if(_tmp3BC == (void*)Cyc_Absyn_Signed){goto _LL955;}else{goto _LL936;}
_LL955: _tmp3BA=((struct Cyc_Absyn_Int_c_struct*)_tmp3A0)->f2;goto _LL935;}else{
goto _LL936;}_LL936: if((unsigned int)_tmp3A0 > 1?*((int*)_tmp3A0)== Cyc_Absyn_Int_c:
0){_LL961: _tmp3C0=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp3A0)->f1;if(_tmp3C0
== (void*)Cyc_Absyn_Unsigned){goto _LL959;}else{goto _LL938;}_LL959: _tmp3BE=((
struct Cyc_Absyn_Int_c_struct*)_tmp3A0)->f2;goto _LL937;}else{goto _LL938;}_LL938:
if((unsigned int)_tmp3A0 > 1?*((int*)_tmp3A0)== Cyc_Absyn_LongLong_c: 0){_LL965:
_tmp3C4=(void*)((struct Cyc_Absyn_LongLong_c_struct*)_tmp3A0)->f1;goto _LL963;
_LL963: _tmp3C2=((struct Cyc_Absyn_LongLong_c_struct*)_tmp3A0)->f2;goto _LL939;}
else{goto _LL940;}_LL940: if((unsigned int)_tmp3A0 > 1?*((int*)_tmp3A0)== Cyc_Absyn_Float_c:
0){_LL967: _tmp3C6=((struct Cyc_Absyn_Float_c_struct*)_tmp3A0)->f1;goto _LL941;}
else{goto _LL942;}_LL942: if(_tmp3A0 == (void*)Cyc_Absyn_Null_c){goto _LL943;}else{
goto _LL944;}_LL944: if((unsigned int)_tmp3A0 > 1?*((int*)_tmp3A0)== Cyc_Absyn_String_c:
0){_LL969: _tmp3C8=((struct Cyc_Absyn_String_c_struct*)_tmp3A0)->f1;goto _LL945;}
else{goto _LL929;}_LL931: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp3CB;_tmp3CB.tag=Cyc_Std_String_pa;_tmp3CB.f1=(struct _tagged_arr)Cyc_Absynpp_char_escape(
_tmp3B2);{void*_tmp3CA[1]={& _tmp3CB};Cyc_Std_aprintf(_tag_arr("'%s'",sizeof(
unsigned char),5),_tag_arr(_tmp3CA,sizeof(void*),1));}}));_LL933: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp3CD;_tmp3CD.tag=Cyc_Std_Int_pa;
_tmp3CD.f1=(int)((unsigned int)((int)_tmp3B6));{void*_tmp3CC[1]={& _tmp3CD};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp3CC,sizeof(void*),1));}}));
_LL935: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp3CF;
_tmp3CF.tag=Cyc_Std_Int_pa;_tmp3CF.f1=(int)((unsigned int)_tmp3BA);{void*_tmp3CE[
1]={& _tmp3CF};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(
_tmp3CE,sizeof(void*),1));}}));_LL937: return Cyc_PP_text((struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp3D1;_tmp3D1.tag=Cyc_Std_Int_pa;_tmp3D1.f1=(
unsigned int)_tmp3BE;{void*_tmp3D0[1]={& _tmp3D1};Cyc_Std_aprintf(_tag_arr("%u",
sizeof(unsigned char),3),_tag_arr(_tmp3D0,sizeof(void*),1));}}));_LL939: return
Cyc_PP_text(_tag_arr("<<FIX LONG LONG CONSTANT>>",sizeof(unsigned char),27));
_LL941: return Cyc_PP_text(_tmp3C6);_LL943: return Cyc_PP_text(_tag_arr("NULL",
sizeof(unsigned char),5));_LL945: return({struct Cyc_PP_Doc*_tmp3D2[3];_tmp3D2[2]=
Cyc_PP_text(_tag_arr("\"",sizeof(unsigned char),2));_tmp3D2[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(
_tmp3C8));_tmp3D2[0]=Cyc_PP_text(_tag_arr("\"",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp3D2,sizeof(struct Cyc_PP_Doc*),3));});_LL929:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(
int inprec,void*p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(
p);if(p == (void*)Cyc_Absyn_Size){if(es == 0? 1: es->tl != 0){(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp3D3=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp3D3[0]=({struct Cyc_Core_Failure_struct _tmp3D4;_tmp3D4.tag=Cyc_Core_Failure;
_tmp3D4.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp3D6;_tmp3D6.tag=
Cyc_Std_String_pa;_tmp3D6.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp3D5[1]={& _tmp3D6};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof(unsigned char),44),_tag_arr(_tmp3D5,sizeof(void*),1));}});_tmp3D4;});
_tmp3D3;}));}{struct Cyc_PP_Doc*_tmp3D7=Cyc_Absynpp_exp2doc_prec(inprec,(struct
Cyc_Absyn_Exp*)es->hd);return({struct Cyc_PP_Doc*_tmp3D8[2];_tmp3D8[1]=Cyc_PP_text(
_tag_arr(".size",sizeof(unsigned char),6));_tmp3D8[0]=_tmp3D7;Cyc_PP_cat(
_tag_arr(_tmp3D8,sizeof(struct Cyc_PP_Doc*),2));});}}else{struct Cyc_List_List*ds=((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);if(ds
== 0){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp3D9=_cycalloc(
sizeof(struct Cyc_Core_Failure_struct));_tmp3D9[0]=({struct Cyc_Core_Failure_struct
_tmp3DA;_tmp3DA.tag=Cyc_Core_Failure;_tmp3DA.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp3DC;_tmp3DC.tag=Cyc_Std_String_pa;_tmp3DC.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(
ps,72);{void*_tmp3DB[1]={& _tmp3DC};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc: %s with no args",
sizeof(unsigned char),38),_tag_arr(_tmp3DB,sizeof(void*),1));}});_tmp3DA;});
_tmp3D9;}));}else{if(ds->tl == 0){return({struct Cyc_PP_Doc*_tmp3DD[3];_tmp3DD[2]=(
struct Cyc_PP_Doc*)ds->hd;_tmp3DD[1]=Cyc_PP_text(_tag_arr(" ",sizeof(
unsigned char),2));_tmp3DD[0]=ps;Cyc_PP_cat(_tag_arr(_tmp3DD,sizeof(struct Cyc_PP_Doc*),
3));});}else{if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp3DE=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp3DE[0]=({struct Cyc_Core_Failure_struct _tmp3DF;_tmp3DF.tag=Cyc_Core_Failure;
_tmp3DF.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp3E1;_tmp3E1.tag=
Cyc_Std_String_pa;_tmp3E1.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp3E0[1]={& _tmp3E1};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof(unsigned char),47),_tag_arr(_tmp3E0,sizeof(void*),1));}});_tmp3DF;});
_tmp3DE;}));}else{return({struct Cyc_PP_Doc*_tmp3E2[5];_tmp3E2[4]=(struct Cyc_PP_Doc*)((
struct Cyc_List_List*)_check_null(ds->tl))->hd;_tmp3E2[3]=Cyc_PP_text(_tag_arr(" ",
sizeof(unsigned char),2));_tmp3E2[2]=ps;_tmp3E2[1]=Cyc_PP_text(_tag_arr(" ",
sizeof(unsigned char),2));_tmp3E2[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(
_tag_arr(_tmp3E2,sizeof(struct Cyc_PP_Doc*),5));});}}}}}struct _tagged_arr Cyc_Absynpp_prim2str(
void*p){void*_tmp3E3=p;_LL997: if(_tmp3E3 == (void*)Cyc_Absyn_Plus){goto _LL998;}
else{goto _LL999;}_LL999: if(_tmp3E3 == (void*)Cyc_Absyn_Times){goto _LL1000;}else{
goto _LL1001;}_LL1001: if(_tmp3E3 == (void*)Cyc_Absyn_Minus){goto _LL1002;}else{goto
_LL1003;}_LL1003: if(_tmp3E3 == (void*)Cyc_Absyn_Div){goto _LL1004;}else{goto
_LL1005;}_LL1005: if(_tmp3E3 == (void*)Cyc_Absyn_Mod){goto _LL1006;}else{goto
_LL1007;}_LL1007: if(_tmp3E3 == (void*)Cyc_Absyn_Eq){goto _LL1008;}else{goto _LL1009;}
_LL1009: if(_tmp3E3 == (void*)Cyc_Absyn_Neq){goto _LL1010;}else{goto _LL1011;}
_LL1011: if(_tmp3E3 == (void*)Cyc_Absyn_Gt){goto _LL1012;}else{goto _LL1013;}_LL1013:
if(_tmp3E3 == (void*)Cyc_Absyn_Lt){goto _LL1014;}else{goto _LL1015;}_LL1015: if(
_tmp3E3 == (void*)Cyc_Absyn_Gte){goto _LL1016;}else{goto _LL1017;}_LL1017: if(
_tmp3E3 == (void*)Cyc_Absyn_Lte){goto _LL1018;}else{goto _LL1019;}_LL1019: if(
_tmp3E3 == (void*)Cyc_Absyn_Not){goto _LL1020;}else{goto _LL1021;}_LL1021: if(
_tmp3E3 == (void*)Cyc_Absyn_Bitnot){goto _LL1022;}else{goto _LL1023;}_LL1023: if(
_tmp3E3 == (void*)Cyc_Absyn_Bitand){goto _LL1024;}else{goto _LL1025;}_LL1025: if(
_tmp3E3 == (void*)Cyc_Absyn_Bitor){goto _LL1026;}else{goto _LL1027;}_LL1027: if(
_tmp3E3 == (void*)Cyc_Absyn_Bitxor){goto _LL1028;}else{goto _LL1029;}_LL1029: if(
_tmp3E3 == (void*)Cyc_Absyn_Bitlshift){goto _LL1030;}else{goto _LL1031;}_LL1031: if(
_tmp3E3 == (void*)Cyc_Absyn_Bitlrshift){goto _LL1032;}else{goto _LL1033;}_LL1033:
if(_tmp3E3 == (void*)Cyc_Absyn_Bitarshift){goto _LL1034;}else{goto _LL1035;}_LL1035:
if(_tmp3E3 == (void*)Cyc_Absyn_Size){goto _LL1036;}else{goto _LL996;}_LL998: return
_tag_arr("+",sizeof(unsigned char),2);_LL1000: return _tag_arr("*",sizeof(
unsigned char),2);_LL1002: return _tag_arr("-",sizeof(unsigned char),2);_LL1004:
return _tag_arr("/",sizeof(unsigned char),2);_LL1006: return _tag_arr("%",sizeof(
unsigned char),2);_LL1008: return _tag_arr("==",sizeof(unsigned char),3);_LL1010:
return _tag_arr("!=",sizeof(unsigned char),3);_LL1012: return _tag_arr(">",sizeof(
unsigned char),2);_LL1014: return _tag_arr("<",sizeof(unsigned char),2);_LL1016:
return _tag_arr(">=",sizeof(unsigned char),3);_LL1018: return _tag_arr("<=",sizeof(
unsigned char),3);_LL1020: return _tag_arr("!",sizeof(unsigned char),2);_LL1022:
return _tag_arr("~",sizeof(unsigned char),2);_LL1024: return _tag_arr("&",sizeof(
unsigned char),2);_LL1026: return _tag_arr("|",sizeof(unsigned char),2);_LL1028:
return _tag_arr("^",sizeof(unsigned char),2);_LL1030: return _tag_arr("<<",sizeof(
unsigned char),3);_LL1032: return _tag_arr(">>",sizeof(unsigned char),3);_LL1034:
return _tag_arr(">>>",sizeof(unsigned char),4);_LL1036: return _tag_arr("size",
sizeof(unsigned char),5);_LL996:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}struct _tuple9{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_structarg2doc(struct
_tuple9*t){if((*t).f1 == 0){return Cyc_Absynpp_exp2doc((*t).f2);}else{return({
struct Cyc_PP_Doc*_tmp40D[3];_tmp40D[2]=Cyc_Absynpp_exp2doc((*t).f2);_tmp40D[1]=
Cyc_PP_text(_tag_arr(" = ",sizeof(unsigned char),4));_tmp40D[0]=Cyc_PP_textptr((
struct _tagged_arr*)((struct Cyc_Core_Opt*)_check_null((*t).f1))->v);Cyc_PP_cat(
_tag_arr(_tmp40D,sizeof(struct Cyc_PP_Doc*),3));});}}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp40E=(void*)s->r;_LL1040: if((unsigned int)
_tmp40E > 1?*((int*)_tmp40E)== Cyc_Absyn_Decl_s: 0){goto _LL1041;}else{goto _LL1042;}
_LL1042: goto _LL1043;_LL1041: return 1;_LL1043: return 0;_LL1039:;}struct Cyc_PP_Doc*
Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st){struct Cyc_PP_Doc*s;{void*_tmp414=(
void*)st->r;struct Cyc_Absyn_Exp*_tmp442;struct Cyc_Absyn_Stmt*_tmp444;struct Cyc_Absyn_Stmt*
_tmp446;struct Cyc_Absyn_Exp*_tmp448;struct Cyc_Absyn_Stmt*_tmp44A;struct Cyc_Absyn_Stmt*
_tmp44C;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Stmt*_tmp450;struct _tuple2
_tmp452;struct Cyc_Absyn_Exp*_tmp454;struct _tagged_arr*_tmp456;struct Cyc_Absyn_Stmt*
_tmp458;struct _tuple2 _tmp45A;struct Cyc_Absyn_Exp*_tmp45C;struct _tuple2 _tmp45E;
struct Cyc_Absyn_Exp*_tmp460;struct Cyc_Absyn_Exp*_tmp462;struct Cyc_Absyn_ForArrayInfo
_tmp464;struct Cyc_Absyn_Stmt*_tmp466;struct _tuple2 _tmp468;struct Cyc_Absyn_Exp*
_tmp46A;struct _tuple2 _tmp46C;struct Cyc_Absyn_Exp*_tmp46E;struct Cyc_List_List*
_tmp470;struct Cyc_List_List*_tmp472;struct Cyc_Absyn_Exp*_tmp474;struct Cyc_List_List*
_tmp476;struct Cyc_Absyn_Exp*_tmp478;struct Cyc_List_List*_tmp47A;struct Cyc_List_List*
_tmp47C;struct Cyc_Absyn_Stmt*_tmp47E;struct Cyc_Absyn_Decl*_tmp480;struct Cyc_Absyn_Stmt*
_tmp482;struct Cyc_Absyn_Stmt*_tmp484;struct Cyc_Absyn_Stmt*_tmp486;struct
_tagged_arr*_tmp488;struct _tuple2 _tmp48A;struct Cyc_Absyn_Exp*_tmp48C;struct Cyc_Absyn_Stmt*
_tmp48E;struct Cyc_List_List*_tmp490;struct Cyc_Absyn_Stmt*_tmp492;struct Cyc_Absyn_Stmt*
_tmp494;struct Cyc_Absyn_Vardecl*_tmp496;struct Cyc_Absyn_Tvar*_tmp498;_LL1046: if(
_tmp414 == (void*)Cyc_Absyn_Skip_s){goto _LL1047;}else{goto _LL1048;}_LL1048: if((
unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_Exp_s: 0){_LL1091: _tmp442=((
struct Cyc_Absyn_Exp_s_struct*)_tmp414)->f1;goto _LL1049;}else{goto _LL1050;}
_LL1050: if((unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_Seq_s: 0){
_LL1095: _tmp446=((struct Cyc_Absyn_Seq_s_struct*)_tmp414)->f1;goto _LL1093;_LL1093:
_tmp444=((struct Cyc_Absyn_Seq_s_struct*)_tmp414)->f2;goto _LL1051;}else{goto
_LL1052;}_LL1052: if((unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_Return_s:
0){_LL1097: _tmp448=((struct Cyc_Absyn_Return_s_struct*)_tmp414)->f1;goto _LL1053;}
else{goto _LL1054;}_LL1054: if((unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_IfThenElse_s:
0){_LL1103: _tmp44E=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp414)->f1;goto
_LL1101;_LL1101: _tmp44C=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp414)->f2;goto
_LL1099;_LL1099: _tmp44A=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp414)->f3;goto
_LL1055;}else{goto _LL1056;}_LL1056: if((unsigned int)_tmp414 > 1?*((int*)_tmp414)
== Cyc_Absyn_While_s: 0){_LL1107: _tmp452=((struct Cyc_Absyn_While_s_struct*)
_tmp414)->f1;_LL1109: _tmp454=_tmp452.f1;goto _LL1105;_LL1105: _tmp450=((struct Cyc_Absyn_While_s_struct*)
_tmp414)->f2;goto _LL1057;}else{goto _LL1058;}_LL1058: if((unsigned int)_tmp414 > 1?*((
int*)_tmp414)== Cyc_Absyn_Break_s: 0){goto _LL1059;}else{goto _LL1060;}_LL1060: if((
unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_Continue_s: 0){goto _LL1061;}
else{goto _LL1062;}_LL1062: if((unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_Goto_s:
0){_LL1111: _tmp456=((struct Cyc_Absyn_Goto_s_struct*)_tmp414)->f1;goto _LL1063;}
else{goto _LL1064;}_LL1064: if((unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_For_s:
0){_LL1123: _tmp462=((struct Cyc_Absyn_For_s_struct*)_tmp414)->f1;goto _LL1119;
_LL1119: _tmp45E=((struct Cyc_Absyn_For_s_struct*)_tmp414)->f2;_LL1121: _tmp460=
_tmp45E.f1;goto _LL1115;_LL1115: _tmp45A=((struct Cyc_Absyn_For_s_struct*)_tmp414)->f3;
_LL1117: _tmp45C=_tmp45A.f1;goto _LL1113;_LL1113: _tmp458=((struct Cyc_Absyn_For_s_struct*)
_tmp414)->f4;goto _LL1065;}else{goto _LL1066;}_LL1066: if((unsigned int)_tmp414 > 1?*((
int*)_tmp414)== Cyc_Absyn_ForArray_s: 0){_LL1125: _tmp464=((struct Cyc_Absyn_ForArray_s_struct*)
_tmp414)->f1;_LL1137: _tmp470=_tmp464.defns;goto _LL1133;_LL1133: _tmp46C=_tmp464.condition;
_LL1135: _tmp46E=_tmp46C.f1;goto _LL1129;_LL1129: _tmp468=_tmp464.delta;_LL1131:
_tmp46A=_tmp468.f1;goto _LL1127;_LL1127: _tmp466=_tmp464.body;goto _LL1067;}else{
goto _LL1068;}_LL1068: if((unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_Switch_s:
0){_LL1141: _tmp474=((struct Cyc_Absyn_Switch_s_struct*)_tmp414)->f1;goto _LL1139;
_LL1139: _tmp472=((struct Cyc_Absyn_Switch_s_struct*)_tmp414)->f2;goto _LL1069;}
else{goto _LL1070;}_LL1070: if((unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_SwitchC_s:
0){_LL1145: _tmp478=((struct Cyc_Absyn_SwitchC_s_struct*)_tmp414)->f1;goto _LL1143;
_LL1143: _tmp476=((struct Cyc_Absyn_SwitchC_s_struct*)_tmp414)->f2;goto _LL1071;}
else{goto _LL1072;}_LL1072: if((unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_Fallthru_s:
0){_LL1147: _tmp47A=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp414)->f1;if(_tmp47A
== 0){goto _LL1073;}else{goto _LL1074;}}else{goto _LL1074;}_LL1074: if((unsigned int)
_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_Fallthru_s: 0){_LL1149: _tmp47C=((struct
Cyc_Absyn_Fallthru_s_struct*)_tmp414)->f1;goto _LL1075;}else{goto _LL1076;}_LL1076:
if((unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_Decl_s: 0){_LL1153:
_tmp480=((struct Cyc_Absyn_Decl_s_struct*)_tmp414)->f1;goto _LL1151;_LL1151:
_tmp47E=((struct Cyc_Absyn_Decl_s_struct*)_tmp414)->f2;goto _LL1077;}else{goto
_LL1078;}_LL1078: if((unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_Cut_s:
0){_LL1155: _tmp482=((struct Cyc_Absyn_Cut_s_struct*)_tmp414)->f1;goto _LL1079;}
else{goto _LL1080;}_LL1080: if((unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_Splice_s:
0){_LL1157: _tmp484=((struct Cyc_Absyn_Splice_s_struct*)_tmp414)->f1;goto _LL1081;}
else{goto _LL1082;}_LL1082: if((unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_Label_s:
0){_LL1161: _tmp488=((struct Cyc_Absyn_Label_s_struct*)_tmp414)->f1;goto _LL1159;
_LL1159: _tmp486=((struct Cyc_Absyn_Label_s_struct*)_tmp414)->f2;goto _LL1083;}
else{goto _LL1084;}_LL1084: if((unsigned int)_tmp414 > 1?*((int*)_tmp414)== Cyc_Absyn_Do_s:
0){_LL1167: _tmp48E=((struct Cyc_Absyn_Do_s_struct*)_tmp414)->f1;goto _LL1163;
_LL1163: _tmp48A=((struct Cyc_Absyn_Do_s_struct*)_tmp414)->f2;_LL1165: _tmp48C=
_tmp48A.f1;goto _LL1085;}else{goto _LL1086;}_LL1086: if((unsigned int)_tmp414 > 1?*((
int*)_tmp414)== Cyc_Absyn_TryCatch_s: 0){_LL1171: _tmp492=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp414)->f1;goto _LL1169;_LL1169: _tmp490=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp414)->f2;goto _LL1087;}else{goto _LL1088;}_LL1088: if((unsigned int)_tmp414 > 1?*((
int*)_tmp414)== Cyc_Absyn_Region_s: 0){_LL1177: _tmp498=((struct Cyc_Absyn_Region_s_struct*)
_tmp414)->f1;goto _LL1175;_LL1175: _tmp496=((struct Cyc_Absyn_Region_s_struct*)
_tmp414)->f2;goto _LL1173;_LL1173: _tmp494=((struct Cyc_Absyn_Region_s_struct*)
_tmp414)->f3;goto _LL1089;}else{goto _LL1045;}_LL1047: s=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));goto _LL1045;_LL1049: s=({struct Cyc_PP_Doc*_tmp49A[2];
_tmp49A[1]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmp49A[0]=Cyc_Absynpp_exp2doc(
_tmp442);Cyc_PP_cat(_tag_arr(_tmp49A,sizeof(struct Cyc_PP_Doc*),2));});goto
_LL1045;_LL1051: if(Cyc_Absynpp_decls_first){if(Cyc_Absynpp_is_declaration(
_tmp446)){s=({struct Cyc_PP_Doc*_tmp49B[6];_tmp49B[5]=Cyc_Absynpp_is_declaration(
_tmp444)?({struct Cyc_PP_Doc*_tmp49C[4];_tmp49C[3]=Cyc_PP_line_doc();_tmp49C[2]=
Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp49C[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp444));_tmp49C[0]=Cyc_PP_text(_tag_arr("{ ",sizeof(unsigned char),3));Cyc_PP_cat(
_tag_arr(_tmp49C,sizeof(struct Cyc_PP_Doc*),4));}): Cyc_Absynpp_stmt2doc(_tmp444);
_tmp49B[4]=Cyc_PP_line_doc();_tmp49B[3]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp49B[2]=Cyc_PP_line_doc();_tmp49B[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp446));_tmp49B[0]=Cyc_PP_text(_tag_arr("{ ",sizeof(unsigned char),3));Cyc_PP_cat(
_tag_arr(_tmp49B,sizeof(struct Cyc_PP_Doc*),6));});}else{if(Cyc_Absynpp_is_declaration(
_tmp444)){s=({struct Cyc_PP_Doc*_tmp49D[6];_tmp49D[5]=Cyc_PP_line_doc();_tmp49D[4]=
Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp49D[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp444));_tmp49D[2]=Cyc_PP_text(_tag_arr("{ ",sizeof(unsigned char),3));_tmp49D[
1]=Cyc_PP_line_doc();_tmp49D[0]=Cyc_Absynpp_stmt2doc(_tmp446);Cyc_PP_cat(
_tag_arr(_tmp49D,sizeof(struct Cyc_PP_Doc*),6));});}else{s=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,_tag_arr("",sizeof(unsigned char),1),({
struct Cyc_List_List*_tmp49E=_cycalloc(sizeof(struct Cyc_List_List));_tmp49E->hd=
_tmp446;_tmp49E->tl=({struct Cyc_List_List*_tmp49F=_cycalloc(sizeof(struct Cyc_List_List));
_tmp49F->hd=_tmp444;_tmp49F->tl=0;_tmp49F;});_tmp49E;}));}}}else{s=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,_tag_arr("",sizeof(unsigned char),1),({
struct Cyc_List_List*_tmp4A0=_cycalloc(sizeof(struct Cyc_List_List));_tmp4A0->hd=
_tmp446;_tmp4A0->tl=({struct Cyc_List_List*_tmp4A1=_cycalloc(sizeof(struct Cyc_List_List));
_tmp4A1->hd=_tmp444;_tmp4A1->tl=0;_tmp4A1;});_tmp4A0;}));}goto _LL1045;_LL1053:
if(_tmp448 == 0){s=Cyc_PP_text(_tag_arr("return;",sizeof(unsigned char),8));}
else{s=({struct Cyc_PP_Doc*_tmp4A2[3];_tmp4A2[2]=Cyc_PP_text(_tag_arr(";",sizeof(
unsigned char),2));_tmp4A2[1]=_tmp448 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(_tmp448));_tmp4A2[0]=Cyc_PP_text(_tag_arr("return ",
sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp4A2,sizeof(struct Cyc_PP_Doc*),
3));});}goto _LL1045;_LL1055: {int print_else;{void*_tmp4A3=(void*)_tmp44A->r;
_LL1189: if(_tmp4A3 == (void*)Cyc_Absyn_Skip_s){goto _LL1190;}else{goto _LL1191;}
_LL1191: goto _LL1192;_LL1190: print_else=0;goto _LL1188;_LL1192: print_else=1;goto
_LL1188;_LL1188:;}s=({struct Cyc_PP_Doc*_tmp4A9[7];_tmp4A9[6]=print_else?({struct
Cyc_PP_Doc*_tmp4AB[5];_tmp4AB[4]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),
2));_tmp4AB[3]=Cyc_PP_line_doc();_tmp4AB[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp4AC[2];_tmp4AC[1]=Cyc_Absynpp_stmt2doc(_tmp44A);_tmp4AC[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp4AC,sizeof(struct Cyc_PP_Doc*),2));}));_tmp4AB[1]=Cyc_PP_text(
_tag_arr("else {",sizeof(unsigned char),7));_tmp4AB[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp4AB,sizeof(struct Cyc_PP_Doc*),5));}): Cyc_PP_nil_doc();_tmp4A9[5]=
Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp4A9[4]=Cyc_PP_line_doc();
_tmp4A9[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp4AA[2];_tmp4AA[1]=Cyc_Absynpp_stmt2doc(
_tmp44C);_tmp4AA[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp4AA,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp4A9[2]=Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),
4));_tmp4A9[1]=Cyc_Absynpp_exp2doc(_tmp44E);_tmp4A9[0]=Cyc_PP_text(_tag_arr("if (",
sizeof(unsigned char),5));Cyc_PP_cat(_tag_arr(_tmp4A9,sizeof(struct Cyc_PP_Doc*),
7));});goto _LL1045;}_LL1057: s=({struct Cyc_PP_Doc*_tmp4AD[6];_tmp4AD[5]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp4AD[4]=Cyc_PP_line_doc();_tmp4AD[3]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp4AE[2];_tmp4AE[1]=Cyc_Absynpp_stmt2doc(
_tmp450);_tmp4AE[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp4AE,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp4AD[2]=Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),
4));_tmp4AD[1]=Cyc_Absynpp_exp2doc(_tmp454);_tmp4AD[0]=Cyc_PP_text(_tag_arr("while (",
sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp4AD,sizeof(struct Cyc_PP_Doc*),
6));});goto _LL1045;_LL1059: s=Cyc_PP_text(_tag_arr("break;",sizeof(unsigned char),
7));goto _LL1045;_LL1061: s=Cyc_PP_text(_tag_arr("continue;",sizeof(unsigned char),
10));goto _LL1045;_LL1063: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp4B0;_tmp4B0.tag=Cyc_Std_String_pa;_tmp4B0.f1=(struct _tagged_arr)*_tmp456;{
void*_tmp4AF[1]={& _tmp4B0};Cyc_Std_aprintf(_tag_arr("goto %s;",sizeof(
unsigned char),9),_tag_arr(_tmp4AF,sizeof(void*),1));}}));goto _LL1045;_LL1065: s=({
struct Cyc_PP_Doc*_tmp4B1[10];_tmp4B1[9]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp4B1[8]=Cyc_PP_line_doc();_tmp4B1[7]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp4B2[2];_tmp4B2[1]=Cyc_Absynpp_stmt2doc(_tmp458);_tmp4B2[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp4B2,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp4B1[6]=Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),4));_tmp4B1[5]=Cyc_Absynpp_exp2doc(
_tmp45C);_tmp4B1[4]=Cyc_PP_text(_tag_arr("; ",sizeof(unsigned char),3));_tmp4B1[
3]=Cyc_Absynpp_exp2doc(_tmp460);_tmp4B1[2]=Cyc_PP_text(_tag_arr("; ",sizeof(
unsigned char),3));_tmp4B1[1]=Cyc_Absynpp_exp2doc(_tmp462);_tmp4B1[0]=Cyc_PP_text(
_tag_arr("for(",sizeof(unsigned char),5));Cyc_PP_cat(_tag_arr(_tmp4B1,sizeof(
struct Cyc_PP_Doc*),10));});goto _LL1045;_LL1067: s=({struct Cyc_PP_Doc*_tmp4B3[9];
_tmp4B3[8]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp4B3[7]=Cyc_PP_line_doc();
_tmp4B3[6]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp4B4[2];_tmp4B4[1]=Cyc_Absynpp_stmt2doc(
_tmp466);_tmp4B4[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp4B4,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp4B3[5]=Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),
4));_tmp4B3[4]=Cyc_Absynpp_exp2doc(_tmp46A);_tmp4B3[3]=Cyc_PP_text(_tag_arr("; ",
sizeof(unsigned char),3));_tmp4B3[2]=Cyc_Absynpp_exp2doc(_tmp46E);_tmp4B3[1]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_vardecl2doc,
_tag_arr(" ",sizeof(unsigned char),2),_tmp470);_tmp4B3[0]=Cyc_PP_text(_tag_arr("forarray(",
sizeof(unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp4B3,sizeof(struct Cyc_PP_Doc*),
9));});goto _LL1045;_LL1069: s=({struct Cyc_PP_Doc*_tmp4B5[7];_tmp4B5[6]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp4B5[5]=Cyc_PP_line_doc();_tmp4B5[4]=
Cyc_Absynpp_switchclauses2doc(_tmp472);_tmp4B5[3]=Cyc_PP_line_doc();_tmp4B5[2]=
Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),4));_tmp4B5[1]=Cyc_Absynpp_exp2doc(
_tmp474);_tmp4B5[0]=Cyc_PP_text(_tag_arr("switch (",sizeof(unsigned char),9));
Cyc_PP_cat(_tag_arr(_tmp4B5,sizeof(struct Cyc_PP_Doc*),7));});goto _LL1045;_LL1071:
s=({struct Cyc_PP_Doc*_tmp4B6[7];_tmp4B6[6]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp4B6[5]=Cyc_PP_line_doc();_tmp4B6[4]=Cyc_Absynpp_switchCclauses2doc(
_tmp476);_tmp4B6[3]=Cyc_PP_line_doc();_tmp4B6[2]=Cyc_PP_text(_tag_arr(") {",
sizeof(unsigned char),4));_tmp4B6[1]=Cyc_Absynpp_exp2doc(_tmp478);_tmp4B6[0]=Cyc_PP_text(
_tag_arr("switch \"C\" (",sizeof(unsigned char),13));Cyc_PP_cat(_tag_arr(_tmp4B6,
sizeof(struct Cyc_PP_Doc*),7));});goto _LL1045;_LL1073: s=Cyc_PP_text(_tag_arr("fallthru;",
sizeof(unsigned char),10));goto _LL1045;_LL1075: s=({struct Cyc_PP_Doc*_tmp4B7[3];
_tmp4B7[2]=Cyc_PP_text(_tag_arr(");",sizeof(unsigned char),3));_tmp4B7[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp47C);_tmp4B7[0]=Cyc_PP_text(_tag_arr("fallthru(",sizeof(unsigned char),10));
Cyc_PP_cat(_tag_arr(_tmp4B7,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1045;_LL1077:
s=({struct Cyc_PP_Doc*_tmp4B8[3];_tmp4B8[2]=Cyc_Absynpp_stmt2doc(_tmp47E);_tmp4B8[
1]=Cyc_PP_line_doc();_tmp4B8[0]=Cyc_Absynpp_decl2doc(_tmp480);Cyc_PP_cat(
_tag_arr(_tmp4B8,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1045;_LL1079: s=({struct
Cyc_PP_Doc*_tmp4B9[2];_tmp4B9[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp482));
_tmp4B9[0]=Cyc_PP_text(_tag_arr("cut ",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp4B9,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1045;_LL1081: s=({struct
Cyc_PP_Doc*_tmp4BA[2];_tmp4BA[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp484));
_tmp4BA[0]=Cyc_PP_text(_tag_arr("splice ",sizeof(unsigned char),8));Cyc_PP_cat(
_tag_arr(_tmp4BA,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1045;_LL1083: if(Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration(_tmp486): 0){s=({struct Cyc_PP_Doc*_tmp4BB[6];_tmp4BB[5]=
Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp4BB[4]=Cyc_PP_line_doc();
_tmp4BB[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp486));_tmp4BB[2]=Cyc_PP_line_doc();
_tmp4BB[1]=Cyc_PP_text(_tag_arr(": {",sizeof(unsigned char),4));_tmp4BB[0]=Cyc_PP_textptr(
_tmp488);Cyc_PP_cat(_tag_arr(_tmp4BB,sizeof(struct Cyc_PP_Doc*),6));});}else{s=({
struct Cyc_PP_Doc*_tmp4BC[3];_tmp4BC[2]=Cyc_Absynpp_stmt2doc(_tmp486);_tmp4BC[1]=
Cyc_PP_text(_tag_arr(": ",sizeof(unsigned char),3));_tmp4BC[0]=Cyc_PP_textptr(
_tmp488);Cyc_PP_cat(_tag_arr(_tmp4BC,sizeof(struct Cyc_PP_Doc*),3));});}goto
_LL1045;_LL1085: s=({struct Cyc_PP_Doc*_tmp4BD[7];_tmp4BD[6]=Cyc_PP_text(_tag_arr(");",
sizeof(unsigned char),3));_tmp4BD[5]=Cyc_Absynpp_exp2doc(_tmp48C);_tmp4BD[4]=Cyc_PP_text(
_tag_arr("} while (",sizeof(unsigned char),10));_tmp4BD[3]=Cyc_PP_line_doc();
_tmp4BD[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp48E));_tmp4BD[1]=Cyc_PP_line_doc();
_tmp4BD[0]=Cyc_PP_text(_tag_arr("do {",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp4BD,sizeof(struct Cyc_PP_Doc*),7));});goto _LL1045;_LL1087: s=({struct
Cyc_PP_Doc*_tmp4BE[9];_tmp4BE[8]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),
2));_tmp4BE[7]=Cyc_PP_line_doc();_tmp4BE[6]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp490));_tmp4BE[5]=Cyc_PP_line_doc();_tmp4BE[4]=Cyc_PP_text(_tag_arr("} catch {",
sizeof(unsigned char),10));_tmp4BE[3]=Cyc_PP_line_doc();_tmp4BE[2]=Cyc_PP_nest(2,
Cyc_Absynpp_stmt2doc(_tmp492));_tmp4BE[1]=Cyc_PP_line_doc();_tmp4BE[0]=Cyc_PP_text(
_tag_arr("try {",sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp4BE,sizeof(
struct Cyc_PP_Doc*),9));});goto _LL1045;_LL1089: s=({struct Cyc_PP_Doc*_tmp4BF[9];
_tmp4BF[8]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp4BF[7]=Cyc_PP_line_doc();
_tmp4BF[6]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp494));_tmp4BF[5]=Cyc_PP_line_doc();
_tmp4BF[4]=Cyc_PP_text(_tag_arr("{",sizeof(unsigned char),2));_tmp4BF[3]=Cyc_Absynpp_qvar2doc(
_tmp496->name);_tmp4BF[2]=Cyc_PP_text(_tag_arr(">",sizeof(unsigned char),2));
_tmp4BF[1]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp498));_tmp4BF[0]=Cyc_PP_text(
_tag_arr("region<",sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp4BF,sizeof(
struct Cyc_PP_Doc*),9));});goto _LL1045;_LL1045:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp4C0=(void*)p->r;int _tmp4E2;
void*_tmp4E4;unsigned char _tmp4E6;struct _tagged_arr _tmp4E8;struct Cyc_Absyn_Vardecl*
_tmp4EA;struct Cyc_List_List*_tmp4EC;struct Cyc_Absyn_Pat*_tmp4EE;struct Cyc_Absyn_Vardecl*
_tmp4F0;struct _tuple0*_tmp4F2;struct Cyc_List_List*_tmp4F4;struct Cyc_List_List*
_tmp4F6;struct _tuple0*_tmp4F8;struct Cyc_List_List*_tmp4FA;struct Cyc_List_List*
_tmp4FC;struct _tuple0*_tmp4FE;struct Cyc_List_List*_tmp500;struct Cyc_List_List*
_tmp502;struct Cyc_Absyn_Structdecl*_tmp504;struct Cyc_Absyn_Enumfield*_tmp506;
struct Cyc_Absyn_Enumfield*_tmp508;struct Cyc_List_List*_tmp50A;struct Cyc_List_List*
_tmp50C;struct Cyc_Absyn_Tunionfield*_tmp50E;_LL1218: if(_tmp4C0 == (void*)Cyc_Absyn_Wild_p){
goto _LL1219;}else{goto _LL1220;}_LL1220: if(_tmp4C0 == (void*)Cyc_Absyn_Null_p){
goto _LL1221;}else{goto _LL1222;}_LL1222: if((unsigned int)_tmp4C0 > 2?*((int*)
_tmp4C0)== Cyc_Absyn_Int_p: 0){_LL1253: _tmp4E4=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp4C0)->f1;goto _LL1251;_LL1251: _tmp4E2=((struct Cyc_Absyn_Int_p_struct*)_tmp4C0)->f2;
goto _LL1223;}else{goto _LL1224;}_LL1224: if((unsigned int)_tmp4C0 > 2?*((int*)
_tmp4C0)== Cyc_Absyn_Char_p: 0){_LL1255: _tmp4E6=((struct Cyc_Absyn_Char_p_struct*)
_tmp4C0)->f1;goto _LL1225;}else{goto _LL1226;}_LL1226: if((unsigned int)_tmp4C0 > 2?*((
int*)_tmp4C0)== Cyc_Absyn_Float_p: 0){_LL1257: _tmp4E8=((struct Cyc_Absyn_Float_p_struct*)
_tmp4C0)->f1;goto _LL1227;}else{goto _LL1228;}_LL1228: if((unsigned int)_tmp4C0 > 2?*((
int*)_tmp4C0)== Cyc_Absyn_Var_p: 0){_LL1259: _tmp4EA=((struct Cyc_Absyn_Var_p_struct*)
_tmp4C0)->f1;goto _LL1229;}else{goto _LL1230;}_LL1230: if((unsigned int)_tmp4C0 > 2?*((
int*)_tmp4C0)== Cyc_Absyn_Tuple_p: 0){_LL1261: _tmp4EC=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp4C0)->f1;goto _LL1231;}else{goto _LL1232;}_LL1232: if((unsigned int)_tmp4C0 > 2?*((
int*)_tmp4C0)== Cyc_Absyn_Pointer_p: 0){_LL1263: _tmp4EE=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp4C0)->f1;goto _LL1233;}else{goto _LL1234;}_LL1234: if((unsigned int)_tmp4C0 > 2?*((
int*)_tmp4C0)== Cyc_Absyn_Reference_p: 0){_LL1265: _tmp4F0=((struct Cyc_Absyn_Reference_p_struct*)
_tmp4C0)->f1;goto _LL1235;}else{goto _LL1236;}_LL1236: if((unsigned int)_tmp4C0 > 2?*((
int*)_tmp4C0)== Cyc_Absyn_UnknownId_p: 0){_LL1267: _tmp4F2=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp4C0)->f1;goto _LL1237;}else{goto _LL1238;}_LL1238: if((unsigned int)_tmp4C0 > 2?*((
int*)_tmp4C0)== Cyc_Absyn_UnknownCall_p: 0){_LL1273: _tmp4F8=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp4C0)->f1;goto _LL1271;_LL1271: _tmp4F6=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp4C0)->f2;goto _LL1269;_LL1269: _tmp4F4=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp4C0)->f3;goto _LL1239;}else{goto _LL1240;}_LL1240: if((unsigned int)_tmp4C0 > 2?*((
int*)_tmp4C0)== Cyc_Absyn_UnknownFields_p: 0){_LL1279: _tmp4FE=((struct Cyc_Absyn_UnknownFields_p_struct*)
_tmp4C0)->f1;goto _LL1277;_LL1277: _tmp4FC=((struct Cyc_Absyn_UnknownFields_p_struct*)
_tmp4C0)->f2;goto _LL1275;_LL1275: _tmp4FA=((struct Cyc_Absyn_UnknownFields_p_struct*)
_tmp4C0)->f3;goto _LL1241;}else{goto _LL1242;}_LL1242: if((unsigned int)_tmp4C0 > 2?*((
int*)_tmp4C0)== Cyc_Absyn_Struct_p: 0){_LL1285: _tmp504=((struct Cyc_Absyn_Struct_p_struct*)
_tmp4C0)->f1;goto _LL1283;_LL1283: _tmp502=((struct Cyc_Absyn_Struct_p_struct*)
_tmp4C0)->f3;goto _LL1281;_LL1281: _tmp500=((struct Cyc_Absyn_Struct_p_struct*)
_tmp4C0)->f4;goto _LL1243;}else{goto _LL1244;}_LL1244: if((unsigned int)_tmp4C0 > 2?*((
int*)_tmp4C0)== Cyc_Absyn_Enum_p: 0){_LL1287: _tmp506=((struct Cyc_Absyn_Enum_p_struct*)
_tmp4C0)->f2;goto _LL1245;}else{goto _LL1246;}_LL1246: if((unsigned int)_tmp4C0 > 2?*((
int*)_tmp4C0)== Cyc_Absyn_AnonEnum_p: 0){_LL1289: _tmp508=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp4C0)->f2;goto _LL1247;}else{goto _LL1248;}_LL1248: if((unsigned int)_tmp4C0 > 2?*((
int*)_tmp4C0)== Cyc_Absyn_Tunion_p: 0){_LL1295: _tmp50E=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp4C0)->f2;goto _LL1293;_LL1293: _tmp50C=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp4C0)->f3;goto _LL1291;_LL1291: _tmp50A=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp4C0)->f4;goto _LL1249;}else{goto _LL1217;}_LL1219: s=Cyc_PP_text(_tag_arr("_",
sizeof(unsigned char),2));goto _LL1217;_LL1221: s=Cyc_PP_text(_tag_arr("NULL",
sizeof(unsigned char),5));goto _LL1217;_LL1223: if(_tmp4E4 == (void*)Cyc_Absyn_Signed){
s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp511;_tmp511.tag=
Cyc_Std_Int_pa;_tmp511.f1=(int)((unsigned int)_tmp4E2);{void*_tmp510[1]={&
_tmp511};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp510,
sizeof(void*),1));}}));}else{s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp513;_tmp513.tag=Cyc_Std_Int_pa;_tmp513.f1=(unsigned int)_tmp4E2;{void*
_tmp512[1]={& _tmp513};Cyc_Std_aprintf(_tag_arr("%u",sizeof(unsigned char),3),
_tag_arr(_tmp512,sizeof(void*),1));}}));}goto _LL1217;_LL1225: s=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp515;_tmp515.tag=Cyc_Std_String_pa;
_tmp515.f1=(struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp4E6);{void*_tmp514[1]={&
_tmp515};Cyc_Std_aprintf(_tag_arr("'%s'",sizeof(unsigned char),5),_tag_arr(
_tmp514,sizeof(void*),1));}}));goto _LL1217;_LL1227: s=Cyc_PP_text(_tmp4E8);goto
_LL1217;_LL1229: s=Cyc_Absynpp_qvar2doc(_tmp4EA->name);goto _LL1217;_LL1231: s=({
struct Cyc_PP_Doc*_tmp516[3];_tmp516[2]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp516[1]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Pat*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_pat2doc,_tag_arr(",",sizeof(unsigned char),2),_tmp4EC);_tmp516[0]=
Cyc_PP_text(_tag_arr("$(",sizeof(unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp516,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL1217;_LL1233: s=({struct Cyc_PP_Doc*
_tmp517[2];_tmp517[1]=Cyc_Absynpp_pat2doc(_tmp4EE);_tmp517[0]=Cyc_PP_text(
_tag_arr("&",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp517,sizeof(struct
Cyc_PP_Doc*),2));});goto _LL1217;_LL1235: s=({struct Cyc_PP_Doc*_tmp518[2];_tmp518[
1]=Cyc_Absynpp_qvar2doc(_tmp4F0->name);_tmp518[0]=Cyc_PP_text(_tag_arr("*",
sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp518,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1217;_LL1237: s=Cyc_Absynpp_qvar2doc(_tmp4F2);goto _LL1217;_LL1239: s=({
struct Cyc_PP_Doc*_tmp519[3];_tmp519[2]=Cyc_PP_group(_tag_arr("(",sizeof(
unsigned char),2),_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp4F4));_tmp519[1]=
Cyc_Absynpp_tvars2doc(_tmp4F6);_tmp519[0]=Cyc_Absynpp_qvar2doc(_tmp4F8);Cyc_PP_cat(
_tag_arr(_tmp519,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1217;_LL1241: s=({struct
Cyc_PP_Doc*_tmp51A[3];_tmp51A[2]=Cyc_PP_group(_tag_arr("{",sizeof(unsigned char),
2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple5*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp4FA));_tmp51A[1]=Cyc_Absynpp_tvars2doc(
_tmp4FC);_tmp51A[0]=Cyc_Absynpp_qvar2doc(_tmp4FE);Cyc_PP_cat(_tag_arr(_tmp51A,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL1217;_LL1243: s=({struct Cyc_PP_Doc*
_tmp51B[3];_tmp51B[2]=Cyc_PP_group(_tag_arr("{",sizeof(unsigned char),2),
_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple5*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp500));_tmp51B[1]=Cyc_Absynpp_tvars2doc(
_tmp502);_tmp51B[0]=_tmp504->name == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp504->name))->v);Cyc_PP_cat(
_tag_arr(_tmp51B,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1217;_LL1245: s=Cyc_Absynpp_qvar2doc(
_tmp506->name);goto _LL1217;_LL1247: s=Cyc_Absynpp_qvar2doc(_tmp508->name);goto
_LL1217;_LL1249: if(_tmp50A == 0){s=Cyc_Absynpp_qvar2doc(_tmp50E->name);}else{s=({
struct Cyc_PP_Doc*_tmp51C[3];_tmp51C[2]=Cyc_PP_egroup(_tag_arr("(",sizeof(
unsigned char),2),_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp50A));_tmp51C[1]=
Cyc_Absynpp_tvars2doc(_tmp50C);_tmp51C[0]=Cyc_Absynpp_qvar2doc(_tmp50E->name);
Cyc_PP_cat(_tag_arr(_tmp51C,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL1217;
_LL1217:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple5*dp){return({
struct Cyc_PP_Doc*_tmp51D[2];_tmp51D[1]=Cyc_Absynpp_pat2doc((*dp).f2);_tmp51D[0]=
Cyc_PP_egroup(_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",sizeof(
unsigned char),2),_tag_arr("=",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*
dp).f1));Cyc_PP_cat(_tag_arr(_tmp51D,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*
Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){if(c->where_clause
== 0?(void*)(c->pattern)->r == (void*)Cyc_Absyn_Wild_p: 0){return({struct Cyc_PP_Doc*
_tmp51E[2];_tmp51E[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp51F[2];_tmp51F[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp51F[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp51F,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp51E[0]=Cyc_PP_text(_tag_arr("default: ",sizeof(
unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp51E,sizeof(struct Cyc_PP_Doc*),2));});}
else{if(c->where_clause == 0){return({struct Cyc_PP_Doc*_tmp520[4];_tmp520[3]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp521[2];_tmp521[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp521[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp521,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp520[2]=Cyc_PP_text(_tag_arr(": ",sizeof(unsigned char),3));_tmp520[1]=Cyc_Absynpp_pat2doc(
c->pattern);_tmp520[0]=Cyc_PP_text(_tag_arr("case ",sizeof(unsigned char),6));
Cyc_PP_cat(_tag_arr(_tmp520,sizeof(struct Cyc_PP_Doc*),4));});}else{return({
struct Cyc_PP_Doc*_tmp522[6];_tmp522[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp523[
2];_tmp523[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp523[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp523,sizeof(struct Cyc_PP_Doc*),2));}));_tmp522[4]=Cyc_PP_text(
_tag_arr(": ",sizeof(unsigned char),3));_tmp522[3]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp522[2]=Cyc_PP_text(_tag_arr(" && ",
sizeof(unsigned char),5));_tmp522[1]=Cyc_Absynpp_pat2doc(c->pattern);_tmp522[0]=
Cyc_PP_text(_tag_arr("case ",sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(
_tmp522,sizeof(struct Cyc_PP_Doc*),6));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Switch_clause*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_switchclause2doc,_tag_arr("",sizeof(unsigned char),1),cs);}struct Cyc_PP_Doc*
Cyc_Absynpp_switchCclause2doc(struct Cyc_Absyn_SwitchC_clause*c){struct Cyc_Absyn_SwitchC_clause
_tmp526;struct Cyc_Absyn_Stmt*_tmp527;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_SwitchC_clause*
_tmp524=c;_tmp526=*_tmp524;_LL1322: _tmp529=_tmp526.cnst_exp;goto _LL1320;_LL1320:
_tmp527=_tmp526.body;goto _LL1317;_LL1317: if(_tmp529 == 0){return({struct Cyc_PP_Doc*
_tmp52B[2];_tmp52B[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp52C[2];_tmp52C[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp52C[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp52C,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp52B[0]=Cyc_PP_text(_tag_arr("default: ",sizeof(
unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp52B,sizeof(struct Cyc_PP_Doc*),2));});}
else{return({struct Cyc_PP_Doc*_tmp52D[4];_tmp52D[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp52E[2];_tmp52E[1]=Cyc_Absynpp_stmt2doc(_tmp527);_tmp52E[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp52E,sizeof(struct Cyc_PP_Doc*),2));}));_tmp52D[2]=Cyc_PP_text(
_tag_arr(": ",sizeof(unsigned char),3));_tmp52D[1]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(_tmp529));_tmp52D[0]=Cyc_PP_text(_tag_arr("case ",
sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp52D,sizeof(struct Cyc_PP_Doc*),
4));});}}struct Cyc_PP_Doc*Cyc_Absynpp_switchCclauses2doc(struct Cyc_List_List*cs){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_SwitchC_clause*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchCclause2doc,
_tag_arr("",sizeof(unsigned char),1),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f){if(f->tag == 0){return Cyc_Absynpp_qvar2doc(f->name);}
else{return({struct Cyc_PP_Doc*_tmp52F[3];_tmp52F[2]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(f->tag));_tmp52F[1]=Cyc_PP_text(_tag_arr(" = ",
sizeof(unsigned char),4));_tmp52F[0]=Cyc_Absynpp_qvar2doc(f->name);Cyc_PP_cat(
_tag_arr(_tmp52F,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Enumfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_enumfield2doc,_tag_arr(",",sizeof(unsigned char),2),fs);}static
struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(
v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,
_tag_arr(",",sizeof(unsigned char),2),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(
struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl _tmp532;struct Cyc_List_List*
_tmp533;struct Cyc_Absyn_Exp*_tmp535;void*_tmp537;struct Cyc_Absyn_Tqual _tmp539;
struct _tuple0*_tmp53B;void*_tmp53D;struct Cyc_Absyn_Vardecl*_tmp530=vd;_tmp532=*
_tmp530;_LL1342: _tmp53D=(void*)_tmp532.sc;goto _LL1340;_LL1340: _tmp53B=_tmp532.name;
goto _LL1338;_LL1338: _tmp539=_tmp532.tq;goto _LL1336;_LL1336: _tmp537=(void*)
_tmp532.type;goto _LL1334;_LL1334: _tmp535=_tmp532.initializer;goto _LL1332;_LL1332:
_tmp533=_tmp532.attributes;goto _LL1329;_LL1329: {struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*
sn=Cyc_Absynpp_typedef_name2doc(_tmp53B);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp533);struct Cyc_PP_Doc*beforenamedoc;if(! Cyc_Absynpp_to_VC){beforenamedoc=
attsdoc;}else{void*_tmp53F=Cyc_Tcutil_compress(_tmp537);struct Cyc_Absyn_FnInfo
_tmp545;struct Cyc_List_List*_tmp547;_LL1345: if((unsigned int)_tmp53F > 3?*((int*)
_tmp53F)== Cyc_Absyn_FnType: 0){_LL1350: _tmp545=((struct Cyc_Absyn_FnType_struct*)
_tmp53F)->f1;_LL1352: _tmp547=_tmp545.attributes;goto _LL1346;}else{goto _LL1347;}
_LL1347: goto _LL1348;_LL1346: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp547);goto
_LL1344;_LL1348: beforenamedoc=Cyc_PP_nil_doc();goto _LL1344;_LL1344:;}s=({struct
Cyc_PP_Doc*_tmp549[5];_tmp549[4]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),
2));_tmp549[3]=_tmp535 == 0? Cyc_PP_text(_tag_arr("",sizeof(unsigned char),1)):({
struct Cyc_PP_Doc*_tmp54C[2];_tmp54C[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmp535));_tmp54C[0]=Cyc_PP_text(_tag_arr(" = ",sizeof(unsigned char),
4));Cyc_PP_cat(_tag_arr(_tmp54C,sizeof(struct Cyc_PP_Doc*),2));});_tmp549[2]=Cyc_Absynpp_tqtd2doc(
_tmp539,_tmp537,({struct Cyc_Core_Opt*_tmp54A=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp54A->v=({struct Cyc_PP_Doc*_tmp54B[2];_tmp54B[1]=sn;_tmp54B[0]=beforenamedoc;
Cyc_PP_cat(_tag_arr(_tmp54B,sizeof(struct Cyc_PP_Doc*),2));});_tmp54A;}));_tmp549[
1]=Cyc_Absynpp_scope2doc(_tmp53D);_tmp549[0]=Cyc_Absynpp_to_VC? attsdoc: Cyc_PP_nil_doc();
Cyc_PP_cat(_tag_arr(_tmp549,sizeof(struct Cyc_PP_Doc*),5));});return s;}}struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp54D=(
void*)d->r;struct Cyc_Absyn_Fndecl*_tmp567;struct Cyc_Absyn_Structdecl*_tmp569;
struct Cyc_Absyn_Uniondecl*_tmp56B;struct Cyc_Absyn_Vardecl*_tmp56D;struct Cyc_Absyn_Tuniondecl*
_tmp56F;struct Cyc_Absyn_Tuniondecl _tmp571;int _tmp572;struct Cyc_Core_Opt*_tmp574;
struct Cyc_List_List*_tmp576;struct _tuple0*_tmp578;void*_tmp57A;struct Cyc_Absyn_Exp*
_tmp57C;struct Cyc_Absyn_Pat*_tmp57E;struct Cyc_List_List*_tmp580;struct Cyc_Absyn_Enumdecl*
_tmp582;struct Cyc_Absyn_Enumdecl _tmp584;struct Cyc_Core_Opt*_tmp585;struct _tuple0*
_tmp587;void*_tmp589;struct Cyc_Absyn_Typedefdecl*_tmp58B;struct Cyc_List_List*
_tmp58D;struct _tagged_arr*_tmp58F;struct Cyc_List_List*_tmp591;struct _tuple0*
_tmp593;struct Cyc_List_List*_tmp595;_LL1359: if(*((int*)_tmp54D)== Cyc_Absyn_Fn_d){
_LL1384: _tmp567=((struct Cyc_Absyn_Fn_d_struct*)_tmp54D)->f1;goto _LL1360;}else{
goto _LL1361;}_LL1361: if(*((int*)_tmp54D)== Cyc_Absyn_Struct_d){_LL1386: _tmp569=((
struct Cyc_Absyn_Struct_d_struct*)_tmp54D)->f1;goto _LL1362;}else{goto _LL1363;}
_LL1363: if(*((int*)_tmp54D)== Cyc_Absyn_Union_d){_LL1388: _tmp56B=((struct Cyc_Absyn_Union_d_struct*)
_tmp54D)->f1;goto _LL1364;}else{goto _LL1365;}_LL1365: if(*((int*)_tmp54D)== Cyc_Absyn_Var_d){
_LL1390: _tmp56D=((struct Cyc_Absyn_Var_d_struct*)_tmp54D)->f1;goto _LL1366;}else{
goto _LL1367;}_LL1367: if(*((int*)_tmp54D)== Cyc_Absyn_Tunion_d){_LL1392: _tmp56F=((
struct Cyc_Absyn_Tunion_d_struct*)_tmp54D)->f1;_tmp571=*_tmp56F;_LL1403: _tmp57A=(
void*)_tmp571.sc;goto _LL1401;_LL1401: _tmp578=_tmp571.name;goto _LL1399;_LL1399:
_tmp576=_tmp571.tvs;goto _LL1397;_LL1397: _tmp574=_tmp571.fields;goto _LL1395;
_LL1395: _tmp572=_tmp571.is_xtunion;goto _LL1368;}else{goto _LL1369;}_LL1369: if(*((
int*)_tmp54D)== Cyc_Absyn_Let_d){_LL1407: _tmp57E=((struct Cyc_Absyn_Let_d_struct*)
_tmp54D)->f1;goto _LL1405;_LL1405: _tmp57C=((struct Cyc_Absyn_Let_d_struct*)_tmp54D)->f4;
goto _LL1370;}else{goto _LL1371;}_LL1371: if(*((int*)_tmp54D)== Cyc_Absyn_Letv_d){
_LL1409: _tmp580=((struct Cyc_Absyn_Letv_d_struct*)_tmp54D)->f1;goto _LL1372;}else{
goto _LL1373;}_LL1373: if(*((int*)_tmp54D)== Cyc_Absyn_Enum_d){_LL1411: _tmp582=((
struct Cyc_Absyn_Enum_d_struct*)_tmp54D)->f1;_tmp584=*_tmp582;_LL1418: _tmp589=(
void*)_tmp584.sc;goto _LL1416;_LL1416: _tmp587=_tmp584.name;goto _LL1414;_LL1414:
_tmp585=_tmp584.fields;goto _LL1374;}else{goto _LL1375;}_LL1375: if(*((int*)_tmp54D)
== Cyc_Absyn_Typedef_d){_LL1420: _tmp58B=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp54D)->f1;goto _LL1376;}else{goto _LL1377;}_LL1377: if(*((int*)_tmp54D)== Cyc_Absyn_Namespace_d){
_LL1424: _tmp58F=((struct Cyc_Absyn_Namespace_d_struct*)_tmp54D)->f1;goto _LL1422;
_LL1422: _tmp58D=((struct Cyc_Absyn_Namespace_d_struct*)_tmp54D)->f2;goto _LL1378;}
else{goto _LL1379;}_LL1379: if(*((int*)_tmp54D)== Cyc_Absyn_Using_d){_LL1428:
_tmp593=((struct Cyc_Absyn_Using_d_struct*)_tmp54D)->f1;goto _LL1426;_LL1426:
_tmp591=((struct Cyc_Absyn_Using_d_struct*)_tmp54D)->f2;goto _LL1380;}else{goto
_LL1381;}_LL1381: if(*((int*)_tmp54D)== Cyc_Absyn_ExternC_d){_LL1430: _tmp595=((
struct Cyc_Absyn_ExternC_d_struct*)_tmp54D)->f1;goto _LL1382;}else{goto _LL1358;}
_LL1360: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp59E=_cycalloc(
sizeof(struct Cyc_Absyn_FnType_struct));_tmp59E[0]=({struct Cyc_Absyn_FnType_struct
_tmp59F;_tmp59F.tag=Cyc_Absyn_FnType;_tmp59F.f1=({struct Cyc_Absyn_FnInfo _tmp5A0;
_tmp5A0.tvars=_tmp567->tvs;_tmp5A0.effect=_tmp567->effect;_tmp5A0.ret_typ=(void*)((
void*)_tmp567->ret_type);_tmp5A0.args=((struct Cyc_List_List*(*)(struct _tuple1*(*
f)(struct _tuple3*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp567->args);_tmp5A0.c_varargs=_tmp567->c_varargs;_tmp5A0.cyc_varargs=_tmp567->cyc_varargs;
_tmp5A0.rgn_po=_tmp567->rgn_po;_tmp5A0.attributes=0;_tmp5A0;});_tmp59F;});
_tmp59E;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp567->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp567->is_inline){if(Cyc_Absynpp_to_VC){
inlinedoc=Cyc_PP_text(_tag_arr("__inline ",sizeof(unsigned char),10));}else{
inlinedoc=Cyc_PP_text(_tag_arr("inline ",sizeof(unsigned char),8));}}else{
inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((
void*)_tmp567->sc);struct Cyc_PP_Doc*beforenamedoc=Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_tmp567->attributes): attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp567->name);struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual
_tmp59B;_tmp59B.q_const=0;_tmp59B.q_volatile=0;_tmp59B.q_restrict=0;_tmp59B;}),t,({
struct Cyc_Core_Opt*_tmp59C=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp59C->v=({
struct Cyc_PP_Doc*_tmp59D[2];_tmp59D[1]=namedoc;_tmp59D[0]=beforenamedoc;Cyc_PP_cat(
_tag_arr(_tmp59D,sizeof(struct Cyc_PP_Doc*),2));});_tmp59C;}));struct Cyc_PP_Doc*
bodydoc=({struct Cyc_PP_Doc*_tmp599[4];_tmp599[3]=Cyc_PP_text(_tag_arr("}",
sizeof(unsigned char),2));_tmp599[2]=Cyc_PP_line_doc();_tmp599[1]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp59A[2];_tmp59A[1]=Cyc_Absynpp_stmt2doc(_tmp567->body);
_tmp59A[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp59A,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp599[0]=Cyc_PP_text(_tag_arr(" {",sizeof(unsigned char),3));Cyc_PP_cat(
_tag_arr(_tmp599,sizeof(struct Cyc_PP_Doc*),4));});s=({struct Cyc_PP_Doc*_tmp597[4];
_tmp597[3]=bodydoc;_tmp597[2]=tqtddoc;_tmp597[1]=scopedoc;_tmp597[0]=inlinedoc;
Cyc_PP_cat(_tag_arr(_tmp597,sizeof(struct Cyc_PP_Doc*),4));});if(Cyc_Absynpp_to_VC){
s=({struct Cyc_PP_Doc*_tmp598[2];_tmp598[1]=s;_tmp598[0]=attsdoc;Cyc_PP_cat(
_tag_arr(_tmp598,sizeof(struct Cyc_PP_Doc*),2));});}goto _LL1358;}}_LL1362: if(
_tmp569->fields == 0){s=({struct Cyc_PP_Doc*_tmp5A1[5];_tmp5A1[4]=Cyc_PP_text(
_tag_arr(";",sizeof(unsigned char),2));_tmp5A1[3]=Cyc_Absynpp_ktvars2doc(_tmp569->tvs);
_tmp5A1[2]=_tmp569->name == 0? Cyc_PP_text(_tag_arr("",sizeof(unsigned char),1)):
Cyc_Absynpp_typedef_name2doc((struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(
_tmp569->name))->v);_tmp5A1[1]=Cyc_PP_text(_tag_arr("struct ",sizeof(
unsigned char),8));_tmp5A1[0]=Cyc_Absynpp_scope2doc((void*)_tmp569->sc);Cyc_PP_cat(
_tag_arr(_tmp5A1,sizeof(struct Cyc_PP_Doc*),5));});}else{s=({struct Cyc_PP_Doc*
_tmp5A2[10];_tmp5A2[9]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));
_tmp5A2[8]=Cyc_Absynpp_atts2doc(_tmp569->attributes);_tmp5A2[7]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp5A2[6]=Cyc_PP_line_doc();_tmp5A2[5]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp5A3[2];_tmp5A3[1]=Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp569->fields))->v);
_tmp5A3[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp5A3,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp5A2[4]=Cyc_PP_text(_tag_arr(" {",sizeof(unsigned char),3));_tmp5A2[3]=
Cyc_Absynpp_ktvars2doc(_tmp569->tvs);_tmp5A2[2]=_tmp569->name == 0? Cyc_PP_text(
_tag_arr("",sizeof(unsigned char),1)): Cyc_Absynpp_typedef_name2doc((struct
_tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp569->name))->v);_tmp5A2[1]=Cyc_PP_text(
_tag_arr("struct ",sizeof(unsigned char),8));_tmp5A2[0]=Cyc_Absynpp_scope2doc((
void*)_tmp569->sc);Cyc_PP_cat(_tag_arr(_tmp5A2,sizeof(struct Cyc_PP_Doc*),10));});}
goto _LL1358;_LL1364: if(_tmp56B->fields == 0){s=({struct Cyc_PP_Doc*_tmp5A4[5];
_tmp5A4[4]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmp5A4[3]=Cyc_Absynpp_tvars2doc(
_tmp56B->tvs);_tmp5A4[2]=_tmp56B->name == 0? Cyc_PP_text(_tag_arr("",sizeof(
unsigned char),1)): Cyc_Absynpp_qvar2doc((struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(_tmp56B->name))->v);_tmp5A4[1]=Cyc_PP_text(_tag_arr("union ",sizeof(
unsigned char),7));_tmp5A4[0]=Cyc_Absynpp_scope2doc((void*)_tmp56B->sc);Cyc_PP_cat(
_tag_arr(_tmp5A4,sizeof(struct Cyc_PP_Doc*),5));});}else{s=({struct Cyc_PP_Doc*
_tmp5A5[10];_tmp5A5[9]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));
_tmp5A5[8]=Cyc_Absynpp_atts2doc(_tmp56B->attributes);_tmp5A5[7]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp5A5[6]=Cyc_PP_line_doc();_tmp5A5[5]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp5A6[2];_tmp5A6[1]=Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp56B->fields))->v);
_tmp5A6[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp5A6,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp5A5[4]=Cyc_PP_text(_tag_arr(" {",sizeof(unsigned char),3));_tmp5A5[3]=
Cyc_Absynpp_tvars2doc(_tmp56B->tvs);_tmp5A5[2]=_tmp56B->name == 0? Cyc_PP_text(
_tag_arr("",sizeof(unsigned char),1)): Cyc_Absynpp_qvar2doc((struct _tuple0*)((
struct Cyc_Core_Opt*)_check_null(_tmp56B->name))->v);_tmp5A5[1]=Cyc_PP_text(
_tag_arr("union ",sizeof(unsigned char),7));_tmp5A5[0]=Cyc_Absynpp_scope2doc((
void*)_tmp56B->sc);Cyc_PP_cat(_tag_arr(_tmp5A5,sizeof(struct Cyc_PP_Doc*),10));});}
goto _LL1358;_LL1366: s=Cyc_Absynpp_vardecl2doc(_tmp56D);goto _LL1358;_LL1368: if(
_tmp574 == 0){s=({struct Cyc_PP_Doc*_tmp5A7[5];_tmp5A7[4]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp5A7[3]=Cyc_Absynpp_ktvars2doc(_tmp576);_tmp5A7[2]=
_tmp572? Cyc_Absynpp_qvar2doc(_tmp578): Cyc_Absynpp_typedef_name2doc(_tmp578);
_tmp5A7[1]=_tmp572? Cyc_PP_text(_tag_arr("xtunion ",sizeof(unsigned char),9)): Cyc_PP_text(
_tag_arr("tunion ",sizeof(unsigned char),8));_tmp5A7[0]=Cyc_Absynpp_scope2doc(
_tmp57A);Cyc_PP_cat(_tag_arr(_tmp5A7,sizeof(struct Cyc_PP_Doc*),5));});}else{s=({
struct Cyc_PP_Doc*_tmp5A8[8];_tmp5A8[7]=Cyc_PP_text(_tag_arr("};",sizeof(
unsigned char),3));_tmp5A8[6]=Cyc_PP_line_doc();_tmp5A8[5]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp5A9[2];_tmp5A9[1]=Cyc_Absynpp_tunionfields2doc((struct Cyc_List_List*)
_tmp574->v);_tmp5A9[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp5A9,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp5A8[4]=Cyc_PP_text(_tag_arr(" {",sizeof(
unsigned char),3));_tmp5A8[3]=Cyc_Absynpp_ktvars2doc(_tmp576);_tmp5A8[2]=_tmp572?
Cyc_Absynpp_qvar2doc(_tmp578): Cyc_Absynpp_typedef_name2doc(_tmp578);_tmp5A8[1]=
_tmp572? Cyc_PP_text(_tag_arr("xtunion ",sizeof(unsigned char),9)): Cyc_PP_text(
_tag_arr("tunion ",sizeof(unsigned char),8));_tmp5A8[0]=Cyc_Absynpp_scope2doc(
_tmp57A);Cyc_PP_cat(_tag_arr(_tmp5A8,sizeof(struct Cyc_PP_Doc*),8));});}goto
_LL1358;_LL1370: s=({struct Cyc_PP_Doc*_tmp5AA[5];_tmp5AA[4]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp5AA[3]=Cyc_Absynpp_exp2doc(_tmp57C);_tmp5AA[2]=Cyc_PP_text(
_tag_arr(" = ",sizeof(unsigned char),4));_tmp5AA[1]=Cyc_Absynpp_pat2doc(_tmp57E);
_tmp5AA[0]=Cyc_PP_text(_tag_arr("let ",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp5AA,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1358;_LL1372: s=({struct
Cyc_PP_Doc*_tmp5AB[3];_tmp5AB[2]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),
2));_tmp5AB[1]=Cyc_Absynpp_ids2doc(_tmp580);_tmp5AB[0]=Cyc_PP_text(_tag_arr("let ",
sizeof(unsigned char),5));Cyc_PP_cat(_tag_arr(_tmp5AB,sizeof(struct Cyc_PP_Doc*),
3));});goto _LL1358;_LL1374: if(_tmp585 == 0){s=({struct Cyc_PP_Doc*_tmp5AC[4];
_tmp5AC[3]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmp5AC[2]=Cyc_Absynpp_typedef_name2doc(
_tmp587);_tmp5AC[1]=Cyc_PP_text(_tag_arr("enum ",sizeof(unsigned char),6));
_tmp5AC[0]=Cyc_Absynpp_scope2doc(_tmp589);Cyc_PP_cat(_tag_arr(_tmp5AC,sizeof(
struct Cyc_PP_Doc*),4));});}else{s=({struct Cyc_PP_Doc*_tmp5AD[7];_tmp5AD[6]=Cyc_PP_text(
_tag_arr("};",sizeof(unsigned char),3));_tmp5AD[5]=Cyc_PP_line_doc();_tmp5AD[4]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp5AE[2];_tmp5AE[1]=Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*)_tmp585->v);_tmp5AE[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp5AE,sizeof(struct Cyc_PP_Doc*),2));}));_tmp5AD[3]=Cyc_PP_text(
_tag_arr(" {",sizeof(unsigned char),3));_tmp5AD[2]=Cyc_Absynpp_qvar2doc(_tmp587);
_tmp5AD[1]=Cyc_PP_text(_tag_arr("enum ",sizeof(unsigned char),6));_tmp5AD[0]=Cyc_Absynpp_scope2doc(
_tmp589);Cyc_PP_cat(_tag_arr(_tmp5AD,sizeof(struct Cyc_PP_Doc*),7));});}goto
_LL1358;_LL1376: s=({struct Cyc_PP_Doc*_tmp5AF[3];_tmp5AF[2]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp5AF[1]=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual
_tmp5B0;_tmp5B0.q_const=0;_tmp5B0.q_volatile=0;_tmp5B0.q_restrict=0;_tmp5B0;}),(
void*)_tmp58B->defn,({struct Cyc_Core_Opt*_tmp5B1=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp5B1->v=({struct Cyc_PP_Doc*_tmp5B2[2];_tmp5B2[1]=Cyc_Absynpp_tvars2doc(
_tmp58B->tvs);_tmp5B2[0]=Cyc_Absynpp_typedef_name2doc(_tmp58B->name);Cyc_PP_cat(
_tag_arr(_tmp5B2,sizeof(struct Cyc_PP_Doc*),2));});_tmp5B1;}));_tmp5AF[0]=Cyc_PP_text(
_tag_arr("typedef ",sizeof(unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp5AF,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL1358;_LL1378: if(Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_add(_tmp58F);}s=({struct Cyc_PP_Doc*_tmp5B3[7];_tmp5B3[
6]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp5B3[5]=Cyc_PP_line_doc();
_tmp5B3[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(unsigned char),1),_tmp58D);_tmp5B3[3]=Cyc_PP_line_doc();
_tmp5B3[2]=Cyc_PP_text(_tag_arr(" {",sizeof(unsigned char),3));_tmp5B3[1]=Cyc_PP_textptr(
_tmp58F);_tmp5B3[0]=Cyc_PP_text(_tag_arr("namespace ",sizeof(unsigned char),11));
Cyc_PP_cat(_tag_arr(_tmp5B3,sizeof(struct Cyc_PP_Doc*),7));});if(Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_drop();}goto _LL1358;_LL1380: if(Cyc_Absynpp_print_using_stmts){
s=({struct Cyc_PP_Doc*_tmp5B4[7];_tmp5B4[6]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp5B4[5]=Cyc_PP_line_doc();_tmp5B4[4]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),
_tmp591);_tmp5B4[3]=Cyc_PP_line_doc();_tmp5B4[2]=Cyc_PP_text(_tag_arr(" {",
sizeof(unsigned char),3));_tmp5B4[1]=Cyc_Absynpp_qvar2doc(_tmp593);_tmp5B4[0]=
Cyc_PP_text(_tag_arr("using ",sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(
_tmp5B4,sizeof(struct Cyc_PP_Doc*),7));});}else{s=({struct Cyc_PP_Doc*_tmp5B5[7];
_tmp5B5[6]=Cyc_PP_text(_tag_arr("/* } */",sizeof(unsigned char),8));_tmp5B5[5]=
Cyc_PP_line_doc();_tmp5B5[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),_tmp591);_tmp5B5[3]=
Cyc_PP_line_doc();_tmp5B5[2]=Cyc_PP_text(_tag_arr(" { */",sizeof(unsigned char),
6));_tmp5B5[1]=Cyc_Absynpp_qvar2doc(_tmp593);_tmp5B5[0]=Cyc_PP_text(_tag_arr("/* using ",
sizeof(unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp5B5,sizeof(struct Cyc_PP_Doc*),
7));});}goto _LL1358;_LL1382: if(Cyc_Absynpp_print_externC_stmts){s=({struct Cyc_PP_Doc*
_tmp5B6[5];_tmp5B6[4]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp5B6[
3]=Cyc_PP_line_doc();_tmp5B6[2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),_tmp595);_tmp5B6[1]=
Cyc_PP_line_doc();_tmp5B6[0]=Cyc_PP_text(_tag_arr("extern \"C\" {",sizeof(
unsigned char),13));Cyc_PP_cat(_tag_arr(_tmp5B6,sizeof(struct Cyc_PP_Doc*),5));});}
else{s=({struct Cyc_PP_Doc*_tmp5B7[5];_tmp5B7[4]=Cyc_PP_text(_tag_arr("/* } */",
sizeof(unsigned char),8));_tmp5B7[3]=Cyc_PP_line_doc();_tmp5B7[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),
_tmp595);_tmp5B7[1]=Cyc_PP_line_doc();_tmp5B7[0]=Cyc_PP_text(_tag_arr("/* extern \"C\" { */",
sizeof(unsigned char),19));Cyc_PP_cat(_tag_arr(_tmp5B7,sizeof(struct Cyc_PP_Doc*),
5));});}goto _LL1358;_LL1358:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
void*sc){void*_tmp5B8=sc;_LL1466: if(_tmp5B8 == (void*)Cyc_Absyn_Static){goto
_LL1467;}else{goto _LL1468;}_LL1468: if(_tmp5B8 == (void*)Cyc_Absyn_Public){goto
_LL1469;}else{goto _LL1470;}_LL1470: if(_tmp5B8 == (void*)Cyc_Absyn_Extern){goto
_LL1471;}else{goto _LL1472;}_LL1472: if(_tmp5B8 == (void*)Cyc_Absyn_ExternC){goto
_LL1473;}else{goto _LL1474;}_LL1474: if(_tmp5B8 == (void*)Cyc_Absyn_Abstract){goto
_LL1475;}else{goto _LL1465;}_LL1467: return Cyc_PP_text(_tag_arr("static ",sizeof(
unsigned char),8));_LL1469: return Cyc_PP_nil_doc();_LL1471: return Cyc_PP_text(
_tag_arr("extern ",sizeof(unsigned char),8));_LL1473: return Cyc_PP_text(_tag_arr("extern \"C\" ",
sizeof(unsigned char),12));_LL1475: return Cyc_PP_text(_tag_arr("abstract ",
sizeof(unsigned char),10));_LL1465:;}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp5C4=t;struct Cyc_Absyn_Tvar*_tmp5CC;struct Cyc_List_List*_tmp5CE;
_LL1478: if((unsigned int)_tmp5C4 > 3?*((int*)_tmp5C4)== Cyc_Absyn_VarType: 0){
_LL1485: _tmp5CC=((struct Cyc_Absyn_VarType_struct*)_tmp5C4)->f1;goto _LL1479;}
else{goto _LL1480;}_LL1480: if((unsigned int)_tmp5C4 > 3?*((int*)_tmp5C4)== Cyc_Absyn_JoinEff:
0){_LL1487: _tmp5CE=((struct Cyc_Absyn_JoinEff_struct*)_tmp5C4)->f1;goto _LL1481;}
else{goto _LL1482;}_LL1482: goto _LL1483;_LL1479: return Cyc_Tcutil_is_temp_tvar(
_tmp5CC);_LL1481: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp5CE);_LL1483: return 0;_LL1477:;}int Cyc_Absynpp_is_anon_structtype(void*t){
void*_tmp5D0=t;struct _tuple0*_tmp5DC;struct Cyc_Core_Opt*_tmp5DE;struct Cyc_Core_Opt*
_tmp5E0;_LL1490: if((unsigned int)_tmp5D0 > 3?*((int*)_tmp5D0)== Cyc_Absyn_StructType:
0){_LL1501: _tmp5DC=((struct Cyc_Absyn_StructType_struct*)_tmp5D0)->f1;if(_tmp5DC
== 0){goto _LL1491;}else{goto _LL1492;}}else{goto _LL1492;}_LL1492: if((unsigned int)
_tmp5D0 > 3?*((int*)_tmp5D0)== Cyc_Absyn_AnonStructType: 0){goto _LL1493;}else{goto
_LL1494;}_LL1494: if((unsigned int)_tmp5D0 > 3?*((int*)_tmp5D0)== Cyc_Absyn_TypedefType:
0){_LL1503: _tmp5DE=((struct Cyc_Absyn_TypedefType_struct*)_tmp5D0)->f3;if(_tmp5DE
== 0){goto _LL1495;}else{goto _LL1496;}}else{goto _LL1496;}_LL1496: if((unsigned int)
_tmp5D0 > 3?*((int*)_tmp5D0)== Cyc_Absyn_TypedefType: 0){_LL1505: _tmp5E0=((struct
Cyc_Absyn_TypedefType_struct*)_tmp5D0)->f3;goto _LL1497;}else{goto _LL1498;}
_LL1498: goto _LL1499;_LL1491: return 1;_LL1493: return 1;_LL1495: return 0;_LL1497:
return Cyc_Absynpp_is_anon_structtype((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp5E0))->v);_LL1499: return 0;_LL1489:;}struct _tuple4 Cyc_Absynpp_to_tms(struct
Cyc_Absyn_Tqual tq,void*t){void*_tmp5E2=t;struct Cyc_Absyn_Exp*_tmp5F0;struct Cyc_Absyn_Tqual
_tmp5F2;void*_tmp5F4;struct Cyc_Absyn_PtrInfo _tmp5F6;struct Cyc_Absyn_Conref*
_tmp5F8;struct Cyc_Absyn_Tqual _tmp5FA;struct Cyc_Absyn_Conref*_tmp5FC;void*_tmp5FE;
void*_tmp600;struct Cyc_Absyn_FnInfo _tmp602;struct Cyc_List_List*_tmp604;struct Cyc_List_List*
_tmp606;struct Cyc_Absyn_VarargInfo*_tmp608;int _tmp60A;struct Cyc_List_List*
_tmp60C;void*_tmp60E;struct Cyc_Core_Opt*_tmp610;struct Cyc_List_List*_tmp612;int
_tmp614;struct Cyc_Core_Opt*_tmp616;struct Cyc_Core_Opt*_tmp618;struct Cyc_Core_Opt*
_tmp61A;struct Cyc_List_List*_tmp61C;struct _tuple0*_tmp61E;_LL1508: if((
unsigned int)_tmp5E2 > 3?*((int*)_tmp5E2)== Cyc_Absyn_ArrayType: 0){_LL1525:
_tmp5F4=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp5E2)->f1;goto _LL1523;
_LL1523: _tmp5F2=((struct Cyc_Absyn_ArrayType_struct*)_tmp5E2)->f2;goto _LL1521;
_LL1521: _tmp5F0=((struct Cyc_Absyn_ArrayType_struct*)_tmp5E2)->f3;goto _LL1509;}
else{goto _LL1510;}_LL1510: if((unsigned int)_tmp5E2 > 3?*((int*)_tmp5E2)== Cyc_Absyn_PointerType:
0){_LL1527: _tmp5F6=((struct Cyc_Absyn_PointerType_struct*)_tmp5E2)->f1;_LL1537:
_tmp600=(void*)_tmp5F6.elt_typ;goto _LL1535;_LL1535: _tmp5FE=(void*)_tmp5F6.rgn_typ;
goto _LL1533;_LL1533: _tmp5FC=_tmp5F6.nullable;goto _LL1531;_LL1531: _tmp5FA=_tmp5F6.tq;
goto _LL1529;_LL1529: _tmp5F8=_tmp5F6.bounds;goto _LL1511;}else{goto _LL1512;}
_LL1512: if((unsigned int)_tmp5E2 > 3?*((int*)_tmp5E2)== Cyc_Absyn_FnType: 0){
_LL1539: _tmp602=((struct Cyc_Absyn_FnType_struct*)_tmp5E2)->f1;_LL1555: _tmp612=
_tmp602.tvars;goto _LL1553;_LL1553: _tmp610=_tmp602.effect;goto _LL1551;_LL1551:
_tmp60E=(void*)_tmp602.ret_typ;goto _LL1549;_LL1549: _tmp60C=_tmp602.args;goto
_LL1547;_LL1547: _tmp60A=_tmp602.c_varargs;goto _LL1545;_LL1545: _tmp608=_tmp602.cyc_varargs;
goto _LL1543;_LL1543: _tmp606=_tmp602.rgn_po;goto _LL1541;_LL1541: _tmp604=_tmp602.attributes;
goto _LL1513;}else{goto _LL1514;}_LL1514: if((unsigned int)_tmp5E2 > 3?*((int*)
_tmp5E2)== Cyc_Absyn_Evar: 0){_LL1561: _tmp618=((struct Cyc_Absyn_Evar_struct*)
_tmp5E2)->f1;goto _LL1559;_LL1559: _tmp616=((struct Cyc_Absyn_Evar_struct*)_tmp5E2)->f2;
goto _LL1557;_LL1557: _tmp614=((struct Cyc_Absyn_Evar_struct*)_tmp5E2)->f3;goto
_LL1515;}else{goto _LL1516;}_LL1516: if((unsigned int)_tmp5E2 > 3?*((int*)_tmp5E2)
== Cyc_Absyn_TypedefType: 0){_LL1567: _tmp61E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5E2)->f1;goto _LL1565;_LL1565: _tmp61C=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5E2)->f2;goto _LL1563;_LL1563: _tmp61A=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5E2)->f3;goto _LL1517;}else{goto _LL1518;}_LL1518: goto _LL1519;_LL1509: {struct
Cyc_List_List*_tmp622;void*_tmp624;struct Cyc_Absyn_Tqual _tmp626;struct _tuple4
_tmp620=Cyc_Absynpp_to_tms(_tmp5F2,_tmp5F4);_LL1575: _tmp626=_tmp620.f1;goto
_LL1573;_LL1573: _tmp624=_tmp620.f2;goto _LL1571;_LL1571: _tmp622=_tmp620.f3;goto
_LL1569;_LL1569: {void*tm;if(_tmp5F0 == 0){tm=(void*)Cyc_Absyn_Carray_mod;}else{
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp628=_cycalloc(sizeof(
struct Cyc_Absyn_ConstArray_mod_struct));_tmp628[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp629;_tmp629.tag=Cyc_Absyn_ConstArray_mod;_tmp629.f1=(struct Cyc_Absyn_Exp*)
_check_null(_tmp5F0);_tmp629;});_tmp628;});}return({struct _tuple4 _tmp62A;_tmp62A.f1=
_tmp626;_tmp62A.f2=_tmp624;_tmp62A.f3=({struct Cyc_List_List*_tmp62B=_cycalloc(
sizeof(struct Cyc_List_List));_tmp62B->hd=(void*)tm;_tmp62B->tl=_tmp622;_tmp62B;});
_tmp62A;});}}_LL1511: {struct Cyc_List_List*_tmp62E;void*_tmp630;struct Cyc_Absyn_Tqual
_tmp632;struct _tuple4 _tmp62C=Cyc_Absynpp_to_tms(_tmp5FA,_tmp600);_LL1587: _tmp632=
_tmp62C.f1;goto _LL1585;_LL1585: _tmp630=_tmp62C.f2;goto _LL1583;_LL1583: _tmp62E=
_tmp62C.f3;goto _LL1581;_LL1581: {void*ps;{struct _tuple8 _tmp635=({struct _tuple8
_tmp634;_tmp634.f1=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))
Cyc_Absyn_compress_conref)(_tmp5FC))->v;_tmp634.f2=(void*)(Cyc_Absyn_compress_conref(
_tmp5F8))->v;_tmp634;});void*_tmp63F;void*_tmp641;struct Cyc_Absyn_Exp*_tmp643;
void*_tmp645;int _tmp647;void*_tmp649;void*_tmp64B;struct Cyc_Absyn_Exp*_tmp64D;
void*_tmp64F;int _tmp651;void*_tmp653;void*_tmp655;_LL1591: _LL1606: _tmp645=
_tmp635.f1;if((unsigned int)_tmp645 > 1?*((int*)_tmp645)== Cyc_Absyn_Eq_constr: 0){
_LL1608: _tmp647=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp645)->f1;if(_tmp647
== 1){goto _LL1600;}else{goto _LL1593;}}else{goto _LL1593;}_LL1600: _tmp63F=_tmp635.f2;
if((unsigned int)_tmp63F > 1?*((int*)_tmp63F)== Cyc_Absyn_Eq_constr: 0){_LL1602:
_tmp641=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp63F)->f1;if((unsigned int)
_tmp641 > 1?*((int*)_tmp641)== Cyc_Absyn_Upper_b: 0){_LL1604: _tmp643=((struct Cyc_Absyn_Upper_b_struct*)
_tmp641)->f1;goto _LL1592;}else{goto _LL1593;}}else{goto _LL1593;}_LL1593: _LL1616:
_tmp64F=_tmp635.f1;if((unsigned int)_tmp64F > 1?*((int*)_tmp64F)== Cyc_Absyn_Eq_constr:
0){_LL1618: _tmp651=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp64F)->f1;if(
_tmp651 == 0){goto _LL1610;}else{goto _LL1595;}}else{goto _LL1595;}_LL1610: _tmp649=
_tmp635.f2;if((unsigned int)_tmp649 > 1?*((int*)_tmp649)== Cyc_Absyn_Eq_constr: 0){
_LL1612: _tmp64B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp649)->f1;if((
unsigned int)_tmp64B > 1?*((int*)_tmp64B)== Cyc_Absyn_Upper_b: 0){_LL1614: _tmp64D=((
struct Cyc_Absyn_Upper_b_struct*)_tmp64B)->f1;goto _LL1594;}else{goto _LL1595;}}
else{goto _LL1595;}_LL1595: _LL1620: _tmp653=_tmp635.f2;if((unsigned int)_tmp653 > 1?*((
int*)_tmp653)== Cyc_Absyn_Eq_constr: 0){_LL1622: _tmp655=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp653)->f1;if(_tmp655 == (void*)Cyc_Absyn_Unknown_b){goto _LL1596;}else{goto
_LL1597;}}else{goto _LL1597;}_LL1597: goto _LL1598;_LL1592: ps=(void*)({struct Cyc_Absyn_Nullable_ps_struct*
_tmp657=_cycalloc(sizeof(struct Cyc_Absyn_Nullable_ps_struct));_tmp657[0]=({
struct Cyc_Absyn_Nullable_ps_struct _tmp658;_tmp658.tag=Cyc_Absyn_Nullable_ps;
_tmp658.f1=_tmp643;_tmp658;});_tmp657;});goto _LL1590;_LL1594: ps=(void*)({struct
Cyc_Absyn_NonNullable_ps_struct*_tmp659=_cycalloc(sizeof(struct Cyc_Absyn_NonNullable_ps_struct));
_tmp659[0]=({struct Cyc_Absyn_NonNullable_ps_struct _tmp65A;_tmp65A.tag=Cyc_Absyn_NonNullable_ps;
_tmp65A.f1=_tmp64D;_tmp65A;});_tmp659;});goto _LL1590;_LL1596: ps=(void*)Cyc_Absyn_TaggedArray_ps;
goto _LL1590;_LL1598: ps=(void*)Cyc_Absyn_TaggedArray_ps;goto _LL1590;_LL1590:;}{
void*tm=(void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp65D=_cycalloc(sizeof(
struct Cyc_Absyn_Pointer_mod_struct));_tmp65D[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp65E;_tmp65E.tag=Cyc_Absyn_Pointer_mod;_tmp65E.f1=(void*)ps;_tmp65E.f2=(void*)
_tmp5FE;_tmp65E.f3=tq;_tmp65E;});_tmp65D;});return({struct _tuple4 _tmp65B;_tmp65B.f1=
_tmp632;_tmp65B.f2=_tmp630;_tmp65B.f3=({struct Cyc_List_List*_tmp65C=_cycalloc(
sizeof(struct Cyc_List_List));_tmp65C->hd=(void*)tm;_tmp65C->tl=_tmp62E;_tmp65C;});
_tmp65B;});}}}_LL1513: if(! Cyc_Absynpp_print_all_tvars){if(_tmp610 == 0? 1: Cyc_Absynpp_exists_temp_tvar_in_effect((
void*)_tmp610->v)){_tmp610=0;_tmp612=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars){((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Tcutil_rewrite_temp_tvar,_tmp612);}}{struct Cyc_List_List*_tmp661;void*
_tmp663;struct Cyc_Absyn_Tqual _tmp665;struct _tuple4 _tmp65F=Cyc_Absynpp_to_tms(Cyc_Absyn_empty_tqual(),
_tmp60E);_LL1638: _tmp665=_tmp65F.f1;goto _LL1636;_LL1636: _tmp663=_tmp65F.f2;goto
_LL1634;_LL1634: _tmp661=_tmp65F.f3;goto _LL1632;_LL1632: {struct Cyc_List_List*tms=
_tmp661;if(_tmp604 != 0? ! Cyc_Absynpp_to_VC: 0){tms=({struct Cyc_List_List*_tmp667=
_cycalloc(sizeof(struct Cyc_List_List));_tmp667->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp668=_cycalloc(sizeof(struct Cyc_Absyn_Attributes_mod_struct));_tmp668[0]=({
struct Cyc_Absyn_Attributes_mod_struct _tmp669;_tmp669.tag=Cyc_Absyn_Attributes_mod;
_tmp669.f1=0;_tmp669.f2=_tmp604;_tmp669;});_tmp668;}));_tmp667->tl=tms;_tmp667;});}
tms=({struct Cyc_List_List*_tmp66A=_cycalloc(sizeof(struct Cyc_List_List));_tmp66A->hd=(
void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp66B=_cycalloc(sizeof(
struct Cyc_Absyn_Function_mod_struct));_tmp66B[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp66C;_tmp66C.tag=Cyc_Absyn_Function_mod;_tmp66C.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp66D=_cycalloc(sizeof(struct Cyc_Absyn_WithTypes_struct));_tmp66D[0]=({struct
Cyc_Absyn_WithTypes_struct _tmp66E;_tmp66E.tag=Cyc_Absyn_WithTypes;_tmp66E.f1=
_tmp60C;_tmp66E.f2=_tmp60A;_tmp66E.f3=_tmp608;_tmp66E.f4=_tmp610;_tmp66E.f5=
_tmp606;_tmp66E;});_tmp66D;}));_tmp66C;});_tmp66B;}));_tmp66A->tl=tms;_tmp66A;});
if(Cyc_Absynpp_to_VC){for(0;_tmp604 != 0;_tmp604=_tmp604->tl){void*_tmp66F=(void*)
_tmp604->hd;_LL1649: if(_tmp66F == (void*)Cyc_Absyn_Stdcall_att){goto _LL1650;}
else{goto _LL1651;}_LL1651: if(_tmp66F == (void*)Cyc_Absyn_Cdecl_att){goto _LL1652;}
else{goto _LL1653;}_LL1653: if(_tmp66F == (void*)Cyc_Absyn_Fastcall_att){goto
_LL1654;}else{goto _LL1655;}_LL1655: goto _LL1656;_LL1650: goto _LL1652;_LL1652: goto
_LL1654;_LL1654: tms=({struct Cyc_List_List*_tmp679=_cycalloc(sizeof(struct Cyc_List_List));
_tmp679->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp67A=
_cycalloc(sizeof(struct Cyc_Absyn_Attributes_mod_struct));_tmp67A[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp67B;_tmp67B.tag=Cyc_Absyn_Attributes_mod;_tmp67B.f1=0;_tmp67B.f2=({struct Cyc_List_List*
_tmp67C=_cycalloc(sizeof(struct Cyc_List_List));_tmp67C->hd=(void*)((void*)
_tmp604->hd);_tmp67C->tl=0;_tmp67C;});_tmp67B;});_tmp67A;}));_tmp679->tl=tms;
_tmp679;});goto AfterAtts;_LL1656: goto _LL1648;_LL1648:;}}AfterAtts: if(_tmp612 != 0){
tms=({struct Cyc_List_List*_tmp67D=_cycalloc(sizeof(struct Cyc_List_List));_tmp67D->hd=(
void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*_tmp67E=_cycalloc(sizeof(
struct Cyc_Absyn_TypeParams_mod_struct));_tmp67E[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp67F;_tmp67F.tag=Cyc_Absyn_TypeParams_mod;_tmp67F.f1=_tmp612;_tmp67F.f2=0;
_tmp67F.f3=1;_tmp67F;});_tmp67E;}));_tmp67D->tl=tms;_tmp67D;});}return({struct
_tuple4 _tmp680;_tmp680.f1=_tmp665;_tmp680.f2=_tmp663;_tmp680.f3=tms;_tmp680;});}}
_LL1515: if(_tmp616 == 0){return({struct _tuple4 _tmp681;_tmp681.f1=tq;_tmp681.f2=t;
_tmp681.f3=0;_tmp681;});}else{return Cyc_Absynpp_to_tms(tq,(void*)_tmp616->v);}
_LL1517: if((_tmp61A == 0? 1: ! Cyc_Absynpp_expand_typedefs)? 1: Cyc_Absynpp_is_anon_structtype((
void*)_tmp61A->v)){return({struct _tuple4 _tmp682;_tmp682.f1=tq;_tmp682.f2=t;
_tmp682.f3=0;_tmp682;});}else{return Cyc_Absynpp_to_tms(tq,(void*)_tmp61A->v);}
_LL1519: return({struct _tuple4 _tmp683;_tmp683.f1=tq;_tmp683.f2=t;_tmp683.f3=0;
_tmp683;});_LL1507:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_Core_Opt*dopt){struct Cyc_List_List*_tmp686;void*_tmp688;
struct Cyc_Absyn_Tqual _tmp68A;struct _tuple4 _tmp684=Cyc_Absynpp_to_tms(tq,t);
_LL1675: _tmp68A=_tmp684.f1;goto _LL1673;_LL1673: _tmp688=_tmp684.f2;goto _LL1671;
_LL1671: _tmp686=_tmp684.f3;goto _LL1669;_LL1669: _tmp686=Cyc_List_imp_rev(_tmp686);
if(_tmp686 == 0? dopt == 0: 0){return({struct Cyc_PP_Doc*_tmp68C[2];_tmp68C[1]=Cyc_Absynpp_ntyp2doc(
_tmp688);_tmp68C[0]=Cyc_Absynpp_tqual2doc(_tmp68A);Cyc_PP_cat(_tag_arr(_tmp68C,
sizeof(struct Cyc_PP_Doc*),2));});}else{return({struct Cyc_PP_Doc*_tmp68D[4];
_tmp68D[3]=Cyc_Absynpp_dtms2doc(dopt == 0? Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)
dopt->v,_tmp686);_tmp68D[2]=Cyc_PP_text(_tag_arr(" ",sizeof(unsigned char),2));
_tmp68D[1]=Cyc_Absynpp_ntyp2doc(_tmp688);_tmp68D[0]=Cyc_Absynpp_tqual2doc(
_tmp68A);Cyc_PP_cat(_tag_arr(_tmp68D,sizeof(struct Cyc_PP_Doc*),4));});}}struct
Cyc_PP_Doc*Cyc_Absynpp_structfield2doc(struct Cyc_Absyn_Structfield*f){if(f->width
!= 0){return({struct Cyc_PP_Doc*_tmp68E[5];_tmp68E[4]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp68E[3]=Cyc_Absynpp_atts2doc(f->attributes);_tmp68E[
2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp68E[1]=
Cyc_PP_text(_tag_arr(":",sizeof(unsigned char),2));_tmp68E[0]=Cyc_Absynpp_tqtd2doc(
f->tq,(void*)f->type,({struct Cyc_Core_Opt*_tmp68F=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp68F->v=Cyc_PP_textptr(f->name);_tmp68F;}));Cyc_PP_cat(_tag_arr(_tmp68E,
sizeof(struct Cyc_PP_Doc*),5));});}else{return({struct Cyc_PP_Doc*_tmp690[3];
_tmp690[2]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmp690[1]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp690[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp691=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp691->v=Cyc_PP_textptr(f->name);
_tmp691;}));Cyc_PP_cat(_tag_arr(_tmp690,sizeof(struct Cyc_PP_Doc*),3));});}}
struct Cyc_PP_Doc*Cyc_Absynpp_structfields2doc(struct Cyc_List_List*fields){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Structfield*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_structfield2doc,
_tag_arr("",sizeof(unsigned char),1),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield*f){return({struct Cyc_PP_Doc*_tmp692[4];_tmp692[3]=f->typs
== 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs);_tmp692[2]=f->tvs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_ktvars2doc(f->tvs);_tmp692[1]=Cyc_Absynpp_typedef_name2doc(f->name);
_tmp692[0]=Cyc_Absynpp_scope2doc((void*)f->sc);Cyc_PP_cat(_tag_arr(_tmp692,
sizeof(struct Cyc_PP_Doc*),4));});}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Tunionfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_tunionfield2doc,_tag_arr(",",sizeof(unsigned char),2),fields);}void
Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc_Std___sFILE*f){for(0;
tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)
tdl->hd),72,f);({void*_tmp693[0]={};Cyc_Std_fprintf(f,_tag_arr("\n",sizeof(
unsigned char),2),_tag_arr(_tmp693,sizeof(void*),0));});}}struct _tagged_arr Cyc_Absynpp_decllist2string(
struct Cyc_List_List*tdl){return Cyc_PP_string_of_doc(Cyc_PP_seql(_tag_arr("",
sizeof(unsigned char),1),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),
72);}struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(
Cyc_Absynpp_exp2doc(e),72);}struct _tagged_arr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*
s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s),72);}struct _tagged_arr Cyc_Absynpp_typ2string(
void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct _tagged_arr
Cyc_Absynpp_qvar2string(struct _tuple0*v){return Cyc_PP_string_of_doc(Cyc_Absynpp_qvar2doc(
v),72);}struct _tagged_arr Cyc_Absynpp_typ2cstring(void*t){int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=
0;{struct _tagged_arr s=Cyc_Absynpp_typ2string(t);Cyc_Absynpp_qvar_to_Cids=
old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;return s;}}struct
_tagged_arr Cyc_Absynpp_prim2string(void*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(
p),72);}struct _tagged_arr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(
Cyc_Absynpp_pat2doc(p),72);}struct _tagged_arr Cyc_Absynpp_scope2string(void*sc){
void*_tmp694=sc;_LL1686: if(_tmp694 == (void*)Cyc_Absyn_Static){goto _LL1687;}else{
goto _LL1688;}_LL1688: if(_tmp694 == (void*)Cyc_Absyn_Public){goto _LL1689;}else{
goto _LL1690;}_LL1690: if(_tmp694 == (void*)Cyc_Absyn_Extern){goto _LL1691;}else{
goto _LL1692;}_LL1692: if(_tmp694 == (void*)Cyc_Absyn_ExternC){goto _LL1693;}else{
goto _LL1694;}_LL1694: if(_tmp694 == (void*)Cyc_Absyn_Abstract){goto _LL1695;}else{
goto _LL1685;}_LL1687: return _tag_arr("static",sizeof(unsigned char),7);_LL1689:
return _tag_arr("public",sizeof(unsigned char),7);_LL1691: return _tag_arr("extern",
sizeof(unsigned char),7);_LL1693: return _tag_arr("extern \"C\"",sizeof(
unsigned char),11);_LL1695: return _tag_arr("abstract",sizeof(unsigned char),9);
_LL1685:;}
