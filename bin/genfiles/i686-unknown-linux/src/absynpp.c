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
 struct Cyc_Core_Opt{void*v;};extern struct _tagged_arr Cyc_Core_new_string(
unsigned int);extern unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___abstractFILE;struct
Cyc_Std___cycFILE;extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int
Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr);
extern struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);
struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),
struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,
void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);extern struct
Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern
unsigned char Cyc_List_Nth[8];extern int Cyc_List_exists(int(*pred)(void*),struct
Cyc_List_List*x);extern int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2);extern int Cyc_List_list_prefix(int(*cmp)(void*,void*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;extern void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc_Std___cycFILE*
f);extern struct _tagged_arr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);extern
struct Cyc_PP_Doc*Cyc_PP_nil_doc();extern struct Cyc_PP_Doc*Cyc_PP_line_doc();
extern struct Cyc_PP_Doc*Cyc_PP_text(struct _tagged_arr s);extern struct Cyc_PP_Doc*
Cyc_PP_textptr(struct _tagged_arr*p);extern struct Cyc_PP_Doc*Cyc_PP_nest(int k,
struct Cyc_PP_Doc*d);extern struct Cyc_PP_Doc*Cyc_PP_cat(struct _tagged_arr);extern
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr sep,
struct Cyc_List_List*l);extern struct Cyc_PP_Doc*Cyc_PP_seql(struct _tagged_arr sep,
struct Cyc_List_List*l0);extern struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*
pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*l);extern struct Cyc_PP_Doc*
Cyc_PP_group(struct _tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,
struct Cyc_List_List*l);extern struct Cyc_PP_Doc*Cyc_PP_egroup(struct _tagged_arr
start,struct _tagged_arr stop,struct _tagged_arr sep,struct Cyc_List_List*l);struct
Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;
int col;};extern unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};
struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int q_const: 1;int q_volatile: 1;int
q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*
f1;void*f2;};struct Cyc_Absyn_Tvar{struct _tagged_arr*name;int*identity;void*kind;
};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrInfo{
void*elt_typ;void*rgn_typ;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Tqual
tq;struct Cyc_Absyn_Conref*bounds;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*
name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct
Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple0*name;int is_xtunion;};struct Cyc_Absyn_UnknownTunion_struct{int tag;struct
Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int tag;struct
Cyc_Absyn_Tuniondecl**f1;};struct Cyc_Absyn_TunionInfo{void*tunion_info;struct Cyc_List_List*
targs;void*rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*
tunion_name;struct _tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{
int tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Absyn_TunionFieldInfo{
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{
int tag;void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct
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
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};
struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Core_Opt*f3;};struct Cyc_Absyn_AccessEff_struct{
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
tag;void*f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*f1;void*f2;};
struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Primop_e_struct{
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
int tag;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Aggrdecl*f3;};
struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};
struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_struct{int
tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Codegen_e_struct{int tag;struct
Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Fill_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_ForArrayInfo{struct Cyc_List_List*defns;struct _tuple2 condition;
struct _tuple2 delta;struct Cyc_Absyn_Stmt*body;};struct Cyc_Absyn_Exp_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple2
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct
Cyc_Absyn_Goto_s_struct{int tag;struct _tagged_arr*f1;struct Cyc_Absyn_Stmt*f2;};
struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple2 f2;
struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_SwitchC_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Cut_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Splice_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Label_s_struct{int tag;struct _tagged_arr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct _tuple2 f2;};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*
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
f3;struct Cyc_List_List*f4;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;
struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{void*r;
struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*
where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct
Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*cnst_exp;struct Cyc_Absyn_Stmt*body;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_List_List*attributes;};
struct Cyc_Absyn_Aggrfield{struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
exist_vars;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};
struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{
int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int
tag;struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
15];extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_compress_kb(
void*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct
Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);extern struct _tagged_arr Cyc_Absyn_attribute2string(
void*);struct _tuple3{void*f1;struct _tuple0*f2;};extern struct _tuple3 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_Buffer_t;extern int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct
_tagged_arr*s2);extern struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,
struct _tagged_arr);extern unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];
struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
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
struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f);extern struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d);extern struct _tagged_arr Cyc_Absynpp_typ2string(void*);
extern struct _tagged_arr Cyc_Absynpp_typ2cstring(void*);extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*);extern struct _tagged_arr Cyc_Absynpp_kindbound2string(void*);extern struct
_tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);extern struct _tagged_arr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*);extern struct _tagged_arr Cyc_Absynpp_decllist2string(struct Cyc_List_List*
tdl);extern struct _tagged_arr Cyc_Absynpp_prim2string(void*p);extern struct
_tagged_arr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);extern struct _tagged_arr
Cyc_Absynpp_scope2string(void*sc);extern int Cyc_Absynpp_is_anon_aggrtype(void*t);
extern struct _tagged_arr Cyc_Absynpp_cyc_string;extern struct _tagged_arr*Cyc_Absynpp_cyc_stringptr;
extern int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);extern struct _tagged_arr Cyc_Absynpp_char_escape(
unsigned char);extern struct _tagged_arr Cyc_Absynpp_string_escape(struct
_tagged_arr);extern struct _tagged_arr Cyc_Absynpp_prim2str(void*p);extern int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);struct _tuple4{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};extern struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple4*arg);
struct _tuple5{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};extern
struct _tuple5 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);struct _tuple6{
struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};extern struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(
struct _tuple6*dp);extern struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*
cs);extern struct Cyc_PP_Doc*Cyc_Absynpp_switchCclauses2doc(struct Cyc_List_List*cs);
extern struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);extern struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields);extern struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*);
extern struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*);extern struct
Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);extern struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);extern struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(
int inprec,struct Cyc_List_List*es);extern struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*);extern struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct
_tuple0*);extern struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*);extern struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc(void*);extern struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int
inprec,void*p,struct Cyc_List_List*es);struct _tuple7{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};extern struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple7*
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
struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->hd=v;_tmp1->tl=0;
_tmp1;}));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){if(((struct
Cyc_List_List*)_check_null(*l))->tl == 0){*l=0;}else{Cyc_Absynpp_suppr_last(&((
struct Cyc_List_List*)_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop(){((
void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
struct _tagged_arr Cyc_Absynpp_char_escape(unsigned char c){switch(c){case '\a': _LL0:
return _tag_arr("\\a",sizeof(unsigned char),3);case '\b': _LL1: return _tag_arr("\\b",
sizeof(unsigned char),3);case '\f': _LL2: return _tag_arr("\\f",sizeof(unsigned char),
3);case '\n': _LL3: return _tag_arr("\\n",sizeof(unsigned char),3);case '\r': _LL4:
return _tag_arr("\\r",sizeof(unsigned char),3);case '\t': _LL5: return _tag_arr("\\t",
sizeof(unsigned char),3);case '\v': _LL6: return _tag_arr("\\v",sizeof(unsigned char),
3);case '\\': _LL7: return _tag_arr("\\\\",sizeof(unsigned char),3);case '"': _LL8:
return _tag_arr("\"",sizeof(unsigned char),2);case '\'': _LL9: return _tag_arr("\\'",
sizeof(unsigned char),3);default: _LLA: if(c >= ' '? c <= '~': 0){struct _tagged_arr
_tmp2=Cyc_Core_new_string(1);*((unsigned char*)_check_unknown_subscript(_tmp2,
sizeof(unsigned char),0))=c;return(struct _tagged_arr)_tmp2;}else{struct
_tagged_arr _tmp3=Cyc_Core_new_string(4);int j=0;*((unsigned char*)
_check_unknown_subscript(_tmp3,sizeof(unsigned char),j ++))='\\';*((unsigned char*)
_check_unknown_subscript(_tmp3,sizeof(unsigned char),j ++))=(unsigned char)('0' + ((
unsigned char)c >> 6 & 7));*((unsigned char*)_check_unknown_subscript(_tmp3,
sizeof(unsigned char),j ++))=(unsigned char)('0' + (c >> 3 & 7));*((unsigned char*)
_check_unknown_subscript(_tmp3,sizeof(unsigned char),j ++))=(unsigned char)('0' + (
c & 7));return(struct _tagged_arr)_tmp3;}}}static int Cyc_Absynpp_special(struct
_tagged_arr s){int sz=(int)(_get_arr_size(s,sizeof(unsigned char))- 1);{int i=0;
for(0;i < sz;i ++){unsigned char c=((const unsigned char*)s.curr)[i];if(((c <= ' '? 1:
c >= '~')? 1: c == '"')? 1: c == '\\'){return 1;}}}return 0;}struct _tagged_arr Cyc_Absynpp_string_escape(
struct _tagged_arr s){if(! Cyc_Absynpp_special(s)){return s;}{int n=(int)(
_get_arr_size(s,sizeof(unsigned char))- 1);if(n > 0?((const unsigned char*)s.curr)[
n]== '\000': 0){n --;}{int len=0;{int i=0;for(0;i <= n;i ++){unsigned char _tmp4=((
const unsigned char*)s.curr)[i];_LLD: if(_tmp4 == '\a'){goto _LLE;}else{goto _LLF;}
_LLF: if(_tmp4 == '\b'){goto _LL10;}else{goto _LL11;}_LL11: if(_tmp4 == '\f'){goto
_LL12;}else{goto _LL13;}_LL13: if(_tmp4 == '\n'){goto _LL14;}else{goto _LL15;}_LL15:
if(_tmp4 == '\r'){goto _LL16;}else{goto _LL17;}_LL17: if(_tmp4 == '\t'){goto _LL18;}
else{goto _LL19;}_LL19: if(_tmp4 == '\v'){goto _LL1A;}else{goto _LL1B;}_LL1B: if(_tmp4
== '\\'){goto _LL1C;}else{goto _LL1D;}_LL1D: if(_tmp4 == '"'){goto _LL1E;}else{goto
_LL1F;}_LL1F: goto _LL20;_LLE: goto _LL10;_LL10: goto _LL12;_LL12: goto _LL14;_LL14: goto
_LL16;_LL16: goto _LL18;_LL18: goto _LL1A;_LL1A: goto _LL1C;_LL1C: goto _LL1E;_LL1E: len +=
2;goto _LLC;_LL20: if(_tmp4 >= ' '? _tmp4 <= '~': 0){len ++;}else{len +=4;}goto _LLC;_LLC:;}}{
struct _tagged_arr t=Cyc_Core_new_string((unsigned int)len);int j=0;{int i=0;for(0;i
<= n;i ++){unsigned char _tmp5=((const unsigned char*)s.curr)[i];_LL22: if(_tmp5 == '\a'){
goto _LL23;}else{goto _LL24;}_LL24: if(_tmp5 == '\b'){goto _LL25;}else{goto _LL26;}
_LL26: if(_tmp5 == '\f'){goto _LL27;}else{goto _LL28;}_LL28: if(_tmp5 == '\n'){goto
_LL29;}else{goto _LL2A;}_LL2A: if(_tmp5 == '\r'){goto _LL2B;}else{goto _LL2C;}_LL2C:
if(_tmp5 == '\t'){goto _LL2D;}else{goto _LL2E;}_LL2E: if(_tmp5 == '\v'){goto _LL2F;}
else{goto _LL30;}_LL30: if(_tmp5 == '\\'){goto _LL31;}else{goto _LL32;}_LL32: if(_tmp5
== '"'){goto _LL33;}else{goto _LL34;}_LL34: goto _LL35;_LL23:*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='a';goto _LL21;_LL25:*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='b';goto
_LL21;_LL27:*((unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j
++))='\\';*((unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='f';
goto _LL21;_LL29:*((unsigned char*)_check_unknown_subscript(t,sizeof(
unsigned char),j ++))='\\';*((unsigned char*)_check_unknown_subscript(t,sizeof(
unsigned char),j ++))='n';goto _LL21;_LL2B:*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='r';goto _LL21;_LL2D:*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='t';goto
_LL21;_LL2F:*((unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j
++))='\\';*((unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='v';
goto _LL21;_LL31:*((unsigned char*)_check_unknown_subscript(t,sizeof(
unsigned char),j ++))='\\';*((unsigned char*)_check_unknown_subscript(t,sizeof(
unsigned char),j ++))='\\';goto _LL21;_LL33:*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='"';goto _LL21;_LL35: if(
_tmp5 >= ' '? _tmp5 <= '~': 0){*((unsigned char*)_check_unknown_subscript(t,sizeof(
unsigned char),j ++))=_tmp5;}else{*((unsigned char*)_check_unknown_subscript(t,
sizeof(unsigned char),j ++))='\\';*((unsigned char*)_check_unknown_subscript(t,
sizeof(unsigned char),j ++))=(unsigned char)('0' + (_tmp5 >> 6 & 7));*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))=(
unsigned char)('0' + (_tmp5 >> 3 & 7));*((unsigned char*)_check_unknown_subscript(t,
sizeof(unsigned char),j ++))=(unsigned char)('0' + (_tmp5 & 7));}goto _LL21;_LL21:;}}
return(struct _tagged_arr)t;}}}}static unsigned char _tmp6[9]="restrict";static
struct _tagged_arr Cyc_Absynpp_restrict_string={_tmp6,_tmp6,_tmp6 + 9};static
unsigned char _tmp7[9]="volatile";static struct _tagged_arr Cyc_Absynpp_volatile_string={
_tmp7,_tmp7,_tmp7 + 9};static unsigned char _tmp8[6]="const";static struct
_tagged_arr Cyc_Absynpp_const_str={_tmp8,_tmp8,_tmp8 + 6};static struct _tagged_arr*
Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;static struct _tagged_arr*Cyc_Absynpp_volatile_sp=&
Cyc_Absynpp_volatile_string;static struct _tagged_arr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*
l=0;if(tq.q_restrict){l=({struct Cyc_List_List*_tmp9=_cycalloc(sizeof(*_tmp9));
_tmp9->hd=Cyc_Absynpp_restrict_sp;_tmp9->tl=l;_tmp9;});}if(tq.q_volatile){l=({
struct Cyc_List_List*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->hd=Cyc_Absynpp_volatile_sp;
_tmpA->tl=l;_tmpA;});}if(tq.q_const){l=({struct Cyc_List_List*_tmpB=_cycalloc(
sizeof(*_tmpB));_tmpB->hd=Cyc_Absynpp_const_sp;_tmpB->tl=l;_tmpB;});}return Cyc_PP_egroup(
_tag_arr("",sizeof(unsigned char),1),_tag_arr(" ",sizeof(unsigned char),2),
_tag_arr(" ",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));}
struct _tagged_arr Cyc_Absynpp_kind2string(void*k){void*_tmpC=k;_LL37: if((int)
_tmpC == 0){goto _LL38;}else{goto _LL39;}_LL39: if((int)_tmpC == 1){goto _LL3A;}else{
goto _LL3B;}_LL3B: if((int)_tmpC == 2){goto _LL3C;}else{goto _LL3D;}_LL3D: if((int)
_tmpC == 3){goto _LL3E;}else{goto _LL3F;}_LL3F: if((int)_tmpC == 4){goto _LL40;}else{
goto _LL36;}_LL38: return _tag_arr("A",sizeof(unsigned char),2);_LL3A: return
_tag_arr("M",sizeof(unsigned char),2);_LL3C: return _tag_arr("B",sizeof(
unsigned char),2);_LL3E: return _tag_arr("R",sizeof(unsigned char),2);_LL40: return
_tag_arr("E",sizeof(unsigned char),2);_LL36:;}struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*c){void*_tmpD=Cyc_Absyn_compress_kb(c);void*_tmpE;void*_tmpF;_LL42: if(*((
int*)_tmpD)== 0){_LL48: _tmpE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpD)->f1;
goto _LL43;}else{goto _LL44;}_LL44: if(*((int*)_tmpD)== 1){goto _LL45;}else{goto
_LL46;}_LL46: if(*((int*)_tmpD)== 2){_LL49: _tmpF=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpD)->f2;goto _LL47;}else{goto _LL41;}_LL43: return Cyc_Absynpp_kind2string(_tmpE);
_LL45: return _tag_arr("?",sizeof(unsigned char),2);_LL47: return Cyc_Absynpp_kind2string(
_tmpF);_LL41:;}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(void*k){return Cyc_PP_text(
Cyc_Absynpp_kind2string(k));}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
return Cyc_PP_text(Cyc_Absynpp_kindbound2string(c));}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(
struct Cyc_List_List*ts){return Cyc_PP_egroup(_tag_arr("<",sizeof(unsigned char),2),
_tag_arr(">",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_typ2doc,ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*
tv){void*_tmp10=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp11;void*_tmp12;
void*_tmp13;_LL4B: if(*((int*)_tmp10)== 1){goto _LL4C;}else{goto _LL4D;}_LL4D: if(*((
int*)_tmp10)== 0){_LL53: _tmp11=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp10)->f1;
if((int)_tmp11 == 2){goto _LL4E;}else{goto _LL4F;}}else{goto _LL4F;}_LL4F: if(*((int*)
_tmp10)== 2){_LL54: _tmp12=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp10)->f2;
goto _LL50;}else{goto _LL51;}_LL51: if(*((int*)_tmp10)== 0){_LL55: _tmp13=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp10)->f1;goto _LL52;}else{goto _LL4A;}_LL4C: goto
_LL4E;_LL4E: return Cyc_PP_textptr(tv->name);_LL50: _tmp13=_tmp12;goto _LL52;_LL52:
return({struct Cyc_PP_Doc*_tmp14[3];_tmp14[2]=Cyc_Absynpp_kind2doc(_tmp13);_tmp14[
1]=Cyc_PP_text(_tag_arr("::",sizeof(unsigned char),3));_tmp14[0]=Cyc_PP_textptr(
tv->name);Cyc_PP_cat(_tag_arr(_tmp14,sizeof(struct Cyc_PP_Doc*),3));});_LL4A:;}
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){return Cyc_PP_egroup(
_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,
tvs));}static struct _tagged_arr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds){return Cyc_Absynpp_ktvars2doc(tvs);}return Cyc_PP_egroup(
_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((
struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));}struct _tuple8{struct Cyc_Absyn_Tqual
f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple8*t){return Cyc_Absynpp_tqtd2doc((*
t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return Cyc_PP_group(_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(
unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmp15=att;
_LL57: if((int)_tmp15 == 0){goto _LL58;}else{goto _LL59;}_LL59: if((int)_tmp15 == 1){
goto _LL5A;}else{goto _LL5B;}_LL5B: if((int)_tmp15 == 2){goto _LL5C;}else{goto _LL5D;}
_LL5D: goto _LL5E;_LL58: goto _LL5A;_LL5A: goto _LL5C;_LL5C: return Cyc_PP_nil_doc();
_LL5E: return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL56:;}struct Cyc_PP_Doc*
Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){
void*_tmp16=(void*)atts->hd;_LL60: if((int)_tmp16 == 0){goto _LL61;}else{goto _LL62;}
_LL62: if((int)_tmp16 == 1){goto _LL63;}else{goto _LL64;}_LL64: if((int)_tmp16 == 2){
goto _LL65;}else{goto _LL66;}_LL66: goto _LL67;_LL61: return Cyc_PP_text(_tag_arr(" _stdcall ",
sizeof(unsigned char),11));_LL63: return Cyc_PP_text(_tag_arr(" _cdecl ",sizeof(
unsigned char),9));_LL65: return Cyc_PP_text(_tag_arr(" _fastcall ",sizeof(
unsigned char),12));_LL67: goto _LL5F;_LL5F:;}return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*
Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*
atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmp17=(void*)atts2->hd;_LL69:
if((int)_tmp17 == 0){goto _LL6A;}else{goto _LL6B;}_LL6B: if((int)_tmp17 == 1){goto
_LL6C;}else{goto _LL6D;}_LL6D: if((int)_tmp17 == 2){goto _LL6E;}else{goto _LL6F;}
_LL6F: goto _LL70;_LL6A: goto _LL6C;_LL6C: goto _LL6E;_LL6E: goto _LL68;_LL70: hasatt=1;
goto _LL68;_LL68:;}}if(! hasatt){return Cyc_PP_nil_doc();}return({struct Cyc_PP_Doc*
_tmp18[3];_tmp18[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp18[1]=
Cyc_PP_group(_tag_arr("",sizeof(unsigned char),1),_tag_arr("",sizeof(
unsigned char),1),_tag_arr(" ",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,
atts));_tmp18[0]=Cyc_PP_text(_tag_arr("__declspec(",sizeof(unsigned char),12));
Cyc_PP_cat(_tag_arr(_tmp18,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*
Cyc_Absynpp_att2doc(void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){if(atts == 0){
return Cyc_PP_nil_doc();}if(Cyc_Absynpp_to_VC){return Cyc_Absynpp_noncallconv2doc(
atts);}return({struct Cyc_PP_Doc*_tmp19[2];_tmp19[1]=Cyc_PP_group(_tag_arr("((",
sizeof(unsigned char),3),_tag_arr("))",sizeof(unsigned char),3),_tag_arr(",",
sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));_tmp19[0]=Cyc_PP_text(
_tag_arr(" __attribute__",sizeof(unsigned char),15));Cyc_PP_cat(_tag_arr(_tmp19,
sizeof(struct Cyc_PP_Doc*),2));});}int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*
tms){if(tms == 0){return 0;}{void*_tmp1A=(void*)tms->hd;_LL72: if((unsigned int)
_tmp1A > 1?*((int*)_tmp1A)== 1: 0){goto _LL73;}else{goto _LL74;}_LL74: if((
unsigned int)_tmp1A > 1?*((int*)_tmp1A)== 4: 0){goto _LL75;}else{goto _LL76;}_LL76:
goto _LL77;_LL73: return 1;_LL75: if(! Cyc_Absynpp_to_VC){return 0;}return Cyc_Absynpp_next_is_pointer(
tms->tl);_LL77: return 0;_LL71:;}}extern struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*
t);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(struct Cyc_PP_Doc*d,struct Cyc_List_List*
tms){if(tms == 0){return d;}{struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_tmp3D[3];_tmp3D[2]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp3D[1]=rest;_tmp3D[0]=Cyc_PP_text(
_tag_arr("(",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp3D,sizeof(struct
Cyc_PP_Doc*),3));});void*_tmp1B=(void*)tms->hd;struct Cyc_Absyn_Exp*_tmp1C;void*
_tmp1D;struct Cyc_List_List*_tmp1E;int _tmp1F;struct Cyc_Position_Segment*_tmp20;
struct Cyc_List_List*_tmp21;struct Cyc_Absyn_Tqual _tmp22;void*_tmp23;void*_tmp24;
_LL79: if((int)_tmp1B == 0){goto _LL7A;}else{goto _LL7B;}_LL7B: if((unsigned int)
_tmp1B > 1?*((int*)_tmp1B)== 0: 0){_LL85: _tmp1C=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp1B)->f1;goto _LL7C;}else{goto _LL7D;}_LL7D: if((unsigned int)_tmp1B > 1?*((int*)
_tmp1B)== 2: 0){_LL86: _tmp1D=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp1B)->f1;
goto _LL7E;}else{goto _LL7F;}_LL7F: if((unsigned int)_tmp1B > 1?*((int*)_tmp1B)== 4:
0){_LL87: _tmp1E=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp1B)->f2;goto _LL80;}
else{goto _LL81;}_LL81: if((unsigned int)_tmp1B > 1?*((int*)_tmp1B)== 3: 0){_LL8A:
_tmp21=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp1B)->f1;goto _LL89;_LL89:
_tmp20=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp1B)->f2;goto _LL88;_LL88:
_tmp1F=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp1B)->f3;goto _LL82;}else{goto
_LL83;}_LL83: if((unsigned int)_tmp1B > 1?*((int*)_tmp1B)== 1: 0){_LL8D: _tmp24=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp1B)->f1;goto _LL8C;_LL8C: _tmp23=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp1B)->f2;goto _LL8B;_LL8B: _tmp22=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp1B)->f3;goto _LL84;}else{goto _LL78;}_LL7A:
if(Cyc_Absynpp_next_is_pointer(tms->tl)){rest=p_rest;}return({struct Cyc_PP_Doc*
_tmp25[2];_tmp25[1]=Cyc_PP_text(_tag_arr("[]",sizeof(unsigned char),3));_tmp25[0]=
rest;Cyc_PP_cat(_tag_arr(_tmp25,sizeof(struct Cyc_PP_Doc*),2));});_LL7C: if(Cyc_Absynpp_next_is_pointer(
tms->tl)){rest=p_rest;}return({struct Cyc_PP_Doc*_tmp26[4];_tmp26[3]=Cyc_PP_text(
_tag_arr("]",sizeof(unsigned char),2));_tmp26[2]=Cyc_Absynpp_exp2doc(_tmp1C);
_tmp26[1]=Cyc_PP_text(_tag_arr("[",sizeof(unsigned char),2));_tmp26[0]=rest;Cyc_PP_cat(
_tag_arr(_tmp26,sizeof(struct Cyc_PP_Doc*),4));});_LL7E: if(Cyc_Absynpp_next_is_pointer(
tms->tl)){rest=p_rest;}{void*_tmp27=_tmp1D;struct Cyc_List_List*_tmp28;struct Cyc_Core_Opt*
_tmp29;struct Cyc_Absyn_VarargInfo*_tmp2A;int _tmp2B;struct Cyc_List_List*_tmp2C;
struct Cyc_Position_Segment*_tmp2D;struct Cyc_List_List*_tmp2E;_LL8F: if(*((int*)
_tmp27)== 1){_LL97: _tmp2C=((struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f1;goto
_LL96;_LL96: _tmp2B=((struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f2;goto _LL95;
_LL95: _tmp2A=((struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f3;goto _LL94;_LL94:
_tmp29=((struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f4;goto _LL93;_LL93: _tmp28=((
struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f5;goto _LL90;}else{goto _LL91;}_LL91:
if(*((int*)_tmp27)== 0){_LL99: _tmp2E=((struct Cyc_Absyn_NoTypes_struct*)_tmp27)->f1;
goto _LL98;_LL98: _tmp2D=((struct Cyc_Absyn_NoTypes_struct*)_tmp27)->f2;goto _LL92;}
else{goto _LL8E;}_LL90: return({struct Cyc_PP_Doc*_tmp2F[2];_tmp2F[1]=Cyc_Absynpp_funargs2doc(
_tmp2C,_tmp2B,_tmp2A,_tmp29,_tmp28);_tmp2F[0]=rest;Cyc_PP_cat(_tag_arr(_tmp2F,
sizeof(struct Cyc_PP_Doc*),2));});_LL92: return({struct Cyc_PP_Doc*_tmp30[2];_tmp30[
1]=Cyc_PP_group(_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(
unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,_tmp2E));_tmp30[0]=rest;Cyc_PP_cat(_tag_arr(_tmp30,sizeof(struct
Cyc_PP_Doc*),2));});_LL8E:;}_LL80: if(! Cyc_Absynpp_to_VC){if(Cyc_Absynpp_next_is_pointer(
tms->tl)){rest=p_rest;}return({struct Cyc_PP_Doc*_tmp31[2];_tmp31[1]=Cyc_Absynpp_atts2doc(
_tmp1E);_tmp31[0]=rest;Cyc_PP_cat(_tag_arr(_tmp31,sizeof(struct Cyc_PP_Doc*),2));});}
else{if(Cyc_Absynpp_next_is_pointer(tms->tl)){return({struct Cyc_PP_Doc*_tmp32[2];
_tmp32[1]=rest;_tmp32[0]=Cyc_Absynpp_callconv2doc(_tmp1E);Cyc_PP_cat(_tag_arr(
_tmp32,sizeof(struct Cyc_PP_Doc*),2));});}return rest;}_LL82: if(Cyc_Absynpp_next_is_pointer(
tms->tl)){rest=p_rest;}if(_tmp1F){return({struct Cyc_PP_Doc*_tmp33[2];_tmp33[1]=
Cyc_Absynpp_ktvars2doc(_tmp21);_tmp33[0]=rest;Cyc_PP_cat(_tag_arr(_tmp33,sizeof(
struct Cyc_PP_Doc*),2));});}else{return({struct Cyc_PP_Doc*_tmp34[2];_tmp34[1]=Cyc_Absynpp_tvars2doc(
_tmp21);_tmp34[0]=rest;Cyc_PP_cat(_tag_arr(_tmp34,sizeof(struct Cyc_PP_Doc*),2));});}
_LL84: {struct Cyc_PP_Doc*ptr;{void*_tmp35=_tmp24;struct Cyc_Absyn_Exp*_tmp36;
struct Cyc_Absyn_Exp*_tmp37;_LL9B: if((unsigned int)_tmp35 > 1?*((int*)_tmp35)== 1:
0){_LLA1: _tmp36=((struct Cyc_Absyn_Nullable_ps_struct*)_tmp35)->f1;goto _LL9C;}
else{goto _LL9D;}_LL9D: if((unsigned int)_tmp35 > 1?*((int*)_tmp35)== 0: 0){_LLA2:
_tmp37=((struct Cyc_Absyn_NonNullable_ps_struct*)_tmp35)->f1;goto _LL9E;}else{goto
_LL9F;}_LL9F: if((int)_tmp35 == 0){goto _LLA0;}else{goto _LL9A;}_LL9C: if(Cyc_Evexp_eval_const_uint_exp(
_tmp36)== 1){ptr=Cyc_PP_text(_tag_arr("*",sizeof(unsigned char),2));}else{ptr=({
struct Cyc_PP_Doc*_tmp38[4];_tmp38[3]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp38[2]=Cyc_Absynpp_exp2doc(_tmp36);_tmp38[1]=Cyc_PP_text(
_tag_arr("{",sizeof(unsigned char),2));_tmp38[0]=Cyc_PP_text(_tag_arr("*",
sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp38,sizeof(struct Cyc_PP_Doc*),4));});}
goto _LL9A;_LL9E: if(Cyc_Evexp_eval_const_uint_exp(_tmp37)== 1){ptr=Cyc_PP_text(
_tag_arr("@",sizeof(unsigned char),2));}else{ptr=({struct Cyc_PP_Doc*_tmp39[4];
_tmp39[3]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp39[2]=Cyc_Absynpp_exp2doc(
_tmp37);_tmp39[1]=Cyc_PP_text(_tag_arr("{",sizeof(unsigned char),2));_tmp39[0]=
Cyc_PP_text(_tag_arr("@",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp39,
sizeof(struct Cyc_PP_Doc*),4));});}goto _LL9A;_LLA0: ptr=Cyc_PP_text(_tag_arr("?",
sizeof(unsigned char),2));goto _LL9A;_LL9A:;}{void*_tmp3A=Cyc_Tcutil_compress(
_tmp23);_LLA4: if((int)_tmp3A == 2){goto _LLA5;}else{goto _LLA6;}_LLA6: goto _LLA7;
_LLA5: return({struct Cyc_PP_Doc*_tmp3B[2];_tmp3B[1]=rest;_tmp3B[0]=ptr;Cyc_PP_cat(
_tag_arr(_tmp3B,sizeof(struct Cyc_PP_Doc*),2));});_LLA7: return({struct Cyc_PP_Doc*
_tmp3C[4];_tmp3C[3]=rest;_tmp3C[2]=Cyc_PP_text(_tag_arr(" ",sizeof(unsigned char),
2));_tmp3C[1]=Cyc_Absynpp_typ2doc(_tmp23);_tmp3C[0]=ptr;Cyc_PP_cat(_tag_arr(
_tmp3C,sizeof(struct Cyc_PP_Doc*),4));});_LLA3:;}}_LL78:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t){void*_tmp3E=Cyc_Tcutil_compress(t);_LLA9: if((int)_tmp3E == 2){goto _LLAA;}
else{goto _LLAB;}_LLAB: goto _LLAC;_LLAA: return Cyc_PP_text(_tag_arr("`H",sizeof(
unsigned char),3));_LLAC: return Cyc_Absynpp_ntyp2doc(t);_LLA8:;}static void Cyc_Absynpp_effects2docs(
struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){void*_tmp3F=Cyc_Tcutil_compress(
t);void*_tmp40;struct Cyc_List_List*_tmp41;_LLAE: if((unsigned int)_tmp3F > 3?*((
int*)_tmp3F)== 17: 0){_LLB4: _tmp40=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp3F)->f1;goto _LLAF;}else{goto _LLB0;}_LLB0: if((unsigned int)_tmp3F > 3?*((int*)
_tmp3F)== 18: 0){_LLB5: _tmp41=((struct Cyc_Absyn_JoinEff_struct*)_tmp3F)->f1;goto
_LLB1;}else{goto _LLB2;}_LLB2: goto _LLB3;_LLAF:*rgions=({struct Cyc_List_List*
_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->hd=Cyc_Absynpp_rgn2doc(_tmp40);_tmp42->tl=*
rgions;_tmp42;});goto _LLAD;_LLB1: for(0;_tmp41 != 0;_tmp41=_tmp41->tl){Cyc_Absynpp_effects2docs(
rgions,effects,(void*)_tmp41->hd);}goto _LLAD;_LLB3:*effects=({struct Cyc_List_List*
_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->hd=Cyc_Absynpp_typ2doc(t);_tmp43->tl=*
effects;_tmp43;});goto _LLAD;_LLAD:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(&
rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0? effects != 0: 0){return Cyc_PP_group(_tag_arr("",sizeof(
unsigned char),1),_tag_arr("",sizeof(unsigned char),1),_tag_arr("+",sizeof(
unsigned char),2),effects);}else{struct Cyc_PP_Doc*_tmp44=Cyc_PP_group(_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2),rgions);return Cyc_PP_group(_tag_arr("",sizeof(
unsigned char),1),_tag_arr("",sizeof(unsigned char),1),_tag_arr("+",sizeof(
unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(effects,({struct Cyc_List_List*_tmp45=_cycalloc(sizeof(*
_tmp45));_tmp45->hd=_tmp44;_tmp45->tl=0;_tmp45;})));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(
void*k){void*_tmp46=k;_LLB7: if((int)_tmp46 == 0){goto _LLB8;}else{goto _LLB9;}_LLB9:
if((int)_tmp46 == 1){goto _LLBA;}else{goto _LLB6;}_LLB8: return Cyc_PP_text(_tag_arr("struct ",
sizeof(unsigned char),8));_LLBA: return Cyc_PP_text(_tag_arr("union ",sizeof(
unsigned char),7));_LLB6:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){struct
Cyc_PP_Doc*s;{void*_tmp47=t;struct Cyc_Core_Opt*_tmp48;int _tmp49;struct Cyc_Core_Opt*
_tmp4A;struct Cyc_Core_Opt*_tmp4B;struct Cyc_Absyn_Tvar*_tmp4C;struct Cyc_Absyn_TunionInfo
_tmp4D;void*_tmp4E;struct Cyc_List_List*_tmp4F;void*_tmp50;struct Cyc_Absyn_TunionFieldInfo
_tmp51;struct Cyc_List_List*_tmp52;void*_tmp53;void*_tmp54;void*_tmp55;int _tmp56;
struct Cyc_List_List*_tmp57;struct Cyc_Absyn_AggrInfo _tmp58;struct Cyc_List_List*
_tmp59;void*_tmp5A;struct Cyc_List_List*_tmp5B;void*_tmp5C;struct Cyc_List_List*
_tmp5D;struct _tuple0*_tmp5E;void*_tmp5F;struct Cyc_Core_Opt*_tmp60;struct Cyc_List_List*
_tmp61;struct _tuple0*_tmp62;void*_tmp63;void*_tmp64;_LLBC: if((unsigned int)
_tmp47 > 3?*((int*)_tmp47)== 7: 0){goto _LLBD;}else{goto _LLBE;}_LLBE: if((
unsigned int)_tmp47 > 3?*((int*)_tmp47)== 8: 0){goto _LLBF;}else{goto _LLC0;}_LLC0:
if((unsigned int)_tmp47 > 3?*((int*)_tmp47)== 4: 0){goto _LLC1;}else{goto _LLC2;}
_LLC2: if((int)_tmp47 == 0){goto _LLC3;}else{goto _LLC4;}_LLC4: if((unsigned int)
_tmp47 > 3?*((int*)_tmp47)== 0: 0){_LLED: _tmp4B=((struct Cyc_Absyn_Evar_struct*)
_tmp47)->f1;goto _LLEC;_LLEC: _tmp4A=((struct Cyc_Absyn_Evar_struct*)_tmp47)->f2;
goto _LLEB;_LLEB: _tmp49=((struct Cyc_Absyn_Evar_struct*)_tmp47)->f3;goto _LLEA;
_LLEA: _tmp48=((struct Cyc_Absyn_Evar_struct*)_tmp47)->f4;goto _LLC5;}else{goto
_LLC6;}_LLC6: if((unsigned int)_tmp47 > 3?*((int*)_tmp47)== 1: 0){_LLEE: _tmp4C=((
struct Cyc_Absyn_VarType_struct*)_tmp47)->f1;goto _LLC7;}else{goto _LLC8;}_LLC8: if((
unsigned int)_tmp47 > 3?*((int*)_tmp47)== 2: 0){_LLEF: _tmp4D=((struct Cyc_Absyn_TunionType_struct*)
_tmp47)->f1;_LLF2: _tmp50=(void*)_tmp4D.tunion_info;goto _LLF1;_LLF1: _tmp4F=_tmp4D.targs;
goto _LLF0;_LLF0: _tmp4E=(void*)_tmp4D.rgn;goto _LLC9;}else{goto _LLCA;}_LLCA: if((
unsigned int)_tmp47 > 3?*((int*)_tmp47)== 3: 0){_LLF3: _tmp51=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp47)->f1;_LLF5: _tmp53=(void*)_tmp51.field_info;goto _LLF4;_LLF4: _tmp52=_tmp51.targs;
goto _LLCB;}else{goto _LLCC;}_LLCC: if((unsigned int)_tmp47 > 3?*((int*)_tmp47)== 5:
0){_LLF7: _tmp55=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47)->f1;goto _LLF6;
_LLF6: _tmp54=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47)->f2;goto _LLCD;}
else{goto _LLCE;}_LLCE: if((int)_tmp47 == 1){goto _LLCF;}else{goto _LLD0;}_LLD0: if((
unsigned int)_tmp47 > 3?*((int*)_tmp47)== 6: 0){_LLF8: _tmp56=((struct Cyc_Absyn_DoubleType_struct*)
_tmp47)->f1;goto _LLD1;}else{goto _LLD2;}_LLD2: if((unsigned int)_tmp47 > 3?*((int*)
_tmp47)== 9: 0){_LLF9: _tmp57=((struct Cyc_Absyn_TupleType_struct*)_tmp47)->f1;goto
_LLD3;}else{goto _LLD4;}_LLD4: if((unsigned int)_tmp47 > 3?*((int*)_tmp47)== 10: 0){
_LLFA: _tmp58=((struct Cyc_Absyn_AggrType_struct*)_tmp47)->f1;_LLFC: _tmp5A=(void*)
_tmp58.aggr_info;goto _LLFB;_LLFB: _tmp59=_tmp58.targs;goto _LLD5;}else{goto _LLD6;}
_LLD6: if((unsigned int)_tmp47 > 3?*((int*)_tmp47)== 11: 0){_LLFE: _tmp5C=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp47)->f1;goto _LLFD;_LLFD: _tmp5B=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp47)->f2;goto _LLD7;}else{goto _LLD8;}_LLD8: if((
unsigned int)_tmp47 > 3?*((int*)_tmp47)== 13: 0){_LLFF: _tmp5D=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp47)->f1;goto _LLD9;}else{goto _LLDA;}_LLDA: if((unsigned int)_tmp47 > 3?*((int*)
_tmp47)== 12: 0){_LL100: _tmp5E=((struct Cyc_Absyn_EnumType_struct*)_tmp47)->f1;
goto _LLDB;}else{goto _LLDC;}_LLDC: if((unsigned int)_tmp47 > 3?*((int*)_tmp47)== 14:
0){_LL101: _tmp5F=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp47)->f1;goto
_LLDD;}else{goto _LLDE;}_LLDE: if((unsigned int)_tmp47 > 3?*((int*)_tmp47)== 16: 0){
_LL104: _tmp62=((struct Cyc_Absyn_TypedefType_struct*)_tmp47)->f1;goto _LL103;
_LL103: _tmp61=((struct Cyc_Absyn_TypedefType_struct*)_tmp47)->f2;goto _LL102;
_LL102: _tmp60=((struct Cyc_Absyn_TypedefType_struct*)_tmp47)->f3;goto _LLDF;}else{
goto _LLE0;}_LLE0: if((unsigned int)_tmp47 > 3?*((int*)_tmp47)== 15: 0){_LL105:
_tmp63=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp47)->f1;goto _LLE1;}
else{goto _LLE2;}_LLE2: if((int)_tmp47 == 2){goto _LLE3;}else{goto _LLE4;}_LLE4: if((
unsigned int)_tmp47 > 3?*((int*)_tmp47)== 19: 0){_LL106: _tmp64=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp47)->f1;goto _LLE5;}else{goto _LLE6;}_LLE6: if((unsigned int)_tmp47 > 3?*((int*)
_tmp47)== 17: 0){goto _LLE7;}else{goto _LLE8;}_LLE8: if((unsigned int)_tmp47 > 3?*((
int*)_tmp47)== 18: 0){goto _LLE9;}else{goto _LLBB;}_LLBD: return Cyc_PP_text(_tag_arr("[[[array]]]",
sizeof(unsigned char),12));_LLBF: return Cyc_PP_nil_doc();_LLC1: return Cyc_PP_nil_doc();
_LLC3: s=Cyc_PP_text(_tag_arr("void",sizeof(unsigned char),5));goto _LLBB;_LLC5:
if(_tmp4A != 0){return Cyc_Absynpp_ntyp2doc((void*)_tmp4A->v);}else{s=({struct Cyc_PP_Doc*
_tmp65[6];_tmp65[5]=_tmp4B == 0? Cyc_PP_text(_tag_arr("?",sizeof(unsigned char),2)):
Cyc_Absynpp_kind2doc((void*)_tmp4B->v);_tmp65[4]=Cyc_PP_text(_tag_arr(")::",
sizeof(unsigned char),4));_tmp65[3]=(! Cyc_Absynpp_print_full_evars? 1: _tmp48 == 0)?
Cyc_PP_text(_tag_arr("",sizeof(unsigned char),1)): Cyc_Absynpp_tvars2doc((struct
Cyc_List_List*)_tmp48->v);_tmp65[2]=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp67;_tmp67.tag=1;_tmp67.f1=(int)((unsigned int)_tmp49);{void*_tmp66[1]={&
_tmp67};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp66,
sizeof(void*),1));}}));_tmp65[1]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),
2));_tmp65[0]=Cyc_PP_text(_tag_arr("%",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp65,sizeof(struct Cyc_PP_Doc*),6));});}goto _LLBB;_LLC7: s=Cyc_PP_textptr(
_tmp4C->name);if(Cyc_Absynpp_print_all_kinds){s=({struct Cyc_PP_Doc*_tmp68[3];
_tmp68[2]=Cyc_Absynpp_kindbound2doc((void*)_tmp4C->kind);_tmp68[1]=Cyc_PP_text(
_tag_arr("::",sizeof(unsigned char),3));_tmp68[0]=s;Cyc_PP_cat(_tag_arr(_tmp68,
sizeof(struct Cyc_PP_Doc*),3));});}if(Cyc_Absynpp_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar(
_tmp4C): 0){s=({struct Cyc_PP_Doc*_tmp69[3];_tmp69[2]=Cyc_PP_text(_tag_arr(" */",
sizeof(unsigned char),4));_tmp69[1]=s;_tmp69[0]=Cyc_PP_text(_tag_arr("_ /* ",
sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp69,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LLBB;_LLC9:{void*_tmp6A=_tmp50;struct Cyc_Absyn_UnknownTunionInfo _tmp6B;int
_tmp6C;struct _tuple0*_tmp6D;struct Cyc_Absyn_Tuniondecl**_tmp6E;struct Cyc_Absyn_Tuniondecl*
_tmp6F;struct Cyc_Absyn_Tuniondecl _tmp70;int _tmp71;struct _tuple0*_tmp72;_LL108:
if(*((int*)_tmp6A)== 0){_LL10C: _tmp6B=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp6A)->f1;_LL10E: _tmp6D=_tmp6B.name;goto _LL10D;_LL10D: _tmp6C=_tmp6B.is_xtunion;
goto _LL109;}else{goto _LL10A;}_LL10A: if(*((int*)_tmp6A)== 1){_LL10F: _tmp6E=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp6A)->f1;_tmp6F=*_tmp6E;_tmp70=*_tmp6F;
_LL111: _tmp72=_tmp70.name;goto _LL110;_LL110: _tmp71=_tmp70.is_xtunion;goto _LL10B;}
else{goto _LL107;}_LL109: _tmp72=_tmp6D;_tmp71=_tmp6C;goto _LL10B;_LL10B: {struct
Cyc_PP_Doc*_tmp73=Cyc_PP_text(_tmp71? _tag_arr("xtunion ",sizeof(unsigned char),9):
_tag_arr("tunion ",sizeof(unsigned char),8));{void*_tmp74=Cyc_Tcutil_compress(
_tmp4E);_LL113: if((int)_tmp74 == 2){goto _LL114;}else{goto _LL115;}_LL115: goto
_LL116;_LL114: s=({struct Cyc_PP_Doc*_tmp75[3];_tmp75[2]=Cyc_Absynpp_tps2doc(
_tmp4F);_tmp75[1]=Cyc_Absynpp_qvar2doc(_tmp72);_tmp75[0]=_tmp73;Cyc_PP_cat(
_tag_arr(_tmp75,sizeof(struct Cyc_PP_Doc*),3));});goto _LL112;_LL116: s=({struct Cyc_PP_Doc*
_tmp76[5];_tmp76[4]=Cyc_Absynpp_tps2doc(_tmp4F);_tmp76[3]=Cyc_Absynpp_qvar2doc(
_tmp72);_tmp76[2]=Cyc_PP_text(_tag_arr(" ",sizeof(unsigned char),2));_tmp76[1]=
Cyc_Absynpp_typ2doc(_tmp4E);_tmp76[0]=_tmp73;Cyc_PP_cat(_tag_arr(_tmp76,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL112;_LL112:;}goto _LL107;}_LL107:;}goto _LLBB;
_LLCB:{void*_tmp77=_tmp53;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp78;int _tmp79;
struct _tuple0*_tmp7A;struct _tuple0*_tmp7B;struct Cyc_Absyn_Tunionfield*_tmp7C;
struct Cyc_Absyn_Tunionfield _tmp7D;struct _tuple0*_tmp7E;struct Cyc_Absyn_Tuniondecl*
_tmp7F;struct Cyc_Absyn_Tuniondecl _tmp80;int _tmp81;struct _tuple0*_tmp82;_LL118:
if(*((int*)_tmp77)== 0){_LL11C: _tmp78=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp77)->f1;_LL11F: _tmp7B=_tmp78.tunion_name;goto _LL11E;_LL11E: _tmp7A=_tmp78.field_name;
goto _LL11D;_LL11D: _tmp79=_tmp78.is_xtunion;goto _LL119;}else{goto _LL11A;}_LL11A:
if(*((int*)_tmp77)== 1){_LL122: _tmp7F=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp77)->f1;_tmp80=*_tmp7F;_LL124: _tmp82=_tmp80.name;goto _LL123;_LL123: _tmp81=
_tmp80.is_xtunion;goto _LL120;_LL120: _tmp7C=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp77)->f2;_tmp7D=*_tmp7C;_LL121: _tmp7E=_tmp7D.name;goto _LL11B;}else{goto _LL117;}
_LL119: _tmp82=_tmp7B;_tmp81=_tmp79;_tmp7E=_tmp7A;goto _LL11B;_LL11B: {struct Cyc_PP_Doc*
_tmp83=Cyc_PP_text(_tmp81? _tag_arr("xtunion ",sizeof(unsigned char),9): _tag_arr("tunion ",
sizeof(unsigned char),8));s=({struct Cyc_PP_Doc*_tmp84[4];_tmp84[3]=Cyc_Absynpp_qvar2doc(
_tmp7E);_tmp84[2]=Cyc_PP_text(_tag_arr(".",sizeof(unsigned char),2));_tmp84[1]=
Cyc_Absynpp_qvar2doc(_tmp82);_tmp84[0]=_tmp83;Cyc_PP_cat(_tag_arr(_tmp84,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL117;}_LL117:;}goto _LLBB;_LLCD: {struct
_tagged_arr sns;struct _tagged_arr ts;{void*_tmp85=_tmp55;_LL126: if((int)_tmp85 == 0){
goto _LL127;}else{goto _LL128;}_LL128: if((int)_tmp85 == 1){goto _LL129;}else{goto
_LL125;}_LL127: sns=_tag_arr("",sizeof(unsigned char),1);goto _LL125;_LL129: sns=
_tag_arr("unsigned ",sizeof(unsigned char),10);goto _LL125;_LL125:;}{void*_tmp86=
_tmp54;_LL12B: if((int)_tmp86 == 0){goto _LL12C;}else{goto _LL12D;}_LL12D: if((int)
_tmp86 == 1){goto _LL12E;}else{goto _LL12F;}_LL12F: if((int)_tmp86 == 2){goto _LL130;}
else{goto _LL131;}_LL131: if((int)_tmp86 == 3){goto _LL132;}else{goto _LL12A;}_LL12C:{
void*_tmp87=_tmp55;_LL134: if((int)_tmp87 == 0){goto _LL135;}else{goto _LL136;}
_LL136: if((int)_tmp87 == 1){goto _LL137;}else{goto _LL133;}_LL135: sns=_tag_arr("signed ",
sizeof(unsigned char),8);goto _LL133;_LL137: goto _LL133;_LL133:;}ts=_tag_arr("char",
sizeof(unsigned char),5);goto _LL12A;_LL12E: ts=_tag_arr("short",sizeof(
unsigned char),6);goto _LL12A;_LL130: ts=_tag_arr("int",sizeof(unsigned char),4);
goto _LL12A;_LL132: ts=Cyc_Absynpp_to_VC? _tag_arr("__int64",sizeof(unsigned char),
8): _tag_arr("long long",sizeof(unsigned char),10);goto _LL12A;_LL12A:;}s=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp8A;_tmp8A.tag=0;_tmp8A.f1=(
struct _tagged_arr)ts;{struct Cyc_Std_String_pa_struct _tmp89;_tmp89.tag=0;_tmp89.f1=(
struct _tagged_arr)sns;{void*_tmp88[2]={& _tmp89,& _tmp8A};Cyc_Std_aprintf(_tag_arr("%s%s",
sizeof(unsigned char),5),_tag_arr(_tmp88,sizeof(void*),2));}}}));goto _LLBB;}
_LLCF: s=Cyc_PP_text(_tag_arr("float",sizeof(unsigned char),6));goto _LLBB;_LLD1:
if(_tmp56){s=Cyc_PP_text(_tag_arr("long double",sizeof(unsigned char),12));}
else{s=Cyc_PP_text(_tag_arr("double",sizeof(unsigned char),7));}goto _LLBB;_LLD3:
s=({struct Cyc_PP_Doc*_tmp8B[2];_tmp8B[1]=Cyc_Absynpp_args2doc(_tmp57);_tmp8B[0]=
Cyc_PP_text(_tag_arr("$",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp8B,
sizeof(struct Cyc_PP_Doc*),2));});goto _LLBB;_LLD5: {struct _tuple0*_tmp8D;void*
_tmp8E;struct _tuple3 _tmp8C=Cyc_Absyn_aggr_kinded_name(_tmp5A);_LL13A: _tmp8E=
_tmp8C.f1;goto _LL139;_LL139: _tmp8D=_tmp8C.f2;goto _LL138;_LL138: s=({struct Cyc_PP_Doc*
_tmp8F[3];_tmp8F[2]=Cyc_Absynpp_tps2doc(_tmp59);_tmp8F[1]=Cyc_Absynpp_qvar2doc(
_tmp8D);_tmp8F[0]=Cyc_Absynpp_aggr_kind2doc(_tmp8E);Cyc_PP_cat(_tag_arr(_tmp8F,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLBB;}_LLD7: s=({struct Cyc_PP_Doc*_tmp90[4];
_tmp90[3]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp90[2]=Cyc_PP_nest(
2,Cyc_Absynpp_aggrfields2doc(_tmp5B));_tmp90[1]=Cyc_PP_text(_tag_arr("{",sizeof(
unsigned char),2));_tmp90[0]=Cyc_Absynpp_aggr_kind2doc(_tmp5C);Cyc_PP_cat(
_tag_arr(_tmp90,sizeof(struct Cyc_PP_Doc*),4));});goto _LLBB;_LLD9: s=({struct Cyc_PP_Doc*
_tmp91[3];_tmp91[2]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp91[1]=
Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp5D));_tmp91[0]=Cyc_PP_text(_tag_arr("enum {",
sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(_tmp91,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLBB;_LLDB: s=({struct Cyc_PP_Doc*_tmp92[2];_tmp92[1]=Cyc_Absynpp_qvar2doc(
_tmp5E);_tmp92[0]=Cyc_PP_text(_tag_arr("enum ",sizeof(unsigned char),6));Cyc_PP_cat(
_tag_arr(_tmp92,sizeof(struct Cyc_PP_Doc*),2));});goto _LLBB;_LLDD: s=({struct Cyc_PP_Doc*
_tmp93[3];_tmp93[2]=Cyc_PP_text(_tag_arr(">",sizeof(unsigned char),2));_tmp93[1]=
Cyc_Absynpp_typ2doc(_tmp5F);_tmp93[0]=Cyc_PP_text(_tag_arr("sizeof_t<",sizeof(
unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp93,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLBB;_LLDF: s=({struct Cyc_PP_Doc*_tmp94[2];_tmp94[1]=Cyc_Absynpp_tps2doc(
_tmp61);_tmp94[0]=Cyc_Absynpp_qvar2doc(_tmp62);Cyc_PP_cat(_tag_arr(_tmp94,
sizeof(struct Cyc_PP_Doc*),2));});goto _LLBB;_LLE1: s=({struct Cyc_PP_Doc*_tmp95[3];
_tmp95[2]=Cyc_PP_text(_tag_arr(">",sizeof(unsigned char),2));_tmp95[1]=Cyc_Absynpp_rgn2doc(
_tmp63);_tmp95[0]=Cyc_PP_text(_tag_arr("region_t<",sizeof(unsigned char),10));
Cyc_PP_cat(_tag_arr(_tmp95,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBB;_LLE3: s=
Cyc_Absynpp_rgn2doc(t);goto _LLBB;_LLE5: s=({struct Cyc_PP_Doc*_tmp96[3];_tmp96[2]=
Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp96[1]=Cyc_Absynpp_typ2doc(
_tmp64);_tmp96[0]=Cyc_PP_text(_tag_arr("regions(",sizeof(unsigned char),9));Cyc_PP_cat(
_tag_arr(_tmp96,sizeof(struct Cyc_PP_Doc*),3));});goto _LLBB;_LLE7: goto _LLE9;_LLE9:
s=Cyc_Absynpp_eff2doc(t);goto _LLBB;_LLBB:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo){return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((struct
_tagged_arr*)vo->v));}struct _tuple9{void*f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(
struct _tuple9*cmp){struct _tuple9 _tmp98;void*_tmp99;void*_tmp9A;struct _tuple9*
_tmp97=cmp;_tmp98=*_tmp97;_LL13D: _tmp9A=_tmp98.f1;goto _LL13C;_LL13C: _tmp99=
_tmp98.f2;goto _LL13B;_LL13B: return({struct Cyc_PP_Doc*_tmp9B[3];_tmp9B[2]=Cyc_Absynpp_rgn2doc(
_tmp99);_tmp9B[1]=Cyc_PP_text(_tag_arr(" < ",sizeof(unsigned char),4));_tmp9B[0]=
Cyc_Absynpp_rgn2doc(_tmp9A);Cyc_PP_cat(_tag_arr(_tmp9B,sizeof(struct Cyc_PP_Doc*),
3));});}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){return Cyc_PP_group(
_tag_arr("",sizeof(unsigned char),1),_tag_arr("",sizeof(unsigned char),1),
_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,
po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple1*t){struct Cyc_Core_Opt*
dopt=(*t).f1 == 0? 0:({struct Cyc_Core_Opt*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->v=
Cyc_PP_text(*((struct _tagged_arr*)((struct Cyc_Core_Opt*)_check_null((*t).f1))->v));
_tmp9C;});return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp9D=((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_PP_Doc*eff_doc;if(
c_varargs){_tmp9D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp9D,({struct Cyc_List_List*_tmp9E=_cycalloc(sizeof(*_tmp9E));
_tmp9E->hd=Cyc_PP_text(_tag_arr("...",sizeof(unsigned char),4));_tmp9E->tl=0;
_tmp9E;}));}else{if(cyc_varargs != 0){struct Cyc_PP_Doc*_tmp9F=({struct Cyc_PP_Doc*
_tmpA1[3];_tmpA1[2]=Cyc_Absynpp_funarg2doc(({struct _tuple1*_tmpA2=_cycalloc(
sizeof(*_tmpA2));_tmpA2->f1=cyc_varargs->name;_tmpA2->f2=cyc_varargs->tq;_tmpA2->f3=(
void*)cyc_varargs->type;_tmpA2;}));_tmpA1[1]=cyc_varargs->inject? Cyc_PP_text(
_tag_arr(" inject ",sizeof(unsigned char),9)): Cyc_PP_text(_tag_arr(" ",sizeof(
unsigned char),2));_tmpA1[0]=Cyc_PP_text(_tag_arr("...",sizeof(unsigned char),4));
Cyc_PP_cat(_tag_arr(_tmpA1,sizeof(struct Cyc_PP_Doc*),3));});_tmp9D=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9D,({struct Cyc_List_List*
_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->hd=_tmp9F;_tmpA0->tl=0;_tmpA0;}));}}{
struct Cyc_PP_Doc*_tmpA3=Cyc_PP_group(_tag_arr("",sizeof(unsigned char),1),
_tag_arr("",sizeof(unsigned char),1),_tag_arr(",",sizeof(unsigned char),2),
_tmp9D);if(effopt != 0){_tmpA3=({struct Cyc_PP_Doc*_tmpA4[3];_tmpA4[2]=Cyc_Absynpp_eff2doc((
void*)effopt->v);_tmpA4[1]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));
_tmpA4[0]=_tmpA3;Cyc_PP_cat(_tag_arr(_tmpA4,sizeof(struct Cyc_PP_Doc*),3));});}
if(rgn_po != 0){_tmpA3=({struct Cyc_PP_Doc*_tmpA5[3];_tmpA5[2]=Cyc_Absynpp_rgnpo2doc(
rgn_po);_tmpA5[1]=Cyc_PP_text(_tag_arr(":",sizeof(unsigned char),2));_tmpA5[0]=
_tmpA3;Cyc_PP_cat(_tag_arr(_tmpA5,sizeof(struct Cyc_PP_Doc*),3));});}return({
struct Cyc_PP_Doc*_tmpA6[3];_tmpA6[2]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmpA6[1]=_tmpA3;_tmpA6[0]=Cyc_PP_text(_tag_arr("(",sizeof(
unsigned char),2));Cyc_PP_cat(_tag_arr(_tmpA6,sizeof(struct Cyc_PP_Doc*),3));});}}
struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple4*arg){return({struct _tuple1*
_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->f1=({struct Cyc_Core_Opt*_tmpA8=
_cycalloc(sizeof(*_tmpA8));_tmpA8->v=(*arg).f1;_tmpA8;});_tmpA7->f2=(*arg).f2;
_tmpA7->f3=(*arg).f3;_tmpA7;});}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_tagged_arr*v){return Cyc_PP_text(*v);}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q){struct Cyc_List_List*_tmpA9=0;int match;{void*_tmpAA=(*q).f1;
struct Cyc_List_List*_tmpAB;struct Cyc_List_List*_tmpAC;_LL13F: if((int)_tmpAA == 0){
goto _LL140;}else{goto _LL141;}_LL141: if((unsigned int)_tmpAA > 1?*((int*)_tmpAA)== 
0: 0){_LL145: _tmpAB=((struct Cyc_Absyn_Rel_n_struct*)_tmpAA)->f1;goto _LL142;}else{
goto _LL143;}_LL143: if((unsigned int)_tmpAA > 1?*((int*)_tmpAA)== 1: 0){_LL146:
_tmpAC=((struct Cyc_Absyn_Abs_n_struct*)_tmpAA)->f1;goto _LL144;}else{goto _LL13E;}
_LL140: _tmpAB=0;goto _LL142;_LL142: match=0;_tmpA9=_tmpAB;goto _LL13E;_LL144: match=
Cyc_Absynpp_use_curr_namespace?((int(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(
Cyc_Std_strptrcmp,_tmpAC,Cyc_Absynpp_curr_namespace): 0;_tmpA9=(Cyc_Absynpp_qvar_to_Cids?
Cyc_Absynpp_add_cyc_prefix: 0)?({struct Cyc_List_List*_tmpAD=_cycalloc(sizeof(*
_tmpAD));_tmpAD->hd=Cyc_Absynpp_cyc_stringptr;_tmpAD->tl=_tmpAC;_tmpAD;}): _tmpAC;
goto _LL13E;_LL13E:;}if(Cyc_Absynpp_qvar_to_Cids){return Cyc_PP_text((struct
_tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmpA9,({struct Cyc_List_List*_tmpAE=
_cycalloc(sizeof(*_tmpAE));_tmpAE->hd=(*q).f2;_tmpAE->tl=0;_tmpAE;})),_tag_arr("_",
sizeof(unsigned char),2)));}else{if(match){return Cyc_Absynpp_var2doc((*q).f2);}
else{return Cyc_PP_text((struct _tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA9,({struct Cyc_List_List*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->hd=(*q).f2;_tmpAF->tl=0;_tmpAF;})),
_tag_arr("::",sizeof(unsigned char),3)));}}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids){return Cyc_Absynpp_qvar2doc(v);}if(
Cyc_Absynpp_use_curr_namespace){void*_tmpB0=(*v).f1;struct Cyc_List_List*_tmpB1;
struct Cyc_List_List*_tmpB2;_LL148: if((int)_tmpB0 == 0){goto _LL149;}else{goto
_LL14A;}_LL14A: if((unsigned int)_tmpB0 > 1?*((int*)_tmpB0)== 0: 0){_LL150: _tmpB1=((
struct Cyc_Absyn_Rel_n_struct*)_tmpB0)->f1;if(_tmpB1 == 0){goto _LL14B;}else{goto
_LL14C;}}else{goto _LL14C;}_LL14C: if((unsigned int)_tmpB0 > 1?*((int*)_tmpB0)== 1:
0){_LL151: _tmpB2=((struct Cyc_Absyn_Abs_n_struct*)_tmpB0)->f1;goto _LL14D;}else{
goto _LL14E;}_LL14E: goto _LL14F;_LL149: goto _LL14B;_LL14B: return Cyc_Absynpp_var2doc((*
v).f2);_LL14D: if(((int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Std_strptrcmp,
_tmpB2,Cyc_Absynpp_curr_namespace)== 0){return Cyc_Absynpp_var2doc((*v).f2);}
else{goto _LL14F;}_LL14F: return({struct Cyc_PP_Doc*_tmpB3[2];_tmpB3[1]=Cyc_Absynpp_qvar2doc(
v);_tmpB3[0]=Cyc_PP_text(_tag_arr("/* bad namespace : */ ",sizeof(unsigned char),
23));Cyc_PP_cat(_tag_arr(_tmpB3,sizeof(struct Cyc_PP_Doc*),2));});_LL147:;}else{
return Cyc_Absynpp_var2doc((*v).f2);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(),t,0);}int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*e){void*_tmpB4=(void*)e->r;void*_tmpB5;struct Cyc_Absyn_Exp*
_tmpB6;struct Cyc_Absyn_Exp*_tmpB7;_LL153: if(*((int*)_tmpB4)== 0){goto _LL154;}
else{goto _LL155;}_LL155: if(*((int*)_tmpB4)== 1){goto _LL156;}else{goto _LL157;}
_LL157: if(*((int*)_tmpB4)== 2){goto _LL158;}else{goto _LL159;}_LL159: if(*((int*)
_tmpB4)== 3){_LL19F: _tmpB5=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmpB4)->f1;
goto _LL15A;}else{goto _LL15B;}_LL15B: if(*((int*)_tmpB4)== 4){goto _LL15C;}else{
goto _LL15D;}_LL15D: if(*((int*)_tmpB4)== 5){goto _LL15E;}else{goto _LL15F;}_LL15F:
if(*((int*)_tmpB4)== 6){goto _LL160;}else{goto _LL161;}_LL161: if(*((int*)_tmpB4)== 
7){goto _LL162;}else{goto _LL163;}_LL163: if(*((int*)_tmpB4)== 8){goto _LL164;}else{
goto _LL165;}_LL165: if(*((int*)_tmpB4)== 9){goto _LL166;}else{goto _LL167;}_LL167:
if(*((int*)_tmpB4)== 10){goto _LL168;}else{goto _LL169;}_LL169: if(*((int*)_tmpB4)
== 11){_LL1A0: _tmpB6=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpB4)->f1;goto
_LL16A;}else{goto _LL16B;}_LL16B: if(*((int*)_tmpB4)== 12){_LL1A1: _tmpB7=((struct
Cyc_Absyn_Instantiate_e_struct*)_tmpB4)->f1;goto _LL16C;}else{goto _LL16D;}_LL16D:
if(*((int*)_tmpB4)== 13){goto _LL16E;}else{goto _LL16F;}_LL16F: if(*((int*)_tmpB4)
== 15){goto _LL170;}else{goto _LL171;}_LL171: if(*((int*)_tmpB4)== 14){goto _LL172;}
else{goto _LL173;}_LL173: if(*((int*)_tmpB4)== 16){goto _LL174;}else{goto _LL175;}
_LL175: if(*((int*)_tmpB4)== 17){goto _LL176;}else{goto _LL177;}_LL177: if(*((int*)
_tmpB4)== 18){goto _LL178;}else{goto _LL179;}_LL179: if(*((int*)_tmpB4)== 19){goto
_LL17A;}else{goto _LL17B;}_LL17B: if(*((int*)_tmpB4)== 20){goto _LL17C;}else{goto
_LL17D;}_LL17D: if(*((int*)_tmpB4)== 21){goto _LL17E;}else{goto _LL17F;}_LL17F: if(*((
int*)_tmpB4)== 22){goto _LL180;}else{goto _LL181;}_LL181: if(*((int*)_tmpB4)== 23){
goto _LL182;}else{goto _LL183;}_LL183: if(*((int*)_tmpB4)== 24){goto _LL184;}else{
goto _LL185;}_LL185: if(*((int*)_tmpB4)== 25){goto _LL186;}else{goto _LL187;}_LL187:
if(*((int*)_tmpB4)== 26){goto _LL188;}else{goto _LL189;}_LL189: if(*((int*)_tmpB4)
== 27){goto _LL18A;}else{goto _LL18B;}_LL18B: if(*((int*)_tmpB4)== 28){goto _LL18C;}
else{goto _LL18D;}_LL18D: if(*((int*)_tmpB4)== 29){goto _LL18E;}else{goto _LL18F;}
_LL18F: if(*((int*)_tmpB4)== 30){goto _LL190;}else{goto _LL191;}_LL191: if(*((int*)
_tmpB4)== 31){goto _LL192;}else{goto _LL193;}_LL193: if(*((int*)_tmpB4)== 32){goto
_LL194;}else{goto _LL195;}_LL195: if(*((int*)_tmpB4)== 33){goto _LL196;}else{goto
_LL197;}_LL197: if(*((int*)_tmpB4)== 34){goto _LL198;}else{goto _LL199;}_LL199: if(*((
int*)_tmpB4)== 35){goto _LL19A;}else{goto _LL19B;}_LL19B: if(*((int*)_tmpB4)== 36){
goto _LL19C;}else{goto _LL19D;}_LL19D: if(*((int*)_tmpB4)== 37){goto _LL19E;}else{
goto _LL152;}_LL154: goto _LL156;_LL156: goto _LL158;_LL158: return 10000;_LL15A: {void*
_tmpB8=_tmpB5;_LL1A3: if((int)_tmpB8 == 0){goto _LL1A4;}else{goto _LL1A5;}_LL1A5: if((
int)_tmpB8 == 1){goto _LL1A6;}else{goto _LL1A7;}_LL1A7: if((int)_tmpB8 == 2){goto
_LL1A8;}else{goto _LL1A9;}_LL1A9: if((int)_tmpB8 == 3){goto _LL1AA;}else{goto _LL1AB;}
_LL1AB: if((int)_tmpB8 == 4){goto _LL1AC;}else{goto _LL1AD;}_LL1AD: if((int)_tmpB8 == 
5){goto _LL1AE;}else{goto _LL1AF;}_LL1AF: if((int)_tmpB8 == 6){goto _LL1B0;}else{goto
_LL1B1;}_LL1B1: if((int)_tmpB8 == 7){goto _LL1B2;}else{goto _LL1B3;}_LL1B3: if((int)
_tmpB8 == 8){goto _LL1B4;}else{goto _LL1B5;}_LL1B5: if((int)_tmpB8 == 9){goto _LL1B6;}
else{goto _LL1B7;}_LL1B7: if((int)_tmpB8 == 10){goto _LL1B8;}else{goto _LL1B9;}_LL1B9:
if((int)_tmpB8 == 11){goto _LL1BA;}else{goto _LL1BB;}_LL1BB: if((int)_tmpB8 == 12){
goto _LL1BC;}else{goto _LL1BD;}_LL1BD: if((int)_tmpB8 == 13){goto _LL1BE;}else{goto
_LL1BF;}_LL1BF: if((int)_tmpB8 == 14){goto _LL1C0;}else{goto _LL1C1;}_LL1C1: if((int)
_tmpB8 == 15){goto _LL1C2;}else{goto _LL1C3;}_LL1C3: if((int)_tmpB8 == 16){goto _LL1C4;}
else{goto _LL1C5;}_LL1C5: if((int)_tmpB8 == 17){goto _LL1C6;}else{goto _LL1C7;}_LL1C7:
if((int)_tmpB8 == 18){goto _LL1C8;}else{goto _LL1C9;}_LL1C9: if((int)_tmpB8 == 19){
goto _LL1CA;}else{goto _LL1A2;}_LL1A4: return 100;_LL1A6: return 110;_LL1A8: return 100;
_LL1AA: goto _LL1AC;_LL1AC: return 110;_LL1AE: goto _LL1B0;_LL1B0: return 70;_LL1B2: goto
_LL1B4;_LL1B4: goto _LL1B6;_LL1B6: goto _LL1B8;_LL1B8: return 80;_LL1BA: goto _LL1BC;
_LL1BC: return 130;_LL1BE: return 60;_LL1C0: return 40;_LL1C2: return 50;_LL1C4: return 90;
_LL1C6: return 80;_LL1C8: return 80;_LL1CA: return 140;_LL1A2:;}_LL15C: return 20;_LL15E:
return 130;_LL160: return 30;_LL162: return 10;_LL164: goto _LL166;_LL166: return 140;
_LL168: return 130;_LL16A: return Cyc_Absynpp_exp_prec(_tmpB6);_LL16C: return Cyc_Absynpp_exp_prec(
_tmpB7);_LL16E: return 120;_LL170: return 15;_LL172: goto _LL174;_LL174: goto _LL176;
_LL176: goto _LL178;_LL178: goto _LL17A;_LL17A: goto _LL17C;_LL17C: return 130;_LL17E:
goto _LL180;_LL180: goto _LL182;_LL182: return 140;_LL184: return 150;_LL186: goto _LL188;
_LL188: goto _LL18A;_LL18A: goto _LL18C;_LL18C: goto _LL18E;_LL18E: goto _LL190;_LL190:
goto _LL192;_LL192: goto _LL194;_LL194: goto _LL196;_LL196: goto _LL198;_LL198: return
140;_LL19A: return 10000;_LL19C: goto _LL19E;_LL19E: return 140;_LL152:;}struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){int
myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*s;{void*_tmpB9=(void*)e->r;void*
_tmpBA;struct _tuple0*_tmpBB;struct _tuple0*_tmpBC;struct Cyc_List_List*_tmpBD;void*
_tmpBE;struct Cyc_Absyn_Exp*_tmpBF;struct Cyc_Core_Opt*_tmpC0;struct Cyc_Absyn_Exp*
_tmpC1;void*_tmpC2;struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_Absyn_Exp*_tmpC4;struct
Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC7;
struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_List_List*_tmpC9;struct Cyc_Absyn_Exp*_tmpCA;
struct Cyc_List_List*_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Exp*_tmpCD;
struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;
void*_tmpD1;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*
_tmpD4;void*_tmpD5;struct Cyc_Absyn_Exp*_tmpD6;void*_tmpD7;struct _tagged_arr*
_tmpD8;void*_tmpD9;void*_tmpDA;unsigned int _tmpDB;void*_tmpDC;void*_tmpDD;struct
Cyc_List_List*_tmpDE;struct Cyc_Absyn_Exp*_tmpDF;struct _tagged_arr*_tmpE0;struct
Cyc_Absyn_Exp*_tmpE1;struct _tagged_arr*_tmpE2;struct Cyc_Absyn_Exp*_tmpE3;struct
Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_List_List*_tmpE6;
struct Cyc_List_List*_tmpE7;struct _tuple1*_tmpE8;struct Cyc_List_List*_tmpE9;
struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Vardecl*
_tmpEC;struct Cyc_List_List*_tmpED;struct _tuple0*_tmpEE;struct Cyc_List_List*
_tmpEF;struct Cyc_Absyn_Tunionfield*_tmpF0;struct Cyc_List_List*_tmpF1;struct
_tuple0*_tmpF2;struct _tuple0*_tmpF3;struct Cyc_Absyn_MallocInfo _tmpF4;struct Cyc_Absyn_Exp*
_tmpF5;void**_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;int _tmpF8;struct Cyc_List_List*
_tmpF9;struct Cyc_Core_Opt*_tmpFA;struct Cyc_Absyn_Stmt*_tmpFB;struct Cyc_Absyn_Fndecl*
_tmpFC;struct Cyc_Absyn_Exp*_tmpFD;_LL1CC: if(*((int*)_tmpB9)== 0){_LL21A: _tmpBA=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmpB9)->f1;goto _LL1CD;}else{goto _LL1CE;}
_LL1CE: if(*((int*)_tmpB9)== 1){_LL21B: _tmpBB=((struct Cyc_Absyn_Var_e_struct*)
_tmpB9)->f1;goto _LL1CF;}else{goto _LL1D0;}_LL1D0: if(*((int*)_tmpB9)== 2){_LL21C:
_tmpBC=((struct Cyc_Absyn_UnknownId_e_struct*)_tmpB9)->f1;goto _LL1D1;}else{goto
_LL1D2;}_LL1D2: if(*((int*)_tmpB9)== 3){_LL21E: _tmpBE=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmpB9)->f1;goto _LL21D;_LL21D: _tmpBD=((struct Cyc_Absyn_Primop_e_struct*)_tmpB9)->f2;
goto _LL1D3;}else{goto _LL1D4;}_LL1D4: if(*((int*)_tmpB9)== 4){_LL221: _tmpC1=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpB9)->f1;goto _LL220;_LL220: _tmpC0=((struct
Cyc_Absyn_AssignOp_e_struct*)_tmpB9)->f2;goto _LL21F;_LL21F: _tmpBF=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpB9)->f3;goto _LL1D5;}else{goto _LL1D6;}_LL1D6: if(*((int*)_tmpB9)== 5){_LL223:
_tmpC3=((struct Cyc_Absyn_Increment_e_struct*)_tmpB9)->f1;goto _LL222;_LL222:
_tmpC2=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpB9)->f2;goto _LL1D7;}
else{goto _LL1D8;}_LL1D8: if(*((int*)_tmpB9)== 6){_LL226: _tmpC6=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB9)->f1;goto _LL225;_LL225: _tmpC5=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB9)->f2;goto _LL224;_LL224: _tmpC4=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB9)->f3;goto _LL1D9;}else{goto _LL1DA;}_LL1DA: if(*((int*)_tmpB9)== 7){_LL228:
_tmpC8=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB9)->f1;goto _LL227;_LL227: _tmpC7=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmpB9)->f2;goto _LL1DB;}else{goto _LL1DC;}_LL1DC:
if(*((int*)_tmpB9)== 8){_LL22A: _tmpCA=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmpB9)->f1;goto _LL229;_LL229: _tmpC9=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmpB9)->f2;goto _LL1DD;}else{goto _LL1DE;}_LL1DE: if(*((int*)_tmpB9)== 9){_LL22C:
_tmpCC=((struct Cyc_Absyn_FnCall_e_struct*)_tmpB9)->f1;goto _LL22B;_LL22B: _tmpCB=((
struct Cyc_Absyn_FnCall_e_struct*)_tmpB9)->f2;goto _LL1DF;}else{goto _LL1E0;}_LL1E0:
if(*((int*)_tmpB9)== 10){_LL22D: _tmpCD=((struct Cyc_Absyn_Throw_e_struct*)_tmpB9)->f1;
goto _LL1E1;}else{goto _LL1E2;}_LL1E2: if(*((int*)_tmpB9)== 11){_LL22E: _tmpCE=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpB9)->f1;goto _LL1E3;}else{goto _LL1E4;}
_LL1E4: if(*((int*)_tmpB9)== 12){_LL22F: _tmpCF=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpB9)->f1;goto _LL1E5;}else{goto _LL1E6;}_LL1E6: if(*((int*)_tmpB9)== 13){_LL231:
_tmpD1=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpB9)->f1;goto _LL230;_LL230:
_tmpD0=((struct Cyc_Absyn_Cast_e_struct*)_tmpB9)->f2;goto _LL1E7;}else{goto _LL1E8;}
_LL1E8: if(*((int*)_tmpB9)== 14){_LL232: _tmpD2=((struct Cyc_Absyn_Address_e_struct*)
_tmpB9)->f1;goto _LL1E9;}else{goto _LL1EA;}_LL1EA: if(*((int*)_tmpB9)== 15){_LL234:
_tmpD4=((struct Cyc_Absyn_New_e_struct*)_tmpB9)->f1;goto _LL233;_LL233: _tmpD3=((
struct Cyc_Absyn_New_e_struct*)_tmpB9)->f2;goto _LL1EB;}else{goto _LL1EC;}_LL1EC:
if(*((int*)_tmpB9)== 16){_LL235: _tmpD5=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpB9)->f1;goto _LL1ED;}else{goto _LL1EE;}_LL1EE: if(*((int*)_tmpB9)== 17){_LL236:
_tmpD6=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpB9)->f1;goto _LL1EF;}else{goto
_LL1F0;}_LL1F0: if(*((int*)_tmpB9)== 18){_LL239: _tmpD9=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpB9)->f1;goto _LL237;_LL237: _tmpD7=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpB9)->f2;if(*((int*)_tmpD7)== 0){_LL238: _tmpD8=((struct Cyc_Absyn_StructField_struct*)
_tmpD7)->f1;goto _LL1F1;}else{goto _LL1F2;}}else{goto _LL1F2;}_LL1F2: if(*((int*)
_tmpB9)== 18){_LL23C: _tmpDC=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpB9)->f1;
goto _LL23A;_LL23A: _tmpDA=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpB9)->f2;
if(*((int*)_tmpDA)== 1){_LL23B: _tmpDB=((struct Cyc_Absyn_TupleIndex_struct*)
_tmpDA)->f1;goto _LL1F3;}else{goto _LL1F4;}}else{goto _LL1F4;}_LL1F4: if(*((int*)
_tmpB9)== 19){_LL23E: _tmpDE=((struct Cyc_Absyn_Gentyp_e_struct*)_tmpB9)->f1;goto
_LL23D;_LL23D: _tmpDD=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmpB9)->f2;goto
_LL1F5;}else{goto _LL1F6;}_LL1F6: if(*((int*)_tmpB9)== 20){_LL23F: _tmpDF=((struct
Cyc_Absyn_Deref_e_struct*)_tmpB9)->f1;goto _LL1F7;}else{goto _LL1F8;}_LL1F8: if(*((
int*)_tmpB9)== 21){_LL241: _tmpE1=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpB9)->f1;
goto _LL240;_LL240: _tmpE0=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpB9)->f2;goto
_LL1F9;}else{goto _LL1FA;}_LL1FA: if(*((int*)_tmpB9)== 22){_LL243: _tmpE3=((struct
Cyc_Absyn_AggrArrow_e_struct*)_tmpB9)->f1;goto _LL242;_LL242: _tmpE2=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpB9)->f2;goto _LL1FB;}else{goto _LL1FC;}_LL1FC: if(*((int*)_tmpB9)== 23){_LL245:
_tmpE5=((struct Cyc_Absyn_Subscript_e_struct*)_tmpB9)->f1;goto _LL244;_LL244:
_tmpE4=((struct Cyc_Absyn_Subscript_e_struct*)_tmpB9)->f2;goto _LL1FD;}else{goto
_LL1FE;}_LL1FE: if(*((int*)_tmpB9)== 24){_LL246: _tmpE6=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpB9)->f1;goto _LL1FF;}else{goto _LL200;}_LL200: if(*((int*)_tmpB9)== 25){_LL248:
_tmpE8=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpB9)->f1;goto _LL247;_LL247:
_tmpE7=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpB9)->f2;goto _LL201;}else{goto
_LL202;}_LL202: if(*((int*)_tmpB9)== 26){_LL249: _tmpE9=((struct Cyc_Absyn_Array_e_struct*)
_tmpB9)->f1;goto _LL203;}else{goto _LL204;}_LL204: if(*((int*)_tmpB9)== 27){_LL24C:
_tmpEC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB9)->f1;goto _LL24B;_LL24B:
_tmpEB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB9)->f2;goto _LL24A;_LL24A:
_tmpEA=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB9)->f3;goto _LL205;}else{
goto _LL206;}_LL206: if(*((int*)_tmpB9)== 28){_LL24E: _tmpEE=((struct Cyc_Absyn_Struct_e_struct*)
_tmpB9)->f1;goto _LL24D;_LL24D: _tmpED=((struct Cyc_Absyn_Struct_e_struct*)_tmpB9)->f2;
goto _LL207;}else{goto _LL208;}_LL208: if(*((int*)_tmpB9)== 29){_LL24F: _tmpEF=((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmpB9)->f2;goto _LL209;}else{goto _LL20A;}
_LL20A: if(*((int*)_tmpB9)== 30){_LL251: _tmpF1=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpB9)->f1;goto _LL250;_LL250: _tmpF0=((struct Cyc_Absyn_Tunion_e_struct*)_tmpB9)->f3;
goto _LL20B;}else{goto _LL20C;}_LL20C: if(*((int*)_tmpB9)== 31){_LL252: _tmpF2=((
struct Cyc_Absyn_Enum_e_struct*)_tmpB9)->f1;goto _LL20D;}else{goto _LL20E;}_LL20E:
if(*((int*)_tmpB9)== 32){_LL253: _tmpF3=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpB9)->f1;goto _LL20F;}else{goto _LL210;}_LL210: if(*((int*)_tmpB9)== 33){_LL254:
_tmpF4=((struct Cyc_Absyn_Malloc_e_struct*)_tmpB9)->f1;_LL258: _tmpF8=_tmpF4.is_calloc;
goto _LL257;_LL257: _tmpF7=_tmpF4.rgn;goto _LL256;_LL256: _tmpF6=_tmpF4.elt_type;
goto _LL255;_LL255: _tmpF5=_tmpF4.num_elts;goto _LL211;}else{goto _LL212;}_LL212: if(*((
int*)_tmpB9)== 34){_LL25A: _tmpFA=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpB9)->f1;goto _LL259;_LL259: _tmpF9=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpB9)->f2;goto _LL213;}else{goto _LL214;}_LL214: if(*((int*)_tmpB9)== 35){_LL25B:
_tmpFB=((struct Cyc_Absyn_StmtExp_e_struct*)_tmpB9)->f1;goto _LL215;}else{goto
_LL216;}_LL216: if(*((int*)_tmpB9)== 36){_LL25C: _tmpFC=((struct Cyc_Absyn_Codegen_e_struct*)
_tmpB9)->f1;goto _LL217;}else{goto _LL218;}_LL218: if(*((int*)_tmpB9)== 37){_LL25D:
_tmpFD=((struct Cyc_Absyn_Fill_e_struct*)_tmpB9)->f1;goto _LL219;}else{goto _LL1CB;}
_LL1CD: s=Cyc_Absynpp_cnst2doc(_tmpBA);goto _LL1CB;_LL1CF: _tmpBC=_tmpBB;goto _LL1D1;
_LL1D1: s=Cyc_Absynpp_qvar2doc(_tmpBC);goto _LL1CB;_LL1D3: s=Cyc_Absynpp_primapp2doc(
myprec,_tmpBE,_tmpBD);goto _LL1CB;_LL1D5: s=({struct Cyc_PP_Doc*_tmpFE[5];_tmpFE[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmpBF);_tmpFE[3]=Cyc_PP_text(_tag_arr("= ",
sizeof(unsigned char),3));_tmpFE[2]=_tmpC0 == 0? Cyc_PP_text(_tag_arr("",sizeof(
unsigned char),1)): Cyc_Absynpp_prim2doc((void*)_tmpC0->v);_tmpFE[1]=Cyc_PP_text(
_tag_arr(" ",sizeof(unsigned char),2));_tmpFE[0]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmpC1);Cyc_PP_cat(_tag_arr(_tmpFE,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1CB;
_LL1D7: {struct Cyc_PP_Doc*_tmpFF=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC3);{void*
_tmp100=_tmpC2;_LL25F: if((int)_tmp100 == 0){goto _LL260;}else{goto _LL261;}_LL261:
if((int)_tmp100 == 2){goto _LL262;}else{goto _LL263;}_LL263: if((int)_tmp100 == 1){
goto _LL264;}else{goto _LL265;}_LL265: if((int)_tmp100 == 3){goto _LL266;}else{goto
_LL25E;}_LL260: s=({struct Cyc_PP_Doc*_tmp101[2];_tmp101[1]=_tmpFF;_tmp101[0]=Cyc_PP_text(
_tag_arr("++",sizeof(unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp101,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL25E;_LL262: s=({struct Cyc_PP_Doc*_tmp102[2];
_tmp102[1]=_tmpFF;_tmp102[0]=Cyc_PP_text(_tag_arr("--",sizeof(unsigned char),3));
Cyc_PP_cat(_tag_arr(_tmp102,sizeof(struct Cyc_PP_Doc*),2));});goto _LL25E;_LL264: s=({
struct Cyc_PP_Doc*_tmp103[2];_tmp103[1]=Cyc_PP_text(_tag_arr("++",sizeof(
unsigned char),3));_tmp103[0]=_tmpFF;Cyc_PP_cat(_tag_arr(_tmp103,sizeof(struct
Cyc_PP_Doc*),2));});goto _LL25E;_LL266: s=({struct Cyc_PP_Doc*_tmp104[2];_tmp104[1]=
Cyc_PP_text(_tag_arr("--",sizeof(unsigned char),3));_tmp104[0]=_tmpFF;Cyc_PP_cat(
_tag_arr(_tmp104,sizeof(struct Cyc_PP_Doc*),2));});goto _LL25E;_LL25E:;}goto _LL1CB;}
_LL1D9:{struct _tuple9 _tmp106=({struct _tuple9 _tmp105;_tmp105.f1=(void*)_tmpC5->r;
_tmp105.f2=(void*)_tmpC4->r;_tmp105;});_LL268: goto _LL269;_LL269: s=({struct Cyc_PP_Doc*
_tmp107[5];_tmp107[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC4);_tmp107[3]=Cyc_PP_text(
_tag_arr(" : ",sizeof(unsigned char),4));_tmp107[2]=Cyc_Absynpp_exp2doc_prec(0,
_tmpC5);_tmp107[1]=Cyc_PP_text(_tag_arr(" ? ",sizeof(unsigned char),4));_tmp107[
0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC6);Cyc_PP_cat(_tag_arr(_tmp107,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL267;_LL267:;}goto _LL1CB;_LL1DB: s=({struct Cyc_PP_Doc*
_tmp108[5];_tmp108[4]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp108[
3]=Cyc_Absynpp_exp2doc(_tmpC7);_tmp108[2]=Cyc_PP_text(_tag_arr(", ",sizeof(
unsigned char),3));_tmp108[1]=Cyc_Absynpp_exp2doc(_tmpC8);_tmp108[0]=Cyc_PP_text(
_tag_arr("(",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp108,sizeof(struct
Cyc_PP_Doc*),5));});goto _LL1CB;_LL1DD: s=({struct Cyc_PP_Doc*_tmp109[4];_tmp109[3]=
Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp109[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmpC9);_tmp109[1]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));_tmp109[
0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpCA);Cyc_PP_cat(_tag_arr(_tmp109,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL1CB;_LL1DF: s=({struct Cyc_PP_Doc*_tmp10A[4];
_tmp10A[3]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp10A[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmpCB);_tmp10A[1]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));_tmp10A[
0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpCC);Cyc_PP_cat(_tag_arr(_tmp10A,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL1CB;_LL1E1: s=({struct Cyc_PP_Doc*_tmp10B[2];
_tmp10B[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpCD);_tmp10B[0]=Cyc_PP_text(
_tag_arr("throw ",sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(_tmp10B,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL1CB;_LL1E3: s=Cyc_Absynpp_exp2doc_prec(inprec,
_tmpCE);goto _LL1CB;_LL1E5: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmpCF);goto _LL1CB;
_LL1E7: s=({struct Cyc_PP_Doc*_tmp10C[4];_tmp10C[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpD0);_tmp10C[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));
_tmp10C[1]=Cyc_Absynpp_typ2doc(_tmpD1);_tmp10C[0]=Cyc_PP_text(_tag_arr("(",
sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp10C,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL1CB;_LL1E9: s=({struct Cyc_PP_Doc*_tmp10D[2];_tmp10D[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpD2);_tmp10D[0]=Cyc_PP_text(_tag_arr("&",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp10D,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1CB;_LL1EB: if(_tmpD4 == 
0){s=({struct Cyc_PP_Doc*_tmp10E[2];_tmp10E[1]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmpD3);_tmp10E[0]=Cyc_PP_text(_tag_arr("new ",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp10E,sizeof(struct Cyc_PP_Doc*),2));});}else{s=({struct Cyc_PP_Doc*
_tmp10F[4];_tmp10F[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpD3);_tmp10F[2]=Cyc_PP_text(
_tag_arr(") ",sizeof(unsigned char),3));_tmp10F[1]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(_tmpD4));_tmp10F[0]=Cyc_PP_text(_tag_arr("rnew(",
sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp10F,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL1CB;_LL1ED: s=({struct Cyc_PP_Doc*_tmp110[3];_tmp110[2]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp110[1]=Cyc_Absynpp_typ2doc(_tmpD5);
_tmp110[0]=Cyc_PP_text(_tag_arr("sizeof(",sizeof(unsigned char),8));Cyc_PP_cat(
_tag_arr(_tmp110,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1CB;_LL1EF: s=({struct
Cyc_PP_Doc*_tmp111[3];_tmp111[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),
2));_tmp111[1]=Cyc_Absynpp_exp2doc(_tmpD6);_tmp111[0]=Cyc_PP_text(_tag_arr("sizeof(",
sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp111,sizeof(struct Cyc_PP_Doc*),
3));});goto _LL1CB;_LL1F1: s=({struct Cyc_PP_Doc*_tmp112[5];_tmp112[4]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp112[3]=Cyc_PP_textptr(_tmpD8);_tmp112[
2]=Cyc_PP_text(_tag_arr(",",sizeof(unsigned char),2));_tmp112[1]=Cyc_Absynpp_typ2doc(
_tmpD9);_tmp112[0]=Cyc_PP_text(_tag_arr("offsetof(",sizeof(unsigned char),10));
Cyc_PP_cat(_tag_arr(_tmp112,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1CB;_LL1F3: s=({
struct Cyc_PP_Doc*_tmp113[5];_tmp113[4]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp113[3]=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp115;_tmp115.tag=1;_tmp115.f1=(int)_tmpDB;{void*_tmp114[1]={& _tmp115};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp114,sizeof(void*),1));}}));
_tmp113[2]=Cyc_PP_text(_tag_arr(",",sizeof(unsigned char),2));_tmp113[1]=Cyc_Absynpp_typ2doc(
_tmpDC);_tmp113[0]=Cyc_PP_text(_tag_arr("offsetof(",sizeof(unsigned char),10));
Cyc_PP_cat(_tag_arr(_tmp113,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1CB;_LL1F5: s=({
struct Cyc_PP_Doc*_tmp116[4];_tmp116[3]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp116[2]=Cyc_Absynpp_typ2doc(_tmpDD);_tmp116[1]=Cyc_Absynpp_tvars2doc(
_tmpDE);_tmp116[0]=Cyc_PP_text(_tag_arr("__gen(",sizeof(unsigned char),7));Cyc_PP_cat(
_tag_arr(_tmp116,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1CB;_LL1F7: s=({struct
Cyc_PP_Doc*_tmp117[2];_tmp117[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpDF);_tmp117[
0]=Cyc_PP_text(_tag_arr("*",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(
_tmp117,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1CB;_LL1F9: s=({struct Cyc_PP_Doc*
_tmp118[3];_tmp118[2]=Cyc_PP_textptr(_tmpE0);_tmp118[1]=Cyc_PP_text(_tag_arr(".",
sizeof(unsigned char),2));_tmp118[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpE1);Cyc_PP_cat(
_tag_arr(_tmp118,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1CB;_LL1FB: s=({struct
Cyc_PP_Doc*_tmp119[3];_tmp119[2]=Cyc_PP_textptr(_tmpE2);_tmp119[1]=Cyc_PP_text(
_tag_arr("->",sizeof(unsigned char),3));_tmp119[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpE3);Cyc_PP_cat(_tag_arr(_tmp119,sizeof(struct Cyc_PP_Doc*),3));});goto
_LL1CB;_LL1FD: s=({struct Cyc_PP_Doc*_tmp11A[4];_tmp11A[3]=Cyc_PP_text(_tag_arr("]",
sizeof(unsigned char),2));_tmp11A[2]=Cyc_Absynpp_exp2doc(_tmpE4);_tmp11A[1]=Cyc_PP_text(
_tag_arr("[",sizeof(unsigned char),2));_tmp11A[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpE5);Cyc_PP_cat(_tag_arr(_tmp11A,sizeof(struct Cyc_PP_Doc*),4));});goto
_LL1CB;_LL1FF: s=({struct Cyc_PP_Doc*_tmp11B[3];_tmp11B[2]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp11B[1]=Cyc_Absynpp_exps2doc_prec(20,_tmpE6);
_tmp11B[0]=Cyc_PP_text(_tag_arr("$(",sizeof(unsigned char),3));Cyc_PP_cat(
_tag_arr(_tmp11B,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1CB;_LL201: s=({struct
Cyc_PP_Doc*_tmp11C[4];_tmp11C[3]=Cyc_PP_group(_tag_arr("{",sizeof(unsigned char),
2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmpE7));_tmp11C[2]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp11C[1]=Cyc_Absynpp_typ2doc((*_tmpE8).f3);_tmp11C[0]=
Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp11C,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL1CB;_LL203: s=Cyc_PP_group(_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmpE9));goto
_LL1CB;_LL205: s=({struct Cyc_PP_Doc*_tmp11D[7];_tmp11D[6]=Cyc_PP_text(_tag_arr("}",
sizeof(unsigned char),2));_tmp11D[5]=Cyc_Absynpp_exp2doc(_tmpEA);_tmp11D[4]=Cyc_PP_text(
_tag_arr(" : ",sizeof(unsigned char),4));_tmp11D[3]=Cyc_Absynpp_exp2doc(_tmpEB);
_tmp11D[2]=Cyc_PP_text(_tag_arr(" < ",sizeof(unsigned char),4));_tmp11D[1]=Cyc_PP_text(*(*
_tmpEC->name).f2);_tmp11D[0]=Cyc_PP_text(_tag_arr("{for ",sizeof(unsigned char),
6));Cyc_PP_cat(_tag_arr(_tmp11D,sizeof(struct Cyc_PP_Doc*),7));});goto _LL1CB;
_LL207: s=({struct Cyc_PP_Doc*_tmp11E[2];_tmp11E[1]=Cyc_PP_group(_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmpED));
_tmp11E[0]=Cyc_Absynpp_qvar2doc(_tmpEE);Cyc_PP_cat(_tag_arr(_tmp11E,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL1CB;_LL209: s=Cyc_PP_group(_tag_arr("{",sizeof(
unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple7*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmpEF));goto _LL1CB;
_LL20B: if(_tmpF1 == 0){s=Cyc_Absynpp_qvar2doc(_tmpF0->name);}else{s=({struct Cyc_PP_Doc*
_tmp11F[2];_tmp11F[1]=Cyc_PP_egroup(_tag_arr("(",sizeof(unsigned char),2),
_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmpF1));_tmp11F[0]=Cyc_Absynpp_qvar2doc(
_tmpF0->name);Cyc_PP_cat(_tag_arr(_tmp11F,sizeof(struct Cyc_PP_Doc*),2));});}goto
_LL1CB;_LL20D: s=Cyc_Absynpp_qvar2doc(_tmpF2);goto _LL1CB;_LL20F: s=Cyc_Absynpp_qvar2doc(
_tmpF3);goto _LL1CB;_LL211: if(_tmpF8){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmpF6)),0);if(_tmpF7 == 0){s=({struct Cyc_PP_Doc*_tmp120[5];
_tmp120[4]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp120[3]=Cyc_Absynpp_exp2doc(
st);_tmp120[2]=Cyc_PP_text(_tag_arr(",",sizeof(unsigned char),2));_tmp120[1]=Cyc_Absynpp_exp2doc(
_tmpF5);_tmp120[0]=Cyc_PP_text(_tag_arr("calloc(",sizeof(unsigned char),8));Cyc_PP_cat(
_tag_arr(_tmp120,sizeof(struct Cyc_PP_Doc*),5));});}else{s=({struct Cyc_PP_Doc*
_tmp121[7];_tmp121[6]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp121[
5]=Cyc_Absynpp_exp2doc(st);_tmp121[4]=Cyc_PP_text(_tag_arr(",",sizeof(
unsigned char),2));_tmp121[3]=Cyc_Absynpp_exp2doc(_tmpF5);_tmp121[2]=Cyc_PP_text(
_tag_arr(",",sizeof(unsigned char),2));_tmp121[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmpF7));_tmp121[0]=Cyc_PP_text(_tag_arr("rcalloc(",sizeof(
unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp121,sizeof(struct Cyc_PP_Doc*),7));});}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmpF6 == 0){new_e=_tmpF5;}else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmpF6,0),_tmpF5,0);}if(_tmpF7 == 0){s=({struct Cyc_PP_Doc*
_tmp122[3];_tmp122[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp122[
1]=Cyc_Absynpp_exp2doc(new_e);_tmp122[0]=Cyc_PP_text(_tag_arr("malloc(",sizeof(
unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp122,sizeof(struct Cyc_PP_Doc*),3));});}
else{s=({struct Cyc_PP_Doc*_tmp123[5];_tmp123[4]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp123[3]=Cyc_Absynpp_exp2doc(new_e);_tmp123[2]=Cyc_PP_text(
_tag_arr(",",sizeof(unsigned char),2));_tmp123[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmpF7));_tmp123[0]=Cyc_PP_text(_tag_arr("rmalloc(",sizeof(
unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp123,sizeof(struct Cyc_PP_Doc*),5));});}}
goto _LL1CB;_LL213: s=Cyc_PP_group(_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpF9));goto _LL1CB;_LL215: s=({struct Cyc_PP_Doc*_tmp124[3];_tmp124[2]=Cyc_PP_text(
_tag_arr(" })",sizeof(unsigned char),4));_tmp124[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmpFB));_tmp124[0]=Cyc_PP_text(_tag_arr("({ ",sizeof(unsigned char),4));Cyc_PP_cat(
_tag_arr(_tmp124,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1CB;_LL217: s=({struct
Cyc_PP_Doc*_tmp125[3];_tmp125[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),
2));_tmp125[1]=Cyc_PP_nest(2,Cyc_Absynpp_decl2doc(({struct Cyc_Absyn_Decl*_tmp126=
_cycalloc(sizeof(*_tmp126));_tmp126->r=(void*)((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp128;_tmp128.tag=1;_tmp128.f1=_tmpFC;_tmp128;});_tmp127;}));_tmp126->loc=e->loc;
_tmp126;})));_tmp125[0]=Cyc_PP_text(_tag_arr("codegen(",sizeof(unsigned char),9));
Cyc_PP_cat(_tag_arr(_tmp125,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1CB;_LL219: s=({
struct Cyc_PP_Doc*_tmp129[3];_tmp129[2]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp129[1]=Cyc_PP_nest(2,Cyc_Absynpp_exp2doc(_tmpFD));_tmp129[
0]=Cyc_PP_text(_tag_arr("fill(",sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(
_tmp129,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1CB;_LL1CB:;}if(inprec >= myprec){
s=({struct Cyc_PP_Doc*_tmp12A[3];_tmp12A[2]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp12A[1]=s;_tmp12A[0]=Cyc_PP_text(_tag_arr("(",sizeof(
unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp12A,sizeof(struct Cyc_PP_Doc*),3));});}
return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){void*_tmp12B=d;
struct Cyc_Absyn_Exp*_tmp12C;struct _tagged_arr*_tmp12D;_LL26B: if(*((int*)_tmp12B)
== 0){_LL26F: _tmp12C=((struct Cyc_Absyn_ArrayElement_struct*)_tmp12B)->f1;goto
_LL26C;}else{goto _LL26D;}_LL26D: if(*((int*)_tmp12B)== 1){_LL270: _tmp12D=((struct
Cyc_Absyn_FieldName_struct*)_tmp12B)->f1;goto _LL26E;}else{goto _LL26A;}_LL26C:
return({struct Cyc_PP_Doc*_tmp12E[3];_tmp12E[2]=Cyc_PP_text(_tag_arr("]",sizeof(
unsigned char),2));_tmp12E[1]=Cyc_Absynpp_exp2doc(_tmp12C);_tmp12E[0]=Cyc_PP_text(
_tag_arr(".[",sizeof(unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp12E,sizeof(
struct Cyc_PP_Doc*),3));});_LL26E: return({struct Cyc_PP_Doc*_tmp12F[2];_tmp12F[1]=
Cyc_PP_textptr(_tmp12D);_tmp12F[0]=Cyc_PP_text(_tag_arr(".",sizeof(unsigned char),
2));Cyc_PP_cat(_tag_arr(_tmp12F,sizeof(struct Cyc_PP_Doc*),2));});_LL26A:;}struct
Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple7*de){if((*de).f1 == 0){return Cyc_Absynpp_exp2doc((*
de).f2);}else{return({struct Cyc_PP_Doc*_tmp130[2];_tmp130[1]=Cyc_Absynpp_exp2doc((*
de).f2);_tmp130[0]=Cyc_PP_egroup(_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",
sizeof(unsigned char),2),_tag_arr("=",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*
de).f1));Cyc_PP_cat(_tag_arr(_tmp130,sizeof(struct Cyc_PP_Doc*),2));});}}struct
Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){return Cyc_PP_group(
_tag_arr("",sizeof(unsigned char),1),_tag_arr("",sizeof(unsigned char),1),
_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*c){void*_tmp131=c;
unsigned char _tmp132;void*_tmp133;short _tmp134;void*_tmp135;int _tmp136;void*
_tmp137;int _tmp138;void*_tmp139;long long _tmp13A;void*_tmp13B;struct _tagged_arr
_tmp13C;struct _tagged_arr _tmp13D;_LL272: if((unsigned int)_tmp131 > 1?*((int*)
_tmp131)== 0: 0){_LL283: _tmp133=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp131)->f1;
goto _LL282;_LL282: _tmp132=((struct Cyc_Absyn_Char_c_struct*)_tmp131)->f2;goto
_LL273;}else{goto _LL274;}_LL274: if((unsigned int)_tmp131 > 1?*((int*)_tmp131)== 1:
0){_LL285: _tmp135=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp131)->f1;goto
_LL284;_LL284: _tmp134=((struct Cyc_Absyn_Short_c_struct*)_tmp131)->f2;goto _LL275;}
else{goto _LL276;}_LL276: if((unsigned int)_tmp131 > 1?*((int*)_tmp131)== 2: 0){
_LL287: _tmp137=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp131)->f1;if((int)
_tmp137 == 0){goto _LL286;}else{goto _LL278;}_LL286: _tmp136=((struct Cyc_Absyn_Int_c_struct*)
_tmp131)->f2;goto _LL277;}else{goto _LL278;}_LL278: if((unsigned int)_tmp131 > 1?*((
int*)_tmp131)== 2: 0){_LL289: _tmp139=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp131)->f1;if((int)_tmp139 == 1){goto _LL288;}else{goto _LL27A;}_LL288: _tmp138=((
struct Cyc_Absyn_Int_c_struct*)_tmp131)->f2;goto _LL279;}else{goto _LL27A;}_LL27A:
if((unsigned int)_tmp131 > 1?*((int*)_tmp131)== 3: 0){_LL28B: _tmp13B=(void*)((
struct Cyc_Absyn_LongLong_c_struct*)_tmp131)->f1;goto _LL28A;_LL28A: _tmp13A=((
struct Cyc_Absyn_LongLong_c_struct*)_tmp131)->f2;goto _LL27B;}else{goto _LL27C;}
_LL27C: if((unsigned int)_tmp131 > 1?*((int*)_tmp131)== 4: 0){_LL28C: _tmp13C=((
struct Cyc_Absyn_Float_c_struct*)_tmp131)->f1;goto _LL27D;}else{goto _LL27E;}_LL27E:
if((int)_tmp131 == 0){goto _LL27F;}else{goto _LL280;}_LL280: if((unsigned int)
_tmp131 > 1?*((int*)_tmp131)== 5: 0){_LL28D: _tmp13D=((struct Cyc_Absyn_String_c_struct*)
_tmp131)->f1;goto _LL281;}else{goto _LL271;}_LL273: return Cyc_PP_text((struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp13F;_tmp13F.tag=0;_tmp13F.f1=(
struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp132);{void*_tmp13E[1]={& _tmp13F};
Cyc_Std_aprintf(_tag_arr("'%s'",sizeof(unsigned char),5),_tag_arr(_tmp13E,
sizeof(void*),1));}}));_LL275: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp141;_tmp141.tag=1;_tmp141.f1=(int)((unsigned int)((int)_tmp134));{void*
_tmp140[1]={& _tmp141};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),
_tag_arr(_tmp140,sizeof(void*),1));}}));_LL277: return Cyc_PP_text((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp143;_tmp143.tag=1;_tmp143.f1=(int)((
unsigned int)_tmp136);{void*_tmp142[1]={& _tmp143};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(unsigned char),3),_tag_arr(_tmp142,sizeof(void*),1));}}));_LL279: return
Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp145;_tmp145.tag=
1;_tmp145.f1=(unsigned int)_tmp138;{void*_tmp144[1]={& _tmp145};Cyc_Std_aprintf(
_tag_arr("%u",sizeof(unsigned char),3),_tag_arr(_tmp144,sizeof(void*),1));}}));
_LL27B: return Cyc_PP_text(_tag_arr("<<FIX LONG LONG CONSTANT>>",sizeof(
unsigned char),27));_LL27D: return Cyc_PP_text(_tmp13C);_LL27F: return Cyc_PP_text(
_tag_arr("NULL",sizeof(unsigned char),5));_LL281: return({struct Cyc_PP_Doc*
_tmp146[3];_tmp146[2]=Cyc_PP_text(_tag_arr("\"",sizeof(unsigned char),2));
_tmp146[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp13D));_tmp146[0]=Cyc_PP_text(
_tag_arr("\"",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp146,sizeof(
struct Cyc_PP_Doc*),3));});_LL271:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int
inprec,void*p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == (void*)19){if(es == 0? 1: es->tl != 0){(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147[0]=({struct Cyc_Core_Failure_struct
_tmp148;_tmp148.tag=Cyc_Core_Failure;_tmp148.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp14A;_tmp14A.tag=0;_tmp14A.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp149[1]={& _tmp14A};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof(unsigned char),44),_tag_arr(_tmp149,sizeof(void*),1));}});_tmp148;});
_tmp147;}));}{struct Cyc_PP_Doc*_tmp14B=Cyc_Absynpp_exp2doc_prec(inprec,(struct
Cyc_Absyn_Exp*)es->hd);return({struct Cyc_PP_Doc*_tmp14C[2];_tmp14C[1]=Cyc_PP_text(
_tag_arr(".size",sizeof(unsigned char),6));_tmp14C[0]=_tmp14B;Cyc_PP_cat(
_tag_arr(_tmp14C,sizeof(struct Cyc_PP_Doc*),2));});}}else{struct Cyc_List_List*ds=((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);if(ds
== 0){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp14D=_cycalloc(
sizeof(*_tmp14D));_tmp14D[0]=({struct Cyc_Core_Failure_struct _tmp14E;_tmp14E.tag=
Cyc_Core_Failure;_tmp14E.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp150;_tmp150.tag=0;_tmp150.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp14F[1]={& _tmp150};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc: %s with no args",
sizeof(unsigned char),38),_tag_arr(_tmp14F,sizeof(void*),1));}});_tmp14E;});
_tmp14D;}));}else{if(ds->tl == 0){return({struct Cyc_PP_Doc*_tmp151[3];_tmp151[2]=(
struct Cyc_PP_Doc*)ds->hd;_tmp151[1]=Cyc_PP_text(_tag_arr(" ",sizeof(
unsigned char),2));_tmp151[0]=ps;Cyc_PP_cat(_tag_arr(_tmp151,sizeof(struct Cyc_PP_Doc*),
3));});}else{if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp152=_cycalloc(sizeof(*_tmp152));
_tmp152[0]=({struct Cyc_Core_Failure_struct _tmp153;_tmp153.tag=Cyc_Core_Failure;
_tmp153.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp155;_tmp155.tag=
0;_tmp155.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{void*_tmp154[1]={&
_tmp155};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof(unsigned char),47),_tag_arr(_tmp154,sizeof(void*),1));}});_tmp153;});
_tmp152;}));}else{return({struct Cyc_PP_Doc*_tmp156[5];_tmp156[4]=(struct Cyc_PP_Doc*)((
struct Cyc_List_List*)_check_null(ds->tl))->hd;_tmp156[3]=Cyc_PP_text(_tag_arr(" ",
sizeof(unsigned char),2));_tmp156[2]=ps;_tmp156[1]=Cyc_PP_text(_tag_arr(" ",
sizeof(unsigned char),2));_tmp156[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(
_tag_arr(_tmp156,sizeof(struct Cyc_PP_Doc*),5));});}}}}}struct _tagged_arr Cyc_Absynpp_prim2str(
void*p){void*_tmp157=p;_LL28F: if((int)_tmp157 == 0){goto _LL290;}else{goto _LL291;}
_LL291: if((int)_tmp157 == 1){goto _LL292;}else{goto _LL293;}_LL293: if((int)_tmp157
== 2){goto _LL294;}else{goto _LL295;}_LL295: if((int)_tmp157 == 3){goto _LL296;}else{
goto _LL297;}_LL297: if((int)_tmp157 == 4){goto _LL298;}else{goto _LL299;}_LL299: if((
int)_tmp157 == 5){goto _LL29A;}else{goto _LL29B;}_LL29B: if((int)_tmp157 == 6){goto
_LL29C;}else{goto _LL29D;}_LL29D: if((int)_tmp157 == 7){goto _LL29E;}else{goto _LL29F;}
_LL29F: if((int)_tmp157 == 8){goto _LL2A0;}else{goto _LL2A1;}_LL2A1: if((int)_tmp157
== 9){goto _LL2A2;}else{goto _LL2A3;}_LL2A3: if((int)_tmp157 == 10){goto _LL2A4;}
else{goto _LL2A5;}_LL2A5: if((int)_tmp157 == 11){goto _LL2A6;}else{goto _LL2A7;}
_LL2A7: if((int)_tmp157 == 12){goto _LL2A8;}else{goto _LL2A9;}_LL2A9: if((int)_tmp157
== 13){goto _LL2AA;}else{goto _LL2AB;}_LL2AB: if((int)_tmp157 == 14){goto _LL2AC;}
else{goto _LL2AD;}_LL2AD: if((int)_tmp157 == 15){goto _LL2AE;}else{goto _LL2AF;}
_LL2AF: if((int)_tmp157 == 16){goto _LL2B0;}else{goto _LL2B1;}_LL2B1: if((int)_tmp157
== 17){goto _LL2B2;}else{goto _LL2B3;}_LL2B3: if((int)_tmp157 == 18){goto _LL2B4;}
else{goto _LL2B5;}_LL2B5: if((int)_tmp157 == 19){goto _LL2B6;}else{goto _LL28E;}
_LL290: return _tag_arr("+",sizeof(unsigned char),2);_LL292: return _tag_arr("*",
sizeof(unsigned char),2);_LL294: return _tag_arr("-",sizeof(unsigned char),2);
_LL296: return _tag_arr("/",sizeof(unsigned char),2);_LL298: return _tag_arr("%",
sizeof(unsigned char),2);_LL29A: return _tag_arr("==",sizeof(unsigned char),3);
_LL29C: return _tag_arr("!=",sizeof(unsigned char),3);_LL29E: return _tag_arr(">",
sizeof(unsigned char),2);_LL2A0: return _tag_arr("<",sizeof(unsigned char),2);
_LL2A2: return _tag_arr(">=",sizeof(unsigned char),3);_LL2A4: return _tag_arr("<=",
sizeof(unsigned char),3);_LL2A6: return _tag_arr("!",sizeof(unsigned char),2);
_LL2A8: return _tag_arr("~",sizeof(unsigned char),2);_LL2AA: return _tag_arr("&",
sizeof(unsigned char),2);_LL2AC: return _tag_arr("|",sizeof(unsigned char),2);
_LL2AE: return _tag_arr("^",sizeof(unsigned char),2);_LL2B0: return _tag_arr("<<",
sizeof(unsigned char),3);_LL2B2: return _tag_arr(">>",sizeof(unsigned char),3);
_LL2B4: return _tag_arr(">>>",sizeof(unsigned char),4);_LL2B6: return _tag_arr("size",
sizeof(unsigned char),5);_LL28E:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(struct
Cyc_Absyn_Stmt*s){void*_tmp158=(void*)s->r;_LL2B8: if((unsigned int)_tmp158 > 1?*((
int*)_tmp158)== 12: 0){goto _LL2B9;}else{goto _LL2BA;}_LL2BA: goto _LL2BB;_LL2B9:
return 1;_LL2BB: return 0;_LL2B7:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*
st){struct Cyc_PP_Doc*s;{void*_tmp159=(void*)st->r;struct Cyc_Absyn_Exp*_tmp15A;
struct Cyc_Absyn_Stmt*_tmp15B;struct Cyc_Absyn_Stmt*_tmp15C;struct Cyc_Absyn_Exp*
_tmp15D;struct Cyc_Absyn_Stmt*_tmp15E;struct Cyc_Absyn_Stmt*_tmp15F;struct Cyc_Absyn_Exp*
_tmp160;struct Cyc_Absyn_Stmt*_tmp161;struct _tuple2 _tmp162;struct Cyc_Absyn_Exp*
_tmp163;struct _tagged_arr*_tmp164;struct Cyc_Absyn_Stmt*_tmp165;struct _tuple2
_tmp166;struct Cyc_Absyn_Exp*_tmp167;struct _tuple2 _tmp168;struct Cyc_Absyn_Exp*
_tmp169;struct Cyc_Absyn_Exp*_tmp16A;struct Cyc_Absyn_ForArrayInfo _tmp16B;struct
Cyc_Absyn_Stmt*_tmp16C;struct _tuple2 _tmp16D;struct Cyc_Absyn_Exp*_tmp16E;struct
_tuple2 _tmp16F;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_List_List*_tmp171;struct Cyc_List_List*
_tmp172;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_List_List*_tmp174;struct Cyc_Absyn_Exp*
_tmp175;struct Cyc_List_List*_tmp176;struct Cyc_List_List*_tmp177;struct Cyc_Absyn_Stmt*
_tmp178;struct Cyc_Absyn_Decl*_tmp179;struct Cyc_Absyn_Stmt*_tmp17A;struct Cyc_Absyn_Stmt*
_tmp17B;struct Cyc_Absyn_Stmt*_tmp17C;struct _tagged_arr*_tmp17D;struct _tuple2
_tmp17E;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_Absyn_Stmt*_tmp180;struct Cyc_List_List*
_tmp181;struct Cyc_Absyn_Stmt*_tmp182;struct Cyc_Absyn_Stmt*_tmp183;struct Cyc_Absyn_Vardecl*
_tmp184;struct Cyc_Absyn_Tvar*_tmp185;_LL2BD: if((int)_tmp159 == 0){goto _LL2BE;}
else{goto _LL2BF;}_LL2BF: if((unsigned int)_tmp159 > 1?*((int*)_tmp159)== 0: 0){
_LL2E9: _tmp15A=((struct Cyc_Absyn_Exp_s_struct*)_tmp159)->f1;goto _LL2C0;}else{
goto _LL2C1;}_LL2C1: if((unsigned int)_tmp159 > 1?*((int*)_tmp159)== 1: 0){_LL2EB:
_tmp15C=((struct Cyc_Absyn_Seq_s_struct*)_tmp159)->f1;goto _LL2EA;_LL2EA: _tmp15B=((
struct Cyc_Absyn_Seq_s_struct*)_tmp159)->f2;goto _LL2C2;}else{goto _LL2C3;}_LL2C3:
if((unsigned int)_tmp159 > 1?*((int*)_tmp159)== 2: 0){_LL2EC: _tmp15D=((struct Cyc_Absyn_Return_s_struct*)
_tmp159)->f1;goto _LL2C4;}else{goto _LL2C5;}_LL2C5: if((unsigned int)_tmp159 > 1?*((
int*)_tmp159)== 3: 0){_LL2EF: _tmp160=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp159)->f1;goto _LL2EE;_LL2EE: _tmp15F=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp159)->f2;goto _LL2ED;_LL2ED: _tmp15E=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp159)->f3;goto _LL2C6;}else{goto _LL2C7;}_LL2C7: if((unsigned int)_tmp159 > 1?*((
int*)_tmp159)== 4: 0){_LL2F1: _tmp162=((struct Cyc_Absyn_While_s_struct*)_tmp159)->f1;
_LL2F2: _tmp163=_tmp162.f1;goto _LL2F0;_LL2F0: _tmp161=((struct Cyc_Absyn_While_s_struct*)
_tmp159)->f2;goto _LL2C8;}else{goto _LL2C9;}_LL2C9: if((unsigned int)_tmp159 > 1?*((
int*)_tmp159)== 5: 0){goto _LL2CA;}else{goto _LL2CB;}_LL2CB: if((unsigned int)
_tmp159 > 1?*((int*)_tmp159)== 6: 0){goto _LL2CC;}else{goto _LL2CD;}_LL2CD: if((
unsigned int)_tmp159 > 1?*((int*)_tmp159)== 7: 0){_LL2F3: _tmp164=((struct Cyc_Absyn_Goto_s_struct*)
_tmp159)->f1;goto _LL2CE;}else{goto _LL2CF;}_LL2CF: if((unsigned int)_tmp159 > 1?*((
int*)_tmp159)== 8: 0){_LL2F9: _tmp16A=((struct Cyc_Absyn_For_s_struct*)_tmp159)->f1;
goto _LL2F7;_LL2F7: _tmp168=((struct Cyc_Absyn_For_s_struct*)_tmp159)->f2;_LL2F8:
_tmp169=_tmp168.f1;goto _LL2F5;_LL2F5: _tmp166=((struct Cyc_Absyn_For_s_struct*)
_tmp159)->f3;_LL2F6: _tmp167=_tmp166.f1;goto _LL2F4;_LL2F4: _tmp165=((struct Cyc_Absyn_For_s_struct*)
_tmp159)->f4;goto _LL2D0;}else{goto _LL2D1;}_LL2D1: if((unsigned int)_tmp159 > 1?*((
int*)_tmp159)== 19: 0){_LL2FA: _tmp16B=((struct Cyc_Absyn_ForArray_s_struct*)
_tmp159)->f1;_LL300: _tmp171=_tmp16B.defns;goto _LL2FE;_LL2FE: _tmp16F=_tmp16B.condition;
_LL2FF: _tmp170=_tmp16F.f1;goto _LL2FC;_LL2FC: _tmp16D=_tmp16B.delta;_LL2FD: _tmp16E=
_tmp16D.f1;goto _LL2FB;_LL2FB: _tmp16C=_tmp16B.body;goto _LL2D2;}else{goto _LL2D3;}
_LL2D3: if((unsigned int)_tmp159 > 1?*((int*)_tmp159)== 9: 0){_LL302: _tmp173=((
struct Cyc_Absyn_Switch_s_struct*)_tmp159)->f1;goto _LL301;_LL301: _tmp172=((struct
Cyc_Absyn_Switch_s_struct*)_tmp159)->f2;goto _LL2D4;}else{goto _LL2D5;}_LL2D5: if((
unsigned int)_tmp159 > 1?*((int*)_tmp159)== 10: 0){_LL304: _tmp175=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmp159)->f1;goto _LL303;_LL303: _tmp174=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmp159)->f2;goto _LL2D6;}else{goto _LL2D7;}_LL2D7: if((unsigned int)_tmp159 > 1?*((
int*)_tmp159)== 11: 0){_LL305: _tmp176=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp159)->f1;if(_tmp176 == 0){goto _LL2D8;}else{goto _LL2D9;}}else{goto _LL2D9;}
_LL2D9: if((unsigned int)_tmp159 > 1?*((int*)_tmp159)== 11: 0){_LL306: _tmp177=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp159)->f1;goto _LL2DA;}else{goto _LL2DB;}
_LL2DB: if((unsigned int)_tmp159 > 1?*((int*)_tmp159)== 12: 0){_LL308: _tmp179=((
struct Cyc_Absyn_Decl_s_struct*)_tmp159)->f1;goto _LL307;_LL307: _tmp178=((struct
Cyc_Absyn_Decl_s_struct*)_tmp159)->f2;goto _LL2DC;}else{goto _LL2DD;}_LL2DD: if((
unsigned int)_tmp159 > 1?*((int*)_tmp159)== 13: 0){_LL309: _tmp17A=((struct Cyc_Absyn_Cut_s_struct*)
_tmp159)->f1;goto _LL2DE;}else{goto _LL2DF;}_LL2DF: if((unsigned int)_tmp159 > 1?*((
int*)_tmp159)== 14: 0){_LL30A: _tmp17B=((struct Cyc_Absyn_Splice_s_struct*)_tmp159)->f1;
goto _LL2E0;}else{goto _LL2E1;}_LL2E1: if((unsigned int)_tmp159 > 1?*((int*)_tmp159)
== 15: 0){_LL30C: _tmp17D=((struct Cyc_Absyn_Label_s_struct*)_tmp159)->f1;goto
_LL30B;_LL30B: _tmp17C=((struct Cyc_Absyn_Label_s_struct*)_tmp159)->f2;goto _LL2E2;}
else{goto _LL2E3;}_LL2E3: if((unsigned int)_tmp159 > 1?*((int*)_tmp159)== 16: 0){
_LL30F: _tmp180=((struct Cyc_Absyn_Do_s_struct*)_tmp159)->f1;goto _LL30D;_LL30D:
_tmp17E=((struct Cyc_Absyn_Do_s_struct*)_tmp159)->f2;_LL30E: _tmp17F=_tmp17E.f1;
goto _LL2E4;}else{goto _LL2E5;}_LL2E5: if((unsigned int)_tmp159 > 1?*((int*)_tmp159)
== 17: 0){_LL311: _tmp182=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp159)->f1;goto
_LL310;_LL310: _tmp181=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp159)->f2;goto
_LL2E6;}else{goto _LL2E7;}_LL2E7: if((unsigned int)_tmp159 > 1?*((int*)_tmp159)== 
18: 0){_LL314: _tmp185=((struct Cyc_Absyn_Region_s_struct*)_tmp159)->f1;goto _LL313;
_LL313: _tmp184=((struct Cyc_Absyn_Region_s_struct*)_tmp159)->f2;goto _LL312;_LL312:
_tmp183=((struct Cyc_Absyn_Region_s_struct*)_tmp159)->f3;goto _LL2E8;}else{goto
_LL2BC;}_LL2BE: s=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));goto _LL2BC;
_LL2C0: s=({struct Cyc_PP_Doc*_tmp186[2];_tmp186[1]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp186[0]=Cyc_Absynpp_exp2doc(_tmp15A);Cyc_PP_cat(
_tag_arr(_tmp186,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2BC;_LL2C2: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp15C)){s=({struct Cyc_PP_Doc*_tmp187[6];_tmp187[
5]=Cyc_Absynpp_is_declaration(_tmp15B)?({struct Cyc_PP_Doc*_tmp188[4];_tmp188[3]=
Cyc_PP_line_doc();_tmp188[2]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));
_tmp188[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp15B));_tmp188[0]=Cyc_PP_text(
_tag_arr("{ ",sizeof(unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp188,sizeof(
struct Cyc_PP_Doc*),4));}): Cyc_Absynpp_stmt2doc(_tmp15B);_tmp187[4]=Cyc_PP_line_doc();
_tmp187[3]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp187[2]=Cyc_PP_line_doc();
_tmp187[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp15C));_tmp187[0]=Cyc_PP_text(
_tag_arr("{ ",sizeof(unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp187,sizeof(
struct Cyc_PP_Doc*),6));});}else{if(Cyc_Absynpp_is_declaration(_tmp15B)){s=({
struct Cyc_PP_Doc*_tmp189[6];_tmp189[5]=Cyc_PP_line_doc();_tmp189[4]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp189[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp15B));_tmp189[2]=Cyc_PP_text(_tag_arr("{ ",sizeof(unsigned char),3));_tmp189[
1]=Cyc_PP_line_doc();_tmp189[0]=Cyc_Absynpp_stmt2doc(_tmp15C);Cyc_PP_cat(
_tag_arr(_tmp189,sizeof(struct Cyc_PP_Doc*),6));});}else{s=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,_tag_arr("",sizeof(unsigned char),1),({
struct Cyc_List_List*_tmp18A=_cycalloc(sizeof(*_tmp18A));_tmp18A->hd=_tmp15C;
_tmp18A->tl=({struct Cyc_List_List*_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B->hd=
_tmp15B;_tmp18B->tl=0;_tmp18B;});_tmp18A;}));}}}else{s=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,_tag_arr("",sizeof(unsigned char),1),({
struct Cyc_List_List*_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C->hd=_tmp15C;
_tmp18C->tl=({struct Cyc_List_List*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->hd=
_tmp15B;_tmp18D->tl=0;_tmp18D;});_tmp18C;}));}goto _LL2BC;_LL2C4: if(_tmp15D == 0){
s=Cyc_PP_text(_tag_arr("return;",sizeof(unsigned char),8));}else{s=({struct Cyc_PP_Doc*
_tmp18E[3];_tmp18E[2]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmp18E[
1]=_tmp15D == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmp15D));_tmp18E[0]=Cyc_PP_text(_tag_arr("return ",sizeof(
unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp18E,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL2BC;_LL2C6: {int print_else;{void*_tmp18F=(void*)_tmp15E->r;_LL316: if((int)
_tmp18F == 0){goto _LL317;}else{goto _LL318;}_LL318: goto _LL319;_LL317: print_else=0;
goto _LL315;_LL319: print_else=1;goto _LL315;_LL315:;}s=({struct Cyc_PP_Doc*_tmp190[
7];_tmp190[6]=print_else?({struct Cyc_PP_Doc*_tmp192[5];_tmp192[4]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp192[3]=Cyc_PP_line_doc();_tmp192[2]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp193[2];_tmp193[1]=Cyc_Absynpp_stmt2doc(
_tmp15E);_tmp193[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp193,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp192[1]=Cyc_PP_text(_tag_arr("else {",sizeof(
unsigned char),7));_tmp192[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp192,
sizeof(struct Cyc_PP_Doc*),5));}): Cyc_PP_nil_doc();_tmp190[5]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp190[4]=Cyc_PP_line_doc();_tmp190[3]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp191[2];_tmp191[1]=Cyc_Absynpp_stmt2doc(
_tmp15F);_tmp191[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp191,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp190[2]=Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),
4));_tmp190[1]=Cyc_Absynpp_exp2doc(_tmp160);_tmp190[0]=Cyc_PP_text(_tag_arr("if (",
sizeof(unsigned char),5));Cyc_PP_cat(_tag_arr(_tmp190,sizeof(struct Cyc_PP_Doc*),
7));});goto _LL2BC;}_LL2C8: s=({struct Cyc_PP_Doc*_tmp194[6];_tmp194[5]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp194[4]=Cyc_PP_line_doc();_tmp194[3]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp195[2];_tmp195[1]=Cyc_Absynpp_stmt2doc(
_tmp161);_tmp195[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp195,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp194[2]=Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),
4));_tmp194[1]=Cyc_Absynpp_exp2doc(_tmp163);_tmp194[0]=Cyc_PP_text(_tag_arr("while (",
sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp194,sizeof(struct Cyc_PP_Doc*),
6));});goto _LL2BC;_LL2CA: s=Cyc_PP_text(_tag_arr("break;",sizeof(unsigned char),7));
goto _LL2BC;_LL2CC: s=Cyc_PP_text(_tag_arr("continue;",sizeof(unsigned char),10));
goto _LL2BC;_LL2CE: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp197;_tmp197.tag=0;_tmp197.f1=(struct _tagged_arr)*_tmp164;{void*_tmp196[1]={&
_tmp197};Cyc_Std_aprintf(_tag_arr("goto %s;",sizeof(unsigned char),9),_tag_arr(
_tmp196,sizeof(void*),1));}}));goto _LL2BC;_LL2D0: s=({struct Cyc_PP_Doc*_tmp198[10];
_tmp198[9]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp198[8]=Cyc_PP_line_doc();
_tmp198[7]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp199[2];_tmp199[1]=Cyc_Absynpp_stmt2doc(
_tmp165);_tmp199[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp199,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp198[6]=Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),
4));_tmp198[5]=Cyc_Absynpp_exp2doc(_tmp167);_tmp198[4]=Cyc_PP_text(_tag_arr("; ",
sizeof(unsigned char),3));_tmp198[3]=Cyc_Absynpp_exp2doc(_tmp169);_tmp198[2]=Cyc_PP_text(
_tag_arr("; ",sizeof(unsigned char),3));_tmp198[1]=Cyc_Absynpp_exp2doc(_tmp16A);
_tmp198[0]=Cyc_PP_text(_tag_arr("for(",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp198,sizeof(struct Cyc_PP_Doc*),10));});goto _LL2BC;_LL2D2: s=({struct
Cyc_PP_Doc*_tmp19A[9];_tmp19A[8]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),
2));_tmp19A[7]=Cyc_PP_line_doc();_tmp19A[6]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp19B[2];_tmp19B[1]=Cyc_Absynpp_stmt2doc(_tmp16C);_tmp19B[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp19B,sizeof(struct Cyc_PP_Doc*),2));}));_tmp19A[5]=Cyc_PP_text(
_tag_arr(") {",sizeof(unsigned char),4));_tmp19A[4]=Cyc_Absynpp_exp2doc(_tmp16E);
_tmp19A[3]=Cyc_PP_text(_tag_arr("; ",sizeof(unsigned char),3));_tmp19A[2]=Cyc_Absynpp_exp2doc(
_tmp170);_tmp19A[1]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_vardecl2doc,
_tag_arr(" ",sizeof(unsigned char),2),_tmp171);_tmp19A[0]=Cyc_PP_text(_tag_arr("forarray(",
sizeof(unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp19A,sizeof(struct Cyc_PP_Doc*),
9));});goto _LL2BC;_LL2D4: s=({struct Cyc_PP_Doc*_tmp19C[7];_tmp19C[6]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp19C[5]=Cyc_PP_line_doc();_tmp19C[4]=
Cyc_Absynpp_switchclauses2doc(_tmp172);_tmp19C[3]=Cyc_PP_line_doc();_tmp19C[2]=
Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),4));_tmp19C[1]=Cyc_Absynpp_exp2doc(
_tmp173);_tmp19C[0]=Cyc_PP_text(_tag_arr("switch (",sizeof(unsigned char),9));
Cyc_PP_cat(_tag_arr(_tmp19C,sizeof(struct Cyc_PP_Doc*),7));});goto _LL2BC;_LL2D6: s=({
struct Cyc_PP_Doc*_tmp19D[7];_tmp19D[6]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp19D[5]=Cyc_PP_line_doc();_tmp19D[4]=Cyc_Absynpp_switchCclauses2doc(
_tmp174);_tmp19D[3]=Cyc_PP_line_doc();_tmp19D[2]=Cyc_PP_text(_tag_arr(") {",
sizeof(unsigned char),4));_tmp19D[1]=Cyc_Absynpp_exp2doc(_tmp175);_tmp19D[0]=Cyc_PP_text(
_tag_arr("switch \"C\" (",sizeof(unsigned char),13));Cyc_PP_cat(_tag_arr(_tmp19D,
sizeof(struct Cyc_PP_Doc*),7));});goto _LL2BC;_LL2D8: s=Cyc_PP_text(_tag_arr("fallthru;",
sizeof(unsigned char),10));goto _LL2BC;_LL2DA: s=({struct Cyc_PP_Doc*_tmp19E[3];
_tmp19E[2]=Cyc_PP_text(_tag_arr(");",sizeof(unsigned char),3));_tmp19E[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp177);_tmp19E[0]=Cyc_PP_text(_tag_arr("fallthru(",sizeof(unsigned char),10));
Cyc_PP_cat(_tag_arr(_tmp19E,sizeof(struct Cyc_PP_Doc*),3));});goto _LL2BC;_LL2DC: s=({
struct Cyc_PP_Doc*_tmp19F[3];_tmp19F[2]=Cyc_Absynpp_stmt2doc(_tmp178);_tmp19F[1]=
Cyc_PP_line_doc();_tmp19F[0]=Cyc_Absynpp_decl2doc(_tmp179);Cyc_PP_cat(_tag_arr(
_tmp19F,sizeof(struct Cyc_PP_Doc*),3));});goto _LL2BC;_LL2DE: s=({struct Cyc_PP_Doc*
_tmp1A0[2];_tmp1A0[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp17A));_tmp1A0[0]=
Cyc_PP_text(_tag_arr("cut ",sizeof(unsigned char),5));Cyc_PP_cat(_tag_arr(
_tmp1A0,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2BC;_LL2E0: s=({struct Cyc_PP_Doc*
_tmp1A1[2];_tmp1A1[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp17B));_tmp1A1[0]=
Cyc_PP_text(_tag_arr("splice ",sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(
_tmp1A1,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2BC;_LL2E2: if(Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration(_tmp17C): 0){s=({struct Cyc_PP_Doc*_tmp1A2[6];_tmp1A2[5]=
Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp1A2[4]=Cyc_PP_line_doc();
_tmp1A2[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp17C));_tmp1A2[2]=Cyc_PP_line_doc();
_tmp1A2[1]=Cyc_PP_text(_tag_arr(": {",sizeof(unsigned char),4));_tmp1A2[0]=Cyc_PP_textptr(
_tmp17D);Cyc_PP_cat(_tag_arr(_tmp1A2,sizeof(struct Cyc_PP_Doc*),6));});}else{s=({
struct Cyc_PP_Doc*_tmp1A3[3];_tmp1A3[2]=Cyc_Absynpp_stmt2doc(_tmp17C);_tmp1A3[1]=
Cyc_PP_text(_tag_arr(": ",sizeof(unsigned char),3));_tmp1A3[0]=Cyc_PP_textptr(
_tmp17D);Cyc_PP_cat(_tag_arr(_tmp1A3,sizeof(struct Cyc_PP_Doc*),3));});}goto
_LL2BC;_LL2E4: s=({struct Cyc_PP_Doc*_tmp1A4[7];_tmp1A4[6]=Cyc_PP_text(_tag_arr(");",
sizeof(unsigned char),3));_tmp1A4[5]=Cyc_Absynpp_exp2doc(_tmp17F);_tmp1A4[4]=Cyc_PP_text(
_tag_arr("} while (",sizeof(unsigned char),10));_tmp1A4[3]=Cyc_PP_line_doc();
_tmp1A4[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp180));_tmp1A4[1]=Cyc_PP_line_doc();
_tmp1A4[0]=Cyc_PP_text(_tag_arr("do {",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp1A4,sizeof(struct Cyc_PP_Doc*),7));});goto _LL2BC;_LL2E6: s=({struct
Cyc_PP_Doc*_tmp1A5[9];_tmp1A5[8]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),
2));_tmp1A5[7]=Cyc_PP_line_doc();_tmp1A5[6]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp181));_tmp1A5[5]=Cyc_PP_line_doc();_tmp1A5[4]=Cyc_PP_text(_tag_arr("} catch {",
sizeof(unsigned char),10));_tmp1A5[3]=Cyc_PP_line_doc();_tmp1A5[2]=Cyc_PP_nest(2,
Cyc_Absynpp_stmt2doc(_tmp182));_tmp1A5[1]=Cyc_PP_line_doc();_tmp1A5[0]=Cyc_PP_text(
_tag_arr("try {",sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp1A5,sizeof(
struct Cyc_PP_Doc*),9));});goto _LL2BC;_LL2E8: s=({struct Cyc_PP_Doc*_tmp1A6[9];
_tmp1A6[8]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp1A6[7]=Cyc_PP_line_doc();
_tmp1A6[6]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp183));_tmp1A6[5]=Cyc_PP_line_doc();
_tmp1A6[4]=Cyc_PP_text(_tag_arr("{",sizeof(unsigned char),2));_tmp1A6[3]=Cyc_Absynpp_qvar2doc(
_tmp184->name);_tmp1A6[2]=Cyc_PP_text(_tag_arr(">",sizeof(unsigned char),2));
_tmp1A6[1]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp185));_tmp1A6[0]=Cyc_PP_text(
_tag_arr("region<",sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp1A6,sizeof(
struct Cyc_PP_Doc*),9));});goto _LL2BC;_LL2BC:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp1A7=(void*)p->r;int _tmp1A8;
void*_tmp1A9;unsigned char _tmp1AA;struct _tagged_arr _tmp1AB;struct Cyc_Absyn_Vardecl*
_tmp1AC;struct Cyc_List_List*_tmp1AD;struct Cyc_Absyn_Pat*_tmp1AE;struct Cyc_Absyn_Vardecl*
_tmp1AF;struct _tuple0*_tmp1B0;struct Cyc_List_List*_tmp1B1;struct Cyc_List_List*
_tmp1B2;struct _tuple0*_tmp1B3;struct Cyc_List_List*_tmp1B4;struct Cyc_List_List*
_tmp1B5;struct Cyc_Absyn_AggrInfo _tmp1B6;void*_tmp1B7;struct Cyc_Absyn_Enumfield*
_tmp1B8;struct Cyc_Absyn_Enumfield*_tmp1B9;struct Cyc_List_List*_tmp1BA;struct Cyc_Absyn_Tunionfield*
_tmp1BB;struct Cyc_List_List*_tmp1BC;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_Tunionfield*
_tmp1BE;_LL31B: if((int)_tmp1A7 == 0){goto _LL31C;}else{goto _LL31D;}_LL31D: if((int)
_tmp1A7 == 1){goto _LL31E;}else{goto _LL31F;}_LL31F: if((unsigned int)_tmp1A7 > 2?*((
int*)_tmp1A7)== 1: 0){_LL33C: _tmp1A9=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp1A7)->f1;goto _LL33B;_LL33B: _tmp1A8=((struct Cyc_Absyn_Int_p_struct*)_tmp1A7)->f2;
goto _LL320;}else{goto _LL321;}_LL321: if((unsigned int)_tmp1A7 > 2?*((int*)_tmp1A7)
== 2: 0){_LL33D: _tmp1AA=((struct Cyc_Absyn_Char_p_struct*)_tmp1A7)->f1;goto _LL322;}
else{goto _LL323;}_LL323: if((unsigned int)_tmp1A7 > 2?*((int*)_tmp1A7)== 3: 0){
_LL33E: _tmp1AB=((struct Cyc_Absyn_Float_p_struct*)_tmp1A7)->f1;goto _LL324;}else{
goto _LL325;}_LL325: if((unsigned int)_tmp1A7 > 2?*((int*)_tmp1A7)== 0: 0){_LL33F:
_tmp1AC=((struct Cyc_Absyn_Var_p_struct*)_tmp1A7)->f1;goto _LL326;}else{goto _LL327;}
_LL327: if((unsigned int)_tmp1A7 > 2?*((int*)_tmp1A7)== 4: 0){_LL340: _tmp1AD=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp1A7)->f1;goto _LL328;}else{goto _LL329;}_LL329:
if((unsigned int)_tmp1A7 > 2?*((int*)_tmp1A7)== 5: 0){_LL341: _tmp1AE=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp1A7)->f1;goto _LL32A;}else{goto _LL32B;}_LL32B: if((unsigned int)_tmp1A7 > 2?*((
int*)_tmp1A7)== 6: 0){_LL342: _tmp1AF=((struct Cyc_Absyn_Reference_p_struct*)
_tmp1A7)->f1;goto _LL32C;}else{goto _LL32D;}_LL32D: if((unsigned int)_tmp1A7 > 2?*((
int*)_tmp1A7)== 11: 0){_LL343: _tmp1B0=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp1A7)->f1;goto _LL32E;}else{goto _LL32F;}_LL32F: if((unsigned int)_tmp1A7 > 2?*((
int*)_tmp1A7)== 12: 0){_LL346: _tmp1B3=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp1A7)->f1;goto _LL345;_LL345: _tmp1B2=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp1A7)->f2;goto _LL344;_LL344: _tmp1B1=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp1A7)->f3;goto _LL330;}else{goto _LL331;}_LL331: if((unsigned int)_tmp1A7 > 2?*((
int*)_tmp1A7)== 7: 0){_LL349: _tmp1B6=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1A7)->f1;
_LL34A: _tmp1B7=(void*)_tmp1B6.aggr_info;goto _LL348;_LL348: _tmp1B5=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp1A7)->f2;goto _LL347;_LL347: _tmp1B4=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1A7)->f3;
goto _LL332;}else{goto _LL333;}_LL333: if((unsigned int)_tmp1A7 > 2?*((int*)_tmp1A7)
== 9: 0){_LL34B: _tmp1B8=((struct Cyc_Absyn_Enum_p_struct*)_tmp1A7)->f2;goto _LL334;}
else{goto _LL335;}_LL335: if((unsigned int)_tmp1A7 > 2?*((int*)_tmp1A7)== 10: 0){
_LL34C: _tmp1B9=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp1A7)->f2;goto _LL336;}
else{goto _LL337;}_LL337: if((unsigned int)_tmp1A7 > 2?*((int*)_tmp1A7)== 8: 0){
_LL34E: _tmp1BB=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1A7)->f2;goto _LL34D;_LL34D:
_tmp1BA=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1A7)->f4;if(_tmp1BA == 0){goto
_LL338;}else{goto _LL339;}}else{goto _LL339;}_LL339: if((unsigned int)_tmp1A7 > 2?*((
int*)_tmp1A7)== 8: 0){_LL351: _tmp1BE=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1A7)->f2;
goto _LL350;_LL350: _tmp1BD=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1A7)->f3;goto
_LL34F;_LL34F: _tmp1BC=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1A7)->f4;goto _LL33A;}
else{goto _LL31A;}_LL31C: s=Cyc_PP_text(_tag_arr("_",sizeof(unsigned char),2));
goto _LL31A;_LL31E: s=Cyc_PP_text(_tag_arr("NULL",sizeof(unsigned char),5));goto
_LL31A;_LL320: if(_tmp1A9 == (void*)0){s=Cyc_PP_text((struct _tagged_arr)({struct
Cyc_Std_Int_pa_struct _tmp1C0;_tmp1C0.tag=1;_tmp1C0.f1=(int)((unsigned int)
_tmp1A8);{void*_tmp1BF[1]={& _tmp1C0};Cyc_Std_aprintf(_tag_arr("%d",sizeof(
unsigned char),3),_tag_arr(_tmp1BF,sizeof(void*),1));}}));}else{s=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp1C2;_tmp1C2.tag=1;_tmp1C2.f1=(
unsigned int)_tmp1A8;{void*_tmp1C1[1]={& _tmp1C2};Cyc_Std_aprintf(_tag_arr("%u",
sizeof(unsigned char),3),_tag_arr(_tmp1C1,sizeof(void*),1));}}));}goto _LL31A;
_LL322: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp1C4;
_tmp1C4.tag=0;_tmp1C4.f1=(struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp1AA);{
void*_tmp1C3[1]={& _tmp1C4};Cyc_Std_aprintf(_tag_arr("'%s'",sizeof(unsigned char),
5),_tag_arr(_tmp1C3,sizeof(void*),1));}}));goto _LL31A;_LL324: s=Cyc_PP_text(
_tmp1AB);goto _LL31A;_LL326: s=Cyc_Absynpp_qvar2doc(_tmp1AC->name);goto _LL31A;
_LL328: s=({struct Cyc_PP_Doc*_tmp1C5[3];_tmp1C5[2]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp1C5[1]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Pat*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_pat2doc,_tag_arr(",",sizeof(unsigned char),2),_tmp1AD);_tmp1C5[0]=
Cyc_PP_text(_tag_arr("$(",sizeof(unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp1C5,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL31A;_LL32A: s=({struct Cyc_PP_Doc*_tmp1C6[
2];_tmp1C6[1]=Cyc_Absynpp_pat2doc(_tmp1AE);_tmp1C6[0]=Cyc_PP_text(_tag_arr("&",
sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp1C6,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL31A;_LL32C: s=({struct Cyc_PP_Doc*_tmp1C7[2];_tmp1C7[1]=Cyc_Absynpp_qvar2doc(
_tmp1AF->name);_tmp1C7[0]=Cyc_PP_text(_tag_arr("*",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp1C7,sizeof(struct Cyc_PP_Doc*),2));});goto _LL31A;_LL32E: s=Cyc_Absynpp_qvar2doc(
_tmp1B0);goto _LL31A;_LL330: s=({struct Cyc_PP_Doc*_tmp1C8[3];_tmp1C8[2]=Cyc_PP_group(
_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp1B1));_tmp1C8[1]=Cyc_Absynpp_tvars2doc(_tmp1B2);_tmp1C8[0]=Cyc_Absynpp_qvar2doc(
_tmp1B3);Cyc_PP_cat(_tag_arr(_tmp1C8,sizeof(struct Cyc_PP_Doc*),3));});goto _LL31A;
_LL332: {struct _tuple0*_tmp1CA;struct _tuple3 _tmp1C9=Cyc_Absyn_aggr_kinded_name(
_tmp1B7);_LL353: _tmp1CA=_tmp1C9.f2;goto _LL352;_LL352: s=({struct Cyc_PP_Doc*
_tmp1CB[4];_tmp1CB[3]=Cyc_PP_group(_tag_arr("",sizeof(unsigned char),1),_tag_arr("}",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp1B4));_tmp1CB[2]=Cyc_PP_egroup(_tag_arr("[",sizeof(unsigned char),2),
_tag_arr("]",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
_tmp1B5)));_tmp1CB[1]=Cyc_PP_text(_tag_arr("{",sizeof(unsigned char),2));_tmp1CB[
0]=Cyc_Absynpp_qvar2doc(_tmp1CA);Cyc_PP_cat(_tag_arr(_tmp1CB,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL31A;}_LL334: _tmp1B9=_tmp1B8;goto _LL336;_LL336: s=Cyc_Absynpp_qvar2doc(
_tmp1B9->name);goto _LL31A;_LL338: s=Cyc_Absynpp_qvar2doc(_tmp1BB->name);goto
_LL31A;_LL33A: s=({struct Cyc_PP_Doc*_tmp1CC[3];_tmp1CC[2]=Cyc_PP_egroup(_tag_arr("(",
sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp1BC));
_tmp1CC[1]=Cyc_Absynpp_tvars2doc(_tmp1BD);_tmp1CC[0]=Cyc_Absynpp_qvar2doc(
_tmp1BE->name);Cyc_PP_cat(_tag_arr(_tmp1CC,sizeof(struct Cyc_PP_Doc*),3));});goto
_LL31A;_LL31A:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple6*dp){
return({struct Cyc_PP_Doc*_tmp1CD[2];_tmp1CD[1]=Cyc_Absynpp_pat2doc((*dp).f2);
_tmp1CD[0]=Cyc_PP_egroup(_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",
sizeof(unsigned char),2),_tag_arr("=",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*
dp).f1));Cyc_PP_cat(_tag_arr(_tmp1CD,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*
Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){if(c->where_clause
== 0?(void*)(c->pattern)->r == (void*)0: 0){return({struct Cyc_PP_Doc*_tmp1CE[2];
_tmp1CE[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1CF[2];_tmp1CF[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp1CF[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1CF,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp1CE[0]=Cyc_PP_text(_tag_arr("default: ",sizeof(
unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp1CE,sizeof(struct Cyc_PP_Doc*),2));});}
else{if(c->where_clause == 0){return({struct Cyc_PP_Doc*_tmp1D0[4];_tmp1D0[3]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp1D1[2];_tmp1D1[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1D1[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1D1,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp1D0[2]=Cyc_PP_text(_tag_arr(": ",sizeof(unsigned char),3));_tmp1D0[1]=Cyc_Absynpp_pat2doc(
c->pattern);_tmp1D0[0]=Cyc_PP_text(_tag_arr("case ",sizeof(unsigned char),6));
Cyc_PP_cat(_tag_arr(_tmp1D0,sizeof(struct Cyc_PP_Doc*),4));});}else{return({
struct Cyc_PP_Doc*_tmp1D2[6];_tmp1D2[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1D3[
2];_tmp1D3[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1D3[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp1D3,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1D2[4]=Cyc_PP_text(
_tag_arr(": ",sizeof(unsigned char),3));_tmp1D2[3]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp1D2[2]=Cyc_PP_text(_tag_arr(" && ",
sizeof(unsigned char),5));_tmp1D2[1]=Cyc_Absynpp_pat2doc(c->pattern);_tmp1D2[0]=
Cyc_PP_text(_tag_arr("case ",sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(
_tmp1D2,sizeof(struct Cyc_PP_Doc*),6));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Switch_clause*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_switchclause2doc,_tag_arr("",sizeof(unsigned char),1),cs);}struct Cyc_PP_Doc*
Cyc_Absynpp_switchCclause2doc(struct Cyc_Absyn_SwitchC_clause*c){struct Cyc_Absyn_SwitchC_clause
_tmp1D5;struct Cyc_Absyn_Stmt*_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D7;struct Cyc_Absyn_SwitchC_clause*
_tmp1D4=c;_tmp1D5=*_tmp1D4;_LL356: _tmp1D7=_tmp1D5.cnst_exp;goto _LL355;_LL355:
_tmp1D6=_tmp1D5.body;goto _LL354;_LL354: if(_tmp1D7 == 0){return({struct Cyc_PP_Doc*
_tmp1D8[2];_tmp1D8[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1D9[2];_tmp1D9[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp1D9[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1D9,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp1D8[0]=Cyc_PP_text(_tag_arr("default: ",sizeof(
unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp1D8,sizeof(struct Cyc_PP_Doc*),2));});}
else{return({struct Cyc_PP_Doc*_tmp1DA[4];_tmp1DA[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp1DB[2];_tmp1DB[1]=Cyc_Absynpp_stmt2doc(_tmp1D6);_tmp1DB[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp1DB,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1DA[2]=Cyc_PP_text(
_tag_arr(": ",sizeof(unsigned char),3));_tmp1DA[1]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(_tmp1D7));_tmp1DA[0]=Cyc_PP_text(_tag_arr("case ",
sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp1DA,sizeof(struct Cyc_PP_Doc*),
4));});}}struct Cyc_PP_Doc*Cyc_Absynpp_switchCclauses2doc(struct Cyc_List_List*cs){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_SwitchC_clause*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchCclause2doc,
_tag_arr("",sizeof(unsigned char),1),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f){if(f->tag == 0){return Cyc_Absynpp_qvar2doc(f->name);}
else{return({struct Cyc_PP_Doc*_tmp1DC[3];_tmp1DC[2]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(f->tag));_tmp1DC[1]=Cyc_PP_text(_tag_arr(" = ",
sizeof(unsigned char),4));_tmp1DC[0]=Cyc_Absynpp_qvar2doc(f->name);Cyc_PP_cat(
_tag_arr(_tmp1DC,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Enumfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_enumfield2doc,_tag_arr(",",sizeof(unsigned char),2),fs);}static
struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(
v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,
_tag_arr(",",sizeof(unsigned char),2),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(
struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl _tmp1DE;struct Cyc_List_List*
_tmp1DF;struct Cyc_Absyn_Exp*_tmp1E0;void*_tmp1E1;struct Cyc_Absyn_Tqual _tmp1E2;
struct _tuple0*_tmp1E3;void*_tmp1E4;struct Cyc_Absyn_Vardecl*_tmp1DD=vd;_tmp1DE=*
_tmp1DD;_LL35D: _tmp1E4=(void*)_tmp1DE.sc;goto _LL35C;_LL35C: _tmp1E3=_tmp1DE.name;
goto _LL35B;_LL35B: _tmp1E2=_tmp1DE.tq;goto _LL35A;_LL35A: _tmp1E1=(void*)_tmp1DE.type;
goto _LL359;_LL359: _tmp1E0=_tmp1DE.initializer;goto _LL358;_LL358: _tmp1DF=_tmp1DE.attributes;
goto _LL357;_LL357: {struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2doc(
_tmp1E3);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp1DF);struct Cyc_PP_Doc*
beforenamedoc;if(! Cyc_Absynpp_to_VC){beforenamedoc=attsdoc;}else{void*_tmp1E5=
Cyc_Tcutil_compress(_tmp1E1);struct Cyc_Absyn_FnInfo _tmp1E6;struct Cyc_List_List*
_tmp1E7;_LL35F: if((unsigned int)_tmp1E5 > 3?*((int*)_tmp1E5)== 8: 0){_LL363:
_tmp1E6=((struct Cyc_Absyn_FnType_struct*)_tmp1E5)->f1;_LL364: _tmp1E7=_tmp1E6.attributes;
goto _LL360;}else{goto _LL361;}_LL361: goto _LL362;_LL360: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp1E7);goto _LL35E;_LL362: beforenamedoc=Cyc_PP_nil_doc();goto _LL35E;_LL35E:;}s=({
struct Cyc_PP_Doc*_tmp1E8[5];_tmp1E8[4]=Cyc_PP_text(_tag_arr(";",sizeof(
unsigned char),2));_tmp1E8[3]=_tmp1E0 == 0? Cyc_PP_text(_tag_arr("",sizeof(
unsigned char),1)):({struct Cyc_PP_Doc*_tmp1EB[2];_tmp1EB[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(_tmp1E0));_tmp1EB[0]=Cyc_PP_text(_tag_arr(" = ",
sizeof(unsigned char),4));Cyc_PP_cat(_tag_arr(_tmp1EB,sizeof(struct Cyc_PP_Doc*),
2));});_tmp1E8[2]=Cyc_Absynpp_tqtd2doc(_tmp1E2,_tmp1E1,({struct Cyc_Core_Opt*
_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->v=({struct Cyc_PP_Doc*_tmp1EA[2];
_tmp1EA[1]=sn;_tmp1EA[0]=beforenamedoc;Cyc_PP_cat(_tag_arr(_tmp1EA,sizeof(struct
Cyc_PP_Doc*),2));});_tmp1E9;}));_tmp1E8[1]=Cyc_Absynpp_scope2doc(_tmp1E4);
_tmp1E8[0]=Cyc_Absynpp_to_VC? attsdoc: Cyc_PP_nil_doc();Cyc_PP_cat(_tag_arr(
_tmp1E8,sizeof(struct Cyc_PP_Doc*),5));});return s;}}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp1EC=(void*)d->r;struct Cyc_Absyn_Fndecl*
_tmp1ED;struct Cyc_Absyn_Aggrdecl*_tmp1EE;struct Cyc_Absyn_Vardecl*_tmp1EF;struct
Cyc_Absyn_Tuniondecl*_tmp1F0;struct Cyc_Absyn_Tuniondecl _tmp1F1;int _tmp1F2;struct
Cyc_Core_Opt*_tmp1F3;struct Cyc_List_List*_tmp1F4;struct _tuple0*_tmp1F5;void*
_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F7;struct Cyc_Absyn_Pat*_tmp1F8;struct Cyc_List_List*
_tmp1F9;struct Cyc_Absyn_Enumdecl*_tmp1FA;struct Cyc_Absyn_Enumdecl _tmp1FB;struct
Cyc_Core_Opt*_tmp1FC;struct _tuple0*_tmp1FD;void*_tmp1FE;struct Cyc_Absyn_Typedefdecl*
_tmp1FF;struct Cyc_List_List*_tmp200;struct _tagged_arr*_tmp201;struct Cyc_List_List*
_tmp202;struct _tuple0*_tmp203;struct Cyc_List_List*_tmp204;_LL366: if(*((int*)
_tmp1EC)== 1){_LL37C: _tmp1ED=((struct Cyc_Absyn_Fn_d_struct*)_tmp1EC)->f1;goto
_LL367;}else{goto _LL368;}_LL368: if(*((int*)_tmp1EC)== 4){_LL37D: _tmp1EE=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp1EC)->f1;goto _LL369;}else{goto _LL36A;}_LL36A: if(*((
int*)_tmp1EC)== 0){_LL37E: _tmp1EF=((struct Cyc_Absyn_Var_d_struct*)_tmp1EC)->f1;
goto _LL36B;}else{goto _LL36C;}_LL36C: if(*((int*)_tmp1EC)== 5){_LL37F: _tmp1F0=((
struct Cyc_Absyn_Tunion_d_struct*)_tmp1EC)->f1;_tmp1F1=*_tmp1F0;_LL384: _tmp1F6=(
void*)_tmp1F1.sc;goto _LL383;_LL383: _tmp1F5=_tmp1F1.name;goto _LL382;_LL382:
_tmp1F4=_tmp1F1.tvs;goto _LL381;_LL381: _tmp1F3=_tmp1F1.fields;goto _LL380;_LL380:
_tmp1F2=_tmp1F1.is_xtunion;goto _LL36D;}else{goto _LL36E;}_LL36E: if(*((int*)
_tmp1EC)== 2){_LL386: _tmp1F8=((struct Cyc_Absyn_Let_d_struct*)_tmp1EC)->f1;goto
_LL385;_LL385: _tmp1F7=((struct Cyc_Absyn_Let_d_struct*)_tmp1EC)->f4;goto _LL36F;}
else{goto _LL370;}_LL370: if(*((int*)_tmp1EC)== 3){_LL387: _tmp1F9=((struct Cyc_Absyn_Letv_d_struct*)
_tmp1EC)->f1;goto _LL371;}else{goto _LL372;}_LL372: if(*((int*)_tmp1EC)== 6){_LL388:
_tmp1FA=((struct Cyc_Absyn_Enum_d_struct*)_tmp1EC)->f1;_tmp1FB=*_tmp1FA;_LL38B:
_tmp1FE=(void*)_tmp1FB.sc;goto _LL38A;_LL38A: _tmp1FD=_tmp1FB.name;goto _LL389;
_LL389: _tmp1FC=_tmp1FB.fields;goto _LL373;}else{goto _LL374;}_LL374: if(*((int*)
_tmp1EC)== 7){_LL38C: _tmp1FF=((struct Cyc_Absyn_Typedef_d_struct*)_tmp1EC)->f1;
goto _LL375;}else{goto _LL376;}_LL376: if(*((int*)_tmp1EC)== 8){_LL38E: _tmp201=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp1EC)->f1;goto _LL38D;_LL38D: _tmp200=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp1EC)->f2;goto _LL377;}else{goto _LL378;}
_LL378: if(*((int*)_tmp1EC)== 9){_LL390: _tmp203=((struct Cyc_Absyn_Using_d_struct*)
_tmp1EC)->f1;goto _LL38F;_LL38F: _tmp202=((struct Cyc_Absyn_Using_d_struct*)_tmp1EC)->f2;
goto _LL379;}else{goto _LL37A;}_LL37A: if(*((int*)_tmp1EC)== 10){_LL391: _tmp204=((
struct Cyc_Absyn_ExternC_d_struct*)_tmp1EC)->f1;goto _LL37B;}else{goto _LL365;}
_LL367: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp20C=_cycalloc(sizeof(*
_tmp20C));_tmp20C[0]=({struct Cyc_Absyn_FnType_struct _tmp20D;_tmp20D.tag=8;
_tmp20D.f1=({struct Cyc_Absyn_FnInfo _tmp20E;_tmp20E.tvars=_tmp1ED->tvs;_tmp20E.effect=
_tmp1ED->effect;_tmp20E.ret_typ=(void*)((void*)_tmp1ED->ret_type);_tmp20E.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple4*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp1ED->args);_tmp20E.c_varargs=_tmp1ED->c_varargs;
_tmp20E.cyc_varargs=_tmp1ED->cyc_varargs;_tmp20E.rgn_po=_tmp1ED->rgn_po;_tmp20E.attributes=
0;_tmp20E;});_tmp20D;});_tmp20C;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp1ED->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp1ED->is_inline){if(Cyc_Absynpp_to_VC){
inlinedoc=Cyc_PP_text(_tag_arr("__inline ",sizeof(unsigned char),10));}else{
inlinedoc=Cyc_PP_text(_tag_arr("inline ",sizeof(unsigned char),8));}}else{
inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((
void*)_tmp1ED->sc);struct Cyc_PP_Doc*beforenamedoc=Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_tmp1ED->attributes): attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp1ED->name);struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual
_tmp209;_tmp209.q_const=0;_tmp209.q_volatile=0;_tmp209.q_restrict=0;_tmp209;}),t,({
struct Cyc_Core_Opt*_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A->v=({struct Cyc_PP_Doc*
_tmp20B[2];_tmp20B[1]=namedoc;_tmp20B[0]=beforenamedoc;Cyc_PP_cat(_tag_arr(
_tmp20B,sizeof(struct Cyc_PP_Doc*),2));});_tmp20A;}));struct Cyc_PP_Doc*bodydoc=({
struct Cyc_PP_Doc*_tmp207[4];_tmp207[3]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp207[2]=Cyc_PP_line_doc();_tmp207[1]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp208[2];_tmp208[1]=Cyc_Absynpp_stmt2doc(_tmp1ED->body);
_tmp208[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp208,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp207[0]=Cyc_PP_text(_tag_arr(" {",sizeof(unsigned char),3));Cyc_PP_cat(
_tag_arr(_tmp207,sizeof(struct Cyc_PP_Doc*),4));});s=({struct Cyc_PP_Doc*_tmp205[4];
_tmp205[3]=bodydoc;_tmp205[2]=tqtddoc;_tmp205[1]=scopedoc;_tmp205[0]=inlinedoc;
Cyc_PP_cat(_tag_arr(_tmp205,sizeof(struct Cyc_PP_Doc*),4));});if(Cyc_Absynpp_to_VC){
s=({struct Cyc_PP_Doc*_tmp206[2];_tmp206[1]=s;_tmp206[0]=attsdoc;Cyc_PP_cat(
_tag_arr(_tmp206,sizeof(struct Cyc_PP_Doc*),2));});}goto _LL365;}}_LL369: if(
_tmp1EE->fields == 0){s=({struct Cyc_PP_Doc*_tmp20F[5];_tmp20F[4]=Cyc_PP_text(
_tag_arr(";",sizeof(unsigned char),2));_tmp20F[3]=Cyc_Absynpp_ktvars2doc(_tmp1EE->tvs);
_tmp20F[2]=Cyc_Absynpp_qvar2doc(_tmp1EE->name);_tmp20F[1]=Cyc_Absynpp_aggr_kind2doc((
void*)_tmp1EE->kind);_tmp20F[0]=Cyc_Absynpp_scope2doc((void*)_tmp1EE->sc);Cyc_PP_cat(
_tag_arr(_tmp20F,sizeof(struct Cyc_PP_Doc*),5));});}else{s=({struct Cyc_PP_Doc*
_tmp210[10];_tmp210[9]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));
_tmp210[8]=Cyc_Absynpp_atts2doc(_tmp1EE->attributes);_tmp210[7]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp210[6]=Cyc_PP_line_doc();_tmp210[5]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp211[2];_tmp211[1]=Cyc_Absynpp_aggrfields2doc((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1EE->fields))->v);
_tmp211[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp211,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp210[4]=Cyc_PP_text(_tag_arr(" {",sizeof(unsigned char),3));_tmp210[3]=
Cyc_Absynpp_ktvars2doc(_tmp1EE->tvs);_tmp210[2]=Cyc_Absynpp_qvar2doc(_tmp1EE->name);
_tmp210[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp1EE->kind);_tmp210[0]=Cyc_Absynpp_scope2doc((
void*)_tmp1EE->sc);Cyc_PP_cat(_tag_arr(_tmp210,sizeof(struct Cyc_PP_Doc*),10));});}
goto _LL365;_LL36B: s=Cyc_Absynpp_vardecl2doc(_tmp1EF);goto _LL365;_LL36D: if(
_tmp1F3 == 0){s=({struct Cyc_PP_Doc*_tmp212[5];_tmp212[4]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp212[3]=Cyc_Absynpp_ktvars2doc(_tmp1F4);_tmp212[2]=
_tmp1F2? Cyc_Absynpp_qvar2doc(_tmp1F5): Cyc_Absynpp_typedef_name2doc(_tmp1F5);
_tmp212[1]=_tmp1F2? Cyc_PP_text(_tag_arr("xtunion ",sizeof(unsigned char),9)): Cyc_PP_text(
_tag_arr("tunion ",sizeof(unsigned char),8));_tmp212[0]=Cyc_Absynpp_scope2doc(
_tmp1F6);Cyc_PP_cat(_tag_arr(_tmp212,sizeof(struct Cyc_PP_Doc*),5));});}else{s=({
struct Cyc_PP_Doc*_tmp213[8];_tmp213[7]=Cyc_PP_text(_tag_arr("};",sizeof(
unsigned char),3));_tmp213[6]=Cyc_PP_line_doc();_tmp213[5]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp214[2];_tmp214[1]=Cyc_Absynpp_tunionfields2doc((struct Cyc_List_List*)
_tmp1F3->v);_tmp214[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp214,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp213[4]=Cyc_PP_text(_tag_arr(" {",sizeof(
unsigned char),3));_tmp213[3]=Cyc_Absynpp_ktvars2doc(_tmp1F4);_tmp213[2]=_tmp1F2?
Cyc_Absynpp_qvar2doc(_tmp1F5): Cyc_Absynpp_typedef_name2doc(_tmp1F5);_tmp213[1]=
_tmp1F2? Cyc_PP_text(_tag_arr("xtunion ",sizeof(unsigned char),9)): Cyc_PP_text(
_tag_arr("tunion ",sizeof(unsigned char),8));_tmp213[0]=Cyc_Absynpp_scope2doc(
_tmp1F6);Cyc_PP_cat(_tag_arr(_tmp213,sizeof(struct Cyc_PP_Doc*),8));});}goto
_LL365;_LL36F: s=({struct Cyc_PP_Doc*_tmp215[5];_tmp215[4]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp215[3]=Cyc_Absynpp_exp2doc(_tmp1F7);_tmp215[2]=Cyc_PP_text(
_tag_arr(" = ",sizeof(unsigned char),4));_tmp215[1]=Cyc_Absynpp_pat2doc(_tmp1F8);
_tmp215[0]=Cyc_PP_text(_tag_arr("let ",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp215,sizeof(struct Cyc_PP_Doc*),5));});goto _LL365;_LL371: s=({struct
Cyc_PP_Doc*_tmp216[3];_tmp216[2]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),
2));_tmp216[1]=Cyc_Absynpp_ids2doc(_tmp1F9);_tmp216[0]=Cyc_PP_text(_tag_arr("let ",
sizeof(unsigned char),5));Cyc_PP_cat(_tag_arr(_tmp216,sizeof(struct Cyc_PP_Doc*),
3));});goto _LL365;_LL373: if(_tmp1FC == 0){s=({struct Cyc_PP_Doc*_tmp217[4];_tmp217[
3]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmp217[2]=Cyc_Absynpp_typedef_name2doc(
_tmp1FD);_tmp217[1]=Cyc_PP_text(_tag_arr("enum ",sizeof(unsigned char),6));
_tmp217[0]=Cyc_Absynpp_scope2doc(_tmp1FE);Cyc_PP_cat(_tag_arr(_tmp217,sizeof(
struct Cyc_PP_Doc*),4));});}else{s=({struct Cyc_PP_Doc*_tmp218[7];_tmp218[6]=Cyc_PP_text(
_tag_arr("};",sizeof(unsigned char),3));_tmp218[5]=Cyc_PP_line_doc();_tmp218[4]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp219[2];_tmp219[1]=Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*)_tmp1FC->v);_tmp219[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp219,sizeof(struct Cyc_PP_Doc*),2));}));_tmp218[3]=Cyc_PP_text(
_tag_arr(" {",sizeof(unsigned char),3));_tmp218[2]=Cyc_Absynpp_qvar2doc(_tmp1FD);
_tmp218[1]=Cyc_PP_text(_tag_arr("enum ",sizeof(unsigned char),6));_tmp218[0]=Cyc_Absynpp_scope2doc(
_tmp1FE);Cyc_PP_cat(_tag_arr(_tmp218,sizeof(struct Cyc_PP_Doc*),7));});}goto
_LL365;_LL375: s=({struct Cyc_PP_Doc*_tmp21A[3];_tmp21A[2]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp21A[1]=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual
_tmp21B;_tmp21B.q_const=0;_tmp21B.q_volatile=0;_tmp21B.q_restrict=0;_tmp21B;}),(
void*)_tmp1FF->defn,({struct Cyc_Core_Opt*_tmp21C=_cycalloc(sizeof(*_tmp21C));
_tmp21C->v=({struct Cyc_PP_Doc*_tmp21D[2];_tmp21D[1]=Cyc_Absynpp_tvars2doc(
_tmp1FF->tvs);_tmp21D[0]=Cyc_Absynpp_typedef_name2doc(_tmp1FF->name);Cyc_PP_cat(
_tag_arr(_tmp21D,sizeof(struct Cyc_PP_Doc*),2));});_tmp21C;}));_tmp21A[0]=Cyc_PP_text(
_tag_arr("typedef ",sizeof(unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp21A,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL365;_LL377: if(Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_add(_tmp201);}s=({struct Cyc_PP_Doc*_tmp21E[7];_tmp21E[
6]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp21E[5]=Cyc_PP_line_doc();
_tmp21E[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(unsigned char),1),_tmp200);_tmp21E[3]=Cyc_PP_line_doc();
_tmp21E[2]=Cyc_PP_text(_tag_arr(" {",sizeof(unsigned char),3));_tmp21E[1]=Cyc_PP_textptr(
_tmp201);_tmp21E[0]=Cyc_PP_text(_tag_arr("namespace ",sizeof(unsigned char),11));
Cyc_PP_cat(_tag_arr(_tmp21E,sizeof(struct Cyc_PP_Doc*),7));});if(Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_drop();}goto _LL365;_LL379: if(Cyc_Absynpp_print_using_stmts){
s=({struct Cyc_PP_Doc*_tmp21F[7];_tmp21F[6]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp21F[5]=Cyc_PP_line_doc();_tmp21F[4]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),
_tmp202);_tmp21F[3]=Cyc_PP_line_doc();_tmp21F[2]=Cyc_PP_text(_tag_arr(" {",
sizeof(unsigned char),3));_tmp21F[1]=Cyc_Absynpp_qvar2doc(_tmp203);_tmp21F[0]=
Cyc_PP_text(_tag_arr("using ",sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(
_tmp21F,sizeof(struct Cyc_PP_Doc*),7));});}else{s=({struct Cyc_PP_Doc*_tmp220[7];
_tmp220[6]=Cyc_PP_text(_tag_arr("/* } */",sizeof(unsigned char),8));_tmp220[5]=
Cyc_PP_line_doc();_tmp220[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),_tmp202);_tmp220[3]=
Cyc_PP_line_doc();_tmp220[2]=Cyc_PP_text(_tag_arr(" { */",sizeof(unsigned char),
6));_tmp220[1]=Cyc_Absynpp_qvar2doc(_tmp203);_tmp220[0]=Cyc_PP_text(_tag_arr("/* using ",
sizeof(unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp220,sizeof(struct Cyc_PP_Doc*),
7));});}goto _LL365;_LL37B: if(Cyc_Absynpp_print_externC_stmts){s=({struct Cyc_PP_Doc*
_tmp221[5];_tmp221[4]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp221[
3]=Cyc_PP_line_doc();_tmp221[2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),_tmp204);_tmp221[1]=
Cyc_PP_line_doc();_tmp221[0]=Cyc_PP_text(_tag_arr("extern \"C\" {",sizeof(
unsigned char),13));Cyc_PP_cat(_tag_arr(_tmp221,sizeof(struct Cyc_PP_Doc*),5));});}
else{s=({struct Cyc_PP_Doc*_tmp222[5];_tmp222[4]=Cyc_PP_text(_tag_arr("/* } */",
sizeof(unsigned char),8));_tmp222[3]=Cyc_PP_line_doc();_tmp222[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),
_tmp204);_tmp222[1]=Cyc_PP_line_doc();_tmp222[0]=Cyc_PP_text(_tag_arr("/* extern \"C\" { */",
sizeof(unsigned char),19));Cyc_PP_cat(_tag_arr(_tmp222,sizeof(struct Cyc_PP_Doc*),
5));});}goto _LL365;_LL365:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*
sc){void*_tmp223=sc;_LL393: if((int)_tmp223 == 0){goto _LL394;}else{goto _LL395;}
_LL395: if((int)_tmp223 == 2){goto _LL396;}else{goto _LL397;}_LL397: if((int)_tmp223
== 3){goto _LL398;}else{goto _LL399;}_LL399: if((int)_tmp223 == 4){goto _LL39A;}else{
goto _LL39B;}_LL39B: if((int)_tmp223 == 1){goto _LL39C;}else{goto _LL392;}_LL394:
return Cyc_PP_text(_tag_arr("static ",sizeof(unsigned char),8));_LL396: return Cyc_PP_nil_doc();
_LL398: return Cyc_PP_text(_tag_arr("extern ",sizeof(unsigned char),8));_LL39A:
return Cyc_PP_text(_tag_arr("extern \"C\" ",sizeof(unsigned char),12));_LL39C:
return Cyc_PP_text(_tag_arr("abstract ",sizeof(unsigned char),10));_LL392:;}int
Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp224=t;struct Cyc_Absyn_Tvar*
_tmp225;struct Cyc_List_List*_tmp226;_LL39E: if((unsigned int)_tmp224 > 3?*((int*)
_tmp224)== 1: 0){_LL3A4: _tmp225=((struct Cyc_Absyn_VarType_struct*)_tmp224)->f1;
goto _LL39F;}else{goto _LL3A0;}_LL3A0: if((unsigned int)_tmp224 > 3?*((int*)_tmp224)
== 18: 0){_LL3A5: _tmp226=((struct Cyc_Absyn_JoinEff_struct*)_tmp224)->f1;goto
_LL3A1;}else{goto _LL3A2;}_LL3A2: goto _LL3A3;_LL39F: return Cyc_Tcutil_is_temp_tvar(
_tmp225);_LL3A1: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp226);_LL3A3: return 0;_LL39D:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){void*
_tmp227=t;struct Cyc_Core_Opt*_tmp228;struct Cyc_Core_Opt _tmp229;void*_tmp22A;
_LL3A7: if((unsigned int)_tmp227 > 3?*((int*)_tmp227)== 11: 0){goto _LL3A8;}else{
goto _LL3A9;}_LL3A9: if((unsigned int)_tmp227 > 3?*((int*)_tmp227)== 16: 0){_LL3AD:
_tmp228=((struct Cyc_Absyn_TypedefType_struct*)_tmp227)->f3;if(_tmp228 == 0){goto
_LL3AB;}else{_tmp229=*_tmp228;_LL3AE: _tmp22A=(void*)_tmp229.v;goto _LL3AA;}}else{
goto _LL3AB;}_LL3AB: goto _LL3AC;_LL3A8: return 1;_LL3AA: return Cyc_Absynpp_is_anon_aggrtype(
_tmp22A);_LL3AC: return 0;_LL3A6:;}struct _tuple5 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual
tq,void*t){void*_tmp22B=t;struct Cyc_Absyn_Exp*_tmp22C;struct Cyc_Absyn_Tqual
_tmp22D;void*_tmp22E;struct Cyc_Absyn_PtrInfo _tmp22F;struct Cyc_Absyn_Conref*
_tmp230;struct Cyc_Absyn_Tqual _tmp231;struct Cyc_Absyn_Conref*_tmp232;void*_tmp233;
void*_tmp234;struct Cyc_Absyn_FnInfo _tmp235;struct Cyc_List_List*_tmp236;struct Cyc_List_List*
_tmp237;struct Cyc_Absyn_VarargInfo*_tmp238;int _tmp239;struct Cyc_List_List*
_tmp23A;void*_tmp23B;struct Cyc_Core_Opt*_tmp23C;struct Cyc_List_List*_tmp23D;int
_tmp23E;struct Cyc_Core_Opt*_tmp23F;struct Cyc_Core_Opt*_tmp240;struct Cyc_Core_Opt*
_tmp241;struct Cyc_List_List*_tmp242;struct _tuple0*_tmp243;_LL3B0: if((
unsigned int)_tmp22B > 3?*((int*)_tmp22B)== 7: 0){_LL3BE: _tmp22E=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp22B)->f1;goto _LL3BD;_LL3BD: _tmp22D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp22B)->f2;goto _LL3BC;_LL3BC: _tmp22C=((struct Cyc_Absyn_ArrayType_struct*)
_tmp22B)->f3;goto _LL3B1;}else{goto _LL3B2;}_LL3B2: if((unsigned int)_tmp22B > 3?*((
int*)_tmp22B)== 4: 0){_LL3BF: _tmp22F=((struct Cyc_Absyn_PointerType_struct*)
_tmp22B)->f1;_LL3C4: _tmp234=(void*)_tmp22F.elt_typ;goto _LL3C3;_LL3C3: _tmp233=(
void*)_tmp22F.rgn_typ;goto _LL3C2;_LL3C2: _tmp232=_tmp22F.nullable;goto _LL3C1;
_LL3C1: _tmp231=_tmp22F.tq;goto _LL3C0;_LL3C0: _tmp230=_tmp22F.bounds;goto _LL3B3;}
else{goto _LL3B4;}_LL3B4: if((unsigned int)_tmp22B > 3?*((int*)_tmp22B)== 8: 0){
_LL3C5: _tmp235=((struct Cyc_Absyn_FnType_struct*)_tmp22B)->f1;_LL3CD: _tmp23D=
_tmp235.tvars;goto _LL3CC;_LL3CC: _tmp23C=_tmp235.effect;goto _LL3CB;_LL3CB: _tmp23B=(
void*)_tmp235.ret_typ;goto _LL3CA;_LL3CA: _tmp23A=_tmp235.args;goto _LL3C9;_LL3C9:
_tmp239=_tmp235.c_varargs;goto _LL3C8;_LL3C8: _tmp238=_tmp235.cyc_varargs;goto
_LL3C7;_LL3C7: _tmp237=_tmp235.rgn_po;goto _LL3C6;_LL3C6: _tmp236=_tmp235.attributes;
goto _LL3B5;}else{goto _LL3B6;}_LL3B6: if((unsigned int)_tmp22B > 3?*((int*)_tmp22B)
== 0: 0){_LL3D0: _tmp240=((struct Cyc_Absyn_Evar_struct*)_tmp22B)->f1;goto _LL3CF;
_LL3CF: _tmp23F=((struct Cyc_Absyn_Evar_struct*)_tmp22B)->f2;goto _LL3CE;_LL3CE:
_tmp23E=((struct Cyc_Absyn_Evar_struct*)_tmp22B)->f3;goto _LL3B7;}else{goto _LL3B8;}
_LL3B8: if((unsigned int)_tmp22B > 3?*((int*)_tmp22B)== 16: 0){_LL3D3: _tmp243=((
struct Cyc_Absyn_TypedefType_struct*)_tmp22B)->f1;goto _LL3D2;_LL3D2: _tmp242=((
struct Cyc_Absyn_TypedefType_struct*)_tmp22B)->f2;goto _LL3D1;_LL3D1: _tmp241=((
struct Cyc_Absyn_TypedefType_struct*)_tmp22B)->f3;goto _LL3B9;}else{goto _LL3BA;}
_LL3BA: goto _LL3BB;_LL3B1: {struct Cyc_List_List*_tmp245;void*_tmp246;struct Cyc_Absyn_Tqual
_tmp247;struct _tuple5 _tmp244=Cyc_Absynpp_to_tms(_tmp22D,_tmp22E);_LL3D7: _tmp247=
_tmp244.f1;goto _LL3D6;_LL3D6: _tmp246=_tmp244.f2;goto _LL3D5;_LL3D5: _tmp245=
_tmp244.f3;goto _LL3D4;_LL3D4: {void*tm;if(_tmp22C == 0){tm=(void*)0;}else{tm=(
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp248=_cycalloc(sizeof(*_tmp248));
_tmp248[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp249;_tmp249.tag=0;_tmp249.f1=(
struct Cyc_Absyn_Exp*)_check_null(_tmp22C);_tmp249;});_tmp248;});}return({struct
_tuple5 _tmp24A;_tmp24A.f1=_tmp247;_tmp24A.f2=_tmp246;_tmp24A.f3=({struct Cyc_List_List*
_tmp24B=_cycalloc(sizeof(*_tmp24B));_tmp24B->hd=(void*)tm;_tmp24B->tl=_tmp245;
_tmp24B;});_tmp24A;});}}_LL3B3: {struct Cyc_List_List*_tmp24D;void*_tmp24E;struct
Cyc_Absyn_Tqual _tmp24F;struct _tuple5 _tmp24C=Cyc_Absynpp_to_tms(_tmp231,_tmp234);
_LL3DB: _tmp24F=_tmp24C.f1;goto _LL3DA;_LL3DA: _tmp24E=_tmp24C.f2;goto _LL3D9;_LL3D9:
_tmp24D=_tmp24C.f3;goto _LL3D8;_LL3D8: {void*ps;{struct _tuple9 _tmp251=({struct
_tuple9 _tmp250;_tmp250.f1=(void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
x))Cyc_Absyn_compress_conref)(_tmp232))->v;_tmp250.f2=(void*)(Cyc_Absyn_compress_conref(
_tmp230))->v;_tmp250;});void*_tmp252;void*_tmp253;struct Cyc_Absyn_Exp*_tmp254;
void*_tmp255;int _tmp256;void*_tmp257;void*_tmp258;struct Cyc_Absyn_Exp*_tmp259;
void*_tmp25A;int _tmp25B;void*_tmp25C;void*_tmp25D;_LL3DD: _LL3E8: _tmp255=_tmp251.f1;
if((unsigned int)_tmp255 > 1?*((int*)_tmp255)== 0: 0){_LL3E9: _tmp256=(int)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp255)->f1;if(_tmp256 == 1){goto _LL3E5;}else{goto
_LL3DF;}}else{goto _LL3DF;}_LL3E5: _tmp252=_tmp251.f2;if((unsigned int)_tmp252 > 1?*((
int*)_tmp252)== 0: 0){_LL3E6: _tmp253=(void*)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp252)->f1;if((unsigned int)_tmp253 > 1?*((int*)_tmp253)== 0: 0){_LL3E7: _tmp254=((
struct Cyc_Absyn_Upper_b_struct*)_tmp253)->f1;goto _LL3DE;}else{goto _LL3DF;}}else{
goto _LL3DF;}_LL3DF: _LL3ED: _tmp25A=_tmp251.f1;if((unsigned int)_tmp25A > 1?*((int*)
_tmp25A)== 0: 0){_LL3EE: _tmp25B=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp25A)->f1;
if(_tmp25B == 0){goto _LL3EA;}else{goto _LL3E1;}}else{goto _LL3E1;}_LL3EA: _tmp257=
_tmp251.f2;if((unsigned int)_tmp257 > 1?*((int*)_tmp257)== 0: 0){_LL3EB: _tmp258=(
void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp257)->f1;if((unsigned int)_tmp258 > 
1?*((int*)_tmp258)== 0: 0){_LL3EC: _tmp259=((struct Cyc_Absyn_Upper_b_struct*)
_tmp258)->f1;goto _LL3E0;}else{goto _LL3E1;}}else{goto _LL3E1;}_LL3E1: _LL3EF:
_tmp25C=_tmp251.f2;if((unsigned int)_tmp25C > 1?*((int*)_tmp25C)== 0: 0){_LL3F0:
_tmp25D=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp25C)->f1;if((int)_tmp25D
== 0){goto _LL3E2;}else{goto _LL3E3;}}else{goto _LL3E3;}_LL3E3: goto _LL3E4;_LL3DE: ps=(
void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp25E=_cycalloc(sizeof(*_tmp25E));
_tmp25E[0]=({struct Cyc_Absyn_Nullable_ps_struct _tmp25F;_tmp25F.tag=1;_tmp25F.f1=
_tmp254;_tmp25F;});_tmp25E;});goto _LL3DC;_LL3E0: ps=(void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp260=_cycalloc(sizeof(*_tmp260));_tmp260[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp261;_tmp261.tag=0;_tmp261.f1=_tmp259;_tmp261;});_tmp260;});goto _LL3DC;_LL3E2:
ps=(void*)0;goto _LL3DC;_LL3E4: ps=(void*)0;goto _LL3DC;_LL3DC:;}{void*tm=(void*)({
struct Cyc_Absyn_Pointer_mod_struct*_tmp264=_cycalloc(sizeof(*_tmp264));_tmp264[0]=({
struct Cyc_Absyn_Pointer_mod_struct _tmp265;_tmp265.tag=1;_tmp265.f1=(void*)ps;
_tmp265.f2=(void*)_tmp233;_tmp265.f3=tq;_tmp265;});_tmp264;});return({struct
_tuple5 _tmp262;_tmp262.f1=_tmp24F;_tmp262.f2=_tmp24E;_tmp262.f3=({struct Cyc_List_List*
_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263->hd=(void*)tm;_tmp263->tl=_tmp24D;
_tmp263;});_tmp262;});}}}_LL3B5: if(! Cyc_Absynpp_print_all_tvars){if(_tmp23C == 0?
1: Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp23C->v)){_tmp23C=0;_tmp23D=0;}}
else{if(Cyc_Absynpp_rewrite_temp_tvars){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp23D);}}{
struct Cyc_List_List*_tmp267;void*_tmp268;struct Cyc_Absyn_Tqual _tmp269;struct
_tuple5 _tmp266=Cyc_Absynpp_to_tms(Cyc_Absyn_empty_tqual(),_tmp23B);_LL3F4:
_tmp269=_tmp266.f1;goto _LL3F3;_LL3F3: _tmp268=_tmp266.f2;goto _LL3F2;_LL3F2:
_tmp267=_tmp266.f3;goto _LL3F1;_LL3F1: {struct Cyc_List_List*tms=_tmp267;if(
_tmp236 != 0? ! Cyc_Absynpp_to_VC: 0){tms=({struct Cyc_List_List*_tmp26A=_cycalloc(
sizeof(*_tmp26A));_tmp26A->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp26C;_tmp26C.tag=4;_tmp26C.f1=0;_tmp26C.f2=_tmp236;_tmp26C;});_tmp26B;}));
_tmp26A->tl=tms;_tmp26A;});}tms=({struct Cyc_List_List*_tmp26D=_cycalloc(sizeof(*
_tmp26D));_tmp26D->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp26F;_tmp26F.tag=2;_tmp26F.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp271;_tmp271.tag=1;_tmp271.f1=_tmp23A;_tmp271.f2=_tmp239;_tmp271.f3=_tmp238;
_tmp271.f4=_tmp23C;_tmp271.f5=_tmp237;_tmp271;});_tmp270;}));_tmp26F;});_tmp26E;}));
_tmp26D->tl=tms;_tmp26D;});if(Cyc_Absynpp_to_VC){for(0;_tmp236 != 0;_tmp236=
_tmp236->tl){void*_tmp272=(void*)_tmp236->hd;_LL3F6: if((int)_tmp272 == 0){goto
_LL3F7;}else{goto _LL3F8;}_LL3F8: if((int)_tmp272 == 1){goto _LL3F9;}else{goto _LL3FA;}
_LL3FA: if((int)_tmp272 == 2){goto _LL3FB;}else{goto _LL3FC;}_LL3FC: goto _LL3FD;
_LL3F7: goto _LL3F9;_LL3F9: goto _LL3FB;_LL3FB: tms=({struct Cyc_List_List*_tmp273=
_cycalloc(sizeof(*_tmp273));_tmp273->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp275;_tmp275.tag=4;_tmp275.f1=0;_tmp275.f2=({struct Cyc_List_List*_tmp276=
_cycalloc(sizeof(*_tmp276));_tmp276->hd=(void*)((void*)_tmp236->hd);_tmp276->tl=
0;_tmp276;});_tmp275;});_tmp274;}));_tmp273->tl=tms;_tmp273;});goto AfterAtts;
_LL3FD: goto _LL3F5;_LL3F5:;}}AfterAtts: if(_tmp23D != 0){tms=({struct Cyc_List_List*
_tmp277=_cycalloc(sizeof(*_tmp277));_tmp277->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp279;_tmp279.tag=3;_tmp279.f1=_tmp23D;_tmp279.f2=0;_tmp279.f3=1;_tmp279;});
_tmp278;}));_tmp277->tl=tms;_tmp277;});}return({struct _tuple5 _tmp27A;_tmp27A.f1=
_tmp269;_tmp27A.f2=_tmp268;_tmp27A.f3=tms;_tmp27A;});}}_LL3B7: if(_tmp23F == 0){
return({struct _tuple5 _tmp27B;_tmp27B.f1=tq;_tmp27B.f2=t;_tmp27B.f3=0;_tmp27B;});}
else{return Cyc_Absynpp_to_tms(tq,(void*)_tmp23F->v);}_LL3B9: if((_tmp241 == 0? 1: !
Cyc_Absynpp_expand_typedefs)? 1: Cyc_Absynpp_is_anon_aggrtype((void*)_tmp241->v)){
return({struct _tuple5 _tmp27C;_tmp27C.f1=tq;_tmp27C.f2=t;_tmp27C.f3=0;_tmp27C;});}
else{return Cyc_Absynpp_to_tms(tq,(void*)_tmp241->v);}_LL3BB: return({struct
_tuple5 _tmp27D;_tmp27D.f1=tq;_tmp27D.f2=t;_tmp27D.f3=0;_tmp27D;});_LL3AF:;}
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*
dopt){struct Cyc_List_List*_tmp27F;void*_tmp280;struct Cyc_Absyn_Tqual _tmp281;
struct _tuple5 _tmp27E=Cyc_Absynpp_to_tms(tq,t);_LL401: _tmp281=_tmp27E.f1;goto
_LL400;_LL400: _tmp280=_tmp27E.f2;goto _LL3FF;_LL3FF: _tmp27F=_tmp27E.f3;goto _LL3FE;
_LL3FE: _tmp27F=Cyc_List_imp_rev(_tmp27F);if(_tmp27F == 0? dopt == 0: 0){return({
struct Cyc_PP_Doc*_tmp282[2];_tmp282[1]=Cyc_Absynpp_ntyp2doc(_tmp280);_tmp282[0]=
Cyc_Absynpp_tqual2doc(_tmp281);Cyc_PP_cat(_tag_arr(_tmp282,sizeof(struct Cyc_PP_Doc*),
2));});}else{return({struct Cyc_PP_Doc*_tmp283[4];_tmp283[3]=Cyc_Absynpp_dtms2doc(
dopt == 0? Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp27F);_tmp283[2]=Cyc_PP_text(
_tag_arr(" ",sizeof(unsigned char),2));_tmp283[1]=Cyc_Absynpp_ntyp2doc(_tmp280);
_tmp283[0]=Cyc_Absynpp_tqual2doc(_tmp281);Cyc_PP_cat(_tag_arr(_tmp283,sizeof(
struct Cyc_PP_Doc*),4));});}}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*
f){if(f->width != 0){return({struct Cyc_PP_Doc*_tmp284[5];_tmp284[4]=Cyc_PP_text(
_tag_arr(";",sizeof(unsigned char),2));_tmp284[3]=Cyc_Absynpp_atts2doc(f->attributes);
_tmp284[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));
_tmp284[1]=Cyc_PP_text(_tag_arr(":",sizeof(unsigned char),2));_tmp284[0]=Cyc_Absynpp_tqtd2doc(
f->tq,(void*)f->type,({struct Cyc_Core_Opt*_tmp285=_cycalloc(sizeof(*_tmp285));
_tmp285->v=Cyc_PP_textptr(f->name);_tmp285;}));Cyc_PP_cat(_tag_arr(_tmp284,
sizeof(struct Cyc_PP_Doc*),5));});}else{return({struct Cyc_PP_Doc*_tmp286[3];
_tmp286[2]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmp286[1]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp286[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp287=_cycalloc(sizeof(*_tmp287));_tmp287->v=Cyc_PP_textptr(f->name);_tmp287;}));
Cyc_PP_cat(_tag_arr(_tmp286,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _tagged_arr sep,struct
Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tag_arr("",sizeof(
unsigned char),1),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct
Cyc_Absyn_Tunionfield*f){return({struct Cyc_PP_Doc*_tmp288[4];_tmp288[3]=f->typs
== 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs);_tmp288[2]=f->tvs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_ktvars2doc(f->tvs);_tmp288[1]=Cyc_Absynpp_typedef_name2doc(f->name);
_tmp288[0]=Cyc_Absynpp_scope2doc((void*)f->sc);Cyc_PP_cat(_tag_arr(_tmp288,
sizeof(struct Cyc_PP_Doc*),4));});}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Tunionfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_tunionfield2doc,_tag_arr(",",sizeof(unsigned char),2),fields);}void
Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)
tdl->hd),72,f);({void*_tmp289[0]={};Cyc_Std_fprintf(f,_tag_arr("\n",sizeof(
unsigned char),2),_tag_arr(_tmp289,sizeof(void*),0));});}}struct _tagged_arr Cyc_Absynpp_decllist2string(
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
return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
