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
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;
extern struct Cyc_Std___sFILE*Cyc_Std_stderr;extern int Cyc_Std_fflush(struct Cyc_Std___sFILE*);
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
static const int Cyc_Std_String_pa=0;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};static const int Cyc_Std_Int_pa=1;struct Cyc_Std_Int_pa_struct{int
tag;unsigned int f1;};static const int Cyc_Std_Double_pa=2;struct Cyc_Std_Double_pa_struct{
int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=3;struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=4;struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);static const int Cyc_Std_ShortPtr_sa=0;struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};static const int Cyc_Std_UShortPtr_sa=1;struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=2;struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=3;struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=4;struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern
unsigned char Cyc_List_List_mismatch[18];extern struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern unsigned char Cyc_List_Nth[8];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(
int(*cmp)(void*,void*));extern int Cyc_Dict_member(struct Cyc_Dict_Dict*d,void*k);
extern struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);extern struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict*d,void*k);struct Cyc_Set_Set;extern struct
Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));extern int Cyc_Set_member(struct
Cyc_Set_Set*s,void*elt);extern unsigned char Cyc_Set_Absent[11];struct Cyc_SlowDict_Dict;
extern unsigned char Cyc_SlowDict_Present[12];extern unsigned char Cyc_SlowDict_Absent[
11];extern struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(int(*cmp)(void*,void*));
extern int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*d);extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*k,void*v);extern struct Cyc_Core_Opt*
Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,void*k);extern struct Cyc_SlowDict_Dict*
Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,void*k);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;static const int Cyc_Position_Lex=
0;static const int Cyc_Position_Parse=1;static const int Cyc_Position_Elab=2;struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];extern int Cyc_Std_zstrptrcmp(
struct _tagged_arr*,struct _tagged_arr*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;static const int Cyc_Absyn_Loc_n=0;static const int Cyc_Absyn_Rel_n=0;
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_Abs_n=
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
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);extern int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct _tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;
extern void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct
Cyc_List_List*curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(
void*);static const int Cyc_Tcenv_VarRes=0;struct Cyc_Tcenv_VarRes_struct{int tag;
void*f1;};static const int Cyc_Tcenv_StructRes=1;struct Cyc_Tcenv_StructRes_struct{
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
ae;struct Cyc_Core_Opt*le;};extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();extern
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv();extern struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);extern struct Cyc_List_List*
Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tagged_arr*,struct Cyc_List_List*);extern void*Cyc_Tcenv_lookup_ordinary(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);extern struct
Cyc_Absyn_Structdecl**Cyc_Tcenv_lookup_structdecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple0*);extern struct Cyc_Absyn_Uniondecl**Cyc_Tcenv_lookup_uniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);extern struct
Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple0*);extern struct Cyc_Core_Opt*Cyc_Tcenv_lookup_xtuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);extern struct
Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple0*);extern struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple0*);extern void*Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);extern
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Vardecl*);extern struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);extern struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
continue_dest);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct
Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*
clause);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct Cyc_Tcenv_Tenv*,void*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct Cyc_Tcenv_Tenv*te);extern
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);extern void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);extern void
Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
extern void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct _tagged_arr*,struct Cyc_Absyn_Stmt**);struct _tuple3{struct Cyc_Absyn_Switch_clause*
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};extern struct _tuple3*Cyc_Tcenv_process_fallthru(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
extern struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);extern
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct
_tagged_arr*,struct Cyc_Absyn_Stmt*);extern int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
extern void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);extern struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*r);extern void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);extern void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);extern int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);extern void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
extern unsigned char Cyc_Tcutil_TypeErr[12];extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
extern void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);extern void*Cyc_Tcutil_compress(
void*t);extern int Cyc_Tcutil_subset_effect(int set_to_empty,void*e1,void*e2);
extern int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);extern struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);extern int*Cyc_Tcutil_new_tvar_id();extern void Cyc_Tcutil_add_tvar_identity(
struct Cyc_Absyn_Tvar*);extern void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
unsigned char Cyc_Tcenv_Env_error[14]="\000\000\000\000Env_error";void*Cyc_Tcenv_env_err(
struct _tagged_arr msg){({struct Cyc_Std_String_pa_struct _tmp1;_tmp1.tag=Cyc_Std_String_pa;
_tmp1.f1=(struct _tagged_arr)msg;{void*_tmp0[1]={& _tmp1};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: %s\n",sizeof(unsigned char),11),_tag_arr(_tmp0,sizeof(void*),1));}});
Cyc_Std_fflush((struct Cyc_Std___sFILE*)Cyc_Std_stderr);(int)_throw((void*)Cyc_Tcenv_Env_error);}
struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;struct Cyc_Tcenv_CtrlEnv{void*
continue_stmt;void*break_stmt;struct _tuple3*fallthru_clause;void*next_stmt;int
try_depth;};struct Cyc_Tcenv_SharedFenv{void*return_typ;struct Cyc_Dict_Dict*
seen_labels;struct Cyc_SlowDict_Dict*needed_labels;};struct Cyc_Tcenv_RgnPO{struct
Cyc_Absyn_Tvar*tvar;struct Cyc_List_List*ancestors;};struct Cyc_Tcenv_Fenv{struct
Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_List_List*
region_order;struct Cyc_Dict_Dict*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*
ctrl_env;void*capability;void*curr_rgn;};struct _tuple4{void*f1;int f2;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(){Cyc_Tcutil_empty_var_set=({struct Cyc_Core_Opt*_tmp2=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp2->v=((struct Cyc_Set_Set*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Set_empty)(Cyc_Std_zstrptrcmp);_tmp2;});{
struct Cyc_Tcenv_Genv*_tmp3=Cyc_Tcenv_empty_genv();struct Cyc_List_List*_tmp4=0;
_tmp3->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(_tmp3->tuniondecls,(*
Cyc_Absyn_exn_name).f2,({struct Cyc_Absyn_Tuniondecl**_tmp5=_cycalloc(sizeof(
struct Cyc_Absyn_Tuniondecl*));_tmp5[0]=Cyc_Absyn_exn_tud;_tmp5;}));{struct Cyc_List_List*
_tmp6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;
for(0;_tmp6 != 0;_tmp6=_tmp6->tl){_tmp3->ordinaries=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple4*v))Cyc_Dict_insert)(
_tmp3->ordinaries,(*((struct Cyc_Absyn_Tunionfield*)_tmp6->hd)->name).f2,({struct
_tuple4*_tmp7=_cycalloc(sizeof(struct _tuple4));_tmp7->f1=(void*)({struct Cyc_Tcenv_TunionRes_struct*
_tmp8=_cycalloc(sizeof(struct Cyc_Tcenv_TunionRes_struct));_tmp8[0]=({struct Cyc_Tcenv_TunionRes_struct
_tmp9;_tmp9.tag=Cyc_Tcenv_TunionRes;_tmp9.f1=Cyc_Absyn_exn_tud;_tmp9.f2=(struct
Cyc_Absyn_Tunionfield*)_tmp6->hd;_tmp9;});_tmp8;});_tmp7->f2=1;_tmp7;}));}}{
struct Cyc_Dict_Dict*ae=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*
k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp),
_tmp4,_tmp3);return({struct Cyc_Tcenv_Tenv*_tmpA=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));
_tmpA->ns=_tmp4;_tmpA->ae=ae;_tmpA->le=0;_tmpA;});}}}static struct Cyc_Tcenv_Genv*
Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tagged_arr*n,struct Cyc_List_List*ns){return((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(
te,loc,n,ns));}static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*
ns){if(ns == 0){return((struct Cyc_List_List*(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv::outer_namespace",sizeof(unsigned char),23));}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2){if(n1 == n2){return 1;}for(0;n1 != 0;
n1=n1->tl){if(n2 == 0){return 0;}if(Cyc_Std_zstrptrcmp((struct _tagged_arr*)n1->hd,(
struct _tagged_arr*)n2->hd)!= 0){return 0;}n2=n2->tl;}if(n2 != 0){return 0;}return 1;}
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _tagged_arr*),struct _tagged_arr*v,
struct Cyc_List_List*cns,struct Cyc_List_List*nss){for(0;nss != 0;nss=nss->tl){if(!
Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)nss->hd);struct _handler_cons _tmpB;_push_handler(&
_tmpB);{int _tmpD=0;if(setjmp(_tmpB.handler)){_tmpD=1;}if(! _tmpD){lookup(ge2,v);({
struct Cyc_Std_String_pa_struct _tmpF;_tmpF.tag=Cyc_Std_String_pa;_tmpF.f1=(struct
_tagged_arr)*v;{void*_tmpE[1]={& _tmpF};Cyc_Tcutil_terr(loc,_tag_arr("%s is ambiguous",
sizeof(unsigned char),16),_tag_arr(_tmpE,sizeof(void*),1));}});;_pop_handler();}
else{void*_tmpC=(void*)_exn_thrown;void*_tmp11=_tmpC;_LL19: if(_tmp11 == Cyc_Dict_Absent){
goto _LL20;}else{goto _LL21;}_LL21: goto _LL22;_LL20: goto _LL18;_LL22:(void)_throw(
_tmp11);_LL18:;}}}}return;}static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_tagged_arr*),struct _tagged_arr*v){struct Cyc_List_List*cns=te->ns;while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);{struct _handler_cons _tmp17;
_push_handler(& _tmp17);{int _tmp19=0;if(setjmp(_tmp17.handler)){_tmp19=1;}if(!
_tmp19){{void*result=lookup(ge,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1A=result;_npop_handler(0);return _tmp1A;}};_pop_handler();}else{void*
_tmp18=(void*)_exn_thrown;void*_tmp1C=_tmp18;_LL30: if(_tmp1C == Cyc_Dict_Absent){
goto _LL31;}else{goto _LL32;}_LL32: goto _LL33;_LL31: goto _LL29;_LL33:(void)_throw(
_tmp1C);_LL29:;}}}{struct Cyc_List_List*_tmp22=ge->availables;for(0;_tmp22 != 0;
_tmp22=_tmp22->tl){struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp22->hd);
struct _handler_cons _tmp23;_push_handler(& _tmp23);{int _tmp25=0;if(setjmp(_tmp23.handler)){
_tmp25=1;}if(! _tmp25){{void*result=lookup(ge2,v);Cyc_Tcenv_check_repeat(te,loc,
lookup,v,(struct Cyc_List_List*)_tmp22->hd,_tmp22->tl);{void*_tmp26=result;
_npop_handler(0);return _tmp26;}};_pop_handler();}else{void*_tmp24=(void*)
_exn_thrown;void*_tmp28=_tmp24;_LL42: if(_tmp28 == Cyc_Dict_Absent){goto _LL43;}
else{goto _LL44;}_LL44: goto _LL45;_LL43: goto _LL41;_LL45:(void)_throw(_tmp28);_LL41:;}}}}
if(cns == 0){(int)_throw((void*)Cyc_Dict_Absent);}cns=Cyc_Tcenv_outer_namespace(
cns);}}static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,
struct _tagged_arr*v){struct _tuple4*ans=((struct _tuple4*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(ge->ordinaries,v);(*ans).f2=1;return(*ans).f1;}
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple0*q){struct _tuple0 _tmp30;struct _tagged_arr*_tmp31;void*_tmp33;
struct _tuple0*_tmp2E=q;_tmp30=*_tmp2E;_LL52: _tmp33=_tmp30.f1;goto _LL50;_LL50:
_tmp31=_tmp30.f2;goto _LL47;_LL47: {void*_tmp35=_tmp33;struct Cyc_List_List*_tmp3F;
struct Cyc_List_List*_tmp41;struct Cyc_List_List _tmp43;struct Cyc_List_List*_tmp44;
struct _tagged_arr*_tmp46;struct Cyc_List_List*_tmp48;_LL55: if(_tmp35 == (void*)Cyc_Absyn_Loc_n){
goto _LL56;}else{goto _LL57;}_LL57: if((unsigned int)_tmp35 > 1?*((int*)_tmp35)== 
Cyc_Absyn_Rel_n: 0){_LL64: _tmp3F=((struct Cyc_Absyn_Rel_n_struct*)_tmp35)->f1;if(
_tmp3F == 0){goto _LL58;}else{goto _LL59;}}else{goto _LL59;}_LL59: if((unsigned int)
_tmp35 > 1?*((int*)_tmp35)== Cyc_Absyn_Rel_n: 0){_LL66: _tmp41=((struct Cyc_Absyn_Rel_n_struct*)
_tmp35)->f1;if(_tmp41 == 0){goto _LL61;}else{_tmp43=*_tmp41;_LL71: _tmp46=(struct
_tagged_arr*)_tmp43.hd;goto _LL69;_LL69: _tmp44=_tmp43.tl;goto _LL60;}}else{goto
_LL61;}_LL61: if((unsigned int)_tmp35 > 1?*((int*)_tmp35)== Cyc_Absyn_Abs_n: 0){
_LL73: _tmp48=((struct Cyc_Absyn_Abs_n_struct*)_tmp35)->f1;goto _LL62;}else{goto
_LL54;}_LL56: goto _LL58;_LL58: return Cyc_Tcenv_scoped_lookup(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,
_tmp31);_LL60: {struct Cyc_Tcenv_Genv*_tmp4A=Cyc_Tcenv_lookup_namespace(te,loc,
_tmp46,_tmp44);return Cyc_Tcenv_lookup_ordinary_global_f(_tmp4A,_tmp31);}_LL62:
return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp48),_tmp31);_LL54:;}}struct
Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tagged_arr*n,struct Cyc_List_List*ns){struct Cyc_List_List*_tmp4B=te->ns;
struct Cyc_List_List*_tmp4C=0;while(1){struct Cyc_Tcenv_Genv*_tmp4D=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp4B);{
struct Cyc_List_List*_tmp4E=_tmp4D->availables;for(0;_tmp4E != 0;_tmp4E=_tmp4E->tl){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp4E->hd);if(((
int(*)(struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(ge2->namespaces,
n)){_tmp4C=({struct Cyc_List_List*_tmp4F=_cycalloc(sizeof(struct Cyc_List_List));
_tmp4F->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp4B,({struct Cyc_List_List*_tmp50=_cycalloc(sizeof(struct Cyc_List_List));
_tmp50->hd=n;_tmp50->tl=ns;_tmp50;}));_tmp4F->tl=_tmp4C;_tmp4F;});}}}if(((int(*)(
struct Cyc_Set_Set*s,struct _tagged_arr*elt))Cyc_Set_member)(_tmp4D->namespaces,n)){
_tmp4C=({struct Cyc_List_List*_tmp51=_cycalloc(sizeof(struct Cyc_List_List));
_tmp51->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(_tmp4B,({struct Cyc_List_List*_tmp52=_cycalloc(sizeof(struct Cyc_List_List));
_tmp52->hd=n;_tmp52->tl=ns;_tmp52;}));_tmp51->tl=_tmp4C;_tmp51;});}if(_tmp4C != 0){
if(_tmp4C->tl != 0){({struct Cyc_Std_String_pa_struct _tmp54;_tmp54.tag=Cyc_Std_String_pa;
_tmp54.f1=(struct _tagged_arr)*n;{void*_tmp53[1]={& _tmp54};Cyc_Tcutil_terr(loc,
_tag_arr("%s is ambiguous",sizeof(unsigned char),16),_tag_arr(_tmp53,sizeof(void*),
1));}});}return(struct Cyc_List_List*)_tmp4C->hd;}if(_tmp4B == 0){(int)_throw((
void*)Cyc_Dict_Absent);}_tmp4B=Cyc_Tcenv_outer_namespace(_tmp4B);}}static struct
Cyc_Absyn_Structdecl**Cyc_Tcenv_lookup_structdecl_f(struct Cyc_Tcenv_Genv*ge,
struct _tagged_arr*v){return((struct Cyc_Absyn_Structdecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->structdecls,v);}struct Cyc_Absyn_Structdecl**
Cyc_Tcenv_lookup_structdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple0*q){struct _tuple0 _tmp57;struct _tagged_arr*_tmp58;void*_tmp5A;
struct _tuple0*_tmp55=q;_tmp57=*_tmp55;_LL91: _tmp5A=_tmp57.f1;goto _LL89;_LL89:
_tmp58=_tmp57.f2;goto _LL86;_LL86: {void*_tmp5C=_tmp5A;struct Cyc_List_List*_tmp66;
struct Cyc_List_List*_tmp68;struct Cyc_List_List*_tmp6A;struct Cyc_List_List _tmp6C;
struct Cyc_List_List*_tmp6D;struct _tagged_arr*_tmp6F;_LL94: if(_tmp5C == (void*)Cyc_Absyn_Loc_n){
goto _LL95;}else{goto _LL96;}_LL96: if((unsigned int)_tmp5C > 1?*((int*)_tmp5C)== 
Cyc_Absyn_Rel_n: 0){_LL103: _tmp66=((struct Cyc_Absyn_Rel_n_struct*)_tmp5C)->f1;if(
_tmp66 == 0){goto _LL97;}else{goto _LL98;}}else{goto _LL98;}_LL98: if((unsigned int)
_tmp5C > 1?*((int*)_tmp5C)== Cyc_Absyn_Abs_n: 0){_LL105: _tmp68=((struct Cyc_Absyn_Abs_n_struct*)
_tmp5C)->f1;goto _LL99;}else{goto _LL100;}_LL100: if((unsigned int)_tmp5C > 1?*((int*)
_tmp5C)== Cyc_Absyn_Rel_n: 0){_LL107: _tmp6A=((struct Cyc_Absyn_Rel_n_struct*)
_tmp5C)->f1;if(_tmp6A == 0){goto _LL93;}else{_tmp6C=*_tmp6A;_LL112: _tmp6F=(struct
_tagged_arr*)_tmp6C.hd;goto _LL110;_LL110: _tmp6D=_tmp6C.tl;goto _LL101;}}else{goto
_LL93;}_LL95: goto _LL97;_LL97: return((struct Cyc_Absyn_Structdecl**(*)(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Structdecl**(*lookup)(struct
Cyc_Tcenv_Genv*,struct _tagged_arr*),struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_structdecl_f,_tmp58);_LL99: {struct Cyc_Tcenv_Genv*_tmp71=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp68);return((struct Cyc_Absyn_Structdecl**(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp71->structdecls,_tmp58);}_LL101: {
struct Cyc_Tcenv_Genv*_tmp72=Cyc_Tcenv_lookup_namespace(te,loc,_tmp6F,_tmp6D);
return((struct Cyc_Absyn_Structdecl**(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*
k))Cyc_Dict_lookup)(_tmp72->structdecls,_tmp58);}_LL93:;}}static struct Cyc_Absyn_Uniondecl**
Cyc_Tcenv_lookup_uniondecl_f(struct Cyc_Tcenv_Genv*ge,struct _tagged_arr*v){return((
struct Cyc_Absyn_Uniondecl**(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
ge->uniondecls,v);}struct Cyc_Absyn_Uniondecl**Cyc_Tcenv_lookup_uniondecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0
_tmp75;struct _tagged_arr*_tmp76;void*_tmp78;struct _tuple0*_tmp73=q;_tmp75=*
_tmp73;_LL121: _tmp78=_tmp75.f1;goto _LL119;_LL119: _tmp76=_tmp75.f2;goto _LL116;
_LL116: {void*_tmp7A=_tmp78;struct Cyc_List_List*_tmp84;struct Cyc_List_List*
_tmp86;struct Cyc_List_List*_tmp88;struct Cyc_List_List _tmp8A;struct Cyc_List_List*
_tmp8B;struct _tagged_arr*_tmp8D;_LL124: if(_tmp7A == (void*)Cyc_Absyn_Loc_n){goto
_LL125;}else{goto _LL126;}_LL126: if((unsigned int)_tmp7A > 1?*((int*)_tmp7A)== Cyc_Absyn_Rel_n:
0){_LL133: _tmp84=((struct Cyc_Absyn_Rel_n_struct*)_tmp7A)->f1;if(_tmp84 == 0){goto
_LL127;}else{goto _LL128;}}else{goto _LL128;}_LL128: if((unsigned int)_tmp7A > 1?*((
int*)_tmp7A)== Cyc_Absyn_Abs_n: 0){_LL135: _tmp86=((struct Cyc_Absyn_Abs_n_struct*)
_tmp7A)->f1;goto _LL129;}else{goto _LL130;}_LL130: if((unsigned int)_tmp7A > 1?*((
int*)_tmp7A)== Cyc_Absyn_Rel_n: 0){_LL137: _tmp88=((struct Cyc_Absyn_Rel_n_struct*)
_tmp7A)->f1;if(_tmp88 == 0){goto _LL123;}else{_tmp8A=*_tmp88;_LL142: _tmp8D=(struct
_tagged_arr*)_tmp8A.hd;goto _LL140;_LL140: _tmp8B=_tmp8A.tl;goto _LL131;}}else{goto
_LL123;}_LL125: goto _LL127;_LL127: return((struct Cyc_Absyn_Uniondecl**(*)(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Uniondecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _tagged_arr*),struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_uniondecl_f,_tmp76);_LL129: {struct Cyc_Tcenv_Genv*_tmp8F=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,_tmp86);return((struct Cyc_Absyn_Uniondecl**(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp8F->uniondecls,_tmp76);}_LL131: {
struct Cyc_Tcenv_Genv*_tmp90=Cyc_Tcenv_lookup_namespace(te,loc,_tmp8D,_tmp8B);
return((struct Cyc_Absyn_Uniondecl**(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))
Cyc_Dict_lookup)(_tmp90->uniondecls,_tmp76);}_LL123:;}}static struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl_f(struct Cyc_Tcenv_Genv*ge,struct _tagged_arr*v){
return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*
k))Cyc_Dict_lookup)(ge->tuniondecls,v);}struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct
_tuple0 _tmp93;struct _tagged_arr*_tmp94;void*_tmp96;struct _tuple0*_tmp91=q;_tmp93=*
_tmp91;_LL151: _tmp96=_tmp93.f1;goto _LL149;_LL149: _tmp94=_tmp93.f2;goto _LL146;
_LL146: {void*_tmp98=_tmp96;struct Cyc_List_List*_tmpA2;struct Cyc_List_List*
_tmpA4;struct Cyc_List_List _tmpA6;struct Cyc_List_List*_tmpA7;struct _tagged_arr*
_tmpA9;struct Cyc_List_List*_tmpAB;_LL154: if(_tmp98 == (void*)Cyc_Absyn_Loc_n){
goto _LL155;}else{goto _LL156;}_LL156: if((unsigned int)_tmp98 > 1?*((int*)_tmp98)== 
Cyc_Absyn_Rel_n: 0){_LL163: _tmpA2=((struct Cyc_Absyn_Rel_n_struct*)_tmp98)->f1;if(
_tmpA2 == 0){goto _LL157;}else{goto _LL158;}}else{goto _LL158;}_LL158: if((
unsigned int)_tmp98 > 1?*((int*)_tmp98)== Cyc_Absyn_Rel_n: 0){_LL165: _tmpA4=((
struct Cyc_Absyn_Rel_n_struct*)_tmp98)->f1;if(_tmpA4 == 0){goto _LL160;}else{_tmpA6=*
_tmpA4;_LL170: _tmpA9=(struct _tagged_arr*)_tmpA6.hd;goto _LL168;_LL168: _tmpA7=
_tmpA6.tl;goto _LL159;}}else{goto _LL160;}_LL160: if((unsigned int)_tmp98 > 1?*((int*)
_tmp98)== Cyc_Absyn_Abs_n: 0){_LL172: _tmpAB=((struct Cyc_Absyn_Abs_n_struct*)
_tmp98)->f1;goto _LL161;}else{goto _LL153;}_LL155: goto _LL157;_LL157: return((struct
Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct Cyc_Absyn_Tuniondecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _tagged_arr*),
struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_tuniondecl_f,
_tmp94);_LL159: {struct Cyc_Tcenv_Genv*_tmpAD=Cyc_Tcenv_lookup_namespace(te,loc,
_tmpA9,_tmpA7);return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmpAD->tuniondecls,_tmp94);}_LL161: {
struct Cyc_Tcenv_Genv*_tmpAE=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmpAB);return((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmpAE->tuniondecls,
_tmp94);}_LL153:;}}static struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv*ge,struct _tagged_arr*v){return((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->tuniondecls,v);}
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_xtuniondecl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0 _tmpB1;struct _tagged_arr*
_tmpB2;void*_tmpB4;struct _tuple0*_tmpAF=q;_tmpB1=*_tmpAF;_LL181: _tmpB4=_tmpB1.f1;
goto _LL179;_LL179: _tmpB2=_tmpB1.f2;goto _LL176;_LL176: {void*_tmpB6=_tmpB4;struct
Cyc_List_List*_tmpC0;struct Cyc_List_List*_tmpC2;struct Cyc_List_List _tmpC4;struct
Cyc_List_List*_tmpC5;struct _tagged_arr*_tmpC7;struct Cyc_List_List*_tmpC9;_LL184:
if((unsigned int)_tmpB6 > 1?*((int*)_tmpB6)== Cyc_Absyn_Rel_n: 0){_LL193: _tmpC0=((
struct Cyc_Absyn_Rel_n_struct*)_tmpB6)->f1;if(_tmpC0 == 0){goto _LL185;}else{goto
_LL186;}}else{goto _LL186;}_LL186: if(_tmpB6 == (void*)Cyc_Absyn_Loc_n){goto _LL187;}
else{goto _LL188;}_LL188: if((unsigned int)_tmpB6 > 1?*((int*)_tmpB6)== Cyc_Absyn_Rel_n:
0){_LL195: _tmpC2=((struct Cyc_Absyn_Rel_n_struct*)_tmpB6)->f1;if(_tmpC2 == 0){goto
_LL190;}else{_tmpC4=*_tmpC2;_LL200: _tmpC7=(struct _tagged_arr*)_tmpC4.hd;goto
_LL198;_LL198: _tmpC5=_tmpC4.tl;goto _LL189;}}else{goto _LL190;}_LL190: if((
unsigned int)_tmpB6 > 1?*((int*)_tmpB6)== Cyc_Absyn_Abs_n: 0){_LL202: _tmpC9=((
struct Cyc_Absyn_Abs_n_struct*)_tmpB6)->f1;goto _LL191;}else{goto _LL183;}_LL185: {
struct _handler_cons _tmpCB;_push_handler(& _tmpCB);{int _tmpCD=0;if(setjmp(_tmpCB.handler)){
_tmpCD=1;}if(! _tmpCD){{struct Cyc_Core_Opt*_tmpCF=({struct Cyc_Core_Opt*_tmpCE=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpCE->v=((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _tagged_arr*),struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(
te,loc,Cyc_Tcenv_lookup_xtuniondecl_f,_tmpB2);_tmpCE;});_npop_handler(0);return
_tmpCF;};_pop_handler();}else{void*_tmpCC=(void*)_exn_thrown;void*_tmpD1=_tmpCC;
_LL211: if(_tmpD1 == Cyc_Dict_Absent){goto _LL212;}else{goto _LL213;}_LL213: goto
_LL214;_LL212: return 0;_LL214:(void)_throw(_tmpD1);_LL210:;}}}_LL187:({void*
_tmpD7[0]={};Cyc_Tcutil_terr(loc,_tag_arr("lookup_xtuniondecl: impossible",
sizeof(unsigned char),31),_tag_arr(_tmpD7,sizeof(void*),0));});return 0;_LL189: {
struct Cyc_Tcenv_Genv*ge;{struct _handler_cons _tmpD8;_push_handler(& _tmpD8);{int
_tmpDA=0;if(setjmp(_tmpD8.handler)){_tmpDA=1;}if(! _tmpDA){ge=Cyc_Tcenv_lookup_namespace(
te,loc,_tmpC7,_tmpC5);;_pop_handler();}else{void*_tmpD9=(void*)_exn_thrown;void*
_tmpDC=_tmpD9;_LL222: if(_tmpDC == Cyc_Dict_Absent){goto _LL223;}else{goto _LL224;}
_LL224: goto _LL225;_LL223:({void*_tmpE2[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad qualified name for xtunion",
sizeof(unsigned char),31),_tag_arr(_tmpE2,sizeof(void*),0));});(int)_throw((void*)
Cyc_Dict_Absent);_LL225:(void)_throw(_tmpDC);_LL221:;}}}return({struct Cyc_Core_Opt*
_tmpE3=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpE3->v=((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->tuniondecls,
_tmpB2);_tmpE3;});}_LL191: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmpC9);
return({struct Cyc_Core_Opt*_tmpE4=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpE4->v=((
struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
ge->tuniondecls,_tmpB2);_tmpE4;});}_LL183:;}}static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _tagged_arr*v){return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->enumdecls,v);}
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0 _tmpE7;struct
_tagged_arr*_tmpE8;void*_tmpEA;struct _tuple0*_tmpE5=q;_tmpE7=*_tmpE5;_LL235:
_tmpEA=_tmpE7.f1;goto _LL233;_LL233: _tmpE8=_tmpE7.f2;goto _LL230;_LL230: {void*
_tmpEC=_tmpEA;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF8;struct Cyc_List_List
_tmpFA;struct Cyc_List_List*_tmpFB;struct _tagged_arr*_tmpFD;struct Cyc_List_List*
_tmpFF;_LL238: if(_tmpEC == (void*)Cyc_Absyn_Loc_n){goto _LL239;}else{goto _LL240;}
_LL240: if((unsigned int)_tmpEC > 1?*((int*)_tmpEC)== Cyc_Absyn_Rel_n: 0){_LL247:
_tmpF6=((struct Cyc_Absyn_Rel_n_struct*)_tmpEC)->f1;if(_tmpF6 == 0){goto _LL241;}
else{goto _LL242;}}else{goto _LL242;}_LL242: if((unsigned int)_tmpEC > 1?*((int*)
_tmpEC)== Cyc_Absyn_Rel_n: 0){_LL249: _tmpF8=((struct Cyc_Absyn_Rel_n_struct*)
_tmpEC)->f1;if(_tmpF8 == 0){goto _LL244;}else{_tmpFA=*_tmpF8;_LL254: _tmpFD=(struct
_tagged_arr*)_tmpFA.hd;goto _LL252;_LL252: _tmpFB=_tmpFA.tl;goto _LL243;}}else{goto
_LL244;}_LL244: if((unsigned int)_tmpEC > 1?*((int*)_tmpEC)== Cyc_Absyn_Abs_n: 0){
_LL256: _tmpFF=((struct Cyc_Absyn_Abs_n_struct*)_tmpEC)->f1;goto _LL245;}else{goto
_LL237;}_LL239: goto _LL241;_LL241: return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,
struct _tagged_arr*),struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,
_tmpE8);_LL243: {struct Cyc_Tcenv_Genv*_tmp101=Cyc_Tcenv_lookup_namespace(te,loc,
_tmpFD,_tmpFB);return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp101->enumdecls,_tmpE8);}_LL245: {
struct Cyc_Tcenv_Genv*_tmp102=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmpFF);return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp102->enumdecls,
_tmpE8);}_LL237:;}}static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _tagged_arr*v){return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(ge->typedefs,v);}
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0 _tmp105;struct
_tagged_arr*_tmp106;void*_tmp108;struct _tuple0*_tmp103=q;_tmp105=*_tmp103;_LL265:
_tmp108=_tmp105.f1;goto _LL263;_LL263: _tmp106=_tmp105.f2;goto _LL260;_LL260: {void*
_tmp10A=_tmp108;struct Cyc_List_List*_tmp114;struct Cyc_List_List*_tmp116;struct
Cyc_List_List _tmp118;struct Cyc_List_List*_tmp119;struct _tagged_arr*_tmp11B;
struct Cyc_List_List*_tmp11D;_LL268: if(_tmp10A == (void*)Cyc_Absyn_Loc_n){goto
_LL269;}else{goto _LL270;}_LL270: if((unsigned int)_tmp10A > 1?*((int*)_tmp10A)== 
Cyc_Absyn_Rel_n: 0){_LL277: _tmp114=((struct Cyc_Absyn_Rel_n_struct*)_tmp10A)->f1;
if(_tmp114 == 0){goto _LL271;}else{goto _LL272;}}else{goto _LL272;}_LL272: if((
unsigned int)_tmp10A > 1?*((int*)_tmp10A)== Cyc_Absyn_Rel_n: 0){_LL279: _tmp116=((
struct Cyc_Absyn_Rel_n_struct*)_tmp10A)->f1;if(_tmp116 == 0){goto _LL274;}else{
_tmp118=*_tmp116;_LL284: _tmp11B=(struct _tagged_arr*)_tmp118.hd;goto _LL282;_LL282:
_tmp119=_tmp118.tl;goto _LL273;}}else{goto _LL274;}_LL274: if((unsigned int)_tmp10A
> 1?*((int*)_tmp10A)== Cyc_Absyn_Abs_n: 0){_LL286: _tmp11D=((struct Cyc_Absyn_Abs_n_struct*)
_tmp10A)->f1;goto _LL275;}else{goto _LL267;}_LL269: goto _LL271;_LL271: return((
struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_tagged_arr*),struct _tagged_arr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,
_tmp106);_LL273: {struct Cyc_Tcenv_Genv*_tmp11F=Cyc_Tcenv_lookup_namespace(te,loc,
_tmp11B,_tmp119);return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp11F->typedefs,_tmp106);}_LL275: {
struct Cyc_Tcenv_Genv*_tmp120=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp11D);return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp120->typedefs,
_tmp106);}_LL267:;}}struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(){return({struct
Cyc_Tcenv_Genv*_tmp121=_cycalloc(sizeof(struct Cyc_Tcenv_Genv));_tmp121->namespaces=(
struct Cyc_Set_Set*)((struct Cyc_Core_Opt*)_check_null(Cyc_Tcutil_empty_var_set))->v;
_tmp121->structdecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,
struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp121->uniondecls=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp121->tuniondecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);
_tmp121->enumdecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp121->typedefs=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);
_tmp121->ordinaries=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,
struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp121->availables=0;
_tmp121;});}void*Cyc_Tcenv_get_visible(void*f){while(1){void*_tmp122=f;void*
_tmp12A;void*_tmp12C;void*_tmp12E;void*_tmp130;_LL292: if(*((int*)_tmp122)== Cyc_Tcenv_Outermost){
_LL299: _tmp12A=(void*)((struct Cyc_Tcenv_Outermost_struct*)_tmp122)->f1;goto
_LL293;}else{goto _LL294;}_LL294: if(*((int*)_tmp122)== Cyc_Tcenv_Frame){_LL301:
_tmp12C=(void*)((struct Cyc_Tcenv_Frame_struct*)_tmp122)->f1;goto _LL295;}else{
goto _LL296;}_LL296: if(*((int*)_tmp122)== Cyc_Tcenv_Hidden){_LL305: _tmp130=(void*)((
struct Cyc_Tcenv_Hidden_struct*)_tmp122)->f1;goto _LL303;_LL303: _tmp12E=(void*)((
struct Cyc_Tcenv_Hidden_struct*)_tmp122)->f2;goto _LL297;}else{goto _LL291;}_LL293:
return _tmp12A;_LL295: return _tmp12C;_LL297: f=_tmp12E;goto _LL291;_LL291:;}}void*
Cyc_Tcenv_put_visible(void*f,void*x){void*_tmp132=f;void*_tmp13A;void*_tmp13C;
void*_tmp13E;void*_tmp140;void*_tmp142;_LL308: if(*((int*)_tmp132)== Cyc_Tcenv_Outermost){
_LL315: _tmp13A=(void*)((struct Cyc_Tcenv_Outermost_struct*)_tmp132)->f1;goto
_LL309;}else{goto _LL310;}_LL310: if(*((int*)_tmp132)== Cyc_Tcenv_Frame){_LL319:
_tmp13E=(void*)((struct Cyc_Tcenv_Frame_struct*)_tmp132)->f1;goto _LL317;_LL317:
_tmp13C=(void*)((struct Cyc_Tcenv_Frame_struct*)_tmp132)->f2;goto _LL311;}else{
goto _LL312;}_LL312: if(*((int*)_tmp132)== Cyc_Tcenv_Hidden){_LL323: _tmp142=(void*)((
struct Cyc_Tcenv_Hidden_struct*)_tmp132)->f1;goto _LL321;_LL321: _tmp140=(void*)((
struct Cyc_Tcenv_Hidden_struct*)_tmp132)->f2;goto _LL313;}else{goto _LL307;}_LL309:
return(void*)({struct Cyc_Tcenv_Outermost_struct*_tmp144=_cycalloc(sizeof(struct
Cyc_Tcenv_Outermost_struct));_tmp144[0]=({struct Cyc_Tcenv_Outermost_struct
_tmp145;_tmp145.tag=Cyc_Tcenv_Outermost;_tmp145.f1=(void*)x;_tmp145;});_tmp144;});
_LL311: return(void*)({struct Cyc_Tcenv_Frame_struct*_tmp146=_cycalloc(sizeof(
struct Cyc_Tcenv_Frame_struct));_tmp146[0]=({struct Cyc_Tcenv_Frame_struct _tmp147;
_tmp147.tag=Cyc_Tcenv_Frame;_tmp147.f1=(void*)x;_tmp147.f2=(void*)_tmp13C;
_tmp147;});_tmp146;});_LL313: return(void*)({struct Cyc_Tcenv_Hidden_struct*
_tmp148=_cycalloc(sizeof(struct Cyc_Tcenv_Hidden_struct));_tmp148[0]=({struct Cyc_Tcenv_Hidden_struct
_tmp149;_tmp149.tag=Cyc_Tcenv_Hidden;_tmp149.f1=(void*)_tmp142;_tmp149.f2=(void*)
Cyc_Tcenv_put_visible(_tmp140,x);_tmp149;});_tmp148;});_LL307:;}void*Cyc_Tcenv_apply_to_visible(
void*(*g)(void*),void*f){return Cyc_Tcenv_put_visible(f,g(Cyc_Tcenv_get_visible(f)));}
void*Cyc_Tcenv_map_frames(void*(*g)(void*),void*f){void*_tmp14A=f;void*_tmp152;
void*_tmp154;void*_tmp156;void*_tmp158;void*_tmp15A;_LL332: if(*((int*)_tmp14A)== 
Cyc_Tcenv_Outermost){_LL339: _tmp152=(void*)((struct Cyc_Tcenv_Outermost_struct*)
_tmp14A)->f1;goto _LL333;}else{goto _LL334;}_LL334: if(*((int*)_tmp14A)== Cyc_Tcenv_Frame){
_LL343: _tmp156=(void*)((struct Cyc_Tcenv_Frame_struct*)_tmp14A)->f1;goto _LL341;
_LL341: _tmp154=(void*)((struct Cyc_Tcenv_Frame_struct*)_tmp14A)->f2;goto _LL335;}
else{goto _LL336;}_LL336: if(*((int*)_tmp14A)== Cyc_Tcenv_Hidden){_LL347: _tmp15A=(
void*)((struct Cyc_Tcenv_Hidden_struct*)_tmp14A)->f1;goto _LL345;_LL345: _tmp158=(
void*)((struct Cyc_Tcenv_Hidden_struct*)_tmp14A)->f2;goto _LL337;}else{goto _LL331;}
_LL333: return(void*)({struct Cyc_Tcenv_Outermost_struct*_tmp15C=_cycalloc(sizeof(
struct Cyc_Tcenv_Outermost_struct));_tmp15C[0]=({struct Cyc_Tcenv_Outermost_struct
_tmp15D;_tmp15D.tag=Cyc_Tcenv_Outermost;_tmp15D.f1=(void*)g(_tmp152);_tmp15D;});
_tmp15C;});_LL335: return(void*)({struct Cyc_Tcenv_Frame_struct*_tmp15E=_cycalloc(
sizeof(struct Cyc_Tcenv_Frame_struct));_tmp15E[0]=({struct Cyc_Tcenv_Frame_struct
_tmp15F;_tmp15F.tag=Cyc_Tcenv_Frame;_tmp15F.f1=(void*)g(_tmp156);_tmp15F.f2=(
void*)Cyc_Tcenv_map_frames(g,_tmp154);_tmp15F;});_tmp15E;});_LL337: return(void*)({
struct Cyc_Tcenv_Hidden_struct*_tmp160=_cycalloc(sizeof(struct Cyc_Tcenv_Hidden_struct));
_tmp160[0]=({struct Cyc_Tcenv_Hidden_struct _tmp161;_tmp161.tag=Cyc_Tcenv_Hidden;
_tmp161.f1=(void*)g(_tmp15A);_tmp161.f2=(void*)Cyc_Tcenv_map_frames(g,_tmp158);
_tmp161;});_tmp160;});_LL331:;}struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*
te){if(te->le == 0){(int)_throw((void*)Cyc_Tcenv_Env_error);}return((struct Cyc_Tcenv_Fenv*(*)(
void*f))Cyc_Tcenv_get_visible)((void*)((struct Cyc_Core_Opt*)_check_null(te->le))->v);}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*
fe){if(te->le == 0){(int)_throw((void*)Cyc_Tcenv_Env_error);}return({struct Cyc_Tcenv_Tenv*
_tmp162=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));_tmp162->ns=te->ns;_tmp162->ae=
te->ae;_tmp162->le=({struct Cyc_Core_Opt*_tmp163=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp163->v=(void*)((void*(*)(void*f,struct Cyc_Tcenv_Fenv*x))Cyc_Tcenv_put_visible)((
void*)((struct Cyc_Core_Opt*)_check_null(te->le))->v,fe);_tmp163;});_tmp162;});}
static int Cyc_Tcenv_region_ancestor(struct Cyc_List_List*ancestors,struct Cyc_Absyn_Tvar*
r){{struct Cyc_List_List*a=ancestors;for(0;a != 0;a=a->tl){if(Cyc_Absyn_tvar_cmp(((
struct Cyc_Tcenv_RgnPO*)a->hd)->tvar,r)== 0){return 1;}}}{struct Cyc_List_List*a=
ancestors;for(0;a != 0;a=a->tl){if(Cyc_Tcenv_region_ancestor(((struct Cyc_Tcenv_RgnPO*)
a->hd)->ancestors,r)){return 1;}}}return 0;}static struct Cyc_Tcenv_RgnPO*Cyc_Tcenv_find_region_in_po(
struct Cyc_List_List*po,struct Cyc_Absyn_Tvar*r){for(0;po != 0;po=po->tl){if(Cyc_Absyn_tvar_cmp(((
struct Cyc_Tcenv_RgnPO*)po->hd)->tvar,r)== 0){return(struct Cyc_Tcenv_RgnPO*)po->hd;}}
return((struct Cyc_Tcenv_RgnPO*(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp165;_tmp165.tag=Cyc_Std_String_pa;
_tmp165.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)({struct Cyc_Absyn_VarType_struct*
_tmp166=_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));_tmp166[0]=({struct Cyc_Absyn_VarType_struct
_tmp167;_tmp167.tag=Cyc_Absyn_VarType;_tmp167.f1=r;_tmp167;});_tmp166;}));{void*
_tmp164[1]={& _tmp165};Cyc_Std_aprintf(_tag_arr("can't find region variable %s in po",
sizeof(unsigned char),36),_tag_arr(_tmp164,sizeof(void*),1));}}));}static int Cyc_Tcenv_check_region_outlives(
struct Cyc_List_List*po,struct Cyc_Absyn_Tvar*r1,struct Cyc_Absyn_Tvar*r2){struct
Cyc_List_List*r2_ancestors;while(po != 0? Cyc_Absyn_tvar_cmp(((struct Cyc_Tcenv_RgnPO*)
po->hd)->tvar,r2)!= 0: 0){po=po->tl;}if(po == 0){return 0;}return Cyc_Tcenv_region_ancestor(((
struct Cyc_Tcenv_RgnPO*)po->hd)->ancestors,r1);}struct _tuple5{void*f1;void*f2;};
static struct Cyc_List_List*Cyc_Tcenv_initial_region_po(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*tvs,struct Cyc_List_List*po){struct Cyc_List_List*result=0;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
if(Cyc_Tcutil_tvar_kind(tv)== (void*)Cyc_Absyn_RgnKind){result=({struct Cyc_List_List*
_tmp168=_cycalloc(sizeof(struct Cyc_List_List));_tmp168->hd=({struct Cyc_Tcenv_RgnPO*
_tmp169=_cycalloc(sizeof(struct Cyc_Tcenv_RgnPO));_tmp169->tvar=tv;_tmp169->ancestors=
0;_tmp169;});_tmp168->tl=result;_tmp168;});}}for(0;po != 0;po=po->tl){struct
_tuple5 _tmp16A=*((struct _tuple5*)po->hd);void*_tmp172;void*_tmp174;struct Cyc_Absyn_Tvar*
_tmp176;void*_tmp178;struct Cyc_Absyn_Tvar*_tmp17A;void*_tmp17C;void*_tmp17E;
_LL364: _LL371: _tmp172=_tmp16A.f1;if(_tmp172 == (void*)Cyc_Absyn_HeapRgn){goto
_LL365;}else{goto _LL366;}_LL366: _LL377: _tmp178=_tmp16A.f1;if((unsigned int)
_tmp178 > 3?*((int*)_tmp178)== Cyc_Absyn_VarType: 0){_LL379: _tmp17A=((struct Cyc_Absyn_VarType_struct*)
_tmp178)->f1;goto _LL373;}else{goto _LL368;}_LL373: _tmp174=_tmp16A.f2;if((
unsigned int)_tmp174 > 3?*((int*)_tmp174)== Cyc_Absyn_VarType: 0){_LL375: _tmp176=((
struct Cyc_Absyn_VarType_struct*)_tmp174)->f1;goto _LL367;}else{goto _LL368;}_LL368:
_LL383: _tmp17E=_tmp16A.f1;goto _LL381;_LL381: _tmp17C=_tmp16A.f2;goto _LL369;_LL365:
goto _LL363;_LL367: if(Cyc_Absyn_tvar_cmp(_tmp17A,_tmp176)== 0? 1: Cyc_Tcenv_check_region_outlives(
result,_tmp176,_tmp17A)){({struct Cyc_Std_String_pa_struct _tmp182;_tmp182.tag=Cyc_Std_String_pa;
_tmp182.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((void*)({struct Cyc_Absyn_VarType_struct*
_tmp185=_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));_tmp185[0]=({struct Cyc_Absyn_VarType_struct
_tmp186;_tmp186.tag=Cyc_Absyn_VarType;_tmp186.f1=_tmp176;_tmp186;});_tmp185;}));{
struct Cyc_Std_String_pa_struct _tmp181;_tmp181.tag=Cyc_Std_String_pa;_tmp181.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)({struct Cyc_Absyn_VarType_struct*
_tmp183=_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));_tmp183[0]=({struct Cyc_Absyn_VarType_struct
_tmp184;_tmp184.tag=Cyc_Absyn_VarType;_tmp184.f1=_tmp17A;_tmp184;});_tmp183;}));{
void*_tmp180[2]={& _tmp181,& _tmp182};Cyc_Tcutil_terr(loc,_tag_arr("bad region lifetime assertion (circularity):%s < %s",
sizeof(unsigned char),52),_tag_arr(_tmp180,sizeof(void*),2));}}});}else{struct
Cyc_Tcenv_RgnPO*_tmp187=Cyc_Tcenv_find_region_in_po(result,_tmp17A);struct Cyc_Tcenv_RgnPO*
_tmp188=Cyc_Tcenv_find_region_in_po(result,_tmp176);_tmp188->ancestors=({struct
Cyc_List_List*_tmp189=_cycalloc(sizeof(struct Cyc_List_List));_tmp189->hd=_tmp187;
_tmp189->tl=_tmp188->ancestors;_tmp189;});}goto _LL363;_LL369:({struct Cyc_Std_String_pa_struct
_tmp18C;_tmp18C.tag=Cyc_Std_String_pa;_tmp18C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp17C);{struct Cyc_Std_String_pa_struct _tmp18B;_tmp18B.tag=Cyc_Std_String_pa;
_tmp18B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp17E);{void*_tmp18A[2]={&
_tmp18B,& _tmp18C};Cyc_Tcutil_terr(loc,_tag_arr("bad region lifetime assertion:: %s < %s",
sizeof(unsigned char),40),_tag_arr(_tmp18A,sizeof(void*),2));}}});goto _LL363;
_LL363:;}return result;}static struct Cyc_List_List*Cyc_Tcenv_add_region_po(struct
Cyc_List_List*po,struct Cyc_Absyn_Tvar*tv){return({struct Cyc_List_List*_tmp18D=
_cycalloc(sizeof(struct Cyc_List_List));_tmp18D->hd=({struct Cyc_Tcenv_RgnPO*
_tmp18E=_cycalloc(sizeof(struct Cyc_Tcenv_RgnPO));_tmp18E->tvar=tv;_tmp18E->ancestors=
po;_tmp18E;});_tmp18D->tl=po;_tmp18D;});}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(
struct Cyc_Tcenv_Fenv*f){return({struct Cyc_Tcenv_Fenv*_tmp18F=_cycalloc(sizeof(
struct Cyc_Tcenv_Fenv));_tmp18F->shared=f->shared;_tmp18F->type_vars=f->type_vars;
_tmp18F->region_order=f->region_order;_tmp18F->locals=f->locals;_tmp18F->encloser=
f->encloser;_tmp18F->ctrl_env=f->ctrl_env;_tmp18F->capability=(void*)((void*)f->capability);
_tmp18F->curr_rgn=(void*)((void*)f->curr_rgn);_tmp18F;});}static struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_copy_fenv_new_ctrl(struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_CtrlEnv*c=f->ctrl_env;
return({struct Cyc_Tcenv_Fenv*_tmp190=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));
_tmp190->shared=f->shared;_tmp190->type_vars=f->type_vars;_tmp190->region_order=
f->region_order;_tmp190->locals=f->locals;_tmp190->encloser=f->encloser;_tmp190->ctrl_env=({
struct Cyc_Tcenv_CtrlEnv*_tmp191=_cycalloc(sizeof(struct Cyc_Tcenv_CtrlEnv));
_tmp191->continue_stmt=(void*)((void*)c->continue_stmt);_tmp191->break_stmt=(
void*)((void*)c->break_stmt);_tmp191->fallthru_clause=c->fallthru_clause;_tmp191->next_stmt=(
void*)((void*)c->next_stmt);_tmp191->try_depth=c->try_depth;_tmp191;});_tmp190->capability=(
void*)((void*)f->capability);_tmp190->curr_rgn=(void*)((void*)f->curr_rgn);
_tmp190;});}void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){if(te->le == 0){((
int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected return_typ",
sizeof(unsigned char),29));}return(void*)((Cyc_Tcenv_get_fenv(te))->shared)->return_typ;}
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){if(te->le
== 0){return 0;}return(Cyc_Tcenv_get_fenv(te))->type_vars;}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_type_vars(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*tvs){if(te->le == 0){((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected add_type_vars",sizeof(unsigned char),32));}{struct
Cyc_Tcenv_Fenv*_tmp192=Cyc_Tcenv_copy_fenv_old_ctrl(Cyc_Tcenv_get_fenv(te));Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_List_List*_tmp193=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmp192->type_vars);Cyc_Tcutil_check_unique_tvars(
loc,_tmp193);_tmp192->type_vars=_tmp193;return Cyc_Tcenv_put_fenv(te,_tmp192);}}}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){if(te->le == 0){((int(*)(
struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected add_local_var",
sizeof(unsigned char),32));}{struct Cyc_Tcenv_Fenv*_tmp194=Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv(te));struct _tagged_arr*_tmp195=(*vd->name).f2;{void*_tmp196=(*
vd->name).f1;_LL408: if(_tmp196 == (void*)Cyc_Absyn_Loc_n){goto _LL409;}else{goto
_LL410;}_LL410: goto _LL411;_LL409: goto _LL407;_LL411:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp19C=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));
_tmp19C[0]=({struct Cyc_Core_Impossible_struct _tmp19D;_tmp19D.tag=Cyc_Core_Impossible;
_tmp19D.f1=_tag_arr("add_local_var: called with Rel_n",sizeof(unsigned char),33);
_tmp19D;});_tmp19C;}));goto _LL407;_LL407:;}_tmp194->locals=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(_tmp194->locals,
_tmp195,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp19E=_cycalloc(sizeof(struct
Cyc_Absyn_Local_b_struct));_tmp19E[0]=({struct Cyc_Absyn_Local_b_struct _tmp19F;
_tmp19F.tag=Cyc_Absyn_Local_b;_tmp19F.f1=vd;_tmp19F;});_tmp19E;}));return Cyc_Tcenv_put_fenv(
te,_tmp194);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){if(te->le == 0){((int(*)(
struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected add_pat_var",
sizeof(unsigned char),30));}{struct Cyc_Tcenv_Fenv*_tmp1A0=Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv(te));struct _tagged_arr*_tmp1A1=(*vd->name).f2;_tmp1A0->locals=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(
_tmp1A0->locals,_tmp1A1,(void*)({struct Cyc_Absyn_Pat_b_struct*_tmp1A2=_cycalloc(
sizeof(struct Cyc_Absyn_Pat_b_struct));_tmp1A2[0]=({struct Cyc_Absyn_Pat_b_struct
_tmp1A3;_tmp1A3.tag=Cyc_Absyn_Pat_b;_tmp1A3.f1=vd;_tmp1A3;});_tmp1A2;}));return
Cyc_Tcenv_put_fenv(te,_tmp1A0);}}void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple0*q){struct _tuple0 _tmp1A6;struct
_tagged_arr*_tmp1A7;void*_tmp1A9;struct _tuple0*_tmp1A4=q;_tmp1A6=*_tmp1A4;_LL426:
_tmp1A9=_tmp1A6.f1;goto _LL424;_LL424: _tmp1A7=_tmp1A6.f2;goto _LL421;_LL421: {void*
_tmp1AB=_tmp1A9;struct Cyc_List_List*_tmp1B4;_LL429: if(_tmp1AB == (void*)Cyc_Absyn_Loc_n){
goto _LL430;}else{goto _LL431;}_LL431: if((unsigned int)_tmp1AB > 1?*((int*)_tmp1AB)
== Cyc_Absyn_Rel_n: 0){_LL437: _tmp1B4=((struct Cyc_Absyn_Rel_n_struct*)_tmp1AB)->f1;
if(_tmp1B4 == 0){goto _LL435;}else{goto _LL433;}}else{goto _LL433;}_LL435: if(te->le
!= 0){goto _LL432;}else{goto _LL433;}_LL433: goto _LL434;_LL430: if(te->le == 0){(int)
_throw((void*)Cyc_Dict_Absent);}else{goto _LL432;}_LL432: {struct Cyc_Tcenv_Fenv*
_tmp1B6=Cyc_Tcenv_get_fenv(te);struct _handler_cons _tmp1B7;_push_handler(& _tmp1B7);{
int _tmp1B9=0;if(setjmp(_tmp1B7.handler)){_tmp1B9=1;}if(! _tmp1B9){{void*_tmp1BC=(
void*)({struct Cyc_Tcenv_VarRes_struct*_tmp1BA=_cycalloc(sizeof(struct Cyc_Tcenv_VarRes_struct));
_tmp1BA[0]=({struct Cyc_Tcenv_VarRes_struct _tmp1BB;_tmp1BB.tag=Cyc_Tcenv_VarRes;
_tmp1BB.f1=(void*)((void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp1B6->locals,_tmp1A7);_tmp1BB;});_tmp1BA;});_npop_handler(0);return _tmp1BC;};
_pop_handler();}else{void*_tmp1B8=(void*)_exn_thrown;void*_tmp1BE=_tmp1B8;_LL448:
if(_tmp1BE == Cyc_Dict_Absent){goto _LL449;}else{goto _LL450;}_LL450: goto _LL451;
_LL449: return Cyc_Tcenv_lookup_ordinary_global(te,loc,q);_LL451:(void)_throw(
_tmp1BE);_LL447:;}}}_LL434: {struct _handler_cons _tmp1C4;_push_handler(& _tmp1C4);{
int _tmp1C6=0;if(setjmp(_tmp1C4.handler)){_tmp1C6=1;}if(! _tmp1C6){{void*_tmp1C7=
Cyc_Tcenv_lookup_ordinary_global(te,loc,q);_npop_handler(0);return _tmp1C7;};
_pop_handler();}else{void*_tmp1C5=(void*)_exn_thrown;void*_tmp1C9=_tmp1C5;_LL459:
if(_tmp1C9 == Cyc_Dict_Absent){goto _LL460;}else{goto _LL461;}_LL461: goto _LL462;
_LL460: return(void*)({struct Cyc_Tcenv_VarRes_struct*_tmp1CF=_cycalloc(sizeof(
struct Cyc_Tcenv_VarRes_struct));_tmp1CF[0]=({struct Cyc_Tcenv_VarRes_struct
_tmp1D0;_tmp1D0.tag=Cyc_Tcenv_VarRes;_tmp1D0.f1=(void*)((void*)Cyc_Absyn_Unresolved_b);
_tmp1D0;});_tmp1CF;});_LL462:(void)_throw(_tmp1C9);_LL458:;}}}_LL428:;}}void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){if(te->le
== 0){((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected process_continue",
sizeof(unsigned char),35));}{void*_tmp1D1=(void*)((Cyc_Tcenv_get_fenv(te))->ctrl_env)->continue_stmt;
struct Cyc_Absyn_Stmt*_tmp1DB;_LL467: if((unsigned int)_tmp1D1 > 3?*((int*)_tmp1D1)
== Cyc_Tcenv_Stmt_j: 0){_LL476: _tmp1DB=((struct Cyc_Tcenv_Stmt_j_struct*)_tmp1D1)->f1;
goto _LL468;}else{goto _LL469;}_LL469: if(_tmp1D1 == (void*)Cyc_Tcenv_NotLoop_j){
goto _LL470;}else{goto _LL471;}_LL471: if(_tmp1D1 == (void*)Cyc_Tcenv_CaseEnd_j){
goto _LL472;}else{goto _LL473;}_LL473: if(_tmp1D1 == (void*)Cyc_Tcenv_FnEnd_j){goto
_LL474;}else{goto _LL466;}_LL468: _tmp1DB->non_local_preds=({struct Cyc_List_List*
_tmp1DD=_cycalloc(sizeof(struct Cyc_List_List));_tmp1DD->hd=s;_tmp1DD->tl=_tmp1DB->non_local_preds;
_tmp1DD;});*sopt=(struct Cyc_Absyn_Stmt*)_tmp1DB;return;_LL470:({void*_tmp1DE[0]={};
Cyc_Tcutil_terr(s->loc,_tag_arr("continue not in a loop",sizeof(unsigned char),
23),_tag_arr(_tmp1DE,sizeof(void*),0));});return;_LL472:((int(*)(struct
_tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: continue dest is CaseEnd",
sizeof(unsigned char),32));return;_LL474:((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: continue dest is CaseEnd",sizeof(unsigned char),32));return;
_LL466:;}}void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Stmt**sopt){if(te->le == 0){((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected process_break",sizeof(unsigned char),32));}{struct
Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te);void*_tmp1DF=(void*)(fe->ctrl_env)->break_stmt;
struct Cyc_Absyn_Stmt*_tmp1E9;_LL481: if((unsigned int)_tmp1DF > 3?*((int*)_tmp1DF)
== Cyc_Tcenv_Stmt_j: 0){_LL490: _tmp1E9=((struct Cyc_Tcenv_Stmt_j_struct*)_tmp1DF)->f1;
goto _LL482;}else{goto _LL483;}_LL483: if(_tmp1DF == (void*)Cyc_Tcenv_NotLoop_j){
goto _LL484;}else{goto _LL485;}_LL485: if(_tmp1DF == (void*)Cyc_Tcenv_FnEnd_j){goto
_LL486;}else{goto _LL487;}_LL487: if(_tmp1DF == (void*)Cyc_Tcenv_CaseEnd_j){goto
_LL488;}else{goto _LL480;}_LL482: _tmp1E9->non_local_preds=({struct Cyc_List_List*
_tmp1EB=_cycalloc(sizeof(struct Cyc_List_List));_tmp1EB->hd=s;_tmp1EB->tl=_tmp1E9->non_local_preds;
_tmp1EB;});*sopt=(struct Cyc_Absyn_Stmt*)_tmp1E9;return;_LL484:({void*_tmp1EC[0]={};
Cyc_Tcutil_terr(s->loc,_tag_arr("break not in a loop or switch",sizeof(
unsigned char),30),_tag_arr(_tmp1EC,sizeof(void*),0));});return;_LL486: if((void*)(
fe->shared)->return_typ != (void*)Cyc_Absyn_VoidType){({void*_tmp1ED[0]={};Cyc_Tcutil_terr(
s->loc,_tag_arr("break causes function not to return a value",sizeof(
unsigned char),44),_tag_arr(_tmp1ED,sizeof(void*),0));});}return;_LL488:({void*
_tmp1EE[0]={};Cyc_Tcutil_terr(s->loc,_tag_arr("break causes outer switch clause to implicitly fallthru",
sizeof(unsigned char),56),_tag_arr(_tmp1EE,sizeof(void*),0));});return;_LL480:;}}
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct
_tagged_arr*l,struct Cyc_Absyn_Stmt**sopt){if(te->le == 0){((int(*)(struct
_tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected process_goto",
sizeof(unsigned char),31));}{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te);
struct Cyc_Core_Opt*_tmp1EF=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_lookup_opt)((fe->shared)->seen_labels,l);if(_tmp1EF == 0){
struct Cyc_SlowDict_Dict*_tmp1F0=(fe->shared)->needed_labels;struct Cyc_Core_Opt*
_tmp1F1=((struct Cyc_Core_Opt*(*)(struct Cyc_SlowDict_Dict*d,struct _tagged_arr*k))
Cyc_SlowDict_lookup_opt)(_tmp1F0,l);if(_tmp1F1 == 0){_tmp1F1=({struct Cyc_Core_Opt*
_tmp1F2=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1F2->v=0;_tmp1F2;});}(fe->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)(struct Cyc_SlowDict_Dict*d,struct _tagged_arr*k,struct
Cyc_List_List*v))Cyc_SlowDict_insert)(_tmp1F0,l,({struct Cyc_List_List*_tmp1F3=
_cycalloc(sizeof(struct Cyc_List_List));_tmp1F3->hd=s;_tmp1F3->tl=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1F1))->v;_tmp1F3;}));}else{((struct Cyc_Absyn_Stmt*)
_tmp1EF->v)->non_local_preds=({struct Cyc_List_List*_tmp1F4=_cycalloc(sizeof(
struct Cyc_List_List));_tmp1F4->hd=s;_tmp1F4->tl=((struct Cyc_Absyn_Stmt*)_tmp1EF->v)->non_local_preds;
_tmp1F4;});*sopt=(struct Cyc_Absyn_Stmt*)((struct Cyc_Absyn_Stmt*)_tmp1EF->v);}}}
struct _tuple3*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Switch_clause***clauseopt){if(te->le == 0){((int(*)(struct
_tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected process_fallthru",
sizeof(unsigned char),35));}{struct Cyc_Tcenv_Fenv*_tmp1F5=Cyc_Tcenv_get_fenv(te);
struct _tuple3*_tmp1F6=(_tmp1F5->ctrl_env)->fallthru_clause;if(_tmp1F6 != 0){(((*
_tmp1F6).f1)->body)->non_local_preds=({struct Cyc_List_List*_tmp1F7=_cycalloc(
sizeof(struct Cyc_List_List));_tmp1F7->hd=s;_tmp1F7->tl=(((*_tmp1F6).f1)->body)->non_local_preds;
_tmp1F7;});*clauseopt=(struct Cyc_Absyn_Switch_clause**)({struct Cyc_Absyn_Switch_clause**
_tmp1F8=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause*)* 1);_tmp1F8[0]=(*
_tmp1F6).f1;_tmp1F8;});}return _tmp1F6;}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,
struct Cyc_Absyn_Switch_clause*clause){if(te->le == 0){((int(*)(struct _tagged_arr
msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected set_fallthru",sizeof(
unsigned char),31));}{struct Cyc_List_List*_tmp1F9=0;for(0;vds != 0;vds=vds->tl){
_tmp1F9=({struct Cyc_List_List*_tmp1FA=_cycalloc(sizeof(struct Cyc_List_List));
_tmp1FA->hd=(void*)((void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type);_tmp1FA->tl=
_tmp1F9;_tmp1FA;});}_tmp1F9=Cyc_List_imp_rev(_tmp1F9);{struct Cyc_Tcenv_Fenv*fe=
Cyc_Tcenv_copy_fenv_new_ctrl(Cyc_Tcenv_get_fenv(te));(fe->ctrl_env)->fallthru_clause=({
struct _tuple3*_tmp1FB=_cycalloc(sizeof(struct _tuple3));_tmp1FB->f1=clause;
_tmp1FB->f2=new_tvs;_tmp1FB->f3=_tmp1F9;_tmp1FB;});return Cyc_Tcenv_put_fenv(te,
fe);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*te){
if(te->le == 0){((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected clear_fallthru",
sizeof(unsigned char),33));}{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv(te));(fe->ctrl_env)->fallthru_clause=0;return Cyc_Tcenv_put_fenv(
te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*continue_dest){if(te->le == 0){((int(*)(struct _tagged_arr msg))
Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected set_in_loop",sizeof(unsigned char),
30));}{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(Cyc_Tcenv_get_fenv(
te));(void*)((fe->ctrl_env)->continue_stmt=(void*)((void*)({struct Cyc_Tcenv_Stmt_j_struct*
_tmp1FC=_cycalloc(sizeof(struct Cyc_Tcenv_Stmt_j_struct));_tmp1FC[0]=({struct Cyc_Tcenv_Stmt_j_struct
_tmp1FD;_tmp1FD.tag=Cyc_Tcenv_Stmt_j;_tmp1FD.f1=continue_dest;_tmp1FD;});_tmp1FC;})));(
void*)((fe->ctrl_env)->break_stmt=(void*)((void*)(fe->ctrl_env)->next_stmt));(
void*)((fe->ctrl_env)->next_stmt=(void*)((void*)({struct Cyc_Tcenv_Stmt_j_struct*
_tmp1FE=_cycalloc(sizeof(struct Cyc_Tcenv_Stmt_j_struct));_tmp1FE[0]=({struct Cyc_Tcenv_Stmt_j_struct
_tmp1FF;_tmp1FF.tag=Cyc_Tcenv_Stmt_j;_tmp1FF.f1=continue_dest;_tmp1FF;});_tmp1FE;})));
return Cyc_Tcenv_put_fenv(te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct Cyc_Tcenv_Tenv*te){if(te->le == 0){((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected enter_try",sizeof(unsigned char),28));}{struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_new_ctrl(Cyc_Tcenv_get_fenv(te));++(fe->ctrl_env)->try_depth;
return Cyc_Tcenv_put_fenv(te,fe);}}int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*
te){if(te->le == 0){((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected get_try_depth",
sizeof(unsigned char),32));}return((Cyc_Tcenv_get_fenv(te))->ctrl_env)->try_depth;}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct Cyc_Tcenv_Tenv*te){if(te->le
== 0){((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected set_in_switch",
sizeof(unsigned char),32));}{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv(te));(void*)((fe->ctrl_env)->break_stmt=(void*)((void*)(fe->ctrl_env)->next_stmt));(
void*)((fe->ctrl_env)->next_stmt=(void*)((void*)Cyc_Tcenv_CaseEnd_j));return Cyc_Tcenv_put_fenv(
te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct Cyc_Tcenv_Tenv*te,void*j){
if(te->le == 0){((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected set_next",
sizeof(unsigned char),27));}{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(
Cyc_Tcenv_get_fenv(te));(void*)((fe->ctrl_env)->next_stmt=(void*)j);return Cyc_Tcenv_put_fenv(
te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct
_tagged_arr*v,struct Cyc_Absyn_Stmt*s){if(te->le == 0){((int(*)(struct _tagged_arr
msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected add_label",sizeof(
unsigned char),28));}{struct Cyc_Tcenv_Fenv*_tmp200=Cyc_Tcenv_get_fenv(te);struct
Cyc_SlowDict_Dict*_tmp201=(_tmp200->shared)->needed_labels;struct Cyc_Core_Opt*
_tmp202=((struct Cyc_Core_Opt*(*)(struct Cyc_SlowDict_Dict*d,struct _tagged_arr*k))
Cyc_SlowDict_lookup_opt)(_tmp201,v);if(_tmp202 != 0){(_tmp200->shared)->needed_labels=((
struct Cyc_SlowDict_Dict*(*)(struct Cyc_SlowDict_Dict*d,struct _tagged_arr*k))Cyc_SlowDict_delete)(
_tmp201,v);{struct Cyc_List_List*_tmp203=(struct Cyc_List_List*)_tmp202->v;s->non_local_preds=
_tmp203;for(0;_tmp203 != 0;_tmp203=_tmp203->tl){void*_tmp204=(void*)((struct Cyc_Absyn_Stmt*)
_tmp203->hd)->r;struct Cyc_Absyn_Stmt*_tmp20A;struct Cyc_Absyn_Stmt**_tmp20C;
_LL518: if((unsigned int)_tmp204 > 1?*((int*)_tmp204)== Cyc_Absyn_Goto_s: 0){_LL523:
_tmp20A=((struct Cyc_Absyn_Goto_s_struct*)_tmp204)->f2;_tmp20C=(struct Cyc_Absyn_Stmt**)&((
struct Cyc_Absyn_Goto_s_struct*)_tmp204)->f2;goto _LL519;}else{goto _LL520;}_LL520:
goto _LL521;_LL519:*_tmp20C=(struct Cyc_Absyn_Stmt*)s;goto _LL517;_LL521:((int(*)(
struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv: add_label backpatching of non-goto",
sizeof(unsigned char),42));goto _LL517;_LL517:;}}}if(((int(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_member)((_tmp200->shared)->seen_labels,v)){({
struct Cyc_Std_String_pa_struct _tmp20E;_tmp20E.tag=Cyc_Std_String_pa;_tmp20E.f1=(
struct _tagged_arr)*v;{void*_tmp20D[1]={& _tmp20E};Cyc_Tcutil_terr(s->loc,_tag_arr("Repeated label: %s",
sizeof(unsigned char),19),_tag_arr(_tmp20D,sizeof(void*),1));}});}(_tmp200->shared)->seen_labels=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Stmt*
v))Cyc_Dict_insert)((_tmp200->shared)->seen_labels,v,s);return te;}}int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*te){return((int(*)(struct Cyc_SlowDict_Dict*d))Cyc_SlowDict_is_empty)(((
Cyc_Tcenv_get_fenv(te))->shared)->needed_labels);}struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*te){if(te->le == 0){((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected get_encloser",sizeof(unsigned char),31));}return(Cyc_Tcenv_get_fenv(
te))->encloser;}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s){if(te->le == 0){((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("Tcenv: unexpected set_encloser",sizeof(unsigned char),31));}{struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_old_ctrl(Cyc_Tcenv_get_fenv(te));fe->encloser=s;return Cyc_Tcenv_put_fenv(
te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*
r){if(te->le == 0){((int(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(_tag_arr("Tcenv::add_region: unexpected add_region",
sizeof(unsigned char),41));}{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(
Cyc_Tcenv_get_fenv(te));struct Cyc_Absyn_Tvar*tv;{void*_tmp20F=Cyc_Tcutil_compress(
r);struct Cyc_Absyn_Tvar*_tmp215;_LL529: if((unsigned int)_tmp20F > 3?*((int*)
_tmp20F)== Cyc_Absyn_VarType: 0){_LL534: _tmp215=((struct Cyc_Absyn_VarType_struct*)
_tmp20F)->f1;goto _LL530;}else{goto _LL531;}_LL531: goto _LL532;_LL530: tv=_tmp215;
goto _LL528;_LL532: tv=((struct Cyc_Absyn_Tvar*(*)(struct _tagged_arr msg))Cyc_Tcenv_env_err)(
_tag_arr("bad add region",sizeof(unsigned char),15));goto _LL528;_LL528:;}fe->region_order=
Cyc_Tcenv_add_region_po(fe->region_order,tv);(void*)(fe->capability=(void*)((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp217=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));
_tmp217[0]=({struct Cyc_Absyn_JoinEff_struct _tmp218;_tmp218.tag=Cyc_Absyn_JoinEff;
_tmp218.f1=({struct Cyc_List_List*_tmp219=_cycalloc(sizeof(struct Cyc_List_List));
_tmp219->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp21B=_cycalloc(
sizeof(struct Cyc_Absyn_AccessEff_struct));_tmp21B[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp21C;_tmp21C.tag=Cyc_Absyn_AccessEff;_tmp21C.f1=(void*)r;_tmp21C;});_tmp21B;}));
_tmp219->tl=({struct Cyc_List_List*_tmp21A=_cycalloc(sizeof(struct Cyc_List_List));
_tmp21A->hd=(void*)((void*)fe->capability);_tmp21A->tl=0;_tmp21A;});_tmp219;});
_tmp218;});_tmp217;})));return Cyc_Tcenv_put_fenv(te,fe);}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_new_named_block(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Tvar*block_rgn){if(te->le == 0){((int(*)(struct _tagged_arr msg))
Cyc_Tcenv_env_err)(_tag_arr("Tcenv: unexpected new_block",sizeof(unsigned char),
28));}{struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(Cyc_Tcenv_get_fenv(
te));void*block_typ=(void*)({struct Cyc_Absyn_VarType_struct*_tmp224=_cycalloc(
sizeof(struct Cyc_Absyn_VarType_struct));_tmp224[0]=({struct Cyc_Absyn_VarType_struct
_tmp225;_tmp225.tag=Cyc_Absyn_VarType;_tmp225.f1=block_rgn;_tmp225;});_tmp224;});
fe->type_vars=({struct Cyc_List_List*_tmp21D=_cycalloc(sizeof(struct Cyc_List_List));
_tmp21D->hd=block_rgn;_tmp21D->tl=fe->type_vars;_tmp21D;});Cyc_Tcutil_check_unique_tvars(
loc,fe->type_vars);fe->region_order=Cyc_Tcenv_add_region_po(fe->region_order,
block_rgn);(void*)(fe->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp21E=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp21E[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp21F;_tmp21F.tag=Cyc_Absyn_JoinEff;_tmp21F.f1=({struct Cyc_List_List*_tmp220=
_cycalloc(sizeof(struct Cyc_List_List));_tmp220->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp222=_cycalloc(sizeof(struct Cyc_Absyn_AccessEff_struct));_tmp222[0]=({struct
Cyc_Absyn_AccessEff_struct _tmp223;_tmp223.tag=Cyc_Absyn_AccessEff;_tmp223.f1=(
void*)block_typ;_tmp223;});_tmp222;}));_tmp220->tl=({struct Cyc_List_List*_tmp221=
_cycalloc(sizeof(struct Cyc_List_List));_tmp221->hd=(void*)((void*)fe->capability);
_tmp221->tl=0;_tmp221;});_tmp220;});_tmp21F;});_tmp21E;})));(void*)(fe->curr_rgn=(
void*)block_typ);return Cyc_Tcenv_put_fenv(te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te){struct Cyc_Absyn_Tvar*t=
Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp226=_cycalloc(
sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp226[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp227;_tmp227.tag=Cyc_Absyn_Eq_kb;_tmp227.f1=(void*)((void*)Cyc_Absyn_RgnKind);
_tmp227;});_tmp226;}));Cyc_Tcutil_add_tvar_identity(t);return Cyc_Tcenv_new_named_block(
loc,te,t);}void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){if(te->le == 0){
return(void*)Cyc_Absyn_HeapRgn;}return(void*)(Cyc_Tcenv_get_fenv(te))->curr_rgn;}
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*rgn){if(te->le != 0){void*capability=(void*)(Cyc_Tcenv_get_fenv(te))->capability;
if(Cyc_Tcutil_region_in_effect(0,rgn,capability)? 1: Cyc_Tcutil_region_in_effect(1,
rgn,capability)){return;}}({struct Cyc_Std_String_pa_struct _tmp229;_tmp229.tag=
Cyc_Std_String_pa;_tmp229.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(rgn);{
void*_tmp228[1]={& _tmp229};Cyc_Tcutil_terr(loc,_tag_arr("Expression accesses unavailable region %s",
sizeof(unsigned char),42),_tag_arr(_tmp228,sizeof(void*),1));}});}int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){rt_a=Cyc_Tcutil_compress(rt_a);rt_b=
Cyc_Tcutil_compress(rt_b);if(rt_a == (void*)Cyc_Absyn_HeapRgn){return 1;}if(te->le
!= 0){struct _tuple5 _tmp22B=({struct _tuple5 _tmp22A;_tmp22A.f1=rt_a;_tmp22A.f2=
rt_b;_tmp22A;});void*_tmp231;struct Cyc_Absyn_Tvar*_tmp233;void*_tmp235;struct Cyc_Absyn_Tvar*
_tmp237;_LL557: _LL566: _tmp235=_tmp22B.f1;if((unsigned int)_tmp235 > 3?*((int*)
_tmp235)== Cyc_Absyn_VarType: 0){_LL568: _tmp237=((struct Cyc_Absyn_VarType_struct*)
_tmp235)->f1;goto _LL562;}else{goto _LL559;}_LL562: _tmp231=_tmp22B.f2;if((
unsigned int)_tmp231 > 3?*((int*)_tmp231)== Cyc_Absyn_VarType: 0){_LL564: _tmp233=((
struct Cyc_Absyn_VarType_struct*)_tmp231)->f1;goto _LL558;}else{goto _LL559;}_LL559:
goto _LL560;_LL558: return Cyc_Tcenv_check_region_outlives((Cyc_Tcenv_get_fenv(te))->region_order,
_tmp237,_tmp233);_LL560: return 0;_LL556:;}return 0;}void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff){void*capability;
if(te->le == 0){capability=(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp239=
_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp239[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp23A;_tmp23A.tag=Cyc_Absyn_JoinEff;_tmp23A.f1=0;_tmp23A;});_tmp239;});}else{
struct Cyc_Tcenv_Fenv*_tmp23B=Cyc_Tcenv_get_fenv(te);capability=(void*)_tmp23B->capability;}
if(! Cyc_Tcutil_subset_effect(1,eff,capability)){({struct Cyc_Std_String_pa_struct
_tmp23E;_tmp23E.tag=Cyc_Std_String_pa;_tmp23E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
eff);{struct Cyc_Std_String_pa_struct _tmp23D;_tmp23D.tag=Cyc_Std_String_pa;
_tmp23D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(capability);{void*_tmp23C[2]={&
_tmp23D,& _tmp23E};Cyc_Tcutil_terr(loc,_tag_arr("Capability %s does not cover function's effect %s",
sizeof(unsigned char),50),_tag_arr(_tmp23C,sizeof(void*),2));}}});}}void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po){
for(0;po != 0;po=po->tl){struct _tuple5 _tmp241;void*_tmp242;void*_tmp244;struct
_tuple5*_tmp23F=(struct _tuple5*)po->hd;_tmp241=*_tmp23F;_LL581: _tmp244=_tmp241.f1;
goto _LL579;_LL579: _tmp242=_tmp241.f2;goto _LL576;_LL576: if(! Cyc_Tcenv_region_outlives(
te,_tmp244,_tmp242)){({struct Cyc_Std_String_pa_struct _tmp248;_tmp248.tag=Cyc_Std_String_pa;
_tmp248.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp242);{struct Cyc_Std_String_pa_struct
_tmp247;_tmp247.tag=Cyc_Std_String_pa;_tmp247.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp244);{void*_tmp246[2]={& _tmp247,& _tmp248};Cyc_Tcutil_terr(loc,_tag_arr("function requires that region %s outlive %s",
sizeof(unsigned char),44),_tag_arr(_tmp246,sizeof(void*),2));}}});}}}struct
_tuple6{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Tcenv_Fenv*
Cyc_Tcenv_new_fenv(struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Dict_Dict*locals=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,
struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);struct Cyc_Absyn_Tvar*
rgn0=({struct Cyc_Absyn_Tvar*_tmp278=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_tmp278->name=({struct _tagged_arr*_tmp27B=_cycalloc(sizeof(struct _tagged_arr));
_tmp27B[0]=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp27D;_tmp27D.tag=
Cyc_Std_String_pa;_tmp27D.f1=(struct _tagged_arr)*(*fd->name).f2;{void*_tmp27C[1]={&
_tmp27D};Cyc_Std_aprintf(_tag_arr("`%s",sizeof(unsigned char),4),_tag_arr(
_tmp27C,sizeof(void*),1));}});_tmp27B;});_tmp278->identity=Cyc_Tcutil_new_tvar_id();
_tmp278->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp279=_cycalloc(
sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp279[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp27A;_tmp27A.tag=Cyc_Absyn_Eq_kb;_tmp27A.f1=(void*)((void*)Cyc_Absyn_RgnKind);
_tmp27A;});_tmp279;}));_tmp278;});struct Cyc_List_List*_tmp249=({struct Cyc_List_List*
_tmp277=_cycalloc(sizeof(struct Cyc_List_List));_tmp277->hd=rgn0;_tmp277->tl=fd->tvs;
_tmp277;});Cyc_Tcutil_check_unique_tvars(loc,_tmp249);{struct Cyc_List_List*
_tmp24A=Cyc_Tcenv_add_region_po(Cyc_Tcenv_initial_region_po(loc,fd->tvs,fd->rgn_po),
rgn0);void*param_rgn=(void*)({struct Cyc_Absyn_VarType_struct*_tmp275=_cycalloc(
sizeof(struct Cyc_Absyn_VarType_struct));_tmp275[0]=({struct Cyc_Absyn_VarType_struct
_tmp276;_tmp276.tag=Cyc_Absyn_VarType;_tmp276.f1=rgn0;_tmp276;});_tmp275;});
struct Cyc_List_List*_tmp24B=0;{struct Cyc_List_List*_tmp24C=fd->args;for(0;
_tmp24C != 0;_tmp24C=_tmp24C->tl){struct Cyc_Absyn_Vardecl*_tmp24D=({struct Cyc_Absyn_Vardecl*
_tmp252=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp252[0]=({struct Cyc_Absyn_Vardecl
_tmp253;_tmp253.sc=(void*)((void*)Cyc_Absyn_Public);_tmp253.name=({struct _tuple0*
_tmp255=_cycalloc(sizeof(struct _tuple0));_tmp255->f1=(void*)Cyc_Absyn_Loc_n;
_tmp255->f2=(*((struct _tuple6*)_tmp24C->hd)).f1;_tmp255;});_tmp253.tq=(*((struct
_tuple6*)_tmp24C->hd)).f2;_tmp253.type=(void*)(*((struct _tuple6*)_tmp24C->hd)).f3;
_tmp253.initializer=0;_tmp253.rgn=({struct Cyc_Core_Opt*_tmp254=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmp254->v=(void*)param_rgn;_tmp254;});_tmp253.attributes=0;
_tmp253.escapes=0;_tmp253;});_tmp252;});_tmp24B=({struct Cyc_List_List*_tmp24E=
_cycalloc(sizeof(struct Cyc_List_List)* 1);_tmp24E[0]=({struct Cyc_List_List
_tmp24F;_tmp24F.hd=_tmp24D;_tmp24F.tl=_tmp24B;_tmp24F;});_tmp24E;});locals=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(
locals,(*((struct _tuple6*)_tmp24C->hd)).f1,(void*)({struct Cyc_Absyn_Param_b_struct*
_tmp250=_cycalloc(sizeof(struct Cyc_Absyn_Param_b_struct));_tmp250[0]=({struct Cyc_Absyn_Param_b_struct
_tmp251;_tmp251.tag=Cyc_Absyn_Param_b;_tmp251.f1=_tmp24D;_tmp251;});_tmp250;}));}}
if(fd->cyc_varargs != 0){int _tmp258;void*_tmp25A;struct Cyc_Absyn_Tqual _tmp25C;
struct Cyc_Core_Opt*_tmp25E;struct Cyc_Absyn_VarargInfo _tmp256=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_LL607: _tmp25E=_tmp256.name;goto _LL605;_LL605:
_tmp25C=_tmp256.tq;goto _LL603;_LL603: _tmp25A=(void*)_tmp256.type;goto _LL601;
_LL601: _tmp258=_tmp256.inject;goto _LL599;_LL599: if(_tmp25E != 0){void*_tmp260=Cyc_Absyn_tagged_typ(
_tmp25A,param_rgn,_tmp25C);struct Cyc_Absyn_Vardecl*_tmp261=({struct Cyc_Absyn_Vardecl*
_tmp265=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1);_tmp265[0]=({struct Cyc_Absyn_Vardecl
_tmp266;_tmp266.sc=(void*)((void*)Cyc_Absyn_Public);_tmp266.name=({struct _tuple0*
_tmp268=_cycalloc(sizeof(struct _tuple0));_tmp268->f1=(void*)Cyc_Absyn_Loc_n;
_tmp268->f2=(struct _tagged_arr*)_tmp25E->v;_tmp268;});_tmp266.tq=Cyc_Absyn_empty_tqual();
_tmp266.type=(void*)_tmp260;_tmp266.initializer=0;_tmp266.rgn=({struct Cyc_Core_Opt*
_tmp267=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp267->v=(void*)param_rgn;
_tmp267;});_tmp266.attributes=0;_tmp266.escapes=0;_tmp266;});_tmp265;});_tmp24B=({
struct Cyc_List_List*_tmp262=_cycalloc(sizeof(struct Cyc_List_List));_tmp262->hd=
_tmp261;_tmp262->tl=_tmp24B;_tmp262;});locals=((struct Cyc_Dict_Dict*(*)(struct
Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(locals,(struct
_tagged_arr*)_tmp25E->v,(void*)({struct Cyc_Absyn_Param_b_struct*_tmp263=
_cycalloc(sizeof(struct Cyc_Absyn_Param_b_struct));_tmp263[0]=({struct Cyc_Absyn_Param_b_struct
_tmp264;_tmp264.tag=Cyc_Absyn_Param_b;_tmp264.f1=_tmp261;_tmp264;});_tmp263;}));}
else{({void*_tmp269[0]={};Cyc_Tcutil_terr(loc,_tag_arr("missing name for varargs",
sizeof(unsigned char),25),_tag_arr(_tmp269,sizeof(void*),0));});}}fd->param_vardecls=({
struct Cyc_Core_Opt*_tmp26A=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1);_tmp26A[0]=({
struct Cyc_Core_Opt _tmp26B;_tmp26B.v=_tmp24B;_tmp26B;});_tmp26A;});return({struct
Cyc_Tcenv_Fenv*_tmp26C=_cycalloc(sizeof(struct Cyc_Tcenv_Fenv));_tmp26C->shared=({
struct Cyc_Tcenv_SharedFenv*_tmp274=_cycalloc(sizeof(struct Cyc_Tcenv_SharedFenv));
_tmp274->return_typ=(void*)((void*)fd->ret_type);_tmp274->seen_labels=((struct
Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp274->needed_labels=((struct Cyc_SlowDict_Dict*(*)(int(*
cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_SlowDict_empty)(Cyc_Std_zstrptrcmp);
_tmp274;});_tmp26C->type_vars=_tmp249;_tmp26C->region_order=_tmp24A;_tmp26C->locals=
locals;_tmp26C->encloser=fd->body;_tmp26C->ctrl_env=({struct Cyc_Tcenv_CtrlEnv*
_tmp273=_cycalloc(sizeof(struct Cyc_Tcenv_CtrlEnv));_tmp273->continue_stmt=(void*)((
void*)Cyc_Tcenv_NotLoop_j);_tmp273->break_stmt=(void*)((void*)Cyc_Tcenv_NotLoop_j);
_tmp273->fallthru_clause=0;_tmp273->next_stmt=(void*)((void*)Cyc_Tcenv_FnEnd_j);
_tmp273->try_depth=0;_tmp273;});_tmp26C->capability=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp26D=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp26D[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp26E;_tmp26E.tag=Cyc_Absyn_JoinEff;_tmp26E.f1=({struct Cyc_List_List*_tmp26F=
_cycalloc(sizeof(struct Cyc_List_List));_tmp26F->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp271=_cycalloc(sizeof(struct Cyc_Absyn_AccessEff_struct));_tmp271[0]=({struct
Cyc_Absyn_AccessEff_struct _tmp272;_tmp272.tag=Cyc_Absyn_AccessEff;_tmp272.f1=(
void*)param_rgn;_tmp272;});_tmp271;}));_tmp26F->tl=({struct Cyc_List_List*_tmp270=
_cycalloc(sizeof(struct Cyc_List_List));_tmp270->hd=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v);_tmp270->tl=0;_tmp270;});_tmp26F;});_tmp26E;});
_tmp26D;}));_tmp26C->curr_rgn=(void*)param_rgn;_tmp26C;});}}
