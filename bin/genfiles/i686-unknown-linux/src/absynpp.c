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
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern struct Cyc_List_List*Cyc_List_map(void*(*f)(
void*),struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(
void*,void*),void*env,struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);extern struct
Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern
unsigned char Cyc_List_Nth[8];extern int Cyc_List_exists(int(*pred)(void*),struct
Cyc_List_List*x);extern int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2);extern int Cyc_List_list_prefix(int(*cmp)(void*,void*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;extern void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc_Std___sFILE*
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
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_TunionType_struct{
int tag;struct Cyc_Absyn_TunionInfo f1;};struct Cyc_Absyn_TunionFieldType_struct{int
tag;struct Cyc_Absyn_TunionFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int
tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;void*f1;struct Cyc_Absyn_Tqual f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_FnType_struct{
int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_StructType_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};struct Cyc_Absyn_UnionType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};
struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_AnonStructType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
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
};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_StructMember_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_StructArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
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
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Struct_p_struct{int tag;
struct Cyc_Absyn_Structdecl*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;};struct Cyc_Absyn_Tunion_p_struct{int tag;struct Cyc_Absyn_Tuniondecl*
f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;
};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_Absyn_UnknownFields_p_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;
struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*
cnst_exp;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
void*sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structfield{
struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Struct_d_struct{int tag;struct Cyc_Absyn_Structdecl*
f1;};struct Cyc_Absyn_Union_d_struct{int tag;struct Cyc_Absyn_Uniondecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
15];extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();extern struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);extern void*Cyc_Absyn_compress_kb(
void*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct
Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);extern struct _tagged_arr Cyc_Absyn_attribute2string(
void*);struct Cyc_Buffer_t;extern int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct
_tagged_arr*s2);extern struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,
struct _tagged_arr);extern unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];
struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_StructRes_struct{
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
le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_compress(
void*t);extern int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);extern void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{int expand_typedefs: 1;int
qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc_Std___sFILE*f);extern struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d);extern struct _tagged_arr Cyc_Absynpp_typ2string(void*);
extern struct _tagged_arr Cyc_Absynpp_typ2cstring(void*);extern struct _tagged_arr Cyc_Absynpp_kind2string(
void*);extern struct _tagged_arr Cyc_Absynpp_kindbound2string(void*);extern struct
_tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);extern struct _tagged_arr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*);extern struct _tagged_arr Cyc_Absynpp_decllist2string(struct Cyc_List_List*
tdl);extern struct _tagged_arr Cyc_Absynpp_prim2string(void*p);extern struct
_tagged_arr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);extern struct _tagged_arr
Cyc_Absynpp_scope2string(void*sc);extern int Cyc_Absynpp_is_anon_structtype(void*t);
extern struct _tagged_arr Cyc_Absynpp_cyc_string;extern struct _tagged_arr*Cyc_Absynpp_cyc_stringptr;
extern int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);extern struct _tagged_arr Cyc_Absynpp_char_escape(
unsigned char);extern struct _tagged_arr Cyc_Absynpp_string_escape(struct
_tagged_arr);extern struct _tagged_arr Cyc_Absynpp_prim2str(void*p);extern int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);struct _tuple3{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};extern struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple3*arg);
struct _tuple4{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};extern
struct _tuple4 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);struct _tuple5{
struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};extern struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(
struct _tuple5*dp);extern struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*
cs);extern struct Cyc_PP_Doc*Cyc_Absynpp_switchCclauses2doc(struct Cyc_List_List*cs);
extern struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);extern struct Cyc_PP_Doc*Cyc_Absynpp_structfields2doc(
struct Cyc_List_List*fields);extern struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*);
extern struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*);extern struct
Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);extern struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);extern struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(
int inprec,struct Cyc_List_List*es);extern struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*);extern struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct
_tuple0*);extern struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*);extern struct Cyc_PP_Doc*
Cyc_Absynpp_prim2doc(void*);extern struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int
inprec,void*p,struct Cyc_List_List*es);struct _tuple6{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};extern struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple6*
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
x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));}struct _tuple7{struct Cyc_Absyn_Tqual
f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple7*t){return Cyc_Absynpp_tqtd2doc((*
t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return Cyc_PP_group(_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(
unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmp15=att;
_LL57: if((int)_tmp15 == 0){goto _LL58;}else{goto _LL59;}_LL59: if((int)_tmp15 == 1){
goto _LL5A;}else{goto _LL5B;}_LL5B: if((int)_tmp15 == 2){goto _LL5C;}else{goto _LL5D;}
_LL5D: goto _LL5E;_LL58: return Cyc_PP_nil_doc();_LL5A: return Cyc_PP_nil_doc();_LL5C:
return Cyc_PP_nil_doc();_LL5E: return Cyc_PP_text(Cyc_Absyn_attribute2string(att));
_LL56:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(
0;atts != 0;atts=atts->tl){void*_tmp16=(void*)atts->hd;_LL60: if((int)_tmp16 == 0){
goto _LL61;}else{goto _LL62;}_LL62: if((int)_tmp16 == 1){goto _LL63;}else{goto _LL64;}
_LL64: if((int)_tmp16 == 2){goto _LL65;}else{goto _LL66;}_LL66: goto _LL67;_LL61:
return Cyc_PP_text(_tag_arr(" _stdcall ",sizeof(unsigned char),11));_LL63: return
Cyc_PP_text(_tag_arr(" _cdecl ",sizeof(unsigned char),9));_LL65: return Cyc_PP_text(
_tag_arr(" _fastcall ",sizeof(unsigned char),12));_LL67: goto _LL5F;_LL5F:;}return
Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp17=(void*)atts2->hd;_LL69: if((int)_tmp17 == 0){goto _LL6A;}else{goto _LL6B;}
_LL6B: if((int)_tmp17 == 1){goto _LL6C;}else{goto _LL6D;}_LL6D: if((int)_tmp17 == 2){
goto _LL6E;}else{goto _LL6F;}_LL6F: goto _LL70;_LL6A: goto _LL68;_LL6C: goto _LL68;_LL6E:
goto _LL68;_LL70: hasatt=1;goto _LL68;_LL68:;}}if(! hasatt){return Cyc_PP_nil_doc();}
return({struct Cyc_PP_Doc*_tmp18[3];_tmp18[2]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp18[1]=Cyc_PP_group(_tag_arr("",sizeof(unsigned char),1),
_tag_arr("",sizeof(unsigned char),1),_tag_arr(" ",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_noncallatt2doc,atts));_tmp18[0]=Cyc_PP_text(_tag_arr("__declspec(",
sizeof(unsigned char),12));Cyc_PP_cat(_tag_arr(_tmp18,sizeof(struct Cyc_PP_Doc*),
3));});}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(
a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){if(atts == 0){
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
int*)_tmp3F)== 19: 0){_LLB4: _tmp40=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp3F)->f1;goto _LLAF;}else{goto _LLB0;}_LLB0: if((unsigned int)_tmp3F > 3?*((int*)
_tmp3F)== 20: 0){_LLB5: _tmp41=((struct Cyc_Absyn_JoinEff_struct*)_tmp3F)->f1;goto
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
_tmp45));_tmp45->hd=_tmp44;_tmp45->tl=0;_tmp45;})));}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(
void*t){struct Cyc_PP_Doc*s;{void*_tmp46=t;struct Cyc_Core_Opt*_tmp47;int _tmp48;
struct Cyc_Core_Opt*_tmp49;struct Cyc_Core_Opt*_tmp4A;struct Cyc_Absyn_Tvar*_tmp4B;
struct Cyc_Absyn_TunionInfo _tmp4C;void*_tmp4D;struct Cyc_List_List*_tmp4E;void*
_tmp4F;struct Cyc_Absyn_TunionFieldInfo _tmp50;struct Cyc_List_List*_tmp51;void*
_tmp52;void*_tmp53;void*_tmp54;int _tmp55;struct Cyc_List_List*_tmp56;struct Cyc_List_List*
_tmp57;struct _tuple0*_tmp58;struct Cyc_List_List*_tmp59;struct _tuple0*_tmp5A;
struct Cyc_List_List*_tmp5B;struct Cyc_List_List*_tmp5C;struct Cyc_List_List*_tmp5D;
struct _tuple0*_tmp5E;void*_tmp5F;struct Cyc_Core_Opt*_tmp60;struct Cyc_List_List*
_tmp61;struct _tuple0*_tmp62;void*_tmp63;void*_tmp64;_LLB7: if((unsigned int)
_tmp46 > 3?*((int*)_tmp46)== 7: 0){goto _LLB8;}else{goto _LLB9;}_LLB9: if((
unsigned int)_tmp46 > 3?*((int*)_tmp46)== 8: 0){goto _LLBA;}else{goto _LLBB;}_LLBB:
if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 4: 0){goto _LLBC;}else{goto _LLBD;}
_LLBD: if((int)_tmp46 == 0){goto _LLBE;}else{goto _LLBF;}_LLBF: if((unsigned int)
_tmp46 > 3?*((int*)_tmp46)== 0: 0){_LLEC: _tmp4A=((struct Cyc_Absyn_Evar_struct*)
_tmp46)->f1;goto _LLEB;_LLEB: _tmp49=((struct Cyc_Absyn_Evar_struct*)_tmp46)->f2;
goto _LLEA;_LLEA: _tmp48=((struct Cyc_Absyn_Evar_struct*)_tmp46)->f3;goto _LLE9;
_LLE9: _tmp47=((struct Cyc_Absyn_Evar_struct*)_tmp46)->f4;goto _LLC0;}else{goto
_LLC1;}_LLC1: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 1: 0){_LLED: _tmp4B=((
struct Cyc_Absyn_VarType_struct*)_tmp46)->f1;goto _LLC2;}else{goto _LLC3;}_LLC3: if((
unsigned int)_tmp46 > 3?*((int*)_tmp46)== 2: 0){_LLEE: _tmp4C=((struct Cyc_Absyn_TunionType_struct*)
_tmp46)->f1;_LLF1: _tmp4F=(void*)_tmp4C.tunion_info;goto _LLF0;_LLF0: _tmp4E=_tmp4C.targs;
goto _LLEF;_LLEF: _tmp4D=(void*)_tmp4C.rgn;goto _LLC4;}else{goto _LLC5;}_LLC5: if((
unsigned int)_tmp46 > 3?*((int*)_tmp46)== 3: 0){_LLF2: _tmp50=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp46)->f1;_LLF4: _tmp52=(void*)_tmp50.field_info;goto _LLF3;_LLF3: _tmp51=_tmp50.targs;
goto _LLC6;}else{goto _LLC7;}_LLC7: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 5:
0){_LLF6: _tmp54=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f1;goto _LLF5;
_LLF5: _tmp53=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp46)->f2;goto _LLC8;}
else{goto _LLC9;}_LLC9: if((int)_tmp46 == 1){goto _LLCA;}else{goto _LLCB;}_LLCB: if((
unsigned int)_tmp46 > 3?*((int*)_tmp46)== 6: 0){_LLF7: _tmp55=((struct Cyc_Absyn_DoubleType_struct*)
_tmp46)->f1;goto _LLCC;}else{goto _LLCD;}_LLCD: if((unsigned int)_tmp46 > 3?*((int*)
_tmp46)== 9: 0){_LLF8: _tmp56=((struct Cyc_Absyn_TupleType_struct*)_tmp46)->f1;goto
_LLCE;}else{goto _LLCF;}_LLCF: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 10: 0){
_LLFA: _tmp58=((struct Cyc_Absyn_StructType_struct*)_tmp46)->f1;goto _LLF9;_LLF9:
_tmp57=((struct Cyc_Absyn_StructType_struct*)_tmp46)->f2;goto _LLD0;}else{goto
_LLD1;}_LLD1: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 11: 0){_LLFC: _tmp5A=((
struct Cyc_Absyn_UnionType_struct*)_tmp46)->f1;goto _LLFB;_LLFB: _tmp59=((struct Cyc_Absyn_UnionType_struct*)
_tmp46)->f2;goto _LLD2;}else{goto _LLD3;}_LLD3: if((unsigned int)_tmp46 > 3?*((int*)
_tmp46)== 14: 0){_LLFD: _tmp5B=((struct Cyc_Absyn_AnonStructType_struct*)_tmp46)->f1;
goto _LLD4;}else{goto _LLD5;}_LLD5: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 15:
0){_LLFE: _tmp5C=((struct Cyc_Absyn_AnonUnionType_struct*)_tmp46)->f1;goto _LLD6;}
else{goto _LLD7;}_LLD7: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 16: 0){_LLFF:
_tmp5D=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp46)->f1;goto _LLD8;}else{goto
_LLD9;}_LLD9: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 12: 0){_LL100: _tmp5E=((
struct Cyc_Absyn_EnumType_struct*)_tmp46)->f1;goto _LLDA;}else{goto _LLDB;}_LLDB:
if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 13: 0){_LL101: _tmp5F=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp46)->f1;goto _LLDC;}else{goto _LLDD;}_LLDD: if((
unsigned int)_tmp46 > 3?*((int*)_tmp46)== 18: 0){_LL104: _tmp62=((struct Cyc_Absyn_TypedefType_struct*)
_tmp46)->f1;goto _LL103;_LL103: _tmp61=((struct Cyc_Absyn_TypedefType_struct*)
_tmp46)->f2;goto _LL102;_LL102: _tmp60=((struct Cyc_Absyn_TypedefType_struct*)
_tmp46)->f3;goto _LLDE;}else{goto _LLDF;}_LLDF: if((unsigned int)_tmp46 > 3?*((int*)
_tmp46)== 17: 0){_LL105: _tmp63=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp46)->f1;goto _LLE0;}else{goto _LLE1;}_LLE1: if((int)_tmp46 == 2){goto _LLE2;}
else{goto _LLE3;}_LLE3: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 21: 0){_LL106:
_tmp64=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp46)->f1;goto _LLE4;}else{goto
_LLE5;}_LLE5: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 19: 0){goto _LLE6;}else{
goto _LLE7;}_LLE7: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 20: 0){goto _LLE8;}
else{goto _LLB6;}_LLB8: return Cyc_PP_text(_tag_arr("[[[array]]]",sizeof(
unsigned char),12));_LLBA: return Cyc_PP_nil_doc();_LLBC: return Cyc_PP_nil_doc();
_LLBE: s=Cyc_PP_text(_tag_arr("void",sizeof(unsigned char),5));goto _LLB6;_LLC0:
if(_tmp49 != 0){return Cyc_Absynpp_ntyp2doc((void*)_tmp49->v);}else{s=({struct Cyc_PP_Doc*
_tmp65[6];_tmp65[5]=_tmp4A == 0? Cyc_PP_text(_tag_arr("?",sizeof(unsigned char),2)):
Cyc_Absynpp_kind2doc((void*)_tmp4A->v);_tmp65[4]=Cyc_PP_text(_tag_arr(")::",
sizeof(unsigned char),4));_tmp65[3]=(! Cyc_Absynpp_print_full_evars? 1: _tmp47 == 0)?
Cyc_PP_text(_tag_arr("",sizeof(unsigned char),1)): Cyc_Absynpp_tvars2doc((struct
Cyc_List_List*)_tmp47->v);_tmp65[2]=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp67;_tmp67.tag=1;_tmp67.f1=(int)((unsigned int)_tmp48);{void*_tmp66[1]={&
_tmp67};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp66,
sizeof(void*),1));}}));_tmp65[1]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),
2));_tmp65[0]=Cyc_PP_text(_tag_arr("%",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp65,sizeof(struct Cyc_PP_Doc*),6));});}goto _LLB6;_LLC2: s=Cyc_PP_textptr(
_tmp4B->name);if(Cyc_Absynpp_print_all_kinds){s=({struct Cyc_PP_Doc*_tmp68[3];
_tmp68[2]=Cyc_Absynpp_kindbound2doc((void*)_tmp4B->kind);_tmp68[1]=Cyc_PP_text(
_tag_arr("::",sizeof(unsigned char),3));_tmp68[0]=s;Cyc_PP_cat(_tag_arr(_tmp68,
sizeof(struct Cyc_PP_Doc*),3));});}if(Cyc_Absynpp_rewrite_temp_tvars? Cyc_Tcutil_is_temp_tvar(
_tmp4B): 0){s=({struct Cyc_PP_Doc*_tmp69[3];_tmp69[2]=Cyc_PP_text(_tag_arr(" */",
sizeof(unsigned char),4));_tmp69[1]=s;_tmp69[0]=Cyc_PP_text(_tag_arr("_ /* ",
sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp69,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LLB6;_LLC4:{void*_tmp6A=_tmp4F;struct Cyc_Absyn_UnknownTunionInfo _tmp6B;int
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
_tmp4D);_LL113: if((int)_tmp74 == 2){goto _LL114;}else{goto _LL115;}_LL115: goto
_LL116;_LL114: s=({struct Cyc_PP_Doc*_tmp75[3];_tmp75[2]=Cyc_Absynpp_tps2doc(
_tmp4E);_tmp75[1]=Cyc_Absynpp_qvar2doc(_tmp72);_tmp75[0]=_tmp73;Cyc_PP_cat(
_tag_arr(_tmp75,sizeof(struct Cyc_PP_Doc*),3));});goto _LL112;_LL116: s=({struct Cyc_PP_Doc*
_tmp76[5];_tmp76[4]=Cyc_Absynpp_tps2doc(_tmp4E);_tmp76[3]=Cyc_Absynpp_qvar2doc(
_tmp72);_tmp76[2]=Cyc_PP_text(_tag_arr(" ",sizeof(unsigned char),2));_tmp76[1]=
Cyc_Absynpp_typ2doc(_tmp4D);_tmp76[0]=_tmp73;Cyc_PP_cat(_tag_arr(_tmp76,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL112;_LL112:;}goto _LL107;}_LL107:;}goto _LLB6;
_LLC6:{void*_tmp77=_tmp52;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp78;int _tmp79;
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
struct Cyc_PP_Doc*),4));});goto _LL117;}_LL117:;}goto _LLB6;_LLC8: {struct
_tagged_arr sns;struct _tagged_arr ts;{void*_tmp85=_tmp54;_LL126: if((int)_tmp85 == 0){
goto _LL127;}else{goto _LL128;}_LL128: if((int)_tmp85 == 1){goto _LL129;}else{goto
_LL125;}_LL127: sns=_tag_arr("",sizeof(unsigned char),1);goto _LL125;_LL129: sns=
_tag_arr("unsigned ",sizeof(unsigned char),10);goto _LL125;_LL125:;}{void*_tmp86=
_tmp53;_LL12B: if((int)_tmp86 == 0){goto _LL12C;}else{goto _LL12D;}_LL12D: if((int)
_tmp86 == 1){goto _LL12E;}else{goto _LL12F;}_LL12F: if((int)_tmp86 == 2){goto _LL130;}
else{goto _LL131;}_LL131: if((int)_tmp86 == 3){goto _LL132;}else{goto _LL12A;}_LL12C:{
void*_tmp87=_tmp54;_LL134: if((int)_tmp87 == 0){goto _LL135;}else{goto _LL136;}
_LL136: if((int)_tmp87 == 1){goto _LL137;}else{goto _LL133;}_LL135: sns=_tag_arr("signed ",
sizeof(unsigned char),8);goto _LL133;_LL137: goto _LL133;_LL133:;}ts=_tag_arr("char",
sizeof(unsigned char),5);goto _LL12A;_LL12E: ts=_tag_arr("short",sizeof(
unsigned char),6);goto _LL12A;_LL130: ts=_tag_arr("int",sizeof(unsigned char),4);
goto _LL12A;_LL132: ts=Cyc_Absynpp_to_VC? _tag_arr("__int64",sizeof(unsigned char),
8): _tag_arr("long long",sizeof(unsigned char),10);goto _LL12A;_LL12A:;}s=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp8A;_tmp8A.tag=0;_tmp8A.f1=(
struct _tagged_arr)ts;{struct Cyc_Std_String_pa_struct _tmp89;_tmp89.tag=0;_tmp89.f1=(
struct _tagged_arr)sns;{void*_tmp88[2]={& _tmp89,& _tmp8A};Cyc_Std_aprintf(_tag_arr("%s%s",
sizeof(unsigned char),5),_tag_arr(_tmp88,sizeof(void*),2));}}}));goto _LLB6;}
_LLCA: s=Cyc_PP_text(_tag_arr("float",sizeof(unsigned char),6));goto _LLB6;_LLCC:
if(_tmp55){s=Cyc_PP_text(_tag_arr("long double",sizeof(unsigned char),12));}
else{s=Cyc_PP_text(_tag_arr("double",sizeof(unsigned char),7));}goto _LLB6;_LLCE:
s=({struct Cyc_PP_Doc*_tmp8B[2];_tmp8B[1]=Cyc_Absynpp_args2doc(_tmp56);_tmp8B[0]=
Cyc_PP_text(_tag_arr("$",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp8B,
sizeof(struct Cyc_PP_Doc*),2));});goto _LLB6;_LLD0: if(_tmp58 == 0){s=({struct Cyc_PP_Doc*
_tmp8C[2];_tmp8C[1]=Cyc_Absynpp_tps2doc(_tmp57);_tmp8C[0]=Cyc_PP_text(_tag_arr("struct ",
sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp8C,sizeof(struct Cyc_PP_Doc*),2));});}
else{s=({struct Cyc_PP_Doc*_tmp8D[3];_tmp8D[2]=Cyc_Absynpp_tps2doc(_tmp57);_tmp8D[
1]=_tmp58 == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((struct _tuple0*)_check_null(
_tmp58));_tmp8D[0]=Cyc_PP_text(_tag_arr("struct ",sizeof(unsigned char),8));Cyc_PP_cat(
_tag_arr(_tmp8D,sizeof(struct Cyc_PP_Doc*),3));});}goto _LLB6;_LLD2: if(_tmp5A == 0){
s=({struct Cyc_PP_Doc*_tmp8E[2];_tmp8E[1]=Cyc_Absynpp_tps2doc(_tmp59);_tmp8E[0]=
Cyc_PP_text(_tag_arr("union ",sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(
_tmp8E,sizeof(struct Cyc_PP_Doc*),2));});}else{s=({struct Cyc_PP_Doc*_tmp8F[3];
_tmp8F[2]=Cyc_Absynpp_tps2doc(_tmp59);_tmp8F[1]=_tmp5A == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_qvar2doc((
struct _tuple0*)_check_null(_tmp5A));_tmp8F[0]=Cyc_PP_text(_tag_arr("union ",
sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(_tmp8F,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LLB6;_LLD4: s=({struct Cyc_PP_Doc*_tmp90[3];_tmp90[2]=Cyc_PP_text(_tag_arr("}",
sizeof(unsigned char),2));_tmp90[1]=Cyc_PP_nest(2,Cyc_Absynpp_structfields2doc(
_tmp5B));_tmp90[0]=Cyc_PP_text(_tag_arr("struct {",sizeof(unsigned char),9));Cyc_PP_cat(
_tag_arr(_tmp90,sizeof(struct Cyc_PP_Doc*),3));});goto _LLB6;_LLD6: s=({struct Cyc_PP_Doc*
_tmp91[3];_tmp91[2]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp91[1]=
Cyc_PP_nest(2,Cyc_Absynpp_structfields2doc(_tmp5C));_tmp91[0]=Cyc_PP_text(
_tag_arr("union {",sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp91,sizeof(
struct Cyc_PP_Doc*),3));});goto _LLB6;_LLD8: s=({struct Cyc_PP_Doc*_tmp92[3];_tmp92[
2]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp92[1]=Cyc_PP_nest(2,
Cyc_Absynpp_enumfields2doc(_tmp5D));_tmp92[0]=Cyc_PP_text(_tag_arr("enum {",
sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(_tmp92,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLB6;_LLDA: s=({struct Cyc_PP_Doc*_tmp93[2];_tmp93[1]=Cyc_Absynpp_qvar2doc(
_tmp5E);_tmp93[0]=Cyc_PP_text(_tag_arr("enum ",sizeof(unsigned char),6));Cyc_PP_cat(
_tag_arr(_tmp93,sizeof(struct Cyc_PP_Doc*),2));});goto _LLB6;_LLDC: s=({struct Cyc_PP_Doc*
_tmp94[3];_tmp94[2]=Cyc_PP_text(_tag_arr(">",sizeof(unsigned char),2));_tmp94[1]=
Cyc_Absynpp_typ2doc(_tmp5F);_tmp94[0]=Cyc_PP_text(_tag_arr("sizeof_t<",sizeof(
unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp94,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLB6;_LLDE: s=({struct Cyc_PP_Doc*_tmp95[2];_tmp95[1]=Cyc_Absynpp_tps2doc(
_tmp61);_tmp95[0]=Cyc_Absynpp_qvar2doc(_tmp62);Cyc_PP_cat(_tag_arr(_tmp95,
sizeof(struct Cyc_PP_Doc*),2));});goto _LLB6;_LLE0: s=({struct Cyc_PP_Doc*_tmp96[3];
_tmp96[2]=Cyc_PP_text(_tag_arr(">",sizeof(unsigned char),2));_tmp96[1]=Cyc_Absynpp_rgn2doc(
_tmp63);_tmp96[0]=Cyc_PP_text(_tag_arr("region_t<",sizeof(unsigned char),10));
Cyc_PP_cat(_tag_arr(_tmp96,sizeof(struct Cyc_PP_Doc*),3));});goto _LLB6;_LLE2: s=
Cyc_Absynpp_rgn2doc(t);goto _LLB6;_LLE4: s=({struct Cyc_PP_Doc*_tmp97[3];_tmp97[2]=
Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp97[1]=Cyc_Absynpp_typ2doc(
_tmp64);_tmp97[0]=Cyc_PP_text(_tag_arr("regions(",sizeof(unsigned char),9));Cyc_PP_cat(
_tag_arr(_tmp97,sizeof(struct Cyc_PP_Doc*),3));});goto _LLB6;_LLE6: goto _LLE8;_LLE8:
s=Cyc_Absynpp_eff2doc(t);goto _LLB6;_LLB6:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo){return vo == 0? Cyc_PP_nil_doc(): Cyc_PP_text(*((struct
_tagged_arr*)vo->v));}struct _tuple8{void*f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(
struct _tuple8*cmp){struct _tuple8 _tmp99;void*_tmp9A;void*_tmp9B;struct _tuple8*
_tmp98=cmp;_tmp99=*_tmp98;_LL13A: _tmp9B=_tmp99.f1;goto _LL139;_LL139: _tmp9A=
_tmp99.f2;goto _LL138;_LL138: return({struct Cyc_PP_Doc*_tmp9C[3];_tmp9C[2]=Cyc_Absynpp_rgn2doc(
_tmp9A);_tmp9C[1]=Cyc_PP_text(_tag_arr(" < ",sizeof(unsigned char),4));_tmp9C[0]=
Cyc_Absynpp_rgn2doc(_tmp9B);Cyc_PP_cat(_tag_arr(_tmp9C,sizeof(struct Cyc_PP_Doc*),
3));});}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){return Cyc_PP_group(
_tag_arr("",sizeof(unsigned char),1),_tag_arr("",sizeof(unsigned char),1),
_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,
po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple1*t){struct Cyc_Core_Opt*
dopt=(*t).f1 == 0? 0:({struct Cyc_Core_Opt*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->v=
Cyc_PP_text(*((struct _tagged_arr*)((struct Cyc_Core_Opt*)_check_null((*t).f1))->v));
_tmp9D;});return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp9E=((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_PP_Doc*eff_doc;if(
c_varargs){_tmp9E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp9E,({struct Cyc_List_List*_tmp9F=_cycalloc(sizeof(*_tmp9F));
_tmp9F->hd=Cyc_PP_text(_tag_arr("...",sizeof(unsigned char),4));_tmp9F->tl=0;
_tmp9F;}));}else{if(cyc_varargs != 0){struct Cyc_PP_Doc*_tmpA0=({struct Cyc_PP_Doc*
_tmpA2[3];_tmpA2[2]=Cyc_Absynpp_funarg2doc(({struct _tuple1*_tmpA3=_cycalloc(
sizeof(*_tmpA3));_tmpA3->f1=cyc_varargs->name;_tmpA3->f2=cyc_varargs->tq;_tmpA3->f3=(
void*)cyc_varargs->type;_tmpA3;}));_tmpA2[1]=cyc_varargs->inject? Cyc_PP_text(
_tag_arr(" inject ",sizeof(unsigned char),9)): Cyc_PP_text(_tag_arr(" ",sizeof(
unsigned char),2));_tmpA2[0]=Cyc_PP_text(_tag_arr("...",sizeof(unsigned char),4));
Cyc_PP_cat(_tag_arr(_tmpA2,sizeof(struct Cyc_PP_Doc*),3));});_tmp9E=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9E,({struct Cyc_List_List*
_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->hd=_tmpA0;_tmpA1->tl=0;_tmpA1;}));}}{
struct Cyc_PP_Doc*_tmpA4=Cyc_PP_group(_tag_arr("",sizeof(unsigned char),1),
_tag_arr("",sizeof(unsigned char),1),_tag_arr(",",sizeof(unsigned char),2),
_tmp9E);if(effopt != 0){_tmpA4=({struct Cyc_PP_Doc*_tmpA5[3];_tmpA5[2]=Cyc_Absynpp_eff2doc((
void*)effopt->v);_tmpA5[1]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));
_tmpA5[0]=_tmpA4;Cyc_PP_cat(_tag_arr(_tmpA5,sizeof(struct Cyc_PP_Doc*),3));});}
if(rgn_po != 0){_tmpA4=({struct Cyc_PP_Doc*_tmpA6[3];_tmpA6[2]=Cyc_Absynpp_rgnpo2doc(
rgn_po);_tmpA6[1]=Cyc_PP_text(_tag_arr(":",sizeof(unsigned char),2));_tmpA6[0]=
_tmpA4;Cyc_PP_cat(_tag_arr(_tmpA6,sizeof(struct Cyc_PP_Doc*),3));});}return({
struct Cyc_PP_Doc*_tmpA7[3];_tmpA7[2]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmpA7[1]=_tmpA4;_tmpA7[0]=Cyc_PP_text(_tag_arr("(",sizeof(
unsigned char),2));Cyc_PP_cat(_tag_arr(_tmpA7,sizeof(struct Cyc_PP_Doc*),3));});}}
struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple3*arg){return({struct _tuple1*
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->f1=({struct Cyc_Core_Opt*_tmpA9=
_cycalloc(sizeof(*_tmpA9));_tmpA9->v=(*arg).f1;_tmpA9;});_tmpA8->f2=(*arg).f2;
_tmpA8->f3=(*arg).f3;_tmpA8;});}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_tagged_arr*v){return Cyc_PP_text(*v);}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q){struct Cyc_List_List*_tmpAA=0;int match;{void*_tmpAB=(*q).f1;
struct Cyc_List_List*_tmpAC;struct Cyc_List_List*_tmpAD;_LL13C: if((int)_tmpAB == 0){
goto _LL13D;}else{goto _LL13E;}_LL13E: if((unsigned int)_tmpAB > 1?*((int*)_tmpAB)== 
0: 0){_LL142: _tmpAC=((struct Cyc_Absyn_Rel_n_struct*)_tmpAB)->f1;goto _LL13F;}else{
goto _LL140;}_LL140: if((unsigned int)_tmpAB > 1?*((int*)_tmpAB)== 1: 0){_LL143:
_tmpAD=((struct Cyc_Absyn_Abs_n_struct*)_tmpAB)->f1;goto _LL141;}else{goto _LL13B;}
_LL13D: _tmpAC=0;goto _LL13F;_LL13F: match=0;_tmpAA=_tmpAC;goto _LL13B;_LL141: match=
Cyc_Absynpp_use_curr_namespace?((int(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(
Cyc_Std_strptrcmp,_tmpAD,Cyc_Absynpp_curr_namespace): 0;_tmpAA=(Cyc_Absynpp_qvar_to_Cids?
Cyc_Absynpp_add_cyc_prefix: 0)?({struct Cyc_List_List*_tmpAE=_cycalloc(sizeof(*
_tmpAE));_tmpAE->hd=Cyc_Absynpp_cyc_stringptr;_tmpAE->tl=_tmpAD;_tmpAE;}): _tmpAD;
goto _LL13B;_LL13B:;}if(Cyc_Absynpp_qvar_to_Cids){return Cyc_PP_text((struct
_tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(_tmpAA,({struct Cyc_List_List*_tmpAF=
_cycalloc(sizeof(*_tmpAF));_tmpAF->hd=(*q).f2;_tmpAF->tl=0;_tmpAF;})),_tag_arr("_",
sizeof(unsigned char),2)));}else{if(match){return Cyc_Absynpp_var2doc((*q).f2);}
else{return Cyc_PP_text((struct _tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpAA,({struct Cyc_List_List*
_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->hd=(*q).f2;_tmpB0->tl=0;_tmpB0;})),
_tag_arr("::",sizeof(unsigned char),3)));}}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids){return Cyc_Absynpp_qvar2doc(v);}if(
Cyc_Absynpp_use_curr_namespace){void*_tmpB1=(*v).f1;struct Cyc_List_List*_tmpB2;
struct Cyc_List_List*_tmpB3;_LL145: if((int)_tmpB1 == 0){goto _LL146;}else{goto
_LL147;}_LL147: if((unsigned int)_tmpB1 > 1?*((int*)_tmpB1)== 0: 0){_LL14D: _tmpB2=((
struct Cyc_Absyn_Rel_n_struct*)_tmpB1)->f1;if(_tmpB2 == 0){goto _LL148;}else{goto
_LL149;}}else{goto _LL149;}_LL149: if((unsigned int)_tmpB1 > 1?*((int*)_tmpB1)== 1:
0){_LL14E: _tmpB3=((struct Cyc_Absyn_Abs_n_struct*)_tmpB1)->f1;goto _LL14A;}else{
goto _LL14B;}_LL14B: goto _LL14C;_LL146: goto _LL148;_LL148: return Cyc_Absynpp_var2doc((*
v).f2);_LL14A: if(((int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Std_strptrcmp,
_tmpB3,Cyc_Absynpp_curr_namespace)== 0){return Cyc_Absynpp_var2doc((*v).f2);}
else{goto _LL14C;}_LL14C: return({struct Cyc_PP_Doc*_tmpB4[2];_tmpB4[1]=Cyc_Absynpp_qvar2doc(
v);_tmpB4[0]=Cyc_PP_text(_tag_arr("/* bad namespace : */ ",sizeof(unsigned char),
23));Cyc_PP_cat(_tag_arr(_tmpB4,sizeof(struct Cyc_PP_Doc*),2));});_LL144:;}else{
return Cyc_Absynpp_var2doc((*v).f2);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(),t,0);}int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*e){void*_tmpB5=(void*)e->r;void*_tmpB6;struct Cyc_Absyn_Exp*
_tmpB7;struct Cyc_Absyn_Exp*_tmpB8;_LL150: if(*((int*)_tmpB5)== 0){goto _LL151;}
else{goto _LL152;}_LL152: if(*((int*)_tmpB5)== 1){goto _LL153;}else{goto _LL154;}
_LL154: if(*((int*)_tmpB5)== 2){goto _LL155;}else{goto _LL156;}_LL156: if(*((int*)
_tmpB5)== 3){_LL19C: _tmpB6=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmpB5)->f1;
goto _LL157;}else{goto _LL158;}_LL158: if(*((int*)_tmpB5)== 4){goto _LL159;}else{
goto _LL15A;}_LL15A: if(*((int*)_tmpB5)== 5){goto _LL15B;}else{goto _LL15C;}_LL15C:
if(*((int*)_tmpB5)== 6){goto _LL15D;}else{goto _LL15E;}_LL15E: if(*((int*)_tmpB5)== 
7){goto _LL15F;}else{goto _LL160;}_LL160: if(*((int*)_tmpB5)== 8){goto _LL161;}else{
goto _LL162;}_LL162: if(*((int*)_tmpB5)== 9){goto _LL163;}else{goto _LL164;}_LL164:
if(*((int*)_tmpB5)== 10){goto _LL165;}else{goto _LL166;}_LL166: if(*((int*)_tmpB5)
== 11){_LL19D: _tmpB7=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpB5)->f1;goto
_LL167;}else{goto _LL168;}_LL168: if(*((int*)_tmpB5)== 12){_LL19E: _tmpB8=((struct
Cyc_Absyn_Instantiate_e_struct*)_tmpB5)->f1;goto _LL169;}else{goto _LL16A;}_LL16A:
if(*((int*)_tmpB5)== 13){goto _LL16B;}else{goto _LL16C;}_LL16C: if(*((int*)_tmpB5)
== 15){goto _LL16D;}else{goto _LL16E;}_LL16E: if(*((int*)_tmpB5)== 14){goto _LL16F;}
else{goto _LL170;}_LL170: if(*((int*)_tmpB5)== 16){goto _LL171;}else{goto _LL172;}
_LL172: if(*((int*)_tmpB5)== 17){goto _LL173;}else{goto _LL174;}_LL174: if(*((int*)
_tmpB5)== 18){goto _LL175;}else{goto _LL176;}_LL176: if(*((int*)_tmpB5)== 19){goto
_LL177;}else{goto _LL178;}_LL178: if(*((int*)_tmpB5)== 20){goto _LL179;}else{goto
_LL17A;}_LL17A: if(*((int*)_tmpB5)== 21){goto _LL17B;}else{goto _LL17C;}_LL17C: if(*((
int*)_tmpB5)== 22){goto _LL17D;}else{goto _LL17E;}_LL17E: if(*((int*)_tmpB5)== 23){
goto _LL17F;}else{goto _LL180;}_LL180: if(*((int*)_tmpB5)== 24){goto _LL181;}else{
goto _LL182;}_LL182: if(*((int*)_tmpB5)== 25){goto _LL183;}else{goto _LL184;}_LL184:
if(*((int*)_tmpB5)== 26){goto _LL185;}else{goto _LL186;}_LL186: if(*((int*)_tmpB5)
== 27){goto _LL187;}else{goto _LL188;}_LL188: if(*((int*)_tmpB5)== 28){goto _LL189;}
else{goto _LL18A;}_LL18A: if(*((int*)_tmpB5)== 29){goto _LL18B;}else{goto _LL18C;}
_LL18C: if(*((int*)_tmpB5)== 30){goto _LL18D;}else{goto _LL18E;}_LL18E: if(*((int*)
_tmpB5)== 31){goto _LL18F;}else{goto _LL190;}_LL190: if(*((int*)_tmpB5)== 32){goto
_LL191;}else{goto _LL192;}_LL192: if(*((int*)_tmpB5)== 33){goto _LL193;}else{goto
_LL194;}_LL194: if(*((int*)_tmpB5)== 34){goto _LL195;}else{goto _LL196;}_LL196: if(*((
int*)_tmpB5)== 35){goto _LL197;}else{goto _LL198;}_LL198: if(*((int*)_tmpB5)== 36){
goto _LL199;}else{goto _LL19A;}_LL19A: if(*((int*)_tmpB5)== 37){goto _LL19B;}else{
goto _LL14F;}_LL151: return 10000;_LL153: return 10000;_LL155: return 10000;_LL157: {
void*_tmpB9=_tmpB6;_LL1A0: if((int)_tmpB9 == 0){goto _LL1A1;}else{goto _LL1A2;}
_LL1A2: if((int)_tmpB9 == 1){goto _LL1A3;}else{goto _LL1A4;}_LL1A4: if((int)_tmpB9 == 
2){goto _LL1A5;}else{goto _LL1A6;}_LL1A6: if((int)_tmpB9 == 3){goto _LL1A7;}else{goto
_LL1A8;}_LL1A8: if((int)_tmpB9 == 4){goto _LL1A9;}else{goto _LL1AA;}_LL1AA: if((int)
_tmpB9 == 5){goto _LL1AB;}else{goto _LL1AC;}_LL1AC: if((int)_tmpB9 == 6){goto _LL1AD;}
else{goto _LL1AE;}_LL1AE: if((int)_tmpB9 == 7){goto _LL1AF;}else{goto _LL1B0;}_LL1B0:
if((int)_tmpB9 == 8){goto _LL1B1;}else{goto _LL1B2;}_LL1B2: if((int)_tmpB9 == 9){goto
_LL1B3;}else{goto _LL1B4;}_LL1B4: if((int)_tmpB9 == 10){goto _LL1B5;}else{goto _LL1B6;}
_LL1B6: if((int)_tmpB9 == 11){goto _LL1B7;}else{goto _LL1B8;}_LL1B8: if((int)_tmpB9 == 
12){goto _LL1B9;}else{goto _LL1BA;}_LL1BA: if((int)_tmpB9 == 13){goto _LL1BB;}else{
goto _LL1BC;}_LL1BC: if((int)_tmpB9 == 14){goto _LL1BD;}else{goto _LL1BE;}_LL1BE: if((
int)_tmpB9 == 15){goto _LL1BF;}else{goto _LL1C0;}_LL1C0: if((int)_tmpB9 == 16){goto
_LL1C1;}else{goto _LL1C2;}_LL1C2: if((int)_tmpB9 == 17){goto _LL1C3;}else{goto _LL1C4;}
_LL1C4: if((int)_tmpB9 == 18){goto _LL1C5;}else{goto _LL1C6;}_LL1C6: if((int)_tmpB9 == 
19){goto _LL1C7;}else{goto _LL19F;}_LL1A1: return 100;_LL1A3: return 110;_LL1A5: return
100;_LL1A7: return 110;_LL1A9: return 110;_LL1AB: return 70;_LL1AD: return 70;_LL1AF:
return 80;_LL1B1: return 80;_LL1B3: return 80;_LL1B5: return 80;_LL1B7: return 130;_LL1B9:
return 130;_LL1BB: return 60;_LL1BD: return 40;_LL1BF: return 50;_LL1C1: return 90;_LL1C3:
return 80;_LL1C5: return 80;_LL1C7: return 140;_LL19F:;}_LL159: return 20;_LL15B: return
130;_LL15D: return 30;_LL15F: return 10;_LL161: return 140;_LL163: return 140;_LL165:
return 130;_LL167: return Cyc_Absynpp_exp_prec(_tmpB7);_LL169: return Cyc_Absynpp_exp_prec(
_tmpB8);_LL16B: return 120;_LL16D: return 15;_LL16F: return 130;_LL171: return 130;
_LL173: return 130;_LL175: return 130;_LL177: return 130;_LL179: return 130;_LL17B:
return 140;_LL17D: return 140;_LL17F: return 140;_LL181: return 150;_LL183: goto _LL185;
_LL185: goto _LL187;_LL187: goto _LL189;_LL189: goto _LL18B;_LL18B: goto _LL18D;_LL18D:
goto _LL18F;_LL18F: goto _LL191;_LL191: goto _LL193;_LL193: return 140;_LL195: return 140;
_LL197: return 10000;_LL199: return 140;_LL19B: return 140;_LL14F:;}struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){int
myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*s;{void*_tmpBA=(void*)e->r;void*
_tmpBB;struct _tuple0*_tmpBC;struct _tuple0*_tmpBD;struct Cyc_List_List*_tmpBE;void*
_tmpBF;struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_Core_Opt*_tmpC1;struct Cyc_Absyn_Exp*
_tmpC2;void*_tmpC3;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Exp*_tmpC5;struct
Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;
struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_List_List*_tmpCA;struct Cyc_Absyn_Exp*_tmpCB;
struct Cyc_List_List*_tmpCC;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*_tmpCE;
struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpD1;
void*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*
_tmpD5;void*_tmpD6;struct Cyc_Absyn_Exp*_tmpD7;void*_tmpD8;struct _tagged_arr*
_tmpD9;void*_tmpDA;void*_tmpDB;unsigned int _tmpDC;void*_tmpDD;void*_tmpDE;struct
Cyc_List_List*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct _tagged_arr*_tmpE1;struct
Cyc_Absyn_Exp*_tmpE2;struct _tagged_arr*_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;struct
Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_List_List*_tmpE7;
struct Cyc_List_List*_tmpE8;struct _tuple1*_tmpE9;struct Cyc_List_List*_tmpEA;
struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Vardecl*
_tmpED;struct Cyc_List_List*_tmpEE;struct _tuple0*_tmpEF;struct Cyc_List_List*
_tmpF0;struct Cyc_Absyn_Tunionfield*_tmpF1;struct Cyc_List_List*_tmpF2;struct
_tuple0*_tmpF3;struct _tuple0*_tmpF4;struct Cyc_Absyn_MallocInfo _tmpF5;struct Cyc_Absyn_Exp*
_tmpF6;void**_tmpF7;struct Cyc_Absyn_Exp*_tmpF8;int _tmpF9;struct Cyc_List_List*
_tmpFA;struct Cyc_Core_Opt*_tmpFB;struct Cyc_Absyn_Stmt*_tmpFC;struct Cyc_Absyn_Fndecl*
_tmpFD;struct Cyc_Absyn_Exp*_tmpFE;_LL1C9: if(*((int*)_tmpBA)== 0){_LL217: _tmpBB=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmpBA)->f1;goto _LL1CA;}else{goto _LL1CB;}
_LL1CB: if(*((int*)_tmpBA)== 1){_LL218: _tmpBC=((struct Cyc_Absyn_Var_e_struct*)
_tmpBA)->f1;goto _LL1CC;}else{goto _LL1CD;}_LL1CD: if(*((int*)_tmpBA)== 2){_LL219:
_tmpBD=((struct Cyc_Absyn_UnknownId_e_struct*)_tmpBA)->f1;goto _LL1CE;}else{goto
_LL1CF;}_LL1CF: if(*((int*)_tmpBA)== 3){_LL21B: _tmpBF=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmpBA)->f1;goto _LL21A;_LL21A: _tmpBE=((struct Cyc_Absyn_Primop_e_struct*)_tmpBA)->f2;
goto _LL1D0;}else{goto _LL1D1;}_LL1D1: if(*((int*)_tmpBA)== 4){_LL21E: _tmpC2=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpBA)->f1;goto _LL21D;_LL21D: _tmpC1=((struct
Cyc_Absyn_AssignOp_e_struct*)_tmpBA)->f2;goto _LL21C;_LL21C: _tmpC0=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpBA)->f3;goto _LL1D2;}else{goto _LL1D3;}_LL1D3: if(*((int*)_tmpBA)== 5){_LL220:
_tmpC4=((struct Cyc_Absyn_Increment_e_struct*)_tmpBA)->f1;goto _LL21F;_LL21F:
_tmpC3=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpBA)->f2;goto _LL1D4;}
else{goto _LL1D5;}_LL1D5: if(*((int*)_tmpBA)== 6){_LL223: _tmpC7=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpBA)->f1;goto _LL222;_LL222: _tmpC6=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpBA)->f2;goto _LL221;_LL221: _tmpC5=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpBA)->f3;goto _LL1D6;}else{goto _LL1D7;}_LL1D7: if(*((int*)_tmpBA)== 7){_LL225:
_tmpC9=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpBA)->f1;goto _LL224;_LL224: _tmpC8=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmpBA)->f2;goto _LL1D8;}else{goto _LL1D9;}_LL1D9:
if(*((int*)_tmpBA)== 8){_LL227: _tmpCB=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmpBA)->f1;goto _LL226;_LL226: _tmpCA=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmpBA)->f2;goto _LL1DA;}else{goto _LL1DB;}_LL1DB: if(*((int*)_tmpBA)== 9){_LL229:
_tmpCD=((struct Cyc_Absyn_FnCall_e_struct*)_tmpBA)->f1;goto _LL228;_LL228: _tmpCC=((
struct Cyc_Absyn_FnCall_e_struct*)_tmpBA)->f2;goto _LL1DC;}else{goto _LL1DD;}_LL1DD:
if(*((int*)_tmpBA)== 10){_LL22A: _tmpCE=((struct Cyc_Absyn_Throw_e_struct*)_tmpBA)->f1;
goto _LL1DE;}else{goto _LL1DF;}_LL1DF: if(*((int*)_tmpBA)== 11){_LL22B: _tmpCF=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpBA)->f1;goto _LL1E0;}else{goto _LL1E1;}
_LL1E1: if(*((int*)_tmpBA)== 12){_LL22C: _tmpD0=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpBA)->f1;goto _LL1E2;}else{goto _LL1E3;}_LL1E3: if(*((int*)_tmpBA)== 13){_LL22E:
_tmpD2=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpBA)->f1;goto _LL22D;_LL22D:
_tmpD1=((struct Cyc_Absyn_Cast_e_struct*)_tmpBA)->f2;goto _LL1E4;}else{goto _LL1E5;}
_LL1E5: if(*((int*)_tmpBA)== 14){_LL22F: _tmpD3=((struct Cyc_Absyn_Address_e_struct*)
_tmpBA)->f1;goto _LL1E6;}else{goto _LL1E7;}_LL1E7: if(*((int*)_tmpBA)== 15){_LL231:
_tmpD5=((struct Cyc_Absyn_New_e_struct*)_tmpBA)->f1;goto _LL230;_LL230: _tmpD4=((
struct Cyc_Absyn_New_e_struct*)_tmpBA)->f2;goto _LL1E8;}else{goto _LL1E9;}_LL1E9:
if(*((int*)_tmpBA)== 16){_LL232: _tmpD6=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpBA)->f1;goto _LL1EA;}else{goto _LL1EB;}_LL1EB: if(*((int*)_tmpBA)== 17){_LL233:
_tmpD7=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpBA)->f1;goto _LL1EC;}else{goto
_LL1ED;}_LL1ED: if(*((int*)_tmpBA)== 18){_LL236: _tmpDA=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpBA)->f1;goto _LL234;_LL234: _tmpD8=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpBA)->f2;if(*((int*)_tmpD8)== 0){_LL235: _tmpD9=((struct Cyc_Absyn_StructField_struct*)
_tmpD8)->f1;goto _LL1EE;}else{goto _LL1EF;}}else{goto _LL1EF;}_LL1EF: if(*((int*)
_tmpBA)== 18){_LL239: _tmpDD=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpBA)->f1;
goto _LL237;_LL237: _tmpDB=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpBA)->f2;
if(*((int*)_tmpDB)== 1){_LL238: _tmpDC=((struct Cyc_Absyn_TupleIndex_struct*)
_tmpDB)->f1;goto _LL1F0;}else{goto _LL1F1;}}else{goto _LL1F1;}_LL1F1: if(*((int*)
_tmpBA)== 19){_LL23B: _tmpDF=((struct Cyc_Absyn_Gentyp_e_struct*)_tmpBA)->f1;goto
_LL23A;_LL23A: _tmpDE=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmpBA)->f2;goto
_LL1F2;}else{goto _LL1F3;}_LL1F3: if(*((int*)_tmpBA)== 20){_LL23C: _tmpE0=((struct
Cyc_Absyn_Deref_e_struct*)_tmpBA)->f1;goto _LL1F4;}else{goto _LL1F5;}_LL1F5: if(*((
int*)_tmpBA)== 21){_LL23E: _tmpE2=((struct Cyc_Absyn_StructMember_e_struct*)_tmpBA)->f1;
goto _LL23D;_LL23D: _tmpE1=((struct Cyc_Absyn_StructMember_e_struct*)_tmpBA)->f2;
goto _LL1F6;}else{goto _LL1F7;}_LL1F7: if(*((int*)_tmpBA)== 22){_LL240: _tmpE4=((
struct Cyc_Absyn_StructArrow_e_struct*)_tmpBA)->f1;goto _LL23F;_LL23F: _tmpE3=((
struct Cyc_Absyn_StructArrow_e_struct*)_tmpBA)->f2;goto _LL1F8;}else{goto _LL1F9;}
_LL1F9: if(*((int*)_tmpBA)== 23){_LL242: _tmpE6=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpBA)->f1;goto _LL241;_LL241: _tmpE5=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpBA)->f2;goto _LL1FA;}else{goto _LL1FB;}_LL1FB: if(*((int*)_tmpBA)== 24){_LL243:
_tmpE7=((struct Cyc_Absyn_Tuple_e_struct*)_tmpBA)->f1;goto _LL1FC;}else{goto _LL1FD;}
_LL1FD: if(*((int*)_tmpBA)== 25){_LL245: _tmpE9=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpBA)->f1;goto _LL244;_LL244: _tmpE8=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmpBA)->f2;goto _LL1FE;}else{goto _LL1FF;}_LL1FF: if(*((int*)_tmpBA)== 26){_LL246:
_tmpEA=((struct Cyc_Absyn_Array_e_struct*)_tmpBA)->f1;goto _LL200;}else{goto _LL201;}
_LL201: if(*((int*)_tmpBA)== 27){_LL249: _tmpED=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpBA)->f1;goto _LL248;_LL248: _tmpEC=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpBA)->f2;goto _LL247;_LL247: _tmpEB=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpBA)->f3;goto _LL202;}else{goto _LL203;}_LL203: if(*((int*)_tmpBA)== 28){_LL24B:
_tmpEF=((struct Cyc_Absyn_Struct_e_struct*)_tmpBA)->f1;goto _LL24A;_LL24A: _tmpEE=((
struct Cyc_Absyn_Struct_e_struct*)_tmpBA)->f3;goto _LL204;}else{goto _LL205;}_LL205:
if(*((int*)_tmpBA)== 29){_LL24C: _tmpF0=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpBA)->f2;goto _LL206;}else{goto _LL207;}_LL207: if(*((int*)_tmpBA)== 30){_LL24E:
_tmpF2=((struct Cyc_Absyn_Tunion_e_struct*)_tmpBA)->f3;goto _LL24D;_LL24D: _tmpF1=((
struct Cyc_Absyn_Tunion_e_struct*)_tmpBA)->f5;goto _LL208;}else{goto _LL209;}_LL209:
if(*((int*)_tmpBA)== 31){_LL24F: _tmpF3=((struct Cyc_Absyn_Enum_e_struct*)_tmpBA)->f1;
goto _LL20A;}else{goto _LL20B;}_LL20B: if(*((int*)_tmpBA)== 32){_LL250: _tmpF4=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmpBA)->f1;goto _LL20C;}else{goto _LL20D;}
_LL20D: if(*((int*)_tmpBA)== 33){_LL251: _tmpF5=((struct Cyc_Absyn_Malloc_e_struct*)
_tmpBA)->f1;_LL255: _tmpF9=_tmpF5.is_calloc;goto _LL254;_LL254: _tmpF8=_tmpF5.rgn;
goto _LL253;_LL253: _tmpF7=_tmpF5.elt_type;goto _LL252;_LL252: _tmpF6=_tmpF5.num_elts;
goto _LL20E;}else{goto _LL20F;}_LL20F: if(*((int*)_tmpBA)== 34){_LL257: _tmpFB=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpBA)->f1;goto _LL256;_LL256: _tmpFA=((
struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpBA)->f2;goto _LL210;}else{goto _LL211;}
_LL211: if(*((int*)_tmpBA)== 35){_LL258: _tmpFC=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpBA)->f1;goto _LL212;}else{goto _LL213;}_LL213: if(*((int*)_tmpBA)== 36){_LL259:
_tmpFD=((struct Cyc_Absyn_Codegen_e_struct*)_tmpBA)->f1;goto _LL214;}else{goto
_LL215;}_LL215: if(*((int*)_tmpBA)== 37){_LL25A: _tmpFE=((struct Cyc_Absyn_Fill_e_struct*)
_tmpBA)->f1;goto _LL216;}else{goto _LL1C8;}_LL1CA: s=Cyc_Absynpp_cnst2doc(_tmpBB);
goto _LL1C8;_LL1CC: _tmpBD=_tmpBC;goto _LL1CE;_LL1CE: s=Cyc_Absynpp_qvar2doc(_tmpBD);
goto _LL1C8;_LL1D0: s=Cyc_Absynpp_primapp2doc(myprec,_tmpBF,_tmpBE);goto _LL1C8;
_LL1D2: s=({struct Cyc_PP_Doc*_tmpFF[5];_tmpFF[4]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmpC0);_tmpFF[3]=Cyc_PP_text(_tag_arr("= ",sizeof(unsigned char),3));_tmpFF[2]=
_tmpC1 == 0? Cyc_PP_text(_tag_arr("",sizeof(unsigned char),1)): Cyc_Absynpp_prim2doc((
void*)_tmpC1->v);_tmpFF[1]=Cyc_PP_text(_tag_arr(" ",sizeof(unsigned char),2));
_tmpFF[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC2);Cyc_PP_cat(_tag_arr(_tmpFF,
sizeof(struct Cyc_PP_Doc*),5));});goto _LL1C8;_LL1D4: {struct Cyc_PP_Doc*_tmp100=
Cyc_Absynpp_exp2doc_prec(myprec,_tmpC4);{void*_tmp101=_tmpC3;_LL25C: if((int)
_tmp101 == 0){goto _LL25D;}else{goto _LL25E;}_LL25E: if((int)_tmp101 == 2){goto _LL25F;}
else{goto _LL260;}_LL260: if((int)_tmp101 == 1){goto _LL261;}else{goto _LL262;}_LL262:
if((int)_tmp101 == 3){goto _LL263;}else{goto _LL25B;}_LL25D: s=({struct Cyc_PP_Doc*
_tmp102[2];_tmp102[1]=_tmp100;_tmp102[0]=Cyc_PP_text(_tag_arr("++",sizeof(
unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp102,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL25B;_LL25F: s=({struct Cyc_PP_Doc*_tmp103[2];_tmp103[1]=_tmp100;_tmp103[0]=
Cyc_PP_text(_tag_arr("--",sizeof(unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp103,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL25B;_LL261: s=({struct Cyc_PP_Doc*_tmp104[
2];_tmp104[1]=Cyc_PP_text(_tag_arr("++",sizeof(unsigned char),3));_tmp104[0]=
_tmp100;Cyc_PP_cat(_tag_arr(_tmp104,sizeof(struct Cyc_PP_Doc*),2));});goto _LL25B;
_LL263: s=({struct Cyc_PP_Doc*_tmp105[2];_tmp105[1]=Cyc_PP_text(_tag_arr("--",
sizeof(unsigned char),3));_tmp105[0]=_tmp100;Cyc_PP_cat(_tag_arr(_tmp105,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL25B;_LL25B:;}goto _LL1C8;}_LL1D6:{struct _tuple8
_tmp107=({struct _tuple8 _tmp106;_tmp106.f1=(void*)_tmpC6->r;_tmp106.f2=(void*)
_tmpC5->r;_tmp106;});_LL265: goto _LL266;_LL266: s=({struct Cyc_PP_Doc*_tmp108[5];
_tmp108[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC5);_tmp108[3]=Cyc_PP_text(
_tag_arr(" : ",sizeof(unsigned char),4));_tmp108[2]=Cyc_Absynpp_exp2doc_prec(0,
_tmpC6);_tmp108[1]=Cyc_PP_text(_tag_arr(" ? ",sizeof(unsigned char),4));_tmp108[
0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC7);Cyc_PP_cat(_tag_arr(_tmp108,sizeof(
struct Cyc_PP_Doc*),5));});goto _LL264;_LL264:;}goto _LL1C8;_LL1D8: s=({struct Cyc_PP_Doc*
_tmp109[5];_tmp109[4]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp109[
3]=Cyc_Absynpp_exp2doc(_tmpC8);_tmp109[2]=Cyc_PP_text(_tag_arr(", ",sizeof(
unsigned char),3));_tmp109[1]=Cyc_Absynpp_exp2doc(_tmpC9);_tmp109[0]=Cyc_PP_text(
_tag_arr("(",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp109,sizeof(struct
Cyc_PP_Doc*),5));});goto _LL1C8;_LL1DA: s=({struct Cyc_PP_Doc*_tmp10A[4];_tmp10A[3]=
Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp10A[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmpCA);_tmp10A[1]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));_tmp10A[
0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpCB);Cyc_PP_cat(_tag_arr(_tmp10A,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL1C8;_LL1DC: s=({struct Cyc_PP_Doc*_tmp10B[4];
_tmp10B[3]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp10B[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmpCC);_tmp10B[1]=Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));_tmp10B[
0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpCD);Cyc_PP_cat(_tag_arr(_tmp10B,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL1C8;_LL1DE: s=({struct Cyc_PP_Doc*_tmp10C[2];
_tmp10C[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpCE);_tmp10C[0]=Cyc_PP_text(
_tag_arr("throw ",sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(_tmp10C,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL1C8;_LL1E0: s=Cyc_Absynpp_exp2doc_prec(inprec,
_tmpCF);goto _LL1C8;_LL1E2: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmpD0);goto _LL1C8;
_LL1E4: s=({struct Cyc_PP_Doc*_tmp10D[4];_tmp10D[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpD1);_tmp10D[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));
_tmp10D[1]=Cyc_Absynpp_typ2doc(_tmpD2);_tmp10D[0]=Cyc_PP_text(_tag_arr("(",
sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp10D,sizeof(struct Cyc_PP_Doc*),
4));});goto _LL1C8;_LL1E6: s=({struct Cyc_PP_Doc*_tmp10E[2];_tmp10E[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpD3);_tmp10E[0]=Cyc_PP_text(_tag_arr("&",sizeof(unsigned char),2));Cyc_PP_cat(
_tag_arr(_tmp10E,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1C8;_LL1E8: if(_tmpD5 == 
0){s=({struct Cyc_PP_Doc*_tmp10F[2];_tmp10F[1]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmpD4);_tmp10F[0]=Cyc_PP_text(_tag_arr("new ",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp10F,sizeof(struct Cyc_PP_Doc*),2));});}else{s=({struct Cyc_PP_Doc*
_tmp110[4];_tmp110[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpD4);_tmp110[2]=Cyc_PP_text(
_tag_arr(") ",sizeof(unsigned char),3));_tmp110[1]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(_tmpD5));_tmp110[0]=Cyc_PP_text(_tag_arr("rnew(",
sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp110,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL1C8;_LL1EA: s=({struct Cyc_PP_Doc*_tmp111[3];_tmp111[2]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp111[1]=Cyc_Absynpp_typ2doc(_tmpD6);
_tmp111[0]=Cyc_PP_text(_tag_arr("sizeof(",sizeof(unsigned char),8));Cyc_PP_cat(
_tag_arr(_tmp111,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1C8;_LL1EC: s=({struct
Cyc_PP_Doc*_tmp112[3];_tmp112[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),
2));_tmp112[1]=Cyc_Absynpp_exp2doc(_tmpD7);_tmp112[0]=Cyc_PP_text(_tag_arr("sizeof(",
sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp112,sizeof(struct Cyc_PP_Doc*),
3));});goto _LL1C8;_LL1EE: s=({struct Cyc_PP_Doc*_tmp113[5];_tmp113[4]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp113[3]=Cyc_PP_textptr(_tmpD9);_tmp113[
2]=Cyc_PP_text(_tag_arr(",",sizeof(unsigned char),2));_tmp113[1]=Cyc_Absynpp_typ2doc(
_tmpDA);_tmp113[0]=Cyc_PP_text(_tag_arr("offsetof(",sizeof(unsigned char),10));
Cyc_PP_cat(_tag_arr(_tmp113,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1C8;_LL1F0: s=({
struct Cyc_PP_Doc*_tmp114[5];_tmp114[4]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp114[3]=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp116;_tmp116.tag=1;_tmp116.f1=(int)_tmpDC;{void*_tmp115[1]={& _tmp116};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp115,sizeof(void*),1));}}));
_tmp114[2]=Cyc_PP_text(_tag_arr(",",sizeof(unsigned char),2));_tmp114[1]=Cyc_Absynpp_typ2doc(
_tmpDD);_tmp114[0]=Cyc_PP_text(_tag_arr("offsetof(",sizeof(unsigned char),10));
Cyc_PP_cat(_tag_arr(_tmp114,sizeof(struct Cyc_PP_Doc*),5));});goto _LL1C8;_LL1F2: s=({
struct Cyc_PP_Doc*_tmp117[4];_tmp117[3]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp117[2]=Cyc_Absynpp_typ2doc(_tmpDE);_tmp117[1]=Cyc_Absynpp_tvars2doc(
_tmpDF);_tmp117[0]=Cyc_PP_text(_tag_arr("__gen(",sizeof(unsigned char),7));Cyc_PP_cat(
_tag_arr(_tmp117,sizeof(struct Cyc_PP_Doc*),4));});goto _LL1C8;_LL1F4: s=({struct
Cyc_PP_Doc*_tmp118[2];_tmp118[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpE0);_tmp118[
0]=Cyc_PP_text(_tag_arr("*",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(
_tmp118,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1C8;_LL1F6: s=({struct Cyc_PP_Doc*
_tmp119[3];_tmp119[2]=Cyc_PP_textptr(_tmpE1);_tmp119[1]=Cyc_PP_text(_tag_arr(".",
sizeof(unsigned char),2));_tmp119[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpE2);Cyc_PP_cat(
_tag_arr(_tmp119,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1C8;_LL1F8: s=({struct
Cyc_PP_Doc*_tmp11A[3];_tmp11A[2]=Cyc_PP_textptr(_tmpE3);_tmp11A[1]=Cyc_PP_text(
_tag_arr("->",sizeof(unsigned char),3));_tmp11A[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpE4);Cyc_PP_cat(_tag_arr(_tmp11A,sizeof(struct Cyc_PP_Doc*),3));});goto
_LL1C8;_LL1FA: s=({struct Cyc_PP_Doc*_tmp11B[4];_tmp11B[3]=Cyc_PP_text(_tag_arr("]",
sizeof(unsigned char),2));_tmp11B[2]=Cyc_Absynpp_exp2doc(_tmpE5);_tmp11B[1]=Cyc_PP_text(
_tag_arr("[",sizeof(unsigned char),2));_tmp11B[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpE6);Cyc_PP_cat(_tag_arr(_tmp11B,sizeof(struct Cyc_PP_Doc*),4));});goto
_LL1C8;_LL1FC: s=({struct Cyc_PP_Doc*_tmp11C[3];_tmp11C[2]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp11C[1]=Cyc_Absynpp_exps2doc_prec(20,_tmpE7);
_tmp11C[0]=Cyc_PP_text(_tag_arr("$(",sizeof(unsigned char),3));Cyc_PP_cat(
_tag_arr(_tmp11C,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1C8;_LL1FE: s=({struct
Cyc_PP_Doc*_tmp11D[4];_tmp11D[3]=Cyc_PP_group(_tag_arr("{",sizeof(unsigned char),
2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple6*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmpE8));_tmp11D[2]=Cyc_PP_text(_tag_arr(")",
sizeof(unsigned char),2));_tmp11D[1]=Cyc_Absynpp_typ2doc((*_tmpE9).f3);_tmp11D[0]=
Cyc_PP_text(_tag_arr("(",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp11D,
sizeof(struct Cyc_PP_Doc*),4));});goto _LL1C8;_LL200: s=Cyc_PP_group(_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_tuple6*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmpEA));goto
_LL1C8;_LL202: s=({struct Cyc_PP_Doc*_tmp11E[7];_tmp11E[6]=Cyc_PP_text(_tag_arr("}",
sizeof(unsigned char),2));_tmp11E[5]=Cyc_Absynpp_exp2doc(_tmpEB);_tmp11E[4]=Cyc_PP_text(
_tag_arr(" : ",sizeof(unsigned char),4));_tmp11E[3]=Cyc_Absynpp_exp2doc(_tmpEC);
_tmp11E[2]=Cyc_PP_text(_tag_arr(" < ",sizeof(unsigned char),4));_tmp11E[1]=Cyc_PP_text(*(*
_tmpED->name).f2);_tmp11E[0]=Cyc_PP_text(_tag_arr("{for ",sizeof(unsigned char),
6));Cyc_PP_cat(_tag_arr(_tmp11E,sizeof(struct Cyc_PP_Doc*),7));});goto _LL1C8;
_LL204: s=({struct Cyc_PP_Doc*_tmp11F[2];_tmp11F[1]=Cyc_PP_group(_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_tuple6*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmpEE));
_tmp11F[0]=Cyc_Absynpp_qvar2doc(_tmpEF);Cyc_PP_cat(_tag_arr(_tmp11F,sizeof(
struct Cyc_PP_Doc*),2));});goto _LL1C8;_LL206: s=Cyc_PP_group(_tag_arr("{",sizeof(
unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple6*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmpF0));goto _LL1C8;
_LL208: if(_tmpF2 == 0){s=Cyc_Absynpp_qvar2doc(_tmpF1->name);}else{s=({struct Cyc_PP_Doc*
_tmp120[2];_tmp120[1]=Cyc_PP_egroup(_tag_arr("(",sizeof(unsigned char),2),
_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmpF2));_tmp120[0]=Cyc_Absynpp_qvar2doc(
_tmpF1->name);Cyc_PP_cat(_tag_arr(_tmp120,sizeof(struct Cyc_PP_Doc*),2));});}goto
_LL1C8;_LL20A: s=Cyc_Absynpp_qvar2doc(_tmpF3);goto _LL1C8;_LL20C: s=Cyc_Absynpp_qvar2doc(
_tmpF4);goto _LL1C8;_LL20E: if(_tmpF9){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmpF7)),0);if(_tmpF8 == 0){s=({struct Cyc_PP_Doc*_tmp121[5];
_tmp121[4]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp121[3]=Cyc_Absynpp_exp2doc(
st);_tmp121[2]=Cyc_PP_text(_tag_arr(",",sizeof(unsigned char),2));_tmp121[1]=Cyc_Absynpp_exp2doc(
_tmpF6);_tmp121[0]=Cyc_PP_text(_tag_arr("calloc(",sizeof(unsigned char),8));Cyc_PP_cat(
_tag_arr(_tmp121,sizeof(struct Cyc_PP_Doc*),5));});}else{s=({struct Cyc_PP_Doc*
_tmp122[7];_tmp122[6]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp122[
5]=Cyc_Absynpp_exp2doc(st);_tmp122[4]=Cyc_PP_text(_tag_arr(",",sizeof(
unsigned char),2));_tmp122[3]=Cyc_Absynpp_exp2doc(_tmpF6);_tmp122[2]=Cyc_PP_text(
_tag_arr(",",sizeof(unsigned char),2));_tmp122[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmpF8));_tmp122[0]=Cyc_PP_text(_tag_arr("rcalloc(",sizeof(
unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp122,sizeof(struct Cyc_PP_Doc*),7));});}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmpF7 == 0){new_e=_tmpF6;}else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmpF7,0),_tmpF6,0);}if(_tmpF8 == 0){s=({struct Cyc_PP_Doc*
_tmp123[3];_tmp123[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),2));_tmp123[
1]=Cyc_Absynpp_exp2doc(new_e);_tmp123[0]=Cyc_PP_text(_tag_arr("malloc(",sizeof(
unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp123,sizeof(struct Cyc_PP_Doc*),3));});}
else{s=({struct Cyc_PP_Doc*_tmp124[5];_tmp124[4]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp124[3]=Cyc_Absynpp_exp2doc(new_e);_tmp124[2]=Cyc_PP_text(
_tag_arr(",",sizeof(unsigned char),2));_tmp124[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmpF8));_tmp124[0]=Cyc_PP_text(_tag_arr("rmalloc(",sizeof(
unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp124,sizeof(struct Cyc_PP_Doc*),5));});}}
goto _LL1C8;_LL210: s=Cyc_PP_group(_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple6*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpFA));goto _LL1C8;_LL212: s=({struct Cyc_PP_Doc*_tmp125[3];_tmp125[2]=Cyc_PP_text(
_tag_arr(" })",sizeof(unsigned char),4));_tmp125[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmpFC));_tmp125[0]=Cyc_PP_text(_tag_arr("({ ",sizeof(unsigned char),4));Cyc_PP_cat(
_tag_arr(_tmp125,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1C8;_LL214: s=({struct
Cyc_PP_Doc*_tmp126[3];_tmp126[2]=Cyc_PP_text(_tag_arr(")",sizeof(unsigned char),
2));_tmp126[1]=Cyc_PP_nest(2,Cyc_Absynpp_decl2doc(({struct Cyc_Absyn_Decl*_tmp127=
_cycalloc(sizeof(*_tmp127));_tmp127->r=(void*)((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp129;_tmp129.tag=1;_tmp129.f1=_tmpFD;_tmp129;});_tmp128;}));_tmp127->loc=e->loc;
_tmp127;})));_tmp126[0]=Cyc_PP_text(_tag_arr("codegen(",sizeof(unsigned char),9));
Cyc_PP_cat(_tag_arr(_tmp126,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1C8;_LL216: s=({
struct Cyc_PP_Doc*_tmp12A[3];_tmp12A[2]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp12A[1]=Cyc_PP_nest(2,Cyc_Absynpp_exp2doc(_tmpFE));_tmp12A[
0]=Cyc_PP_text(_tag_arr("fill(",sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(
_tmp12A,sizeof(struct Cyc_PP_Doc*),3));});goto _LL1C8;_LL1C8:;}if(inprec >= myprec){
s=({struct Cyc_PP_Doc*_tmp12B[3];_tmp12B[2]=Cyc_PP_text(_tag_arr(")",sizeof(
unsigned char),2));_tmp12B[1]=s;_tmp12B[0]=Cyc_PP_text(_tag_arr("(",sizeof(
unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp12B,sizeof(struct Cyc_PP_Doc*),3));});}
return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){void*_tmp12C=d;
struct Cyc_Absyn_Exp*_tmp12D;struct _tagged_arr*_tmp12E;_LL268: if(*((int*)_tmp12C)
== 0){_LL26C: _tmp12D=((struct Cyc_Absyn_ArrayElement_struct*)_tmp12C)->f1;goto
_LL269;}else{goto _LL26A;}_LL26A: if(*((int*)_tmp12C)== 1){_LL26D: _tmp12E=((struct
Cyc_Absyn_FieldName_struct*)_tmp12C)->f1;goto _LL26B;}else{goto _LL267;}_LL269:
return({struct Cyc_PP_Doc*_tmp12F[3];_tmp12F[2]=Cyc_PP_text(_tag_arr("]",sizeof(
unsigned char),2));_tmp12F[1]=Cyc_Absynpp_exp2doc(_tmp12D);_tmp12F[0]=Cyc_PP_text(
_tag_arr(".[",sizeof(unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp12F,sizeof(
struct Cyc_PP_Doc*),3));});_LL26B: return({struct Cyc_PP_Doc*_tmp130[2];_tmp130[1]=
Cyc_PP_textptr(_tmp12E);_tmp130[0]=Cyc_PP_text(_tag_arr(".",sizeof(unsigned char),
2));Cyc_PP_cat(_tag_arr(_tmp130,sizeof(struct Cyc_PP_Doc*),2));});_LL267:;}struct
Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple6*de){if((*de).f1 == 0){return Cyc_Absynpp_exp2doc((*
de).f2);}else{return({struct Cyc_PP_Doc*_tmp131[2];_tmp131[1]=Cyc_Absynpp_exp2doc((*
de).f2);_tmp131[0]=Cyc_PP_egroup(_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",
sizeof(unsigned char),2),_tag_arr("=",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*
de).f1));Cyc_PP_cat(_tag_arr(_tmp131,sizeof(struct Cyc_PP_Doc*),2));});}}struct
Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){return Cyc_PP_group(
_tag_arr("",sizeof(unsigned char),1),_tag_arr("",sizeof(unsigned char),1),
_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*c){void*_tmp132=c;
unsigned char _tmp133;void*_tmp134;short _tmp135;void*_tmp136;int _tmp137;void*
_tmp138;int _tmp139;void*_tmp13A;long long _tmp13B;void*_tmp13C;struct _tagged_arr
_tmp13D;struct _tagged_arr _tmp13E;_LL26F: if((unsigned int)_tmp132 > 1?*((int*)
_tmp132)== 0: 0){_LL280: _tmp134=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp132)->f1;
goto _LL27F;_LL27F: _tmp133=((struct Cyc_Absyn_Char_c_struct*)_tmp132)->f2;goto
_LL270;}else{goto _LL271;}_LL271: if((unsigned int)_tmp132 > 1?*((int*)_tmp132)== 1:
0){_LL282: _tmp136=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp132)->f1;goto
_LL281;_LL281: _tmp135=((struct Cyc_Absyn_Short_c_struct*)_tmp132)->f2;goto _LL272;}
else{goto _LL273;}_LL273: if((unsigned int)_tmp132 > 1?*((int*)_tmp132)== 2: 0){
_LL284: _tmp138=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp132)->f1;if((int)
_tmp138 == 0){goto _LL283;}else{goto _LL275;}_LL283: _tmp137=((struct Cyc_Absyn_Int_c_struct*)
_tmp132)->f2;goto _LL274;}else{goto _LL275;}_LL275: if((unsigned int)_tmp132 > 1?*((
int*)_tmp132)== 2: 0){_LL286: _tmp13A=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp132)->f1;if((int)_tmp13A == 1){goto _LL285;}else{goto _LL277;}_LL285: _tmp139=((
struct Cyc_Absyn_Int_c_struct*)_tmp132)->f2;goto _LL276;}else{goto _LL277;}_LL277:
if((unsigned int)_tmp132 > 1?*((int*)_tmp132)== 3: 0){_LL288: _tmp13C=(void*)((
struct Cyc_Absyn_LongLong_c_struct*)_tmp132)->f1;goto _LL287;_LL287: _tmp13B=((
struct Cyc_Absyn_LongLong_c_struct*)_tmp132)->f2;goto _LL278;}else{goto _LL279;}
_LL279: if((unsigned int)_tmp132 > 1?*((int*)_tmp132)== 4: 0){_LL289: _tmp13D=((
struct Cyc_Absyn_Float_c_struct*)_tmp132)->f1;goto _LL27A;}else{goto _LL27B;}_LL27B:
if((int)_tmp132 == 0){goto _LL27C;}else{goto _LL27D;}_LL27D: if((unsigned int)
_tmp132 > 1?*((int*)_tmp132)== 5: 0){_LL28A: _tmp13E=((struct Cyc_Absyn_String_c_struct*)
_tmp132)->f1;goto _LL27E;}else{goto _LL26E;}_LL270: return Cyc_PP_text((struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp140;_tmp140.tag=0;_tmp140.f1=(
struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp133);{void*_tmp13F[1]={& _tmp140};
Cyc_Std_aprintf(_tag_arr("'%s'",sizeof(unsigned char),5),_tag_arr(_tmp13F,
sizeof(void*),1));}}));_LL272: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp142;_tmp142.tag=1;_tmp142.f1=(int)((unsigned int)((int)_tmp135));{void*
_tmp141[1]={& _tmp142};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),
_tag_arr(_tmp141,sizeof(void*),1));}}));_LL274: return Cyc_PP_text((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp144;_tmp144.tag=1;_tmp144.f1=(int)((
unsigned int)_tmp137);{void*_tmp143[1]={& _tmp144};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(unsigned char),3),_tag_arr(_tmp143,sizeof(void*),1));}}));_LL276: return
Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp146;_tmp146.tag=
1;_tmp146.f1=(unsigned int)_tmp139;{void*_tmp145[1]={& _tmp146};Cyc_Std_aprintf(
_tag_arr("%u",sizeof(unsigned char),3),_tag_arr(_tmp145,sizeof(void*),1));}}));
_LL278: return Cyc_PP_text(_tag_arr("<<FIX LONG LONG CONSTANT>>",sizeof(
unsigned char),27));_LL27A: return Cyc_PP_text(_tmp13D);_LL27C: return Cyc_PP_text(
_tag_arr("NULL",sizeof(unsigned char),5));_LL27E: return({struct Cyc_PP_Doc*
_tmp147[3];_tmp147[2]=Cyc_PP_text(_tag_arr("\"",sizeof(unsigned char),2));
_tmp147[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp13E));_tmp147[0]=Cyc_PP_text(
_tag_arr("\"",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp147,sizeof(
struct Cyc_PP_Doc*),3));});_LL26E:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int
inprec,void*p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == (void*)19){if(es == 0? 1: es->tl != 0){(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148[0]=({struct Cyc_Core_Failure_struct
_tmp149;_tmp149.tag=Cyc_Core_Failure;_tmp149.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp14B;_tmp14B.tag=0;_tmp14B.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp14A[1]={& _tmp14B};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof(unsigned char),44),_tag_arr(_tmp14A,sizeof(void*),1));}});_tmp149;});
_tmp148;}));}{struct Cyc_PP_Doc*_tmp14C=Cyc_Absynpp_exp2doc_prec(inprec,(struct
Cyc_Absyn_Exp*)es->hd);return({struct Cyc_PP_Doc*_tmp14D[2];_tmp14D[1]=Cyc_PP_text(
_tag_arr(".size",sizeof(unsigned char),6));_tmp14D[0]=_tmp14C;Cyc_PP_cat(
_tag_arr(_tmp14D,sizeof(struct Cyc_PP_Doc*),2));});}}else{struct Cyc_List_List*ds=((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);if(ds
== 0){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp14E=_cycalloc(
sizeof(*_tmp14E));_tmp14E[0]=({struct Cyc_Core_Failure_struct _tmp14F;_tmp14F.tag=
Cyc_Core_Failure;_tmp14F.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp151;_tmp151.tag=0;_tmp151.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp150[1]={& _tmp151};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc: %s with no args",
sizeof(unsigned char),38),_tag_arr(_tmp150,sizeof(void*),1));}});_tmp14F;});
_tmp14E;}));}else{if(ds->tl == 0){return({struct Cyc_PP_Doc*_tmp152[3];_tmp152[2]=(
struct Cyc_PP_Doc*)ds->hd;_tmp152[1]=Cyc_PP_text(_tag_arr(" ",sizeof(
unsigned char),2));_tmp152[0]=ps;Cyc_PP_cat(_tag_arr(_tmp152,sizeof(struct Cyc_PP_Doc*),
3));});}else{if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp153=_cycalloc(sizeof(*_tmp153));
_tmp153[0]=({struct Cyc_Core_Failure_struct _tmp154;_tmp154.tag=Cyc_Core_Failure;
_tmp154.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp156;_tmp156.tag=
0;_tmp156.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{void*_tmp155[1]={&
_tmp156};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof(unsigned char),47),_tag_arr(_tmp155,sizeof(void*),1));}});_tmp154;});
_tmp153;}));}else{return({struct Cyc_PP_Doc*_tmp157[5];_tmp157[4]=(struct Cyc_PP_Doc*)((
struct Cyc_List_List*)_check_null(ds->tl))->hd;_tmp157[3]=Cyc_PP_text(_tag_arr(" ",
sizeof(unsigned char),2));_tmp157[2]=ps;_tmp157[1]=Cyc_PP_text(_tag_arr(" ",
sizeof(unsigned char),2));_tmp157[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(
_tag_arr(_tmp157,sizeof(struct Cyc_PP_Doc*),5));});}}}}}struct _tagged_arr Cyc_Absynpp_prim2str(
void*p){void*_tmp158=p;_LL28C: if((int)_tmp158 == 0){goto _LL28D;}else{goto _LL28E;}
_LL28E: if((int)_tmp158 == 1){goto _LL28F;}else{goto _LL290;}_LL290: if((int)_tmp158
== 2){goto _LL291;}else{goto _LL292;}_LL292: if((int)_tmp158 == 3){goto _LL293;}else{
goto _LL294;}_LL294: if((int)_tmp158 == 4){goto _LL295;}else{goto _LL296;}_LL296: if((
int)_tmp158 == 5){goto _LL297;}else{goto _LL298;}_LL298: if((int)_tmp158 == 6){goto
_LL299;}else{goto _LL29A;}_LL29A: if((int)_tmp158 == 7){goto _LL29B;}else{goto _LL29C;}
_LL29C: if((int)_tmp158 == 8){goto _LL29D;}else{goto _LL29E;}_LL29E: if((int)_tmp158
== 9){goto _LL29F;}else{goto _LL2A0;}_LL2A0: if((int)_tmp158 == 10){goto _LL2A1;}
else{goto _LL2A2;}_LL2A2: if((int)_tmp158 == 11){goto _LL2A3;}else{goto _LL2A4;}
_LL2A4: if((int)_tmp158 == 12){goto _LL2A5;}else{goto _LL2A6;}_LL2A6: if((int)_tmp158
== 13){goto _LL2A7;}else{goto _LL2A8;}_LL2A8: if((int)_tmp158 == 14){goto _LL2A9;}
else{goto _LL2AA;}_LL2AA: if((int)_tmp158 == 15){goto _LL2AB;}else{goto _LL2AC;}
_LL2AC: if((int)_tmp158 == 16){goto _LL2AD;}else{goto _LL2AE;}_LL2AE: if((int)_tmp158
== 17){goto _LL2AF;}else{goto _LL2B0;}_LL2B0: if((int)_tmp158 == 18){goto _LL2B1;}
else{goto _LL2B2;}_LL2B2: if((int)_tmp158 == 19){goto _LL2B3;}else{goto _LL28B;}
_LL28D: return _tag_arr("+",sizeof(unsigned char),2);_LL28F: return _tag_arr("*",
sizeof(unsigned char),2);_LL291: return _tag_arr("-",sizeof(unsigned char),2);
_LL293: return _tag_arr("/",sizeof(unsigned char),2);_LL295: return _tag_arr("%",
sizeof(unsigned char),2);_LL297: return _tag_arr("==",sizeof(unsigned char),3);
_LL299: return _tag_arr("!=",sizeof(unsigned char),3);_LL29B: return _tag_arr(">",
sizeof(unsigned char),2);_LL29D: return _tag_arr("<",sizeof(unsigned char),2);
_LL29F: return _tag_arr(">=",sizeof(unsigned char),3);_LL2A1: return _tag_arr("<=",
sizeof(unsigned char),3);_LL2A3: return _tag_arr("!",sizeof(unsigned char),2);
_LL2A5: return _tag_arr("~",sizeof(unsigned char),2);_LL2A7: return _tag_arr("&",
sizeof(unsigned char),2);_LL2A9: return _tag_arr("|",sizeof(unsigned char),2);
_LL2AB: return _tag_arr("^",sizeof(unsigned char),2);_LL2AD: return _tag_arr("<<",
sizeof(unsigned char),3);_LL2AF: return _tag_arr(">>",sizeof(unsigned char),3);
_LL2B1: return _tag_arr(">>>",sizeof(unsigned char),4);_LL2B3: return _tag_arr("size",
sizeof(unsigned char),5);_LL28B:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}struct _tuple9{struct Cyc_Core_Opt*f1;
struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_structarg2doc(struct
_tuple9*t){if((*t).f1 == 0){return Cyc_Absynpp_exp2doc((*t).f2);}else{return({
struct Cyc_PP_Doc*_tmp159[3];_tmp159[2]=Cyc_Absynpp_exp2doc((*t).f2);_tmp159[1]=
Cyc_PP_text(_tag_arr(" = ",sizeof(unsigned char),4));_tmp159[0]=Cyc_PP_textptr((
struct _tagged_arr*)((struct Cyc_Core_Opt*)_check_null((*t).f1))->v);Cyc_PP_cat(
_tag_arr(_tmp159,sizeof(struct Cyc_PP_Doc*),3));});}}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s){void*_tmp15A=(void*)s->r;_LL2B5: if((unsigned int)_tmp15A
> 1?*((int*)_tmp15A)== 12: 0){goto _LL2B6;}else{goto _LL2B7;}_LL2B7: goto _LL2B8;
_LL2B6: return 1;_LL2B8: return 0;_LL2B4:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st){struct Cyc_PP_Doc*s;{void*_tmp15B=(void*)st->r;struct Cyc_Absyn_Exp*
_tmp15C;struct Cyc_Absyn_Stmt*_tmp15D;struct Cyc_Absyn_Stmt*_tmp15E;struct Cyc_Absyn_Exp*
_tmp15F;struct Cyc_Absyn_Stmt*_tmp160;struct Cyc_Absyn_Stmt*_tmp161;struct Cyc_Absyn_Exp*
_tmp162;struct Cyc_Absyn_Stmt*_tmp163;struct _tuple2 _tmp164;struct Cyc_Absyn_Exp*
_tmp165;struct _tagged_arr*_tmp166;struct Cyc_Absyn_Stmt*_tmp167;struct _tuple2
_tmp168;struct Cyc_Absyn_Exp*_tmp169;struct _tuple2 _tmp16A;struct Cyc_Absyn_Exp*
_tmp16B;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_ForArrayInfo _tmp16D;struct
Cyc_Absyn_Stmt*_tmp16E;struct _tuple2 _tmp16F;struct Cyc_Absyn_Exp*_tmp170;struct
_tuple2 _tmp171;struct Cyc_Absyn_Exp*_tmp172;struct Cyc_List_List*_tmp173;struct Cyc_List_List*
_tmp174;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_List_List*_tmp176;struct Cyc_Absyn_Exp*
_tmp177;struct Cyc_List_List*_tmp178;struct Cyc_List_List*_tmp179;struct Cyc_Absyn_Stmt*
_tmp17A;struct Cyc_Absyn_Decl*_tmp17B;struct Cyc_Absyn_Stmt*_tmp17C;struct Cyc_Absyn_Stmt*
_tmp17D;struct Cyc_Absyn_Stmt*_tmp17E;struct _tagged_arr*_tmp17F;struct _tuple2
_tmp180;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Absyn_Stmt*_tmp182;struct Cyc_List_List*
_tmp183;struct Cyc_Absyn_Stmt*_tmp184;struct Cyc_Absyn_Stmt*_tmp185;struct Cyc_Absyn_Vardecl*
_tmp186;struct Cyc_Absyn_Tvar*_tmp187;_LL2BA: if((int)_tmp15B == 0){goto _LL2BB;}
else{goto _LL2BC;}_LL2BC: if((unsigned int)_tmp15B > 1?*((int*)_tmp15B)== 0: 0){
_LL2E6: _tmp15C=((struct Cyc_Absyn_Exp_s_struct*)_tmp15B)->f1;goto _LL2BD;}else{
goto _LL2BE;}_LL2BE: if((unsigned int)_tmp15B > 1?*((int*)_tmp15B)== 1: 0){_LL2E8:
_tmp15E=((struct Cyc_Absyn_Seq_s_struct*)_tmp15B)->f1;goto _LL2E7;_LL2E7: _tmp15D=((
struct Cyc_Absyn_Seq_s_struct*)_tmp15B)->f2;goto _LL2BF;}else{goto _LL2C0;}_LL2C0:
if((unsigned int)_tmp15B > 1?*((int*)_tmp15B)== 2: 0){_LL2E9: _tmp15F=((struct Cyc_Absyn_Return_s_struct*)
_tmp15B)->f1;goto _LL2C1;}else{goto _LL2C2;}_LL2C2: if((unsigned int)_tmp15B > 1?*((
int*)_tmp15B)== 3: 0){_LL2EC: _tmp162=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp15B)->f1;goto _LL2EB;_LL2EB: _tmp161=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp15B)->f2;goto _LL2EA;_LL2EA: _tmp160=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp15B)->f3;goto _LL2C3;}else{goto _LL2C4;}_LL2C4: if((unsigned int)_tmp15B > 1?*((
int*)_tmp15B)== 4: 0){_LL2EE: _tmp164=((struct Cyc_Absyn_While_s_struct*)_tmp15B)->f1;
_LL2EF: _tmp165=_tmp164.f1;goto _LL2ED;_LL2ED: _tmp163=((struct Cyc_Absyn_While_s_struct*)
_tmp15B)->f2;goto _LL2C5;}else{goto _LL2C6;}_LL2C6: if((unsigned int)_tmp15B > 1?*((
int*)_tmp15B)== 5: 0){goto _LL2C7;}else{goto _LL2C8;}_LL2C8: if((unsigned int)
_tmp15B > 1?*((int*)_tmp15B)== 6: 0){goto _LL2C9;}else{goto _LL2CA;}_LL2CA: if((
unsigned int)_tmp15B > 1?*((int*)_tmp15B)== 7: 0){_LL2F0: _tmp166=((struct Cyc_Absyn_Goto_s_struct*)
_tmp15B)->f1;goto _LL2CB;}else{goto _LL2CC;}_LL2CC: if((unsigned int)_tmp15B > 1?*((
int*)_tmp15B)== 8: 0){_LL2F6: _tmp16C=((struct Cyc_Absyn_For_s_struct*)_tmp15B)->f1;
goto _LL2F4;_LL2F4: _tmp16A=((struct Cyc_Absyn_For_s_struct*)_tmp15B)->f2;_LL2F5:
_tmp16B=_tmp16A.f1;goto _LL2F2;_LL2F2: _tmp168=((struct Cyc_Absyn_For_s_struct*)
_tmp15B)->f3;_LL2F3: _tmp169=_tmp168.f1;goto _LL2F1;_LL2F1: _tmp167=((struct Cyc_Absyn_For_s_struct*)
_tmp15B)->f4;goto _LL2CD;}else{goto _LL2CE;}_LL2CE: if((unsigned int)_tmp15B > 1?*((
int*)_tmp15B)== 19: 0){_LL2F7: _tmp16D=((struct Cyc_Absyn_ForArray_s_struct*)
_tmp15B)->f1;_LL2FD: _tmp173=_tmp16D.defns;goto _LL2FB;_LL2FB: _tmp171=_tmp16D.condition;
_LL2FC: _tmp172=_tmp171.f1;goto _LL2F9;_LL2F9: _tmp16F=_tmp16D.delta;_LL2FA: _tmp170=
_tmp16F.f1;goto _LL2F8;_LL2F8: _tmp16E=_tmp16D.body;goto _LL2CF;}else{goto _LL2D0;}
_LL2D0: if((unsigned int)_tmp15B > 1?*((int*)_tmp15B)== 9: 0){_LL2FF: _tmp175=((
struct Cyc_Absyn_Switch_s_struct*)_tmp15B)->f1;goto _LL2FE;_LL2FE: _tmp174=((struct
Cyc_Absyn_Switch_s_struct*)_tmp15B)->f2;goto _LL2D1;}else{goto _LL2D2;}_LL2D2: if((
unsigned int)_tmp15B > 1?*((int*)_tmp15B)== 10: 0){_LL301: _tmp177=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmp15B)->f1;goto _LL300;_LL300: _tmp176=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmp15B)->f2;goto _LL2D3;}else{goto _LL2D4;}_LL2D4: if((unsigned int)_tmp15B > 1?*((
int*)_tmp15B)== 11: 0){_LL302: _tmp178=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp15B)->f1;if(_tmp178 == 0){goto _LL2D5;}else{goto _LL2D6;}}else{goto _LL2D6;}
_LL2D6: if((unsigned int)_tmp15B > 1?*((int*)_tmp15B)== 11: 0){_LL303: _tmp179=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp15B)->f1;goto _LL2D7;}else{goto _LL2D8;}
_LL2D8: if((unsigned int)_tmp15B > 1?*((int*)_tmp15B)== 12: 0){_LL305: _tmp17B=((
struct Cyc_Absyn_Decl_s_struct*)_tmp15B)->f1;goto _LL304;_LL304: _tmp17A=((struct
Cyc_Absyn_Decl_s_struct*)_tmp15B)->f2;goto _LL2D9;}else{goto _LL2DA;}_LL2DA: if((
unsigned int)_tmp15B > 1?*((int*)_tmp15B)== 13: 0){_LL306: _tmp17C=((struct Cyc_Absyn_Cut_s_struct*)
_tmp15B)->f1;goto _LL2DB;}else{goto _LL2DC;}_LL2DC: if((unsigned int)_tmp15B > 1?*((
int*)_tmp15B)== 14: 0){_LL307: _tmp17D=((struct Cyc_Absyn_Splice_s_struct*)_tmp15B)->f1;
goto _LL2DD;}else{goto _LL2DE;}_LL2DE: if((unsigned int)_tmp15B > 1?*((int*)_tmp15B)
== 15: 0){_LL309: _tmp17F=((struct Cyc_Absyn_Label_s_struct*)_tmp15B)->f1;goto
_LL308;_LL308: _tmp17E=((struct Cyc_Absyn_Label_s_struct*)_tmp15B)->f2;goto _LL2DF;}
else{goto _LL2E0;}_LL2E0: if((unsigned int)_tmp15B > 1?*((int*)_tmp15B)== 16: 0){
_LL30C: _tmp182=((struct Cyc_Absyn_Do_s_struct*)_tmp15B)->f1;goto _LL30A;_LL30A:
_tmp180=((struct Cyc_Absyn_Do_s_struct*)_tmp15B)->f2;_LL30B: _tmp181=_tmp180.f1;
goto _LL2E1;}else{goto _LL2E2;}_LL2E2: if((unsigned int)_tmp15B > 1?*((int*)_tmp15B)
== 17: 0){_LL30E: _tmp184=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp15B)->f1;goto
_LL30D;_LL30D: _tmp183=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp15B)->f2;goto
_LL2E3;}else{goto _LL2E4;}_LL2E4: if((unsigned int)_tmp15B > 1?*((int*)_tmp15B)== 
18: 0){_LL311: _tmp187=((struct Cyc_Absyn_Region_s_struct*)_tmp15B)->f1;goto _LL310;
_LL310: _tmp186=((struct Cyc_Absyn_Region_s_struct*)_tmp15B)->f2;goto _LL30F;_LL30F:
_tmp185=((struct Cyc_Absyn_Region_s_struct*)_tmp15B)->f3;goto _LL2E5;}else{goto
_LL2B9;}_LL2BB: s=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));goto _LL2B9;
_LL2BD: s=({struct Cyc_PP_Doc*_tmp188[2];_tmp188[1]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp188[0]=Cyc_Absynpp_exp2doc(_tmp15C);Cyc_PP_cat(
_tag_arr(_tmp188,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2B9;_LL2BF: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp15E)){s=({struct Cyc_PP_Doc*_tmp189[6];_tmp189[
5]=Cyc_Absynpp_is_declaration(_tmp15D)?({struct Cyc_PP_Doc*_tmp18A[4];_tmp18A[3]=
Cyc_PP_line_doc();_tmp18A[2]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));
_tmp18A[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp15D));_tmp18A[0]=Cyc_PP_text(
_tag_arr("{ ",sizeof(unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp18A,sizeof(
struct Cyc_PP_Doc*),4));}): Cyc_Absynpp_stmt2doc(_tmp15D);_tmp189[4]=Cyc_PP_line_doc();
_tmp189[3]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp189[2]=Cyc_PP_line_doc();
_tmp189[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp15E));_tmp189[0]=Cyc_PP_text(
_tag_arr("{ ",sizeof(unsigned char),3));Cyc_PP_cat(_tag_arr(_tmp189,sizeof(
struct Cyc_PP_Doc*),6));});}else{if(Cyc_Absynpp_is_declaration(_tmp15D)){s=({
struct Cyc_PP_Doc*_tmp18B[6];_tmp18B[5]=Cyc_PP_line_doc();_tmp18B[4]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp18B[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp15D));_tmp18B[2]=Cyc_PP_text(_tag_arr("{ ",sizeof(unsigned char),3));_tmp18B[
1]=Cyc_PP_line_doc();_tmp18B[0]=Cyc_Absynpp_stmt2doc(_tmp15E);Cyc_PP_cat(
_tag_arr(_tmp18B,sizeof(struct Cyc_PP_Doc*),6));});}else{s=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,_tag_arr("",sizeof(unsigned char),1),({
struct Cyc_List_List*_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C->hd=_tmp15E;
_tmp18C->tl=({struct Cyc_List_List*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->hd=
_tmp15D;_tmp18D->tl=0;_tmp18D;});_tmp18C;}));}}}else{s=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,_tag_arr("",sizeof(unsigned char),1),({
struct Cyc_List_List*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->hd=_tmp15E;
_tmp18E->tl=({struct Cyc_List_List*_tmp18F=_cycalloc(sizeof(*_tmp18F));_tmp18F->hd=
_tmp15D;_tmp18F->tl=0;_tmp18F;});_tmp18E;}));}goto _LL2B9;_LL2C1: if(_tmp15F == 0){
s=Cyc_PP_text(_tag_arr("return;",sizeof(unsigned char),8));}else{s=({struct Cyc_PP_Doc*
_tmp190[3];_tmp190[2]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmp190[
1]=_tmp15F == 0? Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmp15F));_tmp190[0]=Cyc_PP_text(_tag_arr("return ",sizeof(
unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp190,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL2B9;_LL2C3: {int print_else;{void*_tmp191=(void*)_tmp160->r;_LL313: if((int)
_tmp191 == 0){goto _LL314;}else{goto _LL315;}_LL315: goto _LL316;_LL314: print_else=0;
goto _LL312;_LL316: print_else=1;goto _LL312;_LL312:;}s=({struct Cyc_PP_Doc*_tmp192[
7];_tmp192[6]=print_else?({struct Cyc_PP_Doc*_tmp194[5];_tmp194[4]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp194[3]=Cyc_PP_line_doc();_tmp194[2]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp195[2];_tmp195[1]=Cyc_Absynpp_stmt2doc(
_tmp160);_tmp195[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp195,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp194[1]=Cyc_PP_text(_tag_arr("else {",sizeof(
unsigned char),7));_tmp194[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp194,
sizeof(struct Cyc_PP_Doc*),5));}): Cyc_PP_nil_doc();_tmp192[5]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp192[4]=Cyc_PP_line_doc();_tmp192[3]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp193[2];_tmp193[1]=Cyc_Absynpp_stmt2doc(
_tmp161);_tmp193[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp193,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp192[2]=Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),
4));_tmp192[1]=Cyc_Absynpp_exp2doc(_tmp162);_tmp192[0]=Cyc_PP_text(_tag_arr("if (",
sizeof(unsigned char),5));Cyc_PP_cat(_tag_arr(_tmp192,sizeof(struct Cyc_PP_Doc*),
7));});goto _LL2B9;}_LL2C5: s=({struct Cyc_PP_Doc*_tmp196[6];_tmp196[5]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp196[4]=Cyc_PP_line_doc();_tmp196[3]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp197[2];_tmp197[1]=Cyc_Absynpp_stmt2doc(
_tmp163);_tmp197[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp197,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp196[2]=Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),
4));_tmp196[1]=Cyc_Absynpp_exp2doc(_tmp165);_tmp196[0]=Cyc_PP_text(_tag_arr("while (",
sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp196,sizeof(struct Cyc_PP_Doc*),
6));});goto _LL2B9;_LL2C7: s=Cyc_PP_text(_tag_arr("break;",sizeof(unsigned char),7));
goto _LL2B9;_LL2C9: s=Cyc_PP_text(_tag_arr("continue;",sizeof(unsigned char),10));
goto _LL2B9;_LL2CB: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp199;_tmp199.tag=0;_tmp199.f1=(struct _tagged_arr)*_tmp166;{void*_tmp198[1]={&
_tmp199};Cyc_Std_aprintf(_tag_arr("goto %s;",sizeof(unsigned char),9),_tag_arr(
_tmp198,sizeof(void*),1));}}));goto _LL2B9;_LL2CD: s=({struct Cyc_PP_Doc*_tmp19A[10];
_tmp19A[9]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp19A[8]=Cyc_PP_line_doc();
_tmp19A[7]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp19B[2];_tmp19B[1]=Cyc_Absynpp_stmt2doc(
_tmp167);_tmp19B[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp19B,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp19A[6]=Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),
4));_tmp19A[5]=Cyc_Absynpp_exp2doc(_tmp169);_tmp19A[4]=Cyc_PP_text(_tag_arr("; ",
sizeof(unsigned char),3));_tmp19A[3]=Cyc_Absynpp_exp2doc(_tmp16B);_tmp19A[2]=Cyc_PP_text(
_tag_arr("; ",sizeof(unsigned char),3));_tmp19A[1]=Cyc_Absynpp_exp2doc(_tmp16C);
_tmp19A[0]=Cyc_PP_text(_tag_arr("for(",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp19A,sizeof(struct Cyc_PP_Doc*),10));});goto _LL2B9;_LL2CF: s=({struct
Cyc_PP_Doc*_tmp19C[9];_tmp19C[8]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),
2));_tmp19C[7]=Cyc_PP_line_doc();_tmp19C[6]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp19D[2];_tmp19D[1]=Cyc_Absynpp_stmt2doc(_tmp16E);_tmp19D[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp19D,sizeof(struct Cyc_PP_Doc*),2));}));_tmp19C[5]=Cyc_PP_text(
_tag_arr(") {",sizeof(unsigned char),4));_tmp19C[4]=Cyc_Absynpp_exp2doc(_tmp170);
_tmp19C[3]=Cyc_PP_text(_tag_arr("; ",sizeof(unsigned char),3));_tmp19C[2]=Cyc_Absynpp_exp2doc(
_tmp172);_tmp19C[1]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_vardecl2doc,
_tag_arr(" ",sizeof(unsigned char),2),_tmp173);_tmp19C[0]=Cyc_PP_text(_tag_arr("forarray(",
sizeof(unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp19C,sizeof(struct Cyc_PP_Doc*),
9));});goto _LL2B9;_LL2D1: s=({struct Cyc_PP_Doc*_tmp19E[7];_tmp19E[6]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp19E[5]=Cyc_PP_line_doc();_tmp19E[4]=
Cyc_Absynpp_switchclauses2doc(_tmp174);_tmp19E[3]=Cyc_PP_line_doc();_tmp19E[2]=
Cyc_PP_text(_tag_arr(") {",sizeof(unsigned char),4));_tmp19E[1]=Cyc_Absynpp_exp2doc(
_tmp175);_tmp19E[0]=Cyc_PP_text(_tag_arr("switch (",sizeof(unsigned char),9));
Cyc_PP_cat(_tag_arr(_tmp19E,sizeof(struct Cyc_PP_Doc*),7));});goto _LL2B9;_LL2D3: s=({
struct Cyc_PP_Doc*_tmp19F[7];_tmp19F[6]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp19F[5]=Cyc_PP_line_doc();_tmp19F[4]=Cyc_Absynpp_switchCclauses2doc(
_tmp176);_tmp19F[3]=Cyc_PP_line_doc();_tmp19F[2]=Cyc_PP_text(_tag_arr(") {",
sizeof(unsigned char),4));_tmp19F[1]=Cyc_Absynpp_exp2doc(_tmp177);_tmp19F[0]=Cyc_PP_text(
_tag_arr("switch \"C\" (",sizeof(unsigned char),13));Cyc_PP_cat(_tag_arr(_tmp19F,
sizeof(struct Cyc_PP_Doc*),7));});goto _LL2B9;_LL2D5: s=Cyc_PP_text(_tag_arr("fallthru;",
sizeof(unsigned char),10));goto _LL2B9;_LL2D7: s=({struct Cyc_PP_Doc*_tmp1A0[3];
_tmp1A0[2]=Cyc_PP_text(_tag_arr(");",sizeof(unsigned char),3));_tmp1A0[1]=Cyc_Absynpp_exps2doc_prec(
20,_tmp179);_tmp1A0[0]=Cyc_PP_text(_tag_arr("fallthru(",sizeof(unsigned char),10));
Cyc_PP_cat(_tag_arr(_tmp1A0,sizeof(struct Cyc_PP_Doc*),3));});goto _LL2B9;_LL2D9: s=({
struct Cyc_PP_Doc*_tmp1A1[3];_tmp1A1[2]=Cyc_Absynpp_stmt2doc(_tmp17A);_tmp1A1[1]=
Cyc_PP_line_doc();_tmp1A1[0]=Cyc_Absynpp_decl2doc(_tmp17B);Cyc_PP_cat(_tag_arr(
_tmp1A1,sizeof(struct Cyc_PP_Doc*),3));});goto _LL2B9;_LL2DB: s=({struct Cyc_PP_Doc*
_tmp1A2[2];_tmp1A2[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp17C));_tmp1A2[0]=
Cyc_PP_text(_tag_arr("cut ",sizeof(unsigned char),5));Cyc_PP_cat(_tag_arr(
_tmp1A2,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2B9;_LL2DD: s=({struct Cyc_PP_Doc*
_tmp1A3[2];_tmp1A3[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp17D));_tmp1A3[0]=
Cyc_PP_text(_tag_arr("splice ",sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(
_tmp1A3,sizeof(struct Cyc_PP_Doc*),2));});goto _LL2B9;_LL2DF: if(Cyc_Absynpp_decls_first?
Cyc_Absynpp_is_declaration(_tmp17E): 0){s=({struct Cyc_PP_Doc*_tmp1A4[6];_tmp1A4[5]=
Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp1A4[4]=Cyc_PP_line_doc();
_tmp1A4[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp17E));_tmp1A4[2]=Cyc_PP_line_doc();
_tmp1A4[1]=Cyc_PP_text(_tag_arr(": {",sizeof(unsigned char),4));_tmp1A4[0]=Cyc_PP_textptr(
_tmp17F);Cyc_PP_cat(_tag_arr(_tmp1A4,sizeof(struct Cyc_PP_Doc*),6));});}else{s=({
struct Cyc_PP_Doc*_tmp1A5[3];_tmp1A5[2]=Cyc_Absynpp_stmt2doc(_tmp17E);_tmp1A5[1]=
Cyc_PP_text(_tag_arr(": ",sizeof(unsigned char),3));_tmp1A5[0]=Cyc_PP_textptr(
_tmp17F);Cyc_PP_cat(_tag_arr(_tmp1A5,sizeof(struct Cyc_PP_Doc*),3));});}goto
_LL2B9;_LL2E1: s=({struct Cyc_PP_Doc*_tmp1A6[7];_tmp1A6[6]=Cyc_PP_text(_tag_arr(");",
sizeof(unsigned char),3));_tmp1A6[5]=Cyc_Absynpp_exp2doc(_tmp181);_tmp1A6[4]=Cyc_PP_text(
_tag_arr("} while (",sizeof(unsigned char),10));_tmp1A6[3]=Cyc_PP_line_doc();
_tmp1A6[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp182));_tmp1A6[1]=Cyc_PP_line_doc();
_tmp1A6[0]=Cyc_PP_text(_tag_arr("do {",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp1A6,sizeof(struct Cyc_PP_Doc*),7));});goto _LL2B9;_LL2E3: s=({struct
Cyc_PP_Doc*_tmp1A7[9];_tmp1A7[8]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),
2));_tmp1A7[7]=Cyc_PP_line_doc();_tmp1A7[6]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp183));_tmp1A7[5]=Cyc_PP_line_doc();_tmp1A7[4]=Cyc_PP_text(_tag_arr("} catch {",
sizeof(unsigned char),10));_tmp1A7[3]=Cyc_PP_line_doc();_tmp1A7[2]=Cyc_PP_nest(2,
Cyc_Absynpp_stmt2doc(_tmp184));_tmp1A7[1]=Cyc_PP_line_doc();_tmp1A7[0]=Cyc_PP_text(
_tag_arr("try {",sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp1A7,sizeof(
struct Cyc_PP_Doc*),9));});goto _LL2B9;_LL2E5: s=({struct Cyc_PP_Doc*_tmp1A8[9];
_tmp1A8[8]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp1A8[7]=Cyc_PP_line_doc();
_tmp1A8[6]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp185));_tmp1A8[5]=Cyc_PP_line_doc();
_tmp1A8[4]=Cyc_PP_text(_tag_arr("{",sizeof(unsigned char),2));_tmp1A8[3]=Cyc_Absynpp_qvar2doc(
_tmp186->name);_tmp1A8[2]=Cyc_PP_text(_tag_arr(">",sizeof(unsigned char),2));
_tmp1A8[1]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp187));_tmp1A8[0]=Cyc_PP_text(
_tag_arr("region<",sizeof(unsigned char),8));Cyc_PP_cat(_tag_arr(_tmp1A8,sizeof(
struct Cyc_PP_Doc*),9));});goto _LL2B9;_LL2B9:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(
struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{void*_tmp1A9=(void*)p->r;int _tmp1AA;
void*_tmp1AB;unsigned char _tmp1AC;struct _tagged_arr _tmp1AD;struct Cyc_Absyn_Vardecl*
_tmp1AE;struct Cyc_List_List*_tmp1AF;struct Cyc_Absyn_Pat*_tmp1B0;struct Cyc_Absyn_Vardecl*
_tmp1B1;struct _tuple0*_tmp1B2;struct Cyc_List_List*_tmp1B3;struct Cyc_List_List*
_tmp1B4;struct _tuple0*_tmp1B5;struct Cyc_List_List*_tmp1B6;struct Cyc_List_List*
_tmp1B7;struct _tuple0*_tmp1B8;struct Cyc_List_List*_tmp1B9;struct Cyc_List_List*
_tmp1BA;struct Cyc_Absyn_Structdecl*_tmp1BB;struct Cyc_Absyn_Enumfield*_tmp1BC;
struct Cyc_Absyn_Enumfield*_tmp1BD;struct Cyc_List_List*_tmp1BE;struct Cyc_List_List*
_tmp1BF;struct Cyc_Absyn_Tunionfield*_tmp1C0;_LL318: if((int)_tmp1A9 == 0){goto
_LL319;}else{goto _LL31A;}_LL31A: if((int)_tmp1A9 == 1){goto _LL31B;}else{goto _LL31C;}
_LL31C: if((unsigned int)_tmp1A9 > 2?*((int*)_tmp1A9)== 1: 0){_LL339: _tmp1AB=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp1A9)->f1;goto _LL338;_LL338: _tmp1AA=((struct Cyc_Absyn_Int_p_struct*)
_tmp1A9)->f2;goto _LL31D;}else{goto _LL31E;}_LL31E: if((unsigned int)_tmp1A9 > 2?*((
int*)_tmp1A9)== 2: 0){_LL33A: _tmp1AC=((struct Cyc_Absyn_Char_p_struct*)_tmp1A9)->f1;
goto _LL31F;}else{goto _LL320;}_LL320: if((unsigned int)_tmp1A9 > 2?*((int*)_tmp1A9)
== 3: 0){_LL33B: _tmp1AD=((struct Cyc_Absyn_Float_p_struct*)_tmp1A9)->f1;goto _LL321;}
else{goto _LL322;}_LL322: if((unsigned int)_tmp1A9 > 2?*((int*)_tmp1A9)== 0: 0){
_LL33C: _tmp1AE=((struct Cyc_Absyn_Var_p_struct*)_tmp1A9)->f1;goto _LL323;}else{
goto _LL324;}_LL324: if((unsigned int)_tmp1A9 > 2?*((int*)_tmp1A9)== 4: 0){_LL33D:
_tmp1AF=((struct Cyc_Absyn_Tuple_p_struct*)_tmp1A9)->f1;goto _LL325;}else{goto
_LL326;}_LL326: if((unsigned int)_tmp1A9 > 2?*((int*)_tmp1A9)== 5: 0){_LL33E:
_tmp1B0=((struct Cyc_Absyn_Pointer_p_struct*)_tmp1A9)->f1;goto _LL327;}else{goto
_LL328;}_LL328: if((unsigned int)_tmp1A9 > 2?*((int*)_tmp1A9)== 6: 0){_LL33F:
_tmp1B1=((struct Cyc_Absyn_Reference_p_struct*)_tmp1A9)->f1;goto _LL329;}else{goto
_LL32A;}_LL32A: if((unsigned int)_tmp1A9 > 2?*((int*)_tmp1A9)== 11: 0){_LL340:
_tmp1B2=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp1A9)->f1;goto _LL32B;}else{goto
_LL32C;}_LL32C: if((unsigned int)_tmp1A9 > 2?*((int*)_tmp1A9)== 12: 0){_LL343:
_tmp1B5=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1A9)->f1;goto _LL342;_LL342:
_tmp1B4=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1A9)->f2;goto _LL341;_LL341:
_tmp1B3=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1A9)->f3;goto _LL32D;}else{
goto _LL32E;}_LL32E: if((unsigned int)_tmp1A9 > 2?*((int*)_tmp1A9)== 13: 0){_LL346:
_tmp1B8=((struct Cyc_Absyn_UnknownFields_p_struct*)_tmp1A9)->f1;goto _LL345;_LL345:
_tmp1B7=((struct Cyc_Absyn_UnknownFields_p_struct*)_tmp1A9)->f2;goto _LL344;_LL344:
_tmp1B6=((struct Cyc_Absyn_UnknownFields_p_struct*)_tmp1A9)->f3;goto _LL32F;}else{
goto _LL330;}_LL330: if((unsigned int)_tmp1A9 > 2?*((int*)_tmp1A9)== 7: 0){_LL349:
_tmp1BB=((struct Cyc_Absyn_Struct_p_struct*)_tmp1A9)->f1;goto _LL348;_LL348:
_tmp1BA=((struct Cyc_Absyn_Struct_p_struct*)_tmp1A9)->f3;goto _LL347;_LL347:
_tmp1B9=((struct Cyc_Absyn_Struct_p_struct*)_tmp1A9)->f4;goto _LL331;}else{goto
_LL332;}_LL332: if((unsigned int)_tmp1A9 > 2?*((int*)_tmp1A9)== 9: 0){_LL34A:
_tmp1BC=((struct Cyc_Absyn_Enum_p_struct*)_tmp1A9)->f2;goto _LL333;}else{goto
_LL334;}_LL334: if((unsigned int)_tmp1A9 > 2?*((int*)_tmp1A9)== 10: 0){_LL34B:
_tmp1BD=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp1A9)->f2;goto _LL335;}else{goto
_LL336;}_LL336: if((unsigned int)_tmp1A9 > 2?*((int*)_tmp1A9)== 8: 0){_LL34E:
_tmp1C0=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1A9)->f2;goto _LL34D;_LL34D:
_tmp1BF=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1A9)->f3;goto _LL34C;_LL34C:
_tmp1BE=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1A9)->f4;goto _LL337;}else{goto
_LL317;}_LL319: s=Cyc_PP_text(_tag_arr("_",sizeof(unsigned char),2));goto _LL317;
_LL31B: s=Cyc_PP_text(_tag_arr("NULL",sizeof(unsigned char),5));goto _LL317;_LL31D:
if(_tmp1AB == (void*)0){s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp1C2;_tmp1C2.tag=1;_tmp1C2.f1=(int)((unsigned int)_tmp1AA);{void*_tmp1C1[1]={&
_tmp1C2};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp1C1,
sizeof(void*),1));}}));}else{s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp1C4;_tmp1C4.tag=1;_tmp1C4.f1=(unsigned int)_tmp1AA;{void*_tmp1C3[1]={&
_tmp1C4};Cyc_Std_aprintf(_tag_arr("%u",sizeof(unsigned char),3),_tag_arr(_tmp1C3,
sizeof(void*),1));}}));}goto _LL317;_LL31F: s=Cyc_PP_text((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp1C6;_tmp1C6.tag=0;_tmp1C6.f1=(struct
_tagged_arr)Cyc_Absynpp_char_escape(_tmp1AC);{void*_tmp1C5[1]={& _tmp1C6};Cyc_Std_aprintf(
_tag_arr("'%s'",sizeof(unsigned char),5),_tag_arr(_tmp1C5,sizeof(void*),1));}}));
goto _LL317;_LL321: s=Cyc_PP_text(_tmp1AD);goto _LL317;_LL323: s=Cyc_Absynpp_qvar2doc(
_tmp1AE->name);goto _LL317;_LL325: s=({struct Cyc_PP_Doc*_tmp1C7[3];_tmp1C7[2]=Cyc_PP_text(
_tag_arr(")",sizeof(unsigned char),2));_tmp1C7[1]=((struct Cyc_PP_Doc*(*)(struct
Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,_tag_arr(",",sizeof(unsigned char),2),
_tmp1AF);_tmp1C7[0]=Cyc_PP_text(_tag_arr("$(",sizeof(unsigned char),3));Cyc_PP_cat(
_tag_arr(_tmp1C7,sizeof(struct Cyc_PP_Doc*),3));});goto _LL317;_LL327: s=({struct
Cyc_PP_Doc*_tmp1C8[2];_tmp1C8[1]=Cyc_Absynpp_pat2doc(_tmp1B0);_tmp1C8[0]=Cyc_PP_text(
_tag_arr("&",sizeof(unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp1C8,sizeof(struct
Cyc_PP_Doc*),2));});goto _LL317;_LL329: s=({struct Cyc_PP_Doc*_tmp1C9[2];_tmp1C9[1]=
Cyc_Absynpp_qvar2doc(_tmp1B1->name);_tmp1C9[0]=Cyc_PP_text(_tag_arr("*",sizeof(
unsigned char),2));Cyc_PP_cat(_tag_arr(_tmp1C9,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL317;_LL32B: s=Cyc_Absynpp_qvar2doc(_tmp1B2);goto _LL317;_LL32D: s=({struct
Cyc_PP_Doc*_tmp1CA[3];_tmp1CA[2]=Cyc_PP_group(_tag_arr("(",sizeof(unsigned char),
2),_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp1B3));_tmp1CA[1]=Cyc_Absynpp_tvars2doc(
_tmp1B4);_tmp1CA[0]=Cyc_Absynpp_qvar2doc(_tmp1B5);Cyc_PP_cat(_tag_arr(_tmp1CA,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL317;_LL32F: s=({struct Cyc_PP_Doc*_tmp1CB[
3];_tmp1CB[2]=Cyc_PP_group(_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,
_tmp1B6));_tmp1CB[1]=Cyc_Absynpp_tvars2doc(_tmp1B7);_tmp1CB[0]=Cyc_Absynpp_qvar2doc(
_tmp1B8);Cyc_PP_cat(_tag_arr(_tmp1CB,sizeof(struct Cyc_PP_Doc*),3));});goto _LL317;
_LL331: s=({struct Cyc_PP_Doc*_tmp1CC[3];_tmp1CC[2]=Cyc_PP_group(_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp1B9));
_tmp1CC[1]=Cyc_Absynpp_tvars2doc(_tmp1BA);_tmp1CC[0]=_tmp1BB->name == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_qvar2doc((struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp1BB->name))->v);
Cyc_PP_cat(_tag_arr(_tmp1CC,sizeof(struct Cyc_PP_Doc*),3));});goto _LL317;_LL333: s=
Cyc_Absynpp_qvar2doc(_tmp1BC->name);goto _LL317;_LL335: s=Cyc_Absynpp_qvar2doc(
_tmp1BD->name);goto _LL317;_LL337: if(_tmp1BE == 0){s=Cyc_Absynpp_qvar2doc(_tmp1C0->name);}
else{s=({struct Cyc_PP_Doc*_tmp1CD[3];_tmp1CD[2]=Cyc_PP_egroup(_tag_arr("(",
sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp1BE));
_tmp1CD[1]=Cyc_Absynpp_tvars2doc(_tmp1BF);_tmp1CD[0]=Cyc_Absynpp_qvar2doc(
_tmp1C0->name);Cyc_PP_cat(_tag_arr(_tmp1CD,sizeof(struct Cyc_PP_Doc*),3));});}
goto _LL317;_LL317:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple5*
dp){return({struct Cyc_PP_Doc*_tmp1CE[2];_tmp1CE[1]=Cyc_Absynpp_pat2doc((*dp).f2);
_tmp1CE[0]=Cyc_PP_egroup(_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",
sizeof(unsigned char),2),_tag_arr("=",sizeof(unsigned char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*
dp).f1));Cyc_PP_cat(_tag_arr(_tmp1CE,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*
Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){if(c->where_clause
== 0?(void*)(c->pattern)->r == (void*)0: 0){return({struct Cyc_PP_Doc*_tmp1CF[2];
_tmp1CF[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1D0[2];_tmp1D0[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp1D0[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1D0,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp1CF[0]=Cyc_PP_text(_tag_arr("default: ",sizeof(
unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp1CF,sizeof(struct Cyc_PP_Doc*),2));});}
else{if(c->where_clause == 0){return({struct Cyc_PP_Doc*_tmp1D1[4];_tmp1D1[3]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp1D2[2];_tmp1D2[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1D2[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1D2,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp1D1[2]=Cyc_PP_text(_tag_arr(": ",sizeof(unsigned char),3));_tmp1D1[1]=Cyc_Absynpp_pat2doc(
c->pattern);_tmp1D1[0]=Cyc_PP_text(_tag_arr("case ",sizeof(unsigned char),6));
Cyc_PP_cat(_tag_arr(_tmp1D1,sizeof(struct Cyc_PP_Doc*),4));});}else{return({
struct Cyc_PP_Doc*_tmp1D3[6];_tmp1D3[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1D4[
2];_tmp1D4[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1D4[0]=Cyc_PP_line_doc();Cyc_PP_cat(
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
_tmp1D6;struct Cyc_Absyn_Stmt*_tmp1D7;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_Absyn_SwitchC_clause*
_tmp1D5=c;_tmp1D6=*_tmp1D5;_LL351: _tmp1D8=_tmp1D6.cnst_exp;goto _LL350;_LL350:
_tmp1D7=_tmp1D6.body;goto _LL34F;_LL34F: if(_tmp1D8 == 0){return({struct Cyc_PP_Doc*
_tmp1D9[2];_tmp1D9[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1DA[2];_tmp1DA[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp1DA[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1DA,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp1D9[0]=Cyc_PP_text(_tag_arr("default: ",sizeof(
unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp1D9,sizeof(struct Cyc_PP_Doc*),2));});}
else{return({struct Cyc_PP_Doc*_tmp1DB[4];_tmp1DB[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp1DC[2];_tmp1DC[1]=Cyc_Absynpp_stmt2doc(_tmp1D7);_tmp1DC[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp1DC,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1DB[2]=Cyc_PP_text(
_tag_arr(": ",sizeof(unsigned char),3));_tmp1DB[1]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(_tmp1D8));_tmp1DB[0]=Cyc_PP_text(_tag_arr("case ",
sizeof(unsigned char),6));Cyc_PP_cat(_tag_arr(_tmp1DB,sizeof(struct Cyc_PP_Doc*),
4));});}}struct Cyc_PP_Doc*Cyc_Absynpp_switchCclauses2doc(struct Cyc_List_List*cs){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_SwitchC_clause*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchCclause2doc,
_tag_arr("",sizeof(unsigned char),1),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f){if(f->tag == 0){return Cyc_Absynpp_qvar2doc(f->name);}
else{return({struct Cyc_PP_Doc*_tmp1DD[3];_tmp1DD[2]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(f->tag));_tmp1DD[1]=Cyc_PP_text(_tag_arr(" = ",
sizeof(unsigned char),4));_tmp1DD[0]=Cyc_Absynpp_qvar2doc(f->name);Cyc_PP_cat(
_tag_arr(_tmp1DD,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Enumfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_enumfield2doc,_tag_arr(",",sizeof(unsigned char),2),fs);}static
struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(
v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,
_tag_arr(",",sizeof(unsigned char),2),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(
struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl _tmp1DF;struct Cyc_List_List*
_tmp1E0;struct Cyc_Absyn_Exp*_tmp1E1;void*_tmp1E2;struct Cyc_Absyn_Tqual _tmp1E3;
struct _tuple0*_tmp1E4;void*_tmp1E5;struct Cyc_Absyn_Vardecl*_tmp1DE=vd;_tmp1DF=*
_tmp1DE;_LL358: _tmp1E5=(void*)_tmp1DF.sc;goto _LL357;_LL357: _tmp1E4=_tmp1DF.name;
goto _LL356;_LL356: _tmp1E3=_tmp1DF.tq;goto _LL355;_LL355: _tmp1E2=(void*)_tmp1DF.type;
goto _LL354;_LL354: _tmp1E1=_tmp1DF.initializer;goto _LL353;_LL353: _tmp1E0=_tmp1DF.attributes;
goto _LL352;_LL352: {struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2doc(
_tmp1E4);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp1E0);struct Cyc_PP_Doc*
beforenamedoc;if(! Cyc_Absynpp_to_VC){beforenamedoc=attsdoc;}else{void*_tmp1E6=
Cyc_Tcutil_compress(_tmp1E2);struct Cyc_Absyn_FnInfo _tmp1E7;struct Cyc_List_List*
_tmp1E8;_LL35A: if((unsigned int)_tmp1E6 > 3?*((int*)_tmp1E6)== 8: 0){_LL35E:
_tmp1E7=((struct Cyc_Absyn_FnType_struct*)_tmp1E6)->f1;_LL35F: _tmp1E8=_tmp1E7.attributes;
goto _LL35B;}else{goto _LL35C;}_LL35C: goto _LL35D;_LL35B: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp1E8);goto _LL359;_LL35D: beforenamedoc=Cyc_PP_nil_doc();goto _LL359;_LL359:;}s=({
struct Cyc_PP_Doc*_tmp1E9[5];_tmp1E9[4]=Cyc_PP_text(_tag_arr(";",sizeof(
unsigned char),2));_tmp1E9[3]=_tmp1E1 == 0? Cyc_PP_text(_tag_arr("",sizeof(
unsigned char),1)):({struct Cyc_PP_Doc*_tmp1EC[2];_tmp1EC[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(_tmp1E1));_tmp1EC[0]=Cyc_PP_text(_tag_arr(" = ",
sizeof(unsigned char),4));Cyc_PP_cat(_tag_arr(_tmp1EC,sizeof(struct Cyc_PP_Doc*),
2));});_tmp1E9[2]=Cyc_Absynpp_tqtd2doc(_tmp1E3,_tmp1E2,({struct Cyc_Core_Opt*
_tmp1EA=_cycalloc(sizeof(*_tmp1EA));_tmp1EA->v=({struct Cyc_PP_Doc*_tmp1EB[2];
_tmp1EB[1]=sn;_tmp1EB[0]=beforenamedoc;Cyc_PP_cat(_tag_arr(_tmp1EB,sizeof(struct
Cyc_PP_Doc*),2));});_tmp1EA;}));_tmp1E9[1]=Cyc_Absynpp_scope2doc(_tmp1E5);
_tmp1E9[0]=Cyc_Absynpp_to_VC? attsdoc: Cyc_PP_nil_doc();Cyc_PP_cat(_tag_arr(
_tmp1E9,sizeof(struct Cyc_PP_Doc*),5));});return s;}}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp1ED=(void*)d->r;struct Cyc_Absyn_Fndecl*
_tmp1EE;struct Cyc_Absyn_Structdecl*_tmp1EF;struct Cyc_Absyn_Uniondecl*_tmp1F0;
struct Cyc_Absyn_Vardecl*_tmp1F1;struct Cyc_Absyn_Tuniondecl*_tmp1F2;struct Cyc_Absyn_Tuniondecl
_tmp1F3;int _tmp1F4;struct Cyc_Core_Opt*_tmp1F5;struct Cyc_List_List*_tmp1F6;struct
_tuple0*_tmp1F7;void*_tmp1F8;struct Cyc_Absyn_Exp*_tmp1F9;struct Cyc_Absyn_Pat*
_tmp1FA;struct Cyc_List_List*_tmp1FB;struct Cyc_Absyn_Enumdecl*_tmp1FC;struct Cyc_Absyn_Enumdecl
_tmp1FD;struct Cyc_Core_Opt*_tmp1FE;struct _tuple0*_tmp1FF;void*_tmp200;struct Cyc_Absyn_Typedefdecl*
_tmp201;struct Cyc_List_List*_tmp202;struct _tagged_arr*_tmp203;struct Cyc_List_List*
_tmp204;struct _tuple0*_tmp205;struct Cyc_List_List*_tmp206;_LL361: if(*((int*)
_tmp1ED)== 1){_LL379: _tmp1EE=((struct Cyc_Absyn_Fn_d_struct*)_tmp1ED)->f1;goto
_LL362;}else{goto _LL363;}_LL363: if(*((int*)_tmp1ED)== 4){_LL37A: _tmp1EF=((struct
Cyc_Absyn_Struct_d_struct*)_tmp1ED)->f1;goto _LL364;}else{goto _LL365;}_LL365: if(*((
int*)_tmp1ED)== 5){_LL37B: _tmp1F0=((struct Cyc_Absyn_Union_d_struct*)_tmp1ED)->f1;
goto _LL366;}else{goto _LL367;}_LL367: if(*((int*)_tmp1ED)== 0){_LL37C: _tmp1F1=((
struct Cyc_Absyn_Var_d_struct*)_tmp1ED)->f1;goto _LL368;}else{goto _LL369;}_LL369:
if(*((int*)_tmp1ED)== 6){_LL37D: _tmp1F2=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp1ED)->f1;_tmp1F3=*_tmp1F2;_LL382: _tmp1F8=(void*)_tmp1F3.sc;goto _LL381;_LL381:
_tmp1F7=_tmp1F3.name;goto _LL380;_LL380: _tmp1F6=_tmp1F3.tvs;goto _LL37F;_LL37F:
_tmp1F5=_tmp1F3.fields;goto _LL37E;_LL37E: _tmp1F4=_tmp1F3.is_xtunion;goto _LL36A;}
else{goto _LL36B;}_LL36B: if(*((int*)_tmp1ED)== 2){_LL384: _tmp1FA=((struct Cyc_Absyn_Let_d_struct*)
_tmp1ED)->f1;goto _LL383;_LL383: _tmp1F9=((struct Cyc_Absyn_Let_d_struct*)_tmp1ED)->f4;
goto _LL36C;}else{goto _LL36D;}_LL36D: if(*((int*)_tmp1ED)== 3){_LL385: _tmp1FB=((
struct Cyc_Absyn_Letv_d_struct*)_tmp1ED)->f1;goto _LL36E;}else{goto _LL36F;}_LL36F:
if(*((int*)_tmp1ED)== 7){_LL386: _tmp1FC=((struct Cyc_Absyn_Enum_d_struct*)_tmp1ED)->f1;
_tmp1FD=*_tmp1FC;_LL389: _tmp200=(void*)_tmp1FD.sc;goto _LL388;_LL388: _tmp1FF=
_tmp1FD.name;goto _LL387;_LL387: _tmp1FE=_tmp1FD.fields;goto _LL370;}else{goto
_LL371;}_LL371: if(*((int*)_tmp1ED)== 8){_LL38A: _tmp201=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp1ED)->f1;goto _LL372;}else{goto _LL373;}_LL373: if(*((int*)_tmp1ED)== 9){_LL38C:
_tmp203=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1ED)->f1;goto _LL38B;_LL38B:
_tmp202=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1ED)->f2;goto _LL374;}else{goto
_LL375;}_LL375: if(*((int*)_tmp1ED)== 10){_LL38E: _tmp205=((struct Cyc_Absyn_Using_d_struct*)
_tmp1ED)->f1;goto _LL38D;_LL38D: _tmp204=((struct Cyc_Absyn_Using_d_struct*)_tmp1ED)->f2;
goto _LL376;}else{goto _LL377;}_LL377: if(*((int*)_tmp1ED)== 11){_LL38F: _tmp206=((
struct Cyc_Absyn_ExternC_d_struct*)_tmp1ED)->f1;goto _LL378;}else{goto _LL360;}
_LL362: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp20E=_cycalloc(sizeof(*
_tmp20E));_tmp20E[0]=({struct Cyc_Absyn_FnType_struct _tmp20F;_tmp20F.tag=8;
_tmp20F.f1=({struct Cyc_Absyn_FnInfo _tmp210;_tmp210.tvars=_tmp1EE->tvs;_tmp210.effect=
_tmp1EE->effect;_tmp210.ret_typ=(void*)((void*)_tmp1EE->ret_type);_tmp210.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple3*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp1EE->args);_tmp210.c_varargs=_tmp1EE->c_varargs;
_tmp210.cyc_varargs=_tmp1EE->cyc_varargs;_tmp210.rgn_po=_tmp1EE->rgn_po;_tmp210.attributes=
0;_tmp210;});_tmp20F;});_tmp20E;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp1EE->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp1EE->is_inline){if(Cyc_Absynpp_to_VC){
inlinedoc=Cyc_PP_text(_tag_arr("__inline ",sizeof(unsigned char),10));}else{
inlinedoc=Cyc_PP_text(_tag_arr("inline ",sizeof(unsigned char),8));}}else{
inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((
void*)_tmp1EE->sc);struct Cyc_PP_Doc*beforenamedoc=Cyc_Absynpp_to_VC? Cyc_Absynpp_callconv2doc(
_tmp1EE->attributes): attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp1EE->name);struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual
_tmp20B;_tmp20B.q_const=0;_tmp20B.q_volatile=0;_tmp20B.q_restrict=0;_tmp20B;}),t,({
struct Cyc_Core_Opt*_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C->v=({struct Cyc_PP_Doc*
_tmp20D[2];_tmp20D[1]=namedoc;_tmp20D[0]=beforenamedoc;Cyc_PP_cat(_tag_arr(
_tmp20D,sizeof(struct Cyc_PP_Doc*),2));});_tmp20C;}));struct Cyc_PP_Doc*bodydoc=({
struct Cyc_PP_Doc*_tmp209[4];_tmp209[3]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp209[2]=Cyc_PP_line_doc();_tmp209[1]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp20A[2];_tmp20A[1]=Cyc_Absynpp_stmt2doc(_tmp1EE->body);
_tmp20A[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp20A,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp209[0]=Cyc_PP_text(_tag_arr(" {",sizeof(unsigned char),3));Cyc_PP_cat(
_tag_arr(_tmp209,sizeof(struct Cyc_PP_Doc*),4));});s=({struct Cyc_PP_Doc*_tmp207[4];
_tmp207[3]=bodydoc;_tmp207[2]=tqtddoc;_tmp207[1]=scopedoc;_tmp207[0]=inlinedoc;
Cyc_PP_cat(_tag_arr(_tmp207,sizeof(struct Cyc_PP_Doc*),4));});if(Cyc_Absynpp_to_VC){
s=({struct Cyc_PP_Doc*_tmp208[2];_tmp208[1]=s;_tmp208[0]=attsdoc;Cyc_PP_cat(
_tag_arr(_tmp208,sizeof(struct Cyc_PP_Doc*),2));});}goto _LL360;}}_LL364: if(
_tmp1EF->fields == 0){s=({struct Cyc_PP_Doc*_tmp211[5];_tmp211[4]=Cyc_PP_text(
_tag_arr(";",sizeof(unsigned char),2));_tmp211[3]=Cyc_Absynpp_ktvars2doc(_tmp1EF->tvs);
_tmp211[2]=_tmp1EF->name == 0? Cyc_PP_text(_tag_arr("",sizeof(unsigned char),1)):
Cyc_Absynpp_typedef_name2doc((struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(
_tmp1EF->name))->v);_tmp211[1]=Cyc_PP_text(_tag_arr("struct ",sizeof(
unsigned char),8));_tmp211[0]=Cyc_Absynpp_scope2doc((void*)_tmp1EF->sc);Cyc_PP_cat(
_tag_arr(_tmp211,sizeof(struct Cyc_PP_Doc*),5));});}else{s=({struct Cyc_PP_Doc*
_tmp212[10];_tmp212[9]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));
_tmp212[8]=Cyc_Absynpp_atts2doc(_tmp1EF->attributes);_tmp212[7]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp212[6]=Cyc_PP_line_doc();_tmp212[5]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp213[2];_tmp213[1]=Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1EF->fields))->v);
_tmp213[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp213,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp212[4]=Cyc_PP_text(_tag_arr(" {",sizeof(unsigned char),3));_tmp212[3]=
Cyc_Absynpp_ktvars2doc(_tmp1EF->tvs);_tmp212[2]=_tmp1EF->name == 0? Cyc_PP_text(
_tag_arr("",sizeof(unsigned char),1)): Cyc_Absynpp_typedef_name2doc((struct
_tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmp1EF->name))->v);_tmp212[1]=Cyc_PP_text(
_tag_arr("struct ",sizeof(unsigned char),8));_tmp212[0]=Cyc_Absynpp_scope2doc((
void*)_tmp1EF->sc);Cyc_PP_cat(_tag_arr(_tmp212,sizeof(struct Cyc_PP_Doc*),10));});}
goto _LL360;_LL366: if(_tmp1F0->fields == 0){s=({struct Cyc_PP_Doc*_tmp214[5];
_tmp214[4]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmp214[3]=Cyc_Absynpp_tvars2doc(
_tmp1F0->tvs);_tmp214[2]=_tmp1F0->name == 0? Cyc_PP_text(_tag_arr("",sizeof(
unsigned char),1)): Cyc_Absynpp_qvar2doc((struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(_tmp1F0->name))->v);_tmp214[1]=Cyc_PP_text(_tag_arr("union ",sizeof(
unsigned char),7));_tmp214[0]=Cyc_Absynpp_scope2doc((void*)_tmp1F0->sc);Cyc_PP_cat(
_tag_arr(_tmp214,sizeof(struct Cyc_PP_Doc*),5));});}else{s=({struct Cyc_PP_Doc*
_tmp215[10];_tmp215[9]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));
_tmp215[8]=Cyc_Absynpp_atts2doc(_tmp1F0->attributes);_tmp215[7]=Cyc_PP_text(
_tag_arr("}",sizeof(unsigned char),2));_tmp215[6]=Cyc_PP_line_doc();_tmp215[5]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp216[2];_tmp216[1]=Cyc_Absynpp_structfields2doc((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1F0->fields))->v);
_tmp216[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp216,sizeof(struct Cyc_PP_Doc*),
2));}));_tmp215[4]=Cyc_PP_text(_tag_arr(" {",sizeof(unsigned char),3));_tmp215[3]=
Cyc_Absynpp_tvars2doc(_tmp1F0->tvs);_tmp215[2]=_tmp1F0->name == 0? Cyc_PP_text(
_tag_arr("",sizeof(unsigned char),1)): Cyc_Absynpp_qvar2doc((struct _tuple0*)((
struct Cyc_Core_Opt*)_check_null(_tmp1F0->name))->v);_tmp215[1]=Cyc_PP_text(
_tag_arr("union ",sizeof(unsigned char),7));_tmp215[0]=Cyc_Absynpp_scope2doc((
void*)_tmp1F0->sc);Cyc_PP_cat(_tag_arr(_tmp215,sizeof(struct Cyc_PP_Doc*),10));});}
goto _LL360;_LL368: s=Cyc_Absynpp_vardecl2doc(_tmp1F1);goto _LL360;_LL36A: if(
_tmp1F5 == 0){s=({struct Cyc_PP_Doc*_tmp217[5];_tmp217[4]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp217[3]=Cyc_Absynpp_ktvars2doc(_tmp1F6);_tmp217[2]=
_tmp1F4? Cyc_Absynpp_qvar2doc(_tmp1F7): Cyc_Absynpp_typedef_name2doc(_tmp1F7);
_tmp217[1]=_tmp1F4? Cyc_PP_text(_tag_arr("xtunion ",sizeof(unsigned char),9)): Cyc_PP_text(
_tag_arr("tunion ",sizeof(unsigned char),8));_tmp217[0]=Cyc_Absynpp_scope2doc(
_tmp1F8);Cyc_PP_cat(_tag_arr(_tmp217,sizeof(struct Cyc_PP_Doc*),5));});}else{s=({
struct Cyc_PP_Doc*_tmp218[8];_tmp218[7]=Cyc_PP_text(_tag_arr("};",sizeof(
unsigned char),3));_tmp218[6]=Cyc_PP_line_doc();_tmp218[5]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp219[2];_tmp219[1]=Cyc_Absynpp_tunionfields2doc((struct Cyc_List_List*)
_tmp1F5->v);_tmp219[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp219,sizeof(
struct Cyc_PP_Doc*),2));}));_tmp218[4]=Cyc_PP_text(_tag_arr(" {",sizeof(
unsigned char),3));_tmp218[3]=Cyc_Absynpp_ktvars2doc(_tmp1F6);_tmp218[2]=_tmp1F4?
Cyc_Absynpp_qvar2doc(_tmp1F7): Cyc_Absynpp_typedef_name2doc(_tmp1F7);_tmp218[1]=
_tmp1F4? Cyc_PP_text(_tag_arr("xtunion ",sizeof(unsigned char),9)): Cyc_PP_text(
_tag_arr("tunion ",sizeof(unsigned char),8));_tmp218[0]=Cyc_Absynpp_scope2doc(
_tmp1F8);Cyc_PP_cat(_tag_arr(_tmp218,sizeof(struct Cyc_PP_Doc*),8));});}goto
_LL360;_LL36C: s=({struct Cyc_PP_Doc*_tmp21A[5];_tmp21A[4]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp21A[3]=Cyc_Absynpp_exp2doc(_tmp1F9);_tmp21A[2]=Cyc_PP_text(
_tag_arr(" = ",sizeof(unsigned char),4));_tmp21A[1]=Cyc_Absynpp_pat2doc(_tmp1FA);
_tmp21A[0]=Cyc_PP_text(_tag_arr("let ",sizeof(unsigned char),5));Cyc_PP_cat(
_tag_arr(_tmp21A,sizeof(struct Cyc_PP_Doc*),5));});goto _LL360;_LL36E: s=({struct
Cyc_PP_Doc*_tmp21B[3];_tmp21B[2]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),
2));_tmp21B[1]=Cyc_Absynpp_ids2doc(_tmp1FB);_tmp21B[0]=Cyc_PP_text(_tag_arr("let ",
sizeof(unsigned char),5));Cyc_PP_cat(_tag_arr(_tmp21B,sizeof(struct Cyc_PP_Doc*),
3));});goto _LL360;_LL370: if(_tmp1FE == 0){s=({struct Cyc_PP_Doc*_tmp21C[4];_tmp21C[
3]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmp21C[2]=Cyc_Absynpp_typedef_name2doc(
_tmp1FF);_tmp21C[1]=Cyc_PP_text(_tag_arr("enum ",sizeof(unsigned char),6));
_tmp21C[0]=Cyc_Absynpp_scope2doc(_tmp200);Cyc_PP_cat(_tag_arr(_tmp21C,sizeof(
struct Cyc_PP_Doc*),4));});}else{s=({struct Cyc_PP_Doc*_tmp21D[7];_tmp21D[6]=Cyc_PP_text(
_tag_arr("};",sizeof(unsigned char),3));_tmp21D[5]=Cyc_PP_line_doc();_tmp21D[4]=
Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp21E[2];_tmp21E[1]=Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*)_tmp1FE->v);_tmp21E[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp21E,sizeof(struct Cyc_PP_Doc*),2));}));_tmp21D[3]=Cyc_PP_text(
_tag_arr(" {",sizeof(unsigned char),3));_tmp21D[2]=Cyc_Absynpp_qvar2doc(_tmp1FF);
_tmp21D[1]=Cyc_PP_text(_tag_arr("enum ",sizeof(unsigned char),6));_tmp21D[0]=Cyc_Absynpp_scope2doc(
_tmp200);Cyc_PP_cat(_tag_arr(_tmp21D,sizeof(struct Cyc_PP_Doc*),7));});}goto
_LL360;_LL372: s=({struct Cyc_PP_Doc*_tmp21F[3];_tmp21F[2]=Cyc_PP_text(_tag_arr(";",
sizeof(unsigned char),2));_tmp21F[1]=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual
_tmp220;_tmp220.q_const=0;_tmp220.q_volatile=0;_tmp220.q_restrict=0;_tmp220;}),(
void*)_tmp201->defn,({struct Cyc_Core_Opt*_tmp221=_cycalloc(sizeof(*_tmp221));
_tmp221->v=({struct Cyc_PP_Doc*_tmp222[2];_tmp222[1]=Cyc_Absynpp_tvars2doc(
_tmp201->tvs);_tmp222[0]=Cyc_Absynpp_typedef_name2doc(_tmp201->name);Cyc_PP_cat(
_tag_arr(_tmp222,sizeof(struct Cyc_PP_Doc*),2));});_tmp221;}));_tmp21F[0]=Cyc_PP_text(
_tag_arr("typedef ",sizeof(unsigned char),9));Cyc_PP_cat(_tag_arr(_tmp21F,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL360;_LL374: if(Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_add(_tmp203);}s=({struct Cyc_PP_Doc*_tmp223[7];_tmp223[
6]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp223[5]=Cyc_PP_line_doc();
_tmp223[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(unsigned char),1),_tmp202);_tmp223[3]=Cyc_PP_line_doc();
_tmp223[2]=Cyc_PP_text(_tag_arr(" {",sizeof(unsigned char),3));_tmp223[1]=Cyc_PP_textptr(
_tmp203);_tmp223[0]=Cyc_PP_text(_tag_arr("namespace ",sizeof(unsigned char),11));
Cyc_PP_cat(_tag_arr(_tmp223,sizeof(struct Cyc_PP_Doc*),7));});if(Cyc_Absynpp_use_curr_namespace){
Cyc_Absynpp_curr_namespace_drop();}goto _LL360;_LL376: if(Cyc_Absynpp_print_using_stmts){
s=({struct Cyc_PP_Doc*_tmp224[7];_tmp224[6]=Cyc_PP_text(_tag_arr("}",sizeof(
unsigned char),2));_tmp224[5]=Cyc_PP_line_doc();_tmp224[4]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),
_tmp204);_tmp224[3]=Cyc_PP_line_doc();_tmp224[2]=Cyc_PP_text(_tag_arr(" {",
sizeof(unsigned char),3));_tmp224[1]=Cyc_Absynpp_qvar2doc(_tmp205);_tmp224[0]=
Cyc_PP_text(_tag_arr("using ",sizeof(unsigned char),7));Cyc_PP_cat(_tag_arr(
_tmp224,sizeof(struct Cyc_PP_Doc*),7));});}else{s=({struct Cyc_PP_Doc*_tmp225[7];
_tmp225[6]=Cyc_PP_text(_tag_arr("/* } */",sizeof(unsigned char),8));_tmp225[5]=
Cyc_PP_line_doc();_tmp225[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),_tmp204);_tmp225[3]=
Cyc_PP_line_doc();_tmp225[2]=Cyc_PP_text(_tag_arr(" { */",sizeof(unsigned char),
6));_tmp225[1]=Cyc_Absynpp_qvar2doc(_tmp205);_tmp225[0]=Cyc_PP_text(_tag_arr("/* using ",
sizeof(unsigned char),10));Cyc_PP_cat(_tag_arr(_tmp225,sizeof(struct Cyc_PP_Doc*),
7));});}goto _LL360;_LL378: if(Cyc_Absynpp_print_externC_stmts){s=({struct Cyc_PP_Doc*
_tmp226[5];_tmp226[4]=Cyc_PP_text(_tag_arr("}",sizeof(unsigned char),2));_tmp226[
3]=Cyc_PP_line_doc();_tmp226[2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),_tmp206);_tmp226[1]=
Cyc_PP_line_doc();_tmp226[0]=Cyc_PP_text(_tag_arr("extern \"C\" {",sizeof(
unsigned char),13));Cyc_PP_cat(_tag_arr(_tmp226,sizeof(struct Cyc_PP_Doc*),5));});}
else{s=({struct Cyc_PP_Doc*_tmp227[5];_tmp227[4]=Cyc_PP_text(_tag_arr("/* } */",
sizeof(unsigned char),8));_tmp227[3]=Cyc_PP_line_doc();_tmp227[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tag_arr("",sizeof(unsigned char),1),
_tmp206);_tmp227[1]=Cyc_PP_line_doc();_tmp227[0]=Cyc_PP_text(_tag_arr("/* extern \"C\" { */",
sizeof(unsigned char),19));Cyc_PP_cat(_tag_arr(_tmp227,sizeof(struct Cyc_PP_Doc*),
5));});}goto _LL360;_LL360:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*
sc){void*_tmp228=sc;_LL391: if((int)_tmp228 == 0){goto _LL392;}else{goto _LL393;}
_LL393: if((int)_tmp228 == 2){goto _LL394;}else{goto _LL395;}_LL395: if((int)_tmp228
== 3){goto _LL396;}else{goto _LL397;}_LL397: if((int)_tmp228 == 4){goto _LL398;}else{
goto _LL399;}_LL399: if((int)_tmp228 == 1){goto _LL39A;}else{goto _LL390;}_LL392:
return Cyc_PP_text(_tag_arr("static ",sizeof(unsigned char),8));_LL394: return Cyc_PP_nil_doc();
_LL396: return Cyc_PP_text(_tag_arr("extern ",sizeof(unsigned char),8));_LL398:
return Cyc_PP_text(_tag_arr("extern \"C\" ",sizeof(unsigned char),12));_LL39A:
return Cyc_PP_text(_tag_arr("abstract ",sizeof(unsigned char),10));_LL390:;}int
Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp229=t;struct Cyc_Absyn_Tvar*
_tmp22A;struct Cyc_List_List*_tmp22B;_LL39C: if((unsigned int)_tmp229 > 3?*((int*)
_tmp229)== 1: 0){_LL3A2: _tmp22A=((struct Cyc_Absyn_VarType_struct*)_tmp229)->f1;
goto _LL39D;}else{goto _LL39E;}_LL39E: if((unsigned int)_tmp229 > 3?*((int*)_tmp229)
== 20: 0){_LL3A3: _tmp22B=((struct Cyc_Absyn_JoinEff_struct*)_tmp229)->f1;goto
_LL39F;}else{goto _LL3A0;}_LL3A0: goto _LL3A1;_LL39D: return Cyc_Tcutil_is_temp_tvar(
_tmp22A);_LL39F: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp22B);_LL3A1: return 0;_LL39B:;}int Cyc_Absynpp_is_anon_structtype(void*t){void*
_tmp22C=t;struct _tuple0*_tmp22D;struct Cyc_Core_Opt*_tmp22E;struct Cyc_Core_Opt*
_tmp22F;_LL3A5: if((unsigned int)_tmp22C > 3?*((int*)_tmp22C)== 10: 0){_LL3AF:
_tmp22D=((struct Cyc_Absyn_StructType_struct*)_tmp22C)->f1;if(_tmp22D == 0){goto
_LL3A6;}else{goto _LL3A7;}}else{goto _LL3A7;}_LL3A7: if((unsigned int)_tmp22C > 3?*((
int*)_tmp22C)== 14: 0){goto _LL3A8;}else{goto _LL3A9;}_LL3A9: if((unsigned int)
_tmp22C > 3?*((int*)_tmp22C)== 18: 0){_LL3B0: _tmp22E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp22C)->f3;if(_tmp22E == 0){goto _LL3AA;}else{goto _LL3AB;}}else{goto _LL3AB;}
_LL3AB: if((unsigned int)_tmp22C > 3?*((int*)_tmp22C)== 18: 0){_LL3B1: _tmp22F=((
struct Cyc_Absyn_TypedefType_struct*)_tmp22C)->f3;goto _LL3AC;}else{goto _LL3AD;}
_LL3AD: goto _LL3AE;_LL3A6: return 1;_LL3A8: return 1;_LL3AA: return 0;_LL3AC: return Cyc_Absynpp_is_anon_structtype((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp22F))->v);_LL3AE: return 0;_LL3A4:;}
struct _tuple4 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t){void*_tmp230=t;
struct Cyc_Absyn_Exp*_tmp231;struct Cyc_Absyn_Tqual _tmp232;void*_tmp233;struct Cyc_Absyn_PtrInfo
_tmp234;struct Cyc_Absyn_Conref*_tmp235;struct Cyc_Absyn_Tqual _tmp236;struct Cyc_Absyn_Conref*
_tmp237;void*_tmp238;void*_tmp239;struct Cyc_Absyn_FnInfo _tmp23A;struct Cyc_List_List*
_tmp23B;struct Cyc_List_List*_tmp23C;struct Cyc_Absyn_VarargInfo*_tmp23D;int
_tmp23E;struct Cyc_List_List*_tmp23F;void*_tmp240;struct Cyc_Core_Opt*_tmp241;
struct Cyc_List_List*_tmp242;int _tmp243;struct Cyc_Core_Opt*_tmp244;struct Cyc_Core_Opt*
_tmp245;struct Cyc_Core_Opt*_tmp246;struct Cyc_List_List*_tmp247;struct _tuple0*
_tmp248;_LL3B3: if((unsigned int)_tmp230 > 3?*((int*)_tmp230)== 7: 0){_LL3C1:
_tmp233=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp230)->f1;goto _LL3C0;
_LL3C0: _tmp232=((struct Cyc_Absyn_ArrayType_struct*)_tmp230)->f2;goto _LL3BF;
_LL3BF: _tmp231=((struct Cyc_Absyn_ArrayType_struct*)_tmp230)->f3;goto _LL3B4;}
else{goto _LL3B5;}_LL3B5: if((unsigned int)_tmp230 > 3?*((int*)_tmp230)== 4: 0){
_LL3C2: _tmp234=((struct Cyc_Absyn_PointerType_struct*)_tmp230)->f1;_LL3C7: _tmp239=(
void*)_tmp234.elt_typ;goto _LL3C6;_LL3C6: _tmp238=(void*)_tmp234.rgn_typ;goto
_LL3C5;_LL3C5: _tmp237=_tmp234.nullable;goto _LL3C4;_LL3C4: _tmp236=_tmp234.tq;goto
_LL3C3;_LL3C3: _tmp235=_tmp234.bounds;goto _LL3B6;}else{goto _LL3B7;}_LL3B7: if((
unsigned int)_tmp230 > 3?*((int*)_tmp230)== 8: 0){_LL3C8: _tmp23A=((struct Cyc_Absyn_FnType_struct*)
_tmp230)->f1;_LL3D0: _tmp242=_tmp23A.tvars;goto _LL3CF;_LL3CF: _tmp241=_tmp23A.effect;
goto _LL3CE;_LL3CE: _tmp240=(void*)_tmp23A.ret_typ;goto _LL3CD;_LL3CD: _tmp23F=
_tmp23A.args;goto _LL3CC;_LL3CC: _tmp23E=_tmp23A.c_varargs;goto _LL3CB;_LL3CB:
_tmp23D=_tmp23A.cyc_varargs;goto _LL3CA;_LL3CA: _tmp23C=_tmp23A.rgn_po;goto _LL3C9;
_LL3C9: _tmp23B=_tmp23A.attributes;goto _LL3B8;}else{goto _LL3B9;}_LL3B9: if((
unsigned int)_tmp230 > 3?*((int*)_tmp230)== 0: 0){_LL3D3: _tmp245=((struct Cyc_Absyn_Evar_struct*)
_tmp230)->f1;goto _LL3D2;_LL3D2: _tmp244=((struct Cyc_Absyn_Evar_struct*)_tmp230)->f2;
goto _LL3D1;_LL3D1: _tmp243=((struct Cyc_Absyn_Evar_struct*)_tmp230)->f3;goto _LL3BA;}
else{goto _LL3BB;}_LL3BB: if((unsigned int)_tmp230 > 3?*((int*)_tmp230)== 18: 0){
_LL3D6: _tmp248=((struct Cyc_Absyn_TypedefType_struct*)_tmp230)->f1;goto _LL3D5;
_LL3D5: _tmp247=((struct Cyc_Absyn_TypedefType_struct*)_tmp230)->f2;goto _LL3D4;
_LL3D4: _tmp246=((struct Cyc_Absyn_TypedefType_struct*)_tmp230)->f3;goto _LL3BC;}
else{goto _LL3BD;}_LL3BD: goto _LL3BE;_LL3B4: {struct Cyc_List_List*_tmp24A;void*
_tmp24B;struct Cyc_Absyn_Tqual _tmp24C;struct _tuple4 _tmp249=Cyc_Absynpp_to_tms(
_tmp232,_tmp233);_LL3DA: _tmp24C=_tmp249.f1;goto _LL3D9;_LL3D9: _tmp24B=_tmp249.f2;
goto _LL3D8;_LL3D8: _tmp24A=_tmp249.f3;goto _LL3D7;_LL3D7: {void*tm;if(_tmp231 == 0){
tm=(void*)0;}else{tm=(void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp24D=
_cycalloc(sizeof(*_tmp24D));_tmp24D[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp24E;_tmp24E.tag=0;_tmp24E.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmp231);
_tmp24E;});_tmp24D;});}return({struct _tuple4 _tmp24F;_tmp24F.f1=_tmp24C;_tmp24F.f2=
_tmp24B;_tmp24F.f3=({struct Cyc_List_List*_tmp250=_cycalloc(sizeof(*_tmp250));
_tmp250->hd=(void*)tm;_tmp250->tl=_tmp24A;_tmp250;});_tmp24F;});}}_LL3B6: {
struct Cyc_List_List*_tmp252;void*_tmp253;struct Cyc_Absyn_Tqual _tmp254;struct
_tuple4 _tmp251=Cyc_Absynpp_to_tms(_tmp236,_tmp239);_LL3DE: _tmp254=_tmp251.f1;
goto _LL3DD;_LL3DD: _tmp253=_tmp251.f2;goto _LL3DC;_LL3DC: _tmp252=_tmp251.f3;goto
_LL3DB;_LL3DB: {void*ps;{struct _tuple8 _tmp256=({struct _tuple8 _tmp255;_tmp255.f1=(
void*)(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp237))->v;_tmp255.f2=(void*)(Cyc_Absyn_compress_conref(_tmp235))->v;_tmp255;});
void*_tmp257;void*_tmp258;struct Cyc_Absyn_Exp*_tmp259;void*_tmp25A;int _tmp25B;
void*_tmp25C;void*_tmp25D;struct Cyc_Absyn_Exp*_tmp25E;void*_tmp25F;int _tmp260;
void*_tmp261;void*_tmp262;_LL3E0: _LL3EB: _tmp25A=_tmp256.f1;if((unsigned int)
_tmp25A > 1?*((int*)_tmp25A)== 0: 0){_LL3EC: _tmp25B=(int)((struct Cyc_Absyn_Eq_constr_struct*)
_tmp25A)->f1;if(_tmp25B == 1){goto _LL3E8;}else{goto _LL3E2;}}else{goto _LL3E2;}
_LL3E8: _tmp257=_tmp256.f2;if((unsigned int)_tmp257 > 1?*((int*)_tmp257)== 0: 0){
_LL3E9: _tmp258=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp257)->f1;if((
unsigned int)_tmp258 > 1?*((int*)_tmp258)== 0: 0){_LL3EA: _tmp259=((struct Cyc_Absyn_Upper_b_struct*)
_tmp258)->f1;goto _LL3E1;}else{goto _LL3E2;}}else{goto _LL3E2;}_LL3E2: _LL3F0:
_tmp25F=_tmp256.f1;if((unsigned int)_tmp25F > 1?*((int*)_tmp25F)== 0: 0){_LL3F1:
_tmp260=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp25F)->f1;if(_tmp260 == 0){
goto _LL3ED;}else{goto _LL3E4;}}else{goto _LL3E4;}_LL3ED: _tmp25C=_tmp256.f2;if((
unsigned int)_tmp25C > 1?*((int*)_tmp25C)== 0: 0){_LL3EE: _tmp25D=(void*)((struct
Cyc_Absyn_Eq_constr_struct*)_tmp25C)->f1;if((unsigned int)_tmp25D > 1?*((int*)
_tmp25D)== 0: 0){_LL3EF: _tmp25E=((struct Cyc_Absyn_Upper_b_struct*)_tmp25D)->f1;
goto _LL3E3;}else{goto _LL3E4;}}else{goto _LL3E4;}_LL3E4: _LL3F2: _tmp261=_tmp256.f2;
if((unsigned int)_tmp261 > 1?*((int*)_tmp261)== 0: 0){_LL3F3: _tmp262=(void*)((
struct Cyc_Absyn_Eq_constr_struct*)_tmp261)->f1;if((int)_tmp262 == 0){goto _LL3E5;}
else{goto _LL3E6;}}else{goto _LL3E6;}_LL3E6: goto _LL3E7;_LL3E1: ps=(void*)({struct
Cyc_Absyn_Nullable_ps_struct*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263[0]=({
struct Cyc_Absyn_Nullable_ps_struct _tmp264;_tmp264.tag=1;_tmp264.f1=_tmp259;
_tmp264;});_tmp263;});goto _LL3DF;_LL3E3: ps=(void*)({struct Cyc_Absyn_NonNullable_ps_struct*
_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp266;_tmp266.tag=0;_tmp266.f1=_tmp25E;_tmp266;});_tmp265;});goto _LL3DF;_LL3E5:
ps=(void*)0;goto _LL3DF;_LL3E7: ps=(void*)0;goto _LL3DF;_LL3DF:;}{void*tm=(void*)({
struct Cyc_Absyn_Pointer_mod_struct*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269[0]=({
struct Cyc_Absyn_Pointer_mod_struct _tmp26A;_tmp26A.tag=1;_tmp26A.f1=(void*)ps;
_tmp26A.f2=(void*)_tmp238;_tmp26A.f3=tq;_tmp26A;});_tmp269;});return({struct
_tuple4 _tmp267;_tmp267.f1=_tmp254;_tmp267.f2=_tmp253;_tmp267.f3=({struct Cyc_List_List*
_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268->hd=(void*)tm;_tmp268->tl=_tmp252;
_tmp268;});_tmp267;});}}}_LL3B8: if(! Cyc_Absynpp_print_all_tvars){if(_tmp241 == 0?
1: Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp241->v)){_tmp241=0;_tmp242=0;}}
else{if(Cyc_Absynpp_rewrite_temp_tvars){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp242);}}{
struct Cyc_List_List*_tmp26C;void*_tmp26D;struct Cyc_Absyn_Tqual _tmp26E;struct
_tuple4 _tmp26B=Cyc_Absynpp_to_tms(Cyc_Absyn_empty_tqual(),_tmp240);_LL3F7:
_tmp26E=_tmp26B.f1;goto _LL3F6;_LL3F6: _tmp26D=_tmp26B.f2;goto _LL3F5;_LL3F5:
_tmp26C=_tmp26B.f3;goto _LL3F4;_LL3F4: {struct Cyc_List_List*tms=_tmp26C;if(
_tmp23B != 0? ! Cyc_Absynpp_to_VC: 0){tms=({struct Cyc_List_List*_tmp26F=_cycalloc(
sizeof(*_tmp26F));_tmp26F->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp271;_tmp271.tag=4;_tmp271.f1=0;_tmp271.f2=_tmp23B;_tmp271;});_tmp270;}));
_tmp26F->tl=tms;_tmp26F;});}tms=({struct Cyc_List_List*_tmp272=_cycalloc(sizeof(*
_tmp272));_tmp272->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp274;_tmp274.tag=2;_tmp274.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp276;_tmp276.tag=1;_tmp276.f1=_tmp23F;_tmp276.f2=_tmp23E;_tmp276.f3=_tmp23D;
_tmp276.f4=_tmp241;_tmp276.f5=_tmp23C;_tmp276;});_tmp275;}));_tmp274;});_tmp273;}));
_tmp272->tl=tms;_tmp272;});if(Cyc_Absynpp_to_VC){for(0;_tmp23B != 0;_tmp23B=
_tmp23B->tl){void*_tmp277=(void*)_tmp23B->hd;_LL3F9: if((int)_tmp277 == 0){goto
_LL3FA;}else{goto _LL3FB;}_LL3FB: if((int)_tmp277 == 1){goto _LL3FC;}else{goto _LL3FD;}
_LL3FD: if((int)_tmp277 == 2){goto _LL3FE;}else{goto _LL3FF;}_LL3FF: goto _LL400;
_LL3FA: goto _LL3FC;_LL3FC: goto _LL3FE;_LL3FE: tms=({struct Cyc_List_List*_tmp278=
_cycalloc(sizeof(*_tmp278));_tmp278->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp27A;_tmp27A.tag=4;_tmp27A.f1=0;_tmp27A.f2=({struct Cyc_List_List*_tmp27B=
_cycalloc(sizeof(*_tmp27B));_tmp27B->hd=(void*)((void*)_tmp23B->hd);_tmp27B->tl=
0;_tmp27B;});_tmp27A;});_tmp279;}));_tmp278->tl=tms;_tmp278;});goto AfterAtts;
_LL400: goto _LL3F8;_LL3F8:;}}AfterAtts: if(_tmp242 != 0){tms=({struct Cyc_List_List*
_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp27D=_cycalloc(sizeof(*_tmp27D));_tmp27D[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp27E;_tmp27E.tag=3;_tmp27E.f1=_tmp242;_tmp27E.f2=0;_tmp27E.f3=1;_tmp27E;});
_tmp27D;}));_tmp27C->tl=tms;_tmp27C;});}return({struct _tuple4 _tmp27F;_tmp27F.f1=
_tmp26E;_tmp27F.f2=_tmp26D;_tmp27F.f3=tms;_tmp27F;});}}_LL3BA: if(_tmp244 == 0){
return({struct _tuple4 _tmp280;_tmp280.f1=tq;_tmp280.f2=t;_tmp280.f3=0;_tmp280;});}
else{return Cyc_Absynpp_to_tms(tq,(void*)_tmp244->v);}_LL3BC: if((_tmp246 == 0? 1: !
Cyc_Absynpp_expand_typedefs)? 1: Cyc_Absynpp_is_anon_structtype((void*)_tmp246->v)){
return({struct _tuple4 _tmp281;_tmp281.f1=tq;_tmp281.f2=t;_tmp281.f3=0;_tmp281;});}
else{return Cyc_Absynpp_to_tms(tq,(void*)_tmp246->v);}_LL3BE: return({struct
_tuple4 _tmp282;_tmp282.f1=tq;_tmp282.f2=t;_tmp282.f3=0;_tmp282;});_LL3B2:;}
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*
dopt){struct Cyc_List_List*_tmp284;void*_tmp285;struct Cyc_Absyn_Tqual _tmp286;
struct _tuple4 _tmp283=Cyc_Absynpp_to_tms(tq,t);_LL404: _tmp286=_tmp283.f1;goto
_LL403;_LL403: _tmp285=_tmp283.f2;goto _LL402;_LL402: _tmp284=_tmp283.f3;goto _LL401;
_LL401: _tmp284=Cyc_List_imp_rev(_tmp284);if(_tmp284 == 0? dopt == 0: 0){return({
struct Cyc_PP_Doc*_tmp287[2];_tmp287[1]=Cyc_Absynpp_ntyp2doc(_tmp285);_tmp287[0]=
Cyc_Absynpp_tqual2doc(_tmp286);Cyc_PP_cat(_tag_arr(_tmp287,sizeof(struct Cyc_PP_Doc*),
2));});}else{return({struct Cyc_PP_Doc*_tmp288[4];_tmp288[3]=Cyc_Absynpp_dtms2doc(
dopt == 0? Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp284);_tmp288[2]=Cyc_PP_text(
_tag_arr(" ",sizeof(unsigned char),2));_tmp288[1]=Cyc_Absynpp_ntyp2doc(_tmp285);
_tmp288[0]=Cyc_Absynpp_tqual2doc(_tmp286);Cyc_PP_cat(_tag_arr(_tmp288,sizeof(
struct Cyc_PP_Doc*),4));});}}struct Cyc_PP_Doc*Cyc_Absynpp_structfield2doc(struct
Cyc_Absyn_Structfield*f){if(f->width != 0){return({struct Cyc_PP_Doc*_tmp289[5];
_tmp289[4]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmp289[3]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp289[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(
f->width));_tmp289[1]=Cyc_PP_text(_tag_arr(":",sizeof(unsigned char),2));_tmp289[
0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*_tmp28A=
_cycalloc(sizeof(*_tmp28A));_tmp28A->v=Cyc_PP_textptr(f->name);_tmp28A;}));Cyc_PP_cat(
_tag_arr(_tmp289,sizeof(struct Cyc_PP_Doc*),5));});}else{return({struct Cyc_PP_Doc*
_tmp28B[3];_tmp28B[2]=Cyc_PP_text(_tag_arr(";",sizeof(unsigned char),2));_tmp28B[
1]=Cyc_Absynpp_atts2doc(f->attributes);_tmp28B[0]=Cyc_Absynpp_tqtd2doc(f->tq,(
void*)f->type,({struct Cyc_Core_Opt*_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->v=
Cyc_PP_textptr(f->name);_tmp28C;}));Cyc_PP_cat(_tag_arr(_tmp28B,sizeof(struct Cyc_PP_Doc*),
3));});}}struct Cyc_PP_Doc*Cyc_Absynpp_structfields2doc(struct Cyc_List_List*
fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Structfield*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_structfield2doc,
_tag_arr("",sizeof(unsigned char),1),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(
struct Cyc_Absyn_Tunionfield*f){return({struct Cyc_PP_Doc*_tmp28D[4];_tmp28D[3]=f->typs
== 0? Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs);_tmp28D[2]=f->tvs == 0? Cyc_PP_nil_doc():
Cyc_Absynpp_ktvars2doc(f->tvs);_tmp28D[1]=Cyc_Absynpp_typedef_name2doc(f->name);
_tmp28D[0]=Cyc_Absynpp_scope2doc((void*)f->sc);Cyc_PP_cat(_tag_arr(_tmp28D,
sizeof(struct Cyc_PP_Doc*),4));});}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Tunionfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_tunionfield2doc,_tag_arr(",",sizeof(unsigned char),2),fields);}void
Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc_Std___sFILE*f){for(0;
tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)
tdl->hd),72,f);({void*_tmp28E[0]={};Cyc_Std_fprintf(f,_tag_arr("\n",sizeof(
unsigned char),2),_tag_arr(_tmp28E,sizeof(void*),0));});}}struct _tagged_arr Cyc_Absynpp_decllist2string(
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
void*_tmp28F=sc;_LL406: if((int)_tmp28F == 0){goto _LL407;}else{goto _LL408;}_LL408:
if((int)_tmp28F == 2){goto _LL409;}else{goto _LL40A;}_LL40A: if((int)_tmp28F == 3){
goto _LL40B;}else{goto _LL40C;}_LL40C: if((int)_tmp28F == 4){goto _LL40D;}else{goto
_LL40E;}_LL40E: if((int)_tmp28F == 1){goto _LL40F;}else{goto _LL405;}_LL407: return
_tag_arr("static",sizeof(unsigned char),7);_LL409: return _tag_arr("public",
sizeof(unsigned char),7);_LL40B: return _tag_arr("extern",sizeof(unsigned char),7);
_LL40D: return _tag_arr("extern \"C\"",sizeof(unsigned char),11);_LL40F: return
_tag_arr("abstract",sizeof(unsigned char),9);_LL405:;}
