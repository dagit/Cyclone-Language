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
struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern struct Cyc_Std___sFILE*Cyc_Std_stdout;
extern int Cyc_Std_fputc(int __c,struct Cyc_Std___sFILE*__stream);extern
unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
extern int Cyc_Std_file_string_write(struct Cyc_Std___sFILE*fd,struct _tagged_arr src,
int src_offset,int max_count);static const int Cyc_Std_String_pa=0;struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Std_Int_pa=1;struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};static const int Cyc_Std_Double_pa=2;struct Cyc_Std_Double_pa_struct{
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
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern int
Cyc_List_length(struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern unsigned char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;static const int Cyc_Position_Lex=0;static const int Cyc_Position_Parse=
1;static const int Cyc_Position_Elab=2;struct Cyc_Position_Error{struct _tagged_arr
source;struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
unsigned char Cyc_Position_Nocontext[14];extern struct _tagged_arr Cyc_Position_get_line_directive(
struct Cyc_Position_Segment*loc);static const int Cyc_Absyn_Loc_n=0;static const int
Cyc_Absyn_Rel_n=0;struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};
static const int Cyc_Absyn_Abs_n=1;struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;static
const int Cyc_Absyn_Static=0;static const int Cyc_Absyn_Abstract=1;static const int Cyc_Absyn_Public=
2;static const int Cyc_Absyn_Extern=3;static const int Cyc_Absyn_ExternC=4;struct Cyc_Absyn_Tqual{
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
void*Cyc_Absyn_compress_kb(void*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
extern struct _tagged_arr Cyc_Absyn_attribute2string(void*);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
extern int Cyc_Absynpp_is_anon_structtype(void*t);extern struct _tagged_arr*Cyc_Absynpp_cyc_stringptr;
extern int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);extern struct _tagged_arr Cyc_Absynpp_char_escape(
unsigned char);extern struct _tagged_arr Cyc_Absynpp_string_escape(struct
_tagged_arr);extern struct _tagged_arr Cyc_Absynpp_prim2str(void*p);extern int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);struct _tuple3{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};extern struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple3*arg);
struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};extern
struct _tuple4 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);extern
unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
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
ae;struct Cyc_Core_Opt*le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*
Cyc_Tcutil_compress(void*t);static int Cyc_Absyndump_expand_typedefs;static int Cyc_Absyndump_qvar_to_Cids;
static int Cyc_Absyndump_add_cyc_prefix;static int Cyc_Absyndump_generate_line_directives;
static int Cyc_Absyndump_to_VC;void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*
fs){Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;Cyc_Absyndump_qvar_to_Cids=
fs->qvar_to_Cids;Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absyndump_to_VC=
fs->to_VC;Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_set_params(fs);}void Cyc_Absyndump_dumptyp(void*);void Cyc_Absyndump_dumpntyp(
void*t);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumpexp_prec(
int,struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);void
Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*);void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*);void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(
void*),void*a);void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(
void*),void*);void Cyc_Absyndump_dumpstructfields(struct Cyc_List_List*fields);
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);void Cyc_Absyndump_dumploc(
struct Cyc_Position_Segment*);struct Cyc_Std___sFILE**Cyc_Absyndump_dump_file=& Cyc_Std_stdout;
void Cyc_Absyndump_ignore(void*x){return;}static unsigned int Cyc_Absyndump_pos=0;
static unsigned char Cyc_Absyndump_prev_char='x';int Cyc_Absyndump_need_space_before(){
switch(Cyc_Absyndump_prev_char){case '{': _LL0: goto _LL1;case '}': _LL1: goto _LL2;case
'(': _LL2: goto _LL3;case ')': _LL3: goto _LL4;case '[': _LL4: goto _LL5;case ']': _LL5: goto
_LL6;case ';': _LL6: goto _LL7;case ',': _LL7: goto _LL8;case '=': _LL8: goto _LL9;case ' ':
_LL9: goto _LL10;case '\n': _LL10: goto _LL11;case '*': _LL11: return 0;default: _LL12:
return 1;}}void Cyc_Absyndump_dump(struct _tagged_arr s){int sz=(int)_get_arr_size(s,
sizeof(unsigned char));if(!((int)*((const unsigned char*)_check_unknown_subscript(
s,sizeof(unsigned char),sz - 1)))){-- sz;}Cyc_Absyndump_pos +=sz;if(Cyc_Absyndump_pos
> 80){Cyc_Absyndump_pos=(unsigned int)sz;Cyc_Std_fputc((int)'\n',*Cyc_Absyndump_dump_file);}
else{if(Cyc_Absyndump_need_space_before()){Cyc_Std_fputc((int)' ',*Cyc_Absyndump_dump_file);}}
Cyc_Std_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);if(sz >= 1){Cyc_Absyndump_prev_char=*((
const unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),sz - 1));}}
void Cyc_Absyndump_dump_nospace(struct _tagged_arr s){int sz=(int)_get_arr_size(s,
sizeof(unsigned char));if(!((int)*((const unsigned char*)_check_unknown_subscript(
s,sizeof(unsigned char),sz - 1)))){-- sz;}Cyc_Absyndump_pos +=sz;Cyc_Std_file_string_write(*
Cyc_Absyndump_dump_file,s,0,sz);if(sz >= 1){Cyc_Absyndump_prev_char=*((const
unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),sz - 1));}}void
Cyc_Absyndump_dump_char(int c){++ Cyc_Absyndump_pos;Cyc_Std_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(unsigned char)c;}void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*
loc){if(loc == 0){return;}if(! Cyc_Absyndump_generate_line_directives){return;}{
struct _tagged_arr _tmpE=Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(
_tmpE);}}void Cyc_Absyndump_dump_str(struct _tagged_arr*s){Cyc_Absyndump_dump(*s);}
void Cyc_Absyndump_dump_semi(){Cyc_Absyndump_dump_char((int)';');}void Cyc_Absyndump_dump_sep(
void(*f)(void*),struct Cyc_List_List*l,struct _tagged_arr sep){if(l == 0){return;}
for(0;l->tl != 0;l=l->tl){f((void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f((
void*)l->hd);}void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct
Cyc_List_List*l,struct _tagged_arr sep){if(l == 0){return;}for(0;l->tl != 0;l=l->tl){
f(env,(void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f(env,(void*)l->hd);}void
Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep(
f,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep_c(
f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_egroup(void(*f)(
void*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep){if(l != 0){Cyc_Absyndump_group(f,l,start,end,sep);}}void Cyc_Absyndump_dumpqvar(
struct _tuple0*v){struct Cyc_List_List*_tmpF=0;{void*_tmp10=(*v).f1;struct Cyc_List_List*
_tmp18;struct Cyc_List_List*_tmp1A;_LL18: if(_tmp10 == (void*)Cyc_Absyn_Loc_n){goto
_LL19;}else{goto _LL20;}_LL20: if((unsigned int)_tmp10 > 1?*((int*)_tmp10)== Cyc_Absyn_Rel_n:
0){_LL25: _tmp18=((struct Cyc_Absyn_Rel_n_struct*)_tmp10)->f1;goto _LL21;}else{goto
_LL22;}_LL22: if((unsigned int)_tmp10 > 1?*((int*)_tmp10)== Cyc_Absyn_Abs_n: 0){
_LL27: _tmp1A=((struct Cyc_Absyn_Abs_n_struct*)_tmp10)->f1;goto _LL23;}else{goto
_LL17;}_LL19: _tmp18=0;goto _LL21;_LL21: _tmpF=_tmp18;goto _LL17;_LL23: _tmpF=(Cyc_Absyndump_qvar_to_Cids?
Cyc_Absyndump_add_cyc_prefix: 0)?({struct Cyc_List_List*_tmp1C=_cycalloc(sizeof(
struct Cyc_List_List));_tmp1C->hd=Cyc_Absynpp_cyc_stringptr;_tmp1C->tl=_tmp1A;
_tmp1C;}): _tmp1A;goto _LL17;_LL17:;}if(_tmpF != 0){Cyc_Absyndump_dump_str((struct
_tagged_arr*)_tmpF->hd);for(_tmpF=_tmpF->tl;_tmpF != 0;_tmpF=_tmpF->tl){if(Cyc_Absyndump_qvar_to_Cids){
Cyc_Absyndump_dump_char((int)'_');}else{Cyc_Absyndump_dump_nospace(_tag_arr("::",
sizeof(unsigned char),3));}Cyc_Absyndump_dump_nospace(*((struct _tagged_arr*)
_tmpF->hd));}if(Cyc_Absyndump_qvar_to_Cids){Cyc_Absyndump_dump_nospace(_tag_arr("_",
sizeof(unsigned char),2));}else{Cyc_Absyndump_dump_nospace(_tag_arr("::",sizeof(
unsigned char),3));}Cyc_Absyndump_dump_nospace(*(*v).f2);}else{Cyc_Absyndump_dump_str((*
v).f2);}}void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict){Cyc_Absyndump_dump(
_tag_arr("restrict",sizeof(unsigned char),9));}if(tq.q_volatile){Cyc_Absyndump_dump(
_tag_arr("volatile",sizeof(unsigned char),9));}if(tq.q_const){Cyc_Absyndump_dump(
_tag_arr("const",sizeof(unsigned char),6));}}void Cyc_Absyndump_dumpscope(void*sc){
void*_tmp1D=sc;_LL31: if(_tmp1D == (void*)Cyc_Absyn_Static){goto _LL32;}else{goto
_LL33;}_LL33: if(_tmp1D == (void*)Cyc_Absyn_Public){goto _LL34;}else{goto _LL35;}
_LL35: if(_tmp1D == (void*)Cyc_Absyn_Extern){goto _LL36;}else{goto _LL37;}_LL37: if(
_tmp1D == (void*)Cyc_Absyn_ExternC){goto _LL38;}else{goto _LL39;}_LL39: if(_tmp1D == (
void*)Cyc_Absyn_Abstract){goto _LL40;}else{goto _LL30;}_LL32: Cyc_Absyndump_dump(
_tag_arr("static",sizeof(unsigned char),7));return;_LL34: return;_LL36: Cyc_Absyndump_dump(
_tag_arr("extern",sizeof(unsigned char),7));return;_LL38: Cyc_Absyndump_dump(
_tag_arr("extern \"C\"",sizeof(unsigned char),11));return;_LL40: Cyc_Absyndump_dump(
_tag_arr("abstract",sizeof(unsigned char),9));return;_LL30:;}void Cyc_Absyndump_dumpkind(
void*k){void*_tmp29=k;_LL43: if(_tmp29 == (void*)Cyc_Absyn_AnyKind){goto _LL44;}
else{goto _LL45;}_LL45: if(_tmp29 == (void*)Cyc_Absyn_MemKind){goto _LL46;}else{goto
_LL47;}_LL47: if(_tmp29 == (void*)Cyc_Absyn_BoxKind){goto _LL48;}else{goto _LL49;}
_LL49: if(_tmp29 == (void*)Cyc_Absyn_RgnKind){goto _LL50;}else{goto _LL51;}_LL51: if(
_tmp29 == (void*)Cyc_Absyn_EffKind){goto _LL52;}else{goto _LL42;}_LL44: Cyc_Absyndump_dump(
_tag_arr("A",sizeof(unsigned char),2));return;_LL46: Cyc_Absyndump_dump(_tag_arr("M",
sizeof(unsigned char),2));return;_LL48: Cyc_Absyndump_dump(_tag_arr("B",sizeof(
unsigned char),2));return;_LL50: Cyc_Absyndump_dump(_tag_arr("R",sizeof(
unsigned char),2));return;_LL52: Cyc_Absyndump_dump(_tag_arr("E",sizeof(
unsigned char),2));return;_LL42:;}void Cyc_Absyndump_dumptps(struct Cyc_List_List*
ts){Cyc_Absyndump_egroup(Cyc_Absyndump_dumptyp,ts,_tag_arr("<",sizeof(
unsigned char),2),_tag_arr(">",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2));}void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);}void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);{void*_tmp35=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp3F;void*
_tmp41;_LL55: if(*((int*)_tmp35)== Cyc_Absyn_Eq_kb){_LL64: _tmp3F=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmp35)->f1;if(_tmp3F == (void*)Cyc_Absyn_BoxKind){goto
_LL56;}else{goto _LL57;}}else{goto _LL57;}_LL57: if(*((int*)_tmp35)== Cyc_Absyn_Less_kb){
goto _LL58;}else{goto _LL59;}_LL59: if(*((int*)_tmp35)== Cyc_Absyn_Unknown_kb){goto
_LL60;}else{goto _LL61;}_LL61: if(*((int*)_tmp35)== Cyc_Absyn_Eq_kb){_LL66: _tmp41=(
void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp35)->f1;goto _LL62;}else{goto _LL54;}
_LL56: goto _LL58;_LL58: goto _LL60;_LL60: Cyc_Absyndump_dump(_tag_arr("::?",sizeof(
unsigned char),4));goto _LL54;_LL62: Cyc_Absyndump_dump(_tag_arr("::",sizeof(
unsigned char),3));Cyc_Absyndump_dumpkind(_tmp41);goto _LL54;_LL54:;}}void Cyc_Absyndump_dumptvars(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,tvs,_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,tvs,_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}struct _tuple5{
struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(struct _tuple5*pr){((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple5*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumparg,ts,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dump_callconv(
struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmp43=(void*)atts->hd;
_LL69: if(_tmp43 == (void*)Cyc_Absyn_Stdcall_att){goto _LL70;}else{goto _LL71;}_LL71:
if(_tmp43 == (void*)Cyc_Absyn_Cdecl_att){goto _LL72;}else{goto _LL73;}_LL73: if(
_tmp43 == (void*)Cyc_Absyn_Fastcall_att){goto _LL74;}else{goto _LL75;}_LL75: goto
_LL76;_LL70: Cyc_Absyndump_dump(_tag_arr("_stdcall",sizeof(unsigned char),9));
return;_LL72: Cyc_Absyndump_dump(_tag_arr("_cdecl",sizeof(unsigned char),7));
return;_LL74: Cyc_Absyndump_dump(_tag_arr("_fastcall",sizeof(unsigned char),10));
return;_LL76: goto _LL68;_LL68:;}}void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*
atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp4D=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;_LL79: if(_tmp4D
== (void*)Cyc_Absyn_Stdcall_att){goto _LL80;}else{goto _LL81;}_LL81: if(_tmp4D == (
void*)Cyc_Absyn_Cdecl_att){goto _LL82;}else{goto _LL83;}_LL83: if(_tmp4D == (void*)
Cyc_Absyn_Fastcall_att){goto _LL84;}else{goto _LL85;}_LL85: goto _LL86;_LL80: goto
_LL78;_LL82: goto _LL78;_LL84: goto _LL78;_LL86: hasatt=1;goto _LL78;_LL78:;}}if(!
hasatt){return;}Cyc_Absyndump_dump(_tag_arr("__declspec(",sizeof(unsigned char),
12));for(0;atts != 0;atts=atts->tl){void*_tmp57=(void*)atts->hd;_LL89: if(_tmp57 == (
void*)Cyc_Absyn_Stdcall_att){goto _LL90;}else{goto _LL91;}_LL91: if(_tmp57 == (void*)
Cyc_Absyn_Cdecl_att){goto _LL92;}else{goto _LL93;}_LL93: if(_tmp57 == (void*)Cyc_Absyn_Fastcall_att){
goto _LL94;}else{goto _LL95;}_LL95: goto _LL96;_LL90: goto _LL88;_LL92: goto _LL88;_LL94:
goto _LL88;_LL96: Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));
goto _LL88;_LL88:;}Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpatts(
struct Cyc_List_List*atts){if(atts == 0){return;}if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dump_noncallconv(
atts);return;}Cyc_Absyndump_dump(_tag_arr(" __attribute__((",sizeof(
unsigned char),17));for(0;atts != 0;atts=atts->tl){Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((
void*)atts->hd));if(atts->tl != 0){Cyc_Absyndump_dump(_tag_arr(",",sizeof(
unsigned char),2));}}Cyc_Absyndump_dump(_tag_arr(")) ",sizeof(unsigned char),4));}
int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms){if(tms == 0){return 0;}{
void*_tmp61=(void*)tms->hd;_LL99: if((unsigned int)_tmp61 > 1?*((int*)_tmp61)== 
Cyc_Absyn_Pointer_mod: 0){goto _LL100;}else{goto _LL101;}_LL101: goto _LL102;_LL100:
return 1;_LL102: return 0;_LL98:;}}static void Cyc_Absyndump_dumprgn(void*t){void*
_tmp67=Cyc_Tcutil_compress(t);_LL105: if(_tmp67 == (void*)Cyc_Absyn_HeapRgn){goto
_LL106;}else{goto _LL107;}_LL107: goto _LL108;_LL106: Cyc_Absyndump_dump(_tag_arr("`H",
sizeof(unsigned char),3));goto _LL104;_LL108: Cyc_Absyndump_dumpntyp(t);goto _LL104;
_LL104:;}struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static
struct _tuple6 Cyc_Absyndump_effects_split(void*t){struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;{void*_tmp6D=Cyc_Tcutil_compress(t);void*_tmp75;
struct Cyc_List_List*_tmp77;_LL111: if((unsigned int)_tmp6D > 3?*((int*)_tmp6D)== 
Cyc_Absyn_AccessEff: 0){_LL118: _tmp75=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp6D)->f1;goto _LL112;}else{goto _LL113;}_LL113: if((unsigned int)_tmp6D > 3?*((
int*)_tmp6D)== Cyc_Absyn_JoinEff: 0){_LL120: _tmp77=((struct Cyc_Absyn_JoinEff_struct*)
_tmp6D)->f1;goto _LL114;}else{goto _LL115;}_LL115: goto _LL116;_LL112: rgions=({
struct Cyc_List_List*_tmp79=_cycalloc(sizeof(struct Cyc_List_List));_tmp79->hd=(
void*)_tmp75;_tmp79->tl=rgions;_tmp79;});goto _LL110;_LL114: for(0;_tmp77 != 0;
_tmp77=_tmp77->tl){struct Cyc_List_List*_tmp7C;struct Cyc_List_List*_tmp7E;struct
_tuple6 _tmp7A=Cyc_Absyndump_effects_split((void*)_tmp77->hd);_LL127: _tmp7E=
_tmp7A.f1;goto _LL125;_LL125: _tmp7C=_tmp7A.f2;goto _LL123;_LL123: rgions=Cyc_List_imp_append(
_tmp7E,rgions);effects=Cyc_List_imp_append(_tmp7C,effects);}goto _LL110;_LL116:
effects=({struct Cyc_List_List*_tmp80=_cycalloc(sizeof(struct Cyc_List_List));
_tmp80->hd=(void*)t;_tmp80->tl=effects;_tmp80;});goto _LL110;_LL110:;}return({
struct _tuple6 _tmp81;_tmp81.f1=rgions;_tmp81.f2=effects;_tmp81;});}static void Cyc_Absyndump_dumpeff(
void*t){struct Cyc_List_List*_tmp84;struct Cyc_List_List*_tmp86;struct _tuple6
_tmp82=Cyc_Absyndump_effects_split(t);_LL135: _tmp86=_tmp82.f1;goto _LL133;_LL133:
_tmp84=_tmp82.f2;goto _LL131;_LL131: _tmp86=Cyc_List_imp_rev(_tmp86);_tmp84=Cyc_List_imp_rev(
_tmp84);for(0;_tmp84 != 0;_tmp84=_tmp84->tl){Cyc_Absyndump_dumpntyp((void*)_tmp84->hd);
Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;
_tmp86 != 0;_tmp86=_tmp86->tl){Cyc_Absyndump_dumprgn((void*)_tmp86->hd);if(_tmp86->tl
!= 0){Cyc_Absyndump_dump_char((int)',');}}Cyc_Absyndump_dump_char((int)'}');}
void Cyc_Absyndump_dumpntyp(void*t){void*_tmp88=t;struct Cyc_Absyn_Tvar*_tmpD4;int
_tmpD6;struct Cyc_Core_Opt*_tmpD8;struct Cyc_Core_Opt*_tmpDA;int _tmpDC;struct Cyc_Core_Opt*
_tmpDE;struct Cyc_Core_Opt _tmpE0;void*_tmpE1;struct Cyc_Core_Opt*_tmpE3;struct Cyc_Absyn_TunionInfo
_tmpE5;void*_tmpE7;struct Cyc_List_List*_tmpE9;void*_tmpEB;struct Cyc_Absyn_TunionFieldInfo
_tmpED;struct Cyc_List_List*_tmpEF;void*_tmpF1;struct _tuple0*_tmpF3;void*_tmpF5;
void*_tmpF7;void*_tmpF9;void*_tmpFB;void*_tmpFD;void*_tmpFF;void*_tmp101;void*
_tmp103;void*_tmp105;void*_tmp107;void*_tmp109;void*_tmp10B;void*_tmp10E;void*
_tmp110;void*_tmp112;void*_tmp114;void*_tmp117;void*_tmp119;void*_tmp11B;void*
_tmp11D;int _tmp11F;struct Cyc_List_List*_tmp121;struct Cyc_List_List*_tmp123;
struct _tuple0*_tmp125;struct Cyc_List_List*_tmp127;struct _tuple0*_tmp129;struct
Cyc_List_List*_tmp12B;struct _tuple0*_tmp12D;struct Cyc_List_List*_tmp12F;struct
_tuple0*_tmp131;struct Cyc_List_List*_tmp133;struct Cyc_List_List*_tmp135;struct
Cyc_List_List*_tmp137;struct Cyc_Core_Opt*_tmp139;struct Cyc_List_List*_tmp13B;
struct _tuple0*_tmp13D;void*_tmp13F;void*_tmp141;_LL138: if((unsigned int)_tmp88 > 
3?*((int*)_tmp88)== Cyc_Absyn_ArrayType: 0){goto _LL139;}else{goto _LL140;}_LL140:
if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_FnType: 0){goto _LL141;}
else{goto _LL142;}_LL142: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_PointerType:
0){goto _LL143;}else{goto _LL144;}_LL144: if(_tmp88 == (void*)Cyc_Absyn_VoidType){
goto _LL145;}else{goto _LL146;}_LL146: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== 
Cyc_Absyn_VarType: 0){_LL213: _tmpD4=((struct Cyc_Absyn_VarType_struct*)_tmp88)->f1;
goto _LL147;}else{goto _LL148;}_LL148: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== 
Cyc_Absyn_Evar: 0){_LL219: _tmpDA=((struct Cyc_Absyn_Evar_struct*)_tmp88)->f1;goto
_LL217;_LL217: _tmpD8=((struct Cyc_Absyn_Evar_struct*)_tmp88)->f2;if(_tmpD8 == 0){
goto _LL215;}else{goto _LL150;}_LL215: _tmpD6=((struct Cyc_Absyn_Evar_struct*)_tmp88)->f3;
goto _LL149;}else{goto _LL150;}_LL150: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== 
Cyc_Absyn_Evar: 0){_LL228: _tmpE3=((struct Cyc_Absyn_Evar_struct*)_tmp88)->f1;goto
_LL223;_LL223: _tmpDE=((struct Cyc_Absyn_Evar_struct*)_tmp88)->f2;if(_tmpDE == 0){
goto _LL152;}else{_tmpE0=*_tmpDE;_LL226: _tmpE1=(void*)_tmpE0.v;goto _LL221;}_LL221:
_tmpDC=((struct Cyc_Absyn_Evar_struct*)_tmp88)->f3;goto _LL151;}else{goto _LL152;}
_LL152: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_TunionType: 0){
_LL230: _tmpE5=((struct Cyc_Absyn_TunionType_struct*)_tmp88)->f1;_LL236: _tmpEB=(
void*)_tmpE5.tunion_info;goto _LL234;_LL234: _tmpE9=_tmpE5.targs;goto _LL232;_LL232:
_tmpE7=(void*)_tmpE5.rgn;goto _LL153;}else{goto _LL154;}_LL154: if((unsigned int)
_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_TunionFieldType: 0){_LL238: _tmpED=((struct
Cyc_Absyn_TunionFieldType_struct*)_tmp88)->f1;_LL242: _tmpF1=(void*)_tmpED.field_info;
goto _LL240;_LL240: _tmpEF=_tmpED.targs;goto _LL155;}else{goto _LL156;}_LL156: if((
unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_EnumType: 0){_LL244: _tmpF3=((
struct Cyc_Absyn_EnumType_struct*)_tmp88)->f1;goto _LL157;}else{goto _LL158;}_LL158:
if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_IntType: 0){_LL248: _tmpF7=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp88)->f1;if(_tmpF7 == (void*)Cyc_Absyn_Signed){
goto _LL246;}else{goto _LL160;}_LL246: _tmpF5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp88)->f2;if(_tmpF5 == (void*)Cyc_Absyn_B4){goto _LL159;}else{goto _LL160;}}else{
goto _LL160;}_LL160: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_IntType:
0){_LL252: _tmpFB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp88)->f1;if(_tmpFB
== (void*)Cyc_Absyn_Signed){goto _LL250;}else{goto _LL162;}_LL250: _tmpF9=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp88)->f2;if(_tmpF9 == (void*)Cyc_Absyn_B1){
goto _LL161;}else{goto _LL162;}}else{goto _LL162;}_LL162: if((unsigned int)_tmp88 > 3?*((
int*)_tmp88)== Cyc_Absyn_IntType: 0){_LL256: _tmpFF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp88)->f1;if(_tmpFF == (void*)Cyc_Absyn_Unsigned){goto _LL254;}else{goto _LL164;}
_LL254: _tmpFD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp88)->f2;if(_tmpFD == (
void*)Cyc_Absyn_B1){goto _LL163;}else{goto _LL164;}}else{goto _LL164;}_LL164: if((
unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_IntType: 0){_LL260: _tmp103=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp88)->f1;if(_tmp103 == (void*)Cyc_Absyn_Signed){
goto _LL258;}else{goto _LL166;}_LL258: _tmp101=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp88)->f2;if(_tmp101 == (void*)Cyc_Absyn_B2){goto _LL165;}else{goto _LL166;}}
else{goto _LL166;}_LL166: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_IntType:
0){_LL264: _tmp107=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp88)->f1;if(
_tmp107 == (void*)Cyc_Absyn_Unsigned){goto _LL262;}else{goto _LL168;}_LL262: _tmp105=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp88)->f2;if(_tmp105 == (void*)Cyc_Absyn_B2){
goto _LL167;}else{goto _LL168;}}else{goto _LL168;}_LL168: if((unsigned int)_tmp88 > 3?*((
int*)_tmp88)== Cyc_Absyn_IntType: 0){_LL268: _tmp10B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp88)->f1;if(_tmp10B == (void*)Cyc_Absyn_Unsigned){goto _LL266;}else{goto _LL170;}
_LL266: _tmp109=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp88)->f2;if(_tmp109 == (
void*)Cyc_Absyn_B4){goto _LL169;}else{goto _LL170;}}else{goto _LL170;}_LL170: if((
unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_IntType: 0){_LL273: _tmp110=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp88)->f1;if(_tmp110 == (void*)Cyc_Absyn_Signed){
goto _LL271;}else{goto _LL172;}_LL271: _tmp10E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp88)->f2;if(_tmp10E == (void*)Cyc_Absyn_B8){goto _LL269;}else{goto _LL172;}}
else{goto _LL172;}_LL269: if(Cyc_Absyndump_to_VC){goto _LL171;}else{goto _LL172;}
_LL172: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_IntType: 0){_LL277:
_tmp114=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp88)->f1;if(_tmp114 == (void*)
Cyc_Absyn_Signed){goto _LL275;}else{goto _LL174;}_LL275: _tmp112=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp88)->f2;if(_tmp112 == (void*)Cyc_Absyn_B8){goto _LL173;}else{goto _LL174;}}
else{goto _LL174;}_LL174: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_IntType:
0){_LL282: _tmp119=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp88)->f1;if(
_tmp119 == (void*)Cyc_Absyn_Unsigned){goto _LL280;}else{goto _LL176;}_LL280: _tmp117=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp88)->f2;if(_tmp117 == (void*)Cyc_Absyn_B8){
goto _LL278;}else{goto _LL176;}}else{goto _LL176;}_LL278: if(Cyc_Absyndump_to_VC){
goto _LL175;}else{goto _LL176;}_LL176: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== 
Cyc_Absyn_IntType: 0){_LL286: _tmp11D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp88)->f1;if(_tmp11D == (void*)Cyc_Absyn_Unsigned){goto _LL284;}else{goto _LL178;}
_LL284: _tmp11B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp88)->f2;if(_tmp11B == (
void*)Cyc_Absyn_B8){goto _LL177;}else{goto _LL178;}}else{goto _LL178;}_LL178: if(
_tmp88 == (void*)Cyc_Absyn_FloatType){goto _LL179;}else{goto _LL180;}_LL180: if((
unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_DoubleType: 0){_LL288: _tmp11F=((
struct Cyc_Absyn_DoubleType_struct*)_tmp88)->f1;goto _LL181;}else{goto _LL182;}
_LL182: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_TupleType: 0){
_LL290: _tmp121=((struct Cyc_Absyn_TupleType_struct*)_tmp88)->f1;goto _LL183;}else{
goto _LL184;}_LL184: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_StructType:
0){_LL294: _tmp125=((struct Cyc_Absyn_StructType_struct*)_tmp88)->f1;if(_tmp125 == 
0){goto _LL292;}else{goto _LL186;}_LL292: _tmp123=((struct Cyc_Absyn_StructType_struct*)
_tmp88)->f2;goto _LL185;}else{goto _LL186;}_LL186: if((unsigned int)_tmp88 > 3?*((
int*)_tmp88)== Cyc_Absyn_StructType: 0){_LL298: _tmp129=((struct Cyc_Absyn_StructType_struct*)
_tmp88)->f1;goto _LL296;_LL296: _tmp127=((struct Cyc_Absyn_StructType_struct*)
_tmp88)->f2;goto _LL187;}else{goto _LL188;}_LL188: if((unsigned int)_tmp88 > 3?*((
int*)_tmp88)== Cyc_Absyn_UnionType: 0){_LL302: _tmp12D=((struct Cyc_Absyn_UnionType_struct*)
_tmp88)->f1;if(_tmp12D == 0){goto _LL300;}else{goto _LL190;}_LL300: _tmp12B=((struct
Cyc_Absyn_UnionType_struct*)_tmp88)->f2;goto _LL189;}else{goto _LL190;}_LL190: if((
unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_UnionType: 0){_LL306: _tmp131=((
struct Cyc_Absyn_UnionType_struct*)_tmp88)->f1;goto _LL304;_LL304: _tmp12F=((struct
Cyc_Absyn_UnionType_struct*)_tmp88)->f2;goto _LL191;}else{goto _LL192;}_LL192: if((
unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_AnonStructType: 0){_LL308:
_tmp133=((struct Cyc_Absyn_AnonStructType_struct*)_tmp88)->f1;goto _LL193;}else{
goto _LL194;}_LL194: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_AnonUnionType:
0){_LL310: _tmp135=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp88)->f1;goto _LL195;}
else{goto _LL196;}_LL196: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_AnonEnumType:
0){_LL312: _tmp137=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp88)->f1;goto _LL197;}
else{goto _LL198;}_LL198: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_TypedefType:
0){_LL318: _tmp13D=((struct Cyc_Absyn_TypedefType_struct*)_tmp88)->f1;goto _LL316;
_LL316: _tmp13B=((struct Cyc_Absyn_TypedefType_struct*)_tmp88)->f2;goto _LL314;
_LL314: _tmp139=((struct Cyc_Absyn_TypedefType_struct*)_tmp88)->f3;goto _LL199;}
else{goto _LL200;}_LL200: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_SizeofType:
0){_LL320: _tmp13F=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp88)->f1;goto
_LL201;}else{goto _LL202;}_LL202: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_RgnHandleType:
0){_LL322: _tmp141=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp88)->f1;
goto _LL203;}else{goto _LL204;}_LL204: if(_tmp88 == (void*)Cyc_Absyn_HeapRgn){goto
_LL205;}else{goto _LL206;}_LL206: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_AccessEff:
0){goto _LL207;}else{goto _LL208;}_LL208: if((unsigned int)_tmp88 > 3?*((int*)_tmp88)
== Cyc_Absyn_RgnsEff: 0){goto _LL209;}else{goto _LL210;}_LL210: if((unsigned int)
_tmp88 > 3?*((int*)_tmp88)== Cyc_Absyn_JoinEff: 0){goto _LL211;}else{goto _LL137;}
_LL139: return;_LL141: return;_LL143: return;_LL145: Cyc_Absyndump_dump(_tag_arr("void",
sizeof(unsigned char),5));return;_LL147: Cyc_Absyndump_dump_str(_tmpD4->name);
return;_LL149: Cyc_Absyndump_dump(_tag_arr("%",sizeof(unsigned char),2));if(
_tmpDA == 0){Cyc_Absyndump_dump(_tag_arr("?",sizeof(unsigned char),2));}else{Cyc_Absyndump_dumpkind((
void*)_tmpDA->v);}Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp144;_tmp144.tag=Cyc_Std_Int_pa;_tmp144.f1=(int)((unsigned int)_tmpD6);{void*
_tmp143[1]={& _tmp144};Cyc_Std_aprintf(_tag_arr("(%d)",sizeof(unsigned char),5),
_tag_arr(_tmp143,sizeof(void*),1));}}));return;_LL151: Cyc_Absyndump_dumpntyp(
_tmpE1);return;_LL153:{void*_tmp145=_tmpEB;struct Cyc_Absyn_UnknownTunionInfo
_tmp14B;int _tmp14D;struct _tuple0*_tmp14F;struct Cyc_Absyn_Tuniondecl**_tmp151;
struct Cyc_Absyn_Tuniondecl*_tmp153;struct Cyc_Absyn_Tuniondecl _tmp154;int _tmp155;
struct _tuple0*_tmp157;_LL327: if(*((int*)_tmp145)== Cyc_Absyn_UnknownTunion){
_LL332: _tmp14B=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp145)->f1;_LL336:
_tmp14F=_tmp14B.name;goto _LL334;_LL334: _tmp14D=_tmp14B.is_xtunion;goto _LL328;}
else{goto _LL329;}_LL329: if(*((int*)_tmp145)== Cyc_Absyn_KnownTunion){_LL338:
_tmp151=((struct Cyc_Absyn_KnownTunion_struct*)_tmp145)->f1;_tmp153=*_tmp151;
_tmp154=*_tmp153;_LL344: _tmp157=_tmp154.name;goto _LL342;_LL342: _tmp155=_tmp154.is_xtunion;
goto _LL330;}else{goto _LL326;}_LL328: _tmp157=_tmp14F;_tmp155=_tmp14D;goto _LL330;
_LL330: if(_tmp155){Cyc_Absyndump_dump(_tag_arr("xtunion ",sizeof(unsigned char),
9));}else{Cyc_Absyndump_dump(_tag_arr("tunion ",sizeof(unsigned char),8));}{void*
_tmp159=Cyc_Tcutil_compress(_tmpE7);_LL347: if(_tmp159 == (void*)Cyc_Absyn_HeapRgn){
goto _LL348;}else{goto _LL349;}_LL349: goto _LL350;_LL348: goto _LL346;_LL350: Cyc_Absyndump_dumptyp(
_tmpE7);Cyc_Absyndump_dump(_tag_arr(" ",sizeof(unsigned char),2));goto _LL346;
_LL346:;}Cyc_Absyndump_dumpqvar(_tmp157);Cyc_Absyndump_dumptps(_tmpE9);goto
_LL326;_LL326:;}goto _LL137;_LL155:{void*_tmp15F=_tmpF1;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp165;int _tmp167;struct _tuple0*_tmp169;struct _tuple0*_tmp16B;struct Cyc_Absyn_Tunionfield*
_tmp16D;struct Cyc_Absyn_Tunionfield _tmp16F;struct _tuple0*_tmp170;struct Cyc_Absyn_Tuniondecl*
_tmp172;struct Cyc_Absyn_Tuniondecl _tmp174;int _tmp175;struct _tuple0*_tmp177;
_LL353: if(*((int*)_tmp15F)== Cyc_Absyn_UnknownTunionfield){_LL358: _tmp165=((
struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp15F)->f1;_LL364: _tmp16B=_tmp165.tunion_name;
goto _LL362;_LL362: _tmp169=_tmp165.field_name;goto _LL360;_LL360: _tmp167=_tmp165.is_xtunion;
goto _LL354;}else{goto _LL355;}_LL355: if(*((int*)_tmp15F)== Cyc_Absyn_KnownTunionfield){
_LL371: _tmp172=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp15F)->f1;_tmp174=*
_tmp172;_LL376: _tmp177=_tmp174.name;goto _LL374;_LL374: _tmp175=_tmp174.is_xtunion;
goto _LL366;_LL366: _tmp16D=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp15F)->f2;
_tmp16F=*_tmp16D;_LL369: _tmp170=_tmp16F.name;goto _LL356;}else{goto _LL352;}_LL354:
_tmp177=_tmp16B;_tmp175=_tmp167;_tmp170=_tmp169;goto _LL356;_LL356: if(_tmp175){
Cyc_Absyndump_dump(_tag_arr("xtunion ",sizeof(unsigned char),9));}else{Cyc_Absyndump_dump(
_tag_arr("tunion ",sizeof(unsigned char),8));}Cyc_Absyndump_dumpqvar(_tmp177);
Cyc_Absyndump_dump(_tag_arr(".",sizeof(unsigned char),2));Cyc_Absyndump_dumpqvar(
_tmp170);Cyc_Absyndump_dumptps(_tmpEF);goto _LL352;_LL352:;}goto _LL137;_LL157: Cyc_Absyndump_dump(
_tag_arr("enum ",sizeof(unsigned char),6));Cyc_Absyndump_dumpqvar(_tmpF3);
return;_LL159: Cyc_Absyndump_dump(_tag_arr("int",sizeof(unsigned char),4));
return;_LL161: Cyc_Absyndump_dump(_tag_arr("signed char",sizeof(unsigned char),12));
return;_LL163: Cyc_Absyndump_dump(_tag_arr("unsigned char",sizeof(unsigned char),
14));return;_LL165: Cyc_Absyndump_dump(_tag_arr("short",sizeof(unsigned char),6));
return;_LL167: Cyc_Absyndump_dump(_tag_arr("unsigned short",sizeof(unsigned char),
15));return;_LL169: Cyc_Absyndump_dump(_tag_arr("unsigned int",sizeof(
unsigned char),13));return;_LL171: Cyc_Absyndump_dump(_tag_arr("__int64",sizeof(
unsigned char),8));return;_LL173: Cyc_Absyndump_dump(_tag_arr("long long",sizeof(
unsigned char),10));return;_LL175: Cyc_Absyndump_dump(_tag_arr("unsigned __int64",
sizeof(unsigned char),17));return;_LL177: Cyc_Absyndump_dump(_tag_arr("unsigned long long",
sizeof(unsigned char),19));return;_LL179: Cyc_Absyndump_dump(_tag_arr("float",
sizeof(unsigned char),6));return;_LL181: if(_tmp11F){Cyc_Absyndump_dump(_tag_arr("long double",
sizeof(unsigned char),12));}else{Cyc_Absyndump_dump(_tag_arr("double",sizeof(
unsigned char),7));}return;_LL183: Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(
_tmp121);return;_LL185: Cyc_Absyndump_dump(_tag_arr("struct",sizeof(unsigned char),
7));Cyc_Absyndump_dumptps(_tmp123);return;_LL187: Cyc_Absyndump_dump(_tag_arr("struct",
sizeof(unsigned char),7));Cyc_Absyndump_dumpqvar((struct _tuple0*)_check_null(
_tmp129));Cyc_Absyndump_dumptps(_tmp127);return;_LL189: Cyc_Absyndump_dump(
_tag_arr("union",sizeof(unsigned char),6));Cyc_Absyndump_dumptps(_tmp12B);
return;_LL191: Cyc_Absyndump_dump(_tag_arr("union",sizeof(unsigned char),6));Cyc_Absyndump_dumpqvar((
struct _tuple0*)_check_null(_tmp131));Cyc_Absyndump_dumptps(_tmp12F);return;
_LL193: Cyc_Absyndump_dump(_tag_arr("struct {",sizeof(unsigned char),9));Cyc_Absyndump_dumpstructfields(
_tmp133);Cyc_Absyndump_dump(_tag_arr("}",sizeof(unsigned char),2));return;_LL195:
Cyc_Absyndump_dump(_tag_arr("union {",sizeof(unsigned char),8));Cyc_Absyndump_dumpstructfields(
_tmp135);Cyc_Absyndump_dump(_tag_arr("}",sizeof(unsigned char),2));return;_LL197:
Cyc_Absyndump_dump(_tag_arr("enum {",sizeof(unsigned char),7));Cyc_Absyndump_dumpenumfields(
_tmp137);Cyc_Absyndump_dump(_tag_arr("}",sizeof(unsigned char),2));return;_LL199:(
Cyc_Absyndump_dumpqvar(_tmp13D),Cyc_Absyndump_dumptps(_tmp13B));return;_LL201:
Cyc_Absyndump_dump(_tag_arr("sizeof_t<",sizeof(unsigned char),10));Cyc_Absyndump_dumpntyp(
_tmp13F);Cyc_Absyndump_dump(_tag_arr(">",sizeof(unsigned char),2));return;_LL203:
Cyc_Absyndump_dumprgn(_tmp141);return;_LL205: return;_LL207: return;_LL209: return;
_LL211: return;_LL137:;}void Cyc_Absyndump_dumpvaropt(struct Cyc_Core_Opt*vo){if(vo
!= 0){Cyc_Absyndump_dump_str((struct _tagged_arr*)vo->v);}}void Cyc_Absyndump_dumpfunarg(
struct _tuple1*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),
struct Cyc_Core_Opt*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*
t).f1);}struct _tuple7{void*f1;void*f2;};void Cyc_Absyndump_dump_rgncmp(struct
_tuple7*cmp){struct _tuple7 _tmp17B;void*_tmp17C;void*_tmp17E;struct _tuple7*
_tmp179=cmp;_tmp17B=*_tmp179;_LL383: _tmp17E=_tmp17B.f1;goto _LL381;_LL381: _tmp17C=
_tmp17B.f2;goto _LL378;_LL378: Cyc_Absyndump_dumptyp(_tmp17E);Cyc_Absyndump_dump_char((
int)'<');Cyc_Absyndump_dumptyp(_tmp17C);}void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*
rgn_po){((void(*)(void(*f)(struct _tuple7*),struct Cyc_List_List*l,struct
_tagged_arr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,rgn_po,
_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((int)'(');for(0;args
!= 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple1*)args->hd);if((args->tl
!= 0? 1: c_varargs)? 1: cyc_varargs != 0){Cyc_Absyndump_dump_char((int)',');}}if(
c_varargs){Cyc_Absyndump_dump(_tag_arr("...",sizeof(unsigned char),4));}else{if(
cyc_varargs != 0){struct _tuple1*_tmp180=({struct _tuple1*_tmp181=_cycalloc(sizeof(
struct _tuple1));_tmp181->f1=cyc_varargs->name;_tmp181->f2=cyc_varargs->tq;
_tmp181->f3=(void*)cyc_varargs->type;_tmp181;});Cyc_Absyndump_dump(_tag_arr("...",
sizeof(unsigned char),4));if(cyc_varargs->inject){Cyc_Absyndump_dump(_tag_arr(" inject ",
sizeof(unsigned char),9));}Cyc_Absyndump_dumpfunarg(_tmp180);}}if(effopt != 0){
Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpeff((void*)effopt->v);}if(rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}Cyc_Absyndump_dump_char((
int)')');}void Cyc_Absyndump_dumptyp(void*t){((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(int),int))Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp182;
_tmp182.q_const=0;_tmp182.q_volatile=0;_tmp182.q_restrict=0;_tmp182;}),t,(void(*)(
int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpdesignator(void*d){void*
_tmp183=d;struct Cyc_Absyn_Exp*_tmp189;struct _tagged_arr*_tmp18B;_LL389: if(*((int*)
_tmp183)== Cyc_Absyn_ArrayElement){_LL394: _tmp189=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp183)->f1;goto _LL390;}else{goto _LL391;}_LL391: if(*((int*)_tmp183)== Cyc_Absyn_FieldName){
_LL396: _tmp18B=((struct Cyc_Absyn_FieldName_struct*)_tmp183)->f1;goto _LL392;}
else{goto _LL388;}_LL390: Cyc_Absyndump_dump(_tag_arr(".[",sizeof(unsigned char),3));
Cyc_Absyndump_dumpexp(_tmp189);Cyc_Absyndump_dump_char((int)']');goto _LL388;
_LL392: Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp18B);
goto _LL388;_LL388:;}struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;
};void Cyc_Absyndump_dumpde(struct _tuple8*de){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*
de).f1,_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",sizeof(unsigned char),2),
_tag_arr("=",sizeof(unsigned char),2));Cyc_Absyndump_dumpexp((*de).f2);}void Cyc_Absyndump_dumpexps_prec(
int inprec,struct Cyc_List_List*es){((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),
int env,struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,inprec,es,
_tag_arr("",sizeof(unsigned char),1),_tag_arr("",sizeof(unsigned char),1),
_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpexp_prec(int inprec,
struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= myprec){Cyc_Absyndump_dump_nospace(
_tag_arr("(",sizeof(unsigned char),2));}{void*_tmp18D=(void*)e->r;void*_tmp1F1;
unsigned char _tmp1F3;void*_tmp1F5;void*_tmp1F7;short _tmp1F9;void*_tmp1FB;void*
_tmp1FD;int _tmp1FF;void*_tmp201;void*_tmp203;int _tmp205;void*_tmp207;void*
_tmp209;long long _tmp20B;void*_tmp20D;void*_tmp20F;struct _tagged_arr _tmp211;void*
_tmp213;void*_tmp215;struct _tagged_arr _tmp217;struct _tuple0*_tmp219;struct
_tuple0*_tmp21B;struct Cyc_List_List*_tmp21D;void*_tmp21F;struct Cyc_Absyn_Exp*
_tmp221;struct Cyc_Core_Opt*_tmp223;struct Cyc_Absyn_Exp*_tmp225;void*_tmp227;
struct Cyc_Absyn_Exp*_tmp229;void*_tmp22B;struct Cyc_Absyn_Exp*_tmp22D;void*
_tmp22F;struct Cyc_Absyn_Exp*_tmp231;void*_tmp233;struct Cyc_Absyn_Exp*_tmp235;
struct Cyc_Absyn_Exp*_tmp237;struct Cyc_Absyn_Exp*_tmp239;struct Cyc_Absyn_Exp*
_tmp23B;struct Cyc_Absyn_Exp*_tmp23D;struct Cyc_Absyn_Exp*_tmp23F;struct Cyc_List_List*
_tmp241;struct Cyc_Absyn_Exp*_tmp243;struct Cyc_List_List*_tmp245;struct Cyc_Absyn_Exp*
_tmp247;struct Cyc_Absyn_Exp*_tmp249;struct Cyc_Absyn_Exp*_tmp24B;struct Cyc_Absyn_Exp*
_tmp24D;struct Cyc_Absyn_Exp*_tmp24F;void*_tmp251;struct Cyc_Absyn_Exp*_tmp253;
struct Cyc_Absyn_Exp*_tmp255;struct Cyc_Absyn_Exp*_tmp257;void*_tmp259;struct Cyc_Absyn_Exp*
_tmp25B;void*_tmp25D;struct _tagged_arr*_tmp25F;void*_tmp261;void*_tmp263;
unsigned int _tmp265;void*_tmp267;void*_tmp269;struct Cyc_List_List*_tmp26B;struct
Cyc_Absyn_Exp*_tmp26D;struct _tagged_arr*_tmp26F;struct Cyc_Absyn_Exp*_tmp271;
struct _tagged_arr*_tmp273;struct Cyc_Absyn_Exp*_tmp275;struct Cyc_Absyn_Exp*
_tmp277;struct Cyc_Absyn_Exp*_tmp279;struct Cyc_List_List*_tmp27B;struct Cyc_List_List*
_tmp27D;struct _tuple1*_tmp27F;struct Cyc_List_List*_tmp281;struct Cyc_Absyn_Exp*
_tmp283;struct Cyc_Absyn_Exp*_tmp285;struct Cyc_Absyn_Vardecl*_tmp287;struct Cyc_List_List*
_tmp289;struct _tuple0*_tmp28B;struct Cyc_List_List*_tmp28D;struct Cyc_Absyn_Tunionfield*
_tmp28F;struct Cyc_List_List*_tmp291;struct _tuple0*_tmp293;struct _tuple0*_tmp295;
struct Cyc_Absyn_MallocInfo _tmp297;struct Cyc_Absyn_Exp*_tmp299;void**_tmp29B;
struct Cyc_Absyn_Exp*_tmp29D;int _tmp29F;struct Cyc_List_List*_tmp2A1;struct Cyc_Core_Opt*
_tmp2A3;struct Cyc_Absyn_Stmt*_tmp2A5;struct Cyc_Absyn_Fndecl*_tmp2A7;struct Cyc_Absyn_Exp*
_tmp2A9;_LL399: if(*((int*)_tmp18D)== Cyc_Absyn_Const_e){_LL498: _tmp1F1=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp18D)->f1;if((unsigned int)_tmp1F1 > 1?*((int*)
_tmp1F1)== Cyc_Absyn_Char_c: 0){_LL502: _tmp1F5=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp1F1)->f1;goto _LL500;_LL500: _tmp1F3=((struct Cyc_Absyn_Char_c_struct*)_tmp1F1)->f2;
goto _LL400;}else{goto _LL401;}}else{goto _LL401;}_LL401: if(*((int*)_tmp18D)== Cyc_Absyn_Const_e){
_LL504: _tmp1F7=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp18D)->f1;if((
unsigned int)_tmp1F7 > 1?*((int*)_tmp1F7)== Cyc_Absyn_Short_c: 0){_LL508: _tmp1FB=(
void*)((struct Cyc_Absyn_Short_c_struct*)_tmp1F7)->f1;goto _LL506;_LL506: _tmp1F9=((
struct Cyc_Absyn_Short_c_struct*)_tmp1F7)->f2;goto _LL402;}else{goto _LL403;}}else{
goto _LL403;}_LL403: if(*((int*)_tmp18D)== Cyc_Absyn_Const_e){_LL510: _tmp1FD=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp18D)->f1;if((unsigned int)_tmp1FD > 1?*((int*)
_tmp1FD)== Cyc_Absyn_Int_c: 0){_LL514: _tmp201=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp1FD)->f1;if(_tmp201 == (void*)Cyc_Absyn_Signed){goto _LL512;}else{goto _LL405;}
_LL512: _tmp1FF=((struct Cyc_Absyn_Int_c_struct*)_tmp1FD)->f2;goto _LL404;}else{
goto _LL405;}}else{goto _LL405;}_LL405: if(*((int*)_tmp18D)== Cyc_Absyn_Const_e){
_LL516: _tmp203=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp18D)->f1;if((
unsigned int)_tmp203 > 1?*((int*)_tmp203)== Cyc_Absyn_Int_c: 0){_LL520: _tmp207=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp203)->f1;if(_tmp207 == (void*)Cyc_Absyn_Unsigned){
goto _LL518;}else{goto _LL407;}_LL518: _tmp205=((struct Cyc_Absyn_Int_c_struct*)
_tmp203)->f2;goto _LL406;}else{goto _LL407;}}else{goto _LL407;}_LL407: if(*((int*)
_tmp18D)== Cyc_Absyn_Const_e){_LL522: _tmp209=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp18D)->f1;if((unsigned int)_tmp209 > 1?*((int*)_tmp209)== Cyc_Absyn_LongLong_c:
0){_LL526: _tmp20D=(void*)((struct Cyc_Absyn_LongLong_c_struct*)_tmp209)->f1;goto
_LL524;_LL524: _tmp20B=((struct Cyc_Absyn_LongLong_c_struct*)_tmp209)->f2;goto
_LL408;}else{goto _LL409;}}else{goto _LL409;}_LL409: if(*((int*)_tmp18D)== Cyc_Absyn_Const_e){
_LL528: _tmp20F=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp18D)->f1;if((
unsigned int)_tmp20F > 1?*((int*)_tmp20F)== Cyc_Absyn_Float_c: 0){_LL530: _tmp211=((
struct Cyc_Absyn_Float_c_struct*)_tmp20F)->f1;goto _LL410;}else{goto _LL411;}}else{
goto _LL411;}_LL411: if(*((int*)_tmp18D)== Cyc_Absyn_Const_e){_LL532: _tmp213=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp18D)->f1;if(_tmp213 == (void*)Cyc_Absyn_Null_c){
goto _LL412;}else{goto _LL413;}}else{goto _LL413;}_LL413: if(*((int*)_tmp18D)== Cyc_Absyn_Const_e){
_LL534: _tmp215=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp18D)->f1;if((
unsigned int)_tmp215 > 1?*((int*)_tmp215)== Cyc_Absyn_String_c: 0){_LL536: _tmp217=((
struct Cyc_Absyn_String_c_struct*)_tmp215)->f1;goto _LL414;}else{goto _LL415;}}
else{goto _LL415;}_LL415: if(*((int*)_tmp18D)== Cyc_Absyn_UnknownId_e){_LL538:
_tmp219=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp18D)->f1;goto _LL416;}else{goto
_LL417;}_LL417: if(*((int*)_tmp18D)== Cyc_Absyn_Var_e){_LL540: _tmp21B=((struct Cyc_Absyn_Var_e_struct*)
_tmp18D)->f1;goto _LL418;}else{goto _LL419;}_LL419: if(*((int*)_tmp18D)== Cyc_Absyn_Primop_e){
_LL544: _tmp21F=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp18D)->f1;goto _LL542;
_LL542: _tmp21D=((struct Cyc_Absyn_Primop_e_struct*)_tmp18D)->f2;goto _LL420;}else{
goto _LL421;}_LL421: if(*((int*)_tmp18D)== Cyc_Absyn_AssignOp_e){_LL550: _tmp225=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp18D)->f1;goto _LL548;_LL548: _tmp223=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp18D)->f2;goto _LL546;_LL546: _tmp221=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp18D)->f3;goto _LL422;}else{goto _LL423;}
_LL423: if(*((int*)_tmp18D)== Cyc_Absyn_Increment_e){_LL554: _tmp229=((struct Cyc_Absyn_Increment_e_struct*)
_tmp18D)->f1;goto _LL552;_LL552: _tmp227=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp18D)->f2;if(_tmp227 == (void*)Cyc_Absyn_PreInc){goto _LL424;}else{goto _LL425;}}
else{goto _LL425;}_LL425: if(*((int*)_tmp18D)== Cyc_Absyn_Increment_e){_LL558:
_tmp22D=((struct Cyc_Absyn_Increment_e_struct*)_tmp18D)->f1;goto _LL556;_LL556:
_tmp22B=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp18D)->f2;if(_tmp22B == (
void*)Cyc_Absyn_PreDec){goto _LL426;}else{goto _LL427;}}else{goto _LL427;}_LL427:
if(*((int*)_tmp18D)== Cyc_Absyn_Increment_e){_LL562: _tmp231=((struct Cyc_Absyn_Increment_e_struct*)
_tmp18D)->f1;goto _LL560;_LL560: _tmp22F=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp18D)->f2;if(_tmp22F == (void*)Cyc_Absyn_PostInc){goto _LL428;}else{goto _LL429;}}
else{goto _LL429;}_LL429: if(*((int*)_tmp18D)== Cyc_Absyn_Increment_e){_LL566:
_tmp235=((struct Cyc_Absyn_Increment_e_struct*)_tmp18D)->f1;goto _LL564;_LL564:
_tmp233=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp18D)->f2;if(_tmp233 == (
void*)Cyc_Absyn_PostDec){goto _LL430;}else{goto _LL431;}}else{goto _LL431;}_LL431:
if(*((int*)_tmp18D)== Cyc_Absyn_Conditional_e){_LL572: _tmp23B=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp18D)->f1;goto _LL570;_LL570: _tmp239=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp18D)->f2;goto _LL568;_LL568: _tmp237=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp18D)->f3;goto _LL432;}else{goto _LL433;}_LL433: if(*((int*)_tmp18D)== Cyc_Absyn_SeqExp_e){
_LL576: _tmp23F=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp18D)->f1;goto _LL574;_LL574:
_tmp23D=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp18D)->f2;goto _LL434;}else{goto
_LL435;}_LL435: if(*((int*)_tmp18D)== Cyc_Absyn_UnknownCall_e){_LL580: _tmp243=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp18D)->f1;goto _LL578;_LL578: _tmp241=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp18D)->f2;goto _LL436;}else{goto _LL437;}
_LL437: if(*((int*)_tmp18D)== Cyc_Absyn_FnCall_e){_LL584: _tmp247=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp18D)->f1;goto _LL582;_LL582: _tmp245=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp18D)->f2;goto _LL438;}else{goto _LL439;}_LL439: if(*((int*)_tmp18D)== Cyc_Absyn_Throw_e){
_LL586: _tmp249=((struct Cyc_Absyn_Throw_e_struct*)_tmp18D)->f1;goto _LL440;}else{
goto _LL441;}_LL441: if(*((int*)_tmp18D)== Cyc_Absyn_NoInstantiate_e){_LL588:
_tmp24B=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp18D)->f1;goto _LL442;}else{
goto _LL443;}_LL443: if(*((int*)_tmp18D)== Cyc_Absyn_Instantiate_e){_LL590: _tmp24D=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp18D)->f1;goto _LL444;}else{goto _LL445;}
_LL445: if(*((int*)_tmp18D)== Cyc_Absyn_Cast_e){_LL594: _tmp251=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp18D)->f1;goto _LL592;_LL592: _tmp24F=((struct Cyc_Absyn_Cast_e_struct*)_tmp18D)->f2;
goto _LL446;}else{goto _LL447;}_LL447: if(*((int*)_tmp18D)== Cyc_Absyn_Address_e){
_LL596: _tmp253=((struct Cyc_Absyn_Address_e_struct*)_tmp18D)->f1;goto _LL448;}
else{goto _LL449;}_LL449: if(*((int*)_tmp18D)== Cyc_Absyn_New_e){_LL600: _tmp257=((
struct Cyc_Absyn_New_e_struct*)_tmp18D)->f1;goto _LL598;_LL598: _tmp255=((struct Cyc_Absyn_New_e_struct*)
_tmp18D)->f2;goto _LL450;}else{goto _LL451;}_LL451: if(*((int*)_tmp18D)== Cyc_Absyn_Sizeoftyp_e){
_LL602: _tmp259=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp18D)->f1;goto
_LL452;}else{goto _LL453;}_LL453: if(*((int*)_tmp18D)== Cyc_Absyn_Sizeofexp_e){
_LL604: _tmp25B=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp18D)->f1;goto _LL454;}
else{goto _LL455;}_LL455: if(*((int*)_tmp18D)== Cyc_Absyn_Offsetof_e){_LL610:
_tmp261=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp18D)->f1;goto _LL606;
_LL606: _tmp25D=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp18D)->f2;if(*((
int*)_tmp25D)== Cyc_Absyn_StructField){_LL608: _tmp25F=((struct Cyc_Absyn_StructField_struct*)
_tmp25D)->f1;goto _LL456;}else{goto _LL457;}}else{goto _LL457;}_LL457: if(*((int*)
_tmp18D)== Cyc_Absyn_Offsetof_e){_LL616: _tmp267=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp18D)->f1;goto _LL612;_LL612: _tmp263=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp18D)->f2;if(*((int*)_tmp263)== Cyc_Absyn_TupleIndex){_LL614: _tmp265=((struct
Cyc_Absyn_TupleIndex_struct*)_tmp263)->f1;goto _LL458;}else{goto _LL459;}}else{
goto _LL459;}_LL459: if(*((int*)_tmp18D)== Cyc_Absyn_Gentyp_e){_LL620: _tmp26B=((
struct Cyc_Absyn_Gentyp_e_struct*)_tmp18D)->f1;goto _LL618;_LL618: _tmp269=(void*)((
struct Cyc_Absyn_Gentyp_e_struct*)_tmp18D)->f2;goto _LL460;}else{goto _LL461;}
_LL461: if(*((int*)_tmp18D)== Cyc_Absyn_Deref_e){_LL622: _tmp26D=((struct Cyc_Absyn_Deref_e_struct*)
_tmp18D)->f1;goto _LL462;}else{goto _LL463;}_LL463: if(*((int*)_tmp18D)== Cyc_Absyn_StructMember_e){
_LL626: _tmp271=((struct Cyc_Absyn_StructMember_e_struct*)_tmp18D)->f1;goto _LL624;
_LL624: _tmp26F=((struct Cyc_Absyn_StructMember_e_struct*)_tmp18D)->f2;goto _LL464;}
else{goto _LL465;}_LL465: if(*((int*)_tmp18D)== Cyc_Absyn_StructArrow_e){_LL630:
_tmp275=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp18D)->f1;goto _LL628;_LL628:
_tmp273=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp18D)->f2;goto _LL466;}else{
goto _LL467;}_LL467: if(*((int*)_tmp18D)== Cyc_Absyn_Subscript_e){_LL634: _tmp279=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp18D)->f1;goto _LL632;_LL632: _tmp277=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp18D)->f2;goto _LL468;}else{goto _LL469;}
_LL469: if(*((int*)_tmp18D)== Cyc_Absyn_Tuple_e){_LL636: _tmp27B=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp18D)->f1;goto _LL470;}else{goto _LL471;}_LL471: if(*((int*)_tmp18D)== Cyc_Absyn_CompoundLit_e){
_LL640: _tmp27F=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp18D)->f1;goto _LL638;
_LL638: _tmp27D=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp18D)->f2;goto _LL472;}
else{goto _LL473;}_LL473: if(*((int*)_tmp18D)== Cyc_Absyn_Array_e){_LL642: _tmp281=((
struct Cyc_Absyn_Array_e_struct*)_tmp18D)->f1;goto _LL474;}else{goto _LL475;}_LL475:
if(*((int*)_tmp18D)== Cyc_Absyn_Comprehension_e){_LL648: _tmp287=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp18D)->f1;goto _LL646;_LL646: _tmp285=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp18D)->f2;goto _LL644;_LL644: _tmp283=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp18D)->f3;goto _LL476;}else{goto _LL477;}_LL477: if(*((int*)_tmp18D)== Cyc_Absyn_Struct_e){
_LL652: _tmp28B=((struct Cyc_Absyn_Struct_e_struct*)_tmp18D)->f1;goto _LL650;_LL650:
_tmp289=((struct Cyc_Absyn_Struct_e_struct*)_tmp18D)->f3;goto _LL478;}else{goto
_LL479;}_LL479: if(*((int*)_tmp18D)== Cyc_Absyn_AnonStruct_e){_LL654: _tmp28D=((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp18D)->f2;goto _LL480;}else{goto _LL481;}
_LL481: if(*((int*)_tmp18D)== Cyc_Absyn_Tunion_e){_LL658: _tmp291=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp18D)->f3;goto _LL656;_LL656: _tmp28F=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp18D)->f5;goto _LL482;}else{goto _LL483;}_LL483: if(*((int*)_tmp18D)== Cyc_Absyn_Enum_e){
_LL660: _tmp293=((struct Cyc_Absyn_Enum_e_struct*)_tmp18D)->f1;goto _LL484;}else{
goto _LL485;}_LL485: if(*((int*)_tmp18D)== Cyc_Absyn_AnonEnum_e){_LL662: _tmp295=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp18D)->f1;goto _LL486;}else{goto _LL487;}
_LL487: if(*((int*)_tmp18D)== Cyc_Absyn_Malloc_e){_LL664: _tmp297=((struct Cyc_Absyn_Malloc_e_struct*)
_tmp18D)->f1;_LL672: _tmp29F=_tmp297.is_calloc;goto _LL670;_LL670: _tmp29D=_tmp297.rgn;
goto _LL668;_LL668: _tmp29B=_tmp297.elt_type;goto _LL666;_LL666: _tmp299=_tmp297.num_elts;
goto _LL488;}else{goto _LL489;}_LL489: if(*((int*)_tmp18D)== Cyc_Absyn_UnresolvedMem_e){
_LL676: _tmp2A3=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp18D)->f1;goto _LL674;
_LL674: _tmp2A1=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp18D)->f2;goto _LL490;}
else{goto _LL491;}_LL491: if(*((int*)_tmp18D)== Cyc_Absyn_StmtExp_e){_LL678:
_tmp2A5=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp18D)->f1;goto _LL492;}else{goto
_LL493;}_LL493: if(*((int*)_tmp18D)== Cyc_Absyn_Codegen_e){_LL680: _tmp2A7=((
struct Cyc_Absyn_Codegen_e_struct*)_tmp18D)->f1;goto _LL494;}else{goto _LL495;}
_LL495: if(*((int*)_tmp18D)== Cyc_Absyn_Fill_e){_LL682: _tmp2A9=((struct Cyc_Absyn_Fill_e_struct*)
_tmp18D)->f1;goto _LL496;}else{goto _LL398;}_LL400: Cyc_Absyndump_dump_char((int)'\'');
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp1F3));Cyc_Absyndump_dump_char((
int)'\'');goto _LL398;_LL402: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp2AC;_tmp2AC.tag=Cyc_Std_Int_pa;_tmp2AC.f1=(int)((unsigned int)((int)_tmp1F9));{
void*_tmp2AB[1]={& _tmp2AC};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),
_tag_arr(_tmp2AB,sizeof(void*),1));}}));goto _LL398;_LL404: Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp2AE;_tmp2AE.tag=Cyc_Std_Int_pa;
_tmp2AE.f1=(int)((unsigned int)_tmp1FF);{void*_tmp2AD[1]={& _tmp2AE};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp2AD,sizeof(void*),1));}}));
goto _LL398;_LL406: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp2B0;_tmp2B0.tag=Cyc_Std_Int_pa;_tmp2B0.f1=(unsigned int)_tmp205;{void*
_tmp2AF[1]={& _tmp2B0};Cyc_Std_aprintf(_tag_arr("%u",sizeof(unsigned char),3),
_tag_arr(_tmp2AF,sizeof(void*),1));}}));goto _LL398;_LL408: Cyc_Absyndump_dump(
_tag_arr("<<FIX LONG LONG CONSTANT>>",sizeof(unsigned char),27));goto _LL398;
_LL410: Cyc_Absyndump_dump(_tmp211);goto _LL398;_LL412: Cyc_Absyndump_dump(_tag_arr("NULL",
sizeof(unsigned char),5));goto _LL398;_LL414: Cyc_Absyndump_dump_char((int)'"');
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp217));Cyc_Absyndump_dump_char((
int)'"');goto _LL398;_LL416: _tmp21B=_tmp219;goto _LL418;_LL418: Cyc_Absyndump_dumpqvar(
_tmp21B);goto _LL398;_LL420: {struct _tagged_arr _tmp2B1=Cyc_Absynpp_prim2str(
_tmp21F);switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp21D)){case 1:
_LL690: if(_tmp21F == (void*)Cyc_Absyn_Size){Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp21D))->hd);Cyc_Absyndump_dump(
_tag_arr(".size",sizeof(unsigned char),6));}else{Cyc_Absyndump_dump(_tmp2B1);Cyc_Absyndump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp21D))->hd);}
break;case 2: _LL691: Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp21D))->hd);Cyc_Absyndump_dump(_tmp2B1);Cyc_Absyndump_dump_char((
int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp21D->tl))->hd);break;default: _LL692:(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmp2B6=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp2B6[0]=({struct Cyc_Core_Failure_struct _tmp2B7;_tmp2B7.tag=Cyc_Core_Failure;
_tmp2B7.f1=_tag_arr("Absyndump -- Bad number of arguments to primop",sizeof(
unsigned char),47);_tmp2B7;});_tmp2B6;}));}goto _LL398;}_LL422: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp225);if(_tmp223 != 0){Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((void*)
_tmp223->v));}Cyc_Absyndump_dump_nospace(_tag_arr("=",sizeof(unsigned char),2));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp221);goto _LL398;_LL424: Cyc_Absyndump_dump(
_tag_arr("++",sizeof(unsigned char),3));Cyc_Absyndump_dumpexp_prec(myprec,
_tmp229);goto _LL398;_LL426: Cyc_Absyndump_dump(_tag_arr("--",sizeof(unsigned char),
3));Cyc_Absyndump_dumpexp_prec(myprec,_tmp22D);goto _LL398;_LL428: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp231);Cyc_Absyndump_dump(_tag_arr("++",sizeof(unsigned char),3));goto
_LL398;_LL430: Cyc_Absyndump_dumpexp_prec(myprec,_tmp235);Cyc_Absyndump_dump(
_tag_arr("--",sizeof(unsigned char),3));goto _LL398;_LL432: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp23B);Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,
_tmp239);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,
_tmp237);goto _LL398;_LL434: Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp23F);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp23D);Cyc_Absyndump_dump_char((int)')');goto _LL398;_LL436: _tmp247=
_tmp243;_tmp245=_tmp241;goto _LL438;_LL438: Cyc_Absyndump_dumpexp_prec(myprec,
_tmp247);Cyc_Absyndump_dump_nospace(_tag_arr("(",sizeof(unsigned char),2));Cyc_Absyndump_dumpexps_prec(
20,_tmp245);Cyc_Absyndump_dump_nospace(_tag_arr(")",sizeof(unsigned char),2));
goto _LL398;_LL440: Cyc_Absyndump_dump(_tag_arr("throw",sizeof(unsigned char),6));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp249);goto _LL398;_LL442: _tmp24D=_tmp24B;
goto _LL444;_LL444: Cyc_Absyndump_dumpexp_prec(inprec,_tmp24D);goto _LL398;_LL446:
Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmp251);Cyc_Absyndump_dump_char((
int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmp24F);goto _LL398;_LL448: Cyc_Absyndump_dump_char((
int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp253);goto _LL398;_LL450: Cyc_Absyndump_dump(
_tag_arr("new ",sizeof(unsigned char),5));Cyc_Absyndump_dumpexp_prec(myprec,
_tmp255);goto _LL398;_LL452: Cyc_Absyndump_dump(_tag_arr("sizeof(",sizeof(
unsigned char),8));Cyc_Absyndump_dumptyp(_tmp259);Cyc_Absyndump_dump_char((int)')');
goto _LL398;_LL454: Cyc_Absyndump_dump(_tag_arr("sizeof(",sizeof(unsigned char),8));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp25B);Cyc_Absyndump_dump_char((int)')');
goto _LL398;_LL456: Cyc_Absyndump_dump(_tag_arr("offsetof(",sizeof(unsigned char),
10));Cyc_Absyndump_dumptyp(_tmp261);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*
_tmp25F);Cyc_Absyndump_dump_char((int)')');goto _LL398;_LL458: Cyc_Absyndump_dump(
_tag_arr("offsetof(",sizeof(unsigned char),10));Cyc_Absyndump_dumptyp(_tmp267);
Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump((struct _tagged_arr)({struct
Cyc_Std_Int_pa_struct _tmp2B9;_tmp2B9.tag=Cyc_Std_Int_pa;_tmp2B9.f1=(int)_tmp265;{
void*_tmp2B8[1]={& _tmp2B9};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),
_tag_arr(_tmp2B8,sizeof(void*),1));}}));Cyc_Absyndump_dump_char((int)')');goto
_LL398;_LL460: Cyc_Absyndump_dump(_tag_arr("__gen(",sizeof(unsigned char),7));Cyc_Absyndump_dumptvars(
_tmp26B);Cyc_Absyndump_dumptyp(_tmp269);Cyc_Absyndump_dump_char((int)')');goto
_LL398;_LL462: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp26D);goto _LL398;_LL464: Cyc_Absyndump_dumpexp_prec(myprec,_tmp271);Cyc_Absyndump_dump_char((
int)'.');Cyc_Absyndump_dump_nospace(*_tmp26F);goto _LL398;_LL466: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp275);Cyc_Absyndump_dump_nospace(_tag_arr("->",sizeof(unsigned char),3));
Cyc_Absyndump_dump_nospace(*_tmp273);goto _LL398;_LL468: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp279);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp277);
Cyc_Absyndump_dump_char((int)']');goto _LL398;_LL470: Cyc_Absyndump_dump(_tag_arr("$(",
sizeof(unsigned char),3));Cyc_Absyndump_dumpexps_prec(20,_tmp27B);Cyc_Absyndump_dump_char((
int)')');goto _LL398;_LL472: Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp((*
_tmp27F).f3);Cyc_Absyndump_dump_char((int)')');((void(*)(void(*f)(struct _tuple8*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp27D,_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LL398;_LL474:((void(*)(void(*f)(struct _tuple8*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp281,_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LL398;_LL476: Cyc_Absyndump_dump(_tag_arr("new {for",
sizeof(unsigned char),9));Cyc_Absyndump_dump_str((*_tmp287->name).f2);Cyc_Absyndump_dump_char((
int)'<');Cyc_Absyndump_dumpexp(_tmp285);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(
_tmp283);Cyc_Absyndump_dump_char((int)'}');goto _LL398;_LL478: Cyc_Absyndump_dumpqvar(
_tmp28B);((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp289,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));goto _LL398;
_LL480:((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp28D,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));goto _LL398;
_LL482: Cyc_Absyndump_dumpqvar(_tmp28F->name);if(_tmp291 != 0){((void(*)(void(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,
_tmp291,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));}goto _LL398;_LL484: Cyc_Absyndump_dumpqvar(
_tmp293);goto _LL398;_LL486: Cyc_Absyndump_dumpqvar(_tmp295);goto _LL398;_LL488: if(
_tmp29F){if(_tmp29D != 0){Cyc_Absyndump_dump(_tag_arr("rcalloc(",sizeof(
unsigned char),9));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(
_tmp29D));Cyc_Absyndump_dump(_tag_arr(",",sizeof(unsigned char),2));}else{Cyc_Absyndump_dump(
_tag_arr("calloc",sizeof(unsigned char),7));}Cyc_Absyndump_dumpexp(_tmp299);Cyc_Absyndump_dump(
_tag_arr(",",sizeof(unsigned char),2));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmp29B)),0));Cyc_Absyndump_dump(_tag_arr(")",sizeof(
unsigned char),2));}else{if(_tmp29D != 0){Cyc_Absyndump_dump(_tag_arr("rmalloc(",
sizeof(unsigned char),9));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmp29D));Cyc_Absyndump_dump(_tag_arr(",",sizeof(unsigned char),2));}
else{Cyc_Absyndump_dump(_tag_arr("malloc(",sizeof(unsigned char),8));}if(_tmp29B
!= 0){Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp29B,
0),_tmp299,0));}else{Cyc_Absyndump_dumpexp(_tmp299);}Cyc_Absyndump_dump(_tag_arr(")",
sizeof(unsigned char),2));}goto _LL398;_LL490:((void(*)(void(*f)(struct _tuple8*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp2A1,_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LL398;_LL492: Cyc_Absyndump_dump_nospace(_tag_arr("({",
sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(_tmp2A5);Cyc_Absyndump_dump_nospace(
_tag_arr("})",sizeof(unsigned char),3));goto _LL398;_LL494: Cyc_Absyndump_dump(
_tag_arr("codegen(",sizeof(unsigned char),9));Cyc_Absyndump_dumpdecl(({struct Cyc_Absyn_Decl*
_tmp2BA=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_tmp2BA->r=(void*)((void*)({
struct Cyc_Absyn_Fn_d_struct*_tmp2BB=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_struct));
_tmp2BB[0]=({struct Cyc_Absyn_Fn_d_struct _tmp2BC;_tmp2BC.tag=Cyc_Absyn_Fn_d;
_tmp2BC.f1=_tmp2A7;_tmp2BC;});_tmp2BB;}));_tmp2BA->loc=e->loc;_tmp2BA;}));Cyc_Absyndump_dump(
_tag_arr(")",sizeof(unsigned char),2));goto _LL398;_LL496: Cyc_Absyndump_dump(
_tag_arr("fill(",sizeof(unsigned char),6));Cyc_Absyndump_dumpexp(_tmp2A9);Cyc_Absyndump_dump(
_tag_arr(")",sizeof(unsigned char),2));goto _LL398;_LL398:;}if(inprec >= myprec){
Cyc_Absyndump_dump_char((int)')');}}void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*
e){Cyc_Absyndump_dumpexp_prec(0,e);}void Cyc_Absyndump_dumpswitchclauses(struct
Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmp2BD=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmp2BD->where_clause == 0?(
void*)(_tmp2BD->pattern)->r == (void*)Cyc_Absyn_Wild_p: 0){Cyc_Absyndump_dump(
_tag_arr("default:",sizeof(unsigned char),9));}else{Cyc_Absyndump_dump(_tag_arr("case",
sizeof(unsigned char),5));Cyc_Absyndump_dumppat(_tmp2BD->pattern);if(_tmp2BD->where_clause
!= 0){Cyc_Absyndump_dump(_tag_arr("&&",sizeof(unsigned char),3));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2BD->where_clause));}Cyc_Absyndump_dump_nospace(
_tag_arr(":",sizeof(unsigned char),2));}Cyc_Absyndump_dumpstmt(_tmp2BD->body);}}
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s){void*_tmp2BE=(void*)s->r;
struct Cyc_Absyn_Exp*_tmp2EE;struct Cyc_Absyn_Stmt*_tmp2F0;struct Cyc_Absyn_Stmt*
_tmp2F2;struct Cyc_Absyn_Exp*_tmp2F4;struct Cyc_Absyn_Exp*_tmp2F6;struct Cyc_Absyn_Stmt*
_tmp2F8;struct Cyc_Absyn_Stmt*_tmp2FA;struct Cyc_Absyn_Exp*_tmp2FC;struct Cyc_Absyn_Stmt*
_tmp2FE;struct _tuple2 _tmp300;struct Cyc_Absyn_Exp*_tmp302;struct _tagged_arr*
_tmp304;struct Cyc_Absyn_Stmt*_tmp306;struct _tuple2 _tmp308;struct Cyc_Absyn_Exp*
_tmp30A;struct _tuple2 _tmp30C;struct Cyc_Absyn_Exp*_tmp30E;struct Cyc_Absyn_Exp*
_tmp310;struct Cyc_Absyn_ForArrayInfo _tmp312;struct Cyc_Absyn_Stmt*_tmp314;struct
_tuple2 _tmp316;struct Cyc_Absyn_Exp*_tmp318;struct _tuple2 _tmp31A;struct Cyc_Absyn_Exp*
_tmp31C;struct Cyc_List_List*_tmp31E;struct Cyc_List_List*_tmp320;struct Cyc_Absyn_Exp*
_tmp322;struct Cyc_Absyn_Stmt*_tmp324;struct Cyc_Absyn_Decl*_tmp326;struct Cyc_Absyn_Stmt*
_tmp328;struct _tagged_arr*_tmp32A;struct _tuple2 _tmp32C;struct Cyc_Absyn_Exp*
_tmp32E;struct Cyc_Absyn_Stmt*_tmp330;struct Cyc_List_List*_tmp332;struct Cyc_Absyn_Exp*
_tmp334;struct Cyc_List_List*_tmp336;struct Cyc_List_List*_tmp338;struct Cyc_List_List*
_tmp33A;struct Cyc_Absyn_Stmt*_tmp33C;struct Cyc_Absyn_Stmt*_tmp33E;struct Cyc_Absyn_Vardecl*
_tmp340;struct Cyc_Absyn_Tvar*_tmp342;struct Cyc_Absyn_Stmt*_tmp344;struct Cyc_Absyn_Stmt*
_tmp346;_LL704: if(_tmp2BE == (void*)Cyc_Absyn_Skip_s){goto _LL705;}else{goto _LL706;}
_LL706: if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_Exp_s: 0){_LL751:
_tmp2EE=((struct Cyc_Absyn_Exp_s_struct*)_tmp2BE)->f1;goto _LL707;}else{goto _LL708;}
_LL708: if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_Seq_s: 0){_LL755:
_tmp2F2=((struct Cyc_Absyn_Seq_s_struct*)_tmp2BE)->f1;goto _LL753;_LL753: _tmp2F0=((
struct Cyc_Absyn_Seq_s_struct*)_tmp2BE)->f2;goto _LL709;}else{goto _LL710;}_LL710:
if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_Return_s: 0){_LL757:
_tmp2F4=((struct Cyc_Absyn_Return_s_struct*)_tmp2BE)->f1;if(_tmp2F4 == 0){goto
_LL711;}else{goto _LL712;}}else{goto _LL712;}_LL712: if((unsigned int)_tmp2BE > 1?*((
int*)_tmp2BE)== Cyc_Absyn_Return_s: 0){_LL759: _tmp2F6=((struct Cyc_Absyn_Return_s_struct*)
_tmp2BE)->f1;goto _LL713;}else{goto _LL714;}_LL714: if((unsigned int)_tmp2BE > 1?*((
int*)_tmp2BE)== Cyc_Absyn_IfThenElse_s: 0){_LL765: _tmp2FC=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2BE)->f1;goto _LL763;_LL763: _tmp2FA=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2BE)->f2;goto _LL761;_LL761: _tmp2F8=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2BE)->f3;goto _LL715;}else{goto _LL716;}_LL716: if((unsigned int)_tmp2BE > 1?*((
int*)_tmp2BE)== Cyc_Absyn_While_s: 0){_LL769: _tmp300=((struct Cyc_Absyn_While_s_struct*)
_tmp2BE)->f1;_LL771: _tmp302=_tmp300.f1;goto _LL767;_LL767: _tmp2FE=((struct Cyc_Absyn_While_s_struct*)
_tmp2BE)->f2;goto _LL717;}else{goto _LL718;}_LL718: if((unsigned int)_tmp2BE > 1?*((
int*)_tmp2BE)== Cyc_Absyn_Break_s: 0){goto _LL719;}else{goto _LL720;}_LL720: if((
unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_Continue_s: 0){goto _LL721;}
else{goto _LL722;}_LL722: if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_Goto_s:
0){_LL773: _tmp304=((struct Cyc_Absyn_Goto_s_struct*)_tmp2BE)->f1;goto _LL723;}
else{goto _LL724;}_LL724: if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_For_s:
0){_LL785: _tmp310=((struct Cyc_Absyn_For_s_struct*)_tmp2BE)->f1;goto _LL781;_LL781:
_tmp30C=((struct Cyc_Absyn_For_s_struct*)_tmp2BE)->f2;_LL783: _tmp30E=_tmp30C.f1;
goto _LL777;_LL777: _tmp308=((struct Cyc_Absyn_For_s_struct*)_tmp2BE)->f3;_LL779:
_tmp30A=_tmp308.f1;goto _LL775;_LL775: _tmp306=((struct Cyc_Absyn_For_s_struct*)
_tmp2BE)->f4;goto _LL725;}else{goto _LL726;}_LL726: if((unsigned int)_tmp2BE > 1?*((
int*)_tmp2BE)== Cyc_Absyn_ForArray_s: 0){_LL787: _tmp312=((struct Cyc_Absyn_ForArray_s_struct*)
_tmp2BE)->f1;_LL799: _tmp31E=_tmp312.defns;goto _LL795;_LL795: _tmp31A=_tmp312.condition;
_LL797: _tmp31C=_tmp31A.f1;goto _LL791;_LL791: _tmp316=_tmp312.delta;_LL793: _tmp318=
_tmp316.f1;goto _LL789;_LL789: _tmp314=_tmp312.body;goto _LL727;}else{goto _LL728;}
_LL728: if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_Switch_s: 0){
_LL803: _tmp322=((struct Cyc_Absyn_Switch_s_struct*)_tmp2BE)->f1;goto _LL801;_LL801:
_tmp320=((struct Cyc_Absyn_Switch_s_struct*)_tmp2BE)->f2;goto _LL729;}else{goto
_LL730;}_LL730: if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_Decl_s: 0){
_LL807: _tmp326=((struct Cyc_Absyn_Decl_s_struct*)_tmp2BE)->f1;goto _LL805;_LL805:
_tmp324=((struct Cyc_Absyn_Decl_s_struct*)_tmp2BE)->f2;goto _LL731;}else{goto
_LL732;}_LL732: if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_Label_s:
0){_LL811: _tmp32A=((struct Cyc_Absyn_Label_s_struct*)_tmp2BE)->f1;goto _LL809;
_LL809: _tmp328=((struct Cyc_Absyn_Label_s_struct*)_tmp2BE)->f2;goto _LL733;}else{
goto _LL734;}_LL734: if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_Do_s:
0){_LL817: _tmp330=((struct Cyc_Absyn_Do_s_struct*)_tmp2BE)->f1;goto _LL813;_LL813:
_tmp32C=((struct Cyc_Absyn_Do_s_struct*)_tmp2BE)->f2;_LL815: _tmp32E=_tmp32C.f1;
goto _LL735;}else{goto _LL736;}_LL736: if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)
== Cyc_Absyn_SwitchC_s: 0){_LL821: _tmp334=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmp2BE)->f1;goto _LL819;_LL819: _tmp332=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmp2BE)->f2;goto _LL737;}else{goto _LL738;}_LL738: if((unsigned int)_tmp2BE > 1?*((
int*)_tmp2BE)== Cyc_Absyn_Fallthru_s: 0){_LL823: _tmp336=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp2BE)->f1;if(_tmp336 == 0){goto _LL739;}else{goto _LL740;}}else{goto _LL740;}
_LL740: if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_Fallthru_s: 0){
_LL825: _tmp338=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp2BE)->f1;goto _LL741;}
else{goto _LL742;}_LL742: if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_TryCatch_s:
0){_LL829: _tmp33C=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp2BE)->f1;goto _LL827;
_LL827: _tmp33A=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp2BE)->f2;goto _LL743;}
else{goto _LL744;}_LL744: if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_Region_s:
0){_LL835: _tmp342=((struct Cyc_Absyn_Region_s_struct*)_tmp2BE)->f1;goto _LL833;
_LL833: _tmp340=((struct Cyc_Absyn_Region_s_struct*)_tmp2BE)->f2;goto _LL831;_LL831:
_tmp33E=((struct Cyc_Absyn_Region_s_struct*)_tmp2BE)->f3;goto _LL745;}else{goto
_LL746;}_LL746: if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_Cut_s: 0){
_LL837: _tmp344=((struct Cyc_Absyn_Cut_s_struct*)_tmp2BE)->f1;goto _LL747;}else{
goto _LL748;}_LL748: if((unsigned int)_tmp2BE > 1?*((int*)_tmp2BE)== Cyc_Absyn_Splice_s:
0){_LL839: _tmp346=((struct Cyc_Absyn_Splice_s_struct*)_tmp2BE)->f1;goto _LL749;}
else{goto _LL703;}_LL705: Cyc_Absyndump_dump_semi();goto _LL703;_LL707: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dumpexp(_tmp2EE);Cyc_Absyndump_dump_semi();goto _LL703;
_LL709: if(Cyc_Absynpp_is_declaration(_tmp2F2)){Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp2F2);Cyc_Absyndump_dump_char((int)'}');}else{Cyc_Absyndump_dumpstmt(
_tmp2F2);}if(Cyc_Absynpp_is_declaration(_tmp2F0)){Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp2F0);Cyc_Absyndump_dump_char((int)'}');}else{Cyc_Absyndump_dumpstmt(
_tmp2F0);}goto _LL703;_LL711: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("return;",sizeof(unsigned char),8));goto _LL703;_LL713: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("return",sizeof(unsigned char),7));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2F6));Cyc_Absyndump_dump_semi();goto _LL703;
_LL715: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("if(",sizeof(
unsigned char),4));Cyc_Absyndump_dumpexp(_tmp2FC);Cyc_Absyndump_dump_nospace(
_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(_tmp2FA);Cyc_Absyndump_dump_char((
int)'}');{void*_tmp348=(void*)_tmp2F8->r;_LL842: if(_tmp348 == (void*)Cyc_Absyn_Skip_s){
goto _LL843;}else{goto _LL844;}_LL844: goto _LL845;_LL843: goto _LL841;_LL845: Cyc_Absyndump_dump(
_tag_arr("else{",sizeof(unsigned char),6));Cyc_Absyndump_dumpstmt(_tmp2F8);Cyc_Absyndump_dump_char((
int)'}');goto _LL841;_LL841:;}goto _LL703;_LL717: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("while(",sizeof(unsigned char),7));Cyc_Absyndump_dumpexp(_tmp302);Cyc_Absyndump_dump_nospace(
_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(_tmp2FE);Cyc_Absyndump_dump_char((
int)'}');goto _LL703;_LL719: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("break;",sizeof(unsigned char),7));goto _LL703;_LL721: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("continue;",sizeof(unsigned char),10));goto
_LL703;_LL723: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("goto",
sizeof(unsigned char),5));Cyc_Absyndump_dump_str(_tmp304);Cyc_Absyndump_dump_semi();
goto _LL703;_LL725: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("for(",
sizeof(unsigned char),5));Cyc_Absyndump_dumpexp(_tmp310);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmp30E);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(
_tmp30A);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(
_tmp306);Cyc_Absyndump_dump_char((int)'}');goto _LL703;_LL727: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("forarray(",sizeof(unsigned char),10));for(0;
_tmp31E != 0;_tmp31E=_tmp31E->tl){Cyc_Absyndump_dumpvardecl((struct Cyc_Absyn_Vardecl*)
_tmp31E->hd,s->loc);}Cyc_Absyndump_dumpexp(_tmp31C);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmp318);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(
unsigned char),3));Cyc_Absyndump_dumpstmt(_tmp314);Cyc_Absyndump_dump_char((int)'}');
goto _LL703;_LL729: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("switch(",
sizeof(unsigned char),8));Cyc_Absyndump_dumpexp(_tmp322);Cyc_Absyndump_dump_nospace(
_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpswitchclauses(_tmp320);
Cyc_Absyndump_dump_char((int)'}');goto _LL703;_LL731: Cyc_Absyndump_dumpdecl(
_tmp326);Cyc_Absyndump_dumpstmt(_tmp324);goto _LL703;_LL733: if(Cyc_Absynpp_is_declaration(
_tmp328)){Cyc_Absyndump_dump_str(_tmp32A);Cyc_Absyndump_dump_nospace(_tag_arr(": {",
sizeof(unsigned char),4));Cyc_Absyndump_dumpstmt(_tmp328);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dump_str(_tmp32A);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpstmt(_tmp328);}goto _LL703;_LL735: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(_tag_arr("do {",sizeof(unsigned char),5));Cyc_Absyndump_dumpstmt(
_tmp330);Cyc_Absyndump_dump_nospace(_tag_arr("} while (",sizeof(unsigned char),
10));Cyc_Absyndump_dumpexp(_tmp32E);Cyc_Absyndump_dump_nospace(_tag_arr(");",
sizeof(unsigned char),3));goto _LL703;_LL737: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("switch \"C\" (",sizeof(unsigned char),13));Cyc_Absyndump_dumpexp(
_tmp334);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(unsigned char),3));for(
0;_tmp332 != 0;_tmp332=_tmp332->tl){struct Cyc_Absyn_SwitchC_clause _tmp350;struct
Cyc_Absyn_Stmt*_tmp351;struct Cyc_Absyn_Exp*_tmp353;struct Cyc_Absyn_SwitchC_clause*
_tmp34E=(struct Cyc_Absyn_SwitchC_clause*)_tmp332->hd;_tmp350=*_tmp34E;_LL852:
_tmp353=_tmp350.cnst_exp;goto _LL850;_LL850: _tmp351=_tmp350.body;goto _LL847;
_LL847: if(_tmp353 == 0){Cyc_Absyndump_dump(_tag_arr("default: ",sizeof(
unsigned char),10));}else{Cyc_Absyndump_dump(_tag_arr("case ",sizeof(
unsigned char),6));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(
_tmp353));Cyc_Absyndump_dump_char((int)':');}Cyc_Absyndump_dumpstmt(_tmp351);}
Cyc_Absyndump_dump_char((int)'}');goto _LL703;_LL739: Cyc_Absyndump_dump(_tag_arr("fallthru;",
sizeof(unsigned char),10));goto _LL703;_LL741: Cyc_Absyndump_dump(_tag_arr("fallthru(",
sizeof(unsigned char),10));Cyc_Absyndump_dumpexps_prec(20,_tmp338);Cyc_Absyndump_dump_nospace(
_tag_arr(");",sizeof(unsigned char),3));goto _LL703;_LL743: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("try",sizeof(unsigned char),4));Cyc_Absyndump_dumpstmt(
_tmp33C);Cyc_Absyndump_dump(_tag_arr("catch {",sizeof(unsigned char),8));Cyc_Absyndump_dumpswitchclauses(
_tmp33A);Cyc_Absyndump_dump_char((int)'}');goto _LL703;_LL745: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("region<",sizeof(unsigned char),8));Cyc_Absyndump_dumptvar(
_tmp342);Cyc_Absyndump_dump(_tag_arr("> ",sizeof(unsigned char),3));Cyc_Absyndump_dumpqvar(
_tmp340->name);Cyc_Absyndump_dump(_tag_arr("{",sizeof(unsigned char),2));Cyc_Absyndump_dumpstmt(
_tmp33E);Cyc_Absyndump_dump(_tag_arr("}",sizeof(unsigned char),2));goto _LL703;
_LL747: Cyc_Absyndump_dump(_tag_arr("cut",sizeof(unsigned char),4));Cyc_Absyndump_dumpstmt(
_tmp344);goto _LL703;_LL749: Cyc_Absyndump_dump(_tag_arr("splice",sizeof(
unsigned char),7));Cyc_Absyndump_dumpstmt(_tmp346);goto _LL703;_LL703:;}struct
_tuple9{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(
struct _tuple9*dp){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*dp).f1,
_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",sizeof(unsigned char),2),
_tag_arr("=",sizeof(unsigned char),2));Cyc_Absyndump_dumppat((*dp).f2);}void Cyc_Absyndump_dumppat(
struct Cyc_Absyn_Pat*p){void*_tmp355=(void*)p->r;int _tmp379;void*_tmp37B;int
_tmp37D;void*_tmp37F;unsigned char _tmp381;struct _tagged_arr _tmp383;struct Cyc_Absyn_Vardecl*
_tmp385;struct Cyc_List_List*_tmp387;struct Cyc_Absyn_Pat*_tmp389;struct Cyc_Absyn_Vardecl*
_tmp38B;struct _tuple0*_tmp38D;struct Cyc_List_List*_tmp38F;struct Cyc_List_List*
_tmp391;struct _tuple0*_tmp393;struct Cyc_List_List*_tmp395;struct Cyc_List_List*
_tmp397;struct _tuple0*_tmp399;struct Cyc_List_List*_tmp39B;struct Cyc_List_List*
_tmp39D;struct Cyc_Absyn_Structdecl*_tmp39F;struct Cyc_List_List*_tmp3A1;struct Cyc_List_List*
_tmp3A3;struct Cyc_Absyn_Tunionfield*_tmp3A5;struct Cyc_Absyn_Enumfield*_tmp3A7;
struct Cyc_Absyn_Enumfield*_tmp3A9;_LL855: if(_tmp355 == (void*)Cyc_Absyn_Wild_p){
goto _LL856;}else{goto _LL857;}_LL857: if(_tmp355 == (void*)Cyc_Absyn_Null_p){goto
_LL858;}else{goto _LL859;}_LL859: if((unsigned int)_tmp355 > 2?*((int*)_tmp355)== 
Cyc_Absyn_Int_p: 0){_LL892: _tmp37B=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp355)->f1;
if(_tmp37B == (void*)Cyc_Absyn_Signed){goto _LL890;}else{goto _LL861;}_LL890:
_tmp379=((struct Cyc_Absyn_Int_p_struct*)_tmp355)->f2;goto _LL860;}else{goto _LL861;}
_LL861: if((unsigned int)_tmp355 > 2?*((int*)_tmp355)== Cyc_Absyn_Int_p: 0){_LL896:
_tmp37F=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp355)->f1;if(_tmp37F == (void*)
Cyc_Absyn_Unsigned){goto _LL894;}else{goto _LL863;}_LL894: _tmp37D=((struct Cyc_Absyn_Int_p_struct*)
_tmp355)->f2;goto _LL862;}else{goto _LL863;}_LL863: if((unsigned int)_tmp355 > 2?*((
int*)_tmp355)== Cyc_Absyn_Char_p: 0){_LL898: _tmp381=((struct Cyc_Absyn_Char_p_struct*)
_tmp355)->f1;goto _LL864;}else{goto _LL865;}_LL865: if((unsigned int)_tmp355 > 2?*((
int*)_tmp355)== Cyc_Absyn_Float_p: 0){_LL900: _tmp383=((struct Cyc_Absyn_Float_p_struct*)
_tmp355)->f1;goto _LL866;}else{goto _LL867;}_LL867: if((unsigned int)_tmp355 > 2?*((
int*)_tmp355)== Cyc_Absyn_Var_p: 0){_LL902: _tmp385=((struct Cyc_Absyn_Var_p_struct*)
_tmp355)->f1;goto _LL868;}else{goto _LL869;}_LL869: if((unsigned int)_tmp355 > 2?*((
int*)_tmp355)== Cyc_Absyn_Tuple_p: 0){_LL904: _tmp387=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp355)->f1;goto _LL870;}else{goto _LL871;}_LL871: if((unsigned int)_tmp355 > 2?*((
int*)_tmp355)== Cyc_Absyn_Pointer_p: 0){_LL906: _tmp389=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp355)->f1;goto _LL872;}else{goto _LL873;}_LL873: if((unsigned int)_tmp355 > 2?*((
int*)_tmp355)== Cyc_Absyn_Reference_p: 0){_LL908: _tmp38B=((struct Cyc_Absyn_Reference_p_struct*)
_tmp355)->f1;goto _LL874;}else{goto _LL875;}_LL875: if((unsigned int)_tmp355 > 2?*((
int*)_tmp355)== Cyc_Absyn_UnknownId_p: 0){_LL910: _tmp38D=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp355)->f1;goto _LL876;}else{goto _LL877;}_LL877: if((unsigned int)_tmp355 > 2?*((
int*)_tmp355)== Cyc_Absyn_UnknownCall_p: 0){_LL916: _tmp393=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp355)->f1;goto _LL914;_LL914: _tmp391=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp355)->f2;goto _LL912;_LL912: _tmp38F=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp355)->f3;goto _LL878;}else{goto _LL879;}_LL879: if((unsigned int)_tmp355 > 2?*((
int*)_tmp355)== Cyc_Absyn_UnknownFields_p: 0){_LL922: _tmp399=((struct Cyc_Absyn_UnknownFields_p_struct*)
_tmp355)->f1;goto _LL920;_LL920: _tmp397=((struct Cyc_Absyn_UnknownFields_p_struct*)
_tmp355)->f2;goto _LL918;_LL918: _tmp395=((struct Cyc_Absyn_UnknownFields_p_struct*)
_tmp355)->f3;goto _LL880;}else{goto _LL881;}_LL881: if((unsigned int)_tmp355 > 2?*((
int*)_tmp355)== Cyc_Absyn_Struct_p: 0){_LL928: _tmp39F=((struct Cyc_Absyn_Struct_p_struct*)
_tmp355)->f1;goto _LL926;_LL926: _tmp39D=((struct Cyc_Absyn_Struct_p_struct*)
_tmp355)->f3;goto _LL924;_LL924: _tmp39B=((struct Cyc_Absyn_Struct_p_struct*)
_tmp355)->f4;goto _LL882;}else{goto _LL883;}_LL883: if((unsigned int)_tmp355 > 2?*((
int*)_tmp355)== Cyc_Absyn_Tunion_p: 0){_LL934: _tmp3A5=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp355)->f2;goto _LL932;_LL932: _tmp3A3=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp355)->f3;goto _LL930;_LL930: _tmp3A1=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp355)->f4;goto _LL884;}else{goto _LL885;}_LL885: if((unsigned int)_tmp355 > 2?*((
int*)_tmp355)== Cyc_Absyn_Enum_p: 0){_LL936: _tmp3A7=((struct Cyc_Absyn_Enum_p_struct*)
_tmp355)->f2;goto _LL886;}else{goto _LL887;}_LL887: if((unsigned int)_tmp355 > 2?*((
int*)_tmp355)== Cyc_Absyn_AnonEnum_p: 0){_LL938: _tmp3A9=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp355)->f2;goto _LL888;}else{goto _LL854;}_LL856: Cyc_Absyndump_dump_char((int)'_');
goto _LL854;_LL858: Cyc_Absyndump_dump(_tag_arr("NULL",sizeof(unsigned char),5));
goto _LL854;_LL860: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp3AC;_tmp3AC.tag=Cyc_Std_Int_pa;_tmp3AC.f1=(int)((unsigned int)_tmp379);{void*
_tmp3AB[1]={& _tmp3AC};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),
_tag_arr(_tmp3AB,sizeof(void*),1));}}));goto _LL854;_LL862: Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp3AE;_tmp3AE.tag=Cyc_Std_Int_pa;
_tmp3AE.f1=(unsigned int)_tmp37D;{void*_tmp3AD[1]={& _tmp3AE};Cyc_Std_aprintf(
_tag_arr("%u",sizeof(unsigned char),3),_tag_arr(_tmp3AD,sizeof(void*),1));}}));
goto _LL854;_LL864: Cyc_Absyndump_dump(_tag_arr("'",sizeof(unsigned char),2));Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape(_tmp381));Cyc_Absyndump_dump_nospace(_tag_arr("'",
sizeof(unsigned char),2));goto _LL854;_LL866: Cyc_Absyndump_dump(_tmp383);goto
_LL854;_LL868: Cyc_Absyndump_dumpqvar(_tmp385->name);goto _LL854;_LL870:((void(*)(
void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
_tmp387,_tag_arr("$(",sizeof(unsigned char),3),_tag_arr(")",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LL854;_LL872: Cyc_Absyndump_dump(
_tag_arr("&",sizeof(unsigned char),2));Cyc_Absyndump_dumppat(_tmp389);goto _LL854;
_LL874: Cyc_Absyndump_dump(_tag_arr("*",sizeof(unsigned char),2));Cyc_Absyndump_dumpqvar(
_tmp38B->name);goto _LL854;_LL876: Cyc_Absyndump_dumpqvar(_tmp38D);goto _LL854;
_LL878: Cyc_Absyndump_dumpqvar(_tmp393);Cyc_Absyndump_dumptvars(_tmp391);((void(*)(
void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
_tmp38F,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LL854;_LL880: Cyc_Absyndump_dumpqvar(
_tmp399);Cyc_Absyndump_dumptvars(_tmp397);((void(*)(void(*f)(struct _tuple9*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp395,_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LL854;_LL882: if(_tmp39F->name != 0){Cyc_Absyndump_dumpqvar((
struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp39F->name))->v);}Cyc_Absyndump_dumptvars(
_tmp39D);((void(*)(void(*f)(struct _tuple9*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp,_tmp39B,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));goto _LL854;
_LL884: Cyc_Absyndump_dumpqvar(_tmp3A5->name);Cyc_Absyndump_dumptvars(_tmp3A3);
if(_tmp3A1 != 0){((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp3A1,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}goto _LL854;
_LL886: Cyc_Absyndump_dumpqvar(_tmp3A7->name);goto _LL854;_LL888: Cyc_Absyndump_dumpqvar(
_tmp3A9->name);goto _LL854;_LL854:;}void Cyc_Absyndump_dumptunionfield(struct Cyc_Absyn_Tunionfield*
ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->typs != 0){Cyc_Absyndump_dumpargs(ef->typs);}}
void Cyc_Absyndump_dumptunionfields(struct Cyc_List_List*fields){((void(*)(void(*f)(
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dumptunionfield,fields,_tag_arr(",",sizeof(unsigned char),2));}
void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){Cyc_Absyndump_dumpqvar(
ef->name);if(ef->tag != 0){Cyc_Absyndump_dump(_tag_arr(" = ",sizeof(unsigned char),
4));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List*fields){((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,
fields,_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpstructfields(
struct Cyc_List_List*fields){for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Structfield
_tmp3B1;struct Cyc_List_List*_tmp3B2;struct Cyc_Absyn_Exp*_tmp3B4;void*_tmp3B6;
struct Cyc_Absyn_Tqual _tmp3B8;struct _tagged_arr*_tmp3BA;struct Cyc_Absyn_Structfield*
_tmp3AF=(struct Cyc_Absyn_Structfield*)fields->hd;_tmp3B1=*_tmp3AF;_LL955: _tmp3BA=
_tmp3B1.name;goto _LL953;_LL953: _tmp3B8=_tmp3B1.tq;goto _LL951;_LL951: _tmp3B6=(
void*)_tmp3B1.type;goto _LL949;_LL949: _tmp3B4=_tmp3B1.width;goto _LL947;_LL947:
_tmp3B2=_tmp3B1.attributes;goto _LL944;_LL944:((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _tagged_arr*),struct _tagged_arr*))Cyc_Absyndump_dumptqtd)(
_tmp3B8,_tmp3B6,Cyc_Absyndump_dump_str,_tmp3BA);Cyc_Absyndump_dumpatts(_tmp3B2);
if(_tmp3B4 != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp((struct
Cyc_Absyn_Exp*)_check_null(_tmp3B4));}Cyc_Absyndump_dump_semi();}}void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(td->name);Cyc_Absyndump_dumptvars(
td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple10{void*f1;struct
_tuple0*f2;};static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple10*pr){{void*
_tmp3BC=(*pr).f1;_LL958: if(_tmp3BC == (void*)Cyc_Absyn_Unused_att){goto _LL959;}
else{goto _LL960;}_LL960: if(_tmp3BC == (void*)Cyc_Absyn_Stdcall_att){goto _LL961;}
else{goto _LL962;}_LL962: if(_tmp3BC == (void*)Cyc_Absyn_Cdecl_att){goto _LL963;}
else{goto _LL964;}_LL964: if(_tmp3BC == (void*)Cyc_Absyn_Fastcall_att){goto _LL965;}
else{goto _LL966;}_LL966: goto _LL967;_LL959: goto _LL957;_LL961: Cyc_Absyndump_dump(
_tag_arr("_stdcall",sizeof(unsigned char),9));goto _LL957;_LL963: Cyc_Absyndump_dump(
_tag_arr("_cdecl",sizeof(unsigned char),7));goto _LL957;_LL965: Cyc_Absyndump_dump(
_tag_arr("_fastcall",sizeof(unsigned char),10));goto _LL957;_LL967: goto _LL957;
_LL957:;}Cyc_Absyndump_dumpqvar((*pr).f2);}static void Cyc_Absyndump_dump_callconv_fdqvar(
struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dump_callconv(fd->attributes);Cyc_Absyndump_dumpqvar(
fd->name);}static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){for(0;vds != 
0;vds=vds->tl){Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
if(vds->tl != 0){Cyc_Absyndump_dump_char((int)',');}}}void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Vardecl
_tmp3CA;struct Cyc_List_List*_tmp3CB;struct Cyc_Absyn_Exp*_tmp3CD;void*_tmp3CF;
struct Cyc_Absyn_Tqual _tmp3D1;struct _tuple0*_tmp3D3;void*_tmp3D5;struct Cyc_Absyn_Vardecl*
_tmp3C8=vd;_tmp3CA=*_tmp3C8;_LL982: _tmp3D5=(void*)_tmp3CA.sc;goto _LL980;_LL980:
_tmp3D3=_tmp3CA.name;goto _LL978;_LL978: _tmp3D1=_tmp3CA.tq;goto _LL976;_LL976:
_tmp3CF=(void*)_tmp3CA.type;goto _LL974;_LL974: _tmp3CD=_tmp3CA.initializer;goto
_LL972;_LL972: _tmp3CB=_tmp3CA.attributes;goto _LL969;_LL969: Cyc_Absyndump_dumploc(
loc);if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dumpatts(_tmp3CB);Cyc_Absyndump_dumpscope(
_tmp3D5);{struct Cyc_List_List*_tmp3D9;void*_tmp3DB;struct Cyc_Absyn_Tqual _tmp3DD;
struct _tuple4 _tmp3D7=Cyc_Absynpp_to_tms(_tmp3D1,_tmp3CF);_LL990: _tmp3DD=_tmp3D7.f1;
goto _LL988;_LL988: _tmp3DB=_tmp3D7.f2;goto _LL986;_LL986: _tmp3D9=_tmp3D7.f3;goto
_LL984;_LL984: {void*call_conv=(void*)Cyc_Absyn_Unused_att;{struct Cyc_List_List*
tms2=_tmp3D9;for(0;tms2 != 0;tms2=tms2->tl){void*_tmp3DF=(void*)tms2->hd;struct
Cyc_List_List*_tmp3E5;_LL993: if((unsigned int)_tmp3DF > 1?*((int*)_tmp3DF)== Cyc_Absyn_Attributes_mod:
0){_LL998: _tmp3E5=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp3DF)->f2;goto
_LL994;}else{goto _LL995;}_LL995: goto _LL996;_LL994: for(0;_tmp3E5 != 0;_tmp3E5=
_tmp3E5->tl){void*_tmp3E7=(void*)_tmp3E5->hd;_LL1001: if(_tmp3E7 == (void*)Cyc_Absyn_Stdcall_att){
goto _LL1002;}else{goto _LL1003;}_LL1003: if(_tmp3E7 == (void*)Cyc_Absyn_Cdecl_att){
goto _LL1004;}else{goto _LL1005;}_LL1005: if(_tmp3E7 == (void*)Cyc_Absyn_Fastcall_att){
goto _LL1006;}else{goto _LL1007;}_LL1007: goto _LL1008;_LL1002: call_conv=(void*)Cyc_Absyn_Stdcall_att;
goto _LL1000;_LL1004: call_conv=(void*)Cyc_Absyn_Cdecl_att;goto _LL1000;_LL1006:
call_conv=(void*)Cyc_Absyn_Fastcall_att;goto _LL1000;_LL1008: goto _LL1000;_LL1000:;}
goto _LL992;_LL996: goto _LL992;_LL992:;}}Cyc_Absyndump_dumptq(_tmp3DD);Cyc_Absyndump_dumpntyp(
_tmp3DB);{struct _tuple10 _tmp3F1=({struct _tuple10 _tmp3F2;_tmp3F2.f1=call_conv;
_tmp3F2.f2=_tmp3D3;_tmp3F2;});((void(*)(struct Cyc_List_List*tms,void(*f)(struct
_tuple10*),struct _tuple10*a))Cyc_Absyndump_dumptms)(Cyc_List_imp_rev(_tmp3D9),
Cyc_Absyndump_dump_callconv_qvar,& _tmp3F1);}}}}else{Cyc_Absyndump_dumpscope(
_tmp3D5);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct
_tuple0*))Cyc_Absyndump_dumptqtd)(_tmp3D1,_tmp3CF,Cyc_Absyndump_dumpqvar,_tmp3D3);
Cyc_Absyndump_dumpatts(_tmp3CB);}if(_tmp3CD != 0){Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp3CD));}Cyc_Absyndump_dump_semi();}
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){void*_tmp3F3=(void*)d->r;
struct Cyc_Absyn_Fndecl*_tmp40D;struct Cyc_Absyn_Structdecl*_tmp40F;struct Cyc_Absyn_Uniondecl*
_tmp411;struct Cyc_Absyn_Vardecl*_tmp413;struct Cyc_Absyn_Tuniondecl*_tmp415;
struct Cyc_Absyn_Tuniondecl _tmp417;int _tmp418;struct Cyc_Core_Opt*_tmp41A;struct
Cyc_List_List*_tmp41C;struct _tuple0*_tmp41E;void*_tmp420;struct Cyc_Absyn_Enumdecl*
_tmp422;struct Cyc_Absyn_Enumdecl _tmp424;struct Cyc_Core_Opt*_tmp425;struct _tuple0*
_tmp427;void*_tmp429;struct Cyc_Absyn_Exp*_tmp42B;struct Cyc_Absyn_Pat*_tmp42D;
struct Cyc_List_List*_tmp42F;struct Cyc_Absyn_Typedefdecl*_tmp431;struct Cyc_List_List*
_tmp433;struct _tagged_arr*_tmp435;struct Cyc_List_List*_tmp437;struct _tuple0*
_tmp439;struct Cyc_List_List*_tmp43B;_LL1013: if(*((int*)_tmp3F3)== Cyc_Absyn_Fn_d){
_LL1038: _tmp40D=((struct Cyc_Absyn_Fn_d_struct*)_tmp3F3)->f1;goto _LL1014;}else{
goto _LL1015;}_LL1015: if(*((int*)_tmp3F3)== Cyc_Absyn_Struct_d){_LL1040: _tmp40F=((
struct Cyc_Absyn_Struct_d_struct*)_tmp3F3)->f1;goto _LL1016;}else{goto _LL1017;}
_LL1017: if(*((int*)_tmp3F3)== Cyc_Absyn_Union_d){_LL1042: _tmp411=((struct Cyc_Absyn_Union_d_struct*)
_tmp3F3)->f1;goto _LL1018;}else{goto _LL1019;}_LL1019: if(*((int*)_tmp3F3)== Cyc_Absyn_Var_d){
_LL1044: _tmp413=((struct Cyc_Absyn_Var_d_struct*)_tmp3F3)->f1;goto _LL1020;}else{
goto _LL1021;}_LL1021: if(*((int*)_tmp3F3)== Cyc_Absyn_Tunion_d){_LL1046: _tmp415=((
struct Cyc_Absyn_Tunion_d_struct*)_tmp3F3)->f1;_tmp417=*_tmp415;_LL1057: _tmp420=(
void*)_tmp417.sc;goto _LL1055;_LL1055: _tmp41E=_tmp417.name;goto _LL1053;_LL1053:
_tmp41C=_tmp417.tvs;goto _LL1051;_LL1051: _tmp41A=_tmp417.fields;goto _LL1049;
_LL1049: _tmp418=_tmp417.is_xtunion;goto _LL1022;}else{goto _LL1023;}_LL1023: if(*((
int*)_tmp3F3)== Cyc_Absyn_Enum_d){_LL1059: _tmp422=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3F3)->f1;_tmp424=*_tmp422;_LL1066: _tmp429=(void*)_tmp424.sc;goto _LL1064;
_LL1064: _tmp427=_tmp424.name;goto _LL1062;_LL1062: _tmp425=_tmp424.fields;goto
_LL1024;}else{goto _LL1025;}_LL1025: if(*((int*)_tmp3F3)== Cyc_Absyn_Let_d){
_LL1070: _tmp42D=((struct Cyc_Absyn_Let_d_struct*)_tmp3F3)->f1;goto _LL1068;_LL1068:
_tmp42B=((struct Cyc_Absyn_Let_d_struct*)_tmp3F3)->f4;goto _LL1026;}else{goto
_LL1027;}_LL1027: if(*((int*)_tmp3F3)== Cyc_Absyn_Letv_d){_LL1072: _tmp42F=((
struct Cyc_Absyn_Letv_d_struct*)_tmp3F3)->f1;goto _LL1028;}else{goto _LL1029;}
_LL1029: if(*((int*)_tmp3F3)== Cyc_Absyn_Typedef_d){_LL1074: _tmp431=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp3F3)->f1;goto _LL1030;}else{goto _LL1031;}_LL1031: if(*((int*)_tmp3F3)== Cyc_Absyn_Namespace_d){
_LL1078: _tmp435=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3F3)->f1;goto _LL1076;
_LL1076: _tmp433=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3F3)->f2;goto _LL1032;}
else{goto _LL1033;}_LL1033: if(*((int*)_tmp3F3)== Cyc_Absyn_Using_d){_LL1082:
_tmp439=((struct Cyc_Absyn_Using_d_struct*)_tmp3F3)->f1;goto _LL1080;_LL1080:
_tmp437=((struct Cyc_Absyn_Using_d_struct*)_tmp3F3)->f2;goto _LL1034;}else{goto
_LL1035;}_LL1035: if(*((int*)_tmp3F3)== Cyc_Absyn_ExternC_d){_LL1084: _tmp43B=((
struct Cyc_Absyn_ExternC_d_struct*)_tmp3F3)->f1;goto _LL1036;}else{goto _LL1012;}
_LL1014: Cyc_Absyndump_dumploc(d->loc);if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dumpatts(
_tmp40D->attributes);}if(_tmp40D->is_inline){if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dump(
_tag_arr("__inline",sizeof(unsigned char),9));}else{Cyc_Absyndump_dump(_tag_arr("inline",
sizeof(unsigned char),7));}}Cyc_Absyndump_dumpscope((void*)_tmp40D->sc);{void*t=(
void*)({struct Cyc_Absyn_FnType_struct*_tmp43E=_cycalloc(sizeof(struct Cyc_Absyn_FnType_struct));
_tmp43E[0]=({struct Cyc_Absyn_FnType_struct _tmp43F;_tmp43F.tag=Cyc_Absyn_FnType;
_tmp43F.f1=({struct Cyc_Absyn_FnInfo _tmp440;_tmp440.tvars=_tmp40D->tvs;_tmp440.effect=
_tmp40D->effect;_tmp440.ret_typ=(void*)((void*)_tmp40D->ret_type);_tmp440.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple3*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp40D->args);_tmp440.c_varargs=_tmp40D->c_varargs;
_tmp440.cyc_varargs=_tmp40D->cyc_varargs;_tmp440.rgn_po=_tmp40D->rgn_po;_tmp440.attributes=
0;_tmp440;});_tmp43F;});_tmp43E;});((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _tmp43D;_tmp43D.q_const=0;_tmp43D.q_volatile=0;_tmp43D.q_restrict=
0;_tmp43D;}),t,Cyc_Absyndump_to_VC? Cyc_Absyndump_dump_callconv_fdqvar: Cyc_Absyndump_dump_atts_qvar,
_tmp40D);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp40D->body);
Cyc_Absyndump_dump_char((int)'}');goto _LL1012;}_LL1016: Cyc_Absyndump_dumpscope((
void*)_tmp40F->sc);Cyc_Absyndump_dump(_tag_arr("struct",sizeof(unsigned char),7));
if(_tmp40F->name != 0){Cyc_Absyndump_dumpqvar((struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(_tmp40F->name))->v);}Cyc_Absyndump_dumptvars(_tmp40F->tvs);if(
_tmp40F->fields == 0){Cyc_Absyndump_dump_semi();}else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstructfields((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp40F->fields))->v);Cyc_Absyndump_dump(_tag_arr("}",sizeof(
unsigned char),2));Cyc_Absyndump_dumpatts(_tmp40F->attributes);Cyc_Absyndump_dump(
_tag_arr(";",sizeof(unsigned char),2));}goto _LL1012;_LL1018: Cyc_Absyndump_dumpscope((
void*)_tmp411->sc);Cyc_Absyndump_dump(_tag_arr("union",sizeof(unsigned char),6));
if(_tmp411->name != 0){Cyc_Absyndump_dumpqvar((struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(_tmp411->name))->v);}Cyc_Absyndump_dumptvars(_tmp411->tvs);if(
_tmp411->fields == 0){Cyc_Absyndump_dump_semi();}else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstructfields((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp411->fields))->v);Cyc_Absyndump_dump(_tag_arr("}",sizeof(
unsigned char),2));Cyc_Absyndump_dumpatts(_tmp411->attributes);Cyc_Absyndump_dump(
_tag_arr(";",sizeof(unsigned char),2));}goto _LL1012;_LL1020: Cyc_Absyndump_dumpvardecl(
_tmp413,d->loc);goto _LL1012;_LL1022: Cyc_Absyndump_dumpscope(_tmp420);if(_tmp418){
Cyc_Absyndump_dump(_tag_arr("xtunion ",sizeof(unsigned char),9));}else{Cyc_Absyndump_dump(
_tag_arr("tunion ",sizeof(unsigned char),8));}Cyc_Absyndump_dumpqvar(_tmp41E);
Cyc_Absyndump_dumptvars(_tmp41C);if(_tmp41A == 0){Cyc_Absyndump_dump_semi();}
else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumptunionfields((struct Cyc_List_List*)
_tmp41A->v);Cyc_Absyndump_dump_nospace(_tag_arr("};",sizeof(unsigned char),3));}
goto _LL1012;_LL1024: Cyc_Absyndump_dumpscope(_tmp429);Cyc_Absyndump_dump(_tag_arr("enum ",
sizeof(unsigned char),6));Cyc_Absyndump_dumpqvar(_tmp427);if(_tmp425 == 0){Cyc_Absyndump_dump_semi();}
else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)
_tmp425->v);Cyc_Absyndump_dump_nospace(_tag_arr("};",sizeof(unsigned char),3));}
return;_LL1026: Cyc_Absyndump_dump(_tag_arr("let",sizeof(unsigned char),4));Cyc_Absyndump_dumppat(
_tmp42D);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp42B);Cyc_Absyndump_dump_semi();
goto _LL1012;_LL1028: Cyc_Absyndump_dump(_tag_arr("let ",sizeof(unsigned char),5));
Cyc_Absyndump_dumpids(_tmp42F);Cyc_Absyndump_dump_semi();goto _LL1012;_LL1030: if(
! Cyc_Absyndump_expand_typedefs? 1: Cyc_Absynpp_is_anon_structtype((void*)_tmp431->defn)){
Cyc_Absyndump_dump(_tag_arr("typedef",sizeof(unsigned char),8));((void(*)(struct
Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))
Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp441;_tmp441.q_const=0;_tmp441.q_volatile=
0;_tmp441.q_restrict=0;_tmp441;}),(void*)_tmp431->defn,Cyc_Absyndump_dumptypedefname,
_tmp431);Cyc_Absyndump_dump_semi();}goto _LL1012;_LL1032: Cyc_Absyndump_dump(
_tag_arr("namespace",sizeof(unsigned char),10));Cyc_Absyndump_dump_str(_tmp435);
Cyc_Absyndump_dump_char((int)'{');for(0;_tmp433 != 0;_tmp433=_tmp433->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)_tmp433->hd);}Cyc_Absyndump_dump_char((int)'}');goto
_LL1012;_LL1034: Cyc_Absyndump_dump(_tag_arr("using",sizeof(unsigned char),6));
Cyc_Absyndump_dumpqvar(_tmp439);Cyc_Absyndump_dump_char((int)'{');for(0;_tmp437
!= 0;_tmp437=_tmp437->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp437->hd);}
Cyc_Absyndump_dump_char((int)'}');goto _LL1012;_LL1036: Cyc_Absyndump_dump(
_tag_arr("extern \"C\" {",sizeof(unsigned char),13));for(0;_tmp43B != 0;_tmp43B=
_tmp43B->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp43B->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1012;_LL1012:;}static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*
e){unsigned int i=Cyc_Evexp_eval_const_uint_exp(e);if(i != 1){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}void Cyc_Absyndump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*
_tmp442=(void*)tms->hd;struct Cyc_Absyn_Tqual _tmp454;void*_tmp456;void*_tmp458;
struct Cyc_Absyn_Exp*_tmp45A;struct Cyc_Absyn_Tqual _tmp45C;void*_tmp45E;void*
_tmp460;struct Cyc_Absyn_Exp*_tmp462;struct Cyc_Absyn_Tqual _tmp464;void*_tmp466;
void*_tmp468;struct Cyc_Absyn_Tqual _tmp46A;void*_tmp46C;struct Cyc_Absyn_Tvar*
_tmp46E;void*_tmp470;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_Absyn_Tqual _tmp474;
void*_tmp476;struct Cyc_Absyn_Tvar*_tmp478;void*_tmp47A;struct Cyc_Absyn_Exp*
_tmp47C;struct Cyc_Absyn_Tqual _tmp47E;void*_tmp480;struct Cyc_Absyn_Tvar*_tmp482;
void*_tmp484;_LL1092: if((unsigned int)_tmp442 > 1?*((int*)_tmp442)== Cyc_Absyn_Pointer_mod:
0){_LL1113: _tmp458=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp442)->f1;if((
unsigned int)_tmp458 > 1?*((int*)_tmp458)== Cyc_Absyn_Nullable_ps: 0){_LL1115:
_tmp45A=((struct Cyc_Absyn_Nullable_ps_struct*)_tmp458)->f1;goto _LL1111;}else{
goto _LL1094;}_LL1111: _tmp456=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp442)->f2;if(_tmp456 == (void*)Cyc_Absyn_HeapRgn){goto _LL1109;}else{goto
_LL1094;}_LL1109: _tmp454=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp442)->f3;goto
_LL1093;}else{goto _LL1094;}_LL1094: if((unsigned int)_tmp442 > 1?*((int*)_tmp442)
== Cyc_Absyn_Pointer_mod: 0){_LL1121: _tmp460=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp442)->f1;if((unsigned int)_tmp460 > 1?*((int*)_tmp460)== Cyc_Absyn_NonNullable_ps:
0){_LL1123: _tmp462=((struct Cyc_Absyn_NonNullable_ps_struct*)_tmp460)->f1;goto
_LL1119;}else{goto _LL1096;}_LL1119: _tmp45E=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp442)->f2;if(_tmp45E == (void*)Cyc_Absyn_HeapRgn){goto _LL1117;}else{goto
_LL1096;}_LL1117: _tmp45C=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp442)->f3;goto
_LL1095;}else{goto _LL1096;}_LL1096: if((unsigned int)_tmp442 > 1?*((int*)_tmp442)
== Cyc_Absyn_Pointer_mod: 0){_LL1129: _tmp468=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp442)->f1;if(_tmp468 == (void*)Cyc_Absyn_TaggedArray_ps){goto _LL1127;}else{
goto _LL1098;}_LL1127: _tmp466=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp442)->f2;if(_tmp466 == (void*)Cyc_Absyn_HeapRgn){goto _LL1125;}else{goto
_LL1098;}_LL1125: _tmp464=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp442)->f3;goto
_LL1097;}else{goto _LL1098;}_LL1098: if((unsigned int)_tmp442 > 1?*((int*)_tmp442)
== Cyc_Absyn_Pointer_mod: 0){_LL1137: _tmp470=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp442)->f1;if((unsigned int)_tmp470 > 1?*((int*)_tmp470)== Cyc_Absyn_Nullable_ps:
0){_LL1139: _tmp472=((struct Cyc_Absyn_Nullable_ps_struct*)_tmp470)->f1;goto
_LL1133;}else{goto _LL1100;}_LL1133: _tmp46C=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp442)->f2;if((unsigned int)_tmp46C > 3?*((int*)_tmp46C)== Cyc_Absyn_VarType: 0){
_LL1135: _tmp46E=((struct Cyc_Absyn_VarType_struct*)_tmp46C)->f1;goto _LL1131;}
else{goto _LL1100;}_LL1131: _tmp46A=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp442)->f3;
goto _LL1099;}else{goto _LL1100;}_LL1100: if((unsigned int)_tmp442 > 1?*((int*)
_tmp442)== Cyc_Absyn_Pointer_mod: 0){_LL1147: _tmp47A=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp442)->f1;if((unsigned int)_tmp47A > 1?*((int*)_tmp47A)== Cyc_Absyn_NonNullable_ps:
0){_LL1149: _tmp47C=((struct Cyc_Absyn_NonNullable_ps_struct*)_tmp47A)->f1;goto
_LL1143;}else{goto _LL1102;}_LL1143: _tmp476=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp442)->f2;if((unsigned int)_tmp476 > 3?*((int*)_tmp476)== Cyc_Absyn_VarType: 0){
_LL1145: _tmp478=((struct Cyc_Absyn_VarType_struct*)_tmp476)->f1;goto _LL1141;}
else{goto _LL1102;}_LL1141: _tmp474=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp442)->f3;
goto _LL1101;}else{goto _LL1102;}_LL1102: if((unsigned int)_tmp442 > 1?*((int*)
_tmp442)== Cyc_Absyn_Pointer_mod: 0){_LL1157: _tmp484=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp442)->f1;if(_tmp484 == (void*)Cyc_Absyn_TaggedArray_ps){goto _LL1153;}else{
goto _LL1104;}_LL1153: _tmp480=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp442)->f2;if((unsigned int)_tmp480 > 3?*((int*)_tmp480)== Cyc_Absyn_VarType: 0){
_LL1155: _tmp482=((struct Cyc_Absyn_VarType_struct*)_tmp480)->f1;goto _LL1151;}
else{goto _LL1104;}_LL1151: _tmp47E=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp442)->f3;
goto _LL1103;}else{goto _LL1104;}_LL1104: if((unsigned int)_tmp442 > 1?*((int*)
_tmp442)== Cyc_Absyn_Pointer_mod: 0){goto _LL1105;}else{goto _LL1106;}_LL1106: goto
_LL1107;_LL1093: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dump_upperbound(
_tmp45A);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1095: Cyc_Absyndump_dump_char((
int)'@');Cyc_Absyndump_dump_upperbound(_tmp462);Cyc_Absyndump_dumptms(tms->tl,f,
a);return;_LL1097: Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumptms(tms->tl,
f,a);return;_LL1099: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dump_upperbound(
_tmp472);Cyc_Absyndump_dump_str(_tmp46E->name);Cyc_Absyndump_dumptms(tms->tl,f,a);
return;_LL1101: Cyc_Absyndump_dump_char((int)'@');Cyc_Absyndump_dump_upperbound(
_tmp47C);Cyc_Absyndump_dump_str(_tmp478->name);Cyc_Absyndump_dumptms(tms->tl,f,a);
return;_LL1103: Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dump_str(_tmp482->name);
Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1105:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp486=_cycalloc(sizeof(struct Cyc_Core_Impossible_struct));_tmp486[0]=({struct
Cyc_Core_Impossible_struct _tmp487;_tmp487.tag=Cyc_Core_Impossible;_tmp487.f1=
_tag_arr("dumptms: bad Pointer_mod",sizeof(unsigned char),25);_tmp487;});_tmp486;}));
_LL1107: {int next_is_pointer=0;if(tms->tl != 0){void*_tmp488=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;_LL1162: if((unsigned int)_tmp488 > 1?*((int*)_tmp488)== 
Cyc_Absyn_Pointer_mod: 0){goto _LL1163;}else{goto _LL1164;}_LL1164: goto _LL1165;
_LL1163: next_is_pointer=1;goto _LL1161;_LL1165: goto _LL1161;_LL1161:;}if(
next_is_pointer){Cyc_Absyndump_dump_char((int)'(');}Cyc_Absyndump_dumptms(tms->tl,
f,a);if(next_is_pointer){Cyc_Absyndump_dump_char((int)')');}{void*_tmp48E=(void*)
tms->hd;struct Cyc_Absyn_Exp*_tmp49E;void*_tmp4A0;struct Cyc_List_List*_tmp4A2;
struct Cyc_Core_Opt*_tmp4A4;struct Cyc_Absyn_VarargInfo*_tmp4A6;int _tmp4A8;struct
Cyc_List_List*_tmp4AA;void*_tmp4AC;struct Cyc_Position_Segment*_tmp4AE;struct Cyc_List_List*
_tmp4B0;int _tmp4B2;struct Cyc_Position_Segment*_tmp4B4;struct Cyc_List_List*
_tmp4B6;struct Cyc_List_List*_tmp4B8;void*_tmp4BA;void*_tmp4BC;_LL1168: if(_tmp48E
== (void*)Cyc_Absyn_Carray_mod){goto _LL1169;}else{goto _LL1170;}_LL1170: if((
unsigned int)_tmp48E > 1?*((int*)_tmp48E)== Cyc_Absyn_ConstArray_mod: 0){_LL1183:
_tmp49E=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp48E)->f1;goto _LL1171;}else{
goto _LL1172;}_LL1172: if((unsigned int)_tmp48E > 1?*((int*)_tmp48E)== Cyc_Absyn_Function_mod:
0){_LL1185: _tmp4A0=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp48E)->f1;
if(*((int*)_tmp4A0)== Cyc_Absyn_WithTypes){_LL1195: _tmp4AA=((struct Cyc_Absyn_WithTypes_struct*)
_tmp4A0)->f1;goto _LL1193;_LL1193: _tmp4A8=((struct Cyc_Absyn_WithTypes_struct*)
_tmp4A0)->f2;goto _LL1191;_LL1191: _tmp4A6=((struct Cyc_Absyn_WithTypes_struct*)
_tmp4A0)->f3;goto _LL1189;_LL1189: _tmp4A4=((struct Cyc_Absyn_WithTypes_struct*)
_tmp4A0)->f4;goto _LL1187;_LL1187: _tmp4A2=((struct Cyc_Absyn_WithTypes_struct*)
_tmp4A0)->f5;goto _LL1173;}else{goto _LL1174;}}else{goto _LL1174;}_LL1174: if((
unsigned int)_tmp48E > 1?*((int*)_tmp48E)== Cyc_Absyn_Function_mod: 0){_LL1197:
_tmp4AC=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp48E)->f1;if(*((int*)
_tmp4AC)== Cyc_Absyn_NoTypes){_LL1201: _tmp4B0=((struct Cyc_Absyn_NoTypes_struct*)
_tmp4AC)->f1;goto _LL1199;_LL1199: _tmp4AE=((struct Cyc_Absyn_NoTypes_struct*)
_tmp4AC)->f2;goto _LL1175;}else{goto _LL1176;}}else{goto _LL1176;}_LL1176: if((
unsigned int)_tmp48E > 1?*((int*)_tmp48E)== Cyc_Absyn_TypeParams_mod: 0){_LL1207:
_tmp4B6=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp48E)->f1;goto _LL1205;
_LL1205: _tmp4B4=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp48E)->f2;goto
_LL1203;_LL1203: _tmp4B2=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp48E)->f3;
goto _LL1177;}else{goto _LL1178;}_LL1178: if((unsigned int)_tmp48E > 1?*((int*)
_tmp48E)== Cyc_Absyn_Attributes_mod: 0){_LL1209: _tmp4B8=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmp48E)->f2;goto _LL1179;}else{goto _LL1180;}_LL1180: if((unsigned int)_tmp48E > 1?*((
int*)_tmp48E)== Cyc_Absyn_Pointer_mod: 0){_LL1213: _tmp4BC=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp48E)->f1;goto _LL1211;_LL1211: _tmp4BA=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp48E)->f2;goto _LL1181;}else{goto _LL1167;}_LL1169: Cyc_Absyndump_dump(_tag_arr("[]",
sizeof(unsigned char),3));goto _LL1167;_LL1171: Cyc_Absyndump_dump_char((int)'[');
Cyc_Absyndump_dumpexp(_tmp49E);Cyc_Absyndump_dump_char((int)']');goto _LL1167;
_LL1173: Cyc_Absyndump_dumpfunargs(_tmp4AA,_tmp4A8,_tmp4A6,_tmp4A4,_tmp4A2);goto
_LL1167;_LL1175:((void(*)(void(*f)(struct _tagged_arr*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dump_str,_tmp4B0,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));goto _LL1167;
_LL1177: if(_tmp4B2){Cyc_Absyndump_dumpkindedtvars(_tmp4B6);}else{Cyc_Absyndump_dumptvars(
_tmp4B6);}goto _LL1167;_LL1179: Cyc_Absyndump_dumpatts(_tmp4B8);goto _LL1167;
_LL1181:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp4BE=_cycalloc(
sizeof(struct Cyc_Core_Impossible_struct));_tmp4BE[0]=({struct Cyc_Core_Impossible_struct
_tmp4BF;_tmp4BF.tag=Cyc_Core_Impossible;_tmp4BF.f1=_tag_arr("dumptms",sizeof(
unsigned char),8);_tmp4BF;});_tmp4BE;}));_LL1167:;}return;}_LL1091:;}}void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){struct Cyc_List_List*
_tmp4C2;void*_tmp4C4;struct Cyc_Absyn_Tqual _tmp4C6;struct _tuple4 _tmp4C0=Cyc_Absynpp_to_tms(
tq,t);_LL1223: _tmp4C6=_tmp4C0.f1;goto _LL1221;_LL1221: _tmp4C4=_tmp4C0.f2;goto
_LL1219;_LL1219: _tmp4C2=_tmp4C0.f3;goto _LL1217;_LL1217: Cyc_Absyndump_dumptq(
_tmp4C6);Cyc_Absyndump_dumpntyp(_tmp4C4);Cyc_Absyndump_dumptms(Cyc_List_imp_rev(
_tmp4C2),f,a);}void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,
struct Cyc_Std___sFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=f;for(0;
tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}({
void*_tmp4C8[0]={};Cyc_Std_fprintf(f,_tag_arr("\n",sizeof(unsigned char),2),
_tag_arr(_tmp4C8,sizeof(void*),0));});}
