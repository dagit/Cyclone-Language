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
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern void   _reset_region(struct _RegionHandle *);

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
extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};int Cyc_Std_fprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr);struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];int Cyc_List_exists(
int(*pred)(void*),struct Cyc_List_List*x);int Cyc_List_list_cmp(int(*cmp)(void*,
void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);int Cyc_List_list_prefix(
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
line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple0{void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct
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
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct
Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{
int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct
Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;
struct Cyc_List_List*f5;};struct Cyc_Absyn_NonNullable_ps_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Nullable_ps_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_Regparm_att_struct{int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{
int tag;int f1;};struct Cyc_Absyn_Section_att_struct{int tag;struct _tagged_arr f1;};
struct Cyc_Absyn_Format_att_struct{int tag;void*f1;int f2;int f3;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pointer_mod_struct{int tag;void*
f1;void*f2;struct Cyc_Absyn_Tqual f3;};struct Cyc_Absyn_Function_mod_struct{int tag;
void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int
tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{
int tag;void*f1;char f2;};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};
struct Cyc_Absyn_Int_c_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{
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
f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_ForArray_s_struct{
int tag;struct Cyc_Absyn_ForArrayInfo f1;};struct Cyc_Absyn_ResetRegion_s_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;
int f2;};struct Cyc_Absyn_Char_p_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_struct{
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
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;struct
Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[
15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct _tagged_arr Cyc_Absyn_attribute2string(
void*);struct _tuple3{void*f1;struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_Buffer_t;int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct
_tagged_arr*s2);struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,struct
_tagged_arr);unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
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
Cyc_Absyn_Exp*);struct _tagged_arr Cyc_Absynpp_char_escape(char);struct _tagged_arr
Cyc_Absynpp_string_escape(struct _tagged_arr);struct _tagged_arr Cyc_Absynpp_prim2str(
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
static char _tmp0[4]="Cyc";struct _tagged_arr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,
_tmp0 + 4};struct _tagged_arr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
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
struct _tagged_arr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0: return
_tag_arr("\\a",sizeof(char),3);case '\b': _LL1: return _tag_arr("\\b",sizeof(char),3);
case '\f': _LL2: return _tag_arr("\\f",sizeof(char),3);case '\n': _LL3: return _tag_arr("\\n",
sizeof(char),3);case '\r': _LL4: return _tag_arr("\\r",sizeof(char),3);case '\t': _LL5:
return _tag_arr("\\t",sizeof(char),3);case '\v': _LL6: return _tag_arr("\\v",sizeof(
char),3);case '\\': _LL7: return _tag_arr("\\\\",sizeof(char),3);case '"': _LL8: return
_tag_arr("\"",sizeof(char),2);case '\'': _LL9: return _tag_arr("\\'",sizeof(char),3);
default: _LLA: if(c >= ' '?c <= '~': 0){struct _tagged_arr _tmp2=Cyc_Core_new_string(1);*((
char*)_check_unknown_subscript(_tmp2,sizeof(char),0))=c;return(struct _tagged_arr)
_tmp2;}else{struct _tagged_arr _tmp3=Cyc_Core_new_string(4);int j=0;*((char*)
_check_unknown_subscript(_tmp3,sizeof(char),j ++))='\\';*((char*)
_check_unknown_subscript(_tmp3,sizeof(char),j ++))=(char)('0' + ((char)c >> 6 & 7));*((
char*)_check_unknown_subscript(_tmp3,sizeof(char),j ++))=(char)('0' + (c >> 3 & 7));*((
char*)_check_unknown_subscript(_tmp3,sizeof(char),j ++))=(char)('0' + (c & 7));
return(struct _tagged_arr)_tmp3;}}}static int Cyc_Absynpp_special(struct _tagged_arr
s){int sz=(int)(_get_arr_size(s,sizeof(char))- 1);{int i=0;for(0;i < sz;i ++){char c=((
const char*)s.curr)[i];if(((c <= ' '?1: c >= '~')?1: c == '"')?1: c == '\\')return 1;}}
return 0;}struct _tagged_arr Cyc_Absynpp_string_escape(struct _tagged_arr s){if(!Cyc_Absynpp_special(
s))return s;{int n=(int)(_get_arr_size(s,sizeof(char))- 1);if(n > 0?((const char*)s.curr)[
n]== '\000': 0)n --;{int len=0;{int i=0;for(0;i <= n;i ++){char _tmp4=((const char*)s.curr)[
i];_LLD: if(_tmp4 != '\a')goto _LLF;_LLE: goto _LL10;_LLF: if(_tmp4 != '\b')goto _LL11;
_LL10: goto _LL12;_LL11: if(_tmp4 != '\f')goto _LL13;_LL12: goto _LL14;_LL13: if(_tmp4 != '\n')
goto _LL15;_LL14: goto _LL16;_LL15: if(_tmp4 != '\r')goto _LL17;_LL16: goto _LL18;_LL17:
if(_tmp4 != '\t')goto _LL19;_LL18: goto _LL1A;_LL19: if(_tmp4 != '\v')goto _LL1B;_LL1A:
goto _LL1C;_LL1B: if(_tmp4 != '\\')goto _LL1D;_LL1C: goto _LL1E;_LL1D: if(_tmp4 != '"')
goto _LL1F;_LL1E: len +=2;goto _LLC;_LL1F:;_LL20: if(_tmp4 >= ' '?_tmp4 <= '~': 0)len ++;
else{len +=4;}goto _LLC;_LLC:;}}{struct _tagged_arr t=Cyc_Core_new_string((
unsigned int)len);int j=0;{int i=0;for(0;i <= n;i ++){char _tmp5=((const char*)s.curr)[
i];_LL22: if(_tmp5 != '\a')goto _LL24;_LL23:*((char*)_check_unknown_subscript(t,
sizeof(char),j ++))='\\';*((char*)_check_unknown_subscript(t,sizeof(char),j ++))='a';
goto _LL21;_LL24: if(_tmp5 != '\b')goto _LL26;_LL25:*((char*)
_check_unknown_subscript(t,sizeof(char),j ++))='\\';*((char*)
_check_unknown_subscript(t,sizeof(char),j ++))='b';goto _LL21;_LL26: if(_tmp5 != '\f')
goto _LL28;_LL27:*((char*)_check_unknown_subscript(t,sizeof(char),j ++))='\\';*((
char*)_check_unknown_subscript(t,sizeof(char),j ++))='f';goto _LL21;_LL28: if(_tmp5
!= '\n')goto _LL2A;_LL29:*((char*)_check_unknown_subscript(t,sizeof(char),j ++))='\\';*((
char*)_check_unknown_subscript(t,sizeof(char),j ++))='n';goto _LL21;_LL2A: if(_tmp5
!= '\r')goto _LL2C;_LL2B:*((char*)_check_unknown_subscript(t,sizeof(char),j ++))='\\';*((
char*)_check_unknown_subscript(t,sizeof(char),j ++))='r';goto _LL21;_LL2C: if(_tmp5
!= '\t')goto _LL2E;_LL2D:*((char*)_check_unknown_subscript(t,sizeof(char),j ++))='\\';*((
char*)_check_unknown_subscript(t,sizeof(char),j ++))='t';goto _LL21;_LL2E: if(_tmp5
!= '\v')goto _LL30;_LL2F:*((char*)_check_unknown_subscript(t,sizeof(char),j ++))='\\';*((
char*)_check_unknown_subscript(t,sizeof(char),j ++))='v';goto _LL21;_LL30: if(_tmp5
!= '\\')goto _LL32;_LL31:*((char*)_check_unknown_subscript(t,sizeof(char),j ++))='\\';*((
char*)_check_unknown_subscript(t,sizeof(char),j ++))='\\';goto _LL21;_LL32: if(
_tmp5 != '"')goto _LL34;_LL33:*((char*)_check_unknown_subscript(t,sizeof(char),j ++))='\\';*((
char*)_check_unknown_subscript(t,sizeof(char),j ++))='"';goto _LL21;_LL34:;_LL35:
if(_tmp5 >= ' '?_tmp5 <= '~': 0)*((char*)_check_unknown_subscript(t,sizeof(char),j
++))=_tmp5;else{*((char*)_check_unknown_subscript(t,sizeof(char),j ++))='\\';*((
char*)_check_unknown_subscript(t,sizeof(char),j ++))=(char)('0' + (_tmp5 >> 6 & 7));*((
char*)_check_unknown_subscript(t,sizeof(char),j ++))=(char)('0' + (_tmp5 >> 3 & 7));*((
char*)_check_unknown_subscript(t,sizeof(char),j ++))=(char)('0' + (_tmp5 & 7));}
goto _LL21;_LL21:;}}return(struct _tagged_arr)t;}}}}static char _tmp6[9]="restrict";
static struct _tagged_arr Cyc_Absynpp_restrict_string={_tmp6,_tmp6,_tmp6 + 9};static
char _tmp7[9]="volatile";static struct _tagged_arr Cyc_Absynpp_volatile_string={
_tmp7,_tmp7,_tmp7 + 9};static char _tmp8[6]="const";static struct _tagged_arr Cyc_Absynpp_const_str={
_tmp8,_tmp8,_tmp8 + 6};static struct _tagged_arr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _tagged_arr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _tagged_arr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;struct Cyc_PP_Doc*
Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict)
l=({struct Cyc_List_List*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->hd=Cyc_Absynpp_restrict_sp;
_tmp9->tl=l;_tmp9;});if(tq.q_volatile)l=({struct Cyc_List_List*_tmpA=_cycalloc(
sizeof(*_tmpA));_tmpA->hd=Cyc_Absynpp_volatile_sp;_tmpA->tl=l;_tmpA;});if(tq.q_const)
l=({struct Cyc_List_List*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->hd=Cyc_Absynpp_const_sp;
_tmpB->tl=l;_tmpB;});return Cyc_PP_egroup(_tag_arr("",sizeof(char),1),_tag_arr(" ",
sizeof(char),2),_tag_arr(" ",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));}
struct _tagged_arr Cyc_Absynpp_kind2string(void*k){void*_tmpC=k;_LL37: if((int)
_tmpC != 0)goto _LL39;_LL38: return _tag_arr("A",sizeof(char),2);_LL39: if((int)_tmpC
!= 1)goto _LL3B;_LL3A: return _tag_arr("M",sizeof(char),2);_LL3B: if((int)_tmpC != 2)
goto _LL3D;_LL3C: return _tag_arr("B",sizeof(char),2);_LL3D: if((int)_tmpC != 3)goto
_LL3F;_LL3E: return _tag_arr("R",sizeof(char),2);_LL3F: if((int)_tmpC != 4)goto _LL36;
_LL40: return _tag_arr("E",sizeof(char),2);_LL36:;}struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*c){void*_tmpD=Cyc_Absyn_compress_kb(c);void*_tmpE;void*_tmpF;_LL42: if(*((
int*)_tmpD)!= 0)goto _LL44;_tmpE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpD)->f1;
_LL43: return Cyc_Absynpp_kind2string(_tmpE);_LL44: if(*((int*)_tmpD)!= 1)goto _LL46;
_LL45: return _tag_arr("?",sizeof(char),2);_LL46: if(*((int*)_tmpD)!= 2)goto _LL41;
_tmpF=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpD)->f2;_LL47: return Cyc_Absynpp_kind2string(
_tmpF);_LL41:;}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(void*k){return Cyc_PP_text(
Cyc_Absynpp_kind2string(k));}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
return Cyc_PP_text(Cyc_Absynpp_kindbound2string(c));}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(
struct Cyc_List_List*ts){return Cyc_PP_egroup(_tag_arr("<",sizeof(char),2),
_tag_arr(">",sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*
_tmp10=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp11;void*_tmp12;void*
_tmp13;_LL49: if(*((int*)_tmp10)!= 1)goto _LL4B;_LL4A: goto _LL4C;_LL4B: if(*((int*)
_tmp10)!= 0)goto _LL4D;_tmp11=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp10)->f1;
if((int)_tmp11 != 2)goto _LL4D;_LL4C: return Cyc_PP_textptr(tv->name);_LL4D: if(*((
int*)_tmp10)!= 2)goto _LL4F;_tmp12=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp10)->f2;_LL4E: _tmp13=_tmp12;goto _LL50;_LL4F: if(*((int*)_tmp10)!= 0)goto _LL48;
_tmp13=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp10)->f1;_LL50: return({struct
Cyc_PP_Doc*_tmp14[3];_tmp14[2]=Cyc_Absynpp_kind2doc(_tmp13);_tmp14[1]=Cyc_PP_text(
_tag_arr("::",sizeof(char),3));_tmp14[0]=Cyc_PP_textptr(tv->name);Cyc_PP_cat(
_tag_arr(_tmp14,sizeof(struct Cyc_PP_Doc*),3));});_LL48:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
struct Cyc_List_List*tvs){return Cyc_PP_egroup(_tag_arr("<",sizeof(char),2),
_tag_arr(">",sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_ktvar2doc,tvs));}static struct _tagged_arr*Cyc_Absynpp_get_name(struct
Cyc_Absyn_Tvar*tv){return tv->name;}struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct
Cyc_List_List*tvs){if(Cyc_Absynpp_print_all_kinds)return Cyc_Absynpp_ktvars2doc(
tvs);return Cyc_PP_egroup(_tag_arr("<",sizeof(char),2),_tag_arr(">",sizeof(char),
2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tagged_arr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct
Cyc_List_List*(*)(struct _tagged_arr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));}struct _tuple9{struct Cyc_Absyn_Tqual
f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple9*t){return Cyc_Absynpp_tqtd2doc((*
t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return Cyc_PP_group(_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(char),2),
_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmp15=att;_LL52: if((
int)_tmp15 != 0)goto _LL54;_LL53: goto _LL55;_LL54: if((int)_tmp15 != 1)goto _LL56;
_LL55: goto _LL57;_LL56: if((int)_tmp15 != 2)goto _LL58;_LL57: return Cyc_PP_nil_doc();
_LL58:;_LL59: return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL51:;}struct
Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 0;
atts=atts->tl){void*_tmp16=(void*)atts->hd;_LL5B: if((int)_tmp16 != 0)goto _LL5D;
_LL5C: return Cyc_PP_text(_tag_arr(" _stdcall ",sizeof(char),11));_LL5D: if((int)
_tmp16 != 1)goto _LL5F;_LL5E: return Cyc_PP_text(_tag_arr(" _cdecl ",sizeof(char),9));
_LL5F: if((int)_tmp16 != 2)goto _LL61;_LL60: return Cyc_PP_text(_tag_arr(" _fastcall ",
sizeof(char),12));_LL61:;_LL62: goto _LL5A;_LL5A:;}return Cyc_PP_nil_doc();}struct
Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int hasatt=0;{
struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmp17=(
void*)atts2->hd;_LL64: if((int)_tmp17 != 0)goto _LL66;_LL65: goto _LL67;_LL66: if((int)
_tmp17 != 1)goto _LL68;_LL67: goto _LL69;_LL68: if((int)_tmp17 != 2)goto _LL6A;_LL69:
goto _LL63;_LL6A:;_LL6B: hasatt=1;goto _LL63;_LL63:;}}if(!hasatt)return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_tmp18[3];_tmp18[2]=Cyc_PP_text(_tag_arr(")",sizeof(
char),2));_tmp18[1]=Cyc_PP_group(_tag_arr("",sizeof(char),1),_tag_arr("",sizeof(
char),1),_tag_arr(" ",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts));
_tmp18[0]=Cyc_PP_text(_tag_arr("__declspec(",sizeof(char),12));Cyc_PP_cat(
_tag_arr(_tmp18,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(
struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();if(Cyc_Absynpp_to_VC)
return Cyc_Absynpp_noncallconv2doc(atts);return({struct Cyc_PP_Doc*_tmp19[2];
_tmp19[1]=Cyc_PP_group(_tag_arr("((",sizeof(char),3),_tag_arr("))",sizeof(char),
3),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts));_tmp19[0]=
Cyc_PP_text(_tag_arr(" __attribute__",sizeof(char),15));Cyc_PP_cat(_tag_arr(
_tmp19,sizeof(struct Cyc_PP_Doc*),2));});}int Cyc_Absynpp_next_is_pointer(struct
Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmp1A=(void*)tms->hd;_LL6D: if(
_tmp1A <= (void*)1?1:*((int*)_tmp1A)!= 1)goto _LL6F;_LL6E: return 1;_LL6F: if(_tmp1A
<= (void*)1?1:*((int*)_tmp1A)!= 4)goto _LL71;_LL70: if(!Cyc_Absynpp_to_VC)return 0;
return Cyc_Absynpp_next_is_pointer(tms->tl);_LL71:;_LL72: return 0;_LL6C:;}}struct
Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(
struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*
rest=Cyc_Absynpp_dtms2doc(d,tms->tl);struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*
_tmp3D[3];_tmp3D[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp3D[1]=rest;
_tmp3D[0]=Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp3D,
sizeof(struct Cyc_PP_Doc*),3));});void*_tmp1B=(void*)tms->hd;struct Cyc_Absyn_Exp*
_tmp1C;void*_tmp1D;struct Cyc_List_List*_tmp1E;struct Cyc_List_List*_tmp1F;struct
Cyc_Position_Segment*_tmp20;int _tmp21;void*_tmp22;void*_tmp23;struct Cyc_Absyn_Tqual
_tmp24;_LL74: if((int)_tmp1B != 0)goto _LL76;_LL75: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*_tmp25[2];_tmp25[1]=Cyc_PP_text(
_tag_arr("[]",sizeof(char),3));_tmp25[0]=rest;Cyc_PP_cat(_tag_arr(_tmp25,sizeof(
struct Cyc_PP_Doc*),2));});_LL76: if(_tmp1B <= (void*)1?1:*((int*)_tmp1B)!= 0)goto
_LL78;_tmp1C=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp1B)->f1;_LL77: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*_tmp26[4];_tmp26[3]=Cyc_PP_text(
_tag_arr("]",sizeof(char),2));_tmp26[2]=Cyc_Absynpp_exp2doc(_tmp1C);_tmp26[1]=
Cyc_PP_text(_tag_arr("[",sizeof(char),2));_tmp26[0]=rest;Cyc_PP_cat(_tag_arr(
_tmp26,sizeof(struct Cyc_PP_Doc*),4));});_LL78: if(_tmp1B <= (void*)1?1:*((int*)
_tmp1B)!= 2)goto _LL7A;_tmp1D=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp1B)->f1;_LL79: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{void*
_tmp27=_tmp1D;struct Cyc_List_List*_tmp28;int _tmp29;struct Cyc_Absyn_VarargInfo*
_tmp2A;struct Cyc_Core_Opt*_tmp2B;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*
_tmp2D;struct Cyc_Position_Segment*_tmp2E;_LL81: if(*((int*)_tmp27)!= 1)goto _LL83;
_tmp28=((struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f1;_tmp29=((struct Cyc_Absyn_WithTypes_struct*)
_tmp27)->f2;_tmp2A=((struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f3;_tmp2B=((
struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f4;_tmp2C=((struct Cyc_Absyn_WithTypes_struct*)
_tmp27)->f5;_LL82: return({struct Cyc_PP_Doc*_tmp2F[2];_tmp2F[1]=Cyc_Absynpp_funargs2doc(
_tmp28,_tmp29,_tmp2A,_tmp2B,_tmp2C);_tmp2F[0]=rest;Cyc_PP_cat(_tag_arr(_tmp2F,
sizeof(struct Cyc_PP_Doc*),2));});_LL83: if(*((int*)_tmp27)!= 0)goto _LL80;_tmp2D=((
struct Cyc_Absyn_NoTypes_struct*)_tmp27)->f1;_tmp2E=((struct Cyc_Absyn_NoTypes_struct*)
_tmp27)->f2;_LL84: return({struct Cyc_PP_Doc*_tmp30[2];_tmp30[1]=Cyc_PP_group(
_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(char),2),_tag_arr(",",sizeof(
char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp2D));_tmp30[0]=rest;Cyc_PP_cat(
_tag_arr(_tmp30,sizeof(struct Cyc_PP_Doc*),2));});_LL80:;}_LL7A: if(_tmp1B <= (void*)
1?1:*((int*)_tmp1B)!= 4)goto _LL7C;_tmp1E=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmp1B)->f2;_LL7B: if(!Cyc_Absynpp_to_VC){if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;return({struct Cyc_PP_Doc*_tmp31[2];_tmp31[1]=Cyc_Absynpp_atts2doc(
_tmp1E);_tmp31[0]=rest;Cyc_PP_cat(_tag_arr(_tmp31,sizeof(struct Cyc_PP_Doc*),2));});}
else{if(Cyc_Absynpp_next_is_pointer(tms->tl))return({struct Cyc_PP_Doc*_tmp32[2];
_tmp32[1]=rest;_tmp32[0]=Cyc_Absynpp_callconv2doc(_tmp1E);Cyc_PP_cat(_tag_arr(
_tmp32,sizeof(struct Cyc_PP_Doc*),2));});return rest;}_LL7C: if(_tmp1B <= (void*)1?1:*((
int*)_tmp1B)!= 3)goto _LL7E;_tmp1F=((struct Cyc_Absyn_TypeParams_mod_struct*)
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
_tmp36)== 1)ptr=Cyc_PP_text(_tag_arr("*",sizeof(char),2));else{ptr=({struct Cyc_PP_Doc*
_tmp38[4];_tmp38[3]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp38[2]=Cyc_Absynpp_exp2doc(
_tmp36);_tmp38[1]=Cyc_PP_text(_tag_arr("{",sizeof(char),2));_tmp38[0]=Cyc_PP_text(
_tag_arr("*",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp38,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL85;_LL88: if(_tmp35 <= (void*)1?1:*((int*)_tmp35)!= 0)goto _LL8A;
_tmp37=((struct Cyc_Absyn_NonNullable_ps_struct*)_tmp35)->f1;_LL89: if(Cyc_Evexp_eval_const_uint_exp(
_tmp37)== 1)ptr=Cyc_PP_text(_tag_arr("@",sizeof(char),2));else{ptr=({struct Cyc_PP_Doc*
_tmp39[4];_tmp39[3]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp39[2]=Cyc_Absynpp_exp2doc(
_tmp37);_tmp39[1]=Cyc_PP_text(_tag_arr("{",sizeof(char),2));_tmp39[0]=Cyc_PP_text(
_tag_arr("@",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp39,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL85;_LL8A: if((int)_tmp35 != 0)goto _LL85;_LL8B: ptr=Cyc_PP_text(
_tag_arr("?",sizeof(char),2));goto _LL85;_LL85:;}{void*_tmp3A=Cyc_Tcutil_compress(
_tmp23);_LL8D: if((int)_tmp3A != 2)goto _LL8F;_LL8E: return({struct Cyc_PP_Doc*_tmp3B[
2];_tmp3B[1]=rest;_tmp3B[0]=ptr;Cyc_PP_cat(_tag_arr(_tmp3B,sizeof(struct Cyc_PP_Doc*),
2));});_LL8F:;_LL90: return({struct Cyc_PP_Doc*_tmp3C[4];_tmp3C[3]=rest;_tmp3C[2]=
Cyc_PP_text(_tag_arr(" ",sizeof(char),2));_tmp3C[1]=Cyc_Absynpp_typ2doc(_tmp23);
_tmp3C[0]=ptr;Cyc_PP_cat(_tag_arr(_tmp3C,sizeof(struct Cyc_PP_Doc*),4));});_LL8C:;}}
_LL73:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp3E=Cyc_Tcutil_compress(
t);_LL92: if((int)_tmp3E != 2)goto _LL94;_LL93: return Cyc_PP_text(_tag_arr("`H",
sizeof(char),3));_LL94:;_LL95: return Cyc_Absynpp_ntyp2doc(t);_LL91:;}static void
Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
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
effects);if(rgions == 0?effects != 0: 0)return Cyc_PP_group(_tag_arr("",sizeof(char),
1),_tag_arr("",sizeof(char),1),_tag_arr("+",sizeof(char),2),effects);else{struct
Cyc_PP_Doc*_tmp44=Cyc_PP_group(_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(
char),2),_tag_arr(",",sizeof(char),2),rgions);return Cyc_PP_group(_tag_arr("",
sizeof(char),1),_tag_arr("",sizeof(char),1),_tag_arr("+",sizeof(char),2),((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
effects,({struct Cyc_List_List*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->hd=
_tmp44;_tmp45->tl=0;_tmp45;})));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(
void*k){void*_tmp46=k;_LL9E: if((int)_tmp46 != 0)goto _LLA0;_LL9F: return Cyc_PP_text(
_tag_arr("struct ",sizeof(char),8));_LLA0: if((int)_tmp46 != 1)goto _LL9D;_LLA1:
return Cyc_PP_text(_tag_arr("union ",sizeof(char),7));_LL9D:;}struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmp47=t;struct Cyc_Core_Opt*
_tmp48;struct Cyc_Core_Opt*_tmp49;int _tmp4A;struct Cyc_Core_Opt*_tmp4B;struct Cyc_Absyn_Tvar*
_tmp4C;struct Cyc_Absyn_TunionInfo _tmp4D;void*_tmp4E;struct Cyc_List_List*_tmp4F;
void*_tmp50;struct Cyc_Absyn_TunionFieldInfo _tmp51;void*_tmp52;struct Cyc_List_List*
_tmp53;void*_tmp54;void*_tmp55;int _tmp56;struct Cyc_List_List*_tmp57;struct Cyc_Absyn_AggrInfo
_tmp58;void*_tmp59;struct Cyc_List_List*_tmp5A;void*_tmp5B;struct Cyc_List_List*
_tmp5C;struct Cyc_List_List*_tmp5D;struct _tuple0*_tmp5E;void*_tmp5F;struct _tuple0*
_tmp60;struct Cyc_List_List*_tmp61;struct Cyc_Absyn_Typedefdecl*_tmp62;struct Cyc_Core_Opt*
_tmp63;void*_tmp64;void*_tmp65;_LLA3: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 7)
goto _LLA5;_LLA4: return Cyc_PP_text(_tag_arr("[[[array]]]",sizeof(char),12));_LLA5:
if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 8)goto _LLA7;_LLA6: return Cyc_PP_nil_doc();
_LLA7: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 4)goto _LLA9;_LLA8: return Cyc_PP_nil_doc();
_LLA9: if((int)_tmp47 != 0)goto _LLAB;_LLAA: s=Cyc_PP_text(_tag_arr("void",sizeof(
char),5));goto _LLA2;_LLAB: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 0)goto _LLAD;
_tmp48=((struct Cyc_Absyn_Evar_struct*)_tmp47)->f1;_tmp49=((struct Cyc_Absyn_Evar_struct*)
_tmp47)->f2;_tmp4A=((struct Cyc_Absyn_Evar_struct*)_tmp47)->f3;_tmp4B=((struct Cyc_Absyn_Evar_struct*)
_tmp47)->f4;_LLAC: if(_tmp49 != 0)return Cyc_Absynpp_ntyp2doc((void*)_tmp49->v);
else{s=({struct Cyc_PP_Doc*_tmp66[6];_tmp66[5]=_tmp48 == 0?Cyc_PP_text(_tag_arr("?",
sizeof(char),2)): Cyc_Absynpp_kind2doc((void*)_tmp48->v);_tmp66[4]=Cyc_PP_text(
_tag_arr(")::",sizeof(char),4));_tmp66[3]=(!Cyc_Absynpp_print_full_evars?1:
_tmp4B == 0)?Cyc_PP_text(_tag_arr("",sizeof(char),1)): Cyc_Absynpp_tvars2doc((
struct Cyc_List_List*)_tmp4B->v);_tmp66[2]=Cyc_PP_text((struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmp68;_tmp68.tag=1;_tmp68.f1=(int)((unsigned int)
_tmp4A);{void*_tmp67[1]={& _tmp68};Cyc_Std_aprintf(_tag_arr("%d",sizeof(char),3),
_tag_arr(_tmp67,sizeof(void*),1));}}));_tmp66[1]=Cyc_PP_text(_tag_arr("(",
sizeof(char),2));_tmp66[0]=Cyc_PP_text(_tag_arr("%",sizeof(char),2));Cyc_PP_cat(
_tag_arr(_tmp66,sizeof(struct Cyc_PP_Doc*),6));});}goto _LLA2;_LLAD: if(_tmp47 <= (
void*)3?1:*((int*)_tmp47)!= 1)goto _LLAF;_tmp4C=((struct Cyc_Absyn_VarType_struct*)
_tmp47)->f1;_LLAE: s=Cyc_PP_textptr(_tmp4C->name);if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp69[3];_tmp69[2]=Cyc_Absynpp_kindbound2doc((void*)_tmp4C->kind);
_tmp69[1]=Cyc_PP_text(_tag_arr("::",sizeof(char),3));_tmp69[0]=s;Cyc_PP_cat(
_tag_arr(_tmp69,sizeof(struct Cyc_PP_Doc*),3));});if(Cyc_Absynpp_rewrite_temp_tvars?
Cyc_Tcutil_is_temp_tvar(_tmp4C): 0)s=({struct Cyc_PP_Doc*_tmp6A[3];_tmp6A[2]=Cyc_PP_text(
_tag_arr(" */",sizeof(char),4));_tmp6A[1]=s;_tmp6A[0]=Cyc_PP_text(_tag_arr("_ /* ",
sizeof(char),6));Cyc_PP_cat(_tag_arr(_tmp6A,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLA2;_LLAF: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 2)goto _LLB1;_tmp4D=((
struct Cyc_Absyn_TunionType_struct*)_tmp47)->f1;_tmp4E=(void*)_tmp4D.tunion_info;
_tmp4F=_tmp4D.targs;_tmp50=(void*)_tmp4D.rgn;_LLB0:{void*_tmp6B=_tmp4E;struct Cyc_Absyn_UnknownTunionInfo
_tmp6C;struct _tuple0*_tmp6D;int _tmp6E;struct Cyc_Absyn_Tuniondecl**_tmp6F;struct
Cyc_Absyn_Tuniondecl*_tmp70;struct Cyc_Absyn_Tuniondecl _tmp71;struct _tuple0*
_tmp72;int _tmp73;_LLD2: if(*((int*)_tmp6B)!= 0)goto _LLD4;_tmp6C=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp6B)->f1;_tmp6D=_tmp6C.name;_tmp6E=_tmp6C.is_xtunion;_LLD3: _tmp72=_tmp6D;
_tmp73=_tmp6E;goto _LLD5;_LLD4: if(*((int*)_tmp6B)!= 1)goto _LLD1;_tmp6F=((struct
Cyc_Absyn_KnownTunion_struct*)_tmp6B)->f1;_tmp70=*_tmp6F;_tmp71=*_tmp70;_tmp72=
_tmp71.name;_tmp73=_tmp71.is_xtunion;_LLD5: {struct Cyc_PP_Doc*_tmp74=Cyc_PP_text(
_tmp73?_tag_arr("xtunion ",sizeof(char),9): _tag_arr("tunion ",sizeof(char),8));{
void*_tmp75=Cyc_Tcutil_compress(_tmp50);_LLD7: if((int)_tmp75 != 2)goto _LLD9;_LLD8:
s=({struct Cyc_PP_Doc*_tmp76[3];_tmp76[2]=Cyc_Absynpp_tps2doc(_tmp4F);_tmp76[1]=
Cyc_Absynpp_qvar2doc(_tmp72);_tmp76[0]=_tmp74;Cyc_PP_cat(_tag_arr(_tmp76,sizeof(
struct Cyc_PP_Doc*),3));});goto _LLD6;_LLD9:;_LLDA: s=({struct Cyc_PP_Doc*_tmp77[5];
_tmp77[4]=Cyc_Absynpp_tps2doc(_tmp4F);_tmp77[3]=Cyc_Absynpp_qvar2doc(_tmp72);
_tmp77[2]=Cyc_PP_text(_tag_arr(" ",sizeof(char),2));_tmp77[1]=Cyc_Absynpp_typ2doc(
_tmp50);_tmp77[0]=_tmp74;Cyc_PP_cat(_tag_arr(_tmp77,sizeof(struct Cyc_PP_Doc*),5));});
goto _LLD6;_LLD6:;}goto _LLD1;}_LLD1:;}goto _LLA2;_LLB1: if(_tmp47 <= (void*)3?1:*((
int*)_tmp47)!= 3)goto _LLB3;_tmp51=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp47)->f1;_tmp52=(void*)_tmp51.field_info;_tmp53=_tmp51.targs;_LLB2:{void*
_tmp78=_tmp52;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp79;struct _tuple0*_tmp7A;
struct _tuple0*_tmp7B;int _tmp7C;struct Cyc_Absyn_Tuniondecl*_tmp7D;struct Cyc_Absyn_Tuniondecl
_tmp7E;struct _tuple0*_tmp7F;int _tmp80;struct Cyc_Absyn_Tunionfield*_tmp81;struct
Cyc_Absyn_Tunionfield _tmp82;struct _tuple0*_tmp83;_LLDC: if(*((int*)_tmp78)!= 0)
goto _LLDE;_tmp79=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp78)->f1;_tmp7A=
_tmp79.tunion_name;_tmp7B=_tmp79.field_name;_tmp7C=_tmp79.is_xtunion;_LLDD:
_tmp7F=_tmp7A;_tmp80=_tmp7C;_tmp83=_tmp7B;goto _LLDF;_LLDE: if(*((int*)_tmp78)!= 1)
goto _LLDB;_tmp7D=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp78)->f1;_tmp7E=*
_tmp7D;_tmp7F=_tmp7E.name;_tmp80=_tmp7E.is_xtunion;_tmp81=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp78)->f2;_tmp82=*_tmp81;_tmp83=_tmp82.name;_LLDF: {struct Cyc_PP_Doc*_tmp84=
Cyc_PP_text(_tmp80?_tag_arr("xtunion ",sizeof(char),9): _tag_arr("tunion ",
sizeof(char),8));s=({struct Cyc_PP_Doc*_tmp85[4];_tmp85[3]=Cyc_Absynpp_qvar2doc(
_tmp83);_tmp85[2]=Cyc_PP_text(_tag_arr(".",sizeof(char),2));_tmp85[1]=Cyc_Absynpp_qvar2doc(
_tmp7F);_tmp85[0]=_tmp84;Cyc_PP_cat(_tag_arr(_tmp85,sizeof(struct Cyc_PP_Doc*),4));});
goto _LLDB;}_LLDB:;}goto _LLA2;_LLB3: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 5)
goto _LLB5;_tmp54=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47)->f1;_tmp55=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp47)->f2;_LLB4: {struct _tagged_arr sns;
struct _tagged_arr ts;{void*_tmp86=_tmp54;_LLE1: if((int)_tmp86 != 0)goto _LLE3;_LLE2:
sns=_tag_arr("",sizeof(char),1);goto _LLE0;_LLE3: if((int)_tmp86 != 1)goto _LLE0;
_LLE4: sns=_tag_arr("unsigned ",sizeof(char),10);goto _LLE0;_LLE0:;}{void*_tmp87=
_tmp55;_LLE6: if((int)_tmp87 != 0)goto _LLE8;_LLE7:{void*_tmp88=_tmp54;_LLEF: if((
int)_tmp88 != 0)goto _LLF1;_LLF0: sns=_tag_arr("signed ",sizeof(char),8);goto _LLEE;
_LLF1: if((int)_tmp88 != 1)goto _LLEE;_LLF2: sns=_tag_arr("",sizeof(char),1);goto
_LLEE;_LLEE:;}ts=_tag_arr("char",sizeof(char),5);goto _LLE5;_LLE8: if((int)_tmp87
!= 1)goto _LLEA;_LLE9: ts=_tag_arr("short",sizeof(char),6);goto _LLE5;_LLEA: if((int)
_tmp87 != 2)goto _LLEC;_LLEB: ts=_tag_arr("int",sizeof(char),4);goto _LLE5;_LLEC: if((
int)_tmp87 != 3)goto _LLE5;_LLED: ts=Cyc_Absynpp_to_VC?_tag_arr("__int64",sizeof(
char),8): _tag_arr("long long",sizeof(char),10);goto _LLE5;_LLE5:;}s=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp8B;_tmp8B.tag=0;_tmp8B.f1=(
struct _tagged_arr)ts;{struct Cyc_Std_String_pa_struct _tmp8A;_tmp8A.tag=0;_tmp8A.f1=(
struct _tagged_arr)sns;{void*_tmp89[2]={& _tmp8A,& _tmp8B};Cyc_Std_aprintf(_tag_arr("%s%s",
sizeof(char),5),_tag_arr(_tmp89,sizeof(void*),2));}}}));goto _LLA2;}_LLB5: if((int)
_tmp47 != 1)goto _LLB7;_LLB6: s=Cyc_PP_text(_tag_arr("float",sizeof(char),6));goto
_LLA2;_LLB7: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 6)goto _LLB9;_tmp56=((struct
Cyc_Absyn_DoubleType_struct*)_tmp47)->f1;_LLB8: if(_tmp56)s=Cyc_PP_text(_tag_arr("long double",
sizeof(char),12));else{s=Cyc_PP_text(_tag_arr("double",sizeof(char),7));}goto
_LLA2;_LLB9: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 9)goto _LLBB;_tmp57=((struct
Cyc_Absyn_TupleType_struct*)_tmp47)->f1;_LLBA: s=({struct Cyc_PP_Doc*_tmp8C[2];
_tmp8C[1]=Cyc_Absynpp_args2doc(_tmp57);_tmp8C[0]=Cyc_PP_text(_tag_arr("$",
sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp8C,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLA2;_LLBB: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 10)goto _LLBD;_tmp58=((
struct Cyc_Absyn_AggrType_struct*)_tmp47)->f1;_tmp59=(void*)_tmp58.aggr_info;
_tmp5A=_tmp58.targs;_LLBC: {void*_tmp8E;struct _tuple0*_tmp8F;struct _tuple3 _tmp8D=
Cyc_Absyn_aggr_kinded_name(_tmp59);_tmp8E=_tmp8D.f1;_tmp8F=_tmp8D.f2;s=({struct
Cyc_PP_Doc*_tmp90[3];_tmp90[2]=Cyc_Absynpp_tps2doc(_tmp5A);_tmp90[1]=Cyc_Absynpp_qvar2doc(
_tmp8F);_tmp90[0]=Cyc_Absynpp_aggr_kind2doc(_tmp8E);Cyc_PP_cat(_tag_arr(_tmp90,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLA2;}_LLBD: if(_tmp47 <= (void*)3?1:*((int*)
_tmp47)!= 11)goto _LLBF;_tmp5B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp47)->f1;_tmp5C=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp47)->f2;_LLBE: s=({
struct Cyc_PP_Doc*_tmp91[4];_tmp91[3]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp91[2]=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp5C));_tmp91[1]=Cyc_PP_text(
_tag_arr("{",sizeof(char),2));_tmp91[0]=Cyc_Absynpp_aggr_kind2doc(_tmp5B);Cyc_PP_cat(
_tag_arr(_tmp91,sizeof(struct Cyc_PP_Doc*),4));});goto _LLA2;_LLBF: if(_tmp47 <= (
void*)3?1:*((int*)_tmp47)!= 13)goto _LLC1;_tmp5D=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp47)->f1;_LLC0: s=({struct Cyc_PP_Doc*_tmp92[3];_tmp92[2]=Cyc_PP_text(_tag_arr("}",
sizeof(char),2));_tmp92[1]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp5D));
_tmp92[0]=Cyc_PP_text(_tag_arr("enum {",sizeof(char),7));Cyc_PP_cat(_tag_arr(
_tmp92,sizeof(struct Cyc_PP_Doc*),3));});goto _LLA2;_LLC1: if(_tmp47 <= (void*)3?1:*((
int*)_tmp47)!= 12)goto _LLC3;_tmp5E=((struct Cyc_Absyn_EnumType_struct*)_tmp47)->f1;
_LLC2: s=({struct Cyc_PP_Doc*_tmp93[2];_tmp93[1]=Cyc_Absynpp_qvar2doc(_tmp5E);
_tmp93[0]=Cyc_PP_text(_tag_arr("enum ",sizeof(char),6));Cyc_PP_cat(_tag_arr(
_tmp93,sizeof(struct Cyc_PP_Doc*),2));});goto _LLA2;_LLC3: if(_tmp47 <= (void*)3?1:*((
int*)_tmp47)!= 14)goto _LLC5;_tmp5F=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp47)->f1;_LLC4: s=({struct Cyc_PP_Doc*_tmp94[3];_tmp94[2]=Cyc_PP_text(_tag_arr(">",
sizeof(char),2));_tmp94[1]=Cyc_Absynpp_typ2doc(_tmp5F);_tmp94[0]=Cyc_PP_text(
_tag_arr("sizeof_t<",sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp94,sizeof(struct
Cyc_PP_Doc*),3));});goto _LLA2;_LLC5: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 16)
goto _LLC7;_tmp60=((struct Cyc_Absyn_TypedefType_struct*)_tmp47)->f1;_tmp61=((
struct Cyc_Absyn_TypedefType_struct*)_tmp47)->f2;_tmp62=((struct Cyc_Absyn_TypedefType_struct*)
_tmp47)->f3;_tmp63=((struct Cyc_Absyn_TypedefType_struct*)_tmp47)->f4;_LLC6: s=({
struct Cyc_PP_Doc*_tmp95[2];_tmp95[1]=Cyc_Absynpp_tps2doc(_tmp61);_tmp95[0]=Cyc_Absynpp_qvar2doc(
_tmp60);Cyc_PP_cat(_tag_arr(_tmp95,sizeof(struct Cyc_PP_Doc*),2));});goto _LLA2;
_LLC7: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 15)goto _LLC9;_tmp64=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp47)->f1;_LLC8: s=({struct Cyc_PP_Doc*
_tmp96[3];_tmp96[2]=Cyc_PP_text(_tag_arr(">",sizeof(char),2));_tmp96[1]=Cyc_Absynpp_rgn2doc(
_tmp64);_tmp96[0]=Cyc_PP_text(_tag_arr("region_t<",sizeof(char),10));Cyc_PP_cat(
_tag_arr(_tmp96,sizeof(struct Cyc_PP_Doc*),3));});goto _LLA2;_LLC9: if((int)_tmp47
!= 2)goto _LLCB;_LLCA: s=Cyc_Absynpp_rgn2doc(t);goto _LLA2;_LLCB: if(_tmp47 <= (void*)
3?1:*((int*)_tmp47)!= 19)goto _LLCD;_tmp65=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp47)->f1;_LLCC: s=({struct Cyc_PP_Doc*_tmp97[3];_tmp97[2]=Cyc_PP_text(_tag_arr(")",
sizeof(char),2));_tmp97[1]=Cyc_Absynpp_typ2doc(_tmp65);_tmp97[0]=Cyc_PP_text(
_tag_arr("regions(",sizeof(char),9));Cyc_PP_cat(_tag_arr(_tmp97,sizeof(struct Cyc_PP_Doc*),
3));});goto _LLA2;_LLCD: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 17)goto _LLCF;
_LLCE: goto _LLD0;_LLCF: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 18)goto _LLA2;
_LLD0: s=Cyc_Absynpp_eff2doc(t);goto _LLA2;_LLA2:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(
struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct
_tagged_arr*)vo->v));}struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple4*cmp){
struct _tuple4 _tmp99;void*_tmp9A;void*_tmp9B;struct _tuple4*_tmp98=cmp;_tmp99=*
_tmp98;_tmp9A=_tmp99.f1;_tmp9B=_tmp99.f2;return({struct Cyc_PP_Doc*_tmp9C[3];
_tmp9C[2]=Cyc_Absynpp_rgn2doc(_tmp9B);_tmp9C[1]=Cyc_PP_text(_tag_arr(" < ",
sizeof(char),4));_tmp9C[0]=Cyc_Absynpp_rgn2doc(_tmp9A);Cyc_PP_cat(_tag_arr(
_tmp9C,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List*po){return Cyc_PP_group(_tag_arr("",sizeof(char),1),_tag_arr("",
sizeof(char),1),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,
po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple1*t){struct Cyc_Core_Opt*
dopt=(*t).f1 == 0?0:({struct Cyc_Core_Opt*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->v=
Cyc_PP_text(*((struct _tagged_arr*)((struct Cyc_Core_Opt*)_check_null((*t).f1))->v));
_tmp9D;});return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmp9E=((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_PP_Doc*eff_doc;if(
c_varargs)_tmp9E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp9E,({struct Cyc_List_List*_tmp9F=_cycalloc(sizeof(*_tmp9F));
_tmp9F->hd=Cyc_PP_text(_tag_arr("...",sizeof(char),4));_tmp9F->tl=0;_tmp9F;}));
else{if(cyc_varargs != 0){struct Cyc_PP_Doc*_tmpA0=({struct Cyc_PP_Doc*_tmpA2[3];
_tmpA2[2]=Cyc_Absynpp_funarg2doc(({struct _tuple1*_tmpA3=_cycalloc(sizeof(*_tmpA3));
_tmpA3->f1=cyc_varargs->name;_tmpA3->f2=cyc_varargs->tq;_tmpA3->f3=(void*)
cyc_varargs->type;_tmpA3;}));_tmpA2[1]=cyc_varargs->inject?Cyc_PP_text(_tag_arr(" inject ",
sizeof(char),9)): Cyc_PP_text(_tag_arr(" ",sizeof(char),2));_tmpA2[0]=Cyc_PP_text(
_tag_arr("...",sizeof(char),4));Cyc_PP_cat(_tag_arr(_tmpA2,sizeof(struct Cyc_PP_Doc*),
3));});_tmp9E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_append)(_tmp9E,({struct Cyc_List_List*_tmpA1=_cycalloc(sizeof(*_tmpA1));
_tmpA1->hd=_tmpA0;_tmpA1->tl=0;_tmpA1;}));}}{struct Cyc_PP_Doc*_tmpA4=Cyc_PP_group(
_tag_arr("",sizeof(char),1),_tag_arr("",sizeof(char),1),_tag_arr(",",sizeof(char),
2),_tmp9E);if(effopt != 0)_tmpA4=({struct Cyc_PP_Doc*_tmpA5[3];_tmpA5[2]=Cyc_Absynpp_eff2doc((
void*)effopt->v);_tmpA5[1]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmpA5[0]=
_tmpA4;Cyc_PP_cat(_tag_arr(_tmpA5,sizeof(struct Cyc_PP_Doc*),3));});if(rgn_po != 0)
_tmpA4=({struct Cyc_PP_Doc*_tmpA6[3];_tmpA6[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);
_tmpA6[1]=Cyc_PP_text(_tag_arr(":",sizeof(char),2));_tmpA6[0]=_tmpA4;Cyc_PP_cat(
_tag_arr(_tmpA6,sizeof(struct Cyc_PP_Doc*),3));});return({struct Cyc_PP_Doc*_tmpA7[
3];_tmpA7[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmpA7[1]=_tmpA4;_tmpA7[0]=
Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmpA7,sizeof(
struct Cyc_PP_Doc*),3));});}}struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple5*
arg){return({struct _tuple1*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->f1=({struct
Cyc_Core_Opt*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->v=(*arg).f1;_tmpA9;});
_tmpA8->f2=(*arg).f2;_tmpA8->f3=(*arg).f3;_tmpA8;});}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(
struct _tagged_arr*v){return Cyc_PP_text(*v);}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q){struct Cyc_List_List*_tmpAA=0;int match;{void*_tmpAB=(*q).f1;
struct Cyc_List_List*_tmpAC;struct Cyc_List_List*_tmpAD;_LLF4: if((int)_tmpAB != 0)
goto _LLF6;_LLF5: _tmpAC=0;goto _LLF7;_LLF6: if(_tmpAB <= (void*)1?1:*((int*)_tmpAB)
!= 0)goto _LLF8;_tmpAC=((struct Cyc_Absyn_Rel_n_struct*)_tmpAB)->f1;_LLF7: match=0;
_tmpAA=_tmpAC;goto _LLF3;_LLF8: if(_tmpAB <= (void*)1?1:*((int*)_tmpAB)!= 1)goto
_LLF3;_tmpAD=((struct Cyc_Absyn_Abs_n_struct*)_tmpAB)->f1;_LLF9: match=Cyc_Absynpp_use_curr_namespace?((
int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_Std_strptrcmp,_tmpAD,Cyc_Absynpp_curr_namespace):
0;_tmpAA=(Cyc_Absynpp_qvar_to_Cids?Cyc_Absynpp_add_cyc_prefix: 0)?({struct Cyc_List_List*
_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->hd=Cyc_Absynpp_cyc_stringptr;_tmpAE->tl=
_tmpAD;_tmpAE;}): _tmpAD;goto _LLF3;_LLF3:;}if(Cyc_Absynpp_qvar_to_Cids)return Cyc_PP_text((
struct _tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmpAA,({struct Cyc_List_List*_tmpAF=
_cycalloc(sizeof(*_tmpAF));_tmpAF->hd=(*q).f2;_tmpAF->tl=0;_tmpAF;})),_tag_arr("_",
sizeof(char),2)));else{if(match)return Cyc_Absynpp_var2doc((*q).f2);else{return
Cyc_PP_text((struct _tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpAA,({struct Cyc_List_List*
_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->hd=(*q).f2;_tmpB0->tl=0;_tmpB0;})),
_tag_arr("::",sizeof(char),3)));}}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2doc(v);if(
Cyc_Absynpp_use_curr_namespace){void*_tmpB1=(*v).f1;struct Cyc_List_List*_tmpB2;
struct Cyc_List_List*_tmpB3;_LLFB: if((int)_tmpB1 != 0)goto _LLFD;_LLFC: goto _LLFE;
_LLFD: if(_tmpB1 <= (void*)1?1:*((int*)_tmpB1)!= 0)goto _LLFF;_tmpB2=((struct Cyc_Absyn_Rel_n_struct*)
_tmpB1)->f1;if(_tmpB2 != 0)goto _LLFF;_LLFE: return Cyc_Absynpp_var2doc((*v).f2);
_LLFF: if(_tmpB1 <= (void*)1?1:*((int*)_tmpB1)!= 1)goto _LL101;_tmpB3=((struct Cyc_Absyn_Abs_n_struct*)
_tmpB1)->f1;_LL100: if(((int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Std_strptrcmp,
_tmpB3,Cyc_Absynpp_curr_namespace)== 0)return Cyc_Absynpp_var2doc((*v).f2);else{
goto _LL102;}_LL101:;_LL102: return({struct Cyc_PP_Doc*_tmpB4[2];_tmpB4[1]=Cyc_Absynpp_qvar2doc(
v);_tmpB4[0]=Cyc_PP_text(_tag_arr("/* bad namespace : */ ",sizeof(char),23));Cyc_PP_cat(
_tag_arr(_tmpB4,sizeof(struct Cyc_PP_Doc*),2));});_LLFA:;}else{return Cyc_Absynpp_var2doc((*
v).f2);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(),t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmpB5=(void*)e->r;void*_tmpB6;struct Cyc_Absyn_Exp*_tmpB7;struct Cyc_Absyn_Exp*
_tmpB8;_LL104: if(*((int*)_tmpB5)!= 0)goto _LL106;_LL105: goto _LL107;_LL106: if(*((
int*)_tmpB5)!= 1)goto _LL108;_LL107: goto _LL109;_LL108: if(*((int*)_tmpB5)!= 2)goto
_LL10A;_LL109: return 10000;_LL10A: if(*((int*)_tmpB5)!= 3)goto _LL10C;_tmpB6=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpB5)->f1;_LL10B: {void*_tmpB9=_tmpB6;_LL151:
if((int)_tmpB9 != 0)goto _LL153;_LL152: return 100;_LL153: if((int)_tmpB9 != 1)goto
_LL155;_LL154: return 110;_LL155: if((int)_tmpB9 != 2)goto _LL157;_LL156: return 100;
_LL157: if((int)_tmpB9 != 3)goto _LL159;_LL158: goto _LL15A;_LL159: if((int)_tmpB9 != 4)
goto _LL15B;_LL15A: return 110;_LL15B: if((int)_tmpB9 != 5)goto _LL15D;_LL15C: goto
_LL15E;_LL15D: if((int)_tmpB9 != 6)goto _LL15F;_LL15E: return 70;_LL15F: if((int)
_tmpB9 != 7)goto _LL161;_LL160: goto _LL162;_LL161: if((int)_tmpB9 != 8)goto _LL163;
_LL162: goto _LL164;_LL163: if((int)_tmpB9 != 9)goto _LL165;_LL164: goto _LL166;_LL165:
if((int)_tmpB9 != 10)goto _LL167;_LL166: return 80;_LL167: if((int)_tmpB9 != 11)goto
_LL169;_LL168: goto _LL16A;_LL169: if((int)_tmpB9 != 12)goto _LL16B;_LL16A: return 130;
_LL16B: if((int)_tmpB9 != 13)goto _LL16D;_LL16C: return 60;_LL16D: if((int)_tmpB9 != 14)
goto _LL16F;_LL16E: return 40;_LL16F: if((int)_tmpB9 != 15)goto _LL171;_LL170: return 50;
_LL171: if((int)_tmpB9 != 16)goto _LL173;_LL172: return 90;_LL173: if((int)_tmpB9 != 17)
goto _LL175;_LL174: return 80;_LL175: if((int)_tmpB9 != 18)goto _LL177;_LL176: return 80;
_LL177: if((int)_tmpB9 != 19)goto _LL150;_LL178: return 140;_LL150:;}_LL10C: if(*((int*)
_tmpB5)!= 4)goto _LL10E;_LL10D: return 20;_LL10E: if(*((int*)_tmpB5)!= 5)goto _LL110;
_LL10F: return 130;_LL110: if(*((int*)_tmpB5)!= 6)goto _LL112;_LL111: return 30;_LL112:
if(*((int*)_tmpB5)!= 7)goto _LL114;_LL113: return 10;_LL114: if(*((int*)_tmpB5)!= 8)
goto _LL116;_LL115: goto _LL117;_LL116: if(*((int*)_tmpB5)!= 9)goto _LL118;_LL117:
return 140;_LL118: if(*((int*)_tmpB5)!= 10)goto _LL11A;_LL119: return 130;_LL11A: if(*((
int*)_tmpB5)!= 11)goto _LL11C;_tmpB7=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpB5)->f1;_LL11B: return Cyc_Absynpp_exp_prec(_tmpB7);_LL11C: if(*((int*)_tmpB5)
!= 12)goto _LL11E;_tmpB8=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpB5)->f1;
_LL11D: return Cyc_Absynpp_exp_prec(_tmpB8);_LL11E: if(*((int*)_tmpB5)!= 13)goto
_LL120;_LL11F: return 120;_LL120: if(*((int*)_tmpB5)!= 15)goto _LL122;_LL121: return
15;_LL122: if(*((int*)_tmpB5)!= 14)goto _LL124;_LL123: goto _LL125;_LL124: if(*((int*)
_tmpB5)!= 16)goto _LL126;_LL125: goto _LL127;_LL126: if(*((int*)_tmpB5)!= 17)goto
_LL128;_LL127: goto _LL129;_LL128: if(*((int*)_tmpB5)!= 18)goto _LL12A;_LL129: goto
_LL12B;_LL12A: if(*((int*)_tmpB5)!= 19)goto _LL12C;_LL12B: goto _LL12D;_LL12C: if(*((
int*)_tmpB5)!= 20)goto _LL12E;_LL12D: return 130;_LL12E: if(*((int*)_tmpB5)!= 21)
goto _LL130;_LL12F: goto _LL131;_LL130: if(*((int*)_tmpB5)!= 22)goto _LL132;_LL131:
goto _LL133;_LL132: if(*((int*)_tmpB5)!= 23)goto _LL134;_LL133: return 140;_LL134: if(*((
int*)_tmpB5)!= 24)goto _LL136;_LL135: return 150;_LL136: if(*((int*)_tmpB5)!= 25)
goto _LL138;_LL137: goto _LL139;_LL138: if(*((int*)_tmpB5)!= 26)goto _LL13A;_LL139:
goto _LL13B;_LL13A: if(*((int*)_tmpB5)!= 27)goto _LL13C;_LL13B: goto _LL13D;_LL13C:
if(*((int*)_tmpB5)!= 28)goto _LL13E;_LL13D: goto _LL13F;_LL13E: if(*((int*)_tmpB5)!= 
29)goto _LL140;_LL13F: goto _LL141;_LL140: if(*((int*)_tmpB5)!= 30)goto _LL142;_LL141:
goto _LL143;_LL142: if(*((int*)_tmpB5)!= 31)goto _LL144;_LL143: goto _LL145;_LL144:
if(*((int*)_tmpB5)!= 32)goto _LL146;_LL145: goto _LL147;_LL146: if(*((int*)_tmpB5)!= 
33)goto _LL148;_LL147: goto _LL149;_LL148: if(*((int*)_tmpB5)!= 34)goto _LL14A;_LL149:
return 140;_LL14A: if(*((int*)_tmpB5)!= 35)goto _LL14C;_LL14B: return 10000;_LL14C:
if(*((int*)_tmpB5)!= 36)goto _LL14E;_LL14D: goto _LL14F;_LL14E: if(*((int*)_tmpB5)!= 
37)goto _LL103;_LL14F: return 140;_LL103:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(
struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*
s;{void*_tmpBA=(void*)e->r;void*_tmpBB;struct _tuple0*_tmpBC;struct _tuple0*_tmpBD;
void*_tmpBE;struct Cyc_List_List*_tmpBF;struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_Core_Opt*
_tmpC1;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_Absyn_Exp*_tmpC3;void*_tmpC4;struct
Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC7;
struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Exp*_tmpCA;
struct Cyc_List_List*_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_List_List*_tmpCD;
struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;
void*_tmpD1;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*
_tmpD4;struct Cyc_Absyn_Exp*_tmpD5;void*_tmpD6;struct Cyc_Absyn_Exp*_tmpD7;void*
_tmpD8;void*_tmpD9;struct _tagged_arr*_tmpDA;void*_tmpDB;void*_tmpDC;unsigned int
_tmpDD;struct Cyc_List_List*_tmpDE;void*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct
Cyc_Absyn_Exp*_tmpE1;struct _tagged_arr*_tmpE2;struct Cyc_Absyn_Exp*_tmpE3;struct
_tagged_arr*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE6;struct
Cyc_List_List*_tmpE7;struct _tuple1*_tmpE8;struct Cyc_List_List*_tmpE9;struct Cyc_List_List*
_tmpEA;struct Cyc_Absyn_Vardecl*_tmpEB;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*
_tmpED;struct _tuple0*_tmpEE;struct Cyc_List_List*_tmpEF;struct Cyc_List_List*
_tmpF0;struct Cyc_List_List*_tmpF1;struct Cyc_List_List*_tmpF2;struct Cyc_Absyn_Tunionfield*
_tmpF3;struct _tuple0*_tmpF4;struct _tuple0*_tmpF5;struct Cyc_Absyn_MallocInfo
_tmpF6;int _tmpF7;struct Cyc_Absyn_Exp*_tmpF8;void**_tmpF9;struct Cyc_Absyn_Exp*
_tmpFA;struct Cyc_Core_Opt*_tmpFB;struct Cyc_List_List*_tmpFC;struct Cyc_Absyn_Stmt*
_tmpFD;struct Cyc_Absyn_Fndecl*_tmpFE;struct Cyc_Absyn_Exp*_tmpFF;_LL17A: if(*((int*)
_tmpBA)!= 0)goto _LL17C;_tmpBB=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpBA)->f1;
_LL17B: s=Cyc_Absynpp_cnst2doc(_tmpBB);goto _LL179;_LL17C: if(*((int*)_tmpBA)!= 1)
goto _LL17E;_tmpBC=((struct Cyc_Absyn_Var_e_struct*)_tmpBA)->f1;_LL17D: _tmpBD=
_tmpBC;goto _LL17F;_LL17E: if(*((int*)_tmpBA)!= 2)goto _LL180;_tmpBD=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmpBA)->f1;_LL17F: s=Cyc_Absynpp_qvar2doc(_tmpBD);goto _LL179;_LL180: if(*((int*)
_tmpBA)!= 3)goto _LL182;_tmpBE=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmpBA)->f1;
_tmpBF=((struct Cyc_Absyn_Primop_e_struct*)_tmpBA)->f2;_LL181: s=Cyc_Absynpp_primapp2doc(
myprec,_tmpBE,_tmpBF);goto _LL179;_LL182: if(*((int*)_tmpBA)!= 4)goto _LL184;_tmpC0=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpBA)->f1;_tmpC1=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpBA)->f2;_tmpC2=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpBA)->f3;_LL183: s=({
struct Cyc_PP_Doc*_tmp100[5];_tmp100[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC2);
_tmp100[3]=Cyc_PP_text(_tag_arr("= ",sizeof(char),3));_tmp100[2]=_tmpC1 == 0?Cyc_PP_text(
_tag_arr("",sizeof(char),1)): Cyc_Absynpp_prim2doc((void*)_tmpC1->v);_tmp100[1]=
Cyc_PP_text(_tag_arr(" ",sizeof(char),2));_tmp100[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpC0);Cyc_PP_cat(_tag_arr(_tmp100,sizeof(struct Cyc_PP_Doc*),5));});goto
_LL179;_LL184: if(*((int*)_tmpBA)!= 5)goto _LL186;_tmpC3=((struct Cyc_Absyn_Increment_e_struct*)
_tmpBA)->f1;_tmpC4=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpBA)->f2;
_LL185: {struct Cyc_PP_Doc*_tmp101=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC3);{void*
_tmp102=_tmpC4;_LL1C9: if((int)_tmp102 != 0)goto _LL1CB;_LL1CA: s=({struct Cyc_PP_Doc*
_tmp103[2];_tmp103[1]=_tmp101;_tmp103[0]=Cyc_PP_text(_tag_arr("++",sizeof(char),
3));Cyc_PP_cat(_tag_arr(_tmp103,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1C8;
_LL1CB: if((int)_tmp102 != 2)goto _LL1CD;_LL1CC: s=({struct Cyc_PP_Doc*_tmp104[2];
_tmp104[1]=_tmp101;_tmp104[0]=Cyc_PP_text(_tag_arr("--",sizeof(char),3));Cyc_PP_cat(
_tag_arr(_tmp104,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1C8;_LL1CD: if((int)
_tmp102 != 1)goto _LL1CF;_LL1CE: s=({struct Cyc_PP_Doc*_tmp105[2];_tmp105[1]=Cyc_PP_text(
_tag_arr("++",sizeof(char),3));_tmp105[0]=_tmp101;Cyc_PP_cat(_tag_arr(_tmp105,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL1C8;_LL1CF: if((int)_tmp102 != 3)goto
_LL1C8;_LL1D0: s=({struct Cyc_PP_Doc*_tmp106[2];_tmp106[1]=Cyc_PP_text(_tag_arr("--",
sizeof(char),3));_tmp106[0]=_tmp101;Cyc_PP_cat(_tag_arr(_tmp106,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1C8;_LL1C8:;}goto _LL179;}_LL186: if(*((int*)_tmpBA)!= 6)goto _LL188;
_tmpC5=((struct Cyc_Absyn_Conditional_e_struct*)_tmpBA)->f1;_tmpC6=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpBA)->f2;_tmpC7=((struct Cyc_Absyn_Conditional_e_struct*)_tmpBA)->f3;_LL187:{
struct _tuple4 _tmp108=({struct _tuple4 _tmp107;_tmp107.f1=(void*)_tmpC6->r;_tmp107.f2=(
void*)_tmpC7->r;_tmp107;});_LL1D2:;_LL1D3: s=({struct Cyc_PP_Doc*_tmp109[5];
_tmp109[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC7);_tmp109[3]=Cyc_PP_text(
_tag_arr(" : ",sizeof(char),4));_tmp109[2]=Cyc_Absynpp_exp2doc_prec(0,_tmpC6);
_tmp109[1]=Cyc_PP_text(_tag_arr(" ? ",sizeof(char),4));_tmp109[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpC5);Cyc_PP_cat(_tag_arr(_tmp109,sizeof(struct Cyc_PP_Doc*),5));});goto
_LL1D1;_LL1D1:;}goto _LL179;_LL188: if(*((int*)_tmpBA)!= 7)goto _LL18A;_tmpC8=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmpBA)->f1;_tmpC9=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpBA)->f2;_LL189: s=({struct Cyc_PP_Doc*_tmp10A[5];_tmp10A[4]=Cyc_PP_text(
_tag_arr(")",sizeof(char),2));_tmp10A[3]=Cyc_Absynpp_exp2doc(_tmpC9);_tmp10A[2]=
Cyc_PP_text(_tag_arr(", ",sizeof(char),3));_tmp10A[1]=Cyc_Absynpp_exp2doc(_tmpC8);
_tmp10A[0]=Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp10A,
sizeof(struct Cyc_PP_Doc*),5));});goto _LL179;_LL18A: if(*((int*)_tmpBA)!= 8)goto
_LL18C;_tmpCA=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmpBA)->f1;_tmpCB=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmpBA)->f2;_LL18B: s=({struct Cyc_PP_Doc*
_tmp10B[4];_tmp10B[3]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp10B[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmpCB);_tmp10B[1]=Cyc_PP_text(_tag_arr("(",sizeof(char),2));_tmp10B[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpCA);Cyc_PP_cat(_tag_arr(_tmp10B,sizeof(struct Cyc_PP_Doc*),4));});goto
_LL179;_LL18C: if(*((int*)_tmpBA)!= 9)goto _LL18E;_tmpCC=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpBA)->f1;_tmpCD=((struct Cyc_Absyn_FnCall_e_struct*)_tmpBA)->f2;_LL18D: s=({
struct Cyc_PP_Doc*_tmp10C[4];_tmp10C[3]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp10C[2]=Cyc_Absynpp_exps2doc_prec(20,_tmpCD);_tmp10C[1]=Cyc_PP_text(_tag_arr("(",
sizeof(char),2));_tmp10C[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpCC);Cyc_PP_cat(
_tag_arr(_tmp10C,sizeof(struct Cyc_PP_Doc*),4));});goto _LL179;_LL18E: if(*((int*)
_tmpBA)!= 10)goto _LL190;_tmpCE=((struct Cyc_Absyn_Throw_e_struct*)_tmpBA)->f1;
_LL18F: s=({struct Cyc_PP_Doc*_tmp10D[2];_tmp10D[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpCE);_tmp10D[0]=Cyc_PP_text(_tag_arr("throw ",sizeof(char),7));Cyc_PP_cat(
_tag_arr(_tmp10D,sizeof(struct Cyc_PP_Doc*),2));});goto _LL179;_LL190: if(*((int*)
_tmpBA)!= 11)goto _LL192;_tmpCF=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpBA)->f1;
_LL191: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmpCF);goto _LL179;_LL192: if(*((int*)
_tmpBA)!= 12)goto _LL194;_tmpD0=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpBA)->f1;
_LL193: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmpD0);goto _LL179;_LL194: if(*((int*)
_tmpBA)!= 13)goto _LL196;_tmpD1=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpBA)->f1;
_tmpD2=((struct Cyc_Absyn_Cast_e_struct*)_tmpBA)->f2;_LL195: s=({struct Cyc_PP_Doc*
_tmp10E[4];_tmp10E[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpD2);_tmp10E[2]=Cyc_PP_text(
_tag_arr(")",sizeof(char),2));_tmp10E[1]=Cyc_Absynpp_typ2doc(_tmpD1);_tmp10E[0]=
Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp10E,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL179;_LL196: if(*((int*)_tmpBA)!= 14)goto _LL198;
_tmpD3=((struct Cyc_Absyn_Address_e_struct*)_tmpBA)->f1;_LL197: s=({struct Cyc_PP_Doc*
_tmp10F[2];_tmp10F[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpD3);_tmp10F[0]=Cyc_PP_text(
_tag_arr("&",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp10F,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL179;_LL198: if(*((int*)_tmpBA)!= 15)goto _LL19A;_tmpD4=((struct Cyc_Absyn_New_e_struct*)
_tmpBA)->f1;_tmpD5=((struct Cyc_Absyn_New_e_struct*)_tmpBA)->f2;_LL199: if(_tmpD4
== 0)s=({struct Cyc_PP_Doc*_tmp110[2];_tmp110[1]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmpD5);_tmp110[0]=Cyc_PP_text(_tag_arr("new ",sizeof(char),5));Cyc_PP_cat(
_tag_arr(_tmp110,sizeof(struct Cyc_PP_Doc*),2));});else{s=({struct Cyc_PP_Doc*
_tmp111[4];_tmp111[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpD5);_tmp111[2]=Cyc_PP_text(
_tag_arr(") ",sizeof(char),3));_tmp111[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmpD4));_tmp111[0]=Cyc_PP_text(_tag_arr("rnew(",sizeof(char),6));
Cyc_PP_cat(_tag_arr(_tmp111,sizeof(struct Cyc_PP_Doc*),4));});}goto _LL179;_LL19A:
if(*((int*)_tmpBA)!= 16)goto _LL19C;_tmpD6=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpBA)->f1;_LL19B: s=({struct Cyc_PP_Doc*_tmp112[3];_tmp112[2]=Cyc_PP_text(
_tag_arr(")",sizeof(char),2));_tmp112[1]=Cyc_Absynpp_typ2doc(_tmpD6);_tmp112[0]=
Cyc_PP_text(_tag_arr("sizeof(",sizeof(char),8));Cyc_PP_cat(_tag_arr(_tmp112,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL179;_LL19C: if(*((int*)_tmpBA)!= 17)goto
_LL19E;_tmpD7=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpBA)->f1;_LL19D: s=({
struct Cyc_PP_Doc*_tmp113[3];_tmp113[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp113[1]=Cyc_Absynpp_exp2doc(_tmpD7);_tmp113[0]=Cyc_PP_text(_tag_arr("sizeof(",
sizeof(char),8));Cyc_PP_cat(_tag_arr(_tmp113,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL179;_LL19E: if(*((int*)_tmpBA)!= 18)goto _LL1A0;_tmpD8=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpBA)->f1;_tmpD9=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpBA)->f2;if(*((
int*)_tmpD9)!= 0)goto _LL1A0;_tmpDA=((struct Cyc_Absyn_StructField_struct*)_tmpD9)->f1;
_LL19F: s=({struct Cyc_PP_Doc*_tmp114[5];_tmp114[4]=Cyc_PP_text(_tag_arr(")",
sizeof(char),2));_tmp114[3]=Cyc_PP_textptr(_tmpDA);_tmp114[2]=Cyc_PP_text(
_tag_arr(",",sizeof(char),2));_tmp114[1]=Cyc_Absynpp_typ2doc(_tmpD8);_tmp114[0]=
Cyc_PP_text(_tag_arr("offsetof(",sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp114,
sizeof(struct Cyc_PP_Doc*),5));});goto _LL179;_LL1A0: if(*((int*)_tmpBA)!= 18)goto
_LL1A2;_tmpDB=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpBA)->f1;_tmpDC=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpBA)->f2;if(*((int*)_tmpDC)!= 1)
goto _LL1A2;_tmpDD=((struct Cyc_Absyn_TupleIndex_struct*)_tmpDC)->f1;_LL1A1: s=({
struct Cyc_PP_Doc*_tmp115[5];_tmp115[4]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp115[3]=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp117;
_tmp117.tag=1;_tmp117.f1=(int)_tmpDD;{void*_tmp116[1]={& _tmp117};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(char),3),_tag_arr(_tmp116,sizeof(void*),1));}}));_tmp115[2]=
Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp115[1]=Cyc_Absynpp_typ2doc(_tmpDB);
_tmp115[0]=Cyc_PP_text(_tag_arr("offsetof(",sizeof(char),10));Cyc_PP_cat(
_tag_arr(_tmp115,sizeof(struct Cyc_PP_Doc*),5));});goto _LL179;_LL1A2: if(*((int*)
_tmpBA)!= 19)goto _LL1A4;_tmpDE=((struct Cyc_Absyn_Gentyp_e_struct*)_tmpBA)->f1;
_tmpDF=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmpBA)->f2;_LL1A3: s=({struct
Cyc_PP_Doc*_tmp118[4];_tmp118[3]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp118[2]=Cyc_Absynpp_typ2doc(_tmpDF);_tmp118[1]=Cyc_Absynpp_tvars2doc(_tmpDE);
_tmp118[0]=Cyc_PP_text(_tag_arr("__gen(",sizeof(char),7));Cyc_PP_cat(_tag_arr(
_tmp118,sizeof(struct Cyc_PP_Doc*),4));});goto _LL179;_LL1A4: if(*((int*)_tmpBA)!= 
20)goto _LL1A6;_tmpE0=((struct Cyc_Absyn_Deref_e_struct*)_tmpBA)->f1;_LL1A5: s=({
struct Cyc_PP_Doc*_tmp119[2];_tmp119[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpE0);
_tmp119[0]=Cyc_PP_text(_tag_arr("*",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp119,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL179;_LL1A6: if(*((int*)_tmpBA)!= 21)goto
_LL1A8;_tmpE1=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpBA)->f1;_tmpE2=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmpBA)->f2;_LL1A7: s=({struct Cyc_PP_Doc*_tmp11A[3];
_tmp11A[2]=Cyc_PP_textptr(_tmpE2);_tmp11A[1]=Cyc_PP_text(_tag_arr(".",sizeof(
char),2));_tmp11A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpE1);Cyc_PP_cat(_tag_arr(
_tmp11A,sizeof(struct Cyc_PP_Doc*),3));});goto _LL179;_LL1A8: if(*((int*)_tmpBA)!= 
22)goto _LL1AA;_tmpE3=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpBA)->f1;_tmpE4=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmpBA)->f2;_LL1A9: s=({struct Cyc_PP_Doc*
_tmp11B[3];_tmp11B[2]=Cyc_PP_textptr(_tmpE4);_tmp11B[1]=Cyc_PP_text(_tag_arr("->",
sizeof(char),3));_tmp11B[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpE3);Cyc_PP_cat(
_tag_arr(_tmp11B,sizeof(struct Cyc_PP_Doc*),3));});goto _LL179;_LL1AA: if(*((int*)
_tmpBA)!= 23)goto _LL1AC;_tmpE5=((struct Cyc_Absyn_Subscript_e_struct*)_tmpBA)->f1;
_tmpE6=((struct Cyc_Absyn_Subscript_e_struct*)_tmpBA)->f2;_LL1AB: s=({struct Cyc_PP_Doc*
_tmp11C[4];_tmp11C[3]=Cyc_PP_text(_tag_arr("]",sizeof(char),2));_tmp11C[2]=Cyc_Absynpp_exp2doc(
_tmpE6);_tmp11C[1]=Cyc_PP_text(_tag_arr("[",sizeof(char),2));_tmp11C[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpE5);Cyc_PP_cat(_tag_arr(_tmp11C,sizeof(struct Cyc_PP_Doc*),4));});goto
_LL179;_LL1AC: if(*((int*)_tmpBA)!= 24)goto _LL1AE;_tmpE7=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpBA)->f1;_LL1AD: s=({struct Cyc_PP_Doc*_tmp11D[3];_tmp11D[2]=Cyc_PP_text(
_tag_arr(")",sizeof(char),2));_tmp11D[1]=Cyc_Absynpp_exps2doc_prec(20,_tmpE7);
_tmp11D[0]=Cyc_PP_text(_tag_arr("$(",sizeof(char),3));Cyc_PP_cat(_tag_arr(
_tmp11D,sizeof(struct Cyc_PP_Doc*),3));});goto _LL179;_LL1AE: if(*((int*)_tmpBA)!= 
25)goto _LL1B0;_tmpE8=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpBA)->f1;_tmpE9=((
struct Cyc_Absyn_CompoundLit_e_struct*)_tmpBA)->f2;_LL1AF: s=({struct Cyc_PP_Doc*
_tmp11E[4];_tmp11E[3]=Cyc_PP_group(_tag_arr("{",sizeof(char),2),_tag_arr("}",
sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpE9));_tmp11E[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp11E[1]=Cyc_Absynpp_typ2doc((*
_tmpE8).f3);_tmp11E[0]=Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(
_tag_arr(_tmp11E,sizeof(struct Cyc_PP_Doc*),4));});goto _LL179;_LL1B0: if(*((int*)
_tmpBA)!= 26)goto _LL1B2;_tmpEA=((struct Cyc_Absyn_Array_e_struct*)_tmpBA)->f1;
_LL1B1: s=Cyc_PP_group(_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(char),2),
_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmpEA));
goto _LL179;_LL1B2: if(*((int*)_tmpBA)!= 27)goto _LL1B4;_tmpEB=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpBA)->f1;_tmpEC=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpBA)->f2;_tmpED=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmpBA)->f3;_LL1B3: s=({struct Cyc_PP_Doc*
_tmp11F[7];_tmp11F[6]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp11F[5]=Cyc_Absynpp_exp2doc(
_tmpED);_tmp11F[4]=Cyc_PP_text(_tag_arr(" : ",sizeof(char),4));_tmp11F[3]=Cyc_Absynpp_exp2doc(
_tmpEC);_tmp11F[2]=Cyc_PP_text(_tag_arr(" < ",sizeof(char),4));_tmp11F[1]=Cyc_PP_text(*(*
_tmpEB->name).f2);_tmp11F[0]=Cyc_PP_text(_tag_arr("{for ",sizeof(char),6));Cyc_PP_cat(
_tag_arr(_tmp11F,sizeof(struct Cyc_PP_Doc*),7));});goto _LL179;_LL1B4: if(*((int*)
_tmpBA)!= 28)goto _LL1B6;_tmpEE=((struct Cyc_Absyn_Struct_e_struct*)_tmpBA)->f1;
_tmpEF=((struct Cyc_Absyn_Struct_e_struct*)_tmpBA)->f2;_tmpF0=((struct Cyc_Absyn_Struct_e_struct*)
_tmpBA)->f3;_LL1B5: {struct Cyc_List_List*_tmp120=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpF0);s=({struct Cyc_PP_Doc*_tmp121[2];_tmp121[1]=Cyc_PP_group(_tag_arr("{",
sizeof(char),2),_tag_arr("}",sizeof(char),2),_tag_arr(",",sizeof(char),2),_tmpEF
!= 0?({struct Cyc_List_List*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->hd=Cyc_Absynpp_tps2doc(
_tmpEF);_tmp122->tl=_tmp120;_tmp122;}): _tmp120);_tmp121[0]=Cyc_Absynpp_qvar2doc(
_tmpEE);Cyc_PP_cat(_tag_arr(_tmp121,sizeof(struct Cyc_PP_Doc*),2));});goto _LL179;}
_LL1B6: if(*((int*)_tmpBA)!= 29)goto _LL1B8;_tmpF1=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpBA)->f2;_LL1B7: s=Cyc_PP_group(_tag_arr("{",sizeof(char),2),_tag_arr("}",
sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpF1));goto _LL179;_LL1B8: if(*((int*)_tmpBA)!= 30)goto _LL1BA;_tmpF2=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpBA)->f1;_tmpF3=((struct Cyc_Absyn_Tunion_e_struct*)_tmpBA)->f3;_LL1B9: if(
_tmpF2 == 0)s=Cyc_Absynpp_qvar2doc(_tmpF3->name);else{s=({struct Cyc_PP_Doc*
_tmp123[2];_tmp123[1]=Cyc_PP_egroup(_tag_arr("(",sizeof(char),2),_tag_arr(")",
sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,
_tmpF2));_tmp123[0]=Cyc_Absynpp_qvar2doc(_tmpF3->name);Cyc_PP_cat(_tag_arr(
_tmp123,sizeof(struct Cyc_PP_Doc*),2));});}goto _LL179;_LL1BA: if(*((int*)_tmpBA)!= 
31)goto _LL1BC;_tmpF4=((struct Cyc_Absyn_Enum_e_struct*)_tmpBA)->f1;_LL1BB: s=Cyc_Absynpp_qvar2doc(
_tmpF4);goto _LL179;_LL1BC: if(*((int*)_tmpBA)!= 32)goto _LL1BE;_tmpF5=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpBA)->f1;_LL1BD: s=Cyc_Absynpp_qvar2doc(_tmpF5);goto _LL179;_LL1BE: if(*((int*)
_tmpBA)!= 33)goto _LL1C0;_tmpF6=((struct Cyc_Absyn_Malloc_e_struct*)_tmpBA)->f1;
_tmpF7=_tmpF6.is_calloc;_tmpF8=_tmpF6.rgn;_tmpF9=_tmpF6.elt_type;_tmpFA=_tmpF6.num_elts;
_LL1BF: if(_tmpF7){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)
_check_null(_tmpF9)),0);if(_tmpF8 == 0)s=({struct Cyc_PP_Doc*_tmp124[5];_tmp124[4]=
Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp124[3]=Cyc_Absynpp_exp2doc(st);
_tmp124[2]=Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp124[1]=Cyc_Absynpp_exp2doc(
_tmpFA);_tmp124[0]=Cyc_PP_text(_tag_arr("calloc(",sizeof(char),8));Cyc_PP_cat(
_tag_arr(_tmp124,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp125[7];_tmp125[6]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp125[5]=Cyc_Absynpp_exp2doc(
st);_tmp125[4]=Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp125[3]=Cyc_Absynpp_exp2doc(
_tmpFA);_tmp125[2]=Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp125[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(_tmpF8));_tmp125[0]=Cyc_PP_text(_tag_arr("rcalloc(",
sizeof(char),9));Cyc_PP_cat(_tag_arr(_tmp125,sizeof(struct Cyc_PP_Doc*),7));});}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmpF9 == 0)new_e=_tmpFA;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmpF9,0),_tmpFA,0);}if(_tmpF8 == 0)s=({struct Cyc_PP_Doc*
_tmp126[3];_tmp126[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp126[1]=Cyc_Absynpp_exp2doc(
new_e);_tmp126[0]=Cyc_PP_text(_tag_arr("malloc(",sizeof(char),8));Cyc_PP_cat(
_tag_arr(_tmp126,sizeof(struct Cyc_PP_Doc*),3));});else{s=({struct Cyc_PP_Doc*
_tmp127[5];_tmp127[4]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp127[3]=Cyc_Absynpp_exp2doc(
new_e);_tmp127[2]=Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp127[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(_tmpF8));_tmp127[0]=Cyc_PP_text(_tag_arr("rmalloc(",
sizeof(char),9));Cyc_PP_cat(_tag_arr(_tmp127,sizeof(struct Cyc_PP_Doc*),5));});}}
goto _LL179;_LL1C0: if(*((int*)_tmpBA)!= 34)goto _LL1C2;_tmpFB=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpBA)->f1;_tmpFC=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpBA)->f2;_LL1C1:
s=Cyc_PP_group(_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(char),2),
_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmpFC));
goto _LL179;_LL1C2: if(*((int*)_tmpBA)!= 35)goto _LL1C4;_tmpFD=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpBA)->f1;_LL1C3: s=({struct Cyc_PP_Doc*_tmp128[3];_tmp128[2]=Cyc_PP_text(
_tag_arr(" })",sizeof(char),4));_tmp128[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmpFD));_tmp128[0]=Cyc_PP_text(_tag_arr("({ ",sizeof(char),4));Cyc_PP_cat(
_tag_arr(_tmp128,sizeof(struct Cyc_PP_Doc*),3));});goto _LL179;_LL1C4: if(*((int*)
_tmpBA)!= 36)goto _LL1C6;_tmpFE=((struct Cyc_Absyn_Codegen_e_struct*)_tmpBA)->f1;
_LL1C5: s=({struct Cyc_PP_Doc*_tmp129[3];_tmp129[2]=Cyc_PP_text(_tag_arr(")",
sizeof(char),2));_tmp129[1]=Cyc_PP_nest(2,Cyc_Absynpp_decl2doc(({struct Cyc_Absyn_Decl*
_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->r=(void*)((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp12C;_tmp12C.tag=1;_tmp12C.f1=_tmpFE;_tmp12C;});_tmp12B;}));_tmp12A->loc=e->loc;
_tmp12A;})));_tmp129[0]=Cyc_PP_text(_tag_arr("codegen(",sizeof(char),9));Cyc_PP_cat(
_tag_arr(_tmp129,sizeof(struct Cyc_PP_Doc*),3));});goto _LL179;_LL1C6: if(*((int*)
_tmpBA)!= 37)goto _LL179;_tmpFF=((struct Cyc_Absyn_Fill_e_struct*)_tmpBA)->f1;
_LL1C7: s=({struct Cyc_PP_Doc*_tmp12D[3];_tmp12D[2]=Cyc_PP_text(_tag_arr(")",
sizeof(char),2));_tmp12D[1]=Cyc_PP_nest(2,Cyc_Absynpp_exp2doc(_tmpFF));_tmp12D[0]=
Cyc_PP_text(_tag_arr("fill(",sizeof(char),6));Cyc_PP_cat(_tag_arr(_tmp12D,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL179;_LL179:;}if(inprec >= myprec)s=({
struct Cyc_PP_Doc*_tmp12E[3];_tmp12E[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp12E[1]=s;_tmp12E[0]=Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(
_tag_arr(_tmp12E,sizeof(struct Cyc_PP_Doc*),3));});return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d){void*_tmp12F=d;struct Cyc_Absyn_Exp*_tmp130;struct _tagged_arr*_tmp131;
_LL1D5: if(*((int*)_tmp12F)!= 0)goto _LL1D7;_tmp130=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp12F)->f1;_LL1D6: return({struct Cyc_PP_Doc*_tmp132[3];_tmp132[2]=Cyc_PP_text(
_tag_arr("]",sizeof(char),2));_tmp132[1]=Cyc_Absynpp_exp2doc(_tmp130);_tmp132[0]=
Cyc_PP_text(_tag_arr(".[",sizeof(char),3));Cyc_PP_cat(_tag_arr(_tmp132,sizeof(
struct Cyc_PP_Doc*),3));});_LL1D7: if(*((int*)_tmp12F)!= 1)goto _LL1D4;_tmp131=((
struct Cyc_Absyn_FieldName_struct*)_tmp12F)->f1;_LL1D8: return({struct Cyc_PP_Doc*
_tmp133[2];_tmp133[1]=Cyc_PP_textptr(_tmp131);_tmp133[0]=Cyc_PP_text(_tag_arr(".",
sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp133,sizeof(struct Cyc_PP_Doc*),2));});
_LL1D4:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple8*de){if((*de).f1 == 0)
return Cyc_Absynpp_exp2doc((*de).f2);else{return({struct Cyc_PP_Doc*_tmp134[2];
_tmp134[1]=Cyc_Absynpp_exp2doc((*de).f2);_tmp134[0]=Cyc_PP_egroup(_tag_arr("",
sizeof(char),1),_tag_arr("=",sizeof(char),2),_tag_arr("=",sizeof(char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1));Cyc_PP_cat(_tag_arr(_tmp134,sizeof(struct
Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){return Cyc_PP_group(_tag_arr("",sizeof(char),1),_tag_arr("",
sizeof(char),1),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*c){void*_tmp135=c;void*
_tmp136;char _tmp137;void*_tmp138;short _tmp139;void*_tmp13A;int _tmp13B;void*
_tmp13C;int _tmp13D;void*_tmp13E;long long _tmp13F;struct _tagged_arr _tmp140;struct
_tagged_arr _tmp141;_LL1DA: if(_tmp135 <= (void*)1?1:*((int*)_tmp135)!= 0)goto
_LL1DC;_tmp136=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp135)->f1;_tmp137=((
struct Cyc_Absyn_Char_c_struct*)_tmp135)->f2;_LL1DB: return Cyc_PP_text((struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp143;_tmp143.tag=0;_tmp143.f1=(
struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp137);{void*_tmp142[1]={& _tmp143};
Cyc_Std_aprintf(_tag_arr("'%s'",sizeof(char),5),_tag_arr(_tmp142,sizeof(void*),1));}}));
_LL1DC: if(_tmp135 <= (void*)1?1:*((int*)_tmp135)!= 1)goto _LL1DE;_tmp138=(void*)((
struct Cyc_Absyn_Short_c_struct*)_tmp135)->f1;_tmp139=((struct Cyc_Absyn_Short_c_struct*)
_tmp135)->f2;_LL1DD: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp145;_tmp145.tag=1;_tmp145.f1=(int)((unsigned int)((int)_tmp139));{void*
_tmp144[1]={& _tmp145};Cyc_Std_aprintf(_tag_arr("%d",sizeof(char),3),_tag_arr(
_tmp144,sizeof(void*),1));}}));_LL1DE: if(_tmp135 <= (void*)1?1:*((int*)_tmp135)!= 
2)goto _LL1E0;_tmp13A=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp135)->f1;if((int)
_tmp13A != 0)goto _LL1E0;_tmp13B=((struct Cyc_Absyn_Int_c_struct*)_tmp135)->f2;
_LL1DF: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp147;
_tmp147.tag=1;_tmp147.f1=(int)((unsigned int)_tmp13B);{void*_tmp146[1]={& _tmp147};
Cyc_Std_aprintf(_tag_arr("%d",sizeof(char),3),_tag_arr(_tmp146,sizeof(void*),1));}}));
_LL1E0: if(_tmp135 <= (void*)1?1:*((int*)_tmp135)!= 2)goto _LL1E2;_tmp13C=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp135)->f1;if((int)_tmp13C != 1)goto _LL1E2;
_tmp13D=((struct Cyc_Absyn_Int_c_struct*)_tmp135)->f2;_LL1E1: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp149;_tmp149.tag=1;_tmp149.f1=(
unsigned int)_tmp13D;{void*_tmp148[1]={& _tmp149};Cyc_Std_aprintf(_tag_arr("%u",
sizeof(char),3),_tag_arr(_tmp148,sizeof(void*),1));}}));_LL1E2: if(_tmp135 <= (
void*)1?1:*((int*)_tmp135)!= 3)goto _LL1E4;_tmp13E=(void*)((struct Cyc_Absyn_LongLong_c_struct*)
_tmp135)->f1;_tmp13F=((struct Cyc_Absyn_LongLong_c_struct*)_tmp135)->f2;_LL1E3:
return Cyc_PP_text(_tag_arr("<<FIX LONG LONG CONSTANT>>",sizeof(char),27));_LL1E4:
if(_tmp135 <= (void*)1?1:*((int*)_tmp135)!= 4)goto _LL1E6;_tmp140=((struct Cyc_Absyn_Float_c_struct*)
_tmp135)->f1;_LL1E5: return Cyc_PP_text(_tmp140);_LL1E6: if((int)_tmp135 != 0)goto
_LL1E8;_LL1E7: return Cyc_PP_text(_tag_arr("NULL",sizeof(char),5));_LL1E8: if(
_tmp135 <= (void*)1?1:*((int*)_tmp135)!= 5)goto _LL1D9;_tmp141=((struct Cyc_Absyn_String_c_struct*)
_tmp135)->f1;_LL1E9: return({struct Cyc_PP_Doc*_tmp14A[3];_tmp14A[2]=Cyc_PP_text(
_tag_arr("\"",sizeof(char),2));_tmp14A[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(
_tmp141));_tmp14A[0]=Cyc_PP_text(_tag_arr("\"",sizeof(char),2));Cyc_PP_cat(
_tag_arr(_tmp14A,sizeof(struct Cyc_PP_Doc*),3));});_LL1D9:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(
int inprec,void*p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(
p);if(p == (void*)19){if(es == 0?1: es->tl != 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B[0]=({struct Cyc_Core_Failure_struct
_tmp14C;_tmp14C.tag=Cyc_Core_Failure;_tmp14C.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp14E;_tmp14E.tag=0;_tmp14E.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp14D[1]={& _tmp14E};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof(char),44),_tag_arr(_tmp14D,sizeof(void*),1));}});_tmp14C;});_tmp14B;}));{
struct Cyc_PP_Doc*_tmp14F=Cyc_Absynpp_exp2doc_prec(inprec,(struct Cyc_Absyn_Exp*)
es->hd);return({struct Cyc_PP_Doc*_tmp150[2];_tmp150[1]=Cyc_PP_text(_tag_arr(".size",
sizeof(char),6));_tmp150[0]=_tmp14F;Cyc_PP_cat(_tag_arr(_tmp150,sizeof(struct Cyc_PP_Doc*),
2));});}}else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es);if(ds == 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp151=
_cycalloc(sizeof(*_tmp151));_tmp151[0]=({struct Cyc_Core_Failure_struct _tmp152;
_tmp152.tag=Cyc_Core_Failure;_tmp152.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp154;_tmp154.tag=0;_tmp154.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp153[1]={& _tmp154};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc: %s with no args",
sizeof(char),38),_tag_arr(_tmp153,sizeof(void*),1));}});_tmp152;});_tmp151;}));
else{if(ds->tl == 0)return({struct Cyc_PP_Doc*_tmp155[3];_tmp155[2]=(struct Cyc_PP_Doc*)
ds->hd;_tmp155[1]=Cyc_PP_text(_tag_arr(" ",sizeof(char),2));_tmp155[0]=ps;Cyc_PP_cat(
_tag_arr(_tmp155,sizeof(struct Cyc_PP_Doc*),3));});else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156[0]=({struct Cyc_Core_Failure_struct
_tmp157;_tmp157.tag=Cyc_Core_Failure;_tmp157.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp159;_tmp159.tag=0;_tmp159.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp158[1]={& _tmp159};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof(char),47),_tag_arr(_tmp158,sizeof(void*),1));}});_tmp157;});_tmp156;}));
else{return({struct Cyc_PP_Doc*_tmp15A[5];_tmp15A[4]=(struct Cyc_PP_Doc*)((struct
Cyc_List_List*)_check_null(ds->tl))->hd;_tmp15A[3]=Cyc_PP_text(_tag_arr(" ",
sizeof(char),2));_tmp15A[2]=ps;_tmp15A[1]=Cyc_PP_text(_tag_arr(" ",sizeof(char),
2));_tmp15A[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_arr(_tmp15A,sizeof(
struct Cyc_PP_Doc*),5));});}}}}}struct _tagged_arr Cyc_Absynpp_prim2str(void*p){
void*_tmp15B=p;_LL1EB: if((int)_tmp15B != 0)goto _LL1ED;_LL1EC: return _tag_arr("+",
sizeof(char),2);_LL1ED: if((int)_tmp15B != 1)goto _LL1EF;_LL1EE: return _tag_arr("*",
sizeof(char),2);_LL1EF: if((int)_tmp15B != 2)goto _LL1F1;_LL1F0: return _tag_arr("-",
sizeof(char),2);_LL1F1: if((int)_tmp15B != 3)goto _LL1F3;_LL1F2: return _tag_arr("/",
sizeof(char),2);_LL1F3: if((int)_tmp15B != 4)goto _LL1F5;_LL1F4: return _tag_arr("%",
sizeof(char),2);_LL1F5: if((int)_tmp15B != 5)goto _LL1F7;_LL1F6: return _tag_arr("==",
sizeof(char),3);_LL1F7: if((int)_tmp15B != 6)goto _LL1F9;_LL1F8: return _tag_arr("!=",
sizeof(char),3);_LL1F9: if((int)_tmp15B != 7)goto _LL1FB;_LL1FA: return _tag_arr(">",
sizeof(char),2);_LL1FB: if((int)_tmp15B != 8)goto _LL1FD;_LL1FC: return _tag_arr("<",
sizeof(char),2);_LL1FD: if((int)_tmp15B != 9)goto _LL1FF;_LL1FE: return _tag_arr(">=",
sizeof(char),3);_LL1FF: if((int)_tmp15B != 10)goto _LL201;_LL200: return _tag_arr("<=",
sizeof(char),3);_LL201: if((int)_tmp15B != 11)goto _LL203;_LL202: return _tag_arr("!",
sizeof(char),2);_LL203: if((int)_tmp15B != 12)goto _LL205;_LL204: return _tag_arr("~",
sizeof(char),2);_LL205: if((int)_tmp15B != 13)goto _LL207;_LL206: return _tag_arr("&",
sizeof(char),2);_LL207: if((int)_tmp15B != 14)goto _LL209;_LL208: return _tag_arr("|",
sizeof(char),2);_LL209: if((int)_tmp15B != 15)goto _LL20B;_LL20A: return _tag_arr("^",
sizeof(char),2);_LL20B: if((int)_tmp15B != 16)goto _LL20D;_LL20C: return _tag_arr("<<",
sizeof(char),3);_LL20D: if((int)_tmp15B != 17)goto _LL20F;_LL20E: return _tag_arr(">>",
sizeof(char),3);_LL20F: if((int)_tmp15B != 18)goto _LL211;_LL210: return _tag_arr(">>>",
sizeof(char),4);_LL211: if((int)_tmp15B != 19)goto _LL1EA;_LL212: return _tag_arr("size",
sizeof(char),5);_LL1EA:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p){return Cyc_PP_text(
Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp15C=(void*)s->r;_LL214: if(_tmp15C <= (void*)1?1:*((int*)_tmp15C)!= 12)
goto _LL216;_LL215: return 1;_LL216:;_LL217: return 0;_LL213:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st){struct Cyc_PP_Doc*s;{void*_tmp15D=(void*)st->r;struct Cyc_Absyn_Exp*
_tmp15E;struct Cyc_Absyn_Stmt*_tmp15F;struct Cyc_Absyn_Stmt*_tmp160;struct Cyc_Absyn_Exp*
_tmp161;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Stmt*_tmp163;struct Cyc_Absyn_Stmt*
_tmp164;struct _tuple2 _tmp165;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Stmt*
_tmp167;struct _tagged_arr*_tmp168;struct Cyc_Absyn_Exp*_tmp169;struct _tuple2
_tmp16A;struct Cyc_Absyn_Exp*_tmp16B;struct _tuple2 _tmp16C;struct Cyc_Absyn_Exp*
_tmp16D;struct Cyc_Absyn_Stmt*_tmp16E;struct Cyc_Absyn_ForArrayInfo _tmp16F;struct
Cyc_List_List*_tmp170;struct _tuple2 _tmp171;struct Cyc_Absyn_Exp*_tmp172;struct
_tuple2 _tmp173;struct Cyc_Absyn_Exp*_tmp174;struct Cyc_Absyn_Stmt*_tmp175;struct
Cyc_Absyn_Exp*_tmp176;struct Cyc_List_List*_tmp177;struct Cyc_Absyn_Exp*_tmp178;
struct Cyc_List_List*_tmp179;struct Cyc_List_List*_tmp17A;struct Cyc_List_List*
_tmp17B;struct Cyc_Absyn_Decl*_tmp17C;struct Cyc_Absyn_Stmt*_tmp17D;struct Cyc_Absyn_Stmt*
_tmp17E;struct Cyc_Absyn_Stmt*_tmp17F;struct _tagged_arr*_tmp180;struct Cyc_Absyn_Stmt*
_tmp181;struct Cyc_Absyn_Stmt*_tmp182;struct _tuple2 _tmp183;struct Cyc_Absyn_Exp*
_tmp184;struct Cyc_Absyn_Stmt*_tmp185;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Tvar*
_tmp187;struct Cyc_Absyn_Vardecl*_tmp188;int _tmp189;struct Cyc_Absyn_Stmt*_tmp18A;
struct Cyc_Absyn_Exp*_tmp18B;_LL219: if((int)_tmp15D != 0)goto _LL21B;_LL21A: s=Cyc_PP_text(
_tag_arr(";",sizeof(char),2));goto _LL218;_LL21B: if(_tmp15D <= (void*)1?1:*((int*)
_tmp15D)!= 0)goto _LL21D;_tmp15E=((struct Cyc_Absyn_Exp_s_struct*)_tmp15D)->f1;
_LL21C: s=({struct Cyc_PP_Doc*_tmp18C[2];_tmp18C[1]=Cyc_PP_text(_tag_arr(";",
sizeof(char),2));_tmp18C[0]=Cyc_Absynpp_exp2doc(_tmp15E);Cyc_PP_cat(_tag_arr(
_tmp18C,sizeof(struct Cyc_PP_Doc*),2));});goto _LL218;_LL21D: if(_tmp15D <= (void*)1?
1:*((int*)_tmp15D)!= 1)goto _LL21F;_tmp15F=((struct Cyc_Absyn_Seq_s_struct*)
_tmp15D)->f1;_tmp160=((struct Cyc_Absyn_Seq_s_struct*)_tmp15D)->f2;_LL21E: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp15F))s=({struct Cyc_PP_Doc*_tmp18D[6];_tmp18D[5]=
Cyc_Absynpp_is_declaration(_tmp160)?({struct Cyc_PP_Doc*_tmp18E[4];_tmp18E[3]=Cyc_PP_line_doc();
_tmp18E[2]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp18E[1]=Cyc_PP_nest(2,
Cyc_Absynpp_stmt2doc(_tmp160));_tmp18E[0]=Cyc_PP_text(_tag_arr("{ ",sizeof(char),
3));Cyc_PP_cat(_tag_arr(_tmp18E,sizeof(struct Cyc_PP_Doc*),4));}): Cyc_Absynpp_stmt2doc(
_tmp160);_tmp18D[4]=Cyc_PP_line_doc();_tmp18D[3]=Cyc_PP_text(_tag_arr("}",
sizeof(char),2));_tmp18D[2]=Cyc_PP_line_doc();_tmp18D[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp15F));_tmp18D[0]=Cyc_PP_text(_tag_arr("{ ",sizeof(char),3));Cyc_PP_cat(
_tag_arr(_tmp18D,sizeof(struct Cyc_PP_Doc*),6));});else{if(Cyc_Absynpp_is_declaration(
_tmp160))s=({struct Cyc_PP_Doc*_tmp18F[6];_tmp18F[5]=Cyc_PP_line_doc();_tmp18F[4]=
Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp18F[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp160));_tmp18F[2]=Cyc_PP_text(_tag_arr("{ ",sizeof(char),3));_tmp18F[1]=Cyc_PP_line_doc();
_tmp18F[0]=Cyc_Absynpp_stmt2doc(_tmp15F);Cyc_PP_cat(_tag_arr(_tmp18F,sizeof(
struct Cyc_PP_Doc*),6));});else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,_tag_arr("",sizeof(char),1),({struct Cyc_List_List*_tmp190=
_cycalloc(sizeof(*_tmp190));_tmp190->hd=_tmp15F;_tmp190->tl=({struct Cyc_List_List*
_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191->hd=_tmp160;_tmp191->tl=0;_tmp191;});
_tmp190;}));}}}else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,
_tag_arr("",sizeof(char),1),({struct Cyc_List_List*_tmp192=_cycalloc(sizeof(*
_tmp192));_tmp192->hd=_tmp15F;_tmp192->tl=({struct Cyc_List_List*_tmp193=
_cycalloc(sizeof(*_tmp193));_tmp193->hd=_tmp160;_tmp193->tl=0;_tmp193;});_tmp192;}));}
goto _LL218;_LL21F: if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)!= 2)goto _LL221;
_tmp161=((struct Cyc_Absyn_Return_s_struct*)_tmp15D)->f1;_LL220: if(_tmp161 == 0)s=
Cyc_PP_text(_tag_arr("return;",sizeof(char),8));else{s=({struct Cyc_PP_Doc*
_tmp194[3];_tmp194[2]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmp194[1]=
_tmp161 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmp161));_tmp194[0]=Cyc_PP_text(_tag_arr("return ",sizeof(char),8));
Cyc_PP_cat(_tag_arr(_tmp194,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL218;_LL221:
if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)!= 3)goto _LL223;_tmp162=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp15D)->f1;_tmp163=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp15D)->f2;_tmp164=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp15D)->f3;_LL222: {int print_else;{void*
_tmp195=(void*)_tmp164->r;_LL248: if((int)_tmp195 != 0)goto _LL24A;_LL249:
print_else=0;goto _LL247;_LL24A:;_LL24B: print_else=1;goto _LL247;_LL247:;}s=({
struct Cyc_PP_Doc*_tmp196[7];_tmp196[6]=print_else?({struct Cyc_PP_Doc*_tmp198[5];
_tmp198[4]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp198[3]=Cyc_PP_line_doc();
_tmp198[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp199[2];_tmp199[1]=Cyc_Absynpp_stmt2doc(
_tmp164);_tmp199[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp199,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp198[1]=Cyc_PP_text(_tag_arr("else {",sizeof(char),7));
_tmp198[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp198,sizeof(struct Cyc_PP_Doc*),
5));}): Cyc_PP_nil_doc();_tmp196[5]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp196[4]=Cyc_PP_line_doc();_tmp196[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp197[
2];_tmp197[1]=Cyc_Absynpp_stmt2doc(_tmp163);_tmp197[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp197,sizeof(struct Cyc_PP_Doc*),2));}));_tmp196[2]=Cyc_PP_text(
_tag_arr(") {",sizeof(char),4));_tmp196[1]=Cyc_Absynpp_exp2doc(_tmp162);_tmp196[
0]=Cyc_PP_text(_tag_arr("if (",sizeof(char),5));Cyc_PP_cat(_tag_arr(_tmp196,
sizeof(struct Cyc_PP_Doc*),7));});goto _LL218;}_LL223: if(_tmp15D <= (void*)1?1:*((
int*)_tmp15D)!= 4)goto _LL225;_tmp165=((struct Cyc_Absyn_While_s_struct*)_tmp15D)->f1;
_tmp166=_tmp165.f1;_tmp167=((struct Cyc_Absyn_While_s_struct*)_tmp15D)->f2;_LL224:
s=({struct Cyc_PP_Doc*_tmp19A[6];_tmp19A[5]=Cyc_PP_text(_tag_arr("}",sizeof(char),
2));_tmp19A[4]=Cyc_PP_line_doc();_tmp19A[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp19B[2];_tmp19B[1]=Cyc_Absynpp_stmt2doc(_tmp167);_tmp19B[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp19B,sizeof(struct Cyc_PP_Doc*),2));}));_tmp19A[2]=Cyc_PP_text(
_tag_arr(") {",sizeof(char),4));_tmp19A[1]=Cyc_Absynpp_exp2doc(_tmp166);_tmp19A[
0]=Cyc_PP_text(_tag_arr("while (",sizeof(char),8));Cyc_PP_cat(_tag_arr(_tmp19A,
sizeof(struct Cyc_PP_Doc*),6));});goto _LL218;_LL225: if(_tmp15D <= (void*)1?1:*((
int*)_tmp15D)!= 5)goto _LL227;_LL226: s=Cyc_PP_text(_tag_arr("break;",sizeof(char),
7));goto _LL218;_LL227: if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)!= 6)goto _LL229;
_LL228: s=Cyc_PP_text(_tag_arr("continue;",sizeof(char),10));goto _LL218;_LL229:
if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)!= 7)goto _LL22B;_tmp168=((struct Cyc_Absyn_Goto_s_struct*)
_tmp15D)->f1;_LL22A: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp19D;_tmp19D.tag=0;_tmp19D.f1=(struct _tagged_arr)*_tmp168;{void*_tmp19C[1]={&
_tmp19D};Cyc_Std_aprintf(_tag_arr("goto %s;",sizeof(char),9),_tag_arr(_tmp19C,
sizeof(void*),1));}}));goto _LL218;_LL22B: if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)
!= 8)goto _LL22D;_tmp169=((struct Cyc_Absyn_For_s_struct*)_tmp15D)->f1;_tmp16A=((
struct Cyc_Absyn_For_s_struct*)_tmp15D)->f2;_tmp16B=_tmp16A.f1;_tmp16C=((struct
Cyc_Absyn_For_s_struct*)_tmp15D)->f3;_tmp16D=_tmp16C.f1;_tmp16E=((struct Cyc_Absyn_For_s_struct*)
_tmp15D)->f4;_LL22C: s=({struct Cyc_PP_Doc*_tmp19E[10];_tmp19E[9]=Cyc_PP_text(
_tag_arr("}",sizeof(char),2));_tmp19E[8]=Cyc_PP_line_doc();_tmp19E[7]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp19F[2];_tmp19F[1]=Cyc_Absynpp_stmt2doc(_tmp16E);_tmp19F[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp19F,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp19E[6]=Cyc_PP_text(_tag_arr(") {",sizeof(char),4));_tmp19E[5]=Cyc_Absynpp_exp2doc(
_tmp16D);_tmp19E[4]=Cyc_PP_text(_tag_arr("; ",sizeof(char),3));_tmp19E[3]=Cyc_Absynpp_exp2doc(
_tmp16B);_tmp19E[2]=Cyc_PP_text(_tag_arr("; ",sizeof(char),3));_tmp19E[1]=Cyc_Absynpp_exp2doc(
_tmp169);_tmp19E[0]=Cyc_PP_text(_tag_arr("for(",sizeof(char),5));Cyc_PP_cat(
_tag_arr(_tmp19E,sizeof(struct Cyc_PP_Doc*),10));});goto _LL218;_LL22D: if(_tmp15D
<= (void*)1?1:*((int*)_tmp15D)!= 19)goto _LL22F;_tmp16F=((struct Cyc_Absyn_ForArray_s_struct*)
_tmp15D)->f1;_tmp170=_tmp16F.defns;_tmp171=_tmp16F.condition;_tmp172=_tmp171.f1;
_tmp173=_tmp16F.delta;_tmp174=_tmp173.f1;_tmp175=_tmp16F.body;_LL22E: s=({struct
Cyc_PP_Doc*_tmp1A0[9];_tmp1A0[8]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp1A0[7]=Cyc_PP_line_doc();_tmp1A0[6]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1A1[
2];_tmp1A1[1]=Cyc_Absynpp_stmt2doc(_tmp175);_tmp1A1[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp1A1,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1A0[5]=Cyc_PP_text(
_tag_arr(") {",sizeof(char),4));_tmp1A0[4]=Cyc_Absynpp_exp2doc(_tmp174);_tmp1A0[
3]=Cyc_PP_text(_tag_arr("; ",sizeof(char),3));_tmp1A0[2]=Cyc_Absynpp_exp2doc(
_tmp172);_tmp1A0[1]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_vardecl2doc,
_tag_arr(" ",sizeof(char),2),_tmp170);_tmp1A0[0]=Cyc_PP_text(_tag_arr("forarray(",
sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp1A0,sizeof(struct Cyc_PP_Doc*),9));});
goto _LL218;_LL22F: if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)!= 9)goto _LL231;
_tmp176=((struct Cyc_Absyn_Switch_s_struct*)_tmp15D)->f1;_tmp177=((struct Cyc_Absyn_Switch_s_struct*)
_tmp15D)->f2;_LL230: s=({struct Cyc_PP_Doc*_tmp1A2[7];_tmp1A2[6]=Cyc_PP_text(
_tag_arr("}",sizeof(char),2));_tmp1A2[5]=Cyc_PP_line_doc();_tmp1A2[4]=Cyc_Absynpp_switchclauses2doc(
_tmp177);_tmp1A2[3]=Cyc_PP_line_doc();_tmp1A2[2]=Cyc_PP_text(_tag_arr(") {",
sizeof(char),4));_tmp1A2[1]=Cyc_Absynpp_exp2doc(_tmp176);_tmp1A2[0]=Cyc_PP_text(
_tag_arr("switch (",sizeof(char),9));Cyc_PP_cat(_tag_arr(_tmp1A2,sizeof(struct
Cyc_PP_Doc*),7));});goto _LL218;_LL231: if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)!= 
10)goto _LL233;_tmp178=((struct Cyc_Absyn_SwitchC_s_struct*)_tmp15D)->f1;_tmp179=((
struct Cyc_Absyn_SwitchC_s_struct*)_tmp15D)->f2;_LL232: s=({struct Cyc_PP_Doc*
_tmp1A3[7];_tmp1A3[6]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp1A3[5]=Cyc_PP_line_doc();
_tmp1A3[4]=Cyc_Absynpp_switchCclauses2doc(_tmp179);_tmp1A3[3]=Cyc_PP_line_doc();
_tmp1A3[2]=Cyc_PP_text(_tag_arr(") {",sizeof(char),4));_tmp1A3[1]=Cyc_Absynpp_exp2doc(
_tmp178);_tmp1A3[0]=Cyc_PP_text(_tag_arr("switch \"C\" (",sizeof(char),13));Cyc_PP_cat(
_tag_arr(_tmp1A3,sizeof(struct Cyc_PP_Doc*),7));});goto _LL218;_LL233: if(_tmp15D <= (
void*)1?1:*((int*)_tmp15D)!= 11)goto _LL235;_tmp17A=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp15D)->f1;if(_tmp17A != 0)goto _LL235;_LL234: s=Cyc_PP_text(_tag_arr("fallthru;",
sizeof(char),10));goto _LL218;_LL235: if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)!= 
11)goto _LL237;_tmp17B=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp15D)->f1;_LL236: s=({
struct Cyc_PP_Doc*_tmp1A4[3];_tmp1A4[2]=Cyc_PP_text(_tag_arr(");",sizeof(char),3));
_tmp1A4[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp17B);_tmp1A4[0]=Cyc_PP_text(_tag_arr("fallthru(",
sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp1A4,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL218;_LL237: if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)!= 12)goto _LL239;
_tmp17C=((struct Cyc_Absyn_Decl_s_struct*)_tmp15D)->f1;_tmp17D=((struct Cyc_Absyn_Decl_s_struct*)
_tmp15D)->f2;_LL238: s=({struct Cyc_PP_Doc*_tmp1A5[3];_tmp1A5[2]=Cyc_Absynpp_stmt2doc(
_tmp17D);_tmp1A5[1]=Cyc_PP_line_doc();_tmp1A5[0]=Cyc_Absynpp_decl2doc(_tmp17C);
Cyc_PP_cat(_tag_arr(_tmp1A5,sizeof(struct Cyc_PP_Doc*),3));});goto _LL218;_LL239:
if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)!= 13)goto _LL23B;_tmp17E=((struct Cyc_Absyn_Cut_s_struct*)
_tmp15D)->f1;_LL23A: s=({struct Cyc_PP_Doc*_tmp1A6[2];_tmp1A6[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp17E));_tmp1A6[0]=Cyc_PP_text(_tag_arr("cut ",sizeof(char),5));Cyc_PP_cat(
_tag_arr(_tmp1A6,sizeof(struct Cyc_PP_Doc*),2));});goto _LL218;_LL23B: if(_tmp15D <= (
void*)1?1:*((int*)_tmp15D)!= 14)goto _LL23D;_tmp17F=((struct Cyc_Absyn_Splice_s_struct*)
_tmp15D)->f1;_LL23C: s=({struct Cyc_PP_Doc*_tmp1A7[2];_tmp1A7[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp17F));_tmp1A7[0]=Cyc_PP_text(_tag_arr("splice ",sizeof(char),8));Cyc_PP_cat(
_tag_arr(_tmp1A7,sizeof(struct Cyc_PP_Doc*),2));});goto _LL218;_LL23D: if(_tmp15D <= (
void*)1?1:*((int*)_tmp15D)!= 15)goto _LL23F;_tmp180=((struct Cyc_Absyn_Label_s_struct*)
_tmp15D)->f1;_tmp181=((struct Cyc_Absyn_Label_s_struct*)_tmp15D)->f2;_LL23E: if(
Cyc_Absynpp_decls_first?Cyc_Absynpp_is_declaration(_tmp181): 0)s=({struct Cyc_PP_Doc*
_tmp1A8[6];_tmp1A8[5]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp1A8[4]=Cyc_PP_line_doc();
_tmp1A8[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp181));_tmp1A8[2]=Cyc_PP_line_doc();
_tmp1A8[1]=Cyc_PP_text(_tag_arr(": {",sizeof(char),4));_tmp1A8[0]=Cyc_PP_textptr(
_tmp180);Cyc_PP_cat(_tag_arr(_tmp1A8,sizeof(struct Cyc_PP_Doc*),6));});else{s=({
struct Cyc_PP_Doc*_tmp1A9[3];_tmp1A9[2]=Cyc_Absynpp_stmt2doc(_tmp181);_tmp1A9[1]=
Cyc_PP_text(_tag_arr(": ",sizeof(char),3));_tmp1A9[0]=Cyc_PP_textptr(_tmp180);
Cyc_PP_cat(_tag_arr(_tmp1A9,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL218;_LL23F:
if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)!= 16)goto _LL241;_tmp182=((struct Cyc_Absyn_Do_s_struct*)
_tmp15D)->f1;_tmp183=((struct Cyc_Absyn_Do_s_struct*)_tmp15D)->f2;_tmp184=_tmp183.f1;
_LL240: s=({struct Cyc_PP_Doc*_tmp1AA[7];_tmp1AA[6]=Cyc_PP_text(_tag_arr(");",
sizeof(char),3));_tmp1AA[5]=Cyc_Absynpp_exp2doc(_tmp184);_tmp1AA[4]=Cyc_PP_text(
_tag_arr("} while (",sizeof(char),10));_tmp1AA[3]=Cyc_PP_line_doc();_tmp1AA[2]=
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp182));_tmp1AA[1]=Cyc_PP_line_doc();
_tmp1AA[0]=Cyc_PP_text(_tag_arr("do {",sizeof(char),5));Cyc_PP_cat(_tag_arr(
_tmp1AA,sizeof(struct Cyc_PP_Doc*),7));});goto _LL218;_LL241: if(_tmp15D <= (void*)1?
1:*((int*)_tmp15D)!= 17)goto _LL243;_tmp185=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp15D)->f1;_tmp186=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp15D)->f2;_LL242: s=({
struct Cyc_PP_Doc*_tmp1AB[9];_tmp1AB[8]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp1AB[7]=Cyc_PP_line_doc();_tmp1AB[6]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp186));_tmp1AB[5]=Cyc_PP_line_doc();_tmp1AB[4]=Cyc_PP_text(_tag_arr("} catch {",
sizeof(char),10));_tmp1AB[3]=Cyc_PP_line_doc();_tmp1AB[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp185));_tmp1AB[1]=Cyc_PP_line_doc();_tmp1AB[0]=Cyc_PP_text(_tag_arr("try {",
sizeof(char),6));Cyc_PP_cat(_tag_arr(_tmp1AB,sizeof(struct Cyc_PP_Doc*),9));});
goto _LL218;_LL243: if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)!= 18)goto _LL245;
_tmp187=((struct Cyc_Absyn_Region_s_struct*)_tmp15D)->f1;_tmp188=((struct Cyc_Absyn_Region_s_struct*)
_tmp15D)->f2;_tmp189=((struct Cyc_Absyn_Region_s_struct*)_tmp15D)->f3;_tmp18A=((
struct Cyc_Absyn_Region_s_struct*)_tmp15D)->f4;_LL244: s=({struct Cyc_PP_Doc*
_tmp1AC[11];_tmp1AC[10]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp1AC[9]=Cyc_PP_line_doc();
_tmp1AC[8]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp18A));_tmp1AC[7]=Cyc_PP_line_doc();
_tmp1AC[6]=Cyc_PP_text(_tag_arr("{",sizeof(char),2));_tmp1AC[5]=Cyc_Absynpp_qvar2doc(
_tmp188->name);_tmp1AC[4]=Cyc_PP_text(_tag_arr(">",sizeof(char),2));_tmp1AC[3]=
Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp187));_tmp1AC[2]=Cyc_PP_text(_tag_arr("<",
sizeof(char),2));_tmp1AC[1]=_tmp189?Cyc_PP_text(_tag_arr("",sizeof(char),1)): Cyc_PP_text(
_tag_arr("[resetable]",sizeof(char),12));_tmp1AC[0]=Cyc_PP_text(_tag_arr("region",
sizeof(char),7));Cyc_PP_cat(_tag_arr(_tmp1AC,sizeof(struct Cyc_PP_Doc*),11));});
goto _LL218;_LL245: if(_tmp15D <= (void*)1?1:*((int*)_tmp15D)!= 20)goto _LL218;
_tmp18B=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp15D)->f1;_LL246: s=({struct
Cyc_PP_Doc*_tmp1AD[3];_tmp1AD[2]=Cyc_PP_text(_tag_arr(");",sizeof(char),3));
_tmp1AD[1]=Cyc_Absynpp_exp2doc(_tmp18B);_tmp1AD[0]=Cyc_PP_text(_tag_arr("reset_region(",
sizeof(char),14));Cyc_PP_cat(_tag_arr(_tmp1AD,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL218;_LL218:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp1AE=(void*)p->r;void*_tmp1AF;int _tmp1B0;char
_tmp1B1;struct _tagged_arr _tmp1B2;struct Cyc_Absyn_Vardecl*_tmp1B3;struct Cyc_List_List*
_tmp1B4;struct Cyc_Absyn_Pat*_tmp1B5;struct Cyc_Absyn_Vardecl*_tmp1B6;struct
_tuple0*_tmp1B7;struct _tuple0*_tmp1B8;struct Cyc_List_List*_tmp1B9;struct Cyc_Absyn_AggrInfo
_tmp1BA;void*_tmp1BB;struct Cyc_List_List*_tmp1BC;struct Cyc_List_List*_tmp1BD;
struct Cyc_Absyn_Enumfield*_tmp1BE;struct Cyc_Absyn_Enumfield*_tmp1BF;struct Cyc_Absyn_Tunionfield*
_tmp1C0;struct Cyc_List_List*_tmp1C1;struct Cyc_Absyn_Tunionfield*_tmp1C2;struct
Cyc_List_List*_tmp1C3;_LL24D: if((int)_tmp1AE != 0)goto _LL24F;_LL24E: s=Cyc_PP_text(
_tag_arr("_",sizeof(char),2));goto _LL24C;_LL24F: if((int)_tmp1AE != 1)goto _LL251;
_LL250: s=Cyc_PP_text(_tag_arr("NULL",sizeof(char),5));goto _LL24C;_LL251: if(
_tmp1AE <= (void*)2?1:*((int*)_tmp1AE)!= 1)goto _LL253;_tmp1AF=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp1AE)->f1;_tmp1B0=((struct Cyc_Absyn_Int_p_struct*)_tmp1AE)->f2;_LL252: if(
_tmp1AF == (void*)0)s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp1C5;_tmp1C5.tag=1;_tmp1C5.f1=(int)((unsigned int)_tmp1B0);{void*_tmp1C4[1]={&
_tmp1C5};Cyc_Std_aprintf(_tag_arr("%d",sizeof(char),3),_tag_arr(_tmp1C4,sizeof(
void*),1));}}));else{s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp1C7;_tmp1C7.tag=1;_tmp1C7.f1=(unsigned int)_tmp1B0;{void*_tmp1C6[1]={&
_tmp1C7};Cyc_Std_aprintf(_tag_arr("%u",sizeof(char),3),_tag_arr(_tmp1C6,sizeof(
void*),1));}}));}goto _LL24C;_LL253: if(_tmp1AE <= (void*)2?1:*((int*)_tmp1AE)!= 2)
goto _LL255;_tmp1B1=((struct Cyc_Absyn_Char_p_struct*)_tmp1AE)->f1;_LL254: s=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp1C9;_tmp1C9.tag=0;_tmp1C9.f1=(
struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp1B1);{void*_tmp1C8[1]={& _tmp1C9};
Cyc_Std_aprintf(_tag_arr("'%s'",sizeof(char),5),_tag_arr(_tmp1C8,sizeof(void*),1));}}));
goto _LL24C;_LL255: if(_tmp1AE <= (void*)2?1:*((int*)_tmp1AE)!= 3)goto _LL257;
_tmp1B2=((struct Cyc_Absyn_Float_p_struct*)_tmp1AE)->f1;_LL256: s=Cyc_PP_text(
_tmp1B2);goto _LL24C;_LL257: if(_tmp1AE <= (void*)2?1:*((int*)_tmp1AE)!= 0)goto
_LL259;_tmp1B3=((struct Cyc_Absyn_Var_p_struct*)_tmp1AE)->f1;_LL258: s=Cyc_Absynpp_qvar2doc(
_tmp1B3->name);goto _LL24C;_LL259: if(_tmp1AE <= (void*)2?1:*((int*)_tmp1AE)!= 4)
goto _LL25B;_tmp1B4=((struct Cyc_Absyn_Tuple_p_struct*)_tmp1AE)->f1;_LL25A: s=({
struct Cyc_PP_Doc*_tmp1CA[3];_tmp1CA[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp1CA[1]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,
_tag_arr(",",sizeof(char),2),_tmp1B4);_tmp1CA[0]=Cyc_PP_text(_tag_arr("$(",
sizeof(char),3));Cyc_PP_cat(_tag_arr(_tmp1CA,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL24C;_LL25B: if(_tmp1AE <= (void*)2?1:*((int*)_tmp1AE)!= 5)goto _LL25D;
_tmp1B5=((struct Cyc_Absyn_Pointer_p_struct*)_tmp1AE)->f1;_LL25C: s=({struct Cyc_PP_Doc*
_tmp1CB[2];_tmp1CB[1]=Cyc_Absynpp_pat2doc(_tmp1B5);_tmp1CB[0]=Cyc_PP_text(
_tag_arr("&",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp1CB,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL24C;_LL25D: if(_tmp1AE <= (void*)2?1:*((int*)_tmp1AE)!= 6)goto _LL25F;
_tmp1B6=((struct Cyc_Absyn_Reference_p_struct*)_tmp1AE)->f1;_LL25E: s=({struct Cyc_PP_Doc*
_tmp1CC[2];_tmp1CC[1]=Cyc_Absynpp_qvar2doc(_tmp1B6->name);_tmp1CC[0]=Cyc_PP_text(
_tag_arr("*",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp1CC,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL24C;_LL25F: if(_tmp1AE <= (void*)2?1:*((int*)_tmp1AE)!= 11)goto
_LL261;_tmp1B7=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp1AE)->f1;_LL260: s=Cyc_Absynpp_qvar2doc(
_tmp1B7);goto _LL24C;_LL261: if(_tmp1AE <= (void*)2?1:*((int*)_tmp1AE)!= 12)goto
_LL263;_tmp1B8=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1AE)->f1;_tmp1B9=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1AE)->f2;_LL262: s=({struct Cyc_PP_Doc*
_tmp1CD[2];_tmp1CD[1]=Cyc_PP_group(_tag_arr("(",sizeof(char),2),_tag_arr(")",
sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp1B9));_tmp1CD[0]=Cyc_Absynpp_qvar2doc(_tmp1B8);Cyc_PP_cat(_tag_arr(_tmp1CD,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL24C;_LL263: if(_tmp1AE <= (void*)2?1:*((
int*)_tmp1AE)!= 7)goto _LL265;_tmp1BA=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1AE)->f1;
_tmp1BB=(void*)_tmp1BA.aggr_info;_tmp1BC=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp1AE)->f2;_tmp1BD=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1AE)->f3;_LL264: {
struct _tuple0*_tmp1CF;struct _tuple3 _tmp1CE=Cyc_Absyn_aggr_kinded_name(_tmp1BB);
_tmp1CF=_tmp1CE.f2;s=({struct Cyc_PP_Doc*_tmp1D0[4];_tmp1D0[3]=Cyc_PP_group(
_tag_arr("",sizeof(char),1),_tag_arr("}",sizeof(char),2),_tag_arr(",",sizeof(
char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp1BD));_tmp1D0[2]=Cyc_PP_egroup(
_tag_arr("[",sizeof(char),2),_tag_arr("]",sizeof(char),2),_tag_arr(",",sizeof(
char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_get_name,_tmp1BC)));_tmp1D0[1]=Cyc_PP_text(_tag_arr("{",sizeof(char),
2));_tmp1D0[0]=Cyc_Absynpp_qvar2doc(_tmp1CF);Cyc_PP_cat(_tag_arr(_tmp1D0,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL24C;}_LL265: if(_tmp1AE <= (void*)2?1:*((int*)
_tmp1AE)!= 9)goto _LL267;_tmp1BE=((struct Cyc_Absyn_Enum_p_struct*)_tmp1AE)->f2;
_LL266: _tmp1BF=_tmp1BE;goto _LL268;_LL267: if(_tmp1AE <= (void*)2?1:*((int*)_tmp1AE)
!= 10)goto _LL269;_tmp1BF=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp1AE)->f2;
_LL268: s=Cyc_Absynpp_qvar2doc(_tmp1BF->name);goto _LL24C;_LL269: if(_tmp1AE <= (
void*)2?1:*((int*)_tmp1AE)!= 8)goto _LL26B;_tmp1C0=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1AE)->f2;_tmp1C1=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1AE)->f3;if(_tmp1C1
!= 0)goto _LL26B;_LL26A: s=Cyc_Absynpp_qvar2doc(_tmp1C0->name);goto _LL24C;_LL26B:
if(_tmp1AE <= (void*)2?1:*((int*)_tmp1AE)!= 8)goto _LL24C;_tmp1C2=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1AE)->f2;_tmp1C3=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1AE)->f3;_LL26C: s=({
struct Cyc_PP_Doc*_tmp1D1[2];_tmp1D1[1]=Cyc_PP_egroup(_tag_arr("(",sizeof(char),2),
_tag_arr(")",sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp1C3));_tmp1D1[0]=Cyc_Absynpp_qvar2doc(_tmp1C2->name);Cyc_PP_cat(
_tag_arr(_tmp1D1,sizeof(struct Cyc_PP_Doc*),2));});goto _LL24C;_LL24C:;}return s;}
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple7*dp){return({struct Cyc_PP_Doc*
_tmp1D2[2];_tmp1D2[1]=Cyc_Absynpp_pat2doc((*dp).f2);_tmp1D2[0]=Cyc_PP_egroup(
_tag_arr("",sizeof(char),1),_tag_arr("=",sizeof(char),2),_tag_arr("=",sizeof(
char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));Cyc_PP_cat(_tag_arr(
_tmp1D2,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0?(void*)(c->pattern)->r == (
void*)0: 0)return({struct Cyc_PP_Doc*_tmp1D3[2];_tmp1D3[1]=Cyc_PP_nest(2,({struct
Cyc_PP_Doc*_tmp1D4[2];_tmp1D4[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1D4[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp1D4,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1D3[0]=Cyc_PP_text(
_tag_arr("default: ",sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp1D3,sizeof(struct
Cyc_PP_Doc*),2));});else{if(c->where_clause == 0)return({struct Cyc_PP_Doc*_tmp1D5[
4];_tmp1D5[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1D6[2];_tmp1D6[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp1D6[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1D6,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp1D5[2]=Cyc_PP_text(_tag_arr(": ",sizeof(char),3));
_tmp1D5[1]=Cyc_Absynpp_pat2doc(c->pattern);_tmp1D5[0]=Cyc_PP_text(_tag_arr("case ",
sizeof(char),6));Cyc_PP_cat(_tag_arr(_tmp1D5,sizeof(struct Cyc_PP_Doc*),4));});
else{return({struct Cyc_PP_Doc*_tmp1D7[6];_tmp1D7[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp1D8[2];_tmp1D8[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1D8[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp1D8,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1D7[4]=Cyc_PP_text(
_tag_arr(": ",sizeof(char),3));_tmp1D7[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(c->where_clause));_tmp1D7[2]=Cyc_PP_text(_tag_arr(" && ",sizeof(char),
5));_tmp1D7[1]=Cyc_Absynpp_pat2doc(c->pattern);_tmp1D7[0]=Cyc_PP_text(_tag_arr("case ",
sizeof(char),6));Cyc_PP_cat(_tag_arr(_tmp1D7,sizeof(struct Cyc_PP_Doc*),6));});}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,
_tag_arr("",sizeof(char),1),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause*c){struct Cyc_Absyn_SwitchC_clause _tmp1DA;struct
Cyc_Absyn_Exp*_tmp1DB;struct Cyc_Absyn_Stmt*_tmp1DC;struct Cyc_Absyn_SwitchC_clause*
_tmp1D9=c;_tmp1DA=*_tmp1D9;_tmp1DB=_tmp1DA.cnst_exp;_tmp1DC=_tmp1DA.body;if(
_tmp1DB == 0)return({struct Cyc_PP_Doc*_tmp1DD[2];_tmp1DD[1]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp1DE[2];_tmp1DE[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1DE[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1DE,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp1DD[0]=Cyc_PP_text(_tag_arr("default: ",sizeof(char),10));Cyc_PP_cat(
_tag_arr(_tmp1DD,sizeof(struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*
_tmp1DF[4];_tmp1DF[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1E0[2];_tmp1E0[1]=Cyc_Absynpp_stmt2doc(
_tmp1DC);_tmp1E0[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1E0,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp1DF[2]=Cyc_PP_text(_tag_arr(": ",sizeof(char),3));
_tmp1DF[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(_tmp1DB));
_tmp1DF[0]=Cyc_PP_text(_tag_arr("case ",sizeof(char),6));Cyc_PP_cat(_tag_arr(
_tmp1DF,sizeof(struct Cyc_PP_Doc*),4));});}}struct Cyc_PP_Doc*Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_SwitchC_clause*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_switchCclause2doc,_tag_arr("",sizeof(char),1),cs);}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(
f->name);else{return({struct Cyc_PP_Doc*_tmp1E1[3];_tmp1E1[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp1E1[1]=Cyc_PP_text(_tag_arr(" = ",
sizeof(char),4));_tmp1E1[0]=Cyc_Absynpp_qvar2doc(f->name);Cyc_PP_cat(_tag_arr(
_tmp1E1,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Enumfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_enumfield2doc,_tag_arr(",",sizeof(char),2),fs);}static struct Cyc_PP_Doc*
Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(v->name);}
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tag_arr(",",
sizeof(char),2),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd){struct Cyc_Absyn_Vardecl _tmp1E3;void*_tmp1E4;struct _tuple0*_tmp1E5;struct Cyc_Absyn_Tqual
_tmp1E6;void*_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_List_List*_tmp1E9;
struct Cyc_Absyn_Vardecl*_tmp1E2=vd;_tmp1E3=*_tmp1E2;_tmp1E4=(void*)_tmp1E3.sc;
_tmp1E5=_tmp1E3.name;_tmp1E6=_tmp1E3.tq;_tmp1E7=(void*)_tmp1E3.type;_tmp1E8=
_tmp1E3.initializer;_tmp1E9=_tmp1E3.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*
sn=Cyc_Absynpp_typedef_name2doc(_tmp1E5);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp1E9);struct Cyc_PP_Doc*beforenamedoc;if(!Cyc_Absynpp_to_VC)beforenamedoc=
attsdoc;else{void*_tmp1EA=Cyc_Tcutil_compress(_tmp1E7);struct Cyc_Absyn_FnInfo
_tmp1EB;struct Cyc_List_List*_tmp1EC;_LL26E: if(_tmp1EA <= (void*)3?1:*((int*)
_tmp1EA)!= 8)goto _LL270;_tmp1EB=((struct Cyc_Absyn_FnType_struct*)_tmp1EA)->f1;
_tmp1EC=_tmp1EB.attributes;_LL26F: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp1EC);
goto _LL26D;_LL270:;_LL271: beforenamedoc=Cyc_PP_nil_doc();goto _LL26D;_LL26D:;}s=({
struct Cyc_PP_Doc*_tmp1ED[5];_tmp1ED[4]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));
_tmp1ED[3]=_tmp1E8 == 0?Cyc_PP_text(_tag_arr("",sizeof(char),1)):({struct Cyc_PP_Doc*
_tmp1F0[2];_tmp1F0[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(
_tmp1E8));_tmp1F0[0]=Cyc_PP_text(_tag_arr(" = ",sizeof(char),4));Cyc_PP_cat(
_tag_arr(_tmp1F0,sizeof(struct Cyc_PP_Doc*),2));});_tmp1ED[2]=Cyc_Absynpp_tqtd2doc(
_tmp1E6,_tmp1E7,({struct Cyc_Core_Opt*_tmp1EE=_cycalloc(sizeof(*_tmp1EE));_tmp1EE->v=({
struct Cyc_PP_Doc*_tmp1EF[2];_tmp1EF[1]=sn;_tmp1EF[0]=beforenamedoc;Cyc_PP_cat(
_tag_arr(_tmp1EF,sizeof(struct Cyc_PP_Doc*),2));});_tmp1EE;}));_tmp1ED[1]=Cyc_Absynpp_scope2doc(
_tmp1E4);_tmp1ED[0]=Cyc_Absynpp_to_VC?attsdoc: Cyc_PP_nil_doc();Cyc_PP_cat(
_tag_arr(_tmp1ED,sizeof(struct Cyc_PP_Doc*),5));});return s;}}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp1F1=(void*)d->r;struct Cyc_Absyn_Fndecl*
_tmp1F2;struct Cyc_Absyn_Aggrdecl*_tmp1F3;struct Cyc_Absyn_Vardecl*_tmp1F4;struct
Cyc_Absyn_Tuniondecl*_tmp1F5;struct Cyc_Absyn_Tuniondecl _tmp1F6;void*_tmp1F7;
struct _tuple0*_tmp1F8;struct Cyc_List_List*_tmp1F9;struct Cyc_Core_Opt*_tmp1FA;int
_tmp1FB;struct Cyc_Absyn_Pat*_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FD;struct Cyc_List_List*
_tmp1FE;struct Cyc_Absyn_Enumdecl*_tmp1FF;struct Cyc_Absyn_Enumdecl _tmp200;void*
_tmp201;struct _tuple0*_tmp202;struct Cyc_Core_Opt*_tmp203;struct Cyc_Absyn_Typedefdecl*
_tmp204;struct _tagged_arr*_tmp205;struct Cyc_List_List*_tmp206;struct _tuple0*
_tmp207;struct Cyc_List_List*_tmp208;struct Cyc_List_List*_tmp209;_LL273: if(*((int*)
_tmp1F1)!= 1)goto _LL275;_tmp1F2=((struct Cyc_Absyn_Fn_d_struct*)_tmp1F1)->f1;
_LL274: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp211=_cycalloc(sizeof(*
_tmp211));_tmp211[0]=({struct Cyc_Absyn_FnType_struct _tmp212;_tmp212.tag=8;
_tmp212.f1=({struct Cyc_Absyn_FnInfo _tmp213;_tmp213.tvars=_tmp1F2->tvs;_tmp213.effect=
_tmp1F2->effect;_tmp213.ret_typ=(void*)((void*)_tmp1F2->ret_type);_tmp213.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple5*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp1F2->args);_tmp213.c_varargs=_tmp1F2->c_varargs;
_tmp213.cyc_varargs=_tmp1F2->cyc_varargs;_tmp213.rgn_po=_tmp1F2->rgn_po;_tmp213.attributes=
0;_tmp213;});_tmp212;});_tmp211;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp1F2->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp1F2->is_inline){if(Cyc_Absynpp_to_VC)
inlinedoc=Cyc_PP_text(_tag_arr("__inline ",sizeof(char),10));else{inlinedoc=Cyc_PP_text(
_tag_arr("inline ",sizeof(char),8));}}else{inlinedoc=Cyc_PP_nil_doc();}{struct
Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((void*)_tmp1F2->sc);struct Cyc_PP_Doc*
beforenamedoc=Cyc_Absynpp_to_VC?Cyc_Absynpp_callconv2doc(_tmp1F2->attributes):
attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp1F2->name);
struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual _tmp20E;
_tmp20E.q_const=0;_tmp20E.q_volatile=0;_tmp20E.q_restrict=0;_tmp20E;}),t,({
struct Cyc_Core_Opt*_tmp20F=_cycalloc(sizeof(*_tmp20F));_tmp20F->v=({struct Cyc_PP_Doc*
_tmp210[2];_tmp210[1]=namedoc;_tmp210[0]=beforenamedoc;Cyc_PP_cat(_tag_arr(
_tmp210,sizeof(struct Cyc_PP_Doc*),2));});_tmp20F;}));struct Cyc_PP_Doc*bodydoc=({
struct Cyc_PP_Doc*_tmp20C[4];_tmp20C[3]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp20C[2]=Cyc_PP_line_doc();_tmp20C[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp20D[
2];_tmp20D[1]=Cyc_Absynpp_stmt2doc(_tmp1F2->body);_tmp20D[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp20D,sizeof(struct Cyc_PP_Doc*),2));}));_tmp20C[0]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));Cyc_PP_cat(_tag_arr(_tmp20C,sizeof(struct Cyc_PP_Doc*),
4));});s=({struct Cyc_PP_Doc*_tmp20A[4];_tmp20A[3]=bodydoc;_tmp20A[2]=tqtddoc;
_tmp20A[1]=scopedoc;_tmp20A[0]=inlinedoc;Cyc_PP_cat(_tag_arr(_tmp20A,sizeof(
struct Cyc_PP_Doc*),4));});if(Cyc_Absynpp_to_VC)s=({struct Cyc_PP_Doc*_tmp20B[2];
_tmp20B[1]=s;_tmp20B[0]=attsdoc;Cyc_PP_cat(_tag_arr(_tmp20B,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL272;}}_LL275: if(*((int*)_tmp1F1)!= 4)goto _LL277;_tmp1F3=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp1F1)->f1;_LL276: if(_tmp1F3->fields == 0)s=({struct
Cyc_PP_Doc*_tmp214[5];_tmp214[4]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));
_tmp214[3]=Cyc_Absynpp_ktvars2doc(_tmp1F3->tvs);_tmp214[2]=Cyc_Absynpp_qvar2doc(
_tmp1F3->name);_tmp214[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp1F3->kind);
_tmp214[0]=Cyc_Absynpp_scope2doc((void*)_tmp1F3->sc);Cyc_PP_cat(_tag_arr(_tmp214,
sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp215[10];_tmp215[9]=
Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmp215[8]=Cyc_Absynpp_atts2doc(
_tmp1F3->attributes);_tmp215[7]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp215[6]=Cyc_PP_line_doc();_tmp215[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp216[
2];_tmp216[1]=Cyc_Absynpp_aggrfields2doc((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1F3->fields))->v);_tmp216[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp216,sizeof(struct Cyc_PP_Doc*),2));}));_tmp215[4]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));_tmp215[3]=Cyc_Absynpp_ktvars2doc(_tmp1F3->tvs);
_tmp215[2]=Cyc_Absynpp_qvar2doc(_tmp1F3->name);_tmp215[1]=Cyc_Absynpp_aggr_kind2doc((
void*)_tmp1F3->kind);_tmp215[0]=Cyc_Absynpp_scope2doc((void*)_tmp1F3->sc);Cyc_PP_cat(
_tag_arr(_tmp215,sizeof(struct Cyc_PP_Doc*),10));});}goto _LL272;_LL277: if(*((int*)
_tmp1F1)!= 0)goto _LL279;_tmp1F4=((struct Cyc_Absyn_Var_d_struct*)_tmp1F1)->f1;
_LL278: s=Cyc_Absynpp_vardecl2doc(_tmp1F4);goto _LL272;_LL279: if(*((int*)_tmp1F1)
!= 5)goto _LL27B;_tmp1F5=((struct Cyc_Absyn_Tunion_d_struct*)_tmp1F1)->f1;_tmp1F6=*
_tmp1F5;_tmp1F7=(void*)_tmp1F6.sc;_tmp1F8=_tmp1F6.name;_tmp1F9=_tmp1F6.tvs;
_tmp1FA=_tmp1F6.fields;_tmp1FB=_tmp1F6.is_xtunion;_LL27A: if(_tmp1FA == 0)s=({
struct Cyc_PP_Doc*_tmp217[5];_tmp217[4]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));
_tmp217[3]=Cyc_Absynpp_ktvars2doc(_tmp1F9);_tmp217[2]=_tmp1FB?Cyc_Absynpp_qvar2doc(
_tmp1F8): Cyc_Absynpp_typedef_name2doc(_tmp1F8);_tmp217[1]=_tmp1FB?Cyc_PP_text(
_tag_arr("xtunion ",sizeof(char),9)): Cyc_PP_text(_tag_arr("tunion ",sizeof(char),
8));_tmp217[0]=Cyc_Absynpp_scope2doc(_tmp1F7);Cyc_PP_cat(_tag_arr(_tmp217,
sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp218[8];_tmp218[7]=
Cyc_PP_text(_tag_arr("};",sizeof(char),3));_tmp218[6]=Cyc_PP_line_doc();_tmp218[
5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp219[2];_tmp219[1]=Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*)_tmp1FA->v);_tmp219[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp219,sizeof(struct Cyc_PP_Doc*),2));}));_tmp218[4]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));_tmp218[3]=Cyc_Absynpp_ktvars2doc(_tmp1F9);
_tmp218[2]=_tmp1FB?Cyc_Absynpp_qvar2doc(_tmp1F8): Cyc_Absynpp_typedef_name2doc(
_tmp1F8);_tmp218[1]=_tmp1FB?Cyc_PP_text(_tag_arr("xtunion ",sizeof(char),9)): Cyc_PP_text(
_tag_arr("tunion ",sizeof(char),8));_tmp218[0]=Cyc_Absynpp_scope2doc(_tmp1F7);
Cyc_PP_cat(_tag_arr(_tmp218,sizeof(struct Cyc_PP_Doc*),8));});}goto _LL272;_LL27B:
if(*((int*)_tmp1F1)!= 2)goto _LL27D;_tmp1FC=((struct Cyc_Absyn_Let_d_struct*)
_tmp1F1)->f1;_tmp1FD=((struct Cyc_Absyn_Let_d_struct*)_tmp1F1)->f4;_LL27C: s=({
struct Cyc_PP_Doc*_tmp21A[5];_tmp21A[4]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));
_tmp21A[3]=Cyc_Absynpp_exp2doc(_tmp1FD);_tmp21A[2]=Cyc_PP_text(_tag_arr(" = ",
sizeof(char),4));_tmp21A[1]=Cyc_Absynpp_pat2doc(_tmp1FC);_tmp21A[0]=Cyc_PP_text(
_tag_arr("let ",sizeof(char),5));Cyc_PP_cat(_tag_arr(_tmp21A,sizeof(struct Cyc_PP_Doc*),
5));});goto _LL272;_LL27D: if(*((int*)_tmp1F1)!= 3)goto _LL27F;_tmp1FE=((struct Cyc_Absyn_Letv_d_struct*)
_tmp1F1)->f1;_LL27E: s=({struct Cyc_PP_Doc*_tmp21B[3];_tmp21B[2]=Cyc_PP_text(
_tag_arr(";",sizeof(char),2));_tmp21B[1]=Cyc_Absynpp_ids2doc(_tmp1FE);_tmp21B[0]=
Cyc_PP_text(_tag_arr("let ",sizeof(char),5));Cyc_PP_cat(_tag_arr(_tmp21B,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL272;_LL27F: if(*((int*)_tmp1F1)!= 6)goto _LL281;
_tmp1FF=((struct Cyc_Absyn_Enum_d_struct*)_tmp1F1)->f1;_tmp200=*_tmp1FF;_tmp201=(
void*)_tmp200.sc;_tmp202=_tmp200.name;_tmp203=_tmp200.fields;_LL280: if(_tmp203 == 
0)s=({struct Cyc_PP_Doc*_tmp21C[4];_tmp21C[3]=Cyc_PP_text(_tag_arr(";",sizeof(
char),2));_tmp21C[2]=Cyc_Absynpp_typedef_name2doc(_tmp202);_tmp21C[1]=Cyc_PP_text(
_tag_arr("enum ",sizeof(char),6));_tmp21C[0]=Cyc_Absynpp_scope2doc(_tmp201);Cyc_PP_cat(
_tag_arr(_tmp21C,sizeof(struct Cyc_PP_Doc*),4));});else{s=({struct Cyc_PP_Doc*
_tmp21D[7];_tmp21D[6]=Cyc_PP_text(_tag_arr("};",sizeof(char),3));_tmp21D[5]=Cyc_PP_line_doc();
_tmp21D[4]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp21E[2];_tmp21E[1]=Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*)_tmp203->v);_tmp21E[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp21E,sizeof(struct Cyc_PP_Doc*),2));}));_tmp21D[3]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));_tmp21D[2]=Cyc_Absynpp_qvar2doc(_tmp202);_tmp21D[
1]=Cyc_PP_text(_tag_arr("enum ",sizeof(char),6));_tmp21D[0]=Cyc_Absynpp_scope2doc(
_tmp201);Cyc_PP_cat(_tag_arr(_tmp21D,sizeof(struct Cyc_PP_Doc*),7));});}goto
_LL272;_LL281: if(*((int*)_tmp1F1)!= 7)goto _LL283;_tmp204=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp1F1)->f1;_LL282: {void*t;if(_tmp204->defn != 0)t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp204->defn))->v;else{t=Cyc_Absyn_new_evar(_tmp204->kind,0);}s=({
struct Cyc_PP_Doc*_tmp21F[3];_tmp21F[2]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));
_tmp21F[1]=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual _tmp220;_tmp220.q_const=0;
_tmp220.q_volatile=0;_tmp220.q_restrict=0;_tmp220;}),t,({struct Cyc_Core_Opt*
_tmp221=_cycalloc(sizeof(*_tmp221));_tmp221->v=({struct Cyc_PP_Doc*_tmp222[2];
_tmp222[1]=Cyc_Absynpp_tvars2doc(_tmp204->tvs);_tmp222[0]=Cyc_Absynpp_typedef_name2doc(
_tmp204->name);Cyc_PP_cat(_tag_arr(_tmp222,sizeof(struct Cyc_PP_Doc*),2));});
_tmp221;}));_tmp21F[0]=Cyc_PP_text(_tag_arr("typedef ",sizeof(char),9));Cyc_PP_cat(
_tag_arr(_tmp21F,sizeof(struct Cyc_PP_Doc*),3));});goto _LL272;}_LL283: if(*((int*)
_tmp1F1)!= 8)goto _LL285;_tmp205=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1F1)->f1;
_tmp206=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1F1)->f2;_LL284: if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_add(_tmp205);s=({struct Cyc_PP_Doc*_tmp223[7];_tmp223[
6]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp223[5]=Cyc_PP_line_doc();
_tmp223[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(char),1),_tmp206);_tmp223[3]=Cyc_PP_line_doc();_tmp223[2]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));_tmp223[1]=Cyc_PP_textptr(_tmp205);_tmp223[0]=Cyc_PP_text(
_tag_arr("namespace ",sizeof(char),11));Cyc_PP_cat(_tag_arr(_tmp223,sizeof(
struct Cyc_PP_Doc*),7));});if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL272;_LL285: if(*((int*)_tmp1F1)!= 9)goto _LL287;_tmp207=((struct Cyc_Absyn_Using_d_struct*)
_tmp1F1)->f1;_tmp208=((struct Cyc_Absyn_Using_d_struct*)_tmp1F1)->f2;_LL286: if(
Cyc_Absynpp_print_using_stmts)s=({struct Cyc_PP_Doc*_tmp224[7];_tmp224[6]=Cyc_PP_text(
_tag_arr("}",sizeof(char),2));_tmp224[5]=Cyc_PP_line_doc();_tmp224[4]=((struct
Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr
sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tag_arr("",
sizeof(char),1),_tmp208);_tmp224[3]=Cyc_PP_line_doc();_tmp224[2]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));_tmp224[1]=Cyc_Absynpp_qvar2doc(_tmp207);_tmp224[
0]=Cyc_PP_text(_tag_arr("using ",sizeof(char),7));Cyc_PP_cat(_tag_arr(_tmp224,
sizeof(struct Cyc_PP_Doc*),7));});else{s=({struct Cyc_PP_Doc*_tmp225[7];_tmp225[6]=
Cyc_PP_text(_tag_arr("/* } */",sizeof(char),8));_tmp225[5]=Cyc_PP_line_doc();
_tmp225[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(char),1),_tmp208);_tmp225[3]=Cyc_PP_line_doc();_tmp225[2]=Cyc_PP_text(
_tag_arr(" { */",sizeof(char),6));_tmp225[1]=Cyc_Absynpp_qvar2doc(_tmp207);
_tmp225[0]=Cyc_PP_text(_tag_arr("/* using ",sizeof(char),10));Cyc_PP_cat(
_tag_arr(_tmp225,sizeof(struct Cyc_PP_Doc*),7));});}goto _LL272;_LL287: if(*((int*)
_tmp1F1)!= 10)goto _LL272;_tmp209=((struct Cyc_Absyn_ExternC_d_struct*)_tmp1F1)->f1;
_LL288: if(Cyc_Absynpp_print_externC_stmts)s=({struct Cyc_PP_Doc*_tmp226[5];
_tmp226[4]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp226[3]=Cyc_PP_line_doc();
_tmp226[2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(char),1),_tmp209);_tmp226[1]=Cyc_PP_line_doc();_tmp226[0]=Cyc_PP_text(
_tag_arr("extern \"C\" {",sizeof(char),13));Cyc_PP_cat(_tag_arr(_tmp226,sizeof(
struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp227[5];_tmp227[4]=Cyc_PP_text(
_tag_arr("/* } */",sizeof(char),8));_tmp227[3]=Cyc_PP_line_doc();_tmp227[2]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(char),1),_tmp209);_tmp227[1]=Cyc_PP_line_doc();_tmp227[0]=Cyc_PP_text(
_tag_arr("/* extern \"C\" { */",sizeof(char),19));Cyc_PP_cat(_tag_arr(_tmp227,
sizeof(struct Cyc_PP_Doc*),5));});}goto _LL272;_LL272:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(void*sc){void*_tmp228=sc;_LL28A: if((int)_tmp228 != 0)goto
_LL28C;_LL28B: return Cyc_PP_text(_tag_arr("static ",sizeof(char),8));_LL28C: if((
int)_tmp228 != 2)goto _LL28E;_LL28D: return Cyc_PP_nil_doc();_LL28E: if((int)_tmp228
!= 3)goto _LL290;_LL28F: return Cyc_PP_text(_tag_arr("extern ",sizeof(char),8));
_LL290: if((int)_tmp228 != 4)goto _LL292;_LL291: return Cyc_PP_text(_tag_arr("extern \"C\" ",
sizeof(char),12));_LL292: if((int)_tmp228 != 1)goto _LL289;_LL293: return Cyc_PP_text(
_tag_arr("abstract ",sizeof(char),10));_LL289:;}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp229=t;struct Cyc_Absyn_Tvar*_tmp22A;struct Cyc_List_List*_tmp22B;
_LL295: if(_tmp229 <= (void*)3?1:*((int*)_tmp229)!= 1)goto _LL297;_tmp22A=((struct
Cyc_Absyn_VarType_struct*)_tmp229)->f1;_LL296: return Cyc_Tcutil_is_temp_tvar(
_tmp22A);_LL297: if(_tmp229 <= (void*)3?1:*((int*)_tmp229)!= 18)goto _LL299;_tmp22B=((
struct Cyc_Absyn_JoinEff_struct*)_tmp229)->f1;_LL298: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp22B);_LL299:;_LL29A: return 0;_LL294:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp22C=t;struct Cyc_Core_Opt*_tmp22D;struct Cyc_Core_Opt _tmp22E;void*_tmp22F;
_LL29C: if(_tmp22C <= (void*)3?1:*((int*)_tmp22C)!= 11)goto _LL29E;_LL29D: return 1;
_LL29E: if(_tmp22C <= (void*)3?1:*((int*)_tmp22C)!= 16)goto _LL2A0;_tmp22D=((struct
Cyc_Absyn_TypedefType_struct*)_tmp22C)->f4;if(_tmp22D == 0)goto _LL2A0;_tmp22E=*
_tmp22D;_tmp22F=(void*)_tmp22E.v;_LL29F: return Cyc_Absynpp_is_anon_aggrtype(
_tmp22F);_LL2A0:;_LL2A1: return 0;_LL29B:;}struct _tuple6 Cyc_Absynpp_to_tms(struct
Cyc_Absyn_Tqual tq,void*t){void*_tmp230=t;void*_tmp231;struct Cyc_Absyn_Tqual
_tmp232;struct Cyc_Absyn_Exp*_tmp233;struct Cyc_Absyn_PtrInfo _tmp234;void*_tmp235;
void*_tmp236;struct Cyc_Absyn_Conref*_tmp237;struct Cyc_Absyn_Tqual _tmp238;struct
Cyc_Absyn_Conref*_tmp239;struct Cyc_Absyn_FnInfo _tmp23A;struct Cyc_List_List*
_tmp23B;struct Cyc_Core_Opt*_tmp23C;void*_tmp23D;struct Cyc_List_List*_tmp23E;int
_tmp23F;struct Cyc_Absyn_VarargInfo*_tmp240;struct Cyc_List_List*_tmp241;struct Cyc_List_List*
_tmp242;struct Cyc_Core_Opt*_tmp243;struct Cyc_Core_Opt*_tmp244;int _tmp245;struct
_tuple0*_tmp246;struct Cyc_List_List*_tmp247;struct Cyc_Core_Opt*_tmp248;_LL2A3:
if(_tmp230 <= (void*)3?1:*((int*)_tmp230)!= 7)goto _LL2A5;_tmp231=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp230)->f1;_tmp232=((struct Cyc_Absyn_ArrayType_struct*)
_tmp230)->f2;_tmp233=((struct Cyc_Absyn_ArrayType_struct*)_tmp230)->f3;_LL2A4: {
struct Cyc_Absyn_Tqual _tmp24A;void*_tmp24B;struct Cyc_List_List*_tmp24C;struct
_tuple6 _tmp249=Cyc_Absynpp_to_tms(_tmp232,_tmp231);_tmp24A=_tmp249.f1;_tmp24B=
_tmp249.f2;_tmp24C=_tmp249.f3;{void*tm;if(_tmp233 == 0)tm=(void*)0;else{tm=(void*)({
struct Cyc_Absyn_ConstArray_mod_struct*_tmp24D=_cycalloc(sizeof(*_tmp24D));
_tmp24D[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp24E;_tmp24E.tag=0;_tmp24E.f1=(
struct Cyc_Absyn_Exp*)_check_null(_tmp233);_tmp24E;});_tmp24D;});}return({struct
_tuple6 _tmp24F;_tmp24F.f1=_tmp24A;_tmp24F.f2=_tmp24B;_tmp24F.f3=({struct Cyc_List_List*
_tmp250=_cycalloc(sizeof(*_tmp250));_tmp250->hd=(void*)tm;_tmp250->tl=_tmp24C;
_tmp250;});_tmp24F;});}}_LL2A5: if(_tmp230 <= (void*)3?1:*((int*)_tmp230)!= 4)goto
_LL2A7;_tmp234=((struct Cyc_Absyn_PointerType_struct*)_tmp230)->f1;_tmp235=(void*)
_tmp234.elt_typ;_tmp236=(void*)_tmp234.rgn_typ;_tmp237=_tmp234.nullable;_tmp238=
_tmp234.tq;_tmp239=_tmp234.bounds;_LL2A6: {struct Cyc_Absyn_Tqual _tmp252;void*
_tmp253;struct Cyc_List_List*_tmp254;struct _tuple6 _tmp251=Cyc_Absynpp_to_tms(
_tmp238,_tmp235);_tmp252=_tmp251.f1;_tmp253=_tmp251.f2;_tmp254=_tmp251.f3;{void*
ps;{struct _tuple4 _tmp256=({struct _tuple4 _tmp255;_tmp255.f1=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp237))->v;_tmp255.f2=(
void*)(Cyc_Absyn_compress_conref(_tmp239))->v;_tmp255;});void*_tmp257;int _tmp258;
void*_tmp259;void*_tmp25A;struct Cyc_Absyn_Exp*_tmp25B;void*_tmp25C;int _tmp25D;
void*_tmp25E;void*_tmp25F;struct Cyc_Absyn_Exp*_tmp260;void*_tmp261;void*_tmp262;
_LL2B0: _tmp257=_tmp256.f1;if(_tmp257 <= (void*)1?1:*((int*)_tmp257)!= 0)goto
_LL2B2;_tmp258=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp257)->f1;if(_tmp258
!= 1)goto _LL2B2;_tmp259=_tmp256.f2;if(_tmp259 <= (void*)1?1:*((int*)_tmp259)!= 0)
goto _LL2B2;_tmp25A=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp259)->f1;if(
_tmp25A <= (void*)1?1:*((int*)_tmp25A)!= 0)goto _LL2B2;_tmp25B=((struct Cyc_Absyn_Upper_b_struct*)
_tmp25A)->f1;_LL2B1: ps=(void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp263=
_cycalloc(sizeof(*_tmp263));_tmp263[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp264;_tmp264.tag=1;_tmp264.f1=_tmp25B;_tmp264;});_tmp263;});goto _LL2AF;_LL2B2:
_tmp25C=_tmp256.f1;if(_tmp25C <= (void*)1?1:*((int*)_tmp25C)!= 0)goto _LL2B4;
_tmp25D=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp25C)->f1;if(_tmp25D != 0)
goto _LL2B4;_tmp25E=_tmp256.f2;if(_tmp25E <= (void*)1?1:*((int*)_tmp25E)!= 0)goto
_LL2B4;_tmp25F=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp25E)->f1;if(
_tmp25F <= (void*)1?1:*((int*)_tmp25F)!= 0)goto _LL2B4;_tmp260=((struct Cyc_Absyn_Upper_b_struct*)
_tmp25F)->f1;_LL2B3: ps=(void*)({struct Cyc_Absyn_NonNullable_ps_struct*_tmp265=
_cycalloc(sizeof(*_tmp265));_tmp265[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp266;_tmp266.tag=0;_tmp266.f1=_tmp260;_tmp266;});_tmp265;});goto _LL2AF;_LL2B4:
_tmp261=_tmp256.f2;if(_tmp261 <= (void*)1?1:*((int*)_tmp261)!= 0)goto _LL2B6;
_tmp262=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp261)->f1;if((int)_tmp262
!= 0)goto _LL2B6;_LL2B5: ps=(void*)0;goto _LL2AF;_LL2B6:;_LL2B7: ps=(void*)0;goto
_LL2AF;_LL2AF:;}{void*tm=(void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp269=
_cycalloc(sizeof(*_tmp269));_tmp269[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp26A;_tmp26A.tag=1;_tmp26A.f1=(void*)ps;_tmp26A.f2=(void*)_tmp236;_tmp26A.f3=
tq;_tmp26A;});_tmp269;});return({struct _tuple6 _tmp267;_tmp267.f1=_tmp252;_tmp267.f2=
_tmp253;_tmp267.f3=({struct Cyc_List_List*_tmp268=_cycalloc(sizeof(*_tmp268));
_tmp268->hd=(void*)tm;_tmp268->tl=_tmp254;_tmp268;});_tmp267;});}}}_LL2A7: if(
_tmp230 <= (void*)3?1:*((int*)_tmp230)!= 8)goto _LL2A9;_tmp23A=((struct Cyc_Absyn_FnType_struct*)
_tmp230)->f1;_tmp23B=_tmp23A.tvars;_tmp23C=_tmp23A.effect;_tmp23D=(void*)_tmp23A.ret_typ;
_tmp23E=_tmp23A.args;_tmp23F=_tmp23A.c_varargs;_tmp240=_tmp23A.cyc_varargs;
_tmp241=_tmp23A.rgn_po;_tmp242=_tmp23A.attributes;_LL2A8: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp23C == 0?1: Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp23C->v)){
_tmp23C=0;_tmp23B=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp23B);}{struct Cyc_Absyn_Tqual _tmp26C;void*_tmp26D;struct Cyc_List_List*_tmp26E;
struct _tuple6 _tmp26B=Cyc_Absynpp_to_tms(Cyc_Absyn_empty_tqual(),_tmp23D);_tmp26C=
_tmp26B.f1;_tmp26D=_tmp26B.f2;_tmp26E=_tmp26B.f3;{struct Cyc_List_List*tms=
_tmp26E;if(_tmp242 != 0?!Cyc_Absynpp_to_VC: 0)tms=({struct Cyc_List_List*_tmp26F=
_cycalloc(sizeof(*_tmp26F));_tmp26F->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp271;_tmp271.tag=4;_tmp271.f1=0;_tmp271.f2=_tmp242;_tmp271;});_tmp270;}));
_tmp26F->tl=tms;_tmp26F;});tms=({struct Cyc_List_List*_tmp272=_cycalloc(sizeof(*
_tmp272));_tmp272->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp274;_tmp274.tag=2;_tmp274.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp276;_tmp276.tag=1;_tmp276.f1=_tmp23E;_tmp276.f2=_tmp23F;_tmp276.f3=_tmp240;
_tmp276.f4=_tmp23C;_tmp276.f5=_tmp241;_tmp276;});_tmp275;}));_tmp274;});_tmp273;}));
_tmp272->tl=tms;_tmp272;});if(Cyc_Absynpp_to_VC)for(0;_tmp242 != 0;_tmp242=
_tmp242->tl){void*_tmp277=(void*)_tmp242->hd;_LL2B9: if((int)_tmp277 != 0)goto
_LL2BB;_LL2BA: goto _LL2BC;_LL2BB: if((int)_tmp277 != 1)goto _LL2BD;_LL2BC: goto _LL2BE;
_LL2BD: if((int)_tmp277 != 2)goto _LL2BF;_LL2BE: tms=({struct Cyc_List_List*_tmp278=
_cycalloc(sizeof(*_tmp278));_tmp278->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp27A;_tmp27A.tag=4;_tmp27A.f1=0;_tmp27A.f2=({struct Cyc_List_List*_tmp27B=
_cycalloc(sizeof(*_tmp27B));_tmp27B->hd=(void*)((void*)_tmp242->hd);_tmp27B->tl=
0;_tmp27B;});_tmp27A;});_tmp279;}));_tmp278->tl=tms;_tmp278;});goto AfterAtts;
_LL2BF:;_LL2C0: goto _LL2B8;_LL2B8:;}AfterAtts: if(_tmp23B != 0)tms=({struct Cyc_List_List*
_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp27D=_cycalloc(sizeof(*_tmp27D));_tmp27D[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp27E;_tmp27E.tag=3;_tmp27E.f1=_tmp23B;_tmp27E.f2=0;_tmp27E.f3=1;_tmp27E;});
_tmp27D;}));_tmp27C->tl=tms;_tmp27C;});return({struct _tuple6 _tmp27F;_tmp27F.f1=
_tmp26C;_tmp27F.f2=_tmp26D;_tmp27F.f3=tms;_tmp27F;});}}_LL2A9: if(_tmp230 <= (void*)
3?1:*((int*)_tmp230)!= 0)goto _LL2AB;_tmp243=((struct Cyc_Absyn_Evar_struct*)
_tmp230)->f1;_tmp244=((struct Cyc_Absyn_Evar_struct*)_tmp230)->f2;_tmp245=((
struct Cyc_Absyn_Evar_struct*)_tmp230)->f3;_LL2AA: if(_tmp244 == 0)return({struct
_tuple6 _tmp280;_tmp280.f1=tq;_tmp280.f2=t;_tmp280.f3=0;_tmp280;});else{return Cyc_Absynpp_to_tms(
tq,(void*)_tmp244->v);}_LL2AB: if(_tmp230 <= (void*)3?1:*((int*)_tmp230)!= 16)goto
_LL2AD;_tmp246=((struct Cyc_Absyn_TypedefType_struct*)_tmp230)->f1;_tmp247=((
struct Cyc_Absyn_TypedefType_struct*)_tmp230)->f2;_tmp248=((struct Cyc_Absyn_TypedefType_struct*)
_tmp230)->f4;_LL2AC: if((_tmp248 == 0?1: !Cyc_Absynpp_expand_typedefs)?1: Cyc_Absynpp_is_anon_aggrtype((
void*)_tmp248->v))return({struct _tuple6 _tmp281;_tmp281.f1=tq;_tmp281.f2=t;
_tmp281.f3=0;_tmp281;});else{return Cyc_Absynpp_to_tms(tq,(void*)_tmp248->v);}
_LL2AD:;_LL2AE: return({struct _tuple6 _tmp282;_tmp282.f1=tq;_tmp282.f2=t;_tmp282.f3=
0;_tmp282;});_LL2A2:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_Core_Opt*dopt){struct Cyc_Absyn_Tqual _tmp284;void*_tmp285;
struct Cyc_List_List*_tmp286;struct _tuple6 _tmp283=Cyc_Absynpp_to_tms(tq,t);
_tmp284=_tmp283.f1;_tmp285=_tmp283.f2;_tmp286=_tmp283.f3;_tmp286=Cyc_List_imp_rev(
_tmp286);if(_tmp286 == 0?dopt == 0: 0)return({struct Cyc_PP_Doc*_tmp287[2];_tmp287[1]=
Cyc_Absynpp_ntyp2doc(_tmp285);_tmp287[0]=Cyc_Absynpp_tqual2doc(_tmp284);Cyc_PP_cat(
_tag_arr(_tmp287,sizeof(struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*
_tmp288[4];_tmp288[3]=Cyc_Absynpp_dtms2doc(dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)
dopt->v,_tmp286);_tmp288[2]=Cyc_PP_text(_tag_arr(" ",sizeof(char),2));_tmp288[1]=
Cyc_Absynpp_ntyp2doc(_tmp285);_tmp288[0]=Cyc_Absynpp_tqual2doc(_tmp284);Cyc_PP_cat(
_tag_arr(_tmp288,sizeof(struct Cyc_PP_Doc*),4));});}}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(
struct Cyc_Absyn_Aggrfield*f){if(f->width != 0)return({struct Cyc_PP_Doc*_tmp289[5];
_tmp289[4]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmp289[3]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp289[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(
f->width));_tmp289[1]=Cyc_PP_text(_tag_arr(":",sizeof(char),2));_tmp289[0]=Cyc_Absynpp_tqtd2doc(
f->tq,(void*)f->type,({struct Cyc_Core_Opt*_tmp28A=_cycalloc(sizeof(*_tmp28A));
_tmp28A->v=Cyc_PP_textptr(f->name);_tmp28A;}));Cyc_PP_cat(_tag_arr(_tmp289,
sizeof(struct Cyc_PP_Doc*),5));});else{return({struct Cyc_PP_Doc*_tmp28B[3];
_tmp28B[2]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmp28B[1]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp28B[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp28C=_cycalloc(sizeof(*_tmp28C));_tmp28C->v=Cyc_PP_textptr(f->name);_tmp28C;}));
Cyc_PP_cat(_tag_arr(_tmp28B,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _tagged_arr sep,struct
Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tag_arr("",sizeof(
char),1),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct Cyc_Absyn_Tunionfield*
f){return({struct Cyc_PP_Doc*_tmp28D[3];_tmp28D[2]=f->typs == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc(f->typs);_tmp28D[1]=Cyc_Absynpp_typedef_name2doc(f->name);
_tmp28D[0]=Cyc_Absynpp_scope2doc((void*)f->sc);Cyc_PP_cat(_tag_arr(_tmp28D,
sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Tunionfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_tunionfield2doc,_tag_arr(",",sizeof(char),2),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);({
void*_tmp28E[0]={};Cyc_Std_fprintf(f,_tag_arr("\n",sizeof(char),2),_tag_arr(
_tmp28E,sizeof(void*),0));});}}struct _tagged_arr Cyc_Absynpp_decllist2string(
struct Cyc_List_List*tdl){return Cyc_PP_string_of_doc(Cyc_PP_seql(_tag_arr("",
sizeof(char),1),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}struct
_tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(
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
