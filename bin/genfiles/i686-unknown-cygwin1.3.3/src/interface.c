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
void*Cyc_Core_identity(void*);extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
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
unsigned char Cyc_List_List_mismatch[18];extern struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);extern unsigned char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{
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
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);extern int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern void*Cyc_Absyn_uchar_t;extern void*Cyc_Absyn_uint_t;extern void*Cyc_Absyn_sint_t;
extern struct _tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;
extern void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,
struct Cyc_Absyn_Exp*init);extern void*Cyc_Absyn_function_typ(struct Cyc_List_List*
tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,
struct Cyc_List_List*);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];
struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern unsigned char
Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));extern int Cyc_Dict_member(struct Cyc_Dict_Dict*d,void*k);extern struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);extern void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict*d,void*k);extern void*Cyc_Dict_fold(void*(*f)(void*,void*,
void*),struct Cyc_Dict_Dict*d,void*accum);extern void Cyc_Dict_iter_c(void(*f)(void*,
void*,void*),void*env,struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_map(
void*(*f)(void*),struct Cyc_Dict_Dict*d);static const int Cyc_Tcenv_VarRes=0;struct
Cyc_Tcenv_VarRes_struct{int tag;void*f1;};static const int Cyc_Tcenv_StructRes=1;
struct Cyc_Tcenv_StructRes_struct{int tag;struct Cyc_Absyn_Structdecl*f1;};static
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
ae;struct Cyc_Core_Opt*le;};extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();struct
Cyc_Interface_I;extern struct Cyc_Interface_I*Cyc_Interface_empty();extern struct
Cyc_Interface_I*Cyc_Interface_final();extern struct Cyc_Interface_I*Cyc_Interface_extract(
struct Cyc_Dict_Dict*ae);struct _tuple3{struct _tagged_arr f1;struct _tagged_arr f2;};
extern int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*
i2,struct _tuple3*info);extern struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*
i1,struct Cyc_Interface_I*i2,struct _tuple3*info);extern struct Cyc_Interface_I*Cyc_Interface_merge_list(
struct Cyc_List_List*li,struct Cyc_List_List*linfo);extern struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo);extern void Cyc_Interface_print(struct Cyc_Interface_I*,
struct Cyc_Std___sFILE*);extern struct Cyc_Interface_I*Cyc_Interface_parse(struct
Cyc_Std___sFILE*);extern void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc_Std___sFILE*);
extern struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc_Std___sFILE*);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc_Std___sFILE*f);extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*);extern void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,
struct Cyc_List_List*ds);extern unsigned char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*
refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};extern struct
Cyc_List_List*Cyc_Parse_parse_file(struct Cyc_Std___sFILE*f);struct Cyc_Declaration_spec;
struct Cyc_Declarator;struct Cyc_Abstractdeclarator;extern int Cyc_yyparse();extern
unsigned char Cyc_AbstractDeclarator_tok[27];struct Cyc_AbstractDeclarator_tok_struct{
unsigned char*tag;struct Cyc_Abstractdeclarator*f1;};extern unsigned char Cyc_AttributeList_tok[
22];struct Cyc_AttributeList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;
};extern unsigned char Cyc_Attribute_tok[18];struct Cyc_Attribute_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Bool_tok[13];struct Cyc_Bool_tok_struct{
unsigned char*tag;int f1;};extern unsigned char Cyc_Char_tok[13];struct Cyc_Char_tok_struct{
unsigned char*tag;unsigned char f1;};extern unsigned char Cyc_DeclList_tok[17];
struct Cyc_DeclList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_DeclSpec_tok[17];struct Cyc_DeclSpec_tok_struct{unsigned char*
tag;struct Cyc_Declaration_spec*f1;};extern unsigned char Cyc_Declarator_tok[19];
struct Cyc_Declarator_tok_struct{unsigned char*tag;struct Cyc_Declarator*f1;};
extern unsigned char Cyc_DesignatorList_tok[23];struct Cyc_DesignatorList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Designator_tok[
19];struct Cyc_Designator_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_EnumfieldList_tok[22];struct Cyc_EnumfieldList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Enumfield_tok[
18];struct Cyc_Enumfield_tok_struct{unsigned char*tag;struct Cyc_Absyn_Enumfield*
f1;};extern unsigned char Cyc_ExpList_tok[16];struct Cyc_ExpList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Exp_tok[12];
struct Cyc_Exp_tok_struct{unsigned char*tag;struct Cyc_Absyn_Exp*f1;};extern
unsigned char Cyc_FieldPatternList_tok[25];struct Cyc_FieldPatternList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_FieldPattern_tok[
21];struct _tuple4{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
unsigned char*tag;struct _tuple4*f1;};extern unsigned char Cyc_FnDecl_tok[15];
struct Cyc_FnDecl_tok_struct{unsigned char*tag;struct Cyc_Absyn_Fndecl*f1;};extern
unsigned char Cyc_IdList_tok[15];struct Cyc_IdList_tok_struct{unsigned char*tag;
struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDeclList_tok[21];struct Cyc_InitDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDecl_tok[
17];struct _tuple5{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
unsigned char*tag;struct _tuple5*f1;};extern unsigned char Cyc_InitializerList_tok[
24];struct Cyc_InitializerList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_Int_tok[12];struct _tuple6{void*f1;int f2;};struct Cyc_Int_tok_struct{
unsigned char*tag;struct _tuple6*f1;};extern unsigned char Cyc_Kind_tok[13];struct
Cyc_Kind_tok_struct{unsigned char*tag;void*f1;};extern unsigned char Cyc_Okay_tok[
13];extern unsigned char Cyc_ParamDeclListBool_tok[26];struct _tuple7{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_ParamDeclListBool_tok_struct{unsigned char*tag;struct _tuple7*f1;}
;extern unsigned char Cyc_ParamDeclList_tok[22];struct Cyc_ParamDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_ParamDecl_tok[
18];struct Cyc_ParamDecl_tok_struct{unsigned char*tag;struct _tuple1*f1;};extern
unsigned char Cyc_PatternList_tok[20];struct Cyc_PatternList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Pattern_tok[16];
struct Cyc_Pattern_tok_struct{unsigned char*tag;struct Cyc_Absyn_Pat*f1;};extern
unsigned char Cyc_Pointer_Sort_tok[21];struct Cyc_Pointer_Sort_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Primop_tok[15];struct Cyc_Primop_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Primopopt_tok[18];struct Cyc_Primopopt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_QualId_tok[15];
struct Cyc_QualId_tok_struct{unsigned char*tag;struct _tuple0*f1;};extern
unsigned char Cyc_QualSpecList_tok[21];struct _tuple8{struct Cyc_Absyn_Tqual f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_QualSpecList_tok_struct{
unsigned char*tag;struct _tuple8*f1;};extern unsigned char Cyc_Rgnorder_tok[17];
struct Cyc_Rgnorder_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_Scope_tok[14];struct Cyc_Scope_tok_struct{unsigned char*tag;void*
f1;};extern unsigned char Cyc_Short_tok[14];struct Cyc_Short_tok_struct{
unsigned char*tag;short f1;};extern unsigned char Cyc_Stmt_tok[13];struct Cyc_Stmt_tok_struct{
unsigned char*tag;struct Cyc_Absyn_Stmt*f1;};extern unsigned char Cyc_StorageClass_tok[
21];struct Cyc_StorageClass_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_String_tok[15];struct Cyc_String_tok_struct{unsigned char*tag;
struct _tagged_arr f1;};extern unsigned char Cyc_Stringopt_tok[18];struct Cyc_Stringopt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_StructFieldDeclListList_tok[
32];struct Cyc_StructFieldDeclListList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_StructFieldDeclList_tok[28];struct Cyc_StructFieldDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_StructOrUnion_tok[
22];struct Cyc_StructOrUnion_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_SwitchCClauseList_tok[26];struct Cyc_SwitchCClauseList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_SwitchClauseList_tok[
25];struct Cyc_SwitchClauseList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_TunionFieldList_tok[24];struct Cyc_TunionFieldList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_TunionField_tok[
20];struct Cyc_TunionField_tok_struct{unsigned char*tag;struct Cyc_Absyn_Tunionfield*
f1;};extern unsigned char Cyc_TypeList_tok[17];struct Cyc_TypeList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_TypeModifierList_tok[
25];struct Cyc_TypeModifierList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_TypeOpt_tok[16];struct Cyc_TypeOpt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_TypeQual_tok[17];
struct Cyc_TypeQual_tok_struct{unsigned char*tag;struct Cyc_Absyn_Tqual f1;};extern
unsigned char Cyc_TypeSpecifier_tok[22];struct Cyc_TypeSpecifier_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Type_tok[13];struct Cyc_Type_tok_struct{
unsigned char*tag;void*f1;};extern int Cyc_yyparse();extern unsigned char Cyc_YY1[8];
struct Cyc_YY1_struct{unsigned char*tag;struct _tuple7*f1;};extern unsigned char Cyc_YYINITIALSVAL[
18];struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;
int last_column;};extern int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct
_tagged_arr*s2);extern int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);
extern unsigned char Cyc_Tcutil_TypeErr[12];extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(void*t);
extern int Cyc_Tcutil_is_function_type(void*t);extern unsigned char Cyc_Tcdecl_Incompatible[
17];struct Cyc_Tcdecl_Xtunionfielddecl{struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*
field;};extern void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg1,struct _tagged_arr fmt,struct _tagged_arr ap);extern struct Cyc_Absyn_Structdecl*
Cyc_Tcdecl_merge_structdecl(struct Cyc_Absyn_Structdecl*d0,struct Cyc_Absyn_Structdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Uniondecl*
Cyc_Tcdecl_merge_uniondecl(struct Cyc_Absyn_Uniondecl*d0,struct Cyc_Absyn_Uniondecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Tuniondecl*
Cyc_Tcdecl_merge_tuniondecl(struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Enumdecl*
Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Typedefdecl*
Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_Tcdecl_Xtunionfielddecl*
Cyc_Tcdecl_merge_xtunionfielddecl(struct Cyc_Tcdecl_Xtunionfielddecl*d0,struct Cyc_Tcdecl_Xtunionfielddecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern void Cyc_Lex_lex_init();
struct Cyc_Interface_Ienv{struct Cyc_Dict_Dict*structdecls;struct Cyc_Dict_Dict*
uniondecls;struct Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct
Cyc_Dict_Dict*typedefdecls;struct Cyc_Dict_Dict*vardecls;struct Cyc_Dict_Dict*
xtunionfielddecls;};struct Cyc_Interface_I{struct Cyc_Dict_Dict*imports;struct Cyc_Dict_Dict*
exports;};static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv(){return({struct
Cyc_Interface_Ienv*_tmp0=_cycalloc(sizeof(struct Cyc_Interface_Ienv));_tmp0->structdecls=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp0->uniondecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp0->tuniondecls=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp0->enumdecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp0->typedefdecls=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp0->vardecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp0->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp0;});}static struct Cyc_Interface_Ienv*Cyc_Interface_lazy_new_ienv(){
static struct Cyc_Interface_Ienv*ie=0;if(ie == 0){ie=(struct Cyc_Interface_Ienv*)Cyc_Interface_new_ienv();}
return(struct Cyc_Interface_Ienv*)_check_null(ie);}static struct Cyc_Interface_Ienv*
Cyc_Interface_skel2i_f(void*ignored){return Cyc_Interface_new_ienv();}static
struct Cyc_Interface_I*Cyc_Interface_skel2i(struct Cyc_Dict_Dict*skel){return({
struct Cyc_Interface_I*_tmp1=_cycalloc(sizeof(struct Cyc_Interface_I));_tmp1->imports=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Interface_Ienv*(*f)(void*),struct Cyc_Dict_Dict*
d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel);_tmp1->exports=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(*f)(void*),struct Cyc_Dict_Dict*d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,
skel);_tmp1;});}struct Cyc_Interface_I*Cyc_Interface_empty(){return({struct Cyc_Interface_I*
_tmp2=_cycalloc(sizeof(struct Cyc_Interface_I));_tmp2->imports=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);
_tmp2->exports=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct Cyc_List_List*,struct
Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);_tmp2;});}struct Cyc_Interface_I*
Cyc_Interface_final(){struct Cyc_Interface_I*_tmp3=Cyc_Interface_empty();struct
Cyc_Interface_Ienv*_tmp4=Cyc_Interface_new_ienv();struct Cyc_Absyn_Tuniondecl*
_tmp5=({struct Cyc_Absyn_Tuniondecl*_tmp14=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));
_tmp14[0]=*Cyc_Absyn_exn_tud;_tmp14;});(void*)(_tmp5->sc=(void*)((void*)Cyc_Absyn_Public));
_tmp4->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct Cyc_Absyn_Tuniondecl*v))Cyc_Dict_insert)(_tmp4->tuniondecls,(*
Cyc_Absyn_exn_name).f2,_tmp5);{struct Cyc_List_List*_tmp6=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp6 != 0;
_tmp6=_tmp6->tl){struct Cyc_Absyn_Tunionfield*_tmp7=({struct Cyc_Absyn_Tunionfield*
_tmpA=_cycalloc(sizeof(struct Cyc_Absyn_Tunionfield));_tmpA[0]=*((struct Cyc_Absyn_Tunionfield*)
_tmp6->hd);_tmpA;});(void*)(_tmp7->sc=(void*)((void*)Cyc_Absyn_Public));{struct
Cyc_Tcdecl_Xtunionfielddecl*_tmp8=({struct Cyc_Tcdecl_Xtunionfielddecl*_tmp9=
_cycalloc(sizeof(struct Cyc_Tcdecl_Xtunionfielddecl));_tmp9->base=_tmp5;_tmp9->field=
_tmp7;_tmp9;});_tmp4->xtunionfielddecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Tcdecl_Xtunionfielddecl*v))Cyc_Dict_insert)(
_tmp4->xtunionfielddecls,(*((struct Cyc_Absyn_Tunionfield*)_tmp6->hd)->name).f2,
_tmp8);}}}_tmp3->exports=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(_tmp3->exports,0,
_tmp4);{struct Cyc_Interface_Ienv*_tmpB=Cyc_Interface_new_ienv();struct _tuple0*
qmain=({struct _tuple0*_tmp11=_cycalloc(sizeof(struct _tuple0));_tmp11->f1=(void*)({
struct Cyc_Absyn_Abs_n_struct*_tmp12=_cycalloc(sizeof(struct Cyc_Absyn_Abs_n_struct));
_tmp12[0]=({struct Cyc_Absyn_Abs_n_struct _tmp13;_tmp13.tag=Cyc_Absyn_Abs_n;_tmp13.f1=
0;_tmp13;});_tmp12;});_tmp11->f2=_init_tag_arr(_cycalloc(sizeof(struct
_tagged_arr)),"main",sizeof(unsigned char),5);_tmp11;});struct Cyc_Absyn_Vardecl*
_tmpC=Cyc_Absyn_new_vardecl(qmain,Cyc_Absyn_function_typ(0,0,Cyc_Absyn_sint_t,({
struct Cyc_List_List*_tmpD=_cycalloc(sizeof(struct Cyc_List_List));_tmpD->hd=({
struct _tuple1*_tmp10=_cycalloc(sizeof(struct _tuple1));_tmp10->f1=0;_tmp10->f2=
Cyc_Absyn_empty_tqual();_tmp10->f3=Cyc_Absyn_uint_t;_tmp10;});_tmpD->tl=({struct
Cyc_List_List*_tmpE=_cycalloc(sizeof(struct Cyc_List_List));_tmpE->hd=({struct
_tuple1*_tmpF=_cycalloc(sizeof(struct _tuple1));_tmpF->f1=0;_tmpF->f2=Cyc_Absyn_empty_tqual();
_tmpF->f3=Cyc_Absyn_tagged_typ(Cyc_Absyn_tagged_typ(Cyc_Absyn_uchar_t,(void*)Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()),(void*)Cyc_Absyn_HeapRgn,Cyc_Absyn_empty_tqual());_tmpF;});
_tmpE->tl=0;_tmpE;});_tmpD;}),0,0,0,0),0);(void*)(_tmpC->sc=(void*)((void*)Cyc_Absyn_Extern));
_tmpB->vardecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpB->vardecls,(*
qmain).f2,_tmpC);_tmp3->imports=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(_tmp3->exports,
0,_tmpB);return _tmp3;}}static void Cyc_Interface_err(struct _tagged_arr msg){({void*
_tmp15[0]={};Cyc_Tcutil_terr(0,msg,_tag_arr(_tmp15,sizeof(void*),0));});}static
void Cyc_Interface_fields_err(struct _tagged_arr sc,struct _tagged_arr t,struct
_tuple0*n){Cyc_Interface_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp19;_tmp19.tag=Cyc_Std_String_pa;_tmp19.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
n);{struct Cyc_Std_String_pa_struct _tmp18;_tmp18.tag=Cyc_Std_String_pa;_tmp18.f1=(
struct _tagged_arr)t;{struct Cyc_Std_String_pa_struct _tmp17;_tmp17.tag=Cyc_Std_String_pa;
_tmp17.f1=(struct _tagged_arr)sc;{void*_tmp16[3]={& _tmp17,& _tmp18,& _tmp19};Cyc_Std_aprintf(
_tag_arr("fields of %s %s %s have never been defined",sizeof(unsigned char),43),
_tag_arr(_tmp16,sizeof(void*),3));}}}}));}static void Cyc_Interface_body_err(
struct _tagged_arr sc,struct _tuple0*n){Cyc_Interface_err((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp1C;_tmp1C.tag=Cyc_Std_String_pa;_tmp1C.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(n);{struct Cyc_Std_String_pa_struct
_tmp1B;_tmp1B.tag=Cyc_Std_String_pa;_tmp1B.f1=(struct _tagged_arr)sc;{void*_tmp1A[
2]={& _tmp1B,& _tmp1C};Cyc_Std_aprintf(_tag_arr("the body of %s function %s has never been defined",
sizeof(unsigned char),50),_tag_arr(_tmp1A,sizeof(void*),2));}}}));}static void Cyc_Interface_static_err(
struct _tagged_arr obj1,struct _tuple0*name1,struct _tagged_arr obj2,struct _tuple0*
name2){if(obj1.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){Cyc_Interface_err((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp21;_tmp21.tag=Cyc_Std_String_pa;
_tmp21.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(name2);{struct Cyc_Std_String_pa_struct
_tmp20;_tmp20.tag=Cyc_Std_String_pa;_tmp20.f1=(struct _tagged_arr)obj2;{struct Cyc_Std_String_pa_struct
_tmp1F;_tmp1F.tag=Cyc_Std_String_pa;_tmp1F.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
name1);{struct Cyc_Std_String_pa_struct _tmp1E;_tmp1E.tag=Cyc_Std_String_pa;_tmp1E.f1=(
struct _tagged_arr)obj1;{void*_tmp1D[4]={& _tmp1E,& _tmp1F,& _tmp20,& _tmp21};Cyc_Std_aprintf(
_tag_arr("declaration of %s %s relies on static %s %s",sizeof(unsigned char),44),
_tag_arr(_tmp1D,sizeof(void*),4));}}}}}));}}static void Cyc_Interface_abstract_err(
struct _tagged_arr obj1,struct _tuple0*name1,struct _tagged_arr obj2,struct _tuple0*
name2){if(obj1.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){Cyc_Interface_err((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp26;_tmp26.tag=Cyc_Std_String_pa;
_tmp26.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(name2);{struct Cyc_Std_String_pa_struct
_tmp25;_tmp25.tag=Cyc_Std_String_pa;_tmp25.f1=(struct _tagged_arr)obj2;{struct Cyc_Std_String_pa_struct
_tmp24;_tmp24.tag=Cyc_Std_String_pa;_tmp24.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
name1);{struct Cyc_Std_String_pa_struct _tmp23;_tmp23.tag=Cyc_Std_String_pa;_tmp23.f1=(
struct _tagged_arr)obj1;{void*_tmp22[4]={& _tmp23,& _tmp24,& _tmp25,& _tmp26};Cyc_Std_aprintf(
_tag_arr("declaration of %s %s relies on fields of abstract %s %s",sizeof(
unsigned char),56),_tag_arr(_tmp22,sizeof(void*),4));}}}}}));}}struct Cyc_Interface_Seen{
struct Cyc_Dict_Dict*structs;struct Cyc_Dict_Dict*unions;struct Cyc_Dict_Dict*
tunions;};static struct Cyc_Interface_Seen*Cyc_Interface_new_seen(){return({struct
Cyc_Interface_Seen*_tmp27=_cycalloc(sizeof(struct Cyc_Interface_Seen));_tmp27->structs=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp27->unions=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp27->tunions=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(
Cyc_Absyn_qvar_cmp);_tmp27;});}static int Cyc_Interface_check_public_type(struct
Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct
_tuple0*name,void*t);static int Cyc_Interface_check_public_type_list(struct Cyc_Dict_Dict*
ae,struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct _tuple0*name,void*(*
f)(void*),struct Cyc_List_List*l){int _tmp28=1;for(0;l != 0;l=l->tl){if(! Cyc_Interface_check_public_type(
ae,seen,obj,name,f((void*)l->hd))){_tmp28=0;}}return _tmp28;}static int Cyc_Interface_check_public_structdecl(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Structdecl*
d){struct _handler_cons _tmp29;_push_handler(& _tmp29);{int _tmp2B=0;if(setjmp(
_tmp29.handler)){_tmp2B=1;}if(! _tmp2B){{int _tmp2C=((int(*)(struct Cyc_Dict_Dict*d,
struct _tuple0*k))Cyc_Dict_lookup)(seen->structs,(struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(d->name))->v);_npop_handler(0);return _tmp2C;};_pop_handler();}else{
void*_tmp2A=(void*)_exn_thrown;void*_tmp2E=_tmp2A;_LL48: if(_tmp2E == Cyc_Dict_Absent){
goto _LL49;}else{goto _LL50;}_LL50: goto _LL51;_LL49: {int _tmp34=1;seen->structs=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)(
seen->structs,(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(d->name))->v,
_tmp34);if(d->fields != 0){struct Cyc_List_List*_tmp35=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(d->fields))->v;for(0;_tmp35 != 0;_tmp35=_tmp35->tl){
if(! Cyc_Interface_check_public_type(ae,seen,_tag_arr("struct",sizeof(
unsigned char),7),(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(d->name))->v,(
void*)((struct Cyc_Absyn_Structfield*)_tmp35->hd)->type)){_tmp34=0;}}}seen->structs=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)(
seen->structs,(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(d->name))->v,
_tmp34);return _tmp34;}_LL51:(void)_throw(_tmp2E);_LL47:;}}}static int Cyc_Interface_check_public_uniondecl(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Uniondecl*d){
struct _handler_cons _tmp36;_push_handler(& _tmp36);{int _tmp38=0;if(setjmp(_tmp36.handler)){
_tmp38=1;}if(! _tmp38){{int _tmp39=((int(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))
Cyc_Dict_lookup)(seen->unions,(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(
d->name))->v);_npop_handler(0);return _tmp39;};_pop_handler();}else{void*_tmp37=(
void*)_exn_thrown;void*_tmp3B=_tmp37;_LL61: if(_tmp3B == Cyc_Dict_Absent){goto
_LL62;}else{goto _LL63;}_LL63: goto _LL64;_LL62: {int _tmp41=1;seen->unions=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)(
seen->unions,(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(d->name))->v,
_tmp41);if(d->fields != 0){struct Cyc_List_List*_tmp42=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(d->fields))->v;for(0;_tmp42 != 0;_tmp42=_tmp42->tl){
if(! Cyc_Interface_check_public_type(ae,seen,_tag_arr("union",sizeof(
unsigned char),6),(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(d->name))->v,(
void*)((struct Cyc_Absyn_Structfield*)_tmp42->hd)->type)){_tmp41=0;}}}seen->unions=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)(
seen->unions,(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(d->name))->v,
_tmp41);return _tmp41;}_LL64:(void)_throw(_tmp3B);_LL60:;}}}struct _tuple9{struct
Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_Interface_get_type1(struct _tuple9*x){
return(*x).f2;}static int Cyc_Interface_check_public_tuniondecl(struct Cyc_Dict_Dict*
ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Tuniondecl*d){struct
_handler_cons _tmp43;_push_handler(& _tmp43);{int _tmp45=0;if(setjmp(_tmp43.handler)){
_tmp45=1;}if(! _tmp45){{int _tmp46=((int(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))
Cyc_Dict_lookup)(seen->tunions,d->name);_npop_handler(0);return _tmp46;};
_pop_handler();}else{void*_tmp44=(void*)_exn_thrown;void*_tmp48=_tmp44;_LL74: if(
_tmp48 == Cyc_Dict_Absent){goto _LL75;}else{goto _LL76;}_LL76: goto _LL77;_LL75: {int
_tmp4E=1;seen->tunions=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tuple0*k,int v))Cyc_Dict_insert)(seen->tunions,d->name,_tmp4E);if(d->fields != 0){
struct Cyc_List_List*_tmp4F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(d->fields))->v;for(0;_tmp4F != 0;_tmp4F=_tmp4F->tl){if(!((int(*)(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct
_tuple0*name,void*(*f)(struct _tuple9*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(
ae,seen,_tag_arr("[x]tunion",sizeof(unsigned char),10),d->name,Cyc_Interface_get_type1,((
struct Cyc_Absyn_Tunionfield*)_tmp4F->hd)->typs)){_tmp4E=0;}}}seen->tunions=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)(
seen->tunions,d->name,_tmp4E);return _tmp4E;}_LL77:(void)_throw(_tmp48);_LL73:;}}}
static int Cyc_Interface_check_public_enumdecl(struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*
seen,struct Cyc_Absyn_Enumdecl*d){return 1;}static int Cyc_Interface_check_public_typedefdecl(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*
d){return Cyc_Interface_check_public_type(ae,seen,(struct _tagged_arr)_tag_arr(0,0,
0),d->name,(void*)d->defn);}static int Cyc_Interface_check_public_vardecl(struct
Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d){return
Cyc_Interface_check_public_type(ae,seen,_tag_arr("variable",sizeof(unsigned char),
9),d->name,(void*)d->type);}static void*Cyc_Interface_get_type2(struct _tuple1*x){
return(*x).f3;}static struct Cyc_List_List*Cyc_Interface_get_abs_ns(void*ns){void*
_tmp50=ns;struct Cyc_List_List*_tmp56;_LL82: if((unsigned int)_tmp50 > 1?*((int*)
_tmp50)== Cyc_Absyn_Abs_n: 0){_LL87: _tmp56=((struct Cyc_Absyn_Abs_n_struct*)_tmp50)->f1;
goto _LL83;}else{goto _LL84;}_LL84: goto _LL85;_LL83: return _tmp56;_LL85:(int)_throw((
void*)({struct Cyc_Core_Invalid_argument_struct*_tmp58=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));
_tmp58[0]=({struct Cyc_Core_Invalid_argument_struct _tmp59;_tmp59.tag=Cyc_Core_Invalid_argument;
_tmp59.f1=_tag_arr("get_abs_ns",sizeof(unsigned char),11);_tmp59;});_tmp58;}));
_LL81:;}static int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict*ae,struct
Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct _tuple0*name,void*t){void*
_tmp5A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp72;void*_tmp74;void*
_tmp76;struct Cyc_Core_Opt*_tmp78;struct Cyc_Core_Opt _tmp7A;void*_tmp7B;struct Cyc_Absyn_FnInfo
_tmp7D;struct Cyc_Absyn_VarargInfo*_tmp7F;struct Cyc_List_List*_tmp81;void*_tmp83;
struct Cyc_List_List*_tmp85;struct Cyc_Absyn_Structdecl**_tmp87;struct Cyc_Absyn_Structdecl*
_tmp89;struct Cyc_List_List*_tmp8A;struct Cyc_Absyn_Uniondecl**_tmp8C;struct Cyc_Absyn_Uniondecl*
_tmp8E;struct Cyc_List_List*_tmp8F;struct _tuple0*_tmp91;struct Cyc_Absyn_TunionInfo
_tmp93;struct Cyc_List_List*_tmp95;void*_tmp97;struct Cyc_Absyn_Tuniondecl**_tmp99;
struct Cyc_Absyn_Tuniondecl*_tmp9B;struct Cyc_Absyn_TunionFieldInfo _tmp9C;struct
Cyc_List_List*_tmp9E;void*_tmpA0;struct Cyc_Absyn_Tunionfield*_tmpA2;struct Cyc_Absyn_Tuniondecl*
_tmpA4;_LL92: if((unsigned int)_tmp5A > 3?*((int*)_tmp5A)== Cyc_Absyn_PointerType:
0){_LL115: _tmp72=((struct Cyc_Absyn_PointerType_struct*)_tmp5A)->f1;_LL117: _tmp74=(
void*)_tmp72.elt_typ;goto _LL93;}else{goto _LL94;}_LL94: if((unsigned int)_tmp5A > 3?*((
int*)_tmp5A)== Cyc_Absyn_ArrayType: 0){_LL119: _tmp76=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp5A)->f1;goto _LL95;}else{goto _LL96;}_LL96: if((unsigned int)_tmp5A > 3?*((int*)
_tmp5A)== Cyc_Absyn_TypedefType: 0){_LL121: _tmp78=((struct Cyc_Absyn_TypedefType_struct*)
_tmp5A)->f3;if(_tmp78 == 0){goto _LL98;}else{_tmp7A=*_tmp78;_LL124: _tmp7B=(void*)
_tmp7A.v;goto _LL97;}}else{goto _LL98;}_LL98: if((unsigned int)_tmp5A > 3?*((int*)
_tmp5A)== Cyc_Absyn_FnType: 0){_LL126: _tmp7D=((struct Cyc_Absyn_FnType_struct*)
_tmp5A)->f1;_LL132: _tmp83=(void*)_tmp7D.ret_typ;goto _LL130;_LL130: _tmp81=_tmp7D.args;
goto _LL128;_LL128: _tmp7F=_tmp7D.cyc_varargs;goto _LL99;}else{goto _LL100;}_LL100:
if((unsigned int)_tmp5A > 3?*((int*)_tmp5A)== Cyc_Absyn_TupleType: 0){_LL134:
_tmp85=((struct Cyc_Absyn_TupleType_struct*)_tmp5A)->f1;goto _LL101;}else{goto
_LL102;}_LL102: if((unsigned int)_tmp5A > 3?*((int*)_tmp5A)== Cyc_Absyn_StructType:
0){_LL139: _tmp8A=((struct Cyc_Absyn_StructType_struct*)_tmp5A)->f2;goto _LL136;
_LL136: _tmp87=((struct Cyc_Absyn_StructType_struct*)_tmp5A)->f3;if(_tmp87 == 0){
goto _LL104;}else{_tmp89=*_tmp87;goto _LL103;}}else{goto _LL104;}_LL104: if((
unsigned int)_tmp5A > 3?*((int*)_tmp5A)== Cyc_Absyn_UnionType: 0){_LL144: _tmp8F=((
struct Cyc_Absyn_UnionType_struct*)_tmp5A)->f2;goto _LL141;_LL141: _tmp8C=((struct
Cyc_Absyn_UnionType_struct*)_tmp5A)->f3;if(_tmp8C == 0){goto _LL106;}else{_tmp8E=*
_tmp8C;goto _LL105;}}else{goto _LL106;}_LL106: if((unsigned int)_tmp5A > 3?*((int*)
_tmp5A)== Cyc_Absyn_EnumType: 0){_LL146: _tmp91=((struct Cyc_Absyn_EnumType_struct*)
_tmp5A)->f1;goto _LL107;}else{goto _LL108;}_LL108: if((unsigned int)_tmp5A > 3?*((
int*)_tmp5A)== Cyc_Absyn_TunionType: 0){_LL148: _tmp93=((struct Cyc_Absyn_TunionType_struct*)
_tmp5A)->f1;_LL152: _tmp97=(void*)_tmp93.tunion_info;if(*((int*)_tmp97)== Cyc_Absyn_KnownTunion){
_LL154: _tmp99=((struct Cyc_Absyn_KnownTunion_struct*)_tmp97)->f1;_tmp9B=*_tmp99;
goto _LL150;}else{goto _LL110;}_LL150: _tmp95=_tmp93.targs;goto _LL109;}else{goto
_LL110;}_LL110: if((unsigned int)_tmp5A > 3?*((int*)_tmp5A)== Cyc_Absyn_TunionFieldType:
0){_LL157: _tmp9C=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp5A)->f1;_LL161:
_tmpA0=(void*)_tmp9C.field_info;if(*((int*)_tmpA0)== Cyc_Absyn_KnownTunionfield){
_LL165: _tmpA4=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpA0)->f1;goto _LL163;
_LL163: _tmpA2=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpA0)->f2;goto _LL159;}
else{goto _LL112;}_LL159: _tmp9E=_tmp9C.targs;goto _LL111;}else{goto _LL112;}_LL112:
goto _LL113;_LL93: return Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp74);
_LL95: return Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp76);_LL97:
return Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp7B);_LL99: {int b=((
int(*)(struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,
struct _tuple0*name,void*(*f)(struct _tuple1*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(
ae,seen,obj,name,Cyc_Interface_get_type2,_tmp81)? Cyc_Interface_check_public_type(
ae,seen,obj,name,_tmp83): 0;if(_tmp7F != 0){void*_tmpA8;struct Cyc_Absyn_VarargInfo
_tmpA6=*_tmp7F;_LL169: _tmpA8=(void*)_tmpA6.type;goto _LL167;_LL167: b=Cyc_Interface_check_public_type(
ae,seen,obj,name,_tmpA8);}return b;}_LL101: return((int(*)(struct Cyc_Dict_Dict*ae,
struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct _tuple0*name,void*(*f)(
struct _tuple9*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(ae,
seen,obj,name,Cyc_Interface_get_type1,_tmp85);_LL103: if((void*)_tmp89->sc == (
void*)Cyc_Absyn_Static){Cyc_Interface_static_err(obj,name,_tag_arr("struct",
sizeof(unsigned char),7),(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(
_tmp89->name))->v);return 0;}return Cyc_Interface_check_public_type_list(ae,seen,
obj,name,Cyc_Core_identity,_tmp8A)? Cyc_Interface_check_public_structdecl(ae,seen,
_tmp89): 0;_LL105: if((void*)_tmp8E->sc == (void*)Cyc_Absyn_Static){Cyc_Interface_static_err(
obj,name,_tag_arr("union",sizeof(unsigned char),6),(struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(_tmp8E->name))->v);return 0;}return Cyc_Interface_check_public_type_list(
ae,seen,obj,name,Cyc_Core_identity,_tmp8F)? Cyc_Interface_check_public_uniondecl(
ae,seen,_tmp8E): 0;_LL107: {struct _tuple0 _tmpAC;struct _tagged_arr*_tmpAD;void*
_tmpAF;struct _tuple0*_tmpAA=_tmp91;_tmpAC=*_tmpAA;_LL176: _tmpAF=_tmpAC.f1;goto
_LL174;_LL174: _tmpAD=_tmpAC.f2;goto _LL171;_LL171: {struct Cyc_List_List*_tmpB1=
Cyc_Interface_get_abs_ns(_tmpAF);struct Cyc_Absyn_Enumdecl*ed;{struct
_handler_cons _tmpB2;_push_handler(& _tmpB2);{int _tmpB4=0;if(setjmp(_tmpB2.handler)){
_tmpB4=1;}if(! _tmpB4){{struct Cyc_Tcenv_Genv*_tmpB5=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmpB1);ed=*((
struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmpB5->enumdecls,_tmpAD);};_pop_handler();}else{void*_tmpB3=(void*)_exn_thrown;
void*_tmpB7=_tmpB3;_LL185: if(_tmpB7 == Cyc_Dict_Absent){goto _LL186;}else{goto
_LL187;}_LL187: goto _LL188;_LL186:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmpBD=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));_tmpBD[0]=({
struct Cyc_Core_Invalid_argument_struct _tmpBE;_tmpBE.tag=Cyc_Core_Invalid_argument;
_tmpBE.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpC0;_tmpC0.tag=
Cyc_Std_String_pa;_tmpC0.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp91);{
void*_tmpBF[1]={& _tmpC0};Cyc_Std_aprintf(_tag_arr("check_public_type (can't find enum %s)",
sizeof(unsigned char),39),_tag_arr(_tmpBF,sizeof(void*),1));}});_tmpBE;});_tmpBD;}));
_LL188:(void)_throw(_tmpB7);_LL184:;}}}if((void*)ed->sc == (void*)Cyc_Absyn_Static){
Cyc_Interface_static_err(obj,_tmp91,_tag_arr("enum",sizeof(unsigned char),5),ed->name);
return 0;}return 1;}}_LL109: {struct _tuple0 _tmpC3;struct _tagged_arr*_tmpC4;void*
_tmpC6;struct _tuple0*_tmpC1=_tmp9B->name;_tmpC3=*_tmpC1;_LL199: _tmpC6=_tmpC3.f1;
goto _LL197;_LL197: _tmpC4=_tmpC3.f2;goto _LL194;_LL194: {struct Cyc_List_List*
_tmpC8=Cyc_Interface_get_abs_ns(_tmpC6);struct Cyc_Absyn_Tuniondecl*tud;{struct
_handler_cons _tmpC9;_push_handler(& _tmpC9);{int _tmpCB=0;if(setjmp(_tmpC9.handler)){
_tmpCB=1;}if(! _tmpCB){{struct Cyc_Tcenv_Genv*_tmpCC=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmpC8);tud=*((
struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmpCC->tuniondecls,_tmpC4);};_pop_handler();}else{void*_tmpCA=(void*)
_exn_thrown;void*_tmpCE=_tmpCA;_LL208: if(_tmpCE == Cyc_Dict_Absent){goto _LL209;}
else{goto _LL210;}_LL210: goto _LL211;_LL209:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmpD4=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));_tmpD4[0]=({
struct Cyc_Core_Invalid_argument_struct _tmpD5;_tmpD5.tag=Cyc_Core_Invalid_argument;
_tmpD5.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpD7;_tmpD7.tag=
Cyc_Std_String_pa;_tmpD7.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp9B->name);{
void*_tmpD6[1]={& _tmpD7};Cyc_Std_aprintf(_tag_arr("check_public_type (can't find [x]tunion %s)",
sizeof(unsigned char),44),_tag_arr(_tmpD6,sizeof(void*),1));}});_tmpD5;});_tmpD4;}));
_LL211:(void)_throw(_tmpCE);_LL207:;}}}if((void*)tud->sc == (void*)Cyc_Absyn_Static){
Cyc_Interface_static_err(obj,name,_tag_arr("[x]tunion",sizeof(unsigned char),10),
tud->name);return 0;}return Cyc_Interface_check_public_type_list(ae,seen,obj,name,
Cyc_Core_identity,_tmp95)? Cyc_Interface_check_public_tuniondecl(ae,seen,tud): 0;}}
_LL111: {struct _tuple0 _tmpDA;struct _tagged_arr*_tmpDB;void*_tmpDD;struct _tuple0*
_tmpD8=_tmpA4->name;_tmpDA=*_tmpD8;_LL222: _tmpDD=_tmpDA.f1;goto _LL220;_LL220:
_tmpDB=_tmpDA.f2;goto _LL217;_LL217: {struct Cyc_List_List*_tmpDF=Cyc_Interface_get_abs_ns(
_tmpDD);struct Cyc_Absyn_Tuniondecl*tud;{struct _handler_cons _tmpE0;_push_handler(&
_tmpE0);{int _tmpE2=0;if(setjmp(_tmpE0.handler)){_tmpE2=1;}if(! _tmpE2){{struct Cyc_Tcenv_Genv*
_tmpE3=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(ae,_tmpDF);tud=*((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmpE3->tuniondecls,_tmpDB);};
_pop_handler();}else{void*_tmpE1=(void*)_exn_thrown;void*_tmpE5=_tmpE1;_LL231:
if(_tmpE5 == Cyc_Dict_Absent){goto _LL232;}else{goto _LL233;}_LL233: goto _LL234;
_LL232:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmpEB=
_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));_tmpEB[0]=({struct Cyc_Core_Invalid_argument_struct
_tmpEC;_tmpEC.tag=Cyc_Core_Invalid_argument;_tmpEC.f1=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmpEE;_tmpEE.tag=Cyc_Std_String_pa;_tmpEE.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpA4->name);{void*_tmpED[1]={& _tmpEE};
Cyc_Std_aprintf(_tag_arr("check_public_type (can't find [x]tunion %s and search its fields)",
sizeof(unsigned char),66),_tag_arr(_tmpED,sizeof(void*),1));}});_tmpEC;});_tmpEB;}));
_LL234:(void)_throw(_tmpE5);_LL230:;}}}if(tud->fields == 0){(int)_throw((void*)({
struct Cyc_Core_Invalid_argument_struct*_tmpEF=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));
_tmpEF[0]=({struct Cyc_Core_Invalid_argument_struct _tmpF0;_tmpF0.tag=Cyc_Core_Invalid_argument;
_tmpF0.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpF2;_tmpF2.tag=
Cyc_Std_String_pa;_tmpF2.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tud->name);{
void*_tmpF1[1]={& _tmpF2};Cyc_Std_aprintf(_tag_arr("check_public_type ([x]tunion %s has no fields)",
sizeof(unsigned char),47),_tag_arr(_tmpF1,sizeof(void*),1));}});_tmpF0;});_tmpEF;}));}{
struct Cyc_Absyn_Tunionfield*tuf1=0;{struct Cyc_List_List*_tmpF3=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmpF3 != 0;_tmpF3=_tmpF3->tl){
if(Cyc_Std_strptrcmp((*_tmpA2->name).f2,(*((struct Cyc_Absyn_Tunionfield*)_tmpF3->hd)->name).f2)
== 0){tuf1=(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)_tmpF3->hd);
break;}}}if(tuf1 == 0){(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmpF4=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));_tmpF4[0]=({
struct Cyc_Core_Invalid_argument_struct _tmpF5;_tmpF5.tag=Cyc_Core_Invalid_argument;
_tmpF5.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpF7;_tmpF7.tag=
Cyc_Std_String_pa;_tmpF7.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpA2->name);{
void*_tmpF6[1]={& _tmpF7};Cyc_Std_aprintf(_tag_arr("check_public_type (can't find [x]tunionfield %s)",
sizeof(unsigned char),49),_tag_arr(_tmpF6,sizeof(void*),1));}});_tmpF5;});_tmpF4;}));}{
struct Cyc_Absyn_Tunionfield*tuf=(struct Cyc_Absyn_Tunionfield*)_check_null(tuf1);
if((void*)tud->sc == (void*)Cyc_Absyn_Static){Cyc_Interface_static_err(obj,name,
_tag_arr("[x]tunion",sizeof(unsigned char),10),tud->name);return 0;}if((void*)tud->sc
== (void*)Cyc_Absyn_Abstract){Cyc_Interface_abstract_err(obj,name,_tag_arr("[x]tunion",
sizeof(unsigned char),10),tud->name);return 0;}if((void*)tuf->sc == (void*)Cyc_Absyn_Static){
Cyc_Interface_static_err(obj,name,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmpF9;_tmpF9.tag=Cyc_Std_String_pa;_tmpF9.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
tuf->name);{void*_tmpF8[1]={& _tmpF9};Cyc_Std_aprintf(_tag_arr("field %s of",
sizeof(unsigned char),12),_tag_arr(_tmpF8,sizeof(void*),1));}}),tud->name);
return 0;}return Cyc_Interface_check_public_type_list(ae,seen,obj,name,Cyc_Core_identity,
_tmp9E)? Cyc_Interface_check_public_tuniondecl(ae,seen,tud): 0;}}}}_LL113: return 1;
_LL91:;}struct _tuple10{struct Cyc_Interface_Ienv*f1;struct Cyc_Interface_Ienv*f2;
int f3;struct Cyc_Dict_Dict*f4;struct Cyc_Interface_Seen*f5;struct Cyc_Interface_I*
f6;};static void Cyc_Interface_extract_structdecl(struct _tuple10*env,struct
_tagged_arr*x,struct Cyc_Absyn_Structdecl**dp){struct _tuple10 _tmpFC;struct Cyc_Interface_Seen*
_tmpFD;struct Cyc_Dict_Dict*_tmpFF;int _tmp101;struct Cyc_Interface_Ienv*_tmp103;
struct Cyc_Interface_Ienv*_tmp105;struct _tuple10*_tmpFA=env;_tmpFC=*_tmpFA;_LL262:
_tmp105=_tmpFC.f1;goto _LL260;_LL260: _tmp103=_tmpFC.f2;goto _LL258;_LL258: _tmp101=
_tmpFC.f3;goto _LL256;_LL256: _tmpFF=_tmpFC.f4;goto _LL254;_LL254: _tmpFD=_tmpFC.f5;
goto _LL251;_LL251: {struct Cyc_Absyn_Structdecl*_tmp107=*dp;void*_tmp108=(void*)
_tmp107->sc;_LL266: if(_tmp108 == (void*)Cyc_Absyn_Static){goto _LL267;}else{goto
_LL268;}_LL268: if(_tmp108 == (void*)Cyc_Absyn_Abstract){goto _LL269;}else{goto
_LL270;}_LL270: if(_tmp108 == (void*)Cyc_Absyn_Public){goto _LL271;}else{goto _LL272;}
_LL272: if(_tmp108 == (void*)Cyc_Absyn_ExternC){goto _LL273;}else{goto _LL274;}
_LL274: if(_tmp108 == (void*)Cyc_Absyn_Extern){goto _LL275;}else{goto _LL265;}_LL267:
if(_tmp101? _tmp107->fields == 0: 0){Cyc_Interface_fields_err(_tag_arr("static",
sizeof(unsigned char),7),_tag_arr("struct",sizeof(unsigned char),7),(struct
_tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp107->name))->v);}goto _LL265;
_LL269: if(_tmp107->fields == 0){if(_tmp101){Cyc_Interface_fields_err(_tag_arr("abstract",
sizeof(unsigned char),9),_tag_arr("struct",sizeof(unsigned char),7),(struct
_tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp107->name))->v);}}else{_tmp107=({
struct Cyc_Absyn_Structdecl*_tmp114=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));
_tmp114[0]=*_tmp107;_tmp114;});_tmp107->fields=0;}if(Cyc_Interface_check_public_structdecl(
_tmpFF,_tmpFD,_tmp107)){_tmp103->structdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Structdecl*v))Cyc_Dict_insert)(_tmp103->structdecls,
x,_tmp107);}goto _LL265;_LL271: if(_tmp107->fields == 0){Cyc_Interface_fields_err(
_tag_arr("public",sizeof(unsigned char),7),_tag_arr("struct",sizeof(
unsigned char),7),(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp107->name))->v);
_tmp107=({struct Cyc_Absyn_Structdecl*_tmp115=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));
_tmp115[0]=*_tmp107;_tmp115;});(void*)(_tmp107->sc=(void*)((void*)Cyc_Absyn_Abstract));}
if(Cyc_Interface_check_public_structdecl(_tmpFF,_tmpFD,_tmp107)){_tmp103->structdecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Structdecl*
v))Cyc_Dict_insert)(_tmp103->structdecls,x,_tmp107);}goto _LL265;_LL273: goto
_LL275;_LL275: if(Cyc_Interface_check_public_structdecl(_tmpFF,_tmpFD,_tmp107)){
_tmp105->structdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct Cyc_Absyn_Structdecl*v))Cyc_Dict_insert)(_tmp105->structdecls,
x,_tmp107);}goto _LL265;_LL265:;}}static void Cyc_Interface_extract_uniondecl(
struct _tuple10*env,struct _tagged_arr*x,struct Cyc_Absyn_Uniondecl**dp){struct
_tuple10 _tmp118;struct Cyc_Interface_Seen*_tmp119;struct Cyc_Dict_Dict*_tmp11B;int
_tmp11D;struct Cyc_Interface_Ienv*_tmp11F;struct Cyc_Interface_Ienv*_tmp121;struct
_tuple10*_tmp116=env;_tmp118=*_tmp116;_LL290: _tmp121=_tmp118.f1;goto _LL288;
_LL288: _tmp11F=_tmp118.f2;goto _LL286;_LL286: _tmp11D=_tmp118.f3;goto _LL284;_LL284:
_tmp11B=_tmp118.f4;goto _LL282;_LL282: _tmp119=_tmp118.f5;goto _LL279;_LL279: {
struct Cyc_Absyn_Uniondecl*_tmp123=*dp;void*_tmp124=(void*)_tmp123->sc;_LL294: if(
_tmp124 == (void*)Cyc_Absyn_Static){goto _LL295;}else{goto _LL296;}_LL296: if(
_tmp124 == (void*)Cyc_Absyn_Abstract){goto _LL297;}else{goto _LL298;}_LL298: if(
_tmp124 == (void*)Cyc_Absyn_Public){goto _LL299;}else{goto _LL300;}_LL300: if(
_tmp124 == (void*)Cyc_Absyn_ExternC){goto _LL301;}else{goto _LL302;}_LL302: if(
_tmp124 == (void*)Cyc_Absyn_Extern){goto _LL303;}else{goto _LL293;}_LL295: if(
_tmp11D? _tmp123->fields == 0: 0){Cyc_Interface_fields_err(_tag_arr("static",
sizeof(unsigned char),7),_tag_arr("union",sizeof(unsigned char),6),(struct
_tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp123->name))->v);}goto _LL293;
_LL297: if(_tmp123->fields == 0){if(_tmp11D){Cyc_Interface_fields_err(_tag_arr("abstract",
sizeof(unsigned char),9),_tag_arr("union",sizeof(unsigned char),6),(struct
_tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp123->name))->v);}}else{_tmp123=({
struct Cyc_Absyn_Uniondecl*_tmp130=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl));
_tmp130[0]=*_tmp123;_tmp130;});_tmp123->fields=0;}if(Cyc_Interface_check_public_uniondecl(
_tmp11B,_tmp119,_tmp123)){_tmp11F->uniondecls=((struct Cyc_Dict_Dict*(*)(struct
Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Uniondecl*v))Cyc_Dict_insert)(
_tmp11F->uniondecls,x,_tmp123);}goto _LL293;_LL299: if(_tmp123->fields == 0){Cyc_Interface_fields_err(
_tag_arr("public",sizeof(unsigned char),7),_tag_arr("union",sizeof(unsigned char),
6),(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp123->name))->v);_tmp123=({
struct Cyc_Absyn_Uniondecl*_tmp131=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl));
_tmp131[0]=*_tmp123;_tmp131;});(void*)(_tmp123->sc=(void*)((void*)Cyc_Absyn_Abstract));}
if(Cyc_Interface_check_public_uniondecl(_tmp11B,_tmp119,_tmp123)){_tmp11F->uniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Uniondecl*
v))Cyc_Dict_insert)(_tmp11F->uniondecls,x,_tmp123);}goto _LL293;_LL301: goto _LL303;
_LL303: if(Cyc_Interface_check_public_uniondecl(_tmp11B,_tmp119,_tmp123)){_tmp121->uniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Uniondecl*
v))Cyc_Dict_insert)(_tmp121->uniondecls,x,_tmp123);}goto _LL293;_LL293:;}}static
void Cyc_Interface_extract_enumdecl(struct _tuple10*env,struct _tagged_arr*x,struct
Cyc_Absyn_Enumdecl**dp){struct _tuple10 _tmp134;struct Cyc_Interface_Seen*_tmp135;
struct Cyc_Dict_Dict*_tmp137;int _tmp139;struct Cyc_Interface_Ienv*_tmp13B;struct
Cyc_Interface_Ienv*_tmp13D;struct _tuple10*_tmp132=env;_tmp134=*_tmp132;_LL318:
_tmp13D=_tmp134.f1;goto _LL316;_LL316: _tmp13B=_tmp134.f2;goto _LL314;_LL314:
_tmp139=_tmp134.f3;goto _LL312;_LL312: _tmp137=_tmp134.f4;goto _LL310;_LL310:
_tmp135=_tmp134.f5;goto _LL307;_LL307: {struct Cyc_Absyn_Enumdecl*_tmp13F=*dp;void*
_tmp140=(void*)_tmp13F->sc;_LL322: if(_tmp140 == (void*)Cyc_Absyn_Static){goto
_LL323;}else{goto _LL324;}_LL324: if(_tmp140 == (void*)Cyc_Absyn_Abstract){goto
_LL325;}else{goto _LL326;}_LL326: if(_tmp140 == (void*)Cyc_Absyn_Public){goto _LL327;}
else{goto _LL328;}_LL328: if(_tmp140 == (void*)Cyc_Absyn_ExternC){goto _LL329;}else{
goto _LL330;}_LL330: if(_tmp140 == (void*)Cyc_Absyn_Extern){goto _LL331;}else{goto
_LL321;}_LL323: if(_tmp139? _tmp13F->fields == 0: 0){Cyc_Interface_fields_err(
_tag_arr("static",sizeof(unsigned char),7),_tag_arr("enum",sizeof(unsigned char),
5),_tmp13F->name);}goto _LL321;_LL325: if(_tmp13F->fields == 0){if(_tmp139){Cyc_Interface_fields_err(
_tag_arr("abstract",sizeof(unsigned char),9),_tag_arr("enum",sizeof(
unsigned char),5),_tmp13F->name);}}else{_tmp13F=({struct Cyc_Absyn_Enumdecl*
_tmp14C=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_tmp14C[0]=*_tmp13F;_tmp14C;});
_tmp13F->fields=0;}if(Cyc_Interface_check_public_enumdecl(_tmp137,_tmp135,
_tmp13F)){_tmp13B->enumdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp13B->enumdecls,
x,_tmp13F);}goto _LL321;_LL327: if(_tmp13F->fields == 0){Cyc_Interface_fields_err(
_tag_arr("public",sizeof(unsigned char),7),_tag_arr("enum",sizeof(unsigned char),
5),_tmp13F->name);_tmp13F=({struct Cyc_Absyn_Enumdecl*_tmp14D=_cycalloc(sizeof(
struct Cyc_Absyn_Enumdecl));_tmp14D[0]=*_tmp13F;_tmp14D;});(void*)(_tmp13F->sc=(
void*)((void*)Cyc_Absyn_Abstract));}if(Cyc_Interface_check_public_enumdecl(
_tmp137,_tmp135,_tmp13F)){_tmp13B->enumdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp13B->enumdecls,
x,_tmp13F);}goto _LL321;_LL329: goto _LL331;_LL331: if(Cyc_Interface_check_public_enumdecl(
_tmp137,_tmp135,_tmp13F)){_tmp13D->enumdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp13D->enumdecls,
x,_tmp13F);}goto _LL321;_LL321:;}}static void Cyc_Interface_extract_xtunionfielddecl(
struct Cyc_Interface_I*i,struct Cyc_Absyn_Tuniondecl*d,struct Cyc_Absyn_Tunionfield*
f){struct Cyc_List_List*_tmp14E=0;{void*_tmp14F=(*f->name).f1;struct Cyc_List_List*
_tmp157;struct Cyc_List_List*_tmp159;_LL337: if((unsigned int)_tmp14F > 1?*((int*)
_tmp14F)== Cyc_Absyn_Rel_n: 0){_LL344: _tmp157=((struct Cyc_Absyn_Rel_n_struct*)
_tmp14F)->f1;goto _LL338;}else{goto _LL339;}_LL339: if((unsigned int)_tmp14F > 1?*((
int*)_tmp14F)== Cyc_Absyn_Abs_n: 0){_LL346: _tmp159=((struct Cyc_Absyn_Abs_n_struct*)
_tmp14F)->f1;goto _LL340;}else{goto _LL341;}_LL341: goto _LL342;_LL338: _tmp159=
_tmp157;goto _LL340;_LL340: _tmp14E=_tmp159;goto _LL336;_LL342: goto _LL336;_LL336:;}{
struct Cyc_Dict_Dict**dict;{void*_tmp15B=(void*)f->sc;_LL349: if(_tmp15B == (void*)
Cyc_Absyn_Static){goto _LL350;}else{goto _LL351;}_LL351: if(_tmp15B == (void*)Cyc_Absyn_Extern){
goto _LL352;}else{goto _LL353;}_LL353: if(_tmp15B == (void*)Cyc_Absyn_Public){goto
_LL354;}else{goto _LL355;}_LL355: goto _LL356;_LL350: return;_LL352: dict=& i->imports;
goto _LL348;_LL354: dict=& i->exports;goto _LL348;_LL356:(int)_throw((void*)({struct
Cyc_Core_Invalid_argument_struct*_tmp165=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));
_tmp165[0]=({struct Cyc_Core_Invalid_argument_struct _tmp166;_tmp166.tag=Cyc_Core_Invalid_argument;
_tmp166.f1=_tag_arr("add_xtunionfielddecl",sizeof(unsigned char),21);_tmp166;});
_tmp165;}));_LL348:;}{struct Cyc_Interface_Ienv*env;{struct _handler_cons _tmp167;
_push_handler(& _tmp167);{int _tmp169=0;if(setjmp(_tmp167.handler)){_tmp169=1;}if(
! _tmp169){env=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(*dict,_tmp14E);;_pop_handler();}else{void*_tmp168=(void*)
_exn_thrown;void*_tmp16B=_tmp168;_LL365: if(_tmp16B == Cyc_Dict_Absent){goto _LL366;}
else{goto _LL367;}_LL367: goto _LL368;_LL366: env=Cyc_Interface_new_ienv();*dict=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*
v))Cyc_Dict_insert)(*dict,_tmp14E,env);goto _LL364;_LL368:(void)_throw(_tmp16B);
_LL364:;}}}env->xtunionfielddecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Tcdecl_Xtunionfielddecl*v))Cyc_Dict_insert)(env->xtunionfielddecls,(*
f->name).f2,({struct Cyc_Tcdecl_Xtunionfielddecl*_tmp171=_cycalloc(sizeof(struct
Cyc_Tcdecl_Xtunionfielddecl));_tmp171->base=d;_tmp171->field=f;_tmp171;}));}}}
static void Cyc_Interface_extract_tuniondecl(struct _tuple10*env,struct _tagged_arr*
x,struct Cyc_Absyn_Tuniondecl**dp){struct _tuple10 _tmp174;struct Cyc_Interface_I*
_tmp175;struct Cyc_Interface_Seen*_tmp177;struct Cyc_Dict_Dict*_tmp179;int _tmp17B;
struct Cyc_Interface_Ienv*_tmp17D;struct Cyc_Interface_Ienv*_tmp17F;struct _tuple10*
_tmp172=env;_tmp174=*_tmp172;_LL384: _tmp17F=_tmp174.f1;goto _LL382;_LL382: _tmp17D=
_tmp174.f2;goto _LL380;_LL380: _tmp17B=_tmp174.f3;goto _LL378;_LL378: _tmp179=
_tmp174.f4;goto _LL376;_LL376: _tmp177=_tmp174.f5;goto _LL374;_LL374: _tmp175=
_tmp174.f6;goto _LL371;_LL371: {struct Cyc_Absyn_Tuniondecl*_tmp181=*dp;void*
_tmp182=(void*)_tmp181->sc;_LL388: if(_tmp182 == (void*)Cyc_Absyn_Static){goto
_LL389;}else{goto _LL390;}_LL390: if(_tmp182 == (void*)Cyc_Absyn_Abstract){goto
_LL391;}else{goto _LL392;}_LL392: if(_tmp182 == (void*)Cyc_Absyn_Public){goto _LL393;}
else{goto _LL394;}_LL394: if(_tmp182 == (void*)Cyc_Absyn_ExternC){goto _LL395;}else{
goto _LL396;}_LL396: if(_tmp182 == (void*)Cyc_Absyn_Extern){goto _LL397;}else{goto
_LL387;}_LL389: if((! _tmp181->is_xtunion? _tmp17B: 0)? _tmp181->fields == 0: 0){Cyc_Interface_fields_err(
_tag_arr("static",sizeof(unsigned char),7),_tag_arr("tunion",sizeof(
unsigned char),7),_tmp181->name);}goto _LL387;_LL391: if(_tmp181->fields == 0){if(!
_tmp181->is_xtunion? _tmp17B: 0){Cyc_Interface_fields_err(_tag_arr("abstract",
sizeof(unsigned char),9),_tag_arr("tunion",sizeof(unsigned char),7),_tmp181->name);}}
else{_tmp181=({struct Cyc_Absyn_Tuniondecl*_tmp18E=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));
_tmp18E[0]=*_tmp181;_tmp18E;});_tmp181->fields=0;}if(Cyc_Interface_check_public_tuniondecl(
_tmp179,_tmp177,_tmp181)){_tmp17D->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct
Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl*v))Cyc_Dict_insert)(
_tmp17D->tuniondecls,x,_tmp181);}goto _LL387;_LL393: _tmp181=({struct Cyc_Absyn_Tuniondecl*
_tmp18F=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));_tmp18F[0]=*_tmp181;
_tmp18F;});if(! _tmp181->is_xtunion? _tmp181->fields == 0: 0){Cyc_Interface_fields_err(
_tag_arr("public",sizeof(unsigned char),7),_tag_arr("tunion",sizeof(
unsigned char),7),_tmp181->name);(void*)(_tmp181->sc=(void*)((void*)Cyc_Absyn_Abstract));}
if(Cyc_Interface_check_public_tuniondecl(_tmp179,_tmp177,_tmp181)){if(_tmp181->is_xtunion?
_tmp181->fields != 0: 0){struct Cyc_List_List*_tmp190=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp181->fields))->v;_tmp181->fields=0;{struct
Cyc_List_List*_tmp191=_tmp190;for(0;_tmp191 != 0;_tmp191=_tmp191->tl){Cyc_Interface_extract_xtunionfielddecl(
_tmp175,_tmp181,(struct Cyc_Absyn_Tunionfield*)_tmp191->hd);}}}_tmp17D->tuniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl*
v))Cyc_Dict_insert)(_tmp17D->tuniondecls,x,_tmp181);}goto _LL387;_LL395:(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp192=_cycalloc(sizeof(
struct Cyc_Core_Invalid_argument_struct));_tmp192[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp193;_tmp193.tag=Cyc_Core_Invalid_argument;_tmp193.f1=_tag_arr("extract_tuniondecl",
sizeof(unsigned char),19);_tmp193;});_tmp192;}));_LL397: if(Cyc_Interface_check_public_tuniondecl(
_tmp179,_tmp177,_tmp181)){if(_tmp181->is_xtunion? _tmp181->fields != 0: 0){_tmp181=({
struct Cyc_Absyn_Tuniondecl*_tmp194=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));
_tmp194[0]=*_tmp181;_tmp194;});{struct Cyc_List_List*_tmp195=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp181->fields))->v;_tmp181->fields=0;{struct
Cyc_List_List*_tmp196=_tmp195;for(0;_tmp196 != 0;_tmp196=_tmp196->tl){Cyc_Interface_extract_xtunionfielddecl(
_tmp175,_tmp181,(struct Cyc_Absyn_Tunionfield*)_tmp196->hd);}}}}_tmp17F->tuniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl*
v))Cyc_Dict_insert)(_tmp17F->tuniondecls,x,_tmp181);}goto _LL387;_LL387:;}}static
void Cyc_Interface_extract_typedef(struct _tuple10*env,struct _tagged_arr*x,struct
Cyc_Absyn_Typedefdecl*d){struct _tuple10 _tmp199;struct Cyc_Interface_Seen*_tmp19A;
struct Cyc_Dict_Dict*_tmp19C;struct Cyc_Interface_Ienv*_tmp19E;struct _tuple10*
_tmp197=env;_tmp199=*_tmp197;_LL415: _tmp19E=_tmp199.f2;goto _LL413;_LL413: _tmp19C=
_tmp199.f4;goto _LL411;_LL411: _tmp19A=_tmp199.f5;goto _LL408;_LL408: if(Cyc_Interface_check_public_typedefdecl(
_tmp19C,_tmp19A,d)){_tmp19E->typedefdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmp19E->typedefdecls,
x,d);}}static void Cyc_Interface_extract_ordinarie(struct _tuple10*env,struct
_tagged_arr*x,struct _tuple6*v){struct _tuple10 _tmp1A2;struct Cyc_Interface_Seen*
_tmp1A3;struct Cyc_Dict_Dict*_tmp1A5;int _tmp1A7;struct Cyc_Interface_Ienv*_tmp1A9;
struct Cyc_Interface_Ienv*_tmp1AB;struct _tuple10*_tmp1A0=env;_tmp1A2=*_tmp1A0;
_LL428: _tmp1AB=_tmp1A2.f1;goto _LL426;_LL426: _tmp1A9=_tmp1A2.f2;goto _LL424;_LL424:
_tmp1A7=_tmp1A2.f3;goto _LL422;_LL422: _tmp1A5=_tmp1A2.f4;goto _LL420;_LL420:
_tmp1A3=_tmp1A2.f5;goto _LL417;_LL417: {void*_tmp1AD=(*v).f1;void*_tmp1AE=_tmp1AD;
void*_tmp1B4;_LL432: if(*((int*)_tmp1AE)== Cyc_Tcenv_VarRes){_LL437: _tmp1B4=(void*)((
struct Cyc_Tcenv_VarRes_struct*)_tmp1AE)->f1;goto _LL433;}else{goto _LL434;}_LL434:
goto _LL435;_LL433:{void*_tmp1B6=_tmp1B4;struct Cyc_Absyn_Fndecl*_tmp1BE;struct Cyc_Absyn_Vardecl*
_tmp1C0;_LL440: if((unsigned int)_tmp1B6 > 1?*((int*)_tmp1B6)== Cyc_Absyn_Funname_b:
0){_LL447: _tmp1BE=((struct Cyc_Absyn_Funname_b_struct*)_tmp1B6)->f1;goto _LL441;}
else{goto _LL442;}_LL442: if((unsigned int)_tmp1B6 > 1?*((int*)_tmp1B6)== Cyc_Absyn_Global_b:
0){_LL449: _tmp1C0=((struct Cyc_Absyn_Global_b_struct*)_tmp1B6)->f1;goto _LL443;}
else{goto _LL444;}_LL444: goto _LL445;_LL441: {struct Cyc_Absyn_Vardecl*_tmp1C2=({
struct Cyc_Absyn_Vardecl*_tmp1C3=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));
_tmp1C3->sc=(void*)((void*)_tmp1BE->sc);_tmp1C3->name=_tmp1BE->name;_tmp1C3->tq=({
struct Cyc_Absyn_Tqual _tmp1C4;_tmp1C4.q_const=0;_tmp1C4.q_volatile=0;_tmp1C4.q_restrict=
0;_tmp1C4;});_tmp1C3->type=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1BE->cached_typ))->v);_tmp1C3->initializer=0;_tmp1C3->rgn=0;_tmp1C3->attributes=
0;_tmp1C3->escapes=0;_tmp1C3;});_tmp1A7=0;_tmp1C0=_tmp1C2;goto _LL443;}_LL443: if(
_tmp1C0->initializer != 0){_tmp1C0=({struct Cyc_Absyn_Vardecl*_tmp1C5=_cycalloc(
sizeof(struct Cyc_Absyn_Vardecl));_tmp1C5[0]=*_tmp1C0;_tmp1C5;});_tmp1C0->initializer=
0;}{void*_tmp1C6=(void*)_tmp1C0->sc;_LL456: if(_tmp1C6 == (void*)Cyc_Absyn_Static){
goto _LL457;}else{goto _LL458;}_LL458: if(_tmp1C6 == (void*)Cyc_Absyn_Abstract){goto
_LL459;}else{goto _LL460;}_LL460: if(_tmp1C6 == (void*)Cyc_Absyn_Public){goto _LL461;}
else{goto _LL462;}_LL462: if(_tmp1C6 == (void*)Cyc_Absyn_ExternC){goto _LL463;}else{
goto _LL464;}_LL464: if(_tmp1C6 == (void*)Cyc_Absyn_Extern){goto _LL465;}else{goto
_LL455;}_LL457: if(_tmp1A7? Cyc_Tcutil_is_function_type((void*)_tmp1C0->type): 0){
Cyc_Interface_body_err(_tag_arr("static",sizeof(unsigned char),7),_tmp1C0->name);}
goto _LL455;_LL459:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp1D2=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_struct));_tmp1D2[0]=({
struct Cyc_Core_Invalid_argument_struct _tmp1D3;_tmp1D3.tag=Cyc_Core_Invalid_argument;
_tmp1D3.f1=_tag_arr("extract_ordinarie",sizeof(unsigned char),18);_tmp1D3;});
_tmp1D2;}));_LL461: if(_tmp1A7? Cyc_Tcutil_is_function_type((void*)_tmp1C0->type):
0){Cyc_Interface_body_err(_tag_arr("public",sizeof(unsigned char),7),_tmp1C0->name);}
if(Cyc_Interface_check_public_vardecl(_tmp1A5,_tmp1A3,_tmp1C0)){_tmp1A9->vardecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Vardecl*
v))Cyc_Dict_insert)(_tmp1A9->vardecls,x,_tmp1C0);}goto _LL455;_LL463: goto _LL465;
_LL465: if(Cyc_Interface_check_public_vardecl(_tmp1A5,_tmp1A3,_tmp1C0)){_tmp1AB->vardecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Vardecl*
v))Cyc_Dict_insert)(_tmp1AB->vardecls,x,_tmp1C0);}goto _LL455;_LL455:;}goto _LL439;
_LL445: goto _LL439;_LL439:;}goto _LL431;_LL435: goto _LL431;_LL431:;}}struct _tuple11{
struct Cyc_Interface_I*f1;int f2;struct Cyc_Dict_Dict*f3;struct Cyc_Interface_Seen*
f4;};static void Cyc_Interface_extract_f(struct _tuple11*env_f,struct Cyc_List_List*
ns,struct Cyc_Tcenv_Genv*ge){struct _tuple11 _tmp1D6;struct Cyc_Interface_Seen*
_tmp1D7;struct Cyc_Dict_Dict*_tmp1D9;int _tmp1DB;struct Cyc_Interface_I*_tmp1DD;
struct _tuple11*_tmp1D4=env_f;_tmp1D6=*_tmp1D4;_LL478: _tmp1DD=_tmp1D6.f1;goto
_LL476;_LL476: _tmp1DB=_tmp1D6.f2;goto _LL474;_LL474: _tmp1D9=_tmp1D6.f3;goto _LL472;
_LL472: _tmp1D7=_tmp1D6.f4;goto _LL469;_LL469: {struct _tuple10 _tmp1DF=({struct
_tuple10 _tmp1E0;_tmp1E0.f1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp1DD->imports,ns);_tmp1E0.f2=((struct
Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
_tmp1DD->exports,ns);_tmp1E0.f3=_tmp1DB;_tmp1E0.f4=_tmp1D9;_tmp1E0.f5=_tmp1D7;
_tmp1E0.f6=_tmp1DD;_tmp1E0;});((void(*)(void(*f)(struct _tuple10*,struct
_tagged_arr*,struct Cyc_Absyn_Structdecl**),struct _tuple10*env,struct Cyc_Dict_Dict*
d))Cyc_Dict_iter_c)(Cyc_Interface_extract_structdecl,& _tmp1DF,ge->structdecls);((
void(*)(void(*f)(struct _tuple10*,struct _tagged_arr*,struct Cyc_Absyn_Uniondecl**),
struct _tuple10*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_extract_uniondecl,&
_tmp1DF,ge->uniondecls);((void(*)(void(*f)(struct _tuple10*,struct _tagged_arr*,
struct Cyc_Absyn_Tuniondecl**),struct _tuple10*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_extract_tuniondecl,& _tmp1DF,ge->tuniondecls);((void(*)(void(*f)(
struct _tuple10*,struct _tagged_arr*,struct Cyc_Absyn_Enumdecl**),struct _tuple10*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_extract_enumdecl,&
_tmp1DF,ge->enumdecls);((void(*)(void(*f)(struct _tuple10*,struct _tagged_arr*,
struct Cyc_Absyn_Typedefdecl*),struct _tuple10*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_extract_typedef,& _tmp1DF,ge->typedefs);((void(*)(void(*f)(struct
_tuple10*,struct _tagged_arr*,struct _tuple6*),struct _tuple10*env,struct Cyc_Dict_Dict*
d))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,& _tmp1DF,ge->ordinaries);}}
static struct Cyc_Interface_I*Cyc_Interface_gen_extract(struct Cyc_Dict_Dict*ae,int
check_complete_defs){struct _tuple11 _tmp1E1=({struct _tuple11 _tmp1E2;_tmp1E2.f1=((
struct Cyc_Interface_I*(*)(struct Cyc_Dict_Dict*skel))Cyc_Interface_skel2i)(ae);
_tmp1E2.f2=check_complete_defs;_tmp1E2.f3=ae;_tmp1E2.f4=Cyc_Interface_new_seen();
_tmp1E2;});((void(*)(void(*f)(struct _tuple11*,struct Cyc_List_List*,struct Cyc_Tcenv_Genv*),
struct _tuple11*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_extract_f,&
_tmp1E1,ae);return _tmp1E1.f1;}struct Cyc_Interface_I*Cyc_Interface_extract(struct
Cyc_Dict_Dict*ae){return Cyc_Interface_gen_extract(ae,1);}inline static void Cyc_Interface_check_err(
struct _tagged_arr*msg1,struct _tagged_arr msg2){({struct Cyc_Std_String_pa_struct
_tmp1E4;_tmp1E4.tag=Cyc_Std_String_pa;_tmp1E4.f1=(struct _tagged_arr)msg2;{void*
_tmp1E3[1]={& _tmp1E4};Cyc_Tcdecl_merr(0,msg1,_tag_arr("%s",sizeof(unsigned char),
3),_tag_arr(_tmp1E3,sizeof(void*),1));}});}struct _tuple12{int f1;struct Cyc_Dict_Dict*
f2;int(*f3)(void*,void*,struct _tagged_arr*);struct _tagged_arr f4;struct
_tagged_arr*f5;};static void Cyc_Interface_incl_dict_f(struct _tuple12*env,struct
_tagged_arr*x,void*y1){struct _tuple12 _tmp1E7;struct _tagged_arr*_tmp1E8;struct
_tagged_arr _tmp1EA;int(*_tmp1EC)(void*,void*,struct _tagged_arr*);struct Cyc_Dict_Dict*
_tmp1EE;int _tmp1F0;int*_tmp1F2;struct _tuple12*_tmp1E5=env;_tmp1E7=*_tmp1E5;
_LL497: _tmp1F0=_tmp1E7.f1;_tmp1F2=(int*)&(*_tmp1E5).f1;goto _LL495;_LL495: _tmp1EE=
_tmp1E7.f2;goto _LL493;_LL493: _tmp1EC=_tmp1E7.f3;goto _LL491;_LL491: _tmp1EA=
_tmp1E7.f4;goto _LL489;_LL489: _tmp1E8=_tmp1E7.f5;goto _LL486;_LL486: {struct
_handler_cons _tmp1F3;_push_handler(& _tmp1F3);{int _tmp1F5=0;if(setjmp(_tmp1F3.handler)){
_tmp1F5=1;}if(! _tmp1F5){{void*_tmp1F6=((void*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_lookup)(_tmp1EE,x);if(! _tmp1EC(y1,_tmp1F6,_tmp1E8)){*
_tmp1F2=0;}};_pop_handler();}else{void*_tmp1F4=(void*)_exn_thrown;void*_tmp1F8=
_tmp1F4;_LL506: if(_tmp1F8 == Cyc_Dict_Absent){goto _LL507;}else{goto _LL508;}_LL508:
goto _LL509;_LL507: Cyc_Interface_check_err(_tmp1E8,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp200;_tmp200.tag=Cyc_Std_String_pa;_tmp200.f1=(struct _tagged_arr)*x;{struct
Cyc_Std_String_pa_struct _tmp1FF;_tmp1FF.tag=Cyc_Std_String_pa;_tmp1FF.f1=(struct
_tagged_arr)_tmp1EA;{void*_tmp1FE[2]={& _tmp1FF,& _tmp200};Cyc_Std_aprintf(
_tag_arr("%s %s is missing",sizeof(unsigned char),17),_tag_arr(_tmp1FE,sizeof(
void*),2));}}}));*_tmp1F2=0;goto _LL505;_LL509:(void)_throw(_tmp1F8);_LL505:;}}}}
static int Cyc_Interface_incl_dict(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,int(*incl_f)(void*,void*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg){struct _tuple12 _tmp201=({struct _tuple12 _tmp202;_tmp202.f1=1;
_tmp202.f2=dic2;_tmp202.f3=incl_f;_tmp202.f4=t;_tmp202.f5=msg;_tmp202;});((void(*)(
void(*f)(struct _tuple12*,struct _tagged_arr*,void*),struct _tuple12*env,struct Cyc_Dict_Dict*
d))Cyc_Dict_iter_c)(Cyc_Interface_incl_dict_f,& _tmp201,dic1);return _tmp201.f1;}
static int Cyc_Interface_incl_structdecl(struct Cyc_Absyn_Structdecl*d0,struct Cyc_Absyn_Structdecl*
d1,struct _tagged_arr*msg){struct Cyc_Absyn_Structdecl*_tmp203=Cyc_Tcdecl_merge_structdecl(
d0,d1,0,msg);if(_tmp203 == 0){return 0;}if((struct Cyc_Absyn_Structdecl*)d0 != 
_tmp203){Cyc_Interface_check_err(msg,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp205;_tmp205.tag=Cyc_Std_String_pa;_tmp205.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((
struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(d1->name))->v);{void*_tmp204[1]={&
_tmp205};Cyc_Std_aprintf(_tag_arr("declaration of struct %s discloses too much information",
sizeof(unsigned char),56),_tag_arr(_tmp204,sizeof(void*),1));}}));return 0;}
return 1;}static int Cyc_Interface_incl_uniondecl(struct Cyc_Absyn_Uniondecl*d0,
struct Cyc_Absyn_Uniondecl*d1,struct _tagged_arr*msg){struct Cyc_Absyn_Uniondecl*
_tmp206=Cyc_Tcdecl_merge_uniondecl(d0,d1,0,msg);if(_tmp206 == 0){return 0;}if((
struct Cyc_Absyn_Uniondecl*)d0 != _tmp206){Cyc_Interface_check_err(msg,(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp208;_tmp208.tag=Cyc_Std_String_pa;
_tmp208.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((struct _tuple0*)((struct
Cyc_Core_Opt*)_check_null(d1->name))->v);{void*_tmp207[1]={& _tmp208};Cyc_Std_aprintf(
_tag_arr("declaration of union %s discloses too much information",sizeof(
unsigned char),55),_tag_arr(_tmp207,sizeof(void*),1));}}));return 0;}return 1;}
static int Cyc_Interface_incl_tuniondecl(struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*
d1,struct _tagged_arr*msg){struct Cyc_Absyn_Tuniondecl*_tmp209=Cyc_Tcdecl_merge_tuniondecl(
d0,d1,0,msg);if(_tmp209 == 0){return 0;}if((struct Cyc_Absyn_Tuniondecl*)d0 != 
_tmp209){Cyc_Interface_check_err(msg,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp20B;_tmp20B.tag=Cyc_Std_String_pa;_tmp20B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
d1->name);{void*_tmp20A[1]={& _tmp20B};Cyc_Std_aprintf(_tag_arr("declaration of tunion %s discloses too much information",
sizeof(unsigned char),56),_tag_arr(_tmp20A,sizeof(void*),1));}}));return 0;}
return 1;}static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct
Cyc_Absyn_Enumdecl*d1,struct _tagged_arr*msg){struct Cyc_Absyn_Enumdecl*_tmp20C=
Cyc_Tcdecl_merge_enumdecl(d0,d1,0,msg);if(_tmp20C == 0){return 0;}if((struct Cyc_Absyn_Enumdecl*)
d0 != _tmp20C){Cyc_Interface_check_err(msg,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp20E;_tmp20E.tag=Cyc_Std_String_pa;_tmp20E.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
d1->name);{void*_tmp20D[1]={& _tmp20E};Cyc_Std_aprintf(_tag_arr("declaration of enum %s discloses too much information",
sizeof(unsigned char),54),_tag_arr(_tmp20D,sizeof(void*),1));}}));return 0;}
return 1;}static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,struct
Cyc_Absyn_Vardecl*d1,struct _tagged_arr*msg){struct Cyc_Absyn_Vardecl*_tmp20F=Cyc_Tcdecl_merge_vardecl(
d0,d1,0,msg);if(_tmp20F == 0){return 0;}if((struct Cyc_Absyn_Vardecl*)d0 != _tmp20F){
Cyc_Interface_check_err(msg,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp211;_tmp211.tag=Cyc_Std_String_pa;_tmp211.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
d1->name);{void*_tmp210[1]={& _tmp211};Cyc_Std_aprintf(_tag_arr("declaration of variable %s discloses too much information",
sizeof(unsigned char),58),_tag_arr(_tmp210,sizeof(void*),1));}}));return 0;}
return 1;}static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,
struct Cyc_Absyn_Typedefdecl*d1,struct _tagged_arr*msg){struct Cyc_Absyn_Typedefdecl*
_tmp212=Cyc_Tcdecl_merge_typedefdecl(d0,d1,0,msg);if(_tmp212 == 0){return 0;}if((
struct Cyc_Absyn_Typedefdecl*)d0 != _tmp212){Cyc_Interface_check_err(msg,(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp214;_tmp214.tag=Cyc_Std_String_pa;
_tmp214.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(d1->name);{void*_tmp213[1]={&
_tmp214};Cyc_Std_aprintf(_tag_arr("declaration of typedef %s discloses too much information",
sizeof(unsigned char),57),_tag_arr(_tmp213,sizeof(void*),1));}}));return 0;}
return 1;}static int Cyc_Interface_incl_xtunionfielddecl(struct Cyc_Tcdecl_Xtunionfielddecl*
d0,struct Cyc_Tcdecl_Xtunionfielddecl*d1,struct _tagged_arr*msg){struct Cyc_Tcdecl_Xtunionfielddecl*
_tmp215=Cyc_Tcdecl_merge_xtunionfielddecl(d0,d1,0,msg);if(_tmp215 == 0){return 0;}
if((struct Cyc_Tcdecl_Xtunionfielddecl*)d0 != _tmp215){Cyc_Interface_check_err(msg,(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp217;_tmp217.tag=Cyc_Std_String_pa;
_tmp217.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((d1->field)->name);{void*
_tmp216[1]={& _tmp217};Cyc_Std_aprintf(_tag_arr("declaration of xtunionfield %s discloses too much information",
sizeof(unsigned char),62),_tag_arr(_tmp216,sizeof(void*),1));}}));return 0;}
return 1;}struct Cyc_Core_Opt*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,void*(*merge_f)(void*,
void*,struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg);static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*ie1,
struct Cyc_Interface_Ienv*ie2,struct _tagged_arr*msg){int _tmp218=((int(*)(struct
Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_incl_dict)(ie1->structdecls,ie2->structdecls,Cyc_Interface_incl_structdecl,
_tag_arr("struct",sizeof(unsigned char),7),msg);int _tmp219=((int(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Absyn_Uniondecl*,struct Cyc_Absyn_Uniondecl*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_incl_dict)(
ie1->uniondecls,ie2->uniondecls,Cyc_Interface_incl_uniondecl,_tag_arr("union",
sizeof(unsigned char),6),msg);int _tmp21A=((int(*)(struct Cyc_Dict_Dict*dic1,
struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Absyn_Tuniondecl*,struct Cyc_Absyn_Tuniondecl*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_incl_dict)(
ie1->tuniondecls,ie2->tuniondecls,Cyc_Interface_incl_tuniondecl,_tag_arr("tunion",
sizeof(unsigned char),7),msg);int _tmp21B=((int(*)(struct Cyc_Dict_Dict*dic1,
struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_incl_dict)(
ie1->enumdecls,ie2->enumdecls,Cyc_Interface_incl_enumdecl,_tag_arr("enum",
sizeof(unsigned char),5),msg);int _tmp21C=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Typedefdecl*(*
merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,
_tag_arr("typedef",sizeof(unsigned char),8),msg)!= 0;int _tmp21D=((int(*)(struct
Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_incl_dict)(ie1->vardecls,ie2->vardecls,Cyc_Interface_incl_vardecl,
_tag_arr("variable",sizeof(unsigned char),9),msg);int _tmp21E=((int(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Tcdecl_Xtunionfielddecl*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_incl_dict)(ie1->xtunionfielddecls,ie2->xtunionfielddecls,
Cyc_Interface_incl_xtunionfielddecl,_tag_arr("xtunionfield",sizeof(unsigned char),
13),msg);return(((((_tmp218? _tmp219: 0)? _tmp21A: 0)? _tmp21B: 0)? _tmp21C: 0)? _tmp21D:
0)? _tmp21E: 0;}struct _tuple13{int f1;struct Cyc_Dict_Dict*f2;struct _tagged_arr*f3;}
;static void Cyc_Interface_incl_ns_f(struct _tuple13*env,struct Cyc_List_List*ns,
struct Cyc_Interface_Ienv*ie1){struct _tuple13 _tmp221;struct _tagged_arr*_tmp222;
struct Cyc_Dict_Dict*_tmp224;int _tmp226;int*_tmp228;struct _tuple13*_tmp21F=env;
_tmp221=*_tmp21F;_LL551: _tmp226=_tmp221.f1;_tmp228=(int*)&(*_tmp21F).f1;goto
_LL549;_LL549: _tmp224=_tmp221.f2;goto _LL547;_LL547: _tmp222=_tmp221.f3;goto _LL544;
_LL544: {struct Cyc_Interface_Ienv*ie2;{struct _handler_cons _tmp229;_push_handler(&
_tmp229);{int _tmp22B=0;if(setjmp(_tmp229.handler)){_tmp22B=1;}if(! _tmp22B){ie2=((
struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
_tmp224,ns);;_pop_handler();}else{void*_tmp22A=(void*)_exn_thrown;void*_tmp22D=
_tmp22A;_LL559: if(_tmp22D == Cyc_Dict_Absent){goto _LL560;}else{goto _LL561;}_LL561:
goto _LL562;_LL560: ie2=Cyc_Interface_lazy_new_ienv();goto _LL558;_LL562:(void)
_throw(_tmp22D);_LL558:;}}}if(! Cyc_Interface_incl_ienv(ie1,ie2,_tmp222)){*
_tmp228=0;}}}static int Cyc_Interface_incl_ns(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,struct _tagged_arr*msg){struct _tuple13 _tmp233=({struct _tuple13 _tmp234;
_tmp234.f1=1;_tmp234.f2=dic2;_tmp234.f3=msg;_tmp234;});((void(*)(void(*f)(struct
_tuple13*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct _tuple13*env,
struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_incl_ns_f,& _tmp233,dic1);
return _tmp233.f1;}int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,
struct Cyc_Interface_I*i2,struct _tuple3*info){struct _handler_cons _tmp235;
_push_handler(& _tmp235);{int _tmp237=0;if(setjmp(_tmp235.handler)){_tmp237=1;}if(
! _tmp237){{int _tmp238=1;struct _tagged_arr*msg=0;if(info != 0){msg=({struct
_tagged_arr*_tmp239=_cycalloc(sizeof(struct _tagged_arr));_tmp239[0]=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp23C;_tmp23C.tag=Cyc_Std_String_pa;
_tmp23C.f1=(struct _tagged_arr)(*info).f2;{struct Cyc_Std_String_pa_struct _tmp23B;
_tmp23B.tag=Cyc_Std_String_pa;_tmp23B.f1=(struct _tagged_arr)(*info).f1;{void*
_tmp23A[2]={& _tmp23B,& _tmp23C};Cyc_Std_aprintf(_tag_arr("checking inclusion of %s exports into %s exports,",
sizeof(unsigned char),50),_tag_arr(_tmp23A,sizeof(void*),2));}}});_tmp239;});}
if(! Cyc_Interface_incl_ns(i1->exports,i2->exports,msg)){_tmp238=0;}if(info != 0){
msg=({struct _tagged_arr*_tmp23D=_cycalloc(sizeof(struct _tagged_arr));_tmp23D[0]=(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp240;_tmp240.tag=Cyc_Std_String_pa;
_tmp240.f1=(struct _tagged_arr)(*info).f1;{struct Cyc_Std_String_pa_struct _tmp23F;
_tmp23F.tag=Cyc_Std_String_pa;_tmp23F.f1=(struct _tagged_arr)(*info).f2;{void*
_tmp23E[2]={& _tmp23F,& _tmp240};Cyc_Std_aprintf(_tag_arr("checking inclusion of %s imports into %s imports,",
sizeof(unsigned char),50),_tag_arr(_tmp23E,sizeof(void*),2));}}});_tmp23D;});}
if(! Cyc_Interface_incl_ns(i2->imports,i1->imports,msg)){_tmp238=0;}{int _tmp241=
_tmp238;_npop_handler(0);return _tmp241;}};_pop_handler();}else{void*_tmp236=(
void*)_exn_thrown;void*_tmp243=_tmp236;_LL581: if(_tmp243 == Cyc_Tcdecl_Incompatible){
goto _LL582;}else{goto _LL583;}_LL583: goto _LL584;_LL582: return 0;_LL584:(void)
_throw(_tmp243);_LL580:;}}}struct _tuple14{int f1;struct Cyc_Dict_Dict*f2;struct Cyc_Dict_Dict*
f3;struct Cyc_Dict_Dict*f4;void*(*f5)(void*,void*,struct Cyc_Position_Segment*,
struct _tagged_arr*);struct _tagged_arr f6;struct _tagged_arr*f7;};void Cyc_Interface_compat_merge_dict_f(
struct _tuple14*env,struct _tagged_arr*x,void*y2){struct _tuple14 _tmp24B;struct
_tagged_arr*_tmp24C;struct _tagged_arr _tmp24E;void*(*_tmp250)(void*,void*,struct
Cyc_Position_Segment*,struct _tagged_arr*);struct Cyc_Dict_Dict*_tmp252;struct Cyc_Dict_Dict*
_tmp254;struct Cyc_Dict_Dict*_tmp256;struct Cyc_Dict_Dict**_tmp258;int _tmp259;int*
_tmp25B;struct _tuple14*_tmp249=env;_tmp24B=*_tmp249;_LL602: _tmp259=_tmp24B.f1;
_tmp25B=(int*)&(*_tmp249).f1;goto _LL599;_LL599: _tmp256=_tmp24B.f2;_tmp258=(
struct Cyc_Dict_Dict**)&(*_tmp249).f2;goto _LL597;_LL597: _tmp254=_tmp24B.f3;goto
_LL595;_LL595: _tmp252=_tmp24B.f4;goto _LL593;_LL593: _tmp250=_tmp24B.f5;goto _LL591;
_LL591: _tmp24E=_tmp24B.f6;goto _LL589;_LL589: _tmp24C=_tmp24B.f7;goto _LL586;_LL586: {
void*y;{struct _handler_cons _tmp25C;_push_handler(& _tmp25C);{int _tmp25E=0;if(
setjmp(_tmp25C.handler)){_tmp25E=1;}if(! _tmp25E){{void*_tmp25F=((void*(*)(struct
Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp254,x);void*_tmp260=
_tmp250(_tmp25F,y2,0,_tmp24C);if(!((unsigned int)_tmp260)){*_tmp25B=0;
_npop_handler(0);return;}y=(void*)_check_null(_tmp260);};_pop_handler();}else{
void*_tmp25D=(void*)_exn_thrown;void*_tmp262=_tmp25D;_LL612: if(_tmp262 == Cyc_Dict_Absent){
goto _LL613;}else{goto _LL614;}_LL614: goto _LL615;_LL613: y=y2;goto _LL611;_LL615:(
void)_throw(_tmp262);_LL611:;}}}{struct _handler_cons _tmp268;_push_handler(&
_tmp268);{int _tmp26A=0;if(setjmp(_tmp268.handler)){_tmp26A=1;}if(! _tmp26A){{void*
_tmp26B=((void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp252,x);void*_tmp26C=_tmp250(_tmp26B,y,0,_tmp24C);if(_tmp26C != (void*)_tmp26B){
if((unsigned int)_tmp26C){Cyc_Interface_check_err(_tmp24C,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp26F;_tmp26F.tag=Cyc_Std_String_pa;_tmp26F.f1=(
struct _tagged_arr)*x;{struct Cyc_Std_String_pa_struct _tmp26E;_tmp26E.tag=Cyc_Std_String_pa;
_tmp26E.f1=(struct _tagged_arr)_tmp24E;{void*_tmp26D[2]={& _tmp26E,& _tmp26F};Cyc_Std_aprintf(
_tag_arr("abstract %s %s is being imported as non-abstract",sizeof(unsigned char),
49),_tag_arr(_tmp26D,sizeof(void*),2));}}}));}*_tmp25B=0;}};_pop_handler();}
else{void*_tmp269=(void*)_exn_thrown;void*_tmp271=_tmp269;_LL627: if(_tmp271 == 
Cyc_Dict_Absent){goto _LL628;}else{goto _LL629;}_LL629: goto _LL630;_LL628: if(*
_tmp25B){*_tmp258=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,void*v))Cyc_Dict_insert)(*_tmp258,x,y);}goto _LL626;_LL630:(void)
_throw(_tmp271);_LL626:;}}}}}struct Cyc_Core_Opt*Cyc_Interface_compat_merge_dict(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,void*(*
merge_f)(void*,void*,struct Cyc_Position_Segment*,struct _tagged_arr*),struct
_tagged_arr t,struct _tagged_arr*msg){struct _tuple14 _tmp277=({struct _tuple14
_tmp279;_tmp279.f1=1;_tmp279.f2=dic1;_tmp279.f3=dic1;_tmp279.f4=excl;_tmp279.f5=
merge_f;_tmp279.f6=t;_tmp279.f7=msg;_tmp279;});((void(*)(void(*f)(struct _tuple14*,
struct _tagged_arr*,void*),struct _tuple14*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_compat_merge_dict_f,& _tmp277,dic2);if(_tmp277.f1){return({struct
Cyc_Core_Opt*_tmp278=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp278->v=_tmp277.f2;
_tmp278;});}else{return 0;}}struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(
struct Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*
iexcl,struct _tagged_arr*msg){struct Cyc_Core_Opt*_tmp27A=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,
struct Cyc_Absyn_Structdecl*(*merge_f)(struct Cyc_Absyn_Structdecl*,struct Cyc_Absyn_Structdecl*,
struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->structdecls,ie2->structdecls,
iexcl->structdecls,Cyc_Tcdecl_merge_structdecl,_tag_arr("struct",sizeof(
unsigned char),7),msg);struct Cyc_Core_Opt*_tmp27B=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,
struct Cyc_Absyn_Uniondecl*(*merge_f)(struct Cyc_Absyn_Uniondecl*,struct Cyc_Absyn_Uniondecl*,
struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->uniondecls,ie2->uniondecls,
iexcl->uniondecls,Cyc_Tcdecl_merge_uniondecl,_tag_arr("union",sizeof(
unsigned char),6),msg);struct Cyc_Core_Opt*_tmp27C=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,
struct Cyc_Absyn_Tuniondecl*(*merge_f)(struct Cyc_Absyn_Tuniondecl*,struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->tuniondecls,ie2->tuniondecls,
iexcl->tuniondecls,Cyc_Tcdecl_merge_tuniondecl,_tag_arr("tunion",sizeof(
unsigned char),7),msg);struct Cyc_Core_Opt*_tmp27D=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,
struct Cyc_Absyn_Enumdecl*(*merge_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,
struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->enumdecls,ie2->enumdecls,
iexcl->enumdecls,Cyc_Tcdecl_merge_enumdecl,_tag_arr("enum",sizeof(unsigned char),
5),msg);struct Cyc_Core_Opt*_tmp27E=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Typedefdecl*(*
merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,iexcl->typedefdecls,Cyc_Tcdecl_merge_typedefdecl,
_tag_arr("typedef",sizeof(unsigned char),8),msg);struct Cyc_Core_Opt*_tmp27F=((
struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct
Cyc_Dict_Dict*excl,struct Cyc_Absyn_Vardecl*(*merge_f)(struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*,struct _tagged_arr*),struct
_tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->vardecls,
ie2->vardecls,iexcl->vardecls,Cyc_Tcdecl_merge_vardecl,_tag_arr("variable",
sizeof(unsigned char),9),msg);struct Cyc_Core_Opt*_tmp280=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,
struct Cyc_Tcdecl_Xtunionfielddecl*(*merge_f)(struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Tcdecl_Xtunionfielddecl*,struct Cyc_Position_Segment*,struct _tagged_arr*),
struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->xtunionfielddecls,
ie2->xtunionfielddecls,iexcl->xtunionfielddecls,Cyc_Tcdecl_merge_xtunionfielddecl,
_tag_arr("xtunionfield",sizeof(unsigned char),13),msg);if((((((!((unsigned int)
_tmp27A)? 1: !((unsigned int)_tmp27B))? 1: !((unsigned int)_tmp27C))? 1: !((
unsigned int)_tmp27D))? 1: !((unsigned int)_tmp27E))? 1: !((unsigned int)_tmp27F))? 1:
!((unsigned int)_tmp280)){return 0;}else{return({struct Cyc_Interface_Ienv*_tmp281=
_cycalloc(sizeof(struct Cyc_Interface_Ienv));_tmp281->structdecls=(struct Cyc_Dict_Dict*)
_tmp27A->v;_tmp281->uniondecls=(struct Cyc_Dict_Dict*)_tmp27B->v;_tmp281->tuniondecls=(
struct Cyc_Dict_Dict*)_tmp27C->v;_tmp281->enumdecls=(struct Cyc_Dict_Dict*)_tmp27D->v;
_tmp281->typedefdecls=(struct Cyc_Dict_Dict*)_tmp27E->v;_tmp281->vardecls=(struct
Cyc_Dict_Dict*)_tmp27F->v;_tmp281->xtunionfielddecls=(struct Cyc_Dict_Dict*)
_tmp280->v;_tmp281;});}}struct _tuple15{int f1;struct Cyc_Dict_Dict*f2;struct Cyc_Dict_Dict*
f3;struct Cyc_Dict_Dict*f4;struct _tagged_arr*f5;};void Cyc_Interface_compat_merge_ns_f(
struct _tuple15*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){struct
_tuple15 _tmp284;struct _tagged_arr*_tmp285;struct Cyc_Dict_Dict*_tmp287;struct Cyc_Dict_Dict*
_tmp289;struct Cyc_Dict_Dict*_tmp28B;struct Cyc_Dict_Dict**_tmp28D;int _tmp28E;int*
_tmp290;struct _tuple15*_tmp282=env;_tmp284=*_tmp282;_LL655: _tmp28E=_tmp284.f1;
_tmp290=(int*)&(*_tmp282).f1;goto _LL652;_LL652: _tmp28B=_tmp284.f2;_tmp28D=(
struct Cyc_Dict_Dict**)&(*_tmp282).f2;goto _LL650;_LL650: _tmp289=_tmp284.f3;goto
_LL648;_LL648: _tmp287=_tmp284.f4;goto _LL646;_LL646: _tmp285=_tmp284.f5;goto _LL643;
_LL643: {struct Cyc_Interface_Ienv*ie1;struct Cyc_Interface_Ienv*iexcl;{struct
_handler_cons _tmp291;_push_handler(& _tmp291);{int _tmp293=0;if(setjmp(_tmp291.handler)){
_tmp293=1;}if(! _tmp293){iexcl=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp287,ns);;_pop_handler();}else{void*
_tmp292=(void*)_exn_thrown;void*_tmp295=_tmp292;_LL663: if(_tmp295 == Cyc_Dict_Absent){
goto _LL664;}else{goto _LL665;}_LL665: goto _LL666;_LL664: iexcl=Cyc_Interface_lazy_new_ienv();
goto _LL662;_LL666:(void)_throw(_tmp295);_LL662:;}}}{struct _handler_cons _tmp29B;
_push_handler(& _tmp29B);{int _tmp29D=0;if(setjmp(_tmp29B.handler)){_tmp29D=1;}if(
! _tmp29D){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmp289,ns);{struct Cyc_Interface_Ienv*_tmp29E=Cyc_Interface_compat_merge_ienv(
Cyc_Interface_lazy_new_ienv(),ie1,iexcl,_tmp285);if(_tmp29E == 0){*_tmp290=0;
_npop_handler(0);return;}ie1=(struct Cyc_Interface_Ienv*)_check_null(_tmp29E);};
_pop_handler();}else{void*_tmp29C=(void*)_exn_thrown;void*_tmp2A0=_tmp29C;_LL674:
if(_tmp2A0 == Cyc_Dict_Absent){goto _LL675;}else{goto _LL676;}_LL676: goto _LL677;
_LL675: ie1=Cyc_Interface_lazy_new_ienv();goto _LL673;_LL677:(void)_throw(_tmp2A0);
_LL673:;}}}{struct Cyc_Interface_Ienv*_tmp2A6=Cyc_Interface_compat_merge_ienv(ie1,
ie2,iexcl,_tmp285);if(_tmp2A6 == 0){*_tmp290=0;}else{if(*_tmp290){*_tmp28D=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*
v))Cyc_Dict_insert)(*_tmp28D,ns,(struct Cyc_Interface_Ienv*)_check_null(_tmp2A6));}}}}}
struct Cyc_Core_Opt*Cyc_Interface_compat_merge_ns(struct Cyc_Dict_Dict*dic1,struct
Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct _tagged_arr*msg){struct Cyc_Dict_Dict*
d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))
Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);struct _tuple15 _tmp2A7=({struct _tuple15
_tmp2A9;_tmp2A9.f1=1;_tmp2A9.f2=d;_tmp2A9.f3=dic1;_tmp2A9.f4=excl;_tmp2A9.f5=msg;
_tmp2A9;});((void(*)(void(*f)(struct _tuple15*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct _tuple15*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_ns_f,&
_tmp2A7,dic2);if(_tmp2A7.f1){return({struct Cyc_Core_Opt*_tmp2A8=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp2A8->v=_tmp2A7.f2;_tmp2A8;});}else{return 0;}}
struct _tuple16{int f1;struct Cyc_Dict_Dict*f2;struct Cyc_Dict_Dict*f3;struct
_tagged_arr f4;struct _tagged_arr*f5;};void Cyc_Interface_disj_merge_dict_f(struct
_tuple16*env,struct _tagged_arr*x,void*y){struct _tuple16 _tmp2AC;struct _tagged_arr*
_tmp2AD;struct _tagged_arr _tmp2AF;struct Cyc_Dict_Dict*_tmp2B1;struct Cyc_Dict_Dict*
_tmp2B3;struct Cyc_Dict_Dict**_tmp2B5;int _tmp2B6;int*_tmp2B8;struct _tuple16*
_tmp2AA=env;_tmp2AC=*_tmp2AA;_LL695: _tmp2B6=_tmp2AC.f1;_tmp2B8=(int*)&(*_tmp2AA).f1;
goto _LL692;_LL692: _tmp2B3=_tmp2AC.f2;_tmp2B5=(struct Cyc_Dict_Dict**)&(*_tmp2AA).f2;
goto _LL690;_LL690: _tmp2B1=_tmp2AC.f3;goto _LL688;_LL688: _tmp2AF=_tmp2AC.f4;goto
_LL686;_LL686: _tmp2AD=_tmp2AC.f5;goto _LL683;_LL683: if(((int(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_member)(_tmp2B1,x)){Cyc_Interface_check_err(
_tmp2AD,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp2BB;_tmp2BB.tag=
Cyc_Std_String_pa;_tmp2BB.f1=(struct _tagged_arr)*x;{struct Cyc_Std_String_pa_struct
_tmp2BA;_tmp2BA.tag=Cyc_Std_String_pa;_tmp2BA.f1=(struct _tagged_arr)_tmp2AF;{
void*_tmp2B9[2]={& _tmp2BA,& _tmp2BB};Cyc_Std_aprintf(_tag_arr("%s %s is exported more than once",
sizeof(unsigned char),33),_tag_arr(_tmp2B9,sizeof(void*),2));}}}));*_tmp2B8=0;}
else{if(*_tmp2B8){*_tmp2B5=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,void*v))Cyc_Dict_insert)(*_tmp2B5,x,y);}}}struct Cyc_Core_Opt*
Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,
struct _tagged_arr t,struct _tagged_arr*msg){struct _tuple16 _tmp2BC=({struct _tuple16
_tmp2BE;_tmp2BE.f1=1;_tmp2BE.f2=dic1;_tmp2BE.f3=dic1;_tmp2BE.f4=t;_tmp2BE.f5=msg;
_tmp2BE;});((void(*)(void(*f)(struct _tuple16*,struct _tagged_arr*,void*),struct
_tuple16*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_dict_f,&
_tmp2BC,dic2);if(_tmp2BC.f1){return({struct Cyc_Core_Opt*_tmp2BD=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp2BD->v=_tmp2BC.f2;_tmp2BD;});}else{return 0;}}
struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*
ie1,struct Cyc_Interface_Ienv*ie2,struct _tagged_arr*msg){struct Cyc_Core_Opt*
_tmp2BF=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Structdecl*(*merge_f)(struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*,struct Cyc_Position_Segment*,struct _tagged_arr*),
struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->structdecls,
ie2->structdecls,((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp),Cyc_Tcdecl_merge_structdecl,
_tag_arr("struct",sizeof(unsigned char),7),msg);struct Cyc_Core_Opt*_tmp2C0=((
struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct
_tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_disj_merge_dict)(ie1->uniondecls,
ie2->uniondecls,_tag_arr("union",sizeof(unsigned char),6),msg);struct Cyc_Core_Opt*
_tmp2C1=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_disj_merge_dict)(
ie1->tuniondecls,ie2->tuniondecls,_tag_arr("[x]tunion",sizeof(unsigned char),10),
msg);struct Cyc_Core_Opt*_tmp2C2=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_disj_merge_dict)(
ie1->enumdecls,ie2->enumdecls,_tag_arr("enum",sizeof(unsigned char),5),msg);
struct Cyc_Core_Opt*_tmp2C3=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,
struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Typedefdecl*(*
merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp),Cyc_Tcdecl_merge_typedefdecl,
_tag_arr("typedef",sizeof(unsigned char),8),msg);struct Cyc_Core_Opt*_tmp2C4=((
struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct
_tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_disj_merge_dict)(ie1->vardecls,
ie2->vardecls,_tag_arr("variable",sizeof(unsigned char),9),msg);struct Cyc_Core_Opt*
_tmp2C5=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_disj_merge_dict)(
ie1->xtunionfielddecls,ie2->xtunionfielddecls,_tag_arr("xtunionfield",sizeof(
unsigned char),13),msg);if((((((!((unsigned int)_tmp2BF)? 1: !((unsigned int)
_tmp2C0))? 1: !((unsigned int)_tmp2C1))? 1: !((unsigned int)_tmp2C2))? 1: !((
unsigned int)_tmp2C3))? 1: !((unsigned int)_tmp2C4))? 1: !((unsigned int)_tmp2C5)){
return 0;}else{return({struct Cyc_Interface_Ienv*_tmp2C6=_cycalloc(sizeof(struct
Cyc_Interface_Ienv));_tmp2C6->structdecls=(struct Cyc_Dict_Dict*)_tmp2BF->v;
_tmp2C6->uniondecls=(struct Cyc_Dict_Dict*)_tmp2C0->v;_tmp2C6->tuniondecls=(
struct Cyc_Dict_Dict*)_tmp2C1->v;_tmp2C6->enumdecls=(struct Cyc_Dict_Dict*)_tmp2C2->v;
_tmp2C6->typedefdecls=(struct Cyc_Dict_Dict*)_tmp2C3->v;_tmp2C6->vardecls=(struct
Cyc_Dict_Dict*)_tmp2C4->v;_tmp2C6->xtunionfielddecls=(struct Cyc_Dict_Dict*)
_tmp2C5->v;_tmp2C6;});}}struct _tuple17{int f1;struct Cyc_Dict_Dict*f2;struct Cyc_Dict_Dict*
f3;struct _tagged_arr*f4;};void Cyc_Interface_disj_merge_ns_f(struct _tuple17*env,
struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){struct _tuple17 _tmp2C9;
struct _tagged_arr*_tmp2CA;struct Cyc_Dict_Dict*_tmp2CC;struct Cyc_Dict_Dict*
_tmp2CE;struct Cyc_Dict_Dict**_tmp2D0;int _tmp2D1;int*_tmp2D3;struct _tuple17*
_tmp2C7=env;_tmp2C9=*_tmp2C7;_LL722: _tmp2D1=_tmp2C9.f1;_tmp2D3=(int*)&(*_tmp2C7).f1;
goto _LL719;_LL719: _tmp2CE=_tmp2C9.f2;_tmp2D0=(struct Cyc_Dict_Dict**)&(*_tmp2C7).f2;
goto _LL717;_LL717: _tmp2CC=_tmp2C9.f3;goto _LL715;_LL715: _tmp2CA=_tmp2C9.f4;goto
_LL712;_LL712: {struct Cyc_Interface_Ienv*ie1;{struct _handler_cons _tmp2D4;
_push_handler(& _tmp2D4);{int _tmp2D6=0;if(setjmp(_tmp2D4.handler)){_tmp2D6=1;}if(
! _tmp2D6){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmp2CC,ns);;_pop_handler();}else{void*_tmp2D5=(void*)
_exn_thrown;void*_tmp2D8=_tmp2D5;_LL730: if(_tmp2D8 == Cyc_Dict_Absent){goto _LL731;}
else{goto _LL732;}_LL732: goto _LL733;_LL731: ie1=Cyc_Interface_lazy_new_ienv();goto
_LL729;_LL733:(void)_throw(_tmp2D8);_LL729:;}}}{struct Cyc_Interface_Ienv*_tmp2DE=
Cyc_Interface_disj_merge_ienv(ie1,ie2,_tmp2CA);if(_tmp2DE == 0){*_tmp2D3=0;}else{
if(*_tmp2D3){*_tmp2D0=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*
k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*_tmp2D0,ns,(struct Cyc_Interface_Ienv*)
_check_null(_tmp2DE));}}}}}struct Cyc_Core_Opt*Cyc_Interface_disj_merge_ns(struct
Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr*msg){struct
_tuple17 _tmp2DF=({struct _tuple17 _tmp2E1;_tmp2E1.f1=1;_tmp2E1.f2=dic1;_tmp2E1.f3=
dic1;_tmp2E1.f4=msg;_tmp2E1;});((void(*)(void(*f)(struct _tuple17*,struct Cyc_List_List*,
struct Cyc_Interface_Ienv*),struct _tuple17*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_disj_merge_ns_f,& _tmp2DF,dic2);if(_tmp2DF.f1){return({struct Cyc_Core_Opt*
_tmp2E0=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp2E0->v=_tmp2DF.f2;_tmp2E0;});}
else{return 0;}}struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*
i1,struct Cyc_Interface_I*i2,struct _tuple3*info){struct _handler_cons _tmp2E2;
_push_handler(& _tmp2E2);{int _tmp2E4=0;if(setjmp(_tmp2E2.handler)){_tmp2E4=1;}if(
! _tmp2E4){{struct _tagged_arr*msg=0;if(info != 0){msg=({struct _tagged_arr*_tmp2E5=
_cycalloc(sizeof(struct _tagged_arr));_tmp2E5[0]=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp2E8;_tmp2E8.tag=Cyc_Std_String_pa;_tmp2E8.f1=(struct _tagged_arr)(*info).f2;{
struct Cyc_Std_String_pa_struct _tmp2E7;_tmp2E7.tag=Cyc_Std_String_pa;_tmp2E7.f1=(
struct _tagged_arr)(*info).f1;{void*_tmp2E6[2]={& _tmp2E7,& _tmp2E8};Cyc_Std_aprintf(
_tag_arr("merging exports of %s and %s,",sizeof(unsigned char),30),_tag_arr(
_tmp2E6,sizeof(void*),2));}}});_tmp2E5;});}{struct Cyc_Core_Opt*_tmp2E9=Cyc_Interface_disj_merge_ns(
i1->exports,i2->exports,msg);if(_tmp2E9 == 0){struct Cyc_Interface_I*_tmp2EA=0;
_npop_handler(0);return _tmp2EA;}if(info != 0){msg=({struct _tagged_arr*_tmp2EB=
_cycalloc(sizeof(struct _tagged_arr));_tmp2EB[0]=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp2EE;_tmp2EE.tag=Cyc_Std_String_pa;_tmp2EE.f1=(struct _tagged_arr)(*info).f2;{
struct Cyc_Std_String_pa_struct _tmp2ED;_tmp2ED.tag=Cyc_Std_String_pa;_tmp2ED.f1=(
struct _tagged_arr)(*info).f1;{void*_tmp2EC[2]={& _tmp2ED,& _tmp2EE};Cyc_Std_aprintf(
_tag_arr("merging imports of %s and %s,",sizeof(unsigned char),30),_tag_arr(
_tmp2EC,sizeof(void*),2));}}});_tmp2EB;});}{struct Cyc_Core_Opt*_tmp2EF=Cyc_Interface_compat_merge_ns(
i1->imports,i2->imports,(struct Cyc_Dict_Dict*)_tmp2E9->v,msg);if(_tmp2EF == 0){
struct Cyc_Interface_I*_tmp2F0=0;_npop_handler(0);return _tmp2F0;}{struct Cyc_Interface_I*
_tmp2F2=({struct Cyc_Interface_I*_tmp2F1=_cycalloc(sizeof(struct Cyc_Interface_I));
_tmp2F1->imports=(struct Cyc_Dict_Dict*)_tmp2EF->v;_tmp2F1->exports=(struct Cyc_Dict_Dict*)
_tmp2E9->v;_tmp2F1;});_npop_handler(0);return _tmp2F2;}}}};_pop_handler();}else{
void*_tmp2E3=(void*)_exn_thrown;void*_tmp2F4=_tmp2E3;_LL758: if(_tmp2F4 == Cyc_Tcdecl_Incompatible){
goto _LL759;}else{goto _LL760;}_LL760: goto _LL761;_LL759: return 0;_LL761:(void)
_throw(_tmp2F4);_LL757:;}}}struct Cyc_Interface_I*Cyc_Interface_merge_list(struct
Cyc_List_List*li,struct Cyc_List_List*linfo){if(li == 0){return(struct Cyc_Interface_I*)
Cyc_Interface_empty();}{struct Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)li->hd;
struct _tagged_arr*curr_info=linfo != 0?(struct _tagged_arr*)((struct _tagged_arr*)
linfo->hd): 0;li=li->tl;if(linfo != 0){linfo=linfo->tl;}for(0;li != 0;li=li->tl){
struct Cyc_Interface_I*_tmp2FA=Cyc_Interface_merge(curr_i,(struct Cyc_Interface_I*)
li->hd,(curr_info != 0? linfo != 0: 0)?({struct _tuple3*_tmp2FF=_cycalloc(sizeof(
struct _tuple3));_tmp2FF->f1=*curr_info;_tmp2FF->f2=*((struct _tagged_arr*)linfo->hd);
_tmp2FF;}): 0);if(_tmp2FA == 0){return 0;}curr_i=(struct Cyc_Interface_I*)
_check_null(_tmp2FA);if(curr_info != 0){curr_info=linfo != 0?({struct _tagged_arr*
_tmp2FB=_cycalloc(sizeof(struct _tagged_arr));_tmp2FB[0]=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp2FE;_tmp2FE.tag=Cyc_Std_String_pa;_tmp2FE.f1=(
struct _tagged_arr)*((struct _tagged_arr*)linfo->hd);{struct Cyc_Std_String_pa_struct
_tmp2FD;_tmp2FD.tag=Cyc_Std_String_pa;_tmp2FD.f1=(struct _tagged_arr)*curr_info;{
void*_tmp2FC[2]={& _tmp2FD,& _tmp2FE};Cyc_Std_aprintf(_tag_arr("%s+%s",sizeof(
unsigned char),6),_tag_arr(_tmp2FC,sizeof(void*),2));}}});_tmp2FB;}): 0;}if(linfo
!= 0){linfo=linfo->tl;}}return(struct Cyc_Interface_I*)curr_i;}}struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo){if(la == 0){return(struct Cyc_Interface_I*)Cyc_Interface_empty();}{
struct Cyc_Interface_I*curr_i=get((void*)la->hd);struct _tagged_arr*curr_info=
linfo != 0?(struct _tagged_arr*)((struct _tagged_arr*)linfo->hd): 0;la=la->tl;if(
linfo != 0){linfo=linfo->tl;}for(0;la != 0;la=la->tl){struct Cyc_Interface_I*
_tmp300=Cyc_Interface_merge(curr_i,get((void*)la->hd),(curr_info != 0? linfo != 0: 0)?({
struct _tuple3*_tmp305=_cycalloc(sizeof(struct _tuple3));_tmp305->f1=*curr_info;
_tmp305->f2=*((struct _tagged_arr*)linfo->hd);_tmp305;}): 0);if(_tmp300 == 0){
return 0;}curr_i=(struct Cyc_Interface_I*)_check_null(_tmp300);if(curr_info != 0){
curr_info=linfo != 0?({struct _tagged_arr*_tmp301=_cycalloc(sizeof(struct
_tagged_arr));_tmp301[0]=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp304;_tmp304.tag=Cyc_Std_String_pa;_tmp304.f1=(struct _tagged_arr)*((struct
_tagged_arr*)linfo->hd);{struct Cyc_Std_String_pa_struct _tmp303;_tmp303.tag=Cyc_Std_String_pa;
_tmp303.f1=(struct _tagged_arr)*curr_info;{void*_tmp302[2]={& _tmp303,& _tmp304};
Cyc_Std_aprintf(_tag_arr("%s+%s",sizeof(unsigned char),6),_tag_arr(_tmp302,
sizeof(void*),2));}}});_tmp301;}): 0;}if(linfo != 0){linfo=linfo->tl;}}return(
struct Cyc_Interface_I*)curr_i;}}static struct Cyc_List_List*Cyc_Interface_add_namespace(
struct Cyc_List_List*tds,struct Cyc_List_List*ns){if(ns == 0){return tds;}return({
struct Cyc_List_List*_tmp306=_cycalloc(sizeof(struct Cyc_List_List));_tmp306->hd=
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp307=_cycalloc(
sizeof(struct Cyc_Absyn_Namespace_d_struct));_tmp307[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp308;_tmp308.tag=Cyc_Absyn_Namespace_d;_tmp308.f1=(struct _tagged_arr*)ns->hd;
_tmp308.f2=Cyc_Interface_add_namespace(tds,ns->tl);_tmp308;});_tmp307;}),0);
_tmp306->tl=0;_tmp306;});}static struct Cyc_List_List*Cyc_Interface_add_structdecl(
struct _tagged_arr*x,struct Cyc_Absyn_Structdecl*d,struct Cyc_List_List*tds){return({
struct Cyc_List_List*_tmp309=_cycalloc(sizeof(struct Cyc_List_List));_tmp309->hd=
Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Struct_d_struct*_tmp30A=_cycalloc(
sizeof(struct Cyc_Absyn_Struct_d_struct));_tmp30A[0]=({struct Cyc_Absyn_Struct_d_struct
_tmp30B;_tmp30B.tag=Cyc_Absyn_Struct_d;_tmp30B.f1=d;_tmp30B;});_tmp30A;}),0);
_tmp309->tl=tds;_tmp309;});}static struct Cyc_List_List*Cyc_Interface_add_structdecl_header(
struct _tagged_arr*x,struct Cyc_Absyn_Structdecl*d,struct Cyc_List_List*tds){d=({
struct Cyc_Absyn_Structdecl*_tmp30C=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));
_tmp30C[0]=*d;_tmp30C;});d->fields=0;if((void*)d->sc != (void*)Cyc_Absyn_ExternC){(
void*)(d->sc=(void*)((void*)Cyc_Absyn_Extern));}return({struct Cyc_List_List*
_tmp30D=_cycalloc(sizeof(struct Cyc_List_List));_tmp30D->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Struct_d_struct*_tmp30E=_cycalloc(sizeof(struct Cyc_Absyn_Struct_d_struct));
_tmp30E[0]=({struct Cyc_Absyn_Struct_d_struct _tmp30F;_tmp30F.tag=Cyc_Absyn_Struct_d;
_tmp30F.f1=d;_tmp30F;});_tmp30E;}),0);_tmp30D->tl=tds;_tmp30D;});}static struct
Cyc_List_List*Cyc_Interface_add_uniondecl(struct _tagged_arr*x,struct Cyc_Absyn_Uniondecl*
d,struct Cyc_List_List*tds){return({struct Cyc_List_List*_tmp310=_cycalloc(sizeof(
struct Cyc_List_List));_tmp310->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Union_d_struct*
_tmp311=_cycalloc(sizeof(struct Cyc_Absyn_Union_d_struct));_tmp311[0]=({struct Cyc_Absyn_Union_d_struct
_tmp312;_tmp312.tag=Cyc_Absyn_Union_d;_tmp312.f1=d;_tmp312;});_tmp311;}),0);
_tmp310->tl=tds;_tmp310;});}static struct Cyc_List_List*Cyc_Interface_add_uniondecl_header(
struct _tagged_arr*x,struct Cyc_Absyn_Uniondecl*d,struct Cyc_List_List*tds){d=({
struct Cyc_Absyn_Uniondecl*_tmp313=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl));
_tmp313[0]=*d;_tmp313;});d->fields=0;if((void*)d->sc != (void*)Cyc_Absyn_ExternC){(
void*)(d->sc=(void*)((void*)Cyc_Absyn_Extern));}return({struct Cyc_List_List*
_tmp314=_cycalloc(sizeof(struct Cyc_List_List));_tmp314->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Union_d_struct*_tmp315=_cycalloc(sizeof(struct Cyc_Absyn_Union_d_struct));
_tmp315[0]=({struct Cyc_Absyn_Union_d_struct _tmp316;_tmp316.tag=Cyc_Absyn_Union_d;
_tmp316.f1=d;_tmp316;});_tmp315;}),0);_tmp314->tl=tds;_tmp314;});}static struct
Cyc_List_List*Cyc_Interface_add_tuniondecl(struct _tagged_arr*x,struct Cyc_Absyn_Tuniondecl*
d,struct Cyc_List_List*tds){return({struct Cyc_List_List*_tmp317=_cycalloc(sizeof(
struct Cyc_List_List));_tmp317->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp318=_cycalloc(sizeof(struct Cyc_Absyn_Tunion_d_struct));_tmp318[0]=({struct
Cyc_Absyn_Tunion_d_struct _tmp319;_tmp319.tag=Cyc_Absyn_Tunion_d;_tmp319.f1=d;
_tmp319;});_tmp318;}),0);_tmp317->tl=tds;_tmp317;});}static unsigned char _tmp31A[
2]="_";static struct _tagged_arr Cyc_Interface_us={_tmp31A,_tmp31A,_tmp31A + 2};
static struct _tagged_arr*Cyc_Interface_us_p=& Cyc_Interface_us;static struct _tuple9*
Cyc_Interface_rewrite_tunionfield_type(struct _tuple9*x){return({struct _tuple9*
_tmp31B=_cycalloc(sizeof(struct _tuple9));_tmp31B->f1=Cyc_Absyn_empty_tqual();
_tmp31B->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmp31C=_cycalloc(sizeof(
struct Cyc_Absyn_VarType_struct));_tmp31C[0]=({struct Cyc_Absyn_VarType_struct
_tmp31D;_tmp31D.tag=Cyc_Absyn_VarType;_tmp31D.f1=({struct Cyc_Absyn_Tvar*_tmp31E=
_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_tmp31E->name=Cyc_Interface_us_p;_tmp31E->identity=
0;_tmp31E->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp31F=_cycalloc(
sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp31F[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp320;_tmp320.tag=Cyc_Absyn_Eq_kb;_tmp320.f1=(void*)((void*)Cyc_Absyn_BoxKind);
_tmp320;});_tmp31F;}));_tmp31E;});_tmp31D;});_tmp31C;});_tmp31B;});}static struct
Cyc_Absyn_Tunionfield*Cyc_Interface_rewrite_tunionfield(struct Cyc_Absyn_Tunionfield*
f){f=({struct Cyc_Absyn_Tunionfield*_tmp321=_cycalloc(sizeof(struct Cyc_Absyn_Tunionfield));
_tmp321[0]=*f;_tmp321;});f->typs=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(
struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_tunionfield_type,
f->typs);return f;}static struct Cyc_List_List*Cyc_Interface_add_tuniondecl_header(
struct _tagged_arr*x,struct Cyc_Absyn_Tuniondecl*d,struct Cyc_List_List*tds){d=({
struct Cyc_Absyn_Tuniondecl*_tmp322=_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));
_tmp322[0]=*d;_tmp322;});if(d->fields != 0){d->fields=({struct Cyc_Core_Opt*
_tmp323=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp323->v=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tunionfield*(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Interface_rewrite_tunionfield,(struct Cyc_List_List*)(d->fields)->v);
_tmp323;});}if((void*)d->sc != (void*)Cyc_Absyn_ExternC){(void*)(d->sc=(void*)((
void*)Cyc_Absyn_Extern));}return({struct Cyc_List_List*_tmp324=_cycalloc(sizeof(
struct Cyc_List_List));_tmp324->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp325=_cycalloc(sizeof(struct Cyc_Absyn_Tunion_d_struct));_tmp325[0]=({struct
Cyc_Absyn_Tunion_d_struct _tmp326;_tmp326.tag=Cyc_Absyn_Tunion_d;_tmp326.f1=d;
_tmp326;});_tmp325;}),0);_tmp324->tl=tds;_tmp324;});}static struct Cyc_List_List*
Cyc_Interface_add_enumdecl(struct _tagged_arr*x,struct Cyc_Absyn_Enumdecl*d,struct
Cyc_List_List*tds){return({struct Cyc_List_List*_tmp327=_cycalloc(sizeof(struct
Cyc_List_List));_tmp327->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp328=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_struct));_tmp328[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp329;_tmp329.tag=Cyc_Absyn_Enum_d;_tmp329.f1=d;_tmp329;});_tmp328;}),0);
_tmp327->tl=tds;_tmp327;});}static struct Cyc_List_List*Cyc_Interface_add_enumdecl_header(
struct _tagged_arr*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){d=({
struct Cyc_Absyn_Enumdecl*_tmp32A=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));
_tmp32A[0]=*d;_tmp32A;});d->fields=0;if((void*)d->sc != (void*)Cyc_Absyn_ExternC){(
void*)(d->sc=(void*)((void*)Cyc_Absyn_Extern));}return({struct Cyc_List_List*
_tmp32B=_cycalloc(sizeof(struct Cyc_List_List));_tmp32B->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Enum_d_struct*_tmp32C=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_struct));
_tmp32C[0]=({struct Cyc_Absyn_Enum_d_struct _tmp32D;_tmp32D.tag=Cyc_Absyn_Enum_d;
_tmp32D.f1=d;_tmp32D;});_tmp32C;}),0);_tmp32B->tl=tds;_tmp32B;});}static struct
Cyc_List_List*Cyc_Interface_add_typedef(struct _tagged_arr*x,struct Cyc_Absyn_Typedefdecl*
d,struct Cyc_List_List*tds){return({struct Cyc_List_List*_tmp32E=_cycalloc(sizeof(
struct Cyc_List_List));_tmp32E->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_struct*
_tmp32F=_cycalloc(sizeof(struct Cyc_Absyn_Typedef_d_struct));_tmp32F[0]=({struct
Cyc_Absyn_Typedef_d_struct _tmp330;_tmp330.tag=Cyc_Absyn_Typedef_d;_tmp330.f1=d;
_tmp330;});_tmp32F;}),0);_tmp32E->tl=tds;_tmp32E;});}static struct Cyc_List_List*
Cyc_Interface_add_vardecl(struct _tagged_arr*x,struct Cyc_Absyn_Vardecl*d,struct
Cyc_List_List*tds){return({struct Cyc_List_List*_tmp331=_cycalloc(sizeof(struct
Cyc_List_List));_tmp331->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp332=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_struct));_tmp332[0]=({struct Cyc_Absyn_Var_d_struct
_tmp333;_tmp333.tag=Cyc_Absyn_Var_d;_tmp333.f1=d;_tmp333;});_tmp332;}),0);
_tmp331->tl=tds;_tmp331;});}static struct Cyc_List_List*Cyc_Interface_add_xtunionfielddecl(
struct _tagged_arr*x,struct Cyc_Tcdecl_Xtunionfielddecl*d,struct Cyc_List_List*tds){
struct Cyc_Tcdecl_Xtunionfielddecl _tmp336;struct Cyc_Absyn_Tunionfield*_tmp337;
struct Cyc_Absyn_Tuniondecl*_tmp339;struct Cyc_Tcdecl_Xtunionfielddecl*_tmp334=d;
_tmp336=*_tmp334;_LL826: _tmp339=_tmp336.base;goto _LL824;_LL824: _tmp337=_tmp336.field;
goto _LL821;_LL821: _tmp339=({struct Cyc_Absyn_Tuniondecl*_tmp33B=_cycalloc(sizeof(
struct Cyc_Absyn_Tuniondecl));_tmp33B[0]=*_tmp339;_tmp33B;});_tmp339->fields=({
struct Cyc_Core_Opt*_tmp33C=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp33C->v=({
struct Cyc_List_List*_tmp33D=_cycalloc(sizeof(struct Cyc_List_List));_tmp33D->hd=
_tmp337;_tmp33D->tl=0;_tmp33D;});_tmp33C;});(void*)(_tmp339->sc=(void*)((void*)
Cyc_Absyn_Extern));return({struct Cyc_List_List*_tmp33E=_cycalloc(sizeof(struct
Cyc_List_List));_tmp33E->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp33F=_cycalloc(sizeof(struct Cyc_Absyn_Tunion_d_struct));_tmp33F[0]=({struct
Cyc_Absyn_Tunion_d_struct _tmp340;_tmp340.tag=Cyc_Absyn_Tunion_d;_tmp340.f1=
_tmp339;_tmp340;});_tmp33F;}),0);_tmp33E->tl=tds;_tmp33E;});}static struct Cyc_List_List*
Cyc_Interface_add_xtunionfielddecl_header(struct _tagged_arr*x,struct Cyc_Tcdecl_Xtunionfielddecl*
d,struct Cyc_List_List*tds){struct Cyc_Tcdecl_Xtunionfielddecl _tmp343;struct Cyc_Absyn_Tunionfield*
_tmp344;struct Cyc_Absyn_Tuniondecl*_tmp346;struct Cyc_Tcdecl_Xtunionfielddecl*
_tmp341=d;_tmp343=*_tmp341;_LL839: _tmp346=_tmp343.base;goto _LL837;_LL837: _tmp344=
_tmp343.field;goto _LL834;_LL834: _tmp346=({struct Cyc_Absyn_Tuniondecl*_tmp348=
_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));_tmp348[0]=*_tmp346;_tmp348;});
_tmp344=Cyc_Interface_rewrite_tunionfield(_tmp344);(void*)(_tmp344->sc=(void*)((
void*)Cyc_Absyn_Extern));_tmp346->fields=({struct Cyc_Core_Opt*_tmp349=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp349->v=({struct Cyc_List_List*_tmp34A=_cycalloc(
sizeof(struct Cyc_List_List));_tmp34A->hd=_tmp344;_tmp34A->tl=0;_tmp34A;});
_tmp349;});(void*)(_tmp346->sc=(void*)((void*)Cyc_Absyn_Extern));return({struct
Cyc_List_List*_tmp34B=_cycalloc(sizeof(struct Cyc_List_List));_tmp34B->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp34C=_cycalloc(sizeof(struct Cyc_Absyn_Tunion_d_struct));
_tmp34C[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp34D;_tmp34D.tag=Cyc_Absyn_Tunion_d;
_tmp34D.f1=_tmp346;_tmp34D;});_tmp34C;}),0);_tmp34B->tl=tds;_tmp34B;});}static
void Cyc_Interface_print_ns_headers(struct Cyc_Std___sFILE*f,struct Cyc_List_List*
ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*_tmp34E=0;_tmp34E=((struct
Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tagged_arr*,struct Cyc_Absyn_Structdecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_structdecl_header,ie->structdecls,_tmp34E);_tmp34E=((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _tagged_arr*,struct Cyc_Absyn_Uniondecl*,struct Cyc_List_List*),
struct Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_uniondecl_header,
ie->uniondecls,_tmp34E);_tmp34E=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _tagged_arr*,struct Cyc_Absyn_Tuniondecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls,_tmp34E);_tmp34E=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _tagged_arr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl_header,ie->enumdecls,
_tmp34E);if(_tmp34E != 0){_tmp34E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp34E);_tmp34E=Cyc_Interface_add_namespace(_tmp34E,ns);Cyc_Absynpp_decllist2file(
_tmp34E,f);}}static void Cyc_Interface_print_ns_xtunionfielddecl_headers(struct Cyc_Std___sFILE*
f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*
_tmp34F=0;_tmp34F=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Tcdecl_Xtunionfielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xtunionfielddecl_header,
ie->xtunionfielddecls,_tmp34F);if(_tmp34F != 0){_tmp34F=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp34F);_tmp34F=Cyc_Interface_add_namespace(
_tmp34F,ns);Cyc_Absynpp_decllist2file(_tmp34F,f);}}static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Std___sFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp350=0;_tmp350=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _tagged_arr*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_typedef,
ie->typedefdecls,_tmp350);if(_tmp350 != 0){_tmp350=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp350);_tmp350=Cyc_Interface_add_namespace(
_tmp350,ns);Cyc_Absynpp_decllist2file(_tmp350,f);}}static void Cyc_Interface_print_ns_decls(
struct Cyc_Std___sFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp351=0;_tmp351=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _tagged_arr*,struct Cyc_Absyn_Structdecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_structdecl,
ie->structdecls,_tmp351);_tmp351=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _tagged_arr*,struct Cyc_Absyn_Uniondecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_uniondecl,ie->uniondecls,
_tmp351);_tmp351=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Absyn_Tuniondecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_tuniondecl,ie->tuniondecls,
_tmp351);_tmp351=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl,ie->enumdecls,
_tmp351);_tmp351=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_vardecl,ie->vardecls,
_tmp351);_tmp351=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Tcdecl_Xtunionfielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls,_tmp351);if(_tmp351 != 0){_tmp351=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp351);_tmp351=Cyc_Interface_add_namespace(
_tmp351,ns);Cyc_Absynpp_decllist2file(_tmp351,f);}}void Cyc_Interface_print(
struct Cyc_Interface_I*i,struct Cyc_Std___sFILE*f){Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);({
void*_tmp352[0]={};Cyc_Std_fprintf(f,_tag_arr("/****** needed (headers) ******/\n",
sizeof(unsigned char),34),_tag_arr(_tmp352,sizeof(void*),0));});((void(*)(void(*
f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
Cyc_Std___sFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,
f,i->imports);({void*_tmp353[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** provided (headers) ******/\n",
sizeof(unsigned char),37),_tag_arr(_tmp353,sizeof(void*),0));});((void(*)(void(*
f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
Cyc_Std___sFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,
f,i->exports);({void*_tmp354[0]={};Cyc_Std_fprintf(f,_tag_arr("/****** needed (headers of xtunionfielddecls) ******/\n",
sizeof(unsigned char),55),_tag_arr(_tmp354,sizeof(void*),0));});((void(*)(void(*
f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
Cyc_Std___sFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xtunionfielddecl_headers,
f,i->imports);({void*_tmp355[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** provided (headers of xtunionfielddecls) ******/\n",
sizeof(unsigned char),58),_tag_arr(_tmp355,sizeof(void*),0));});((void(*)(void(*
f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
Cyc_Std___sFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xtunionfielddecl_headers,
f,i->exports);({void*_tmp356[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** provided (typedefs) ******/\n",
sizeof(unsigned char),38),_tag_arr(_tmp356,sizeof(void*),0));});((void(*)(void(*
f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
Cyc_Std___sFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_typedefs,
f,i->exports);({void*_tmp357[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** needed (declarations) ******/\n",
sizeof(unsigned char),40),_tag_arr(_tmp357,sizeof(void*),0));});((void(*)(void(*
f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
Cyc_Std___sFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,
f,i->imports);({void*_tmp358[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** provided (declarations) ******/\n",
sizeof(unsigned char),42),_tag_arr(_tmp358,sizeof(void*),0));});((void(*)(void(*
f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
Cyc_Std___sFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,
f,i->exports);}struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc_Std___sFILE*f){
Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp359=Cyc_Parse_parse_file(f);struct
Cyc_Tcenv_Tenv*_tmp35A=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp35A,0,_tmp359);return
Cyc_Interface_gen_extract(_tmp35A->ae,0);}}void Cyc_Interface_save(struct Cyc_Interface_I*
i,struct Cyc_Std___sFILE*f){Cyc_Interface_print(i,f);}struct Cyc_Interface_I*Cyc_Interface_load(
struct Cyc_Std___sFILE*f){return Cyc_Interface_parse(f);}
