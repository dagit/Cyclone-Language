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
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Cstdio___sFILE;struct
Cyc_Std___sFILE;extern struct Cyc_Std___sFILE*Cyc_Std_stderr;extern int Cyc_Std_fflush(
struct Cyc_Std___sFILE*);extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};static const int Cyc_Std_String_pa=0;struct
Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Std_Int_pa=
1;struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};static const int Cyc_Std_Double_pa=
2;struct Cyc_Std_Double_pa_struct{int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=
3;struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=
4;struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int Cyc_Std_fprintf(
struct Cyc_Std___sFILE*,struct _tagged_arr fmt,struct _tagged_arr);extern struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_vfprintf(
struct Cyc_Std___sFILE*,struct _tagged_arr fmt,struct _tagged_arr ap);static const int
Cyc_Std_ShortPtr_sa=0;struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};static
const int Cyc_Std_UShortPtr_sa=1;struct Cyc_Std_UShortPtr_sa_struct{int tag;
unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=2;struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=3;struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=4;struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern
struct Cyc_List_List*Cyc_List_list(struct _tagged_arr);extern int Cyc_List_length(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_rmap(struct
_RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern unsigned char Cyc_List_List_mismatch[18];extern void Cyc_List_iter_c(void(*
f)(void*,void*),void*env,struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_rrev(struct
_RegionHandle*,struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern unsigned char Cyc_List_Nth[8];extern void*Cyc_List_nth(
struct Cyc_List_List*x,int n);extern int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*
x);extern int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);extern
struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);struct
Cyc_Set_Set;extern struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));extern
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);extern int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];
extern struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));extern struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);extern void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict*d,void*k);extern struct Cyc_Core_Opt*Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict*d,void*k);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;
struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;extern struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);static const int Cyc_Position_Lex=0;static const int Cyc_Position_Parse=
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
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);extern struct Cyc_Absyn_Rel_n_struct
Cyc_Absyn_rel_ns_null_value;extern void*Cyc_Absyn_rel_ns_null;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_conref_def(
void*,struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_uchar_t;extern void*Cyc_Absyn_uint_t;
extern void*Cyc_Absyn_sint_t;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
extern void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);extern void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern void*Cyc_Absyn_void_star_typ();
extern void*Cyc_Absyn_strct(struct _tagged_arr*name);extern void*Cyc_Absyn_strctq(
struct _tuple0*name);extern void*Cyc_Absyn_unionq_typ(struct _tuple0*name);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_char_exp(unsigned char c,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_float_exp(struct _tagged_arr f,struct Cyc_Position_Segment*);extern struct
Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _tagged_arr s,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*
Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_structmember_exp(
struct Cyc_Absyn_Exp*,struct _tagged_arr*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_structarrow_exp(struct Cyc_Absyn_Exp*,struct
_tagged_arr*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(
void*s,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct
Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct
Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(
struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _tagged_arr*v,struct
Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _tagged_arr*lab,struct Cyc_Position_Segment*loc);extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);extern struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,
struct Cyc_Absyn_Exp*init);extern struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);extern int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Structfield*Cyc_Absyn_lookup_field(
struct Cyc_List_List*,struct _tagged_arr*);extern struct _tagged_arr*Cyc_Absyn_fieldname(
int);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(void*);extern
struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);extern struct
_tagged_arr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);extern struct
_tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*);static const int Cyc_Tcenv_VarRes=
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
ae;struct Cyc_Core_Opt*le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*
Cyc_Tcutil_impos(struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);extern void
Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct
_tagged_arr ap);extern void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);extern
void*Cyc_Tcutil_typ_kind(void*t);extern void*Cyc_Tcutil_compress(void*t);extern
int Cyc_Tcutil_is_pointer_type(void*t);extern int Cyc_Tcutil_unify(void*,void*);
extern struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields);extern int Cyc_Tcutil_is_tagged_pointer_typ(void*);extern int
Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);struct _tuple3{
struct Cyc_Absyn_Tqual f1;void*f2;};extern void*Cyc_Tcutil_snd_tqt(struct _tuple3*);
extern unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);static
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
f2;};extern struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);extern struct
_tuple0*Cyc_Toc_temp_var();int Cyc_Toc_warn_bounds_checks=0;unsigned int Cyc_Toc_total_bounds_checks=
0;unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;static struct Cyc_List_List*Cyc_Toc_tuple_types=0;static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
unsigned char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented";
unsigned char Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible";static
void*Cyc_Toc_unimp(struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Std_vfprintf(
Cyc_Std_stderr,fmt,ap);({void*_tmp0[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\n",sizeof(unsigned char),2),_tag_arr(_tmp0,sizeof(void*),0));});Cyc_Std_fflush((
struct Cyc_Std___sFILE*)Cyc_Std_stderr);(int)_throw((void*)Cyc_Toc_Toc_Unimplemented);}
static void*Cyc_Toc_toc_impos(struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Std_vfprintf(
Cyc_Std_stderr,fmt,ap);({void*_tmp1[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\n",sizeof(unsigned char),2),_tag_arr(_tmp1,sizeof(void*),0));});Cyc_Std_fflush((
struct Cyc_Std___sFILE*)Cyc_Std_stderr);(int)_throw((void*)Cyc_Toc_Toc_Impossible);}
unsigned char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error";static
unsigned char _tmp2[5]="curr";static struct _tagged_arr Cyc_Toc_curr_string={_tmp2,
_tmp2,_tmp2 + 5};static struct _tagged_arr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static unsigned char _tmp3[4]="tag";static struct _tagged_arr Cyc_Toc_tag_string={
_tmp3,_tmp3,_tmp3 + 4};static struct _tagged_arr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;
static unsigned char _tmp4[14]="_handler_cons";static struct _tagged_arr Cyc_Toc__handler_cons_string={
_tmp4,_tmp4,_tmp4 + 14};static struct _tagged_arr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static unsigned char _tmp5[8]="handler";static struct _tagged_arr Cyc_Toc_handler_string={
_tmp5,_tmp5,_tmp5 + 8};static struct _tagged_arr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static unsigned char _tmp6[14]="_RegionHandle";static struct _tagged_arr Cyc_Toc__RegionHandle_string={
_tmp6,_tmp6,_tmp6 + 14};static struct _tagged_arr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static unsigned char _tmp7[7]="_throw";static struct _tagged_arr Cyc_Toc__throw_str={
_tmp7,_tmp7,_tmp7 + 7};static struct _tuple0 Cyc_Toc__throw_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)((void*)&
Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*
Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static unsigned char _tmp9[7]="setjmp";static
struct _tagged_arr Cyc_Toc_setjmp_str={_tmp9,_tmp9,_tmp9 + 7};static struct _tuple0
Cyc_Toc_setjmp_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc_setjmp_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)((void*)& Cyc_Toc_setjmp_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=&
Cyc_Toc_setjmp_ev;static unsigned char _tmpB[14]="_push_handler";static struct
_tagged_arr Cyc_Toc__push_handler_str={_tmpB,_tmpB,_tmpB + 14};static struct _tuple0
Cyc_Toc__push_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_handler_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((
void*)& Cyc_Toc__push_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static
unsigned char _tmpD[13]="_pop_handler";static struct _tagged_arr Cyc_Toc__pop_handler_str={
_tmpD,_tmpD,_tmpD + 13};static struct _tuple0 Cyc_Toc__pop_handler_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
unsigned char _tmpF[12]="_exn_thrown";static struct _tagged_arr Cyc_Toc__exn_thrown_str={
_tmpF,_tmpF,_tmpF + 12};static struct _tuple0 Cyc_Toc__exn_thrown_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__exn_thrown_re={
1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={
0,(void*)((void*)& Cyc_Toc__exn_thrown_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static
unsigned char _tmp11[14]="_npop_handler";static struct _tagged_arr Cyc_Toc__npop_handler_str={
_tmp11,_tmp11,_tmp11 + 14};static struct _tuple0 Cyc_Toc__npop_handler_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)((void*)& Cyc_Toc__npop_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=&
Cyc_Toc__npop_handler_ev;static unsigned char _tmp13[12]="_check_null";static
struct _tagged_arr Cyc_Toc__check_null_str={_tmp13,_tmp13,_tmp13 + 12};static struct
_tuple0 Cyc_Toc__check_null_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((
void*)& Cyc_Toc__check_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static
unsigned char _tmp15[28]="_check_known_subscript_null";static struct _tagged_arr Cyc_Toc__check_known_subscript_null_str={
_tmp15,_tmp15,_tmp15 + 28};static struct _tuple0 Cyc_Toc__check_known_subscript_null_pr={(
void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static unsigned char _tmp17[31]="_check_known_subscript_notnull";static struct
_tagged_arr Cyc_Toc__check_known_subscript_notnull_str={_tmp17,_tmp17,_tmp17 + 31};
static struct _tuple0 Cyc_Toc__check_known_subscript_notnull_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_known_subscript_notnull_re={1,& Cyc_Toc__check_known_subscript_notnull_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static unsigned char _tmp19[25]="_check_unknown_subscript";
static struct _tagged_arr Cyc_Toc__check_unknown_subscript_str={_tmp19,_tmp19,
_tmp19 + 25};static struct _tuple0 Cyc_Toc__check_unknown_subscript_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__check_unknown_subscript_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_unknown_subscript_re={
1,& Cyc_Toc__check_unknown_subscript_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_unknown_subscript_ev={0,(void*)((void*)& Cyc_Toc__check_unknown_subscript_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_unknown_subscript_e=&
Cyc_Toc__check_unknown_subscript_ev;static unsigned char _tmp1B[12]="_tagged_arr";
static struct _tagged_arr Cyc_Toc__tagged_arr_str={_tmp1B,_tmp1B,_tmp1B + 12};static
struct _tuple0 Cyc_Toc__tagged_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_re={1,& Cyc_Toc__tagged_arr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_ev={0,(void*)((
void*)& Cyc_Toc__tagged_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_e=& Cyc_Toc__tagged_arr_ev;static
unsigned char _tmp1D[9]="_tag_arr";static struct _tagged_arr Cyc_Toc__tag_arr_str={
_tmp1D,_tmp1D,_tmp1D + 9};static struct _tuple0 Cyc_Toc__tag_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tag_arr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_arr_re={1,&
Cyc_Toc__tag_arr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_arr_ev={
0,(void*)((void*)& Cyc_Toc__tag_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_arr_e=& Cyc_Toc__tag_arr_ev;static
unsigned char _tmp1F[14]="_init_tag_arr";static struct _tagged_arr Cyc_Toc__init_tag_arr_str={
_tmp1F,_tmp1F,_tmp1F + 14};static struct _tuple0 Cyc_Toc__init_tag_arr_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__init_tag_arr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__init_tag_arr_re={1,& Cyc_Toc__init_tag_arr_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__init_tag_arr_ev={0,(void*)((void*)& Cyc_Toc__init_tag_arr_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__init_tag_arr_e=&
Cyc_Toc__init_tag_arr_ev;static unsigned char _tmp21[11]="_untag_arr";static struct
_tagged_arr Cyc_Toc__untag_arr_str={_tmp21,_tmp21,_tmp21 + 11};static struct _tuple0
Cyc_Toc__untag_arr_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__untag_arr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_arr_re={1,& Cyc_Toc__untag_arr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_arr_ev={0,(void*)((
void*)& Cyc_Toc__untag_arr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__untag_arr_e=& Cyc_Toc__untag_arr_ev;static
unsigned char _tmp23[14]="_get_arr_size";static struct _tagged_arr Cyc_Toc__get_arr_size_str={
_tmp23,_tmp23,_tmp23 + 14};static struct _tuple0 Cyc_Toc__get_arr_size_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__get_arr_size_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__get_arr_size_re={1,& Cyc_Toc__get_arr_size_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__get_arr_size_ev={0,(void*)((void*)& Cyc_Toc__get_arr_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__get_arr_size_e=&
Cyc_Toc__get_arr_size_ev;static unsigned char _tmp25[17]="_tagged_arr_plus";static
struct _tagged_arr Cyc_Toc__tagged_arr_plus_str={_tmp25,_tmp25,_tmp25 + 17};static
struct _tuple0 Cyc_Toc__tagged_arr_plus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tagged_arr_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_plus_re={
1,& Cyc_Toc__tagged_arr_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_plus_ev={
0,(void*)((void*)& Cyc_Toc__tagged_arr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_plus_e=& Cyc_Toc__tagged_arr_plus_ev;
static unsigned char _tmp27[25]="_tagged_arr_inplace_plus";static struct _tagged_arr
Cyc_Toc__tagged_arr_inplace_plus_str={_tmp27,_tmp27,_tmp27 + 25};static struct
_tuple0 Cyc_Toc__tagged_arr_inplace_plus_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__tagged_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_re={
1,& Cyc_Toc__tagged_arr_inplace_plus_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__tagged_arr_inplace_plus_ev={0,(void*)((void*)& Cyc_Toc__tagged_arr_inplace_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_inplace_plus_e=&
Cyc_Toc__tagged_arr_inplace_plus_ev;static unsigned char _tmp29[30]="_tagged_arr_inplace_plus_post";
static struct _tagged_arr Cyc_Toc__tagged_arr_inplace_plus_post_str={_tmp29,_tmp29,
_tmp29 + 30};static struct _tuple0 Cyc_Toc__tagged_arr_inplace_plus_post_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__tagged_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__tagged_arr_inplace_plus_post_re={1,& Cyc_Toc__tagged_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tagged_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__tagged_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tagged_arr_inplace_plus_post_e=& Cyc_Toc__tagged_arr_inplace_plus_post_ev;
static unsigned char _tmp2B[10]="_cycalloc";static struct _tagged_arr Cyc_Toc__cycalloc_str={
_tmp2B,_tmp2B,_tmp2B + 10};static struct _tuple0 Cyc_Toc__cycalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_re={1,&
Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static
unsigned char _tmp2D[11]="_cyccalloc";static struct _tagged_arr Cyc_Toc__cyccalloc_str={
_tmp2D,_tmp2D,_tmp2D + 11};static struct _tuple0 Cyc_Toc__cyccalloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_re={
1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static
unsigned char _tmp2F[17]="_cycalloc_atomic";static struct _tagged_arr Cyc_Toc__cycalloc_atomic_str={
_tmp2F,_tmp2F,_tmp2F + 17};static struct _tuple0 Cyc_Toc__cycalloc_atomic_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=&
Cyc_Toc__cycalloc_atomic_ev;static unsigned char _tmp31[18]="_cyccalloc_atomic";
static struct _tagged_arr Cyc_Toc__cyccalloc_atomic_str={_tmp31,_tmp31,_tmp31 + 18};
static struct _tuple0 Cyc_Toc__cyccalloc_atomic_pr={(void*)& Cyc_Absyn_rel_ns_null_value,&
Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={
1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static unsigned char _tmp33[15]="_region_malloc";static struct _tagged_arr Cyc_Toc__region_malloc_str={
_tmp33,_tmp33,_tmp33 + 15};static struct _tuple0 Cyc_Toc__region_malloc_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((void*)& Cyc_Toc__region_malloc_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static unsigned char _tmp35[15]="_region_calloc";static
struct _tagged_arr Cyc_Toc__region_calloc_str={_tmp35,_tmp35,_tmp35 + 15};static
struct _tuple0 Cyc_Toc__region_calloc_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__region_calloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)((
void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static
unsigned char _tmp37[13]="_check_times";static struct _tagged_arr Cyc_Toc__check_times_str={
_tmp37,_tmp37,_tmp37 + 13};static struct _tuple0 Cyc_Toc__check_times_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static unsigned char _tmp39[12]="_new_region";static struct
_tagged_arr Cyc_Toc__new_region_str={_tmp39,_tmp39,_tmp39 + 12};static struct
_tuple0 Cyc_Toc__new_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__new_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)((
void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
unsigned char _tmp3B[13]="_push_region";static struct _tagged_arr Cyc_Toc__push_region_str={
_tmp3B,_tmp3B,_tmp3B + 13};static struct _tuple0 Cyc_Toc__push_region_pr={(void*)&
Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static unsigned char _tmp3D[12]="_pop_region";static struct
_tagged_arr Cyc_Toc__pop_region_str={_tmp3D,_tmp3D,_tmp3D + 12};static struct
_tuple0 Cyc_Toc__pop_region_pr={(void*)& Cyc_Absyn_rel_ns_null_value,& Cyc_Toc__pop_region_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)((
void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static struct
Cyc_Absyn_StructType_struct Cyc_Toc_tagged_arr_typ_v={10,(struct _tuple0*)& Cyc_Toc__tagged_arr_pr,
0,0};static void*Cyc_Toc_tagged_arr_typ=(void*)& Cyc_Toc_tagged_arr_typ_v;static
struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0};struct _tuple4{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmp40=(void*)e->r;void*_tmp5A;unsigned char _tmp5C;void*_tmp5E;short _tmp60;void*
_tmp62;int _tmp64;void*_tmp66;long long _tmp68;void*_tmp6A;struct Cyc_Absyn_Exp*
_tmp6C;struct Cyc_List_List*_tmp6E;struct Cyc_List_List*_tmp70;struct Cyc_List_List*
_tmp72;struct Cyc_List_List*_tmp74;struct Cyc_List_List*_tmp76;_LL66: if(*((int*)
_tmp40)== Cyc_Absyn_Const_e){_LL91: _tmp5A=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp40)->f1;if((unsigned int)_tmp5A > 1?*((int*)_tmp5A)== Cyc_Absyn_Char_c: 0){
_LL93: _tmp5C=((struct Cyc_Absyn_Char_c_struct*)_tmp5A)->f2;goto _LL67;}else{goto
_LL68;}}else{goto _LL68;}_LL68: if(*((int*)_tmp40)== Cyc_Absyn_Const_e){_LL95:
_tmp5E=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp40)->f1;if((unsigned int)
_tmp5E > 1?*((int*)_tmp5E)== Cyc_Absyn_Short_c: 0){_LL97: _tmp60=((struct Cyc_Absyn_Short_c_struct*)
_tmp5E)->f2;goto _LL69;}else{goto _LL70;}}else{goto _LL70;}_LL70: if(*((int*)_tmp40)
== Cyc_Absyn_Const_e){_LL99: _tmp62=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp40)->f1;if((unsigned int)_tmp62 > 1?*((int*)_tmp62)== Cyc_Absyn_Int_c: 0){
_LL101: _tmp64=((struct Cyc_Absyn_Int_c_struct*)_tmp62)->f2;goto _LL71;}else{goto
_LL72;}}else{goto _LL72;}_LL72: if(*((int*)_tmp40)== Cyc_Absyn_Const_e){_LL103:
_tmp66=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp40)->f1;if((unsigned int)
_tmp66 > 1?*((int*)_tmp66)== Cyc_Absyn_LongLong_c: 0){_LL105: _tmp68=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp66)->f2;goto _LL73;}else{goto _LL74;}}else{goto _LL74;}_LL74: if(*((int*)_tmp40)
== Cyc_Absyn_Const_e){_LL107: _tmp6A=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp40)->f1;if(_tmp6A == (void*)Cyc_Absyn_Null_c){goto _LL75;}else{goto _LL76;}}
else{goto _LL76;}_LL76: if(*((int*)_tmp40)== Cyc_Absyn_Cast_e){_LL109: _tmp6C=((
struct Cyc_Absyn_Cast_e_struct*)_tmp40)->f2;goto _LL77;}else{goto _LL78;}_LL78: if(*((
int*)_tmp40)== Cyc_Absyn_Tuple_e){_LL111: _tmp6E=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp40)->f1;goto _LL79;}else{goto _LL80;}_LL80: if(*((int*)_tmp40)== Cyc_Absyn_Array_e){
_LL113: _tmp70=((struct Cyc_Absyn_Array_e_struct*)_tmp40)->f1;goto _LL81;}else{goto
_LL82;}_LL82: if(*((int*)_tmp40)== Cyc_Absyn_Struct_e){_LL115: _tmp72=((struct Cyc_Absyn_Struct_e_struct*)
_tmp40)->f3;goto _LL83;}else{goto _LL84;}_LL84: if(*((int*)_tmp40)== Cyc_Absyn_CompoundLit_e){
_LL117: _tmp74=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp40)->f2;goto _LL85;}
else{goto _LL86;}_LL86: if(*((int*)_tmp40)== Cyc_Absyn_UnresolvedMem_e){_LL119:
_tmp76=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp40)->f2;goto _LL87;}else{
goto _LL88;}_LL88: goto _LL89;_LL67: return _tmp5C == '\000';_LL69: return _tmp60 == 0;
_LL71: return _tmp64 == 0;_LL73: return _tmp68 == 0;_LL75: return 1;_LL77: return Cyc_Toc_is_zero(
_tmp6C);_LL79: return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp6E);_LL81: _tmp72=_tmp70;goto _LL83;_LL83:
_tmp74=_tmp72;goto _LL85;_LL85: _tmp76=_tmp74;goto _LL87;_LL87: for(0;_tmp76 != 0;
_tmp76=_tmp76->tl){if(! Cyc_Toc_is_zero((*((struct _tuple4*)_tmp76->hd)).f2)){
return 0;}}return 1;_LL89: return 0;_LL65:;}static int Cyc_Toc_is_nullable(void*t){
void*_tmp78=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp7E;struct Cyc_Absyn_Conref*
_tmp80;_LL122: if((unsigned int)_tmp78 > 3?*((int*)_tmp78)== Cyc_Absyn_PointerType:
0){_LL127: _tmp7E=((struct Cyc_Absyn_PointerType_struct*)_tmp78)->f1;_LL129: _tmp80=
_tmp7E.nullable;goto _LL123;}else{goto _LL124;}_LL124: goto _LL125;_LL123: return((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp80);_LL125:({
void*_tmp82[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("is_nullable",sizeof(unsigned char),12),_tag_arr(_tmp82,sizeof(void*),0));});
return 0;_LL121:;}static struct _tuple0*Cyc_Toc_collapse_qvar_tag(struct _tuple0*x,
struct _tagged_arr tag){return({struct _tuple0*_tmp83=_cycalloc(sizeof(struct
_tuple0));_tmp83->f1=(*x).f1;_tmp83->f2=({struct _tagged_arr*_tmp84=_cycalloc(
sizeof(struct _tagged_arr));_tmp84[0]=(struct _tagged_arr)Cyc_Std_strconcat(*(*x).f2,
tag);_tmp84;});_tmp83;});}struct _tuple5{struct _tagged_arr*f1;struct Cyc_List_List*
f2;};static struct _tagged_arr*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){{
struct Cyc_List_List*_tmp85=Cyc_Toc_tuple_types;for(0;_tmp85 != 0;_tmp85=_tmp85->tl){
struct _tuple5 _tmp88;struct Cyc_List_List*_tmp89;struct _tagged_arr*_tmp8B;struct
_tuple5*_tmp86=(struct _tuple5*)_tmp85->hd;_tmp88=*_tmp86;_LL140: _tmp8B=_tmp88.f1;
goto _LL138;_LL138: _tmp89=_tmp88.f2;goto _LL135;_LL135: {struct Cyc_List_List*
_tmp8D=tqs0;for(0;_tmp8D != 0? _tmp89 != 0: 0;(_tmp8D=_tmp8D->tl,_tmp89=_tmp89->tl)){
if(! Cyc_Tcutil_unify((*((struct _tuple3*)_tmp8D->hd)).f2,(void*)_tmp89->hd)){
break;}}if(_tmp8D == 0? _tmp89 == 0: 0){return _tmp8B;}}}}{struct _tagged_arr*x=({
struct _tagged_arr*_tmp9B=_cycalloc(sizeof(struct _tagged_arr));_tmp9B[0]=(struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp9D;_tmp9D.tag=Cyc_Std_Int_pa;_tmp9D.f1=(
int)((unsigned int)Cyc_Toc_tuple_type_counter ++);{void*_tmp9C[1]={& _tmp9D};Cyc_Std_aprintf(
_tag_arr("_tuple%d",sizeof(unsigned char),9),_tag_arr(_tmp9C,sizeof(void*),1));}});
_tmp9B;});struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple3*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_snd_tqt,tqs0);struct
Cyc_List_List*_tmp8E=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,
i ++)){_tmp8E=({struct Cyc_List_List*_tmp8F=_cycalloc(sizeof(struct Cyc_List_List));
_tmp8F->hd=({struct Cyc_Absyn_Structfield*_tmp90=_cycalloc(sizeof(struct Cyc_Absyn_Structfield));
_tmp90->name=Cyc_Absyn_fieldname(i);_tmp90->tq=Cyc_Toc_mt_tq;_tmp90->type=(void*)((
void*)ts2->hd);_tmp90->width=0;_tmp90->attributes=0;_tmp90;});_tmp8F->tl=_tmp8E;
_tmp8F;});}}_tmp8E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp8E);{struct Cyc_Absyn_Structdecl*_tmp91=({struct Cyc_Absyn_Structdecl*_tmp97=
_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));_tmp97->sc=(void*)((void*)Cyc_Absyn_Public);
_tmp97->name=({struct Cyc_Core_Opt*_tmp99=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp99->v=({struct _tuple0*_tmp9A=_cycalloc(sizeof(struct _tuple0));_tmp9A->f1=Cyc_Absyn_rel_ns_null;
_tmp9A->f2=x;_tmp9A;});_tmp99;});_tmp97->tvs=0;_tmp97->fields=({struct Cyc_Core_Opt*
_tmp98=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp98->v=_tmp8E;_tmp98;});_tmp97->attributes=
0;_tmp97;});Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp92=_cycalloc(sizeof(
struct Cyc_List_List));_tmp92->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Struct_d_struct*
_tmp93=_cycalloc(sizeof(struct Cyc_Absyn_Struct_d_struct));_tmp93[0]=({struct Cyc_Absyn_Struct_d_struct
_tmp94;_tmp94.tag=Cyc_Absyn_Struct_d;_tmp94.f1=_tmp91;_tmp94;});_tmp93;}),0);
_tmp92->tl=Cyc_Toc_result_decls;_tmp92;});Cyc_Toc_tuple_types=({struct Cyc_List_List*
_tmp95=_cycalloc(sizeof(struct Cyc_List_List));_tmp95->hd=({struct _tuple5*_tmp96=
_cycalloc(sizeof(struct _tuple5));_tmp96->f1=x;_tmp96->f2=ts;_tmp96;});_tmp95->tl=
Cyc_Toc_tuple_types;_tmp95;});return x;}}}struct _tuple0*Cyc_Toc_temp_var(){return({
struct _tuple0*_tmp9E=_cycalloc(sizeof(struct _tuple0));_tmp9E->f1=(void*)Cyc_Absyn_Loc_n;
_tmp9E->f2=({struct _tagged_arr*_tmp9F=_cycalloc(sizeof(struct _tagged_arr));
_tmp9F[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpA1;_tmpA1.tag=Cyc_Std_Int_pa;
_tmpA1.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;{void*_tmpA0[1]={& _tmpA1};Cyc_Std_aprintf(
_tag_arr("_tmp%X",sizeof(unsigned char),7),_tag_arr(_tmpA0,sizeof(void*),1));}});
_tmp9F;});_tmp9E;});}static struct _tagged_arr*Cyc_Toc_fresh_label(){return({
struct _tagged_arr*_tmpA2=_cycalloc(sizeof(struct _tagged_arr));_tmpA2[0]=(struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpA4;_tmpA4.tag=Cyc_Std_Int_pa;_tmpA4.f1=(
int)((unsigned int)Cyc_Toc_temp_var_counter ++);{void*_tmpA3[1]={& _tmpA4};Cyc_Std_aprintf(
_tag_arr("_LL%d",sizeof(unsigned char),6),_tag_arr(_tmpA3,sizeof(void*),1));}});
_tmpA2;});}static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple1*Cyc_Toc_arg_to_c(
struct _tuple1*a){void*_tmpA7;struct Cyc_Absyn_Tqual _tmpA9;struct Cyc_Core_Opt*
_tmpAB;struct _tuple1 _tmpA5=*a;_LL172: _tmpAB=_tmpA5.f1;goto _LL170;_LL170: _tmpA9=
_tmpA5.f2;goto _LL168;_LL168: _tmpA7=_tmpA5.f3;goto _LL166;_LL166: return({struct
_tuple1*_tmpAD=_cycalloc(sizeof(struct _tuple1));_tmpAD->f1=_tmpAB;_tmpAD->f2=
_tmpA9;_tmpAD->f3=Cyc_Toc_typ_to_c(_tmpA7);_tmpAD;});}static struct _tuple3*Cyc_Toc_typ_to_c_f(
struct _tuple3*x){void*_tmpB0;struct Cyc_Absyn_Tqual _tmpB2;struct _tuple3 _tmpAE=*x;
_LL179: _tmpB2=_tmpAE.f1;goto _LL177;_LL177: _tmpB0=_tmpAE.f2;goto _LL175;_LL175:
return({struct _tuple3*_tmpB4=_cycalloc(sizeof(struct _tuple3));_tmpB4->f1=_tmpB2;
_tmpB4->f2=Cyc_Toc_typ_to_c(_tmpB0);_tmpB4;});}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmpB5=t;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Tqual _tmpBF;
void*_tmpC1;struct Cyc_Core_Opt*_tmpC3;struct Cyc_Core_Opt _tmpC5;void*_tmpC6;
_LL183: if((unsigned int)_tmpB5 > 3?*((int*)_tmpB5)== Cyc_Absyn_ArrayType: 0){
_LL194: _tmpC1=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpB5)->f1;goto _LL192;
_LL192: _tmpBF=((struct Cyc_Absyn_ArrayType_struct*)_tmpB5)->f2;goto _LL190;_LL190:
_tmpBD=((struct Cyc_Absyn_ArrayType_struct*)_tmpB5)->f3;goto _LL184;}else{goto
_LL185;}_LL185: if((unsigned int)_tmpB5 > 3?*((int*)_tmpB5)== Cyc_Absyn_Evar: 0){
_LL196: _tmpC3=((struct Cyc_Absyn_Evar_struct*)_tmpB5)->f2;if(_tmpC3 == 0){goto
_LL187;}else{_tmpC5=*_tmpC3;_LL199: _tmpC6=(void*)_tmpC5.v;goto _LL186;}}else{goto
_LL187;}_LL187: goto _LL188;_LL184: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmpC8=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmpC8[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpC9;_tmpC9.tag=Cyc_Absyn_ArrayType;_tmpC9.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmpC1);_tmpC9.f2=_tmpBF;_tmpC9.f3=_tmpBD;_tmpC9;});_tmpC8;});_LL186: return Cyc_Toc_typ_to_c_array(
_tmpC6);_LL188: return Cyc_Toc_typ_to_c(t);_LL182:;}static struct Cyc_Absyn_Structfield*
Cyc_Toc_structfield_to_c(struct Cyc_Absyn_Structfield*f){return({struct Cyc_Absyn_Structfield*
_tmpCA=_cycalloc(sizeof(struct Cyc_Absyn_Structfield));_tmpCA->name=f->name;
_tmpCA->tq=f->tq;_tmpCA->type=(void*)Cyc_Toc_typ_to_c((void*)f->type);_tmpCA->width=
f->width;_tmpCA->attributes=f->attributes;_tmpCA;});}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){struct Cyc_List_List*_tmpCB=fs;for(0;_tmpCB != 0;_tmpCB=
_tmpCB->tl){if(((struct Cyc_Absyn_Enumfield*)_tmpCB->hd)->tag != 0){unsigned int i=
Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)
_tmpCB->hd)->tag));((struct Cyc_Absyn_Enumfield*)_tmpCB->hd)->tag=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(i,0);}}}static void*Cyc_Toc_typ_to_c(void*t){void*_tmpCC=t;
struct Cyc_Core_Opt*_tmp106;struct Cyc_Core_Opt*_tmp108;struct Cyc_Core_Opt _tmp10A;
void*_tmp10B;struct Cyc_Absyn_Tvar*_tmp10D;struct Cyc_Absyn_TunionInfo _tmp10F;void*
_tmp111;struct Cyc_Absyn_TunionFieldInfo _tmp113;void*_tmp115;struct Cyc_Absyn_Tunionfield*
_tmp117;struct Cyc_Absyn_Tuniondecl*_tmp119;struct Cyc_Absyn_PtrInfo _tmp11B;struct
Cyc_Absyn_Conref*_tmp11D;struct Cyc_Absyn_Tqual _tmp11F;void*_tmp121;struct Cyc_Absyn_Exp*
_tmp123;struct Cyc_Absyn_Tqual _tmp125;void*_tmp127;struct Cyc_Absyn_FnInfo _tmp129;
struct Cyc_List_List*_tmp12B;struct Cyc_Absyn_VarargInfo*_tmp12D;int _tmp12F;struct
Cyc_List_List*_tmp131;void*_tmp133;struct Cyc_List_List*_tmp135;struct Cyc_List_List*
_tmp137;struct Cyc_List_List*_tmp139;struct _tuple0*_tmp13B;struct _tuple0*_tmp13D;
struct _tuple0*_tmp13F;struct Cyc_List_List*_tmp141;struct Cyc_Core_Opt*_tmp143;
struct Cyc_List_List*_tmp145;struct _tuple0*_tmp147;void*_tmp149;_LL206: if(_tmpCC
== (void*)Cyc_Absyn_VoidType){goto _LL207;}else{goto _LL208;}_LL208: if((
unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_Evar: 0){_LL263: _tmp106=((
struct Cyc_Absyn_Evar_struct*)_tmpCC)->f2;if(_tmp106 == 0){goto _LL209;}else{goto
_LL210;}}else{goto _LL210;}_LL210: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== 
Cyc_Absyn_Evar: 0){_LL265: _tmp108=((struct Cyc_Absyn_Evar_struct*)_tmpCC)->f2;if(
_tmp108 == 0){goto _LL212;}else{_tmp10A=*_tmp108;_LL268: _tmp10B=(void*)_tmp10A.v;
goto _LL211;}}else{goto _LL212;}_LL212: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)
== Cyc_Absyn_VarType: 0){_LL270: _tmp10D=((struct Cyc_Absyn_VarType_struct*)_tmpCC)->f1;
goto _LL213;}else{goto _LL214;}_LL214: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== 
Cyc_Absyn_TunionType: 0){_LL272: _tmp10F=((struct Cyc_Absyn_TunionType_struct*)
_tmpCC)->f1;_LL274: _tmp111=(void*)_tmp10F.tunion_info;if(*((int*)_tmp111)== Cyc_Absyn_KnownTunion){
goto _LL215;}else{goto _LL216;}}else{goto _LL216;}_LL216: if((unsigned int)_tmpCC > 3?*((
int*)_tmpCC)== Cyc_Absyn_TunionType: 0){goto _LL217;}else{goto _LL218;}_LL218: if((
unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_TunionFieldType: 0){_LL276:
_tmp113=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpCC)->f1;_LL278: _tmp115=(
void*)_tmp113.field_info;if(*((int*)_tmp115)== Cyc_Absyn_KnownTunionfield){
_LL282: _tmp119=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp115)->f1;goto
_LL280;_LL280: _tmp117=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp115)->f2;
goto _LL219;}else{goto _LL220;}}else{goto _LL220;}_LL220: if((unsigned int)_tmpCC > 3?*((
int*)_tmpCC)== Cyc_Absyn_TunionFieldType: 0){goto _LL221;}else{goto _LL222;}_LL222:
if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_PointerType: 0){_LL284:
_tmp11B=((struct Cyc_Absyn_PointerType_struct*)_tmpCC)->f1;_LL290: _tmp121=(void*)
_tmp11B.elt_typ;goto _LL288;_LL288: _tmp11F=_tmp11B.tq;goto _LL286;_LL286: _tmp11D=
_tmp11B.bounds;goto _LL223;}else{goto _LL224;}_LL224: if((unsigned int)_tmpCC > 3?*((
int*)_tmpCC)== Cyc_Absyn_IntType: 0){goto _LL225;}else{goto _LL226;}_LL226: if(
_tmpCC == (void*)Cyc_Absyn_FloatType){goto _LL227;}else{goto _LL228;}_LL228: if((
unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_DoubleType: 0){goto _LL229;}
else{goto _LL230;}_LL230: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_ArrayType:
0){_LL296: _tmp127=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpCC)->f1;goto
_LL294;_LL294: _tmp125=((struct Cyc_Absyn_ArrayType_struct*)_tmpCC)->f2;goto _LL292;
_LL292: _tmp123=((struct Cyc_Absyn_ArrayType_struct*)_tmpCC)->f3;goto _LL231;}else{
goto _LL232;}_LL232: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_FnType:
0){_LL298: _tmp129=((struct Cyc_Absyn_FnType_struct*)_tmpCC)->f1;_LL308: _tmp133=(
void*)_tmp129.ret_typ;goto _LL306;_LL306: _tmp131=_tmp129.args;goto _LL304;_LL304:
_tmp12F=_tmp129.c_varargs;goto _LL302;_LL302: _tmp12D=_tmp129.cyc_varargs;goto
_LL300;_LL300: _tmp12B=_tmp129.attributes;goto _LL233;}else{goto _LL234;}_LL234: if((
unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_TupleType: 0){_LL310: _tmp135=((
struct Cyc_Absyn_TupleType_struct*)_tmpCC)->f1;goto _LL235;}else{goto _LL236;}
_LL236: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_AnonStructType: 0){
_LL312: _tmp137=((struct Cyc_Absyn_AnonStructType_struct*)_tmpCC)->f1;goto _LL237;}
else{goto _LL238;}_LL238: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_AnonUnionType:
0){_LL314: _tmp139=((struct Cyc_Absyn_AnonUnionType_struct*)_tmpCC)->f1;goto _LL239;}
else{goto _LL240;}_LL240: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_UnionType:
0){_LL316: _tmp13B=((struct Cyc_Absyn_UnionType_struct*)_tmpCC)->f1;goto _LL241;}
else{goto _LL242;}_LL242: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_StructType:
0){_LL318: _tmp13D=((struct Cyc_Absyn_StructType_struct*)_tmpCC)->f1;goto _LL243;}
else{goto _LL244;}_LL244: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_EnumType:
0){_LL320: _tmp13F=((struct Cyc_Absyn_EnumType_struct*)_tmpCC)->f1;goto _LL245;}
else{goto _LL246;}_LL246: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_AnonEnumType:
0){_LL322: _tmp141=((struct Cyc_Absyn_AnonEnumType_struct*)_tmpCC)->f1;goto _LL247;}
else{goto _LL248;}_LL248: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_TypedefType:
0){_LL328: _tmp147=((struct Cyc_Absyn_TypedefType_struct*)_tmpCC)->f1;goto _LL326;
_LL326: _tmp145=((struct Cyc_Absyn_TypedefType_struct*)_tmpCC)->f2;goto _LL324;
_LL324: _tmp143=((struct Cyc_Absyn_TypedefType_struct*)_tmpCC)->f3;goto _LL249;}
else{goto _LL250;}_LL250: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_SizeofType:
0){goto _LL251;}else{goto _LL252;}_LL252: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)
== Cyc_Absyn_RgnHandleType: 0){_LL330: _tmp149=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmpCC)->f1;goto _LL253;}else{goto _LL254;}_LL254: if(_tmpCC == (void*)Cyc_Absyn_HeapRgn){
goto _LL255;}else{goto _LL256;}_LL256: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== 
Cyc_Absyn_AccessEff: 0){goto _LL257;}else{goto _LL258;}_LL258: if((unsigned int)
_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_JoinEff: 0){goto _LL259;}else{goto _LL260;}
_LL260: if((unsigned int)_tmpCC > 3?*((int*)_tmpCC)== Cyc_Absyn_RgnsEff: 0){goto
_LL261;}else{goto _LL205;}_LL207: return t;_LL209: return Cyc_Absyn_sint_t;_LL211:
return Cyc_Toc_typ_to_c(_tmp10B);_LL213: if(Cyc_Tcutil_tvar_kind(_tmp10D)== (void*)
Cyc_Absyn_AnyKind){return(void*)Cyc_Absyn_VoidType;}else{return Cyc_Absyn_void_star_typ();}
_LL215: return Cyc_Absyn_void_star_typ();_LL217:(int)_throw(({void*_tmp14B[0]={};
Cyc_Tcutil_impos(_tag_arr("unresolved TunionType",sizeof(unsigned char),22),
_tag_arr(_tmp14B,sizeof(void*),0));}));_LL219: if(_tmp117->typs == 0){if(_tmp119->is_xtunion){
return Cyc_Absyn_star_typ(Cyc_Absyn_uchar_t,(void*)Cyc_Absyn_HeapRgn,Cyc_Toc_mt_tq);}
else{return Cyc_Absyn_uint_t;}}else{return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp117->name,_tag_arr("_struct",sizeof(unsigned char),8)));}_LL221:(int)_throw(({
void*_tmp14C[0]={};Cyc_Tcutil_impos(_tag_arr("unresolved TunionFieldType",
sizeof(unsigned char),27),_tag_arr(_tmp14C,sizeof(void*),0));}));_LL223: _tmp121=
Cyc_Toc_typ_to_c_array(_tmp121);{void*_tmp14D=(void*)(Cyc_Absyn_compress_conref(
_tmp11D))->v;void*_tmp153;_LL335: if((unsigned int)_tmp14D > 1?*((int*)_tmp14D)== 
Cyc_Absyn_Eq_constr: 0){_LL340: _tmp153=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp14D)->f1;if(_tmp153 == (void*)Cyc_Absyn_Unknown_b){goto _LL336;}else{goto
_LL337;}}else{goto _LL337;}_LL337: goto _LL338;_LL336: return Cyc_Toc_tagged_arr_typ;
_LL338: return Cyc_Absyn_star_typ(_tmp121,(void*)Cyc_Absyn_HeapRgn,_tmp11F);_LL334:;}
_LL225: return t;_LL227: return t;_LL229: return t;_LL231: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp155=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp155[0]=({struct
Cyc_Absyn_ArrayType_struct _tmp156;_tmp156.tag=Cyc_Absyn_ArrayType;_tmp156.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp127);_tmp156.f2=_tmp125;_tmp156.f3=_tmp123;
_tmp156;});_tmp155;});_LL233: {struct Cyc_List_List*_tmp157=0;for(0;_tmp12B != 0;
_tmp12B=_tmp12B->tl){void*_tmp158=(void*)_tmp12B->hd;_LL346: if(_tmp158 == (void*)
Cyc_Absyn_Noreturn_att){goto _LL347;}else{goto _LL348;}_LL348: if(_tmp158 == (void*)
Cyc_Absyn_Const_att){goto _LL349;}else{goto _LL350;}_LL350: if((unsigned int)
_tmp158 > 16?*((int*)_tmp158)== Cyc_Absyn_Format_att: 0){goto _LL351;}else{goto
_LL352;}_LL352: goto _LL353;_LL347: goto _LL349;_LL349: goto _LL351;_LL351: continue;
_LL353: _tmp157=({struct Cyc_List_List*_tmp162=_cycalloc(sizeof(struct Cyc_List_List));
_tmp162->hd=(void*)((void*)_tmp12B->hd);_tmp162->tl=_tmp157;_tmp162;});goto
_LL345;_LL345:;}{struct Cyc_List_List*_tmp163=((struct Cyc_List_List*(*)(struct
_tuple1*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmp131);if(_tmp12D != 0){void*_tmp164=Cyc_Toc_typ_to_c(Cyc_Absyn_tagged_typ((
void*)_tmp12D->type,(void*)Cyc_Absyn_HeapRgn,Cyc_Toc_mt_tq));struct _tuple1*
_tmp165=({struct _tuple1*_tmp167=_cycalloc(sizeof(struct _tuple1));_tmp167->f1=
_tmp12D->name;_tmp167->f2=_tmp12D->tq;_tmp167->f3=_tmp164;_tmp167;});_tmp163=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp163,({struct Cyc_List_List*_tmp166=_cycalloc(sizeof(struct Cyc_List_List));
_tmp166->hd=_tmp165;_tmp166->tl=0;_tmp166;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp168=_cycalloc(sizeof(struct Cyc_Absyn_FnType_struct));_tmp168[0]=({struct Cyc_Absyn_FnType_struct
_tmp169;_tmp169.tag=Cyc_Absyn_FnType;_tmp169.f1=({struct Cyc_Absyn_FnInfo _tmp16A;
_tmp16A.tvars=0;_tmp16A.effect=0;_tmp16A.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmp133);
_tmp16A.args=_tmp163;_tmp16A.c_varargs=_tmp12F;_tmp16A.cyc_varargs=0;_tmp16A.rgn_po=
0;_tmp16A.attributes=_tmp157;_tmp16A;});_tmp169;});_tmp168;});}}_LL235: _tmp135=((
struct Cyc_List_List*(*)(struct _tuple3*(*f)(struct _tuple3*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp135);{struct _tagged_arr*_tmp16B=Cyc_Toc_add_tuple_type(
_tmp135);return Cyc_Absyn_strct(_tmp16B);}_LL237: return(void*)({struct Cyc_Absyn_AnonStructType_struct*
_tmp16C=_cycalloc(sizeof(struct Cyc_Absyn_AnonStructType_struct));_tmp16C[0]=({
struct Cyc_Absyn_AnonStructType_struct _tmp16D;_tmp16D.tag=Cyc_Absyn_AnonStructType;
_tmp16D.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*f)(struct Cyc_Absyn_Structfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_structfield_to_c,_tmp137);_tmp16D;});
_tmp16C;});_LL239: return(void*)({struct Cyc_Absyn_AnonUnionType_struct*_tmp16E=
_cycalloc(sizeof(struct Cyc_Absyn_AnonUnionType_struct));_tmp16E[0]=({struct Cyc_Absyn_AnonUnionType_struct
_tmp16F;_tmp16F.tag=Cyc_Absyn_AnonUnionType;_tmp16F.f1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(*f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_structfield_to_c,_tmp139);_tmp16F;});_tmp16E;});_LL241:
if(_tmp13B == 0){({void*_tmp170[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("anonymous union",sizeof(unsigned char),
16),_tag_arr(_tmp170,sizeof(void*),0));});}return Cyc_Absyn_unionq_typ((struct
_tuple0*)_check_null(_tmp13B));_LL243: if(_tmp13D == 0){({void*_tmp171[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("anonymous struct",
sizeof(unsigned char),17),_tag_arr(_tmp171,sizeof(void*),0));});}return Cyc_Absyn_strctq((
struct _tuple0*)_check_null(_tmp13D));_LL245: return t;_LL247: Cyc_Toc_enumfields_to_c(
_tmp141);return t;_LL249: if(_tmp143 == 0){return({void*_tmp172[0]={};Cyc_Toc_toc_impos(
_tag_arr("Toc::typ_to_c: unresolved TypedefType",sizeof(unsigned char),38),
_tag_arr(_tmp172,sizeof(void*),0));});}return(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp173=_cycalloc(sizeof(struct Cyc_Absyn_TypedefType_struct));_tmp173[0]=({
struct Cyc_Absyn_TypedefType_struct _tmp174;_tmp174.tag=Cyc_Absyn_TypedefType;
_tmp174.f1=_tmp147;_tmp174.f2=0;_tmp174.f3=({struct Cyc_Core_Opt*_tmp175=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp175->v=(void*)Cyc_Toc_typ_to_c_array((
void*)_tmp143->v);_tmp175;});_tmp174;});_tmp173;});_LL251: return Cyc_Absyn_uint_t;
_LL253: return Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);
_LL255: return({void*_tmp176[0]={};Cyc_Toc_toc_impos(_tag_arr("Toc::typ_to_c: type translation passed the heap region",
sizeof(unsigned char),55),_tag_arr(_tmp176,sizeof(void*),0));});_LL257: return({
void*_tmp177[0]={};Cyc_Toc_toc_impos(_tag_arr("Toc::typ_to_c: type translation passed the access eff",
sizeof(unsigned char),54),_tag_arr(_tmp177,sizeof(void*),0));});_LL259: return({
void*_tmp178[0]={};Cyc_Toc_toc_impos(_tag_arr("Toc::typ_to_c: type translation passed the join eff",
sizeof(unsigned char),52),_tag_arr(_tmp178,sizeof(void*),0));});_LL261: return({
void*_tmp179[0]={};Cyc_Toc_toc_impos(_tag_arr("Toc::typ_to_c: type translation passed the regions eff",
sizeof(unsigned char),55),_tag_arr(_tmp179,sizeof(void*),0));});_LL205:;}static
struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*l){void*_tmp17A=t;struct Cyc_Absyn_Tqual _tmp180;void*
_tmp182;_LL380: if((unsigned int)_tmp17A > 3?*((int*)_tmp17A)== Cyc_Absyn_ArrayType:
0){_LL387: _tmp182=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp17A)->f1;goto
_LL385;_LL385: _tmp180=((struct Cyc_Absyn_ArrayType_struct*)_tmp17A)->f2;goto
_LL381;}else{goto _LL382;}_LL382: goto _LL383;_LL381: return Cyc_Absyn_cast_exp(Cyc_Absyn_star_typ(
_tmp182,(void*)Cyc_Absyn_HeapRgn,_tmp180),e,l);_LL383: return Cyc_Absyn_cast_exp(t,
e,l);_LL379:;}static int Cyc_Toc_atomic_typ(void*t){void*_tmp184=Cyc_Tcutil_compress(
t);void*_tmp1AE;struct Cyc_Absyn_Structdecl**_tmp1B0;struct Cyc_List_List*_tmp1B2;
struct Cyc_List_List*_tmp1B4;struct Cyc_Absyn_TunionFieldInfo _tmp1B6;void*_tmp1B8;
struct Cyc_Absyn_Tunionfield*_tmp1BA;struct Cyc_Absyn_Tuniondecl*_tmp1BC;struct Cyc_List_List*
_tmp1BE;_LL390: if(_tmp184 == (void*)Cyc_Absyn_VoidType){goto _LL391;}else{goto
_LL392;}_LL392: if((unsigned int)_tmp184 > 3?*((int*)_tmp184)== Cyc_Absyn_VarType:
0){goto _LL393;}else{goto _LL394;}_LL394: if((unsigned int)_tmp184 > 3?*((int*)
_tmp184)== Cyc_Absyn_IntType: 0){goto _LL395;}else{goto _LL396;}_LL396: if((
unsigned int)_tmp184 > 3?*((int*)_tmp184)== Cyc_Absyn_EnumType: 0){goto _LL397;}
else{goto _LL398;}_LL398: if((unsigned int)_tmp184 > 3?*((int*)_tmp184)== Cyc_Absyn_AnonEnumType:
0){goto _LL399;}else{goto _LL400;}_LL400: if(_tmp184 == (void*)Cyc_Absyn_FloatType){
goto _LL401;}else{goto _LL402;}_LL402: if((unsigned int)_tmp184 > 3?*((int*)_tmp184)
== Cyc_Absyn_DoubleType: 0){goto _LL403;}else{goto _LL404;}_LL404: if((unsigned int)
_tmp184 > 3?*((int*)_tmp184)== Cyc_Absyn_FnType: 0){goto _LL405;}else{goto _LL406;}
_LL406: if((unsigned int)_tmp184 > 3?*((int*)_tmp184)== Cyc_Absyn_SizeofType: 0){
goto _LL407;}else{goto _LL408;}_LL408: if((unsigned int)_tmp184 > 3?*((int*)_tmp184)
== Cyc_Absyn_ArrayType: 0){_LL431: _tmp1AE=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp184)->f1;goto _LL409;}else{goto _LL410;}_LL410: if((unsigned int)_tmp184 > 3?*((
int*)_tmp184)== Cyc_Absyn_StructType: 0){_LL433: _tmp1B0=((struct Cyc_Absyn_StructType_struct*)
_tmp184)->f3;goto _LL411;}else{goto _LL412;}_LL412: if((unsigned int)_tmp184 > 3?*((
int*)_tmp184)== Cyc_Absyn_AnonStructType: 0){_LL435: _tmp1B2=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp184)->f1;goto _LL413;}else{goto _LL414;}_LL414: if((unsigned int)_tmp184 > 3?*((
int*)_tmp184)== Cyc_Absyn_AnonUnionType: 0){_LL437: _tmp1B4=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp184)->f1;goto _LL415;}else{goto _LL416;}_LL416: if((unsigned int)_tmp184 > 3?*((
int*)_tmp184)== Cyc_Absyn_TunionFieldType: 0){_LL439: _tmp1B6=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp184)->f1;_LL441: _tmp1B8=(void*)_tmp1B6.field_info;if(*((int*)_tmp1B8)== Cyc_Absyn_KnownTunionfield){
_LL445: _tmp1BC=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp1B8)->f1;goto
_LL443;_LL443: _tmp1BA=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp1B8)->f2;
goto _LL417;}else{goto _LL418;}}else{goto _LL418;}_LL418: if((unsigned int)_tmp184 > 
3?*((int*)_tmp184)== Cyc_Absyn_TupleType: 0){_LL447: _tmp1BE=((struct Cyc_Absyn_TupleType_struct*)
_tmp184)->f1;goto _LL419;}else{goto _LL420;}_LL420: if((unsigned int)_tmp184 > 3?*((
int*)_tmp184)== Cyc_Absyn_TunionType: 0){goto _LL421;}else{goto _LL422;}_LL422: if((
unsigned int)_tmp184 > 3?*((int*)_tmp184)== Cyc_Absyn_UnionType: 0){goto _LL423;}
else{goto _LL424;}_LL424: if((unsigned int)_tmp184 > 3?*((int*)_tmp184)== Cyc_Absyn_PointerType:
0){goto _LL425;}else{goto _LL426;}_LL426: if((unsigned int)_tmp184 > 3?*((int*)
_tmp184)== Cyc_Absyn_RgnHandleType: 0){goto _LL427;}else{goto _LL428;}_LL428: goto
_LL429;_LL391: return 1;_LL393: return 0;_LL395: return 1;_LL397: return 1;_LL399: return
1;_LL401: return 1;_LL403: return 1;_LL405: return 1;_LL407: return 1;_LL409: return Cyc_Toc_atomic_typ(
_tmp1AE);_LL411: if(_tmp1B0 == 0? 1:(*_tmp1B0)->fields == 0){return 0;}{struct Cyc_List_List*
_tmp1C0=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null((*_tmp1B0)->fields))->v;
for(0;_tmp1C0 != 0;_tmp1C0=_tmp1C0->tl){if(! Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Structfield*)
_tmp1C0->hd)->type)){return 0;}}}return 1;_LL413: _tmp1B4=_tmp1B2;goto _LL415;_LL415:
for(0;_tmp1B4 != 0;_tmp1B4=_tmp1B4->tl){if(! Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Structfield*)
_tmp1B4->hd)->type)){return 0;}}return 1;_LL417: _tmp1BE=_tmp1BA->typs;goto _LL419;
_LL419: for(0;_tmp1BE != 0;_tmp1BE=_tmp1BE->tl){if(! Cyc_Toc_atomic_typ((*((struct
_tuple3*)_tmp1BE->hd)).f2)){return 0;}}return 1;_LL421: return 0;_LL423: return 0;
_LL425: return 0;_LL427: return 0;_LL429:({struct Cyc_Std_String_pa_struct _tmp1C2;
_tmp1C2.tag=Cyc_Std_String_pa;_tmp1C2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{void*_tmp1C1[1]={& _tmp1C2};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("atomic_typ:  bad type %s",sizeof(unsigned char),25),
_tag_arr(_tmp1C1,sizeof(void*),1));}});return 0;_LL389:;}static int Cyc_Toc_is_void_star(
void*t){void*_tmp1C3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1C9;void*
_tmp1CB;_LL453: if((unsigned int)_tmp1C3 > 3?*((int*)_tmp1C3)== Cyc_Absyn_PointerType:
0){_LL458: _tmp1C9=((struct Cyc_Absyn_PointerType_struct*)_tmp1C3)->f1;_LL460:
_tmp1CB=(void*)_tmp1C9.elt_typ;goto _LL454;}else{goto _LL455;}_LL455: goto _LL456;
_LL454: {void*_tmp1CD=Cyc_Tcutil_compress(_tmp1CB);_LL463: if(_tmp1CD == (void*)
Cyc_Absyn_VoidType){goto _LL464;}else{goto _LL465;}_LL465: goto _LL466;_LL464: return
1;_LL466: return 0;_LL462:;}_LL456: return 0;_LL452:;}static int Cyc_Toc_is_poly_field(
void*t,struct _tagged_arr*f){void*_tmp1D3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Structdecl**
_tmp1DF;struct Cyc_Absyn_Structdecl*_tmp1E1;struct Cyc_List_List*_tmp1E2;_LL469:
if((unsigned int)_tmp1D3 > 3?*((int*)_tmp1D3)== Cyc_Absyn_StructType: 0){_LL480:
_tmp1DF=((struct Cyc_Absyn_StructType_struct*)_tmp1D3)->f3;if(_tmp1DF == 0){goto
_LL471;}else{_tmp1E1=*_tmp1DF;goto _LL470;}}else{goto _LL471;}_LL471: if((
unsigned int)_tmp1D3 > 3?*((int*)_tmp1D3)== Cyc_Absyn_AnonStructType: 0){_LL483:
_tmp1E2=((struct Cyc_Absyn_AnonStructType_struct*)_tmp1D3)->f1;goto _LL472;}else{
goto _LL473;}_LL473: if((unsigned int)_tmp1D3 > 3?*((int*)_tmp1D3)== Cyc_Absyn_AnonUnionType:
0){goto _LL474;}else{goto _LL475;}_LL475: if((unsigned int)_tmp1D3 > 3?*((int*)
_tmp1D3)== Cyc_Absyn_UnionType: 0){goto _LL476;}else{goto _LL477;}_LL477: goto _LL478;
_LL470: if(_tmp1E1->fields == 0){(int)_throw(({void*_tmp1E4[0]={};Cyc_Toc_toc_impos(
_tag_arr("is_poly_field: struct missing fields",sizeof(unsigned char),37),
_tag_arr(_tmp1E4,sizeof(void*),0));}));}_tmp1E2=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp1E1->fields))->v;goto _LL472;_LL472: {struct Cyc_Absyn_Structfield*
_tmp1E5=Cyc_Absyn_lookup_field(_tmp1E2,f);if(_tmp1E5 == 0){({struct Cyc_Std_String_pa_struct
_tmp1E7;_tmp1E7.tag=Cyc_Std_String_pa;_tmp1E7.f1=(struct _tagged_arr)*f;{void*
_tmp1E6[1]={& _tmp1E7};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("is_poly_field: bad field %s",sizeof(unsigned char),28),_tag_arr(
_tmp1E6,sizeof(void*),1));}});}return Cyc_Toc_is_void_star((void*)((struct Cyc_Absyn_Structfield*)
_check_null(_tmp1E5))->type);}_LL474: return 0;_LL476: return 0;_LL478:(int)_throw(({
struct Cyc_Std_String_pa_struct _tmp1E9;_tmp1E9.tag=Cyc_Std_String_pa;_tmp1E9.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1E8[1]={& _tmp1E9};Cyc_Toc_toc_impos(
_tag_arr("is_poly_field: bad type %s",sizeof(unsigned char),27),_tag_arr(_tmp1E8,
sizeof(void*),1));}}));_LL468:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*
e){void*_tmp1EA=(void*)e->r;struct _tagged_arr*_tmp1F2;struct Cyc_Absyn_Exp*
_tmp1F4;struct _tagged_arr*_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F8;_LL492: if(*((int*)
_tmp1EA)== Cyc_Absyn_StructMember_e){_LL501: _tmp1F4=((struct Cyc_Absyn_StructMember_e_struct*)
_tmp1EA)->f1;goto _LL499;_LL499: _tmp1F2=((struct Cyc_Absyn_StructMember_e_struct*)
_tmp1EA)->f2;goto _LL493;}else{goto _LL494;}_LL494: if(*((int*)_tmp1EA)== Cyc_Absyn_StructArrow_e){
_LL505: _tmp1F8=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp1EA)->f1;goto _LL503;
_LL503: _tmp1F6=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp1EA)->f2;goto _LL495;}
else{goto _LL496;}_LL496: goto _LL497;_LL493: return Cyc_Toc_is_poly_field((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1F4->topt))->v,_tmp1F2);_LL495: {void*
_tmp1FA=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp1F8->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp200;void*_tmp202;_LL508: if((unsigned int)_tmp1FA > 3?*((
int*)_tmp1FA)== Cyc_Absyn_PointerType: 0){_LL513: _tmp200=((struct Cyc_Absyn_PointerType_struct*)
_tmp1FA)->f1;_LL515: _tmp202=(void*)_tmp200.elt_typ;goto _LL509;}else{goto _LL510;}
_LL510: goto _LL511;_LL509: return Cyc_Toc_is_poly_field(_tmp202,_tmp1F6);_LL511:({
struct Cyc_Std_String_pa_struct _tmp205;_tmp205.tag=Cyc_Std_String_pa;_tmp205.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp1F8->topt))->v);{void*_tmp204[1]={& _tmp205};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("is_poly_project:  bad type %s",
sizeof(unsigned char),30),_tag_arr(_tmp204,sizeof(void*),1));}});return 0;_LL507:;}
_LL497: return 0;_LL491:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*
s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp206=
_cycalloc(sizeof(struct Cyc_List_List));_tmp206->hd=s;_tmp206->tl=0;_tmp206;}),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp207=_cycalloc(sizeof(struct
Cyc_List_List));_tmp207->hd=s;_tmp207->tl=0;_tmp207;}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t)){
return Cyc_Toc_malloc_atomic(s);}return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*
Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp208[2];_tmp208[1]=s;_tmp208[0]=
rgn;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp208,
sizeof(struct Cyc_Absyn_Exp*),2));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(
void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(
elt_type)){return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({struct Cyc_Absyn_Exp*
_tmp209[2];_tmp209[1]=n;_tmp209[0]=s;((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp209,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}else{
return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*_tmp20A[2];
_tmp20A[1]=n;_tmp20A[0]=s;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp20A,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*
n){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({struct Cyc_Absyn_Exp*
_tmp20B[3];_tmp20B[2]=n;_tmp20B[1]=s;_tmp20B[0]=rgn;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp20B,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*_tmp20C=
_cycalloc(sizeof(struct Cyc_List_List));_tmp20C->hd=e;_tmp20C->tl=0;_tmp20C;}),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_tagged_arr(void*t,struct Cyc_Absyn_Exp*
sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*_tmp20D=(void*)e->r;void*_tmp213;
_LL527: if(*((int*)_tmp20D)== Cyc_Absyn_Const_e){_LL532: _tmp213=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmp20D)->f1;if((unsigned int)_tmp213 > 1?*((int*)
_tmp213)== Cyc_Absyn_String_c: 0){goto _LL528;}else{goto _LL529;}}else{goto _LL529;}
_LL529: goto _LL530;_LL528: is_string=1;goto _LL526;_LL530: goto _LL526;_LL526:;}{
struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple0*
x=Cyc_Toc_temp_var();void*vd_typ=(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp218=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp218[0]=({struct
Cyc_Absyn_ArrayType_struct _tmp219;_tmp219.tag=Cyc_Absyn_ArrayType;_tmp219.f1=(
void*)Cyc_Absyn_uchar_t;_tmp219.f2=Cyc_Toc_mt_tq;_tmp219.f3=(struct Cyc_Absyn_Exp*)
sz;_tmp219;});_tmp218;});struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,
vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp215=_cycalloc(sizeof(struct Cyc_List_List));_tmp215->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp216=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));
_tmp216[0]=({struct Cyc_Absyn_Var_d_struct _tmp217;_tmp217.tag=Cyc_Absyn_Var_d;
_tmp217.f1=vd;_tmp217;});_tmp216;}),0);_tmp215->tl=Cyc_Toc_result_decls;_tmp215;});
xexp=Cyc_Absyn_var_exp(x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Absyn_cast_exp(
Cyc_Absyn_void_star_typ(),e,0);xplussz=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_add_exp(e,sz,0),0);}{struct Cyc_Absyn_Exp*urm_exp=Cyc_Absyn_unresolvedmem_exp(
0,({struct _tuple4*_tmp21A[3];_tmp21A[2]=({struct _tuple4*_tmp21D=_cycalloc(
sizeof(struct _tuple4));_tmp21D->f1=0;_tmp21D->f2=xplussz;_tmp21D;});_tmp21A[1]=({
struct _tuple4*_tmp21C=_cycalloc(sizeof(struct _tuple4));_tmp21C->f1=0;_tmp21C->f2=
xexp;_tmp21C;});_tmp21A[0]=({struct _tuple4*_tmp21B=_cycalloc(sizeof(struct
_tuple4));_tmp21B->f1=0;_tmp21B->f2=xexp;_tmp21B;});((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp21A,sizeof(struct _tuple4*),3));}),
0);return urm_exp;}}}struct Cyc_Toc_Env{struct Cyc_Core_Opt*break_lab;struct Cyc_Core_Opt*
continue_lab;struct Cyc_Core_Opt*fallthru_info;struct Cyc_Dict_Dict*varmap;int
toplevel;};static struct Cyc_Toc_Env*Cyc_Toc_empty_env(){return({struct Cyc_Toc_Env*
_tmp21E=_cycalloc(sizeof(struct Cyc_Toc_Env));_tmp21E->break_lab=0;_tmp21E->continue_lab=
0;_tmp21E->fallthru_info=0;_tmp21E->varmap=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp21E->toplevel=
1;_tmp21E;});}static struct Cyc_Toc_Env*Cyc_Toc_copy_env(struct Cyc_Toc_Env*e){
return({struct Cyc_Toc_Env*_tmp21F=_cycalloc(sizeof(struct Cyc_Toc_Env));_tmp21F->break_lab=
e->break_lab;_tmp21F->continue_lab=e->continue_lab;_tmp21F->fallthru_info=e->fallthru_info;
_tmp21F->varmap=e->varmap;_tmp21F->toplevel=e->toplevel;_tmp21F;});}static struct
Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=
Cyc_Toc_copy_env(e);ans->toplevel=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct Cyc_Toc_Env*e,struct _tuple0*x,struct Cyc_Absyn_Exp*y){{void*_tmp220=(*x).f1;
_LL546: if((unsigned int)_tmp220 > 1?*((int*)_tmp220)== Cyc_Absyn_Rel_n: 0){goto
_LL547;}else{goto _LL548;}_LL548: goto _LL549;_LL547:(int)_throw(({struct Cyc_Std_String_pa_struct
_tmp227;_tmp227.tag=Cyc_Std_String_pa;_tmp227.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
x);{void*_tmp226[1]={& _tmp227};Cyc_Toc_toc_impos(_tag_arr("Toc::add_varmap on Rel_n: %s\n",
sizeof(unsigned char),30),_tag_arr(_tmp226,sizeof(void*),1));}}));_LL549: goto
_LL545;_LL545:;}{struct Cyc_Toc_Env*ans=Cyc_Toc_copy_env(e);ans->varmap=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,struct Cyc_Absyn_Exp*v))
Cyc_Dict_insert)(ans->varmap,x,y);return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(
struct Cyc_Toc_Env*e){struct Cyc_Toc_Env*ans=Cyc_Toc_copy_env(e);ans->break_lab=0;
ans->continue_lab=0;return ans;}struct _tuple6{struct _tagged_arr*f1;struct Cyc_List_List*
f2;struct Cyc_Dict_Dict*f3;};static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*break_l,struct _tagged_arr*fallthru_l,
struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){struct Cyc_List_List*
_tmp228=0;for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
_tmp228=({struct Cyc_List_List*_tmp229=_cycalloc(sizeof(struct Cyc_List_List));
_tmp229->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name;_tmp229->tl=
_tmp228;_tmp229;});}_tmp228=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp228);{struct Cyc_Toc_Env*ans=Cyc_Toc_copy_env(e);ans->break_lab=({struct Cyc_Core_Opt*
_tmp22A=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp22A->v=break_l;_tmp22A;});ans->fallthru_info=({
struct Cyc_Core_Opt*_tmp22B=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp22B->v=({
struct _tuple6*_tmp22C=_cycalloc(sizeof(struct _tuple6));_tmp22C->f1=fallthru_l;
_tmp22C->f2=_tmp228;_tmp22C->f3=next_case_env->varmap;_tmp22C;});_tmp22B;});
return ans;}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct Cyc_Toc_Env*
e,struct _tagged_arr*break_l){struct Cyc_Toc_Env*ans=Cyc_Toc_copy_env(e);ans->break_lab=({
struct Cyc_Core_Opt*_tmp22D=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp22D->v=
break_l;_tmp22D;});ans->fallthru_info=0;return ans;}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct Cyc_Toc_Env*e,struct _tagged_arr*next_l){struct Cyc_Toc_Env*ans=Cyc_Toc_copy_env(
e);ans->break_lab=0;ans->fallthru_info=({struct Cyc_Core_Opt*_tmp22E=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp22E->v=({struct _tuple6*_tmp22F=_cycalloc(sizeof(
struct _tuple6));_tmp22F->f1=next_l;_tmp22F->f2=0;_tmp22F->f3=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp22F;});_tmp22E;});return ans;}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*
_tmp230=(void*)e->annot;_LL562: if(*((void**)_tmp230)== Cyc_CfFlowInfo_UnknownZ){
goto _LL563;}else{goto _LL564;}_LL564: if(*((void**)_tmp230)== Cyc_CfFlowInfo_NotZero){
goto _LL565;}else{goto _LL566;}_LL566: if(*((void**)_tmp230)== Cyc_CfFlowInfo_IsZero){
goto _LL567;}else{goto _LL568;}_LL568: if(_tmp230 == Cyc_Absyn_EmptyAnnot){goto
_LL569;}else{goto _LL570;}_LL570: goto _LL571;_LL563: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL565: return 0;_LL567:({
void*_tmp23C[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of NULL pointer",
sizeof(unsigned char),28),_tag_arr(_tmp23C,sizeof(void*),0));});return 0;_LL569:({
void*_tmp23D[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("!",sizeof(
unsigned char),2),_tag_arr(_tmp23D,sizeof(void*),0));});({void*_tmp23E[0]={};Cyc_Tcutil_warn(
e->loc,_tag_arr("unreachable dereference",sizeof(unsigned char),24),_tag_arr(
_tmp23E,sizeof(void*),0));});return 0;_LL571:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp23F=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp23F[0]=({struct
Cyc_Core_Impossible_struct _tmp240;_tmp240.tag=Cyc_Core_Impossible;_tmp240.f1=
_tag_arr("need_null_check",sizeof(unsigned char),16);_tmp240;});_tmp23F;}));
_LL561:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){void*
_tmp241=(void*)e->annot;struct Cyc_List_List*_tmp24D;struct Cyc_List_List*_tmp24F;
_LL579: if(*((void**)_tmp241)== Cyc_CfFlowInfo_UnknownZ){_LL590: _tmp24D=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmp241)->f1;goto _LL580;}else{goto _LL581;}_LL581:
if(*((void**)_tmp241)== Cyc_CfFlowInfo_NotZero){_LL592: _tmp24F=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp241)->f1;goto _LL582;}else{goto _LL583;}_LL583: if(*((void**)_tmp241)== Cyc_CfFlowInfo_IsZero){
goto _LL584;}else{goto _LL585;}_LL585: if(_tmp241 == Cyc_Absyn_EmptyAnnot){goto
_LL586;}else{goto _LL587;}_LL587: goto _LL588;_LL580: return _tmp24D;_LL582: return
_tmp24F;_LL584:({void*_tmp251[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("dereference of NULL pointer",
sizeof(unsigned char),28),_tag_arr(_tmp251,sizeof(void*),0));});return 0;_LL586:
return 0;_LL588:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp252=
_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp252[0]=({struct Cyc_Core_Impossible_struct
_tmp253;_tmp253.tag=Cyc_Core_Impossible;_tmp253.f1=_tag_arr("get_relns",sizeof(
unsigned char),10);_tmp253;});_tmp252;}));_LL578:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t){void*_tmp254=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp25C;struct Cyc_Absyn_Conref*_tmp25E;struct Cyc_Absyn_Exp*_tmp260;_LL598: if((
unsigned int)_tmp254 > 3?*((int*)_tmp254)== Cyc_Absyn_PointerType: 0){_LL605:
_tmp25C=((struct Cyc_Absyn_PointerType_struct*)_tmp254)->f1;_LL607: _tmp25E=
_tmp25C.bounds;goto _LL599;}else{goto _LL600;}_LL600: if((unsigned int)_tmp254 > 3?*((
int*)_tmp254)== Cyc_Absyn_ArrayType: 0){_LL609: _tmp260=((struct Cyc_Absyn_ArrayType_struct*)
_tmp254)->f3;goto _LL601;}else{goto _LL602;}_LL602: goto _LL603;_LL599: {void*
_tmp262=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp25E);struct Cyc_Absyn_Exp*
_tmp268;_LL612: if(_tmp262 == (void*)Cyc_Absyn_Unknown_b){goto _LL613;}else{goto
_LL614;}_LL614: if((unsigned int)_tmp262 > 1?*((int*)_tmp262)== Cyc_Absyn_Upper_b:
0){_LL617: _tmp268=((struct Cyc_Absyn_Upper_b_struct*)_tmp262)->f1;goto _LL615;}
else{goto _LL611;}_LL613: return 0;_LL615: {unsigned int j=Cyc_Evexp_eval_const_uint_exp(
_tmp268);return i <= j;}_LL611:;}_LL601: if(_tmp260 == 0){return 0;}{unsigned int j=
Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp260));return
i <= j;}_LL603: return 0;_LL597:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*
relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){for(0;relns != 0;relns=
relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp26A=(struct Cyc_CfFlowInfo_Reln*)relns->hd;
if(_tmp26A->vd != y){continue;}{void*_tmp26B=(void*)_tmp26A->rop;struct Cyc_Absyn_Vardecl*
_tmp273;struct Cyc_Absyn_Vardecl*_tmp275;_LL621: if(*((int*)_tmp26B)== Cyc_CfFlowInfo_LessSize){
_LL628: _tmp273=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp26B)->f1;goto _LL622;}
else{goto _LL623;}_LL623: if(*((int*)_tmp26B)== Cyc_CfFlowInfo_LessEqSize){_LL630:
_tmp275=((struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp26B)->f1;goto _LL624;}else{
goto _LL625;}_LL625: goto _LL626;_LL622: _tmp275=_tmp273;goto _LL624;_LL624: if(
_tmp275 == v){return 1;}else{goto _LL620;}_LL626: continue;_LL620:;}}return 0;}static
int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,
struct Cyc_Absyn_Exp*e){{void*_tmp277=(void*)e->r;void*_tmp285;struct Cyc_Absyn_Vardecl*
_tmp287;void*_tmp289;struct Cyc_Absyn_Vardecl*_tmp28B;void*_tmp28D;struct Cyc_Absyn_Vardecl*
_tmp28F;void*_tmp291;struct Cyc_Absyn_Vardecl*_tmp293;struct Cyc_List_List*_tmp295;
struct Cyc_List_List _tmp297;struct Cyc_Absyn_Exp*_tmp298;void*_tmp29A;_LL633: if(*((
int*)_tmp277)== Cyc_Absyn_Var_e){_LL646: _tmp285=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp277)->f2;if((unsigned int)_tmp285 > 1?*((int*)_tmp285)== Cyc_Absyn_Pat_b: 0){
_LL648: _tmp287=((struct Cyc_Absyn_Pat_b_struct*)_tmp285)->f1;goto _LL634;}else{
goto _LL635;}}else{goto _LL635;}_LL635: if(*((int*)_tmp277)== Cyc_Absyn_Var_e){
_LL650: _tmp289=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp277)->f2;if((
unsigned int)_tmp289 > 1?*((int*)_tmp289)== Cyc_Absyn_Local_b: 0){_LL652: _tmp28B=((
struct Cyc_Absyn_Local_b_struct*)_tmp289)->f1;goto _LL636;}else{goto _LL637;}}else{
goto _LL637;}_LL637: if(*((int*)_tmp277)== Cyc_Absyn_Var_e){_LL654: _tmp28D=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp277)->f2;if((unsigned int)_tmp28D > 1?*((int*)
_tmp28D)== Cyc_Absyn_Global_b: 0){_LL656: _tmp28F=((struct Cyc_Absyn_Global_b_struct*)
_tmp28D)->f1;goto _LL638;}else{goto _LL639;}}else{goto _LL639;}_LL639: if(*((int*)
_tmp277)== Cyc_Absyn_Var_e){_LL658: _tmp291=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp277)->f2;if((unsigned int)_tmp291 > 1?*((int*)_tmp291)== Cyc_Absyn_Param_b: 0){
_LL660: _tmp293=((struct Cyc_Absyn_Param_b_struct*)_tmp291)->f1;goto _LL640;}else{
goto _LL641;}}else{goto _LL641;}_LL641: if(*((int*)_tmp277)== Cyc_Absyn_Primop_e){
_LL667: _tmp29A=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp277)->f1;if(_tmp29A
== (void*)Cyc_Absyn_Size){goto _LL662;}else{goto _LL643;}_LL662: _tmp295=((struct
Cyc_Absyn_Primop_e_struct*)_tmp277)->f2;if(_tmp295 == 0){goto _LL643;}else{_tmp297=*
_tmp295;_LL665: _tmp298=(struct Cyc_Absyn_Exp*)_tmp297.hd;goto _LL642;}}else{goto
_LL643;}_LL643: goto _LL644;_LL634: _tmp28B=_tmp287;goto _LL636;_LL636: _tmp28F=
_tmp28B;goto _LL638;_LL638: _tmp293=_tmp28F;goto _LL640;_LL640: if(_tmp293->escapes){
return 0;}if(Cyc_Toc_check_leq_size_var(relns,v,_tmp293)){return 1;}goto _LL632;
_LL642:{void*_tmp29C=(void*)_tmp298->r;void*_tmp2A8;struct Cyc_Absyn_Vardecl*
_tmp2AA;void*_tmp2AC;struct Cyc_Absyn_Vardecl*_tmp2AE;void*_tmp2B0;struct Cyc_Absyn_Vardecl*
_tmp2B2;void*_tmp2B4;struct Cyc_Absyn_Vardecl*_tmp2B6;_LL670: if(*((int*)_tmp29C)
== Cyc_Absyn_Var_e){_LL681: _tmp2A8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp29C)->f2;if((unsigned int)_tmp2A8 > 1?*((int*)_tmp2A8)== Cyc_Absyn_Pat_b: 0){
_LL683: _tmp2AA=((struct Cyc_Absyn_Pat_b_struct*)_tmp2A8)->f1;goto _LL671;}else{
goto _LL672;}}else{goto _LL672;}_LL672: if(*((int*)_tmp29C)== Cyc_Absyn_Var_e){
_LL685: _tmp2AC=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp29C)->f2;if((
unsigned int)_tmp2AC > 1?*((int*)_tmp2AC)== Cyc_Absyn_Local_b: 0){_LL687: _tmp2AE=((
struct Cyc_Absyn_Local_b_struct*)_tmp2AC)->f1;goto _LL673;}else{goto _LL674;}}else{
goto _LL674;}_LL674: if(*((int*)_tmp29C)== Cyc_Absyn_Var_e){_LL689: _tmp2B0=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp29C)->f2;if((unsigned int)_tmp2B0 > 1?*((int*)
_tmp2B0)== Cyc_Absyn_Global_b: 0){_LL691: _tmp2B2=((struct Cyc_Absyn_Global_b_struct*)
_tmp2B0)->f1;goto _LL675;}else{goto _LL676;}}else{goto _LL676;}_LL676: if(*((int*)
_tmp29C)== Cyc_Absyn_Var_e){_LL693: _tmp2B4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp29C)->f2;if((unsigned int)_tmp2B4 > 1?*((int*)_tmp2B4)== Cyc_Absyn_Param_b: 0){
_LL695: _tmp2B6=((struct Cyc_Absyn_Param_b_struct*)_tmp2B4)->f1;goto _LL677;}else{
goto _LL678;}}else{goto _LL678;}_LL678: goto _LL679;_LL671: _tmp2AE=_tmp2AA;goto
_LL673;_LL673: _tmp2B2=_tmp2AE;goto _LL675;_LL675: _tmp2B6=_tmp2B2;goto _LL677;
_LL677: return _tmp2B6 == v;_LL679: goto _LL669;_LL669:;}goto _LL632;_LL644: goto _LL632;
_LL632:;}return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct
Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*_tmp2B8=(void*)a->r;void*_tmp2C4;
struct Cyc_Absyn_Vardecl*_tmp2C6;void*_tmp2C8;struct Cyc_Absyn_Vardecl*_tmp2CA;
void*_tmp2CC;struct Cyc_Absyn_Vardecl*_tmp2CE;void*_tmp2D0;struct Cyc_Absyn_Vardecl*
_tmp2D2;_LL698: if(*((int*)_tmp2B8)== Cyc_Absyn_Var_e){_LL709: _tmp2C4=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2B8)->f2;if((unsigned int)_tmp2C4 > 1?*((int*)
_tmp2C4)== Cyc_Absyn_Pat_b: 0){_LL711: _tmp2C6=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2C4)->f1;goto _LL699;}else{goto _LL700;}}else{goto _LL700;}_LL700: if(*((int*)
_tmp2B8)== Cyc_Absyn_Var_e){_LL713: _tmp2C8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2B8)->f2;if((unsigned int)_tmp2C8 > 1?*((int*)_tmp2C8)== Cyc_Absyn_Local_b: 0){
_LL715: _tmp2CA=((struct Cyc_Absyn_Local_b_struct*)_tmp2C8)->f1;goto _LL701;}else{
goto _LL702;}}else{goto _LL702;}_LL702: if(*((int*)_tmp2B8)== Cyc_Absyn_Var_e){
_LL717: _tmp2CC=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2B8)->f2;if((
unsigned int)_tmp2CC > 1?*((int*)_tmp2CC)== Cyc_Absyn_Global_b: 0){_LL719: _tmp2CE=((
struct Cyc_Absyn_Global_b_struct*)_tmp2CC)->f1;goto _LL703;}else{goto _LL704;}}
else{goto _LL704;}_LL704: if(*((int*)_tmp2B8)== Cyc_Absyn_Var_e){_LL721: _tmp2D0=(
void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2B8)->f2;if((unsigned int)_tmp2D0 > 1?*((
int*)_tmp2D0)== Cyc_Absyn_Param_b: 0){_LL723: _tmp2D2=((struct Cyc_Absyn_Param_b_struct*)
_tmp2D0)->f1;goto _LL705;}else{goto _LL706;}}else{goto _LL706;}_LL706: goto _LL707;
_LL699: _tmp2CA=_tmp2C6;goto _LL701;_LL701: _tmp2CE=_tmp2CA;goto _LL703;_LL703:
_tmp2D2=_tmp2CE;goto _LL705;_LL705: if(_tmp2D2->escapes){return 0;}inner_loop: {
void*_tmp2D4=(void*)i->r;struct Cyc_Absyn_Exp*_tmp2E8;void*_tmp2EA;void*_tmp2EC;
int _tmp2EE;void*_tmp2F0;void*_tmp2F2;int _tmp2F4;void*_tmp2F6;struct Cyc_List_List*
_tmp2F8;struct Cyc_List_List _tmp2FA;struct Cyc_List_List*_tmp2FB;struct Cyc_List_List
_tmp2FD;struct Cyc_Absyn_Exp*_tmp2FE;struct Cyc_Absyn_Exp*_tmp300;void*_tmp302;
void*_tmp304;struct Cyc_Absyn_Vardecl*_tmp306;void*_tmp308;struct Cyc_Absyn_Vardecl*
_tmp30A;void*_tmp30C;struct Cyc_Absyn_Vardecl*_tmp30E;void*_tmp310;struct Cyc_Absyn_Vardecl*
_tmp312;_LL726: if(*((int*)_tmp2D4)== Cyc_Absyn_Cast_e){_LL747: _tmp2EA=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp2D4)->f1;goto _LL745;_LL745: _tmp2E8=((struct
Cyc_Absyn_Cast_e_struct*)_tmp2D4)->f2;goto _LL727;}else{goto _LL728;}_LL728: if(*((
int*)_tmp2D4)== Cyc_Absyn_Const_e){_LL749: _tmp2EC=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp2D4)->f1;if((unsigned int)_tmp2EC > 1?*((int*)_tmp2EC)== Cyc_Absyn_Int_c: 0){
_LL753: _tmp2F0=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp2EC)->f1;if(_tmp2F0 == (
void*)Cyc_Absyn_Signed){goto _LL751;}else{goto _LL730;}_LL751: _tmp2EE=((struct Cyc_Absyn_Int_c_struct*)
_tmp2EC)->f2;goto _LL729;}else{goto _LL730;}}else{goto _LL730;}_LL730: if(*((int*)
_tmp2D4)== Cyc_Absyn_Const_e){_LL755: _tmp2F2=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp2D4)->f1;if((unsigned int)_tmp2F2 > 1?*((int*)_tmp2F2)== Cyc_Absyn_Int_c: 0){
_LL759: _tmp2F6=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp2F2)->f1;if(_tmp2F6 == (
void*)Cyc_Absyn_Unsigned){goto _LL757;}else{goto _LL732;}_LL757: _tmp2F4=((struct
Cyc_Absyn_Int_c_struct*)_tmp2F2)->f2;goto _LL731;}else{goto _LL732;}}else{goto
_LL732;}_LL732: if(*((int*)_tmp2D4)== Cyc_Absyn_Primop_e){_LL771: _tmp302=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp2D4)->f1;if(_tmp302 == (void*)Cyc_Absyn_Mod){
goto _LL761;}else{goto _LL734;}_LL761: _tmp2F8=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2D4)->f2;if(_tmp2F8 == 0){goto _LL734;}else{_tmp2FA=*_tmp2F8;_LL769: _tmp300=(
struct Cyc_Absyn_Exp*)_tmp2FA.hd;goto _LL764;_LL764: _tmp2FB=_tmp2FA.tl;if(_tmp2FB
== 0){goto _LL734;}else{_tmp2FD=*_tmp2FB;_LL767: _tmp2FE=(struct Cyc_Absyn_Exp*)
_tmp2FD.hd;goto _LL733;}}}else{goto _LL734;}_LL734: if(*((int*)_tmp2D4)== Cyc_Absyn_Var_e){
_LL773: _tmp304=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2D4)->f2;if((
unsigned int)_tmp304 > 1?*((int*)_tmp304)== Cyc_Absyn_Pat_b: 0){_LL775: _tmp306=((
struct Cyc_Absyn_Pat_b_struct*)_tmp304)->f1;goto _LL735;}else{goto _LL736;}}else{
goto _LL736;}_LL736: if(*((int*)_tmp2D4)== Cyc_Absyn_Var_e){_LL777: _tmp308=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp2D4)->f2;if((unsigned int)_tmp308 > 1?*((int*)
_tmp308)== Cyc_Absyn_Local_b: 0){_LL779: _tmp30A=((struct Cyc_Absyn_Local_b_struct*)
_tmp308)->f1;goto _LL737;}else{goto _LL738;}}else{goto _LL738;}_LL738: if(*((int*)
_tmp2D4)== Cyc_Absyn_Var_e){_LL781: _tmp30C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D4)->f2;if((unsigned int)_tmp30C > 1?*((int*)_tmp30C)== Cyc_Absyn_Global_b: 0){
_LL783: _tmp30E=((struct Cyc_Absyn_Global_b_struct*)_tmp30C)->f1;goto _LL739;}else{
goto _LL740;}}else{goto _LL740;}_LL740: if(*((int*)_tmp2D4)== Cyc_Absyn_Var_e){
_LL785: _tmp310=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2D4)->f2;if((
unsigned int)_tmp310 > 1?*((int*)_tmp310)== Cyc_Absyn_Param_b: 0){_LL787: _tmp312=((
struct Cyc_Absyn_Param_b_struct*)_tmp310)->f1;goto _LL741;}else{goto _LL742;}}else{
goto _LL742;}_LL742: goto _LL743;_LL727: i=_tmp2E8;goto inner_loop;_LL729: return
_tmp2EE >= 0? Cyc_Toc_check_const_array((unsigned int)(_tmp2EE + 1),(void*)_tmp2D2->type):
0;_LL731: return Cyc_Toc_check_const_array((unsigned int)(_tmp2F4 + 1),(void*)
_tmp2D2->type);_LL733: return Cyc_Toc_check_leq_size(relns,_tmp2D2,_tmp2FE);_LL735:
_tmp30A=_tmp306;goto _LL737;_LL737: _tmp30E=_tmp30A;goto _LL739;_LL739: _tmp312=
_tmp30E;goto _LL741;_LL741: if(_tmp312->escapes){return 0;}{struct Cyc_List_List*
_tmp314=relns;for(0;_tmp314 != 0;_tmp314=_tmp314->tl){struct Cyc_CfFlowInfo_Reln*
_tmp315=(struct Cyc_CfFlowInfo_Reln*)_tmp314->hd;if(_tmp315->vd == _tmp312){void*
_tmp316=(void*)_tmp315->rop;struct Cyc_Absyn_Vardecl*_tmp320;struct Cyc_Absyn_Vardecl*
_tmp322;unsigned int _tmp324;_LL792: if(*((int*)_tmp316)== Cyc_CfFlowInfo_LessSize){
_LL801: _tmp320=((struct Cyc_CfFlowInfo_LessSize_struct*)_tmp316)->f1;goto _LL793;}
else{goto _LL794;}_LL794: if(*((int*)_tmp316)== Cyc_CfFlowInfo_LessVar){_LL803:
_tmp322=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp316)->f1;goto _LL795;}else{
goto _LL796;}_LL796: if(*((int*)_tmp316)== Cyc_CfFlowInfo_LessConst){_LL805:
_tmp324=((struct Cyc_CfFlowInfo_LessConst_struct*)_tmp316)->f1;goto _LL797;}else{
goto _LL798;}_LL798: goto _LL799;_LL793: if(_tmp2D2 == _tmp320){return 1;}else{goto
_LL791;}_LL795:{struct Cyc_List_List*_tmp326=relns;for(0;_tmp326 != 0;_tmp326=
_tmp326->tl){struct Cyc_CfFlowInfo_Reln*_tmp327=(struct Cyc_CfFlowInfo_Reln*)
_tmp326->hd;if(_tmp327->vd == _tmp322){void*_tmp328=(void*)_tmp327->rop;struct Cyc_Absyn_Vardecl*
_tmp334;struct Cyc_Absyn_Vardecl*_tmp336;unsigned int _tmp338;struct Cyc_Absyn_Vardecl*
_tmp33A;_LL810: if(*((int*)_tmp328)== Cyc_CfFlowInfo_LessEqSize){_LL821: _tmp334=((
struct Cyc_CfFlowInfo_LessEqSize_struct*)_tmp328)->f1;goto _LL811;}else{goto _LL812;}
_LL812: if(*((int*)_tmp328)== Cyc_CfFlowInfo_LessSize){_LL823: _tmp336=((struct Cyc_CfFlowInfo_LessSize_struct*)
_tmp328)->f1;goto _LL813;}else{goto _LL814;}_LL814: if(*((int*)_tmp328)== Cyc_CfFlowInfo_EqualConst){
_LL825: _tmp338=((struct Cyc_CfFlowInfo_EqualConst_struct*)_tmp328)->f1;goto _LL815;}
else{goto _LL816;}_LL816: if(*((int*)_tmp328)== Cyc_CfFlowInfo_LessVar){_LL827:
_tmp33A=((struct Cyc_CfFlowInfo_LessVar_struct*)_tmp328)->f1;goto _LL817;}else{
goto _LL818;}_LL818: goto _LL819;_LL811: _tmp336=_tmp334;goto _LL813;_LL813: if(
_tmp2D2 == _tmp336){return 1;}else{goto _LL809;}_LL815: return Cyc_Toc_check_const_array(
_tmp338,(void*)_tmp2D2->type);_LL817: if(Cyc_Toc_check_leq_size_var(relns,_tmp2D2,
_tmp33A)){return 1;}else{goto _LL809;}_LL819: goto _LL809;_LL809:;}}}goto _LL791;
_LL797: return Cyc_Toc_check_const_array(_tmp324,(void*)_tmp2D2->type);_LL799: goto
_LL791;_LL791:;}}}goto _LL725;_LL743: goto _LL725;_LL725:;}goto _LL697;_LL707: goto
_LL697;_LL697:;}return 0;}static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){if(
e->topt == 0){({void*_tmp33C[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Missing type in primop ",sizeof(
unsigned char),24),_tag_arr(_tmp33C,sizeof(void*),0));});}return Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0){({void*_tmp33D[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Missing type in primop ",
sizeof(unsigned char),24),_tag_arr(_tmp33D,sizeof(void*),0));});}return(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v;}static struct _tuple3*Cyc_Toc_tup_to_c(
struct Cyc_Absyn_Exp*e){return({struct _tuple3*_tmp33E=_cycalloc(sizeof(struct
_tuple3));_tmp33E->f1=Cyc_Toc_mt_tq;_tmp33E->f2=Cyc_Toc_typ_to_c((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_tmp33E;});}static struct _tuple4*Cyc_Toc_add_designator(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);return({
struct _tuple4*_tmp33F=_cycalloc(sizeof(struct _tuple4));_tmp33F->f1=0;_tmp33F->f2=
e;_tmp33F;});}static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,
struct _tuple0*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*
rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp340=Cyc_Absyn_sizeoftyp_exp(
struct_typ,0);if(rgnopt == 0){eo=(struct Cyc_Absyn_Exp*)(is_atomic? Cyc_Toc_malloc_atomic(
_tmp340): Cyc_Toc_malloc_ptr(_tmp340));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_check_null(rgnopt);Cyc_Toc_exp_to_c(nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(
r,_tmp340);}}}else{t=struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(
x,t,eo,s,0),0);}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*s,int e1_already_translated);static
struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);static
struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct Cyc_List_List*_tmp341=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;
_tmp341 != 0;_tmp341=_tmp341->tl){struct _tuple4 _tmp344;struct Cyc_Absyn_Exp*
_tmp345;struct Cyc_List_List*_tmp347;struct _tuple4*_tmp342=(struct _tuple4*)
_tmp341->hd;_tmp344=*_tmp342;_LL840: _tmp347=_tmp344.f1;goto _LL838;_LL838: _tmp345=
_tmp344.f2;goto _LL835;_LL835: {struct Cyc_Absyn_Exp*e_index;if(_tmp347 == 0){
e_index=Cyc_Absyn_signed_int_exp(count --,0);}else{if(_tmp347->tl != 0){({void*
_tmp349[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("multiple designators in array",sizeof(unsigned char),30),_tag_arr(
_tmp349,sizeof(void*),0));});}{void*_tmp34A=(void*)_tmp347->hd;void*_tmp34B=
_tmp34A;struct Cyc_Absyn_Exp*_tmp351;_LL845: if(*((int*)_tmp34B)== Cyc_Absyn_ArrayElement){
_LL850: _tmp351=((struct Cyc_Absyn_ArrayElement_struct*)_tmp34B)->f1;goto _LL846;}
else{goto _LL847;}_LL847: if(*((int*)_tmp34B)== Cyc_Absyn_FieldName){goto _LL848;}
else{goto _LL844;}_LL846: Cyc_Toc_exp_to_c(nv,_tmp351);e_index=_tmp351;goto _LL844;
_LL848: e_index=({void*_tmp353[0]={};((struct Cyc_Absyn_Exp*(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("field name designators in array",
sizeof(unsigned char),32),_tag_arr(_tmp353,sizeof(void*),0));});goto _LL844;
_LL844:;}}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);void*
_tmp354=(void*)_tmp345->r;struct Cyc_List_List*_tmp35E;struct Cyc_Absyn_Exp*
_tmp360;struct Cyc_Absyn_Exp*_tmp362;struct Cyc_Absyn_Vardecl*_tmp364;struct Cyc_List_List*
_tmp366;void*_tmp368;_LL854: if(*((int*)_tmp354)== Cyc_Absyn_Array_e){_LL863:
_tmp35E=((struct Cyc_Absyn_Array_e_struct*)_tmp354)->f1;goto _LL855;}else{goto
_LL856;}_LL856: if(*((int*)_tmp354)== Cyc_Absyn_Comprehension_e){_LL869: _tmp364=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp354)->f1;goto _LL867;_LL867: _tmp362=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp354)->f2;goto _LL865;_LL865: _tmp360=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp354)->f3;goto _LL857;}else{goto _LL858;}
_LL858: if(*((int*)_tmp354)== Cyc_Absyn_AnonStruct_e){_LL873: _tmp368=(void*)((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp354)->f1;goto _LL871;_LL871: _tmp366=((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp354)->f2;goto _LL859;}else{goto _LL860;}
_LL860: goto _LL861;_LL855: s=Cyc_Toc_init_array(nv,lval,_tmp35E,s);goto _LL853;
_LL857: s=Cyc_Toc_init_comprehension(nv,lval,_tmp364,_tmp362,_tmp360,s,0);goto
_LL853;_LL859: s=Cyc_Toc_init_anon_struct(nv,lval,_tmp368,_tmp366,s);goto _LL853;
_LL861: Cyc_Toc_exp_to_c(nv,_tmp345);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp(lhs,e_index,0),_tmp345,0),s,0);goto _LL853;_LL853:;}}}}
return s;}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct
_tuple0*_tmp36A=vd->name;if(! e1_already_translated){Cyc_Toc_exp_to_c(nv,e1);}{
struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(nv,_tmp36A,Cyc_Absyn_varb_exp(_tmp36A,(
void*)({struct Cyc_Absyn_Local_b_struct*_tmp381=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_struct));
_tmp381[0]=({struct Cyc_Absyn_Local_b_struct _tmp382;_tmp382.tag=Cyc_Absyn_Local_b;
_tmp382.f1=vd;_tmp382;});_tmp381;}),0));struct _tuple0*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp36A,0),Cyc_Absyn_signed_int_exp(
0,0),0);struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp36A,0),Cyc_Absyn_var_exp(
max,0),0);struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(
_tmp36A,0),0);struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(
_tmp36A,0),0);struct Cyc_Absyn_Stmt*body;{void*_tmp36B=(void*)e2->r;struct Cyc_List_List*
_tmp375;struct Cyc_Absyn_Exp*_tmp377;struct Cyc_Absyn_Exp*_tmp379;struct Cyc_Absyn_Vardecl*
_tmp37B;struct Cyc_List_List*_tmp37D;void*_tmp37F;_LL877: if(*((int*)_tmp36B)== 
Cyc_Absyn_Array_e){_LL886: _tmp375=((struct Cyc_Absyn_Array_e_struct*)_tmp36B)->f1;
goto _LL878;}else{goto _LL879;}_LL879: if(*((int*)_tmp36B)== Cyc_Absyn_Comprehension_e){
_LL892: _tmp37B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp36B)->f1;goto _LL890;
_LL890: _tmp379=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp36B)->f2;goto _LL888;
_LL888: _tmp377=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp36B)->f3;goto _LL880;}
else{goto _LL881;}_LL881: if(*((int*)_tmp36B)== Cyc_Absyn_AnonStruct_e){_LL896:
_tmp37F=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp36B)->f1;goto _LL894;
_LL894: _tmp37D=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp36B)->f2;goto _LL882;}
else{goto _LL883;}_LL883: goto _LL884;_LL878: body=Cyc_Toc_init_array(nv2,lval,
_tmp375,Cyc_Absyn_skip_stmt(0));goto _LL876;_LL880: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp37B,_tmp379,_tmp377,Cyc_Absyn_skip_stmt(0),0);goto _LL876;_LL882:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp37F,_tmp37D,Cyc_Absyn_skip_stmt(0));
goto _LL876;_LL884: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(lval,e2,0);
goto _LL876;_LL876:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_t,(struct Cyc_Absyn_Exp*)
e1,Cyc_Absyn_declare_stmt(_tmp36A,Cyc_Absyn_uint_t,0,s2,0),0),s,0);return s;}}}
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp383=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp383 != 0;_tmp383=_tmp383->tl){struct _tuple4 _tmp386;
struct Cyc_Absyn_Exp*_tmp387;struct Cyc_List_List*_tmp389;struct _tuple4*_tmp384=(
struct _tuple4*)_tmp383->hd;_tmp386=*_tmp384;_LL906: _tmp389=_tmp386.f1;goto _LL904;
_LL904: _tmp387=_tmp386.f2;goto _LL901;_LL901: if(_tmp389 == 0){({void*_tmp38B[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("empty designator list",
sizeof(unsigned char),22),_tag_arr(_tmp38B,sizeof(void*),0));});}if(_tmp389->tl
!= 0){({void*_tmp38C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("too many designators in anonymous struct",sizeof(
unsigned char),41),_tag_arr(_tmp38C,sizeof(void*),0));});}{void*_tmp38D=(void*)
_tmp389->hd;struct _tagged_arr*_tmp393;_LL911: if(*((int*)_tmp38D)== Cyc_Absyn_FieldName){
_LL916: _tmp393=((struct Cyc_Absyn_FieldName_struct*)_tmp38D)->f1;goto _LL912;}
else{goto _LL913;}_LL913: goto _LL914;_LL912: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_structmember_exp(
lhs,_tmp393,0);{void*_tmp395=(void*)_tmp387->r;struct Cyc_List_List*_tmp39F;
struct Cyc_Absyn_Exp*_tmp3A1;struct Cyc_Absyn_Exp*_tmp3A3;struct Cyc_Absyn_Vardecl*
_tmp3A5;struct Cyc_List_List*_tmp3A7;void*_tmp3A9;_LL919: if(*((int*)_tmp395)== 
Cyc_Absyn_Array_e){_LL928: _tmp39F=((struct Cyc_Absyn_Array_e_struct*)_tmp395)->f1;
goto _LL920;}else{goto _LL921;}_LL921: if(*((int*)_tmp395)== Cyc_Absyn_Comprehension_e){
_LL934: _tmp3A5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp395)->f1;goto _LL932;
_LL932: _tmp3A3=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp395)->f2;goto _LL930;
_LL930: _tmp3A1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp395)->f3;goto _LL922;}
else{goto _LL923;}_LL923: if(*((int*)_tmp395)== Cyc_Absyn_AnonStruct_e){_LL938:
_tmp3A9=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp395)->f1;goto _LL936;
_LL936: _tmp3A7=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp395)->f2;goto _LL924;}
else{goto _LL925;}_LL925: goto _LL926;_LL920: s=Cyc_Toc_init_array(nv,lval,_tmp39F,s);
goto _LL918;_LL922: s=Cyc_Toc_init_comprehension(nv,lval,_tmp3A5,_tmp3A3,_tmp3A1,s,
0);goto _LL918;_LL924: s=Cyc_Toc_init_anon_struct(nv,lval,_tmp3A9,_tmp3A7,s);goto
_LL918;_LL926: Cyc_Toc_exp_to_c(nv,_tmp387);if(Cyc_Toc_is_poly_field(struct_type,
_tmp393)){_tmp387=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),_tmp387,0);}s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp387,0),0),s,0);goto _LL918;
_LL918:;}goto _LL910;}_LL914:(int)_throw(({void*_tmp3AB[0]={};Cyc_Toc_toc_impos(
_tag_arr("array designator in struct",sizeof(unsigned char),27),_tag_arr(_tmp3AB,
sizeof(void*),0));}));_LL910:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(
struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
es){struct _RegionHandle _tmp3AC=_new_region("r");struct _RegionHandle*r=& _tmp3AC;
_push_region(r);{struct Cyc_List_List*_tmp3AD=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple3*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);struct _tagged_arr*_tmp3AE=Cyc_Toc_add_tuple_type(
_tmp3AD);struct _tuple0*_tmp3AF=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp3B0=
Cyc_Absyn_var_exp(_tmp3AF,0);struct Cyc_Absyn_Stmt*_tmp3B1=Cyc_Absyn_exp_stmt(
_tmp3B0,0);struct Cyc_Absyn_Exp*(*_tmp3B2)(struct Cyc_Absyn_Exp*,struct _tagged_arr*,
struct Cyc_Position_Segment*)=pointer? Cyc_Absyn_structarrow_exp: Cyc_Absyn_structmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp3B3=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp3B3);for(0;_tmp3B3 != 0;(_tmp3B3=_tmp3B3->tl,
-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp3B3->hd;struct Cyc_Absyn_Exp*
lval=_tmp3B2(_tmp3B0,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic? Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v): 0;{void*_tmp3B4=(void*)e->r;
struct Cyc_List_List*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BE;struct Cyc_Absyn_Exp*
_tmp3C0;struct Cyc_Absyn_Vardecl*_tmp3C2;_LL950: if(*((int*)_tmp3B4)== Cyc_Absyn_Array_e){
_LL957: _tmp3BC=((struct Cyc_Absyn_Array_e_struct*)_tmp3B4)->f1;goto _LL951;}else{
goto _LL952;}_LL952: if(*((int*)_tmp3B4)== Cyc_Absyn_Comprehension_e){_LL963:
_tmp3C2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3B4)->f1;goto _LL961;_LL961:
_tmp3C0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3B4)->f2;goto _LL959;_LL959:
_tmp3BE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3B4)->f3;goto _LL953;}else{
goto _LL954;}_LL954: goto _LL955;_LL951: _tmp3B1=Cyc_Toc_init_array(nv,lval,_tmp3BC,
_tmp3B1);goto _LL949;_LL953: _tmp3B1=Cyc_Toc_init_comprehension(nv,lval,_tmp3C2,
_tmp3C0,_tmp3BE,_tmp3B1,0);goto _LL949;_LL955: Cyc_Toc_exp_to_c(nv,e);_tmp3B1=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3B2(_tmp3B0,Cyc_Absyn_fieldname(i),0),
e,0),0),_tmp3B1,0);goto _LL949;_LL949:;}}}{struct Cyc_Absyn_Exp*_tmp3C4=Cyc_Toc_make_struct(
nv,_tmp3AF,Cyc_Absyn_strct(_tmp3AE),_tmp3B1,pointer,rgnopt,is_atomic);
_npop_handler(0);return _tmp3C4;}};_pop_region(r);}static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(
struct Cyc_Toc_Env*nv,void*struct_type,int pointer,struct Cyc_Absyn_Exp*rgnopt,
struct Cyc_List_List*dles,struct _tuple0*tdn){struct _tuple0*_tmp3C5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3C6=Cyc_Absyn_var_exp(_tmp3C5,0);struct Cyc_Absyn_Stmt*
_tmp3C7=Cyc_Absyn_exp_stmt(_tmp3C6,0);struct Cyc_Absyn_Exp*(*_tmp3C8)(struct Cyc_Absyn_Exp*,
struct _tagged_arr*,struct Cyc_Position_Segment*)=pointer? Cyc_Absyn_structarrow_exp:
Cyc_Absyn_structmember_exp;int is_atomic=1;struct _RegionHandle _tmp3C9=_new_region("r");
struct _RegionHandle*r=& _tmp3C9;_push_region(r);{struct Cyc_List_List*_tmp3CA=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
r,dles);for(0;_tmp3CA != 0;_tmp3CA=_tmp3CA->tl){struct _tuple4 _tmp3CD;struct Cyc_Absyn_Exp*
_tmp3CE;struct Cyc_List_List*_tmp3D0;struct _tuple4*_tmp3CB=(struct _tuple4*)
_tmp3CA->hd;_tmp3CD=*_tmp3CB;_LL977: _tmp3D0=_tmp3CD.f1;goto _LL975;_LL975: _tmp3CE=
_tmp3CD.f2;goto _LL972;_LL972: is_atomic=is_atomic? Cyc_Toc_atomic_typ((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3CE->topt))->v): 0;if(_tmp3D0 == 0){({void*
_tmp3D2[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("empty designator list",sizeof(unsigned char),22),_tag_arr(_tmp3D2,
sizeof(void*),0));});}if(_tmp3D0->tl != 0){struct _tuple0*_tmp3D3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3D4=Cyc_Absyn_var_exp(_tmp3D3,0);for(0;_tmp3D0 != 0;
_tmp3D0=_tmp3D0->tl){void*_tmp3D5=(void*)_tmp3D0->hd;struct _tagged_arr*_tmp3DB;
_LL983: if(*((int*)_tmp3D5)== Cyc_Absyn_FieldName){_LL988: _tmp3DB=((struct Cyc_Absyn_FieldName_struct*)
_tmp3D5)->f1;goto _LL984;}else{goto _LL985;}_LL985: goto _LL986;_LL984: if(Cyc_Toc_is_poly_field(
struct_type,_tmp3DB)){_tmp3D4=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
_tmp3D4,0);}_tmp3C7=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp3C8(_tmp3C6,_tmp3DB,0),_tmp3D4,0),0),_tmp3C7,0);goto _LL982;_LL986:(int)
_throw(({void*_tmp3DD[0]={};Cyc_Toc_toc_impos(_tag_arr("array designator in struct",
sizeof(unsigned char),27),_tag_arr(_tmp3DD,sizeof(void*),0));}));_LL982:;}Cyc_Toc_exp_to_c(
nv,_tmp3CE);_tmp3C7=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp3D4,_tmp3CE,0),0),_tmp3C7,0);}else{void*_tmp3DE=(void*)_tmp3D0->hd;struct
_tagged_arr*_tmp3E4;_LL992: if(*((int*)_tmp3DE)== Cyc_Absyn_FieldName){_LL997:
_tmp3E4=((struct Cyc_Absyn_FieldName_struct*)_tmp3DE)->f1;goto _LL993;}else{goto
_LL994;}_LL994: goto _LL995;_LL993: {struct Cyc_Absyn_Exp*lval=_tmp3C8(_tmp3C6,
_tmp3E4,0);{void*_tmp3E6=(void*)_tmp3CE->r;struct Cyc_List_List*_tmp3F0;struct Cyc_Absyn_Exp*
_tmp3F2;struct Cyc_Absyn_Exp*_tmp3F4;struct Cyc_Absyn_Vardecl*_tmp3F6;struct Cyc_List_List*
_tmp3F8;void*_tmp3FA;_LL1000: if(*((int*)_tmp3E6)== Cyc_Absyn_Array_e){_LL1009:
_tmp3F0=((struct Cyc_Absyn_Array_e_struct*)_tmp3E6)->f1;goto _LL1001;}else{goto
_LL1002;}_LL1002: if(*((int*)_tmp3E6)== Cyc_Absyn_Comprehension_e){_LL1015:
_tmp3F6=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3E6)->f1;goto _LL1013;
_LL1013: _tmp3F4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3E6)->f2;goto
_LL1011;_LL1011: _tmp3F2=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3E6)->f3;
goto _LL1003;}else{goto _LL1004;}_LL1004: if(*((int*)_tmp3E6)== Cyc_Absyn_AnonStruct_e){
_LL1019: _tmp3FA=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3E6)->f1;goto
_LL1017;_LL1017: _tmp3F8=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3E6)->f2;goto
_LL1005;}else{goto _LL1006;}_LL1006: goto _LL1007;_LL1001: _tmp3C7=Cyc_Toc_init_array(
nv,lval,_tmp3F0,_tmp3C7);goto _LL999;_LL1003: _tmp3C7=Cyc_Toc_init_comprehension(
nv,lval,_tmp3F6,_tmp3F4,_tmp3F2,_tmp3C7,0);goto _LL999;_LL1005: _tmp3C7=Cyc_Toc_init_anon_struct(
nv,lval,_tmp3FA,_tmp3F8,_tmp3C7);goto _LL999;_LL1007: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3CE->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp3CE);if(Cyc_Toc_is_poly_field(struct_type,_tmp3E4)? ! was_ptr_type: 0){_tmp3CE=
Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),_tmp3CE,0);}_tmp3C7=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3CE,0),0),_tmp3C7,0);goto _LL999;}
_LL999:;}goto _LL991;}_LL995:(int)_throw(({void*_tmp3FC[0]={};Cyc_Toc_toc_impos(
_tag_arr("array designator in struct",sizeof(unsigned char),27),_tag_arr(_tmp3FC,
sizeof(void*),0));}));_LL991:;}}}{struct Cyc_Absyn_Exp*_tmp3FD=Cyc_Toc_make_struct(
nv,_tmp3C5,Cyc_Absyn_strctq(tdn),_tmp3C7,pointer,rgnopt,is_atomic);_npop_handler(
0);return _tmp3FD;};_pop_region(r);}struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*
f2;};static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,
struct _tuple7*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static
struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,void*incr){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmp3FE=_cycalloc(sizeof(struct Cyc_Absyn_Increment_e_struct));_tmp3FE[0]=({
struct Cyc_Absyn_Increment_e_struct _tmp3FF;_tmp3FF.tag=Cyc_Absyn_Increment_e;
_tmp3FF.f1=e1;_tmp3FF.f2=(void*)incr;_tmp3FF;});_tmp3FE;}),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp400=(void*)e1->r;struct Cyc_Absyn_Stmt*_tmp40A;struct Cyc_Absyn_Exp*
_tmp40C;void*_tmp40E;struct _tagged_arr*_tmp410;struct Cyc_Absyn_Exp*_tmp412;
_LL1026: if(*((int*)_tmp400)== Cyc_Absyn_StmtExp_e){_LL1035: _tmp40A=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp400)->f1;goto _LL1027;}else{goto _LL1028;}_LL1028: if(*((int*)_tmp400)== Cyc_Absyn_Cast_e){
_LL1039: _tmp40E=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp400)->f1;goto _LL1037;
_LL1037: _tmp40C=((struct Cyc_Absyn_Cast_e_struct*)_tmp400)->f2;goto _LL1029;}else{
goto _LL1030;}_LL1030: if(*((int*)_tmp400)== Cyc_Absyn_StructMember_e){_LL1043:
_tmp412=((struct Cyc_Absyn_StructMember_e_struct*)_tmp400)->f1;goto _LL1041;
_LL1041: _tmp410=((struct Cyc_Absyn_StructMember_e_struct*)_tmp400)->f2;goto
_LL1031;}else{goto _LL1032;}_LL1032: goto _LL1033;_LL1027: Cyc_Toc_lvalue_assign_stmt(
_tmp40A,fs,f,f_env);goto _LL1025;_LL1029: Cyc_Toc_lvalue_assign(_tmp40C,fs,f,f_env);
goto _LL1025;_LL1031:(void*)(e1->r=(void*)((void*)_tmp412->r));Cyc_Toc_lvalue_assign(
e1,(struct Cyc_List_List*)({struct Cyc_List_List*_tmp414=_cycalloc(sizeof(struct
Cyc_List_List));_tmp414->hd=_tmp410;_tmp414->tl=fs;_tmp414;}),f,f_env);goto
_LL1025;_LL1033: {struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 
0;fs=fs->tl){e1_copy=Cyc_Absyn_structmember_exp(e1_copy,(struct _tagged_arr*)fs->hd,
e1_copy->loc);}(void*)(e1->r=(void*)((void*)(f(e1_copy,f_env))->r));goto _LL1025;}
_LL1025:;}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp415=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp41F;struct Cyc_Absyn_Stmt*_tmp421;struct Cyc_Absyn_Decl*
_tmp423;struct Cyc_Absyn_Stmt*_tmp425;_LL1047: if((unsigned int)_tmp415 > 1?*((int*)
_tmp415)== Cyc_Absyn_Exp_s: 0){_LL1056: _tmp41F=((struct Cyc_Absyn_Exp_s_struct*)
_tmp415)->f1;goto _LL1048;}else{goto _LL1049;}_LL1049: if((unsigned int)_tmp415 > 1?*((
int*)_tmp415)== Cyc_Absyn_Decl_s: 0){_LL1060: _tmp423=((struct Cyc_Absyn_Decl_s_struct*)
_tmp415)->f1;goto _LL1058;_LL1058: _tmp421=((struct Cyc_Absyn_Decl_s_struct*)
_tmp415)->f2;goto _LL1050;}else{goto _LL1051;}_LL1051: if((unsigned int)_tmp415 > 1?*((
int*)_tmp415)== Cyc_Absyn_Seq_s: 0){_LL1062: _tmp425=((struct Cyc_Absyn_Seq_s_struct*)
_tmp415)->f2;goto _LL1052;}else{goto _LL1053;}_LL1053: goto _LL1054;_LL1048: Cyc_Toc_lvalue_assign(
_tmp41F,fs,f,f_env);goto _LL1046;_LL1050: Cyc_Toc_lvalue_assign_stmt(_tmp421,fs,f,
f_env);goto _LL1046;_LL1052: Cyc_Toc_lvalue_assign_stmt(_tmp425,fs,f,f_env);goto
_LL1046;_LL1054:({struct Cyc_Std_String_pa_struct _tmp428;_tmp428.tag=Cyc_Std_String_pa;
_tmp428.f1=(struct _tagged_arr)Cyc_Absynpp_stmt2string(s);{void*_tmp427[1]={&
_tmp428};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("lvalue_assign_stmt: %s",sizeof(unsigned char),23),_tag_arr(_tmp427,
sizeof(void*),1));}});goto _LL1046;_LL1046:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(
struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0){return 0;}result=({
struct Cyc_List_List*_tmp429=_region_malloc(r2,sizeof(struct Cyc_List_List));
_tmp429->hd=(void*)f((void*)x->hd,env);_tmp429->tl=0;_tmp429;});prev=result;for(
x=x->tl;x != 0;x=x->tl){((struct Cyc_List_List*)_check_null(prev))->tl=({struct Cyc_List_List*
_tmp42A=_region_malloc(r2,sizeof(struct Cyc_List_List));_tmp42A->hd=(void*)f((
void*)x->hd,env);_tmp42A->tl=0;_tmp42A;});prev=((struct Cyc_List_List*)
_check_null(prev))->tl;}return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,
f,env,x);}static struct _tuple4*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){return({
struct _tuple4*_tmp42B=_cycalloc(sizeof(struct _tuple4));_tmp42B->f1=0;_tmp42B->f2=
e;_tmp42B;});}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*
_tmp42C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp432;_LL1070: if((
unsigned int)_tmp42C > 3?*((int*)_tmp42C)== Cyc_Absyn_PointerType: 0){_LL1075:
_tmp432=((struct Cyc_Absyn_PointerType_struct*)_tmp42C)->f1;goto _LL1071;}else{
goto _LL1072;}_LL1072: goto _LL1073;_LL1071: return _tmp432;_LL1073:(int)_throw(({
void*_tmp434[0]={};Cyc_Tcutil_impos(_tag_arr("get_ptr_typ: not a pointer!",
sizeof(unsigned char),28),_tag_arr(_tmp434,sizeof(void*),0));}));_LL1069:;}
struct _tuple8{void*f1;void*f2;};struct _tuple9{struct _tuple0*f1;void*f2;struct Cyc_Absyn_Exp*
f3;};struct _tuple10{struct Cyc_Absyn_Structfield*f1;struct Cyc_Absyn_Exp*f2;};
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*
_tmp435=(void*)e->r;if(e->topt == 0){({struct Cyc_Std_String_pa_struct _tmp437;
_tmp437.tag=Cyc_Std_String_pa;_tmp437.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(
e);{void*_tmp436[1]={& _tmp437};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: no type for %s",sizeof(unsigned char),25),
_tag_arr(_tmp436,sizeof(void*),1));}});return;}{void*old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;void*_tmp438=_tmp435;void*_tmp48E;void*_tmp490;struct
_tuple0*_tmp492;struct _tuple0*_tmp494;struct Cyc_List_List*_tmp496;void*_tmp498;
void*_tmp49A;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Core_Opt*
_tmp4A0;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Absyn_Exp*
_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A8;struct Cyc_Absyn_Exp*_tmp4AA;struct Cyc_Absyn_Exp*
_tmp4AC;struct Cyc_List_List*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4B0;struct Cyc_Absyn_VarargCallInfo*
_tmp4B2;struct Cyc_List_List*_tmp4B4;struct Cyc_Absyn_Exp*_tmp4B6;struct Cyc_Absyn_VarargCallInfo*
_tmp4B8;struct Cyc_Absyn_VarargCallInfo _tmp4BA;struct Cyc_Absyn_VarargInfo*_tmp4BB;
struct Cyc_List_List*_tmp4BD;int _tmp4BF;struct Cyc_List_List*_tmp4C1;struct Cyc_Absyn_Exp*
_tmp4C3;struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C7;struct Cyc_List_List*
_tmp4C9;struct Cyc_Absyn_Exp*_tmp4CB;struct Cyc_Absyn_Exp*_tmp4CD;void*_tmp4CF;
void**_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D2;struct Cyc_Absyn_Exp*_tmp4D4;struct Cyc_Absyn_Exp*
_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D8;void*_tmp4DA;void*_tmp4DC;struct _tagged_arr*
_tmp4DE;void*_tmp4E0;void*_tmp4E2;unsigned int _tmp4E4;void*_tmp4E6;struct Cyc_Absyn_Exp*
_tmp4E8;struct _tagged_arr*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4EC;struct _tagged_arr*
_tmp4EE;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*
_tmp4F4;struct Cyc_List_List*_tmp4F6;struct Cyc_List_List*_tmp4F8;struct Cyc_Absyn_Exp*
_tmp4FA;struct Cyc_Absyn_Exp*_tmp4FC;struct Cyc_Absyn_Vardecl*_tmp4FE;struct Cyc_Absyn_Structdecl*
_tmp500;struct Cyc_List_List*_tmp502;struct Cyc_Core_Opt*_tmp504;struct _tuple0*
_tmp506;struct Cyc_List_List*_tmp508;void*_tmp50A;struct Cyc_Absyn_Tunionfield*
_tmp50C;struct Cyc_Absyn_Tuniondecl*_tmp50E;struct Cyc_List_List*_tmp510;struct Cyc_Absyn_Tunionfield*
_tmp512;struct Cyc_Absyn_Tuniondecl*_tmp514;struct Cyc_List_List*_tmp516;struct Cyc_Absyn_MallocInfo
_tmp518;int _tmp51A;struct Cyc_Absyn_Exp*_tmp51C;void**_tmp51E;struct Cyc_Absyn_Exp*
_tmp520;int _tmp522;struct Cyc_Absyn_Stmt*_tmp524;struct Cyc_Absyn_Fndecl*_tmp526;
_LL1082: if(*((int*)_tmp438)== Cyc_Absyn_Const_e){_LL1167: _tmp48E=(void*)((struct
Cyc_Absyn_Const_e_struct*)_tmp438)->f1;if(_tmp48E == (void*)Cyc_Absyn_Null_c){
goto _LL1083;}else{goto _LL1084;}}else{goto _LL1084;}_LL1084: if(*((int*)_tmp438)== 
Cyc_Absyn_Const_e){goto _LL1085;}else{goto _LL1086;}_LL1086: if(*((int*)_tmp438)== 
Cyc_Absyn_Var_e){_LL1171: _tmp492=((struct Cyc_Absyn_Var_e_struct*)_tmp438)->f1;
goto _LL1169;_LL1169: _tmp490=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp438)->f2;
goto _LL1087;}else{goto _LL1088;}_LL1088: if(*((int*)_tmp438)== Cyc_Absyn_UnknownId_e){
_LL1173: _tmp494=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp438)->f1;goto _LL1089;}
else{goto _LL1090;}_LL1090: if(*((int*)_tmp438)== Cyc_Absyn_Primop_e){_LL1177:
_tmp498=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp438)->f1;goto _LL1175;
_LL1175: _tmp496=((struct Cyc_Absyn_Primop_e_struct*)_tmp438)->f2;goto _LL1091;}
else{goto _LL1092;}_LL1092: if(*((int*)_tmp438)== Cyc_Absyn_Increment_e){_LL1181:
_tmp49C=((struct Cyc_Absyn_Increment_e_struct*)_tmp438)->f1;goto _LL1179;_LL1179:
_tmp49A=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp438)->f2;goto _LL1093;}
else{goto _LL1094;}_LL1094: if(*((int*)_tmp438)== Cyc_Absyn_AssignOp_e){_LL1187:
_tmp4A2=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp438)->f1;goto _LL1185;_LL1185:
_tmp4A0=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp438)->f2;goto _LL1183;_LL1183:
_tmp49E=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp438)->f3;goto _LL1095;}else{goto
_LL1096;}_LL1096: if(*((int*)_tmp438)== Cyc_Absyn_Conditional_e){_LL1193: _tmp4A8=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp438)->f1;goto _LL1191;_LL1191: _tmp4A6=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp438)->f2;goto _LL1189;_LL1189: _tmp4A4=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp438)->f3;goto _LL1097;}else{goto _LL1098;}
_LL1098: if(*((int*)_tmp438)== Cyc_Absyn_SeqExp_e){_LL1197: _tmp4AC=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp438)->f1;goto _LL1195;_LL1195: _tmp4AA=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp438)->f2;goto _LL1099;}else{goto _LL1100;}_LL1100: if(*((int*)_tmp438)== Cyc_Absyn_UnknownCall_e){
_LL1201: _tmp4B0=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp438)->f1;goto _LL1199;
_LL1199: _tmp4AE=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp438)->f2;goto _LL1101;}
else{goto _LL1102;}_LL1102: if(*((int*)_tmp438)== Cyc_Absyn_FnCall_e){_LL1207:
_tmp4B6=((struct Cyc_Absyn_FnCall_e_struct*)_tmp438)->f1;goto _LL1205;_LL1205:
_tmp4B4=((struct Cyc_Absyn_FnCall_e_struct*)_tmp438)->f2;goto _LL1203;_LL1203:
_tmp4B2=((struct Cyc_Absyn_FnCall_e_struct*)_tmp438)->f3;if(_tmp4B2 == 0){goto
_LL1103;}else{goto _LL1104;}}else{goto _LL1104;}_LL1104: if(*((int*)_tmp438)== Cyc_Absyn_FnCall_e){
_LL1220: _tmp4C3=((struct Cyc_Absyn_FnCall_e_struct*)_tmp438)->f1;goto _LL1218;
_LL1218: _tmp4C1=((struct Cyc_Absyn_FnCall_e_struct*)_tmp438)->f2;goto _LL1209;
_LL1209: _tmp4B8=((struct Cyc_Absyn_FnCall_e_struct*)_tmp438)->f3;if(_tmp4B8 == 0){
goto _LL1106;}else{_tmp4BA=*_tmp4B8;_LL1216: _tmp4BF=_tmp4BA.num_varargs;goto
_LL1214;_LL1214: _tmp4BD=_tmp4BA.injectors;goto _LL1212;_LL1212: _tmp4BB=_tmp4BA.vai;
goto _LL1105;}}else{goto _LL1106;}_LL1106: if(*((int*)_tmp438)== Cyc_Absyn_Throw_e){
_LL1222: _tmp4C5=((struct Cyc_Absyn_Throw_e_struct*)_tmp438)->f1;goto _LL1107;}
else{goto _LL1108;}_LL1108: if(*((int*)_tmp438)== Cyc_Absyn_NoInstantiate_e){
_LL1224: _tmp4C7=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp438)->f1;goto
_LL1109;}else{goto _LL1110;}_LL1110: if(*((int*)_tmp438)== Cyc_Absyn_Instantiate_e){
_LL1228: _tmp4CB=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp438)->f1;goto _LL1226;
_LL1226: _tmp4C9=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp438)->f2;goto _LL1111;}
else{goto _LL1112;}_LL1112: if(*((int*)_tmp438)== Cyc_Absyn_Cast_e){_LL1232:
_tmp4CF=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp438)->f1;_tmp4D1=(void**)&((
void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp438)->f1);goto _LL1230;_LL1230: _tmp4CD=((
struct Cyc_Absyn_Cast_e_struct*)_tmp438)->f2;goto _LL1113;}else{goto _LL1114;}
_LL1114: if(*((int*)_tmp438)== Cyc_Absyn_Address_e){_LL1235: _tmp4D2=((struct Cyc_Absyn_Address_e_struct*)
_tmp438)->f1;goto _LL1115;}else{goto _LL1116;}_LL1116: if(*((int*)_tmp438)== Cyc_Absyn_New_e){
_LL1239: _tmp4D6=((struct Cyc_Absyn_New_e_struct*)_tmp438)->f1;goto _LL1237;_LL1237:
_tmp4D4=((struct Cyc_Absyn_New_e_struct*)_tmp438)->f2;goto _LL1117;}else{goto
_LL1118;}_LL1118: if(*((int*)_tmp438)== Cyc_Absyn_Sizeofexp_e){_LL1241: _tmp4D8=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp438)->f1;goto _LL1119;}else{goto _LL1120;}
_LL1120: if(*((int*)_tmp438)== Cyc_Absyn_Sizeoftyp_e){_LL1243: _tmp4DA=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp438)->f1;goto _LL1121;}else{goto _LL1122;}
_LL1122: if(*((int*)_tmp438)== Cyc_Absyn_Gentyp_e){goto _LL1123;}else{goto _LL1124;}
_LL1124: if(*((int*)_tmp438)== Cyc_Absyn_Offsetof_e){_LL1249: _tmp4E0=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp438)->f1;goto _LL1245;_LL1245: _tmp4DC=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp438)->f2;if(*((int*)_tmp4DC)== 
Cyc_Absyn_StructField){_LL1247: _tmp4DE=((struct Cyc_Absyn_StructField_struct*)
_tmp4DC)->f1;goto _LL1125;}else{goto _LL1126;}}else{goto _LL1126;}_LL1126: if(*((int*)
_tmp438)== Cyc_Absyn_Offsetof_e){_LL1255: _tmp4E6=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp438)->f1;goto _LL1251;_LL1251: _tmp4E2=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp438)->f2;if(*((int*)_tmp4E2)== Cyc_Absyn_TupleIndex){_LL1253: _tmp4E4=((
struct Cyc_Absyn_TupleIndex_struct*)_tmp4E2)->f1;goto _LL1127;}else{goto _LL1128;}}
else{goto _LL1128;}_LL1128: if(*((int*)_tmp438)== Cyc_Absyn_Deref_e){_LL1257:
_tmp4E8=((struct Cyc_Absyn_Deref_e_struct*)_tmp438)->f1;goto _LL1129;}else{goto
_LL1130;}_LL1130: if(*((int*)_tmp438)== Cyc_Absyn_StructMember_e){_LL1261: _tmp4EC=((
struct Cyc_Absyn_StructMember_e_struct*)_tmp438)->f1;goto _LL1259;_LL1259: _tmp4EA=((
struct Cyc_Absyn_StructMember_e_struct*)_tmp438)->f2;goto _LL1131;}else{goto
_LL1132;}_LL1132: if(*((int*)_tmp438)== Cyc_Absyn_StructArrow_e){_LL1265: _tmp4F0=((
struct Cyc_Absyn_StructArrow_e_struct*)_tmp438)->f1;goto _LL1263;_LL1263: _tmp4EE=((
struct Cyc_Absyn_StructArrow_e_struct*)_tmp438)->f2;goto _LL1133;}else{goto _LL1134;}
_LL1134: if(*((int*)_tmp438)== Cyc_Absyn_Subscript_e){_LL1269: _tmp4F4=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp438)->f1;goto _LL1267;_LL1267: _tmp4F2=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp438)->f2;goto _LL1135;}else{goto _LL1136;}_LL1136: if(*((int*)_tmp438)== Cyc_Absyn_Tuple_e){
_LL1271: _tmp4F6=((struct Cyc_Absyn_Tuple_e_struct*)_tmp438)->f1;goto _LL1137;}
else{goto _LL1138;}_LL1138: if(*((int*)_tmp438)== Cyc_Absyn_Array_e){_LL1273:
_tmp4F8=((struct Cyc_Absyn_Array_e_struct*)_tmp438)->f1;goto _LL1139;}else{goto
_LL1140;}_LL1140: if(*((int*)_tmp438)== Cyc_Absyn_Comprehension_e){_LL1279:
_tmp4FE=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp438)->f1;goto _LL1277;
_LL1277: _tmp4FC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp438)->f2;goto
_LL1275;_LL1275: _tmp4FA=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp438)->f3;
goto _LL1141;}else{goto _LL1142;}_LL1142: if(*((int*)_tmp438)== Cyc_Absyn_Struct_e){
_LL1287: _tmp506=((struct Cyc_Absyn_Struct_e_struct*)_tmp438)->f1;goto _LL1285;
_LL1285: _tmp504=((struct Cyc_Absyn_Struct_e_struct*)_tmp438)->f2;goto _LL1283;
_LL1283: _tmp502=((struct Cyc_Absyn_Struct_e_struct*)_tmp438)->f3;goto _LL1281;
_LL1281: _tmp500=((struct Cyc_Absyn_Struct_e_struct*)_tmp438)->f4;goto _LL1143;}
else{goto _LL1144;}_LL1144: if(*((int*)_tmp438)== Cyc_Absyn_AnonStruct_e){_LL1291:
_tmp50A=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp438)->f1;goto _LL1289;
_LL1289: _tmp508=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp438)->f2;goto _LL1145;}
else{goto _LL1146;}_LL1146: if(*((int*)_tmp438)== Cyc_Absyn_Tunion_e){_LL1297:
_tmp510=((struct Cyc_Absyn_Tunion_e_struct*)_tmp438)->f3;if(_tmp510 == 0){goto
_LL1295;}else{goto _LL1148;}_LL1295: _tmp50E=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp438)->f4;goto _LL1293;_LL1293: _tmp50C=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp438)->f5;goto _LL1147;}else{goto _LL1148;}_LL1148: if(*((int*)_tmp438)== Cyc_Absyn_Tunion_e){
_LL1303: _tmp516=((struct Cyc_Absyn_Tunion_e_struct*)_tmp438)->f3;goto _LL1301;
_LL1301: _tmp514=((struct Cyc_Absyn_Tunion_e_struct*)_tmp438)->f4;goto _LL1299;
_LL1299: _tmp512=((struct Cyc_Absyn_Tunion_e_struct*)_tmp438)->f5;goto _LL1149;}
else{goto _LL1150;}_LL1150: if(*((int*)_tmp438)== Cyc_Absyn_Enum_e){goto _LL1151;}
else{goto _LL1152;}_LL1152: if(*((int*)_tmp438)== Cyc_Absyn_AnonEnum_e){goto
_LL1153;}else{goto _LL1154;}_LL1154: if(*((int*)_tmp438)== Cyc_Absyn_Malloc_e){
_LL1305: _tmp518=((struct Cyc_Absyn_Malloc_e_struct*)_tmp438)->f1;_LL1315: _tmp522=
_tmp518.is_calloc;goto _LL1313;_LL1313: _tmp520=_tmp518.rgn;goto _LL1311;_LL1311:
_tmp51E=_tmp518.elt_type;goto _LL1309;_LL1309: _tmp51C=_tmp518.num_elts;goto
_LL1307;_LL1307: _tmp51A=_tmp518.fat_result;goto _LL1155;}else{goto _LL1156;}
_LL1156: if(*((int*)_tmp438)== Cyc_Absyn_StmtExp_e){_LL1317: _tmp524=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp438)->f1;goto _LL1157;}else{goto _LL1158;}_LL1158: if(*((int*)_tmp438)== Cyc_Absyn_UnresolvedMem_e){
goto _LL1159;}else{goto _LL1160;}_LL1160: if(*((int*)_tmp438)== Cyc_Absyn_CompoundLit_e){
goto _LL1161;}else{goto _LL1162;}_LL1162: if(*((int*)_tmp438)== Cyc_Absyn_Codegen_e){
_LL1319: _tmp526=((struct Cyc_Absyn_Codegen_e_struct*)_tmp438)->f1;goto _LL1163;}
else{goto _LL1164;}_LL1164: if(*((int*)_tmp438)== Cyc_Absyn_Fill_e){goto _LL1165;}
else{goto _LL1081;}_LL1083: {struct Cyc_Absyn_Exp*_tmp528=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){if(nv->toplevel){(void*)(e->r=(
void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_typ,_tmp528,_tmp528))->r));}
else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp529[3];_tmp529[2]=_tmp528;_tmp529[1]=_tmp528;_tmp529[0]=
_tmp528;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp529,sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}else{(void*)(e->r=(void*)((
void*)(Cyc_Absyn_signed_int_exp(0,0))->r));}goto _LL1081;}_LL1085: goto _LL1081;
_LL1087:{struct _handler_cons _tmp52A;_push_handler(& _tmp52A);{int _tmp52C=0;if(
setjmp(_tmp52A.handler)){_tmp52C=1;}if(! _tmp52C){(void*)(e->r=(void*)((void*)(((
struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup)(
nv->varmap,_tmp492))->r));;_pop_handler();}else{void*_tmp52B=(void*)_exn_thrown;
void*_tmp52E=_tmp52B;_LL1328: if(_tmp52E == Cyc_Dict_Absent){goto _LL1329;}else{
goto _LL1330;}_LL1330: goto _LL1331;_LL1329:({struct Cyc_Std_String_pa_struct _tmp535;
_tmp535.tag=Cyc_Std_String_pa;_tmp535.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp492);{void*_tmp534[1]={& _tmp535};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Can't find %s in exp_to_c, Var\n",
sizeof(unsigned char),32),_tag_arr(_tmp534,sizeof(void*),1));}});return;_LL1331:(
void)_throw(_tmp52E);_LL1327:;}}}goto _LL1081;_LL1089:({void*_tmp536[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unknownid",
sizeof(unsigned char),10),_tag_arr(_tmp536,sizeof(void*),0));});goto _LL1081;
_LL1091: {struct Cyc_List_List*_tmp537=((struct Cyc_List_List*(*)(void*(*f)(struct
Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp496);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp496);{void*
_tmp538=_tmp498;_LL1338: if(_tmp538 == (void*)Cyc_Absyn_Size){goto _LL1339;}else{
goto _LL1340;}_LL1340: if(_tmp538 == (void*)Cyc_Absyn_Plus){goto _LL1341;}else{goto
_LL1342;}_LL1342: if(_tmp538 == (void*)Cyc_Absyn_Minus){goto _LL1343;}else{goto
_LL1344;}_LL1344: if(_tmp538 == (void*)Cyc_Absyn_Eq){goto _LL1345;}else{goto _LL1346;}
_LL1346: if(_tmp538 == (void*)Cyc_Absyn_Neq){goto _LL1347;}else{goto _LL1348;}
_LL1348: if(_tmp538 == (void*)Cyc_Absyn_Gt){goto _LL1349;}else{goto _LL1350;}_LL1350:
if(_tmp538 == (void*)Cyc_Absyn_Gte){goto _LL1351;}else{goto _LL1352;}_LL1352: if(
_tmp538 == (void*)Cyc_Absyn_Lt){goto _LL1353;}else{goto _LL1354;}_LL1354: if(_tmp538
== (void*)Cyc_Absyn_Lte){goto _LL1355;}else{goto _LL1356;}_LL1356: goto _LL1357;
_LL1339: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp496))->hd;{void*_tmp54E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v);struct Cyc_Absyn_Exp*_tmp556;struct Cyc_Absyn_PtrInfo
_tmp558;struct Cyc_Absyn_Conref*_tmp55A;struct Cyc_Absyn_Conref*_tmp55C;void*
_tmp55E;_LL1360: if((unsigned int)_tmp54E > 3?*((int*)_tmp54E)== Cyc_Absyn_ArrayType:
0){_LL1367: _tmp556=((struct Cyc_Absyn_ArrayType_struct*)_tmp54E)->f3;goto _LL1361;}
else{goto _LL1362;}_LL1362: if((unsigned int)_tmp54E > 3?*((int*)_tmp54E)== Cyc_Absyn_PointerType:
0){_LL1369: _tmp558=((struct Cyc_Absyn_PointerType_struct*)_tmp54E)->f1;_LL1375:
_tmp55E=(void*)_tmp558.elt_typ;goto _LL1373;_LL1373: _tmp55C=_tmp558.nullable;goto
_LL1371;_LL1371: _tmp55A=_tmp558.bounds;goto _LL1363;}else{goto _LL1364;}_LL1364:
goto _LL1365;_LL1361:(void*)(e->r=(void*)((void*)((struct Cyc_Absyn_Exp*)
_check_null(_tmp556))->r));goto _LL1359;_LL1363:{void*_tmp560=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp55A);struct Cyc_Absyn_Exp*_tmp566;_LL1378: if(_tmp560 == (
void*)Cyc_Absyn_Unknown_b){goto _LL1379;}else{goto _LL1380;}_LL1380: if((
unsigned int)_tmp560 > 1?*((int*)_tmp560)== Cyc_Absyn_Upper_b: 0){_LL1383: _tmp566=((
struct Cyc_Absyn_Upper_b_struct*)_tmp560)->f1;goto _LL1381;}else{goto _LL1377;}
_LL1379:(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__get_arr_size_e,({
struct Cyc_Absyn_Exp*_tmp568[2];_tmp568[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp55E),0);_tmp568[0]=(struct Cyc_Absyn_Exp*)_tmp496->hd;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp568,sizeof(struct Cyc_Absyn_Exp*),
2));}),0))->r));goto _LL1377;_LL1381: if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp55C)){(void*)(e->r=(void*)((void*)(Cyc_Absyn_conditional_exp(
arg,_tmp566,Cyc_Absyn_uint_exp(0,0),0))->r));}else{(void*)(e->r=(void*)((void*)
_tmp566->r));goto _LL1377;}_LL1377:;}goto _LL1359;_LL1365:({struct Cyc_Std_String_pa_struct
_tmp56B;_tmp56B.tag=Cyc_Std_String_pa;_tmp56B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp56A;_tmp56A.tag=Cyc_Std_String_pa;_tmp56A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);{void*_tmp569[2]={&
_tmp56A,& _tmp56B};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("size primop applied to non-array %s (%s)",sizeof(unsigned char),41),
_tag_arr(_tmp569,sizeof(void*),2));}}});return;_LL1359:;}goto _LL1337;}_LL1341: {
void*elt_typ=(void*)Cyc_Absyn_VoidType;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp537))->hd,& elt_typ)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp496))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp496->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({
struct Cyc_Absyn_Exp*_tmp56C[3];_tmp56C[2]=e2;_tmp56C[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp56C[0]=e1;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp56C,sizeof(struct Cyc_Absyn_Exp*),3));}),
0))->r));}goto _LL1337;}_LL1343: {void*elt_typ=(void*)Cyc_Absyn_VoidType;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp537))->hd,& elt_typ)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp496))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp496->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)
_check_null(_tmp537->tl))->hd)){(void*)(e1->r=(void*)((void*)(Cyc_Absyn_structmember_exp(
Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));(void*)(e2->r=(void*)((
void*)(Cyc_Absyn_structmember_exp(Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp,
0))->r));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}else{(void*)(e->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_plus_e,({struct Cyc_Absyn_Exp*
_tmp56D[3];_tmp56D[2]=Cyc_Absyn_prim1_exp((void*)Cyc_Absyn_Minus,e2,0);_tmp56D[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp56D[0]=e1;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp56D,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}}goto _LL1337;}_LL1345: goto _LL1347;_LL1347: goto _LL1349;_LL1349:
goto _LL1351;_LL1351: goto _LL1353;_LL1353: goto _LL1355;_LL1355: {struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp496))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp496->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp537))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp537->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(
t1)){(void*)(e1->r=(void*)((void*)(Cyc_Absyn_structmember_exp(Cyc_Absyn_new_exp((
void*)e1->r,0),Cyc_Toc_curr_sp,0))->r));}if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){(
void*)(e2->r=(void*)((void*)(Cyc_Absyn_structmember_exp(Cyc_Absyn_new_exp((void*)
e2->r,0),Cyc_Toc_curr_sp,0))->r));}goto _LL1337;}_LL1357: goto _LL1337;_LL1337:;}
goto _LL1081;}_LL1093: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp49C->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp49C);{void*elt_typ=(void*)Cyc_Absyn_VoidType;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
_tmp56E=(_tmp49A == (void*)Cyc_Absyn_PostInc? 1: _tmp49A == (void*)Cyc_Absyn_PostDec)?
Cyc_Toc__tagged_arr_inplace_plus_post_e: Cyc_Toc__tagged_arr_inplace_plus_e;int
_tmp56F=(_tmp49A == (void*)Cyc_Absyn_PreInc? 1: _tmp49A == (void*)Cyc_Absyn_PostInc)?
1: - 1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(_tmp56E,({struct Cyc_Absyn_Exp*
_tmp570[3];_tmp570[2]=Cyc_Absyn_signed_int_exp(_tmp56F,0);_tmp570[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp570[0]=Cyc_Absyn_address_exp(_tmp49C,0);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp570,
sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}if(elt_typ == (void*)Cyc_Absyn_VoidType?
! Cyc_Absyn_is_lvalue(_tmp49C): 0){Cyc_Toc_lvalue_assign(_tmp49C,0,Cyc_Toc_incr_lvalue,
_tmp49A);(void*)(e->r=(void*)((void*)_tmp49C->r));}goto _LL1081;}}_LL1095: {int
e1_poly=Cyc_Toc_is_poly_project(_tmp4A2);void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4A2->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp49E->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp4A2);Cyc_Toc_exp_to_c(nv,
_tmp49E);{int done=0;if(_tmp4A0 != 0){void*elt_typ=(void*)Cyc_Absyn_VoidType;if(
Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;{void*_tmp571=(void*)_tmp4A0->v;_LL1395: if(_tmp571 == (void*)Cyc_Absyn_Plus){
goto _LL1396;}else{goto _LL1397;}_LL1397: if(_tmp571 == (void*)Cyc_Absyn_Minus){goto
_LL1398;}else{goto _LL1399;}_LL1399: goto _LL1400;_LL1396: change=_tmp49E;goto
_LL1394;_LL1398: change=Cyc_Absyn_prim1_exp((void*)Cyc_Absyn_Minus,_tmp49E,0);
goto _LL1394;_LL1400:(int)_throw(({void*_tmp579[0]={};Cyc_Tcutil_impos(_tag_arr("bad t ? pointer arithmetic",
sizeof(unsigned char),27),_tag_arr(_tmp579,sizeof(void*),0));}));goto _LL1394;
_LL1394:;}done=1;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tagged_arr_inplace_plus_e,({
struct Cyc_Absyn_Exp*_tmp57A[3];_tmp57A[2]=change;_tmp57A[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp57A[0]=Cyc_Absyn_address_exp(_tmp4A2,0);((
struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp57A,
sizeof(struct Cyc_Absyn_Exp*),3));}),0))->r));}}if(! done){if(e1_poly){(void*)(
_tmp49E->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((
void*)_tmp49E->r,0),0))->r));}if(! Cyc_Absyn_is_lvalue(_tmp4A2)){((void(*)(struct
Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
struct _tuple7*),struct _tuple7*f_env))Cyc_Toc_lvalue_assign)(_tmp4A2,0,Cyc_Toc_assignop_lvalue,({
struct _tuple7*_tmp57B=_cycalloc(sizeof(struct _tuple7)* 1);_tmp57B[0]=({struct
_tuple7 _tmp57C;_tmp57C.f1=_tmp4A0;_tmp57C.f2=_tmp49E;_tmp57C;});_tmp57B;}));(
void*)(e->r=(void*)((void*)_tmp4A2->r));}}goto _LL1081;}}_LL1097: Cyc_Toc_exp_to_c(
nv,_tmp4A8);Cyc_Toc_exp_to_c(nv,_tmp4A6);Cyc_Toc_exp_to_c(nv,_tmp4A4);goto
_LL1081;_LL1099: Cyc_Toc_exp_to_c(nv,_tmp4AC);Cyc_Toc_exp_to_c(nv,_tmp4AA);goto
_LL1081;_LL1101: _tmp4B6=_tmp4B0;_tmp4B4=_tmp4AE;goto _LL1103;_LL1103: Cyc_Toc_exp_to_c(
nv,_tmp4B6);((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct
Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,
_tmp4B4);goto _LL1081;_LL1105:{struct _RegionHandle _tmp57D=_new_region("r");struct
_RegionHandle*r=& _tmp57D;_push_region(r);{struct _tuple0*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*
num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4BF,0);void*cva_type=Cyc_Toc_typ_to_c((
void*)_tmp4BB->type);void*arr_type=(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp58E=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp58E[0]=({struct
Cyc_Absyn_ArrayType_struct _tmp58F;_tmp58F.tag=Cyc_Absyn_ArrayType;_tmp58F.f1=(
void*)cva_type;_tmp58F.f2=Cyc_Toc_mt_tq;_tmp58F.f3=(struct Cyc_Absyn_Exp*)
num_varargs_exp;_tmp58F;});_tmp58E;});int num_args=((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp4C1);int num_normargs=num_args - _tmp4BF;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp4C1=_tmp4C1->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4C1))->hd);
new_args=({struct Cyc_List_List*_tmp57E=_cycalloc(sizeof(struct Cyc_List_List));
_tmp57E->hd=(struct Cyc_Absyn_Exp*)_tmp4C1->hd;_tmp57E->tl=new_args;_tmp57E;});}}
new_args=({struct Cyc_List_List*_tmp57F=_cycalloc(sizeof(struct Cyc_List_List));
_tmp57F->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*
_tmp580[3];_tmp580[2]=num_varargs_exp;_tmp580[1]=Cyc_Absyn_sizeoftyp_exp(
cva_type,0);_tmp580[0]=argvexp;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp580,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp57F->tl=new_args;
_tmp57F;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
new_args);Cyc_Toc_exp_to_c(nv,_tmp4C3);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(_tmp4C3,new_args,0),0);if(_tmp4BB->inject){struct
_tagged_arr vs=({unsigned int _tmp58A=(unsigned int)_tmp4BF;struct _tuple0**_tmp58B=(
struct _tuple0**)_region_malloc(r,_check_times(sizeof(struct _tuple0*),_tmp58A));
struct _tagged_arr _tmp58D=_tag_arr(_tmp58B,sizeof(struct _tuple0*),(unsigned int)
_tmp4BF);{unsigned int _tmp58C=_tmp58A;unsigned int i;for(i=0;i < _tmp58C;i ++){
_tmp58B[i]=Cyc_Toc_temp_var();}};_tmp58D;});struct Cyc_List_List*_tmp581=0;{int i=
_tmp4BF - 1;for(0;i >= 0;-- i){_tmp581=({struct Cyc_List_List*_tmp582=_cycalloc(
sizeof(struct Cyc_List_List));_tmp582->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(((struct _tuple0**)vs.curr)[i],0),0));_tmp582->tl=_tmp581;
_tmp582;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(
0,_tmp581,0),s,0);{int i=0;for(0;_tmp4C1 != 0;(((_tmp4C1=_tmp4C1->tl,_tmp4BD=
_tmp4BD->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4C1->hd;
void*arg_type=(void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct
_tuple0*var=((struct _tuple0**)vs.curr)[i];struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(
var,0);struct Cyc_Absyn_Tunionfield _tmp585;struct Cyc_List_List*_tmp586;struct
_tuple0*_tmp588;struct Cyc_Absyn_Tunionfield*_tmp583=(struct Cyc_Absyn_Tunionfield*)((
struct Cyc_List_List*)_check_null(_tmp4BD))->hd;_tmp585=*_tmp583;_LL1417: _tmp588=
_tmp585.name;goto _LL1415;_LL1415: _tmp586=_tmp585.typs;goto _LL1412;_LL1412: {void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple3*)((struct Cyc_List_List*)_check_null(
_tmp586))->hd)).f2);Cyc_Toc_exp_to_c(nv,arg);if(Cyc_Toc_is_void_star(field_typ)){
arg=Cyc_Absyn_cast_exp(field_typ,arg,0);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_structmember_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_structmember_exp(varexp,Cyc_Toc_tag_sp,0),Cyc_Absyn_var_exp(
_tmp588,0),0),s,0);s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp588,_tag_arr("_struct",sizeof(unsigned char),8))),0,s,0);}}}}else{{int i=0;
for(0;_tmp4C1 != 0;(_tmp4C1=_tmp4C1->tl,++ i)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp4C1->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp4C1->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(r);}goto _LL1081;_LL1107: Cyc_Toc_exp_to_c(
nv,_tmp4C5);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(
old_typ),Cyc_Toc_newthrow_exp(_tmp4C5),0))->r));goto _LL1081;_LL1109: Cyc_Toc_exp_to_c(
nv,_tmp4C7);goto _LL1081;_LL1111: Cyc_Toc_exp_to_c(nv,_tmp4CB);for(0;_tmp4C9 != 0;
_tmp4C9=_tmp4C9->tl){void*k=Cyc_Tcutil_typ_kind((void*)_tmp4C9->hd);if(k != (void*)
Cyc_Absyn_EffKind? k != (void*)Cyc_Absyn_RgnKind: 0){{void*_tmp590=Cyc_Tcutil_compress((
void*)_tmp4C9->hd);_LL1426: if((unsigned int)_tmp590 > 3?*((int*)_tmp590)== Cyc_Absyn_VarType:
0){goto _LL1427;}else{goto _LL1428;}_LL1428: if((unsigned int)_tmp590 > 3?*((int*)
_tmp590)== Cyc_Absyn_TunionType: 0){goto _LL1429;}else{goto _LL1430;}_LL1430: goto
_LL1431;_LL1427: goto _LL1429;_LL1429: continue;_LL1431:(void*)(e->r=(void*)((void*)(
Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),_tmp4CB,0))->r));goto _LL1425;_LL1425:;}break;}}goto
_LL1081;_LL1113: {void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4CD->topt))->v;
void*new_typ=*_tmp4D1;*_tmp4D1=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(nv,
_tmp4CD);{struct _tuple8 _tmp599=({struct _tuple8 _tmp598;_tmp598.f1=Cyc_Tcutil_compress(
old_t2);_tmp598.f2=Cyc_Tcutil_compress(new_typ);_tmp598;});void*_tmp5A1;struct
Cyc_Absyn_PtrInfo _tmp5A3;void*_tmp5A5;struct Cyc_Absyn_PtrInfo _tmp5A7;void*
_tmp5A9;void*_tmp5AB;struct Cyc_Absyn_PtrInfo _tmp5AD;_LL1435: _LL1446: _tmp5A5=
_tmp599.f1;if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)== Cyc_Absyn_PointerType:
0){_LL1448: _tmp5A7=((struct Cyc_Absyn_PointerType_struct*)_tmp5A5)->f1;goto
_LL1442;}else{goto _LL1437;}_LL1442: _tmp5A1=_tmp599.f2;if((unsigned int)_tmp5A1 > 
3?*((int*)_tmp5A1)== Cyc_Absyn_PointerType: 0){_LL1444: _tmp5A3=((struct Cyc_Absyn_PointerType_struct*)
_tmp5A1)->f1;goto _LL1436;}else{goto _LL1437;}_LL1437: _LL1452: _tmp5AB=_tmp599.f1;
if((unsigned int)_tmp5AB > 3?*((int*)_tmp5AB)== Cyc_Absyn_PointerType: 0){_LL1454:
_tmp5AD=((struct Cyc_Absyn_PointerType_struct*)_tmp5AB)->f1;goto _LL1450;}else{
goto _LL1439;}_LL1450: _tmp5A9=_tmp599.f2;if((unsigned int)_tmp5A9 > 3?*((int*)
_tmp5A9)== Cyc_Absyn_IntType: 0){goto _LL1438;}else{goto _LL1439;}_LL1439: goto
_LL1440;_LL1436: {int _tmp5AF=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp5A7.nullable);int _tmp5B0=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp5A3.nullable);void*_tmp5B1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp5A7.bounds);void*_tmp5B2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp5A3.bounds);{
struct _tuple8 _tmp5B4=({struct _tuple8 _tmp5B3;_tmp5B3.f1=_tmp5B1;_tmp5B3.f2=
_tmp5B2;_tmp5B3;});void*_tmp5BE;void*_tmp5C0;void*_tmp5C2;void*_tmp5C4;struct Cyc_Absyn_Exp*
_tmp5C6;void*_tmp5C8;struct Cyc_Absyn_Exp*_tmp5CA;void*_tmp5CC;_LL1462: _LL1473:
_tmp5C0=_tmp5B4.f1;if((unsigned int)_tmp5C0 > 1?*((int*)_tmp5C0)== Cyc_Absyn_Upper_b:
0){goto _LL1471;}else{goto _LL1464;}_LL1471: _tmp5BE=_tmp5B4.f2;if((unsigned int)
_tmp5BE > 1?*((int*)_tmp5BE)== Cyc_Absyn_Upper_b: 0){goto _LL1463;}else{goto _LL1464;}
_LL1464: _LL1477: _tmp5C4=_tmp5B4.f1;if((unsigned int)_tmp5C4 > 1?*((int*)_tmp5C4)
== Cyc_Absyn_Upper_b: 0){_LL1479: _tmp5C6=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5C4)->f1;goto _LL1475;}else{goto _LL1466;}_LL1475: _tmp5C2=_tmp5B4.f2;if(
_tmp5C2 == (void*)Cyc_Absyn_Unknown_b){goto _LL1465;}else{goto _LL1466;}_LL1466:
_LL1485: _tmp5CC=_tmp5B4.f1;if(_tmp5CC == (void*)Cyc_Absyn_Unknown_b){goto _LL1481;}
else{goto _LL1468;}_LL1481: _tmp5C8=_tmp5B4.f2;if((unsigned int)_tmp5C8 > 1?*((int*)
_tmp5C8)== Cyc_Absyn_Upper_b: 0){_LL1483: _tmp5CA=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5C8)->f1;goto _LL1467;}else{goto _LL1468;}_LL1468: goto _LL1469;_LL1463: if(
_tmp5AF? ! _tmp5B0: 0){if(nv->toplevel){({void*_tmp5CE[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("can't do NULL-check conversion at top-level",
sizeof(unsigned char),44),_tag_arr(_tmp5CE,sizeof(void*),0));});}(void*)(e->r=(
void*)((void*)(Cyc_Absyn_cast_exp(*_tmp4D1,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp5CF=_cycalloc(sizeof(struct Cyc_List_List));_tmp5CF->hd=
_tmp4CD;_tmp5CF->tl=0;_tmp5CF;}),0),0))->r));}goto _LL1461;_LL1465: if(nv->toplevel){(
void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_tagged_arr(old_t2,_tmp5C6,
_tmp4CD))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp5D0[3];_tmp5D0[2]=_tmp5C6;_tmp5D0[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp5A3.elt_typ),0);_tmp5D0[0]=_tmp4CD;((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp5D0,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}goto _LL1461;_LL1467: if(nv->toplevel){({void*_tmp5D1[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("can't coerce t? to t* or t@ at the top-level",
sizeof(unsigned char),45),_tag_arr(_tmp5D1,sizeof(void*),0));});}{struct Cyc_Absyn_Exp*
_tmp5D2=Cyc_Absyn_fncall_exp(Cyc_Toc__untag_arr_e,({struct Cyc_Absyn_Exp*_tmp5D4[
3];_tmp5D4[2]=_tmp5CA;_tmp5D4[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)
_tmp5A7.elt_typ),0);_tmp5D4[0]=_tmp4CD;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp5D4,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);if(_tmp5B0){(void*)(_tmp5D2->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp5D3=_cycalloc(sizeof(struct Cyc_List_List));_tmp5D3->hd=
Cyc_Absyn_copy_exp(_tmp5D2);_tmp5D3->tl=0;_tmp5D3;}),0))->r));}(void*)(e->r=(
void*)((void*)(Cyc_Absyn_cast_exp(*_tmp4D1,_tmp5D2,0))->r));goto _LL1461;}_LL1469:
goto _LL1461;_LL1461:;}goto _LL1434;}_LL1438:{void*_tmp5D5=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp5AD.bounds);_LL1495: if(_tmp5D5 == (void*)Cyc_Absyn_Unknown_b){
goto _LL1496;}else{goto _LL1497;}_LL1497: goto _LL1498;_LL1496:(void*)(_tmp4CD->r=(
void*)((void*)(Cyc_Absyn_structmember_exp(Cyc_Absyn_new_exp((void*)_tmp4CD->r,
_tmp4CD->loc),Cyc_Toc_curr_sp,0))->r));goto _LL1494;_LL1498: goto _LL1494;_LL1494:;}
goto _LL1434;_LL1440: goto _LL1434;_LL1434:;}goto _LL1081;}_LL1115:{void*_tmp5DB=(
void*)_tmp4D2->r;struct Cyc_List_List*_tmp5E3;struct _tuple0*_tmp5E5;struct Cyc_List_List*
_tmp5E7;_LL1501: if(*((int*)_tmp5DB)== Cyc_Absyn_Struct_e){_LL1510: _tmp5E5=((
struct Cyc_Absyn_Struct_e_struct*)_tmp5DB)->f1;goto _LL1508;_LL1508: _tmp5E3=((
struct Cyc_Absyn_Struct_e_struct*)_tmp5DB)->f3;goto _LL1502;}else{goto _LL1503;}
_LL1503: if(*((int*)_tmp5DB)== Cyc_Absyn_Tuple_e){_LL1512: _tmp5E7=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp5DB)->f1;goto _LL1504;}else{goto _LL1505;}_LL1505: goto _LL1506;_LL1502: if(nv->toplevel){({
struct Cyc_Std_String_pa_struct _tmp5EA;_tmp5EA.tag=Cyc_Std_String_pa;_tmp5EA.f1=(
struct _tagged_arr)Cyc_Position_string_of_segment(_tmp4D2->loc);{void*_tmp5E9[1]={&
_tmp5EA};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("%s: & on non-identifiers at the top-level",sizeof(unsigned char),42),
_tag_arr(_tmp5E9,sizeof(void*),1));}});}(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D2->topt))->v,1,0,_tmp5E3,
_tmp5E5))->r));goto _LL1500;_LL1504: if(nv->toplevel){({struct Cyc_Std_String_pa_struct
_tmp5EC;_tmp5EC.tag=Cyc_Std_String_pa;_tmp5EC.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(
_tmp4D2->loc);{void*_tmp5EB[1]={& _tmp5EC};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("%s: & on non-identifiers at the top-level",
sizeof(unsigned char),42),_tag_arr(_tmp5EB,sizeof(void*),1));}});}(void*)(e->r=(
void*)((void*)(Cyc_Toc_init_tuple(nv,1,0,_tmp5E7))->r));goto _LL1500;_LL1506: Cyc_Toc_exp_to_c(
nv,_tmp4D2);if(! Cyc_Absyn_is_lvalue(_tmp4D2)){((void(*)(struct Cyc_Absyn_Exp*e1,
struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int
f_env))Cyc_Toc_lvalue_assign)(_tmp4D2,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(
void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),_tmp4D2,0))->r));}goto _LL1500;_LL1500:;}goto _LL1081;
_LL1117: if(nv->toplevel){({struct Cyc_Std_String_pa_struct _tmp5EE;_tmp5EE.tag=Cyc_Std_String_pa;
_tmp5EE.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(_tmp4D4->loc);{void*
_tmp5ED[1]={& _tmp5EE};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("%s: new at top-level",sizeof(unsigned char),21),_tag_arr(_tmp5ED,
sizeof(void*),1));}});}{void*_tmp5EF=(void*)_tmp4D4->r;struct Cyc_List_List*
_tmp5FB;struct Cyc_Absyn_Exp*_tmp5FD;struct Cyc_Absyn_Exp*_tmp5FF;struct Cyc_Absyn_Vardecl*
_tmp601;struct Cyc_List_List*_tmp603;struct _tuple0*_tmp605;struct Cyc_List_List*
_tmp607;_LL1521: if(*((int*)_tmp5EF)== Cyc_Absyn_Array_e){_LL1532: _tmp5FB=((
struct Cyc_Absyn_Array_e_struct*)_tmp5EF)->f1;goto _LL1522;}else{goto _LL1523;}
_LL1523: if(*((int*)_tmp5EF)== Cyc_Absyn_Comprehension_e){_LL1538: _tmp601=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp5EF)->f1;goto _LL1536;_LL1536: _tmp5FF=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp5EF)->f2;goto _LL1534;_LL1534: _tmp5FD=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp5EF)->f3;goto _LL1524;}else{goto
_LL1525;}_LL1525: if(*((int*)_tmp5EF)== Cyc_Absyn_Struct_e){_LL1542: _tmp605=((
struct Cyc_Absyn_Struct_e_struct*)_tmp5EF)->f1;goto _LL1540;_LL1540: _tmp603=((
struct Cyc_Absyn_Struct_e_struct*)_tmp5EF)->f3;goto _LL1526;}else{goto _LL1527;}
_LL1527: if(*((int*)_tmp5EF)== Cyc_Absyn_Tuple_e){_LL1544: _tmp607=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp5EF)->f1;goto _LL1528;}else{goto _LL1529;}_LL1529: goto _LL1530;_LL1522: {struct
_tuple0*_tmp609=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp60A=Cyc_Absyn_var_exp(
_tmp609,0);struct Cyc_Absyn_Stmt*_tmp60B=Cyc_Toc_init_array(nv,_tmp60A,_tmp5FB,
Cyc_Absyn_exp_stmt(_tmp60A,0));void*old_elt_typ;{void*_tmp60C=Cyc_Tcutil_compress(
old_typ);struct Cyc_Absyn_PtrInfo _tmp612;struct Cyc_Absyn_Tqual _tmp614;void*
_tmp616;_LL1550: if((unsigned int)_tmp60C > 3?*((int*)_tmp60C)== Cyc_Absyn_PointerType:
0){_LL1555: _tmp612=((struct Cyc_Absyn_PointerType_struct*)_tmp60C)->f1;_LL1559:
_tmp616=(void*)_tmp612.elt_typ;goto _LL1557;_LL1557: _tmp614=_tmp612.tq;goto
_LL1551;}else{goto _LL1552;}_LL1552: goto _LL1553;_LL1551: old_elt_typ=_tmp616;goto
_LL1549;_LL1553: old_elt_typ=({void*_tmp618[0]={};Cyc_Toc_toc_impos(_tag_arr("exp_to_c:new array expression doesn't have ptr type",
sizeof(unsigned char),52),_tag_arr(_tmp618,sizeof(void*),0));});goto _LL1549;
_LL1549:;}{void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp619=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp61A=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp5FB),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp4D6 == 0){e1=Cyc_Toc_malloc_exp(
old_elt_typ,_tmp61A);}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_check_null(_tmp4D6);Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp61A);}(
void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp609,
_tmp619,(struct Cyc_Absyn_Exp*)e1,_tmp60B,0),0))->r));goto _LL1520;}}_LL1524: {int
is_tagged_ptr=0;{void*_tmp61B=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp621;struct Cyc_Absyn_Conref*_tmp623;struct Cyc_Absyn_Tqual _tmp625;void*_tmp627;
_LL1565: if((unsigned int)_tmp61B > 3?*((int*)_tmp61B)== Cyc_Absyn_PointerType: 0){
_LL1570: _tmp621=((struct Cyc_Absyn_PointerType_struct*)_tmp61B)->f1;_LL1576:
_tmp627=(void*)_tmp621.elt_typ;goto _LL1574;_LL1574: _tmp625=_tmp621.tq;goto
_LL1572;_LL1572: _tmp623=_tmp621.bounds;goto _LL1566;}else{goto _LL1567;}_LL1567:
goto _LL1568;_LL1566: is_tagged_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp623)== (void*)Cyc_Absyn_Unknown_b;goto _LL1564;_LL1568:({void*_tmp629[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: comprehension not an array type",
sizeof(unsigned char),42),_tag_arr(_tmp629,sizeof(void*),0));});goto _LL1564;
_LL1564:;}{struct _tuple0*max=Cyc_Toc_temp_var();struct _tuple0*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5FD->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp5FF);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(
nv,Cyc_Absyn_var_exp(a,0),_tmp601,Cyc_Absyn_var_exp(max,0),_tmp5FD,Cyc_Absyn_skip_stmt(
0),1);{struct _RegionHandle _tmp62A=_new_region("r");struct _RegionHandle*r=&
_tmp62A;_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp63E=
_region_malloc(r,sizeof(struct Cyc_List_List));_tmp63E->hd=({struct _tuple9*
_tmp63F=_region_malloc(r,sizeof(struct _tuple9));_tmp63F->f1=max;_tmp63F->f2=Cyc_Absyn_uint_t;
_tmp63F->f3=(struct Cyc_Absyn_Exp*)_tmp5FF;_tmp63F;});_tmp63E->tl=0;_tmp63E;});
struct Cyc_Absyn_Exp*ai;if(_tmp4D6 == 0){ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp62B[2];_tmp62B[1]=Cyc_Absyn_var_exp(
max,0);_tmp62B[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp62B,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));}else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_check_null(_tmp4D6);
Cyc_Toc_exp_to_c(nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({
struct Cyc_Absyn_Exp*_tmp62C[2];_tmp62C[1]=Cyc_Absyn_var_exp(max,0);_tmp62C[0]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct _tagged_arr))
Cyc_List_list)(_tag_arr(_tmp62C,sizeof(struct Cyc_Absyn_Exp*),2));}),0));}{struct
Cyc_Absyn_Exp*ainit=Cyc_Absyn_cast_exp(ptr_typ,ai,0);decls=({struct Cyc_List_List*
_tmp62D=_region_malloc(r,sizeof(struct Cyc_List_List));_tmp62D->hd=({struct
_tuple9*_tmp62E=_region_malloc(r,sizeof(struct _tuple9));_tmp62E->f1=a;_tmp62E->f2=
ptr_typ;_tmp62E->f3=(struct Cyc_Absyn_Exp*)ainit;_tmp62E;});_tmp62D->tl=decls;
_tmp62D;});if(is_tagged_ptr){struct _tuple0*_tmp62F=Cyc_Toc_temp_var();void*
_tmp630=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp631=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp634[3];_tmp634[2]=_tmp5FF;_tmp634[1]=
Cyc_Absyn_sizeoftyp_exp(elt_typ,0);_tmp634[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(
struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp634,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);decls=({struct Cyc_List_List*_tmp632=_region_malloc(r,sizeof(struct Cyc_List_List));
_tmp632->hd=({struct _tuple9*_tmp633=_region_malloc(r,sizeof(struct _tuple9));
_tmp633->f1=_tmp62F;_tmp633->f2=_tmp630;_tmp633->f3=(struct Cyc_Absyn_Exp*)
_tmp631;_tmp633;});_tmp632->tl=decls;_tmp632;});s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp(_tmp62F,0),0),0);}else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp(a,0),0),0);}{struct Cyc_List_List*_tmp635=decls;for(0;_tmp635 != 
0;_tmp635=_tmp635->tl){struct Cyc_Absyn_Exp*_tmp638;void*_tmp63A;struct _tuple0*
_tmp63C;struct _tuple9 _tmp636=*((struct _tuple9*)_tmp635->hd);_LL1597: _tmp63C=
_tmp636.f1;goto _LL1595;_LL1595: _tmp63A=_tmp636.f2;goto _LL1593;_LL1593: _tmp638=
_tmp636.f3;goto _LL1591;_LL1591: s=Cyc_Absyn_declare_stmt(_tmp63C,_tmp63A,_tmp638,
s,0);}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(s,0))->r));}};_pop_region(
r);}goto _LL1520;}}}_LL1526:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4D4->topt))->v,1,_tmp4D6,_tmp603,
_tmp605))->r));goto _LL1520;_LL1528:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,_tmp4D6,_tmp607))->r));goto _LL1520;_LL1530: {void*old_elt_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4D4->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(
old_elt_typ);struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);if(
_tmp4D6 == 0){mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);}else{struct Cyc_Absyn_Exp*
r=(struct Cyc_Absyn_Exp*)_check_null(_tmp4D6);Cyc_Toc_exp_to_c(nv,r);mexp=Cyc_Toc_rmalloc_exp(
r,mexp);}{int done=0;{void*_tmp640=(void*)_tmp4D4->r;struct Cyc_Absyn_Exp*_tmp646;
void*_tmp648;_LL1602: if(*((int*)_tmp640)== Cyc_Absyn_Cast_e){_LL1609: _tmp648=(
void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp640)->f1;goto _LL1607;_LL1607: _tmp646=((
struct Cyc_Absyn_Cast_e_struct*)_tmp640)->f2;goto _LL1603;}else{goto _LL1604;}
_LL1604: goto _LL1605;_LL1603:{struct _tuple8 _tmp64B=({struct _tuple8 _tmp64A;_tmp64A.f1=
Cyc_Tcutil_compress(_tmp648);_tmp64A.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp646->topt))->v);_tmp64A;});void*_tmp651;struct Cyc_Absyn_PtrInfo
_tmp653;struct Cyc_Absyn_Conref*_tmp655;void*_tmp657;struct Cyc_Absyn_PtrInfo
_tmp659;struct Cyc_Absyn_Conref*_tmp65B;void*_tmp65D;_LL1613: _LL1624: _tmp657=
_tmp64B.f1;if((unsigned int)_tmp657 > 3?*((int*)_tmp657)== Cyc_Absyn_PointerType:
0){_LL1626: _tmp659=((struct Cyc_Absyn_PointerType_struct*)_tmp657)->f1;_LL1630:
_tmp65D=(void*)_tmp659.elt_typ;goto _LL1628;_LL1628: _tmp65B=_tmp659.bounds;goto
_LL1618;}else{goto _LL1615;}_LL1618: _tmp651=_tmp64B.f2;if((unsigned int)_tmp651 > 
3?*((int*)_tmp651)== Cyc_Absyn_PointerType: 0){_LL1620: _tmp653=((struct Cyc_Absyn_PointerType_struct*)
_tmp651)->f1;_LL1622: _tmp655=_tmp653.bounds;goto _LL1614;}else{goto _LL1615;}
_LL1615: goto _LL1616;_LL1614:{struct _tuple8 _tmp660=({struct _tuple8 _tmp65F;_tmp65F.f1=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp65B);_tmp65F.f2=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp655);_tmp65F;});void*_tmp666;struct Cyc_Absyn_Exp*
_tmp668;void*_tmp66A;_LL1634: _LL1643: _tmp66A=_tmp660.f1;if(_tmp66A == (void*)Cyc_Absyn_Unknown_b){
goto _LL1639;}else{goto _LL1636;}_LL1639: _tmp666=_tmp660.f2;if((unsigned int)
_tmp666 > 1?*((int*)_tmp666)== Cyc_Absyn_Upper_b: 0){_LL1641: _tmp668=((struct Cyc_Absyn_Upper_b_struct*)
_tmp666)->f1;goto _LL1635;}else{goto _LL1636;}_LL1636: goto _LL1637;_LL1635: done=1;(
void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__init_tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp66C[4];_tmp66C[3]=_tmp668;_tmp66C[2]=Cyc_Absyn_sizeoftyp_exp(
_tmp65D,0);_tmp66C[1]=_tmp646;_tmp66C[0]=mexp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp66C,sizeof(struct Cyc_Absyn_Exp*),4));}),
0))->r));goto _LL1633;_LL1637: goto _LL1633;_LL1633:;}goto _LL1612;_LL1616: goto
_LL1612;_LL1612:;}goto _LL1601;_LL1605: goto _LL1601;_LL1601:;}if(! done){struct
_tuple0*_tmp66D=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp66E=Cyc_Absyn_exp_stmt(
Cyc_Absyn_var_exp(_tmp66D,0),0);struct Cyc_Absyn_Exp*_tmp66F=Cyc_Absyn_signed_int_exp(
0,0);Cyc_Toc_exp_to_c(nv,_tmp4D4);_tmp66E=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(_tmp66D,0),_tmp66F,0),_tmp4D4,0),
_tmp66E,0);{void*_tmp670=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)((void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp66D,_tmp670,(struct
Cyc_Absyn_Exp*)mexp,_tmp66E,0),0))->r));}}goto _LL1520;}}_LL1520:;}goto _LL1081;
_LL1119: Cyc_Toc_exp_to_c(nv,_tmp4D8);goto _LL1081;_LL1121:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp671=_cycalloc(sizeof(struct Cyc_Absyn_Sizeoftyp_e_struct));
_tmp671[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmp672;_tmp672.tag=Cyc_Absyn_Sizeoftyp_e;
_tmp672.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4DA);_tmp672;});_tmp671;})));goto
_LL1081;_LL1123:(int)_throw(({void*_tmp673[0]={};Cyc_Toc_toc_impos(_tag_arr("__gen() in code generator",
sizeof(unsigned char),26),_tag_arr(_tmp673,sizeof(void*),0));}));_LL1125:(void*)(
e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp674=_cycalloc(
sizeof(struct Cyc_Absyn_Offsetof_e_struct));_tmp674[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp675;_tmp675.tag=Cyc_Absyn_Offsetof_e;_tmp675.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp4E0);_tmp675.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp676=
_cycalloc(sizeof(struct Cyc_Absyn_StructField_struct));_tmp676[0]=({struct Cyc_Absyn_StructField_struct
_tmp677;_tmp677.tag=Cyc_Absyn_StructField;_tmp677.f1=_tmp4DE;_tmp677;});_tmp676;}));
_tmp675;});_tmp674;})));goto _LL1081;_LL1127:{void*_tmp678=Cyc_Tcutil_compress(
_tmp4E6);struct Cyc_Absyn_Structdecl**_tmp688;struct Cyc_List_List*_tmp68A;struct
Cyc_Absyn_Uniondecl**_tmp68C;struct Cyc_List_List*_tmp68E;_LL1658: if((
unsigned int)_tmp678 > 3?*((int*)_tmp678)== Cyc_Absyn_StructType: 0){_LL1673:
_tmp688=((struct Cyc_Absyn_StructType_struct*)_tmp678)->f3;goto _LL1659;}else{goto
_LL1660;}_LL1660: if((unsigned int)_tmp678 > 3?*((int*)_tmp678)== Cyc_Absyn_AnonStructType:
0){_LL1675: _tmp68A=((struct Cyc_Absyn_AnonStructType_struct*)_tmp678)->f1;goto
_LL1661;}else{goto _LL1662;}_LL1662: if((unsigned int)_tmp678 > 3?*((int*)_tmp678)
== Cyc_Absyn_UnionType: 0){_LL1677: _tmp68C=((struct Cyc_Absyn_UnionType_struct*)
_tmp678)->f3;goto _LL1663;}else{goto _LL1664;}_LL1664: if((unsigned int)_tmp678 > 3?*((
int*)_tmp678)== Cyc_Absyn_AnonUnionType: 0){_LL1679: _tmp68E=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp678)->f1;goto _LL1665;}else{goto _LL1666;}_LL1666: if((unsigned int)_tmp678 > 3?*((
int*)_tmp678)== Cyc_Absyn_TupleType: 0){goto _LL1667;}else{goto _LL1668;}_LL1668:
if((unsigned int)_tmp678 > 3?*((int*)_tmp678)== Cyc_Absyn_TunionFieldType: 0){goto
_LL1669;}else{goto _LL1670;}_LL1670: goto _LL1671;_LL1659: if(_tmp688 == 0? 1:(*
_tmp688)->fields == 0){(int)_throw(({void*_tmp690[0]={};Cyc_Toc_toc_impos(
_tag_arr("struct fields must be known",sizeof(unsigned char),28),_tag_arr(
_tmp690,sizeof(void*),0));}));}_tmp68A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null((*_tmp688)->fields))->v;goto _LL1661;_LL1661: {struct Cyc_Absyn_Structfield*
_tmp691=((struct Cyc_Absyn_Structfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmp68A,(int)_tmp4E4);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp692=_cycalloc(sizeof(struct Cyc_Absyn_Offsetof_e_struct));_tmp692[0]=({struct
Cyc_Absyn_Offsetof_e_struct _tmp693;_tmp693.tag=Cyc_Absyn_Offsetof_e;_tmp693.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp4E6);_tmp693.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*
_tmp694=_cycalloc(sizeof(struct Cyc_Absyn_StructField_struct));_tmp694[0]=({
struct Cyc_Absyn_StructField_struct _tmp695;_tmp695.tag=Cyc_Absyn_StructField;
_tmp695.f1=_tmp691->name;_tmp695;});_tmp694;}));_tmp693;});_tmp692;})));goto
_LL1657;}_LL1663: if(_tmp68C == 0? 1:(*_tmp68C)->fields == 0){(int)_throw(({void*
_tmp696[0]={};Cyc_Toc_toc_impos(_tag_arr("union fields must be known",sizeof(
unsigned char),27),_tag_arr(_tmp696,sizeof(void*),0));}));}_tmp68E=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null((*_tmp68C)->fields))->v;goto _LL1665;_LL1665: {
struct Cyc_Absyn_Structfield*_tmp697=((struct Cyc_Absyn_Structfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp68E,(int)_tmp4E4);(void*)(e->r=(void*)((void*)({struct
Cyc_Absyn_Offsetof_e_struct*_tmp698=_cycalloc(sizeof(struct Cyc_Absyn_Offsetof_e_struct));
_tmp698[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp699;_tmp699.tag=Cyc_Absyn_Offsetof_e;
_tmp699.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4E6);_tmp699.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp69A=_cycalloc(sizeof(struct Cyc_Absyn_StructField_struct));
_tmp69A[0]=({struct Cyc_Absyn_StructField_struct _tmp69B;_tmp69B.tag=Cyc_Absyn_StructField;
_tmp69B.f1=_tmp697->name;_tmp69B;});_tmp69A;}));_tmp699;});_tmp698;})));goto
_LL1657;}_LL1667:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp69C=_cycalloc(sizeof(struct Cyc_Absyn_Offsetof_e_struct));_tmp69C[0]=({struct
Cyc_Absyn_Offsetof_e_struct _tmp69D;_tmp69D.tag=Cyc_Absyn_Offsetof_e;_tmp69D.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp4E6);_tmp69D.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*
_tmp69E=_cycalloc(sizeof(struct Cyc_Absyn_StructField_struct));_tmp69E[0]=({
struct Cyc_Absyn_StructField_struct _tmp69F;_tmp69F.tag=Cyc_Absyn_StructField;
_tmp69F.f1=Cyc_Absyn_fieldname((int)(_tmp4E4 + 1));_tmp69F;});_tmp69E;}));_tmp69D;});
_tmp69C;})));goto _LL1657;_LL1669: if(_tmp4E4 == 0){(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Offsetof_e_struct*_tmp6A0=_cycalloc(sizeof(struct Cyc_Absyn_Offsetof_e_struct));
_tmp6A0[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp6A1;_tmp6A1.tag=Cyc_Absyn_Offsetof_e;
_tmp6A1.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp4E6);_tmp6A1.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp6A2=_cycalloc(sizeof(struct Cyc_Absyn_StructField_struct));
_tmp6A2[0]=({struct Cyc_Absyn_StructField_struct _tmp6A3;_tmp6A3.tag=Cyc_Absyn_StructField;
_tmp6A3.f1=Cyc_Toc_tag_sp;_tmp6A3;});_tmp6A2;}));_tmp6A1;});_tmp6A0;})));}else{(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp6A4=_cycalloc(
sizeof(struct Cyc_Absyn_Offsetof_e_struct));_tmp6A4[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp6A5;_tmp6A5.tag=Cyc_Absyn_Offsetof_e;_tmp6A5.f1=(void*)Cyc_Toc_typ_to_c_array(
_tmp4E6);_tmp6A5.f2=(void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp6A6=
_cycalloc(sizeof(struct Cyc_Absyn_StructField_struct));_tmp6A6[0]=({struct Cyc_Absyn_StructField_struct
_tmp6A7;_tmp6A7.tag=Cyc_Absyn_StructField;_tmp6A7.f1=Cyc_Absyn_fieldname((int)
_tmp4E4);_tmp6A7;});_tmp6A6;}));_tmp6A5;});_tmp6A4;})));}goto _LL1657;_LL1671:(
int)_throw(({void*_tmp6A8[0]={};Cyc_Toc_toc_impos(_tag_arr("impossible type for offsetof tuple index",
sizeof(unsigned char),41),_tag_arr(_tmp6A8,sizeof(void*),0));}));_LL1657:;}goto
_LL1081;_LL1129: {void*_tmp6A9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp4E8->topt))->v);{void*_tmp6AA=_tmp6A9;struct Cyc_Absyn_PtrInfo
_tmp6B0;struct Cyc_Absyn_Conref*_tmp6B2;struct Cyc_Absyn_Tqual _tmp6B4;struct Cyc_Absyn_Conref*
_tmp6B6;void*_tmp6B8;void*_tmp6BA;_LL1708: if((unsigned int)_tmp6AA > 3?*((int*)
_tmp6AA)== Cyc_Absyn_PointerType: 0){_LL1713: _tmp6B0=((struct Cyc_Absyn_PointerType_struct*)
_tmp6AA)->f1;_LL1723: _tmp6BA=(void*)_tmp6B0.elt_typ;goto _LL1721;_LL1721: _tmp6B8=(
void*)_tmp6B0.rgn_typ;goto _LL1719;_LL1719: _tmp6B6=_tmp6B0.nullable;goto _LL1717;
_LL1717: _tmp6B4=_tmp6B0.tq;goto _LL1715;_LL1715: _tmp6B2=_tmp6B0.bounds;goto
_LL1709;}else{goto _LL1710;}_LL1710: goto _LL1711;_LL1709:{void*_tmp6BC=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp6B2);_LL1726: if((unsigned int)_tmp6BC > 1?*((int*)
_tmp6BC)== Cyc_Absyn_Upper_b: 0){goto _LL1727;}else{goto _LL1728;}_LL1728: if(
_tmp6BC == (void*)Cyc_Absyn_Unknown_b){goto _LL1729;}else{goto _LL1725;}_LL1727: {
int do_null_check=Cyc_Toc_need_null_check(_tmp4E8);Cyc_Toc_exp_to_c(nv,_tmp4E8);
if(do_null_check){(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(_tmp4E8->topt))->v),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp6C2=
_cycalloc(sizeof(struct Cyc_List_List));_tmp6C2->hd=_tmp4E8;_tmp6C2->tl=0;_tmp6C2;}),
0),0),0))->r));}goto _LL1725;}_LL1729: {struct Cyc_Absyn_Exp*_tmp6C3=Cyc_Absyn_uint_exp(
0,0);_tmp6C3->topt=({struct Cyc_Core_Opt*_tmp6C4=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp6C4->v=(void*)Cyc_Absyn_uint_t;_tmp6C4;});(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(
_tmp4E8,_tmp6C3,0))->r));Cyc_Toc_exp_to_c(nv,e);goto _LL1725;}_LL1725:;}goto
_LL1707;_LL1711:({void*_tmp6C5[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: Deref: non-pointer",
sizeof(unsigned char),29),_tag_arr(_tmp6C5,sizeof(void*),0));});goto _LL1707;
_LL1707:;}goto _LL1081;}_LL1131: Cyc_Toc_exp_to_c(nv,_tmp4EC);if(Cyc_Toc_is_poly_project(
e)){(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,
0),0))->r));}goto _LL1081;_LL1133: {void*e1typ=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4F0->topt))->v);int do_null_check=Cyc_Toc_need_null_check(
_tmp4F0);Cyc_Toc_exp_to_c(nv,_tmp4F0);{int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_Conref*_tmp6C8;struct Cyc_Absyn_Tqual _tmp6CA;struct Cyc_Absyn_Conref*
_tmp6CC;void*_tmp6CE;void*_tmp6D0;struct Cyc_Absyn_PtrInfo _tmp6C6=Cyc_Toc_get_ptr_type(
e1typ);_LL1745: _tmp6D0=(void*)_tmp6C6.elt_typ;goto _LL1743;_LL1743: _tmp6CE=(void*)
_tmp6C6.rgn_typ;goto _LL1741;_LL1741: _tmp6CC=_tmp6C6.nullable;goto _LL1739;_LL1739:
_tmp6CA=_tmp6C6.tq;goto _LL1737;_LL1737: _tmp6C8=_tmp6C6.bounds;goto _LL1735;
_LL1735:{void*_tmp6D2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp6C8);struct
Cyc_Absyn_Exp*_tmp6D8;_LL1748: if((unsigned int)_tmp6D2 > 1?*((int*)_tmp6D2)== Cyc_Absyn_Upper_b:
0){_LL1753: _tmp6D8=((struct Cyc_Absyn_Upper_b_struct*)_tmp6D2)->f1;goto _LL1749;}
else{goto _LL1750;}_LL1750: if(_tmp6D2 == (void*)Cyc_Absyn_Unknown_b){goto _LL1751;}
else{goto _LL1747;}_LL1749: if(Cyc_Evexp_eval_const_uint_exp(_tmp6D8)< 1){({void*
_tmp6DA[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("exp_to_c:  StructArrow_e on pointer of size 0",sizeof(unsigned char),
46),_tag_arr(_tmp6DA,sizeof(void*),0));});}if(do_null_check){(void*)(e->r=(void*)((
void*)(Cyc_Absyn_structarrow_exp(Cyc_Absyn_cast_exp(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp4F0->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp6DB=_cycalloc(sizeof(struct Cyc_List_List));_tmp6DB->hd=
_tmp4F0;_tmp6DB->tl=0;_tmp6DB;}),0),0),_tmp4EE,0))->r));}goto _LL1747;_LL1751: {
void*ta1=Cyc_Toc_typ_to_c_array(_tmp6D0);(void*)(_tmp4F0->r=(void*)((void*)(Cyc_Absyn_cast_exp(
Cyc_Absyn_cstar_typ(ta1,_tmp6CA),Cyc_Absyn_fncall_exp(Cyc_Toc__check_unknown_subscript_e,({
struct Cyc_Absyn_Exp*_tmp6DC[3];_tmp6DC[2]=Cyc_Absyn_uint_exp(0,0);_tmp6DC[1]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp6DC[0]=Cyc_Absyn_copy_exp(_tmp4F0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp6DC,sizeof(struct Cyc_Absyn_Exp*),3));}),
0),0))->r));goto _LL1747;}_LL1747:;}if(is_poly){(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(
Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((
void*)e->r,0),0))->r));}goto _LL1081;}}_LL1135: {void*_tmp6DD=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4F4->topt))->v);{void*_tmp6DE=
_tmp6DD;struct Cyc_List_List*_tmp6E6;struct Cyc_Absyn_PtrInfo _tmp6E8;struct Cyc_Absyn_Conref*
_tmp6EA;struct Cyc_Absyn_Tqual _tmp6EC;struct Cyc_Absyn_Conref*_tmp6EE;void*_tmp6F0;
void*_tmp6F2;_LL1760: if((unsigned int)_tmp6DE > 3?*((int*)_tmp6DE)== Cyc_Absyn_TupleType:
0){_LL1767: _tmp6E6=((struct Cyc_Absyn_TupleType_struct*)_tmp6DE)->f1;goto _LL1761;}
else{goto _LL1762;}_LL1762: if((unsigned int)_tmp6DE > 3?*((int*)_tmp6DE)== Cyc_Absyn_PointerType:
0){_LL1769: _tmp6E8=((struct Cyc_Absyn_PointerType_struct*)_tmp6DE)->f1;_LL1779:
_tmp6F2=(void*)_tmp6E8.elt_typ;goto _LL1777;_LL1777: _tmp6F0=(void*)_tmp6E8.rgn_typ;
goto _LL1775;_LL1775: _tmp6EE=_tmp6E8.nullable;goto _LL1773;_LL1773: _tmp6EC=_tmp6E8.tq;
goto _LL1771;_LL1771: _tmp6EA=_tmp6E8.bounds;goto _LL1763;}else{goto _LL1764;}
_LL1764: goto _LL1765;_LL1761: Cyc_Toc_exp_to_c(nv,_tmp4F4);Cyc_Toc_exp_to_c(nv,
_tmp4F2);{unsigned int i=Cyc_Evexp_eval_const_uint_exp(_tmp4F2)+ 1;(void*)(e->r=(
void*)((void*)(Cyc_Absyn_structmember_exp(_tmp4F4,Cyc_Absyn_fieldname((int)i),0))->r));
goto _LL1759;}_LL1763: {struct Cyc_List_List*_tmp6F4=Cyc_Toc_get_relns(_tmp4F4);
int _tmp6F5=Cyc_Toc_check_bounds(_tmp6F4,_tmp4F4,_tmp4F2);if(Cyc_Toc_warn_bounds_checks?
! _tmp6F5: 0){({struct Cyc_Std_String_pa_struct _tmp6F7;_tmp6F7.tag=Cyc_Std_String_pa;
_tmp6F7.f1=(struct _tagged_arr)Cyc_Absynpp_exp2string(e);{void*_tmp6F6[1]={&
_tmp6F7};Cyc_Tcutil_warn(e->loc,_tag_arr("bounds check necessary for %s",sizeof(
unsigned char),30),_tag_arr(_tmp6F6,sizeof(void*),1));}});}Cyc_Toc_exp_to_c(nv,
_tmp4F4);Cyc_Toc_exp_to_c(nv,_tmp4F2);Cyc_Toc_total_bounds_checks ++;{void*
_tmp6F8=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp6EA);struct Cyc_Absyn_Exp*
_tmp6FE;_LL1786: if((unsigned int)_tmp6F8 > 1?*((int*)_tmp6F8)== Cyc_Absyn_Upper_b:
0){_LL1791: _tmp6FE=((struct Cyc_Absyn_Upper_b_struct*)_tmp6F8)->f1;goto _LL1787;}
else{goto _LL1788;}_LL1788: if(_tmp6F8 == (void*)Cyc_Absyn_Unknown_b){goto _LL1789;}
else{goto _LL1785;}_LL1787: _tmp6FE=Cyc_Absyn_uint_exp(Cyc_Evexp_eval_const_uint_exp(
_tmp6FE),0);{int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp6EE);void*ta1=Cyc_Toc_typ_to_c(_tmp6F2);void*ta2=Cyc_Absyn_cstar_typ(ta1,
_tmp6EC);if(_tmp6F5){Cyc_Toc_bounds_checks_eliminated ++;}else{if(possibly_null){(
void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(ta2,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp700[4];_tmp700[3]=
_tmp4F2;_tmp700[2]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp700[1]=_tmp6FE;_tmp700[0]=
_tmp4F4;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp700,sizeof(struct Cyc_Absyn_Exp*),4));}),0),0),0))->r));}else{(void*)(_tmp4F2->r=(
void*)((void*)(Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp701[2];_tmp701[1]=Cyc_Absyn_copy_exp(_tmp4F2);_tmp701[0]=
_tmp6FE;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp701,sizeof(struct Cyc_Absyn_Exp*),2));}),0))->r));}}goto _LL1785;}_LL1789: {
void*ta1=Cyc_Toc_typ_to_c_array(_tmp6F2);if(_tmp6F5){Cyc_Toc_bounds_checks_eliminated
++;(void*)(e->r=(void*)((void*)(Cyc_Absyn_subscript_exp(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
ta1,_tmp6EC),Cyc_Absyn_structmember_exp(_tmp4F4,Cyc_Toc_curr_sp,0),0),_tmp4F2,0))->r));}
else{(void*)(e->r=(void*)((void*)(Cyc_Absyn_deref_exp(Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
ta1,_tmp6EC),Cyc_Absyn_fncall_exp(Cyc_Toc__check_unknown_subscript_e,({struct Cyc_Absyn_Exp*
_tmp702[3];_tmp702[2]=_tmp4F2;_tmp702[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp702[
0]=_tmp4F4;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(
_tmp702,sizeof(struct Cyc_Absyn_Exp*),3));}),0),0),0))->r));}goto _LL1785;}_LL1785:;}
goto _LL1759;}_LL1765:({void*_tmp703[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("exp_to_c: Subscript on non-tuple/array/tuple ptr",
sizeof(unsigned char),49),_tag_arr(_tmp703,sizeof(void*),0));});goto _LL1759;
_LL1759:;}goto _LL1081;}_LL1137: if(! nv->toplevel){(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_tuple(nv,0,0,_tmp4F6))->r));}else{struct Cyc_List_List*_tmp704=((
struct Cyc_List_List*(*)(struct _tuple3*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp4F6);struct _tagged_arr*_tmp705=Cyc_Toc_add_tuple_type(
_tmp704);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp4F6 != 0;(_tmp4F6=_tmp4F6->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4F6->hd);dles=({struct Cyc_List_List*
_tmp706=_cycalloc(sizeof(struct Cyc_List_List));_tmp706->hd=({struct _tuple4*
_tmp707=_cycalloc(sizeof(struct _tuple4));_tmp707->f1=0;_tmp707->f2=(struct Cyc_Absyn_Exp*)
_tmp4F6->hd;_tmp707;});_tmp706->tl=dles;_tmp706;});}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(
0,dles,0))->r));}goto _LL1081;_LL1139:(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(
0,_tmp4F8,0))->r));{struct Cyc_List_List*_tmp708=_tmp4F8;for(0;_tmp708 != 0;
_tmp708=_tmp708->tl){struct _tuple4 _tmp70B;struct Cyc_Absyn_Exp*_tmp70C;struct
_tuple4*_tmp709=(struct _tuple4*)_tmp708->hd;_tmp70B=*_tmp709;_LL1805: _tmp70C=
_tmp70B.f2;goto _LL1802;_LL1802: Cyc_Toc_exp_to_c(nv,_tmp70C);}}goto _LL1081;
_LL1141: {unsigned int _tmp70E=Cyc_Evexp_eval_const_uint_exp(_tmp4FC);Cyc_Toc_exp_to_c(
nv,_tmp4FA);{struct Cyc_List_List*es=0;if(! Cyc_Toc_is_zero(_tmp4FA)){unsigned int
i=0;for(0;i < _tmp70E;i ++){es=({struct Cyc_List_List*_tmp70F=_cycalloc(sizeof(
struct Cyc_List_List));_tmp70F->hd=({struct _tuple4*_tmp710=_cycalloc(sizeof(
struct _tuple4));_tmp710->f1=0;_tmp710->f2=_tmp4FA;_tmp710;});_tmp70F->tl=es;
_tmp70F;});}}(void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(0,es,0))->r));
goto _LL1081;}}_LL1143: if(! nv->toplevel){(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,old_typ,0,0,_tmp502,_tmp506))->r));}else{if(_tmp500 == 0){({void*_tmp711[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("Struct_e: missing structdecl pointer",
sizeof(unsigned char),37),_tag_arr(_tmp711,sizeof(void*),0));});}{struct Cyc_Absyn_Structdecl*
sd2=(struct Cyc_Absyn_Structdecl*)_check_null(_tmp500);struct _RegionHandle _tmp712=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp712;_push_region(rgn);{struct Cyc_List_List*
_tmp713=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,e->loc,_tmp502,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sd2->fields))->v);
struct Cyc_List_List*_tmp714=0;{struct Cyc_List_List*_tmp715=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(sd2->fields))->v;for(0;_tmp715 != 0;_tmp715=
_tmp715->tl){struct Cyc_List_List*_tmp716=_tmp713;for(0;_tmp716 != 0;_tmp716=
_tmp716->tl){if((*((struct _tuple10*)_tmp716->hd)).f1 == (struct Cyc_Absyn_Structfield*)
_tmp715->hd){struct _tuple10 _tmp719;struct Cyc_Absyn_Exp*_tmp71A;struct Cyc_Absyn_Structfield*
_tmp71C;struct _tuple10*_tmp717=(struct _tuple10*)_tmp716->hd;_tmp719=*_tmp717;
_LL1821: _tmp71C=_tmp719.f1;goto _LL1819;_LL1819: _tmp71A=_tmp719.f2;goto _LL1816;
_LL1816: {void*_tmp71E=(void*)_tmp71C->type;Cyc_Toc_exp_to_c(nv,_tmp71A);if(Cyc_Toc_is_void_star(
_tmp71E)){(void*)(_tmp71A->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp71A->r,0),0))->r));}_tmp714=({struct Cyc_List_List*
_tmp71F=_cycalloc(sizeof(struct Cyc_List_List));_tmp71F->hd=({struct _tuple4*
_tmp720=_cycalloc(sizeof(struct _tuple4));_tmp720->f1=0;_tmp720->f2=_tmp71A;
_tmp720;});_tmp71F->tl=_tmp714;_tmp71F;});break;}}}}}(void*)(e->r=(void*)((void*)(
Cyc_Absyn_unresolvedmem_exp(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp714),0))->r));};_pop_region(rgn);}}goto _LL1081;_LL1145: {
struct Cyc_List_List*fs;{void*_tmp721=Cyc_Tcutil_compress(_tmp50A);struct Cyc_List_List*
_tmp727;_LL1827: if((unsigned int)_tmp721 > 3?*((int*)_tmp721)== Cyc_Absyn_AnonStructType:
0){_LL1832: _tmp727=((struct Cyc_Absyn_AnonStructType_struct*)_tmp721)->f1;goto
_LL1828;}else{goto _LL1829;}_LL1829: goto _LL1830;_LL1828: fs=_tmp727;goto _LL1826;
_LL1830: fs=({struct Cyc_Std_String_pa_struct _tmp72A;_tmp72A.tag=Cyc_Std_String_pa;
_tmp72A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp50A);{void*_tmp729[1]={&
_tmp72A};((struct Cyc_List_List*(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(
_tag_arr("anon struct has type %s",sizeof(unsigned char),24),_tag_arr(_tmp729,
sizeof(void*),1));}});goto _LL1826;_LL1826:;}{struct _RegionHandle _tmp72B=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp72B;_push_region(rgn);{struct Cyc_List_List*
_tmp72C=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,e->loc,_tmp508,fs);for(0;_tmp72C != 0;_tmp72C=_tmp72C->tl){struct _tuple10
_tmp72F;struct Cyc_Absyn_Exp*_tmp730;struct Cyc_Absyn_Structfield*_tmp732;struct
_tuple10*_tmp72D=(struct _tuple10*)_tmp72C->hd;_tmp72F=*_tmp72D;_LL1843: _tmp732=
_tmp72F.f1;goto _LL1841;_LL1841: _tmp730=_tmp72F.f2;goto _LL1838;_LL1838: {void*
_tmp734=(void*)_tmp732->type;Cyc_Toc_exp_to_c(nv,_tmp730);if(Cyc_Toc_is_void_star(
_tmp734)){(void*)(_tmp730->r=(void*)((void*)(Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),
Cyc_Absyn_new_exp((void*)_tmp730->r,0),0))->r));}}}(void*)(e->r=(void*)((void*)(
Cyc_Absyn_unresolvedmem_exp(0,_tmp508,0))->r));};_pop_region(rgn);}goto _LL1081;}
_LL1147: {struct _tuple0*qv=_tmp50C->name;if(_tmp50E->is_xtunion? 1: ! nv->toplevel){(
void*)(e->r=(void*)((void*)(Cyc_Absyn_var_exp(qv,0))->r));}else{int tag_count=0;
struct Cyc_List_List*_tmp735=_tmp50E->fields == 0? 0:(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp50E->fields))->v;while(Cyc_Absyn_qvar_cmp(qv,((
struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp735))->hd)->name)
!= 0){if(((struct Cyc_Absyn_Tunionfield*)_tmp735->hd)->typs == 0){tag_count ++;}
_tmp735=_tmp735->tl;}(void*)(e->r=(void*)((void*)(Cyc_Absyn_uint_exp((
unsigned int)tag_count,0))->r));}goto _LL1081;}_LL1149: {struct _tuple0*_tmp736=
_tmp512->name;struct Cyc_List_List*_tmp737=_tmp512->typs;struct _tuple0*_tmp738=
Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp739=Cyc_Absyn_var_exp(_tmp738,0);void*
_tmp73A=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp736,_tag_arr("_struct",
sizeof(unsigned char),8)));if(nv->toplevel){struct Cyc_Absyn_Exp*tag_exp;if(
_tmp514->is_xtunion){tag_exp=Cyc_Absyn_var_exp(_tmp736,0);}else{int tag_count=0;
struct Cyc_List_List*_tmp73B=_tmp514->fields == 0? 0:(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp514->fields))->v;while(Cyc_Absyn_qvar_cmp(_tmp736,((
struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp73B))->hd)->name)
!= 0){if(((struct Cyc_Absyn_Tunionfield*)_tmp73B->hd)->typs != 0){tag_count ++;}
_tmp73B=_tmp73B->tl;}tag_exp=Cyc_Absyn_uint_exp((unsigned int)tag_count,0);}{
struct Cyc_List_List*dles=0;for(0;_tmp516 != 0;(_tmp516=_tmp516->tl,_tmp737=
_tmp737->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp516->hd;void*
field_typ=Cyc_Toc_typ_to_c((*((struct _tuple3*)((struct Cyc_List_List*)_check_null(
_tmp737))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(field_typ)){
cur_e=Cyc_Absyn_cast_exp(field_typ,cur_e,0);}dles=({struct Cyc_List_List*_tmp73C=
_cycalloc(sizeof(struct Cyc_List_List));_tmp73C->hd=({struct _tuple4*_tmp73D=
_cycalloc(sizeof(struct _tuple4));_tmp73D->f1=0;_tmp73D->f2=cur_e;_tmp73D;});
_tmp73C->tl=dles;_tmp73C;});}dles=({struct Cyc_List_List*_tmp73E=_cycalloc(
sizeof(struct Cyc_List_List));_tmp73E->hd=({struct _tuple4*_tmp73F=_cycalloc(
sizeof(struct _tuple4));_tmp73F->f1=0;_tmp73F->f2=tag_exp;_tmp73F;});_tmp73E->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp73E;});(
void*)(e->r=(void*)((void*)(Cyc_Absyn_unresolvedmem_exp(({struct Cyc_Core_Opt*
_tmp740=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp740->v=Cyc_Toc_collapse_qvar_tag(
_tmp736,_tag_arr("_struct",sizeof(unsigned char),8));_tmp740;}),dles,0))->r));}}
else{struct Cyc_List_List*_tmp741=({struct Cyc_List_List*_tmp747=_cycalloc(sizeof(
struct Cyc_List_List));_tmp747->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_structmember_exp(
_tmp739,Cyc_Toc_tag_sp,0),Cyc_Absyn_var_exp(_tmp736,0),0);_tmp747->tl=0;_tmp747;});{
int i=1;for(0;_tmp516 != 0;(((_tmp516=_tmp516->tl,i ++)),_tmp737=_tmp737->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp516->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple3*)((struct Cyc_List_List*)_check_null(_tmp737))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ)){cur_e=Cyc_Absyn_cast_exp(field_typ,
cur_e,0);}{struct Cyc_Absyn_Stmt*_tmp742=Cyc_Absyn_assign_stmt(Cyc_Absyn_structmember_exp(
_tmp739,Cyc_Absyn_fieldname(i),0),cur_e,0);_tmp741=({struct Cyc_List_List*_tmp743=
_cycalloc(sizeof(struct Cyc_List_List));_tmp743->hd=_tmp742;_tmp743->tl=_tmp741;
_tmp743;});}}}{struct Cyc_Absyn_Stmt*_tmp744=Cyc_Absyn_exp_stmt(_tmp739,0);struct
Cyc_Absyn_Stmt*_tmp745=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp746=_cycalloc(sizeof(struct Cyc_List_List));
_tmp746->hd=_tmp744;_tmp746->tl=_tmp741;_tmp746;})),0);(void*)(e->r=(void*)((
void*)(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp738,_tmp73A,0,_tmp745,0),0))->r));}}
goto _LL1081;}_LL1151: goto _LL1081;_LL1153: goto _LL1081;_LL1155: {void*t_c=Cyc_Toc_typ_to_c(*((
void**)_check_null(_tmp51E)));Cyc_Toc_exp_to_c(nv,_tmp51C);if(_tmp51A){struct
_tuple0*_tmp748=Cyc_Toc_temp_var();struct _tuple0*_tmp749=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(
_tmp522){xexp=_tmp51C;if(_tmp520 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp520);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp748,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp51E,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp748,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({
struct Cyc_Absyn_Exp*_tmp74A[3];_tmp74A[2]=Cyc_Absyn_var_exp(_tmp748,0);_tmp74A[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp74A[0]=Cyc_Absyn_var_exp(_tmp749,0);((struct
Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(_tag_arr(_tmp74A,sizeof(
struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp51C,0);if(_tmp520 != 0){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_check_null(_tmp520);Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(
_tmp748,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp51E,Cyc_Absyn_var_exp(_tmp748,0));}
rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_arr_e,({struct Cyc_Absyn_Exp*_tmp74B[3];
_tmp74B[2]=Cyc_Absyn_var_exp(_tmp748,0);_tmp74B[1]=Cyc_Absyn_uint_exp(1,0);
_tmp74B[0]=Cyc_Absyn_var_exp(_tmp749,0);((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp74B,sizeof(struct Cyc_Absyn_Exp*),3));}),
0);}{struct Cyc_Absyn_Stmt*_tmp74C=Cyc_Absyn_declare_stmt(_tmp748,Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp749,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)((void*)(Cyc_Absyn_stmt_exp(_tmp74C,0))->r));}}else{if(_tmp520 != 0){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_check_null(_tmp520);Cyc_Toc_exp_to_c(
nv,rgn);(void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp51E,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL1081;}_LL1157: Cyc_Toc_stmt_to_c(nv,_tmp524);goto _LL1081;
_LL1159:({void*_tmp74D[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("UnresolvedMem",sizeof(unsigned char),14),_tag_arr(
_tmp74D,sizeof(void*),0));});goto _LL1081;_LL1161:({void*_tmp74E[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("compoundlit",
sizeof(unsigned char),12),_tag_arr(_tmp74E,sizeof(void*),0));});goto _LL1081;
_LL1163:({void*_tmp74F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_unimp)(_tag_arr("codegen",sizeof(unsigned char),8),_tag_arr(_tmp74F,
sizeof(void*),0));});goto _LL1081;_LL1165:({void*_tmp750[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("fill",sizeof(
unsigned char),5),_tag_arr(_tmp750,sizeof(void*),0));});goto _LL1081;_LL1081:;}}
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_eq_goto_stmt(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,struct _tagged_arr*succ_lab,struct _tagged_arr*fail_lab){return
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(e1,e2,0),Cyc_Absyn_goto_stmt(succ_lab,
0),Cyc_Absyn_goto_stmt(fail_lab,0),0);}struct _tuple11{struct Cyc_Toc_Env*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple12{struct _tuple0*f1;
void*f2;};struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static
struct _tuple11 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,
void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,
struct _tagged_arr*succ_lab,struct _tagged_arr*fail_lab,struct Cyc_List_List*decls){
struct Cyc_Absyn_Stmt*s;{void*_tmp751=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp777;
struct Cyc_Absyn_Vardecl _tmp779;struct _tuple0*_tmp77A;struct Cyc_Absyn_Vardecl*
_tmp77C;int _tmp77E;void*_tmp780;unsigned char _tmp782;struct _tagged_arr _tmp784;
struct Cyc_Absyn_Enumfield*_tmp786;struct Cyc_Absyn_Enumdecl*_tmp788;struct Cyc_Absyn_Enumfield*
_tmp78A;void*_tmp78C;struct Cyc_List_List*_tmp78E;struct Cyc_List_List*_tmp790;
struct Cyc_Absyn_Tunionfield*_tmp792;struct Cyc_Absyn_Tuniondecl*_tmp794;struct Cyc_Absyn_Pat*
_tmp797;struct Cyc_Absyn_Pat _tmp799;void*_tmp79A;struct Cyc_List_List*_tmp79C;
struct Cyc_List_List*_tmp79E;struct Cyc_Absyn_Tunionfield*_tmp7A0;struct Cyc_Absyn_Tuniondecl*
_tmp7A2;struct Cyc_List_List*_tmp7A4;struct Cyc_List_List*_tmp7A6;struct Cyc_List_List*
_tmp7A8;struct Cyc_List_List*_tmp7AA;struct Cyc_Core_Opt*_tmp7AC;struct Cyc_Absyn_Structdecl*
_tmp7AE;struct Cyc_Absyn_Pat*_tmp7B0;_LL1875: if((unsigned int)_tmp751 > 2?*((int*)
_tmp751)== Cyc_Absyn_Var_p: 0){_LL1912: _tmp777=((struct Cyc_Absyn_Var_p_struct*)
_tmp751)->f1;_tmp779=*_tmp777;_LL1915: _tmp77A=_tmp779.name;goto _LL1876;}else{
goto _LL1877;}_LL1877: if(_tmp751 == (void*)Cyc_Absyn_Wild_p){goto _LL1878;}else{
goto _LL1879;}_LL1879: if((unsigned int)_tmp751 > 2?*((int*)_tmp751)== Cyc_Absyn_Reference_p:
0){_LL1917: _tmp77C=((struct Cyc_Absyn_Reference_p_struct*)_tmp751)->f1;goto
_LL1880;}else{goto _LL1881;}_LL1881: if(_tmp751 == (void*)Cyc_Absyn_Null_p){goto
_LL1882;}else{goto _LL1883;}_LL1883: if((unsigned int)_tmp751 > 2?*((int*)_tmp751)
== Cyc_Absyn_Int_p: 0){_LL1921: _tmp780=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp751)->f1;goto _LL1919;_LL1919: _tmp77E=((struct Cyc_Absyn_Int_p_struct*)_tmp751)->f2;
goto _LL1884;}else{goto _LL1885;}_LL1885: if((unsigned int)_tmp751 > 2?*((int*)
_tmp751)== Cyc_Absyn_Char_p: 0){_LL1923: _tmp782=((struct Cyc_Absyn_Char_p_struct*)
_tmp751)->f1;goto _LL1886;}else{goto _LL1887;}_LL1887: if((unsigned int)_tmp751 > 2?*((
int*)_tmp751)== Cyc_Absyn_Float_p: 0){_LL1925: _tmp784=((struct Cyc_Absyn_Float_p_struct*)
_tmp751)->f1;goto _LL1888;}else{goto _LL1889;}_LL1889: if((unsigned int)_tmp751 > 2?*((
int*)_tmp751)== Cyc_Absyn_Enum_p: 0){_LL1929: _tmp788=((struct Cyc_Absyn_Enum_p_struct*)
_tmp751)->f1;goto _LL1927;_LL1927: _tmp786=((struct Cyc_Absyn_Enum_p_struct*)
_tmp751)->f2;goto _LL1890;}else{goto _LL1891;}_LL1891: if((unsigned int)_tmp751 > 2?*((
int*)_tmp751)== Cyc_Absyn_AnonEnum_p: 0){_LL1933: _tmp78C=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp751)->f1;goto _LL1931;_LL1931: _tmp78A=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp751)->f2;goto _LL1892;}else{goto _LL1893;}_LL1893: if((unsigned int)_tmp751 > 2?*((
int*)_tmp751)== Cyc_Absyn_Tunion_p: 0){_LL1941: _tmp794=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp751)->f1;goto _LL1939;_LL1939: _tmp792=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp751)->f2;goto _LL1937;_LL1937: _tmp790=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp751)->f3;goto _LL1935;_LL1935: _tmp78E=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp751)->f4;if(_tmp78E == 0){goto _LL1894;}else{goto _LL1895;}}else{goto _LL1895;}
_LL1895: if((unsigned int)_tmp751 > 2?*((int*)_tmp751)== Cyc_Absyn_Pointer_p: 0){
_LL1944: _tmp797=((struct Cyc_Absyn_Pointer_p_struct*)_tmp751)->f1;_tmp799=*
_tmp797;_LL1947: _tmp79A=(void*)_tmp799.r;if((unsigned int)_tmp79A > 2?*((int*)
_tmp79A)== Cyc_Absyn_Tunion_p: 0){_LL1955: _tmp7A2=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp79A)->f1;goto _LL1953;_LL1953: _tmp7A0=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp79A)->f2;goto _LL1951;_LL1951: _tmp79E=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp79A)->f3;goto _LL1949;_LL1949: _tmp79C=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp79A)->f4;goto _LL1942;}else{goto _LL1897;}}else{goto _LL1897;}_LL1942: if(
_tmp79C != 0){goto _LL1896;}else{goto _LL1897;}_LL1897: if((unsigned int)_tmp751 > 2?*((
int*)_tmp751)== Cyc_Absyn_Tunion_p: 0){_LL1957: _tmp7A4=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp751)->f4;goto _LL1898;}else{goto _LL1899;}_LL1899: if((unsigned int)_tmp751 > 2?*((
int*)_tmp751)== Cyc_Absyn_Tuple_p: 0){_LL1959: _tmp7A6=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp751)->f1;goto _LL1900;}else{goto _LL1901;}_LL1901: if((unsigned int)_tmp751 > 2?*((
int*)_tmp751)== Cyc_Absyn_Struct_p: 0){_LL1967: _tmp7AE=((struct Cyc_Absyn_Struct_p_struct*)
_tmp751)->f1;goto _LL1965;_LL1965: _tmp7AC=((struct Cyc_Absyn_Struct_p_struct*)
_tmp751)->f2;goto _LL1963;_LL1963: _tmp7AA=((struct Cyc_Absyn_Struct_p_struct*)
_tmp751)->f3;goto _LL1961;_LL1961: _tmp7A8=((struct Cyc_Absyn_Struct_p_struct*)
_tmp751)->f4;goto _LL1902;}else{goto _LL1903;}_LL1903: if((unsigned int)_tmp751 > 2?*((
int*)_tmp751)== Cyc_Absyn_Pointer_p: 0){_LL1969: _tmp7B0=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp751)->f1;goto _LL1904;}else{goto _LL1905;}_LL1905: if((unsigned int)_tmp751 > 2?*((
int*)_tmp751)== Cyc_Absyn_UnknownId_p: 0){goto _LL1906;}else{goto _LL1907;}_LL1907:
if((unsigned int)_tmp751 > 2?*((int*)_tmp751)== Cyc_Absyn_UnknownCall_p: 0){goto
_LL1908;}else{goto _LL1909;}_LL1909: if((unsigned int)_tmp751 > 2?*((int*)_tmp751)
== Cyc_Absyn_UnknownFields_p: 0){goto _LL1910;}else{goto _LL1874;}_LL1876: nv=Cyc_Toc_add_varmap(
nv,_tmp77A,r);goto _LL1878;_LL1878: s=Cyc_Absyn_goto_stmt(succ_lab,0);goto _LL1874;
_LL1880: {struct _tuple0*_tmp7B2=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*
_tmp7B3=_region_malloc(rgn,sizeof(struct Cyc_List_List));_tmp7B3->hd=({struct
_tuple12*_tmp7B4=_region_malloc(rgn,sizeof(struct _tuple12));_tmp7B4->f1=_tmp7B2;
_tmp7B4->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp7B4;});
_tmp7B3->tl=decls;_tmp7B3;});nv=Cyc_Toc_add_varmap(nv,_tmp77C->name,Cyc_Absyn_var_exp(
_tmp7B2,0));s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7B2,
0),Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(
path,0),0),0),Cyc_Absyn_goto_stmt(succ_lab,0),0);goto _LL1874;}_LL1882: s=Cyc_Toc_if_eq_goto_stmt(
r,Cyc_Absyn_signed_int_exp(0,0),succ_lab,fail_lab);goto _LL1874;_LL1884: s=Cyc_Toc_if_eq_goto_stmt(
r,Cyc_Absyn_int_exp(_tmp780,_tmp77E,0),succ_lab,fail_lab);goto _LL1874;_LL1886: s=
Cyc_Toc_if_eq_goto_stmt(r,Cyc_Absyn_char_exp(_tmp782,0),succ_lab,fail_lab);goto
_LL1874;_LL1888: s=Cyc_Toc_if_eq_goto_stmt(r,Cyc_Absyn_float_exp(_tmp784,0),
succ_lab,fail_lab);goto _LL1874;_LL1890: s=Cyc_Toc_if_eq_goto_stmt(r,Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Enum_e_struct*_tmp7B5=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_struct));
_tmp7B5[0]=({struct Cyc_Absyn_Enum_e_struct _tmp7B6;_tmp7B6.tag=Cyc_Absyn_Enum_e;
_tmp7B6.f1=_tmp786->name;_tmp7B6.f2=(struct Cyc_Absyn_Enumdecl*)_tmp788;_tmp7B6.f3=(
struct Cyc_Absyn_Enumfield*)_tmp786;_tmp7B6;});_tmp7B5;}),0),succ_lab,fail_lab);
goto _LL1874;_LL1892: s=Cyc_Toc_if_eq_goto_stmt(r,Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AnonEnum_e_struct*_tmp7B7=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_struct));
_tmp7B7[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp7B8;_tmp7B8.tag=Cyc_Absyn_AnonEnum_e;
_tmp7B8.f1=_tmp78A->name;_tmp7B8.f2=(void*)_tmp78C;_tmp7B8.f3=(struct Cyc_Absyn_Enumfield*)
_tmp78A;_tmp7B8;});_tmp7B7;}),0),succ_lab,fail_lab);goto _LL1874;_LL1894: {struct
Cyc_Absyn_Exp*cmp_exp=Cyc_Absyn_var_exp(_tmp792->name,0);if(! _tmp794->is_xtunion){
cmp_exp=Cyc_Absyn_cast_exp(Cyc_Absyn_void_star_typ(),cmp_exp,0);}s=Cyc_Toc_if_eq_goto_stmt(
r,cmp_exp,succ_lab,fail_lab);goto _LL1874;}_LL1896: {struct Cyc_List_List*ss=0;int
cnt=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp79C);struct _tuple0*
tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp7A0->name,_tag_arr("_struct",sizeof(
unsigned char),8));struct Cyc_Absyn_Exp*rcast=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r,0);struct Cyc_List_List*_tmp7B9=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(
rgn,_tmp7A0->typs);struct Cyc_List_List*_tmp7BA=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(rgn,_tmp79C);for(0;_tmp7BA
!= 0;(((_tmp7BA=_tmp7BA->tl,_tmp7B9=((struct Cyc_List_List*)_check_null(_tmp7B9))->tl)),
cnt --)){struct Cyc_Absyn_Pat*_tmp7BB=(struct Cyc_Absyn_Pat*)_tmp7BA->hd;if((void*)
_tmp7BB->r == (void*)Cyc_Absyn_Wild_p){continue;}{void*_tmp7BC=(*((struct _tuple3*)((
struct Cyc_List_List*)_check_null(_tmp7B9))->hd)).f2;struct _tuple0*_tmp7BD=Cyc_Toc_temp_var();
void*_tmp7BE=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp7BB->topt))->v;void*
_tmp7BF=Cyc_Toc_typ_to_c(_tmp7BE);struct _tagged_arr*_tmp7C0=Cyc_Toc_fresh_label();
struct Cyc_Absyn_Exp*_tmp7C1=Cyc_Absyn_structarrow_exp(rcast,Cyc_Absyn_fieldname(
cnt),0);if(Cyc_Toc_is_void_star(Cyc_Toc_typ_to_c(_tmp7BC))){_tmp7C1=Cyc_Absyn_cast_exp(
_tmp7BF,_tmp7C1,0);}decls=({struct Cyc_List_List*_tmp7C2=_region_malloc(rgn,
sizeof(struct Cyc_List_List));_tmp7C2->hd=({struct _tuple12*_tmp7C3=_region_malloc(
rgn,sizeof(struct _tuple12));_tmp7C3->f1=_tmp7BD;_tmp7C3->f2=_tmp7BF;_tmp7C3;});
_tmp7C2->tl=decls;_tmp7C2;});{struct _tuple11 _tmp7C4=Cyc_Toc_xlate_pat(nv,rgn,
_tmp7BE,Cyc_Absyn_var_exp(_tmp7BD,0),_tmp7C1,_tmp7BB,succ_lab,fail_lab,decls);nv=
_tmp7C4.f1;decls=_tmp7C4.f2;{struct Cyc_Absyn_Stmt*_tmp7C5=_tmp7C4.f3;struct Cyc_Absyn_Stmt*
_tmp7C6=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7BD,0),_tmp7C1,0);struct Cyc_Absyn_Stmt*
_tmp7C7=Cyc_Absyn_seq_stmt(_tmp7C6,_tmp7C5,0);ss=({struct Cyc_List_List*_tmp7C8=
_cycalloc(sizeof(struct Cyc_List_List));_tmp7C8->hd=Cyc_Absyn_label_stmt(_tmp7C0,
_tmp7C7,0);_tmp7C8->tl=ss;_tmp7C8;});succ_lab=_tmp7C0;}}}}if(ss == 0){ss=({struct
Cyc_List_List*_tmp7C9=_cycalloc(sizeof(struct Cyc_List_List));_tmp7C9->hd=Cyc_Absyn_goto_stmt(
succ_lab,0);_tmp7C9->tl=0;_tmp7C9;});}{struct Cyc_Absyn_Exp*test_exp;if(_tmp7A2->is_xtunion){
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),
Cyc_Toc_mt_tq),r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*
e=Cyc_Absyn_var_exp(_tmp7A0->name,0);test_exp=Cyc_Absyn_eq_exp(e1,e,0);s=Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(e1,e,0),Cyc_Absyn_seq_stmts(ss,0),Cyc_Absyn_goto_stmt(fail_lab,
0),0);}else{unsigned int max_tag=0;{struct Cyc_List_List*_tmp7CA=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp7A2->fields))->v;for(0;_tmp7CA != 0;_tmp7CA=
_tmp7CA->tl){struct Cyc_Absyn_Tunionfield*_tmp7CB=(struct Cyc_Absyn_Tunionfield*)
_tmp7CA->hd;if(_tmp7CB->typs == 0){max_tag ++;}}}{struct Cyc_Absyn_Exp*max_tag_exp=
Cyc_Absyn_uint_exp(max_tag,0);struct Cyc_Absyn_Exp*e3=Cyc_Absyn_cast_exp(Cyc_Absyn_cstar_typ(
Cyc_Absyn_sint_t,Cyc_Toc_mt_tq),r,0);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(
e3,0);struct Cyc_Absyn_Exp*e5=Cyc_Absyn_gt_exp(Cyc_Absyn_cast_exp(Cyc_Absyn_uint_t,
r,0),max_tag_exp,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(_tmp7A0->name,0);
test_exp=max_tag > 0? Cyc_Absyn_and_exp(e5,Cyc_Absyn_eq_exp(e1,e,0),0): Cyc_Absyn_eq_exp(
e1,e,0);}}s=Cyc_Absyn_ifthenelse_stmt(test_exp,Cyc_Absyn_seq_stmts(ss,0),Cyc_Absyn_goto_stmt(
fail_lab,0),0);goto _LL1874;}}_LL1898: _tmp7A6=_tmp7A4;goto _LL1900;_LL1900: {
struct Cyc_List_List*ss=0;int cnt=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp7A6);{struct Cyc_List_List*_tmp7CC=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(rgn,_tmp7A6);for(0;_tmp7CC
!= 0;(_tmp7CC=_tmp7CC->tl,cnt --)){struct Cyc_Absyn_Pat*_tmp7CD=(struct Cyc_Absyn_Pat*)
_tmp7CC->hd;if((void*)_tmp7CD->r == (void*)Cyc_Absyn_Wild_p){continue;}{struct
_tuple0*_tmp7CE=Cyc_Toc_temp_var();void*_tmp7CF=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7CD->topt))->v;struct _tagged_arr*_tmp7D0=Cyc_Toc_fresh_label();
decls=({struct Cyc_List_List*_tmp7D1=_region_malloc(rgn,sizeof(struct Cyc_List_List));
_tmp7D1->hd=({struct _tuple12*_tmp7D2=_region_malloc(rgn,sizeof(struct _tuple12));
_tmp7D2->f1=_tmp7CE;_tmp7D2->f2=Cyc_Toc_typ_to_c(_tmp7CF);_tmp7D2;});_tmp7D1->tl=
decls;_tmp7D1;});{struct _tuple11 _tmp7D3=Cyc_Toc_xlate_pat(nv,rgn,_tmp7CF,Cyc_Absyn_var_exp(
_tmp7CE,0),Cyc_Absyn_structmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp7CD,
succ_lab,fail_lab,decls);nv=_tmp7D3.f1;decls=_tmp7D3.f2;{struct Cyc_Absyn_Stmt*
_tmp7D4=_tmp7D3.f3;struct Cyc_Absyn_Stmt*_tmp7D5=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp7CE,0),Cyc_Absyn_structmember_exp(r,Cyc_Absyn_fieldname(
cnt),0),0),_tmp7D4,0);ss=({struct Cyc_List_List*_tmp7D6=_cycalloc(sizeof(struct
Cyc_List_List));_tmp7D6->hd=Cyc_Absyn_label_stmt(_tmp7D0,_tmp7D5,0);_tmp7D6->tl=
ss;_tmp7D6;});succ_lab=_tmp7D0;}}}}}if(ss == 0){ss=({struct Cyc_List_List*_tmp7D7=
_cycalloc(sizeof(struct Cyc_List_List));_tmp7D7->hd=Cyc_Absyn_goto_stmt(succ_lab,
0);_tmp7D7->tl=0;_tmp7D7;});}s=Cyc_Absyn_seq_stmts(ss,0);goto _LL1874;}_LL1902: {
struct Cyc_List_List*ss=0;{struct Cyc_List_List*_tmp7D8=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(rgn,_tmp7A8);for(0;
_tmp7D8 != 0;_tmp7D8=_tmp7D8->tl){struct _tuple13*_tmp7D9=(struct _tuple13*)_tmp7D8->hd;
struct Cyc_Absyn_Pat*_tmp7DA=(*_tmp7D9).f2;if((void*)_tmp7DA->r == (void*)Cyc_Absyn_Wild_p){
continue;}{struct _tagged_arr*f;{void*_tmp7DB=(void*)((struct Cyc_List_List*)
_check_null((*_tmp7D9).f1))->hd;struct _tagged_arr*_tmp7E1;_LL2013: if(*((int*)
_tmp7DB)== Cyc_Absyn_FieldName){_LL2018: _tmp7E1=((struct Cyc_Absyn_FieldName_struct*)
_tmp7DB)->f1;goto _LL2014;}else{goto _LL2015;}_LL2015: goto _LL2016;_LL2014: f=
_tmp7E1;goto _LL2012;_LL2016:(int)_throw((void*)Cyc_Toc_Match_error);_LL2012:;}{
struct _tuple0*_tmp7E3=Cyc_Toc_temp_var();void*_tmp7E4=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7DA->topt))->v;void*_tmp7E5=Cyc_Toc_typ_to_c(_tmp7E4);struct
_tagged_arr*_tmp7E6=Cyc_Toc_fresh_label();decls=({struct Cyc_List_List*_tmp7E7=
_region_malloc(rgn,sizeof(struct Cyc_List_List));_tmp7E7->hd=({struct _tuple12*
_tmp7E8=_region_malloc(rgn,sizeof(struct _tuple12));_tmp7E8->f1=_tmp7E3;_tmp7E8->f2=
_tmp7E5;_tmp7E8;});_tmp7E7->tl=decls;_tmp7E7;});{struct _tuple11 _tmp7E9=Cyc_Toc_xlate_pat(
nv,rgn,_tmp7E4,Cyc_Absyn_var_exp(_tmp7E3,0),Cyc_Absyn_structmember_exp(path,f,0),
_tmp7DA,succ_lab,fail_lab,decls);nv=_tmp7E9.f1;decls=_tmp7E9.f2;{struct Cyc_Absyn_Exp*
_tmp7EA=Cyc_Absyn_structmember_exp(r,f,0);if(Cyc_Toc_is_void_star((void*)((
struct Cyc_Absyn_Structfield*)_check_null(Cyc_Absyn_lookup_field((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp7AE->fields))->v,f)))->type)){_tmp7EA=Cyc_Absyn_cast_exp(
_tmp7E5,_tmp7EA,0);}{struct Cyc_Absyn_Stmt*_tmp7EB=_tmp7E9.f3;struct Cyc_Absyn_Stmt*
_tmp7EC=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7E3,0),
_tmp7EA,0),_tmp7EB,0);ss=({struct Cyc_List_List*_tmp7ED=_cycalloc(sizeof(struct
Cyc_List_List));_tmp7ED->hd=Cyc_Absyn_label_stmt(_tmp7E6,_tmp7EC,0);_tmp7ED->tl=
ss;_tmp7ED;});succ_lab=_tmp7E6;}}}}}}}if(ss == 0){ss=({struct Cyc_List_List*
_tmp7EE=_cycalloc(sizeof(struct Cyc_List_List));_tmp7EE->hd=Cyc_Absyn_goto_stmt(
succ_lab,0);_tmp7EE->tl=0;_tmp7EE;});}s=Cyc_Absyn_seq_stmts(ss,0);goto _LL1874;}
_LL1904: {struct _tuple0*_tmp7EF=Cyc_Toc_temp_var();void*_tmp7F0=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp7B0->topt))->v;decls=({struct Cyc_List_List*_tmp7F1=
_region_malloc(rgn,sizeof(struct Cyc_List_List));_tmp7F1->hd=({struct _tuple12*
_tmp7F2=_region_malloc(rgn,sizeof(struct _tuple12));_tmp7F2->f1=_tmp7EF;_tmp7F2->f2=
Cyc_Toc_typ_to_c(_tmp7F0);_tmp7F2;});_tmp7F1->tl=decls;_tmp7F1;});{struct
_tuple11 _tmp7F3=Cyc_Toc_xlate_pat(nv,rgn,_tmp7F0,Cyc_Absyn_var_exp(_tmp7EF,0),
Cyc_Absyn_deref_exp(path,0),_tmp7B0,succ_lab,fail_lab,decls);nv=_tmp7F3.f1;decls=
_tmp7F3.f2;{struct Cyc_Absyn_Stmt*_tmp7F4=_tmp7F3.f3;struct Cyc_Absyn_Stmt*_tmp7F5=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7EF,0),Cyc_Absyn_deref_exp(
r,0),0),_tmp7F4,0);if(Cyc_Toc_is_nullable(t)){s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(
r,Cyc_Absyn_signed_int_exp(0,0),0),Cyc_Absyn_goto_stmt(fail_lab,0),_tmp7F5,0);}
else{s=_tmp7F5;}goto _LL1874;}}}_LL1906: s=({void*_tmp7F6[0]={};((struct Cyc_Absyn_Stmt*(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unknownid",
sizeof(unsigned char),10),_tag_arr(_tmp7F6,sizeof(void*),0));});goto _LL1874;
_LL1908: s=({void*_tmp7F7[0]={};((struct Cyc_Absyn_Stmt*(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unknowncall",sizeof(
unsigned char),12),_tag_arr(_tmp7F7,sizeof(void*),0));});goto _LL1874;_LL1910: s=({
void*_tmp7F8[0]={};((struct Cyc_Absyn_Stmt*(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("unknownfields",sizeof(unsigned char),
14),_tag_arr(_tmp7F8,sizeof(void*),0));});goto _LL1874;_LL1874:;}return({struct
_tuple11 _tmp7F9;_tmp7F9.f1=nv;_tmp7F9.f2=decls;_tmp7F9.f3=s;_tmp7F9;});}struct
_tuple14{struct _tagged_arr*f1;struct _tagged_arr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple14*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple14*_tmp7FA=_region_malloc(r,sizeof(struct _tuple14));
_tmp7FA->f1=Cyc_Toc_fresh_label();_tmp7FA->f2=Cyc_Toc_fresh_label();_tmp7FA->f3=
sc;_tmp7FA;});}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7FB=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int
leave_as_switch;{void*_tmp7FC=Cyc_Tcutil_compress(_tmp7FB);_LL2046: if((
unsigned int)_tmp7FC > 3?*((int*)_tmp7FC)== Cyc_Absyn_IntType: 0){goto _LL2047;}
else{goto _LL2048;}_LL2048: if((unsigned int)_tmp7FC > 3?*((int*)_tmp7FC)== Cyc_Absyn_EnumType:
0){goto _LL2049;}else{goto _LL2050;}_LL2050: goto _LL2051;_LL2047: goto _LL2049;
_LL2049: leave_as_switch=1;goto _LL2045;_LL2051: leave_as_switch=0;goto _LL2045;
_LL2045:;}{struct Cyc_List_List*_tmp804=scs;for(0;_tmp804 != 0;_tmp804=_tmp804->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)
_tmp804->hd)->pat_vars))->v != 0? 1:((struct Cyc_Absyn_Switch_clause*)_tmp804->hd)->where_clause
!= 0){leave_as_switch=0;break;}}}if(leave_as_switch){struct _tagged_arr*next_l=
Cyc_Toc_fresh_label();{struct Cyc_List_List*_tmp805=scs;for(0;_tmp805 != 0;_tmp805=
_tmp805->tl){struct Cyc_Absyn_Stmt*_tmp806=((struct Cyc_Absyn_Switch_clause*)
_tmp805->hd)->body;((struct Cyc_Absyn_Switch_clause*)_tmp805->hd)->body=Cyc_Absyn_label_stmt(
next_l,_tmp806,0);next_l=Cyc_Toc_fresh_label();Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(
nv,next_l),_tmp806);}}return;}{struct _tuple0*v=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);struct
_tagged_arr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle _tmp807=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp807;_push_region(rgn);{struct Cyc_List_List*lscs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;{struct Cyc_List_List*
_tmp808=lscs;for(0;_tmp808 != 0;_tmp808=_tmp808->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple14*)_tmp808->hd)).f3;struct _tagged_arr*fail_lab=_tmp808->tl == 
0? end_l:(*((struct _tuple14*)((struct Cyc_List_List*)_check_null(_tmp808->tl))->hd)).f1;
struct _tagged_arr*succ_lab=(*((struct _tuple14*)_tmp808->hd)).f2;if(sc->where_clause
== 0){struct _tuple11 _tmp809=Cyc_Toc_xlate_pat(nv,rgn,_tmp7FB,r,path,sc->pattern,
succ_lab,fail_lab,decls);nvs=({struct Cyc_List_List*_tmp80A=_region_malloc(rgn,
sizeof(struct Cyc_List_List));_tmp80A->hd=_tmp809.f1;_tmp80A->tl=nvs;_tmp80A;});
decls=_tmp809.f2;test_stmts=({struct Cyc_List_List*_tmp80B=_region_malloc(rgn,
sizeof(struct Cyc_List_List));_tmp80B->hd=Cyc_Absyn_label_stmt((*((struct _tuple14*)
_tmp808->hd)).f1,_tmp809.f3,0);_tmp80B->tl=test_stmts;_tmp80B;});}else{struct Cyc_Absyn_Exp*
_tmp80C=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);struct _tagged_arr*
_tmp80D=Cyc_Toc_fresh_label();struct _tuple11 _tmp80E=Cyc_Toc_xlate_pat(nv,rgn,
_tmp7FB,r,path,sc->pattern,_tmp80D,fail_lab,decls);Cyc_Toc_exp_to_c(_tmp80E.f1,
_tmp80C);nvs=({struct Cyc_List_List*_tmp80F=_region_malloc(rgn,sizeof(struct Cyc_List_List));
_tmp80F->hd=_tmp80E.f1;_tmp80F->tl=nvs;_tmp80F;});decls=_tmp80E.f2;{struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_ifthenelse_stmt(_tmp80C,Cyc_Absyn_goto_stmt(succ_lab,0),Cyc_Absyn_goto_stmt(
fail_lab,0),0);struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_label_stmt(_tmp80D,s2,0);
test_stmts=({struct Cyc_List_List*_tmp810=_region_malloc(rgn,sizeof(struct Cyc_List_List));
_tmp810->hd=Cyc_Absyn_label_stmt((*((struct _tuple14*)_tmp808->hd)).f1,Cyc_Absyn_seq_stmt(
_tmp80E.f3,s3,0),0);_tmp810->tl=test_stmts;_tmp810;});}}}}{struct Cyc_Absyn_Stmt*
_tmp811=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
test_stmts),0);nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
nvs);{struct Cyc_Absyn_Stmt*clauses=Cyc_Absyn_skip_stmt(0);if(lscs != 0){for(0;
lscs->tl != 0;(lscs=lscs->tl,nvs=nvs->tl)){struct Cyc_Toc_Env*_tmp812=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=((*((struct
_tuple14*)lscs->hd)).f3)->body;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
_tmp812,end_l,(*((struct _tuple14*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd)).f2,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((*((struct _tuple14*)((
struct Cyc_List_List*)_check_null(lscs->tl))->hd)).f3)->pat_vars))->v,(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);clauses=Cyc_Absyn_seq_stmt(
clauses,Cyc_Absyn_label_stmt((*((struct _tuple14*)lscs->hd)).f2,s,0),0);}{struct
Cyc_Toc_Env*_tmp813=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=((*((struct _tuple14*)lscs->hd)).f3)->body;Cyc_Toc_stmt_to_c(
Cyc_Toc_last_switchclause_env(_tmp813,end_l),s);clauses=Cyc_Absyn_seq_stmt(
clauses,Cyc_Absyn_label_stmt((*((struct _tuple14*)lscs->hd)).f2,s,0),0);}}{struct
Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(_tmp811,Cyc_Absyn_seq_stmt(clauses,Cyc_Absyn_label_stmt(
end_l,Cyc_Absyn_skip_stmt(0),0),0),0);for(decls;decls != 0;decls=decls->tl){
struct _tuple12 _tmp816;void*_tmp817;struct _tuple0*_tmp819;struct _tuple12*_tmp814=(
struct _tuple12*)((struct Cyc_List_List*)_check_null(decls))->hd;_tmp816=*_tmp814;
_LL2074: _tmp819=_tmp816.f1;goto _LL2072;_LL2072: _tmp817=_tmp816.f2;goto _LL2069;
_LL2069: res=Cyc_Absyn_declare_stmt(_tmp819,_tmp817,0,res,0);}(void*)(whole_s->r=(
void*)((void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,res,0))->r));}}}};_pop_region(
rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,struct Cyc_Core_Opt*topt,struct Cyc_Absyn_Exp*e,int exhaust,
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp81B=
_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp81B[0]=({struct Cyc_List_List
_tmp81C;_tmp81C.hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp81C.tl=0;
_tmp81C;});_tmp81B;}),0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0){(void*)(s->r=(void*)((void*)(Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
n),Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}}static void Cyc_Toc_stmt_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){while(1){void*_tmp81D=(void*)s->r;
struct Cyc_Absyn_Exp*_tmp849;struct Cyc_Absyn_Stmt*_tmp84B;struct Cyc_Absyn_Stmt*
_tmp84D;struct Cyc_Absyn_Exp*_tmp84F;struct Cyc_Absyn_Stmt*_tmp851;struct Cyc_Absyn_Stmt*
_tmp853;struct Cyc_Absyn_Exp*_tmp855;struct Cyc_Absyn_Stmt*_tmp857;struct _tuple2
_tmp859;struct Cyc_Absyn_Exp*_tmp85B;struct Cyc_Absyn_Stmt*_tmp85D;struct Cyc_Absyn_Stmt*
_tmp85F;struct Cyc_Absyn_Stmt*_tmp861;struct Cyc_Absyn_Stmt*_tmp863;struct _tuple2
_tmp865;struct Cyc_Absyn_Exp*_tmp867;struct _tuple2 _tmp869;struct Cyc_Absyn_Exp*
_tmp86B;struct Cyc_Absyn_Exp*_tmp86D;struct Cyc_List_List*_tmp86F;struct Cyc_Absyn_Exp*
_tmp871;struct Cyc_Absyn_Switch_clause**_tmp873;struct Cyc_List_List*_tmp875;
struct Cyc_Absyn_Stmt*_tmp877;struct Cyc_Absyn_Decl*_tmp879;struct Cyc_Absyn_Stmt*
_tmp87B;struct _tagged_arr*_tmp87D;struct _tuple2 _tmp87F;struct Cyc_Absyn_Exp*
_tmp881;struct Cyc_Absyn_Stmt*_tmp883;struct Cyc_List_List*_tmp885;struct Cyc_Absyn_Stmt*
_tmp887;struct Cyc_Absyn_Stmt*_tmp889;struct Cyc_Absyn_Vardecl*_tmp88B;struct Cyc_Absyn_Tvar*
_tmp88D;_LL2079: if(_tmp81D == (void*)Cyc_Absyn_Skip_s){goto _LL2080;}else{goto
_LL2081;}_LL2081: if((unsigned int)_tmp81D > 1?*((int*)_tmp81D)== Cyc_Absyn_Exp_s:
0){_LL2122: _tmp849=((struct Cyc_Absyn_Exp_s_struct*)_tmp81D)->f1;goto _LL2082;}
else{goto _LL2083;}_LL2083: if((unsigned int)_tmp81D > 1?*((int*)_tmp81D)== Cyc_Absyn_Seq_s:
0){_LL2126: _tmp84D=((struct Cyc_Absyn_Seq_s_struct*)_tmp81D)->f1;goto _LL2124;
_LL2124: _tmp84B=((struct Cyc_Absyn_Seq_s_struct*)_tmp81D)->f2;goto _LL2084;}else{
goto _LL2085;}_LL2085: if((unsigned int)_tmp81D > 1?*((int*)_tmp81D)== Cyc_Absyn_Return_s:
0){_LL2128: _tmp84F=((struct Cyc_Absyn_Return_s_struct*)_tmp81D)->f1;goto _LL2086;}
else{goto _LL2087;}_LL2087: if((unsigned int)_tmp81D > 1?*((int*)_tmp81D)== Cyc_Absyn_IfThenElse_s:
0){_LL2134: _tmp855=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp81D)->f1;goto
_LL2132;_LL2132: _tmp853=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp81D)->f2;goto
_LL2130;_LL2130: _tmp851=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp81D)->f3;goto
_LL2088;}else{goto _LL2089;}_LL2089: if((unsigned int)_tmp81D > 1?*((int*)_tmp81D)
== Cyc_Absyn_While_s: 0){_LL2138: _tmp859=((struct Cyc_Absyn_While_s_struct*)
_tmp81D)->f1;_LL2140: _tmp85B=_tmp859.f1;goto _LL2136;_LL2136: _tmp857=((struct Cyc_Absyn_While_s_struct*)
_tmp81D)->f2;goto _LL2090;}else{goto _LL2091;}_LL2091: if((unsigned int)_tmp81D > 1?*((
int*)_tmp81D)== Cyc_Absyn_Break_s: 0){_LL2142: _tmp85D=((struct Cyc_Absyn_Break_s_struct*)
_tmp81D)->f1;goto _LL2092;}else{goto _LL2093;}_LL2093: if((unsigned int)_tmp81D > 1?*((
int*)_tmp81D)== Cyc_Absyn_Continue_s: 0){_LL2144: _tmp85F=((struct Cyc_Absyn_Continue_s_struct*)
_tmp81D)->f1;goto _LL2094;}else{goto _LL2095;}_LL2095: if((unsigned int)_tmp81D > 1?*((
int*)_tmp81D)== Cyc_Absyn_Goto_s: 0){_LL2146: _tmp861=((struct Cyc_Absyn_Goto_s_struct*)
_tmp81D)->f2;goto _LL2096;}else{goto _LL2097;}_LL2097: if((unsigned int)_tmp81D > 1?*((
int*)_tmp81D)== Cyc_Absyn_For_s: 0){_LL2158: _tmp86D=((struct Cyc_Absyn_For_s_struct*)
_tmp81D)->f1;goto _LL2154;_LL2154: _tmp869=((struct Cyc_Absyn_For_s_struct*)_tmp81D)->f2;
_LL2156: _tmp86B=_tmp869.f1;goto _LL2150;_LL2150: _tmp865=((struct Cyc_Absyn_For_s_struct*)
_tmp81D)->f3;_LL2152: _tmp867=_tmp865.f1;goto _LL2148;_LL2148: _tmp863=((struct Cyc_Absyn_For_s_struct*)
_tmp81D)->f4;goto _LL2098;}else{goto _LL2099;}_LL2099: if((unsigned int)_tmp81D > 1?*((
int*)_tmp81D)== Cyc_Absyn_ForArray_s: 0){goto _LL2100;}else{goto _LL2101;}_LL2101:
if((unsigned int)_tmp81D > 1?*((int*)_tmp81D)== Cyc_Absyn_Switch_s: 0){_LL2162:
_tmp871=((struct Cyc_Absyn_Switch_s_struct*)_tmp81D)->f1;goto _LL2160;_LL2160:
_tmp86F=((struct Cyc_Absyn_Switch_s_struct*)_tmp81D)->f2;goto _LL2102;}else{goto
_LL2103;}_LL2103: if((unsigned int)_tmp81D > 1?*((int*)_tmp81D)== Cyc_Absyn_Fallthru_s:
0){_LL2166: _tmp875=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp81D)->f1;goto _LL2164;
_LL2164: _tmp873=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp81D)->f2;goto _LL2104;}
else{goto _LL2105;}_LL2105: if((unsigned int)_tmp81D > 1?*((int*)_tmp81D)== Cyc_Absyn_Decl_s:
0){_LL2170: _tmp879=((struct Cyc_Absyn_Decl_s_struct*)_tmp81D)->f1;goto _LL2168;
_LL2168: _tmp877=((struct Cyc_Absyn_Decl_s_struct*)_tmp81D)->f2;goto _LL2106;}else{
goto _LL2107;}_LL2107: if((unsigned int)_tmp81D > 1?*((int*)_tmp81D)== Cyc_Absyn_Label_s:
0){_LL2174: _tmp87D=((struct Cyc_Absyn_Label_s_struct*)_tmp81D)->f1;goto _LL2172;
_LL2172: _tmp87B=((struct Cyc_Absyn_Label_s_struct*)_tmp81D)->f2;goto _LL2108;}
else{goto _LL2109;}_LL2109: if((unsigned int)_tmp81D > 1?*((int*)_tmp81D)== Cyc_Absyn_Do_s:
0){_LL2180: _tmp883=((struct Cyc_Absyn_Do_s_struct*)_tmp81D)->f1;goto _LL2176;
_LL2176: _tmp87F=((struct Cyc_Absyn_Do_s_struct*)_tmp81D)->f2;_LL2178: _tmp881=
_tmp87F.f1;goto _LL2110;}else{goto _LL2111;}_LL2111: if((unsigned int)_tmp81D > 1?*((
int*)_tmp81D)== Cyc_Absyn_TryCatch_s: 0){_LL2184: _tmp887=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp81D)->f1;goto _LL2182;_LL2182: _tmp885=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp81D)->f2;goto _LL2112;}else{goto _LL2113;}_LL2113: if((unsigned int)_tmp81D > 1?*((
int*)_tmp81D)== Cyc_Absyn_Region_s: 0){_LL2190: _tmp88D=((struct Cyc_Absyn_Region_s_struct*)
_tmp81D)->f1;goto _LL2188;_LL2188: _tmp88B=((struct Cyc_Absyn_Region_s_struct*)
_tmp81D)->f2;goto _LL2186;_LL2186: _tmp889=((struct Cyc_Absyn_Region_s_struct*)
_tmp81D)->f3;goto _LL2114;}else{goto _LL2115;}_LL2115: if((unsigned int)_tmp81D > 1?*((
int*)_tmp81D)== Cyc_Absyn_SwitchC_s: 0){goto _LL2116;}else{goto _LL2117;}_LL2117:
if((unsigned int)_tmp81D > 1?*((int*)_tmp81D)== Cyc_Absyn_Cut_s: 0){goto _LL2118;}
else{goto _LL2119;}_LL2119: if((unsigned int)_tmp81D > 1?*((int*)_tmp81D)== Cyc_Absyn_Splice_s:
0){goto _LL2120;}else{goto _LL2078;}_LL2080: return;_LL2082: Cyc_Toc_exp_to_c(nv,
_tmp849);return;_LL2084: Cyc_Toc_stmt_to_c(nv,_tmp84D);s=_tmp84B;continue;_LL2086: {
struct Cyc_Core_Opt*topt=0;if(_tmp84F != 0){topt=({struct Cyc_Core_Opt*_tmp88F=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp88F->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp84F->topt))->v);_tmp88F;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp84F));}if(s->try_depth > 0){if(topt != 0){
struct _tuple0*_tmp890=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp891=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp890,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp890,(void*)topt->v,_tmp84F,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp891,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL2088: Cyc_Toc_exp_to_c(nv,_tmp855);Cyc_Toc_stmt_to_c(nv,_tmp853);s=
_tmp851;continue;_LL2090: Cyc_Toc_exp_to_c(nv,_tmp85B);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
nv),_tmp857);return;_LL2092: if(nv->break_lab != 0){(void*)(s->r=(void*)((void*)(
Cyc_Absyn_goto_stmt((struct _tagged_arr*)((struct Cyc_Core_Opt*)_check_null(nv->break_lab))->v,
0))->r));}{int dest_depth=_tmp85D == 0? 0: _tmp85D->try_depth;Cyc_Toc_do_npop_before(
s->try_depth - dest_depth,s);return;}_LL2094: if(nv->continue_lab != 0){(void*)(s->r=(
void*)((void*)(Cyc_Absyn_goto_stmt((struct _tagged_arr*)((struct Cyc_Core_Opt*)
_check_null(nv->continue_lab))->v,0))->r));}_tmp861=_tmp85F;goto _LL2096;_LL2096:
Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp861))->try_depth,
s);return;_LL2098: Cyc_Toc_exp_to_c(nv,_tmp86D);Cyc_Toc_exp_to_c(nv,_tmp86B);Cyc_Toc_exp_to_c(
nv,_tmp867);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(nv),_tmp863);return;_LL2100:({
void*_tmp892[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("forarray not yet implemented",sizeof(unsigned char),29),_tag_arr(
_tmp892,sizeof(void*),0));});return;_LL2102: Cyc_Toc_xlate_switch(nv,s,_tmp871,
_tmp86F);return;_LL2104: if(nv->fallthru_info == 0){(int)_throw(({void*_tmp893[0]={};
Cyc_Toc_toc_impos(_tag_arr("fallthru in unexpected place",sizeof(unsigned char),
29),_tag_arr(_tmp893,sizeof(void*),0));}));}{struct _tuple6 _tmp896;struct Cyc_Dict_Dict*
_tmp897;struct Cyc_List_List*_tmp899;struct _tagged_arr*_tmp89B;struct _tuple6*
_tmp894=(struct _tuple6*)((struct Cyc_Core_Opt*)_check_null(nv->fallthru_info))->v;
_tmp896=*_tmp894;_LL2204: _tmp89B=_tmp896.f1;goto _LL2202;_LL2202: _tmp899=_tmp896.f2;
goto _LL2200;_LL2200: _tmp897=_tmp896.f3;goto _LL2197;_LL2197: {struct Cyc_Absyn_Stmt*
s2=Cyc_Absyn_goto_stmt(_tmp89B,0);Cyc_Toc_do_npop_before(s->try_depth - ((*((
struct Cyc_Absyn_Switch_clause**)_check_null(_tmp873)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp89D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
_tmp899);struct Cyc_List_List*_tmp89E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(_tmp875);for(0;_tmp89D != 0;(_tmp89D=_tmp89D->tl,_tmp89E=_tmp89E->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp89E))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(
struct Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup)(_tmp897,(struct _tuple0*)
_tmp89D->hd),(struct Cyc_Absyn_Exp*)_tmp89E->hd,0),s2,0);}(void*)(s->r=(void*)((
void*)s2->r));return;}}}_LL2106:{void*_tmp89F=(void*)_tmp879->r;struct Cyc_Absyn_Vardecl*
_tmp8A9;int _tmp8AB;struct Cyc_Absyn_Exp*_tmp8AD;struct Cyc_Core_Opt*_tmp8AF;struct
Cyc_Absyn_Pat*_tmp8B1;struct Cyc_List_List*_tmp8B3;_LL2209: if(*((int*)_tmp89F)== 
Cyc_Absyn_Var_d){_LL2218: _tmp8A9=((struct Cyc_Absyn_Var_d_struct*)_tmp89F)->f1;
goto _LL2210;}else{goto _LL2211;}_LL2211: if(*((int*)_tmp89F)== Cyc_Absyn_Let_d){
_LL2226: _tmp8B1=((struct Cyc_Absyn_Let_d_struct*)_tmp89F)->f1;goto _LL2224;_LL2224:
_tmp8AF=((struct Cyc_Absyn_Let_d_struct*)_tmp89F)->f3;goto _LL2222;_LL2222: _tmp8AD=((
struct Cyc_Absyn_Let_d_struct*)_tmp89F)->f4;goto _LL2220;_LL2220: _tmp8AB=((struct
Cyc_Absyn_Let_d_struct*)_tmp89F)->f5;goto _LL2212;}else{goto _LL2213;}_LL2213: if(*((
int*)_tmp89F)== Cyc_Absyn_Letv_d){_LL2228: _tmp8B3=((struct Cyc_Absyn_Letv_d_struct*)
_tmp89F)->f1;goto _LL2214;}else{goto _LL2215;}_LL2215: goto _LL2216;_LL2210: {struct
Cyc_Toc_Env*_tmp8B5=Cyc_Toc_add_varmap(nv,_tmp8A9->name,Cyc_Absyn_varb_exp(
_tmp8A9->name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp8B6=_cycalloc(sizeof(
struct Cyc_Absyn_Local_b_struct));_tmp8B6[0]=({struct Cyc_Absyn_Local_b_struct
_tmp8B7;_tmp8B7.tag=Cyc_Absyn_Local_b;_tmp8B7.f1=_tmp8A9;_tmp8B7;});_tmp8B6;}),0));
Cyc_Toc_local_decl_to_c(_tmp8B5,_tmp8B5,_tmp8A9,_tmp877);goto _LL2208;}_LL2212:{
void*_tmp8B8=(void*)_tmp8B1->r;struct Cyc_Absyn_Vardecl*_tmp8BE;_LL2234: if((
unsigned int)_tmp8B8 > 2?*((int*)_tmp8B8)== Cyc_Absyn_Var_p: 0){_LL2239: _tmp8BE=((
struct Cyc_Absyn_Var_p_struct*)_tmp8B8)->f1;goto _LL2235;}else{goto _LL2236;}
_LL2236: goto _LL2237;_LL2235: {struct _tuple0*old_name=_tmp8BE->name;struct _tuple0*
new_name=Cyc_Toc_temp_var();_tmp8BE->name=new_name;_tmp8BE->initializer=(struct
Cyc_Absyn_Exp*)_tmp8AD;(void*)(_tmp879->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp8C0=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));_tmp8C0[0]=({struct Cyc_Absyn_Var_d_struct
_tmp8C1;_tmp8C1.tag=Cyc_Absyn_Var_d;_tmp8C1.f1=_tmp8BE;_tmp8C1;});_tmp8C0;})));{
struct Cyc_Toc_Env*_tmp8C2=Cyc_Toc_add_varmap(nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp8C3=_cycalloc(sizeof(struct
Cyc_Absyn_Local_b_struct));_tmp8C3[0]=({struct Cyc_Absyn_Local_b_struct _tmp8C4;
_tmp8C4.tag=Cyc_Absyn_Local_b;_tmp8C4.f1=_tmp8BE;_tmp8C4;});_tmp8C3;}),0));Cyc_Toc_local_decl_to_c(
_tmp8C2,nv,_tmp8BE,_tmp877);goto _LL2233;}}_LL2237:(void*)(s->r=(void*)((void*)(
Cyc_Toc_letdecl_to_c(nv,_tmp8B1,_tmp8AF,_tmp8AD,_tmp8AB,_tmp877))->r));goto
_LL2233;_LL2233:;}goto _LL2208;_LL2214: {struct Cyc_List_List*_tmp8C5=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp8B3);if(_tmp8C5 == 0){({void*_tmp8C6[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("empty Letv_d",
sizeof(unsigned char),13),_tag_arr(_tmp8C6,sizeof(void*),0));});}(void*)(_tmp879->r=(
void*)((void*)({struct Cyc_Absyn_Var_d_struct*_tmp8C7=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));
_tmp8C7[0]=({struct Cyc_Absyn_Var_d_struct _tmp8C8;_tmp8C8.tag=Cyc_Absyn_Var_d;
_tmp8C8.f1=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(_tmp8C5))->hd;
_tmp8C8;});_tmp8C7;})));_tmp8C5=_tmp8C5->tl;for(0;_tmp8C5 != 0;_tmp8C5=_tmp8C5->tl){
struct Cyc_Absyn_Decl*_tmp8C9=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp8CA=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));_tmp8CA[0]=({struct Cyc_Absyn_Var_d_struct
_tmp8CB;_tmp8CB.tag=Cyc_Absyn_Var_d;_tmp8CB.f1=(struct Cyc_Absyn_Vardecl*)_tmp8C5->hd;
_tmp8CB;});_tmp8CA;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp8C9,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto
_LL2208;}_LL2216:({void*_tmp8CC[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bad nested declaration within function",
sizeof(unsigned char),39),_tag_arr(_tmp8CC,sizeof(void*),0));});goto _LL2208;
_LL2208:;}return;_LL2108: s=_tmp87B;continue;_LL2110: Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(
nv),_tmp883);Cyc_Toc_exp_to_c(nv,_tmp881);return;_LL2112: {struct _tuple0*h_var=
Cyc_Toc_temp_var();struct _tuple0*e_var=Cyc_Toc_temp_var();struct _tuple0*
was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_t);e_exp->topt=({struct Cyc_Core_Opt*
_tmp8CD=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp8CD->v=(void*)e_typ;_tmp8CD;});
nv=Cyc_Toc_add_varmap(nv,e_var,e_exp);Cyc_Toc_stmt_to_c(nv,_tmp887);{struct Cyc_Absyn_Stmt*
_tmp8CE=Cyc_Absyn_seq_stmt(_tmp887,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple0*_tmp8CF=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp8D0=
Cyc_Absyn_var_exp(_tmp8CF,0);struct Cyc_Absyn_Vardecl*_tmp8D1=Cyc_Absyn_new_vardecl(
_tmp8CF,Cyc_Absyn_exn_typ,0);_tmp8D0->topt=({struct Cyc_Core_Opt*_tmp8D2=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp8D2->v=(void*)Cyc_Absyn_exn_typ;
_tmp8D2;});{struct Cyc_Absyn_Pat*_tmp8D3=({struct Cyc_Absyn_Pat*_tmp8E3=_cycalloc(
sizeof(struct Cyc_Absyn_Pat));_tmp8E3->r=(void*)((void*)({struct Cyc_Absyn_Var_p_struct*
_tmp8E5=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_struct));_tmp8E5[0]=({struct Cyc_Absyn_Var_p_struct
_tmp8E6;_tmp8E6.tag=Cyc_Absyn_Var_p;_tmp8E6.f1=_tmp8D1;_tmp8E6;});_tmp8E5;}));
_tmp8E3->topt=({struct Cyc_Core_Opt*_tmp8E4=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp8E4->v=(void*)Cyc_Absyn_exn_typ;_tmp8E4;});_tmp8E3->loc=0;_tmp8E3;});struct
Cyc_Absyn_Exp*_tmp8D4=Cyc_Absyn_throw_exp(_tmp8D0,0);_tmp8D4->topt=({struct Cyc_Core_Opt*
_tmp8D5=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp8D5->v=(void*)((void*)Cyc_Absyn_VoidType);
_tmp8D5;});{struct Cyc_Absyn_Stmt*_tmp8D6=Cyc_Absyn_exp_stmt(_tmp8D4,0);struct Cyc_Absyn_Switch_clause*
_tmp8D7=({struct Cyc_Absyn_Switch_clause*_tmp8E0=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
_tmp8E0->pattern=_tmp8D3;_tmp8E0->pat_vars=({struct Cyc_Core_Opt*_tmp8E1=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp8E1->v=({struct Cyc_List_List*_tmp8E2=
_cycalloc(sizeof(struct Cyc_List_List));_tmp8E2->hd=_tmp8D1;_tmp8E2->tl=0;_tmp8E2;});
_tmp8E1;});_tmp8E0->where_clause=0;_tmp8E0->body=_tmp8D6;_tmp8E0->loc=0;_tmp8E0;});
struct Cyc_Absyn_Stmt*_tmp8D8=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp885,({struct
Cyc_List_List*_tmp8DF=_cycalloc(sizeof(struct Cyc_List_List));_tmp8DF->hd=_tmp8D7;
_tmp8DF->tl=0;_tmp8DF;})),0);Cyc_Toc_stmt_to_c(nv,_tmp8D8);{struct Cyc_Absyn_Exp*
_tmp8D9=Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp8DE=
_cycalloc(sizeof(struct Cyc_List_List));_tmp8DE->hd=Cyc_Absyn_structmember_exp(
h_exp,Cyc_Toc_handler_sp,0);_tmp8DE->tl=0;_tmp8DE;}),0);struct Cyc_Absyn_Stmt*
_tmp8DA=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct
Cyc_List_List*_tmp8DD=_cycalloc(sizeof(struct Cyc_List_List));_tmp8DD->hd=Cyc_Absyn_address_exp(
h_exp,0);_tmp8DD->tl=0;_tmp8DD;}),0),0);struct Cyc_Absyn_Exp*_tmp8DB=Cyc_Absyn_int_exp((
void*)Cyc_Absyn_Signed,0,0);struct Cyc_Absyn_Exp*_tmp8DC=Cyc_Absyn_int_exp((void*)
Cyc_Absyn_Signed,1,0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(h_var,
h_typ,0,Cyc_Absyn_seq_stmt(_tmp8DA,Cyc_Absyn_declare_stmt(was_thrown_var,
was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp8DB,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
_tmp8D9,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8DC,0),Cyc_Absyn_skip_stmt(0),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)Cyc_Absyn_Not,
was_thrown_exp,0),_tmp8CE,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_cast_exp(e_typ,Cyc_Toc__exn_thrown_e,0),_tmp8D8,0),0),0),0),0),0))->r));
return;}}}}}_LL2114: {void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct
_tuple0*rh_var=Cyc_Toc_temp_var();struct _tuple0*x_var=_tmp88B->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(nv,x_var,x_exp),_tmp889);(void*)(s->r=(
void*)((void*)(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({struct Cyc_List_List*_tmp8E7=
_cycalloc(sizeof(struct Cyc_List_List));_tmp8E7->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(
x_var),0);_tmp8E7->tl=0;_tmp8E7;}),0),Cyc_Absyn_declare_stmt(x_var,
rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({struct Cyc_Absyn_Exp*
_tmp8E8[1];_tmp8E8[0]=x_exp;((struct Cyc_List_List*(*)(struct _tagged_arr))Cyc_List_list)(
_tag_arr(_tmp8E8,sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),Cyc_Absyn_seq_stmt(
_tmp889,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,({struct
Cyc_Absyn_Exp*_tmp8E9[1];_tmp8E9[0]=x_exp;((struct Cyc_List_List*(*)(struct
_tagged_arr))Cyc_List_list)(_tag_arr(_tmp8E9,sizeof(struct Cyc_Absyn_Exp*),1));}),
0),0),0),0),0),0))->r));return;}_LL2116:({void*_tmp8EA[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_toc_impos)(_tag_arr("switchC_s",
sizeof(unsigned char),10),_tag_arr(_tmp8EA,sizeof(void*),0));});return;_LL2118:({
void*_tmp8EB[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Toc_unimp)(
_tag_arr("cut",sizeof(unsigned char),4),_tag_arr(_tmp8EB,sizeof(void*),0));});
return;_LL2120:({void*_tmp8EC[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Toc_unimp)(_tag_arr("splice",sizeof(unsigned char),7),
_tag_arr(_tmp8EC,sizeof(void*),0));});return;_LL2078:;}}struct _tuple15{struct
_tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f){f->tvs=0;f->effect=0;f->rgn_po=0;(
void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((void*)f->ret_type));{struct Cyc_List_List*
_tmp8ED=f->args;for(0;_tmp8ED != 0;_tmp8ED=_tmp8ED->tl){(*((struct _tuple15*)
_tmp8ED->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple15*)_tmp8ED->hd)).f3);{struct
_tuple0*_tmp8EE=({struct _tuple0*_tmp8EF=_cycalloc(sizeof(struct _tuple0));_tmp8EF->f1=(
void*)Cyc_Absyn_Loc_n;_tmp8EF->f2=(*((struct _tuple15*)_tmp8ED->hd)).f1;_tmp8EF;});
nv=Cyc_Toc_add_varmap(nv,_tmp8EE,Cyc_Absyn_var_exp(_tmp8EE,0));}}}if((
unsigned int)f->cyc_varargs?((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name
!= 0: 0){int _tmp8F2;void*_tmp8F4;struct Cyc_Absyn_Tqual _tmp8F6;struct Cyc_Core_Opt*
_tmp8F8;struct Cyc_Absyn_VarargInfo _tmp8F0=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_LL2297: _tmp8F8=_tmp8F0.name;goto _LL2295;_LL2295:
_tmp8F6=_tmp8F0.tq;goto _LL2293;_LL2293: _tmp8F4=(void*)_tmp8F0.type;goto _LL2291;
_LL2291: _tmp8F2=_tmp8F0.inject;goto _LL2289;_LL2289: {void*_tmp8FA=Cyc_Toc_typ_to_c(
Cyc_Absyn_tagged_typ(_tmp8F4,(void*)Cyc_Absyn_HeapRgn,_tmp8F6));struct _tuple0*
_tmp8FB=({struct _tuple0*_tmp8FE=_cycalloc(sizeof(struct _tuple0));_tmp8FE->f1=(
void*)Cyc_Absyn_Loc_n;_tmp8FE->f2=(struct _tagged_arr*)((struct Cyc_Core_Opt*)
_check_null(_tmp8F8))->v;_tmp8FE;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp8FC=
_cycalloc(sizeof(struct Cyc_List_List));_tmp8FC->hd=({struct _tuple15*_tmp8FD=
_cycalloc(sizeof(struct _tuple15));_tmp8FD->f1=(struct _tagged_arr*)_tmp8F8->v;
_tmp8FD->f2=_tmp8F6;_tmp8FD->f3=_tmp8FA;_tmp8FD;});_tmp8FC->tl=0;_tmp8FC;}));nv=
Cyc_Toc_add_varmap(nv,_tmp8FB,Cyc_Absyn_var_exp(_tmp8FB,0));f->cyc_varargs=0;}}{
struct Cyc_List_List*_tmp8FF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(f->param_vardecls))->v;for(0;_tmp8FF != 0;_tmp8FF=_tmp8FF->tl){(void*)(((
struct Cyc_Absyn_Vardecl*)_tmp8FF->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Absyn_Vardecl*)_tmp8FF->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(
nv),f->body);}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp900=s;_LL2306: if(
_tmp900 == (void*)Cyc_Absyn_Abstract){goto _LL2307;}else{goto _LL2308;}_LL2308: if(
_tmp900 == (void*)Cyc_Absyn_ExternC){goto _LL2309;}else{goto _LL2310;}_LL2310: goto
_LL2311;_LL2307: return(void*)Cyc_Absyn_Public;_LL2309: return(void*)Cyc_Absyn_Extern;
_LL2311: return s;_LL2305:;}struct Cyc_Core_Opt*Cyc_Toc_structs_so_far=0;static void
Cyc_Toc_structdecl_to_c(struct Cyc_Absyn_Structdecl*s){struct _tuple0*_tmp908=(
struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(s->name))->v;if(Cyc_Toc_structs_so_far
== 0){Cyc_Toc_structs_so_far=({struct Cyc_Core_Opt*_tmp909=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmp909->v=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp909;});}{
int seen_defn_before;struct Cyc_Core_Opt*_tmp90A=((struct Cyc_Core_Opt*(*)(struct
Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_structs_so_far))->v,(*_tmp908).f2);if(
_tmp90A == 0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_structs_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct
Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Structdecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_structs_so_far))->v,(*
_tmp908).f2,s));}else{if(((struct Cyc_Absyn_Structdecl*)_tmp90A->v)->fields == 0){(
struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_structs_so_far))->v=(
void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct
Cyc_Absyn_Structdecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_structs_so_far))->v,(*_tmp908).f2,s));seen_defn_before=0;}
else{seen_defn_before=1;}}(void*)(s->sc=(void*)((void*)Cyc_Absyn_Public));s->tvs=
0;s->name=({struct Cyc_Core_Opt*_tmp90B=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp90B->v=_tmp908;_tmp90B;});if(s->fields != 0){if(seen_defn_before){s->fields=0;}
else{struct Cyc_List_List*_tmp90C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(s->fields))->v;for(0;_tmp90C != 0;_tmp90C=_tmp90C->tl){(void*)(((
struct Cyc_Absyn_Structfield*)_tmp90C->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Structfield*)_tmp90C->hd)->type));}}}}}struct Cyc_Core_Opt*
Cyc_Toc_unions_so_far=0;static void Cyc_Toc_uniondecl_to_c(struct Cyc_Absyn_Uniondecl*
u){struct _tuple0*_tmp90D=(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(u->name))->v;
if(Cyc_Toc_unions_so_far == 0){Cyc_Toc_unions_so_far=({struct Cyc_Core_Opt*_tmp90E=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp90E->v=((struct Cyc_Dict_Dict*(*)(int(*
cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);
_tmp90E;});}{int seen_defn_before;struct Cyc_Core_Opt*_tmp90F=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup_opt)((struct Cyc_Dict_Dict*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_unions_so_far))->v,(*_tmp90D).f2);if(
_tmp90F == 0){seen_defn_before=0;(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_unions_so_far))->v=(void*)((struct Cyc_Dict_Dict*(*)(struct
Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Uniondecl*v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_unions_so_far))->v,(*
_tmp90D).f2,u));}else{if(((struct Cyc_Absyn_Uniondecl*)_tmp90F->v)->fields == 0){(
struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_unions_so_far))->v=(
void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct
Cyc_Absyn_Uniondecl*v))Cyc_Dict_insert)((struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Toc_unions_so_far))->v,(*_tmp90D).f2,u));seen_defn_before=0;}
else{seen_defn_before=1;}}(void*)(u->sc=(void*)((void*)Cyc_Absyn_Public));u->tvs=
0;u->name=({struct Cyc_Core_Opt*_tmp910=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp910->v=_tmp90D;_tmp910;});if(u->fields != 0){if(seen_defn_before){u->fields=0;}
else{struct Cyc_List_List*_tmp911=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(u->fields))->v;for(0;_tmp911 != 0;_tmp911=_tmp911->tl){(void*)(((
struct Cyc_Absyn_Structfield*)_tmp911->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Structfield*)_tmp911->hd)->type));}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_tunions_so_far=0;static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
tud){if(Cyc_Toc_tunions_so_far == 0){Cyc_Toc_tunions_so_far=({struct Cyc_Core_Opt*
_tmp912=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp912->v=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Set_empty)(Cyc_Absyn_qvar_cmp);
_tmp912;});}{struct _tuple0*_tmp913=tud->name;if(tud->fields == 0){return;}if(((
int(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_member)((struct Cyc_Set_Set*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,_tmp913)){return;}(
struct Cyc_Set_Set*)(((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v=(
void*)((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _tuple0*elt))Cyc_Set_insert)((
struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_tunions_so_far))->v,
_tmp913));{int tag_count=0;int box_count=0;struct Cyc_List_List*_tmp914=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp914 != 0;_tmp914=
_tmp914->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)_tmp914->hd;
if(f->typs == 0){struct Cyc_Absyn_Exp*eopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_signed_int_exp(
tag_count ++,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(f->name,Cyc_Absyn_sint_t,
eopt);vd->tq=({struct Cyc_Absyn_Tqual _tmp915;_tmp915.q_const=1;_tmp915.q_volatile=
0;_tmp915.q_restrict=0;_tmp915;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp916=_cycalloc(sizeof(struct Cyc_List_List));_tmp916->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp917=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));
_tmp917[0]=({struct Cyc_Absyn_Var_d_struct _tmp918;_tmp918.tag=Cyc_Absyn_Var_d;
_tmp918.f1=vd;_tmp918;});_tmp917;}),0);_tmp916->tl=Cyc_Toc_result_decls;_tmp916;});}
else{struct Cyc_Absyn_Exp*eopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_signed_int_exp(
box_count ++,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(f->name,Cyc_Absyn_sint_t,
eopt);vd->tq=({struct Cyc_Absyn_Tqual _tmp919;_tmp919.q_const=1;_tmp919.q_volatile=
0;_tmp919.q_restrict=0;_tmp919;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp91A=_cycalloc(sizeof(struct Cyc_List_List));_tmp91A->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp91B=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));
_tmp91B[0]=({struct Cyc_Absyn_Var_d_struct _tmp91C;_tmp91C.tag=Cyc_Absyn_Var_d;
_tmp91C.f1=vd;_tmp91C;});_tmp91B;}),0);_tmp91A->tl=Cyc_Toc_result_decls;_tmp91A;});{
struct Cyc_List_List*_tmp91D=0;int i=1;{struct Cyc_List_List*_tmp91E=f->typs;for(0;
_tmp91E != 0;(_tmp91E=_tmp91E->tl,i ++)){struct _tagged_arr*_tmp91F=Cyc_Absyn_fieldname(
i);struct Cyc_Absyn_Structfield*_tmp920=({struct Cyc_Absyn_Structfield*_tmp922=
_cycalloc(sizeof(struct Cyc_Absyn_Structfield));_tmp922->name=_tmp91F;_tmp922->tq=(*((
struct _tuple3*)_tmp91E->hd)).f1;_tmp922->type=(void*)Cyc_Toc_typ_to_c_array((*((
struct _tuple3*)_tmp91E->hd)).f2);_tmp922->width=0;_tmp922->attributes=0;_tmp922;});
_tmp91D=({struct Cyc_List_List*_tmp921=_cycalloc(sizeof(struct Cyc_List_List));
_tmp921->hd=_tmp920;_tmp921->tl=_tmp91D;_tmp921;});}}_tmp91D=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp91D);_tmp91D=({struct Cyc_List_List*
_tmp923=_cycalloc(sizeof(struct Cyc_List_List));_tmp923->hd=({struct Cyc_Absyn_Structfield*
_tmp924=_cycalloc(sizeof(struct Cyc_Absyn_Structfield));_tmp924->name=Cyc_Toc_tag_sp;
_tmp924->tq=Cyc_Toc_mt_tq;_tmp924->type=(void*)Cyc_Absyn_sint_t;_tmp924->width=0;
_tmp924->attributes=0;_tmp924;});_tmp923->tl=_tmp91D;_tmp923;});{struct Cyc_Absyn_Structdecl*
_tmp925=({struct Cyc_Absyn_Structdecl*_tmp929=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));
_tmp929->sc=(void*)((void*)Cyc_Absyn_Public);_tmp929->name=({struct Cyc_Core_Opt*
_tmp92B=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp92B->v=Cyc_Toc_collapse_qvar_tag(
f->name,_tag_arr("_struct",sizeof(unsigned char),8));_tmp92B;});_tmp929->tvs=0;
_tmp929->fields=({struct Cyc_Core_Opt*_tmp92A=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp92A->v=_tmp91D;_tmp92A;});_tmp929->attributes=0;_tmp929;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp926=_cycalloc(sizeof(struct Cyc_List_List));_tmp926->hd=
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Struct_d_struct*_tmp927=_cycalloc(
sizeof(struct Cyc_Absyn_Struct_d_struct));_tmp927[0]=({struct Cyc_Absyn_Struct_d_struct
_tmp928;_tmp928.tag=Cyc_Absyn_Struct_d;_tmp928.f1=_tmp925;_tmp928;});_tmp927;}),
0);_tmp926->tl=Cyc_Toc_result_decls;_tmp926;});}}}}}}}static struct Cyc_Core_Opt*
Cyc_Toc_xtunion_fields_so_far=0;static void Cyc_Toc_xtuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
xd){if(Cyc_Toc_xtunion_fields_so_far == 0){Cyc_Toc_xtunion_fields_so_far=({struct
Cyc_Core_Opt*_tmp92C=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp92C->v=((struct
Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp92C;});}if(xd->fields == 0){return;}{struct _tuple0*_tmp92D=xd->name;struct Cyc_List_List*
_tmp92E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(xd->fields))->v;
for(0;_tmp92E != 0;_tmp92E=_tmp92E->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)
_tmp92E->hd;struct _tagged_arr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp92F=Cyc_Absyn_uint_exp(
_get_arr_size(*fn,sizeof(unsigned char))+ 4,0);struct Cyc_Absyn_ArrayType_struct*
_tmp930=({struct Cyc_Absyn_ArrayType_struct*_tmp962=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));
_tmp962[0]=({struct Cyc_Absyn_ArrayType_struct _tmp963;_tmp963.tag=Cyc_Absyn_ArrayType;
_tmp963.f1=(void*)Cyc_Absyn_uchar_t;_tmp963.f2=Cyc_Toc_mt_tq;_tmp963.f3=(struct
Cyc_Absyn_Exp*)_tmp92F;_tmp963;});_tmp962;});struct Cyc_Core_Opt*_tmp931=((struct
Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))Cyc_Dict_lookup_opt)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name);struct Cyc_Core_Opt _tmp939;int _tmp93A;_LL2355: if(_tmp931 == 0){goto
_LL2356;}else{goto _LL2357;}_LL2357: if(_tmp931 == 0){goto _LL2359;}else{_tmp939=*
_tmp931;_LL2363: _tmp93A=(int)_tmp939.v;if(_tmp93A == 0){goto _LL2358;}else{goto
_LL2359;}}_LL2359: goto _LL2360;_LL2356: {struct Cyc_Absyn_Exp*initopt=0;if((void*)
f->sc != (void*)Cyc_Absyn_Extern){initopt=(struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp941;_tmp941.tag=Cyc_Std_String_pa;
_tmp941.f1=(struct _tagged_arr)*fn;{struct Cyc_Std_Int_pa_struct _tmp940;_tmp940.tag=
Cyc_Std_Int_pa;_tmp940.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp93F;_tmp93F.tag=Cyc_Std_Int_pa;_tmp93F.f1=(int)((unsigned int)((int)'\000'));{
struct Cyc_Std_Int_pa_struct _tmp93E;_tmp93E.tag=Cyc_Std_Int_pa;_tmp93E.f1=(int)((
unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct _tmp93D;_tmp93D.tag=Cyc_Std_Int_pa;
_tmp93D.f1=(int)((unsigned int)((int)'\000'));{void*_tmp93C[5]={& _tmp93D,&
_tmp93E,& _tmp93F,& _tmp940,& _tmp941};Cyc_Std_aprintf(_tag_arr("%c%c%c%c%s",
sizeof(unsigned char),11),_tag_arr(_tmp93C,sizeof(void*),5));}}}}}}),0);}{struct
Cyc_Absyn_Vardecl*_tmp942=Cyc_Absyn_new_vardecl(f->name,(void*)_tmp930,initopt);(
void*)(_tmp942->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp943=_cycalloc(sizeof(struct Cyc_List_List));_tmp943->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp944=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));
_tmp944[0]=({struct Cyc_Absyn_Var_d_struct _tmp945;_tmp945.tag=Cyc_Absyn_Var_d;
_tmp945.f1=_tmp942;_tmp945;});_tmp944;}),0);_tmp943->tl=Cyc_Toc_result_decls;
_tmp943;});(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(
void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,(void*)f->sc != (void*)Cyc_Absyn_Extern));if(f->typs != 0){struct Cyc_List_List*
fields=0;int i=1;{struct Cyc_List_List*_tmp946=f->typs;for(0;_tmp946 != 0;(_tmp946=
_tmp946->tl,i ++)){struct _tagged_arr*_tmp947=({struct _tagged_arr*_tmp94B=
_cycalloc(sizeof(struct _tagged_arr));_tmp94B[0]=(struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp94D;_tmp94D.tag=Cyc_Std_Int_pa;_tmp94D.f1=(int)((unsigned int)i);{void*
_tmp94C[1]={& _tmp94D};Cyc_Std_aprintf(_tag_arr("f%d",sizeof(unsigned char),4),
_tag_arr(_tmp94C,sizeof(void*),1));}});_tmp94B;});struct Cyc_Absyn_Structfield*
_tmp948=({struct Cyc_Absyn_Structfield*_tmp94A=_cycalloc(sizeof(struct Cyc_Absyn_Structfield));
_tmp94A->name=_tmp947;_tmp94A->tq=(*((struct _tuple3*)_tmp946->hd)).f1;_tmp94A->type=(
void*)Cyc_Toc_typ_to_c_array((*((struct _tuple3*)_tmp946->hd)).f2);_tmp94A->width=
0;_tmp94A->attributes=0;_tmp94A;});fields=({struct Cyc_List_List*_tmp949=
_cycalloc(sizeof(struct Cyc_List_List));_tmp949->hd=_tmp948;_tmp949->tl=fields;
_tmp949;});}}fields=({struct Cyc_List_List*_tmp94E=_cycalloc(sizeof(struct Cyc_List_List));
_tmp94E->hd=({struct Cyc_Absyn_Structfield*_tmp94F=_cycalloc(sizeof(struct Cyc_Absyn_Structfield));
_tmp94F->name=Cyc_Toc_tag_sp;_tmp94F->tq=Cyc_Toc_mt_tq;_tmp94F->type=(void*)Cyc_Absyn_cstar_typ(
Cyc_Absyn_uchar_t,Cyc_Toc_mt_tq);_tmp94F->width=0;_tmp94F->attributes=0;_tmp94F;});
_tmp94E->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fields);_tmp94E;});{struct Cyc_Absyn_Structdecl*_tmp950=({struct Cyc_Absyn_Structdecl*
_tmp954=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));_tmp954->sc=(void*)((void*)
Cyc_Absyn_Public);_tmp954->name=({struct Cyc_Core_Opt*_tmp956=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmp956->v=Cyc_Toc_collapse_qvar_tag(f->name,_tag_arr("_struct",
sizeof(unsigned char),8));_tmp956;});_tmp954->tvs=0;_tmp954->fields=({struct Cyc_Core_Opt*
_tmp955=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp955->v=fields;_tmp955;});
_tmp954->attributes=0;_tmp954;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp951=_cycalloc(sizeof(struct Cyc_List_List));_tmp951->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Struct_d_struct*_tmp952=_cycalloc(sizeof(struct Cyc_Absyn_Struct_d_struct));
_tmp952[0]=({struct Cyc_Absyn_Struct_d_struct _tmp953;_tmp953.tag=Cyc_Absyn_Struct_d;
_tmp953.f1=_tmp950;_tmp953;});_tmp952;}),0);_tmp951->tl=Cyc_Toc_result_decls;
_tmp951;});}}goto _LL2354;}}_LL2358: if((void*)f->sc != (void*)Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp957=Cyc_Absyn_string_exp((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp961;_tmp961.tag=Cyc_Std_String_pa;_tmp961.f1=(struct _tagged_arr)*fn;{struct
Cyc_Std_Int_pa_struct _tmp960;_tmp960.tag=Cyc_Std_Int_pa;_tmp960.f1=(int)((
unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct _tmp95F;_tmp95F.tag=Cyc_Std_Int_pa;
_tmp95F.f1=(int)((unsigned int)((int)'\000'));{struct Cyc_Std_Int_pa_struct
_tmp95E;_tmp95E.tag=Cyc_Std_Int_pa;_tmp95E.f1=(int)((unsigned int)((int)'\000'));{
struct Cyc_Std_Int_pa_struct _tmp95D;_tmp95D.tag=Cyc_Std_Int_pa;_tmp95D.f1=(int)((
unsigned int)((int)'\000'));{void*_tmp95C[5]={& _tmp95D,& _tmp95E,& _tmp95F,&
_tmp960,& _tmp961};Cyc_Std_aprintf(_tag_arr("%c%c%c%c%s",sizeof(unsigned char),11),
_tag_arr(_tmp95C,sizeof(void*),5));}}}}}}),0);struct Cyc_Absyn_Vardecl*_tmp958=
Cyc_Absyn_new_vardecl(f->name,(void*)_tmp930,(struct Cyc_Absyn_Exp*)_tmp957);(
void*)(_tmp958->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp959=_cycalloc(sizeof(struct Cyc_List_List));_tmp959->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp95A=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));
_tmp95A[0]=({struct Cyc_Absyn_Var_d_struct _tmp95B;_tmp95B.tag=Cyc_Absyn_Var_d;
_tmp95B.f1=_tmp958;_tmp95B;});_tmp95A;}),0);_tmp959->tl=Cyc_Toc_result_decls;
_tmp959;});(struct Cyc_Dict_Dict*)(((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v=(
void*)((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)((
struct Cyc_Dict_Dict*)((struct Cyc_Core_Opt*)_check_null(Cyc_Toc_xtunion_fields_so_far))->v,
f->name,1));}goto _LL2354;_LL2360: goto _LL2354;_LL2354:;}}}static void Cyc_Toc_enumdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Enumdecl*ed){struct _tuple0*_tmp964=ed->name;(
void*)(ed->sc=(void*)((void*)Cyc_Absyn_Public));if(ed->fields != 0){Cyc_Toc_enumfields_to_c((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}}static
void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array((
void*)vd->type));Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*
init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);void*_tmp965=(void*)init->r;
struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_Absyn_Exp*_tmp96D;struct Cyc_Absyn_Vardecl*
_tmp96F;_LL2407: if(*((int*)_tmp965)== Cyc_Absyn_Comprehension_e){_LL2416: _tmp96F=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp965)->f1;goto _LL2414;_LL2414: _tmp96D=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp965)->f2;goto _LL2412;_LL2412: _tmp96B=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp965)->f3;goto _LL2408;}else{goto
_LL2409;}_LL2409: goto _LL2410;_LL2408: vd->initializer=0;(void*)(s->r=(void*)((
void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp96F,
_tmp96D,_tmp96B,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));goto _LL2406;_LL2410:
if((void*)vd->sc == (void*)Cyc_Absyn_Static){struct Cyc_Toc_Env*_tmp971=Cyc_Toc_copy_env(
init_nv);_tmp971->toplevel=1;Cyc_Toc_exp_to_c(_tmp971,init);}else{Cyc_Toc_exp_to_c(
init_nv,init);}goto _LL2406;_LL2406:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,struct Cyc_Core_Opt*topt,struct Cyc_Absyn_Exp*
e,int exhaust,struct Cyc_Absyn_Stmt*s){void*t=(void*)((struct Cyc_Core_Opt*)
_check_null(topt))->v;Cyc_Toc_exp_to_c(nv,e);{struct _tuple0*x=Cyc_Toc_temp_var();
struct _tagged_arr*succ_lab=Cyc_Toc_fresh_label();struct _tagged_arr*fail_lab=
exhaust? succ_lab: Cyc_Toc_fresh_label();{struct _RegionHandle _tmp972=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp972;_push_region(prgn);{struct Cyc_Absyn_Stmt*
_tmp975;struct Cyc_List_List*_tmp977;struct Cyc_Toc_Env*_tmp979;struct _tuple11
_tmp973=Cyc_Toc_xlate_pat(nv,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),
p,succ_lab,fail_lab,0);_LL2426: _tmp979=_tmp973.f1;goto _LL2424;_LL2424: _tmp977=
_tmp973.f2;goto _LL2422;_LL2422: _tmp975=_tmp973.f3;goto _LL2420;_LL2420: Cyc_Toc_stmt_to_c(
_tmp979,s);{struct Cyc_Absyn_Stmt*_tmp97B=Cyc_Absyn_label_stmt(succ_lab,s,0);if(
exhaust){s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)e,
Cyc_Absyn_seq_stmt(_tmp975,_tmp97B,0),0);}else{struct Cyc_Absyn_Exp*_tmp97C=Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0));struct Cyc_Absyn_Stmt*_tmp97D=Cyc_Absyn_label_stmt(
fail_lab,Cyc_Absyn_exp_stmt(_tmp97C,0),0);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(
t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp975,Cyc_Absyn_seq_stmt(_tmp97D,
_tmp97B,0),0),0);}for(0;_tmp977 != 0;_tmp977=_tmp977->tl){struct _tuple12 _tmp980;
void*_tmp981;struct _tuple0*_tmp983;struct _tuple12*_tmp97E=(struct _tuple12*)
_tmp977->hd;_tmp980=*_tmp97E;_LL2436: _tmp983=_tmp980.f1;goto _LL2434;_LL2434:
_tmp981=_tmp980.f2;goto _LL2431;_LL2431: s=Cyc_Absyn_declare_stmt(_tmp983,_tmp981,
0,s,0);}}};_pop_region(prgn);}return s;}}static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top){for(0;ds != 0;ds=ds->tl){if(!
nv->toplevel){({void*_tmp985[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("decls_to_c: not at toplevel!",sizeof(
unsigned char),29),_tag_arr(_tmp985,sizeof(void*),0));});}{struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)ds->hd;void*_tmp986=(void*)d->r;struct Cyc_Absyn_Vardecl*
_tmp9A0;struct Cyc_Absyn_Fndecl*_tmp9A2;struct Cyc_Absyn_Structdecl*_tmp9A4;struct
Cyc_Absyn_Uniondecl*_tmp9A6;struct Cyc_Absyn_Tuniondecl*_tmp9A8;struct Cyc_Absyn_Enumdecl*
_tmp9AA;struct Cyc_Absyn_Typedefdecl*_tmp9AC;struct Cyc_List_List*_tmp9AE;struct
Cyc_List_List*_tmp9B0;struct Cyc_List_List*_tmp9B2;_LL2440: if(*((int*)_tmp986)== 
Cyc_Absyn_Var_d){_LL2465: _tmp9A0=((struct Cyc_Absyn_Var_d_struct*)_tmp986)->f1;
goto _LL2441;}else{goto _LL2442;}_LL2442: if(*((int*)_tmp986)== Cyc_Absyn_Fn_d){
_LL2467: _tmp9A2=((struct Cyc_Absyn_Fn_d_struct*)_tmp986)->f1;goto _LL2443;}else{
goto _LL2444;}_LL2444: if(*((int*)_tmp986)== Cyc_Absyn_Let_d){goto _LL2445;}else{
goto _LL2446;}_LL2446: if(*((int*)_tmp986)== Cyc_Absyn_Letv_d){goto _LL2447;}else{
goto _LL2448;}_LL2448: if(*((int*)_tmp986)== Cyc_Absyn_Struct_d){_LL2469: _tmp9A4=((
struct Cyc_Absyn_Struct_d_struct*)_tmp986)->f1;goto _LL2449;}else{goto _LL2450;}
_LL2450: if(*((int*)_tmp986)== Cyc_Absyn_Union_d){_LL2471: _tmp9A6=((struct Cyc_Absyn_Union_d_struct*)
_tmp986)->f1;goto _LL2451;}else{goto _LL2452;}_LL2452: if(*((int*)_tmp986)== Cyc_Absyn_Tunion_d){
_LL2473: _tmp9A8=((struct Cyc_Absyn_Tunion_d_struct*)_tmp986)->f1;goto _LL2453;}
else{goto _LL2454;}_LL2454: if(*((int*)_tmp986)== Cyc_Absyn_Enum_d){_LL2475:
_tmp9AA=((struct Cyc_Absyn_Enum_d_struct*)_tmp986)->f1;goto _LL2455;}else{goto
_LL2456;}_LL2456: if(*((int*)_tmp986)== Cyc_Absyn_Typedef_d){_LL2477: _tmp9AC=((
struct Cyc_Absyn_Typedef_d_struct*)_tmp986)->f1;goto _LL2457;}else{goto _LL2458;}
_LL2458: if(*((int*)_tmp986)== Cyc_Absyn_Namespace_d){_LL2479: _tmp9AE=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp986)->f2;goto _LL2459;}else{goto _LL2460;}_LL2460: if(*((int*)_tmp986)== Cyc_Absyn_Using_d){
_LL2481: _tmp9B0=((struct Cyc_Absyn_Using_d_struct*)_tmp986)->f2;goto _LL2461;}
else{goto _LL2462;}_LL2462: if(*((int*)_tmp986)== Cyc_Absyn_ExternC_d){_LL2483:
_tmp9B2=((struct Cyc_Absyn_ExternC_d_struct*)_tmp986)->f1;goto _LL2463;}else{goto
_LL2439;}_LL2441: {struct _tuple0*_tmp9B4=_tmp9A0->name;if((void*)_tmp9A0->sc == (
void*)Cyc_Absyn_ExternC){_tmp9B4=({struct _tuple0*_tmp9B5=_cycalloc(sizeof(struct
_tuple0));_tmp9B5->f1=Cyc_Absyn_rel_ns_null;_tmp9B5->f2=(*_tmp9B4).f2;_tmp9B5;});}
if(_tmp9A0->initializer != 0){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_check_null(_tmp9A0->initializer));}nv=Cyc_Toc_add_varmap(nv,_tmp9A0->name,Cyc_Absyn_varb_exp(
_tmp9B4,(void*)({struct Cyc_Absyn_Global_b_struct*_tmp9B6=_cycalloc(sizeof(struct
Cyc_Absyn_Global_b_struct));_tmp9B6[0]=({struct Cyc_Absyn_Global_b_struct _tmp9B7;
_tmp9B7.tag=Cyc_Absyn_Global_b;_tmp9B7.f1=_tmp9A0;_tmp9B7;});_tmp9B6;}),0));
_tmp9A0->name=_tmp9B4;(void*)(_tmp9A0->sc=(void*)Cyc_Toc_scope_to_c((void*)
_tmp9A0->sc));(void*)(_tmp9A0->type=(void*)Cyc_Toc_typ_to_c_array((void*)_tmp9A0->type));
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp9B8=_cycalloc(sizeof(struct Cyc_List_List));
_tmp9B8->hd=d;_tmp9B8->tl=Cyc_Toc_result_decls;_tmp9B8;});goto _LL2439;}_LL2443: {
struct _tuple0*_tmp9B9=_tmp9A2->name;if((void*)_tmp9A2->sc == (void*)Cyc_Absyn_ExternC){
_tmp9B9=({struct _tuple0*_tmp9BA=_cycalloc(sizeof(struct _tuple0));_tmp9BA->f1=Cyc_Absyn_rel_ns_null;
_tmp9BA->f2=(*_tmp9B9).f2;_tmp9BA;});}nv=Cyc_Toc_add_varmap(nv,_tmp9A2->name,Cyc_Absyn_var_exp(
_tmp9B9,0));_tmp9A2->name=_tmp9B9;Cyc_Toc_fndecl_to_c(nv,_tmp9A2);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp9BB=_cycalloc(sizeof(struct Cyc_List_List));_tmp9BB->hd=d;
_tmp9BB->tl=Cyc_Toc_result_decls;_tmp9BB;});goto _LL2439;}_LL2445: goto _LL2447;
_LL2447:({void*_tmp9BC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Toc_toc_impos)(_tag_arr("letdecl at toplevel",sizeof(unsigned char),20),
_tag_arr(_tmp9BC,sizeof(void*),0));});goto _LL2439;_LL2449: Cyc_Toc_structdecl_to_c(
_tmp9A4);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp9BD=_cycalloc(sizeof(
struct Cyc_List_List));_tmp9BD->hd=d;_tmp9BD->tl=Cyc_Toc_result_decls;_tmp9BD;});
goto _LL2439;_LL2451: Cyc_Toc_uniondecl_to_c(_tmp9A6);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp9BE=_cycalloc(sizeof(struct Cyc_List_List));_tmp9BE->hd=d;
_tmp9BE->tl=Cyc_Toc_result_decls;_tmp9BE;});goto _LL2439;_LL2453: if(_tmp9A8->is_xtunion){
Cyc_Toc_xtuniondecl_to_c(_tmp9A8);}else{Cyc_Toc_tuniondecl_to_c(_tmp9A8);}goto
_LL2439;_LL2455: Cyc_Toc_enumdecl_to_c(nv,_tmp9AA);Cyc_Toc_result_decls=({struct
Cyc_List_List*_tmp9BF=_cycalloc(sizeof(struct Cyc_List_List));_tmp9BF->hd=d;
_tmp9BF->tl=Cyc_Toc_result_decls;_tmp9BF;});goto _LL2439;_LL2457: _tmp9AC->name=
_tmp9AC->name;_tmp9AC->tvs=0;(void*)(_tmp9AC->defn=(void*)Cyc_Toc_typ_to_c_array((
void*)_tmp9AC->defn));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp9C0=
_cycalloc(sizeof(struct Cyc_List_List));_tmp9C0->hd=d;_tmp9C0->tl=Cyc_Toc_result_decls;
_tmp9C0;});goto _LL2439;_LL2459: _tmp9B0=_tmp9AE;goto _LL2461;_LL2461: _tmp9B2=
_tmp9B0;goto _LL2463;_LL2463: nv=Cyc_Toc_decls_to_c(nv,_tmp9B2,top);goto _LL2439;
_LL2439:;}}return nv;}static void Cyc_Toc_init(){Cyc_Toc_result_decls=0;Cyc_Toc_tuple_types=
0;Cyc_Toc_tuple_type_counter=0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=
0;Cyc_Toc_structs_so_far=0;Cyc_Toc_tunions_so_far=0;Cyc_Toc_xtunion_fields_so_far=
0;Cyc_Toc_total_bounds_checks=0;Cyc_Toc_bounds_checks_eliminated=0;}struct Cyc_List_List*
Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();Cyc_Toc_decls_to_c(Cyc_Toc_empty_env(),
ds,1);return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Toc_result_decls);}
