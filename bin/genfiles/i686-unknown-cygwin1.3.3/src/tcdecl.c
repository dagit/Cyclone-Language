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
unsigned char*tag;struct _tagged_arr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern struct Cyc_Std___sFILE*Cyc_Std_stderr;
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
static const int Cyc_Std_String_pa=0;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};static const int Cyc_Std_Int_pa=1;struct Cyc_Std_Int_pa_struct{int
tag;unsigned int f1;};static const int Cyc_Std_Double_pa=2;struct Cyc_Std_Double_pa_struct{
int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=3;struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=4;struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_vrprintf(struct
_RegionHandle*r1,struct _tagged_arr fmt,struct _tagged_arr ap);static const int Cyc_Std_ShortPtr_sa=
0;struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};static const int Cyc_Std_UShortPtr_sa=
1;struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=
2;struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=
3;struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=
4;struct Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};static const int
Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};
static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{int tag;
float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern int Cyc_List_length(
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,
void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
extern unsigned char Cyc_List_Nth[8];extern int Cyc_List_exists_c(int(*pred)(void*,
void*),void*env,struct Cyc_List_List*x);struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;static const int Cyc_Position_Lex=0;static const int Cyc_Position_Parse=
1;static const int Cyc_Position_Elab=2;struct Cyc_Position_Error{struct _tagged_arr
source;struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _tagged_arr);extern unsigned char Cyc_Position_Nocontext[14];extern void Cyc_Position_post_error(
struct Cyc_Position_Error*);static const int Cyc_Absyn_Loc_n=0;static const int Cyc_Absyn_Rel_n=
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
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);extern struct Cyc_Absyn_Tqual
Cyc_Absyn_empty_tqual();extern void*Cyc_Absyn_force_kb(void*kb);extern struct
_tagged_arr Cyc_Absyn_attribute2string(void*);extern unsigned char Cyc_Tcdecl_Incompatible[
17];struct Cyc_Tcdecl_Xtunionfielddecl{struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*
field;};extern void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg1,struct _tagged_arr fmt,struct _tagged_arr ap);struct _tuple3{void*f1;
int f2;};extern struct _tuple3 Cyc_Tcdecl_merge_scope(void*s0,void*s1,struct
_tagged_arr t,struct _tagged_arr v,struct Cyc_Position_Segment*loc,struct _tagged_arr*
msg);extern struct Cyc_Absyn_Structdecl*Cyc_Tcdecl_merge_structdecl(struct Cyc_Absyn_Structdecl*
d0,struct Cyc_Absyn_Structdecl*d1,struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg);extern struct Cyc_Absyn_Uniondecl*Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl*d0,struct Cyc_Absyn_Uniondecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Tuniondecl*Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);extern void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_Tcdecl_Xtunionfielddecl*
Cyc_Tcdecl_merge_xtunionfielddecl(struct Cyc_Tcdecl_Xtunionfielddecl*d0,struct Cyc_Tcdecl_Xtunionfielddecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_List_List*
Cyc_Tcdecl_sort_xtunion_fields(struct Cyc_List_List*f,int*res,struct _tagged_arr*v,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Dict_Dict;extern
unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];extern
int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);extern int Cyc_Std_zstrptrcmp(
struct _tagged_arr*,struct _tagged_arr*);extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr,struct _tagged_arr);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[
11];static const int Cyc_Tcenv_VarRes=0;struct Cyc_Tcenv_VarRes_struct{int tag;void*
f1;};static const int Cyc_Tcenv_StructRes=1;struct Cyc_Tcenv_StructRes_struct{int tag;
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
ae;struct Cyc_Core_Opt*le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void
Cyc_Tcutil_explain_failure();extern int Cyc_Tcutil_unify(void*,void*);extern void*
Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);extern int Cyc_Tcutil_same_atts(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern struct
_tagged_arr Cyc_Absynpp_typ2string(void*);extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*);extern struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*);extern
struct _tagged_arr Cyc_Absynpp_scope2string(void*sc);unsigned char Cyc_Tcdecl_Incompatible[
17]="\000\000\000\000Incompatible";void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg1,struct _tagged_arr fmt,struct _tagged_arr ap){if(msg1 == 0){(
int)_throw((void*)Cyc_Tcdecl_Incompatible);}{struct _tagged_arr fmt2=(struct
_tagged_arr)Cyc_Std_strconcat(_tag_arr("%s ",sizeof(unsigned char),4),fmt);
struct _tagged_arr ap2=({unsigned int _tmp0=_get_arr_size(ap,sizeof(void*))+ 1;void**
_tmp1=(void**)_cycalloc(_check_times(sizeof(void*),_tmp0));struct _tagged_arr
_tmp5=_tag_arr(_tmp1,sizeof(void*),_get_arr_size(ap,sizeof(void*))+ 1);{
unsigned int _tmp2=_tmp0;unsigned int i;for(i=0;i < _tmp2;i ++){_tmp1[i]=i == 0?(void*)({
struct Cyc_Std_String_pa_struct*_tmp3=_cycalloc(sizeof(struct Cyc_Std_String_pa_struct));
_tmp3[0]=({struct Cyc_Std_String_pa_struct _tmp4;_tmp4.tag=Cyc_Std_String_pa;_tmp4.f1=(
struct _tagged_arr)*msg1;_tmp4;});_tmp3;}):*((void**)_check_unknown_subscript(ap,
sizeof(void*),(int)(i - 1)));}};_tmp5;});Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,fmt2,ap2)));}}
static void Cyc_Tcdecl_merge_scope_err(void*s0,void*s1,struct _tagged_arr t,struct
_tagged_arr v,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){({struct Cyc_Std_String_pa_struct
_tmpA;_tmpA.tag=Cyc_Std_String_pa;_tmpA.f1=(struct _tagged_arr)Cyc_Absynpp_scope2string(
s0);{struct Cyc_Std_String_pa_struct _tmp9;_tmp9.tag=Cyc_Std_String_pa;_tmp9.f1=(
struct _tagged_arr)Cyc_Absynpp_scope2string(s1);{struct Cyc_Std_String_pa_struct
_tmp8;_tmp8.tag=Cyc_Std_String_pa;_tmp8.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp7;_tmp7.tag=Cyc_Std_String_pa;_tmp7.f1=(struct _tagged_arr)t;{void*_tmp6[4]={&
_tmp7,& _tmp8,& _tmp9,& _tmpA};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s is %s whereas expected scope is %s",
sizeof(unsigned char),41),_tag_arr(_tmp6,sizeof(void*),4));}}}}});}struct _tuple4{
void*f1;void*f2;};struct _tuple3 Cyc_Tcdecl_merge_scope(void*s0,void*s1,struct
_tagged_arr t,struct _tagged_arr v,struct Cyc_Position_Segment*loc,struct _tagged_arr*
msg){{struct _tuple4 _tmpC=({struct _tuple4 _tmpB;_tmpB.f1=s0;_tmpB.f2=s1;_tmpB;});
void*_tmp20;void*_tmp22;void*_tmp24;void*_tmp26;void*_tmp28;void*_tmp2A;void*
_tmp2C;void*_tmp2E;void*_tmp30;void*_tmp32;void*_tmp34;void*_tmp36;_LL14: _LL35:
_tmp22=_tmpC.f1;if(_tmp22 == (void*)Cyc_Absyn_ExternC){goto _LL33;}else{goto _LL16;}
_LL33: _tmp20=_tmpC.f2;if(_tmp20 == (void*)Cyc_Absyn_ExternC){goto _LL15;}else{goto
_LL16;}_LL16: _LL37: _tmp24=_tmpC.f1;if(_tmp24 == (void*)Cyc_Absyn_ExternC){goto
_LL17;}else{goto _LL18;}_LL18: _LL39: _tmp26=_tmpC.f2;if(_tmp26 == (void*)Cyc_Absyn_ExternC){
goto _LL19;}else{goto _LL20;}_LL20: _LL41: _tmp28=_tmpC.f2;if(_tmp28 == (void*)Cyc_Absyn_Extern){
goto _LL21;}else{goto _LL22;}_LL22: _LL43: _tmp2A=_tmpC.f1;if(_tmp2A == (void*)Cyc_Absyn_Extern){
goto _LL23;}else{goto _LL24;}_LL24: _LL47: _tmp2E=_tmpC.f1;if(_tmp2E == (void*)Cyc_Absyn_Static){
goto _LL45;}else{goto _LL26;}_LL45: _tmp2C=_tmpC.f2;if(_tmp2C == (void*)Cyc_Absyn_Static){
goto _LL25;}else{goto _LL26;}_LL26: _LL51: _tmp32=_tmpC.f1;if(_tmp32 == (void*)Cyc_Absyn_Public){
goto _LL49;}else{goto _LL28;}_LL49: _tmp30=_tmpC.f2;if(_tmp30 == (void*)Cyc_Absyn_Public){
goto _LL27;}else{goto _LL28;}_LL28: _LL55: _tmp36=_tmpC.f1;if(_tmp36 == (void*)Cyc_Absyn_Abstract){
goto _LL53;}else{goto _LL30;}_LL53: _tmp34=_tmpC.f2;if(_tmp34 == (void*)Cyc_Absyn_Abstract){
goto _LL29;}else{goto _LL30;}_LL30: goto _LL31;_LL15: goto _LL13;_LL17: goto _LL19;_LL19:
Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple3 _tmp38;_tmp38.f1=
s1;_tmp38.f2=0;_tmp38;});_LL21: s1=s0;goto _LL13;_LL23: goto _LL13;_LL25: goto _LL27;
_LL27: goto _LL29;_LL29: goto _LL13;_LL31: Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,
msg);return({struct _tuple3 _tmp39;_tmp39.f1=s1;_tmp39.f2=0;_tmp39;});_LL13:;}
return({struct _tuple3 _tmp3A;_tmp3A.f1=s1;_tmp3A.f2=1;_tmp3A;});}static int Cyc_Tcdecl_check_type(
void*t0,void*t1){return Cyc_Tcutil_unify(t0,t1);}static unsigned int Cyc_Tcdecl_get_uint_const_value(
struct Cyc_Absyn_Exp*e){void*_tmp3B=(void*)e->r;void*_tmp41;int _tmp43;_LL61: if(*((
int*)_tmp3B)== Cyc_Absyn_Const_e){_LL66: _tmp41=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp3B)->f1;if((unsigned int)_tmp41 > 1?*((int*)_tmp41)== Cyc_Absyn_Int_c: 0){
_LL68: _tmp43=((struct Cyc_Absyn_Int_c_struct*)_tmp41)->f2;goto _LL62;}else{goto
_LL63;}}else{goto _LL63;}_LL63: goto _LL64;_LL62: return(unsigned int)_tmp43;_LL64:(
int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp45=_cycalloc(
sizeof(struct Cyc_Core_Invalid_argument_struct));_tmp45[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp46;_tmp46.tag=Cyc_Core_Invalid_argument;_tmp46.f1=_tag_arr("Tcdecl::get_uint_const_value",
sizeof(unsigned char),29);_tmp46;});_tmp45;}));_LL60:;}inline static int Cyc_Tcdecl_check_tvs(
struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,struct _tagged_arr t,struct
_tagged_arr v,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(tvs0)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tvs1)){({struct Cyc_Std_String_pa_struct _tmp49;_tmp49.tag=Cyc_Std_String_pa;
_tmp49.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct _tmp48;_tmp48.tag=
Cyc_Std_String_pa;_tmp48.f1=(struct _tagged_arr)t;{void*_tmp47[2]={& _tmp48,&
_tmp49};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s has a different number of type parameters",
sizeof(unsigned char),48),_tag_arr(_tmp47,sizeof(void*),2));}}});return 0;}{
struct Cyc_List_List*_tmp4A=tvs0;struct Cyc_List_List*_tmp4B=tvs1;for(0;_tmp4A != 0;(
_tmp4A=_tmp4A->tl,_tmp4B=_tmp4B->tl)){void*_tmp4C=Cyc_Absyn_force_kb((void*)((
struct Cyc_Absyn_Tvar*)_tmp4A->hd)->kind);void*_tmp4D=Cyc_Absyn_force_kb((void*)((
struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp4B))->hd)->kind);
if(_tmp4C == _tmp4D){continue;}({struct Cyc_Std_String_pa_struct _tmp53;_tmp53.tag=
Cyc_Std_String_pa;_tmp53.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp4D);{
struct Cyc_Std_String_pa_struct _tmp52;_tmp52.tag=Cyc_Std_String_pa;_tmp52.f1=(
struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)_tmp4A->hd)->name;{struct Cyc_Std_String_pa_struct
_tmp51;_tmp51.tag=Cyc_Std_String_pa;_tmp51.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp4C);{struct Cyc_Std_String_pa_struct _tmp50;_tmp50.tag=Cyc_Std_String_pa;
_tmp50.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct _tmp4F;_tmp4F.tag=
Cyc_Std_String_pa;_tmp4F.f1=(struct _tagged_arr)t;{void*_tmp4E[5]={& _tmp4F,&
_tmp50,& _tmp51,& _tmp52,& _tmp53};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s has a different kind (%s) for type parameter %s (%s)",
sizeof(unsigned char),59),_tag_arr(_tmp4E,sizeof(void*),5));}}}}}});return 0;}
return 1;}}static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*
atts1,struct _tagged_arr t,struct _tagged_arr v,struct Cyc_Position_Segment*loc,
struct _tagged_arr*msg){if(! Cyc_Tcutil_same_atts(atts0,atts1)){({struct Cyc_Std_String_pa_struct
_tmp56;_tmp56.tag=Cyc_Std_String_pa;_tmp56.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp55;_tmp55.tag=Cyc_Std_String_pa;_tmp55.f1=(struct _tagged_arr)t;{void*_tmp54[
2]={& _tmp55,& _tmp56};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s has different attributes",
sizeof(unsigned char),31),_tag_arr(_tmp54,sizeof(void*),2));}}});return 0;}return
1;}struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(
struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){struct Cyc_List_List*_tmp57=0;
for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){_tmp57=({struct Cyc_List_List*
_tmp58=_cycalloc(sizeof(struct Cyc_List_List));_tmp58->hd=({struct _tuple5*_tmp59=
_cycalloc(sizeof(struct _tuple5));_tmp59->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(tvs1))->hd;_tmp59->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp5A=
_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));_tmp5A[0]=({struct Cyc_Absyn_VarType_struct
_tmp5B;_tmp5B.tag=Cyc_Absyn_VarType;_tmp5B.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd;
_tmp5B;});_tmp5A;});_tmp59;});_tmp58->tl=_tmp57;_tmp58;});}return _tmp57;}struct
_tuple6{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};static struct Cyc_Absyn_Structdecl*
Cyc_Tcdecl_merge_struct_or_union_decl(struct Cyc_Absyn_Structdecl*d0,struct Cyc_Absyn_Structdecl*
d1,struct _tagged_arr t,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){
struct _tagged_arr _tmp5C=Cyc_Absynpp_qvar2string((struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(d0->name))->v);int _tmp5D=1;if(! Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,
t,_tmp5C,loc,msg)){return 0;}{int _tmp60;void*_tmp62;struct _tuple3 _tmp5E=Cyc_Tcdecl_merge_scope((
void*)d0->sc,(void*)d1->sc,t,_tmp5C,loc,msg);_LL99: _tmp62=_tmp5E.f1;goto _LL97;
_LL97: _tmp60=_tmp5E.f2;goto _LL95;_LL95: if(! _tmp60){_tmp5D=0;}if(! Cyc_Tcdecl_check_atts(
d0->attributes,d1->attributes,t,_tmp5C,loc,msg)){_tmp5D=0;}{struct Cyc_Absyn_Structdecl*
d2;{struct _tuple6 _tmp65=({struct _tuple6 _tmp64;_tmp64.f1=d0->fields;_tmp64.f2=d1->fields;
_tmp64;});struct Cyc_Core_Opt*_tmp6D;struct Cyc_Core_Opt*_tmp6F;struct Cyc_Core_Opt*
_tmp71;struct Cyc_Core_Opt _tmp73;struct Cyc_List_List*_tmp74;struct Cyc_Core_Opt*
_tmp76;struct Cyc_Core_Opt _tmp78;struct Cyc_List_List*_tmp79;_LL103: _LL110: _tmp6D=
_tmp65.f2;if(_tmp6D == 0){goto _LL104;}else{goto _LL105;}_LL105: _LL112: _tmp6F=
_tmp65.f1;if(_tmp6F == 0){goto _LL106;}else{goto _LL107;}_LL107: _LL119: _tmp76=
_tmp65.f1;if(_tmp76 == 0){goto _LL102;}else{_tmp78=*_tmp76;_LL122: _tmp79=(struct
Cyc_List_List*)_tmp78.v;goto _LL114;}_LL114: _tmp71=_tmp65.f2;if(_tmp71 == 0){goto
_LL102;}else{_tmp73=*_tmp71;_LL117: _tmp74=(struct Cyc_List_List*)_tmp73.v;goto
_LL108;}_LL104: d2=d0;goto _LL102;_LL106: d2=d1;goto _LL102;_LL108: {struct Cyc_List_List*
_tmp7B=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);for(0;_tmp79 != 0? _tmp74 != 0: 0;(
_tmp79=_tmp79->tl,_tmp74=_tmp74->tl)){struct Cyc_Absyn_Structfield _tmp7E;struct
Cyc_List_List*_tmp7F;struct Cyc_Absyn_Exp*_tmp81;void*_tmp83;struct Cyc_Absyn_Tqual
_tmp85;struct _tagged_arr*_tmp87;struct Cyc_Absyn_Structfield*_tmp7C=(struct Cyc_Absyn_Structfield*)
_tmp79->hd;_tmp7E=*_tmp7C;_LL136: _tmp87=_tmp7E.name;goto _LL134;_LL134: _tmp85=
_tmp7E.tq;goto _LL132;_LL132: _tmp83=(void*)_tmp7E.type;goto _LL130;_LL130: _tmp81=
_tmp7E.width;goto _LL128;_LL128: _tmp7F=_tmp7E.attributes;goto _LL125;_LL125: {
struct Cyc_Absyn_Structfield _tmp8B;struct Cyc_List_List*_tmp8C;struct Cyc_Absyn_Exp*
_tmp8E;void*_tmp90;struct Cyc_Absyn_Tqual _tmp92;struct _tagged_arr*_tmp94;struct
Cyc_Absyn_Structfield*_tmp89=(struct Cyc_Absyn_Structfield*)_tmp74->hd;_tmp8B=*
_tmp89;_LL149: _tmp94=_tmp8B.name;goto _LL147;_LL147: _tmp92=_tmp8B.tq;goto _LL145;
_LL145: _tmp90=(void*)_tmp8B.type;goto _LL143;_LL143: _tmp8E=_tmp8B.width;goto
_LL141;_LL141: _tmp8C=_tmp8B.attributes;goto _LL138;_LL138: if(Cyc_Std_zstrptrcmp(
_tmp87,_tmp94)!= 0){({struct Cyc_Std_String_pa_struct _tmp9A;_tmp9A.tag=Cyc_Std_String_pa;
_tmp9A.f1=(struct _tagged_arr)*_tmp94;{struct Cyc_Std_String_pa_struct _tmp99;
_tmp99.tag=Cyc_Std_String_pa;_tmp99.f1=(struct _tagged_arr)*_tmp87;{struct Cyc_Std_String_pa_struct
_tmp98;_tmp98.tag=Cyc_Std_String_pa;_tmp98.f1=(struct _tagged_arr)_tmp5C;{struct
Cyc_Std_String_pa_struct _tmp97;_tmp97.tag=Cyc_Std_String_pa;_tmp97.f1=(struct
_tagged_arr)t;{void*_tmp96[4]={& _tmp97,& _tmp98,& _tmp99,& _tmp9A};Cyc_Tcdecl_merr(
loc,msg,_tag_arr("%s %s : field name mismatch %s != %s",sizeof(unsigned char),37),
_tag_arr(_tmp96,sizeof(void*),4));}}}}});return 0;}if(! Cyc_Tcutil_same_atts(
_tmp7F,_tmp8C)){({struct Cyc_Std_String_pa_struct _tmp9E;_tmp9E.tag=Cyc_Std_String_pa;
_tmp9E.f1=(struct _tagged_arr)*_tmp87;{struct Cyc_Std_String_pa_struct _tmp9D;
_tmp9D.tag=Cyc_Std_String_pa;_tmp9D.f1=(struct _tagged_arr)_tmp5C;{struct Cyc_Std_String_pa_struct
_tmp9C;_tmp9C.tag=Cyc_Std_String_pa;_tmp9C.f1=(struct _tagged_arr)t;{void*_tmp9B[
3]={& _tmp9C,& _tmp9D,& _tmp9E};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s : attribute mismatch on field %s",
sizeof(unsigned char),39),_tag_arr(_tmp9B,sizeof(void*),3));}}}});_tmp5D=0;}if(!
Cyc_Tcutil_equal_tqual(_tmp85,_tmp92)){({struct Cyc_Std_String_pa_struct _tmpA2;
_tmpA2.tag=Cyc_Std_String_pa;_tmpA2.f1=(struct _tagged_arr)*_tmp87;{struct Cyc_Std_String_pa_struct
_tmpA1;_tmpA1.tag=Cyc_Std_String_pa;_tmpA1.f1=(struct _tagged_arr)_tmp5C;{struct
Cyc_Std_String_pa_struct _tmpA0;_tmpA0.tag=Cyc_Std_String_pa;_tmpA0.f1=(struct
_tagged_arr)t;{void*_tmp9F[3]={& _tmpA0,& _tmpA1,& _tmpA2};Cyc_Tcdecl_merr(loc,msg,
_tag_arr("%s %s : qualifier mismatch on field %s",sizeof(unsigned char),39),
_tag_arr(_tmp9F,sizeof(void*),3));}}}});_tmp5D=0;}if((((_tmp81 != 0? _tmp8E != 0: 0)?
Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp81))!= Cyc_Tcdecl_get_uint_const_value((
struct Cyc_Absyn_Exp*)_check_null(_tmp8E)): 0)? 1:(_tmp81 == 0? _tmp8E != 0: 0))? 1:(
_tmp81 != 0? _tmp8E == 0: 0)){({struct Cyc_Std_String_pa_struct _tmpA6;_tmpA6.tag=Cyc_Std_String_pa;
_tmpA6.f1=(struct _tagged_arr)*_tmp87;{struct Cyc_Std_String_pa_struct _tmpA5;
_tmpA5.tag=Cyc_Std_String_pa;_tmpA5.f1=(struct _tagged_arr)_tmp5C;{struct Cyc_Std_String_pa_struct
_tmpA4;_tmpA4.tag=Cyc_Std_String_pa;_tmpA4.f1=(struct _tagged_arr)t;{void*_tmpA3[
3]={& _tmpA4,& _tmpA5,& _tmpA6};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s : bitfield mismatch on field %s",
sizeof(unsigned char),38),_tag_arr(_tmpA3,sizeof(void*),3));}}}});_tmp5D=0;}{
void*subst_t1=Cyc_Tcutil_substitute(_tmp7B,_tmp90);if(! Cyc_Tcdecl_check_type(
_tmp83,subst_t1)){({struct Cyc_Std_String_pa_struct _tmpAC;_tmpAC.tag=Cyc_Std_String_pa;
_tmpAC.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(subst_t1);{struct Cyc_Std_String_pa_struct
_tmpAB;_tmpAB.tag=Cyc_Std_String_pa;_tmpAB.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
_tmp83);{struct Cyc_Std_String_pa_struct _tmpAA;_tmpAA.tag=Cyc_Std_String_pa;
_tmpAA.f1=(struct _tagged_arr)*_tmp87;{struct Cyc_Std_String_pa_struct _tmpA9;
_tmpA9.tag=Cyc_Std_String_pa;_tmpA9.f1=(struct _tagged_arr)_tmp5C;{struct Cyc_Std_String_pa_struct
_tmpA8;_tmpA8.tag=Cyc_Std_String_pa;_tmpA8.f1=(struct _tagged_arr)t;{void*_tmpA7[
5]={& _tmpA8,& _tmpA9,& _tmpAA,& _tmpAB,& _tmpAC};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s : type mismatch on field %s: %s != %s",
sizeof(unsigned char),44),_tag_arr(_tmpA7,sizeof(void*),5));}}}}}});Cyc_Tcutil_explain_failure();
_tmp5D=0;}}}}if(_tmp79 != 0){({struct Cyc_Std_String_pa_struct _tmpB0;_tmpB0.tag=
Cyc_Std_String_pa;_tmpB0.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Structfield*)
_tmp79->hd)->name;{struct Cyc_Std_String_pa_struct _tmpAF;_tmpAF.tag=Cyc_Std_String_pa;
_tmpAF.f1=(struct _tagged_arr)_tmp5C;{struct Cyc_Std_String_pa_struct _tmpAE;_tmpAE.tag=
Cyc_Std_String_pa;_tmpAE.f1=(struct _tagged_arr)t;{void*_tmpAD[3]={& _tmpAE,&
_tmpAF,& _tmpB0};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s is missing field %s",
sizeof(unsigned char),26),_tag_arr(_tmpAD,sizeof(void*),3));}}}});_tmp5D=0;}if(
_tmp74 != 0){({struct Cyc_Std_String_pa_struct _tmpB4;_tmpB4.tag=Cyc_Std_String_pa;
_tmpB4.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Structfield*)_tmp74->hd)->name;{
struct Cyc_Std_String_pa_struct _tmpB3;_tmpB3.tag=Cyc_Std_String_pa;_tmpB3.f1=(
struct _tagged_arr)_tmp5C;{struct Cyc_Std_String_pa_struct _tmpB2;_tmpB2.tag=Cyc_Std_String_pa;
_tmpB2.f1=(struct _tagged_arr)t;{void*_tmpB1[3]={& _tmpB2,& _tmpB3,& _tmpB4};Cyc_Tcdecl_merr(
loc,msg,_tag_arr("%s %s has extra field %s",sizeof(unsigned char),25),_tag_arr(
_tmpB1,sizeof(void*),3));}}}});_tmp5D=0;}d2=d0;goto _LL102;}_LL102:;}if(! _tmp5D){
return 0;}if(_tmp62 == (void*)d2->sc){return(struct Cyc_Absyn_Structdecl*)d2;}else{
d2=({struct Cyc_Absyn_Structdecl*_tmpB5=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));
_tmpB5[0]=*d2;_tmpB5;});(void*)(d2->sc=(void*)_tmp62);return(struct Cyc_Absyn_Structdecl*)
d2;}}}}struct Cyc_Absyn_Structdecl*Cyc_Tcdecl_merge_structdecl(struct Cyc_Absyn_Structdecl*
d0,struct Cyc_Absyn_Structdecl*d1,struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg){return Cyc_Tcdecl_merge_struct_or_union_decl(d0,d1,_tag_arr("struct",
sizeof(unsigned char),7),loc,msg);}struct Cyc_Absyn_Uniondecl*Cyc_Tcdecl_merge_uniondecl(
struct Cyc_Absyn_Uniondecl*d0,struct Cyc_Absyn_Uniondecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg){return(struct Cyc_Absyn_Uniondecl*)Cyc_Tcdecl_merge_struct_or_union_decl((
struct Cyc_Absyn_Structdecl*)d0,(struct Cyc_Absyn_Structdecl*)d1,_tag_arr("union",
sizeof(unsigned char),6),loc,msg);}inline static struct _tagged_arr Cyc_Tcdecl_is_x2string(
int is_x){return is_x? _tag_arr("xtunion",sizeof(unsigned char),8): _tag_arr("tunion",
sizeof(unsigned char),7);}struct _tuple7{struct Cyc_Absyn_Tqual f1;void*f2;};static
struct Cyc_Absyn_Tunionfield*Cyc_Tcdecl_merge_tunionfield(struct Cyc_Absyn_Tunionfield*
f0,struct Cyc_Absyn_Tunionfield*f1,struct Cyc_List_List*inst,struct _tagged_arr t,
struct _tagged_arr v,struct _tagged_arr*msg){struct Cyc_Position_Segment*loc=f1->loc;
if(Cyc_Std_zstrptrcmp((*f0->name).f2,(*f1->name).f2)!= 0){({struct Cyc_Std_String_pa_struct
_tmpBA;_tmpBA.tag=Cyc_Std_String_pa;_tmpBA.f1=(struct _tagged_arr)*(*f0->name).f2;{
struct Cyc_Std_String_pa_struct _tmpB9;_tmpB9.tag=Cyc_Std_String_pa;_tmpB9.f1=(
struct _tagged_arr)*(*f1->name).f2;{struct Cyc_Std_String_pa_struct _tmpB8;_tmpB8.tag=
Cyc_Std_String_pa;_tmpB8.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmpB7;_tmpB7.tag=Cyc_Std_String_pa;_tmpB7.f1=(struct _tagged_arr)t;{void*_tmpB6[
4]={& _tmpB7,& _tmpB8,& _tmpB9,& _tmpBA};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s: field name mismatch %s != %s",
sizeof(unsigned char),36),_tag_arr(_tmpB6,sizeof(void*),4));}}}}});return 0;}{
struct _tagged_arr _tmpBB=*(*f0->name).f2;int _tmpC1;void*_tmpC3;struct _tuple3
_tmpBF=Cyc_Tcdecl_merge_scope((void*)f0->sc,(void*)f1->sc,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmpBE;_tmpBE.tag=Cyc_Std_String_pa;_tmpBE.f1=(
struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct _tmpBD;_tmpBD.tag=Cyc_Std_String_pa;
_tmpBD.f1=(struct _tagged_arr)t;{void*_tmpBC[2]={& _tmpBD,& _tmpBE};Cyc_Std_aprintf(
_tag_arr("in %s %s, field",sizeof(unsigned char),16),_tag_arr(_tmpBC,sizeof(void*),
2));}}}),_tmpBB,loc,msg);_LL196: _tmpC3=_tmpBF.f1;goto _LL194;_LL194: _tmpC1=_tmpBF.f2;
goto _LL192;_LL192: {struct Cyc_List_List*_tmpC5=f0->tvs;struct Cyc_List_List*
_tmpC6=f1->tvs;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC5)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC6)){({struct Cyc_Std_String_pa_struct
_tmpCA;_tmpCA.tag=Cyc_Std_String_pa;_tmpCA.f1=(struct _tagged_arr)_tmpBB;{struct
Cyc_Std_String_pa_struct _tmpC9;_tmpC9.tag=Cyc_Std_String_pa;_tmpC9.f1=(struct
_tagged_arr)v;{struct Cyc_Std_String_pa_struct _tmpC8;_tmpC8.tag=Cyc_Std_String_pa;
_tmpC8.f1=(struct _tagged_arr)t;{void*_tmpC7[3]={& _tmpC8,& _tmpC9,& _tmpCA};Cyc_Tcdecl_merr(
loc,msg,_tag_arr("%s %s, field %s: type parameter number mismatch",sizeof(
unsigned char),48),_tag_arr(_tmpC7,sizeof(void*),3));}}}});return 0;}{struct Cyc_List_List*
_tmpCB=Cyc_Tcdecl_build_tvs_map(_tmpC5,_tmpC6);struct Cyc_List_List*_tmpCC=f0->typs;
struct Cyc_List_List*_tmpCD=f1->typs;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpCC)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpCD)){({struct Cyc_Std_String_pa_struct
_tmpD1;_tmpD1.tag=Cyc_Std_String_pa;_tmpD1.f1=(struct _tagged_arr)_tmpBB;{struct
Cyc_Std_String_pa_struct _tmpD0;_tmpD0.tag=Cyc_Std_String_pa;_tmpD0.f1=(struct
_tagged_arr)v;{struct Cyc_Std_String_pa_struct _tmpCF;_tmpCF.tag=Cyc_Std_String_pa;
_tmpCF.f1=(struct _tagged_arr)t;{void*_tmpCE[3]={& _tmpCF,& _tmpD0,& _tmpD1};Cyc_Tcdecl_merr(
loc,msg,_tag_arr("%s %s, field %s: parameter number mismatch",sizeof(
unsigned char),43),_tag_arr(_tmpCE,sizeof(void*),3));}}}});_tmpC1=0;}for(0;
_tmpCC != 0;(_tmpCC=_tmpCC->tl,_tmpCD=_tmpCD->tl)){if(! Cyc_Tcutil_equal_tqual((*((
struct _tuple7*)_tmpCC->hd)).f1,(*((struct _tuple7*)((struct Cyc_List_List*)
_check_null(_tmpCD))->hd)).f1)){({struct Cyc_Std_String_pa_struct _tmpD5;_tmpD5.tag=
Cyc_Std_String_pa;_tmpD5.f1=(struct _tagged_arr)_tmpBB;{struct Cyc_Std_String_pa_struct
_tmpD4;_tmpD4.tag=Cyc_Std_String_pa;_tmpD4.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmpD3;_tmpD3.tag=Cyc_Std_String_pa;_tmpD3.f1=(struct _tagged_arr)t;{void*_tmpD2[
3]={& _tmpD3,& _tmpD4,& _tmpD5};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s, field %s: parameter qualifier",
sizeof(unsigned char),37),_tag_arr(_tmpD2,sizeof(void*),3));}}}});_tmpC1=0;}{
void*subst_t1=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmpCB,inst),(*((struct _tuple7*)_tmpCD->hd)).f2);
if(! Cyc_Tcdecl_check_type((*((struct _tuple7*)_tmpCC->hd)).f2,subst_t1)){({struct
Cyc_Std_String_pa_struct _tmpDB;_tmpDB.tag=Cyc_Std_String_pa;_tmpDB.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(subst_t1);{struct Cyc_Std_String_pa_struct
_tmpDA;_tmpDA.tag=Cyc_Std_String_pa;_tmpDA.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((*((
struct _tuple7*)_tmpCC->hd)).f2);{struct Cyc_Std_String_pa_struct _tmpD9;_tmpD9.tag=
Cyc_Std_String_pa;_tmpD9.f1=(struct _tagged_arr)_tmpBB;{struct Cyc_Std_String_pa_struct
_tmpD8;_tmpD8.tag=Cyc_Std_String_pa;_tmpD8.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmpD7;_tmpD7.tag=Cyc_Std_String_pa;_tmpD7.f1=(struct _tagged_arr)t;{void*_tmpD6[
5]={& _tmpD7,& _tmpD8,& _tmpD9,& _tmpDA,& _tmpDB};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s, field %s: parameter type mismatch %s != %s",
sizeof(unsigned char),50),_tag_arr(_tmpD6,sizeof(void*),5));}}}}}});Cyc_Tcutil_explain_failure();
_tmpC1=0;}}}if(! _tmpC1){return 0;}if((void*)f0->sc != _tmpC3){struct Cyc_Absyn_Tunionfield*
_tmpDC=({struct Cyc_Absyn_Tunionfield*_tmpDD=_cycalloc(sizeof(struct Cyc_Absyn_Tunionfield));
_tmpDD[0]=*f0;_tmpDD;});(void*)(((struct Cyc_Absyn_Tunionfield*)_check_null(
_tmpDC))->sc=(void*)_tmpC3);return _tmpDC;}else{return(struct Cyc_Absyn_Tunionfield*)
f0;}}}}}static int Cyc_Tcdecl_substitute_tunionfield_f1(struct _tagged_arr*name,
struct Cyc_Absyn_Tvar*x){return Cyc_Std_strptrcmp(x->name,name)== 0;}static struct
_tuple7*Cyc_Tcdecl_substitute_tunionfield_f2(struct Cyc_List_List*inst,struct
_tuple7*x){struct _tuple7 _tmpE0;void*_tmpE1;struct Cyc_Absyn_Tqual _tmpE3;struct
_tuple7*_tmpDE=x;_tmpE0=*_tmpDE;_LL228: _tmpE3=_tmpE0.f1;goto _LL226;_LL226: _tmpE1=
_tmpE0.f2;goto _LL223;_LL223: return({struct _tuple7*_tmpE5=_cycalloc(sizeof(struct
_tuple7));_tmpE5->f1=_tmpE3;_tmpE5->f2=Cyc_Tcutil_substitute(inst,_tmpE1);_tmpE5;});}
static struct Cyc_Absyn_Tunionfield*Cyc_Tcdecl_substitute_tunionfield(struct Cyc_List_List*
d0tvs,struct Cyc_List_List*d1tvs,struct Cyc_List_List*inst1,struct Cyc_Absyn_Tunionfield*
f1){struct Cyc_Absyn_Tunionfield*_tmpE6=({struct Cyc_Absyn_Tunionfield*_tmpF0=
_cycalloc(sizeof(struct Cyc_Absyn_Tunionfield));_tmpF0[0]=*f1;_tmpF0;});struct Cyc_List_List*
_tmpE7=0;{struct Cyc_List_List*_tmpE8=f1->tvs;for(0;_tmpE8 != 0;_tmpE8=_tmpE8->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)_tmpE8->hd;if(((int(*)(int(*pred)(
struct _tagged_arr*,struct Cyc_Absyn_Tvar*),struct _tagged_arr*env,struct Cyc_List_List*
x))Cyc_List_exists_c)(Cyc_Tcdecl_substitute_tunionfield_f1,tv->name,d0tvs)? 1:((
int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Tvar*),struct _tagged_arr*
env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name,d1tvs)){int i=0;struct _tagged_arr base=*tv->name;struct _tagged_arr*
new_name;do {new_name=({struct _tagged_arr*_tmpE9=_cycalloc(sizeof(struct
_tagged_arr));_tmpE9[0]=(struct _tagged_arr)Cyc_Std_strconcat(base,(struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpEB;_tmpEB.tag=Cyc_Std_Int_pa;_tmpEB.f1=(
int)((unsigned int)i);{void*_tmpEA[1]={& _tmpEB};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(unsigned char),3),_tag_arr(_tmpEA,sizeof(void*),1));}}));_tmpE9;});i ++;} while (((
int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Tvar*),struct _tagged_arr*
env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcdecl_substitute_tunionfield_f1,
new_name,d0tvs)? 1:((int(*)(int(*pred)(struct _tagged_arr*,struct Cyc_Absyn_Tvar*),
struct _tagged_arr*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcdecl_substitute_tunionfield_f1,
tv->name,d1tvs));tv=({struct Cyc_Absyn_Tvar*_tmpEC=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_tmpEC[0]=*tv;_tmpEC;});tv->name=new_name;}_tmpE7=({struct Cyc_List_List*_tmpED=
_cycalloc(sizeof(struct Cyc_List_List));_tmpED->hd=tv;_tmpED->tl=_tmpE7;_tmpED;});}}
_tmpE7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpE7);
_tmpE6->tvs=_tmpE7;{struct Cyc_List_List*_tmpEE=Cyc_Tcdecl_build_tvs_map(_tmpE7,
f1->tvs);struct Cyc_List_List*_tmpEF=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmpEE,inst1);_tmpE6->typs=((struct Cyc_List_List*(*)(
struct _tuple7*(*f)(struct Cyc_List_List*,struct _tuple7*),struct Cyc_List_List*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_tunionfield_f2,
_tmpEF,f1->typs);return _tmpE6;}}static struct Cyc_List_List*Cyc_Tcdecl_merge_xtunion_fields(
struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct
Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct _tagged_arr t,
struct _tagged_arr v,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){struct
Cyc_List_List**f2sp=({struct Cyc_List_List**_tmpF9=_cycalloc(sizeof(struct Cyc_List_List*));
_tmpF9[0]=0;_tmpF9;});struct Cyc_List_List**_tmpF1=f2sp;int cmp=- 1;for(0;f0s != 0?
f1s != 0: 0;f1s=f1s->tl){while(f0s != 0?(cmp=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Tunionfield*)
f0s->hd)->name,((struct Cyc_Absyn_Tunionfield*)f1s->hd)->name))< 0: 0){struct Cyc_List_List*
_tmpF2=({struct Cyc_List_List*_tmpF3=_cycalloc(sizeof(struct Cyc_List_List));
_tmpF3->hd=(struct Cyc_Absyn_Tunionfield*)f0s->hd;_tmpF3->tl=0;_tmpF3;});*_tmpF1=
_tmpF2;_tmpF1=&((struct Cyc_List_List*)_check_null(_tmpF2))->tl;f0s=f0s->tl;}if(
f0s == 0? 1: cmp > 0){*incl=0;{struct Cyc_List_List*_tmpF4=({struct Cyc_List_List*
_tmpF5=_cycalloc(sizeof(struct Cyc_List_List));_tmpF5->hd=Cyc_Tcdecl_substitute_tunionfield(
tvs0,tvs1,inst,(struct Cyc_Absyn_Tunionfield*)f1s->hd);_tmpF5->tl=0;_tmpF5;});*
_tmpF1=_tmpF4;_tmpF1=&((struct Cyc_List_List*)_check_null(_tmpF4))->tl;}}else{
struct Cyc_Absyn_Tunionfield*_tmpF6=Cyc_Tcdecl_merge_tunionfield((struct Cyc_Absyn_Tunionfield*)
f0s->hd,(struct Cyc_Absyn_Tunionfield*)f1s->hd,inst,t,v,msg);if(_tmpF6 != 0){if(
_tmpF6 != (struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)f0s->hd)){*
incl=0;}{struct Cyc_List_List*_tmpF7=({struct Cyc_List_List*_tmpF8=_cycalloc(
sizeof(struct Cyc_List_List));_tmpF8->hd=(struct Cyc_Absyn_Tunionfield*)
_check_null(_tmpF6);_tmpF8->tl=0;_tmpF8;});*_tmpF1=_tmpF7;_tmpF1=&((struct Cyc_List_List*)
_check_null(_tmpF7))->tl;}}else{*res=0;}f0s=f0s->tl;}}if(f1s != 0){*incl=0;*
_tmpF1=f1s;}else{*_tmpF1=f0s;}return*f2sp;}struct _tuple8{struct Cyc_List_List*f1;
struct Cyc_List_List*f2;};static struct _tuple8 Cyc_Tcdecl_split(struct Cyc_List_List*
f){if(f == 0){return({struct _tuple8 _tmpFA;_tmpFA.f1=0;_tmpFA.f2=0;_tmpFA;});}if(f->tl
== 0){return({struct _tuple8 _tmpFB;_tmpFB.f1=f;_tmpFB.f2=0;_tmpFB;});}{struct Cyc_List_List*
_tmpFE;struct Cyc_List_List*_tmp100;struct _tuple8 _tmpFC=Cyc_Tcdecl_split(((struct
Cyc_List_List*)_check_null(f->tl))->tl);_LL257: _tmp100=_tmpFC.f1;goto _LL255;
_LL255: _tmpFE=_tmpFC.f2;goto _LL253;_LL253: return({struct _tuple8 _tmp102;_tmp102.f1=({
struct Cyc_List_List*_tmp104=_cycalloc(sizeof(struct Cyc_List_List));_tmp104->hd=(
void*)((void*)f->hd);_tmp104->tl=_tmp100;_tmp104;});_tmp102.f2=({struct Cyc_List_List*
_tmp103=_cycalloc(sizeof(struct Cyc_List_List));_tmp103->hd=(void*)((void*)((
struct Cyc_List_List*)_check_null(f->tl))->hd);_tmp103->tl=_tmpFE;_tmp103;});
_tmp102;});}}struct Cyc_List_List*Cyc_Tcdecl_sort_xtunion_fields(struct Cyc_List_List*
f,int*res,struct _tagged_arr*v,struct Cyc_Position_Segment*loc,struct _tagged_arr*
msg){struct Cyc_List_List*_tmp107;struct Cyc_List_List*_tmp109;struct _tuple8
_tmp105=((struct _tuple8(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);_LL266:
_tmp109=_tmp105.f1;goto _LL264;_LL264: _tmp107=_tmp105.f2;goto _LL262;_LL262: if(
_tmp109 != 0? _tmp109->tl != 0: 0){_tmp109=Cyc_Tcdecl_sort_xtunion_fields(_tmp109,
res,v,loc,msg);}if(_tmp107 != 0? _tmp107->tl != 0: 0){_tmp107=Cyc_Tcdecl_sort_xtunion_fields(
_tmp107,res,v,loc,msg);}return Cyc_Tcdecl_merge_xtunion_fields(_tmp109,_tmp107,0,
0,0,res,({int*_tmp10B=_cycalloc_atomic(sizeof(int));_tmp10B[0]=1;_tmp10B;}),
_tag_arr("xtunion",sizeof(unsigned char),8),*v,loc,msg);}struct Cyc_Absyn_Tuniondecl*
Cyc_Tcdecl_merge_tuniondecl(struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){struct _tagged_arr
_tmp10C=Cyc_Absynpp_qvar2string(d0->name);struct _tagged_arr t=_tag_arr("[x]tunion",
sizeof(unsigned char),10);int _tmp10D=1;if(d0->is_xtunion != d1->is_xtunion){({
struct Cyc_Std_String_pa_struct _tmp111;_tmp111.tag=Cyc_Std_String_pa;_tmp111.f1=(
struct _tagged_arr)Cyc_Tcdecl_is_x2string(d1->is_xtunion);{struct Cyc_Std_String_pa_struct
_tmp110;_tmp110.tag=Cyc_Std_String_pa;_tmp110.f1=(struct _tagged_arr)Cyc_Tcdecl_is_x2string(
d0->is_xtunion);{struct Cyc_Std_String_pa_struct _tmp10F;_tmp10F.tag=Cyc_Std_String_pa;
_tmp10F.f1=(struct _tagged_arr)_tmp10C;{void*_tmp10E[3]={& _tmp10F,& _tmp110,&
_tmp111};Cyc_Tcdecl_merr(loc,msg,_tag_arr("expected %s to be a %s instead of a %s",
sizeof(unsigned char),39),_tag_arr(_tmp10E,sizeof(void*),3));}}}});_tmp10D=0;}
else{t=Cyc_Tcdecl_is_x2string(d0->is_xtunion);}if(! Cyc_Tcdecl_check_tvs(d0->tvs,
d1->tvs,t,_tmp10C,loc,msg)){return 0;}{int _tmp114;void*_tmp116;struct _tuple3
_tmp112=Cyc_Tcdecl_merge_scope((void*)d0->sc,(void*)d1->sc,t,_tmp10C,loc,msg);
_LL279: _tmp116=_tmp112.f1;goto _LL277;_LL277: _tmp114=_tmp112.f2;goto _LL275;_LL275:
if(! _tmp114){_tmp10D=0;}{struct Cyc_Absyn_Tuniondecl*d2;{struct _tuple6 _tmp119=({
struct _tuple6 _tmp118;_tmp118.f1=d0->fields;_tmp118.f2=d1->fields;_tmp118;});
struct Cyc_Core_Opt*_tmp121;struct Cyc_Core_Opt*_tmp123;struct Cyc_Core_Opt*_tmp125;
struct Cyc_Core_Opt _tmp127;struct Cyc_List_List*_tmp128;struct Cyc_Core_Opt*_tmp12A;
struct Cyc_Core_Opt _tmp12C;struct Cyc_List_List*_tmp12D;_LL283: _LL290: _tmp121=
_tmp119.f2;if(_tmp121 == 0){goto _LL284;}else{goto _LL285;}_LL285: _LL292: _tmp123=
_tmp119.f1;if(_tmp123 == 0){goto _LL286;}else{goto _LL287;}_LL287: _LL299: _tmp12A=
_tmp119.f1;if(_tmp12A == 0){goto _LL282;}else{_tmp12C=*_tmp12A;_LL302: _tmp12D=(
struct Cyc_List_List*)_tmp12C.v;goto _LL294;}_LL294: _tmp125=_tmp119.f2;if(_tmp125
== 0){goto _LL282;}else{_tmp127=*_tmp125;_LL297: _tmp128=(struct Cyc_List_List*)
_tmp127.v;goto _LL288;}_LL284: d2=d0;goto _LL282;_LL286: d2=d1;goto _LL282;_LL288: {
struct Cyc_List_List*_tmp12F=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);if(d0->is_xtunion){
int _tmp130=1;struct Cyc_List_List*_tmp131=Cyc_Tcdecl_merge_xtunion_fields(_tmp12D,
_tmp128,_tmp12F,d0->tvs,d1->tvs,& _tmp10D,& _tmp130,t,_tmp10C,loc,msg);if(_tmp130){
d2=d0;}else{d2=({struct Cyc_Absyn_Tuniondecl*_tmp132=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));
_tmp132[0]=*d0;_tmp132;});(void*)(d2->sc=(void*)_tmp116);d2->fields=({struct Cyc_Core_Opt*
_tmp133=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp133->v=_tmp131;_tmp133;});}}
else{for(0;_tmp12D != 0? _tmp128 != 0: 0;(_tmp12D=_tmp12D->tl,_tmp128=_tmp128->tl)){
Cyc_Tcdecl_merge_tunionfield((struct Cyc_Absyn_Tunionfield*)_tmp12D->hd,(struct
Cyc_Absyn_Tunionfield*)_tmp128->hd,_tmp12F,t,_tmp10C,msg);}if(_tmp12D != 0){({
struct Cyc_Std_String_pa_struct _tmp137;_tmp137.tag=Cyc_Std_String_pa;_tmp137.f1=(
struct _tagged_arr)*(*((struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)
_check_null(_tmp128))->hd)->name).f2;{struct Cyc_Std_String_pa_struct _tmp136;
_tmp136.tag=Cyc_Std_String_pa;_tmp136.f1=(struct _tagged_arr)_tmp10C;{struct Cyc_Std_String_pa_struct
_tmp135;_tmp135.tag=Cyc_Std_String_pa;_tmp135.f1=(struct _tagged_arr)t;{void*
_tmp134[3]={& _tmp135,& _tmp136,& _tmp137};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s has extra field %s",
sizeof(unsigned char),25),_tag_arr(_tmp134,sizeof(void*),3));}}}});_tmp10D=0;}
if(_tmp128 != 0){({struct Cyc_Std_String_pa_struct _tmp13B;_tmp13B.tag=Cyc_Std_String_pa;
_tmp13B.f1=(struct _tagged_arr)*(*((struct Cyc_Absyn_Tunionfield*)_tmp128->hd)->name).f2;{
struct Cyc_Std_String_pa_struct _tmp13A;_tmp13A.tag=Cyc_Std_String_pa;_tmp13A.f1=(
struct _tagged_arr)_tmp10C;{struct Cyc_Std_String_pa_struct _tmp139;_tmp139.tag=Cyc_Std_String_pa;
_tmp139.f1=(struct _tagged_arr)t;{void*_tmp138[3]={& _tmp139,& _tmp13A,& _tmp13B};
Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s is missing field %s",sizeof(
unsigned char),26),_tag_arr(_tmp138,sizeof(void*),3));}}}});_tmp10D=0;}d2=d0;}
goto _LL282;}_LL282:;}if(! _tmp10D){return 0;}if(_tmp116 == (void*)d2->sc){return(
struct Cyc_Absyn_Tuniondecl*)d2;}else{d2=({struct Cyc_Absyn_Tuniondecl*_tmp13C=
_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));_tmp13C[0]=*d2;_tmp13C;});(void*)(
d2->sc=(void*)_tmp116);return(struct Cyc_Absyn_Tuniondecl*)d2;}}}}struct Cyc_Absyn_Enumdecl*
Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){struct _tagged_arr
_tmp13D=Cyc_Absynpp_qvar2string(d0->name);int _tmp13E=1;int _tmp141;void*_tmp143;
struct _tuple3 _tmp13F=Cyc_Tcdecl_merge_scope((void*)d0->sc,(void*)d1->sc,_tag_arr("enum",
sizeof(unsigned char),5),_tmp13D,loc,msg);_LL324: _tmp143=_tmp13F.f1;goto _LL322;
_LL322: _tmp141=_tmp13F.f2;goto _LL320;_LL320: if(! _tmp141){_tmp13E=0;}{struct Cyc_Absyn_Enumdecl*
d2;{struct _tuple6 _tmp146=({struct _tuple6 _tmp145;_tmp145.f1=d0->fields;_tmp145.f2=
d1->fields;_tmp145;});struct Cyc_Core_Opt*_tmp14E;struct Cyc_Core_Opt*_tmp150;
struct Cyc_Core_Opt*_tmp152;struct Cyc_Core_Opt _tmp154;struct Cyc_List_List*_tmp155;
struct Cyc_Core_Opt*_tmp157;struct Cyc_Core_Opt _tmp159;struct Cyc_List_List*_tmp15A;
_LL328: _LL335: _tmp14E=_tmp146.f2;if(_tmp14E == 0){goto _LL329;}else{goto _LL330;}
_LL330: _LL337: _tmp150=_tmp146.f1;if(_tmp150 == 0){goto _LL331;}else{goto _LL332;}
_LL332: _LL344: _tmp157=_tmp146.f1;if(_tmp157 == 0){goto _LL327;}else{_tmp159=*
_tmp157;_LL347: _tmp15A=(struct Cyc_List_List*)_tmp159.v;goto _LL339;}_LL339:
_tmp152=_tmp146.f2;if(_tmp152 == 0){goto _LL327;}else{_tmp154=*_tmp152;_LL342:
_tmp155=(struct Cyc_List_List*)_tmp154.v;goto _LL333;}_LL329: d2=d0;goto _LL327;
_LL331: d2=d1;goto _LL327;_LL333: for(0;_tmp15A != 0? _tmp155 != 0: 0;(_tmp15A=_tmp15A->tl,
_tmp155=_tmp155->tl)){struct Cyc_Absyn_Enumfield _tmp15E;struct Cyc_Position_Segment*
_tmp15F;struct Cyc_Absyn_Exp*_tmp161;struct _tuple0*_tmp163;struct _tuple0 _tmp165;
struct _tagged_arr*_tmp166;struct Cyc_Absyn_Enumfield*_tmp15C=(struct Cyc_Absyn_Enumfield*)
_tmp15A->hd;_tmp15E=*_tmp15C;_LL356: _tmp163=_tmp15E.name;_tmp165=*_tmp163;_LL359:
_tmp166=_tmp165.f2;goto _LL354;_LL354: _tmp161=_tmp15E.tag;goto _LL352;_LL352:
_tmp15F=_tmp15E.loc;goto _LL349;_LL349: {struct Cyc_Absyn_Enumfield _tmp16A;struct
Cyc_Position_Segment*_tmp16B;struct Cyc_Absyn_Exp*_tmp16D;struct _tuple0*_tmp16F;
struct _tuple0 _tmp171;struct _tagged_arr*_tmp172;struct Cyc_Absyn_Enumfield*_tmp168=(
struct Cyc_Absyn_Enumfield*)_tmp155->hd;_tmp16A=*_tmp168;_LL368: _tmp16F=_tmp16A.name;
_tmp171=*_tmp16F;_LL371: _tmp172=_tmp171.f2;goto _LL366;_LL366: _tmp16D=_tmp16A.tag;
goto _LL364;_LL364: _tmp16B=_tmp16A.loc;goto _LL361;_LL361: if(Cyc_Std_zstrptrcmp(
_tmp166,_tmp172)!= 0){({struct Cyc_Std_String_pa_struct _tmp177;_tmp177.tag=Cyc_Std_String_pa;
_tmp177.f1=(struct _tagged_arr)*_tmp172;{struct Cyc_Std_String_pa_struct _tmp176;
_tmp176.tag=Cyc_Std_String_pa;_tmp176.f1=(struct _tagged_arr)*_tmp166;{struct Cyc_Std_String_pa_struct
_tmp175;_tmp175.tag=Cyc_Std_String_pa;_tmp175.f1=(struct _tagged_arr)_tmp13D;{
void*_tmp174[3]={& _tmp175,& _tmp176,& _tmp177};Cyc_Tcdecl_merr(loc,msg,_tag_arr("enum %s: field name mismatch %s != %s",
sizeof(unsigned char),38),_tag_arr(_tmp174,sizeof(void*),3));}}}});_tmp13E=0;}
if(Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp161))!= 
Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp16D))){({
struct Cyc_Std_String_pa_struct _tmp17A;_tmp17A.tag=Cyc_Std_String_pa;_tmp17A.f1=(
struct _tagged_arr)*_tmp172;{struct Cyc_Std_String_pa_struct _tmp179;_tmp179.tag=
Cyc_Std_String_pa;_tmp179.f1=(struct _tagged_arr)_tmp13D;{void*_tmp178[2]={&
_tmp179,& _tmp17A};Cyc_Tcdecl_merr(loc,msg,_tag_arr("enum %s, field %s, value mismatch",
sizeof(unsigned char),34),_tag_arr(_tmp178,sizeof(void*),2));}}});_tmp13E=0;}}}
d2=d0;goto _LL327;_LL327:;}if(! _tmp13E){return 0;}if((void*)d2->sc == _tmp143){
return(struct Cyc_Absyn_Enumdecl*)d2;}else{d2=({struct Cyc_Absyn_Enumdecl*_tmp17B=
_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_tmp17B[0]=*d2;_tmp17B;});(void*)(d2->sc=(
void*)_tmp143);return(struct Cyc_Absyn_Enumdecl*)d2;}}}inline static struct _tuple3
Cyc_Tcdecl_check_var_or_fn_decl(void*sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct
Cyc_List_List*atts0,void*sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*
atts1,struct _tagged_arr t,struct _tagged_arr v,struct Cyc_Position_Segment*loc,
struct _tagged_arr*msg){int _tmp17C=1;int _tmp17F;void*_tmp181;struct _tuple3 _tmp17D=
Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg);_LL386: _tmp181=_tmp17D.f1;goto _LL384;
_LL384: _tmp17F=_tmp17D.f2;goto _LL382;_LL382: if(! _tmp17F){_tmp17C=0;}if(! Cyc_Tcdecl_check_type(
t0,t1)){({struct Cyc_Std_String_pa_struct _tmp187;_tmp187.tag=Cyc_Std_String_pa;
_tmp187.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t0);{struct Cyc_Std_String_pa_struct
_tmp186;_tmp186.tag=Cyc_Std_String_pa;_tmp186.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t1);{struct Cyc_Std_String_pa_struct _tmp185;_tmp185.tag=Cyc_Std_String_pa;_tmp185.f1=(
struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct _tmp184;_tmp184.tag=Cyc_Std_String_pa;
_tmp184.f1=(struct _tagged_arr)t;{void*_tmp183[4]={& _tmp184,& _tmp185,& _tmp186,&
_tmp187};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s has type \n%s\n instead of \n%s\n",
sizeof(unsigned char),36),_tag_arr(_tmp183,sizeof(void*),4));}}}}});Cyc_Tcutil_explain_failure();
_tmp17C=0;}if(! Cyc_Tcutil_equal_tqual(tq0,tq1)){({struct Cyc_Std_String_pa_struct
_tmp18A;_tmp18A.tag=Cyc_Std_String_pa;_tmp18A.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp189;_tmp189.tag=Cyc_Std_String_pa;_tmp189.f1=(struct _tagged_arr)t;{void*
_tmp188[2]={& _tmp189,& _tmp18A};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s has different type qualifiers",
sizeof(unsigned char),36),_tag_arr(_tmp188,sizeof(void*),2));}}});_tmp17C=0;}if(
! Cyc_Tcutil_same_atts(atts0,atts1)){({struct Cyc_Std_String_pa_struct _tmp18D;
_tmp18D.tag=Cyc_Std_String_pa;_tmp18D.f1=(struct _tagged_arr)v;{struct Cyc_Std_String_pa_struct
_tmp18C;_tmp18C.tag=Cyc_Std_String_pa;_tmp18C.f1=(struct _tagged_arr)t;{void*
_tmp18B[2]={& _tmp18C,& _tmp18D};Cyc_Tcdecl_merr(loc,msg,_tag_arr("%s %s has different attributes",
sizeof(unsigned char),31),_tag_arr(_tmp18B,sizeof(void*),2));}}});({void*_tmp18E[
0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("previous attributes: ",sizeof(
unsigned char),22),_tag_arr(_tmp18E,sizeof(void*),0));});for(0;atts0 != 0;atts0=
atts0->tl){({struct Cyc_Std_String_pa_struct _tmp190;_tmp190.tag=Cyc_Std_String_pa;
_tmp190.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts0->hd);{void*
_tmp18F[1]={& _tmp190};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s ",sizeof(
unsigned char),4),_tag_arr(_tmp18F,sizeof(void*),1));}});}({void*_tmp191[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\ncurrent attributes: ",sizeof(
unsigned char),22),_tag_arr(_tmp191,sizeof(void*),0));});for(0;atts1 != 0;atts1=
atts1->tl){({struct Cyc_Std_String_pa_struct _tmp193;_tmp193.tag=Cyc_Std_String_pa;
_tmp193.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts1->hd);{void*
_tmp192[1]={& _tmp193};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s ",sizeof(
unsigned char),4),_tag_arr(_tmp192,sizeof(void*),1));}});}({void*_tmp194[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n",sizeof(unsigned char),2),_tag_arr(
_tmp194,sizeof(void*),0));});_tmp17C=0;}return({struct _tuple3 _tmp195;_tmp195.f1=
_tmp181;_tmp195.f2=_tmp17C;_tmp195;});}struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg){struct _tagged_arr _tmp196=Cyc_Absynpp_qvar2string(d0->name);
int _tmp199;void*_tmp19B;struct _tuple3 _tmp197=Cyc_Tcdecl_check_var_or_fn_decl((
void*)d0->sc,(void*)d0->type,d0->tq,d0->attributes,(void*)d1->sc,(void*)d1->type,
d1->tq,d1->attributes,_tag_arr("variable",sizeof(unsigned char),9),_tmp196,loc,
msg);_LL412: _tmp19B=_tmp197.f1;goto _LL410;_LL410: _tmp199=_tmp197.f2;goto _LL408;
_LL408: if(! _tmp199){return 0;}if((void*)d0->sc == _tmp19B){return(struct Cyc_Absyn_Vardecl*)
d0;}else{struct Cyc_Absyn_Vardecl*_tmp19D=({struct Cyc_Absyn_Vardecl*_tmp19E=
_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_tmp19E[0]=*d0;_tmp19E;});(void*)(((
struct Cyc_Absyn_Vardecl*)_check_null(_tmp19D))->sc=(void*)_tmp19B);return _tmp19D;}}
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*
d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg){struct _tagged_arr _tmp19F=Cyc_Absynpp_qvar2string(d0->name);if(!
Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,_tag_arr("typedef",sizeof(unsigned char),8),
_tmp19F,loc,msg)){return 0;}{struct Cyc_List_List*_tmp1A0=Cyc_Tcdecl_build_tvs_map(
d0->tvs,d1->tvs);void*subst_defn1=Cyc_Tcutil_substitute(_tmp1A0,(void*)d1->defn);
if(! Cyc_Tcdecl_check_type((void*)d0->defn,subst_defn1)){({struct Cyc_Std_String_pa_struct
_tmp1A4;_tmp1A4.tag=Cyc_Std_String_pa;_tmp1A4.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string((
void*)d0->defn);{struct Cyc_Std_String_pa_struct _tmp1A3;_tmp1A3.tag=Cyc_Std_String_pa;
_tmp1A3.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(subst_defn1);{struct Cyc_Std_String_pa_struct
_tmp1A2;_tmp1A2.tag=Cyc_Std_String_pa;_tmp1A2.f1=(struct _tagged_arr)_tmp19F;{
void*_tmp1A1[3]={& _tmp1A2,& _tmp1A3,& _tmp1A4};Cyc_Tcdecl_merr(loc,msg,_tag_arr("typedef %s does not refer to the same type: %s != %s",
sizeof(unsigned char),53),_tag_arr(_tmp1A1,sizeof(void*),3));}}}});return 0;}
return(struct Cyc_Absyn_Typedefdecl*)d0;}}void*Cyc_Tcdecl_merge_binding(void*b0,
void*b1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg){struct _tuple4
_tmp1A6=({struct _tuple4 _tmp1A5;_tmp1A5.f1=b0;_tmp1A5.f2=b1;_tmp1A5;});void*
_tmp1B4;void*_tmp1B6;void*_tmp1B8;struct Cyc_Absyn_Vardecl*_tmp1BA;void*_tmp1BC;
struct Cyc_Absyn_Vardecl*_tmp1BE;void*_tmp1C0;struct Cyc_Absyn_Fndecl*_tmp1C2;void*
_tmp1C4;struct Cyc_Absyn_Vardecl*_tmp1C6;void*_tmp1C8;struct Cyc_Absyn_Fndecl*
_tmp1CA;void*_tmp1CC;void*_tmp1CE;struct Cyc_Absyn_Vardecl*_tmp1D0;void*_tmp1D2;
struct Cyc_Absyn_Fndecl*_tmp1D4;_LL424: _LL439: _tmp1B6=_tmp1A6.f1;if(_tmp1B6 == (
void*)Cyc_Absyn_Unresolved_b){goto _LL437;}else{goto _LL426;}_LL437: _tmp1B4=
_tmp1A6.f2;if(_tmp1B4 == (void*)Cyc_Absyn_Unresolved_b){goto _LL425;}else{goto
_LL426;}_LL426: _LL445: _tmp1BC=_tmp1A6.f1;if((unsigned int)_tmp1BC > 1?*((int*)
_tmp1BC)== Cyc_Absyn_Global_b: 0){_LL447: _tmp1BE=((struct Cyc_Absyn_Global_b_struct*)
_tmp1BC)->f1;goto _LL441;}else{goto _LL428;}_LL441: _tmp1B8=_tmp1A6.f2;if((
unsigned int)_tmp1B8 > 1?*((int*)_tmp1B8)== Cyc_Absyn_Global_b: 0){_LL443: _tmp1BA=((
struct Cyc_Absyn_Global_b_struct*)_tmp1B8)->f1;goto _LL427;}else{goto _LL428;}
_LL428: _LL453: _tmp1C4=_tmp1A6.f1;if((unsigned int)_tmp1C4 > 1?*((int*)_tmp1C4)== 
Cyc_Absyn_Global_b: 0){_LL455: _tmp1C6=((struct Cyc_Absyn_Global_b_struct*)_tmp1C4)->f1;
goto _LL449;}else{goto _LL430;}_LL449: _tmp1C0=_tmp1A6.f2;if((unsigned int)_tmp1C0 > 
1?*((int*)_tmp1C0)== Cyc_Absyn_Funname_b: 0){_LL451: _tmp1C2=((struct Cyc_Absyn_Funname_b_struct*)
_tmp1C0)->f1;goto _LL429;}else{goto _LL430;}_LL430: _LL461: _tmp1CC=_tmp1A6.f1;if((
unsigned int)_tmp1CC > 1?*((int*)_tmp1CC)== Cyc_Absyn_Funname_b: 0){goto _LL457;}
else{goto _LL432;}_LL457: _tmp1C8=_tmp1A6.f2;if((unsigned int)_tmp1C8 > 1?*((int*)
_tmp1C8)== Cyc_Absyn_Funname_b: 0){_LL459: _tmp1CA=((struct Cyc_Absyn_Funname_b_struct*)
_tmp1C8)->f1;goto _LL431;}else{goto _LL432;}_LL432: _LL467: _tmp1D2=_tmp1A6.f1;if((
unsigned int)_tmp1D2 > 1?*((int*)_tmp1D2)== Cyc_Absyn_Funname_b: 0){_LL469: _tmp1D4=((
struct Cyc_Absyn_Funname_b_struct*)_tmp1D2)->f1;goto _LL463;}else{goto _LL434;}
_LL463: _tmp1CE=_tmp1A6.f2;if((unsigned int)_tmp1CE > 1?*((int*)_tmp1CE)== Cyc_Absyn_Global_b:
0){_LL465: _tmp1D0=((struct Cyc_Absyn_Global_b_struct*)_tmp1CE)->f1;goto _LL433;}
else{goto _LL434;}_LL434: goto _LL435;_LL425: return(void*)Cyc_Absyn_Unresolved_b;
_LL427: {struct Cyc_Absyn_Vardecl*_tmp1D6=Cyc_Tcdecl_merge_vardecl(_tmp1BE,
_tmp1BA,loc,msg);if(_tmp1D6 == 0){return(void*)Cyc_Absyn_Unresolved_b;}if((struct
Cyc_Absyn_Vardecl*)_tmp1BE == _tmp1D6){return b0;}else{if((struct Cyc_Absyn_Vardecl*)
_tmp1BA == _tmp1D6){return b1;}else{return(void*)({struct Cyc_Absyn_Global_b_struct*
_tmp1D7=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_struct));_tmp1D7[0]=({struct
Cyc_Absyn_Global_b_struct _tmp1D8;_tmp1D8.tag=Cyc_Absyn_Global_b;_tmp1D8.f1=(
struct Cyc_Absyn_Vardecl*)_check_null(_tmp1D6);_tmp1D8;});_tmp1D7;});}}}_LL429: {
int _tmp1DB;struct _tuple3 _tmp1D9=Cyc_Tcdecl_check_var_or_fn_decl((void*)_tmp1C6->sc,(
void*)_tmp1C6->type,_tmp1C6->tq,_tmp1C6->attributes,(void*)_tmp1C2->sc,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp1C2->cached_typ))->v,Cyc_Absyn_empty_tqual(),
_tmp1C2->attributes,_tag_arr("function",sizeof(unsigned char),9),Cyc_Absynpp_qvar2string(
_tmp1C6->name),loc,msg);_LL476: _tmp1DB=_tmp1D9.f2;goto _LL474;_LL474: return
_tmp1DB? b1:(void*)Cyc_Absyn_Unresolved_b;}_LL431:({struct Cyc_Std_String_pa_struct
_tmp1DE;_tmp1DE.tag=Cyc_Std_String_pa;_tmp1DE.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp1CA->name);{void*_tmp1DD[1]={& _tmp1DE};Cyc_Tcdecl_merr(loc,msg,_tag_arr("redefinition of function %s",
sizeof(unsigned char),28),_tag_arr(_tmp1DD,sizeof(void*),1));}});return(void*)
Cyc_Absyn_Unresolved_b;_LL433: {int _tmp1E1;struct _tuple3 _tmp1DF=Cyc_Tcdecl_check_var_or_fn_decl((
void*)_tmp1D4->sc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1D4->cached_typ))->v,
Cyc_Absyn_empty_tqual(),_tmp1D4->attributes,(void*)_tmp1D0->sc,(void*)_tmp1D0->type,
_tmp1D0->tq,_tmp1D0->attributes,_tag_arr("variable",sizeof(unsigned char),9),Cyc_Absynpp_qvar2string(
_tmp1D4->name),loc,msg);_LL482: _tmp1E1=_tmp1DF.f2;goto _LL480;_LL480: return
_tmp1E1? b0:(void*)Cyc_Absyn_Unresolved_b;}_LL435:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp1E3=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));_tmp1E3[0]=({
struct Cyc_Core_Invalid_argument_struct _tmp1E4;_tmp1E4.tag=Cyc_Core_Invalid_argument;
_tmp1E4.f1=_tag_arr("Tcdecl::merge_binding",sizeof(unsigned char),22);_tmp1E4;});
_tmp1E3;}));_LL423:;}struct Cyc_Tcdecl_Xtunionfielddecl*Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl*d0,struct Cyc_Tcdecl_Xtunionfielddecl*d1,struct
Cyc_Position_Segment*loc,struct _tagged_arr*msg){struct Cyc_Tcdecl_Xtunionfielddecl
_tmp1E7;struct Cyc_Absyn_Tunionfield*_tmp1E8;struct Cyc_Absyn_Tuniondecl*_tmp1EA;
struct Cyc_Tcdecl_Xtunionfielddecl*_tmp1E5=d0;_tmp1E7=*_tmp1E5;_LL491: _tmp1EA=
_tmp1E7.base;goto _LL489;_LL489: _tmp1E8=_tmp1E7.field;goto _LL486;_LL486: {struct
Cyc_Tcdecl_Xtunionfielddecl _tmp1EE;struct Cyc_Absyn_Tunionfield*_tmp1EF;struct Cyc_Absyn_Tuniondecl*
_tmp1F1;struct Cyc_Tcdecl_Xtunionfielddecl*_tmp1EC=d1;_tmp1EE=*_tmp1EC;_LL498:
_tmp1F1=_tmp1EE.base;goto _LL496;_LL496: _tmp1EF=_tmp1EE.field;goto _LL493;_LL493: {
struct _tagged_arr _tmp1F3=Cyc_Absynpp_qvar2string(_tmp1E8->name);if(Cyc_Absyn_qvar_cmp(
_tmp1EA->name,_tmp1F1->name)!= 0){({struct Cyc_Std_String_pa_struct _tmp1F7;
_tmp1F7.tag=Cyc_Std_String_pa;_tmp1F7.f1=(struct _tagged_arr)_tmp1F3;{struct Cyc_Std_String_pa_struct
_tmp1F6;_tmp1F6.tag=Cyc_Std_String_pa;_tmp1F6.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp1F1->name);{struct Cyc_Std_String_pa_struct _tmp1F5;_tmp1F5.tag=Cyc_Std_String_pa;
_tmp1F5.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp1EA->name);{void*
_tmp1F4[3]={& _tmp1F5,& _tmp1F6,& _tmp1F7};Cyc_Tcdecl_merr(loc,msg,_tag_arr("xtunions %s and %s have both a field named %s",
sizeof(unsigned char),46),_tag_arr(_tmp1F4,sizeof(void*),3));}}}});return 0;}if(!
Cyc_Tcdecl_check_tvs(_tmp1EA->tvs,_tmp1F1->tvs,_tag_arr("xtunion",sizeof(
unsigned char),8),Cyc_Absynpp_qvar2string(_tmp1EA->name),loc,msg)){return 0;}{
struct Cyc_List_List*_tmp1F8=Cyc_Tcdecl_build_tvs_map(_tmp1EA->tvs,_tmp1F1->tvs);
struct Cyc_Absyn_Tunionfield*_tmp1F9=Cyc_Tcdecl_merge_tunionfield(_tmp1E8,_tmp1EF,
_tmp1F8,_tag_arr("xtunionfield",sizeof(unsigned char),13),_tmp1F3,msg);if(
_tmp1F9 == 0){return 0;}if(_tmp1F9 == (struct Cyc_Absyn_Tunionfield*)_tmp1E8){return(
struct Cyc_Tcdecl_Xtunionfielddecl*)d0;}else{return({struct Cyc_Tcdecl_Xtunionfielddecl*
_tmp1FA=_cycalloc(sizeof(struct Cyc_Tcdecl_Xtunionfielddecl));_tmp1FA->base=
_tmp1EA;_tmp1FA->field=(struct Cyc_Absyn_Tunionfield*)_check_null(_tmp1F9);
_tmp1FA;});}}}}}
