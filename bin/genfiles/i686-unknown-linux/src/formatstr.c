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
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern
unsigned char Cyc_List_Nth[8];extern int isalnum(int);extern int isalpha(int);extern
int isascii(int);extern int iscntrl(int);extern int isdigit(int);extern int isgraph(int);
extern int islower(int);extern int isprint(int);extern int ispunct(int);extern int
isspace(int);extern int isupper(int);extern int isxdigit(int);extern int toascii(int);
extern int tolower(int);extern int toupper(int);extern int _tolower(int);extern int
_toupper(int);extern unsigned int Cyc_Std_strlen(struct _tagged_arr s);extern struct
_tagged_arr Cyc_Std_implode(struct Cyc_List_List*c);struct Cyc_Lineno_Pos{struct
_tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;static const int Cyc_Position_Lex=
0;static const int Cyc_Position_Parse=1;static const int Cyc_Position_Elab=2;struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];struct Cyc_Cstdio___sFILE;
struct Cyc_Std___sFILE;extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};static const int Cyc_Std_String_pa=0;struct
Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Std_Int_pa=
1;struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};static const int Cyc_Std_Double_pa=
2;struct Cyc_Std_Double_pa_struct{int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=
3;struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=
4;struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};static const int Cyc_Std_ShortPtr_sa=
0;struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};static const int Cyc_Std_UShortPtr_sa=
1;struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=
2;struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=
3;struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=
4;struct Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};static const int
Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};
static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{int tag;
float*f1;};static const int Cyc_Absyn_Loc_n=0;static const int Cyc_Absyn_Rel_n=0;
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
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();extern void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uchar_t;
extern void*Cyc_Absyn_ushort_t;extern void*Cyc_Absyn_uint_t;extern void*Cyc_Absyn_ulong_t;
extern void*Cyc_Absyn_schar_t;extern void*Cyc_Absyn_sshort_t;extern void*Cyc_Absyn_sint_t;
extern void*Cyc_Absyn_slong_t;extern void*Cyc_Absyn_float_t;extern void*Cyc_Absyn_double_t(
int);extern void*Cyc_Absyn_string_typ(void*rgn);extern void*Cyc_Absyn_const_string_typ(
void*rgn);extern void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);
struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
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
ae;struct Cyc_Core_Opt*le;};extern struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);extern unsigned char Cyc_Tcutil_TypeErr[12];extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);extern
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct Cyc_Tcenv_Tenv*,struct
_tagged_arr,struct Cyc_Position_Segment*);extern struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(
struct Cyc_Tcenv_Tenv*,struct _tagged_arr,struct Cyc_Position_Segment*);extern
struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(struct _tagged_arr
s,int i);static struct Cyc_Core_Opt Cyc_Formatstr_rk={(void*)((void*)3)};struct
_tuple3{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;unsigned char f5;int f6;};struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(
struct _tagged_arr s,int i){unsigned int _tmp0=Cyc_Std_strlen(s);if(i < 0? 1: i >= _tmp0){
return 0;}{struct Cyc_List_List*_tmp1=0;unsigned char c=' ';for(0;i < _tmp0;i ++){c=((
const unsigned char*)s.curr)[i];switch(c){case '+': _LL2: goto _LL3;case '-': _LL3: goto
_LL4;case ' ': _LL4: goto _LL5;case '#': _LL5: goto _LL6;case '0': _LL6: _tmp1=({struct Cyc_List_List*
_tmp8=_cycalloc(sizeof(struct Cyc_List_List));_tmp8->hd=(void*)((int)c);_tmp8->tl=
_tmp1;_tmp8;});continue;default: _LL7: break;}break;}if(i >= _tmp0){return 0;}_tmp1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1);{struct
Cyc_List_List*_tmpA=0;c=((const unsigned char*)s.curr)[i];if(c == '*'){_tmpA=({
struct Cyc_List_List*_tmpB=_cycalloc(sizeof(struct Cyc_List_List));_tmpB->hd=(void*)((
int)c);_tmpB->tl=_tmpA;_tmpB;});i ++;}else{for(0;i < _tmp0;i ++){c=((const
unsigned char*)s.curr)[i];if(isdigit((int)c)){_tmpA=({struct Cyc_List_List*_tmpC=
_cycalloc(sizeof(struct Cyc_List_List));_tmpC->hd=(void*)((int)c);_tmpC->tl=_tmpA;
_tmpC;});}else{break;}}}if(i >= _tmp0){return 0;}_tmpA=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA);{struct Cyc_List_List*_tmpD=0;c=((
const unsigned char*)s.curr)[i];if(c == '.'){_tmpD=({struct Cyc_List_List*_tmpE=
_cycalloc(sizeof(struct Cyc_List_List));_tmpE->hd=(void*)((int)c);_tmpE->tl=_tmpD;
_tmpE;});i ++;if(i >= _tmp0){return 0;}c=((const unsigned char*)s.curr)[i];if(c == '*'){
_tmpD=({struct Cyc_List_List*_tmpF=_cycalloc(sizeof(struct Cyc_List_List));_tmpF->hd=(
void*)((int)c);_tmpF->tl=_tmpD;_tmpF;});i ++;}else{for(0;i < _tmp0;i ++){c=((const
unsigned char*)s.curr)[i];if(isdigit((int)c)){_tmpD=({struct Cyc_List_List*_tmp10=
_cycalloc(sizeof(struct Cyc_List_List));_tmp10->hd=(void*)((int)c);_tmp10->tl=
_tmpD;_tmp10;});}else{break;}}}}if(i >= _tmp0){return 0;}_tmpD=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD);{struct Cyc_List_List*_tmp11=0;c=((
const unsigned char*)s.curr)[i];switch(c){case 'h': _LL18: _tmp11=({struct Cyc_List_List*
_tmp14=_cycalloc(sizeof(struct Cyc_List_List));_tmp14->hd=(void*)((int)c);_tmp14->tl=
_tmp11;_tmp14;});i ++;if(i >= _tmp0){return 0;}c=((const unsigned char*)s.curr)[i];
if(c == 'h'){_tmp11=({struct Cyc_List_List*_tmp15=_cycalloc(sizeof(struct Cyc_List_List));
_tmp15->hd=(void*)((int)c);_tmp15->tl=_tmp11;_tmp15;});i ++;}break;case 'l': _LL19:
_tmp11=({struct Cyc_List_List*_tmp17=_cycalloc(sizeof(struct Cyc_List_List));
_tmp17->hd=(void*)((int)c);_tmp17->tl=_tmp11;_tmp17;});i ++;if(i >= _tmp0){return 0;}
c=((const unsigned char*)s.curr)[i];if(c == 'l'){_tmp11=({struct Cyc_List_List*
_tmp18=_cycalloc(sizeof(struct Cyc_List_List));_tmp18->hd=(void*)((int)c);_tmp18->tl=
_tmp11;_tmp18;});i ++;}break;case 'j': _LL22: goto _LL25;case 'z': _LL25: goto _LL26;case
't': _LL26: goto _LL27;case 'L': _LL27: _tmp11=({struct Cyc_List_List*_tmp1D=_cycalloc(
sizeof(struct Cyc_List_List));_tmp1D->hd=(void*)((int)c);_tmp1D->tl=_tmp11;_tmp1D;});
i ++;break;default: _LL28: break;}if(i >= _tmp0){return 0;}_tmp11=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp11);c=((const unsigned char*)s.curr)[
i];switch(c){case 'd': _LL31: break;case 'i': _LL32: break;case 'o': _LL33: break;case 'u':
_LL34: break;case 'x': _LL35: break;case 'X': _LL36: break;case 'f': _LL37: break;case 'F':
_LL38: break;case 'e': _LL39: break;case 'E': _LL40: break;case 'g': _LL41: break;case 'G':
_LL42: break;case 'a': _LL43: break;case 'A': _LL44: break;case 'c': _LL45: break;case 's':
_LL46: break;case 'p': _LL47: break;case 'n': _LL48: break;case '%': _LL49: break;default:
_LL50: return 0;}return({struct Cyc_Core_Opt*_tmp34=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp34->v=({struct _tuple3*_tmp35=_cycalloc(sizeof(struct _tuple3));_tmp35->f1=
_tmp1;_tmp35->f2=_tmpA;_tmp35->f3=_tmpD;_tmp35->f4=_tmp11;_tmp35->f5=c;_tmp35->f6=
i + 1;_tmp35;});_tmp34;});}}}}}struct Cyc_List_List*Cyc_Formatstr_get_format_typs(
struct Cyc_Tcenv_Tenv*te,struct _tagged_arr s,struct Cyc_Position_Segment*loc){
unsigned int _tmp36=Cyc_Std_strlen(s);struct Cyc_List_List*_tmp37=0;int i;for(i=0;i
< _tmp36;i ++){if(((const unsigned char*)s.curr)[i]!= '%'){continue;}{struct Cyc_Core_Opt*
_tmp38=Cyc_Formatstr_parse_conversionspecification(s,i + 1);if(_tmp38 == 0){({void*
_tmp39[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad format string",sizeof(
unsigned char),18),_tag_arr(_tmp39,sizeof(void*),0));});return 0;}{struct _tuple3
_tmp3C;int _tmp3D;unsigned char _tmp3F;struct Cyc_List_List*_tmp41;struct Cyc_List_List*
_tmp43;struct Cyc_List_List*_tmp45;struct Cyc_List_List*_tmp47;struct _tuple3*
_tmp3A=(struct _tuple3*)_tmp38->v;_tmp3C=*_tmp3A;_LL72: _tmp47=_tmp3C.f1;goto _LL70;
_LL70: _tmp45=_tmp3C.f2;goto _LL68;_LL68: _tmp43=_tmp3C.f3;goto _LL66;_LL66: _tmp41=
_tmp3C.f4;goto _LL64;_LL64: _tmp3F=_tmp3C.f5;goto _LL62;_LL62: _tmp3D=_tmp3C.f6;goto
_LL59;_LL59: i=_tmp3D - 1;{struct Cyc_List_List*_tmp49=_tmp41;struct Cyc_List_List
_tmp50;struct Cyc_List_List*_tmp51;int _tmp53;_LL75: if(_tmp49 == 0){goto _LL77;}
else{_tmp50=*_tmp49;_LL84: _tmp53=(int)_tmp50.hd;goto _LL82;_LL82: _tmp51=_tmp50.tl;
if(_tmp51 == 0){goto _LL79;}else{goto _LL77;}}_LL79: if((_tmp53 == 'j'? 1: _tmp53 == 'z')?
1: _tmp53 == 't'){goto _LL76;}else{goto _LL77;}_LL77: goto _LL78;_LL76:({struct Cyc_Std_Int_pa_struct
_tmp56;_tmp56.tag=Cyc_Std_Int_pa;_tmp56.f1=(int)((unsigned int)_tmp53);{void*
_tmp55[1]={& _tmp56};Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%c' is not supported",
sizeof(unsigned char),38),_tag_arr(_tmp55,sizeof(void*),1));}});return 0;_LL78:
goto _LL74;_LL74:;}{struct Cyc_List_List*_tmp57=_tmp45;struct Cyc_List_List _tmp5E;
struct Cyc_List_List*_tmp5F;int _tmp61;_LL89: if(_tmp57 == 0){goto _LL91;}else{_tmp5E=*
_tmp57;_LL98: _tmp61=(int)_tmp5E.hd;goto _LL96;_LL96: _tmp5F=_tmp5E.tl;if(_tmp5F == 
0){goto _LL93;}else{goto _LL91;}}_LL93: if(_tmp61 == '*'){goto _LL90;}else{goto _LL91;}
_LL91: goto _LL92;_LL90: _tmp37=({struct Cyc_List_List*_tmp63=_cycalloc(sizeof(
struct Cyc_List_List));_tmp63->hd=(void*)Cyc_Absyn_sint_t;_tmp63->tl=_tmp37;
_tmp63;});goto _LL88;_LL92: goto _LL88;_LL88:;}{struct Cyc_List_List*_tmp64=_tmp43;
struct Cyc_List_List _tmp6B;struct Cyc_List_List*_tmp6C;struct Cyc_List_List _tmp6E;
struct Cyc_List_List*_tmp6F;int _tmp71;int _tmp73;_LL102: if(_tmp64 == 0){goto _LL104;}
else{_tmp6B=*_tmp64;_LL116: _tmp73=(int)_tmp6B.hd;goto _LL109;_LL109: _tmp6C=_tmp6B.tl;
if(_tmp6C == 0){goto _LL104;}else{_tmp6E=*_tmp6C;_LL114: _tmp71=(int)_tmp6E.hd;goto
_LL112;_LL112: _tmp6F=_tmp6E.tl;if(_tmp6F == 0){goto _LL106;}else{goto _LL104;}}}
_LL106: if(_tmp73 == '.'? _tmp71 == '*': 0){goto _LL103;}else{goto _LL104;}_LL104: goto
_LL105;_LL103: _tmp37=({struct Cyc_List_List*_tmp75=_cycalloc(sizeof(struct Cyc_List_List));
_tmp75->hd=(void*)Cyc_Absyn_sint_t;_tmp75->tl=_tmp37;_tmp75;});goto _LL101;_LL105:
goto _LL101;_LL101:;}{void*t;switch(_tmp3F){case 'd': _LL118: goto _LL119;case 'i':
_LL119:{struct Cyc_List_List*f=_tmp47;for(0;f != 0;f=f->tl){if((int)f->hd == '#'){({
struct Cyc_Std_Int_pa_struct _tmp7A;_tmp7A.tag=Cyc_Std_Int_pa;_tmp7A.f1=(int)((
unsigned int)((int)_tmp3F));{void*_tmp79[1]={& _tmp7A};Cyc_Tcutil_terr(loc,
_tag_arr("flag '#' is not valid with %%%c",sizeof(unsigned char),32),_tag_arr(
_tmp79,sizeof(void*),1));}});return 0;}}}{struct Cyc_List_List*_tmp7B=_tmp41;
struct Cyc_List_List _tmp88;struct Cyc_List_List*_tmp89;int _tmp8B;struct Cyc_List_List
_tmp8E;struct Cyc_List_List*_tmp8F;int _tmp91;struct Cyc_List_List _tmp94;struct Cyc_List_List*
_tmp95;struct Cyc_List_List _tmp97;struct Cyc_List_List*_tmp98;int _tmp9A;int _tmp9C;
_LL125: if(_tmp7B == 0){goto _LL126;}else{goto _LL127;}_LL127: if(_tmp7B == 0){goto
_LL129;}else{_tmp88=*_tmp7B;_LL140: _tmp8B=(int)_tmp88.hd;goto _LL138;_LL138:
_tmp89=_tmp88.tl;if(_tmp89 == 0){goto _LL135;}else{goto _LL129;}}_LL135: if(_tmp8B == 'l'){
goto _LL128;}else{goto _LL129;}_LL129: if(_tmp7B == 0){goto _LL131;}else{_tmp8E=*
_tmp7B;_LL146: _tmp91=(int)_tmp8E.hd;goto _LL144;_LL144: _tmp8F=_tmp8E.tl;if(_tmp8F
== 0){goto _LL141;}else{goto _LL131;}}_LL141: if(_tmp91 == 'h'){goto _LL130;}else{
goto _LL131;}_LL131: if(_tmp7B == 0){goto _LL133;}else{_tmp94=*_tmp7B;_LL157: _tmp9C=(
int)_tmp94.hd;goto _LL150;_LL150: _tmp95=_tmp94.tl;if(_tmp95 == 0){goto _LL133;}
else{_tmp97=*_tmp95;_LL155: _tmp9A=(int)_tmp97.hd;goto _LL153;_LL153: _tmp98=_tmp97.tl;
if(_tmp98 == 0){goto _LL147;}else{goto _LL133;}}}_LL147: if(_tmp9C == 'h'? _tmp9A == 'h':
0){goto _LL132;}else{goto _LL133;}_LL133: goto _LL134;_LL126: t=Cyc_Absyn_sint_t;goto
_LL124;_LL128: t=Cyc_Absyn_slong_t;goto _LL124;_LL130: t=Cyc_Absyn_sshort_t;goto
_LL124;_LL132: t=Cyc_Absyn_schar_t;goto _LL124;_LL134:({struct Cyc_Std_Int_pa_struct
_tmpA0;_tmpA0.tag=Cyc_Std_Int_pa;_tmpA0.f1=(int)((unsigned int)((int)_tmp3F));{
struct Cyc_Std_String_pa_struct _tmp9F;_tmp9F.tag=Cyc_Std_String_pa;_tmp9F.f1=(
struct _tagged_arr)Cyc_Std_implode(_tmp41);{void*_tmp9E[2]={& _tmp9F,& _tmpA0};Cyc_Tcutil_terr(
loc,_tag_arr("length modifier '%s' is not allowed with %%%c",sizeof(
unsigned char),46),_tag_arr(_tmp9E,sizeof(void*),2));}}});return 0;_LL124:;}
_tmp37=({struct Cyc_List_List*_tmpA1=_cycalloc(sizeof(struct Cyc_List_List));
_tmpA1->hd=(void*)t;_tmpA1->tl=_tmp37;_tmpA1;});break;case 'u': _LL120:{struct Cyc_List_List*
f=_tmp47;for(0;f != 0;f=f->tl){if((int)f->hd == '#'){({void*_tmpA3[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("Flag '#' not valid with %%u",sizeof(unsigned char),28),_tag_arr(
_tmpA3,sizeof(void*),0));});return 0;}}}goto _LL162;case 'o': _LL162: goto _LL164;case
'x': _LL164: goto _LL165;case 'X': _LL165:{struct Cyc_List_List*_tmpA7=_tmp41;struct
Cyc_List_List _tmpB4;struct Cyc_List_List*_tmpB5;int _tmpB7;struct Cyc_List_List
_tmpBA;struct Cyc_List_List*_tmpBB;int _tmpBD;struct Cyc_List_List _tmpC0;struct Cyc_List_List*
_tmpC1;struct Cyc_List_List _tmpC3;struct Cyc_List_List*_tmpC4;int _tmpC6;int _tmpC8;
_LL169: if(_tmpA7 == 0){goto _LL170;}else{goto _LL171;}_LL171: if(_tmpA7 == 0){goto
_LL173;}else{_tmpB4=*_tmpA7;_LL184: _tmpB7=(int)_tmpB4.hd;goto _LL182;_LL182:
_tmpB5=_tmpB4.tl;if(_tmpB5 == 0){goto _LL179;}else{goto _LL173;}}_LL179: if(_tmpB7 == 'l'){
goto _LL172;}else{goto _LL173;}_LL173: if(_tmpA7 == 0){goto _LL175;}else{_tmpBA=*
_tmpA7;_LL190: _tmpBD=(int)_tmpBA.hd;goto _LL188;_LL188: _tmpBB=_tmpBA.tl;if(_tmpBB
== 0){goto _LL185;}else{goto _LL175;}}_LL185: if(_tmpBD == 'h'){goto _LL174;}else{
goto _LL175;}_LL175: if(_tmpA7 == 0){goto _LL177;}else{_tmpC0=*_tmpA7;_LL201: _tmpC8=(
int)_tmpC0.hd;goto _LL194;_LL194: _tmpC1=_tmpC0.tl;if(_tmpC1 == 0){goto _LL177;}
else{_tmpC3=*_tmpC1;_LL199: _tmpC6=(int)_tmpC3.hd;goto _LL197;_LL197: _tmpC4=_tmpC3.tl;
if(_tmpC4 == 0){goto _LL191;}else{goto _LL177;}}}_LL191: if(_tmpC8 == 'h'? _tmpC6 == 'h':
0){goto _LL176;}else{goto _LL177;}_LL177: goto _LL178;_LL170: t=Cyc_Absyn_uint_t;goto
_LL168;_LL172: t=Cyc_Absyn_ulong_t;goto _LL168;_LL174: t=Cyc_Absyn_ushort_t;goto
_LL168;_LL176: t=Cyc_Absyn_uchar_t;goto _LL168;_LL178:({struct Cyc_Std_Int_pa_struct
_tmpCC;_tmpCC.tag=Cyc_Std_Int_pa;_tmpCC.f1=(int)((unsigned int)((int)_tmp3F));{
struct Cyc_Std_String_pa_struct _tmpCB;_tmpCB.tag=Cyc_Std_String_pa;_tmpCB.f1=(
struct _tagged_arr)Cyc_Std_implode(_tmp41);{void*_tmpCA[2]={& _tmpCB,& _tmpCC};Cyc_Tcutil_terr(
loc,_tag_arr("length modifier '%s' is not allowed with %%%c",sizeof(
unsigned char),46),_tag_arr(_tmpCA,sizeof(void*),2));}}});return 0;_LL168:;}
_tmp37=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(struct Cyc_List_List));
_tmpCD->hd=(void*)t;_tmpCD->tl=_tmp37;_tmpCD;});break;case 'f': _LL166: goto _LL206;
case 'F': _LL206: goto _LL207;case 'e': _LL207: goto _LL208;case 'E': _LL208: goto _LL209;
case 'g': _LL209: goto _LL210;case 'G': _LL210: goto _LL211;case 'a': _LL211: goto _LL212;
case 'A': _LL212: if(_tmp41 != 0){({struct Cyc_Std_Int_pa_struct _tmpD7;_tmpD7.tag=Cyc_Std_Int_pa;
_tmpD7.f1=(int)((unsigned int)((int)_tmp3F));{void*_tmpD6[1]={& _tmpD7};Cyc_Tcutil_terr(
loc,_tag_arr("length modifiers are not allowed with %%%c",sizeof(unsigned char),
43),_tag_arr(_tmpD6,sizeof(void*),1));}});return 0;}_tmp37=({struct Cyc_List_List*
_tmpD8=_cycalloc(sizeof(struct Cyc_List_List));_tmpD8->hd=(void*)Cyc_Absyn_double_t(
0);_tmpD8->tl=_tmp37;_tmpD8;});break;case 'c': _LL213:{struct Cyc_List_List*f=
_tmp47;for(0;f != 0;f=f->tl){if((int)f->hd == '#'? 1:(int)f->hd == '0'){({struct Cyc_Std_Int_pa_struct
_tmpDB;_tmpDB.tag=Cyc_Std_Int_pa;_tmpDB.f1=(int)((unsigned int)((int)f->hd));{
void*_tmpDA[1]={& _tmpDB};Cyc_Tcutil_terr(loc,_tag_arr("flag '%c' not allowed with %%c",
sizeof(unsigned char),31),_tag_arr(_tmpDA,sizeof(void*),1));}});return 0;}}}if(
_tmp41 != 0){({struct Cyc_Std_String_pa_struct _tmpDD;_tmpDD.tag=Cyc_Std_String_pa;
_tmpDD.f1=(struct _tagged_arr)Cyc_Std_implode(_tmp41);{void*_tmpDC[1]={& _tmpDD};
Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%s' not allowed with %%c",sizeof(
unsigned char),42),_tag_arr(_tmpDC,sizeof(void*),1));}});return 0;}if(_tmp43 != 0){({
struct Cyc_Std_String_pa_struct _tmpDF;_tmpDF.tag=Cyc_Std_String_pa;_tmpDF.f1=(
struct _tagged_arr)Cyc_Std_implode(_tmp43);{void*_tmpDE[1]={& _tmpDF};Cyc_Tcutil_terr(
loc,_tag_arr("precision '%s' not allowed with %%c",sizeof(unsigned char),36),
_tag_arr(_tmpDE,sizeof(void*),1));}});return 0;}_tmp37=({struct Cyc_List_List*
_tmpE0=_cycalloc(sizeof(struct Cyc_List_List));_tmpE0->hd=(void*)Cyc_Absyn_sint_t;
_tmpE0->tl=_tmp37;_tmpE0;});break;case 's': _LL217:{struct Cyc_List_List*f=_tmp47;
for(0;f != 0;f=f->tl){if((int)f->hd != '-'){({void*_tmpE2[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("a flag not allowed with %%s",sizeof(unsigned char),28),_tag_arr(
_tmpE2,sizeof(void*),0));});return 0;}}}if(_tmp41 != 0){({void*_tmpE3[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("length modifiers not allowed with %%s",sizeof(unsigned char),38),
_tag_arr(_tmpE3,sizeof(void*),0));});return 0;}_tmp37=({struct Cyc_List_List*
_tmpE4=_cycalloc(sizeof(struct Cyc_List_List));_tmpE4->hd=(void*)Cyc_Absyn_const_string_typ(
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,({struct Cyc_Core_Opt*
_tmpE5=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpE5->v=Cyc_Tcenv_lookup_type_vars(
te);_tmpE5;})));_tmpE4->tl=_tmp37;_tmpE4;});break;case 'p': _LL225: _tmp37=({struct
Cyc_List_List*_tmpE7=_cycalloc(sizeof(struct Cyc_List_List));_tmpE7->hd=(void*)
Cyc_Absyn_uint_t;_tmpE7->tl=_tmp37;_tmpE7;});break;case 'n': _LL230:{struct Cyc_List_List*
f=_tmp47;for(0;f != 0;f=f->tl){if((int)f->hd == '#'? 1:(int)f->hd == '0'){({struct
Cyc_Std_Int_pa_struct _tmpEA;_tmpEA.tag=Cyc_Std_Int_pa;_tmpEA.f1=(int)((
unsigned int)((int)f->hd));{void*_tmpE9[1]={& _tmpEA};Cyc_Tcutil_terr(loc,
_tag_arr("flag '%c' not allowed with %%n",sizeof(unsigned char),31),_tag_arr(
_tmpE9,sizeof(void*),1));}});return 0;}}}if(_tmp43 != 0){({struct Cyc_Std_String_pa_struct
_tmpEC;_tmpEC.tag=Cyc_Std_String_pa;_tmpEC.f1=(struct _tagged_arr)Cyc_Std_implode(
_tmp43);{void*_tmpEB[1]={& _tmpEC};Cyc_Tcutil_terr(loc,_tag_arr("precision '%s' not allowed with %%n",
sizeof(unsigned char),36),_tag_arr(_tmpEB,sizeof(void*),1));}});return 0;}{struct
Cyc_Core_Opt*_tmpED=({struct Cyc_Core_Opt*_tmp114=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp114->v=Cyc_Tcenv_lookup_type_vars(te);_tmp114;});{struct Cyc_List_List*_tmpEE=
_tmp41;struct Cyc_List_List _tmpFB;struct Cyc_List_List*_tmpFC;int _tmpFE;struct Cyc_List_List
_tmp101;struct Cyc_List_List*_tmp102;int _tmp104;struct Cyc_List_List _tmp107;struct
Cyc_List_List*_tmp108;struct Cyc_List_List _tmp10A;struct Cyc_List_List*_tmp10B;int
_tmp10D;int _tmp10F;_LL240: if(_tmpEE == 0){goto _LL241;}else{goto _LL242;}_LL242: if(
_tmpEE == 0){goto _LL244;}else{_tmpFB=*_tmpEE;_LL255: _tmpFE=(int)_tmpFB.hd;goto
_LL253;_LL253: _tmpFC=_tmpFB.tl;if(_tmpFC == 0){goto _LL250;}else{goto _LL244;}}
_LL250: if(_tmpFE == 'l'){goto _LL243;}else{goto _LL244;}_LL244: if(_tmpEE == 0){goto
_LL246;}else{_tmp101=*_tmpEE;_LL261: _tmp104=(int)_tmp101.hd;goto _LL259;_LL259:
_tmp102=_tmp101.tl;if(_tmp102 == 0){goto _LL256;}else{goto _LL246;}}_LL256: if(
_tmp104 == 'h'){goto _LL245;}else{goto _LL246;}_LL246: if(_tmpEE == 0){goto _LL248;}
else{_tmp107=*_tmpEE;_LL272: _tmp10F=(int)_tmp107.hd;goto _LL265;_LL265: _tmp108=
_tmp107.tl;if(_tmp108 == 0){goto _LL248;}else{_tmp10A=*_tmp108;_LL270: _tmp10D=(int)
_tmp10A.hd;goto _LL268;_LL268: _tmp10B=_tmp10A.tl;if(_tmp10B == 0){goto _LL262;}
else{goto _LL248;}}}_LL262: if(_tmp10F == 'h'? _tmp10D == 'h': 0){goto _LL247;}else{
goto _LL248;}_LL248: goto _LL249;_LL241: t=Cyc_Absyn_at_typ(Cyc_Absyn_sint_t,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmpED),Cyc_Absyn_empty_tqual());goto
_LL239;_LL243: t=Cyc_Absyn_at_typ(Cyc_Absyn_ulong_t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk,_tmpED),Cyc_Absyn_empty_tqual());goto _LL239;_LL245: t=Cyc_Absyn_at_typ(
Cyc_Absyn_sshort_t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_tmpED),Cyc_Absyn_empty_tqual());goto _LL239;_LL247: t=Cyc_Absyn_at_typ(Cyc_Absyn_schar_t,
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmpED),Cyc_Absyn_empty_tqual());
goto _LL239;_LL249:({struct Cyc_Std_String_pa_struct _tmp112;_tmp112.tag=Cyc_Std_String_pa;
_tmp112.f1=(struct _tagged_arr)Cyc_Std_implode(_tmp41);{void*_tmp111[1]={& _tmp112};
Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%s' not allowed with %%n",sizeof(
unsigned char),42),_tag_arr(_tmp111,sizeof(void*),1));}});return 0;_LL239:;}
_tmp37=({struct Cyc_List_List*_tmp113=_cycalloc(sizeof(struct Cyc_List_List));
_tmp113->hd=(void*)t;_tmp113->tl=_tmp37;_tmp113;});break;}case '%': _LL232: if(
_tmp47 != 0){({struct Cyc_Std_String_pa_struct _tmp117;_tmp117.tag=Cyc_Std_String_pa;
_tmp117.f1=(struct _tagged_arr)Cyc_Std_implode(_tmp47);{void*_tmp116[1]={& _tmp117};
Cyc_Tcutil_terr(loc,_tag_arr("flags '%s' not allowed with %%%%",sizeof(
unsigned char),33),_tag_arr(_tmp116,sizeof(void*),1));}});return 0;}if(_tmp45 != 0){({
struct Cyc_Std_String_pa_struct _tmp119;_tmp119.tag=Cyc_Std_String_pa;_tmp119.f1=(
struct _tagged_arr)Cyc_Std_implode(_tmp45);{void*_tmp118[1]={& _tmp119};Cyc_Tcutil_terr(
loc,_tag_arr("width '%s' not allowed with %%%%",sizeof(unsigned char),33),
_tag_arr(_tmp118,sizeof(void*),1));}});return 0;}if(_tmp43 != 0){({struct Cyc_Std_String_pa_struct
_tmp11B;_tmp11B.tag=Cyc_Std_String_pa;_tmp11B.f1=(struct _tagged_arr)Cyc_Std_implode(
_tmp43);{void*_tmp11A[1]={& _tmp11B};Cyc_Tcutil_terr(loc,_tag_arr("precision '%s' not allowed with %%%%",
sizeof(unsigned char),37),_tag_arr(_tmp11A,sizeof(void*),1));}});return 0;}if(
_tmp41 != 0){({struct Cyc_Std_String_pa_struct _tmp11D;_tmp11D.tag=Cyc_Std_String_pa;
_tmp11D.f1=(struct _tagged_arr)Cyc_Std_implode(_tmp41);{void*_tmp11C[1]={& _tmp11D};
Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%s' not allowed with %%%%",
sizeof(unsigned char),43),_tag_arr(_tmp11C,sizeof(void*),1));}});return 0;}break;
default: _LL277: return 0;}}}}}return Cyc_List_imp_rev(_tmp37);}struct _tuple4{int f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;unsigned char f4;int f5;};struct Cyc_Core_Opt*
Cyc_Formatstr_parse_inputformat(struct _tagged_arr s,int i){unsigned int _tmp11F=Cyc_Std_strlen(
s);if(i < 0? 1: i >= _tmp11F){return 0;}{int _tmp120=0;unsigned char _tmp121=((const
unsigned char*)s.curr)[i];if(_tmp121 == '*'){_tmp120=1;i ++;if(i >= _tmp11F){return
0;}}{struct Cyc_List_List*_tmp122=0;for(0;i < _tmp11F;i ++){_tmp121=((const
unsigned char*)s.curr)[i];if(isdigit((int)_tmp121)){_tmp122=({struct Cyc_List_List*
_tmp123=_cycalloc(sizeof(struct Cyc_List_List));_tmp123->hd=(void*)((int)_tmp121);
_tmp123->tl=_tmp122;_tmp123;});}else{break;}}if(i >= _tmp11F){return 0;}_tmp122=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp122);{
struct Cyc_List_List*_tmp124=0;_tmp121=((const unsigned char*)s.curr)[i];switch(
_tmp121){case 'h': _LL293: _tmp124=({struct Cyc_List_List*_tmp127=_cycalloc(sizeof(
struct Cyc_List_List));_tmp127->hd=(void*)((int)_tmp121);_tmp127->tl=_tmp124;
_tmp127;});i ++;if(i >= _tmp11F){return 0;}_tmp121=((const unsigned char*)s.curr)[i];
if(_tmp121 == 'h'){_tmp124=({struct Cyc_List_List*_tmp128=_cycalloc(sizeof(struct
Cyc_List_List));_tmp128->hd=(void*)((int)_tmp121);_tmp128->tl=_tmp124;_tmp128;});
i ++;}break;case 'l': _LL294: _tmp124=({struct Cyc_List_List*_tmp12A=_cycalloc(
sizeof(struct Cyc_List_List));_tmp12A->hd=(void*)((int)_tmp121);_tmp12A->tl=
_tmp124;_tmp12A;});i ++;if(i >= _tmp11F){return 0;}_tmp121=((const unsigned char*)s.curr)[
i];if(_tmp121 == 'l'){_tmp124=({struct Cyc_List_List*_tmp12B=_cycalloc(sizeof(
struct Cyc_List_List));_tmp12B->hd=(void*)((int)_tmp121);_tmp12B->tl=_tmp124;
_tmp12B;});i ++;}break;case 'j': _LL297: goto _LL300;case 'z': _LL300: goto _LL301;case 't':
_LL301: goto _LL302;case 'L': _LL302: _tmp124=({struct Cyc_List_List*_tmp130=_cycalloc(
sizeof(struct Cyc_List_List));_tmp130->hd=(void*)((int)_tmp121);_tmp130->tl=
_tmp124;_tmp130;});i ++;break;default: _LL303: break;}if(i >= _tmp11F){return 0;}
_tmp124=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp124);_tmp121=((const unsigned char*)s.curr)[i];switch(_tmp121){case 'd': _LL306:
break;case 'i': _LL307: break;case 'o': _LL308: break;case 'u': _LL309: break;case 'x':
_LL310: break;case 'X': _LL311: break;case 'f': _LL312: break;case 'F': _LL313: break;case
'e': _LL314: break;case 'E': _LL315: break;case 'g': _LL316: break;case 'G': _LL317: break;
case 'a': _LL318: break;case 'A': _LL319: break;case 'c': _LL320: break;case 's': _LL321:
break;case 'p': _LL322: break;case 'n': _LL323: break;case '%': _LL324: break;default:
_LL325: return 0;}return({struct Cyc_Core_Opt*_tmp147=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp147->v=({struct _tuple4*_tmp148=_cycalloc(sizeof(struct _tuple4));_tmp148->f1=
_tmp120;_tmp148->f2=_tmp122;_tmp148->f3=_tmp124;_tmp148->f4=_tmp121;_tmp148->f5=
i + 1;_tmp148;});_tmp147;});}}}}struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(
struct Cyc_Tcenv_Tenv*te,struct _tagged_arr s,struct Cyc_Position_Segment*loc){
unsigned int _tmp149=Cyc_Std_strlen(s);struct Cyc_List_List*_tmp14A=0;int i;for(i=0;
i < _tmp149;i ++){if(((const unsigned char*)s.curr)[i]!= '%'){continue;}{struct Cyc_Core_Opt*
_tmp14B=Cyc_Formatstr_parse_inputformat(s,i + 1);if(_tmp14B == 0){({void*_tmp14C[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("bad format string",sizeof(unsigned char),18),
_tag_arr(_tmp14C,sizeof(void*),0));});return 0;}{struct _tuple4 _tmp14F;int _tmp150;
unsigned char _tmp152;struct Cyc_List_List*_tmp154;struct Cyc_List_List*_tmp156;int
_tmp158;struct _tuple4*_tmp14D=(struct _tuple4*)_tmp14B->v;_tmp14F=*_tmp14D;_LL345:
_tmp158=_tmp14F.f1;goto _LL343;_LL343: _tmp156=_tmp14F.f2;goto _LL341;_LL341:
_tmp154=_tmp14F.f3;goto _LL339;_LL339: _tmp152=_tmp14F.f4;goto _LL337;_LL337:
_tmp150=_tmp14F.f5;goto _LL334;_LL334: i=_tmp150 - 1;{struct Cyc_List_List*_tmp15A=
_tmp154;struct Cyc_List_List _tmp161;struct Cyc_List_List*_tmp162;int _tmp164;_LL348:
if(_tmp15A == 0){goto _LL350;}else{_tmp161=*_tmp15A;_LL357: _tmp164=(int)_tmp161.hd;
goto _LL355;_LL355: _tmp162=_tmp161.tl;if(_tmp162 == 0){goto _LL352;}else{goto _LL350;}}
_LL352: if((_tmp164 == 'j'? 1: _tmp164 == 'z')? 1: _tmp164 == 't'){goto _LL349;}else{goto
_LL350;}_LL350: goto _LL351;_LL349:({struct Cyc_Std_Int_pa_struct _tmp167;_tmp167.tag=
Cyc_Std_Int_pa;_tmp167.f1=(int)((unsigned int)_tmp164);{void*_tmp166[1]={&
_tmp167};Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%c' is not supported",
sizeof(unsigned char),38),_tag_arr(_tmp166,sizeof(void*),1));}});return 0;_LL351:
goto _LL347;_LL347:;}if(_tmp158){continue;}{void*t;switch(_tmp152){case 'd': _LL360:
goto _LL361;case 'i': _LL361: {struct Cyc_Core_Opt*_tmp16B=({struct Cyc_Core_Opt*
_tmp193=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp193->v=Cyc_Tcenv_lookup_type_vars(
te);_tmp193;});{struct Cyc_List_List*_tmp16C=_tmp154;struct Cyc_List_List _tmp179;
struct Cyc_List_List*_tmp17A;int _tmp17C;struct Cyc_List_List _tmp17F;struct Cyc_List_List*
_tmp180;int _tmp182;struct Cyc_List_List _tmp185;struct Cyc_List_List*_tmp186;struct
Cyc_List_List _tmp188;struct Cyc_List_List*_tmp189;int _tmp18B;int _tmp18D;_LL366:
if(_tmp16C == 0){goto _LL367;}else{goto _LL368;}_LL368: if(_tmp16C == 0){goto _LL370;}
else{_tmp179=*_tmp16C;_LL381: _tmp17C=(int)_tmp179.hd;goto _LL379;_LL379: _tmp17A=
_tmp179.tl;if(_tmp17A == 0){goto _LL376;}else{goto _LL370;}}_LL376: if(_tmp17C == 'l'){
goto _LL369;}else{goto _LL370;}_LL370: if(_tmp16C == 0){goto _LL372;}else{_tmp17F=*
_tmp16C;_LL387: _tmp182=(int)_tmp17F.hd;goto _LL385;_LL385: _tmp180=_tmp17F.tl;if(
_tmp180 == 0){goto _LL382;}else{goto _LL372;}}_LL382: if(_tmp182 == 'h'){goto _LL371;}
else{goto _LL372;}_LL372: if(_tmp16C == 0){goto _LL374;}else{_tmp185=*_tmp16C;_LL398:
_tmp18D=(int)_tmp185.hd;goto _LL391;_LL391: _tmp186=_tmp185.tl;if(_tmp186 == 0){
goto _LL374;}else{_tmp188=*_tmp186;_LL396: _tmp18B=(int)_tmp188.hd;goto _LL394;
_LL394: _tmp189=_tmp188.tl;if(_tmp189 == 0){goto _LL388;}else{goto _LL374;}}}_LL388:
if(_tmp18D == 'h'? _tmp18B == 'h': 0){goto _LL373;}else{goto _LL374;}_LL374: goto _LL375;
_LL367: t=Cyc_Absyn_at_typ(Cyc_Absyn_sint_t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk,_tmp16B),Cyc_Absyn_empty_tqual());goto _LL365;_LL369: t=Cyc_Absyn_at_typ(
Cyc_Absyn_slong_t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_tmp16B),Cyc_Absyn_empty_tqual());goto _LL365;_LL371: t=Cyc_Absyn_at_typ(Cyc_Absyn_sshort_t,
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmp16B),Cyc_Absyn_empty_tqual());
goto _LL365;_LL373: t=Cyc_Absyn_at_typ(Cyc_Absyn_schar_t,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmp16B),Cyc_Absyn_empty_tqual());goto
_LL365;_LL375:({struct Cyc_Std_Int_pa_struct _tmp191;_tmp191.tag=Cyc_Std_Int_pa;
_tmp191.f1=(int)((unsigned int)((int)_tmp152));{struct Cyc_Std_String_pa_struct
_tmp190;_tmp190.tag=Cyc_Std_String_pa;_tmp190.f1=(struct _tagged_arr)Cyc_Std_implode(
_tmp154);{void*_tmp18F[2]={& _tmp190,& _tmp191};Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof(unsigned char),46),_tag_arr(_tmp18F,sizeof(void*),2));}}});return 0;_LL365:;}
_tmp14A=({struct Cyc_List_List*_tmp192=_cycalloc(sizeof(struct Cyc_List_List));
_tmp192->hd=(void*)t;_tmp192->tl=_tmp14A;_tmp192;});break;}case 'u': _LL362: goto
_LL404;case 'o': _LL404: goto _LL405;case 'x': _LL405: goto _LL406;case 'X': _LL406: {
struct Cyc_Core_Opt*_tmp198=({struct Cyc_Core_Opt*_tmp1C0=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmp1C0->v=Cyc_Tcenv_lookup_type_vars(te);_tmp1C0;});{struct Cyc_List_List*
_tmp199=_tmp154;struct Cyc_List_List _tmp1A6;struct Cyc_List_List*_tmp1A7;int
_tmp1A9;struct Cyc_List_List _tmp1AC;struct Cyc_List_List*_tmp1AD;int _tmp1AF;struct
Cyc_List_List _tmp1B2;struct Cyc_List_List*_tmp1B3;struct Cyc_List_List _tmp1B5;
struct Cyc_List_List*_tmp1B6;int _tmp1B8;int _tmp1BA;_LL411: if(_tmp199 == 0){goto
_LL412;}else{goto _LL413;}_LL413: if(_tmp199 == 0){goto _LL415;}else{_tmp1A6=*
_tmp199;_LL426: _tmp1A9=(int)_tmp1A6.hd;goto _LL424;_LL424: _tmp1A7=_tmp1A6.tl;if(
_tmp1A7 == 0){goto _LL421;}else{goto _LL415;}}_LL421: if(_tmp1A9 == 'l'){goto _LL414;}
else{goto _LL415;}_LL415: if(_tmp199 == 0){goto _LL417;}else{_tmp1AC=*_tmp199;_LL432:
_tmp1AF=(int)_tmp1AC.hd;goto _LL430;_LL430: _tmp1AD=_tmp1AC.tl;if(_tmp1AD == 0){
goto _LL427;}else{goto _LL417;}}_LL427: if(_tmp1AF == 'h'){goto _LL416;}else{goto
_LL417;}_LL417: if(_tmp199 == 0){goto _LL419;}else{_tmp1B2=*_tmp199;_LL443: _tmp1BA=(
int)_tmp1B2.hd;goto _LL436;_LL436: _tmp1B3=_tmp1B2.tl;if(_tmp1B3 == 0){goto _LL419;}
else{_tmp1B5=*_tmp1B3;_LL441: _tmp1B8=(int)_tmp1B5.hd;goto _LL439;_LL439: _tmp1B6=
_tmp1B5.tl;if(_tmp1B6 == 0){goto _LL433;}else{goto _LL419;}}}_LL433: if(_tmp1BA == 'h'?
_tmp1B8 == 'h': 0){goto _LL418;}else{goto _LL419;}_LL419: goto _LL420;_LL412: t=Cyc_Absyn_at_typ(
Cyc_Absyn_uint_t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_tmp198),Cyc_Absyn_empty_tqual());goto _LL410;_LL414: t=Cyc_Absyn_at_typ(Cyc_Absyn_ulong_t,
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmp198),Cyc_Absyn_empty_tqual());
goto _LL410;_LL416: t=Cyc_Absyn_at_typ(Cyc_Absyn_ushort_t,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmp198),Cyc_Absyn_empty_tqual());goto
_LL410;_LL418: t=Cyc_Absyn_at_typ(Cyc_Absyn_uchar_t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk,_tmp198),Cyc_Absyn_empty_tqual());goto _LL410;_LL420:({struct Cyc_Std_Int_pa_struct
_tmp1BE;_tmp1BE.tag=Cyc_Std_Int_pa;_tmp1BE.f1=(int)((unsigned int)((int)_tmp152));{
struct Cyc_Std_String_pa_struct _tmp1BD;_tmp1BD.tag=Cyc_Std_String_pa;_tmp1BD.f1=(
struct _tagged_arr)Cyc_Std_implode(_tmp154);{void*_tmp1BC[2]={& _tmp1BD,& _tmp1BE};
Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof(unsigned char),46),_tag_arr(_tmp1BC,sizeof(void*),2));}}});return 0;_LL410:;}
_tmp14A=({struct Cyc_List_List*_tmp1BF=_cycalloc(sizeof(struct Cyc_List_List));
_tmp1BF->hd=(void*)t;_tmp1BF->tl=_tmp14A;_tmp1BF;});break;}case 'f': _LL407: goto
_LL449;case 'F': _LL449: goto _LL450;case 'e': _LL450: goto _LL451;case 'E': _LL451: goto
_LL452;case 'g': _LL452: goto _LL453;case 'G': _LL453: goto _LL454;case 'a': _LL454: goto
_LL455;case 'A': _LL455: {struct Cyc_Core_Opt*_tmp1C9=({struct Cyc_Core_Opt*_tmp1DC=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1DC->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp1DC;});{struct Cyc_List_List*_tmp1CA=_tmp154;struct Cyc_List_List _tmp1D3;
struct Cyc_List_List*_tmp1D4;int _tmp1D6;_LL460: if(_tmp1CA == 0){goto _LL461;}else{
goto _LL462;}_LL462: if(_tmp1CA == 0){goto _LL464;}else{_tmp1D3=*_tmp1CA;_LL471:
_tmp1D6=(int)_tmp1D3.hd;goto _LL469;_LL469: _tmp1D4=_tmp1D3.tl;if(_tmp1D4 == 0){
goto _LL466;}else{goto _LL464;}}_LL466: if(_tmp1D6 == 'l'){goto _LL463;}else{goto
_LL464;}_LL464: goto _LL465;_LL461: t=Cyc_Absyn_at_typ(Cyc_Absyn_float_t,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmp1C9),Cyc_Absyn_empty_tqual());goto
_LL459;_LL463: t=Cyc_Absyn_at_typ(Cyc_Absyn_double_t(0),Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmp1C9),Cyc_Absyn_empty_tqual());goto
_LL459;_LL465:({struct Cyc_Std_Int_pa_struct _tmp1DA;_tmp1DA.tag=Cyc_Std_Int_pa;
_tmp1DA.f1=(int)((unsigned int)((int)_tmp152));{struct Cyc_Std_String_pa_struct
_tmp1D9;_tmp1D9.tag=Cyc_Std_String_pa;_tmp1D9.f1=(struct _tagged_arr)Cyc_Std_implode(
_tmp154);{void*_tmp1D8[2]={& _tmp1D9,& _tmp1DA};Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%s' is not allowed with %%%c",
sizeof(unsigned char),46),_tag_arr(_tmp1D8,sizeof(void*),2));}}});return 0;_LL459:;}
_tmp14A=({struct Cyc_List_List*_tmp1DB=_cycalloc(sizeof(struct Cyc_List_List));
_tmp1DB->hd=(void*)t;_tmp1DB->tl=_tmp14A;_tmp1DB;});break;}case 'c': _LL456: goto
_LL477;case 's': _LL477: {struct Cyc_Core_Opt*_tmp1DF=({struct Cyc_Core_Opt*_tmp1E1=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1E1->v=Cyc_Tcenv_lookup_type_vars(te);
_tmp1E1;});_tmp14A=({struct Cyc_List_List*_tmp1E0=_cycalloc(sizeof(struct Cyc_List_List));
_tmp1E0->hd=(void*)Cyc_Absyn_string_typ(Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk,_tmp1DF));_tmp1E0->tl=_tmp14A;_tmp1E0;});break;}case '[': _LL478:({
void*_tmp1E3[0]={};Cyc_Tcutil_terr(loc,_tag_arr("%%[ is not supported",sizeof(
unsigned char),21),_tag_arr(_tmp1E3,sizeof(void*),0));});return 0;case 'p': _LL482:({
void*_tmp1E5[0]={};Cyc_Tcutil_terr(loc,_tag_arr("%%p is not supported",sizeof(
unsigned char),21),_tag_arr(_tmp1E5,sizeof(void*),0));});return 0;case 'n': _LL484: {
struct Cyc_Core_Opt*_tmp1E7=({struct Cyc_Core_Opt*_tmp20E=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmp20E->v=Cyc_Tcenv_lookup_type_vars(te);_tmp20E;});{struct Cyc_List_List*
_tmp1E8=_tmp154;struct Cyc_List_List _tmp1F5;struct Cyc_List_List*_tmp1F6;int
_tmp1F8;struct Cyc_List_List _tmp1FB;struct Cyc_List_List*_tmp1FC;int _tmp1FE;struct
Cyc_List_List _tmp201;struct Cyc_List_List*_tmp202;struct Cyc_List_List _tmp204;
struct Cyc_List_List*_tmp205;int _tmp207;int _tmp209;_LL490: if(_tmp1E8 == 0){goto
_LL491;}else{goto _LL492;}_LL492: if(_tmp1E8 == 0){goto _LL494;}else{_tmp1F5=*
_tmp1E8;_LL505: _tmp1F8=(int)_tmp1F5.hd;goto _LL503;_LL503: _tmp1F6=_tmp1F5.tl;if(
_tmp1F6 == 0){goto _LL500;}else{goto _LL494;}}_LL500: if(_tmp1F8 == 'l'){goto _LL493;}
else{goto _LL494;}_LL494: if(_tmp1E8 == 0){goto _LL496;}else{_tmp1FB=*_tmp1E8;_LL511:
_tmp1FE=(int)_tmp1FB.hd;goto _LL509;_LL509: _tmp1FC=_tmp1FB.tl;if(_tmp1FC == 0){
goto _LL506;}else{goto _LL496;}}_LL506: if(_tmp1FE == 'h'){goto _LL495;}else{goto
_LL496;}_LL496: if(_tmp1E8 == 0){goto _LL498;}else{_tmp201=*_tmp1E8;_LL522: _tmp209=(
int)_tmp201.hd;goto _LL515;_LL515: _tmp202=_tmp201.tl;if(_tmp202 == 0){goto _LL498;}
else{_tmp204=*_tmp202;_LL520: _tmp207=(int)_tmp204.hd;goto _LL518;_LL518: _tmp205=
_tmp204.tl;if(_tmp205 == 0){goto _LL512;}else{goto _LL498;}}}_LL512: if(_tmp209 == 'h'?
_tmp207 == 'h': 0){goto _LL497;}else{goto _LL498;}_LL498: goto _LL499;_LL491: t=Cyc_Absyn_at_typ(
Cyc_Absyn_sint_t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,
_tmp1E7),Cyc_Absyn_empty_tqual());goto _LL489;_LL493: t=Cyc_Absyn_at_typ(Cyc_Absyn_ulong_t,
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmp1E7),Cyc_Absyn_empty_tqual());
goto _LL489;_LL495: t=Cyc_Absyn_at_typ(Cyc_Absyn_sshort_t,Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Formatstr_rk,_tmp1E7),Cyc_Absyn_empty_tqual());goto
_LL489;_LL497: t=Cyc_Absyn_at_typ(Cyc_Absyn_schar_t,Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Formatstr_rk,_tmp1E7),Cyc_Absyn_empty_tqual());goto _LL489;_LL499:({struct Cyc_Std_String_pa_struct
_tmp20C;_tmp20C.tag=Cyc_Std_String_pa;_tmp20C.f1=(struct _tagged_arr)Cyc_Std_implode(
_tmp154);{void*_tmp20B[1]={& _tmp20C};Cyc_Tcutil_terr(loc,_tag_arr("length modifier '%s' not allowed with %%n",
sizeof(unsigned char),42),_tag_arr(_tmp20B,sizeof(void*),1));}});return 0;_LL489:;}
_tmp14A=({struct Cyc_List_List*_tmp20D=_cycalloc(sizeof(struct Cyc_List_List));
_tmp20D->hd=(void*)t;_tmp20D->tl=_tmp14A;_tmp20D;});break;}case '%': _LL486: if(
_tmp158){({void*_tmp210[0]={};Cyc_Tcutil_terr(loc,_tag_arr("Assignment suppression (*) is not allowed with %%%%",
sizeof(unsigned char),52),_tag_arr(_tmp210,sizeof(void*),0));});return 0;}if(
_tmp156 != 0){({struct Cyc_Std_String_pa_struct _tmp212;_tmp212.tag=Cyc_Std_String_pa;
_tmp212.f1=(struct _tagged_arr)Cyc_Std_implode(_tmp156);{void*_tmp211[1]={&
_tmp212};Cyc_Tcutil_terr(loc,_tag_arr("width '%s' not allowed with %%%%",sizeof(
unsigned char),33),_tag_arr(_tmp211,sizeof(void*),1));}});return 0;}if(_tmp154 != 
0){({struct Cyc_Std_String_pa_struct _tmp214;_tmp214.tag=Cyc_Std_String_pa;_tmp214.f1=(
struct _tagged_arr)Cyc_Std_implode(_tmp154);{void*_tmp213[1]={& _tmp214};Cyc_Tcutil_terr(
loc,_tag_arr("length modifier '%s' not allowed with %%%%",sizeof(unsigned char),
43),_tag_arr(_tmp213,sizeof(void*),1));}});return 0;}break;default: _LL527: return 0;}}}}}
return Cyc_List_imp_rev(_tmp14A);}
