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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void *_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void *_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)cks_ptr) + cks_elt_sz*cks_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#else
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.base) _throw_null(); \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif

#define _tag_arr(tcurr,elt_sz,num_elts) ({ \
  struct _tagged_arr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void *)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void * _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_arr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _untag_arr(arr,elt_sz,num_elts) ({ \
  struct _tagged_arr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif

#define _get_arr_size(arr,elt_sz) \
  ({struct _tagged_arr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})

#define _tagged_arr_plus(arr,elt_sz,change) ({ \
  struct _tagged_arr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })

#define _tagged_arr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })

#define _tagged_arr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  struct _tagged_arr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })

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
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int
Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct _tagged_arr fmt,struct _tagged_arr);
extern int Cyc_Std_printf(struct _tagged_arr fmt,struct _tagged_arr);extern struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);extern struct
_tagged_arr Cyc_Std_vrprintf(struct _RegionHandle*r1,struct _tagged_arr fmt,struct
_tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};extern struct Cyc_List_List*Cyc_List_list(struct
_tagged_arr);extern int Cyc_List_length(struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct
Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*
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
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern struct Cyc_Position_Error*Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*,struct _tagged_arr);extern unsigned char Cyc_Position_Nocontext[
14];extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;extern void
Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{
void*v;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*
name;int*identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_PtrInfo{void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*
nullable;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Conref*bounds;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple1*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*
tunion_name;struct _tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_TunionType_struct{
int tag;struct Cyc_Absyn_TunionInfo f1;};struct Cyc_Absyn_TunionFieldType_struct{int
tag;struct Cyc_Absyn_TunionFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int
tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;void*f1;struct Cyc_Absyn_Tqual f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_FnType_struct{
int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_StructType_struct{int tag;struct _tuple1*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};struct Cyc_Absyn_UnionType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};
struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_AnonStructType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Core_Opt*f3;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_NonNullable_ps_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Nullable_ps_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};struct Cyc_Absyn_Section_att_struct{int tag;struct _tagged_arr f1;};
struct Cyc_Absyn_Format_att_struct{int tag;void*f1;int f2;int f3;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pointer_mod_struct{int tag;void*
f1;void*f2;struct Cyc_Absyn_Tqual f3;};struct Cyc_Absyn_Function_mod_struct{int tag;
void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int
tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{
int tag;void*f1;unsigned char f2;};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;
short f2;};struct Cyc_Absyn_Int_c_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{
int tag;void*f1;long long f2;};struct Cyc_Absyn_Float_c_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Absyn_String_c_struct{int tag;struct _tagged_arr f1;};
struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;
struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{int tag;
struct _tagged_arr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;unsigned int f1;}
;struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;
struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{int
tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple1*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Primop_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_Absyn_Conditional_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;};struct Cyc_Absyn_Throw_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{int tag;void*f1;struct
Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Address_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*f1;void*f2;
};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_StructMember_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_StructArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple1*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Codegen_e_struct{int tag;struct
Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Fill_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct _tuple3{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;struct _tuple3 condition;
struct _tuple3 delta;struct Cyc_Absyn_Stmt*body;};struct Cyc_Absyn_Exp_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple3
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct
Cyc_Absyn_Goto_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple3 f2;
struct _tuple3 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_SwitchC_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Cut_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Splice_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct _tuple3 f2;};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Region_s_struct{int tag;struct Cyc_Absyn_Tvar*
f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_ForArray_s_struct{
int tag;struct Cyc_Absyn_ForArrayInfo f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;
int f2;};struct Cyc_Absyn_Char_p_struct{int tag;unsigned char f1;};struct Cyc_Absyn_Float_p_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Absyn_Tuple_p_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Pointer_p_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Struct_p_struct{int tag;
struct Cyc_Absyn_Structdecl*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;};struct Cyc_Absyn_Tunion_p_struct{int tag;struct Cyc_Absyn_Tuniondecl*
f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;
};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_Absyn_UnknownFields_p_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;
struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*
cnst_exp;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
void*sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structfield{
struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Struct_d_struct{int tag;struct Cyc_Absyn_Structdecl*
f1;};struct Cyc_Absyn_Union_d_struct{int tag;struct Cyc_Absyn_Uniondecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
15];extern int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);extern int Cyc_Absyn_tvar_cmp(
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
d,void*k);struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_StructRes_struct{
int tag;struct Cyc_Absyn_Structdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*structdecls;struct Cyc_Dict_Dict*uniondecls;struct
Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*
typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;};struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{int tag;void*f1;void*f2;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*
le;};extern void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);extern struct Cyc_Absyn_Structdecl**Cyc_Tcenv_lookup_structdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);extern struct
Cyc_Absyn_Uniondecl**Cyc_Tcenv_lookup_uniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);extern struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);extern struct
Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);extern struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);extern struct
Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);extern int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*,void*r1,void*r2);extern int Cyc_Std_strcmp(struct _tagged_arr
s1,struct _tagged_arr s2);extern int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct
_tagged_arr*);extern struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct
_tagged_arr);extern unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(struct
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
int pos=8;({struct Cyc_Std_String_pa_struct _tmp1;_tmp1.tag=0;_tmp1.f1=(struct
_tagged_arr)s1;{void*_tmp0[1]={& _tmp1};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\t%s and ",
sizeof(unsigned char),9),_tag_arr(_tmp0,sizeof(void*),1));}});pos +=_get_arr_size(
s1,sizeof(unsigned char))+ 5;if(pos >= 80){({void*_tmp2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\n\t",sizeof(unsigned char),3),_tag_arr(_tmp2,sizeof(void*),0));});pos=
8;}({struct Cyc_Std_String_pa_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(struct _tagged_arr)
s2;{void*_tmp3[1]={& _tmp4};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s ",sizeof(
unsigned char),4),_tag_arr(_tmp3,sizeof(void*),1));}});pos +=_get_arr_size(s2,
sizeof(unsigned char))+ 1;if(pos >= 80){({void*_tmp5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\n\t",sizeof(unsigned char),3),_tag_arr(_tmp5,sizeof(void*),0));});pos=
8;}({void*_tmp6[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("failed to unify. ",
sizeof(unsigned char),18),_tag_arr(_tmp6,sizeof(void*),0));});pos +=17;if(Cyc_Tcutil_failure_reason.curr
!= ((struct _tagged_arr)_tag_arr(0,0,0)).curr){if(pos >= 80){({void*_tmp7[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n\t",sizeof(unsigned char),3),_tag_arr(
_tmp7,sizeof(void*),0));});pos=8;}({struct Cyc_Std_String_pa_struct _tmp9;_tmp9.tag=
0;_tmp9.f1=(struct _tagged_arr)Cyc_Tcutil_failure_reason;{void*_tmp8[1]={& _tmp9};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(
_tmp8,sizeof(void*),1));}});}({void*_tmpA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\n",sizeof(unsigned char),2),_tag_arr(_tmpA,sizeof(void*),0));});Cyc_Std_fflush((
struct Cyc_Std___sFILE*)Cyc_Std_stderr);}}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap){struct _tagged_arr msg=(struct
_tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,fmt,ap);({struct Cyc_Std_String_pa_struct
_tmpC;_tmpC.tag=0;_tmpC.f1=(struct _tagged_arr)msg;{void*_tmpB[1]={& _tmpC};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: %s\n",sizeof(unsigned char),11),_tag_arr(_tmpB,
sizeof(void*),1));}});Cyc_Std_fflush((struct Cyc_Std___sFILE*)Cyc_Std_stderr);(
int)_throw((void*)Cyc_Tcutil_TypeErr);}static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){({struct Cyc_Std_String_pa_struct _tmpF;_tmpF.tag=
0;_tmpF.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);{struct Cyc_Std_String_pa_struct _tmpE;_tmpE.tag=0;_tmpE.f1=(
struct _tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd);{void*
_tmpD[2]={& _tmpE,& _tmpF};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::%s ",
sizeof(unsigned char),8),_tag_arr(_tmpD,sizeof(void*),2));}}});}({void*_tmp10[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n",sizeof(unsigned char),2),_tag_arr(
_tmp10,sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___sFILE*)Cyc_Std_stderr);}
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;static struct Cyc_List_List*
Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct Cyc_Position_Segment*sg,
struct _tagged_arr fmt,struct _tagged_arr ap){struct _tagged_arr msg=(struct
_tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,fmt,ap);Cyc_Tcutil_warning_segs=({
struct Cyc_List_List*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->hd=sg;_tmp11->tl=
Cyc_Tcutil_warning_segs;_tmp11;});Cyc_Tcutil_warning_msgs=({struct Cyc_List_List*
_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->hd=({struct _tagged_arr*_tmp13=
_cycalloc(sizeof(*_tmp13));_tmp13[0]=msg;_tmp13;});_tmp12->tl=Cyc_Tcutil_warning_msgs;
_tmp12;});}void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0){
return;}({void*_tmp14[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("***Warnings***\n",
sizeof(unsigned char),16),_tag_arr(_tmp14,sizeof(void*),0));});{struct Cyc_List_List*
_tmp15=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=
0;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_Tcutil_warning_msgs);while(Cyc_Tcutil_warning_msgs != 0){({struct Cyc_Std_String_pa_struct
_tmp18;_tmp18.tag=0;_tmp18.f1=(struct _tagged_arr)*((struct _tagged_arr*)((struct
Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd);{struct Cyc_Std_String_pa_struct
_tmp17;_tmp17.tag=0;_tmp17.f1=(struct _tagged_arr)*((struct _tagged_arr*)((struct
Cyc_List_List*)_check_null(_tmp15))->hd);{void*_tmp16[2]={& _tmp17,& _tmp18};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s: %s\n",sizeof(unsigned char),8),_tag_arr(_tmp16,
sizeof(void*),2));}}});_tmp15=_tmp15->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->tl;}({void*_tmp19[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("**************\n",sizeof(unsigned char),16),_tag_arr(
_tmp19,sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___sFILE*)Cyc_Std_stderr);}}
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;static int Cyc_Tcutil_fast_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){return*((int*)_check_null(tv1->identity))
- *((int*)_check_null(tv2->identity));}void*Cyc_Tcutil_compress(void*t){void*
_tmp1A=t;struct Cyc_Core_Opt*_tmp1B;struct Cyc_Core_Opt*_tmp1C;struct Cyc_Core_Opt**
_tmp1D;struct Cyc_Core_Opt*_tmp1E;struct Cyc_Core_Opt*_tmp1F;struct Cyc_Core_Opt**
_tmp20;_LL1: if((unsigned int)_tmp1A > 3?*((int*)_tmp1A)== 0: 0){_LLB: _tmp1B=((
struct Cyc_Absyn_Evar_struct*)_tmp1A)->f2;if(_tmp1B == 0){goto _LL2;}else{goto _LL3;}}
else{goto _LL3;}_LL3: if((unsigned int)_tmp1A > 3?*((int*)_tmp1A)== 0: 0){_LLC:
_tmp1C=((struct Cyc_Absyn_Evar_struct*)_tmp1A)->f2;_tmp1D=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1A)->f2;goto _LL4;}else{goto _LL5;}_LL5: if((
unsigned int)_tmp1A > 3?*((int*)_tmp1A)== 18: 0){_LLD: _tmp1E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1A)->f3;if(_tmp1E == 0){goto _LL6;}else{goto _LL7;}}else{goto _LL7;}_LL7: if((
unsigned int)_tmp1A > 3?*((int*)_tmp1A)== 18: 0){_LLE: _tmp1F=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1A)->f3;_tmp20=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_TypedefType_struct*)
_tmp1A)->f3;goto _LL8;}else{goto _LL9;}_LL9: goto _LLA;_LL2: return t;_LL4: {void*t2=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(*_tmp1D))->v);if(t2
!= (void*)((struct Cyc_Core_Opt*)_check_null(*_tmp1D))->v){*_tmp1D=({struct Cyc_Core_Opt*
_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->v=(void*)t2;_tmp21;});}return t2;}_LL6:
return t;_LL8: {void*t2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp20))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)_check_null(*
_tmp20))->v){*_tmp20=({struct Cyc_Core_Opt*_tmp22=_cycalloc(sizeof(*_tmp22));
_tmp22->v=(void*)t2;_tmp22;});}return t2;}_LLA: return t;_LL0:;}void*Cyc_Tcutil_copy_type(
void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(
struct Cyc_Absyn_Conref*c){void*_tmp23=(void*)c->v;void*_tmp24;struct Cyc_Absyn_Conref*
_tmp25;_LL10: if((int)_tmp23 == 0){goto _LL11;}else{goto _LL12;}_LL12: if((
unsigned int)_tmp23 > 1?*((int*)_tmp23)== 0: 0){_LL16: _tmp24=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp23)->f1;goto _LL13;}else{goto _LL14;}_LL14: if((unsigned int)_tmp23 > 1?*((int*)
_tmp23)== 1: 0){_LL17: _tmp25=((struct Cyc_Absyn_Forward_constr_struct*)_tmp23)->f1;
goto _LL15;}else{goto _LLF;}_LL11: return Cyc_Absyn_empty_conref();_LL13: return Cyc_Absyn_new_conref(
_tmp24);_LL15: return Cyc_Tcutil_copy_conref(_tmp25);_LLF:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp26=Cyc_Absyn_compress_kb(kb);void*_tmp27;void*_tmp28;_LL19: if(*((
int*)_tmp26)== 0){_LL1F: _tmp27=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp26)->f1;
goto _LL1A;}else{goto _LL1B;}_LL1B: if(*((int*)_tmp26)== 1){goto _LL1C;}else{goto
_LL1D;}_LL1D: if(*((int*)_tmp26)== 2){_LL20: _tmp28=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp26)->f2;goto _LL1E;}else{goto _LL18;}_LL1A: return(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp2A;
_tmp2A.tag=0;_tmp2A.f1=(void*)_tmp27;_tmp2A;});_tmp29;});_LL1C: return(void*)({
struct Cyc_Absyn_Unknown_kb_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B[0]=({
struct Cyc_Absyn_Unknown_kb_struct _tmp2C;_tmp2C.tag=1;_tmp2C.f1=0;_tmp2C;});
_tmp2B;});_LL1E: return(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp2D=_cycalloc(
sizeof(*_tmp2D));_tmp2D[0]=({struct Cyc_Absyn_Less_kb_struct _tmp2E;_tmp2E.tag=2;
_tmp2E.f1=0;_tmp2E.f2=(void*)_tmp28;_tmp2E;});_tmp2D;});_LL18:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){return({struct Cyc_Absyn_Tvar*
_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->name=tv->name;_tmp2F->identity=0;
_tmp2F->kind=(void*)Cyc_Tcutil_copy_kindbound((void*)tv->kind);_tmp2F;});}static
struct _tuple2*Cyc_Tcutil_copy_arg(struct _tuple2*arg){void*_tmp31;struct Cyc_Absyn_Tqual
_tmp32;struct Cyc_Core_Opt*_tmp33;struct _tuple2 _tmp30=*arg;_LL24: _tmp33=_tmp30.f1;
goto _LL23;_LL23: _tmp32=_tmp30.f2;goto _LL22;_LL22: _tmp31=_tmp30.f3;goto _LL21;
_LL21: return({struct _tuple2*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->f1=_tmp33;
_tmp34->f2=_tmp32;_tmp34->f3=Cyc_Tcutil_copy_type(_tmp31);_tmp34;});}static
struct _tuple4*Cyc_Tcutil_copy_tqt(struct _tuple4*arg){void*_tmp36;struct Cyc_Absyn_Tqual
_tmp37;struct _tuple4 _tmp35=*arg;_LL27: _tmp37=_tmp35.f1;goto _LL26;_LL26: _tmp36=
_tmp35.f2;goto _LL25;_LL25: return({struct _tuple4*_tmp38=_cycalloc(sizeof(*_tmp38));
_tmp38->f1=_tmp37;_tmp38->f2=Cyc_Tcutil_copy_type(_tmp36);_tmp38;});}static
struct Cyc_Absyn_Structfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Structfield*f){
return({struct Cyc_Absyn_Structfield*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->name=
f->name;_tmp39->tq=f->tq;_tmp39->type=(void*)Cyc_Tcutil_copy_type((void*)f->type);
_tmp39->width=f->width;_tmp39->attributes=f->attributes;_tmp39;});}struct _tuple8{
void*f1;void*f2;};static struct _tuple8*Cyc_Tcutil_copy_rgncmp(struct _tuple8*x){
struct _tuple8 _tmp3B;void*_tmp3C;void*_tmp3D;struct _tuple8*_tmp3A=x;_tmp3B=*
_tmp3A;_LL2A: _tmp3D=_tmp3B.f1;goto _LL29;_LL29: _tmp3C=_tmp3B.f2;goto _LL28;_LL28:
return({struct _tuple8*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E->f1=Cyc_Tcutil_copy_type(
_tmp3D);_tmp3E->f2=Cyc_Tcutil_copy_type(_tmp3C);_tmp3E;});}static struct Cyc_Absyn_Enumfield*
Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){return({struct Cyc_Absyn_Enumfield*
_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->name=f->name;_tmp3F->tag=f->tag;_tmp3F->loc=
f->loc;_tmp3F;});}void*Cyc_Tcutil_copy_type(void*t){void*_tmp40=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp41;struct Cyc_Absyn_TunionInfo _tmp42;void*_tmp43;
struct Cyc_List_List*_tmp44;void*_tmp45;struct Cyc_Absyn_TunionFieldInfo _tmp46;
struct Cyc_List_List*_tmp47;void*_tmp48;struct Cyc_Absyn_PtrInfo _tmp49;struct Cyc_Absyn_Conref*
_tmp4A;struct Cyc_Absyn_Tqual _tmp4B;struct Cyc_Absyn_Conref*_tmp4C;void*_tmp4D;
void*_tmp4E;void*_tmp4F;void*_tmp50;int _tmp51;struct Cyc_Absyn_Exp*_tmp52;struct
Cyc_Absyn_Tqual _tmp53;void*_tmp54;struct Cyc_Absyn_FnInfo _tmp55;struct Cyc_List_List*
_tmp56;struct Cyc_List_List*_tmp57;struct Cyc_Absyn_VarargInfo*_tmp58;int _tmp59;
struct Cyc_List_List*_tmp5A;void*_tmp5B;struct Cyc_Core_Opt*_tmp5C;struct Cyc_List_List*
_tmp5D;struct Cyc_List_List*_tmp5E;struct Cyc_List_List*_tmp5F;struct _tuple1*
_tmp60;struct Cyc_List_List*_tmp61;struct _tuple1*_tmp62;struct Cyc_List_List*
_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_Absyn_Enumdecl*_tmp65;struct _tuple1*
_tmp66;struct Cyc_List_List*_tmp67;void*_tmp68;void*_tmp69;struct Cyc_List_List*
_tmp6A;struct _tuple1*_tmp6B;void*_tmp6C;struct Cyc_List_List*_tmp6D;void*_tmp6E;
_LL2C: if((int)_tmp40 == 0){goto _LL2D;}else{goto _LL2E;}_LL2E: if((unsigned int)
_tmp40 > 3?*((int*)_tmp40)== 0: 0){goto _LL2F;}else{goto _LL30;}_LL30: if((
unsigned int)_tmp40 > 3?*((int*)_tmp40)== 1: 0){_LL5E: _tmp41=((struct Cyc_Absyn_VarType_struct*)
_tmp40)->f1;goto _LL31;}else{goto _LL32;}_LL32: if((unsigned int)_tmp40 > 3?*((int*)
_tmp40)== 2: 0){_LL5F: _tmp42=((struct Cyc_Absyn_TunionType_struct*)_tmp40)->f1;
_LL62: _tmp45=(void*)_tmp42.tunion_info;goto _LL61;_LL61: _tmp44=_tmp42.targs;goto
_LL60;_LL60: _tmp43=(void*)_tmp42.rgn;goto _LL33;}else{goto _LL34;}_LL34: if((
unsigned int)_tmp40 > 3?*((int*)_tmp40)== 3: 0){_LL63: _tmp46=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp40)->f1;_LL65: _tmp48=(void*)_tmp46.field_info;goto _LL64;_LL64: _tmp47=_tmp46.targs;
goto _LL35;}else{goto _LL36;}_LL36: if((unsigned int)_tmp40 > 3?*((int*)_tmp40)== 4:
0){_LL66: _tmp49=((struct Cyc_Absyn_PointerType_struct*)_tmp40)->f1;_LL6B: _tmp4E=(
void*)_tmp49.elt_typ;goto _LL6A;_LL6A: _tmp4D=(void*)_tmp49.rgn_typ;goto _LL69;
_LL69: _tmp4C=_tmp49.nullable;goto _LL68;_LL68: _tmp4B=_tmp49.tq;goto _LL67;_LL67:
_tmp4A=_tmp49.bounds;goto _LL37;}else{goto _LL38;}_LL38: if((unsigned int)_tmp40 > 3?*((
int*)_tmp40)== 5: 0){_LL6D: _tmp50=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp40)->f1;
goto _LL6C;_LL6C: _tmp4F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp40)->f2;goto
_LL39;}else{goto _LL3A;}_LL3A: if((int)_tmp40 == 1){goto _LL3B;}else{goto _LL3C;}
_LL3C: if((unsigned int)_tmp40 > 3?*((int*)_tmp40)== 6: 0){_LL6E: _tmp51=((struct Cyc_Absyn_DoubleType_struct*)
_tmp40)->f1;goto _LL3D;}else{goto _LL3E;}_LL3E: if((unsigned int)_tmp40 > 3?*((int*)
_tmp40)== 7: 0){_LL71: _tmp54=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp40)->f1;
goto _LL70;_LL70: _tmp53=((struct Cyc_Absyn_ArrayType_struct*)_tmp40)->f2;goto _LL6F;
_LL6F: _tmp52=((struct Cyc_Absyn_ArrayType_struct*)_tmp40)->f3;goto _LL3F;}else{
goto _LL40;}_LL40: if((unsigned int)_tmp40 > 3?*((int*)_tmp40)== 8: 0){_LL72: _tmp55=((
struct Cyc_Absyn_FnType_struct*)_tmp40)->f1;_LL7A: _tmp5D=_tmp55.tvars;goto _LL79;
_LL79: _tmp5C=_tmp55.effect;goto _LL78;_LL78: _tmp5B=(void*)_tmp55.ret_typ;goto
_LL77;_LL77: _tmp5A=_tmp55.args;goto _LL76;_LL76: _tmp59=_tmp55.c_varargs;goto _LL75;
_LL75: _tmp58=_tmp55.cyc_varargs;goto _LL74;_LL74: _tmp57=_tmp55.rgn_po;goto _LL73;
_LL73: _tmp56=_tmp55.attributes;goto _LL41;}else{goto _LL42;}_LL42: if((unsigned int)
_tmp40 > 3?*((int*)_tmp40)== 9: 0){_LL7B: _tmp5E=((struct Cyc_Absyn_TupleType_struct*)
_tmp40)->f1;goto _LL43;}else{goto _LL44;}_LL44: if((unsigned int)_tmp40 > 3?*((int*)
_tmp40)== 10: 0){_LL7D: _tmp60=((struct Cyc_Absyn_StructType_struct*)_tmp40)->f1;
goto _LL7C;_LL7C: _tmp5F=((struct Cyc_Absyn_StructType_struct*)_tmp40)->f2;goto
_LL45;}else{goto _LL46;}_LL46: if((unsigned int)_tmp40 > 3?*((int*)_tmp40)== 11: 0){
_LL7F: _tmp62=((struct Cyc_Absyn_UnionType_struct*)_tmp40)->f1;goto _LL7E;_LL7E:
_tmp61=((struct Cyc_Absyn_UnionType_struct*)_tmp40)->f2;goto _LL47;}else{goto _LL48;}
_LL48: if((unsigned int)_tmp40 > 3?*((int*)_tmp40)== 14: 0){_LL80: _tmp63=((struct
Cyc_Absyn_AnonStructType_struct*)_tmp40)->f1;goto _LL49;}else{goto _LL4A;}_LL4A:
if((unsigned int)_tmp40 > 3?*((int*)_tmp40)== 15: 0){_LL81: _tmp64=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp40)->f1;goto _LL4B;}else{goto _LL4C;}_LL4C: if((unsigned int)_tmp40 > 3?*((int*)
_tmp40)== 12: 0){_LL83: _tmp66=((struct Cyc_Absyn_EnumType_struct*)_tmp40)->f1;goto
_LL82;_LL82: _tmp65=((struct Cyc_Absyn_EnumType_struct*)_tmp40)->f2;goto _LL4D;}
else{goto _LL4E;}_LL4E: if((unsigned int)_tmp40 > 3?*((int*)_tmp40)== 16: 0){_LL84:
_tmp67=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp40)->f1;goto _LL4F;}else{goto
_LL50;}_LL50: if((unsigned int)_tmp40 > 3?*((int*)_tmp40)== 13: 0){_LL85: _tmp68=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp40)->f1;goto _LL51;}else{goto _LL52;}
_LL52: if((unsigned int)_tmp40 > 3?*((int*)_tmp40)== 17: 0){_LL86: _tmp69=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp40)->f1;goto _LL53;}else{goto _LL54;}
_LL54: if((unsigned int)_tmp40 > 3?*((int*)_tmp40)== 18: 0){_LL88: _tmp6B=((struct
Cyc_Absyn_TypedefType_struct*)_tmp40)->f1;goto _LL87;_LL87: _tmp6A=((struct Cyc_Absyn_TypedefType_struct*)
_tmp40)->f2;goto _LL55;}else{goto _LL56;}_LL56: if((int)_tmp40 == 2){goto _LL57;}
else{goto _LL58;}_LL58: if((unsigned int)_tmp40 > 3?*((int*)_tmp40)== 19: 0){_LL89:
_tmp6C=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp40)->f1;goto _LL59;}else{
goto _LL5A;}_LL5A: if((unsigned int)_tmp40 > 3?*((int*)_tmp40)== 20: 0){_LL8A: _tmp6D=((
struct Cyc_Absyn_JoinEff_struct*)_tmp40)->f1;goto _LL5B;}else{goto _LL5C;}_LL5C: if((
unsigned int)_tmp40 > 3?*((int*)_tmp40)== 21: 0){_LL8B: _tmp6E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp40)->f1;goto _LL5D;}else{goto _LL2B;}_LL2D: goto _LL2F;_LL2F: return t;_LL31:
return(void*)({struct Cyc_Absyn_VarType_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));
_tmp6F[0]=({struct Cyc_Absyn_VarType_struct _tmp70;_tmp70.tag=1;_tmp70.f1=Cyc_Tcutil_copy_tvar(
_tmp41);_tmp70;});_tmp6F;});_LL33: return(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Absyn_TunionType_struct
_tmp72;_tmp72.tag=2;_tmp72.f1=({struct Cyc_Absyn_TunionInfo _tmp73;_tmp73.tunion_info=(
void*)_tmp45;_tmp73.targs=Cyc_Tcutil_copy_types(_tmp44);_tmp73.rgn=(void*)Cyc_Tcutil_copy_type(
_tmp43);_tmp73;});_tmp72;});_tmp71;});_LL35: return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp75;_tmp75.tag=3;_tmp75.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp76;_tmp76.field_info=(
void*)_tmp48;_tmp76.targs=Cyc_Tcutil_copy_types(_tmp47);_tmp76;});_tmp75;});
_tmp74;});_LL37: {void*_tmp77=Cyc_Tcutil_copy_type(_tmp4E);void*_tmp78=Cyc_Tcutil_copy_type(
_tmp4D);struct Cyc_Absyn_Conref*_tmp79=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp4C);struct Cyc_Absyn_Tqual _tmp7A=_tmp4B;struct Cyc_Absyn_Conref*
_tmp7B=Cyc_Tcutil_copy_conref(_tmp4A);return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Absyn_PointerType_struct
_tmp7D;_tmp7D.tag=4;_tmp7D.f1=({struct Cyc_Absyn_PtrInfo _tmp7E;_tmp7E.elt_typ=(
void*)_tmp77;_tmp7E.rgn_typ=(void*)_tmp78;_tmp7E.nullable=_tmp79;_tmp7E.tq=
_tmp7A;_tmp7E.bounds=_tmp7B;_tmp7E;});_tmp7D;});_tmp7C;});}_LL39: return(void*)({
struct Cyc_Absyn_IntType_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[0]=({
struct Cyc_Absyn_IntType_struct _tmp80;_tmp80.tag=5;_tmp80.f1=(void*)_tmp50;_tmp80.f2=(
void*)_tmp4F;_tmp80;});_tmp7F;});_LL3B: return t;_LL3D: return(void*)({struct Cyc_Absyn_DoubleType_struct*
_tmp81=_cycalloc_atomic(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_Absyn_DoubleType_struct
_tmp82;_tmp82.tag=6;_tmp82.f1=_tmp51;_tmp82;});_tmp81;});_LL3F: return(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp84;_tmp84.tag=7;_tmp84.f1=(void*)Cyc_Tcutil_copy_type(
_tmp54);_tmp84.f2=_tmp53;_tmp84.f3=_tmp52;_tmp84;});_tmp83;});_LL41: {struct Cyc_List_List*
_tmp85=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp5D);struct Cyc_Core_Opt*
_tmp86=_tmp5C == 0? 0:({struct Cyc_Core_Opt*_tmp90=_cycalloc(sizeof(*_tmp90));
_tmp90->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp5C->v);_tmp90;});void*_tmp87=
Cyc_Tcutil_copy_type(_tmp5B);struct Cyc_List_List*_tmp88=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp5A);int _tmp89=_tmp59;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp58 != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_check_null(_tmp58);
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp8A=_cycalloc(sizeof(*_tmp8A));
_tmp8A->name=cv->name;_tmp8A->tq=cv->tq;_tmp8A->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmp8A->inject=cv->inject;_tmp8A;});}{struct Cyc_List_List*_tmp8B=((
struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp57);struct Cyc_List_List*_tmp8C=_tmp56;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));
_tmp8D[0]=({struct Cyc_Absyn_FnType_struct _tmp8E;_tmp8E.tag=8;_tmp8E.f1=({struct
Cyc_Absyn_FnInfo _tmp8F;_tmp8F.tvars=_tmp85;_tmp8F.effect=_tmp86;_tmp8F.ret_typ=(
void*)_tmp87;_tmp8F.args=_tmp88;_tmp8F.c_varargs=_tmp89;_tmp8F.cyc_varargs=
cyc_varargs2;_tmp8F.rgn_po=_tmp8B;_tmp8F.attributes=_tmp8C;_tmp8F;});_tmp8E;});
_tmp8D;});}}_LL43: return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp91=
_cycalloc(sizeof(*_tmp91));_tmp91[0]=({struct Cyc_Absyn_TupleType_struct _tmp92;
_tmp92.tag=9;_tmp92.f1=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp5E);
_tmp92;});_tmp91;});_LL45: return(void*)({struct Cyc_Absyn_StructType_struct*
_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=({struct Cyc_Absyn_StructType_struct
_tmp94;_tmp94.tag=10;_tmp94.f1=_tmp60;_tmp94.f2=Cyc_Tcutil_copy_types(_tmp5F);
_tmp94.f3=0;_tmp94;});_tmp93;});_LL47: return(void*)({struct Cyc_Absyn_UnionType_struct*
_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[0]=({struct Cyc_Absyn_UnionType_struct
_tmp96;_tmp96.tag=11;_tmp96.f1=_tmp62;_tmp96.f2=Cyc_Tcutil_copy_types(_tmp61);
_tmp96.f3=0;_tmp96;});_tmp95;});_LL49: return(void*)({struct Cyc_Absyn_AnonStructType_struct*
_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Absyn_AnonStructType_struct
_tmp98;_tmp98.tag=14;_tmp98.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*
f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmp63);_tmp98;});_tmp97;});_LL4B: return(void*)({struct Cyc_Absyn_AnonUnionType_struct*
_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Absyn_AnonUnionType_struct
_tmp9A;_tmp9A.tag=15;_tmp9A.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*
f)(struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,
_tmp64);_tmp9A;});_tmp99;});_LL4D: return(void*)({struct Cyc_Absyn_EnumType_struct*
_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({struct Cyc_Absyn_EnumType_struct
_tmp9C;_tmp9C.tag=12;_tmp9C.f1=_tmp66;_tmp9C.f2=_tmp65;_tmp9C;});_tmp9B;});_LL4F:
return(void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmp9D=_cycalloc(sizeof(*
_tmp9D));_tmp9D[0]=({struct Cyc_Absyn_AnonEnumType_struct _tmp9E;_tmp9E.tag=16;
_tmp9E.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp67);_tmp9E;});
_tmp9D;});_LL51: return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmp9F=
_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({struct Cyc_Absyn_SizeofType_struct _tmpA0;
_tmpA0.tag=13;_tmpA0.f1=(void*)Cyc_Tcutil_copy_type(_tmp68);_tmpA0;});_tmp9F;});
_LL53: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpA1=_cycalloc(
sizeof(*_tmpA1));_tmpA1[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmpA2;_tmpA2.tag=
17;_tmpA2.f1=(void*)Cyc_Tcutil_copy_type(_tmp69);_tmpA2;});_tmpA1;});_LL55:
return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpA3=_cycalloc(sizeof(*
_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_TypedefType_struct _tmpA4;_tmpA4.tag=18;
_tmpA4.f1=_tmp6B;_tmpA4.f2=Cyc_Tcutil_copy_types(_tmp6A);_tmpA4.f3=0;_tmpA4;});
_tmpA3;});_LL57: return t;_LL59: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({struct Cyc_Absyn_AccessEff_struct
_tmpA6;_tmpA6.tag=19;_tmpA6.f1=(void*)Cyc_Tcutil_copy_type(_tmp6C);_tmpA6;});
_tmpA5;});_LL5B: return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmpA7=_cycalloc(
sizeof(*_tmpA7));_tmpA7[0]=({struct Cyc_Absyn_JoinEff_struct _tmpA8;_tmpA8.tag=20;
_tmpA8.f1=Cyc_Tcutil_copy_types(_tmp6D);_tmpA8;});_tmpA7;});_LL5D: return(void*)({
struct Cyc_Absyn_RgnsEff_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({
struct Cyc_Absyn_RgnsEff_struct _tmpAA;_tmpAA.tag=21;_tmpAA.f1=(void*)Cyc_Tcutil_copy_type(
_tmp6E);_tmpAA;});_tmpA9;});_LL2B:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(
k1 == k2){return 1;}{struct _tuple8 _tmpAC=({struct _tuple8 _tmpAB;_tmpAB.f1=k1;_tmpAB.f2=
k2;_tmpAB;});void*_tmpAD;void*_tmpAE;void*_tmpAF;void*_tmpB0;void*_tmpB1;void*
_tmpB2;_LL8D: _LL96: _tmpAE=_tmpAC.f1;if((int)_tmpAE == 2){goto _LL95;}else{goto
_LL8F;}_LL95: _tmpAD=_tmpAC.f2;if((int)_tmpAD == 1){goto _LL8E;}else{goto _LL8F;}
_LL8F: _LL98: _tmpB0=_tmpAC.f1;if((int)_tmpB0 == 2){goto _LL97;}else{goto _LL91;}
_LL97: _tmpAF=_tmpAC.f2;if((int)_tmpAF == 0){goto _LL90;}else{goto _LL91;}_LL91:
_LL9A: _tmpB2=_tmpAC.f1;if((int)_tmpB2 == 1){goto _LL99;}else{goto _LL93;}_LL99:
_tmpB1=_tmpAC.f2;if((int)_tmpB1 == 0){goto _LL92;}else{goto _LL93;}_LL93: goto _LL94;
_LL8E: goto _LL90;_LL90: goto _LL92;_LL92: return 1;_LL94: return 0;_LL8C:;}}void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv){void*_tmpB3=Cyc_Absyn_compress_kb((void*)tv->kind);void*
_tmpB4;void*_tmpB5;_LL9C: if(*((int*)_tmpB3)== 0){_LLA2: _tmpB4=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmpB3)->f1;goto _LL9D;}else{goto _LL9E;}_LL9E: if(*((int*)_tmpB3)== 2){_LLA3:
_tmpB5=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpB3)->f2;goto _LL9F;}else{goto
_LLA0;}_LLA0: goto _LLA1;_LL9D: return _tmpB4;_LL9F: return _tmpB5;_LLA1: return({void*
_tmpB6[0]={};Cyc_Tcutil_impos(_tag_arr("kind not suitably constrained!",sizeof(
unsigned char),31),_tag_arr(_tmpB6,sizeof(void*),0));});_LL9B:;}void*Cyc_Tcutil_typ_kind(
void*t){void*_tmpB7=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpB8;struct Cyc_Core_Opt*
_tmpB9;struct Cyc_Absyn_Tvar*_tmpBA;void*_tmpBB;struct Cyc_Absyn_TunionFieldInfo
_tmpBC;void*_tmpBD;struct Cyc_Absyn_Tunionfield*_tmpBE;struct Cyc_Absyn_TunionFieldInfo
_tmpBF;void*_tmpC0;struct Cyc_Absyn_Structdecl**_tmpC1;struct Cyc_Absyn_Uniondecl**
_tmpC2;struct Cyc_Absyn_Enumdecl*_tmpC3;struct Cyc_Absyn_Enumdecl*_tmpC4;struct Cyc_Absyn_PtrInfo
_tmpC5;struct Cyc_Core_Opt*_tmpC6;_LLA5: if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)
== 0: 0){_LLE0: _tmpB9=((struct Cyc_Absyn_Evar_struct*)_tmpB7)->f1;goto _LLDF;_LLDF:
_tmpB8=((struct Cyc_Absyn_Evar_struct*)_tmpB7)->f2;goto _LLA6;}else{goto _LLA7;}
_LLA7: if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 1: 0){_LLE1: _tmpBA=((struct Cyc_Absyn_VarType_struct*)
_tmpB7)->f1;goto _LLA8;}else{goto _LLA9;}_LLA9: if((int)_tmpB7 == 0){goto _LLAA;}
else{goto _LLAB;}_LLAB: if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 5: 0){_LLE2:
_tmpBB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB7)->f2;goto _LLAC;}else{goto
_LLAD;}_LLAD: if((int)_tmpB7 == 1){goto _LLAE;}else{goto _LLAF;}_LLAF: if((
unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 6: 0){goto _LLB0;}else{goto _LLB1;}_LLB1:
if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 8: 0){goto _LLB2;}else{goto _LLB3;}
_LLB3: if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 17: 0){goto _LLB4;}else{goto
_LLB5;}_LLB5: if((int)_tmpB7 == 2){goto _LLB6;}else{goto _LLB7;}_LLB7: if((
unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 2: 0){goto _LLB8;}else{goto _LLB9;}_LLB9:
if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 3: 0){_LLE3: _tmpBC=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpB7)->f1;_LLE4: _tmpBD=(void*)_tmpBC.field_info;if(*((int*)_tmpBD)== 1){_LLE5:
_tmpBE=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmpBD)->f2;goto _LLBA;}else{
goto _LLBB;}}else{goto _LLBB;}_LLBB: if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 3:
0){_LLE6: _tmpBF=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpB7)->f1;_LLE7:
_tmpC0=(void*)_tmpBF.field_info;if(*((int*)_tmpC0)== 0){goto _LLBC;}else{goto
_LLBD;}}else{goto _LLBD;}_LLBD: if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 10: 0){
_LLE8: _tmpC1=((struct Cyc_Absyn_StructType_struct*)_tmpB7)->f3;if(_tmpC1 == 0){
goto _LLBE;}else{goto _LLBF;}}else{goto _LLBF;}_LLBF: if((unsigned int)_tmpB7 > 3?*((
int*)_tmpB7)== 11: 0){_LLE9: _tmpC2=((struct Cyc_Absyn_UnionType_struct*)_tmpB7)->f3;
if(_tmpC2 == 0){goto _LLC0;}else{goto _LLC1;}}else{goto _LLC1;}_LLC1: if((
unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 12: 0){_LLEA: _tmpC3=((struct Cyc_Absyn_EnumType_struct*)
_tmpB7)->f2;if(_tmpC3 == 0){goto _LLC2;}else{goto _LLC3;}}else{goto _LLC3;}_LLC3: if((
unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 10: 0){goto _LLC4;}else{goto _LLC5;}_LLC5:
if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 11: 0){goto _LLC6;}else{goto _LLC7;}
_LLC7: if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 14: 0){goto _LLC8;}else{goto
_LLC9;}_LLC9: if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 16: 0){goto _LLCA;}else{
goto _LLCB;}_LLCB: if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 15: 0){goto _LLCC;}
else{goto _LLCD;}_LLCD: if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 12: 0){_LLEB:
_tmpC4=((struct Cyc_Absyn_EnumType_struct*)_tmpB7)->f2;goto _LLCE;}else{goto _LLCF;}
_LLCF: if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 4: 0){_LLEC: _tmpC5=((struct Cyc_Absyn_PointerType_struct*)
_tmpB7)->f1;goto _LLD0;}else{goto _LLD1;}_LLD1: if((unsigned int)_tmpB7 > 3?*((int*)
_tmpB7)== 13: 0){goto _LLD2;}else{goto _LLD3;}_LLD3: if((unsigned int)_tmpB7 > 3?*((
int*)_tmpB7)== 7: 0){goto _LLD4;}else{goto _LLD5;}_LLD5: if((unsigned int)_tmpB7 > 3?*((
int*)_tmpB7)== 9: 0){goto _LLD6;}else{goto _LLD7;}_LLD7: if((unsigned int)_tmpB7 > 3?*((
int*)_tmpB7)== 18: 0){_LLED: _tmpC6=((struct Cyc_Absyn_TypedefType_struct*)_tmpB7)->f3;
goto _LLD8;}else{goto _LLD9;}_LLD9: if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 19:
0){goto _LLDA;}else{goto _LLDB;}_LLDB: if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)== 
20: 0){goto _LLDC;}else{goto _LLDD;}_LLDD: if((unsigned int)_tmpB7 > 3?*((int*)_tmpB7)
== 21: 0){goto _LLDE;}else{goto _LLA4;}_LLA6: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpB9))->v;_LLA8: return Cyc_Tcutil_tvar_kind(_tmpBA);_LLAA: return(
void*)1;_LLAC: return _tmpBB == (void*)2?(void*)2:(void*)1;_LLAE: goto _LLB0;_LLB0:
goto _LLB2;_LLB2: return(void*)1;_LLB4: return(void*)2;_LLB6: return(void*)3;_LLB8:
return(void*)2;_LLBA: if(_tmpBE->typs == 0){return(void*)2;}else{return(void*)1;}
_LLBC: return({void*_tmpC7[0]={};Cyc_Tcutil_impos(_tag_arr("typ_kind: Unresolved TunionFieldType",
sizeof(unsigned char),37),_tag_arr(_tmpC7,sizeof(void*),0));});_LLBE: goto _LLC0;
_LLC0: goto _LLC2;_LLC2: return(void*)0;_LLC4: goto _LLC6;_LLC6: goto _LLC8;_LLC8: goto
_LLCA;_LLCA: goto _LLCC;_LLCC: return(void*)1;_LLCE: if(_tmpC4->fields == 0){return(
void*)0;}else{return(void*)1;}_LLD0: {void*_tmpC8=(void*)(Cyc_Absyn_compress_conref(
_tmpC5.bounds))->v;void*_tmpC9;void*_tmpCA;_LLEF: if((unsigned int)_tmpC8 > 1?*((
int*)_tmpC8)== 0: 0){_LLF7: _tmpC9=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpC8)->f1;if((int)_tmpC9 == 0){goto _LLF0;}else{goto _LLF1;}}else{goto _LLF1;}
_LLF1: if((unsigned int)_tmpC8 > 1?*((int*)_tmpC8)== 0: 0){_LLF8: _tmpCA=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpC8)->f1;if((unsigned int)_tmpCA > 1?*((int*)
_tmpCA)== 0: 0){goto _LLF2;}else{goto _LLF3;}}else{goto _LLF3;}_LLF3: if((int)_tmpC8
== 0){goto _LLF4;}else{goto _LLF5;}_LLF5: if((unsigned int)_tmpC8 > 1?*((int*)_tmpC8)
== 1: 0){goto _LLF6;}else{goto _LLEE;}_LLF0: return(void*)1;_LLF2: return(void*)2;
_LLF4: return(void*)1;_LLF6: return({void*_tmpCB[0]={};Cyc_Tcutil_impos(_tag_arr("typ_kind: forward constraint in ptr bounds",
sizeof(unsigned char),43),_tag_arr(_tmpCB,sizeof(void*),0));});_LLEE:;}_LLD2:
return(void*)2;_LLD4: goto _LLD6;_LLD6: return(void*)1;_LLD8: return({struct Cyc_Std_String_pa_struct
_tmpCD;_tmpCD.tag=0;_tmpCD.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpCC[1]={& _tmpCD};Cyc_Tcutil_impos(_tag_arr("typ_kind: typedef found: %s",
sizeof(unsigned char),28),_tag_arr(_tmpCC,sizeof(void*),1));}});_LLDA: goto _LLDC;
_LLDC: goto _LLDE;_LLDE: return(void*)4;_LLA4:;}unsigned char Cyc_Tcutil_Unify[10]="\000\000\000\000Unify";
int Cyc_Tcutil_unify(void*t1,void*t2){struct _handler_cons _tmpCE;_push_handler(&
_tmpCE);{int _tmpD0=0;if(setjmp(_tmpCE.handler)){_tmpD0=1;}if(! _tmpD0){Cyc_Tcutil_unify_it(
t1,t2);{int _tmpD1=1;_npop_handler(0);return _tmpD1;};_pop_handler();}else{void*
_tmpCF=(void*)_exn_thrown;void*_tmpD3=_tmpCF;_LLFA: if(_tmpD3 == Cyc_Tcutil_Unify){
goto _LLFB;}else{goto _LLFC;}_LLFC: goto _LLFD;_LLFB: return 0;_LLFD:(void)_throw(
_tmpD3);_LLF9:;}}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*
r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){void*_tmpD4=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmpD5;struct Cyc_Core_Opt*_tmpD6;struct Cyc_Core_Opt**
_tmpD7;struct Cyc_Core_Opt*_tmpD8;struct Cyc_Absyn_PtrInfo _tmpD9;void*_tmpDA;
struct Cyc_Absyn_FnInfo _tmpDB;struct Cyc_List_List*_tmpDC;struct Cyc_List_List*
_tmpDD;struct Cyc_Absyn_VarargInfo*_tmpDE;int _tmpDF;struct Cyc_List_List*_tmpE0;
void*_tmpE1;struct Cyc_Core_Opt*_tmpE2;struct Cyc_List_List*_tmpE3;struct Cyc_List_List*
_tmpE4;void*_tmpE5;struct Cyc_Absyn_TunionInfo _tmpE6;void*_tmpE7;struct Cyc_List_List*
_tmpE8;struct Cyc_Core_Opt*_tmpE9;struct Cyc_List_List*_tmpEA;struct Cyc_Absyn_TunionFieldInfo
_tmpEB;struct Cyc_List_List*_tmpEC;struct Cyc_List_List*_tmpED;struct Cyc_List_List*
_tmpEE;struct Cyc_List_List*_tmpEF;void*_tmpF0;void*_tmpF1;struct Cyc_List_List*
_tmpF2;void*_tmpF3;_LLFF: if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 1: 0){
_LL123: _tmpD5=((struct Cyc_Absyn_VarType_struct*)_tmpD4)->f1;goto _LL100;}else{
goto _LL101;}_LL101: if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 0: 0){_LL125:
_tmpD8=((struct Cyc_Absyn_Evar_struct*)_tmpD4)->f2;goto _LL124;_LL124: _tmpD6=((
struct Cyc_Absyn_Evar_struct*)_tmpD4)->f4;_tmpD7=(struct Cyc_Core_Opt**)&((struct
Cyc_Absyn_Evar_struct*)_tmpD4)->f4;goto _LL102;}else{goto _LL103;}_LL103: if((
unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 4: 0){_LL126: _tmpD9=((struct Cyc_Absyn_PointerType_struct*)
_tmpD4)->f1;goto _LL104;}else{goto _LL105;}_LL105: if((unsigned int)_tmpD4 > 3?*((
int*)_tmpD4)== 7: 0){_LL127: _tmpDA=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmpD4)->f1;goto _LL106;}else{goto _LL107;}_LL107: if((unsigned int)_tmpD4 > 3?*((
int*)_tmpD4)== 8: 0){_LL128: _tmpDB=((struct Cyc_Absyn_FnType_struct*)_tmpD4)->f1;
_LL130: _tmpE3=_tmpDB.tvars;goto _LL12F;_LL12F: _tmpE2=_tmpDB.effect;goto _LL12E;
_LL12E: _tmpE1=(void*)_tmpDB.ret_typ;goto _LL12D;_LL12D: _tmpE0=_tmpDB.args;goto
_LL12C;_LL12C: _tmpDF=_tmpDB.c_varargs;goto _LL12B;_LL12B: _tmpDE=_tmpDB.cyc_varargs;
goto _LL12A;_LL12A: _tmpDD=_tmpDB.rgn_po;goto _LL129;_LL129: _tmpDC=_tmpDB.attributes;
goto _LL108;}else{goto _LL109;}_LL109: if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 
9: 0){_LL131: _tmpE4=((struct Cyc_Absyn_TupleType_struct*)_tmpD4)->f1;goto _LL10A;}
else{goto _LL10B;}_LL10B: if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 17: 0){
_LL132: _tmpE5=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmpD4)->f1;goto
_LL10C;}else{goto _LL10D;}_LL10D: if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 2: 0){
_LL133: _tmpE6=((struct Cyc_Absyn_TunionType_struct*)_tmpD4)->f1;_LL135: _tmpE8=
_tmpE6.targs;goto _LL134;_LL134: _tmpE7=(void*)_tmpE6.rgn;goto _LL10E;}else{goto
_LL10F;}_LL10F: if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 18: 0){_LL137: _tmpEA=((
struct Cyc_Absyn_TypedefType_struct*)_tmpD4)->f2;goto _LL136;_LL136: _tmpE9=((
struct Cyc_Absyn_TypedefType_struct*)_tmpD4)->f3;goto _LL110;}else{goto _LL111;}
_LL111: if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 3: 0){_LL138: _tmpEB=((struct
Cyc_Absyn_TunionFieldType_struct*)_tmpD4)->f1;_LL139: _tmpEC=_tmpEB.targs;goto
_LL112;}else{goto _LL113;}_LL113: if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 10:
0){_LL13A: _tmpED=((struct Cyc_Absyn_StructType_struct*)_tmpD4)->f2;goto _LL114;}
else{goto _LL115;}_LL115: if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 14: 0){
_LL13B: _tmpEE=((struct Cyc_Absyn_AnonStructType_struct*)_tmpD4)->f1;goto _LL116;}
else{goto _LL117;}_LL117: if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 15: 0){
_LL13C: _tmpEF=((struct Cyc_Absyn_AnonUnionType_struct*)_tmpD4)->f1;goto _LL118;}
else{goto _LL119;}_LL119: if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 13: 0){
_LL13D: _tmpF0=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmpD4)->f1;goto _LL11A;}
else{goto _LL11B;}_LL11B: if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 19: 0){
_LL13E: _tmpF1=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmpD4)->f1;goto _LL11C;}
else{goto _LL11D;}_LL11D: if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 20: 0){
_LL13F: _tmpF2=((struct Cyc_Absyn_JoinEff_struct*)_tmpD4)->f1;goto _LL11E;}else{
goto _LL11F;}_LL11F: if((unsigned int)_tmpD4 > 3?*((int*)_tmpD4)== 21: 0){_LL140:
_tmpF3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmpD4)->f1;goto _LL120;}else{
goto _LL121;}_LL121: goto _LL122;_LL100: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,env,_tmpD5)){Cyc_Tcutil_failure_reason=_tag_arr("(type variable would escape scope)",
sizeof(unsigned char),35);(int)_throw((void*)Cyc_Tcutil_Unify);}goto _LLFE;_LL102:
if(t == evar){Cyc_Tcutil_failure_reason=_tag_arr("(occurs check)",sizeof(
unsigned char),15);(int)_throw((void*)Cyc_Tcutil_Unify);}else{if(_tmpD8 != 0){Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmpD8->v);}else{int problem=0;{struct Cyc_List_List*s=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpD7))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){problem=1;break;}}}if(problem){struct Cyc_List_List*
_tmpF4=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpD7))->v;for(0;s != 0;s=s->tl){if(((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
_tmpF4=({struct Cyc_List_List*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->hd=(
struct Cyc_Absyn_Tvar*)s->hd;_tmpF5->tl=_tmpF4;_tmpF5;});}}}*_tmpD7=({struct Cyc_Core_Opt*
_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->v=_tmpF4;_tmpF6;});}}}goto _LLFE;_LL104:
Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpD9.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(
void*)_tmpD9.rgn_typ);goto _LLFE;_LL106: Cyc_Tcutil_occurs(evar,r,env,_tmpDA);goto
_LLFE;_LL108: env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmpE3,env);if(_tmpE2 != 0){Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmpE2->v);}Cyc_Tcutil_occurs(evar,r,env,_tmpE1);for(0;_tmpE0
!= 0;_tmpE0=_tmpE0->tl){Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple2*)_tmpE0->hd)).f3);}
if(_tmpDE != 0){void*_tmpF8;struct Cyc_Absyn_VarargInfo _tmpF7=*_tmpDE;_LL142:
_tmpF8=(void*)_tmpF7.type;goto _LL141;_LL141: Cyc_Tcutil_occurs(evar,r,env,_tmpF8);}
for(0;_tmpDD != 0;_tmpDD=_tmpDD->tl){struct _tuple8 _tmpFA;void*_tmpFB;void*_tmpFC;
struct _tuple8*_tmpF9=(struct _tuple8*)_tmpDD->hd;_tmpFA=*_tmpF9;_LL145: _tmpFC=
_tmpFA.f1;goto _LL144;_LL144: _tmpFB=_tmpFA.f2;goto _LL143;_LL143: Cyc_Tcutil_occurs(
evar,r,env,_tmpFC);Cyc_Tcutil_occurs(evar,r,env,_tmpFB);}goto _LLFE;_LL10A: for(0;
_tmpE4 != 0;_tmpE4=_tmpE4->tl){Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple4*)
_tmpE4->hd)).f2);}goto _LLFE;_LL10C: Cyc_Tcutil_occurs(evar,r,env,_tmpE5);goto
_LLFE;_LL10E: Cyc_Tcutil_occurs(evar,r,env,_tmpE7);Cyc_Tcutil_occurslist(evar,r,
env,_tmpE8);goto _LLFE;_LL110: _tmpEC=_tmpEA;goto _LL112;_LL112: _tmpED=_tmpEC;goto
_LL114;_LL114: Cyc_Tcutil_occurslist(evar,r,env,_tmpED);goto _LLFE;_LL116: _tmpEF=
_tmpEE;goto _LL118;_LL118: for(0;_tmpEF != 0;_tmpEF=_tmpEF->tl){Cyc_Tcutil_occurs(
evar,r,env,(void*)((struct Cyc_Absyn_Structfield*)_tmpEF->hd)->type);}goto _LLFE;
_LL11A: Cyc_Tcutil_occurs(evar,r,env,_tmpF0);goto _LLFE;_LL11C: Cyc_Tcutil_occurs(
evar,r,env,_tmpF1);goto _LLFE;_LL11E: Cyc_Tcutil_occurslist(evar,r,env,_tmpF2);
goto _LLFE;_LL120: Cyc_Tcutil_occurs(evar,r,env,_tmpF3);goto _LLFE;_LL122: goto _LLFE;
_LLFE:;}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct
Cyc_List_List*env,struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(
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
y=Cyc_Absyn_compress_conref(y);if(x == y){return;}{void*_tmpFD=(void*)x->v;void*
_tmpFE;_LL147: if((int)_tmpFD == 0){goto _LL148;}else{goto _LL149;}_LL149: if((
unsigned int)_tmpFD > 1?*((int*)_tmpFD)== 1: 0){goto _LL14A;}else{goto _LL14B;}
_LL14B: if((unsigned int)_tmpFD > 1?*((int*)_tmpFD)== 0: 0){_LL14D: _tmpFE=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpFD)->f1;goto _LL14C;}else{goto _LL146;}
_LL148:(void*)(x->v=(void*)((void*)({struct Cyc_Absyn_Forward_constr_struct*
_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({struct Cyc_Absyn_Forward_constr_struct
_tmp100;_tmp100.tag=1;_tmp100.f1=y;_tmp100;});_tmpFF;})));return;_LL14A:(int)
_throw(({void*_tmp101[0]={};Cyc_Tcutil_impos(_tag_arr("unify_conref: forward after compress",
sizeof(unsigned char),37),_tag_arr(_tmp101,sizeof(void*),0));}));_LL14C: {void*
_tmp102=(void*)y->v;void*_tmp103;_LL14F: if((int)_tmp102 == 0){goto _LL150;}else{
goto _LL151;}_LL151: if((unsigned int)_tmp102 > 1?*((int*)_tmp102)== 1: 0){goto
_LL152;}else{goto _LL153;}_LL153: if((unsigned int)_tmp102 > 1?*((int*)_tmp102)== 0:
0){_LL155: _tmp103=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp102)->f1;goto
_LL154;}else{goto _LL14E;}_LL150:(void*)(y->v=(void*)((void*)x->v));return;_LL152:(
int)_throw(({void*_tmp104[0]={};Cyc_Tcutil_impos(_tag_arr("unify_conref: forward after compress(2)",
sizeof(unsigned char),40),_tag_arr(_tmp104,sizeof(void*),0));}));_LL154: if(cmp(
_tmpFE,_tmp103)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)Cyc_Tcutil_Unify);}
return;_LL14E:;}_LL146:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct _handler_cons _tmp105;
_push_handler(& _tmp105);{int _tmp107=0;if(setjmp(_tmp105.handler)){_tmp107=1;}if(
! _tmp107){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _tagged_arr)_tag_arr(0,0,0));{
int _tmp108=1;_npop_handler(0);return _tmp108;};_pop_handler();}else{void*_tmp106=(
void*)_exn_thrown;void*_tmp10A=_tmp106;_LL157: if(_tmp10A == Cyc_Tcutil_Unify){
goto _LL158;}else{goto _LL159;}_LL159: goto _LL15A;_LL158: return 0;_LL15A:(void)
_throw(_tmp10A);_LL156:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){struct
_tuple8 _tmp10C=({struct _tuple8 _tmp10B;_tmp10B.f1=b1;_tmp10B.f2=b2;_tmp10B;});
void*_tmp10D;void*_tmp10E;void*_tmp10F;void*_tmp110;void*_tmp111;struct Cyc_Absyn_Exp*
_tmp112;void*_tmp113;struct Cyc_Absyn_Exp*_tmp114;_LL15C: _LL165: _tmp10E=_tmp10C.f1;
if((int)_tmp10E == 0){goto _LL164;}else{goto _LL15E;}_LL164: _tmp10D=_tmp10C.f2;if((
int)_tmp10D == 0){goto _LL15D;}else{goto _LL15E;}_LL15E: _LL166: _tmp10F=_tmp10C.f1;
if((int)_tmp10F == 0){goto _LL15F;}else{goto _LL160;}_LL160: _LL167: _tmp110=_tmp10C.f2;
if((int)_tmp110 == 0){goto _LL161;}else{goto _LL162;}_LL162: _LL16A: _tmp113=_tmp10C.f1;
if((unsigned int)_tmp113 > 1?*((int*)_tmp113)== 0: 0){_LL16B: _tmp114=((struct Cyc_Absyn_Upper_b_struct*)
_tmp113)->f1;goto _LL168;}else{goto _LL15B;}_LL168: _tmp111=_tmp10C.f2;if((
unsigned int)_tmp111 > 1?*((int*)_tmp111)== 0: 0){_LL169: _tmp112=((struct Cyc_Absyn_Upper_b_struct*)
_tmp111)->f1;goto _LL163;}else{goto _LL15B;}_LL15D: return 0;_LL15F: return - 1;_LL161:
return 1;_LL163: {int i1=(int)Cyc_Evexp_eval_const_uint_exp(_tmp114);int i2=(int)
Cyc_Evexp_eval_const_uint_exp(_tmp112);if(i1 == i2){return 0;}if(i1 < i2){return - 1;}
return 1;}_LL15B:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){if(a1 == a2){
return 1;}{struct _tuple8 _tmp116=({struct _tuple8 _tmp115;_tmp115.f1=a1;_tmp115.f2=
a2;_tmp115;});void*_tmp117;int _tmp118;int _tmp119;void*_tmp11A;void*_tmp11B;int
_tmp11C;int _tmp11D;void*_tmp11E;void*_tmp11F;int _tmp120;void*_tmp121;int _tmp122;
void*_tmp123;int _tmp124;void*_tmp125;int _tmp126;void*_tmp127;struct _tagged_arr
_tmp128;void*_tmp129;struct _tagged_arr _tmp12A;_LL16D: _LL17B: _tmp11B=_tmp116.f1;
if((unsigned int)_tmp11B > 16?*((int*)_tmp11B)== 3: 0){_LL17E: _tmp11E=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp11B)->f1;goto _LL17D;_LL17D: _tmp11D=((
struct Cyc_Absyn_Format_att_struct*)_tmp11B)->f2;goto _LL17C;_LL17C: _tmp11C=((
struct Cyc_Absyn_Format_att_struct*)_tmp11B)->f3;goto _LL177;}else{goto _LL16F;}
_LL177: _tmp117=_tmp116.f2;if((unsigned int)_tmp117 > 16?*((int*)_tmp117)== 3: 0){
_LL17A: _tmp11A=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp117)->f1;goto
_LL179;_LL179: _tmp119=((struct Cyc_Absyn_Format_att_struct*)_tmp117)->f2;goto
_LL178;_LL178: _tmp118=((struct Cyc_Absyn_Format_att_struct*)_tmp117)->f3;goto
_LL16E;}else{goto _LL16F;}_LL16F: _LL181: _tmp121=_tmp116.f1;if((unsigned int)
_tmp121 > 16?*((int*)_tmp121)== 0: 0){_LL182: _tmp122=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp121)->f1;goto _LL17F;}else{goto _LL171;}_LL17F: _tmp11F=_tmp116.f2;if((
unsigned int)_tmp11F > 16?*((int*)_tmp11F)== 0: 0){_LL180: _tmp120=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp11F)->f1;goto _LL170;}else{goto _LL171;}_LL171: _LL185: _tmp125=_tmp116.f1;if((
unsigned int)_tmp125 > 16?*((int*)_tmp125)== 1: 0){_LL186: _tmp126=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp125)->f1;goto _LL183;}else{goto _LL173;}_LL183: _tmp123=_tmp116.f2;if((
unsigned int)_tmp123 > 16?*((int*)_tmp123)== 1: 0){_LL184: _tmp124=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp123)->f1;goto _LL172;}else{goto _LL173;}_LL173: _LL189: _tmp129=_tmp116.f1;if((
unsigned int)_tmp129 > 16?*((int*)_tmp129)== 2: 0){_LL18A: _tmp12A=((struct Cyc_Absyn_Section_att_struct*)
_tmp129)->f1;goto _LL187;}else{goto _LL175;}_LL187: _tmp127=_tmp116.f2;if((
unsigned int)_tmp127 > 16?*((int*)_tmp127)== 2: 0){_LL188: _tmp128=((struct Cyc_Absyn_Section_att_struct*)
_tmp127)->f1;goto _LL174;}else{goto _LL175;}_LL175: goto _LL176;_LL16E: return(
_tmp11E == _tmp11A? _tmp11D == _tmp119: 0)? _tmp11C == _tmp118: 0;_LL170: _tmp126=
_tmp122;_tmp124=_tmp120;goto _LL172;_LL172: return _tmp126 == _tmp124;_LL174: return
Cyc_Std_strcmp(_tmp12A,_tmp128)== 0;_LL176: return 0;_LL16C:;}}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(! Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2)){
return 0;}}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(! Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a->hd,a1)){return 0;}}}return 1;}static void*Cyc_Tcutil_normalize_effect(
void*e);static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Structfield*sf){return Cyc_Tcutil_rgns_of((void*)sf->type);}
static struct _tuple5*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;{
void*_tmp12B=Cyc_Tcutil_tvar_kind(tv);_LL18C: if((int)_tmp12B == 3){goto _LL18D;}
else{goto _LL18E;}_LL18E: if((int)_tmp12B == 4){goto _LL18F;}else{goto _LL190;}_LL190:
goto _LL191;_LL18D: t=(void*)2;goto _LL18B;_LL18F: t=Cyc_Absyn_empty_effect;goto
_LL18B;_LL191: t=Cyc_Absyn_sint_t;goto _LL18B;_LL18B:;}return({struct _tuple5*
_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->f1=tv;_tmp12C->f2=t;_tmp12C;});}
static void*Cyc_Tcutil_rgns_of(void*t){void*_tmp12D=Cyc_Tcutil_compress(t);void*
_tmp12E;struct Cyc_Absyn_TunionInfo _tmp12F;void*_tmp130;struct Cyc_List_List*
_tmp131;struct Cyc_Absyn_PtrInfo _tmp132;void*_tmp133;void*_tmp134;void*_tmp135;
struct Cyc_List_List*_tmp136;struct Cyc_Absyn_TunionFieldInfo _tmp137;struct Cyc_List_List*
_tmp138;struct Cyc_List_List*_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_List_List*
_tmp13B;struct Cyc_List_List*_tmp13C;void*_tmp13D;struct Cyc_Absyn_FnInfo _tmp13E;
struct Cyc_List_List*_tmp13F;struct Cyc_Absyn_VarargInfo*_tmp140;struct Cyc_List_List*
_tmp141;void*_tmp142;struct Cyc_Core_Opt*_tmp143;struct Cyc_List_List*_tmp144;void*
_tmp145;_LL193: if((int)_tmp12D == 0){goto _LL194;}else{goto _LL195;}_LL195: if((int)
_tmp12D == 1){goto _LL196;}else{goto _LL197;}_LL197: if((unsigned int)_tmp12D > 3?*((
int*)_tmp12D)== 6: 0){goto _LL198;}else{goto _LL199;}_LL199: if((unsigned int)
_tmp12D > 3?*((int*)_tmp12D)== 12: 0){goto _LL19A;}else{goto _LL19B;}_LL19B: if((
unsigned int)_tmp12D > 3?*((int*)_tmp12D)== 16: 0){goto _LL19C;}else{goto _LL19D;}
_LL19D: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)== 5: 0){goto _LL19E;}else{goto
_LL19F;}_LL19F: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)== 0: 0){goto _LL1A0;}
else{goto _LL1A1;}_LL1A1: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)== 1: 0){goto
_LL1A2;}else{goto _LL1A3;}_LL1A3: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)== 
17: 0){_LL1C5: _tmp12E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp12D)->f1;
goto _LL1A4;}else{goto _LL1A5;}_LL1A5: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)
== 2: 0){_LL1C6: _tmp12F=((struct Cyc_Absyn_TunionType_struct*)_tmp12D)->f1;_LL1C8:
_tmp131=_tmp12F.targs;goto _LL1C7;_LL1C7: _tmp130=(void*)_tmp12F.rgn;goto _LL1A6;}
else{goto _LL1A7;}_LL1A7: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)== 4: 0){
_LL1C9: _tmp132=((struct Cyc_Absyn_PointerType_struct*)_tmp12D)->f1;_LL1CB: _tmp134=(
void*)_tmp132.elt_typ;goto _LL1CA;_LL1CA: _tmp133=(void*)_tmp132.rgn_typ;goto
_LL1A8;}else{goto _LL1A9;}_LL1A9: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)== 7:
0){_LL1CC: _tmp135=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp12D)->f1;goto
_LL1AA;}else{goto _LL1AB;}_LL1AB: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)== 9:
0){_LL1CD: _tmp136=((struct Cyc_Absyn_TupleType_struct*)_tmp12D)->f1;goto _LL1AC;}
else{goto _LL1AD;}_LL1AD: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)== 3: 0){
_LL1CE: _tmp137=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp12D)->f1;_LL1CF:
_tmp138=_tmp137.targs;goto _LL1AE;}else{goto _LL1AF;}_LL1AF: if((unsigned int)
_tmp12D > 3?*((int*)_tmp12D)== 10: 0){_LL1D0: _tmp139=((struct Cyc_Absyn_StructType_struct*)
_tmp12D)->f2;goto _LL1B0;}else{goto _LL1B1;}_LL1B1: if((unsigned int)_tmp12D > 3?*((
int*)_tmp12D)== 11: 0){_LL1D1: _tmp13A=((struct Cyc_Absyn_UnionType_struct*)_tmp12D)->f2;
goto _LL1B2;}else{goto _LL1B3;}_LL1B3: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)
== 14: 0){_LL1D2: _tmp13B=((struct Cyc_Absyn_AnonStructType_struct*)_tmp12D)->f1;
goto _LL1B4;}else{goto _LL1B5;}_LL1B5: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)
== 15: 0){_LL1D3: _tmp13C=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp12D)->f1;
goto _LL1B6;}else{goto _LL1B7;}_LL1B7: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)
== 13: 0){_LL1D4: _tmp13D=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp12D)->f1;
goto _LL1B8;}else{goto _LL1B9;}_LL1B9: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)
== 8: 0){_LL1D5: _tmp13E=((struct Cyc_Absyn_FnType_struct*)_tmp12D)->f1;_LL1DB:
_tmp144=_tmp13E.tvars;goto _LL1DA;_LL1DA: _tmp143=_tmp13E.effect;goto _LL1D9;_LL1D9:
_tmp142=(void*)_tmp13E.ret_typ;goto _LL1D8;_LL1D8: _tmp141=_tmp13E.args;goto _LL1D7;
_LL1D7: _tmp140=_tmp13E.cyc_varargs;goto _LL1D6;_LL1D6: _tmp13F=_tmp13E.rgn_po;goto
_LL1BA;}else{goto _LL1BB;}_LL1BB: if((int)_tmp12D == 2){goto _LL1BC;}else{goto _LL1BD;}
_LL1BD: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)== 19: 0){goto _LL1BE;}else{
goto _LL1BF;}_LL1BF: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)== 20: 0){goto
_LL1C0;}else{goto _LL1C1;}_LL1C1: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)== 
21: 0){_LL1DC: _tmp145=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp12D)->f1;goto
_LL1C2;}else{goto _LL1C3;}_LL1C3: if((unsigned int)_tmp12D > 3?*((int*)_tmp12D)== 
18: 0){goto _LL1C4;}else{goto _LL192;}_LL194: goto _LL196;_LL196: goto _LL198;_LL198:
goto _LL19A;_LL19A: goto _LL19C;_LL19C: goto _LL19E;_LL19E: return Cyc_Absyn_empty_effect;
_LL1A0: goto _LL1A2;_LL1A2: {void*_tmp146=Cyc_Tcutil_typ_kind(t);_LL1DE: if((int)
_tmp146 == 3){goto _LL1DF;}else{goto _LL1E0;}_LL1E0: if((int)_tmp146 == 4){goto _LL1E1;}
else{goto _LL1E2;}_LL1E2: goto _LL1E3;_LL1DF: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp148;_tmp148.tag=19;_tmp148.f1=(void*)t;_tmp148;});_tmp147;});_LL1E1: return t;
_LL1E3: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp149=_cycalloc(sizeof(*
_tmp149));_tmp149[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp14A;_tmp14A.tag=21;
_tmp14A.f1=(void*)t;_tmp14A;});_tmp149;});_LL1DD:;}_LL1A4: return(void*)({struct
Cyc_Absyn_AccessEff_struct*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp14C;_tmp14C.tag=19;_tmp14C.f1=(void*)_tmp12E;
_tmp14C;});_tmp14B;});_LL1A6: {struct Cyc_List_List*ts=({struct Cyc_List_List*
_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp151;_tmp151.tag=19;_tmp151.f1=(void*)_tmp130;_tmp151;});_tmp150;}));_tmp14F->tl=
Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp131);_tmp14F;});return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp14D=_cycalloc(sizeof(*_tmp14D));
_tmp14D[0]=({struct Cyc_Absyn_JoinEff_struct _tmp14E;_tmp14E.tag=20;_tmp14E.f1=ts;
_tmp14E;});_tmp14D;}));}_LL1A8: return Cyc_Tcutil_normalize_effect((void*)({struct
Cyc_Absyn_JoinEff_struct*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152[0]=({struct
Cyc_Absyn_JoinEff_struct _tmp153;_tmp153.tag=20;_tmp153.f1=({void*_tmp154[2];
_tmp154[1]=Cyc_Tcutil_rgns_of(_tmp134);_tmp154[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp156;_tmp156.tag=19;_tmp156.f1=(void*)_tmp133;_tmp156;});_tmp155;});Cyc_List_list(
_tag_arr(_tmp154,sizeof(void*),2));});_tmp153;});_tmp152;}));_LL1AA: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp135));_LL1AC: {struct Cyc_List_List*_tmp157=0;for(0;
_tmp136 != 0;_tmp136=_tmp136->tl){_tmp157=({struct Cyc_List_List*_tmp158=_cycalloc(
sizeof(*_tmp158));_tmp158->hd=(void*)(*((struct _tuple4*)_tmp136->hd)).f2;_tmp158->tl=
_tmp157;_tmp158;});}_tmp138=_tmp157;goto _LL1AE;}_LL1AE: _tmp139=_tmp138;goto
_LL1B0;_LL1B0: _tmp13A=_tmp139;goto _LL1B2;_LL1B2: return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp159=_cycalloc(sizeof(*_tmp159));
_tmp159[0]=({struct Cyc_Absyn_JoinEff_struct _tmp15A;_tmp15A.tag=20;_tmp15A.f1=Cyc_List_map(
Cyc_Tcutil_rgns_of,_tmp13A);_tmp15A;});_tmp159;}));_LL1B4: _tmp13C=_tmp13B;goto
_LL1B6;_LL1B6: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp15C;_tmp15C.tag=20;_tmp15C.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Structfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp13C);_tmp15C;});
_tmp15B;}));_LL1B8: return Cyc_Tcutil_rgns_of(_tmp13D);_LL1BA: {void*_tmp15D=Cyc_Tcutil_substitute(((
struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp144),(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp143))->v);return Cyc_Tcutil_normalize_effect(_tmp15D);}_LL1BC:
return Cyc_Absyn_empty_effect;_LL1BE: goto _LL1C0;_LL1C0: return t;_LL1C2: return Cyc_Tcutil_rgns_of(
_tmp145);_LL1C4: return({void*_tmp15E[0]={};Cyc_Tcutil_impos(_tag_arr("typedef in compressed type",
sizeof(unsigned char),27),_tag_arr(_tmp15E,sizeof(void*),0));});_LL192:;}static
void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{void*_tmp15F=
e;struct Cyc_List_List*_tmp160;struct Cyc_List_List**_tmp161;void*_tmp162;_LL1E5:
if((unsigned int)_tmp15F > 3?*((int*)_tmp15F)== 20: 0){_LL1EB: _tmp160=((struct Cyc_Absyn_JoinEff_struct*)
_tmp15F)->f1;_tmp161=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_struct*)
_tmp15F)->f1;goto _LL1E6;}else{goto _LL1E7;}_LL1E7: if((unsigned int)_tmp15F > 3?*((
int*)_tmp15F)== 21: 0){_LL1EC: _tmp162=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp15F)->f1;goto _LL1E8;}else{goto _LL1E9;}_LL1E9: goto _LL1EA;_LL1E6: {int
nested_join=1;{struct Cyc_List_List*effs=*_tmp161;for(0;effs != 0;effs=effs->tl){
void*_tmp163=(void*)effs->hd;(void*)(effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
_tmp163)));{void*_tmp164=(void*)effs->hd;_LL1EE: if((unsigned int)_tmp164 > 3?*((
int*)_tmp164)== 20: 0){goto _LL1EF;}else{goto _LL1F0;}_LL1F0: goto _LL1F1;_LL1EF:
nested_join=1;goto _LL1ED;_LL1F1: goto _LL1ED;_LL1ED:;}}}if(! nested_join){return e;}{
struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*_tmp161;for(0;effs != 0;
effs=effs->tl){void*_tmp165=Cyc_Tcutil_compress((void*)effs->hd);struct Cyc_List_List*
_tmp166;void*_tmp167;_LL1F3: if((unsigned int)_tmp165 > 3?*((int*)_tmp165)== 20: 0){
_LL1F9: _tmp166=((struct Cyc_Absyn_JoinEff_struct*)_tmp165)->f1;goto _LL1F4;}else{
goto _LL1F5;}_LL1F5: if((unsigned int)_tmp165 > 3?*((int*)_tmp165)== 19: 0){_LL1FA:
_tmp167=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp165)->f1;if((int)_tmp167
== 2){goto _LL1F6;}else{goto _LL1F7;}}else{goto _LL1F7;}_LL1F7: goto _LL1F8;_LL1F4:
effects=Cyc_List_revappend(_tmp166,effects);goto _LL1F2;_LL1F6: goto _LL1F2;_LL1F8:
effects=({struct Cyc_List_List*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168->hd=(
void*)_tmp165;_tmp168->tl=effects;_tmp168;});goto _LL1F2;_LL1F2:;}}*_tmp161=Cyc_List_imp_rev(
effects);return e;}}_LL1E8: return Cyc_Tcutil_rgns_of(_tmp162);_LL1EA: return e;
_LL1E4:;}}struct _tuple9{void*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static struct _tuple9*Cyc_Tcutil_get_effect_evar(void*t){void*_tmp169=Cyc_Tcutil_compress(
t);struct Cyc_List_List*_tmp16A;struct Cyc_List_List _tmp16B;struct Cyc_List_List*
_tmp16C;void*_tmp16D;struct Cyc_Core_Opt*_tmp16E;struct Cyc_Core_Opt*_tmp16F;
_LL1FC: if((unsigned int)_tmp169 > 3?*((int*)_tmp169)== 20: 0){_LL202: _tmp16A=((
struct Cyc_Absyn_JoinEff_struct*)_tmp169)->f1;if(_tmp16A == 0){goto _LL1FE;}else{
_tmp16B=*_tmp16A;_LL204: _tmp16D=(void*)_tmp16B.hd;goto _LL203;_LL203: _tmp16C=
_tmp16B.tl;goto _LL1FD;}}else{goto _LL1FE;}_LL1FE: if((unsigned int)_tmp169 > 3?*((
int*)_tmp169)== 0: 0){_LL206: _tmp16F=((struct Cyc_Absyn_Evar_struct*)_tmp169)->f1;
goto _LL205;_LL205: _tmp16E=((struct Cyc_Absyn_Evar_struct*)_tmp169)->f4;goto _LL1FF;}
else{goto _LL200;}_LL200: goto _LL201;_LL1FD: {void*_tmp170=Cyc_Tcutil_compress(
_tmp16D);struct Cyc_Core_Opt*_tmp171;_LL208: if((unsigned int)_tmp170 > 3?*((int*)
_tmp170)== 0: 0){_LL20C: _tmp171=((struct Cyc_Absyn_Evar_struct*)_tmp170)->f4;goto
_LL209;}else{goto _LL20A;}_LL20A: goto _LL20B;_LL209: return({struct _tuple9*_tmp172=
_cycalloc(sizeof(*_tmp172));_tmp172->f1=_tmp16D;_tmp172->f2=_tmp16C;_tmp172->f3=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp171))->v;_tmp172;});
_LL20B: return 0;_LL207:;}_LL1FF: if(_tmp16F == 0? 1:(void*)_tmp16F->v != (void*)4){({
void*_tmp173[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(unsigned char),27),_tag_arr(_tmp173,
sizeof(void*),0));});}return({struct _tuple9*_tmp174=_cycalloc(sizeof(*_tmp174));
_tmp174->f1=t;_tmp174->f2=0;_tmp174->f3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp16E))->v;_tmp174;});_LL201: return 0;_LL1FB:;}static struct Cyc_Core_Opt
Cyc_Tcutil_ek={(void*)((void*)4)};static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_struct*_tmp175=({struct Cyc_Absyn_FnType_struct*_tmp176=
_cycalloc(sizeof(*_tmp176));_tmp176[0]=({struct Cyc_Absyn_FnType_struct _tmp177;
_tmp177.tag=8;_tmp177.f1=({struct Cyc_Absyn_FnInfo _tmp178;_tmp178.tvars=0;_tmp178.effect=({
struct Cyc_Core_Opt*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->v=(void*)eff;
_tmp179;});_tmp178.ret_typ=(void*)((void*)0);_tmp178.args=0;_tmp178.c_varargs=0;
_tmp178.cyc_varargs=0;_tmp178.rgn_po=0;_tmp178.attributes=0;_tmp178;});_tmp177;});
_tmp176;});return Cyc_Absyn_atb_typ((void*)_tmp175,(void*)2,Cyc_Absyn_empty_tqual(),
Cyc_Absyn_bounds_one);}int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);if(r == (void*)2){return 1;}{void*_tmp17A=Cyc_Tcutil_compress(
e);void*_tmp17B;struct Cyc_List_List*_tmp17C;void*_tmp17D;struct Cyc_Core_Opt*
_tmp17E;struct Cyc_Core_Opt*_tmp17F;struct Cyc_Core_Opt**_tmp180;struct Cyc_Core_Opt*
_tmp181;_LL20E: if((unsigned int)_tmp17A > 3?*((int*)_tmp17A)== 19: 0){_LL218:
_tmp17B=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp17A)->f1;goto _LL20F;}
else{goto _LL210;}_LL210: if((unsigned int)_tmp17A > 3?*((int*)_tmp17A)== 20: 0){
_LL219: _tmp17C=((struct Cyc_Absyn_JoinEff_struct*)_tmp17A)->f1;goto _LL211;}else{
goto _LL212;}_LL212: if((unsigned int)_tmp17A > 3?*((int*)_tmp17A)== 21: 0){_LL21A:
_tmp17D=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp17A)->f1;goto _LL213;}else{
goto _LL214;}_LL214: if((unsigned int)_tmp17A > 3?*((int*)_tmp17A)== 0: 0){_LL21D:
_tmp181=((struct Cyc_Absyn_Evar_struct*)_tmp17A)->f1;goto _LL21C;_LL21C: _tmp17F=((
struct Cyc_Absyn_Evar_struct*)_tmp17A)->f2;_tmp180=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp17A)->f2;goto _LL21B;_LL21B: _tmp17E=((struct Cyc_Absyn_Evar_struct*)
_tmp17A)->f4;goto _LL215;}else{goto _LL216;}_LL216: goto _LL217;_LL20F: if(constrain){
return Cyc_Tcutil_unify(r,_tmp17B);}_tmp17B=Cyc_Tcutil_compress(_tmp17B);if(r == 
_tmp17B){return 1;}{struct _tuple8 _tmp183=({struct _tuple8 _tmp182;_tmp182.f1=r;
_tmp182.f2=_tmp17B;_tmp182;});void*_tmp184;struct Cyc_Absyn_Tvar*_tmp185;void*
_tmp186;struct Cyc_Absyn_Tvar*_tmp187;_LL21F: _LL225: _tmp186=_tmp183.f1;if((
unsigned int)_tmp186 > 3?*((int*)_tmp186)== 1: 0){_LL226: _tmp187=((struct Cyc_Absyn_VarType_struct*)
_tmp186)->f1;goto _LL223;}else{goto _LL221;}_LL223: _tmp184=_tmp183.f2;if((
unsigned int)_tmp184 > 3?*((int*)_tmp184)== 1: 0){_LL224: _tmp185=((struct Cyc_Absyn_VarType_struct*)
_tmp184)->f1;goto _LL220;}else{goto _LL221;}_LL221: goto _LL222;_LL220: return Cyc_Absyn_tvar_cmp(
_tmp187,_tmp185)== 0;_LL222: return 0;_LL21E:;}_LL211: for(0;_tmp17C != 0;_tmp17C=
_tmp17C->tl){if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp17C->hd)){
return 1;}}return 0;_LL213: {void*_tmp188=Cyc_Tcutil_rgns_of(_tmp17D);void*_tmp189;
_LL228: if((unsigned int)_tmp188 > 3?*((int*)_tmp188)== 21: 0){_LL22C: _tmp189=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp188)->f1;goto _LL229;}else{goto _LL22A;}_LL22A:
goto _LL22B;_LL229: if(! constrain){return 0;}{void*_tmp18A=Cyc_Tcutil_compress(
_tmp189);struct Cyc_Core_Opt*_tmp18B;struct Cyc_Core_Opt*_tmp18C;struct Cyc_Core_Opt**
_tmp18D;struct Cyc_Core_Opt*_tmp18E;_LL22E: if((unsigned int)_tmp18A > 3?*((int*)
_tmp18A)== 0: 0){_LL234: _tmp18E=((struct Cyc_Absyn_Evar_struct*)_tmp18A)->f1;goto
_LL233;_LL233: _tmp18C=((struct Cyc_Absyn_Evar_struct*)_tmp18A)->f2;_tmp18D=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp18A)->f2;goto _LL232;
_LL232: _tmp18B=((struct Cyc_Absyn_Evar_struct*)_tmp18A)->f4;goto _LL22F;}else{goto
_LL230;}_LL230: goto _LL231;_LL22F: {void*_tmp18F=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp18B);Cyc_Tcutil_occurs(_tmp18F,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp18B))->v,r);{void*_tmp190=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp192=_cycalloc(sizeof(*_tmp192));
_tmp192[0]=({struct Cyc_Absyn_JoinEff_struct _tmp193;_tmp193.tag=20;_tmp193.f1=({
void*_tmp194[2];_tmp194[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp195=
_cycalloc(sizeof(*_tmp195));_tmp195[0]=({struct Cyc_Absyn_AccessEff_struct _tmp196;
_tmp196.tag=19;_tmp196.f1=(void*)r;_tmp196;});_tmp195;});_tmp194[0]=_tmp18F;Cyc_List_list(
_tag_arr(_tmp194,sizeof(void*),2));});_tmp193;});_tmp192;}));*_tmp18D=({struct
Cyc_Core_Opt*_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191->v=(void*)_tmp190;
_tmp191;});return 1;}}_LL231: return 0;_LL22D:;}_LL22B: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp188);_LL227:;}_LL215: if(_tmp181 == 0? 1:(void*)_tmp181->v != (void*)
4){({void*_tmp197[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(unsigned char),27),_tag_arr(_tmp197,
sizeof(void*),0));});}if(! constrain){return 0;}{void*_tmp198=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp17E);Cyc_Tcutil_occurs(_tmp198,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp17E))->v,r);{struct
Cyc_Absyn_JoinEff_struct*_tmp199=({struct Cyc_Absyn_JoinEff_struct*_tmp19B=
_cycalloc(sizeof(*_tmp19B));_tmp19B[0]=({struct Cyc_Absyn_JoinEff_struct _tmp19C;
_tmp19C.tag=20;_tmp19C.f1=({struct Cyc_List_List*_tmp19D=_cycalloc(sizeof(*
_tmp19D));_tmp19D->hd=(void*)_tmp198;_tmp19D->tl=({struct Cyc_List_List*_tmp19E=
_cycalloc(sizeof(*_tmp19E));_tmp19E->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1A0;_tmp1A0.tag=19;_tmp1A0.f1=(void*)r;_tmp1A0;});_tmp19F;}));_tmp19E->tl=0;
_tmp19E;});_tmp19D;});_tmp19C;});_tmp19B;});*_tmp180=({struct Cyc_Core_Opt*
_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->v=(void*)((void*)_tmp199);_tmp19A;});
return 1;}}_LL217: return 0;_LL20D:;}}static int Cyc_Tcutil_type_in_effect(int
constrain,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp1A1=Cyc_Tcutil_compress(
e);struct Cyc_List_List*_tmp1A2;void*_tmp1A3;struct Cyc_Core_Opt*_tmp1A4;struct Cyc_Core_Opt*
_tmp1A5;struct Cyc_Core_Opt**_tmp1A6;struct Cyc_Core_Opt*_tmp1A7;_LL236: if((
unsigned int)_tmp1A1 > 3?*((int*)_tmp1A1)== 19: 0){goto _LL237;}else{goto _LL238;}
_LL238: if((unsigned int)_tmp1A1 > 3?*((int*)_tmp1A1)== 20: 0){_LL240: _tmp1A2=((
struct Cyc_Absyn_JoinEff_struct*)_tmp1A1)->f1;goto _LL239;}else{goto _LL23A;}_LL23A:
if((unsigned int)_tmp1A1 > 3?*((int*)_tmp1A1)== 21: 0){_LL241: _tmp1A3=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1A1)->f1;goto _LL23B;}else{goto _LL23C;}_LL23C:
if((unsigned int)_tmp1A1 > 3?*((int*)_tmp1A1)== 0: 0){_LL244: _tmp1A7=((struct Cyc_Absyn_Evar_struct*)
_tmp1A1)->f1;goto _LL243;_LL243: _tmp1A5=((struct Cyc_Absyn_Evar_struct*)_tmp1A1)->f2;
_tmp1A6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1A1)->f2;goto
_LL242;_LL242: _tmp1A4=((struct Cyc_Absyn_Evar_struct*)_tmp1A1)->f4;goto _LL23D;}
else{goto _LL23E;}_LL23E: goto _LL23F;_LL237: return 0;_LL239: for(0;_tmp1A2 != 0;
_tmp1A2=_tmp1A2->tl){if(Cyc_Tcutil_type_in_effect(constrain,t,(void*)_tmp1A2->hd)){
return 1;}}return 0;_LL23B: _tmp1A3=Cyc_Tcutil_compress(_tmp1A3);if(t == _tmp1A3){
return 1;}if(constrain){return Cyc_Tcutil_unify(t,_tmp1A3);}{void*_tmp1A8=Cyc_Tcutil_rgns_of(
t);void*_tmp1A9;_LL246: if((unsigned int)_tmp1A8 > 3?*((int*)_tmp1A8)== 21: 0){
_LL24A: _tmp1A9=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1A8)->f1;goto _LL247;}
else{goto _LL248;}_LL248: goto _LL249;_LL247: {struct _tuple8 _tmp1AB=({struct _tuple8
_tmp1AA;_tmp1AA.f1=t;_tmp1AA.f2=Cyc_Tcutil_compress(_tmp1A9);_tmp1AA;});void*
_tmp1AC;struct Cyc_Absyn_Tvar*_tmp1AD;void*_tmp1AE;struct Cyc_Absyn_Tvar*_tmp1AF;
_LL24C: _LL252: _tmp1AE=_tmp1AB.f1;if((unsigned int)_tmp1AE > 3?*((int*)_tmp1AE)== 
1: 0){_LL253: _tmp1AF=((struct Cyc_Absyn_VarType_struct*)_tmp1AE)->f1;goto _LL250;}
else{goto _LL24E;}_LL250: _tmp1AC=_tmp1AB.f2;if((unsigned int)_tmp1AC > 3?*((int*)
_tmp1AC)== 1: 0){_LL251: _tmp1AD=((struct Cyc_Absyn_VarType_struct*)_tmp1AC)->f1;
goto _LL24D;}else{goto _LL24E;}_LL24E: goto _LL24F;_LL24D: return Cyc_Tcutil_unify(t,
_tmp1A9);_LL24F: return t == _tmp1A9;_LL24B:;}_LL249: return Cyc_Tcutil_type_in_effect(
constrain,t,_tmp1A8);_LL245:;}_LL23D: if(_tmp1A7 == 0? 1:(void*)_tmp1A7->v != (void*)
4){({void*_tmp1B0[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(unsigned char),27),_tag_arr(_tmp1B0,
sizeof(void*),0));});}if(! constrain){return 0;}{void*_tmp1B1=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1A4);Cyc_Tcutil_occurs(_tmp1B1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1A4))->v,t);{struct
Cyc_Absyn_JoinEff_struct*_tmp1B2=({struct Cyc_Absyn_JoinEff_struct*_tmp1B4=
_cycalloc(sizeof(*_tmp1B4));_tmp1B4[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1B5;
_tmp1B5.tag=20;_tmp1B5.f1=({struct Cyc_List_List*_tmp1B6=_cycalloc(sizeof(*
_tmp1B6));_tmp1B6->hd=(void*)_tmp1B1;_tmp1B6->tl=({struct Cyc_List_List*_tmp1B7=
_cycalloc(sizeof(*_tmp1B7));_tmp1B7->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1B9;_tmp1B9.tag=21;_tmp1B9.f1=(void*)t;_tmp1B9;});_tmp1B8;}));_tmp1B7->tl=0;
_tmp1B7;});_tmp1B6;});_tmp1B5;});_tmp1B4;});*_tmp1A6=({struct Cyc_Core_Opt*
_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3->v=(void*)((void*)_tmp1B2);_tmp1B3;});
return 1;}}_LL23F: return 0;_LL235:;}}static int Cyc_Tcutil_variable_in_effect(int
constrain,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp1BA=
e;struct Cyc_Absyn_Tvar*_tmp1BB;struct Cyc_List_List*_tmp1BC;void*_tmp1BD;struct
Cyc_Core_Opt*_tmp1BE;struct Cyc_Core_Opt*_tmp1BF;struct Cyc_Core_Opt**_tmp1C0;
struct Cyc_Core_Opt*_tmp1C1;_LL255: if((unsigned int)_tmp1BA > 3?*((int*)_tmp1BA)== 
1: 0){_LL25F: _tmp1BB=((struct Cyc_Absyn_VarType_struct*)_tmp1BA)->f1;goto _LL256;}
else{goto _LL257;}_LL257: if((unsigned int)_tmp1BA > 3?*((int*)_tmp1BA)== 20: 0){
_LL260: _tmp1BC=((struct Cyc_Absyn_JoinEff_struct*)_tmp1BA)->f1;goto _LL258;}else{
goto _LL259;}_LL259: if((unsigned int)_tmp1BA > 3?*((int*)_tmp1BA)== 21: 0){_LL261:
_tmp1BD=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1BA)->f1;goto _LL25A;}else{
goto _LL25B;}_LL25B: if((unsigned int)_tmp1BA > 3?*((int*)_tmp1BA)== 0: 0){_LL264:
_tmp1C1=((struct Cyc_Absyn_Evar_struct*)_tmp1BA)->f1;goto _LL263;_LL263: _tmp1BF=((
struct Cyc_Absyn_Evar_struct*)_tmp1BA)->f2;_tmp1C0=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1BA)->f2;goto _LL262;_LL262: _tmp1BE=((struct Cyc_Absyn_Evar_struct*)
_tmp1BA)->f4;goto _LL25C;}else{goto _LL25D;}_LL25D: goto _LL25E;_LL256: return Cyc_Absyn_tvar_cmp(
v,_tmp1BB)== 0;_LL258: for(0;_tmp1BC != 0;_tmp1BC=_tmp1BC->tl){if(Cyc_Tcutil_variable_in_effect(
constrain,v,(void*)_tmp1BC->hd)){return 1;}}return 0;_LL25A: {void*_tmp1C2=Cyc_Tcutil_rgns_of(
_tmp1BD);void*_tmp1C3;_LL266: if((unsigned int)_tmp1C2 > 3?*((int*)_tmp1C2)== 21: 0){
_LL26A: _tmp1C3=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1C2)->f1;goto _LL267;}
else{goto _LL268;}_LL268: goto _LL269;_LL267: if(! constrain){return 0;}{void*_tmp1C4=
Cyc_Tcutil_compress(_tmp1C3);struct Cyc_Core_Opt*_tmp1C5;struct Cyc_Core_Opt*
_tmp1C6;struct Cyc_Core_Opt**_tmp1C7;struct Cyc_Core_Opt*_tmp1C8;_LL26C: if((
unsigned int)_tmp1C4 > 3?*((int*)_tmp1C4)== 0: 0){_LL272: _tmp1C8=((struct Cyc_Absyn_Evar_struct*)
_tmp1C4)->f1;goto _LL271;_LL271: _tmp1C6=((struct Cyc_Absyn_Evar_struct*)_tmp1C4)->f2;
_tmp1C7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1C4)->f2;goto
_LL270;_LL270: _tmp1C5=((struct Cyc_Absyn_Evar_struct*)_tmp1C4)->f4;goto _LL26D;}
else{goto _LL26E;}_LL26E: goto _LL26F;_LL26D: {void*_tmp1C9=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1C5);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1C5))->v,v)){return 0;}{void*_tmp1CA=Cyc_Tcutil_dummy_fntype((void*)({struct
Cyc_Absyn_JoinEff_struct*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC[0]=({struct
Cyc_Absyn_JoinEff_struct _tmp1CD;_tmp1CD.tag=20;_tmp1CD.f1=({void*_tmp1CE[2];
_tmp1CE[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp1CF=_cycalloc(sizeof(*
_tmp1CF));_tmp1CF[0]=({struct Cyc_Absyn_VarType_struct _tmp1D0;_tmp1D0.tag=1;
_tmp1D0.f1=v;_tmp1D0;});_tmp1CF;});_tmp1CE[0]=_tmp1C9;Cyc_List_list(_tag_arr(
_tmp1CE,sizeof(void*),2));});_tmp1CD;});_tmp1CC;}));*_tmp1C7=({struct Cyc_Core_Opt*
_tmp1CB=_cycalloc(sizeof(*_tmp1CB));_tmp1CB->v=(void*)_tmp1CA;_tmp1CB;});return 1;}}
_LL26F: return 0;_LL26B:;}_LL269: return Cyc_Tcutil_variable_in_effect(constrain,v,
_tmp1C2);_LL265:;}_LL25C: if(_tmp1C1 == 0? 1:(void*)_tmp1C1->v != (void*)4){({void*
_tmp1D1[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(unsigned char),27),_tag_arr(_tmp1D1,
sizeof(void*),0));});}{void*_tmp1D2=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1BE);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1BE))->v,v)){return 0;}{
struct Cyc_Absyn_JoinEff_struct*_tmp1D3=({struct Cyc_Absyn_JoinEff_struct*_tmp1D5=
_cycalloc(sizeof(*_tmp1D5));_tmp1D5[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1D6;
_tmp1D6.tag=20;_tmp1D6.f1=({struct Cyc_List_List*_tmp1D7=_cycalloc(sizeof(*
_tmp1D7));_tmp1D7->hd=(void*)_tmp1D2;_tmp1D7->tl=({struct Cyc_List_List*_tmp1D8=
_cycalloc(sizeof(*_tmp1D8));_tmp1D8->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9[0]=({struct Cyc_Absyn_VarType_struct
_tmp1DA;_tmp1DA.tag=1;_tmp1DA.f1=v;_tmp1DA;});_tmp1D9;}));_tmp1D8->tl=0;_tmp1D8;});
_tmp1D7;});_tmp1D6;});_tmp1D5;});*_tmp1C0=({struct Cyc_Core_Opt*_tmp1D4=_cycalloc(
sizeof(*_tmp1D4));_tmp1D4->v=(void*)((void*)_tmp1D3);_tmp1D4;});return 1;}}_LL25E:
return 0;_LL254:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp1DB=e;struct Cyc_List_List*_tmp1DC;void*_tmp1DD;_LL274: if((
unsigned int)_tmp1DB > 3?*((int*)_tmp1DB)== 20: 0){_LL27C: _tmp1DC=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1DB)->f1;goto _LL275;}else{goto _LL276;}_LL276: if((unsigned int)_tmp1DB > 3?*((
int*)_tmp1DB)== 21: 0){_LL27D: _tmp1DD=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1DB)->f1;goto _LL277;}else{goto _LL278;}_LL278: if((unsigned int)_tmp1DB > 3?*((
int*)_tmp1DB)== 0: 0){goto _LL279;}else{goto _LL27A;}_LL27A: goto _LL27B;_LL275: for(0;
_tmp1DC != 0;_tmp1DC=_tmp1DC->tl){if(Cyc_Tcutil_evar_in_effect(evar,(void*)
_tmp1DC->hd)){return 1;}}return 0;_LL277: {void*_tmp1DE=Cyc_Tcutil_rgns_of(_tmp1DD);
void*_tmp1DF;_LL27F: if((unsigned int)_tmp1DE > 3?*((int*)_tmp1DE)== 21: 0){_LL283:
_tmp1DF=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1DE)->f1;goto _LL280;}else{
goto _LL281;}_LL281: goto _LL282;_LL280: return 0;_LL282: return Cyc_Tcutil_evar_in_effect(
evar,_tmp1DE);_LL27E:;}_LL279: return evar == e;_LL27B: return 0;_LL273:;}}int Cyc_Tcutil_subset_effect(
int set_to_empty,void*e1,void*e2){void*_tmp1E0=Cyc_Tcutil_compress(e1);struct Cyc_List_List*
_tmp1E1;void*_tmp1E2;struct Cyc_Absyn_Tvar*_tmp1E3;void*_tmp1E4;struct Cyc_Core_Opt*
_tmp1E5;struct Cyc_Core_Opt*_tmp1E6;struct Cyc_Core_Opt**_tmp1E7;_LL285: if((
unsigned int)_tmp1E0 > 3?*((int*)_tmp1E0)== 20: 0){_LL291: _tmp1E1=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1E0)->f1;goto _LL286;}else{goto _LL287;}_LL287: if((unsigned int)_tmp1E0 > 3?*((
int*)_tmp1E0)== 19: 0){_LL292: _tmp1E2=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1E0)->f1;goto _LL288;}else{goto _LL289;}_LL289: if((unsigned int)_tmp1E0 > 3?*((
int*)_tmp1E0)== 1: 0){_LL293: _tmp1E3=((struct Cyc_Absyn_VarType_struct*)_tmp1E0)->f1;
goto _LL28A;}else{goto _LL28B;}_LL28B: if((unsigned int)_tmp1E0 > 3?*((int*)_tmp1E0)
== 21: 0){_LL294: _tmp1E4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1E0)->f1;
goto _LL28C;}else{goto _LL28D;}_LL28D: if((unsigned int)_tmp1E0 > 3?*((int*)_tmp1E0)
== 0: 0){_LL296: _tmp1E6=((struct Cyc_Absyn_Evar_struct*)_tmp1E0)->f2;_tmp1E7=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1E0)->f2;goto _LL295;
_LL295: _tmp1E5=((struct Cyc_Absyn_Evar_struct*)_tmp1E0)->f4;goto _LL28E;}else{goto
_LL28F;}_LL28F: goto _LL290;_LL286: for(0;_tmp1E1 != 0;_tmp1E1=_tmp1E1->tl){if(! Cyc_Tcutil_subset_effect(
set_to_empty,(void*)_tmp1E1->hd,e2)){return 0;}}return 1;_LL288: return(Cyc_Tcutil_region_in_effect(
0,_tmp1E2,e2)? 1: Cyc_Tcutil_region_in_effect(1,_tmp1E2,e2))? 1: Cyc_Tcutil_unify(
_tmp1E2,(void*)2);_LL28A: return Cyc_Tcutil_variable_in_effect(0,_tmp1E3,e2)? 1: Cyc_Tcutil_variable_in_effect(
1,_tmp1E3,e2);_LL28C: {void*_tmp1E8=Cyc_Tcutil_rgns_of(_tmp1E4);void*_tmp1E9;
_LL298: if((unsigned int)_tmp1E8 > 3?*((int*)_tmp1E8)== 21: 0){_LL29C: _tmp1E9=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1E8)->f1;goto _LL299;}else{goto _LL29A;}_LL29A:
goto _LL29B;_LL299: return(Cyc_Tcutil_type_in_effect(0,_tmp1E9,e2)? 1: Cyc_Tcutil_type_in_effect(
1,_tmp1E9,e2))? 1: Cyc_Tcutil_unify(_tmp1E9,Cyc_Absyn_sint_t);_LL29B: return Cyc_Tcutil_subset_effect(
set_to_empty,_tmp1E8,e2);_LL297:;}_LL28E: if(! Cyc_Tcutil_evar_in_effect(e1,e2)){
if(set_to_empty){*_tmp1E7=({struct Cyc_Core_Opt*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));
_tmp1EA->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1EB=_cycalloc(
sizeof(*_tmp1EB));_tmp1EB[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1EC;_tmp1EC.tag=
20;_tmp1EC.f1=0;_tmp1EC;});_tmp1EB;}));_tmp1EA;});}else{Cyc_Tcutil_occurs(e1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1E5))->v,e2);*_tmp1E7=({
struct Cyc_Core_Opt*_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED->v=(void*)e2;
_tmp1ED;});}}return 1;_LL290: return({struct Cyc_Std_String_pa_struct _tmp1EF;
_tmp1EF.tag=0;_tmp1EF.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e1);{void*
_tmp1EE[1]={& _tmp1EF};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("subset_effect: bad effect: %s",sizeof(unsigned char),30),_tag_arr(
_tmp1EE,sizeof(void*),1));}});_LL284:;}struct _tuple10{struct _tuple9*f1;struct
_tuple9*f2;};static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);{struct _tuple10 _tmp1F1=({struct _tuple10
_tmp1F0;_tmp1F0.f1=Cyc_Tcutil_get_effect_evar(e1);_tmp1F0.f2=Cyc_Tcutil_get_effect_evar(
e2);_tmp1F0;});_LL29E: goto _LL29F;_LL29F: return Cyc_Tcutil_subset_effect(0,e1,e2)?
Cyc_Tcutil_subset_effect(0,e2,e1): 0;_LL29D:;}}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple8 _tmp1F3;void*_tmp1F4;void*_tmp1F5;struct
_tuple8*_tmp1F2=(struct _tuple8*)r1->hd;_tmp1F3=*_tmp1F2;_LL2A2: _tmp1F5=_tmp1F3.f1;
goto _LL2A1;_LL2A1: _tmp1F4=_tmp1F3.f2;goto _LL2A0;_LL2A0: {int found=_tmp1F5 == (
void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0? ! found: 0;r2=r2->tl){struct
_tuple8 _tmp1F7;void*_tmp1F8;void*_tmp1F9;struct _tuple8*_tmp1F6=(struct _tuple8*)
r2->hd;_tmp1F7=*_tmp1F6;_LL2A5: _tmp1F9=_tmp1F7.f1;goto _LL2A4;_LL2A4: _tmp1F8=
_tmp1F7.f2;goto _LL2A3;_LL2A3: if(Cyc_Tcutil_unify(_tmp1F5,_tmp1F9)? Cyc_Tcutil_unify(
_tmp1F4,_tmp1F8): 0){found=1;break;}}}if(! found){return 0;}}}}return 1;}static int
Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return
Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)? Cyc_Tcutil_sub_rgnpo(rpo2,rpo1): 0;}struct
_tuple11{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};struct
_tuple12{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _tagged_arr)_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2){return;}{void*_tmp1FA=t1;struct Cyc_Core_Opt*_tmp1FB;struct Cyc_Core_Opt*
_tmp1FC;struct Cyc_Core_Opt**_tmp1FD;struct Cyc_Core_Opt*_tmp1FE;_LL2A7: if((
unsigned int)_tmp1FA > 3?*((int*)_tmp1FA)== 0: 0){_LL2AD: _tmp1FE=((struct Cyc_Absyn_Evar_struct*)
_tmp1FA)->f1;goto _LL2AC;_LL2AC: _tmp1FC=((struct Cyc_Absyn_Evar_struct*)_tmp1FA)->f2;
_tmp1FD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1FA)->f2;goto
_LL2AB;_LL2AB: _tmp1FB=((struct Cyc_Absyn_Evar_struct*)_tmp1FA)->f4;goto _LL2A8;}
else{goto _LL2A9;}_LL2A9: goto _LL2AA;_LL2A8: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1FB))->v,t2);{void*
_tmp1FF=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp1FF,(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp1FE))->v)){*_tmp1FD=({struct Cyc_Core_Opt*_tmp200=
_cycalloc(sizeof(*_tmp200));_tmp200->v=(void*)t2;_tmp200;});return;}else{{void*
_tmp201=t2;struct Cyc_Core_Opt*_tmp202;struct Cyc_Core_Opt*_tmp203;struct Cyc_Core_Opt**
_tmp204;struct Cyc_Absyn_PtrInfo _tmp205;_LL2AF: if((unsigned int)_tmp201 > 3?*((int*)
_tmp201)== 0: 0){_LL2B6: _tmp203=((struct Cyc_Absyn_Evar_struct*)_tmp201)->f2;
_tmp204=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp201)->f2;goto
_LL2B5;_LL2B5: _tmp202=((struct Cyc_Absyn_Evar_struct*)_tmp201)->f4;goto _LL2B0;}
else{goto _LL2B1;}_LL2B1: if((unsigned int)_tmp201 > 3?*((int*)_tmp201)== 4: 0){
_LL2B8: _tmp205=((struct Cyc_Absyn_PointerType_struct*)_tmp201)->f1;goto _LL2B7;}
else{goto _LL2B3;}_LL2B7: if((void*)_tmp1FE->v == (void*)2){goto _LL2B2;}else{goto
_LL2B3;}_LL2B3: goto _LL2B4;_LL2B0: {struct Cyc_List_List*_tmp206=(struct Cyc_List_List*)
_tmp1FB->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp202))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp206,(struct Cyc_Absyn_Tvar*)s2->hd)){
Cyc_Tcutil_failure_reason=_tag_arr("(type variable would escape scope)",sizeof(
unsigned char),35);(int)_throw((void*)Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((
void*)_tmp1FE->v,_tmp1FF)){*_tmp204=({struct Cyc_Core_Opt*_tmp207=_cycalloc(
sizeof(*_tmp207));_tmp207->v=(void*)t1;_tmp207;});return;}Cyc_Tcutil_failure_reason=
_tag_arr("(kinds are incompatible)",sizeof(unsigned char),25);goto _LL2AE;}_LL2B2: {
struct Cyc_Absyn_Conref*_tmp208=Cyc_Absyn_compress_conref(_tmp205.bounds);{void*
_tmp209=(void*)_tmp208->v;_LL2BA: if((int)_tmp209 == 0){goto _LL2BB;}else{goto
_LL2BC;}_LL2BC: goto _LL2BD;_LL2BB:(void*)(_tmp208->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp20B;_tmp20B.tag=0;_tmp20B.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp20D;_tmp20D.tag=0;_tmp20D.f1=Cyc_Absyn_signed_int_exp(1,0);_tmp20D;});
_tmp20C;}));_tmp20B;});_tmp20A;})));*_tmp1FD=({struct Cyc_Core_Opt*_tmp20E=
_cycalloc(sizeof(*_tmp20E));_tmp20E->v=(void*)t2;_tmp20E;});return;_LL2BD: goto
_LL2B9;_LL2B9:;}goto _LL2AE;}_LL2B4: goto _LL2AE;_LL2AE:;}Cyc_Tcutil_failure_reason=
_tag_arr("(kinds are incompatible)",sizeof(unsigned char),25);(int)_throw((void*)
Cyc_Tcutil_Unify);}}_LL2AA: goto _LL2A6;_LL2A6:;}{struct _tuple8 _tmp210=({struct
_tuple8 _tmp20F;_tmp20F.f1=t2;_tmp20F.f2=t1;_tmp20F;});void*_tmp211;void*_tmp212;
void*_tmp213;void*_tmp214;struct Cyc_Absyn_Tvar*_tmp215;void*_tmp216;struct Cyc_Absyn_Tvar*
_tmp217;void*_tmp218;struct Cyc_Absyn_Structdecl**_tmp219;struct Cyc_List_List*
_tmp21A;struct _tuple1*_tmp21B;void*_tmp21C;struct Cyc_Absyn_Structdecl**_tmp21D;
struct Cyc_List_List*_tmp21E;struct _tuple1*_tmp21F;void*_tmp220;struct _tuple1*
_tmp221;void*_tmp222;struct _tuple1*_tmp223;void*_tmp224;struct Cyc_List_List*
_tmp225;void*_tmp226;struct Cyc_List_List*_tmp227;void*_tmp228;struct Cyc_Absyn_Uniondecl**
_tmp229;struct Cyc_List_List*_tmp22A;struct _tuple1*_tmp22B;void*_tmp22C;struct Cyc_Absyn_Uniondecl**
_tmp22D;struct Cyc_List_List*_tmp22E;struct _tuple1*_tmp22F;void*_tmp230;struct Cyc_Absyn_TunionInfo
_tmp231;void*_tmp232;struct Cyc_List_List*_tmp233;void*_tmp234;struct Cyc_Absyn_Tuniondecl**
_tmp235;struct Cyc_Absyn_Tuniondecl*_tmp236;void*_tmp237;struct Cyc_Absyn_TunionInfo
_tmp238;void*_tmp239;struct Cyc_List_List*_tmp23A;void*_tmp23B;struct Cyc_Absyn_Tuniondecl**
_tmp23C;struct Cyc_Absyn_Tuniondecl*_tmp23D;void*_tmp23E;struct Cyc_Absyn_TunionFieldInfo
_tmp23F;struct Cyc_List_List*_tmp240;void*_tmp241;struct Cyc_Absyn_Tunionfield*
_tmp242;struct Cyc_Absyn_Tuniondecl*_tmp243;void*_tmp244;struct Cyc_Absyn_TunionFieldInfo
_tmp245;struct Cyc_List_List*_tmp246;void*_tmp247;struct Cyc_Absyn_Tunionfield*
_tmp248;struct Cyc_Absyn_Tuniondecl*_tmp249;void*_tmp24A;struct Cyc_Absyn_PtrInfo
_tmp24B;struct Cyc_Absyn_Conref*_tmp24C;struct Cyc_Absyn_Tqual _tmp24D;struct Cyc_Absyn_Conref*
_tmp24E;void*_tmp24F;void*_tmp250;void*_tmp251;struct Cyc_Absyn_PtrInfo _tmp252;
struct Cyc_Absyn_Conref*_tmp253;struct Cyc_Absyn_Tqual _tmp254;struct Cyc_Absyn_Conref*
_tmp255;void*_tmp256;void*_tmp257;void*_tmp258;void*_tmp259;void*_tmp25A;void*
_tmp25B;void*_tmp25C;void*_tmp25D;void*_tmp25E;void*_tmp25F;void*_tmp260;int
_tmp261;void*_tmp262;int _tmp263;void*_tmp264;void*_tmp265;void*_tmp266;void*
_tmp267;void*_tmp268;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Tqual _tmp26A;
void*_tmp26B;void*_tmp26C;struct Cyc_Absyn_Exp*_tmp26D;struct Cyc_Absyn_Tqual
_tmp26E;void*_tmp26F;void*_tmp270;struct Cyc_Absyn_FnInfo _tmp271;struct Cyc_List_List*
_tmp272;struct Cyc_List_List*_tmp273;struct Cyc_Absyn_VarargInfo*_tmp274;int
_tmp275;struct Cyc_List_List*_tmp276;void*_tmp277;struct Cyc_Core_Opt*_tmp278;
struct Cyc_List_List*_tmp279;void*_tmp27A;struct Cyc_Absyn_FnInfo _tmp27B;struct Cyc_List_List*
_tmp27C;struct Cyc_List_List*_tmp27D;struct Cyc_Absyn_VarargInfo*_tmp27E;int
_tmp27F;struct Cyc_List_List*_tmp280;void*_tmp281;struct Cyc_Core_Opt*_tmp282;
struct Cyc_List_List*_tmp283;void*_tmp284;struct Cyc_List_List*_tmp285;void*
_tmp286;struct Cyc_List_List*_tmp287;void*_tmp288;struct Cyc_List_List*_tmp289;
void*_tmp28A;struct Cyc_List_List*_tmp28B;void*_tmp28C;struct Cyc_List_List*
_tmp28D;void*_tmp28E;struct Cyc_List_List*_tmp28F;void*_tmp290;void*_tmp291;void*
_tmp292;void*_tmp293;void*_tmp294;void*_tmp295;void*_tmp296;void*_tmp297;void*
_tmp298;void*_tmp299;void*_tmp29A;void*_tmp29B;_LL2BF: _LL2F7: _tmp211=_tmp210.f1;
if((unsigned int)_tmp211 > 3?*((int*)_tmp211)== 0: 0){goto _LL2C0;}else{goto _LL2C1;}
_LL2C1: _LL2F9: _tmp213=_tmp210.f1;if((int)_tmp213 == 0){goto _LL2F8;}else{goto
_LL2C3;}_LL2F8: _tmp212=_tmp210.f2;if((int)_tmp212 == 0){goto _LL2C2;}else{goto
_LL2C3;}_LL2C3: _LL2FC: _tmp216=_tmp210.f1;if((unsigned int)_tmp216 > 3?*((int*)
_tmp216)== 1: 0){_LL2FD: _tmp217=((struct Cyc_Absyn_VarType_struct*)_tmp216)->f1;
goto _LL2FA;}else{goto _LL2C5;}_LL2FA: _tmp214=_tmp210.f2;if((unsigned int)_tmp214 > 
3?*((int*)_tmp214)== 1: 0){_LL2FB: _tmp215=((struct Cyc_Absyn_VarType_struct*)
_tmp214)->f1;goto _LL2C4;}else{goto _LL2C5;}_LL2C5: _LL302: _tmp21C=_tmp210.f1;if((
unsigned int)_tmp21C > 3?*((int*)_tmp21C)== 10: 0){_LL305: _tmp21F=((struct Cyc_Absyn_StructType_struct*)
_tmp21C)->f1;goto _LL304;_LL304: _tmp21E=((struct Cyc_Absyn_StructType_struct*)
_tmp21C)->f2;goto _LL303;_LL303: _tmp21D=((struct Cyc_Absyn_StructType_struct*)
_tmp21C)->f3;goto _LL2FE;}else{goto _LL2C7;}_LL2FE: _tmp218=_tmp210.f2;if((
unsigned int)_tmp218 > 3?*((int*)_tmp218)== 10: 0){_LL301: _tmp21B=((struct Cyc_Absyn_StructType_struct*)
_tmp218)->f1;goto _LL300;_LL300: _tmp21A=((struct Cyc_Absyn_StructType_struct*)
_tmp218)->f2;goto _LL2FF;_LL2FF: _tmp219=((struct Cyc_Absyn_StructType_struct*)
_tmp218)->f3;goto _LL2C6;}else{goto _LL2C7;}_LL2C7: _LL308: _tmp222=_tmp210.f1;if((
unsigned int)_tmp222 > 3?*((int*)_tmp222)== 12: 0){_LL309: _tmp223=((struct Cyc_Absyn_EnumType_struct*)
_tmp222)->f1;goto _LL306;}else{goto _LL2C9;}_LL306: _tmp220=_tmp210.f2;if((
unsigned int)_tmp220 > 3?*((int*)_tmp220)== 12: 0){_LL307: _tmp221=((struct Cyc_Absyn_EnumType_struct*)
_tmp220)->f1;goto _LL2C8;}else{goto _LL2C9;}_LL2C9: _LL30C: _tmp226=_tmp210.f1;if((
unsigned int)_tmp226 > 3?*((int*)_tmp226)== 16: 0){_LL30D: _tmp227=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp226)->f1;goto _LL30A;}else{goto _LL2CB;}_LL30A: _tmp224=_tmp210.f2;if((
unsigned int)_tmp224 > 3?*((int*)_tmp224)== 16: 0){_LL30B: _tmp225=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp224)->f1;goto _LL2CA;}else{goto _LL2CB;}_LL2CB: _LL312: _tmp22C=_tmp210.f1;if((
unsigned int)_tmp22C > 3?*((int*)_tmp22C)== 11: 0){_LL315: _tmp22F=((struct Cyc_Absyn_UnionType_struct*)
_tmp22C)->f1;goto _LL314;_LL314: _tmp22E=((struct Cyc_Absyn_UnionType_struct*)
_tmp22C)->f2;goto _LL313;_LL313: _tmp22D=((struct Cyc_Absyn_UnionType_struct*)
_tmp22C)->f3;goto _LL30E;}else{goto _LL2CD;}_LL30E: _tmp228=_tmp210.f2;if((
unsigned int)_tmp228 > 3?*((int*)_tmp228)== 11: 0){_LL311: _tmp22B=((struct Cyc_Absyn_UnionType_struct*)
_tmp228)->f1;goto _LL310;_LL310: _tmp22A=((struct Cyc_Absyn_UnionType_struct*)
_tmp228)->f2;goto _LL30F;_LL30F: _tmp229=((struct Cyc_Absyn_UnionType_struct*)
_tmp228)->f3;goto _LL2CC;}else{goto _LL2CD;}_LL2CD: _LL31C: _tmp237=_tmp210.f1;if((
unsigned int)_tmp237 > 3?*((int*)_tmp237)== 2: 0){_LL31D: _tmp238=((struct Cyc_Absyn_TunionType_struct*)
_tmp237)->f1;_LL320: _tmp23B=(void*)_tmp238.tunion_info;if(*((int*)_tmp23B)== 1){
_LL321: _tmp23C=((struct Cyc_Absyn_KnownTunion_struct*)_tmp23B)->f1;_tmp23D=*
_tmp23C;goto _LL31F;}else{goto _LL2CF;}_LL31F: _tmp23A=_tmp238.targs;goto _LL31E;
_LL31E: _tmp239=(void*)_tmp238.rgn;goto _LL316;}else{goto _LL2CF;}_LL316: _tmp230=
_tmp210.f2;if((unsigned int)_tmp230 > 3?*((int*)_tmp230)== 2: 0){_LL317: _tmp231=((
struct Cyc_Absyn_TunionType_struct*)_tmp230)->f1;_LL31A: _tmp234=(void*)_tmp231.tunion_info;
if(*((int*)_tmp234)== 1){_LL31B: _tmp235=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp234)->f1;_tmp236=*_tmp235;goto _LL319;}else{goto _LL2CF;}_LL319: _tmp233=
_tmp231.targs;goto _LL318;_LL318: _tmp232=(void*)_tmp231.rgn;goto _LL2CE;}else{goto
_LL2CF;}_LL2CF: _LL328: _tmp244=_tmp210.f1;if((unsigned int)_tmp244 > 3?*((int*)
_tmp244)== 3: 0){_LL329: _tmp245=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp244)->f1;
_LL32B: _tmp247=(void*)_tmp245.field_info;if(*((int*)_tmp247)== 1){_LL32D: _tmp249=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp247)->f1;goto _LL32C;_LL32C: _tmp248=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp247)->f2;goto _LL32A;}else{goto
_LL2D1;}_LL32A: _tmp246=_tmp245.targs;goto _LL322;}else{goto _LL2D1;}_LL322: _tmp23E=
_tmp210.f2;if((unsigned int)_tmp23E > 3?*((int*)_tmp23E)== 3: 0){_LL323: _tmp23F=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp23E)->f1;_LL325: _tmp241=(void*)
_tmp23F.field_info;if(*((int*)_tmp241)== 1){_LL327: _tmp243=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp241)->f1;goto _LL326;_LL326: _tmp242=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp241)->f2;goto _LL324;}else{goto _LL2D1;}_LL324: _tmp240=_tmp23F.targs;goto
_LL2D0;}else{goto _LL2D1;}_LL2D1: _LL335: _tmp251=_tmp210.f1;if((unsigned int)
_tmp251 > 3?*((int*)_tmp251)== 4: 0){_LL336: _tmp252=((struct Cyc_Absyn_PointerType_struct*)
_tmp251)->f1;_LL33B: _tmp257=(void*)_tmp252.elt_typ;goto _LL33A;_LL33A: _tmp256=(
void*)_tmp252.rgn_typ;goto _LL339;_LL339: _tmp255=_tmp252.nullable;goto _LL338;
_LL338: _tmp254=_tmp252.tq;goto _LL337;_LL337: _tmp253=_tmp252.bounds;goto _LL32E;}
else{goto _LL2D3;}_LL32E: _tmp24A=_tmp210.f2;if((unsigned int)_tmp24A > 3?*((int*)
_tmp24A)== 4: 0){_LL32F: _tmp24B=((struct Cyc_Absyn_PointerType_struct*)_tmp24A)->f1;
_LL334: _tmp250=(void*)_tmp24B.elt_typ;goto _LL333;_LL333: _tmp24F=(void*)_tmp24B.rgn_typ;
goto _LL332;_LL332: _tmp24E=_tmp24B.nullable;goto _LL331;_LL331: _tmp24D=_tmp24B.tq;
goto _LL330;_LL330: _tmp24C=_tmp24B.bounds;goto _LL2D2;}else{goto _LL2D3;}_LL2D3:
_LL33F: _tmp25B=_tmp210.f1;if((unsigned int)_tmp25B > 3?*((int*)_tmp25B)== 5: 0){
_LL341: _tmp25D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp25B)->f1;goto _LL340;
_LL340: _tmp25C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp25B)->f2;goto _LL33C;}
else{goto _LL2D5;}_LL33C: _tmp258=_tmp210.f2;if((unsigned int)_tmp258 > 3?*((int*)
_tmp258)== 5: 0){_LL33E: _tmp25A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp258)->f1;
goto _LL33D;_LL33D: _tmp259=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp258)->f2;
goto _LL2D4;}else{goto _LL2D5;}_LL2D5: _LL343: _tmp25F=_tmp210.f1;if((int)_tmp25F == 
1){goto _LL342;}else{goto _LL2D7;}_LL342: _tmp25E=_tmp210.f2;if((int)_tmp25E == 1){
goto _LL2D6;}else{goto _LL2D7;}_LL2D7: _LL346: _tmp262=_tmp210.f1;if((unsigned int)
_tmp262 > 3?*((int*)_tmp262)== 6: 0){_LL347: _tmp263=((struct Cyc_Absyn_DoubleType_struct*)
_tmp262)->f1;goto _LL344;}else{goto _LL2D9;}_LL344: _tmp260=_tmp210.f2;if((
unsigned int)_tmp260 > 3?*((int*)_tmp260)== 6: 0){_LL345: _tmp261=((struct Cyc_Absyn_DoubleType_struct*)
_tmp260)->f1;goto _LL2D8;}else{goto _LL2D9;}_LL2D9: _LL34A: _tmp266=_tmp210.f1;if((
unsigned int)_tmp266 > 3?*((int*)_tmp266)== 13: 0){_LL34B: _tmp267=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp266)->f1;goto _LL348;}else{goto _LL2DB;}_LL348:
_tmp264=_tmp210.f2;if((unsigned int)_tmp264 > 3?*((int*)_tmp264)== 13: 0){_LL349:
_tmp265=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp264)->f1;goto _LL2DA;}
else{goto _LL2DB;}_LL2DB: _LL350: _tmp26C=_tmp210.f1;if((unsigned int)_tmp26C > 3?*((
int*)_tmp26C)== 7: 0){_LL353: _tmp26F=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp26C)->f1;goto _LL352;_LL352: _tmp26E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp26C)->f2;goto _LL351;_LL351: _tmp26D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp26C)->f3;goto _LL34C;}else{goto _LL2DD;}_LL34C: _tmp268=_tmp210.f2;if((
unsigned int)_tmp268 > 3?*((int*)_tmp268)== 7: 0){_LL34F: _tmp26B=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp268)->f1;goto _LL34E;_LL34E: _tmp26A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp268)->f2;goto _LL34D;_LL34D: _tmp269=((struct Cyc_Absyn_ArrayType_struct*)
_tmp268)->f3;goto _LL2DC;}else{goto _LL2DD;}_LL2DD: _LL35E: _tmp27A=_tmp210.f1;if((
unsigned int)_tmp27A > 3?*((int*)_tmp27A)== 8: 0){_LL35F: _tmp27B=((struct Cyc_Absyn_FnType_struct*)
_tmp27A)->f1;_LL367: _tmp283=_tmp27B.tvars;goto _LL366;_LL366: _tmp282=_tmp27B.effect;
goto _LL365;_LL365: _tmp281=(void*)_tmp27B.ret_typ;goto _LL364;_LL364: _tmp280=
_tmp27B.args;goto _LL363;_LL363: _tmp27F=_tmp27B.c_varargs;goto _LL362;_LL362:
_tmp27E=_tmp27B.cyc_varargs;goto _LL361;_LL361: _tmp27D=_tmp27B.rgn_po;goto _LL360;
_LL360: _tmp27C=_tmp27B.attributes;goto _LL354;}else{goto _LL2DF;}_LL354: _tmp270=
_tmp210.f2;if((unsigned int)_tmp270 > 3?*((int*)_tmp270)== 8: 0){_LL355: _tmp271=((
struct Cyc_Absyn_FnType_struct*)_tmp270)->f1;_LL35D: _tmp279=_tmp271.tvars;goto
_LL35C;_LL35C: _tmp278=_tmp271.effect;goto _LL35B;_LL35B: _tmp277=(void*)_tmp271.ret_typ;
goto _LL35A;_LL35A: _tmp276=_tmp271.args;goto _LL359;_LL359: _tmp275=_tmp271.c_varargs;
goto _LL358;_LL358: _tmp274=_tmp271.cyc_varargs;goto _LL357;_LL357: _tmp273=_tmp271.rgn_po;
goto _LL356;_LL356: _tmp272=_tmp271.attributes;goto _LL2DE;}else{goto _LL2DF;}_LL2DF:
_LL36A: _tmp286=_tmp210.f1;if((unsigned int)_tmp286 > 3?*((int*)_tmp286)== 9: 0){
_LL36B: _tmp287=((struct Cyc_Absyn_TupleType_struct*)_tmp286)->f1;goto _LL368;}
else{goto _LL2E1;}_LL368: _tmp284=_tmp210.f2;if((unsigned int)_tmp284 > 3?*((int*)
_tmp284)== 9: 0){_LL369: _tmp285=((struct Cyc_Absyn_TupleType_struct*)_tmp284)->f1;
goto _LL2E0;}else{goto _LL2E1;}_LL2E1: _LL36E: _tmp28A=_tmp210.f1;if((unsigned int)
_tmp28A > 3?*((int*)_tmp28A)== 14: 0){_LL36F: _tmp28B=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp28A)->f1;goto _LL36C;}else{goto _LL2E3;}_LL36C: _tmp288=_tmp210.f2;if((
unsigned int)_tmp288 > 3?*((int*)_tmp288)== 14: 0){_LL36D: _tmp289=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp288)->f1;goto _LL2E2;}else{goto _LL2E3;}_LL2E3: _LL372: _tmp28E=_tmp210.f1;if((
unsigned int)_tmp28E > 3?*((int*)_tmp28E)== 15: 0){_LL373: _tmp28F=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp28E)->f1;goto _LL370;}else{goto _LL2E5;}_LL370: _tmp28C=_tmp210.f2;if((
unsigned int)_tmp28C > 3?*((int*)_tmp28C)== 15: 0){_LL371: _tmp28D=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp28C)->f1;goto _LL2E4;}else{goto _LL2E5;}_LL2E5: _LL375: _tmp291=_tmp210.f1;if((
int)_tmp291 == 2){goto _LL374;}else{goto _LL2E7;}_LL374: _tmp290=_tmp210.f2;if((int)
_tmp290 == 2){goto _LL2E6;}else{goto _LL2E7;}_LL2E7: _LL378: _tmp294=_tmp210.f1;if((
unsigned int)_tmp294 > 3?*((int*)_tmp294)== 17: 0){_LL379: _tmp295=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp294)->f1;goto _LL376;}else{goto _LL2E9;}_LL376:
_tmp292=_tmp210.f2;if((unsigned int)_tmp292 > 3?*((int*)_tmp292)== 17: 0){_LL377:
_tmp293=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp292)->f1;goto _LL2E8;}
else{goto _LL2E9;}_LL2E9: _LL37A: _tmp296=_tmp210.f1;if((unsigned int)_tmp296 > 3?*((
int*)_tmp296)== 20: 0){goto _LL2EA;}else{goto _LL2EB;}_LL2EB: _LL37B: _tmp297=_tmp210.f2;
if((unsigned int)_tmp297 > 3?*((int*)_tmp297)== 20: 0){goto _LL2EC;}else{goto _LL2ED;}
_LL2ED: _LL37C: _tmp298=_tmp210.f1;if((unsigned int)_tmp298 > 3?*((int*)_tmp298)== 
19: 0){goto _LL2EE;}else{goto _LL2EF;}_LL2EF: _LL37D: _tmp299=_tmp210.f1;if((
unsigned int)_tmp299 > 3?*((int*)_tmp299)== 21: 0){goto _LL2F0;}else{goto _LL2F1;}
_LL2F1: _LL37E: _tmp29A=_tmp210.f2;if((unsigned int)_tmp29A > 3?*((int*)_tmp29A)== 
21: 0){goto _LL2F2;}else{goto _LL2F3;}_LL2F3: _LL37F: _tmp29B=_tmp210.f2;if((
unsigned int)_tmp29B > 3?*((int*)_tmp29B)== 19: 0){goto _LL2F4;}else{goto _LL2F5;}
_LL2F5: goto _LL2F6;_LL2C0: Cyc_Tcutil_unify_it(t2,t1);return;_LL2C2: return;_LL2C4: {
struct _tagged_arr*_tmp29C=_tmp217->name;struct _tagged_arr*_tmp29D=_tmp215->name;
int _tmp29E=*((int*)_check_null(_tmp217->identity));int _tmp29F=*((int*)
_check_null(_tmp215->identity));void*_tmp2A0=Cyc_Tcutil_tvar_kind(_tmp217);void*
_tmp2A1=Cyc_Tcutil_tvar_kind(_tmp215);if(_tmp29F == _tmp29E? Cyc_Std_zstrptrcmp(
_tmp29C,_tmp29D)== 0: 0){if(_tmp2A0 != _tmp2A1){({struct Cyc_Std_String_pa_struct
_tmp2A5;_tmp2A5.tag=0;_tmp2A5.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2A1);{struct Cyc_Std_String_pa_struct _tmp2A4;_tmp2A4.tag=0;_tmp2A4.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2A0);{struct Cyc_Std_String_pa_struct
_tmp2A3;_tmp2A3.tag=0;_tmp2A3.f1=(struct _tagged_arr)*_tmp29C;{void*_tmp2A2[3]={&
_tmp2A3,& _tmp2A4,& _tmp2A5};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("same type variable %s has kinds %s and %s",sizeof(
unsigned char),42),_tag_arr(_tmp2A2,sizeof(void*),3));}}}});}return;}Cyc_Tcutil_failure_reason=
_tag_arr("(variable types are not the same)",sizeof(unsigned char),34);goto
_LL2BE;}_LL2C6: if(((_tmp21B != 0? _tmp21F != 0: 0)? Cyc_Absyn_qvar_cmp((struct _tuple1*)
_check_null(_tmp21B),(struct _tuple1*)_check_null(_tmp21F))== 0: 0)? 1:(_tmp21B == 0?
_tmp21F == 0: 0)){Cyc_Tcutil_unify_list(_tmp21A,_tmp21E);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different struct types)",sizeof(unsigned char),25);goto _LL2BE;_LL2C8:
if(Cyc_Absyn_qvar_cmp(_tmp223,_tmp221)== 0){return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different enum types)",sizeof(unsigned char),23);goto _LL2BE;_LL2CA: {
int bad=0;for(0;_tmp227 != 0? _tmp225 != 0: 0;(_tmp227=_tmp227->tl,_tmp225=_tmp225->tl)){
struct Cyc_Absyn_Enumfield*_tmp2A6=(struct Cyc_Absyn_Enumfield*)_tmp227->hd;struct
Cyc_Absyn_Enumfield*_tmp2A7=(struct Cyc_Absyn_Enumfield*)_tmp225->hd;if(Cyc_Absyn_qvar_cmp(
_tmp2A6->name,_tmp2A7->name)!= 0){Cyc_Tcutil_failure_reason=_tag_arr("(different names for enum fields)",
sizeof(unsigned char),34);bad=1;break;}if(_tmp2A6->tag == _tmp2A7->tag){continue;}
if(_tmp2A6->tag == 0? 1: _tmp2A7->tag == 0){Cyc_Tcutil_failure_reason=_tag_arr("(different tag values for enum fields)",
sizeof(unsigned char),39);bad=1;break;}if(Cyc_Evexp_eval_const_uint_exp((struct
Cyc_Absyn_Exp*)_check_null(_tmp2A6->tag))!= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2A7->tag))){Cyc_Tcutil_failure_reason=
_tag_arr("(different tag values for enum fields)",sizeof(unsigned char),39);bad=
1;break;}}if(bad){goto _LL2BE;}if(_tmp227 == 0? _tmp225 == 0: 0){return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different number of fields for enums)",sizeof(unsigned char),39);goto
_LL2BE;}_LL2CC: if(((_tmp22B != 0? _tmp22F != 0: 0)? Cyc_Absyn_qvar_cmp((struct _tuple1*)
_check_null(_tmp22B),(struct _tuple1*)_check_null(_tmp22F))== 0: 0)? 1:(_tmp22B == 0?
_tmp22F == 0: 0)){Cyc_Tcutil_unify_list(_tmp22A,_tmp22E);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different union types)",sizeof(unsigned char),24);goto _LL2BE;_LL2CE:
if(_tmp23D == _tmp236? 1: Cyc_Absyn_qvar_cmp(_tmp23D->name,_tmp236->name)== 0){Cyc_Tcutil_unify_it(
_tmp232,_tmp239);Cyc_Tcutil_unify_list(_tmp233,_tmp23A);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion types)",sizeof(unsigned char),25);goto _LL2BE;_LL2D0:
if((_tmp249 == _tmp243? 1: Cyc_Absyn_qvar_cmp(_tmp249->name,_tmp243->name)== 0)?
_tmp248 == _tmp242? 1: Cyc_Absyn_qvar_cmp(_tmp248->name,_tmp242->name)== 0: 0){Cyc_Tcutil_unify_list(
_tmp240,_tmp246);return;}Cyc_Tcutil_failure_reason=_tag_arr("(different tunion field types)",
sizeof(unsigned char),31);goto _LL2BE;_LL2D2: Cyc_Tcutil_unify_it(_tmp250,_tmp257);
Cyc_Tcutil_unify_it(_tmp256,_tmp24F);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;Cyc_Tcutil_unify_tqual(_tmp24D,_tmp254);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_boundscmp,
_tmp24C,_tmp253,_tag_arr("(different pointer bounds)",sizeof(unsigned char),27));{
void*_tmp2A8=(void*)(Cyc_Absyn_compress_conref(_tmp24C))->v;void*_tmp2A9;_LL381:
if((unsigned int)_tmp2A8 > 1?*((int*)_tmp2A8)== 0: 0){_LL385: _tmp2A9=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp2A8)->f1;if((int)_tmp2A9 == 0){goto _LL382;}
else{goto _LL383;}}else{goto _LL383;}_LL383: goto _LL384;_LL382: return;_LL384: goto
_LL380;_LL380:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp24E,
_tmp255,_tag_arr("(different pointer types)",sizeof(unsigned char),26));return;
_LL2D4: if(_tmp25A == _tmp25D? _tmp259 == _tmp25C: 0){return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different integral types)",sizeof(unsigned char),27);goto _LL2BE;
_LL2D6: return;_LL2D8: if(_tmp263 == _tmp261){return;}goto _LL2BE;_LL2DA: Cyc_Tcutil_unify_it(
_tmp267,_tmp265);return;_LL2DC: Cyc_Tcutil_unify_tqual(_tmp26A,_tmp26E);Cyc_Tcutil_unify_it(
_tmp26B,_tmp26F);if(_tmp26D == _tmp269){return;}if(_tmp26D == 0? 1: _tmp269 == 0){
goto _LL2BE;}if(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp26D))== Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp269))){return;}Cyc_Tcutil_failure_reason=_tag_arr("(different array sizes)",
sizeof(unsigned char),24);goto _LL2BE;_LL2DE: {int done=0;{struct _RegionHandle
_tmp2AA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2AA;_push_region(rgn);{
struct Cyc_List_List*inst=0;while(_tmp279 != 0){if(_tmp283 == 0){Cyc_Tcutil_failure_reason=
_tag_arr("(second function type has too few type variables)",sizeof(
unsigned char),50);(int)_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp2AB=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp279->hd);void*_tmp2AC=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp283->hd);if(_tmp2AB != _tmp2AC){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp2B0;_tmp2B0.tag=0;_tmp2B0.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2AC);{struct Cyc_Std_String_pa_struct
_tmp2AF;_tmp2AF.tag=0;_tmp2AF.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2AB);{struct Cyc_Std_String_pa_struct _tmp2AE;_tmp2AE.tag=0;_tmp2AE.f1=(struct
_tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp279->hd);{void*
_tmp2AD[3]={& _tmp2AE,& _tmp2AF,& _tmp2B0};Cyc_Std_aprintf(_tag_arr("(type var %s has different kinds %s and %s)",
sizeof(unsigned char),44),_tag_arr(_tmp2AD,sizeof(void*),3));}}}});(int)_throw((
void*)Cyc_Tcutil_Unify);}inst=({struct Cyc_List_List*_tmp2B1=_region_malloc(rgn,
sizeof(*_tmp2B1));_tmp2B1->hd=({struct _tuple5*_tmp2B2=_region_malloc(rgn,sizeof(*
_tmp2B2));_tmp2B2->f1=(struct Cyc_Absyn_Tvar*)_tmp283->hd;_tmp2B2->f2=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));_tmp2B3[0]=({
struct Cyc_Absyn_VarType_struct _tmp2B4;_tmp2B4.tag=1;_tmp2B4.f1=(struct Cyc_Absyn_Tvar*)
_tmp279->hd;_tmp2B4;});_tmp2B3;});_tmp2B2;});_tmp2B1->tl=inst;_tmp2B1;});_tmp279=
_tmp279->tl;_tmp283=_tmp283->tl;}}if(_tmp283 != 0){Cyc_Tcutil_failure_reason=
_tag_arr("(second function type has too many type variables)",sizeof(
unsigned char),51);_npop_handler(0);goto _LL2BE;}if(inst != 0){Cyc_Tcutil_unify_it((
void*)({struct Cyc_Absyn_FnType_struct*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));
_tmp2B5[0]=({struct Cyc_Absyn_FnType_struct _tmp2B6;_tmp2B6.tag=8;_tmp2B6.f1=({
struct Cyc_Absyn_FnInfo _tmp2B7;_tmp2B7.tvars=0;_tmp2B7.effect=_tmp278;_tmp2B7.ret_typ=(
void*)_tmp277;_tmp2B7.args=_tmp276;_tmp2B7.c_varargs=_tmp275;_tmp2B7.cyc_varargs=
_tmp274;_tmp2B7.rgn_po=_tmp273;_tmp2B7.attributes=_tmp272;_tmp2B7;});_tmp2B6;});
_tmp2B5;}),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8[0]=({struct Cyc_Absyn_FnType_struct
_tmp2B9;_tmp2B9.tag=8;_tmp2B9.f1=({struct Cyc_Absyn_FnInfo _tmp2BA;_tmp2BA.tvars=0;
_tmp2BA.effect=_tmp282;_tmp2BA.ret_typ=(void*)_tmp281;_tmp2BA.args=_tmp280;
_tmp2BA.c_varargs=_tmp27F;_tmp2BA.cyc_varargs=_tmp27E;_tmp2BA.rgn_po=_tmp27D;
_tmp2BA.attributes=_tmp27C;_tmp2BA;});_tmp2B9;});_tmp2B8;})));done=1;}};
_pop_region(rgn);}if(done){return;}Cyc_Tcutil_unify_it(_tmp277,_tmp281);for(0;
_tmp276 != 0? _tmp280 != 0: 0;(_tmp276=_tmp276->tl,_tmp280=_tmp280->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)_tmp276->hd)).f2,(*((struct _tuple2*)_tmp280->hd)).f2);Cyc_Tcutil_unify_it((*((
struct _tuple2*)_tmp276->hd)).f3,(*((struct _tuple2*)_tmp280->hd)).f3);}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp276 != 0? 1: _tmp280 != 0){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different number of arguments)",sizeof(
unsigned char),52);goto _LL2BE;}if(_tmp275 != _tmp27F){Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type takes C varargs)",sizeof(unsigned char),41);
goto _LL2BE;}{int bad_cyc_vararg=0;{struct _tuple11 _tmp2BC=({struct _tuple11 _tmp2BB;
_tmp2BB.f1=_tmp274;_tmp2BB.f2=_tmp27E;_tmp2BB;});struct Cyc_Absyn_VarargInfo*
_tmp2BD;struct Cyc_Absyn_VarargInfo*_tmp2BE;struct Cyc_Absyn_VarargInfo*_tmp2BF;
struct Cyc_Absyn_VarargInfo*_tmp2C0;struct Cyc_Absyn_VarargInfo*_tmp2C1;struct Cyc_Absyn_VarargInfo
_tmp2C2;int _tmp2C3;void*_tmp2C4;struct Cyc_Absyn_Tqual _tmp2C5;struct Cyc_Core_Opt*
_tmp2C6;struct Cyc_Absyn_VarargInfo*_tmp2C7;struct Cyc_Absyn_VarargInfo _tmp2C8;int
_tmp2C9;void*_tmp2CA;struct Cyc_Absyn_Tqual _tmp2CB;struct Cyc_Core_Opt*_tmp2CC;
_LL387: _LL390: _tmp2BE=_tmp2BC.f1;if(_tmp2BE == 0){goto _LL38F;}else{goto _LL389;}
_LL38F: _tmp2BD=_tmp2BC.f2;if(_tmp2BD == 0){goto _LL388;}else{goto _LL389;}_LL389:
_LL391: _tmp2BF=_tmp2BC.f1;if(_tmp2BF == 0){goto _LL38A;}else{goto _LL38B;}_LL38B:
_LL392: _tmp2C0=_tmp2BC.f2;if(_tmp2C0 == 0){goto _LL38C;}else{goto _LL38D;}_LL38D:
_LL398: _tmp2C7=_tmp2BC.f1;if(_tmp2C7 == 0){goto _LL386;}else{_tmp2C8=*_tmp2C7;
_LL39C: _tmp2CC=_tmp2C8.name;goto _LL39B;_LL39B: _tmp2CB=_tmp2C8.tq;goto _LL39A;
_LL39A: _tmp2CA=(void*)_tmp2C8.type;goto _LL399;_LL399: _tmp2C9=_tmp2C8.inject;goto
_LL393;}_LL393: _tmp2C1=_tmp2BC.f2;if(_tmp2C1 == 0){goto _LL386;}else{_tmp2C2=*
_tmp2C1;_LL397: _tmp2C6=_tmp2C2.name;goto _LL396;_LL396: _tmp2C5=_tmp2C2.tq;goto
_LL395;_LL395: _tmp2C4=(void*)_tmp2C2.type;goto _LL394;_LL394: _tmp2C3=_tmp2C2.inject;
goto _LL38E;}_LL388: goto _LL386;_LL38A: goto _LL38C;_LL38C: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type takes varargs)",sizeof(unsigned char),39);goto
_LL386;_LL38E: Cyc_Tcutil_unify_tqual(_tmp2CB,_tmp2C5);Cyc_Tcutil_unify_it(
_tmp2CA,_tmp2C4);if(_tmp2C9 != _tmp2C3){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type injects varargs)",sizeof(unsigned char),41);}
goto _LL386;_LL386:;}if(bad_cyc_vararg){goto _LL2BE;}{int bad_effect=0;{struct
_tuple12 _tmp2CE=({struct _tuple12 _tmp2CD;_tmp2CD.f1=_tmp278;_tmp2CD.f2=_tmp282;
_tmp2CD;});struct Cyc_Core_Opt*_tmp2CF;struct Cyc_Core_Opt*_tmp2D0;struct Cyc_Core_Opt*
_tmp2D1;struct Cyc_Core_Opt*_tmp2D2;_LL39E: _LL3A7: _tmp2D0=_tmp2CE.f1;if(_tmp2D0 == 
0){goto _LL3A6;}else{goto _LL3A0;}_LL3A6: _tmp2CF=_tmp2CE.f2;if(_tmp2CF == 0){goto
_LL39F;}else{goto _LL3A0;}_LL3A0: _LL3A8: _tmp2D1=_tmp2CE.f1;if(_tmp2D1 == 0){goto
_LL3A1;}else{goto _LL3A2;}_LL3A2: _LL3A9: _tmp2D2=_tmp2CE.f2;if(_tmp2D2 == 0){goto
_LL3A3;}else{goto _LL3A4;}_LL3A4: goto _LL3A5;_LL39F: goto _LL39D;_LL3A1: goto _LL3A3;
_LL3A3: bad_effect=1;goto _LL39D;_LL3A5: bad_effect=! Cyc_Tcutil_unify_effect((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp278))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp282))->v);goto _LL39D;_LL39D:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=_tag_arr("(function type effects do not unify)",
sizeof(unsigned char),37);goto _LL2BE;}if(! Cyc_Tcutil_same_atts(_tmp27C,_tmp272)){
Cyc_Tcutil_failure_reason=_tag_arr("(function types have different attributes)",
sizeof(unsigned char),43);goto _LL2BE;}if(! Cyc_Tcutil_same_rgn_po(_tmp27D,_tmp273)){
Cyc_Tcutil_failure_reason=_tag_arr("(function types have different region lifetime orderings)",
sizeof(unsigned char),58);goto _LL2BE;}return;}}}_LL2E0: for(0;_tmp285 != 0? _tmp287
!= 0: 0;(_tmp285=_tmp285->tl,_tmp287=_tmp287->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)_tmp285->hd)).f1,(*((struct _tuple4*)_tmp287->hd)).f1);Cyc_Tcutil_unify_it((*((
struct _tuple4*)_tmp285->hd)).f2,(*((struct _tuple4*)_tmp287->hd)).f2);}if(_tmp285
== 0? _tmp287 == 0: 0){return;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_arr("(tuple types have different numbers of components)",
sizeof(unsigned char),51);goto _LL2BE;_LL2E2: _tmp28F=_tmp28B;_tmp28D=_tmp289;goto
_LL2E4;_LL2E4: for(0;_tmp28D != 0? _tmp28F != 0: 0;(_tmp28D=_tmp28D->tl,_tmp28F=
_tmp28F->tl)){struct Cyc_Absyn_Structfield*_tmp2D3=(struct Cyc_Absyn_Structfield*)
_tmp28D->hd;struct Cyc_Absyn_Structfield*_tmp2D4=(struct Cyc_Absyn_Structfield*)
_tmp28F->hd;if(Cyc_Std_zstrptrcmp(_tmp2D3->name,_tmp2D4->name)!= 0){Cyc_Tcutil_failure_reason=
_tag_arr("(different member names in anonymous structs/unions)",sizeof(
unsigned char),53);(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_tqual(
_tmp2D3->tq,_tmp2D4->tq);Cyc_Tcutil_unify_it((void*)_tmp2D3->type,(void*)_tmp2D4->type);
if(! Cyc_Tcutil_same_atts(_tmp2D3->attributes,_tmp2D4->attributes)){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=_tag_arr("(different attributes on member)",
sizeof(unsigned char),33);(int)_throw((void*)Cyc_Tcutil_Unify);}if(((_tmp2D3->width
!= 0? _tmp2D4->width == 0: 0)? 1:(_tmp2D4->width != 0? _tmp2D3->width == 0: 0))? 1:((
_tmp2D3->width != 0? _tmp2D4->width != 0: 0)? Cyc_Evexp_eval_const_uint_exp((struct
Cyc_Absyn_Exp*)_check_null(_tmp2D3->width))!= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2D4->width)): 0)){Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=_tag_arr("(different bitfield widths on member)",
sizeof(unsigned char),38);(int)_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp28D == 0?
_tmp28F == 0: 0){return;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different number of members)",sizeof(unsigned char),30);goto _LL2BE;
_LL2E6: return;_LL2E8: Cyc_Tcutil_unify_it(_tmp295,_tmp293);return;_LL2EA: goto
_LL2EC;_LL2EC: goto _LL2EE;_LL2EE: goto _LL2F0;_LL2F0: goto _LL2F2;_LL2F2: goto _LL2F4;
_LL2F4: if(Cyc_Tcutil_unify_effect(t1,t2)){return;}Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)",sizeof(unsigned char),22);goto _LL2BE;_LL2F6:
goto _LL2BE;_LL2BE:;}(int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_opt_cmp(
int(*cmp)(void*,void*),struct Cyc_Core_Opt*a1,struct Cyc_Core_Opt*a2){if(a1 == a2){
return 0;}if(a1 == 0? a2 != 0: 0){return - 1;}if(a1 != 0? a2 == 0: 0){return 1;}return cmp((
void*)((struct Cyc_Core_Opt*)_check_null(a1))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(a2))->v);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*
a2){if(a1 == a2){return 0;}if(a1 == 0? a2 != 0: 0){return - 1;}if(a1 != 0? a2 == 0: 0){
return 1;}return cmp((void*)_check_null(a1),(void*)_check_null(a2));}int Cyc_Tcutil_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2){
return 0;}for(0;l1 != 0? l2 != 0: 0;(l1=l1->tl,l2=l2->tl)){int _tmp2D5=cmp((void*)l1->hd,(
void*)l2->hd);if(_tmp2D5 != 0){return _tmp2D5;}}if(l2 != 0){return - 1;}if(l1 != 0){
return 1;}return 0;}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct
Cyc_Absyn_Tqual tq2){int _tmp2D6=(tq1.q_const + (tq1.q_volatile << 1))+ (tq1.q_restrict
<< 2);int _tmp2D7=(tq2.q_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp2D6,_tmp2D7);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(
void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y){return 0;}{void*_tmp2D8=(void*)x->v;
void*_tmp2D9;_LL3AB: if((int)_tmp2D8 == 0){goto _LL3AC;}else{goto _LL3AD;}_LL3AD: if((
unsigned int)_tmp2D8 > 1?*((int*)_tmp2D8)== 1: 0){goto _LL3AE;}else{goto _LL3AF;}
_LL3AF: if((unsigned int)_tmp2D8 > 1?*((int*)_tmp2D8)== 0: 0){_LL3B1: _tmp2D9=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp2D8)->f1;goto _LL3B0;}else{goto _LL3AA;}
_LL3AC: return - 1;_LL3AE:(int)_throw(({void*_tmp2DA[0]={};Cyc_Tcutil_impos(
_tag_arr("unify_conref: forward after compress",sizeof(unsigned char),37),
_tag_arr(_tmp2DA,sizeof(void*),0));}));_LL3B0: {void*_tmp2DB=(void*)y->v;void*
_tmp2DC;_LL3B3: if((int)_tmp2DB == 0){goto _LL3B4;}else{goto _LL3B5;}_LL3B5: if((
unsigned int)_tmp2DB > 1?*((int*)_tmp2DB)== 1: 0){goto _LL3B6;}else{goto _LL3B7;}
_LL3B7: if((unsigned int)_tmp2DB > 1?*((int*)_tmp2DB)== 0: 0){_LL3B9: _tmp2DC=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp2DB)->f1;goto _LL3B8;}else{goto _LL3B2;}
_LL3B4: return 1;_LL3B6:(int)_throw(({void*_tmp2DD[0]={};Cyc_Tcutil_impos(_tag_arr("unify_conref: forward after compress(2)",
sizeof(unsigned char),40),_tag_arr(_tmp2DD,sizeof(void*),0));}));_LL3B8: return
cmp(_tmp2D9,_tmp2DC);_LL3B2:;}_LL3AA:;}}static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp2DF;void*_tmp2E0;struct
Cyc_Absyn_Tqual _tmp2E1;struct _tuple4*_tmp2DE=tqt1;_tmp2DF=*_tmp2DE;_LL3BC:
_tmp2E1=_tmp2DF.f1;goto _LL3BB;_LL3BB: _tmp2E0=_tmp2DF.f2;goto _LL3BA;_LL3BA: {
struct _tuple4 _tmp2E3;void*_tmp2E4;struct Cyc_Absyn_Tqual _tmp2E5;struct _tuple4*
_tmp2E2=tqt2;_tmp2E3=*_tmp2E2;_LL3BF: _tmp2E5=_tmp2E3.f1;goto _LL3BE;_LL3BE:
_tmp2E4=_tmp2E3.f2;goto _LL3BD;_LL3BD: {int _tmp2E6=Cyc_Tcutil_tqual_cmp(_tmp2E1,
_tmp2E5);if(_tmp2E6 != 0){return _tmp2E6;}return Cyc_Tcutil_typecmp(_tmp2E0,_tmp2E4);}}}
static int Cyc_Tcutil_const_uint_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){return Cyc_Core_intcmp((int)Cyc_Evexp_eval_const_uint_exp(e1),(int)Cyc_Evexp_eval_const_uint_exp(
e2));}static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp2E7=att;
_LL3C1: if((unsigned int)_tmp2E7 > 16?*((int*)_tmp2E7)== 0: 0){goto _LL3C2;}else{
goto _LL3C3;}_LL3C3: if((int)_tmp2E7 == 0){goto _LL3C4;}else{goto _LL3C5;}_LL3C5: if((
int)_tmp2E7 == 1){goto _LL3C6;}else{goto _LL3C7;}_LL3C7: if((int)_tmp2E7 == 2){goto
_LL3C8;}else{goto _LL3C9;}_LL3C9: if((int)_tmp2E7 == 3){goto _LL3CA;}else{goto _LL3CB;}
_LL3CB: if((int)_tmp2E7 == 4){goto _LL3CC;}else{goto _LL3CD;}_LL3CD: if((unsigned int)
_tmp2E7 > 16?*((int*)_tmp2E7)== 1: 0){goto _LL3CE;}else{goto _LL3CF;}_LL3CF: if((int)
_tmp2E7 == 5){goto _LL3D0;}else{goto _LL3D1;}_LL3D1: if((unsigned int)_tmp2E7 > 16?*((
int*)_tmp2E7)== 2: 0){goto _LL3D2;}else{goto _LL3D3;}_LL3D3: if((int)_tmp2E7 == 6){
goto _LL3D4;}else{goto _LL3D5;}_LL3D5: if((int)_tmp2E7 == 7){goto _LL3D6;}else{goto
_LL3D7;}_LL3D7: if((int)_tmp2E7 == 8){goto _LL3D8;}else{goto _LL3D9;}_LL3D9: if((int)
_tmp2E7 == 9){goto _LL3DA;}else{goto _LL3DB;}_LL3DB: if((int)_tmp2E7 == 10){goto
_LL3DC;}else{goto _LL3DD;}_LL3DD: if((int)_tmp2E7 == 11){goto _LL3DE;}else{goto
_LL3DF;}_LL3DF: if((int)_tmp2E7 == 12){goto _LL3E0;}else{goto _LL3E1;}_LL3E1: if((int)
_tmp2E7 == 13){goto _LL3E2;}else{goto _LL3E3;}_LL3E3: if((int)_tmp2E7 == 14){goto
_LL3E4;}else{goto _LL3E5;}_LL3E5: if((int)_tmp2E7 == 15){goto _LL3E6;}else{goto
_LL3E7;}_LL3E7: if((unsigned int)_tmp2E7 > 16?*((int*)_tmp2E7)== 3: 0){goto _LL3E8;}
else{goto _LL3C0;}_LL3C2: return 0;_LL3C4: return 1;_LL3C6: return 2;_LL3C8: return 3;
_LL3CA: return 4;_LL3CC: return 5;_LL3CE: return 6;_LL3D0: return 7;_LL3D2: return 8;
_LL3D4: return 9;_LL3D6: return 10;_LL3D8: return 11;_LL3DA: return 12;_LL3DC: return 13;
_LL3DE: return 14;_LL3E0: return 15;_LL3E2: return 16;_LL3E4: return 17;_LL3E6: return 18;
_LL3E8: return 19;_LL3C0:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple8 _tmp2E9=({struct _tuple8 _tmp2E8;_tmp2E8.f1=att1;_tmp2E8.f2=att2;
_tmp2E8;});void*_tmp2EA;int _tmp2EB;void*_tmp2EC;int _tmp2ED;void*_tmp2EE;int
_tmp2EF;void*_tmp2F0;int _tmp2F1;void*_tmp2F2;struct _tagged_arr _tmp2F3;void*
_tmp2F4;struct _tagged_arr _tmp2F5;void*_tmp2F6;int _tmp2F7;int _tmp2F8;void*_tmp2F9;
void*_tmp2FA;int _tmp2FB;int _tmp2FC;void*_tmp2FD;_LL3EA: _LL3F6: _tmp2EC=_tmp2E9.f1;
if((unsigned int)_tmp2EC > 16?*((int*)_tmp2EC)== 0: 0){_LL3F7: _tmp2ED=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp2EC)->f1;goto _LL3F4;}else{goto _LL3EC;}_LL3F4: _tmp2EA=_tmp2E9.f2;if((
unsigned int)_tmp2EA > 16?*((int*)_tmp2EA)== 0: 0){_LL3F5: _tmp2EB=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp2EA)->f1;goto _LL3EB;}else{goto _LL3EC;}_LL3EC: _LL3FA: _tmp2F0=_tmp2E9.f1;if((
unsigned int)_tmp2F0 > 16?*((int*)_tmp2F0)== 1: 0){_LL3FB: _tmp2F1=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp2F0)->f1;goto _LL3F8;}else{goto _LL3EE;}_LL3F8: _tmp2EE=_tmp2E9.f2;if((
unsigned int)_tmp2EE > 16?*((int*)_tmp2EE)== 1: 0){_LL3F9: _tmp2EF=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp2EE)->f1;goto _LL3ED;}else{goto _LL3EE;}_LL3EE: _LL3FE: _tmp2F4=_tmp2E9.f1;if((
unsigned int)_tmp2F4 > 16?*((int*)_tmp2F4)== 2: 0){_LL3FF: _tmp2F5=((struct Cyc_Absyn_Section_att_struct*)
_tmp2F4)->f1;goto _LL3FC;}else{goto _LL3F0;}_LL3FC: _tmp2F2=_tmp2E9.f2;if((
unsigned int)_tmp2F2 > 16?*((int*)_tmp2F2)== 2: 0){_LL3FD: _tmp2F3=((struct Cyc_Absyn_Section_att_struct*)
_tmp2F2)->f1;goto _LL3EF;}else{goto _LL3F0;}_LL3F0: _LL404: _tmp2FA=_tmp2E9.f1;if((
unsigned int)_tmp2FA > 16?*((int*)_tmp2FA)== 3: 0){_LL407: _tmp2FD=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp2FA)->f1;goto _LL406;_LL406: _tmp2FC=((struct Cyc_Absyn_Format_att_struct*)
_tmp2FA)->f2;goto _LL405;_LL405: _tmp2FB=((struct Cyc_Absyn_Format_att_struct*)
_tmp2FA)->f3;goto _LL400;}else{goto _LL3F2;}_LL400: _tmp2F6=_tmp2E9.f2;if((
unsigned int)_tmp2F6 > 16?*((int*)_tmp2F6)== 3: 0){_LL403: _tmp2F9=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp2F6)->f1;goto _LL402;_LL402: _tmp2F8=((struct Cyc_Absyn_Format_att_struct*)
_tmp2F6)->f2;goto _LL401;_LL401: _tmp2F7=((struct Cyc_Absyn_Format_att_struct*)
_tmp2F6)->f3;goto _LL3F1;}else{goto _LL3F2;}_LL3F2: goto _LL3F3;_LL3EB: _tmp2F1=
_tmp2ED;_tmp2EF=_tmp2EB;goto _LL3ED;_LL3ED: return Cyc_Core_intcmp(_tmp2F1,_tmp2EF);
_LL3EF: return Cyc_Std_strcmp(_tmp2F5,_tmp2F3);_LL3F1: {int _tmp2FE=Cyc_Core_intcmp((
int)((unsigned int)_tmp2FD),(int)((unsigned int)_tmp2F9));if(_tmp2FE != 0){return
_tmp2FE;}{int _tmp2FF=Cyc_Core_intcmp(_tmp2FC,_tmp2F8);if(_tmp2FF != 0){return
_tmp2FF;}return Cyc_Core_intcmp(_tmp2FB,_tmp2F7);}}_LL3F3: {int _tmp300=Cyc_Tcutil_attribute_case_number(
att1);int _tmp301=Cyc_Tcutil_attribute_case_number(att2);return Cyc_Core_intcmp(
_tmp300,_tmp301);}_LL3E9:;}static int Cyc_Tcutil_structfield_cmp(struct Cyc_Absyn_Structfield*
f1,struct Cyc_Absyn_Structfield*f2){int _tmp302=Cyc_Std_zstrptrcmp(f1->name,f2->name);
if(_tmp302 != 0){return _tmp302;}{int _tmp303=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp303 != 0){return _tmp303;}{int _tmp304=Cyc_Tcutil_typecmp((void*)f1->type,(
void*)f2->type);if(_tmp304 != 0){return _tmp304;}{int _tmp305=Cyc_Tcutil_list_cmp(
Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);if(_tmp305 != 0){return
_tmp305;}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,
f1->width,f2->width);}}}}static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*
e1,struct Cyc_Absyn_Enumfield*e2){int _tmp306=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp306 != 0){return _tmp306;}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Tcutil_const_uint_exp_cmp,e1->tag,e2->tag);}extern int Cyc_Tcutil_typecmp(void*
t1,void*t2);int Cyc_Tcutil_type_case_number(void*t){void*_tmp307=t;_LL409: if((int)
_tmp307 == 0){goto _LL40A;}else{goto _LL40B;}_LL40B: if((unsigned int)_tmp307 > 3?*((
int*)_tmp307)== 0: 0){goto _LL40C;}else{goto _LL40D;}_LL40D: if((unsigned int)
_tmp307 > 3?*((int*)_tmp307)== 1: 0){goto _LL40E;}else{goto _LL40F;}_LL40F: if((
unsigned int)_tmp307 > 3?*((int*)_tmp307)== 2: 0){goto _LL410;}else{goto _LL411;}
_LL411: if((unsigned int)_tmp307 > 3?*((int*)_tmp307)== 3: 0){goto _LL412;}else{goto
_LL413;}_LL413: if((unsigned int)_tmp307 > 3?*((int*)_tmp307)== 4: 0){goto _LL414;}
else{goto _LL415;}_LL415: if((unsigned int)_tmp307 > 3?*((int*)_tmp307)== 5: 0){goto
_LL416;}else{goto _LL417;}_LL417: if((int)_tmp307 == 1){goto _LL418;}else{goto _LL419;}
_LL419: if((unsigned int)_tmp307 > 3?*((int*)_tmp307)== 6: 0){goto _LL41A;}else{goto
_LL41B;}_LL41B: if((unsigned int)_tmp307 > 3?*((int*)_tmp307)== 7: 0){goto _LL41C;}
else{goto _LL41D;}_LL41D: if((unsigned int)_tmp307 > 3?*((int*)_tmp307)== 8: 0){goto
_LL41E;}else{goto _LL41F;}_LL41F: if((unsigned int)_tmp307 > 3?*((int*)_tmp307)== 9:
0){goto _LL420;}else{goto _LL421;}_LL421: if((unsigned int)_tmp307 > 3?*((int*)
_tmp307)== 10: 0){goto _LL422;}else{goto _LL423;}_LL423: if((unsigned int)_tmp307 > 3?*((
int*)_tmp307)== 11: 0){goto _LL424;}else{goto _LL425;}_LL425: if((unsigned int)
_tmp307 > 3?*((int*)_tmp307)== 14: 0){goto _LL426;}else{goto _LL427;}_LL427: if((
unsigned int)_tmp307 > 3?*((int*)_tmp307)== 15: 0){goto _LL428;}else{goto _LL429;}
_LL429: if((unsigned int)_tmp307 > 3?*((int*)_tmp307)== 12: 0){goto _LL42A;}else{
goto _LL42B;}_LL42B: if((unsigned int)_tmp307 > 3?*((int*)_tmp307)== 16: 0){goto
_LL42C;}else{goto _LL42D;}_LL42D: if((unsigned int)_tmp307 > 3?*((int*)_tmp307)== 
17: 0){goto _LL42E;}else{goto _LL42F;}_LL42F: if((unsigned int)_tmp307 > 3?*((int*)
_tmp307)== 18: 0){goto _LL430;}else{goto _LL431;}_LL431: if((int)_tmp307 == 2){goto
_LL432;}else{goto _LL433;}_LL433: if((unsigned int)_tmp307 > 3?*((int*)_tmp307)== 
19: 0){goto _LL434;}else{goto _LL435;}_LL435: if((unsigned int)_tmp307 > 3?*((int*)
_tmp307)== 20: 0){goto _LL436;}else{goto _LL437;}_LL437: if((unsigned int)_tmp307 > 3?*((
int*)_tmp307)== 21: 0){goto _LL438;}else{goto _LL439;}_LL439: if((unsigned int)
_tmp307 > 3?*((int*)_tmp307)== 13: 0){goto _LL43A;}else{goto _LL408;}_LL40A: return 0;
_LL40C: return 1;_LL40E: return 2;_LL410: return 3;_LL412: return 4;_LL414: return 5;
_LL416: return 6;_LL418: return 7;_LL41A: return 8;_LL41C: return 9;_LL41E: return 10;
_LL420: return 11;_LL422: return 12;_LL424: return 13;_LL426: return 14;_LL428: return 15;
_LL42A: return 16;_LL42C: return 17;_LL42E: return 18;_LL430: return 19;_LL432: return 20;
_LL434: return 21;_LL436: return 22;_LL438: return 23;_LL43A: return 24;_LL408:;}int Cyc_Tcutil_shallow_typecmp_it(
void*t1,void*t2){return Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));}int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2){return 0;}{int _tmp308=Cyc_Tcutil_shallow_typecmp_it(t1,t2);if(
_tmp308 != 0){return _tmp308;}{struct _tuple8 _tmp30A=({struct _tuple8 _tmp309;_tmp309.f1=
t2;_tmp309.f2=t1;_tmp309;});void*_tmp30B;void*_tmp30C;void*_tmp30D;void*_tmp30E;
void*_tmp30F;struct Cyc_Absyn_Tvar*_tmp310;void*_tmp311;struct Cyc_Absyn_Tvar*
_tmp312;void*_tmp313;struct Cyc_List_List*_tmp314;struct _tuple1*_tmp315;void*
_tmp316;struct Cyc_List_List*_tmp317;struct _tuple1*_tmp318;void*_tmp319;struct
_tuple1*_tmp31A;void*_tmp31B;struct _tuple1*_tmp31C;void*_tmp31D;struct Cyc_List_List*
_tmp31E;void*_tmp31F;struct Cyc_List_List*_tmp320;void*_tmp321;struct Cyc_Absyn_Uniondecl**
_tmp322;struct Cyc_List_List*_tmp323;struct _tuple1*_tmp324;void*_tmp325;struct Cyc_Absyn_Uniondecl**
_tmp326;struct Cyc_List_List*_tmp327;struct _tuple1*_tmp328;void*_tmp329;struct Cyc_Absyn_TunionInfo
_tmp32A;void*_tmp32B;struct Cyc_List_List*_tmp32C;void*_tmp32D;struct Cyc_Absyn_Tuniondecl**
_tmp32E;struct Cyc_Absyn_Tuniondecl*_tmp32F;void*_tmp330;struct Cyc_Absyn_TunionInfo
_tmp331;void*_tmp332;struct Cyc_List_List*_tmp333;void*_tmp334;struct Cyc_Absyn_Tuniondecl**
_tmp335;struct Cyc_Absyn_Tuniondecl*_tmp336;void*_tmp337;struct Cyc_Absyn_TunionFieldInfo
_tmp338;struct Cyc_List_List*_tmp339;void*_tmp33A;struct Cyc_Absyn_Tunionfield*
_tmp33B;struct Cyc_Absyn_Tuniondecl*_tmp33C;void*_tmp33D;struct Cyc_Absyn_TunionFieldInfo
_tmp33E;struct Cyc_List_List*_tmp33F;void*_tmp340;struct Cyc_Absyn_Tunionfield*
_tmp341;struct Cyc_Absyn_Tuniondecl*_tmp342;void*_tmp343;struct Cyc_Absyn_PtrInfo
_tmp344;struct Cyc_Absyn_Conref*_tmp345;struct Cyc_Absyn_Tqual _tmp346;struct Cyc_Absyn_Conref*
_tmp347;void*_tmp348;void*_tmp349;void*_tmp34A;struct Cyc_Absyn_PtrInfo _tmp34B;
struct Cyc_Absyn_Conref*_tmp34C;struct Cyc_Absyn_Tqual _tmp34D;struct Cyc_Absyn_Conref*
_tmp34E;void*_tmp34F;void*_tmp350;void*_tmp351;void*_tmp352;void*_tmp353;void*
_tmp354;void*_tmp355;void*_tmp356;void*_tmp357;void*_tmp358;void*_tmp359;int
_tmp35A;void*_tmp35B;int _tmp35C;void*_tmp35D;struct Cyc_Absyn_Exp*_tmp35E;struct
Cyc_Absyn_Tqual _tmp35F;void*_tmp360;void*_tmp361;struct Cyc_Absyn_Exp*_tmp362;
struct Cyc_Absyn_Tqual _tmp363;void*_tmp364;void*_tmp365;struct Cyc_Absyn_FnInfo
_tmp366;struct Cyc_List_List*_tmp367;struct Cyc_List_List*_tmp368;struct Cyc_Absyn_VarargInfo*
_tmp369;int _tmp36A;struct Cyc_List_List*_tmp36B;void*_tmp36C;struct Cyc_Core_Opt*
_tmp36D;struct Cyc_List_List*_tmp36E;void*_tmp36F;struct Cyc_Absyn_FnInfo _tmp370;
struct Cyc_List_List*_tmp371;struct Cyc_List_List*_tmp372;struct Cyc_Absyn_VarargInfo*
_tmp373;int _tmp374;struct Cyc_List_List*_tmp375;void*_tmp376;struct Cyc_Core_Opt*
_tmp377;struct Cyc_List_List*_tmp378;void*_tmp379;struct Cyc_List_List*_tmp37A;
void*_tmp37B;struct Cyc_List_List*_tmp37C;void*_tmp37D;struct Cyc_List_List*
_tmp37E;void*_tmp37F;struct Cyc_List_List*_tmp380;void*_tmp381;struct Cyc_List_List*
_tmp382;void*_tmp383;struct Cyc_List_List*_tmp384;void*_tmp385;void*_tmp386;void*
_tmp387;void*_tmp388;void*_tmp389;void*_tmp38A;void*_tmp38B;void*_tmp38C;void*
_tmp38D;void*_tmp38E;void*_tmp38F;void*_tmp390;void*_tmp391;void*_tmp392;void*
_tmp393;void*_tmp394;_LL43C: _LL475: _tmp30C=_tmp30A.f1;if((unsigned int)_tmp30C > 
3?*((int*)_tmp30C)== 0: 0){goto _LL474;}else{goto _LL43E;}_LL474: _tmp30B=_tmp30A.f2;
if((unsigned int)_tmp30B > 3?*((int*)_tmp30B)== 0: 0){goto _LL43D;}else{goto _LL43E;}
_LL43E: _LL477: _tmp30E=_tmp30A.f1;if((int)_tmp30E == 0){goto _LL476;}else{goto
_LL440;}_LL476: _tmp30D=_tmp30A.f2;if((int)_tmp30D == 0){goto _LL43F;}else{goto
_LL440;}_LL440: _LL47A: _tmp311=_tmp30A.f1;if((unsigned int)_tmp311 > 3?*((int*)
_tmp311)== 1: 0){_LL47B: _tmp312=((struct Cyc_Absyn_VarType_struct*)_tmp311)->f1;
goto _LL478;}else{goto _LL442;}_LL478: _tmp30F=_tmp30A.f2;if((unsigned int)_tmp30F > 
3?*((int*)_tmp30F)== 1: 0){_LL479: _tmp310=((struct Cyc_Absyn_VarType_struct*)
_tmp30F)->f1;goto _LL441;}else{goto _LL442;}_LL442: _LL47F: _tmp316=_tmp30A.f1;if((
unsigned int)_tmp316 > 3?*((int*)_tmp316)== 10: 0){_LL481: _tmp318=((struct Cyc_Absyn_StructType_struct*)
_tmp316)->f1;goto _LL480;_LL480: _tmp317=((struct Cyc_Absyn_StructType_struct*)
_tmp316)->f2;goto _LL47C;}else{goto _LL444;}_LL47C: _tmp313=_tmp30A.f2;if((
unsigned int)_tmp313 > 3?*((int*)_tmp313)== 10: 0){_LL47E: _tmp315=((struct Cyc_Absyn_StructType_struct*)
_tmp313)->f1;goto _LL47D;_LL47D: _tmp314=((struct Cyc_Absyn_StructType_struct*)
_tmp313)->f2;goto _LL443;}else{goto _LL444;}_LL444: _LL484: _tmp31B=_tmp30A.f1;if((
unsigned int)_tmp31B > 3?*((int*)_tmp31B)== 12: 0){_LL485: _tmp31C=((struct Cyc_Absyn_EnumType_struct*)
_tmp31B)->f1;goto _LL482;}else{goto _LL446;}_LL482: _tmp319=_tmp30A.f2;if((
unsigned int)_tmp319 > 3?*((int*)_tmp319)== 12: 0){_LL483: _tmp31A=((struct Cyc_Absyn_EnumType_struct*)
_tmp319)->f1;goto _LL445;}else{goto _LL446;}_LL446: _LL488: _tmp31F=_tmp30A.f1;if((
unsigned int)_tmp31F > 3?*((int*)_tmp31F)== 16: 0){_LL489: _tmp320=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp31F)->f1;goto _LL486;}else{goto _LL448;}_LL486: _tmp31D=_tmp30A.f2;if((
unsigned int)_tmp31D > 3?*((int*)_tmp31D)== 16: 0){_LL487: _tmp31E=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp31D)->f1;goto _LL447;}else{goto _LL448;}_LL448: _LL48E: _tmp325=_tmp30A.f1;if((
unsigned int)_tmp325 > 3?*((int*)_tmp325)== 11: 0){_LL491: _tmp328=((struct Cyc_Absyn_UnionType_struct*)
_tmp325)->f1;goto _LL490;_LL490: _tmp327=((struct Cyc_Absyn_UnionType_struct*)
_tmp325)->f2;goto _LL48F;_LL48F: _tmp326=((struct Cyc_Absyn_UnionType_struct*)
_tmp325)->f3;goto _LL48A;}else{goto _LL44A;}_LL48A: _tmp321=_tmp30A.f2;if((
unsigned int)_tmp321 > 3?*((int*)_tmp321)== 11: 0){_LL48D: _tmp324=((struct Cyc_Absyn_UnionType_struct*)
_tmp321)->f1;goto _LL48C;_LL48C: _tmp323=((struct Cyc_Absyn_UnionType_struct*)
_tmp321)->f2;goto _LL48B;_LL48B: _tmp322=((struct Cyc_Absyn_UnionType_struct*)
_tmp321)->f3;goto _LL449;}else{goto _LL44A;}_LL44A: _LL498: _tmp330=_tmp30A.f1;if((
unsigned int)_tmp330 > 3?*((int*)_tmp330)== 2: 0){_LL499: _tmp331=((struct Cyc_Absyn_TunionType_struct*)
_tmp330)->f1;_LL49C: _tmp334=(void*)_tmp331.tunion_info;if(*((int*)_tmp334)== 1){
_LL49D: _tmp335=((struct Cyc_Absyn_KnownTunion_struct*)_tmp334)->f1;_tmp336=*
_tmp335;goto _LL49B;}else{goto _LL44C;}_LL49B: _tmp333=_tmp331.targs;goto _LL49A;
_LL49A: _tmp332=(void*)_tmp331.rgn;goto _LL492;}else{goto _LL44C;}_LL492: _tmp329=
_tmp30A.f2;if((unsigned int)_tmp329 > 3?*((int*)_tmp329)== 2: 0){_LL493: _tmp32A=((
struct Cyc_Absyn_TunionType_struct*)_tmp329)->f1;_LL496: _tmp32D=(void*)_tmp32A.tunion_info;
if(*((int*)_tmp32D)== 1){_LL497: _tmp32E=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp32D)->f1;_tmp32F=*_tmp32E;goto _LL495;}else{goto _LL44C;}_LL495: _tmp32C=
_tmp32A.targs;goto _LL494;_LL494: _tmp32B=(void*)_tmp32A.rgn;goto _LL44B;}else{goto
_LL44C;}_LL44C: _LL4A4: _tmp33D=_tmp30A.f1;if((unsigned int)_tmp33D > 3?*((int*)
_tmp33D)== 3: 0){_LL4A5: _tmp33E=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp33D)->f1;
_LL4A7: _tmp340=(void*)_tmp33E.field_info;if(*((int*)_tmp340)== 1){_LL4A9: _tmp342=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp340)->f1;goto _LL4A8;_LL4A8: _tmp341=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp340)->f2;goto _LL4A6;}else{goto
_LL44E;}_LL4A6: _tmp33F=_tmp33E.targs;goto _LL49E;}else{goto _LL44E;}_LL49E: _tmp337=
_tmp30A.f2;if((unsigned int)_tmp337 > 3?*((int*)_tmp337)== 3: 0){_LL49F: _tmp338=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp337)->f1;_LL4A1: _tmp33A=(void*)
_tmp338.field_info;if(*((int*)_tmp33A)== 1){_LL4A3: _tmp33C=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp33A)->f1;goto _LL4A2;_LL4A2: _tmp33B=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp33A)->f2;goto _LL4A0;}else{goto _LL44E;}_LL4A0: _tmp339=_tmp338.targs;goto
_LL44D;}else{goto _LL44E;}_LL44E: _LL4B1: _tmp34A=_tmp30A.f1;if((unsigned int)
_tmp34A > 3?*((int*)_tmp34A)== 4: 0){_LL4B2: _tmp34B=((struct Cyc_Absyn_PointerType_struct*)
_tmp34A)->f1;_LL4B7: _tmp350=(void*)_tmp34B.elt_typ;goto _LL4B6;_LL4B6: _tmp34F=(
void*)_tmp34B.rgn_typ;goto _LL4B5;_LL4B5: _tmp34E=_tmp34B.nullable;goto _LL4B4;
_LL4B4: _tmp34D=_tmp34B.tq;goto _LL4B3;_LL4B3: _tmp34C=_tmp34B.bounds;goto _LL4AA;}
else{goto _LL450;}_LL4AA: _tmp343=_tmp30A.f2;if((unsigned int)_tmp343 > 3?*((int*)
_tmp343)== 4: 0){_LL4AB: _tmp344=((struct Cyc_Absyn_PointerType_struct*)_tmp343)->f1;
_LL4B0: _tmp349=(void*)_tmp344.elt_typ;goto _LL4AF;_LL4AF: _tmp348=(void*)_tmp344.rgn_typ;
goto _LL4AE;_LL4AE: _tmp347=_tmp344.nullable;goto _LL4AD;_LL4AD: _tmp346=_tmp344.tq;
goto _LL4AC;_LL4AC: _tmp345=_tmp344.bounds;goto _LL44F;}else{goto _LL450;}_LL450:
_LL4BB: _tmp354=_tmp30A.f1;if((unsigned int)_tmp354 > 3?*((int*)_tmp354)== 5: 0){
_LL4BD: _tmp356=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp354)->f1;goto _LL4BC;
_LL4BC: _tmp355=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp354)->f2;goto _LL4B8;}
else{goto _LL452;}_LL4B8: _tmp351=_tmp30A.f2;if((unsigned int)_tmp351 > 3?*((int*)
_tmp351)== 5: 0){_LL4BA: _tmp353=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp351)->f1;
goto _LL4B9;_LL4B9: _tmp352=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp351)->f2;
goto _LL451;}else{goto _LL452;}_LL452: _LL4BF: _tmp358=_tmp30A.f1;if((int)_tmp358 == 
1){goto _LL4BE;}else{goto _LL454;}_LL4BE: _tmp357=_tmp30A.f2;if((int)_tmp357 == 1){
goto _LL453;}else{goto _LL454;}_LL454: _LL4C2: _tmp35B=_tmp30A.f1;if((unsigned int)
_tmp35B > 3?*((int*)_tmp35B)== 6: 0){_LL4C3: _tmp35C=((struct Cyc_Absyn_DoubleType_struct*)
_tmp35B)->f1;goto _LL4C0;}else{goto _LL456;}_LL4C0: _tmp359=_tmp30A.f2;if((
unsigned int)_tmp359 > 3?*((int*)_tmp359)== 6: 0){_LL4C1: _tmp35A=((struct Cyc_Absyn_DoubleType_struct*)
_tmp359)->f1;goto _LL455;}else{goto _LL456;}_LL456: _LL4C8: _tmp361=_tmp30A.f1;if((
unsigned int)_tmp361 > 3?*((int*)_tmp361)== 7: 0){_LL4CB: _tmp364=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp361)->f1;goto _LL4CA;_LL4CA: _tmp363=((struct Cyc_Absyn_ArrayType_struct*)
_tmp361)->f2;goto _LL4C9;_LL4C9: _tmp362=((struct Cyc_Absyn_ArrayType_struct*)
_tmp361)->f3;goto _LL4C4;}else{goto _LL458;}_LL4C4: _tmp35D=_tmp30A.f2;if((
unsigned int)_tmp35D > 3?*((int*)_tmp35D)== 7: 0){_LL4C7: _tmp360=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp35D)->f1;goto _LL4C6;_LL4C6: _tmp35F=((struct Cyc_Absyn_ArrayType_struct*)
_tmp35D)->f2;goto _LL4C5;_LL4C5: _tmp35E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp35D)->f3;goto _LL457;}else{goto _LL458;}_LL458: _LL4D6: _tmp36F=_tmp30A.f1;if((
unsigned int)_tmp36F > 3?*((int*)_tmp36F)== 8: 0){_LL4D7: _tmp370=((struct Cyc_Absyn_FnType_struct*)
_tmp36F)->f1;_LL4DF: _tmp378=_tmp370.tvars;goto _LL4DE;_LL4DE: _tmp377=_tmp370.effect;
goto _LL4DD;_LL4DD: _tmp376=(void*)_tmp370.ret_typ;goto _LL4DC;_LL4DC: _tmp375=
_tmp370.args;goto _LL4DB;_LL4DB: _tmp374=_tmp370.c_varargs;goto _LL4DA;_LL4DA:
_tmp373=_tmp370.cyc_varargs;goto _LL4D9;_LL4D9: _tmp372=_tmp370.rgn_po;goto _LL4D8;
_LL4D8: _tmp371=_tmp370.attributes;goto _LL4CC;}else{goto _LL45A;}_LL4CC: _tmp365=
_tmp30A.f2;if((unsigned int)_tmp365 > 3?*((int*)_tmp365)== 8: 0){_LL4CD: _tmp366=((
struct Cyc_Absyn_FnType_struct*)_tmp365)->f1;_LL4D5: _tmp36E=_tmp366.tvars;goto
_LL4D4;_LL4D4: _tmp36D=_tmp366.effect;goto _LL4D3;_LL4D3: _tmp36C=(void*)_tmp366.ret_typ;
goto _LL4D2;_LL4D2: _tmp36B=_tmp366.args;goto _LL4D1;_LL4D1: _tmp36A=_tmp366.c_varargs;
goto _LL4D0;_LL4D0: _tmp369=_tmp366.cyc_varargs;goto _LL4CF;_LL4CF: _tmp368=_tmp366.rgn_po;
goto _LL4CE;_LL4CE: _tmp367=_tmp366.attributes;goto _LL459;}else{goto _LL45A;}_LL45A:
_LL4E2: _tmp37B=_tmp30A.f1;if((unsigned int)_tmp37B > 3?*((int*)_tmp37B)== 9: 0){
_LL4E3: _tmp37C=((struct Cyc_Absyn_TupleType_struct*)_tmp37B)->f1;goto _LL4E0;}
else{goto _LL45C;}_LL4E0: _tmp379=_tmp30A.f2;if((unsigned int)_tmp379 > 3?*((int*)
_tmp379)== 9: 0){_LL4E1: _tmp37A=((struct Cyc_Absyn_TupleType_struct*)_tmp379)->f1;
goto _LL45B;}else{goto _LL45C;}_LL45C: _LL4E6: _tmp37F=_tmp30A.f1;if((unsigned int)
_tmp37F > 3?*((int*)_tmp37F)== 14: 0){_LL4E7: _tmp380=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp37F)->f1;goto _LL4E4;}else{goto _LL45E;}_LL4E4: _tmp37D=_tmp30A.f2;if((
unsigned int)_tmp37D > 3?*((int*)_tmp37D)== 14: 0){_LL4E5: _tmp37E=((struct Cyc_Absyn_AnonStructType_struct*)
_tmp37D)->f1;goto _LL45D;}else{goto _LL45E;}_LL45E: _LL4EA: _tmp383=_tmp30A.f1;if((
unsigned int)_tmp383 > 3?*((int*)_tmp383)== 15: 0){_LL4EB: _tmp384=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp383)->f1;goto _LL4E8;}else{goto _LL460;}_LL4E8: _tmp381=_tmp30A.f2;if((
unsigned int)_tmp381 > 3?*((int*)_tmp381)== 15: 0){_LL4E9: _tmp382=((struct Cyc_Absyn_AnonUnionType_struct*)
_tmp381)->f1;goto _LL45F;}else{goto _LL460;}_LL460: _LL4ED: _tmp386=_tmp30A.f1;if((
int)_tmp386 == 2){goto _LL4EC;}else{goto _LL462;}_LL4EC: _tmp385=_tmp30A.f2;if((int)
_tmp385 == 2){goto _LL461;}else{goto _LL462;}_LL462: _LL4F0: _tmp389=_tmp30A.f1;if((
unsigned int)_tmp389 > 3?*((int*)_tmp389)== 17: 0){_LL4F1: _tmp38A=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp389)->f1;goto _LL4EE;}else{goto _LL464;}_LL4EE:
_tmp387=_tmp30A.f2;if((unsigned int)_tmp387 > 3?*((int*)_tmp387)== 17: 0){_LL4EF:
_tmp388=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp387)->f1;goto _LL463;}
else{goto _LL464;}_LL464: _LL4F4: _tmp38D=_tmp30A.f1;if((unsigned int)_tmp38D > 3?*((
int*)_tmp38D)== 13: 0){_LL4F5: _tmp38E=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp38D)->f1;goto _LL4F2;}else{goto _LL466;}_LL4F2: _tmp38B=_tmp30A.f2;if((
unsigned int)_tmp38B > 3?*((int*)_tmp38B)== 13: 0){_LL4F3: _tmp38C=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp38B)->f1;goto _LL465;}else{goto _LL466;}_LL466:
_LL4F6: _tmp38F=_tmp30A.f1;if((unsigned int)_tmp38F > 3?*((int*)_tmp38F)== 20: 0){
goto _LL467;}else{goto _LL468;}_LL468: _LL4F7: _tmp390=_tmp30A.f2;if((unsigned int)
_tmp390 > 3?*((int*)_tmp390)== 20: 0){goto _LL469;}else{goto _LL46A;}_LL46A: _LL4F8:
_tmp391=_tmp30A.f1;if((unsigned int)_tmp391 > 3?*((int*)_tmp391)== 19: 0){goto
_LL46B;}else{goto _LL46C;}_LL46C: _LL4F9: _tmp392=_tmp30A.f1;if((unsigned int)
_tmp392 > 3?*((int*)_tmp392)== 21: 0){goto _LL46D;}else{goto _LL46E;}_LL46E: _LL4FA:
_tmp393=_tmp30A.f2;if((unsigned int)_tmp393 > 3?*((int*)_tmp393)== 21: 0){goto
_LL46F;}else{goto _LL470;}_LL470: _LL4FB: _tmp394=_tmp30A.f2;if((unsigned int)
_tmp394 > 3?*((int*)_tmp394)== 19: 0){goto _LL471;}else{goto _LL472;}_LL472: goto
_LL473;_LL43D:({void*_tmp395[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: can only compare closed types",
sizeof(unsigned char),39),_tag_arr(_tmp395,sizeof(void*),0));});return 0;_LL43F:
return 0;_LL441: {struct _tagged_arr*_tmp396=_tmp310->name;struct _tagged_arr*
_tmp397=_tmp312->name;int _tmp398=*((int*)_check_null(_tmp310->identity));int
_tmp399=*((int*)_check_null(_tmp312->identity));return Cyc_Core_intcmp(_tmp398,
_tmp399);}_LL443: {int _tmp39A=((int(*)(int(*cmp)(struct _tuple1*,struct _tuple1*),
struct _tuple1*a1,struct _tuple1*a2))Cyc_Tcutil_star_cmp)(Cyc_Absyn_qvar_cmp,
_tmp315,_tmp318);if(_tmp39A != 0){return _tmp39A;}else{return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp,_tmp314,_tmp317);}}_LL445: {int _tmp39B=Cyc_Absyn_qvar_cmp(
_tmp31C,_tmp31A);return _tmp39B;}_LL447: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,
struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(
Cyc_Tcutil_enumfield_cmp,_tmp320,_tmp31E);_LL449: {int _tmp39C=((int(*)(int(*cmp)(
struct _tuple1*,struct _tuple1*),struct _tuple1*a1,struct _tuple1*a2))Cyc_Tcutil_star_cmp)(
Cyc_Absyn_qvar_cmp,_tmp324,_tmp328);if(_tmp39C != 0){return _tmp39C;}else{return
Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,_tmp323,_tmp327);}}_LL44B: if(_tmp32F == 
_tmp336){return 0;}{int _tmp39D=Cyc_Absyn_qvar_cmp(_tmp32F->name,_tmp336->name);
if(_tmp39D != 0){return _tmp39D;}{int _tmp39E=Cyc_Tcutil_typecmp(_tmp32B,_tmp332);
if(_tmp39E != 0){return _tmp39E;}return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,
_tmp32C,_tmp333);}}_LL44D: if(_tmp33C == _tmp342){return 0;}{int _tmp39F=Cyc_Absyn_qvar_cmp(
_tmp342->name,_tmp33C->name);if(_tmp39F != 0){return _tmp39F;}{int _tmp3A0=Cyc_Absyn_qvar_cmp(
_tmp341->name,_tmp33B->name);if(_tmp3A0 != 0){return _tmp3A0;}return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp,_tmp339,_tmp33F);}}_LL44F: {int _tmp3A1=Cyc_Tcutil_typecmp(
_tmp349,_tmp350);if(_tmp3A1 != 0){return _tmp3A1;}{int _tmp3A2=Cyc_Tcutil_typecmp(
_tmp348,_tmp34F);if(_tmp3A2 != 0){return _tmp3A2;}{int _tmp3A3=Cyc_Tcutil_tqual_cmp(
_tmp346,_tmp34D);if(_tmp3A3 != 0){return _tmp3A3;}{int _tmp3A4=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp345,_tmp34C);if(_tmp3A4 != 0){return _tmp3A4;}{void*
_tmp3A5=(void*)(Cyc_Absyn_compress_conref(_tmp345))->v;void*_tmp3A6;_LL4FD: if((
unsigned int)_tmp3A5 > 1?*((int*)_tmp3A5)== 0: 0){_LL501: _tmp3A6=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp3A5)->f1;if((int)_tmp3A6 == 0){goto _LL4FE;}else{
goto _LL4FF;}}else{goto _LL4FF;}_LL4FF: goto _LL500;_LL4FE: return 0;_LL500: goto _LL4FC;
_LL4FC:;}return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp347,_tmp34E);}}}}_LL451: if(
_tmp353 != _tmp356){return Cyc_Core_intcmp((int)((unsigned int)_tmp353),(int)((
unsigned int)_tmp356));}if(_tmp352 != _tmp355){return Cyc_Core_intcmp((int)((
unsigned int)_tmp352),(int)((unsigned int)_tmp355));}return 0;_LL453: return 0;
_LL455: if(_tmp35C == _tmp35A){return 0;}else{if(_tmp35C){return - 1;}else{return 1;}}
_LL457: {int _tmp3A7=Cyc_Tcutil_tqual_cmp(_tmp35F,_tmp363);if(_tmp3A7 != 0){return
_tmp3A7;}{int _tmp3A8=Cyc_Tcutil_typecmp(_tmp360,_tmp364);if(_tmp3A8 != 0){return
_tmp3A8;}if(_tmp362 == _tmp35E){return 0;}if(_tmp362 == 0? 1: _tmp35E == 0){({void*
_tmp3A9[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("missing expression in array index",sizeof(unsigned char),34),_tag_arr(
_tmp3A9,sizeof(void*),0));});}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Tcutil_const_uint_exp_cmp,_tmp362,_tmp35E);}}_LL459:({void*_tmp3AA[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: function types not handled",
sizeof(unsigned char),36),_tag_arr(_tmp3AA,sizeof(void*),0));});goto _LL43B;
_LL45B: return((int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_tqual_type_cmp,
_tmp37A,_tmp37C);_LL45D: _tmp384=_tmp380;_tmp382=_tmp37E;goto _LL45F;_LL45F: return((
int(*)(int(*cmp)(struct Cyc_Absyn_Structfield*,struct Cyc_Absyn_Structfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_structfield_cmp,
_tmp382,_tmp384);_LL461: return 0;_LL463: return Cyc_Tcutil_typecmp(_tmp38A,_tmp388);
_LL465: return Cyc_Tcutil_typecmp(_tmp38E,_tmp38C);_LL467: goto _LL469;_LL469: goto
_LL46B;_LL46B: goto _LL46D;_LL46D: goto _LL46F;_LL46F: goto _LL471;_LL471:({void*
_tmp3AB[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("typecmp: effects not handled",sizeof(unsigned char),29),_tag_arr(
_tmp3AB,sizeof(void*),0));});goto _LL43B;_LL473: goto _LL43B;_LL43B:;}return({void*
_tmp3AC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("Unmatched case in typecmp",sizeof(unsigned char),26),_tag_arr(_tmp3AC,
sizeof(void*),0));});}}int Cyc_Tcutil_is_arithmetic_type(void*t){void*_tmp3AD=Cyc_Tcutil_compress(
t);_LL503: if((unsigned int)_tmp3AD > 3?*((int*)_tmp3AD)== 5: 0){goto _LL504;}else{
goto _LL505;}_LL505: if((int)_tmp3AD == 1){goto _LL506;}else{goto _LL507;}_LL507: if((
unsigned int)_tmp3AD > 3?*((int*)_tmp3AD)== 6: 0){goto _LL508;}else{goto _LL509;}
_LL509: if((unsigned int)_tmp3AD > 3?*((int*)_tmp3AD)== 12: 0){goto _LL50A;}else{
goto _LL50B;}_LL50B: if((unsigned int)_tmp3AD > 3?*((int*)_tmp3AD)== 16: 0){goto
_LL50C;}else{goto _LL50D;}_LL50D: goto _LL50E;_LL504: goto _LL506;_LL506: goto _LL508;
_LL508: goto _LL50A;_LL50A: goto _LL50C;_LL50C: return 1;_LL50E: return 0;_LL502:;}int
Cyc_Tcutil_will_lose_precision(void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple8 _tmp3AF=({struct _tuple8 _tmp3AE;_tmp3AE.f1=
t1;_tmp3AE.f2=t2;_tmp3AE;});void*_tmp3B0;int _tmp3B1;void*_tmp3B2;int _tmp3B3;void*
_tmp3B4;void*_tmp3B5;void*_tmp3B6;void*_tmp3B7;void*_tmp3B8;void*_tmp3B9;void*
_tmp3BA;void*_tmp3BB;void*_tmp3BC;void*_tmp3BD;void*_tmp3BE;void*_tmp3BF;void*
_tmp3C0;void*_tmp3C1;void*_tmp3C2;void*_tmp3C3;void*_tmp3C4;void*_tmp3C5;void*
_tmp3C6;void*_tmp3C7;void*_tmp3C8;void*_tmp3C9;void*_tmp3CA;void*_tmp3CB;void*
_tmp3CC;void*_tmp3CD;void*_tmp3CE;void*_tmp3CF;void*_tmp3D0;void*_tmp3D1;void*
_tmp3D2;void*_tmp3D3;void*_tmp3D4;void*_tmp3D5;void*_tmp3D6;void*_tmp3D7;void*
_tmp3D8;_LL510: _LL530: _tmp3B2=_tmp3AF.f1;if((unsigned int)_tmp3B2 > 3?*((int*)
_tmp3B2)== 6: 0){_LL531: _tmp3B3=((struct Cyc_Absyn_DoubleType_struct*)_tmp3B2)->f1;
goto _LL52E;}else{goto _LL512;}_LL52E: _tmp3B0=_tmp3AF.f2;if((unsigned int)_tmp3B0 > 
3?*((int*)_tmp3B0)== 6: 0){_LL52F: _tmp3B1=((struct Cyc_Absyn_DoubleType_struct*)
_tmp3B0)->f1;goto _LL511;}else{goto _LL512;}_LL512: _LL533: _tmp3B5=_tmp3AF.f1;if((
unsigned int)_tmp3B5 > 3?*((int*)_tmp3B5)== 6: 0){goto _LL532;}else{goto _LL514;}
_LL532: _tmp3B4=_tmp3AF.f2;if((int)_tmp3B4 == 1){goto _LL513;}else{goto _LL514;}
_LL514: _LL535: _tmp3B7=_tmp3AF.f1;if((unsigned int)_tmp3B7 > 3?*((int*)_tmp3B7)== 
6: 0){goto _LL534;}else{goto _LL516;}_LL534: _tmp3B6=_tmp3AF.f2;if((unsigned int)
_tmp3B6 > 3?*((int*)_tmp3B6)== 5: 0){goto _LL515;}else{goto _LL516;}_LL516: _LL537:
_tmp3B9=_tmp3AF.f1;if((unsigned int)_tmp3B9 > 3?*((int*)_tmp3B9)== 6: 0){goto
_LL536;}else{goto _LL518;}_LL536: _tmp3B8=_tmp3AF.f2;if((unsigned int)_tmp3B8 > 3?*((
int*)_tmp3B8)== 13: 0){goto _LL517;}else{goto _LL518;}_LL518: _LL539: _tmp3BB=_tmp3AF.f1;
if((int)_tmp3BB == 1){goto _LL538;}else{goto _LL51A;}_LL538: _tmp3BA=_tmp3AF.f2;if((
unsigned int)_tmp3BA > 3?*((int*)_tmp3BA)== 13: 0){goto _LL519;}else{goto _LL51A;}
_LL51A: _LL53B: _tmp3BD=_tmp3AF.f1;if((int)_tmp3BD == 1){goto _LL53A;}else{goto
_LL51C;}_LL53A: _tmp3BC=_tmp3AF.f2;if((unsigned int)_tmp3BC > 3?*((int*)_tmp3BC)== 
5: 0){goto _LL51B;}else{goto _LL51C;}_LL51C: _LL53E: _tmp3C0=_tmp3AF.f1;if((
unsigned int)_tmp3C0 > 3?*((int*)_tmp3C0)== 5: 0){_LL53F: _tmp3C1=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp3C0)->f2;if((int)_tmp3C1 == 3){goto _LL53C;}else{
goto _LL51E;}}else{goto _LL51E;}_LL53C: _tmp3BE=_tmp3AF.f2;if((unsigned int)_tmp3BE
> 3?*((int*)_tmp3BE)== 5: 0){_LL53D: _tmp3BF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3BE)->f2;if((int)_tmp3BF == 3){goto _LL51D;}else{goto _LL51E;}}else{goto _LL51E;}
_LL51E: _LL540: _tmp3C2=_tmp3AF.f1;if((unsigned int)_tmp3C2 > 3?*((int*)_tmp3C2)== 
5: 0){_LL541: _tmp3C3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3C2)->f2;if((
int)_tmp3C3 == 3){goto _LL51F;}else{goto _LL520;}}else{goto _LL520;}_LL520: _LL543:
_tmp3C5=_tmp3AF.f1;if((unsigned int)_tmp3C5 > 3?*((int*)_tmp3C5)== 5: 0){_LL544:
_tmp3C6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3C5)->f2;if((int)_tmp3C6 == 
2){goto _LL542;}else{goto _LL522;}}else{goto _LL522;}_LL542: _tmp3C4=_tmp3AF.f2;if((
int)_tmp3C4 == 1){goto _LL521;}else{goto _LL522;}_LL522: _LL547: _tmp3C9=_tmp3AF.f1;
if((unsigned int)_tmp3C9 > 3?*((int*)_tmp3C9)== 5: 0){_LL548: _tmp3CA=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3C9)->f2;if((int)_tmp3CA == 2){goto _LL545;}
else{goto _LL524;}}else{goto _LL524;}_LL545: _tmp3C7=_tmp3AF.f2;if((unsigned int)
_tmp3C7 > 3?*((int*)_tmp3C7)== 5: 0){_LL546: _tmp3C8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3C7)->f2;if((int)_tmp3C8 == 1){goto _LL523;}else{goto _LL524;}}else{goto _LL524;}
_LL524: _LL54B: _tmp3CD=_tmp3AF.f1;if((unsigned int)_tmp3CD > 3?*((int*)_tmp3CD)== 
5: 0){_LL54C: _tmp3CE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3CD)->f2;if((
int)_tmp3CE == 2){goto _LL549;}else{goto _LL526;}}else{goto _LL526;}_LL549: _tmp3CB=
_tmp3AF.f2;if((unsigned int)_tmp3CB > 3?*((int*)_tmp3CB)== 5: 0){_LL54A: _tmp3CC=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp3CB)->f2;if((int)_tmp3CC == 0){goto
_LL525;}else{goto _LL526;}}else{goto _LL526;}_LL526: _LL54F: _tmp3D1=_tmp3AF.f1;if((
unsigned int)_tmp3D1 > 3?*((int*)_tmp3D1)== 5: 0){_LL550: _tmp3D2=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp3D1)->f2;if((int)_tmp3D2 == 1){goto _LL54D;}else{
goto _LL528;}}else{goto _LL528;}_LL54D: _tmp3CF=_tmp3AF.f2;if((unsigned int)_tmp3CF
> 3?*((int*)_tmp3CF)== 5: 0){_LL54E: _tmp3D0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3CF)->f2;if((int)_tmp3D0 == 0){goto _LL527;}else{goto _LL528;}}else{goto _LL528;}
_LL528: _LL553: _tmp3D5=_tmp3AF.f1;if((unsigned int)_tmp3D5 > 3?*((int*)_tmp3D5)== 
13: 0){goto _LL551;}else{goto _LL52A;}_LL551: _tmp3D3=_tmp3AF.f2;if((unsigned int)
_tmp3D3 > 3?*((int*)_tmp3D3)== 5: 0){_LL552: _tmp3D4=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3D3)->f2;if((int)_tmp3D4 == 1){goto _LL529;}else{goto _LL52A;}}else{goto _LL52A;}
_LL52A: _LL556: _tmp3D8=_tmp3AF.f1;if((unsigned int)_tmp3D8 > 3?*((int*)_tmp3D8)== 
13: 0){goto _LL554;}else{goto _LL52C;}_LL554: _tmp3D6=_tmp3AF.f2;if((unsigned int)
_tmp3D6 > 3?*((int*)_tmp3D6)== 5: 0){_LL555: _tmp3D7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3D6)->f2;if((int)_tmp3D7 == 0){goto _LL52B;}else{goto _LL52C;}}else{goto _LL52C;}
_LL52C: goto _LL52D;_LL511: return ! _tmp3B1? _tmp3B3: 0;_LL513: goto _LL515;_LL515: goto
_LL517;_LL517: goto _LL519;_LL519: goto _LL51B;_LL51B: return 1;_LL51D: return 0;_LL51F:
goto _LL521;_LL521: goto _LL523;_LL523: goto _LL525;_LL525: goto _LL527;_LL527: goto
_LL529;_LL529: goto _LL52B;_LL52B: return 1;_LL52D: return 0;_LL50F:;}}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct Cyc_Core_Opt*
max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);
if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,(void*)max_arith_type->v)){max_arith_type=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*
_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9->v=(void*)t1;_tmp3D9;});}}}}if(
max_arith_type != 0){if(! Cyc_Tcutil_unify(t,(void*)((struct Cyc_Core_Opt*)
_check_null(max_arith_type))->v)){return 0;}}{struct Cyc_List_List*el=es;for(0;el
!= 0;el=el->tl){if(! Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){({
struct Cyc_Std_String_pa_struct _tmp3DC;_tmp3DC.tag=0;_tmp3DC.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);{struct Cyc_Std_String_pa_struct _tmp3DB;
_tmp3DB.tag=0;_tmp3DB.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmp3DA[2]={& _tmp3DB,& _tmp3DC};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,
_tag_arr("type mismatch: expecting %s but found %s",sizeof(unsigned char),41),
_tag_arr(_tmp3DA,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(! Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp3DD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL558: if((unsigned int)_tmp3DD > 3?*((int*)_tmp3DD)== 4: 0){goto _LL559;}else{goto
_LL55A;}_LL55A: goto _LL55B;_LL559: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_t);
goto _LL557;_LL55B: return 0;_LL557:;}return 1;}int Cyc_Tcutil_is_integral_type(void*
t){void*_tmp3DE=Cyc_Tcutil_compress(t);_LL55D: if((unsigned int)_tmp3DE > 3?*((int*)
_tmp3DE)== 5: 0){goto _LL55E;}else{goto _LL55F;}_LL55F: if((unsigned int)_tmp3DE > 3?*((
int*)_tmp3DE)== 13: 0){goto _LL560;}else{goto _LL561;}_LL561: if((unsigned int)
_tmp3DE > 3?*((int*)_tmp3DE)== 12: 0){goto _LL562;}else{goto _LL563;}_LL563: if((
unsigned int)_tmp3DE > 3?*((int*)_tmp3DE)== 16: 0){goto _LL564;}else{goto _LL565;}
_LL565: goto _LL566;_LL55E: goto _LL560;_LL560: goto _LL562;_LL562: goto _LL564;_LL564:
return 1;_LL566: return 0;_LL55C:;}int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_t)){return 1;}if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_t)){({void*
_tmp3DF[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(unsigned char),44),_tag_arr(_tmp3DF,sizeof(void*),0));});}Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_t);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_t)){return 1;}if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_t)){({void*
_tmp3E0[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(unsigned char),44),_tag_arr(_tmp3E0,sizeof(void*),0));});}Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_t);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple8 _tmp3E2=({struct _tuple8 _tmp3E1;_tmp3E1.f1=
t1;_tmp3E1.f2=t2;_tmp3E1;});void*_tmp3E3;struct Cyc_Absyn_PtrInfo _tmp3E4;void*
_tmp3E5;struct Cyc_Absyn_PtrInfo _tmp3E6;void*_tmp3E7;struct Cyc_Absyn_Exp*_tmp3E8;
struct Cyc_Absyn_Tqual _tmp3E9;void*_tmp3EA;void*_tmp3EB;struct Cyc_Absyn_Exp*
_tmp3EC;struct Cyc_Absyn_Tqual _tmp3ED;void*_tmp3EE;void*_tmp3EF;struct Cyc_Absyn_TunionInfo
_tmp3F0;struct Cyc_List_List*_tmp3F1;void*_tmp3F2;struct Cyc_Absyn_Tuniondecl**
_tmp3F3;struct Cyc_Absyn_Tuniondecl*_tmp3F4;void*_tmp3F5;struct Cyc_Absyn_TunionFieldInfo
_tmp3F6;struct Cyc_List_List*_tmp3F7;void*_tmp3F8;struct Cyc_Absyn_Tunionfield*
_tmp3F9;struct Cyc_Absyn_Tuniondecl*_tmp3FA;void*_tmp3FB;struct Cyc_Absyn_TunionInfo
_tmp3FC;void*_tmp3FD;struct Cyc_List_List*_tmp3FE;void*_tmp3FF;struct Cyc_Absyn_Tuniondecl**
_tmp400;struct Cyc_Absyn_Tuniondecl*_tmp401;void*_tmp402;struct Cyc_Absyn_PtrInfo
_tmp403;struct Cyc_Absyn_Conref*_tmp404;struct Cyc_Absyn_Tqual _tmp405;struct Cyc_Absyn_Conref*
_tmp406;void*_tmp407;void*_tmp408;void*_tmp409;void*_tmp40A;void*_tmp40B;_LL568:
_LL576: _tmp3E5=_tmp3E2.f1;if((unsigned int)_tmp3E5 > 3?*((int*)_tmp3E5)== 4: 0){
_LL577: _tmp3E6=((struct Cyc_Absyn_PointerType_struct*)_tmp3E5)->f1;goto _LL574;}
else{goto _LL56A;}_LL574: _tmp3E3=_tmp3E2.f2;if((unsigned int)_tmp3E3 > 3?*((int*)
_tmp3E3)== 4: 0){_LL575: _tmp3E4=((struct Cyc_Absyn_PointerType_struct*)_tmp3E3)->f1;
goto _LL569;}else{goto _LL56A;}_LL56A: _LL57C: _tmp3EB=_tmp3E2.f1;if((unsigned int)
_tmp3EB > 3?*((int*)_tmp3EB)== 7: 0){_LL57F: _tmp3EE=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp3EB)->f1;goto _LL57E;_LL57E: _tmp3ED=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3EB)->f2;goto _LL57D;_LL57D: _tmp3EC=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3EB)->f3;goto _LL578;}else{goto _LL56C;}_LL578: _tmp3E7=_tmp3E2.f2;if((
unsigned int)_tmp3E7 > 3?*((int*)_tmp3E7)== 7: 0){_LL57B: _tmp3EA=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp3E7)->f1;goto _LL57A;_LL57A: _tmp3E9=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3E7)->f2;goto _LL579;_LL579: _tmp3E8=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3E7)->f3;goto _LL56B;}else{goto _LL56C;}_LL56C: _LL585: _tmp3F5=_tmp3E2.f1;if((
unsigned int)_tmp3F5 > 3?*((int*)_tmp3F5)== 3: 0){_LL586: _tmp3F6=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3F5)->f1;_LL588: _tmp3F8=(void*)_tmp3F6.field_info;if(*((int*)_tmp3F8)== 1){
_LL58A: _tmp3FA=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3F8)->f1;goto
_LL589;_LL589: _tmp3F9=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3F8)->f2;
goto _LL587;}else{goto _LL56E;}_LL587: _tmp3F7=_tmp3F6.targs;goto _LL580;}else{goto
_LL56E;}_LL580: _tmp3EF=_tmp3E2.f2;if((unsigned int)_tmp3EF > 3?*((int*)_tmp3EF)== 
2: 0){_LL581: _tmp3F0=((struct Cyc_Absyn_TunionType_struct*)_tmp3EF)->f1;_LL583:
_tmp3F2=(void*)_tmp3F0.tunion_info;if(*((int*)_tmp3F2)== 1){_LL584: _tmp3F3=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp3F2)->f1;_tmp3F4=*_tmp3F3;goto _LL582;}
else{goto _LL56E;}_LL582: _tmp3F1=_tmp3F0.targs;goto _LL56D;}else{goto _LL56E;}
_LL56E: _LL591: _tmp402=_tmp3E2.f1;if((unsigned int)_tmp402 > 3?*((int*)_tmp402)== 
4: 0){_LL592: _tmp403=((struct Cyc_Absyn_PointerType_struct*)_tmp402)->f1;_LL597:
_tmp408=(void*)_tmp403.elt_typ;goto _LL596;_LL596: _tmp407=(void*)_tmp403.rgn_typ;
goto _LL595;_LL595: _tmp406=_tmp403.nullable;goto _LL594;_LL594: _tmp405=_tmp403.tq;
goto _LL593;_LL593: _tmp404=_tmp403.bounds;goto _LL58B;}else{goto _LL570;}_LL58B:
_tmp3FB=_tmp3E2.f2;if((unsigned int)_tmp3FB > 3?*((int*)_tmp3FB)== 2: 0){_LL58C:
_tmp3FC=((struct Cyc_Absyn_TunionType_struct*)_tmp3FB)->f1;_LL58F: _tmp3FF=(void*)
_tmp3FC.tunion_info;if(*((int*)_tmp3FF)== 1){_LL590: _tmp400=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3FF)->f1;_tmp401=*_tmp400;goto _LL58E;}else{goto _LL570;}_LL58E: _tmp3FE=
_tmp3FC.targs;goto _LL58D;_LL58D: _tmp3FD=(void*)_tmp3FC.rgn;goto _LL56F;}else{goto
_LL570;}_LL570: _LL59A: _tmp40B=_tmp3E2.f1;if((unsigned int)_tmp40B > 3?*((int*)
_tmp40B)== 13: 0){goto _LL598;}else{goto _LL572;}_LL598: _tmp409=_tmp3E2.f2;if((
unsigned int)_tmp409 > 3?*((int*)_tmp409)== 5: 0){_LL599: _tmp40A=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp409)->f2;if((int)_tmp40A == 2){goto _LL571;}else{
goto _LL572;}}else{goto _LL572;}_LL572: goto _LL573;_LL569: {int okay=1;if(!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp3E6.nullable,_tmp3E4.nullable)){void*_tmp40C=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp3E6.nullable))->v;int _tmp40D;_LL59C: if((unsigned int)_tmp40C > 1?*((int*)
_tmp40C)== 0: 0){_LL5A0: _tmp40D=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp40C)->f1;
goto _LL59D;}else{goto _LL59E;}_LL59E: goto _LL59F;_LL59D: okay=! _tmp40D;goto _LL59B;
_LL59F:(int)_throw(({void*_tmp40E[0]={};Cyc_Tcutil_impos(_tag_arr("silent_castable conref not eq",
sizeof(unsigned char),30),_tag_arr(_tmp40E,sizeof(void*),0));}));_LL59B:;}if(!
Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp3E6.bounds,_tmp3E4.bounds)){
struct _tuple8 _tmp410=({struct _tuple8 _tmp40F;_tmp40F.f1=(void*)(Cyc_Absyn_compress_conref(
_tmp3E6.bounds))->v;_tmp40F.f2=(void*)(Cyc_Absyn_compress_conref(_tmp3E4.bounds))->v;
_tmp40F;});void*_tmp411;void*_tmp412;void*_tmp413;void*_tmp414;void*_tmp415;void*
_tmp416;struct Cyc_Absyn_Exp*_tmp417;void*_tmp418;void*_tmp419;struct Cyc_Absyn_Exp*
_tmp41A;void*_tmp41B;void*_tmp41C;void*_tmp41D;void*_tmp41E;_LL5A2: _LL5AC:
_tmp413=_tmp410.f1;if((unsigned int)_tmp413 > 1?*((int*)_tmp413)== 0: 0){_LL5AD:
_tmp414=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp413)->f1;if((unsigned int)
_tmp414 > 1?*((int*)_tmp414)== 0: 0){goto _LL5AA;}else{goto _LL5A4;}}else{goto _LL5A4;}
_LL5AA: _tmp411=_tmp410.f2;if((unsigned int)_tmp411 > 1?*((int*)_tmp411)== 0: 0){
_LL5AB: _tmp412=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp411)->f1;if((int)
_tmp412 == 0){goto _LL5A3;}else{goto _LL5A4;}}else{goto _LL5A4;}_LL5A4: _LL5B1:
_tmp418=_tmp410.f1;if((unsigned int)_tmp418 > 1?*((int*)_tmp418)== 0: 0){_LL5B2:
_tmp419=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp418)->f1;if((unsigned int)
_tmp419 > 1?*((int*)_tmp419)== 0: 0){_LL5B3: _tmp41A=((struct Cyc_Absyn_Upper_b_struct*)
_tmp419)->f1;goto _LL5AE;}else{goto _LL5A6;}}else{goto _LL5A6;}_LL5AE: _tmp415=
_tmp410.f2;if((unsigned int)_tmp415 > 1?*((int*)_tmp415)== 0: 0){_LL5AF: _tmp416=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp415)->f1;if((unsigned int)_tmp416 > 
1?*((int*)_tmp416)== 0: 0){_LL5B0: _tmp417=((struct Cyc_Absyn_Upper_b_struct*)
_tmp416)->f1;goto _LL5A5;}else{goto _LL5A6;}}else{goto _LL5A6;}_LL5A6: _LL5B6:
_tmp41D=_tmp410.f1;if((unsigned int)_tmp41D > 1?*((int*)_tmp41D)== 0: 0){_LL5B7:
_tmp41E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp41D)->f1;if((int)_tmp41E
== 0){goto _LL5B4;}else{goto _LL5A8;}}else{goto _LL5A8;}_LL5B4: _tmp41B=_tmp410.f2;
if((unsigned int)_tmp41B > 1?*((int*)_tmp41B)== 0: 0){_LL5B5: _tmp41C=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp41B)->f1;if((int)_tmp41C == 0){goto _LL5A7;}
else{goto _LL5A8;}}else{goto _LL5A8;}_LL5A8: goto _LL5A9;_LL5A3: okay=1;goto _LL5A1;
_LL5A5: okay=okay? Cyc_Evexp_eval_const_uint_exp(_tmp41A)>= Cyc_Evexp_eval_const_uint_exp(
_tmp417): 0;({void*_tmp41F[0]={};Cyc_Tcutil_warn(loc,_tag_arr("implicit cast to shorter array",
sizeof(unsigned char),31),_tag_arr(_tmp41F,sizeof(void*),0));});goto _LL5A1;
_LL5A7: okay=1;goto _LL5A1;_LL5A9: okay=0;goto _LL5A1;_LL5A1:;}okay=okay? Cyc_Tcutil_unify((
void*)_tmp3E6.elt_typ,(void*)_tmp3E4.elt_typ): 0;okay=okay? Cyc_Tcutil_unify((void*)
_tmp3E6.rgn_typ,(void*)_tmp3E4.rgn_typ)? 1: Cyc_Tcenv_region_outlives(te,(void*)
_tmp3E6.rgn_typ,(void*)_tmp3E4.rgn_typ): 0;okay=okay? !(_tmp3E6.tq).q_const? 1:(
_tmp3E4.tq).q_const: 0;return okay;}_LL56B: {int okay;okay=(_tmp3EC != 0? _tmp3E8 != 0:
0)? Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp3EC))== 
Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp3E8)): 0;
return(okay? Cyc_Tcutil_unify(_tmp3EE,_tmp3EA): 0)? ! _tmp3ED.q_const? 1: _tmp3E9.q_const:
0;}_LL56D: if((_tmp3FA == _tmp3F4? 1: Cyc_Absyn_qvar_cmp(_tmp3FA->name,_tmp3F4->name)
== 0)? _tmp3F9->typs == 0: 0){for(0;_tmp3F7 != 0? _tmp3F1 != 0: 0;(_tmp3F7=_tmp3F7->tl,
_tmp3F1=_tmp3F1->tl)){if(! Cyc_Tcutil_unify((void*)_tmp3F7->hd,(void*)_tmp3F1->hd)){
break;}}if(_tmp3F7 == 0? _tmp3F1 == 0: 0){return 1;}}return 0;_LL56F:{void*_tmp420=Cyc_Tcutil_compress(
_tmp408);struct Cyc_Absyn_TunionFieldInfo _tmp421;struct Cyc_List_List*_tmp422;void*
_tmp423;struct Cyc_Absyn_Tunionfield*_tmp424;struct Cyc_Absyn_Tuniondecl*_tmp425;
_LL5B9: if((unsigned int)_tmp420 > 3?*((int*)_tmp420)== 3: 0){_LL5BD: _tmp421=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp420)->f1;_LL5BF: _tmp423=(void*)
_tmp421.field_info;if(*((int*)_tmp423)== 1){_LL5C1: _tmp425=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp423)->f1;goto _LL5C0;_LL5C0: _tmp424=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp423)->f2;goto _LL5BE;}else{goto _LL5BB;}_LL5BE: _tmp422=_tmp421.targs;goto
_LL5BA;}else{goto _LL5BB;}_LL5BB: goto _LL5BC;_LL5BA: if(! Cyc_Tcutil_unify(_tmp407,
_tmp3FD)? ! Cyc_Tcenv_region_outlives(te,_tmp407,_tmp3FD): 0){return 0;}if(!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp406,((struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(
0))){return 0;}if(! Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp404,Cyc_Absyn_new_conref(
Cyc_Absyn_bounds_one))){return 0;}if(Cyc_Absyn_qvar_cmp(_tmp401->name,_tmp425->name)
== 0? _tmp424->typs != 0: 0){int okay=1;for(0;_tmp422 != 0? _tmp3FE != 0: 0;(_tmp422=
_tmp422->tl,_tmp3FE=_tmp3FE->tl)){if(! Cyc_Tcutil_unify((void*)_tmp422->hd,(void*)
_tmp3FE->hd)){okay=0;break;}}if((! okay? 1: _tmp422 != 0)? 1: _tmp3FE != 0){return 0;}
return 1;}goto _LL5B8;_LL5BC: goto _LL5B8;_LL5B8:;}return 0;_LL571: return 1;_LL573:
return Cyc_Tcutil_unify(t1,t2);_LL567:;}}int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp426=Cyc_Tcutil_compress(t);_LL5C3: if((unsigned int)_tmp426 > 3?*((int*)
_tmp426)== 4: 0){goto _LL5C4;}else{goto _LL5C5;}_LL5C5: goto _LL5C6;_LL5C4: return 1;
_LL5C6: return 0;_LL5C2:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmp427=(void*)e->r;void*_tmp428;int _tmp429;_LL5C8: if(*((int*)_tmp427)== 0){
_LL5CC: _tmp428=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp427)->f1;if((
unsigned int)_tmp428 > 1?*((int*)_tmp428)== 2: 0){_LL5CD: _tmp429=((struct Cyc_Absyn_Int_c_struct*)
_tmp428)->f2;if(_tmp429 == 0){goto _LL5C9;}else{goto _LL5CA;}}else{goto _LL5CA;}}
else{goto _LL5CA;}_LL5CA: goto _LL5CB;_LL5C9: return 1;_LL5CB: return 0;_LL5C7:;}struct
Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(
void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt
Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*
te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(t2)? Cyc_Tcutil_is_zero(
e1): 0){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp42A=
_cycalloc(sizeof(*_tmp42A));_tmp42A[0]=({struct Cyc_Absyn_Const_e_struct _tmp42B;
_tmp42B.tag=0;_tmp42B.f1=(void*)((void*)0);_tmp42B;});_tmp42A;})));{struct Cyc_List_List*
_tmp42C=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp42D=({struct Cyc_Absyn_PointerType_struct*_tmp42E=_cycalloc(sizeof(*_tmp42E));
_tmp42E[0]=({struct Cyc_Absyn_PointerType_struct _tmp42F;_tmp42F.tag=4;_tmp42F.f1=({
struct Cyc_Absyn_PtrInfo _tmp430;_tmp430.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp432=_cycalloc(sizeof(*
_tmp432));_tmp432->v=_tmp42C;_tmp432;}));_tmp430.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp431=_cycalloc(
sizeof(*_tmp431));_tmp431->v=_tmp42C;_tmp431;}));_tmp430.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(1);_tmp430.tq=Cyc_Absyn_empty_tqual();_tmp430.bounds=
Cyc_Absyn_empty_conref();_tmp430;});_tmp42F;});_tmp42E;});(void*)(((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v=(void*)((void*)_tmp42D));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}static int Cyc_Tcutil_is_sizeof_type(void*t1){void*_tmp433=
Cyc_Tcutil_compress(t1);_LL5CF: if((unsigned int)_tmp433 > 3?*((int*)_tmp433)== 13:
0){goto _LL5D0;}else{goto _LL5D1;}_LL5D1: goto _LL5D2;_LL5D0: return 1;_LL5D2: return 0;
_LL5CE:;}static struct _tagged_arr Cyc_Tcutil_b2s(int b){return b? _tag_arr("true",
sizeof(unsigned char),5): _tag_arr("false",sizeof(unsigned char),6);}int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);if(Cyc_Tcutil_unify(t1,t2)){
return 1;}if((Cyc_Tcutil_is_arithmetic_type(t1)? Cyc_Tcutil_is_arithmetic_type(t2):
0)? 1:(Cyc_Tcutil_is_sizeof_type(t1)? Cyc_Tcutil_is_arithmetic_type(t2): 0)){if(Cyc_Tcutil_will_lose_precision(
t1,t2)){({struct Cyc_Std_String_pa_struct _tmp436;_tmp436.tag=0;_tmp436.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t2);{struct Cyc_Std_String_pa_struct _tmp435;
_tmp435.tag=0;_tmp435.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t1);{void*
_tmp434[2]={& _tmp435,& _tmp436};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; %s -> %s conversion supplied",
sizeof(unsigned char),53),_tag_arr(_tmp434,sizeof(void*),2));}}});}Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e)){return 1;}else{if(Cyc_Tcutil_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2);({struct Cyc_Std_String_pa_struct
_tmp439;_tmp439.tag=0;_tmp439.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp438;_tmp438.tag=0;_tmp438.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp437[2]={& _tmp438,& _tmp439};Cyc_Tcutil_warn(
e->loc,_tag_arr("implicit cast from %s to %s",sizeof(unsigned char),28),_tag_arr(
_tmp437,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp43A=Cyc_Tcutil_compress(t);
_LL5D4: if((unsigned int)_tmp43A > 3?*((int*)_tmp43A)== 5: 0){goto _LL5D5;}else{goto
_LL5D6;}_LL5D6: if((int)_tmp43A == 1){goto _LL5D7;}else{goto _LL5D8;}_LL5D8: if((
unsigned int)_tmp43A > 3?*((int*)_tmp43A)== 6: 0){goto _LL5D9;}else{goto _LL5DA;}
_LL5DA: goto _LL5DB;_LL5D5: goto _LL5D7;_LL5D7: goto _LL5D9;_LL5D9: return 1;_LL5DB:
return 0;_LL5D3:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*
inst,struct Cyc_Absyn_Structfield*x){return({struct _tuple4*_tmp43B=_cycalloc(
sizeof(*_tmp43B));_tmp43B->f1=x->tq;_tmp43B->f2=Cyc_Tcutil_substitute(inst,(void*)
x->type);_tmp43B;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp43C=t1;struct Cyc_List_List*
_tmp43D;struct Cyc_Absyn_Structdecl**_tmp43E;struct Cyc_List_List*_tmp43F;struct
_tuple1*_tmp440;struct Cyc_List_List*_tmp441;_LL5DD: if((int)_tmp43C == 0){goto
_LL5DE;}else{goto _LL5DF;}_LL5DF: if((unsigned int)_tmp43C > 3?*((int*)_tmp43C)== 9:
0){_LL5E7: _tmp43D=((struct Cyc_Absyn_TupleType_struct*)_tmp43C)->f1;goto _LL5E0;}
else{goto _LL5E1;}_LL5E1: if((unsigned int)_tmp43C > 3?*((int*)_tmp43C)== 10: 0){
_LL5EA: _tmp440=((struct Cyc_Absyn_StructType_struct*)_tmp43C)->f1;goto _LL5E9;
_LL5E9: _tmp43F=((struct Cyc_Absyn_StructType_struct*)_tmp43C)->f2;goto _LL5E8;
_LL5E8: _tmp43E=((struct Cyc_Absyn_StructType_struct*)_tmp43C)->f3;goto _LL5E2;}
else{goto _LL5E3;}_LL5E3: if((unsigned int)_tmp43C > 3?*((int*)_tmp43C)== 14: 0){
_LL5EB: _tmp441=((struct Cyc_Absyn_AnonStructType_struct*)_tmp43C)->f1;goto _LL5E4;}
else{goto _LL5E5;}_LL5E5: goto _LL5E6;_LL5DE: return 0;_LL5E0: return _tmp43D;_LL5E2:
if(_tmp43E == 0? 1: _tmp440 == 0){return({struct Cyc_List_List*_tmp442=_cycalloc(
sizeof(*_tmp442));_tmp442->hd=({struct _tuple4*_tmp443=_cycalloc(sizeof(*_tmp443));
_tmp443->f1=Cyc_Absyn_empty_tqual();_tmp443->f2=t1;_tmp443;});_tmp442->tl=0;
_tmp442;});}{struct Cyc_Absyn_Structdecl*_tmp444=*_tmp43E;struct _tuple1 _tmp445=*
_tmp440;struct Cyc_List_List*_tmp446=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp444->tvs,_tmp43F);if(_tmp444->fields
== 0){return({struct Cyc_List_List*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->hd=({
struct _tuple4*_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->f1=Cyc_Absyn_empty_tqual();
_tmp448->f2=t1;_tmp448;});_tmp447->tl=0;_tmp447;});}return((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Structfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp446,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp444->fields))->v);}
_LL5E4: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Structfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp441);_LL5E6: return({struct Cyc_List_List*_tmp449=
_cycalloc(sizeof(*_tmp449));_tmp449->hd=({struct _tuple4*_tmp44A=_cycalloc(
sizeof(*_tmp44A));_tmp44A->f1=Cyc_Absyn_empty_tqual();_tmp44A->f2=t1;_tmp44A;});
_tmp449->tl=0;_tmp449;});_LL5DC:;}}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2)){return 1;}{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple8*)a->hd)).f1)? Cyc_Tcutil_unify(t2,(*((struct _tuple8*)a->hd)).f2):
0){return 1;}}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple8 _tmp44C=({struct _tuple8 _tmp44B;_tmp44B.f1=t1;_tmp44B.f2=t2;_tmp44B;});
void*_tmp44D;struct Cyc_Absyn_PtrInfo _tmp44E;struct Cyc_Absyn_Conref*_tmp44F;
struct Cyc_Absyn_Tqual _tmp450;struct Cyc_Absyn_Conref*_tmp451;void*_tmp452;void*
_tmp453;void*_tmp454;struct Cyc_Absyn_PtrInfo _tmp455;struct Cyc_Absyn_Conref*
_tmp456;struct Cyc_Absyn_Tqual _tmp457;struct Cyc_Absyn_Conref*_tmp458;void*_tmp459;
void*_tmp45A;_LL5ED: _LL5F8: _tmp454=_tmp44C.f1;if((unsigned int)_tmp454 > 3?*((int*)
_tmp454)== 4: 0){_LL5F9: _tmp455=((struct Cyc_Absyn_PointerType_struct*)_tmp454)->f1;
_LL5FE: _tmp45A=(void*)_tmp455.elt_typ;goto _LL5FD;_LL5FD: _tmp459=(void*)_tmp455.rgn_typ;
goto _LL5FC;_LL5FC: _tmp458=_tmp455.nullable;goto _LL5FB;_LL5FB: _tmp457=_tmp455.tq;
goto _LL5FA;_LL5FA: _tmp456=_tmp455.bounds;goto _LL5F1;}else{goto _LL5EF;}_LL5F1:
_tmp44D=_tmp44C.f2;if((unsigned int)_tmp44D > 3?*((int*)_tmp44D)== 4: 0){_LL5F2:
_tmp44E=((struct Cyc_Absyn_PointerType_struct*)_tmp44D)->f1;_LL5F7: _tmp453=(void*)
_tmp44E.elt_typ;goto _LL5F6;_LL5F6: _tmp452=(void*)_tmp44E.rgn_typ;goto _LL5F5;
_LL5F5: _tmp451=_tmp44E.nullable;goto _LL5F4;_LL5F4: _tmp450=_tmp44E.tq;goto _LL5F3;
_LL5F3: _tmp44F=_tmp44E.bounds;goto _LL5EE;}else{goto _LL5EF;}_LL5EF: goto _LL5F0;
_LL5EE: if(_tmp457.q_const? ! _tmp450.q_const: 0){return 0;}if((!((int(*)(int(*cmp)(
int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp458,_tmp451)?((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp458): 0)? !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp451): 0){
return 0;}if(! Cyc_Tcutil_unify(_tmp459,_tmp452)? ! Cyc_Tcenv_region_outlives(te,
_tmp459,_tmp452): 0){return 0;}if(! Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp456,_tmp44F)){struct _tuple8 _tmp45C=({struct _tuple8 _tmp45B;_tmp45B.f1=Cyc_Absyn_conref_val(
_tmp456);_tmp45B.f2=Cyc_Absyn_conref_val(_tmp44F);_tmp45B;});void*_tmp45D;void*
_tmp45E;void*_tmp45F;struct Cyc_Absyn_Exp*_tmp460;void*_tmp461;struct Cyc_Absyn_Exp*
_tmp462;_LL600: _LL607: _tmp45E=_tmp45C.f1;if((unsigned int)_tmp45E > 1?*((int*)
_tmp45E)== 0: 0){goto _LL606;}else{goto _LL602;}_LL606: _tmp45D=_tmp45C.f2;if((int)
_tmp45D == 0){goto _LL601;}else{goto _LL602;}_LL602: _LL60A: _tmp461=_tmp45C.f1;if((
unsigned int)_tmp461 > 1?*((int*)_tmp461)== 0: 0){_LL60B: _tmp462=((struct Cyc_Absyn_Upper_b_struct*)
_tmp461)->f1;goto _LL608;}else{goto _LL604;}_LL608: _tmp45F=_tmp45C.f2;if((
unsigned int)_tmp45F > 1?*((int*)_tmp45F)== 0: 0){_LL609: _tmp460=((struct Cyc_Absyn_Upper_b_struct*)
_tmp45F)->f1;goto _LL603;}else{goto _LL604;}_LL604: goto _LL605;_LL601: goto _LL5FF;
_LL603: if(Cyc_Evexp_eval_const_uint_exp(_tmp462)< Cyc_Evexp_eval_const_uint_exp(
_tmp460)){return 0;}goto _LL5FF;_LL605: return 0;_LL5FF:;}return Cyc_Tcutil_ptrsubtype(
te,({struct Cyc_List_List*_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463->hd=({
struct _tuple8*_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464->f1=t1;_tmp464->f2=t2;
_tmp464;});_tmp463->tl=assume;_tmp463;}),_tmp45A,_tmp453);_LL5F0: return 0;_LL5EC:;}}
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple8 _tmp466=({struct
_tuple8 _tmp465;_tmp465.f1=Cyc_Tcutil_compress(t1);_tmp465.f2=Cyc_Tcutil_compress(
t2);_tmp465;});void*_tmp467;void*_tmp468;void*_tmp469;void*_tmp46A;_LL60D: _LL613:
_tmp469=_tmp466.f1;if((unsigned int)_tmp469 > 3?*((int*)_tmp469)== 5: 0){_LL614:
_tmp46A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp469)->f2;goto _LL611;}else{
goto _LL60F;}_LL611: _tmp467=_tmp466.f2;if((unsigned int)_tmp467 > 3?*((int*)
_tmp467)== 5: 0){_LL612: _tmp468=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp467)->f2;
goto _LL60E;}else{goto _LL60F;}_LL60F: goto _LL610;_LL60E: return _tmp46A == _tmp468;
_LL610: return 0;_LL60C:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0){return 0;}{struct _tuple4 _tmp46C;void*
_tmp46D;struct Cyc_Absyn_Tqual _tmp46E;struct _tuple4*_tmp46B=(struct _tuple4*)tqs1->hd;
_tmp46C=*_tmp46B;_LL617: _tmp46E=_tmp46C.f1;goto _LL616;_LL616: _tmp46D=_tmp46C.f2;
goto _LL615;_LL615: {struct _tuple4 _tmp470;void*_tmp471;struct Cyc_Absyn_Tqual
_tmp472;struct _tuple4*_tmp46F=(struct _tuple4*)tqs2->hd;_tmp470=*_tmp46F;_LL61A:
_tmp472=_tmp470.f1;goto _LL619;_LL619: _tmp471=_tmp470.f2;goto _LL618;_LL618: if(
_tmp472.q_const? Cyc_Tcutil_subtype(te,assume,_tmp46D,_tmp471): 0){continue;}else{
if(Cyc_Tcutil_unify(_tmp46D,_tmp471)){continue;}else{if(Cyc_Tcutil_isomorphic(
_tmp46D,_tmp471)){continue;}else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp473=Cyc_Tcutil_compress(t);void*_tmp474;_LL61C: if((unsigned int)
_tmp473 > 3?*((int*)_tmp473)== 5: 0){_LL620: _tmp474=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp473)->f2;if((int)_tmp474 == 0){goto _LL61D;}else{goto _LL61E;}}else{goto _LL61E;}
_LL61E: goto _LL61F;_LL61D: return 1;_LL61F: return 0;_LL61B:;}int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2)){return 1;}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{void*
_tmp475=t2;struct Cyc_Absyn_Uniondecl**_tmp476;struct Cyc_List_List*_tmp477;void*
_tmp478;_LL622: if((unsigned int)_tmp475 > 3?*((int*)_tmp475)== 11: 0){_LL629:
_tmp477=((struct Cyc_Absyn_UnionType_struct*)_tmp475)->f2;goto _LL628;_LL628:
_tmp476=((struct Cyc_Absyn_UnionType_struct*)_tmp475)->f3;goto _LL623;}else{goto
_LL624;}_LL624: if((unsigned int)_tmp475 > 3?*((int*)_tmp475)== 5: 0){_LL62A:
_tmp478=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp475)->f2;if((int)_tmp478 == 
2){goto _LL625;}else{goto _LL626;}}else{goto _LL626;}_LL626: goto _LL627;_LL623: if(
_tmp476 == 0){goto _LL621;}{struct Cyc_Absyn_Uniondecl*_tmp479=*_tmp476;if(_tmp479->fields
== 0){goto _LL621;}{struct Cyc_List_List*_tmp47A=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp479->tvs,_tmp477);{
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp479->fields))->v;for(0;fs != 0;fs=fs->tl){if(Cyc_Tcutil_unify(t1,Cyc_Tcutil_substitute(
_tmp47A,(void*)((struct Cyc_Absyn_Structfield*)fs->hd)->type))){return 1;}}}goto
_LL621;}}_LL625: if(Cyc_Tcutil_typ_kind(t2)== (void*)2){return 1;}goto _LL621;
_LL627: goto _LL621;_LL621:;}{void*_tmp47B=t1;struct Cyc_Absyn_PtrInfo _tmp47C;
struct Cyc_Absyn_Conref*_tmp47D;struct Cyc_Absyn_Tqual _tmp47E;struct Cyc_Absyn_Conref*
_tmp47F;void*_tmp480;void*_tmp481;struct Cyc_Absyn_Exp*_tmp482;struct Cyc_Absyn_Tqual
_tmp483;void*_tmp484;struct Cyc_Absyn_Enumdecl*_tmp485;_LL62C: if((unsigned int)
_tmp47B > 3?*((int*)_tmp47B)== 4: 0){_LL63C: _tmp47C=((struct Cyc_Absyn_PointerType_struct*)
_tmp47B)->f1;_LL641: _tmp481=(void*)_tmp47C.elt_typ;goto _LL640;_LL640: _tmp480=(
void*)_tmp47C.rgn_typ;goto _LL63F;_LL63F: _tmp47F=_tmp47C.nullable;goto _LL63E;
_LL63E: _tmp47E=_tmp47C.tq;goto _LL63D;_LL63D: _tmp47D=_tmp47C.bounds;goto _LL62D;}
else{goto _LL62E;}_LL62E: if((unsigned int)_tmp47B > 3?*((int*)_tmp47B)== 7: 0){
_LL644: _tmp484=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp47B)->f1;goto
_LL643;_LL643: _tmp483=((struct Cyc_Absyn_ArrayType_struct*)_tmp47B)->f2;goto
_LL642;_LL642: _tmp482=((struct Cyc_Absyn_ArrayType_struct*)_tmp47B)->f3;goto
_LL62F;}else{goto _LL630;}_LL630: if((unsigned int)_tmp47B > 3?*((int*)_tmp47B)== 
12: 0){_LL645: _tmp485=((struct Cyc_Absyn_EnumType_struct*)_tmp47B)->f2;goto _LL631;}
else{goto _LL632;}_LL632: if((unsigned int)_tmp47B > 3?*((int*)_tmp47B)== 5: 0){goto
_LL633;}else{goto _LL634;}_LL634: if((int)_tmp47B == 1){goto _LL635;}else{goto _LL636;}
_LL636: if((unsigned int)_tmp47B > 3?*((int*)_tmp47B)== 6: 0){goto _LL637;}else{goto
_LL638;}_LL638: if((int)_tmp47B == 0){goto _LL639;}else{goto _LL63A;}_LL63A: goto
_LL63B;_LL62D:{void*_tmp486=t2;struct Cyc_Absyn_PtrInfo _tmp487;struct Cyc_Absyn_Conref*
_tmp488;struct Cyc_Absyn_Tqual _tmp489;struct Cyc_Absyn_Conref*_tmp48A;void*_tmp48B;
void*_tmp48C;_LL647: if((unsigned int)_tmp486 > 3?*((int*)_tmp486)== 4: 0){_LL64D:
_tmp487=((struct Cyc_Absyn_PointerType_struct*)_tmp486)->f1;_LL652: _tmp48C=(void*)
_tmp487.elt_typ;goto _LL651;_LL651: _tmp48B=(void*)_tmp487.rgn_typ;goto _LL650;
_LL650: _tmp48A=_tmp487.nullable;goto _LL64F;_LL64F: _tmp489=_tmp487.tq;goto _LL64E;
_LL64E: _tmp488=_tmp487.bounds;goto _LL648;}else{goto _LL649;}_LL649: if((int)
_tmp486 == 0){goto _LL64A;}else{goto _LL64B;}_LL64B: goto _LL64C;_LL648: {struct Cyc_List_List*
_tmp48D=({struct Cyc_List_List*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496->hd=({
struct _tuple8*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->f1=t1;_tmp497->f2=t2;
_tmp497;});_tmp496->tl=0;_tmp496;});int _tmp48E=Cyc_Tcutil_ptrsubtype(te,_tmp48D,
_tmp481,_tmp48C)? ! _tmp47E.q_const? 1: _tmp489.q_const: 0;int _tmp48F=_tmp48E? 0:((Cyc_Tcutil_bits_only(
_tmp481)? Cyc_Tcutil_is_char_type(_tmp48C): 0)? _tmp489.q_const? 1: ! _tmp47E.q_const:
0);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp47D,_tmp488);
if(! bounds_ok? ! _tmp48F: 0){struct _tuple8 _tmp491=({struct _tuple8 _tmp490;_tmp490.f1=
Cyc_Absyn_conref_val(_tmp47D);_tmp490.f2=Cyc_Absyn_conref_val(_tmp488);_tmp490;});
void*_tmp492;struct Cyc_Absyn_Exp*_tmp493;void*_tmp494;struct Cyc_Absyn_Exp*
_tmp495;_LL654: _LL65A: _tmp494=_tmp491.f1;if((unsigned int)_tmp494 > 1?*((int*)
_tmp494)== 0: 0){_LL65B: _tmp495=((struct Cyc_Absyn_Upper_b_struct*)_tmp494)->f1;
goto _LL658;}else{goto _LL656;}_LL658: _tmp492=_tmp491.f2;if((unsigned int)_tmp492 > 
1?*((int*)_tmp492)== 0: 0){_LL659: _tmp493=((struct Cyc_Absyn_Upper_b_struct*)
_tmp492)->f1;goto _LL655;}else{goto _LL656;}_LL656: goto _LL657;_LL655: if(Cyc_Evexp_eval_const_uint_exp(
_tmp495)>= Cyc_Evexp_eval_const_uint_exp(_tmp493)){bounds_ok=1;}goto _LL653;
_LL657: bounds_ok=1;goto _LL653;_LL653:;}return(bounds_ok? _tmp48E? 1: _tmp48F: 0)? Cyc_Tcutil_unify(
_tmp480,_tmp48B)? 1: Cyc_Tcenv_region_outlives(te,_tmp480,_tmp48B): 0;}_LL64A:
return 1;_LL64C: goto _LL646;_LL646:;}return 0;_LL62F:{void*_tmp498=t2;struct Cyc_Absyn_Exp*
_tmp499;struct Cyc_Absyn_Tqual _tmp49A;void*_tmp49B;_LL65D: if((unsigned int)
_tmp498 > 3?*((int*)_tmp498)== 7: 0){_LL663: _tmp49B=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp498)->f1;goto _LL662;_LL662: _tmp49A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp498)->f2;goto _LL661;_LL661: _tmp499=((struct Cyc_Absyn_ArrayType_struct*)
_tmp498)->f3;goto _LL65E;}else{goto _LL65F;}_LL65F: goto _LL660;_LL65E: {int okay;
okay=(_tmp482 != 0? _tmp499 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp482))>= Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp499)): 0;return(okay? Cyc_Tcutil_unify(_tmp484,_tmp49B): 0)? !
_tmp483.q_const? 1: _tmp49A.q_const: 0;}_LL660: return 0;_LL65C:;}return 0;_LL631:{
void*_tmp49C=t2;struct Cyc_Absyn_Enumdecl*_tmp49D;_LL665: if((unsigned int)_tmp49C
> 3?*((int*)_tmp49C)== 12: 0){_LL669: _tmp49D=((struct Cyc_Absyn_EnumType_struct*)
_tmp49C)->f2;goto _LL666;}else{goto _LL667;}_LL667: goto _LL668;_LL666: if((_tmp485->fields
!= 0? _tmp49D->fields != 0: 0)?((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp485->fields))->v)>= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp49D->fields))->v): 0){return 1;}goto _LL664;_LL668: goto _LL664;
_LL664:;}goto _LL633;_LL633: goto _LL635;_LL635: goto _LL637;_LL637: return Cyc_Tcutil_coerceable(
t2)? 1: t2 == (void*)0;_LL639: return t2 == (void*)0;_LL63B: return 0;_LL62B:;}}void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){if(! Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp49E=Cyc_Absyn_copy_exp(
e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp49F=_cycalloc(
sizeof(*_tmp49F));_tmp49F[0]=({struct Cyc_Absyn_Cast_e_struct _tmp4A0;_tmp4A0.tag=
13;_tmp4A0.f1=(void*)t;_tmp4A0.f2=_tmp49E;_tmp4A0;});_tmp49F;})));e->topt=({
struct Cyc_Core_Opt*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1->v=(void*)t;
_tmp4A1;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*_tmp4A2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL66B: if((unsigned int)
_tmp4A2 > 3?*((int*)_tmp4A2)== 12: 0){goto _LL66C;}else{goto _LL66D;}_LL66D: if((
unsigned int)_tmp4A2 > 3?*((int*)_tmp4A2)== 16: 0){goto _LL66E;}else{goto _LL66F;}
_LL66F: if((unsigned int)_tmp4A2 > 3?*((int*)_tmp4A2)== 5: 0){goto _LL670;}else{goto
_LL671;}_LL671: if((unsigned int)_tmp4A2 > 3?*((int*)_tmp4A2)== 0: 0){goto _LL672;}
else{goto _LL673;}_LL673: if((unsigned int)_tmp4A2 > 3?*((int*)_tmp4A2)== 13: 0){
goto _LL674;}else{goto _LL675;}_LL675: goto _LL676;_LL66C: goto _LL66E;_LL66E: goto
_LL670;_LL670: return 1;_LL672: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_t);_LL674: return 1;_LL676: return 0;_LL66A:;}
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e)){
return 1;}{void*_tmp4A3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL678: if((int)_tmp4A3 == 1){goto _LL679;}else{goto
_LL67A;}_LL67A: if((unsigned int)_tmp4A3 > 3?*((int*)_tmp4A3)== 6: 0){goto _LL67B;}
else{goto _LL67C;}_LL67C: goto _LL67D;_LL679: goto _LL67B;_LL67B: return 1;_LL67D:
return 0;_LL677:;}}int Cyc_Tcutil_is_function_type(void*t){void*_tmp4A4=Cyc_Tcutil_compress(
t);_LL67F: if((unsigned int)_tmp4A4 > 3?*((int*)_tmp4A4)== 8: 0){goto _LL680;}else{
goto _LL681;}_LL681: goto _LL682;_LL680: return 1;_LL682: return 0;_LL67E:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2){struct _tuple8 _tmp4A6=({struct _tuple8 _tmp4A5;_tmp4A5.f1=t1;
_tmp4A5.f2=t2;_tmp4A5;});void*_tmp4A7;int _tmp4A8;void*_tmp4A9;int _tmp4AA;void*
_tmp4AB;void*_tmp4AC;void*_tmp4AD;void*_tmp4AE;void*_tmp4AF;void*_tmp4B0;void*
_tmp4B1;void*_tmp4B2;void*_tmp4B3;void*_tmp4B4;void*_tmp4B5;void*_tmp4B6;void*
_tmp4B7;void*_tmp4B8;void*_tmp4B9;void*_tmp4BA;void*_tmp4BB;void*_tmp4BC;void*
_tmp4BD;void*_tmp4BE;void*_tmp4BF;void*_tmp4C0;_LL684: _LL69E: _tmp4A9=_tmp4A6.f1;
if((unsigned int)_tmp4A9 > 3?*((int*)_tmp4A9)== 6: 0){_LL69F: _tmp4AA=((struct Cyc_Absyn_DoubleType_struct*)
_tmp4A9)->f1;goto _LL69C;}else{goto _LL686;}_LL69C: _tmp4A7=_tmp4A6.f2;if((
unsigned int)_tmp4A7 > 3?*((int*)_tmp4A7)== 6: 0){_LL69D: _tmp4A8=((struct Cyc_Absyn_DoubleType_struct*)
_tmp4A7)->f1;goto _LL685;}else{goto _LL686;}_LL686: _LL6A0: _tmp4AB=_tmp4A6.f1;if((
unsigned int)_tmp4AB > 3?*((int*)_tmp4AB)== 6: 0){goto _LL687;}else{goto _LL688;}
_LL688: _LL6A1: _tmp4AC=_tmp4A6.f2;if((unsigned int)_tmp4AC > 3?*((int*)_tmp4AC)== 
6: 0){goto _LL689;}else{goto _LL68A;}_LL68A: _LL6A2: _tmp4AD=_tmp4A6.f1;if((int)
_tmp4AD == 1){goto _LL68B;}else{goto _LL68C;}_LL68C: _LL6A3: _tmp4AE=_tmp4A6.f2;if((
int)_tmp4AE == 1){goto _LL68D;}else{goto _LL68E;}_LL68E: _LL6A4: _tmp4AF=_tmp4A6.f1;
if((unsigned int)_tmp4AF > 3?*((int*)_tmp4AF)== 5: 0){_LL6A6: _tmp4B1=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4AF)->f1;if((int)_tmp4B1 == 1){goto _LL6A5;}
else{goto _LL690;}_LL6A5: _tmp4B0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AF)->f2;
if((int)_tmp4B0 == 3){goto _LL68F;}else{goto _LL690;}}else{goto _LL690;}_LL690:
_LL6A7: _tmp4B2=_tmp4A6.f2;if((unsigned int)_tmp4B2 > 3?*((int*)_tmp4B2)== 5: 0){
_LL6A9: _tmp4B4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B2)->f1;if((int)
_tmp4B4 == 1){goto _LL6A8;}else{goto _LL692;}_LL6A8: _tmp4B3=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B2)->f2;if((int)_tmp4B3 == 3){goto _LL691;}else{goto _LL692;}}else{goto _LL692;}
_LL692: _LL6AA: _tmp4B5=_tmp4A6.f1;if((unsigned int)_tmp4B5 > 3?*((int*)_tmp4B5)== 
5: 0){_LL6AC: _tmp4B7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B5)->f1;if((
int)_tmp4B7 == 0){goto _LL6AB;}else{goto _LL694;}_LL6AB: _tmp4B6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B5)->f2;if((int)_tmp4B6 == 3){goto _LL693;}else{goto _LL694;}}else{goto _LL694;}
_LL694: _LL6AD: _tmp4B8=_tmp4A6.f2;if((unsigned int)_tmp4B8 > 3?*((int*)_tmp4B8)== 
5: 0){_LL6AF: _tmp4BA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B8)->f1;if((
int)_tmp4BA == 0){goto _LL6AE;}else{goto _LL696;}_LL6AE: _tmp4B9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B8)->f2;if((int)_tmp4B9 == 3){goto _LL695;}else{goto _LL696;}}else{goto _LL696;}
_LL696: _LL6B0: _tmp4BB=_tmp4A6.f1;if((unsigned int)_tmp4BB > 3?*((int*)_tmp4BB)== 
5: 0){_LL6B2: _tmp4BD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4BB)->f1;if((
int)_tmp4BD == 1){goto _LL6B1;}else{goto _LL698;}_LL6B1: _tmp4BC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4BB)->f2;if((int)_tmp4BC == 2){goto _LL697;}else{goto _LL698;}}else{goto _LL698;}
_LL698: _LL6B3: _tmp4BE=_tmp4A6.f2;if((unsigned int)_tmp4BE > 3?*((int*)_tmp4BE)== 
5: 0){_LL6B5: _tmp4C0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4BE)->f1;if((
int)_tmp4C0 == 1){goto _LL6B4;}else{goto _LL69A;}_LL6B4: _tmp4BF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4BE)->f2;if((int)_tmp4BF == 2){goto _LL699;}else{goto _LL69A;}}else{goto _LL69A;}
_LL69A: goto _LL69B;_LL685: if(_tmp4AA){return t1;}else{return t2;}_LL687: return t1;
_LL689: return t2;_LL68B: goto _LL68D;_LL68D: return(void*)1;_LL68F: goto _LL691;_LL691:
return Cyc_Absyn_ulonglong_t;_LL693: goto _LL695;_LL695: return Cyc_Absyn_slonglong_t;
_LL697: goto _LL699;_LL699: return Cyc_Absyn_uint_t;_LL69B: return Cyc_Absyn_sint_t;
_LL683:;}void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){void*
_tmp4C1=(void*)e->r;struct Cyc_Core_Opt*_tmp4C2;_LL6B7: if(*((int*)_tmp4C1)== 4){
_LL6BB: _tmp4C2=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp4C1)->f2;if(_tmp4C2 == 0){
goto _LL6B8;}else{goto _LL6B9;}}else{goto _LL6B9;}_LL6B9: goto _LL6BA;_LL6B8:({void*
_tmp4C3[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("assignment in test",sizeof(
unsigned char),19),_tag_arr(_tmp4C3,sizeof(void*),0));});goto _LL6B6;_LL6BA: goto
_LL6B6;_LL6B6:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple8 _tmp4C5=({struct _tuple8 _tmp4C4;
_tmp4C4.f1=c1;_tmp4C4.f2=c2;_tmp4C4;});void*_tmp4C6;void*_tmp4C7;void*_tmp4C8;
void*_tmp4C9;void*_tmp4CA;struct Cyc_Core_Opt*_tmp4CB;struct Cyc_Core_Opt**_tmp4CC;
void*_tmp4CD;struct Cyc_Core_Opt*_tmp4CE;struct Cyc_Core_Opt**_tmp4CF;void*_tmp4D0;
void*_tmp4D1;void*_tmp4D2;void*_tmp4D3;struct Cyc_Core_Opt*_tmp4D4;struct Cyc_Core_Opt**
_tmp4D5;void*_tmp4D6;void*_tmp4D7;struct Cyc_Core_Opt*_tmp4D8;struct Cyc_Core_Opt**
_tmp4D9;void*_tmp4DA;void*_tmp4DB;void*_tmp4DC;void*_tmp4DD;struct Cyc_Core_Opt*
_tmp4DE;struct Cyc_Core_Opt**_tmp4DF;void*_tmp4E0;void*_tmp4E1;struct Cyc_Core_Opt*
_tmp4E2;struct Cyc_Core_Opt**_tmp4E3;_LL6BD: _LL6CB: _tmp4C8=_tmp4C5.f1;if(*((int*)
_tmp4C8)== 0){_LL6CC: _tmp4C9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4C8)->f1;
goto _LL6C9;}else{goto _LL6BF;}_LL6C9: _tmp4C6=_tmp4C5.f2;if(*((int*)_tmp4C6)== 0){
_LL6CA: _tmp4C7=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4C6)->f1;goto _LL6BE;}
else{goto _LL6BF;}_LL6BF: _LL6CD: _tmp4CA=_tmp4C5.f2;if(*((int*)_tmp4CA)== 1){
_LL6CE: _tmp4CB=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4CA)->f1;_tmp4CC=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4CA)->f1;goto _LL6C0;}
else{goto _LL6C1;}_LL6C1: _LL6CF: _tmp4CD=_tmp4C5.f1;if(*((int*)_tmp4CD)== 1){
_LL6D0: _tmp4CE=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4CD)->f1;_tmp4CF=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4CD)->f1;goto _LL6C2;}
else{goto _LL6C3;}_LL6C3: _LL6D3: _tmp4D2=_tmp4C5.f1;if(*((int*)_tmp4D2)== 2){
_LL6D5: _tmp4D4=((struct Cyc_Absyn_Less_kb_struct*)_tmp4D2)->f1;_tmp4D5=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4D2)->f1;goto _LL6D4;_LL6D4: _tmp4D3=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp4D2)->f2;goto _LL6D1;}else{goto _LL6C5;}_LL6D1:
_tmp4D0=_tmp4C5.f2;if(*((int*)_tmp4D0)== 0){_LL6D2: _tmp4D1=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp4D0)->f1;goto _LL6C4;}else{goto _LL6C5;}_LL6C5: _LL6D9: _tmp4DA=_tmp4C5.f1;if(*((
int*)_tmp4DA)== 0){_LL6DA: _tmp4DB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4DA)->f1;
goto _LL6D6;}else{goto _LL6C7;}_LL6D6: _tmp4D6=_tmp4C5.f2;if(*((int*)_tmp4D6)== 2){
_LL6D8: _tmp4D8=((struct Cyc_Absyn_Less_kb_struct*)_tmp4D6)->f1;_tmp4D9=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4D6)->f1;goto _LL6D7;_LL6D7: _tmp4D7=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp4D6)->f2;goto _LL6C6;}else{goto _LL6C7;}_LL6C7:
_LL6DE: _tmp4E0=_tmp4C5.f1;if(*((int*)_tmp4E0)== 2){_LL6E0: _tmp4E2=((struct Cyc_Absyn_Less_kb_struct*)
_tmp4E0)->f1;_tmp4E3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp4E0)->f1;goto _LL6DF;_LL6DF: _tmp4E1=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4E0)->f2;goto _LL6DB;}else{goto _LL6BC;}_LL6DB: _tmp4DC=_tmp4C5.f2;if(*((int*)
_tmp4DC)== 2){_LL6DD: _tmp4DE=((struct Cyc_Absyn_Less_kb_struct*)_tmp4DC)->f1;
_tmp4DF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp4DC)->f1;
goto _LL6DC;_LL6DC: _tmp4DD=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4DC)->f2;
goto _LL6C8;}else{goto _LL6BC;}_LL6BE: return _tmp4C9 == _tmp4C7;_LL6C0:*_tmp4CC=({
struct Cyc_Core_Opt*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->v=(void*)c1;
_tmp4E4;});return 1;_LL6C2:*_tmp4CF=({struct Cyc_Core_Opt*_tmp4E5=_cycalloc(
sizeof(*_tmp4E5));_tmp4E5->v=(void*)c2;_tmp4E5;});return 1;_LL6C4: if(Cyc_Tcutil_kind_leq(
_tmp4D1,_tmp4D3)){*_tmp4D5=({struct Cyc_Core_Opt*_tmp4E6=_cycalloc(sizeof(*
_tmp4E6));_tmp4E6->v=(void*)c2;_tmp4E6;});return 1;}else{return 0;}_LL6C6: if(Cyc_Tcutil_kind_leq(
_tmp4DB,_tmp4D7)){*_tmp4D9=({struct Cyc_Core_Opt*_tmp4E7=_cycalloc(sizeof(*
_tmp4E7));_tmp4E7->v=(void*)c1;_tmp4E7;});return 1;}else{return 0;}_LL6C8: if(Cyc_Tcutil_kind_leq(
_tmp4E1,_tmp4DD)){*_tmp4DF=({struct Cyc_Core_Opt*_tmp4E8=_cycalloc(sizeof(*
_tmp4E8));_tmp4E8->v=(void*)c1;_tmp4E8;});return 1;}else{if(Cyc_Tcutil_kind_leq(
_tmp4DD,_tmp4E1)){*_tmp4E3=({struct Cyc_Core_Opt*_tmp4E9=_cycalloc(sizeof(*
_tmp4E9));_tmp4E9->v=(void*)c2;_tmp4E9;});return 1;}else{return 0;}}_LL6BC:;}}
static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){return({int*
_tmp4EA=_cycalloc_atomic(sizeof(*_tmp4EA));_tmp4EA[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp4EA;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp4EE;_tmp4EE.tag=1;_tmp4EE.f1=(int)((unsigned int)
i);{void*_tmp4ED[1]={& _tmp4EE};Cyc_Std_aprintf(_tag_arr("#%d",sizeof(
unsigned char),4),_tag_arr(_tmp4ED,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*
_tmp4EB=_cycalloc(sizeof(*_tmp4EB));_tmp4EB->name=({struct _tagged_arr*_tmp4EC=
_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp4EC[0]=s;_tmp4EC;});_tmp4EB->identity=
0;_tmp4EB->kind=(void*)k;_tmp4EB;});}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _tagged_arr _tmp4EF=*t->name;return*((const unsigned char*)
_check_unknown_subscript(_tmp4EF,sizeof(unsigned char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_Std_String_pa_struct _tmp4F1;_tmp4F1.tag=0;
_tmp4F1.f1=(struct _tagged_arr)*t->name;{void*_tmp4F0[1]={& _tmp4F1};Cyc_Std_printf(
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp4F0,sizeof(void*),1));}});
if(! Cyc_Tcutil_is_temp_tvar(t)){return;}{struct _tagged_arr _tmp4F2=Cyc_Std_strconcat(
_tag_arr("`",sizeof(unsigned char),2),*t->name);*((unsigned char*)
_check_unknown_subscript(_tmp4F2,sizeof(unsigned char),1))='t';t->name=({struct
_tagged_arr*_tmp4F3=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp4F3[0]=(struct
_tagged_arr)_tmp4F2;_tmp4F3;});}}struct _tuple13{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(struct _tuple13*x){
return({struct _tuple2*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4->f1=(struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->v=(*x).f1;
_tmp4F5;});_tmp4F4->f2=(*x).f2;_tmp4F4->f3=(*x).f3;_tmp4F4;});}void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){struct Cyc_List_List*_tmp4F6=0;{
struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd)){_tmp4F6=({struct Cyc_List_List*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));
_tmp4F7->hd=(void*)((void*)atts->hd);_tmp4F7->tl=_tmp4F6;_tmp4F7;});}}}return(
void*)({struct Cyc_Absyn_FnType_struct*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));
_tmp4F8[0]=({struct Cyc_Absyn_FnType_struct _tmp4F9;_tmp4F9.tag=8;_tmp4F9.f1=({
struct Cyc_Absyn_FnInfo _tmp4FA;_tmp4FA.tvars=fd->tvs;_tmp4FA.effect=fd->effect;
_tmp4FA.ret_typ=(void*)((void*)fd->ret_type);_tmp4FA.args=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args);_tmp4FA.c_varargs=fd->c_varargs;_tmp4FA.cyc_varargs=fd->cyc_varargs;
_tmp4FA.rgn_po=fd->rgn_po;_tmp4FA.attributes=_tmp4F6;_tmp4FA;});_tmp4F9;});
_tmp4F8;});}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple14{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple14*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*
t).f2;}static struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({
struct _tuple4*_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB->f1=(*pr).f1;_tmp4FB->f2=
t;_tmp4FB;});}struct _tuple15{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};
struct _tuple16{struct _tuple15*f1;void*f2;};static struct _tuple16*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple2*y){return({struct _tuple16*_tmp4FC=
_region_malloc(rgn,sizeof(*_tmp4FC));_tmp4FC->f1=({struct _tuple15*_tmp4FD=
_region_malloc(rgn,sizeof(*_tmp4FD));_tmp4FD->f1=(*y).f1;_tmp4FD->f2=(*y).f2;
_tmp4FD;});_tmp4FC->f2=(*y).f3;_tmp4FC;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(
struct _tuple16*w){void*_tmp4FF;struct _tuple15*_tmp500;struct _tuple16 _tmp4FE=*w;
_LL6E3: _tmp500=_tmp4FE.f1;goto _LL6E2;_LL6E2: _tmp4FF=_tmp4FE.f2;goto _LL6E1;_LL6E1: {
struct Cyc_Absyn_Tqual _tmp502;struct Cyc_Core_Opt*_tmp503;struct _tuple15 _tmp501=*
_tmp500;_LL6E6: _tmp503=_tmp501.f1;goto _LL6E5;_LL6E5: _tmp502=_tmp501.f2;goto
_LL6E4;_LL6E4: return({struct _tuple2*_tmp504=_cycalloc(sizeof(*_tmp504));_tmp504->f1=
_tmp503;_tmp504->f2=_tmp502;_tmp504->f3=_tmp4FF;_tmp504;});}}static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Structfield*f){return(void*)f->type;}static struct Cyc_Absyn_Structfield*
Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Structfield*f,void*t){return({struct
Cyc_Absyn_Structfield*_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505->name=f->name;
_tmp505->tq=f->tq;_tmp505->type=(void*)t;_tmp505->width=f->width;_tmp505->attributes=
f->attributes;_tmp505;});}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){void*_tmp506=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp507;struct Cyc_Absyn_Structdecl**_tmp508;struct Cyc_List_List*
_tmp509;struct _tuple1*_tmp50A;struct Cyc_Absyn_Uniondecl**_tmp50B;struct Cyc_List_List*
_tmp50C;struct _tuple1*_tmp50D;struct Cyc_Absyn_TunionInfo _tmp50E;void*_tmp50F;
struct Cyc_List_List*_tmp510;void*_tmp511;struct Cyc_Absyn_TunionFieldInfo _tmp512;
struct Cyc_List_List*_tmp513;void*_tmp514;struct Cyc_Core_Opt*_tmp515;struct Cyc_List_List*
_tmp516;struct _tuple1*_tmp517;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Tqual
_tmp519;void*_tmp51A;struct Cyc_Absyn_PtrInfo _tmp51B;struct Cyc_Absyn_Conref*
_tmp51C;struct Cyc_Absyn_Tqual _tmp51D;struct Cyc_Absyn_Conref*_tmp51E;void*_tmp51F;
void*_tmp520;struct Cyc_Absyn_FnInfo _tmp521;struct Cyc_List_List*_tmp522;struct Cyc_List_List*
_tmp523;struct Cyc_Absyn_VarargInfo*_tmp524;int _tmp525;struct Cyc_List_List*
_tmp526;void*_tmp527;struct Cyc_Core_Opt*_tmp528;struct Cyc_List_List*_tmp529;
struct Cyc_List_List*_tmp52A;struct Cyc_List_List*_tmp52B;struct Cyc_List_List*
_tmp52C;struct Cyc_Core_Opt*_tmp52D;void*_tmp52E;void*_tmp52F;void*_tmp530;void*
_tmp531;struct Cyc_List_List*_tmp532;_LL6E8: if((unsigned int)_tmp506 > 3?*((int*)
_tmp506)== 1: 0){_LL71A: _tmp507=((struct Cyc_Absyn_VarType_struct*)_tmp506)->f1;
goto _LL6E9;}else{goto _LL6EA;}_LL6EA: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)
== 10: 0){_LL71D: _tmp50A=((struct Cyc_Absyn_StructType_struct*)_tmp506)->f1;goto
_LL71C;_LL71C: _tmp509=((struct Cyc_Absyn_StructType_struct*)_tmp506)->f2;goto
_LL71B;_LL71B: _tmp508=((struct Cyc_Absyn_StructType_struct*)_tmp506)->f3;goto
_LL6EB;}else{goto _LL6EC;}_LL6EC: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)== 
11: 0){_LL720: _tmp50D=((struct Cyc_Absyn_UnionType_struct*)_tmp506)->f1;goto _LL71F;
_LL71F: _tmp50C=((struct Cyc_Absyn_UnionType_struct*)_tmp506)->f2;goto _LL71E;
_LL71E: _tmp50B=((struct Cyc_Absyn_UnionType_struct*)_tmp506)->f3;goto _LL6ED;}
else{goto _LL6EE;}_LL6EE: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)== 2: 0){
_LL721: _tmp50E=((struct Cyc_Absyn_TunionType_struct*)_tmp506)->f1;_LL724: _tmp511=(
void*)_tmp50E.tunion_info;goto _LL723;_LL723: _tmp510=_tmp50E.targs;goto _LL722;
_LL722: _tmp50F=(void*)_tmp50E.rgn;goto _LL6EF;}else{goto _LL6F0;}_LL6F0: if((
unsigned int)_tmp506 > 3?*((int*)_tmp506)== 3: 0){_LL725: _tmp512=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp506)->f1;_LL727: _tmp514=(void*)_tmp512.field_info;goto _LL726;_LL726: _tmp513=
_tmp512.targs;goto _LL6F1;}else{goto _LL6F2;}_LL6F2: if((unsigned int)_tmp506 > 3?*((
int*)_tmp506)== 18: 0){_LL72A: _tmp517=((struct Cyc_Absyn_TypedefType_struct*)
_tmp506)->f1;goto _LL729;_LL729: _tmp516=((struct Cyc_Absyn_TypedefType_struct*)
_tmp506)->f2;goto _LL728;_LL728: _tmp515=((struct Cyc_Absyn_TypedefType_struct*)
_tmp506)->f3;goto _LL6F3;}else{goto _LL6F4;}_LL6F4: if((unsigned int)_tmp506 > 3?*((
int*)_tmp506)== 7: 0){_LL72D: _tmp51A=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp506)->f1;goto _LL72C;_LL72C: _tmp519=((struct Cyc_Absyn_ArrayType_struct*)
_tmp506)->f2;goto _LL72B;_LL72B: _tmp518=((struct Cyc_Absyn_ArrayType_struct*)
_tmp506)->f3;goto _LL6F5;}else{goto _LL6F6;}_LL6F6: if((unsigned int)_tmp506 > 3?*((
int*)_tmp506)== 4: 0){_LL72E: _tmp51B=((struct Cyc_Absyn_PointerType_struct*)
_tmp506)->f1;_LL733: _tmp520=(void*)_tmp51B.elt_typ;goto _LL732;_LL732: _tmp51F=(
void*)_tmp51B.rgn_typ;goto _LL731;_LL731: _tmp51E=_tmp51B.nullable;goto _LL730;
_LL730: _tmp51D=_tmp51B.tq;goto _LL72F;_LL72F: _tmp51C=_tmp51B.bounds;goto _LL6F7;}
else{goto _LL6F8;}_LL6F8: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)== 8: 0){
_LL734: _tmp521=((struct Cyc_Absyn_FnType_struct*)_tmp506)->f1;_LL73C: _tmp529=
_tmp521.tvars;goto _LL73B;_LL73B: _tmp528=_tmp521.effect;goto _LL73A;_LL73A: _tmp527=(
void*)_tmp521.ret_typ;goto _LL739;_LL739: _tmp526=_tmp521.args;goto _LL738;_LL738:
_tmp525=_tmp521.c_varargs;goto _LL737;_LL737: _tmp524=_tmp521.cyc_varargs;goto
_LL736;_LL736: _tmp523=_tmp521.rgn_po;goto _LL735;_LL735: _tmp522=_tmp521.attributes;
goto _LL6F9;}else{goto _LL6FA;}_LL6FA: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)
== 9: 0){_LL73D: _tmp52A=((struct Cyc_Absyn_TupleType_struct*)_tmp506)->f1;goto
_LL6FB;}else{goto _LL6FC;}_LL6FC: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)== 
14: 0){_LL73E: _tmp52B=((struct Cyc_Absyn_AnonStructType_struct*)_tmp506)->f1;goto
_LL6FD;}else{goto _LL6FE;}_LL6FE: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)== 
15: 0){_LL73F: _tmp52C=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp506)->f1;goto
_LL6FF;}else{goto _LL700;}_LL700: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)== 0:
0){_LL740: _tmp52D=((struct Cyc_Absyn_Evar_struct*)_tmp506)->f2;goto _LL701;}else{
goto _LL702;}_LL702: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)== 17: 0){_LL741:
_tmp52E=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp506)->f1;goto _LL703;}
else{goto _LL704;}_LL704: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)== 13: 0){
_LL742: _tmp52F=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp506)->f1;goto
_LL705;}else{goto _LL706;}_LL706: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)== 
12: 0){goto _LL707;}else{goto _LL708;}_LL708: if((unsigned int)_tmp506 > 3?*((int*)
_tmp506)== 16: 0){goto _LL709;}else{goto _LL70A;}_LL70A: if((int)_tmp506 == 0){goto
_LL70B;}else{goto _LL70C;}_LL70C: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)== 5:
0){goto _LL70D;}else{goto _LL70E;}_LL70E: if((int)_tmp506 == 1){goto _LL70F;}else{
goto _LL710;}_LL710: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)== 6: 0){goto
_LL711;}else{goto _LL712;}_LL712: if((int)_tmp506 == 2){goto _LL713;}else{goto _LL714;}
_LL714: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)== 21: 0){_LL743: _tmp530=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp506)->f1;goto _LL715;}else{goto _LL716;}_LL716:
if((unsigned int)_tmp506 > 3?*((int*)_tmp506)== 19: 0){_LL744: _tmp531=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp506)->f1;goto _LL717;}else{goto _LL718;}
_LL718: if((unsigned int)_tmp506 > 3?*((int*)_tmp506)== 20: 0){_LL745: _tmp532=((
struct Cyc_Absyn_JoinEff_struct*)_tmp506)->f1;goto _LL719;}else{goto _LL6E7;}_LL6E9: {
struct _handler_cons _tmp533;_push_handler(& _tmp533);{int _tmp535=0;if(setjmp(
_tmp533.handler)){_tmp535=1;}if(! _tmp535){{void*_tmp536=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp507);_npop_handler(0);return
_tmp536;};_pop_handler();}else{void*_tmp534=(void*)_exn_thrown;void*_tmp538=
_tmp534;_LL747: if(_tmp538 == Cyc_Core_Not_found){goto _LL748;}else{goto _LL749;}
_LL749: goto _LL74A;_LL748: return t;_LL74A:(void)_throw(_tmp538);_LL746:;}}}_LL6EB: {
struct Cyc_List_List*_tmp539=Cyc_Tcutil_substs(rgn,inst,_tmp509);return _tmp539 == 
_tmp509? t:(void*)({struct Cyc_Absyn_StructType_struct*_tmp53A=_cycalloc(sizeof(*
_tmp53A));_tmp53A[0]=({struct Cyc_Absyn_StructType_struct _tmp53B;_tmp53B.tag=10;
_tmp53B.f1=_tmp50A;_tmp53B.f2=_tmp539;_tmp53B.f3=_tmp508;_tmp53B;});_tmp53A;});}
_LL6ED: {struct Cyc_List_List*_tmp53C=Cyc_Tcutil_substs(rgn,inst,_tmp50C);return
_tmp53C == _tmp50C? t:(void*)({struct Cyc_Absyn_UnionType_struct*_tmp53D=_cycalloc(
sizeof(*_tmp53D));_tmp53D[0]=({struct Cyc_Absyn_UnionType_struct _tmp53E;_tmp53E.tag=
11;_tmp53E.f1=_tmp50D;_tmp53E.f2=_tmp53C;_tmp53E.f3=_tmp50B;_tmp53E;});_tmp53D;});}
_LL6EF: {struct Cyc_List_List*_tmp53F=Cyc_Tcutil_substs(rgn,inst,_tmp510);void*
_tmp540=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp50F);return(_tmp53F == _tmp510?
_tmp540 == _tmp50F: 0)? t:(void*)({struct Cyc_Absyn_TunionType_struct*_tmp541=
_cycalloc(sizeof(*_tmp541));_tmp541[0]=({struct Cyc_Absyn_TunionType_struct
_tmp542;_tmp542.tag=2;_tmp542.f1=({struct Cyc_Absyn_TunionInfo _tmp543;_tmp543.tunion_info=(
void*)_tmp511;_tmp543.targs=_tmp53F;_tmp543.rgn=(void*)_tmp540;_tmp543;});
_tmp542;});_tmp541;});}_LL6F1: {struct Cyc_List_List*_tmp544=Cyc_Tcutil_substs(
rgn,inst,_tmp513);return _tmp544 == _tmp513? t:(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp546;_tmp546.tag=3;_tmp546.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp547;
_tmp547.field_info=(void*)_tmp514;_tmp547.targs=_tmp544;_tmp547;});_tmp546;});
_tmp545;});}_LL6F3: {struct Cyc_List_List*_tmp548=Cyc_Tcutil_substs(rgn,inst,
_tmp516);return _tmp548 == _tmp516? t:(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp54A;_tmp54A.tag=18;_tmp54A.f1=_tmp517;_tmp54A.f2=_tmp548;_tmp54A.f3=_tmp515;
_tmp54A;});_tmp549;});}_LL6F5: {void*_tmp54B=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp51A);return _tmp54B == _tmp51A? t:(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp54D;_tmp54D.tag=7;_tmp54D.f1=(void*)_tmp54B;_tmp54D.f2=_tmp519;_tmp54D.f3=
_tmp518;_tmp54D;});_tmp54C;});}_LL6F7: {void*_tmp54E=Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp520);void*_tmp54F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp51F);if(_tmp54E == 
_tmp520? _tmp54F == _tmp51F: 0){return t;}return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550[0]=({struct Cyc_Absyn_PointerType_struct
_tmp551;_tmp551.tag=4;_tmp551.f1=({struct Cyc_Absyn_PtrInfo _tmp552;_tmp552.elt_typ=(
void*)_tmp54E;_tmp552.rgn_typ=(void*)_tmp54F;_tmp552.nullable=_tmp51E;_tmp552.tq=
_tmp51D;_tmp552.bounds=_tmp51C;_tmp552;});_tmp551;});_tmp550;});}_LL6F9:{struct
Cyc_List_List*_tmp553=_tmp529;for(0;_tmp553 != 0;_tmp553=_tmp553->tl){inst=({
struct Cyc_List_List*_tmp554=_region_malloc(rgn,sizeof(*_tmp554));_tmp554->hd=({
struct _tuple5*_tmp555=_region_malloc(rgn,sizeof(*_tmp555));_tmp555->f1=(struct
Cyc_Absyn_Tvar*)_tmp553->hd;_tmp555->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556[0]=({struct Cyc_Absyn_VarType_struct
_tmp557;_tmp557.tag=1;_tmp557.f1=(struct Cyc_Absyn_Tvar*)_tmp553->hd;_tmp557;});
_tmp556;});_tmp555;});_tmp554->tl=inst;_tmp554;});}}{struct Cyc_List_List*_tmp559;
struct Cyc_List_List*_tmp55A;struct _tuple0 _tmp558=((struct _tuple0(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple16*(*f)(
struct _RegionHandle*,struct _tuple2*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp526));_LL74D: _tmp55A=
_tmp558.f1;goto _LL74C;_LL74C: _tmp559=_tmp558.f2;goto _LL74B;_LL74B: {struct Cyc_List_List*
_tmp55B=Cyc_Tcutil_substs(rgn,inst,_tmp559);struct Cyc_List_List*_tmp55C=((struct
Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_substitute_f2,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,
rgn,_tmp55A,_tmp55B));struct Cyc_Core_Opt*eff2;if(_tmp528 == 0){eff2=0;}else{void*
_tmp55D=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmp528->v);if(_tmp55D == (void*)
_tmp528->v){eff2=_tmp528;}else{eff2=({struct Cyc_Core_Opt*_tmp55E=_cycalloc(
sizeof(*_tmp55E));_tmp55E->v=(void*)_tmp55D;_tmp55E;});}}{struct Cyc_Absyn_VarargInfo*
cyc_varargs2;if(_tmp524 == 0){cyc_varargs2=0;}else{int _tmp560;void*_tmp561;struct
Cyc_Absyn_Tqual _tmp562;struct Cyc_Core_Opt*_tmp563;struct Cyc_Absyn_VarargInfo
_tmp55F=*_tmp524;_LL752: _tmp563=_tmp55F.name;goto _LL751;_LL751: _tmp562=_tmp55F.tq;
goto _LL750;_LL750: _tmp561=(void*)_tmp55F.type;goto _LL74F;_LL74F: _tmp560=_tmp55F.inject;
goto _LL74E;_LL74E: {void*_tmp564=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp561);if(
_tmp564 == _tmp561){cyc_varargs2=_tmp524;}else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*
_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565->name=_tmp563;_tmp565->tq=_tmp562;
_tmp565->type=(void*)_tmp564;_tmp565->inject=_tmp560;_tmp565;});}}}{struct Cyc_List_List*
rgn_po2;struct Cyc_List_List*_tmp567;struct Cyc_List_List*_tmp568;struct _tuple0
_tmp566=Cyc_List_rsplit(rgn,rgn,_tmp523);_LL755: _tmp568=_tmp566.f1;goto _LL754;
_LL754: _tmp567=_tmp566.f2;goto _LL753;_LL753: {struct Cyc_List_List*_tmp569=Cyc_Tcutil_substs(
rgn,inst,_tmp568);struct Cyc_List_List*_tmp56A=Cyc_Tcutil_substs(rgn,inst,_tmp567);
if(_tmp569 == _tmp568? _tmp56A == _tmp567: 0){rgn_po2=_tmp523;}else{rgn_po2=Cyc_List_zip(
_tmp569,_tmp56A);}return(void*)({struct Cyc_Absyn_FnType_struct*_tmp56B=_cycalloc(
sizeof(*_tmp56B));_tmp56B[0]=({struct Cyc_Absyn_FnType_struct _tmp56C;_tmp56C.tag=
8;_tmp56C.f1=({struct Cyc_Absyn_FnInfo _tmp56D;_tmp56D.tvars=_tmp529;_tmp56D.effect=
eff2;_tmp56D.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,inst,_tmp527);_tmp56D.args=
_tmp55C;_tmp56D.c_varargs=_tmp525;_tmp56D.cyc_varargs=cyc_varargs2;_tmp56D.rgn_po=
rgn_po2;_tmp56D.attributes=_tmp522;_tmp56D;});_tmp56C;});_tmp56B;});}}}}}_LL6FB: {
struct Cyc_List_List*_tmp56E=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp52A);struct Cyc_List_List*_tmp56F=Cyc_Tcutil_substs(rgn,inst,_tmp56E);if(
_tmp56F == _tmp56E){return t;}{struct Cyc_List_List*_tmp570=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp52A,_tmp56F);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571[0]=({struct Cyc_Absyn_TupleType_struct
_tmp572;_tmp572.tag=9;_tmp572.f1=_tmp570;_tmp572;});_tmp571;});}}_LL6FD: {struct
Cyc_List_List*_tmp573=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(
struct Cyc_Absyn_Structfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp52B);struct Cyc_List_List*_tmp574=Cyc_Tcutil_substs(rgn,inst,_tmp573);if(
_tmp574 == _tmp573){return t;}{struct Cyc_List_List*_tmp575=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(*f)(struct Cyc_Absyn_Structfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp52B,
_tmp574);return(void*)({struct Cyc_Absyn_AnonStructType_struct*_tmp576=_cycalloc(
sizeof(*_tmp576));_tmp576[0]=({struct Cyc_Absyn_AnonStructType_struct _tmp577;
_tmp577.tag=14;_tmp577.f1=_tmp575;_tmp577;});_tmp576;});}}_LL6FF: {struct Cyc_List_List*
_tmp578=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct Cyc_Absyn_Structfield*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,_tmp52C);struct
Cyc_List_List*_tmp579=Cyc_Tcutil_substs(rgn,inst,_tmp578);if(_tmp579 == _tmp578){
return t;}{struct Cyc_List_List*_tmp57A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Structfield*(*
f)(struct Cyc_Absyn_Structfield*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp52C,_tmp579);return(void*)({
struct Cyc_Absyn_AnonStructType_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));
_tmp57B[0]=({struct Cyc_Absyn_AnonStructType_struct _tmp57C;_tmp57C.tag=14;_tmp57C.f1=
_tmp57A;_tmp57C;});_tmp57B;});}}_LL701: if(_tmp52D != 0){return Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp52D->v);}else{return t;}_LL703: {void*_tmp57D=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp52E);return _tmp57D == _tmp52E? t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*
_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp57F;_tmp57F.tag=17;_tmp57F.f1=(void*)_tmp57D;_tmp57F;});_tmp57E;});}_LL705: {
void*_tmp580=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp52F);return _tmp580 == _tmp52F? t:(
void*)({struct Cyc_Absyn_SizeofType_struct*_tmp581=_cycalloc(sizeof(*_tmp581));
_tmp581[0]=({struct Cyc_Absyn_SizeofType_struct _tmp582;_tmp582.tag=13;_tmp582.f1=(
void*)_tmp580;_tmp582;});_tmp581;});}_LL707: return t;_LL709: return t;_LL70B: return
t;_LL70D: return t;_LL70F: return t;_LL711: return t;_LL713: return t;_LL715: {void*
_tmp583=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp530);return _tmp583 == _tmp530? t:(void*)({
struct Cyc_Absyn_RgnsEff_struct*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584[0]=({
struct Cyc_Absyn_RgnsEff_struct _tmp585;_tmp585.tag=21;_tmp585.f1=(void*)_tmp583;
_tmp585;});_tmp584;});}_LL717: {void*_tmp586=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp531);return _tmp586 == _tmp531? t:(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp588;_tmp588.tag=19;_tmp588.f1=(void*)_tmp586;_tmp588;});_tmp587;});}_LL719: {
struct Cyc_List_List*_tmp589=Cyc_Tcutil_substs(rgn,inst,_tmp532);return _tmp589 == 
_tmp532? t:(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp58A=_cycalloc(sizeof(*
_tmp58A));_tmp58A[0]=({struct Cyc_Absyn_JoinEff_struct _tmp58B;_tmp58B.tag=20;
_tmp58B.f1=_tmp589;_tmp58B;});_tmp58A;});}_LL6E7:;}static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 
0){return 0;}{void*_tmp58C=(void*)ts->hd;struct Cyc_List_List*_tmp58D=ts->tl;void*
_tmp58E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp58C);struct Cyc_List_List*_tmp58F=Cyc_Tcutil_substs(
rgn,inst,_tmp58D);if(_tmp58C == _tmp58E? _tmp58D == _tmp58F: 0){return ts;}return(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp590=_cycalloc(sizeof(*_tmp590));
_tmp590->hd=(void*)_tmp58E;_tmp590->tl=_tmp58F;_tmp590;});}}extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct Cyc_Absyn_Exp*Cyc_Tcutil_default_initializer(struct Cyc_Tcenv_Tenv*
te,void*t,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Exp*e=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Const_e_struct*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));
_tmp5AC[0]=({struct Cyc_Absyn_Const_e_struct _tmp5AD;_tmp5AD.tag=0;_tmp5AD.f1=(
void*)((void*)0);_tmp5AD;});_tmp5AC;}),loc);{void*_tmp591=Cyc_Tcutil_compress(t);
void*_tmp592;void*_tmp593;void*_tmp594;void*_tmp595;_LL757: if((unsigned int)
_tmp591 > 3?*((int*)_tmp591)== 4: 0){goto _LL758;}else{goto _LL759;}_LL759: if((
unsigned int)_tmp591 > 3?*((int*)_tmp591)== 5: 0){_LL764: _tmp593=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp591)->f1;goto _LL763;_LL763: _tmp592=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp591)->f2;if((int)_tmp592 == 0){goto _LL75A;}else{
goto _LL75B;}}else{goto _LL75B;}_LL75B: if((unsigned int)_tmp591 > 3?*((int*)_tmp591)
== 5: 0){_LL766: _tmp595=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp591)->f1;
goto _LL765;_LL765: _tmp594=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp591)->f2;
goto _LL75C;}else{goto _LL75D;}_LL75D: if((int)_tmp591 == 1){goto _LL75E;}else{goto
_LL75F;}_LL75F: if((unsigned int)_tmp591 > 3?*((int*)_tmp591)== 6: 0){goto _LL760;}
else{goto _LL761;}_LL761: goto _LL762;_LL758: goto _LL756;_LL75A:(void*)(e->r=(void*)((
void*)({struct Cyc_Absyn_Const_e_struct*_tmp596=_cycalloc(sizeof(*_tmp596));
_tmp596[0]=({struct Cyc_Absyn_Const_e_struct _tmp597;_tmp597.tag=0;_tmp597.f1=(
void*)((void*)({struct Cyc_Absyn_Char_c_struct*_tmp598=_cycalloc(sizeof(*_tmp598));
_tmp598[0]=({struct Cyc_Absyn_Char_c_struct _tmp599;_tmp599.tag=0;_tmp599.f1=(void*)((
void*)0);_tmp599.f2='\000';_tmp599;});_tmp598;}));_tmp597;});_tmp596;})));goto
_LL756;_LL75C:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A[0]=({struct Cyc_Absyn_Const_e_struct
_tmp59B;_tmp59B.tag=0;_tmp59B.f1=(void*)((void*)({struct Cyc_Absyn_Int_c_struct*
_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C[0]=({struct Cyc_Absyn_Int_c_struct
_tmp59D;_tmp59D.tag=2;_tmp59D.f1=(void*)_tmp595;_tmp59D.f2=0;_tmp59D;});_tmp59C;}));
_tmp59B;});_tmp59A;})));if(_tmp594 != (void*)2){e=Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Cast_e_struct*_tmp59E=_cycalloc(sizeof(*_tmp59E));_tmp59E[0]=({
struct Cyc_Absyn_Cast_e_struct _tmp59F;_tmp59F.tag=13;_tmp59F.f1=(void*)t;_tmp59F.f2=
e;_tmp59F;});_tmp59E;}),loc);}goto _LL756;_LL75E:(void*)(e->r=(void*)((void*)({
struct Cyc_Absyn_Const_e_struct*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));_tmp5A0[0]=({
struct Cyc_Absyn_Const_e_struct _tmp5A1;_tmp5A1.tag=0;_tmp5A1.f1=(void*)((void*)({
struct Cyc_Absyn_Float_c_struct*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2[0]=({
struct Cyc_Absyn_Float_c_struct _tmp5A3;_tmp5A3.tag=4;_tmp5A3.f1=_tag_arr("0.0",
sizeof(unsigned char),4);_tmp5A3;});_tmp5A2;}));_tmp5A1;});_tmp5A0;})));goto
_LL756;_LL760:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*
_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4[0]=({struct Cyc_Absyn_Cast_e_struct
_tmp5A5;_tmp5A5.tag=13;_tmp5A5.f1=(void*)t;_tmp5A5.f2=Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Const_e_struct*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));_tmp5A6[0]=({
struct Cyc_Absyn_Const_e_struct _tmp5A7;_tmp5A7.tag=0;_tmp5A7.f1=(void*)((void*)({
struct Cyc_Absyn_Float_c_struct*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8[0]=({
struct Cyc_Absyn_Float_c_struct _tmp5A9;_tmp5A9.tag=4;_tmp5A9.f1=_tag_arr("0.0",
sizeof(unsigned char),4);_tmp5A9;});_tmp5A8;}));_tmp5A7;});_tmp5A6;}),loc);
_tmp5A5;});_tmp5A4;})));goto _LL756;_LL762:({struct Cyc_Std_String_pa_struct
_tmp5AB;_tmp5AB.tag=0;_tmp5AB.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp5AA[1]={& _tmp5AB};Cyc_Tcutil_terr(loc,_tag_arr("declaration of type %s requires initializer",
sizeof(unsigned char),44),_tag_arr(_tmp5AA,sizeof(void*),1));}});goto _LL756;
_LL756:;}return e;}struct _tuple5*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,
struct Cyc_Absyn_Tvar*tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple5*
_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->f1=tv;_tmp5AE->f2=Cyc_Absyn_new_evar(({
struct Cyc_Core_Opt*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->v=(void*)k;
_tmp5AF;}),({struct Cyc_Core_Opt*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->v=s;
_tmp5B0;}));_tmp5AE;});}struct _tuple5*Cyc_Tcutil_r_make_inst_var(struct _tuple6*
env,struct Cyc_Absyn_Tvar*tv){struct _tuple6 _tmp5B2;struct _RegionHandle*_tmp5B3;
struct Cyc_List_List*_tmp5B4;struct _tuple6*_tmp5B1=env;_tmp5B2=*_tmp5B1;_LL769:
_tmp5B4=_tmp5B2.f1;goto _LL768;_LL768: _tmp5B3=_tmp5B2.f2;goto _LL767;_LL767: {void*
k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple5*_tmp5B5=_region_malloc(_tmp5B3,
sizeof(*_tmp5B5));_tmp5B5->f1=tv;_tmp5B5->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6->v=(void*)k;_tmp5B6;}),({struct Cyc_Core_Opt*
_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->v=_tmp5B4;_tmp5B7;}));_tmp5B5;});}}
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=
tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){if(Cyc_Std_zstrptrcmp(((struct Cyc_Absyn_Tvar*)
tvs2->hd)->name,tv->name)== 0){void*k1=(void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=(void*)tv->kind;if(! Cyc_Tcutil_constrain_kinds(k1,k2)){({struct Cyc_Std_String_pa_struct
_tmp5BB;_tmp5BB.tag=0;_tmp5BB.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string(
k2);{struct Cyc_Std_String_pa_struct _tmp5BA;_tmp5BA.tag=0;_tmp5BA.f1=(struct
_tagged_arr)Cyc_Absynpp_kindbound2string(k1);{struct Cyc_Std_String_pa_struct
_tmp5B9;_tmp5B9.tag=0;_tmp5B9.f1=(struct _tagged_arr)*tv->name;{void*_tmp5B8[3]={&
_tmp5B9,& _tmp5BA,& _tmp5BB};Cyc_Tcutil_terr(loc,_tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof(unsigned char),59),_tag_arr(_tmp5B8,sizeof(void*),3));}}}});}if(tv->identity
== 0){tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;}else{if(*((int*)
_check_null(tv->identity))!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))){({
void*_tmp5BC[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("same type variable has different identity!",sizeof(unsigned char),43),
_tag_arr(_tmp5BC,sizeof(void*),0));});}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD->hd=tv;
_tmp5BD->tl=tvs;_tmp5BD;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0){({void*
_tmp5BE[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tv",sizeof(unsigned char),39),
_tag_arr(_tmp5BE,sizeof(void*),0));});}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2
!= 0;tvs2=tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0){({void*
_tmp5BF[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tvs2",sizeof(unsigned char),41),
_tag_arr(_tmp5BF,sizeof(void*),0));});}if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity))== *((int*)_check_null(tv->identity))){return tvs;}}}return({
struct Cyc_List_List*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0->hd=tv;_tmp5C0->tl=
tvs;_tmp5C0;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*
tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0){({struct Cyc_Std_String_pa_struct
_tmp5C2;_tmp5C2.tag=0;_tmp5C2.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string(tv);{
void*_tmp5C1[1]={& _tmp5C2};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("bound tvar id for %s is NULL",sizeof(unsigned char),
29),_tag_arr(_tmp5C1,sizeof(void*),1));}});}return({struct Cyc_List_List*_tmp5C3=
_cycalloc(sizeof(*_tmp5C3));_tmp5C3->hd=tv;_tmp5C3->tl=tvs;_tmp5C3;});}static
struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct Cyc_List_List*es,void*e){void*
_tmp5C4=Cyc_Tcutil_compress(e);int _tmp5C5;_LL76B: if((unsigned int)_tmp5C4 > 3?*((
int*)_tmp5C4)== 0: 0){_LL76F: _tmp5C5=((struct Cyc_Absyn_Evar_struct*)_tmp5C4)->f3;
goto _LL76C;}else{goto _LL76D;}_LL76D: goto _LL76E;_LL76C:{struct Cyc_List_List*es2=
es;for(0;es2 != 0;es2=es2->tl){void*_tmp5C6=Cyc_Tcutil_compress((void*)es2->hd);
int _tmp5C7;_LL771: if((unsigned int)_tmp5C6 > 3?*((int*)_tmp5C6)== 0: 0){_LL775:
_tmp5C7=((struct Cyc_Absyn_Evar_struct*)_tmp5C6)->f3;goto _LL772;}else{goto _LL773;}
_LL773: goto _LL774;_LL772: if(_tmp5C5 == _tmp5C7){return es;}goto _LL770;_LL774: goto
_LL770;_LL770:;}}return({struct Cyc_List_List*_tmp5C8=_cycalloc(sizeof(*_tmp5C8));
_tmp5C8->hd=(void*)e;_tmp5C8->tl=es;_tmp5C8;});_LL76E: return es;_LL76A:;}static
struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*tvs,struct
Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int
present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(*((int*)
_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)_check_null(((
struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(! present){r=({
struct Cyc_List_List*_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9->hd=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp5C9->tl=r;_tmp5C9;});}}r=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_tagged_arr*fn){if(width != 0){unsigned int w=0;if(! Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)_check_null(width))){({struct Cyc_Std_String_pa_struct
_tmp5CB;_tmp5CB.tag=0;_tmp5CB.f1=(struct _tagged_arr)*fn;{void*_tmp5CA[1]={&
_tmp5CB};Cyc_Tcutil_terr(loc,_tag_arr("bitfield %s does not have constant width",
sizeof(unsigned char),41),_tag_arr(_tmp5CA,sizeof(void*),1));}});}else{w=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(width));}{void*_tmp5CC=Cyc_Tcutil_compress(
field_typ);void*_tmp5CD;_LL777: if((unsigned int)_tmp5CC > 3?*((int*)_tmp5CC)== 5:
0){_LL77B: _tmp5CD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5CC)->f2;goto
_LL778;}else{goto _LL779;}_LL779: goto _LL77A;_LL778:{void*_tmp5CE=_tmp5CD;_LL77D:
if((int)_tmp5CE == 0){goto _LL77E;}else{goto _LL77F;}_LL77F: if((int)_tmp5CE == 1){
goto _LL780;}else{goto _LL781;}_LL781: if((int)_tmp5CE == 2){goto _LL782;}else{goto
_LL783;}_LL783: if((int)_tmp5CE == 3){goto _LL784;}else{goto _LL77C;}_LL77E: if(w > 8){({
void*_tmp5CF[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(unsigned char),26),_tag_arr(_tmp5CF,sizeof(void*),0));});}goto _LL77C;
_LL780: if(w > 16){({void*_tmp5D0[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(unsigned char),26),_tag_arr(_tmp5D0,sizeof(void*),0));});}goto _LL77C;
_LL782: if(w > 32){({void*_tmp5D1[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(unsigned char),26),_tag_arr(_tmp5D1,sizeof(void*),0));});}goto _LL77C;
_LL784: if(w > 64){({void*_tmp5D2[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(unsigned char),26),_tag_arr(_tmp5D2,sizeof(void*),0));});}goto _LL77C;
_LL77C:;}goto _LL776;_LL77A:({struct Cyc_Std_String_pa_struct _tmp5D5;_tmp5D5.tag=0;
_tmp5D5.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(field_typ);{struct Cyc_Std_String_pa_struct
_tmp5D4;_tmp5D4.tag=0;_tmp5D4.f1=(struct _tagged_arr)*fn;{void*_tmp5D3[2]={&
_tmp5D4,& _tmp5D5};Cyc_Tcutil_terr(loc,_tag_arr("bitfield %s must have integral type but has type %s",
sizeof(unsigned char),52),_tag_arr(_tmp5D3,sizeof(void*),2));}}});goto _LL776;
_LL776:;}}}static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,
struct _tagged_arr*fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){
void*_tmp5D6=(void*)atts->hd;_LL786: if((int)_tmp5D6 == 5){goto _LL787;}else{goto
_LL788;}_LL788: if((unsigned int)_tmp5D6 > 16?*((int*)_tmp5D6)== 1: 0){goto _LL789;}
else{goto _LL78A;}_LL78A: goto _LL78B;_LL787: continue;_LL789: continue;_LL78B:({
struct Cyc_Std_String_pa_struct _tmp5D9;_tmp5D9.tag=0;_tmp5D9.f1=(struct
_tagged_arr)*fn;{struct Cyc_Std_String_pa_struct _tmp5D8;_tmp5D8.tag=0;_tmp5D8.f1=(
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd);{void*_tmp5D7[2]={&
_tmp5D8,& _tmp5D9};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s on member %s",
sizeof(unsigned char),30),_tag_arr(_tmp5D7,sizeof(void*),2));}}});goto _LL785;
_LL785:;}}struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*kind_env;struct Cyc_List_List*
free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
struct _tuple17{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,void*expected_kind,void*t){{void*_tmp5DA=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*
_tmp5DB;struct Cyc_Core_Opt**_tmp5DC;struct Cyc_Core_Opt*_tmp5DD;struct Cyc_Core_Opt**
_tmp5DE;struct Cyc_Absyn_Tvar*_tmp5DF;struct Cyc_List_List*_tmp5E0;struct Cyc_Absyn_Enumdecl*
_tmp5E1;struct Cyc_Absyn_Enumdecl**_tmp5E2;struct _tuple1*_tmp5E3;struct Cyc_Absyn_TunionInfo
_tmp5E4;void*_tmp5E5;struct Cyc_List_List*_tmp5E6;struct Cyc_List_List**_tmp5E7;
void*_tmp5E8;void**_tmp5E9;struct Cyc_Absyn_TunionFieldInfo _tmp5EA;struct Cyc_List_List*
_tmp5EB;void*_tmp5EC;void**_tmp5ED;struct Cyc_Absyn_PtrInfo _tmp5EE;struct Cyc_Absyn_Conref*
_tmp5EF;struct Cyc_Absyn_Tqual _tmp5F0;struct Cyc_Absyn_Conref*_tmp5F1;void*_tmp5F2;
void*_tmp5F3;void*_tmp5F4;struct Cyc_Absyn_Exp*_tmp5F5;struct Cyc_Absyn_Tqual
_tmp5F6;void*_tmp5F7;struct Cyc_Absyn_FnInfo _tmp5F8;struct Cyc_List_List*_tmp5F9;
struct Cyc_List_List*_tmp5FA;struct Cyc_Absyn_VarargInfo*_tmp5FB;int _tmp5FC;struct
Cyc_List_List*_tmp5FD;void*_tmp5FE;struct Cyc_Core_Opt*_tmp5FF;struct Cyc_Core_Opt**
_tmp600;struct Cyc_List_List*_tmp601;struct Cyc_List_List**_tmp602;struct Cyc_List_List*
_tmp603;struct Cyc_List_List*_tmp604;struct Cyc_List_List*_tmp605;struct Cyc_Absyn_Structdecl**
_tmp606;struct Cyc_Absyn_Structdecl***_tmp607;struct Cyc_List_List*_tmp608;struct
Cyc_List_List**_tmp609;struct _tuple1*_tmp60A;struct Cyc_Absyn_Uniondecl**_tmp60B;
struct Cyc_Absyn_Uniondecl***_tmp60C;struct Cyc_List_List*_tmp60D;struct Cyc_List_List**
_tmp60E;struct _tuple1*_tmp60F;struct Cyc_Core_Opt*_tmp610;struct Cyc_Core_Opt**
_tmp611;struct Cyc_List_List*_tmp612;struct Cyc_List_List**_tmp613;struct _tuple1*
_tmp614;void*_tmp615;void*_tmp616;void*_tmp617;struct Cyc_List_List*_tmp618;
_LL78D: if((int)_tmp5DA == 0){goto _LL78E;}else{goto _LL78F;}_LL78F: if((unsigned int)
_tmp5DA > 3?*((int*)_tmp5DA)== 0: 0){_LL7C0: _tmp5DD=((struct Cyc_Absyn_Evar_struct*)
_tmp5DA)->f1;_tmp5DE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp5DA)->f1;goto _LL7BF;_LL7BF: _tmp5DB=((struct Cyc_Absyn_Evar_struct*)_tmp5DA)->f2;
_tmp5DC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp5DA)->f2;goto
_LL790;}else{goto _LL791;}_LL791: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 1:
0){_LL7C1: _tmp5DF=((struct Cyc_Absyn_VarType_struct*)_tmp5DA)->f1;goto _LL792;}
else{goto _LL793;}_LL793: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 16: 0){
_LL7C2: _tmp5E0=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp5DA)->f1;goto _LL794;}
else{goto _LL795;}_LL795: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 12: 0){
_LL7C4: _tmp5E3=((struct Cyc_Absyn_EnumType_struct*)_tmp5DA)->f1;goto _LL7C3;_LL7C3:
_tmp5E1=((struct Cyc_Absyn_EnumType_struct*)_tmp5DA)->f2;_tmp5E2=(struct Cyc_Absyn_Enumdecl**)&((
struct Cyc_Absyn_EnumType_struct*)_tmp5DA)->f2;goto _LL796;}else{goto _LL797;}
_LL797: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 2: 0){_LL7C5: _tmp5E4=((
struct Cyc_Absyn_TunionType_struct*)_tmp5DA)->f1;_LL7C8: _tmp5E8=(void*)_tmp5E4.tunion_info;
_tmp5E9=(void**)&(((struct Cyc_Absyn_TunionType_struct*)_tmp5DA)->f1).tunion_info;
goto _LL7C7;_LL7C7: _tmp5E6=_tmp5E4.targs;_tmp5E7=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp5DA)->f1).targs;goto _LL7C6;_LL7C6: _tmp5E5=(
void*)_tmp5E4.rgn;goto _LL798;}else{goto _LL799;}_LL799: if((unsigned int)_tmp5DA > 
3?*((int*)_tmp5DA)== 3: 0){_LL7C9: _tmp5EA=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5DA)->f1;_LL7CB: _tmp5EC=(void*)_tmp5EA.field_info;_tmp5ED=(void**)&(((struct
Cyc_Absyn_TunionFieldType_struct*)_tmp5DA)->f1).field_info;goto _LL7CA;_LL7CA:
_tmp5EB=_tmp5EA.targs;goto _LL79A;}else{goto _LL79B;}_LL79B: if((unsigned int)
_tmp5DA > 3?*((int*)_tmp5DA)== 4: 0){_LL7CC: _tmp5EE=((struct Cyc_Absyn_PointerType_struct*)
_tmp5DA)->f1;_LL7D1: _tmp5F3=(void*)_tmp5EE.elt_typ;goto _LL7D0;_LL7D0: _tmp5F2=(
void*)_tmp5EE.rgn_typ;goto _LL7CF;_LL7CF: _tmp5F1=_tmp5EE.nullable;goto _LL7CE;
_LL7CE: _tmp5F0=_tmp5EE.tq;goto _LL7CD;_LL7CD: _tmp5EF=_tmp5EE.bounds;goto _LL79C;}
else{goto _LL79D;}_LL79D: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 13: 0){
_LL7D2: _tmp5F4=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp5DA)->f1;goto
_LL79E;}else{goto _LL79F;}_LL79F: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 5:
0){goto _LL7A0;}else{goto _LL7A1;}_LL7A1: if((int)_tmp5DA == 1){goto _LL7A2;}else{
goto _LL7A3;}_LL7A3: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 6: 0){goto
_LL7A4;}else{goto _LL7A5;}_LL7A5: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 7:
0){_LL7D5: _tmp5F7=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp5DA)->f1;goto
_LL7D4;_LL7D4: _tmp5F6=((struct Cyc_Absyn_ArrayType_struct*)_tmp5DA)->f2;goto
_LL7D3;_LL7D3: _tmp5F5=((struct Cyc_Absyn_ArrayType_struct*)_tmp5DA)->f3;goto
_LL7A6;}else{goto _LL7A7;}_LL7A7: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 8:
0){_LL7D6: _tmp5F8=((struct Cyc_Absyn_FnType_struct*)_tmp5DA)->f1;_LL7DE: _tmp601=
_tmp5F8.tvars;_tmp602=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp5DA)->f1).tvars;goto _LL7DD;_LL7DD: _tmp5FF=_tmp5F8.effect;_tmp600=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp5DA)->f1).effect;goto _LL7DC;_LL7DC: _tmp5FE=(
void*)_tmp5F8.ret_typ;goto _LL7DB;_LL7DB: _tmp5FD=_tmp5F8.args;goto _LL7DA;_LL7DA:
_tmp5FC=_tmp5F8.c_varargs;goto _LL7D9;_LL7D9: _tmp5FB=_tmp5F8.cyc_varargs;goto
_LL7D8;_LL7D8: _tmp5FA=_tmp5F8.rgn_po;goto _LL7D7;_LL7D7: _tmp5F9=_tmp5F8.attributes;
goto _LL7A8;}else{goto _LL7A9;}_LL7A9: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)
== 9: 0){_LL7DF: _tmp603=((struct Cyc_Absyn_TupleType_struct*)_tmp5DA)->f1;goto
_LL7AA;}else{goto _LL7AB;}_LL7AB: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 
14: 0){_LL7E0: _tmp604=((struct Cyc_Absyn_AnonStructType_struct*)_tmp5DA)->f1;goto
_LL7AC;}else{goto _LL7AD;}_LL7AD: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 
15: 0){_LL7E1: _tmp605=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp5DA)->f1;goto
_LL7AE;}else{goto _LL7AF;}_LL7AF: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 
10: 0){_LL7E4: _tmp60A=((struct Cyc_Absyn_StructType_struct*)_tmp5DA)->f1;goto
_LL7E3;_LL7E3: _tmp608=((struct Cyc_Absyn_StructType_struct*)_tmp5DA)->f2;_tmp609=(
struct Cyc_List_List**)&((struct Cyc_Absyn_StructType_struct*)_tmp5DA)->f2;goto
_LL7E2;_LL7E2: _tmp606=((struct Cyc_Absyn_StructType_struct*)_tmp5DA)->f3;_tmp607=(
struct Cyc_Absyn_Structdecl***)&((struct Cyc_Absyn_StructType_struct*)_tmp5DA)->f3;
goto _LL7B0;}else{goto _LL7B1;}_LL7B1: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)
== 11: 0){_LL7E7: _tmp60F=((struct Cyc_Absyn_UnionType_struct*)_tmp5DA)->f1;goto
_LL7E6;_LL7E6: _tmp60D=((struct Cyc_Absyn_UnionType_struct*)_tmp5DA)->f2;_tmp60E=(
struct Cyc_List_List**)&((struct Cyc_Absyn_UnionType_struct*)_tmp5DA)->f2;goto
_LL7E5;_LL7E5: _tmp60B=((struct Cyc_Absyn_UnionType_struct*)_tmp5DA)->f3;_tmp60C=(
struct Cyc_Absyn_Uniondecl***)&((struct Cyc_Absyn_UnionType_struct*)_tmp5DA)->f3;
goto _LL7B2;}else{goto _LL7B3;}_LL7B3: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)
== 18: 0){_LL7EA: _tmp614=((struct Cyc_Absyn_TypedefType_struct*)_tmp5DA)->f1;goto
_LL7E9;_LL7E9: _tmp612=((struct Cyc_Absyn_TypedefType_struct*)_tmp5DA)->f2;_tmp613=(
struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp5DA)->f2;goto
_LL7E8;_LL7E8: _tmp610=((struct Cyc_Absyn_TypedefType_struct*)_tmp5DA)->f3;_tmp611=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp5DA)->f3;goto
_LL7B4;}else{goto _LL7B5;}_LL7B5: if((int)_tmp5DA == 2){goto _LL7B6;}else{goto _LL7B7;}
_LL7B7: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 17: 0){_LL7EB: _tmp615=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp5DA)->f1;goto _LL7B8;}else{goto _LL7B9;}
_LL7B9: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 19: 0){_LL7EC: _tmp616=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp5DA)->f1;goto _LL7BA;}else{goto _LL7BB;}
_LL7BB: if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 21: 0){_LL7ED: _tmp617=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp5DA)->f1;goto _LL7BC;}else{goto _LL7BD;}_LL7BD:
if((unsigned int)_tmp5DA > 3?*((int*)_tmp5DA)== 20: 0){_LL7EE: _tmp618=((struct Cyc_Absyn_JoinEff_struct*)
_tmp5DA)->f1;goto _LL7BE;}else{goto _LL78C;}_LL78E: goto _LL78C;_LL790: if(*_tmp5DE == 
0){*_tmp5DE=({struct Cyc_Core_Opt*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->v=(
void*)expected_kind;_tmp619;});}if((cvtenv.fn_result? cvtenv.generalize_evars: 0)?
expected_kind == (void*)3: 0){*_tmp5DC=({struct Cyc_Core_Opt*_tmp61A=_cycalloc(
sizeof(*_tmp61A));_tmp61A->v=(void*)((void*)2);_tmp61A;});}else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp61B=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp620;_tmp620.tag=2;_tmp620.f1=0;_tmp620.f2=(void*)expected_kind;_tmp620;});
_tmp61F;}));*_tmp5DC=({struct Cyc_Core_Opt*_tmp61C=_cycalloc(sizeof(*_tmp61C));
_tmp61C->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp61D=_cycalloc(
sizeof(*_tmp61D));_tmp61D[0]=({struct Cyc_Absyn_VarType_struct _tmp61E;_tmp61E.tag=
1;_tmp61E.f1=_tmp61B;_tmp61E;});_tmp61D;}));_tmp61C;});_tmp5DF=_tmp61B;goto
_LL792;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL78C;_LL792:{void*_tmp621=Cyc_Absyn_compress_kb((void*)_tmp5DF->kind);
struct Cyc_Core_Opt*_tmp622;struct Cyc_Core_Opt**_tmp623;_LL7F0: if(*((int*)_tmp621)
== 1){_LL7F4: _tmp622=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp621)->f1;_tmp623=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp621)->f1;goto
_LL7F1;}else{goto _LL7F2;}_LL7F2: goto _LL7F3;_LL7F1:*_tmp623=({struct Cyc_Core_Opt*
_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp626;_tmp626.tag=2;_tmp626.f1=0;_tmp626.f2=(void*)expected_kind;_tmp626;});
_tmp625;}));_tmp624;});goto _LL7EF;_LL7F3: goto _LL7EF;_LL7EF:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp5DF);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp5DF);goto _LL78C;_LL794: {struct Cyc_Tcenv_Genv*ge=((struct
Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);{struct _RegionHandle _tmp627=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmp627;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmp5E0 != 0;_tmp5E0=_tmp5E0->tl){
struct Cyc_Absyn_Enumfield*_tmp628=(struct Cyc_Absyn_Enumfield*)_tmp5E0->hd;if(((
int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*
l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,prev_fields,(*_tmp628->name).f2)){({
struct Cyc_Std_String_pa_struct _tmp62A;_tmp62A.tag=0;_tmp62A.f1=(struct
_tagged_arr)*(*_tmp628->name).f2;{void*_tmp629[1]={& _tmp62A};Cyc_Tcutil_terr(
_tmp628->loc,_tag_arr("duplicate enum field name %s",sizeof(unsigned char),29),
_tag_arr(_tmp629,sizeof(void*),1));}});}else{prev_fields=({struct Cyc_List_List*
_tmp62B=_region_malloc(uprev_rgn,sizeof(*_tmp62B));_tmp62B->hd=(*_tmp628->name).f2;
_tmp62B->tl=prev_fields;_tmp62B;});}if(_tmp628->tag == 0){_tmp628->tag=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(tag_count,_tmp628->loc);}else{if(! Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)_check_null(_tmp628->tag))){({struct Cyc_Std_String_pa_struct
_tmp62D;_tmp62D.tag=0;_tmp62D.f1=(struct _tagged_arr)*(*_tmp628->name).f2;{void*
_tmp62C[1]={& _tmp62D};Cyc_Tcutil_terr(loc,_tag_arr("enum field %s: expression is not constant",
sizeof(unsigned char),42),_tag_arr(_tmp62C,sizeof(void*),1));}});}}{unsigned int
t1=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp628->tag));
tag_count=t1 + 1;(*_tmp628->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp62F;_tmp62F.tag=1;_tmp62F.f1=te->ns;_tmp62F;});_tmp62E;});ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple17*
v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp628->name).f2,({struct _tuple17*_tmp630=
_cycalloc(sizeof(*_tmp630));_tmp630->f1=(void*)({struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631[0]=({struct Cyc_Tcenv_AnonEnumRes_struct
_tmp632;_tmp632.tag=4;_tmp632.f1=(void*)t;_tmp632.f2=_tmp628;_tmp632;});_tmp631;});
_tmp630->f2=1;_tmp630;}));}}};_pop_region(uprev_rgn);}goto _LL78C;}_LL796: if(*
_tmp5E2 == 0){struct _handler_cons _tmp633;_push_handler(& _tmp633);{int _tmp635=0;
if(setjmp(_tmp633.handler)){_tmp635=1;}if(! _tmp635){{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp5E3);*_tmp5E2=(struct Cyc_Absyn_Enumdecl*)*
ed;};_pop_handler();}else{void*_tmp634=(void*)_exn_thrown;void*_tmp637=_tmp634;
_LL7F6: if(_tmp637 == Cyc_Dict_Absent){goto _LL7F7;}else{goto _LL7F8;}_LL7F8: goto
_LL7F9;_LL7F7: {struct Cyc_Tcenv_Genv*_tmp638=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*
_tmp639=({struct Cyc_Absyn_Enumdecl*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->sc=(
void*)((void*)3);_tmp63A->name=_tmp5E3;_tmp63A->fields=0;_tmp63A;});Cyc_Tc_tcEnumdecl(
te,_tmp638,loc,_tmp639);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmp5E3);*_tmp5E2=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL7F5;}}_LL7F9:(
void)_throw(_tmp637);_LL7F5:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmp5E2);*_tmp5E3=(ed->name)[_check_known_subscript_notnull(1,0)];
goto _LL78C;}_LL798: {struct Cyc_List_List*_tmp63B=*_tmp5E7;{void*_tmp63C=*_tmp5E9;
struct Cyc_Absyn_UnknownTunionInfo _tmp63D;int _tmp63E;struct _tuple1*_tmp63F;struct
Cyc_Absyn_Tuniondecl**_tmp640;struct Cyc_Absyn_Tuniondecl*_tmp641;_LL7FB: if(*((
int*)_tmp63C)== 0){_LL7FF: _tmp63D=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp63C)->f1;_LL801: _tmp63F=_tmp63D.name;goto _LL800;_LL800: _tmp63E=_tmp63D.is_xtunion;
goto _LL7FC;}else{goto _LL7FD;}_LL7FD: if(*((int*)_tmp63C)== 1){_LL802: _tmp640=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp63C)->f1;_tmp641=*_tmp640;goto _LL7FE;}
else{goto _LL7FA;}_LL7FC: {struct Cyc_Absyn_Tuniondecl**tudp;{struct _handler_cons
_tmp642;_push_handler(& _tmp642);{int _tmp644=0;if(setjmp(_tmp642.handler)){
_tmp644=1;}if(! _tmp644){tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp63F);;
_pop_handler();}else{void*_tmp643=(void*)_exn_thrown;void*_tmp646=_tmp643;_LL804:
if(_tmp646 == Cyc_Dict_Absent){goto _LL805;}else{goto _LL806;}_LL806: goto _LL807;
_LL805: {struct Cyc_Tcenv_Genv*_tmp647=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*
_tmp648=({struct Cyc_Absyn_Tuniondecl*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->sc=(
void*)((void*)3);_tmp64C->name=_tmp63F;_tmp64C->tvs=0;_tmp64C->fields=0;_tmp64C->is_xtunion=
_tmp63E;_tmp64C;});Cyc_Tc_tcTuniondecl(te,_tmp647,loc,_tmp648);tudp=Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp63F);if(_tmp63B != 0){({struct Cyc_Std_String_pa_struct _tmp64B;_tmp64B.tag=
0;_tmp64B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp63F);{struct Cyc_Std_String_pa_struct
_tmp64A;_tmp64A.tag=0;_tmp64A.f1=(struct _tagged_arr)(_tmp63E? _tag_arr("xtunion",
sizeof(unsigned char),8): _tag_arr("tunion",sizeof(unsigned char),7));{void*
_tmp649[2]={& _tmp64A,& _tmp64B};Cyc_Tcutil_terr(loc,_tag_arr("please declare parameterized %s %s before using",
sizeof(unsigned char),48),_tag_arr(_tmp649,sizeof(void*),2));}}});return cvtenv;}
goto _LL803;}_LL807:(void)_throw(_tmp646);_LL803:;}}}if((*tudp)->is_xtunion != 
_tmp63E){({struct Cyc_Std_String_pa_struct _tmp64E;_tmp64E.tag=0;_tmp64E.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp63F);{void*_tmp64D[1]={& _tmp64E};
Cyc_Tcutil_terr(loc,_tag_arr("[x]tunion is different from type declaration %s",
sizeof(unsigned char),48),_tag_arr(_tmp64D,sizeof(void*),1));}});}*_tmp5E9=(void*)({
struct Cyc_Absyn_KnownTunion_struct*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F[0]=({
struct Cyc_Absyn_KnownTunion_struct _tmp650;_tmp650.tag=1;_tmp650.f1=tudp;_tmp650;});
_tmp64F;});_tmp641=*tudp;goto _LL7FE;}_LL7FE: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp5E5);{struct Cyc_List_List*tvs=_tmp641->tvs;for(0;
_tmp63B != 0? tvs != 0: 0;(_tmp63B=_tmp63B->tl,tvs=tvs->tl)){void*t1=(void*)_tmp63B->hd;
void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp63B != 0){({struct Cyc_Std_String_pa_struct _tmp652;
_tmp652.tag=0;_tmp652.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp641->name);{
void*_tmp651[1]={& _tmp652};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s",
sizeof(unsigned char),38),_tag_arr(_tmp651,sizeof(void*),1));}});}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->hd=(void*)e;_tmp653->tl=
hidden_ts;_tmp653;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*
_tmp5E7=Cyc_List_imp_append(*_tmp5E7,Cyc_List_imp_rev(hidden_ts));}goto _LL7FA;}
_LL7FA:;}goto _LL78C;}_LL79A:{void*_tmp654=*_tmp5ED;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp655;int _tmp656;struct _tuple1*_tmp657;struct _tuple1*_tmp658;struct Cyc_Absyn_Tunionfield*
_tmp659;struct Cyc_Absyn_Tuniondecl*_tmp65A;_LL809: if(*((int*)_tmp654)== 0){
_LL80D: _tmp655=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp654)->f1;_LL810:
_tmp658=_tmp655.tunion_name;goto _LL80F;_LL80F: _tmp657=_tmp655.field_name;goto
_LL80E;_LL80E: _tmp656=_tmp655.is_xtunion;goto _LL80A;}else{goto _LL80B;}_LL80B: if(*((
int*)_tmp654)== 1){_LL812: _tmp65A=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp654)->f1;goto _LL811;_LL811: _tmp659=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp654)->f2;goto _LL80C;}else{goto _LL808;}_LL80A: {struct Cyc_Absyn_Tuniondecl*
tud;struct Cyc_Absyn_Tunionfield*tuf;{struct _handler_cons _tmp65B;_push_handler(&
_tmp65B);{int _tmp65D=0;if(setjmp(_tmp65B.handler)){_tmp65D=1;}if(! _tmp65D){*Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp658);;_pop_handler();}else{void*_tmp65C=(void*)_exn_thrown;void*
_tmp65F=_tmp65C;_LL814: if(_tmp65F == Cyc_Dict_Absent){goto _LL815;}else{goto _LL816;}
_LL816: goto _LL817;_LL815:({struct Cyc_Std_String_pa_struct _tmp661;_tmp661.tag=0;
_tmp661.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp658);{void*_tmp660[1]={&
_tmp661};Cyc_Tcutil_terr(loc,_tag_arr("unbound type tunion %s",sizeof(
unsigned char),23),_tag_arr(_tmp660,sizeof(void*),1));}});return cvtenv;_LL817:(
void)_throw(_tmp65F);_LL813:;}}}{struct _handler_cons _tmp662;_push_handler(&
_tmp662);{int _tmp664=0;if(setjmp(_tmp662.handler)){_tmp664=1;}if(! _tmp664){{void*
_tmp665=Cyc_Tcenv_lookup_ordinary(te,loc,_tmp657);struct Cyc_Absyn_Tunionfield*
_tmp666;struct Cyc_Absyn_Tuniondecl*_tmp667;_LL819: if(*((int*)_tmp665)== 2){
_LL81E: _tmp667=((struct Cyc_Tcenv_TunionRes_struct*)_tmp665)->f1;goto _LL81D;
_LL81D: _tmp666=((struct Cyc_Tcenv_TunionRes_struct*)_tmp665)->f2;goto _LL81A;}
else{goto _LL81B;}_LL81B: goto _LL81C;_LL81A: tuf=_tmp666;tud=_tmp667;if(tud->is_xtunion
!= _tmp656){({struct Cyc_Std_String_pa_struct _tmp669;_tmp669.tag=0;_tmp669.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp658);{void*_tmp668[1]={& _tmp669};
Cyc_Tcutil_terr(loc,_tag_arr("[x]tunion is different from type declaration %s",
sizeof(unsigned char),48),_tag_arr(_tmp668,sizeof(void*),1));}});}goto _LL818;
_LL81C:({struct Cyc_Std_String_pa_struct _tmp66C;_tmp66C.tag=0;_tmp66C.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp658);{struct Cyc_Std_String_pa_struct
_tmp66B;_tmp66B.tag=0;_tmp66B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp657);{void*_tmp66A[2]={& _tmp66B,& _tmp66C};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(unsigned char),35),_tag_arr(_tmp66A,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv
_tmp66D=cvtenv;_npop_handler(0);return _tmp66D;}_LL818:;};_pop_handler();}else{
void*_tmp663=(void*)_exn_thrown;void*_tmp66F=_tmp663;_LL820: if(_tmp66F == Cyc_Dict_Absent){
goto _LL821;}else{goto _LL822;}_LL822: goto _LL823;_LL821:({struct Cyc_Std_String_pa_struct
_tmp672;_tmp672.tag=0;_tmp672.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp658);{struct Cyc_Std_String_pa_struct _tmp671;_tmp671.tag=0;_tmp671.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp657);{void*_tmp670[2]={& _tmp671,& _tmp672};
Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",sizeof(
unsigned char),35),_tag_arr(_tmp670,sizeof(void*),2));}}});return cvtenv;_LL823:(
void)_throw(_tmp66F);_LL81F:;}}}*_tmp5ED=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp674;_tmp674.tag=1;_tmp674.f1=tud;_tmp674.f2=tuf;_tmp674;});_tmp673;});
_tmp65A=tud;_tmp659=tuf;goto _LL80C;}_LL80C: {struct Cyc_List_List*tvs=_tmp65A->tvs;
for(0;_tmp5EB != 0? tvs != 0: 0;(_tmp5EB=_tmp5EB->tl,tvs=tvs->tl)){void*t1=(void*)
_tmp5EB->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp5EB != 0){({struct Cyc_Std_String_pa_struct
_tmp677;_tmp677.tag=0;_tmp677.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp659->name);{struct Cyc_Std_String_pa_struct _tmp676;_tmp676.tag=0;_tmp676.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp65A->name);{void*_tmp675[2]={&
_tmp676,& _tmp677};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s.%s",
sizeof(unsigned char),41),_tag_arr(_tmp675,sizeof(void*),2));}}});}if(tvs != 0){({
struct Cyc_Std_String_pa_struct _tmp67A;_tmp67A.tag=0;_tmp67A.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp659->name);{struct Cyc_Std_String_pa_struct
_tmp679;_tmp679.tag=0;_tmp679.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp65A->name);{void*_tmp678[2]={& _tmp679,& _tmp67A};Cyc_Tcutil_terr(loc,_tag_arr("too few type arguments for tunion %s.%s",
sizeof(unsigned char),40),_tag_arr(_tmp678,sizeof(void*),2));}}});}goto _LL808;}
_LL808:;}goto _LL78C;_LL79C: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(
void*)0,_tmp5F3);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp5F2);{void*_tmp67B=(void*)(Cyc_Absyn_compress_conref(_tmp5EF))->v;void*
_tmp67C;struct Cyc_Absyn_Exp*_tmp67D;_LL825: if((unsigned int)_tmp67B > 1?*((int*)
_tmp67B)== 0: 0){_LL829: _tmp67C=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp67B)->f1;if((unsigned int)_tmp67C > 1?*((int*)_tmp67C)== 0: 0){_LL82A: _tmp67D=((
struct Cyc_Absyn_Upper_b_struct*)_tmp67C)->f1;goto _LL826;}else{goto _LL827;}}else{
goto _LL827;}_LL827: goto _LL828;_LL826: if(! Cyc_Tcutil_is_const_exp(te,_tmp67D)){({
void*_tmp67E[0]={};Cyc_Tcutil_terr(loc,_tag_arr("pointer bounds expression is not a constant",
sizeof(unsigned char),44),_tag_arr(_tmp67E,sizeof(void*),0));});}Cyc_Tcexp_tcExp(
te,0,_tmp67D);if(! Cyc_Tcutil_coerce_uint_typ(te,_tmp67D)){({void*_tmp67F[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("pointer bounds expression is not an unsigned int",
sizeof(unsigned char),49),_tag_arr(_tmp67F,sizeof(void*),0));});}Cyc_Evexp_eval_const_uint_exp(
_tmp67D);goto _LL824;_LL828: goto _LL824;_LL824:;}goto _LL78C;_LL79E: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp5F4);goto _LL78C;_LL7A0: goto _LL78C;_LL7A2: goto _LL78C;
_LL7A4: goto _LL78C;_LL7A6: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(
void*)1,_tmp5F7);if(_tmp5F5 == 0? 1: ! Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp5F5))){({void*_tmp680[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array bounds expression is not constant",
sizeof(unsigned char),40),_tag_arr(_tmp680,sizeof(void*),0));});}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp5F5));if(! Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmp5F5))){({void*_tmp681[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("array bounds expression is not an unsigned int",sizeof(
unsigned char),47),_tag_arr(_tmp681,sizeof(void*),0));});}Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5F5));goto _LL78C;_LL7A8: {int num_convs=0;
int seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(
void*)0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp5F9 != 0;_tmp5F9=_tmp5F9->tl){
if(! Cyc_Absyn_fntype_att((void*)_tmp5F9->hd)){({struct Cyc_Std_String_pa_struct
_tmp683;_tmp683.tag=0;_tmp683.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp5F9->hd);{void*_tmp682[1]={& _tmp683};Cyc_Tcutil_terr(loc,_tag_arr("bad function type attribute %s",
sizeof(unsigned char),31),_tag_arr(_tmp682,sizeof(void*),1));}});}{void*_tmp684=(
void*)_tmp5F9->hd;int _tmp685;int _tmp686;void*_tmp687;_LL82C: if((int)_tmp684 == 0){
goto _LL82D;}else{goto _LL82E;}_LL82E: if((int)_tmp684 == 1){goto _LL82F;}else{goto
_LL830;}_LL830: if((int)_tmp684 == 2){goto _LL831;}else{goto _LL832;}_LL832: if((
unsigned int)_tmp684 > 16?*((int*)_tmp684)== 3: 0){_LL838: _tmp687=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp684)->f1;goto _LL837;_LL837: _tmp686=((struct Cyc_Absyn_Format_att_struct*)
_tmp684)->f2;goto _LL836;_LL836: _tmp685=((struct Cyc_Absyn_Format_att_struct*)
_tmp684)->f3;goto _LL833;}else{goto _LL834;}_LL834: goto _LL835;_LL82D: if(!
seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL82B;_LL82F: if(! seen_cdecl){
seen_cdecl=1;++ num_convs;}goto _LL82B;_LL831: if(! seen_fastcall){seen_fastcall=1;
++ num_convs;}goto _LL82B;_LL833: if(! seen_format){seen_format=1;ft=_tmp687;
fmt_desc_arg=_tmp686;fmt_arg_start=_tmp685;}else{({void*_tmp688[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("function can't have multiple format attributes",sizeof(
unsigned char),47),_tag_arr(_tmp688,sizeof(void*),0));});}goto _LL82B;_LL835: goto
_LL82B;_LL82B:;}}if(num_convs > 1){({void*_tmp689[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("function can't have multiple calling conventions",sizeof(unsigned char),
49),_tag_arr(_tmp689,sizeof(void*),0));});}Cyc_Tcutil_check_unique_tvars(loc,*
_tmp602);{struct Cyc_List_List*b=*_tmp602;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)
b->hd)->identity=Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(
cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp68A=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp68B;_LL83A: if(*((int*)
_tmp68A)== 0){_LL83E: _tmp68B=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp68A)->f1;
if((int)_tmp68B == 1){goto _LL83B;}else{goto _LL83C;}}else{goto _LL83C;}_LL83C: goto
_LL83D;_LL83B:({struct Cyc_Std_String_pa_struct _tmp68D;_tmp68D.tag=0;_tmp68D.f1=(
struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name;{void*_tmp68C[1]={&
_tmp68D};Cyc_Tcutil_terr(loc,_tag_arr("function attempts to abstract Mem type variable %s",
sizeof(unsigned char),51),_tag_arr(_tmp68C,sizeof(void*),1));}});goto _LL839;
_LL83D: goto _LL839;_LL839:;}}}{struct Cyc_Tcutil_CVTEnv _tmp68E=({struct Cyc_Tcutil_CVTEnv
_tmp6FD;_tmp6FD.kind_env=cvtenv.kind_env;_tmp6FD.free_vars=0;_tmp6FD.free_evars=
0;_tmp6FD.generalize_evars=cvtenv.generalize_evars;_tmp6FD.fn_result=1;_tmp6FD;});
_tmp68E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp68E,(void*)1,_tmp5FE);_tmp68E.fn_result=
0;{struct Cyc_List_List*a=_tmp5FD;for(0;a != 0;a=a->tl){_tmp68E=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp68E,(void*)1,(*((struct _tuple2*)a->hd)).f3);}}if(_tmp5FB != 0){if(
_tmp5FC){({void*_tmp68F[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("both c_vararg and cyc_vararg",sizeof(unsigned char),
29),_tag_arr(_tmp68F,sizeof(void*),0));});}{int _tmp691;void*_tmp692;struct Cyc_Absyn_Tqual
_tmp693;struct Cyc_Core_Opt*_tmp694;struct Cyc_Absyn_VarargInfo _tmp690=*_tmp5FB;
_LL843: _tmp694=_tmp690.name;goto _LL842;_LL842: _tmp693=_tmp690.tq;goto _LL841;
_LL841: _tmp692=(void*)_tmp690.type;goto _LL840;_LL840: _tmp691=_tmp690.inject;goto
_LL83F;_LL83F: _tmp68E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp68E,(void*)1,
_tmp692);if(_tmp691){void*_tmp695=Cyc_Tcutil_compress(_tmp692);struct Cyc_Absyn_TunionInfo
_tmp696;void*_tmp697;void*_tmp698;_LL845: if((unsigned int)_tmp695 > 3?*((int*)
_tmp695)== 2: 0){_LL849: _tmp696=((struct Cyc_Absyn_TunionType_struct*)_tmp695)->f1;
_LL84B: _tmp698=(void*)_tmp696.tunion_info;if(*((int*)_tmp698)== 1){goto _LL84A;}
else{goto _LL847;}_LL84A: _tmp697=(void*)_tmp696.rgn;goto _LL846;}else{goto _LL847;}
_LL847: goto _LL848;_LL846: goto _LL844;_LL848:({void*_tmp699[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("can't inject a non-[x]tunion type",sizeof(unsigned char),34),
_tag_arr(_tmp699,sizeof(void*),0));});goto _LL844;_LL844:;}}}if(seen_format){int
_tmp69A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5FD);if((((
fmt_desc_arg < 0? 1: fmt_desc_arg > _tmp69A)? 1: fmt_arg_start < 0)? 1:(_tmp5FB == 0?
fmt_arg_start != 0: 0))? 1:(_tmp5FB != 0? fmt_arg_start != _tmp69A + 1: 0)){({void*
_tmp69B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad format descriptor",sizeof(
unsigned char),22),_tag_arr(_tmp69B,sizeof(void*),0));});}else{void*_tmp69D;
struct _tuple2 _tmp69C=*((struct _tuple2*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmp5FD,fmt_desc_arg - 1);_LL84D: _tmp69D=_tmp69C.f3;goto _LL84C;_LL84C:{void*
_tmp69E=Cyc_Tcutil_compress(_tmp69D);struct Cyc_Absyn_PtrInfo _tmp69F;struct Cyc_Absyn_Conref*
_tmp6A0;void*_tmp6A1;_LL84F: if((unsigned int)_tmp69E > 3?*((int*)_tmp69E)== 4: 0){
_LL853: _tmp69F=((struct Cyc_Absyn_PointerType_struct*)_tmp69E)->f1;_LL855: _tmp6A1=(
void*)_tmp69F.elt_typ;goto _LL854;_LL854: _tmp6A0=_tmp69F.bounds;goto _LL850;}else{
goto _LL851;}_LL851: goto _LL852;_LL850:{struct _tuple8 _tmp6A3=({struct _tuple8
_tmp6A2;_tmp6A2.f1=Cyc_Tcutil_compress(_tmp6A1);_tmp6A2.f2=Cyc_Absyn_conref_def((
void*)0,_tmp6A0);_tmp6A2;});void*_tmp6A4;void*_tmp6A5;void*_tmp6A6;void*_tmp6A7;
_LL857: _LL85C: _tmp6A5=_tmp6A3.f1;if((unsigned int)_tmp6A5 > 3?*((int*)_tmp6A5)== 
5: 0){_LL85E: _tmp6A7=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp6A5)->f1;if((
int)_tmp6A7 == 1){goto _LL85D;}else{goto _LL859;}_LL85D: _tmp6A6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp6A5)->f2;if((int)_tmp6A6 == 0){goto _LL85B;}else{goto _LL859;}}else{goto _LL859;}
_LL85B: _tmp6A4=_tmp6A3.f2;if((int)_tmp6A4 == 0){goto _LL858;}else{goto _LL859;}
_LL859: goto _LL85A;_LL858: goto _LL856;_LL85A:({void*_tmp6A8[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("format descriptor is not a char ? type",sizeof(unsigned char),39),
_tag_arr(_tmp6A8,sizeof(void*),0));});goto _LL856;_LL856:;}goto _LL84E;_LL852:({
void*_tmp6A9[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format descriptor is not a char ? type",
sizeof(unsigned char),39),_tag_arr(_tmp6A9,sizeof(void*),0));});goto _LL84E;
_LL84E:;}if(fmt_arg_start != 0){void*_tmp6AA=Cyc_Tcutil_compress((void*)((struct
Cyc_Absyn_VarargInfo*)_check_null(_tmp5FB))->type);int problem;{void*_tmp6AB=ft;
_LL860: if((int)_tmp6AB == 0){goto _LL861;}else{goto _LL862;}_LL862: if((int)_tmp6AB
== 1){goto _LL863;}else{goto _LL85F;}_LL861:{void*_tmp6AC=_tmp6AA;struct Cyc_Absyn_TunionInfo
_tmp6AD;void*_tmp6AE;struct Cyc_Absyn_Tuniondecl**_tmp6AF;struct Cyc_Absyn_Tuniondecl*
_tmp6B0;_LL865: if((unsigned int)_tmp6AC > 3?*((int*)_tmp6AC)== 2: 0){_LL869:
_tmp6AD=((struct Cyc_Absyn_TunionType_struct*)_tmp6AC)->f1;_LL86A: _tmp6AE=(void*)
_tmp6AD.tunion_info;if(*((int*)_tmp6AE)== 1){_LL86B: _tmp6AF=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp6AE)->f1;_tmp6B0=*_tmp6AF;goto _LL866;}else{goto _LL867;}}else{goto _LL867;}
_LL867: goto _LL868;_LL866: problem=Cyc_Absyn_qvar_cmp(_tmp6B0->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL864;_LL868: problem=1;goto _LL864;_LL864:;}goto _LL85F;_LL863:{void*
_tmp6B1=_tmp6AA;struct Cyc_Absyn_TunionInfo _tmp6B2;void*_tmp6B3;struct Cyc_Absyn_Tuniondecl**
_tmp6B4;struct Cyc_Absyn_Tuniondecl*_tmp6B5;_LL86D: if((unsigned int)_tmp6B1 > 3?*((
int*)_tmp6B1)== 2: 0){_LL871: _tmp6B2=((struct Cyc_Absyn_TunionType_struct*)_tmp6B1)->f1;
_LL872: _tmp6B3=(void*)_tmp6B2.tunion_info;if(*((int*)_tmp6B3)== 1){_LL873:
_tmp6B4=((struct Cyc_Absyn_KnownTunion_struct*)_tmp6B3)->f1;_tmp6B5=*_tmp6B4;goto
_LL86E;}else{goto _LL86F;}}else{goto _LL86F;}_LL86F: goto _LL870;_LL86E: problem=Cyc_Absyn_qvar_cmp(
_tmp6B5->name,Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL86C;_LL870: problem=1;
goto _LL86C;_LL86C:;}goto _LL85F;_LL85F:;}if(problem){({void*_tmp6B6[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("format attribute and vararg types don't match",sizeof(
unsigned char),46),_tag_arr(_tmp6B6,sizeof(void*),0));});}}}}{struct Cyc_List_List*
rpo=_tmp5FA;for(0;rpo != 0;rpo=rpo->tl){struct _tuple8 _tmp6B8;void*_tmp6B9;void*
_tmp6BA;struct _tuple8*_tmp6B7=(struct _tuple8*)rpo->hd;_tmp6B8=*_tmp6B7;_LL876:
_tmp6BA=_tmp6B8.f1;goto _LL875;_LL875: _tmp6B9=_tmp6B8.f2;goto _LL874;_LL874:
_tmp68E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp68E,(void*)3,_tmp6BA);_tmp68E=
Cyc_Tcutil_i_check_valid_type(loc,te,_tmp68E,(void*)3,_tmp6B9);}}if(*_tmp600 != 0){
_tmp68E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp68E,(void*)4,(void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp600))->v);}else{if(cvtenv.generalize_evars){;}{struct Cyc_List_List*
effect=0;{struct Cyc_List_List*tvs=_tmp68E.free_vars;for(0;tvs != 0;tvs=tvs->tl){
void*_tmp6BB=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);
void*_tmp6BC;struct Cyc_Core_Opt*_tmp6BD;struct Cyc_Core_Opt**_tmp6BE;void*_tmp6BF;
void*_tmp6C0;struct Cyc_Core_Opt*_tmp6C1;struct Cyc_Core_Opt**_tmp6C2;void*_tmp6C3;
struct Cyc_Core_Opt*_tmp6C4;struct Cyc_Core_Opt**_tmp6C5;_LL878: if(*((int*)_tmp6BB)
== 2){_LL885: _tmp6BD=((struct Cyc_Absyn_Less_kb_struct*)_tmp6BB)->f1;_tmp6BE=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp6BB)->f1;goto _LL884;
_LL884: _tmp6BC=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6BB)->f2;if((int)
_tmp6BC == 3){goto _LL879;}else{goto _LL87A;}}else{goto _LL87A;}_LL87A: if(*((int*)
_tmp6BB)== 0){_LL886: _tmp6BF=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp6BB)->f1;
if((int)_tmp6BF == 3){goto _LL87B;}else{goto _LL87C;}}else{goto _LL87C;}_LL87C: if(*((
int*)_tmp6BB)== 2){_LL888: _tmp6C1=((struct Cyc_Absyn_Less_kb_struct*)_tmp6BB)->f1;
_tmp6C2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp6BB)->f1;
goto _LL887;_LL887: _tmp6C0=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp6BB)->f2;
if((int)_tmp6C0 == 4){goto _LL87D;}else{goto _LL87E;}}else{goto _LL87E;}_LL87E: if(*((
int*)_tmp6BB)== 0){_LL889: _tmp6C3=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp6BB)->f1;
if((int)_tmp6C3 == 4){goto _LL87F;}else{goto _LL880;}}else{goto _LL880;}_LL880: if(*((
int*)_tmp6BB)== 1){_LL88A: _tmp6C4=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp6BB)->f1;
_tmp6C5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp6BB)->f1;
goto _LL881;}else{goto _LL882;}_LL882: goto _LL883;_LL879:*_tmp6BE=({struct Cyc_Core_Opt*
_tmp6C6=_cycalloc(sizeof(*_tmp6C6));_tmp6C6->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp6C8;_tmp6C8.tag=0;_tmp6C8.f1=(void*)((void*)3);_tmp6C8;});_tmp6C7;}));
_tmp6C6;});goto _LL87B;_LL87B: effect=({struct Cyc_List_List*_tmp6C9=_cycalloc(
sizeof(*_tmp6C9));_tmp6C9->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp6CA=_cycalloc(sizeof(*_tmp6CA));_tmp6CA[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp6CB;_tmp6CB.tag=19;_tmp6CB.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp6CC=_cycalloc(sizeof(*_tmp6CC));_tmp6CC[0]=({struct Cyc_Absyn_VarType_struct
_tmp6CD;_tmp6CD.tag=1;_tmp6CD.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6CD;});
_tmp6CC;}));_tmp6CB;});_tmp6CA;}));_tmp6C9->tl=effect;_tmp6C9;});goto _LL877;
_LL87D:*_tmp6C2=({struct Cyc_Core_Opt*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));_tmp6CE->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp6CF=_cycalloc(sizeof(*_tmp6CF));
_tmp6CF[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp6D0;_tmp6D0.tag=0;_tmp6D0.f1=(void*)((
void*)4);_tmp6D0;});_tmp6CF;}));_tmp6CE;});goto _LL87F;_LL87F: effect=({struct Cyc_List_List*
_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=({struct Cyc_Absyn_VarType_struct
_tmp6D3;_tmp6D3.tag=1;_tmp6D3.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6D3;});
_tmp6D2;}));_tmp6D1->tl=effect;_tmp6D1;});goto _LL877;_LL881:*_tmp6C5=({struct Cyc_Core_Opt*
_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp6D6;_tmp6D6.tag=2;_tmp6D6.f1=0;_tmp6D6.f2=(void*)((void*)0);_tmp6D6;});
_tmp6D5;}));_tmp6D4;});goto _LL883;_LL883: effect=({struct Cyc_List_List*_tmp6D7=
_cycalloc(sizeof(*_tmp6D7));_tmp6D7->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp6D8=_cycalloc(sizeof(*_tmp6D8));_tmp6D8[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp6D9;_tmp6D9.tag=21;_tmp6D9.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp6DA=_cycalloc(sizeof(*_tmp6DA));_tmp6DA[0]=({struct Cyc_Absyn_VarType_struct
_tmp6DB;_tmp6DB.tag=1;_tmp6DB.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6DB;});
_tmp6DA;}));_tmp6D9;});_tmp6D8;}));_tmp6D7->tl=effect;_tmp6D7;});goto _LL877;
_LL877:;}}effect=Cyc_List_imp_rev(effect);{struct Cyc_List_List*ts=_tmp68E.free_evars;
for(0;ts != 0;ts=ts->tl){void*_tmp6DC=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL88C:
if((int)_tmp6DC == 3){goto _LL88D;}else{goto _LL88E;}_LL88E: if((int)_tmp6DC == 4){
goto _LL88F;}else{goto _LL890;}_LL890: goto _LL891;_LL88D: effect=({struct Cyc_List_List*
_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp6DF;_tmp6DF.tag=19;_tmp6DF.f1=(void*)((void*)ts->hd);_tmp6DF;});_tmp6DE;}));
_tmp6DD->tl=effect;_tmp6DD;});goto _LL88B;_LL88F: effect=({struct Cyc_List_List*
_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0->hd=(void*)((void*)ts->hd);_tmp6E0->tl=
effect;_tmp6E0;});goto _LL88B;_LL891: effect=({struct Cyc_List_List*_tmp6E1=
_cycalloc(sizeof(*_tmp6E1));_tmp6E1->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp6E2=_cycalloc(sizeof(*_tmp6E2));_tmp6E2[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp6E3;_tmp6E3.tag=21;_tmp6E3.f1=(void*)((void*)ts->hd);_tmp6E3;});_tmp6E2;}));
_tmp6E1->tl=effect;_tmp6E1;});goto _LL88B;_LL88B:;}}*_tmp600=({struct Cyc_Core_Opt*
_tmp6E4=_cycalloc(sizeof(*_tmp6E4));_tmp6E4->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6E5=_cycalloc(sizeof(*_tmp6E5));_tmp6E5[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6E6;_tmp6E6.tag=20;_tmp6E6.f1=Cyc_List_imp_rev(effect);_tmp6E6;});_tmp6E5;}));
_tmp6E4;});}}if(*_tmp602 != 0){struct Cyc_List_List*bs=*_tmp602;for(0;bs != 0;bs=bs->tl){
void*_tmp6E7=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp6E8;struct Cyc_Core_Opt**_tmp6E9;void*_tmp6EA;struct Cyc_Core_Opt*
_tmp6EB;struct Cyc_Core_Opt**_tmp6EC;void*_tmp6ED;struct Cyc_Core_Opt*_tmp6EE;
struct Cyc_Core_Opt**_tmp6EF;void*_tmp6F0;struct Cyc_Core_Opt*_tmp6F1;struct Cyc_Core_Opt**
_tmp6F2;void*_tmp6F3;_LL893: if(*((int*)_tmp6E7)== 1){_LL89F: _tmp6E8=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp6E7)->f1;_tmp6E9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp6E7)->f1;goto _LL894;}else{goto _LL895;}_LL895: if(*((int*)_tmp6E7)== 2){_LL8A1:
_tmp6EB=((struct Cyc_Absyn_Less_kb_struct*)_tmp6E7)->f1;_tmp6EC=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp6E7)->f1;goto _LL8A0;_LL8A0: _tmp6EA=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp6E7)->f2;if((int)_tmp6EA == 1){goto _LL896;}
else{goto _LL897;}}else{goto _LL897;}_LL897: if(*((int*)_tmp6E7)== 2){_LL8A3:
_tmp6EE=((struct Cyc_Absyn_Less_kb_struct*)_tmp6E7)->f1;_tmp6EF=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp6E7)->f1;goto _LL8A2;_LL8A2: _tmp6ED=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp6E7)->f2;if((int)_tmp6ED == 0){goto _LL898;}
else{goto _LL899;}}else{goto _LL899;}_LL899: if(*((int*)_tmp6E7)== 2){_LL8A5:
_tmp6F1=((struct Cyc_Absyn_Less_kb_struct*)_tmp6E7)->f1;_tmp6F2=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp6E7)->f1;goto _LL8A4;_LL8A4: _tmp6F0=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp6E7)->f2;goto _LL89A;}else{goto _LL89B;}_LL89B:
if(*((int*)_tmp6E7)== 0){_LL8A6: _tmp6F3=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp6E7)->f1;if((int)_tmp6F3 == 1){goto _LL89C;}else{goto _LL89D;}}else{goto _LL89D;}
_LL89D: goto _LL89E;_LL894:({struct Cyc_Std_String_pa_struct _tmp6F5;_tmp6F5.tag=0;
_tmp6F5.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name;{void*
_tmp6F4[1]={& _tmp6F5};Cyc_Tcutil_warn(loc,_tag_arr("Type variable %s unconstrained, assuming boxed",
sizeof(unsigned char),47),_tag_arr(_tmp6F4,sizeof(void*),1));}});_tmp6EC=_tmp6E9;
goto _LL896;_LL896: _tmp6EF=_tmp6EC;goto _LL898;_LL898:*_tmp6EF=({struct Cyc_Core_Opt*
_tmp6F6=_cycalloc(sizeof(*_tmp6F6));_tmp6F6->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp6F7=_cycalloc(sizeof(*_tmp6F7));_tmp6F7[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp6F8;_tmp6F8.tag=0;_tmp6F8.f1=(void*)((void*)2);_tmp6F8;});_tmp6F7;}));
_tmp6F6;});goto _LL892;_LL89A:*_tmp6F2=({struct Cyc_Core_Opt*_tmp6F9=_cycalloc(
sizeof(*_tmp6F9));_tmp6F9->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp6FB;_tmp6FB.tag=0;_tmp6FB.f1=(void*)_tmp6F0;_tmp6FB;});_tmp6FA;}));_tmp6F9;});
goto _LL892;_LL89C:({void*_tmp6FC[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function's can't abstract M types",
sizeof(unsigned char),34),_tag_arr(_tmp6FC,sizeof(void*),0));});goto _LL892;
_LL89E: goto _LL892;_LL892:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp68E.kind_env,*_tmp602);_tmp68E.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp68E.free_vars,*_tmp602);{struct Cyc_List_List*tvs=_tmp68E.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp68E.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL78C;}}_LL7AA: for(0;_tmp603 != 0;_tmp603=_tmp603->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,(*((struct _tuple4*)
_tmp603->hd)).f2);}goto _LL78C;_LL7AC:{struct _RegionHandle _tmp6FE=_new_region("sprev_rgn");
struct _RegionHandle*sprev_rgn=& _tmp6FE;_push_region(sprev_rgn);{struct Cyc_List_List*
prev_fields=0;for(0;_tmp604 != 0;_tmp604=_tmp604->tl){struct Cyc_Absyn_Structfield
_tmp700;struct Cyc_List_List*_tmp701;struct Cyc_Absyn_Exp*_tmp702;void*_tmp703;
struct Cyc_Absyn_Tqual _tmp704;struct _tagged_arr*_tmp705;struct Cyc_Absyn_Structfield*
_tmp6FF=(struct Cyc_Absyn_Structfield*)_tmp604->hd;_tmp700=*_tmp6FF;_LL8AC:
_tmp705=_tmp700.name;goto _LL8AB;_LL8AB: _tmp704=_tmp700.tq;goto _LL8AA;_LL8AA:
_tmp703=(void*)_tmp700.type;goto _LL8A9;_LL8A9: _tmp702=_tmp700.width;goto _LL8A8;
_LL8A8: _tmp701=_tmp700.attributes;goto _LL8A7;_LL8A7: if(((int(*)(int(*compare)(
struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))
Cyc_List_mem)(Cyc_Std_zstrptrcmp,prev_fields,_tmp705)){({struct Cyc_Std_String_pa_struct
_tmp707;_tmp707.tag=0;_tmp707.f1=(struct _tagged_arr)*_tmp705;{void*_tmp706[1]={&
_tmp707};Cyc_Tcutil_terr(loc,_tag_arr("duplicate field %s in struct",sizeof(
unsigned char),29),_tag_arr(_tmp706,sizeof(void*),1));}});}if(Cyc_Std_strcmp(*
_tmp705,_tag_arr("",sizeof(unsigned char),1))!= 0){prev_fields=({struct Cyc_List_List*
_tmp708=_region_malloc(sprev_rgn,sizeof(*_tmp708));_tmp708->hd=_tmp705;_tmp708->tl=
prev_fields;_tmp708;});}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
1,_tmp703);Cyc_Tcutil_check_bitfield(loc,te,_tmp703,_tmp702,_tmp705);Cyc_Tcutil_check_field_atts(
loc,_tmp705,_tmp701);}};_pop_region(sprev_rgn);}goto _LL78C;_LL7AE:{struct
_RegionHandle _tmp709=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=&
_tmp709;_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;for(0;_tmp605
!= 0;_tmp605=_tmp605->tl){struct Cyc_Absyn_Structfield _tmp70B;struct Cyc_List_List*
_tmp70C;struct Cyc_Absyn_Exp*_tmp70D;void*_tmp70E;struct Cyc_Absyn_Tqual _tmp70F;
struct _tagged_arr*_tmp710;struct Cyc_Absyn_Structfield*_tmp70A=(struct Cyc_Absyn_Structfield*)
_tmp605->hd;_tmp70B=*_tmp70A;_LL8B2: _tmp710=_tmp70B.name;goto _LL8B1;_LL8B1:
_tmp70F=_tmp70B.tq;goto _LL8B0;_LL8B0: _tmp70E=(void*)_tmp70B.type;goto _LL8AF;
_LL8AF: _tmp70D=_tmp70B.width;goto _LL8AE;_LL8AE: _tmp70C=_tmp70B.attributes;goto
_LL8AD;_LL8AD: if(((int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),
struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,
prev_fields,_tmp710)){({struct Cyc_Std_String_pa_struct _tmp712;_tmp712.tag=0;
_tmp712.f1=(struct _tagged_arr)*_tmp710;{void*_tmp711[1]={& _tmp712};Cyc_Tcutil_terr(
loc,_tag_arr("duplicate field %s in struct",sizeof(unsigned char),29),_tag_arr(
_tmp711,sizeof(void*),1));}});}if(Cyc_Std_strcmp(*_tmp710,_tag_arr("",sizeof(
unsigned char),1))!= 0){prev_fields=({struct Cyc_List_List*_tmp713=_region_malloc(
uprev_rgn,sizeof(*_tmp713));_tmp713->hd=_tmp710;_tmp713->tl=prev_fields;_tmp713;});}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,_tmp70E);if(! Cyc_Tcutil_bits_only(
_tmp70E)){({struct Cyc_Std_String_pa_struct _tmp715;_tmp715.tag=0;_tmp715.f1=(
struct _tagged_arr)*_tmp710;{void*_tmp714[1]={& _tmp715};Cyc_Tcutil_terr(loc,
_tag_arr("%s has a type that is possibly incompatible with other members of union",
sizeof(unsigned char),72),_tag_arr(_tmp714,sizeof(void*),1));}});}Cyc_Tcutil_check_bitfield(
loc,te,_tmp70E,_tmp70D,_tmp710);Cyc_Tcutil_check_field_atts(loc,_tmp710,_tmp70C);}};
_pop_region(uprev_rgn);}goto _LL78C;_LL7B0: if(_tmp60A == 0){if(*_tmp607 == 0){({
void*_tmp716[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad anonymous struct",sizeof(
unsigned char),21),_tag_arr(_tmp716,sizeof(void*),0));});return cvtenv;}{struct
Cyc_Absyn_Structdecl*sd=*((struct Cyc_Absyn_Structdecl**)_check_null(*_tmp607));;}}
if(*_tmp607 == 0){struct _handler_cons _tmp717;_push_handler(& _tmp717);{int _tmp719=
0;if(setjmp(_tmp717.handler)){_tmp719=1;}if(! _tmp719){*_tmp607=(struct Cyc_Absyn_Structdecl**)
Cyc_Tcenv_lookup_structdecl(te,loc,(struct _tuple1*)_check_null(_tmp60A));;
_pop_handler();}else{void*_tmp718=(void*)_exn_thrown;void*_tmp71B=_tmp718;_LL8B4:
if(_tmp71B == Cyc_Dict_Absent){goto _LL8B5;}else{goto _LL8B6;}_LL8B6: goto _LL8B7;
_LL8B5: {struct _tuple1*tdn=(struct _tuple1*)_check_null(_tmp60A);struct Cyc_Tcenv_Genv*
_tmp71C=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Structdecl*_tmp71D=({struct Cyc_Absyn_Structdecl*
_tmp720=_cycalloc(sizeof(*_tmp720));_tmp720->sc=(void*)((void*)3);_tmp720->name=({
struct Cyc_Core_Opt*_tmp721=_cycalloc(sizeof(*_tmp721));_tmp721->v=tdn;_tmp721;});
_tmp720->tvs=0;_tmp720->fields=0;_tmp720->attributes=0;_tmp720;});Cyc_Tc_tcStructdecl(
te,_tmp71C,loc,_tmp71D);*_tmp607=(struct Cyc_Absyn_Structdecl**)Cyc_Tcenv_lookup_structdecl(
te,loc,tdn);if(*_tmp609 != 0){({struct Cyc_Std_String_pa_struct _tmp71F;_tmp71F.tag=
0;_tmp71F.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tdn);{void*_tmp71E[1]={&
_tmp71F};Cyc_Tcutil_terr(loc,_tag_arr("please declare parameterized struct %s before using",
sizeof(unsigned char),52),_tag_arr(_tmp71E,sizeof(void*),1));}});return cvtenv;}
goto _LL8B3;}_LL8B7:(void)_throw(_tmp71B);_LL8B3:;}}}{struct Cyc_Absyn_Structdecl*
sd=*((struct Cyc_Absyn_Structdecl**)_check_null(*_tmp607));if(sd->name != 0){*((
struct _tuple1*)_check_null(_tmp60A))=((struct _tuple1*)((struct Cyc_Core_Opt*)
_check_null(sd->name))->v)[_check_known_subscript_notnull(1,0)];}{struct Cyc_List_List*
tvs=sd->tvs;struct Cyc_List_List*ts=*_tmp609;for(0;ts != 0? tvs != 0: 0;(ts=ts->tl,
tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd);}if(ts != 0){({
struct Cyc_Std_String_pa_struct _tmp723;_tmp723.tag=0;_tmp723.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string((struct _tuple1*)_check_null(_tmp60A));{void*
_tmp722[1]={& _tmp723};Cyc_Tcutil_terr(loc,_tag_arr("too many parameters for struct %s",
sizeof(unsigned char),34),_tag_arr(_tmp722,sizeof(void*),1));}});}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp724=_cycalloc(sizeof(*_tmp724));_tmp724->hd=(void*)e;_tmp724->tl=
hidden_ts;_tmp724;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*
_tmp609=Cyc_List_imp_append(*_tmp609,Cyc_List_imp_rev(hidden_ts));}goto _LL78C;}}
_LL7B2: if(_tmp60F == 0){({void*_tmp725[0]={};Cyc_Tcutil_terr(loc,_tag_arr("missing union name",
sizeof(unsigned char),19),_tag_arr(_tmp725,sizeof(void*),0));});return cvtenv;}
if(*_tmp60C == 0){struct _handler_cons _tmp726;_push_handler(& _tmp726);{int _tmp728=
0;if(setjmp(_tmp726.handler)){_tmp728=1;}if(! _tmp728){*_tmp60C=(struct Cyc_Absyn_Uniondecl**)
Cyc_Tcenv_lookup_uniondecl(te,loc,(struct _tuple1*)_check_null(_tmp60F));;
_pop_handler();}else{void*_tmp727=(void*)_exn_thrown;void*_tmp72A=_tmp727;_LL8B9:
if(_tmp72A == Cyc_Dict_Absent){goto _LL8BA;}else{goto _LL8BB;}_LL8BB: goto _LL8BC;
_LL8BA: {struct _tuple1*_tmp72B=(struct _tuple1*)_check_null(_tmp60F);struct Cyc_Tcenv_Genv*
_tmp72C=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Uniondecl*_tmp72D=({struct Cyc_Absyn_Uniondecl*
_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730->sc=(void*)((void*)3);_tmp730->name=({
struct Cyc_Core_Opt*_tmp731=_cycalloc(sizeof(*_tmp731));_tmp731->v=_tmp72B;
_tmp731;});_tmp730->tvs=0;_tmp730->fields=0;_tmp730->attributes=0;_tmp730;});Cyc_Tc_tcUniondecl(
te,_tmp72C,loc,_tmp72D);*_tmp60C=(struct Cyc_Absyn_Uniondecl**)Cyc_Tcenv_lookup_uniondecl(
te,loc,_tmp72B);if(*_tmp60E != 0){({struct Cyc_Std_String_pa_struct _tmp72F;_tmp72F.tag=
0;_tmp72F.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp72B);{void*_tmp72E[1]={&
_tmp72F};Cyc_Tcutil_terr(loc,_tag_arr("please declare parameterized union %s before using",
sizeof(unsigned char),51),_tag_arr(_tmp72E,sizeof(void*),1));}});return cvtenv;}
goto _LL8B8;}_LL8BC:(void)_throw(_tmp72A);_LL8B8:;}}}{struct Cyc_Absyn_Uniondecl*
ud=*((struct Cyc_Absyn_Uniondecl**)_check_null(*_tmp60C));if(ud->name != 0){*
_tmp60F=((struct _tuple1*)((struct Cyc_Core_Opt*)_check_null(ud->name))->v)[
_check_known_subscript_notnull(1,0)];}{struct Cyc_List_List*tvs=ud->tvs;struct Cyc_List_List*
ts=*_tmp60E;for(0;ts != 0? tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd);}if(ts != 0){({struct Cyc_Std_String_pa_struct _tmp733;
_tmp733.tag=0;_tmp733.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((struct
_tuple1*)_check_null(_tmp60F));{void*_tmp732[1]={& _tmp733};Cyc_Tcutil_terr(loc,
_tag_arr("too many parameters for union %s",sizeof(unsigned char),33),_tag_arr(
_tmp732,sizeof(void*),1));}});}if(tvs != 0){struct Cyc_List_List*hidden_ts=0;for(0;
tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);
void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp734=_cycalloc(
sizeof(*_tmp734));_tmp734->hd=(void*)e;_tmp734->tl=hidden_ts;_tmp734;});cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*_tmp60E=Cyc_List_imp_append(*
_tmp60E,Cyc_List_imp_rev(hidden_ts));}goto _LL78C;}}_LL7B4: {struct Cyc_List_List*
targs=*_tmp613;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp735;
_push_handler(& _tmp735);{int _tmp737=0;if(setjmp(_tmp735.handler)){_tmp737=1;}if(
! _tmp737){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp614);;_pop_handler();}else{
void*_tmp736=(void*)_exn_thrown;void*_tmp739=_tmp736;_LL8BE: if(_tmp739 == Cyc_Dict_Absent){
goto _LL8BF;}else{goto _LL8C0;}_LL8C0: goto _LL8C1;_LL8BF:({struct Cyc_Std_String_pa_struct
_tmp73B;_tmp73B.tag=0;_tmp73B.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp614);{void*_tmp73A[1]={& _tmp73B};Cyc_Tcutil_terr(loc,_tag_arr("unbound typedef name %s",
sizeof(unsigned char),24),_tag_arr(_tmp73A,sizeof(void*),1));}});return cvtenv;
_LL8C1:(void)_throw(_tmp739);_LL8BD:;}}}_tmp614[0]=(td->name)[
_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*
ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0? tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);inst=({struct Cyc_List_List*_tmp73C=_cycalloc(
sizeof(*_tmp73C));_tmp73C->hd=({struct _tuple5*_tmp73D=_cycalloc(sizeof(*_tmp73D));
_tmp73D->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp73D->f2=(void*)ts->hd;_tmp73D;});
_tmp73C->tl=inst;_tmp73C;});}if(ts != 0){({struct Cyc_Std_String_pa_struct _tmp73F;
_tmp73F.tag=0;_tmp73F.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp614);{
void*_tmp73E[1]={& _tmp73F};Cyc_Tcutil_terr(loc,_tag_arr("too many parameters for typedef %s",
sizeof(unsigned char),35),_tag_arr(_tmp73E,sizeof(void*),1));}});}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp740=_cycalloc(sizeof(*_tmp740));_tmp740->hd=(void*)e;_tmp740->tl=
hidden_ts;_tmp740;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);
inst=({struct Cyc_List_List*_tmp741=_cycalloc(sizeof(*_tmp741));_tmp741->hd=({
struct _tuple5*_tmp742=_cycalloc(sizeof(*_tmp742));_tmp742->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp742->f2=e;_tmp742;});_tmp741->tl=inst;_tmp741;});}*_tmp613=Cyc_List_imp_append(
targs,Cyc_List_imp_rev(hidden_ts));}{void*new_typ=Cyc_Tcutil_substitute(inst,(
void*)td->defn);*_tmp611=({struct Cyc_Core_Opt*_tmp743=_cycalloc(sizeof(*_tmp743));
_tmp743->v=(void*)new_typ;_tmp743;});goto _LL78C;}}}_LL7B6: goto _LL78C;_LL7B8:
_tmp616=_tmp615;goto _LL7BA;_LL7BA: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp616);goto _LL78C;_LL7BC: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp617);goto _LL78C;_LL7BE: for(0;_tmp618 != 0;_tmp618=
_tmp618->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)
_tmp618->hd);}goto _LL78C;_LL78C:;}if(! Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),
expected_kind)){({struct Cyc_Std_String_pa_struct _tmp747;_tmp747.tag=0;_tmp747.f1=(
struct _tagged_arr)Cyc_Absynpp_kind2string(expected_kind);{struct Cyc_Std_String_pa_struct
_tmp746;_tmp746.tag=0;_tmp746.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(
t));{struct Cyc_Std_String_pa_struct _tmp745;_tmp745.tag=0;_tmp745.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp744[3]={& _tmp745,& _tmp746,&
_tmp747};Cyc_Tcutil_terr(loc,_tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof(unsigned char),51),_tag_arr(_tmp744,sizeof(void*),3));}}}});}return cvtenv;}
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*kind_env,void*expected_kind,int
generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp748=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp751;_tmp751.kind_env=kind_env;_tmp751.free_vars=
0;_tmp751.free_evars=0;_tmp751.generalize_evars=generalize_evars;_tmp751.fn_result=
0;_tmp751;}),expected_kind,t);{struct Cyc_List_List*vs=_tmp748.free_vars;for(0;vs
!= 0;vs=vs->tl){_tmp748.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(struct
Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp748.free_evars;for(0;evs
!= 0;evs=evs->tl){void*_tmp749=Cyc_Tcutil_compress((void*)evs->hd);struct Cyc_Core_Opt*
_tmp74A;struct Cyc_Core_Opt**_tmp74B;_LL8C3: if((unsigned int)_tmp749 > 3?*((int*)
_tmp749)== 0: 0){_LL8C7: _tmp74A=((struct Cyc_Absyn_Evar_struct*)_tmp749)->f4;
_tmp74B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp749)->f4;goto
_LL8C4;}else{goto _LL8C5;}_LL8C5: goto _LL8C6;_LL8C4: if(*_tmp74B == 0){*_tmp74B=({
struct Cyc_Core_Opt*_tmp74C=_cycalloc(sizeof(*_tmp74C));_tmp74C->v=kind_env;
_tmp74C;});}else{struct Cyc_List_List*_tmp74D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp74B))->v;struct Cyc_List_List*_tmp74E=0;for(0;_tmp74D != 0;
_tmp74D=_tmp74D->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
kind_env,(struct Cyc_Absyn_Tvar*)_tmp74D->hd)){_tmp74E=({struct Cyc_List_List*
_tmp74F=_cycalloc(sizeof(*_tmp74F));_tmp74F->hd=(struct Cyc_Absyn_Tvar*)_tmp74D->hd;
_tmp74F->tl=_tmp74E;_tmp74F;});}}*_tmp74B=({struct Cyc_Core_Opt*_tmp750=_cycalloc(
sizeof(*_tmp750));_tmp750->v=_tmp74E;_tmp750;});}goto _LL8C2;_LL8C6: goto _LL8C2;
_LL8C2:;}}return _tmp748;}void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_Tcutil_CVTEnv _tmp752=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp753=_tmp752.free_vars;struct Cyc_List_List*
_tmp754=_tmp752.free_evars;{struct Cyc_List_List*x=_tmp753;for(0;x != 0;x=x->tl){
void*_tmp755=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp756;struct Cyc_Core_Opt**_tmp757;void*_tmp758;struct Cyc_Core_Opt*
_tmp759;struct Cyc_Core_Opt**_tmp75A;void*_tmp75B;struct Cyc_Core_Opt*_tmp75C;
struct Cyc_Core_Opt**_tmp75D;void*_tmp75E;struct Cyc_Core_Opt*_tmp75F;struct Cyc_Core_Opt**
_tmp760;void*_tmp761;_LL8C9: if(*((int*)_tmp755)== 1){_LL8D5: _tmp756=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp755)->f1;_tmp757=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp755)->f1;goto _LL8CA;}else{goto _LL8CB;}_LL8CB: if(*((int*)_tmp755)== 2){_LL8D7:
_tmp759=((struct Cyc_Absyn_Less_kb_struct*)_tmp755)->f1;_tmp75A=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp755)->f1;goto _LL8D6;_LL8D6: _tmp758=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp755)->f2;if((int)_tmp758 == 1){goto _LL8CC;}
else{goto _LL8CD;}}else{goto _LL8CD;}_LL8CD: if(*((int*)_tmp755)== 2){_LL8D9:
_tmp75C=((struct Cyc_Absyn_Less_kb_struct*)_tmp755)->f1;_tmp75D=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp755)->f1;goto _LL8D8;_LL8D8: _tmp75B=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp755)->f2;if((int)_tmp75B == 0){goto _LL8CE;}
else{goto _LL8CF;}}else{goto _LL8CF;}_LL8CF: if(*((int*)_tmp755)== 2){_LL8DB:
_tmp75F=((struct Cyc_Absyn_Less_kb_struct*)_tmp755)->f1;_tmp760=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp755)->f1;goto _LL8DA;_LL8DA: _tmp75E=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp755)->f2;goto _LL8D0;}else{goto _LL8D1;}_LL8D1:
if(*((int*)_tmp755)== 0){_LL8DC: _tmp761=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp755)->f1;if((int)_tmp761 == 1){goto _LL8D2;}else{goto _LL8D3;}}else{goto _LL8D3;}
_LL8D3: goto _LL8D4;_LL8CA: _tmp75A=_tmp757;goto _LL8CC;_LL8CC: _tmp75D=_tmp75A;goto
_LL8CE;_LL8CE:*_tmp75D=({struct Cyc_Core_Opt*_tmp762=_cycalloc(sizeof(*_tmp762));
_tmp762->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp763=_cycalloc(
sizeof(*_tmp763));_tmp763[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp764;_tmp764.tag=0;
_tmp764.f1=(void*)((void*)2);_tmp764;});_tmp763;}));_tmp762;});goto _LL8C8;_LL8D0:*
_tmp760=({struct Cyc_Core_Opt*_tmp765=_cycalloc(sizeof(*_tmp765));_tmp765->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp766=_cycalloc(sizeof(*_tmp766));
_tmp766[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp767;_tmp767.tag=0;_tmp767.f1=(void*)
_tmp75E;_tmp767;});_tmp766;}));_tmp765;});goto _LL8C8;_LL8D2:({struct Cyc_Std_String_pa_struct
_tmp769;_tmp769.tag=0;_tmp769.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)x->hd);{void*_tmp768[1]={& _tmp769};Cyc_Tcutil_terr(loc,
_tag_arr("type variable %s cannot have kind M",sizeof(unsigned char),36),
_tag_arr(_tmp768,sizeof(void*),1));}});goto _LL8C8;_LL8D4: goto _LL8C8;_LL8C8:;}}
if(_tmp753 != 0? 1: _tmp754 != 0){{void*_tmp76A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmp76B;struct Cyc_List_List*_tmp76C;struct Cyc_List_List*_tmp76D;struct Cyc_Absyn_VarargInfo*
_tmp76E;int _tmp76F;struct Cyc_List_List*_tmp770;void*_tmp771;struct Cyc_Core_Opt*
_tmp772;struct Cyc_List_List*_tmp773;struct Cyc_List_List**_tmp774;_LL8DE: if((
unsigned int)_tmp76A > 3?*((int*)_tmp76A)== 8: 0){_LL8E2: _tmp76B=((struct Cyc_Absyn_FnType_struct*)
_tmp76A)->f1;_LL8EA: _tmp773=_tmp76B.tvars;_tmp774=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp76A)->f1).tvars;goto _LL8E9;_LL8E9: _tmp772=
_tmp76B.effect;goto _LL8E8;_LL8E8: _tmp771=(void*)_tmp76B.ret_typ;goto _LL8E7;
_LL8E7: _tmp770=_tmp76B.args;goto _LL8E6;_LL8E6: _tmp76F=_tmp76B.c_varargs;goto
_LL8E5;_LL8E5: _tmp76E=_tmp76B.cyc_varargs;goto _LL8E4;_LL8E4: _tmp76D=_tmp76B.rgn_po;
goto _LL8E3;_LL8E3: _tmp76C=_tmp76B.attributes;goto _LL8DF;}else{goto _LL8E0;}_LL8E0:
goto _LL8E1;_LL8DF: if(*_tmp774 == 0){*_tmp774=_tmp753;_tmp753=0;}goto _LL8DD;_LL8E1:
goto _LL8DD;_LL8DD:;}if(_tmp753 != 0){({struct Cyc_Std_String_pa_struct _tmp776;
_tmp776.tag=0;_tmp776.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)_tmp753->hd)->name;{
void*_tmp775[1]={& _tmp776};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s",
sizeof(unsigned char),25),_tag_arr(_tmp775,sizeof(void*),1));}});}if(_tmp754 != 0){
for(0;_tmp754 != 0;_tmp754=_tmp754->tl){void*e=(void*)_tmp754->hd;void*_tmp777=
Cyc_Tcutil_typ_kind(e);_LL8EC: if((int)_tmp777 == 3){goto _LL8ED;}else{goto _LL8EE;}
_LL8EE: if((int)_tmp777 == 4){goto _LL8EF;}else{goto _LL8F0;}_LL8F0: goto _LL8F1;
_LL8ED: if(! Cyc_Tcutil_unify(e,(void*)2)){({void*_tmp778[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with heap!",
sizeof(unsigned char),28),_tag_arr(_tmp778,sizeof(void*),0));});}goto _LL8EB;
_LL8EF: if(! Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){({void*_tmp779[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with {}!",
sizeof(unsigned char),26),_tag_arr(_tmp779,sizeof(void*),0));});}goto _LL8EB;
_LL8F1:({struct Cyc_Std_String_pa_struct _tmp77C;_tmp77C.tag=0;_tmp77C.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp77B;
_tmp77B.tag=0;_tmp77B.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{void*
_tmp77A[2]={& _tmp77B,& _tmp77C};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(unsigned char),52),_tag_arr(_tmp77A,sizeof(void*),2));}}});goto _LL8EB;
_LL8EB:;}}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmp77D=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp77E;struct Cyc_List_List*_tmp77F;struct Cyc_List_List*
_tmp780;struct Cyc_Absyn_VarargInfo*_tmp781;int _tmp782;struct Cyc_List_List*
_tmp783;void*_tmp784;struct Cyc_Core_Opt*_tmp785;struct Cyc_List_List*_tmp786;
_LL8F3: if((unsigned int)_tmp77D > 3?*((int*)_tmp77D)== 8: 0){_LL8F7: _tmp77E=((
struct Cyc_Absyn_FnType_struct*)_tmp77D)->f1;_LL8FF: _tmp786=_tmp77E.tvars;goto
_LL8FE;_LL8FE: _tmp785=_tmp77E.effect;goto _LL8FD;_LL8FD: _tmp784=(void*)_tmp77E.ret_typ;
goto _LL8FC;_LL8FC: _tmp783=_tmp77E.args;goto _LL8FB;_LL8FB: _tmp782=_tmp77E.c_varargs;
goto _LL8FA;_LL8FA: _tmp781=_tmp77E.cyc_varargs;goto _LL8F9;_LL8F9: _tmp780=_tmp77E.rgn_po;
goto _LL8F8;_LL8F8: _tmp77F=_tmp77E.attributes;goto _LL8F4;}else{goto _LL8F5;}_LL8F5:
goto _LL8F6;_LL8F4: fd->tvs=_tmp786;fd->effect=_tmp785;goto _LL8F2;_LL8F6:({void*
_tmp787[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("check_fndecl_valid_type: not a FnType",sizeof(unsigned char),38),
_tag_arr(_tmp787,sizeof(void*),0));});return;_LL8F2:;}{struct _RegionHandle
_tmp788=_new_region("r");struct _RegionHandle*r=& _tmp788;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tagged_arr*(*f)(struct
_tuple13*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct
_tuple13*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,_tag_arr("function declaration has repeated parameter",
sizeof(unsigned char),44));;_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*
_tmp789=_cycalloc(sizeof(*_tmp789));_tmp789->v=(void*)t;_tmp789;});}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct Cyc_Tcutil_CVTEnv
_tmp78A=Cyc_Tcutil_check_valid_type(loc,te,bound_tvars,expected_kind,0,t);struct
Cyc_List_List*_tmp78B=Cyc_Tcutil_remove_bound_tvars(_tmp78A.free_vars,
bound_tvars);struct Cyc_List_List*_tmp78C=_tmp78A.free_evars;{struct Cyc_List_List*
fs=_tmp78B;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp78D=((struct Cyc_Absyn_Tvar*)
fs->hd)->name;({struct Cyc_Std_String_pa_struct _tmp790;_tmp790.tag=0;_tmp790.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp78F;_tmp78F.tag=0;_tmp78F.f1=(struct _tagged_arr)*_tmp78D;{void*_tmp78E[2]={&
_tmp78F,& _tmp790};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s in type %s",
sizeof(unsigned char),36),_tag_arr(_tmp78E,sizeof(void*),2));}}});}}if(!
allow_evars? _tmp78C != 0: 0){for(0;_tmp78C != 0;_tmp78C=_tmp78C->tl){void*e=(void*)
_tmp78C->hd;void*_tmp791=Cyc_Tcutil_typ_kind(e);_LL901: if((int)_tmp791 == 3){goto
_LL902;}else{goto _LL903;}_LL903: if((int)_tmp791 == 4){goto _LL904;}else{goto _LL905;}
_LL905: goto _LL906;_LL902: if(! Cyc_Tcutil_unify(e,(void*)2)){({void*_tmp792[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with heap!",
sizeof(unsigned char),28),_tag_arr(_tmp792,sizeof(void*),0));});}goto _LL900;
_LL904: if(! Cyc_Tcutil_unify(e,(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp793=
_cycalloc(sizeof(*_tmp793));_tmp793[0]=({struct Cyc_Absyn_JoinEff_struct _tmp794;
_tmp794.tag=20;_tmp794.f1=0;_tmp794;});_tmp793;}))){({void*_tmp795[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with {}!",
sizeof(unsigned char),26),_tag_arr(_tmp795,sizeof(void*),0));});}goto _LL900;
_LL906:({struct Cyc_Std_String_pa_struct _tmp798;_tmp798.tag=0;_tmp798.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp797;
_tmp797.tag=0;_tmp797.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{void*
_tmp796[2]={& _tmp797,& _tmp798};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(unsigned char),52),_tag_arr(_tmp796,sizeof(void*),2));}}});goto _LL900;
_LL900:;}}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity
== 0){tv->identity=Cyc_Tcutil_new_tvar_id();}}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _tagged_arr(*a2string)(void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(
void*)vs2->hd)== 0){({struct Cyc_Std_String_pa_struct _tmp79B;_tmp79B.tag=0;
_tmp79B.f1=(struct _tagged_arr)a2string((void*)vs->hd);{struct Cyc_Std_String_pa_struct
_tmp79A;_tmp79A.tag=0;_tmp79A.f1=(struct _tagged_arr)msg;{void*_tmp799[2]={&
_tmp79A,& _tmp79B};Cyc_Tcutil_terr(loc,_tag_arr("%s: %s",sizeof(unsigned char),7),
_tag_arr(_tmp799,sizeof(void*),2));}}});}}}}static struct _tagged_arr Cyc_Tcutil_strptr2string(
struct _tagged_arr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _tagged_arr msg){((void(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr(*a2string)(struct _tagged_arr*),struct _tagged_arr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Std_zstrptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){((void(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr(*a2string)(struct Cyc_Absyn_Tvar*),struct _tagged_arr msg))
Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,
_tag_arr("duplicate type variable",sizeof(unsigned char),24));}struct _tuple18{
struct Cyc_Absyn_Structfield*f1;int f2;};struct _tuple19{struct Cyc_List_List*f1;
void*f2;};struct _tuple20{struct Cyc_Absyn_Structfield*f1;void*f2;};struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){if(Cyc_Std_strcmp(*((struct Cyc_Absyn_Structfield*)sd_fields->hd)->name,
_tag_arr("",sizeof(unsigned char),1))!= 0){fields=({struct Cyc_List_List*_tmp79C=
_cycalloc(sizeof(*_tmp79C));_tmp79C->hd=({struct _tuple18*_tmp79D=_cycalloc(
sizeof(*_tmp79D));_tmp79D->f1=(struct Cyc_Absyn_Structfield*)sd_fields->hd;
_tmp79D->f2=0;_tmp79D;});_tmp79C->tl=fields;_tmp79C;});}}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple19 _tmp79F;void*_tmp7A0;struct Cyc_List_List*
_tmp7A1;struct _tuple19*_tmp79E=(struct _tuple19*)des->hd;_tmp79F=*_tmp79E;_LL909:
_tmp7A1=_tmp79F.f1;goto _LL908;_LL908: _tmp7A0=_tmp79F.f2;goto _LL907;_LL907: if(
_tmp7A1 == 0){struct Cyc_List_List*_tmp7A2=fields;for(0;_tmp7A2 != 0;_tmp7A2=
_tmp7A2->tl){if(!(*((struct _tuple18*)_tmp7A2->hd)).f2){(*((struct _tuple18*)
_tmp7A2->hd)).f2=1;(*((struct _tuple19*)des->hd)).f1=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp7A3=_cycalloc(sizeof(*_tmp7A3));_tmp7A3->hd=(void*)((
void*)({struct Cyc_Absyn_FieldName_struct*_tmp7A4=_cycalloc(sizeof(*_tmp7A4));
_tmp7A4[0]=({struct Cyc_Absyn_FieldName_struct _tmp7A5;_tmp7A5.tag=1;_tmp7A5.f1=((*((
struct _tuple18*)_tmp7A2->hd)).f1)->name;_tmp7A5;});_tmp7A4;}));_tmp7A3->tl=0;
_tmp7A3;});ans=({struct Cyc_List_List*_tmp7A6=_region_malloc(rgn,sizeof(*_tmp7A6));
_tmp7A6->hd=({struct _tuple20*_tmp7A7=_region_malloc(rgn,sizeof(*_tmp7A7));
_tmp7A7->f1=(*((struct _tuple18*)_tmp7A2->hd)).f1;_tmp7A7->f2=_tmp7A0;_tmp7A7;});
_tmp7A6->tl=ans;_tmp7A6;});break;}}if(_tmp7A2 == 0){({void*_tmp7A8[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("too many arguments to struct",sizeof(unsigned char),29),_tag_arr(
_tmp7A8,sizeof(void*),0));});}}else{if(_tmp7A1->tl != 0){({void*_tmp7A9[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("multiple designators are not supported",sizeof(unsigned char),39),
_tag_arr(_tmp7A9,sizeof(void*),0));});}else{void*_tmp7AA=(void*)_tmp7A1->hd;
struct _tagged_arr*_tmp7AB;_LL90B: if(*((int*)_tmp7AA)== 0){goto _LL90C;}else{goto
_LL90D;}_LL90D: if(*((int*)_tmp7AA)== 1){_LL90F: _tmp7AB=((struct Cyc_Absyn_FieldName_struct*)
_tmp7AA)->f1;goto _LL90E;}else{goto _LL90A;}_LL90C:({void*_tmp7AC[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("array designator used in argument to struct",sizeof(unsigned char),
44),_tag_arr(_tmp7AC,sizeof(void*),0));});goto _LL90A;_LL90E: {struct Cyc_List_List*
_tmp7AD=fields;for(0;_tmp7AD != 0;_tmp7AD=_tmp7AD->tl){if(Cyc_Std_zstrptrcmp(
_tmp7AB,((*((struct _tuple18*)_tmp7AD->hd)).f1)->name)== 0){if((*((struct _tuple18*)
_tmp7AD->hd)).f2){({struct Cyc_Std_String_pa_struct _tmp7AF;_tmp7AF.tag=0;_tmp7AF.f1=(
struct _tagged_arr)*_tmp7AB;{void*_tmp7AE[1]={& _tmp7AF};Cyc_Tcutil_terr(loc,
_tag_arr("field %s has already been used as an argument",sizeof(unsigned char),
46),_tag_arr(_tmp7AE,sizeof(void*),1));}});}(*((struct _tuple18*)_tmp7AD->hd)).f2=
1;ans=({struct Cyc_List_List*_tmp7B0=_region_malloc(rgn,sizeof(*_tmp7B0));_tmp7B0->hd=({
struct _tuple20*_tmp7B1=_region_malloc(rgn,sizeof(*_tmp7B1));_tmp7B1->f1=(*((
struct _tuple18*)_tmp7AD->hd)).f1;_tmp7B1->f2=_tmp7A0;_tmp7B1;});_tmp7B0->tl=ans;
_tmp7B0;});break;}}if(_tmp7AD == 0){({struct Cyc_Std_String_pa_struct _tmp7B3;
_tmp7B3.tag=0;_tmp7B3.f1=(struct _tagged_arr)*_tmp7AB;{void*_tmp7B2[1]={& _tmp7B3};
Cyc_Tcutil_terr(loc,_tag_arr("bad field designator %s",sizeof(unsigned char),24),
_tag_arr(_tmp7B2,sizeof(void*),1));}});}goto _LL90A;}_LL90A:;}}}for(0;fields != 0;
fields=fields->tl){if(!(*((struct _tuple18*)fields->hd)).f2){({void*_tmp7B4[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too few arguments to struct",sizeof(unsigned char),
28),_tag_arr(_tmp7B4,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp7B5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp7B6;struct Cyc_Absyn_Conref*_tmp7B7;void*_tmp7B8;_LL911: if((unsigned int)
_tmp7B5 > 3?*((int*)_tmp7B5)== 4: 0){_LL915: _tmp7B6=((struct Cyc_Absyn_PointerType_struct*)
_tmp7B5)->f1;_LL917: _tmp7B8=(void*)_tmp7B6.elt_typ;goto _LL916;_LL916: _tmp7B7=
_tmp7B6.bounds;goto _LL912;}else{goto _LL913;}_LL913: goto _LL914;_LL912: {struct Cyc_Absyn_Conref*
_tmp7B9=Cyc_Absyn_compress_conref(_tmp7B7);void*_tmp7BA=(void*)(Cyc_Absyn_compress_conref(
_tmp7B9))->v;void*_tmp7BB;_LL919: if((unsigned int)_tmp7BA > 1?*((int*)_tmp7BA)== 
0: 0){_LL91F: _tmp7BB=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp7BA)->f1;if((
int)_tmp7BB == 0){goto _LL91A;}else{goto _LL91B;}}else{goto _LL91B;}_LL91B: if((int)
_tmp7BA == 0){goto _LL91C;}else{goto _LL91D;}_LL91D: goto _LL91E;_LL91A:*elt_typ_dest=
_tmp7B8;return 1;_LL91C:(void*)(_tmp7B9->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp7BC=_cycalloc(sizeof(*_tmp7BC));_tmp7BC[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp7BD;_tmp7BD.tag=0;_tmp7BD.f1=(void*)((void*)0);_tmp7BD;});_tmp7BC;})));*
elt_typ_dest=_tmp7B8;return 1;_LL91E: return 0;_LL918:;}_LL914: return 0;_LL910:;}int
Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore);}struct _tuple7 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){struct _tuple7 bogus_ans=({struct _tuple7 _tmp805;_tmp805.f1=
0;_tmp805.f2=(void*)2;_tmp805;});void*_tmp7BE=(void*)e->r;void*_tmp7BF;struct
_tuple1*_tmp7C0;struct _tagged_arr*_tmp7C1;struct Cyc_Absyn_Exp*_tmp7C2;struct
_tagged_arr*_tmp7C3;struct Cyc_Absyn_Exp*_tmp7C4;struct Cyc_Absyn_Exp*_tmp7C5;
struct Cyc_Absyn_Exp*_tmp7C6;struct Cyc_Absyn_Exp*_tmp7C7;_LL921: if(*((int*)
_tmp7BE)== 1){_LL92E: _tmp7C0=((struct Cyc_Absyn_Var_e_struct*)_tmp7BE)->f1;goto
_LL92D;_LL92D: _tmp7BF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp7BE)->f2;goto
_LL922;}else{goto _LL923;}_LL923: if(*((int*)_tmp7BE)== 21){_LL930: _tmp7C2=((
struct Cyc_Absyn_StructMember_e_struct*)_tmp7BE)->f1;goto _LL92F;_LL92F: _tmp7C1=((
struct Cyc_Absyn_StructMember_e_struct*)_tmp7BE)->f2;goto _LL924;}else{goto _LL925;}
_LL925: if(*((int*)_tmp7BE)== 22){_LL932: _tmp7C4=((struct Cyc_Absyn_StructArrow_e_struct*)
_tmp7BE)->f1;goto _LL931;_LL931: _tmp7C3=((struct Cyc_Absyn_StructArrow_e_struct*)
_tmp7BE)->f2;goto _LL926;}else{goto _LL927;}_LL927: if(*((int*)_tmp7BE)== 20){
_LL933: _tmp7C5=((struct Cyc_Absyn_Deref_e_struct*)_tmp7BE)->f1;goto _LL928;}else{
goto _LL929;}_LL929: if(*((int*)_tmp7BE)== 23){_LL935: _tmp7C7=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp7BE)->f1;goto _LL934;_LL934: _tmp7C6=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp7BE)->f2;goto _LL92A;}else{goto _LL92B;}_LL92B: goto _LL92C;_LL922: {void*
_tmp7C8=_tmp7BF;struct Cyc_Absyn_Vardecl*_tmp7C9;struct Cyc_Absyn_Vardecl*_tmp7CA;
struct Cyc_Absyn_Vardecl*_tmp7CB;struct Cyc_Absyn_Vardecl*_tmp7CC;_LL937: if((int)
_tmp7C8 == 0){goto _LL938;}else{goto _LL939;}_LL939: if((unsigned int)_tmp7C8 > 1?*((
int*)_tmp7C8)== 1: 0){goto _LL93A;}else{goto _LL93B;}_LL93B: if((unsigned int)
_tmp7C8 > 1?*((int*)_tmp7C8)== 0: 0){_LL943: _tmp7C9=((struct Cyc_Absyn_Global_b_struct*)
_tmp7C8)->f1;goto _LL93C;}else{goto _LL93D;}_LL93D: if((unsigned int)_tmp7C8 > 1?*((
int*)_tmp7C8)== 3: 0){_LL944: _tmp7CA=((struct Cyc_Absyn_Local_b_struct*)_tmp7C8)->f1;
goto _LL93E;}else{goto _LL93F;}_LL93F: if((unsigned int)_tmp7C8 > 1?*((int*)_tmp7C8)
== 4: 0){_LL945: _tmp7CB=((struct Cyc_Absyn_Pat_b_struct*)_tmp7C8)->f1;goto _LL940;}
else{goto _LL941;}_LL941: if((unsigned int)_tmp7C8 > 1?*((int*)_tmp7C8)== 2: 0){
_LL946: _tmp7CC=((struct Cyc_Absyn_Param_b_struct*)_tmp7C8)->f1;goto _LL942;}else{
goto _LL936;}_LL938: return bogus_ans;_LL93A: return({struct _tuple7 _tmp7CD;_tmp7CD.f1=
0;_tmp7CD.f2=(void*)2;_tmp7CD;});_LL93C: {void*_tmp7CE=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL948: if((unsigned int)_tmp7CE > 3?*((
int*)_tmp7CE)== 7: 0){goto _LL949;}else{goto _LL94A;}_LL94A: goto _LL94B;_LL949:
return({struct _tuple7 _tmp7CF;_tmp7CF.f1=1;_tmp7CF.f2=(void*)2;_tmp7CF;});_LL94B:
return({struct _tuple7 _tmp7D0;_tmp7D0.f1=(_tmp7C9->tq).q_const;_tmp7D0.f2=(void*)
2;_tmp7D0;});_LL947:;}_LL93E: {void*_tmp7D1=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL94D: if((unsigned int)_tmp7D1 > 3?*((
int*)_tmp7D1)== 7: 0){goto _LL94E;}else{goto _LL94F;}_LL94F: goto _LL950;_LL94E:
return({struct _tuple7 _tmp7D2;_tmp7D2.f1=1;_tmp7D2.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7CA->rgn))->v;_tmp7D2;});_LL950: _tmp7CA->escapes=1;return({
struct _tuple7 _tmp7D3;_tmp7D3.f1=(_tmp7CA->tq).q_const;_tmp7D3.f2=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp7CA->rgn))->v;_tmp7D3;});_LL94C:;}_LL940: _tmp7CC=
_tmp7CB;goto _LL942;_LL942: _tmp7CC->escapes=1;return({struct _tuple7 _tmp7D4;
_tmp7D4.f1=(_tmp7CC->tq).q_const;_tmp7D4.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7CC->rgn))->v;_tmp7D4;});_LL936:;}_LL924: {void*_tmp7D5=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp7C2->topt))->v);struct Cyc_List_List*
_tmp7D6;struct Cyc_List_List*_tmp7D7;struct Cyc_Absyn_Structdecl**_tmp7D8;struct
Cyc_Absyn_Structdecl*_tmp7D9;struct Cyc_Absyn_Uniondecl**_tmp7DA;struct Cyc_Absyn_Uniondecl*
_tmp7DB;_LL952: if((unsigned int)_tmp7D5 > 3?*((int*)_tmp7D5)== 14: 0){_LL95C:
_tmp7D6=((struct Cyc_Absyn_AnonStructType_struct*)_tmp7D5)->f1;goto _LL953;}else{
goto _LL954;}_LL954: if((unsigned int)_tmp7D5 > 3?*((int*)_tmp7D5)== 15: 0){_LL95D:
_tmp7D7=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp7D5)->f1;goto _LL955;}else{
goto _LL956;}_LL956: if((unsigned int)_tmp7D5 > 3?*((int*)_tmp7D5)== 10: 0){_LL95E:
_tmp7D8=((struct Cyc_Absyn_StructType_struct*)_tmp7D5)->f3;if(_tmp7D8 == 0){goto
_LL958;}else{_tmp7D9=*_tmp7D8;goto _LL957;}}else{goto _LL958;}_LL958: if((
unsigned int)_tmp7D5 > 3?*((int*)_tmp7D5)== 11: 0){_LL95F: _tmp7DA=((struct Cyc_Absyn_UnionType_struct*)
_tmp7D5)->f3;if(_tmp7DA == 0){goto _LL95A;}else{_tmp7DB=*_tmp7DA;goto _LL959;}}
else{goto _LL95A;}_LL95A: goto _LL95B;_LL953: _tmp7D7=_tmp7D6;goto _LL955;_LL955: {
struct Cyc_Absyn_Structfield*_tmp7DC=Cyc_Absyn_lookup_field(_tmp7D7,_tmp7C1);if(
_tmp7DC != 0? _tmp7DC->width != 0: 0){return({struct _tuple7 _tmp7DD;_tmp7DD.f1=(
_tmp7DC->tq).q_const;_tmp7DD.f2=(Cyc_Tcutil_addressof_props(te,_tmp7C2)).f2;
_tmp7DD;});}return bogus_ans;}_LL957: {struct Cyc_Absyn_Structfield*_tmp7DE=Cyc_Absyn_lookup_struct_field(
_tmp7D9,_tmp7C1);if(_tmp7DE != 0? _tmp7DE->width != 0: 0){return({struct _tuple7
_tmp7DF;_tmp7DF.f1=(_tmp7DE->tq).q_const;_tmp7DF.f2=(Cyc_Tcutil_addressof_props(
te,_tmp7C2)).f2;_tmp7DF;});}return bogus_ans;}_LL959: {struct Cyc_Absyn_Structfield*
_tmp7E0=Cyc_Absyn_lookup_union_field(_tmp7DB,_tmp7C1);if(_tmp7E0 != 0){return({
struct _tuple7 _tmp7E1;_tmp7E1.f1=(_tmp7E0->tq).q_const;_tmp7E1.f2=(Cyc_Tcutil_addressof_props(
te,_tmp7C2)).f2;_tmp7E1;});}goto _LL95B;}_LL95B: return bogus_ans;_LL951:;}_LL926: {
void*_tmp7E2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp7C4->topt))->v);struct Cyc_Absyn_PtrInfo _tmp7E3;void*_tmp7E4;void*_tmp7E5;
_LL961: if((unsigned int)_tmp7E2 > 3?*((int*)_tmp7E2)== 4: 0){_LL965: _tmp7E3=((
struct Cyc_Absyn_PointerType_struct*)_tmp7E2)->f1;_LL967: _tmp7E5=(void*)_tmp7E3.elt_typ;
goto _LL966;_LL966: _tmp7E4=(void*)_tmp7E3.rgn_typ;goto _LL962;}else{goto _LL963;}
_LL963: goto _LL964;_LL962: {void*_tmp7E6=Cyc_Tcutil_compress(_tmp7E5);struct Cyc_List_List*
_tmp7E7;struct Cyc_List_List*_tmp7E8;struct Cyc_Absyn_Structdecl**_tmp7E9;struct
Cyc_Absyn_Structdecl*_tmp7EA;struct Cyc_Absyn_Uniondecl**_tmp7EB;struct Cyc_Absyn_Uniondecl*
_tmp7EC;_LL969: if((unsigned int)_tmp7E6 > 3?*((int*)_tmp7E6)== 14: 0){_LL973:
_tmp7E7=((struct Cyc_Absyn_AnonStructType_struct*)_tmp7E6)->f1;goto _LL96A;}else{
goto _LL96B;}_LL96B: if((unsigned int)_tmp7E6 > 3?*((int*)_tmp7E6)== 15: 0){_LL974:
_tmp7E8=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp7E6)->f1;goto _LL96C;}else{
goto _LL96D;}_LL96D: if((unsigned int)_tmp7E6 > 3?*((int*)_tmp7E6)== 10: 0){_LL975:
_tmp7E9=((struct Cyc_Absyn_StructType_struct*)_tmp7E6)->f3;if(_tmp7E9 == 0){goto
_LL96F;}else{_tmp7EA=*_tmp7E9;goto _LL96E;}}else{goto _LL96F;}_LL96F: if((
unsigned int)_tmp7E6 > 3?*((int*)_tmp7E6)== 11: 0){_LL976: _tmp7EB=((struct Cyc_Absyn_UnionType_struct*)
_tmp7E6)->f3;if(_tmp7EB == 0){goto _LL971;}else{_tmp7EC=*_tmp7EB;goto _LL970;}}
else{goto _LL971;}_LL971: goto _LL972;_LL96A: _tmp7E8=_tmp7E7;goto _LL96C;_LL96C: {
struct Cyc_Absyn_Structfield*_tmp7ED=Cyc_Absyn_lookup_field(_tmp7E8,_tmp7C3);if(
_tmp7ED != 0? _tmp7ED->width != 0: 0){return({struct _tuple7 _tmp7EE;_tmp7EE.f1=(
_tmp7ED->tq).q_const;_tmp7EE.f2=_tmp7E4;_tmp7EE;});}return bogus_ans;}_LL96E: {
struct Cyc_Absyn_Structfield*_tmp7EF=Cyc_Absyn_lookup_struct_field(_tmp7EA,
_tmp7C3);if(_tmp7EF != 0? _tmp7EF->width != 0: 0){return({struct _tuple7 _tmp7F0;
_tmp7F0.f1=(_tmp7EF->tq).q_const;_tmp7F0.f2=_tmp7E4;_tmp7F0;});}return bogus_ans;}
_LL970: {struct Cyc_Absyn_Structfield*_tmp7F1=Cyc_Absyn_lookup_union_field(
_tmp7EC,_tmp7C3);if(_tmp7F1 != 0){return({struct _tuple7 _tmp7F2;_tmp7F2.f1=(
_tmp7F1->tq).q_const;_tmp7F2.f2=_tmp7E4;_tmp7F2;});}return bogus_ans;}_LL972:
return bogus_ans;_LL968:;}_LL964: return bogus_ans;_LL960:;}_LL928: {void*_tmp7F3=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7C5->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp7F4;struct Cyc_Absyn_Tqual _tmp7F5;void*_tmp7F6;_LL978:
if((unsigned int)_tmp7F3 > 3?*((int*)_tmp7F3)== 4: 0){_LL97C: _tmp7F4=((struct Cyc_Absyn_PointerType_struct*)
_tmp7F3)->f1;_LL97E: _tmp7F6=(void*)_tmp7F4.rgn_typ;goto _LL97D;_LL97D: _tmp7F5=
_tmp7F4.tq;goto _LL979;}else{goto _LL97A;}_LL97A: goto _LL97B;_LL979: return({struct
_tuple7 _tmp7F7;_tmp7F7.f1=_tmp7F5.q_const;_tmp7F7.f2=_tmp7F6;_tmp7F7;});_LL97B:
return bogus_ans;_LL977:;}_LL92A: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp7C7->topt))->v);void*_tmp7F8=t;struct Cyc_Absyn_Tqual _tmp7F9;
struct Cyc_List_List*_tmp7FA;struct Cyc_Absyn_PtrInfo _tmp7FB;struct Cyc_Absyn_Conref*
_tmp7FC;struct Cyc_Absyn_Tqual _tmp7FD;void*_tmp7FE;void*_tmp7FF;_LL980: if((
unsigned int)_tmp7F8 > 3?*((int*)_tmp7F8)== 7: 0){_LL988: _tmp7F9=((struct Cyc_Absyn_ArrayType_struct*)
_tmp7F8)->f2;goto _LL981;}else{goto _LL982;}_LL982: if((unsigned int)_tmp7F8 > 3?*((
int*)_tmp7F8)== 9: 0){_LL989: _tmp7FA=((struct Cyc_Absyn_TupleType_struct*)_tmp7F8)->f1;
goto _LL983;}else{goto _LL984;}_LL984: if((unsigned int)_tmp7F8 > 3?*((int*)_tmp7F8)
== 4: 0){_LL98A: _tmp7FB=((struct Cyc_Absyn_PointerType_struct*)_tmp7F8)->f1;_LL98E:
_tmp7FF=(void*)_tmp7FB.elt_typ;goto _LL98D;_LL98D: _tmp7FE=(void*)_tmp7FB.rgn_typ;
goto _LL98C;_LL98C: _tmp7FD=_tmp7FB.tq;goto _LL98B;_LL98B: _tmp7FC=_tmp7FB.bounds;
goto _LL985;}else{goto _LL986;}_LL986: goto _LL987;_LL981: return({struct _tuple7
_tmp800;_tmp800.f1=_tmp7F9.q_const;_tmp800.f2=(Cyc_Tcutil_addressof_props(te,
_tmp7C7)).f2;_tmp800;});_LL983: {struct _tuple4*_tmp801=Cyc_Absyn_lookup_tuple_field(
_tmp7FA,(int)Cyc_Evexp_eval_const_uint_exp(_tmp7C6));if(_tmp801 != 0){return({
struct _tuple7 _tmp802;_tmp802.f1=((*_tmp801).f1).q_const;_tmp802.f2=(Cyc_Tcutil_addressof_props(
te,_tmp7C7)).f2;_tmp802;});}return bogus_ans;}_LL985: return({struct _tuple7 _tmp803;
_tmp803.f1=_tmp7FD.q_const;_tmp803.f2=_tmp7FE;_tmp803;});_LL987: return bogus_ans;
_LL97F:;}_LL92C:({void*_tmp804[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("unary & applied to non-lvalue",
sizeof(unsigned char),30),_tag_arr(_tmp804,sizeof(void*),0));});return bogus_ans;
_LL920:;}void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct
Cyc_Absyn_Exp*e){void*_tmp806=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_Tqual
_tmp807;void*_tmp808;_LL990: if((unsigned int)_tmp806 > 3?*((int*)_tmp806)== 7: 0){
_LL995: _tmp808=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp806)->f1;goto
_LL994;_LL994: _tmp807=((struct Cyc_Absyn_ArrayType_struct*)_tmp806)->f2;goto
_LL991;}else{goto _LL992;}_LL992: goto _LL993;_LL991: {void*_tmp80A;struct _tuple7
_tmp809=Cyc_Tcutil_addressof_props(te,e);_LL997: _tmp80A=_tmp809.f2;goto _LL996;
_LL996: return Cyc_Absyn_atb_typ(_tmp808,_tmp80A,_tmp807,(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp80B=_cycalloc(sizeof(*_tmp80B));_tmp80B[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp80C;_tmp80C.tag=0;_tmp80C.f1=e;_tmp80C;});_tmp80B;}));}_LL993: return e_typ;
_LL98F:;}void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,
struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(b);{void*_tmp80D=(void*)b->v;
void*_tmp80E;void*_tmp80F;struct Cyc_Absyn_Exp*_tmp810;_LL999: if((unsigned int)
_tmp80D > 1?*((int*)_tmp80D)== 0: 0){_LL99F: _tmp80E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp80D)->f1;if((int)_tmp80E == 0){goto _LL99A;}else{goto _LL99B;}}else{goto _LL99B;}
_LL99B: if((unsigned int)_tmp80D > 1?*((int*)_tmp80D)== 0: 0){_LL9A0: _tmp80F=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp80D)->f1;if((unsigned int)_tmp80F > 1?*((
int*)_tmp80F)== 0: 0){_LL9A1: _tmp810=((struct Cyc_Absyn_Upper_b_struct*)_tmp80F)->f1;
goto _LL99C;}else{goto _LL99D;}}else{goto _LL99D;}_LL99D: goto _LL99E;_LL99A: return;
_LL99C: if(Cyc_Evexp_eval_const_uint_exp(_tmp810)<= i){({struct Cyc_Std_Int_pa_struct
_tmp813;_tmp813.tag=1;_tmp813.f1=(int)i;{struct Cyc_Std_Int_pa_struct _tmp812;
_tmp812.tag=1;_tmp812.f1=(int)Cyc_Evexp_eval_const_uint_exp(_tmp810);{void*
_tmp811[2]={& _tmp812,& _tmp813};Cyc_Tcutil_terr(loc,_tag_arr("dereference is out of bounds: %d <= %d",
sizeof(unsigned char),39),_tag_arr(_tmp811,sizeof(void*),2));}}});}return;_LL99E:(
void*)(b->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp814=_cycalloc(
sizeof(*_tmp814));_tmp814[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp815;_tmp815.tag=
0;_tmp815.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp816=_cycalloc(
sizeof(*_tmp816));_tmp816[0]=({struct Cyc_Absyn_Upper_b_struct _tmp817;_tmp817.tag=
0;_tmp817.f1=Cyc_Absyn_uint_exp(i + 1,0);_tmp817;});_tmp816;}));_tmp815;});
_tmp814;})));return;_LL998:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp818=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp819;struct Cyc_Absyn_Exp*_tmp81A;_LL9A3: if((unsigned int)_tmp818 > 1?*((
int*)_tmp818)== 0: 0){_LL9A7: _tmp819=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp818)->f1;if((unsigned int)_tmp819 > 1?*((int*)_tmp819)== 0: 0){_LL9A8: _tmp81A=((
struct Cyc_Absyn_Upper_b_struct*)_tmp819)->f1;goto _LL9A4;}else{goto _LL9A5;}}else{
goto _LL9A5;}_LL9A5: goto _LL9A6;_LL9A4: return Cyc_Evexp_eval_const_uint_exp(_tmp81A)
== 1;_LL9A6: return 0;_LL9A2:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp81B=Cyc_Tcutil_compress(
t);void*_tmp81C;struct Cyc_List_List*_tmp81D;struct Cyc_Absyn_Structdecl**_tmp81E;
struct Cyc_List_List*_tmp81F;struct Cyc_Absyn_Uniondecl**_tmp820;struct Cyc_List_List*
_tmp821;struct Cyc_List_List*_tmp822;struct Cyc_List_List*_tmp823;_LL9AA: if((int)
_tmp81B == 0){goto _LL9AB;}else{goto _LL9AC;}_LL9AC: if((unsigned int)_tmp81B > 3?*((
int*)_tmp81B)== 5: 0){goto _LL9AD;}else{goto _LL9AE;}_LL9AE: if((int)_tmp81B == 1){
goto _LL9AF;}else{goto _LL9B0;}_LL9B0: if((unsigned int)_tmp81B > 3?*((int*)_tmp81B)
== 6: 0){goto _LL9B1;}else{goto _LL9B2;}_LL9B2: if((unsigned int)_tmp81B > 3?*((int*)
_tmp81B)== 12: 0){goto _LL9B3;}else{goto _LL9B4;}_LL9B4: if((unsigned int)_tmp81B > 3?*((
int*)_tmp81B)== 16: 0){goto _LL9B5;}else{goto _LL9B6;}_LL9B6: if((unsigned int)
_tmp81B > 3?*((int*)_tmp81B)== 7: 0){_LL9C4: _tmp81C=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp81B)->f1;goto _LL9B7;}else{goto _LL9B8;}_LL9B8: if((unsigned int)_tmp81B > 3?*((
int*)_tmp81B)== 9: 0){_LL9C5: _tmp81D=((struct Cyc_Absyn_TupleType_struct*)_tmp81B)->f1;
goto _LL9B9;}else{goto _LL9BA;}_LL9BA: if((unsigned int)_tmp81B > 3?*((int*)_tmp81B)
== 10: 0){_LL9C7: _tmp81F=((struct Cyc_Absyn_StructType_struct*)_tmp81B)->f2;goto
_LL9C6;_LL9C6: _tmp81E=((struct Cyc_Absyn_StructType_struct*)_tmp81B)->f3;goto
_LL9BB;}else{goto _LL9BC;}_LL9BC: if((unsigned int)_tmp81B > 3?*((int*)_tmp81B)== 
11: 0){_LL9C9: _tmp821=((struct Cyc_Absyn_UnionType_struct*)_tmp81B)->f2;goto _LL9C8;
_LL9C8: _tmp820=((struct Cyc_Absyn_UnionType_struct*)_tmp81B)->f3;goto _LL9BD;}
else{goto _LL9BE;}_LL9BE: if((unsigned int)_tmp81B > 3?*((int*)_tmp81B)== 14: 0){
_LL9CA: _tmp822=((struct Cyc_Absyn_AnonStructType_struct*)_tmp81B)->f1;goto _LL9BF;}
else{goto _LL9C0;}_LL9C0: if((unsigned int)_tmp81B > 3?*((int*)_tmp81B)== 15: 0){
_LL9CB: _tmp823=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp81B)->f1;goto _LL9C1;}
else{goto _LL9C2;}_LL9C2: goto _LL9C3;_LL9AB: goto _LL9AD;_LL9AD: goto _LL9AF;_LL9AF:
goto _LL9B1;_LL9B1: return 1;_LL9B3: return 0;_LL9B5: return 0;_LL9B7: return Cyc_Tcutil_bits_only(
_tmp81C);_LL9B9: for(0;_tmp81D != 0;_tmp81D=_tmp81D->tl){if(! Cyc_Tcutil_bits_only((*((
struct _tuple4*)_tmp81D->hd)).f2)){return 0;}}return 1;_LL9BB: if(_tmp81E == 0){
return 0;}{struct Cyc_Absyn_Structdecl*_tmp824=*_tmp81E;if(_tmp824->fields == 0){
return 0;}{struct _RegionHandle _tmp825=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp825;_push_region(rgn);{struct Cyc_List_List*_tmp826=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmp824->tvs,_tmp81F);{struct Cyc_List_List*fs=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp824->fields))->v;for(0;fs != 
0;fs=fs->tl){if(! Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmp826,(void*)((
struct Cyc_Absyn_Structfield*)fs->hd)->type))){int _tmp827=0;_npop_handler(0);
return _tmp827;}}}{int _tmp828=1;_npop_handler(0);return _tmp828;}};_pop_region(rgn);}}
_LL9BD: if(_tmp820 == 0){return 0;}{struct Cyc_Absyn_Uniondecl*_tmp829=*_tmp820;if(
_tmp829->fields == 0){return 0;}{struct _RegionHandle _tmp82A=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp82A;_push_region(rgn);{struct Cyc_List_List*_tmp82B=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp829->tvs,_tmp821);{struct Cyc_List_List*
fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp829->fields))->v;
for(0;fs != 0;fs=fs->tl){if(! Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,
_tmp82B,(void*)((struct Cyc_Absyn_Structfield*)fs->hd)->type))){int _tmp82C=0;
_npop_handler(0);return _tmp82C;}}}{int _tmp82D=1;_npop_handler(0);return _tmp82D;}};
_pop_region(rgn);}}_LL9BF: _tmp823=_tmp822;goto _LL9C1;_LL9C1: for(0;_tmp823 != 0;
_tmp823=_tmp823->tl){if(! Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Structfield*)
_tmp823->hd)->type)){return 0;}}return 1;_LL9C3: return 0;_LL9A9:;}struct _tuple21{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(
struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp82E=(void*)e->r;
void*_tmp82F;struct _tuple1*_tmp830;struct Cyc_Absyn_Exp*_tmp831;struct Cyc_Absyn_Exp*
_tmp832;struct Cyc_Absyn_Exp*_tmp833;struct Cyc_Absyn_Exp*_tmp834;struct Cyc_Absyn_Exp*
_tmp835;struct Cyc_Absyn_Exp*_tmp836;struct Cyc_Absyn_Exp*_tmp837;struct Cyc_Absyn_Exp*
_tmp838;void*_tmp839;struct Cyc_Absyn_Exp*_tmp83A;struct Cyc_Absyn_Exp*_tmp83B;
struct Cyc_Absyn_Exp*_tmp83C;struct Cyc_List_List*_tmp83D;struct Cyc_List_List*
_tmp83E;struct Cyc_List_List*_tmp83F;struct Cyc_List_List*_tmp840;void*_tmp841;
struct Cyc_List_List*_tmp842;struct Cyc_List_List*_tmp843;_LL9CD: if(*((int*)
_tmp82E)== 0){goto _LL9CE;}else{goto _LL9CF;}_LL9CF: if(*((int*)_tmp82E)== 16){goto
_LL9D0;}else{goto _LL9D1;}_LL9D1: if(*((int*)_tmp82E)== 17){goto _LL9D2;}else{goto
_LL9D3;}_LL9D3: if(*((int*)_tmp82E)== 18){goto _LL9D4;}else{goto _LL9D5;}_LL9D5: if(*((
int*)_tmp82E)== 19){goto _LL9D6;}else{goto _LL9D7;}_LL9D7: if(*((int*)_tmp82E)== 31){
goto _LL9D8;}else{goto _LL9D9;}_LL9D9: if(*((int*)_tmp82E)== 32){goto _LL9DA;}else{
goto _LL9DB;}_LL9DB: if(*((int*)_tmp82E)== 1){_LL9FA: _tmp830=((struct Cyc_Absyn_Var_e_struct*)
_tmp82E)->f1;goto _LL9F9;_LL9F9: _tmp82F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp82E)->f2;goto _LL9DC;}else{goto _LL9DD;}_LL9DD: if(*((int*)_tmp82E)== 6){_LL9FD:
_tmp833=((struct Cyc_Absyn_Conditional_e_struct*)_tmp82E)->f1;goto _LL9FC;_LL9FC:
_tmp832=((struct Cyc_Absyn_Conditional_e_struct*)_tmp82E)->f2;goto _LL9FB;_LL9FB:
_tmp831=((struct Cyc_Absyn_Conditional_e_struct*)_tmp82E)->f3;goto _LL9DE;}else{
goto _LL9DF;}_LL9DF: if(*((int*)_tmp82E)== 7){_LL9FF: _tmp835=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp82E)->f1;goto _LL9FE;_LL9FE: _tmp834=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp82E)->f2;goto _LL9E0;}else{goto _LL9E1;}_LL9E1: if(*((int*)_tmp82E)== 11){
_LLA00: _tmp836=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp82E)->f1;goto _LL9E2;}
else{goto _LL9E3;}_LL9E3: if(*((int*)_tmp82E)== 12){_LLA01: _tmp837=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp82E)->f1;goto _LL9E4;}else{goto _LL9E5;}_LL9E5: if(*((int*)_tmp82E)== 13){
_LLA03: _tmp839=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp82E)->f1;goto _LLA02;
_LLA02: _tmp838=((struct Cyc_Absyn_Cast_e_struct*)_tmp82E)->f2;goto _LL9E6;}else{
goto _LL9E7;}_LL9E7: if(*((int*)_tmp82E)== 14){_LLA04: _tmp83A=((struct Cyc_Absyn_Address_e_struct*)
_tmp82E)->f1;goto _LL9E8;}else{goto _LL9E9;}_LL9E9: if(*((int*)_tmp82E)== 27){
_LLA06: _tmp83C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp82E)->f2;goto _LLA05;
_LLA05: _tmp83B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp82E)->f3;goto _LL9EA;}
else{goto _LL9EB;}_LL9EB: if(*((int*)_tmp82E)== 26){_LLA07: _tmp83D=((struct Cyc_Absyn_Array_e_struct*)
_tmp82E)->f1;goto _LL9EC;}else{goto _LL9ED;}_LL9ED: if(*((int*)_tmp82E)== 29){
_LLA08: _tmp83E=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp82E)->f2;goto _LL9EE;}
else{goto _LL9EF;}_LL9EF: if(*((int*)_tmp82E)== 28){_LLA09: _tmp83F=((struct Cyc_Absyn_Struct_e_struct*)
_tmp82E)->f3;goto _LL9F0;}else{goto _LL9F1;}_LL9F1: if(*((int*)_tmp82E)== 3){_LLA0B:
_tmp841=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp82E)->f1;goto _LLA0A;_LLA0A:
_tmp840=((struct Cyc_Absyn_Primop_e_struct*)_tmp82E)->f2;goto _LL9F2;}else{goto
_LL9F3;}_LL9F3: if(*((int*)_tmp82E)== 24){_LLA0C: _tmp842=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp82E)->f1;goto _LL9F4;}else{goto _LL9F5;}_LL9F5: if(*((int*)_tmp82E)== 30){
_LLA0D: _tmp843=((struct Cyc_Absyn_Tunion_e_struct*)_tmp82E)->f3;goto _LL9F6;}else{
goto _LL9F7;}_LL9F7: goto _LL9F8;_LL9CE: goto _LL9D0;_LL9D0: goto _LL9D2;_LL9D2: goto
_LL9D4;_LL9D4: goto _LL9D6;_LL9D6: goto _LL9D8;_LL9D8: goto _LL9DA;_LL9DA: return 1;
_LL9DC: {void*_tmp844=_tmp82F;struct Cyc_Absyn_Vardecl*_tmp845;_LLA0F: if((
unsigned int)_tmp844 > 1?*((int*)_tmp844)== 1: 0){goto _LLA10;}else{goto _LLA11;}
_LLA11: if((unsigned int)_tmp844 > 1?*((int*)_tmp844)== 0: 0){_LLA17: _tmp845=((
struct Cyc_Absyn_Global_b_struct*)_tmp844)->f1;goto _LLA12;}else{goto _LLA13;}
_LLA13: if((int)_tmp844 == 0){goto _LLA14;}else{goto _LLA15;}_LLA15: goto _LLA16;
_LLA10: return 1;_LLA12: {void*_tmp846=Cyc_Tcutil_compress((void*)_tmp845->type);
_LLA19: if((unsigned int)_tmp846 > 3?*((int*)_tmp846)== 7: 0){goto _LLA1A;}else{goto
_LLA1B;}_LLA1B: if((unsigned int)_tmp846 > 3?*((int*)_tmp846)== 8: 0){goto _LLA1C;}
else{goto _LLA1D;}_LLA1D: goto _LLA1E;_LLA1A: return 1;_LLA1C: return 1;_LLA1E: return
var_okay;_LLA18:;}_LLA14: return 0;_LLA16: return var_okay;_LLA0E:;}_LL9DE: return(
Cyc_Tcutil_cnst_exp(te,0,_tmp833)? Cyc_Tcutil_cnst_exp(te,0,_tmp832): 0)? Cyc_Tcutil_cnst_exp(
te,0,_tmp831): 0;_LL9E0: return Cyc_Tcutil_cnst_exp(te,0,_tmp835)? Cyc_Tcutil_cnst_exp(
te,0,_tmp834): 0;_LL9E2: _tmp837=_tmp836;goto _LL9E4;_LL9E4: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp837);_LL9E6: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp838);
_LL9E8: return Cyc_Tcutil_cnst_exp(te,1,_tmp83A);_LL9EA: return Cyc_Tcutil_cnst_exp(
te,0,_tmp83C)? Cyc_Tcutil_cnst_exp(te,0,_tmp83B): 0;_LL9EC: _tmp83E=_tmp83D;goto
_LL9EE;_LL9EE: _tmp83F=_tmp83E;goto _LL9F0;_LL9F0: for(0;_tmp83F != 0;_tmp83F=
_tmp83F->tl){if(! Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple21*)_tmp83F->hd)).f2)){
return 0;}}return 1;_LL9F2: _tmp842=_tmp840;goto _LL9F4;_LL9F4: _tmp843=_tmp842;goto
_LL9F6;_LL9F6: for(0;_tmp843 != 0;_tmp843=_tmp843->tl){if(! Cyc_Tcutil_cnst_exp(te,
0,(struct Cyc_Absyn_Exp*)_tmp843->hd)){return 0;}}return 1;_LL9F8: return 0;_LL9CC:;}
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){return
Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(struct
Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp847=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp848;
struct Cyc_Absyn_Conref*_tmp849;struct Cyc_Absyn_Conref*_tmp84A;void*_tmp84B;
struct Cyc_List_List*_tmp84C;struct Cyc_Absyn_Structdecl**_tmp84D;struct Cyc_List_List*
_tmp84E;struct Cyc_Absyn_Uniondecl**_tmp84F;struct Cyc_List_List*_tmp850;struct Cyc_List_List*
_tmp851;struct Cyc_List_List*_tmp852;_LLA20: if((int)_tmp847 == 0){goto _LLA21;}
else{goto _LLA22;}_LLA22: if((unsigned int)_tmp847 > 3?*((int*)_tmp847)== 5: 0){goto
_LLA23;}else{goto _LLA24;}_LLA24: if((int)_tmp847 == 1){goto _LLA25;}else{goto _LLA26;}
_LLA26: if((unsigned int)_tmp847 > 3?*((int*)_tmp847)== 6: 0){goto _LLA27;}else{goto
_LLA28;}_LLA28: if((unsigned int)_tmp847 > 3?*((int*)_tmp847)== 4: 0){_LLA3C:
_tmp848=((struct Cyc_Absyn_PointerType_struct*)_tmp847)->f1;_LLA3E: _tmp84A=
_tmp848.nullable;goto _LLA3D;_LLA3D: _tmp849=_tmp848.bounds;goto _LLA29;}else{goto
_LLA2A;}_LLA2A: if((unsigned int)_tmp847 > 3?*((int*)_tmp847)== 7: 0){_LLA3F:
_tmp84B=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp847)->f1;goto _LLA2B;}
else{goto _LLA2C;}_LLA2C: if((unsigned int)_tmp847 > 3?*((int*)_tmp847)== 9: 0){
_LLA40: _tmp84C=((struct Cyc_Absyn_TupleType_struct*)_tmp847)->f1;goto _LLA2D;}
else{goto _LLA2E;}_LLA2E: if((unsigned int)_tmp847 > 3?*((int*)_tmp847)== 10: 0){
_LLA42: _tmp84E=((struct Cyc_Absyn_StructType_struct*)_tmp847)->f2;goto _LLA41;
_LLA41: _tmp84D=((struct Cyc_Absyn_StructType_struct*)_tmp847)->f3;goto _LLA2F;}
else{goto _LLA30;}_LLA30: if((unsigned int)_tmp847 > 3?*((int*)_tmp847)== 11: 0){
_LLA44: _tmp850=((struct Cyc_Absyn_UnionType_struct*)_tmp847)->f2;goto _LLA43;
_LLA43: _tmp84F=((struct Cyc_Absyn_UnionType_struct*)_tmp847)->f3;goto _LLA31;}
else{goto _LLA32;}_LLA32: if((unsigned int)_tmp847 > 3?*((int*)_tmp847)== 14: 0){
_LLA45: _tmp851=((struct Cyc_Absyn_AnonStructType_struct*)_tmp847)->f1;goto _LLA33;}
else{goto _LLA34;}_LLA34: if((unsigned int)_tmp847 > 3?*((int*)_tmp847)== 15: 0){
_LLA46: _tmp852=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp847)->f1;goto _LLA35;}
else{goto _LLA36;}_LLA36: if((unsigned int)_tmp847 > 3?*((int*)_tmp847)== 16: 0){
goto _LLA37;}else{goto _LLA38;}_LLA38: if((unsigned int)_tmp847 > 3?*((int*)_tmp847)
== 12: 0){goto _LLA39;}else{goto _LLA3A;}_LLA3A: goto _LLA3B;_LLA21: goto _LLA23;_LLA23:
goto _LLA25;_LLA25: goto _LLA27;_LLA27: return 1;_LLA29: {void*_tmp853=(void*)(Cyc_Absyn_compress_conref(
_tmp849))->v;void*_tmp854;void*_tmp855;_LLA48: if((unsigned int)_tmp853 > 1?*((int*)
_tmp853)== 0: 0){_LLA4E: _tmp854=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp853)->f1;if((int)_tmp854 == 0){goto _LLA49;}else{goto _LLA4A;}}else{goto _LLA4A;}
_LLA4A: if((unsigned int)_tmp853 > 1?*((int*)_tmp853)== 0: 0){_LLA4F: _tmp855=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp853)->f1;if((unsigned int)_tmp855 > 1?*((
int*)_tmp855)== 0: 0){goto _LLA4B;}else{goto _LLA4C;}}else{goto _LLA4C;}_LLA4C: goto
_LLA4D;_LLA49: return 1;_LLA4B: {void*_tmp856=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp84A))->v;int _tmp857;
_LLA51: if((unsigned int)_tmp856 > 1?*((int*)_tmp856)== 0: 0){_LLA55: _tmp857=(int)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp856)->f1;goto _LLA52;}else{goto _LLA53;}
_LLA53: goto _LLA54;_LLA52: return _tmp857;_LLA54: return 0;_LLA50:;}_LLA4D: return 0;
_LLA47:;}_LLA2B: return Cyc_Tcutil_supports_default(_tmp84B);_LLA2D: for(0;_tmp84C
!= 0;_tmp84C=_tmp84C->tl){if(! Cyc_Tcutil_supports_default((*((struct _tuple4*)
_tmp84C->hd)).f2)){return 0;}}return 1;_LLA2F: if(_tmp84D == 0){return 0;}{struct Cyc_Absyn_Structdecl*
sd=*_tmp84D;if(sd->fields == 0){return 0;}return Cyc_Tcutil_fields_support_default(
sd->tvs,_tmp84E,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(sd->fields))->v);}
_LLA31: if(_tmp84F == 0){return 0;}{struct Cyc_Absyn_Uniondecl*ud=*_tmp84F;if(ud->fields
== 0){return 0;}return Cyc_Tcutil_fields_support_default(ud->tvs,_tmp850,(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ud->fields))->v);}_LLA33:
_tmp852=_tmp851;goto _LLA35;_LLA35: return Cyc_Tcutil_fields_support_default(0,0,
_tmp852);_LLA37: goto _LLA39;_LLA39: return 1;_LLA3B: return 0;_LLA1F:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct
_RegionHandle _tmp858=_new_region("rgn");struct _RegionHandle*rgn=& _tmp858;
_push_region(rgn);{struct Cyc_List_List*_tmp859=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp859,(void*)((struct Cyc_Absyn_Structfield*)fs->hd)->type);if(! Cyc_Tcutil_supports_default(
t)){int _tmp85A=0;_npop_handler(0);return _tmp85A;}}};_pop_region(rgn);}return 1;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp85B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp85C;void*_tmp85D;struct Cyc_Absyn_FnInfo _tmp85E;struct Cyc_List_List*_tmp85F;
_LLA57: if((unsigned int)_tmp85B > 3?*((int*)_tmp85B)== 4: 0){_LLA5D: _tmp85C=((
struct Cyc_Absyn_PointerType_struct*)_tmp85B)->f1;_LLA5E: _tmp85D=(void*)_tmp85C.elt_typ;
goto _LLA58;}else{goto _LLA59;}_LLA59: if((unsigned int)_tmp85B > 3?*((int*)_tmp85B)
== 8: 0){_LLA5F: _tmp85E=((struct Cyc_Absyn_FnType_struct*)_tmp85B)->f1;_LLA60:
_tmp85F=_tmp85E.attributes;goto _LLA5A;}else{goto _LLA5B;}_LLA5B: goto _LLA5C;_LLA58:
return Cyc_Tcutil_is_noreturn(_tmp85D);_LLA5A: for(0;_tmp85F != 0;_tmp85F=_tmp85F->tl){
void*_tmp860=(void*)_tmp85F->hd;_LLA62: if((int)_tmp860 == 3){goto _LLA63;}else{
goto _LLA64;}_LLA64: goto _LLA65;_LLA63: return 1;_LLA65: continue;_LLA61:;}goto _LLA56;
_LLA5C: goto _LLA56;_LLA56:;}return 0;}
