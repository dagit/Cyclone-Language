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
  unsigned total_bytes;
  unsigned free_bytes;
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
extern void* _region_malloc(struct _RegionHandle *, unsigned);
extern void* _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void  _free_region(struct _RegionHandle *);

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
extern int _throw(void* e);
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
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
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
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
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
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

static inline void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned x, unsigned y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void* _profile_GC_malloc(int,char *file,int lineno);
extern void* _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
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
 struct Cyc_Core_Opt{void*v;};struct _tagged_arr Cyc_Core_new_string(unsigned int);
extern unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
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
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};int Cyc_Std_fprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr);struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern unsigned char Cyc_List_Nth[8];
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);int Cyc_List_list_prefix(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,
struct Cyc_Std___cycFILE*f);struct _tagged_arr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*
d,int w);struct Cyc_PP_Doc*Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_line_doc();
struct Cyc_PP_Doc*Cyc_PP_text(struct _tagged_arr s);struct Cyc_PP_Doc*Cyc_PP_textptr(
struct _tagged_arr*p);struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
struct Cyc_PP_Doc*Cyc_PP_cat(struct _tagged_arr);struct Cyc_PP_Doc*Cyc_PP_ppseq(
struct Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*l);struct
Cyc_PP_Doc*Cyc_PP_seql(struct _tagged_arr sep,struct Cyc_List_List*l0);struct Cyc_PP_Doc*
Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*
l);struct Cyc_PP_Doc*Cyc_PP_group(struct _tagged_arr start,struct _tagged_arr stop,
struct _tagged_arr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(
struct _tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,struct Cyc_List_List*
l);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};extern unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
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
f3;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Pat{void*r;
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
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
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
15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_compress_kb(void*);struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct _tagged_arr Cyc_Absyn_attribute2string(void*);struct _tuple3{void*f1;struct
_tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(void*);struct Cyc_Buffer_t;
int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);struct
_tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,struct _tagged_arr);
unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];
struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Tcenv_VarRes_struct{int tag;void*
f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*
f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*
aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct
Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*
availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;};struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{int tag;void*f1;void*f2;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*
le;};void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f);struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d);struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct
_tagged_arr Cyc_Absynpp_typ2cstring(void*);struct _tagged_arr Cyc_Absynpp_kind2string(
void*);struct _tagged_arr Cyc_Absynpp_kindbound2string(void*);struct _tagged_arr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _tagged_arr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _tagged_arr Cyc_Absynpp_decllist2string(
struct Cyc_List_List*tdl);struct _tagged_arr Cyc_Absynpp_prim2string(void*p);struct
_tagged_arr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);struct _tagged_arr Cyc_Absynpp_scope2string(
void*sc);int Cyc_Absynpp_is_anon_aggrtype(void*t);extern struct _tagged_arr Cyc_Absynpp_cyc_string;
extern struct _tagged_arr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct
Cyc_Absyn_Exp*);struct _tagged_arr Cyc_Absynpp_char_escape(unsigned char);struct
_tagged_arr Cyc_Absynpp_string_escape(struct _tagged_arr);struct _tagged_arr Cyc_Absynpp_prim2str(
void*p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);struct _tuple5{
struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(
struct _tuple5*arg);struct _tuple6{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*
f3;};struct _tuple6 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);struct
_tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(
struct _tuple7*dp);struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*
cs);struct Cyc_PP_Doc*Cyc_Absynpp_switchCclauses2doc(struct Cyc_List_List*cs);
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*);struct
Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(
struct Cyc_Absyn_Exp*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct
Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*
es);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*);struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,
void*p,struct Cyc_List_List*es);struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple8*de);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct
Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
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
Cyc_List_List*)_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((
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
sizeof(unsigned char),3);default: _LLA: if(c >= ' '?c <= '~': 0){struct _tagged_arr
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
for(0;i < sz;i ++){unsigned char c=((const unsigned char*)s.curr)[i];if(((c <= ' '?1:
c >= '~')?1: c == '"')?1: c == '\\')return 1;}}return 0;}struct _tagged_arr Cyc_Absynpp_string_escape(
struct _tagged_arr s){if(!Cyc_Absynpp_special(s))return s;{int n=(int)(_get_arr_size(
s,sizeof(unsigned char))- 1);if(n > 0?((const unsigned char*)s.curr)[n]== '\000': 0)
n --;{int len=0;{int i=0;for(0;i <= n;i ++){unsigned char _tmp4=((const unsigned char*)
s.curr)[i];_LLD: if(_tmp4 != '\a')goto _LLF;_LLE: goto _LL10;_LLF: if(_tmp4 != '\b')
goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp4 != '\f')goto _LL13;_LL12: goto _LL14;_LL13:
if(_tmp4 != '\n')goto _LL15;_LL14: goto _LL16;_LL15: if(_tmp4 != '\r')goto _LL17;_LL16:
goto _LL18;_LL17: if(_tmp4 != '\t')goto _LL19;_LL18: goto _LL1A;_LL19: if(_tmp4 != '\v')
goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp4 != '\\')goto _LL1D;_LL1C: goto _LL1E;_LL1D:
if(_tmp4 != '"')goto _LL1F;_LL1E: len +=2;goto _LLC;_LL1F:;_LL20: if(_tmp4 >= ' '?_tmp4
<= '~': 0)len ++;else{len +=4;}goto _LLC;_LLC:;}}{struct _tagged_arr t=Cyc_Core_new_string((
unsigned int)len);int j=0;{int i=0;for(0;i <= n;i ++){unsigned char _tmp5=((const
unsigned char*)s.curr)[i];_LL22: if(_tmp5 != '\a')goto _LL24;_LL23:*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='a';goto _LL21;_LL24: if(
_tmp5 != '\b')goto _LL26;_LL25:*((unsigned char*)_check_unknown_subscript(t,
sizeof(unsigned char),j ++))='\\';*((unsigned char*)_check_unknown_subscript(t,
sizeof(unsigned char),j ++))='b';goto _LL21;_LL26: if(_tmp5 != '\f')goto _LL28;_LL27:*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='f';goto
_LL21;_LL28: if(_tmp5 != '\n')goto _LL2A;_LL29:*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='n';goto _LL21;_LL2A: if(
_tmp5 != '\r')goto _LL2C;_LL2B:*((unsigned char*)_check_unknown_subscript(t,
sizeof(unsigned char),j ++))='\\';*((unsigned char*)_check_unknown_subscript(t,
sizeof(unsigned char),j ++))='r';goto _LL21;_LL2C: if(_tmp5 != '\t')goto _LL2E;_LL2D:*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='t';goto
_LL21;_LL2E: if(_tmp5 != '\v')goto _LL30;_LL2F:*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))='v';goto _LL21;_LL30: if(
_tmp5 != '\\')goto _LL32;_LL31:*((unsigned char*)_check_unknown_subscript(t,
sizeof(unsigned char),j ++))='\\';*((unsigned char*)_check_unknown_subscript(t,
sizeof(unsigned char),j ++))='\\';goto _LL21;_LL32: if(_tmp5 != '"')goto _LL34;_LL33:*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='"';goto
_LL21;_LL34:;_LL35: if(_tmp5 >= ' '?_tmp5 <= '~': 0)*((unsigned char*)
_check_unknown_subscript(t,sizeof(unsigned char),j ++))=_tmp5;else{*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))='\\';*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))=(
unsigned char)('0' + (_tmp5 >> 6 & 7));*((unsigned char*)_check_unknown_subscript(t,
sizeof(unsigned char),j ++))=(unsigned char)('0' + (_tmp5 >> 3 & 7));*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),j ++))=(
unsigned char)('0' + (_tmp5 & 7));}goto _LL21;_LL21:;}}return(struct _tagged_arr)t;}}}}
static unsigned char _tmp6[9]="restrict";static struct _tagged_arr Cyc_Absynpp_restrict_string={
_tmp6,_tmp6,_tmp6 + 9};static unsigned char _tmp7[9]="volatile";static struct
_tagged_arr Cyc_Absynpp_volatile_string={_tmp7,_tmp7,_tmp7 + 9};static
unsigned char _tmp8[6]="const";static struct _tagged_arr Cyc_Absynpp_const_str={
_tmp8,_tmp8,_tmp8 + 6};static struct _tagged_arr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _tagged_arr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _tagged_arr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict)
l=({struct Cyc_List_List*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->hd=Cyc_Absynpp_restrict_sp;
_tmp9->tl=l;_tmp9;});if(tq.q_volatile)l=({struct Cyc_List_List*_tmpA=_cycalloc(
sizeof(*_tmpA));_tmpA->hd=Cyc_Absynpp_volatile_sp;_tmpA->tl=l;_tmpA;});if(tq.q_const)
l=({struct Cyc_List_List*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->hd=Cyc_Absynpp_const_sp;
_tmpB->tl=l;_tmpB;});return Cyc_PP_egroup(_tag_arr("",sizeof(unsigned char),1),
_tag_arr(" ",sizeof(unsigned char),2),_tag_arr(" ",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,l));}struct _tagged_arr Cyc_Absynpp_kind2string(
void*k){void*_tmpC=k;_LL37: if((int)_tmpC != 0)goto _LL39;_LL38: return _tag_arr("A",
sizeof(unsigned char),2);_LL39: if((int)_tmpC != 1)goto _LL3B;_LL3A: return _tag_arr("M",
sizeof(unsigned char),2);_LL3B: if((int)_tmpC != 2)goto _LL3D;_LL3C: return _tag_arr("B",
sizeof(unsigned char),2);_LL3D: if((int)_tmpC != 3)goto _LL3F;_LL3E: return _tag_arr("R",
sizeof(unsigned char),2);_LL3F: if((int)_tmpC != 4)goto _LL36;_LL40: return _tag_arr("E",
sizeof(unsigned char),2);_LL36:;}struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*c){void*_tmpD=Cyc_Absyn_compress_kb(c);void*_tmpE;void*_tmpF;_LL42: if(*((
int*)_tmpD)!= 0)goto _LL44;_tmpE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpD)->f1;
_LL43: return Cyc_Absynpp_kind2string(_tmpE);_LL44: if(*((int*)_tmpD)!= 1)goto _LL46;
_LL45: return _tag_arr("?",sizeof(unsigned char),2);_LL46: if(*((int*)_tmpD)!= 2)
goto _LL41;_tmpF=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpD)->f2;_LL47: return
Cyc_Absynpp_kind2string(_tmpF);_LL41:;}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
void*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(
void*c){return Cyc_PP_text(Cyc_Absynpp_kindbound2string(c));}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(
struct Cyc_List_List*ts){return Cyc_PP_egroup(_tag_arr("<",sizeof(unsigned char),2),
_tag_arr(">",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_typ2doc,ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*
tv){void*_tmp10=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp11;void*_tmp12;
void*_tmp13;_LL49: if(*((int*)_tmp10)!= 1)goto _LL4B;_LL4A: goto _LL4C;_LL4B: if(*((
int*)_tmp10)!= 0)goto _LL4D;_tmp11=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp10)->f1;
if((int)_tmp11 != 2)goto _LL4D;_LL4C: return Cyc_PP_textptr(tv->name);_LL4D: if(*((
int*)_tmp10)!= 2)goto _LL4F;_tmp12=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp10)->f2;_LL4E: _tmp13=_tmp12;goto _LL50;_LL4F: if(*((int*)_tmp10)!= 0)goto _LL48;
_tmp13=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp10)->f1;_LL50: return({struct
Cyc_PP_Doc*_tmp14[3];_tmp14[2]=Cyc_Absynpp_kind2doc(_tmp13);_tmp14[1]=Cyc_PP_text(
_tag_arr("::",sizeof(unsigned char),3));_tmp14[0]=Cyc_PP_textptr(tv->name);Cyc_PP_cat(
_tag_arr(_tmp14,sizeof(struct Cyc_PP_Doc*),3));});_LL48:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs){return Cyc_PP_egroup(_tag_arr("<",sizeof(unsigned char),
2),_tag_arr(">",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}static struct _tagged_arr*Cyc_Absynpp_get_name(
struct Cyc_Absyn_Tvar*tv){return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(
struct Cyc_List_List*tvs){if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(
tvs);return Cyc_PP_egroup(_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));}struct _tuple9{
struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct
_tuple9*t){return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(
struct Cyc_List_List*ts){return Cyc_PP_group(_tag_arr("(",sizeof(unsigned char),2),
_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(
void*att){void*_tmp15=att;_LL52: if((int)_tmp15 != 0)goto _LL54;_LL53: goto _LL55;
_LL54: if((int)_tmp15 != 1)goto _LL56;_LL55: goto _LL57;_LL56: if((int)_tmp15 != 2)goto
_LL58;_LL57: return Cyc_PP_nil_doc();_LL58:;_LL59: return Cyc_PP_text(Cyc_Absyn_attribute2string(
att));_LL51:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp16=(void*)atts->hd;_LL5B: if((int)_tmp16 != 
0)goto _LL5D;_LL5C: return Cyc_PP_text(_tag_arr(" _stdcall ",sizeof(unsigned char),
11));_LL5D: if((int)_tmp16 != 1)goto _LL5F;_LL5E: return Cyc_PP_text(_tag_arr(" _cdecl ",
sizeof(unsigned char),9));_LL5F: if((int)_tmp16 != 2)goto _LL61;_LL60: return Cyc_PP_text(
_tag_arr(" _fastcall ",sizeof(unsigned char),12));_LL61:;_LL62: goto _LL5A;_LL5A:;}
return Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp17=(void*)atts2->hd;_LL64: if((int)_tmp17 != 0)goto _LL66;_LL65: goto _LL67;
_LL66: if((int)_tmp17 != 1)goto _LL68;_LL67: goto _LL69;_LL68: if((int)_tmp17 != 2)goto
_LL6A;_LL69: goto _LL63;_LL6A:;_LL6B: hasatt=1;goto _LL63;_LL63:;}}if(!hasatt)return
Cyc_PP_nil_doc();return({struct Cyc_PP_Doc*_tmp18[3];_tmp18[2]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp18[1]=Cyc_PP_group(_tag_arr("",
sizeof(unsigned char),1),_tag_arr("",sizeof(unsigned char),1),_tag_arr(" ",
sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));_tmp18[0]=
Cyc_PP_text(_tag_arr("__declspec(",sizeof(unsigned char),12));Cyc_PP_cat(
_tag_arr(_tmp18,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();if(Cyc_Absynpp_to_VC)
return Cyc_Absynpp_noncallconv2doc(atts);return({struct Cyc_PP_Doc*_tmp19[2];
_tmp19[1]=Cyc_PP_group(_tag_arr("((",sizeof(unsigned char),3),_tag_arr("))",
sizeof(unsigned char),3),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,
atts));_tmp19[0]=Cyc_PP_text(_tag_arr(" __attribute__",sizeof(unsigned char),15));
Cyc_PP_cat(_tag_arr(_tmp19,sizeof(struct Cyc_PP_Doc*),2));});}int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmp1A=(void*)tms->hd;_LL6D:
if(_tmp1A <= (void*)1?1:*((int*)_tmp1A)!= 1)goto _LL6F;_LL6E: return 1;_LL6F: if(
_tmp1A <= (void*)1?1:*((int*)_tmp1A)!= 4)goto _LL71;_LL70: if(!Cyc_Absynpp_to_VC)
return 0;return Cyc_Absynpp_next_is_pointer(tms->tl);_LL71:;_LL72: return 0;_LL6C:;}}
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(
struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*
rest=Cyc_Absynpp_dtms2doc(d,tms->tl);struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*
_tmp3D[3];_tmp3D[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp3D[1]=
rest;_tmp3D[0]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp3D,sizeof(struct Cyc_PP_Doc*),3));});void*_tmp1B=(void*)tms->hd;
struct Cyc_Absyn_Exp*_tmp1C;void*_tmp1D;struct Cyc_List_List*_tmp1E;struct Cyc_List_List*
_tmp1F;struct Cyc_Position_Segment*_tmp20;int _tmp21;void*_tmp22;void*_tmp23;
struct Cyc_Absyn_Tqual _tmp24;_LL74: if((int)_tmp1B != 0)goto _LL76;_LL75: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*_tmp25[2];_tmp25[1]=Cyc_PP_text(
_tag_arr("[]",sizeof(unsigned char),3));_tmp25[0]=rest;Cyc_PP_cat(_tag_arr(
_tmp25,sizeof(struct Cyc_PP_Doc*),2));});_LL76: if(_tmp1B <= (void*)1?1:*((int*)
_tmp1B)!= 0)goto _LL78;_tmp1C=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp1B)->f1;
_LL77: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*
_tmp26[4];_tmp26[3]=Cyc_PP_text(_tag_arr("]",sizeof(unsigned char),2));_tmp26[2]=
Cyc_Absynpp_exp2doc(_tmp1C);_tmp26[1]=Cyc_PP_text(_tag_arr("[",sizeof(
unsigned char),2));_tmp26[0]=rest;Cyc_PP_cat(_tag_arr(_tmp26,sizeof(struct Cyc_PP_Doc*),
4));});_LL78: if(_tmp1B <= (void*)1?1:*((int*)_tmp1B)!= 2)goto _LL7A;_tmp1D=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmp1B)->f1;_LL79: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{void*_tmp27=_tmp1D;struct Cyc_List_List*_tmp28;int _tmp29;
struct Cyc_Absyn_VarargInfo*_tmp2A;struct Cyc_Core_Opt*_tmp2B;struct Cyc_List_List*
_tmp2C;struct Cyc_List_List*_tmp2D;struct Cyc_Position_Segment*_tmp2E;_LL81: if(*((
int*)_tmp27)!= 1)goto _LL83;_tmp28=((struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f1;
_tmp29=((struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f2;_tmp2A=((struct Cyc_Absyn_WithTypes_struct*)
_tmp27)->f3;_tmp2B=((struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f4;_tmp2C=((
struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f5;_LL82: return({struct Cyc_PP_Doc*
_tmp2F[2];_tmp2F[1]=Cyc_Absynpp_funargs2doc(_tmp28,_tmp29,_tmp2A,_tmp2B,_tmp2C);
_tmp2F[0]=rest;Cyc_PP_cat(_tag_arr(_tmp2F,sizeof(struct Cyc_PP_Doc*),2));});_LL83:
if(*((int*)_tmp27)!= 0)goto _LL80;_tmp2D=((struct Cyc_Absyn_NoTypes_struct*)_tmp27)->f1;
_tmp2E=((struct Cyc_Absyn_NoTypes_struct*)_tmp27)->f2;_LL84: return({struct Cyc_PP_Doc*
_tmp30[2];_tmp30[1]=Cyc_PP_group(_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,_tmp2D));_tmp30[0]=rest;Cyc_PP_cat(_tag_arr(_tmp30,sizeof(struct
Cyc_PP_Doc*),2));});_LL80:;}_LL7A: if(_tmp1B <= (void*)1?1:*((int*)_tmp1B)!= 4)
goto _LL7C;_tmp1E=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp1B)->f2;_LL7B: if(!
Cyc_Absynpp_to_VC){if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;return({
struct Cyc_PP_Doc*_tmp31[2];_tmp31[1]=Cyc_Absynpp_atts2doc(_tmp1E);_tmp31[0]=rest;
Cyc_PP_cat(_tag_arr(_tmp31,sizeof(struct Cyc_PP_Doc*),2));});}else{if(Cyc_Absynpp_next_is_pointer(
tms->tl))return({struct Cyc_PP_Doc*_tmp32[2];_tmp32[1]=rest;_tmp32[0]=Cyc_Absynpp_callconv2doc(
_tmp1E);Cyc_PP_cat(_tag_arr(_tmp32,sizeof(struct Cyc_PP_Doc*),2));});return rest;}
_LL7C: if(_tmp1B <= (void*)1?1:*((int*)_tmp1B)!= 3)goto _LL7E;_tmp1F=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp1B)->f1;_tmp20=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp1B)->f2;_tmp21=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmp1B)->f3;_LL7D: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmp21)return({struct Cyc_PP_Doc*_tmp33[2];_tmp33[1]=Cyc_Absynpp_ktvars2doc(
_tmp1F);_tmp33[0]=rest;Cyc_PP_cat(_tag_arr(_tmp33,sizeof(struct Cyc_PP_Doc*),2));});
else{return({struct Cyc_PP_Doc*_tmp34[2];_tmp34[1]=Cyc_Absynpp_tvars2doc(_tmp1F);
_tmp34[0]=rest;Cyc_PP_cat(_tag_arr(_tmp34,sizeof(struct Cyc_PP_Doc*),2));});}
_LL7E: if(_tmp1B <= (void*)1?1:*((int*)_tmp1B)!= 1)goto _LL73;_tmp22=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp1B)->f1;_tmp23=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp1B)->f2;_tmp24=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp1B)->f3;_LL7F: {
struct Cyc_PP_Doc*ptr;{void*_tmp35=_tmp22;struct Cyc_Absyn_Exp*_tmp36;struct Cyc_Absyn_Exp*
_tmp37;_LL86: if(_tmp35 <= (void*)1?1:*((int*)_tmp35)!= 1)goto _LL88;_tmp36=((
struct Cyc_Absyn_Nullable_ps_struct*)_tmp35)->f1;_LL87: if(Cyc_Evexp_eval_const_uint_exp(
_tmp36)== 1)ptr=Cyc_PP_text(_tag_arr("*",sizeof(unsigned char),2));else{ptr=({
struct Cyc_PP_Doc*_tmp38[4];_tmp38[3]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp38[2]=Cyc_Absynpp_exp2doc(_tmp36);_tmp38[1]=Cyc_PP_text(
_tag_arr("{",sizeof(unsigned char),2));_tmp38[0]=Cyc_PP_text(_tag_arr("*",
sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp38,sizeof(struct Cyc_PP_Doc*),4));});}
goto _LL85;_LL88: if(_tmp35 <= (void*)1?1:*((int*)_tmp35)!= 0)goto _LL8A;_tmp37=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmp35)->f1;_LL89: if(Cyc_Evexp_eval_const_uint_exp(
_tmp37)== 1)ptr=Cyc_PP_text(_tag_arr("@",sizeof(unsigned char),2));else{ptr=({
struct Cyc_PP_Doc*_tmp39[4];_tmp39[3]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp39[2]=Cyc_Absynpp_exp2doc(_tmp37);_tmp39[1]=Cyc_PP_text(
_tag_arr("{",sizeof(unsigned char),2));_tmp39[0]=Cyc_PP_text(_tag_arr("@",
sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp39,sizeof(struct Cyc_PP_Doc*),4));});}
goto _LL85;_LL8A: if((int)_tmp35 != 0)goto _LL85;_LL8B: ptr=Cyc_PP_text(_tag_arr("?",
sizeof(unsigned char),2));goto _LL85;_LL85:;}{void*_tmp3A=Cyc_Tcutil_compress(
_tmp23);_LL8D: if((int)_tmp3A != 2)goto _LL8F;_LL8E: return({struct Cyc_PP_Doc*_tmp3B[
2];_tmp3B[1]=rest;_tmp3B[0]=ptr;Cyc_PP_cat(_tag_arr(_tmp3B,sizeof(struct Cyc_PP_Doc*),
2));});_LL8F:;_LL90: return({struct Cyc_PP_Doc*_tmp3C[4];_tmp3C[3]=rest;_tmp3C[2]=
Cyc_PP_text(_tag_arr(" ",sizeof(unsigned char),2));_tmp3C[1]=Cyc_Absynpp_typ2doc(
_tmp23);_tmp3C[0]=ptr;Cyc_PP_cat(_tag_arr(_tmp3C,sizeof(struct Cyc_PP_Doc*),4));});
_LL8C:;}}_LL73:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp3E=Cyc_Tcutil_compress(
t);_LL92: if((int)_tmp3E != 2)goto _LL94;_LL93: return Cyc_PP_text(_tag_arr("`H",
sizeof(unsigned char),3));_LL94:;_LL95: return Cyc_Absynpp_ntyp2doc(t);_LL91:;}
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t){void*_tmp3F=Cyc_Tcutil_compress(t);void*_tmp40;struct Cyc_List_List*
_tmp41;_LL97: if(_tmp3F <= (void*)3?1:*((int*)_tmp3F)!= 17)goto _LL99;_tmp40=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp3F)->f1;_LL98:*rgions=({struct Cyc_List_List*
_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->hd=Cyc_Absynpp_rgn2doc(_tmp40);_tmp42->tl=*
rgions;_tmp42;});goto _LL96;_LL99: if(_tmp3F <= (void*)3?1:*((int*)_tmp3F)!= 18)
goto _LL9B;_tmp41=((struct Cyc_Absyn_JoinEff_struct*)_tmp3F)->f1;_LL9A: for(0;
_tmp41 != 0;_tmp41=_tmp41->tl){Cyc_Absynpp_effects2docs(rgions,effects,(void*)
_tmp41->hd);}goto _LL96;_LL9B:;_LL9C:*effects=({struct Cyc_List_List*_tmp43=
_cycalloc(sizeof(*_tmp43));_tmp43->hd=Cyc_Absynpp_typ2doc(t);_tmp43->tl=*effects;
_tmp43;});goto _LL96;_LL96:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct
Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(&
rgions,& effects,t);rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
rgions);effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
effects);if(rgions == 0?effects != 0: 0)return Cyc_PP_group(_tag_arr("",sizeof(
unsigned char),1),_tag_arr("",sizeof(unsigned char),1),_tag_arr("+",sizeof(
unsigned char),2),effects);else{struct Cyc_PP_Doc*_tmp44=Cyc_PP_group(_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2),rgions);return Cyc_PP_group(_tag_arr("",sizeof(
unsigned char),1),_tag_arr("",sizeof(unsigned char),1),_tag_arr("+",sizeof(
unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(effects,({struct Cyc_List_List*_tmp45=_cycalloc(sizeof(*
_tmp45));_tmp45->hd=_tmp44;_tmp45->tl=0;_tmp45;})));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(
void*k){void*_tmp46=k;_LL9E: if((int)_tmp46 != 0)goto _LLA0;_LL9F: return Cyc_PP_text(
_tag_arr("struct ",sizeof(unsigned char),8));_LLA0: if((int)_tmp46 != 1)goto _LL9D;
_LLA1: return Cyc_PP_text(_tag_arr("union ",sizeof(unsigned char),7));_LL9D:;}
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmp47=t;
struct Cyc_Core_Opt*_tmp48;struct Cyc_Core_Opt*_tmp49;int _tmp4A;struct Cyc_Core_Opt*
_tmp4B;struct Cyc_Absyn_Tvar*_tmp4C;struct Cyc_Absyn_TunionInfo _tmp4D;void*_tmp4E;
struct Cyc_List_List*_tmp4F;void*_tmp50;struct Cyc_Absyn_TunionFieldInfo _tmp51;
void*_tmp52;struct Cyc_List_List*_tmp53;void*_tmp54;void*_tmp55;int _tmp56;struct
Cyc_List_List*_tmp57;struct Cyc_Absyn_AggrInfo _tmp58;void*_tmp59;struct Cyc_List_List*
_tmp5A;void*_tmp5B;struct Cyc_List_List*_tmp5C;struct Cyc_List_List*_tmp5D;struct
_tuple0*_tmp5E;void*_tmp5F;struct _tuple0*_tmp60;struct Cyc_List_List*_tmp61;
struct Cyc_Core_Opt*_tmp62;void*_tmp63;void*_tmp64;_LLA3: if(_tmp47 <= (void*)3?1:*((
int*)_tmp47)!= 7)goto _LLA5;_LLA4: return Cyc_PP_text(_tag_arr("[[[array]]]",
sizeof(unsigned char),12));_LLA5: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 8)goto
_LLA7;_LLA6: return Cyc_PP_nil_doc();_LLA7: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)
!= 4)goto _LLA9;_LLA8: return Cyc_PP_nil_doc();_LLA9: if((int)_tmp47 != 0)goto _LLAB;
_LLAA: s=Cyc_PP_text(_tag_arr("void",sizeof(unsigned char),5));goto _LLA2;_LLAB:
if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 0)goto _LLAD;_tmp48=((struct Cyc_Absyn_Evar_struct*)
_tmp47)->f1;_tmp49=((struct Cyc_Absyn_Evar_struct*)_tmp47)->f2;_tmp4A=((struct Cyc_Absyn_Evar_struct*)
_tmp47)->f3;_tmp4B=((struct Cyc_Absyn_Evar_struct*)_tmp47)->f4;_LLAC: if(_tmp49 != 
0)return Cyc_Absynpp_ntyp2doc((void*)_tmp49->v);else{s=({struct Cyc_PP_Doc*_tmp65[
6];_tmp65[5]=_tmp48 == 0?Cyc_PP_text(_tag_arr("?",sizeof(unsigned char),2)): Cyc_Absynpp_kind2doc((
void*)_tmp48->v);_tmp65[4]=Cyc_PP_text(_tag_arr(")::",sizeof(unsigned char),4));
_tmp65[3]=(!Cyc_Absynpp_print_full_evars?1: _tmp4B == 0)?Cyc_PP_text(_tag_arr("",
sizeof(unsigned char),1)): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp4B->v);
_tmp65[2]=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp67;
_tmp67.tag=1;_tmp67.f1=(int)((unsigned int)_tmp4A);{void*_tmp66[1]={& _tmp67};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp66,sizeof(void*),1));}}));
_tmp65[1]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));_tmp65[0]=Cyc_PP_text(
_tag_arr("%",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp65,sizeof(struct
Cyc_PP_Doc*),6));});}goto _LLA2;_LLAD: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 1)
goto _LLAF;_tmp4C=((struct Cyc_Absyn_VarType_struct*)_tmp47)->f1;_LLAE: s=Cyc_PP_textptr(
_tmp4C->name);if(Cyc_Absynpp_print_all_kinds)s=({struct Cyc_PP_Doc*_tmp68[3];
_tmp68[2]=Cyc_Absynpp_kindbound2doc((void*)_tmp4C->kind);_tmp68[1]=Cyc_PP_text(
_tag_arr("::",sizeof(unsigned char),3));_tmp68[0]=s;Cyc_PP_cat(_tag_arr(_tmp68,
sizeof(struct Cyc_PP_Doc*),3));});if(Cyc_Absynpp_rewrite_temp_tvars?Cyc_Tcutil_is_temp_tvar(
_tmp4C): 0)s=({struct Cyc_PP_Doc*_tmp69[3];_tmp69[2]=Cyc_PP_text(_tag_arr(" */",
sizeof(unsigned char),4));_tmp69[1]=s;_tmp69[0]=Cyc_PP_text(_tag_arr("_ /* ",
sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp69,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLA2;_LLAF: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 2)goto _LLB1;_tmp4D=((
struct Cyc_Absyn_TunionType_struct*)_tmp47)->f1;_tmp4E=(void*)_tmp4D.tunion_info;
_tmp4F=_tmp4D.targs;_tmp50=(void*)_tmp4D.rgn;_LLB0:{void*_tmp6A=_tmp4E;struct Cyc_Absyn_UnknownTunionInfo
_tmp6B;struct _tuple0*_tmp6C;int _tmp6D;struct Cyc_Absyn_Tuniondecl**_tmp6E;struct
Cyc_Absyn_Tuniondecl*_tmp6F;struct Cyc_Absyn_Tuniondecl _tmp70;struct _tuple0*
_tmp71;int _tmp72;_LLD2: if(*((int*)_tmp6A)!= 0)goto _LLD4;_tmp6B=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp6A)->f1;_tmp6C=_tmp6B.name;_tmp6D=_tmp6B.is_xtunion;_LLD3: _tmp71=_tmp6C;
_tmp72=_tmp6D;goto _LLD5;_LLD4: if(*((int*)_tmp6A)!= 1)goto _LLD1;_tmp6E=((struct
Cyc_Absyn_KnownTunion_struct*)_tmp6A)->f1;_tmp6F=*_tmp6E;_tmp70=*_tmp6F;_tmp71=
_tmp70.name;_tmp72=_tmp70.is_xtunion;_LLD5: {struct Cyc_PP_Doc*_tmp73=Cyc_PP_text(
_tmp72?_tag_arr("xtunion ",sizeof(unsigned char),9): _tag_arr("tunion ",sizeof(
unsigned char),8));{void*_tmp74=Cyc_Tcutil_compress(_tmp50);_LLD7: if((int)_tmp74
!= 2)goto _LLD9;_LLD8: s=({struct Cyc_PP_Doc*_tmp75[3];_tmp75[2]=Cyc_Absynpp_tps2doc(
_tmp4F);_tmp75[1]=Cyc_Absynpp_qvar2doc(_tmp71);_tmp75[0]=_tmp73;Cyc_PP_cat(
_tag_arr(_tmp75,sizeof(struct Cyc_PP_Doc*),3));});goto _LLD6;_LLD9:;_LLDA: s=({
struct Cyc_PP_Doc*_tmp76[5];_tmp76[4]=Cyc_Absynpp_tps2doc(_tmp4F);_tmp76[3]=Cyc_Absynpp_qvar2doc(
_tmp71);_tmp76[2]=Cyc_PP_text(_tag_arr(" ",sizeof(unsigned char),2));_tmp76[1]=
Cyc_Absynpp_typ2doc(_tmp50);_tmp76[0]=_tmp73;Cyc_PP_cat(_tag_arr(_tmp76,sizeof(
struct Cyc_PP_Doc*),5));});goto _LLD6;_LLD6:;}goto _LLD1;}_LLD1:;}goto _LLA2;_LLB1:
if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 3)goto _LLB3;_tmp51=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp47)->f1;_tmp52=(void*)_tmp51.field_info;_tmp53=_tmp51.targs;_LLB2:{void*
_tmp77=_tmp52;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp78;struct _tuple0*_tmp79;
struct _tuple0*_tmp7A;int _tmp7B;struct Cyc_Absyn_Tuniondecl*_tmp7C;struct Cyc_Absyn_Tuniondecl
_tmp7D;struct _tuple0*_tmp7E;int _tmp7F;struct Cyc_Absyn_Tunionfield*_tmp80;struct
Cyc_Absyn_Tunionfield _tmp81;struct _tuple0*_tmp82;_LLDC: if(*((int*)_tmp77)!= 0)
goto _LLDE;_tmp78=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp77)->f1;_tmp79=
_tmp78.tunion_name;_tmp7A=_tmp78.field_name;_tmp7B=_tmp78.is_xtunion;_LLDD:
_tmp7E=_tmp79;_tmp7F=_tmp7B;_tmp82=_tmp7A;goto _LLDF;_LLDE: if(*((int*)_tmp77)!= 1)
goto _LLDB;_tmp7C=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp77)->f1;_tmp7D=*
_tmp7C;_tmp7E=_tmp7D.name;_tmp7F=_tmp7D.is_xtunion;_tmp80=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp77)->f2;_tmp81=*_tmp80;_tmp82=_tmp81.name;_LLDF: {struct Cyc_PP_Doc*_tmp83=
Cyc_PP_text(_tmp7F?_tag_arr("xtunion ",sizeof(unsigned char),9): _tag_arr("tunion ",
sizeof(unsigned char),8));s=({struct Cyc_PP_Doc*_tmp84[4];_tmp84[3]=Cyc_Absynpp_qvar2doc(
_tmp82);_tmp84[2]=Cyc_PP_text(_tag_arr(".",sizeof(unsigned char),2));_tmp84[1]=
Cyc_Absynpp_qvar2doc(_tmp7E);_tmp84[0]=_tmp83;Cyc_PP_cat(_tag_arr(_tmp84,sizeof(
struct Cyc_PP_Doc*),4));});goto _LLDB;}_LLDB:;}goto _LLA2;_LLB3: if(_tmp47 <= (void*)
3?1:*((int*)_tmp47)!= 5)goto _LLB5;_tmp54=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp47)->f1;_tmp55=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47)->f2;_LLB4: {
struct _tagged_arr sns;struct _tagged_arr ts;{void*_tmp85=_tmp54;_LLE1: if((int)
_tmp85 != 0)goto _LLE3;_LLE2: sns=_tag_arr("",sizeof(unsigned char),1);goto _LLE0;
_LLE3: if((int)_tmp85 != 1)goto _LLE0;_LLE4: sns=_tag_arr("unsigned ",sizeof(
unsigned char),10);goto _LLE0;_LLE0:;}{void*_tmp86=_tmp55;_LLE6: if((int)_tmp86 != 
0)goto _LLE8;_LLE7:{void*_tmp87=_tmp54;_LLEF: if((int)_tmp87 != 0)goto _LLF1;_LLF0:
sns=_tag_arr("signed ",sizeof(unsigned char),8);goto _LLEE;_LLF1: if((int)_tmp87 != 
1)goto _LLEE;_LLF2: goto _LLEE;_LLEE:;}ts=_tag_arr("char",sizeof(unsigned char),5);
goto _LLE5;_LLE8: if((int)_tmp86 != 1)goto _LLEA;_LLE9: ts=_tag_arr("short",sizeof(
unsigned char),6);goto _LLE5;_LLEA: if((int)_tmp86 != 2)goto _LLEC;_LLEB: ts=_tag_arr("int",
sizeof(unsigned char),4);goto _LLE5;_LLEC: if((int)_tmp86 != 3)goto _LLE5;_LLED: ts=
Cyc_Absynpp_to_VC?_tag_arr("__int64",sizeof(unsigned char),8): _tag_arr("long long",
sizeof(unsigned char),10);goto _LLE5;_LLE5:;}s=Cyc_PP_text((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp8A;_tmp8A.tag=0;_tmp8A.f1=(struct _tagged_arr)
ts;{struct Cyc_Std_String_pa_struct _tmp89;_tmp89.tag=0;_tmp89.f1=(struct
_tagged_arr)sns;{void*_tmp88[2]={& _tmp89,& _tmp8A};Cyc_Std_aprintf(_tag_arr("%s%s",
sizeof(unsigned char),5),_tag_arr(_tmp88,sizeof(void*),2));}}}));goto _LLA2;}
_LLB5: if((int)_tmp47 != 1)goto _LLB7;_LLB6: s=Cyc_PP_text(_tag_arr("float",sizeof(
unsigned char),6));goto _LLA2;_LLB7: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 6)
goto _LLB9;_tmp56=((struct Cyc_Absyn_DoubleType_struct*)_tmp47)->f1;_LLB8: if(
_tmp56)s=Cyc_PP_text(_tag_arr("long double",sizeof(unsigned char),12));else{s=
Cyc_PP_text(_tag_arr("double",sizeof(unsigned char),7));}goto _LLA2;_LLB9: if(
_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 9)goto _LLBB;_tmp57=((struct Cyc_Absyn_TupleType_struct*)
_tmp47)->f1;_LLBA: s=({struct Cyc_PP_Doc*_tmp8B[2];_tmp8B[1]=Cyc_Absynpp_args2doc(
_tmp57);_tmp8B[0]=Cyc_PP_text(_tag_arr("$",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp8B,sizeof(struct Cyc_PP_Doc*),2));});goto _LLA2;_LLBB: if(_tmp47 <= (
void*)3?1:*((int*)_tmp47)!= 10)goto _LLBD;_tmp58=((struct Cyc_Absyn_AggrType_struct*)
_tmp47)->f1;_tmp59=(void*)_tmp58.aggr_info;_tmp5A=_tmp58.targs;_LLBC: {void*
_tmp8D;struct _tuple0*_tmp8E;struct _tuple3 _tmp8C=Cyc_Absyn_aggr_kinded_name(
_tmp59);_tmp8D=_tmp8C.f1;_tmp8E=_tmp8C.f2;s=({struct Cyc_PP_Doc*_tmp8F[3];_tmp8F[
2]=Cyc_Absynpp_tps2doc(_tmp5A);_tmp8F[1]=Cyc_Absynpp_qvar2doc(_tmp8E);_tmp8F[0]=
Cyc_Absynpp_aggr_kind2doc(_tmp8D);Cyc_PP_cat(_tag_arr(_tmp8F,sizeof(struct Cyc_PP_Doc*),
3));});goto _LLA2;}_LLBD: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 11)goto _LLBF;
_tmp5B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp47)->f1;_tmp5C=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp47)->f2;_LLBE: s=({struct Cyc_PP_Doc*_tmp90[4];
_tmp90[3]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp90[2]=Cyc_PP_nest(
2,Cyc_Absynpp_aggrfields2doc(_tmp5C));_tmp90[1]=Cyc_PP_text(_tag_arr("{",sizeof(
unsigned char),2));_tmp90[0]=Cyc_Absynpp_aggr_kind2doc(_tmp5B);Cyc_PP_cat(
_tag_arr(_tmp90,sizeof(struct Cyc_PP_Doc*),4));});goto _LLA2;_LLBF: if(_tmp47 <= (
void*)3?1:*((int*)_tmp47)!= 13)goto _LLC1;_tmp5D=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp47)->f1;_LLC0: s=({struct Cyc_PP_Doc*_tmp91[3];_tmp91[2]=Cyc_PP_text(_tag_arr("}",
sizeof(unsigned char),2));_tmp91[1]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(
_tmp5D));_tmp91[0]=Cyc_PP_text(_tag_arr("enum {",sizeof(unsigned char),7));Cyc_PP_cat(
_tag_arr(_tmp91,sizeof(struct Cyc_PP_Doc*),3));});goto _LLA2;_LLC1: if(_tmp47 <= (
void*)3?1:*((int*)_tmp47)!= 12)goto _LLC3;_tmp5E=((struct Cyc_Absyn_EnumType_struct*)
_tmp47)->f1;_LLC2: s=({struct Cyc_PP_Doc*_tmp92[2];_tmp92[1]=Cyc_Absynpp_qvar2doc(
_tmp5E);_tmp92[0]=Cyc_PP_text(_tag_arr("enum ",sizeof(unsigned char),6));Cyc_PP_cat(
_tag_arr(_tmp92,sizeof(struct Cyc_PP_Doc*),2));});goto _LLA2;_LLC3: if(_tmp47 <= (
void*)3?1:*((int*)_tmp47)!= 14)goto _LLC5;_tmp5F=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp47)->f1;_LLC4: s=({struct Cyc_PP_Doc*_tmp93[3];_tmp93[2]=Cyc_PP_text(_tag_arr(">",
sizeof(unsigned char),2));_tmp93[1]=Cyc_Absynpp_typ2doc(_tmp5F);_tmp93[0]=Cyc_PP_text(
_tag_arr("sizeof_t<",sizeof(unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp93,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLA2;_LLC5: if(_tmp47 <= (void*)3?1:*((int*)
_tmp47)!= 16)goto _LLC7;_tmp60=((struct Cyc_Absyn_TypedefType_struct*)_tmp47)->f1;
_tmp61=((struct Cyc_Absyn_TypedefType_struct*)_tmp47)->f2;_tmp62=((struct Cyc_Absyn_TypedefType_struct*)
_tmp47)->f3;_LLC6: s=({struct Cyc_PP_Doc*_tmp94[2];_tmp94[1]=Cyc_Absynpp_tps2doc(
_tmp61);_tmp94[0]=Cyc_Absynpp_qvar2doc(_tmp60);Cyc_PP_cat(_tag_arr(_tmp94,
sizeof(struct Cyc_PP_Doc*),2));});goto _LLA2;_LLC7: if(_tmp47 <= (void*)3?1:*((int*)
_tmp47)!= 15)goto _LLC9;_tmp63=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp47)->f1;_LLC8: s=({struct Cyc_PP_Doc*_tmp95[3];_tmp95[2]=Cyc_PP_text(_tag_arr(">",
sizeof(unsigned char),2));_tmp95[1]=Cyc_Absynpp_rgn2doc(_tmp63);_tmp95[0]=Cyc_PP_text(
_tag_arr("region_t<",sizeof(unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp95,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLA2;_LLC9: if((int)_tmp47 != 2)goto _LLCB;
_LLCA: s=Cyc_Absynpp_rgn2doc(t);goto _LLA2;_LLCB: if(_tmp47 <= (void*)3?1:*((int*)
_tmp47)!= 19)goto _LLCD;_tmp64=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp47)->f1;
_LLCC: s=({struct Cyc_PP_Doc*_tmp96[3];_tmp96[2]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp96[1]=Cyc_Absynpp_typ2doc(_tmp64);_tmp96[0]=Cyc_PP_text(
_tag_arr("regions(",sizeof(unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp96,sizeof(
struct Cyc_PP_Doc*),3));});goto _LLA2;_LLCD: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)
!= 17)goto _LLCF;_LLCE: goto _LLD0;_LLCF: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 
18)goto _LLA2;_LLD0: s=Cyc_Absynpp_eff2doc(t);goto _LLA2;_LLA2:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((
struct _tagged_arr*)vo->v));}struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct
_tuple4*cmp){struct _tuple4 _tmp98;void*_tmp99;void*_tmp9A;struct _tuple4*_tmp97=
cmp;_tmp98=*_tmp97;_tmp99=_tmp98.f1;_tmp9A=_tmp98.f2;return({struct Cyc_PP_Doc*
_tmp9B[3];_tmp9B[2]=Cyc_Absynpp_rgn2doc(_tmp9A);_tmp9B[1]=Cyc_PP_text(_tag_arr(" < ",
sizeof(unsigned char),4));_tmp9B[0]=Cyc_Absynpp_rgn2doc(_tmp99);Cyc_PP_cat(
_tag_arr(_tmp9B,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List*po){return Cyc_PP_group(_tag_arr("",sizeof(unsigned char),1),
_tag_arr("",sizeof(unsigned char),1),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple4*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(
struct _tuple1*t){struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:({struct Cyc_Core_Opt*
_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->v=Cyc_PP_text(*((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null((*t).f1))->v));_tmp9C;});return Cyc_Absynpp_tqtd2doc((*
t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp9D=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs)_tmp9D=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9D,({struct Cyc_List_List*
_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->hd=Cyc_PP_text(_tag_arr("...",sizeof(
unsigned char),4));_tmp9E->tl=0;_tmp9E;}));else{if(cyc_varargs != 0){struct Cyc_PP_Doc*
_tmp9F=({struct Cyc_PP_Doc*_tmpA1[3];_tmpA1[2]=Cyc_Absynpp_funarg2doc(({struct
_tuple1*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->f1=cyc_varargs->name;_tmpA2->f2=
cyc_varargs->tq;_tmpA2->f3=(void*)cyc_varargs->type;_tmpA2;}));_tmpA1[1]=
cyc_varargs->inject?Cyc_PP_text(_tag_arr(" inject ",sizeof(unsigned char),9)):
Cyc_PP_text(_tag_arr(" ",sizeof(unsigned char),2));_tmpA1[0]=Cyc_PP_text(
_tag_arr("...",sizeof(unsigned char),4));Cyc_PP_cat(_tag_arr(_tmpA1,sizeof(
struct Cyc_PP_Doc*),3));});_tmp9D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmp9D,({struct Cyc_List_List*_tmpA0=
_cycalloc(sizeof(*_tmpA0));_tmpA0->hd=_tmp9F;_tmpA0->tl=0;_tmpA0;}));}}{struct
Cyc_PP_Doc*_tmpA3=Cyc_PP_group(_tag_arr("",sizeof(unsigned char),1),_tag_arr("",
sizeof(unsigned char),1),_tag_arr(",",sizeof(unsigned char),2),_tmp9D);if(effopt
!= 0)_tmpA3=({struct Cyc_PP_Doc*_tmpA4[3];_tmpA4[2]=Cyc_Absynpp_eff2doc((void*)
effopt->v);_tmpA4[1]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmpA4[0]=
_tmpA3;Cyc_PP_cat(_tag_arr(_tmpA4,sizeof(struct Cyc_PP_Doc*),3));});if(rgn_po != 0)
_tmpA3=({struct Cyc_PP_Doc*_tmpA5[3];_tmpA5[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);
_tmpA5[1]=Cyc_PP_text(_tag_arr(":",sizeof(unsigned char),2));_tmpA5[0]=_tmpA3;
Cyc_PP_cat(_tag_arr(_tmpA5,sizeof(struct Cyc_PP_Doc*),3));});return({struct Cyc_PP_Doc*
_tmpA6[3];_tmpA6[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmpA6[1]=
_tmpA3;_tmpA6[0]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmpA6,sizeof(struct Cyc_PP_Doc*),3));});}}struct _tuple1*Cyc_Absynpp_arg_mk_opt(
struct _tuple5*arg){return({struct _tuple1*_tmpA7=_cycalloc(sizeof(*_tmpA7));
_tmpA7->f1=({struct Cyc_Core_Opt*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->v=(*
arg).f1;_tmpA8;});_tmpA7->f2=(*arg).f2;_tmpA7->f3=(*arg).f3;_tmpA7;});}struct Cyc_PP_Doc*
Cyc_Absynpp_var2doc(struct _tagged_arr*v){return Cyc_PP_text(*v);}struct Cyc_PP_Doc*
Cyc_Absynpp_qvar2doc(struct _tuple0*q){struct Cyc_List_List*_tmpA9=0;int match;{
void*_tmpAA=(*q).f1;struct Cyc_List_List*_tmpAB;struct Cyc_List_List*_tmpAC;_LLF4:
if((int)_tmpAA != 0)goto _LLF6;_LLF5: _tmpAB=0;goto _LLF7;_LLF6: if(_tmpAA <= (void*)1?
1:*((int*)_tmpAA)!= 0)goto _LLF8;_tmpAB=((struct Cyc_Absyn_Rel_n_struct*)_tmpAA)->f1;
_LLF7: match=0;_tmpA9=_tmpAB;goto _LLF3;_LLF8: if(_tmpAA <= (void*)1?1:*((int*)
_tmpAA)!= 1)goto _LLF3;_tmpAC=((struct Cyc_Absyn_Abs_n_struct*)_tmpAA)->f1;_LLF9:
match=Cyc_Absynpp_use_curr_namespace?((int(*)(int(*cmp)(struct _tagged_arr*,
struct _tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(
Cyc_Std_strptrcmp,_tmpAC,Cyc_Absynpp_curr_namespace): 0;_tmpA9=(Cyc_Absynpp_qvar_to_Cids?
Cyc_Absynpp_add_cyc_prefix: 0)?({struct Cyc_List_List*_tmpAD=_cycalloc(sizeof(*
_tmpAD));_tmpAD->hd=Cyc_Absynpp_cyc_stringptr;_tmpAD->tl=_tmpAC;_tmpAD;}): _tmpAC;
goto _LLF3;_LLF3:;}if(Cyc_Absynpp_qvar_to_Cids)return Cyc_PP_text((struct
_tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmpA9,({struct Cyc_List_List*_tmpAE=
_cycalloc(sizeof(*_tmpAE));_tmpAE->hd=(*q).f2;_tmpAE->tl=0;_tmpAE;})),_tag_arr("_",
sizeof(unsigned char),2)));else{if(match)return Cyc_Absynpp_var2doc((*q).f2);
else{return Cyc_PP_text((struct _tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA9,({struct Cyc_List_List*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->hd=(*q).f2;_tmpAF->tl=0;_tmpAF;})),
_tag_arr("::",sizeof(unsigned char),3)));}}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2doc(v);if(
Cyc_Absynpp_use_curr_namespace){void*_tmpB0=(*v).f1;struct Cyc_List_List*_tmpB1;
struct Cyc_List_List*_tmpB2;_LLFB: if((int)_tmpB0 != 0)goto _LLFD;_LLFC: goto _LLFE;
_LLFD: if(_tmpB0 <= (void*)1?1:*((int*)_tmpB0)!= 0)goto _LLFF;_tmpB1=((struct Cyc_Absyn_Rel_n_struct*)
_tmpB0)->f1;if(_tmpB1 != 0)goto _LLFF;_LLFE: return Cyc_Absynpp_var2doc((*v).f2);
_LLFF: if(_tmpB0 <= (void*)1?1:*((int*)_tmpB0)!= 1)goto _LL101;_tmpB2=((struct Cyc_Absyn_Abs_n_struct*)
_tmpB0)->f1;_LL100: if(((int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Std_strptrcmp,
_tmpB2,Cyc_Absynpp_curr_namespace)== 0)return Cyc_Absynpp_var2doc((*v).f2);else{
goto _LL102;}_LL101:;_LL102: return({struct Cyc_PP_Doc*_tmpB3[2];_tmpB3[1]=Cyc_Absynpp_qvar2doc(
v);_tmpB3[0]=Cyc_PP_text(_tag_arr("/* bad namespace : */ ",sizeof(unsigned char),
23));Cyc_PP_cat(_tag_arr(_tmpB3,sizeof(struct Cyc_PP_Doc*),2));});_LLFA:;}else{
return Cyc_Absynpp_var2doc((*v).f2);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(),t,0);}int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*e){void*_tmpB4=(void*)e->r;void*_tmpB5;struct Cyc_Absyn_Exp*
_tmpB6;struct Cyc_Absyn_Exp*_tmpB7;_LL104: if(*((int*)_tmpB4)!= 0)goto _LL106;
_LL105: goto _LL107;_LL106: if(*((int*)_tmpB4)!= 1)goto _LL108;_LL107: goto _LL109;
_LL108: if(*((int*)_tmpB4)!= 2)goto _LL10A;_LL109: return 10000;_LL10A: if(*((int*)
_tmpB4)!= 3)goto _LL10C;_tmpB5=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmpB4)->f1;
_LL10B: {void*_tmpB8=_tmpB5;_LL151: if((int)_tmpB8 != 0)goto _LL153;_LL152: return
100;_LL153: if((int)_tmpB8 != 1)goto _LL155;_LL154: return 110;_LL155: if((int)_tmpB8
!= 2)goto _LL157;_LL156: return 100;_LL157: if((int)_tmpB8 != 3)goto _LL159;_LL158:
goto _LL15A;_LL159: if((int)_tmpB8 != 4)goto _LL15B;_LL15A: return 110;_LL15B: if((int)
_tmpB8 != 5)goto _LL15D;_LL15C: goto _LL15E;_LL15D: if((int)_tmpB8 != 6)goto _LL15F;
_LL15E: return 70;_LL15F: if((int)_tmpB8 != 7)goto _LL161;_LL160: goto _LL162;_LL161:
if((int)_tmpB8 != 8)goto _LL163;_LL162: goto _LL164;_LL163: if((int)_tmpB8 != 9)goto
_LL165;_LL164: goto _LL166;_LL165: if((int)_tmpB8 != 10)goto _LL167;_LL166: return 80;
_LL167: if((int)_tmpB8 != 11)goto _LL169;_LL168: goto _LL16A;_LL169: if((int)_tmpB8 != 
12)goto _LL16B;_LL16A: return 130;_LL16B: if((int)_tmpB8 != 13)goto _LL16D;_LL16C:
return 60;_LL16D: if((int)_tmpB8 != 14)goto _LL16F;_LL16E: return 40;_LL16F: if((int)
_tmpB8 != 15)goto _LL171;_LL170: return 50;_LL171: if((int)_tmpB8 != 16)goto _LL173;
_LL172: return 90;_LL173: if((int)_tmpB8 != 17)goto _LL175;_LL174: return 80;_LL175: if((
int)_tmpB8 != 18)goto _LL177;_LL176: return 80;_LL177: if((int)_tmpB8 != 19)goto _LL150;
_LL178: return 140;_LL150:;}_LL10C: if(*((int*)_tmpB4)!= 4)goto _LL10E;_LL10D: return
20;_LL10E: if(*((int*)_tmpB4)!= 5)goto _LL110;_LL10F: return 130;_LL110: if(*((int*)
_tmpB4)!= 6)goto _LL112;_LL111: return 30;_LL112: if(*((int*)_tmpB4)!= 7)goto _LL114;
_LL113: return 10;_LL114: if(*((int*)_tmpB4)!= 8)goto _LL116;_LL115: goto _LL117;
_LL116: if(*((int*)_tmpB4)!= 9)goto _LL118;_LL117: return 140;_LL118: if(*((int*)
_tmpB4)!= 10)goto _LL11A;_LL119: return 130;_LL11A: if(*((int*)_tmpB4)!= 11)goto
_LL11C;_tmpB6=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpB4)->f1;_LL11B:
return Cyc_Absynpp_exp_prec(_tmpB6);_LL11C: if(*((int*)_tmpB4)!= 12)goto _LL11E;
_tmpB7=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpB4)->f1;_LL11D: return Cyc_Absynpp_exp_prec(
_tmpB7);_LL11E: if(*((int*)_tmpB4)!= 13)goto _LL120;_LL11F: return 120;_LL120: if(*((
int*)_tmpB4)!= 15)goto _LL122;_LL121: return 15;_LL122: if(*((int*)_tmpB4)!= 14)goto
_LL124;_LL123: goto _LL125;_LL124: if(*((int*)_tmpB4)!= 16)goto _LL126;_LL125: goto
_LL127;_LL126: if(*((int*)_tmpB4)!= 17)goto _LL128;_LL127: goto _LL129;_LL128: if(*((
int*)_tmpB4)!= 18)goto _LL12A;_LL129: goto _LL12B;_LL12A: if(*((int*)_tmpB4)!= 19)
goto _LL12C;_LL12B: goto _LL12D;_LL12C: if(*((int*)_tmpB4)!= 20)goto _LL12E;_LL12D:
return 130;_LL12E: if(*((int*)_tmpB4)!= 21)goto _LL130;_LL12F: goto _LL131;_LL130: if(*((
int*)_tmpB4)!= 22)goto _LL132;_LL131: goto _LL133;_LL132: if(*((int*)_tmpB4)!= 23)
goto _LL134;_LL133: return 140;_LL134: if(*((int*)_tmpB4)!= 24)goto _LL136;_LL135:
return 150;_LL136: if(*((int*)_tmpB4)!= 25)goto _LL138;_LL137: goto _LL139;_LL138: if(*((
int*)_tmpB4)!= 26)goto _LL13A;_LL139: goto _LL13B;_LL13A: if(*((int*)_tmpB4)!= 27)
goto _LL13C;_LL13B: goto _LL13D;_LL13C: if(*((int*)_tmpB4)!= 28)goto _LL13E;_LL13D:
goto _LL13F;_LL13E: if(*((int*)_tmpB4)!= 29)goto _LL140;_LL13F: goto _LL141;_LL140:
if(*((int*)_tmpB4)!= 30)goto _LL142;_LL141: goto _LL143;_LL142: if(*((int*)_tmpB4)!= 
31)goto _LL144;_LL143: goto _LL145;_LL144: if(*((int*)_tmpB4)!= 32)goto _LL146;_LL145:
goto _LL147;_LL146: if(*((int*)_tmpB4)!= 33)goto _LL148;_LL147: goto _LL149;_LL148:
if(*((int*)_tmpB4)!= 34)goto _LL14A;_LL149: return 140;_LL14A: if(*((int*)_tmpB4)!= 
35)goto _LL14C;_LL14B: return 10000;_LL14C: if(*((int*)_tmpB4)!= 36)goto _LL14E;
_LL14D: goto _LL14F;_LL14E: if(*((int*)_tmpB4)!= 37)goto _LL103;_LL14F: return 140;
_LL103:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(
0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*s;{void*_tmpB9=(void*)e->r;
void*_tmpBA;struct _tuple0*_tmpBB;struct _tuple0*_tmpBC;void*_tmpBD;struct Cyc_List_List*
_tmpBE;struct Cyc_Absyn_Exp*_tmpBF;struct Cyc_Core_Opt*_tmpC0;struct Cyc_Absyn_Exp*
_tmpC1;struct Cyc_Absyn_Exp*_tmpC2;void*_tmpC3;struct Cyc_Absyn_Exp*_tmpC4;struct
Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC7;
struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_List_List*_tmpCA;
struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_List_List*_tmpCC;struct Cyc_Absyn_Exp*_tmpCD;
struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;void*_tmpD0;struct Cyc_Absyn_Exp*
_tmpD1;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*
_tmpD4;void*_tmpD5;struct Cyc_Absyn_Exp*_tmpD6;void*_tmpD7;void*_tmpD8;struct
_tagged_arr*_tmpD9;void*_tmpDA;void*_tmpDB;unsigned int _tmpDC;struct Cyc_List_List*
_tmpDD;void*_tmpDE;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct
_tagged_arr*_tmpE1;struct Cyc_Absyn_Exp*_tmpE2;struct _tagged_arr*_tmpE3;struct Cyc_Absyn_Exp*
_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_List_List*_tmpE6;struct _tuple1*
_tmpE7;struct Cyc_List_List*_tmpE8;struct Cyc_List_List*_tmpE9;struct Cyc_Absyn_Vardecl*
_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEC;struct _tuple0*
_tmpED;struct Cyc_List_List*_tmpEE;struct Cyc_List_List*_tmpEF;struct Cyc_List_List*
_tmpF0;struct Cyc_List_List*_tmpF1;struct Cyc_Absyn_Tunionfield*_tmpF2;struct
_tuple0*_tmpF3;struct _tuple0*_tmpF4;struct Cyc_Absyn_MallocInfo _tmpF5;int _tmpF6;
struct Cyc_Absyn_Exp*_tmpF7;void**_tmpF8;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Core_Opt*
_tmpFA;struct Cyc_List_List*_tmpFB;struct Cyc_Absyn_Stmt*_tmpFC;struct Cyc_Absyn_Fndecl*
_tmpFD;struct Cyc_Absyn_Exp*_tmpFE;_LL17A: if(*((int*)_tmpB9)!= 0)goto _LL17C;
_tmpBA=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpB9)->f1;_LL17B: s=Cyc_Absynpp_cnst2doc(
_tmpBA);goto _LL179;_LL17C: if(*((int*)_tmpB9)!= 1)goto _LL17E;_tmpBB=((struct Cyc_Absyn_Var_e_struct*)
_tmpB9)->f1;_LL17D: _tmpBC=_tmpBB;goto _LL17F;_LL17E: if(*((int*)_tmpB9)!= 2)goto
_LL180;_tmpBC=((struct Cyc_Absyn_UnknownId_e_struct*)_tmpB9)->f1;_LL17F: s=Cyc_Absynpp_qvar2doc(
_tmpBC);goto _LL179;_LL180: if(*((int*)_tmpB9)!= 3)goto _LL182;_tmpBD=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpB9)->f1;_tmpBE=((struct Cyc_Absyn_Primop_e_struct*)
_tmpB9)->f2;_LL181: s=Cyc_Absynpp_primapp2doc(myprec,_tmpBD,_tmpBE);goto _LL179;
_LL182: if(*((int*)_tmpB9)!= 4)goto _LL184;_tmpBF=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpB9)->f1;_tmpC0=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpB9)->f2;_tmpC1=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpB9)->f3;_LL183: s=({struct Cyc_PP_Doc*
_tmpFF[5];_tmpFF[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC1);_tmpFF[3]=Cyc_PP_text(
_tag_arr("= ",sizeof(unsigned char),3));_tmpFF[2]=_tmpC0 == 0?Cyc_PP_text(
_tag_arr("",sizeof(unsigned char),1)): Cyc_Absynpp_prim2doc((void*)_tmpC0->v);
_tmpFF[1]=Cyc_PP_text(_tag_arr(" ",sizeof(unsigned char),2));_tmpFF[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpBF);Cyc_PP_cat(_tag_arr(_tmpFF,sizeof(struct Cyc_PP_Doc*),5));});goto
_LL179;_LL184: if(*((int*)_tmpB9)!= 5)goto _LL186;_tmpC2=((struct Cyc_Absyn_Increment_e_struct*)
_tmpB9)->f1;_tmpC3=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpB9)->f2;
_LL185: {struct Cyc_PP_Doc*_tmp100=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC2);{void*
_tmp101=_tmpC3;_LL1C9: if((int)_tmp101 != 0)goto _LL1CB;_LL1CA: s=({struct Cyc_PP_Doc*
_tmp102[2];_tmp102[1]=_tmp100;_tmp102[0]=Cyc_PP_text(_tag_arr("++",sizeof(
unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp102,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL1C8;_LL1CB: if((int)_tmp101 != 2)goto _LL1CD;_LL1CC: s=({struct Cyc_PP_Doc*
_tmp103[2];_tmp103[1]=_tmp100;_tmp103[0]=Cyc_PP_text(_tag_arr("--",sizeof(
unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp103,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL1C8;_LL1CD: if((int)_tmp101 != 1)goto _LL1CF;_LL1CE: s=({struct Cyc_PP_Doc*
_tmp104[2];_tmp104[1]=Cyc_PP_text(_tag_arr("++",sizeof(unsigned char),3));
_tmp104[0]=_tmp100;Cyc_PP_cat(_tag_arr(_tmp104,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL1C8;_LL1CF: if((int)_tmp101 != 3)goto _LL1C8;_LL1D0: s=({struct Cyc_PP_Doc*
_tmp105[2];_tmp105[1]=Cyc_PP_text(_tag_arr("--",sizeof(unsigned char),3));
_tmp105[0]=_tmp100;Cyc_PP_cat(_tag_arr(_tmp105,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL1C8;_LL1C8:;}goto _LL179;}_LL186: if(*((int*)_tmpB9)!= 6)goto _LL188;_tmpC4=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpB9)->f1;_tmpC5=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB9)->f2;_tmpC6=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB9)->f3;_LL187:{
struct _tuple4 _tmp107=({struct _tuple4 _tmp106;_tmp106.f1=(void*)_tmpC5->r;_tmp106.f2=(
void*)_tmpC6->r;_tmp106;});_LL1D2:;_LL1D3: s=({struct Cyc_PP_Doc*_tmp108[5];
_tmp108[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC6);_tmp108[3]=Cyc_PP_text(
_tag_arr(" : ",sizeof(unsigned char),4));_tmp108[2]=Cyc_Absynpp_exp2doc_prec(0,
_tmpC5);_tmp108[1]=Cyc_PP_text(_tag_arr(" ? ",sizeof(unsigned char),4));_tmp108[
0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC4);Cyc_PP_cat(_tag_arr(_tmp108,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL1D1;_LL1D1:;}goto _LL179;_LL188: if(*((int*)
_tmpB9)!= 7)goto _LL18A;_tmpC7=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB9)->f1;
_tmpC8=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB9)->f2;_LL189: s=({struct Cyc_PP_Doc*
_tmp109[5];_tmp109[4]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp109[
3]=Cyc_Absynpp_exp2doc(_tmpC8);_tmp109[2]=Cyc_PP_text(_tag_arr(", ",sizeof(
unsigned char),3));_tmp109[1]=Cyc_Absynpp_exp2doc(_tmpC7);_tmp109[0]=Cyc_PP_text(
_tag_arr("(",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp109,sizeof(struct
Cyc_PP_Doc*),5));});goto _LL179;_LL18A: if(*((int*)_tmpB9)!= 8)goto _LL18C;_tmpC9=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmpB9)->f1;_tmpCA=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmpB9)->f2;_LL18B: s=({struct Cyc_PP_Doc*_tmp10A[4];_tmp10A[3]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp10A[2]=Cyc_Absynpp_exps2doc_prec(20,
_tmpCA);_tmp10A[1]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));_tmp10A[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmpC9);Cyc_PP_cat(_tag_arr(_tmp10A,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL179;_LL18C: if(*((int*)_tmpB9)!= 9)goto _LL18E;
_tmpCB=((struct Cyc_Absyn_FnCall_e_struct*)_tmpB9)->f1;_tmpCC=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpB9)->f2;_LL18D: s=({struct Cyc_PP_Doc*_tmp10B[4];_tmp10B[3]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp10B[2]=Cyc_Absynpp_exps2doc_prec(20,
_tmpCC);_tmp10B[1]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));_tmp10B[0]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmpCB);Cyc_PP_cat(_tag_arr(_tmp10B,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL179;_LL18E: if(*((int*)_tmpB9)!= 10)goto _LL190;
_tmpCD=((struct Cyc_Absyn_Throw_e_struct*)_tmpB9)->f1;_LL18F: s=({struct Cyc_PP_Doc*
_tmp10C[2];_tmp10C[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpCD);_tmp10C[0]=Cyc_PP_text(
_tag_arr("throw ",sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(_tmp10C,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL179;_LL190: if(*((int*)_tmpB9)!= 11)goto _LL192;
_tmpCE=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpB9)->f1;_LL191: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmpCE);goto _LL179;_LL192: if(*((int*)_tmpB9)!= 12)goto _LL194;_tmpCF=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmpB9)->f1;_LL193: s=Cyc_Absynpp_exp2doc_prec(
inprec,_tmpCF);goto _LL179;_LL194: if(*((int*)_tmpB9)!= 13)goto _LL196;_tmpD0=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpB9)->f1;_tmpD1=((struct Cyc_Absyn_Cast_e_struct*)
_tmpB9)->f2;_LL195: s=({struct Cyc_PP_Doc*_tmp10D[4];_tmp10D[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpD1);_tmp10D[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));
_tmp10D[1]=Cyc_Absynpp_typ2doc(_tmpD0);_tmp10D[0]=Cyc_PP_text(_tag_arr("(",
sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp10D,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL179;_LL196: if(*((int*)_tmpB9)!= 14)goto _LL198;_tmpD2=((struct Cyc_Absyn_Address_e_struct*)
_tmpB9)->f1;_LL197: s=({struct Cyc_PP_Doc*_tmp10E[2];_tmp10E[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpD2);_tmp10E[0]=Cyc_PP_text(_tag_arr("&",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp10E,sizeof(struct Cyc_PP_Doc*),2));});goto _LL179;_LL198: if(*((int*)
_tmpB9)!= 15)goto _LL19A;_tmpD3=((struct Cyc_Absyn_New_e_struct*)_tmpB9)->f1;
_tmpD4=((struct Cyc_Absyn_New_e_struct*)_tmpB9)->f2;_LL199: if(_tmpD3 == 0)s=({
struct Cyc_PP_Doc*_tmp10F[2];_tmp10F[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpD4);
_tmp10F[0]=Cyc_PP_text(_tag_arr("new ",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp10F,sizeof(struct Cyc_PP_Doc*),2));});else{s=({struct Cyc_PP_Doc*
_tmp110[4];_tmp110[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpD4);_tmp110[2]=Cyc_PP_text(
_tag_arr(") ",sizeof(unsigned char),3));_tmp110[1]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(_tmpD3));_tmp110[0]=Cyc_PP_text(_tag_arr("rnew(",
sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp110,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL179;_LL19A: if(*((int*)_tmpB9)!= 16)goto _LL19C;_tmpD5=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpB9)->f1;_LL19B: s=({struct Cyc_PP_Doc*
_tmp111[3];_tmp111[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp111[
1]=Cyc_Absynpp_typ2doc(_tmpD5);_tmp111[0]=Cyc_PP_text(_tag_arr("sizeof(",sizeof(
unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp111,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL179;_LL19C: if(*((int*)_tmpB9)!= 17)goto _LL19E;_tmpD6=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpB9)->f1;_LL19D: s=({struct Cyc_PP_Doc*_tmp112[3];_tmp112[2]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp112[1]=Cyc_Absynpp_exp2doc(_tmpD6);
_tmp112[0]=Cyc_PP_text(_tag_arr("sizeof(",sizeof(unsigned char),8));Cyc_PP_cat(
_tag_arr(_tmp112,sizeof(struct Cyc_PP_Doc*),3));});goto _LL179;_LL19E: if(*((int*)
_tmpB9)!= 18)goto _LL1A0;_tmpD7=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpB9)->f1;_tmpD8=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpB9)->f2;if(*((
int*)_tmpD8)!= 0)goto _LL1A0;_tmpD9=((struct Cyc_Absyn_StructField_struct*)_tmpD8)->f1;
_LL19F: s=({struct Cyc_PP_Doc*_tmp113[5];_tmp113[4]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp113[3]=Cyc_PP_textptr(_tmpD9);_tmp113[2]=Cyc_PP_text(
_tag_arr(",",sizeof(unsigned char),2));_tmp113[1]=Cyc_Absynpp_typ2doc(_tmpD7);
_tmp113[0]=Cyc_PP_text(_tag_arr("offsetof(",sizeof(unsigned char),10));Cyc_PP_cat(
_tag_arr(_tmp113,sizeof(struct Cyc_PP_Doc*),5));});goto _LL179;_LL1A0: if(*((int*)
_tmpB9)!= 18)goto _LL1A2;_tmpDA=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpB9)->f1;_tmpDB=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpB9)->f2;if(*((
int*)_tmpDB)!= 1)goto _LL1A2;_tmpDC=((struct Cyc_Absyn_TupleIndex_struct*)_tmpDB)->f1;
_LL1A1: s=({struct Cyc_PP_Doc*_tmp114[5];_tmp114[4]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp114[3]=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp116;_tmp116.tag=1;_tmp116.f1=(int)_tmpDC;{void*_tmp115[1]={& _tmp116};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp115,sizeof(void*),1));}}));
_tmp114[2]=Cyc_PP_text(_tag_arr(",",sizeof(unsigned char),2));_tmp114[1]=Cyc_Absynpp_typ2doc(
_tmpDA);_tmp114[0]=Cyc_PP_text(_tag_arr("offsetof(",sizeof(unsigned char),10));
Cyc_PP_cat(_tag_arr(_tmp114,sizeof(struct Cyc_PP_Doc*),5));});goto _LL179;_LL1A2:
if(*((int*)_tmpB9)!= 19)goto _LL1A4;_tmpDD=((struct Cyc_Absyn_Gentyp_e_struct*)
_tmpB9)->f1;_tmpDE=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmpB9)->f2;_LL1A3:
s=({struct Cyc_PP_Doc*_tmp117[4];_tmp117[3]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp117[2]=Cyc_Absynpp_typ2doc(_tmpDE);_tmp117[1]=Cyc_Absynpp_tvars2doc(
_tmpDD);_tmp117[0]=Cyc_PP_text(_tag_arr("__gen(",sizeof(unsigned char),7));Cyc_PP_cat(
_tag_arr(_tmp117,sizeof(struct Cyc_PP_Doc*),4));});goto _LL179;_LL1A4: if(*((int*)
_tmpB9)!= 20)goto _LL1A6;_tmpDF=((struct Cyc_Absyn_Deref_e_struct*)_tmpB9)->f1;
_LL1A5: s=({struct Cyc_PP_Doc*_tmp118[2];_tmp118[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpDF);_tmp118[0]=Cyc_PP_text(_tag_arr("*",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp118,sizeof(struct Cyc_PP_Doc*),2));});goto _LL179;_LL1A6: if(*((int*)
_tmpB9)!= 21)goto _LL1A8;_tmpE0=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpB9)->f1;
_tmpE1=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpB9)->f2;_LL1A7: s=({struct Cyc_PP_Doc*
_tmp119[3];_tmp119[2]=Cyc_PP_textptr(_tmpE1);_tmp119[1]=Cyc_PP_text(_tag_arr(".",
sizeof(unsigned char),2));_tmp119[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpE0);Cyc_PP_cat(
_tag_arr(_tmp119,sizeof(struct Cyc_PP_Doc*),3));});goto _LL179;_LL1A8: if(*((int*)
_tmpB9)!= 22)goto _LL1AA;_tmpE2=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpB9)->f1;
_tmpE3=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpB9)->f2;_LL1A9: s=({struct Cyc_PP_Doc*
_tmp11A[3];_tmp11A[2]=Cyc_PP_textptr(_tmpE3);_tmp11A[1]=Cyc_PP_text(_tag_arr("->",
sizeof(unsigned char),3));_tmp11A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpE2);Cyc_PP_cat(
_tag_arr(_tmp11A,sizeof(struct Cyc_PP_Doc*),3));});goto _LL179;_LL1AA: if(*((int*)
_tmpB9)!= 23)goto _LL1AC;_tmpE4=((struct Cyc_Absyn_Subscript_e_struct*)_tmpB9)->f1;
_tmpE5=((struct Cyc_Absyn_Subscript_e_struct*)_tmpB9)->f2;_LL1AB: s=({struct Cyc_PP_Doc*
_tmp11B[4];_tmp11B[3]=Cyc_PP_text(_tag_arr("]",sizeof(unsigned char),2));_tmp11B[
2]=Cyc_Absynpp_exp2doc(_tmpE5);_tmp11B[1]=Cyc_PP_text(_tag_arr("[",sizeof(
unsigned char),2));_tmp11B[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpE4);Cyc_PP_cat(
_tag_arr(_tmp11B,sizeof(struct Cyc_PP_Doc*),4));});goto _LL179;_LL1AC: if(*((int*)
_tmpB9)!= 24)goto _LL1AE;_tmpE6=((struct Cyc_Absyn_Tuple_e_struct*)_tmpB9)->f1;
_LL1AD: s=({struct Cyc_PP_Doc*_tmp11C[3];_tmp11C[2]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp11C[1]=Cyc_Absynpp_exps2doc_prec(20,_tmpE6);
_tmp11C[0]=Cyc_PP_text(_tag_arr("$(",sizeof(unsigned char),3));Cyc_PP_cat(
_tag_arr(_tmp11C,sizeof(struct Cyc_PP_Doc*),3));});goto _LL179;_LL1AE: if(*((int*)
_tmpB9)!= 25)goto _LL1B0;_tmpE7=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpB9)->f1;
_tmpE8=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpB9)->f2;_LL1AF: s=({struct Cyc_PP_Doc*
_tmp11D[4];_tmp11D[3]=Cyc_PP_group(_tag_arr("{",sizeof(unsigned char),2),
_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmpE8));_tmp11D[2]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp11D[1]=Cyc_Absynpp_typ2doc((*_tmpE7).f3);_tmp11D[0]=
Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp11D,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL179;_LL1B0: if(*((int*)_tmpB9)!= 26)goto
_LL1B2;_tmpE9=((struct Cyc_Absyn_Array_e_struct*)_tmpB9)->f1;_LL1B1: s=Cyc_PP_group(
_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpE9));goto _LL179;_LL1B2: if(*((int*)_tmpB9)!= 27)goto _LL1B4;_tmpEA=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpB9)->f1;_tmpEB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB9)->f2;_tmpEC=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmpB9)->f3;_LL1B3: s=({struct Cyc_PP_Doc*
_tmp11E[7];_tmp11E[6]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp11E[
5]=Cyc_Absynpp_exp2doc(_tmpEC);_tmp11E[4]=Cyc_PP_text(_tag_arr(" : ",sizeof(
unsigned char),4));_tmp11E[3]=Cyc_Absynpp_exp2doc(_tmpEB);_tmp11E[2]=Cyc_PP_text(
_tag_arr(" < ",sizeof(unsigned char),4));_tmp11E[1]=Cyc_PP_text(*(*_tmpEA->name).f2);
_tmp11E[0]=Cyc_PP_text(_tag_arr("{for ",sizeof(unsigned char),6));Cyc_PP_cat(
_tag_arr(_tmp11E,sizeof(struct Cyc_PP_Doc*),7));});goto _LL179;_LL1B4: if(*((int*)
_tmpB9)!= 28)goto _LL1B6;_tmpED=((struct Cyc_Absyn_Struct_e_struct*)_tmpB9)->f1;
_tmpEE=((struct Cyc_Absyn_Struct_e_struct*)_tmpB9)->f2;_tmpEF=((struct Cyc_Absyn_Struct_e_struct*)
_tmpB9)->f3;_LL1B5: {struct Cyc_List_List*_tmp11F=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpEF);s=({struct Cyc_PP_Doc*_tmp120[2];_tmp120[1]=Cyc_PP_group(_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2),_tmpEE != 0?({struct Cyc_List_List*_tmp121=_cycalloc(
sizeof(*_tmp121));_tmp121->hd=Cyc_Absynpp_tps2doc(_tmpEE);_tmp121->tl=_tmp11F;
_tmp121;}): _tmp11F);_tmp120[0]=Cyc_Absynpp_qvar2doc(_tmpED);Cyc_PP_cat(_tag_arr(
_tmp120,sizeof(struct Cyc_PP_Doc*),2));});goto _LL179;}_LL1B6: if(*((int*)_tmpB9)!= 
29)goto _LL1B8;_tmpF0=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpB9)->f2;_LL1B7: s=
Cyc_PP_group(_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(
unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpF0));goto _LL179;_LL1B8: if(*((int*)_tmpB9)!= 30)goto _LL1BA;_tmpF1=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpB9)->f1;_tmpF2=((struct Cyc_Absyn_Tunion_e_struct*)_tmpB9)->f3;_LL1B9: if(
_tmpF1 == 0)s=Cyc_Absynpp_qvar2doc(_tmpF2->name);else{s=({struct Cyc_PP_Doc*
_tmp122[2];_tmp122[1]=Cyc_PP_egroup(_tag_arr("(",sizeof(unsigned char),2),
_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmpF1));_tmp122[0]=Cyc_Absynpp_qvar2doc(
_tmpF2->name);Cyc_PP_cat(_tag_arr(_tmp122,sizeof(struct Cyc_PP_Doc*),2));});}goto
_LL179;_LL1BA: if(*((int*)_tmpB9)!= 31)goto _LL1BC;_tmpF3=((struct Cyc_Absyn_Enum_e_struct*)
_tmpB9)->f1;_LL1BB: s=Cyc_Absynpp_qvar2doc(_tmpF3);goto _LL179;_LL1BC: if(*((int*)
_tmpB9)!= 32)goto _LL1BE;_tmpF4=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmpB9)->f1;
_LL1BD: s=Cyc_Absynpp_qvar2doc(_tmpF4);goto _LL179;_LL1BE: if(*((int*)_tmpB9)!= 33)
goto _LL1C0;_tmpF5=((struct Cyc_Absyn_Malloc_e_struct*)_tmpB9)->f1;_tmpF6=_tmpF5.is_calloc;
_tmpF7=_tmpF5.rgn;_tmpF8=_tmpF5.elt_type;_tmpF9=_tmpF5.num_elts;_LL1BF: if(_tmpF6){
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmpF8)),0);
if(_tmpF7 == 0)s=({struct Cyc_PP_Doc*_tmp123[5];_tmp123[4]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp123[3]=Cyc_Absynpp_exp2doc(st);_tmp123[2]=Cyc_PP_text(
_tag_arr(",",sizeof(unsigned char),2));_tmp123[1]=Cyc_Absynpp_exp2doc(_tmpF9);
_tmp123[0]=Cyc_PP_text(_tag_arr("calloc(",sizeof(unsigned char),8));Cyc_PP_cat(
_tag_arr(_tmp123,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp124[7];_tmp124[6]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp124[
5]=Cyc_Absynpp_exp2doc(st);_tmp124[4]=Cyc_PP_text(_tag_arr(",",sizeof(
unsigned char),2));_tmp124[3]=Cyc_Absynpp_exp2doc(_tmpF9);_tmp124[2]=Cyc_PP_text(
_tag_arr(",",sizeof(unsigned char),2));_tmp124[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmpF7));_tmp124[0]=Cyc_PP_text(_tag_arr("rcalloc(",sizeof(
unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp124,sizeof(struct Cyc_PP_Doc*),7));});}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmpF8 == 0)new_e=_tmpF9;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmpF8,0),_tmpF9,0);}if(_tmpF7 == 0)s=({struct Cyc_PP_Doc*
_tmp125[3];_tmp125[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp125[
1]=Cyc_Absynpp_exp2doc(new_e);_tmp125[0]=Cyc_PP_text(_tag_arr("malloc(",sizeof(
unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp125,sizeof(struct Cyc_PP_Doc*),3));});
else{s=({struct Cyc_PP_Doc*_tmp126[5];_tmp126[4]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp126[3]=Cyc_Absynpp_exp2doc(new_e);_tmp126[2]=Cyc_PP_text(
_tag_arr(",",sizeof(unsigned char),2));_tmp126[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmpF7));_tmp126[0]=Cyc_PP_text(_tag_arr("rmalloc(",sizeof(
unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp126,sizeof(struct Cyc_PP_Doc*),5));});}}
goto _LL179;_LL1C0: if(*((int*)_tmpB9)!= 34)goto _LL1C2;_tmpFA=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpB9)->f1;_tmpFB=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpB9)->f2;_LL1C1:
s=Cyc_PP_group(_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(
unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpFB));goto _LL179;_LL1C2: if(*((int*)_tmpB9)!= 35)goto _LL1C4;_tmpFC=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpB9)->f1;_LL1C3: s=({struct Cyc_PP_Doc*_tmp127[3];_tmp127[2]=Cyc_PP_text(
_tag_arr(" })",sizeof(unsigned char),4));_tmp127[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmpFC));_tmp127[0]=Cyc_PP_text(_tag_arr("({ ",sizeof(unsigned char),4));Cyc_PP_cat(
_tag_arr(_tmp127,sizeof(struct Cyc_PP_Doc*),3));});goto _LL179;_LL1C4: if(*((int*)
_tmpB9)!= 36)goto _LL1C6;_tmpFD=((struct Cyc_Absyn_Codegen_e_struct*)_tmpB9)->f1;
_LL1C5: s=({struct Cyc_PP_Doc*_tmp128[3];_tmp128[2]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp128[1]=Cyc_PP_nest(2,Cyc_Absynpp_decl2doc(({struct
Cyc_Absyn_Decl*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->r=(void*)((void*)({
struct Cyc_Absyn_Fn_d_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A[0]=({
struct Cyc_Absyn_Fn_d_struct _tmp12B;_tmp12B.tag=1;_tmp12B.f1=_tmpFD;_tmp12B;});
_tmp12A;}));_tmp129->loc=e->loc;_tmp129;})));_tmp128[0]=Cyc_PP_text(_tag_arr("codegen(",
sizeof(unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp128,sizeof(struct Cyc_PP_Doc*),
3));});goto _LL179;_LL1C6: if(*((int*)_tmpB9)!= 37)goto _LL179;_tmpFE=((struct Cyc_Absyn_Fill_e_struct*)
_tmpB9)->f1;_LL1C7: s=({struct Cyc_PP_Doc*_tmp12C[3];_tmp12C[2]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp12C[1]=Cyc_PP_nest(2,Cyc_Absynpp_exp2doc(
_tmpFE));_tmp12C[0]=Cyc_PP_text(_tag_arr("fill(",sizeof(unsigned char),6));Cyc_PP_cat(
_tag_arr(_tmp12C,sizeof(struct Cyc_PP_Doc*),3));});goto _LL179;_LL179:;}if(inprec
>= myprec)s=({struct Cyc_PP_Doc*_tmp12D[3];_tmp12D[2]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp12D[1]=s;_tmp12D[0]=Cyc_PP_text(_tag_arr("(",
sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp12D,sizeof(struct Cyc_PP_Doc*),
3));});return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){void*_tmp12E=
d;struct Cyc_Absyn_Exp*_tmp12F;struct _tagged_arr*_tmp130;_LL1D5: if(*((int*)
_tmp12E)!= 0)goto _LL1D7;_tmp12F=((struct Cyc_Absyn_ArrayElement_struct*)_tmp12E)->f1;
_LL1D6: return({struct Cyc_PP_Doc*_tmp131[3];_tmp131[2]=Cyc_PP_text(_tag_arr("]",
sizeof(unsigned char),2));_tmp131[1]=Cyc_Absynpp_exp2doc(_tmp12F);_tmp131[0]=Cyc_PP_text(
_tag_arr(".[",sizeof(unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp131,sizeof(
struct Cyc_PP_Doc*),3));});_LL1D7: if(*((int*)_tmp12E)!= 1)goto _LL1D4;_tmp130=((
struct Cyc_Absyn_FieldName_struct*)_tmp12E)->f1;_LL1D8: return({struct Cyc_PP_Doc*
_tmp132[2];_tmp132[1]=Cyc_PP_textptr(_tmp130);_tmp132[0]=Cyc_PP_text(_tag_arr(".",
sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp132,sizeof(struct Cyc_PP_Doc*),
2));});_LL1D4:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple8*de){if((*de).f1
== 0)return Cyc_Absynpp_exp2doc((*de).f2);else{return({struct Cyc_PP_Doc*_tmp133[2];
_tmp133[1]=Cyc_Absynpp_exp2doc((*de).f2);_tmp133[0]=Cyc_PP_egroup(_tag_arr("",
sizeof(unsigned char),1),_tag_arr("=",sizeof(unsigned char),2),_tag_arr("=",
sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1));Cyc_PP_cat(
_tag_arr(_tmp133,sizeof(struct Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(
int inprec,struct Cyc_List_List*es){return Cyc_PP_group(_tag_arr("",sizeof(
unsigned char),1),_tag_arr("",sizeof(unsigned char),1),_tag_arr(",",sizeof(
unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),
int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*c){void*_tmp134=c;void*_tmp135;
unsigned char _tmp136;void*_tmp137;short _tmp138;void*_tmp139;int _tmp13A;void*
_tmp13B;int _tmp13C;void*_tmp13D;long long _tmp13E;struct _tagged_arr _tmp13F;struct
_tagged_arr _tmp140;_LL1DA: if(_tmp134 <= (void*)1?1:*((int*)_tmp134)!= 0)goto
_LL1DC;_tmp135=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp134)->f1;_tmp136=((
struct Cyc_Absyn_Char_c_struct*)_tmp134)->f2;_LL1DB: return Cyc_PP_text((struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp142;_tmp142.tag=0;_tmp142.f1=(
struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp136);{void*_tmp141[1]={& _tmp142};
Cyc_Std_aprintf(_tag_arr("'%s'",sizeof(unsigned char),5),_tag_arr(_tmp141,
sizeof(void*),1));}}));_LL1DC: if(_tmp134 <= (void*)1?1:*((int*)_tmp134)!= 1)goto
_LL1DE;_tmp137=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp134)->f1;_tmp138=((
struct Cyc_Absyn_Short_c_struct*)_tmp134)->f2;_LL1DD: return Cyc_PP_text((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp144;_tmp144.tag=1;_tmp144.f1=(int)((
unsigned int)((int)_tmp138));{void*_tmp143[1]={& _tmp144};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp143,sizeof(void*),1));}}));
_LL1DE: if(_tmp134 <= (void*)1?1:*((int*)_tmp134)!= 2)goto _LL1E0;_tmp139=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp134)->f1;if((int)_tmp139 != 0)goto _LL1E0;
_tmp13A=((struct Cyc_Absyn_Int_c_struct*)_tmp134)->f2;_LL1DF: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp146;_tmp146.tag=1;_tmp146.f1=(
int)((unsigned int)_tmp13A);{void*_tmp145[1]={& _tmp146};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(unsigned char),3),_tag_arr(_tmp145,sizeof(void*),1));}}));_LL1E0: if(
_tmp134 <= (void*)1?1:*((int*)_tmp134)!= 2)goto _LL1E2;_tmp13B=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp134)->f1;if((int)_tmp13B != 1)goto _LL1E2;_tmp13C=((struct Cyc_Absyn_Int_c_struct*)
_tmp134)->f2;_LL1E1: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp148;_tmp148.tag=1;_tmp148.f1=(unsigned int)_tmp13C;{void*_tmp147[1]={&
_tmp148};Cyc_Std_aprintf(_tag_arr("%u",sizeof(unsigned char),3),_tag_arr(_tmp147,
sizeof(void*),1));}}));_LL1E2: if(_tmp134 <= (void*)1?1:*((int*)_tmp134)!= 3)goto
_LL1E4;_tmp13D=(void*)((struct Cyc_Absyn_LongLong_c_struct*)_tmp134)->f1;_tmp13E=((
struct Cyc_Absyn_LongLong_c_struct*)_tmp134)->f2;_LL1E3: return Cyc_PP_text(
_tag_arr("<<FIX LONG LONG CONSTANT>>",sizeof(unsigned char),27));_LL1E4: if(
_tmp134 <= (void*)1?1:*((int*)_tmp134)!= 4)goto _LL1E6;_tmp13F=((struct Cyc_Absyn_Float_c_struct*)
_tmp134)->f1;_LL1E5: return Cyc_PP_text(_tmp13F);_LL1E6: if((int)_tmp134 != 0)goto
_LL1E8;_LL1E7: return Cyc_PP_text(_tag_arr("NULL",sizeof(unsigned char),5));_LL1E8:
if(_tmp134 <= (void*)1?1:*((int*)_tmp134)!= 5)goto _LL1D9;_tmp140=((struct Cyc_Absyn_String_c_struct*)
_tmp134)->f1;_LL1E9: return({struct Cyc_PP_Doc*_tmp149[3];_tmp149[2]=Cyc_PP_text(
_tag_arr("\"",sizeof(unsigned char),2));_tmp149[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(
_tmp140));_tmp149[0]=Cyc_PP_text(_tag_arr("\"",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp149,sizeof(struct Cyc_PP_Doc*),3));});_LL1D9:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(
int inprec,void*p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(
p);if(p == (void*)19){if(es == 0?1: es->tl != 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A[0]=({struct Cyc_Core_Failure_struct
_tmp14B;_tmp14B.tag=Cyc_Core_Failure;_tmp14B.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp14D;_tmp14D.tag=0;_tmp14D.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp14C[1]={& _tmp14D};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof(unsigned char),44),_tag_arr(_tmp14C,sizeof(void*),1));}});_tmp14B;});
_tmp14A;}));{struct Cyc_PP_Doc*_tmp14E=Cyc_Absynpp_exp2doc_prec(inprec,(struct Cyc_Absyn_Exp*)
es->hd);return({struct Cyc_PP_Doc*_tmp14F[2];_tmp14F[1]=Cyc_PP_text(_tag_arr(".size",
sizeof(unsigned char),6));_tmp14F[0]=_tmp14E;Cyc_PP_cat(_tag_arr(_tmp14F,sizeof(
struct Cyc_PP_Doc*),2));});}}else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);if(ds == 0)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=({
struct Cyc_Core_Failure_struct _tmp151;_tmp151.tag=Cyc_Core_Failure;_tmp151.f1=(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp153;_tmp153.tag=0;_tmp153.f1=(
struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{void*_tmp152[1]={& _tmp153};Cyc_Std_aprintf(
_tag_arr("Absynpp::primapp2doc: %s with no args",sizeof(unsigned char),38),
_tag_arr(_tmp152,sizeof(void*),1));}});_tmp151;});_tmp150;}));else{if(ds->tl == 0)
return({struct Cyc_PP_Doc*_tmp154[3];_tmp154[2]=(struct Cyc_PP_Doc*)ds->hd;_tmp154[
1]=Cyc_PP_text(_tag_arr(" ",sizeof(unsigned char),2));_tmp154[0]=ps;Cyc_PP_cat(
_tag_arr(_tmp154,sizeof(struct Cyc_PP_Doc*),3));});else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155[0]=({struct Cyc_Core_Failure_struct
_tmp156;_tmp156.tag=Cyc_Core_Failure;_tmp156.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp158;_tmp158.tag=0;_tmp158.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp157[1]={& _tmp158};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof(unsigned char),47),_tag_arr(_tmp157,sizeof(void*),1));}});_tmp156;});
_tmp155;}));else{return({struct Cyc_PP_Doc*_tmp159[5];_tmp159[4]=(struct Cyc_PP_Doc*)((
struct Cyc_List_List*)_check_null(ds->tl))->hd;_tmp159[3]=Cyc_PP_text(_tag_arr(" ",
sizeof(unsigned char),2));_tmp159[2]=ps;_tmp159[1]=Cyc_PP_text(_tag_arr(" ",
sizeof(unsigned char),2));_tmp159[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(
_tag_arr(_tmp159,sizeof(struct Cyc_PP_Doc*),5));});}}}}}struct _tagged_arr Cyc_Absynpp_prim2str(
void*p){void*_tmp15A=p;_LL1EB: if((int)_tmp15A != 0)goto _LL1ED;_LL1EC: return
_tag_arr("+",sizeof(unsigned char),2);_LL1ED: if((int)_tmp15A != 1)goto _LL1EF;
_LL1EE: return _tag_arr("*",sizeof(unsigned char),2);_LL1EF: if((int)_tmp15A != 2)
goto _LL1F1;_LL1F0: return _tag_arr("-",sizeof(unsigned char),2);_LL1F1: if((int)
_tmp15A != 3)goto _LL1F3;_LL1F2: return _tag_arr("/",sizeof(unsigned char),2);_LL1F3:
if((int)_tmp15A != 4)goto _LL1F5;_LL1F4: return _tag_arr("%",sizeof(unsigned char),2);
_LL1F5: if((int)_tmp15A != 5)goto _LL1F7;_LL1F6: return _tag_arr("==",sizeof(
unsigned char),3);_LL1F7: if((int)_tmp15A != 6)goto _LL1F9;_LL1F8: return _tag_arr("!=",
sizeof(unsigned char),3);_LL1F9: if((int)_tmp15A != 7)goto _LL1FB;_LL1FA: return
_tag_arr(">",sizeof(unsigned char),2);_LL1FB: if((int)_tmp15A != 8)goto _LL1FD;
_LL1FC: return _tag_arr("<",sizeof(unsigned char),2);_LL1FD: if((int)_tmp15A != 9)
goto _LL1FF;_LL1FE: return _tag_arr(">=",sizeof(unsigned char),3);_LL1FF: if((int)
_tmp15A != 10)goto _LL201;_LL200: return _tag_arr("<=",sizeof(unsigned char),3);
_LL201: if((int)_tmp15A != 11)goto _LL203;_LL202: return _tag_arr("!",sizeof(
unsigned char),2);_LL203: if((int)_tmp15A != 12)goto _LL205;_LL204: return _tag_arr("~",
sizeof(unsigned char),2);_LL205: if((int)_tmp15A != 13)goto _LL207;_LL206: return
_tag_arr("&",sizeof(unsigned char),2);_LL207: if((int)_tmp15A != 14)goto _LL209;
_LL208: return _tag_arr("|",sizeof(unsigned char),2);_LL209: if((int)_tmp15A != 15)
goto _LL20B;_LL20A: return _tag_arr("^",sizeof(unsigned char),2);_LL20B: if((int)
_tmp15A != 16)goto _LL20D;_LL20C: return _tag_arr("<<",sizeof(unsigned char),3);
_LL20D: if((int)_tmp15A != 17)goto _LL20F;_LL20E: return _tag_arr(">>",sizeof(
unsigned char),3);_LL20F: if((int)_tmp15A != 18)goto _LL211;_LL210: return _tag_arr(">>>",
sizeof(unsigned char),4);_LL211: if((int)_tmp15A != 19)goto _LL1EA;_LL212: return
_tag_arr("size",sizeof(unsigned char),5);_LL1EA:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp15B=(void*)s->r;_LL214: if(_tmp15B <= (void*)1?1:*((
int*)_tmp15B)!= 12)goto _LL216;_LL215: return 1;_LL216:;_LL217: return 0;_LL213:;}
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st){struct Cyc_PP_Doc*
s;{void*_tmp15C=(void*)st->r;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Stmt*
_tmp15E;struct Cyc_Absyn_Stmt*_tmp15F;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_Exp*
_tmp161;struct Cyc_Absyn_Stmt*_tmp162;struct Cyc_Absyn_Stmt*_tmp163;struct _tuple2
_tmp164;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Stmt*_tmp166;struct
_tagged_arr*_tmp167;struct Cyc_Absyn_Exp*_tmp168;struct _tuple2 _tmp169;struct Cyc_Absyn_Exp*
_tmp16A;struct _tuple2 _tmp16B;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_Stmt*
_tmp16D;struct Cyc_Absyn_ForArrayInfo _tmp16E;struct Cyc_List_List*_tmp16F;struct
_tuple2 _tmp170;struct Cyc_Absyn_Exp*_tmp171;struct _tuple2 _tmp172;struct Cyc_Absyn_Exp*
_tmp173;struct Cyc_Absyn_Stmt*_tmp174;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_List_List*
_tmp176;struct Cyc_Absyn_Exp*_tmp177;struct Cyc_List_List*_tmp178;struct Cyc_List_List*
_tmp179;struct Cyc_List_List*_tmp17A;struct Cyc_Absyn_Decl*_tmp17B;struct Cyc_Absyn_Stmt*
_tmp17C;struct Cyc_Absyn_Stmt*_tmp17D;struct Cyc_Absyn_Stmt*_tmp17E;struct
_tagged_arr*_tmp17F;struct Cyc_Absyn_Stmt*_tmp180;struct Cyc_Absyn_Stmt*_tmp181;
struct _tuple2 _tmp182;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Absyn_Stmt*_tmp184;
struct Cyc_List_List*_tmp185;struct Cyc_Absyn_Tvar*_tmp186;struct Cyc_Absyn_Vardecl*
_tmp187;struct Cyc_Absyn_Stmt*_tmp188;_LL219: if((int)_tmp15C != 0)goto _LL21B;
_LL21A: s=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));goto _LL218;_LL21B:
if(_tmp15C <= (void*)1?1:*((int*)_tmp15C)!= 0)goto _LL21D;_tmp15D=((struct Cyc_Absyn_Exp_s_struct*)
_tmp15C)->f1;_LL21C: s=({struct Cyc_PP_Doc*_tmp189[2];_tmp189[1]=Cyc_PP_text(
_tag_arr(";",sizeof(unsigned char),2));_tmp189[0]=Cyc_Absynpp_exp2doc(_tmp15D);
Cyc_PP_cat(_tag_arr(_tmp189,sizeof(struct Cyc_PP_Doc*),2));});goto _LL218;_LL21D:
if(_tmp15C <= (void*)1?1:*((int*)_tmp15C)!= 1)goto _LL21F;_tmp15E=((struct Cyc_Absyn_Seq_s_struct*)
_tmp15C)->f1;_tmp15F=((struct Cyc_Absyn_Seq_s_struct*)_tmp15C)->f2;_LL21E: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp15E))s=({struct Cyc_PP_Doc*_tmp18A[6];_tmp18A[5]=
Cyc_Absynpp_is_declaration(_tmp15F)?({struct Cyc_PP_Doc*_tmp18B[4];_tmp18B[3]=Cyc_PP_line_doc();
_tmp18B[2]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp18B[1]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp15F));_tmp18B[0]=Cyc_PP_text(_tag_arr("{ ",sizeof(
unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp18B,sizeof(struct Cyc_PP_Doc*),4));}):
Cyc_Absynpp_stmt2doc(_tmp15F);_tmp18A[4]=Cyc_PP_line_doc();_tmp18A[3]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp18A[2]=Cyc_PP_line_doc();_tmp18A[1]=
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp15E));_tmp18A[0]=Cyc_PP_text(_tag_arr("{ ",
sizeof(unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp18A,sizeof(struct Cyc_PP_Doc*),
6));});else{if(Cyc_Absynpp_is_declaration(_tmp15F))s=({struct Cyc_PP_Doc*_tmp18C[
6];_tmp18C[5]=Cyc_PP_line_doc();_tmp18C[4]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp18C[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp15F));
_tmp18C[2]=Cyc_PP_text(_tag_arr("{ ",sizeof(unsigned char),3));_tmp18C[1]=Cyc_PP_line_doc();
_tmp18C[0]=Cyc_Absynpp_stmt2doc(_tmp15E);Cyc_PP_cat(_tag_arr(_tmp18C,sizeof(
struct Cyc_PP_Doc*),6));});else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,_tag_arr("",sizeof(unsigned char),1),({struct Cyc_List_List*
_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->hd=_tmp15E;_tmp18D->tl=({struct Cyc_List_List*
_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->hd=_tmp15F;_tmp18E->tl=0;_tmp18E;});
_tmp18D;}));}}}else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,
_tag_arr("",sizeof(unsigned char),1),({struct Cyc_List_List*_tmp18F=_cycalloc(
sizeof(*_tmp18F));_tmp18F->hd=_tmp15E;_tmp18F->tl=({struct Cyc_List_List*_tmp190=
_cycalloc(sizeof(*_tmp190));_tmp190->hd=_tmp15F;_tmp190->tl=0;_tmp190;});_tmp18F;}));}
goto _LL218;_LL21F: if(_tmp15C <= (void*)1?1:*((int*)_tmp15C)!= 2)goto _LL221;
_tmp160=((struct Cyc_Absyn_Return_s_struct*)_tmp15C)->f1;_LL220: if(_tmp160 == 0)s=
Cyc_PP_text(_tag_arr("return;",sizeof(unsigned char),8));else{s=({struct Cyc_PP_Doc*
_tmp191[3];_tmp191[2]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmp191[
1]=_tmp160 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmp160));_tmp191[0]=Cyc_PP_text(_tag_arr("return ",sizeof(
unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp191,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL218;_LL221: if(_tmp15C <= (void*)1?1:*((int*)_tmp15C)!= 3)goto _LL223;
_tmp161=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp15C)->f1;_tmp162=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp15C)->f2;_tmp163=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp15C)->f3;_LL222: {
int print_else;{void*_tmp192=(void*)_tmp163->r;_LL246: if((int)_tmp192 != 0)goto
_LL248;_LL247: print_else=0;goto _LL245;_LL248:;_LL249: print_else=1;goto _LL245;
_LL245:;}s=({struct Cyc_PP_Doc*_tmp193[7];_tmp193[6]=print_else?({struct Cyc_PP_Doc*
_tmp195[5];_tmp195[4]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp195[
3]=Cyc_PP_line_doc();_tmp195[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp196[2];
_tmp196[1]=Cyc_Absynpp_stmt2doc(_tmp163);_tmp196[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp196,sizeof(struct Cyc_PP_Doc*),2));}));_tmp195[1]=Cyc_PP_text(
_tag_arr("else {",sizeof(unsigned char),7));_tmp195[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp195,sizeof(struct Cyc_PP_Doc*),5));}): Cyc_PP_nil_doc();_tmp193[5]=
Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp193[4]=Cyc_PP_line_doc();
_tmp193[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp194[2];_tmp194[1]=Cyc_Absynpp_stmt2doc(
_tmp162);_tmp194[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp194,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp193[2]=Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),
4));_tmp193[1]=Cyc_Absynpp_exp2doc(_tmp161);_tmp193[0]=Cyc_PP_text(_tag_arr("if (",
sizeof(unsigned char),5));Cyc_PP_cat(_tag_arr(_tmp193,sizeof(struct Cyc_PP_Doc*),
7));});goto _LL218;}_LL223: if(_tmp15C <= (void*)1?1:*((int*)_tmp15C)!= 4)goto
_LL225;_tmp164=((struct Cyc_Absyn_While_s_struct*)_tmp15C)->f1;_tmp165=_tmp164.f1;
_tmp166=((struct Cyc_Absyn_While_s_struct*)_tmp15C)->f2;_LL224: s=({struct Cyc_PP_Doc*
_tmp197[6];_tmp197[5]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp197[
4]=Cyc_PP_line_doc();_tmp197[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp198[2];
_tmp198[1]=Cyc_Absynpp_stmt2doc(_tmp166);_tmp198[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp198,sizeof(struct Cyc_PP_Doc*),2));}));_tmp197[2]=Cyc_PP_text(
_tag_arr(") {",sizeof(unsigned char),4));_tmp197[1]=Cyc_Absynpp_exp2doc(_tmp165);
_tmp197[0]=Cyc_PP_text(_tag_arr("while (",sizeof(unsigned char),8));Cyc_PP_cat(
_tag_arr(_tmp197,sizeof(struct Cyc_PP_Doc*),6));});goto _LL218;_LL225: if(_tmp15C <= (
void*)1?1:*((int*)_tmp15C)!= 5)goto _LL227;_LL226: s=Cyc_PP_text(_tag_arr("break;",
sizeof(unsigned char),7));goto _LL218;_LL227: if(_tmp15C <= (void*)1?1:*((int*)
_tmp15C)!= 6)goto _LL229;_LL228: s=Cyc_PP_text(_tag_arr("continue;",sizeof(
unsigned char),10));goto _LL218;_LL229: if(_tmp15C <= (void*)1?1:*((int*)_tmp15C)!= 
7)goto _LL22B;_tmp167=((struct Cyc_Absyn_Goto_s_struct*)_tmp15C)->f1;_LL22A: s=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp19A;_tmp19A.tag=0;_tmp19A.f1=(
struct _tagged_arr)*_tmp167;{void*_tmp199[1]={& _tmp19A};Cyc_Std_aprintf(_tag_arr("goto %s;",
sizeof(unsigned char),9),_tag_arr(_tmp199,sizeof(void*),1));}}));goto _LL218;
_LL22B: if(_tmp15C <= (void*)1?1:*((int*)_tmp15C)!= 8)goto _LL22D;_tmp168=((struct
Cyc_Absyn_For_s_struct*)_tmp15C)->f1;_tmp169=((struct Cyc_Absyn_For_s_struct*)
_tmp15C)->f2;_tmp16A=_tmp169.f1;_tmp16B=((struct Cyc_Absyn_For_s_struct*)_tmp15C)->f3;
_tmp16C=_tmp16B.f1;_tmp16D=((struct Cyc_Absyn_For_s_struct*)_tmp15C)->f4;_LL22C: s=({
struct Cyc_PP_Doc*_tmp19B[10];_tmp19B[9]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp19B[8]=Cyc_PP_line_doc();_tmp19B[7]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp19C[2];_tmp19C[1]=Cyc_Absynpp_stmt2doc(_tmp16D);_tmp19C[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp19C,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp19B[6]=Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),4));_tmp19B[5]=Cyc_Absynpp_exp2doc(
_tmp16C);_tmp19B[4]=Cyc_PP_text(_tag_arr("; ",sizeof(unsigned char),3));_tmp19B[
3]=Cyc_Absynpp_exp2doc(_tmp16A);_tmp19B[2]=Cyc_PP_text(_tag_arr("; ",sizeof(
unsigned char),3));_tmp19B[1]=Cyc_Absynpp_exp2doc(_tmp168);_tmp19B[0]=Cyc_PP_text(
_tag_arr("for(",sizeof(unsigned char),5));Cyc_PP_cat(_tag_arr(_tmp19B,sizeof(
struct Cyc_PP_Doc*),10));});goto _LL218;_LL22D: if(_tmp15C <= (void*)1?1:*((int*)
_tmp15C)!= 19)goto _LL22F;_tmp16E=((struct Cyc_Absyn_ForArray_s_struct*)_tmp15C)->f1;
_tmp16F=_tmp16E.defns;_tmp170=_tmp16E.condition;_tmp171=_tmp170.f1;_tmp172=
_tmp16E.delta;_tmp173=_tmp172.f1;_tmp174=_tmp16E.body;_LL22E: s=({struct Cyc_PP_Doc*
_tmp19D[9];_tmp19D[8]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp19D[
7]=Cyc_PP_line_doc();_tmp19D[6]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp19E[2];
_tmp19E[1]=Cyc_Absynpp_stmt2doc(_tmp174);_tmp19E[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp19E,sizeof(struct Cyc_PP_Doc*),2));}));_tmp19D[5]=Cyc_PP_text(
_tag_arr(") {",sizeof(unsigned char),4));_tmp19D[4]=Cyc_Absynpp_exp2doc(_tmp173);
_tmp19D[3]=Cyc_PP_text(_tag_arr("; ",sizeof(unsigned char),3));_tmp19D[2]=Cyc_Absynpp_exp2doc(
_tmp171);_tmp19D[1]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_vardecl2doc,
_tag_arr(" ",sizeof(unsigned char),2),_tmp16F);_tmp19D[0]=Cyc_PP_text(_tag_arr("forarray(",
sizeof(unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp19D,sizeof(struct Cyc_PP_Doc*),
9));});goto _LL218;_LL22F: if(_tmp15C <= (void*)1?1:*((int*)_tmp15C)!= 9)goto _LL231;
_tmp175=((struct Cyc_Absyn_Switch_s_struct*)_tmp15C)->f1;_tmp176=((struct Cyc_Absyn_Switch_s_struct*)
_tmp15C)->f2;_LL230: s=({struct Cyc_PP_Doc*_tmp19F[7];_tmp19F[6]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp19F[5]=Cyc_PP_line_doc();_tmp19F[4]=
Cyc_Absynpp_switchclauses2doc(_tmp176);_tmp19F[3]=Cyc_PP_line_doc();_tmp19F[2]=
Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),4));_tmp19F[1]=Cyc_Absynpp_exp2doc(
_tmp175);_tmp19F[0]=Cyc_PP_text(_tag_arr("switch (",sizeof(unsigned char),9));
Cyc_PP_cat(_tag_arr(_tmp19F,sizeof(struct Cyc_PP_Doc*),7));});goto _LL218;_LL231:
if(_tmp15C <= (void*)1?1:*((int*)_tmp15C)!= 10)goto _LL233;_tmp177=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmp15C)->f1;_tmp178=((struct Cyc_Absyn_SwitchC_s_struct*)_tmp15C)->f2;_LL232: s=({
struct Cyc_PP_Doc*_tmp1A0[7];_tmp1A0[6]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp1A0[5]=Cyc_PP_line_doc();_tmp1A0[4]=Cyc_Absynpp_switchCclauses2doc(
_tmp178);_tmp1A0[3]=Cyc_PP_line_doc();_tmp1A0[2]=Cyc_PP_text(_tag_arr(") {",
sizeof(unsigned char),4));_tmp1A0[1]=Cyc_Absynpp_exp2doc(_tmp177);_tmp1A0[0]=Cyc_PP_text(
_tag_arr("switch \"C\" (",sizeof(unsigned char),13));Cyc_PP_cat(_tag_arr(_tmp1A0,
sizeof(struct Cyc_PP_Doc*),7));});goto _LL218;_LL233: if(_tmp15C <= (void*)1?1:*((
int*)_tmp15C)!= 11)goto _LL235;_tmp179=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp15C)->f1;if(_tmp179 != 0)goto _LL235;_LL234: s=Cyc_PP_text(_tag_arr("fallthru;",
sizeof(unsigned char),10));goto _LL218;_LL235: if(_tmp15C <= (void*)1?1:*((int*)
_tmp15C)!= 11)goto _LL237;_tmp17A=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp15C)->f1;
_LL236: s=({struct Cyc_PP_Doc*_tmp1A1[3];_tmp1A1[2]=Cyc_PP_text(_tag_arr(");",
sizeof(unsigned char),3));_tmp1A1[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp17A);
_tmp1A1[0]=Cyc_PP_text(_tag_arr("fallthru(",sizeof(unsigned char),10));Cyc_PP_cat(
_tag_arr(_tmp1A1,sizeof(struct Cyc_PP_Doc*),3));});goto _LL218;_LL237: if(_tmp15C <= (
void*)1?1:*((int*)_tmp15C)!= 12)goto _LL239;_tmp17B=((struct Cyc_Absyn_Decl_s_struct*)
_tmp15C)->f1;_tmp17C=((struct Cyc_Absyn_Decl_s_struct*)_tmp15C)->f2;_LL238: s=({
struct Cyc_PP_Doc*_tmp1A2[3];_tmp1A2[2]=Cyc_Absynpp_stmt2doc(_tmp17C);_tmp1A2[1]=
Cyc_PP_line_doc();_tmp1A2[0]=Cyc_Absynpp_decl2doc(_tmp17B);Cyc_PP_cat(_tag_arr(
_tmp1A2,sizeof(struct Cyc_PP_Doc*),3));});goto _LL218;_LL239: if(_tmp15C <= (void*)1?
1:*((int*)_tmp15C)!= 13)goto _LL23B;_tmp17D=((struct Cyc_Absyn_Cut_s_struct*)
_tmp15C)->f1;_LL23A: s=({struct Cyc_PP_Doc*_tmp1A3[2];_tmp1A3[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp17D));_tmp1A3[0]=Cyc_PP_text(_tag_arr("cut ",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp1A3,sizeof(struct Cyc_PP_Doc*),2));});goto _LL218;_LL23B: if(_tmp15C <= (
void*)1?1:*((int*)_tmp15C)!= 14)goto _LL23D;_tmp17E=((struct Cyc_Absyn_Splice_s_struct*)
_tmp15C)->f1;_LL23C: s=({struct Cyc_PP_Doc*_tmp1A4[2];_tmp1A4[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp17E));_tmp1A4[0]=Cyc_PP_text(_tag_arr("splice ",sizeof(unsigned char),8));
Cyc_PP_cat(_tag_arr(_tmp1A4,sizeof(struct Cyc_PP_Doc*),2));});goto _LL218;_LL23D:
if(_tmp15C <= (void*)1?1:*((int*)_tmp15C)!= 15)goto _LL23F;_tmp17F=((struct Cyc_Absyn_Label_s_struct*)
_tmp15C)->f1;_tmp180=((struct Cyc_Absyn_Label_s_struct*)_tmp15C)->f2;_LL23E: if(
Cyc_Absynpp_decls_first?Cyc_Absynpp_is_declaration(_tmp180): 0)s=({struct Cyc_PP_Doc*
_tmp1A5[6];_tmp1A5[5]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp1A5[
4]=Cyc_PP_line_doc();_tmp1A5[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp180));
_tmp1A5[2]=Cyc_PP_line_doc();_tmp1A5[1]=Cyc_PP_text(_tag_arr(": {",sizeof(
unsigned char),4));_tmp1A5[0]=Cyc_PP_textptr(_tmp17F);Cyc_PP_cat(_tag_arr(
_tmp1A5,sizeof(struct Cyc_PP_Doc*),6));});else{s=({struct Cyc_PP_Doc*_tmp1A6[3];
_tmp1A6[2]=Cyc_Absynpp_stmt2doc(_tmp180);_tmp1A6[1]=Cyc_PP_text(_tag_arr(": ",
sizeof(unsigned char),3));_tmp1A6[0]=Cyc_PP_textptr(_tmp17F);Cyc_PP_cat(_tag_arr(
_tmp1A6,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL218;_LL23F: if(_tmp15C <= (void*)
1?1:*((int*)_tmp15C)!= 16)goto _LL241;_tmp181=((struct Cyc_Absyn_Do_s_struct*)
_tmp15C)->f1;_tmp182=((struct Cyc_Absyn_Do_s_struct*)_tmp15C)->f2;_tmp183=_tmp182.f1;
_LL240: s=({struct Cyc_PP_Doc*_tmp1A7[7];_tmp1A7[6]=Cyc_PP_text(_tag_arr(");",
sizeof(unsigned char),3));_tmp1A7[5]=Cyc_Absynpp_exp2doc(_tmp183);_tmp1A7[4]=Cyc_PP_text(
_tag_arr("} while (",sizeof(unsigned char),10));_tmp1A7[3]=Cyc_PP_line_doc();
_tmp1A7[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp181));_tmp1A7[1]=Cyc_PP_line_doc();
_tmp1A7[0]=Cyc_PP_text(_tag_arr("do {",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp1A7,sizeof(struct Cyc_PP_Doc*),7));});goto _LL218;_LL241: if(_tmp15C <= (
void*)1?1:*((int*)_tmp15C)!= 17)goto _LL243;_tmp184=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp15C)->f1;_tmp185=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp15C)->f2;_LL242: s=({
struct Cyc_PP_Doc*_tmp1A8[9];_tmp1A8[8]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp1A8[7]=Cyc_PP_line_doc();_tmp1A8[6]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp185));_tmp1A8[5]=Cyc_PP_line_doc();_tmp1A8[4]=Cyc_PP_text(_tag_arr("} catch {",
sizeof(unsigned char),10));_tmp1A8[3]=Cyc_PP_line_doc();_tmp1A8[2]=Cyc_PP_nest(2,
Cyc_Absynpp_stmt2doc(_tmp184));_tmp1A8[1]=Cyc_PP_line_doc();_tmp1A8[0]=Cyc_PP_text(
_tag_arr("try {",sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp1A8,sizeof(
struct Cyc_PP_Doc*),9));});goto _LL218;_LL243: if(_tmp15C <= (void*)1?1:*((int*)
_tmp15C)!= 18)goto _LL218;_tmp186=((struct Cyc_Absyn_Region_s_struct*)_tmp15C)->f1;
_tmp187=((struct Cyc_Absyn_Region_s_struct*)_tmp15C)->f2;_tmp188=((struct Cyc_Absyn_Region_s_struct*)
_tmp15C)->f3;_LL244: s=({struct Cyc_PP_Doc*_tmp1A9[9];_tmp1A9[8]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp1A9[7]=Cyc_PP_line_doc();_tmp1A9[6]=
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp188));_tmp1A9[5]=Cyc_PP_line_doc();
_tmp1A9[4]=Cyc_PP_text(_tag_arr("{",sizeof(unsigned char),2));_tmp1A9[3]=Cyc_Absynpp_qvar2doc(
_tmp187->name);_tmp1A9[2]=Cyc_PP_text(_tag_arr(">",sizeof(unsigned char),2));
_tmp1A9[1]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp186));_tmp1A9[0]=Cyc_PP_text(
_tag_arr("region<",sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp1A9,sizeof(
struct Cyc_PP_Doc*),9));});goto _LL218;_LL218:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp1AA=(void*)p->r;void*_tmp1AB;
int _tmp1AC;unsigned char _tmp1AD;struct _tagged_arr _tmp1AE;struct Cyc_Absyn_Vardecl*
_tmp1AF;struct Cyc_List_List*_tmp1B0;struct Cyc_Absyn_Pat*_tmp1B1;struct Cyc_Absyn_Vardecl*
_tmp1B2;struct _tuple0*_tmp1B3;struct _tuple0*_tmp1B4;struct Cyc_List_List*_tmp1B5;
struct Cyc_Absyn_AggrInfo _tmp1B6;void*_tmp1B7;struct Cyc_List_List*_tmp1B8;struct
Cyc_List_List*_tmp1B9;struct Cyc_Absyn_Enumfield*_tmp1BA;struct Cyc_Absyn_Enumfield*
_tmp1BB;struct Cyc_Absyn_Tunionfield*_tmp1BC;struct Cyc_List_List*_tmp1BD;struct
Cyc_Absyn_Tunionfield*_tmp1BE;struct Cyc_List_List*_tmp1BF;_LL24B: if((int)_tmp1AA
!= 0)goto _LL24D;_LL24C: s=Cyc_PP_text(_tag_arr("_",sizeof(unsigned char),2));goto
_LL24A;_LL24D: if((int)_tmp1AA != 1)goto _LL24F;_LL24E: s=Cyc_PP_text(_tag_arr("NULL",
sizeof(unsigned char),5));goto _LL24A;_LL24F: if(_tmp1AA <= (void*)2?1:*((int*)
_tmp1AA)!= 1)goto _LL251;_tmp1AB=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp1AA)->f1;
_tmp1AC=((struct Cyc_Absyn_Int_p_struct*)_tmp1AA)->f2;_LL250: if(_tmp1AB == (void*)
0)s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp1C1;_tmp1C1.tag=
1;_tmp1C1.f1=(int)((unsigned int)_tmp1AC);{void*_tmp1C0[1]={& _tmp1C1};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp1C0,sizeof(void*),1));}}));
else{s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp1C3;
_tmp1C3.tag=1;_tmp1C3.f1=(unsigned int)_tmp1AC;{void*_tmp1C2[1]={& _tmp1C3};Cyc_Std_aprintf(
_tag_arr("%u",sizeof(unsigned char),3),_tag_arr(_tmp1C2,sizeof(void*),1));}}));}
goto _LL24A;_LL251: if(_tmp1AA <= (void*)2?1:*((int*)_tmp1AA)!= 2)goto _LL253;
_tmp1AD=((struct Cyc_Absyn_Char_p_struct*)_tmp1AA)->f1;_LL252: s=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp1C5;_tmp1C5.tag=0;_tmp1C5.f1=(
struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp1AD);{void*_tmp1C4[1]={& _tmp1C5};
Cyc_Std_aprintf(_tag_arr("'%s'",sizeof(unsigned char),5),_tag_arr(_tmp1C4,
sizeof(void*),1));}}));goto _LL24A;_LL253: if(_tmp1AA <= (void*)2?1:*((int*)_tmp1AA)
!= 3)goto _LL255;_tmp1AE=((struct Cyc_Absyn_Float_p_struct*)_tmp1AA)->f1;_LL254: s=
Cyc_PP_text(_tmp1AE);goto _LL24A;_LL255: if(_tmp1AA <= (void*)2?1:*((int*)_tmp1AA)
!= 0)goto _LL257;_tmp1AF=((struct Cyc_Absyn_Var_p_struct*)_tmp1AA)->f1;_LL256: s=
Cyc_Absynpp_qvar2doc(_tmp1AF->name);goto _LL24A;_LL257: if(_tmp1AA <= (void*)2?1:*((
int*)_tmp1AA)!= 4)goto _LL259;_tmp1B0=((struct Cyc_Absyn_Tuple_p_struct*)_tmp1AA)->f1;
_LL258: s=({struct Cyc_PP_Doc*_tmp1C6[3];_tmp1C6[2]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp1C6[1]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Pat*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_pat2doc,_tag_arr(",",sizeof(unsigned char),2),_tmp1B0);_tmp1C6[0]=
Cyc_PP_text(_tag_arr("$(",sizeof(unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp1C6,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL24A;_LL259: if(_tmp1AA <= (void*)2?1:*((
int*)_tmp1AA)!= 5)goto _LL25B;_tmp1B1=((struct Cyc_Absyn_Pointer_p_struct*)_tmp1AA)->f1;
_LL25A: s=({struct Cyc_PP_Doc*_tmp1C7[2];_tmp1C7[1]=Cyc_Absynpp_pat2doc(_tmp1B1);
_tmp1C7[0]=Cyc_PP_text(_tag_arr("&",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp1C7,sizeof(struct Cyc_PP_Doc*),2));});goto _LL24A;_LL25B: if(_tmp1AA <= (
void*)2?1:*((int*)_tmp1AA)!= 6)goto _LL25D;_tmp1B2=((struct Cyc_Absyn_Reference_p_struct*)
_tmp1AA)->f1;_LL25C: s=({struct Cyc_PP_Doc*_tmp1C8[2];_tmp1C8[1]=Cyc_Absynpp_qvar2doc(
_tmp1B2->name);_tmp1C8[0]=Cyc_PP_text(_tag_arr("*",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp1C8,sizeof(struct Cyc_PP_Doc*),2));});goto _LL24A;_LL25D: if(_tmp1AA <= (
void*)2?1:*((int*)_tmp1AA)!= 11)goto _LL25F;_tmp1B3=((struct Cyc_Absyn_UnknownId_p_struct*)
_tmp1AA)->f1;_LL25E: s=Cyc_Absynpp_qvar2doc(_tmp1B3);goto _LL24A;_LL25F: if(_tmp1AA
<= (void*)2?1:*((int*)_tmp1AA)!= 12)goto _LL261;_tmp1B4=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp1AA)->f1;_tmp1B5=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1AA)->f2;_LL260:
s=({struct Cyc_PP_Doc*_tmp1C9[2];_tmp1C9[1]=Cyc_PP_group(_tag_arr("(",sizeof(
unsigned char),2),_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp1B5));_tmp1C9[0]=
Cyc_Absynpp_qvar2doc(_tmp1B4);Cyc_PP_cat(_tag_arr(_tmp1C9,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL24A;_LL261: if(_tmp1AA <= (void*)2?1:*((int*)_tmp1AA)!= 7)goto _LL263;
_tmp1B6=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1AA)->f1;_tmp1B7=(void*)_tmp1B6.aggr_info;
_tmp1B8=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1AA)->f2;_tmp1B9=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp1AA)->f3;_LL262: {struct _tuple0*_tmp1CB;struct _tuple3 _tmp1CA=Cyc_Absyn_aggr_kinded_name(
_tmp1B7);_tmp1CB=_tmp1CA.f2;s=({struct Cyc_PP_Doc*_tmp1CC[4];_tmp1CC[3]=Cyc_PP_group(
_tag_arr("",sizeof(unsigned char),1),_tag_arr("}",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp1B9));_tmp1CC[2]=Cyc_PP_egroup(_tag_arr("[",sizeof(unsigned char),2),
_tag_arr("]",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _tagged_arr*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,
_tmp1B8)));_tmp1CC[1]=Cyc_PP_text(_tag_arr("{",sizeof(unsigned char),2));_tmp1CC[
0]=Cyc_Absynpp_qvar2doc(_tmp1CB);Cyc_PP_cat(_tag_arr(_tmp1CC,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL24A;}_LL263: if(_tmp1AA <= (void*)2?1:*((int*)_tmp1AA)!= 9)goto
_LL265;_tmp1BA=((struct Cyc_Absyn_Enum_p_struct*)_tmp1AA)->f2;_LL264: _tmp1BB=
_tmp1BA;goto _LL266;_LL265: if(_tmp1AA <= (void*)2?1:*((int*)_tmp1AA)!= 10)goto
_LL267;_tmp1BB=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp1AA)->f2;_LL266: s=Cyc_Absynpp_qvar2doc(
_tmp1BB->name);goto _LL24A;_LL267: if(_tmp1AA <= (void*)2?1:*((int*)_tmp1AA)!= 8)
goto _LL269;_tmp1BC=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1AA)->f2;_tmp1BD=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp1AA)->f3;if(_tmp1BD != 0)goto _LL269;_LL268: s=
Cyc_Absynpp_qvar2doc(_tmp1BC->name);goto _LL24A;_LL269: if(_tmp1AA <= (void*)2?1:*((
int*)_tmp1AA)!= 8)goto _LL24A;_tmp1BE=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1AA)->f2;
_tmp1BF=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1AA)->f3;_LL26A: s=({struct Cyc_PP_Doc*
_tmp1CD[2];_tmp1CD[1]=Cyc_PP_egroup(_tag_arr("(",sizeof(unsigned char),2),
_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp1BF));_tmp1CD[0]=Cyc_Absynpp_qvar2doc(
_tmp1BE->name);Cyc_PP_cat(_tag_arr(_tmp1CD,sizeof(struct Cyc_PP_Doc*),2));});goto
_LL24A;_LL24A:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple7*dp){
return({struct Cyc_PP_Doc*_tmp1CE[2];_tmp1CE[1]=Cyc_Absynpp_pat2doc((*dp).f2);
_tmp1CE[0]=Cyc_PP_egroup(_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",
sizeof(unsigned char),2),_tag_arr("=",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*
dp).f1));Cyc_PP_cat(_tag_arr(_tmp1CE,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*
Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){if(c->where_clause
== 0?(void*)(c->pattern)->r == (void*)0: 0)return({struct Cyc_PP_Doc*_tmp1CF[2];
_tmp1CF[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1D0[2];_tmp1D0[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp1D0[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1D0,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp1CF[0]=Cyc_PP_text(_tag_arr("default: ",sizeof(
unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp1CF,sizeof(struct Cyc_PP_Doc*),2));});
else{if(c->where_clause == 0)return({struct Cyc_PP_Doc*_tmp1D1[4];_tmp1D1[3]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp1D2[2];_tmp1D2[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1D2[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1D2,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp1D1[2]=Cyc_PP_text(_tag_arr(": ",sizeof(unsigned char),3));_tmp1D1[1]=Cyc_Absynpp_pat2doc(
c->pattern);_tmp1D1[0]=Cyc_PP_text(_tag_arr("case ",sizeof(unsigned char),6));
Cyc_PP_cat(_tag_arr(_tmp1D1,sizeof(struct Cyc_PP_Doc*),4));});else{return({struct
Cyc_PP_Doc*_tmp1D3[6];_tmp1D3[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1D4[2];
_tmp1D4[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1D4[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp1D4,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1D3[4]=Cyc_PP_text(
_tag_arr(": ",sizeof(unsigned char),3));_tmp1D3[3]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(c->where_clause));_tmp1D3[2]=Cyc_PP_text(_tag_arr(" && ",
sizeof(unsigned char),5));_tmp1D3[1]=Cyc_Absynpp_pat2doc(c->pattern);_tmp1D3[0]=
Cyc_PP_text(_tag_arr("case ",sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(
_tmp1D3,sizeof(struct Cyc_PP_Doc*),6));});}}}struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Switch_clause*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_switchclause2doc,_tag_arr("",sizeof(unsigned char),1),cs);}struct Cyc_PP_Doc*
Cyc_Absynpp_switchCclause2doc(struct Cyc_Absyn_SwitchC_clause*c){struct Cyc_Absyn_SwitchC_clause
_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D7;struct Cyc_Absyn_Stmt*_tmp1D8;struct Cyc_Absyn_SwitchC_clause*
_tmp1D5=c;_tmp1D6=*_tmp1D5;_tmp1D7=_tmp1D6.cnst_exp;_tmp1D8=_tmp1D6.body;if(
_tmp1D7 == 0)return({struct Cyc_PP_Doc*_tmp1D9[2];_tmp1D9[1]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp1DA[2];_tmp1DA[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1DA[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1DA,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp1D9[0]=Cyc_PP_text(_tag_arr("default: ",sizeof(unsigned char),10));Cyc_PP_cat(
_tag_arr(_tmp1D9,sizeof(struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*
_tmp1DB[4];_tmp1DB[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1DC[2];_tmp1DC[1]=Cyc_Absynpp_stmt2doc(
_tmp1D8);_tmp1DC[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1DC,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp1DB[2]=Cyc_PP_text(_tag_arr(": ",sizeof(unsigned char),
3));_tmp1DB[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(_tmp1D7));
_tmp1DB[0]=Cyc_PP_text(_tag_arr("case ",sizeof(unsigned char),6));Cyc_PP_cat(
_tag_arr(_tmp1DB,sizeof(struct Cyc_PP_Doc*),4));});}}struct Cyc_PP_Doc*Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_SwitchC_clause*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_switchCclause2doc,_tag_arr("",sizeof(unsigned char),1),cs);}struct
Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{return({struct Cyc_PP_Doc*_tmp1DD[3];
_tmp1DD[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag));
_tmp1DD[1]=Cyc_PP_text(_tag_arr(" = ",sizeof(unsigned char),4));_tmp1DD[0]=Cyc_Absynpp_qvar2doc(
f->name);Cyc_PP_cat(_tag_arr(_tmp1DD,sizeof(struct Cyc_PP_Doc*),3));});}}struct
Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _tagged_arr sep,struct
Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,_tag_arr(",",sizeof(
unsigned char),2),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*
v){return Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(
struct Cyc_List_List*vds){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Vardecl*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_id2doc,_tag_arr(",",sizeof(unsigned char),2),vds);}struct Cyc_PP_Doc*
Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl
_tmp1DF;void*_tmp1E0;struct _tuple0*_tmp1E1;struct Cyc_Absyn_Tqual _tmp1E2;void*
_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_List_List*_tmp1E5;struct Cyc_Absyn_Vardecl*
_tmp1DE=vd;_tmp1DF=*_tmp1DE;_tmp1E0=(void*)_tmp1DF.sc;_tmp1E1=_tmp1DF.name;
_tmp1E2=_tmp1DF.tq;_tmp1E3=(void*)_tmp1DF.type;_tmp1E4=_tmp1DF.initializer;
_tmp1E5=_tmp1DF.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2doc(
_tmp1E1);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp1E5);struct Cyc_PP_Doc*
beforenamedoc;if(!Cyc_Absynpp_to_VC)beforenamedoc=attsdoc;else{void*_tmp1E6=Cyc_Tcutil_compress(
_tmp1E3);struct Cyc_Absyn_FnInfo _tmp1E7;struct Cyc_List_List*_tmp1E8;_LL26C: if(
_tmp1E6 <= (void*)3?1:*((int*)_tmp1E6)!= 8)goto _LL26E;_tmp1E7=((struct Cyc_Absyn_FnType_struct*)
_tmp1E6)->f1;_tmp1E8=_tmp1E7.attributes;_LL26D: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp1E8);goto _LL26B;_LL26E:;_LL26F: beforenamedoc=Cyc_PP_nil_doc();goto _LL26B;
_LL26B:;}s=({struct Cyc_PP_Doc*_tmp1E9[5];_tmp1E9[4]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp1E9[3]=_tmp1E4 == 0?Cyc_PP_text(_tag_arr("",sizeof(
unsigned char),1)):({struct Cyc_PP_Doc*_tmp1EC[2];_tmp1EC[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(_tmp1E4));_tmp1EC[0]=Cyc_PP_text(_tag_arr(" = ",
sizeof(unsigned char),4));Cyc_PP_cat(_tag_arr(_tmp1EC,sizeof(struct Cyc_PP_Doc*),
2));});_tmp1E9[2]=Cyc_Absynpp_tqtd2doc(_tmp1E2,_tmp1E3,({struct Cyc_Core_Opt*
_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA->v=({struct Cyc_PP_Doc*_tmp1EB[2];
_tmp1EB[1]=sn;_tmp1EB[0]=beforenamedoc;Cyc_PP_cat(_tag_arr(_tmp1EB,sizeof(struct
Cyc_PP_Doc*),2));});_tmp1EA;}));_tmp1E9[1]=Cyc_Absynpp_scope2doc(_tmp1E0);
_tmp1E9[0]=Cyc_Absynpp_to_VC?attsdoc: Cyc_PP_nil_doc();Cyc_PP_cat(_tag_arr(
_tmp1E9,sizeof(struct Cyc_PP_Doc*),5));});return s;}}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp1ED=(void*)d->r;struct Cyc_Absyn_Fndecl*
_tmp1EE;struct Cyc_Absyn_Aggrdecl*_tmp1EF;struct Cyc_Absyn_Vardecl*_tmp1F0;struct
Cyc_Absyn_Tuniondecl*_tmp1F1;struct Cyc_Absyn_Tuniondecl _tmp1F2;void*_tmp1F3;
struct _tuple0*_tmp1F4;struct Cyc_List_List*_tmp1F5;struct Cyc_Core_Opt*_tmp1F6;int
_tmp1F7;struct Cyc_Absyn_Pat*_tmp1F8;struct Cyc_Absyn_Exp*_tmp1F9;struct Cyc_List_List*
_tmp1FA;struct Cyc_Absyn_Enumdecl*_tmp1FB;struct Cyc_Absyn_Enumdecl _tmp1FC;void*
_tmp1FD;struct _tuple0*_tmp1FE;struct Cyc_Core_Opt*_tmp1FF;struct Cyc_Absyn_Typedefdecl*
_tmp200;struct _tagged_arr*_tmp201;struct Cyc_List_List*_tmp202;struct _tuple0*
_tmp203;struct Cyc_List_List*_tmp204;struct Cyc_List_List*_tmp205;_LL271: if(*((int*)
_tmp1ED)!= 1)goto _LL273;_tmp1EE=((struct Cyc_Absyn_Fn_d_struct*)_tmp1ED)->f1;
_LL272: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp20D=_cycalloc(sizeof(*
_tmp20D));_tmp20D[0]=({struct Cyc_Absyn_FnType_struct _tmp20E;_tmp20E.tag=8;
_tmp20E.f1=({struct Cyc_Absyn_FnInfo _tmp20F;_tmp20F.tvars=_tmp1EE->tvs;_tmp20F.effect=
_tmp1EE->effect;_tmp20F.ret_typ=(void*)((void*)_tmp1EE->ret_type);_tmp20F.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple5*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp1EE->args);_tmp20F.c_varargs=_tmp1EE->c_varargs;
_tmp20F.cyc_varargs=_tmp1EE->cyc_varargs;_tmp20F.rgn_po=_tmp1EE->rgn_po;_tmp20F.attributes=
0;_tmp20F;});_tmp20E;});_tmp20D;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp1EE->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp1EE->is_inline){if(Cyc_Absynpp_to_VC)
inlinedoc=Cyc_PP_text(_tag_arr("__inline ",sizeof(unsigned char),10));else{
inlinedoc=Cyc_PP_text(_tag_arr("inline ",sizeof(unsigned char),8));}}else{
inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((
void*)_tmp1EE->sc);struct Cyc_PP_Doc*beforenamedoc=Cyc_Absynpp_to_VC?Cyc_Absynpp_callconv2doc(
_tmp1EE->attributes): attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp1EE->name);struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual
_tmp20A;_tmp20A.q_const=0;_tmp20A.q_volatile=0;_tmp20A.q_restrict=0;_tmp20A;}),t,({
struct Cyc_Core_Opt*_tmp20B=_cycalloc(sizeof(*_tmp20B));_tmp20B->v=({struct Cyc_PP_Doc*
_tmp20C[2];_tmp20C[1]=namedoc;_tmp20C[0]=beforenamedoc;Cyc_PP_cat(_tag_arr(
_tmp20C,sizeof(struct Cyc_PP_Doc*),2));});_tmp20B;}));struct Cyc_PP_Doc*bodydoc=({
struct Cyc_PP_Doc*_tmp208[4];_tmp208[3]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp208[2]=Cyc_PP_line_doc();_tmp208[1]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp209[2];_tmp209[1]=Cyc_Absynpp_stmt2doc(_tmp1EE->body);
_tmp209[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp209,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp208[0]=Cyc_PP_text(_tag_arr(" {",sizeof(unsigned char),3));Cyc_PP_cat(
_tag_arr(_tmp208,sizeof(struct Cyc_PP_Doc*),4));});s=({struct Cyc_PP_Doc*_tmp206[4];
_tmp206[3]=bodydoc;_tmp206[2]=tqtddoc;_tmp206[1]=scopedoc;_tmp206[0]=inlinedoc;
Cyc_PP_cat(_tag_arr(_tmp206,sizeof(struct Cyc_PP_Doc*),4));});if(Cyc_Absynpp_to_VC)
s=({struct Cyc_PP_Doc*_tmp207[2];_tmp207[1]=s;_tmp207[0]=attsdoc;Cyc_PP_cat(
_tag_arr(_tmp207,sizeof(struct Cyc_PP_Doc*),2));});goto _LL270;}}_LL273: if(*((int*)
_tmp1ED)!= 4)goto _LL275;_tmp1EF=((struct Cyc_Absyn_Aggr_d_struct*)_tmp1ED)->f1;
_LL274: if(_tmp1EF->fields == 0)s=({struct Cyc_PP_Doc*_tmp210[5];_tmp210[4]=Cyc_PP_text(
_tag_arr(";",sizeof(unsigned char),2));_tmp210[3]=Cyc_Absynpp_ktvars2doc(_tmp1EF->tvs);
_tmp210[2]=Cyc_Absynpp_qvar2doc(_tmp1EF->name);_tmp210[1]=Cyc_Absynpp_aggr_kind2doc((
void*)_tmp1EF->kind);_tmp210[0]=Cyc_Absynpp_scope2doc((void*)_tmp1EF->sc);Cyc_PP_cat(
_tag_arr(_tmp210,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp211[10];_tmp211[9]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));
_tmp211[8]=Cyc_Absynpp_atts2doc(_tmp1EF->attributes);_tmp211[7]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp211[6]=Cyc_PP_line_doc();_tmp211[5]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp212[2];_tmp212[1]=Cyc_Absynpp_aggrfields2doc((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1EF->fields))->v);
_tmp212[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp212,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp211[4]=Cyc_PP_text(_tag_arr(" {",sizeof(unsigned char),3));_tmp211[3]=
Cyc_Absynpp_ktvars2doc(_tmp1EF->tvs);_tmp211[2]=Cyc_Absynpp_qvar2doc(_tmp1EF->name);
_tmp211[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp1EF->kind);_tmp211[0]=Cyc_Absynpp_scope2doc((
void*)_tmp1EF->sc);Cyc_PP_cat(_tag_arr(_tmp211,sizeof(struct Cyc_PP_Doc*),10));});}
goto _LL270;_LL275: if(*((int*)_tmp1ED)!= 0)goto _LL277;_tmp1F0=((struct Cyc_Absyn_Var_d_struct*)
_tmp1ED)->f1;_LL276: s=Cyc_Absynpp_vardecl2doc(_tmp1F0);goto _LL270;_LL277: if(*((
int*)_tmp1ED)!= 5)goto _LL279;_tmp1F1=((struct Cyc_Absyn_Tunion_d_struct*)_tmp1ED)->f1;
_tmp1F2=*_tmp1F1;_tmp1F3=(void*)_tmp1F2.sc;_tmp1F4=_tmp1F2.name;_tmp1F5=_tmp1F2.tvs;
_tmp1F6=_tmp1F2.fields;_tmp1F7=_tmp1F2.is_xtunion;_LL278: if(_tmp1F6 == 0)s=({
struct Cyc_PP_Doc*_tmp213[5];_tmp213[4]=Cyc_PP_text(_tag_arr(";",sizeof(
unsigned char),2));_tmp213[3]=Cyc_Absynpp_ktvars2doc(_tmp1F5);_tmp213[2]=_tmp1F7?
Cyc_Absynpp_qvar2doc(_tmp1F4): Cyc_Absynpp_typedef_name2doc(_tmp1F4);_tmp213[1]=
_tmp1F7?Cyc_PP_text(_tag_arr("xtunion ",sizeof(unsigned char),9)): Cyc_PP_text(
_tag_arr("tunion ",sizeof(unsigned char),8));_tmp213[0]=Cyc_Absynpp_scope2doc(
_tmp1F3);Cyc_PP_cat(_tag_arr(_tmp213,sizeof(struct Cyc_PP_Doc*),5));});else{s=({
struct Cyc_PP_Doc*_tmp214[8];_tmp214[7]=Cyc_PP_text(_tag_arr("};",sizeof(
unsigned char),3));_tmp214[6]=Cyc_PP_line_doc();_tmp214[5]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp215[2];_tmp215[1]=Cyc_Absynpp_tunionfields2doc((struct Cyc_List_List*)
_tmp1F6->v);_tmp215[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp215,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp214[4]=Cyc_PP_text(_tag_arr(" {",sizeof(
unsigned char),3));_tmp214[3]=Cyc_Absynpp_ktvars2doc(_tmp1F5);_tmp214[2]=_tmp1F7?
Cyc_Absynpp_qvar2doc(_tmp1F4): Cyc_Absynpp_typedef_name2doc(_tmp1F4);_tmp214[1]=
_tmp1F7?Cyc_PP_text(_tag_arr("xtunion ",sizeof(unsigned char),9)): Cyc_PP_text(
_tag_arr("tunion ",sizeof(unsigned char),8));_tmp214[0]=Cyc_Absynpp_scope2doc(
_tmp1F3);Cyc_PP_cat(_tag_arr(_tmp214,sizeof(struct Cyc_PP_Doc*),8));});}goto
_LL270;_LL279: if(*((int*)_tmp1ED)!= 2)goto _LL27B;_tmp1F8=((struct Cyc_Absyn_Let_d_struct*)
_tmp1ED)->f1;_tmp1F9=((struct Cyc_Absyn_Let_d_struct*)_tmp1ED)->f4;_LL27A: s=({
struct Cyc_PP_Doc*_tmp216[5];_tmp216[4]=Cyc_PP_text(_tag_arr(";",sizeof(
unsigned char),2));_tmp216[3]=Cyc_Absynpp_exp2doc(_tmp1F9);_tmp216[2]=Cyc_PP_text(
_tag_arr(" = ",sizeof(unsigned char),4));_tmp216[1]=Cyc_Absynpp_pat2doc(_tmp1F8);
_tmp216[0]=Cyc_PP_text(_tag_arr("let ",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp216,sizeof(struct Cyc_PP_Doc*),5));});goto _LL270;_LL27B: if(*((int*)
_tmp1ED)!= 3)goto _LL27D;_tmp1FA=((struct Cyc_Absyn_Letv_d_struct*)_tmp1ED)->f1;
_LL27C: s=({struct Cyc_PP_Doc*_tmp217[3];_tmp217[2]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp217[1]=Cyc_Absynpp_ids2doc(_tmp1FA);_tmp217[0]=Cyc_PP_text(
_tag_arr("let ",sizeof(unsigned char),5));Cyc_PP_cat(_tag_arr(_tmp217,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL270;_LL27D: if(*((int*)_tmp1ED)!= 6)goto _LL27F;
_tmp1FB=((struct Cyc_Absyn_Enum_d_struct*)_tmp1ED)->f1;_tmp1FC=*_tmp1FB;_tmp1FD=(
void*)_tmp1FC.sc;_tmp1FE=_tmp1FC.name;_tmp1FF=_tmp1FC.fields;_LL27E: if(_tmp1FF == 
0)s=({struct Cyc_PP_Doc*_tmp218[4];_tmp218[3]=Cyc_PP_text(_tag_arr(";",sizeof(
unsigned char),2));_tmp218[2]=Cyc_Absynpp_typedef_name2doc(_tmp1FE);_tmp218[1]=
Cyc_PP_text(_tag_arr("enum ",sizeof(unsigned char),6));_tmp218[0]=Cyc_Absynpp_scope2doc(
_tmp1FD);Cyc_PP_cat(_tag_arr(_tmp218,sizeof(struct Cyc_PP_Doc*),4));});else{s=({
struct Cyc_PP_Doc*_tmp219[7];_tmp219[6]=Cyc_PP_text(_tag_arr("};",sizeof(
unsigned char),3));_tmp219[5]=Cyc_PP_line_doc();_tmp219[4]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp21A[2];_tmp21A[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)
_tmp1FF->v);_tmp21A[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp21A,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp219[3]=Cyc_PP_text(_tag_arr(" {",sizeof(
unsigned char),3));_tmp219[2]=Cyc_Absynpp_qvar2doc(_tmp1FE);_tmp219[1]=Cyc_PP_text(
_tag_arr("enum ",sizeof(unsigned char),6));_tmp219[0]=Cyc_Absynpp_scope2doc(
_tmp1FD);Cyc_PP_cat(_tag_arr(_tmp219,sizeof(struct Cyc_PP_Doc*),7));});}goto
_LL270;_LL27F: if(*((int*)_tmp1ED)!= 7)goto _LL281;_tmp200=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp1ED)->f1;_LL280: s=({struct Cyc_PP_Doc*_tmp21B[3];_tmp21B[2]=Cyc_PP_text(
_tag_arr(";",sizeof(unsigned char),2));_tmp21B[1]=Cyc_Absynpp_tqtd2doc(({struct
Cyc_Absyn_Tqual _tmp21C;_tmp21C.q_const=0;_tmp21C.q_volatile=0;_tmp21C.q_restrict=
0;_tmp21C;}),(void*)_tmp200->defn,({struct Cyc_Core_Opt*_tmp21D=_cycalloc(sizeof(*
_tmp21D));_tmp21D->v=({struct Cyc_PP_Doc*_tmp21E[2];_tmp21E[1]=Cyc_Absynpp_tvars2doc(
_tmp200->tvs);_tmp21E[0]=Cyc_Absynpp_typedef_name2doc(_tmp200->name);Cyc_PP_cat(
_tag_arr(_tmp21E,sizeof(struct Cyc_PP_Doc*),2));});_tmp21D;}));_tmp21B[0]=Cyc_PP_text(
_tag_arr("typedef ",sizeof(unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp21B,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL270;_LL281: if(*((int*)_tmp1ED)!= 8)goto
_LL283;_tmp201=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1ED)->f1;_tmp202=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp1ED)->f2;_LL282: if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_add(_tmp201);s=({struct Cyc_PP_Doc*_tmp21F[7];_tmp21F[
6]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp21F[5]=Cyc_PP_line_doc();
_tmp21F[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(unsigned char),1),_tmp202);_tmp21F[3]=Cyc_PP_line_doc();
_tmp21F[2]=Cyc_PP_text(_tag_arr(" {",sizeof(unsigned char),3));_tmp21F[1]=Cyc_PP_textptr(
_tmp201);_tmp21F[0]=Cyc_PP_text(_tag_arr("namespace ",sizeof(unsigned char),11));
Cyc_PP_cat(_tag_arr(_tmp21F,sizeof(struct Cyc_PP_Doc*),7));});if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_drop();goto _LL270;_LL283: if(*((int*)_tmp1ED)!= 9)goto
_LL285;_tmp203=((struct Cyc_Absyn_Using_d_struct*)_tmp1ED)->f1;_tmp204=((struct
Cyc_Absyn_Using_d_struct*)_tmp1ED)->f2;_LL284: if(Cyc_Absynpp_print_using_stmts)s=({
struct Cyc_PP_Doc*_tmp220[7];_tmp220[6]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp220[5]=Cyc_PP_line_doc();_tmp220[4]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),
_tmp204);_tmp220[3]=Cyc_PP_line_doc();_tmp220[2]=Cyc_PP_text(_tag_arr(" {",
sizeof(unsigned char),3));_tmp220[1]=Cyc_Absynpp_qvar2doc(_tmp203);_tmp220[0]=
Cyc_PP_text(_tag_arr("using ",sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(
_tmp220,sizeof(struct Cyc_PP_Doc*),7));});else{s=({struct Cyc_PP_Doc*_tmp221[7];
_tmp221[6]=Cyc_PP_text(_tag_arr("/* } */",sizeof(unsigned char),8));_tmp221[5]=
Cyc_PP_line_doc();_tmp221[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),_tmp204);_tmp221[3]=
Cyc_PP_line_doc();_tmp221[2]=Cyc_PP_text(_tag_arr(" { */",sizeof(unsigned char),
6));_tmp221[1]=Cyc_Absynpp_qvar2doc(_tmp203);_tmp221[0]=Cyc_PP_text(_tag_arr("/* using ",
sizeof(unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp221,sizeof(struct Cyc_PP_Doc*),
7));});}goto _LL270;_LL285: if(*((int*)_tmp1ED)!= 10)goto _LL270;_tmp205=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp1ED)->f1;_LL286: if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_tmp222[5];_tmp222[4]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp222[3]=Cyc_PP_line_doc();_tmp222[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),
_tmp205);_tmp222[1]=Cyc_PP_line_doc();_tmp222[0]=Cyc_PP_text(_tag_arr("extern \"C\" {",
sizeof(unsigned char),13));Cyc_PP_cat(_tag_arr(_tmp222,sizeof(struct Cyc_PP_Doc*),
5));});else{s=({struct Cyc_PP_Doc*_tmp223[5];_tmp223[4]=Cyc_PP_text(_tag_arr("/* } */",
sizeof(unsigned char),8));_tmp223[3]=Cyc_PP_line_doc();_tmp223[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),
_tmp205);_tmp223[1]=Cyc_PP_line_doc();_tmp223[0]=Cyc_PP_text(_tag_arr("/* extern \"C\" { */",
sizeof(unsigned char),19));Cyc_PP_cat(_tag_arr(_tmp223,sizeof(struct Cyc_PP_Doc*),
5));});}goto _LL270;_LL270:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*
sc){void*_tmp224=sc;_LL288: if((int)_tmp224 != 0)goto _LL28A;_LL289: return Cyc_PP_text(
_tag_arr("static ",sizeof(unsigned char),8));_LL28A: if((int)_tmp224 != 2)goto
_LL28C;_LL28B: return Cyc_PP_nil_doc();_LL28C: if((int)_tmp224 != 3)goto _LL28E;
_LL28D: return Cyc_PP_text(_tag_arr("extern ",sizeof(unsigned char),8));_LL28E: if((
int)_tmp224 != 4)goto _LL290;_LL28F: return Cyc_PP_text(_tag_arr("extern \"C\" ",
sizeof(unsigned char),12));_LL290: if((int)_tmp224 != 1)goto _LL287;_LL291: return
Cyc_PP_text(_tag_arr("abstract ",sizeof(unsigned char),10));_LL287:;}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp225=t;struct Cyc_Absyn_Tvar*_tmp226;struct Cyc_List_List*_tmp227;
_LL293: if(_tmp225 <= (void*)3?1:*((int*)_tmp225)!= 1)goto _LL295;_tmp226=((struct
Cyc_Absyn_VarType_struct*)_tmp225)->f1;_LL294: return Cyc_Tcutil_is_temp_tvar(
_tmp226);_LL295: if(_tmp225 <= (void*)3?1:*((int*)_tmp225)!= 18)goto _LL297;_tmp227=((
struct Cyc_Absyn_JoinEff_struct*)_tmp225)->f1;_LL296: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp227);_LL297:;_LL298: return 0;_LL292:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp228=t;struct Cyc_Core_Opt*_tmp229;struct Cyc_Core_Opt _tmp22A;void*_tmp22B;
_LL29A: if(_tmp228 <= (void*)3?1:*((int*)_tmp228)!= 11)goto _LL29C;_LL29B: return 1;
_LL29C: if(_tmp228 <= (void*)3?1:*((int*)_tmp228)!= 16)goto _LL29E;_tmp229=((struct
Cyc_Absyn_TypedefType_struct*)_tmp228)->f3;if(_tmp229 == 0)goto _LL29E;_tmp22A=*
_tmp229;_tmp22B=(void*)_tmp22A.v;_LL29D: return Cyc_Absynpp_is_anon_aggrtype(
_tmp22B);_LL29E:;_LL29F: return 0;_LL299:;}struct _tuple6 Cyc_Absynpp_to_tms(struct
Cyc_Absyn_Tqual tq,void*t){void*_tmp22C=t;void*_tmp22D;struct Cyc_Absyn_Tqual
_tmp22E;struct Cyc_Absyn_Exp*_tmp22F;struct Cyc_Absyn_PtrInfo _tmp230;void*_tmp231;
void*_tmp232;struct Cyc_Absyn_Conref*_tmp233;struct Cyc_Absyn_Tqual _tmp234;struct
Cyc_Absyn_Conref*_tmp235;struct Cyc_Absyn_FnInfo _tmp236;struct Cyc_List_List*
_tmp237;struct Cyc_Core_Opt*_tmp238;void*_tmp239;struct Cyc_List_List*_tmp23A;int
_tmp23B;struct Cyc_Absyn_VarargInfo*_tmp23C;struct Cyc_List_List*_tmp23D;struct Cyc_List_List*
_tmp23E;struct Cyc_Core_Opt*_tmp23F;struct Cyc_Core_Opt*_tmp240;int _tmp241;struct
_tuple0*_tmp242;struct Cyc_List_List*_tmp243;struct Cyc_Core_Opt*_tmp244;_LL2A1:
if(_tmp22C <= (void*)3?1:*((int*)_tmp22C)!= 7)goto _LL2A3;_tmp22D=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp22C)->f1;_tmp22E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp22C)->f2;_tmp22F=((struct Cyc_Absyn_ArrayType_struct*)_tmp22C)->f3;_LL2A2: {
struct Cyc_Absyn_Tqual _tmp246;void*_tmp247;struct Cyc_List_List*_tmp248;struct
_tuple6 _tmp245=Cyc_Absynpp_to_tms(_tmp22E,_tmp22D);_tmp246=_tmp245.f1;_tmp247=
_tmp245.f2;_tmp248=_tmp245.f3;{void*tm;if(_tmp22F == 0)tm=(void*)0;else{tm=(void*)({
struct Cyc_Absyn_ConstArray_mod_struct*_tmp249=_cycalloc(sizeof(*_tmp249));
_tmp249[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp24A;_tmp24A.tag=0;_tmp24A.f1=(
struct Cyc_Absyn_Exp*)_check_null(_tmp22F);_tmp24A;});_tmp249;});}return({struct
_tuple6 _tmp24B;_tmp24B.f1=_tmp246;_tmp24B.f2=_tmp247;_tmp24B.f3=({struct Cyc_List_List*
_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C->hd=(void*)tm;_tmp24C->tl=_tmp248;
_tmp24C;});_tmp24B;});}}_LL2A3: if(_tmp22C <= (void*)3?1:*((int*)_tmp22C)!= 4)goto
_LL2A5;_tmp230=((struct Cyc_Absyn_PointerType_struct*)_tmp22C)->f1;_tmp231=(void*)
_tmp230.elt_typ;_tmp232=(void*)_tmp230.rgn_typ;_tmp233=_tmp230.nullable;_tmp234=
_tmp230.tq;_tmp235=_tmp230.bounds;_LL2A4: {struct Cyc_Absyn_Tqual _tmp24E;void*
_tmp24F;struct Cyc_List_List*_tmp250;struct _tuple6 _tmp24D=Cyc_Absynpp_to_tms(
_tmp234,_tmp231);_tmp24E=_tmp24D.f1;_tmp24F=_tmp24D.f2;_tmp250=_tmp24D.f3;{void*
ps;{struct _tuple4 _tmp252=({struct _tuple4 _tmp251;_tmp251.f1=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp233))->v;_tmp251.f2=(
void*)(Cyc_Absyn_compress_conref(_tmp235))->v;_tmp251;});void*_tmp253;int _tmp254;
void*_tmp255;void*_tmp256;struct Cyc_Absyn_Exp*_tmp257;void*_tmp258;int _tmp259;
void*_tmp25A;void*_tmp25B;struct Cyc_Absyn_Exp*_tmp25C;void*_tmp25D;void*_tmp25E;
_LL2AE: _tmp253=_tmp252.f1;if(_tmp253 <= (void*)1?1:*((int*)_tmp253)!= 0)goto
_LL2B0;_tmp254=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp253)->f1;if(_tmp254
!= 1)goto _LL2B0;_tmp255=_tmp252.f2;if(_tmp255 <= (void*)1?1:*((int*)_tmp255)!= 0)
goto _LL2B0;_tmp256=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp255)->f1;if(
_tmp256 <= (void*)1?1:*((int*)_tmp256)!= 0)goto _LL2B0;_tmp257=((struct Cyc_Absyn_Upper_b_struct*)
_tmp256)->f1;_LL2AF: ps=(void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp25F=
_cycalloc(sizeof(*_tmp25F));_tmp25F[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp260;_tmp260.tag=1;_tmp260.f1=_tmp257;_tmp260;});_tmp25F;});goto _LL2AD;_LL2B0:
_tmp258=_tmp252.f1;if(_tmp258 <= (void*)1?1:*((int*)_tmp258)!= 0)goto _LL2B2;
_tmp259=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp258)->f1;if(_tmp259 != 0)
goto _LL2B2;_tmp25A=_tmp252.f2;if(_tmp25A <= (void*)1?1:*((int*)_tmp25A)!= 0)goto
_LL2B2;_tmp25B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp25A)->f1;if(
_tmp25B <= (void*)1?1:*((int*)_tmp25B)!= 0)goto _LL2B2;_tmp25C=((struct Cyc_Absyn_Upper_b_struct*)
_tmp25B)->f1;_LL2B1: ps=(void*)({struct Cyc_Absyn_NonNullable_ps_struct*_tmp261=
_cycalloc(sizeof(*_tmp261));_tmp261[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp262;_tmp262.tag=0;_tmp262.f1=_tmp25C;_tmp262;});_tmp261;});goto _LL2AD;_LL2B2:
_tmp25D=_tmp252.f2;if(_tmp25D <= (void*)1?1:*((int*)_tmp25D)!= 0)goto _LL2B4;
_tmp25E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp25D)->f1;if((int)_tmp25E
!= 0)goto _LL2B4;_LL2B3: ps=(void*)0;goto _LL2AD;_LL2B4:;_LL2B5: ps=(void*)0;goto
_LL2AD;_LL2AD:;}{void*tm=(void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp265=
_cycalloc(sizeof(*_tmp265));_tmp265[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp266;_tmp266.tag=1;_tmp266.f1=(void*)ps;_tmp266.f2=(void*)_tmp232;_tmp266.f3=
tq;_tmp266;});_tmp265;});return({struct _tuple6 _tmp263;_tmp263.f1=_tmp24E;_tmp263.f2=
_tmp24F;_tmp263.f3=({struct Cyc_List_List*_tmp264=_cycalloc(sizeof(*_tmp264));
_tmp264->hd=(void*)tm;_tmp264->tl=_tmp250;_tmp264;});_tmp263;});}}}_LL2A5: if(
_tmp22C <= (void*)3?1:*((int*)_tmp22C)!= 8)goto _LL2A7;_tmp236=((struct Cyc_Absyn_FnType_struct*)
_tmp22C)->f1;_tmp237=_tmp236.tvars;_tmp238=_tmp236.effect;_tmp239=(void*)_tmp236.ret_typ;
_tmp23A=_tmp236.args;_tmp23B=_tmp236.c_varargs;_tmp23C=_tmp236.cyc_varargs;
_tmp23D=_tmp236.rgn_po;_tmp23E=_tmp236.attributes;_LL2A6: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp238 == 0?1: Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp238->v)){
_tmp238=0;_tmp237=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp237);}{struct Cyc_Absyn_Tqual _tmp268;void*_tmp269;struct Cyc_List_List*_tmp26A;
struct _tuple6 _tmp267=Cyc_Absynpp_to_tms(Cyc_Absyn_empty_tqual(),_tmp239);_tmp268=
_tmp267.f1;_tmp269=_tmp267.f2;_tmp26A=_tmp267.f3;{struct Cyc_List_List*tms=
_tmp26A;if(_tmp23E != 0?!Cyc_Absynpp_to_VC: 0)tms=({struct Cyc_List_List*_tmp26B=
_cycalloc(sizeof(*_tmp26B));_tmp26B->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp26D;_tmp26D.tag=4;_tmp26D.f1=0;_tmp26D.f2=_tmp23E;_tmp26D;});_tmp26C;}));
_tmp26B->tl=tms;_tmp26B;});tms=({struct Cyc_List_List*_tmp26E=_cycalloc(sizeof(*
_tmp26E));_tmp26E->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp270;_tmp270.tag=2;_tmp270.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp271=_cycalloc(sizeof(*_tmp271));_tmp271[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp272;_tmp272.tag=1;_tmp272.f1=_tmp23A;_tmp272.f2=_tmp23B;_tmp272.f3=_tmp23C;
_tmp272.f4=_tmp238;_tmp272.f5=_tmp23D;_tmp272;});_tmp271;}));_tmp270;});_tmp26F;}));
_tmp26E->tl=tms;_tmp26E;});if(Cyc_Absynpp_to_VC)for(0;_tmp23E != 0;_tmp23E=
_tmp23E->tl){void*_tmp273=(void*)_tmp23E->hd;_LL2B7: if((int)_tmp273 != 0)goto
_LL2B9;_LL2B8: goto _LL2BA;_LL2B9: if((int)_tmp273 != 1)goto _LL2BB;_LL2BA: goto _LL2BC;
_LL2BB: if((int)_tmp273 != 2)goto _LL2BD;_LL2BC: tms=({struct Cyc_List_List*_tmp274=
_cycalloc(sizeof(*_tmp274));_tmp274->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp276;_tmp276.tag=4;_tmp276.f1=0;_tmp276.f2=({struct Cyc_List_List*_tmp277=
_cycalloc(sizeof(*_tmp277));_tmp277->hd=(void*)((void*)_tmp23E->hd);_tmp277->tl=
0;_tmp277;});_tmp276;});_tmp275;}));_tmp274->tl=tms;_tmp274;});goto AfterAtts;
_LL2BD:;_LL2BE: goto _LL2B6;_LL2B6:;}AfterAtts: if(_tmp237 != 0)tms=({struct Cyc_List_List*
_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp27A;_tmp27A.tag=3;_tmp27A.f1=_tmp237;_tmp27A.f2=0;_tmp27A.f3=1;_tmp27A;});
_tmp279;}));_tmp278->tl=tms;_tmp278;});return({struct _tuple6 _tmp27B;_tmp27B.f1=
_tmp268;_tmp27B.f2=_tmp269;_tmp27B.f3=tms;_tmp27B;});}}_LL2A7: if(_tmp22C <= (void*)
3?1:*((int*)_tmp22C)!= 0)goto _LL2A9;_tmp23F=((struct Cyc_Absyn_Evar_struct*)
_tmp22C)->f1;_tmp240=((struct Cyc_Absyn_Evar_struct*)_tmp22C)->f2;_tmp241=((
struct Cyc_Absyn_Evar_struct*)_tmp22C)->f3;_LL2A8: if(_tmp240 == 0)return({struct
_tuple6 _tmp27C;_tmp27C.f1=tq;_tmp27C.f2=t;_tmp27C.f3=0;_tmp27C;});else{return Cyc_Absynpp_to_tms(
tq,(void*)_tmp240->v);}_LL2A9: if(_tmp22C <= (void*)3?1:*((int*)_tmp22C)!= 16)goto
_LL2AB;_tmp242=((struct Cyc_Absyn_TypedefType_struct*)_tmp22C)->f1;_tmp243=((
struct Cyc_Absyn_TypedefType_struct*)_tmp22C)->f2;_tmp244=((struct Cyc_Absyn_TypedefType_struct*)
_tmp22C)->f3;_LL2AA: if((_tmp244 == 0?1: !Cyc_Absynpp_expand_typedefs)?1: Cyc_Absynpp_is_anon_aggrtype((
void*)_tmp244->v))return({struct _tuple6 _tmp27D;_tmp27D.f1=tq;_tmp27D.f2=t;
_tmp27D.f3=0;_tmp27D;});else{return Cyc_Absynpp_to_tms(tq,(void*)_tmp244->v);}
_LL2AB:;_LL2AC: return({struct _tuple6 _tmp27E;_tmp27E.f1=tq;_tmp27E.f2=t;_tmp27E.f3=
0;_tmp27E;});_LL2A0:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_Core_Opt*dopt){struct Cyc_Absyn_Tqual _tmp280;void*_tmp281;
struct Cyc_List_List*_tmp282;struct _tuple6 _tmp27F=Cyc_Absynpp_to_tms(tq,t);
_tmp280=_tmp27F.f1;_tmp281=_tmp27F.f2;_tmp282=_tmp27F.f3;_tmp282=Cyc_List_imp_rev(
_tmp282);if(_tmp282 == 0?dopt == 0: 0)return({struct Cyc_PP_Doc*_tmp283[2];_tmp283[1]=
Cyc_Absynpp_ntyp2doc(_tmp281);_tmp283[0]=Cyc_Absynpp_tqual2doc(_tmp280);Cyc_PP_cat(
_tag_arr(_tmp283,sizeof(struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*
_tmp284[4];_tmp284[3]=Cyc_Absynpp_dtms2doc(dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)
dopt->v,_tmp282);_tmp284[2]=Cyc_PP_text(_tag_arr(" ",sizeof(unsigned char),2));
_tmp284[1]=Cyc_Absynpp_ntyp2doc(_tmp281);_tmp284[0]=Cyc_Absynpp_tqual2doc(
_tmp280);Cyc_PP_cat(_tag_arr(_tmp284,sizeof(struct Cyc_PP_Doc*),4));});}}struct
Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){if(f->width != 
0)return({struct Cyc_PP_Doc*_tmp285[5];_tmp285[4]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp285[3]=Cyc_Absynpp_atts2doc(f->attributes);_tmp285[
2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width));_tmp285[1]=
Cyc_PP_text(_tag_arr(":",sizeof(unsigned char),2));_tmp285[0]=Cyc_Absynpp_tqtd2doc(
f->tq,(void*)f->type,({struct Cyc_Core_Opt*_tmp286=_cycalloc(sizeof(*_tmp286));
_tmp286->v=Cyc_PP_textptr(f->name);_tmp286;}));Cyc_PP_cat(_tag_arr(_tmp285,
sizeof(struct Cyc_PP_Doc*),5));});else{return({struct Cyc_PP_Doc*_tmp287[3];
_tmp287[2]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmp287[1]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp287[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp288=_cycalloc(sizeof(*_tmp288));_tmp288->v=Cyc_PP_textptr(f->name);_tmp288;}));
Cyc_PP_cat(_tag_arr(_tmp287,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _tagged_arr sep,struct
Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tag_arr("",sizeof(
unsigned char),1),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct
Cyc_Absyn_Tunionfield*f){return({struct Cyc_PP_Doc*_tmp289[3];_tmp289[2]=f->typs
== 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs);_tmp289[1]=Cyc_Absynpp_typedef_name2doc(
f->name);_tmp289[0]=Cyc_Absynpp_scope2doc((void*)f->sc);Cyc_PP_cat(_tag_arr(
_tmp289,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Tunionfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_tunionfield2doc,_tag_arr(",",sizeof(unsigned char),2),fields);}void
Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)
tdl->hd),72,f);({void*_tmp28A[0]={};Cyc_Std_fprintf(f,_tag_arr("\n",sizeof(
unsigned char),2),_tag_arr(_tmp28A,sizeof(void*),0));});}}struct _tagged_arr Cyc_Absynpp_decllist2string(
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
