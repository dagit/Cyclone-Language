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
 struct Cyc_Core_Opt{void*v;};extern int Cyc_Core_intcmp(int,int);extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;
struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*
Cyc_Std_stderr;extern int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);extern
unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_printf(struct _tagged_arr fmt,
struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,
struct _tagged_arr);extern struct _tagged_arr Cyc_Std_vrprintf(struct _RegionHandle*
r1,struct _tagged_arr fmt,struct _tagged_arr ap);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
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
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple1*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
Cyc_Absyn_Aggrdecl**f1;};struct Cyc_Absyn_AggrInfo{void*aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_TunionType_struct{int tag;struct Cyc_Absyn_TunionInfo f1;};
struct Cyc_Absyn_TunionFieldType_struct{int tag;struct Cyc_Absyn_TunionFieldInfo f1;
};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct
Cyc_Absyn_IntType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;void*f1;struct Cyc_Absyn_Tqual
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo
f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{
int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;
void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{int tag;struct
_tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};
struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
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
};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Aggrdecl*f3;};
struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};
struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
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
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct
Cyc_Absyn_AggrInfo f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;
struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*body;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Aggrfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
exist_vars;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple1*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple1*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple1*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple1*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
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
struct Cyc_Absyn_Tqual tq,void*b);extern struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(
struct Cyc_List_List*,struct _tagged_arr*);extern struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _tagged_arr*);struct _tuple4{struct Cyc_Absyn_Tqual
f1;void*f2;};extern struct _tuple4*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,
int);extern struct _tagged_arr Cyc_Absyn_attribute2string(void*);extern int Cyc_Absyn_fntype_att(
void*a);struct _tuple5{void*f1;struct _tuple1*f2;};extern struct _tuple5 Cyc_Absyn_aggr_kinded_name(
void*);extern struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(void*info);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(void*);extern
struct _tagged_arr Cyc_Absynpp_kind2string(void*);extern struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*);extern struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_insert(
struct Cyc_Dict_Dict*d,void*k,void*v);extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*
d,void*k);struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct
Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;
};struct Cyc_Tcenv_Frame_struct{int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*
ae;struct Cyc_Core_Opt*le;};extern void*Cyc_Tcenv_lookup_ordinary(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);extern struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
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
void*r,void*e);extern void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct
_tuple6{struct Cyc_Absyn_Tvar*f1;void*f2;};extern struct _tuple6*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple7{struct Cyc_List_List*f1;
struct _RegionHandle*f2;};extern struct _tuple6*Cyc_Tcutil_r_make_inst_var(struct
_tuple7*,struct Cyc_Absyn_Tvar*);extern void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_tagged_arr*fn);extern void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
extern void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
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
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple8{int f1;void*f2;
};extern struct _tuple8 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Exp*e);extern struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);extern
int*Cyc_Tcutil_new_tvar_id();extern void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
extern void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);extern int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);extern void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
extern int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);extern
int Cyc_Tcutil_bits_only(void*t);extern int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);extern void*Cyc_Tcutil_snd_tqt(struct _tuple4*);extern
int Cyc_Tcutil_supports_default(void*);extern int Cyc_Tcutil_is_noreturn(void*);
extern void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
extern void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct
Cyc_Position_Segment*,struct Cyc_Absyn_Aggrdecl*);extern void Cyc_Tc_tcTuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Tuniondecl*);extern void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct
Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct Cyc_Absyn_Enumdecl*);
unsigned char Cyc_Tcutil_TypeErr[12]="\000\000\000\000TypeErr";unsigned char Cyc_Tcutil_Unify[
10]="\000\000\000\000Unify";extern void Cyc_Tcutil_unify_it(void*t1,void*t2);void*
Cyc_Tcutil_t1_failure=(void*)0;void*Cyc_Tcutil_t2_failure=(void*)0;struct
_tagged_arr Cyc_Tcutil_failure_reason=(struct _tagged_arr){(void*)0,(void*)0,(void*)(
0 + 0)};void Cyc_Tcutil_explain_failure(){if(Cyc_Position_num_errors >= Cyc_Position_max_errors){
return;}Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);{struct
_tagged_arr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);struct _tagged_arr s2=
Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);int pos=8;({struct Cyc_Std_String_pa_struct
_tmp1;_tmp1.tag=0;_tmp1.f1=(struct _tagged_arr)s1;{void*_tmp0[1]={& _tmp1};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("\t%s and ",sizeof(unsigned char),9),_tag_arr(_tmp0,
sizeof(void*),1));}});pos +=_get_arr_size(s1,sizeof(unsigned char))+ 5;if(pos >= 
80){({void*_tmp2[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n\t",sizeof(
unsigned char),3),_tag_arr(_tmp2,sizeof(void*),0));});pos=8;}({struct Cyc_Std_String_pa_struct
_tmp4;_tmp4.tag=0;_tmp4.f1=(struct _tagged_arr)s2;{void*_tmp3[1]={& _tmp4};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s ",sizeof(unsigned char),4),_tag_arr(_tmp3,sizeof(
void*),1));}});pos +=_get_arr_size(s2,sizeof(unsigned char))+ 1;if(pos >= 80){({
void*_tmp5[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n\t",sizeof(
unsigned char),3),_tag_arr(_tmp5,sizeof(void*),0));});pos=8;}({void*_tmp6[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("failed to unify. ",sizeof(unsigned char),
18),_tag_arr(_tmp6,sizeof(void*),0));});pos +=17;if(Cyc_Tcutil_failure_reason.curr
!= ((struct _tagged_arr)_tag_arr(0,0,0)).curr){if(pos >= 80){({void*_tmp7[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n\t",sizeof(unsigned char),3),_tag_arr(
_tmp7,sizeof(void*),0));});pos=8;}({struct Cyc_Std_String_pa_struct _tmp9;_tmp9.tag=
0;_tmp9.f1=(struct _tagged_arr)Cyc_Tcutil_failure_reason;{void*_tmp8[1]={& _tmp9};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(
_tmp8,sizeof(void*),1));}});}({void*_tmpA[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("\n",sizeof(unsigned char),2),_tag_arr(_tmpA,sizeof(void*),0));});Cyc_Std_fflush((
struct Cyc_Std___cycFILE*)Cyc_Std_stderr);}}void Cyc_Tcutil_terr(struct Cyc_Position_Segment*
loc,struct _tagged_arr fmt,struct _tagged_arr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap){struct _tagged_arr msg=(struct
_tagged_arr)Cyc_Std_vrprintf(Cyc_Core_heap_region,fmt,ap);({struct Cyc_Std_String_pa_struct
_tmpC;_tmpC.tag=0;_tmpC.f1=(struct _tagged_arr)msg;{void*_tmpB[1]={& _tmpC};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("Error: %s\n",sizeof(unsigned char),11),_tag_arr(_tmpB,
sizeof(void*),1));}});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);(
int)_throw((void*)Cyc_Tcutil_TypeErr);}static struct _tagged_arr Cyc_Tcutil_tvar2string(
struct Cyc_Absyn_Tvar*tv){return*tv->name;}void Cyc_Tcutil_print_tvars(struct Cyc_List_List*
tvs){for(0;tvs != 0;tvs=tvs->tl){({struct Cyc_Std_String_pa_struct _tmpF;_tmpF.tag=
0;_tmpF.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string((void*)((struct Cyc_Absyn_Tvar*)
tvs->hd)->kind);{struct Cyc_Std_String_pa_struct _tmpE;_tmpE.tag=0;_tmpE.f1=(
struct _tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd);{void*
_tmpD[2]={& _tmpE,& _tmpF};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s::%s ",
sizeof(unsigned char),8),_tag_arr(_tmpD,sizeof(void*),2));}}});}({void*_tmp10[0]={};
Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("\n",sizeof(unsigned char),2),_tag_arr(
_tmp10,sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);}
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
_tmp19,sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);}}
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;static int Cyc_Tcutil_fast_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){return*((int*)_check_null(tv1->identity))
- *((int*)_check_null(tv2->identity));}void*Cyc_Tcutil_compress(void*t){void*
_tmp1A=t;struct Cyc_Core_Opt*_tmp1B;struct Cyc_Core_Opt*_tmp1C;struct Cyc_Core_Opt*
_tmp1D;struct Cyc_Core_Opt**_tmp1E;struct Cyc_Core_Opt*_tmp1F;struct Cyc_Core_Opt**
_tmp20;_LL1: if((unsigned int)_tmp1A > 3?*((int*)_tmp1A)== 0: 0){_LLB: _tmp1B=((
struct Cyc_Absyn_Evar_struct*)_tmp1A)->f2;if(_tmp1B == 0){goto _LL2;}else{goto _LL3;}}
else{goto _LL3;}_LL3: if((unsigned int)_tmp1A > 3?*((int*)_tmp1A)== 16: 0){_LLC:
_tmp1C=((struct Cyc_Absyn_TypedefType_struct*)_tmp1A)->f3;if(_tmp1C == 0){goto _LL4;}
else{goto _LL5;}}else{goto _LL5;}_LL5: if((unsigned int)_tmp1A > 3?*((int*)_tmp1A)== 
16: 0){_LLD: _tmp1D=((struct Cyc_Absyn_TypedefType_struct*)_tmp1A)->f3;_tmp1E=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp1A)->f3;goto _LL6;}
else{goto _LL7;}_LL7: if((unsigned int)_tmp1A > 3?*((int*)_tmp1A)== 0: 0){_LLE:
_tmp1F=((struct Cyc_Absyn_Evar_struct*)_tmp1A)->f2;_tmp20=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1A)->f2;goto _LL8;}else{goto _LL9;}_LL9: goto _LLA;
_LL2: goto _LL4;_LL4: return t;_LL6: _tmp20=_tmp1E;goto _LL8;_LL8: {void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(*_tmp20))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp20))->v){*_tmp20=({struct Cyc_Core_Opt*_tmp21=_cycalloc(sizeof(*
_tmp21));_tmp21->v=(void*)t2;_tmp21;});}return t2;}_LLA: return t;_LL0:;}void*Cyc_Tcutil_copy_type(
void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static struct Cyc_Absyn_Conref*Cyc_Tcutil_copy_conref(
struct Cyc_Absyn_Conref*c){void*_tmp22=(void*)c->v;void*_tmp23;struct Cyc_Absyn_Conref*
_tmp24;_LL10: if((int)_tmp22 == 0){goto _LL11;}else{goto _LL12;}_LL12: if((
unsigned int)_tmp22 > 1?*((int*)_tmp22)== 0: 0){_LL16: _tmp23=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp22)->f1;goto _LL13;}else{goto _LL14;}_LL14: if((unsigned int)_tmp22 > 1?*((int*)
_tmp22)== 1: 0){_LL17: _tmp24=((struct Cyc_Absyn_Forward_constr_struct*)_tmp22)->f1;
goto _LL15;}else{goto _LLF;}_LL11: return Cyc_Absyn_empty_conref();_LL13: return Cyc_Absyn_new_conref(
_tmp23);_LL15: return Cyc_Tcutil_copy_conref(_tmp24);_LLF:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb){void*_tmp25=Cyc_Absyn_compress_kb(kb);void*_tmp26;void*_tmp27;_LL19: if(*((
int*)_tmp25)== 0){_LL1F: _tmp26=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp25)->f1;
goto _LL1A;}else{goto _LL1B;}_LL1B: if(*((int*)_tmp25)== 1){goto _LL1C;}else{goto
_LL1D;}_LL1D: if(*((int*)_tmp25)== 2){_LL20: _tmp27=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp25)->f2;goto _LL1E;}else{goto _LL18;}_LL1A: return(void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp29;
_tmp29.tag=0;_tmp29.f1=(void*)_tmp26;_tmp29;});_tmp28;});_LL1C: return(void*)({
struct Cyc_Absyn_Unknown_kb_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({
struct Cyc_Absyn_Unknown_kb_struct _tmp2B;_tmp2B.tag=1;_tmp2B.f1=0;_tmp2B;});
_tmp2A;});_LL1E: return(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp2C=_cycalloc(
sizeof(*_tmp2C));_tmp2C[0]=({struct Cyc_Absyn_Less_kb_struct _tmp2D;_tmp2D.tag=2;
_tmp2D.f1=0;_tmp2D.f2=(void*)_tmp27;_tmp2D;});_tmp2C;});_LL18:;}static struct Cyc_Absyn_Tvar*
Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){return({struct Cyc_Absyn_Tvar*
_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->name=tv->name;_tmp2E->identity=0;
_tmp2E->kind=(void*)Cyc_Tcutil_copy_kindbound((void*)tv->kind);_tmp2E;});}static
struct _tuple2*Cyc_Tcutil_copy_arg(struct _tuple2*arg){struct _tuple2 _tmp30;void*
_tmp31;struct Cyc_Absyn_Tqual _tmp32;struct Cyc_Core_Opt*_tmp33;struct _tuple2*
_tmp2F=arg;_tmp30=*_tmp2F;_LL24: _tmp33=_tmp30.f1;goto _LL23;_LL23: _tmp32=_tmp30.f2;
goto _LL22;_LL22: _tmp31=_tmp30.f3;goto _LL21;_LL21: return({struct _tuple2*_tmp34=
_cycalloc(sizeof(*_tmp34));_tmp34->f1=_tmp33;_tmp34->f2=_tmp32;_tmp34->f3=Cyc_Tcutil_copy_type(
_tmp31);_tmp34;});}static struct _tuple4*Cyc_Tcutil_copy_tqt(struct _tuple4*arg){
struct _tuple4 _tmp36;void*_tmp37;struct Cyc_Absyn_Tqual _tmp38;struct _tuple4*_tmp35=
arg;_tmp36=*_tmp35;_LL27: _tmp38=_tmp36.f1;goto _LL26;_LL26: _tmp37=_tmp36.f2;goto
_LL25;_LL25: return({struct _tuple4*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->f1=
_tmp38;_tmp39->f2=Cyc_Tcutil_copy_type(_tmp37);_tmp39;});}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->name=f->name;_tmp3A->tq=f->tq;_tmp3A->type=(
void*)Cyc_Tcutil_copy_type((void*)f->type);_tmp3A->width=f->width;_tmp3A->attributes=
f->attributes;_tmp3A;});}struct _tuple9{void*f1;void*f2;};static struct _tuple9*Cyc_Tcutil_copy_rgncmp(
struct _tuple9*x){struct _tuple9 _tmp3C;void*_tmp3D;void*_tmp3E;struct _tuple9*
_tmp3B=x;_tmp3C=*_tmp3B;_LL2A: _tmp3E=_tmp3C.f1;goto _LL29;_LL29: _tmp3D=_tmp3C.f2;
goto _LL28;_LL28: return({struct _tuple9*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->f1=
Cyc_Tcutil_copy_type(_tmp3E);_tmp3F->f2=Cyc_Tcutil_copy_type(_tmp3D);_tmp3F;});}
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*
f){return({struct Cyc_Absyn_Enumfield*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->name=
f->name;_tmp40->tag=f->tag;_tmp40->loc=f->loc;_tmp40;});}void*Cyc_Tcutil_copy_type(
void*t){void*_tmp41=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp42;struct Cyc_Absyn_TunionInfo
_tmp43;void*_tmp44;struct Cyc_List_List*_tmp45;void*_tmp46;struct Cyc_Absyn_TunionFieldInfo
_tmp47;struct Cyc_List_List*_tmp48;void*_tmp49;struct Cyc_Absyn_PtrInfo _tmp4A;
struct Cyc_Absyn_Conref*_tmp4B;struct Cyc_Absyn_Tqual _tmp4C;struct Cyc_Absyn_Conref*
_tmp4D;void*_tmp4E;void*_tmp4F;void*_tmp50;void*_tmp51;int _tmp52;struct Cyc_Absyn_Exp*
_tmp53;struct Cyc_Absyn_Tqual _tmp54;void*_tmp55;struct Cyc_Absyn_FnInfo _tmp56;
struct Cyc_List_List*_tmp57;struct Cyc_List_List*_tmp58;struct Cyc_Absyn_VarargInfo*
_tmp59;int _tmp5A;struct Cyc_List_List*_tmp5B;void*_tmp5C;struct Cyc_Core_Opt*
_tmp5D;struct Cyc_List_List*_tmp5E;struct Cyc_List_List*_tmp5F;struct Cyc_Absyn_AggrInfo
_tmp60;struct Cyc_List_List*_tmp61;void*_tmp62;struct _tuple1*_tmp63;void*_tmp64;
struct Cyc_Absyn_AggrInfo _tmp65;struct Cyc_List_List*_tmp66;void*_tmp67;struct Cyc_Absyn_Aggrdecl**
_tmp68;struct Cyc_List_List*_tmp69;void*_tmp6A;struct Cyc_Absyn_Enumdecl*_tmp6B;
struct _tuple1*_tmp6C;struct Cyc_List_List*_tmp6D;void*_tmp6E;void*_tmp6F;struct
Cyc_List_List*_tmp70;struct _tuple1*_tmp71;void*_tmp72;struct Cyc_List_List*_tmp73;
void*_tmp74;_LL2C: if((int)_tmp41 == 0){goto _LL2D;}else{goto _LL2E;}_LL2E: if((
unsigned int)_tmp41 > 3?*((int*)_tmp41)== 0: 0){goto _LL2F;}else{goto _LL30;}_LL30:
if((unsigned int)_tmp41 > 3?*((int*)_tmp41)== 1: 0){_LL5C: _tmp42=((struct Cyc_Absyn_VarType_struct*)
_tmp41)->f1;goto _LL31;}else{goto _LL32;}_LL32: if((unsigned int)_tmp41 > 3?*((int*)
_tmp41)== 2: 0){_LL5D: _tmp43=((struct Cyc_Absyn_TunionType_struct*)_tmp41)->f1;
_LL60: _tmp46=(void*)_tmp43.tunion_info;goto _LL5F;_LL5F: _tmp45=_tmp43.targs;goto
_LL5E;_LL5E: _tmp44=(void*)_tmp43.rgn;goto _LL33;}else{goto _LL34;}_LL34: if((
unsigned int)_tmp41 > 3?*((int*)_tmp41)== 3: 0){_LL61: _tmp47=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp41)->f1;_LL63: _tmp49=(void*)_tmp47.field_info;goto _LL62;_LL62: _tmp48=_tmp47.targs;
goto _LL35;}else{goto _LL36;}_LL36: if((unsigned int)_tmp41 > 3?*((int*)_tmp41)== 4:
0){_LL64: _tmp4A=((struct Cyc_Absyn_PointerType_struct*)_tmp41)->f1;_LL69: _tmp4F=(
void*)_tmp4A.elt_typ;goto _LL68;_LL68: _tmp4E=(void*)_tmp4A.rgn_typ;goto _LL67;
_LL67: _tmp4D=_tmp4A.nullable;goto _LL66;_LL66: _tmp4C=_tmp4A.tq;goto _LL65;_LL65:
_tmp4B=_tmp4A.bounds;goto _LL37;}else{goto _LL38;}_LL38: if((unsigned int)_tmp41 > 3?*((
int*)_tmp41)== 5: 0){_LL6B: _tmp51=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp41)->f1;
goto _LL6A;_LL6A: _tmp50=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp41)->f2;goto
_LL39;}else{goto _LL3A;}_LL3A: if((int)_tmp41 == 1){goto _LL3B;}else{goto _LL3C;}
_LL3C: if((unsigned int)_tmp41 > 3?*((int*)_tmp41)== 6: 0){_LL6C: _tmp52=((struct Cyc_Absyn_DoubleType_struct*)
_tmp41)->f1;goto _LL3D;}else{goto _LL3E;}_LL3E: if((unsigned int)_tmp41 > 3?*((int*)
_tmp41)== 7: 0){_LL6F: _tmp55=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp41)->f1;
goto _LL6E;_LL6E: _tmp54=((struct Cyc_Absyn_ArrayType_struct*)_tmp41)->f2;goto _LL6D;
_LL6D: _tmp53=((struct Cyc_Absyn_ArrayType_struct*)_tmp41)->f3;goto _LL3F;}else{
goto _LL40;}_LL40: if((unsigned int)_tmp41 > 3?*((int*)_tmp41)== 8: 0){_LL70: _tmp56=((
struct Cyc_Absyn_FnType_struct*)_tmp41)->f1;_LL78: _tmp5E=_tmp56.tvars;goto _LL77;
_LL77: _tmp5D=_tmp56.effect;goto _LL76;_LL76: _tmp5C=(void*)_tmp56.ret_typ;goto
_LL75;_LL75: _tmp5B=_tmp56.args;goto _LL74;_LL74: _tmp5A=_tmp56.c_varargs;goto _LL73;
_LL73: _tmp59=_tmp56.cyc_varargs;goto _LL72;_LL72: _tmp58=_tmp56.rgn_po;goto _LL71;
_LL71: _tmp57=_tmp56.attributes;goto _LL41;}else{goto _LL42;}_LL42: if((unsigned int)
_tmp41 > 3?*((int*)_tmp41)== 9: 0){_LL79: _tmp5F=((struct Cyc_Absyn_TupleType_struct*)
_tmp41)->f1;goto _LL43;}else{goto _LL44;}_LL44: if((unsigned int)_tmp41 > 3?*((int*)
_tmp41)== 10: 0){_LL7A: _tmp60=((struct Cyc_Absyn_AggrType_struct*)_tmp41)->f1;
_LL7C: _tmp62=(void*)_tmp60.aggr_info;if(*((int*)_tmp62)== 0){_LL7E: _tmp64=(void*)((
struct Cyc_Absyn_UnknownAggr_struct*)_tmp62)->f1;goto _LL7D;_LL7D: _tmp63=((struct
Cyc_Absyn_UnknownAggr_struct*)_tmp62)->f2;goto _LL7B;}else{goto _LL46;}_LL7B:
_tmp61=_tmp60.targs;goto _LL45;}else{goto _LL46;}_LL46: if((unsigned int)_tmp41 > 3?*((
int*)_tmp41)== 10: 0){_LL7F: _tmp65=((struct Cyc_Absyn_AggrType_struct*)_tmp41)->f1;
_LL81: _tmp67=(void*)_tmp65.aggr_info;if(*((int*)_tmp67)== 1){_LL82: _tmp68=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp67)->f1;goto _LL80;}else{goto _LL48;}_LL80:
_tmp66=_tmp65.targs;goto _LL47;}else{goto _LL48;}_LL48: if((unsigned int)_tmp41 > 3?*((
int*)_tmp41)== 11: 0){_LL84: _tmp6A=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp41)->f1;goto _LL83;_LL83: _tmp69=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp41)->f2;
goto _LL49;}else{goto _LL4A;}_LL4A: if((unsigned int)_tmp41 > 3?*((int*)_tmp41)== 12:
0){_LL86: _tmp6C=((struct Cyc_Absyn_EnumType_struct*)_tmp41)->f1;goto _LL85;_LL85:
_tmp6B=((struct Cyc_Absyn_EnumType_struct*)_tmp41)->f2;goto _LL4B;}else{goto _LL4C;}
_LL4C: if((unsigned int)_tmp41 > 3?*((int*)_tmp41)== 13: 0){_LL87: _tmp6D=((struct
Cyc_Absyn_AnonEnumType_struct*)_tmp41)->f1;goto _LL4D;}else{goto _LL4E;}_LL4E: if((
unsigned int)_tmp41 > 3?*((int*)_tmp41)== 14: 0){_LL88: _tmp6E=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp41)->f1;goto _LL4F;}else{goto _LL50;}_LL50: if((unsigned int)_tmp41 > 3?*((int*)
_tmp41)== 15: 0){_LL89: _tmp6F=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp41)->f1;goto _LL51;}else{goto _LL52;}_LL52: if((unsigned int)_tmp41 > 3?*((int*)
_tmp41)== 16: 0){_LL8B: _tmp71=((struct Cyc_Absyn_TypedefType_struct*)_tmp41)->f1;
goto _LL8A;_LL8A: _tmp70=((struct Cyc_Absyn_TypedefType_struct*)_tmp41)->f2;goto
_LL53;}else{goto _LL54;}_LL54: if((int)_tmp41 == 2){goto _LL55;}else{goto _LL56;}
_LL56: if((unsigned int)_tmp41 > 3?*((int*)_tmp41)== 17: 0){_LL8C: _tmp72=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp41)->f1;goto _LL57;}else{goto _LL58;}_LL58:
if((unsigned int)_tmp41 > 3?*((int*)_tmp41)== 18: 0){_LL8D: _tmp73=((struct Cyc_Absyn_JoinEff_struct*)
_tmp41)->f1;goto _LL59;}else{goto _LL5A;}_LL5A: if((unsigned int)_tmp41 > 3?*((int*)
_tmp41)== 19: 0){_LL8E: _tmp74=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp41)->f1;
goto _LL5B;}else{goto _LL2B;}_LL2D: goto _LL2F;_LL2F: return t;_LL31: return(void*)({
struct Cyc_Absyn_VarType_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75[0]=({
struct Cyc_Absyn_VarType_struct _tmp76;_tmp76.tag=1;_tmp76.f1=Cyc_Tcutil_copy_tvar(
_tmp42);_tmp76;});_tmp75;});_LL33: return(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[0]=({struct Cyc_Absyn_TunionType_struct
_tmp78;_tmp78.tag=2;_tmp78.f1=({struct Cyc_Absyn_TunionInfo _tmp79;_tmp79.tunion_info=(
void*)_tmp46;_tmp79.targs=Cyc_Tcutil_copy_types(_tmp45);_tmp79.rgn=(void*)Cyc_Tcutil_copy_type(
_tmp44);_tmp79;});_tmp78;});_tmp77;});_LL35: return(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp7B;_tmp7B.tag=3;_tmp7B.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp7C;_tmp7C.field_info=(
void*)_tmp49;_tmp7C.targs=Cyc_Tcutil_copy_types(_tmp48);_tmp7C;});_tmp7B;});
_tmp7A;});_LL37: {void*_tmp7D=Cyc_Tcutil_copy_type(_tmp4F);void*_tmp7E=Cyc_Tcutil_copy_type(
_tmp4E);struct Cyc_Absyn_Conref*_tmp7F=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp4D);struct Cyc_Absyn_Tqual _tmp80=_tmp4C;struct Cyc_Absyn_Conref*
_tmp81=Cyc_Tcutil_copy_conref(_tmp4B);return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82[0]=({struct Cyc_Absyn_PointerType_struct
_tmp83;_tmp83.tag=4;_tmp83.f1=({struct Cyc_Absyn_PtrInfo _tmp84;_tmp84.elt_typ=(
void*)_tmp7D;_tmp84.rgn_typ=(void*)_tmp7E;_tmp84.nullable=_tmp7F;_tmp84.tq=
_tmp80;_tmp84.bounds=_tmp81;_tmp84;});_tmp83;});_tmp82;});}_LL39: return(void*)({
struct Cyc_Absyn_IntType_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=({
struct Cyc_Absyn_IntType_struct _tmp86;_tmp86.tag=5;_tmp86.f1=(void*)_tmp51;_tmp86.f2=(
void*)_tmp50;_tmp86;});_tmp85;});_LL3B: return t;_LL3D: return(void*)({struct Cyc_Absyn_DoubleType_struct*
_tmp87=_cycalloc_atomic(sizeof(*_tmp87));_tmp87[0]=({struct Cyc_Absyn_DoubleType_struct
_tmp88;_tmp88.tag=6;_tmp88.f1=_tmp52;_tmp88;});_tmp87;});_LL3F: return(void*)({
struct Cyc_Absyn_ArrayType_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({
struct Cyc_Absyn_ArrayType_struct _tmp8A;_tmp8A.tag=7;_tmp8A.f1=(void*)Cyc_Tcutil_copy_type(
_tmp55);_tmp8A.f2=_tmp54;_tmp8A.f3=_tmp53;_tmp8A;});_tmp89;});_LL41: {struct Cyc_List_List*
_tmp8B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp5E);struct Cyc_Core_Opt*
_tmp8C=_tmp5D == 0? 0:({struct Cyc_Core_Opt*_tmp96=_cycalloc(sizeof(*_tmp96));
_tmp96->v=(void*)Cyc_Tcutil_copy_type((void*)_tmp5D->v);_tmp96;});void*_tmp8D=
Cyc_Tcutil_copy_type(_tmp5C);struct Cyc_List_List*_tmp8E=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,
_tmp5B);int _tmp8F=_tmp5A;struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;if(_tmp59 != 0){
struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)_check_null(_tmp59);
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp90=_cycalloc(sizeof(*_tmp90));
_tmp90->name=cv->name;_tmp90->tq=cv->tq;_tmp90->type=(void*)Cyc_Tcutil_copy_type((
void*)cv->type);_tmp90->inject=cv->inject;_tmp90;});}{struct Cyc_List_List*_tmp91=((
struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp58);struct Cyc_List_List*_tmp92=_tmp57;
return(void*)({struct Cyc_Absyn_FnType_struct*_tmp93=_cycalloc(sizeof(*_tmp93));
_tmp93[0]=({struct Cyc_Absyn_FnType_struct _tmp94;_tmp94.tag=8;_tmp94.f1=({struct
Cyc_Absyn_FnInfo _tmp95;_tmp95.tvars=_tmp8B;_tmp95.effect=_tmp8C;_tmp95.ret_typ=(
void*)_tmp8D;_tmp95.args=_tmp8E;_tmp95.c_varargs=_tmp8F;_tmp95.cyc_varargs=
cyc_varargs2;_tmp95.rgn_po=_tmp91;_tmp95.attributes=_tmp92;_tmp95;});_tmp94;});
_tmp93;});}}_LL43: return(void*)({struct Cyc_Absyn_TupleType_struct*_tmp97=
_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Absyn_TupleType_struct _tmp98;
_tmp98.tag=9;_tmp98.f1=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp5F);
_tmp98;});_tmp97;});_LL45: return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp99=
_cycalloc(sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Absyn_AggrType_struct _tmp9A;
_tmp9A.tag=10;_tmp9A.f1=({struct Cyc_Absyn_AggrInfo _tmp9B;_tmp9B.aggr_info=(void*)((
void*)({struct Cyc_Absyn_UnknownAggr_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));
_tmp9C[0]=({struct Cyc_Absyn_UnknownAggr_struct _tmp9D;_tmp9D.tag=0;_tmp9D.f1=(
void*)_tmp64;_tmp9D.f2=_tmp63;_tmp9D;});_tmp9C;}));_tmp9B.targs=Cyc_Tcutil_copy_types(
_tmp61);_tmp9B;});_tmp9A;});_tmp99;});_LL47: return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E[0]=({struct Cyc_Absyn_AggrType_struct
_tmp9F;_tmp9F.tag=10;_tmp9F.f1=({struct Cyc_Absyn_AggrInfo _tmpA0;_tmpA0.aggr_info=(
void*)((void*)({struct Cyc_Absyn_KnownAggr_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));
_tmpA1[0]=({struct Cyc_Absyn_KnownAggr_struct _tmpA2;_tmpA2.tag=1;_tmpA2.f1=_tmp68;
_tmpA2;});_tmpA1;}));_tmpA0.targs=Cyc_Tcutil_copy_types(_tmp66);_tmpA0;});_tmp9F;});
_tmp9E;});_LL49: return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmpA3=
_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmpA4;
_tmpA4.tag=11;_tmpA4.f1=(void*)_tmp6A;_tmpA4.f2=((struct Cyc_List_List*(*)(struct
Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_copy_field,_tmp69);_tmpA4;});_tmpA3;});_LL4B: return(void*)({struct Cyc_Absyn_EnumType_struct*
_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5[0]=({struct Cyc_Absyn_EnumType_struct
_tmpA6;_tmpA6.tag=12;_tmpA6.f1=_tmp6C;_tmpA6.f2=_tmp6B;_tmpA6;});_tmpA5;});_LL4D:
return(void*)({struct Cyc_Absyn_AnonEnumType_struct*_tmpA7=_cycalloc(sizeof(*
_tmpA7));_tmpA7[0]=({struct Cyc_Absyn_AnonEnumType_struct _tmpA8;_tmpA8.tag=13;
_tmpA8.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp6D);_tmpA8;});
_tmpA7;});_LL4F: return(void*)({struct Cyc_Absyn_SizeofType_struct*_tmpA9=
_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({struct Cyc_Absyn_SizeofType_struct _tmpAA;
_tmpAA.tag=14;_tmpAA.f1=(void*)Cyc_Tcutil_copy_type(_tmp6E);_tmpAA;});_tmpA9;});
_LL51: return(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmpAB=_cycalloc(
sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmpAC;_tmpAC.tag=
15;_tmpAC.f1=(void*)Cyc_Tcutil_copy_type(_tmp6F);_tmpAC;});_tmpAB;});_LL53:
return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmpAD=_cycalloc(sizeof(*
_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_TypedefType_struct _tmpAE;_tmpAE.tag=16;
_tmpAE.f1=_tmp71;_tmpAE.f2=Cyc_Tcutil_copy_types(_tmp70);_tmpAE.f3=0;_tmpAE;});
_tmpAD;});_LL55: return t;_LL57: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({struct Cyc_Absyn_AccessEff_struct
_tmpB0;_tmpB0.tag=17;_tmpB0.f1=(void*)Cyc_Tcutil_copy_type(_tmp72);_tmpB0;});
_tmpAF;});_LL59: return(void*)({struct Cyc_Absyn_JoinEff_struct*_tmpB1=_cycalloc(
sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_JoinEff_struct _tmpB2;_tmpB2.tag=18;
_tmpB2.f1=Cyc_Tcutil_copy_types(_tmp73);_tmpB2;});_tmpB1;});_LL5B: return(void*)({
struct Cyc_Absyn_RgnsEff_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({
struct Cyc_Absyn_RgnsEff_struct _tmpB4;_tmpB4.tag=19;_tmpB4.f1=(void*)Cyc_Tcutil_copy_type(
_tmp74);_tmpB4;});_tmpB3;});_LL2B:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(
k1 == k2){return 1;}{struct _tuple9 _tmpB6=({struct _tuple9 _tmpB5;_tmpB5.f1=k1;_tmpB5.f2=
k2;_tmpB5;});void*_tmpB7;void*_tmpB8;void*_tmpB9;void*_tmpBA;void*_tmpBB;void*
_tmpBC;_LL90: _LL99: _tmpB8=_tmpB6.f1;if((int)_tmpB8 == 2){goto _LL98;}else{goto
_LL92;}_LL98: _tmpB7=_tmpB6.f2;if((int)_tmpB7 == 1){goto _LL91;}else{goto _LL92;}
_LL92: _LL9B: _tmpBA=_tmpB6.f1;if((int)_tmpBA == 2){goto _LL9A;}else{goto _LL94;}
_LL9A: _tmpB9=_tmpB6.f2;if((int)_tmpB9 == 0){goto _LL93;}else{goto _LL94;}_LL94:
_LL9D: _tmpBC=_tmpB6.f1;if((int)_tmpBC == 1){goto _LL9C;}else{goto _LL96;}_LL9C:
_tmpBB=_tmpB6.f2;if((int)_tmpBB == 0){goto _LL95;}else{goto _LL96;}_LL96: goto _LL97;
_LL91: goto _LL93;_LL93: goto _LL95;_LL95: return 1;_LL97: return 0;_LL8F:;}}void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*tv){void*_tmpBD=Cyc_Absyn_compress_kb((void*)tv->kind);void*
_tmpBE;void*_tmpBF;_LL9F: if(*((int*)_tmpBD)== 0){_LLA5: _tmpBE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmpBD)->f1;goto _LLA0;}else{goto _LLA1;}_LLA1: if(*((int*)_tmpBD)== 2){_LLA6:
_tmpBF=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpBD)->f2;goto _LLA2;}else{goto
_LLA3;}_LLA3: goto _LLA4;_LLA0: return _tmpBE;_LLA2: return _tmpBF;_LLA4:({void*_tmpC0[
0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("kind not suitably constrained!",sizeof(unsigned char),31),_tag_arr(
_tmpC0,sizeof(void*),0));});_LL9E:;}void*Cyc_Tcutil_typ_kind(void*t){void*_tmpC1=
Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpC2;struct Cyc_Core_Opt*_tmpC3;
struct Cyc_Absyn_Tvar*_tmpC4;void*_tmpC5;struct Cyc_Absyn_TunionFieldInfo _tmpC6;
void*_tmpC7;struct Cyc_Absyn_Tunionfield*_tmpC8;struct Cyc_Absyn_TunionFieldInfo
_tmpC9;void*_tmpCA;struct Cyc_Absyn_Enumdecl*_tmpCB;struct Cyc_Absyn_Enumdecl*
_tmpCC;struct Cyc_Absyn_PtrInfo _tmpCD;struct Cyc_Core_Opt*_tmpCE;_LLA8: if((
unsigned int)_tmpC1 > 3?*((int*)_tmpC1)== 0: 0){_LLDB: _tmpC3=((struct Cyc_Absyn_Evar_struct*)
_tmpC1)->f1;goto _LLDA;_LLDA: _tmpC2=((struct Cyc_Absyn_Evar_struct*)_tmpC1)->f2;
goto _LLA9;}else{goto _LLAA;}_LLAA: if((unsigned int)_tmpC1 > 3?*((int*)_tmpC1)== 1:
0){_LLDC: _tmpC4=((struct Cyc_Absyn_VarType_struct*)_tmpC1)->f1;goto _LLAB;}else{
goto _LLAC;}_LLAC: if((int)_tmpC1 == 0){goto _LLAD;}else{goto _LLAE;}_LLAE: if((
unsigned int)_tmpC1 > 3?*((int*)_tmpC1)== 5: 0){_LLDD: _tmpC5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpC1)->f2;goto _LLAF;}else{goto _LLB0;}_LLB0: if((int)_tmpC1 == 1){goto _LLB1;}
else{goto _LLB2;}_LLB2: if((unsigned int)_tmpC1 > 3?*((int*)_tmpC1)== 6: 0){goto
_LLB3;}else{goto _LLB4;}_LLB4: if((unsigned int)_tmpC1 > 3?*((int*)_tmpC1)== 8: 0){
goto _LLB5;}else{goto _LLB6;}_LLB6: if((unsigned int)_tmpC1 > 3?*((int*)_tmpC1)== 15:
0){goto _LLB7;}else{goto _LLB8;}_LLB8: if((int)_tmpC1 == 2){goto _LLB9;}else{goto
_LLBA;}_LLBA: if((unsigned int)_tmpC1 > 3?*((int*)_tmpC1)== 2: 0){goto _LLBB;}else{
goto _LLBC;}_LLBC: if((unsigned int)_tmpC1 > 3?*((int*)_tmpC1)== 3: 0){_LLDE: _tmpC6=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmpC1)->f1;_LLDF: _tmpC7=(void*)_tmpC6.field_info;
if(*((int*)_tmpC7)== 1){_LLE0: _tmpC8=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmpC7)->f2;goto _LLBD;}else{goto _LLBE;}}else{goto _LLBE;}_LLBE: if((unsigned int)
_tmpC1 > 3?*((int*)_tmpC1)== 3: 0){_LLE1: _tmpC9=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpC1)->f1;_LLE2: _tmpCA=(void*)_tmpC9.field_info;if(*((int*)_tmpCA)== 0){goto
_LLBF;}else{goto _LLC0;}}else{goto _LLC0;}_LLC0: if((unsigned int)_tmpC1 > 3?*((int*)
_tmpC1)== 12: 0){_LLE3: _tmpCB=((struct Cyc_Absyn_EnumType_struct*)_tmpC1)->f2;if(
_tmpCB == 0){goto _LLC1;}else{goto _LLC2;}}else{goto _LLC2;}_LLC2: if((unsigned int)
_tmpC1 > 3?*((int*)_tmpC1)== 10: 0){goto _LLC3;}else{goto _LLC4;}_LLC4: if((
unsigned int)_tmpC1 > 3?*((int*)_tmpC1)== 11: 0){goto _LLC5;}else{goto _LLC6;}_LLC6:
if((unsigned int)_tmpC1 > 3?*((int*)_tmpC1)== 13: 0){goto _LLC7;}else{goto _LLC8;}
_LLC8: if((unsigned int)_tmpC1 > 3?*((int*)_tmpC1)== 12: 0){_LLE4: _tmpCC=((struct
Cyc_Absyn_EnumType_struct*)_tmpC1)->f2;goto _LLC9;}else{goto _LLCA;}_LLCA: if((
unsigned int)_tmpC1 > 3?*((int*)_tmpC1)== 4: 0){_LLE5: _tmpCD=((struct Cyc_Absyn_PointerType_struct*)
_tmpC1)->f1;goto _LLCB;}else{goto _LLCC;}_LLCC: if((unsigned int)_tmpC1 > 3?*((int*)
_tmpC1)== 14: 0){goto _LLCD;}else{goto _LLCE;}_LLCE: if((unsigned int)_tmpC1 > 3?*((
int*)_tmpC1)== 7: 0){goto _LLCF;}else{goto _LLD0;}_LLD0: if((unsigned int)_tmpC1 > 3?*((
int*)_tmpC1)== 9: 0){goto _LLD1;}else{goto _LLD2;}_LLD2: if((unsigned int)_tmpC1 > 3?*((
int*)_tmpC1)== 16: 0){_LLE6: _tmpCE=((struct Cyc_Absyn_TypedefType_struct*)_tmpC1)->f3;
goto _LLD3;}else{goto _LLD4;}_LLD4: if((unsigned int)_tmpC1 > 3?*((int*)_tmpC1)== 17:
0){goto _LLD5;}else{goto _LLD6;}_LLD6: if((unsigned int)_tmpC1 > 3?*((int*)_tmpC1)== 
18: 0){goto _LLD7;}else{goto _LLD8;}_LLD8: if((unsigned int)_tmpC1 > 3?*((int*)_tmpC1)
== 19: 0){goto _LLD9;}else{goto _LLA7;}_LLA9: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpC3))->v;_LLAB: return Cyc_Tcutil_tvar_kind(_tmpC4);_LLAD: return(
void*)1;_LLAF: return _tmpC5 == (void*)2?(void*)2:(void*)1;_LLB1: goto _LLB3;_LLB3:
goto _LLB5;_LLB5: return(void*)1;_LLB7: return(void*)2;_LLB9: return(void*)3;_LLBB:
return(void*)2;_LLBD: if(_tmpC8->typs == 0){return(void*)2;}else{return(void*)1;}
_LLBF:({void*_tmpCF[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("typ_kind: Unresolved TunionFieldType",sizeof(unsigned char),37),
_tag_arr(_tmpCF,sizeof(void*),0));});_LLC1: return(void*)0;_LLC3: goto _LLC5;_LLC5:
goto _LLC7;_LLC7: return(void*)1;_LLC9: if(_tmpCC->fields == 0){return(void*)0;}
else{return(void*)1;}_LLCB: {void*_tmpD0=(void*)(Cyc_Absyn_compress_conref(
_tmpCD.bounds))->v;void*_tmpD1;void*_tmpD2;_LLE8: if((unsigned int)_tmpD0 > 1?*((
int*)_tmpD0)== 0: 0){_LLF0: _tmpD1=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmpD0)->f1;if((int)_tmpD1 == 0){goto _LLE9;}else{goto _LLEA;}}else{goto _LLEA;}
_LLEA: if((unsigned int)_tmpD0 > 1?*((int*)_tmpD0)== 0: 0){_LLF1: _tmpD2=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmpD0)->f1;if((unsigned int)_tmpD2 > 1?*((int*)
_tmpD2)== 0: 0){goto _LLEB;}else{goto _LLEC;}}else{goto _LLEC;}_LLEC: if((int)_tmpD0
== 0){goto _LLED;}else{goto _LLEE;}_LLEE: if((unsigned int)_tmpD0 > 1?*((int*)_tmpD0)
== 1: 0){goto _LLEF;}else{goto _LLE7;}_LLE9: return(void*)1;_LLEB: return(void*)2;
_LLED: return(void*)1;_LLEF:({void*_tmpD3[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typ_kind: forward constraint in ptr bounds",
sizeof(unsigned char),43),_tag_arr(_tmpD3,sizeof(void*),0));});_LLE7:;}_LLCD:
return(void*)2;_LLCF: goto _LLD1;_LLD1: return(void*)1;_LLD3:({struct Cyc_Std_String_pa_struct
_tmpD5;_tmpD5.tag=0;_tmpD5.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmpD4[1]={& _tmpD5};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("typ_kind: typedef found: %s",sizeof(unsigned char),28),_tag_arr(_tmpD4,
sizeof(void*),1));}});_LLD5: goto _LLD7;_LLD7: goto _LLD9;_LLD9: return(void*)4;_LLA7:;}
int Cyc_Tcutil_unify(void*t1,void*t2){struct _handler_cons _tmpD6;_push_handler(&
_tmpD6);{int _tmpD8=0;if(setjmp(_tmpD6.handler)){_tmpD8=1;}if(! _tmpD8){Cyc_Tcutil_unify_it(
t1,t2);{int _tmpD9=1;_npop_handler(0);return _tmpD9;};_pop_handler();}else{void*
_tmpD7=(void*)_exn_thrown;void*_tmpDB=_tmpD7;_LLF3: if(_tmpDB == Cyc_Tcutil_Unify){
goto _LLF4;}else{goto _LLF5;}_LLF5: goto _LLF6;_LLF4: return 0;_LLF6:(void)_throw(
_tmpDB);_LLF2:;}}}static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*
r,struct Cyc_List_List*env,struct Cyc_List_List*ts);static void Cyc_Tcutil_occurs(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){void*_tmpDC=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmpDD;struct Cyc_Core_Opt*_tmpDE;struct Cyc_Core_Opt**
_tmpDF;struct Cyc_Core_Opt*_tmpE0;struct Cyc_Absyn_PtrInfo _tmpE1;void*_tmpE2;
struct Cyc_Absyn_FnInfo _tmpE3;struct Cyc_List_List*_tmpE4;struct Cyc_List_List*
_tmpE5;struct Cyc_Absyn_VarargInfo*_tmpE6;int _tmpE7;struct Cyc_List_List*_tmpE8;
void*_tmpE9;struct Cyc_Core_Opt*_tmpEA;struct Cyc_List_List*_tmpEB;struct Cyc_List_List*
_tmpEC;struct Cyc_Absyn_TunionInfo _tmpED;void*_tmpEE;struct Cyc_List_List*_tmpEF;
struct Cyc_Core_Opt*_tmpF0;struct Cyc_List_List*_tmpF1;struct Cyc_Absyn_TunionFieldInfo
_tmpF2;struct Cyc_List_List*_tmpF3;struct Cyc_Absyn_AggrInfo _tmpF4;struct Cyc_List_List*
_tmpF5;struct Cyc_List_List*_tmpF6;void*_tmpF7;void*_tmpF8;void*_tmpF9;void*
_tmpFA;struct Cyc_List_List*_tmpFB;_LLF8: if((unsigned int)_tmpDC > 3?*((int*)
_tmpDC)== 1: 0){_LL11A: _tmpDD=((struct Cyc_Absyn_VarType_struct*)_tmpDC)->f1;goto
_LLF9;}else{goto _LLFA;}_LLFA: if((unsigned int)_tmpDC > 3?*((int*)_tmpDC)== 0: 0){
_LL11C: _tmpE0=((struct Cyc_Absyn_Evar_struct*)_tmpDC)->f2;goto _LL11B;_LL11B:
_tmpDE=((struct Cyc_Absyn_Evar_struct*)_tmpDC)->f4;_tmpDF=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmpDC)->f4;goto _LLFB;}else{goto _LLFC;}_LLFC: if((
unsigned int)_tmpDC > 3?*((int*)_tmpDC)== 4: 0){_LL11D: _tmpE1=((struct Cyc_Absyn_PointerType_struct*)
_tmpDC)->f1;goto _LLFD;}else{goto _LLFE;}_LLFE: if((unsigned int)_tmpDC > 3?*((int*)
_tmpDC)== 7: 0){_LL11E: _tmpE2=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmpDC)->f1;
goto _LLFF;}else{goto _LL100;}_LL100: if((unsigned int)_tmpDC > 3?*((int*)_tmpDC)== 
8: 0){_LL11F: _tmpE3=((struct Cyc_Absyn_FnType_struct*)_tmpDC)->f1;_LL127: _tmpEB=
_tmpE3.tvars;goto _LL126;_LL126: _tmpEA=_tmpE3.effect;goto _LL125;_LL125: _tmpE9=(
void*)_tmpE3.ret_typ;goto _LL124;_LL124: _tmpE8=_tmpE3.args;goto _LL123;_LL123:
_tmpE7=_tmpE3.c_varargs;goto _LL122;_LL122: _tmpE6=_tmpE3.cyc_varargs;goto _LL121;
_LL121: _tmpE5=_tmpE3.rgn_po;goto _LL120;_LL120: _tmpE4=_tmpE3.attributes;goto
_LL101;}else{goto _LL102;}_LL102: if((unsigned int)_tmpDC > 3?*((int*)_tmpDC)== 9: 0){
_LL128: _tmpEC=((struct Cyc_Absyn_TupleType_struct*)_tmpDC)->f1;goto _LL103;}else{
goto _LL104;}_LL104: if((unsigned int)_tmpDC > 3?*((int*)_tmpDC)== 2: 0){_LL129:
_tmpED=((struct Cyc_Absyn_TunionType_struct*)_tmpDC)->f1;_LL12B: _tmpEF=_tmpED.targs;
goto _LL12A;_LL12A: _tmpEE=(void*)_tmpED.rgn;goto _LL105;}else{goto _LL106;}_LL106:
if((unsigned int)_tmpDC > 3?*((int*)_tmpDC)== 16: 0){_LL12D: _tmpF1=((struct Cyc_Absyn_TypedefType_struct*)
_tmpDC)->f2;goto _LL12C;_LL12C: _tmpF0=((struct Cyc_Absyn_TypedefType_struct*)
_tmpDC)->f3;goto _LL107;}else{goto _LL108;}_LL108: if((unsigned int)_tmpDC > 3?*((
int*)_tmpDC)== 3: 0){_LL12E: _tmpF2=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpDC)->f1;_LL12F: _tmpF3=_tmpF2.targs;goto _LL109;}else{goto _LL10A;}_LL10A: if((
unsigned int)_tmpDC > 3?*((int*)_tmpDC)== 10: 0){_LL130: _tmpF4=((struct Cyc_Absyn_AggrType_struct*)
_tmpDC)->f1;_LL131: _tmpF5=_tmpF4.targs;goto _LL10B;}else{goto _LL10C;}_LL10C: if((
unsigned int)_tmpDC > 3?*((int*)_tmpDC)== 11: 0){_LL132: _tmpF6=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpDC)->f2;goto _LL10D;}else{goto _LL10E;}_LL10E: if((unsigned int)_tmpDC > 3?*((
int*)_tmpDC)== 15: 0){_LL133: _tmpF7=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmpDC)->f1;goto _LL10F;}else{goto _LL110;}_LL110: if((unsigned int)_tmpDC > 3?*((
int*)_tmpDC)== 14: 0){_LL134: _tmpF8=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmpDC)->f1;goto _LL111;}else{goto _LL112;}_LL112: if((unsigned int)_tmpDC > 3?*((
int*)_tmpDC)== 17: 0){_LL135: _tmpF9=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmpDC)->f1;goto _LL113;}else{goto _LL114;}_LL114: if((unsigned int)_tmpDC > 3?*((
int*)_tmpDC)== 19: 0){_LL136: _tmpFA=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmpDC)->f1;goto _LL115;}else{goto _LL116;}_LL116: if((unsigned int)_tmpDC > 3?*((
int*)_tmpDC)== 18: 0){_LL137: _tmpFB=((struct Cyc_Absyn_JoinEff_struct*)_tmpDC)->f1;
goto _LL117;}else{goto _LL118;}_LL118: goto _LL119;_LLF9: if(!((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmpDD)){Cyc_Tcutil_failure_reason=
_tag_arr("(type variable would escape scope)",sizeof(unsigned char),35);(int)
_throw((void*)Cyc_Tcutil_Unify);}goto _LLF7;_LLFB: if(t == evar){Cyc_Tcutil_failure_reason=
_tag_arr("(occurs check)",sizeof(unsigned char),15);(int)_throw((void*)Cyc_Tcutil_Unify);}
else{if(_tmpE0 != 0){Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpE0->v);}else{int
problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmpDF))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}if(problem){struct Cyc_List_List*_tmpFC=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpDF))->v;for(0;s != 
0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){_tmpFC=({struct Cyc_List_List*_tmpFD=_cycalloc(
sizeof(*_tmpFD));_tmpFD->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmpFD->tl=_tmpFC;
_tmpFD;});}}}*_tmpDF=({struct Cyc_Core_Opt*_tmpFE=_cycalloc(sizeof(*_tmpFE));
_tmpFE->v=_tmpFC;_tmpFE;});}}}goto _LLF7;_LLFD: Cyc_Tcutil_occurs(evar,r,env,(void*)
_tmpE1.elt_typ);Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpE1.rgn_typ);goto _LLF7;
_LLFF: Cyc_Tcutil_occurs(evar,r,env,_tmpE2);goto _LLF7;_LL101: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmpEB,env);if(_tmpEA != 0){Cyc_Tcutil_occurs(evar,r,env,(void*)_tmpEA->v);}Cyc_Tcutil_occurs(
evar,r,env,_tmpE9);for(0;_tmpE8 != 0;_tmpE8=_tmpE8->tl){Cyc_Tcutil_occurs(evar,r,
env,(*((struct _tuple2*)_tmpE8->hd)).f3);}if(_tmpE6 != 0){void*_tmp100;struct Cyc_Absyn_VarargInfo
_tmpFF=*_tmpE6;_LL139: _tmp100=(void*)_tmpFF.type;goto _LL138;_LL138: Cyc_Tcutil_occurs(
evar,r,env,_tmp100);}for(0;_tmpE5 != 0;_tmpE5=_tmpE5->tl){struct _tuple9 _tmp102;
void*_tmp103;void*_tmp104;struct _tuple9*_tmp101=(struct _tuple9*)_tmpE5->hd;
_tmp102=*_tmp101;_LL13C: _tmp104=_tmp102.f1;goto _LL13B;_LL13B: _tmp103=_tmp102.f2;
goto _LL13A;_LL13A: Cyc_Tcutil_occurs(evar,r,env,_tmp104);Cyc_Tcutil_occurs(evar,r,
env,_tmp103);}goto _LLF7;_LL103: for(0;_tmpEC != 0;_tmpEC=_tmpEC->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple4*)_tmpEC->hd)).f2);}goto _LLF7;_LL105: Cyc_Tcutil_occurs(
evar,r,env,_tmpEE);Cyc_Tcutil_occurslist(evar,r,env,_tmpEF);goto _LLF7;_LL107:
_tmpF3=_tmpF1;goto _LL109;_LL109: _tmpF5=_tmpF3;goto _LL10B;_LL10B: Cyc_Tcutil_occurslist(
evar,r,env,_tmpF5);goto _LLF7;_LL10D: for(0;_tmpF6 != 0;_tmpF6=_tmpF6->tl){Cyc_Tcutil_occurs(
evar,r,env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmpF6->hd)->type);}goto _LLF7;
_LL10F: _tmpF8=_tmpF7;goto _LL111;_LL111: _tmpF9=_tmpF8;goto _LL113;_LL113: _tmpFA=
_tmpF9;goto _LL115;_LL115: Cyc_Tcutil_occurs(evar,r,env,_tmpFA);goto _LLF7;_LL117:
Cyc_Tcutil_occurslist(evar,r,env,_tmpFB);goto _LLF7;_LL119: goto _LLF7;_LLF7:;}
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(
void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*
t2){for(0;t1 != 0? t2 != 0: 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(
void*)t2->hd);}if(t1 != 0? 1: t2 != 0){(int)_throw((void*)Cyc_Tcutil_Unify);}}static
void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
if((tq1.q_const != tq2.q_const? 1: tq1.q_volatile != tq2.q_volatile)? 1: tq1.q_restrict
!= tq2.q_restrict){Cyc_Tcutil_failure_reason=_tag_arr("(qualifiers don't match)",
sizeof(unsigned char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}}int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){return(tq1.q_const == tq2.q_const?
tq1.q_volatile == tq2.q_volatile: 0)? tq1.q_restrict == tq2.q_restrict: 0;}static void
Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y,struct _tagged_arr reason){x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);if(x == y){return;}{void*_tmp105=(void*)x->v;void*
_tmp106;_LL13E: if((int)_tmp105 == 0){goto _LL13F;}else{goto _LL140;}_LL140: if((
unsigned int)_tmp105 > 1?*((int*)_tmp105)== 1: 0){goto _LL141;}else{goto _LL142;}
_LL142: if((unsigned int)_tmp105 > 1?*((int*)_tmp105)== 0: 0){_LL144: _tmp106=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp105)->f1;goto _LL143;}else{goto _LL13D;}
_LL13F:(void*)(x->v=(void*)((void*)({struct Cyc_Absyn_Forward_constr_struct*
_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107[0]=({struct Cyc_Absyn_Forward_constr_struct
_tmp108;_tmp108.tag=1;_tmp108.f1=y;_tmp108;});_tmp107;})));return;_LL141:({void*
_tmp109[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("unify_conref: forward after compress",sizeof(unsigned char),37),
_tag_arr(_tmp109,sizeof(void*),0));});_LL143: {void*_tmp10A=(void*)y->v;void*
_tmp10B;_LL146: if((int)_tmp10A == 0){goto _LL147;}else{goto _LL148;}_LL148: if((
unsigned int)_tmp10A > 1?*((int*)_tmp10A)== 1: 0){goto _LL149;}else{goto _LL14A;}
_LL14A: if((unsigned int)_tmp10A > 1?*((int*)_tmp10A)== 0: 0){_LL14C: _tmp10B=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp10A)->f1;goto _LL14B;}else{goto _LL145;}
_LL147:(void*)(y->v=(void*)((void*)x->v));return;_LL149:({void*_tmp10C[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unify_conref: forward after compress(2)",
sizeof(unsigned char),40),_tag_arr(_tmp10C,sizeof(void*),0));});_LL14B: if(cmp(
_tmp106,_tmp10B)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((void*)Cyc_Tcutil_Unify);}
return;_LL145:;}_LL13D:;}}static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){struct _handler_cons _tmp10D;
_push_handler(& _tmp10D);{int _tmp10F=0;if(setjmp(_tmp10D.handler)){_tmp10F=1;}if(
! _tmp10F){Cyc_Tcutil_unify_it_conrefs(cmp,x,y,(struct _tagged_arr)_tag_arr(0,0,0));{
int _tmp110=1;_npop_handler(0);return _tmp110;};_pop_handler();}else{void*_tmp10E=(
void*)_exn_thrown;void*_tmp112=_tmp10E;_LL14E: if(_tmp112 == Cyc_Tcutil_Unify){
goto _LL14F;}else{goto _LL150;}_LL150: goto _LL151;_LL14F: return 0;_LL151:(void)
_throw(_tmp112);_LL14D:;}}}static int Cyc_Tcutil_boundscmp(void*b1,void*b2){struct
_tuple9 _tmp114=({struct _tuple9 _tmp113;_tmp113.f1=b1;_tmp113.f2=b2;_tmp113;});
void*_tmp115;void*_tmp116;void*_tmp117;void*_tmp118;void*_tmp119;struct Cyc_Absyn_Exp*
_tmp11A;void*_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;_LL153: _LL15C: _tmp116=_tmp114.f1;
if((int)_tmp116 == 0){goto _LL15B;}else{goto _LL155;}_LL15B: _tmp115=_tmp114.f2;if((
int)_tmp115 == 0){goto _LL154;}else{goto _LL155;}_LL155: _LL15D: _tmp117=_tmp114.f1;
if((int)_tmp117 == 0){goto _LL156;}else{goto _LL157;}_LL157: _LL15E: _tmp118=_tmp114.f2;
if((int)_tmp118 == 0){goto _LL158;}else{goto _LL159;}_LL159: _LL161: _tmp11B=_tmp114.f1;
if((unsigned int)_tmp11B > 1?*((int*)_tmp11B)== 0: 0){_LL162: _tmp11C=((struct Cyc_Absyn_Upper_b_struct*)
_tmp11B)->f1;goto _LL15F;}else{goto _LL152;}_LL15F: _tmp119=_tmp114.f2;if((
unsigned int)_tmp119 > 1?*((int*)_tmp119)== 0: 0){_LL160: _tmp11A=((struct Cyc_Absyn_Upper_b_struct*)
_tmp119)->f1;goto _LL15A;}else{goto _LL152;}_LL154: return 0;_LL156: return - 1;_LL158:
return 1;_LL15A: {int i1=(int)Cyc_Evexp_eval_const_uint_exp(_tmp11C);int i2=(int)
Cyc_Evexp_eval_const_uint_exp(_tmp11A);if(i1 == i2){return 0;}if(i1 < i2){return - 1;}
return 1;}_LL152:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2){if(a1 == a2){
return 1;}{struct _tuple9 _tmp11E=({struct _tuple9 _tmp11D;_tmp11D.f1=a1;_tmp11D.f2=
a2;_tmp11D;});void*_tmp11F;int _tmp120;int _tmp121;void*_tmp122;void*_tmp123;int
_tmp124;int _tmp125;void*_tmp126;void*_tmp127;int _tmp128;void*_tmp129;int _tmp12A;
void*_tmp12B;int _tmp12C;void*_tmp12D;int _tmp12E;void*_tmp12F;struct _tagged_arr
_tmp130;void*_tmp131;struct _tagged_arr _tmp132;_LL164: _LL172: _tmp123=_tmp11E.f1;
if((unsigned int)_tmp123 > 16?*((int*)_tmp123)== 3: 0){_LL175: _tmp126=(void*)((
struct Cyc_Absyn_Format_att_struct*)_tmp123)->f1;goto _LL174;_LL174: _tmp125=((
struct Cyc_Absyn_Format_att_struct*)_tmp123)->f2;goto _LL173;_LL173: _tmp124=((
struct Cyc_Absyn_Format_att_struct*)_tmp123)->f3;goto _LL16E;}else{goto _LL166;}
_LL16E: _tmp11F=_tmp11E.f2;if((unsigned int)_tmp11F > 16?*((int*)_tmp11F)== 3: 0){
_LL171: _tmp122=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp11F)->f1;goto
_LL170;_LL170: _tmp121=((struct Cyc_Absyn_Format_att_struct*)_tmp11F)->f2;goto
_LL16F;_LL16F: _tmp120=((struct Cyc_Absyn_Format_att_struct*)_tmp11F)->f3;goto
_LL165;}else{goto _LL166;}_LL166: _LL178: _tmp129=_tmp11E.f1;if((unsigned int)
_tmp129 > 16?*((int*)_tmp129)== 0: 0){_LL179: _tmp12A=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp129)->f1;goto _LL176;}else{goto _LL168;}_LL176: _tmp127=_tmp11E.f2;if((
unsigned int)_tmp127 > 16?*((int*)_tmp127)== 0: 0){_LL177: _tmp128=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp127)->f1;goto _LL167;}else{goto _LL168;}_LL168: _LL17C: _tmp12D=_tmp11E.f1;if((
unsigned int)_tmp12D > 16?*((int*)_tmp12D)== 1: 0){_LL17D: _tmp12E=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp12D)->f1;goto _LL17A;}else{goto _LL16A;}_LL17A: _tmp12B=_tmp11E.f2;if((
unsigned int)_tmp12B > 16?*((int*)_tmp12B)== 1: 0){_LL17B: _tmp12C=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp12B)->f1;goto _LL169;}else{goto _LL16A;}_LL16A: _LL180: _tmp131=_tmp11E.f1;if((
unsigned int)_tmp131 > 16?*((int*)_tmp131)== 2: 0){_LL181: _tmp132=((struct Cyc_Absyn_Section_att_struct*)
_tmp131)->f1;goto _LL17E;}else{goto _LL16C;}_LL17E: _tmp12F=_tmp11E.f2;if((
unsigned int)_tmp12F > 16?*((int*)_tmp12F)== 2: 0){_LL17F: _tmp130=((struct Cyc_Absyn_Section_att_struct*)
_tmp12F)->f1;goto _LL16B;}else{goto _LL16C;}_LL16C: goto _LL16D;_LL165: return(
_tmp126 == _tmp122? _tmp125 == _tmp121: 0)? _tmp124 == _tmp120: 0;_LL167: _tmp12E=
_tmp12A;_tmp12C=_tmp128;goto _LL169;_LL169: return _tmp12E == _tmp12C;_LL16B: return
Cyc_Std_strcmp(_tmp132,_tmp130)== 0;_LL16D: return 0;_LL163:;}}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a
!= 0;a=a->tl){if(! Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2)){
return 0;}}}{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){if(! Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)a->hd,a1)){return 0;}}}return 1;}static void*Cyc_Tcutil_normalize_effect(
void*e);static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Aggrfield*af){return Cyc_Tcutil_rgns_of((void*)af->type);}static
struct _tuple6*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){void*t;{void*
_tmp133=Cyc_Tcutil_tvar_kind(tv);_LL183: if((int)_tmp133 == 3){goto _LL184;}else{
goto _LL185;}_LL185: if((int)_tmp133 == 4){goto _LL186;}else{goto _LL187;}_LL187: goto
_LL188;_LL184: t=(void*)2;goto _LL182;_LL186: t=Cyc_Absyn_empty_effect;goto _LL182;
_LL188: t=Cyc_Absyn_sint_t;goto _LL182;_LL182:;}return({struct _tuple6*_tmp134=
_cycalloc(sizeof(*_tmp134));_tmp134->f1=tv;_tmp134->f2=t;_tmp134;});}static void*
Cyc_Tcutil_rgns_of(void*t){void*_tmp135=Cyc_Tcutil_compress(t);void*_tmp136;
struct Cyc_Absyn_TunionInfo _tmp137;void*_tmp138;struct Cyc_List_List*_tmp139;
struct Cyc_Absyn_PtrInfo _tmp13A;void*_tmp13B;void*_tmp13C;void*_tmp13D;struct Cyc_List_List*
_tmp13E;struct Cyc_Absyn_TunionFieldInfo _tmp13F;struct Cyc_List_List*_tmp140;
struct Cyc_Absyn_AggrInfo _tmp141;struct Cyc_List_List*_tmp142;struct Cyc_List_List*
_tmp143;void*_tmp144;struct Cyc_Absyn_FnInfo _tmp145;struct Cyc_List_List*_tmp146;
struct Cyc_Absyn_VarargInfo*_tmp147;struct Cyc_List_List*_tmp148;void*_tmp149;
struct Cyc_Core_Opt*_tmp14A;struct Cyc_List_List*_tmp14B;void*_tmp14C;_LL18A: if((
int)_tmp135 == 0){goto _LL18B;}else{goto _LL18C;}_LL18C: if((int)_tmp135 == 1){goto
_LL18D;}else{goto _LL18E;}_LL18E: if((unsigned int)_tmp135 > 3?*((int*)_tmp135)== 6:
0){goto _LL18F;}else{goto _LL190;}_LL190: if((unsigned int)_tmp135 > 3?*((int*)
_tmp135)== 12: 0){goto _LL191;}else{goto _LL192;}_LL192: if((unsigned int)_tmp135 > 3?*((
int*)_tmp135)== 13: 0){goto _LL193;}else{goto _LL194;}_LL194: if((unsigned int)
_tmp135 > 3?*((int*)_tmp135)== 5: 0){goto _LL195;}else{goto _LL196;}_LL196: if((
unsigned int)_tmp135 > 3?*((int*)_tmp135)== 0: 0){goto _LL197;}else{goto _LL198;}
_LL198: if((unsigned int)_tmp135 > 3?*((int*)_tmp135)== 1: 0){goto _LL199;}else{goto
_LL19A;}_LL19A: if((unsigned int)_tmp135 > 3?*((int*)_tmp135)== 15: 0){_LL1B8:
_tmp136=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp135)->f1;goto _LL19B;}
else{goto _LL19C;}_LL19C: if((unsigned int)_tmp135 > 3?*((int*)_tmp135)== 2: 0){
_LL1B9: _tmp137=((struct Cyc_Absyn_TunionType_struct*)_tmp135)->f1;_LL1BB: _tmp139=
_tmp137.targs;goto _LL1BA;_LL1BA: _tmp138=(void*)_tmp137.rgn;goto _LL19D;}else{goto
_LL19E;}_LL19E: if((unsigned int)_tmp135 > 3?*((int*)_tmp135)== 4: 0){_LL1BC:
_tmp13A=((struct Cyc_Absyn_PointerType_struct*)_tmp135)->f1;_LL1BE: _tmp13C=(void*)
_tmp13A.elt_typ;goto _LL1BD;_LL1BD: _tmp13B=(void*)_tmp13A.rgn_typ;goto _LL19F;}
else{goto _LL1A0;}_LL1A0: if((unsigned int)_tmp135 > 3?*((int*)_tmp135)== 7: 0){
_LL1BF: _tmp13D=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp135)->f1;goto
_LL1A1;}else{goto _LL1A2;}_LL1A2: if((unsigned int)_tmp135 > 3?*((int*)_tmp135)== 9:
0){_LL1C0: _tmp13E=((struct Cyc_Absyn_TupleType_struct*)_tmp135)->f1;goto _LL1A3;}
else{goto _LL1A4;}_LL1A4: if((unsigned int)_tmp135 > 3?*((int*)_tmp135)== 3: 0){
_LL1C1: _tmp13F=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp135)->f1;_LL1C2:
_tmp140=_tmp13F.targs;goto _LL1A5;}else{goto _LL1A6;}_LL1A6: if((unsigned int)
_tmp135 > 3?*((int*)_tmp135)== 10: 0){_LL1C3: _tmp141=((struct Cyc_Absyn_AggrType_struct*)
_tmp135)->f1;_LL1C4: _tmp142=_tmp141.targs;goto _LL1A7;}else{goto _LL1A8;}_LL1A8:
if((unsigned int)_tmp135 > 3?*((int*)_tmp135)== 11: 0){_LL1C5: _tmp143=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp135)->f2;goto _LL1A9;}else{goto _LL1AA;}_LL1AA: if((unsigned int)_tmp135 > 3?*((
int*)_tmp135)== 14: 0){_LL1C6: _tmp144=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp135)->f1;goto _LL1AB;}else{goto _LL1AC;}_LL1AC: if((unsigned int)_tmp135 > 3?*((
int*)_tmp135)== 8: 0){_LL1C7: _tmp145=((struct Cyc_Absyn_FnType_struct*)_tmp135)->f1;
_LL1CD: _tmp14B=_tmp145.tvars;goto _LL1CC;_LL1CC: _tmp14A=_tmp145.effect;goto _LL1CB;
_LL1CB: _tmp149=(void*)_tmp145.ret_typ;goto _LL1CA;_LL1CA: _tmp148=_tmp145.args;
goto _LL1C9;_LL1C9: _tmp147=_tmp145.cyc_varargs;goto _LL1C8;_LL1C8: _tmp146=_tmp145.rgn_po;
goto _LL1AD;}else{goto _LL1AE;}_LL1AE: if((int)_tmp135 == 2){goto _LL1AF;}else{goto
_LL1B0;}_LL1B0: if((unsigned int)_tmp135 > 3?*((int*)_tmp135)== 17: 0){goto _LL1B1;}
else{goto _LL1B2;}_LL1B2: if((unsigned int)_tmp135 > 3?*((int*)_tmp135)== 18: 0){
goto _LL1B3;}else{goto _LL1B4;}_LL1B4: if((unsigned int)_tmp135 > 3?*((int*)_tmp135)
== 19: 0){_LL1CE: _tmp14C=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp135)->f1;
goto _LL1B5;}else{goto _LL1B6;}_LL1B6: if((unsigned int)_tmp135 > 3?*((int*)_tmp135)
== 16: 0){goto _LL1B7;}else{goto _LL189;}_LL18B: goto _LL18D;_LL18D: goto _LL18F;_LL18F:
goto _LL191;_LL191: goto _LL193;_LL193: goto _LL195;_LL195: return Cyc_Absyn_empty_effect;
_LL197: goto _LL199;_LL199: {void*_tmp14D=Cyc_Tcutil_typ_kind(t);_LL1D0: if((int)
_tmp14D == 3){goto _LL1D1;}else{goto _LL1D2;}_LL1D2: if((int)_tmp14D == 4){goto _LL1D3;}
else{goto _LL1D4;}_LL1D4: goto _LL1D5;_LL1D1: return(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp14F;_tmp14F.tag=17;_tmp14F.f1=(void*)t;_tmp14F;});_tmp14E;});_LL1D3: return t;
_LL1D5: return(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp150=_cycalloc(sizeof(*
_tmp150));_tmp150[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp151;_tmp151.tag=19;
_tmp151.f1=(void*)t;_tmp151;});_tmp150;});_LL1CF:;}_LL19B: return(void*)({struct
Cyc_Absyn_AccessEff_struct*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp153;_tmp153.tag=17;_tmp153.f1=(void*)_tmp136;
_tmp153;});_tmp152;});_LL19D: {struct Cyc_List_List*ts=({struct Cyc_List_List*
_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp158;_tmp158.tag=17;_tmp158.f1=(void*)_tmp138;_tmp158;});_tmp157;}));_tmp156->tl=
Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp139);_tmp156;});return Cyc_Tcutil_normalize_effect((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp154=_cycalloc(sizeof(*_tmp154));
_tmp154[0]=({struct Cyc_Absyn_JoinEff_struct _tmp155;_tmp155.tag=18;_tmp155.f1=ts;
_tmp155;});_tmp154;}));}_LL19F: return Cyc_Tcutil_normalize_effect((void*)({struct
Cyc_Absyn_JoinEff_struct*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159[0]=({struct
Cyc_Absyn_JoinEff_struct _tmp15A;_tmp15A.tag=18;_tmp15A.f1=({void*_tmp15B[2];
_tmp15B[1]=Cyc_Tcutil_rgns_of(_tmp13C);_tmp15B[0]=(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp15D;_tmp15D.tag=17;_tmp15D.f1=(void*)_tmp13B;_tmp15D;});_tmp15C;});Cyc_List_list(
_tag_arr(_tmp15B,sizeof(void*),2));});_tmp15A;});_tmp159;}));_LL1A1: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp13D));_LL1A3: {struct Cyc_List_List*_tmp15E=0;for(0;
_tmp13E != 0;_tmp13E=_tmp13E->tl){_tmp15E=({struct Cyc_List_List*_tmp15F=_cycalloc(
sizeof(*_tmp15F));_tmp15F->hd=(void*)(*((struct _tuple4*)_tmp13E->hd)).f2;_tmp15F->tl=
_tmp15E;_tmp15F;});}_tmp140=_tmp15E;goto _LL1A5;}_LL1A5: _tmp142=_tmp140;goto
_LL1A7;_LL1A7: return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp160=_cycalloc(sizeof(*_tmp160));_tmp160[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp161;_tmp161.tag=18;_tmp161.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,_tmp142);
_tmp161;});_tmp160;}));_LL1A9: return Cyc_Tcutil_normalize_effect((void*)({struct
Cyc_Absyn_JoinEff_struct*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162[0]=({struct
Cyc_Absyn_JoinEff_struct _tmp163;_tmp163.tag=18;_tmp163.f1=((struct Cyc_List_List*(*)(
void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,
_tmp143);_tmp163;});_tmp162;}));_LL1AB: return Cyc_Tcutil_rgns_of(_tmp144);_LL1AD: {
void*_tmp164=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp14B),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp14A))->v);return Cyc_Tcutil_normalize_effect(
_tmp164);}_LL1AF: return Cyc_Absyn_empty_effect;_LL1B1: goto _LL1B3;_LL1B3: return t;
_LL1B5: return Cyc_Tcutil_rgns_of(_tmp14C);_LL1B7:({void*_tmp165[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typedef in compressed type",
sizeof(unsigned char),27),_tag_arr(_tmp165,sizeof(void*),0));});_LL189:;}static
void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{void*_tmp166=
e;struct Cyc_List_List*_tmp167;struct Cyc_List_List**_tmp168;void*_tmp169;_LL1D7:
if((unsigned int)_tmp166 > 3?*((int*)_tmp166)== 18: 0){_LL1DD: _tmp167=((struct Cyc_Absyn_JoinEff_struct*)
_tmp166)->f1;_tmp168=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_struct*)
_tmp166)->f1;goto _LL1D8;}else{goto _LL1D9;}_LL1D9: if((unsigned int)_tmp166 > 3?*((
int*)_tmp166)== 19: 0){_LL1DE: _tmp169=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp166)->f1;goto _LL1DA;}else{goto _LL1DB;}_LL1DB: goto _LL1DC;_LL1D8: {int
nested_join=1;{struct Cyc_List_List*effs=*_tmp168;for(0;effs != 0;effs=effs->tl){
void*_tmp16A=(void*)effs->hd;(void*)(effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(
_tmp16A)));{void*_tmp16B=(void*)effs->hd;_LL1E0: if((unsigned int)_tmp16B > 3?*((
int*)_tmp16B)== 18: 0){goto _LL1E1;}else{goto _LL1E2;}_LL1E2: goto _LL1E3;_LL1E1:
nested_join=1;goto _LL1DF;_LL1E3: goto _LL1DF;_LL1DF:;}}}if(! nested_join){return e;}{
struct Cyc_List_List*effects=0;{struct Cyc_List_List*effs=*_tmp168;for(0;effs != 0;
effs=effs->tl){void*_tmp16C=Cyc_Tcutil_compress((void*)effs->hd);struct Cyc_List_List*
_tmp16D;void*_tmp16E;_LL1E5: if((unsigned int)_tmp16C > 3?*((int*)_tmp16C)== 18: 0){
_LL1EB: _tmp16D=((struct Cyc_Absyn_JoinEff_struct*)_tmp16C)->f1;goto _LL1E6;}else{
goto _LL1E7;}_LL1E7: if((unsigned int)_tmp16C > 3?*((int*)_tmp16C)== 17: 0){_LL1EC:
_tmp16E=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp16C)->f1;if((int)_tmp16E
== 2){goto _LL1E8;}else{goto _LL1E9;}}else{goto _LL1E9;}_LL1E9: goto _LL1EA;_LL1E6:
effects=Cyc_List_revappend(_tmp16D,effects);goto _LL1E4;_LL1E8: goto _LL1E4;_LL1EA:
effects=({struct Cyc_List_List*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->hd=(
void*)_tmp16C;_tmp16F->tl=effects;_tmp16F;});goto _LL1E4;_LL1E4:;}}*_tmp168=Cyc_List_imp_rev(
effects);return e;}}_LL1DA: return Cyc_Tcutil_rgns_of(_tmp169);_LL1DC: return e;
_LL1D6:;}}struct _tuple10{void*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;}
;static struct _tuple10*Cyc_Tcutil_get_effect_evar(void*t){void*_tmp170=Cyc_Tcutil_compress(
t);struct Cyc_List_List*_tmp171;struct Cyc_List_List _tmp172;struct Cyc_List_List*
_tmp173;void*_tmp174;struct Cyc_Core_Opt*_tmp175;struct Cyc_Core_Opt*_tmp176;
_LL1EE: if((unsigned int)_tmp170 > 3?*((int*)_tmp170)== 18: 0){_LL1F4: _tmp171=((
struct Cyc_Absyn_JoinEff_struct*)_tmp170)->f1;if(_tmp171 == 0){goto _LL1F0;}else{
_tmp172=*_tmp171;_LL1F6: _tmp174=(void*)_tmp172.hd;goto _LL1F5;_LL1F5: _tmp173=
_tmp172.tl;goto _LL1EF;}}else{goto _LL1F0;}_LL1F0: if((unsigned int)_tmp170 > 3?*((
int*)_tmp170)== 0: 0){_LL1F8: _tmp176=((struct Cyc_Absyn_Evar_struct*)_tmp170)->f1;
goto _LL1F7;_LL1F7: _tmp175=((struct Cyc_Absyn_Evar_struct*)_tmp170)->f4;goto _LL1F1;}
else{goto _LL1F2;}_LL1F2: goto _LL1F3;_LL1EF: {void*_tmp177=Cyc_Tcutil_compress(
_tmp174);struct Cyc_Core_Opt*_tmp178;_LL1FA: if((unsigned int)_tmp177 > 3?*((int*)
_tmp177)== 0: 0){_LL1FE: _tmp178=((struct Cyc_Absyn_Evar_struct*)_tmp177)->f4;goto
_LL1FB;}else{goto _LL1FC;}_LL1FC: goto _LL1FD;_LL1FB: return({struct _tuple10*_tmp179=
_cycalloc(sizeof(*_tmp179));_tmp179->f1=_tmp174;_tmp179->f2=_tmp173;_tmp179->f3=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp178))->v;_tmp179;});
_LL1FD: return 0;_LL1F9:;}_LL1F1: if(_tmp176 == 0? 1:(void*)_tmp176->v != (void*)4){({
void*_tmp17A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(unsigned char),27),_tag_arr(_tmp17A,
sizeof(void*),0));});}return({struct _tuple10*_tmp17B=_cycalloc(sizeof(*_tmp17B));
_tmp17B->f1=t;_tmp17B->f2=0;_tmp17B->f3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp175))->v;_tmp17B;});_LL1F3: return 0;_LL1ED:;}static struct Cyc_Core_Opt
Cyc_Tcutil_ek={(void*)((void*)4)};static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_struct*_tmp17C=({struct Cyc_Absyn_FnType_struct*_tmp17D=
_cycalloc(sizeof(*_tmp17D));_tmp17D[0]=({struct Cyc_Absyn_FnType_struct _tmp17E;
_tmp17E.tag=8;_tmp17E.f1=({struct Cyc_Absyn_FnInfo _tmp17F;_tmp17F.tvars=0;_tmp17F.effect=({
struct Cyc_Core_Opt*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->v=(void*)eff;
_tmp180;});_tmp17F.ret_typ=(void*)((void*)0);_tmp17F.args=0;_tmp17F.c_varargs=0;
_tmp17F.cyc_varargs=0;_tmp17F.rgn_po=0;_tmp17F.attributes=0;_tmp17F;});_tmp17E;});
_tmp17D;});return Cyc_Absyn_atb_typ((void*)_tmp17C,(void*)2,Cyc_Absyn_empty_tqual(),
Cyc_Absyn_bounds_one);}int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);if(r == (void*)2){return 1;}{void*_tmp181=Cyc_Tcutil_compress(
e);void*_tmp182;struct Cyc_List_List*_tmp183;void*_tmp184;struct Cyc_Core_Opt*
_tmp185;struct Cyc_Core_Opt*_tmp186;struct Cyc_Core_Opt**_tmp187;struct Cyc_Core_Opt*
_tmp188;_LL200: if((unsigned int)_tmp181 > 3?*((int*)_tmp181)== 17: 0){_LL20A:
_tmp182=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp181)->f1;goto _LL201;}
else{goto _LL202;}_LL202: if((unsigned int)_tmp181 > 3?*((int*)_tmp181)== 18: 0){
_LL20B: _tmp183=((struct Cyc_Absyn_JoinEff_struct*)_tmp181)->f1;goto _LL203;}else{
goto _LL204;}_LL204: if((unsigned int)_tmp181 > 3?*((int*)_tmp181)== 19: 0){_LL20C:
_tmp184=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp181)->f1;goto _LL205;}else{
goto _LL206;}_LL206: if((unsigned int)_tmp181 > 3?*((int*)_tmp181)== 0: 0){_LL20F:
_tmp188=((struct Cyc_Absyn_Evar_struct*)_tmp181)->f1;goto _LL20E;_LL20E: _tmp186=((
struct Cyc_Absyn_Evar_struct*)_tmp181)->f2;_tmp187=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp181)->f2;goto _LL20D;_LL20D: _tmp185=((struct Cyc_Absyn_Evar_struct*)
_tmp181)->f4;goto _LL207;}else{goto _LL208;}_LL208: goto _LL209;_LL201: if(constrain){
return Cyc_Tcutil_unify(r,_tmp182);}_tmp182=Cyc_Tcutil_compress(_tmp182);if(r == 
_tmp182){return 1;}{struct _tuple9 _tmp18A=({struct _tuple9 _tmp189;_tmp189.f1=r;
_tmp189.f2=_tmp182;_tmp189;});void*_tmp18B;struct Cyc_Absyn_Tvar*_tmp18C;void*
_tmp18D;struct Cyc_Absyn_Tvar*_tmp18E;_LL211: _LL217: _tmp18D=_tmp18A.f1;if((
unsigned int)_tmp18D > 3?*((int*)_tmp18D)== 1: 0){_LL218: _tmp18E=((struct Cyc_Absyn_VarType_struct*)
_tmp18D)->f1;goto _LL215;}else{goto _LL213;}_LL215: _tmp18B=_tmp18A.f2;if((
unsigned int)_tmp18B > 3?*((int*)_tmp18B)== 1: 0){_LL216: _tmp18C=((struct Cyc_Absyn_VarType_struct*)
_tmp18B)->f1;goto _LL212;}else{goto _LL213;}_LL213: goto _LL214;_LL212: return Cyc_Absyn_tvar_cmp(
_tmp18E,_tmp18C)== 0;_LL214: return 0;_LL210:;}_LL203: for(0;_tmp183 != 0;_tmp183=
_tmp183->tl){if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp183->hd)){
return 1;}}return 0;_LL205: {void*_tmp18F=Cyc_Tcutil_rgns_of(_tmp184);void*_tmp190;
_LL21A: if((unsigned int)_tmp18F > 3?*((int*)_tmp18F)== 19: 0){_LL21E: _tmp190=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp18F)->f1;goto _LL21B;}else{goto _LL21C;}_LL21C:
goto _LL21D;_LL21B: if(! constrain){return 0;}{void*_tmp191=Cyc_Tcutil_compress(
_tmp190);struct Cyc_Core_Opt*_tmp192;struct Cyc_Core_Opt*_tmp193;struct Cyc_Core_Opt**
_tmp194;struct Cyc_Core_Opt*_tmp195;_LL220: if((unsigned int)_tmp191 > 3?*((int*)
_tmp191)== 0: 0){_LL226: _tmp195=((struct Cyc_Absyn_Evar_struct*)_tmp191)->f1;goto
_LL225;_LL225: _tmp193=((struct Cyc_Absyn_Evar_struct*)_tmp191)->f2;_tmp194=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp191)->f2;goto _LL224;
_LL224: _tmp192=((struct Cyc_Absyn_Evar_struct*)_tmp191)->f4;goto _LL221;}else{goto
_LL222;}_LL222: goto _LL223;_LL221: {void*_tmp196=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_ek,_tmp192);Cyc_Tcutil_occurs(_tmp196,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp192))->v,r);{void*_tmp197=Cyc_Tcutil_dummy_fntype((
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp199=_cycalloc(sizeof(*_tmp199));
_tmp199[0]=({struct Cyc_Absyn_JoinEff_struct _tmp19A;_tmp19A.tag=18;_tmp19A.f1=({
void*_tmp19B[2];_tmp19B[1]=(void*)({struct Cyc_Absyn_AccessEff_struct*_tmp19C=
_cycalloc(sizeof(*_tmp19C));_tmp19C[0]=({struct Cyc_Absyn_AccessEff_struct _tmp19D;
_tmp19D.tag=17;_tmp19D.f1=(void*)r;_tmp19D;});_tmp19C;});_tmp19B[0]=_tmp196;Cyc_List_list(
_tag_arr(_tmp19B,sizeof(void*),2));});_tmp19A;});_tmp199;}));*_tmp194=({struct
Cyc_Core_Opt*_tmp198=_cycalloc(sizeof(*_tmp198));_tmp198->v=(void*)_tmp197;
_tmp198;});return 1;}}_LL223: return 0;_LL21F:;}_LL21D: return Cyc_Tcutil_region_in_effect(
constrain,r,_tmp18F);_LL219:;}_LL207: if(_tmp188 == 0? 1:(void*)_tmp188->v != (void*)
4){({void*_tmp19E[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(unsigned char),27),_tag_arr(_tmp19E,
sizeof(void*),0));});}if(! constrain){return 0;}{void*_tmp19F=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp185);Cyc_Tcutil_occurs(_tmp19F,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp185))->v,r);{struct
Cyc_Absyn_JoinEff_struct*_tmp1A0=({struct Cyc_Absyn_JoinEff_struct*_tmp1A2=
_cycalloc(sizeof(*_tmp1A2));_tmp1A2[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1A3;
_tmp1A3.tag=18;_tmp1A3.f1=({struct Cyc_List_List*_tmp1A4=_cycalloc(sizeof(*
_tmp1A4));_tmp1A4->hd=(void*)_tmp19F;_tmp1A4->tl=({struct Cyc_List_List*_tmp1A5=
_cycalloc(sizeof(*_tmp1A5));_tmp1A5->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp1A7;_tmp1A7.tag=17;_tmp1A7.f1=(void*)r;_tmp1A7;});_tmp1A6;}));_tmp1A5->tl=0;
_tmp1A5;});_tmp1A4;});_tmp1A3;});_tmp1A2;});*_tmp187=({struct Cyc_Core_Opt*
_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1->v=(void*)((void*)_tmp1A0);_tmp1A1;});
return 1;}}_LL209: return 0;_LL1FF:;}}static int Cyc_Tcutil_type_in_effect(int
constrain,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp1A8=Cyc_Tcutil_compress(
e);struct Cyc_List_List*_tmp1A9;void*_tmp1AA;struct Cyc_Core_Opt*_tmp1AB;struct Cyc_Core_Opt*
_tmp1AC;struct Cyc_Core_Opt**_tmp1AD;struct Cyc_Core_Opt*_tmp1AE;_LL228: if((
unsigned int)_tmp1A8 > 3?*((int*)_tmp1A8)== 17: 0){goto _LL229;}else{goto _LL22A;}
_LL22A: if((unsigned int)_tmp1A8 > 3?*((int*)_tmp1A8)== 18: 0){_LL232: _tmp1A9=((
struct Cyc_Absyn_JoinEff_struct*)_tmp1A8)->f1;goto _LL22B;}else{goto _LL22C;}_LL22C:
if((unsigned int)_tmp1A8 > 3?*((int*)_tmp1A8)== 19: 0){_LL233: _tmp1AA=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1A8)->f1;goto _LL22D;}else{goto _LL22E;}_LL22E:
if((unsigned int)_tmp1A8 > 3?*((int*)_tmp1A8)== 0: 0){_LL236: _tmp1AE=((struct Cyc_Absyn_Evar_struct*)
_tmp1A8)->f1;goto _LL235;_LL235: _tmp1AC=((struct Cyc_Absyn_Evar_struct*)_tmp1A8)->f2;
_tmp1AD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1A8)->f2;goto
_LL234;_LL234: _tmp1AB=((struct Cyc_Absyn_Evar_struct*)_tmp1A8)->f4;goto _LL22F;}
else{goto _LL230;}_LL230: goto _LL231;_LL229: return 0;_LL22B: for(0;_tmp1A9 != 0;
_tmp1A9=_tmp1A9->tl){if(Cyc_Tcutil_type_in_effect(constrain,t,(void*)_tmp1A9->hd)){
return 1;}}return 0;_LL22D: _tmp1AA=Cyc_Tcutil_compress(_tmp1AA);if(t == _tmp1AA){
return 1;}if(constrain){return Cyc_Tcutil_unify(t,_tmp1AA);}{void*_tmp1AF=Cyc_Tcutil_rgns_of(
t);void*_tmp1B0;_LL238: if((unsigned int)_tmp1AF > 3?*((int*)_tmp1AF)== 19: 0){
_LL23C: _tmp1B0=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1AF)->f1;goto _LL239;}
else{goto _LL23A;}_LL23A: goto _LL23B;_LL239: {struct _tuple9 _tmp1B2=({struct _tuple9
_tmp1B1;_tmp1B1.f1=t;_tmp1B1.f2=Cyc_Tcutil_compress(_tmp1B0);_tmp1B1;});void*
_tmp1B3;struct Cyc_Absyn_Tvar*_tmp1B4;void*_tmp1B5;struct Cyc_Absyn_Tvar*_tmp1B6;
_LL23E: _LL244: _tmp1B5=_tmp1B2.f1;if((unsigned int)_tmp1B5 > 3?*((int*)_tmp1B5)== 
1: 0){_LL245: _tmp1B6=((struct Cyc_Absyn_VarType_struct*)_tmp1B5)->f1;goto _LL242;}
else{goto _LL240;}_LL242: _tmp1B3=_tmp1B2.f2;if((unsigned int)_tmp1B3 > 3?*((int*)
_tmp1B3)== 1: 0){_LL243: _tmp1B4=((struct Cyc_Absyn_VarType_struct*)_tmp1B3)->f1;
goto _LL23F;}else{goto _LL240;}_LL240: goto _LL241;_LL23F: return Cyc_Tcutil_unify(t,
_tmp1B0);_LL241: return t == _tmp1B0;_LL23D:;}_LL23B: return Cyc_Tcutil_type_in_effect(
constrain,t,_tmp1AF);_LL237:;}_LL22F: if(_tmp1AE == 0? 1:(void*)_tmp1AE->v != (void*)
4){({void*_tmp1B7[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(unsigned char),27),_tag_arr(_tmp1B7,
sizeof(void*),0));});}if(! constrain){return 0;}{void*_tmp1B8=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1AB);Cyc_Tcutil_occurs(_tmp1B8,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1AB))->v,t);{struct
Cyc_Absyn_JoinEff_struct*_tmp1B9=({struct Cyc_Absyn_JoinEff_struct*_tmp1BB=
_cycalloc(sizeof(*_tmp1BB));_tmp1BB[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1BC;
_tmp1BC.tag=18;_tmp1BC.f1=({struct Cyc_List_List*_tmp1BD=_cycalloc(sizeof(*
_tmp1BD));_tmp1BD->hd=(void*)_tmp1B8;_tmp1BD->tl=({struct Cyc_List_List*_tmp1BE=
_cycalloc(sizeof(*_tmp1BE));_tmp1BE->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp1C0;_tmp1C0.tag=19;_tmp1C0.f1=(void*)t;_tmp1C0;});_tmp1BF;}));_tmp1BE->tl=0;
_tmp1BE;});_tmp1BD;});_tmp1BC;});_tmp1BB;});*_tmp1AD=({struct Cyc_Core_Opt*
_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA->v=(void*)((void*)_tmp1B9);_tmp1BA;});
return 1;}}_LL231: return 0;_LL227:;}}static int Cyc_Tcutil_variable_in_effect(int
constrain,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{void*_tmp1C1=
e;struct Cyc_Absyn_Tvar*_tmp1C2;struct Cyc_List_List*_tmp1C3;void*_tmp1C4;struct
Cyc_Core_Opt*_tmp1C5;struct Cyc_Core_Opt*_tmp1C6;struct Cyc_Core_Opt**_tmp1C7;
struct Cyc_Core_Opt*_tmp1C8;_LL247: if((unsigned int)_tmp1C1 > 3?*((int*)_tmp1C1)== 
1: 0){_LL251: _tmp1C2=((struct Cyc_Absyn_VarType_struct*)_tmp1C1)->f1;goto _LL248;}
else{goto _LL249;}_LL249: if((unsigned int)_tmp1C1 > 3?*((int*)_tmp1C1)== 18: 0){
_LL252: _tmp1C3=((struct Cyc_Absyn_JoinEff_struct*)_tmp1C1)->f1;goto _LL24A;}else{
goto _LL24B;}_LL24B: if((unsigned int)_tmp1C1 > 3?*((int*)_tmp1C1)== 19: 0){_LL253:
_tmp1C4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1C1)->f1;goto _LL24C;}else{
goto _LL24D;}_LL24D: if((unsigned int)_tmp1C1 > 3?*((int*)_tmp1C1)== 0: 0){_LL256:
_tmp1C8=((struct Cyc_Absyn_Evar_struct*)_tmp1C1)->f1;goto _LL255;_LL255: _tmp1C6=((
struct Cyc_Absyn_Evar_struct*)_tmp1C1)->f2;_tmp1C7=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp1C1)->f2;goto _LL254;_LL254: _tmp1C5=((struct Cyc_Absyn_Evar_struct*)
_tmp1C1)->f4;goto _LL24E;}else{goto _LL24F;}_LL24F: goto _LL250;_LL248: return Cyc_Absyn_tvar_cmp(
v,_tmp1C2)== 0;_LL24A: for(0;_tmp1C3 != 0;_tmp1C3=_tmp1C3->tl){if(Cyc_Tcutil_variable_in_effect(
constrain,v,(void*)_tmp1C3->hd)){return 1;}}return 0;_LL24C: {void*_tmp1C9=Cyc_Tcutil_rgns_of(
_tmp1C4);void*_tmp1CA;_LL258: if((unsigned int)_tmp1C9 > 3?*((int*)_tmp1C9)== 19: 0){
_LL25C: _tmp1CA=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1C9)->f1;goto _LL259;}
else{goto _LL25A;}_LL25A: goto _LL25B;_LL259: if(! constrain){return 0;}{void*_tmp1CB=
Cyc_Tcutil_compress(_tmp1CA);struct Cyc_Core_Opt*_tmp1CC;struct Cyc_Core_Opt*
_tmp1CD;struct Cyc_Core_Opt**_tmp1CE;struct Cyc_Core_Opt*_tmp1CF;_LL25E: if((
unsigned int)_tmp1CB > 3?*((int*)_tmp1CB)== 0: 0){_LL264: _tmp1CF=((struct Cyc_Absyn_Evar_struct*)
_tmp1CB)->f1;goto _LL263;_LL263: _tmp1CD=((struct Cyc_Absyn_Evar_struct*)_tmp1CB)->f2;
_tmp1CE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1CB)->f2;goto
_LL262;_LL262: _tmp1CC=((struct Cyc_Absyn_Evar_struct*)_tmp1CB)->f4;goto _LL25F;}
else{goto _LL260;}_LL260: goto _LL261;_LL25F: {void*_tmp1D0=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp1CC);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1CC))->v,v)){return 0;}{void*_tmp1D1=Cyc_Tcutil_dummy_fntype((void*)({struct
Cyc_Absyn_JoinEff_struct*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3[0]=({struct
Cyc_Absyn_JoinEff_struct _tmp1D4;_tmp1D4.tag=18;_tmp1D4.f1=({void*_tmp1D5[2];
_tmp1D5[1]=(void*)({struct Cyc_Absyn_VarType_struct*_tmp1D6=_cycalloc(sizeof(*
_tmp1D6));_tmp1D6[0]=({struct Cyc_Absyn_VarType_struct _tmp1D7;_tmp1D7.tag=1;
_tmp1D7.f1=v;_tmp1D7;});_tmp1D6;});_tmp1D5[0]=_tmp1D0;Cyc_List_list(_tag_arr(
_tmp1D5,sizeof(void*),2));});_tmp1D4;});_tmp1D3;}));*_tmp1CE=({struct Cyc_Core_Opt*
_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->v=(void*)_tmp1D1;_tmp1D2;});return 1;}}
_LL261: return 0;_LL25D:;}_LL25B: return Cyc_Tcutil_variable_in_effect(constrain,v,
_tmp1C9);_LL257:;}_LL24E: if(_tmp1C8 == 0? 1:(void*)_tmp1C8->v != (void*)4){({void*
_tmp1D8[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("effect evar has wrong kind",sizeof(unsigned char),27),_tag_arr(_tmp1D8,
sizeof(void*),0));});}{void*_tmp1D9=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp1C5);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1C5))->v,v)){return 0;}{
struct Cyc_Absyn_JoinEff_struct*_tmp1DA=({struct Cyc_Absyn_JoinEff_struct*_tmp1DC=
_cycalloc(sizeof(*_tmp1DC));_tmp1DC[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1DD;
_tmp1DD.tag=18;_tmp1DD.f1=({struct Cyc_List_List*_tmp1DE=_cycalloc(sizeof(*
_tmp1DE));_tmp1DE->hd=(void*)_tmp1D9;_tmp1DE->tl=({struct Cyc_List_List*_tmp1DF=
_cycalloc(sizeof(*_tmp1DF));_tmp1DF->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0[0]=({struct Cyc_Absyn_VarType_struct
_tmp1E1;_tmp1E1.tag=1;_tmp1E1.f1=v;_tmp1E1;});_tmp1E0;}));_tmp1DF->tl=0;_tmp1DF;});
_tmp1DE;});_tmp1DD;});_tmp1DC;});*_tmp1C7=({struct Cyc_Core_Opt*_tmp1DB=_cycalloc(
sizeof(*_tmp1DB));_tmp1DB->v=(void*)((void*)_tmp1DA);_tmp1DB;});return 1;}}_LL250:
return 0;_LL246:;}}static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp1E2=e;struct Cyc_List_List*_tmp1E3;void*_tmp1E4;_LL266: if((
unsigned int)_tmp1E2 > 3?*((int*)_tmp1E2)== 18: 0){_LL26E: _tmp1E3=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1E2)->f1;goto _LL267;}else{goto _LL268;}_LL268: if((unsigned int)_tmp1E2 > 3?*((
int*)_tmp1E2)== 19: 0){_LL26F: _tmp1E4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp1E2)->f1;goto _LL269;}else{goto _LL26A;}_LL26A: if((unsigned int)_tmp1E2 > 3?*((
int*)_tmp1E2)== 0: 0){goto _LL26B;}else{goto _LL26C;}_LL26C: goto _LL26D;_LL267: for(0;
_tmp1E3 != 0;_tmp1E3=_tmp1E3->tl){if(Cyc_Tcutil_evar_in_effect(evar,(void*)
_tmp1E3->hd)){return 1;}}return 0;_LL269: {void*_tmp1E5=Cyc_Tcutil_rgns_of(_tmp1E4);
void*_tmp1E6;_LL271: if((unsigned int)_tmp1E5 > 3?*((int*)_tmp1E5)== 19: 0){_LL275:
_tmp1E6=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1E5)->f1;goto _LL272;}else{
goto _LL273;}_LL273: goto _LL274;_LL272: return 0;_LL274: return Cyc_Tcutil_evar_in_effect(
evar,_tmp1E5);_LL270:;}_LL26B: return evar == e;_LL26D: return 0;_LL265:;}}int Cyc_Tcutil_subset_effect(
int set_to_empty,void*e1,void*e2){void*_tmp1E7=Cyc_Tcutil_compress(e1);struct Cyc_List_List*
_tmp1E8;void*_tmp1E9;struct Cyc_Absyn_Tvar*_tmp1EA;void*_tmp1EB;struct Cyc_Core_Opt*
_tmp1EC;struct Cyc_Core_Opt*_tmp1ED;struct Cyc_Core_Opt**_tmp1EE;_LL277: if((
unsigned int)_tmp1E7 > 3?*((int*)_tmp1E7)== 18: 0){_LL283: _tmp1E8=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1E7)->f1;goto _LL278;}else{goto _LL279;}_LL279: if((unsigned int)_tmp1E7 > 3?*((
int*)_tmp1E7)== 17: 0){_LL284: _tmp1E9=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1E7)->f1;goto _LL27A;}else{goto _LL27B;}_LL27B: if((unsigned int)_tmp1E7 > 3?*((
int*)_tmp1E7)== 1: 0){_LL285: _tmp1EA=((struct Cyc_Absyn_VarType_struct*)_tmp1E7)->f1;
goto _LL27C;}else{goto _LL27D;}_LL27D: if((unsigned int)_tmp1E7 > 3?*((int*)_tmp1E7)
== 19: 0){_LL286: _tmp1EB=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1E7)->f1;
goto _LL27E;}else{goto _LL27F;}_LL27F: if((unsigned int)_tmp1E7 > 3?*((int*)_tmp1E7)
== 0: 0){_LL288: _tmp1ED=((struct Cyc_Absyn_Evar_struct*)_tmp1E7)->f2;_tmp1EE=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp1E7)->f2;goto _LL287;
_LL287: _tmp1EC=((struct Cyc_Absyn_Evar_struct*)_tmp1E7)->f4;goto _LL280;}else{goto
_LL281;}_LL281: goto _LL282;_LL278: for(0;_tmp1E8 != 0;_tmp1E8=_tmp1E8->tl){if(! Cyc_Tcutil_subset_effect(
set_to_empty,(void*)_tmp1E8->hd,e2)){return 0;}}return 1;_LL27A: return(Cyc_Tcutil_region_in_effect(
0,_tmp1E9,e2)? 1: Cyc_Tcutil_region_in_effect(1,_tmp1E9,e2))? 1: Cyc_Tcutil_unify(
_tmp1E9,(void*)2);_LL27C: return Cyc_Tcutil_variable_in_effect(0,_tmp1EA,e2)? 1: Cyc_Tcutil_variable_in_effect(
1,_tmp1EA,e2);_LL27E: {void*_tmp1EF=Cyc_Tcutil_rgns_of(_tmp1EB);void*_tmp1F0;
_LL28A: if((unsigned int)_tmp1EF > 3?*((int*)_tmp1EF)== 19: 0){_LL28E: _tmp1F0=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp1EF)->f1;goto _LL28B;}else{goto _LL28C;}_LL28C:
goto _LL28D;_LL28B: return(Cyc_Tcutil_type_in_effect(0,_tmp1F0,e2)? 1: Cyc_Tcutil_type_in_effect(
1,_tmp1F0,e2))? 1: Cyc_Tcutil_unify(_tmp1F0,Cyc_Absyn_sint_t);_LL28D: return Cyc_Tcutil_subset_effect(
set_to_empty,_tmp1EF,e2);_LL289:;}_LL280: if(! Cyc_Tcutil_evar_in_effect(e1,e2)){
if(set_to_empty){*_tmp1EE=({struct Cyc_Core_Opt*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));
_tmp1F1->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp1F2=_cycalloc(
sizeof(*_tmp1F2));_tmp1F2[0]=({struct Cyc_Absyn_JoinEff_struct _tmp1F3;_tmp1F3.tag=
18;_tmp1F3.f1=0;_tmp1F3;});_tmp1F2;}));_tmp1F1;});}else{Cyc_Tcutil_occurs(e1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1EC))->v,e2);*_tmp1EE=({
struct Cyc_Core_Opt*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));_tmp1F4->v=(void*)e2;
_tmp1F4;});}}return 1;_LL282:({struct Cyc_Std_String_pa_struct _tmp1F6;_tmp1F6.tag=
0;_tmp1F6.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e1);{void*_tmp1F5[1]={&
_tmp1F6};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("subset_effect: bad effect: %s",sizeof(unsigned char),30),_tag_arr(
_tmp1F5,sizeof(void*),1));}});_LL276:;}struct _tuple11{struct _tuple10*f1;struct
_tuple10*f2;};static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);{struct _tuple11 _tmp1F8=({struct _tuple11
_tmp1F7;_tmp1F7.f1=Cyc_Tcutil_get_effect_evar(e1);_tmp1F7.f2=Cyc_Tcutil_get_effect_evar(
e2);_tmp1F7;});_LL290: goto _LL291;_LL291: return Cyc_Tcutil_subset_effect(0,e1,e2)?
Cyc_Tcutil_subset_effect(0,e2,e1): 0;_LL28F:;}}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple9 _tmp1FA;void*_tmp1FB;void*_tmp1FC;struct
_tuple9*_tmp1F9=(struct _tuple9*)r1->hd;_tmp1FA=*_tmp1F9;_LL294: _tmp1FC=_tmp1FA.f1;
goto _LL293;_LL293: _tmp1FB=_tmp1FA.f2;goto _LL292;_LL292: {int found=_tmp1FC == (
void*)2;{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0? ! found: 0;r2=r2->tl){struct
_tuple9 _tmp1FE;void*_tmp1FF;void*_tmp200;struct _tuple9*_tmp1FD=(struct _tuple9*)
r2->hd;_tmp1FE=*_tmp1FD;_LL297: _tmp200=_tmp1FE.f1;goto _LL296;_LL296: _tmp1FF=
_tmp1FE.f2;goto _LL295;_LL295: if(Cyc_Tcutil_unify(_tmp1FC,_tmp200)? Cyc_Tcutil_unify(
_tmp1FB,_tmp1FF): 0){found=1;break;}}}if(! found){return 0;}}}}return 1;}static int
Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return
Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)? Cyc_Tcutil_sub_rgnpo(rpo2,rpo1): 0;}struct
_tuple12{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};struct
_tuple13{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _tagged_arr)_tag_arr(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2){return;}{void*_tmp201=t1;struct Cyc_Core_Opt*_tmp202;struct Cyc_Core_Opt*
_tmp203;struct Cyc_Core_Opt**_tmp204;struct Cyc_Core_Opt*_tmp205;_LL299: if((
unsigned int)_tmp201 > 3?*((int*)_tmp201)== 0: 0){_LL29F: _tmp205=((struct Cyc_Absyn_Evar_struct*)
_tmp201)->f1;goto _LL29E;_LL29E: _tmp203=((struct Cyc_Absyn_Evar_struct*)_tmp201)->f2;
_tmp204=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp201)->f2;goto
_LL29D;_LL29D: _tmp202=((struct Cyc_Absyn_Evar_struct*)_tmp201)->f4;goto _LL29A;}
else{goto _LL29B;}_LL29B: goto _LL29C;_LL29A: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp202))->v,t2);{void*
_tmp206=Cyc_Tcutil_typ_kind(t2);if(Cyc_Tcutil_kind_leq(_tmp206,(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp205))->v)){*_tmp204=({struct Cyc_Core_Opt*_tmp207=
_cycalloc(sizeof(*_tmp207));_tmp207->v=(void*)t2;_tmp207;});return;}else{{void*
_tmp208=t2;struct Cyc_Core_Opt*_tmp209;struct Cyc_Core_Opt*_tmp20A;struct Cyc_Core_Opt**
_tmp20B;struct Cyc_Absyn_PtrInfo _tmp20C;_LL2A1: if((unsigned int)_tmp208 > 3?*((int*)
_tmp208)== 0: 0){_LL2A8: _tmp20A=((struct Cyc_Absyn_Evar_struct*)_tmp208)->f2;
_tmp20B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp208)->f2;goto
_LL2A7;_LL2A7: _tmp209=((struct Cyc_Absyn_Evar_struct*)_tmp208)->f4;goto _LL2A2;}
else{goto _LL2A3;}_LL2A3: if((unsigned int)_tmp208 > 3?*((int*)_tmp208)== 4: 0){
_LL2AA: _tmp20C=((struct Cyc_Absyn_PointerType_struct*)_tmp208)->f1;goto _LL2A9;}
else{goto _LL2A5;}_LL2A9: if((void*)_tmp205->v == (void*)2){goto _LL2A4;}else{goto
_LL2A5;}_LL2A5: goto _LL2A6;_LL2A2: {struct Cyc_List_List*_tmp20D=(struct Cyc_List_List*)
_tmp202->v;{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp209))->v;for(0;s2 != 0;s2=s2->tl){if(!((int(*)(int(*compare)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp20D,(struct Cyc_Absyn_Tvar*)s2->hd)){
Cyc_Tcutil_failure_reason=_tag_arr("(type variable would escape scope)",sizeof(
unsigned char),35);(int)_throw((void*)Cyc_Tcutil_Unify);}}}if(Cyc_Tcutil_kind_leq((
void*)_tmp205->v,_tmp206)){*_tmp20B=({struct Cyc_Core_Opt*_tmp20E=_cycalloc(
sizeof(*_tmp20E));_tmp20E->v=(void*)t1;_tmp20E;});return;}Cyc_Tcutil_failure_reason=
_tag_arr("(kinds are incompatible)",sizeof(unsigned char),25);goto _LL2A0;}_LL2A4: {
struct Cyc_Absyn_Conref*_tmp20F=Cyc_Absyn_compress_conref(_tmp20C.bounds);{void*
_tmp210=(void*)_tmp20F->v;_LL2AC: if((int)_tmp210 == 0){goto _LL2AD;}else{goto
_LL2AE;}_LL2AE: goto _LL2AF;_LL2AD:(void*)(_tmp20F->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp212;_tmp212.tag=0;_tmp212.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp214;_tmp214.tag=0;_tmp214.f1=Cyc_Absyn_signed_int_exp(1,0);_tmp214;});
_tmp213;}));_tmp212;});_tmp211;})));*_tmp204=({struct Cyc_Core_Opt*_tmp215=
_cycalloc(sizeof(*_tmp215));_tmp215->v=(void*)t2;_tmp215;});return;_LL2AF: goto
_LL2AB;_LL2AB:;}goto _LL2A0;}_LL2A6: goto _LL2A0;_LL2A0:;}Cyc_Tcutil_failure_reason=
_tag_arr("(kinds are incompatible)",sizeof(unsigned char),25);(int)_throw((void*)
Cyc_Tcutil_Unify);}}_LL29C: goto _LL298;_LL298:;}{struct _tuple9 _tmp217=({struct
_tuple9 _tmp216;_tmp216.f1=t2;_tmp216.f2=t1;_tmp216;});void*_tmp218;void*_tmp219;
void*_tmp21A;void*_tmp21B;struct Cyc_Absyn_Tvar*_tmp21C;void*_tmp21D;struct Cyc_Absyn_Tvar*
_tmp21E;void*_tmp21F;struct Cyc_Absyn_AggrInfo _tmp220;struct Cyc_List_List*_tmp221;
void*_tmp222;void*_tmp223;struct Cyc_Absyn_AggrInfo _tmp224;struct Cyc_List_List*
_tmp225;void*_tmp226;void*_tmp227;struct _tuple1*_tmp228;void*_tmp229;struct
_tuple1*_tmp22A;void*_tmp22B;struct Cyc_List_List*_tmp22C;void*_tmp22D;struct Cyc_List_List*
_tmp22E;void*_tmp22F;struct Cyc_Absyn_TunionInfo _tmp230;void*_tmp231;struct Cyc_List_List*
_tmp232;void*_tmp233;struct Cyc_Absyn_Tuniondecl**_tmp234;struct Cyc_Absyn_Tuniondecl*
_tmp235;void*_tmp236;struct Cyc_Absyn_TunionInfo _tmp237;void*_tmp238;struct Cyc_List_List*
_tmp239;void*_tmp23A;struct Cyc_Absyn_Tuniondecl**_tmp23B;struct Cyc_Absyn_Tuniondecl*
_tmp23C;void*_tmp23D;struct Cyc_Absyn_TunionFieldInfo _tmp23E;struct Cyc_List_List*
_tmp23F;void*_tmp240;struct Cyc_Absyn_Tunionfield*_tmp241;struct Cyc_Absyn_Tuniondecl*
_tmp242;void*_tmp243;struct Cyc_Absyn_TunionFieldInfo _tmp244;struct Cyc_List_List*
_tmp245;void*_tmp246;struct Cyc_Absyn_Tunionfield*_tmp247;struct Cyc_Absyn_Tuniondecl*
_tmp248;void*_tmp249;struct Cyc_Absyn_PtrInfo _tmp24A;struct Cyc_Absyn_Conref*
_tmp24B;struct Cyc_Absyn_Tqual _tmp24C;struct Cyc_Absyn_Conref*_tmp24D;void*_tmp24E;
void*_tmp24F;void*_tmp250;struct Cyc_Absyn_PtrInfo _tmp251;struct Cyc_Absyn_Conref*
_tmp252;struct Cyc_Absyn_Tqual _tmp253;struct Cyc_Absyn_Conref*_tmp254;void*_tmp255;
void*_tmp256;void*_tmp257;void*_tmp258;void*_tmp259;void*_tmp25A;void*_tmp25B;
void*_tmp25C;void*_tmp25D;void*_tmp25E;void*_tmp25F;int _tmp260;void*_tmp261;int
_tmp262;void*_tmp263;void*_tmp264;void*_tmp265;void*_tmp266;void*_tmp267;struct
Cyc_Absyn_Exp*_tmp268;struct Cyc_Absyn_Tqual _tmp269;void*_tmp26A;void*_tmp26B;
struct Cyc_Absyn_Exp*_tmp26C;struct Cyc_Absyn_Tqual _tmp26D;void*_tmp26E;void*
_tmp26F;struct Cyc_Absyn_FnInfo _tmp270;struct Cyc_List_List*_tmp271;struct Cyc_List_List*
_tmp272;struct Cyc_Absyn_VarargInfo*_tmp273;int _tmp274;struct Cyc_List_List*
_tmp275;void*_tmp276;struct Cyc_Core_Opt*_tmp277;struct Cyc_List_List*_tmp278;void*
_tmp279;struct Cyc_Absyn_FnInfo _tmp27A;struct Cyc_List_List*_tmp27B;struct Cyc_List_List*
_tmp27C;struct Cyc_Absyn_VarargInfo*_tmp27D;int _tmp27E;struct Cyc_List_List*
_tmp27F;void*_tmp280;struct Cyc_Core_Opt*_tmp281;struct Cyc_List_List*_tmp282;void*
_tmp283;struct Cyc_List_List*_tmp284;void*_tmp285;struct Cyc_List_List*_tmp286;
void*_tmp287;struct Cyc_List_List*_tmp288;void*_tmp289;void*_tmp28A;struct Cyc_List_List*
_tmp28B;void*_tmp28C;void*_tmp28D;void*_tmp28E;void*_tmp28F;void*_tmp290;void*
_tmp291;void*_tmp292;void*_tmp293;void*_tmp294;void*_tmp295;void*_tmp296;void*
_tmp297;void*_tmp298;_LL2B1: _LL2E5: _tmp218=_tmp217.f1;if((unsigned int)_tmp218 > 
3?*((int*)_tmp218)== 0: 0){goto _LL2B2;}else{goto _LL2B3;}_LL2B3: _LL2E7: _tmp21A=
_tmp217.f1;if((int)_tmp21A == 0){goto _LL2E6;}else{goto _LL2B5;}_LL2E6: _tmp219=
_tmp217.f2;if((int)_tmp219 == 0){goto _LL2B4;}else{goto _LL2B5;}_LL2B5: _LL2EA:
_tmp21D=_tmp217.f1;if((unsigned int)_tmp21D > 3?*((int*)_tmp21D)== 1: 0){_LL2EB:
_tmp21E=((struct Cyc_Absyn_VarType_struct*)_tmp21D)->f1;goto _LL2E8;}else{goto
_LL2B7;}_LL2E8: _tmp21B=_tmp217.f2;if((unsigned int)_tmp21B > 3?*((int*)_tmp21B)== 
1: 0){_LL2E9: _tmp21C=((struct Cyc_Absyn_VarType_struct*)_tmp21B)->f1;goto _LL2B6;}
else{goto _LL2B7;}_LL2B7: _LL2F0: _tmp223=_tmp217.f1;if((unsigned int)_tmp223 > 3?*((
int*)_tmp223)== 10: 0){_LL2F1: _tmp224=((struct Cyc_Absyn_AggrType_struct*)_tmp223)->f1;
_LL2F3: _tmp226=(void*)_tmp224.aggr_info;goto _LL2F2;_LL2F2: _tmp225=_tmp224.targs;
goto _LL2EC;}else{goto _LL2B9;}_LL2EC: _tmp21F=_tmp217.f2;if((unsigned int)_tmp21F > 
3?*((int*)_tmp21F)== 10: 0){_LL2ED: _tmp220=((struct Cyc_Absyn_AggrType_struct*)
_tmp21F)->f1;_LL2EF: _tmp222=(void*)_tmp220.aggr_info;goto _LL2EE;_LL2EE: _tmp221=
_tmp220.targs;goto _LL2B8;}else{goto _LL2B9;}_LL2B9: _LL2F6: _tmp229=_tmp217.f1;if((
unsigned int)_tmp229 > 3?*((int*)_tmp229)== 12: 0){_LL2F7: _tmp22A=((struct Cyc_Absyn_EnumType_struct*)
_tmp229)->f1;goto _LL2F4;}else{goto _LL2BB;}_LL2F4: _tmp227=_tmp217.f2;if((
unsigned int)_tmp227 > 3?*((int*)_tmp227)== 12: 0){_LL2F5: _tmp228=((struct Cyc_Absyn_EnumType_struct*)
_tmp227)->f1;goto _LL2BA;}else{goto _LL2BB;}_LL2BB: _LL2FA: _tmp22D=_tmp217.f1;if((
unsigned int)_tmp22D > 3?*((int*)_tmp22D)== 13: 0){_LL2FB: _tmp22E=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp22D)->f1;goto _LL2F8;}else{goto _LL2BD;}_LL2F8: _tmp22B=_tmp217.f2;if((
unsigned int)_tmp22B > 3?*((int*)_tmp22B)== 13: 0){_LL2F9: _tmp22C=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp22B)->f1;goto _LL2BC;}else{goto _LL2BD;}_LL2BD: _LL302: _tmp236=_tmp217.f1;if((
unsigned int)_tmp236 > 3?*((int*)_tmp236)== 2: 0){_LL303: _tmp237=((struct Cyc_Absyn_TunionType_struct*)
_tmp236)->f1;_LL306: _tmp23A=(void*)_tmp237.tunion_info;if(*((int*)_tmp23A)== 1){
_LL307: _tmp23B=((struct Cyc_Absyn_KnownTunion_struct*)_tmp23A)->f1;_tmp23C=*
_tmp23B;goto _LL305;}else{goto _LL2BF;}_LL305: _tmp239=_tmp237.targs;goto _LL304;
_LL304: _tmp238=(void*)_tmp237.rgn;goto _LL2FC;}else{goto _LL2BF;}_LL2FC: _tmp22F=
_tmp217.f2;if((unsigned int)_tmp22F > 3?*((int*)_tmp22F)== 2: 0){_LL2FD: _tmp230=((
struct Cyc_Absyn_TunionType_struct*)_tmp22F)->f1;_LL300: _tmp233=(void*)_tmp230.tunion_info;
if(*((int*)_tmp233)== 1){_LL301: _tmp234=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp233)->f1;_tmp235=*_tmp234;goto _LL2FF;}else{goto _LL2BF;}_LL2FF: _tmp232=
_tmp230.targs;goto _LL2FE;_LL2FE: _tmp231=(void*)_tmp230.rgn;goto _LL2BE;}else{goto
_LL2BF;}_LL2BF: _LL30E: _tmp243=_tmp217.f1;if((unsigned int)_tmp243 > 3?*((int*)
_tmp243)== 3: 0){_LL30F: _tmp244=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp243)->f1;
_LL311: _tmp246=(void*)_tmp244.field_info;if(*((int*)_tmp246)== 1){_LL313: _tmp248=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp246)->f1;goto _LL312;_LL312: _tmp247=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp246)->f2;goto _LL310;}else{goto
_LL2C1;}_LL310: _tmp245=_tmp244.targs;goto _LL308;}else{goto _LL2C1;}_LL308: _tmp23D=
_tmp217.f2;if((unsigned int)_tmp23D > 3?*((int*)_tmp23D)== 3: 0){_LL309: _tmp23E=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp23D)->f1;_LL30B: _tmp240=(void*)
_tmp23E.field_info;if(*((int*)_tmp240)== 1){_LL30D: _tmp242=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp240)->f1;goto _LL30C;_LL30C: _tmp241=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp240)->f2;goto _LL30A;}else{goto _LL2C1;}_LL30A: _tmp23F=_tmp23E.targs;goto
_LL2C0;}else{goto _LL2C1;}_LL2C1: _LL31B: _tmp250=_tmp217.f1;if((unsigned int)
_tmp250 > 3?*((int*)_tmp250)== 4: 0){_LL31C: _tmp251=((struct Cyc_Absyn_PointerType_struct*)
_tmp250)->f1;_LL321: _tmp256=(void*)_tmp251.elt_typ;goto _LL320;_LL320: _tmp255=(
void*)_tmp251.rgn_typ;goto _LL31F;_LL31F: _tmp254=_tmp251.nullable;goto _LL31E;
_LL31E: _tmp253=_tmp251.tq;goto _LL31D;_LL31D: _tmp252=_tmp251.bounds;goto _LL314;}
else{goto _LL2C3;}_LL314: _tmp249=_tmp217.f2;if((unsigned int)_tmp249 > 3?*((int*)
_tmp249)== 4: 0){_LL315: _tmp24A=((struct Cyc_Absyn_PointerType_struct*)_tmp249)->f1;
_LL31A: _tmp24F=(void*)_tmp24A.elt_typ;goto _LL319;_LL319: _tmp24E=(void*)_tmp24A.rgn_typ;
goto _LL318;_LL318: _tmp24D=_tmp24A.nullable;goto _LL317;_LL317: _tmp24C=_tmp24A.tq;
goto _LL316;_LL316: _tmp24B=_tmp24A.bounds;goto _LL2C2;}else{goto _LL2C3;}_LL2C3:
_LL325: _tmp25A=_tmp217.f1;if((unsigned int)_tmp25A > 3?*((int*)_tmp25A)== 5: 0){
_LL327: _tmp25C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp25A)->f1;goto _LL326;
_LL326: _tmp25B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp25A)->f2;goto _LL322;}
else{goto _LL2C5;}_LL322: _tmp257=_tmp217.f2;if((unsigned int)_tmp257 > 3?*((int*)
_tmp257)== 5: 0){_LL324: _tmp259=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp257)->f1;
goto _LL323;_LL323: _tmp258=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp257)->f2;
goto _LL2C4;}else{goto _LL2C5;}_LL2C5: _LL329: _tmp25E=_tmp217.f1;if((int)_tmp25E == 
1){goto _LL328;}else{goto _LL2C7;}_LL328: _tmp25D=_tmp217.f2;if((int)_tmp25D == 1){
goto _LL2C6;}else{goto _LL2C7;}_LL2C7: _LL32C: _tmp261=_tmp217.f1;if((unsigned int)
_tmp261 > 3?*((int*)_tmp261)== 6: 0){_LL32D: _tmp262=((struct Cyc_Absyn_DoubleType_struct*)
_tmp261)->f1;goto _LL32A;}else{goto _LL2C9;}_LL32A: _tmp25F=_tmp217.f2;if((
unsigned int)_tmp25F > 3?*((int*)_tmp25F)== 6: 0){_LL32B: _tmp260=((struct Cyc_Absyn_DoubleType_struct*)
_tmp25F)->f1;goto _LL2C8;}else{goto _LL2C9;}_LL2C9: _LL330: _tmp265=_tmp217.f1;if((
unsigned int)_tmp265 > 3?*((int*)_tmp265)== 14: 0){_LL331: _tmp266=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp265)->f1;goto _LL32E;}else{goto _LL2CB;}_LL32E:
_tmp263=_tmp217.f2;if((unsigned int)_tmp263 > 3?*((int*)_tmp263)== 14: 0){_LL32F:
_tmp264=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp263)->f1;goto _LL2CA;}
else{goto _LL2CB;}_LL2CB: _LL336: _tmp26B=_tmp217.f1;if((unsigned int)_tmp26B > 3?*((
int*)_tmp26B)== 7: 0){_LL339: _tmp26E=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp26B)->f1;goto _LL338;_LL338: _tmp26D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp26B)->f2;goto _LL337;_LL337: _tmp26C=((struct Cyc_Absyn_ArrayType_struct*)
_tmp26B)->f3;goto _LL332;}else{goto _LL2CD;}_LL332: _tmp267=_tmp217.f2;if((
unsigned int)_tmp267 > 3?*((int*)_tmp267)== 7: 0){_LL335: _tmp26A=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp267)->f1;goto _LL334;_LL334: _tmp269=((struct Cyc_Absyn_ArrayType_struct*)
_tmp267)->f2;goto _LL333;_LL333: _tmp268=((struct Cyc_Absyn_ArrayType_struct*)
_tmp267)->f3;goto _LL2CC;}else{goto _LL2CD;}_LL2CD: _LL344: _tmp279=_tmp217.f1;if((
unsigned int)_tmp279 > 3?*((int*)_tmp279)== 8: 0){_LL345: _tmp27A=((struct Cyc_Absyn_FnType_struct*)
_tmp279)->f1;_LL34D: _tmp282=_tmp27A.tvars;goto _LL34C;_LL34C: _tmp281=_tmp27A.effect;
goto _LL34B;_LL34B: _tmp280=(void*)_tmp27A.ret_typ;goto _LL34A;_LL34A: _tmp27F=
_tmp27A.args;goto _LL349;_LL349: _tmp27E=_tmp27A.c_varargs;goto _LL348;_LL348:
_tmp27D=_tmp27A.cyc_varargs;goto _LL347;_LL347: _tmp27C=_tmp27A.rgn_po;goto _LL346;
_LL346: _tmp27B=_tmp27A.attributes;goto _LL33A;}else{goto _LL2CF;}_LL33A: _tmp26F=
_tmp217.f2;if((unsigned int)_tmp26F > 3?*((int*)_tmp26F)== 8: 0){_LL33B: _tmp270=((
struct Cyc_Absyn_FnType_struct*)_tmp26F)->f1;_LL343: _tmp278=_tmp270.tvars;goto
_LL342;_LL342: _tmp277=_tmp270.effect;goto _LL341;_LL341: _tmp276=(void*)_tmp270.ret_typ;
goto _LL340;_LL340: _tmp275=_tmp270.args;goto _LL33F;_LL33F: _tmp274=_tmp270.c_varargs;
goto _LL33E;_LL33E: _tmp273=_tmp270.cyc_varargs;goto _LL33D;_LL33D: _tmp272=_tmp270.rgn_po;
goto _LL33C;_LL33C: _tmp271=_tmp270.attributes;goto _LL2CE;}else{goto _LL2CF;}_LL2CF:
_LL350: _tmp285=_tmp217.f1;if((unsigned int)_tmp285 > 3?*((int*)_tmp285)== 9: 0){
_LL351: _tmp286=((struct Cyc_Absyn_TupleType_struct*)_tmp285)->f1;goto _LL34E;}
else{goto _LL2D1;}_LL34E: _tmp283=_tmp217.f2;if((unsigned int)_tmp283 > 3?*((int*)
_tmp283)== 9: 0){_LL34F: _tmp284=((struct Cyc_Absyn_TupleType_struct*)_tmp283)->f1;
goto _LL2D0;}else{goto _LL2D1;}_LL2D1: _LL355: _tmp28A=_tmp217.f1;if((unsigned int)
_tmp28A > 3?*((int*)_tmp28A)== 11: 0){_LL357: _tmp28C=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp28A)->f1;goto _LL356;_LL356: _tmp28B=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp28A)->f2;goto _LL352;}else{goto _LL2D3;}_LL352: _tmp287=_tmp217.f2;if((
unsigned int)_tmp287 > 3?*((int*)_tmp287)== 11: 0){_LL354: _tmp289=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp287)->f1;goto _LL353;_LL353: _tmp288=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp287)->f2;goto _LL2D2;}else{goto _LL2D3;}_LL2D3:
_LL359: _tmp28E=_tmp217.f1;if((int)_tmp28E == 2){goto _LL358;}else{goto _LL2D5;}
_LL358: _tmp28D=_tmp217.f2;if((int)_tmp28D == 2){goto _LL2D4;}else{goto _LL2D5;}
_LL2D5: _LL35C: _tmp291=_tmp217.f1;if((unsigned int)_tmp291 > 3?*((int*)_tmp291)== 
15: 0){_LL35D: _tmp292=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp291)->f1;
goto _LL35A;}else{goto _LL2D7;}_LL35A: _tmp28F=_tmp217.f2;if((unsigned int)_tmp28F > 
3?*((int*)_tmp28F)== 15: 0){_LL35B: _tmp290=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp28F)->f1;goto _LL2D6;}else{goto _LL2D7;}_LL2D7: _LL35E: _tmp293=_tmp217.f1;if((
unsigned int)_tmp293 > 3?*((int*)_tmp293)== 18: 0){goto _LL2D8;}else{goto _LL2D9;}
_LL2D9: _LL35F: _tmp294=_tmp217.f2;if((unsigned int)_tmp294 > 3?*((int*)_tmp294)== 
18: 0){goto _LL2DA;}else{goto _LL2DB;}_LL2DB: _LL360: _tmp295=_tmp217.f1;if((
unsigned int)_tmp295 > 3?*((int*)_tmp295)== 17: 0){goto _LL2DC;}else{goto _LL2DD;}
_LL2DD: _LL361: _tmp296=_tmp217.f1;if((unsigned int)_tmp296 > 3?*((int*)_tmp296)== 
19: 0){goto _LL2DE;}else{goto _LL2DF;}_LL2DF: _LL362: _tmp297=_tmp217.f2;if((
unsigned int)_tmp297 > 3?*((int*)_tmp297)== 19: 0){goto _LL2E0;}else{goto _LL2E1;}
_LL2E1: _LL363: _tmp298=_tmp217.f2;if((unsigned int)_tmp298 > 3?*((int*)_tmp298)== 
17: 0){goto _LL2E2;}else{goto _LL2E3;}_LL2E3: goto _LL2E4;_LL2B2: Cyc_Tcutil_unify_it(
t2,t1);return;_LL2B4: return;_LL2B6: {struct _tagged_arr*_tmp299=_tmp21E->name;
struct _tagged_arr*_tmp29A=_tmp21C->name;int _tmp29B=*((int*)_check_null(_tmp21E->identity));
int _tmp29C=*((int*)_check_null(_tmp21C->identity));void*_tmp29D=Cyc_Tcutil_tvar_kind(
_tmp21E);void*_tmp29E=Cyc_Tcutil_tvar_kind(_tmp21C);if(_tmp29C == _tmp29B? Cyc_Std_zstrptrcmp(
_tmp299,_tmp29A)== 0: 0){if(_tmp29D != _tmp29E){({struct Cyc_Std_String_pa_struct
_tmp2A2;_tmp2A2.tag=0;_tmp2A2.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp29E);{struct Cyc_Std_String_pa_struct _tmp2A1;_tmp2A1.tag=0;_tmp2A1.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp29D);{struct Cyc_Std_String_pa_struct
_tmp2A0;_tmp2A0.tag=0;_tmp2A0.f1=(struct _tagged_arr)*_tmp299;{void*_tmp29F[3]={&
_tmp2A0,& _tmp2A1,& _tmp2A2};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("same type variable %s has kinds %s and %s",sizeof(
unsigned char),42),_tag_arr(_tmp29F,sizeof(void*),3));}}}});}return;}Cyc_Tcutil_failure_reason=
_tag_arr("(variable types are not the same)",sizeof(unsigned char),34);goto
_LL2B0;}_LL2B8: {struct _tuple1*_tmp2A4;void*_tmp2A5;struct _tuple5 _tmp2A3=Cyc_Absyn_aggr_kinded_name(
_tmp222);_LL366: _tmp2A5=_tmp2A3.f1;goto _LL365;_LL365: _tmp2A4=_tmp2A3.f2;goto
_LL364;_LL364: {struct _tuple1*_tmp2A7;void*_tmp2A8;struct _tuple5 _tmp2A6=Cyc_Absyn_aggr_kinded_name(
_tmp226);_LL369: _tmp2A8=_tmp2A6.f1;goto _LL368;_LL368: _tmp2A7=_tmp2A6.f2;goto
_LL367;_LL367: if(_tmp2A5 != _tmp2A8){Cyc_Tcutil_failure_reason=_tag_arr("(struct and union type)",
sizeof(unsigned char),24);goto _LL2B0;}if(Cyc_Absyn_qvar_cmp(_tmp2A4,_tmp2A7)!= 0){
Cyc_Tcutil_failure_reason=_tag_arr("(different type name)",sizeof(unsigned char),
22);goto _LL2B0;}Cyc_Tcutil_unify_list(_tmp221,_tmp225);return;}}_LL2BA: if(Cyc_Absyn_qvar_cmp(
_tmp22A,_tmp228)== 0){return;}Cyc_Tcutil_failure_reason=_tag_arr("(different enum types)",
sizeof(unsigned char),23);goto _LL2B0;_LL2BC: {int bad=0;for(0;_tmp22E != 0? _tmp22C
!= 0: 0;(_tmp22E=_tmp22E->tl,_tmp22C=_tmp22C->tl)){struct Cyc_Absyn_Enumfield*
_tmp2A9=(struct Cyc_Absyn_Enumfield*)_tmp22E->hd;struct Cyc_Absyn_Enumfield*
_tmp2AA=(struct Cyc_Absyn_Enumfield*)_tmp22C->hd;if(Cyc_Absyn_qvar_cmp(_tmp2A9->name,
_tmp2AA->name)!= 0){Cyc_Tcutil_failure_reason=_tag_arr("(different names for enum fields)",
sizeof(unsigned char),34);bad=1;break;}if(_tmp2A9->tag == _tmp2AA->tag){continue;}
if(_tmp2A9->tag == 0? 1: _tmp2AA->tag == 0){Cyc_Tcutil_failure_reason=_tag_arr("(different tag values for enum fields)",
sizeof(unsigned char),39);bad=1;break;}if(Cyc_Evexp_eval_const_uint_exp((struct
Cyc_Absyn_Exp*)_check_null(_tmp2A9->tag))!= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2AA->tag))){Cyc_Tcutil_failure_reason=
_tag_arr("(different tag values for enum fields)",sizeof(unsigned char),39);bad=
1;break;}}if(bad){goto _LL2B0;}if(_tmp22E == 0? _tmp22C == 0: 0){return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different number of fields for enums)",sizeof(unsigned char),39);goto
_LL2B0;}_LL2BE: if(_tmp23C == _tmp235? 1: Cyc_Absyn_qvar_cmp(_tmp23C->name,_tmp235->name)
== 0){Cyc_Tcutil_unify_it(_tmp231,_tmp238);Cyc_Tcutil_unify_list(_tmp232,_tmp239);
return;}Cyc_Tcutil_failure_reason=_tag_arr("(different tunion types)",sizeof(
unsigned char),25);goto _LL2B0;_LL2C0: if((_tmp248 == _tmp242? 1: Cyc_Absyn_qvar_cmp(
_tmp248->name,_tmp242->name)== 0)? _tmp247 == _tmp241? 1: Cyc_Absyn_qvar_cmp(_tmp247->name,
_tmp241->name)== 0: 0){Cyc_Tcutil_unify_list(_tmp23F,_tmp245);return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different tunion field types)",sizeof(unsigned char),31);goto _LL2B0;
_LL2C2: Cyc_Tcutil_unify_it(_tmp24F,_tmp256);Cyc_Tcutil_unify_it(_tmp255,_tmp24E);
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_unify_tqual(_tmp24C,
_tmp253);Cyc_Tcutil_unify_it_conrefs(Cyc_Tcutil_boundscmp,_tmp24B,_tmp252,
_tag_arr("(different pointer bounds)",sizeof(unsigned char),27));{void*_tmp2AB=(
void*)(Cyc_Absyn_compress_conref(_tmp24B))->v;void*_tmp2AC;_LL36B: if((
unsigned int)_tmp2AB > 1?*((int*)_tmp2AB)== 0: 0){_LL36F: _tmp2AC=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp2AB)->f1;if((int)_tmp2AC == 0){goto _LL36C;}else{
goto _LL36D;}}else{goto _LL36D;}_LL36D: goto _LL36E;_LL36C: return;_LL36E: goto _LL36A;
_LL36A:;}((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y,struct _tagged_arr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp24D,
_tmp254,_tag_arr("(different pointer types)",sizeof(unsigned char),26));return;
_LL2C4: if(_tmp259 == _tmp25C? _tmp258 == _tmp25B: 0){return;}Cyc_Tcutil_failure_reason=
_tag_arr("(different integral types)",sizeof(unsigned char),27);goto _LL2B0;
_LL2C6: return;_LL2C8: if(_tmp262 == _tmp260){return;}goto _LL2B0;_LL2CA: Cyc_Tcutil_unify_it(
_tmp266,_tmp264);return;_LL2CC: Cyc_Tcutil_unify_tqual(_tmp269,_tmp26D);Cyc_Tcutil_unify_it(
_tmp26A,_tmp26E);if(_tmp26C == _tmp268){return;}if(_tmp26C == 0? 1: _tmp268 == 0){
goto _LL2B0;}if(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp26C))== Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(
_tmp268))){return;}Cyc_Tcutil_failure_reason=_tag_arr("(different array sizes)",
sizeof(unsigned char),24);goto _LL2B0;_LL2CE: {int done=0;{struct _RegionHandle
_tmp2AD=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2AD;_push_region(rgn);{
struct Cyc_List_List*inst=0;while(_tmp278 != 0){if(_tmp282 == 0){Cyc_Tcutil_failure_reason=
_tag_arr("(second function type has too few type variables)",sizeof(
unsigned char),50);(int)_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp2AE=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)_tmp278->hd);void*_tmp2AF=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp282->hd);if(_tmp2AE != _tmp2AF){Cyc_Tcutil_failure_reason=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp2B3;_tmp2B3.tag=0;_tmp2B3.f1=(struct
_tagged_arr)Cyc_Absynpp_kind2string(_tmp2AF);{struct Cyc_Std_String_pa_struct
_tmp2B2;_tmp2B2.tag=0;_tmp2B2.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(
_tmp2AE);{struct Cyc_Std_String_pa_struct _tmp2B1;_tmp2B1.tag=0;_tmp2B1.f1=(struct
_tagged_arr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp278->hd);{void*
_tmp2B0[3]={& _tmp2B1,& _tmp2B2,& _tmp2B3};Cyc_Std_aprintf(_tag_arr("(type var %s has different kinds %s and %s)",
sizeof(unsigned char),44),_tag_arr(_tmp2B0,sizeof(void*),3));}}}});(int)_throw((
void*)Cyc_Tcutil_Unify);}inst=({struct Cyc_List_List*_tmp2B4=_region_malloc(rgn,
sizeof(*_tmp2B4));_tmp2B4->hd=({struct _tuple6*_tmp2B5=_region_malloc(rgn,sizeof(*
_tmp2B5));_tmp2B5->f1=(struct Cyc_Absyn_Tvar*)_tmp282->hd;_tmp2B5->f2=(void*)({
struct Cyc_Absyn_VarType_struct*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6[0]=({
struct Cyc_Absyn_VarType_struct _tmp2B7;_tmp2B7.tag=1;_tmp2B7.f1=(struct Cyc_Absyn_Tvar*)
_tmp278->hd;_tmp2B7;});_tmp2B6;});_tmp2B5;});_tmp2B4->tl=inst;_tmp2B4;});_tmp278=
_tmp278->tl;_tmp282=_tmp282->tl;}}if(_tmp282 != 0){Cyc_Tcutil_failure_reason=
_tag_arr("(second function type has too many type variables)",sizeof(
unsigned char),51);_npop_handler(0);goto _LL2B0;}if(inst != 0){Cyc_Tcutil_unify_it((
void*)({struct Cyc_Absyn_FnType_struct*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));
_tmp2B8[0]=({struct Cyc_Absyn_FnType_struct _tmp2B9;_tmp2B9.tag=8;_tmp2B9.f1=({
struct Cyc_Absyn_FnInfo _tmp2BA;_tmp2BA.tvars=0;_tmp2BA.effect=_tmp277;_tmp2BA.ret_typ=(
void*)_tmp276;_tmp2BA.args=_tmp275;_tmp2BA.c_varargs=_tmp274;_tmp2BA.cyc_varargs=
_tmp273;_tmp2BA.rgn_po=_tmp272;_tmp2BA.attributes=_tmp271;_tmp2BA;});_tmp2B9;});
_tmp2B8;}),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_struct*
_tmp2BB=_cycalloc(sizeof(*_tmp2BB));_tmp2BB[0]=({struct Cyc_Absyn_FnType_struct
_tmp2BC;_tmp2BC.tag=8;_tmp2BC.f1=({struct Cyc_Absyn_FnInfo _tmp2BD;_tmp2BD.tvars=0;
_tmp2BD.effect=_tmp281;_tmp2BD.ret_typ=(void*)_tmp280;_tmp2BD.args=_tmp27F;
_tmp2BD.c_varargs=_tmp27E;_tmp2BD.cyc_varargs=_tmp27D;_tmp2BD.rgn_po=_tmp27C;
_tmp2BD.attributes=_tmp27B;_tmp2BD;});_tmp2BC;});_tmp2BB;})));done=1;}};
_pop_region(rgn);}if(done){return;}Cyc_Tcutil_unify_it(_tmp276,_tmp280);for(0;
_tmp275 != 0? _tmp27F != 0: 0;(_tmp275=_tmp275->tl,_tmp27F=_tmp27F->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple2*)_tmp275->hd)).f2,(*((struct _tuple2*)_tmp27F->hd)).f2);Cyc_Tcutil_unify_it((*((
struct _tuple2*)_tmp275->hd)).f3,(*((struct _tuple2*)_tmp27F->hd)).f3);}Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;if(_tmp275 != 0? 1: _tmp27F != 0){Cyc_Tcutil_failure_reason=
_tag_arr("(function types have different number of arguments)",sizeof(
unsigned char),52);goto _LL2B0;}if(_tmp274 != _tmp27E){Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type takes C varargs)",sizeof(unsigned char),41);
goto _LL2B0;}{int bad_cyc_vararg=0;{struct _tuple12 _tmp2BF=({struct _tuple12 _tmp2BE;
_tmp2BE.f1=_tmp273;_tmp2BE.f2=_tmp27D;_tmp2BE;});struct Cyc_Absyn_VarargInfo*
_tmp2C0;struct Cyc_Absyn_VarargInfo*_tmp2C1;struct Cyc_Absyn_VarargInfo*_tmp2C2;
struct Cyc_Absyn_VarargInfo*_tmp2C3;struct Cyc_Absyn_VarargInfo*_tmp2C4;struct Cyc_Absyn_VarargInfo
_tmp2C5;int _tmp2C6;void*_tmp2C7;struct Cyc_Absyn_Tqual _tmp2C8;struct Cyc_Core_Opt*
_tmp2C9;struct Cyc_Absyn_VarargInfo*_tmp2CA;struct Cyc_Absyn_VarargInfo _tmp2CB;int
_tmp2CC;void*_tmp2CD;struct Cyc_Absyn_Tqual _tmp2CE;struct Cyc_Core_Opt*_tmp2CF;
_LL371: _LL37A: _tmp2C1=_tmp2BF.f1;if(_tmp2C1 == 0){goto _LL379;}else{goto _LL373;}
_LL379: _tmp2C0=_tmp2BF.f2;if(_tmp2C0 == 0){goto _LL372;}else{goto _LL373;}_LL373:
_LL37B: _tmp2C2=_tmp2BF.f1;if(_tmp2C2 == 0){goto _LL374;}else{goto _LL375;}_LL375:
_LL37C: _tmp2C3=_tmp2BF.f2;if(_tmp2C3 == 0){goto _LL376;}else{goto _LL377;}_LL377:
_LL382: _tmp2CA=_tmp2BF.f1;if(_tmp2CA == 0){goto _LL370;}else{_tmp2CB=*_tmp2CA;
_LL386: _tmp2CF=_tmp2CB.name;goto _LL385;_LL385: _tmp2CE=_tmp2CB.tq;goto _LL384;
_LL384: _tmp2CD=(void*)_tmp2CB.type;goto _LL383;_LL383: _tmp2CC=_tmp2CB.inject;goto
_LL37D;}_LL37D: _tmp2C4=_tmp2BF.f2;if(_tmp2C4 == 0){goto _LL370;}else{_tmp2C5=*
_tmp2C4;_LL381: _tmp2C9=_tmp2C5.name;goto _LL380;_LL380: _tmp2C8=_tmp2C5.tq;goto
_LL37F;_LL37F: _tmp2C7=(void*)_tmp2C5.type;goto _LL37E;_LL37E: _tmp2C6=_tmp2C5.inject;
goto _LL378;}_LL372: goto _LL370;_LL374: goto _LL376;_LL376: bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type takes varargs)",sizeof(unsigned char),39);goto
_LL370;_LL378: Cyc_Tcutil_unify_tqual(_tmp2CE,_tmp2C8);Cyc_Tcutil_unify_it(
_tmp2CD,_tmp2C7);if(_tmp2CC != _tmp2C6){bad_cyc_vararg=1;Cyc_Tcutil_failure_reason=
_tag_arr("(only one function type injects varargs)",sizeof(unsigned char),41);}
goto _LL370;_LL370:;}if(bad_cyc_vararg){goto _LL2B0;}{int bad_effect=0;{struct
_tuple13 _tmp2D1=({struct _tuple13 _tmp2D0;_tmp2D0.f1=_tmp277;_tmp2D0.f2=_tmp281;
_tmp2D0;});struct Cyc_Core_Opt*_tmp2D2;struct Cyc_Core_Opt*_tmp2D3;struct Cyc_Core_Opt*
_tmp2D4;struct Cyc_Core_Opt*_tmp2D5;_LL388: _LL391: _tmp2D3=_tmp2D1.f1;if(_tmp2D3 == 
0){goto _LL390;}else{goto _LL38A;}_LL390: _tmp2D2=_tmp2D1.f2;if(_tmp2D2 == 0){goto
_LL389;}else{goto _LL38A;}_LL38A: _LL392: _tmp2D4=_tmp2D1.f1;if(_tmp2D4 == 0){goto
_LL38B;}else{goto _LL38C;}_LL38C: _LL393: _tmp2D5=_tmp2D1.f2;if(_tmp2D5 == 0){goto
_LL38D;}else{goto _LL38E;}_LL38E: goto _LL38F;_LL389: goto _LL387;_LL38B: goto _LL38D;
_LL38D: bad_effect=1;goto _LL387;_LL38F: bad_effect=! Cyc_Tcutil_unify_effect((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp277))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp281))->v);goto _LL387;_LL387:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(bad_effect){Cyc_Tcutil_failure_reason=_tag_arr("(function type effects do not unify)",
sizeof(unsigned char),37);goto _LL2B0;}if(! Cyc_Tcutil_same_atts(_tmp27B,_tmp271)){
Cyc_Tcutil_failure_reason=_tag_arr("(function types have different attributes)",
sizeof(unsigned char),43);goto _LL2B0;}if(! Cyc_Tcutil_same_rgn_po(_tmp27C,_tmp272)){
Cyc_Tcutil_failure_reason=_tag_arr("(function types have different region lifetime orderings)",
sizeof(unsigned char),58);goto _LL2B0;}return;}}}_LL2D0: for(0;_tmp284 != 0? _tmp286
!= 0: 0;(_tmp284=_tmp284->tl,_tmp286=_tmp286->tl)){Cyc_Tcutil_unify_tqual((*((
struct _tuple4*)_tmp284->hd)).f1,(*((struct _tuple4*)_tmp286->hd)).f1);Cyc_Tcutil_unify_it((*((
struct _tuple4*)_tmp284->hd)).f2,(*((struct _tuple4*)_tmp286->hd)).f2);}if(_tmp284
== 0? _tmp286 == 0: 0){return;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_arr("(tuple types have different numbers of components)",
sizeof(unsigned char),51);goto _LL2B0;_LL2D2: if(_tmp289 != _tmp28C){Cyc_Tcutil_failure_reason=
_tag_arr("(struct and union type)",sizeof(unsigned char),24);goto _LL2B0;}for(0;
_tmp288 != 0? _tmp28B != 0: 0;(_tmp288=_tmp288->tl,_tmp28B=_tmp28B->tl)){struct Cyc_Absyn_Aggrfield*
_tmp2D6=(struct Cyc_Absyn_Aggrfield*)_tmp288->hd;struct Cyc_Absyn_Aggrfield*
_tmp2D7=(struct Cyc_Absyn_Aggrfield*)_tmp28B->hd;if(Cyc_Std_zstrptrcmp(_tmp2D6->name,
_tmp2D7->name)!= 0){Cyc_Tcutil_failure_reason=_tag_arr("(different member names)",
sizeof(unsigned char),25);(int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_tqual(
_tmp2D6->tq,_tmp2D7->tq);Cyc_Tcutil_unify_it((void*)_tmp2D6->type,(void*)_tmp2D7->type);
if(! Cyc_Tcutil_same_atts(_tmp2D6->attributes,_tmp2D7->attributes)){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=_tag_arr("(different attributes on member)",
sizeof(unsigned char),33);(int)_throw((void*)Cyc_Tcutil_Unify);}if(((_tmp2D6->width
!= 0? _tmp2D7->width == 0: 0)? 1:(_tmp2D7->width != 0? _tmp2D6->width == 0: 0))? 1:((
_tmp2D6->width != 0? _tmp2D7->width != 0: 0)? Cyc_Evexp_eval_const_uint_exp((struct
Cyc_Absyn_Exp*)_check_null(_tmp2D6->width))!= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp2D7->width)): 0)){Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=_tag_arr("(different bitfield widths on member)",
sizeof(unsigned char),38);(int)_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp288 == 0?
_tmp28B == 0: 0){return;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=
_tag_arr("(different number of members)",sizeof(unsigned char),30);goto _LL2B0;
_LL2D4: return;_LL2D6: Cyc_Tcutil_unify_it(_tmp292,_tmp290);return;_LL2D8: goto
_LL2DA;_LL2DA: goto _LL2DC;_LL2DC: goto _LL2DE;_LL2DE: goto _LL2E0;_LL2E0: goto _LL2E2;
_LL2E2: if(Cyc_Tcutil_unify_effect(t1,t2)){return;}Cyc_Tcutil_failure_reason=
_tag_arr("(effects don't unify)",sizeof(unsigned char),22);goto _LL2B0;_LL2E4:
goto _LL2B0;_LL2B0:;}(int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_opt_cmp(
int(*cmp)(void*,void*),struct Cyc_Core_Opt*a1,struct Cyc_Core_Opt*a2){if(a1 == a2){
return 0;}if(a1 == 0? a2 != 0: 0){return - 1;}if(a1 != 0? a2 == 0: 0){return 1;}return cmp((
void*)((struct Cyc_Core_Opt*)_check_null(a1))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(a2))->v);}int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*
a2){if(a1 == a2){return 0;}if(a1 == 0? a2 != 0: 0){return - 1;}if(a1 != 0? a2 == 0: 0){
return 1;}return cmp((void*)_check_null(a1),(void*)_check_null(a2));}int Cyc_Tcutil_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){if(l1 == l2){
return 0;}for(0;l1 != 0? l2 != 0: 0;(l1=l1->tl,l2=l2->tl)){int _tmp2D8=cmp((void*)l1->hd,(
void*)l2->hd);if(_tmp2D8 != 0){return _tmp2D8;}}if(l2 != 0){return - 1;}if(l1 != 0){
return 1;}return 0;}static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct
Cyc_Absyn_Tqual tq2){int _tmp2D9=(tq1.q_const + (tq1.q_volatile << 1))+ (tq1.q_restrict
<< 2);int _tmp2DA=(tq2.q_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp2D9,_tmp2DA);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(
void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y){return 0;}{void*_tmp2DB=(void*)x->v;
void*_tmp2DC;_LL395: if((int)_tmp2DB == 0){goto _LL396;}else{goto _LL397;}_LL397: if((
unsigned int)_tmp2DB > 1?*((int*)_tmp2DB)== 0: 0){_LL39B: _tmp2DC=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp2DB)->f1;goto _LL398;}else{goto _LL399;}_LL399: if((
unsigned int)_tmp2DB > 1?*((int*)_tmp2DB)== 1: 0){goto _LL39A;}else{goto _LL394;}
_LL396: return - 1;_LL398: {void*_tmp2DD=(void*)y->v;void*_tmp2DE;_LL39D: if((int)
_tmp2DD == 0){goto _LL39E;}else{goto _LL39F;}_LL39F: if((unsigned int)_tmp2DD > 1?*((
int*)_tmp2DD)== 0: 0){_LL3A3: _tmp2DE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp2DD)->f1;goto _LL3A0;}else{goto _LL3A1;}_LL3A1: if((unsigned int)_tmp2DD > 1?*((
int*)_tmp2DD)== 1: 0){goto _LL3A2;}else{goto _LL39C;}_LL39E: return 1;_LL3A0: return
cmp(_tmp2DC,_tmp2DE);_LL3A2:({void*_tmp2DF[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("unify_conref: forward after compress(2)",
sizeof(unsigned char),40),_tag_arr(_tmp2DF,sizeof(void*),0));});_LL39C:;}_LL39A:({
void*_tmp2E0[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("unify_conref: forward after compress",sizeof(unsigned char),37),
_tag_arr(_tmp2E0,sizeof(void*),0));});_LL394:;}}static int Cyc_Tcutil_tqual_type_cmp(
struct _tuple4*tqt1,struct _tuple4*tqt2){struct _tuple4 _tmp2E2;void*_tmp2E3;struct
Cyc_Absyn_Tqual _tmp2E4;struct _tuple4*_tmp2E1=tqt1;_tmp2E2=*_tmp2E1;_LL3A6:
_tmp2E4=_tmp2E2.f1;goto _LL3A5;_LL3A5: _tmp2E3=_tmp2E2.f2;goto _LL3A4;_LL3A4: {
struct _tuple4 _tmp2E6;void*_tmp2E7;struct Cyc_Absyn_Tqual _tmp2E8;struct _tuple4*
_tmp2E5=tqt2;_tmp2E6=*_tmp2E5;_LL3A9: _tmp2E8=_tmp2E6.f1;goto _LL3A8;_LL3A8:
_tmp2E7=_tmp2E6.f2;goto _LL3A7;_LL3A7: {int _tmp2E9=Cyc_Tcutil_tqual_cmp(_tmp2E4,
_tmp2E8);if(_tmp2E9 != 0){return _tmp2E9;}return Cyc_Tcutil_typecmp(_tmp2E3,_tmp2E7);}}}
static int Cyc_Tcutil_const_uint_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){return Cyc_Core_intcmp((int)Cyc_Evexp_eval_const_uint_exp(e1),(int)Cyc_Evexp_eval_const_uint_exp(
e2));}static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp2EA=att;
_LL3AB: if((unsigned int)_tmp2EA > 16?*((int*)_tmp2EA)== 0: 0){goto _LL3AC;}else{
goto _LL3AD;}_LL3AD: if((int)_tmp2EA == 0){goto _LL3AE;}else{goto _LL3AF;}_LL3AF: if((
int)_tmp2EA == 1){goto _LL3B0;}else{goto _LL3B1;}_LL3B1: if((int)_tmp2EA == 2){goto
_LL3B2;}else{goto _LL3B3;}_LL3B3: if((int)_tmp2EA == 3){goto _LL3B4;}else{goto _LL3B5;}
_LL3B5: if((int)_tmp2EA == 4){goto _LL3B6;}else{goto _LL3B7;}_LL3B7: if((unsigned int)
_tmp2EA > 16?*((int*)_tmp2EA)== 1: 0){goto _LL3B8;}else{goto _LL3B9;}_LL3B9: if((int)
_tmp2EA == 5){goto _LL3BA;}else{goto _LL3BB;}_LL3BB: if((unsigned int)_tmp2EA > 16?*((
int*)_tmp2EA)== 2: 0){goto _LL3BC;}else{goto _LL3BD;}_LL3BD: if((int)_tmp2EA == 6){
goto _LL3BE;}else{goto _LL3BF;}_LL3BF: if((int)_tmp2EA == 7){goto _LL3C0;}else{goto
_LL3C1;}_LL3C1: if((int)_tmp2EA == 8){goto _LL3C2;}else{goto _LL3C3;}_LL3C3: if((int)
_tmp2EA == 9){goto _LL3C4;}else{goto _LL3C5;}_LL3C5: if((int)_tmp2EA == 10){goto
_LL3C6;}else{goto _LL3C7;}_LL3C7: if((int)_tmp2EA == 11){goto _LL3C8;}else{goto
_LL3C9;}_LL3C9: if((int)_tmp2EA == 12){goto _LL3CA;}else{goto _LL3CB;}_LL3CB: if((int)
_tmp2EA == 13){goto _LL3CC;}else{goto _LL3CD;}_LL3CD: if((int)_tmp2EA == 14){goto
_LL3CE;}else{goto _LL3CF;}_LL3CF: if((int)_tmp2EA == 15){goto _LL3D0;}else{goto
_LL3D1;}_LL3D1: if((unsigned int)_tmp2EA > 16?*((int*)_tmp2EA)== 3: 0){goto _LL3D2;}
else{goto _LL3AA;}_LL3AC: return 0;_LL3AE: return 1;_LL3B0: return 2;_LL3B2: return 3;
_LL3B4: return 4;_LL3B6: return 5;_LL3B8: return 6;_LL3BA: return 7;_LL3BC: return 8;
_LL3BE: return 9;_LL3C0: return 10;_LL3C2: return 11;_LL3C4: return 12;_LL3C6: return 13;
_LL3C8: return 14;_LL3CA: return 15;_LL3CC: return 16;_LL3CE: return 17;_LL3D0: return 18;
_LL3D2: return 19;_LL3AA:;}static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple9 _tmp2EC=({struct _tuple9 _tmp2EB;_tmp2EB.f1=att1;_tmp2EB.f2=att2;
_tmp2EB;});void*_tmp2ED;int _tmp2EE;void*_tmp2EF;int _tmp2F0;void*_tmp2F1;int
_tmp2F2;void*_tmp2F3;int _tmp2F4;void*_tmp2F5;struct _tagged_arr _tmp2F6;void*
_tmp2F7;struct _tagged_arr _tmp2F8;void*_tmp2F9;int _tmp2FA;int _tmp2FB;void*_tmp2FC;
void*_tmp2FD;int _tmp2FE;int _tmp2FF;void*_tmp300;_LL3D4: _LL3E0: _tmp2EF=_tmp2EC.f1;
if((unsigned int)_tmp2EF > 16?*((int*)_tmp2EF)== 0: 0){_LL3E1: _tmp2F0=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp2EF)->f1;goto _LL3DE;}else{goto _LL3D6;}_LL3DE: _tmp2ED=_tmp2EC.f2;if((
unsigned int)_tmp2ED > 16?*((int*)_tmp2ED)== 0: 0){_LL3DF: _tmp2EE=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp2ED)->f1;goto _LL3D5;}else{goto _LL3D6;}_LL3D6: _LL3E4: _tmp2F3=_tmp2EC.f1;if((
unsigned int)_tmp2F3 > 16?*((int*)_tmp2F3)== 1: 0){_LL3E5: _tmp2F4=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp2F3)->f1;goto _LL3E2;}else{goto _LL3D8;}_LL3E2: _tmp2F1=_tmp2EC.f2;if((
unsigned int)_tmp2F1 > 16?*((int*)_tmp2F1)== 1: 0){_LL3E3: _tmp2F2=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp2F1)->f1;goto _LL3D7;}else{goto _LL3D8;}_LL3D8: _LL3E8: _tmp2F7=_tmp2EC.f1;if((
unsigned int)_tmp2F7 > 16?*((int*)_tmp2F7)== 2: 0){_LL3E9: _tmp2F8=((struct Cyc_Absyn_Section_att_struct*)
_tmp2F7)->f1;goto _LL3E6;}else{goto _LL3DA;}_LL3E6: _tmp2F5=_tmp2EC.f2;if((
unsigned int)_tmp2F5 > 16?*((int*)_tmp2F5)== 2: 0){_LL3E7: _tmp2F6=((struct Cyc_Absyn_Section_att_struct*)
_tmp2F5)->f1;goto _LL3D9;}else{goto _LL3DA;}_LL3DA: _LL3EE: _tmp2FD=_tmp2EC.f1;if((
unsigned int)_tmp2FD > 16?*((int*)_tmp2FD)== 3: 0){_LL3F1: _tmp300=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp2FD)->f1;goto _LL3F0;_LL3F0: _tmp2FF=((struct Cyc_Absyn_Format_att_struct*)
_tmp2FD)->f2;goto _LL3EF;_LL3EF: _tmp2FE=((struct Cyc_Absyn_Format_att_struct*)
_tmp2FD)->f3;goto _LL3EA;}else{goto _LL3DC;}_LL3EA: _tmp2F9=_tmp2EC.f2;if((
unsigned int)_tmp2F9 > 16?*((int*)_tmp2F9)== 3: 0){_LL3ED: _tmp2FC=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp2F9)->f1;goto _LL3EC;_LL3EC: _tmp2FB=((struct Cyc_Absyn_Format_att_struct*)
_tmp2F9)->f2;goto _LL3EB;_LL3EB: _tmp2FA=((struct Cyc_Absyn_Format_att_struct*)
_tmp2F9)->f3;goto _LL3DB;}else{goto _LL3DC;}_LL3DC: goto _LL3DD;_LL3D5: _tmp2F4=
_tmp2F0;_tmp2F2=_tmp2EE;goto _LL3D7;_LL3D7: return Cyc_Core_intcmp(_tmp2F4,_tmp2F2);
_LL3D9: return Cyc_Std_strcmp(_tmp2F8,_tmp2F6);_LL3DB: {int _tmp301=Cyc_Core_intcmp((
int)((unsigned int)_tmp300),(int)((unsigned int)_tmp2FC));if(_tmp301 != 0){return
_tmp301;}{int _tmp302=Cyc_Core_intcmp(_tmp2FF,_tmp2FB);if(_tmp302 != 0){return
_tmp302;}return Cyc_Core_intcmp(_tmp2FE,_tmp2FA);}}_LL3DD: return Cyc_Core_intcmp(
Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));
_LL3D3:;}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct
Cyc_Absyn_Aggrfield*f2){int _tmp303=Cyc_Std_zstrptrcmp(f1->name,f2->name);if(
_tmp303 != 0){return _tmp303;}{int _tmp304=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp304 != 0){return _tmp304;}{int _tmp305=Cyc_Tcutil_typecmp((void*)f1->type,(void*)
f2->type);if(_tmp305 != 0){return _tmp305;}{int _tmp306=Cyc_Tcutil_list_cmp(Cyc_Tcutil_attribute_cmp,
f1->attributes,f2->attributes);if(_tmp306 != 0){return _tmp306;}return((int(*)(int(*
cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,f1->width,f2->width);}}}}
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2){int _tmp307=Cyc_Absyn_qvar_cmp(e1->name,e2->name);if(_tmp307 != 0){return
_tmp307;}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Tcutil_const_uint_exp_cmp,
e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(void*t){void*_tmp308=t;
_LL3F3: if((int)_tmp308 == 0){goto _LL3F4;}else{goto _LL3F5;}_LL3F5: if((unsigned int)
_tmp308 > 3?*((int*)_tmp308)== 0: 0){goto _LL3F6;}else{goto _LL3F7;}_LL3F7: if((
unsigned int)_tmp308 > 3?*((int*)_tmp308)== 1: 0){goto _LL3F8;}else{goto _LL3F9;}
_LL3F9: if((unsigned int)_tmp308 > 3?*((int*)_tmp308)== 2: 0){goto _LL3FA;}else{goto
_LL3FB;}_LL3FB: if((unsigned int)_tmp308 > 3?*((int*)_tmp308)== 3: 0){goto _LL3FC;}
else{goto _LL3FD;}_LL3FD: if((unsigned int)_tmp308 > 3?*((int*)_tmp308)== 4: 0){goto
_LL3FE;}else{goto _LL3FF;}_LL3FF: if((unsigned int)_tmp308 > 3?*((int*)_tmp308)== 5:
0){goto _LL400;}else{goto _LL401;}_LL401: if((int)_tmp308 == 1){goto _LL402;}else{
goto _LL403;}_LL403: if((unsigned int)_tmp308 > 3?*((int*)_tmp308)== 6: 0){goto
_LL404;}else{goto _LL405;}_LL405: if((unsigned int)_tmp308 > 3?*((int*)_tmp308)== 7:
0){goto _LL406;}else{goto _LL407;}_LL407: if((unsigned int)_tmp308 > 3?*((int*)
_tmp308)== 8: 0){goto _LL408;}else{goto _LL409;}_LL409: if((unsigned int)_tmp308 > 3?*((
int*)_tmp308)== 9: 0){goto _LL40A;}else{goto _LL40B;}_LL40B: if((unsigned int)
_tmp308 > 3?*((int*)_tmp308)== 10: 0){goto _LL40C;}else{goto _LL40D;}_LL40D: if((
unsigned int)_tmp308 > 3?*((int*)_tmp308)== 11: 0){goto _LL40E;}else{goto _LL40F;}
_LL40F: if((unsigned int)_tmp308 > 3?*((int*)_tmp308)== 12: 0){goto _LL410;}else{
goto _LL411;}_LL411: if((unsigned int)_tmp308 > 3?*((int*)_tmp308)== 13: 0){goto
_LL412;}else{goto _LL413;}_LL413: if((unsigned int)_tmp308 > 3?*((int*)_tmp308)== 
15: 0){goto _LL414;}else{goto _LL415;}_LL415: if((unsigned int)_tmp308 > 3?*((int*)
_tmp308)== 16: 0){goto _LL416;}else{goto _LL417;}_LL417: if((int)_tmp308 == 2){goto
_LL418;}else{goto _LL419;}_LL419: if((unsigned int)_tmp308 > 3?*((int*)_tmp308)== 
17: 0){goto _LL41A;}else{goto _LL41B;}_LL41B: if((unsigned int)_tmp308 > 3?*((int*)
_tmp308)== 18: 0){goto _LL41C;}else{goto _LL41D;}_LL41D: if((unsigned int)_tmp308 > 3?*((
int*)_tmp308)== 19: 0){goto _LL41E;}else{goto _LL41F;}_LL41F: if((unsigned int)
_tmp308 > 3?*((int*)_tmp308)== 14: 0){goto _LL420;}else{goto _LL3F2;}_LL3F4: return 0;
_LL3F6: return 1;_LL3F8: return 2;_LL3FA: return 3;_LL3FC: return 4;_LL3FE: return 5;
_LL400: return 6;_LL402: return 7;_LL404: return 8;_LL406: return 9;_LL408: return 10;
_LL40A: return 11;_LL40C: return 12;_LL40E: return 14;_LL410: return 16;_LL412: return 17;
_LL414: return 18;_LL416: return 19;_LL418: return 20;_LL41A: return 21;_LL41C: return 22;
_LL41E: return 23;_LL420: return 24;_LL3F2:;}int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2){return 0;}{int
_tmp309=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(
t2));if(_tmp309 != 0){return _tmp309;}{struct _tuple9 _tmp30B=({struct _tuple9 _tmp30A;
_tmp30A.f1=t2;_tmp30A.f2=t1;_tmp30A;});void*_tmp30C;void*_tmp30D;void*_tmp30E;
void*_tmp30F;void*_tmp310;struct Cyc_Absyn_Tvar*_tmp311;void*_tmp312;struct Cyc_Absyn_Tvar*
_tmp313;void*_tmp314;struct Cyc_Absyn_AggrInfo _tmp315;struct Cyc_List_List*_tmp316;
void*_tmp317;void*_tmp318;struct Cyc_Absyn_AggrInfo _tmp319;struct Cyc_List_List*
_tmp31A;void*_tmp31B;void*_tmp31C;struct _tuple1*_tmp31D;void*_tmp31E;struct
_tuple1*_tmp31F;void*_tmp320;struct Cyc_List_List*_tmp321;void*_tmp322;struct Cyc_List_List*
_tmp323;void*_tmp324;struct Cyc_Absyn_TunionInfo _tmp325;void*_tmp326;struct Cyc_List_List*
_tmp327;void*_tmp328;struct Cyc_Absyn_Tuniondecl**_tmp329;struct Cyc_Absyn_Tuniondecl*
_tmp32A;void*_tmp32B;struct Cyc_Absyn_TunionInfo _tmp32C;void*_tmp32D;struct Cyc_List_List*
_tmp32E;void*_tmp32F;struct Cyc_Absyn_Tuniondecl**_tmp330;struct Cyc_Absyn_Tuniondecl*
_tmp331;void*_tmp332;struct Cyc_Absyn_TunionFieldInfo _tmp333;struct Cyc_List_List*
_tmp334;void*_tmp335;struct Cyc_Absyn_Tunionfield*_tmp336;struct Cyc_Absyn_Tuniondecl*
_tmp337;void*_tmp338;struct Cyc_Absyn_TunionFieldInfo _tmp339;struct Cyc_List_List*
_tmp33A;void*_tmp33B;struct Cyc_Absyn_Tunionfield*_tmp33C;struct Cyc_Absyn_Tuniondecl*
_tmp33D;void*_tmp33E;struct Cyc_Absyn_PtrInfo _tmp33F;struct Cyc_Absyn_Conref*
_tmp340;struct Cyc_Absyn_Tqual _tmp341;struct Cyc_Absyn_Conref*_tmp342;void*_tmp343;
void*_tmp344;void*_tmp345;struct Cyc_Absyn_PtrInfo _tmp346;struct Cyc_Absyn_Conref*
_tmp347;struct Cyc_Absyn_Tqual _tmp348;struct Cyc_Absyn_Conref*_tmp349;void*_tmp34A;
void*_tmp34B;void*_tmp34C;void*_tmp34D;void*_tmp34E;void*_tmp34F;void*_tmp350;
void*_tmp351;void*_tmp352;void*_tmp353;void*_tmp354;int _tmp355;void*_tmp356;int
_tmp357;void*_tmp358;struct Cyc_Absyn_Exp*_tmp359;struct Cyc_Absyn_Tqual _tmp35A;
void*_tmp35B;void*_tmp35C;struct Cyc_Absyn_Exp*_tmp35D;struct Cyc_Absyn_Tqual
_tmp35E;void*_tmp35F;void*_tmp360;struct Cyc_Absyn_FnInfo _tmp361;struct Cyc_List_List*
_tmp362;struct Cyc_List_List*_tmp363;struct Cyc_Absyn_VarargInfo*_tmp364;int
_tmp365;struct Cyc_List_List*_tmp366;void*_tmp367;struct Cyc_Core_Opt*_tmp368;
struct Cyc_List_List*_tmp369;void*_tmp36A;struct Cyc_Absyn_FnInfo _tmp36B;struct Cyc_List_List*
_tmp36C;struct Cyc_List_List*_tmp36D;struct Cyc_Absyn_VarargInfo*_tmp36E;int
_tmp36F;struct Cyc_List_List*_tmp370;void*_tmp371;struct Cyc_Core_Opt*_tmp372;
struct Cyc_List_List*_tmp373;void*_tmp374;struct Cyc_List_List*_tmp375;void*
_tmp376;struct Cyc_List_List*_tmp377;void*_tmp378;struct Cyc_List_List*_tmp379;
void*_tmp37A;void*_tmp37B;struct Cyc_List_List*_tmp37C;void*_tmp37D;void*_tmp37E;
void*_tmp37F;void*_tmp380;void*_tmp381;void*_tmp382;void*_tmp383;void*_tmp384;
void*_tmp385;void*_tmp386;void*_tmp387;void*_tmp388;void*_tmp389;void*_tmp38A;
void*_tmp38B;void*_tmp38C;void*_tmp38D;_LL422: _LL457: _tmp30D=_tmp30B.f1;if((
unsigned int)_tmp30D > 3?*((int*)_tmp30D)== 0: 0){goto _LL456;}else{goto _LL424;}
_LL456: _tmp30C=_tmp30B.f2;if((unsigned int)_tmp30C > 3?*((int*)_tmp30C)== 0: 0){
goto _LL423;}else{goto _LL424;}_LL424: _LL459: _tmp30F=_tmp30B.f1;if((int)_tmp30F == 
0){goto _LL458;}else{goto _LL426;}_LL458: _tmp30E=_tmp30B.f2;if((int)_tmp30E == 0){
goto _LL425;}else{goto _LL426;}_LL426: _LL45C: _tmp312=_tmp30B.f1;if((unsigned int)
_tmp312 > 3?*((int*)_tmp312)== 1: 0){_LL45D: _tmp313=((struct Cyc_Absyn_VarType_struct*)
_tmp312)->f1;goto _LL45A;}else{goto _LL428;}_LL45A: _tmp310=_tmp30B.f2;if((
unsigned int)_tmp310 > 3?*((int*)_tmp310)== 1: 0){_LL45B: _tmp311=((struct Cyc_Absyn_VarType_struct*)
_tmp310)->f1;goto _LL427;}else{goto _LL428;}_LL428: _LL462: _tmp318=_tmp30B.f1;if((
unsigned int)_tmp318 > 3?*((int*)_tmp318)== 10: 0){_LL463: _tmp319=((struct Cyc_Absyn_AggrType_struct*)
_tmp318)->f1;_LL465: _tmp31B=(void*)_tmp319.aggr_info;goto _LL464;_LL464: _tmp31A=
_tmp319.targs;goto _LL45E;}else{goto _LL42A;}_LL45E: _tmp314=_tmp30B.f2;if((
unsigned int)_tmp314 > 3?*((int*)_tmp314)== 10: 0){_LL45F: _tmp315=((struct Cyc_Absyn_AggrType_struct*)
_tmp314)->f1;_LL461: _tmp317=(void*)_tmp315.aggr_info;goto _LL460;_LL460: _tmp316=
_tmp315.targs;goto _LL429;}else{goto _LL42A;}_LL42A: _LL468: _tmp31E=_tmp30B.f1;if((
unsigned int)_tmp31E > 3?*((int*)_tmp31E)== 12: 0){_LL469: _tmp31F=((struct Cyc_Absyn_EnumType_struct*)
_tmp31E)->f1;goto _LL466;}else{goto _LL42C;}_LL466: _tmp31C=_tmp30B.f2;if((
unsigned int)_tmp31C > 3?*((int*)_tmp31C)== 12: 0){_LL467: _tmp31D=((struct Cyc_Absyn_EnumType_struct*)
_tmp31C)->f1;goto _LL42B;}else{goto _LL42C;}_LL42C: _LL46C: _tmp322=_tmp30B.f1;if((
unsigned int)_tmp322 > 3?*((int*)_tmp322)== 13: 0){_LL46D: _tmp323=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp322)->f1;goto _LL46A;}else{goto _LL42E;}_LL46A: _tmp320=_tmp30B.f2;if((
unsigned int)_tmp320 > 3?*((int*)_tmp320)== 13: 0){_LL46B: _tmp321=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp320)->f1;goto _LL42D;}else{goto _LL42E;}_LL42E: _LL474: _tmp32B=_tmp30B.f1;if((
unsigned int)_tmp32B > 3?*((int*)_tmp32B)== 2: 0){_LL475: _tmp32C=((struct Cyc_Absyn_TunionType_struct*)
_tmp32B)->f1;_LL478: _tmp32F=(void*)_tmp32C.tunion_info;if(*((int*)_tmp32F)== 1){
_LL479: _tmp330=((struct Cyc_Absyn_KnownTunion_struct*)_tmp32F)->f1;_tmp331=*
_tmp330;goto _LL477;}else{goto _LL430;}_LL477: _tmp32E=_tmp32C.targs;goto _LL476;
_LL476: _tmp32D=(void*)_tmp32C.rgn;goto _LL46E;}else{goto _LL430;}_LL46E: _tmp324=
_tmp30B.f2;if((unsigned int)_tmp324 > 3?*((int*)_tmp324)== 2: 0){_LL46F: _tmp325=((
struct Cyc_Absyn_TunionType_struct*)_tmp324)->f1;_LL472: _tmp328=(void*)_tmp325.tunion_info;
if(*((int*)_tmp328)== 1){_LL473: _tmp329=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp328)->f1;_tmp32A=*_tmp329;goto _LL471;}else{goto _LL430;}_LL471: _tmp327=
_tmp325.targs;goto _LL470;_LL470: _tmp326=(void*)_tmp325.rgn;goto _LL42F;}else{goto
_LL430;}_LL430: _LL480: _tmp338=_tmp30B.f1;if((unsigned int)_tmp338 > 3?*((int*)
_tmp338)== 3: 0){_LL481: _tmp339=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp338)->f1;
_LL483: _tmp33B=(void*)_tmp339.field_info;if(*((int*)_tmp33B)== 1){_LL485: _tmp33D=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp33B)->f1;goto _LL484;_LL484: _tmp33C=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp33B)->f2;goto _LL482;}else{goto
_LL432;}_LL482: _tmp33A=_tmp339.targs;goto _LL47A;}else{goto _LL432;}_LL47A: _tmp332=
_tmp30B.f2;if((unsigned int)_tmp332 > 3?*((int*)_tmp332)== 3: 0){_LL47B: _tmp333=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp332)->f1;_LL47D: _tmp335=(void*)
_tmp333.field_info;if(*((int*)_tmp335)== 1){_LL47F: _tmp337=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp335)->f1;goto _LL47E;_LL47E: _tmp336=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp335)->f2;goto _LL47C;}else{goto _LL432;}_LL47C: _tmp334=_tmp333.targs;goto
_LL431;}else{goto _LL432;}_LL432: _LL48D: _tmp345=_tmp30B.f1;if((unsigned int)
_tmp345 > 3?*((int*)_tmp345)== 4: 0){_LL48E: _tmp346=((struct Cyc_Absyn_PointerType_struct*)
_tmp345)->f1;_LL493: _tmp34B=(void*)_tmp346.elt_typ;goto _LL492;_LL492: _tmp34A=(
void*)_tmp346.rgn_typ;goto _LL491;_LL491: _tmp349=_tmp346.nullable;goto _LL490;
_LL490: _tmp348=_tmp346.tq;goto _LL48F;_LL48F: _tmp347=_tmp346.bounds;goto _LL486;}
else{goto _LL434;}_LL486: _tmp33E=_tmp30B.f2;if((unsigned int)_tmp33E > 3?*((int*)
_tmp33E)== 4: 0){_LL487: _tmp33F=((struct Cyc_Absyn_PointerType_struct*)_tmp33E)->f1;
_LL48C: _tmp344=(void*)_tmp33F.elt_typ;goto _LL48B;_LL48B: _tmp343=(void*)_tmp33F.rgn_typ;
goto _LL48A;_LL48A: _tmp342=_tmp33F.nullable;goto _LL489;_LL489: _tmp341=_tmp33F.tq;
goto _LL488;_LL488: _tmp340=_tmp33F.bounds;goto _LL433;}else{goto _LL434;}_LL434:
_LL497: _tmp34F=_tmp30B.f1;if((unsigned int)_tmp34F > 3?*((int*)_tmp34F)== 5: 0){
_LL499: _tmp351=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp34F)->f1;goto _LL498;
_LL498: _tmp350=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp34F)->f2;goto _LL494;}
else{goto _LL436;}_LL494: _tmp34C=_tmp30B.f2;if((unsigned int)_tmp34C > 3?*((int*)
_tmp34C)== 5: 0){_LL496: _tmp34E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp34C)->f1;
goto _LL495;_LL495: _tmp34D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp34C)->f2;
goto _LL435;}else{goto _LL436;}_LL436: _LL49B: _tmp353=_tmp30B.f1;if((int)_tmp353 == 
1){goto _LL49A;}else{goto _LL438;}_LL49A: _tmp352=_tmp30B.f2;if((int)_tmp352 == 1){
goto _LL437;}else{goto _LL438;}_LL438: _LL49E: _tmp356=_tmp30B.f1;if((unsigned int)
_tmp356 > 3?*((int*)_tmp356)== 6: 0){_LL49F: _tmp357=((struct Cyc_Absyn_DoubleType_struct*)
_tmp356)->f1;goto _LL49C;}else{goto _LL43A;}_LL49C: _tmp354=_tmp30B.f2;if((
unsigned int)_tmp354 > 3?*((int*)_tmp354)== 6: 0){_LL49D: _tmp355=((struct Cyc_Absyn_DoubleType_struct*)
_tmp354)->f1;goto _LL439;}else{goto _LL43A;}_LL43A: _LL4A4: _tmp35C=_tmp30B.f1;if((
unsigned int)_tmp35C > 3?*((int*)_tmp35C)== 7: 0){_LL4A7: _tmp35F=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp35C)->f1;goto _LL4A6;_LL4A6: _tmp35E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp35C)->f2;goto _LL4A5;_LL4A5: _tmp35D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp35C)->f3;goto _LL4A0;}else{goto _LL43C;}_LL4A0: _tmp358=_tmp30B.f2;if((
unsigned int)_tmp358 > 3?*((int*)_tmp358)== 7: 0){_LL4A3: _tmp35B=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp358)->f1;goto _LL4A2;_LL4A2: _tmp35A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp358)->f2;goto _LL4A1;_LL4A1: _tmp359=((struct Cyc_Absyn_ArrayType_struct*)
_tmp358)->f3;goto _LL43B;}else{goto _LL43C;}_LL43C: _LL4B2: _tmp36A=_tmp30B.f1;if((
unsigned int)_tmp36A > 3?*((int*)_tmp36A)== 8: 0){_LL4B3: _tmp36B=((struct Cyc_Absyn_FnType_struct*)
_tmp36A)->f1;_LL4BB: _tmp373=_tmp36B.tvars;goto _LL4BA;_LL4BA: _tmp372=_tmp36B.effect;
goto _LL4B9;_LL4B9: _tmp371=(void*)_tmp36B.ret_typ;goto _LL4B8;_LL4B8: _tmp370=
_tmp36B.args;goto _LL4B7;_LL4B7: _tmp36F=_tmp36B.c_varargs;goto _LL4B6;_LL4B6:
_tmp36E=_tmp36B.cyc_varargs;goto _LL4B5;_LL4B5: _tmp36D=_tmp36B.rgn_po;goto _LL4B4;
_LL4B4: _tmp36C=_tmp36B.attributes;goto _LL4A8;}else{goto _LL43E;}_LL4A8: _tmp360=
_tmp30B.f2;if((unsigned int)_tmp360 > 3?*((int*)_tmp360)== 8: 0){_LL4A9: _tmp361=((
struct Cyc_Absyn_FnType_struct*)_tmp360)->f1;_LL4B1: _tmp369=_tmp361.tvars;goto
_LL4B0;_LL4B0: _tmp368=_tmp361.effect;goto _LL4AF;_LL4AF: _tmp367=(void*)_tmp361.ret_typ;
goto _LL4AE;_LL4AE: _tmp366=_tmp361.args;goto _LL4AD;_LL4AD: _tmp365=_tmp361.c_varargs;
goto _LL4AC;_LL4AC: _tmp364=_tmp361.cyc_varargs;goto _LL4AB;_LL4AB: _tmp363=_tmp361.rgn_po;
goto _LL4AA;_LL4AA: _tmp362=_tmp361.attributes;goto _LL43D;}else{goto _LL43E;}_LL43E:
_LL4BE: _tmp376=_tmp30B.f1;if((unsigned int)_tmp376 > 3?*((int*)_tmp376)== 9: 0){
_LL4BF: _tmp377=((struct Cyc_Absyn_TupleType_struct*)_tmp376)->f1;goto _LL4BC;}
else{goto _LL440;}_LL4BC: _tmp374=_tmp30B.f2;if((unsigned int)_tmp374 > 3?*((int*)
_tmp374)== 9: 0){_LL4BD: _tmp375=((struct Cyc_Absyn_TupleType_struct*)_tmp374)->f1;
goto _LL43F;}else{goto _LL440;}_LL440: _LL4C3: _tmp37B=_tmp30B.f1;if((unsigned int)
_tmp37B > 3?*((int*)_tmp37B)== 11: 0){_LL4C5: _tmp37D=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp37B)->f1;goto _LL4C4;_LL4C4: _tmp37C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp37B)->f2;goto _LL4C0;}else{goto _LL442;}_LL4C0: _tmp378=_tmp30B.f2;if((
unsigned int)_tmp378 > 3?*((int*)_tmp378)== 11: 0){_LL4C2: _tmp37A=(void*)((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp378)->f1;goto _LL4C1;_LL4C1: _tmp379=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp378)->f2;goto _LL441;}else{goto _LL442;}_LL442:
_LL4C7: _tmp37F=_tmp30B.f1;if((int)_tmp37F == 2){goto _LL4C6;}else{goto _LL444;}
_LL4C6: _tmp37E=_tmp30B.f2;if((int)_tmp37E == 2){goto _LL443;}else{goto _LL444;}
_LL444: _LL4CA: _tmp382=_tmp30B.f1;if((unsigned int)_tmp382 > 3?*((int*)_tmp382)== 
15: 0){_LL4CB: _tmp383=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp382)->f1;
goto _LL4C8;}else{goto _LL446;}_LL4C8: _tmp380=_tmp30B.f2;if((unsigned int)_tmp380 > 
3?*((int*)_tmp380)== 15: 0){_LL4C9: _tmp381=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp380)->f1;goto _LL445;}else{goto _LL446;}_LL446: _LL4CE: _tmp386=_tmp30B.f1;if((
unsigned int)_tmp386 > 3?*((int*)_tmp386)== 14: 0){_LL4CF: _tmp387=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp386)->f1;goto _LL4CC;}else{goto _LL448;}_LL4CC:
_tmp384=_tmp30B.f2;if((unsigned int)_tmp384 > 3?*((int*)_tmp384)== 14: 0){_LL4CD:
_tmp385=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp384)->f1;goto _LL447;}
else{goto _LL448;}_LL448: _LL4D0: _tmp388=_tmp30B.f1;if((unsigned int)_tmp388 > 3?*((
int*)_tmp388)== 18: 0){goto _LL449;}else{goto _LL44A;}_LL44A: _LL4D1: _tmp389=_tmp30B.f2;
if((unsigned int)_tmp389 > 3?*((int*)_tmp389)== 18: 0){goto _LL44B;}else{goto _LL44C;}
_LL44C: _LL4D2: _tmp38A=_tmp30B.f1;if((unsigned int)_tmp38A > 3?*((int*)_tmp38A)== 
17: 0){goto _LL44D;}else{goto _LL44E;}_LL44E: _LL4D3: _tmp38B=_tmp30B.f1;if((
unsigned int)_tmp38B > 3?*((int*)_tmp38B)== 19: 0){goto _LL44F;}else{goto _LL450;}
_LL450: _LL4D4: _tmp38C=_tmp30B.f2;if((unsigned int)_tmp38C > 3?*((int*)_tmp38C)== 
19: 0){goto _LL451;}else{goto _LL452;}_LL452: _LL4D5: _tmp38D=_tmp30B.f2;if((
unsigned int)_tmp38D > 3?*((int*)_tmp38D)== 17: 0){goto _LL453;}else{goto _LL454;}
_LL454: goto _LL455;_LL423:({void*_tmp38E[0]={};((int(*)(struct _tagged_arr fmt,
struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: can only compare closed types",
sizeof(unsigned char),39),_tag_arr(_tmp38E,sizeof(void*),0));});_LL425: return 0;
_LL427: return Cyc_Core_intcmp(*((int*)_check_null(_tmp311->identity)),*((int*)
_check_null(_tmp313->identity)));_LL429: {struct _tuple1*_tmp390;struct _tuple5
_tmp38F=Cyc_Absyn_aggr_kinded_name(_tmp31B);_LL4D7: _tmp390=_tmp38F.f2;goto _LL4D6;
_LL4D6: {struct _tuple1*_tmp392;struct _tuple5 _tmp391=Cyc_Absyn_aggr_kinded_name(
_tmp317);_LL4D9: _tmp392=_tmp391.f2;goto _LL4D8;_LL4D8: {int _tmp393=Cyc_Absyn_qvar_cmp(
_tmp390,_tmp392);if(_tmp393 != 0){return _tmp393;}else{return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp,_tmp31A,_tmp316);}}}}_LL42B: return Cyc_Absyn_qvar_cmp(_tmp31F,
_tmp31D);_LL42D: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_enumfield_cmp,
_tmp323,_tmp321);_LL42F: if(_tmp32A == _tmp331){return 0;}{int _tmp394=Cyc_Absyn_qvar_cmp(
_tmp32A->name,_tmp331->name);if(_tmp394 != 0){return _tmp394;}{int _tmp395=Cyc_Tcutil_typecmp(
_tmp326,_tmp32D);if(_tmp395 != 0){return _tmp395;}return Cyc_Tcutil_list_cmp(Cyc_Tcutil_typecmp,
_tmp327,_tmp32E);}}_LL431: if(_tmp337 == _tmp33D){return 0;}{int _tmp396=Cyc_Absyn_qvar_cmp(
_tmp33D->name,_tmp337->name);if(_tmp396 != 0){return _tmp396;}{int _tmp397=Cyc_Absyn_qvar_cmp(
_tmp33C->name,_tmp336->name);if(_tmp397 != 0){return _tmp397;}return Cyc_Tcutil_list_cmp(
Cyc_Tcutil_typecmp,_tmp334,_tmp33A);}}_LL433: {int _tmp398=Cyc_Tcutil_typecmp(
_tmp344,_tmp34B);if(_tmp398 != 0){return _tmp398;}{int _tmp399=Cyc_Tcutil_typecmp(
_tmp343,_tmp34A);if(_tmp399 != 0){return _tmp399;}{int _tmp39A=Cyc_Tcutil_tqual_cmp(
_tmp341,_tmp348);if(_tmp39A != 0){return _tmp39A;}{int _tmp39B=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp340,_tmp347);if(_tmp39B != 0){return _tmp39B;}{void*
_tmp39C=(void*)(Cyc_Absyn_compress_conref(_tmp340))->v;void*_tmp39D;_LL4DB: if((
unsigned int)_tmp39C > 1?*((int*)_tmp39C)== 0: 0){_LL4DF: _tmp39D=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp39C)->f1;if((int)_tmp39D == 0){goto _LL4DC;}else{
goto _LL4DD;}}else{goto _LL4DD;}_LL4DD: goto _LL4DE;_LL4DC: return 0;_LL4DE: goto _LL4DA;
_LL4DA:;}return((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp342,_tmp349);}}}}_LL435: if(
_tmp34E != _tmp351){return Cyc_Core_intcmp((int)((unsigned int)_tmp34E),(int)((
unsigned int)_tmp351));}if(_tmp34D != _tmp350){return Cyc_Core_intcmp((int)((
unsigned int)_tmp34D),(int)((unsigned int)_tmp350));}return 0;_LL437: return 0;
_LL439: if(_tmp357 == _tmp355){return 0;}else{if(_tmp357){return - 1;}else{return 1;}}
_LL43B: {int _tmp39E=Cyc_Tcutil_tqual_cmp(_tmp35A,_tmp35E);if(_tmp39E != 0){return
_tmp39E;}{int _tmp39F=Cyc_Tcutil_typecmp(_tmp35B,_tmp35F);if(_tmp39F != 0){return
_tmp39F;}if(_tmp35D == _tmp359){return 0;}if(_tmp35D == 0? 1: _tmp359 == 0){({void*
_tmp3A0[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("missing expression in array index",sizeof(unsigned char),34),_tag_arr(
_tmp3A0,sizeof(void*),0));});}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Tcutil_const_uint_exp_cmp,_tmp35D,_tmp359);}}_LL43D:({void*_tmp3A1[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: function types not handled",
sizeof(unsigned char),36),_tag_arr(_tmp3A1,sizeof(void*),0));});_LL43F: return((
int(*)(int(*cmp)(struct _tuple4*,struct _tuple4*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp375,_tmp377);_LL441: if(
_tmp37A != _tmp37D){if(_tmp37A == (void*)0){return - 1;}else{return 1;}}return((int(*)(
int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_Tcutil_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp379,
_tmp37C);_LL443: return 0;_LL445: return Cyc_Tcutil_typecmp(_tmp383,_tmp381);_LL447:
return Cyc_Tcutil_typecmp(_tmp387,_tmp385);_LL449: goto _LL44B;_LL44B: goto _LL44D;
_LL44D: goto _LL44F;_LL44F: goto _LL451;_LL451: goto _LL453;_LL453:({void*_tmp3A2[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("typecmp: effects not handled",
sizeof(unsigned char),29),_tag_arr(_tmp3A2,sizeof(void*),0));});_LL455:({void*
_tmp3A3[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("Unmatched case in typecmp",sizeof(unsigned char),26),_tag_arr(_tmp3A3,
sizeof(void*),0));});_LL421:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t){void*
_tmp3A4=Cyc_Tcutil_compress(t);_LL4E1: if((unsigned int)_tmp3A4 > 3?*((int*)
_tmp3A4)== 5: 0){goto _LL4E2;}else{goto _LL4E3;}_LL4E3: if((int)_tmp3A4 == 1){goto
_LL4E4;}else{goto _LL4E5;}_LL4E5: if((unsigned int)_tmp3A4 > 3?*((int*)_tmp3A4)== 6:
0){goto _LL4E6;}else{goto _LL4E7;}_LL4E7: if((unsigned int)_tmp3A4 > 3?*((int*)
_tmp3A4)== 12: 0){goto _LL4E8;}else{goto _LL4E9;}_LL4E9: if((unsigned int)_tmp3A4 > 3?*((
int*)_tmp3A4)== 13: 0){goto _LL4EA;}else{goto _LL4EB;}_LL4EB: goto _LL4EC;_LL4E2: goto
_LL4E4;_LL4E4: goto _LL4E6;_LL4E6: goto _LL4E8;_LL4E8: goto _LL4EA;_LL4EA: return 1;
_LL4EC: return 0;_LL4E0:;}int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple9 _tmp3A6=({struct _tuple9 _tmp3A5;_tmp3A5.f1=Cyc_Tcutil_compress(t1);
_tmp3A5.f2=Cyc_Tcutil_compress(t2);_tmp3A5;});void*_tmp3A7;int _tmp3A8;void*
_tmp3A9;int _tmp3AA;void*_tmp3AB;void*_tmp3AC;void*_tmp3AD;void*_tmp3AE;void*
_tmp3AF;void*_tmp3B0;void*_tmp3B1;void*_tmp3B2;void*_tmp3B3;void*_tmp3B4;void*
_tmp3B5;void*_tmp3B6;void*_tmp3B7;void*_tmp3B8;void*_tmp3B9;void*_tmp3BA;void*
_tmp3BB;void*_tmp3BC;void*_tmp3BD;void*_tmp3BE;void*_tmp3BF;void*_tmp3C0;void*
_tmp3C1;void*_tmp3C2;void*_tmp3C3;void*_tmp3C4;void*_tmp3C5;void*_tmp3C6;void*
_tmp3C7;void*_tmp3C8;void*_tmp3C9;void*_tmp3CA;void*_tmp3CB;void*_tmp3CC;void*
_tmp3CD;void*_tmp3CE;void*_tmp3CF;_LL4EE: _LL50E: _tmp3A9=_tmp3A6.f1;if((
unsigned int)_tmp3A9 > 3?*((int*)_tmp3A9)== 6: 0){_LL50F: _tmp3AA=((struct Cyc_Absyn_DoubleType_struct*)
_tmp3A9)->f1;goto _LL50C;}else{goto _LL4F0;}_LL50C: _tmp3A7=_tmp3A6.f2;if((
unsigned int)_tmp3A7 > 3?*((int*)_tmp3A7)== 6: 0){_LL50D: _tmp3A8=((struct Cyc_Absyn_DoubleType_struct*)
_tmp3A7)->f1;goto _LL4EF;}else{goto _LL4F0;}_LL4F0: _LL511: _tmp3AC=_tmp3A6.f1;if((
unsigned int)_tmp3AC > 3?*((int*)_tmp3AC)== 6: 0){goto _LL510;}else{goto _LL4F2;}
_LL510: _tmp3AB=_tmp3A6.f2;if((int)_tmp3AB == 1){goto _LL4F1;}else{goto _LL4F2;}
_LL4F2: _LL513: _tmp3AE=_tmp3A6.f1;if((unsigned int)_tmp3AE > 3?*((int*)_tmp3AE)== 
6: 0){goto _LL512;}else{goto _LL4F4;}_LL512: _tmp3AD=_tmp3A6.f2;if((unsigned int)
_tmp3AD > 3?*((int*)_tmp3AD)== 5: 0){goto _LL4F3;}else{goto _LL4F4;}_LL4F4: _LL515:
_tmp3B0=_tmp3A6.f1;if((unsigned int)_tmp3B0 > 3?*((int*)_tmp3B0)== 6: 0){goto
_LL514;}else{goto _LL4F6;}_LL514: _tmp3AF=_tmp3A6.f2;if((unsigned int)_tmp3AF > 3?*((
int*)_tmp3AF)== 14: 0){goto _LL4F5;}else{goto _LL4F6;}_LL4F6: _LL517: _tmp3B2=_tmp3A6.f1;
if((int)_tmp3B2 == 1){goto _LL516;}else{goto _LL4F8;}_LL516: _tmp3B1=_tmp3A6.f2;if((
unsigned int)_tmp3B1 > 3?*((int*)_tmp3B1)== 14: 0){goto _LL4F7;}else{goto _LL4F8;}
_LL4F8: _LL519: _tmp3B4=_tmp3A6.f1;if((int)_tmp3B4 == 1){goto _LL518;}else{goto
_LL4FA;}_LL518: _tmp3B3=_tmp3A6.f2;if((unsigned int)_tmp3B3 > 3?*((int*)_tmp3B3)== 
5: 0){goto _LL4F9;}else{goto _LL4FA;}_LL4FA: _LL51C: _tmp3B7=_tmp3A6.f1;if((
unsigned int)_tmp3B7 > 3?*((int*)_tmp3B7)== 5: 0){_LL51D: _tmp3B8=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp3B7)->f2;if((int)_tmp3B8 == 3){goto _LL51A;}else{
goto _LL4FC;}}else{goto _LL4FC;}_LL51A: _tmp3B5=_tmp3A6.f2;if((unsigned int)_tmp3B5
> 3?*((int*)_tmp3B5)== 5: 0){_LL51B: _tmp3B6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3B5)->f2;if((int)_tmp3B6 == 3){goto _LL4FB;}else{goto _LL4FC;}}else{goto _LL4FC;}
_LL4FC: _LL51E: _tmp3B9=_tmp3A6.f1;if((unsigned int)_tmp3B9 > 3?*((int*)_tmp3B9)== 
5: 0){_LL51F: _tmp3BA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3B9)->f2;if((
int)_tmp3BA == 3){goto _LL4FD;}else{goto _LL4FE;}}else{goto _LL4FE;}_LL4FE: _LL521:
_tmp3BC=_tmp3A6.f1;if((unsigned int)_tmp3BC > 3?*((int*)_tmp3BC)== 5: 0){_LL522:
_tmp3BD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3BC)->f2;if((int)_tmp3BD == 
2){goto _LL520;}else{goto _LL500;}}else{goto _LL500;}_LL520: _tmp3BB=_tmp3A6.f2;if((
int)_tmp3BB == 1){goto _LL4FF;}else{goto _LL500;}_LL500: _LL525: _tmp3C0=_tmp3A6.f1;
if((unsigned int)_tmp3C0 > 3?*((int*)_tmp3C0)== 5: 0){_LL526: _tmp3C1=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp3C0)->f2;if((int)_tmp3C1 == 2){goto _LL523;}
else{goto _LL502;}}else{goto _LL502;}_LL523: _tmp3BE=_tmp3A6.f2;if((unsigned int)
_tmp3BE > 3?*((int*)_tmp3BE)== 5: 0){_LL524: _tmp3BF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3BE)->f2;if((int)_tmp3BF == 1){goto _LL501;}else{goto _LL502;}}else{goto _LL502;}
_LL502: _LL529: _tmp3C4=_tmp3A6.f1;if((unsigned int)_tmp3C4 > 3?*((int*)_tmp3C4)== 
5: 0){_LL52A: _tmp3C5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3C4)->f2;if((
int)_tmp3C5 == 2){goto _LL527;}else{goto _LL504;}}else{goto _LL504;}_LL527: _tmp3C2=
_tmp3A6.f2;if((unsigned int)_tmp3C2 > 3?*((int*)_tmp3C2)== 5: 0){_LL528: _tmp3C3=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp3C2)->f2;if((int)_tmp3C3 == 0){goto
_LL503;}else{goto _LL504;}}else{goto _LL504;}_LL504: _LL52D: _tmp3C8=_tmp3A6.f1;if((
unsigned int)_tmp3C8 > 3?*((int*)_tmp3C8)== 5: 0){_LL52E: _tmp3C9=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp3C8)->f2;if((int)_tmp3C9 == 1){goto _LL52B;}else{
goto _LL506;}}else{goto _LL506;}_LL52B: _tmp3C6=_tmp3A6.f2;if((unsigned int)_tmp3C6
> 3?*((int*)_tmp3C6)== 5: 0){_LL52C: _tmp3C7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3C6)->f2;if((int)_tmp3C7 == 0){goto _LL505;}else{goto _LL506;}}else{goto _LL506;}
_LL506: _LL531: _tmp3CC=_tmp3A6.f1;if((unsigned int)_tmp3CC > 3?*((int*)_tmp3CC)== 
14: 0){goto _LL52F;}else{goto _LL508;}_LL52F: _tmp3CA=_tmp3A6.f2;if((unsigned int)
_tmp3CA > 3?*((int*)_tmp3CA)== 5: 0){_LL530: _tmp3CB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3CA)->f2;if((int)_tmp3CB == 1){goto _LL507;}else{goto _LL508;}}else{goto _LL508;}
_LL508: _LL534: _tmp3CF=_tmp3A6.f1;if((unsigned int)_tmp3CF > 3?*((int*)_tmp3CF)== 
14: 0){goto _LL532;}else{goto _LL50A;}_LL532: _tmp3CD=_tmp3A6.f2;if((unsigned int)
_tmp3CD > 3?*((int*)_tmp3CD)== 5: 0){_LL533: _tmp3CE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3CD)->f2;if((int)_tmp3CE == 0){goto _LL509;}else{goto _LL50A;}}else{goto _LL50A;}
_LL50A: goto _LL50B;_LL4EF: return ! _tmp3A8? _tmp3AA: 0;_LL4F1: goto _LL4F3;_LL4F3: goto
_LL4F5;_LL4F5: goto _LL4F7;_LL4F7: goto _LL4F9;_LL4F9: return 1;_LL4FB: return 0;_LL4FD:
goto _LL4FF;_LL4FF: goto _LL501;_LL501: goto _LL503;_LL503: goto _LL505;_LL505: goto
_LL507;_LL507: goto _LL509;_LL509: return 1;_LL50B: return 0;_LL4ED:;}int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){struct Cyc_Core_Opt*
max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){void*t1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v);
if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0? 1: Cyc_Tcutil_will_lose_precision(
t1,(void*)max_arith_type->v)){max_arith_type=({struct Cyc_Core_Opt*_tmp3D0=
_cycalloc(sizeof(*_tmp3D0));_tmp3D0->v=(void*)t1;_tmp3D0;});}}}}if(
max_arith_type != 0){if(! Cyc_Tcutil_unify(t,(void*)((struct Cyc_Core_Opt*)
_check_null(max_arith_type))->v)){return 0;}}{struct Cyc_List_List*el=es;for(0;el
!= 0;el=el->tl){if(! Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){({
struct Cyc_Std_String_pa_struct _tmp3D3;_tmp3D3.tag=0;_tmp3D3.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)el->hd)->topt))->v);{struct Cyc_Std_String_pa_struct _tmp3D2;
_tmp3D2.tag=0;_tmp3D2.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*
_tmp3D1[2]={& _tmp3D2,& _tmp3D3};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,
_tag_arr("type mismatch: expecting %s but found %s",sizeof(unsigned char),41),
_tag_arr(_tmp3D1,sizeof(void*),2));}}});return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(! Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp3D4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL536: if((unsigned int)_tmp3D4 > 3?*((int*)_tmp3D4)== 4: 0){goto _LL537;}else{goto
_LL538;}_LL538: goto _LL539;_LL537: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_t);
goto _LL535;_LL539: return 0;_LL535:;}return 1;}int Cyc_Tcutil_is_integral_type(void*
t){void*_tmp3D5=Cyc_Tcutil_compress(t);_LL53B: if((unsigned int)_tmp3D5 > 3?*((int*)
_tmp3D5)== 5: 0){goto _LL53C;}else{goto _LL53D;}_LL53D: if((unsigned int)_tmp3D5 > 3?*((
int*)_tmp3D5)== 14: 0){goto _LL53E;}else{goto _LL53F;}_LL53F: if((unsigned int)
_tmp3D5 > 3?*((int*)_tmp3D5)== 12: 0){goto _LL540;}else{goto _LL541;}_LL541: if((
unsigned int)_tmp3D5 > 3?*((int*)_tmp3D5)== 13: 0){goto _LL542;}else{goto _LL543;}
_LL543: goto _LL544;_LL53C: goto _LL53E;_LL53E: goto _LL540;_LL540: goto _LL542;_LL542:
return 1;_LL544: return 0;_LL53A:;}int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_uint_t)){return 1;}if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_t)){({void*
_tmp3D6[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(unsigned char),44),_tag_arr(_tmp3D6,sizeof(void*),0));});}Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_uint_t);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_t)){return 1;}if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_t)){({void*
_tmp3D7[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("integral size mismatch; conversion supplied",
sizeof(unsigned char),44),_tag_arr(_tmp3D7,sizeof(void*),0));});}Cyc_Tcutil_unchecked_cast(
te,e,Cyc_Absyn_sint_t);return 1;}return 0;}int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(t1);t2=
Cyc_Tcutil_compress(t2);{struct _tuple9 _tmp3D9=({struct _tuple9 _tmp3D8;_tmp3D8.f1=
t1;_tmp3D8.f2=t2;_tmp3D8;});void*_tmp3DA;struct Cyc_Absyn_PtrInfo _tmp3DB;void*
_tmp3DC;struct Cyc_Absyn_PtrInfo _tmp3DD;void*_tmp3DE;struct Cyc_Absyn_Exp*_tmp3DF;
struct Cyc_Absyn_Tqual _tmp3E0;void*_tmp3E1;void*_tmp3E2;struct Cyc_Absyn_Exp*
_tmp3E3;struct Cyc_Absyn_Tqual _tmp3E4;void*_tmp3E5;void*_tmp3E6;struct Cyc_Absyn_TunionInfo
_tmp3E7;struct Cyc_List_List*_tmp3E8;void*_tmp3E9;struct Cyc_Absyn_Tuniondecl**
_tmp3EA;struct Cyc_Absyn_Tuniondecl*_tmp3EB;void*_tmp3EC;struct Cyc_Absyn_TunionFieldInfo
_tmp3ED;struct Cyc_List_List*_tmp3EE;void*_tmp3EF;struct Cyc_Absyn_Tunionfield*
_tmp3F0;struct Cyc_Absyn_Tuniondecl*_tmp3F1;void*_tmp3F2;struct Cyc_Absyn_TunionInfo
_tmp3F3;void*_tmp3F4;struct Cyc_List_List*_tmp3F5;void*_tmp3F6;struct Cyc_Absyn_Tuniondecl**
_tmp3F7;struct Cyc_Absyn_Tuniondecl*_tmp3F8;void*_tmp3F9;struct Cyc_Absyn_PtrInfo
_tmp3FA;struct Cyc_Absyn_Conref*_tmp3FB;struct Cyc_Absyn_Tqual _tmp3FC;struct Cyc_Absyn_Conref*
_tmp3FD;void*_tmp3FE;void*_tmp3FF;void*_tmp400;void*_tmp401;void*_tmp402;_LL546:
_LL554: _tmp3DC=_tmp3D9.f1;if((unsigned int)_tmp3DC > 3?*((int*)_tmp3DC)== 4: 0){
_LL555: _tmp3DD=((struct Cyc_Absyn_PointerType_struct*)_tmp3DC)->f1;goto _LL552;}
else{goto _LL548;}_LL552: _tmp3DA=_tmp3D9.f2;if((unsigned int)_tmp3DA > 3?*((int*)
_tmp3DA)== 4: 0){_LL553: _tmp3DB=((struct Cyc_Absyn_PointerType_struct*)_tmp3DA)->f1;
goto _LL547;}else{goto _LL548;}_LL548: _LL55A: _tmp3E2=_tmp3D9.f1;if((unsigned int)
_tmp3E2 > 3?*((int*)_tmp3E2)== 7: 0){_LL55D: _tmp3E5=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp3E2)->f1;goto _LL55C;_LL55C: _tmp3E4=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3E2)->f2;goto _LL55B;_LL55B: _tmp3E3=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3E2)->f3;goto _LL556;}else{goto _LL54A;}_LL556: _tmp3DE=_tmp3D9.f2;if((
unsigned int)_tmp3DE > 3?*((int*)_tmp3DE)== 7: 0){_LL559: _tmp3E1=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp3DE)->f1;goto _LL558;_LL558: _tmp3E0=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3DE)->f2;goto _LL557;_LL557: _tmp3DF=((struct Cyc_Absyn_ArrayType_struct*)
_tmp3DE)->f3;goto _LL549;}else{goto _LL54A;}_LL54A: _LL563: _tmp3EC=_tmp3D9.f1;if((
unsigned int)_tmp3EC > 3?*((int*)_tmp3EC)== 3: 0){_LL564: _tmp3ED=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3EC)->f1;_LL566: _tmp3EF=(void*)_tmp3ED.field_info;if(*((int*)_tmp3EF)== 1){
_LL568: _tmp3F1=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3EF)->f1;goto
_LL567;_LL567: _tmp3F0=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp3EF)->f2;
goto _LL565;}else{goto _LL54C;}_LL565: _tmp3EE=_tmp3ED.targs;goto _LL55E;}else{goto
_LL54C;}_LL55E: _tmp3E6=_tmp3D9.f2;if((unsigned int)_tmp3E6 > 3?*((int*)_tmp3E6)== 
2: 0){_LL55F: _tmp3E7=((struct Cyc_Absyn_TunionType_struct*)_tmp3E6)->f1;_LL561:
_tmp3E9=(void*)_tmp3E7.tunion_info;if(*((int*)_tmp3E9)== 1){_LL562: _tmp3EA=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp3E9)->f1;_tmp3EB=*_tmp3EA;goto _LL560;}
else{goto _LL54C;}_LL560: _tmp3E8=_tmp3E7.targs;goto _LL54B;}else{goto _LL54C;}
_LL54C: _LL56F: _tmp3F9=_tmp3D9.f1;if((unsigned int)_tmp3F9 > 3?*((int*)_tmp3F9)== 
4: 0){_LL570: _tmp3FA=((struct Cyc_Absyn_PointerType_struct*)_tmp3F9)->f1;_LL575:
_tmp3FF=(void*)_tmp3FA.elt_typ;goto _LL574;_LL574: _tmp3FE=(void*)_tmp3FA.rgn_typ;
goto _LL573;_LL573: _tmp3FD=_tmp3FA.nullable;goto _LL572;_LL572: _tmp3FC=_tmp3FA.tq;
goto _LL571;_LL571: _tmp3FB=_tmp3FA.bounds;goto _LL569;}else{goto _LL54E;}_LL569:
_tmp3F2=_tmp3D9.f2;if((unsigned int)_tmp3F2 > 3?*((int*)_tmp3F2)== 2: 0){_LL56A:
_tmp3F3=((struct Cyc_Absyn_TunionType_struct*)_tmp3F2)->f1;_LL56D: _tmp3F6=(void*)
_tmp3F3.tunion_info;if(*((int*)_tmp3F6)== 1){_LL56E: _tmp3F7=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp3F6)->f1;_tmp3F8=*_tmp3F7;goto _LL56C;}else{goto _LL54E;}_LL56C: _tmp3F5=
_tmp3F3.targs;goto _LL56B;_LL56B: _tmp3F4=(void*)_tmp3F3.rgn;goto _LL54D;}else{goto
_LL54E;}_LL54E: _LL578: _tmp402=_tmp3D9.f1;if((unsigned int)_tmp402 > 3?*((int*)
_tmp402)== 14: 0){goto _LL576;}else{goto _LL550;}_LL576: _tmp400=_tmp3D9.f2;if((
unsigned int)_tmp400 > 3?*((int*)_tmp400)== 5: 0){_LL577: _tmp401=(void*)((struct
Cyc_Absyn_IntType_struct*)_tmp400)->f2;if((int)_tmp401 == 2){goto _LL54F;}else{
goto _LL550;}}else{goto _LL550;}_LL550: goto _LL551;_LL547: {int okay=1;if(!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp3DD.nullable,_tmp3DB.nullable)){void*_tmp403=(void*)(((
struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp3DD.nullable))->v;int _tmp404;_LL57A: if((unsigned int)_tmp403 > 1?*((int*)
_tmp403)== 0: 0){_LL57E: _tmp404=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp403)->f1;
goto _LL57B;}else{goto _LL57C;}_LL57C: goto _LL57D;_LL57B: okay=! _tmp404;goto _LL579;
_LL57D:({void*_tmp405[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("silent_castable conref not eq",sizeof(unsigned char),
30),_tag_arr(_tmp405,sizeof(void*),0));});_LL579:;}if(! Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_boundscmp,_tmp3DD.bounds,_tmp3DB.bounds)){struct _tuple9 _tmp407=({
struct _tuple9 _tmp406;_tmp406.f1=(void*)(Cyc_Absyn_compress_conref(_tmp3DD.bounds))->v;
_tmp406.f2=(void*)(Cyc_Absyn_compress_conref(_tmp3DB.bounds))->v;_tmp406;});void*
_tmp408;void*_tmp409;void*_tmp40A;void*_tmp40B;void*_tmp40C;void*_tmp40D;struct
Cyc_Absyn_Exp*_tmp40E;void*_tmp40F;void*_tmp410;struct Cyc_Absyn_Exp*_tmp411;void*
_tmp412;void*_tmp413;void*_tmp414;void*_tmp415;_LL580: _LL58A: _tmp40A=_tmp407.f1;
if((unsigned int)_tmp40A > 1?*((int*)_tmp40A)== 0: 0){_LL58B: _tmp40B=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp40A)->f1;if((unsigned int)_tmp40B > 1?*((
int*)_tmp40B)== 0: 0){goto _LL588;}else{goto _LL582;}}else{goto _LL582;}_LL588:
_tmp408=_tmp407.f2;if((unsigned int)_tmp408 > 1?*((int*)_tmp408)== 0: 0){_LL589:
_tmp409=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp408)->f1;if((int)_tmp409
== 0){goto _LL581;}else{goto _LL582;}}else{goto _LL582;}_LL582: _LL58F: _tmp40F=
_tmp407.f1;if((unsigned int)_tmp40F > 1?*((int*)_tmp40F)== 0: 0){_LL590: _tmp410=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp40F)->f1;if((unsigned int)_tmp410 > 
1?*((int*)_tmp410)== 0: 0){_LL591: _tmp411=((struct Cyc_Absyn_Upper_b_struct*)
_tmp410)->f1;goto _LL58C;}else{goto _LL584;}}else{goto _LL584;}_LL58C: _tmp40C=
_tmp407.f2;if((unsigned int)_tmp40C > 1?*((int*)_tmp40C)== 0: 0){_LL58D: _tmp40D=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp40C)->f1;if((unsigned int)_tmp40D > 
1?*((int*)_tmp40D)== 0: 0){_LL58E: _tmp40E=((struct Cyc_Absyn_Upper_b_struct*)
_tmp40D)->f1;goto _LL583;}else{goto _LL584;}}else{goto _LL584;}_LL584: _LL594:
_tmp414=_tmp407.f1;if((unsigned int)_tmp414 > 1?*((int*)_tmp414)== 0: 0){_LL595:
_tmp415=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp414)->f1;if((int)_tmp415
== 0){goto _LL592;}else{goto _LL586;}}else{goto _LL586;}_LL592: _tmp412=_tmp407.f2;
if((unsigned int)_tmp412 > 1?*((int*)_tmp412)== 0: 0){_LL593: _tmp413=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp412)->f1;if((int)_tmp413 == 0){goto _LL585;}
else{goto _LL586;}}else{goto _LL586;}_LL586: goto _LL587;_LL581: okay=1;goto _LL57F;
_LL583: okay=okay? Cyc_Evexp_eval_const_uint_exp(_tmp411)>= Cyc_Evexp_eval_const_uint_exp(
_tmp40E): 0;({void*_tmp416[0]={};Cyc_Tcutil_warn(loc,_tag_arr("implicit cast to shorter array",
sizeof(unsigned char),31),_tag_arr(_tmp416,sizeof(void*),0));});goto _LL57F;
_LL585: okay=1;goto _LL57F;_LL587: okay=0;goto _LL57F;_LL57F:;}okay=okay? Cyc_Tcutil_unify((
void*)_tmp3DD.elt_typ,(void*)_tmp3DB.elt_typ): 0;okay=okay? Cyc_Tcutil_unify((void*)
_tmp3DD.rgn_typ,(void*)_tmp3DB.rgn_typ)? 1: Cyc_Tcenv_region_outlives(te,(void*)
_tmp3DD.rgn_typ,(void*)_tmp3DB.rgn_typ): 0;okay=okay? !(_tmp3DD.tq).q_const? 1:(
_tmp3DB.tq).q_const: 0;return okay;}_LL549: {int okay;okay=(_tmp3E3 != 0? _tmp3DF != 0:
0)? Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp3E3))== 
Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp3DF)): 0;
return(okay? Cyc_Tcutil_unify(_tmp3E5,_tmp3E1): 0)? ! _tmp3E4.q_const? 1: _tmp3E0.q_const:
0;}_LL54B: if((_tmp3F1 == _tmp3EB? 1: Cyc_Absyn_qvar_cmp(_tmp3F1->name,_tmp3EB->name)
== 0)? _tmp3F0->typs == 0: 0){for(0;_tmp3EE != 0? _tmp3E8 != 0: 0;(_tmp3EE=_tmp3EE->tl,
_tmp3E8=_tmp3E8->tl)){if(! Cyc_Tcutil_unify((void*)_tmp3EE->hd,(void*)_tmp3E8->hd)){
break;}}if(_tmp3EE == 0? _tmp3E8 == 0: 0){return 1;}}return 0;_LL54D:{void*_tmp417=Cyc_Tcutil_compress(
_tmp3FF);struct Cyc_Absyn_TunionFieldInfo _tmp418;struct Cyc_List_List*_tmp419;void*
_tmp41A;struct Cyc_Absyn_Tunionfield*_tmp41B;struct Cyc_Absyn_Tuniondecl*_tmp41C;
_LL597: if((unsigned int)_tmp417 > 3?*((int*)_tmp417)== 3: 0){_LL59B: _tmp418=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp417)->f1;_LL59D: _tmp41A=(void*)
_tmp418.field_info;if(*((int*)_tmp41A)== 1){_LL59F: _tmp41C=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp41A)->f1;goto _LL59E;_LL59E: _tmp41B=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp41A)->f2;goto _LL59C;}else{goto _LL599;}_LL59C: _tmp419=_tmp418.targs;goto
_LL598;}else{goto _LL599;}_LL599: goto _LL59A;_LL598: if(! Cyc_Tcutil_unify(_tmp3FE,
_tmp3F4)? ! Cyc_Tcenv_region_outlives(te,_tmp3FE,_tmp3F4): 0){return 0;}if(!((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp3FD,((struct Cyc_Absyn_Conref*(*)(int x))Cyc_Absyn_new_conref)(
0))){return 0;}if(! Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp3FB,Cyc_Absyn_new_conref(
Cyc_Absyn_bounds_one))){return 0;}if(Cyc_Absyn_qvar_cmp(_tmp3F8->name,_tmp41C->name)
== 0? _tmp41B->typs != 0: 0){int okay=1;for(0;_tmp419 != 0? _tmp3F5 != 0: 0;(_tmp419=
_tmp419->tl,_tmp3F5=_tmp3F5->tl)){if(! Cyc_Tcutil_unify((void*)_tmp419->hd,(void*)
_tmp3F5->hd)){okay=0;break;}}if((! okay? 1: _tmp419 != 0)? 1: _tmp3F5 != 0){return 0;}
return 1;}goto _LL596;_LL59A: goto _LL596;_LL596:;}return 0;_LL54F: return 1;_LL551:
return Cyc_Tcutil_unify(t1,t2);_LL545:;}}int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp41D=Cyc_Tcutil_compress(t);_LL5A1: if((unsigned int)_tmp41D > 3?*((int*)
_tmp41D)== 4: 0){goto _LL5A2;}else{goto _LL5A3;}_LL5A3: goto _LL5A4;_LL5A2: return 1;
_LL5A4: return 0;_LL5A0:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){void*
_tmp41E=(void*)e->r;void*_tmp41F;int _tmp420;_LL5A6: if(*((int*)_tmp41E)== 0){
_LL5AA: _tmp41F=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp41E)->f1;if((
unsigned int)_tmp41F > 1?*((int*)_tmp41F)== 2: 0){_LL5AB: _tmp420=((struct Cyc_Absyn_Int_c_struct*)
_tmp41F)->f2;if(_tmp420 == 0){goto _LL5A7;}else{goto _LL5A8;}}else{goto _LL5A8;}}
else{goto _LL5A8;}_LL5A8: goto _LL5A9;_LL5A7: return 1;_LL5A9: return 0;_LL5A5:;}struct
Cyc_Core_Opt Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(
void*)((void*)0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt
Cyc_Tcutil_mk={(void*)((void*)1)};int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*
te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(t2)? Cyc_Tcutil_is_zero(
e1): 0){(void*)(e1->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmp421=
_cycalloc(sizeof(*_tmp421));_tmp421[0]=({struct Cyc_Absyn_Const_e_struct _tmp422;
_tmp422.tag=0;_tmp422.f1=(void*)((void*)0);_tmp422;});_tmp421;})));{struct Cyc_List_List*
_tmp423=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_PointerType_struct*
_tmp424=({struct Cyc_Absyn_PointerType_struct*_tmp425=_cycalloc(sizeof(*_tmp425));
_tmp425[0]=({struct Cyc_Absyn_PointerType_struct _tmp426;_tmp426.tag=4;_tmp426.f1=({
struct Cyc_Absyn_PtrInfo _tmp427;_tmp427.elt_typ=(void*)Cyc_Absyn_new_evar((struct
Cyc_Core_Opt*)& Cyc_Tcutil_ak,({struct Cyc_Core_Opt*_tmp429=_cycalloc(sizeof(*
_tmp429));_tmp429->v=_tmp423;_tmp429;}));_tmp427.rgn_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp428=_cycalloc(
sizeof(*_tmp428));_tmp428->v=_tmp423;_tmp428;}));_tmp427.nullable=((struct Cyc_Absyn_Conref*(*)(
int x))Cyc_Absyn_new_conref)(1);_tmp427.tq=Cyc_Absyn_empty_tqual();_tmp427.bounds=
Cyc_Absyn_empty_conref();_tmp427;});_tmp426;});_tmp425;});(void*)(((struct Cyc_Core_Opt*)
_check_null(e1->topt))->v=(void*)((void*)_tmp424));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}static int Cyc_Tcutil_is_sizeof_type(void*t1){void*_tmp42A=
Cyc_Tcutil_compress(t1);_LL5AD: if((unsigned int)_tmp42A > 3?*((int*)_tmp42A)== 14:
0){goto _LL5AE;}else{goto _LL5AF;}_LL5AF: goto _LL5B0;_LL5AE: return 1;_LL5B0: return 0;
_LL5AC:;}int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,
void*t2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
if(Cyc_Tcutil_unify(t1,t2)){return 1;}if((Cyc_Tcutil_is_arithmetic_type(t1)? Cyc_Tcutil_is_arithmetic_type(
t2): 0)? 1:(Cyc_Tcutil_is_sizeof_type(t1)? Cyc_Tcutil_is_arithmetic_type(t2): 0)){
if(Cyc_Tcutil_will_lose_precision(t1,t2)){({struct Cyc_Std_String_pa_struct
_tmp42D;_tmp42D.tag=0;_tmp42D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp42C;_tmp42C.tag=0;_tmp42C.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp42B[2]={& _tmp42C,& _tmp42D};Cyc_Tcutil_warn(
e->loc,_tag_arr("integral size mismatch; %s -> %s conversion supplied",sizeof(
unsigned char),53),_tag_arr(_tmp42B,sizeof(void*),2));}}});}Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(
te,e,t2);return 1;}else{if(Cyc_Tcutil_zero_to_null(te,t2,e)){return 1;}else{if(Cyc_Tcutil_castable(
te,e->loc,t1,t2)){Cyc_Tcutil_unchecked_cast(te,e,t2);({struct Cyc_Std_String_pa_struct
_tmp430;_tmp430.tag=0;_tmp430.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t2);{
struct Cyc_Std_String_pa_struct _tmp42F;_tmp42F.tag=0;_tmp42F.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t1);{void*_tmp42E[2]={& _tmp42F,& _tmp430};Cyc_Tcutil_warn(
e->loc,_tag_arr("implicit cast from %s to %s",sizeof(unsigned char),28),_tag_arr(
_tmp42E,sizeof(void*),2));}}});return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(
te,e,t);}int Cyc_Tcutil_coerceable(void*t){void*_tmp431=Cyc_Tcutil_compress(t);
_LL5B2: if((unsigned int)_tmp431 > 3?*((int*)_tmp431)== 5: 0){goto _LL5B3;}else{goto
_LL5B4;}_LL5B4: if((int)_tmp431 == 1){goto _LL5B5;}else{goto _LL5B6;}_LL5B6: if((
unsigned int)_tmp431 > 3?*((int*)_tmp431)== 6: 0){goto _LL5B7;}else{goto _LL5B8;}
_LL5B8: goto _LL5B9;_LL5B3: goto _LL5B5;_LL5B5: goto _LL5B7;_LL5B7: return 1;_LL5B9:
return 0;_LL5B1:;}static struct _tuple4*Cyc_Tcutil_flatten_typ_f(struct Cyc_List_List*
inst,struct Cyc_Absyn_Aggrfield*x){return({struct _tuple4*_tmp432=_cycalloc(
sizeof(*_tmp432));_tmp432->f1=x->tq;_tmp432->f2=Cyc_Tcutil_substitute(inst,(void*)
x->type);_tmp432;});}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct Cyc_Tcenv_Tenv*
te,void*t1){t1=Cyc_Tcutil_compress(t1);{void*_tmp433=t1;struct Cyc_List_List*
_tmp434;struct Cyc_Absyn_AggrInfo _tmp435;struct Cyc_List_List*_tmp436;void*_tmp437;
struct Cyc_Absyn_Aggrdecl**_tmp438;struct Cyc_Absyn_Aggrdecl*_tmp439;struct Cyc_List_List*
_tmp43A;void*_tmp43B;_LL5BB: if((int)_tmp433 == 0){goto _LL5BC;}else{goto _LL5BD;}
_LL5BD: if((unsigned int)_tmp433 > 3?*((int*)_tmp433)== 9: 0){_LL5C5: _tmp434=((
struct Cyc_Absyn_TupleType_struct*)_tmp433)->f1;goto _LL5BE;}else{goto _LL5BF;}
_LL5BF: if((unsigned int)_tmp433 > 3?*((int*)_tmp433)== 10: 0){_LL5C6: _tmp435=((
struct Cyc_Absyn_AggrType_struct*)_tmp433)->f1;_LL5C8: _tmp437=(void*)_tmp435.aggr_info;
if(*((int*)_tmp437)== 1){_LL5C9: _tmp438=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp437)->f1;_tmp439=*_tmp438;goto _LL5C7;}else{goto _LL5C1;}_LL5C7: _tmp436=
_tmp435.targs;goto _LL5C0;}else{goto _LL5C1;}_LL5C1: if((unsigned int)_tmp433 > 3?*((
int*)_tmp433)== 11: 0){_LL5CB: _tmp43B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp433)->f1;if((int)_tmp43B == 0){goto _LL5CA;}else{goto _LL5C3;}_LL5CA: _tmp43A=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp433)->f2;goto _LL5C2;}else{goto _LL5C3;}
_LL5C3: goto _LL5C4;_LL5BC: return 0;_LL5BE: return _tmp434;_LL5C0: if((void*)_tmp439->kind
== (void*)1? 1: _tmp439->fields == 0){return({struct Cyc_List_List*_tmp43C=_cycalloc(
sizeof(*_tmp43C));_tmp43C->hd=({struct _tuple4*_tmp43D=_cycalloc(sizeof(*_tmp43D));
_tmp43D->f1=Cyc_Absyn_empty_tqual();_tmp43D->f2=t1;_tmp43D;});_tmp43C->tl=0;
_tmp43C;});}{struct Cyc_List_List*_tmp43E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp439->tvs,_tmp436);return((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_flatten_typ_f,_tmp43E,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp439->fields))->v);}
_LL5C2: return((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcutil_flatten_typ_f,0,_tmp43A);_LL5C4: return({struct Cyc_List_List*_tmp43F=
_cycalloc(sizeof(*_tmp43F));_tmp43F->hd=({struct _tuple4*_tmp440=_cycalloc(
sizeof(*_tmp440));_tmp440->f1=Cyc_Absyn_empty_tqual();_tmp440->f2=t1;_tmp440;});
_tmp43F->tl=0;_tmp43F;});_LL5BA:;}}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(
struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2)){return 1;}{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple9*)a->hd)).f1)? Cyc_Tcutil_unify(t2,(*((struct _tuple9*)a->hd)).f2):
0){return 1;}}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct
_tuple9 _tmp442=({struct _tuple9 _tmp441;_tmp441.f1=t1;_tmp441.f2=t2;_tmp441;});
void*_tmp443;struct Cyc_Absyn_PtrInfo _tmp444;struct Cyc_Absyn_Conref*_tmp445;
struct Cyc_Absyn_Tqual _tmp446;struct Cyc_Absyn_Conref*_tmp447;void*_tmp448;void*
_tmp449;void*_tmp44A;struct Cyc_Absyn_PtrInfo _tmp44B;struct Cyc_Absyn_Conref*
_tmp44C;struct Cyc_Absyn_Tqual _tmp44D;struct Cyc_Absyn_Conref*_tmp44E;void*_tmp44F;
void*_tmp450;_LL5CD: _LL5D8: _tmp44A=_tmp442.f1;if((unsigned int)_tmp44A > 3?*((int*)
_tmp44A)== 4: 0){_LL5D9: _tmp44B=((struct Cyc_Absyn_PointerType_struct*)_tmp44A)->f1;
_LL5DE: _tmp450=(void*)_tmp44B.elt_typ;goto _LL5DD;_LL5DD: _tmp44F=(void*)_tmp44B.rgn_typ;
goto _LL5DC;_LL5DC: _tmp44E=_tmp44B.nullable;goto _LL5DB;_LL5DB: _tmp44D=_tmp44B.tq;
goto _LL5DA;_LL5DA: _tmp44C=_tmp44B.bounds;goto _LL5D1;}else{goto _LL5CF;}_LL5D1:
_tmp443=_tmp442.f2;if((unsigned int)_tmp443 > 3?*((int*)_tmp443)== 4: 0){_LL5D2:
_tmp444=((struct Cyc_Absyn_PointerType_struct*)_tmp443)->f1;_LL5D7: _tmp449=(void*)
_tmp444.elt_typ;goto _LL5D6;_LL5D6: _tmp448=(void*)_tmp444.rgn_typ;goto _LL5D5;
_LL5D5: _tmp447=_tmp444.nullable;goto _LL5D4;_LL5D4: _tmp446=_tmp444.tq;goto _LL5D3;
_LL5D3: _tmp445=_tmp444.bounds;goto _LL5CE;}else{goto _LL5CF;}_LL5CF: goto _LL5D0;
_LL5CE: if(_tmp44D.q_const? ! _tmp446.q_const: 0){return 0;}if((!((int(*)(int(*cmp)(
int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp44E,_tmp447)?((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp44E): 0)? !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp447): 0){
return 0;}if(! Cyc_Tcutil_unify(_tmp44F,_tmp448)? ! Cyc_Tcenv_region_outlives(te,
_tmp44F,_tmp448): 0){return 0;}if(! Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,
_tmp44C,_tmp445)){struct _tuple9 _tmp452=({struct _tuple9 _tmp451;_tmp451.f1=Cyc_Absyn_conref_val(
_tmp44C);_tmp451.f2=Cyc_Absyn_conref_val(_tmp445);_tmp451;});void*_tmp453;void*
_tmp454;void*_tmp455;struct Cyc_Absyn_Exp*_tmp456;void*_tmp457;struct Cyc_Absyn_Exp*
_tmp458;_LL5E0: _LL5E7: _tmp454=_tmp452.f1;if((unsigned int)_tmp454 > 1?*((int*)
_tmp454)== 0: 0){goto _LL5E6;}else{goto _LL5E2;}_LL5E6: _tmp453=_tmp452.f2;if((int)
_tmp453 == 0){goto _LL5E1;}else{goto _LL5E2;}_LL5E2: _LL5EA: _tmp457=_tmp452.f1;if((
unsigned int)_tmp457 > 1?*((int*)_tmp457)== 0: 0){_LL5EB: _tmp458=((struct Cyc_Absyn_Upper_b_struct*)
_tmp457)->f1;goto _LL5E8;}else{goto _LL5E4;}_LL5E8: _tmp455=_tmp452.f2;if((
unsigned int)_tmp455 > 1?*((int*)_tmp455)== 0: 0){_LL5E9: _tmp456=((struct Cyc_Absyn_Upper_b_struct*)
_tmp455)->f1;goto _LL5E3;}else{goto _LL5E4;}_LL5E4: goto _LL5E5;_LL5E1: goto _LL5DF;
_LL5E3: if(Cyc_Evexp_eval_const_uint_exp(_tmp458)< Cyc_Evexp_eval_const_uint_exp(
_tmp456)){return 0;}goto _LL5DF;_LL5E5: return 0;_LL5DF:;}return Cyc_Tcutil_ptrsubtype(
te,({struct Cyc_List_List*_tmp459=_cycalloc(sizeof(*_tmp459));_tmp459->hd=({
struct _tuple9*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->f1=t1;_tmp45A->f2=t2;
_tmp45A;});_tmp459->tl=assume;_tmp459;}),_tmp450,_tmp449);_LL5D0: return 0;_LL5CC:;}}
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){struct _tuple9 _tmp45C=({struct
_tuple9 _tmp45B;_tmp45B.f1=Cyc_Tcutil_compress(t1);_tmp45B.f2=Cyc_Tcutil_compress(
t2);_tmp45B;});void*_tmp45D;void*_tmp45E;void*_tmp45F;void*_tmp460;_LL5ED: _LL5F3:
_tmp45F=_tmp45C.f1;if((unsigned int)_tmp45F > 3?*((int*)_tmp45F)== 5: 0){_LL5F4:
_tmp460=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp45F)->f2;goto _LL5F1;}else{
goto _LL5EF;}_LL5F1: _tmp45D=_tmp45C.f2;if((unsigned int)_tmp45D > 3?*((int*)
_tmp45D)== 5: 0){_LL5F2: _tmp45E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp45D)->f2;
goto _LL5EE;}else{goto _LL5EF;}_LL5EF: goto _LL5F0;_LL5EE: return _tmp460 == _tmp45E;
_LL5F0: return 0;_LL5EC:;}static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2){struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(te,t2);for(0;tqs2 != 0;(
tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0){return 0;}{struct _tuple4 _tmp462;void*
_tmp463;struct Cyc_Absyn_Tqual _tmp464;struct _tuple4*_tmp461=(struct _tuple4*)tqs1->hd;
_tmp462=*_tmp461;_LL5F7: _tmp464=_tmp462.f1;goto _LL5F6;_LL5F6: _tmp463=_tmp462.f2;
goto _LL5F5;_LL5F5: {struct _tuple4 _tmp466;void*_tmp467;struct Cyc_Absyn_Tqual
_tmp468;struct _tuple4*_tmp465=(struct _tuple4*)tqs2->hd;_tmp466=*_tmp465;_LL5FA:
_tmp468=_tmp466.f1;goto _LL5F9;_LL5F9: _tmp467=_tmp466.f2;goto _LL5F8;_LL5F8: if(
_tmp468.q_const? Cyc_Tcutil_subtype(te,assume,_tmp463,_tmp467): 0){continue;}else{
if(Cyc_Tcutil_unify(_tmp463,_tmp467)){continue;}else{if(Cyc_Tcutil_isomorphic(
_tmp463,_tmp467)){continue;}else{return 0;}}}}}}return 1;}static int Cyc_Tcutil_is_char_type(
void*t){void*_tmp469=Cyc_Tcutil_compress(t);void*_tmp46A;_LL5FC: if((unsigned int)
_tmp469 > 3?*((int*)_tmp469)== 5: 0){_LL600: _tmp46A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp469)->f2;if((int)_tmp46A == 0){goto _LL5FD;}else{goto _LL5FE;}}else{goto _LL5FE;}
_LL5FE: goto _LL5FF;_LL5FD: return 1;_LL5FF: return 0;_LL5FB:;}int Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2)){return 1;}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{void*
_tmp46B=t2;void*_tmp46C;_LL602: if((unsigned int)_tmp46B > 3?*((int*)_tmp46B)== 5:
0){_LL606: _tmp46C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46B)->f2;if((int)
_tmp46C == 2){goto _LL603;}else{goto _LL604;}}else{goto _LL604;}_LL604: goto _LL605;
_LL603: if(Cyc_Tcutil_typ_kind(t2)== (void*)2){return 1;}goto _LL601;_LL605: goto
_LL601;_LL601:;}{void*_tmp46D=t1;struct Cyc_Absyn_PtrInfo _tmp46E;struct Cyc_Absyn_Conref*
_tmp46F;struct Cyc_Absyn_Tqual _tmp470;struct Cyc_Absyn_Conref*_tmp471;void*_tmp472;
void*_tmp473;struct Cyc_Absyn_Exp*_tmp474;struct Cyc_Absyn_Tqual _tmp475;void*
_tmp476;struct Cyc_Absyn_Enumdecl*_tmp477;_LL608: if((unsigned int)_tmp46D > 3?*((
int*)_tmp46D)== 4: 0){_LL618: _tmp46E=((struct Cyc_Absyn_PointerType_struct*)
_tmp46D)->f1;_LL61D: _tmp473=(void*)_tmp46E.elt_typ;goto _LL61C;_LL61C: _tmp472=(
void*)_tmp46E.rgn_typ;goto _LL61B;_LL61B: _tmp471=_tmp46E.nullable;goto _LL61A;
_LL61A: _tmp470=_tmp46E.tq;goto _LL619;_LL619: _tmp46F=_tmp46E.bounds;goto _LL609;}
else{goto _LL60A;}_LL60A: if((unsigned int)_tmp46D > 3?*((int*)_tmp46D)== 7: 0){
_LL620: _tmp476=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp46D)->f1;goto
_LL61F;_LL61F: _tmp475=((struct Cyc_Absyn_ArrayType_struct*)_tmp46D)->f2;goto
_LL61E;_LL61E: _tmp474=((struct Cyc_Absyn_ArrayType_struct*)_tmp46D)->f3;goto
_LL60B;}else{goto _LL60C;}_LL60C: if((unsigned int)_tmp46D > 3?*((int*)_tmp46D)== 
12: 0){_LL621: _tmp477=((struct Cyc_Absyn_EnumType_struct*)_tmp46D)->f2;goto _LL60D;}
else{goto _LL60E;}_LL60E: if((unsigned int)_tmp46D > 3?*((int*)_tmp46D)== 5: 0){goto
_LL60F;}else{goto _LL610;}_LL610: if((int)_tmp46D == 1){goto _LL611;}else{goto _LL612;}
_LL612: if((unsigned int)_tmp46D > 3?*((int*)_tmp46D)== 6: 0){goto _LL613;}else{goto
_LL614;}_LL614: if((int)_tmp46D == 0){goto _LL615;}else{goto _LL616;}_LL616: goto
_LL617;_LL609:{void*_tmp478=t2;struct Cyc_Absyn_PtrInfo _tmp479;struct Cyc_Absyn_Conref*
_tmp47A;struct Cyc_Absyn_Tqual _tmp47B;struct Cyc_Absyn_Conref*_tmp47C;void*_tmp47D;
void*_tmp47E;_LL623: if((unsigned int)_tmp478 > 3?*((int*)_tmp478)== 4: 0){_LL629:
_tmp479=((struct Cyc_Absyn_PointerType_struct*)_tmp478)->f1;_LL62E: _tmp47E=(void*)
_tmp479.elt_typ;goto _LL62D;_LL62D: _tmp47D=(void*)_tmp479.rgn_typ;goto _LL62C;
_LL62C: _tmp47C=_tmp479.nullable;goto _LL62B;_LL62B: _tmp47B=_tmp479.tq;goto _LL62A;
_LL62A: _tmp47A=_tmp479.bounds;goto _LL624;}else{goto _LL625;}_LL625: if((int)
_tmp478 == 0){goto _LL626;}else{goto _LL627;}_LL627: goto _LL628;_LL624: {struct Cyc_List_List*
_tmp47F=({struct Cyc_List_List*_tmp488=_cycalloc(sizeof(*_tmp488));_tmp488->hd=({
struct _tuple9*_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489->f1=t1;_tmp489->f2=t2;
_tmp489;});_tmp488->tl=0;_tmp488;});int _tmp480=Cyc_Tcutil_ptrsubtype(te,_tmp47F,
_tmp473,_tmp47E)? ! _tmp470.q_const? 1: _tmp47B.q_const: 0;int _tmp481=_tmp480? 0:((Cyc_Tcutil_bits_only(
_tmp473)? Cyc_Tcutil_is_char_type(_tmp47E): 0)? _tmp47B.q_const? 1: ! _tmp470.q_const:
0);int bounds_ok=Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_boundscmp,_tmp46F,_tmp47A);
if(! bounds_ok? ! _tmp481: 0){struct _tuple9 _tmp483=({struct _tuple9 _tmp482;_tmp482.f1=
Cyc_Absyn_conref_val(_tmp46F);_tmp482.f2=Cyc_Absyn_conref_val(_tmp47A);_tmp482;});
void*_tmp484;struct Cyc_Absyn_Exp*_tmp485;void*_tmp486;struct Cyc_Absyn_Exp*
_tmp487;_LL630: _LL636: _tmp486=_tmp483.f1;if((unsigned int)_tmp486 > 1?*((int*)
_tmp486)== 0: 0){_LL637: _tmp487=((struct Cyc_Absyn_Upper_b_struct*)_tmp486)->f1;
goto _LL634;}else{goto _LL632;}_LL634: _tmp484=_tmp483.f2;if((unsigned int)_tmp484 > 
1?*((int*)_tmp484)== 0: 0){_LL635: _tmp485=((struct Cyc_Absyn_Upper_b_struct*)
_tmp484)->f1;goto _LL631;}else{goto _LL632;}_LL632: goto _LL633;_LL631: if(Cyc_Evexp_eval_const_uint_exp(
_tmp487)>= Cyc_Evexp_eval_const_uint_exp(_tmp485)){bounds_ok=1;}goto _LL62F;
_LL633: bounds_ok=1;goto _LL62F;_LL62F:;}return(bounds_ok? _tmp480? 1: _tmp481: 0)? Cyc_Tcutil_unify(
_tmp472,_tmp47D)? 1: Cyc_Tcenv_region_outlives(te,_tmp472,_tmp47D): 0;}_LL626:
return 1;_LL628: goto _LL622;_LL622:;}return 0;_LL60B:{void*_tmp48A=t2;struct Cyc_Absyn_Exp*
_tmp48B;struct Cyc_Absyn_Tqual _tmp48C;void*_tmp48D;_LL639: if((unsigned int)
_tmp48A > 3?*((int*)_tmp48A)== 7: 0){_LL63F: _tmp48D=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp48A)->f1;goto _LL63E;_LL63E: _tmp48C=((struct Cyc_Absyn_ArrayType_struct*)
_tmp48A)->f2;goto _LL63D;_LL63D: _tmp48B=((struct Cyc_Absyn_ArrayType_struct*)
_tmp48A)->f3;goto _LL63A;}else{goto _LL63B;}_LL63B: goto _LL63C;_LL63A: {int okay;
okay=(_tmp474 != 0? _tmp48B != 0: 0)? Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp474))>= Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
_check_null(_tmp48B)): 0;return(okay? Cyc_Tcutil_unify(_tmp476,_tmp48D): 0)? !
_tmp475.q_const? 1: _tmp48C.q_const: 0;}_LL63C: return 0;_LL638:;}return 0;_LL60D:{
void*_tmp48E=t2;struct Cyc_Absyn_Enumdecl*_tmp48F;_LL641: if((unsigned int)_tmp48E
> 3?*((int*)_tmp48E)== 12: 0){_LL645: _tmp48F=((struct Cyc_Absyn_EnumType_struct*)
_tmp48E)->f2;goto _LL642;}else{goto _LL643;}_LL643: goto _LL644;_LL642: if((_tmp477->fields
!= 0? _tmp48F->fields != 0: 0)?((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp477->fields))->v)>= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp48F->fields))->v): 0){return 1;}goto _LL640;_LL644: goto _LL640;
_LL640:;}goto _LL60F;_LL60F: goto _LL611;_LL611: goto _LL613;_LL613: return Cyc_Tcutil_coerceable(
t2)? 1: t2 == (void*)0;_LL615: return t2 == (void*)0;_LL617: return 0;_LL607:;}}void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){if(! Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp490=Cyc_Absyn_copy_exp(
e);(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Cast_e_struct*_tmp491=_cycalloc(
sizeof(*_tmp491));_tmp491[0]=({struct Cyc_Absyn_Cast_e_struct _tmp492;_tmp492.tag=
13;_tmp492.f1=(void*)t;_tmp492.f2=_tmp490;_tmp492;});_tmp491;})));e->topt=({
struct Cyc_Core_Opt*_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493->v=(void*)t;
_tmp493;});}}int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*_tmp494=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL647: if((unsigned int)
_tmp494 > 3?*((int*)_tmp494)== 12: 0){goto _LL648;}else{goto _LL649;}_LL649: if((
unsigned int)_tmp494 > 3?*((int*)_tmp494)== 13: 0){goto _LL64A;}else{goto _LL64B;}
_LL64B: if((unsigned int)_tmp494 > 3?*((int*)_tmp494)== 5: 0){goto _LL64C;}else{goto
_LL64D;}_LL64D: if((unsigned int)_tmp494 > 3?*((int*)_tmp494)== 0: 0){goto _LL64E;}
else{goto _LL64F;}_LL64F: if((unsigned int)_tmp494 > 3?*((int*)_tmp494)== 14: 0){
goto _LL650;}else{goto _LL651;}_LL651: goto _LL652;_LL648: goto _LL64A;_LL64A: goto
_LL64C;_LL64C: return 1;_LL64E: return Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_t);_LL650: return 1;_LL652: return 0;_LL646:;}
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e)){
return 1;}{void*_tmp495=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL654: if((int)_tmp495 == 1){goto _LL655;}else{goto
_LL656;}_LL656: if((unsigned int)_tmp495 > 3?*((int*)_tmp495)== 6: 0){goto _LL657;}
else{goto _LL658;}_LL658: goto _LL659;_LL655: goto _LL657;_LL657: return 1;_LL659:
return 0;_LL653:;}}int Cyc_Tcutil_is_function_type(void*t){void*_tmp496=Cyc_Tcutil_compress(
t);_LL65B: if((unsigned int)_tmp496 > 3?*((int*)_tmp496)== 8: 0){goto _LL65C;}else{
goto _LL65D;}_LL65D: goto _LL65E;_LL65C: return 1;_LL65E: return 0;_LL65A:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2){struct _tuple9 _tmp498=({struct _tuple9 _tmp497;_tmp497.f1=t1;
_tmp497.f2=t2;_tmp497;});void*_tmp499;int _tmp49A;void*_tmp49B;int _tmp49C;void*
_tmp49D;void*_tmp49E;void*_tmp49F;void*_tmp4A0;void*_tmp4A1;void*_tmp4A2;void*
_tmp4A3;void*_tmp4A4;void*_tmp4A5;void*_tmp4A6;void*_tmp4A7;void*_tmp4A8;void*
_tmp4A9;void*_tmp4AA;void*_tmp4AB;void*_tmp4AC;void*_tmp4AD;void*_tmp4AE;void*
_tmp4AF;void*_tmp4B0;void*_tmp4B1;void*_tmp4B2;_LL660: _LL67A: _tmp49B=_tmp498.f1;
if((unsigned int)_tmp49B > 3?*((int*)_tmp49B)== 6: 0){_LL67B: _tmp49C=((struct Cyc_Absyn_DoubleType_struct*)
_tmp49B)->f1;goto _LL678;}else{goto _LL662;}_LL678: _tmp499=_tmp498.f2;if((
unsigned int)_tmp499 > 3?*((int*)_tmp499)== 6: 0){_LL679: _tmp49A=((struct Cyc_Absyn_DoubleType_struct*)
_tmp499)->f1;goto _LL661;}else{goto _LL662;}_LL662: _LL67C: _tmp49D=_tmp498.f1;if((
unsigned int)_tmp49D > 3?*((int*)_tmp49D)== 6: 0){goto _LL663;}else{goto _LL664;}
_LL664: _LL67D: _tmp49E=_tmp498.f2;if((unsigned int)_tmp49E > 3?*((int*)_tmp49E)== 
6: 0){goto _LL665;}else{goto _LL666;}_LL666: _LL67E: _tmp49F=_tmp498.f1;if((int)
_tmp49F == 1){goto _LL667;}else{goto _LL668;}_LL668: _LL67F: _tmp4A0=_tmp498.f2;if((
int)_tmp4A0 == 1){goto _LL669;}else{goto _LL66A;}_LL66A: _LL680: _tmp4A1=_tmp498.f1;
if((unsigned int)_tmp4A1 > 3?*((int*)_tmp4A1)== 5: 0){_LL682: _tmp4A3=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4A1)->f1;if((int)_tmp4A3 == 1){goto _LL681;}
else{goto _LL66C;}_LL681: _tmp4A2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A1)->f2;
if((int)_tmp4A2 == 3){goto _LL66B;}else{goto _LL66C;}}else{goto _LL66C;}_LL66C:
_LL683: _tmp4A4=_tmp498.f2;if((unsigned int)_tmp4A4 > 3?*((int*)_tmp4A4)== 5: 0){
_LL685: _tmp4A6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A4)->f1;if((int)
_tmp4A6 == 1){goto _LL684;}else{goto _LL66E;}_LL684: _tmp4A5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A4)->f2;if((int)_tmp4A5 == 3){goto _LL66D;}else{goto _LL66E;}}else{goto _LL66E;}
_LL66E: _LL686: _tmp4A7=_tmp498.f1;if((unsigned int)_tmp4A7 > 3?*((int*)_tmp4A7)== 
5: 0){_LL688: _tmp4A9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A7)->f1;if((
int)_tmp4A9 == 0){goto _LL687;}else{goto _LL670;}_LL687: _tmp4A8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A7)->f2;if((int)_tmp4A8 == 3){goto _LL66F;}else{goto _LL670;}}else{goto _LL670;}
_LL670: _LL689: _tmp4AA=_tmp498.f2;if((unsigned int)_tmp4AA > 3?*((int*)_tmp4AA)== 
5: 0){_LL68B: _tmp4AC=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AA)->f1;if((
int)_tmp4AC == 0){goto _LL68A;}else{goto _LL672;}_LL68A: _tmp4AB=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4AA)->f2;if((int)_tmp4AB == 3){goto _LL671;}else{goto _LL672;}}else{goto _LL672;}
_LL672: _LL68C: _tmp4AD=_tmp498.f1;if((unsigned int)_tmp4AD > 3?*((int*)_tmp4AD)== 
5: 0){_LL68E: _tmp4AF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AD)->f1;if((
int)_tmp4AF == 1){goto _LL68D;}else{goto _LL674;}_LL68D: _tmp4AE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4AD)->f2;if((int)_tmp4AE == 2){goto _LL673;}else{goto _LL674;}}else{goto _LL674;}
_LL674: _LL68F: _tmp4B0=_tmp498.f2;if((unsigned int)_tmp4B0 > 3?*((int*)_tmp4B0)== 
5: 0){_LL691: _tmp4B2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B0)->f1;if((
int)_tmp4B2 == 1){goto _LL690;}else{goto _LL676;}_LL690: _tmp4B1=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B0)->f2;if((int)_tmp4B1 == 2){goto _LL675;}else{goto _LL676;}}else{goto _LL676;}
_LL676: goto _LL677;_LL661: if(_tmp49C){return t1;}else{return t2;}_LL663: return t1;
_LL665: return t2;_LL667: goto _LL669;_LL669: return(void*)1;_LL66B: goto _LL66D;_LL66D:
return Cyc_Absyn_ulonglong_t;_LL66F: goto _LL671;_LL671: return Cyc_Absyn_slonglong_t;
_LL673: goto _LL675;_LL675: return Cyc_Absyn_uint_t;_LL677: return Cyc_Absyn_sint_t;
_LL65F:;}void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){void*
_tmp4B3=(void*)e->r;struct Cyc_Core_Opt*_tmp4B4;_LL693: if(*((int*)_tmp4B3)== 4){
_LL697: _tmp4B4=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp4B3)->f2;if(_tmp4B4 == 0){
goto _LL694;}else{goto _LL695;}}else{goto _LL695;}_LL695: goto _LL696;_LL694:({void*
_tmp4B5[0]={};Cyc_Tcutil_warn(e->loc,_tag_arr("assignment in test",sizeof(
unsigned char),19),_tag_arr(_tmp4B5,sizeof(void*),0));});goto _LL692;_LL696: goto
_LL692;_LL692:;}static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple9 _tmp4B7=({struct _tuple9 _tmp4B6;
_tmp4B6.f1=c1;_tmp4B6.f2=c2;_tmp4B6;});void*_tmp4B8;void*_tmp4B9;void*_tmp4BA;
void*_tmp4BB;void*_tmp4BC;struct Cyc_Core_Opt*_tmp4BD;struct Cyc_Core_Opt**_tmp4BE;
void*_tmp4BF;struct Cyc_Core_Opt*_tmp4C0;struct Cyc_Core_Opt**_tmp4C1;void*_tmp4C2;
void*_tmp4C3;void*_tmp4C4;void*_tmp4C5;struct Cyc_Core_Opt*_tmp4C6;struct Cyc_Core_Opt**
_tmp4C7;void*_tmp4C8;void*_tmp4C9;struct Cyc_Core_Opt*_tmp4CA;struct Cyc_Core_Opt**
_tmp4CB;void*_tmp4CC;void*_tmp4CD;void*_tmp4CE;void*_tmp4CF;struct Cyc_Core_Opt*
_tmp4D0;struct Cyc_Core_Opt**_tmp4D1;void*_tmp4D2;void*_tmp4D3;struct Cyc_Core_Opt*
_tmp4D4;struct Cyc_Core_Opt**_tmp4D5;_LL699: _LL6A7: _tmp4BA=_tmp4B7.f1;if(*((int*)
_tmp4BA)== 0){_LL6A8: _tmp4BB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4BA)->f1;
goto _LL6A5;}else{goto _LL69B;}_LL6A5: _tmp4B8=_tmp4B7.f2;if(*((int*)_tmp4B8)== 0){
_LL6A6: _tmp4B9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4B8)->f1;goto _LL69A;}
else{goto _LL69B;}_LL69B: _LL6A9: _tmp4BC=_tmp4B7.f2;if(*((int*)_tmp4BC)== 1){
_LL6AA: _tmp4BD=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4BC)->f1;_tmp4BE=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4BC)->f1;goto _LL69C;}
else{goto _LL69D;}_LL69D: _LL6AB: _tmp4BF=_tmp4B7.f1;if(*((int*)_tmp4BF)== 1){
_LL6AC: _tmp4C0=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4BF)->f1;_tmp4C1=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp4BF)->f1;goto _LL69E;}
else{goto _LL69F;}_LL69F: _LL6AF: _tmp4C4=_tmp4B7.f1;if(*((int*)_tmp4C4)== 2){
_LL6B1: _tmp4C6=((struct Cyc_Absyn_Less_kb_struct*)_tmp4C4)->f1;_tmp4C7=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4C4)->f1;goto _LL6B0;_LL6B0: _tmp4C5=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp4C4)->f2;goto _LL6AD;}else{goto _LL6A1;}_LL6AD:
_tmp4C2=_tmp4B7.f2;if(*((int*)_tmp4C2)== 0){_LL6AE: _tmp4C3=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp4C2)->f1;goto _LL6A0;}else{goto _LL6A1;}_LL6A1: _LL6B5: _tmp4CC=_tmp4B7.f1;if(*((
int*)_tmp4CC)== 0){_LL6B6: _tmp4CD=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4CC)->f1;
goto _LL6B2;}else{goto _LL6A3;}_LL6B2: _tmp4C8=_tmp4B7.f2;if(*((int*)_tmp4C8)== 2){
_LL6B4: _tmp4CA=((struct Cyc_Absyn_Less_kb_struct*)_tmp4C8)->f1;_tmp4CB=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp4C8)->f1;goto _LL6B3;_LL6B3: _tmp4C9=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp4C8)->f2;goto _LL6A2;}else{goto _LL6A3;}_LL6A3:
_LL6BA: _tmp4D2=_tmp4B7.f1;if(*((int*)_tmp4D2)== 2){_LL6BC: _tmp4D4=((struct Cyc_Absyn_Less_kb_struct*)
_tmp4D2)->f1;_tmp4D5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp4D2)->f1;goto _LL6BB;_LL6BB: _tmp4D3=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp4D2)->f2;goto _LL6B7;}else{goto _LL698;}_LL6B7: _tmp4CE=_tmp4B7.f2;if(*((int*)
_tmp4CE)== 2){_LL6B9: _tmp4D0=((struct Cyc_Absyn_Less_kb_struct*)_tmp4CE)->f1;
_tmp4D1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp4CE)->f1;
goto _LL6B8;_LL6B8: _tmp4CF=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4CE)->f2;
goto _LL6A4;}else{goto _LL698;}_LL69A: return _tmp4BB == _tmp4B9;_LL69C:*_tmp4BE=({
struct Cyc_Core_Opt*_tmp4D6=_cycalloc(sizeof(*_tmp4D6));_tmp4D6->v=(void*)c1;
_tmp4D6;});return 1;_LL69E:*_tmp4C1=({struct Cyc_Core_Opt*_tmp4D7=_cycalloc(
sizeof(*_tmp4D7));_tmp4D7->v=(void*)c2;_tmp4D7;});return 1;_LL6A0: if(Cyc_Tcutil_kind_leq(
_tmp4C3,_tmp4C5)){*_tmp4C7=({struct Cyc_Core_Opt*_tmp4D8=_cycalloc(sizeof(*
_tmp4D8));_tmp4D8->v=(void*)c2;_tmp4D8;});return 1;}else{return 0;}_LL6A2: if(Cyc_Tcutil_kind_leq(
_tmp4CD,_tmp4C9)){*_tmp4CB=({struct Cyc_Core_Opt*_tmp4D9=_cycalloc(sizeof(*
_tmp4D9));_tmp4D9->v=(void*)c1;_tmp4D9;});return 1;}else{return 0;}_LL6A4: if(Cyc_Tcutil_kind_leq(
_tmp4D3,_tmp4CF)){*_tmp4D1=({struct Cyc_Core_Opt*_tmp4DA=_cycalloc(sizeof(*
_tmp4DA));_tmp4DA->v=(void*)c1;_tmp4DA;});return 1;}else{if(Cyc_Tcutil_kind_leq(
_tmp4CF,_tmp4D3)){*_tmp4D5=({struct Cyc_Core_Opt*_tmp4DB=_cycalloc(sizeof(*
_tmp4DB));_tmp4DB->v=(void*)c2;_tmp4DB;});return 1;}else{return 0;}}_LL698:;}}
static int Cyc_Tcutil_tvar_id_counter=0;int*Cyc_Tcutil_new_tvar_id(){return({int*
_tmp4DC=_cycalloc_atomic(sizeof(*_tmp4DC));_tmp4DC[0]=Cyc_Tcutil_tvar_id_counter
++;_tmp4DC;});}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k){int i=Cyc_Tcutil_tvar_counter ++;struct _tagged_arr s=(struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp4E0;_tmp4E0.tag=1;_tmp4E0.f1=(int)((unsigned int)
i);{void*_tmp4DF[1]={& _tmp4E0};Cyc_Std_aprintf(_tag_arr("#%d",sizeof(
unsigned char),4),_tag_arr(_tmp4DF,sizeof(void*),1));}});return({struct Cyc_Absyn_Tvar*
_tmp4DD=_cycalloc(sizeof(*_tmp4DD));_tmp4DD->name=({struct _tagged_arr*_tmp4DE=
_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp4DE[0]=s;_tmp4DE;});_tmp4DD->identity=
0;_tmp4DD->kind=(void*)k;_tmp4DD;});}int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*
t){struct _tagged_arr _tmp4E1=*t->name;return*((const unsigned char*)
_check_unknown_subscript(_tmp4E1,sizeof(unsigned char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t){({struct Cyc_Std_String_pa_struct _tmp4E3;_tmp4E3.tag=0;
_tmp4E3.f1=(struct _tagged_arr)*t->name;{void*_tmp4E2[1]={& _tmp4E3};Cyc_Std_printf(
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp4E2,sizeof(void*),1));}});
if(! Cyc_Tcutil_is_temp_tvar(t)){return;}{struct _tagged_arr _tmp4E4=Cyc_Std_strconcat(
_tag_arr("`",sizeof(unsigned char),2),*t->name);*((unsigned char*)
_check_unknown_subscript(_tmp4E4,sizeof(unsigned char),1))='t';t->name=({struct
_tagged_arr*_tmp4E5=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp4E5[0]=(struct
_tagged_arr)_tmp4E4;_tmp4E5;});}}struct _tuple14{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static struct _tuple2*Cyc_Tcutil_fndecl2typ_f(struct _tuple14*x){
return({struct _tuple2*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));_tmp4E6->f1=(struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7->v=(*x).f1;
_tmp4E7;});_tmp4E6->f2=(*x).f2;_tmp4E6->f3=(*x).f3;_tmp4E6;});}void*Cyc_Tcutil_fndecl2typ(
struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){struct Cyc_List_List*_tmp4E8=0;{
struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((
void*)atts->hd)){_tmp4E8=({struct Cyc_List_List*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));
_tmp4E9->hd=(void*)((void*)atts->hd);_tmp4E9->tl=_tmp4E8;_tmp4E9;});}}}return(
void*)({struct Cyc_Absyn_FnType_struct*_tmp4EA=_cycalloc(sizeof(*_tmp4EA));
_tmp4EA[0]=({struct Cyc_Absyn_FnType_struct _tmp4EB;_tmp4EB.tag=8;_tmp4EB.f1=({
struct Cyc_Absyn_FnInfo _tmp4EC;_tmp4EC.tvars=fd->tvs;_tmp4EC.effect=fd->effect;
_tmp4EC.ret_typ=(void*)((void*)fd->ret_type);_tmp4EC.args=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args);_tmp4EC.c_varargs=fd->c_varargs;_tmp4EC.cyc_varargs=fd->cyc_varargs;
_tmp4EC.rgn_po=fd->rgn_po;_tmp4EC.attributes=_tmp4E8;_tmp4EC;});_tmp4EB;});
_tmp4EA;});}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple15{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple15*t){return(*t).f1;}void*Cyc_Tcutil_snd_tqt(struct _tuple4*t){return(*
t).f2;}static struct _tuple4*Cyc_Tcutil_map2_tq(struct _tuple4*pr,void*t){return({
struct _tuple4*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED->f1=(*pr).f1;_tmp4ED->f2=
t;_tmp4ED;});}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;};
struct _tuple17{struct _tuple16*f1;void*f2;};static struct _tuple17*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple2*y){return({struct _tuple17*_tmp4EE=
_region_malloc(rgn,sizeof(*_tmp4EE));_tmp4EE->f1=({struct _tuple16*_tmp4EF=
_region_malloc(rgn,sizeof(*_tmp4EF));_tmp4EF->f1=(*y).f1;_tmp4EF->f2=(*y).f2;
_tmp4EF;});_tmp4EE->f2=(*y).f3;_tmp4EE;});}static struct _tuple2*Cyc_Tcutil_substitute_f2(
struct _tuple17*w){void*_tmp4F1;struct _tuple16*_tmp4F2;struct _tuple17 _tmp4F0=*w;
_LL6BF: _tmp4F2=_tmp4F0.f1;goto _LL6BE;_LL6BE: _tmp4F1=_tmp4F0.f2;goto _LL6BD;_LL6BD: {
struct Cyc_Absyn_Tqual _tmp4F4;struct Cyc_Core_Opt*_tmp4F5;struct _tuple16 _tmp4F3=*
_tmp4F2;_LL6C2: _tmp4F5=_tmp4F3.f1;goto _LL6C1;_LL6C1: _tmp4F4=_tmp4F3.f2;goto
_LL6C0;_LL6C0: return({struct _tuple2*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->f1=
_tmp4F5;_tmp4F6->f2=_tmp4F4;_tmp4F6->f3=_tmp4F1;_tmp4F6;});}}static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*
Cyc_Tcutil_zip_field_type(struct Cyc_Absyn_Aggrfield*f,void*t){return({struct Cyc_Absyn_Aggrfield*
_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->name=f->name;_tmp4F7->tq=f->tq;
_tmp4F7->type=(void*)t;_tmp4F7->width=f->width;_tmp4F7->attributes=f->attributes;
_tmp4F7;});}static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,
struct Cyc_List_List*inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){void*_tmp4F8=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp4F9;struct Cyc_Absyn_AggrInfo _tmp4FA;struct Cyc_List_List*
_tmp4FB;void*_tmp4FC;struct Cyc_Absyn_TunionInfo _tmp4FD;void*_tmp4FE;struct Cyc_List_List*
_tmp4FF;void*_tmp500;struct Cyc_Absyn_TunionFieldInfo _tmp501;struct Cyc_List_List*
_tmp502;void*_tmp503;struct Cyc_Core_Opt*_tmp504;struct Cyc_List_List*_tmp505;
struct _tuple1*_tmp506;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Tqual _tmp508;
void*_tmp509;struct Cyc_Absyn_PtrInfo _tmp50A;struct Cyc_Absyn_Conref*_tmp50B;
struct Cyc_Absyn_Tqual _tmp50C;struct Cyc_Absyn_Conref*_tmp50D;void*_tmp50E;void*
_tmp50F;struct Cyc_Absyn_FnInfo _tmp510;struct Cyc_List_List*_tmp511;struct Cyc_List_List*
_tmp512;struct Cyc_Absyn_VarargInfo*_tmp513;int _tmp514;struct Cyc_List_List*
_tmp515;void*_tmp516;struct Cyc_Core_Opt*_tmp517;struct Cyc_List_List*_tmp518;
struct Cyc_List_List*_tmp519;struct Cyc_List_List*_tmp51A;void*_tmp51B;struct Cyc_Core_Opt*
_tmp51C;void*_tmp51D;void*_tmp51E;void*_tmp51F;void*_tmp520;struct Cyc_List_List*
_tmp521;_LL6C4: if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)== 1: 0){_LL6F2:
_tmp4F9=((struct Cyc_Absyn_VarType_struct*)_tmp4F8)->f1;goto _LL6C5;}else{goto
_LL6C6;}_LL6C6: if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)== 10: 0){_LL6F3:
_tmp4FA=((struct Cyc_Absyn_AggrType_struct*)_tmp4F8)->f1;_LL6F5: _tmp4FC=(void*)
_tmp4FA.aggr_info;goto _LL6F4;_LL6F4: _tmp4FB=_tmp4FA.targs;goto _LL6C7;}else{goto
_LL6C8;}_LL6C8: if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)== 2: 0){_LL6F6:
_tmp4FD=((struct Cyc_Absyn_TunionType_struct*)_tmp4F8)->f1;_LL6F9: _tmp500=(void*)
_tmp4FD.tunion_info;goto _LL6F8;_LL6F8: _tmp4FF=_tmp4FD.targs;goto _LL6F7;_LL6F7:
_tmp4FE=(void*)_tmp4FD.rgn;goto _LL6C9;}else{goto _LL6CA;}_LL6CA: if((unsigned int)
_tmp4F8 > 3?*((int*)_tmp4F8)== 3: 0){_LL6FA: _tmp501=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp4F8)->f1;_LL6FC: _tmp503=(void*)_tmp501.field_info;goto _LL6FB;_LL6FB: _tmp502=
_tmp501.targs;goto _LL6CB;}else{goto _LL6CC;}_LL6CC: if((unsigned int)_tmp4F8 > 3?*((
int*)_tmp4F8)== 16: 0){_LL6FF: _tmp506=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4F8)->f1;goto _LL6FE;_LL6FE: _tmp505=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4F8)->f2;goto _LL6FD;_LL6FD: _tmp504=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4F8)->f3;goto _LL6CD;}else{goto _LL6CE;}_LL6CE: if((unsigned int)_tmp4F8 > 3?*((
int*)_tmp4F8)== 7: 0){_LL702: _tmp509=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp4F8)->f1;goto _LL701;_LL701: _tmp508=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4F8)->f2;goto _LL700;_LL700: _tmp507=((struct Cyc_Absyn_ArrayType_struct*)
_tmp4F8)->f3;goto _LL6CF;}else{goto _LL6D0;}_LL6D0: if((unsigned int)_tmp4F8 > 3?*((
int*)_tmp4F8)== 4: 0){_LL703: _tmp50A=((struct Cyc_Absyn_PointerType_struct*)
_tmp4F8)->f1;_LL708: _tmp50F=(void*)_tmp50A.elt_typ;goto _LL707;_LL707: _tmp50E=(
void*)_tmp50A.rgn_typ;goto _LL706;_LL706: _tmp50D=_tmp50A.nullable;goto _LL705;
_LL705: _tmp50C=_tmp50A.tq;goto _LL704;_LL704: _tmp50B=_tmp50A.bounds;goto _LL6D1;}
else{goto _LL6D2;}_LL6D2: if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)== 8: 0){
_LL709: _tmp510=((struct Cyc_Absyn_FnType_struct*)_tmp4F8)->f1;_LL711: _tmp518=
_tmp510.tvars;goto _LL710;_LL710: _tmp517=_tmp510.effect;goto _LL70F;_LL70F: _tmp516=(
void*)_tmp510.ret_typ;goto _LL70E;_LL70E: _tmp515=_tmp510.args;goto _LL70D;_LL70D:
_tmp514=_tmp510.c_varargs;goto _LL70C;_LL70C: _tmp513=_tmp510.cyc_varargs;goto
_LL70B;_LL70B: _tmp512=_tmp510.rgn_po;goto _LL70A;_LL70A: _tmp511=_tmp510.attributes;
goto _LL6D3;}else{goto _LL6D4;}_LL6D4: if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)
== 9: 0){_LL712: _tmp519=((struct Cyc_Absyn_TupleType_struct*)_tmp4F8)->f1;goto
_LL6D5;}else{goto _LL6D6;}_LL6D6: if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)== 
11: 0){_LL714: _tmp51B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp4F8)->f1;
goto _LL713;_LL713: _tmp51A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp4F8)->f2;
goto _LL6D7;}else{goto _LL6D8;}_LL6D8: if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)
== 0: 0){_LL715: _tmp51C=((struct Cyc_Absyn_Evar_struct*)_tmp4F8)->f2;goto _LL6D9;}
else{goto _LL6DA;}_LL6DA: if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)== 15: 0){
_LL716: _tmp51D=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp4F8)->f1;goto
_LL6DB;}else{goto _LL6DC;}_LL6DC: if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)== 
14: 0){_LL717: _tmp51E=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp4F8)->f1;
goto _LL6DD;}else{goto _LL6DE;}_LL6DE: if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)
== 12: 0){goto _LL6DF;}else{goto _LL6E0;}_LL6E0: if((unsigned int)_tmp4F8 > 3?*((int*)
_tmp4F8)== 13: 0){goto _LL6E1;}else{goto _LL6E2;}_LL6E2: if((int)_tmp4F8 == 0){goto
_LL6E3;}else{goto _LL6E4;}_LL6E4: if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)== 5:
0){goto _LL6E5;}else{goto _LL6E6;}_LL6E6: if((int)_tmp4F8 == 1){goto _LL6E7;}else{
goto _LL6E8;}_LL6E8: if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)== 6: 0){goto
_LL6E9;}else{goto _LL6EA;}_LL6EA: if((int)_tmp4F8 == 2){goto _LL6EB;}else{goto _LL6EC;}
_LL6EC: if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)== 19: 0){_LL718: _tmp51F=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp4F8)->f1;goto _LL6ED;}else{goto _LL6EE;}_LL6EE:
if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)== 17: 0){_LL719: _tmp520=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp4F8)->f1;goto _LL6EF;}else{goto _LL6F0;}
_LL6F0: if((unsigned int)_tmp4F8 > 3?*((int*)_tmp4F8)== 18: 0){_LL71A: _tmp521=((
struct Cyc_Absyn_JoinEff_struct*)_tmp4F8)->f1;goto _LL6F1;}else{goto _LL6C3;}_LL6C5: {
struct _handler_cons _tmp522;_push_handler(& _tmp522);{int _tmp524=0;if(setjmp(
_tmp522.handler)){_tmp524=1;}if(! _tmp524){{void*_tmp525=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp4F9);_npop_handler(0);return
_tmp525;};_pop_handler();}else{void*_tmp523=(void*)_exn_thrown;void*_tmp527=
_tmp523;_LL71C: if(_tmp527 == Cyc_Core_Not_found){goto _LL71D;}else{goto _LL71E;}
_LL71E: goto _LL71F;_LL71D: return t;_LL71F:(void)_throw(_tmp527);_LL71B:;}}}_LL6C7: {
struct Cyc_List_List*_tmp528=Cyc_Tcutil_substs(rgn,inst,_tmp4FB);return _tmp528 == 
_tmp4FB? t:(void*)({struct Cyc_Absyn_AggrType_struct*_tmp529=_cycalloc(sizeof(*
_tmp529));_tmp529[0]=({struct Cyc_Absyn_AggrType_struct _tmp52A;_tmp52A.tag=10;
_tmp52A.f1=({struct Cyc_Absyn_AggrInfo _tmp52B;_tmp52B.aggr_info=(void*)_tmp4FC;
_tmp52B.targs=_tmp528;_tmp52B;});_tmp52A;});_tmp529;});}_LL6C9: {struct Cyc_List_List*
_tmp52C=Cyc_Tcutil_substs(rgn,inst,_tmp4FF);void*_tmp52D=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp4FE);return(_tmp52C == _tmp4FF? _tmp52D == _tmp4FE: 0)? t:(void*)({
struct Cyc_Absyn_TunionType_struct*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E[0]=({
struct Cyc_Absyn_TunionType_struct _tmp52F;_tmp52F.tag=2;_tmp52F.f1=({struct Cyc_Absyn_TunionInfo
_tmp530;_tmp530.tunion_info=(void*)_tmp500;_tmp530.targs=_tmp52C;_tmp530.rgn=(
void*)_tmp52D;_tmp530;});_tmp52F;});_tmp52E;});}_LL6CB: {struct Cyc_List_List*
_tmp531=Cyc_Tcutil_substs(rgn,inst,_tmp502);return _tmp531 == _tmp502? t:(void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp532=_cycalloc(sizeof(*_tmp532));
_tmp532[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp533;_tmp533.tag=3;_tmp533.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp534;_tmp534.field_info=(void*)_tmp503;_tmp534.targs=
_tmp531;_tmp534;});_tmp533;});_tmp532;});}_LL6CD: {struct Cyc_List_List*_tmp535=
Cyc_Tcutil_substs(rgn,inst,_tmp505);return _tmp535 == _tmp505? t:(void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp536=_cycalloc(sizeof(*_tmp536));_tmp536[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp537;_tmp537.tag=16;_tmp537.f1=_tmp506;_tmp537.f2=_tmp535;_tmp537.f3=_tmp504;
_tmp537;});_tmp536;});}_LL6CF: {void*_tmp538=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp509);return _tmp538 == _tmp509? t:(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp539=_cycalloc(sizeof(*_tmp539));_tmp539[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp53A;_tmp53A.tag=7;_tmp53A.f1=(void*)_tmp538;_tmp53A.f2=_tmp508;_tmp53A.f3=
_tmp507;_tmp53A;});_tmp539;});}_LL6D1: {void*_tmp53B=Cyc_Tcutil_rsubstitute(rgn,
inst,_tmp50F);void*_tmp53C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp50E);if(_tmp53B == 
_tmp50F? _tmp53C == _tmp50E: 0){return t;}return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D[0]=({struct Cyc_Absyn_PointerType_struct
_tmp53E;_tmp53E.tag=4;_tmp53E.f1=({struct Cyc_Absyn_PtrInfo _tmp53F;_tmp53F.elt_typ=(
void*)_tmp53B;_tmp53F.rgn_typ=(void*)_tmp53C;_tmp53F.nullable=_tmp50D;_tmp53F.tq=
_tmp50C;_tmp53F.bounds=_tmp50B;_tmp53F;});_tmp53E;});_tmp53D;});}_LL6D3:{struct
Cyc_List_List*_tmp540=_tmp518;for(0;_tmp540 != 0;_tmp540=_tmp540->tl){inst=({
struct Cyc_List_List*_tmp541=_region_malloc(rgn,sizeof(*_tmp541));_tmp541->hd=({
struct _tuple6*_tmp542=_region_malloc(rgn,sizeof(*_tmp542));_tmp542->f1=(struct
Cyc_Absyn_Tvar*)_tmp540->hd;_tmp542->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543[0]=({struct Cyc_Absyn_VarType_struct
_tmp544;_tmp544.tag=1;_tmp544.f1=(struct Cyc_Absyn_Tvar*)_tmp540->hd;_tmp544;});
_tmp543;});_tmp542;});_tmp541->tl=inst;_tmp541;});}}{struct Cyc_List_List*_tmp546;
struct Cyc_List_List*_tmp547;struct _tuple0 _tmp545=((struct _tuple0(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(
rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple17*(*f)(
struct _RegionHandle*,struct _tuple2*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp515));_LL722: _tmp547=
_tmp545.f1;goto _LL721;_LL721: _tmp546=_tmp545.f2;goto _LL720;_LL720: {struct Cyc_List_List*
_tmp548=Cyc_Tcutil_substs(rgn,inst,_tmp546);struct Cyc_List_List*_tmp549=((struct
Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcutil_substitute_f2,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,
rgn,_tmp547,_tmp548));struct Cyc_Core_Opt*eff2;if(_tmp517 == 0){eff2=0;}else{void*
_tmp54A=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmp517->v);if(_tmp54A == (void*)
_tmp517->v){eff2=_tmp517;}else{eff2=({struct Cyc_Core_Opt*_tmp54B=_cycalloc(
sizeof(*_tmp54B));_tmp54B->v=(void*)_tmp54A;_tmp54B;});}}{struct Cyc_Absyn_VarargInfo*
cyc_varargs2;if(_tmp513 == 0){cyc_varargs2=0;}else{int _tmp54D;void*_tmp54E;struct
Cyc_Absyn_Tqual _tmp54F;struct Cyc_Core_Opt*_tmp550;struct Cyc_Absyn_VarargInfo
_tmp54C=*_tmp513;_LL727: _tmp550=_tmp54C.name;goto _LL726;_LL726: _tmp54F=_tmp54C.tq;
goto _LL725;_LL725: _tmp54E=(void*)_tmp54C.type;goto _LL724;_LL724: _tmp54D=_tmp54C.inject;
goto _LL723;_LL723: {void*_tmp551=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp54E);if(
_tmp551 == _tmp54E){cyc_varargs2=_tmp513;}else{cyc_varargs2=({struct Cyc_Absyn_VarargInfo*
_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->name=_tmp550;_tmp552->tq=_tmp54F;
_tmp552->type=(void*)_tmp551;_tmp552->inject=_tmp54D;_tmp552;});}}}{struct Cyc_List_List*
rgn_po2;struct Cyc_List_List*_tmp554;struct Cyc_List_List*_tmp555;struct _tuple0
_tmp553=Cyc_List_rsplit(rgn,rgn,_tmp512);_LL72A: _tmp555=_tmp553.f1;goto _LL729;
_LL729: _tmp554=_tmp553.f2;goto _LL728;_LL728: {struct Cyc_List_List*_tmp556=Cyc_Tcutil_substs(
rgn,inst,_tmp555);struct Cyc_List_List*_tmp557=Cyc_Tcutil_substs(rgn,inst,_tmp554);
if(_tmp556 == _tmp555? _tmp557 == _tmp554: 0){rgn_po2=_tmp512;}else{rgn_po2=Cyc_List_zip(
_tmp556,_tmp557);}return(void*)({struct Cyc_Absyn_FnType_struct*_tmp558=_cycalloc(
sizeof(*_tmp558));_tmp558[0]=({struct Cyc_Absyn_FnType_struct _tmp559;_tmp559.tag=
8;_tmp559.f1=({struct Cyc_Absyn_FnInfo _tmp55A;_tmp55A.tvars=_tmp518;_tmp55A.effect=
eff2;_tmp55A.ret_typ=(void*)Cyc_Tcutil_rsubstitute(rgn,inst,_tmp516);_tmp55A.args=
_tmp549;_tmp55A.c_varargs=_tmp514;_tmp55A.cyc_varargs=cyc_varargs2;_tmp55A.rgn_po=
rgn_po2;_tmp55A.attributes=_tmp511;_tmp55A;});_tmp559;});_tmp558;});}}}}}_LL6D5: {
struct Cyc_List_List*_tmp55B=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,
_tmp519);struct Cyc_List_List*_tmp55C=Cyc_Tcutil_substs(rgn,inst,_tmp55B);if(
_tmp55C == _tmp55B){return t;}{struct Cyc_List_List*_tmp55D=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct _tuple4*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp519,_tmp55C);return(void*)({struct Cyc_Absyn_TupleType_struct*
_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E[0]=({struct Cyc_Absyn_TupleType_struct
_tmp55F;_tmp55F.tag=9;_tmp55F.f1=_tmp55D;_tmp55F;});_tmp55E;});}}_LL6D7: {struct
Cyc_List_List*_tmp560=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp51A);struct Cyc_List_List*_tmp561=Cyc_Tcutil_substs(rgn,inst,_tmp560);if(
_tmp561 == _tmp560){return t;}{struct Cyc_List_List*_tmp562=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp51A,
_tmp561);return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp563=_cycalloc(
sizeof(*_tmp563));_tmp563[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp564;
_tmp564.tag=11;_tmp564.f1=(void*)_tmp51B;_tmp564.f2=_tmp562;_tmp564;});_tmp563;});}}
_LL6D9: if(_tmp51C != 0){return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)_tmp51C->v);}
else{return t;}_LL6DB: {void*_tmp565=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp51D);
return _tmp565 == _tmp51D? t:(void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp566=
_cycalloc(sizeof(*_tmp566));_tmp566[0]=({struct Cyc_Absyn_RgnHandleType_struct
_tmp567;_tmp567.tag=15;_tmp567.f1=(void*)_tmp565;_tmp567;});_tmp566;});}_LL6DD: {
void*_tmp568=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp51E);return _tmp568 == _tmp51E? t:(
void*)({struct Cyc_Absyn_SizeofType_struct*_tmp569=_cycalloc(sizeof(*_tmp569));
_tmp569[0]=({struct Cyc_Absyn_SizeofType_struct _tmp56A;_tmp56A.tag=14;_tmp56A.f1=(
void*)_tmp568;_tmp56A;});_tmp569;});}_LL6DF: goto _LL6E1;_LL6E1: goto _LL6E3;_LL6E3:
goto _LL6E5;_LL6E5: goto _LL6E7;_LL6E7: goto _LL6E9;_LL6E9: goto _LL6EB;_LL6EB: return t;
_LL6ED: {void*_tmp56B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp51F);return _tmp56B == 
_tmp51F? t:(void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp56C=_cycalloc(sizeof(*
_tmp56C));_tmp56C[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp56D;_tmp56D.tag=19;
_tmp56D.f1=(void*)_tmp56B;_tmp56D;});_tmp56C;});}_LL6EF: {void*_tmp56E=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp520);return _tmp56E == _tmp520? t:(void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp570;_tmp570.tag=17;_tmp570.f1=(void*)_tmp56E;_tmp570;});_tmp56F;});}_LL6F1: {
struct Cyc_List_List*_tmp571=Cyc_Tcutil_substs(rgn,inst,_tmp521);return _tmp571 == 
_tmp521? t:(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp572=_cycalloc(sizeof(*
_tmp572));_tmp572[0]=({struct Cyc_Absyn_JoinEff_struct _tmp573;_tmp573.tag=18;
_tmp573.f1=_tmp571;_tmp573;});_tmp572;});}_LL6C3:;}static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){if(ts == 
0){return 0;}{void*_tmp574=(void*)ts->hd;struct Cyc_List_List*_tmp575=ts->tl;void*
_tmp576=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp574);struct Cyc_List_List*_tmp577=Cyc_Tcutil_substs(
rgn,inst,_tmp575);if(_tmp574 == _tmp576? _tmp575 == _tmp577: 0){return ts;}return(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp578=_cycalloc(sizeof(*_tmp578));
_tmp578->hd=(void*)_tmp576;_tmp578->tl=_tmp577;_tmp578;});}}extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple6*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){void*k=Cyc_Tcutil_tvar_kind(tv);return({struct _tuple6*_tmp579=_cycalloc(
sizeof(*_tmp579));_tmp579->f1=tv;_tmp579->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*
_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->v=(void*)k;_tmp57A;}),({struct Cyc_Core_Opt*
_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->v=s;_tmp57B;}));_tmp579;});}struct
_tuple6*Cyc_Tcutil_r_make_inst_var(struct _tuple7*env,struct Cyc_Absyn_Tvar*tv){
struct _tuple7 _tmp57D;struct _RegionHandle*_tmp57E;struct Cyc_List_List*_tmp57F;
struct _tuple7*_tmp57C=env;_tmp57D=*_tmp57C;_LL72D: _tmp57F=_tmp57D.f1;goto _LL72C;
_LL72C: _tmp57E=_tmp57D.f2;goto _LL72B;_LL72B: {void*k=Cyc_Tcutil_tvar_kind(tv);
return({struct _tuple6*_tmp580=_region_malloc(_tmp57E,sizeof(*_tmp580));_tmp580->f1=
tv;_tmp580->f2=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp581=_cycalloc(sizeof(*
_tmp581));_tmp581->v=(void*)k;_tmp581;}),({struct Cyc_Core_Opt*_tmp582=_cycalloc(
sizeof(*_tmp582));_tmp582->v=_tmp57F;_tmp582;}));_tmp580;});}}static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_Std_zstrptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=(void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;
if(! Cyc_Tcutil_constrain_kinds(k1,k2)){({struct Cyc_Std_String_pa_struct _tmp586;
_tmp586.tag=0;_tmp586.f1=(struct _tagged_arr)Cyc_Absynpp_kindbound2string(k2);{
struct Cyc_Std_String_pa_struct _tmp585;_tmp585.tag=0;_tmp585.f1=(struct
_tagged_arr)Cyc_Absynpp_kindbound2string(k1);{struct Cyc_Std_String_pa_struct
_tmp584;_tmp584.tag=0;_tmp584.f1=(struct _tagged_arr)*tv->name;{void*_tmp583[3]={&
_tmp584,& _tmp585,& _tmp586};Cyc_Tcutil_terr(loc,_tag_arr("type variable %s is used with inconsistent kinds %s and %s",
sizeof(unsigned char),59),_tag_arr(_tmp583,sizeof(void*),3));}}}});}if(tv->identity
== 0){tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;}else{if(*((int*)
_check_null(tv->identity))!= *((int*)_check_null(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity))){({
void*_tmp587[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("same type variable has different identity!",sizeof(unsigned char),43),
_tag_arr(_tmp587,sizeof(void*),0));});}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588->hd=tv;
_tmp588->tl=tvs;_tmp588;});}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0){({void*
_tmp589[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tv",sizeof(unsigned char),39),
_tag_arr(_tmp589,sizeof(void*),0));});}{struct Cyc_List_List*tvs2=tvs;for(0;tvs2
!= 0;tvs2=tvs2->tl){if(((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity == 0){({void*
_tmp58A[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("fast_add_free_tvar: bad identity in tvs2",sizeof(unsigned char),41),
_tag_arr(_tmp58A,sizeof(void*),0));});}if(*((int*)_check_null(((struct Cyc_Absyn_Tvar*)
tvs2->hd)->identity))== *((int*)_check_null(tv->identity))){return tvs;}}}return({
struct Cyc_List_List*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->hd=tv;_tmp58B->tl=
tvs;_tmp58B;});}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*
tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == 0){({struct Cyc_Std_String_pa_struct
_tmp58D;_tmp58D.tag=0;_tmp58D.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string(tv);{
void*_tmp58C[1]={& _tmp58D};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("bound tvar id for %s is NULL",sizeof(unsigned char),
29),_tag_arr(_tmp58C,sizeof(void*),1));}});}return({struct Cyc_List_List*_tmp58E=
_cycalloc(sizeof(*_tmp58E));_tmp58E->hd=tv;_tmp58E->tl=tvs;_tmp58E;});}static
struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct Cyc_List_List*es,void*e){void*
_tmp58F=Cyc_Tcutil_compress(e);int _tmp590;_LL72F: if((unsigned int)_tmp58F > 3?*((
int*)_tmp58F)== 0: 0){_LL733: _tmp590=((struct Cyc_Absyn_Evar_struct*)_tmp58F)->f3;
goto _LL730;}else{goto _LL731;}_LL731: goto _LL732;_LL730:{struct Cyc_List_List*es2=
es;for(0;es2 != 0;es2=es2->tl){void*_tmp591=Cyc_Tcutil_compress((void*)es2->hd);
int _tmp592;_LL735: if((unsigned int)_tmp591 > 3?*((int*)_tmp591)== 0: 0){_LL739:
_tmp592=((struct Cyc_Absyn_Evar_struct*)_tmp591)->f3;goto _LL736;}else{goto _LL737;}
_LL737: goto _LL738;_LL736: if(_tmp590 == _tmp592){return es;}goto _LL734;_LL738: goto
_LL734;_LL734:;}}return({struct Cyc_List_List*_tmp593=_cycalloc(sizeof(*_tmp593));
_tmp593->hd=(void*)e;_tmp593->tl=es;_tmp593;});_LL732: return es;_LL72E:;}static
struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct Cyc_List_List*tvs,struct
Cyc_List_List*btvs){struct Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int
present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){if(*((int*)
_check_null(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity))== *((int*)_check_null(((
struct Cyc_Absyn_Tvar*)b->hd)->identity))){present=1;break;}}}if(! present){r=({
struct Cyc_List_List*_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594->hd=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp594->tl=r;_tmp594;});}}r=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(r);return r;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_tagged_arr*fn){if(width != 0){unsigned int w=0;if(! Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)_check_null(width))){({struct Cyc_Std_String_pa_struct
_tmp596;_tmp596.tag=0;_tmp596.f1=(struct _tagged_arr)*fn;{void*_tmp595[1]={&
_tmp596};Cyc_Tcutil_terr(loc,_tag_arr("bitfield %s does not have constant width",
sizeof(unsigned char),41),_tag_arr(_tmp595,sizeof(void*),1));}});}else{w=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(width));}{void*_tmp597=Cyc_Tcutil_compress(
field_typ);void*_tmp598;_LL73B: if((unsigned int)_tmp597 > 3?*((int*)_tmp597)== 5:
0){_LL73F: _tmp598=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp597)->f2;goto
_LL73C;}else{goto _LL73D;}_LL73D: goto _LL73E;_LL73C:{void*_tmp599=_tmp598;_LL741:
if((int)_tmp599 == 0){goto _LL742;}else{goto _LL743;}_LL743: if((int)_tmp599 == 1){
goto _LL744;}else{goto _LL745;}_LL745: if((int)_tmp599 == 2){goto _LL746;}else{goto
_LL747;}_LL747: if((int)_tmp599 == 3){goto _LL748;}else{goto _LL740;}_LL742: if(w > 8){({
void*_tmp59A[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(unsigned char),26),_tag_arr(_tmp59A,sizeof(void*),0));});}goto _LL740;
_LL744: if(w > 16){({void*_tmp59B[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(unsigned char),26),_tag_arr(_tmp59B,sizeof(void*),0));});}goto _LL740;
_LL746: if(w > 32){({void*_tmp59C[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(unsigned char),26),_tag_arr(_tmp59C,sizeof(void*),0));});}goto _LL740;
_LL748: if(w > 64){({void*_tmp59D[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bitfield larger than type",
sizeof(unsigned char),26),_tag_arr(_tmp59D,sizeof(void*),0));});}goto _LL740;
_LL740:;}goto _LL73A;_LL73E:({struct Cyc_Std_String_pa_struct _tmp5A0;_tmp5A0.tag=0;
_tmp5A0.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(field_typ);{struct Cyc_Std_String_pa_struct
_tmp59F;_tmp59F.tag=0;_tmp59F.f1=(struct _tagged_arr)*fn;{void*_tmp59E[2]={&
_tmp59F,& _tmp5A0};Cyc_Tcutil_terr(loc,_tag_arr("bitfield %s must have integral type but has type %s",
sizeof(unsigned char),52),_tag_arr(_tmp59E,sizeof(void*),2));}}});goto _LL73A;
_LL73A:;}}}static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,
struct _tagged_arr*fn,struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){
void*_tmp5A1=(void*)atts->hd;_LL74A: if((int)_tmp5A1 == 5){goto _LL74B;}else{goto
_LL74C;}_LL74C: if((unsigned int)_tmp5A1 > 16?*((int*)_tmp5A1)== 1: 0){goto _LL74D;}
else{goto _LL74E;}_LL74E: goto _LL74F;_LL74B: continue;_LL74D: continue;_LL74F:({
struct Cyc_Std_String_pa_struct _tmp5A4;_tmp5A4.tag=0;_tmp5A4.f1=(struct
_tagged_arr)*fn;{struct Cyc_Std_String_pa_struct _tmp5A3;_tmp5A3.tag=0;_tmp5A3.f1=(
struct _tagged_arr)Cyc_Absyn_attribute2string((void*)atts->hd);{void*_tmp5A2[2]={&
_tmp5A3,& _tmp5A4};Cyc_Tcutil_terr(loc,_tag_arr("bad attribute %s on member %s",
sizeof(unsigned char),30),_tag_arr(_tmp5A2,sizeof(void*),2));}}});_LL749:;}}
struct Cyc_Tcutil_CVTEnv{struct Cyc_List_List*kind_env;struct Cyc_List_List*
free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
struct _tuple18{void*f1;int f2;};static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,void*expected_kind,void*t){{void*_tmp5A5=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*
_tmp5A6;struct Cyc_Core_Opt**_tmp5A7;struct Cyc_Core_Opt*_tmp5A8;struct Cyc_Core_Opt**
_tmp5A9;struct Cyc_Absyn_Tvar*_tmp5AA;struct Cyc_List_List*_tmp5AB;struct Cyc_Absyn_Enumdecl*
_tmp5AC;struct Cyc_Absyn_Enumdecl**_tmp5AD;struct _tuple1*_tmp5AE;struct Cyc_Absyn_TunionInfo
_tmp5AF;void*_tmp5B0;struct Cyc_List_List*_tmp5B1;struct Cyc_List_List**_tmp5B2;
void*_tmp5B3;void**_tmp5B4;struct Cyc_Absyn_TunionFieldInfo _tmp5B5;struct Cyc_List_List*
_tmp5B6;void*_tmp5B7;void**_tmp5B8;struct Cyc_Absyn_PtrInfo _tmp5B9;struct Cyc_Absyn_Conref*
_tmp5BA;struct Cyc_Absyn_Tqual _tmp5BB;struct Cyc_Absyn_Conref*_tmp5BC;void*_tmp5BD;
void*_tmp5BE;void*_tmp5BF;struct Cyc_Absyn_Exp*_tmp5C0;struct Cyc_Absyn_Tqual
_tmp5C1;void*_tmp5C2;struct Cyc_Absyn_FnInfo _tmp5C3;struct Cyc_List_List*_tmp5C4;
struct Cyc_List_List*_tmp5C5;struct Cyc_Absyn_VarargInfo*_tmp5C6;int _tmp5C7;struct
Cyc_List_List*_tmp5C8;void*_tmp5C9;struct Cyc_Core_Opt*_tmp5CA;struct Cyc_Core_Opt**
_tmp5CB;struct Cyc_List_List*_tmp5CC;struct Cyc_List_List**_tmp5CD;struct Cyc_List_List*
_tmp5CE;struct Cyc_List_List*_tmp5CF;void*_tmp5D0;struct Cyc_Absyn_AggrInfo _tmp5D1;
struct Cyc_List_List*_tmp5D2;struct Cyc_List_List**_tmp5D3;void*_tmp5D4;void**
_tmp5D5;struct Cyc_Core_Opt*_tmp5D6;struct Cyc_Core_Opt**_tmp5D7;struct Cyc_List_List*
_tmp5D8;struct Cyc_List_List**_tmp5D9;struct _tuple1*_tmp5DA;void*_tmp5DB;void*
_tmp5DC;void*_tmp5DD;struct Cyc_List_List*_tmp5DE;_LL751: if((int)_tmp5A5 == 0){
goto _LL752;}else{goto _LL753;}_LL753: if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)
== 0: 0){_LL780: _tmp5A8=((struct Cyc_Absyn_Evar_struct*)_tmp5A5)->f1;_tmp5A9=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp5A5)->f1;goto _LL77F;
_LL77F: _tmp5A6=((struct Cyc_Absyn_Evar_struct*)_tmp5A5)->f2;_tmp5A7=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp5A5)->f2;goto _LL754;}else{goto _LL755;}_LL755:
if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)== 1: 0){_LL781: _tmp5AA=((struct Cyc_Absyn_VarType_struct*)
_tmp5A5)->f1;goto _LL756;}else{goto _LL757;}_LL757: if((unsigned int)_tmp5A5 > 3?*((
int*)_tmp5A5)== 13: 0){_LL782: _tmp5AB=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp5A5)->f1;goto _LL758;}else{goto _LL759;}_LL759: if((unsigned int)_tmp5A5 > 3?*((
int*)_tmp5A5)== 12: 0){_LL784: _tmp5AE=((struct Cyc_Absyn_EnumType_struct*)_tmp5A5)->f1;
goto _LL783;_LL783: _tmp5AC=((struct Cyc_Absyn_EnumType_struct*)_tmp5A5)->f2;
_tmp5AD=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)_tmp5A5)->f2;
goto _LL75A;}else{goto _LL75B;}_LL75B: if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)
== 2: 0){_LL785: _tmp5AF=((struct Cyc_Absyn_TunionType_struct*)_tmp5A5)->f1;_LL788:
_tmp5B3=(void*)_tmp5AF.tunion_info;_tmp5B4=(void**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp5A5)->f1).tunion_info;goto _LL787;_LL787: _tmp5B1=_tmp5AF.targs;_tmp5B2=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_TunionType_struct*)_tmp5A5)->f1).targs;
goto _LL786;_LL786: _tmp5B0=(void*)_tmp5AF.rgn;goto _LL75C;}else{goto _LL75D;}_LL75D:
if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)== 3: 0){_LL789: _tmp5B5=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp5A5)->f1;_LL78B: _tmp5B7=(void*)_tmp5B5.field_info;_tmp5B8=(void**)&(((struct
Cyc_Absyn_TunionFieldType_struct*)_tmp5A5)->f1).field_info;goto _LL78A;_LL78A:
_tmp5B6=_tmp5B5.targs;goto _LL75E;}else{goto _LL75F;}_LL75F: if((unsigned int)
_tmp5A5 > 3?*((int*)_tmp5A5)== 4: 0){_LL78C: _tmp5B9=((struct Cyc_Absyn_PointerType_struct*)
_tmp5A5)->f1;_LL791: _tmp5BE=(void*)_tmp5B9.elt_typ;goto _LL790;_LL790: _tmp5BD=(
void*)_tmp5B9.rgn_typ;goto _LL78F;_LL78F: _tmp5BC=_tmp5B9.nullable;goto _LL78E;
_LL78E: _tmp5BB=_tmp5B9.tq;goto _LL78D;_LL78D: _tmp5BA=_tmp5B9.bounds;goto _LL760;}
else{goto _LL761;}_LL761: if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)== 14: 0){
_LL792: _tmp5BF=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp5A5)->f1;goto
_LL762;}else{goto _LL763;}_LL763: if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)== 5:
0){goto _LL764;}else{goto _LL765;}_LL765: if((int)_tmp5A5 == 1){goto _LL766;}else{
goto _LL767;}_LL767: if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)== 6: 0){goto
_LL768;}else{goto _LL769;}_LL769: if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)== 7:
0){_LL795: _tmp5C2=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp5A5)->f1;goto
_LL794;_LL794: _tmp5C1=((struct Cyc_Absyn_ArrayType_struct*)_tmp5A5)->f2;goto
_LL793;_LL793: _tmp5C0=((struct Cyc_Absyn_ArrayType_struct*)_tmp5A5)->f3;goto
_LL76A;}else{goto _LL76B;}_LL76B: if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)== 8:
0){_LL796: _tmp5C3=((struct Cyc_Absyn_FnType_struct*)_tmp5A5)->f1;_LL79E: _tmp5CC=
_tmp5C3.tvars;_tmp5CD=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)
_tmp5A5)->f1).tvars;goto _LL79D;_LL79D: _tmp5CA=_tmp5C3.effect;_tmp5CB=(struct Cyc_Core_Opt**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp5A5)->f1).effect;goto _LL79C;_LL79C: _tmp5C9=(
void*)_tmp5C3.ret_typ;goto _LL79B;_LL79B: _tmp5C8=_tmp5C3.args;goto _LL79A;_LL79A:
_tmp5C7=_tmp5C3.c_varargs;goto _LL799;_LL799: _tmp5C6=_tmp5C3.cyc_varargs;goto
_LL798;_LL798: _tmp5C5=_tmp5C3.rgn_po;goto _LL797;_LL797: _tmp5C4=_tmp5C3.attributes;
goto _LL76C;}else{goto _LL76D;}_LL76D: if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)
== 9: 0){_LL79F: _tmp5CE=((struct Cyc_Absyn_TupleType_struct*)_tmp5A5)->f1;goto
_LL76E;}else{goto _LL76F;}_LL76F: if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)== 
11: 0){_LL7A1: _tmp5D0=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp5A5)->f1;
goto _LL7A0;_LL7A0: _tmp5CF=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp5A5)->f2;
goto _LL770;}else{goto _LL771;}_LL771: if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)
== 10: 0){_LL7A2: _tmp5D1=((struct Cyc_Absyn_AggrType_struct*)_tmp5A5)->f1;_LL7A4:
_tmp5D4=(void*)_tmp5D1.aggr_info;_tmp5D5=(void**)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp5A5)->f1).aggr_info;goto _LL7A3;_LL7A3: _tmp5D2=_tmp5D1.targs;_tmp5D3=(struct
Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_struct*)_tmp5A5)->f1).targs;goto
_LL772;}else{goto _LL773;}_LL773: if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)== 
16: 0){_LL7A7: _tmp5DA=((struct Cyc_Absyn_TypedefType_struct*)_tmp5A5)->f1;goto
_LL7A6;_LL7A6: _tmp5D8=((struct Cyc_Absyn_TypedefType_struct*)_tmp5A5)->f2;_tmp5D9=(
struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp5A5)->f2;goto
_LL7A5;_LL7A5: _tmp5D6=((struct Cyc_Absyn_TypedefType_struct*)_tmp5A5)->f3;_tmp5D7=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp5A5)->f3;goto
_LL774;}else{goto _LL775;}_LL775: if((int)_tmp5A5 == 2){goto _LL776;}else{goto _LL777;}
_LL777: if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)== 15: 0){_LL7A8: _tmp5DB=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp5A5)->f1;goto _LL778;}else{goto _LL779;}
_LL779: if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)== 17: 0){_LL7A9: _tmp5DC=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp5A5)->f1;goto _LL77A;}else{goto _LL77B;}
_LL77B: if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)== 19: 0){_LL7AA: _tmp5DD=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp5A5)->f1;goto _LL77C;}else{goto _LL77D;}_LL77D:
if((unsigned int)_tmp5A5 > 3?*((int*)_tmp5A5)== 18: 0){_LL7AB: _tmp5DE=((struct Cyc_Absyn_JoinEff_struct*)
_tmp5A5)->f1;goto _LL77E;}else{goto _LL750;}_LL752: goto _LL750;_LL754: if(*_tmp5A9 == 
0){*_tmp5A9=({struct Cyc_Core_Opt*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF->v=(
void*)expected_kind;_tmp5DF;});}if((cvtenv.fn_result? cvtenv.generalize_evars: 0)?
expected_kind == (void*)3: 0){*_tmp5A7=({struct Cyc_Core_Opt*_tmp5E0=_cycalloc(
sizeof(*_tmp5E0));_tmp5E0->v=(void*)((void*)2);_tmp5E0;});}else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp5E1=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp5E5=_cycalloc(sizeof(*_tmp5E5));_tmp5E5[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp5E6;_tmp5E6.tag=2;_tmp5E6.f1=0;_tmp5E6.f2=(void*)expected_kind;_tmp5E6;});
_tmp5E5;}));*_tmp5A7=({struct Cyc_Core_Opt*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));
_tmp5E2->v=(void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp5E3=_cycalloc(
sizeof(*_tmp5E3));_tmp5E3[0]=({struct Cyc_Absyn_VarType_struct _tmp5E4;_tmp5E4.tag=
1;_tmp5E4.f1=_tmp5E1;_tmp5E4;});_tmp5E3;}));_tmp5E2;});_tmp5AA=_tmp5E1;goto
_LL756;}else{cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,t);}}
goto _LL750;_LL756:{void*_tmp5E7=Cyc_Absyn_compress_kb((void*)_tmp5AA->kind);
struct Cyc_Core_Opt*_tmp5E8;struct Cyc_Core_Opt**_tmp5E9;_LL7AD: if(*((int*)_tmp5E7)
== 1){_LL7B1: _tmp5E8=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E7)->f1;_tmp5E9=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp5E7)->f1;goto
_LL7AE;}else{goto _LL7AF;}_LL7AF: goto _LL7B0;_LL7AE:*_tmp5E9=({struct Cyc_Core_Opt*
_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp5EC;_tmp5EC.tag=2;_tmp5EC.f1=0;_tmp5EC.f2=(void*)expected_kind;_tmp5EC;});
_tmp5EB;}));_tmp5EA;});goto _LL7AC;_LL7B0: goto _LL7AC;_LL7AC:;}cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(
loc,cvtenv.kind_env,_tmp5AA);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(
cvtenv.free_vars,_tmp5AA);goto _LL750;_LL758: {struct Cyc_Tcenv_Genv*ge=((struct
Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,te->ns);{struct _RegionHandle _tmp5ED=_new_region("uprev_rgn");struct
_RegionHandle*uprev_rgn=& _tmp5ED;_push_region(uprev_rgn);{struct Cyc_List_List*
prev_fields=0;unsigned int tag_count=0;for(0;_tmp5AB != 0;_tmp5AB=_tmp5AB->tl){
struct Cyc_Absyn_Enumfield*_tmp5EE=(struct Cyc_Absyn_Enumfield*)_tmp5AB->hd;if(((
int(*)(int(*compare)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*
l,struct _tagged_arr*x))Cyc_List_mem)(Cyc_Std_zstrptrcmp,prev_fields,(*_tmp5EE->name).f2)){({
struct Cyc_Std_String_pa_struct _tmp5F0;_tmp5F0.tag=0;_tmp5F0.f1=(struct
_tagged_arr)*(*_tmp5EE->name).f2;{void*_tmp5EF[1]={& _tmp5F0};Cyc_Tcutil_terr(
_tmp5EE->loc,_tag_arr("duplicate enum field name %s",sizeof(unsigned char),29),
_tag_arr(_tmp5EF,sizeof(void*),1));}});}else{prev_fields=({struct Cyc_List_List*
_tmp5F1=_region_malloc(uprev_rgn,sizeof(*_tmp5F1));_tmp5F1->hd=(*_tmp5EE->name).f2;
_tmp5F1->tl=prev_fields;_tmp5F1;});}if(_tmp5EE->tag == 0){_tmp5EE->tag=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(tag_count,_tmp5EE->loc);}else{if(! Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)_check_null(_tmp5EE->tag))){({struct Cyc_Std_String_pa_struct
_tmp5F3;_tmp5F3.tag=0;_tmp5F3.f1=(struct _tagged_arr)*(*_tmp5EE->name).f2;{void*
_tmp5F2[1]={& _tmp5F3};Cyc_Tcutil_terr(loc,_tag_arr("enum field %s: expression is not constant",
sizeof(unsigned char),42),_tag_arr(_tmp5F2,sizeof(void*),1));}});}}{unsigned int
t1=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp5EE->tag));
tag_count=t1 + 1;(*_tmp5EE->name).f1=(void*)({struct Cyc_Absyn_Abs_n_struct*
_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4[0]=({struct Cyc_Absyn_Abs_n_struct
_tmp5F5;_tmp5F5.tag=1;_tmp5F5.f1=te->ns;_tmp5F5;});_tmp5F4;});ge->ordinaries=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct _tuple18*
v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp5EE->name).f2,({struct _tuple18*_tmp5F6=
_cycalloc(sizeof(*_tmp5F6));_tmp5F6->f1=(void*)({struct Cyc_Tcenv_AnonEnumRes_struct*
_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7[0]=({struct Cyc_Tcenv_AnonEnumRes_struct
_tmp5F8;_tmp5F8.tag=4;_tmp5F8.f1=(void*)t;_tmp5F8.f2=_tmp5EE;_tmp5F8;});_tmp5F7;});
_tmp5F6->f2=1;_tmp5F6;}));}}};_pop_region(uprev_rgn);}goto _LL750;}_LL75A: if(*
_tmp5AD == 0? 1:((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp5AD))->fields == 0){
struct _handler_cons _tmp5F9;_push_handler(& _tmp5F9);{int _tmp5FB=0;if(setjmp(
_tmp5F9.handler)){_tmp5FB=1;}if(! _tmp5FB){{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(
te,loc,_tmp5AE);*_tmp5AD=(struct Cyc_Absyn_Enumdecl*)*ed;};_pop_handler();}else{
void*_tmp5FA=(void*)_exn_thrown;void*_tmp5FD=_tmp5FA;_LL7B3: if(_tmp5FD == Cyc_Dict_Absent){
goto _LL7B4;}else{goto _LL7B5;}_LL7B5: goto _LL7B6;_LL7B4: {struct Cyc_Tcenv_Genv*
_tmp5FE=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*_tmp5FF=({struct Cyc_Absyn_Enumdecl*
_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->sc=(void*)((void*)3);_tmp600->name=
_tmp5AE;_tmp600->fields=0;_tmp600;});Cyc_Tc_tcEnumdecl(te,_tmp5FE,loc,_tmp5FF);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp5AE);*_tmp5AD=(
struct Cyc_Absyn_Enumdecl*)*ed;goto _LL7B2;}}_LL7B6:(void)_throw(_tmp5FD);_LL7B2:;}}}{
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp5AD);*
_tmp5AE=(ed->name)[_check_known_subscript_notnull(1,0)];goto _LL750;}_LL75C: {
struct Cyc_List_List*_tmp601=*_tmp5B2;{void*_tmp602=*_tmp5B4;struct Cyc_Absyn_UnknownTunionInfo
_tmp603;int _tmp604;struct _tuple1*_tmp605;struct Cyc_Absyn_Tuniondecl**_tmp606;
struct Cyc_Absyn_Tuniondecl*_tmp607;_LL7B8: if(*((int*)_tmp602)== 0){_LL7BC:
_tmp603=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp602)->f1;_LL7BE: _tmp605=
_tmp603.name;goto _LL7BD;_LL7BD: _tmp604=_tmp603.is_xtunion;goto _LL7B9;}else{goto
_LL7BA;}_LL7BA: if(*((int*)_tmp602)== 1){_LL7BF: _tmp606=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp602)->f1;_tmp607=*_tmp606;goto _LL7BB;}else{goto _LL7B7;}_LL7B9: {struct Cyc_Absyn_Tuniondecl**
tudp;{struct _handler_cons _tmp608;_push_handler(& _tmp608);{int _tmp60A=0;if(setjmp(
_tmp608.handler)){_tmp60A=1;}if(! _tmp60A){tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp605);;_pop_handler();}else{void*_tmp609=(void*)_exn_thrown;void*_tmp60C=
_tmp609;_LL7C1: if(_tmp60C == Cyc_Dict_Absent){goto _LL7C2;}else{goto _LL7C3;}_LL7C3:
goto _LL7C4;_LL7C2: {struct Cyc_Tcenv_Genv*_tmp60D=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Tuniondecl*_tmp60E=({struct Cyc_Absyn_Tuniondecl*_tmp612=
_cycalloc(sizeof(*_tmp612));_tmp612->sc=(void*)((void*)3);_tmp612->name=_tmp605;
_tmp612->tvs=0;_tmp612->fields=0;_tmp612->is_xtunion=_tmp604;_tmp612;});Cyc_Tc_tcTuniondecl(
te,_tmp60D,loc,_tmp60E);tudp=Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp605);if(
_tmp601 != 0){({struct Cyc_Std_String_pa_struct _tmp611;_tmp611.tag=0;_tmp611.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp605);{struct Cyc_Std_String_pa_struct
_tmp610;_tmp610.tag=0;_tmp610.f1=(struct _tagged_arr)(_tmp604? _tag_arr("xtunion",
sizeof(unsigned char),8): _tag_arr("tunion",sizeof(unsigned char),7));{void*
_tmp60F[2]={& _tmp610,& _tmp611};Cyc_Tcutil_terr(loc,_tag_arr("please declare parameterized %s %s before using",
sizeof(unsigned char),48),_tag_arr(_tmp60F,sizeof(void*),2));}}});return cvtenv;}
goto _LL7C0;}_LL7C4:(void)_throw(_tmp60C);_LL7C0:;}}}if((*tudp)->is_xtunion != 
_tmp604){({struct Cyc_Std_String_pa_struct _tmp614;_tmp614.tag=0;_tmp614.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp605);{void*_tmp613[1]={& _tmp614};
Cyc_Tcutil_terr(loc,_tag_arr("[x]tunion is different from type declaration %s",
sizeof(unsigned char),48),_tag_arr(_tmp613,sizeof(void*),1));}});}*_tmp5B4=(void*)({
struct Cyc_Absyn_KnownTunion_struct*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615[0]=({
struct Cyc_Absyn_KnownTunion_struct _tmp616;_tmp616.tag=1;_tmp616.f1=tudp;_tmp616;});
_tmp615;});_tmp607=*tudp;goto _LL7BB;}_LL7BB: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp5B0);{struct Cyc_List_List*tvs=_tmp607->tvs;for(0;
_tmp601 != 0? tvs != 0: 0;(_tmp601=_tmp601->tl,tvs=tvs->tl)){void*t1=(void*)_tmp601->hd;
void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1);}if(_tmp601 != 0){({struct Cyc_Std_String_pa_struct _tmp618;
_tmp618.tag=0;_tmp618.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp607->name);{
void*_tmp617[1]={& _tmp618};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s",
sizeof(unsigned char),38),_tag_arr(_tmp617,sizeof(void*),1));}});}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->hd=(void*)e;_tmp619->tl=
hidden_ts;_tmp619;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e);}*
_tmp5B2=Cyc_List_imp_append(*_tmp5B2,Cyc_List_imp_rev(hidden_ts));}goto _LL7B7;}
_LL7B7:;}goto _LL750;}_LL75E:{void*_tmp61A=*_tmp5B8;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp61B;int _tmp61C;struct _tuple1*_tmp61D;struct _tuple1*_tmp61E;struct Cyc_Absyn_Tunionfield*
_tmp61F;struct Cyc_Absyn_Tuniondecl*_tmp620;_LL7C6: if(*((int*)_tmp61A)== 0){
_LL7CA: _tmp61B=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp61A)->f1;_LL7CD:
_tmp61E=_tmp61B.tunion_name;goto _LL7CC;_LL7CC: _tmp61D=_tmp61B.field_name;goto
_LL7CB;_LL7CB: _tmp61C=_tmp61B.is_xtunion;goto _LL7C7;}else{goto _LL7C8;}_LL7C8: if(*((
int*)_tmp61A)== 1){_LL7CF: _tmp620=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp61A)->f1;goto _LL7CE;_LL7CE: _tmp61F=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp61A)->f2;goto _LL7C9;}else{goto _LL7C5;}_LL7C7: {struct Cyc_Absyn_Tuniondecl*
tud;struct Cyc_Absyn_Tunionfield*tuf;{struct _handler_cons _tmp621;_push_handler(&
_tmp621);{int _tmp623=0;if(setjmp(_tmp621.handler)){_tmp623=1;}if(! _tmp623){*Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp61E);;_pop_handler();}else{void*_tmp622=(void*)_exn_thrown;void*
_tmp625=_tmp622;_LL7D1: if(_tmp625 == Cyc_Dict_Absent){goto _LL7D2;}else{goto _LL7D3;}
_LL7D3: goto _LL7D4;_LL7D2:({struct Cyc_Std_String_pa_struct _tmp627;_tmp627.tag=0;
_tmp627.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp61E);{void*_tmp626[1]={&
_tmp627};Cyc_Tcutil_terr(loc,_tag_arr("unbound type tunion %s",sizeof(
unsigned char),23),_tag_arr(_tmp626,sizeof(void*),1));}});return cvtenv;_LL7D4:(
void)_throw(_tmp625);_LL7D0:;}}}{struct _handler_cons _tmp628;_push_handler(&
_tmp628);{int _tmp62A=0;if(setjmp(_tmp628.handler)){_tmp62A=1;}if(! _tmp62A){{void*
_tmp62B=Cyc_Tcenv_lookup_ordinary(te,loc,_tmp61D);struct Cyc_Absyn_Tunionfield*
_tmp62C;struct Cyc_Absyn_Tuniondecl*_tmp62D;_LL7D6: if(*((int*)_tmp62B)== 2){
_LL7DB: _tmp62D=((struct Cyc_Tcenv_TunionRes_struct*)_tmp62B)->f1;goto _LL7DA;
_LL7DA: _tmp62C=((struct Cyc_Tcenv_TunionRes_struct*)_tmp62B)->f2;goto _LL7D7;}
else{goto _LL7D8;}_LL7D8: goto _LL7D9;_LL7D7: tuf=_tmp62C;tud=_tmp62D;if(tud->is_xtunion
!= _tmp61C){({struct Cyc_Std_String_pa_struct _tmp62F;_tmp62F.tag=0;_tmp62F.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp61E);{void*_tmp62E[1]={& _tmp62F};
Cyc_Tcutil_terr(loc,_tag_arr("[x]tunion is different from type declaration %s",
sizeof(unsigned char),48),_tag_arr(_tmp62E,sizeof(void*),1));}});}goto _LL7D5;
_LL7D9:({struct Cyc_Std_String_pa_struct _tmp632;_tmp632.tag=0;_tmp632.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp61E);{struct Cyc_Std_String_pa_struct
_tmp631;_tmp631.tag=0;_tmp631.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp61D);{void*_tmp630[2]={& _tmp631,& _tmp632};Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",
sizeof(unsigned char),35),_tag_arr(_tmp630,sizeof(void*),2));}}});{struct Cyc_Tcutil_CVTEnv
_tmp633=cvtenv;_npop_handler(0);return _tmp633;}_LL7D5:;};_pop_handler();}else{
void*_tmp629=(void*)_exn_thrown;void*_tmp635=_tmp629;_LL7DD: if(_tmp635 == Cyc_Dict_Absent){
goto _LL7DE;}else{goto _LL7DF;}_LL7DF: goto _LL7E0;_LL7DE:({struct Cyc_Std_String_pa_struct
_tmp638;_tmp638.tag=0;_tmp638.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp61E);{struct Cyc_Std_String_pa_struct _tmp637;_tmp637.tag=0;_tmp637.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp61D);{void*_tmp636[2]={& _tmp637,& _tmp638};
Cyc_Tcutil_terr(loc,_tag_arr("unbound field %s in type tunion %s",sizeof(
unsigned char),35),_tag_arr(_tmp636,sizeof(void*),2));}}});return cvtenv;_LL7E0:(
void)_throw(_tmp635);_LL7DC:;}}}*_tmp5B8=(void*)({struct Cyc_Absyn_KnownTunionfield_struct*
_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639[0]=({struct Cyc_Absyn_KnownTunionfield_struct
_tmp63A;_tmp63A.tag=1;_tmp63A.f1=tud;_tmp63A.f2=tuf;_tmp63A;});_tmp639;});
_tmp620=tud;_tmp61F=tuf;goto _LL7C9;}_LL7C9: {struct Cyc_List_List*tvs=_tmp620->tvs;
for(0;_tmp5B6 != 0? tvs != 0: 0;(_tmp5B6=_tmp5B6->tl,tvs=tvs->tl)){void*t1=(void*)
_tmp5B6->hd;void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,t1);}if(_tmp5B6 != 0){({struct Cyc_Std_String_pa_struct
_tmp63D;_tmp63D.tag=0;_tmp63D.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp61F->name);{struct Cyc_Std_String_pa_struct _tmp63C;_tmp63C.tag=0;_tmp63C.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp620->name);{void*_tmp63B[2]={&
_tmp63C,& _tmp63D};Cyc_Tcutil_terr(loc,_tag_arr("too many type arguments for tunion %s.%s",
sizeof(unsigned char),41),_tag_arr(_tmp63B,sizeof(void*),2));}}});}if(tvs != 0){({
struct Cyc_Std_String_pa_struct _tmp640;_tmp640.tag=0;_tmp640.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp61F->name);{struct Cyc_Std_String_pa_struct
_tmp63F;_tmp63F.tag=0;_tmp63F.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp620->name);{void*_tmp63E[2]={& _tmp63F,& _tmp640};Cyc_Tcutil_terr(loc,_tag_arr("too few type arguments for tunion %s.%s",
sizeof(unsigned char),40),_tag_arr(_tmp63E,sizeof(void*),2));}}});}goto _LL7C5;}
_LL7C5:;}goto _LL750;_LL760: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(
void*)0,_tmp5BE);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,
_tmp5BD);{void*_tmp641=(void*)(Cyc_Absyn_compress_conref(_tmp5BA))->v;void*
_tmp642;struct Cyc_Absyn_Exp*_tmp643;_LL7E2: if((unsigned int)_tmp641 > 1?*((int*)
_tmp641)== 0: 0){_LL7E6: _tmp642=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp641)->f1;if((unsigned int)_tmp642 > 1?*((int*)_tmp642)== 0: 0){_LL7E7: _tmp643=((
struct Cyc_Absyn_Upper_b_struct*)_tmp642)->f1;goto _LL7E3;}else{goto _LL7E4;}}else{
goto _LL7E4;}_LL7E4: goto _LL7E5;_LL7E3: if(! Cyc_Tcutil_is_const_exp(te,_tmp643)){({
void*_tmp644[0]={};Cyc_Tcutil_terr(loc,_tag_arr("pointer bounds expression is not a constant",
sizeof(unsigned char),44),_tag_arr(_tmp644,sizeof(void*),0));});}Cyc_Tcexp_tcExp(
te,0,_tmp643);if(! Cyc_Tcutil_coerce_uint_typ(te,_tmp643)){({void*_tmp645[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("pointer bounds expression is not an unsigned int",
sizeof(unsigned char),49),_tag_arr(_tmp645,sizeof(void*),0));});}Cyc_Evexp_eval_const_uint_exp(
_tmp643);goto _LL7E1;_LL7E5: goto _LL7E1;_LL7E1:;}goto _LL750;_LL762: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp5BF);goto _LL750;_LL764: goto _LL766;_LL766: goto _LL768;
_LL768: goto _LL750;_LL76A: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(
void*)1,_tmp5C2);if(_tmp5C0 == 0? 1: ! Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_check_null(_tmp5C0))){({void*_tmp646[0]={};Cyc_Tcutil_terr(loc,_tag_arr("array bounds expression is not constant",
sizeof(unsigned char),40),_tag_arr(_tmp646,sizeof(void*),0));});}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp5C0));if(! Cyc_Tcutil_coerce_uint_typ(
te,(struct Cyc_Absyn_Exp*)_check_null(_tmp5C0))){({void*_tmp647[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("array bounds expression is not an unsigned int",sizeof(
unsigned char),47),_tag_arr(_tmp647,sizeof(void*),0));});}Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp5C0));goto _LL750;_LL76C: {int num_convs=0;
int seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int seen_format=0;void*ft=(
void*)0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;_tmp5C4 != 0;_tmp5C4=_tmp5C4->tl){
if(! Cyc_Absyn_fntype_att((void*)_tmp5C4->hd)){({struct Cyc_Std_String_pa_struct
_tmp649;_tmp649.tag=0;_tmp649.f1=(struct _tagged_arr)Cyc_Absyn_attribute2string((
void*)_tmp5C4->hd);{void*_tmp648[1]={& _tmp649};Cyc_Tcutil_terr(loc,_tag_arr("bad function type attribute %s",
sizeof(unsigned char),31),_tag_arr(_tmp648,sizeof(void*),1));}});}{void*_tmp64A=(
void*)_tmp5C4->hd;int _tmp64B;int _tmp64C;void*_tmp64D;_LL7E9: if((int)_tmp64A == 0){
goto _LL7EA;}else{goto _LL7EB;}_LL7EB: if((int)_tmp64A == 1){goto _LL7EC;}else{goto
_LL7ED;}_LL7ED: if((int)_tmp64A == 2){goto _LL7EE;}else{goto _LL7EF;}_LL7EF: if((
unsigned int)_tmp64A > 16?*((int*)_tmp64A)== 3: 0){_LL7F5: _tmp64D=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp64A)->f1;goto _LL7F4;_LL7F4: _tmp64C=((struct Cyc_Absyn_Format_att_struct*)
_tmp64A)->f2;goto _LL7F3;_LL7F3: _tmp64B=((struct Cyc_Absyn_Format_att_struct*)
_tmp64A)->f3;goto _LL7F0;}else{goto _LL7F1;}_LL7F1: goto _LL7F2;_LL7EA: if(!
seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL7E8;_LL7EC: if(! seen_cdecl){
seen_cdecl=1;++ num_convs;}goto _LL7E8;_LL7EE: if(! seen_fastcall){seen_fastcall=1;
++ num_convs;}goto _LL7E8;_LL7F0: if(! seen_format){seen_format=1;ft=_tmp64D;
fmt_desc_arg=_tmp64C;fmt_arg_start=_tmp64B;}else{({void*_tmp64E[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("function can't have multiple format attributes",sizeof(
unsigned char),47),_tag_arr(_tmp64E,sizeof(void*),0));});}goto _LL7E8;_LL7F2: goto
_LL7E8;_LL7E8:;}}if(num_convs > 1){({void*_tmp64F[0]={};Cyc_Tcutil_terr(loc,
_tag_arr("function can't have multiple calling conventions",sizeof(unsigned char),
49),_tag_arr(_tmp64F,sizeof(void*),0));});}Cyc_Tcutil_check_unique_tvars(loc,*
_tmp5CD);{struct Cyc_List_List*b=*_tmp5CD;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)
b->hd)->identity=Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(
cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp650=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp651;_LL7F7: if(*((int*)
_tmp650)== 0){_LL7FB: _tmp651=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp650)->f1;
if((int)_tmp651 == 1){goto _LL7F8;}else{goto _LL7F9;}}else{goto _LL7F9;}_LL7F9: goto
_LL7FA;_LL7F8:({struct Cyc_Std_String_pa_struct _tmp653;_tmp653.tag=0;_tmp653.f1=(
struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)b->hd)->name;{void*_tmp652[1]={&
_tmp653};Cyc_Tcutil_terr(loc,_tag_arr("function attempts to abstract Mem type variable %s",
sizeof(unsigned char),51),_tag_arr(_tmp652,sizeof(void*),1));}});goto _LL7F6;
_LL7FA: goto _LL7F6;_LL7F6:;}}}{struct Cyc_Tcutil_CVTEnv _tmp654=({struct Cyc_Tcutil_CVTEnv
_tmp6C3;_tmp6C3.kind_env=cvtenv.kind_env;_tmp6C3.free_vars=0;_tmp6C3.free_evars=
0;_tmp6C3.generalize_evars=cvtenv.generalize_evars;_tmp6C3.fn_result=1;_tmp6C3;});
_tmp654=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp654,(void*)1,_tmp5C9);_tmp654.fn_result=
0;{struct Cyc_List_List*a=_tmp5C8;for(0;a != 0;a=a->tl){_tmp654=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp654,(void*)1,(*((struct _tuple2*)a->hd)).f3);}}if(_tmp5C6 != 0){if(
_tmp5C7){({void*_tmp655[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("both c_vararg and cyc_vararg",sizeof(unsigned char),
29),_tag_arr(_tmp655,sizeof(void*),0));});}{int _tmp657;void*_tmp658;struct Cyc_Absyn_Tqual
_tmp659;struct Cyc_Core_Opt*_tmp65A;struct Cyc_Absyn_VarargInfo _tmp656=*_tmp5C6;
_LL800: _tmp65A=_tmp656.name;goto _LL7FF;_LL7FF: _tmp659=_tmp656.tq;goto _LL7FE;
_LL7FE: _tmp658=(void*)_tmp656.type;goto _LL7FD;_LL7FD: _tmp657=_tmp656.inject;goto
_LL7FC;_LL7FC: _tmp654=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp654,(void*)1,
_tmp658);if(_tmp657){void*_tmp65B=Cyc_Tcutil_compress(_tmp658);struct Cyc_Absyn_TunionInfo
_tmp65C;void*_tmp65D;void*_tmp65E;_LL802: if((unsigned int)_tmp65B > 3?*((int*)
_tmp65B)== 2: 0){_LL806: _tmp65C=((struct Cyc_Absyn_TunionType_struct*)_tmp65B)->f1;
_LL808: _tmp65E=(void*)_tmp65C.tunion_info;if(*((int*)_tmp65E)== 1){goto _LL807;}
else{goto _LL804;}_LL807: _tmp65D=(void*)_tmp65C.rgn;goto _LL803;}else{goto _LL804;}
_LL804: goto _LL805;_LL803: goto _LL801;_LL805:({void*_tmp65F[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("can't inject a non-[x]tunion type",sizeof(unsigned char),34),
_tag_arr(_tmp65F,sizeof(void*),0));});goto _LL801;_LL801:;}}}if(seen_format){int
_tmp660=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5C8);if((((
fmt_desc_arg < 0? 1: fmt_desc_arg > _tmp660)? 1: fmt_arg_start < 0)? 1:(_tmp5C6 == 0?
fmt_arg_start != 0: 0))? 1:(_tmp5C6 != 0? fmt_arg_start != _tmp660 + 1: 0)){({void*
_tmp661[0]={};Cyc_Tcutil_terr(loc,_tag_arr("bad format descriptor",sizeof(
unsigned char),22),_tag_arr(_tmp661,sizeof(void*),0));});}else{void*_tmp663;
struct _tuple2 _tmp662=*((struct _tuple2*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(
_tmp5C8,fmt_desc_arg - 1);_LL80A: _tmp663=_tmp662.f3;goto _LL809;_LL809:{void*
_tmp664=Cyc_Tcutil_compress(_tmp663);struct Cyc_Absyn_PtrInfo _tmp665;struct Cyc_Absyn_Conref*
_tmp666;void*_tmp667;_LL80C: if((unsigned int)_tmp664 > 3?*((int*)_tmp664)== 4: 0){
_LL810: _tmp665=((struct Cyc_Absyn_PointerType_struct*)_tmp664)->f1;_LL812: _tmp667=(
void*)_tmp665.elt_typ;goto _LL811;_LL811: _tmp666=_tmp665.bounds;goto _LL80D;}else{
goto _LL80E;}_LL80E: goto _LL80F;_LL80D:{struct _tuple9 _tmp669=({struct _tuple9
_tmp668;_tmp668.f1=Cyc_Tcutil_compress(_tmp667);_tmp668.f2=Cyc_Absyn_conref_def((
void*)0,_tmp666);_tmp668;});void*_tmp66A;void*_tmp66B;void*_tmp66C;void*_tmp66D;
_LL814: _LL819: _tmp66B=_tmp669.f1;if((unsigned int)_tmp66B > 3?*((int*)_tmp66B)== 
5: 0){_LL81B: _tmp66D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp66B)->f1;if((
int)_tmp66D == 1){goto _LL81A;}else{goto _LL816;}_LL81A: _tmp66C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp66B)->f2;if((int)_tmp66C == 0){goto _LL818;}else{goto _LL816;}}else{goto _LL816;}
_LL818: _tmp66A=_tmp669.f2;if((int)_tmp66A == 0){goto _LL815;}else{goto _LL816;}
_LL816: goto _LL817;_LL815: goto _LL813;_LL817:({void*_tmp66E[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("format descriptor is not a char ? type",sizeof(unsigned char),39),
_tag_arr(_tmp66E,sizeof(void*),0));});goto _LL813;_LL813:;}goto _LL80B;_LL80F:({
void*_tmp66F[0]={};Cyc_Tcutil_terr(loc,_tag_arr("format descriptor is not a char ? type",
sizeof(unsigned char),39),_tag_arr(_tmp66F,sizeof(void*),0));});goto _LL80B;
_LL80B:;}if(fmt_arg_start != 0){void*_tmp670=Cyc_Tcutil_compress((void*)((struct
Cyc_Absyn_VarargInfo*)_check_null(_tmp5C6))->type);int problem;{void*_tmp671=ft;
_LL81D: if((int)_tmp671 == 0){goto _LL81E;}else{goto _LL81F;}_LL81F: if((int)_tmp671
== 1){goto _LL820;}else{goto _LL81C;}_LL81E:{void*_tmp672=_tmp670;struct Cyc_Absyn_TunionInfo
_tmp673;void*_tmp674;struct Cyc_Absyn_Tuniondecl**_tmp675;struct Cyc_Absyn_Tuniondecl*
_tmp676;_LL822: if((unsigned int)_tmp672 > 3?*((int*)_tmp672)== 2: 0){_LL826:
_tmp673=((struct Cyc_Absyn_TunionType_struct*)_tmp672)->f1;_LL827: _tmp674=(void*)
_tmp673.tunion_info;if(*((int*)_tmp674)== 1){_LL828: _tmp675=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp674)->f1;_tmp676=*_tmp675;goto _LL823;}else{goto _LL824;}}else{goto _LL824;}
_LL824: goto _LL825;_LL823: problem=Cyc_Absyn_qvar_cmp(_tmp676->name,Cyc_Absyn_tunion_print_arg_qvar)
!= 0;goto _LL821;_LL825: problem=1;goto _LL821;_LL821:;}goto _LL81C;_LL820:{void*
_tmp677=_tmp670;struct Cyc_Absyn_TunionInfo _tmp678;void*_tmp679;struct Cyc_Absyn_Tuniondecl**
_tmp67A;struct Cyc_Absyn_Tuniondecl*_tmp67B;_LL82A: if((unsigned int)_tmp677 > 3?*((
int*)_tmp677)== 2: 0){_LL82E: _tmp678=((struct Cyc_Absyn_TunionType_struct*)_tmp677)->f1;
_LL82F: _tmp679=(void*)_tmp678.tunion_info;if(*((int*)_tmp679)== 1){_LL830:
_tmp67A=((struct Cyc_Absyn_KnownTunion_struct*)_tmp679)->f1;_tmp67B=*_tmp67A;goto
_LL82B;}else{goto _LL82C;}}else{goto _LL82C;}_LL82C: goto _LL82D;_LL82B: problem=Cyc_Absyn_qvar_cmp(
_tmp67B->name,Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL829;_LL82D: problem=1;
goto _LL829;_LL829:;}goto _LL81C;_LL81C:;}if(problem){({void*_tmp67C[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("format attribute and vararg types don't match",sizeof(
unsigned char),46),_tag_arr(_tmp67C,sizeof(void*),0));});}}}}{struct Cyc_List_List*
rpo=_tmp5C5;for(0;rpo != 0;rpo=rpo->tl){struct _tuple9 _tmp67E;void*_tmp67F;void*
_tmp680;struct _tuple9*_tmp67D=(struct _tuple9*)rpo->hd;_tmp67E=*_tmp67D;_LL833:
_tmp680=_tmp67E.f1;goto _LL832;_LL832: _tmp67F=_tmp67E.f2;goto _LL831;_LL831:
_tmp654=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp654,(void*)3,_tmp680);_tmp654=
Cyc_Tcutil_i_check_valid_type(loc,te,_tmp654,(void*)3,_tmp67F);}}if(*_tmp5CB != 0){
_tmp654=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp654,(void*)4,(void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp5CB))->v);}else{if(cvtenv.generalize_evars){;}{struct Cyc_List_List*
effect=0;{struct Cyc_List_List*tvs=_tmp654.free_vars;for(0;tvs != 0;tvs=tvs->tl){
void*_tmp681=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);
void*_tmp682;struct Cyc_Core_Opt*_tmp683;struct Cyc_Core_Opt**_tmp684;void*_tmp685;
void*_tmp686;struct Cyc_Core_Opt*_tmp687;struct Cyc_Core_Opt**_tmp688;void*_tmp689;
struct Cyc_Core_Opt*_tmp68A;struct Cyc_Core_Opt**_tmp68B;_LL835: if(*((int*)_tmp681)
== 2){_LL842: _tmp683=((struct Cyc_Absyn_Less_kb_struct*)_tmp681)->f1;_tmp684=(
struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp681)->f1;goto _LL841;
_LL841: _tmp682=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp681)->f2;if((int)
_tmp682 == 3){goto _LL836;}else{goto _LL837;}}else{goto _LL837;}_LL837: if(*((int*)
_tmp681)== 0){_LL843: _tmp685=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp681)->f1;
if((int)_tmp685 == 3){goto _LL838;}else{goto _LL839;}}else{goto _LL839;}_LL839: if(*((
int*)_tmp681)== 2){_LL845: _tmp687=((struct Cyc_Absyn_Less_kb_struct*)_tmp681)->f1;
_tmp688=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp681)->f1;
goto _LL844;_LL844: _tmp686=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp681)->f2;
if((int)_tmp686 == 4){goto _LL83A;}else{goto _LL83B;}}else{goto _LL83B;}_LL83B: if(*((
int*)_tmp681)== 0){_LL846: _tmp689=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp681)->f1;
if((int)_tmp689 == 4){goto _LL83C;}else{goto _LL83D;}}else{goto _LL83D;}_LL83D: if(*((
int*)_tmp681)== 1){_LL847: _tmp68A=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp681)->f1;
_tmp68B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp681)->f1;
goto _LL83E;}else{goto _LL83F;}_LL83F: goto _LL840;_LL836:*_tmp684=({struct Cyc_Core_Opt*
_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp68E;_tmp68E.tag=0;_tmp68E.f1=(void*)((void*)3);_tmp68E;});_tmp68D;}));
_tmp68C;});goto _LL838;_LL838: effect=({struct Cyc_List_List*_tmp68F=_cycalloc(
sizeof(*_tmp68F));_tmp68F->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp691;_tmp691.tag=17;_tmp691.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692[0]=({struct Cyc_Absyn_VarType_struct
_tmp693;_tmp693.tag=1;_tmp693.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp693;});
_tmp692;}));_tmp691;});_tmp690;}));_tmp68F->tl=effect;_tmp68F;});goto _LL834;
_LL83A:*_tmp688=({struct Cyc_Core_Opt*_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp695=_cycalloc(sizeof(*_tmp695));
_tmp695[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp696;_tmp696.tag=0;_tmp696.f1=(void*)((
void*)4);_tmp696;});_tmp695;}));_tmp694;});goto _LL83C;_LL83C: effect=({struct Cyc_List_List*
_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697->hd=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698[0]=({struct Cyc_Absyn_VarType_struct
_tmp699;_tmp699.tag=1;_tmp699.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp699;});
_tmp698;}));_tmp697->tl=effect;_tmp697;});goto _LL834;_LL83E:*_tmp68B=({struct Cyc_Core_Opt*
_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A->v=(void*)((void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp69C;_tmp69C.tag=2;_tmp69C.f1=0;_tmp69C.f2=(void*)((void*)0);_tmp69C;});
_tmp69B;}));_tmp69A;});goto _LL840;_LL840: effect=({struct Cyc_List_List*_tmp69D=
_cycalloc(sizeof(*_tmp69D));_tmp69D->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp69E=_cycalloc(sizeof(*_tmp69E));_tmp69E[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp69F;_tmp69F.tag=19;_tmp69F.f1=(void*)((void*)({struct Cyc_Absyn_VarType_struct*
_tmp6A0=_cycalloc(sizeof(*_tmp6A0));_tmp6A0[0]=({struct Cyc_Absyn_VarType_struct
_tmp6A1;_tmp6A1.tag=1;_tmp6A1.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6A1;});
_tmp6A0;}));_tmp69F;});_tmp69E;}));_tmp69D->tl=effect;_tmp69D;});goto _LL834;
_LL834:;}}effect=Cyc_List_imp_rev(effect);{struct Cyc_List_List*ts=_tmp654.free_evars;
for(0;ts != 0;ts=ts->tl){void*_tmp6A2=Cyc_Tcutil_typ_kind((void*)ts->hd);_LL849:
if((int)_tmp6A2 == 3){goto _LL84A;}else{goto _LL84B;}_LL84B: if((int)_tmp6A2 == 4){
goto _LL84C;}else{goto _LL84D;}_LL84D: goto _LL84E;_LL84A: effect=({struct Cyc_List_List*
_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp6A4=_cycalloc(sizeof(*_tmp6A4));_tmp6A4[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp6A5;_tmp6A5.tag=17;_tmp6A5.f1=(void*)((void*)ts->hd);_tmp6A5;});_tmp6A4;}));
_tmp6A3->tl=effect;_tmp6A3;});goto _LL848;_LL84C: effect=({struct Cyc_List_List*
_tmp6A6=_cycalloc(sizeof(*_tmp6A6));_tmp6A6->hd=(void*)((void*)ts->hd);_tmp6A6->tl=
effect;_tmp6A6;});goto _LL848;_LL84E: effect=({struct Cyc_List_List*_tmp6A7=
_cycalloc(sizeof(*_tmp6A7));_tmp6A7->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp6A8=_cycalloc(sizeof(*_tmp6A8));_tmp6A8[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp6A9;_tmp6A9.tag=19;_tmp6A9.f1=(void*)((void*)ts->hd);_tmp6A9;});_tmp6A8;}));
_tmp6A7->tl=effect;_tmp6A7;});goto _LL848;_LL848:;}}*_tmp5CB=({struct Cyc_Core_Opt*
_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp6AB=_cycalloc(sizeof(*_tmp6AB));_tmp6AB[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp6AC;_tmp6AC.tag=18;_tmp6AC.f1=Cyc_List_imp_rev(effect);_tmp6AC;});_tmp6AB;}));
_tmp6AA;});}}if(*_tmp5CD != 0){struct Cyc_List_List*bs=*_tmp5CD;for(0;bs != 0;bs=bs->tl){
void*_tmp6AD=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp6AE;struct Cyc_Core_Opt**_tmp6AF;void*_tmp6B0;struct Cyc_Core_Opt*
_tmp6B1;struct Cyc_Core_Opt**_tmp6B2;void*_tmp6B3;struct Cyc_Core_Opt*_tmp6B4;
struct Cyc_Core_Opt**_tmp6B5;void*_tmp6B6;struct Cyc_Core_Opt*_tmp6B7;struct Cyc_Core_Opt**
_tmp6B8;void*_tmp6B9;_LL850: if(*((int*)_tmp6AD)== 1){_LL85C: _tmp6AE=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp6AD)->f1;_tmp6AF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp6AD)->f1;goto _LL851;}else{goto _LL852;}_LL852: if(*((int*)_tmp6AD)== 2){_LL85E:
_tmp6B1=((struct Cyc_Absyn_Less_kb_struct*)_tmp6AD)->f1;_tmp6B2=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp6AD)->f1;goto _LL85D;_LL85D: _tmp6B0=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp6AD)->f2;if((int)_tmp6B0 == 1){goto _LL853;}
else{goto _LL854;}}else{goto _LL854;}_LL854: if(*((int*)_tmp6AD)== 2){_LL860:
_tmp6B4=((struct Cyc_Absyn_Less_kb_struct*)_tmp6AD)->f1;_tmp6B5=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp6AD)->f1;goto _LL85F;_LL85F: _tmp6B3=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp6AD)->f2;if((int)_tmp6B3 == 0){goto _LL855;}
else{goto _LL856;}}else{goto _LL856;}_LL856: if(*((int*)_tmp6AD)== 2){_LL862:
_tmp6B7=((struct Cyc_Absyn_Less_kb_struct*)_tmp6AD)->f1;_tmp6B8=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp6AD)->f1;goto _LL861;_LL861: _tmp6B6=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp6AD)->f2;goto _LL857;}else{goto _LL858;}_LL858:
if(*((int*)_tmp6AD)== 0){_LL863: _tmp6B9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp6AD)->f1;if((int)_tmp6B9 == 1){goto _LL859;}else{goto _LL85A;}}else{goto _LL85A;}
_LL85A: goto _LL85B;_LL851:({struct Cyc_Std_String_pa_struct _tmp6BB;_tmp6BB.tag=0;
_tmp6BB.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name;{void*
_tmp6BA[1]={& _tmp6BB};Cyc_Tcutil_warn(loc,_tag_arr("Type variable %s unconstrained, assuming boxed",
sizeof(unsigned char),47),_tag_arr(_tmp6BA,sizeof(void*),1));}});_tmp6B2=_tmp6AF;
goto _LL853;_LL853: _tmp6B5=_tmp6B2;goto _LL855;_LL855:*_tmp6B5=({struct Cyc_Core_Opt*
_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp6BE;_tmp6BE.tag=0;_tmp6BE.f1=(void*)((void*)2);_tmp6BE;});_tmp6BD;}));
_tmp6BC;});goto _LL84F;_LL857:*_tmp6B8=({struct Cyc_Core_Opt*_tmp6BF=_cycalloc(
sizeof(*_tmp6BF));_tmp6BF->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp6C0=_cycalloc(sizeof(*_tmp6C0));_tmp6C0[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp6C1;_tmp6C1.tag=0;_tmp6C1.f1=(void*)_tmp6B6;_tmp6C1;});_tmp6C0;}));_tmp6BF;});
goto _LL84F;_LL859:({void*_tmp6C2[0]={};Cyc_Tcutil_terr(loc,_tag_arr("function's can't abstract M types",
sizeof(unsigned char),34),_tag_arr(_tmp6C2,sizeof(void*),0));});goto _LL84F;
_LL85B: goto _LL84F;_LL84F:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(
_tmp654.kind_env,*_tmp5CD);_tmp654.free_vars=Cyc_Tcutil_remove_bound_tvars(
_tmp654.free_vars,*_tmp5CD);{struct Cyc_List_List*tvs=_tmp654.free_vars;for(0;tvs
!= 0;tvs=tvs->tl){cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar(cvtenv.free_vars,(
struct Cyc_Absyn_Tvar*)tvs->hd);}}{struct Cyc_List_List*evs=_tmp654.free_evars;
for(0;evs != 0;evs=evs->tl){cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.free_evars,(
void*)evs->hd);}}goto _LL750;}}_LL76E: for(0;_tmp5CE != 0;_tmp5CE=_tmp5CE->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)1,(*((struct _tuple4*)
_tmp5CE->hd)).f2);}goto _LL750;_LL770:{struct _RegionHandle _tmp6C4=_new_region("aprev_rgn");
struct _RegionHandle*aprev_rgn=& _tmp6C4;_push_region(aprev_rgn);{struct Cyc_List_List*
prev_fields=0;for(0;_tmp5CF != 0;_tmp5CF=_tmp5CF->tl){struct Cyc_Absyn_Aggrfield
_tmp6C6;struct Cyc_List_List*_tmp6C7;struct Cyc_Absyn_Exp*_tmp6C8;void*_tmp6C9;
struct Cyc_Absyn_Tqual _tmp6CA;struct _tagged_arr*_tmp6CB;struct Cyc_Absyn_Aggrfield*
_tmp6C5=(struct Cyc_Absyn_Aggrfield*)_tmp5CF->hd;_tmp6C6=*_tmp6C5;_LL869: _tmp6CB=
_tmp6C6.name;goto _LL868;_LL868: _tmp6CA=_tmp6C6.tq;goto _LL867;_LL867: _tmp6C9=(
void*)_tmp6C6.type;goto _LL866;_LL866: _tmp6C8=_tmp6C6.width;goto _LL865;_LL865:
_tmp6C7=_tmp6C6.attributes;goto _LL864;_LL864: if(((int(*)(int(*compare)(struct
_tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr*x))Cyc_List_mem)(
Cyc_Std_zstrptrcmp,prev_fields,_tmp6CB)){({struct Cyc_Std_String_pa_struct _tmp6CD;
_tmp6CD.tag=0;_tmp6CD.f1=(struct _tagged_arr)*_tmp6CB;{void*_tmp6CC[1]={& _tmp6CD};
Cyc_Tcutil_terr(loc,_tag_arr("duplicate field %s",sizeof(unsigned char),19),
_tag_arr(_tmp6CC,sizeof(void*),1));}});}if(Cyc_Std_strcmp(*_tmp6CB,_tag_arr("",
sizeof(unsigned char),1))!= 0){prev_fields=({struct Cyc_List_List*_tmp6CE=
_region_malloc(aprev_rgn,sizeof(*_tmp6CE));_tmp6CE->hd=_tmp6CB;_tmp6CE->tl=
prev_fields;_tmp6CE;});}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)
1,_tmp6C9);if(_tmp5D0 == (void*)1? ! Cyc_Tcutil_bits_only(_tmp6C9): 0){({struct Cyc_Std_String_pa_struct
_tmp6D0;_tmp6D0.tag=0;_tmp6D0.f1=(struct _tagged_arr)*_tmp6CB;{void*_tmp6CF[1]={&
_tmp6D0};Cyc_Tcutil_terr(loc,_tag_arr("union member %s has a non-integral type",
sizeof(unsigned char),40),_tag_arr(_tmp6CF,sizeof(void*),1));}});}Cyc_Tcutil_check_bitfield(
loc,te,_tmp6C9,_tmp6C8,_tmp6CB);Cyc_Tcutil_check_field_atts(loc,_tmp6CB,_tmp6C7);}};
_pop_region(aprev_rgn);}goto _LL750;_LL772:{void*_tmp6D1=*_tmp5D5;struct _tuple1*
_tmp6D2;void*_tmp6D3;struct Cyc_Absyn_Aggrdecl**_tmp6D4;struct Cyc_Absyn_Aggrdecl*
_tmp6D5;_LL86B: if(*((int*)_tmp6D1)== 0){_LL870: _tmp6D3=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp6D1)->f1;goto _LL86F;_LL86F: _tmp6D2=((struct Cyc_Absyn_UnknownAggr_struct*)
_tmp6D1)->f2;goto _LL86C;}else{goto _LL86D;}_LL86D: if(*((int*)_tmp6D1)== 1){_LL871:
_tmp6D4=((struct Cyc_Absyn_KnownAggr_struct*)_tmp6D1)->f1;_tmp6D5=*_tmp6D4;goto
_LL86E;}else{goto _LL86A;}_LL86C: {struct Cyc_Absyn_Aggrdecl**adp;{struct
_handler_cons _tmp6D6;_push_handler(& _tmp6D6);{int _tmp6D8=0;if(setjmp(_tmp6D6.handler)){
_tmp6D8=1;}if(! _tmp6D8){adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp6D2);*_tmp5D5=(
void*)({struct Cyc_Absyn_KnownAggr_struct*_tmp6D9=_cycalloc(sizeof(*_tmp6D9));
_tmp6D9[0]=({struct Cyc_Absyn_KnownAggr_struct _tmp6DA;_tmp6DA.tag=1;_tmp6DA.f1=
adp;_tmp6DA;});_tmp6D9;});;_pop_handler();}else{void*_tmp6D7=(void*)_exn_thrown;
void*_tmp6DC=_tmp6D7;_LL873: if(_tmp6DC == Cyc_Dict_Absent){goto _LL874;}else{goto
_LL875;}_LL875: goto _LL876;_LL874: {struct Cyc_Tcenv_Genv*_tmp6DD=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp6DE=({struct Cyc_Absyn_Aggrdecl*_tmp6E3=_cycalloc(
sizeof(*_tmp6E3));_tmp6E3->kind=(void*)_tmp6D3;_tmp6E3->sc=(void*)((void*)3);
_tmp6E3->name=_tmp6D2;_tmp6E3->tvs=0;_tmp6E3->exist_vars=0;_tmp6E3->fields=0;
_tmp6E3->attributes=0;_tmp6E3;});Cyc_Tc_tcAggrdecl(te,_tmp6DD,loc,_tmp6DE);adp=
Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp6D2);*_tmp5D5=(void*)({struct Cyc_Absyn_KnownAggr_struct*
_tmp6DF=_cycalloc(sizeof(*_tmp6DF));_tmp6DF[0]=({struct Cyc_Absyn_KnownAggr_struct
_tmp6E0;_tmp6E0.tag=1;_tmp6E0.f1=adp;_tmp6E0;});_tmp6DF;});if(*_tmp5D3 != 0){({
struct Cyc_Std_String_pa_struct _tmp6E2;_tmp6E2.tag=0;_tmp6E2.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp6D2);{void*_tmp6E1[1]={& _tmp6E2};Cyc_Tcutil_terr(
loc,_tag_arr("declare parameterized type %s before using",sizeof(unsigned char),
43),_tag_arr(_tmp6E1,sizeof(void*),1));}});return cvtenv;}goto _LL872;}_LL876:(
void)_throw(_tmp6DC);_LL872:;}}}_tmp6D5=*adp;goto _LL86E;}_LL86E: {struct Cyc_List_List*
tvs=_tmp6D5->tvs;struct Cyc_List_List*ts=*_tmp5D3;for(0;ts != 0? tvs != 0: 0;(ts=ts->tl,
tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd);}if(ts != 0){({
struct Cyc_Std_String_pa_struct _tmp6E5;_tmp6E5.tag=0;_tmp6E5.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(_tmp6D5->name);{void*_tmp6E4[1]={& _tmp6E5};
Cyc_Tcutil_terr(loc,_tag_arr("too many parameters for type %s",sizeof(
unsigned char),32),_tag_arr(_tmp6E4,sizeof(void*),1));}});}if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct Cyc_List_List*_tmp6E6=
_cycalloc(sizeof(*_tmp6E6));_tmp6E6->hd=(void*)e;_tmp6E6->tl=hidden_ts;_tmp6E6;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);}*_tmp5D3=Cyc_List_imp_append(*
_tmp5D3,Cyc_List_imp_rev(hidden_ts));}}_LL86A:;}goto _LL750;_LL774: {struct Cyc_List_List*
targs=*_tmp5D9;struct Cyc_Absyn_Typedefdecl*td;{struct _handler_cons _tmp6E7;
_push_handler(& _tmp6E7);{int _tmp6E9=0;if(setjmp(_tmp6E7.handler)){_tmp6E9=1;}if(
! _tmp6E9){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp5DA);;_pop_handler();}else{
void*_tmp6E8=(void*)_exn_thrown;void*_tmp6EB=_tmp6E8;_LL878: if(_tmp6EB == Cyc_Dict_Absent){
goto _LL879;}else{goto _LL87A;}_LL87A: goto _LL87B;_LL879:({struct Cyc_Std_String_pa_struct
_tmp6ED;_tmp6ED.tag=0;_tmp6ED.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(
_tmp5DA);{void*_tmp6EC[1]={& _tmp6ED};Cyc_Tcutil_terr(loc,_tag_arr("unbound typedef name %s",
sizeof(unsigned char),24),_tag_arr(_tmp6EC,sizeof(void*),1));}});return cvtenv;
_LL87B:(void)_throw(_tmp6EB);_LL877:;}}}_tmp5DA[0]=(td->name)[
_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*tvs=td->tvs;struct Cyc_List_List*
ts=targs;struct Cyc_List_List*inst=0;for(0;ts != 0? tvs != 0: 0;(ts=ts->tl,tvs=tvs->tl)){
void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k,(void*)ts->hd);inst=({struct Cyc_List_List*_tmp6EE=_cycalloc(
sizeof(*_tmp6EE));_tmp6EE->hd=({struct _tuple6*_tmp6EF=_cycalloc(sizeof(*_tmp6EF));
_tmp6EF->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp6EF->f2=(void*)ts->hd;_tmp6EF;});
_tmp6EE->tl=inst;_tmp6EE;});}if(ts != 0){({struct Cyc_Std_String_pa_struct _tmp6F1;
_tmp6F1.tag=0;_tmp6F1.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp5DA);{
void*_tmp6F0[1]={& _tmp6F1};Cyc_Tcutil_terr(loc,_tag_arr("too many parameters for typedef %s",
sizeof(unsigned char),35),_tag_arr(_tmp6F0,sizeof(void*),1));}});}if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);hidden_ts=({struct
Cyc_List_List*_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2->hd=(void*)e;_tmp6F2->tl=
hidden_ts;_tmp6F2;});cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e);
inst=({struct Cyc_List_List*_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3->hd=({
struct _tuple6*_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4->f1=(struct Cyc_Absyn_Tvar*)
tvs->hd;_tmp6F4->f2=e;_tmp6F4;});_tmp6F3->tl=inst;_tmp6F3;});}*_tmp5D9=Cyc_List_imp_append(
targs,Cyc_List_imp_rev(hidden_ts));}{void*new_typ=Cyc_Tcutil_substitute(inst,(
void*)td->defn);*_tmp5D7=({struct Cyc_Core_Opt*_tmp6F5=_cycalloc(sizeof(*_tmp6F5));
_tmp6F5->v=(void*)new_typ;_tmp6F5;});goto _LL750;}}}_LL776: goto _LL750;_LL778:
_tmp5DC=_tmp5DB;goto _LL77A;_LL77A: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)3,_tmp5DC);goto _LL750;_LL77C: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp5DD);goto _LL750;_LL77E: for(0;_tmp5DE != 0;_tmp5DE=
_tmp5DE->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)4,(void*)
_tmp5DE->hd);}goto _LL750;_LL750:;}if(! Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),
expected_kind)){({struct Cyc_Std_String_pa_struct _tmp6F9;_tmp6F9.tag=0;_tmp6F9.f1=(
struct _tagged_arr)Cyc_Absynpp_kind2string(expected_kind);{struct Cyc_Std_String_pa_struct
_tmp6F8;_tmp6F8.tag=0;_tmp6F8.f1=(struct _tagged_arr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(
t));{struct Cyc_Std_String_pa_struct _tmp6F7;_tmp6F7.tag=0;_tmp6F7.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp6F6[3]={& _tmp6F7,& _tmp6F8,&
_tmp6F9};Cyc_Tcutil_terr(loc,_tag_arr("type %s has kind %s but as used here needs kind %s",
sizeof(unsigned char),51),_tag_arr(_tmp6F6,sizeof(void*),3));}}}});}return cvtenv;}
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*kind_env,void*expected_kind,int
generalize_evars,void*t){struct Cyc_Tcutil_CVTEnv _tmp6FA=Cyc_Tcutil_i_check_valid_type(
loc,te,({struct Cyc_Tcutil_CVTEnv _tmp703;_tmp703.kind_env=kind_env;_tmp703.free_vars=
0;_tmp703.free_evars=0;_tmp703.generalize_evars=generalize_evars;_tmp703.fn_result=
0;_tmp703;}),expected_kind,t);{struct Cyc_List_List*vs=_tmp6FA.free_vars;for(0;vs
!= 0;vs=vs->tl){_tmp6FA.kind_env=Cyc_Tcutil_fast_add_free_tvar(kind_env,(struct
Cyc_Absyn_Tvar*)vs->hd);}}{struct Cyc_List_List*evs=_tmp6FA.free_evars;for(0;evs
!= 0;evs=evs->tl){void*_tmp6FB=Cyc_Tcutil_compress((void*)evs->hd);struct Cyc_Core_Opt*
_tmp6FC;struct Cyc_Core_Opt**_tmp6FD;_LL87D: if((unsigned int)_tmp6FB > 3?*((int*)
_tmp6FB)== 0: 0){_LL881: _tmp6FC=((struct Cyc_Absyn_Evar_struct*)_tmp6FB)->f4;
_tmp6FD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp6FB)->f4;goto
_LL87E;}else{goto _LL87F;}_LL87F: goto _LL880;_LL87E: if(*_tmp6FD == 0){*_tmp6FD=({
struct Cyc_Core_Opt*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE->v=kind_env;
_tmp6FE;});}else{struct Cyc_List_List*_tmp6FF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp6FD))->v;struct Cyc_List_List*_tmp700=0;for(0;_tmp6FF != 0;
_tmp6FF=_tmp6FF->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
kind_env,(struct Cyc_Absyn_Tvar*)_tmp6FF->hd)){_tmp700=({struct Cyc_List_List*
_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701->hd=(struct Cyc_Absyn_Tvar*)_tmp6FF->hd;
_tmp701->tl=_tmp700;_tmp701;});}}*_tmp6FD=({struct Cyc_Core_Opt*_tmp702=_cycalloc(
sizeof(*_tmp702));_tmp702->v=_tmp700;_tmp702;});}goto _LL87C;_LL880: goto _LL87C;
_LL87C:;}}return _tmp6FA;}void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,void*t){int generalize_evars=Cyc_Tcutil_is_function_type(
t);struct Cyc_Tcutil_CVTEnv _tmp704=Cyc_Tcutil_check_valid_type(loc,te,0,(void*)1,
generalize_evars,t);struct Cyc_List_List*_tmp705=_tmp704.free_vars;struct Cyc_List_List*
_tmp706=_tmp704.free_evars;{struct Cyc_List_List*x=_tmp705;for(0;x != 0;x=x->tl){
void*_tmp707=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);
struct Cyc_Core_Opt*_tmp708;struct Cyc_Core_Opt**_tmp709;void*_tmp70A;struct Cyc_Core_Opt*
_tmp70B;struct Cyc_Core_Opt**_tmp70C;void*_tmp70D;struct Cyc_Core_Opt*_tmp70E;
struct Cyc_Core_Opt**_tmp70F;void*_tmp710;struct Cyc_Core_Opt*_tmp711;struct Cyc_Core_Opt**
_tmp712;void*_tmp713;_LL883: if(*((int*)_tmp707)== 1){_LL88F: _tmp708=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp707)->f1;_tmp709=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp707)->f1;goto _LL884;}else{goto _LL885;}_LL885: if(*((int*)_tmp707)== 2){_LL891:
_tmp70B=((struct Cyc_Absyn_Less_kb_struct*)_tmp707)->f1;_tmp70C=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp707)->f1;goto _LL890;_LL890: _tmp70A=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp707)->f2;if((int)_tmp70A == 1){goto _LL886;}
else{goto _LL887;}}else{goto _LL887;}_LL887: if(*((int*)_tmp707)== 2){_LL893:
_tmp70E=((struct Cyc_Absyn_Less_kb_struct*)_tmp707)->f1;_tmp70F=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp707)->f1;goto _LL892;_LL892: _tmp70D=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp707)->f2;if((int)_tmp70D == 0){goto _LL888;}
else{goto _LL889;}}else{goto _LL889;}_LL889: if(*((int*)_tmp707)== 2){_LL895:
_tmp711=((struct Cyc_Absyn_Less_kb_struct*)_tmp707)->f1;_tmp712=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp707)->f1;goto _LL894;_LL894: _tmp710=(void*)((
struct Cyc_Absyn_Less_kb_struct*)_tmp707)->f2;goto _LL88A;}else{goto _LL88B;}_LL88B:
if(*((int*)_tmp707)== 0){_LL896: _tmp713=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp707)->f1;if((int)_tmp713 == 1){goto _LL88C;}else{goto _LL88D;}}else{goto _LL88D;}
_LL88D: goto _LL88E;_LL884: _tmp70C=_tmp709;goto _LL886;_LL886: _tmp70F=_tmp70C;goto
_LL888;_LL888:*_tmp70F=({struct Cyc_Core_Opt*_tmp714=_cycalloc(sizeof(*_tmp714));
_tmp714->v=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp715=_cycalloc(
sizeof(*_tmp715));_tmp715[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp716;_tmp716.tag=0;
_tmp716.f1=(void*)((void*)2);_tmp716;});_tmp715;}));_tmp714;});goto _LL882;_LL88A:*
_tmp712=({struct Cyc_Core_Opt*_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717->v=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp718=_cycalloc(sizeof(*_tmp718));
_tmp718[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp719;_tmp719.tag=0;_tmp719.f1=(void*)
_tmp710;_tmp719;});_tmp718;}));_tmp717;});goto _LL882;_LL88C:({struct Cyc_Std_String_pa_struct
_tmp71B;_tmp71B.tag=0;_tmp71B.f1=(struct _tagged_arr)Cyc_Tcutil_tvar2string((
struct Cyc_Absyn_Tvar*)x->hd);{void*_tmp71A[1]={& _tmp71B};Cyc_Tcutil_terr(loc,
_tag_arr("type variable %s cannot have kind M",sizeof(unsigned char),36),
_tag_arr(_tmp71A,sizeof(void*),1));}});goto _LL882;_LL88E: goto _LL882;_LL882:;}}
if(_tmp705 != 0? 1: _tmp706 != 0){{void*_tmp71C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo
_tmp71D;struct Cyc_List_List*_tmp71E;struct Cyc_List_List*_tmp71F;struct Cyc_Absyn_VarargInfo*
_tmp720;int _tmp721;struct Cyc_List_List*_tmp722;void*_tmp723;struct Cyc_Core_Opt*
_tmp724;struct Cyc_List_List*_tmp725;struct Cyc_List_List**_tmp726;_LL898: if((
unsigned int)_tmp71C > 3?*((int*)_tmp71C)== 8: 0){_LL89C: _tmp71D=((struct Cyc_Absyn_FnType_struct*)
_tmp71C)->f1;_LL8A4: _tmp725=_tmp71D.tvars;_tmp726=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp71C)->f1).tvars;goto _LL8A3;_LL8A3: _tmp724=
_tmp71D.effect;goto _LL8A2;_LL8A2: _tmp723=(void*)_tmp71D.ret_typ;goto _LL8A1;
_LL8A1: _tmp722=_tmp71D.args;goto _LL8A0;_LL8A0: _tmp721=_tmp71D.c_varargs;goto
_LL89F;_LL89F: _tmp720=_tmp71D.cyc_varargs;goto _LL89E;_LL89E: _tmp71F=_tmp71D.rgn_po;
goto _LL89D;_LL89D: _tmp71E=_tmp71D.attributes;goto _LL899;}else{goto _LL89A;}_LL89A:
goto _LL89B;_LL899: if(*_tmp726 == 0){*_tmp726=_tmp705;_tmp705=0;}goto _LL897;_LL89B:
goto _LL897;_LL897:;}if(_tmp705 != 0){({struct Cyc_Std_String_pa_struct _tmp728;
_tmp728.tag=0;_tmp728.f1=(struct _tagged_arr)*((struct Cyc_Absyn_Tvar*)_tmp705->hd)->name;{
void*_tmp727[1]={& _tmp728};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s",
sizeof(unsigned char),25),_tag_arr(_tmp727,sizeof(void*),1));}});}if(_tmp706 != 0){
for(0;_tmp706 != 0;_tmp706=_tmp706->tl){void*e=(void*)_tmp706->hd;void*_tmp729=
Cyc_Tcutil_typ_kind(e);_LL8A6: if((int)_tmp729 == 3){goto _LL8A7;}else{goto _LL8A8;}
_LL8A8: if((int)_tmp729 == 4){goto _LL8A9;}else{goto _LL8AA;}_LL8AA: goto _LL8AB;
_LL8A7: if(! Cyc_Tcutil_unify(e,(void*)2)){({void*_tmp72A[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with heap!",
sizeof(unsigned char),28),_tag_arr(_tmp72A,sizeof(void*),0));});}goto _LL8A5;
_LL8A9: if(! Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){({void*_tmp72B[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with {}!",
sizeof(unsigned char),26),_tag_arr(_tmp72B,sizeof(void*),0));});}goto _LL8A5;
_LL8AB:({struct Cyc_Std_String_pa_struct _tmp72E;_tmp72E.tag=0;_tmp72E.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp72D;
_tmp72D.tag=0;_tmp72D.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{void*
_tmp72C[2]={& _tmp72D,& _tmp72E};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(unsigned char),52),_tag_arr(_tmp72C,sizeof(void*),2));}}});goto _LL8A5;
_LL8A5:;}}}}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){void*t=Cyc_Tcutil_fndecl2typ(
fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{void*_tmp72F=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp730;struct Cyc_List_List*_tmp731;struct Cyc_List_List*
_tmp732;struct Cyc_Absyn_VarargInfo*_tmp733;int _tmp734;struct Cyc_List_List*
_tmp735;void*_tmp736;struct Cyc_Core_Opt*_tmp737;struct Cyc_List_List*_tmp738;
_LL8AD: if((unsigned int)_tmp72F > 3?*((int*)_tmp72F)== 8: 0){_LL8B1: _tmp730=((
struct Cyc_Absyn_FnType_struct*)_tmp72F)->f1;_LL8B9: _tmp738=_tmp730.tvars;goto
_LL8B8;_LL8B8: _tmp737=_tmp730.effect;goto _LL8B7;_LL8B7: _tmp736=(void*)_tmp730.ret_typ;
goto _LL8B6;_LL8B6: _tmp735=_tmp730.args;goto _LL8B5;_LL8B5: _tmp734=_tmp730.c_varargs;
goto _LL8B4;_LL8B4: _tmp733=_tmp730.cyc_varargs;goto _LL8B3;_LL8B3: _tmp732=_tmp730.rgn_po;
goto _LL8B2;_LL8B2: _tmp731=_tmp730.attributes;goto _LL8AE;}else{goto _LL8AF;}_LL8AF:
goto _LL8B0;_LL8AE: fd->tvs=_tmp738;fd->effect=_tmp737;goto _LL8AC;_LL8B0:({void*
_tmp739[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("check_fndecl_valid_type: not a FnType",sizeof(unsigned char),38),
_tag_arr(_tmp739,sizeof(void*),0));});_LL8AC:;}{struct _RegionHandle _tmp73A=
_new_region("r");struct _RegionHandle*r=& _tmp73A;_push_region(r);Cyc_Tcutil_check_unique_vars(((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tagged_arr*(*f)(struct
_tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _tagged_arr*(*)(struct
_tuple14*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,_tag_arr("function declaration has repeated parameter",
sizeof(unsigned char),44));;_pop_region(r);}fd->cached_typ=({struct Cyc_Core_Opt*
_tmp73B=_cycalloc(sizeof(*_tmp73B));_tmp73B->v=(void*)t;_tmp73B;});}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct Cyc_Tcutil_CVTEnv
_tmp73C=Cyc_Tcutil_check_valid_type(loc,te,bound_tvars,expected_kind,0,t);struct
Cyc_List_List*_tmp73D=Cyc_Tcutil_remove_bound_tvars(_tmp73C.free_vars,
bound_tvars);struct Cyc_List_List*_tmp73E=_tmp73C.free_evars;{struct Cyc_List_List*
fs=_tmp73D;for(0;fs != 0;fs=fs->tl){struct _tagged_arr*_tmp73F=((struct Cyc_Absyn_Tvar*)
fs->hd)->name;({struct Cyc_Std_String_pa_struct _tmp742;_tmp742.tag=0;_tmp742.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct
_tmp741;_tmp741.tag=0;_tmp741.f1=(struct _tagged_arr)*_tmp73F;{void*_tmp740[2]={&
_tmp741,& _tmp742};Cyc_Tcutil_terr(loc,_tag_arr("unbound type variable %s in type %s",
sizeof(unsigned char),36),_tag_arr(_tmp740,sizeof(void*),2));}}});}}if(!
allow_evars? _tmp73E != 0: 0){for(0;_tmp73E != 0;_tmp73E=_tmp73E->tl){void*e=(void*)
_tmp73E->hd;void*_tmp743=Cyc_Tcutil_typ_kind(e);_LL8BB: if((int)_tmp743 == 3){goto
_LL8BC;}else{goto _LL8BD;}_LL8BD: if((int)_tmp743 == 4){goto _LL8BE;}else{goto _LL8BF;}
_LL8BF: goto _LL8C0;_LL8BC: if(! Cyc_Tcutil_unify(e,(void*)2)){({void*_tmp744[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with heap!",
sizeof(unsigned char),28),_tag_arr(_tmp744,sizeof(void*),0));});}goto _LL8BA;
_LL8BE: if(! Cyc_Tcutil_unify(e,(void*)({struct Cyc_Absyn_JoinEff_struct*_tmp745=
_cycalloc(sizeof(*_tmp745));_tmp745[0]=({struct Cyc_Absyn_JoinEff_struct _tmp746;
_tmp746.tag=18;_tmp746.f1=0;_tmp746;});_tmp745;}))){({void*_tmp747[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("can't unify evar with {}!",
sizeof(unsigned char),26),_tag_arr(_tmp747,sizeof(void*),0));});}goto _LL8BA;
_LL8C0:({struct Cyc_Std_String_pa_struct _tmp74A;_tmp74A.tag=0;_tmp74A.f1=(struct
_tagged_arr)Cyc_Absynpp_typ2string(t);{struct Cyc_Std_String_pa_struct _tmp749;
_tmp749.tag=0;_tmp749.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(e);{void*
_tmp748[2]={& _tmp749,& _tmp74A};Cyc_Tcutil_terr(loc,_tag_arr("hidden type variable %s isn't abstracted in type %s",
sizeof(unsigned char),52),_tag_arr(_tmp748,sizeof(void*),2));}}});goto _LL8BA;
_LL8BA:;}}}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity
== 0){tv->identity=Cyc_Tcutil_new_tvar_id();}}void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}static void Cyc_Tcutil_check_unique_unsorted(
int(*cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,
struct _tagged_arr(*a2string)(void*),struct _tagged_arr msg){for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(
void*)vs2->hd)== 0){({struct Cyc_Std_String_pa_struct _tmp74D;_tmp74D.tag=0;
_tmp74D.f1=(struct _tagged_arr)a2string((void*)vs->hd);{struct Cyc_Std_String_pa_struct
_tmp74C;_tmp74C.tag=0;_tmp74C.f1=(struct _tagged_arr)msg;{void*_tmp74B[2]={&
_tmp74C,& _tmp74D};Cyc_Tcutil_terr(loc,_tag_arr("%s: %s",sizeof(unsigned char),7),
_tag_arr(_tmp74B,sizeof(void*),2));}}});}}}}static struct _tagged_arr Cyc_Tcutil_strptr2string(
struct _tagged_arr*s){return*s;}void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _tagged_arr msg){((void(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr(*a2string)(struct _tagged_arr*),struct _tagged_arr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Std_zstrptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs){((void(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _tagged_arr(*a2string)(struct Cyc_Absyn_Tvar*),struct _tagged_arr msg))
Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,
_tag_arr("duplicate type variable",sizeof(unsigned char),24));}struct _tuple19{
struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple20{struct Cyc_List_List*f1;void*
f2;};struct _tuple21{struct Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){if(Cyc_Std_strcmp(*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,
_tag_arr("",sizeof(unsigned char),1))!= 0){fields=({struct Cyc_List_List*_tmp74E=
_cycalloc(sizeof(*_tmp74E));_tmp74E->hd=({struct _tuple19*_tmp74F=_cycalloc(
sizeof(*_tmp74F));_tmp74F->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp74F->f2=
0;_tmp74F;});_tmp74E->tl=fields;_tmp74E;});}}}fields=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{struct Cyc_List_List*ans=0;for(
0;des != 0;des=des->tl){struct _tuple20 _tmp751;void*_tmp752;struct Cyc_List_List*
_tmp753;struct _tuple20*_tmp750=(struct _tuple20*)des->hd;_tmp751=*_tmp750;_LL8C3:
_tmp753=_tmp751.f1;goto _LL8C2;_LL8C2: _tmp752=_tmp751.f2;goto _LL8C1;_LL8C1: if(
_tmp753 == 0){struct Cyc_List_List*_tmp754=fields;for(0;_tmp754 != 0;_tmp754=
_tmp754->tl){if(!(*((struct _tuple19*)_tmp754->hd)).f2){(*((struct _tuple19*)
_tmp754->hd)).f2=1;(*((struct _tuple20*)des->hd)).f1=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp755=_cycalloc(sizeof(*_tmp755));_tmp755->hd=(void*)((
void*)({struct Cyc_Absyn_FieldName_struct*_tmp756=_cycalloc(sizeof(*_tmp756));
_tmp756[0]=({struct Cyc_Absyn_FieldName_struct _tmp757;_tmp757.tag=1;_tmp757.f1=((*((
struct _tuple19*)_tmp754->hd)).f1)->name;_tmp757;});_tmp756;}));_tmp755->tl=0;
_tmp755;});ans=({struct Cyc_List_List*_tmp758=_region_malloc(rgn,sizeof(*_tmp758));
_tmp758->hd=({struct _tuple21*_tmp759=_region_malloc(rgn,sizeof(*_tmp759));
_tmp759->f1=(*((struct _tuple19*)_tmp754->hd)).f1;_tmp759->f2=_tmp752;_tmp759;});
_tmp758->tl=ans;_tmp758;});break;}}if(_tmp754 == 0){({void*_tmp75A[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("too many arguments to struct",sizeof(unsigned char),29),_tag_arr(
_tmp75A,sizeof(void*),0));});}}else{if(_tmp753->tl != 0){({void*_tmp75B[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("multiple designators are not supported",sizeof(unsigned char),39),
_tag_arr(_tmp75B,sizeof(void*),0));});}else{void*_tmp75C=(void*)_tmp753->hd;
struct _tagged_arr*_tmp75D;_LL8C5: if(*((int*)_tmp75C)== 0){goto _LL8C6;}else{goto
_LL8C7;}_LL8C7: if(*((int*)_tmp75C)== 1){_LL8C9: _tmp75D=((struct Cyc_Absyn_FieldName_struct*)
_tmp75C)->f1;goto _LL8C8;}else{goto _LL8C4;}_LL8C6:({void*_tmp75E[0]={};Cyc_Tcutil_terr(
loc,_tag_arr("array designator used in argument to struct",sizeof(unsigned char),
44),_tag_arr(_tmp75E,sizeof(void*),0));});goto _LL8C4;_LL8C8: {struct Cyc_List_List*
_tmp75F=fields;for(0;_tmp75F != 0;_tmp75F=_tmp75F->tl){if(Cyc_Std_zstrptrcmp(
_tmp75D,((*((struct _tuple19*)_tmp75F->hd)).f1)->name)== 0){if((*((struct _tuple19*)
_tmp75F->hd)).f2){({struct Cyc_Std_String_pa_struct _tmp761;_tmp761.tag=0;_tmp761.f1=(
struct _tagged_arr)*_tmp75D;{void*_tmp760[1]={& _tmp761};Cyc_Tcutil_terr(loc,
_tag_arr("field %s has already been used as an argument",sizeof(unsigned char),
46),_tag_arr(_tmp760,sizeof(void*),1));}});}(*((struct _tuple19*)_tmp75F->hd)).f2=
1;ans=({struct Cyc_List_List*_tmp762=_region_malloc(rgn,sizeof(*_tmp762));_tmp762->hd=({
struct _tuple21*_tmp763=_region_malloc(rgn,sizeof(*_tmp763));_tmp763->f1=(*((
struct _tuple19*)_tmp75F->hd)).f1;_tmp763->f2=_tmp752;_tmp763;});_tmp762->tl=ans;
_tmp762;});break;}}if(_tmp75F == 0){({struct Cyc_Std_String_pa_struct _tmp765;
_tmp765.tag=0;_tmp765.f1=(struct _tagged_arr)*_tmp75D;{void*_tmp764[1]={& _tmp765};
Cyc_Tcutil_terr(loc,_tag_arr("bad field designator %s",sizeof(unsigned char),24),
_tag_arr(_tmp764,sizeof(void*),1));}});}goto _LL8C4;}_LL8C4:;}}}for(0;fields != 0;
fields=fields->tl){if(!(*((struct _tuple19*)fields->hd)).f2){({void*_tmp766[0]={};
Cyc_Tcutil_terr(loc,_tag_arr("too few arguments to struct",sizeof(unsigned char),
28),_tag_arr(_tmp766,sizeof(void*),0));});break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest){void*_tmp767=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp768;struct Cyc_Absyn_Conref*_tmp769;void*_tmp76A;_LL8CB: if((unsigned int)
_tmp767 > 3?*((int*)_tmp767)== 4: 0){_LL8CF: _tmp768=((struct Cyc_Absyn_PointerType_struct*)
_tmp767)->f1;_LL8D1: _tmp76A=(void*)_tmp768.elt_typ;goto _LL8D0;_LL8D0: _tmp769=
_tmp768.bounds;goto _LL8CC;}else{goto _LL8CD;}_LL8CD: goto _LL8CE;_LL8CC: {struct Cyc_Absyn_Conref*
_tmp76B=Cyc_Absyn_compress_conref(_tmp769);void*_tmp76C=(void*)(Cyc_Absyn_compress_conref(
_tmp76B))->v;void*_tmp76D;_LL8D3: if((unsigned int)_tmp76C > 1?*((int*)_tmp76C)== 
0: 0){_LL8D9: _tmp76D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp76C)->f1;if((
int)_tmp76D == 0){goto _LL8D4;}else{goto _LL8D5;}}else{goto _LL8D5;}_LL8D5: if((int)
_tmp76C == 0){goto _LL8D6;}else{goto _LL8D7;}_LL8D7: goto _LL8D8;_LL8D4:*elt_typ_dest=
_tmp76A;return 1;_LL8D6:(void*)(_tmp76B->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*
_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E[0]=({struct Cyc_Absyn_Eq_constr_struct
_tmp76F;_tmp76F.tag=0;_tmp76F.f1=(void*)((void*)0);_tmp76F;});_tmp76E;})));*
elt_typ_dest=_tmp76A;return 1;_LL8D8: return 0;_LL8D2:;}_LL8CE: return 0;_LL8CA:;}int
Cyc_Tcutil_is_tagged_pointer_typ(void*t){void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(
t,& ignore);}struct _tuple8 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){struct _tuple8 bogus_ans=({struct _tuple8 _tmp7B1;_tmp7B1.f1=
0;_tmp7B1.f2=(void*)2;_tmp7B1;});void*_tmp770=(void*)e->r;void*_tmp771;struct
_tuple1*_tmp772;struct _tagged_arr*_tmp773;struct Cyc_Absyn_Exp*_tmp774;struct
_tagged_arr*_tmp775;struct Cyc_Absyn_Exp*_tmp776;struct Cyc_Absyn_Exp*_tmp777;
struct Cyc_Absyn_Exp*_tmp778;struct Cyc_Absyn_Exp*_tmp779;_LL8DB: if(*((int*)
_tmp770)== 1){_LL8E8: _tmp772=((struct Cyc_Absyn_Var_e_struct*)_tmp770)->f1;goto
_LL8E7;_LL8E7: _tmp771=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp770)->f2;goto
_LL8DC;}else{goto _LL8DD;}_LL8DD: if(*((int*)_tmp770)== 21){_LL8EA: _tmp774=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp770)->f1;goto _LL8E9;_LL8E9: _tmp773=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmp770)->f2;goto _LL8DE;}else{goto _LL8DF;}
_LL8DF: if(*((int*)_tmp770)== 22){_LL8EC: _tmp776=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp770)->f1;goto _LL8EB;_LL8EB: _tmp775=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp770)->f2;goto _LL8E0;}else{goto _LL8E1;}_LL8E1: if(*((int*)_tmp770)== 20){
_LL8ED: _tmp777=((struct Cyc_Absyn_Deref_e_struct*)_tmp770)->f1;goto _LL8E2;}else{
goto _LL8E3;}_LL8E3: if(*((int*)_tmp770)== 23){_LL8EF: _tmp779=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp770)->f1;goto _LL8EE;_LL8EE: _tmp778=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp770)->f2;goto _LL8E4;}else{goto _LL8E5;}_LL8E5: goto _LL8E6;_LL8DC: {void*
_tmp77A=_tmp771;struct Cyc_Absyn_Vardecl*_tmp77B;struct Cyc_Absyn_Vardecl*_tmp77C;
struct Cyc_Absyn_Vardecl*_tmp77D;struct Cyc_Absyn_Vardecl*_tmp77E;_LL8F1: if((int)
_tmp77A == 0){goto _LL8F2;}else{goto _LL8F3;}_LL8F3: if((unsigned int)_tmp77A > 1?*((
int*)_tmp77A)== 1: 0){goto _LL8F4;}else{goto _LL8F5;}_LL8F5: if((unsigned int)
_tmp77A > 1?*((int*)_tmp77A)== 0: 0){_LL8FD: _tmp77B=((struct Cyc_Absyn_Global_b_struct*)
_tmp77A)->f1;goto _LL8F6;}else{goto _LL8F7;}_LL8F7: if((unsigned int)_tmp77A > 1?*((
int*)_tmp77A)== 3: 0){_LL8FE: _tmp77C=((struct Cyc_Absyn_Local_b_struct*)_tmp77A)->f1;
goto _LL8F8;}else{goto _LL8F9;}_LL8F9: if((unsigned int)_tmp77A > 1?*((int*)_tmp77A)
== 4: 0){_LL8FF: _tmp77D=((struct Cyc_Absyn_Pat_b_struct*)_tmp77A)->f1;goto _LL8FA;}
else{goto _LL8FB;}_LL8FB: if((unsigned int)_tmp77A > 1?*((int*)_tmp77A)== 2: 0){
_LL900: _tmp77E=((struct Cyc_Absyn_Param_b_struct*)_tmp77A)->f1;goto _LL8FC;}else{
goto _LL8F0;}_LL8F2: return bogus_ans;_LL8F4: return({struct _tuple8 _tmp77F;_tmp77F.f1=
0;_tmp77F.f2=(void*)2;_tmp77F;});_LL8F6: {void*_tmp780=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL902: if((unsigned int)_tmp780 > 3?*((
int*)_tmp780)== 7: 0){goto _LL903;}else{goto _LL904;}_LL904: goto _LL905;_LL903:
return({struct _tuple8 _tmp781;_tmp781.f1=1;_tmp781.f2=(void*)2;_tmp781;});_LL905:
return({struct _tuple8 _tmp782;_tmp782.f1=(_tmp77B->tq).q_const;_tmp782.f2=(void*)
2;_tmp782;});_LL901:;}_LL8F8: {void*_tmp783=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v);_LL907: if((unsigned int)_tmp783 > 3?*((
int*)_tmp783)== 7: 0){goto _LL908;}else{goto _LL909;}_LL909: goto _LL90A;_LL908:
return({struct _tuple8 _tmp784;_tmp784.f1=1;_tmp784.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp77C->rgn))->v;_tmp784;});_LL90A: _tmp77C->escapes=1;return({
struct _tuple8 _tmp785;_tmp785.f1=(_tmp77C->tq).q_const;_tmp785.f2=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp77C->rgn))->v;_tmp785;});_LL906:;}_LL8FA: _tmp77E=
_tmp77D;goto _LL8FC;_LL8FC: _tmp77E->escapes=1;return({struct _tuple8 _tmp786;
_tmp786.f1=(_tmp77E->tq).q_const;_tmp786.f2=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp77E->rgn))->v;_tmp786;});_LL8F0:;}_LL8DE: {void*_tmp787=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp774->topt))->v);struct Cyc_List_List*
_tmp788;struct Cyc_Absyn_AggrInfo _tmp789;void*_tmp78A;struct Cyc_Absyn_Aggrdecl**
_tmp78B;struct Cyc_Absyn_Aggrdecl*_tmp78C;_LL90C: if((unsigned int)_tmp787 > 3?*((
int*)_tmp787)== 11: 0){_LL912: _tmp788=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp787)->f2;goto _LL90D;}else{goto _LL90E;}_LL90E: if((unsigned int)_tmp787 > 3?*((
int*)_tmp787)== 10: 0){_LL913: _tmp789=((struct Cyc_Absyn_AggrType_struct*)_tmp787)->f1;
_LL914: _tmp78A=(void*)_tmp789.aggr_info;if(*((int*)_tmp78A)== 1){_LL915: _tmp78B=((
struct Cyc_Absyn_KnownAggr_struct*)_tmp78A)->f1;_tmp78C=*_tmp78B;goto _LL90F;}
else{goto _LL910;}}else{goto _LL910;}_LL910: goto _LL911;_LL90D: {struct Cyc_Absyn_Aggrfield*
_tmp78D=Cyc_Absyn_lookup_field(_tmp788,_tmp773);if(_tmp78D != 0? _tmp78D->width != 
0: 0){return({struct _tuple8 _tmp78E;_tmp78E.f1=(_tmp78D->tq).q_const;_tmp78E.f2=(
Cyc_Tcutil_addressof_props(te,_tmp774)).f2;_tmp78E;});}return bogus_ans;}_LL90F: {
struct Cyc_Absyn_Aggrfield*_tmp78F=Cyc_Absyn_lookup_decl_field(_tmp78C,_tmp773);
if(_tmp78F != 0? _tmp78F->width != 0: 0){return({struct _tuple8 _tmp790;_tmp790.f1=(
_tmp78F->tq).q_const;_tmp790.f2=(Cyc_Tcutil_addressof_props(te,_tmp774)).f2;
_tmp790;});}return bogus_ans;}_LL911: return bogus_ans;_LL90B:;}_LL8E0: {void*
_tmp791=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp776->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp792;void*_tmp793;void*_tmp794;_LL917: if((unsigned int)
_tmp791 > 3?*((int*)_tmp791)== 4: 0){_LL91B: _tmp792=((struct Cyc_Absyn_PointerType_struct*)
_tmp791)->f1;_LL91D: _tmp794=(void*)_tmp792.elt_typ;goto _LL91C;_LL91C: _tmp793=(
void*)_tmp792.rgn_typ;goto _LL918;}else{goto _LL919;}_LL919: goto _LL91A;_LL918: {
void*_tmp795=Cyc_Tcutil_compress(_tmp794);struct Cyc_List_List*_tmp796;struct Cyc_Absyn_AggrInfo
_tmp797;void*_tmp798;struct Cyc_Absyn_Aggrdecl**_tmp799;struct Cyc_Absyn_Aggrdecl*
_tmp79A;_LL91F: if((unsigned int)_tmp795 > 3?*((int*)_tmp795)== 11: 0){_LL925:
_tmp796=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp795)->f2;goto _LL920;}else{
goto _LL921;}_LL921: if((unsigned int)_tmp795 > 3?*((int*)_tmp795)== 10: 0){_LL926:
_tmp797=((struct Cyc_Absyn_AggrType_struct*)_tmp795)->f1;_LL927: _tmp798=(void*)
_tmp797.aggr_info;if(*((int*)_tmp798)== 1){_LL928: _tmp799=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp798)->f1;_tmp79A=*_tmp799;goto _LL922;}else{goto _LL923;}}else{goto _LL923;}
_LL923: goto _LL924;_LL920: {struct Cyc_Absyn_Aggrfield*_tmp79B=Cyc_Absyn_lookup_field(
_tmp796,_tmp775);if(_tmp79B != 0? _tmp79B->width != 0: 0){return({struct _tuple8
_tmp79C;_tmp79C.f1=(_tmp79B->tq).q_const;_tmp79C.f2=_tmp793;_tmp79C;});}return
bogus_ans;}_LL922: {struct Cyc_Absyn_Aggrfield*_tmp79D=Cyc_Absyn_lookup_decl_field(
_tmp79A,_tmp775);if(_tmp79D != 0? _tmp79D->width != 0: 0){return({struct _tuple8
_tmp79E;_tmp79E.f1=(_tmp79D->tq).q_const;_tmp79E.f2=(Cyc_Tcutil_addressof_props(
te,_tmp776)).f2;_tmp79E;});}return bogus_ans;}_LL924: return bogus_ans;_LL91E:;}
_LL91A: return bogus_ans;_LL916:;}_LL8E2: {void*_tmp79F=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp777->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp7A0;struct Cyc_Absyn_Tqual _tmp7A1;void*_tmp7A2;_LL92A: if((unsigned int)
_tmp79F > 3?*((int*)_tmp79F)== 4: 0){_LL92E: _tmp7A0=((struct Cyc_Absyn_PointerType_struct*)
_tmp79F)->f1;_LL930: _tmp7A2=(void*)_tmp7A0.rgn_typ;goto _LL92F;_LL92F: _tmp7A1=
_tmp7A0.tq;goto _LL92B;}else{goto _LL92C;}_LL92C: goto _LL92D;_LL92B: return({struct
_tuple8 _tmp7A3;_tmp7A3.f1=_tmp7A1.q_const;_tmp7A3.f2=_tmp7A2;_tmp7A3;});_LL92D:
return bogus_ans;_LL929:;}_LL8E4: {void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp779->topt))->v);void*_tmp7A4=t;struct Cyc_Absyn_Tqual _tmp7A5;
struct Cyc_List_List*_tmp7A6;struct Cyc_Absyn_PtrInfo _tmp7A7;struct Cyc_Absyn_Conref*
_tmp7A8;struct Cyc_Absyn_Tqual _tmp7A9;void*_tmp7AA;void*_tmp7AB;_LL932: if((
unsigned int)_tmp7A4 > 3?*((int*)_tmp7A4)== 7: 0){_LL93A: _tmp7A5=((struct Cyc_Absyn_ArrayType_struct*)
_tmp7A4)->f2;goto _LL933;}else{goto _LL934;}_LL934: if((unsigned int)_tmp7A4 > 3?*((
int*)_tmp7A4)== 9: 0){_LL93B: _tmp7A6=((struct Cyc_Absyn_TupleType_struct*)_tmp7A4)->f1;
goto _LL935;}else{goto _LL936;}_LL936: if((unsigned int)_tmp7A4 > 3?*((int*)_tmp7A4)
== 4: 0){_LL93C: _tmp7A7=((struct Cyc_Absyn_PointerType_struct*)_tmp7A4)->f1;_LL940:
_tmp7AB=(void*)_tmp7A7.elt_typ;goto _LL93F;_LL93F: _tmp7AA=(void*)_tmp7A7.rgn_typ;
goto _LL93E;_LL93E: _tmp7A9=_tmp7A7.tq;goto _LL93D;_LL93D: _tmp7A8=_tmp7A7.bounds;
goto _LL937;}else{goto _LL938;}_LL938: goto _LL939;_LL933: return({struct _tuple8
_tmp7AC;_tmp7AC.f1=_tmp7A5.q_const;_tmp7AC.f2=(Cyc_Tcutil_addressof_props(te,
_tmp779)).f2;_tmp7AC;});_LL935: {struct _tuple4*_tmp7AD=Cyc_Absyn_lookup_tuple_field(
_tmp7A6,(int)Cyc_Evexp_eval_const_uint_exp(_tmp778));if(_tmp7AD != 0){return({
struct _tuple8 _tmp7AE;_tmp7AE.f1=((*_tmp7AD).f1).q_const;_tmp7AE.f2=(Cyc_Tcutil_addressof_props(
te,_tmp779)).f2;_tmp7AE;});}return bogus_ans;}_LL937: return({struct _tuple8 _tmp7AF;
_tmp7AF.f1=_tmp7A9.q_const;_tmp7AF.f2=_tmp7AA;_tmp7AF;});_LL939: return bogus_ans;
_LL931:;}_LL8E6:({void*_tmp7B0[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("unary & applied to non-lvalue",
sizeof(unsigned char),30),_tag_arr(_tmp7B0,sizeof(void*),0));});return bogus_ans;
_LL8DA:;}void*Cyc_Tcutil_array_to_ptr(struct Cyc_Tcenv_Tenv*te,void*e_typ,struct
Cyc_Absyn_Exp*e){void*_tmp7B2=Cyc_Tcutil_compress(e_typ);struct Cyc_Absyn_Tqual
_tmp7B3;void*_tmp7B4;_LL942: if((unsigned int)_tmp7B2 > 3?*((int*)_tmp7B2)== 7: 0){
_LL947: _tmp7B4=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp7B2)->f1;goto
_LL946;_LL946: _tmp7B3=((struct Cyc_Absyn_ArrayType_struct*)_tmp7B2)->f2;goto
_LL943;}else{goto _LL944;}_LL944: goto _LL945;_LL943: {void*_tmp7B6;struct _tuple8
_tmp7B5=Cyc_Tcutil_addressof_props(te,e);_LL949: _tmp7B6=_tmp7B5.f2;goto _LL948;
_LL948: return Cyc_Absyn_atb_typ(_tmp7B4,_tmp7B6,_tmp7B3,(void*)({struct Cyc_Absyn_Upper_b_struct*
_tmp7B7=_cycalloc(sizeof(*_tmp7B7));_tmp7B7[0]=({struct Cyc_Absyn_Upper_b_struct
_tmp7B8;_tmp7B8.tag=0;_tmp7B8.f1=e;_tmp7B8;});_tmp7B7;}));}_LL945: return e_typ;
_LL941:;}void Cyc_Tcutil_check_bound(struct Cyc_Position_Segment*loc,unsigned int i,
struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(b);{void*_tmp7B9=(void*)b->v;
void*_tmp7BA;void*_tmp7BB;struct Cyc_Absyn_Exp*_tmp7BC;_LL94B: if((unsigned int)
_tmp7B9 > 1?*((int*)_tmp7B9)== 0: 0){_LL951: _tmp7BA=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7B9)->f1;if((int)_tmp7BA == 0){goto _LL94C;}else{goto _LL94D;}}else{goto _LL94D;}
_LL94D: if((unsigned int)_tmp7B9 > 1?*((int*)_tmp7B9)== 0: 0){_LL952: _tmp7BB=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp7B9)->f1;if((unsigned int)_tmp7BB > 1?*((
int*)_tmp7BB)== 0: 0){_LL953: _tmp7BC=((struct Cyc_Absyn_Upper_b_struct*)_tmp7BB)->f1;
goto _LL94E;}else{goto _LL94F;}}else{goto _LL94F;}_LL94F: goto _LL950;_LL94C: return;
_LL94E: if(Cyc_Evexp_eval_const_uint_exp(_tmp7BC)<= i){({struct Cyc_Std_Int_pa_struct
_tmp7BF;_tmp7BF.tag=1;_tmp7BF.f1=(int)i;{struct Cyc_Std_Int_pa_struct _tmp7BE;
_tmp7BE.tag=1;_tmp7BE.f1=(int)Cyc_Evexp_eval_const_uint_exp(_tmp7BC);{void*
_tmp7BD[2]={& _tmp7BE,& _tmp7BF};Cyc_Tcutil_terr(loc,_tag_arr("dereference is out of bounds: %d <= %d",
sizeof(unsigned char),39),_tag_arr(_tmp7BD,sizeof(void*),2));}}});}return;_LL950:(
void*)(b->v=(void*)((void*)({struct Cyc_Absyn_Eq_constr_struct*_tmp7C0=_cycalloc(
sizeof(*_tmp7C0));_tmp7C0[0]=({struct Cyc_Absyn_Eq_constr_struct _tmp7C1;_tmp7C1.tag=
0;_tmp7C1.f1=(void*)((void*)({struct Cyc_Absyn_Upper_b_struct*_tmp7C2=_cycalloc(
sizeof(*_tmp7C2));_tmp7C2[0]=({struct Cyc_Absyn_Upper_b_struct _tmp7C3;_tmp7C3.tag=
0;_tmp7C3.f1=Cyc_Absyn_uint_exp(i + 1,0);_tmp7C3;});_tmp7C2;}));_tmp7C1;});
_tmp7C0;})));return;_LL94A:;}}void Cyc_Tcutil_check_nonzero_bound(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(loc,0,b);}int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){void*_tmp7C4=(void*)(Cyc_Absyn_compress_conref(b))->v;
void*_tmp7C5;struct Cyc_Absyn_Exp*_tmp7C6;_LL955: if((unsigned int)_tmp7C4 > 1?*((
int*)_tmp7C4)== 0: 0){_LL959: _tmp7C5=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7C4)->f1;if((unsigned int)_tmp7C5 > 1?*((int*)_tmp7C5)== 0: 0){_LL95A: _tmp7C6=((
struct Cyc_Absyn_Upper_b_struct*)_tmp7C5)->f1;goto _LL956;}else{goto _LL957;}}else{
goto _LL957;}_LL957: goto _LL958;_LL956: return Cyc_Evexp_eval_const_uint_exp(_tmp7C6)
== 1;_LL958: return 0;_LL954:;}int Cyc_Tcutil_bits_only(void*t){void*_tmp7C7=Cyc_Tcutil_compress(
t);void*_tmp7C8;struct Cyc_List_List*_tmp7C9;struct Cyc_Absyn_AggrInfo _tmp7CA;void*
_tmp7CB;void*_tmp7CC;struct Cyc_Absyn_AggrInfo _tmp7CD;void*_tmp7CE;void*_tmp7CF;
struct Cyc_Absyn_AggrInfo _tmp7D0;struct Cyc_List_List*_tmp7D1;void*_tmp7D2;struct
Cyc_Absyn_Aggrdecl**_tmp7D3;struct Cyc_Absyn_Aggrdecl*_tmp7D4;struct Cyc_List_List*
_tmp7D5;_LL95C: if((int)_tmp7C7 == 0){goto _LL95D;}else{goto _LL95E;}_LL95E: if((
unsigned int)_tmp7C7 > 3?*((int*)_tmp7C7)== 5: 0){goto _LL95F;}else{goto _LL960;}
_LL960: if((int)_tmp7C7 == 1){goto _LL961;}else{goto _LL962;}_LL962: if((unsigned int)
_tmp7C7 > 3?*((int*)_tmp7C7)== 6: 0){goto _LL963;}else{goto _LL964;}_LL964: if((
unsigned int)_tmp7C7 > 3?*((int*)_tmp7C7)== 12: 0){goto _LL965;}else{goto _LL966;}
_LL966: if((unsigned int)_tmp7C7 > 3?*((int*)_tmp7C7)== 13: 0){goto _LL967;}else{
goto _LL968;}_LL968: if((unsigned int)_tmp7C7 > 3?*((int*)_tmp7C7)== 7: 0){_LL976:
_tmp7C8=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp7C7)->f1;goto _LL969;}
else{goto _LL96A;}_LL96A: if((unsigned int)_tmp7C7 > 3?*((int*)_tmp7C7)== 9: 0){
_LL977: _tmp7C9=((struct Cyc_Absyn_TupleType_struct*)_tmp7C7)->f1;goto _LL96B;}
else{goto _LL96C;}_LL96C: if((unsigned int)_tmp7C7 > 3?*((int*)_tmp7C7)== 10: 0){
_LL978: _tmp7CA=((struct Cyc_Absyn_AggrType_struct*)_tmp7C7)->f1;_LL979: _tmp7CB=(
void*)_tmp7CA.aggr_info;if(*((int*)_tmp7CB)== 0){_LL97A: _tmp7CC=(void*)((struct
Cyc_Absyn_UnknownAggr_struct*)_tmp7CB)->f1;if((int)_tmp7CC == 1){goto _LL96D;}
else{goto _LL96E;}}else{goto _LL96E;}}else{goto _LL96E;}_LL96E: if((unsigned int)
_tmp7C7 > 3?*((int*)_tmp7C7)== 10: 0){_LL97B: _tmp7CD=((struct Cyc_Absyn_AggrType_struct*)
_tmp7C7)->f1;_LL97C: _tmp7CE=(void*)_tmp7CD.aggr_info;if(*((int*)_tmp7CE)== 0){
_LL97D: _tmp7CF=(void*)((struct Cyc_Absyn_UnknownAggr_struct*)_tmp7CE)->f1;if((int)
_tmp7CF == 0){goto _LL96F;}else{goto _LL970;}}else{goto _LL970;}}else{goto _LL970;}
_LL970: if((unsigned int)_tmp7C7 > 3?*((int*)_tmp7C7)== 10: 0){_LL97E: _tmp7D0=((
struct Cyc_Absyn_AggrType_struct*)_tmp7C7)->f1;_LL980: _tmp7D2=(void*)_tmp7D0.aggr_info;
if(*((int*)_tmp7D2)== 1){_LL981: _tmp7D3=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp7D2)->f1;_tmp7D4=*_tmp7D3;goto _LL97F;}else{goto _LL972;}_LL97F: _tmp7D1=
_tmp7D0.targs;goto _LL971;}else{goto _LL972;}_LL972: if((unsigned int)_tmp7C7 > 3?*((
int*)_tmp7C7)== 11: 0){_LL982: _tmp7D5=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp7C7)->f2;goto _LL973;}else{goto _LL974;}_LL974: goto _LL975;_LL95D: goto _LL95F;
_LL95F: goto _LL961;_LL961: goto _LL963;_LL963: return 1;_LL965: goto _LL967;_LL967:
return 0;_LL969: return Cyc_Tcutil_bits_only(_tmp7C8);_LL96B: for(0;_tmp7C9 != 0;
_tmp7C9=_tmp7C9->tl){if(! Cyc_Tcutil_bits_only((*((struct _tuple4*)_tmp7C9->hd)).f2)){
return 0;}}return 1;_LL96D: return 1;_LL96F: return 0;_LL971: if((void*)_tmp7D4->kind == (
void*)1){return 1;}if(_tmp7D4->fields == 0){return 0;}{struct _RegionHandle _tmp7D6=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp7D6;_push_region(rgn);{struct Cyc_List_List*
_tmp7D7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp7D4->tvs,
_tmp7D1);{struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp7D4->fields))->v;for(0;fs != 0;fs=fs->tl){if(! Cyc_Tcutil_bits_only(
Cyc_Tcutil_rsubstitute(rgn,_tmp7D7,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp7D8=0;_npop_handler(0);return _tmp7D8;}}}{int _tmp7D9=1;_npop_handler(0);
return _tmp7D9;}};_pop_region(rgn);}_LL973: for(0;_tmp7D5 != 0;_tmp7D5=_tmp7D5->tl){
if(! Cyc_Tcutil_bits_only((void*)((struct Cyc_Absyn_Aggrfield*)_tmp7D5->hd)->type)){
return 0;}}return 1;_LL975: return 0;_LL95B:;}struct _tuple22{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,
int var_okay,struct Cyc_Absyn_Exp*e){void*_tmp7DA=(void*)e->r;void*_tmp7DB;struct
_tuple1*_tmp7DC;struct Cyc_Absyn_Exp*_tmp7DD;struct Cyc_Absyn_Exp*_tmp7DE;struct
Cyc_Absyn_Exp*_tmp7DF;struct Cyc_Absyn_Exp*_tmp7E0;struct Cyc_Absyn_Exp*_tmp7E1;
struct Cyc_Absyn_Exp*_tmp7E2;struct Cyc_Absyn_Exp*_tmp7E3;struct Cyc_Absyn_Exp*
_tmp7E4;void*_tmp7E5;struct Cyc_Absyn_Exp*_tmp7E6;struct Cyc_Absyn_Exp*_tmp7E7;
struct Cyc_Absyn_Exp*_tmp7E8;struct Cyc_List_List*_tmp7E9;struct Cyc_List_List*
_tmp7EA;struct Cyc_List_List*_tmp7EB;struct Cyc_List_List*_tmp7EC;void*_tmp7ED;
struct Cyc_List_List*_tmp7EE;struct Cyc_List_List*_tmp7EF;_LL984: if(*((int*)
_tmp7DA)== 0){goto _LL985;}else{goto _LL986;}_LL986: if(*((int*)_tmp7DA)== 16){goto
_LL987;}else{goto _LL988;}_LL988: if(*((int*)_tmp7DA)== 17){goto _LL989;}else{goto
_LL98A;}_LL98A: if(*((int*)_tmp7DA)== 18){goto _LL98B;}else{goto _LL98C;}_LL98C: if(*((
int*)_tmp7DA)== 19){goto _LL98D;}else{goto _LL98E;}_LL98E: if(*((int*)_tmp7DA)== 31){
goto _LL98F;}else{goto _LL990;}_LL990: if(*((int*)_tmp7DA)== 32){goto _LL991;}else{
goto _LL992;}_LL992: if(*((int*)_tmp7DA)== 1){_LL9B1: _tmp7DC=((struct Cyc_Absyn_Var_e_struct*)
_tmp7DA)->f1;goto _LL9B0;_LL9B0: _tmp7DB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp7DA)->f2;goto _LL993;}else{goto _LL994;}_LL994: if(*((int*)_tmp7DA)== 6){_LL9B4:
_tmp7DF=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7DA)->f1;goto _LL9B3;_LL9B3:
_tmp7DE=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7DA)->f2;goto _LL9B2;_LL9B2:
_tmp7DD=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7DA)->f3;goto _LL995;}else{
goto _LL996;}_LL996: if(*((int*)_tmp7DA)== 7){_LL9B6: _tmp7E1=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp7DA)->f1;goto _LL9B5;_LL9B5: _tmp7E0=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp7DA)->f2;goto _LL997;}else{goto _LL998;}_LL998: if(*((int*)_tmp7DA)== 11){
_LL9B7: _tmp7E2=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp7DA)->f1;goto _LL999;}
else{goto _LL99A;}_LL99A: if(*((int*)_tmp7DA)== 12){_LL9B8: _tmp7E3=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp7DA)->f1;goto _LL99B;}else{goto _LL99C;}_LL99C: if(*((int*)_tmp7DA)== 13){
_LL9BA: _tmp7E5=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp7DA)->f1;goto _LL9B9;
_LL9B9: _tmp7E4=((struct Cyc_Absyn_Cast_e_struct*)_tmp7DA)->f2;goto _LL99D;}else{
goto _LL99E;}_LL99E: if(*((int*)_tmp7DA)== 14){_LL9BB: _tmp7E6=((struct Cyc_Absyn_Address_e_struct*)
_tmp7DA)->f1;goto _LL99F;}else{goto _LL9A0;}_LL9A0: if(*((int*)_tmp7DA)== 27){
_LL9BD: _tmp7E8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7DA)->f2;goto _LL9BC;
_LL9BC: _tmp7E7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7DA)->f3;goto _LL9A1;}
else{goto _LL9A2;}_LL9A2: if(*((int*)_tmp7DA)== 26){_LL9BE: _tmp7E9=((struct Cyc_Absyn_Array_e_struct*)
_tmp7DA)->f1;goto _LL9A3;}else{goto _LL9A4;}_LL9A4: if(*((int*)_tmp7DA)== 29){
_LL9BF: _tmp7EA=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp7DA)->f2;goto _LL9A5;}
else{goto _LL9A6;}_LL9A6: if(*((int*)_tmp7DA)== 28){_LL9C0: _tmp7EB=((struct Cyc_Absyn_Struct_e_struct*)
_tmp7DA)->f2;goto _LL9A7;}else{goto _LL9A8;}_LL9A8: if(*((int*)_tmp7DA)== 3){_LL9C2:
_tmp7ED=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp7DA)->f1;goto _LL9C1;_LL9C1:
_tmp7EC=((struct Cyc_Absyn_Primop_e_struct*)_tmp7DA)->f2;goto _LL9A9;}else{goto
_LL9AA;}_LL9AA: if(*((int*)_tmp7DA)== 24){_LL9C3: _tmp7EE=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp7DA)->f1;goto _LL9AB;}else{goto _LL9AC;}_LL9AC: if(*((int*)_tmp7DA)== 30){
_LL9C4: _tmp7EF=((struct Cyc_Absyn_Tunion_e_struct*)_tmp7DA)->f1;goto _LL9AD;}else{
goto _LL9AE;}_LL9AE: goto _LL9AF;_LL985: goto _LL987;_LL987: goto _LL989;_LL989: goto
_LL98B;_LL98B: goto _LL98D;_LL98D: goto _LL98F;_LL98F: goto _LL991;_LL991: return 1;
_LL993: {void*_tmp7F0=_tmp7DB;struct Cyc_Absyn_Vardecl*_tmp7F1;_LL9C6: if((
unsigned int)_tmp7F0 > 1?*((int*)_tmp7F0)== 1: 0){goto _LL9C7;}else{goto _LL9C8;}
_LL9C8: if((unsigned int)_tmp7F0 > 1?*((int*)_tmp7F0)== 0: 0){_LL9CE: _tmp7F1=((
struct Cyc_Absyn_Global_b_struct*)_tmp7F0)->f1;goto _LL9C9;}else{goto _LL9CA;}
_LL9CA: if((int)_tmp7F0 == 0){goto _LL9CB;}else{goto _LL9CC;}_LL9CC: goto _LL9CD;
_LL9C7: return 1;_LL9C9: {void*_tmp7F2=Cyc_Tcutil_compress((void*)_tmp7F1->type);
_LL9D0: if((unsigned int)_tmp7F2 > 3?*((int*)_tmp7F2)== 7: 0){goto _LL9D1;}else{goto
_LL9D2;}_LL9D2: if((unsigned int)_tmp7F2 > 3?*((int*)_tmp7F2)== 8: 0){goto _LL9D3;}
else{goto _LL9D4;}_LL9D4: goto _LL9D5;_LL9D1: goto _LL9D3;_LL9D3: return 1;_LL9D5:
return var_okay;_LL9CF:;}_LL9CB: return 0;_LL9CD: return var_okay;_LL9C5:;}_LL995:
return(Cyc_Tcutil_cnst_exp(te,0,_tmp7DF)? Cyc_Tcutil_cnst_exp(te,0,_tmp7DE): 0)?
Cyc_Tcutil_cnst_exp(te,0,_tmp7DD): 0;_LL997: return Cyc_Tcutil_cnst_exp(te,0,
_tmp7E1)? Cyc_Tcutil_cnst_exp(te,0,_tmp7E0): 0;_LL999: _tmp7E3=_tmp7E2;goto _LL99B;
_LL99B: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmp7E3);_LL99D: return Cyc_Tcutil_cnst_exp(
te,var_okay,_tmp7E4);_LL99F: return Cyc_Tcutil_cnst_exp(te,1,_tmp7E6);_LL9A1:
return Cyc_Tcutil_cnst_exp(te,0,_tmp7E8)? Cyc_Tcutil_cnst_exp(te,0,_tmp7E7): 0;
_LL9A3: _tmp7EA=_tmp7E9;goto _LL9A5;_LL9A5: _tmp7EB=_tmp7EA;goto _LL9A7;_LL9A7: for(0;
_tmp7EB != 0;_tmp7EB=_tmp7EB->tl){if(! Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple22*)
_tmp7EB->hd)).f2)){return 0;}}return 1;_LL9A9: _tmp7EE=_tmp7EC;goto _LL9AB;_LL9AB:
_tmp7EF=_tmp7EE;goto _LL9AD;_LL9AD: for(0;_tmp7EF != 0;_tmp7EF=_tmp7EF->tl){if(! Cyc_Tcutil_cnst_exp(
te,0,(struct Cyc_Absyn_Exp*)_tmp7EF->hd)){return 0;}}return 1;_LL9AF: return 0;_LL983:;}
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){return
Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(struct
Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t){void*_tmp7F3=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp7F4;
struct Cyc_Absyn_Conref*_tmp7F5;struct Cyc_Absyn_Conref*_tmp7F6;void*_tmp7F7;
struct Cyc_List_List*_tmp7F8;struct Cyc_Absyn_AggrInfo _tmp7F9;struct Cyc_List_List*
_tmp7FA;void*_tmp7FB;struct Cyc_List_List*_tmp7FC;_LL9D7: if((int)_tmp7F3 == 0){
goto _LL9D8;}else{goto _LL9D9;}_LL9D9: if((unsigned int)_tmp7F3 > 3?*((int*)_tmp7F3)
== 5: 0){goto _LL9DA;}else{goto _LL9DB;}_LL9DB: if((int)_tmp7F3 == 1){goto _LL9DC;}
else{goto _LL9DD;}_LL9DD: if((unsigned int)_tmp7F3 > 3?*((int*)_tmp7F3)== 6: 0){goto
_LL9DE;}else{goto _LL9DF;}_LL9DF: if((unsigned int)_tmp7F3 > 3?*((int*)_tmp7F3)== 4:
0){_LL9EF: _tmp7F4=((struct Cyc_Absyn_PointerType_struct*)_tmp7F3)->f1;_LL9F1:
_tmp7F6=_tmp7F4.nullable;goto _LL9F0;_LL9F0: _tmp7F5=_tmp7F4.bounds;goto _LL9E0;}
else{goto _LL9E1;}_LL9E1: if((unsigned int)_tmp7F3 > 3?*((int*)_tmp7F3)== 7: 0){
_LL9F2: _tmp7F7=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp7F3)->f1;goto
_LL9E2;}else{goto _LL9E3;}_LL9E3: if((unsigned int)_tmp7F3 > 3?*((int*)_tmp7F3)== 9:
0){_LL9F3: _tmp7F8=((struct Cyc_Absyn_TupleType_struct*)_tmp7F3)->f1;goto _LL9E4;}
else{goto _LL9E5;}_LL9E5: if((unsigned int)_tmp7F3 > 3?*((int*)_tmp7F3)== 10: 0){
_LL9F4: _tmp7F9=((struct Cyc_Absyn_AggrType_struct*)_tmp7F3)->f1;_LL9F6: _tmp7FB=(
void*)_tmp7F9.aggr_info;goto _LL9F5;_LL9F5: _tmp7FA=_tmp7F9.targs;goto _LL9E6;}
else{goto _LL9E7;}_LL9E7: if((unsigned int)_tmp7F3 > 3?*((int*)_tmp7F3)== 11: 0){
_LL9F7: _tmp7FC=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp7F3)->f2;goto _LL9E8;}
else{goto _LL9E9;}_LL9E9: if((unsigned int)_tmp7F3 > 3?*((int*)_tmp7F3)== 13: 0){
goto _LL9EA;}else{goto _LL9EB;}_LL9EB: if((unsigned int)_tmp7F3 > 3?*((int*)_tmp7F3)
== 12: 0){goto _LL9EC;}else{goto _LL9ED;}_LL9ED: goto _LL9EE;_LL9D8: goto _LL9DA;_LL9DA:
goto _LL9DC;_LL9DC: goto _LL9DE;_LL9DE: return 1;_LL9E0: {void*_tmp7FD=(void*)(Cyc_Absyn_compress_conref(
_tmp7F5))->v;void*_tmp7FE;void*_tmp7FF;_LL9F9: if((unsigned int)_tmp7FD > 1?*((int*)
_tmp7FD)== 0: 0){_LL9FF: _tmp7FE=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp7FD)->f1;if((int)_tmp7FE == 0){goto _LL9FA;}else{goto _LL9FB;}}else{goto _LL9FB;}
_LL9FB: if((unsigned int)_tmp7FD > 1?*((int*)_tmp7FD)== 0: 0){_LLA00: _tmp7FF=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp7FD)->f1;if((unsigned int)_tmp7FF > 1?*((
int*)_tmp7FF)== 0: 0){goto _LL9FC;}else{goto _LL9FD;}}else{goto _LL9FD;}_LL9FD: goto
_LL9FE;_LL9FA: return 1;_LL9FC: {void*_tmp800=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp7F6))->v;int _tmp801;
_LLA02: if((unsigned int)_tmp800 > 1?*((int*)_tmp800)== 0: 0){_LLA06: _tmp801=(int)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp800)->f1;goto _LLA03;}else{goto _LLA04;}
_LLA04: goto _LLA05;_LLA03: return _tmp801;_LLA05: return 0;_LLA01:;}_LL9FE: return 0;
_LL9F8:;}_LL9E2: return Cyc_Tcutil_supports_default(_tmp7F7);_LL9E4: for(0;_tmp7F8
!= 0;_tmp7F8=_tmp7F8->tl){if(! Cyc_Tcutil_supports_default((*((struct _tuple4*)
_tmp7F8->hd)).f2)){return 0;}}return 1;_LL9E6: {struct Cyc_Absyn_Aggrdecl*_tmp802=
Cyc_Absyn_get_known_aggrdecl(_tmp7FB);if(_tmp802->fields == 0){return 0;}if((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp802->exist_vars))->v
!= 0){return 0;}return Cyc_Tcutil_fields_support_default(_tmp802->tvs,_tmp7FA,(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp802->fields))->v);}
_LL9E8: return Cyc_Tcutil_fields_support_default(0,0,_tmp7FC);_LL9EA: goto _LL9EC;
_LL9EC: return 1;_LL9EE: return 0;_LL9D6:;}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct
_RegionHandle _tmp803=_new_region("rgn");struct _RegionHandle*rgn=& _tmp803;
_push_region(rgn);{struct Cyc_List_List*_tmp804=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(
rgn,_tmp804,(void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);if(! Cyc_Tcutil_supports_default(
t)){int _tmp805=0;_npop_handler(0);return _tmp805;}}};_pop_region(rgn);}return 1;}
int Cyc_Tcutil_is_noreturn(void*t){{void*_tmp806=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp807;void*_tmp808;struct Cyc_Absyn_FnInfo _tmp809;struct Cyc_List_List*_tmp80A;
_LLA08: if((unsigned int)_tmp806 > 3?*((int*)_tmp806)== 4: 0){_LLA0E: _tmp807=((
struct Cyc_Absyn_PointerType_struct*)_tmp806)->f1;_LLA0F: _tmp808=(void*)_tmp807.elt_typ;
goto _LLA09;}else{goto _LLA0A;}_LLA0A: if((unsigned int)_tmp806 > 3?*((int*)_tmp806)
== 8: 0){_LLA10: _tmp809=((struct Cyc_Absyn_FnType_struct*)_tmp806)->f1;_LLA11:
_tmp80A=_tmp809.attributes;goto _LLA0B;}else{goto _LLA0C;}_LLA0C: goto _LLA0D;_LLA09:
return Cyc_Tcutil_is_noreturn(_tmp808);_LLA0B: for(0;_tmp80A != 0;_tmp80A=_tmp80A->tl){
void*_tmp80B=(void*)_tmp80A->hd;_LLA13: if((int)_tmp80B == 3){goto _LLA14;}else{
goto _LLA15;}_LLA15: goto _LLA16;_LLA14: return 1;_LLA16: continue;_LLA12:;}goto _LLA07;
_LLA0D: goto _LLA07;_LLA07:;}return 0;}
