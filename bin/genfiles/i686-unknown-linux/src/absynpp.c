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
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })
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
struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeInt_struct{int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{int tag;
void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
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
type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{
struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*
fields;};struct Cyc_Absyn_Aggrdecl{void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*
sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;struct Cyc_Core_Opt*
defn;};struct Cyc_Absyn_Var_d_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_struct{
int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Tunion_d_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _tagged_arr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{
void*r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct
Cyc_Core_Opt*tenv);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct _tagged_arr Cyc_Absyn_attribute2string(
void*);struct _tuple3{void*f1;struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_Buffer_t;int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct
_tagged_arr*s2);struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,struct
_tagged_arr);unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*
effect,struct Cyc_Absyn_Tvar*fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f);
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct _tagged_arr
Cyc_Absynpp_typ2string(void*);struct _tagged_arr Cyc_Absynpp_typ2cstring(void*);
struct _tagged_arr Cyc_Absynpp_kind2string(void*);struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*);struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_tagged_arr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*);struct _tagged_arr Cyc_Absynpp_decllist2string(struct Cyc_List_List*
tdl);struct _tagged_arr Cyc_Absynpp_prim2string(void*p);struct _tagged_arr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p);struct _tagged_arr Cyc_Absynpp_scope2string(void*sc);int Cyc_Absynpp_is_anon_aggrtype(
void*t);extern struct _tagged_arr Cyc_Absynpp_cyc_string;extern struct _tagged_arr*
Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);struct
_tagged_arr Cyc_Absynpp_char_escape(char);struct _tagged_arr Cyc_Absynpp_string_escape(
struct _tagged_arr);struct _tagged_arr Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);struct _tuple5{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple5*arg);struct
_tuple6{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple6
Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);struct _tuple7{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple7*dp);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*
Cyc_Absynpp_switchCclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
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
_LL3F;_LL3E: return _tag_arr("R",sizeof(char),2);_LL3F: if((int)_tmpC != 4)goto _LL41;
_LL40: return _tag_arr("E",sizeof(char),2);_LL41: if((int)_tmpC != 5)goto _LL36;_LL42:
return _tag_arr("I",sizeof(char),2);_LL36:;}struct _tagged_arr Cyc_Absynpp_kindbound2string(
void*c){void*_tmpD=Cyc_Absyn_compress_kb(c);void*_tmpE;void*_tmpF;_LL44: if(*((
int*)_tmpD)!= 0)goto _LL46;_tmpE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpD)->f1;
_LL45: return Cyc_Absynpp_kind2string(_tmpE);_LL46: if(*((int*)_tmpD)!= 1)goto _LL48;
_LL47: return _tag_arr("?",sizeof(char),2);_LL48: if(*((int*)_tmpD)!= 2)goto _LL43;
_tmpF=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpD)->f2;_LL49: return Cyc_Absynpp_kind2string(
_tmpF);_LL43:;}struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(void*k){return Cyc_PP_text(
Cyc_Absynpp_kind2string(k));}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
return Cyc_PP_text(Cyc_Absynpp_kindbound2string(c));}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(
struct Cyc_List_List*ts){return Cyc_PP_egroup(_tag_arr("<",sizeof(char),2),
_tag_arr(">",sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,
ts));}struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){void*
_tmp10=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp11;void*_tmp12;void*
_tmp13;_LL4B: if(*((int*)_tmp10)!= 1)goto _LL4D;_LL4C: goto _LL4E;_LL4D: if(*((int*)
_tmp10)!= 0)goto _LL4F;_tmp11=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp10)->f1;
if((int)_tmp11 != 2)goto _LL4F;_LL4E: return Cyc_PP_textptr(tv->name);_LL4F: if(*((
int*)_tmp10)!= 2)goto _LL51;_tmp12=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp10)->f2;_LL50: _tmp13=_tmp12;goto _LL52;_LL51: if(*((int*)_tmp10)!= 0)goto _LL4A;
_tmp13=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp10)->f1;_LL52: return({struct
Cyc_PP_Doc*_tmp14[3];_tmp14[2]=Cyc_Absynpp_kind2doc(_tmp13);_tmp14[1]=Cyc_PP_text(
_tag_arr("::",sizeof(char),3));_tmp14[0]=Cyc_PP_textptr(tv->name);Cyc_PP_cat(
_tag_arr(_tmp14,sizeof(struct Cyc_PP_Doc*),3));});_LL4A:;}struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(
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
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){void*_tmp15=att;_LL54: if((
int)_tmp15 != 0)goto _LL56;_LL55: goto _LL57;_LL56: if((int)_tmp15 != 1)goto _LL58;
_LL57: goto _LL59;_LL58: if((int)_tmp15 != 2)goto _LL5A;_LL59: return Cyc_PP_nil_doc();
_LL5A:;_LL5B: return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL53:;}struct
Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 0;
atts=atts->tl){void*_tmp16=(void*)atts->hd;_LL5D: if((int)_tmp16 != 0)goto _LL5F;
_LL5E: return Cyc_PP_text(_tag_arr(" _stdcall ",sizeof(char),11));_LL5F: if((int)
_tmp16 != 1)goto _LL61;_LL60: return Cyc_PP_text(_tag_arr(" _cdecl ",sizeof(char),9));
_LL61: if((int)_tmp16 != 2)goto _LL63;_LL62: return Cyc_PP_text(_tag_arr(" _fastcall ",
sizeof(char),12));_LL63:;_LL64: goto _LL5C;_LL5C:;}return Cyc_PP_nil_doc();}struct
Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int hasatt=0;{
struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmp17=(
void*)atts2->hd;_LL66: if((int)_tmp17 != 0)goto _LL68;_LL67: goto _LL69;_LL68: if((int)
_tmp17 != 1)goto _LL6A;_LL69: goto _LL6B;_LL6A: if((int)_tmp17 != 2)goto _LL6C;_LL6B:
goto _LL65;_LL6C:;_LL6D: hasatt=1;goto _LL65;_LL65:;}}if(!hasatt)return Cyc_PP_nil_doc();
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
Cyc_List_List*tms){if(tms == 0)return 0;{void*_tmp1A=(void*)tms->hd;_LL6F: if(
_tmp1A <= (void*)1?1:*((int*)_tmp1A)!= 1)goto _LL71;_LL70: return 1;_LL71: if(_tmp1A
<= (void*)1?1:*((int*)_tmp1A)!= 4)goto _LL73;_LL72: if(!Cyc_Absynpp_to_VC)return 0;
return Cyc_Absynpp_next_is_pointer(tms->tl);_LL73:;_LL74: return 0;_LL6E:;}}struct
Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(
struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{struct Cyc_PP_Doc*
rest=Cyc_Absynpp_dtms2doc(d,tms->tl);struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*
_tmp3D[3];_tmp3D[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp3D[1]=rest;
_tmp3D[0]=Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp3D,
sizeof(struct Cyc_PP_Doc*),3));});void*_tmp1B=(void*)tms->hd;struct Cyc_Absyn_Exp*
_tmp1C;void*_tmp1D;struct Cyc_List_List*_tmp1E;struct Cyc_List_List*_tmp1F;struct
Cyc_Position_Segment*_tmp20;int _tmp21;void*_tmp22;void*_tmp23;struct Cyc_Absyn_Tqual
_tmp24;_LL76: if((int)_tmp1B != 0)goto _LL78;_LL77: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*_tmp25[2];_tmp25[1]=Cyc_PP_text(
_tag_arr("[]",sizeof(char),3));_tmp25[0]=rest;Cyc_PP_cat(_tag_arr(_tmp25,sizeof(
struct Cyc_PP_Doc*),2));});_LL78: if(_tmp1B <= (void*)1?1:*((int*)_tmp1B)!= 0)goto
_LL7A;_tmp1C=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp1B)->f1;_LL79: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;return({struct Cyc_PP_Doc*_tmp26[4];_tmp26[3]=Cyc_PP_text(
_tag_arr("]",sizeof(char),2));_tmp26[2]=Cyc_Absynpp_exp2doc(_tmp1C);_tmp26[1]=
Cyc_PP_text(_tag_arr("[",sizeof(char),2));_tmp26[0]=rest;Cyc_PP_cat(_tag_arr(
_tmp26,sizeof(struct Cyc_PP_Doc*),4));});_LL7A: if(_tmp1B <= (void*)1?1:*((int*)
_tmp1B)!= 2)goto _LL7C;_tmp1D=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp1B)->f1;_LL7B: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{void*
_tmp27=_tmp1D;struct Cyc_List_List*_tmp28;int _tmp29;struct Cyc_Absyn_VarargInfo*
_tmp2A;struct Cyc_Core_Opt*_tmp2B;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*
_tmp2D;struct Cyc_Position_Segment*_tmp2E;_LL83: if(*((int*)_tmp27)!= 1)goto _LL85;
_tmp28=((struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f1;_tmp29=((struct Cyc_Absyn_WithTypes_struct*)
_tmp27)->f2;_tmp2A=((struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f3;_tmp2B=((
struct Cyc_Absyn_WithTypes_struct*)_tmp27)->f4;_tmp2C=((struct Cyc_Absyn_WithTypes_struct*)
_tmp27)->f5;_LL84: return({struct Cyc_PP_Doc*_tmp2F[2];_tmp2F[1]=Cyc_Absynpp_funargs2doc(
_tmp28,_tmp29,_tmp2A,_tmp2B,_tmp2C);_tmp2F[0]=rest;Cyc_PP_cat(_tag_arr(_tmp2F,
sizeof(struct Cyc_PP_Doc*),2));});_LL85: if(*((int*)_tmp27)!= 0)goto _LL82;_tmp2D=((
struct Cyc_Absyn_NoTypes_struct*)_tmp27)->f1;_tmp2E=((struct Cyc_Absyn_NoTypes_struct*)
_tmp27)->f2;_LL86: return({struct Cyc_PP_Doc*_tmp30[2];_tmp30[1]=Cyc_PP_group(
_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(char),2),_tag_arr(",",sizeof(
char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp2D));_tmp30[0]=rest;Cyc_PP_cat(
_tag_arr(_tmp30,sizeof(struct Cyc_PP_Doc*),2));});_LL82:;}_LL7C: if(_tmp1B <= (void*)
1?1:*((int*)_tmp1B)!= 4)goto _LL7E;_tmp1E=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmp1B)->f2;_LL7D: if(!Cyc_Absynpp_to_VC){if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;return({struct Cyc_PP_Doc*_tmp31[2];_tmp31[1]=Cyc_Absynpp_atts2doc(
_tmp1E);_tmp31[0]=rest;Cyc_PP_cat(_tag_arr(_tmp31,sizeof(struct Cyc_PP_Doc*),2));});}
else{if(Cyc_Absynpp_next_is_pointer(tms->tl))return({struct Cyc_PP_Doc*_tmp32[2];
_tmp32[1]=rest;_tmp32[0]=Cyc_Absynpp_callconv2doc(_tmp1E);Cyc_PP_cat(_tag_arr(
_tmp32,sizeof(struct Cyc_PP_Doc*),2));});return rest;}_LL7E: if(_tmp1B <= (void*)1?1:*((
int*)_tmp1B)!= 3)goto _LL80;_tmp1F=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp1B)->f1;_tmp20=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp1B)->f2;_tmp21=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmp1B)->f3;_LL7F: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmp21)return({struct Cyc_PP_Doc*_tmp33[2];_tmp33[1]=Cyc_Absynpp_ktvars2doc(
_tmp1F);_tmp33[0]=rest;Cyc_PP_cat(_tag_arr(_tmp33,sizeof(struct Cyc_PP_Doc*),2));});
else{return({struct Cyc_PP_Doc*_tmp34[2];_tmp34[1]=Cyc_Absynpp_tvars2doc(_tmp1F);
_tmp34[0]=rest;Cyc_PP_cat(_tag_arr(_tmp34,sizeof(struct Cyc_PP_Doc*),2));});}
_LL80: if(_tmp1B <= (void*)1?1:*((int*)_tmp1B)!= 1)goto _LL75;_tmp22=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp1B)->f1;_tmp23=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp1B)->f2;_tmp24=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp1B)->f3;_LL81: {
struct Cyc_PP_Doc*ptr;{void*_tmp35=_tmp22;struct Cyc_Absyn_Exp*_tmp36;struct Cyc_Absyn_Exp*
_tmp37;_LL88: if(_tmp35 <= (void*)1?1:*((int*)_tmp35)!= 1)goto _LL8A;_tmp36=((
struct Cyc_Absyn_Nullable_ps_struct*)_tmp35)->f1;_LL89: if(Cyc_Evexp_eval_const_uint_exp(
_tmp36)== 1)ptr=Cyc_PP_text(_tag_arr("*",sizeof(char),2));else{ptr=({struct Cyc_PP_Doc*
_tmp38[4];_tmp38[3]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp38[2]=Cyc_Absynpp_exp2doc(
_tmp36);_tmp38[1]=Cyc_PP_text(_tag_arr("{",sizeof(char),2));_tmp38[0]=Cyc_PP_text(
_tag_arr("*",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp38,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL87;_LL8A: if(_tmp35 <= (void*)1?1:*((int*)_tmp35)!= 0)goto _LL8C;
_tmp37=((struct Cyc_Absyn_NonNullable_ps_struct*)_tmp35)->f1;_LL8B: if(Cyc_Evexp_eval_const_uint_exp(
_tmp37)== 1)ptr=Cyc_PP_text(_tag_arr("@",sizeof(char),2));else{ptr=({struct Cyc_PP_Doc*
_tmp39[4];_tmp39[3]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp39[2]=Cyc_Absynpp_exp2doc(
_tmp37);_tmp39[1]=Cyc_PP_text(_tag_arr("{",sizeof(char),2));_tmp39[0]=Cyc_PP_text(
_tag_arr("@",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp39,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL87;_LL8C: if((int)_tmp35 != 0)goto _LL87;_LL8D: ptr=Cyc_PP_text(
_tag_arr("?",sizeof(char),2));goto _LL87;_LL87:;}{void*_tmp3A=Cyc_Tcutil_compress(
_tmp23);_LL8F: if((int)_tmp3A != 2)goto _LL91;_LL90: return({struct Cyc_PP_Doc*_tmp3B[
2];_tmp3B[1]=rest;_tmp3B[0]=ptr;Cyc_PP_cat(_tag_arr(_tmp3B,sizeof(struct Cyc_PP_Doc*),
2));});_LL91:;_LL92: return({struct Cyc_PP_Doc*_tmp3C[4];_tmp3C[3]=rest;_tmp3C[2]=
Cyc_PP_text(_tag_arr(" ",sizeof(char),2));_tmp3C[1]=Cyc_Absynpp_typ2doc(_tmp23);
_tmp3C[0]=ptr;Cyc_PP_cat(_tag_arr(_tmp3C,sizeof(struct Cyc_PP_Doc*),4));});_LL8E:;}}
_LL75:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp3E=Cyc_Tcutil_compress(
t);_LL94: if((int)_tmp3E != 2)goto _LL96;_LL95: return Cyc_PP_text(_tag_arr("`H",
sizeof(char),3));_LL96:;_LL97: return Cyc_Absynpp_ntyp2doc(t);_LL93:;}static void
Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**
effects,void*t){void*_tmp3F=Cyc_Tcutil_compress(t);void*_tmp40;struct Cyc_List_List*
_tmp41;_LL99: if(_tmp3F <= (void*)3?1:*((int*)_tmp3F)!= 19)goto _LL9B;_tmp40=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp3F)->f1;_LL9A:*rgions=({struct Cyc_List_List*
_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->hd=Cyc_Absynpp_rgn2doc(_tmp40);_tmp42->tl=*
rgions;_tmp42;});goto _LL98;_LL9B: if(_tmp3F <= (void*)3?1:*((int*)_tmp3F)!= 20)
goto _LL9D;_tmp41=((struct Cyc_Absyn_JoinEff_struct*)_tmp3F)->f1;_LL9C: for(0;
_tmp41 != 0;_tmp41=_tmp41->tl){Cyc_Absynpp_effects2docs(rgions,effects,(void*)
_tmp41->hd);}goto _LL98;_LL9D:;_LL9E:*effects=({struct Cyc_List_List*_tmp43=
_cycalloc(sizeof(*_tmp43));_tmp43->hd=Cyc_Absynpp_typ2doc(t);_tmp43->tl=*effects;
_tmp43;});goto _LL98;_LL98:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct
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
void*k){void*_tmp46=k;_LLA0: if((int)_tmp46 != 0)goto _LLA2;_LLA1: return Cyc_PP_text(
_tag_arr("struct ",sizeof(char),8));_LLA2: if((int)_tmp46 != 1)goto _LL9F;_LLA3:
return Cyc_PP_text(_tag_arr("union ",sizeof(char),7));_LL9F:;}struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmp47=t;struct Cyc_Core_Opt*
_tmp48;struct Cyc_Core_Opt*_tmp49;int _tmp4A;struct Cyc_Core_Opt*_tmp4B;struct Cyc_Absyn_Tvar*
_tmp4C;struct Cyc_Absyn_TunionInfo _tmp4D;void*_tmp4E;struct Cyc_List_List*_tmp4F;
void*_tmp50;struct Cyc_Absyn_TunionFieldInfo _tmp51;void*_tmp52;struct Cyc_List_List*
_tmp53;void*_tmp54;void*_tmp55;int _tmp56;struct Cyc_List_List*_tmp57;struct Cyc_Absyn_AggrInfo
_tmp58;void*_tmp59;struct Cyc_List_List*_tmp5A;void*_tmp5B;struct Cyc_List_List*
_tmp5C;struct Cyc_List_List*_tmp5D;struct _tuple0*_tmp5E;void*_tmp5F;struct _tuple0*
_tmp60;struct Cyc_List_List*_tmp61;struct Cyc_Absyn_Typedefdecl*_tmp62;struct Cyc_Core_Opt*
_tmp63;void*_tmp64;int _tmp65;void*_tmp66;void*_tmp67;_LLA5: if(_tmp47 <= (void*)3?
1:*((int*)_tmp47)!= 7)goto _LLA7;_LLA6: return Cyc_PP_text(_tag_arr("[[[array]]]",
sizeof(char),12));_LLA7: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 8)goto _LLA9;
_LLA8: return Cyc_PP_nil_doc();_LLA9: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 4)
goto _LLAB;_LLAA: return Cyc_PP_nil_doc();_LLAB: if((int)_tmp47 != 0)goto _LLAD;_LLAC:
s=Cyc_PP_text(_tag_arr("void",sizeof(char),5));goto _LLA4;_LLAD: if(_tmp47 <= (void*)
3?1:*((int*)_tmp47)!= 0)goto _LLAF;_tmp48=((struct Cyc_Absyn_Evar_struct*)_tmp47)->f1;
_tmp49=((struct Cyc_Absyn_Evar_struct*)_tmp47)->f2;_tmp4A=((struct Cyc_Absyn_Evar_struct*)
_tmp47)->f3;_tmp4B=((struct Cyc_Absyn_Evar_struct*)_tmp47)->f4;_LLAE: if(_tmp49 != 
0)return Cyc_Absynpp_ntyp2doc((void*)_tmp49->v);else{s=({struct Cyc_PP_Doc*_tmp68[
6];_tmp68[5]=_tmp48 == 0?Cyc_PP_text(_tag_arr("?",sizeof(char),2)): Cyc_Absynpp_kind2doc((
void*)_tmp48->v);_tmp68[4]=Cyc_PP_text(_tag_arr(")::",sizeof(char),4));_tmp68[3]=(
!Cyc_Absynpp_print_full_evars?1: _tmp4B == 0)?Cyc_PP_text(_tag_arr("",sizeof(char),
1)): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp4B->v);_tmp68[2]=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp6A;_tmp6A.tag=1;_tmp6A.f1=(
int)((unsigned int)_tmp4A);{void*_tmp69[1]={& _tmp6A};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(char),3),_tag_arr(_tmp69,sizeof(void*),1));}}));_tmp68[1]=Cyc_PP_text(
_tag_arr("(",sizeof(char),2));_tmp68[0]=Cyc_PP_text(_tag_arr("%",sizeof(char),2));
Cyc_PP_cat(_tag_arr(_tmp68,sizeof(struct Cyc_PP_Doc*),6));});}goto _LLA4;_LLAF: if(
_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 1)goto _LLB1;_tmp4C=((struct Cyc_Absyn_VarType_struct*)
_tmp47)->f1;_LLB0: s=Cyc_PP_textptr(_tmp4C->name);if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_tmp6B[3];_tmp6B[2]=Cyc_Absynpp_kindbound2doc((void*)_tmp4C->kind);
_tmp6B[1]=Cyc_PP_text(_tag_arr("::",sizeof(char),3));_tmp6B[0]=s;Cyc_PP_cat(
_tag_arr(_tmp6B,sizeof(struct Cyc_PP_Doc*),3));});if(Cyc_Absynpp_rewrite_temp_tvars?
Cyc_Tcutil_is_temp_tvar(_tmp4C): 0)s=({struct Cyc_PP_Doc*_tmp6C[3];_tmp6C[2]=Cyc_PP_text(
_tag_arr(" */",sizeof(char),4));_tmp6C[1]=s;_tmp6C[0]=Cyc_PP_text(_tag_arr("_ /* ",
sizeof(char),6));Cyc_PP_cat(_tag_arr(_tmp6C,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLA4;_LLB1: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 2)goto _LLB3;_tmp4D=((
struct Cyc_Absyn_TunionType_struct*)_tmp47)->f1;_tmp4E=(void*)_tmp4D.tunion_info;
_tmp4F=_tmp4D.targs;_tmp50=(void*)_tmp4D.rgn;_LLB2:{void*_tmp6D=_tmp4E;struct Cyc_Absyn_UnknownTunionInfo
_tmp6E;struct _tuple0*_tmp6F;int _tmp70;struct Cyc_Absyn_Tuniondecl**_tmp71;struct
Cyc_Absyn_Tuniondecl*_tmp72;struct Cyc_Absyn_Tuniondecl _tmp73;struct _tuple0*
_tmp74;int _tmp75;_LLD8: if(*((int*)_tmp6D)!= 0)goto _LLDA;_tmp6E=((struct Cyc_Absyn_UnknownTunion_struct*)
_tmp6D)->f1;_tmp6F=_tmp6E.name;_tmp70=_tmp6E.is_xtunion;_LLD9: _tmp74=_tmp6F;
_tmp75=_tmp70;goto _LLDB;_LLDA: if(*((int*)_tmp6D)!= 1)goto _LLD7;_tmp71=((struct
Cyc_Absyn_KnownTunion_struct*)_tmp6D)->f1;_tmp72=*_tmp71;_tmp73=*_tmp72;_tmp74=
_tmp73.name;_tmp75=_tmp73.is_xtunion;_LLDB: {struct Cyc_PP_Doc*_tmp76=Cyc_PP_text(
_tmp75?_tag_arr("xtunion ",sizeof(char),9): _tag_arr("tunion ",sizeof(char),8));{
void*_tmp77=Cyc_Tcutil_compress(_tmp50);_LLDD: if((int)_tmp77 != 2)goto _LLDF;_LLDE:
s=({struct Cyc_PP_Doc*_tmp78[3];_tmp78[2]=Cyc_Absynpp_tps2doc(_tmp4F);_tmp78[1]=
Cyc_Absynpp_qvar2doc(_tmp74);_tmp78[0]=_tmp76;Cyc_PP_cat(_tag_arr(_tmp78,sizeof(
struct Cyc_PP_Doc*),3));});goto _LLDC;_LLDF:;_LLE0: s=({struct Cyc_PP_Doc*_tmp79[5];
_tmp79[4]=Cyc_Absynpp_tps2doc(_tmp4F);_tmp79[3]=Cyc_Absynpp_qvar2doc(_tmp74);
_tmp79[2]=Cyc_PP_text(_tag_arr(" ",sizeof(char),2));_tmp79[1]=Cyc_Absynpp_typ2doc(
_tmp50);_tmp79[0]=_tmp76;Cyc_PP_cat(_tag_arr(_tmp79,sizeof(struct Cyc_PP_Doc*),5));});
goto _LLDC;_LLDC:;}goto _LLD7;}_LLD7:;}goto _LLA4;_LLB3: if(_tmp47 <= (void*)3?1:*((
int*)_tmp47)!= 3)goto _LLB5;_tmp51=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp47)->f1;_tmp52=(void*)_tmp51.field_info;_tmp53=_tmp51.targs;_LLB4:{void*
_tmp7A=_tmp52;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp7B;struct _tuple0*_tmp7C;
struct _tuple0*_tmp7D;int _tmp7E;struct Cyc_Absyn_Tuniondecl*_tmp7F;struct Cyc_Absyn_Tuniondecl
_tmp80;struct _tuple0*_tmp81;int _tmp82;struct Cyc_Absyn_Tunionfield*_tmp83;struct
Cyc_Absyn_Tunionfield _tmp84;struct _tuple0*_tmp85;_LLE2: if(*((int*)_tmp7A)!= 0)
goto _LLE4;_tmp7B=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp7A)->f1;_tmp7C=
_tmp7B.tunion_name;_tmp7D=_tmp7B.field_name;_tmp7E=_tmp7B.is_xtunion;_LLE3:
_tmp81=_tmp7C;_tmp82=_tmp7E;_tmp85=_tmp7D;goto _LLE5;_LLE4: if(*((int*)_tmp7A)!= 1)
goto _LLE1;_tmp7F=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp7A)->f1;_tmp80=*
_tmp7F;_tmp81=_tmp80.name;_tmp82=_tmp80.is_xtunion;_tmp83=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp7A)->f2;_tmp84=*_tmp83;_tmp85=_tmp84.name;_LLE5: {struct Cyc_PP_Doc*_tmp86=
Cyc_PP_text(_tmp82?_tag_arr("xtunion ",sizeof(char),9): _tag_arr("tunion ",
sizeof(char),8));s=({struct Cyc_PP_Doc*_tmp87[4];_tmp87[3]=Cyc_Absynpp_qvar2doc(
_tmp85);_tmp87[2]=Cyc_PP_text(_tag_arr(".",sizeof(char),2));_tmp87[1]=Cyc_Absynpp_qvar2doc(
_tmp81);_tmp87[0]=_tmp86;Cyc_PP_cat(_tag_arr(_tmp87,sizeof(struct Cyc_PP_Doc*),4));});
goto _LLE1;}_LLE1:;}goto _LLA4;_LLB5: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 5)
goto _LLB7;_tmp54=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47)->f1;_tmp55=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp47)->f2;_LLB6: {struct _tagged_arr sns;
struct _tagged_arr ts;{void*_tmp88=_tmp54;_LLE7: if((int)_tmp88 != 0)goto _LLE9;_LLE8:
sns=_tag_arr("",sizeof(char),1);goto _LLE6;_LLE9: if((int)_tmp88 != 1)goto _LLE6;
_LLEA: sns=_tag_arr("unsigned ",sizeof(char),10);goto _LLE6;_LLE6:;}{void*_tmp89=
_tmp55;_LLEC: if((int)_tmp89 != 0)goto _LLEE;_LLED:{void*_tmp8A=_tmp54;_LLF5: if((
int)_tmp8A != 0)goto _LLF7;_LLF6: sns=_tag_arr("signed ",sizeof(char),8);goto _LLF4;
_LLF7: if((int)_tmp8A != 1)goto _LLF4;_LLF8: sns=_tag_arr("",sizeof(char),1);goto
_LLF4;_LLF4:;}ts=_tag_arr("char",sizeof(char),5);goto _LLEB;_LLEE: if((int)_tmp89
!= 1)goto _LLF0;_LLEF: ts=_tag_arr("short",sizeof(char),6);goto _LLEB;_LLF0: if((int)
_tmp89 != 2)goto _LLF2;_LLF1: ts=_tag_arr("int",sizeof(char),4);goto _LLEB;_LLF2: if((
int)_tmp89 != 3)goto _LLEB;_LLF3: ts=Cyc_Absynpp_to_VC?_tag_arr("__int64",sizeof(
char),8): _tag_arr("long long",sizeof(char),10);goto _LLEB;_LLEB:;}s=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp8D;_tmp8D.tag=0;_tmp8D.f1=(
struct _tagged_arr)ts;{struct Cyc_Std_String_pa_struct _tmp8C;_tmp8C.tag=0;_tmp8C.f1=(
struct _tagged_arr)sns;{void*_tmp8B[2]={& _tmp8C,& _tmp8D};Cyc_Std_aprintf(_tag_arr("%s%s",
sizeof(char),5),_tag_arr(_tmp8B,sizeof(void*),2));}}}));goto _LLA4;}_LLB7: if((int)
_tmp47 != 1)goto _LLB9;_LLB8: s=Cyc_PP_text(_tag_arr("float",sizeof(char),6));goto
_LLA4;_LLB9: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 6)goto _LLBB;_tmp56=((struct
Cyc_Absyn_DoubleType_struct*)_tmp47)->f1;_LLBA: if(_tmp56)s=Cyc_PP_text(_tag_arr("long double",
sizeof(char),12));else{s=Cyc_PP_text(_tag_arr("double",sizeof(char),7));}goto
_LLA4;_LLBB: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 9)goto _LLBD;_tmp57=((struct
Cyc_Absyn_TupleType_struct*)_tmp47)->f1;_LLBC: s=({struct Cyc_PP_Doc*_tmp8E[2];
_tmp8E[1]=Cyc_Absynpp_args2doc(_tmp57);_tmp8E[0]=Cyc_PP_text(_tag_arr("$",
sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp8E,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLA4;_LLBD: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 10)goto _LLBF;_tmp58=((
struct Cyc_Absyn_AggrType_struct*)_tmp47)->f1;_tmp59=(void*)_tmp58.aggr_info;
_tmp5A=_tmp58.targs;_LLBE: {void*_tmp90;struct _tuple0*_tmp91;struct _tuple3 _tmp8F=
Cyc_Absyn_aggr_kinded_name(_tmp59);_tmp90=_tmp8F.f1;_tmp91=_tmp8F.f2;s=({struct
Cyc_PP_Doc*_tmp92[3];_tmp92[2]=Cyc_Absynpp_tps2doc(_tmp5A);_tmp92[1]=Cyc_Absynpp_qvar2doc(
_tmp91);_tmp92[0]=Cyc_Absynpp_aggr_kind2doc(_tmp90);Cyc_PP_cat(_tag_arr(_tmp92,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLA4;}_LLBF: if(_tmp47 <= (void*)3?1:*((int*)
_tmp47)!= 11)goto _LLC1;_tmp5B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp47)->f1;_tmp5C=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp47)->f2;_LLC0: s=({
struct Cyc_PP_Doc*_tmp93[4];_tmp93[3]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp93[2]=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp5C));_tmp93[1]=Cyc_PP_text(
_tag_arr("{",sizeof(char),2));_tmp93[0]=Cyc_Absynpp_aggr_kind2doc(_tmp5B);Cyc_PP_cat(
_tag_arr(_tmp93,sizeof(struct Cyc_PP_Doc*),4));});goto _LLA4;_LLC1: if(_tmp47 <= (
void*)3?1:*((int*)_tmp47)!= 13)goto _LLC3;_tmp5D=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp47)->f1;_LLC2: s=({struct Cyc_PP_Doc*_tmp94[3];_tmp94[2]=Cyc_PP_text(_tag_arr("}",
sizeof(char),2));_tmp94[1]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp5D));
_tmp94[0]=Cyc_PP_text(_tag_arr("enum {",sizeof(char),7));Cyc_PP_cat(_tag_arr(
_tmp94,sizeof(struct Cyc_PP_Doc*),3));});goto _LLA4;_LLC3: if(_tmp47 <= (void*)3?1:*((
int*)_tmp47)!= 12)goto _LLC5;_tmp5E=((struct Cyc_Absyn_EnumType_struct*)_tmp47)->f1;
_LLC4: s=({struct Cyc_PP_Doc*_tmp95[2];_tmp95[1]=Cyc_Absynpp_qvar2doc(_tmp5E);
_tmp95[0]=Cyc_PP_text(_tag_arr("enum ",sizeof(char),6));Cyc_PP_cat(_tag_arr(
_tmp95,sizeof(struct Cyc_PP_Doc*),2));});goto _LLA4;_LLC5: if(_tmp47 <= (void*)3?1:*((
int*)_tmp47)!= 14)goto _LLC7;_tmp5F=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp47)->f1;_LLC6: s=({struct Cyc_PP_Doc*_tmp96[3];_tmp96[2]=Cyc_PP_text(_tag_arr(">",
sizeof(char),2));_tmp96[1]=Cyc_Absynpp_typ2doc(_tmp5F);_tmp96[0]=Cyc_PP_text(
_tag_arr("sizeof_t<",sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp96,sizeof(struct
Cyc_PP_Doc*),3));});goto _LLA4;_LLC7: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 16)
goto _LLC9;_tmp60=((struct Cyc_Absyn_TypedefType_struct*)_tmp47)->f1;_tmp61=((
struct Cyc_Absyn_TypedefType_struct*)_tmp47)->f2;_tmp62=((struct Cyc_Absyn_TypedefType_struct*)
_tmp47)->f3;_tmp63=((struct Cyc_Absyn_TypedefType_struct*)_tmp47)->f4;_LLC8: s=({
struct Cyc_PP_Doc*_tmp97[2];_tmp97[1]=Cyc_Absynpp_tps2doc(_tmp61);_tmp97[0]=Cyc_Absynpp_qvar2doc(
_tmp60);Cyc_PP_cat(_tag_arr(_tmp97,sizeof(struct Cyc_PP_Doc*),2));});goto _LLA4;
_LLC9: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 15)goto _LLCB;_tmp64=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp47)->f1;_LLCA: s=({struct Cyc_PP_Doc*
_tmp98[3];_tmp98[2]=Cyc_PP_text(_tag_arr(">",sizeof(char),2));_tmp98[1]=Cyc_Absynpp_rgn2doc(
_tmp64);_tmp98[0]=Cyc_PP_text(_tag_arr("region_t<",sizeof(char),10));Cyc_PP_cat(
_tag_arr(_tmp98,sizeof(struct Cyc_PP_Doc*),3));});goto _LLA4;_LLCB: if(_tmp47 <= (
void*)3?1:*((int*)_tmp47)!= 18)goto _LLCD;_tmp65=((struct Cyc_Absyn_TypeInt_struct*)
_tmp47)->f1;_LLCC: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp9A;_tmp9A.tag=1;_tmp9A.f1=(int)((unsigned int)_tmp65);{void*_tmp99[1]={&
_tmp9A};Cyc_Std_aprintf(_tag_arr("`%d",sizeof(char),4),_tag_arr(_tmp99,sizeof(
void*),1));}}));goto _LLA4;_LLCD: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 17)goto
_LLCF;_tmp66=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp47)->f1;_LLCE: s=({
struct Cyc_PP_Doc*_tmp9B[3];_tmp9B[2]=Cyc_PP_text(_tag_arr(">",sizeof(char),2));
_tmp9B[1]=Cyc_Absynpp_typ2doc(_tmp66);_tmp9B[0]=Cyc_PP_text(_tag_arr("tag_t<",
sizeof(char),7));Cyc_PP_cat(_tag_arr(_tmp9B,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLA4;_LLCF: if((int)_tmp47 != 2)goto _LLD1;_LLD0: s=Cyc_Absynpp_rgn2doc(t);goto
_LLA4;_LLD1: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 21)goto _LLD3;_tmp67=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp47)->f1;_LLD2: s=({struct Cyc_PP_Doc*_tmp9C[3];
_tmp9C[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp9C[1]=Cyc_Absynpp_typ2doc(
_tmp67);_tmp9C[0]=Cyc_PP_text(_tag_arr("regions(",sizeof(char),9));Cyc_PP_cat(
_tag_arr(_tmp9C,sizeof(struct Cyc_PP_Doc*),3));});goto _LLA4;_LLD3: if(_tmp47 <= (
void*)3?1:*((int*)_tmp47)!= 19)goto _LLD5;_LLD4: goto _LLD6;_LLD5: if(_tmp47 <= (void*)
3?1:*((int*)_tmp47)!= 20)goto _LLA4;_LLD6: s=Cyc_Absynpp_eff2doc(t);goto _LLA4;
_LLA4:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _tagged_arr*)vo->v));}struct
Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple4*cmp){struct _tuple4 _tmp9E;void*
_tmp9F;void*_tmpA0;struct _tuple4*_tmp9D=cmp;_tmp9E=*_tmp9D;_tmp9F=_tmp9E.f1;
_tmpA0=_tmp9E.f2;return({struct Cyc_PP_Doc*_tmpA1[3];_tmpA1[2]=Cyc_Absynpp_rgn2doc(
_tmpA0);_tmpA1[1]=Cyc_PP_text(_tag_arr(" > ",sizeof(char),4));_tmpA1[0]=Cyc_Absynpp_rgn2doc(
_tmp9F);Cyc_PP_cat(_tag_arr(_tmpA1,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*
Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){return Cyc_PP_group(_tag_arr("",
sizeof(char),1),_tag_arr("",sizeof(char),1),_tag_arr(",",sizeof(char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple4*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(
struct _tuple1*t){struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:({struct Cyc_Core_Opt*
_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->v=Cyc_PP_text(*((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null((*t).f1))->v));_tmpA2;});return Cyc_Absynpp_tqtd2doc((*
t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmpA3=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs)_tmpA3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA3,({struct Cyc_List_List*
_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->hd=Cyc_PP_text(_tag_arr("...",sizeof(
char),4));_tmpA4->tl=0;_tmpA4;}));else{if(cyc_varargs != 0){struct Cyc_PP_Doc*
_tmpA5=({struct Cyc_PP_Doc*_tmpA7[3];_tmpA7[2]=Cyc_Absynpp_funarg2doc(({struct
_tuple1*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->f1=cyc_varargs->name;_tmpA8->f2=
cyc_varargs->tq;_tmpA8->f3=(void*)cyc_varargs->type;_tmpA8;}));_tmpA7[1]=
cyc_varargs->inject?Cyc_PP_text(_tag_arr(" inject ",sizeof(char),9)): Cyc_PP_text(
_tag_arr(" ",sizeof(char),2));_tmpA7[0]=Cyc_PP_text(_tag_arr("...",sizeof(char),
4));Cyc_PP_cat(_tag_arr(_tmpA7,sizeof(struct Cyc_PP_Doc*),3));});_tmpA3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmpA3,({struct Cyc_List_List*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->hd=_tmpA5;
_tmpA6->tl=0;_tmpA6;}));}}{struct Cyc_PP_Doc*_tmpA9=Cyc_PP_group(_tag_arr("",
sizeof(char),1),_tag_arr("",sizeof(char),1),_tag_arr(",",sizeof(char),2),_tmpA3);
if(effopt != 0)_tmpA9=({struct Cyc_PP_Doc*_tmpAA[3];_tmpAA[2]=Cyc_Absynpp_eff2doc((
void*)effopt->v);_tmpAA[1]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmpAA[0]=
_tmpA9;Cyc_PP_cat(_tag_arr(_tmpAA,sizeof(struct Cyc_PP_Doc*),3));});if(rgn_po != 0)
_tmpA9=({struct Cyc_PP_Doc*_tmpAB[3];_tmpAB[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);
_tmpAB[1]=Cyc_PP_text(_tag_arr(":",sizeof(char),2));_tmpAB[0]=_tmpA9;Cyc_PP_cat(
_tag_arr(_tmpAB,sizeof(struct Cyc_PP_Doc*),3));});return({struct Cyc_PP_Doc*_tmpAC[
3];_tmpAC[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmpAC[1]=_tmpA9;_tmpAC[0]=
Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmpAC,sizeof(
struct Cyc_PP_Doc*),3));});}}struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple5*
arg){return({struct _tuple1*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->f1=({struct
Cyc_Core_Opt*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->v=(*arg).f1;_tmpAE;});
_tmpAD->f2=(*arg).f2;_tmpAD->f3=(*arg).f3;_tmpAD;});}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(
struct _tagged_arr*v){return Cyc_PP_text(*v);}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q){struct Cyc_List_List*_tmpAF=0;int match;{void*_tmpB0=(*q).f1;
struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB2;_LLFA: if((int)_tmpB0 != 0)
goto _LLFC;_LLFB: _tmpB1=0;goto _LLFD;_LLFC: if(_tmpB0 <= (void*)1?1:*((int*)_tmpB0)
!= 0)goto _LLFE;_tmpB1=((struct Cyc_Absyn_Rel_n_struct*)_tmpB0)->f1;_LLFD: match=0;
_tmpAF=_tmpB1;goto _LLF9;_LLFE: if(_tmpB0 <= (void*)1?1:*((int*)_tmpB0)!= 1)goto
_LLF9;_tmpB2=((struct Cyc_Absyn_Abs_n_struct*)_tmpB0)->f1;_LLFF: match=Cyc_Absynpp_use_curr_namespace?((
int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_Std_strptrcmp,_tmpB2,Cyc_Absynpp_curr_namespace):
0;_tmpAF=(Cyc_Absynpp_qvar_to_Cids?Cyc_Absynpp_add_cyc_prefix: 0)?({struct Cyc_List_List*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->hd=Cyc_Absynpp_cyc_stringptr;_tmpB3->tl=
_tmpB2;_tmpB3;}): _tmpB2;goto _LLF9;_LLF9:;}if(Cyc_Absynpp_qvar_to_Cids)return Cyc_PP_text((
struct _tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmpAF,({struct Cyc_List_List*_tmpB4=
_cycalloc(sizeof(*_tmpB4));_tmpB4->hd=(*q).f2;_tmpB4->tl=0;_tmpB4;})),_tag_arr("_",
sizeof(char),2)));else{if(match)return Cyc_Absynpp_var2doc((*q).f2);else{return
Cyc_PP_text((struct _tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpAF,({struct Cyc_List_List*
_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->hd=(*q).f2;_tmpB5->tl=0;_tmpB5;})),
_tag_arr("::",sizeof(char),3)));}}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2doc(v);if(
Cyc_Absynpp_use_curr_namespace){void*_tmpB6=(*v).f1;struct Cyc_List_List*_tmpB7;
struct Cyc_List_List*_tmpB8;_LL101: if((int)_tmpB6 != 0)goto _LL103;_LL102: goto
_LL104;_LL103: if(_tmpB6 <= (void*)1?1:*((int*)_tmpB6)!= 0)goto _LL105;_tmpB7=((
struct Cyc_Absyn_Rel_n_struct*)_tmpB6)->f1;if(_tmpB7 != 0)goto _LL105;_LL104: return
Cyc_Absynpp_var2doc((*v).f2);_LL105: if(_tmpB6 <= (void*)1?1:*((int*)_tmpB6)!= 1)
goto _LL107;_tmpB8=((struct Cyc_Absyn_Abs_n_struct*)_tmpB6)->f1;_LL106: if(((int(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Std_strptrcmp,_tmpB8,Cyc_Absynpp_curr_namespace)
== 0)return Cyc_Absynpp_var2doc((*v).f2);else{goto _LL108;}_LL107:;_LL108: return({
struct Cyc_PP_Doc*_tmpB9[2];_tmpB9[1]=Cyc_Absynpp_qvar2doc(v);_tmpB9[0]=Cyc_PP_text(
_tag_arr("/* bad namespace : */ ",sizeof(char),23));Cyc_PP_cat(_tag_arr(_tmpB9,
sizeof(struct Cyc_PP_Doc*),2));});_LL100:;}else{return Cyc_Absynpp_var2doc((*v).f2);}}
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(),
t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmpBA=(void*)e->r;
void*_tmpBB;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Exp*_tmpBD;_LL10A: if(*((
int*)_tmpBA)!= 0)goto _LL10C;_LL10B: goto _LL10D;_LL10C: if(*((int*)_tmpBA)!= 1)goto
_LL10E;_LL10D: goto _LL10F;_LL10E: if(*((int*)_tmpBA)!= 2)goto _LL110;_LL10F: return
10000;_LL110: if(*((int*)_tmpBA)!= 3)goto _LL112;_tmpBB=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmpBA)->f1;_LL111: {void*_tmpBE=_tmpBB;_LL157: if((int)_tmpBE != 0)goto _LL159;
_LL158: return 100;_LL159: if((int)_tmpBE != 1)goto _LL15B;_LL15A: return 110;_LL15B:
if((int)_tmpBE != 2)goto _LL15D;_LL15C: return 100;_LL15D: if((int)_tmpBE != 3)goto
_LL15F;_LL15E: goto _LL160;_LL15F: if((int)_tmpBE != 4)goto _LL161;_LL160: return 110;
_LL161: if((int)_tmpBE != 5)goto _LL163;_LL162: goto _LL164;_LL163: if((int)_tmpBE != 6)
goto _LL165;_LL164: return 70;_LL165: if((int)_tmpBE != 7)goto _LL167;_LL166: goto
_LL168;_LL167: if((int)_tmpBE != 8)goto _LL169;_LL168: goto _LL16A;_LL169: if((int)
_tmpBE != 9)goto _LL16B;_LL16A: goto _LL16C;_LL16B: if((int)_tmpBE != 10)goto _LL16D;
_LL16C: return 80;_LL16D: if((int)_tmpBE != 11)goto _LL16F;_LL16E: goto _LL170;_LL16F:
if((int)_tmpBE != 12)goto _LL171;_LL170: return 130;_LL171: if((int)_tmpBE != 13)goto
_LL173;_LL172: return 60;_LL173: if((int)_tmpBE != 14)goto _LL175;_LL174: return 40;
_LL175: if((int)_tmpBE != 15)goto _LL177;_LL176: return 50;_LL177: if((int)_tmpBE != 16)
goto _LL179;_LL178: return 90;_LL179: if((int)_tmpBE != 17)goto _LL17B;_LL17A: return 80;
_LL17B: if((int)_tmpBE != 18)goto _LL17D;_LL17C: return 80;_LL17D: if((int)_tmpBE != 19)
goto _LL156;_LL17E: return 140;_LL156:;}_LL112: if(*((int*)_tmpBA)!= 4)goto _LL114;
_LL113: return 20;_LL114: if(*((int*)_tmpBA)!= 5)goto _LL116;_LL115: return 130;_LL116:
if(*((int*)_tmpBA)!= 6)goto _LL118;_LL117: return 30;_LL118: if(*((int*)_tmpBA)!= 7)
goto _LL11A;_LL119: return 10;_LL11A: if(*((int*)_tmpBA)!= 8)goto _LL11C;_LL11B: goto
_LL11D;_LL11C: if(*((int*)_tmpBA)!= 9)goto _LL11E;_LL11D: return 140;_LL11E: if(*((
int*)_tmpBA)!= 10)goto _LL120;_LL11F: return 130;_LL120: if(*((int*)_tmpBA)!= 11)
goto _LL122;_tmpBC=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpBA)->f1;_LL121:
return Cyc_Absynpp_exp_prec(_tmpBC);_LL122: if(*((int*)_tmpBA)!= 12)goto _LL124;
_tmpBD=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpBA)->f1;_LL123: return Cyc_Absynpp_exp_prec(
_tmpBD);_LL124: if(*((int*)_tmpBA)!= 13)goto _LL126;_LL125: return 120;_LL126: if(*((
int*)_tmpBA)!= 15)goto _LL128;_LL127: return 15;_LL128: if(*((int*)_tmpBA)!= 14)goto
_LL12A;_LL129: goto _LL12B;_LL12A: if(*((int*)_tmpBA)!= 16)goto _LL12C;_LL12B: goto
_LL12D;_LL12C: if(*((int*)_tmpBA)!= 17)goto _LL12E;_LL12D: goto _LL12F;_LL12E: if(*((
int*)_tmpBA)!= 18)goto _LL130;_LL12F: goto _LL131;_LL130: if(*((int*)_tmpBA)!= 19)
goto _LL132;_LL131: goto _LL133;_LL132: if(*((int*)_tmpBA)!= 20)goto _LL134;_LL133:
return 130;_LL134: if(*((int*)_tmpBA)!= 21)goto _LL136;_LL135: goto _LL137;_LL136: if(*((
int*)_tmpBA)!= 22)goto _LL138;_LL137: goto _LL139;_LL138: if(*((int*)_tmpBA)!= 23)
goto _LL13A;_LL139: return 140;_LL13A: if(*((int*)_tmpBA)!= 24)goto _LL13C;_LL13B:
return 150;_LL13C: if(*((int*)_tmpBA)!= 25)goto _LL13E;_LL13D: goto _LL13F;_LL13E: if(*((
int*)_tmpBA)!= 26)goto _LL140;_LL13F: goto _LL141;_LL140: if(*((int*)_tmpBA)!= 27)
goto _LL142;_LL141: goto _LL143;_LL142: if(*((int*)_tmpBA)!= 28)goto _LL144;_LL143:
goto _LL145;_LL144: if(*((int*)_tmpBA)!= 29)goto _LL146;_LL145: goto _LL147;_LL146:
if(*((int*)_tmpBA)!= 30)goto _LL148;_LL147: goto _LL149;_LL148: if(*((int*)_tmpBA)!= 
31)goto _LL14A;_LL149: goto _LL14B;_LL14A: if(*((int*)_tmpBA)!= 32)goto _LL14C;_LL14B:
goto _LL14D;_LL14C: if(*((int*)_tmpBA)!= 33)goto _LL14E;_LL14D: goto _LL14F;_LL14E:
if(*((int*)_tmpBA)!= 34)goto _LL150;_LL14F: return 140;_LL150: if(*((int*)_tmpBA)!= 
35)goto _LL152;_LL151: return 10000;_LL152: if(*((int*)_tmpBA)!= 36)goto _LL154;
_LL153: goto _LL155;_LL154: if(*((int*)_tmpBA)!= 37)goto _LL109;_LL155: return 140;
_LL109:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(
0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*s;{void*_tmpBF=(void*)e->r;
void*_tmpC0;struct _tuple0*_tmpC1;struct _tuple0*_tmpC2;void*_tmpC3;struct Cyc_List_List*
_tmpC4;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_Core_Opt*_tmpC6;struct Cyc_Absyn_Exp*
_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;void*_tmpC9;struct Cyc_Absyn_Exp*_tmpCA;struct
Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Exp*_tmpCD;
struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_List_List*_tmpD0;
struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_List_List*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;
struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD5;void*_tmpD6;struct Cyc_Absyn_Exp*
_tmpD7;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_Absyn_Exp*
_tmpDA;void*_tmpDB;struct Cyc_Absyn_Exp*_tmpDC;void*_tmpDD;void*_tmpDE;struct
_tagged_arr*_tmpDF;void*_tmpE0;void*_tmpE1;unsigned int _tmpE2;struct Cyc_List_List*
_tmpE3;void*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE6;struct
_tagged_arr*_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;struct _tagged_arr*_tmpE9;struct Cyc_Absyn_Exp*
_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_List_List*_tmpEC;struct _tuple1*
_tmpED;struct Cyc_List_List*_tmpEE;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_Vardecl*
_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF2;struct _tuple0*
_tmpF3;struct Cyc_List_List*_tmpF4;struct Cyc_List_List*_tmpF5;struct Cyc_List_List*
_tmpF6;struct Cyc_List_List*_tmpF7;struct Cyc_Absyn_Tunionfield*_tmpF8;struct
_tuple0*_tmpF9;struct _tuple0*_tmpFA;struct Cyc_Absyn_MallocInfo _tmpFB;int _tmpFC;
struct Cyc_Absyn_Exp*_tmpFD;void**_tmpFE;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_Core_Opt*
_tmp100;struct Cyc_List_List*_tmp101;struct Cyc_Absyn_Stmt*_tmp102;struct Cyc_Absyn_Fndecl*
_tmp103;struct Cyc_Absyn_Exp*_tmp104;_LL180: if(*((int*)_tmpBF)!= 0)goto _LL182;
_tmpC0=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpBF)->f1;_LL181: s=Cyc_Absynpp_cnst2doc(
_tmpC0);goto _LL17F;_LL182: if(*((int*)_tmpBF)!= 1)goto _LL184;_tmpC1=((struct Cyc_Absyn_Var_e_struct*)
_tmpBF)->f1;_LL183: _tmpC2=_tmpC1;goto _LL185;_LL184: if(*((int*)_tmpBF)!= 2)goto
_LL186;_tmpC2=((struct Cyc_Absyn_UnknownId_e_struct*)_tmpBF)->f1;_LL185: s=Cyc_Absynpp_qvar2doc(
_tmpC2);goto _LL17F;_LL186: if(*((int*)_tmpBF)!= 3)goto _LL188;_tmpC3=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpBF)->f1;_tmpC4=((struct Cyc_Absyn_Primop_e_struct*)
_tmpBF)->f2;_LL187: s=Cyc_Absynpp_primapp2doc(myprec,_tmpC3,_tmpC4);goto _LL17F;
_LL188: if(*((int*)_tmpBF)!= 4)goto _LL18A;_tmpC5=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpBF)->f1;_tmpC6=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpBF)->f2;_tmpC7=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpBF)->f3;_LL189: s=({struct Cyc_PP_Doc*
_tmp105[5];_tmp105[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC7);_tmp105[3]=Cyc_PP_text(
_tag_arr("= ",sizeof(char),3));_tmp105[2]=_tmpC6 == 0?Cyc_PP_text(_tag_arr("",
sizeof(char),1)): Cyc_Absynpp_prim2doc((void*)_tmpC6->v);_tmp105[1]=Cyc_PP_text(
_tag_arr(" ",sizeof(char),2));_tmp105[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC5);
Cyc_PP_cat(_tag_arr(_tmp105,sizeof(struct Cyc_PP_Doc*),5));});goto _LL17F;_LL18A:
if(*((int*)_tmpBF)!= 5)goto _LL18C;_tmpC8=((struct Cyc_Absyn_Increment_e_struct*)
_tmpBF)->f1;_tmpC9=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpBF)->f2;
_LL18B: {struct Cyc_PP_Doc*_tmp106=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC8);{void*
_tmp107=_tmpC9;_LL1CF: if((int)_tmp107 != 0)goto _LL1D1;_LL1D0: s=({struct Cyc_PP_Doc*
_tmp108[2];_tmp108[1]=_tmp106;_tmp108[0]=Cyc_PP_text(_tag_arr("++",sizeof(char),
3));Cyc_PP_cat(_tag_arr(_tmp108,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1CE;
_LL1D1: if((int)_tmp107 != 2)goto _LL1D3;_LL1D2: s=({struct Cyc_PP_Doc*_tmp109[2];
_tmp109[1]=_tmp106;_tmp109[0]=Cyc_PP_text(_tag_arr("--",sizeof(char),3));Cyc_PP_cat(
_tag_arr(_tmp109,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1CE;_LL1D3: if((int)
_tmp107 != 1)goto _LL1D5;_LL1D4: s=({struct Cyc_PP_Doc*_tmp10A[2];_tmp10A[1]=Cyc_PP_text(
_tag_arr("++",sizeof(char),3));_tmp10A[0]=_tmp106;Cyc_PP_cat(_tag_arr(_tmp10A,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL1CE;_LL1D5: if((int)_tmp107 != 3)goto
_LL1CE;_LL1D6: s=({struct Cyc_PP_Doc*_tmp10B[2];_tmp10B[1]=Cyc_PP_text(_tag_arr("--",
sizeof(char),3));_tmp10B[0]=_tmp106;Cyc_PP_cat(_tag_arr(_tmp10B,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1CE;_LL1CE:;}goto _LL17F;}_LL18C: if(*((int*)_tmpBF)!= 6)goto _LL18E;
_tmpCA=((struct Cyc_Absyn_Conditional_e_struct*)_tmpBF)->f1;_tmpCB=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpBF)->f2;_tmpCC=((struct Cyc_Absyn_Conditional_e_struct*)_tmpBF)->f3;_LL18D:{
struct _tuple4 _tmp10D=({struct _tuple4 _tmp10C;_tmp10C.f1=(void*)_tmpCB->r;_tmp10C.f2=(
void*)_tmpCC->r;_tmp10C;});_LL1D8:;_LL1D9: s=({struct Cyc_PP_Doc*_tmp10E[5];
_tmp10E[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpCC);_tmp10E[3]=Cyc_PP_text(
_tag_arr(" : ",sizeof(char),4));_tmp10E[2]=Cyc_Absynpp_exp2doc_prec(0,_tmpCB);
_tmp10E[1]=Cyc_PP_text(_tag_arr(" ? ",sizeof(char),4));_tmp10E[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpCA);Cyc_PP_cat(_tag_arr(_tmp10E,sizeof(struct Cyc_PP_Doc*),5));});goto
_LL1D7;_LL1D7:;}goto _LL17F;_LL18E: if(*((int*)_tmpBF)!= 7)goto _LL190;_tmpCD=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmpBF)->f1;_tmpCE=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpBF)->f2;_LL18F: s=({struct Cyc_PP_Doc*_tmp10F[5];_tmp10F[4]=Cyc_PP_text(
_tag_arr(")",sizeof(char),2));_tmp10F[3]=Cyc_Absynpp_exp2doc(_tmpCE);_tmp10F[2]=
Cyc_PP_text(_tag_arr(", ",sizeof(char),3));_tmp10F[1]=Cyc_Absynpp_exp2doc(_tmpCD);
_tmp10F[0]=Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp10F,
sizeof(struct Cyc_PP_Doc*),5));});goto _LL17F;_LL190: if(*((int*)_tmpBF)!= 8)goto
_LL192;_tmpCF=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmpBF)->f1;_tmpD0=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmpBF)->f2;_LL191: s=({struct Cyc_PP_Doc*
_tmp110[4];_tmp110[3]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp110[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmpD0);_tmp110[1]=Cyc_PP_text(_tag_arr("(",sizeof(char),2));_tmp110[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpCF);Cyc_PP_cat(_tag_arr(_tmp110,sizeof(struct Cyc_PP_Doc*),4));});goto
_LL17F;_LL192: if(*((int*)_tmpBF)!= 9)goto _LL194;_tmpD1=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpBF)->f1;_tmpD2=((struct Cyc_Absyn_FnCall_e_struct*)_tmpBF)->f2;_LL193: s=({
struct Cyc_PP_Doc*_tmp111[4];_tmp111[3]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp111[2]=Cyc_Absynpp_exps2doc_prec(20,_tmpD2);_tmp111[1]=Cyc_PP_text(_tag_arr("(",
sizeof(char),2));_tmp111[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpD1);Cyc_PP_cat(
_tag_arr(_tmp111,sizeof(struct Cyc_PP_Doc*),4));});goto _LL17F;_LL194: if(*((int*)
_tmpBF)!= 10)goto _LL196;_tmpD3=((struct Cyc_Absyn_Throw_e_struct*)_tmpBF)->f1;
_LL195: s=({struct Cyc_PP_Doc*_tmp112[2];_tmp112[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpD3);_tmp112[0]=Cyc_PP_text(_tag_arr("throw ",sizeof(char),7));Cyc_PP_cat(
_tag_arr(_tmp112,sizeof(struct Cyc_PP_Doc*),2));});goto _LL17F;_LL196: if(*((int*)
_tmpBF)!= 11)goto _LL198;_tmpD4=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpBF)->f1;
_LL197: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmpD4);goto _LL17F;_LL198: if(*((int*)
_tmpBF)!= 12)goto _LL19A;_tmpD5=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpBF)->f1;
_LL199: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmpD5);goto _LL17F;_LL19A: if(*((int*)
_tmpBF)!= 13)goto _LL19C;_tmpD6=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpBF)->f1;
_tmpD7=((struct Cyc_Absyn_Cast_e_struct*)_tmpBF)->f2;_LL19B: s=({struct Cyc_PP_Doc*
_tmp113[4];_tmp113[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpD7);_tmp113[2]=Cyc_PP_text(
_tag_arr(")",sizeof(char),2));_tmp113[1]=Cyc_Absynpp_typ2doc(_tmpD6);_tmp113[0]=
Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp113,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL17F;_LL19C: if(*((int*)_tmpBF)!= 14)goto _LL19E;
_tmpD8=((struct Cyc_Absyn_Address_e_struct*)_tmpBF)->f1;_LL19D: s=({struct Cyc_PP_Doc*
_tmp114[2];_tmp114[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpD8);_tmp114[0]=Cyc_PP_text(
_tag_arr("&",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp114,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL17F;_LL19E: if(*((int*)_tmpBF)!= 15)goto _LL1A0;_tmpD9=((struct Cyc_Absyn_New_e_struct*)
_tmpBF)->f1;_tmpDA=((struct Cyc_Absyn_New_e_struct*)_tmpBF)->f2;_LL19F: if(_tmpD9
== 0)s=({struct Cyc_PP_Doc*_tmp115[2];_tmp115[1]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmpDA);_tmp115[0]=Cyc_PP_text(_tag_arr("new ",sizeof(char),5));Cyc_PP_cat(
_tag_arr(_tmp115,sizeof(struct Cyc_PP_Doc*),2));});else{s=({struct Cyc_PP_Doc*
_tmp116[4];_tmp116[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpDA);_tmp116[2]=Cyc_PP_text(
_tag_arr(") ",sizeof(char),3));_tmp116[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmpD9));_tmp116[0]=Cyc_PP_text(_tag_arr("rnew(",sizeof(char),6));
Cyc_PP_cat(_tag_arr(_tmp116,sizeof(struct Cyc_PP_Doc*),4));});}goto _LL17F;_LL1A0:
if(*((int*)_tmpBF)!= 16)goto _LL1A2;_tmpDB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpBF)->f1;_LL1A1: s=({struct Cyc_PP_Doc*_tmp117[3];_tmp117[2]=Cyc_PP_text(
_tag_arr(")",sizeof(char),2));_tmp117[1]=Cyc_Absynpp_typ2doc(_tmpDB);_tmp117[0]=
Cyc_PP_text(_tag_arr("sizeof(",sizeof(char),8));Cyc_PP_cat(_tag_arr(_tmp117,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL17F;_LL1A2: if(*((int*)_tmpBF)!= 17)goto
_LL1A4;_tmpDC=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpBF)->f1;_LL1A3: s=({
struct Cyc_PP_Doc*_tmp118[3];_tmp118[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp118[1]=Cyc_Absynpp_exp2doc(_tmpDC);_tmp118[0]=Cyc_PP_text(_tag_arr("sizeof(",
sizeof(char),8));Cyc_PP_cat(_tag_arr(_tmp118,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL17F;_LL1A4: if(*((int*)_tmpBF)!= 18)goto _LL1A6;_tmpDD=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpBF)->f1;_tmpDE=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpBF)->f2;if(*((
int*)_tmpDE)!= 0)goto _LL1A6;_tmpDF=((struct Cyc_Absyn_StructField_struct*)_tmpDE)->f1;
_LL1A5: s=({struct Cyc_PP_Doc*_tmp119[5];_tmp119[4]=Cyc_PP_text(_tag_arr(")",
sizeof(char),2));_tmp119[3]=Cyc_PP_textptr(_tmpDF);_tmp119[2]=Cyc_PP_text(
_tag_arr(",",sizeof(char),2));_tmp119[1]=Cyc_Absynpp_typ2doc(_tmpDD);_tmp119[0]=
Cyc_PP_text(_tag_arr("offsetof(",sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp119,
sizeof(struct Cyc_PP_Doc*),5));});goto _LL17F;_LL1A6: if(*((int*)_tmpBF)!= 18)goto
_LL1A8;_tmpE0=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpBF)->f1;_tmpE1=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpBF)->f2;if(*((int*)_tmpE1)!= 1)
goto _LL1A8;_tmpE2=((struct Cyc_Absyn_TupleIndex_struct*)_tmpE1)->f1;_LL1A7: s=({
struct Cyc_PP_Doc*_tmp11A[5];_tmp11A[4]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp11A[3]=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp11C;
_tmp11C.tag=1;_tmp11C.f1=(int)_tmpE2;{void*_tmp11B[1]={& _tmp11C};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(char),3),_tag_arr(_tmp11B,sizeof(void*),1));}}));_tmp11A[2]=
Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp11A[1]=Cyc_Absynpp_typ2doc(_tmpE0);
_tmp11A[0]=Cyc_PP_text(_tag_arr("offsetof(",sizeof(char),10));Cyc_PP_cat(
_tag_arr(_tmp11A,sizeof(struct Cyc_PP_Doc*),5));});goto _LL17F;_LL1A8: if(*((int*)
_tmpBF)!= 19)goto _LL1AA;_tmpE3=((struct Cyc_Absyn_Gentyp_e_struct*)_tmpBF)->f1;
_tmpE4=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmpBF)->f2;_LL1A9: s=({struct
Cyc_PP_Doc*_tmp11D[4];_tmp11D[3]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp11D[2]=Cyc_Absynpp_typ2doc(_tmpE4);_tmp11D[1]=Cyc_Absynpp_tvars2doc(_tmpE3);
_tmp11D[0]=Cyc_PP_text(_tag_arr("__gen(",sizeof(char),7));Cyc_PP_cat(_tag_arr(
_tmp11D,sizeof(struct Cyc_PP_Doc*),4));});goto _LL17F;_LL1AA: if(*((int*)_tmpBF)!= 
20)goto _LL1AC;_tmpE5=((struct Cyc_Absyn_Deref_e_struct*)_tmpBF)->f1;_LL1AB: s=({
struct Cyc_PP_Doc*_tmp11E[2];_tmp11E[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpE5);
_tmp11E[0]=Cyc_PP_text(_tag_arr("*",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp11E,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL17F;_LL1AC: if(*((int*)_tmpBF)!= 21)goto
_LL1AE;_tmpE6=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpBF)->f1;_tmpE7=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmpBF)->f2;_LL1AD: s=({struct Cyc_PP_Doc*_tmp11F[3];
_tmp11F[2]=Cyc_PP_textptr(_tmpE7);_tmp11F[1]=Cyc_PP_text(_tag_arr(".",sizeof(
char),2));_tmp11F[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpE6);Cyc_PP_cat(_tag_arr(
_tmp11F,sizeof(struct Cyc_PP_Doc*),3));});goto _LL17F;_LL1AE: if(*((int*)_tmpBF)!= 
22)goto _LL1B0;_tmpE8=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpBF)->f1;_tmpE9=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmpBF)->f2;_LL1AF: s=({struct Cyc_PP_Doc*
_tmp120[3];_tmp120[2]=Cyc_PP_textptr(_tmpE9);_tmp120[1]=Cyc_PP_text(_tag_arr("->",
sizeof(char),3));_tmp120[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpE8);Cyc_PP_cat(
_tag_arr(_tmp120,sizeof(struct Cyc_PP_Doc*),3));});goto _LL17F;_LL1B0: if(*((int*)
_tmpBF)!= 23)goto _LL1B2;_tmpEA=((struct Cyc_Absyn_Subscript_e_struct*)_tmpBF)->f1;
_tmpEB=((struct Cyc_Absyn_Subscript_e_struct*)_tmpBF)->f2;_LL1B1: s=({struct Cyc_PP_Doc*
_tmp121[4];_tmp121[3]=Cyc_PP_text(_tag_arr("]",sizeof(char),2));_tmp121[2]=Cyc_Absynpp_exp2doc(
_tmpEB);_tmp121[1]=Cyc_PP_text(_tag_arr("[",sizeof(char),2));_tmp121[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpEA);Cyc_PP_cat(_tag_arr(_tmp121,sizeof(struct Cyc_PP_Doc*),4));});goto
_LL17F;_LL1B2: if(*((int*)_tmpBF)!= 24)goto _LL1B4;_tmpEC=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpBF)->f1;_LL1B3: s=({struct Cyc_PP_Doc*_tmp122[3];_tmp122[2]=Cyc_PP_text(
_tag_arr(")",sizeof(char),2));_tmp122[1]=Cyc_Absynpp_exps2doc_prec(20,_tmpEC);
_tmp122[0]=Cyc_PP_text(_tag_arr("$(",sizeof(char),3));Cyc_PP_cat(_tag_arr(
_tmp122,sizeof(struct Cyc_PP_Doc*),3));});goto _LL17F;_LL1B4: if(*((int*)_tmpBF)!= 
25)goto _LL1B6;_tmpED=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpBF)->f1;_tmpEE=((
struct Cyc_Absyn_CompoundLit_e_struct*)_tmpBF)->f2;_LL1B5: s=({struct Cyc_PP_Doc*
_tmp123[4];_tmp123[3]=Cyc_PP_group(_tag_arr("{",sizeof(char),2),_tag_arr("}",
sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpEE));_tmp123[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp123[1]=Cyc_Absynpp_typ2doc((*
_tmpED).f3);_tmp123[0]=Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(
_tag_arr(_tmp123,sizeof(struct Cyc_PP_Doc*),4));});goto _LL17F;_LL1B6: if(*((int*)
_tmpBF)!= 26)goto _LL1B8;_tmpEF=((struct Cyc_Absyn_Array_e_struct*)_tmpBF)->f1;
_LL1B7: s=Cyc_PP_group(_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(char),2),
_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmpEF));
goto _LL17F;_LL1B8: if(*((int*)_tmpBF)!= 27)goto _LL1BA;_tmpF0=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpBF)->f1;_tmpF1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpBF)->f2;_tmpF2=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmpBF)->f3;_LL1B9: s=({struct Cyc_PP_Doc*
_tmp124[7];_tmp124[6]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp124[5]=Cyc_Absynpp_exp2doc(
_tmpF2);_tmp124[4]=Cyc_PP_text(_tag_arr(" : ",sizeof(char),4));_tmp124[3]=Cyc_Absynpp_exp2doc(
_tmpF1);_tmp124[2]=Cyc_PP_text(_tag_arr(" < ",sizeof(char),4));_tmp124[1]=Cyc_PP_text(*(*
_tmpF0->name).f2);_tmp124[0]=Cyc_PP_text(_tag_arr("{for ",sizeof(char),6));Cyc_PP_cat(
_tag_arr(_tmp124,sizeof(struct Cyc_PP_Doc*),7));});goto _LL17F;_LL1BA: if(*((int*)
_tmpBF)!= 28)goto _LL1BC;_tmpF3=((struct Cyc_Absyn_Struct_e_struct*)_tmpBF)->f1;
_tmpF4=((struct Cyc_Absyn_Struct_e_struct*)_tmpBF)->f2;_tmpF5=((struct Cyc_Absyn_Struct_e_struct*)
_tmpBF)->f3;_LL1BB: {struct Cyc_List_List*_tmp125=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpF5);s=({struct Cyc_PP_Doc*_tmp126[2];_tmp126[1]=Cyc_PP_group(_tag_arr("{",
sizeof(char),2),_tag_arr("}",sizeof(char),2),_tag_arr(",",sizeof(char),2),_tmpF4
!= 0?({struct Cyc_List_List*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->hd=Cyc_Absynpp_tps2doc(
_tmpF4);_tmp127->tl=_tmp125;_tmp127;}): _tmp125);_tmp126[0]=Cyc_Absynpp_qvar2doc(
_tmpF3);Cyc_PP_cat(_tag_arr(_tmp126,sizeof(struct Cyc_PP_Doc*),2));});goto _LL17F;}
_LL1BC: if(*((int*)_tmpBF)!= 29)goto _LL1BE;_tmpF6=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpBF)->f2;_LL1BD: s=Cyc_PP_group(_tag_arr("{",sizeof(char),2),_tag_arr("}",
sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpF6));goto _LL17F;_LL1BE: if(*((int*)_tmpBF)!= 30)goto _LL1C0;_tmpF7=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpBF)->f1;_tmpF8=((struct Cyc_Absyn_Tunion_e_struct*)_tmpBF)->f3;_LL1BF: if(
_tmpF7 == 0)s=Cyc_Absynpp_qvar2doc(_tmpF8->name);else{s=({struct Cyc_PP_Doc*
_tmp128[2];_tmp128[1]=Cyc_PP_egroup(_tag_arr("(",sizeof(char),2),_tag_arr(")",
sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,
_tmpF7));_tmp128[0]=Cyc_Absynpp_qvar2doc(_tmpF8->name);Cyc_PP_cat(_tag_arr(
_tmp128,sizeof(struct Cyc_PP_Doc*),2));});}goto _LL17F;_LL1C0: if(*((int*)_tmpBF)!= 
31)goto _LL1C2;_tmpF9=((struct Cyc_Absyn_Enum_e_struct*)_tmpBF)->f1;_LL1C1: s=Cyc_Absynpp_qvar2doc(
_tmpF9);goto _LL17F;_LL1C2: if(*((int*)_tmpBF)!= 32)goto _LL1C4;_tmpFA=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpBF)->f1;_LL1C3: s=Cyc_Absynpp_qvar2doc(_tmpFA);goto _LL17F;_LL1C4: if(*((int*)
_tmpBF)!= 33)goto _LL1C6;_tmpFB=((struct Cyc_Absyn_Malloc_e_struct*)_tmpBF)->f1;
_tmpFC=_tmpFB.is_calloc;_tmpFD=_tmpFB.rgn;_tmpFE=_tmpFB.elt_type;_tmpFF=_tmpFB.num_elts;
_LL1C5: if(_tmpFC){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)
_check_null(_tmpFE)),0);if(_tmpFD == 0)s=({struct Cyc_PP_Doc*_tmp129[5];_tmp129[4]=
Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp129[3]=Cyc_Absynpp_exp2doc(st);
_tmp129[2]=Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp129[1]=Cyc_Absynpp_exp2doc(
_tmpFF);_tmp129[0]=Cyc_PP_text(_tag_arr("calloc(",sizeof(char),8));Cyc_PP_cat(
_tag_arr(_tmp129,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp12A[7];_tmp12A[6]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp12A[5]=Cyc_Absynpp_exp2doc(
st);_tmp12A[4]=Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp12A[3]=Cyc_Absynpp_exp2doc(
_tmpFF);_tmp12A[2]=Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp12A[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(_tmpFD));_tmp12A[0]=Cyc_PP_text(_tag_arr("rcalloc(",
sizeof(char),9));Cyc_PP_cat(_tag_arr(_tmp12A,sizeof(struct Cyc_PP_Doc*),7));});}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmpFE == 0)new_e=_tmpFF;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmpFE,0),_tmpFF,0);}if(_tmpFD == 0)s=({struct Cyc_PP_Doc*
_tmp12B[3];_tmp12B[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp12B[1]=Cyc_Absynpp_exp2doc(
new_e);_tmp12B[0]=Cyc_PP_text(_tag_arr("malloc(",sizeof(char),8));Cyc_PP_cat(
_tag_arr(_tmp12B,sizeof(struct Cyc_PP_Doc*),3));});else{s=({struct Cyc_PP_Doc*
_tmp12C[5];_tmp12C[4]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp12C[3]=Cyc_Absynpp_exp2doc(
new_e);_tmp12C[2]=Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp12C[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(_tmpFD));_tmp12C[0]=Cyc_PP_text(_tag_arr("rmalloc(",
sizeof(char),9));Cyc_PP_cat(_tag_arr(_tmp12C,sizeof(struct Cyc_PP_Doc*),5));});}}
goto _LL17F;_LL1C6: if(*((int*)_tmpBF)!= 34)goto _LL1C8;_tmp100=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpBF)->f1;_tmp101=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpBF)->f2;_LL1C7:
s=Cyc_PP_group(_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(char),2),
_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp101));
goto _LL17F;_LL1C8: if(*((int*)_tmpBF)!= 35)goto _LL1CA;_tmp102=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpBF)->f1;_LL1C9: s=({struct Cyc_PP_Doc*_tmp12D[3];_tmp12D[2]=Cyc_PP_text(
_tag_arr(" })",sizeof(char),4));_tmp12D[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp102));_tmp12D[0]=Cyc_PP_text(_tag_arr("({ ",sizeof(char),4));Cyc_PP_cat(
_tag_arr(_tmp12D,sizeof(struct Cyc_PP_Doc*),3));});goto _LL17F;_LL1CA: if(*((int*)
_tmpBF)!= 36)goto _LL1CC;_tmp103=((struct Cyc_Absyn_Codegen_e_struct*)_tmpBF)->f1;
_LL1CB: s=({struct Cyc_PP_Doc*_tmp12E[3];_tmp12E[2]=Cyc_PP_text(_tag_arr(")",
sizeof(char),2));_tmp12E[1]=Cyc_PP_nest(2,Cyc_Absynpp_decl2doc(({struct Cyc_Absyn_Decl*
_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->r=(void*)((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp131;_tmp131.tag=1;_tmp131.f1=_tmp103;_tmp131;});_tmp130;}));_tmp12F->loc=e->loc;
_tmp12F;})));_tmp12E[0]=Cyc_PP_text(_tag_arr("codegen(",sizeof(char),9));Cyc_PP_cat(
_tag_arr(_tmp12E,sizeof(struct Cyc_PP_Doc*),3));});goto _LL17F;_LL1CC: if(*((int*)
_tmpBF)!= 37)goto _LL17F;_tmp104=((struct Cyc_Absyn_Fill_e_struct*)_tmpBF)->f1;
_LL1CD: s=({struct Cyc_PP_Doc*_tmp132[3];_tmp132[2]=Cyc_PP_text(_tag_arr(")",
sizeof(char),2));_tmp132[1]=Cyc_PP_nest(2,Cyc_Absynpp_exp2doc(_tmp104));_tmp132[
0]=Cyc_PP_text(_tag_arr("fill(",sizeof(char),6));Cyc_PP_cat(_tag_arr(_tmp132,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL17F;_LL17F:;}if(inprec >= myprec)s=({
struct Cyc_PP_Doc*_tmp133[3];_tmp133[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp133[1]=s;_tmp133[0]=Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(
_tag_arr(_tmp133,sizeof(struct Cyc_PP_Doc*),3));});return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d){void*_tmp134=d;struct Cyc_Absyn_Exp*_tmp135;struct _tagged_arr*_tmp136;
_LL1DB: if(*((int*)_tmp134)!= 0)goto _LL1DD;_tmp135=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp134)->f1;_LL1DC: return({struct Cyc_PP_Doc*_tmp137[3];_tmp137[2]=Cyc_PP_text(
_tag_arr("]",sizeof(char),2));_tmp137[1]=Cyc_Absynpp_exp2doc(_tmp135);_tmp137[0]=
Cyc_PP_text(_tag_arr(".[",sizeof(char),3));Cyc_PP_cat(_tag_arr(_tmp137,sizeof(
struct Cyc_PP_Doc*),3));});_LL1DD: if(*((int*)_tmp134)!= 1)goto _LL1DA;_tmp136=((
struct Cyc_Absyn_FieldName_struct*)_tmp134)->f1;_LL1DE: return({struct Cyc_PP_Doc*
_tmp138[2];_tmp138[1]=Cyc_PP_textptr(_tmp136);_tmp138[0]=Cyc_PP_text(_tag_arr(".",
sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp138,sizeof(struct Cyc_PP_Doc*),2));});
_LL1DA:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple8*de){if((*de).f1 == 0)
return Cyc_Absynpp_exp2doc((*de).f2);else{return({struct Cyc_PP_Doc*_tmp139[2];
_tmp139[1]=Cyc_Absynpp_exp2doc((*de).f2);_tmp139[0]=Cyc_PP_egroup(_tag_arr("",
sizeof(char),1),_tag_arr("=",sizeof(char),2),_tag_arr("=",sizeof(char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1));Cyc_PP_cat(_tag_arr(_tmp139,sizeof(struct
Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){return Cyc_PP_group(_tag_arr("",sizeof(char),1),_tag_arr("",
sizeof(char),1),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*c){void*_tmp13A=c;void*
_tmp13B;char _tmp13C;void*_tmp13D;short _tmp13E;void*_tmp13F;int _tmp140;void*
_tmp141;int _tmp142;void*_tmp143;long long _tmp144;struct _tagged_arr _tmp145;struct
_tagged_arr _tmp146;_LL1E0: if(_tmp13A <= (void*)1?1:*((int*)_tmp13A)!= 0)goto
_LL1E2;_tmp13B=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp13A)->f1;_tmp13C=((
struct Cyc_Absyn_Char_c_struct*)_tmp13A)->f2;_LL1E1: return Cyc_PP_text((struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp148;_tmp148.tag=0;_tmp148.f1=(
struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp13C);{void*_tmp147[1]={& _tmp148};
Cyc_Std_aprintf(_tag_arr("'%s'",sizeof(char),5),_tag_arr(_tmp147,sizeof(void*),1));}}));
_LL1E2: if(_tmp13A <= (void*)1?1:*((int*)_tmp13A)!= 1)goto _LL1E4;_tmp13D=(void*)((
struct Cyc_Absyn_Short_c_struct*)_tmp13A)->f1;_tmp13E=((struct Cyc_Absyn_Short_c_struct*)
_tmp13A)->f2;_LL1E3: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp14A;_tmp14A.tag=1;_tmp14A.f1=(int)((unsigned int)((int)_tmp13E));{void*
_tmp149[1]={& _tmp14A};Cyc_Std_aprintf(_tag_arr("%d",sizeof(char),3),_tag_arr(
_tmp149,sizeof(void*),1));}}));_LL1E4: if(_tmp13A <= (void*)1?1:*((int*)_tmp13A)!= 
2)goto _LL1E6;_tmp13F=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp13A)->f1;if((int)
_tmp13F != 0)goto _LL1E6;_tmp140=((struct Cyc_Absyn_Int_c_struct*)_tmp13A)->f2;
_LL1E5: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp14C;
_tmp14C.tag=1;_tmp14C.f1=(int)((unsigned int)_tmp140);{void*_tmp14B[1]={& _tmp14C};
Cyc_Std_aprintf(_tag_arr("%d",sizeof(char),3),_tag_arr(_tmp14B,sizeof(void*),1));}}));
_LL1E6: if(_tmp13A <= (void*)1?1:*((int*)_tmp13A)!= 2)goto _LL1E8;_tmp141=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp13A)->f1;if((int)_tmp141 != 1)goto _LL1E8;
_tmp142=((struct Cyc_Absyn_Int_c_struct*)_tmp13A)->f2;_LL1E7: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp14E;_tmp14E.tag=1;_tmp14E.f1=(
unsigned int)_tmp142;{void*_tmp14D[1]={& _tmp14E};Cyc_Std_aprintf(_tag_arr("%u",
sizeof(char),3),_tag_arr(_tmp14D,sizeof(void*),1));}}));_LL1E8: if(_tmp13A <= (
void*)1?1:*((int*)_tmp13A)!= 3)goto _LL1EA;_tmp143=(void*)((struct Cyc_Absyn_LongLong_c_struct*)
_tmp13A)->f1;_tmp144=((struct Cyc_Absyn_LongLong_c_struct*)_tmp13A)->f2;_LL1E9:
return Cyc_PP_text(_tag_arr("<<FIX LONG LONG CONSTANT>>",sizeof(char),27));_LL1EA:
if(_tmp13A <= (void*)1?1:*((int*)_tmp13A)!= 4)goto _LL1EC;_tmp145=((struct Cyc_Absyn_Float_c_struct*)
_tmp13A)->f1;_LL1EB: return Cyc_PP_text(_tmp145);_LL1EC: if((int)_tmp13A != 0)goto
_LL1EE;_LL1ED: return Cyc_PP_text(_tag_arr("NULL",sizeof(char),5));_LL1EE: if(
_tmp13A <= (void*)1?1:*((int*)_tmp13A)!= 5)goto _LL1DF;_tmp146=((struct Cyc_Absyn_String_c_struct*)
_tmp13A)->f1;_LL1EF: return({struct Cyc_PP_Doc*_tmp14F[3];_tmp14F[2]=Cyc_PP_text(
_tag_arr("\"",sizeof(char),2));_tmp14F[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(
_tmp146));_tmp14F[0]=Cyc_PP_text(_tag_arr("\"",sizeof(char),2));Cyc_PP_cat(
_tag_arr(_tmp14F,sizeof(struct Cyc_PP_Doc*),3));});_LL1DF:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(
int inprec,void*p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(
p);if(p == (void*)19){if(es == 0?1: es->tl != 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150[0]=({struct Cyc_Core_Failure_struct
_tmp151;_tmp151.tag=Cyc_Core_Failure;_tmp151.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp153;_tmp153.tag=0;_tmp153.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp152[1]={& _tmp153};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof(char),44),_tag_arr(_tmp152,sizeof(void*),1));}});_tmp151;});_tmp150;}));{
struct Cyc_PP_Doc*_tmp154=Cyc_Absynpp_exp2doc_prec(inprec,(struct Cyc_Absyn_Exp*)
es->hd);return({struct Cyc_PP_Doc*_tmp155[2];_tmp155[1]=Cyc_PP_text(_tag_arr(".size",
sizeof(char),6));_tmp155[0]=_tmp154;Cyc_PP_cat(_tag_arr(_tmp155,sizeof(struct Cyc_PP_Doc*),
2));});}}else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es);if(ds == 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp156=
_cycalloc(sizeof(*_tmp156));_tmp156[0]=({struct Cyc_Core_Failure_struct _tmp157;
_tmp157.tag=Cyc_Core_Failure;_tmp157.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp159;_tmp159.tag=0;_tmp159.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp158[1]={& _tmp159};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc: %s with no args",
sizeof(char),38),_tag_arr(_tmp158,sizeof(void*),1));}});_tmp157;});_tmp156;}));
else{if(ds->tl == 0)return({struct Cyc_PP_Doc*_tmp15A[3];_tmp15A[2]=(struct Cyc_PP_Doc*)
ds->hd;_tmp15A[1]=Cyc_PP_text(_tag_arr(" ",sizeof(char),2));_tmp15A[0]=ps;Cyc_PP_cat(
_tag_arr(_tmp15A,sizeof(struct Cyc_PP_Doc*),3));});else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B[0]=({struct Cyc_Core_Failure_struct
_tmp15C;_tmp15C.tag=Cyc_Core_Failure;_tmp15C.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp15E;_tmp15E.tag=0;_tmp15E.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp15D[1]={& _tmp15E};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof(char),47),_tag_arr(_tmp15D,sizeof(void*),1));}});_tmp15C;});_tmp15B;}));
else{return({struct Cyc_PP_Doc*_tmp15F[5];_tmp15F[4]=(struct Cyc_PP_Doc*)((struct
Cyc_List_List*)_check_null(ds->tl))->hd;_tmp15F[3]=Cyc_PP_text(_tag_arr(" ",
sizeof(char),2));_tmp15F[2]=ps;_tmp15F[1]=Cyc_PP_text(_tag_arr(" ",sizeof(char),
2));_tmp15F[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_arr(_tmp15F,sizeof(
struct Cyc_PP_Doc*),5));});}}}}}struct _tagged_arr Cyc_Absynpp_prim2str(void*p){
void*_tmp160=p;_LL1F1: if((int)_tmp160 != 0)goto _LL1F3;_LL1F2: return _tag_arr("+",
sizeof(char),2);_LL1F3: if((int)_tmp160 != 1)goto _LL1F5;_LL1F4: return _tag_arr("*",
sizeof(char),2);_LL1F5: if((int)_tmp160 != 2)goto _LL1F7;_LL1F6: return _tag_arr("-",
sizeof(char),2);_LL1F7: if((int)_tmp160 != 3)goto _LL1F9;_LL1F8: return _tag_arr("/",
sizeof(char),2);_LL1F9: if((int)_tmp160 != 4)goto _LL1FB;_LL1FA: return _tag_arr("%",
sizeof(char),2);_LL1FB: if((int)_tmp160 != 5)goto _LL1FD;_LL1FC: return _tag_arr("==",
sizeof(char),3);_LL1FD: if((int)_tmp160 != 6)goto _LL1FF;_LL1FE: return _tag_arr("!=",
sizeof(char),3);_LL1FF: if((int)_tmp160 != 7)goto _LL201;_LL200: return _tag_arr(">",
sizeof(char),2);_LL201: if((int)_tmp160 != 8)goto _LL203;_LL202: return _tag_arr("<",
sizeof(char),2);_LL203: if((int)_tmp160 != 9)goto _LL205;_LL204: return _tag_arr(">=",
sizeof(char),3);_LL205: if((int)_tmp160 != 10)goto _LL207;_LL206: return _tag_arr("<=",
sizeof(char),3);_LL207: if((int)_tmp160 != 11)goto _LL209;_LL208: return _tag_arr("!",
sizeof(char),2);_LL209: if((int)_tmp160 != 12)goto _LL20B;_LL20A: return _tag_arr("~",
sizeof(char),2);_LL20B: if((int)_tmp160 != 13)goto _LL20D;_LL20C: return _tag_arr("&",
sizeof(char),2);_LL20D: if((int)_tmp160 != 14)goto _LL20F;_LL20E: return _tag_arr("|",
sizeof(char),2);_LL20F: if((int)_tmp160 != 15)goto _LL211;_LL210: return _tag_arr("^",
sizeof(char),2);_LL211: if((int)_tmp160 != 16)goto _LL213;_LL212: return _tag_arr("<<",
sizeof(char),3);_LL213: if((int)_tmp160 != 17)goto _LL215;_LL214: return _tag_arr(">>",
sizeof(char),3);_LL215: if((int)_tmp160 != 18)goto _LL217;_LL216: return _tag_arr(">>>",
sizeof(char),4);_LL217: if((int)_tmp160 != 19)goto _LL1F0;_LL218: return _tag_arr("size",
sizeof(char),5);_LL1F0:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p){return Cyc_PP_text(
Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp161=(void*)s->r;_LL21A: if(_tmp161 <= (void*)1?1:*((int*)_tmp161)!= 12)
goto _LL21C;_LL21B: return 1;_LL21C:;_LL21D: return 0;_LL219:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st){struct Cyc_PP_Doc*s;{void*_tmp162=(void*)st->r;struct Cyc_Absyn_Exp*
_tmp163;struct Cyc_Absyn_Stmt*_tmp164;struct Cyc_Absyn_Stmt*_tmp165;struct Cyc_Absyn_Exp*
_tmp166;struct Cyc_Absyn_Exp*_tmp167;struct Cyc_Absyn_Stmt*_tmp168;struct Cyc_Absyn_Stmt*
_tmp169;struct _tuple2 _tmp16A;struct Cyc_Absyn_Exp*_tmp16B;struct Cyc_Absyn_Stmt*
_tmp16C;struct _tagged_arr*_tmp16D;struct Cyc_Absyn_Exp*_tmp16E;struct _tuple2
_tmp16F;struct Cyc_Absyn_Exp*_tmp170;struct _tuple2 _tmp171;struct Cyc_Absyn_Exp*
_tmp172;struct Cyc_Absyn_Stmt*_tmp173;struct Cyc_Absyn_ForArrayInfo _tmp174;struct
Cyc_List_List*_tmp175;struct _tuple2 _tmp176;struct Cyc_Absyn_Exp*_tmp177;struct
_tuple2 _tmp178;struct Cyc_Absyn_Exp*_tmp179;struct Cyc_Absyn_Stmt*_tmp17A;struct
Cyc_Absyn_Exp*_tmp17B;struct Cyc_List_List*_tmp17C;struct Cyc_Absyn_Exp*_tmp17D;
struct Cyc_List_List*_tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_List_List*
_tmp180;struct Cyc_Absyn_Decl*_tmp181;struct Cyc_Absyn_Stmt*_tmp182;struct Cyc_Absyn_Stmt*
_tmp183;struct Cyc_Absyn_Stmt*_tmp184;struct _tagged_arr*_tmp185;struct Cyc_Absyn_Stmt*
_tmp186;struct Cyc_Absyn_Stmt*_tmp187;struct _tuple2 _tmp188;struct Cyc_Absyn_Exp*
_tmp189;struct Cyc_Absyn_Stmt*_tmp18A;struct Cyc_List_List*_tmp18B;struct Cyc_Absyn_Tvar*
_tmp18C;struct Cyc_Absyn_Vardecl*_tmp18D;int _tmp18E;struct Cyc_Absyn_Stmt*_tmp18F;
struct Cyc_Absyn_Exp*_tmp190;_LL21F: if((int)_tmp162 != 0)goto _LL221;_LL220: s=Cyc_PP_text(
_tag_arr(";",sizeof(char),2));goto _LL21E;_LL221: if(_tmp162 <= (void*)1?1:*((int*)
_tmp162)!= 0)goto _LL223;_tmp163=((struct Cyc_Absyn_Exp_s_struct*)_tmp162)->f1;
_LL222: s=({struct Cyc_PP_Doc*_tmp191[2];_tmp191[1]=Cyc_PP_text(_tag_arr(";",
sizeof(char),2));_tmp191[0]=Cyc_Absynpp_exp2doc(_tmp163);Cyc_PP_cat(_tag_arr(
_tmp191,sizeof(struct Cyc_PP_Doc*),2));});goto _LL21E;_LL223: if(_tmp162 <= (void*)1?
1:*((int*)_tmp162)!= 1)goto _LL225;_tmp164=((struct Cyc_Absyn_Seq_s_struct*)
_tmp162)->f1;_tmp165=((struct Cyc_Absyn_Seq_s_struct*)_tmp162)->f2;_LL224: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp164))s=({struct Cyc_PP_Doc*_tmp192[6];_tmp192[5]=
Cyc_Absynpp_is_declaration(_tmp165)?({struct Cyc_PP_Doc*_tmp193[4];_tmp193[3]=Cyc_PP_line_doc();
_tmp193[2]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp193[1]=Cyc_PP_nest(2,
Cyc_Absynpp_stmt2doc(_tmp165));_tmp193[0]=Cyc_PP_text(_tag_arr("{ ",sizeof(char),
3));Cyc_PP_cat(_tag_arr(_tmp193,sizeof(struct Cyc_PP_Doc*),4));}): Cyc_Absynpp_stmt2doc(
_tmp165);_tmp192[4]=Cyc_PP_line_doc();_tmp192[3]=Cyc_PP_text(_tag_arr("}",
sizeof(char),2));_tmp192[2]=Cyc_PP_line_doc();_tmp192[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp164));_tmp192[0]=Cyc_PP_text(_tag_arr("{ ",sizeof(char),3));Cyc_PP_cat(
_tag_arr(_tmp192,sizeof(struct Cyc_PP_Doc*),6));});else{if(Cyc_Absynpp_is_declaration(
_tmp165))s=({struct Cyc_PP_Doc*_tmp194[6];_tmp194[5]=Cyc_PP_line_doc();_tmp194[4]=
Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp194[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp165));_tmp194[2]=Cyc_PP_text(_tag_arr("{ ",sizeof(char),3));_tmp194[1]=Cyc_PP_line_doc();
_tmp194[0]=Cyc_Absynpp_stmt2doc(_tmp164);Cyc_PP_cat(_tag_arr(_tmp194,sizeof(
struct Cyc_PP_Doc*),6));});else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,_tag_arr("",sizeof(char),1),({struct Cyc_List_List*_tmp195=
_cycalloc(sizeof(*_tmp195));_tmp195->hd=_tmp164;_tmp195->tl=({struct Cyc_List_List*
_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196->hd=_tmp165;_tmp196->tl=0;_tmp196;});
_tmp195;}));}}}else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,
_tag_arr("",sizeof(char),1),({struct Cyc_List_List*_tmp197=_cycalloc(sizeof(*
_tmp197));_tmp197->hd=_tmp164;_tmp197->tl=({struct Cyc_List_List*_tmp198=
_cycalloc(sizeof(*_tmp198));_tmp198->hd=_tmp165;_tmp198->tl=0;_tmp198;});_tmp197;}));}
goto _LL21E;_LL225: if(_tmp162 <= (void*)1?1:*((int*)_tmp162)!= 2)goto _LL227;
_tmp166=((struct Cyc_Absyn_Return_s_struct*)_tmp162)->f1;_LL226: if(_tmp166 == 0)s=
Cyc_PP_text(_tag_arr("return;",sizeof(char),8));else{s=({struct Cyc_PP_Doc*
_tmp199[3];_tmp199[2]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmp199[1]=
_tmp166 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmp166));_tmp199[0]=Cyc_PP_text(_tag_arr("return ",sizeof(char),8));
Cyc_PP_cat(_tag_arr(_tmp199,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL21E;_LL227:
if(_tmp162 <= (void*)1?1:*((int*)_tmp162)!= 3)goto _LL229;_tmp167=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp162)->f1;_tmp168=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp162)->f2;_tmp169=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp162)->f3;_LL228: {int print_else;{void*
_tmp19A=(void*)_tmp169->r;_LL24E: if((int)_tmp19A != 0)goto _LL250;_LL24F:
print_else=0;goto _LL24D;_LL250:;_LL251: print_else=1;goto _LL24D;_LL24D:;}s=({
struct Cyc_PP_Doc*_tmp19B[7];_tmp19B[6]=print_else?({struct Cyc_PP_Doc*_tmp19D[5];
_tmp19D[4]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp19D[3]=Cyc_PP_line_doc();
_tmp19D[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp19E[2];_tmp19E[1]=Cyc_Absynpp_stmt2doc(
_tmp169);_tmp19E[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp19E,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp19D[1]=Cyc_PP_text(_tag_arr("else {",sizeof(char),7));
_tmp19D[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp19D,sizeof(struct Cyc_PP_Doc*),
5));}): Cyc_PP_nil_doc();_tmp19B[5]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp19B[4]=Cyc_PP_line_doc();_tmp19B[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp19C[
2];_tmp19C[1]=Cyc_Absynpp_stmt2doc(_tmp168);_tmp19C[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp19C,sizeof(struct Cyc_PP_Doc*),2));}));_tmp19B[2]=Cyc_PP_text(
_tag_arr(") {",sizeof(char),4));_tmp19B[1]=Cyc_Absynpp_exp2doc(_tmp167);_tmp19B[
0]=Cyc_PP_text(_tag_arr("if (",sizeof(char),5));Cyc_PP_cat(_tag_arr(_tmp19B,
sizeof(struct Cyc_PP_Doc*),7));});goto _LL21E;}_LL229: if(_tmp162 <= (void*)1?1:*((
int*)_tmp162)!= 4)goto _LL22B;_tmp16A=((struct Cyc_Absyn_While_s_struct*)_tmp162)->f1;
_tmp16B=_tmp16A.f1;_tmp16C=((struct Cyc_Absyn_While_s_struct*)_tmp162)->f2;_LL22A:
s=({struct Cyc_PP_Doc*_tmp19F[6];_tmp19F[5]=Cyc_PP_text(_tag_arr("}",sizeof(char),
2));_tmp19F[4]=Cyc_PP_line_doc();_tmp19F[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp1A0[2];_tmp1A0[1]=Cyc_Absynpp_stmt2doc(_tmp16C);_tmp1A0[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp1A0,sizeof(struct Cyc_PP_Doc*),2));}));_tmp19F[2]=Cyc_PP_text(
_tag_arr(") {",sizeof(char),4));_tmp19F[1]=Cyc_Absynpp_exp2doc(_tmp16B);_tmp19F[
0]=Cyc_PP_text(_tag_arr("while (",sizeof(char),8));Cyc_PP_cat(_tag_arr(_tmp19F,
sizeof(struct Cyc_PP_Doc*),6));});goto _LL21E;_LL22B: if(_tmp162 <= (void*)1?1:*((
int*)_tmp162)!= 5)goto _LL22D;_LL22C: s=Cyc_PP_text(_tag_arr("break;",sizeof(char),
7));goto _LL21E;_LL22D: if(_tmp162 <= (void*)1?1:*((int*)_tmp162)!= 6)goto _LL22F;
_LL22E: s=Cyc_PP_text(_tag_arr("continue;",sizeof(char),10));goto _LL21E;_LL22F:
if(_tmp162 <= (void*)1?1:*((int*)_tmp162)!= 7)goto _LL231;_tmp16D=((struct Cyc_Absyn_Goto_s_struct*)
_tmp162)->f1;_LL230: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp1A2;_tmp1A2.tag=0;_tmp1A2.f1=(struct _tagged_arr)*_tmp16D;{void*_tmp1A1[1]={&
_tmp1A2};Cyc_Std_aprintf(_tag_arr("goto %s;",sizeof(char),9),_tag_arr(_tmp1A1,
sizeof(void*),1));}}));goto _LL21E;_LL231: if(_tmp162 <= (void*)1?1:*((int*)_tmp162)
!= 8)goto _LL233;_tmp16E=((struct Cyc_Absyn_For_s_struct*)_tmp162)->f1;_tmp16F=((
struct Cyc_Absyn_For_s_struct*)_tmp162)->f2;_tmp170=_tmp16F.f1;_tmp171=((struct
Cyc_Absyn_For_s_struct*)_tmp162)->f3;_tmp172=_tmp171.f1;_tmp173=((struct Cyc_Absyn_For_s_struct*)
_tmp162)->f4;_LL232: s=({struct Cyc_PP_Doc*_tmp1A3[10];_tmp1A3[9]=Cyc_PP_text(
_tag_arr("}",sizeof(char),2));_tmp1A3[8]=Cyc_PP_line_doc();_tmp1A3[7]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp1A4[2];_tmp1A4[1]=Cyc_Absynpp_stmt2doc(_tmp173);_tmp1A4[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1A4,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp1A3[6]=Cyc_PP_text(_tag_arr(") {",sizeof(char),4));_tmp1A3[5]=Cyc_Absynpp_exp2doc(
_tmp172);_tmp1A3[4]=Cyc_PP_text(_tag_arr("; ",sizeof(char),3));_tmp1A3[3]=Cyc_Absynpp_exp2doc(
_tmp170);_tmp1A3[2]=Cyc_PP_text(_tag_arr("; ",sizeof(char),3));_tmp1A3[1]=Cyc_Absynpp_exp2doc(
_tmp16E);_tmp1A3[0]=Cyc_PP_text(_tag_arr("for(",sizeof(char),5));Cyc_PP_cat(
_tag_arr(_tmp1A3,sizeof(struct Cyc_PP_Doc*),10));});goto _LL21E;_LL233: if(_tmp162
<= (void*)1?1:*((int*)_tmp162)!= 19)goto _LL235;_tmp174=((struct Cyc_Absyn_ForArray_s_struct*)
_tmp162)->f1;_tmp175=_tmp174.defns;_tmp176=_tmp174.condition;_tmp177=_tmp176.f1;
_tmp178=_tmp174.delta;_tmp179=_tmp178.f1;_tmp17A=_tmp174.body;_LL234: s=({struct
Cyc_PP_Doc*_tmp1A5[9];_tmp1A5[8]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp1A5[7]=Cyc_PP_line_doc();_tmp1A5[6]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1A6[
2];_tmp1A6[1]=Cyc_Absynpp_stmt2doc(_tmp17A);_tmp1A6[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp1A6,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1A5[5]=Cyc_PP_text(
_tag_arr(") {",sizeof(char),4));_tmp1A5[4]=Cyc_Absynpp_exp2doc(_tmp179);_tmp1A5[
3]=Cyc_PP_text(_tag_arr("; ",sizeof(char),3));_tmp1A5[2]=Cyc_Absynpp_exp2doc(
_tmp177);_tmp1A5[1]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_vardecl2doc,
_tag_arr(" ",sizeof(char),2),_tmp175);_tmp1A5[0]=Cyc_PP_text(_tag_arr("forarray(",
sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp1A5,sizeof(struct Cyc_PP_Doc*),9));});
goto _LL21E;_LL235: if(_tmp162 <= (void*)1?1:*((int*)_tmp162)!= 9)goto _LL237;
_tmp17B=((struct Cyc_Absyn_Switch_s_struct*)_tmp162)->f1;_tmp17C=((struct Cyc_Absyn_Switch_s_struct*)
_tmp162)->f2;_LL236: s=({struct Cyc_PP_Doc*_tmp1A7[7];_tmp1A7[6]=Cyc_PP_text(
_tag_arr("}",sizeof(char),2));_tmp1A7[5]=Cyc_PP_line_doc();_tmp1A7[4]=Cyc_Absynpp_switchclauses2doc(
_tmp17C);_tmp1A7[3]=Cyc_PP_line_doc();_tmp1A7[2]=Cyc_PP_text(_tag_arr(") {",
sizeof(char),4));_tmp1A7[1]=Cyc_Absynpp_exp2doc(_tmp17B);_tmp1A7[0]=Cyc_PP_text(
_tag_arr("switch (",sizeof(char),9));Cyc_PP_cat(_tag_arr(_tmp1A7,sizeof(struct
Cyc_PP_Doc*),7));});goto _LL21E;_LL237: if(_tmp162 <= (void*)1?1:*((int*)_tmp162)!= 
10)goto _LL239;_tmp17D=((struct Cyc_Absyn_SwitchC_s_struct*)_tmp162)->f1;_tmp17E=((
struct Cyc_Absyn_SwitchC_s_struct*)_tmp162)->f2;_LL238: s=({struct Cyc_PP_Doc*
_tmp1A8[7];_tmp1A8[6]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp1A8[5]=Cyc_PP_line_doc();
_tmp1A8[4]=Cyc_Absynpp_switchCclauses2doc(_tmp17E);_tmp1A8[3]=Cyc_PP_line_doc();
_tmp1A8[2]=Cyc_PP_text(_tag_arr(") {",sizeof(char),4));_tmp1A8[1]=Cyc_Absynpp_exp2doc(
_tmp17D);_tmp1A8[0]=Cyc_PP_text(_tag_arr("switch \"C\" (",sizeof(char),13));Cyc_PP_cat(
_tag_arr(_tmp1A8,sizeof(struct Cyc_PP_Doc*),7));});goto _LL21E;_LL239: if(_tmp162 <= (
void*)1?1:*((int*)_tmp162)!= 11)goto _LL23B;_tmp17F=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp162)->f1;if(_tmp17F != 0)goto _LL23B;_LL23A: s=Cyc_PP_text(_tag_arr("fallthru;",
sizeof(char),10));goto _LL21E;_LL23B: if(_tmp162 <= (void*)1?1:*((int*)_tmp162)!= 
11)goto _LL23D;_tmp180=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp162)->f1;_LL23C: s=({
struct Cyc_PP_Doc*_tmp1A9[3];_tmp1A9[2]=Cyc_PP_text(_tag_arr(");",sizeof(char),3));
_tmp1A9[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp180);_tmp1A9[0]=Cyc_PP_text(_tag_arr("fallthru(",
sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp1A9,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL21E;_LL23D: if(_tmp162 <= (void*)1?1:*((int*)_tmp162)!= 12)goto _LL23F;
_tmp181=((struct Cyc_Absyn_Decl_s_struct*)_tmp162)->f1;_tmp182=((struct Cyc_Absyn_Decl_s_struct*)
_tmp162)->f2;_LL23E: s=({struct Cyc_PP_Doc*_tmp1AA[3];_tmp1AA[2]=Cyc_Absynpp_stmt2doc(
_tmp182);_tmp1AA[1]=Cyc_PP_line_doc();_tmp1AA[0]=Cyc_Absynpp_decl2doc(_tmp181);
Cyc_PP_cat(_tag_arr(_tmp1AA,sizeof(struct Cyc_PP_Doc*),3));});goto _LL21E;_LL23F:
if(_tmp162 <= (void*)1?1:*((int*)_tmp162)!= 13)goto _LL241;_tmp183=((struct Cyc_Absyn_Cut_s_struct*)
_tmp162)->f1;_LL240: s=({struct Cyc_PP_Doc*_tmp1AB[2];_tmp1AB[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp183));_tmp1AB[0]=Cyc_PP_text(_tag_arr("cut ",sizeof(char),5));Cyc_PP_cat(
_tag_arr(_tmp1AB,sizeof(struct Cyc_PP_Doc*),2));});goto _LL21E;_LL241: if(_tmp162 <= (
void*)1?1:*((int*)_tmp162)!= 14)goto _LL243;_tmp184=((struct Cyc_Absyn_Splice_s_struct*)
_tmp162)->f1;_LL242: s=({struct Cyc_PP_Doc*_tmp1AC[2];_tmp1AC[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp184));_tmp1AC[0]=Cyc_PP_text(_tag_arr("splice ",sizeof(char),8));Cyc_PP_cat(
_tag_arr(_tmp1AC,sizeof(struct Cyc_PP_Doc*),2));});goto _LL21E;_LL243: if(_tmp162 <= (
void*)1?1:*((int*)_tmp162)!= 15)goto _LL245;_tmp185=((struct Cyc_Absyn_Label_s_struct*)
_tmp162)->f1;_tmp186=((struct Cyc_Absyn_Label_s_struct*)_tmp162)->f2;_LL244: if(
Cyc_Absynpp_decls_first?Cyc_Absynpp_is_declaration(_tmp186): 0)s=({struct Cyc_PP_Doc*
_tmp1AD[6];_tmp1AD[5]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp1AD[4]=Cyc_PP_line_doc();
_tmp1AD[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp186));_tmp1AD[2]=Cyc_PP_line_doc();
_tmp1AD[1]=Cyc_PP_text(_tag_arr(": {",sizeof(char),4));_tmp1AD[0]=Cyc_PP_textptr(
_tmp185);Cyc_PP_cat(_tag_arr(_tmp1AD,sizeof(struct Cyc_PP_Doc*),6));});else{s=({
struct Cyc_PP_Doc*_tmp1AE[3];_tmp1AE[2]=Cyc_Absynpp_stmt2doc(_tmp186);_tmp1AE[1]=
Cyc_PP_text(_tag_arr(": ",sizeof(char),3));_tmp1AE[0]=Cyc_PP_textptr(_tmp185);
Cyc_PP_cat(_tag_arr(_tmp1AE,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL21E;_LL245:
if(_tmp162 <= (void*)1?1:*((int*)_tmp162)!= 16)goto _LL247;_tmp187=((struct Cyc_Absyn_Do_s_struct*)
_tmp162)->f1;_tmp188=((struct Cyc_Absyn_Do_s_struct*)_tmp162)->f2;_tmp189=_tmp188.f1;
_LL246: s=({struct Cyc_PP_Doc*_tmp1AF[7];_tmp1AF[6]=Cyc_PP_text(_tag_arr(");",
sizeof(char),3));_tmp1AF[5]=Cyc_Absynpp_exp2doc(_tmp189);_tmp1AF[4]=Cyc_PP_text(
_tag_arr("} while (",sizeof(char),10));_tmp1AF[3]=Cyc_PP_line_doc();_tmp1AF[2]=
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp187));_tmp1AF[1]=Cyc_PP_line_doc();
_tmp1AF[0]=Cyc_PP_text(_tag_arr("do {",sizeof(char),5));Cyc_PP_cat(_tag_arr(
_tmp1AF,sizeof(struct Cyc_PP_Doc*),7));});goto _LL21E;_LL247: if(_tmp162 <= (void*)1?
1:*((int*)_tmp162)!= 17)goto _LL249;_tmp18A=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp162)->f1;_tmp18B=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp162)->f2;_LL248: s=({
struct Cyc_PP_Doc*_tmp1B0[9];_tmp1B0[8]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp1B0[7]=Cyc_PP_line_doc();_tmp1B0[6]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp18B));_tmp1B0[5]=Cyc_PP_line_doc();_tmp1B0[4]=Cyc_PP_text(_tag_arr("} catch {",
sizeof(char),10));_tmp1B0[3]=Cyc_PP_line_doc();_tmp1B0[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp18A));_tmp1B0[1]=Cyc_PP_line_doc();_tmp1B0[0]=Cyc_PP_text(_tag_arr("try {",
sizeof(char),6));Cyc_PP_cat(_tag_arr(_tmp1B0,sizeof(struct Cyc_PP_Doc*),9));});
goto _LL21E;_LL249: if(_tmp162 <= (void*)1?1:*((int*)_tmp162)!= 18)goto _LL24B;
_tmp18C=((struct Cyc_Absyn_Region_s_struct*)_tmp162)->f1;_tmp18D=((struct Cyc_Absyn_Region_s_struct*)
_tmp162)->f2;_tmp18E=((struct Cyc_Absyn_Region_s_struct*)_tmp162)->f3;_tmp18F=((
struct Cyc_Absyn_Region_s_struct*)_tmp162)->f4;_LL24A: s=({struct Cyc_PP_Doc*
_tmp1B1[11];_tmp1B1[10]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp1B1[9]=Cyc_PP_line_doc();
_tmp1B1[8]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp18F));_tmp1B1[7]=Cyc_PP_line_doc();
_tmp1B1[6]=Cyc_PP_text(_tag_arr("{",sizeof(char),2));_tmp1B1[5]=Cyc_Absynpp_qvar2doc(
_tmp18D->name);_tmp1B1[4]=Cyc_PP_text(_tag_arr(">",sizeof(char),2));_tmp1B1[3]=
Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp18C));_tmp1B1[2]=Cyc_PP_text(_tag_arr("<",
sizeof(char),2));_tmp1B1[1]=_tmp18E?Cyc_PP_text(_tag_arr("",sizeof(char),1)): Cyc_PP_text(
_tag_arr("[resetable]",sizeof(char),12));_tmp1B1[0]=Cyc_PP_text(_tag_arr("region",
sizeof(char),7));Cyc_PP_cat(_tag_arr(_tmp1B1,sizeof(struct Cyc_PP_Doc*),11));});
goto _LL21E;_LL24B: if(_tmp162 <= (void*)1?1:*((int*)_tmp162)!= 20)goto _LL21E;
_tmp190=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp162)->f1;_LL24C: s=({struct
Cyc_PP_Doc*_tmp1B2[3];_tmp1B2[2]=Cyc_PP_text(_tag_arr(");",sizeof(char),3));
_tmp1B2[1]=Cyc_Absynpp_exp2doc(_tmp190);_tmp1B2[0]=Cyc_PP_text(_tag_arr("reset_region(",
sizeof(char),14));Cyc_PP_cat(_tag_arr(_tmp1B2,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL21E;_LL21E:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp1B3=(void*)p->r;void*_tmp1B4;int _tmp1B5;char
_tmp1B6;struct _tagged_arr _tmp1B7;struct Cyc_Absyn_Vardecl*_tmp1B8;struct Cyc_List_List*
_tmp1B9;struct Cyc_Absyn_Pat*_tmp1BA;struct Cyc_Absyn_Vardecl*_tmp1BB;struct
_tuple0*_tmp1BC;struct _tuple0*_tmp1BD;struct Cyc_List_List*_tmp1BE;struct Cyc_Absyn_AggrInfo
_tmp1BF;void*_tmp1C0;struct Cyc_List_List*_tmp1C1;struct Cyc_List_List*_tmp1C2;
struct Cyc_Absyn_Enumfield*_tmp1C3;struct Cyc_Absyn_Enumfield*_tmp1C4;struct Cyc_Absyn_Tunionfield*
_tmp1C5;struct Cyc_List_List*_tmp1C6;struct Cyc_Absyn_Tunionfield*_tmp1C7;struct
Cyc_List_List*_tmp1C8;_LL253: if((int)_tmp1B3 != 0)goto _LL255;_LL254: s=Cyc_PP_text(
_tag_arr("_",sizeof(char),2));goto _LL252;_LL255: if((int)_tmp1B3 != 1)goto _LL257;
_LL256: s=Cyc_PP_text(_tag_arr("NULL",sizeof(char),5));goto _LL252;_LL257: if(
_tmp1B3 <= (void*)2?1:*((int*)_tmp1B3)!= 1)goto _LL259;_tmp1B4=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp1B3)->f1;_tmp1B5=((struct Cyc_Absyn_Int_p_struct*)_tmp1B3)->f2;_LL258: if(
_tmp1B4 == (void*)0)s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp1CA;_tmp1CA.tag=1;_tmp1CA.f1=(int)((unsigned int)_tmp1B5);{void*_tmp1C9[1]={&
_tmp1CA};Cyc_Std_aprintf(_tag_arr("%d",sizeof(char),3),_tag_arr(_tmp1C9,sizeof(
void*),1));}}));else{s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp1CC;_tmp1CC.tag=1;_tmp1CC.f1=(unsigned int)_tmp1B5;{void*_tmp1CB[1]={&
_tmp1CC};Cyc_Std_aprintf(_tag_arr("%u",sizeof(char),3),_tag_arr(_tmp1CB,sizeof(
void*),1));}}));}goto _LL252;_LL259: if(_tmp1B3 <= (void*)2?1:*((int*)_tmp1B3)!= 2)
goto _LL25B;_tmp1B6=((struct Cyc_Absyn_Char_p_struct*)_tmp1B3)->f1;_LL25A: s=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp1CE;_tmp1CE.tag=0;_tmp1CE.f1=(
struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp1B6);{void*_tmp1CD[1]={& _tmp1CE};
Cyc_Std_aprintf(_tag_arr("'%s'",sizeof(char),5),_tag_arr(_tmp1CD,sizeof(void*),1));}}));
goto _LL252;_LL25B: if(_tmp1B3 <= (void*)2?1:*((int*)_tmp1B3)!= 3)goto _LL25D;
_tmp1B7=((struct Cyc_Absyn_Float_p_struct*)_tmp1B3)->f1;_LL25C: s=Cyc_PP_text(
_tmp1B7);goto _LL252;_LL25D: if(_tmp1B3 <= (void*)2?1:*((int*)_tmp1B3)!= 0)goto
_LL25F;_tmp1B8=((struct Cyc_Absyn_Var_p_struct*)_tmp1B3)->f1;_LL25E: s=Cyc_Absynpp_qvar2doc(
_tmp1B8->name);goto _LL252;_LL25F: if(_tmp1B3 <= (void*)2?1:*((int*)_tmp1B3)!= 4)
goto _LL261;_tmp1B9=((struct Cyc_Absyn_Tuple_p_struct*)_tmp1B3)->f1;_LL260: s=({
struct Cyc_PP_Doc*_tmp1CF[3];_tmp1CF[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp1CF[1]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,
_tag_arr(",",sizeof(char),2),_tmp1B9);_tmp1CF[0]=Cyc_PP_text(_tag_arr("$(",
sizeof(char),3));Cyc_PP_cat(_tag_arr(_tmp1CF,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL252;_LL261: if(_tmp1B3 <= (void*)2?1:*((int*)_tmp1B3)!= 5)goto _LL263;
_tmp1BA=((struct Cyc_Absyn_Pointer_p_struct*)_tmp1B3)->f1;_LL262: s=({struct Cyc_PP_Doc*
_tmp1D0[2];_tmp1D0[1]=Cyc_Absynpp_pat2doc(_tmp1BA);_tmp1D0[0]=Cyc_PP_text(
_tag_arr("&",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp1D0,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL252;_LL263: if(_tmp1B3 <= (void*)2?1:*((int*)_tmp1B3)!= 6)goto _LL265;
_tmp1BB=((struct Cyc_Absyn_Reference_p_struct*)_tmp1B3)->f1;_LL264: s=({struct Cyc_PP_Doc*
_tmp1D1[2];_tmp1D1[1]=Cyc_Absynpp_qvar2doc(_tmp1BB->name);_tmp1D1[0]=Cyc_PP_text(
_tag_arr("*",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp1D1,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL252;_LL265: if(_tmp1B3 <= (void*)2?1:*((int*)_tmp1B3)!= 11)goto
_LL267;_tmp1BC=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp1B3)->f1;_LL266: s=Cyc_Absynpp_qvar2doc(
_tmp1BC);goto _LL252;_LL267: if(_tmp1B3 <= (void*)2?1:*((int*)_tmp1B3)!= 12)goto
_LL269;_tmp1BD=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1B3)->f1;_tmp1BE=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1B3)->f2;_LL268: s=({struct Cyc_PP_Doc*
_tmp1D2[2];_tmp1D2[1]=Cyc_PP_group(_tag_arr("(",sizeof(char),2),_tag_arr(")",
sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp1BE));_tmp1D2[0]=Cyc_Absynpp_qvar2doc(_tmp1BD);Cyc_PP_cat(_tag_arr(_tmp1D2,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL252;_LL269: if(_tmp1B3 <= (void*)2?1:*((
int*)_tmp1B3)!= 7)goto _LL26B;_tmp1BF=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1B3)->f1;
_tmp1C0=(void*)_tmp1BF.aggr_info;_tmp1C1=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp1B3)->f2;_tmp1C2=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1B3)->f3;_LL26A: {
struct _tuple0*_tmp1D4;struct _tuple3 _tmp1D3=Cyc_Absyn_aggr_kinded_name(_tmp1C0);
_tmp1D4=_tmp1D3.f2;s=({struct Cyc_PP_Doc*_tmp1D5[4];_tmp1D5[3]=Cyc_PP_group(
_tag_arr("",sizeof(char),1),_tag_arr("}",sizeof(char),2),_tag_arr(",",sizeof(
char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple7*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp1C2));_tmp1D5[2]=Cyc_PP_egroup(
_tag_arr("[",sizeof(char),2),_tag_arr("]",sizeof(char),2),_tag_arr(",",sizeof(
char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_get_name,_tmp1C1)));_tmp1D5[1]=Cyc_PP_text(_tag_arr("{",sizeof(char),
2));_tmp1D5[0]=Cyc_Absynpp_qvar2doc(_tmp1D4);Cyc_PP_cat(_tag_arr(_tmp1D5,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL252;}_LL26B: if(_tmp1B3 <= (void*)2?1:*((int*)
_tmp1B3)!= 9)goto _LL26D;_tmp1C3=((struct Cyc_Absyn_Enum_p_struct*)_tmp1B3)->f2;
_LL26C: _tmp1C4=_tmp1C3;goto _LL26E;_LL26D: if(_tmp1B3 <= (void*)2?1:*((int*)_tmp1B3)
!= 10)goto _LL26F;_tmp1C4=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp1B3)->f2;
_LL26E: s=Cyc_Absynpp_qvar2doc(_tmp1C4->name);goto _LL252;_LL26F: if(_tmp1B3 <= (
void*)2?1:*((int*)_tmp1B3)!= 8)goto _LL271;_tmp1C5=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1B3)->f2;_tmp1C6=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1B3)->f3;if(_tmp1C6
!= 0)goto _LL271;_LL270: s=Cyc_Absynpp_qvar2doc(_tmp1C5->name);goto _LL252;_LL271:
if(_tmp1B3 <= (void*)2?1:*((int*)_tmp1B3)!= 8)goto _LL252;_tmp1C7=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1B3)->f2;_tmp1C8=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1B3)->f3;_LL272: s=({
struct Cyc_PP_Doc*_tmp1D6[2];_tmp1D6[1]=Cyc_PP_egroup(_tag_arr("(",sizeof(char),2),
_tag_arr(")",sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp1C8));_tmp1D6[0]=Cyc_Absynpp_qvar2doc(_tmp1C7->name);Cyc_PP_cat(
_tag_arr(_tmp1D6,sizeof(struct Cyc_PP_Doc*),2));});goto _LL252;_LL252:;}return s;}
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple7*dp){return({struct Cyc_PP_Doc*
_tmp1D7[2];_tmp1D7[1]=Cyc_Absynpp_pat2doc((*dp).f2);_tmp1D7[0]=Cyc_PP_egroup(
_tag_arr("",sizeof(char),1),_tag_arr("=",sizeof(char),2),_tag_arr("=",sizeof(
char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));Cyc_PP_cat(_tag_arr(
_tmp1D7,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0?(void*)(c->pattern)->r == (
void*)0: 0)return({struct Cyc_PP_Doc*_tmp1D8[2];_tmp1D8[1]=Cyc_PP_nest(2,({struct
Cyc_PP_Doc*_tmp1D9[2];_tmp1D9[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1D9[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp1D9,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1D8[0]=Cyc_PP_text(
_tag_arr("default: ",sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp1D8,sizeof(struct
Cyc_PP_Doc*),2));});else{if(c->where_clause == 0)return({struct Cyc_PP_Doc*_tmp1DA[
4];_tmp1DA[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1DB[2];_tmp1DB[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp1DB[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1DB,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp1DA[2]=Cyc_PP_text(_tag_arr(": ",sizeof(char),3));
_tmp1DA[1]=Cyc_Absynpp_pat2doc(c->pattern);_tmp1DA[0]=Cyc_PP_text(_tag_arr("case ",
sizeof(char),6));Cyc_PP_cat(_tag_arr(_tmp1DA,sizeof(struct Cyc_PP_Doc*),4));});
else{return({struct Cyc_PP_Doc*_tmp1DC[6];_tmp1DC[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp1DD[2];_tmp1DD[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1DD[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp1DD,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1DC[4]=Cyc_PP_text(
_tag_arr(": ",sizeof(char),3));_tmp1DC[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(c->where_clause));_tmp1DC[2]=Cyc_PP_text(_tag_arr(" && ",sizeof(char),
5));_tmp1DC[1]=Cyc_Absynpp_pat2doc(c->pattern);_tmp1DC[0]=Cyc_PP_text(_tag_arr("case ",
sizeof(char),6));Cyc_PP_cat(_tag_arr(_tmp1DC,sizeof(struct Cyc_PP_Doc*),6));});}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,
_tag_arr("",sizeof(char),1),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause*c){struct Cyc_Absyn_SwitchC_clause _tmp1DF;struct
Cyc_Absyn_Exp*_tmp1E0;struct Cyc_Absyn_Stmt*_tmp1E1;struct Cyc_Absyn_SwitchC_clause*
_tmp1DE=c;_tmp1DF=*_tmp1DE;_tmp1E0=_tmp1DF.cnst_exp;_tmp1E1=_tmp1DF.body;if(
_tmp1E0 == 0)return({struct Cyc_PP_Doc*_tmp1E2[2];_tmp1E2[1]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp1E3[2];_tmp1E3[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1E3[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1E3,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp1E2[0]=Cyc_PP_text(_tag_arr("default: ",sizeof(char),10));Cyc_PP_cat(
_tag_arr(_tmp1E2,sizeof(struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*
_tmp1E4[4];_tmp1E4[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1E5[2];_tmp1E5[1]=Cyc_Absynpp_stmt2doc(
_tmp1E1);_tmp1E5[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1E5,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp1E4[2]=Cyc_PP_text(_tag_arr(": ",sizeof(char),3));
_tmp1E4[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(_tmp1E0));
_tmp1E4[0]=Cyc_PP_text(_tag_arr("case ",sizeof(char),6));Cyc_PP_cat(_tag_arr(
_tmp1E4,sizeof(struct Cyc_PP_Doc*),4));});}}struct Cyc_PP_Doc*Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_SwitchC_clause*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_switchCclause2doc,_tag_arr("",sizeof(char),1),cs);}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(
f->name);else{return({struct Cyc_PP_Doc*_tmp1E6[3];_tmp1E6[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp1E6[1]=Cyc_PP_text(_tag_arr(" = ",
sizeof(char),4));_tmp1E6[0]=Cyc_Absynpp_qvar2doc(f->name);Cyc_PP_cat(_tag_arr(
_tmp1E6,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Enumfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_enumfield2doc,_tag_arr(",",sizeof(char),2),fs);}static struct Cyc_PP_Doc*
Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(v->name);}
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tag_arr(",",
sizeof(char),2),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd){struct Cyc_Absyn_Vardecl _tmp1E8;void*_tmp1E9;struct _tuple0*_tmp1EA;struct Cyc_Absyn_Tqual
_tmp1EB;void*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_List_List*_tmp1EE;
struct Cyc_Absyn_Vardecl*_tmp1E7=vd;_tmp1E8=*_tmp1E7;_tmp1E9=(void*)_tmp1E8.sc;
_tmp1EA=_tmp1E8.name;_tmp1EB=_tmp1E8.tq;_tmp1EC=(void*)_tmp1E8.type;_tmp1ED=
_tmp1E8.initializer;_tmp1EE=_tmp1E8.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*
sn=Cyc_Absynpp_typedef_name2doc(_tmp1EA);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp1EE);struct Cyc_PP_Doc*beforenamedoc;if(!Cyc_Absynpp_to_VC)beforenamedoc=
attsdoc;else{void*_tmp1EF=Cyc_Tcutil_compress(_tmp1EC);struct Cyc_Absyn_FnInfo
_tmp1F0;struct Cyc_List_List*_tmp1F1;_LL274: if(_tmp1EF <= (void*)3?1:*((int*)
_tmp1EF)!= 8)goto _LL276;_tmp1F0=((struct Cyc_Absyn_FnType_struct*)_tmp1EF)->f1;
_tmp1F1=_tmp1F0.attributes;_LL275: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp1F1);
goto _LL273;_LL276:;_LL277: beforenamedoc=Cyc_PP_nil_doc();goto _LL273;_LL273:;}s=({
struct Cyc_PP_Doc*_tmp1F2[5];_tmp1F2[4]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));
_tmp1F2[3]=_tmp1ED == 0?Cyc_PP_text(_tag_arr("",sizeof(char),1)):({struct Cyc_PP_Doc*
_tmp1F5[2];_tmp1F5[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(
_tmp1ED));_tmp1F5[0]=Cyc_PP_text(_tag_arr(" = ",sizeof(char),4));Cyc_PP_cat(
_tag_arr(_tmp1F5,sizeof(struct Cyc_PP_Doc*),2));});_tmp1F2[2]=Cyc_Absynpp_tqtd2doc(
_tmp1EB,_tmp1EC,({struct Cyc_Core_Opt*_tmp1F3=_cycalloc(sizeof(*_tmp1F3));_tmp1F3->v=({
struct Cyc_PP_Doc*_tmp1F4[2];_tmp1F4[1]=sn;_tmp1F4[0]=beforenamedoc;Cyc_PP_cat(
_tag_arr(_tmp1F4,sizeof(struct Cyc_PP_Doc*),2));});_tmp1F3;}));_tmp1F2[1]=Cyc_Absynpp_scope2doc(
_tmp1E9);_tmp1F2[0]=Cyc_Absynpp_to_VC?attsdoc: Cyc_PP_nil_doc();Cyc_PP_cat(
_tag_arr(_tmp1F2,sizeof(struct Cyc_PP_Doc*),5));});return s;}}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp1F6=(void*)d->r;struct Cyc_Absyn_Fndecl*
_tmp1F7;struct Cyc_Absyn_Aggrdecl*_tmp1F8;struct Cyc_Absyn_Vardecl*_tmp1F9;struct
Cyc_Absyn_Tuniondecl*_tmp1FA;struct Cyc_Absyn_Tuniondecl _tmp1FB;void*_tmp1FC;
struct _tuple0*_tmp1FD;struct Cyc_List_List*_tmp1FE;struct Cyc_Core_Opt*_tmp1FF;int
_tmp200;struct Cyc_Absyn_Pat*_tmp201;struct Cyc_Absyn_Exp*_tmp202;struct Cyc_List_List*
_tmp203;struct Cyc_Absyn_Enumdecl*_tmp204;struct Cyc_Absyn_Enumdecl _tmp205;void*
_tmp206;struct _tuple0*_tmp207;struct Cyc_Core_Opt*_tmp208;struct Cyc_Absyn_Typedefdecl*
_tmp209;struct _tagged_arr*_tmp20A;struct Cyc_List_List*_tmp20B;struct _tuple0*
_tmp20C;struct Cyc_List_List*_tmp20D;struct Cyc_List_List*_tmp20E;_LL279: if(*((int*)
_tmp1F6)!= 1)goto _LL27B;_tmp1F7=((struct Cyc_Absyn_Fn_d_struct*)_tmp1F6)->f1;
_LL27A: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp216=_cycalloc(sizeof(*
_tmp216));_tmp216[0]=({struct Cyc_Absyn_FnType_struct _tmp217;_tmp217.tag=8;
_tmp217.f1=({struct Cyc_Absyn_FnInfo _tmp218;_tmp218.tvars=_tmp1F7->tvs;_tmp218.effect=
_tmp1F7->effect;_tmp218.ret_typ=(void*)((void*)_tmp1F7->ret_type);_tmp218.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple5*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp1F7->args);_tmp218.c_varargs=_tmp1F7->c_varargs;
_tmp218.cyc_varargs=_tmp1F7->cyc_varargs;_tmp218.rgn_po=_tmp1F7->rgn_po;_tmp218.attributes=
0;_tmp218;});_tmp217;});_tmp216;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp1F7->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp1F7->is_inline){if(Cyc_Absynpp_to_VC)
inlinedoc=Cyc_PP_text(_tag_arr("__inline ",sizeof(char),10));else{inlinedoc=Cyc_PP_text(
_tag_arr("inline ",sizeof(char),8));}}else{inlinedoc=Cyc_PP_nil_doc();}{struct
Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((void*)_tmp1F7->sc);struct Cyc_PP_Doc*
beforenamedoc=Cyc_Absynpp_to_VC?Cyc_Absynpp_callconv2doc(_tmp1F7->attributes):
attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp1F7->name);
struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual _tmp213;
_tmp213.q_const=0;_tmp213.q_volatile=0;_tmp213.q_restrict=0;_tmp213;}),t,({
struct Cyc_Core_Opt*_tmp214=_cycalloc(sizeof(*_tmp214));_tmp214->v=({struct Cyc_PP_Doc*
_tmp215[2];_tmp215[1]=namedoc;_tmp215[0]=beforenamedoc;Cyc_PP_cat(_tag_arr(
_tmp215,sizeof(struct Cyc_PP_Doc*),2));});_tmp214;}));struct Cyc_PP_Doc*bodydoc=({
struct Cyc_PP_Doc*_tmp211[4];_tmp211[3]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp211[2]=Cyc_PP_line_doc();_tmp211[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp212[
2];_tmp212[1]=Cyc_Absynpp_stmt2doc(_tmp1F7->body);_tmp212[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp212,sizeof(struct Cyc_PP_Doc*),2));}));_tmp211[0]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));Cyc_PP_cat(_tag_arr(_tmp211,sizeof(struct Cyc_PP_Doc*),
4));});s=({struct Cyc_PP_Doc*_tmp20F[4];_tmp20F[3]=bodydoc;_tmp20F[2]=tqtddoc;
_tmp20F[1]=scopedoc;_tmp20F[0]=inlinedoc;Cyc_PP_cat(_tag_arr(_tmp20F,sizeof(
struct Cyc_PP_Doc*),4));});if(Cyc_Absynpp_to_VC)s=({struct Cyc_PP_Doc*_tmp210[2];
_tmp210[1]=s;_tmp210[0]=attsdoc;Cyc_PP_cat(_tag_arr(_tmp210,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL278;}}_LL27B: if(*((int*)_tmp1F6)!= 4)goto _LL27D;_tmp1F8=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp1F6)->f1;_LL27C: if(_tmp1F8->impl == 0)s=({struct Cyc_PP_Doc*
_tmp219[5];_tmp219[4]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmp219[3]=Cyc_Absynpp_ktvars2doc(
_tmp1F8->tvs);_tmp219[2]=Cyc_Absynpp_qvar2doc(_tmp1F8->name);_tmp219[1]=Cyc_Absynpp_aggr_kind2doc((
void*)_tmp1F8->kind);_tmp219[0]=Cyc_Absynpp_scope2doc((void*)_tmp1F8->sc);Cyc_PP_cat(
_tag_arr(_tmp219,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp21A[11];_tmp21A[10]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmp21A[9]=Cyc_Absynpp_atts2doc(
_tmp1F8->attributes);_tmp21A[8]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp21A[7]=Cyc_PP_line_doc();_tmp21A[6]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp21C[
2];_tmp21C[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1F8->impl))->fields);_tmp21C[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp21C,sizeof(struct Cyc_PP_Doc*),2));}));_tmp21A[5]=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1F8->impl))->rgn_po == 0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*
_tmp21B[2];_tmp21B[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1F8->impl))->rgn_po);_tmp21B[0]=Cyc_PP_text(_tag_arr(":",sizeof(
char),2));Cyc_PP_cat(_tag_arr(_tmp21B,sizeof(struct Cyc_PP_Doc*),2));});_tmp21A[4]=
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F8->impl))->exist_vars);
_tmp21A[3]=Cyc_PP_text(_tag_arr(" {",sizeof(char),3));_tmp21A[2]=Cyc_Absynpp_qvar2doc(
_tmp1F8->name);_tmp21A[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp1F8->kind);
_tmp21A[0]=Cyc_Absynpp_scope2doc((void*)_tmp1F8->sc);Cyc_PP_cat(_tag_arr(_tmp21A,
sizeof(struct Cyc_PP_Doc*),11));});}goto _LL278;_LL27D: if(*((int*)_tmp1F6)!= 0)
goto _LL27F;_tmp1F9=((struct Cyc_Absyn_Var_d_struct*)_tmp1F6)->f1;_LL27E: s=Cyc_Absynpp_vardecl2doc(
_tmp1F9);goto _LL278;_LL27F: if(*((int*)_tmp1F6)!= 5)goto _LL281;_tmp1FA=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp1F6)->f1;_tmp1FB=*_tmp1FA;_tmp1FC=(void*)_tmp1FB.sc;
_tmp1FD=_tmp1FB.name;_tmp1FE=_tmp1FB.tvs;_tmp1FF=_tmp1FB.fields;_tmp200=_tmp1FB.is_xtunion;
_LL280: if(_tmp1FF == 0)s=({struct Cyc_PP_Doc*_tmp21D[5];_tmp21D[4]=Cyc_PP_text(
_tag_arr(";",sizeof(char),2));_tmp21D[3]=Cyc_Absynpp_ktvars2doc(_tmp1FE);_tmp21D[
2]=_tmp200?Cyc_Absynpp_qvar2doc(_tmp1FD): Cyc_Absynpp_typedef_name2doc(_tmp1FD);
_tmp21D[1]=_tmp200?Cyc_PP_text(_tag_arr("xtunion ",sizeof(char),9)): Cyc_PP_text(
_tag_arr("tunion ",sizeof(char),8));_tmp21D[0]=Cyc_Absynpp_scope2doc(_tmp1FC);
Cyc_PP_cat(_tag_arr(_tmp21D,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp21E[8];_tmp21E[7]=Cyc_PP_text(_tag_arr("};",sizeof(char),3));_tmp21E[6]=Cyc_PP_line_doc();
_tmp21E[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp21F[2];_tmp21F[1]=Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*)_tmp1FF->v);_tmp21F[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp21F,sizeof(struct Cyc_PP_Doc*),2));}));_tmp21E[4]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));_tmp21E[3]=Cyc_Absynpp_ktvars2doc(_tmp1FE);
_tmp21E[2]=_tmp200?Cyc_Absynpp_qvar2doc(_tmp1FD): Cyc_Absynpp_typedef_name2doc(
_tmp1FD);_tmp21E[1]=_tmp200?Cyc_PP_text(_tag_arr("xtunion ",sizeof(char),9)): Cyc_PP_text(
_tag_arr("tunion ",sizeof(char),8));_tmp21E[0]=Cyc_Absynpp_scope2doc(_tmp1FC);
Cyc_PP_cat(_tag_arr(_tmp21E,sizeof(struct Cyc_PP_Doc*),8));});}goto _LL278;_LL281:
if(*((int*)_tmp1F6)!= 2)goto _LL283;_tmp201=((struct Cyc_Absyn_Let_d_struct*)
_tmp1F6)->f1;_tmp202=((struct Cyc_Absyn_Let_d_struct*)_tmp1F6)->f3;_LL282: s=({
struct Cyc_PP_Doc*_tmp220[5];_tmp220[4]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));
_tmp220[3]=Cyc_Absynpp_exp2doc(_tmp202);_tmp220[2]=Cyc_PP_text(_tag_arr(" = ",
sizeof(char),4));_tmp220[1]=Cyc_Absynpp_pat2doc(_tmp201);_tmp220[0]=Cyc_PP_text(
_tag_arr("let ",sizeof(char),5));Cyc_PP_cat(_tag_arr(_tmp220,sizeof(struct Cyc_PP_Doc*),
5));});goto _LL278;_LL283: if(*((int*)_tmp1F6)!= 3)goto _LL285;_tmp203=((struct Cyc_Absyn_Letv_d_struct*)
_tmp1F6)->f1;_LL284: s=({struct Cyc_PP_Doc*_tmp221[3];_tmp221[2]=Cyc_PP_text(
_tag_arr(";",sizeof(char),2));_tmp221[1]=Cyc_Absynpp_ids2doc(_tmp203);_tmp221[0]=
Cyc_PP_text(_tag_arr("let ",sizeof(char),5));Cyc_PP_cat(_tag_arr(_tmp221,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL278;_LL285: if(*((int*)_tmp1F6)!= 6)goto _LL287;
_tmp204=((struct Cyc_Absyn_Enum_d_struct*)_tmp1F6)->f1;_tmp205=*_tmp204;_tmp206=(
void*)_tmp205.sc;_tmp207=_tmp205.name;_tmp208=_tmp205.fields;_LL286: if(_tmp208 == 
0)s=({struct Cyc_PP_Doc*_tmp222[4];_tmp222[3]=Cyc_PP_text(_tag_arr(";",sizeof(
char),2));_tmp222[2]=Cyc_Absynpp_typedef_name2doc(_tmp207);_tmp222[1]=Cyc_PP_text(
_tag_arr("enum ",sizeof(char),6));_tmp222[0]=Cyc_Absynpp_scope2doc(_tmp206);Cyc_PP_cat(
_tag_arr(_tmp222,sizeof(struct Cyc_PP_Doc*),4));});else{s=({struct Cyc_PP_Doc*
_tmp223[7];_tmp223[6]=Cyc_PP_text(_tag_arr("};",sizeof(char),3));_tmp223[5]=Cyc_PP_line_doc();
_tmp223[4]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp224[2];_tmp224[1]=Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*)_tmp208->v);_tmp224[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp224,sizeof(struct Cyc_PP_Doc*),2));}));_tmp223[3]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));_tmp223[2]=Cyc_Absynpp_qvar2doc(_tmp207);_tmp223[
1]=Cyc_PP_text(_tag_arr("enum ",sizeof(char),6));_tmp223[0]=Cyc_Absynpp_scope2doc(
_tmp206);Cyc_PP_cat(_tag_arr(_tmp223,sizeof(struct Cyc_PP_Doc*),7));});}goto
_LL278;_LL287: if(*((int*)_tmp1F6)!= 7)goto _LL289;_tmp209=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp1F6)->f1;_LL288: {void*t;if(_tmp209->defn != 0)t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp209->defn))->v;else{t=Cyc_Absyn_new_evar(_tmp209->kind,0);}s=({
struct Cyc_PP_Doc*_tmp225[3];_tmp225[2]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));
_tmp225[1]=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual _tmp226;_tmp226.q_const=0;
_tmp226.q_volatile=0;_tmp226.q_restrict=0;_tmp226;}),t,({struct Cyc_Core_Opt*
_tmp227=_cycalloc(sizeof(*_tmp227));_tmp227->v=({struct Cyc_PP_Doc*_tmp228[2];
_tmp228[1]=Cyc_Absynpp_tvars2doc(_tmp209->tvs);_tmp228[0]=Cyc_Absynpp_typedef_name2doc(
_tmp209->name);Cyc_PP_cat(_tag_arr(_tmp228,sizeof(struct Cyc_PP_Doc*),2));});
_tmp227;}));_tmp225[0]=Cyc_PP_text(_tag_arr("typedef ",sizeof(char),9));Cyc_PP_cat(
_tag_arr(_tmp225,sizeof(struct Cyc_PP_Doc*),3));});goto _LL278;}_LL289: if(*((int*)
_tmp1F6)!= 8)goto _LL28B;_tmp20A=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1F6)->f1;
_tmp20B=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1F6)->f2;_LL28A: if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_add(_tmp20A);s=({struct Cyc_PP_Doc*_tmp229[7];_tmp229[
6]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp229[5]=Cyc_PP_line_doc();
_tmp229[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(char),1),_tmp20B);_tmp229[3]=Cyc_PP_line_doc();_tmp229[2]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));_tmp229[1]=Cyc_PP_textptr(_tmp20A);_tmp229[0]=Cyc_PP_text(
_tag_arr("namespace ",sizeof(char),11));Cyc_PP_cat(_tag_arr(_tmp229,sizeof(
struct Cyc_PP_Doc*),7));});if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL278;_LL28B: if(*((int*)_tmp1F6)!= 9)goto _LL28D;_tmp20C=((struct Cyc_Absyn_Using_d_struct*)
_tmp1F6)->f1;_tmp20D=((struct Cyc_Absyn_Using_d_struct*)_tmp1F6)->f2;_LL28C: if(
Cyc_Absynpp_print_using_stmts)s=({struct Cyc_PP_Doc*_tmp22A[7];_tmp22A[6]=Cyc_PP_text(
_tag_arr("}",sizeof(char),2));_tmp22A[5]=Cyc_PP_line_doc();_tmp22A[4]=((struct
Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr
sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tag_arr("",
sizeof(char),1),_tmp20D);_tmp22A[3]=Cyc_PP_line_doc();_tmp22A[2]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));_tmp22A[1]=Cyc_Absynpp_qvar2doc(_tmp20C);_tmp22A[
0]=Cyc_PP_text(_tag_arr("using ",sizeof(char),7));Cyc_PP_cat(_tag_arr(_tmp22A,
sizeof(struct Cyc_PP_Doc*),7));});else{s=({struct Cyc_PP_Doc*_tmp22B[7];_tmp22B[6]=
Cyc_PP_text(_tag_arr("/* } */",sizeof(char),8));_tmp22B[5]=Cyc_PP_line_doc();
_tmp22B[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(char),1),_tmp20D);_tmp22B[3]=Cyc_PP_line_doc();_tmp22B[2]=Cyc_PP_text(
_tag_arr(" { */",sizeof(char),6));_tmp22B[1]=Cyc_Absynpp_qvar2doc(_tmp20C);
_tmp22B[0]=Cyc_PP_text(_tag_arr("/* using ",sizeof(char),10));Cyc_PP_cat(
_tag_arr(_tmp22B,sizeof(struct Cyc_PP_Doc*),7));});}goto _LL278;_LL28D: if(*((int*)
_tmp1F6)!= 10)goto _LL278;_tmp20E=((struct Cyc_Absyn_ExternC_d_struct*)_tmp1F6)->f1;
_LL28E: if(Cyc_Absynpp_print_externC_stmts)s=({struct Cyc_PP_Doc*_tmp22C[5];
_tmp22C[4]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp22C[3]=Cyc_PP_line_doc();
_tmp22C[2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(char),1),_tmp20E);_tmp22C[1]=Cyc_PP_line_doc();_tmp22C[0]=Cyc_PP_text(
_tag_arr("extern \"C\" {",sizeof(char),13));Cyc_PP_cat(_tag_arr(_tmp22C,sizeof(
struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp22D[5];_tmp22D[4]=Cyc_PP_text(
_tag_arr("/* } */",sizeof(char),8));_tmp22D[3]=Cyc_PP_line_doc();_tmp22D[2]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(char),1),_tmp20E);_tmp22D[1]=Cyc_PP_line_doc();_tmp22D[0]=Cyc_PP_text(
_tag_arr("/* extern \"C\" { */",sizeof(char),19));Cyc_PP_cat(_tag_arr(_tmp22D,
sizeof(struct Cyc_PP_Doc*),5));});}goto _LL278;_LL278:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(void*sc){void*_tmp22E=sc;_LL290: if((int)_tmp22E != 0)goto
_LL292;_LL291: return Cyc_PP_text(_tag_arr("static ",sizeof(char),8));_LL292: if((
int)_tmp22E != 2)goto _LL294;_LL293: return Cyc_PP_nil_doc();_LL294: if((int)_tmp22E
!= 3)goto _LL296;_LL295: return Cyc_PP_text(_tag_arr("extern ",sizeof(char),8));
_LL296: if((int)_tmp22E != 4)goto _LL298;_LL297: return Cyc_PP_text(_tag_arr("extern \"C\" ",
sizeof(char),12));_LL298: if((int)_tmp22E != 1)goto _LL28F;_LL299: return Cyc_PP_text(
_tag_arr("abstract ",sizeof(char),10));_LL28F:;}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp22F=t;struct Cyc_Absyn_Tvar*_tmp230;struct Cyc_List_List*_tmp231;
_LL29B: if(_tmp22F <= (void*)3?1:*((int*)_tmp22F)!= 1)goto _LL29D;_tmp230=((struct
Cyc_Absyn_VarType_struct*)_tmp22F)->f1;_LL29C: return Cyc_Tcutil_is_temp_tvar(
_tmp230);_LL29D: if(_tmp22F <= (void*)3?1:*((int*)_tmp22F)!= 20)goto _LL29F;_tmp231=((
struct Cyc_Absyn_JoinEff_struct*)_tmp22F)->f1;_LL29E: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp231);_LL29F:;_LL2A0: return 0;_LL29A:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp232=t;struct Cyc_Core_Opt*_tmp233;struct Cyc_Core_Opt _tmp234;void*_tmp235;
_LL2A2: if(_tmp232 <= (void*)3?1:*((int*)_tmp232)!= 11)goto _LL2A4;_LL2A3: return 1;
_LL2A4: if(_tmp232 <= (void*)3?1:*((int*)_tmp232)!= 16)goto _LL2A6;_tmp233=((struct
Cyc_Absyn_TypedefType_struct*)_tmp232)->f4;if(_tmp233 == 0)goto _LL2A6;_tmp234=*
_tmp233;_tmp235=(void*)_tmp234.v;_LL2A5: return Cyc_Absynpp_is_anon_aggrtype(
_tmp235);_LL2A6:;_LL2A7: return 0;_LL2A1:;}struct _tuple6 Cyc_Absynpp_to_tms(struct
Cyc_Absyn_Tqual tq,void*t){void*_tmp236=t;void*_tmp237;struct Cyc_Absyn_Tqual
_tmp238;struct Cyc_Absyn_Exp*_tmp239;struct Cyc_Absyn_PtrInfo _tmp23A;void*_tmp23B;
void*_tmp23C;struct Cyc_Absyn_Conref*_tmp23D;struct Cyc_Absyn_Tqual _tmp23E;struct
Cyc_Absyn_Conref*_tmp23F;struct Cyc_Absyn_FnInfo _tmp240;struct Cyc_List_List*
_tmp241;struct Cyc_Core_Opt*_tmp242;void*_tmp243;struct Cyc_List_List*_tmp244;int
_tmp245;struct Cyc_Absyn_VarargInfo*_tmp246;struct Cyc_List_List*_tmp247;struct Cyc_List_List*
_tmp248;struct Cyc_Core_Opt*_tmp249;struct Cyc_Core_Opt*_tmp24A;int _tmp24B;struct
_tuple0*_tmp24C;struct Cyc_List_List*_tmp24D;struct Cyc_Core_Opt*_tmp24E;_LL2A9:
if(_tmp236 <= (void*)3?1:*((int*)_tmp236)!= 7)goto _LL2AB;_tmp237=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp236)->f1;_tmp238=((struct Cyc_Absyn_ArrayType_struct*)
_tmp236)->f2;_tmp239=((struct Cyc_Absyn_ArrayType_struct*)_tmp236)->f3;_LL2AA: {
struct Cyc_Absyn_Tqual _tmp250;void*_tmp251;struct Cyc_List_List*_tmp252;struct
_tuple6 _tmp24F=Cyc_Absynpp_to_tms(_tmp238,_tmp237);_tmp250=_tmp24F.f1;_tmp251=
_tmp24F.f2;_tmp252=_tmp24F.f3;{void*tm;if(_tmp239 == 0)tm=(void*)0;else{tm=(void*)({
struct Cyc_Absyn_ConstArray_mod_struct*_tmp253=_cycalloc(sizeof(*_tmp253));
_tmp253[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp254;_tmp254.tag=0;_tmp254.f1=(
struct Cyc_Absyn_Exp*)_check_null(_tmp239);_tmp254;});_tmp253;});}return({struct
_tuple6 _tmp255;_tmp255.f1=_tmp250;_tmp255.f2=_tmp251;_tmp255.f3=({struct Cyc_List_List*
_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256->hd=(void*)tm;_tmp256->tl=_tmp252;
_tmp256;});_tmp255;});}}_LL2AB: if(_tmp236 <= (void*)3?1:*((int*)_tmp236)!= 4)goto
_LL2AD;_tmp23A=((struct Cyc_Absyn_PointerType_struct*)_tmp236)->f1;_tmp23B=(void*)
_tmp23A.elt_typ;_tmp23C=(void*)_tmp23A.rgn_typ;_tmp23D=_tmp23A.nullable;_tmp23E=
_tmp23A.tq;_tmp23F=_tmp23A.bounds;_LL2AC: {struct Cyc_Absyn_Tqual _tmp258;void*
_tmp259;struct Cyc_List_List*_tmp25A;struct _tuple6 _tmp257=Cyc_Absynpp_to_tms(
_tmp23E,_tmp23B);_tmp258=_tmp257.f1;_tmp259=_tmp257.f2;_tmp25A=_tmp257.f3;{void*
ps;{struct _tuple4 _tmp25C=({struct _tuple4 _tmp25B;_tmp25B.f1=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp23D))->v;_tmp25B.f2=(
void*)(Cyc_Absyn_compress_conref(_tmp23F))->v;_tmp25B;});void*_tmp25D;int _tmp25E;
void*_tmp25F;void*_tmp260;struct Cyc_Absyn_Exp*_tmp261;void*_tmp262;int _tmp263;
void*_tmp264;void*_tmp265;struct Cyc_Absyn_Exp*_tmp266;void*_tmp267;void*_tmp268;
_LL2B6: _tmp25D=_tmp25C.f1;if(_tmp25D <= (void*)1?1:*((int*)_tmp25D)!= 0)goto
_LL2B8;_tmp25E=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp25D)->f1;if(_tmp25E
!= 1)goto _LL2B8;_tmp25F=_tmp25C.f2;if(_tmp25F <= (void*)1?1:*((int*)_tmp25F)!= 0)
goto _LL2B8;_tmp260=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp25F)->f1;if(
_tmp260 <= (void*)1?1:*((int*)_tmp260)!= 0)goto _LL2B8;_tmp261=((struct Cyc_Absyn_Upper_b_struct*)
_tmp260)->f1;_LL2B7: ps=(void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp269=
_cycalloc(sizeof(*_tmp269));_tmp269[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp26A;_tmp26A.tag=1;_tmp26A.f1=_tmp261;_tmp26A;});_tmp269;});goto _LL2B5;_LL2B8:
_tmp262=_tmp25C.f1;if(_tmp262 <= (void*)1?1:*((int*)_tmp262)!= 0)goto _LL2BA;
_tmp263=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp262)->f1;if(_tmp263 != 0)
goto _LL2BA;_tmp264=_tmp25C.f2;if(_tmp264 <= (void*)1?1:*((int*)_tmp264)!= 0)goto
_LL2BA;_tmp265=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp264)->f1;if(
_tmp265 <= (void*)1?1:*((int*)_tmp265)!= 0)goto _LL2BA;_tmp266=((struct Cyc_Absyn_Upper_b_struct*)
_tmp265)->f1;_LL2B9: ps=(void*)({struct Cyc_Absyn_NonNullable_ps_struct*_tmp26B=
_cycalloc(sizeof(*_tmp26B));_tmp26B[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp26C;_tmp26C.tag=0;_tmp26C.f1=_tmp266;_tmp26C;});_tmp26B;});goto _LL2B5;_LL2BA:
_tmp267=_tmp25C.f2;if(_tmp267 <= (void*)1?1:*((int*)_tmp267)!= 0)goto _LL2BC;
_tmp268=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp267)->f1;if((int)_tmp268
!= 0)goto _LL2BC;_LL2BB: ps=(void*)0;goto _LL2B5;_LL2BC:;_LL2BD: ps=(void*)0;goto
_LL2B5;_LL2B5:;}{void*tm=(void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp26F=
_cycalloc(sizeof(*_tmp26F));_tmp26F[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp270;_tmp270.tag=1;_tmp270.f1=(void*)ps;_tmp270.f2=(void*)_tmp23C;_tmp270.f3=
tq;_tmp270;});_tmp26F;});return({struct _tuple6 _tmp26D;_tmp26D.f1=_tmp258;_tmp26D.f2=
_tmp259;_tmp26D.f3=({struct Cyc_List_List*_tmp26E=_cycalloc(sizeof(*_tmp26E));
_tmp26E->hd=(void*)tm;_tmp26E->tl=_tmp25A;_tmp26E;});_tmp26D;});}}}_LL2AD: if(
_tmp236 <= (void*)3?1:*((int*)_tmp236)!= 8)goto _LL2AF;_tmp240=((struct Cyc_Absyn_FnType_struct*)
_tmp236)->f1;_tmp241=_tmp240.tvars;_tmp242=_tmp240.effect;_tmp243=(void*)_tmp240.ret_typ;
_tmp244=_tmp240.args;_tmp245=_tmp240.c_varargs;_tmp246=_tmp240.cyc_varargs;
_tmp247=_tmp240.rgn_po;_tmp248=_tmp240.attributes;_LL2AE: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp242 == 0?1: Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp242->v)){
_tmp242=0;_tmp241=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp241);}{struct Cyc_Absyn_Tqual _tmp272;void*_tmp273;struct Cyc_List_List*_tmp274;
struct _tuple6 _tmp271=Cyc_Absynpp_to_tms(Cyc_Absyn_empty_tqual(),_tmp243);_tmp272=
_tmp271.f1;_tmp273=_tmp271.f2;_tmp274=_tmp271.f3;{struct Cyc_List_List*tms=
_tmp274;if(_tmp248 != 0?!Cyc_Absynpp_to_VC: 0)tms=({struct Cyc_List_List*_tmp275=
_cycalloc(sizeof(*_tmp275));_tmp275->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp276=_cycalloc(sizeof(*_tmp276));_tmp276[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp277;_tmp277.tag=4;_tmp277.f1=0;_tmp277.f2=_tmp248;_tmp277;});_tmp276;}));
_tmp275->tl=tms;_tmp275;});tms=({struct Cyc_List_List*_tmp278=_cycalloc(sizeof(*
_tmp278));_tmp278->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp27A;_tmp27A.tag=2;_tmp27A.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp27C;_tmp27C.tag=1;_tmp27C.f1=_tmp244;_tmp27C.f2=_tmp245;_tmp27C.f3=_tmp246;
_tmp27C.f4=_tmp242;_tmp27C.f5=_tmp247;_tmp27C;});_tmp27B;}));_tmp27A;});_tmp279;}));
_tmp278->tl=tms;_tmp278;});if(Cyc_Absynpp_to_VC)for(0;_tmp248 != 0;_tmp248=
_tmp248->tl){void*_tmp27D=(void*)_tmp248->hd;_LL2BF: if((int)_tmp27D != 0)goto
_LL2C1;_LL2C0: goto _LL2C2;_LL2C1: if((int)_tmp27D != 1)goto _LL2C3;_LL2C2: goto _LL2C4;
_LL2C3: if((int)_tmp27D != 2)goto _LL2C5;_LL2C4: tms=({struct Cyc_List_List*_tmp27E=
_cycalloc(sizeof(*_tmp27E));_tmp27E->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp280;_tmp280.tag=4;_tmp280.f1=0;_tmp280.f2=({struct Cyc_List_List*_tmp281=
_cycalloc(sizeof(*_tmp281));_tmp281->hd=(void*)((void*)_tmp248->hd);_tmp281->tl=
0;_tmp281;});_tmp280;});_tmp27F;}));_tmp27E->tl=tms;_tmp27E;});goto AfterAtts;
_LL2C5:;_LL2C6: goto _LL2BE;_LL2BE:;}AfterAtts: if(_tmp241 != 0)tms=({struct Cyc_List_List*
_tmp282=_cycalloc(sizeof(*_tmp282));_tmp282->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp283=_cycalloc(sizeof(*_tmp283));_tmp283[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp284;_tmp284.tag=3;_tmp284.f1=_tmp241;_tmp284.f2=0;_tmp284.f3=1;_tmp284;});
_tmp283;}));_tmp282->tl=tms;_tmp282;});return({struct _tuple6 _tmp285;_tmp285.f1=
_tmp272;_tmp285.f2=_tmp273;_tmp285.f3=tms;_tmp285;});}}_LL2AF: if(_tmp236 <= (void*)
3?1:*((int*)_tmp236)!= 0)goto _LL2B1;_tmp249=((struct Cyc_Absyn_Evar_struct*)
_tmp236)->f1;_tmp24A=((struct Cyc_Absyn_Evar_struct*)_tmp236)->f2;_tmp24B=((
struct Cyc_Absyn_Evar_struct*)_tmp236)->f3;_LL2B0: if(_tmp24A == 0)return({struct
_tuple6 _tmp286;_tmp286.f1=tq;_tmp286.f2=t;_tmp286.f3=0;_tmp286;});else{return Cyc_Absynpp_to_tms(
tq,(void*)_tmp24A->v);}_LL2B1: if(_tmp236 <= (void*)3?1:*((int*)_tmp236)!= 16)goto
_LL2B3;_tmp24C=((struct Cyc_Absyn_TypedefType_struct*)_tmp236)->f1;_tmp24D=((
struct Cyc_Absyn_TypedefType_struct*)_tmp236)->f2;_tmp24E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp236)->f4;_LL2B2: if((_tmp24E == 0?1: !Cyc_Absynpp_expand_typedefs)?1: Cyc_Absynpp_is_anon_aggrtype((
void*)_tmp24E->v))return({struct _tuple6 _tmp287;_tmp287.f1=tq;_tmp287.f2=t;
_tmp287.f3=0;_tmp287;});else{return Cyc_Absynpp_to_tms(tq,(void*)_tmp24E->v);}
_LL2B3:;_LL2B4: return({struct _tuple6 _tmp288;_tmp288.f1=tq;_tmp288.f2=t;_tmp288.f3=
0;_tmp288;});_LL2A8:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_Core_Opt*dopt){struct Cyc_Absyn_Tqual _tmp28A;void*_tmp28B;
struct Cyc_List_List*_tmp28C;struct _tuple6 _tmp289=Cyc_Absynpp_to_tms(tq,t);
_tmp28A=_tmp289.f1;_tmp28B=_tmp289.f2;_tmp28C=_tmp289.f3;_tmp28C=Cyc_List_imp_rev(
_tmp28C);if(_tmp28C == 0?dopt == 0: 0)return({struct Cyc_PP_Doc*_tmp28D[2];_tmp28D[1]=
Cyc_Absynpp_ntyp2doc(_tmp28B);_tmp28D[0]=Cyc_Absynpp_tqual2doc(_tmp28A);Cyc_PP_cat(
_tag_arr(_tmp28D,sizeof(struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*
_tmp28E[4];_tmp28E[3]=Cyc_Absynpp_dtms2doc(dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)
dopt->v,_tmp28C);_tmp28E[2]=Cyc_PP_text(_tag_arr(" ",sizeof(char),2));_tmp28E[1]=
Cyc_Absynpp_ntyp2doc(_tmp28B);_tmp28E[0]=Cyc_Absynpp_tqual2doc(_tmp28A);Cyc_PP_cat(
_tag_arr(_tmp28E,sizeof(struct Cyc_PP_Doc*),4));});}}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(
struct Cyc_Absyn_Aggrfield*f){if(f->width != 0)return({struct Cyc_PP_Doc*_tmp28F[5];
_tmp28F[4]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmp28F[3]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp28F[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(
f->width));_tmp28F[1]=Cyc_PP_text(_tag_arr(":",sizeof(char),2));_tmp28F[0]=Cyc_Absynpp_tqtd2doc(
f->tq,(void*)f->type,({struct Cyc_Core_Opt*_tmp290=_cycalloc(sizeof(*_tmp290));
_tmp290->v=Cyc_PP_textptr(f->name);_tmp290;}));Cyc_PP_cat(_tag_arr(_tmp28F,
sizeof(struct Cyc_PP_Doc*),5));});else{return({struct Cyc_PP_Doc*_tmp291[3];
_tmp291[2]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmp291[1]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp291[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292->v=Cyc_PP_textptr(f->name);_tmp292;}));
Cyc_PP_cat(_tag_arr(_tmp291,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _tagged_arr sep,struct
Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tag_arr("",sizeof(
char),1),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct Cyc_Absyn_Tunionfield*
f){return({struct Cyc_PP_Doc*_tmp293[3];_tmp293[2]=f->typs == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc(f->typs);_tmp293[1]=Cyc_Absynpp_typedef_name2doc(f->name);
_tmp293[0]=Cyc_Absynpp_scope2doc((void*)f->sc);Cyc_PP_cat(_tag_arr(_tmp293,
sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Tunionfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_tunionfield2doc,_tag_arr(",",sizeof(char),2),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);({
void*_tmp294[0]={};Cyc_Std_fprintf(f,_tag_arr("\n",sizeof(char),2),_tag_arr(
_tmp294,sizeof(void*),0));});}}struct _tagged_arr Cyc_Absynpp_decllist2string(
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
