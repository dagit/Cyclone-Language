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
int Cyc_Core_intcmp(int,int);extern unsigned char Cyc_Core_Invalid_argument[21];
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
struct Cyc_Std___sFILE*,struct _tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_printf(
struct _tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_vrprintf(
struct _RegionHandle*r1,struct _tagged_arr fmt,struct _tagged_arr ap);static const int
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
Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[18];
extern struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y);extern void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);extern struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,
struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern unsigned char Cyc_List_Nth[8];extern void*Cyc_List_nth(
struct Cyc_List_List*x,int n);extern int Cyc_List_exists_c(int(*pred)(void*,void*),
void*env,struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};extern struct
_tuple0 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x);extern int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
extern void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*
x);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};extern unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
extern struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);
static const int Cyc_Position_Lex=0;static const int Cyc_Position_Parse=1;static const
int Cyc_Position_Elab=2;struct Cyc_Position_Error{struct _tagged_arr source;struct
Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern struct Cyc_Position_Error*
Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,struct _tagged_arr);extern
unsigned char Cyc_Position_Nocontext[14];extern int Cyc_Position_num_errors;extern
int Cyc_Position_max_errors;extern void Cyc_Position_post_error(struct Cyc_Position_Error*);
static const int Cyc_Absyn_Loc_n=0;static const int Cyc_Absyn_Rel_n=0;struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};static const int Cyc_Absyn_Abs_n=1;struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};
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
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);extern int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_empty_conref();extern struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*
x);extern void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern void*
Cyc_Absyn_compress_kb(void*);extern void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,
struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_t;extern void*Cyc_Absyn_ulonglong_t;
extern void*Cyc_Absyn_sint_t;extern void*Cyc_Absyn_slonglong_t;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple1*Cyc_Absyn_tunion_print_arg_qvar;extern struct _tuple1*Cyc_Absyn_tunion_scanf_arg_qvar;
extern void*Cyc_Absyn_bounds_one;extern void*Cyc_Absyn_atb_typ(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,void*b);extern struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct
Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Structfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _tagged_arr*);extern struct Cyc_Absyn_Structfield*Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*,struct _tagged_arr*);extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field(struct Cyc_Absyn_Uniondecl*,struct _tagged_arr*);
struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};extern struct _tuple4*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*,int);extern struct _tagged_arr Cyc_Absyn_attribute2string(void*);
extern int Cyc_Absyn_fntype_att(void*a);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern struct
_tagged_arr Cyc_Absynpp_typ2string(void*);extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*);extern struct _tagged_arr Cyc_Absynpp_kindbound2string(void*);extern struct
_tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Set_Set;extern
unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*
d,void*k);static const int Cyc_Tcenv_VarRes=0;struct Cyc_Tcenv_VarRes_struct{int tag;
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
ae;struct Cyc_Core_Opt*le;};extern void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);extern struct Cyc_Absyn_Structdecl**
Cyc_Tcenv_lookup_structdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);extern struct Cyc_Absyn_Uniondecl**Cyc_Tcenv_lookup_uniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);extern struct
Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple1*);extern struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);extern struct
Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple1*);extern struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);extern int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,
void*r1,void*r2);extern int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);
extern int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);extern struct
_tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);extern
unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);extern
unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(struct
_tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;extern void*Cyc_Tcutil_copy_type(
void*t);extern int Cyc_Tcutil_kind_leq(void*k1,void*k2);extern void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);extern void*Cyc_Tcutil_typ_kind(void*t);extern void*Cyc_Tcutil_compress(
void*t);extern void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);extern int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);extern int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,
void*);extern int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
extern int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_coerceable(void*);extern int Cyc_Tcutil_silent_castable(struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);extern int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);extern int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*);extern int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
extern int Cyc_Tcutil_is_function_type(void*t);extern int Cyc_Tcutil_is_pointer_type(
void*t);extern int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);extern struct Cyc_Core_Opt
Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;
extern struct Cyc_Core_Opt Cyc_Tcutil_mk;extern int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,
void*t,struct Cyc_Absyn_Exp*e);extern void*Cyc_Tcutil_max_arithmetic_type(void*,
void*);extern void Cyc_Tcutil_explain_failure();extern int Cyc_Tcutil_unify(void*,
void*);extern int Cyc_Tcutil_typecmp(void*,void*);extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);extern void*Cyc_Tcutil_rsubstitute(struct
_RegionHandle*,struct Cyc_List_List*,void*);extern int Cyc_Tcutil_subset_effect(int
set_to_empty,void*e1,void*e2);extern int Cyc_Tcutil_region_in_effect(int constrain,
void*r,void*e);extern void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);extern
struct Cyc_Absyn_Exp*Cyc_Tcutil_default_initializer(struct Cyc_Tcenv_Tenv*,void*,
struct Cyc_Position_Segment*);struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};
extern struct _tuple5*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
struct _tuple6{struct Cyc_List_List*f1;struct _RegionHandle*f2;};extern struct
_tuple5*Cyc_Tcutil_r_make_inst_var(struct _tuple6*,struct Cyc_Absyn_Tvar*);extern
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _tagged_arr*fn);extern void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);extern void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);extern void Cyc_Tcutil_check_fndecl_valid_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);extern void
Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct
Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);extern void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr err_msg);
extern void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,struct Cyc_List_List*);
extern void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);
extern void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*,unsigned int i,
struct Cyc_Absyn_Conref*);extern int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*
b);extern int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2);extern struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields);extern int Cyc_Tcutil_is_tagged_pointer_typ(void*);extern int
Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);extern void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple7{int f1;void*f2;
};extern struct _tuple7 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);extern struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);extern
int*Cyc_Tcutil_new_tvar_id();extern void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);extern int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);extern void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
extern int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);extern
int Cyc_Tcutil_bits_only(void*t);extern int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);extern void*Cyc_Tcutil_snd_tqt(struct _tuple4*);extern
int Cyc_Tcutil_supports_default(void*);extern int Cyc_Tcutil_is_noreturn(void*);
extern void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
extern void Cyc_Tc_tcStructdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct
Cyc_Position_Segment*,struct Cyc_Absyn_Structdecl*);extern void Cyc_Tc_tcUniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Uniondecl*);extern void Cyc_Tc_tcTuniondecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Tuniondecl*);extern
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,
struct Cyc_Absyn_Enumdecl*);unsigned char Cyc_Tcutil_TypeErr[12]="\000\000\000\000TypeErr";
extern void Cyc_Tcutil_unify_it(void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)
0;void*Cyc_Tcutil_t2_failure=(void*)0;struct _tagged_arr Cyc_Tcutil_failure_reason=(
struct _tagged_arr){(void*)0,(void*)0,(void*)(0 + 0)};void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors){return;}Cyc_Std_fflush((
struct Cyc_Std___sFILE*)Cyc_Std_stderr);{struct _tagged_arr s1=Cyc_Absynpp_typ2string(
Cyc_Tcutil_t1_failure);struct _tagged_arr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=8;({struct Cyc_Std_String_pa_struct _tmp1;_tmp1.tag=Cyc_Std_String_pa;_tmp1.f1=(
struct _tagged_arr)s1;{void*_tmp0[1]={& _tmp1};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\t%s and ",sizeof(unsigned char),9),_tag_arr(_tmp0,sizeof(void*),1));}});
pos +=_get_arr_size(s1,sizeof(unsigned char))+ 5;if(pos >= 80){({void*_tmp2[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n\t",sizeof(unsigned char),3),_tag_arr(
_tmp2,sizeof(void*),0));});pos=8;}({struct Cyc_Std_String_pa_struct _tmp4;_tmp4.tag=
Cyc_Std_String_pa;_tmp4.f1=(struct _tagged_arr)s2;{void*_tmp3[1]={& _tmp4};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s ",sizeof(unsigned char),4),_tag_arr(_tmp3,sizeof(
void*),1));}});pos +=_get_arr_size(s2,sizeof(unsigned char))+ 1;if(pos >= 80){({
void*_tmp5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n\t",sizeof(
unsigned char),3),_tag_arr(_tmp5,sizeof(void*),0));});pos=8;}({void*_tmp6[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("failed to unify. ",sizeof(unsigned char),
18),_tag_arr(_tmp6,sizeof(void*),0));});pos +=17;if(Cyc_Tcutil_failure_reason.curr
!= ((struct _tagged_arr)_tag_arr(0,0,0)).curr){if(pos >= 80){({void*_tmp7[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n\t",sizeof(unsigned char),3),_tag_arr(
_tmp7,sizeof(void*),0));});pos=8;}({struct Cyc_Std_String_pa_struct _tmp9;_tmp9.tag=
Cyc_Std_String_pa;_tmp9.f1=(struct _tagged_arr)Cyc_Tcutil_failure_reason;{void*
_tmp8[1]={& _tmp9};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s",sizeof(
unsigned char),3),_tag_arr(_tmp8,sizeof(void*),1));}});}({void*_tmpA[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("\n",sizeof(unsigned char),2),_tag_arr(_tmpA,sizeof(void*),
0));});Cyc_Std_fflush((struct Cyc_Std___sFILE*)Cyc_Std_stderr);}}void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*loc,struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Position_post_error(
Cyc_Position_mk_err_elab(loc,(struct _tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,
fmt,ap)));}void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct _tagged_arr ap){
struct _tagged_arr msg=(struct _tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,
fmt,ap);({struct Cyc_Std_String_pa_struct _tmpC;_tmpC.tag=Cyc_Std_String_pa;_tmpC.f1=(
struct _tagged_arr)msg;{void*_tmpB[1]={& _tmpC};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Error: %s\n",sizeof(unsigned char),11),_tag_arr(_tmpB,sizeof(void*),1));}});
Cyc_Std_fflush((struct Cyc_Std___sFILE*)Cyc_Std_stderr);(int)_throw((void*)Cyc_Tcutil_TypeErr);}
static struct _tagged_arr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){return*tv->name;}
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){({
struct Cyc_Std_String_pa_struct _tmpF;_tmpF.tag=Cyc_Std_String_pa;_tmpF.f1=(struct
_tagged_arr)Cyc_Absynpp_kindbound2string((void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);{
struct Cyc_Std_String_pa_struct _tmpE;_tmpE.tag=Cyc_Std_String_pa;_tmpE.f1=(struct
_tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd);{void*_tmpD[2]={&
_tmpE,& _tmpF};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::%s ",sizeof(
unsigned char),8),_tag_arr(_tmpD,sizeof(void*),2));}}});}({void*_tmp10[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("\n",sizeof(unsigned char),2),_tag_arr(_tmp10,sizeof(
void*),0));});Cyc_Std_fflush((struct Cyc_Std___sFILE*)Cyc_Std_stderr);}static
struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=
0;void Cyc_Tcutil_warn(struct Cyc_Position_Segment*sg,struct _tagged_arr fmt,struct
_tagged_arr ap){struct _tagged_arr msg=(struct _tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,
fmt,ap);Cyc_Tcutil_warning_segs=({struct Cyc_List_List*_tmp11=_cycalloc(sizeof(
struct Cyc_List_List));_tmp11->hd=sg;_tmp11->tl=Cyc_Tcutil_warning_segs;_tmp11;});
Cyc_Tcutil_warning_msgs=({struct Cyc_List_List*_tmp12=_cycalloc(sizeof(struct Cyc_List_List));
_tmp12->hd=({struct _tagged_arr*_tmp13=_cycalloc(sizeof(struct _tagged_arr));
_tmp13[0]=msg;_tmp13;});_tmp12->tl=Cyc_Tcutil_warning_msgs;_tmp12;});}void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0){return;}({void*_tmp14[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("***Warnings***\n",sizeof(unsigned char),16),_tag_arr(_tmp14,sizeof(
void*),0));});{struct Cyc_List_List*_tmp15=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs
!= 0){({struct Cyc_Std_String_pa_struct _tmp18;_tmp18.tag=Cyc_Std_String_pa;_tmp18.f1=(
struct _tagged_arr)*((struct _tagged_arr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd);{
struct Cyc_Std_String_pa_struct _tmp17;_tmp17.tag=Cyc_Std_String_pa;_tmp17.f1=(
struct _tagged_arr)*((struct _tagged_arr*)((struct Cyc_List_List*)_check_null(
_tmp15))->hd);{void*_tmp16[2]={& _tmp17,& _tmp18};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%s: %s\n",sizeof(unsigned char),8),_tag_arr(_tmp16,sizeof(void*),2));}}});
_tmp15=_tmp15->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}({
void*_tmp19[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("**************\n",
sizeof(unsigned char),16),_tag_arr(_tmp19,sizeof(void*),0));});Cyc_Std_fflush((
struct Cyc_Std___sFILE*)Cyc_Std_stderr);}}struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=
0;static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){return*((int*)_check_null(tv1->identity))- *((int*)_check_null(tv2->identity));}
void*Cyc_Tcutil_compress(void*t){void*_tmp1A=t;struct Cyc_Core_Opt*_tmp26;struct
Cyc_Core_Opt*_tmp28;struct Cyc_Core_Opt**_tmp2A;struct Cyc_Core_Opt*_tmp2B;struct
Cyc_Core_Opt*_tmp2D;struct Cyc_Core_Opt**_tmp2F;_LL28: if((unsigned int)_tmp1A > 3?*((
int*)_tmp1A)== Cyc_Absyn_Evar: 0){_LL39: _tmp26=((struct Cyc_Absyn_Evar_struct*)
_tmp1A)->f2;if(_tmp26 == 0){goto _LL29;}else{goto _LL30;}}else{goto _LL30;}_LL30: if((
unsigned int)_tmp1A > 3?*((int*)_tmp1A)== Cyc_Absyn_Evar: 0){_LL41: _tmp28=((struct
Cyc_Absyn_Evar_struct*)_tmp1A)->f2;_tmp2A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp1A)->f2;goto _LL31;}else{goto _LL32;}_LL32: if((unsigned int)_tmp1A > 3?*((int*)
_tmp1A)== Cyc_Absyn_TypedefType: 0){_LL44: _tmp2B=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1A)->f3;if(_tmp2B == 0){goto _LL33;}else{goto _LL34;}}else{goto _LL34;}_LL34: if((
unsigned int)_tmp1A > 3?*((int*)_tmp1A)== Cyc_Absyn_TypedefType: 0){_LL46: _tmp2D=((
struct Cyc_Absyn_TypedefType_struct*)_tmp1A)->f3;_tmp2F=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp1A)->f3;goto _LL35;}else{goto _LL36;}_LL36:
goto _LL37;_LL29: return t;_LL31: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp2A))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp2A))->v){*_tmp2A=({struct Cyc_Core_Opt*_tmp30=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp30->v=(void*)t2;_tmp30;});}return t2;}_LL33: return t;_LL35: {void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(*_tmp2F))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp2F))->v){*_tmp2F=({struct Cyc_Core_Opt*_tmp31=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmp31->v=(void*)t2;_tmp31;});}return t2;}_LL37: return t;
_LL27:;}void*Cyc_Tcutil_copy_type(void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(
struct Cyc_List_List*ts){return Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static
struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(struct Cyc_Absyn_Conref*c){void*
_tmp32=(void*)c->v;void*_tmp3A;struct Cyc_Absyn_Conref*_tmp3C;_LL52: if(_tmp32 == (
void*)Cyc_Absyn_No_constr){goto _LL53;}else{goto _LL54;}_LL54: if((unsigned int)
_tmp32 > 1?*((int*)_tmp32)== Cyc_Absyn_Eq_constr: 0){_LL59: _tmp3A=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp32)->f1;goto _LL55;}else{goto _LL56;}_LL56: if((
unsigned int)_tmp32 > 1?*((int*)_tmp32)== Cyc_Absyn_Forward_constr: 0){_LL61:
_tmp3C=((struct Cyc_Absyn_Forward_constr_struct*)_tmp32)->f1;goto _LL57;}else{goto
_LL51;}_LL53: return Cyc_Absyn_empty_conref();_LL55: return Cyc_Absyn_new_conref(
_tmp3A);_LL57: return Cyc_Tcutil_copy_conref(_tmp3C);_LL51:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp3E=Cyc_Absyn_compress_kb(kb);void*_tmp46;void*_tmp48;_LL64: if(*((
int*)_tmp3E)== Cyc_Absyn_Eq_kb){_LL71: _tmp46=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp3E)->f1;goto _LL65;}else{goto _LL66;}_LL66: if(*((int*)_tmp3E)== Cyc_Absyn_Unknown_kb){
goto _LL67;}else{goto _LL68;}_LL68: if(*((int*)_tmp3E)== Cyc_Absyn_Less_kb){_LL73:
_tmp48=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp3E)->f2;goto _LL69;}else{goto
_LL63;}_LL65: return(void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp4A=_cycalloc(
sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp4A[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp4B;_tmp4B.tag=Cyc_Absyn_Eq_kb;_tmp4B.f1=(void*)_tmp46;_tmp4B;});_tmp4A;});
_LL67: return(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp4C=_cycalloc(sizeof(
struct Cyc_Absyn_Unknown_kb_struct));_tmp4C[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp4D;_tmp4D.tag=Cyc_Absyn_Unknown_kb;_tmp4D.f1=0;_tmp4D;});_tmp4C;});_LL69:
return(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp4E=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_struct));
_tmp4E[0]=({struct Cyc_Absyn_Less_kb_struct _tmp4F;_tmp4F.tag=Cyc_Absyn_Less_kb;
_tmp4F.f1=0;_tmp4F.f2=(void*)_tmp48;_tmp4F;});_tmp4E;});_LL63:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){return({struct Cyc_Absyn_Tvar*
_tmp50=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_tmp50->name=tv->name;_tmp50->identity=
0;_tmp50->kind=(void*)Cyc_Tcutil_copy_kindbound((void*)tv->kind);_tmp50;});}
static struct _tuple2*Cyc_Tcutil_copy_arg(struct _tuple2*arg){void*_tmp53;struct Cyc_Absyn_Tqual
_tmp55;struct Cyc_Core_Opt*_tmp57;struct _tuple2 _tmp51=*arg;_LL88: _tmp57=_tmp51.f1;
goto _LL86;_LL86: _tmp55=_tmp51.f2;goto _LL84;_LL84: _tmp53=_tmp51.f3;goto _LL82;
_LL82: return({struct _tuple2*_tmp59=_cycalloc(sizeof(struct _tuple2));_tmp59->f1=
_tmp57;_tmp59->f2=_tmp55;_tmp59->f3=Cyc_Tcutil_copy_type(_tmp53);_tmp59;});}
static struct _tuple4*Cyc_Tcutil_copy_tqt(struct _tuple4*arg){void*_tmp5C;struct Cyc_Absyn_Tqual
_tmp5E;struct _tuple4 _tmp5A=*arg;_LL95: _tmp5E=_tmp5A.f1;goto _LL93;_LL93: _tmp5C=
_tmp5A.f2;goto _LL91;_LL91: return({struct _tuple4*_tmp60=_cycalloc(sizeof(struct
_tuple4));_tmp60->f1=_tmp5E;_tmp60->f2=Cyc_Tcutil_copy_type(_tmp5C);_tmp60;});}
static struct Cyc_Absyn_Structfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Structfield*
f){return({struct Cyc_Absyn_Structfield*_tmp61=_cycalloc(sizeof(struct Cyc_Absyn_Structfield));
_tmp61->name=f->name;_tmp61->tq=f->tq;_tmp61->type=(void*)Cyc_Tcutil_copy_type((
void*)f->type);_tmp61->width=f->width;_tmp61->attributes=f->attributes;_tmp61;});}
struct _tuple8{void*f1;void*f2;};static struct _tuple8*Cyc_Tcutil_copy_rgncmp(
struct _tuple8*x){struct _tuple8 _tmp64;void*_tmp65;void*_tmp67;struct _tuple8*
_tmp62=x;_tmp64=*_tmp62;_LL104: _tmp67=_tmp64.f1;goto _LL102;_LL102: _tmp65=_tmp64.f2;
goto _LL99;_LL99: return({struct _tuple8*_tmp69=_cycalloc(sizeof(struct _tuple8));
_tmp69->f1=Cyc_Tcutil_copy_type(_tmp67);_tmp69->f2=Cyc_Tcutil_copy_type(_tmp65);
_tmp69;});}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*
f){return({struct Cyc_Absyn_Enumfield*_tmp6A=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));
_tmp6A->name=f->name;_tmp6A->tag=f->tag;_tmp6A->loc=f->loc;_tmp6A;});}void*Cyc_Tcutil_copy_type(
void*t){void*_tmp6B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp9F;struct Cyc_Absyn_TunionInfo
_tmpA1;void*_tmpA3;struct Cyc_List_List*_tmpA5;void*_tmpA7;struct Cyc_Absyn_TunionFieldInfo
_tmpA9;struct Cyc_List_List*_tmpAB;void*_tmpAD;struct Cyc_Absyn_PtrInfo _tmpAF;
struct Cyc_Absyn_Conref*_tmpB1;struct Cyc_Absyn_Tqual _tmpB3;struct Cyc_Absyn_Conref*
_tmpB5;void*_tmpB7;void*_tmpB9;void*_tmpBB;void*_tmpBD;int _tmpBF;struct Cyc_Absyn_Exp*
_tmpC1;struct Cyc_Absyn_Tqual _tmpC3;void*_tmpC5;struct Cyc_Absyn_FnInfo _tmpC7;
struct Cyc_List_List*_tmpC9;struct Cyc_List_List*_tmpCB;struct Cyc_Absyn_VarargInfo*
_tmpCD;int _tmpCF;struct Cyc_List_List*_tmpD1;void*_tmpD3;struct Cyc_Core_Opt*
_tmpD5;struct Cyc_List_List*_tmpD7;struct Cyc_List_List*_tmpD9;struct Cyc_List_List*
_tmpDB;struct _tuple1*_tmpDD;struct Cyc_List_List*_tmpDF;struct _tuple1*_tmpE1;
struct Cyc_List_List*_tmpE3;struct Cyc_List_List*_tmpE5;struct Cyc_Absyn_Enumdecl*
_tmpE7;struct _tuple1*_tmpE9;struct Cyc_List_List*_tmpEB;void*_tmpED;void*_tmpEF;
struct Cyc_List_List*_tmpF1;struct _tuple1*_tmpF3;void*_tmpF5;struct Cyc_List_List*
_tmpF7;void*_tmpF9;_LL109: if(_tmp6B == (void*)Cyc_Absyn_VoidType){goto _LL110;}
else{goto _LL111;}_LL111: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_Evar:
0){goto _LL112;}else{goto _LL113;}_LL113: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)
== Cyc_Absyn_VarType: 0){_LL160: _tmp9F=((struct Cyc_Absyn_VarType_struct*)_tmp6B)->f1;
goto _LL114;}else{goto _LL115;}_LL115: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== 
Cyc_Absyn_TunionType: 0){_LL162: _tmpA1=((struct Cyc_Absyn_TunionType_struct*)
_tmp6B)->f1;_LL168: _tmpA7=(void*)_tmpA1.tunion_info;goto _LL166;_LL166: _tmpA5=
_tmpA1.targs;goto _LL164;_LL164: _tmpA3=(void*)_tmpA1.rgn;goto _LL116;}else{goto
_LL117;}_LL117: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_TunionFieldType:
0){_LL170: _tmpA9=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp6B)->f1;_LL174:
_tmpAD=(void*)_tmpA9.field_info;goto _LL172;_LL172: _tmpAB=_tmpA9.targs;goto _LL118;}
else{goto _LL119;}_LL119: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_PointerType:
0){_LL176: _tmpAF=((struct Cyc_Absyn_PointerType_struct*)_tmp6B)->f1;_LL186: _tmpB9=(
void*)_tmpAF.elt_typ;goto _LL184;_LL184: _tmpB7=(void*)_tmpAF.rgn_typ;goto _LL182;
_LL182: _tmpB5=_tmpAF.nullable;goto _LL180;_LL180: _tmpB3=_tmpAF.tq;goto _LL178;
_LL178: _tmpB1=_tmpAF.bounds;goto _LL120;}else{goto _LL121;}_LL121: if((unsigned int)
_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_IntType: 0){_LL190: _tmpBD=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp6B)->f1;goto _LL188;_LL188: _tmpBB=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp6B)->f2;goto _LL122;}else{goto _LL123;}_LL123: if(
_tmp6B == (void*)Cyc_Absyn_FloatType){goto _LL124;}else{goto _LL125;}_LL125: if((
unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_DoubleType: 0){_LL192: _tmpBF=((
struct Cyc_Absyn_DoubleType_struct*)_tmp6B)->f1;goto _LL126;}else{goto _LL127;}
_LL127: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_ArrayType: 0){
_LL198: _tmpC5=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp6B)->f1;goto _LL196;
_LL196: _tmpC3=((struct Cyc_Absyn_ArrayType_struct*)_tmp6B)->f2;goto _LL194;_LL194:
_tmpC1=((struct Cyc_Absyn_ArrayType_struct*)_tmp6B)->f3;goto _LL128;}else{goto
_LL129;}_LL129: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_FnType: 0){
_LL200: _tmpC7=((struct Cyc_Absyn_FnType_struct*)_tmp6B)->f1;_LL216: _tmpD7=_tmpC7.tvars;
goto _LL214;_LL214: _tmpD5=_tmpC7.effect;goto _LL212;_LL212: _tmpD3=(void*)_tmpC7.ret_typ;
goto _LL210;_LL210: _tmpD1=_tmpC7.args;goto _LL208;_LL208: _tmpCF=_tmpC7.c_varargs;
goto _LL206;_LL206: _tmpCD=_tmpC7.cyc_varargs;goto _LL204;_LL204: _tmpCB=_tmpC7.rgn_po;
goto _LL202;_LL202: _tmpC9=_tmpC7.attributes;goto _LL130;}else{goto _LL131;}_LL131:
if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_TupleType: 0){_LL218:
_tmpD9=((struct Cyc_Absyn_TupleType_struct*)_tmp6B)->f1;goto _LL132;}else{goto
_LL133;}_LL133: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_StructType:
0){_LL222: _tmpDD=((struct Cyc_Absyn_StructType_struct*)_tmp6B)->f1;goto _LL220;
_LL220: _tmpDB=((struct Cyc_Absyn_StructType_struct*)_tmp6B)->f2;goto _LL134;}else{
goto _LL135;}_LL135: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_UnionType:
0){_LL226: _tmpE1=((struct Cyc_Absyn_UnionType_struct*)_tmp6B)->f1;goto _LL224;
_LL224: _tmpDF=((struct Cyc_Absyn_UnionType_struct*)_tmp6B)->f2;goto _LL136;}else{
goto _LL137;}_LL137: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_AnonStructType:
0){_LL228: _tmpE3=((struct Cyc_Absyn_AnonStructType_struct*)_tmp6B)->f1;goto _LL138;}
else{goto _LL139;}_LL139: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_AnonUnionType:
0){_LL230: _tmpE5=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp6B)->f1;goto _LL140;}
else{goto _LL141;}_LL141: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_EnumType:
0){_LL234: _tmpE9=((struct Cyc_Absyn_EnumType_struct*)_tmp6B)->f1;goto _LL232;
_LL232: _tmpE7=((struct Cyc_Absyn_EnumType_struct*)_tmp6B)->f2;goto _LL142;}else{
goto _LL143;}_LL143: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_AnonEnumType:
0){_LL236: _tmpEB=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp6B)->f1;goto _LL144;}
else{goto _LL145;}_LL145: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_SizeofType:
0){_LL238: _tmpED=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp6B)->f1;goto
_LL146;}else{goto _LL147;}_LL147: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_RgnHandleType:
0){_LL240: _tmpEF=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp6B)->f1;goto
_LL148;}else{goto _LL149;}_LL149: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_TypedefType:
0){_LL244: _tmpF3=((struct Cyc_Absyn_TypedefType_struct*)_tmp6B)->f1;goto _LL242;
_LL242: _tmpF1=((struct Cyc_Absyn_TypedefType_struct*)_tmp6B)->f2;goto _LL150;}
else{goto _LL151;}_LL151: if(_tmp6B == (void*)Cyc_Absyn_HeapRgn){goto _LL152;}else{
goto _LL153;}_LL153: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_AccessEff:
0){_LL246: _tmpF5=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp6B)->f1;goto
_LL154;}else{goto _LL155;}_LL155: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_JoinEff:
0){_LL248: _tmpF7=((struct Cyc_Absyn_JoinEff_struct*)_tmp6B)->f1;goto _LL156;}else{
goto _LL157;}_LL157: if((unsigned int)_tmp6B > 3?*((int*)_tmp6B)== Cyc_Absyn_RgnsEff:
0){_LL250: _tmpF9=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp6B)->f1;goto _LL158;}
else{goto _LL108;}_LL110: goto _LL112;_LL112: return t;_LL114: return(void*)({struct
Cyc_Absyn_VarType_struct*_tmpFB=_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));
_tmpFB[0]=({struct Cyc_Absyn_VarType_struct _tmpFC;_tmpFC.tag=Cyc_Absyn_VarType;
_tmpFC.f1=Cyc_Tcutil_copy_tvar(_tmp9F);_tmpFC;});_tmpFB;});_LL116: return(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpFD=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));
_tmpFD[0]=({struct Cyc_Absyn_TunionType_struct _tmpFE;_tmpFE.tag=Cyc_Absyn_TunionType;
_tmpFE.f1=({struct Cyc_Absyn_TunionInfo _tmpFF;_tmpFF.tunion_info=(void*)_tmpA7;
_tmpFF.targs=Cyc_Tcutil_copy_types(_tmpA5);_tmpFF.rgn=(void*)Cyc_Tcutil_copy_type(
_tmpA3);_tmpFF;});_tmpFE;});_tmpFD;});_LL118: return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp100=_cycalloc(sizeof(struct Cyc_Absyn_TunionFieldType_struct));_tmp100[0]=({
struct Cyc_Absyn_TunionFieldType_struct _tmp101;_tmp101.tag=Cyc_Absyn_TunionFieldType;
_tmp101.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp102;_tmp102.field_info=(void*)
_tmpAD;_tmp102.targs=Cyc_Tcutil_copy_types(_tmpAB);_tmp102;});_tmp101;});_tmp100;});
_LL120: {void*_tmp103=Cyc_Tcutil_copy_type(_tmpB9);void*_tmp104=Cyc_Tcutil_copy_type(
_tmpB7);struct Cyc_Absyn_Conref*_tmp105=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmpB5);struct Cyc_Absyn_Tqual _tmp106=_tmpB3;struct Cyc_Absyn_Conref*
_tmp107=Cyc_Tcutil_copy_conref(_tmpB1);return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp108=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_struct));_tmp108[0]=({
struct Cyc_Absyn_PointerType_struct _tmp109;_tmp109.tag=Cyc_Absyn_PointerType;
_tmp109.f1=({struct Cyc_Absyn_PtrInfo _tmp10A;_tmp10A.elt_typ=(void*)_tmp103;
_tmp10A.rgn_typ=(void*)_tmp104;_tmp10A.nullable=_tmp105;_tmp10A.tq=_tmp106;
_tmp10A.bounds=_tmp107;_tmp10A;});_tmp109;});_tmp108;});}_LL122: return(void*)({
struct Cyc_Absyn_IntType_struct*_tmp10B=_cycalloc(sizeof(struct Cyc_Absyn_IntType_struct));
_tmp10B[0]=({struct Cyc_Absyn_IntType_struct _tmp10C;_tmp10C.tag=Cyc_Absyn_IntType;
_tmp10C.f1=(void*)_tmpBD;_tmp10C.f2=(void*)_tmpBB;_tmp10C;});_tmp10B;});_LL124:
return t;_LL126: return(void*)({struct Cyc_Absyn_DoubleType_struct*_tmp10D=
_cycalloc_atomic(sizeof(struct Cyc_Absyn_DoubleType_struct));_tmp10D[0]=({struct
Cyc_Absyn_DoubleType_struct _tmp10E;_tmp10E.tag=Cyc_Absyn_DoubleType;_tmp10E.f1=
_tmpBF;_tmp10E;});_tmp10D;});_LL128: return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp10F=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_struct));_tmp10F[0]=({struct
Cyc_Absyn_ArrayType_struct _tmp110;_tmp110.tag=Cyc_Absyn_ArrayType;_tmp110.f1=(
void*)Cyc_Tcutil_copy_type(_tmpC5);_tmp110.f2=_tmpC3;_tmp110.f3=_tmpC1;_tmp110;});
_tmp10F;});_LL130: {struct Cyc_List_List*_tmp111=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_tvar,_tmpD7);struct Cyc_Core_Opt*_tmp112=_tmpD5 == 0? 0:({struct Cyc_Core_Opt*
_tmp11C=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp11C->v=(void*)Cyc_Tcutil_copy_type((
void*)_tmpD5->v);_tmp11C;});void*_tmp113=Cyc_Tcutil_copy_type(_tmpD3);struct Cyc_List_List*
_tmp114=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpD1);int _tmp115=_tmpCF;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmpCD != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_check_null(_tmpCD);cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp116=_cycalloc(
sizeof(struct Cyc_Absyn_VarargInfo));_tmp116->name=cv->name;_tmp116->tq=cv->tq;
_tmp116->type=(void*)Cyc_Tcutil_copy_type((void*)cv->type);_tmp116->inject=cv->inject;
_tmp116;});}{struct Cyc_List_List*_tmp117=((struct Cyc_List_List*(*)(struct _tuple8*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,
_tmpCB);struct Cyc_List_List*_tmp118=_tmpC9;return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp119=_cycalloc(sizeof(struct Cyc_Absyn_FnType_struct));_tmp119[0]=({struct Cyc_Absyn_FnType_struct
_tmp11A;_tmp11A.tag=Cyc_Absyn_FnType;_tmp11A.f1=({struct Cyc_Absyn_FnInfo _tmp11B;
_tmp11B.tvars=_tmp111;_tmp11B.effect=_tmp112;_tmp11B.ret_typ=(void*)_tmp113;
_tmp11B.args=_tmp114;_tmp11B.c_varargs=_tmp115;_tmp11B.cyc_varargs=cyc_varargs2;
_tmp11B.rgn_po=_tmp117;_tmp11B.attributes=_tmp118;_tmp11B;});_tmp11A;});_tmp119;});}}
_LL132: return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp11D=_cycalloc(sizeof(
struct Cyc_Absyn_TupleType_struct));_tmp11D[0]=({struct Cyc_Absyn_TupleType_struct
_tmp11E;_tmp11E.tag=Cyc_Absyn_TupleType;_tmp11E.f1=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,
_tmpD9);_tmp11E;});_tmp11D;});_LL134: return(void*)({struct Cyc_Absyn_StructType_struct*
_tmp11F=_cycalloc(sizeof(struct Cyc_Absyn_StructType_struct));_tmp11F[0]=({struct
Cyc_Absyn_StructType_struct _tmp120;_tmp120.tag=Cyc_Absyn_StructType;_tmp120.f1=
_tmpDD;_tmp120.f2=Cyc_Tcutil_copy_types(_tmpDB);_tmp120.f3=0;_tmp120;});_tmp11F;});
_LL136: return(void*)({struct Cyc_Absyn_UnionType_struct*_tmp121=_cycalloc(sizeof(
struct Cyc_Absyn_UnionType_struct));_tmp121[0]=({struct Cyc_Absyn_UnionType_struct
_tmp122;_tmp122.tag=Cyc_Absyn_UnionType;_tmp122.f1=_tmpE1;_tmp122.f2=Cyc_Tcutil_copy_types(
_tmpDF);_tmp122.f3=0;_tmp122;});_tmp121;});_LL138: return(void*)({struct Cyc_Absyn_AnonStructType_struct*
_tmp123=_cycalloc(sizeof(struct Cyc_Absyn_AnonStructType_struct));_tmp123[0]=({
struct Cyc_Absyn_AnonStructType_struct _tmp124;_tmp124.tag=Cyc_Absyn_AnonStructType;
_tmp124.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*f)(struct Cyc_Absyn_Structfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpE3);_tmp124;});
_tmp123;});_LL140: return(void*)({struct Cyc_Absyn_AnonUnionType_struct*_tmp125=
_cycalloc(sizeof(struct Cyc_Absyn_AnonUnionType_struct));_tmp125[0]=({struct Cyc_Absyn_AnonUnionType_struct
_tmp126;_tmp126.tag=Cyc_Absyn_AnonUnionType;_tmp126.f1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(*f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpE5);_tmp126;});_tmp125;});_LL142:
return(void*)({struct Cyc_Absyn_EnumType_struct*_tmp127=_cycalloc(sizeof(struct
Cyc_Absyn_EnumType_struct));_tmp127[0]=({struct Cyc_Absyn_EnumType_struct _tmp128;
_tmp128.tag=Cyc_Absyn_EnumType;_tmp128.f1=_tmpE9;_tmp128.f2=_tmpE7;_tmp128;});
_tmp127;});_LL144: return(void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp129=
_cycalloc(sizeof(struct Cyc_Absyn_AnonEnumType_struct));_tmp129[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp12A;_tmp12A.tag=Cyc_Absyn_AnonEnumType;_tmp12A.f1=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpEB);_tmp12A;});_tmp129;});_LL146:
return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp12B=_cycalloc(sizeof(struct
Cyc_Absyn_SizeofType_struct));_tmp12B[0]=({struct Cyc_Absyn_SizeofType_struct
_tmp12C;_tmp12C.tag=Cyc_Absyn_SizeofType;_tmp12C.f1=(void*)Cyc_Tcutil_copy_type(
_tmpED);_tmp12C;});_tmp12B;});_LL148: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp12D=_cycalloc(sizeof(struct Cyc_Absyn_RgnHandleType_struct));_tmp12D[0]=({
struct Cyc_Absyn_RgnHandleType_struct _tmp12E;_tmp12E.tag=Cyc_Absyn_RgnHandleType;
_tmp12E.f1=(void*)Cyc_Tcutil_copy_type(_tmpEF);_tmp12E;});_tmp12D;});_LL150:
return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp12F=_cycalloc(sizeof(
struct Cyc_Absyn_TypedefType_struct));_tmp12F[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp130;_tmp130.tag=Cyc_Absyn_TypedefType;_tmp130.f1=_tmpF3;_tmp130.f2=Cyc_Tcutil_copy_types(
_tmpF1);_tmp130.f3=0;_tmp130;});_tmp12F;});_LL152: return t;_LL154: return(void*)({
struct Cyc_Absyn_AccessEff_struct*_tmp131=_cycalloc(sizeof(struct Cyc_Absyn_AccessEff_struct));
_tmp131[0]=({struct Cyc_Absyn_AccessEff_struct _tmp132;_tmp132.tag=Cyc_Absyn_AccessEff;
_tmp132.f1=(void*)Cyc_Tcutil_copy_type(_tmpF5);_tmp132;});_tmp131;});_LL156:
return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp133=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));
_tmp133[0]=({struct Cyc_Absyn_JoinEff_struct _tmp134;_tmp134.tag=Cyc_Absyn_JoinEff;
_tmp134.f1=Cyc_Tcutil_copy_types(_tmpF7);_tmp134;});_tmp133;});_LL158: return(
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp135=_cycalloc(sizeof(struct Cyc_Absyn_RgnsEff_struct));
_tmp135[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp136;_tmp136.tag=Cyc_Absyn_RgnsEff;
_tmp136.f1=(void*)Cyc_Tcutil_copy_type(_tmpF9);_tmp136;});_tmp135;});_LL108:;}
int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(k1 == k2){return 1;}{struct _tuple8
_tmp138=({struct _tuple8 _tmp137;_tmp137.f1=k1;_tmp137.f2=k2;_tmp137;});void*
_tmp142;void*_tmp144;void*_tmp146;void*_tmp148;void*_tmp14A;void*_tmp14C;_LL314:
_LL325: _tmp144=_tmp138.f1;if(_tmp144 == (void*)Cyc_Absyn_BoxKind){goto _LL323;}
else{goto _LL316;}_LL323: _tmp142=_tmp138.f2;if(_tmp142 == (void*)Cyc_Absyn_MemKind){
goto _LL315;}else{goto _LL316;}_LL316: _LL329: _tmp148=_tmp138.f1;if(_tmp148 == (void*)
Cyc_Absyn_BoxKind){goto _LL327;}else{goto _LL318;}_LL327: _tmp146=_tmp138.f2;if(
_tmp146 == (void*)Cyc_Absyn_AnyKind){goto _LL317;}else{goto _LL318;}_LL318: _LL333:
_tmp14C=_tmp138.f1;if(_tmp14C == (void*)Cyc_Absyn_MemKind){goto _LL331;}else{goto
_LL320;}_LL331: _tmp14A=_tmp138.f2;if(_tmp14A == (void*)Cyc_Absyn_AnyKind){goto
_LL319;}else{goto _LL320;}_LL320: goto _LL321;_LL315: goto _LL317;_LL317: goto _LL319;
_LL319: return 1;_LL321: return 0;_LL313:;}}void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
tv){void*_tmp14E=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp156;void*
_tmp158;_LL336: if(*((int*)_tmp14E)== Cyc_Absyn_Eq_kb){_LL343: _tmp156=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp14E)->f1;goto _LL337;}else{goto _LL338;}_LL338:
if(*((int*)_tmp14E)== Cyc_Absyn_Less_kb){_LL345: _tmp158=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp14E)->f2;goto _LL339;}else{goto _LL340;}_LL340: goto _LL341;_LL337: return _tmp156;
_LL339: return _tmp158;_LL341: return({void*_tmp15A[0]={};Cyc_Tcutil_impos(_tag_arr("kind not suitably constrained!",
sizeof(unsigned char),31),_tag_arr(_tmp15A,sizeof(void*),0));});_LL335:;}void*
Cyc_Tcutil_typ_kind(void*t){void*_tmp15B=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*
_tmp197;struct Cyc_Core_Opt*_tmp199;struct Cyc_Absyn_Tvar*_tmp19B;void*_tmp19D;
struct Cyc_Absyn_TunionFieldInfo _tmp19F;void*_tmp1A1;struct Cyc_Absyn_Tunionfield*
_tmp1A3;struct Cyc_Absyn_TunionFieldInfo _tmp1A5;void*_tmp1A7;struct Cyc_Absyn_Structdecl**
_tmp1A9;struct Cyc_Absyn_Uniondecl**_tmp1AB;struct Cyc_Absyn_Enumdecl*_tmp1AD;
struct Cyc_Absyn_Enumdecl*_tmp1AF;struct Cyc_Absyn_PtrInfo _tmp1B1;struct Cyc_Core_Opt*
_tmp1B3;_LL349: if((unsigned int)_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_Evar: 0){
_LL410: _tmp199=((struct Cyc_Absyn_Evar_struct*)_tmp15B)->f1;goto _LL408;_LL408:
_tmp197=((struct Cyc_Absyn_Evar_struct*)_tmp15B)->f2;goto _LL350;}else{goto _LL351;}
_LL351: if((unsigned int)_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_VarType: 0){
_LL412: _tmp19B=((struct Cyc_Absyn_VarType_struct*)_tmp15B)->f1;goto _LL352;}else{
goto _LL353;}_LL353: if(_tmp15B == (void*)Cyc_Absyn_VoidType){goto _LL354;}else{goto
_LL355;}_LL355: if((unsigned int)_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_IntType:
0){_LL414: _tmp19D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp15B)->f2;goto
_LL356;}else{goto _LL357;}_LL357: if(_tmp15B == (void*)Cyc_Absyn_FloatType){goto
_LL358;}else{goto _LL359;}_LL359: if((unsigned int)_tmp15B > 3?*((int*)_tmp15B)== 
Cyc_Absyn_DoubleType: 0){goto _LL360;}else{goto _LL361;}_LL361: if((unsigned int)
_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_FnType: 0){goto _LL362;}else{goto _LL363;}
_LL363: if((unsigned int)_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_RgnHandleType: 0){
goto _LL364;}else{goto _LL365;}_LL365: if(_tmp15B == (void*)Cyc_Absyn_HeapRgn){goto
_LL366;}else{goto _LL367;}_LL367: if((unsigned int)_tmp15B > 3?*((int*)_tmp15B)== 
Cyc_Absyn_TunionType: 0){goto _LL368;}else{goto _LL369;}_LL369: if((unsigned int)
_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_TunionFieldType: 0){_LL416: _tmp19F=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp15B)->f1;_LL418: _tmp1A1=(void*)
_tmp19F.field_info;if(*((int*)_tmp1A1)== Cyc_Absyn_KnownTunionfield){_LL420:
_tmp1A3=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp1A1)->f2;goto _LL370;}
else{goto _LL371;}}else{goto _LL371;}_LL371: if((unsigned int)_tmp15B > 3?*((int*)
_tmp15B)== Cyc_Absyn_TunionFieldType: 0){_LL422: _tmp1A5=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp15B)->f1;_LL424: _tmp1A7=(void*)_tmp1A5.field_info;if(*((int*)_tmp1A7)== Cyc_Absyn_UnknownTunionfield){
goto _LL372;}else{goto _LL373;}}else{goto _LL373;}_LL373: if((unsigned int)_tmp15B > 
3?*((int*)_tmp15B)== Cyc_Absyn_StructType: 0){_LL426: _tmp1A9=((struct Cyc_Absyn_StructType_struct*)
_tmp15B)->f3;if(_tmp1A9 == 0){goto _LL374;}else{goto _LL375;}}else{goto _LL375;}
_LL375: if((unsigned int)_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_UnionType: 0){
_LL428: _tmp1AB=((struct Cyc_Absyn_UnionType_struct*)_tmp15B)->f3;if(_tmp1AB == 0){
goto _LL376;}else{goto _LL377;}}else{goto _LL377;}_LL377: if((unsigned int)_tmp15B > 
3?*((int*)_tmp15B)== Cyc_Absyn_EnumType: 0){_LL430: _tmp1AD=((struct Cyc_Absyn_EnumType_struct*)
_tmp15B)->f2;if(_tmp1AD == 0){goto _LL378;}else{goto _LL379;}}else{goto _LL379;}
_LL379: if((unsigned int)_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_StructType: 0){
goto _LL380;}else{goto _LL381;}_LL381: if((unsigned int)_tmp15B > 3?*((int*)_tmp15B)
== Cyc_Absyn_UnionType: 0){goto _LL382;}else{goto _LL383;}_LL383: if((unsigned int)
_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_AnonStructType: 0){goto _LL384;}else{goto
_LL385;}_LL385: if((unsigned int)_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_AnonEnumType:
0){goto _LL386;}else{goto _LL387;}_LL387: if((unsigned int)_tmp15B > 3?*((int*)
_tmp15B)== Cyc_Absyn_AnonUnionType: 0){goto _LL388;}else{goto _LL389;}_LL389: if((
unsigned int)_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_EnumType: 0){_LL432: _tmp1AF=((
struct Cyc_Absyn_EnumType_struct*)_tmp15B)->f2;goto _LL390;}else{goto _LL391;}
_LL391: if((unsigned int)_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_PointerType: 0){
_LL434: _tmp1B1=((struct Cyc_Absyn_PointerType_struct*)_tmp15B)->f1;goto _LL392;}
else{goto _LL393;}_LL393: if((unsigned int)_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_SizeofType:
0){goto _LL394;}else{goto _LL395;}_LL395: if((unsigned int)_tmp15B > 3?*((int*)
_tmp15B)== Cyc_Absyn_ArrayType: 0){goto _LL396;}else{goto _LL397;}_LL397: if((
unsigned int)_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_TupleType: 0){goto _LL398;}
else{goto _LL399;}_LL399: if((unsigned int)_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_TypedefType:
0){_LL436: _tmp1B3=((struct Cyc_Absyn_TypedefType_struct*)_tmp15B)->f3;goto _LL400;}
else{goto _LL401;}_LL401: if((unsigned int)_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_AccessEff:
0){goto _LL402;}else{goto _LL403;}_LL403: if((unsigned int)_tmp15B > 3?*((int*)
_tmp15B)== Cyc_Absyn_JoinEff: 0){goto _LL404;}else{goto _LL405;}_LL405: if((
unsigned int)_tmp15B > 3?*((int*)_tmp15B)== Cyc_Absyn_RgnsEff: 0){goto _LL406;}
else{goto _LL348;}_LL350: return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp199))->v;
_LL352: return Cyc_Tcutil_tvar_kind(_tmp19B);_LL354: return(void*)Cyc_Absyn_MemKind;
_LL356: return _tmp19D == (void*)Cyc_Absyn_B4?(void*)Cyc_Absyn_BoxKind:(void*)Cyc_Absyn_MemKind;
_LL358: goto _LL360;_LL360: goto _LL362;_LL362: return(void*)Cyc_Absyn_MemKind;_LL364:
return(void*)Cyc_Absyn_BoxKind;_LL366: return(void*)Cyc_Absyn_RgnKind;_LL368:
return(void*)Cyc_Absyn_BoxKind;_LL370: if(_tmp1A3->typs == 0){return(void*)Cyc_Absyn_BoxKind;}
else{return(void*)Cyc_Absyn_MemKind;}_LL372: return({void*_tmp1B5[0]={};Cyc_Tcutil_impos(
_tag_arr("typ_kind: Unresolved TunionFieldType",sizeof(unsigned char),37),
_tag_arr(_tmp1B5,sizeof(void*),0));});_LL374: goto _LL376;_LL376: goto _LL378;_LL378:
return(void*)Cyc_Absyn_AnyKind;_LL380: goto _LL382;_LL382: goto _LL384;_LL384: goto
_LL386;_LL386: goto _LL388;_LL388: return(void*)Cyc_Absyn_MemKind;_LL390: if(_tmp1AF->fields
== 0){return(void*)Cyc_Absyn_AnyKind;}else{return(void*)Cyc_Absyn_MemKind;}
_LL392: {void*_tmp1B6=(void*)(Cyc_Absyn_compress_conref(_tmp1B1.bounds))->v;void*
_tmp1C0;void*_tmp1C2;_LL440: if((unsigned int)_tmp1B6 > 1?*((int*)_tmp1B6)== Cyc_Absyn_Eq_constr:
0){_LL449: _tmp1C0=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp1B6)->f1;if(
_tmp1C0 == (void*)Cyc_Absyn_Unknown_b){goto _LL441;}else{goto _LL442;}}else{goto
_LL442;}_LL442: if((unsigned int)_tmp1B6 > 1?*((int*)_tmp1B6)== Cyc_Absyn_Eq_constr:
0){_LL451: _tmp1C2=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp1B6)->f1;if((
unsigned int)_tmp1C2 > 1?*((int*)_tmp1C2)== Cyc_Absyn_Upper_b: 0){goto _LL443;}
else{goto _LL444;}}else{goto _LL444;}_LL444: if(_tmp1B6 == (void*)Cyc_Absyn_No_constr){
goto _LL445;}else{goto _LL446;}_LL446: if((unsigned int)_tmp1B6 > 1?*((int*)_tmp1B6)
== Cyc_Absyn_Forward_constr: 0){goto _LL447;}else{goto _LL439;}_LL441: return(void*)
Cyc_Absyn_MemKind;_LL443: return(void*)Cyc_Absyn_BoxKind;_LL445: return(void*)Cyc_Absyn_MemKind;
_LL447: return({void*_tmp1C4[0]={};Cyc_Tcutil_impos(_tag_arr("typ_kind: forward constraint in ptr bounds",
sizeof(unsigned char),43),_tag_arr(_tmp1C4,sizeof(void*),0));});_LL439:;}_LL394:
return(void*)Cyc_Absyn_BoxKind;_LL396: goto _LL398;_LL398: return(void*)Cyc_Absyn_MemKind;
_LL400: return({struct Cyc_Std_String_pa_struct _tmp1C6;_tmp1C6.tag=Cyc_Std_String_pa;
_tmp1C6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1C5[1]={&
_tmp1C6};Cyc_Tcutil_impos(_tag_arr("typ_kind: typedef found: %s",sizeof(
unsigned char),28),_tag_arr(_tmp1C5,sizeof(void*),1));}});_LL402: goto _LL404;
_LL404: goto _LL406;_LL406: return(void*)Cyc_Absyn_EffKind;_LL348:;}unsigned char
Cyc_Tcutil_Unify[10]="\000\000\000\000Unify";int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp1C7;_push_handler(& _tmp1C7);{int _tmp1C9=0;if(setjmp(
_tmp1C7.handler)){_tmp1C9=1;}if(! _tmp1C9){Cyc_Tcutil_unify_it(t1,t2);{int _tmp1CA=
1;_npop_handler(0);return _tmp1CA;};_pop_handler();}else{void*_tmp1C8=(void*)
_exn_thrown;void*_tmp1CC=_tmp1C8;_LL462: if(_tmp1CC == Cyc_Tcutil_Unify){goto
_LL463;}else{goto _LL464;}_LL464: goto _LL465;_LL463: return 0;_LL465:(void)_throw(
_tmp1CC);_LL461:;}}}static void Cyc_Tcutil_occurslist(void*evar,struct
_RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){void*_tmp1D2=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp1F8;struct Cyc_Core_Opt*_tmp1FA;struct Cyc_Core_Opt**
_tmp1FC;struct Cyc_Core_Opt*_tmp1FD;struct Cyc_Absyn_PtrInfo _tmp1FF;void*_tmp201;
struct Cyc_Absyn_FnInfo _tmp203;struct Cyc_List_List*_tmp205;struct Cyc_List_List*
_tmp207;struct Cyc_Absyn_VarargInfo*_tmp209;int _tmp20B;struct Cyc_List_List*
_tmp20D;void*_tmp20F;struct Cyc_Core_Opt*_tmp211;struct Cyc_List_List*_tmp213;
struct Cyc_List_List*_tmp215;void*_tmp217;struct Cyc_Absyn_TunionInfo _tmp219;void*
_tmp21B;struct Cyc_List_List*_tmp21D;struct Cyc_Core_Opt*_tmp21F;struct Cyc_List_List*
_tmp221;struct Cyc_Absyn_TunionFieldInfo _tmp223;struct Cyc_List_List*_tmp225;
struct Cyc_List_List*_tmp227;struct Cyc_List_List*_tmp229;struct Cyc_List_List*
_tmp22B;void*_tmp22D;void*_tmp22F;struct Cyc_List_List*_tmp231;void*_tmp233;
_LL468: if((unsigned int)_tmp1D2 > 3?*((int*)_tmp1D2)== Cyc_Absyn_VarType: 0){
_LL505: _tmp1F8=((struct Cyc_Absyn_VarType_struct*)_tmp1D2)->f1;goto _LL469;}else{
goto _LL470;}_LL470: if((unsigned int)_tmp1D2 > 3?*((int*)_tmp1D2)== Cyc_Absyn_Evar:
0){_LL510: _tmp1FD=((struct Cyc_Absyn_Evar_struct*)_tmp1D2)->f2;goto _LL507;_LL507:
_tmp1FA=((struct Cyc_Absyn_Evar_struct*)_tmp1D2)->f4;_tmp1FC=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1D2)->f4;goto _LL471;}else{goto _LL472;}_LL472:
if((unsigned int)_tmp1D2 > 3?*((int*)_tmp1D2)== Cyc_Absyn_PointerType: 0){_LL512:
_tmp1FF=((struct Cyc_Absyn_PointerType_struct*)_tmp1D2)->f1;goto _LL473;}else{goto
_LL474;}_LL474: if((unsigned int)_tmp1D2 > 3?*((int*)_tmp1D2)== Cyc_Absyn_ArrayType:
0){_LL514: _tmp201=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp1D2)->f1;goto
_LL475;}else{goto _LL476;}_LL476: if((unsigned int)_tmp1D2 > 3?*((int*)_tmp1D2)== 
Cyc_Absyn_FnType: 0){_LL516: _tmp203=((struct Cyc_Absyn_FnType_struct*)_tmp1D2)->f1;
_LL532: _tmp213=_tmp203.tvars;goto _LL530;_LL530: _tmp211=_tmp203.effect;goto _LL528;
_LL528: _tmp20F=(void*)_tmp203.ret_typ;goto _LL526;_LL526: _tmp20D=_tmp203.args;
goto _LL524;_LL524: _tmp20B=_tmp203.c_varargs;goto _LL522;_LL522: _tmp209=_tmp203.cyc_varargs;
goto _LL520;_LL520: _tmp207=_tmp203.rgn_po;goto _LL518;_LL518: _tmp205=_tmp203.attributes;
goto _LL477;}else{goto _LL478;}_LL478: if((unsigned int)_tmp1D2 > 3?*((int*)_tmp1D2)
== Cyc_Absyn_TupleType: 0){_LL534: _tmp215=((struct Cyc_Absyn_TupleType_struct*)
_tmp1D2)->f1;goto _LL479;}else{goto _LL480;}_LL480: if((unsigned int)_tmp1D2 > 3?*((
int*)_tmp1D2)== Cyc_Absyn_RgnHandleType: 0){_LL536: _tmp217=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp1D2)->f1;goto _LL481;}else{goto _LL482;}_LL482: if((unsigned int)_tmp1D2 > 3?*((
int*)_tmp1D2)== Cyc_Absyn_TunionType: 0){_LL538: _tmp219=((struct Cyc_Absyn_TunionType_struct*)
_tmp1D2)->f1;_LL542: _tmp21D=_tmp219.targs;goto _LL540;_LL540: _tmp21B=(void*)
_tmp219.rgn;goto _LL483;}else{goto _LL484;}_LL484: if((unsigned int)_tmp1D2 > 3?*((
int*)_tmp1D2)== Cyc_Absyn_TypedefType: 0){_LL546: _tmp221=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1D2)->f2;goto _LL544;_LL544: _tmp21F=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1D2)->f3;goto _LL485;}else{goto _LL486;}_LL486: if((unsigned int)_tmp1D2 > 3?*((
int*)_tmp1D2)== Cyc_Absyn_TunionFieldType: 0){_LL548: _tmp223=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1D2)->f1;_LL550: _tmp225=_tmp223.targs;goto _LL487;}else{goto _LL488;}_LL488:
if((unsigned int)_tmp1D2 > 3?*((int*)_tmp1D2)== Cyc_Absyn_StructType: 0){_LL552:
_tmp227=((struct Cyc_Absyn_StructType_struct*)_tmp1D2)->f2;goto _LL489;}else{goto
_LL490;}_LL490: if((unsigned int)_tmp1D2 > 3?*((int*)_tmp1D2)== Cyc_Absyn_AnonStructType:
0){_LL554: _tmp229=((struct Cyc_Absyn_AnonStructType_struct*)_tmp1D2)->f1;goto
_LL491;}else{goto _LL492;}_LL492: if((unsigned int)_tmp1D2 > 3?*((int*)_tmp1D2)== 
Cyc_Absyn_AnonUnionType: 0){_LL556: _tmp22B=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp1D2)->f1;goto _LL493;}else{goto _LL494;}_LL494: if((unsigned int)_tmp1D2 > 3?*((
int*)_tmp1D2)== Cyc_Absyn_SizeofType: 0){_LL558: _tmp22D=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp1D2)->f1;goto _LL495;}else{goto _LL496;}_LL496: if((unsigned int)_tmp1D2 > 3?*((
int*)_tmp1D2)== Cyc_Absyn_AccessEff: 0){_LL560: _tmp22F=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1D2)->f1;goto _LL497;}else{goto _LL498;}_LL498: if((unsigned int)_tmp1D2 > 3?*((
int*)_tmp1D2)== Cyc_Absyn_JoinEff: 0){_LL562: _tmp231=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1D2)->f1;goto _LL499;}else{goto _LL500;}_LL500: if((unsigned int)_tmp1D2 > 3?*((
int*)_tmp1D2)== Cyc_Absyn_RgnsEff: 0){_LL564: _tmp233=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1D2)->f1;goto _LL501;}else{goto _LL502;}_LL502: goto _LL503;_LL469: if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1F8)){Cyc_Tcutil_failure_reason=
_tag_arr("(type variable would escape scope)",sizeof(unsigned char),35);(int)
_throw((void*)Cyc_Tcutil_Unify);}goto _LL467;_LL471: if(t == evar){Cyc_Tcutil_failure_reason=
_tag_arr("(occurs check)",sizeof(unsigned char),15);(int)_throw((void*)Cyc_Tcutil_Unify);}
else{if(_tmp1FD != 0){Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp1FD->v);}else{int
problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp1FC))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp235=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1FC))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){_tmp235=({struct Cyc_List_List*_tmp236=
_cycalloc(sizeof(struct Cyc_List_List));_tmp236->hd=(struct Cyc_Absyn_Tvar*)s->hd;
_tmp236->tl=_tmp235;_tmp236;});}}}*_tmp1FC=({struct Cyc_Core_Opt*_tmp237=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp237->v=_tmp235;_tmp237;});}}}goto
_LL467;_LL473: Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp1FF.elt_typ);Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp1FF.rgn_typ);goto _LL467;_LL475: Cyc_Tcutil_occurs(evar,r,
env,_tmp201);goto _LL467;_LL477: env=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp213,env);
if(_tmp211 != 0){Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp211->v);}Cyc_Tcutil_occurs(
evar,r,env,_tmp20F);for(0;_tmp20D != 0;_tmp20D=_tmp20D->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple2*)_tmp20D->hd)).f3);}if(_tmp209 != 0){void*_tmp23A;
struct Cyc_Absyn_VarargInfo _tmp238=*_tmp209;_LL571: _tmp23A=(void*)_tmp238.type;
goto _LL569;_LL569: Cyc_Tcutil_occurs(evar,r,env,_tmp23A);}for(0;_tmp207 != 0;
_tmp207=_tmp207->tl){struct _tuple8 _tmp23E;void*_tmp23F;void*_tmp241;struct
_tuple8*_tmp23C=(struct _tuple8*)_tmp207->hd;_tmp23E=*_tmp23C;_LL578: _tmp241=
_tmp23E.f1;goto _LL576;_LL576: _tmp23F=_tmp23E.f2;goto _LL573;_LL573: Cyc_Tcutil_occurs(
evar,r,env,_tmp241);Cyc_Tcutil_occurs(evar,r,env,_tmp23F);}goto _LL467;_LL479:
for(0;_tmp215 != 0;_tmp215=_tmp215->tl){Cyc_Tcutil_occurs(evar,r,env,(*((struct
_tuple4*)_tmp215->hd)).f2);}goto _LL467;_LL481: Cyc_Tcutil_occurs(evar,r,env,
_tmp217);goto _LL467;_LL483: Cyc_Tcutil_occurs(evar,r,env,_tmp21B);Cyc_Tcutil_occurslist(
evar,r,env,_tmp21D);goto _LL467;_LL485: _tmp225=_tmp221;goto _LL487;_LL487: _tmp227=
_tmp225;goto _LL489;_LL489: Cyc_Tcutil_occurslist(evar,r,env,_tmp227);goto _LL467;
_LL491: _tmp22B=_tmp229;goto _LL493;_LL493: for(0;_tmp22B != 0;_tmp22B=_tmp22B->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)((struct Cyc_Absyn_Structfield*)_tmp22B->hd)->type);}
goto _LL467;_LL495: Cyc_Tcutil_occurs(evar,r,env,_tmp22D);goto _LL467;_LL497: Cyc_Tcutil_occurs(
evar,r,env,_tmp22F);goto _LL467;_LL499: Cyc_Tcutil_occurslist(evar,r,env,_tmp231);
goto _LL467;_LL501: Cyc_Tcutil_occurs(evar,r,env,_tmp233);goto _LL467;_LL503: goto
_LL467;_LL467:;}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,
struct Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(
evar,r,env,(void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*
t1,struct Cyc_List_List*t2){for(0;t1 != 0? t2 != 0: 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((
void*)t1->hd,(void*)t2->hd);}if(t1 != 0? 1: t2 != 0){(int)_throw((void*)Cyc_Tcutil_Unify);}}
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2){if((tq1.q_const != tq2.q_const? 1: tq1.q_volatile != tq2.q_volatile)? 1: tq1.q_restrict
!= tq2.q_restrict){Cyc_Tcutil_failure_reason=_tag_arr("(qualifiers don't match)",
sizeof(unsigned char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}}int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.q_const == tq2.q_const?
tq1.q_volatile == tq2.q_volatile: 0)? tq1.q_restrict == tq2.q_restrict: 0;}static void
Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y,struct _tagged_arr reason){x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);if(x == y){return;}{void*_tmp243=(void*)x->v;void*
_tmp24B;_LL581: if(_tmp243 == (void*)Cyc_Absyn_No_constr){goto _LL582;}else{goto
_LL583;}_LL583: if((unsigned int)_tmp243 > 1?*((int*)_tmp243)== Cyc_Absyn_Forward_constr:
0){goto _LL584;}else{goto _LL585;}_LL585: if((unsigned int)_tmp243 > 1?*((int*)
_tmp243)== Cyc_Absyn_Eq_constr: 0){_LL588: _tmp24B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp243)->f1;goto _LL586;}else{goto _LL580;}_LL582:(void*)(x->v=(void*)((void*)({
struct Cyc_Absyn_Forward_constr_struct*_tmp24D=_cycalloc(sizeof(struct Cyc_Absyn_Forward_constr_struct));
_tmp24D[0]=({struct Cyc_Absyn_Forward_constr_struct _tmp24E;_tmp24E.tag=Cyc_Absyn_Forward_constr;
_tmp24E.f1=y;_tmp24E;});_tmp24D;})));return;_LL584:(int)_throw(({void*_tmp24F[0]={};
Cyc_Tcutil_impos(_tag_arr("unify_conref: forward after compress",sizeof(
unsigned char),37),_tag_arr(_tmp24F,sizeof(void*),0));}));_LL586: {void*_tmp250=(
void*)y->v;void*_tmp258;_LL594: if(_tmp250 == (void*)Cyc_Absyn_No_constr){goto
_LL595;}else{goto _LL596;}_LL596: if((unsigned int)_tmp250 > 1?*((int*)_tmp250)== 
Cyc_Absyn_Forward_constr: 0){goto _LL597;}else{goto _LL598;}_LL598: if((unsigned int)
_tmp250 > 1?*((int*)_tmp250)== Cyc_Absyn_Eq_constr: 0){_LL601: _tmp258=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp250)->f1;goto _LL599;}else{goto _LL593;}
_LL595:(void*)(y->v=(void*)((void*)x->v));return;_LL597:(int)_throw(({void*
_tmp25A[0]={};Cyc_Tcutil_impos(_tag_arr("unify_conref: forward after compress(2)",
sizeof(unsigned char),40),_tag_arr(_tmp25A,sizeof(void*),0));}));_LL599: if(cmp(
_tmp24B,_tmp258)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)Cyc_Tcutil_Unify);}
return;_LL593:;}_LL580:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct _handler_cons _tmp25B;
_push_handler(& _tmp25B);{int _tmp25D=0;if(setjmp(_tmp25B.handler)){_tmp25D=1;}if(
! _tmp25D){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _tagged_arr)_tag_arr(0,0,0));{
int _tmp25E=1;_npop_handler(0);return _tmp25E;};_pop_handler();}else{void*_tmp25C=(
void*)_exn_thrown;void*_tmp260=_tmp25C;_LL610: if(_tmp260 == Cyc_Tcutil_Unify){
goto _LL611;}else{goto _LL612;}_LL612: goto _LL613;_LL611: return 0;_LL613:(void)
_throw(_tmp260);_LL609:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){struct
_tuple8 _tmp267=({struct _tuple8 _tmp266;_tmp266.f1=b1;_tmp266.f2=b2;_tmp266;});
void*_tmp271;void*_tmp273;void*_tmp275;void*_tmp277;void*_tmp279;struct Cyc_Absyn_Exp*
_tmp27B;void*_tmp27D;struct Cyc_Absyn_Exp*_tmp27F;_LL617: _LL628: _tmp273=_tmp267.f1;
if(_tmp273 == (void*)Cyc_Absyn_Unknown_b){goto _LL626;}else{goto _LL619;}_LL626:
_tmp271=_tmp267.f2;if(_tmp271 == (void*)Cyc_Absyn_Unknown_b){goto _LL618;}else{
goto _LL619;}_LL619: _LL630: _tmp275=_tmp267.f1;if(_tmp275 == (void*)Cyc_Absyn_Unknown_b){
goto _LL620;}else{goto _LL621;}_LL621: _LL632: _tmp277=_tmp267.f2;if(_tmp277 == (void*)
Cyc_Absyn_Unknown_b){goto _LL622;}else{goto _LL623;}_LL623: _LL638: _tmp27D=_tmp267.f1;
if((unsigned int)_tmp27D > 1?*((int*)_tmp27D)== Cyc_Absyn_Upper_b: 0){_LL640:
_tmp27F=((struct Cyc_Absyn_Upper_b_struct*)_tmp27D)->f1;goto _LL634;}else{goto
_LL616;}_LL634: _tmp279=_tmp267.f2;if((unsigned int)_tmp279 > 1?*((int*)_tmp279)== 
Cyc_Absyn_Upper_b: 0){_LL636: _tmp27B=((struct Cyc_Absyn_Upper_b_struct*)_tmp279)->f1;
goto _LL624;}else{goto _LL616;}_LL618: return 0;_LL620: return - 1;_LL622: return 1;
_LL624: {int i1=(int)Cyc_Evexp_eval_const_uint_exp(_tmp27F);int i2=(int)Cyc_Evexp_eval_const_uint_exp(
_tmp27B);if(i1 == i2){return 0;}if(i1 < i2){return - 1;}return 1;}_LL616:;}static int
Cyc_Tcutil_equal_att(void*a1,void*a2){if(a1 == a2){return 1;}{struct _tuple8 _tmp282=({
struct _tuple8 _tmp281;_tmp281.f1=a1;_tmp281.f2=a2;_tmp281;});void*_tmp28E;int
_tmp290;int _tmp292;void*_tmp294;void*_tmp296;int _tmp298;int _tmp29A;void*_tmp29C;
void*_tmp29E;int _tmp2A0;void*_tmp2A2;int _tmp2A4;void*_tmp2A6;int _tmp2A8;void*
_tmp2AA;int _tmp2AC;void*_tmp2AE;struct _tagged_arr _tmp2B0;void*_tmp2B2;struct
_tagged_arr _tmp2B4;_LL644: _LL663: _tmp296=_tmp282.f1;if((unsigned int)_tmp296 > 16?*((
int*)_tmp296)== Cyc_Absyn_Format_att: 0){_LL669: _tmp29C=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp296)->f1;goto _LL667;_LL667: _tmp29A=((struct Cyc_Absyn_Format_att_struct*)
_tmp296)->f2;goto _LL665;_LL665: _tmp298=((struct Cyc_Absyn_Format_att_struct*)
_tmp296)->f3;goto _LL655;}else{goto _LL646;}_LL655: _tmp28E=_tmp282.f2;if((
unsigned int)_tmp28E > 16?*((int*)_tmp28E)== Cyc_Absyn_Format_att: 0){_LL661:
_tmp294=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp28E)->f1;goto _LL659;
_LL659: _tmp292=((struct Cyc_Absyn_Format_att_struct*)_tmp28E)->f2;goto _LL657;
_LL657: _tmp290=((struct Cyc_Absyn_Format_att_struct*)_tmp28E)->f3;goto _LL645;}
else{goto _LL646;}_LL646: _LL675: _tmp2A2=_tmp282.f1;if((unsigned int)_tmp2A2 > 16?*((
int*)_tmp2A2)== Cyc_Absyn_Regparm_att: 0){_LL677: _tmp2A4=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp2A2)->f1;goto _LL671;}else{goto _LL648;}_LL671: _tmp29E=_tmp282.f2;if((
unsigned int)_tmp29E > 16?*((int*)_tmp29E)== Cyc_Absyn_Regparm_att: 0){_LL673:
_tmp2A0=((struct Cyc_Absyn_Regparm_att_struct*)_tmp29E)->f1;goto _LL647;}else{goto
_LL648;}_LL648: _LL683: _tmp2AA=_tmp282.f1;if((unsigned int)_tmp2AA > 16?*((int*)
_tmp2AA)== Cyc_Absyn_Aligned_att: 0){_LL685: _tmp2AC=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp2AA)->f1;goto _LL679;}else{goto _LL650;}_LL679: _tmp2A6=_tmp282.f2;if((
unsigned int)_tmp2A6 > 16?*((int*)_tmp2A6)== Cyc_Absyn_Aligned_att: 0){_LL681:
_tmp2A8=((struct Cyc_Absyn_Aligned_att_struct*)_tmp2A6)->f1;goto _LL649;}else{goto
_LL650;}_LL650: _LL691: _tmp2B2=_tmp282.f1;if((unsigned int)_tmp2B2 > 16?*((int*)
_tmp2B2)== Cyc_Absyn_Section_att: 0){_LL693: _tmp2B4=((struct Cyc_Absyn_Section_att_struct*)
_tmp2B2)->f1;goto _LL687;}else{goto _LL652;}_LL687: _tmp2AE=_tmp282.f2;if((
unsigned int)_tmp2AE > 16?*((int*)_tmp2AE)== Cyc_Absyn_Section_att: 0){_LL689:
_tmp2B0=((struct Cyc_Absyn_Section_att_struct*)_tmp2AE)->f1;goto _LL651;}else{goto
_LL652;}_LL652: goto _LL653;_LL645: return(_tmp29C == _tmp294? _tmp29A == _tmp292: 0)?
_tmp298 == _tmp290: 0;_LL647: _tmp2AC=_tmp2A4;_tmp2A8=_tmp2A0;goto _LL649;_LL649:
return _tmp2AC == _tmp2A8;_LL651: return Cyc_Std_strcmp(_tmp2B4,_tmp2B0)== 0;_LL653:
return 0;_LL643:;}}int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(! Cyc_List_exists_c(Cyc_Tcutil_equal_att,(
void*)a->hd,a2)){return 0;}}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(!
Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a1)){return 0;}}}return 1;}
static void*Cyc_Tcutil_normalize_effect(void*e);static void*Cyc_Tcutil_rgns_of(
void*t);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Structfield*sf){
return Cyc_Tcutil_rgns_of((void*)sf->type);}static struct _tuple5*Cyc_Tcutil_region_free_subst(
struct Cyc_Absyn_Tvar*tv){void*t;{void*_tmp2B6=Cyc_Tcutil_tvar_kind(tv);_LL696:
if(_tmp2B6 == (void*)Cyc_Absyn_RgnKind){goto _LL697;}else{goto _LL698;}_LL698: if(
_tmp2B6 == (void*)Cyc_Absyn_EffKind){goto _LL699;}else{goto _LL700;}_LL700: goto
_LL701;_LL697: t=(void*)Cyc_Absyn_HeapRgn;goto _LL695;_LL699: t=Cyc_Absyn_empty_effect;
goto _LL695;_LL701: t=Cyc_Absyn_sint_t;goto _LL695;_LL695:;}return({struct _tuple5*
_tmp2BE=_cycalloc(sizeof(struct _tuple5));_tmp2BE->f1=tv;_tmp2BE->f2=t;_tmp2BE;});}
static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp2BF=Cyc_Tcutil_compress(t);void*
_tmp2F3;struct Cyc_Absyn_TunionInfo _tmp2F5;void*_tmp2F7;struct Cyc_List_List*
_tmp2F9;struct Cyc_Absyn_PtrInfo _tmp2FB;void*_tmp2FD;void*_tmp2FF;void*_tmp301;
struct Cyc_List_List*_tmp303;struct Cyc_Absyn_TunionFieldInfo _tmp305;struct Cyc_List_List*
_tmp307;struct Cyc_List_List*_tmp309;struct Cyc_List_List*_tmp30B;struct Cyc_List_List*
_tmp30D;struct Cyc_List_List*_tmp30F;void*_tmp311;struct Cyc_Absyn_FnInfo _tmp313;
struct Cyc_List_List*_tmp315;struct Cyc_Absyn_VarargInfo*_tmp317;struct Cyc_List_List*
_tmp319;void*_tmp31B;struct Cyc_Core_Opt*_tmp31D;struct Cyc_List_List*_tmp31F;void*
_tmp321;_LL705: if(_tmp2BF == (void*)Cyc_Absyn_VoidType){goto _LL706;}else{goto
_LL707;}_LL707: if(_tmp2BF == (void*)Cyc_Absyn_FloatType){goto _LL708;}else{goto
_LL709;}_LL709: if((unsigned int)_tmp2BF > 3?*((int*)_tmp2BF)== Cyc_Absyn_DoubleType:
0){goto _LL710;}else{goto _LL711;}_LL711: if((unsigned int)_tmp2BF > 3?*((int*)
_tmp2BF)== Cyc_Absyn_EnumType: 0){goto _LL712;}else{goto _LL713;}_LL713: if((
unsigned int)_tmp2BF > 3?*((int*)_tmp2BF)== Cyc_Absyn_AnonEnumType: 0){goto _LL714;}
else{goto _LL715;}_LL715: if((unsigned int)_tmp2BF > 3?*((int*)_tmp2BF)== Cyc_Absyn_IntType:
0){goto _LL716;}else{goto _LL717;}_LL717: if((unsigned int)_tmp2BF > 3?*((int*)
_tmp2BF)== Cyc_Absyn_Evar: 0){goto _LL718;}else{goto _LL719;}_LL719: if((
unsigned int)_tmp2BF > 3?*((int*)_tmp2BF)== Cyc_Absyn_VarType: 0){goto _LL720;}
else{goto _LL721;}_LL721: if((unsigned int)_tmp2BF > 3?*((int*)_tmp2BF)== Cyc_Absyn_RgnHandleType:
0){_LL756: _tmp2F3=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp2BF)->f1;
goto _LL722;}else{goto _LL723;}_LL723: if((unsigned int)_tmp2BF > 3?*((int*)_tmp2BF)
== Cyc_Absyn_TunionType: 0){_LL758: _tmp2F5=((struct Cyc_Absyn_TunionType_struct*)
_tmp2BF)->f1;_LL762: _tmp2F9=_tmp2F5.targs;goto _LL760;_LL760: _tmp2F7=(void*)
_tmp2F5.rgn;goto _LL724;}else{goto _LL725;}_LL725: if((unsigned int)_tmp2BF > 3?*((
int*)_tmp2BF)== Cyc_Absyn_PointerType: 0){_LL764: _tmp2FB=((struct Cyc_Absyn_PointerType_struct*)
_tmp2BF)->f1;_LL768: _tmp2FF=(void*)_tmp2FB.elt_typ;goto _LL766;_LL766: _tmp2FD=(
void*)_tmp2FB.rgn_typ;goto _LL726;}else{goto _LL727;}_LL727: if((unsigned int)
_tmp2BF > 3?*((int*)_tmp2BF)== Cyc_Absyn_ArrayType: 0){_LL770: _tmp301=(void*)((
struct Cyc_Absyn_ArrayType_struct*)_tmp2BF)->f1;goto _LL728;}else{goto _LL729;}
_LL729: if((unsigned int)_tmp2BF > 3?*((int*)_tmp2BF)== Cyc_Absyn_TupleType: 0){
_LL772: _tmp303=((struct Cyc_Absyn_TupleType_struct*)_tmp2BF)->f1;goto _LL730;}
else{goto _LL731;}_LL731: if((unsigned int)_tmp2BF > 3?*((int*)_tmp2BF)== Cyc_Absyn_TunionFieldType:
0){_LL774: _tmp305=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2BF)->f1;_LL776:
_tmp307=_tmp305.targs;goto _LL732;}else{goto _LL733;}_LL733: if((unsigned int)
_tmp2BF > 3?*((int*)_tmp2BF)== Cyc_Absyn_StructType: 0){_LL778: _tmp309=((struct Cyc_Absyn_StructType_struct*)
_tmp2BF)->f2;goto _LL734;}else{goto _LL735;}_LL735: if((unsigned int)_tmp2BF > 3?*((
int*)_tmp2BF)== Cyc_Absyn_UnionType: 0){_LL780: _tmp30B=((struct Cyc_Absyn_UnionType_struct*)
_tmp2BF)->f2;goto _LL736;}else{goto _LL737;}_LL737: if((unsigned int)_tmp2BF > 3?*((
int*)_tmp2BF)== Cyc_Absyn_AnonStructType: 0){_LL782: _tmp30D=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp2BF)->f1;goto _LL738;}else{goto _LL739;}_LL739: if((unsigned int)_tmp2BF > 3?*((
int*)_tmp2BF)== Cyc_Absyn_AnonUnionType: 0){_LL784: _tmp30F=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp2BF)->f1;goto _LL740;}else{goto _LL741;}_LL741: if((unsigned int)_tmp2BF > 3?*((
int*)_tmp2BF)== Cyc_Absyn_SizeofType: 0){_LL786: _tmp311=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp2BF)->f1;goto _LL742;}else{goto _LL743;}_LL743: if((unsigned int)_tmp2BF > 3?*((
int*)_tmp2BF)== Cyc_Absyn_FnType: 0){_LL788: _tmp313=((struct Cyc_Absyn_FnType_struct*)
_tmp2BF)->f1;_LL800: _tmp31F=_tmp313.tvars;goto _LL798;_LL798: _tmp31D=_tmp313.effect;
goto _LL796;_LL796: _tmp31B=(void*)_tmp313.ret_typ;goto _LL794;_LL794: _tmp319=
_tmp313.args;goto _LL792;_LL792: _tmp317=_tmp313.cyc_varargs;goto _LL790;_LL790:
_tmp315=_tmp313.rgn_po;goto _LL744;}else{goto _LL745;}_LL745: if(_tmp2BF == (void*)
Cyc_Absyn_HeapRgn){goto _LL746;}else{goto _LL747;}_LL747: if((unsigned int)_tmp2BF > 
3?*((int*)_tmp2BF)== Cyc_Absyn_AccessEff: 0){goto _LL748;}else{goto _LL749;}_LL749:
if((unsigned int)_tmp2BF > 3?*((int*)_tmp2BF)== Cyc_Absyn_JoinEff: 0){goto _LL750;}
else{goto _LL751;}_LL751: if((unsigned int)_tmp2BF > 3?*((int*)_tmp2BF)== Cyc_Absyn_RgnsEff:
0){_LL802: _tmp321=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp2BF)->f1;goto
_LL752;}else{goto _LL753;}_LL753: if((unsigned int)_tmp2BF > 3?*((int*)_tmp2BF)== 
Cyc_Absyn_TypedefType: 0){goto _LL754;}else{goto _LL704;}_LL706: goto _LL708;_LL708:
goto _LL710;_LL710: goto _LL712;_LL712: goto _LL714;_LL714: goto _LL716;_LL716: return
Cyc_Absyn_empty_effect;_LL718: goto _LL720;_LL720: {void*_tmp323=Cyc_Tcutil_typ_kind(
t);_LL805: if(_tmp323 == (void*)Cyc_Absyn_RgnKind){goto _LL806;}else{goto _LL807;}
_LL807: if(_tmp323 == (void*)Cyc_Absyn_EffKind){goto _LL808;}else{goto _LL809;}
_LL809: goto _LL810;_LL806: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp32B=
_cycalloc(sizeof(struct Cyc_Absyn_AccessEff_struct));_tmp32B[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp32C;_tmp32C.tag=Cyc_Absyn_AccessEff;_tmp32C.f1=(void*)t;_tmp32C;});_tmp32B;});
_LL808: return t;_LL810: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp32D=
_cycalloc(sizeof(struct Cyc_Absyn_RgnsEff_struct));_tmp32D[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp32E;_tmp32E.tag=Cyc_Absyn_RgnsEff;_tmp32E.f1=(void*)t;_tmp32E;});_tmp32D;});
_LL804:;}_LL722: return(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp32F=
_cycalloc(sizeof(struct Cyc_Absyn_AccessEff_struct));_tmp32F[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp330;_tmp330.tag=Cyc_Absyn_AccessEff;_tmp330.f1=(void*)_tmp2F3;_tmp330;});
_tmp32F;});_LL724: {struct Cyc_List_List*ts=({struct Cyc_List_List*_tmp333=
_cycalloc(sizeof(struct Cyc_List_List));_tmp333->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp334=_cycalloc(sizeof(struct Cyc_Absyn_AccessEff_struct));_tmp334[0]=({struct
Cyc_Absyn_AccessEff_struct _tmp335;_tmp335.tag=Cyc_Absyn_AccessEff;_tmp335.f1=(
void*)_tmp2F7;_tmp335;});_tmp334;}));_tmp333->tl=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp2F9);_tmp333;});return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp331=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp331[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp332;_tmp332.tag=Cyc_Absyn_JoinEff;_tmp332.f1=ts;_tmp332;});_tmp331;}));}
_LL726: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp336=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp336[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp337;_tmp337.tag=Cyc_Absyn_JoinEff;_tmp337.f1=({void*_tmp338[2];_tmp338[1]=
Cyc_Tcutil_rgns_of(_tmp2FF);_tmp338[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp339=_cycalloc(sizeof(struct Cyc_Absyn_AccessEff_struct));_tmp339[0]=({struct
Cyc_Absyn_AccessEff_struct _tmp33A;_tmp33A.tag=Cyc_Absyn_AccessEff;_tmp33A.f1=(
void*)_tmp2FD;_tmp33A;});_tmp339;});Cyc_List_list(_tag_arr(_tmp338,sizeof(void*),
2));});_tmp337;});_tmp336;}));_LL728: return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(
_tmp301));_LL730: {struct Cyc_List_List*_tmp33B=0;for(0;_tmp303 != 0;_tmp303=
_tmp303->tl){_tmp33B=({struct Cyc_List_List*_tmp33C=_cycalloc(sizeof(struct Cyc_List_List));
_tmp33C->hd=(void*)(*((struct _tuple4*)_tmp303->hd)).f2;_tmp33C->tl=_tmp33B;
_tmp33C;});}_tmp307=_tmp33B;goto _LL732;}_LL732: _tmp309=_tmp307;goto _LL734;_LL734:
_tmp30B=_tmp309;goto _LL736;_LL736: return Cyc_Tcutil_normalize_effect((void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp33D=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));
_tmp33D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp33E;_tmp33E.tag=Cyc_Absyn_JoinEff;
_tmp33E.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp30B);_tmp33E;});_tmp33D;}));
_LL738: _tmp30F=_tmp30D;goto _LL740;_LL740: return Cyc_Tcutil_normalize_effect((void*)({
struct Cyc_Absyn_JoinEff_struct*_tmp33F=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));
_tmp33F[0]=({struct Cyc_Absyn_JoinEff_struct _tmp340;_tmp340.tag=Cyc_Absyn_JoinEff;
_tmp340.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Structfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp30F);_tmp340;});
_tmp33F;}));_LL742: return Cyc_Tcutil_rgns_of(_tmp311);_LL744: {void*_tmp341=Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp31F),(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp31D))->v);return Cyc_Tcutil_normalize_effect(_tmp341);}_LL746:
return Cyc_Absyn_empty_effect;_LL748: goto _LL750;_LL750: return t;_LL752: return Cyc_Tcutil_rgns_of(
_tmp321);_LL754: return({void*_tmp342[0]={};Cyc_Tcutil_impos(_tag_arr("typedef in compressed type",
sizeof(unsigned char),27),_tag_arr(_tmp342,sizeof(void*),0));});_LL704:;}static
void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{void*_tmp343=
e;struct Cyc_List_List*_tmp34B;struct Cyc_List_List**_tmp34D;void*_tmp34E;_LL837:
if((unsigned int)_tmp343 > 3?*((int*)_tmp343)== Cyc_Absyn_JoinEff: 0){_LL844:
_tmp34B=((struct Cyc_Absyn_JoinEff_struct*)_tmp343)->f1;_tmp34D=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp343)->f1;goto _LL838;}else{goto _LL839;}_LL839:
if((unsigned int)_tmp343 > 3?*((int*)_tmp343)== Cyc_Absyn_RgnsEff: 0){_LL847:
_tmp34E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp343)->f1;goto _LL840;}else{
goto _LL841;}_LL841: goto _LL842;_LL838: {int nested_join=1;{struct Cyc_List_List*
effs=*_tmp34D;for(0;effs != 0;effs=effs->tl){void*_tmp350=(void*)effs->hd;(void*)(
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp350)));{void*
_tmp351=(void*)effs->hd;_LL851: if((unsigned int)_tmp351 > 3?*((int*)_tmp351)== 
Cyc_Absyn_JoinEff: 0){goto _LL852;}else{goto _LL853;}_LL853: goto _LL854;_LL852:
nested_join=1;goto _LL850;_LL854: goto _LL850;_LL850:;}}}if(! nested_join){return e;}{
struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*_tmp34D;for(0;effs != 0;
effs=effs->tl){void*_tmp357=Cyc_Tcutil_compress((void*)effs->hd);struct Cyc_List_List*
_tmp35F;void*_tmp361;_LL857: if((unsigned int)_tmp357 > 3?*((int*)_tmp357)== Cyc_Absyn_JoinEff:
0){_LL864: _tmp35F=((struct Cyc_Absyn_JoinEff_struct*)_tmp357)->f1;goto _LL858;}
else{goto _LL859;}_LL859: if((unsigned int)_tmp357 > 3?*((int*)_tmp357)== Cyc_Absyn_AccessEff:
0){_LL866: _tmp361=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp357)->f1;if(
_tmp361 == (void*)Cyc_Absyn_HeapRgn){goto _LL860;}else{goto _LL861;}}else{goto
_LL861;}_LL861: goto _LL862;_LL858: effects=Cyc_List_revappend(_tmp35F,effects);
goto _LL856;_LL860: goto _LL856;_LL862: effects=({struct Cyc_List_List*_tmp363=
_cycalloc(sizeof(struct Cyc_List_List));_tmp363->hd=(void*)_tmp357;_tmp363->tl=
effects;_tmp363;});goto _LL856;_LL856:;}}*_tmp34D=Cyc_List_imp_rev(effects);
return e;}}_LL840: return Cyc_Tcutil_rgns_of(_tmp34E);_LL842: return e;_LL836:;}}
struct _tuple9{void*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};static
struct _tuple9*Cyc_Tcutil_get_effect_evar(void*t){void*_tmp364=Cyc_Tcutil_compress(
t);struct Cyc_List_List*_tmp36C;struct Cyc_List_List _tmp36E;struct Cyc_List_List*
_tmp36F;void*_tmp371;struct Cyc_Core_Opt*_tmp373;struct Cyc_Core_Opt*_tmp375;
_LL870: if((unsigned int)_tmp364 > 3?*((int*)_tmp364)== Cyc_Absyn_JoinEff: 0){
_LL877: _tmp36C=((struct Cyc_Absyn_JoinEff_struct*)_tmp364)->f1;if(_tmp36C == 0){
goto _LL872;}else{_tmp36E=*_tmp36C;_LL882: _tmp371=(void*)_tmp36E.hd;goto _LL880;
_LL880: _tmp36F=_tmp36E.tl;goto _LL871;}}else{goto _LL872;}_LL872: if((unsigned int)
_tmp364 > 3?*((int*)_tmp364)== Cyc_Absyn_Evar: 0){_LL886: _tmp375=((struct Cyc_Absyn_Evar_struct*)
_tmp364)->f1;goto _LL884;_LL884: _tmp373=((struct Cyc_Absyn_Evar_struct*)_tmp364)->f4;
goto _LL873;}else{goto _LL874;}_LL874: goto _LL875;_LL871: {void*_tmp377=Cyc_Tcutil_compress(
_tmp371);struct Cyc_Core_Opt*_tmp37D;_LL889: if((unsigned int)_tmp377 > 3?*((int*)
_tmp377)== Cyc_Absyn_Evar: 0){_LL894: _tmp37D=((struct Cyc_Absyn_Evar_struct*)
_tmp377)->f4;goto _LL890;}else{goto _LL891;}_LL891: goto _LL892;_LL890: return({
struct _tuple9*_tmp37F=_cycalloc(sizeof(struct _tuple9));_tmp37F->f1=_tmp371;
_tmp37F->f2=_tmp36F;_tmp37F->f3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp37D))->v;_tmp37F;});_LL892: return 0;_LL888:;}_LL873: if(_tmp375 == 
0? 1:(void*)_tmp375->v != (void*)Cyc_Absyn_EffKind){({void*_tmp380[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("effect evar has wrong kind",
sizeof(unsigned char),27),_tag_arr(_tmp380,sizeof(void*),0));});}return({struct
_tuple9*_tmp381=_cycalloc(sizeof(struct _tuple9));_tmp381->f1=t;_tmp381->f2=0;
_tmp381->f3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp373))->v;
_tmp381;});_LL875: return 0;_LL869:;}static struct Cyc_Core_Opt Cyc_Tcutil_ek={(void*)((
void*)4)};static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct*
_tmp382=({struct Cyc_Absyn_FnType_struct*_tmp383=_cycalloc(sizeof(struct Cyc_Absyn_FnType_struct));
_tmp383[0]=({struct Cyc_Absyn_FnType_struct _tmp384;_tmp384.tag=Cyc_Absyn_FnType;
_tmp384.f1=({struct Cyc_Absyn_FnInfo _tmp385;_tmp385.tvars=0;_tmp385.effect=({
struct Cyc_Core_Opt*_tmp386=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp386->v=(
void*)eff;_tmp386;});_tmp385.ret_typ=(void*)((void*)Cyc_Absyn_VoidType);_tmp385.args=
0;_tmp385.c_varargs=0;_tmp385.cyc_varargs=0;_tmp385.rgn_po=0;_tmp385.attributes=
0;_tmp385;});_tmp384;});_tmp383;});return Cyc_Absyn_atb_typ((void*)_tmp382,(void*)
Cyc_Absyn_HeapRgn,Cyc_Absyn_empty_tqual(),Cyc_Absyn_bounds_one);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)Cyc_Absyn_HeapRgn){
return 1;}{void*_tmp387=Cyc_Tcutil_compress(e);void*_tmp393;struct Cyc_List_List*
_tmp395;void*_tmp397;struct Cyc_Core_Opt*_tmp399;struct Cyc_Core_Opt*_tmp39B;
struct Cyc_Core_Opt**_tmp39D;struct Cyc_Core_Opt*_tmp39E;_LL905: if((unsigned int)
_tmp387 > 3?*((int*)_tmp387)== Cyc_Absyn_AccessEff: 0){_LL916: _tmp393=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp387)->f1;goto _LL906;}else{goto _LL907;}
_LL907: if((unsigned int)_tmp387 > 3?*((int*)_tmp387)== Cyc_Absyn_JoinEff: 0){
_LL918: _tmp395=((struct Cyc_Absyn_JoinEff_struct*)_tmp387)->f1;goto _LL908;}else{
goto _LL909;}_LL909: if((unsigned int)_tmp387 > 3?*((int*)_tmp387)== Cyc_Absyn_RgnsEff:
0){_LL920: _tmp397=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp387)->f1;goto
_LL910;}else{goto _LL911;}_LL911: if((unsigned int)_tmp387 > 3?*((int*)_tmp387)== 
Cyc_Absyn_Evar: 0){_LL927: _tmp39E=((struct Cyc_Absyn_Evar_struct*)_tmp387)->f1;
goto _LL924;_LL924: _tmp39B=((struct Cyc_Absyn_Evar_struct*)_tmp387)->f2;_tmp39D=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp387)->f2;goto _LL922;
_LL922: _tmp399=((struct Cyc_Absyn_Evar_struct*)_tmp387)->f4;goto _LL912;}else{goto
_LL913;}_LL913: goto _LL914;_LL906: if(constrain){return Cyc_Tcutil_unify(r,_tmp393);}
_tmp393=Cyc_Tcutil_compress(_tmp393);if(r == _tmp393){return 1;}{struct _tuple8
_tmp3A1=({struct _tuple8 _tmp3A0;_tmp3A0.f1=r;_tmp3A0.f2=_tmp393;_tmp3A0;});void*
_tmp3A7;struct Cyc_Absyn_Tvar*_tmp3A9;void*_tmp3AB;struct Cyc_Absyn_Tvar*_tmp3AD;
_LL931: _LL940: _tmp3AB=_tmp3A1.f1;if((unsigned int)_tmp3AB > 3?*((int*)_tmp3AB)== 
Cyc_Absyn_VarType: 0){_LL942: _tmp3AD=((struct Cyc_Absyn_VarType_struct*)_tmp3AB)->f1;
goto _LL936;}else{goto _LL933;}_LL936: _tmp3A7=_tmp3A1.f2;if((unsigned int)_tmp3A7 > 
3?*((int*)_tmp3A7)== Cyc_Absyn_VarType: 0){_LL938: _tmp3A9=((struct Cyc_Absyn_VarType_struct*)
_tmp3A7)->f1;goto _LL932;}else{goto _LL933;}_LL933: goto _LL934;_LL932: return Cyc_Absyn_tvar_cmp(
_tmp3AD,_tmp3A9)== 0;_LL934: return 0;_LL930:;}_LL908: for(0;_tmp395 != 0;_tmp395=
_tmp395->tl){if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp395->hd)){
return 1;}}return 0;_LL910: {void*_tmp3AF=Cyc_Tcutil_rgns_of(_tmp397);void*_tmp3B5;
_LL945: if((unsigned int)_tmp3AF > 3?*((int*)_tmp3AF)== Cyc_Absyn_RgnsEff: 0){
_LL950: _tmp3B5=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp3AF)->f1;goto _LL946;}
else{goto _LL947;}_LL947: goto _LL948;_LL946: if(! constrain){return 0;}{void*_tmp3B7=
Cyc_Tcutil_compress(_tmp3B5);struct Cyc_Core_Opt*_tmp3BD;struct Cyc_Core_Opt*
_tmp3BF;struct Cyc_Core_Opt**_tmp3C1;struct Cyc_Core_Opt*_tmp3C2;_LL953: if((
unsigned int)_tmp3B7 > 3?*((int*)_tmp3B7)== Cyc_Absyn_Evar: 0){_LL963: _tmp3C2=((
struct Cyc_Absyn_Evar_struct*)_tmp3B7)->f1;goto _LL960;_LL960: _tmp3BF=((struct Cyc_Absyn_Evar_struct*)
_tmp3B7)->f2;_tmp3C1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp3B7)->f2;goto _LL958;_LL958: _tmp3BD=((struct Cyc_Absyn_Evar_struct*)_tmp3B7)->f4;
goto _LL954;}else{goto _LL955;}_LL955: goto _LL956;_LL954: {void*_tmp3C4=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp3BD);Cyc_Tcutil_occurs(_tmp3C4,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3BD))->v,r);{void*
_tmp3C5=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp3C7=
_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp3C7[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp3C8;_tmp3C8.tag=Cyc_Absyn_JoinEff;_tmp3C8.f1=({void*_tmp3C9[2];_tmp3C9[1]=(
void*)({struct Cyc_Absyn_AccessEff_struct*_tmp3CA=_cycalloc(sizeof(struct Cyc_Absyn_AccessEff_struct));
_tmp3CA[0]=({struct Cyc_Absyn_AccessEff_struct _tmp3CB;_tmp3CB.tag=Cyc_Absyn_AccessEff;
_tmp3CB.f1=(void*)r;_tmp3CB;});_tmp3CA;});_tmp3C9[0]=_tmp3C4;Cyc_List_list(
_tag_arr(_tmp3C9,sizeof(void*),2));});_tmp3C8;});_tmp3C7;}));*_tmp3C1=({struct
Cyc_Core_Opt*_tmp3C6=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp3C6->v=(void*)
_tmp3C5;_tmp3C6;});return 1;}}_LL956: return 0;_LL952:;}_LL948: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp3AF);_LL944:;}_LL912: if(_tmp39E == 0? 1:(void*)_tmp39E->v != (void*)
Cyc_Absyn_EffKind){({void*_tmp3CC[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("effect evar has wrong kind",sizeof(
unsigned char),27),_tag_arr(_tmp3CC,sizeof(void*),0));});}if(! constrain){return 0;}{
void*_tmp3CD=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp399);Cyc_Tcutil_occurs(
_tmp3CD,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp399))->v,r);{struct Cyc_Absyn_JoinEff_struct*_tmp3CE=({struct Cyc_Absyn_JoinEff_struct*
_tmp3D0=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp3D0[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp3D1;_tmp3D1.tag=Cyc_Absyn_JoinEff;_tmp3D1.f1=({struct Cyc_List_List*_tmp3D2=
_cycalloc(sizeof(struct Cyc_List_List));_tmp3D2->hd=(void*)_tmp3CD;_tmp3D2->tl=({
struct Cyc_List_List*_tmp3D3=_cycalloc(sizeof(struct Cyc_List_List));_tmp3D3->hd=(
void*)((void*)({struct Cyc_Absyn_AccessEff_struct*_tmp3D4=_cycalloc(sizeof(struct
Cyc_Absyn_AccessEff_struct));_tmp3D4[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp3D5;_tmp3D5.tag=Cyc_Absyn_AccessEff;_tmp3D5.f1=(void*)r;_tmp3D5;});_tmp3D4;}));
_tmp3D3->tl=0;_tmp3D3;});_tmp3D2;});_tmp3D1;});_tmp3D0;});*_tmp39D=({struct Cyc_Core_Opt*
_tmp3CF=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp3CF->v=(void*)((void*)_tmp3CE);
_tmp3CF;});return 1;}}_LL914: return 0;_LL904:;}}static int Cyc_Tcutil_type_in_effect(
int constrain,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp3D6=Cyc_Tcutil_compress(
e);struct Cyc_List_List*_tmp3E2;void*_tmp3E4;struct Cyc_Core_Opt*_tmp3E6;struct Cyc_Core_Opt*
_tmp3E8;struct Cyc_Core_Opt**_tmp3EA;struct Cyc_Core_Opt*_tmp3EB;_LL984: if((
unsigned int)_tmp3D6 > 3?*((int*)_tmp3D6)== Cyc_Absyn_AccessEff: 0){goto _LL985;}
else{goto _LL986;}_LL986: if((unsigned int)_tmp3D6 > 3?*((int*)_tmp3D6)== Cyc_Absyn_JoinEff:
0){_LL995: _tmp3E2=((struct Cyc_Absyn_JoinEff_struct*)_tmp3D6)->f1;goto _LL987;}
else{goto _LL988;}_LL988: if((unsigned int)_tmp3D6 > 3?*((int*)_tmp3D6)== Cyc_Absyn_RgnsEff:
0){_LL997: _tmp3E4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp3D6)->f1;goto
_LL989;}else{goto _LL990;}_LL990: if((unsigned int)_tmp3D6 > 3?*((int*)_tmp3D6)== 
Cyc_Absyn_Evar: 0){_LL1004: _tmp3EB=((struct Cyc_Absyn_Evar_struct*)_tmp3D6)->f1;
goto _LL1001;_LL1001: _tmp3E8=((struct Cyc_Absyn_Evar_struct*)_tmp3D6)->f2;_tmp3EA=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp3D6)->f2;goto _LL999;
_LL999: _tmp3E6=((struct Cyc_Absyn_Evar_struct*)_tmp3D6)->f4;goto _LL991;}else{goto
_LL992;}_LL992: goto _LL993;_LL985: return 0;_LL987: for(0;_tmp3E2 != 0;_tmp3E2=
_tmp3E2->tl){if(Cyc_Tcutil_type_in_effect(constrain,t,(void*)_tmp3E2->hd)){
return 1;}}return 0;_LL989: _tmp3E4=Cyc_Tcutil_compress(_tmp3E4);if(t == _tmp3E4){
return 1;}if(constrain){return Cyc_Tcutil_unify(t,_tmp3E4);}{void*_tmp3ED=Cyc_Tcutil_rgns_of(
t);void*_tmp3F3;_LL1007: if((unsigned int)_tmp3ED > 3?*((int*)_tmp3ED)== Cyc_Absyn_RgnsEff:
0){_LL1012: _tmp3F3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp3ED)->f1;goto
_LL1008;}else{goto _LL1009;}_LL1009: goto _LL1010;_LL1008: {struct _tuple8 _tmp3F6=({
struct _tuple8 _tmp3F5;_tmp3F5.f1=t;_tmp3F5.f2=Cyc_Tcutil_compress(_tmp3F3);
_tmp3F5;});void*_tmp3FC;struct Cyc_Absyn_Tvar*_tmp3FE;void*_tmp400;struct Cyc_Absyn_Tvar*
_tmp402;_LL1016: _LL1025: _tmp400=_tmp3F6.f1;if((unsigned int)_tmp400 > 3?*((int*)
_tmp400)== Cyc_Absyn_VarType: 0){_LL1027: _tmp402=((struct Cyc_Absyn_VarType_struct*)
_tmp400)->f1;goto _LL1021;}else{goto _LL1018;}_LL1021: _tmp3FC=_tmp3F6.f2;if((
unsigned int)_tmp3FC > 3?*((int*)_tmp3FC)== Cyc_Absyn_VarType: 0){_LL1023: _tmp3FE=((
struct Cyc_Absyn_VarType_struct*)_tmp3FC)->f1;goto _LL1017;}else{goto _LL1018;}
_LL1018: goto _LL1019;_LL1017: return Cyc_Tcutil_unify(t,_tmp3F3);_LL1019: return t == 
_tmp3F3;_LL1015:;}_LL1010: return Cyc_Tcutil_type_in_effect(constrain,t,_tmp3ED);
_LL1006:;}_LL991: if(_tmp3EB == 0? 1:(void*)_tmp3EB->v != (void*)Cyc_Absyn_EffKind){({
void*_tmp404[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(unsigned char),27),_tag_arr(_tmp404,
sizeof(void*),0));});}if(! constrain){return 0;}{void*_tmp405=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp3E6);Cyc_Tcutil_occurs(_tmp405,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3E6))->v,t);{struct
Cyc_Absyn_JoinEff_struct*_tmp406=({struct Cyc_Absyn_JoinEff_struct*_tmp408=
_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp408[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp409;_tmp409.tag=Cyc_Absyn_JoinEff;_tmp409.f1=({struct Cyc_List_List*_tmp40A=
_cycalloc(sizeof(struct Cyc_List_List));_tmp40A->hd=(void*)_tmp405;_tmp40A->tl=({
struct Cyc_List_List*_tmp40B=_cycalloc(sizeof(struct Cyc_List_List));_tmp40B->hd=(
void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp40C=_cycalloc(sizeof(struct
Cyc_Absyn_RgnsEff_struct));_tmp40C[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp40D;
_tmp40D.tag=Cyc_Absyn_RgnsEff;_tmp40D.f1=(void*)t;_tmp40D;});_tmp40C;}));_tmp40B->tl=
0;_tmp40B;});_tmp40A;});_tmp409;});_tmp408;});*_tmp3EA=({struct Cyc_Core_Opt*
_tmp407=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp407->v=(void*)((void*)_tmp406);
_tmp407;});return 1;}}_LL993: return 0;_LL983:;}}static int Cyc_Tcutil_variable_in_effect(
int constrain,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*
_tmp40E=e;struct Cyc_Absyn_Tvar*_tmp41A;struct Cyc_List_List*_tmp41C;void*_tmp41E;
struct Cyc_Core_Opt*_tmp420;struct Cyc_Core_Opt*_tmp422;struct Cyc_Core_Opt**
_tmp424;struct Cyc_Core_Opt*_tmp425;_LL1040: if((unsigned int)_tmp40E > 3?*((int*)
_tmp40E)== Cyc_Absyn_VarType: 0){_LL1051: _tmp41A=((struct Cyc_Absyn_VarType_struct*)
_tmp40E)->f1;goto _LL1041;}else{goto _LL1042;}_LL1042: if((unsigned int)_tmp40E > 3?*((
int*)_tmp40E)== Cyc_Absyn_JoinEff: 0){_LL1053: _tmp41C=((struct Cyc_Absyn_JoinEff_struct*)
_tmp40E)->f1;goto _LL1043;}else{goto _LL1044;}_LL1044: if((unsigned int)_tmp40E > 3?*((
int*)_tmp40E)== Cyc_Absyn_RgnsEff: 0){_LL1055: _tmp41E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp40E)->f1;goto _LL1045;}else{goto _LL1046;}_LL1046: if((unsigned int)_tmp40E > 3?*((
int*)_tmp40E)== Cyc_Absyn_Evar: 0){_LL1062: _tmp425=((struct Cyc_Absyn_Evar_struct*)
_tmp40E)->f1;goto _LL1059;_LL1059: _tmp422=((struct Cyc_Absyn_Evar_struct*)_tmp40E)->f2;
_tmp424=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp40E)->f2;goto
_LL1057;_LL1057: _tmp420=((struct Cyc_Absyn_Evar_struct*)_tmp40E)->f4;goto _LL1047;}
else{goto _LL1048;}_LL1048: goto _LL1049;_LL1041: return Cyc_Absyn_tvar_cmp(v,_tmp41A)
== 0;_LL1043: for(0;_tmp41C != 0;_tmp41C=_tmp41C->tl){if(Cyc_Tcutil_variable_in_effect(
constrain,v,(void*)_tmp41C->hd)){return 1;}}return 0;_LL1045: {void*_tmp427=Cyc_Tcutil_rgns_of(
_tmp41E);void*_tmp42D;_LL1065: if((unsigned int)_tmp427 > 3?*((int*)_tmp427)== Cyc_Absyn_RgnsEff:
0){_LL1070: _tmp42D=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp427)->f1;goto
_LL1066;}else{goto _LL1067;}_LL1067: goto _LL1068;_LL1066: if(! constrain){return 0;}{
void*_tmp42F=Cyc_Tcutil_compress(_tmp42D);struct Cyc_Core_Opt*_tmp435;struct Cyc_Core_Opt*
_tmp437;struct Cyc_Core_Opt**_tmp439;struct Cyc_Core_Opt*_tmp43A;_LL1073: if((
unsigned int)_tmp42F > 3?*((int*)_tmp42F)== Cyc_Absyn_Evar: 0){_LL1083: _tmp43A=((
struct Cyc_Absyn_Evar_struct*)_tmp42F)->f1;goto _LL1080;_LL1080: _tmp437=((struct
Cyc_Absyn_Evar_struct*)_tmp42F)->f2;_tmp439=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp42F)->f2;goto _LL1078;_LL1078: _tmp435=((struct Cyc_Absyn_Evar_struct*)_tmp42F)->f4;
goto _LL1074;}else{goto _LL1075;}_LL1075: goto _LL1076;_LL1074: {void*_tmp43C=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp435);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp435))->v,v)){return 0;}{void*_tmp43D=Cyc_Tcutil_dummy_fntype((void*)({struct
Cyc_Absyn_JoinEff_struct*_tmp43F=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));
_tmp43F[0]=({struct Cyc_Absyn_JoinEff_struct _tmp440;_tmp440.tag=Cyc_Absyn_JoinEff;
_tmp440.f1=({void*_tmp441[2];_tmp441[1]=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp442=_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));_tmp442[0]=({struct Cyc_Absyn_VarType_struct
_tmp443;_tmp443.tag=Cyc_Absyn_VarType;_tmp443.f1=v;_tmp443;});_tmp442;});_tmp441[
0]=_tmp43C;Cyc_List_list(_tag_arr(_tmp441,sizeof(void*),2));});_tmp440;});
_tmp43F;}));*_tmp439=({struct Cyc_Core_Opt*_tmp43E=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp43E->v=(void*)_tmp43D;_tmp43E;});return 1;}}_LL1076: return 0;_LL1072:;}_LL1068:
return Cyc_Tcutil_variable_in_effect(constrain,v,_tmp427);_LL1064:;}_LL1047: if(
_tmp425 == 0? 1:(void*)_tmp425->v != (void*)Cyc_Absyn_EffKind){({void*_tmp444[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("effect evar has wrong kind",
sizeof(unsigned char),27),_tag_arr(_tmp444,sizeof(void*),0));});}{void*_tmp445=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp420);if(!((int(*)(int(*
compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp420))->v,v)){return 0;}{struct Cyc_Absyn_JoinEff_struct*
_tmp446=({struct Cyc_Absyn_JoinEff_struct*_tmp448=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));
_tmp448[0]=({struct Cyc_Absyn_JoinEff_struct _tmp449;_tmp449.tag=Cyc_Absyn_JoinEff;
_tmp449.f1=({struct Cyc_List_List*_tmp44A=_cycalloc(sizeof(struct Cyc_List_List));
_tmp44A->hd=(void*)_tmp445;_tmp44A->tl=({struct Cyc_List_List*_tmp44B=_cycalloc(
sizeof(struct Cyc_List_List));_tmp44B->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp44C=_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));_tmp44C[0]=({struct Cyc_Absyn_VarType_struct
_tmp44D;_tmp44D.tag=Cyc_Absyn_VarType;_tmp44D.f1=v;_tmp44D;});_tmp44C;}));
_tmp44B->tl=0;_tmp44B;});_tmp44A;});_tmp449;});_tmp448;});*_tmp424=({struct Cyc_Core_Opt*
_tmp447=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp447->v=(void*)((void*)_tmp446);
_tmp447;});return 1;}}_LL1049: return 0;_LL1039:;}}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp44E=e;struct Cyc_List_List*
_tmp458;void*_tmp45A;_LL1104: if((unsigned int)_tmp44E > 3?*((int*)_tmp44E)== Cyc_Absyn_JoinEff:
0){_LL1113: _tmp458=((struct Cyc_Absyn_JoinEff_struct*)_tmp44E)->f1;goto _LL1105;}
else{goto _LL1106;}_LL1106: if((unsigned int)_tmp44E > 3?*((int*)_tmp44E)== Cyc_Absyn_RgnsEff:
0){_LL1115: _tmp45A=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp44E)->f1;goto
_LL1107;}else{goto _LL1108;}_LL1108: if((unsigned int)_tmp44E > 3?*((int*)_tmp44E)
== Cyc_Absyn_Evar: 0){goto _LL1109;}else{goto _LL1110;}_LL1110: goto _LL1111;_LL1105:
for(0;_tmp458 != 0;_tmp458=_tmp458->tl){if(Cyc_Tcutil_evar_in_effect(evar,(void*)
_tmp458->hd)){return 1;}}return 0;_LL1107: {void*_tmp45C=Cyc_Tcutil_rgns_of(
_tmp45A);void*_tmp462;_LL1118: if((unsigned int)_tmp45C > 3?*((int*)_tmp45C)== Cyc_Absyn_RgnsEff:
0){_LL1123: _tmp462=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp45C)->f1;goto
_LL1119;}else{goto _LL1120;}_LL1120: goto _LL1121;_LL1119: return 0;_LL1121: return Cyc_Tcutil_evar_in_effect(
evar,_tmp45C);_LL1117:;}_LL1109: return evar == e;_LL1111: return 0;_LL1103:;}}int Cyc_Tcutil_subset_effect(
int set_to_empty,void*e1,void*e2){void*_tmp464=Cyc_Tcutil_compress(e1);struct Cyc_List_List*
_tmp472;void*_tmp474;struct Cyc_Absyn_Tvar*_tmp476;void*_tmp478;struct Cyc_Core_Opt*
_tmp47A;struct Cyc_Core_Opt*_tmp47C;struct Cyc_Core_Opt**_tmp47E;_LL1126: if((
unsigned int)_tmp464 > 3?*((int*)_tmp464)== Cyc_Absyn_JoinEff: 0){_LL1139: _tmp472=((
struct Cyc_Absyn_JoinEff_struct*)_tmp464)->f1;goto _LL1127;}else{goto _LL1128;}
_LL1128: if((unsigned int)_tmp464 > 3?*((int*)_tmp464)== Cyc_Absyn_AccessEff: 0){
_LL1141: _tmp474=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp464)->f1;goto
_LL1129;}else{goto _LL1130;}_LL1130: if((unsigned int)_tmp464 > 3?*((int*)_tmp464)
== Cyc_Absyn_VarType: 0){_LL1143: _tmp476=((struct Cyc_Absyn_VarType_struct*)
_tmp464)->f1;goto _LL1131;}else{goto _LL1132;}_LL1132: if((unsigned int)_tmp464 > 3?*((
int*)_tmp464)== Cyc_Absyn_RgnsEff: 0){_LL1145: _tmp478=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp464)->f1;goto _LL1133;}else{goto _LL1134;}_LL1134: if((unsigned int)_tmp464 > 3?*((
int*)_tmp464)== Cyc_Absyn_Evar: 0){_LL1149: _tmp47C=((struct Cyc_Absyn_Evar_struct*)
_tmp464)->f2;_tmp47E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp464)->f2;goto _LL1147;_LL1147: _tmp47A=((struct Cyc_Absyn_Evar_struct*)_tmp464)->f4;
goto _LL1135;}else{goto _LL1136;}_LL1136: goto _LL1137;_LL1127: for(0;_tmp472 != 0;
_tmp472=_tmp472->tl){if(! Cyc_Tcutil_subset_effect(set_to_empty,(void*)_tmp472->hd,
e2)){return 0;}}return 1;_LL1129: return(Cyc_Tcutil_region_in_effect(0,_tmp474,e2)?
1: Cyc_Tcutil_region_in_effect(1,_tmp474,e2))? 1: Cyc_Tcutil_unify(_tmp474,(void*)
Cyc_Absyn_HeapRgn);_LL1131: return Cyc_Tcutil_variable_in_effect(0,_tmp476,e2)? 1:
Cyc_Tcutil_variable_in_effect(1,_tmp476,e2);_LL1133: {void*_tmp47F=Cyc_Tcutil_rgns_of(
_tmp478);void*_tmp485;_LL1153: if((unsigned int)_tmp47F > 3?*((int*)_tmp47F)== Cyc_Absyn_RgnsEff:
0){_LL1158: _tmp485=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp47F)->f1;goto
_LL1154;}else{goto _LL1155;}_LL1155: goto _LL1156;_LL1154: return(Cyc_Tcutil_type_in_effect(
0,_tmp485,e2)? 1: Cyc_Tcutil_type_in_effect(1,_tmp485,e2))? 1: Cyc_Tcutil_unify(
_tmp485,Cyc_Absyn_sint_t);_LL1156: return Cyc_Tcutil_subset_effect(set_to_empty,
_tmp47F,e2);_LL1152:;}_LL1135: if(! Cyc_Tcutil_evar_in_effect(e1,e2)){if(
set_to_empty){*_tmp47E=({struct Cyc_Core_Opt*_tmp487=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp487->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp488=_cycalloc(
sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp488[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp489;_tmp489.tag=Cyc_Absyn_JoinEff;_tmp489.f1=0;_tmp489;});_tmp488;}));
_tmp487;});}else{Cyc_Tcutil_occurs(e1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp47A))->v,e2);*_tmp47E=({struct Cyc_Core_Opt*
_tmp48A=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp48A->v=(void*)e2;_tmp48A;});}}
return 1;_LL1137: return({struct Cyc_Std_String_pa_struct _tmp48C;_tmp48C.tag=Cyc_Std_String_pa;
_tmp48C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e1);{void*_tmp48B[1]={&
_tmp48C};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("subset_effect: bad effect: %s",sizeof(unsigned char),30),_tag_arr(
_tmp48B,sizeof(void*),1));}});_LL1125:;}struct _tuple10{struct _tuple9*f1;struct
_tuple9*f2;};static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);{struct _tuple10 _tmp48E=({struct _tuple10
_tmp48D;_tmp48D.f1=Cyc_Tcutil_get_effect_evar(e1);_tmp48D.f2=Cyc_Tcutil_get_effect_evar(
e2);_tmp48D;});_LL1168: goto _LL1169;_LL1169: return Cyc_Tcutil_subset_effect(0,e1,
e2)? Cyc_Tcutil_subset_effect(0,e2,e1): 0;_LL1167:;}}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple8 _tmp494;void*_tmp495;void*_tmp497;struct
_tuple8*_tmp492=(struct _tuple8*)r1->hd;_tmp494=*_tmp492;_LL1176: _tmp497=_tmp494.f1;
goto _LL1174;_LL1174: _tmp495=_tmp494.f2;goto _LL1171;_LL1171: {int found=_tmp497 == (
void*)Cyc_Absyn_HeapRgn;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0? ! found: 0;r2=r2->tl){
struct _tuple8 _tmp49B;void*_tmp49C;void*_tmp49E;struct _tuple8*_tmp499=(struct
_tuple8*)r2->hd;_tmp49B=*_tmp499;_LL1183: _tmp49E=_tmp49B.f1;goto _LL1181;_LL1181:
_tmp49C=_tmp49B.f2;goto _LL1178;_LL1178: if(Cyc_Tcutil_unify(_tmp497,_tmp49E)? Cyc_Tcutil_unify(
_tmp495,_tmp49C): 0){found=1;break;}}}if(! found){return 0;}}}}return 1;}static int
Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return
Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)? Cyc_Tcutil_sub_rgnpo(rpo2,rpo1): 0;}struct
_tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};struct
_tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _tagged_arr)_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2){return;}{void*_tmp4A0=t1;struct Cyc_Core_Opt*_tmp4A6;struct Cyc_Core_Opt*
_tmp4A8;struct Cyc_Core_Opt**_tmp4AA;struct Cyc_Core_Opt*_tmp4AB;_LL1186: if((
unsigned int)_tmp4A0 > 3?*((int*)_tmp4A0)== Cyc_Absyn_Evar: 0){_LL1196: _tmp4AB=((
struct Cyc_Absyn_Evar_struct*)_tmp4A0)->f1;goto _LL1193;_LL1193: _tmp4A8=((struct
Cyc_Absyn_Evar_struct*)_tmp4A0)->f2;_tmp4AA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp4A0)->f2;goto _LL1191;_LL1191: _tmp4A6=((struct Cyc_Absyn_Evar_struct*)_tmp4A0)->f4;
goto _LL1187;}else{goto _LL1188;}_LL1188: goto _LL1189;_LL1187: Cyc_Tcutil_occurs(t1,
Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp4A6))->v,t2);{void*_tmp4AD=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(
_tmp4AD,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4AB))->v)){*_tmp4AA=({
struct Cyc_Core_Opt*_tmp4AE=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp4AE->v=(
void*)t2;_tmp4AE;});return;}else{{void*_tmp4AF=t2;struct Cyc_Core_Opt*_tmp4B7;
struct Cyc_Core_Opt*_tmp4B9;struct Cyc_Core_Opt**_tmp4BB;struct Cyc_Absyn_PtrInfo
_tmp4BD;_LL1201: if((unsigned int)_tmp4AF > 3?*((int*)_tmp4AF)== Cyc_Absyn_Evar: 0){
_LL1210: _tmp4B9=((struct Cyc_Absyn_Evar_struct*)_tmp4AF)->f2;_tmp4BB=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp4AF)->f2;goto _LL1208;_LL1208: _tmp4B7=((struct
Cyc_Absyn_Evar_struct*)_tmp4AF)->f4;goto _LL1202;}else{goto _LL1203;}_LL1203: if((
unsigned int)_tmp4AF > 3?*((int*)_tmp4AF)== Cyc_Absyn_PointerType: 0){_LL1214:
_tmp4BD=((struct Cyc_Absyn_PointerType_struct*)_tmp4AF)->f1;goto _LL1212;}else{
goto _LL1205;}_LL1212: if((void*)_tmp4AB->v == (void*)Cyc_Absyn_BoxKind){goto
_LL1204;}else{goto _LL1205;}_LL1205: goto _LL1206;_LL1202: {struct Cyc_List_List*
_tmp4BF=(struct Cyc_List_List*)_tmp4A6->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4B7))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(
int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,
struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp4BF,(struct
Cyc_Absyn_Tvar*)s2->hd)){Cyc_Tcutil_failure_reason=_tag_arr("(type variable would escape scope)",
sizeof(unsigned char),35);(int)_throw((void*)Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((
void*)_tmp4AB->v,_tmp4AD)){*_tmp4BB=({struct Cyc_Core_Opt*_tmp4C0=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp4C0->v=(void*)t1;_tmp4C0;});return;}Cyc_Tcutil_failure_reason=
_tag_arr("(kinds are incompatible)",sizeof(unsigned char),25);goto _LL1200;}
_LL1204: {struct Cyc_Absyn_Conref*_tmp4C1=Cyc_Absyn_compress_conref(_tmp4BD.bounds);{
void*_tmp4C2=(void*)_tmp4C1->v;_LL1220: if(_tmp4C2 == (void*)Cyc_Absyn_No_constr){
goto _LL1221;}else{goto _LL1222;}_LL1222: goto _LL1223;_LL1221:(void*)(_tmp4C1->v=(
void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp4C8=_cycalloc(sizeof(struct
Cyc_Absyn_Eq_constr_struct));_tmp4C8[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp4C9;_tmp4C9.tag=Cyc_Absyn_Eq_constr;_tmp4C9.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp4CA=_cycalloc(sizeof(struct Cyc_Absyn_Upper_b_struct));_tmp4CA[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp4CB;_tmp4CB.tag=Cyc_Absyn_Upper_b;_tmp4CB.f1=Cyc_Absyn_signed_int_exp(1,0);
_tmp4CB;});_tmp4CA;}));_tmp4C9;});_tmp4C8;})));*_tmp4AA=({struct Cyc_Core_Opt*
_tmp4CC=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp4CC->v=(void*)t2;_tmp4CC;});
return;_LL1223: goto _LL1219;_LL1219:;}goto _LL1200;}_LL1206: goto _LL1200;_LL1200:;}
Cyc_Tcutil_failure_reason=_tag_arr("(kinds are incompatible)",sizeof(
unsigned char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}}_LL1189: goto _LL1185;
_LL1185:;}{struct _tuple8 _tmp4CE=({struct _tuple8 _tmp4CD;_tmp4CD.f1=t2;_tmp4CD.f2=
t1;_tmp4CD;});void*_tmp508;void*_tmp50A;void*_tmp50C;void*_tmp50E;struct Cyc_Absyn_Tvar*
_tmp510;void*_tmp512;struct Cyc_Absyn_Tvar*_tmp514;void*_tmp516;struct Cyc_Absyn_Structdecl**
_tmp518;struct Cyc_List_List*_tmp51A;struct _tuple1*_tmp51C;void*_tmp51E;struct Cyc_Absyn_Structdecl**
_tmp520;struct Cyc_List_List*_tmp522;struct _tuple1*_tmp524;void*_tmp526;struct
_tuple1*_tmp528;void*_tmp52A;struct _tuple1*_tmp52C;void*_tmp52E;struct Cyc_List_List*
_tmp530;void*_tmp532;struct Cyc_List_List*_tmp534;void*_tmp536;struct Cyc_Absyn_Uniondecl**
_tmp538;struct Cyc_List_List*_tmp53A;struct _tuple1*_tmp53C;void*_tmp53E;struct Cyc_Absyn_Uniondecl**
_tmp540;struct Cyc_List_List*_tmp542;struct _tuple1*_tmp544;void*_tmp546;struct Cyc_Absyn_TunionInfo
_tmp548;void*_tmp54A;struct Cyc_List_List*_tmp54C;void*_tmp54E;struct Cyc_Absyn_Tuniondecl**
_tmp550;struct Cyc_Absyn_Tuniondecl*_tmp552;void*_tmp553;struct Cyc_Absyn_TunionInfo
_tmp555;void*_tmp557;struct Cyc_List_List*_tmp559;void*_tmp55B;struct Cyc_Absyn_Tuniondecl**
_tmp55D;struct Cyc_Absyn_Tuniondecl*_tmp55F;void*_tmp560;struct Cyc_Absyn_TunionFieldInfo
_tmp562;struct Cyc_List_List*_tmp564;void*_tmp566;struct Cyc_Absyn_Tunionfield*
_tmp568;struct Cyc_Absyn_Tuniondecl*_tmp56A;void*_tmp56C;struct Cyc_Absyn_TunionFieldInfo
_tmp56E;struct Cyc_List_List*_tmp570;void*_tmp572;struct Cyc_Absyn_Tunionfield*
_tmp574;struct Cyc_Absyn_Tuniondecl*_tmp576;void*_tmp578;struct Cyc_Absyn_PtrInfo
_tmp57A;struct Cyc_Absyn_Conref*_tmp57C;struct Cyc_Absyn_Tqual _tmp57E;struct Cyc_Absyn_Conref*
_tmp580;void*_tmp582;void*_tmp584;void*_tmp586;struct Cyc_Absyn_PtrInfo _tmp588;
struct Cyc_Absyn_Conref*_tmp58A;struct Cyc_Absyn_Tqual _tmp58C;struct Cyc_Absyn_Conref*
_tmp58E;void*_tmp590;void*_tmp592;void*_tmp594;void*_tmp596;void*_tmp598;void*
_tmp59A;void*_tmp59C;void*_tmp59E;void*_tmp5A0;void*_tmp5A2;void*_tmp5A4;int
_tmp5A6;void*_tmp5A8;int _tmp5AA;void*_tmp5AC;void*_tmp5AE;void*_tmp5B0;void*
_tmp5B2;void*_tmp5B4;struct Cyc_Absyn_Exp*_tmp5B6;struct Cyc_Absyn_Tqual _tmp5B8;
void*_tmp5BA;void*_tmp5BC;struct Cyc_Absyn_Exp*_tmp5BE;struct Cyc_Absyn_Tqual
_tmp5C0;void*_tmp5C2;void*_tmp5C4;struct Cyc_Absyn_FnInfo _tmp5C6;struct Cyc_List_List*
_tmp5C8;struct Cyc_List_List*_tmp5CA;struct Cyc_Absyn_VarargInfo*_tmp5CC;int
_tmp5CE;struct Cyc_List_List*_tmp5D0;void*_tmp5D2;struct Cyc_Core_Opt*_tmp5D4;
struct Cyc_List_List*_tmp5D6;void*_tmp5D8;struct Cyc_Absyn_FnInfo _tmp5DA;struct Cyc_List_List*
_tmp5DC;struct Cyc_List_List*_tmp5DE;struct Cyc_Absyn_VarargInfo*_tmp5E0;int
_tmp5E2;struct Cyc_List_List*_tmp5E4;void*_tmp5E6;struct Cyc_Core_Opt*_tmp5E8;
struct Cyc_List_List*_tmp5EA;void*_tmp5EC;struct Cyc_List_List*_tmp5EE;void*
_tmp5F0;struct Cyc_List_List*_tmp5F2;void*_tmp5F4;struct Cyc_List_List*_tmp5F6;
void*_tmp5F8;struct Cyc_List_List*_tmp5FA;void*_tmp5FC;struct Cyc_List_List*
_tmp5FE;void*_tmp600;struct Cyc_List_List*_tmp602;void*_tmp604;void*_tmp606;void*
_tmp608;void*_tmp60A;void*_tmp60C;void*_tmp60E;void*_tmp610;void*_tmp612;void*
_tmp614;void*_tmp616;void*_tmp618;void*_tmp61A;_LL1232: _LL1289: _tmp508=_tmp4CE.f1;
if((unsigned int)_tmp508 > 3?*((int*)_tmp508)== Cyc_Absyn_Evar: 0){goto _LL1233;}
else{goto _LL1234;}_LL1234: _LL1293: _tmp50C=_tmp4CE.f1;if(_tmp50C == (void*)Cyc_Absyn_VoidType){
goto _LL1291;}else{goto _LL1236;}_LL1291: _tmp50A=_tmp4CE.f2;if(_tmp50A == (void*)
Cyc_Absyn_VoidType){goto _LL1235;}else{goto _LL1236;}_LL1236: _LL1299: _tmp512=
_tmp4CE.f1;if((unsigned int)_tmp512 > 3?*((int*)_tmp512)== Cyc_Absyn_VarType: 0){
_LL1301: _tmp514=((struct Cyc_Absyn_VarType_struct*)_tmp512)->f1;goto _LL1295;}
else{goto _LL1238;}_LL1295: _tmp50E=_tmp4CE.f2;if((unsigned int)_tmp50E > 3?*((int*)
_tmp50E)== Cyc_Absyn_VarType: 0){_LL1297: _tmp510=((struct Cyc_Absyn_VarType_struct*)
_tmp50E)->f1;goto _LL1237;}else{goto _LL1238;}_LL1238: _LL1311: _tmp51E=_tmp4CE.f1;
if((unsigned int)_tmp51E > 3?*((int*)_tmp51E)== Cyc_Absyn_StructType: 0){_LL1317:
_tmp524=((struct Cyc_Absyn_StructType_struct*)_tmp51E)->f1;goto _LL1315;_LL1315:
_tmp522=((struct Cyc_Absyn_StructType_struct*)_tmp51E)->f2;goto _LL1313;_LL1313:
_tmp520=((struct Cyc_Absyn_StructType_struct*)_tmp51E)->f3;goto _LL1303;}else{goto
_LL1240;}_LL1303: _tmp516=_tmp4CE.f2;if((unsigned int)_tmp516 > 3?*((int*)_tmp516)
== Cyc_Absyn_StructType: 0){_LL1309: _tmp51C=((struct Cyc_Absyn_StructType_struct*)
_tmp516)->f1;goto _LL1307;_LL1307: _tmp51A=((struct Cyc_Absyn_StructType_struct*)
_tmp516)->f2;goto _LL1305;_LL1305: _tmp518=((struct Cyc_Absyn_StructType_struct*)
_tmp516)->f3;goto _LL1239;}else{goto _LL1240;}_LL1240: _LL1323: _tmp52A=_tmp4CE.f1;
if((unsigned int)_tmp52A > 3?*((int*)_tmp52A)== Cyc_Absyn_EnumType: 0){_LL1325:
_tmp52C=((struct Cyc_Absyn_EnumType_struct*)_tmp52A)->f1;goto _LL1319;}else{goto
_LL1242;}_LL1319: _tmp526=_tmp4CE.f2;if((unsigned int)_tmp526 > 3?*((int*)_tmp526)
== Cyc_Absyn_EnumType: 0){_LL1321: _tmp528=((struct Cyc_Absyn_EnumType_struct*)
_tmp526)->f1;goto _LL1241;}else{goto _LL1242;}_LL1242: _LL1331: _tmp532=_tmp4CE.f1;
if((unsigned int)_tmp532 > 3?*((int*)_tmp532)== Cyc_Absyn_AnonEnumType: 0){_LL1333:
_tmp534=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp532)->f1;goto _LL1327;}else{
goto _LL1244;}_LL1327: _tmp52E=_tmp4CE.f2;if((unsigned int)_tmp52E > 3?*((int*)
_tmp52E)== Cyc_Absyn_AnonEnumType: 0){_LL1329: _tmp530=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp52E)->f1;goto _LL1243;}else{goto _LL1244;}_LL1244: _LL1343: _tmp53E=_tmp4CE.f1;
if((unsigned int)_tmp53E > 3?*((int*)_tmp53E)== Cyc_Absyn_UnionType: 0){_LL1349:
_tmp544=((struct Cyc_Absyn_UnionType_struct*)_tmp53E)->f1;goto _LL1347;_LL1347:
_tmp542=((struct Cyc_Absyn_UnionType_struct*)_tmp53E)->f2;goto _LL1345;_LL1345:
_tmp540=((struct Cyc_Absyn_UnionType_struct*)_tmp53E)->f3;goto _LL1335;}else{goto
_LL1246;}_LL1335: _tmp536=_tmp4CE.f2;if((unsigned int)_tmp536 > 3?*((int*)_tmp536)
== Cyc_Absyn_UnionType: 0){_LL1341: _tmp53C=((struct Cyc_Absyn_UnionType_struct*)
_tmp536)->f1;goto _LL1339;_LL1339: _tmp53A=((struct Cyc_Absyn_UnionType_struct*)
_tmp536)->f2;goto _LL1337;_LL1337: _tmp538=((struct Cyc_Absyn_UnionType_struct*)
_tmp536)->f3;goto _LL1245;}else{goto _LL1246;}_LL1246: _LL1364: _tmp553=_tmp4CE.f1;
if((unsigned int)_tmp553 > 3?*((int*)_tmp553)== Cyc_Absyn_TunionType: 0){_LL1366:
_tmp555=((struct Cyc_Absyn_TunionType_struct*)_tmp553)->f1;_LL1372: _tmp55B=(void*)
_tmp555.tunion_info;if(*((int*)_tmp55B)== Cyc_Absyn_KnownTunion){_LL1374: _tmp55D=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp55B)->f1;_tmp55F=*_tmp55D;goto _LL1370;}
else{goto _LL1248;}_LL1370: _tmp559=_tmp555.targs;goto _LL1368;_LL1368: _tmp557=(
void*)_tmp555.rgn;goto _LL1351;}else{goto _LL1248;}_LL1351: _tmp546=_tmp4CE.f2;if((
unsigned int)_tmp546 > 3?*((int*)_tmp546)== Cyc_Absyn_TunionType: 0){_LL1353:
_tmp548=((struct Cyc_Absyn_TunionType_struct*)_tmp546)->f1;_LL1359: _tmp54E=(void*)
_tmp548.tunion_info;if(*((int*)_tmp54E)== Cyc_Absyn_KnownTunion){_LL1361: _tmp550=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp54E)->f1;_tmp552=*_tmp550;goto _LL1357;}
else{goto _LL1248;}_LL1357: _tmp54C=_tmp548.targs;goto _LL1355;_LL1355: _tmp54A=(
void*)_tmp548.rgn;goto _LL1247;}else{goto _LL1248;}_LL1248: _LL1389: _tmp56C=_tmp4CE.f1;
if((unsigned int)_tmp56C > 3?*((int*)_tmp56C)== Cyc_Absyn_TunionFieldType: 0){
_LL1391: _tmp56E=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp56C)->f1;_LL1395:
_tmp572=(void*)_tmp56E.field_info;if(*((int*)_tmp572)== Cyc_Absyn_KnownTunionfield){
_LL1399: _tmp576=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp572)->f1;goto
_LL1397;_LL1397: _tmp574=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp572)->f2;
goto _LL1393;}else{goto _LL1250;}_LL1393: _tmp570=_tmp56E.targs;goto _LL1377;}else{
goto _LL1250;}_LL1377: _tmp560=_tmp4CE.f2;if((unsigned int)_tmp560 > 3?*((int*)
_tmp560)== Cyc_Absyn_TunionFieldType: 0){_LL1379: _tmp562=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp560)->f1;_LL1383: _tmp566=(void*)_tmp562.field_info;if(*((int*)_tmp566)== Cyc_Absyn_KnownTunionfield){
_LL1387: _tmp56A=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp566)->f1;goto
_LL1385;_LL1385: _tmp568=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp566)->f2;
goto _LL1381;}else{goto _LL1250;}_LL1381: _tmp564=_tmp562.targs;goto _LL1249;}else{
goto _LL1250;}_LL1250: _LL1415: _tmp586=_tmp4CE.f1;if((unsigned int)_tmp586 > 3?*((
int*)_tmp586)== Cyc_Absyn_PointerType: 0){_LL1417: _tmp588=((struct Cyc_Absyn_PointerType_struct*)
_tmp586)->f1;_LL1427: _tmp592=(void*)_tmp588.elt_typ;goto _LL1425;_LL1425: _tmp590=(
void*)_tmp588.rgn_typ;goto _LL1423;_LL1423: _tmp58E=_tmp588.nullable;goto _LL1421;
_LL1421: _tmp58C=_tmp588.tq;goto _LL1419;_LL1419: _tmp58A=_tmp588.bounds;goto
_LL1401;}else{goto _LL1252;}_LL1401: _tmp578=_tmp4CE.f2;if((unsigned int)_tmp578 > 
3?*((int*)_tmp578)== Cyc_Absyn_PointerType: 0){_LL1403: _tmp57A=((struct Cyc_Absyn_PointerType_struct*)
_tmp578)->f1;_LL1413: _tmp584=(void*)_tmp57A.elt_typ;goto _LL1411;_LL1411: _tmp582=(
void*)_tmp57A.rgn_typ;goto _LL1409;_LL1409: _tmp580=_tmp57A.nullable;goto _LL1407;
_LL1407: _tmp57E=_tmp57A.tq;goto _LL1405;_LL1405: _tmp57C=_tmp57A.bounds;goto
_LL1251;}else{goto _LL1252;}_LL1252: _LL1435: _tmp59A=_tmp4CE.f1;if((unsigned int)
_tmp59A > 3?*((int*)_tmp59A)== Cyc_Absyn_IntType: 0){_LL1439: _tmp59E=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp59A)->f1;goto _LL1437;_LL1437: _tmp59C=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp59A)->f2;goto _LL1429;}else{goto _LL1254;}
_LL1429: _tmp594=_tmp4CE.f2;if((unsigned int)_tmp594 > 3?*((int*)_tmp594)== Cyc_Absyn_IntType:
0){_LL1433: _tmp598=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp594)->f1;goto
_LL1431;_LL1431: _tmp596=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp594)->f2;
goto _LL1253;}else{goto _LL1254;}_LL1254: _LL1443: _tmp5A2=_tmp4CE.f1;if(_tmp5A2 == (
void*)Cyc_Absyn_FloatType){goto _LL1441;}else{goto _LL1256;}_LL1441: _tmp5A0=
_tmp4CE.f2;if(_tmp5A0 == (void*)Cyc_Absyn_FloatType){goto _LL1255;}else{goto
_LL1256;}_LL1256: _LL1449: _tmp5A8=_tmp4CE.f1;if((unsigned int)_tmp5A8 > 3?*((int*)
_tmp5A8)== Cyc_Absyn_DoubleType: 0){_LL1451: _tmp5AA=((struct Cyc_Absyn_DoubleType_struct*)
_tmp5A8)->f1;goto _LL1445;}else{goto _LL1258;}_LL1445: _tmp5A4=_tmp4CE.f2;if((
unsigned int)_tmp5A4 > 3?*((int*)_tmp5A4)== Cyc_Absyn_DoubleType: 0){_LL1447:
_tmp5A6=((struct Cyc_Absyn_DoubleType_struct*)_tmp5A4)->f1;goto _LL1257;}else{goto
_LL1258;}_LL1258: _LL1457: _tmp5B0=_tmp4CE.f1;if((unsigned int)_tmp5B0 > 3?*((int*)
_tmp5B0)== Cyc_Absyn_SizeofType: 0){_LL1459: _tmp5B2=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp5B0)->f1;goto _LL1453;}else{goto _LL1260;}_LL1453: _tmp5AC=_tmp4CE.f2;if((
unsigned int)_tmp5AC > 3?*((int*)_tmp5AC)== Cyc_Absyn_SizeofType: 0){_LL1455:
_tmp5AE=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp5AC)->f1;goto _LL1259;}
else{goto _LL1260;}_LL1260: _LL1469: _tmp5BC=_tmp4CE.f1;if((unsigned int)_tmp5BC > 3?*((
int*)_tmp5BC)== Cyc_Absyn_ArrayType: 0){_LL1475: _tmp5C2=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp5BC)->f1;goto _LL1473;_LL1473: _tmp5C0=((struct Cyc_Absyn_ArrayType_struct*)
_tmp5BC)->f2;goto _LL1471;_LL1471: _tmp5BE=((struct Cyc_Absyn_ArrayType_struct*)
_tmp5BC)->f3;goto _LL1461;}else{goto _LL1262;}_LL1461: _tmp5B4=_tmp4CE.f2;if((
unsigned int)_tmp5B4 > 3?*((int*)_tmp5B4)== Cyc_Absyn_ArrayType: 0){_LL1467:
_tmp5BA=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp5B4)->f1;goto _LL1465;
_LL1465: _tmp5B8=((struct Cyc_Absyn_ArrayType_struct*)_tmp5B4)->f2;goto _LL1463;
_LL1463: _tmp5B6=((struct Cyc_Absyn_ArrayType_struct*)_tmp5B4)->f3;goto _LL1261;}
else{goto _LL1262;}_LL1262: _LL1497: _tmp5D8=_tmp4CE.f1;if((unsigned int)_tmp5D8 > 3?*((
int*)_tmp5D8)== Cyc_Absyn_FnType: 0){_LL1499: _tmp5DA=((struct Cyc_Absyn_FnType_struct*)
_tmp5D8)->f1;_LL1515: _tmp5EA=_tmp5DA.tvars;goto _LL1513;_LL1513: _tmp5E8=_tmp5DA.effect;
goto _LL1511;_LL1511: _tmp5E6=(void*)_tmp5DA.ret_typ;goto _LL1509;_LL1509: _tmp5E4=
_tmp5DA.args;goto _LL1507;_LL1507: _tmp5E2=_tmp5DA.c_varargs;goto _LL1505;_LL1505:
_tmp5E0=_tmp5DA.cyc_varargs;goto _LL1503;_LL1503: _tmp5DE=_tmp5DA.rgn_po;goto
_LL1501;_LL1501: _tmp5DC=_tmp5DA.attributes;goto _LL1477;}else{goto _LL1264;}
_LL1477: _tmp5C4=_tmp4CE.f2;if((unsigned int)_tmp5C4 > 3?*((int*)_tmp5C4)== Cyc_Absyn_FnType:
0){_LL1479: _tmp5C6=((struct Cyc_Absyn_FnType_struct*)_tmp5C4)->f1;_LL1495: _tmp5D6=
_tmp5C6.tvars;goto _LL1493;_LL1493: _tmp5D4=_tmp5C6.effect;goto _LL1491;_LL1491:
_tmp5D2=(void*)_tmp5C6.ret_typ;goto _LL1489;_LL1489: _tmp5D0=_tmp5C6.args;goto
_LL1487;_LL1487: _tmp5CE=_tmp5C6.c_varargs;goto _LL1485;_LL1485: _tmp5CC=_tmp5C6.cyc_varargs;
goto _LL1483;_LL1483: _tmp5CA=_tmp5C6.rgn_po;goto _LL1481;_LL1481: _tmp5C8=_tmp5C6.attributes;
goto _LL1263;}else{goto _LL1264;}_LL1264: _LL1521: _tmp5F0=_tmp4CE.f1;if((
unsigned int)_tmp5F0 > 3?*((int*)_tmp5F0)== Cyc_Absyn_TupleType: 0){_LL1523:
_tmp5F2=((struct Cyc_Absyn_TupleType_struct*)_tmp5F0)->f1;goto _LL1517;}else{goto
_LL1266;}_LL1517: _tmp5EC=_tmp4CE.f2;if((unsigned int)_tmp5EC > 3?*((int*)_tmp5EC)
== Cyc_Absyn_TupleType: 0){_LL1519: _tmp5EE=((struct Cyc_Absyn_TupleType_struct*)
_tmp5EC)->f1;goto _LL1265;}else{goto _LL1266;}_LL1266: _LL1529: _tmp5F8=_tmp4CE.f1;
if((unsigned int)_tmp5F8 > 3?*((int*)_tmp5F8)== Cyc_Absyn_AnonStructType: 0){
_LL1531: _tmp5FA=((struct Cyc_Absyn_AnonStructType_struct*)_tmp5F8)->f1;goto
_LL1525;}else{goto _LL1268;}_LL1525: _tmp5F4=_tmp4CE.f2;if((unsigned int)_tmp5F4 > 
3?*((int*)_tmp5F4)== Cyc_Absyn_AnonStructType: 0){_LL1527: _tmp5F6=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp5F4)->f1;goto _LL1267;}else{goto _LL1268;}_LL1268: _LL1537: _tmp600=_tmp4CE.f1;
if((unsigned int)_tmp600 > 3?*((int*)_tmp600)== Cyc_Absyn_AnonUnionType: 0){
_LL1539: _tmp602=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp600)->f1;goto _LL1533;}
else{goto _LL1270;}_LL1533: _tmp5FC=_tmp4CE.f2;if((unsigned int)_tmp5FC > 3?*((int*)
_tmp5FC)== Cyc_Absyn_AnonUnionType: 0){_LL1535: _tmp5FE=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp5FC)->f1;goto _LL1269;}else{goto _LL1270;}_LL1270: _LL1543: _tmp606=_tmp4CE.f1;
if(_tmp606 == (void*)Cyc_Absyn_HeapRgn){goto _LL1541;}else{goto _LL1272;}_LL1541:
_tmp604=_tmp4CE.f2;if(_tmp604 == (void*)Cyc_Absyn_HeapRgn){goto _LL1271;}else{goto
_LL1272;}_LL1272: _LL1549: _tmp60C=_tmp4CE.f1;if((unsigned int)_tmp60C > 3?*((int*)
_tmp60C)== Cyc_Absyn_RgnHandleType: 0){_LL1551: _tmp60E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp60C)->f1;goto _LL1545;}else{goto _LL1274;}_LL1545: _tmp608=_tmp4CE.f2;if((
unsigned int)_tmp608 > 3?*((int*)_tmp608)== Cyc_Absyn_RgnHandleType: 0){_LL1547:
_tmp60A=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp608)->f1;goto _LL1273;}
else{goto _LL1274;}_LL1274: _LL1553: _tmp610=_tmp4CE.f1;if((unsigned int)_tmp610 > 3?*((
int*)_tmp610)== Cyc_Absyn_JoinEff: 0){goto _LL1275;}else{goto _LL1276;}_LL1276:
_LL1555: _tmp612=_tmp4CE.f2;if((unsigned int)_tmp612 > 3?*((int*)_tmp612)== Cyc_Absyn_JoinEff:
0){goto _LL1277;}else{goto _LL1278;}_LL1278: _LL1557: _tmp614=_tmp4CE.f1;if((
unsigned int)_tmp614 > 3?*((int*)_tmp614)== Cyc_Absyn_AccessEff: 0){goto _LL1279;}
else{goto _LL1280;}_LL1280: _LL1559: _tmp616=_tmp4CE.f1;if((unsigned int)_tmp616 > 3?*((
int*)_tmp616)== Cyc_Absyn_RgnsEff: 0){goto _LL1281;}else{goto _LL1282;}_LL1282:
_LL1561: _tmp618=_tmp4CE.f2;if((unsigned int)_tmp618 > 3?*((int*)_tmp618)== Cyc_Absyn_RgnsEff:
0){goto _LL1283;}else{goto _LL1284;}_LL1284: _LL1563: _tmp61A=_tmp4CE.f2;if((
unsigned int)_tmp61A > 3?*((int*)_tmp61A)== Cyc_Absyn_AccessEff: 0){goto _LL1285;}
else{goto _LL1286;}_LL1286: goto _LL1287;_LL1233: Cyc_Tcutil_unify_it(t2,t1);return;
_LL1235: return;_LL1237: {struct _tagged_arr*_tmp61C=_tmp514->name;struct
_tagged_arr*_tmp61D=_tmp510->name;int _tmp61E=*((int*)_check_null(_tmp514->identity));
int _tmp61F=*((int*)_check_null(_tmp510->identity));void*_tmp620=Cyc_Tcutil_tvar_kind(
_tmp514);void*_tmp621=Cyc_Tcutil_tvar_kind(_tmp510);if(_tmp61F == _tmp61E? Cyc_Std_zstrptrcmp(
_tmp61C,_tmp61D)== 0: 0){if(_tmp620 != _tmp621){({struct Cyc_Std_String_pa_struct
_tmp625;_tmp625.tag=Cyc_Std_String_pa;_tmp625.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp621);{struct Cyc_Std_String_pa_struct _tmp624;_tmp624.tag=Cyc_Std_String_pa;
_tmp624.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp620);{struct Cyc_Std_String_pa_struct
_tmp623;_tmp623.tag=Cyc_Std_String_pa;_tmp623.f1=(struct _tagged_arr)*_tmp61C;{
void*_tmp622[3]={& _tmp623,& _tmp624,& _tmp625};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("same type variable %s has kinds %s and %s",
sizeof(unsigned char),42),_tag_arr(_tmp622,sizeof(void*),3));}}}});}return;}Cyc_Tcutil_failure_reason=
_tag_arr("(variable types are not the same)",sizeof(unsigned char),34);goto
_LL1231;}_LL1239: if(((_tmp51C != 0? _tmp524 != 0: 0)? Cyc_Absyn_qvar_cmp((struct
_tuple1*)_check_null(_tmp51C),(struct _tuple1*)_check_null(_tmp524))== 0: 0)? 1:(
_tmp51C == 0? _tmp524 == 0: 0)){Cyc_Tcutil_unify_list(_tmp51A,_tmp522);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different struct types)",sizeof(unsigned char),25);goto _LL1231;
_LL1241: if(Cyc_Absyn_qvar_cmp(_tmp52C,_tmp528)== 0){return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different enum types)",sizeof(unsigned char),23);goto _LL1231;_LL1243: {
int bad=0;for(0;_tmp534 != 0? _tmp530 != 0: 0;(_tmp534=_tmp534->tl,_tmp530=_tmp530->tl)){
struct Cyc_Absyn_Enumfield*_tmp626=(struct Cyc_Absyn_Enumfield*)_tmp534->hd;struct
Cyc_Absyn_Enumfield*_tmp627=(struct Cyc_Absyn_Enumfield*)_tmp530->hd;if(Cyc_Absyn_qvar_cmp(
_tmp626->name,_tmp627->name)!= 0){Cyc_Tcutil_failure_reason=_tag_arr("(different names for enum fields)",
sizeof(unsigned char),34);bad=1;break;}if(_tmp626->tag == _tmp627->tag){continue;}
if(_tmp626->tag == 0? 1: _tmp627->tag == 0){Cyc_Tcutil_failure_reason=_tag_arr("(different tag values for enum fields)",
sizeof(unsigned char),39);bad=1;break;}if(Cyc_Evexp_eval_const_uint_exp((struct
Cyc_Absyn_Exp*)_check_null(_tmp626->tag))!= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp627->tag))){Cyc_Tcutil_failure_reason=
_tag_arr("(different tag values for enum fields)",sizeof(unsigned char),39);bad=
1;break;}}if(bad){goto _LL1231;}if(_tmp534 == 0? _tmp530 == 0: 0){return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different number of fields for enums)",sizeof(unsigned char),39);goto
_LL1231;}_LL1245: if(((_tmp53C != 0? _tmp544 != 0: 0)? Cyc_Absyn_qvar_cmp((struct
_tuple1*)_check_null(_tmp53C),(struct _tuple1*)_check_null(_tmp544))== 0: 0)? 1:(
_tmp53C == 0? _tmp544 == 0: 0)){Cyc_Tcutil_unify_list(_tmp53A,_tmp542);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different union types)",sizeof(unsigned char),24);goto _LL1231;_LL1247:
if(_tmp55F == _tmp552? 1: Cyc_Absyn_qvar_cmp(_tmp55F->name,_tmp552->name)== 0){Cyc_Tcutil_unify_it(
_tmp54A,_tmp557);Cyc_Tcutil_unify_list(_tmp54C,_tmp559);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion types)",sizeof(unsigned char),25);goto _LL1231;
_LL1249: if((_tmp576 == _tmp56A? 1: Cyc_Absyn_qvar_cmp(_tmp576->name,_tmp56A->name)
== 0)? _tmp574 == _tmp568? 1: Cyc_Absyn_qvar_cmp(_tmp574->name,_tmp568->name)== 0: 0){
Cyc_Tcutil_unify_list(_tmp564,_tmp570);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion field types)",sizeof(unsigned char),31);goto _LL1231;
_LL1251: Cyc_Tcutil_unify_it(_tmp584,_tmp592);Cyc_Tcutil_unify_it(_tmp590,_tmp582);
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_unify_tqual(_tmp57E,
_tmp58C);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_boundscmp,_tmp57C,_tmp58A,
_tag_arr("(different pointer bounds)",sizeof(unsigned char),27));{void*_tmp628=(
void*)(Cyc_Absyn_compress_conref(_tmp57C))->v;void*_tmp62E;_LL1578: if((
unsigned int)_tmp628 > 1?*((int*)_tmp628)== Cyc_Absyn_Eq_constr: 0){_LL1583:
_tmp62E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp628)->f1;if(_tmp62E == (
void*)Cyc_Absyn_Unknown_b){goto _LL1579;}else{goto _LL1580;}}else{goto _LL1580;}
_LL1580: goto _LL1581;_LL1579: return;_LL1581: goto _LL1577;_LL1577:;}((void(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp580,_tmp58E,
_tag_arr("(different pointer types)",sizeof(unsigned char),26));return;_LL1253:
if(_tmp598 == _tmp59E? _tmp596 == _tmp59C: 0){return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different integral types)",sizeof(unsigned char),27);goto _LL1231;
_LL1255: return;_LL1257: if(_tmp5AA == _tmp5A6){return;}goto _LL1231;_LL1259: Cyc_Tcutil_unify_it(
_tmp5B2,_tmp5AE);return;_LL1261: Cyc_Tcutil_unify_tqual(_tmp5B8,_tmp5C0);Cyc_Tcutil_unify_it(
_tmp5BA,_tmp5C2);if(_tmp5BE == _tmp5B6){return;}if(_tmp5BE == 0? 1: _tmp5B6 == 0){
goto _LL1231;}if(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp5BE))== Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp5B6))){return;}Cyc_Tcutil_failure_reason=_tag_arr("(different array sizes)",
sizeof(unsigned char),24);goto _LL1231;_LL1263: {int done=0;{struct _RegionHandle
_tmp630=_new_region("rgn");struct _RegionHandle*rgn=& _tmp630;_push_region(rgn);{
struct Cyc_List_List*inst=0;while(_tmp5D6 != 0){if(_tmp5EA == 0){Cyc_Tcutil_failure_reason=
_tag_arr("(second function type has too few type variables)",sizeof(
unsigned char),50);(int)_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp631=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp5D6->hd);void*_tmp632=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp5EA->hd);if(_tmp631 != _tmp632){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp636;_tmp636.tag=Cyc_Std_String_pa;_tmp636.f1=(
struct _tagged_arr)Cyc_Absynpp_kind2string(_tmp632);{struct Cyc_Std_String_pa_struct
_tmp635;_tmp635.tag=Cyc_Std_String_pa;_tmp635.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp631);{struct Cyc_Std_String_pa_struct _tmp634;_tmp634.tag=Cyc_Std_String_pa;
_tmp634.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
_tmp5D6->hd);{void*_tmp633[3]={& _tmp634,& _tmp635,& _tmp636};Cyc_Std_aprintf(
_tag_arr("(type var %s has different kinds %s and %s)",sizeof(unsigned char),44),
_tag_arr(_tmp633,sizeof(void*),3));}}}});(int)_throw((void*)Cyc_Tcutil_Unify);}
inst=({struct Cyc_List_List*_tmp637=_region_malloc(rgn,sizeof(struct Cyc_List_List));
_tmp637->hd=({struct _tuple5*_tmp638=_region_malloc(rgn,sizeof(struct _tuple5));
_tmp638->f1=(struct Cyc_Absyn_Tvar*)_tmp5EA->hd;_tmp638->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp639=_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));_tmp639[0]=({struct Cyc_Absyn_VarType_struct
_tmp63A;_tmp63A.tag=Cyc_Absyn_VarType;_tmp63A.f1=(struct Cyc_Absyn_Tvar*)_tmp5D6->hd;
_tmp63A;});_tmp639;});_tmp638;});_tmp637->tl=inst;_tmp637;});_tmp5D6=_tmp5D6->tl;
_tmp5EA=_tmp5EA->tl;}}if(_tmp5EA != 0){Cyc_Tcutil_failure_reason=_tag_arr("(second function type has too many type variables)",
sizeof(unsigned char),51);_npop_handler(0);goto _LL1231;}if(inst != 0){Cyc_Tcutil_unify_it((
void*)({struct Cyc_Absyn_FnType_struct*_tmp63B=_cycalloc(sizeof(struct Cyc_Absyn_FnType_struct));
_tmp63B[0]=({struct Cyc_Absyn_FnType_struct _tmp63C;_tmp63C.tag=Cyc_Absyn_FnType;
_tmp63C.f1=({struct Cyc_Absyn_FnInfo _tmp63D;_tmp63D.tvars=0;_tmp63D.effect=
_tmp5D4;_tmp63D.ret_typ=(void*)_tmp5D2;_tmp63D.args=_tmp5D0;_tmp63D.c_varargs=
_tmp5CE;_tmp63D.cyc_varargs=_tmp5CC;_tmp63D.rgn_po=_tmp5CA;_tmp63D.attributes=
_tmp5C8;_tmp63D;});_tmp63C;});_tmp63B;}),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({
struct Cyc_Absyn_FnType_struct*_tmp63E=_cycalloc(sizeof(struct Cyc_Absyn_FnType_struct));
_tmp63E[0]=({struct Cyc_Absyn_FnType_struct _tmp63F;_tmp63F.tag=Cyc_Absyn_FnType;
_tmp63F.f1=({struct Cyc_Absyn_FnInfo _tmp640;_tmp640.tvars=0;_tmp640.effect=
_tmp5E8;_tmp640.ret_typ=(void*)_tmp5E6;_tmp640.args=_tmp5E4;_tmp640.c_varargs=
_tmp5E2;_tmp640.cyc_varargs=_tmp5E0;_tmp640.rgn_po=_tmp5DE;_tmp640.attributes=
_tmp5DC;_tmp640;});_tmp63F;});_tmp63E;})));done=1;}};_pop_region(rgn);}if(done){
return;}Cyc_Tcutil_unify_it(_tmp5D2,_tmp5E6);for(0;_tmp5D0 != 0? _tmp5E4 != 0: 0;(
_tmp5D0=_tmp5D0->tl,_tmp5E4=_tmp5E4->tl)){Cyc_Tcutil_unify_tqual((*((struct
_tuple2*)_tmp5D0->hd)).f2,(*((struct _tuple2*)_tmp5E4->hd)).f2);Cyc_Tcutil_unify_it((*((
struct _tuple2*)_tmp5D0->hd)).f3,(*((struct _tuple2*)_tmp5E4->hd)).f3);}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp5D0 != 0? 1: _tmp5E4 != 0){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different number of arguments)",sizeof(
unsigned char),52);goto _LL1231;}if(_tmp5CE != _tmp5E2){Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type takes C varargs)",sizeof(unsigned char),41);
goto _LL1231;}{int bad_cyc_vararg=0;{struct _tuple11 _tmp642=({struct _tuple11 _tmp641;
_tmp641.f1=_tmp5CC;_tmp641.f2=_tmp5E0;_tmp641;});struct Cyc_Absyn_VarargInfo*
_tmp64C;struct Cyc_Absyn_VarargInfo*_tmp64E;struct Cyc_Absyn_VarargInfo*_tmp650;
struct Cyc_Absyn_VarargInfo*_tmp652;struct Cyc_Absyn_VarargInfo*_tmp654;struct Cyc_Absyn_VarargInfo
_tmp656;int _tmp657;void*_tmp659;struct Cyc_Absyn_Tqual _tmp65B;struct Cyc_Core_Opt*
_tmp65D;struct Cyc_Absyn_VarargInfo*_tmp65F;struct Cyc_Absyn_VarargInfo _tmp661;int
_tmp662;void*_tmp664;struct Cyc_Absyn_Tqual _tmp666;struct Cyc_Core_Opt*_tmp668;
_LL1604: _LL1615: _tmp64E=_tmp642.f1;if(_tmp64E == 0){goto _LL1613;}else{goto _LL1606;}
_LL1613: _tmp64C=_tmp642.f2;if(_tmp64C == 0){goto _LL1605;}else{goto _LL1606;}
_LL1606: _LL1617: _tmp650=_tmp642.f1;if(_tmp650 == 0){goto _LL1607;}else{goto _LL1608;}
_LL1608: _LL1619: _tmp652=_tmp642.f2;if(_tmp652 == 0){goto _LL1609;}else{goto _LL1610;}
_LL1610: _LL1632: _tmp65F=_tmp642.f1;if(_tmp65F == 0){goto _LL1603;}else{_tmp661=*
_tmp65F;_LL1641: _tmp668=_tmp661.name;goto _LL1639;_LL1639: _tmp666=_tmp661.tq;goto
_LL1637;_LL1637: _tmp664=(void*)_tmp661.type;goto _LL1635;_LL1635: _tmp662=_tmp661.inject;
goto _LL1621;}_LL1621: _tmp654=_tmp642.f2;if(_tmp654 == 0){goto _LL1603;}else{
_tmp656=*_tmp654;_LL1630: _tmp65D=_tmp656.name;goto _LL1628;_LL1628: _tmp65B=
_tmp656.tq;goto _LL1626;_LL1626: _tmp659=(void*)_tmp656.type;goto _LL1624;_LL1624:
_tmp657=_tmp656.inject;goto _LL1611;}_LL1605: goto _LL1603;_LL1607: goto _LL1609;
_LL1609: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=_tag_arr("(only one function type takes varargs)",
sizeof(unsigned char),39);goto _LL1603;_LL1611: Cyc_Tcutil_unify_tqual(_tmp666,
_tmp65B);Cyc_Tcutil_unify_it(_tmp664,_tmp659);if(_tmp662 != _tmp657){
bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=_tag_arr("(only one function type injects varargs)",
sizeof(unsigned char),41);}goto _LL1603;_LL1603:;}if(bad_cyc_vararg){goto _LL1231;}{
int bad_effect=0;{struct _tuple12 _tmp66B=({struct _tuple12 _tmp66A;_tmp66A.f1=
_tmp5D4;_tmp66A.f2=_tmp5E8;_tmp66A;});struct Cyc_Core_Opt*_tmp675;struct Cyc_Core_Opt*
_tmp677;struct Cyc_Core_Opt*_tmp679;struct Cyc_Core_Opt*_tmp67B;_LL1645: _LL1656:
_tmp677=_tmp66B.f1;if(_tmp677 == 0){goto _LL1654;}else{goto _LL1647;}_LL1654:
_tmp675=_tmp66B.f2;if(_tmp675 == 0){goto _LL1646;}else{goto _LL1647;}_LL1647:
_LL1658: _tmp679=_tmp66B.f1;if(_tmp679 == 0){goto _LL1648;}else{goto _LL1649;}
_LL1649: _LL1660: _tmp67B=_tmp66B.f2;if(_tmp67B == 0){goto _LL1650;}else{goto _LL1651;}
_LL1651: goto _LL1652;_LL1646: goto _LL1644;_LL1648: goto _LL1650;_LL1650: bad_effect=1;
goto _LL1644;_LL1652: bad_effect=! Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp5D4))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5E8))->v);
goto _LL1644;_LL1644:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){Cyc_Tcutil_failure_reason=_tag_arr("(function type effects do not unify)",
sizeof(unsigned char),37);goto _LL1231;}if(! Cyc_Tcutil_same_atts(_tmp5DC,_tmp5C8)){
Cyc_Tcutil_failure_reason=_tag_arr("(function types have different attributes)",
sizeof(unsigned char),43);goto _LL1231;}if(! Cyc_Tcutil_same_rgn_po(_tmp5DE,
_tmp5CA)){Cyc_Tcutil_failure_reason=_tag_arr("(function types have different region lifetime orderings)",
sizeof(unsigned char),58);goto _LL1231;}return;}}}_LL1265: for(0;_tmp5EE != 0?
_tmp5F2 != 0: 0;(_tmp5EE=_tmp5EE->tl,_tmp5F2=_tmp5F2->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)_tmp5EE->hd)).f1,(*((struct _tuple4*)_tmp5F2->hd)).f1);Cyc_Tcutil_unify_it((*((
struct _tuple4*)_tmp5EE->hd)).f2,(*((struct _tuple4*)_tmp5F2->hd)).f2);}if(_tmp5EE
== 0? _tmp5F2 == 0: 0){return;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_arr("(tuple types have different numbers of components)",
sizeof(unsigned char),51);goto _LL1231;_LL1267: _tmp602=_tmp5FA;_tmp5FE=_tmp5F6;
goto _LL1269;_LL1269: for(0;_tmp5FE != 0? _tmp602 != 0: 0;(_tmp5FE=_tmp5FE->tl,_tmp602=
_tmp602->tl)){struct Cyc_Absyn_Structfield*_tmp67D=(struct Cyc_Absyn_Structfield*)
_tmp5FE->hd;struct Cyc_Absyn_Structfield*_tmp67E=(struct Cyc_Absyn_Structfield*)
_tmp602->hd;if(Cyc_Std_zstrptrcmp(_tmp67D->name,_tmp67E->name)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different member names in anonymous structs/unions)",sizeof(
unsigned char),53);(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_tqual(
_tmp67D->tq,_tmp67E->tq);Cyc_Tcutil_unify_it((void*)_tmp67D->type,(void*)_tmp67E->type);
if(! Cyc_Tcutil_same_atts(_tmp67D->attributes,_tmp67E->attributes)){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=_tag_arr("(different attributes on member)",
sizeof(unsigned char),33);(int)_throw((void*)Cyc_Tcutil_Unify);}if(((_tmp67D->width
!= 0? _tmp67E->width == 0: 0)? 1:(_tmp67E->width != 0? _tmp67D->width == 0: 0))? 1:((
_tmp67D->width != 0? _tmp67E->width != 0: 0)? Cyc_Evexp_eval_const_uint_exp((struct
Cyc_Absyn_Exp*)_check_null(_tmp67D->width))!= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp67E->width)): 0)){Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=_tag_arr("(different bitfield widths on member)",
sizeof(unsigned char),38);(int)_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp5FE == 0?
_tmp602 == 0: 0){return;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different number of members)",sizeof(unsigned char),30);goto _LL1231;
_LL1271: return;_LL1273: Cyc_Tcutil_unify_it(_tmp60E,_tmp60A);return;_LL1275: goto
_LL1277;_LL1277: goto _LL1279;_LL1279: goto _LL1281;_LL1281: goto _LL1283;_LL1283: goto
_LL1285;_LL1285: if(Cyc_Tcutil_unify_effect(t1,t2)){return;}Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)",sizeof(unsigned char),22);goto _LL1231;_LL1287:
goto _LL1231;_LL1231:;}(int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_opt_cmp(
int(*cmp)(void*,void*),struct Cyc_Core_Opt*a1,struct Cyc_Core_Opt*a2){if(a1 == a2){
return 0;}if(a1 == 0? a2 != 0: 0){return - 1;}if(a1 != 0? a2 == 0: 0){return 1;}return cmp((
void*)((struct Cyc_Core_Opt*)_check_null(a1))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(a2))->v);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*
a2){if(a1 == a2){return 0;}if(a1 == 0? a2 != 0: 0){return - 1;}if(a1 != 0? a2 == 0: 0){
return 1;}return cmp((void*)_check_null(a1),(void*)_check_null(a2));}int Cyc_Tcutil_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2){
return 0;}for(0;l1 != 0? l2 != 0: 0;(l1=l1->tl,l2=l2->tl)){int _tmp67F=cmp((void*)l1->hd,(
void*)l2->hd);if(_tmp67F != 0){return _tmp67F;}}if(l2 != 0){return - 1;}if(l1 != 0){
return 1;}return 0;}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct
Cyc_Absyn_Tqual tq2){int _tmp680=(tq1.q_const + (tq1.q_volatile << 1))+ (tq1.q_restrict
<< 2);int _tmp681=(tq2.q_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp680,_tmp681);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(
void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y){return 0;}{void*_tmp682=(void*)x->v;
void*_tmp68A;_LL1668: if(_tmp682 == (void*)Cyc_Absyn_No_constr){goto _LL1669;}else{
goto _LL1670;}_LL1670: if((unsigned int)_tmp682 > 1?*((int*)_tmp682)== Cyc_Absyn_Forward_constr:
0){goto _LL1671;}else{goto _LL1672;}_LL1672: if((unsigned int)_tmp682 > 1?*((int*)
_tmp682)== Cyc_Absyn_Eq_constr: 0){_LL1675: _tmp68A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp682)->f1;goto _LL1673;}else{goto _LL1667;}_LL1669: return - 1;_LL1671:(int)_throw(({
void*_tmp68C[0]={};Cyc_Tcutil_impos(_tag_arr("unify_conref: forward after compress",
sizeof(unsigned char),37),_tag_arr(_tmp68C,sizeof(void*),0));}));_LL1673: {void*
_tmp68D=(void*)y->v;void*_tmp695;_LL1679: if(_tmp68D == (void*)Cyc_Absyn_No_constr){
goto _LL1680;}else{goto _LL1681;}_LL1681: if((unsigned int)_tmp68D > 1?*((int*)
_tmp68D)== Cyc_Absyn_Forward_constr: 0){goto _LL1682;}else{goto _LL1683;}_LL1683:
if((unsigned int)_tmp68D > 1?*((int*)_tmp68D)== Cyc_Absyn_Eq_constr: 0){_LL1686:
_tmp695=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp68D)->f1;goto _LL1684;}
else{goto _LL1678;}_LL1680: return 1;_LL1682:(int)_throw(({void*_tmp697[0]={};Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress(2)",sizeof(unsigned char),40),
_tag_arr(_tmp697,sizeof(void*),0));}));_LL1684: return cmp(_tmp68A,_tmp695);
_LL1678:;}_LL1667:;}}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple4*tqt1,
struct _tuple4*tqt2){struct _tuple4 _tmp69A;void*_tmp69B;struct Cyc_Absyn_Tqual
_tmp69D;struct _tuple4*_tmp698=tqt1;_tmp69A=*_tmp698;_LL1694: _tmp69D=_tmp69A.f1;
goto _LL1692;_LL1692: _tmp69B=_tmp69A.f2;goto _LL1689;_LL1689: {struct _tuple4
_tmp6A1;void*_tmp6A2;struct Cyc_Absyn_Tqual _tmp6A4;struct _tuple4*_tmp69F=tqt2;
_tmp6A1=*_tmp69F;_LL1701: _tmp6A4=_tmp6A1.f1;goto _LL1699;_LL1699: _tmp6A2=_tmp6A1.f2;
goto _LL1696;_LL1696: {int _tmp6A6=Cyc_Tcutil_tqual_cmp(_tmp69D,_tmp6A4);if(
_tmp6A6 != 0){return _tmp6A6;}return Cyc_Tcutil_typecmp(_tmp69B,_tmp6A2);}}}static
int Cyc_Tcutil_const_uint_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Core_intcmp((int)Cyc_Evexp_eval_const_uint_exp(e1),(int)Cyc_Evexp_eval_const_uint_exp(
e2));}static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp6A7=att;
_LL1705: if((unsigned int)_tmp6A7 > 16?*((int*)_tmp6A7)== Cyc_Absyn_Regparm_att: 0){
goto _LL1706;}else{goto _LL1707;}_LL1707: if(_tmp6A7 == (void*)Cyc_Absyn_Stdcall_att){
goto _LL1708;}else{goto _LL1709;}_LL1709: if(_tmp6A7 == (void*)Cyc_Absyn_Cdecl_att){
goto _LL1710;}else{goto _LL1711;}_LL1711: if(_tmp6A7 == (void*)Cyc_Absyn_Fastcall_att){
goto _LL1712;}else{goto _LL1713;}_LL1713: if(_tmp6A7 == (void*)Cyc_Absyn_Noreturn_att){
goto _LL1714;}else{goto _LL1715;}_LL1715: if(_tmp6A7 == (void*)Cyc_Absyn_Const_att){
goto _LL1716;}else{goto _LL1717;}_LL1717: if((unsigned int)_tmp6A7 > 16?*((int*)
_tmp6A7)== Cyc_Absyn_Aligned_att: 0){goto _LL1718;}else{goto _LL1719;}_LL1719: if(
_tmp6A7 == (void*)Cyc_Absyn_Packed_att){goto _LL1720;}else{goto _LL1721;}_LL1721:
if((unsigned int)_tmp6A7 > 16?*((int*)_tmp6A7)== Cyc_Absyn_Section_att: 0){goto
_LL1722;}else{goto _LL1723;}_LL1723: if(_tmp6A7 == (void*)Cyc_Absyn_Nocommon_att){
goto _LL1724;}else{goto _LL1725;}_LL1725: if(_tmp6A7 == (void*)Cyc_Absyn_Shared_att){
goto _LL1726;}else{goto _LL1727;}_LL1727: if(_tmp6A7 == (void*)Cyc_Absyn_Unused_att){
goto _LL1728;}else{goto _LL1729;}_LL1729: if(_tmp6A7 == (void*)Cyc_Absyn_Weak_att){
goto _LL1730;}else{goto _LL1731;}_LL1731: if(_tmp6A7 == (void*)Cyc_Absyn_Dllimport_att){
goto _LL1732;}else{goto _LL1733;}_LL1733: if(_tmp6A7 == (void*)Cyc_Absyn_Dllexport_att){
goto _LL1734;}else{goto _LL1735;}_LL1735: if(_tmp6A7 == (void*)Cyc_Absyn_No_instrument_function_att){
goto _LL1736;}else{goto _LL1737;}_LL1737: if(_tmp6A7 == (void*)Cyc_Absyn_Constructor_att){
goto _LL1738;}else{goto _LL1739;}_LL1739: if(_tmp6A7 == (void*)Cyc_Absyn_Destructor_att){
goto _LL1740;}else{goto _LL1741;}_LL1741: if(_tmp6A7 == (void*)Cyc_Absyn_No_check_memory_usage_att){
goto _LL1742;}else{goto _LL1743;}_LL1743: if((unsigned int)_tmp6A7 > 16?*((int*)
_tmp6A7)== Cyc_Absyn_Format_att: 0){goto _LL1744;}else{goto _LL1704;}_LL1706: return
0;_LL1708: return 1;_LL1710: return 2;_LL1712: return 3;_LL1714: return 4;_LL1716: return
5;_LL1718: return 6;_LL1720: return 7;_LL1722: return 8;_LL1724: return 9;_LL1726: return
10;_LL1728: return 11;_LL1730: return 12;_LL1732: return 13;_LL1734: return 14;_LL1736:
return 15;_LL1738: return 16;_LL1740: return 17;_LL1742: return 18;_LL1744: return 19;
_LL1704:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){struct _tuple8
_tmp6D2=({struct _tuple8 _tmp6D1;_tmp6D1.f1=att1;_tmp6D1.f2=att2;_tmp6D1;});void*
_tmp6DE;int _tmp6E0;void*_tmp6E2;int _tmp6E4;void*_tmp6E6;int _tmp6E8;void*_tmp6EA;
int _tmp6EC;void*_tmp6EE;struct _tagged_arr _tmp6F0;void*_tmp6F2;struct _tagged_arr
_tmp6F4;void*_tmp6F6;int _tmp6F8;int _tmp6FA;void*_tmp6FC;void*_tmp6FE;int _tmp700;
int _tmp702;void*_tmp704;_LL1748: _LL1763: _tmp6E2=_tmp6D2.f1;if((unsigned int)
_tmp6E2 > 16?*((int*)_tmp6E2)== Cyc_Absyn_Regparm_att: 0){_LL1765: _tmp6E4=((struct
Cyc_Absyn_Regparm_att_struct*)_tmp6E2)->f1;goto _LL1759;}else{goto _LL1750;}
_LL1759: _tmp6DE=_tmp6D2.f2;if((unsigned int)_tmp6DE > 16?*((int*)_tmp6DE)== Cyc_Absyn_Regparm_att:
0){_LL1761: _tmp6E0=((struct Cyc_Absyn_Regparm_att_struct*)_tmp6DE)->f1;goto
_LL1749;}else{goto _LL1750;}_LL1750: _LL1771: _tmp6EA=_tmp6D2.f1;if((unsigned int)
_tmp6EA > 16?*((int*)_tmp6EA)== Cyc_Absyn_Aligned_att: 0){_LL1773: _tmp6EC=((struct
Cyc_Absyn_Aligned_att_struct*)_tmp6EA)->f1;goto _LL1767;}else{goto _LL1752;}
_LL1767: _tmp6E6=_tmp6D2.f2;if((unsigned int)_tmp6E6 > 16?*((int*)_tmp6E6)== Cyc_Absyn_Aligned_att:
0){_LL1769: _tmp6E8=((struct Cyc_Absyn_Aligned_att_struct*)_tmp6E6)->f1;goto
_LL1751;}else{goto _LL1752;}_LL1752: _LL1779: _tmp6F2=_tmp6D2.f1;if((unsigned int)
_tmp6F2 > 16?*((int*)_tmp6F2)== Cyc_Absyn_Section_att: 0){_LL1781: _tmp6F4=((struct
Cyc_Absyn_Section_att_struct*)_tmp6F2)->f1;goto _LL1775;}else{goto _LL1754;}
_LL1775: _tmp6EE=_tmp6D2.f2;if((unsigned int)_tmp6EE > 16?*((int*)_tmp6EE)== Cyc_Absyn_Section_att:
0){_LL1777: _tmp6F0=((struct Cyc_Absyn_Section_att_struct*)_tmp6EE)->f1;goto
_LL1753;}else{goto _LL1754;}_LL1754: _LL1791: _tmp6FE=_tmp6D2.f1;if((unsigned int)
_tmp6FE > 16?*((int*)_tmp6FE)== Cyc_Absyn_Format_att: 0){_LL1797: _tmp704=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp6FE)->f1;goto _LL1795;_LL1795: _tmp702=((
struct Cyc_Absyn_Format_att_struct*)_tmp6FE)->f2;goto _LL1793;_LL1793: _tmp700=((
struct Cyc_Absyn_Format_att_struct*)_tmp6FE)->f3;goto _LL1783;}else{goto _LL1756;}
_LL1783: _tmp6F6=_tmp6D2.f2;if((unsigned int)_tmp6F6 > 16?*((int*)_tmp6F6)== Cyc_Absyn_Format_att:
0){_LL1789: _tmp6FC=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp6F6)->f1;goto
_LL1787;_LL1787: _tmp6FA=((struct Cyc_Absyn_Format_att_struct*)_tmp6F6)->f2;goto
_LL1785;_LL1785: _tmp6F8=((struct Cyc_Absyn_Format_att_struct*)_tmp6F6)->f3;goto
_LL1755;}else{goto _LL1756;}_LL1756: goto _LL1757;_LL1749: _tmp6EC=_tmp6E4;_tmp6E8=
_tmp6E0;goto _LL1751;_LL1751: return Cyc_Core_intcmp(_tmp6EC,_tmp6E8);_LL1753:
return Cyc_Std_strcmp(_tmp6F4,_tmp6F0);_LL1755: {int _tmp706=Cyc_Core_intcmp((int)((
unsigned int)_tmp704),(int)((unsigned int)_tmp6FC));if(_tmp706 != 0){return
_tmp706;}{int _tmp707=Cyc_Core_intcmp(_tmp702,_tmp6FA);if(_tmp707 != 0){return
_tmp707;}return Cyc_Core_intcmp(_tmp700,_tmp6F8);}}_LL1757: {int _tmp708=Cyc_Tcutil_attribute_case_number(
att1);int _tmp709=Cyc_Tcutil_attribute_case_number(att2);return Cyc_Core_intcmp(
_tmp708,_tmp709);}_LL1747:;}static int Cyc_Tcutil_structfield_cmp(struct Cyc_Absyn_Structfield*
f1,struct Cyc_Absyn_Structfield*f2){int _tmp70A=Cyc_Std_zstrptrcmp(f1->name,f2->name);
if(_tmp70A != 0){return _tmp70A;}{int _tmp70B=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp70B != 0){return _tmp70B;}{int _tmp70C=Cyc_Tcutil_typecmp((void*)f1->type,(
void*)f2->type);if(_tmp70C != 0){return _tmp70C;}{int _tmp70D=Cyc_Tcutil_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp70D != 0){return
_tmp70D;}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp70E=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp70E != 0){return _tmp70E;}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Tcutil_const_uint_exp_cmp,e1->tag,e2->tag);}extern int Cyc_Tcutil_typecmp(void*
t1,void*t2);int Cyc_Tcutil_type_case_number(void*t){void*_tmp70F=t;_LL1809: if(
_tmp70F == (void*)Cyc_Absyn_VoidType){goto _LL1810;}else{goto _LL1811;}_LL1811: if((
unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_Evar: 0){goto _LL1812;}else{
goto _LL1813;}_LL1813: if((unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_VarType:
0){goto _LL1814;}else{goto _LL1815;}_LL1815: if((unsigned int)_tmp70F > 3?*((int*)
_tmp70F)== Cyc_Absyn_TunionType: 0){goto _LL1816;}else{goto _LL1817;}_LL1817: if((
unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_TunionFieldType: 0){goto
_LL1818;}else{goto _LL1819;}_LL1819: if((unsigned int)_tmp70F > 3?*((int*)_tmp70F)
== Cyc_Absyn_PointerType: 0){goto _LL1820;}else{goto _LL1821;}_LL1821: if((
unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_IntType: 0){goto _LL1822;}
else{goto _LL1823;}_LL1823: if(_tmp70F == (void*)Cyc_Absyn_FloatType){goto _LL1824;}
else{goto _LL1825;}_LL1825: if((unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_DoubleType:
0){goto _LL1826;}else{goto _LL1827;}_LL1827: if((unsigned int)_tmp70F > 3?*((int*)
_tmp70F)== Cyc_Absyn_ArrayType: 0){goto _LL1828;}else{goto _LL1829;}_LL1829: if((
unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_FnType: 0){goto _LL1830;}
else{goto _LL1831;}_LL1831: if((unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_TupleType:
0){goto _LL1832;}else{goto _LL1833;}_LL1833: if((unsigned int)_tmp70F > 3?*((int*)
_tmp70F)== Cyc_Absyn_StructType: 0){goto _LL1834;}else{goto _LL1835;}_LL1835: if((
unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_UnionType: 0){goto _LL1836;}
else{goto _LL1837;}_LL1837: if((unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_AnonStructType:
0){goto _LL1838;}else{goto _LL1839;}_LL1839: if((unsigned int)_tmp70F > 3?*((int*)
_tmp70F)== Cyc_Absyn_AnonUnionType: 0){goto _LL1840;}else{goto _LL1841;}_LL1841: if((
unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_EnumType: 0){goto _LL1842;}
else{goto _LL1843;}_LL1843: if((unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_AnonEnumType:
0){goto _LL1844;}else{goto _LL1845;}_LL1845: if((unsigned int)_tmp70F > 3?*((int*)
_tmp70F)== Cyc_Absyn_RgnHandleType: 0){goto _LL1846;}else{goto _LL1847;}_LL1847: if((
unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_TypedefType: 0){goto _LL1848;}
else{goto _LL1849;}_LL1849: if(_tmp70F == (void*)Cyc_Absyn_HeapRgn){goto _LL1850;}
else{goto _LL1851;}_LL1851: if((unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_AccessEff:
0){goto _LL1852;}else{goto _LL1853;}_LL1853: if((unsigned int)_tmp70F > 3?*((int*)
_tmp70F)== Cyc_Absyn_JoinEff: 0){goto _LL1854;}else{goto _LL1855;}_LL1855: if((
unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_RgnsEff: 0){goto _LL1856;}
else{goto _LL1857;}_LL1857: if((unsigned int)_tmp70F > 3?*((int*)_tmp70F)== Cyc_Absyn_SizeofType:
0){goto _LL1858;}else{goto _LL1808;}_LL1810: return 0;_LL1812: return 1;_LL1814: return
2;_LL1816: return 3;_LL1818: return 4;_LL1820: return 5;_LL1822: return 6;_LL1824: return
7;_LL1826: return 8;_LL1828: return 9;_LL1830: return 10;_LL1832: return 11;_LL1834:
return 12;_LL1836: return 13;_LL1838: return 14;_LL1840: return 15;_LL1842: return 16;
_LL1844: return 17;_LL1846: return 18;_LL1848: return 19;_LL1850: return 20;_LL1852:
return 21;_LL1854: return 22;_LL1856: return 23;_LL1858: return 24;_LL1808:;}int Cyc_Tcutil_shallow_typecmp_it(
void*t1,void*t2){return Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2){return 0;}{int _tmp743=Cyc_Tcutil_shallow_typecmp_it(t1,t2);if(
_tmp743 != 0){return _tmp743;}{struct _tuple8 _tmp745=({struct _tuple8 _tmp744;_tmp744.f1=
t2;_tmp744.f2=t1;_tmp744;});void*_tmp77F;void*_tmp781;void*_tmp783;void*_tmp785;
void*_tmp787;struct Cyc_Absyn_Tvar*_tmp789;void*_tmp78B;struct Cyc_Absyn_Tvar*
_tmp78D;void*_tmp78F;struct Cyc_List_List*_tmp791;struct _tuple1*_tmp793;void*
_tmp795;struct Cyc_List_List*_tmp797;struct _tuple1*_tmp799;void*_tmp79B;struct
_tuple1*_tmp79D;void*_tmp79F;struct _tuple1*_tmp7A1;void*_tmp7A3;struct Cyc_List_List*
_tmp7A5;void*_tmp7A7;struct Cyc_List_List*_tmp7A9;void*_tmp7AB;struct Cyc_Absyn_Uniondecl**
_tmp7AD;struct Cyc_List_List*_tmp7AF;struct _tuple1*_tmp7B1;void*_tmp7B3;struct Cyc_Absyn_Uniondecl**
_tmp7B5;struct Cyc_List_List*_tmp7B7;struct _tuple1*_tmp7B9;void*_tmp7BB;struct Cyc_Absyn_TunionInfo
_tmp7BD;void*_tmp7BF;struct Cyc_List_List*_tmp7C1;void*_tmp7C3;struct Cyc_Absyn_Tuniondecl**
_tmp7C5;struct Cyc_Absyn_Tuniondecl*_tmp7C7;void*_tmp7C8;struct Cyc_Absyn_TunionInfo
_tmp7CA;void*_tmp7CC;struct Cyc_List_List*_tmp7CE;void*_tmp7D0;struct Cyc_Absyn_Tuniondecl**
_tmp7D2;struct Cyc_Absyn_Tuniondecl*_tmp7D4;void*_tmp7D5;struct Cyc_Absyn_TunionFieldInfo
_tmp7D7;struct Cyc_List_List*_tmp7D9;void*_tmp7DB;struct Cyc_Absyn_Tunionfield*
_tmp7DD;struct Cyc_Absyn_Tuniondecl*_tmp7DF;void*_tmp7E1;struct Cyc_Absyn_TunionFieldInfo
_tmp7E3;struct Cyc_List_List*_tmp7E5;void*_tmp7E7;struct Cyc_Absyn_Tunionfield*
_tmp7E9;struct Cyc_Absyn_Tuniondecl*_tmp7EB;void*_tmp7ED;struct Cyc_Absyn_PtrInfo
_tmp7EF;struct Cyc_Absyn_Conref*_tmp7F1;struct Cyc_Absyn_Tqual _tmp7F3;struct Cyc_Absyn_Conref*
_tmp7F5;void*_tmp7F7;void*_tmp7F9;void*_tmp7FB;struct Cyc_Absyn_PtrInfo _tmp7FD;
struct Cyc_Absyn_Conref*_tmp7FF;struct Cyc_Absyn_Tqual _tmp801;struct Cyc_Absyn_Conref*
_tmp803;void*_tmp805;void*_tmp807;void*_tmp809;void*_tmp80B;void*_tmp80D;void*
_tmp80F;void*_tmp811;void*_tmp813;void*_tmp815;void*_tmp817;void*_tmp819;int
_tmp81B;void*_tmp81D;int _tmp81F;void*_tmp821;struct Cyc_Absyn_Exp*_tmp823;struct
Cyc_Absyn_Tqual _tmp825;void*_tmp827;void*_tmp829;struct Cyc_Absyn_Exp*_tmp82B;
struct Cyc_Absyn_Tqual _tmp82D;void*_tmp82F;void*_tmp831;struct Cyc_Absyn_FnInfo
_tmp833;struct Cyc_List_List*_tmp835;struct Cyc_List_List*_tmp837;struct Cyc_Absyn_VarargInfo*
_tmp839;int _tmp83B;struct Cyc_List_List*_tmp83D;void*_tmp83F;struct Cyc_Core_Opt*
_tmp841;struct Cyc_List_List*_tmp843;void*_tmp845;struct Cyc_Absyn_FnInfo _tmp847;
struct Cyc_List_List*_tmp849;struct Cyc_List_List*_tmp84B;struct Cyc_Absyn_VarargInfo*
_tmp84D;int _tmp84F;struct Cyc_List_List*_tmp851;void*_tmp853;struct Cyc_Core_Opt*
_tmp855;struct Cyc_List_List*_tmp857;void*_tmp859;struct Cyc_List_List*_tmp85B;
void*_tmp85D;struct Cyc_List_List*_tmp85F;void*_tmp861;struct Cyc_List_List*
_tmp863;void*_tmp865;struct Cyc_List_List*_tmp867;void*_tmp869;struct Cyc_List_List*
_tmp86B;void*_tmp86D;struct Cyc_List_List*_tmp86F;void*_tmp871;void*_tmp873;void*
_tmp875;void*_tmp877;void*_tmp879;void*_tmp87B;void*_tmp87D;void*_tmp87F;void*
_tmp881;void*_tmp883;void*_tmp885;void*_tmp887;void*_tmp889;void*_tmp88B;void*
_tmp88D;void*_tmp88F;_LL1863: _LL1922: _tmp781=_tmp745.f1;if((unsigned int)_tmp781
> 3?*((int*)_tmp781)== Cyc_Absyn_Evar: 0){goto _LL1920;}else{goto _LL1865;}_LL1920:
_tmp77F=_tmp745.f2;if((unsigned int)_tmp77F > 3?*((int*)_tmp77F)== Cyc_Absyn_Evar:
0){goto _LL1864;}else{goto _LL1865;}_LL1865: _LL1926: _tmp785=_tmp745.f1;if(_tmp785
== (void*)Cyc_Absyn_VoidType){goto _LL1924;}else{goto _LL1867;}_LL1924: _tmp783=
_tmp745.f2;if(_tmp783 == (void*)Cyc_Absyn_VoidType){goto _LL1866;}else{goto _LL1867;}
_LL1867: _LL1932: _tmp78B=_tmp745.f1;if((unsigned int)_tmp78B > 3?*((int*)_tmp78B)
== Cyc_Absyn_VarType: 0){_LL1934: _tmp78D=((struct Cyc_Absyn_VarType_struct*)
_tmp78B)->f1;goto _LL1928;}else{goto _LL1869;}_LL1928: _tmp787=_tmp745.f2;if((
unsigned int)_tmp787 > 3?*((int*)_tmp787)== Cyc_Absyn_VarType: 0){_LL1930: _tmp789=((
struct Cyc_Absyn_VarType_struct*)_tmp787)->f1;goto _LL1868;}else{goto _LL1869;}
_LL1869: _LL1942: _tmp795=_tmp745.f1;if((unsigned int)_tmp795 > 3?*((int*)_tmp795)
== Cyc_Absyn_StructType: 0){_LL1946: _tmp799=((struct Cyc_Absyn_StructType_struct*)
_tmp795)->f1;goto _LL1944;_LL1944: _tmp797=((struct Cyc_Absyn_StructType_struct*)
_tmp795)->f2;goto _LL1936;}else{goto _LL1871;}_LL1936: _tmp78F=_tmp745.f2;if((
unsigned int)_tmp78F > 3?*((int*)_tmp78F)== Cyc_Absyn_StructType: 0){_LL1940:
_tmp793=((struct Cyc_Absyn_StructType_struct*)_tmp78F)->f1;goto _LL1938;_LL1938:
_tmp791=((struct Cyc_Absyn_StructType_struct*)_tmp78F)->f2;goto _LL1870;}else{goto
_LL1871;}_LL1871: _LL1952: _tmp79F=_tmp745.f1;if((unsigned int)_tmp79F > 3?*((int*)
_tmp79F)== Cyc_Absyn_EnumType: 0){_LL1954: _tmp7A1=((struct Cyc_Absyn_EnumType_struct*)
_tmp79F)->f1;goto _LL1948;}else{goto _LL1873;}_LL1948: _tmp79B=_tmp745.f2;if((
unsigned int)_tmp79B > 3?*((int*)_tmp79B)== Cyc_Absyn_EnumType: 0){_LL1950: _tmp79D=((
struct Cyc_Absyn_EnumType_struct*)_tmp79B)->f1;goto _LL1872;}else{goto _LL1873;}
_LL1873: _LL1960: _tmp7A7=_tmp745.f1;if((unsigned int)_tmp7A7 > 3?*((int*)_tmp7A7)
== Cyc_Absyn_AnonEnumType: 0){_LL1962: _tmp7A9=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp7A7)->f1;goto _LL1956;}else{goto _LL1875;}_LL1956: _tmp7A3=_tmp745.f2;if((
unsigned int)_tmp7A3 > 3?*((int*)_tmp7A3)== Cyc_Absyn_AnonEnumType: 0){_LL1958:
_tmp7A5=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp7A3)->f1;goto _LL1874;}else{
goto _LL1875;}_LL1875: _LL1972: _tmp7B3=_tmp745.f1;if((unsigned int)_tmp7B3 > 3?*((
int*)_tmp7B3)== Cyc_Absyn_UnionType: 0){_LL1978: _tmp7B9=((struct Cyc_Absyn_UnionType_struct*)
_tmp7B3)->f1;goto _LL1976;_LL1976: _tmp7B7=((struct Cyc_Absyn_UnionType_struct*)
_tmp7B3)->f2;goto _LL1974;_LL1974: _tmp7B5=((struct Cyc_Absyn_UnionType_struct*)
_tmp7B3)->f3;goto _LL1964;}else{goto _LL1877;}_LL1964: _tmp7AB=_tmp745.f2;if((
unsigned int)_tmp7AB > 3?*((int*)_tmp7AB)== Cyc_Absyn_UnionType: 0){_LL1970:
_tmp7B1=((struct Cyc_Absyn_UnionType_struct*)_tmp7AB)->f1;goto _LL1968;_LL1968:
_tmp7AF=((struct Cyc_Absyn_UnionType_struct*)_tmp7AB)->f2;goto _LL1966;_LL1966:
_tmp7AD=((struct Cyc_Absyn_UnionType_struct*)_tmp7AB)->f3;goto _LL1876;}else{goto
_LL1877;}_LL1877: _LL1993: _tmp7C8=_tmp745.f1;if((unsigned int)_tmp7C8 > 3?*((int*)
_tmp7C8)== Cyc_Absyn_TunionType: 0){_LL1995: _tmp7CA=((struct Cyc_Absyn_TunionType_struct*)
_tmp7C8)->f1;_LL2001: _tmp7D0=(void*)_tmp7CA.tunion_info;if(*((int*)_tmp7D0)== 
Cyc_Absyn_KnownTunion){_LL2003: _tmp7D2=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp7D0)->f1;_tmp7D4=*_tmp7D2;goto _LL1999;}else{goto _LL1879;}_LL1999: _tmp7CE=
_tmp7CA.targs;goto _LL1997;_LL1997: _tmp7CC=(void*)_tmp7CA.rgn;goto _LL1980;}else{
goto _LL1879;}_LL1980: _tmp7BB=_tmp745.f2;if((unsigned int)_tmp7BB > 3?*((int*)
_tmp7BB)== Cyc_Absyn_TunionType: 0){_LL1982: _tmp7BD=((struct Cyc_Absyn_TunionType_struct*)
_tmp7BB)->f1;_LL1988: _tmp7C3=(void*)_tmp7BD.tunion_info;if(*((int*)_tmp7C3)== 
Cyc_Absyn_KnownTunion){_LL1990: _tmp7C5=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp7C3)->f1;_tmp7C7=*_tmp7C5;goto _LL1986;}else{goto _LL1879;}_LL1986: _tmp7C1=
_tmp7BD.targs;goto _LL1984;_LL1984: _tmp7BF=(void*)_tmp7BD.rgn;goto _LL1878;}else{
goto _LL1879;}_LL1879: _LL2018: _tmp7E1=_tmp745.f1;if((unsigned int)_tmp7E1 > 3?*((
int*)_tmp7E1)== Cyc_Absyn_TunionFieldType: 0){_LL2020: _tmp7E3=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp7E1)->f1;_LL2024: _tmp7E7=(void*)_tmp7E3.field_info;if(*((int*)_tmp7E7)== Cyc_Absyn_KnownTunionfield){
_LL2028: _tmp7EB=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp7E7)->f1;goto
_LL2026;_LL2026: _tmp7E9=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp7E7)->f2;
goto _LL2022;}else{goto _LL1881;}_LL2022: _tmp7E5=_tmp7E3.targs;goto _LL2006;}else{
goto _LL1881;}_LL2006: _tmp7D5=_tmp745.f2;if((unsigned int)_tmp7D5 > 3?*((int*)
_tmp7D5)== Cyc_Absyn_TunionFieldType: 0){_LL2008: _tmp7D7=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp7D5)->f1;_LL2012: _tmp7DB=(void*)_tmp7D7.field_info;if(*((int*)_tmp7DB)== Cyc_Absyn_KnownTunionfield){
_LL2016: _tmp7DF=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp7DB)->f1;goto
_LL2014;_LL2014: _tmp7DD=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp7DB)->f2;
goto _LL2010;}else{goto _LL1881;}_LL2010: _tmp7D9=_tmp7D7.targs;goto _LL1880;}else{
goto _LL1881;}_LL1881: _LL2044: _tmp7FB=_tmp745.f1;if((unsigned int)_tmp7FB > 3?*((
int*)_tmp7FB)== Cyc_Absyn_PointerType: 0){_LL2046: _tmp7FD=((struct Cyc_Absyn_PointerType_struct*)
_tmp7FB)->f1;_LL2056: _tmp807=(void*)_tmp7FD.elt_typ;goto _LL2054;_LL2054: _tmp805=(
void*)_tmp7FD.rgn_typ;goto _LL2052;_LL2052: _tmp803=_tmp7FD.nullable;goto _LL2050;
_LL2050: _tmp801=_tmp7FD.tq;goto _LL2048;_LL2048: _tmp7FF=_tmp7FD.bounds;goto
_LL2030;}else{goto _LL1883;}_LL2030: _tmp7ED=_tmp745.f2;if((unsigned int)_tmp7ED > 
3?*((int*)_tmp7ED)== Cyc_Absyn_PointerType: 0){_LL2032: _tmp7EF=((struct Cyc_Absyn_PointerType_struct*)
_tmp7ED)->f1;_LL2042: _tmp7F9=(void*)_tmp7EF.elt_typ;goto _LL2040;_LL2040: _tmp7F7=(
void*)_tmp7EF.rgn_typ;goto _LL2038;_LL2038: _tmp7F5=_tmp7EF.nullable;goto _LL2036;
_LL2036: _tmp7F3=_tmp7EF.tq;goto _LL2034;_LL2034: _tmp7F1=_tmp7EF.bounds;goto
_LL1882;}else{goto _LL1883;}_LL1883: _LL2064: _tmp80F=_tmp745.f1;if((unsigned int)
_tmp80F > 3?*((int*)_tmp80F)== Cyc_Absyn_IntType: 0){_LL2068: _tmp813=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp80F)->f1;goto _LL2066;_LL2066: _tmp811=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp80F)->f2;goto _LL2058;}else{goto _LL1885;}
_LL2058: _tmp809=_tmp745.f2;if((unsigned int)_tmp809 > 3?*((int*)_tmp809)== Cyc_Absyn_IntType:
0){_LL2062: _tmp80D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp809)->f1;goto
_LL2060;_LL2060: _tmp80B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp809)->f2;
goto _LL1884;}else{goto _LL1885;}_LL1885: _LL2072: _tmp817=_tmp745.f1;if(_tmp817 == (
void*)Cyc_Absyn_FloatType){goto _LL2070;}else{goto _LL1887;}_LL2070: _tmp815=
_tmp745.f2;if(_tmp815 == (void*)Cyc_Absyn_FloatType){goto _LL1886;}else{goto
_LL1887;}_LL1887: _LL2078: _tmp81D=_tmp745.f1;if((unsigned int)_tmp81D > 3?*((int*)
_tmp81D)== Cyc_Absyn_DoubleType: 0){_LL2080: _tmp81F=((struct Cyc_Absyn_DoubleType_struct*)
_tmp81D)->f1;goto _LL2074;}else{goto _LL1889;}_LL2074: _tmp819=_tmp745.f2;if((
unsigned int)_tmp819 > 3?*((int*)_tmp819)== Cyc_Absyn_DoubleType: 0){_LL2076:
_tmp81B=((struct Cyc_Absyn_DoubleType_struct*)_tmp819)->f1;goto _LL1888;}else{goto
_LL1889;}_LL1889: _LL2090: _tmp829=_tmp745.f1;if((unsigned int)_tmp829 > 3?*((int*)
_tmp829)== Cyc_Absyn_ArrayType: 0){_LL2096: _tmp82F=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp829)->f1;goto _LL2094;_LL2094: _tmp82D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp829)->f2;goto _LL2092;_LL2092: _tmp82B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp829)->f3;goto _LL2082;}else{goto _LL1891;}_LL2082: _tmp821=_tmp745.f2;if((
unsigned int)_tmp821 > 3?*((int*)_tmp821)== Cyc_Absyn_ArrayType: 0){_LL2088:
_tmp827=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp821)->f1;goto _LL2086;
_LL2086: _tmp825=((struct Cyc_Absyn_ArrayType_struct*)_tmp821)->f2;goto _LL2084;
_LL2084: _tmp823=((struct Cyc_Absyn_ArrayType_struct*)_tmp821)->f3;goto _LL1890;}
else{goto _LL1891;}_LL1891: _LL2118: _tmp845=_tmp745.f1;if((unsigned int)_tmp845 > 3?*((
int*)_tmp845)== Cyc_Absyn_FnType: 0){_LL2120: _tmp847=((struct Cyc_Absyn_FnType_struct*)
_tmp845)->f1;_LL2136: _tmp857=_tmp847.tvars;goto _LL2134;_LL2134: _tmp855=_tmp847.effect;
goto _LL2132;_LL2132: _tmp853=(void*)_tmp847.ret_typ;goto _LL2130;_LL2130: _tmp851=
_tmp847.args;goto _LL2128;_LL2128: _tmp84F=_tmp847.c_varargs;goto _LL2126;_LL2126:
_tmp84D=_tmp847.cyc_varargs;goto _LL2124;_LL2124: _tmp84B=_tmp847.rgn_po;goto
_LL2122;_LL2122: _tmp849=_tmp847.attributes;goto _LL2098;}else{goto _LL1893;}
_LL2098: _tmp831=_tmp745.f2;if((unsigned int)_tmp831 > 3?*((int*)_tmp831)== Cyc_Absyn_FnType:
0){_LL2100: _tmp833=((struct Cyc_Absyn_FnType_struct*)_tmp831)->f1;_LL2116: _tmp843=
_tmp833.tvars;goto _LL2114;_LL2114: _tmp841=_tmp833.effect;goto _LL2112;_LL2112:
_tmp83F=(void*)_tmp833.ret_typ;goto _LL2110;_LL2110: _tmp83D=_tmp833.args;goto
_LL2108;_LL2108: _tmp83B=_tmp833.c_varargs;goto _LL2106;_LL2106: _tmp839=_tmp833.cyc_varargs;
goto _LL2104;_LL2104: _tmp837=_tmp833.rgn_po;goto _LL2102;_LL2102: _tmp835=_tmp833.attributes;
goto _LL1892;}else{goto _LL1893;}_LL1893: _LL2142: _tmp85D=_tmp745.f1;if((
unsigned int)_tmp85D > 3?*((int*)_tmp85D)== Cyc_Absyn_TupleType: 0){_LL2144:
_tmp85F=((struct Cyc_Absyn_TupleType_struct*)_tmp85D)->f1;goto _LL2138;}else{goto
_LL1895;}_LL2138: _tmp859=_tmp745.f2;if((unsigned int)_tmp859 > 3?*((int*)_tmp859)
== Cyc_Absyn_TupleType: 0){_LL2140: _tmp85B=((struct Cyc_Absyn_TupleType_struct*)
_tmp859)->f1;goto _LL1894;}else{goto _LL1895;}_LL1895: _LL2150: _tmp865=_tmp745.f1;
if((unsigned int)_tmp865 > 3?*((int*)_tmp865)== Cyc_Absyn_AnonStructType: 0){
_LL2152: _tmp867=((struct Cyc_Absyn_AnonStructType_struct*)_tmp865)->f1;goto
_LL2146;}else{goto _LL1897;}_LL2146: _tmp861=_tmp745.f2;if((unsigned int)_tmp861 > 
3?*((int*)_tmp861)== Cyc_Absyn_AnonStructType: 0){_LL2148: _tmp863=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp861)->f1;goto _LL1896;}else{goto _LL1897;}_LL1897: _LL2158: _tmp86D=_tmp745.f1;
if((unsigned int)_tmp86D > 3?*((int*)_tmp86D)== Cyc_Absyn_AnonUnionType: 0){
_LL2160: _tmp86F=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp86D)->f1;goto _LL2154;}
else{goto _LL1899;}_LL2154: _tmp869=_tmp745.f2;if((unsigned int)_tmp869 > 3?*((int*)
_tmp869)== Cyc_Absyn_AnonUnionType: 0){_LL2156: _tmp86B=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp869)->f1;goto _LL1898;}else{goto _LL1899;}_LL1899: _LL2164: _tmp873=_tmp745.f1;
if(_tmp873 == (void*)Cyc_Absyn_HeapRgn){goto _LL2162;}else{goto _LL1901;}_LL2162:
_tmp871=_tmp745.f2;if(_tmp871 == (void*)Cyc_Absyn_HeapRgn){goto _LL1900;}else{goto
_LL1901;}_LL1901: _LL2170: _tmp879=_tmp745.f1;if((unsigned int)_tmp879 > 3?*((int*)
_tmp879)== Cyc_Absyn_RgnHandleType: 0){_LL2172: _tmp87B=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp879)->f1;goto _LL2166;}else{goto _LL1903;}_LL2166: _tmp875=_tmp745.f2;if((
unsigned int)_tmp875 > 3?*((int*)_tmp875)== Cyc_Absyn_RgnHandleType: 0){_LL2168:
_tmp877=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp875)->f1;goto _LL1902;}
else{goto _LL1903;}_LL1903: _LL2178: _tmp881=_tmp745.f1;if((unsigned int)_tmp881 > 3?*((
int*)_tmp881)== Cyc_Absyn_SizeofType: 0){_LL2180: _tmp883=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp881)->f1;goto _LL2174;}else{goto _LL1905;}_LL2174: _tmp87D=_tmp745.f2;if((
unsigned int)_tmp87D > 3?*((int*)_tmp87D)== Cyc_Absyn_SizeofType: 0){_LL2176:
_tmp87F=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp87D)->f1;goto _LL1904;}
else{goto _LL1905;}_LL1905: _LL2182: _tmp885=_tmp745.f1;if((unsigned int)_tmp885 > 3?*((
int*)_tmp885)== Cyc_Absyn_JoinEff: 0){goto _LL1906;}else{goto _LL1907;}_LL1907:
_LL2184: _tmp887=_tmp745.f2;if((unsigned int)_tmp887 > 3?*((int*)_tmp887)== Cyc_Absyn_JoinEff:
0){goto _LL1908;}else{goto _LL1909;}_LL1909: _LL2186: _tmp889=_tmp745.f1;if((
unsigned int)_tmp889 > 3?*((int*)_tmp889)== Cyc_Absyn_AccessEff: 0){goto _LL1910;}
else{goto _LL1911;}_LL1911: _LL2188: _tmp88B=_tmp745.f1;if((unsigned int)_tmp88B > 3?*((
int*)_tmp88B)== Cyc_Absyn_RgnsEff: 0){goto _LL1912;}else{goto _LL1913;}_LL1913:
_LL2190: _tmp88D=_tmp745.f2;if((unsigned int)_tmp88D > 3?*((int*)_tmp88D)== Cyc_Absyn_RgnsEff:
0){goto _LL1914;}else{goto _LL1915;}_LL1915: _LL2192: _tmp88F=_tmp745.f2;if((
unsigned int)_tmp88F > 3?*((int*)_tmp88F)== Cyc_Absyn_AccessEff: 0){goto _LL1916;}
else{goto _LL1917;}_LL1917: goto _LL1918;_LL1864:({void*_tmp891[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: can only compare closed types",
sizeof(unsigned char),39),_tag_arr(_tmp891,sizeof(void*),0));});return 0;_LL1866:
return 0;_LL1868: {struct _tagged_arr*_tmp892=_tmp789->name;struct _tagged_arr*
_tmp893=_tmp78D->name;int _tmp894=*((int*)_check_null(_tmp789->identity));int
_tmp895=*((int*)_check_null(_tmp78D->identity));return Cyc_Core_intcmp(_tmp894,
_tmp895);}_LL1870: {int _tmp896=((int(*)(int(*cmp)(struct _tuple1*,struct _tuple1*),
struct _tuple1*a1,struct _tuple1*a2))Cyc_Tcutil_star_cmp)(Cyc_Absyn_qvar_cmp,
_tmp793,_tmp799);if(_tmp896 != 0){return _tmp896;}else{return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp,_tmp791,_tmp797);}}_LL1872: {int _tmp897=Cyc_Absyn_qvar_cmp(
_tmp7A1,_tmp79D);return _tmp897;}_LL1874: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp7A9,_tmp7A5);_LL1876: {int _tmp898=((int(*)(int(*cmp)(
struct _tuple1*,struct _tuple1*),struct _tuple1*a1,struct _tuple1*a2))Cyc_Tcutil_star_cmp)(
Cyc_Absyn_qvar_cmp,_tmp7B1,_tmp7B9);if(_tmp898 != 0){return _tmp898;}else{return
Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,_tmp7AF,_tmp7B7);}}_LL1878: if(_tmp7C7 == 
_tmp7D4){return 0;}{int _tmp899=Cyc_Absyn_qvar_cmp(_tmp7C7->name,_tmp7D4->name);
if(_tmp899 != 0){return _tmp899;}{int _tmp89A=Cyc_Tcutil_typecmp(_tmp7BF,_tmp7CC);
if(_tmp89A != 0){return _tmp89A;}return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,
_tmp7C1,_tmp7CE);}}_LL1880: if(_tmp7DF == _tmp7EB){return 0;}{int _tmp89B=Cyc_Absyn_qvar_cmp(
_tmp7EB->name,_tmp7DF->name);if(_tmp89B != 0){return _tmp89B;}{int _tmp89C=Cyc_Absyn_qvar_cmp(
_tmp7E9->name,_tmp7DD->name);if(_tmp89C != 0){return _tmp89C;}return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp,_tmp7D9,_tmp7E5);}}_LL1882: {int _tmp89D=Cyc_Tcutil_typecmp(
_tmp7F9,_tmp807);if(_tmp89D != 0){return _tmp89D;}{int _tmp89E=Cyc_Tcutil_typecmp(
_tmp7F7,_tmp805);if(_tmp89E != 0){return _tmp89E;}{int _tmp89F=Cyc_Tcutil_tqual_cmp(
_tmp7F3,_tmp801);if(_tmp89F != 0){return _tmp89F;}{int _tmp8A0=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp7F1,_tmp7FF);if(_tmp8A0 != 0){return _tmp8A0;}{void*
_tmp8A1=(void*)(Cyc_Absyn_compress_conref(_tmp7F1))->v;void*_tmp8A7;_LL2211: if((
unsigned int)_tmp8A1 > 1?*((int*)_tmp8A1)== Cyc_Absyn_Eq_constr: 0){_LL2216:
_tmp8A7=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp8A1)->f1;if(_tmp8A7 == (
void*)Cyc_Absyn_Unknown_b){goto _LL2212;}else{goto _LL2213;}}else{goto _LL2213;}
_LL2213: goto _LL2214;_LL2212: return 0;_LL2214: goto _LL2210;_LL2210:;}return((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp7F5,_tmp803);}}}}_LL1884: if(_tmp80D != _tmp813){return Cyc_Core_intcmp((
int)((unsigned int)_tmp80D),(int)((unsigned int)_tmp813));}if(_tmp80B != _tmp811){
return Cyc_Core_intcmp((int)((unsigned int)_tmp80B),(int)((unsigned int)_tmp811));}
return 0;_LL1886: return 0;_LL1888: if(_tmp81F == _tmp81B){return 0;}else{if(_tmp81F){
return - 1;}else{return 1;}}_LL1890: {int _tmp8A9=Cyc_Tcutil_tqual_cmp(_tmp825,
_tmp82D);if(_tmp8A9 != 0){return _tmp8A9;}{int _tmp8AA=Cyc_Tcutil_typecmp(_tmp827,
_tmp82F);if(_tmp8AA != 0){return _tmp8AA;}if(_tmp82B == _tmp823){return 0;}if(
_tmp82B == 0? 1: _tmp823 == 0){({void*_tmp8AB[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("missing expression in array index",
sizeof(unsigned char),34),_tag_arr(_tmp8AB,sizeof(void*),0));});}return((int(*)(
int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,
struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,
_tmp82B,_tmp823);}}_LL1892:({void*_tmp8AC[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: function types not handled",
sizeof(unsigned char),36),_tag_arr(_tmp8AC,sizeof(void*),0));});goto _LL1862;
_LL1894: return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_tqual_type_cmp,
_tmp85B,_tmp85F);_LL1896: _tmp86F=_tmp867;_tmp86B=_tmp863;goto _LL1898;_LL1898:
return((int(*)(int(*cmp)(struct Cyc_Absyn_Structfield*,struct Cyc_Absyn_Structfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_structfield_cmp,
_tmp86B,_tmp86F);_LL1900: return 0;_LL1902: return Cyc_Tcutil_typecmp(_tmp87B,
_tmp877);_LL1904: return Cyc_Tcutil_typecmp(_tmp883,_tmp87F);_LL1906: goto _LL1908;
_LL1908: goto _LL1910;_LL1910: goto _LL1912;_LL1912: goto _LL1914;_LL1914: goto _LL1916;
_LL1916:({void*_tmp8AD[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("typecmp: effects not handled",sizeof(unsigned char),
29),_tag_arr(_tmp8AD,sizeof(void*),0));});goto _LL1862;_LL1918: goto _LL1862;
_LL1862:;}return({void*_tmp8AE[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Unmatched case in typecmp",sizeof(
unsigned char),26),_tag_arr(_tmp8AE,sizeof(void*),0));});}}int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp8AF=Cyc_Tcutil_compress(t);_LL2225: if((unsigned int)_tmp8AF > 3?*((
int*)_tmp8AF)== Cyc_Absyn_IntType: 0){goto _LL2226;}else{goto _LL2227;}_LL2227: if(
_tmp8AF == (void*)Cyc_Absyn_FloatType){goto _LL2228;}else{goto _LL2229;}_LL2229: if((
unsigned int)_tmp8AF > 3?*((int*)_tmp8AF)== Cyc_Absyn_DoubleType: 0){goto _LL2230;}
else{goto _LL2231;}_LL2231: if((unsigned int)_tmp8AF > 3?*((int*)_tmp8AF)== Cyc_Absyn_EnumType:
0){goto _LL2232;}else{goto _LL2233;}_LL2233: if((unsigned int)_tmp8AF > 3?*((int*)
_tmp8AF)== Cyc_Absyn_AnonEnumType: 0){goto _LL2234;}else{goto _LL2235;}_LL2235: goto
_LL2236;_LL2226: goto _LL2228;_LL2228: goto _LL2230;_LL2230: goto _LL2232;_LL2232: goto
_LL2234;_LL2234: return 1;_LL2236: return 0;_LL2224:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple8 _tmp8BE=({struct _tuple8 _tmp8BD;_tmp8BD.f1=t1;_tmp8BD.f2=t2;_tmp8BD;});
void*_tmp8DE;int _tmp8E0;void*_tmp8E2;int _tmp8E4;void*_tmp8E6;void*_tmp8E8;void*
_tmp8EA;void*_tmp8EC;void*_tmp8EE;void*_tmp8F0;void*_tmp8F2;void*_tmp8F4;void*
_tmp8F6;void*_tmp8F8;void*_tmp8FA;void*_tmp8FC;void*_tmp8FE;void*_tmp900;void*
_tmp902;void*_tmp904;void*_tmp906;void*_tmp908;void*_tmp90A;void*_tmp90C;void*
_tmp90E;void*_tmp910;void*_tmp912;void*_tmp914;void*_tmp916;void*_tmp918;void*
_tmp91A;void*_tmp91C;void*_tmp91E;void*_tmp920;void*_tmp922;void*_tmp924;void*
_tmp926;void*_tmp928;void*_tmp92A;void*_tmp92C;void*_tmp92E;_LL2240: _LL2275:
_tmp8E2=_tmp8BE.f1;if((unsigned int)_tmp8E2 > 3?*((int*)_tmp8E2)== Cyc_Absyn_DoubleType:
0){_LL2277: _tmp8E4=((struct Cyc_Absyn_DoubleType_struct*)_tmp8E2)->f1;goto _LL2271;}
else{goto _LL2242;}_LL2271: _tmp8DE=_tmp8BE.f2;if((unsigned int)_tmp8DE > 3?*((int*)
_tmp8DE)== Cyc_Absyn_DoubleType: 0){_LL2273: _tmp8E0=((struct Cyc_Absyn_DoubleType_struct*)
_tmp8DE)->f1;goto _LL2241;}else{goto _LL2242;}_LL2242: _LL2281: _tmp8E8=_tmp8BE.f1;
if((unsigned int)_tmp8E8 > 3?*((int*)_tmp8E8)== Cyc_Absyn_DoubleType: 0){goto
_LL2279;}else{goto _LL2244;}_LL2279: _tmp8E6=_tmp8BE.f2;if(_tmp8E6 == (void*)Cyc_Absyn_FloatType){
goto _LL2243;}else{goto _LL2244;}_LL2244: _LL2285: _tmp8EC=_tmp8BE.f1;if((
unsigned int)_tmp8EC > 3?*((int*)_tmp8EC)== Cyc_Absyn_DoubleType: 0){goto _LL2283;}
else{goto _LL2246;}_LL2283: _tmp8EA=_tmp8BE.f2;if((unsigned int)_tmp8EA > 3?*((int*)
_tmp8EA)== Cyc_Absyn_IntType: 0){goto _LL2245;}else{goto _LL2246;}_LL2246: _LL2289:
_tmp8F0=_tmp8BE.f1;if((unsigned int)_tmp8F0 > 3?*((int*)_tmp8F0)== Cyc_Absyn_DoubleType:
0){goto _LL2287;}else{goto _LL2248;}_LL2287: _tmp8EE=_tmp8BE.f2;if((unsigned int)
_tmp8EE > 3?*((int*)_tmp8EE)== Cyc_Absyn_SizeofType: 0){goto _LL2247;}else{goto
_LL2248;}_LL2248: _LL2293: _tmp8F4=_tmp8BE.f1;if(_tmp8F4 == (void*)Cyc_Absyn_FloatType){
goto _LL2291;}else{goto _LL2250;}_LL2291: _tmp8F2=_tmp8BE.f2;if((unsigned int)
_tmp8F2 > 3?*((int*)_tmp8F2)== Cyc_Absyn_SizeofType: 0){goto _LL2249;}else{goto
_LL2250;}_LL2250: _LL2297: _tmp8F8=_tmp8BE.f1;if(_tmp8F8 == (void*)Cyc_Absyn_FloatType){
goto _LL2295;}else{goto _LL2252;}_LL2295: _tmp8F6=_tmp8BE.f2;if((unsigned int)
_tmp8F6 > 3?*((int*)_tmp8F6)== Cyc_Absyn_IntType: 0){goto _LL2251;}else{goto _LL2252;}
_LL2252: _LL2303: _tmp8FE=_tmp8BE.f1;if((unsigned int)_tmp8FE > 3?*((int*)_tmp8FE)
== Cyc_Absyn_IntType: 0){_LL2305: _tmp900=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp8FE)->f2;if(_tmp900 == (void*)Cyc_Absyn_B8){goto _LL2299;}else{goto _LL2254;}}
else{goto _LL2254;}_LL2299: _tmp8FA=_tmp8BE.f2;if((unsigned int)_tmp8FA > 3?*((int*)
_tmp8FA)== Cyc_Absyn_IntType: 0){_LL2301: _tmp8FC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp8FA)->f2;if(_tmp8FC == (void*)Cyc_Absyn_B8){goto _LL2253;}else{goto _LL2254;}}
else{goto _LL2254;}_LL2254: _LL2307: _tmp902=_tmp8BE.f1;if((unsigned int)_tmp902 > 3?*((
int*)_tmp902)== Cyc_Absyn_IntType: 0){_LL2309: _tmp904=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp902)->f2;if(_tmp904 == (void*)Cyc_Absyn_B8){goto _LL2255;}else{goto _LL2256;}}
else{goto _LL2256;}_LL2256: _LL2313: _tmp908=_tmp8BE.f1;if((unsigned int)_tmp908 > 3?*((
int*)_tmp908)== Cyc_Absyn_IntType: 0){_LL2315: _tmp90A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp908)->f2;if(_tmp90A == (void*)Cyc_Absyn_B4){goto _LL2311;}else{goto _LL2258;}}
else{goto _LL2258;}_LL2311: _tmp906=_tmp8BE.f2;if(_tmp906 == (void*)Cyc_Absyn_FloatType){
goto _LL2257;}else{goto _LL2258;}_LL2258: _LL2321: _tmp910=_tmp8BE.f1;if((
unsigned int)_tmp910 > 3?*((int*)_tmp910)== Cyc_Absyn_IntType: 0){_LL2323: _tmp912=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp910)->f2;if(_tmp912 == (void*)Cyc_Absyn_B4){
goto _LL2317;}else{goto _LL2260;}}else{goto _LL2260;}_LL2317: _tmp90C=_tmp8BE.f2;if((
unsigned int)_tmp90C > 3?*((int*)_tmp90C)== Cyc_Absyn_IntType: 0){_LL2319: _tmp90E=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp90C)->f2;if(_tmp90E == (void*)Cyc_Absyn_B2){
goto _LL2259;}else{goto _LL2260;}}else{goto _LL2260;}_LL2260: _LL2329: _tmp918=
_tmp8BE.f1;if((unsigned int)_tmp918 > 3?*((int*)_tmp918)== Cyc_Absyn_IntType: 0){
_LL2331: _tmp91A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp918)->f2;if(_tmp91A
== (void*)Cyc_Absyn_B4){goto _LL2325;}else{goto _LL2262;}}else{goto _LL2262;}
_LL2325: _tmp914=_tmp8BE.f2;if((unsigned int)_tmp914 > 3?*((int*)_tmp914)== Cyc_Absyn_IntType:
0){_LL2327: _tmp916=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp914)->f2;if(
_tmp916 == (void*)Cyc_Absyn_B1){goto _LL2261;}else{goto _LL2262;}}else{goto _LL2262;}
_LL2262: _LL2337: _tmp920=_tmp8BE.f1;if((unsigned int)_tmp920 > 3?*((int*)_tmp920)
== Cyc_Absyn_IntType: 0){_LL2339: _tmp922=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp920)->f2;if(_tmp922 == (void*)Cyc_Absyn_B2){goto _LL2333;}else{goto _LL2264;}}
else{goto _LL2264;}_LL2333: _tmp91C=_tmp8BE.f2;if((unsigned int)_tmp91C > 3?*((int*)
_tmp91C)== Cyc_Absyn_IntType: 0){_LL2335: _tmp91E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp91C)->f2;if(_tmp91E == (void*)Cyc_Absyn_B1){goto _LL2263;}else{goto _LL2264;}}
else{goto _LL2264;}_LL2264: _LL2345: _tmp928=_tmp8BE.f1;if((unsigned int)_tmp928 > 3?*((
int*)_tmp928)== Cyc_Absyn_SizeofType: 0){goto _LL2341;}else{goto _LL2266;}_LL2341:
_tmp924=_tmp8BE.f2;if((unsigned int)_tmp924 > 3?*((int*)_tmp924)== Cyc_Absyn_IntType:
0){_LL2343: _tmp926=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp924)->f2;if(
_tmp926 == (void*)Cyc_Absyn_B2){goto _LL2265;}else{goto _LL2266;}}else{goto _LL2266;}
_LL2266: _LL2351: _tmp92E=_tmp8BE.f1;if((unsigned int)_tmp92E > 3?*((int*)_tmp92E)
== Cyc_Absyn_SizeofType: 0){goto _LL2347;}else{goto _LL2268;}_LL2347: _tmp92A=
_tmp8BE.f2;if((unsigned int)_tmp92A > 3?*((int*)_tmp92A)== Cyc_Absyn_IntType: 0){
_LL2349: _tmp92C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp92A)->f2;if(_tmp92C
== (void*)Cyc_Absyn_B1){goto _LL2267;}else{goto _LL2268;}}else{goto _LL2268;}
_LL2268: goto _LL2269;_LL2241: return ! _tmp8E0? _tmp8E4: 0;_LL2243: goto _LL2245;_LL2245:
goto _LL2247;_LL2247: goto _LL2249;_LL2249: goto _LL2251;_LL2251: return 1;_LL2253:
return 0;_LL2255: goto _LL2257;_LL2257: goto _LL2259;_LL2259: goto _LL2261;_LL2261: goto
_LL2263;_LL2263: goto _LL2265;_LL2265: goto _LL2267;_LL2267: return 1;_LL2269: return 0;
_LL2239:;}}int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*
es){struct Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;
el=el->tl){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
max_arith_type=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp930=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmp930->v=(void*)t1;_tmp930;});}}}}if(
max_arith_type != 0){if(! Cyc_Tcutil_unify(t,(void*)((struct Cyc_Core_Opt*)
_check_null(max_arith_type))->v)){return 0;}}{struct Cyc_List_List*el=es;for(0;el
!= 0;el=el->tl){if(! Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){({
struct Cyc_Std_String_pa_struct _tmp933;_tmp933.tag=Cyc_Std_String_pa;_tmp933.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);{struct Cyc_Std_String_pa_struct
_tmp932;_tmp932.tag=Cyc_Std_String_pa;_tmp932.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{void*_tmp931[2]={& _tmp932,& _tmp933};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,_tag_arr("type mismatch: expecting %s but found %s",sizeof(
unsigned char),41),_tag_arr(_tmp931,sizeof(void*),2));}}});return 0;}}}return 1;}
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!
Cyc_Tcutil_coerce_sint_typ(te,e)){void*_tmp934=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL2358: if((unsigned int)_tmp934 > 
3?*((int*)_tmp934)== Cyc_Absyn_PointerType: 0){goto _LL2359;}else{goto _LL2360;}
_LL2360: goto _LL2361;_LL2359: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_t);
goto _LL2357;_LL2361: return 0;_LL2357:;}return 1;}int Cyc_Tcutil_is_integral_type(
void*t){void*_tmp93A=Cyc_Tcutil_compress(t);_LL2364: if((unsigned int)_tmp93A > 3?*((
int*)_tmp93A)== Cyc_Absyn_IntType: 0){goto _LL2365;}else{goto _LL2366;}_LL2366: if((
unsigned int)_tmp93A > 3?*((int*)_tmp93A)== Cyc_Absyn_SizeofType: 0){goto _LL2367;}
else{goto _LL2368;}_LL2368: if((unsigned int)_tmp93A > 3?*((int*)_tmp93A)== Cyc_Absyn_EnumType:
0){goto _LL2369;}else{goto _LL2370;}_LL2370: if((unsigned int)_tmp93A > 3?*((int*)
_tmp93A)== Cyc_Absyn_AnonEnumType: 0){goto _LL2371;}else{goto _LL2372;}_LL2372: goto
_LL2373;_LL2365: goto _LL2367;_LL2367: goto _LL2369;_LL2369: goto _LL2371;_LL2371:
return 1;_LL2373: return 0;_LL2363:;}int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_t)){return 1;}if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_t)){({void*
_tmp946[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(unsigned char),44),_tag_arr(_tmp946,sizeof(void*),0));});}Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_t);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_t)){return 1;}if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_t)){({void*
_tmp947[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(unsigned char),44),_tag_arr(_tmp947,sizeof(void*),0));});}Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_t);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple8 _tmp949=({struct _tuple8 _tmp948;_tmp948.f1=
t1;_tmp948.f2=t2;_tmp948;});void*_tmp957;struct Cyc_Absyn_PtrInfo _tmp959;void*
_tmp95B;struct Cyc_Absyn_PtrInfo _tmp95D;void*_tmp95F;struct Cyc_Absyn_Exp*_tmp961;
struct Cyc_Absyn_Tqual _tmp963;void*_tmp965;void*_tmp967;struct Cyc_Absyn_Exp*
_tmp969;struct Cyc_Absyn_Tqual _tmp96B;void*_tmp96D;void*_tmp96F;struct Cyc_Absyn_TunionInfo
_tmp971;struct Cyc_List_List*_tmp973;void*_tmp975;struct Cyc_Absyn_Tuniondecl**
_tmp977;struct Cyc_Absyn_Tuniondecl*_tmp979;void*_tmp97A;struct Cyc_Absyn_TunionFieldInfo
_tmp97C;struct Cyc_List_List*_tmp97E;void*_tmp980;struct Cyc_Absyn_Tunionfield*
_tmp982;struct Cyc_Absyn_Tuniondecl*_tmp984;void*_tmp986;struct Cyc_Absyn_TunionInfo
_tmp988;void*_tmp98A;struct Cyc_List_List*_tmp98C;void*_tmp98E;struct Cyc_Absyn_Tuniondecl**
_tmp990;struct Cyc_Absyn_Tuniondecl*_tmp992;void*_tmp993;struct Cyc_Absyn_PtrInfo
_tmp995;struct Cyc_Absyn_Conref*_tmp997;struct Cyc_Absyn_Tqual _tmp999;struct Cyc_Absyn_Conref*
_tmp99B;void*_tmp99D;void*_tmp99F;void*_tmp9A1;void*_tmp9A3;void*_tmp9A5;_LL2379:
_LL2396: _tmp95B=_tmp949.f1;if((unsigned int)_tmp95B > 3?*((int*)_tmp95B)== Cyc_Absyn_PointerType:
0){_LL2398: _tmp95D=((struct Cyc_Absyn_PointerType_struct*)_tmp95B)->f1;goto
_LL2392;}else{goto _LL2381;}_LL2392: _tmp957=_tmp949.f2;if((unsigned int)_tmp957 > 
3?*((int*)_tmp957)== Cyc_Absyn_PointerType: 0){_LL2394: _tmp959=((struct Cyc_Absyn_PointerType_struct*)
_tmp957)->f1;goto _LL2380;}else{goto _LL2381;}_LL2381: _LL2408: _tmp967=_tmp949.f1;
if((unsigned int)_tmp967 > 3?*((int*)_tmp967)== Cyc_Absyn_ArrayType: 0){_LL2414:
_tmp96D=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp967)->f1;goto _LL2412;
_LL2412: _tmp96B=((struct Cyc_Absyn_ArrayType_struct*)_tmp967)->f2;goto _LL2410;
_LL2410: _tmp969=((struct Cyc_Absyn_ArrayType_struct*)_tmp967)->f3;goto _LL2400;}
else{goto _LL2383;}_LL2400: _tmp95F=_tmp949.f2;if((unsigned int)_tmp95F > 3?*((int*)
_tmp95F)== Cyc_Absyn_ArrayType: 0){_LL2406: _tmp965=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp95F)->f1;goto _LL2404;_LL2404: _tmp963=((struct Cyc_Absyn_ArrayType_struct*)
_tmp95F)->f2;goto _LL2402;_LL2402: _tmp961=((struct Cyc_Absyn_ArrayType_struct*)
_tmp95F)->f3;goto _LL2382;}else{goto _LL2383;}_LL2383: _LL2427: _tmp97A=_tmp949.f1;
if((unsigned int)_tmp97A > 3?*((int*)_tmp97A)== Cyc_Absyn_TunionFieldType: 0){
_LL2429: _tmp97C=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp97A)->f1;_LL2433:
_tmp980=(void*)_tmp97C.field_info;if(*((int*)_tmp980)== Cyc_Absyn_KnownTunionfield){
_LL2437: _tmp984=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp980)->f1;goto
_LL2435;_LL2435: _tmp982=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp980)->f2;
goto _LL2431;}else{goto _LL2385;}_LL2431: _tmp97E=_tmp97C.targs;goto _LL2416;}else{
goto _LL2385;}_LL2416: _tmp96F=_tmp949.f2;if((unsigned int)_tmp96F > 3?*((int*)
_tmp96F)== Cyc_Absyn_TunionType: 0){_LL2418: _tmp971=((struct Cyc_Absyn_TunionType_struct*)
_tmp96F)->f1;_LL2422: _tmp975=(void*)_tmp971.tunion_info;if(*((int*)_tmp975)== 
Cyc_Absyn_KnownTunion){_LL2424: _tmp977=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp975)->f1;_tmp979=*_tmp977;goto _LL2420;}else{goto _LL2385;}_LL2420: _tmp973=
_tmp971.targs;goto _LL2384;}else{goto _LL2385;}_LL2385: _LL2452: _tmp993=_tmp949.f1;
if((unsigned int)_tmp993 > 3?*((int*)_tmp993)== Cyc_Absyn_PointerType: 0){_LL2454:
_tmp995=((struct Cyc_Absyn_PointerType_struct*)_tmp993)->f1;_LL2464: _tmp99F=(void*)
_tmp995.elt_typ;goto _LL2462;_LL2462: _tmp99D=(void*)_tmp995.rgn_typ;goto _LL2460;
_LL2460: _tmp99B=_tmp995.nullable;goto _LL2458;_LL2458: _tmp999=_tmp995.tq;goto
_LL2456;_LL2456: _tmp997=_tmp995.bounds;goto _LL2439;}else{goto _LL2387;}_LL2439:
_tmp986=_tmp949.f2;if((unsigned int)_tmp986 > 3?*((int*)_tmp986)== Cyc_Absyn_TunionType:
0){_LL2441: _tmp988=((struct Cyc_Absyn_TunionType_struct*)_tmp986)->f1;_LL2447:
_tmp98E=(void*)_tmp988.tunion_info;if(*((int*)_tmp98E)== Cyc_Absyn_KnownTunion){
_LL2449: _tmp990=((struct Cyc_Absyn_KnownTunion_struct*)_tmp98E)->f1;_tmp992=*
_tmp990;goto _LL2445;}else{goto _LL2387;}_LL2445: _tmp98C=_tmp988.targs;goto _LL2443;
_LL2443: _tmp98A=(void*)_tmp988.rgn;goto _LL2386;}else{goto _LL2387;}_LL2387:
_LL2470: _tmp9A5=_tmp949.f1;if((unsigned int)_tmp9A5 > 3?*((int*)_tmp9A5)== Cyc_Absyn_SizeofType:
0){goto _LL2466;}else{goto _LL2389;}_LL2466: _tmp9A1=_tmp949.f2;if((unsigned int)
_tmp9A1 > 3?*((int*)_tmp9A1)== Cyc_Absyn_IntType: 0){_LL2468: _tmp9A3=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp9A1)->f2;if(_tmp9A3 == (void*)Cyc_Absyn_B4){
goto _LL2388;}else{goto _LL2389;}}else{goto _LL2389;}_LL2389: goto _LL2390;_LL2380: {
int okay=1;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp95D.nullable,_tmp959.nullable)){
void*_tmp9A7=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp95D.nullable))->v;int _tmp9AD;_LL2473: if((unsigned int)_tmp9A7 > 1?*((int*)
_tmp9A7)== Cyc_Absyn_Eq_constr: 0){_LL2478: _tmp9AD=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9A7)->f1;goto _LL2474;}else{goto _LL2475;}_LL2475: goto _LL2476;_LL2474: okay=!
_tmp9AD;goto _LL2472;_LL2476:(int)_throw(({void*_tmp9AF[0]={};Cyc_Tcutil_impos(
_tag_arr("silent_castable conref not eq",sizeof(unsigned char),30),_tag_arr(
_tmp9AF,sizeof(void*),0));}));_LL2472:;}if(! Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp95D.bounds,_tmp959.bounds)){struct _tuple8 _tmp9B1=({struct _tuple8 _tmp9B0;
_tmp9B0.f1=(void*)(Cyc_Absyn_compress_conref(_tmp95D.bounds))->v;_tmp9B0.f2=(
void*)(Cyc_Absyn_compress_conref(_tmp959.bounds))->v;_tmp9B0;});void*_tmp9BB;
void*_tmp9BD;void*_tmp9BF;void*_tmp9C1;void*_tmp9C3;void*_tmp9C5;struct Cyc_Absyn_Exp*
_tmp9C7;void*_tmp9C9;void*_tmp9CB;struct Cyc_Absyn_Exp*_tmp9CD;void*_tmp9CF;void*
_tmp9D1;void*_tmp9D3;void*_tmp9D5;_LL2483: _LL2496: _tmp9BF=_tmp9B1.f1;if((
unsigned int)_tmp9BF > 1?*((int*)_tmp9BF)== Cyc_Absyn_Eq_constr: 0){_LL2498:
_tmp9C1=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9BF)->f1;if((unsigned int)
_tmp9C1 > 1?*((int*)_tmp9C1)== Cyc_Absyn_Upper_b: 0){goto _LL2492;}else{goto _LL2485;}}
else{goto _LL2485;}_LL2492: _tmp9BB=_tmp9B1.f2;if((unsigned int)_tmp9BB > 1?*((int*)
_tmp9BB)== Cyc_Absyn_Eq_constr: 0){_LL2494: _tmp9BD=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9BB)->f1;if(_tmp9BD == (void*)Cyc_Absyn_Unknown_b){goto _LL2484;}else{goto
_LL2485;}}else{goto _LL2485;}_LL2485: _LL2506: _tmp9C9=_tmp9B1.f1;if((unsigned int)
_tmp9C9 > 1?*((int*)_tmp9C9)== Cyc_Absyn_Eq_constr: 0){_LL2508: _tmp9CB=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp9C9)->f1;if((unsigned int)_tmp9CB > 1?*((
int*)_tmp9CB)== Cyc_Absyn_Upper_b: 0){_LL2510: _tmp9CD=((struct Cyc_Absyn_Upper_b_struct*)
_tmp9CB)->f1;goto _LL2500;}else{goto _LL2487;}}else{goto _LL2487;}_LL2500: _tmp9C3=
_tmp9B1.f2;if((unsigned int)_tmp9C3 > 1?*((int*)_tmp9C3)== Cyc_Absyn_Eq_constr: 0){
_LL2502: _tmp9C5=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp9C3)->f1;if((
unsigned int)_tmp9C5 > 1?*((int*)_tmp9C5)== Cyc_Absyn_Upper_b: 0){_LL2504: _tmp9C7=((
struct Cyc_Absyn_Upper_b_struct*)_tmp9C5)->f1;goto _LL2486;}else{goto _LL2487;}}
else{goto _LL2487;}_LL2487: _LL2516: _tmp9D3=_tmp9B1.f1;if((unsigned int)_tmp9D3 > 1?*((
int*)_tmp9D3)== Cyc_Absyn_Eq_constr: 0){_LL2518: _tmp9D5=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9D3)->f1;if(_tmp9D5 == (void*)Cyc_Absyn_Unknown_b){goto _LL2512;}else{goto
_LL2489;}}else{goto _LL2489;}_LL2512: _tmp9CF=_tmp9B1.f2;if((unsigned int)_tmp9CF > 
1?*((int*)_tmp9CF)== Cyc_Absyn_Eq_constr: 0){_LL2514: _tmp9D1=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp9CF)->f1;if(_tmp9D1 == (void*)Cyc_Absyn_Unknown_b){goto _LL2488;}else{goto
_LL2489;}}else{goto _LL2489;}_LL2489: goto _LL2490;_LL2484: okay=1;goto _LL2482;
_LL2486: okay=okay? Cyc_Evexp_eval_const_uint_exp(_tmp9CD)>= Cyc_Evexp_eval_const_uint_exp(
_tmp9C7): 0;({void*_tmp9D7[0]={};Cyc_Tcutil_warn(loc,_tag_arr("implicit cast to shorter array",
sizeof(unsigned char),31),_tag_arr(_tmp9D7,sizeof(void*),0));});goto _LL2482;
_LL2488: okay=1;goto _LL2482;_LL2490: okay=0;goto _LL2482;_LL2482:;}okay=okay? Cyc_Tcutil_unify((
void*)_tmp95D.elt_typ,(void*)_tmp959.elt_typ): 0;okay=okay? Cyc_Tcutil_unify((void*)
_tmp95D.rgn_typ,(void*)_tmp959.rgn_typ)? 1: Cyc_Tcenv_region_outlives(te,(void*)
_tmp95D.rgn_typ,(void*)_tmp959.rgn_typ): 0;okay=okay? !(_tmp95D.tq).q_const? 1:(
_tmp959.tq).q_const: 0;return okay;}_LL2382: {int okay;okay=(_tmp969 != 0? _tmp961 != 
0: 0)? Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp969))== 
Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp961)): 0;
return(okay? Cyc_Tcutil_unify(_tmp96D,_tmp965): 0)? ! _tmp96B.q_const? 1: _tmp963.q_const:
0;}_LL2384: if((_tmp984 == _tmp979? 1: Cyc_Absyn_qvar_cmp(_tmp984->name,_tmp979->name)
== 0)? _tmp982->typs == 0: 0){for(0;_tmp97E != 0? _tmp973 != 0: 0;(_tmp97E=_tmp97E->tl,
_tmp973=_tmp973->tl)){if(! Cyc_Tcutil_unify((void*)_tmp97E->hd,(void*)_tmp973->hd)){
break;}}if(_tmp97E == 0? _tmp973 == 0: 0){return 1;}}return 0;_LL2386:{void*_tmp9D8=
Cyc_Tcutil_compress(_tmp99F);struct Cyc_Absyn_TunionFieldInfo _tmp9DE;struct Cyc_List_List*
_tmp9E0;void*_tmp9E2;struct Cyc_Absyn_Tunionfield*_tmp9E4;struct Cyc_Absyn_Tuniondecl*
_tmp9E6;_LL2522: if((unsigned int)_tmp9D8 > 3?*((int*)_tmp9D8)== Cyc_Absyn_TunionFieldType:
0){_LL2527: _tmp9DE=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp9D8)->f1;
_LL2531: _tmp9E2=(void*)_tmp9DE.field_info;if(*((int*)_tmp9E2)== Cyc_Absyn_KnownTunionfield){
_LL2535: _tmp9E6=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp9E2)->f1;goto
_LL2533;_LL2533: _tmp9E4=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp9E2)->f2;
goto _LL2529;}else{goto _LL2524;}_LL2529: _tmp9E0=_tmp9DE.targs;goto _LL2523;}else{
goto _LL2524;}_LL2524: goto _LL2525;_LL2523: if(! Cyc_Tcutil_unify(_tmp99D,_tmp98A)? !
Cyc_Tcenv_region_outlives(te,_tmp99D,_tmp98A): 0){return 0;}if(!((int(*)(int(*cmp)(
int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp99B,((struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(
0))){return 0;}if(! Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp997,Cyc_Absyn_new_conref(
Cyc_Absyn_bounds_one))){return 0;}if(Cyc_Absyn_qvar_cmp(_tmp992->name,_tmp9E6->name)
== 0? _tmp9E4->typs != 0: 0){int okay=1;for(0;_tmp9E0 != 0? _tmp98C != 0: 0;(_tmp9E0=
_tmp9E0->tl,_tmp98C=_tmp98C->tl)){if(! Cyc_Tcutil_unify((void*)_tmp9E0->hd,(void*)
_tmp98C->hd)){okay=0;break;}}if((! okay? 1: _tmp9E0 != 0)? 1: _tmp98C != 0){return 0;}
return 1;}goto _LL2521;_LL2525: goto _LL2521;_LL2521:;}return 0;_LL2388: return 1;
_LL2390: return Cyc_Tcutil_unify(t1,t2);_LL2378:;}}int Cyc_Tcutil_is_pointer_type(
void*t){void*_tmp9E8=Cyc_Tcutil_compress(t);_LL2538: if((unsigned int)_tmp9E8 > 3?*((
int*)_tmp9E8)== Cyc_Absyn_PointerType: 0){goto _LL2539;}else{goto _LL2540;}_LL2540:
goto _LL2541;_LL2539: return 1;_LL2541: return 0;_LL2537:;}int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp9EE=(void*)e->r;void*_tmp9F4;int _tmp9F6;_LL2544:
if(*((int*)_tmp9EE)== Cyc_Absyn_Const_e){_LL2549: _tmp9F4=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp9EE)->f1;if((unsigned int)_tmp9F4 > 1?*((int*)_tmp9F4)== Cyc_Absyn_Int_c: 0){
_LL2551: _tmp9F6=((struct Cyc_Absyn_Int_c_struct*)_tmp9F4)->f2;if(_tmp9F6 == 0){
goto _LL2545;}else{goto _LL2546;}}else{goto _LL2546;}}else{goto _LL2546;}_LL2546:
goto _LL2547;_LL2545: return 1;_LL2547: return 0;_LL2543:;}struct Cyc_Core_Opt Cyc_Tcutil_rk={(
void*)((void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)0)};struct Cyc_Core_Opt
Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(void*)((void*)
1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*
e1){if(Cyc_Tcutil_is_pointer_type(t2)? Cyc_Tcutil_is_zero(e1): 0){(void*)(e1->r=(
void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp9F8=_cycalloc(sizeof(struct
Cyc_Absyn_Const_e_struct));_tmp9F8[0]=({struct Cyc_Absyn_Const_e_struct _tmp9F9;
_tmp9F9.tag=Cyc_Absyn_Const_e;_tmp9F9.f1=(void*)((void*)Cyc_Absyn_Null_c);
_tmp9F9;});_tmp9F8;})));{struct Cyc_List_List*_tmp9FA=Cyc_Tcenv_lookup_type_vars(
te);struct Cyc_Absyn_PointerType_struct*_tmp9FB=({struct Cyc_Absyn_PointerType_struct*
_tmp9FC=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_struct));_tmp9FC[0]=({
struct Cyc_Absyn_PointerType_struct _tmp9FD;_tmp9FD.tag=Cyc_Absyn_PointerType;
_tmp9FD.f1=({struct Cyc_Absyn_PtrInfo _tmp9FE;_tmp9FE.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmpA00=_cycalloc(
sizeof(struct Cyc_Core_Opt));_tmpA00->v=_tmp9FA;_tmpA00;}));_tmp9FE.rgn_typ=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*
_tmp9FF=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp9FF->v=_tmp9FA;_tmp9FF;}));
_tmp9FE.nullable=((struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(1);
_tmp9FE.tq=Cyc_Absyn_empty_tqual();_tmp9FE.bounds=Cyc_Absyn_empty_conref();
_tmp9FE;});_tmp9FD;});_tmp9FC;});(void*)(((struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(
void*)((void*)_tmp9FB));return Cyc_Tcutil_coerce_arg(te,e1,t2);}}return 0;}static
int Cyc_Tcutil_is_sizeof_type(void*t1){void*_tmpA01=Cyc_Tcutil_compress(t1);
_LL2563: if((unsigned int)_tmpA01 > 3?*((int*)_tmpA01)== Cyc_Absyn_SizeofType: 0){
goto _LL2564;}else{goto _LL2565;}_LL2565: goto _LL2566;_LL2564: return 1;_LL2566:
return 0;_LL2562:;}static struct _tagged_arr Cyc_Tcutil_b2s(int b){return b? _tag_arr("true",
sizeof(unsigned char),5): _tag_arr("false",sizeof(unsigned char),6);}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);if(Cyc_Tcutil_unify(t1,t2)){
return 1;}if((Cyc_Tcutil_is_arithmetic_type(t1)? Cyc_Tcutil_is_arithmetic_type(t2):
0)? 1:(Cyc_Tcutil_is_sizeof_type(t1)? Cyc_Tcutil_is_arithmetic_type(t2): 0)){if(Cyc_Tcutil_will_lose_precision(
t1,t2)){({struct Cyc_Std_String_pa_struct _tmpA09;_tmpA09.tag=Cyc_Std_String_pa;
_tmpA09.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct
_tmpA08;_tmpA08.tag=Cyc_Std_String_pa;_tmpA08.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t1);{void*_tmpA07[2]={& _tmpA08,& _tmpA09};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; %s -> %s conversion supplied",
sizeof(unsigned char),53),_tag_arr(_tmpA07,sizeof(void*),2));}}});}Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e)){return 1;}else{if(Cyc_Tcutil_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2);({struct Cyc_Std_String_pa_struct
_tmpA0C;_tmpA0C.tag=Cyc_Std_String_pa;_tmpA0C.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t2);{struct Cyc_Std_String_pa_struct _tmpA0B;_tmpA0B.tag=Cyc_Std_String_pa;_tmpA0B.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmpA0A[2]={& _tmpA0B,& _tmpA0C};
Cyc_Tcutil_warn(e->loc,_tag_arr("implicit cast from %s to %s",sizeof(
unsigned char),28),_tag_arr(_tmpA0A,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
return Cyc_Tcutil_coerce_arg(te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*
_tmpA0D=Cyc_Tcutil_compress(t);_LL2575: if((unsigned int)_tmpA0D > 3?*((int*)
_tmpA0D)== Cyc_Absyn_IntType: 0){goto _LL2576;}else{goto _LL2577;}_LL2577: if(
_tmpA0D == (void*)Cyc_Absyn_FloatType){goto _LL2578;}else{goto _LL2579;}_LL2579: if((
unsigned int)_tmpA0D > 3?*((int*)_tmpA0D)== Cyc_Absyn_DoubleType: 0){goto _LL2580;}
else{goto _LL2581;}_LL2581: goto _LL2582;_LL2576: goto _LL2578;_LL2578: goto _LL2580;
_LL2580: return 1;_LL2582: return 0;_LL2574:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(
struct Cyc_List_List*inst,struct Cyc_Absyn_Structfield*x){return({struct _tuple4*
_tmpA17=_cycalloc(sizeof(struct _tuple4));_tmpA17->f1=x->tq;_tmpA17->f2=Cyc_Tcutil_substitute(
inst,(void*)x->type);_tmpA17;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct Cyc_Tcenv_Tenv*te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmpA18=t1;
struct Cyc_List_List*_tmpA24;struct Cyc_Absyn_Structdecl**_tmpA26;struct Cyc_List_List*
_tmpA28;struct _tuple1*_tmpA2A;struct Cyc_List_List*_tmpA2C;_LL2586: if(_tmpA18 == (
void*)Cyc_Absyn_VoidType){goto _LL2587;}else{goto _LL2588;}_LL2588: if((
unsigned int)_tmpA18 > 3?*((int*)_tmpA18)== Cyc_Absyn_TupleType: 0){_LL2597:
_tmpA24=((struct Cyc_Absyn_TupleType_struct*)_tmpA18)->f1;goto _LL2589;}else{goto
_LL2590;}_LL2590: if((unsigned int)_tmpA18 > 3?*((int*)_tmpA18)== Cyc_Absyn_StructType:
0){_LL2603: _tmpA2A=((struct Cyc_Absyn_StructType_struct*)_tmpA18)->f1;goto _LL2601;
_LL2601: _tmpA28=((struct Cyc_Absyn_StructType_struct*)_tmpA18)->f2;goto _LL2599;
_LL2599: _tmpA26=((struct Cyc_Absyn_StructType_struct*)_tmpA18)->f3;goto _LL2591;}
else{goto _LL2592;}_LL2592: if((unsigned int)_tmpA18 > 3?*((int*)_tmpA18)== Cyc_Absyn_AnonStructType:
0){_LL2605: _tmpA2C=((struct Cyc_Absyn_AnonStructType_struct*)_tmpA18)->f1;goto
_LL2593;}else{goto _LL2594;}_LL2594: goto _LL2595;_LL2587: return 0;_LL2589: return
_tmpA24;_LL2591: if(_tmpA26 == 0? 1: _tmpA2A == 0){return({struct Cyc_List_List*
_tmpA2E=_cycalloc(sizeof(struct Cyc_List_List));_tmpA2E->hd=({struct _tuple4*
_tmpA2F=_cycalloc(sizeof(struct _tuple4));_tmpA2F->f1=Cyc_Absyn_empty_tqual();
_tmpA2F->f2=t1;_tmpA2F;});_tmpA2E->tl=0;_tmpA2E;});}{struct Cyc_Absyn_Structdecl*
_tmpA30=*_tmpA26;struct _tuple1 _tmpA31=*_tmpA2A;struct Cyc_List_List*_tmpA32=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmpA30->tvs,_tmpA28);if(_tmpA30->fields == 0){return({struct Cyc_List_List*
_tmpA33=_cycalloc(sizeof(struct Cyc_List_List));_tmpA33->hd=({struct _tuple4*
_tmpA34=_cycalloc(sizeof(struct _tuple4));_tmpA34->f1=Cyc_Absyn_empty_tqual();
_tmpA34->f2=t1;_tmpA34;});_tmpA33->tl=0;_tmpA33;});}return((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Structfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmpA32,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpA30->fields))->v);}
_LL2593: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmpA2C);_LL2595: return({struct Cyc_List_List*_tmpA35=
_cycalloc(sizeof(struct Cyc_List_List));_tmpA35->hd=({struct _tuple4*_tmpA36=
_cycalloc(sizeof(struct _tuple4));_tmpA36->f1=Cyc_Absyn_empty_tqual();_tmpA36->f2=
t1;_tmpA36;});_tmpA35->tl=0;_tmpA35;});_LL2585:;}}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2)){return 1;}{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple8*)a->hd)).f1)? Cyc_Tcutil_unify(t2,(*((struct _tuple8*)a->hd)).f2):
0){return 1;}}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple8 _tmpA38=({struct _tuple8 _tmpA37;_tmpA37.f1=t1;_tmpA37.f2=t2;_tmpA37;});
void*_tmpA3E;struct Cyc_Absyn_PtrInfo _tmpA40;struct Cyc_Absyn_Conref*_tmpA42;
struct Cyc_Absyn_Tqual _tmpA44;struct Cyc_Absyn_Conref*_tmpA46;void*_tmpA48;void*
_tmpA4A;void*_tmpA4C;struct Cyc_Absyn_PtrInfo _tmpA4E;struct Cyc_Absyn_Conref*
_tmpA50;struct Cyc_Absyn_Tqual _tmpA52;struct Cyc_Absyn_Conref*_tmpA54;void*_tmpA56;
void*_tmpA58;_LL2618: _LL2637: _tmpA4C=_tmpA38.f1;if((unsigned int)_tmpA4C > 3?*((
int*)_tmpA4C)== Cyc_Absyn_PointerType: 0){_LL2639: _tmpA4E=((struct Cyc_Absyn_PointerType_struct*)
_tmpA4C)->f1;_LL2649: _tmpA58=(void*)_tmpA4E.elt_typ;goto _LL2647;_LL2647: _tmpA56=(
void*)_tmpA4E.rgn_typ;goto _LL2645;_LL2645: _tmpA54=_tmpA4E.nullable;goto _LL2643;
_LL2643: _tmpA52=_tmpA4E.tq;goto _LL2641;_LL2641: _tmpA50=_tmpA4E.bounds;goto
_LL2623;}else{goto _LL2620;}_LL2623: _tmpA3E=_tmpA38.f2;if((unsigned int)_tmpA3E > 
3?*((int*)_tmpA3E)== Cyc_Absyn_PointerType: 0){_LL2625: _tmpA40=((struct Cyc_Absyn_PointerType_struct*)
_tmpA3E)->f1;_LL2635: _tmpA4A=(void*)_tmpA40.elt_typ;goto _LL2633;_LL2633: _tmpA48=(
void*)_tmpA40.rgn_typ;goto _LL2631;_LL2631: _tmpA46=_tmpA40.nullable;goto _LL2629;
_LL2629: _tmpA44=_tmpA40.tq;goto _LL2627;_LL2627: _tmpA42=_tmpA40.bounds;goto
_LL2619;}else{goto _LL2620;}_LL2620: goto _LL2621;_LL2619: if(_tmpA52.q_const? !
_tmpA44.q_const: 0){return 0;}if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmpA54,
_tmpA46)?((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmpA54): 0)? !((
int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmpA46): 0){return 0;}if(!
Cyc_Tcutil_unify(_tmpA56,_tmpA48)? ! Cyc_Tcenv_region_outlives(te,_tmpA56,_tmpA48):
0){return 0;}if(! Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmpA50,_tmpA42)){
struct _tuple8 _tmpA5B=({struct _tuple8 _tmpA5A;_tmpA5A.f1=Cyc_Absyn_conref_val(
_tmpA50);_tmpA5A.f2=Cyc_Absyn_conref_val(_tmpA42);_tmpA5A;});void*_tmpA63;void*
_tmpA65;void*_tmpA67;struct Cyc_Absyn_Exp*_tmpA69;void*_tmpA6B;struct Cyc_Absyn_Exp*
_tmpA6D;_LL2653: _LL2662: _tmpA65=_tmpA5B.f1;if((unsigned int)_tmpA65 > 1?*((int*)
_tmpA65)== Cyc_Absyn_Upper_b: 0){goto _LL2660;}else{goto _LL2655;}_LL2660: _tmpA63=
_tmpA5B.f2;if(_tmpA63 == (void*)Cyc_Absyn_Unknown_b){goto _LL2654;}else{goto
_LL2655;}_LL2655: _LL2668: _tmpA6B=_tmpA5B.f1;if((unsigned int)_tmpA6B > 1?*((int*)
_tmpA6B)== Cyc_Absyn_Upper_b: 0){_LL2670: _tmpA6D=((struct Cyc_Absyn_Upper_b_struct*)
_tmpA6B)->f1;goto _LL2664;}else{goto _LL2657;}_LL2664: _tmpA67=_tmpA5B.f2;if((
unsigned int)_tmpA67 > 1?*((int*)_tmpA67)== Cyc_Absyn_Upper_b: 0){_LL2666: _tmpA69=((
struct Cyc_Absyn_Upper_b_struct*)_tmpA67)->f1;goto _LL2656;}else{goto _LL2657;}
_LL2657: goto _LL2658;_LL2654: goto _LL2652;_LL2656: if(Cyc_Evexp_eval_const_uint_exp(
_tmpA6D)< Cyc_Evexp_eval_const_uint_exp(_tmpA69)){return 0;}goto _LL2652;_LL2658:
return 0;_LL2652:;}return Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*_tmpA6F=
_cycalloc(sizeof(struct Cyc_List_List));_tmpA6F->hd=({struct _tuple8*_tmpA70=
_cycalloc(sizeof(struct _tuple8));_tmpA70->f1=t1;_tmpA70->f2=t2;_tmpA70;});
_tmpA6F->tl=assume;_tmpA6F;}),_tmpA58,_tmpA4A);_LL2621: return 0;_LL2617:;}}static
int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple8 _tmpA72=({struct _tuple8
_tmpA71;_tmpA71.f1=Cyc_Tcutil_compress(t1);_tmpA71.f2=Cyc_Tcutil_compress(t2);
_tmpA71;});void*_tmpA78;void*_tmpA7A;void*_tmpA7C;void*_tmpA7E;_LL2676: _LL2685:
_tmpA7C=_tmpA72.f1;if((unsigned int)_tmpA7C > 3?*((int*)_tmpA7C)== Cyc_Absyn_IntType:
0){_LL2687: _tmpA7E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpA7C)->f2;goto
_LL2681;}else{goto _LL2678;}_LL2681: _tmpA78=_tmpA72.f2;if((unsigned int)_tmpA78 > 
3?*((int*)_tmpA78)== Cyc_Absyn_IntType: 0){_LL2683: _tmpA7A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpA78)->f2;goto _LL2677;}else{goto _LL2678;}_LL2678: goto _LL2679;_LL2677: return
_tmpA7E == _tmpA7A;_LL2679: return 0;_LL2675:;}static int Cyc_Tcutil_ptrsubtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*
tqs1=Cyc_Tcutil_flatten_typ(te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(
te,t2);for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0){return 0;}{
struct _tuple4 _tmpA82;void*_tmpA83;struct Cyc_Absyn_Tqual _tmpA85;struct _tuple4*
_tmpA80=(struct _tuple4*)tqs1->hd;_tmpA82=*_tmpA80;_LL2694: _tmpA85=_tmpA82.f1;
goto _LL2692;_LL2692: _tmpA83=_tmpA82.f2;goto _LL2689;_LL2689: {struct _tuple4
_tmpA89;void*_tmpA8A;struct Cyc_Absyn_Tqual _tmpA8C;struct _tuple4*_tmpA87=(struct
_tuple4*)tqs2->hd;_tmpA89=*_tmpA87;_LL2701: _tmpA8C=_tmpA89.f1;goto _LL2699;
_LL2699: _tmpA8A=_tmpA89.f2;goto _LL2696;_LL2696: if(_tmpA8C.q_const? Cyc_Tcutil_subtype(
te,assume,_tmpA83,_tmpA8A): 0){continue;}else{if(Cyc_Tcutil_unify(_tmpA83,_tmpA8A)){
continue;}else{if(Cyc_Tcutil_isomorphic(_tmpA83,_tmpA8A)){continue;}else{return 0;}}}}}}
return 1;}static int Cyc_Tcutil_is_char_type(void*t){void*_tmpA8E=Cyc_Tcutil_compress(
t);void*_tmpA94;_LL2704: if((unsigned int)_tmpA8E > 3?*((int*)_tmpA8E)== Cyc_Absyn_IntType:
0){_LL2709: _tmpA94=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpA8E)->f2;if(
_tmpA94 == (void*)Cyc_Absyn_B1){goto _LL2705;}else{goto _LL2706;}}else{goto _LL2706;}
_LL2706: goto _LL2707;_LL2705: return 1;_LL2707: return 0;_LL2703:;}int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2)){return 1;}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{void*
_tmpA96=t2;struct Cyc_Absyn_Uniondecl**_tmpA9E;struct Cyc_List_List*_tmpAA0;void*
_tmpAA2;_LL2712: if((unsigned int)_tmpA96 > 3?*((int*)_tmpA96)== Cyc_Absyn_UnionType:
0){_LL2721: _tmpAA0=((struct Cyc_Absyn_UnionType_struct*)_tmpA96)->f2;goto _LL2719;
_LL2719: _tmpA9E=((struct Cyc_Absyn_UnionType_struct*)_tmpA96)->f3;goto _LL2713;}
else{goto _LL2714;}_LL2714: if((unsigned int)_tmpA96 > 3?*((int*)_tmpA96)== Cyc_Absyn_IntType:
0){_LL2723: _tmpAA2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpA96)->f2;if(
_tmpAA2 == (void*)Cyc_Absyn_B4){goto _LL2715;}else{goto _LL2716;}}else{goto _LL2716;}
_LL2716: goto _LL2717;_LL2713: if(_tmpA9E == 0){goto _LL2711;}{struct Cyc_Absyn_Uniondecl*
_tmpAA4=*_tmpA9E;if(_tmpAA4->fields == 0){goto _LL2711;}{struct Cyc_List_List*
_tmpAA5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmpAA4->tvs,_tmpAA0);{struct Cyc_List_List*fs=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmpAA4->fields))->v;for(0;fs != 0;fs=fs->tl){if(
Cyc_Tcutil_unify(t1,Cyc_Tcutil_substitute(_tmpAA5,(void*)((struct Cyc_Absyn_Structfield*)
fs->hd)->type))){return 1;}}}goto _LL2711;}}_LL2715: if(Cyc_Tcutil_typ_kind(t2)== (
void*)Cyc_Absyn_BoxKind){return 1;}goto _LL2711;_LL2717: goto _LL2711;_LL2711:;}{
void*_tmpAA6=t1;struct Cyc_Absyn_PtrInfo _tmpAB8;struct Cyc_Absyn_Conref*_tmpABA;
struct Cyc_Absyn_Tqual _tmpABC;struct Cyc_Absyn_Conref*_tmpABE;void*_tmpAC0;void*
_tmpAC2;struct Cyc_Absyn_Exp*_tmpAC4;struct Cyc_Absyn_Tqual _tmpAC6;void*_tmpAC8;
struct Cyc_Absyn_Enumdecl*_tmpACA;_LL2728: if((unsigned int)_tmpAA6 > 3?*((int*)
_tmpAA6)== Cyc_Absyn_PointerType: 0){_LL2745: _tmpAB8=((struct Cyc_Absyn_PointerType_struct*)
_tmpAA6)->f1;_LL2755: _tmpAC2=(void*)_tmpAB8.elt_typ;goto _LL2753;_LL2753: _tmpAC0=(
void*)_tmpAB8.rgn_typ;goto _LL2751;_LL2751: _tmpABE=_tmpAB8.nullable;goto _LL2749;
_LL2749: _tmpABC=_tmpAB8.tq;goto _LL2747;_LL2747: _tmpABA=_tmpAB8.bounds;goto
_LL2729;}else{goto _LL2730;}_LL2730: if((unsigned int)_tmpAA6 > 3?*((int*)_tmpAA6)
== Cyc_Absyn_ArrayType: 0){_LL2761: _tmpAC8=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmpAA6)->f1;goto _LL2759;_LL2759: _tmpAC6=((struct Cyc_Absyn_ArrayType_struct*)
_tmpAA6)->f2;goto _LL2757;_LL2757: _tmpAC4=((struct Cyc_Absyn_ArrayType_struct*)
_tmpAA6)->f3;goto _LL2731;}else{goto _LL2732;}_LL2732: if((unsigned int)_tmpAA6 > 3?*((
int*)_tmpAA6)== Cyc_Absyn_EnumType: 0){_LL2763: _tmpACA=((struct Cyc_Absyn_EnumType_struct*)
_tmpAA6)->f2;goto _LL2733;}else{goto _LL2734;}_LL2734: if((unsigned int)_tmpAA6 > 3?*((
int*)_tmpAA6)== Cyc_Absyn_IntType: 0){goto _LL2735;}else{goto _LL2736;}_LL2736: if(
_tmpAA6 == (void*)Cyc_Absyn_FloatType){goto _LL2737;}else{goto _LL2738;}_LL2738: if((
unsigned int)_tmpAA6 > 3?*((int*)_tmpAA6)== Cyc_Absyn_DoubleType: 0){goto _LL2739;}
else{goto _LL2740;}_LL2740: if(_tmpAA6 == (void*)Cyc_Absyn_VoidType){goto _LL2741;}
else{goto _LL2742;}_LL2742: goto _LL2743;_LL2729:{void*_tmpACC=t2;struct Cyc_Absyn_PtrInfo
_tmpAD4;struct Cyc_Absyn_Conref*_tmpAD6;struct Cyc_Absyn_Tqual _tmpAD8;struct Cyc_Absyn_Conref*
_tmpADA;void*_tmpADC;void*_tmpADE;_LL2766: if((unsigned int)_tmpACC > 3?*((int*)
_tmpACC)== Cyc_Absyn_PointerType: 0){_LL2773: _tmpAD4=((struct Cyc_Absyn_PointerType_struct*)
_tmpACC)->f1;_LL2783: _tmpADE=(void*)_tmpAD4.elt_typ;goto _LL2781;_LL2781: _tmpADC=(
void*)_tmpAD4.rgn_typ;goto _LL2779;_LL2779: _tmpADA=_tmpAD4.nullable;goto _LL2777;
_LL2777: _tmpAD8=_tmpAD4.tq;goto _LL2775;_LL2775: _tmpAD6=_tmpAD4.bounds;goto
_LL2767;}else{goto _LL2768;}_LL2768: if(_tmpACC == (void*)Cyc_Absyn_VoidType){goto
_LL2769;}else{goto _LL2770;}_LL2770: goto _LL2771;_LL2767: {struct Cyc_List_List*
_tmpAE0=({struct Cyc_List_List*_tmpAF2=_cycalloc(sizeof(struct Cyc_List_List));
_tmpAF2->hd=({struct _tuple8*_tmpAF3=_cycalloc(sizeof(struct _tuple8));_tmpAF3->f1=
t1;_tmpAF3->f2=t2;_tmpAF3;});_tmpAF2->tl=0;_tmpAF2;});int _tmpAE1=Cyc_Tcutil_ptrsubtype(
te,_tmpAE0,_tmpAC2,_tmpADE)? ! _tmpABC.q_const? 1: _tmpAD8.q_const: 0;int _tmpAE2=
_tmpAE1? 0:((Cyc_Tcutil_bits_only(_tmpAC2)? Cyc_Tcutil_is_char_type(_tmpADE): 0)?
_tmpAD8.q_const? 1: ! _tmpABC.q_const: 0);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmpABA,_tmpAD6);if(! bounds_ok? ! _tmpAE2: 0){struct _tuple8 _tmpAE4=({struct _tuple8
_tmpAE3;_tmpAE3.f1=Cyc_Absyn_conref_val(_tmpABA);_tmpAE3.f2=Cyc_Absyn_conref_val(
_tmpAD6);_tmpAE3;});void*_tmpAEA;struct Cyc_Absyn_Exp*_tmpAEC;void*_tmpAEE;struct
Cyc_Absyn_Exp*_tmpAF0;_LL2790: _LL2799: _tmpAEE=_tmpAE4.f1;if((unsigned int)
_tmpAEE > 1?*((int*)_tmpAEE)== Cyc_Absyn_Upper_b: 0){_LL2801: _tmpAF0=((struct Cyc_Absyn_Upper_b_struct*)
_tmpAEE)->f1;goto _LL2795;}else{goto _LL2792;}_LL2795: _tmpAEA=_tmpAE4.f2;if((
unsigned int)_tmpAEA > 1?*((int*)_tmpAEA)== Cyc_Absyn_Upper_b: 0){_LL2797: _tmpAEC=((
struct Cyc_Absyn_Upper_b_struct*)_tmpAEA)->f1;goto _LL2791;}else{goto _LL2792;}
_LL2792: goto _LL2793;_LL2791: if(Cyc_Evexp_eval_const_uint_exp(_tmpAF0)>= Cyc_Evexp_eval_const_uint_exp(
_tmpAEC)){bounds_ok=1;}goto _LL2789;_LL2793: bounds_ok=1;goto _LL2789;_LL2789:;}
return(bounds_ok? _tmpAE1? 1: _tmpAE2: 0)? Cyc_Tcutil_unify(_tmpAC0,_tmpADC)? 1: Cyc_Tcenv_region_outlives(
te,_tmpAC0,_tmpADC): 0;}_LL2769: return 1;_LL2771: goto _LL2765;_LL2765:;}return 0;
_LL2731:{void*_tmpAF4=t2;struct Cyc_Absyn_Exp*_tmpAFA;struct Cyc_Absyn_Tqual
_tmpAFC;void*_tmpAFE;_LL2806: if((unsigned int)_tmpAF4 > 3?*((int*)_tmpAF4)== Cyc_Absyn_ArrayType:
0){_LL2815: _tmpAFE=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpAF4)->f1;goto
_LL2813;_LL2813: _tmpAFC=((struct Cyc_Absyn_ArrayType_struct*)_tmpAF4)->f2;goto
_LL2811;_LL2811: _tmpAFA=((struct Cyc_Absyn_ArrayType_struct*)_tmpAF4)->f3;goto
_LL2807;}else{goto _LL2808;}_LL2808: goto _LL2809;_LL2807: {int okay;okay=(_tmpAC4 != 
0? _tmpAFA != 0: 0)? Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmpAC4))>= Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmpAFA)): 0;return(okay? Cyc_Tcutil_unify(_tmpAC8,_tmpAFE): 0)? ! _tmpAC6.q_const? 1:
_tmpAFC.q_const: 0;}_LL2809: return 0;_LL2805:;}return 0;_LL2733:{void*_tmpB00=t2;
struct Cyc_Absyn_Enumdecl*_tmpB06;_LL2818: if((unsigned int)_tmpB00 > 3?*((int*)
_tmpB00)== Cyc_Absyn_EnumType: 0){_LL2823: _tmpB06=((struct Cyc_Absyn_EnumType_struct*)
_tmpB00)->f2;goto _LL2819;}else{goto _LL2820;}_LL2820: goto _LL2821;_LL2819: if((
_tmpACA->fields != 0? _tmpB06->fields != 0: 0)?((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpACA->fields))->v)>= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmpB06->fields))->v): 0){return 1;}goto _LL2817;_LL2821: goto _LL2817;
_LL2817:;}goto _LL2735;_LL2735: goto _LL2737;_LL2737: goto _LL2739;_LL2739: return Cyc_Tcutil_coerceable(
t2)? 1: t2 == (void*)Cyc_Absyn_VoidType;_LL2741: return t2 == (void*)Cyc_Absyn_VoidType;
_LL2743: return 0;_LL2727:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e,void*t){if(! Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmpB08=Cyc_Absyn_copy_exp(e);(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmpB09=_cycalloc(
sizeof(struct Cyc_Absyn_Cast_e_struct));_tmpB09[0]=({struct Cyc_Absyn_Cast_e_struct
_tmpB0A;_tmpB0A.tag=Cyc_Absyn_Cast_e;_tmpB0A.f1=(void*)t;_tmpB0A.f2=_tmpB08;
_tmpB0A;});_tmpB09;})));e->topt=({struct Cyc_Core_Opt*_tmpB0B=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmpB0B->v=(void*)t;_tmpB0B;});}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e){void*_tmpB0C=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL2830: if((unsigned int)_tmpB0C > 3?*((int*)_tmpB0C)== 
Cyc_Absyn_EnumType: 0){goto _LL2831;}else{goto _LL2832;}_LL2832: if((unsigned int)
_tmpB0C > 3?*((int*)_tmpB0C)== Cyc_Absyn_AnonEnumType: 0){goto _LL2833;}else{goto
_LL2834;}_LL2834: if((unsigned int)_tmpB0C > 3?*((int*)_tmpB0C)== Cyc_Absyn_IntType:
0){goto _LL2835;}else{goto _LL2836;}_LL2836: if((unsigned int)_tmpB0C > 3?*((int*)
_tmpB0C)== Cyc_Absyn_Evar: 0){goto _LL2837;}else{goto _LL2838;}_LL2838: if((
unsigned int)_tmpB0C > 3?*((int*)_tmpB0C)== Cyc_Absyn_SizeofType: 0){goto _LL2839;}
else{goto _LL2840;}_LL2840: goto _LL2841;_LL2831: goto _LL2833;_LL2833: goto _LL2835;
_LL2835: return 1;_LL2837: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_t);_LL2839: return 1;_LL2841: return 0;
_LL2829:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(
e)){return 1;}{void*_tmpB1A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL2844: if(_tmpB1A == (void*)Cyc_Absyn_FloatType){goto
_LL2845;}else{goto _LL2846;}_LL2846: if((unsigned int)_tmpB1A > 3?*((int*)_tmpB1A)
== Cyc_Absyn_DoubleType: 0){goto _LL2847;}else{goto _LL2848;}_LL2848: goto _LL2849;
_LL2845: goto _LL2847;_LL2847: return 1;_LL2849: return 0;_LL2843:;}}int Cyc_Tcutil_is_function_type(
void*t){void*_tmpB22=Cyc_Tcutil_compress(t);_LL2852: if((unsigned int)_tmpB22 > 3?*((
int*)_tmpB22)== Cyc_Absyn_FnType: 0){goto _LL2853;}else{goto _LL2854;}_LL2854: goto
_LL2855;_LL2853: return 1;_LL2855: return 0;_LL2851:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2){struct _tuple8 _tmpB29=({struct _tuple8 _tmpB28;_tmpB28.f1=t1;
_tmpB28.f2=t2;_tmpB28;});void*_tmpB43;int _tmpB45;void*_tmpB47;int _tmpB49;void*
_tmpB4B;void*_tmpB4D;void*_tmpB4F;void*_tmpB51;void*_tmpB53;void*_tmpB55;void*
_tmpB57;void*_tmpB59;void*_tmpB5B;void*_tmpB5D;void*_tmpB5F;void*_tmpB61;void*
_tmpB63;void*_tmpB65;void*_tmpB67;void*_tmpB69;void*_tmpB6B;void*_tmpB6D;void*
_tmpB6F;void*_tmpB71;void*_tmpB73;void*_tmpB75;_LL2859: _LL2888: _tmpB47=_tmpB29.f1;
if((unsigned int)_tmpB47 > 3?*((int*)_tmpB47)== Cyc_Absyn_DoubleType: 0){_LL2890:
_tmpB49=((struct Cyc_Absyn_DoubleType_struct*)_tmpB47)->f1;goto _LL2884;}else{goto
_LL2861;}_LL2884: _tmpB43=_tmpB29.f2;if((unsigned int)_tmpB43 > 3?*((int*)_tmpB43)
== Cyc_Absyn_DoubleType: 0){_LL2886: _tmpB45=((struct Cyc_Absyn_DoubleType_struct*)
_tmpB43)->f1;goto _LL2860;}else{goto _LL2861;}_LL2861: _LL2892: _tmpB4B=_tmpB29.f1;
if((unsigned int)_tmpB4B > 3?*((int*)_tmpB4B)== Cyc_Absyn_DoubleType: 0){goto
_LL2862;}else{goto _LL2863;}_LL2863: _LL2894: _tmpB4D=_tmpB29.f2;if((unsigned int)
_tmpB4D > 3?*((int*)_tmpB4D)== Cyc_Absyn_DoubleType: 0){goto _LL2864;}else{goto
_LL2865;}_LL2865: _LL2896: _tmpB4F=_tmpB29.f1;if(_tmpB4F == (void*)Cyc_Absyn_FloatType){
goto _LL2866;}else{goto _LL2867;}_LL2867: _LL2898: _tmpB51=_tmpB29.f2;if(_tmpB51 == (
void*)Cyc_Absyn_FloatType){goto _LL2868;}else{goto _LL2869;}_LL2869: _LL2900:
_tmpB53=_tmpB29.f1;if((unsigned int)_tmpB53 > 3?*((int*)_tmpB53)== Cyc_Absyn_IntType:
0){_LL2904: _tmpB57=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB53)->f1;if(
_tmpB57 == (void*)Cyc_Absyn_Unsigned){goto _LL2902;}else{goto _LL2871;}_LL2902:
_tmpB55=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB53)->f2;if(_tmpB55 == (void*)
Cyc_Absyn_B8){goto _LL2870;}else{goto _LL2871;}}else{goto _LL2871;}_LL2871: _LL2906:
_tmpB59=_tmpB29.f2;if((unsigned int)_tmpB59 > 3?*((int*)_tmpB59)== Cyc_Absyn_IntType:
0){_LL2910: _tmpB5D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB59)->f1;if(
_tmpB5D == (void*)Cyc_Absyn_Unsigned){goto _LL2908;}else{goto _LL2873;}_LL2908:
_tmpB5B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB59)->f2;if(_tmpB5B == (void*)
Cyc_Absyn_B8){goto _LL2872;}else{goto _LL2873;}}else{goto _LL2873;}_LL2873: _LL2912:
_tmpB5F=_tmpB29.f1;if((unsigned int)_tmpB5F > 3?*((int*)_tmpB5F)== Cyc_Absyn_IntType:
0){_LL2916: _tmpB63=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB5F)->f1;if(
_tmpB63 == (void*)Cyc_Absyn_Signed){goto _LL2914;}else{goto _LL2875;}_LL2914:
_tmpB61=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB5F)->f2;if(_tmpB61 == (void*)
Cyc_Absyn_B8){goto _LL2874;}else{goto _LL2875;}}else{goto _LL2875;}_LL2875: _LL2918:
_tmpB65=_tmpB29.f2;if((unsigned int)_tmpB65 > 3?*((int*)_tmpB65)== Cyc_Absyn_IntType:
0){_LL2922: _tmpB69=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB65)->f1;if(
_tmpB69 == (void*)Cyc_Absyn_Signed){goto _LL2920;}else{goto _LL2877;}_LL2920:
_tmpB67=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB65)->f2;if(_tmpB67 == (void*)
Cyc_Absyn_B8){goto _LL2876;}else{goto _LL2877;}}else{goto _LL2877;}_LL2877: _LL2924:
_tmpB6B=_tmpB29.f1;if((unsigned int)_tmpB6B > 3?*((int*)_tmpB6B)== Cyc_Absyn_IntType:
0){_LL2928: _tmpB6F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB6B)->f1;if(
_tmpB6F == (void*)Cyc_Absyn_Unsigned){goto _LL2926;}else{goto _LL2879;}_LL2926:
_tmpB6D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB6B)->f2;if(_tmpB6D == (void*)
Cyc_Absyn_B4){goto _LL2878;}else{goto _LL2879;}}else{goto _LL2879;}_LL2879: _LL2930:
_tmpB71=_tmpB29.f2;if((unsigned int)_tmpB71 > 3?*((int*)_tmpB71)== Cyc_Absyn_IntType:
0){_LL2934: _tmpB75=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB71)->f1;if(
_tmpB75 == (void*)Cyc_Absyn_Unsigned){goto _LL2932;}else{goto _LL2881;}_LL2932:
_tmpB73=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB71)->f2;if(_tmpB73 == (void*)
Cyc_Absyn_B4){goto _LL2880;}else{goto _LL2881;}}else{goto _LL2881;}_LL2881: goto
_LL2882;_LL2860: if(_tmpB49){return t1;}else{return t2;}_LL2862: return t1;_LL2864:
return t2;_LL2866: goto _LL2868;_LL2868: return(void*)Cyc_Absyn_FloatType;_LL2870:
goto _LL2872;_LL2872: return Cyc_Absyn_ulonglong_t;_LL2874: goto _LL2876;_LL2876:
return Cyc_Absyn_slonglong_t;_LL2878: goto _LL2880;_LL2880: return Cyc_Absyn_uint_t;
_LL2882: return Cyc_Absyn_sint_t;_LL2858:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e){void*_tmpB77=(void*)e->r;struct Cyc_Core_Opt*_tmpB7D;
_LL2937: if(*((int*)_tmpB77)== Cyc_Absyn_AssignOp_e){_LL2942: _tmpB7D=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpB77)->f2;if(_tmpB7D == 0){goto _LL2938;}else{goto _LL2939;}}else{goto _LL2939;}
_LL2939: goto _LL2940;_LL2938:({void*_tmpB7F[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("assignment in test",
sizeof(unsigned char),19),_tag_arr(_tmpB7F,sizeof(void*),0));});goto _LL2936;
_LL2940: goto _LL2936;_LL2936:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*
c2){c1=Cyc_Absyn_compress_kb(c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple8
_tmpB81=({struct _tuple8 _tmpB80;_tmpB80.f1=c1;_tmpB80.f2=c2;_tmpB80;});void*
_tmpB8F;void*_tmpB91;void*_tmpB93;void*_tmpB95;void*_tmpB97;struct Cyc_Core_Opt*
_tmpB99;struct Cyc_Core_Opt**_tmpB9B;void*_tmpB9C;struct Cyc_Core_Opt*_tmpB9E;
struct Cyc_Core_Opt**_tmpBA0;void*_tmpBA1;void*_tmpBA3;void*_tmpBA5;void*_tmpBA7;
struct Cyc_Core_Opt*_tmpBA9;struct Cyc_Core_Opt**_tmpBAB;void*_tmpBAC;void*_tmpBAE;
struct Cyc_Core_Opt*_tmpBB0;struct Cyc_Core_Opt**_tmpBB2;void*_tmpBB3;void*_tmpBB5;
void*_tmpBB7;void*_tmpBB9;struct Cyc_Core_Opt*_tmpBBB;struct Cyc_Core_Opt**_tmpBBD;
void*_tmpBBE;void*_tmpBC0;struct Cyc_Core_Opt*_tmpBC2;struct Cyc_Core_Opt**_tmpBC4;
_LL2947: _LL2964: _tmpB93=_tmpB81.f1;if(*((int*)_tmpB93)== Cyc_Absyn_Eq_kb){
_LL2966: _tmpB95=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpB93)->f1;goto _LL2960;}
else{goto _LL2949;}_LL2960: _tmpB8F=_tmpB81.f2;if(*((int*)_tmpB8F)== Cyc_Absyn_Eq_kb){
_LL2962: _tmpB91=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpB8F)->f1;goto _LL2948;}
else{goto _LL2949;}_LL2949: _LL2968: _tmpB97=_tmpB81.f2;if(*((int*)_tmpB97)== Cyc_Absyn_Unknown_kb){
_LL2970: _tmpB99=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpB97)->f1;_tmpB9B=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmpB97)->f1;goto
_LL2950;}else{goto _LL2951;}_LL2951: _LL2973: _tmpB9C=_tmpB81.f1;if(*((int*)_tmpB9C)
== Cyc_Absyn_Unknown_kb){_LL2975: _tmpB9E=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB9C)->f1;_tmpBA0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpB9C)->f1;goto _LL2952;}else{goto _LL2953;}_LL2953: _LL2982: _tmpBA5=_tmpB81.f1;
if(*((int*)_tmpBA5)== Cyc_Absyn_Less_kb){_LL2986: _tmpBA9=((struct Cyc_Absyn_Less_kb_struct*)
_tmpBA5)->f1;_tmpBAB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpBA5)->f1;goto _LL2984;_LL2984: _tmpBA7=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpBA5)->f2;goto _LL2978;}else{goto _LL2955;}_LL2978: _tmpBA1=_tmpB81.f2;if(*((int*)
_tmpBA1)== Cyc_Absyn_Eq_kb){_LL2980: _tmpBA3=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmpBA1)->f1;goto _LL2954;}else{goto _LL2955;}_LL2955: _LL2996: _tmpBB3=_tmpB81.f1;
if(*((int*)_tmpBB3)== Cyc_Absyn_Eq_kb){_LL2998: _tmpBB5=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmpBB3)->f1;goto _LL2989;}else{goto _LL2957;}_LL2989: _tmpBAC=_tmpB81.f2;if(*((int*)
_tmpBAC)== Cyc_Absyn_Less_kb){_LL2993: _tmpBB0=((struct Cyc_Absyn_Less_kb_struct*)
_tmpBAC)->f1;_tmpBB2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpBAC)->f1;goto _LL2991;_LL2991: _tmpBAE=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpBAC)->f2;goto _LL2956;}else{goto _LL2957;}_LL2957: _LL3007: _tmpBBE=_tmpB81.f1;
if(*((int*)_tmpBBE)== Cyc_Absyn_Less_kb){_LL3011: _tmpBC2=((struct Cyc_Absyn_Less_kb_struct*)
_tmpBBE)->f1;_tmpBC4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpBBE)->f1;goto _LL3009;_LL3009: _tmpBC0=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpBBE)->f2;goto _LL3000;}else{goto _LL2946;}_LL3000: _tmpBB7=_tmpB81.f2;if(*((int*)
_tmpBB7)== Cyc_Absyn_Less_kb){_LL3004: _tmpBBB=((struct Cyc_Absyn_Less_kb_struct*)
_tmpBB7)->f1;_tmpBBD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpBB7)->f1;goto _LL3002;_LL3002: _tmpBB9=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpBB7)->f2;goto _LL2958;}else{goto _LL2946;}_LL2948: return _tmpB95 == _tmpB91;
_LL2950:*_tmpB9B=({struct Cyc_Core_Opt*_tmpBC5=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmpBC5->v=(void*)c1;_tmpBC5;});return 1;_LL2952:*_tmpBA0=({struct Cyc_Core_Opt*
_tmpBC6=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpBC6->v=(void*)c2;_tmpBC6;});
return 1;_LL2954: if(Cyc_Tcutil_kind_leq(_tmpBA3,_tmpBA7)){*_tmpBAB=({struct Cyc_Core_Opt*
_tmpBC7=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpBC7->v=(void*)c2;_tmpBC7;});
return 1;}else{return 0;}_LL2956: if(Cyc_Tcutil_kind_leq(_tmpBB5,_tmpBAE)){*_tmpBB2=({
struct Cyc_Core_Opt*_tmpBC8=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpBC8->v=(
void*)c1;_tmpBC8;});return 1;}else{return 0;}_LL2958: if(Cyc_Tcutil_kind_leq(
_tmpBC0,_tmpBB9)){*_tmpBBD=({struct Cyc_Core_Opt*_tmpBC9=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmpBC9->v=(void*)c1;_tmpBC9;});return 1;}else{if(Cyc_Tcutil_kind_leq(
_tmpBB9,_tmpBC0)){*_tmpBC4=({struct Cyc_Core_Opt*_tmpBCA=_cycalloc(sizeof(struct
Cyc_Core_Opt));_tmpBCA->v=(void*)c2;_tmpBCA;});return 1;}else{return 0;}}_LL2946:;}}
static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){return({int*
_tmpBCB=_cycalloc_atomic(sizeof(int));_tmpBCB[0]=Cyc_Tcutil_tvar_id_counter ++;
_tmpBCB;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmpBCF;_tmpBCF.tag=Cyc_Std_Int_pa;_tmpBCF.f1=(int)((
unsigned int)i);{void*_tmpBCE[1]={& _tmpBCF};Cyc_Std_aprintf(_tag_arr("#%d",
sizeof(unsigned char),4),_tag_arr(_tmpBCE,sizeof(void*),1));}});return({struct
Cyc_Absyn_Tvar*_tmpBCC=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_tmpBCC->name=({
struct _tagged_arr*_tmpBCD=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmpBCD[0]=s;
_tmpBCD;});_tmpBCC->identity=0;_tmpBCC->kind=(void*)k;_tmpBCC;});}int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _tagged_arr _tmpBD0=*t->name;return*((const
unsigned char*)_check_unknown_subscript(_tmpBD0,sizeof(unsigned char),0))== '#';}
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){({struct Cyc_Std_String_pa_struct
_tmpBD2;_tmpBD2.tag=Cyc_Std_String_pa;_tmpBD2.f1=(struct _tagged_arr)*t->name;{
void*_tmpBD1[1]={& _tmpBD2};Cyc_Std_printf(_tag_arr("%s",sizeof(unsigned char),3),
_tag_arr(_tmpBD1,sizeof(void*),1));}});if(! Cyc_Tcutil_is_temp_tvar(t)){return;}{
struct _tagged_arr _tmpBD3=Cyc_Std_strconcat(_tag_arr("`",sizeof(unsigned char),2),*
t->name);*((unsigned char*)_check_unknown_subscript(_tmpBD3,sizeof(unsigned char),
1))='t';t->name=({struct _tagged_arr*_tmpBD4=_cycalloc(sizeof(struct _tagged_arr)* 
1);_tmpBD4[0]=(struct _tagged_arr)_tmpBD3;_tmpBD4;});}}struct _tuple13{struct
_tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(
struct _tuple13*x){return({struct _tuple2*_tmpBD5=_cycalloc(sizeof(struct _tuple2));
_tmpBD5->f1=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmpBD6=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmpBD6->v=(*x).f1;_tmpBD6;});_tmpBD5->f2=(*x).f2;_tmpBD5->f3=(*
x).f3;_tmpBD5;});}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ
== 0){struct Cyc_List_List*_tmpBD7=0;{struct Cyc_List_List*atts=fd->attributes;
for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){_tmpBD7=({
struct Cyc_List_List*_tmpBD8=_cycalloc(sizeof(struct Cyc_List_List));_tmpBD8->hd=(
void*)((void*)atts->hd);_tmpBD8->tl=_tmpBD7;_tmpBD8;});}}}return(void*)({struct
Cyc_Absyn_FnType_struct*_tmpBD9=_cycalloc(sizeof(struct Cyc_Absyn_FnType_struct));
_tmpBD9[0]=({struct Cyc_Absyn_FnType_struct _tmpBDA;_tmpBDA.tag=Cyc_Absyn_FnType;
_tmpBDA.f1=({struct Cyc_Absyn_FnInfo _tmpBDB;_tmpBDB.tvars=fd->tvs;_tmpBDB.effect=
fd->effect;_tmpBDB.ret_typ=(void*)((void*)fd->ret_type);_tmpBDB.args=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args);_tmpBDB.c_varargs=fd->c_varargs;_tmpBDB.cyc_varargs=fd->cyc_varargs;
_tmpBDB.rgn_po=fd->rgn_po;_tmpBDB.attributes=_tmpBD7;_tmpBDB;});_tmpBDA;});
_tmpBD9;});}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple14{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple14*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*
t).f2;}static struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({
struct _tuple4*_tmpBDC=_cycalloc(sizeof(struct _tuple4));_tmpBDC->f1=(*pr).f1;
_tmpBDC->f2=t;_tmpBDC;});}struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;};struct _tuple16{struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmpBDD=
_region_malloc(rgn,sizeof(struct _tuple16));_tmpBDD->f1=({struct _tuple15*_tmpBDE=
_region_malloc(rgn,sizeof(struct _tuple15));_tmpBDE->f1=(*y).f1;_tmpBDE->f2=(*y).f2;
_tmpBDE;});_tmpBDD->f2=(*y).f3;_tmpBDD;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(
struct _tuple16*w){void*_tmpBE1;struct _tuple15*_tmpBE3;struct _tuple16 _tmpBDF=*w;
_LL3044: _tmpBE3=_tmpBDF.f1;goto _LL3042;_LL3042: _tmpBE1=_tmpBDF.f2;goto _LL3040;
_LL3040: {struct Cyc_Absyn_Tqual _tmpBE7;struct Cyc_Core_Opt*_tmpBE9;struct _tuple15
_tmpBE5=*_tmpBE3;_LL3050: _tmpBE9=_tmpBE5.f1;goto _LL3048;_LL3048: _tmpBE7=_tmpBE5.f2;
goto _LL3046;_LL3046: return({struct _tuple2*_tmpBEB=_cycalloc(sizeof(struct _tuple2));
_tmpBEB->f1=_tmpBE9;_tmpBEB->f2=_tmpBE7;_tmpBEB->f3=_tmpBE1;_tmpBEB;});}}static
void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Structfield*f){return(void*)f->type;}
static struct Cyc_Absyn_Structfield*Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Structfield*
f,void*t){return({struct Cyc_Absyn_Structfield*_tmpBEC=_cycalloc(sizeof(struct Cyc_Absyn_Structfield));
_tmpBEC->name=f->name;_tmpBEC->tq=f->tq;_tmpBEC->type=(void*)t;_tmpBEC->width=f->width;
_tmpBEC->attributes=f->attributes;_tmpBEC;});}static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);void*
Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
void*_tmpBED=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmpC21;struct Cyc_Absyn_Structdecl**
_tmpC23;struct Cyc_List_List*_tmpC25;struct _tuple1*_tmpC27;struct Cyc_Absyn_Uniondecl**
_tmpC29;struct Cyc_List_List*_tmpC2B;struct _tuple1*_tmpC2D;struct Cyc_Absyn_TunionInfo
_tmpC2F;void*_tmpC31;struct Cyc_List_List*_tmpC33;void*_tmpC35;struct Cyc_Absyn_TunionFieldInfo
_tmpC37;struct Cyc_List_List*_tmpC39;void*_tmpC3B;struct Cyc_Core_Opt*_tmpC3D;
struct Cyc_List_List*_tmpC3F;struct _tuple1*_tmpC41;struct Cyc_Absyn_Exp*_tmpC43;
struct Cyc_Absyn_Tqual _tmpC45;void*_tmpC47;struct Cyc_Absyn_PtrInfo _tmpC49;struct
Cyc_Absyn_Conref*_tmpC4B;struct Cyc_Absyn_Tqual _tmpC4D;struct Cyc_Absyn_Conref*
_tmpC4F;void*_tmpC51;void*_tmpC53;struct Cyc_Absyn_FnInfo _tmpC55;struct Cyc_List_List*
_tmpC57;struct Cyc_List_List*_tmpC59;struct Cyc_Absyn_VarargInfo*_tmpC5B;int
_tmpC5D;struct Cyc_List_List*_tmpC5F;void*_tmpC61;struct Cyc_Core_Opt*_tmpC63;
struct Cyc_List_List*_tmpC65;struct Cyc_List_List*_tmpC67;struct Cyc_List_List*
_tmpC69;struct Cyc_List_List*_tmpC6B;struct Cyc_Core_Opt*_tmpC6D;void*_tmpC6F;void*
_tmpC71;void*_tmpC73;void*_tmpC75;struct Cyc_List_List*_tmpC77;_LL3055: if((
unsigned int)_tmpBED > 3?*((int*)_tmpBED)== Cyc_Absyn_VarType: 0){_LL3106: _tmpC21=((
struct Cyc_Absyn_VarType_struct*)_tmpBED)->f1;goto _LL3056;}else{goto _LL3057;}
_LL3057: if((unsigned int)_tmpBED > 3?*((int*)_tmpBED)== Cyc_Absyn_StructType: 0){
_LL3112: _tmpC27=((struct Cyc_Absyn_StructType_struct*)_tmpBED)->f1;goto _LL3110;
_LL3110: _tmpC25=((struct Cyc_Absyn_StructType_struct*)_tmpBED)->f2;goto _LL3108;
_LL3108: _tmpC23=((struct Cyc_Absyn_StructType_struct*)_tmpBED)->f3;goto _LL3058;}
else{goto _LL3059;}_LL3059: if((unsigned int)_tmpBED > 3?*((int*)_tmpBED)== Cyc_Absyn_UnionType:
0){_LL3118: _tmpC2D=((struct Cyc_Absyn_UnionType_struct*)_tmpBED)->f1;goto _LL3116;
_LL3116: _tmpC2B=((struct Cyc_Absyn_UnionType_struct*)_tmpBED)->f2;goto _LL3114;
_LL3114: _tmpC29=((struct Cyc_Absyn_UnionType_struct*)_tmpBED)->f3;goto _LL3060;}
else{goto _LL3061;}_LL3061: if((unsigned int)_tmpBED > 3?*((int*)_tmpBED)== Cyc_Absyn_TunionType:
0){_LL3120: _tmpC2F=((struct Cyc_Absyn_TunionType_struct*)_tmpBED)->f1;_LL3126:
_tmpC35=(void*)_tmpC2F.tunion_info;goto _LL3124;_LL3124: _tmpC33=_tmpC2F.targs;
goto _LL3122;_LL3122: _tmpC31=(void*)_tmpC2F.rgn;goto _LL3062;}else{goto _LL3063;}
_LL3063: if((unsigned int)_tmpBED > 3?*((int*)_tmpBED)== Cyc_Absyn_TunionFieldType:
0){_LL3128: _tmpC37=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpBED)->f1;
_LL3132: _tmpC3B=(void*)_tmpC37.field_info;goto _LL3130;_LL3130: _tmpC39=_tmpC37.targs;
goto _LL3064;}else{goto _LL3065;}_LL3065: if((unsigned int)_tmpBED > 3?*((int*)
_tmpBED)== Cyc_Absyn_TypedefType: 0){_LL3138: _tmpC41=((struct Cyc_Absyn_TypedefType_struct*)
_tmpBED)->f1;goto _LL3136;_LL3136: _tmpC3F=((struct Cyc_Absyn_TypedefType_struct*)
_tmpBED)->f2;goto _LL3134;_LL3134: _tmpC3D=((struct Cyc_Absyn_TypedefType_struct*)
_tmpBED)->f3;goto _LL3066;}else{goto _LL3067;}_LL3067: if((unsigned int)_tmpBED > 3?*((
int*)_tmpBED)== Cyc_Absyn_ArrayType: 0){_LL3144: _tmpC47=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmpBED)->f1;goto _LL3142;_LL3142: _tmpC45=((struct Cyc_Absyn_ArrayType_struct*)
_tmpBED)->f2;goto _LL3140;_LL3140: _tmpC43=((struct Cyc_Absyn_ArrayType_struct*)
_tmpBED)->f3;goto _LL3068;}else{goto _LL3069;}_LL3069: if((unsigned int)_tmpBED > 3?*((
int*)_tmpBED)== Cyc_Absyn_PointerType: 0){_LL3146: _tmpC49=((struct Cyc_Absyn_PointerType_struct*)
_tmpBED)->f1;_LL3156: _tmpC53=(void*)_tmpC49.elt_typ;goto _LL3154;_LL3154: _tmpC51=(
void*)_tmpC49.rgn_typ;goto _LL3152;_LL3152: _tmpC4F=_tmpC49.nullable;goto _LL3150;
_LL3150: _tmpC4D=_tmpC49.tq;goto _LL3148;_LL3148: _tmpC4B=_tmpC49.bounds;goto
_LL3070;}else{goto _LL3071;}_LL3071: if((unsigned int)_tmpBED > 3?*((int*)_tmpBED)
== Cyc_Absyn_FnType: 0){_LL3158: _tmpC55=((struct Cyc_Absyn_FnType_struct*)_tmpBED)->f1;
_LL3174: _tmpC65=_tmpC55.tvars;goto _LL3172;_LL3172: _tmpC63=_tmpC55.effect;goto
_LL3170;_LL3170: _tmpC61=(void*)_tmpC55.ret_typ;goto _LL3168;_LL3168: _tmpC5F=
_tmpC55.args;goto _LL3166;_LL3166: _tmpC5D=_tmpC55.c_varargs;goto _LL3164;_LL3164:
_tmpC5B=_tmpC55.cyc_varargs;goto _LL3162;_LL3162: _tmpC59=_tmpC55.rgn_po;goto
_LL3160;_LL3160: _tmpC57=_tmpC55.attributes;goto _LL3072;}else{goto _LL3073;}
_LL3073: if((unsigned int)_tmpBED > 3?*((int*)_tmpBED)== Cyc_Absyn_TupleType: 0){
_LL3176: _tmpC67=((struct Cyc_Absyn_TupleType_struct*)_tmpBED)->f1;goto _LL3074;}
else{goto _LL3075;}_LL3075: if((unsigned int)_tmpBED > 3?*((int*)_tmpBED)== Cyc_Absyn_AnonStructType:
0){_LL3178: _tmpC69=((struct Cyc_Absyn_AnonStructType_struct*)_tmpBED)->f1;goto
_LL3076;}else{goto _LL3077;}_LL3077: if((unsigned int)_tmpBED > 3?*((int*)_tmpBED)
== Cyc_Absyn_AnonUnionType: 0){_LL3180: _tmpC6B=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmpBED)->f1;goto _LL3078;}else{goto _LL3079;}_LL3079: if((unsigned int)_tmpBED > 3?*((
int*)_tmpBED)== Cyc_Absyn_Evar: 0){_LL3182: _tmpC6D=((struct Cyc_Absyn_Evar_struct*)
_tmpBED)->f2;goto _LL3080;}else{goto _LL3081;}_LL3081: if((unsigned int)_tmpBED > 3?*((
int*)_tmpBED)== Cyc_Absyn_RgnHandleType: 0){_LL3184: _tmpC6F=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmpBED)->f1;goto _LL3082;}else{goto _LL3083;}_LL3083: if((unsigned int)_tmpBED > 3?*((
int*)_tmpBED)== Cyc_Absyn_SizeofType: 0){_LL3186: _tmpC71=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmpBED)->f1;goto _LL3084;}else{goto _LL3085;}_LL3085: if((unsigned int)_tmpBED > 3?*((
int*)_tmpBED)== Cyc_Absyn_EnumType: 0){goto _LL3086;}else{goto _LL3087;}_LL3087: if((
unsigned int)_tmpBED > 3?*((int*)_tmpBED)== Cyc_Absyn_AnonEnumType: 0){goto _LL3088;}
else{goto _LL3089;}_LL3089: if(_tmpBED == (void*)Cyc_Absyn_VoidType){goto _LL3090;}
else{goto _LL3091;}_LL3091: if((unsigned int)_tmpBED > 3?*((int*)_tmpBED)== Cyc_Absyn_IntType:
0){goto _LL3092;}else{goto _LL3093;}_LL3093: if(_tmpBED == (void*)Cyc_Absyn_FloatType){
goto _LL3094;}else{goto _LL3095;}_LL3095: if((unsigned int)_tmpBED > 3?*((int*)
_tmpBED)== Cyc_Absyn_DoubleType: 0){goto _LL3096;}else{goto _LL3097;}_LL3097: if(
_tmpBED == (void*)Cyc_Absyn_HeapRgn){goto _LL3098;}else{goto _LL3099;}_LL3099: if((
unsigned int)_tmpBED > 3?*((int*)_tmpBED)== Cyc_Absyn_RgnsEff: 0){_LL3188: _tmpC73=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmpBED)->f1;goto _LL3100;}else{goto
_LL3101;}_LL3101: if((unsigned int)_tmpBED > 3?*((int*)_tmpBED)== Cyc_Absyn_AccessEff:
0){_LL3190: _tmpC75=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmpBED)->f1;goto
_LL3102;}else{goto _LL3103;}_LL3103: if((unsigned int)_tmpBED > 3?*((int*)_tmpBED)
== Cyc_Absyn_JoinEff: 0){_LL3192: _tmpC77=((struct Cyc_Absyn_JoinEff_struct*)
_tmpBED)->f1;goto _LL3104;}else{goto _LL3054;}_LL3056: {struct _handler_cons _tmpC79;
_push_handler(& _tmpC79);{int _tmpC7B=0;if(setjmp(_tmpC79.handler)){_tmpC7B=1;}if(
! _tmpC7B){{void*_tmpC7C=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,
inst,_tmpC21);_npop_handler(0);return _tmpC7C;};_pop_handler();}else{void*_tmpC7A=(
void*)_exn_thrown;void*_tmpC7E=_tmpC7A;_LL3200: if(_tmpC7E == Cyc_Core_Not_found){
goto _LL3201;}else{goto _LL3202;}_LL3202: goto _LL3203;_LL3201: return t;_LL3203:(void)
_throw(_tmpC7E);_LL3199:;}}}_LL3058: {struct Cyc_List_List*_tmpC84=Cyc_Tcutil_substs(
rgn,inst,_tmpC25);return _tmpC84 == _tmpC25? t:(void*)({struct Cyc_Absyn_StructType_struct*
_tmpC85=_cycalloc(sizeof(struct Cyc_Absyn_StructType_struct));_tmpC85[0]=({struct
Cyc_Absyn_StructType_struct _tmpC86;_tmpC86.tag=Cyc_Absyn_StructType;_tmpC86.f1=
_tmpC27;_tmpC86.f2=_tmpC84;_tmpC86.f3=_tmpC23;_tmpC86;});_tmpC85;});}_LL3060: {
struct Cyc_List_List*_tmpC87=Cyc_Tcutil_substs(rgn,inst,_tmpC2B);return _tmpC87 == 
_tmpC2B? t:(void*)({struct Cyc_Absyn_UnionType_struct*_tmpC88=_cycalloc(sizeof(
struct Cyc_Absyn_UnionType_struct));_tmpC88[0]=({struct Cyc_Absyn_UnionType_struct
_tmpC89;_tmpC89.tag=Cyc_Absyn_UnionType;_tmpC89.f1=_tmpC2D;_tmpC89.f2=_tmpC87;
_tmpC89.f3=_tmpC29;_tmpC89;});_tmpC88;});}_LL3062: {struct Cyc_List_List*_tmpC8A=
Cyc_Tcutil_substs(rgn,inst,_tmpC33);void*_tmpC8B=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmpC31);return(_tmpC8A == _tmpC33? _tmpC8B == _tmpC31: 0)? t:(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpC8C=_cycalloc(sizeof(struct Cyc_Absyn_TunionType_struct));_tmpC8C[0]=({struct
Cyc_Absyn_TunionType_struct _tmpC8D;_tmpC8D.tag=Cyc_Absyn_TunionType;_tmpC8D.f1=({
struct Cyc_Absyn_TunionInfo _tmpC8E;_tmpC8E.tunion_info=(void*)_tmpC35;_tmpC8E.targs=
_tmpC8A;_tmpC8E.rgn=(void*)_tmpC8B;_tmpC8E;});_tmpC8D;});_tmpC8C;});}_LL3064: {
struct Cyc_List_List*_tmpC8F=Cyc_Tcutil_substs(rgn,inst,_tmpC39);return _tmpC8F == 
_tmpC39? t:(void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmpC90=_cycalloc(
sizeof(struct Cyc_Absyn_TunionFieldType_struct));_tmpC90[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmpC91;_tmpC91.tag=Cyc_Absyn_TunionFieldType;_tmpC91.f1=({struct Cyc_Absyn_TunionFieldInfo
_tmpC92;_tmpC92.field_info=(void*)_tmpC3B;_tmpC92.targs=_tmpC8F;_tmpC92;});
_tmpC91;});_tmpC90;});}_LL3066: {struct Cyc_List_List*_tmpC93=Cyc_Tcutil_substs(
rgn,inst,_tmpC3F);return _tmpC93 == _tmpC3F? t:(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmpC94=_cycalloc(sizeof(struct Cyc_Absyn_TypedefType_struct));_tmpC94[0]=({
struct Cyc_Absyn_TypedefType_struct _tmpC95;_tmpC95.tag=Cyc_Absyn_TypedefType;
_tmpC95.f1=_tmpC41;_tmpC95.f2=_tmpC93;_tmpC95.f3=_tmpC3D;_tmpC95;});_tmpC94;});}
_LL3068: {void*_tmpC96=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpC47);return _tmpC96 == 
_tmpC47? t:(void*)({struct Cyc_Absyn_ArrayType_struct*_tmpC97=_cycalloc(sizeof(
struct Cyc_Absyn_ArrayType_struct));_tmpC97[0]=({struct Cyc_Absyn_ArrayType_struct
_tmpC98;_tmpC98.tag=Cyc_Absyn_ArrayType;_tmpC98.f1=(void*)_tmpC96;_tmpC98.f2=
_tmpC45;_tmpC98.f3=_tmpC43;_tmpC98;});_tmpC97;});}_LL3070: {void*_tmpC99=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpC53);void*_tmpC9A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpC51);if(
_tmpC99 == _tmpC53? _tmpC9A == _tmpC51: 0){return t;}return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmpC9B=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_struct));_tmpC9B[0]=({
struct Cyc_Absyn_PointerType_struct _tmpC9C;_tmpC9C.tag=Cyc_Absyn_PointerType;
_tmpC9C.f1=({struct Cyc_Absyn_PtrInfo _tmpC9D;_tmpC9D.elt_typ=(void*)_tmpC99;
_tmpC9D.rgn_typ=(void*)_tmpC9A;_tmpC9D.nullable=_tmpC4F;_tmpC9D.tq=_tmpC4D;
_tmpC9D.bounds=_tmpC4B;_tmpC9D;});_tmpC9C;});_tmpC9B;});}_LL3072:{struct Cyc_List_List*
_tmpC9E=_tmpC65;for(0;_tmpC9E != 0;_tmpC9E=_tmpC9E->tl){inst=({struct Cyc_List_List*
_tmpC9F=_region_malloc(rgn,sizeof(struct Cyc_List_List));_tmpC9F->hd=({struct
_tuple5*_tmpCA0=_region_malloc(rgn,sizeof(struct _tuple5));_tmpCA0->f1=(struct Cyc_Absyn_Tvar*)
_tmpC9E->hd;_tmpCA0->f2=(void*)({struct Cyc_Absyn_VarType_struct*_tmpCA1=
_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));_tmpCA1[0]=({struct Cyc_Absyn_VarType_struct
_tmpCA2;_tmpCA2.tag=Cyc_Absyn_VarType;_tmpCA2.f1=(struct Cyc_Absyn_Tvar*)_tmpC9E->hd;
_tmpCA2;});_tmpCA1;});_tmpCA0;});_tmpC9F->tl=inst;_tmpC9F;});}}{struct Cyc_List_List*
_tmpCA5;struct Cyc_List_List*_tmpCA7;struct _tuple0 _tmpCA3=((struct _tuple0(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple16*(*f)(
struct _RegionHandle*,struct _tuple2*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmpC5F));_LL3240: _tmpCA7=
_tmpCA3.f1;goto _LL3238;_LL3238: _tmpCA5=_tmpCA3.f2;goto _LL3236;_LL3236: {struct
Cyc_List_List*_tmpCA9=Cyc_Tcutil_substs(rgn,inst,_tmpCA5);struct Cyc_List_List*
_tmpCAA=((struct Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple16*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpCA7,_tmpCA9));struct Cyc_Core_Opt*eff2;if(_tmpC63 == 
0){eff2=0;}else{void*_tmpCAB=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmpC63->v);
if(_tmpCAB == (void*)_tmpC63->v){eff2=_tmpC63;}else{eff2=({struct Cyc_Core_Opt*
_tmpCAC=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpCAC->v=(void*)_tmpCAB;_tmpCAC;});}}{
struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmpC5B == 0){cyc_varargs2=0;}else{int
_tmpCAF;void*_tmpCB1;struct Cyc_Absyn_Tqual _tmpCB3;struct Cyc_Core_Opt*_tmpCB5;
struct Cyc_Absyn_VarargInfo _tmpCAD=*_tmpC5B;_LL3254: _tmpCB5=_tmpCAD.name;goto
_LL3252;_LL3252: _tmpCB3=_tmpCAD.tq;goto _LL3250;_LL3250: _tmpCB1=(void*)_tmpCAD.type;
goto _LL3248;_LL3248: _tmpCAF=_tmpCAD.inject;goto _LL3246;_LL3246: {void*_tmpCB7=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmpCB1);if(_tmpCB7 == _tmpCB1){cyc_varargs2=
_tmpC5B;}else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpCB8=_cycalloc(
sizeof(struct Cyc_Absyn_VarargInfo));_tmpCB8->name=_tmpCB5;_tmpCB8->tq=_tmpCB3;
_tmpCB8->type=(void*)_tmpCB7;_tmpCB8->inject=_tmpCAF;_tmpCB8;});}}}{struct Cyc_List_List*
rgn_po2;struct Cyc_List_List*_tmpCBB;struct Cyc_List_List*_tmpCBD;struct _tuple0
_tmpCB9=Cyc_List_rsplit(rgn,rgn,_tmpC59);_LL3262: _tmpCBD=_tmpCB9.f1;goto _LL3260;
_LL3260: _tmpCBB=_tmpCB9.f2;goto _LL3258;_LL3258: {struct Cyc_List_List*_tmpCBF=Cyc_Tcutil_substs(
rgn,inst,_tmpCBD);struct Cyc_List_List*_tmpCC0=Cyc_Tcutil_substs(rgn,inst,_tmpCBB);
if(_tmpCBF == _tmpCBD? _tmpCC0 == _tmpCBB: 0){rgn_po2=_tmpC59;}else{rgn_po2=Cyc_List_zip(
_tmpCBF,_tmpCC0);}return(void*)({struct Cyc_Absyn_FnType_struct*_tmpCC1=_cycalloc(
sizeof(struct Cyc_Absyn_FnType_struct));_tmpCC1[0]=({struct Cyc_Absyn_FnType_struct
_tmpCC2;_tmpCC2.tag=Cyc_Absyn_FnType;_tmpCC2.f1=({struct Cyc_Absyn_FnInfo _tmpCC3;
_tmpCC3.tvars=_tmpC65;_tmpCC3.effect=eff2;_tmpCC3.ret_typ=(void*)Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpC61);_tmpCC3.args=_tmpCAA;_tmpCC3.c_varargs=_tmpC5D;_tmpCC3.cyc_varargs=
cyc_varargs2;_tmpCC3.rgn_po=rgn_po2;_tmpCC3.attributes=_tmpC57;_tmpCC3;});
_tmpCC2;});_tmpCC1;});}}}}}_LL3074: {struct Cyc_List_List*_tmpCC4=((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(
rgn,Cyc_Tcutil_snd_tqt,_tmpC67);struct Cyc_List_List*_tmpCC5=Cyc_Tcutil_substs(
rgn,inst,_tmpCC4);if(_tmpCC5 == _tmpCC4){return t;}{struct Cyc_List_List*_tmpCC6=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpC67,_tmpCC5);
return(void*)({struct Cyc_Absyn_TupleType_struct*_tmpCC7=_cycalloc(sizeof(struct
Cyc_Absyn_TupleType_struct));_tmpCC7[0]=({struct Cyc_Absyn_TupleType_struct
_tmpCC8;_tmpCC8.tag=Cyc_Absyn_TupleType;_tmpCC8.f1=_tmpCC6;_tmpCC8;});_tmpCC7;});}}
_LL3076: {struct Cyc_List_List*_tmpCC9=((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))
Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmpC69);struct Cyc_List_List*_tmpCCA=
Cyc_Tcutil_substs(rgn,inst,_tmpCC9);if(_tmpCCA == _tmpCC9){return t;}{struct Cyc_List_List*
_tmpCCB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*f)(struct Cyc_Absyn_Structfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmpC69,_tmpCCA);return(void*)({struct Cyc_Absyn_AnonStructType_struct*_tmpCCC=
_cycalloc(sizeof(struct Cyc_Absyn_AnonStructType_struct));_tmpCCC[0]=({struct Cyc_Absyn_AnonStructType_struct
_tmpCCD;_tmpCCD.tag=Cyc_Absyn_AnonStructType;_tmpCCD.f1=_tmpCCB;_tmpCCD;});
_tmpCCC;});}}_LL3078: {struct Cyc_List_List*_tmpCCE=((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*
x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmpC6B);struct Cyc_List_List*_tmpCCF=
Cyc_Tcutil_substs(rgn,inst,_tmpCCE);if(_tmpCCF == _tmpCCE){return t;}{struct Cyc_List_List*
_tmpCD0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*f)(struct Cyc_Absyn_Structfield*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,
_tmpC6B,_tmpCCF);return(void*)({struct Cyc_Absyn_AnonStructType_struct*_tmpCD1=
_cycalloc(sizeof(struct Cyc_Absyn_AnonStructType_struct));_tmpCD1[0]=({struct Cyc_Absyn_AnonStructType_struct
_tmpCD2;_tmpCD2.tag=Cyc_Absyn_AnonStructType;_tmpCD2.f1=_tmpCD0;_tmpCD2;});
_tmpCD1;});}}_LL3080: if(_tmpC6D != 0){return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmpC6D->v);}else{return t;}_LL3082: {void*_tmpCD3=Cyc_Tcutil_rsubstitute(rgn,
inst,_tmpC6F);return _tmpCD3 == _tmpC6F? t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmpCD4=_cycalloc(sizeof(struct Cyc_Absyn_RgnHandleType_struct));_tmpCD4[0]=({
struct Cyc_Absyn_RgnHandleType_struct _tmpCD5;_tmpCD5.tag=Cyc_Absyn_RgnHandleType;
_tmpCD5.f1=(void*)_tmpCD3;_tmpCD5;});_tmpCD4;});}_LL3084: {void*_tmpCD6=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpC71);return _tmpCD6 == _tmpC71? t:(void*)({struct Cyc_Absyn_SizeofType_struct*
_tmpCD7=_cycalloc(sizeof(struct Cyc_Absyn_SizeofType_struct));_tmpCD7[0]=({struct
Cyc_Absyn_SizeofType_struct _tmpCD8;_tmpCD8.tag=Cyc_Absyn_SizeofType;_tmpCD8.f1=(
void*)_tmpCD6;_tmpCD8;});_tmpCD7;});}_LL3086: return t;_LL3088: return t;_LL3090:
return t;_LL3092: return t;_LL3094: return t;_LL3096: return t;_LL3098: return t;_LL3100: {
void*_tmpCD9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmpC73);return _tmpCD9 == _tmpC73? t:(
void*)({struct Cyc_Absyn_RgnsEff_struct*_tmpCDA=_cycalloc(sizeof(struct Cyc_Absyn_RgnsEff_struct));
_tmpCDA[0]=({struct Cyc_Absyn_RgnsEff_struct _tmpCDB;_tmpCDB.tag=Cyc_Absyn_RgnsEff;
_tmpCDB.f1=(void*)_tmpCD9;_tmpCDB;});_tmpCDA;});}_LL3102: {void*_tmpCDC=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpC75);return _tmpCDC == _tmpC75? t:(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmpCDD=_cycalloc(sizeof(struct Cyc_Absyn_AccessEff_struct));_tmpCDD[0]=({struct
Cyc_Absyn_AccessEff_struct _tmpCDE;_tmpCDE.tag=Cyc_Absyn_AccessEff;_tmpCDE.f1=(
void*)_tmpCDC;_tmpCDE;});_tmpCDD;});}_LL3104: {struct Cyc_List_List*_tmpCDF=Cyc_Tcutil_substs(
rgn,inst,_tmpC77);return _tmpCDF == _tmpC77? t:(void*)({struct Cyc_Absyn_JoinEff_struct*
_tmpCE0=_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmpCE0[0]=({struct Cyc_Absyn_JoinEff_struct
_tmpCE1;_tmpCE1.tag=Cyc_Absyn_JoinEff;_tmpCE1.f1=_tmpCDF;_tmpCE1;});_tmpCE0;});}
_LL3054:;}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 0){return 0;}{void*
_tmpCE2=(void*)ts->hd;struct Cyc_List_List*_tmpCE3=ts->tl;void*_tmpCE4=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmpCE2);struct Cyc_List_List*_tmpCE5=Cyc_Tcutil_substs(rgn,inst,_tmpCE3);
if(_tmpCE2 == _tmpCE4? _tmpCE3 == _tmpCE5: 0){return ts;}return(struct Cyc_List_List*)({
struct Cyc_List_List*_tmpCE6=_cycalloc(sizeof(struct Cyc_List_List));_tmpCE6->hd=(
void*)_tmpCE4;_tmpCE6->tl=_tmpCE5;_tmpCE6;});}}extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct Cyc_Absyn_Exp*Cyc_Tcutil_default_initializer(struct Cyc_Tcenv_Tenv*
te,void*t,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Exp*e=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Const_e_struct*_tmpD13=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_struct));
_tmpD13[0]=({struct Cyc_Absyn_Const_e_struct _tmpD14;_tmpD14.tag=Cyc_Absyn_Const_e;
_tmpD14.f1=(void*)((void*)Cyc_Absyn_Null_c);_tmpD14;});_tmpD13;}),loc);{void*
_tmpCE7=Cyc_Tcutil_compress(t);void*_tmpCF5;void*_tmpCF7;void*_tmpCF9;void*
_tmpCFB;_LL3305: if((unsigned int)_tmpCE7 > 3?*((int*)_tmpCE7)== Cyc_Absyn_PointerType:
0){goto _LL3306;}else{goto _LL3307;}_LL3307: if((unsigned int)_tmpCE7 > 3?*((int*)
_tmpCE7)== Cyc_Absyn_IntType: 0){_LL3320: _tmpCF7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpCE7)->f1;goto _LL3318;_LL3318: _tmpCF5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpCE7)->f2;if(_tmpCF5 == (void*)Cyc_Absyn_B1){goto _LL3308;}else{goto _LL3309;}}
else{goto _LL3309;}_LL3309: if((unsigned int)_tmpCE7 > 3?*((int*)_tmpCE7)== Cyc_Absyn_IntType:
0){_LL3324: _tmpCFB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpCE7)->f1;goto
_LL3322;_LL3322: _tmpCF9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpCE7)->f2;
goto _LL3310;}else{goto _LL3311;}_LL3311: if(_tmpCE7 == (void*)Cyc_Absyn_FloatType){
goto _LL3312;}else{goto _LL3313;}_LL3313: if((unsigned int)_tmpCE7 > 3?*((int*)
_tmpCE7)== Cyc_Absyn_DoubleType: 0){goto _LL3314;}else{goto _LL3315;}_LL3315: goto
_LL3316;_LL3306: goto _LL3304;_LL3308:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmpCFD=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_struct));_tmpCFD[0]=({struct Cyc_Absyn_Const_e_struct
_tmpCFE;_tmpCFE.tag=Cyc_Absyn_Const_e;_tmpCFE.f1=(void*)((void*)({struct Cyc_Absyn_Char_c_struct*
_tmpCFF=_cycalloc(sizeof(struct Cyc_Absyn_Char_c_struct));_tmpCFF[0]=({struct Cyc_Absyn_Char_c_struct
_tmpD00;_tmpD00.tag=Cyc_Absyn_Char_c;_tmpD00.f1=(void*)((void*)Cyc_Absyn_Signed);
_tmpD00.f2='\000';_tmpD00;});_tmpCFF;}));_tmpCFE;});_tmpCFD;})));goto _LL3304;
_LL3310:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmpD01=
_cycalloc(sizeof(struct Cyc_Absyn_Const_e_struct));_tmpD01[0]=({struct Cyc_Absyn_Const_e_struct
_tmpD02;_tmpD02.tag=Cyc_Absyn_Const_e;_tmpD02.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*
_tmpD03=_cycalloc(sizeof(struct Cyc_Absyn_Int_c_struct));_tmpD03[0]=({struct Cyc_Absyn_Int_c_struct
_tmpD04;_tmpD04.tag=Cyc_Absyn_Int_c;_tmpD04.f1=(void*)_tmpCFB;_tmpD04.f2=0;
_tmpD04;});_tmpD03;}));_tmpD02;});_tmpD01;})));if(_tmpCF9 != (void*)Cyc_Absyn_B4){
e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Cast_e_struct*_tmpD05=_cycalloc(
sizeof(struct Cyc_Absyn_Cast_e_struct));_tmpD05[0]=({struct Cyc_Absyn_Cast_e_struct
_tmpD06;_tmpD06.tag=Cyc_Absyn_Cast_e;_tmpD06.f1=(void*)t;_tmpD06.f2=e;_tmpD06;});
_tmpD05;}),loc);}goto _LL3304;_LL3312:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmpD07=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_struct));_tmpD07[0]=({struct Cyc_Absyn_Const_e_struct
_tmpD08;_tmpD08.tag=Cyc_Absyn_Const_e;_tmpD08.f1=(void*)((void*)({struct Cyc_Absyn_Float_c_struct*
_tmpD09=_cycalloc(sizeof(struct Cyc_Absyn_Float_c_struct));_tmpD09[0]=({struct Cyc_Absyn_Float_c_struct
_tmpD0A;_tmpD0A.tag=Cyc_Absyn_Float_c;_tmpD0A.f1=_tag_arr("0.0",sizeof(
unsigned char),4);_tmpD0A;});_tmpD09;}));_tmpD08;});_tmpD07;})));goto _LL3304;
_LL3314:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmpD0B=
_cycalloc(sizeof(struct Cyc_Absyn_Cast_e_struct));_tmpD0B[0]=({struct Cyc_Absyn_Cast_e_struct
_tmpD0C;_tmpD0C.tag=Cyc_Absyn_Cast_e;_tmpD0C.f1=(void*)t;_tmpD0C.f2=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Const_e_struct*_tmpD0D=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_struct));
_tmpD0D[0]=({struct Cyc_Absyn_Const_e_struct _tmpD0E;_tmpD0E.tag=Cyc_Absyn_Const_e;
_tmpD0E.f1=(void*)((void*)({struct Cyc_Absyn_Float_c_struct*_tmpD0F=_cycalloc(
sizeof(struct Cyc_Absyn_Float_c_struct));_tmpD0F[0]=({struct Cyc_Absyn_Float_c_struct
_tmpD10;_tmpD10.tag=Cyc_Absyn_Float_c;_tmpD10.f1=_tag_arr("0.0",sizeof(
unsigned char),4);_tmpD10;});_tmpD0F;}));_tmpD0E;});_tmpD0D;}),loc);_tmpD0C;});
_tmpD0B;})));goto _LL3304;_LL3316:({struct Cyc_Std_String_pa_struct _tmpD12;_tmpD12.tag=
Cyc_Std_String_pa;_tmpD12.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpD11[1]={& _tmpD12};Cyc_Tcutil_terr(loc,_tag_arr("declaration of type %s requires initializer",
sizeof(unsigned char),44),_tag_arr(_tmpD11,sizeof(void*),1));}});goto _LL3304;
_LL3304:;}return e;}struct _tuple5*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,
struct Cyc_Absyn_Tvar*tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple5*
_tmpD15=_cycalloc(sizeof(struct _tuple5));_tmpD15->f1=tv;_tmpD15->f2=Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmpD16=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpD16->v=(
void*)k;_tmpD16;}),({struct Cyc_Core_Opt*_tmpD17=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmpD17->v=s;_tmpD17;}));_tmpD15;});}struct _tuple5*Cyc_Tcutil_r_make_inst_var(
struct _tuple6*env,struct Cyc_Absyn_Tvar*tv){struct _tuple6 _tmpD1A;struct
_RegionHandle*_tmpD1B;struct Cyc_List_List*_tmpD1D;struct _tuple6*_tmpD18=env;
_tmpD1A=*_tmpD18;_LL3358: _tmpD1D=_tmpD1A.f1;goto _LL3356;_LL3356: _tmpD1B=_tmpD1A.f2;
goto _LL3353;_LL3353: {void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple5*
_tmpD1F=_region_malloc(_tmpD1B,sizeof(struct _tuple5));_tmpD1F->f1=tv;_tmpD1F->f2=
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpD20=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmpD20->v=(void*)k;_tmpD20;}),({struct Cyc_Core_Opt*_tmpD21=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmpD21->v=_tmpD1D;_tmpD21;}));_tmpD1F;});}}static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_Std_zstrptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=(void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;
if(! Cyc_Tcutil_constrain_kinds(k1,k2)){({struct Cyc_Std_String_pa_struct _tmpD25;
_tmpD25.tag=Cyc_Std_String_pa;_tmpD25.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string(
k2);{struct Cyc_Std_String_pa_struct _tmpD24;_tmpD24.tag=Cyc_Std_String_pa;_tmpD24.f1=(
struct _tagged_arr)Cyc_Absynpp_kindbound2string(k1);{struct Cyc_Std_String_pa_struct
_tmpD23;_tmpD23.tag=Cyc_Std_String_pa;_tmpD23.f1=(struct _tagged_arr)*tv->name;{
void*_tmpD22[3]={& _tmpD23,& _tmpD24,& _tmpD25};Cyc_Tcutil_terr(loc,_tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof(unsigned char),59),_tag_arr(_tmpD22,sizeof(void*),3));}}}});}if(tv->identity
== 0){tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;}else{if(*((int*)
_check_null(tv->identity))!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))){({
void*_tmpD26[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("same type variable has different identity!",sizeof(unsigned char),43),
_tag_arr(_tmpD26,sizeof(void*),0));});}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmpD27=_cycalloc(sizeof(struct Cyc_List_List));
_tmpD27->hd=tv;_tmpD27->tl=tvs;_tmpD27;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0){({void*
_tmpD28[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tv",sizeof(unsigned char),39),
_tag_arr(_tmpD28,sizeof(void*),0));});}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2
!= 0;tvs2=tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0){({void*
_tmpD29[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tvs2",sizeof(unsigned char),41),
_tag_arr(_tmpD29,sizeof(void*),0));});}if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity))== *((int*)_check_null(tv->identity))){return tvs;}}}return({
struct Cyc_List_List*_tmpD2A=_cycalloc(sizeof(struct Cyc_List_List));_tmpD2A->hd=
tv;_tmpD2A->tl=tvs;_tmpD2A;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0){({struct
Cyc_Std_String_pa_struct _tmpD2C;_tmpD2C.tag=Cyc_Std_String_pa;_tmpD2C.f1=(struct
_tagged_arr)Cyc_Tcutil_tvar2string(tv);{void*_tmpD2B[1]={& _tmpD2C};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("bound tvar id for %s is NULL",
sizeof(unsigned char),29),_tag_arr(_tmpD2B,sizeof(void*),1));}});}return({struct
Cyc_List_List*_tmpD2D=_cycalloc(sizeof(struct Cyc_List_List));_tmpD2D->hd=tv;
_tmpD2D->tl=tvs;_tmpD2D;});}static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(
struct Cyc_List_List*es,void*e){void*_tmpD2E=Cyc_Tcutil_compress(e);int _tmpD34;
_LL3376: if((unsigned int)_tmpD2E > 3?*((int*)_tmpD2E)== Cyc_Absyn_Evar: 0){_LL3381:
_tmpD34=((struct Cyc_Absyn_Evar_struct*)_tmpD2E)->f3;goto _LL3377;}else{goto
_LL3378;}_LL3378: goto _LL3379;_LL3377:{struct Cyc_List_List*es2=es;for(0;es2 != 0;
es2=es2->tl){void*_tmpD36=Cyc_Tcutil_compress((void*)es2->hd);int _tmpD3C;_LL3384:
if((unsigned int)_tmpD36 > 3?*((int*)_tmpD36)== Cyc_Absyn_Evar: 0){_LL3389: _tmpD3C=((
struct Cyc_Absyn_Evar_struct*)_tmpD36)->f3;goto _LL3385;}else{goto _LL3386;}_LL3386:
goto _LL3387;_LL3385: if(_tmpD34 == _tmpD3C){return es;}goto _LL3383;_LL3387: goto
_LL3383;_LL3383:;}}return({struct Cyc_List_List*_tmpD3E=_cycalloc(sizeof(struct
Cyc_List_List));_tmpD3E->hd=(void*)e;_tmpD3E->tl=es;_tmpD3E;});_LL3379: return es;
_LL3375:;}static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*
tvs,struct Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){
int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(*((int*)
_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)_check_null(((
struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(! present){r=({
struct Cyc_List_List*_tmpD3F=_cycalloc(sizeof(struct Cyc_List_List));_tmpD3F->hd=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmpD3F->tl=r;_tmpD3F;});}}r=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*
width,struct _tagged_arr*fn){if(width != 0){unsigned int w=0;if(! Cyc_Tcutil_is_const_exp(
te,(struct Cyc_Absyn_Exp*)_check_null(width))){({struct Cyc_Std_String_pa_struct
_tmpD41;_tmpD41.tag=Cyc_Std_String_pa;_tmpD41.f1=(struct _tagged_arr)*fn;{void*
_tmpD40[1]={& _tmpD41};Cyc_Tcutil_terr(loc,_tag_arr("bitfield %s does not have constant width",
sizeof(unsigned char),41),_tag_arr(_tmpD40,sizeof(void*),1));}});}else{w=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(width));}{void*_tmpD42=Cyc_Tcutil_compress(
field_typ);void*_tmpD48;_LL3396: if((unsigned int)_tmpD42 > 3?*((int*)_tmpD42)== 
Cyc_Absyn_IntType: 0){_LL3401: _tmpD48=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpD42)->f2;goto _LL3397;}else{goto _LL3398;}_LL3398: goto _LL3399;_LL3397:{void*
_tmpD4A=_tmpD48;_LL3404: if(_tmpD4A == (void*)Cyc_Absyn_B1){goto _LL3405;}else{goto
_LL3406;}_LL3406: if(_tmpD4A == (void*)Cyc_Absyn_B2){goto _LL3407;}else{goto _LL3408;}
_LL3408: if(_tmpD4A == (void*)Cyc_Absyn_B4){goto _LL3409;}else{goto _LL3410;}_LL3410:
if(_tmpD4A == (void*)Cyc_Absyn_B8){goto _LL3411;}else{goto _LL3403;}_LL3405: if(w > 8){({
void*_tmpD54[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(unsigned char),26),_tag_arr(_tmpD54,sizeof(void*),0));});}goto _LL3403;
_LL3407: if(w > 16){({void*_tmpD55[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(unsigned char),26),_tag_arr(_tmpD55,sizeof(void*),0));});}goto _LL3403;
_LL3409: if(w > 32){({void*_tmpD56[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(unsigned char),26),_tag_arr(_tmpD56,sizeof(void*),0));});}goto _LL3403;
_LL3411: if(w > 64){({void*_tmpD57[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(unsigned char),26),_tag_arr(_tmpD57,sizeof(void*),0));});}goto _LL3403;
_LL3403:;}goto _LL3395;_LL3399:({struct Cyc_Std_String_pa_struct _tmpD5A;_tmpD5A.tag=
Cyc_Std_String_pa;_tmpD5A.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(field_typ);{
struct Cyc_Std_String_pa_struct _tmpD59;_tmpD59.tag=Cyc_Std_String_pa;_tmpD59.f1=(
struct _tagged_arr)*fn;{void*_tmpD58[2]={& _tmpD59,& _tmpD5A};Cyc_Tcutil_terr(loc,
_tag_arr("bitfield %s must have integral type but has type %s",sizeof(
unsigned char),52),_tag_arr(_tmpD58,sizeof(void*),2));}}});goto _LL3395;_LL3395:;}}}
static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct
_tagged_arr*fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*
_tmpD5B=(void*)atts->hd;_LL3421: if(_tmpD5B == (void*)Cyc_Absyn_Packed_att){goto
_LL3422;}else{goto _LL3423;}_LL3423: if((unsigned int)_tmpD5B > 16?*((int*)_tmpD5B)
== Cyc_Absyn_Aligned_att: 0){goto _LL3424;}else{goto _LL3425;}_LL3425: goto _LL3426;
_LL3422: continue;_LL3424: continue;_LL3426:({struct Cyc_Std_String_pa_struct
_tmpD65;_tmpD65.tag=Cyc_Std_String_pa;_tmpD65.f1=(struct _tagged_arr)*fn;{struct
Cyc_Std_String_pa_struct _tmpD64;_tmpD64.tag=Cyc_Std_String_pa;_tmpD64.f1=(struct
_tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd);{void*_tmpD63[2]={&
_tmpD64,& _tmpD65};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s on member %s",
sizeof(unsigned char),30),_tag_arr(_tmpD63,sizeof(void*),2));}}});goto _LL3420;
_LL3420:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*kind_env;struct Cyc_List_List*
free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,void*expected_kind,void*t){{void*_tmpD66=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*
_tmpD9A;struct Cyc_Core_Opt**_tmpD9C;struct Cyc_Core_Opt*_tmpD9D;struct Cyc_Core_Opt**
_tmpD9F;struct Cyc_Absyn_Tvar*_tmpDA0;struct Cyc_List_List*_tmpDA2;struct Cyc_Absyn_Enumdecl*
_tmpDA4;struct Cyc_Absyn_Enumdecl**_tmpDA6;struct _tuple1*_tmpDA7;struct Cyc_Absyn_TunionInfo
_tmpDA9;void*_tmpDAB;struct Cyc_List_List*_tmpDAD;struct Cyc_List_List**_tmpDAF;
void*_tmpDB0;void**_tmpDB2;struct Cyc_Absyn_TunionFieldInfo _tmpDB3;struct Cyc_List_List*
_tmpDB5;void*_tmpDB7;void**_tmpDB9;struct Cyc_Absyn_PtrInfo _tmpDBA;struct Cyc_Absyn_Conref*
_tmpDBC;struct Cyc_Absyn_Tqual _tmpDBE;struct Cyc_Absyn_Conref*_tmpDC0;void*_tmpDC2;
void*_tmpDC4;void*_tmpDC6;struct Cyc_Absyn_Exp*_tmpDC8;struct Cyc_Absyn_Tqual
_tmpDCA;void*_tmpDCC;struct Cyc_Absyn_FnInfo _tmpDCE;struct Cyc_List_List*_tmpDD0;
struct Cyc_List_List*_tmpDD2;struct Cyc_Absyn_VarargInfo*_tmpDD4;int _tmpDD6;struct
Cyc_List_List*_tmpDD8;void*_tmpDDA;struct Cyc_Core_Opt*_tmpDDC;struct Cyc_Core_Opt**
_tmpDDE;struct Cyc_List_List*_tmpDDF;struct Cyc_List_List**_tmpDE1;struct Cyc_List_List*
_tmpDE2;struct Cyc_List_List*_tmpDE4;struct Cyc_List_List*_tmpDE6;struct Cyc_Absyn_Structdecl**
_tmpDE8;struct Cyc_Absyn_Structdecl***_tmpDEA;struct Cyc_List_List*_tmpDEB;struct
Cyc_List_List**_tmpDED;struct _tuple1*_tmpDEE;struct Cyc_Absyn_Uniondecl**_tmpDF0;
struct Cyc_Absyn_Uniondecl***_tmpDF2;struct Cyc_List_List*_tmpDF3;struct Cyc_List_List**
_tmpDF5;struct _tuple1*_tmpDF6;struct Cyc_Core_Opt*_tmpDF8;struct Cyc_Core_Opt**
_tmpDFA;struct Cyc_List_List*_tmpDFB;struct Cyc_List_List**_tmpDFD;struct _tuple1*
_tmpDFE;void*_tmpE00;void*_tmpE02;void*_tmpE04;struct Cyc_List_List*_tmpE06;
_LL3432: if(_tmpD66 == (void*)Cyc_Absyn_VoidType){goto _LL3433;}else{goto _LL3434;}
_LL3434: if((unsigned int)_tmpD66 > 3?*((int*)_tmpD66)== Cyc_Absyn_Evar: 0){_LL3486:
_tmpD9D=((struct Cyc_Absyn_Evar_struct*)_tmpD66)->f1;_tmpD9F=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmpD66)->f1;goto _LL3483;_LL3483: _tmpD9A=((struct
Cyc_Absyn_Evar_struct*)_tmpD66)->f2;_tmpD9C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmpD66)->f2;goto _LL3435;}else{goto _LL3436;}_LL3436: if((unsigned int)_tmpD66 > 3?*((
int*)_tmpD66)== Cyc_Absyn_VarType: 0){_LL3489: _tmpDA0=((struct Cyc_Absyn_VarType_struct*)
_tmpD66)->f1;goto _LL3437;}else{goto _LL3438;}_LL3438: if((unsigned int)_tmpD66 > 3?*((
int*)_tmpD66)== Cyc_Absyn_AnonEnumType: 0){_LL3491: _tmpDA2=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmpD66)->f1;goto _LL3439;}else{goto _LL3440;}_LL3440: if((unsigned int)_tmpD66 > 3?*((
int*)_tmpD66)== Cyc_Absyn_EnumType: 0){_LL3496: _tmpDA7=((struct Cyc_Absyn_EnumType_struct*)
_tmpD66)->f1;goto _LL3493;_LL3493: _tmpDA4=((struct Cyc_Absyn_EnumType_struct*)
_tmpD66)->f2;_tmpDA6=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)
_tmpD66)->f2;goto _LL3441;}else{goto _LL3442;}_LL3442: if((unsigned int)_tmpD66 > 3?*((
int*)_tmpD66)== Cyc_Absyn_TunionType: 0){_LL3498: _tmpDA9=((struct Cyc_Absyn_TunionType_struct*)
_tmpD66)->f1;_LL3505: _tmpDB0=(void*)_tmpDA9.tunion_info;_tmpDB2=(void**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmpD66)->f1).tunion_info;goto _LL3502;_LL3502:
_tmpDAD=_tmpDA9.targs;_tmpDAF=(struct Cyc_List_List**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmpD66)->f1).targs;goto _LL3500;_LL3500: _tmpDAB=(void*)_tmpDA9.rgn;goto _LL3443;}
else{goto _LL3444;}_LL3444: if((unsigned int)_tmpD66 > 3?*((int*)_tmpD66)== Cyc_Absyn_TunionFieldType:
0){_LL3508: _tmpDB3=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpD66)->f1;
_LL3512: _tmpDB7=(void*)_tmpDB3.field_info;_tmpDB9=(void**)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpD66)->f1).field_info;goto _LL3510;_LL3510: _tmpDB5=_tmpDB3.targs;goto _LL3445;}
else{goto _LL3446;}_LL3446: if((unsigned int)_tmpD66 > 3?*((int*)_tmpD66)== Cyc_Absyn_PointerType:
0){_LL3515: _tmpDBA=((struct Cyc_Absyn_PointerType_struct*)_tmpD66)->f1;_LL3525:
_tmpDC4=(void*)_tmpDBA.elt_typ;goto _LL3523;_LL3523: _tmpDC2=(void*)_tmpDBA.rgn_typ;
goto _LL3521;_LL3521: _tmpDC0=_tmpDBA.nullable;goto _LL3519;_LL3519: _tmpDBE=_tmpDBA.tq;
goto _LL3517;_LL3517: _tmpDBC=_tmpDBA.bounds;goto _LL3447;}else{goto _LL3448;}
_LL3448: if((unsigned int)_tmpD66 > 3?*((int*)_tmpD66)== Cyc_Absyn_SizeofType: 0){
_LL3527: _tmpDC6=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmpD66)->f1;goto
_LL3449;}else{goto _LL3450;}_LL3450: if((unsigned int)_tmpD66 > 3?*((int*)_tmpD66)
== Cyc_Absyn_IntType: 0){goto _LL3451;}else{goto _LL3452;}_LL3452: if(_tmpD66 == (
void*)Cyc_Absyn_FloatType){goto _LL3453;}else{goto _LL3454;}_LL3454: if((
unsigned int)_tmpD66 > 3?*((int*)_tmpD66)== Cyc_Absyn_DoubleType: 0){goto _LL3455;}
else{goto _LL3456;}_LL3456: if((unsigned int)_tmpD66 > 3?*((int*)_tmpD66)== Cyc_Absyn_ArrayType:
0){_LL3533: _tmpDCC=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpD66)->f1;goto
_LL3531;_LL3531: _tmpDCA=((struct Cyc_Absyn_ArrayType_struct*)_tmpD66)->f2;goto
_LL3529;_LL3529: _tmpDC8=((struct Cyc_Absyn_ArrayType_struct*)_tmpD66)->f3;goto
_LL3457;}else{goto _LL3458;}_LL3458: if((unsigned int)_tmpD66 > 3?*((int*)_tmpD66)
== Cyc_Absyn_FnType: 0){_LL3535: _tmpDCE=((struct Cyc_Absyn_FnType_struct*)_tmpD66)->f1;
_LL3552: _tmpDDF=_tmpDCE.tvars;_tmpDE1=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmpD66)->f1).tvars;goto _LL3549;_LL3549: _tmpDDC=_tmpDCE.effect;_tmpDDE=(struct
Cyc_Core_Opt**)&(((struct Cyc_Absyn_FnType_struct*)_tmpD66)->f1).effect;goto
_LL3547;_LL3547: _tmpDDA=(void*)_tmpDCE.ret_typ;goto _LL3545;_LL3545: _tmpDD8=
_tmpDCE.args;goto _LL3543;_LL3543: _tmpDD6=_tmpDCE.c_varargs;goto _LL3541;_LL3541:
_tmpDD4=_tmpDCE.cyc_varargs;goto _LL3539;_LL3539: _tmpDD2=_tmpDCE.rgn_po;goto
_LL3537;_LL3537: _tmpDD0=_tmpDCE.attributes;goto _LL3459;}else{goto _LL3460;}
_LL3460: if((unsigned int)_tmpD66 > 3?*((int*)_tmpD66)== Cyc_Absyn_TupleType: 0){
_LL3555: _tmpDE2=((struct Cyc_Absyn_TupleType_struct*)_tmpD66)->f1;goto _LL3461;}
else{goto _LL3462;}_LL3462: if((unsigned int)_tmpD66 > 3?*((int*)_tmpD66)== Cyc_Absyn_AnonStructType:
0){_LL3557: _tmpDE4=((struct Cyc_Absyn_AnonStructType_struct*)_tmpD66)->f1;goto
_LL3463;}else{goto _LL3464;}_LL3464: if((unsigned int)_tmpD66 > 3?*((int*)_tmpD66)
== Cyc_Absyn_AnonUnionType: 0){_LL3559: _tmpDE6=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmpD66)->f1;goto _LL3465;}else{goto _LL3466;}_LL3466: if((unsigned int)_tmpD66 > 3?*((
int*)_tmpD66)== Cyc_Absyn_StructType: 0){_LL3567: _tmpDEE=((struct Cyc_Absyn_StructType_struct*)
_tmpD66)->f1;goto _LL3564;_LL3564: _tmpDEB=((struct Cyc_Absyn_StructType_struct*)
_tmpD66)->f2;_tmpDED=(struct Cyc_List_List**)&((struct Cyc_Absyn_StructType_struct*)
_tmpD66)->f2;goto _LL3561;_LL3561: _tmpDE8=((struct Cyc_Absyn_StructType_struct*)
_tmpD66)->f3;_tmpDEA=(struct Cyc_Absyn_Structdecl***)&((struct Cyc_Absyn_StructType_struct*)
_tmpD66)->f3;goto _LL3467;}else{goto _LL3468;}_LL3468: if((unsigned int)_tmpD66 > 3?*((
int*)_tmpD66)== Cyc_Absyn_UnionType: 0){_LL3575: _tmpDF6=((struct Cyc_Absyn_UnionType_struct*)
_tmpD66)->f1;goto _LL3572;_LL3572: _tmpDF3=((struct Cyc_Absyn_UnionType_struct*)
_tmpD66)->f2;_tmpDF5=(struct Cyc_List_List**)&((struct Cyc_Absyn_UnionType_struct*)
_tmpD66)->f2;goto _LL3569;_LL3569: _tmpDF0=((struct Cyc_Absyn_UnionType_struct*)
_tmpD66)->f3;_tmpDF2=(struct Cyc_Absyn_Uniondecl***)&((struct Cyc_Absyn_UnionType_struct*)
_tmpD66)->f3;goto _LL3469;}else{goto _LL3470;}_LL3470: if((unsigned int)_tmpD66 > 3?*((
int*)_tmpD66)== Cyc_Absyn_TypedefType: 0){_LL3583: _tmpDFE=((struct Cyc_Absyn_TypedefType_struct*)
_tmpD66)->f1;goto _LL3580;_LL3580: _tmpDFB=((struct Cyc_Absyn_TypedefType_struct*)
_tmpD66)->f2;_tmpDFD=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmpD66)->f2;goto _LL3577;_LL3577: _tmpDF8=((struct Cyc_Absyn_TypedefType_struct*)
_tmpD66)->f3;_tmpDFA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmpD66)->f3;goto _LL3471;}else{goto _LL3472;}_LL3472: if(_tmpD66 == (void*)Cyc_Absyn_HeapRgn){
goto _LL3473;}else{goto _LL3474;}_LL3474: if((unsigned int)_tmpD66 > 3?*((int*)
_tmpD66)== Cyc_Absyn_RgnHandleType: 0){_LL3585: _tmpE00=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmpD66)->f1;goto _LL3475;}else{goto _LL3476;}_LL3476: if((unsigned int)_tmpD66 > 3?*((
int*)_tmpD66)== Cyc_Absyn_AccessEff: 0){_LL3587: _tmpE02=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmpD66)->f1;goto _LL3477;}else{goto _LL3478;}_LL3478: if((unsigned int)_tmpD66 > 3?*((
int*)_tmpD66)== Cyc_Absyn_RgnsEff: 0){_LL3589: _tmpE04=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmpD66)->f1;goto _LL3479;}else{goto _LL3480;}_LL3480: if((unsigned int)_tmpD66 > 3?*((
int*)_tmpD66)== Cyc_Absyn_JoinEff: 0){_LL3591: _tmpE06=((struct Cyc_Absyn_JoinEff_struct*)
_tmpD66)->f1;goto _LL3481;}else{goto _LL3431;}_LL3433: goto _LL3431;_LL3435: if(*
_tmpD9F == 0){*_tmpD9F=({struct Cyc_Core_Opt*_tmpE08=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmpE08->v=(void*)expected_kind;_tmpE08;});}if((cvtenv.fn_result? cvtenv.generalize_evars:
0)? expected_kind == (void*)Cyc_Absyn_RgnKind: 0){*_tmpD9C=({struct Cyc_Core_Opt*
_tmpE09=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpE09->v=(void*)((void*)Cyc_Absyn_HeapRgn);
_tmpE09;});}else{if(cvtenv.generalize_evars){struct Cyc_Absyn_Tvar*_tmpE0A=Cyc_Tcutil_new_tvar((
void*)({struct Cyc_Absyn_Less_kb_struct*_tmpE0E=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_struct));
_tmpE0E[0]=({struct Cyc_Absyn_Less_kb_struct _tmpE0F;_tmpE0F.tag=Cyc_Absyn_Less_kb;
_tmpE0F.f1=0;_tmpE0F.f2=(void*)expected_kind;_tmpE0F;});_tmpE0E;}));*_tmpD9C=({
struct Cyc_Core_Opt*_tmpE0B=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpE0B->v=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmpE0C=_cycalloc(sizeof(struct
Cyc_Absyn_VarType_struct));_tmpE0C[0]=({struct Cyc_Absyn_VarType_struct _tmpE0D;
_tmpE0D.tag=Cyc_Absyn_VarType;_tmpE0D.f1=_tmpE0A;_tmpE0D;});_tmpE0C;}));_tmpE0B;});
_tmpDA0=_tmpE0A;goto _LL3437;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.free_evars,t);}}goto _LL3431;_LL3437:{void*_tmpE10=Cyc_Absyn_compress_kb((
void*)_tmpDA0->kind);struct Cyc_Core_Opt*_tmpE16;struct Cyc_Core_Opt**_tmpE18;
_LL3602: if(*((int*)_tmpE10)== Cyc_Absyn_Unknown_kb){_LL3607: _tmpE16=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpE10)->f1;_tmpE18=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpE10)->f1;goto _LL3603;}else{goto _LL3604;}_LL3604: goto _LL3605;_LL3603:*_tmpE18=({
struct Cyc_Core_Opt*_tmpE19=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpE19->v=(
void*)((void*)({struct Cyc_Absyn_Less_kb_struct*_tmpE1A=_cycalloc(sizeof(struct
Cyc_Absyn_Less_kb_struct));_tmpE1A[0]=({struct Cyc_Absyn_Less_kb_struct _tmpE1B;
_tmpE1B.tag=Cyc_Absyn_Less_kb;_tmpE1B.f1=0;_tmpE1B.f2=(void*)expected_kind;
_tmpE1B;});_tmpE1A;}));_tmpE19;});goto _LL3601;_LL3605: goto _LL3601;_LL3601:;}
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpDA0);cvtenv.free_vars=
Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,_tmpDA0);goto _LL3431;_LL3439: {
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle _tmpE1C=
_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpE1C;_push_region(
uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=0;for(0;
_tmpDA2 != 0;_tmpDA2=_tmpDA2->tl){struct Cyc_Absyn_Enumfield*_tmpE1D=(struct Cyc_Absyn_Enumfield*)
_tmpDA2->hd;if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),
struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,
prev_fields,(*_tmpE1D->name).f2)){({struct Cyc_Std_String_pa_struct _tmpE1F;
_tmpE1F.tag=Cyc_Std_String_pa;_tmpE1F.f1=(struct _tagged_arr)*(*_tmpE1D->name).f2;{
void*_tmpE1E[1]={& _tmpE1F};Cyc_Tcutil_terr(_tmpE1D->loc,_tag_arr("duplicate enum field name %s",
sizeof(unsigned char),29),_tag_arr(_tmpE1E,sizeof(void*),1));}});}else{
prev_fields=({struct Cyc_List_List*_tmpE20=_region_malloc(uprev_rgn,sizeof(struct
Cyc_List_List));_tmpE20->hd=(*_tmpE1D->name).f2;_tmpE20->tl=prev_fields;_tmpE20;});}
if(_tmpE1D->tag == 0){_tmpE1D->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
tag_count,_tmpE1D->loc);}else{if(! Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmpE1D->tag))){({struct Cyc_Std_String_pa_struct _tmpE22;_tmpE22.tag=
Cyc_Std_String_pa;_tmpE22.f1=(struct _tagged_arr)*(*_tmpE1D->name).f2;{void*
_tmpE21[1]={& _tmpE22};Cyc_Tcutil_terr(loc,_tag_arr("enum field %s: expression is not constant",
sizeof(unsigned char),42),_tag_arr(_tmpE21,sizeof(void*),1));}});}}{unsigned int
t1=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpE1D->tag));
tag_count=t1 + 1;(*_tmpE1D->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmpE23=_cycalloc(sizeof(struct Cyc_Absyn_Abs_n_struct));_tmpE23[0]=({struct Cyc_Absyn_Abs_n_struct
_tmpE24;_tmpE24.tag=Cyc_Absyn_Abs_n;_tmpE24.f1=te->ns;_tmpE24;});_tmpE23;});ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple17*
v))Cyc_Dict_insert)(ge->ordinaries,(*_tmpE1D->name).f2,({struct _tuple17*_tmpE25=
_cycalloc(sizeof(struct _tuple17));_tmpE25->f1=(void*)({struct Cyc_Tcenv_AnonEnumRes_struct*
_tmpE26=_cycalloc(sizeof(struct Cyc_Tcenv_AnonEnumRes_struct));_tmpE26[0]=({
struct Cyc_Tcenv_AnonEnumRes_struct _tmpE27;_tmpE27.tag=Cyc_Tcenv_AnonEnumRes;
_tmpE27.f1=(void*)t;_tmpE27.f2=_tmpE1D;_tmpE27;});_tmpE26;});_tmpE25->f2=1;
_tmpE25;}));}}};_pop_region(uprev_rgn);}goto _LL3431;}_LL3441: if(*_tmpDA6 == 0){
struct _handler_cons _tmpE28;_push_handler(& _tmpE28);{int _tmpE2A=0;if(setjmp(
_tmpE28.handler)){_tmpE2A=1;}if(! _tmpE2A){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmpDA7);*_tmpDA6=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmpE29=(void*)_exn_thrown;void*_tmpE2C=_tmpE29;_LL3630: if(_tmpE2C == Cyc_Dict_Absent){
goto _LL3631;}else{goto _LL3632;}_LL3632: goto _LL3633;_LL3631: {struct Cyc_Tcenv_Genv*
_tmpE32=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*_tmpE33=({struct Cyc_Absyn_Enumdecl*
_tmpE34=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_tmpE34->sc=(void*)((void*)
Cyc_Absyn_Extern);_tmpE34->name=_tmpDA7;_tmpE34->fields=0;_tmpE34;});Cyc_Tc_tcEnumdecl(
te,_tmpE32,loc,_tmpE33);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmpDA7);*_tmpDA6=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL3629;}}_LL3633:(
void)_throw(_tmpE2C);_LL3629:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmpDA6);*_tmpDA7=(ed->name)[_check_known_subscript_notnull(1,0)];
goto _LL3431;}_LL3443: {struct Cyc_List_List*_tmpE35=*_tmpDAF;{void*_tmpE36=*
_tmpDB2;struct Cyc_Absyn_UnknownTunionInfo _tmpE3C;int _tmpE3E;struct _tuple1*
_tmpE40;struct Cyc_Absyn_Tuniondecl**_tmpE42;struct Cyc_Absyn_Tuniondecl*_tmpE44;
_LL3640: if(*((int*)_tmpE36)== Cyc_Absyn_UnknownTunion){_LL3645: _tmpE3C=((struct
Cyc_Absyn_UnknownTunion_struct*)_tmpE36)->f1;_LL3649: _tmpE40=_tmpE3C.name;goto
_LL3647;_LL3647: _tmpE3E=_tmpE3C.is_xtunion;goto _LL3641;}else{goto _LL3642;}
_LL3642: if(*((int*)_tmpE36)== Cyc_Absyn_KnownTunion){_LL3651: _tmpE42=((struct Cyc_Absyn_KnownTunion_struct*)
_tmpE36)->f1;_tmpE44=*_tmpE42;goto _LL3643;}else{goto _LL3639;}_LL3641: {struct Cyc_Absyn_Tuniondecl**
tudp;{struct _handler_cons _tmpE45;_push_handler(& _tmpE45);{int _tmpE47=0;if(setjmp(
_tmpE45.handler)){_tmpE47=1;}if(! _tmpE47){tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmpE40);;_pop_handler();}else{void*_tmpE46=(void*)_exn_thrown;void*_tmpE49=
_tmpE46;_LL3659: if(_tmpE49 == Cyc_Dict_Absent){goto _LL3660;}else{goto _LL3661;}
_LL3661: goto _LL3662;_LL3660: {struct Cyc_Tcenv_Genv*_tmpE4F=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Tuniondecl*_tmpE50=({struct Cyc_Absyn_Tuniondecl*_tmpE54=
_cycalloc(sizeof(struct Cyc_Absyn_Tuniondecl));_tmpE54->sc=(void*)((void*)Cyc_Absyn_Extern);
_tmpE54->name=_tmpE40;_tmpE54->tvs=0;_tmpE54->fields=0;_tmpE54->is_xtunion=
_tmpE3E;_tmpE54;});Cyc_Tc_tcTuniondecl(te,_tmpE4F,loc,_tmpE50);tudp=Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmpE40);if(_tmpE35 != 0){({struct Cyc_Std_String_pa_struct _tmpE53;_tmpE53.tag=
Cyc_Std_String_pa;_tmpE53.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpE40);{
struct Cyc_Std_String_pa_struct _tmpE52;_tmpE52.tag=Cyc_Std_String_pa;_tmpE52.f1=(
struct _tagged_arr)(_tmpE3E? _tag_arr("xtunion",sizeof(unsigned char),8): _tag_arr("tunion",
sizeof(unsigned char),7));{void*_tmpE51[2]={& _tmpE52,& _tmpE53};Cyc_Tcutil_terr(
loc,_tag_arr("please declare parameterized %s %s before using",sizeof(
unsigned char),48),_tag_arr(_tmpE51,sizeof(void*),2));}}});return cvtenv;}goto
_LL3658;}_LL3662:(void)_throw(_tmpE49);_LL3658:;}}}if((*tudp)->is_xtunion != 
_tmpE3E){({struct Cyc_Std_String_pa_struct _tmpE56;_tmpE56.tag=Cyc_Std_String_pa;
_tmpE56.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpE40);{void*_tmpE55[1]={&
_tmpE56};Cyc_Tcutil_terr(loc,_tag_arr("[x]tunion is different from type declaration %s",
sizeof(unsigned char),48),_tag_arr(_tmpE55,sizeof(void*),1));}});}*_tmpDB2=(void*)({
struct Cyc_Absyn_KnownTunion_struct*_tmpE57=_cycalloc(sizeof(struct Cyc_Absyn_KnownTunion_struct));
_tmpE57[0]=({struct Cyc_Absyn_KnownTunion_struct _tmpE58;_tmpE58.tag=Cyc_Absyn_KnownTunion;
_tmpE58.f1=tudp;_tmpE58;});_tmpE57;});_tmpE44=*tudp;goto _LL3643;}_LL3643: cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)Cyc_Absyn_RgnKind,_tmpDAB);{
struct Cyc_List_List*tvs=_tmpE44->tvs;for(0;_tmpE35 != 0? tvs != 0: 0;(_tmpE35=
_tmpE35->tl,tvs=tvs->tl)){void*t1=(void*)_tmpE35->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1);}if(_tmpE35 != 0){({struct Cyc_Std_String_pa_struct _tmpE5A;_tmpE5A.tag=
Cyc_Std_String_pa;_tmpE5A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpE44->name);{
void*_tmpE59[1]={& _tmpE5A};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s",
sizeof(unsigned char),38),_tag_arr(_tmpE59,sizeof(void*),1));}});}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmpE5B=_cycalloc(sizeof(struct Cyc_List_List));_tmpE5B->hd=(void*)
e;_tmpE5B->tl=hidden_ts;_tmpE5B;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,e);}*_tmpDAF=Cyc_List_imp_append(*_tmpDAF,Cyc_List_imp_rev(hidden_ts));}
goto _LL3639;}_LL3639:;}goto _LL3431;}_LL3445:{void*_tmpE5C=*_tmpDB9;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmpE62;int _tmpE64;struct _tuple1*_tmpE66;struct _tuple1*_tmpE68;struct Cyc_Absyn_Tunionfield*
_tmpE6A;struct Cyc_Absyn_Tuniondecl*_tmpE6C;_LL3678: if(*((int*)_tmpE5C)== Cyc_Absyn_UnknownTunionfield){
_LL3683: _tmpE62=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmpE5C)->f1;
_LL3689: _tmpE68=_tmpE62.tunion_name;goto _LL3687;_LL3687: _tmpE66=_tmpE62.field_name;
goto _LL3685;_LL3685: _tmpE64=_tmpE62.is_xtunion;goto _LL3679;}else{goto _LL3680;}
_LL3680: if(*((int*)_tmpE5C)== Cyc_Absyn_KnownTunionfield){_LL3693: _tmpE6C=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmpE5C)->f1;goto _LL3691;_LL3691:
_tmpE6A=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpE5C)->f2;goto _LL3681;}
else{goto _LL3677;}_LL3679: {struct Cyc_Absyn_Tuniondecl*tud;struct Cyc_Absyn_Tunionfield*
tuf;{struct _handler_cons _tmpE6E;_push_handler(& _tmpE6E);{int _tmpE70=0;if(setjmp(
_tmpE6E.handler)){_tmpE70=1;}if(! _tmpE70){*Cyc_Tcenv_lookup_tuniondecl(te,loc,
_tmpE68);;_pop_handler();}else{void*_tmpE6F=(void*)_exn_thrown;void*_tmpE72=
_tmpE6F;_LL3700: if(_tmpE72 == Cyc_Dict_Absent){goto _LL3701;}else{goto _LL3702;}
_LL3702: goto _LL3703;_LL3701:({struct Cyc_Std_String_pa_struct _tmpE79;_tmpE79.tag=
Cyc_Std_String_pa;_tmpE79.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpE68);{
void*_tmpE78[1]={& _tmpE79};Cyc_Tcutil_terr(loc,_tag_arr("unbound type tunion %s",
sizeof(unsigned char),23),_tag_arr(_tmpE78,sizeof(void*),1));}});return cvtenv;
_LL3703:(void)_throw(_tmpE72);_LL3699:;}}}{struct _handler_cons _tmpE7A;
_push_handler(& _tmpE7A);{int _tmpE7C=0;if(setjmp(_tmpE7A.handler)){_tmpE7C=1;}if(
! _tmpE7C){{void*_tmpE7D=Cyc_Tcenv_lookup_ordinary(te,loc,_tmpE66);struct Cyc_Absyn_Tunionfield*
_tmpE83;struct Cyc_Absyn_Tuniondecl*_tmpE85;_LL3711: if(*((int*)_tmpE7D)== Cyc_Tcenv_TunionRes){
_LL3718: _tmpE85=((struct Cyc_Tcenv_TunionRes_struct*)_tmpE7D)->f1;goto _LL3716;
_LL3716: _tmpE83=((struct Cyc_Tcenv_TunionRes_struct*)_tmpE7D)->f2;goto _LL3712;}
else{goto _LL3713;}_LL3713: goto _LL3714;_LL3712: tuf=_tmpE83;tud=_tmpE85;if(tud->is_xtunion
!= _tmpE64){({struct Cyc_Std_String_pa_struct _tmpE88;_tmpE88.tag=Cyc_Std_String_pa;
_tmpE88.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpE68);{void*_tmpE87[1]={&
_tmpE88};Cyc_Tcutil_terr(loc,_tag_arr("[x]tunion is different from type declaration %s",
sizeof(unsigned char),48),_tag_arr(_tmpE87,sizeof(void*),1));}});}goto _LL3710;
_LL3714:({struct Cyc_Std_String_pa_struct _tmpE8B;_tmpE8B.tag=Cyc_Std_String_pa;
_tmpE8B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpE68);{struct Cyc_Std_String_pa_struct
_tmpE8A;_tmpE8A.tag=Cyc_Std_String_pa;_tmpE8A.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmpE66);{void*_tmpE89[2]={& _tmpE8A,& _tmpE8B};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(unsigned char),35),_tag_arr(_tmpE89,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv
_tmpE8C=cvtenv;_npop_handler(0);return _tmpE8C;}_LL3710:;};_pop_handler();}else{
void*_tmpE7B=(void*)_exn_thrown;void*_tmpE8E=_tmpE7B;_LL3728: if(_tmpE8E == Cyc_Dict_Absent){
goto _LL3729;}else{goto _LL3730;}_LL3730: goto _LL3731;_LL3729:({struct Cyc_Std_String_pa_struct
_tmpE96;_tmpE96.tag=Cyc_Std_String_pa;_tmpE96.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmpE68);{struct Cyc_Std_String_pa_struct _tmpE95;_tmpE95.tag=Cyc_Std_String_pa;
_tmpE95.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpE66);{void*_tmpE94[2]={&
_tmpE95,& _tmpE96};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(unsigned char),35),_tag_arr(_tmpE94,sizeof(void*),2));}}});return cvtenv;
_LL3731:(void)_throw(_tmpE8E);_LL3727:;}}}*_tmpDB9=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmpE97=_cycalloc(sizeof(struct Cyc_Absyn_KnownTunionfield_struct));_tmpE97[0]=({
struct Cyc_Absyn_KnownTunionfield_struct _tmpE98;_tmpE98.tag=Cyc_Absyn_KnownTunionfield;
_tmpE98.f1=tud;_tmpE98.f2=tuf;_tmpE98;});_tmpE97;});_tmpE6C=tud;_tmpE6A=tuf;goto
_LL3681;}_LL3681: {struct Cyc_List_List*tvs=_tmpE6C->tvs;for(0;_tmpDB5 != 0? tvs != 
0: 0;(_tmpDB5=_tmpDB5->tl,tvs=tvs->tl)){void*t1=(void*)_tmpDB5->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1);}if(_tmpDB5 != 0){({struct Cyc_Std_String_pa_struct _tmpE9B;_tmpE9B.tag=
Cyc_Std_String_pa;_tmpE9B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpE6A->name);{
struct Cyc_Std_String_pa_struct _tmpE9A;_tmpE9A.tag=Cyc_Std_String_pa;_tmpE9A.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpE6C->name);{void*_tmpE99[2]={&
_tmpE9A,& _tmpE9B};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s.%s",
sizeof(unsigned char),41),_tag_arr(_tmpE99,sizeof(void*),2));}}});}if(tvs != 0){({
struct Cyc_Std_String_pa_struct _tmpE9E;_tmpE9E.tag=Cyc_Std_String_pa;_tmpE9E.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpE6A->name);{struct Cyc_Std_String_pa_struct
_tmpE9D;_tmpE9D.tag=Cyc_Std_String_pa;_tmpE9D.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmpE6C->name);{void*_tmpE9C[2]={& _tmpE9D,& _tmpE9E};Cyc_Tcutil_terr(loc,_tag_arr("too few type arguments for tunion %s.%s",
sizeof(unsigned char),40),_tag_arr(_tmpE9C,sizeof(void*),2));}}});}goto _LL3677;}
_LL3677:;}goto _LL3431;_LL3447: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(
void*)Cyc_Absyn_AnyKind,_tmpDC4);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)Cyc_Absyn_RgnKind,_tmpDC2);{void*_tmpE9F=(void*)(Cyc_Absyn_compress_conref(
_tmpDBC))->v;void*_tmpEA5;struct Cyc_Absyn_Exp*_tmpEA7;_LL3745: if((unsigned int)
_tmpE9F > 1?*((int*)_tmpE9F)== Cyc_Absyn_Eq_constr: 0){_LL3750: _tmpEA5=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpE9F)->f1;if((unsigned int)_tmpEA5 > 1?*((
int*)_tmpEA5)== Cyc_Absyn_Upper_b: 0){_LL3752: _tmpEA7=((struct Cyc_Absyn_Upper_b_struct*)
_tmpEA5)->f1;goto _LL3746;}else{goto _LL3747;}}else{goto _LL3747;}_LL3747: goto
_LL3748;_LL3746: if(! Cyc_Tcutil_is_const_exp(te,_tmpEA7)){({void*_tmpEA9[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("pointer bounds expression is not a constant",
sizeof(unsigned char),44),_tag_arr(_tmpEA9,sizeof(void*),0));});}Cyc_Tcexp_tcExp(
te,0,_tmpEA7);if(! Cyc_Tcutil_coerce_uint_typ(te,_tmpEA7)){({void*_tmpEAA[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("pointer bounds expression is not an unsigned int",
sizeof(unsigned char),49),_tag_arr(_tmpEAA,sizeof(void*),0));});}Cyc_Evexp_eval_const_uint_exp(
_tmpEA7);goto _LL3744;_LL3748: goto _LL3744;_LL3744:;}goto _LL3431;_LL3449: cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)Cyc_Absyn_AnyKind,_tmpDC6);
goto _LL3431;_LL3451: goto _LL3431;_LL3453: goto _LL3431;_LL3455: goto _LL3431;_LL3457:
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)Cyc_Absyn_MemKind,
_tmpDCC);if(_tmpDC8 == 0? 1: ! Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmpDC8))){({void*_tmpEAB[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array bounds expression is not constant",
sizeof(unsigned char),40),_tag_arr(_tmpEAB,sizeof(void*),0));});}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmpDC8));if(! Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmpDC8))){({void*_tmpEAC[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("array bounds expression is not an unsigned int",sizeof(
unsigned char),47),_tag_arr(_tmpEAC,sizeof(void*),0));});}Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmpDC8));goto _LL3431;_LL3459: {int num_convs=0;
int seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(
void*)Cyc_Absyn_Printf_ft;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmpDD0 != 
0;_tmpDD0=_tmpDD0->tl){if(! Cyc_Absyn_fntype_att((void*)_tmpDD0->hd)){({struct Cyc_Std_String_pa_struct
_tmpEAE;_tmpEAE.tag=Cyc_Std_String_pa;_tmpEAE.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmpDD0->hd);{void*_tmpEAD[1]={& _tmpEAE};Cyc_Tcutil_terr(loc,_tag_arr("bad function type attribute %s",
sizeof(unsigned char),31),_tag_arr(_tmpEAD,sizeof(void*),1));}});}{void*_tmpEAF=(
void*)_tmpDD0->hd;int _tmpEBB;int _tmpEBD;void*_tmpEBF;_LL3761: if(_tmpEAF == (void*)
Cyc_Absyn_Stdcall_att){goto _LL3762;}else{goto _LL3763;}_LL3763: if(_tmpEAF == (void*)
Cyc_Absyn_Cdecl_att){goto _LL3764;}else{goto _LL3765;}_LL3765: if(_tmpEAF == (void*)
Cyc_Absyn_Fastcall_att){goto _LL3766;}else{goto _LL3767;}_LL3767: if((unsigned int)
_tmpEAF > 16?*((int*)_tmpEAF)== Cyc_Absyn_Format_att: 0){_LL3776: _tmpEBF=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmpEAF)->f1;goto _LL3774;_LL3774: _tmpEBD=((
struct Cyc_Absyn_Format_att_struct*)_tmpEAF)->f2;goto _LL3772;_LL3772: _tmpEBB=((
struct Cyc_Absyn_Format_att_struct*)_tmpEAF)->f3;goto _LL3768;}else{goto _LL3769;}
_LL3769: goto _LL3770;_LL3762: if(! seen_stdcall){seen_stdcall=1;++ num_convs;}goto
_LL3760;_LL3764: if(! seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL3760;_LL3766:
if(! seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL3760;_LL3768: if(!
seen_format){seen_format=1;ft=_tmpEBF;fmt_desc_arg=_tmpEBD;fmt_arg_start=_tmpEBB;}
else{({void*_tmpEC1[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function can't have multiple format attributes",
sizeof(unsigned char),47),_tag_arr(_tmpEC1,sizeof(void*),0));});}goto _LL3760;
_LL3770: goto _LL3760;_LL3760:;}}if(num_convs > 1){({void*_tmpEC2[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("function can't have multiple calling conventions",sizeof(
unsigned char),49),_tag_arr(_tmpEC2,sizeof(void*),0));});}Cyc_Tcutil_check_unique_tvars(
loc,*_tmpDE1);{struct Cyc_List_List*b=*_tmpDE1;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)
b->hd)->identity=Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(
cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{void*_tmpEC3=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmpEC9;_LL3781: if(*((int*)
_tmpEC3)== Cyc_Absyn_Eq_kb){_LL3786: _tmpEC9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmpEC3)->f1;if(_tmpEC9 == (void*)Cyc_Absyn_MemKind){goto _LL3782;}else{goto
_LL3783;}}else{goto _LL3783;}_LL3783: goto _LL3784;_LL3782:({struct Cyc_Std_String_pa_struct
_tmpECC;_tmpECC.tag=Cyc_Std_String_pa;_tmpECC.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)
b->hd)->name;{void*_tmpECB[1]={& _tmpECC};Cyc_Tcutil_terr(loc,_tag_arr("function attempts to abstract Mem type variable %s",
sizeof(unsigned char),51),_tag_arr(_tmpECB,sizeof(void*),1));}});goto _LL3780;
_LL3784: goto _LL3780;_LL3780:;}}}{struct Cyc_Tcutil_CVTEnv _tmpECD=({struct Cyc_Tcutil_CVTEnv
_tmpFA4;_tmpFA4.kind_env=cvtenv.kind_env;_tmpFA4.free_vars=0;_tmpFA4.free_evars=
0;_tmpFA4.generalize_evars=cvtenv.generalize_evars;_tmpFA4.fn_result=1;_tmpFA4;});
_tmpECD=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpECD,(void*)Cyc_Absyn_MemKind,
_tmpDDA);_tmpECD.fn_result=0;{struct Cyc_List_List*a=_tmpDD8;for(0;a != 0;a=a->tl){
_tmpECD=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpECD,(void*)Cyc_Absyn_MemKind,(*((
struct _tuple2*)a->hd)).f3);}}if(_tmpDD4 != 0){if(_tmpDD6){({void*_tmpECE[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("both c_vararg and cyc_vararg",
sizeof(unsigned char),29),_tag_arr(_tmpECE,sizeof(void*),0));});}{int _tmpED1;
void*_tmpED3;struct Cyc_Absyn_Tqual _tmpED5;struct Cyc_Core_Opt*_tmpED7;struct Cyc_Absyn_VarargInfo
_tmpECF=*_tmpDD4;_LL3800: _tmpED7=_tmpECF.name;goto _LL3798;_LL3798: _tmpED5=
_tmpECF.tq;goto _LL3796;_LL3796: _tmpED3=(void*)_tmpECF.type;goto _LL3794;_LL3794:
_tmpED1=_tmpECF.inject;goto _LL3792;_LL3792: _tmpECD=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmpECD,(void*)Cyc_Absyn_MemKind,_tmpED3);if(_tmpED1){void*_tmpED9=Cyc_Tcutil_compress(
_tmpED3);struct Cyc_Absyn_TunionInfo _tmpEDF;void*_tmpEE1;void*_tmpEE3;_LL3803: if((
unsigned int)_tmpED9 > 3?*((int*)_tmpED9)== Cyc_Absyn_TunionType: 0){_LL3808:
_tmpEDF=((struct Cyc_Absyn_TunionType_struct*)_tmpED9)->f1;_LL3812: _tmpEE3=(void*)
_tmpEDF.tunion_info;if(*((int*)_tmpEE3)== Cyc_Absyn_KnownTunion){goto _LL3810;}
else{goto _LL3805;}_LL3810: _tmpEE1=(void*)_tmpEDF.rgn;goto _LL3804;}else{goto
_LL3805;}_LL3805: goto _LL3806;_LL3804: goto _LL3802;_LL3806:({void*_tmpEE5[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("can't inject a non-[x]tunion type",sizeof(
unsigned char),34),_tag_arr(_tmpEE5,sizeof(void*),0));});goto _LL3802;_LL3802:;}}}
if(seen_format){int _tmpEE6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpDD8);if((((fmt_desc_arg < 0? 1: fmt_desc_arg > _tmpEE6)? 1: fmt_arg_start < 0)? 1:(
_tmpDD4 == 0? fmt_arg_start != 0: 0))? 1:(_tmpDD4 != 0? fmt_arg_start != _tmpEE6 + 1: 0)){({
void*_tmpEE7[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad format descriptor",sizeof(
unsigned char),22),_tag_arr(_tmpEE7,sizeof(void*),0));});}else{void*_tmpEEA;
struct _tuple2 _tmpEE8=*((struct _tuple2*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmpDD8,fmt_desc_arg - 1);_LL3819: _tmpEEA=_tmpEE8.f3;goto _LL3817;_LL3817:{void*
_tmpEEC=Cyc_Tcutil_compress(_tmpEEA);struct Cyc_Absyn_PtrInfo _tmpEF2;struct Cyc_Absyn_Conref*
_tmpEF4;void*_tmpEF6;_LL3822: if((unsigned int)_tmpEEC > 3?*((int*)_tmpEEC)== Cyc_Absyn_PointerType:
0){_LL3827: _tmpEF2=((struct Cyc_Absyn_PointerType_struct*)_tmpEEC)->f1;_LL3831:
_tmpEF6=(void*)_tmpEF2.elt_typ;goto _LL3829;_LL3829: _tmpEF4=_tmpEF2.bounds;goto
_LL3823;}else{goto _LL3824;}_LL3824: goto _LL3825;_LL3823:{struct _tuple8 _tmpEF9=({
struct _tuple8 _tmpEF8;_tmpEF8.f1=Cyc_Tcutil_compress(_tmpEF6);_tmpEF8.f2=Cyc_Absyn_conref_def((
void*)Cyc_Absyn_Unknown_b,_tmpEF4);_tmpEF8;});void*_tmpEFF;void*_tmpF01;void*
_tmpF03;void*_tmpF05;_LL3835: _LL3842: _tmpF01=_tmpEF9.f1;if((unsigned int)_tmpF01
> 3?*((int*)_tmpF01)== Cyc_Absyn_IntType: 0){_LL3846: _tmpF05=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpF01)->f1;if(_tmpF05 == (void*)Cyc_Absyn_Unsigned){goto _LL3844;}else{goto
_LL3837;}_LL3844: _tmpF03=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpF01)->f2;
if(_tmpF03 == (void*)Cyc_Absyn_B1){goto _LL3840;}else{goto _LL3837;}}else{goto
_LL3837;}_LL3840: _tmpEFF=_tmpEF9.f2;if(_tmpEFF == (void*)Cyc_Absyn_Unknown_b){
goto _LL3836;}else{goto _LL3837;}_LL3837: goto _LL3838;_LL3836: goto _LL3834;_LL3838:({
void*_tmpF07[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format descriptor is not a char ? type",
sizeof(unsigned char),39),_tag_arr(_tmpF07,sizeof(void*),0));});goto _LL3834;
_LL3834:;}goto _LL3821;_LL3825:({void*_tmpF08[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format descriptor is not a char ? type",
sizeof(unsigned char),39),_tag_arr(_tmpF08,sizeof(void*),0));});goto _LL3821;
_LL3821:;}if(fmt_arg_start != 0){void*_tmpF09=Cyc_Tcutil_compress((void*)((struct
Cyc_Absyn_VarargInfo*)_check_null(_tmpDD4))->type);int problem;{void*_tmpF0A=ft;
_LL3852: if(_tmpF0A == (void*)Cyc_Absyn_Printf_ft){goto _LL3853;}else{goto _LL3854;}
_LL3854: if(_tmpF0A == (void*)Cyc_Absyn_Scanf_ft){goto _LL3855;}else{goto _LL3851;}
_LL3853:{void*_tmpF10=_tmpF09;struct Cyc_Absyn_TunionInfo _tmpF16;void*_tmpF18;
struct Cyc_Absyn_Tuniondecl**_tmpF1A;struct Cyc_Absyn_Tuniondecl*_tmpF1C;_LL3858:
if((unsigned int)_tmpF10 > 3?*((int*)_tmpF10)== Cyc_Absyn_TunionType: 0){_LL3863:
_tmpF16=((struct Cyc_Absyn_TunionType_struct*)_tmpF10)->f1;_LL3865: _tmpF18=(void*)
_tmpF16.tunion_info;if(*((int*)_tmpF18)== Cyc_Absyn_KnownTunion){_LL3867: _tmpF1A=((
struct Cyc_Absyn_KnownTunion_struct*)_tmpF18)->f1;_tmpF1C=*_tmpF1A;goto _LL3859;}
else{goto _LL3860;}}else{goto _LL3860;}_LL3860: goto _LL3861;_LL3859: problem=Cyc_Absyn_qvar_cmp(
_tmpF1C->name,Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL3857;_LL3861: problem=1;
goto _LL3857;_LL3857:;}goto _LL3851;_LL3855:{void*_tmpF1D=_tmpF09;struct Cyc_Absyn_TunionInfo
_tmpF23;void*_tmpF25;struct Cyc_Absyn_Tuniondecl**_tmpF27;struct Cyc_Absyn_Tuniondecl*
_tmpF29;_LL3871: if((unsigned int)_tmpF1D > 3?*((int*)_tmpF1D)== Cyc_Absyn_TunionType:
0){_LL3876: _tmpF23=((struct Cyc_Absyn_TunionType_struct*)_tmpF1D)->f1;_LL3878:
_tmpF25=(void*)_tmpF23.tunion_info;if(*((int*)_tmpF25)== Cyc_Absyn_KnownTunion){
_LL3880: _tmpF27=((struct Cyc_Absyn_KnownTunion_struct*)_tmpF25)->f1;_tmpF29=*
_tmpF27;goto _LL3872;}else{goto _LL3873;}}else{goto _LL3873;}_LL3873: goto _LL3874;
_LL3872: problem=Cyc_Absyn_qvar_cmp(_tmpF29->name,Cyc_Absyn_tunion_scanf_arg_qvar)
!= 0;goto _LL3870;_LL3874: problem=1;goto _LL3870;_LL3870:;}goto _LL3851;_LL3851:;}
if(problem){({void*_tmpF2A[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format attribute and vararg types don't match",
sizeof(unsigned char),46),_tag_arr(_tmpF2A,sizeof(void*),0));});}}}}{struct Cyc_List_List*
rpo=_tmpDD2;for(0;rpo != 0;rpo=rpo->tl){struct _tuple8 _tmpF2D;void*_tmpF2E;void*
_tmpF30;struct _tuple8*_tmpF2B=(struct _tuple8*)rpo->hd;_tmpF2D=*_tmpF2B;_LL3889:
_tmpF30=_tmpF2D.f1;goto _LL3887;_LL3887: _tmpF2E=_tmpF2D.f2;goto _LL3884;_LL3884:
_tmpECD=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpECD,(void*)Cyc_Absyn_RgnKind,
_tmpF30);_tmpECD=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpECD,(void*)Cyc_Absyn_RgnKind,
_tmpF2E);}}if(*_tmpDDE != 0){_tmpECD=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpECD,(
void*)Cyc_Absyn_EffKind,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmpDDE))->v);}
else{if(cvtenv.generalize_evars){;}{struct Cyc_List_List*effect=0;{struct Cyc_List_List*
tvs=_tmpECD.free_vars;for(0;tvs != 0;tvs=tvs->tl){void*_tmpF32=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmpF40;struct Cyc_Core_Opt*
_tmpF42;struct Cyc_Core_Opt**_tmpF44;void*_tmpF45;void*_tmpF47;struct Cyc_Core_Opt*
_tmpF49;struct Cyc_Core_Opt**_tmpF4B;void*_tmpF4C;struct Cyc_Core_Opt*_tmpF4E;
struct Cyc_Core_Opt**_tmpF50;_LL3892: if(*((int*)_tmpF32)== Cyc_Absyn_Less_kb){
_LL3907: _tmpF42=((struct Cyc_Absyn_Less_kb_struct*)_tmpF32)->f1;_tmpF44=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpF32)->f1;goto _LL3905;
_LL3905: _tmpF40=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpF32)->f2;if(_tmpF40
== (void*)Cyc_Absyn_RgnKind){goto _LL3893;}else{goto _LL3894;}}else{goto _LL3894;}
_LL3894: if(*((int*)_tmpF32)== Cyc_Absyn_Eq_kb){_LL3910: _tmpF45=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmpF32)->f1;if(_tmpF45 == (void*)Cyc_Absyn_RgnKind){goto
_LL3895;}else{goto _LL3896;}}else{goto _LL3896;}_LL3896: if(*((int*)_tmpF32)== Cyc_Absyn_Less_kb){
_LL3914: _tmpF49=((struct Cyc_Absyn_Less_kb_struct*)_tmpF32)->f1;_tmpF4B=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpF32)->f1;goto _LL3912;
_LL3912: _tmpF47=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpF32)->f2;if(_tmpF47
== (void*)Cyc_Absyn_EffKind){goto _LL3897;}else{goto _LL3898;}}else{goto _LL3898;}
_LL3898: if(*((int*)_tmpF32)== Cyc_Absyn_Eq_kb){_LL3917: _tmpF4C=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmpF32)->f1;if(_tmpF4C == (void*)Cyc_Absyn_EffKind){goto
_LL3899;}else{goto _LL3900;}}else{goto _LL3900;}_LL3900: if(*((int*)_tmpF32)== Cyc_Absyn_Unknown_kb){
_LL3919: _tmpF4E=((struct Cyc_Absyn_Unknown_kb_struct*)_tmpF32)->f1;_tmpF50=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmpF32)->f1;goto
_LL3901;}else{goto _LL3902;}_LL3902: goto _LL3903;_LL3893:*_tmpF44=({struct Cyc_Core_Opt*
_tmpF51=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpF51->v=(void*)((void*)({struct
Cyc_Absyn_Eq_kb_struct*_tmpF52=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));
_tmpF52[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpF53;_tmpF53.tag=Cyc_Absyn_Eq_kb;
_tmpF53.f1=(void*)((void*)Cyc_Absyn_RgnKind);_tmpF53;});_tmpF52;}));_tmpF51;});
goto _LL3895;_LL3895: effect=({struct Cyc_List_List*_tmpF54=_cycalloc(sizeof(struct
Cyc_List_List));_tmpF54->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmpF55=_cycalloc(sizeof(struct Cyc_Absyn_AccessEff_struct));_tmpF55[0]=({struct
Cyc_Absyn_AccessEff_struct _tmpF56;_tmpF56.tag=Cyc_Absyn_AccessEff;_tmpF56.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmpF57=_cycalloc(sizeof(struct
Cyc_Absyn_VarType_struct));_tmpF57[0]=({struct Cyc_Absyn_VarType_struct _tmpF58;
_tmpF58.tag=Cyc_Absyn_VarType;_tmpF58.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmpF58;});
_tmpF57;}));_tmpF56;});_tmpF55;}));_tmpF54->tl=effect;_tmpF54;});goto _LL3891;
_LL3897:*_tmpF4B=({struct Cyc_Core_Opt*_tmpF59=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmpF59->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmpF5A=_cycalloc(
sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmpF5A[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmpF5B;_tmpF5B.tag=Cyc_Absyn_Eq_kb;_tmpF5B.f1=(void*)((void*)Cyc_Absyn_EffKind);
_tmpF5B;});_tmpF5A;}));_tmpF59;});goto _LL3899;_LL3899: effect=({struct Cyc_List_List*
_tmpF5C=_cycalloc(sizeof(struct Cyc_List_List));_tmpF5C->hd=(void*)((void*)({
struct Cyc_Absyn_VarType_struct*_tmpF5D=_cycalloc(sizeof(struct Cyc_Absyn_VarType_struct));
_tmpF5D[0]=({struct Cyc_Absyn_VarType_struct _tmpF5E;_tmpF5E.tag=Cyc_Absyn_VarType;
_tmpF5E.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmpF5E;});_tmpF5D;}));_tmpF5C->tl=
effect;_tmpF5C;});goto _LL3891;_LL3901:*_tmpF50=({struct Cyc_Core_Opt*_tmpF5F=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpF5F->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmpF60=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_struct));_tmpF60[0]=({struct Cyc_Absyn_Less_kb_struct
_tmpF61;_tmpF61.tag=Cyc_Absyn_Less_kb;_tmpF61.f1=0;_tmpF61.f2=(void*)((void*)Cyc_Absyn_AnyKind);
_tmpF61;});_tmpF60;}));_tmpF5F;});goto _LL3903;_LL3903: effect=({struct Cyc_List_List*
_tmpF62=_cycalloc(sizeof(struct Cyc_List_List));_tmpF62->hd=(void*)((void*)({
struct Cyc_Absyn_RgnsEff_struct*_tmpF63=_cycalloc(sizeof(struct Cyc_Absyn_RgnsEff_struct));
_tmpF63[0]=({struct Cyc_Absyn_RgnsEff_struct _tmpF64;_tmpF64.tag=Cyc_Absyn_RgnsEff;
_tmpF64.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmpF65=_cycalloc(
sizeof(struct Cyc_Absyn_VarType_struct));_tmpF65[0]=({struct Cyc_Absyn_VarType_struct
_tmpF66;_tmpF66.tag=Cyc_Absyn_VarType;_tmpF66.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;
_tmpF66;});_tmpF65;}));_tmpF64;});_tmpF63;}));_tmpF62->tl=effect;_tmpF62;});goto
_LL3891;_LL3891:;}}effect=Cyc_List_imp_rev(effect);{struct Cyc_List_List*ts=
_tmpECD.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmpF67=Cyc_Tcutil_typ_kind((
void*)ts->hd);_LL3945: if(_tmpF67 == (void*)Cyc_Absyn_RgnKind){goto _LL3946;}else{
goto _LL3947;}_LL3947: if(_tmpF67 == (void*)Cyc_Absyn_EffKind){goto _LL3948;}else{
goto _LL3949;}_LL3949: goto _LL3950;_LL3946: effect=({struct Cyc_List_List*_tmpF6F=
_cycalloc(sizeof(struct Cyc_List_List));_tmpF6F->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmpF70=_cycalloc(sizeof(struct Cyc_Absyn_AccessEff_struct));_tmpF70[0]=({struct
Cyc_Absyn_AccessEff_struct _tmpF71;_tmpF71.tag=Cyc_Absyn_AccessEff;_tmpF71.f1=(
void*)((void*)ts->hd);_tmpF71;});_tmpF70;}));_tmpF6F->tl=effect;_tmpF6F;});goto
_LL3944;_LL3948: effect=({struct Cyc_List_List*_tmpF72=_cycalloc(sizeof(struct Cyc_List_List));
_tmpF72->hd=(void*)((void*)ts->hd);_tmpF72->tl=effect;_tmpF72;});goto _LL3944;
_LL3950: effect=({struct Cyc_List_List*_tmpF73=_cycalloc(sizeof(struct Cyc_List_List));
_tmpF73->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmpF74=_cycalloc(
sizeof(struct Cyc_Absyn_RgnsEff_struct));_tmpF74[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmpF75;_tmpF75.tag=Cyc_Absyn_RgnsEff;_tmpF75.f1=(void*)((void*)ts->hd);_tmpF75;});
_tmpF74;}));_tmpF73->tl=effect;_tmpF73;});goto _LL3944;_LL3944:;}}*_tmpDDE=({
struct Cyc_Core_Opt*_tmpF76=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpF76->v=(
void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmpF77=_cycalloc(sizeof(struct
Cyc_Absyn_JoinEff_struct));_tmpF77[0]=({struct Cyc_Absyn_JoinEff_struct _tmpF78;
_tmpF78.tag=Cyc_Absyn_JoinEff;_tmpF78.f1=Cyc_List_imp_rev(effect);_tmpF78;});
_tmpF77;}));_tmpF76;});}}if(*_tmpDE1 != 0){struct Cyc_List_List*bs=*_tmpDE1;for(0;
bs != 0;bs=bs->tl){void*_tmpF79=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)
bs->hd)->kind);struct Cyc_Core_Opt*_tmpF87;struct Cyc_Core_Opt**_tmpF89;void*
_tmpF8A;struct Cyc_Core_Opt*_tmpF8C;struct Cyc_Core_Opt**_tmpF8E;void*_tmpF8F;
struct Cyc_Core_Opt*_tmpF91;struct Cyc_Core_Opt**_tmpF93;void*_tmpF94;struct Cyc_Core_Opt*
_tmpF96;struct Cyc_Core_Opt**_tmpF98;void*_tmpF99;_LL3963: if(*((int*)_tmpF79)== 
Cyc_Absyn_Unknown_kb){_LL3976: _tmpF87=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpF79)->f1;_tmpF89=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpF79)->f1;goto _LL3964;}else{goto _LL3965;}_LL3965: if(*((int*)_tmpF79)== Cyc_Absyn_Less_kb){
_LL3981: _tmpF8C=((struct Cyc_Absyn_Less_kb_struct*)_tmpF79)->f1;_tmpF8E=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpF79)->f1;goto _LL3979;
_LL3979: _tmpF8A=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpF79)->f2;if(_tmpF8A
== (void*)Cyc_Absyn_MemKind){goto _LL3966;}else{goto _LL3967;}}else{goto _LL3967;}
_LL3967: if(*((int*)_tmpF79)== Cyc_Absyn_Less_kb){_LL3986: _tmpF91=((struct Cyc_Absyn_Less_kb_struct*)
_tmpF79)->f1;_tmpF93=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmpF79)->f1;goto _LL3984;_LL3984: _tmpF8F=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpF79)->f2;if(_tmpF8F == (void*)Cyc_Absyn_AnyKind){goto _LL3968;}else{goto
_LL3969;}}else{goto _LL3969;}_LL3969: if(*((int*)_tmpF79)== Cyc_Absyn_Less_kb){
_LL3991: _tmpF96=((struct Cyc_Absyn_Less_kb_struct*)_tmpF79)->f1;_tmpF98=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpF79)->f1;goto _LL3989;
_LL3989: _tmpF94=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpF79)->f2;goto
_LL3970;}else{goto _LL3971;}_LL3971: if(*((int*)_tmpF79)== Cyc_Absyn_Eq_kb){
_LL3994: _tmpF99=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpF79)->f1;if(_tmpF99 == (
void*)Cyc_Absyn_MemKind){goto _LL3972;}else{goto _LL3973;}}else{goto _LL3973;}
_LL3973: goto _LL3974;_LL3964:({struct Cyc_Std_String_pa_struct _tmpF9C;_tmpF9C.tag=
Cyc_Std_String_pa;_tmpF9C.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name;{
void*_tmpF9B[1]={& _tmpF9C};Cyc_Tcutil_warn(loc,_tag_arr("Type variable %s unconstrained, assuming boxed",
sizeof(unsigned char),47),_tag_arr(_tmpF9B,sizeof(void*),1));}});_tmpF8E=_tmpF89;
goto _LL3966;_LL3966: _tmpF93=_tmpF8E;goto _LL3968;_LL3968:*_tmpF93=({struct Cyc_Core_Opt*
_tmpF9D=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpF9D->v=(void*)((void*)({struct
Cyc_Absyn_Eq_kb_struct*_tmpF9E=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));
_tmpF9E[0]=({struct Cyc_Absyn_Eq_kb_struct _tmpF9F;_tmpF9F.tag=Cyc_Absyn_Eq_kb;
_tmpF9F.f1=(void*)((void*)Cyc_Absyn_BoxKind);_tmpF9F;});_tmpF9E;}));_tmpF9D;});
goto _LL3962;_LL3970:*_tmpF98=({struct Cyc_Core_Opt*_tmpFA0=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmpFA0->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmpFA1=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmpFA1[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmpFA2;_tmpFA2.tag=Cyc_Absyn_Eq_kb;_tmpFA2.f1=(void*)_tmpF94;_tmpFA2;});_tmpFA1;}));
_tmpFA0;});goto _LL3962;_LL3972:({void*_tmpFA3[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function's can't abstract M types",
sizeof(unsigned char),34),_tag_arr(_tmpFA3,sizeof(void*),0));});goto _LL3962;
_LL3974: goto _LL3962;_LL3962:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmpECD.kind_env,*_tmpDE1);_tmpECD.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmpECD.free_vars,*_tmpDE1);{struct Cyc_List_List*tvs=_tmpECD.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmpECD.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL3431;}}_LL3461: for(0;_tmpDE2 != 0;_tmpDE2=_tmpDE2->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)Cyc_Absyn_MemKind,(*((
struct _tuple4*)_tmpDE2->hd)).f2);}goto _LL3431;_LL3463:{struct _RegionHandle
_tmpFA5=_new_region("sprev_rgn");struct _RegionHandle*sprev_rgn=& _tmpFA5;
_push_region(sprev_rgn);{struct Cyc_List_List*prev_fields=0;for(0;_tmpDE4 != 0;
_tmpDE4=_tmpDE4->tl){struct Cyc_Absyn_Structfield _tmpFA8;struct Cyc_List_List*
_tmpFA9;struct Cyc_Absyn_Exp*_tmpFAB;void*_tmpFAD;struct Cyc_Absyn_Tqual _tmpFAF;
struct _tagged_arr*_tmpFB1;struct Cyc_Absyn_Structfield*_tmpFA6=(struct Cyc_Absyn_Structfield*)
_tmpDE4->hd;_tmpFA8=*_tmpFA6;_LL4018: _tmpFB1=_tmpFA8.name;goto _LL4016;_LL4016:
_tmpFAF=_tmpFA8.tq;goto _LL4014;_LL4014: _tmpFAD=(void*)_tmpFA8.type;goto _LL4012;
_LL4012: _tmpFAB=_tmpFA8.width;goto _LL4010;_LL4010: _tmpFA9=_tmpFA8.attributes;
goto _LL4007;_LL4007: if(((int(*)(int(*compare)(struct _tagged_arr*,struct
_tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,
prev_fields,_tmpFB1)){({struct Cyc_Std_String_pa_struct _tmpFB4;_tmpFB4.tag=Cyc_Std_String_pa;
_tmpFB4.f1=(struct _tagged_arr)*_tmpFB1;{void*_tmpFB3[1]={& _tmpFB4};Cyc_Tcutil_terr(
loc,_tag_arr("duplicate field %s in struct",sizeof(unsigned char),29),_tag_arr(
_tmpFB3,sizeof(void*),1));}});}if(Cyc_Std_strcmp(*_tmpFB1,_tag_arr("",sizeof(
unsigned char),1))!= 0){prev_fields=({struct Cyc_List_List*_tmpFB5=_region_malloc(
sprev_rgn,sizeof(struct Cyc_List_List));_tmpFB5->hd=_tmpFB1;_tmpFB5->tl=
prev_fields;_tmpFB5;});}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
Cyc_Absyn_MemKind,_tmpFAD);Cyc_Tcutil_check_bitfield(loc,te,_tmpFAD,_tmpFAB,
_tmpFB1);Cyc_Tcutil_check_field_atts(loc,_tmpFB1,_tmpFA9);}};_pop_region(
sprev_rgn);}goto _LL3431;_LL3465:{struct _RegionHandle _tmpFB6=_new_region("uprev_rgn");
struct _RegionHandle*uprev_rgn=& _tmpFB6;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;for(0;_tmpDE6 != 0;_tmpDE6=_tmpDE6->tl){struct Cyc_Absyn_Structfield
_tmpFB9;struct Cyc_List_List*_tmpFBA;struct Cyc_Absyn_Exp*_tmpFBC;void*_tmpFBE;
struct Cyc_Absyn_Tqual _tmpFC0;struct _tagged_arr*_tmpFC2;struct Cyc_Absyn_Structfield*
_tmpFB7=(struct Cyc_Absyn_Structfield*)_tmpDE6->hd;_tmpFB9=*_tmpFB7;_LL4035:
_tmpFC2=_tmpFB9.name;goto _LL4033;_LL4033: _tmpFC0=_tmpFB9.tq;goto _LL4031;_LL4031:
_tmpFBE=(void*)_tmpFB9.type;goto _LL4029;_LL4029: _tmpFBC=_tmpFB9.width;goto
_LL4027;_LL4027: _tmpFBA=_tmpFB9.attributes;goto _LL4024;_LL4024: if(((int(*)(int(*
compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct
_tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,prev_fields,_tmpFC2)){({struct
Cyc_Std_String_pa_struct _tmpFC5;_tmpFC5.tag=Cyc_Std_String_pa;_tmpFC5.f1=(struct
_tagged_arr)*_tmpFC2;{void*_tmpFC4[1]={& _tmpFC5};Cyc_Tcutil_terr(loc,_tag_arr("duplicate field %s in struct",
sizeof(unsigned char),29),_tag_arr(_tmpFC4,sizeof(void*),1));}});}if(Cyc_Std_strcmp(*
_tmpFC2,_tag_arr("",sizeof(unsigned char),1))!= 0){prev_fields=({struct Cyc_List_List*
_tmpFC6=_region_malloc(uprev_rgn,sizeof(struct Cyc_List_List));_tmpFC6->hd=
_tmpFC2;_tmpFC6->tl=prev_fields;_tmpFC6;});}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)Cyc_Absyn_MemKind,_tmpFBE);if(! Cyc_Tcutil_bits_only(_tmpFBE)){({
struct Cyc_Std_String_pa_struct _tmpFC8;_tmpFC8.tag=Cyc_Std_String_pa;_tmpFC8.f1=(
struct _tagged_arr)*_tmpFC2;{void*_tmpFC7[1]={& _tmpFC8};Cyc_Tcutil_terr(loc,
_tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof(unsigned char),72),_tag_arr(_tmpFC7,sizeof(void*),1));}});}Cyc_Tcutil_check_bitfield(
loc,te,_tmpFBE,_tmpFBC,_tmpFC2);Cyc_Tcutil_check_field_atts(loc,_tmpFC2,_tmpFBA);}};
_pop_region(uprev_rgn);}goto _LL3431;_LL3467: if(_tmpDEE == 0){if(*_tmpDEA == 0){({
void*_tmpFC9[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad anonymous struct",sizeof(
unsigned char),21),_tag_arr(_tmpFC9,sizeof(void*),0));});return cvtenv;}{struct
Cyc_Absyn_Structdecl*sd=*((struct Cyc_Absyn_Structdecl**)_check_null(*_tmpDEA));;}}
if(*_tmpDEA == 0){struct _handler_cons _tmpFCA;_push_handler(& _tmpFCA);{int _tmpFCC=
0;if(setjmp(_tmpFCA.handler)){_tmpFCC=1;}if(! _tmpFCC){*_tmpDEA=(struct Cyc_Absyn_Structdecl**)
Cyc_Tcenv_lookup_structdecl(te,loc,(struct _tuple1*)_check_null(_tmpDEE));;
_pop_handler();}else{void*_tmpFCB=(void*)_exn_thrown;void*_tmpFCE=_tmpFCB;
_LL4048: if(_tmpFCE == Cyc_Dict_Absent){goto _LL4049;}else{goto _LL4050;}_LL4050:
goto _LL4051;_LL4049: {struct _tuple1*tdn=(struct _tuple1*)_check_null(_tmpDEE);
struct Cyc_Tcenv_Genv*_tmpFD4=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Structdecl*
_tmpFD5=({struct Cyc_Absyn_Structdecl*_tmpFD8=_cycalloc(sizeof(struct Cyc_Absyn_Structdecl));
_tmpFD8->sc=(void*)((void*)Cyc_Absyn_Extern);_tmpFD8->name=({struct Cyc_Core_Opt*
_tmpFD9=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmpFD9->v=tdn;_tmpFD9;});_tmpFD8->tvs=
0;_tmpFD8->fields=0;_tmpFD8->attributes=0;_tmpFD8;});Cyc_Tc_tcStructdecl(te,
_tmpFD4,loc,_tmpFD5);*_tmpDEA=(struct Cyc_Absyn_Structdecl**)Cyc_Tcenv_lookup_structdecl(
te,loc,tdn);if(*_tmpDED != 0){({struct Cyc_Std_String_pa_struct _tmpFD7;_tmpFD7.tag=
Cyc_Std_String_pa;_tmpFD7.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tdn);{
void*_tmpFD6[1]={& _tmpFD7};Cyc_Tcutil_terr(loc,_tag_arr("please declare parameterized struct %s before using",
sizeof(unsigned char),52),_tag_arr(_tmpFD6,sizeof(void*),1));}});return cvtenv;}
goto _LL4047;}_LL4051:(void)_throw(_tmpFCE);_LL4047:;}}}{struct Cyc_Absyn_Structdecl*
sd=*((struct Cyc_Absyn_Structdecl**)_check_null(*_tmpDEA));if(sd->name != 0){*((
struct _tuple1*)_check_null(_tmpDEE))=((struct _tuple1*)((struct Cyc_Core_Opt*)
_check_null(sd->name))->v)[_check_known_subscript_notnull(1,0)];}{struct Cyc_List_List*
tvs=sd->tvs;struct Cyc_List_List*ts=*_tmpDED;for(0;ts != 0? tvs != 0: 0;(ts=ts->tl,
tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd);}if(ts != 0){({
struct Cyc_Std_String_pa_struct _tmpFDB;_tmpFDB.tag=Cyc_Std_String_pa;_tmpFDB.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string((struct _tuple1*)_check_null(_tmpDEE));{
void*_tmpFDA[1]={& _tmpFDB};Cyc_Tcutil_terr(loc,_tag_arr("too many parameters for struct %s",
sizeof(unsigned char),34),_tag_arr(_tmpFDA,sizeof(void*),1));}});}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmpFDC=_cycalloc(sizeof(struct Cyc_List_List));_tmpFDC->hd=(void*)
e;_tmpFDC->tl=hidden_ts;_tmpFDC;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,e);}*_tmpDED=Cyc_List_imp_append(*_tmpDED,Cyc_List_imp_rev(hidden_ts));}
goto _LL3431;}}_LL3469: if(_tmpDF6 == 0){({void*_tmpFDD[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("missing union name",sizeof(unsigned char),19),_tag_arr(_tmpFDD,sizeof(
void*),0));});return cvtenv;}if(*_tmpDF2 == 0){struct _handler_cons _tmpFDE;
_push_handler(& _tmpFDE);{int _tmpFE0=0;if(setjmp(_tmpFDE.handler)){_tmpFE0=1;}if(
! _tmpFE0){*_tmpDF2=(struct Cyc_Absyn_Uniondecl**)Cyc_Tcenv_lookup_uniondecl(te,
loc,(struct _tuple1*)_check_null(_tmpDF6));;_pop_handler();}else{void*_tmpFDF=(
void*)_exn_thrown;void*_tmpFE2=_tmpFDF;_LL4068: if(_tmpFE2 == Cyc_Dict_Absent){
goto _LL4069;}else{goto _LL4070;}_LL4070: goto _LL4071;_LL4069: {struct _tuple1*
_tmpFE8=(struct _tuple1*)_check_null(_tmpDF6);struct Cyc_Tcenv_Genv*_tmpFE9=((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);struct Cyc_Absyn_Uniondecl*_tmpFEA=({struct Cyc_Absyn_Uniondecl*
_tmpFED=_cycalloc(sizeof(struct Cyc_Absyn_Uniondecl));_tmpFED->sc=(void*)((void*)
Cyc_Absyn_Extern);_tmpFED->name=({struct Cyc_Core_Opt*_tmpFEE=_cycalloc(sizeof(
struct Cyc_Core_Opt));_tmpFEE->v=_tmpFE8;_tmpFEE;});_tmpFED->tvs=0;_tmpFED->fields=
0;_tmpFED->attributes=0;_tmpFED;});Cyc_Tc_tcUniondecl(te,_tmpFE9,loc,_tmpFEA);*
_tmpDF2=(struct Cyc_Absyn_Uniondecl**)Cyc_Tcenv_lookup_uniondecl(te,loc,_tmpFE8);
if(*_tmpDF5 != 0){({struct Cyc_Std_String_pa_struct _tmpFEC;_tmpFEC.tag=Cyc_Std_String_pa;
_tmpFEC.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpFE8);{void*_tmpFEB[1]={&
_tmpFEC};Cyc_Tcutil_terr(loc,_tag_arr("please declare parameterized union %s before using",
sizeof(unsigned char),51),_tag_arr(_tmpFEB,sizeof(void*),1));}});return cvtenv;}
goto _LL4067;}_LL4071:(void)_throw(_tmpFE2);_LL4067:;}}}{struct Cyc_Absyn_Uniondecl*
ud=*((struct Cyc_Absyn_Uniondecl**)_check_null(*_tmpDF2));if(ud->name != 0){*
_tmpDF6=((struct _tuple1*)((struct Cyc_Core_Opt*)_check_null(ud->name))->v)[
_check_known_subscript_notnull(1,0)];}{struct Cyc_List_List*tvs=ud->tvs;struct Cyc_List_List*
ts=*_tmpDF5;for(0;ts != 0? tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd);}if(ts != 0){({struct Cyc_Std_String_pa_struct _tmpFF0;
_tmpFF0.tag=Cyc_Std_String_pa;_tmpFF0.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((
struct _tuple1*)_check_null(_tmpDF6));{void*_tmpFEF[1]={& _tmpFF0};Cyc_Tcutil_terr(
loc,_tag_arr("too many parameters for union %s",sizeof(unsigned char),33),
_tag_arr(_tmpFEF,sizeof(void*),1));}});}if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmpFF1=
_cycalloc(sizeof(struct Cyc_List_List));_tmpFF1->hd=(void*)e;_tmpFF1->tl=
hidden_ts;_tmpFF1;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*
_tmpDF5=Cyc_List_imp_append(*_tmpDF5,Cyc_List_imp_rev(hidden_ts));}goto _LL3431;}}
_LL3471: {struct Cyc_List_List*targs=*_tmpDFD;struct Cyc_Absyn_Typedefdecl*td;{
struct _handler_cons _tmpFF2;_push_handler(& _tmpFF2);{int _tmpFF4=0;if(setjmp(
_tmpFF2.handler)){_tmpFF4=1;}if(! _tmpFF4){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmpDFE);;_pop_handler();}else{void*_tmpFF3=(void*)_exn_thrown;void*_tmpFF6=
_tmpFF3;_LL4088: if(_tmpFF6 == Cyc_Dict_Absent){goto _LL4089;}else{goto _LL4090;}
_LL4090: goto _LL4091;_LL4089:({struct Cyc_Std_String_pa_struct _tmpFFD;_tmpFFD.tag=
Cyc_Std_String_pa;_tmpFFD.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmpDFE);{
void*_tmpFFC[1]={& _tmpFFD};Cyc_Tcutil_terr(loc,_tag_arr("unbound typedef name %s",
sizeof(unsigned char),24),_tag_arr(_tmpFFC,sizeof(void*),1));}});return cvtenv;
_LL4091:(void)_throw(_tmpFF6);_LL4087:;}}}_tmpDFE[0]=(td->name)[
_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*
ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0? tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);inst=({struct Cyc_List_List*_tmpFFE=_cycalloc(
sizeof(struct Cyc_List_List));_tmpFFE->hd=({struct _tuple5*_tmpFFF=_cycalloc(
sizeof(struct _tuple5));_tmpFFF->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmpFFF->f2=(
void*)ts->hd;_tmpFFF;});_tmpFFE->tl=inst;_tmpFFE;});}if(ts != 0){({struct Cyc_Std_String_pa_struct
_tmp1001;_tmp1001.tag=Cyc_Std_String_pa;_tmp1001.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmpDFE);{void*_tmp1000[1]={& _tmp1001};Cyc_Tcutil_terr(loc,_tag_arr("too many parameters for typedef %s",
sizeof(unsigned char),35),_tag_arr(_tmp1000,sizeof(void*),1));}});}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp1002=_cycalloc(sizeof(struct Cyc_List_List));_tmp1002->hd=(void*)
e;_tmp1002->tl=hidden_ts;_tmp1002;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,e);inst=({struct Cyc_List_List*_tmp1003=_cycalloc(sizeof(struct Cyc_List_List));
_tmp1003->hd=({struct _tuple5*_tmp1004=_cycalloc(sizeof(struct _tuple5));_tmp1004->f1=(
struct Cyc_Absyn_Tvar*)tvs->hd;_tmp1004->f2=e;_tmp1004;});_tmp1003->tl=inst;
_tmp1003;});}*_tmpDFD=Cyc_List_imp_append(targs,Cyc_List_imp_rev(hidden_ts));}{
void*new_typ=Cyc_Tcutil_substitute(inst,(void*)td->defn);*_tmpDFA=({struct Cyc_Core_Opt*
_tmp1005=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1005->v=(void*)new_typ;
_tmp1005;});goto _LL3431;}}}_LL3473: goto _LL3431;_LL3475: _tmpE02=_tmpE00;goto
_LL3477;_LL3477: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)Cyc_Absyn_RgnKind,
_tmpE02);goto _LL3431;_LL3479: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(
void*)Cyc_Absyn_AnyKind,_tmpE04);goto _LL3431;_LL3481: for(0;_tmpE06 != 0;_tmpE06=
_tmpE06->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)Cyc_Absyn_EffKind,(
void*)_tmpE06->hd);}goto _LL3431;_LL3431:;}if(! Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(
t),expected_kind)){({struct Cyc_Std_String_pa_struct _tmp1009;_tmp1009.tag=Cyc_Std_String_pa;
_tmp1009.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(expected_kind);{struct Cyc_Std_String_pa_struct
_tmp1008;_tmp1008.tag=Cyc_Std_String_pa;_tmp1008.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind(t));{struct Cyc_Std_String_pa_struct _tmp1007;_tmp1007.tag=Cyc_Std_String_pa;
_tmp1007.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp1006[3]={&
_tmp1007,& _tmp1008,& _tmp1009};Cyc_Tcutil_terr(loc,_tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof(unsigned char),51),_tag_arr(_tmp1006,sizeof(void*),3));}}}});}return
cvtenv;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*kind_env,void*expected_kind,int
generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp100A=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp1019;_tmp1019.kind_env=kind_env;_tmp1019.free_vars=
0;_tmp1019.free_evars=0;_tmp1019.generalize_evars=generalize_evars;_tmp1019.fn_result=
0;_tmp1019;}),expected_kind,t);{struct Cyc_List_List*vs=_tmp100A.free_vars;for(0;
vs != 0;vs=vs->tl){_tmp100A.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(
struct Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp100A.free_evars;
for(0;evs != 0;evs=evs->tl){void*_tmp100B=Cyc_Tcutil_compress((void*)evs->hd);
struct Cyc_Core_Opt*_tmp1011;struct Cyc_Core_Opt**_tmp1013;_LL4109: if((
unsigned int)_tmp100B > 3?*((int*)_tmp100B)== Cyc_Absyn_Evar: 0){_LL4114: _tmp1011=((
struct Cyc_Absyn_Evar_struct*)_tmp100B)->f4;_tmp1013=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp100B)->f4;goto _LL4110;}else{goto _LL4111;}
_LL4111: goto _LL4112;_LL4110: if(*_tmp1013 == 0){*_tmp1013=({struct Cyc_Core_Opt*
_tmp1014=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1014->v=kind_env;_tmp1014;});}
else{struct Cyc_List_List*_tmp1015=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp1013))->v;struct Cyc_List_List*_tmp1016=0;for(0;_tmp1015 != 0;
_tmp1015=_tmp1015->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
kind_env,(struct Cyc_Absyn_Tvar*)_tmp1015->hd)){_tmp1016=({struct Cyc_List_List*
_tmp1017=_cycalloc(sizeof(struct Cyc_List_List));_tmp1017->hd=(struct Cyc_Absyn_Tvar*)
_tmp1015->hd;_tmp1017->tl=_tmp1016;_tmp1017;});}}*_tmp1013=({struct Cyc_Core_Opt*
_tmp1018=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1018->v=_tmp1016;_tmp1018;});}
goto _LL4108;_LL4112: goto _LL4108;_LL4108:;}}return _tmp100A;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_Tcutil_CVTEnv _tmp101A=
Cyc_Tcutil_check_valid_type(loc,te,0,(void*)Cyc_Absyn_MemKind,generalize_evars,t);
struct Cyc_List_List*_tmp101B=_tmp101A.free_vars;struct Cyc_List_List*_tmp101C=
_tmp101A.free_evars;{struct Cyc_List_List*x=_tmp101B;for(0;x != 0;x=x->tl){void*
_tmp101D=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp102B;struct Cyc_Core_Opt**_tmp102D;void*_tmp102E;struct Cyc_Core_Opt*
_tmp1030;struct Cyc_Core_Opt**_tmp1032;void*_tmp1033;struct Cyc_Core_Opt*_tmp1035;
struct Cyc_Core_Opt**_tmp1037;void*_tmp1038;struct Cyc_Core_Opt*_tmp103A;struct Cyc_Core_Opt**
_tmp103C;void*_tmp103D;_LL4127: if(*((int*)_tmp101D)== Cyc_Absyn_Unknown_kb){
_LL4140: _tmp102B=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp101D)->f1;_tmp102D=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp101D)->f1;goto
_LL4128;}else{goto _LL4129;}_LL4129: if(*((int*)_tmp101D)== Cyc_Absyn_Less_kb){
_LL4145: _tmp1030=((struct Cyc_Absyn_Less_kb_struct*)_tmp101D)->f1;_tmp1032=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp101D)->f1;goto
_LL4143;_LL4143: _tmp102E=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp101D)->f2;
if(_tmp102E == (void*)Cyc_Absyn_MemKind){goto _LL4130;}else{goto _LL4131;}}else{
goto _LL4131;}_LL4131: if(*((int*)_tmp101D)== Cyc_Absyn_Less_kb){_LL4150: _tmp1035=((
struct Cyc_Absyn_Less_kb_struct*)_tmp101D)->f1;_tmp1037=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp101D)->f1;goto _LL4148;_LL4148: _tmp1033=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp101D)->f2;if(_tmp1033 == (void*)Cyc_Absyn_AnyKind){
goto _LL4132;}else{goto _LL4133;}}else{goto _LL4133;}_LL4133: if(*((int*)_tmp101D)== 
Cyc_Absyn_Less_kb){_LL4155: _tmp103A=((struct Cyc_Absyn_Less_kb_struct*)_tmp101D)->f1;
_tmp103C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp101D)->f1;
goto _LL4153;_LL4153: _tmp1038=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp101D)->f2;
goto _LL4134;}else{goto _LL4135;}_LL4135: if(*((int*)_tmp101D)== Cyc_Absyn_Eq_kb){
_LL4158: _tmp103D=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp101D)->f1;if(
_tmp103D == (void*)Cyc_Absyn_MemKind){goto _LL4136;}else{goto _LL4137;}}else{goto
_LL4137;}_LL4137: goto _LL4138;_LL4128: _tmp1032=_tmp102D;goto _LL4130;_LL4130:
_tmp1037=_tmp1032;goto _LL4132;_LL4132:*_tmp1037=({struct Cyc_Core_Opt*_tmp103F=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp103F->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp1040=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));_tmp1040[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp1041;_tmp1041.tag=Cyc_Absyn_Eq_kb;_tmp1041.f1=(void*)((void*)Cyc_Absyn_BoxKind);
_tmp1041;});_tmp1040;}));_tmp103F;});goto _LL4126;_LL4134:*_tmp103C=({struct Cyc_Core_Opt*
_tmp1042=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1042->v=(void*)((void*)({
struct Cyc_Absyn_Eq_kb_struct*_tmp1043=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_struct));
_tmp1043[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp1044;_tmp1044.tag=Cyc_Absyn_Eq_kb;
_tmp1044.f1=(void*)_tmp1038;_tmp1044;});_tmp1043;}));_tmp1042;});goto _LL4126;
_LL4136:({struct Cyc_Std_String_pa_struct _tmp1046;_tmp1046.tag=Cyc_Std_String_pa;
_tmp1046.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd);{
void*_tmp1045[1]={& _tmp1046};Cyc_Tcutil_terr(loc,_tag_arr("type variable %s cannot have kind M",
sizeof(unsigned char),36),_tag_arr(_tmp1045,sizeof(void*),1));}});goto _LL4126;
_LL4138: goto _LL4126;_LL4126:;}}if(_tmp101B != 0? 1: _tmp101C != 0){{void*_tmp1047=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp104D;struct Cyc_List_List*
_tmp104F;struct Cyc_List_List*_tmp1051;struct Cyc_Absyn_VarargInfo*_tmp1053;int
_tmp1055;struct Cyc_List_List*_tmp1057;void*_tmp1059;struct Cyc_Core_Opt*_tmp105B;
struct Cyc_List_List*_tmp105D;struct Cyc_List_List**_tmp105F;_LL4169: if((
unsigned int)_tmp1047 > 3?*((int*)_tmp1047)== Cyc_Absyn_FnType: 0){_LL4174:
_tmp104D=((struct Cyc_Absyn_FnType_struct*)_tmp1047)->f1;_LL4190: _tmp105D=
_tmp104D.tvars;_tmp105F=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp1047)->f1).tvars;goto _LL4188;_LL4188: _tmp105B=_tmp104D.effect;goto _LL4186;
_LL4186: _tmp1059=(void*)_tmp104D.ret_typ;goto _LL4184;_LL4184: _tmp1057=_tmp104D.args;
goto _LL4182;_LL4182: _tmp1055=_tmp104D.c_varargs;goto _LL4180;_LL4180: _tmp1053=
_tmp104D.cyc_varargs;goto _LL4178;_LL4178: _tmp1051=_tmp104D.rgn_po;goto _LL4176;
_LL4176: _tmp104F=_tmp104D.attributes;goto _LL4170;}else{goto _LL4171;}_LL4171: goto
_LL4172;_LL4170: if(*_tmp105F == 0){*_tmp105F=_tmp101B;_tmp101B=0;}goto _LL4168;
_LL4172: goto _LL4168;_LL4168:;}if(_tmp101B != 0){({struct Cyc_Std_String_pa_struct
_tmp1061;_tmp1061.tag=Cyc_Std_String_pa;_tmp1061.f1=(struct _tagged_arr)*((struct
Cyc_Absyn_Tvar*)_tmp101B->hd)->name;{void*_tmp1060[1]={& _tmp1061};Cyc_Tcutil_terr(
loc,_tag_arr("unbound type variable %s",sizeof(unsigned char),25),_tag_arr(
_tmp1060,sizeof(void*),1));}});}if(_tmp101C != 0){for(0;_tmp101C != 0;_tmp101C=
_tmp101C->tl){void*e=(void*)_tmp101C->hd;void*_tmp1062=Cyc_Tcutil_typ_kind(e);
_LL4196: if(_tmp1062 == (void*)Cyc_Absyn_RgnKind){goto _LL4197;}else{goto _LL4198;}
_LL4198: if(_tmp1062 == (void*)Cyc_Absyn_EffKind){goto _LL4199;}else{goto _LL4200;}
_LL4200: goto _LL4201;_LL4197: if(! Cyc_Tcutil_unify(e,(void*)Cyc_Absyn_HeapRgn)){({
void*_tmp106A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with heap!",sizeof(unsigned char),28),_tag_arr(
_tmp106A,sizeof(void*),0));});}goto _LL4195;_LL4199: if(! Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){({
void*_tmp106B[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!",sizeof(unsigned char),26),_tag_arr(_tmp106B,
sizeof(void*),0));});}goto _LL4195;_LL4201:({struct Cyc_Std_String_pa_struct
_tmp106E;_tmp106E.tag=Cyc_Std_String_pa;_tmp106E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{struct Cyc_Std_String_pa_struct _tmp106D;_tmp106D.tag=Cyc_Std_String_pa;
_tmp106D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{void*_tmp106C[2]={&
_tmp106D,& _tmp106E};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(unsigned char),52),_tag_arr(_tmp106C,sizeof(void*),2));}}});goto _LL4195;
_LL4195:;}}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmp106F=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp1075;struct Cyc_List_List*_tmp1077;struct Cyc_List_List*
_tmp1079;struct Cyc_Absyn_VarargInfo*_tmp107B;int _tmp107D;struct Cyc_List_List*
_tmp107F;void*_tmp1081;struct Cyc_Core_Opt*_tmp1083;struct Cyc_List_List*_tmp1085;
_LL4209: if((unsigned int)_tmp106F > 3?*((int*)_tmp106F)== Cyc_Absyn_FnType: 0){
_LL4214: _tmp1075=((struct Cyc_Absyn_FnType_struct*)_tmp106F)->f1;_LL4230: _tmp1085=
_tmp1075.tvars;goto _LL4228;_LL4228: _tmp1083=_tmp1075.effect;goto _LL4226;_LL4226:
_tmp1081=(void*)_tmp1075.ret_typ;goto _LL4224;_LL4224: _tmp107F=_tmp1075.args;goto
_LL4222;_LL4222: _tmp107D=_tmp1075.c_varargs;goto _LL4220;_LL4220: _tmp107B=
_tmp1075.cyc_varargs;goto _LL4218;_LL4218: _tmp1079=_tmp1075.rgn_po;goto _LL4216;
_LL4216: _tmp1077=_tmp1075.attributes;goto _LL4210;}else{goto _LL4211;}_LL4211: goto
_LL4212;_LL4210: fd->tvs=_tmp1085;fd->effect=_tmp1083;goto _LL4208;_LL4212:({void*
_tmp1087[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("check_fndecl_valid_type: not a FnType",sizeof(unsigned char),38),
_tag_arr(_tmp1087,sizeof(void*),0));});return;_LL4208:;}{struct _RegionHandle
_tmp1088=_new_region("r");struct _RegionHandle*r=& _tmp1088;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tagged_arr*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct
_tuple13*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,_tag_arr("function declaration has repeated parameter",
sizeof(unsigned char),44));;_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*
_tmp1089=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp1089->v=(void*)t;_tmp1089;});}
void Cyc_Tcutil_check_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*bound_tvars,void*expected_kind,int allow_evars,void*t){struct
Cyc_Tcutil_CVTEnv _tmp108A=Cyc_Tcutil_check_valid_type(loc,te,bound_tvars,
expected_kind,0,t);struct Cyc_List_List*_tmp108B=Cyc_Tcutil_remove_bound_tvars(
_tmp108A.free_vars,bound_tvars);struct Cyc_List_List*_tmp108C=_tmp108A.free_evars;{
struct Cyc_List_List*fs=_tmp108B;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*
_tmp108D=((struct Cyc_Absyn_Tvar*)fs->hd)->name;({struct Cyc_Std_String_pa_struct
_tmp1090;_tmp1090.tag=Cyc_Std_String_pa;_tmp1090.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{struct Cyc_Std_String_pa_struct _tmp108F;_tmp108F.tag=Cyc_Std_String_pa;
_tmp108F.f1=(struct _tagged_arr)*_tmp108D;{void*_tmp108E[2]={& _tmp108F,& _tmp1090};
Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s in type %s",sizeof(
unsigned char),36),_tag_arr(_tmp108E,sizeof(void*),2));}}});}}if(! allow_evars?
_tmp108C != 0: 0){for(0;_tmp108C != 0;_tmp108C=_tmp108C->tl){void*e=(void*)_tmp108C->hd;
void*_tmp1091=Cyc_Tcutil_typ_kind(e);_LL4243: if(_tmp1091 == (void*)Cyc_Absyn_RgnKind){
goto _LL4244;}else{goto _LL4245;}_LL4245: if(_tmp1091 == (void*)Cyc_Absyn_EffKind){
goto _LL4246;}else{goto _LL4247;}_LL4247: goto _LL4248;_LL4244: if(! Cyc_Tcutil_unify(
e,(void*)Cyc_Absyn_HeapRgn)){({void*_tmp1099[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with heap!",
sizeof(unsigned char),28),_tag_arr(_tmp1099,sizeof(void*),0));});}goto _LL4242;
_LL4246: if(! Cyc_Tcutil_unify(e,(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp109A=
_cycalloc(sizeof(struct Cyc_Absyn_JoinEff_struct));_tmp109A[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp109B;_tmp109B.tag=Cyc_Absyn_JoinEff;_tmp109B.f1=0;_tmp109B;});_tmp109A;}))){({
void*_tmp109C[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("can't unify evar with {}!",sizeof(unsigned char),26),_tag_arr(_tmp109C,
sizeof(void*),0));});}goto _LL4242;_LL4248:({struct Cyc_Std_String_pa_struct
_tmp109F;_tmp109F.tag=Cyc_Std_String_pa;_tmp109F.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(
t);{struct Cyc_Std_String_pa_struct _tmp109E;_tmp109E.tag=Cyc_Std_String_pa;
_tmp109E.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{void*_tmp109D[2]={&
_tmp109E,& _tmp109F};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(unsigned char),52),_tag_arr(_tmp109D,sizeof(void*),2));}}});goto _LL4242;
_LL4242:;}}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity
== 0){tv->identity=Cyc_Tcutil_new_tvar_id();}}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _tagged_arr(*a2string)(void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(
void*)vs2->hd)== 0){({struct Cyc_Std_String_pa_struct _tmp10A2;_tmp10A2.tag=Cyc_Std_String_pa;
_tmp10A2.f1=(struct _tagged_arr)a2string((void*)vs->hd);{struct Cyc_Std_String_pa_struct
_tmp10A1;_tmp10A1.tag=Cyc_Std_String_pa;_tmp10A1.f1=(struct _tagged_arr)msg;{void*
_tmp10A0[2]={& _tmp10A1,& _tmp10A2};Cyc_Tcutil_terr(loc,_tag_arr("%s: %s",sizeof(
unsigned char),7),_tag_arr(_tmp10A0,sizeof(void*),2));}}});}}}}static struct
_tagged_arr Cyc_Tcutil_strptr2string(struct _tagged_arr*s){return*s;}void Cyc_Tcutil_check_unique_vars(
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr msg){((
void(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _tagged_arr(*a2string)(struct _tagged_arr*),
struct _tagged_arr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Std_zstrptrcmp,vs,
loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*tvs){((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _tagged_arr(*
a2string)(struct Cyc_Absyn_Tvar*),struct _tagged_arr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,_tag_arr("duplicate type variable",
sizeof(unsigned char),24));}struct _tuple18{struct Cyc_Absyn_Structfield*f1;int f2;
};struct _tuple19{struct Cyc_List_List*f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Structfield*
f1;void*f2;};struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*sdfields){struct Cyc_List_List*fields=0;{struct Cyc_List_List*
sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){if(Cyc_Std_strcmp(*((
struct Cyc_Absyn_Structfield*)sd_fields->hd)->name,_tag_arr("",sizeof(
unsigned char),1))!= 0){fields=({struct Cyc_List_List*_tmp10A3=_cycalloc(sizeof(
struct Cyc_List_List));_tmp10A3->hd=({struct _tuple18*_tmp10A4=_cycalloc(sizeof(
struct _tuple18));_tmp10A4->f1=(struct Cyc_Absyn_Structfield*)sd_fields->hd;
_tmp10A4->f2=0;_tmp10A4;});_tmp10A3->tl=fields;_tmp10A3;});}}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple19 _tmp10A7;void*_tmp10A8;struct Cyc_List_List*
_tmp10AA;struct _tuple19*_tmp10A5=(struct _tuple19*)des->hd;_tmp10A7=*_tmp10A5;
_LL4267: _tmp10AA=_tmp10A7.f1;goto _LL4265;_LL4265: _tmp10A8=_tmp10A7.f2;goto
_LL4262;_LL4262: if(_tmp10AA == 0){struct Cyc_List_List*_tmp10AC=fields;for(0;
_tmp10AC != 0;_tmp10AC=_tmp10AC->tl){if(!(*((struct _tuple18*)_tmp10AC->hd)).f2){(*((
struct _tuple18*)_tmp10AC->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp10AD=_cycalloc(sizeof(struct Cyc_List_List));_tmp10AD->hd=(
void*)((void*)({struct Cyc_Absyn_FieldName_struct*_tmp10AE=_cycalloc(sizeof(
struct Cyc_Absyn_FieldName_struct));_tmp10AE[0]=({struct Cyc_Absyn_FieldName_struct
_tmp10AF;_tmp10AF.tag=Cyc_Absyn_FieldName;_tmp10AF.f1=((*((struct _tuple18*)
_tmp10AC->hd)).f1)->name;_tmp10AF;});_tmp10AE;}));_tmp10AD->tl=0;_tmp10AD;});ans=({
struct Cyc_List_List*_tmp10B0=_region_malloc(rgn,sizeof(struct Cyc_List_List));
_tmp10B0->hd=({struct _tuple20*_tmp10B1=_region_malloc(rgn,sizeof(struct _tuple20));
_tmp10B1->f1=(*((struct _tuple18*)_tmp10AC->hd)).f1;_tmp10B1->f2=_tmp10A8;
_tmp10B1;});_tmp10B0->tl=ans;_tmp10B0;});break;}}if(_tmp10AC == 0){({void*
_tmp10B2[0]={};Cyc_Tcutil_terr(loc,_tag_arr("too many arguments to struct",
sizeof(unsigned char),29),_tag_arr(_tmp10B2,sizeof(void*),0));});}}else{if(
_tmp10AA->tl != 0){({void*_tmp10B3[0]={};Cyc_Tcutil_terr(loc,_tag_arr("multiple designators are not supported",
sizeof(unsigned char),39),_tag_arr(_tmp10B3,sizeof(void*),0));});}else{void*
_tmp10B4=(void*)_tmp10AA->hd;struct _tagged_arr*_tmp10BA;_LL4278: if(*((int*)
_tmp10B4)== Cyc_Absyn_ArrayElement){goto _LL4279;}else{goto _LL4280;}_LL4280: if(*((
int*)_tmp10B4)== Cyc_Absyn_FieldName){_LL4283: _tmp10BA=((struct Cyc_Absyn_FieldName_struct*)
_tmp10B4)->f1;goto _LL4281;}else{goto _LL4277;}_LL4279:({void*_tmp10BC[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("array designator used in argument to struct",sizeof(unsigned char),
44),_tag_arr(_tmp10BC,sizeof(void*),0));});goto _LL4277;_LL4281: {struct Cyc_List_List*
_tmp10BD=fields;for(0;_tmp10BD != 0;_tmp10BD=_tmp10BD->tl){if(Cyc_Std_zstrptrcmp(
_tmp10BA,((*((struct _tuple18*)_tmp10BD->hd)).f1)->name)== 0){if((*((struct
_tuple18*)_tmp10BD->hd)).f2){({struct Cyc_Std_String_pa_struct _tmp10BF;_tmp10BF.tag=
Cyc_Std_String_pa;_tmp10BF.f1=(struct _tagged_arr)*_tmp10BA;{void*_tmp10BE[1]={&
_tmp10BF};Cyc_Tcutil_terr(loc,_tag_arr("field %s has already been used as an argument",
sizeof(unsigned char),46),_tag_arr(_tmp10BE,sizeof(void*),1));}});}(*((struct
_tuple18*)_tmp10BD->hd)).f2=1;ans=({struct Cyc_List_List*_tmp10C0=_region_malloc(
rgn,sizeof(struct Cyc_List_List));_tmp10C0->hd=({struct _tuple20*_tmp10C1=
_region_malloc(rgn,sizeof(struct _tuple20));_tmp10C1->f1=(*((struct _tuple18*)
_tmp10BD->hd)).f1;_tmp10C1->f2=_tmp10A8;_tmp10C1;});_tmp10C0->tl=ans;_tmp10C0;});
break;}}if(_tmp10BD == 0){({struct Cyc_Std_String_pa_struct _tmp10C3;_tmp10C3.tag=
Cyc_Std_String_pa;_tmp10C3.f1=(struct _tagged_arr)*_tmp10BA;{void*_tmp10C2[1]={&
_tmp10C3};Cyc_Tcutil_terr(loc,_tag_arr("bad field designator %s",sizeof(
unsigned char),24),_tag_arr(_tmp10C2,sizeof(void*),1));}});}goto _LL4277;}_LL4277:;}}}
for(0;fields != 0;fields=fields->tl){if(!(*((struct _tuple18*)fields->hd)).f2){({
void*_tmp10C4[0]={};Cyc_Tcutil_terr(loc,_tag_arr("too few arguments to struct",
sizeof(unsigned char),28),_tag_arr(_tmp10C4,sizeof(void*),0));});break;}}return((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp10C5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp10CB;struct Cyc_Absyn_Conref*_tmp10CD;void*_tmp10CF;_LL4295: if((unsigned int)
_tmp10C5 > 3?*((int*)_tmp10C5)== Cyc_Absyn_PointerType: 0){_LL4300: _tmp10CB=((
struct Cyc_Absyn_PointerType_struct*)_tmp10C5)->f1;_LL4304: _tmp10CF=(void*)
_tmp10CB.elt_typ;goto _LL4302;_LL4302: _tmp10CD=_tmp10CB.bounds;goto _LL4296;}else{
goto _LL4297;}_LL4297: goto _LL4298;_LL4296: {struct Cyc_Absyn_Conref*_tmp10D1=Cyc_Absyn_compress_conref(
_tmp10CD);void*_tmp10D2=(void*)(Cyc_Absyn_compress_conref(_tmp10D1))->v;void*
_tmp10DA;_LL4308: if((unsigned int)_tmp10D2 > 1?*((int*)_tmp10D2)== Cyc_Absyn_Eq_constr:
0){_LL4315: _tmp10DA=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp10D2)->f1;if(
_tmp10DA == (void*)Cyc_Absyn_Unknown_b){goto _LL4309;}else{goto _LL4310;}}else{goto
_LL4310;}_LL4310: if(_tmp10D2 == (void*)Cyc_Absyn_No_constr){goto _LL4311;}else{
goto _LL4312;}_LL4312: goto _LL4313;_LL4309:*elt_typ_dest=_tmp10CF;return 1;_LL4311:(
void*)(_tmp10D1->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp10DC=
_cycalloc(sizeof(struct Cyc_Absyn_Eq_constr_struct));_tmp10DC[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp10DD;_tmp10DD.tag=Cyc_Absyn_Eq_constr;_tmp10DD.f1=(void*)((void*)Cyc_Absyn_Unknown_b);
_tmp10DD;});_tmp10DC;})));*elt_typ_dest=_tmp10CF;return 1;_LL4313: return 0;_LL4307:;}
_LL4298: return 0;_LL4294:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*
ignore=(void*)Cyc_Absyn_VoidType;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,&
ignore);}struct _tuple7 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e){struct _tuple7 bogus_ans=({struct _tuple7 _tmp1194;_tmp1194.f1=0;
_tmp1194.f2=(void*)Cyc_Absyn_HeapRgn;_tmp1194;});void*_tmp10DE=(void*)e->r;void*
_tmp10EC;struct _tuple1*_tmp10EE;struct _tagged_arr*_tmp10F0;struct Cyc_Absyn_Exp*
_tmp10F2;struct _tagged_arr*_tmp10F4;struct Cyc_Absyn_Exp*_tmp10F6;struct Cyc_Absyn_Exp*
_tmp10F8;struct Cyc_Absyn_Exp*_tmp10FA;struct Cyc_Absyn_Exp*_tmp10FC;_LL4320: if(*((
int*)_tmp10DE)== Cyc_Absyn_Var_e){_LL4335: _tmp10EE=((struct Cyc_Absyn_Var_e_struct*)
_tmp10DE)->f1;goto _LL4333;_LL4333: _tmp10EC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp10DE)->f2;goto _LL4321;}else{goto _LL4322;}_LL4322: if(*((int*)_tmp10DE)== Cyc_Absyn_StructMember_e){
_LL4339: _tmp10F2=((struct Cyc_Absyn_StructMember_e_struct*)_tmp10DE)->f1;goto
_LL4337;_LL4337: _tmp10F0=((struct Cyc_Absyn_StructMember_e_struct*)_tmp10DE)->f2;
goto _LL4323;}else{goto _LL4324;}_LL4324: if(*((int*)_tmp10DE)== Cyc_Absyn_StructArrow_e){
_LL4343: _tmp10F6=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp10DE)->f1;goto
_LL4341;_LL4341: _tmp10F4=((struct Cyc_Absyn_StructArrow_e_struct*)_tmp10DE)->f2;
goto _LL4325;}else{goto _LL4326;}_LL4326: if(*((int*)_tmp10DE)== Cyc_Absyn_Deref_e){
_LL4345: _tmp10F8=((struct Cyc_Absyn_Deref_e_struct*)_tmp10DE)->f1;goto _LL4327;}
else{goto _LL4328;}_LL4328: if(*((int*)_tmp10DE)== Cyc_Absyn_Subscript_e){_LL4349:
_tmp10FC=((struct Cyc_Absyn_Subscript_e_struct*)_tmp10DE)->f1;goto _LL4347;_LL4347:
_tmp10FA=((struct Cyc_Absyn_Subscript_e_struct*)_tmp10DE)->f2;goto _LL4329;}else{
goto _LL4330;}_LL4330: goto _LL4331;_LL4321: {void*_tmp10FE=_tmp10EC;struct Cyc_Absyn_Vardecl*
_tmp110C;struct Cyc_Absyn_Vardecl*_tmp110E;struct Cyc_Absyn_Vardecl*_tmp1110;
struct Cyc_Absyn_Vardecl*_tmp1112;_LL4352: if(_tmp10FE == (void*)Cyc_Absyn_Unresolved_b){
goto _LL4353;}else{goto _LL4354;}_LL4354: if((unsigned int)_tmp10FE > 1?*((int*)
_tmp10FE)== Cyc_Absyn_Funname_b: 0){goto _LL4355;}else{goto _LL4356;}_LL4356: if((
unsigned int)_tmp10FE > 1?*((int*)_tmp10FE)== Cyc_Absyn_Global_b: 0){_LL4365:
_tmp110C=((struct Cyc_Absyn_Global_b_struct*)_tmp10FE)->f1;goto _LL4357;}else{goto
_LL4358;}_LL4358: if((unsigned int)_tmp10FE > 1?*((int*)_tmp10FE)== Cyc_Absyn_Local_b:
0){_LL4367: _tmp110E=((struct Cyc_Absyn_Local_b_struct*)_tmp10FE)->f1;goto _LL4359;}
else{goto _LL4360;}_LL4360: if((unsigned int)_tmp10FE > 1?*((int*)_tmp10FE)== Cyc_Absyn_Pat_b:
0){_LL4369: _tmp1110=((struct Cyc_Absyn_Pat_b_struct*)_tmp10FE)->f1;goto _LL4361;}
else{goto _LL4362;}_LL4362: if((unsigned int)_tmp10FE > 1?*((int*)_tmp10FE)== Cyc_Absyn_Param_b:
0){_LL4371: _tmp1112=((struct Cyc_Absyn_Param_b_struct*)_tmp10FE)->f1;goto _LL4363;}
else{goto _LL4351;}_LL4353: return bogus_ans;_LL4355: return({struct _tuple7 _tmp1114;
_tmp1114.f1=0;_tmp1114.f2=(void*)Cyc_Absyn_HeapRgn;_tmp1114;});_LL4357: {void*
_tmp1115=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL4375: if((unsigned int)_tmp1115 > 3?*((int*)_tmp1115)== Cyc_Absyn_ArrayType: 0){
goto _LL4376;}else{goto _LL4377;}_LL4377: goto _LL4378;_LL4376: return({struct _tuple7
_tmp111B;_tmp111B.f1=1;_tmp111B.f2=(void*)Cyc_Absyn_HeapRgn;_tmp111B;});_LL4378:
return({struct _tuple7 _tmp111C;_tmp111C.f1=(_tmp110C->tq).q_const;_tmp111C.f2=(
void*)Cyc_Absyn_HeapRgn;_tmp111C;});_LL4374:;}_LL4359: {void*_tmp111D=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL4383: if((unsigned int)
_tmp111D > 3?*((int*)_tmp111D)== Cyc_Absyn_ArrayType: 0){goto _LL4384;}else{goto
_LL4385;}_LL4385: goto _LL4386;_LL4384: return({struct _tuple7 _tmp1123;_tmp1123.f1=1;
_tmp1123.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp110E->rgn))->v;_tmp1123;});
_LL4386: _tmp110E->escapes=1;return({struct _tuple7 _tmp1124;_tmp1124.f1=(_tmp110E->tq).q_const;
_tmp1124.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp110E->rgn))->v;_tmp1124;});
_LL4382:;}_LL4361: _tmp1112=_tmp1110;goto _LL4363;_LL4363: _tmp1112->escapes=1;
return({struct _tuple7 _tmp1125;_tmp1125.f1=(_tmp1112->tq).q_const;_tmp1125.f2=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1112->rgn))->v;_tmp1125;});_LL4351:;}
_LL4323: {void*_tmp1126=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp10F2->topt))->v);struct Cyc_List_List*_tmp1132;struct Cyc_List_List*
_tmp1134;struct Cyc_Absyn_Structdecl**_tmp1136;struct Cyc_Absyn_Structdecl*
_tmp1138;struct Cyc_Absyn_Uniondecl**_tmp1139;struct Cyc_Absyn_Uniondecl*_tmp113B;
_LL4392: if((unsigned int)_tmp1126 > 3?*((int*)_tmp1126)== Cyc_Absyn_AnonStructType:
0){_LL4403: _tmp1132=((struct Cyc_Absyn_AnonStructType_struct*)_tmp1126)->f1;goto
_LL4393;}else{goto _LL4394;}_LL4394: if((unsigned int)_tmp1126 > 3?*((int*)_tmp1126)
== Cyc_Absyn_AnonUnionType: 0){_LL4405: _tmp1134=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp1126)->f1;goto _LL4395;}else{goto _LL4396;}_LL4396: if((unsigned int)_tmp1126 > 
3?*((int*)_tmp1126)== Cyc_Absyn_StructType: 0){_LL4407: _tmp1136=((struct Cyc_Absyn_StructType_struct*)
_tmp1126)->f3;if(_tmp1136 == 0){goto _LL4398;}else{_tmp1138=*_tmp1136;goto _LL4397;}}
else{goto _LL4398;}_LL4398: if((unsigned int)_tmp1126 > 3?*((int*)_tmp1126)== Cyc_Absyn_UnionType:
0){_LL4410: _tmp1139=((struct Cyc_Absyn_UnionType_struct*)_tmp1126)->f3;if(
_tmp1139 == 0){goto _LL4400;}else{_tmp113B=*_tmp1139;goto _LL4399;}}else{goto
_LL4400;}_LL4400: goto _LL4401;_LL4393: _tmp1134=_tmp1132;goto _LL4395;_LL4395: {
struct Cyc_Absyn_Structfield*_tmp113C=Cyc_Absyn_lookup_field(_tmp1134,_tmp10F0);
if(_tmp113C != 0? _tmp113C->width != 0: 0){return({struct _tuple7 _tmp113D;_tmp113D.f1=(
_tmp113C->tq).q_const;_tmp113D.f2=(Cyc_Tcutil_addressof_props(te,_tmp10F2)).f2;
_tmp113D;});}return bogus_ans;}_LL4397: {struct Cyc_Absyn_Structfield*_tmp113E=Cyc_Absyn_lookup_struct_field(
_tmp1138,_tmp10F0);if(_tmp113E != 0? _tmp113E->width != 0: 0){return({struct _tuple7
_tmp113F;_tmp113F.f1=(_tmp113E->tq).q_const;_tmp113F.f2=(Cyc_Tcutil_addressof_props(
te,_tmp10F2)).f2;_tmp113F;});}return bogus_ans;}_LL4399: {struct Cyc_Absyn_Structfield*
_tmp1140=Cyc_Absyn_lookup_union_field(_tmp113B,_tmp10F0);if(_tmp1140 != 0){return({
struct _tuple7 _tmp1141;_tmp1141.f1=(_tmp1140->tq).q_const;_tmp1141.f2=(Cyc_Tcutil_addressof_props(
te,_tmp10F2)).f2;_tmp1141;});}goto _LL4401;}_LL4401: return bogus_ans;_LL4391:;}
_LL4325: {void*_tmp1142=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp10F6->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1148;void*_tmp114A;
void*_tmp114C;_LL4420: if((unsigned int)_tmp1142 > 3?*((int*)_tmp1142)== Cyc_Absyn_PointerType:
0){_LL4425: _tmp1148=((struct Cyc_Absyn_PointerType_struct*)_tmp1142)->f1;_LL4429:
_tmp114C=(void*)_tmp1148.elt_typ;goto _LL4427;_LL4427: _tmp114A=(void*)_tmp1148.rgn_typ;
goto _LL4421;}else{goto _LL4422;}_LL4422: goto _LL4423;_LL4421: {void*_tmp114E=Cyc_Tcutil_compress(
_tmp114C);struct Cyc_List_List*_tmp115A;struct Cyc_List_List*_tmp115C;struct Cyc_Absyn_Structdecl**
_tmp115E;struct Cyc_Absyn_Structdecl*_tmp1160;struct Cyc_Absyn_Uniondecl**_tmp1161;
struct Cyc_Absyn_Uniondecl*_tmp1163;_LL4432: if((unsigned int)_tmp114E > 3?*((int*)
_tmp114E)== Cyc_Absyn_AnonStructType: 0){_LL4443: _tmp115A=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp114E)->f1;goto _LL4433;}else{goto _LL4434;}_LL4434: if((unsigned int)_tmp114E > 
3?*((int*)_tmp114E)== Cyc_Absyn_AnonUnionType: 0){_LL4445: _tmp115C=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp114E)->f1;goto _LL4435;}else{goto _LL4436;}_LL4436: if((unsigned int)_tmp114E > 
3?*((int*)_tmp114E)== Cyc_Absyn_StructType: 0){_LL4447: _tmp115E=((struct Cyc_Absyn_StructType_struct*)
_tmp114E)->f3;if(_tmp115E == 0){goto _LL4438;}else{_tmp1160=*_tmp115E;goto _LL4437;}}
else{goto _LL4438;}_LL4438: if((unsigned int)_tmp114E > 3?*((int*)_tmp114E)== Cyc_Absyn_UnionType:
0){_LL4450: _tmp1161=((struct Cyc_Absyn_UnionType_struct*)_tmp114E)->f3;if(
_tmp1161 == 0){goto _LL4440;}else{_tmp1163=*_tmp1161;goto _LL4439;}}else{goto
_LL4440;}_LL4440: goto _LL4441;_LL4433: _tmp115C=_tmp115A;goto _LL4435;_LL4435: {
struct Cyc_Absyn_Structfield*_tmp1164=Cyc_Absyn_lookup_field(_tmp115C,_tmp10F4);
if(_tmp1164 != 0? _tmp1164->width != 0: 0){return({struct _tuple7 _tmp1165;_tmp1165.f1=(
_tmp1164->tq).q_const;_tmp1165.f2=_tmp114A;_tmp1165;});}return bogus_ans;}_LL4437: {
struct Cyc_Absyn_Structfield*_tmp1166=Cyc_Absyn_lookup_struct_field(_tmp1160,
_tmp10F4);if(_tmp1166 != 0? _tmp1166->width != 0: 0){return({struct _tuple7 _tmp1167;
_tmp1167.f1=(_tmp1166->tq).q_const;_tmp1167.f2=_tmp114A;_tmp1167;});}return
bogus_ans;}_LL4439: {struct Cyc_Absyn_Structfield*_tmp1168=Cyc_Absyn_lookup_union_field(
_tmp1163,_tmp10F4);if(_tmp1168 != 0){return({struct _tuple7 _tmp1169;_tmp1169.f1=(
_tmp1168->tq).q_const;_tmp1169.f2=_tmp114A;_tmp1169;});}return bogus_ans;}_LL4441:
return bogus_ans;_LL4431:;}_LL4423: return bogus_ans;_LL4419:;}_LL4327: {void*
_tmp116A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp10F8->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1170;struct Cyc_Absyn_Tqual _tmp1172;void*_tmp1174;
_LL4460: if((unsigned int)_tmp116A > 3?*((int*)_tmp116A)== Cyc_Absyn_PointerType: 0){
_LL4465: _tmp1170=((struct Cyc_Absyn_PointerType_struct*)_tmp116A)->f1;_LL4469:
_tmp1174=(void*)_tmp1170.rgn_typ;goto _LL4467;_LL4467: _tmp1172=_tmp1170.tq;goto
_LL4461;}else{goto _LL4462;}_LL4462: goto _LL4463;_LL4461: return({struct _tuple7
_tmp1176;_tmp1176.f1=_tmp1172.q_const;_tmp1176.f2=_tmp1174;_tmp1176;});_LL4463:
return bogus_ans;_LL4459:;}_LL4329: {void*t=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp10FC->topt))->v);void*_tmp1177=t;struct Cyc_Absyn_Tqual
_tmp1181;struct Cyc_List_List*_tmp1183;struct Cyc_Absyn_PtrInfo _tmp1185;struct Cyc_Absyn_Conref*
_tmp1187;struct Cyc_Absyn_Tqual _tmp1189;void*_tmp118B;void*_tmp118D;_LL4473: if((
unsigned int)_tmp1177 > 3?*((int*)_tmp1177)== Cyc_Absyn_ArrayType: 0){_LL4482:
_tmp1181=((struct Cyc_Absyn_ArrayType_struct*)_tmp1177)->f2;goto _LL4474;}else{
goto _LL4475;}_LL4475: if((unsigned int)_tmp1177 > 3?*((int*)_tmp1177)== Cyc_Absyn_TupleType:
0){_LL4484: _tmp1183=((struct Cyc_Absyn_TupleType_struct*)_tmp1177)->f1;goto
_LL4476;}else{goto _LL4477;}_LL4477: if((unsigned int)_tmp1177 > 3?*((int*)_tmp1177)
== Cyc_Absyn_PointerType: 0){_LL4486: _tmp1185=((struct Cyc_Absyn_PointerType_struct*)
_tmp1177)->f1;_LL4494: _tmp118D=(void*)_tmp1185.elt_typ;goto _LL4492;_LL4492:
_tmp118B=(void*)_tmp1185.rgn_typ;goto _LL4490;_LL4490: _tmp1189=_tmp1185.tq;goto
_LL4488;_LL4488: _tmp1187=_tmp1185.bounds;goto _LL4478;}else{goto _LL4479;}_LL4479:
goto _LL4480;_LL4474: return({struct _tuple7 _tmp118F;_tmp118F.f1=_tmp1181.q_const;
_tmp118F.f2=(Cyc_Tcutil_addressof_props(te,_tmp10FC)).f2;_tmp118F;});_LL4476: {
struct _tuple4*_tmp1190=Cyc_Absyn_lookup_tuple_field(_tmp1183,(int)Cyc_Evexp_eval_const_uint_exp(
_tmp10FA));if(_tmp1190 != 0){return({struct _tuple7 _tmp1191;_tmp1191.f1=((*
_tmp1190).f1).q_const;_tmp1191.f2=(Cyc_Tcutil_addressof_props(te,_tmp10FC)).f2;
_tmp1191;});}return bogus_ans;}_LL4478: return({struct _tuple7 _tmp1192;_tmp1192.f1=
_tmp1189.q_const;_tmp1192.f2=_tmp118B;_tmp1192;});_LL4480: return bogus_ans;
_LL4472:;}_LL4331:({void*_tmp1193[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("unary & applied to non-lvalue",
sizeof(unsigned char),30),_tag_arr(_tmp1193,sizeof(void*),0));});return bogus_ans;
_LL4319:;}void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct
Cyc_Absyn_Exp*e){void*_tmp1195=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_Tqual
_tmp119B;void*_tmp119D;_LL4503: if((unsigned int)_tmp1195 > 3?*((int*)_tmp1195)== 
Cyc_Absyn_ArrayType: 0){_LL4510: _tmp119D=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp1195)->f1;goto _LL4508;_LL4508: _tmp119B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1195)->f2;goto _LL4504;}else{goto _LL4505;}_LL4505: goto _LL4506;_LL4504: {void*
_tmp11A1;struct _tuple7 _tmp119F=Cyc_Tcutil_addressof_props(te,e);_LL4514: _tmp11A1=
_tmp119F.f2;goto _LL4512;_LL4512: return Cyc_Absyn_atb_typ(_tmp119D,_tmp11A1,
_tmp119B,(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp11A3=_cycalloc(sizeof(
struct Cyc_Absyn_Upper_b_struct));_tmp11A3[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp11A4;_tmp11A4.tag=Cyc_Absyn_Upper_b;_tmp11A4.f1=e;_tmp11A4;});_tmp11A3;}));}
_LL4506: return e_typ;_LL4502:;}void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*
loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(b);{void*
_tmp11A5=(void*)b->v;void*_tmp11AD;void*_tmp11AF;struct Cyc_Absyn_Exp*_tmp11B1;
_LL4519: if((unsigned int)_tmp11A5 > 1?*((int*)_tmp11A5)== Cyc_Absyn_Eq_constr: 0){
_LL4526: _tmp11AD=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp11A5)->f1;if(
_tmp11AD == (void*)Cyc_Absyn_Unknown_b){goto _LL4520;}else{goto _LL4521;}}else{goto
_LL4521;}_LL4521: if((unsigned int)_tmp11A5 > 1?*((int*)_tmp11A5)== Cyc_Absyn_Eq_constr:
0){_LL4528: _tmp11AF=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp11A5)->f1;if((
unsigned int)_tmp11AF > 1?*((int*)_tmp11AF)== Cyc_Absyn_Upper_b: 0){_LL4530:
_tmp11B1=((struct Cyc_Absyn_Upper_b_struct*)_tmp11AF)->f1;goto _LL4522;}else{goto
_LL4523;}}else{goto _LL4523;}_LL4523: goto _LL4524;_LL4520: return;_LL4522: if(Cyc_Evexp_eval_const_uint_exp(
_tmp11B1)<= i){({struct Cyc_Std_Int_pa_struct _tmp11B5;_tmp11B5.tag=Cyc_Std_Int_pa;
_tmp11B5.f1=(int)i;{struct Cyc_Std_Int_pa_struct _tmp11B4;_tmp11B4.tag=Cyc_Std_Int_pa;
_tmp11B4.f1=(int)Cyc_Evexp_eval_const_uint_exp(_tmp11B1);{void*_tmp11B3[2]={&
_tmp11B4,& _tmp11B5};Cyc_Tcutil_terr(loc,_tag_arr("dereference is out of bounds: %d <= %d",
sizeof(unsigned char),39),_tag_arr(_tmp11B3,sizeof(void*),2));}}});}return;
_LL4524:(void*)(b->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp11B6=
_cycalloc(sizeof(struct Cyc_Absyn_Eq_constr_struct));_tmp11B6[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp11B7;_tmp11B7.tag=Cyc_Absyn_Eq_constr;_tmp11B7.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp11B8=_cycalloc(sizeof(struct Cyc_Absyn_Upper_b_struct));_tmp11B8[0]=({struct
Cyc_Absyn_Upper_b_struct _tmp11B9;_tmp11B9.tag=Cyc_Absyn_Upper_b;_tmp11B9.f1=Cyc_Absyn_uint_exp(
i + 1,0);_tmp11B9;});_tmp11B8;}));_tmp11B7;});_tmp11B6;})));return;_LL4518:;}}
void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Conref*
b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*
b){void*_tmp11BA=(void*)(Cyc_Absyn_compress_conref(b))->v;void*_tmp11C0;struct
Cyc_Absyn_Exp*_tmp11C2;_LL4540: if((unsigned int)_tmp11BA > 1?*((int*)_tmp11BA)== 
Cyc_Absyn_Eq_constr: 0){_LL4545: _tmp11C0=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp11BA)->f1;if((unsigned int)_tmp11C0 > 1?*((int*)_tmp11C0)== Cyc_Absyn_Upper_b:
0){_LL4547: _tmp11C2=((struct Cyc_Absyn_Upper_b_struct*)_tmp11C0)->f1;goto _LL4541;}
else{goto _LL4542;}}else{goto _LL4542;}_LL4542: goto _LL4543;_LL4541: return Cyc_Evexp_eval_const_uint_exp(
_tmp11C2)== 1;_LL4543: return 0;_LL4539:;}int Cyc_Tcutil_bits_only(void*t){void*
_tmp11C4=Cyc_Tcutil_compress(t);void*_tmp11E0;struct Cyc_List_List*_tmp11E2;
struct Cyc_Absyn_Structdecl**_tmp11E4;struct Cyc_List_List*_tmp11E6;struct Cyc_Absyn_Uniondecl**
_tmp11E8;struct Cyc_List_List*_tmp11EA;struct Cyc_List_List*_tmp11EC;struct Cyc_List_List*
_tmp11EE;_LL4550: if(_tmp11C4 == (void*)Cyc_Absyn_VoidType){goto _LL4551;}else{goto
_LL4552;}_LL4552: if((unsigned int)_tmp11C4 > 3?*((int*)_tmp11C4)== Cyc_Absyn_IntType:
0){goto _LL4553;}else{goto _LL4554;}_LL4554: if(_tmp11C4 == (void*)Cyc_Absyn_FloatType){
goto _LL4555;}else{goto _LL4556;}_LL4556: if((unsigned int)_tmp11C4 > 3?*((int*)
_tmp11C4)== Cyc_Absyn_DoubleType: 0){goto _LL4557;}else{goto _LL4558;}_LL4558: if((
unsigned int)_tmp11C4 > 3?*((int*)_tmp11C4)== Cyc_Absyn_EnumType: 0){goto _LL4559;}
else{goto _LL4560;}_LL4560: if((unsigned int)_tmp11C4 > 3?*((int*)_tmp11C4)== Cyc_Absyn_AnonEnumType:
0){goto _LL4561;}else{goto _LL4562;}_LL4562: if((unsigned int)_tmp11C4 > 3?*((int*)
_tmp11C4)== Cyc_Absyn_ArrayType: 0){_LL4577: _tmp11E0=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp11C4)->f1;goto _LL4563;}else{goto _LL4564;}_LL4564: if((unsigned int)_tmp11C4 > 
3?*((int*)_tmp11C4)== Cyc_Absyn_TupleType: 0){_LL4579: _tmp11E2=((struct Cyc_Absyn_TupleType_struct*)
_tmp11C4)->f1;goto _LL4565;}else{goto _LL4566;}_LL4566: if((unsigned int)_tmp11C4 > 
3?*((int*)_tmp11C4)== Cyc_Absyn_StructType: 0){_LL4583: _tmp11E6=((struct Cyc_Absyn_StructType_struct*)
_tmp11C4)->f2;goto _LL4581;_LL4581: _tmp11E4=((struct Cyc_Absyn_StructType_struct*)
_tmp11C4)->f3;goto _LL4567;}else{goto _LL4568;}_LL4568: if((unsigned int)_tmp11C4 > 
3?*((int*)_tmp11C4)== Cyc_Absyn_UnionType: 0){_LL4587: _tmp11EA=((struct Cyc_Absyn_UnionType_struct*)
_tmp11C4)->f2;goto _LL4585;_LL4585: _tmp11E8=((struct Cyc_Absyn_UnionType_struct*)
_tmp11C4)->f3;goto _LL4569;}else{goto _LL4570;}_LL4570: if((unsigned int)_tmp11C4 > 
3?*((int*)_tmp11C4)== Cyc_Absyn_AnonStructType: 0){_LL4589: _tmp11EC=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp11C4)->f1;goto _LL4571;}else{goto _LL4572;}_LL4572: if((unsigned int)_tmp11C4 > 
3?*((int*)_tmp11C4)== Cyc_Absyn_AnonUnionType: 0){_LL4591: _tmp11EE=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp11C4)->f1;goto _LL4573;}else{goto _LL4574;}_LL4574: goto _LL4575;_LL4551: goto
_LL4553;_LL4553: goto _LL4555;_LL4555: goto _LL4557;_LL4557: return 1;_LL4559: return 0;
_LL4561: return 0;_LL4563: return Cyc_Tcutil_bits_only(_tmp11E0);_LL4565: for(0;
_tmp11E2 != 0;_tmp11E2=_tmp11E2->tl){if(! Cyc_Tcutil_bits_only((*((struct _tuple4*)
_tmp11E2->hd)).f2)){return 0;}}return 1;_LL4567: if(_tmp11E4 == 0){return 0;}{struct
Cyc_Absyn_Structdecl*_tmp11F0=*_tmp11E4;if(_tmp11F0->fields == 0){return 0;}{
struct _RegionHandle _tmp11F1=_new_region("rgn");struct _RegionHandle*rgn=& _tmp11F1;
_push_region(rgn);{struct Cyc_List_List*_tmp11F2=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp11F0->tvs,_tmp11E6);{struct Cyc_List_List*fs=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp11F0->fields))->v;
for(0;fs != 0;fs=fs->tl){if(! Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,
_tmp11F2,(void*)((struct Cyc_Absyn_Structfield*)fs->hd)->type))){int _tmp11F3=0;
_npop_handler(0);return _tmp11F3;}}}{int _tmp11F4=1;_npop_handler(0);return
_tmp11F4;}};_pop_region(rgn);}}_LL4569: if(_tmp11E8 == 0){return 0;}{struct Cyc_Absyn_Uniondecl*
_tmp11F5=*_tmp11E8;if(_tmp11F5->fields == 0){return 0;}{struct _RegionHandle
_tmp11F6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp11F6;_push_region(rgn);{
struct Cyc_List_List*_tmp11F7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,
struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(
rgn,rgn,_tmp11F5->tvs,_tmp11EA);{struct Cyc_List_List*fs=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp11F5->fields))->v;for(0;fs != 0;fs=fs->tl){
if(! Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp11F7,(void*)((struct Cyc_Absyn_Structfield*)
fs->hd)->type))){int _tmp11F8=0;_npop_handler(0);return _tmp11F8;}}}{int _tmp11F9=1;
_npop_handler(0);return _tmp11F9;}};_pop_region(rgn);}}_LL4571: _tmp11EE=_tmp11EC;
goto _LL4573;_LL4573: for(0;_tmp11EE != 0;_tmp11EE=_tmp11EE->tl){if(! Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Structfield*)_tmp11EE->hd)->type)){return 0;}}return 1;
_LL4575: return 0;_LL4549:;}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmp11FA=(void*)e->r;void*_tmp1228;struct _tuple1*_tmp122A;struct Cyc_Absyn_Exp*
_tmp122C;struct Cyc_Absyn_Exp*_tmp122E;struct Cyc_Absyn_Exp*_tmp1230;struct Cyc_Absyn_Exp*
_tmp1232;struct Cyc_Absyn_Exp*_tmp1234;struct Cyc_Absyn_Exp*_tmp1236;struct Cyc_Absyn_Exp*
_tmp1238;struct Cyc_Absyn_Exp*_tmp123A;void*_tmp123C;struct Cyc_Absyn_Exp*_tmp123E;
struct Cyc_Absyn_Exp*_tmp1240;struct Cyc_Absyn_Exp*_tmp1242;struct Cyc_List_List*
_tmp1244;struct Cyc_List_List*_tmp1246;struct Cyc_List_List*_tmp1248;struct Cyc_List_List*
_tmp124A;void*_tmp124C;struct Cyc_List_List*_tmp124E;struct Cyc_List_List*_tmp1250;
_LL4604: if(*((int*)_tmp11FA)== Cyc_Absyn_Const_e){goto _LL4605;}else{goto _LL4606;}
_LL4606: if(*((int*)_tmp11FA)== Cyc_Absyn_Sizeoftyp_e){goto _LL4607;}else{goto
_LL4608;}_LL4608: if(*((int*)_tmp11FA)== Cyc_Absyn_Sizeofexp_e){goto _LL4609;}
else{goto _LL4610;}_LL4610: if(*((int*)_tmp11FA)== Cyc_Absyn_Offsetof_e){goto
_LL4611;}else{goto _LL4612;}_LL4612: if(*((int*)_tmp11FA)== Cyc_Absyn_Gentyp_e){
goto _LL4613;}else{goto _LL4614;}_LL4614: if(*((int*)_tmp11FA)== Cyc_Absyn_Enum_e){
goto _LL4615;}else{goto _LL4616;}_LL4616: if(*((int*)_tmp11FA)== Cyc_Absyn_AnonEnum_e){
goto _LL4617;}else{goto _LL4618;}_LL4618: if(*((int*)_tmp11FA)== Cyc_Absyn_Var_e){
_LL4651: _tmp122A=((struct Cyc_Absyn_Var_e_struct*)_tmp11FA)->f1;goto _LL4649;
_LL4649: _tmp1228=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp11FA)->f2;goto
_LL4619;}else{goto _LL4620;}_LL4620: if(*((int*)_tmp11FA)== Cyc_Absyn_Conditional_e){
_LL4657: _tmp1230=((struct Cyc_Absyn_Conditional_e_struct*)_tmp11FA)->f1;goto
_LL4655;_LL4655: _tmp122E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp11FA)->f2;
goto _LL4653;_LL4653: _tmp122C=((struct Cyc_Absyn_Conditional_e_struct*)_tmp11FA)->f3;
goto _LL4621;}else{goto _LL4622;}_LL4622: if(*((int*)_tmp11FA)== Cyc_Absyn_SeqExp_e){
_LL4661: _tmp1234=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp11FA)->f1;goto _LL4659;
_LL4659: _tmp1232=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp11FA)->f2;goto _LL4623;}
else{goto _LL4624;}_LL4624: if(*((int*)_tmp11FA)== Cyc_Absyn_NoInstantiate_e){
_LL4663: _tmp1236=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp11FA)->f1;goto
_LL4625;}else{goto _LL4626;}_LL4626: if(*((int*)_tmp11FA)== Cyc_Absyn_Instantiate_e){
_LL4665: _tmp1238=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp11FA)->f1;goto
_LL4627;}else{goto _LL4628;}_LL4628: if(*((int*)_tmp11FA)== Cyc_Absyn_Cast_e){
_LL4669: _tmp123C=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp11FA)->f1;goto
_LL4667;_LL4667: _tmp123A=((struct Cyc_Absyn_Cast_e_struct*)_tmp11FA)->f2;goto
_LL4629;}else{goto _LL4630;}_LL4630: if(*((int*)_tmp11FA)== Cyc_Absyn_Address_e){
_LL4671: _tmp123E=((struct Cyc_Absyn_Address_e_struct*)_tmp11FA)->f1;goto _LL4631;}
else{goto _LL4632;}_LL4632: if(*((int*)_tmp11FA)== Cyc_Absyn_Comprehension_e){
_LL4675: _tmp1242=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp11FA)->f2;goto
_LL4673;_LL4673: _tmp1240=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp11FA)->f3;
goto _LL4633;}else{goto _LL4634;}_LL4634: if(*((int*)_tmp11FA)== Cyc_Absyn_Array_e){
_LL4677: _tmp1244=((struct Cyc_Absyn_Array_e_struct*)_tmp11FA)->f1;goto _LL4635;}
else{goto _LL4636;}_LL4636: if(*((int*)_tmp11FA)== Cyc_Absyn_AnonStruct_e){_LL4679:
_tmp1246=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp11FA)->f2;goto _LL4637;}else{
goto _LL4638;}_LL4638: if(*((int*)_tmp11FA)== Cyc_Absyn_Struct_e){_LL4681: _tmp1248=((
struct Cyc_Absyn_Struct_e_struct*)_tmp11FA)->f3;goto _LL4639;}else{goto _LL4640;}
_LL4640: if(*((int*)_tmp11FA)== Cyc_Absyn_Primop_e){_LL4685: _tmp124C=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp11FA)->f1;goto _LL4683;_LL4683: _tmp124A=((
struct Cyc_Absyn_Primop_e_struct*)_tmp11FA)->f2;goto _LL4641;}else{goto _LL4642;}
_LL4642: if(*((int*)_tmp11FA)== Cyc_Absyn_Tuple_e){_LL4687: _tmp124E=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp11FA)->f1;goto _LL4643;}else{goto _LL4644;}_LL4644: if(*((int*)_tmp11FA)== Cyc_Absyn_Tunion_e){
_LL4689: _tmp1250=((struct Cyc_Absyn_Tunion_e_struct*)_tmp11FA)->f3;goto _LL4645;}
else{goto _LL4646;}_LL4646: goto _LL4647;_LL4605: goto _LL4607;_LL4607: goto _LL4609;
_LL4609: goto _LL4611;_LL4611: goto _LL4613;_LL4613: goto _LL4615;_LL4615: goto _LL4617;
_LL4617: return 1;_LL4619: {void*_tmp1252=_tmp1228;struct Cyc_Absyn_Vardecl*
_tmp125C;_LL4692: if((unsigned int)_tmp1252 > 1?*((int*)_tmp1252)== Cyc_Absyn_Funname_b:
0){goto _LL4693;}else{goto _LL4694;}_LL4694: if((unsigned int)_tmp1252 > 1?*((int*)
_tmp1252)== Cyc_Absyn_Global_b: 0){_LL4701: _tmp125C=((struct Cyc_Absyn_Global_b_struct*)
_tmp1252)->f1;goto _LL4695;}else{goto _LL4696;}_LL4696: if(_tmp1252 == (void*)Cyc_Absyn_Unresolved_b){
goto _LL4697;}else{goto _LL4698;}_LL4698: goto _LL4699;_LL4693: return 1;_LL4695: {
void*_tmp125E=Cyc_Tcutil_compress((void*)_tmp125C->type);_LL4704: if((
unsigned int)_tmp125E > 3?*((int*)_tmp125E)== Cyc_Absyn_ArrayType: 0){goto _LL4705;}
else{goto _LL4706;}_LL4706: if((unsigned int)_tmp125E > 3?*((int*)_tmp125E)== Cyc_Absyn_FnType:
0){goto _LL4707;}else{goto _LL4708;}_LL4708: goto _LL4709;_LL4705: return 1;_LL4707:
return 1;_LL4709: return var_okay;_LL4703:;}_LL4697: return 0;_LL4699: return var_okay;
_LL4691:;}_LL4621: return(Cyc_Tcutil_cnst_exp(te,0,_tmp1230)? Cyc_Tcutil_cnst_exp(
te,0,_tmp122E): 0)? Cyc_Tcutil_cnst_exp(te,0,_tmp122C): 0;_LL4623: return Cyc_Tcutil_cnst_exp(
te,0,_tmp1234)? Cyc_Tcutil_cnst_exp(te,0,_tmp1232): 0;_LL4625: _tmp1238=_tmp1236;
goto _LL4627;_LL4627: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp1238);_LL4629:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp123A);_LL4631: return Cyc_Tcutil_cnst_exp(
te,1,_tmp123E);_LL4633: return Cyc_Tcutil_cnst_exp(te,0,_tmp1242)? Cyc_Tcutil_cnst_exp(
te,0,_tmp1240): 0;_LL4635: _tmp1246=_tmp1244;goto _LL4637;_LL4637: _tmp1248=_tmp1246;
goto _LL4639;_LL4639: for(0;_tmp1248 != 0;_tmp1248=_tmp1248->tl){if(! Cyc_Tcutil_cnst_exp(
te,0,(*((struct _tuple21*)_tmp1248->hd)).f2)){return 0;}}return 1;_LL4641: _tmp124E=
_tmp124A;goto _LL4643;_LL4643: _tmp1250=_tmp124E;goto _LL4645;_LL4645: for(0;
_tmp1250 != 0;_tmp1250=_tmp1250->tl){if(! Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)
_tmp1250->hd)){return 0;}}return 1;_LL4647: return 0;_LL4603:;}int Cyc_Tcutil_is_const_exp(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(void*t){void*_tmp1266=
Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp1284;struct Cyc_Absyn_Conref*
_tmp1286;struct Cyc_Absyn_Conref*_tmp1288;void*_tmp128A;struct Cyc_List_List*
_tmp128C;struct Cyc_Absyn_Structdecl**_tmp128E;struct Cyc_List_List*_tmp1290;
struct Cyc_Absyn_Uniondecl**_tmp1292;struct Cyc_List_List*_tmp1294;struct Cyc_List_List*
_tmp1296;struct Cyc_List_List*_tmp1298;_LL4712: if(_tmp1266 == (void*)Cyc_Absyn_VoidType){
goto _LL4713;}else{goto _LL4714;}_LL4714: if((unsigned int)_tmp1266 > 3?*((int*)
_tmp1266)== Cyc_Absyn_IntType: 0){goto _LL4715;}else{goto _LL4716;}_LL4716: if(
_tmp1266 == (void*)Cyc_Absyn_FloatType){goto _LL4717;}else{goto _LL4718;}_LL4718:
if((unsigned int)_tmp1266 > 3?*((int*)_tmp1266)== Cyc_Absyn_DoubleType: 0){goto
_LL4719;}else{goto _LL4720;}_LL4720: if((unsigned int)_tmp1266 > 3?*((int*)_tmp1266)
== Cyc_Absyn_PointerType: 0){_LL4741: _tmp1284=((struct Cyc_Absyn_PointerType_struct*)
_tmp1266)->f1;_LL4745: _tmp1288=_tmp1284.nullable;goto _LL4743;_LL4743: _tmp1286=
_tmp1284.bounds;goto _LL4721;}else{goto _LL4722;}_LL4722: if((unsigned int)_tmp1266
> 3?*((int*)_tmp1266)== Cyc_Absyn_ArrayType: 0){_LL4747: _tmp128A=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp1266)->f1;goto _LL4723;}else{goto _LL4724;}_LL4724:
if((unsigned int)_tmp1266 > 3?*((int*)_tmp1266)== Cyc_Absyn_TupleType: 0){_LL4749:
_tmp128C=((struct Cyc_Absyn_TupleType_struct*)_tmp1266)->f1;goto _LL4725;}else{
goto _LL4726;}_LL4726: if((unsigned int)_tmp1266 > 3?*((int*)_tmp1266)== Cyc_Absyn_StructType:
0){_LL4753: _tmp1290=((struct Cyc_Absyn_StructType_struct*)_tmp1266)->f2;goto
_LL4751;_LL4751: _tmp128E=((struct Cyc_Absyn_StructType_struct*)_tmp1266)->f3;goto
_LL4727;}else{goto _LL4728;}_LL4728: if((unsigned int)_tmp1266 > 3?*((int*)_tmp1266)
== Cyc_Absyn_UnionType: 0){_LL4757: _tmp1294=((struct Cyc_Absyn_UnionType_struct*)
_tmp1266)->f2;goto _LL4755;_LL4755: _tmp1292=((struct Cyc_Absyn_UnionType_struct*)
_tmp1266)->f3;goto _LL4729;}else{goto _LL4730;}_LL4730: if((unsigned int)_tmp1266 > 
3?*((int*)_tmp1266)== Cyc_Absyn_AnonStructType: 0){_LL4759: _tmp1296=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp1266)->f1;goto _LL4731;}else{goto _LL4732;}_LL4732: if((unsigned int)_tmp1266 > 
3?*((int*)_tmp1266)== Cyc_Absyn_AnonUnionType: 0){_LL4761: _tmp1298=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp1266)->f1;goto _LL4733;}else{goto _LL4734;}_LL4734: if((unsigned int)_tmp1266 > 
3?*((int*)_tmp1266)== Cyc_Absyn_AnonEnumType: 0){goto _LL4735;}else{goto _LL4736;}
_LL4736: if((unsigned int)_tmp1266 > 3?*((int*)_tmp1266)== Cyc_Absyn_EnumType: 0){
goto _LL4737;}else{goto _LL4738;}_LL4738: goto _LL4739;_LL4713: goto _LL4715;_LL4715:
goto _LL4717;_LL4717: goto _LL4719;_LL4719: return 1;_LL4721: {void*_tmp129A=(void*)(
Cyc_Absyn_compress_conref(_tmp1286))->v;void*_tmp12A2;void*_tmp12A4;_LL4764: if((
unsigned int)_tmp129A > 1?*((int*)_tmp129A)== Cyc_Absyn_Eq_constr: 0){_LL4771:
_tmp12A2=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp129A)->f1;if(_tmp12A2 == (
void*)Cyc_Absyn_Unknown_b){goto _LL4765;}else{goto _LL4766;}}else{goto _LL4766;}
_LL4766: if((unsigned int)_tmp129A > 1?*((int*)_tmp129A)== Cyc_Absyn_Eq_constr: 0){
_LL4773: _tmp12A4=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp129A)->f1;if((
unsigned int)_tmp12A4 > 1?*((int*)_tmp12A4)== Cyc_Absyn_Upper_b: 0){goto _LL4767;}
else{goto _LL4768;}}else{goto _LL4768;}_LL4768: goto _LL4769;_LL4765: return 1;_LL4767: {
void*_tmp12A6=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp1288))->v;int _tmp12AC;_LL4776: if((unsigned int)_tmp12A6 > 1?*((int*)_tmp12A6)
== Cyc_Absyn_Eq_constr: 0){_LL4781: _tmp12AC=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp12A6)->f1;goto _LL4777;}else{goto _LL4778;}_LL4778: goto _LL4779;_LL4777: return
_tmp12AC;_LL4779: return 0;_LL4775:;}_LL4769: return 0;_LL4763:;}_LL4723: return Cyc_Tcutil_supports_default(
_tmp128A);_LL4725: for(0;_tmp128C != 0;_tmp128C=_tmp128C->tl){if(! Cyc_Tcutil_supports_default((*((
struct _tuple4*)_tmp128C->hd)).f2)){return 0;}}return 1;_LL4727: if(_tmp128E == 0){
return 0;}{struct Cyc_Absyn_Structdecl*sd=*_tmp128E;if(sd->fields == 0){return 0;}
return Cyc_Tcutil_fields_support_default(sd->tvs,_tmp1290,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(sd->fields))->v);}_LL4729: if(_tmp1292 == 0){
return 0;}{struct Cyc_Absyn_Uniondecl*ud=*_tmp1292;if(ud->fields == 0){return 0;}
return Cyc_Tcutil_fields_support_default(ud->tvs,_tmp1294,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(ud->fields))->v);}_LL4731: _tmp1298=_tmp1296;goto
_LL4733;_LL4733: return Cyc_Tcutil_fields_support_default(0,0,_tmp1298);_LL4735:
goto _LL4737;_LL4737: return 1;_LL4739: return 0;_LL4711:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct
_RegionHandle _tmp12AE=_new_region("rgn");struct _RegionHandle*rgn=& _tmp12AE;
_push_region(rgn);{struct Cyc_List_List*_tmp12AF=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp12AF,(void*)((struct Cyc_Absyn_Structfield*)fs->hd)->type);if(! Cyc_Tcutil_supports_default(
t)){int _tmp12B0=0;_npop_handler(0);return _tmp12B0;}}};_pop_region(rgn);}return 1;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp12B1=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_PtrInfo _tmp12B9;void*_tmp12BB;struct Cyc_Absyn_FnInfo _tmp12BD;struct Cyc_List_List*
_tmp12BF;_LL4787: if((unsigned int)_tmp12B1 > 3?*((int*)_tmp12B1)== Cyc_Absyn_PointerType:
0){_LL4794: _tmp12B9=((struct Cyc_Absyn_PointerType_struct*)_tmp12B1)->f1;_LL4796:
_tmp12BB=(void*)_tmp12B9.elt_typ;goto _LL4788;}else{goto _LL4789;}_LL4789: if((
unsigned int)_tmp12B1 > 3?*((int*)_tmp12B1)== Cyc_Absyn_FnType: 0){_LL4798:
_tmp12BD=((struct Cyc_Absyn_FnType_struct*)_tmp12B1)->f1;_LL4800: _tmp12BF=
_tmp12BD.attributes;goto _LL4790;}else{goto _LL4791;}_LL4791: goto _LL4792;_LL4788:
return Cyc_Tcutil_is_noreturn(_tmp12BB);_LL4790: for(0;_tmp12BF != 0;_tmp12BF=
_tmp12BF->tl){void*_tmp12C1=(void*)_tmp12BF->hd;_LL4803: if(_tmp12C1 == (void*)Cyc_Absyn_Noreturn_att){
goto _LL4804;}else{goto _LL4805;}_LL4805: goto _LL4806;_LL4804: return 1;_LL4806:
continue;_LL4802:;}goto _LL4786;_LL4792: goto _LL4786;_LL4786:;}return 0;}
