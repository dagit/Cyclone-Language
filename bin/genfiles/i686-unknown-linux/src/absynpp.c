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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};typedef
struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc_Std___mbstate_t;
typedef struct{int __pos;Cyc_Std___mbstate_t __state;}Cyc_Std__G_fpos_t;typedef Cyc_Std__G_fpos_t
Cyc_Std_fpos_t;struct Cyc_Std___cycFILE;struct Cyc_Std_Cstdio___abstractFILE;
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct _tagged_arr,
struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(
struct Cyc_PP_Doc*d,int w,struct Cyc_Std___cycFILE*f);struct _tagged_arr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_line_doc();
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
struct Cyc_Absyn_Format_att_struct{int tag;void*f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{
int tag;int f1;};struct Cyc_Absyn_ConstArray_mod_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pointer_mod_struct{int tag;void*f1;void*f2;struct Cyc_Absyn_Tqual
f3;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{
int tag;struct Cyc_Position_Segment*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{
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
_tagged_arr);struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char
Cyc_Dict_Absent[11];struct _tuple5{void*f1;void*f2;};struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
struct Cyc_Absyn_Stmt*s);struct _tuple6{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg);struct
_tuple7{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple7
Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);struct _tuple8{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp);
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
void*p,struct Cyc_List_List*es);struct _tuple9{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
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
_check_unknown_subscript(_tmp3,sizeof(char),j ++))=(char)('0' + ((unsigned char)c
>> 6 & 3));*((char*)_check_unknown_subscript(_tmp3,sizeof(char),j ++))=(char)('0' + (
c >> 3 & 7));*((char*)_check_unknown_subscript(_tmp3,sizeof(char),j ++))=(char)('0'
+ (c & 7));return(struct _tagged_arr)_tmp3;}}}static int Cyc_Absynpp_special(struct
_tagged_arr s){int sz=(int)(_get_arr_size(s,sizeof(char))- 1);{int i=0;for(0;i < sz;
i ++){char c=*((const char*)_check_unknown_subscript(s,sizeof(char),i));if(((c <= ' '?
1: c >= '~')?1: c == '"')?1: c == '\\')return 1;}}return 0;}struct _tagged_arr Cyc_Absynpp_string_escape(
struct _tagged_arr s){if(!Cyc_Absynpp_special(s))return s;{int n=(int)(_get_arr_size(
s,sizeof(char))- 1);if(n > 0?*((const char*)_check_unknown_subscript(s,sizeof(char),
n))== '\000': 0)n --;{int len=0;{int i=0;for(0;i <= n;i ++){char _tmp4=*((const char*)
_check_unknown_subscript(s,sizeof(char),i));_LLD: if(_tmp4 != '\a')goto _LLF;_LLE:
goto _LL10;_LLF: if(_tmp4 != '\b')goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp4 != '\f')
goto _LL13;_LL12: goto _LL14;_LL13: if(_tmp4 != '\n')goto _LL15;_LL14: goto _LL16;_LL15:
if(_tmp4 != '\r')goto _LL17;_LL16: goto _LL18;_LL17: if(_tmp4 != '\t')goto _LL19;_LL18:
goto _LL1A;_LL19: if(_tmp4 != '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp4 != '\\')
goto _LL1D;_LL1C: goto _LL1E;_LL1D: if(_tmp4 != '"')goto _LL1F;_LL1E: len +=2;goto _LLC;
_LL1F:;_LL20: if(_tmp4 >= ' '?_tmp4 <= '~': 0)len ++;else{len +=4;}goto _LLC;_LLC:;}}{
struct _tagged_arr t=Cyc_Core_new_string((unsigned int)len);int j=0;{int i=0;for(0;i
<= n;i ++){char _tmp5=*((const char*)_check_unknown_subscript(s,sizeof(char),i));
_LL22: if(_tmp5 != '\a')goto _LL24;_LL23:*((char*)_check_unknown_subscript(t,
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
x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));}struct _tuple10{struct Cyc_Absyn_Tqual
f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple10*t){return Cyc_Absynpp_tqtd2doc((*
t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return Cyc_PP_group(_tag_arr("(",sizeof(char),2),_tag_arr(")",sizeof(char),2),
_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
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
struct Cyc_Absyn_Nullable_ps_struct*)_tmp35)->f1;_LL89: {struct _tuple4 pr=Cyc_Evexp_eval_const_uint_exp(
_tmp36);if(pr.f1 == 1?pr.f2: 0)ptr=Cyc_PP_text(_tag_arr("*",sizeof(char),2));else{
ptr=({struct Cyc_PP_Doc*_tmp38[4];_tmp38[3]=Cyc_PP_text(_tag_arr("}",sizeof(char),
2));_tmp38[2]=Cyc_Absynpp_exp2doc(_tmp36);_tmp38[1]=Cyc_PP_text(_tag_arr("{",
sizeof(char),2));_tmp38[0]=Cyc_PP_text(_tag_arr("*",sizeof(char),2));Cyc_PP_cat(
_tag_arr(_tmp38,sizeof(struct Cyc_PP_Doc*),4));});}goto _LL87;}_LL8A: if(_tmp35 <= (
void*)1?1:*((int*)_tmp35)!= 0)goto _LL8C;_tmp37=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmp35)->f1;_LL8B: {struct _tuple4 pr=Cyc_Evexp_eval_const_uint_exp(_tmp37);if(pr.f1
== 1?pr.f2: 0)ptr=Cyc_PP_text(_tag_arr("@",sizeof(char),2));else{ptr=({struct Cyc_PP_Doc*
_tmp39[4];_tmp39[3]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp39[2]=Cyc_Absynpp_exp2doc(
_tmp37);_tmp39[1]=Cyc_PP_text(_tag_arr("{",sizeof(char),2));_tmp39[0]=Cyc_PP_text(
_tag_arr("@",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp39,sizeof(struct Cyc_PP_Doc*),
4));});}goto _LL87;}_LL8C: if((int)_tmp35 != 0)goto _LL87;_LL8D: ptr=Cyc_PP_text(
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
0)return Cyc_Absynpp_ntyp2doc((void*)_tmp49->v);else{void*_tmp68=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp48))->v;_LLD8: if((int)_tmp68 != 3)goto _LLDA;_LLD9: s=({
struct Cyc_PP_Doc*_tmp69[2];_tmp69[1]=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp6B;_tmp6B.tag=1;_tmp6B.f1=(int)((unsigned int)_tmp4A);{void*_tmp6A[1]={&
_tmp6B};Cyc_Std_aprintf(_tag_arr("%d",sizeof(char),3),_tag_arr(_tmp6A,sizeof(
void*),1));}}));_tmp69[0]=Cyc_PP_text(_tag_arr("`r",sizeof(char),3));Cyc_PP_cat(
_tag_arr(_tmp69,sizeof(struct Cyc_PP_Doc*),2));});goto _LLD7;_LLDA:;_LLDB: s=({
struct Cyc_PP_Doc*_tmp6C[6];_tmp6C[5]=_tmp48 == 0?Cyc_PP_text(_tag_arr("?",sizeof(
char),2)): Cyc_Absynpp_kind2doc((void*)_tmp48->v);_tmp6C[4]=Cyc_PP_text(_tag_arr(")::",
sizeof(char),4));_tmp6C[3]=(!Cyc_Absynpp_print_full_evars?1: _tmp4B == 0)?Cyc_PP_text(
_tag_arr("",sizeof(char),1)): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp4B->v);
_tmp6C[2]=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp6E;
_tmp6E.tag=1;_tmp6E.f1=(int)((unsigned int)_tmp4A);{void*_tmp6D[1]={& _tmp6E};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(char),3),_tag_arr(_tmp6D,sizeof(void*),1));}}));_tmp6C[1]=
Cyc_PP_text(_tag_arr("(",sizeof(char),2));_tmp6C[0]=Cyc_PP_text(_tag_arr("%",
sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp6C,sizeof(struct Cyc_PP_Doc*),6));});
_LLD7:;}goto _LLA4;_LLAF: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 1)goto _LLB1;
_tmp4C=((struct Cyc_Absyn_VarType_struct*)_tmp47)->f1;_LLB0: s=Cyc_PP_textptr(
_tmp4C->name);if(Cyc_Absynpp_print_all_kinds)s=({struct Cyc_PP_Doc*_tmp6F[3];
_tmp6F[2]=Cyc_Absynpp_kindbound2doc((void*)_tmp4C->kind);_tmp6F[1]=Cyc_PP_text(
_tag_arr("::",sizeof(char),3));_tmp6F[0]=s;Cyc_PP_cat(_tag_arr(_tmp6F,sizeof(
struct Cyc_PP_Doc*),3));});if(Cyc_Absynpp_rewrite_temp_tvars?Cyc_Tcutil_is_temp_tvar(
_tmp4C): 0)s=({struct Cyc_PP_Doc*_tmp70[3];_tmp70[2]=Cyc_PP_text(_tag_arr(" */",
sizeof(char),4));_tmp70[1]=s;_tmp70[0]=Cyc_PP_text(_tag_arr("_ /* ",sizeof(char),
6));Cyc_PP_cat(_tag_arr(_tmp70,sizeof(struct Cyc_PP_Doc*),3));});goto _LLA4;_LLB1:
if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 2)goto _LLB3;_tmp4D=((struct Cyc_Absyn_TunionType_struct*)
_tmp47)->f1;_tmp4E=(void*)_tmp4D.tunion_info;_tmp4F=_tmp4D.targs;_tmp50=(void*)
_tmp4D.rgn;_LLB2:{void*_tmp71=_tmp4E;struct Cyc_Absyn_UnknownTunionInfo _tmp72;
struct _tuple0*_tmp73;int _tmp74;struct Cyc_Absyn_Tuniondecl**_tmp75;struct Cyc_Absyn_Tuniondecl*
_tmp76;struct Cyc_Absyn_Tuniondecl _tmp77;struct _tuple0*_tmp78;int _tmp79;_LLDD: if(*((
int*)_tmp71)!= 0)goto _LLDF;_tmp72=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp71)->f1;
_tmp73=_tmp72.name;_tmp74=_tmp72.is_xtunion;_LLDE: _tmp78=_tmp73;_tmp79=_tmp74;
goto _LLE0;_LLDF: if(*((int*)_tmp71)!= 1)goto _LLDC;_tmp75=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp71)->f1;_tmp76=*_tmp75;_tmp77=*_tmp76;_tmp78=_tmp77.name;_tmp79=_tmp77.is_xtunion;
_LLE0: {struct Cyc_PP_Doc*_tmp7A=Cyc_PP_text(_tmp79?_tag_arr("xtunion ",sizeof(
char),9): _tag_arr("tunion ",sizeof(char),8));{void*_tmp7B=Cyc_Tcutil_compress(
_tmp50);_LLE2: if((int)_tmp7B != 2)goto _LLE4;_LLE3: s=({struct Cyc_PP_Doc*_tmp7C[3];
_tmp7C[2]=Cyc_Absynpp_tps2doc(_tmp4F);_tmp7C[1]=Cyc_Absynpp_qvar2doc(_tmp78);
_tmp7C[0]=_tmp7A;Cyc_PP_cat(_tag_arr(_tmp7C,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLE1;_LLE4:;_LLE5: s=({struct Cyc_PP_Doc*_tmp7D[5];_tmp7D[4]=Cyc_Absynpp_tps2doc(
_tmp4F);_tmp7D[3]=Cyc_Absynpp_qvar2doc(_tmp78);_tmp7D[2]=Cyc_PP_text(_tag_arr(" ",
sizeof(char),2));_tmp7D[1]=Cyc_Absynpp_typ2doc(_tmp50);_tmp7D[0]=_tmp7A;Cyc_PP_cat(
_tag_arr(_tmp7D,sizeof(struct Cyc_PP_Doc*),5));});goto _LLE1;_LLE1:;}goto _LLDC;}
_LLDC:;}goto _LLA4;_LLB3: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 3)goto _LLB5;
_tmp51=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp47)->f1;_tmp52=(void*)
_tmp51.field_info;_tmp53=_tmp51.targs;_LLB4:{void*_tmp7E=_tmp52;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp7F;struct _tuple0*_tmp80;struct _tuple0*_tmp81;int _tmp82;struct Cyc_Absyn_Tuniondecl*
_tmp83;struct Cyc_Absyn_Tuniondecl _tmp84;struct _tuple0*_tmp85;int _tmp86;struct Cyc_Absyn_Tunionfield*
_tmp87;struct Cyc_Absyn_Tunionfield _tmp88;struct _tuple0*_tmp89;_LLE7: if(*((int*)
_tmp7E)!= 0)goto _LLE9;_tmp7F=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp7E)->f1;
_tmp80=_tmp7F.tunion_name;_tmp81=_tmp7F.field_name;_tmp82=_tmp7F.is_xtunion;
_LLE8: _tmp85=_tmp80;_tmp86=_tmp82;_tmp89=_tmp81;goto _LLEA;_LLE9: if(*((int*)
_tmp7E)!= 1)goto _LLE6;_tmp83=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp7E)->f1;
_tmp84=*_tmp83;_tmp85=_tmp84.name;_tmp86=_tmp84.is_xtunion;_tmp87=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp7E)->f2;_tmp88=*_tmp87;_tmp89=_tmp88.name;_LLEA: {struct Cyc_PP_Doc*_tmp8A=
Cyc_PP_text(_tmp86?_tag_arr("xtunion ",sizeof(char),9): _tag_arr("tunion ",
sizeof(char),8));s=({struct Cyc_PP_Doc*_tmp8B[4];_tmp8B[3]=Cyc_Absynpp_qvar2doc(
_tmp89);_tmp8B[2]=Cyc_PP_text(_tag_arr(".",sizeof(char),2));_tmp8B[1]=Cyc_Absynpp_qvar2doc(
_tmp85);_tmp8B[0]=_tmp8A;Cyc_PP_cat(_tag_arr(_tmp8B,sizeof(struct Cyc_PP_Doc*),4));});
goto _LLE6;}_LLE6:;}goto _LLA4;_LLB5: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 5)
goto _LLB7;_tmp54=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp47)->f1;_tmp55=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp47)->f2;_LLB6: {struct _tagged_arr sns;
struct _tagged_arr ts;{void*_tmp8C=_tmp54;_LLEC: if((int)_tmp8C != 2)goto _LLEE;_LLED:
goto _LLEF;_LLEE: if((int)_tmp8C != 0)goto _LLF0;_LLEF: sns=_tag_arr("",sizeof(char),
1);goto _LLEB;_LLF0: if((int)_tmp8C != 1)goto _LLEB;_LLF1: sns=_tag_arr("unsigned ",
sizeof(char),10);goto _LLEB;_LLEB:;}{void*_tmp8D=_tmp55;_LLF3: if((int)_tmp8D != 0)
goto _LLF5;_LLF4:{void*_tmp8E=_tmp54;_LLFC: if((int)_tmp8E != 2)goto _LLFE;_LLFD: sns=
_tag_arr("",sizeof(char),1);goto _LLFB;_LLFE: if((int)_tmp8E != 0)goto _LL100;_LLFF:
sns=_tag_arr("signed ",sizeof(char),8);goto _LLFB;_LL100: if((int)_tmp8E != 1)goto
_LLFB;_LL101: sns=_tag_arr("unsigned ",sizeof(char),10);goto _LLFB;_LLFB:;}ts=
_tag_arr("char",sizeof(char),5);goto _LLF2;_LLF5: if((int)_tmp8D != 1)goto _LLF7;
_LLF6: ts=_tag_arr("short",sizeof(char),6);goto _LLF2;_LLF7: if((int)_tmp8D != 2)
goto _LLF9;_LLF8: ts=_tag_arr("int",sizeof(char),4);goto _LLF2;_LLF9: if((int)_tmp8D
!= 3)goto _LLF2;_LLFA: ts=Cyc_Absynpp_to_VC?_tag_arr("__int64",sizeof(char),8):
_tag_arr("long long",sizeof(char),10);goto _LLF2;_LLF2:;}s=Cyc_PP_text((struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp91;_tmp91.tag=0;_tmp91.f1=(
struct _tagged_arr)ts;{struct Cyc_Std_String_pa_struct _tmp90;_tmp90.tag=0;_tmp90.f1=(
struct _tagged_arr)sns;{void*_tmp8F[2]={& _tmp90,& _tmp91};Cyc_Std_aprintf(_tag_arr("%s%s",
sizeof(char),5),_tag_arr(_tmp8F,sizeof(void*),2));}}}));goto _LLA4;}_LLB7: if((int)
_tmp47 != 1)goto _LLB9;_LLB8: s=Cyc_PP_text(_tag_arr("float",sizeof(char),6));goto
_LLA4;_LLB9: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 6)goto _LLBB;_tmp56=((struct
Cyc_Absyn_DoubleType_struct*)_tmp47)->f1;_LLBA: if(_tmp56)s=Cyc_PP_text(_tag_arr("long double",
sizeof(char),12));else{s=Cyc_PP_text(_tag_arr("double",sizeof(char),7));}goto
_LLA4;_LLBB: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 9)goto _LLBD;_tmp57=((struct
Cyc_Absyn_TupleType_struct*)_tmp47)->f1;_LLBC: s=({struct Cyc_PP_Doc*_tmp92[2];
_tmp92[1]=Cyc_Absynpp_args2doc(_tmp57);_tmp92[0]=Cyc_PP_text(_tag_arr("$",
sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp92,sizeof(struct Cyc_PP_Doc*),2));});
goto _LLA4;_LLBD: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 10)goto _LLBF;_tmp58=((
struct Cyc_Absyn_AggrType_struct*)_tmp47)->f1;_tmp59=(void*)_tmp58.aggr_info;
_tmp5A=_tmp58.targs;_LLBE: {void*_tmp94;struct _tuple0*_tmp95;struct _tuple3 _tmp93=
Cyc_Absyn_aggr_kinded_name(_tmp59);_tmp94=_tmp93.f1;_tmp95=_tmp93.f2;s=({struct
Cyc_PP_Doc*_tmp96[3];_tmp96[2]=Cyc_Absynpp_tps2doc(_tmp5A);_tmp96[1]=Cyc_Absynpp_qvar2doc(
_tmp95);_tmp96[0]=Cyc_Absynpp_aggr_kind2doc(_tmp94);Cyc_PP_cat(_tag_arr(_tmp96,
sizeof(struct Cyc_PP_Doc*),3));});goto _LLA4;}_LLBF: if(_tmp47 <= (void*)3?1:*((int*)
_tmp47)!= 11)goto _LLC1;_tmp5B=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp47)->f1;_tmp5C=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp47)->f2;_LLC0: s=({
struct Cyc_PP_Doc*_tmp97[4];_tmp97[3]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp97[2]=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp5C));_tmp97[1]=Cyc_PP_text(
_tag_arr("{",sizeof(char),2));_tmp97[0]=Cyc_Absynpp_aggr_kind2doc(_tmp5B);Cyc_PP_cat(
_tag_arr(_tmp97,sizeof(struct Cyc_PP_Doc*),4));});goto _LLA4;_LLC1: if(_tmp47 <= (
void*)3?1:*((int*)_tmp47)!= 13)goto _LLC3;_tmp5D=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp47)->f1;_LLC2: s=({struct Cyc_PP_Doc*_tmp98[3];_tmp98[2]=Cyc_PP_text(_tag_arr("}",
sizeof(char),2));_tmp98[1]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp5D));
_tmp98[0]=Cyc_PP_text(_tag_arr("enum {",sizeof(char),7));Cyc_PP_cat(_tag_arr(
_tmp98,sizeof(struct Cyc_PP_Doc*),3));});goto _LLA4;_LLC3: if(_tmp47 <= (void*)3?1:*((
int*)_tmp47)!= 12)goto _LLC5;_tmp5E=((struct Cyc_Absyn_EnumType_struct*)_tmp47)->f1;
_LLC4: s=({struct Cyc_PP_Doc*_tmp99[2];_tmp99[1]=Cyc_Absynpp_qvar2doc(_tmp5E);
_tmp99[0]=Cyc_PP_text(_tag_arr("enum ",sizeof(char),6));Cyc_PP_cat(_tag_arr(
_tmp99,sizeof(struct Cyc_PP_Doc*),2));});goto _LLA4;_LLC5: if(_tmp47 <= (void*)3?1:*((
int*)_tmp47)!= 14)goto _LLC7;_tmp5F=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp47)->f1;_LLC6: s=({struct Cyc_PP_Doc*_tmp9A[3];_tmp9A[2]=Cyc_PP_text(_tag_arr(">",
sizeof(char),2));_tmp9A[1]=Cyc_Absynpp_typ2doc(_tmp5F);_tmp9A[0]=Cyc_PP_text(
_tag_arr("sizeof_t<",sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp9A,sizeof(struct
Cyc_PP_Doc*),3));});goto _LLA4;_LLC7: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 16)
goto _LLC9;_tmp60=((struct Cyc_Absyn_TypedefType_struct*)_tmp47)->f1;_tmp61=((
struct Cyc_Absyn_TypedefType_struct*)_tmp47)->f2;_tmp62=((struct Cyc_Absyn_TypedefType_struct*)
_tmp47)->f3;_tmp63=((struct Cyc_Absyn_TypedefType_struct*)_tmp47)->f4;_LLC8: s=({
struct Cyc_PP_Doc*_tmp9B[2];_tmp9B[1]=Cyc_Absynpp_tps2doc(_tmp61);_tmp9B[0]=Cyc_Absynpp_qvar2doc(
_tmp60);Cyc_PP_cat(_tag_arr(_tmp9B,sizeof(struct Cyc_PP_Doc*),2));});goto _LLA4;
_LLC9: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 15)goto _LLCB;_tmp64=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp47)->f1;_LLCA: s=({struct Cyc_PP_Doc*
_tmp9C[3];_tmp9C[2]=Cyc_PP_text(_tag_arr(">",sizeof(char),2));_tmp9C[1]=Cyc_Absynpp_rgn2doc(
_tmp64);_tmp9C[0]=Cyc_PP_text(_tag_arr("region_t<",sizeof(char),10));Cyc_PP_cat(
_tag_arr(_tmp9C,sizeof(struct Cyc_PP_Doc*),3));});goto _LLA4;_LLCB: if(_tmp47 <= (
void*)3?1:*((int*)_tmp47)!= 18)goto _LLCD;_tmp65=((struct Cyc_Absyn_TypeInt_struct*)
_tmp47)->f1;_LLCC: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp9E;_tmp9E.tag=1;_tmp9E.f1=(int)((unsigned int)_tmp65);{void*_tmp9D[1]={&
_tmp9E};Cyc_Std_aprintf(_tag_arr("`%d",sizeof(char),4),_tag_arr(_tmp9D,sizeof(
void*),1));}}));goto _LLA4;_LLCD: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 17)goto
_LLCF;_tmp66=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp47)->f1;_LLCE: s=({
struct Cyc_PP_Doc*_tmp9F[3];_tmp9F[2]=Cyc_PP_text(_tag_arr(">",sizeof(char),2));
_tmp9F[1]=Cyc_Absynpp_typ2doc(_tmp66);_tmp9F[0]=Cyc_PP_text(_tag_arr("tag_t<",
sizeof(char),7));Cyc_PP_cat(_tag_arr(_tmp9F,sizeof(struct Cyc_PP_Doc*),3));});
goto _LLA4;_LLCF: if((int)_tmp47 != 2)goto _LLD1;_LLD0: s=Cyc_Absynpp_rgn2doc(t);goto
_LLA4;_LLD1: if(_tmp47 <= (void*)3?1:*((int*)_tmp47)!= 21)goto _LLD3;_tmp67=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp47)->f1;_LLD2: s=({struct Cyc_PP_Doc*_tmpA0[3];
_tmpA0[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmpA0[1]=Cyc_Absynpp_typ2doc(
_tmp67);_tmpA0[0]=Cyc_PP_text(_tag_arr("regions(",sizeof(char),9));Cyc_PP_cat(
_tag_arr(_tmpA0,sizeof(struct Cyc_PP_Doc*),3));});goto _LLA4;_LLD3: if(_tmp47 <= (
void*)3?1:*((int*)_tmp47)!= 19)goto _LLD5;_LLD4: goto _LLD6;_LLD5: if(_tmp47 <= (void*)
3?1:*((int*)_tmp47)!= 20)goto _LLA4;_LLD6: s=Cyc_Absynpp_eff2doc(t);goto _LLA4;
_LLA4:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _tagged_arr*)vo->v));}struct
Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple5*cmp){struct _tuple5 _tmpA2;void*
_tmpA3;void*_tmpA4;struct _tuple5*_tmpA1=cmp;_tmpA2=*_tmpA1;_tmpA3=_tmpA2.f1;
_tmpA4=_tmpA2.f2;return({struct Cyc_PP_Doc*_tmpA5[3];_tmpA5[2]=Cyc_Absynpp_rgn2doc(
_tmpA4);_tmpA5[1]=Cyc_PP_text(_tag_arr(" > ",sizeof(char),4));_tmpA5[0]=Cyc_Absynpp_rgn2doc(
_tmpA3);Cyc_PP_cat(_tag_arr(_tmpA5,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*
Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){return Cyc_PP_group(_tag_arr("",
sizeof(char),1),_tag_arr("",sizeof(char),1),_tag_arr(",",sizeof(char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple5*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(
struct _tuple1*t){struct Cyc_Core_Opt*dopt=(*t).f1 == 0?0:({struct Cyc_Core_Opt*
_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->v=Cyc_PP_text(*((struct _tagged_arr*)((
struct Cyc_Core_Opt*)_check_null((*t).f1))->v));_tmpA6;});return Cyc_Absynpp_tqtd2doc((*
t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*_tmpA7=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,
args);struct Cyc_PP_Doc*eff_doc;if(c_varargs)_tmpA7=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA7,({struct Cyc_List_List*
_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->hd=Cyc_PP_text(_tag_arr("...",sizeof(
char),4));_tmpA8->tl=0;_tmpA8;}));else{if(cyc_varargs != 0){struct Cyc_PP_Doc*
_tmpA9=({struct Cyc_PP_Doc*_tmpAB[3];_tmpAB[2]=Cyc_Absynpp_funarg2doc(({struct
_tuple1*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->f1=cyc_varargs->name;_tmpAC->f2=
cyc_varargs->tq;_tmpAC->f3=(void*)cyc_varargs->type;_tmpAC;}));_tmpAB[1]=
cyc_varargs->inject?Cyc_PP_text(_tag_arr(" inject ",sizeof(char),9)): Cyc_PP_text(
_tag_arr(" ",sizeof(char),2));_tmpAB[0]=Cyc_PP_text(_tag_arr("...",sizeof(char),
4));Cyc_PP_cat(_tag_arr(_tmpAB,sizeof(struct Cyc_PP_Doc*),3));});_tmpA7=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmpA7,({struct Cyc_List_List*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->hd=_tmpA9;
_tmpAA->tl=0;_tmpAA;}));}}{struct Cyc_PP_Doc*_tmpAD=Cyc_PP_group(_tag_arr("",
sizeof(char),1),_tag_arr("",sizeof(char),1),_tag_arr(",",sizeof(char),2),_tmpA7);
if(effopt != 0)_tmpAD=({struct Cyc_PP_Doc*_tmpAE[3];_tmpAE[2]=Cyc_Absynpp_eff2doc((
void*)effopt->v);_tmpAE[1]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmpAE[0]=
_tmpAD;Cyc_PP_cat(_tag_arr(_tmpAE,sizeof(struct Cyc_PP_Doc*),3));});if(rgn_po != 0)
_tmpAD=({struct Cyc_PP_Doc*_tmpAF[3];_tmpAF[2]=Cyc_Absynpp_rgnpo2doc(rgn_po);
_tmpAF[1]=Cyc_PP_text(_tag_arr(":",sizeof(char),2));_tmpAF[0]=_tmpAD;Cyc_PP_cat(
_tag_arr(_tmpAF,sizeof(struct Cyc_PP_Doc*),3));});return({struct Cyc_PP_Doc*_tmpB0[
3];_tmpB0[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmpB0[1]=_tmpAD;_tmpB0[0]=
Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmpB0,sizeof(
struct Cyc_PP_Doc*),3));});}}struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*
arg){return({struct _tuple1*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->f1=({struct
Cyc_Core_Opt*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->v=(*arg).f1;_tmpB2;});
_tmpB1->f2=(*arg).f2;_tmpB1->f3=(*arg).f3;_tmpB1;});}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(
struct _tagged_arr*v){return Cyc_PP_text(*v);}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q){struct Cyc_List_List*_tmpB3=0;int match;{void*_tmpB4=(*q).f1;
struct Cyc_List_List*_tmpB5;struct Cyc_List_List*_tmpB6;_LL103: if((int)_tmpB4 != 0)
goto _LL105;_LL104: _tmpB5=0;goto _LL106;_LL105: if(_tmpB4 <= (void*)1?1:*((int*)
_tmpB4)!= 0)goto _LL107;_tmpB5=((struct Cyc_Absyn_Rel_n_struct*)_tmpB4)->f1;_LL106:
match=0;_tmpB3=_tmpB5;goto _LL102;_LL107: if(_tmpB4 <= (void*)1?1:*((int*)_tmpB4)!= 
1)goto _LL102;_tmpB6=((struct Cyc_Absyn_Abs_n_struct*)_tmpB4)->f1;_LL108: match=Cyc_Absynpp_use_curr_namespace?((
int(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_Std_strptrcmp,_tmpB6,Cyc_Absynpp_curr_namespace):
0;_tmpB3=(Cyc_Absynpp_qvar_to_Cids?Cyc_Absynpp_add_cyc_prefix: 0)?({struct Cyc_List_List*
_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->hd=Cyc_Absynpp_cyc_stringptr;_tmpB7->tl=
_tmpB6;_tmpB7;}): _tmpB6;goto _LL102;_LL102:;}if(Cyc_Absynpp_qvar_to_Cids)return
Cyc_PP_text((struct _tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB3,({struct Cyc_List_List*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->hd=(*q).f2;_tmpB8->tl=0;_tmpB8;})),
_tag_arr("_",sizeof(char),2)));else{if(match)return Cyc_Absynpp_var2doc((*q).f2);
else{return Cyc_PP_text((struct _tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB3,({struct Cyc_List_List*
_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->hd=(*q).f2;_tmpB9->tl=0;_tmpB9;})),
_tag_arr("::",sizeof(char),3)));}}}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(
struct _tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2doc(v);if(
Cyc_Absynpp_use_curr_namespace){void*_tmpBA=(*v).f1;struct Cyc_List_List*_tmpBB;
struct Cyc_List_List*_tmpBC;_LL10A: if((int)_tmpBA != 0)goto _LL10C;_LL10B: goto
_LL10D;_LL10C: if(_tmpBA <= (void*)1?1:*((int*)_tmpBA)!= 0)goto _LL10E;_tmpBB=((
struct Cyc_Absyn_Rel_n_struct*)_tmpBA)->f1;if(_tmpBB != 0)goto _LL10E;_LL10D: return
Cyc_Absynpp_var2doc((*v).f2);_LL10E: if(_tmpBA <= (void*)1?1:*((int*)_tmpBA)!= 1)
goto _LL110;_tmpBC=((struct Cyc_Absyn_Abs_n_struct*)_tmpBA)->f1;_LL10F: if(((int(*)(
int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Std_strptrcmp,_tmpBC,Cyc_Absynpp_curr_namespace)
== 0)return Cyc_Absynpp_var2doc((*v).f2);else{goto _LL111;}_LL110:;_LL111: return({
struct Cyc_PP_Doc*_tmpBD[2];_tmpBD[1]=Cyc_Absynpp_qvar2doc(v);_tmpBD[0]=Cyc_PP_text(
_tag_arr("/* bad namespace : */ ",sizeof(char),23));Cyc_PP_cat(_tag_arr(_tmpBD,
sizeof(struct Cyc_PP_Doc*),2));});_LL109:;}else{return Cyc_Absynpp_var2doc((*v).f2);}}
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(),
t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmpBE=(void*)e->r;
void*_tmpBF;struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_Absyn_Exp*_tmpC1;_LL113: if(*((
int*)_tmpBE)!= 0)goto _LL115;_LL114: goto _LL116;_LL115: if(*((int*)_tmpBE)!= 1)goto
_LL117;_LL116: goto _LL118;_LL117: if(*((int*)_tmpBE)!= 2)goto _LL119;_LL118: return
10000;_LL119: if(*((int*)_tmpBE)!= 3)goto _LL11B;_tmpBF=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmpBE)->f1;_LL11A: {void*_tmpC2=_tmpBF;_LL160: if((int)_tmpC2 != 0)goto _LL162;
_LL161: return 100;_LL162: if((int)_tmpC2 != 1)goto _LL164;_LL163: return 110;_LL164:
if((int)_tmpC2 != 2)goto _LL166;_LL165: return 100;_LL166: if((int)_tmpC2 != 3)goto
_LL168;_LL167: goto _LL169;_LL168: if((int)_tmpC2 != 4)goto _LL16A;_LL169: return 110;
_LL16A: if((int)_tmpC2 != 5)goto _LL16C;_LL16B: goto _LL16D;_LL16C: if((int)_tmpC2 != 6)
goto _LL16E;_LL16D: return 70;_LL16E: if((int)_tmpC2 != 7)goto _LL170;_LL16F: goto
_LL171;_LL170: if((int)_tmpC2 != 8)goto _LL172;_LL171: goto _LL173;_LL172: if((int)
_tmpC2 != 9)goto _LL174;_LL173: goto _LL175;_LL174: if((int)_tmpC2 != 10)goto _LL176;
_LL175: return 80;_LL176: if((int)_tmpC2 != 11)goto _LL178;_LL177: goto _LL179;_LL178:
if((int)_tmpC2 != 12)goto _LL17A;_LL179: return 130;_LL17A: if((int)_tmpC2 != 13)goto
_LL17C;_LL17B: return 60;_LL17C: if((int)_tmpC2 != 14)goto _LL17E;_LL17D: return 40;
_LL17E: if((int)_tmpC2 != 15)goto _LL180;_LL17F: return 50;_LL180: if((int)_tmpC2 != 16)
goto _LL182;_LL181: return 90;_LL182: if((int)_tmpC2 != 17)goto _LL184;_LL183: return 80;
_LL184: if((int)_tmpC2 != 18)goto _LL186;_LL185: return 80;_LL186: if((int)_tmpC2 != 19)
goto _LL15F;_LL187: return 140;_LL15F:;}_LL11B: if(*((int*)_tmpBE)!= 4)goto _LL11D;
_LL11C: return 20;_LL11D: if(*((int*)_tmpBE)!= 5)goto _LL11F;_LL11E: return 130;_LL11F:
if(*((int*)_tmpBE)!= 6)goto _LL121;_LL120: return 30;_LL121: if(*((int*)_tmpBE)!= 7)
goto _LL123;_LL122: return 10;_LL123: if(*((int*)_tmpBE)!= 8)goto _LL125;_LL124: goto
_LL126;_LL125: if(*((int*)_tmpBE)!= 9)goto _LL127;_LL126: return 140;_LL127: if(*((
int*)_tmpBE)!= 10)goto _LL129;_LL128: return 130;_LL129: if(*((int*)_tmpBE)!= 11)
goto _LL12B;_tmpC0=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpBE)->f1;_LL12A:
return Cyc_Absynpp_exp_prec(_tmpC0);_LL12B: if(*((int*)_tmpBE)!= 12)goto _LL12D;
_tmpC1=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpBE)->f1;_LL12C: return Cyc_Absynpp_exp_prec(
_tmpC1);_LL12D: if(*((int*)_tmpBE)!= 13)goto _LL12F;_LL12E: return 120;_LL12F: if(*((
int*)_tmpBE)!= 15)goto _LL131;_LL130: return 15;_LL131: if(*((int*)_tmpBE)!= 14)goto
_LL133;_LL132: goto _LL134;_LL133: if(*((int*)_tmpBE)!= 16)goto _LL135;_LL134: goto
_LL136;_LL135: if(*((int*)_tmpBE)!= 17)goto _LL137;_LL136: goto _LL138;_LL137: if(*((
int*)_tmpBE)!= 18)goto _LL139;_LL138: goto _LL13A;_LL139: if(*((int*)_tmpBE)!= 19)
goto _LL13B;_LL13A: goto _LL13C;_LL13B: if(*((int*)_tmpBE)!= 20)goto _LL13D;_LL13C:
return 130;_LL13D: if(*((int*)_tmpBE)!= 21)goto _LL13F;_LL13E: goto _LL140;_LL13F: if(*((
int*)_tmpBE)!= 22)goto _LL141;_LL140: goto _LL142;_LL141: if(*((int*)_tmpBE)!= 23)
goto _LL143;_LL142: return 140;_LL143: if(*((int*)_tmpBE)!= 24)goto _LL145;_LL144:
return 150;_LL145: if(*((int*)_tmpBE)!= 25)goto _LL147;_LL146: goto _LL148;_LL147: if(*((
int*)_tmpBE)!= 26)goto _LL149;_LL148: goto _LL14A;_LL149: if(*((int*)_tmpBE)!= 27)
goto _LL14B;_LL14A: goto _LL14C;_LL14B: if(*((int*)_tmpBE)!= 28)goto _LL14D;_LL14C:
goto _LL14E;_LL14D: if(*((int*)_tmpBE)!= 29)goto _LL14F;_LL14E: goto _LL150;_LL14F:
if(*((int*)_tmpBE)!= 30)goto _LL151;_LL150: goto _LL152;_LL151: if(*((int*)_tmpBE)!= 
31)goto _LL153;_LL152: goto _LL154;_LL153: if(*((int*)_tmpBE)!= 32)goto _LL155;_LL154:
goto _LL156;_LL155: if(*((int*)_tmpBE)!= 33)goto _LL157;_LL156: goto _LL158;_LL157:
if(*((int*)_tmpBE)!= 34)goto _LL159;_LL158: return 140;_LL159: if(*((int*)_tmpBE)!= 
35)goto _LL15B;_LL15A: return 10000;_LL15B: if(*((int*)_tmpBE)!= 36)goto _LL15D;
_LL15C: goto _LL15E;_LL15D: if(*((int*)_tmpBE)!= 37)goto _LL112;_LL15E: return 140;
_LL112:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(
0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*s;{void*_tmpC3=(void*)e->r;
void*_tmpC4;struct _tuple0*_tmpC5;struct _tuple0*_tmpC6;void*_tmpC7;struct Cyc_List_List*
_tmpC8;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Core_Opt*_tmpCA;struct Cyc_Absyn_Exp*
_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;void*_tmpCD;struct Cyc_Absyn_Exp*_tmpCE;struct
Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpD1;
struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_List_List*_tmpD4;
struct Cyc_Absyn_Exp*_tmpD5;struct Cyc_List_List*_tmpD6;struct Cyc_Absyn_Exp*_tmpD7;
struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD9;void*_tmpDA;struct Cyc_Absyn_Exp*
_tmpDB;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*
_tmpDE;void*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;void*_tmpE1;void*_tmpE2;struct
_tagged_arr*_tmpE3;void*_tmpE4;void*_tmpE5;unsigned int _tmpE6;struct Cyc_List_List*
_tmpE7;void*_tmpE8;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;struct
_tagged_arr*_tmpEB;struct Cyc_Absyn_Exp*_tmpEC;struct _tagged_arr*_tmpED;struct Cyc_Absyn_Exp*
_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_List_List*_tmpF0;struct _tuple1*
_tmpF1;struct Cyc_List_List*_tmpF2;struct Cyc_List_List*_tmpF3;struct Cyc_Absyn_Vardecl*
_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF6;struct _tuple0*
_tmpF7;struct Cyc_List_List*_tmpF8;struct Cyc_List_List*_tmpF9;struct Cyc_List_List*
_tmpFA;struct Cyc_List_List*_tmpFB;struct Cyc_Absyn_Tunionfield*_tmpFC;struct
_tuple0*_tmpFD;struct _tuple0*_tmpFE;struct Cyc_Absyn_MallocInfo _tmpFF;int _tmp100;
struct Cyc_Absyn_Exp*_tmp101;void**_tmp102;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Core_Opt*
_tmp104;struct Cyc_List_List*_tmp105;struct Cyc_Absyn_Stmt*_tmp106;struct Cyc_Absyn_Fndecl*
_tmp107;struct Cyc_Absyn_Exp*_tmp108;_LL189: if(*((int*)_tmpC3)!= 0)goto _LL18B;
_tmpC4=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmpC3)->f1;_LL18A: s=Cyc_Absynpp_cnst2doc(
_tmpC4);goto _LL188;_LL18B: if(*((int*)_tmpC3)!= 1)goto _LL18D;_tmpC5=((struct Cyc_Absyn_Var_e_struct*)
_tmpC3)->f1;_LL18C: _tmpC6=_tmpC5;goto _LL18E;_LL18D: if(*((int*)_tmpC3)!= 2)goto
_LL18F;_tmpC6=((struct Cyc_Absyn_UnknownId_e_struct*)_tmpC3)->f1;_LL18E: s=Cyc_Absynpp_qvar2doc(
_tmpC6);goto _LL188;_LL18F: if(*((int*)_tmpC3)!= 3)goto _LL191;_tmpC7=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpC3)->f1;_tmpC8=((struct Cyc_Absyn_Primop_e_struct*)
_tmpC3)->f2;_LL190: s=Cyc_Absynpp_primapp2doc(myprec,_tmpC7,_tmpC8);goto _LL188;
_LL191: if(*((int*)_tmpC3)!= 4)goto _LL193;_tmpC9=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpC3)->f1;_tmpCA=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpC3)->f2;_tmpCB=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpC3)->f3;_LL192: s=({struct Cyc_PP_Doc*
_tmp109[5];_tmp109[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpCB);_tmp109[3]=Cyc_PP_text(
_tag_arr("= ",sizeof(char),3));_tmp109[2]=_tmpCA == 0?Cyc_PP_text(_tag_arr("",
sizeof(char),1)): Cyc_Absynpp_prim2doc((void*)_tmpCA->v);_tmp109[1]=Cyc_PP_text(
_tag_arr(" ",sizeof(char),2));_tmp109[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpC9);
Cyc_PP_cat(_tag_arr(_tmp109,sizeof(struct Cyc_PP_Doc*),5));});goto _LL188;_LL193:
if(*((int*)_tmpC3)!= 5)goto _LL195;_tmpCC=((struct Cyc_Absyn_Increment_e_struct*)
_tmpC3)->f1;_tmpCD=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpC3)->f2;
_LL194: {struct Cyc_PP_Doc*_tmp10A=Cyc_Absynpp_exp2doc_prec(myprec,_tmpCC);{void*
_tmp10B=_tmpCD;_LL1D8: if((int)_tmp10B != 0)goto _LL1DA;_LL1D9: s=({struct Cyc_PP_Doc*
_tmp10C[2];_tmp10C[1]=_tmp10A;_tmp10C[0]=Cyc_PP_text(_tag_arr("++",sizeof(char),
3));Cyc_PP_cat(_tag_arr(_tmp10C,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1D7;
_LL1DA: if((int)_tmp10B != 2)goto _LL1DC;_LL1DB: s=({struct Cyc_PP_Doc*_tmp10D[2];
_tmp10D[1]=_tmp10A;_tmp10D[0]=Cyc_PP_text(_tag_arr("--",sizeof(char),3));Cyc_PP_cat(
_tag_arr(_tmp10D,sizeof(struct Cyc_PP_Doc*),2));});goto _LL1D7;_LL1DC: if((int)
_tmp10B != 1)goto _LL1DE;_LL1DD: s=({struct Cyc_PP_Doc*_tmp10E[2];_tmp10E[1]=Cyc_PP_text(
_tag_arr("++",sizeof(char),3));_tmp10E[0]=_tmp10A;Cyc_PP_cat(_tag_arr(_tmp10E,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL1D7;_LL1DE: if((int)_tmp10B != 3)goto
_LL1D7;_LL1DF: s=({struct Cyc_PP_Doc*_tmp10F[2];_tmp10F[1]=Cyc_PP_text(_tag_arr("--",
sizeof(char),3));_tmp10F[0]=_tmp10A;Cyc_PP_cat(_tag_arr(_tmp10F,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL1D7;_LL1D7:;}goto _LL188;}_LL195: if(*((int*)_tmpC3)!= 6)goto _LL197;
_tmpCE=((struct Cyc_Absyn_Conditional_e_struct*)_tmpC3)->f1;_tmpCF=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpC3)->f2;_tmpD0=((struct Cyc_Absyn_Conditional_e_struct*)_tmpC3)->f3;_LL196:{
struct _tuple5 _tmp111=({struct _tuple5 _tmp110;_tmp110.f1=(void*)_tmpCF->r;_tmp110.f2=(
void*)_tmpD0->r;_tmp110;});_LL1E1:;_LL1E2: s=({struct Cyc_PP_Doc*_tmp112[5];
_tmp112[4]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpD0);_tmp112[3]=Cyc_PP_text(
_tag_arr(" : ",sizeof(char),4));_tmp112[2]=Cyc_Absynpp_exp2doc_prec(0,_tmpCF);
_tmp112[1]=Cyc_PP_text(_tag_arr(" ? ",sizeof(char),4));_tmp112[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpCE);Cyc_PP_cat(_tag_arr(_tmp112,sizeof(struct Cyc_PP_Doc*),5));});goto
_LL1E0;_LL1E0:;}goto _LL188;_LL197: if(*((int*)_tmpC3)!= 7)goto _LL199;_tmpD1=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmpC3)->f1;_tmpD2=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpC3)->f2;_LL198: s=({struct Cyc_PP_Doc*_tmp113[5];_tmp113[4]=Cyc_PP_text(
_tag_arr(")",sizeof(char),2));_tmp113[3]=Cyc_Absynpp_exp2doc(_tmpD2);_tmp113[2]=
Cyc_PP_text(_tag_arr(", ",sizeof(char),3));_tmp113[1]=Cyc_Absynpp_exp2doc(_tmpD1);
_tmp113[0]=Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp113,
sizeof(struct Cyc_PP_Doc*),5));});goto _LL188;_LL199: if(*((int*)_tmpC3)!= 8)goto
_LL19B;_tmpD3=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmpC3)->f1;_tmpD4=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmpC3)->f2;_LL19A: s=({struct Cyc_PP_Doc*
_tmp114[4];_tmp114[3]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp114[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmpD4);_tmp114[1]=Cyc_PP_text(_tag_arr("(",sizeof(char),2));_tmp114[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpD3);Cyc_PP_cat(_tag_arr(_tmp114,sizeof(struct Cyc_PP_Doc*),4));});goto
_LL188;_LL19B: if(*((int*)_tmpC3)!= 9)goto _LL19D;_tmpD5=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpC3)->f1;_tmpD6=((struct Cyc_Absyn_FnCall_e_struct*)_tmpC3)->f2;_LL19C: s=({
struct Cyc_PP_Doc*_tmp115[4];_tmp115[3]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp115[2]=Cyc_Absynpp_exps2doc_prec(20,_tmpD6);_tmp115[1]=Cyc_PP_text(_tag_arr("(",
sizeof(char),2));_tmp115[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpD5);Cyc_PP_cat(
_tag_arr(_tmp115,sizeof(struct Cyc_PP_Doc*),4));});goto _LL188;_LL19D: if(*((int*)
_tmpC3)!= 10)goto _LL19F;_tmpD7=((struct Cyc_Absyn_Throw_e_struct*)_tmpC3)->f1;
_LL19E: s=({struct Cyc_PP_Doc*_tmp116[2];_tmp116[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpD7);_tmp116[0]=Cyc_PP_text(_tag_arr("throw ",sizeof(char),7));Cyc_PP_cat(
_tag_arr(_tmp116,sizeof(struct Cyc_PP_Doc*),2));});goto _LL188;_LL19F: if(*((int*)
_tmpC3)!= 11)goto _LL1A1;_tmpD8=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpC3)->f1;
_LL1A0: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmpD8);goto _LL188;_LL1A1: if(*((int*)
_tmpC3)!= 12)goto _LL1A3;_tmpD9=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpC3)->f1;
_LL1A2: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmpD9);goto _LL188;_LL1A3: if(*((int*)
_tmpC3)!= 13)goto _LL1A5;_tmpDA=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpC3)->f1;
_tmpDB=((struct Cyc_Absyn_Cast_e_struct*)_tmpC3)->f2;_LL1A4: s=({struct Cyc_PP_Doc*
_tmp117[4];_tmp117[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpDB);_tmp117[2]=Cyc_PP_text(
_tag_arr(")",sizeof(char),2));_tmp117[1]=Cyc_Absynpp_typ2doc(_tmpDA);_tmp117[0]=
Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp117,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL188;_LL1A5: if(*((int*)_tmpC3)!= 14)goto _LL1A7;
_tmpDC=((struct Cyc_Absyn_Address_e_struct*)_tmpC3)->f1;_LL1A6: s=({struct Cyc_PP_Doc*
_tmp118[2];_tmp118[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpDC);_tmp118[0]=Cyc_PP_text(
_tag_arr("&",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp118,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL188;_LL1A7: if(*((int*)_tmpC3)!= 15)goto _LL1A9;_tmpDD=((struct Cyc_Absyn_New_e_struct*)
_tmpC3)->f1;_tmpDE=((struct Cyc_Absyn_New_e_struct*)_tmpC3)->f2;_LL1A8: if(_tmpDD
== 0)s=({struct Cyc_PP_Doc*_tmp119[2];_tmp119[1]=Cyc_Absynpp_exp2doc_prec(myprec,
_tmpDE);_tmp119[0]=Cyc_PP_text(_tag_arr("new ",sizeof(char),5));Cyc_PP_cat(
_tag_arr(_tmp119,sizeof(struct Cyc_PP_Doc*),2));});else{s=({struct Cyc_PP_Doc*
_tmp11A[4];_tmp11A[3]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpDE);_tmp11A[2]=Cyc_PP_text(
_tag_arr(") ",sizeof(char),3));_tmp11A[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmpDD));_tmp11A[0]=Cyc_PP_text(_tag_arr("rnew(",sizeof(char),6));
Cyc_PP_cat(_tag_arr(_tmp11A,sizeof(struct Cyc_PP_Doc*),4));});}goto _LL188;_LL1A9:
if(*((int*)_tmpC3)!= 16)goto _LL1AB;_tmpDF=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpC3)->f1;_LL1AA: s=({struct Cyc_PP_Doc*_tmp11B[3];_tmp11B[2]=Cyc_PP_text(
_tag_arr(")",sizeof(char),2));_tmp11B[1]=Cyc_Absynpp_typ2doc(_tmpDF);_tmp11B[0]=
Cyc_PP_text(_tag_arr("sizeof(",sizeof(char),8));Cyc_PP_cat(_tag_arr(_tmp11B,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL188;_LL1AB: if(*((int*)_tmpC3)!= 17)goto
_LL1AD;_tmpE0=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpC3)->f1;_LL1AC: s=({
struct Cyc_PP_Doc*_tmp11C[3];_tmp11C[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp11C[1]=Cyc_Absynpp_exp2doc(_tmpE0);_tmp11C[0]=Cyc_PP_text(_tag_arr("sizeof(",
sizeof(char),8));Cyc_PP_cat(_tag_arr(_tmp11C,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL188;_LL1AD: if(*((int*)_tmpC3)!= 18)goto _LL1AF;_tmpE1=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpC3)->f1;_tmpE2=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpC3)->f2;if(*((
int*)_tmpE2)!= 0)goto _LL1AF;_tmpE3=((struct Cyc_Absyn_StructField_struct*)_tmpE2)->f1;
_LL1AE: s=({struct Cyc_PP_Doc*_tmp11D[5];_tmp11D[4]=Cyc_PP_text(_tag_arr(")",
sizeof(char),2));_tmp11D[3]=Cyc_PP_textptr(_tmpE3);_tmp11D[2]=Cyc_PP_text(
_tag_arr(",",sizeof(char),2));_tmp11D[1]=Cyc_Absynpp_typ2doc(_tmpE1);_tmp11D[0]=
Cyc_PP_text(_tag_arr("offsetof(",sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp11D,
sizeof(struct Cyc_PP_Doc*),5));});goto _LL188;_LL1AF: if(*((int*)_tmpC3)!= 18)goto
_LL1B1;_tmpE4=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpC3)->f1;_tmpE5=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpC3)->f2;if(*((int*)_tmpE5)!= 1)
goto _LL1B1;_tmpE6=((struct Cyc_Absyn_TupleIndex_struct*)_tmpE5)->f1;_LL1B0: s=({
struct Cyc_PP_Doc*_tmp11E[5];_tmp11E[4]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp11E[3]=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp120;
_tmp120.tag=1;_tmp120.f1=(int)_tmpE6;{void*_tmp11F[1]={& _tmp120};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(char),3),_tag_arr(_tmp11F,sizeof(void*),1));}}));_tmp11E[2]=
Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp11E[1]=Cyc_Absynpp_typ2doc(_tmpE4);
_tmp11E[0]=Cyc_PP_text(_tag_arr("offsetof(",sizeof(char),10));Cyc_PP_cat(
_tag_arr(_tmp11E,sizeof(struct Cyc_PP_Doc*),5));});goto _LL188;_LL1B1: if(*((int*)
_tmpC3)!= 19)goto _LL1B3;_tmpE7=((struct Cyc_Absyn_Gentyp_e_struct*)_tmpC3)->f1;
_tmpE8=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmpC3)->f2;_LL1B2: s=({struct
Cyc_PP_Doc*_tmp121[4];_tmp121[3]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp121[2]=Cyc_Absynpp_typ2doc(_tmpE8);_tmp121[1]=Cyc_Absynpp_tvars2doc(_tmpE7);
_tmp121[0]=Cyc_PP_text(_tag_arr("__gen(",sizeof(char),7));Cyc_PP_cat(_tag_arr(
_tmp121,sizeof(struct Cyc_PP_Doc*),4));});goto _LL188;_LL1B3: if(*((int*)_tmpC3)!= 
20)goto _LL1B5;_tmpE9=((struct Cyc_Absyn_Deref_e_struct*)_tmpC3)->f1;_LL1B4: s=({
struct Cyc_PP_Doc*_tmp122[2];_tmp122[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpE9);
_tmp122[0]=Cyc_PP_text(_tag_arr("*",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp122,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL188;_LL1B5: if(*((int*)_tmpC3)!= 21)goto
_LL1B7;_tmpEA=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpC3)->f1;_tmpEB=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmpC3)->f2;_LL1B6: s=({struct Cyc_PP_Doc*_tmp123[3];
_tmp123[2]=Cyc_PP_textptr(_tmpEB);_tmp123[1]=Cyc_PP_text(_tag_arr(".",sizeof(
char),2));_tmp123[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpEA);Cyc_PP_cat(_tag_arr(
_tmp123,sizeof(struct Cyc_PP_Doc*),3));});goto _LL188;_LL1B7: if(*((int*)_tmpC3)!= 
22)goto _LL1B9;_tmpEC=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpC3)->f1;_tmpED=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmpC3)->f2;_LL1B8: s=({struct Cyc_PP_Doc*
_tmp124[3];_tmp124[2]=Cyc_PP_textptr(_tmpED);_tmp124[1]=Cyc_PP_text(_tag_arr("->",
sizeof(char),3));_tmp124[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmpEC);Cyc_PP_cat(
_tag_arr(_tmp124,sizeof(struct Cyc_PP_Doc*),3));});goto _LL188;_LL1B9: if(*((int*)
_tmpC3)!= 23)goto _LL1BB;_tmpEE=((struct Cyc_Absyn_Subscript_e_struct*)_tmpC3)->f1;
_tmpEF=((struct Cyc_Absyn_Subscript_e_struct*)_tmpC3)->f2;_LL1BA: s=({struct Cyc_PP_Doc*
_tmp125[4];_tmp125[3]=Cyc_PP_text(_tag_arr("]",sizeof(char),2));_tmp125[2]=Cyc_Absynpp_exp2doc(
_tmpEF);_tmp125[1]=Cyc_PP_text(_tag_arr("[",sizeof(char),2));_tmp125[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmpEE);Cyc_PP_cat(_tag_arr(_tmp125,sizeof(struct Cyc_PP_Doc*),4));});goto
_LL188;_LL1BB: if(*((int*)_tmpC3)!= 24)goto _LL1BD;_tmpF0=((struct Cyc_Absyn_Tuple_e_struct*)
_tmpC3)->f1;_LL1BC: s=({struct Cyc_PP_Doc*_tmp126[3];_tmp126[2]=Cyc_PP_text(
_tag_arr(")",sizeof(char),2));_tmp126[1]=Cyc_Absynpp_exps2doc_prec(20,_tmpF0);
_tmp126[0]=Cyc_PP_text(_tag_arr("$(",sizeof(char),3));Cyc_PP_cat(_tag_arr(
_tmp126,sizeof(struct Cyc_PP_Doc*),3));});goto _LL188;_LL1BD: if(*((int*)_tmpC3)!= 
25)goto _LL1BF;_tmpF1=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpC3)->f1;_tmpF2=((
struct Cyc_Absyn_CompoundLit_e_struct*)_tmpC3)->f2;_LL1BE: s=({struct Cyc_PP_Doc*
_tmp127[4];_tmp127[3]=Cyc_PP_group(_tag_arr("{",sizeof(char),2),_tag_arr("}",
sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpF2));_tmp127[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp127[1]=Cyc_Absynpp_typ2doc((*
_tmpF1).f3);_tmp127[0]=Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(
_tag_arr(_tmp127,sizeof(struct Cyc_PP_Doc*),4));});goto _LL188;_LL1BF: if(*((int*)
_tmpC3)!= 26)goto _LL1C1;_tmpF3=((struct Cyc_Absyn_Array_e_struct*)_tmpC3)->f1;
_LL1C0: s=Cyc_PP_group(_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(char),2),
_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmpF3));
goto _LL188;_LL1C1: if(*((int*)_tmpC3)!= 27)goto _LL1C3;_tmpF4=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpC3)->f1;_tmpF5=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpC3)->f2;_tmpF6=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmpC3)->f3;_LL1C2: s=({struct Cyc_PP_Doc*
_tmp128[7];_tmp128[6]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp128[5]=Cyc_Absynpp_exp2doc(
_tmpF6);_tmp128[4]=Cyc_PP_text(_tag_arr(" : ",sizeof(char),4));_tmp128[3]=Cyc_Absynpp_exp2doc(
_tmpF5);_tmp128[2]=Cyc_PP_text(_tag_arr(" < ",sizeof(char),4));_tmp128[1]=Cyc_PP_text(*(*
_tmpF4->name).f2);_tmp128[0]=Cyc_PP_text(_tag_arr("{for ",sizeof(char),6));Cyc_PP_cat(
_tag_arr(_tmp128,sizeof(struct Cyc_PP_Doc*),7));});goto _LL188;_LL1C3: if(*((int*)
_tmpC3)!= 28)goto _LL1C5;_tmpF7=((struct Cyc_Absyn_Struct_e_struct*)_tmpC3)->f1;
_tmpF8=((struct Cyc_Absyn_Struct_e_struct*)_tmpC3)->f2;_tmpF9=((struct Cyc_Absyn_Struct_e_struct*)
_tmpC3)->f3;_LL1C4: {struct Cyc_List_List*_tmp129=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpF9);s=({struct Cyc_PP_Doc*_tmp12A[2];_tmp12A[1]=Cyc_PP_group(_tag_arr("{",
sizeof(char),2),_tag_arr("}",sizeof(char),2),_tag_arr(",",sizeof(char),2),_tmpF8
!= 0?({struct Cyc_List_List*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->hd=Cyc_Absynpp_tps2doc(
_tmpF8);_tmp12B->tl=_tmp129;_tmp12B;}): _tmp129);_tmp12A[0]=Cyc_Absynpp_qvar2doc(
_tmpF7);Cyc_PP_cat(_tag_arr(_tmp12A,sizeof(struct Cyc_PP_Doc*),2));});goto _LL188;}
_LL1C5: if(*((int*)_tmpC3)!= 29)goto _LL1C7;_tmpFA=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpC3)->f2;_LL1C6: s=Cyc_PP_group(_tag_arr("{",sizeof(char),2),_tag_arr("}",
sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmpFA));goto _LL188;_LL1C7: if(*((int*)_tmpC3)!= 30)goto _LL1C9;_tmpFB=((struct Cyc_Absyn_Tunion_e_struct*)
_tmpC3)->f1;_tmpFC=((struct Cyc_Absyn_Tunion_e_struct*)_tmpC3)->f3;_LL1C8: if(
_tmpFB == 0)s=Cyc_Absynpp_qvar2doc(_tmpFC->name);else{s=({struct Cyc_PP_Doc*
_tmp12C[2];_tmp12C[1]=Cyc_PP_egroup(_tag_arr("(",sizeof(char),2),_tag_arr(")",
sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,
_tmpFB));_tmp12C[0]=Cyc_Absynpp_qvar2doc(_tmpFC->name);Cyc_PP_cat(_tag_arr(
_tmp12C,sizeof(struct Cyc_PP_Doc*),2));});}goto _LL188;_LL1C9: if(*((int*)_tmpC3)!= 
31)goto _LL1CB;_tmpFD=((struct Cyc_Absyn_Enum_e_struct*)_tmpC3)->f1;_LL1CA: s=Cyc_Absynpp_qvar2doc(
_tmpFD);goto _LL188;_LL1CB: if(*((int*)_tmpC3)!= 32)goto _LL1CD;_tmpFE=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmpC3)->f1;_LL1CC: s=Cyc_Absynpp_qvar2doc(_tmpFE);goto _LL188;_LL1CD: if(*((int*)
_tmpC3)!= 33)goto _LL1CF;_tmpFF=((struct Cyc_Absyn_Malloc_e_struct*)_tmpC3)->f1;
_tmp100=_tmpFF.is_calloc;_tmp101=_tmpFF.rgn;_tmp102=_tmpFF.elt_type;_tmp103=
_tmpFF.num_elts;_LL1CE: if(_tmp100){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmp102)),0);if(_tmp101 == 0)s=({struct Cyc_PP_Doc*_tmp12D[5];
_tmp12D[4]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp12D[3]=Cyc_Absynpp_exp2doc(
st);_tmp12D[2]=Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp12D[1]=Cyc_Absynpp_exp2doc(
_tmp103);_tmp12D[0]=Cyc_PP_text(_tag_arr("calloc(",sizeof(char),8));Cyc_PP_cat(
_tag_arr(_tmp12D,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp12E[7];_tmp12E[6]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp12E[5]=Cyc_Absynpp_exp2doc(
st);_tmp12E[4]=Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp12E[3]=Cyc_Absynpp_exp2doc(
_tmp103);_tmp12E[2]=Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp12E[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(_tmp101));_tmp12E[0]=Cyc_PP_text(_tag_arr("rcalloc(",
sizeof(char),9));Cyc_PP_cat(_tag_arr(_tmp12E,sizeof(struct Cyc_PP_Doc*),7));});}}
else{struct Cyc_Absyn_Exp*new_e;if(_tmp102 == 0)new_e=_tmp103;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp102,0),_tmp103,0);}if(_tmp101 == 0)s=({struct Cyc_PP_Doc*
_tmp12F[3];_tmp12F[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp12F[1]=Cyc_Absynpp_exp2doc(
new_e);_tmp12F[0]=Cyc_PP_text(_tag_arr("malloc(",sizeof(char),8));Cyc_PP_cat(
_tag_arr(_tmp12F,sizeof(struct Cyc_PP_Doc*),3));});else{s=({struct Cyc_PP_Doc*
_tmp130[5];_tmp130[4]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));_tmp130[3]=Cyc_Absynpp_exp2doc(
new_e);_tmp130[2]=Cyc_PP_text(_tag_arr(",",sizeof(char),2));_tmp130[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(_tmp101));_tmp130[0]=Cyc_PP_text(_tag_arr("rmalloc(",
sizeof(char),9));Cyc_PP_cat(_tag_arr(_tmp130,sizeof(struct Cyc_PP_Doc*),5));});}}
goto _LL188;_LL1CF: if(*((int*)_tmpC3)!= 34)goto _LL1D1;_tmp104=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpC3)->f1;_tmp105=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpC3)->f2;_LL1D0:
s=Cyc_PP_group(_tag_arr("{",sizeof(char),2),_tag_arr("}",sizeof(char),2),
_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(
struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp105));
goto _LL188;_LL1D1: if(*((int*)_tmpC3)!= 35)goto _LL1D3;_tmp106=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpC3)->f1;_LL1D2: s=({struct Cyc_PP_Doc*_tmp131[3];_tmp131[2]=Cyc_PP_text(
_tag_arr(" })",sizeof(char),4));_tmp131[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp106));_tmp131[0]=Cyc_PP_text(_tag_arr("({ ",sizeof(char),4));Cyc_PP_cat(
_tag_arr(_tmp131,sizeof(struct Cyc_PP_Doc*),3));});goto _LL188;_LL1D3: if(*((int*)
_tmpC3)!= 36)goto _LL1D5;_tmp107=((struct Cyc_Absyn_Codegen_e_struct*)_tmpC3)->f1;
_LL1D4: s=({struct Cyc_PP_Doc*_tmp132[3];_tmp132[2]=Cyc_PP_text(_tag_arr(")",
sizeof(char),2));_tmp132[1]=Cyc_PP_nest(2,Cyc_Absynpp_decl2doc(({struct Cyc_Absyn_Decl*
_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->r=(void*)((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp135;_tmp135.tag=1;_tmp135.f1=_tmp107;_tmp135;});_tmp134;}));_tmp133->loc=e->loc;
_tmp133;})));_tmp132[0]=Cyc_PP_text(_tag_arr("codegen(",sizeof(char),9));Cyc_PP_cat(
_tag_arr(_tmp132,sizeof(struct Cyc_PP_Doc*),3));});goto _LL188;_LL1D5: if(*((int*)
_tmpC3)!= 37)goto _LL188;_tmp108=((struct Cyc_Absyn_Fill_e_struct*)_tmpC3)->f1;
_LL1D6: s=({struct Cyc_PP_Doc*_tmp136[3];_tmp136[2]=Cyc_PP_text(_tag_arr(")",
sizeof(char),2));_tmp136[1]=Cyc_PP_nest(2,Cyc_Absynpp_exp2doc(_tmp108));_tmp136[
0]=Cyc_PP_text(_tag_arr("fill(",sizeof(char),6));Cyc_PP_cat(_tag_arr(_tmp136,
sizeof(struct Cyc_PP_Doc*),3));});goto _LL188;_LL188:;}if(inprec >= myprec)s=({
struct Cyc_PP_Doc*_tmp137[3];_tmp137[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp137[1]=s;_tmp137[0]=Cyc_PP_text(_tag_arr("(",sizeof(char),2));Cyc_PP_cat(
_tag_arr(_tmp137,sizeof(struct Cyc_PP_Doc*),3));});return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(
void*d){void*_tmp138=d;struct Cyc_Absyn_Exp*_tmp139;struct _tagged_arr*_tmp13A;
_LL1E4: if(*((int*)_tmp138)!= 0)goto _LL1E6;_tmp139=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp138)->f1;_LL1E5: return({struct Cyc_PP_Doc*_tmp13B[3];_tmp13B[2]=Cyc_PP_text(
_tag_arr("]",sizeof(char),2));_tmp13B[1]=Cyc_Absynpp_exp2doc(_tmp139);_tmp13B[0]=
Cyc_PP_text(_tag_arr(".[",sizeof(char),3));Cyc_PP_cat(_tag_arr(_tmp13B,sizeof(
struct Cyc_PP_Doc*),3));});_LL1E6: if(*((int*)_tmp138)!= 1)goto _LL1E3;_tmp13A=((
struct Cyc_Absyn_FieldName_struct*)_tmp138)->f1;_LL1E7: return({struct Cyc_PP_Doc*
_tmp13C[2];_tmp13C[1]=Cyc_PP_textptr(_tmp13A);_tmp13C[0]=Cyc_PP_text(_tag_arr(".",
sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp13C,sizeof(struct Cyc_PP_Doc*),2));});
_LL1E3:;}struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple9*de){if((*de).f1 == 0)
return Cyc_Absynpp_exp2doc((*de).f2);else{return({struct Cyc_PP_Doc*_tmp13D[2];
_tmp13D[1]=Cyc_Absynpp_exp2doc((*de).f2);_tmp13D[0]=Cyc_PP_egroup(_tag_arr("",
sizeof(char),1),_tag_arr("=",sizeof(char),2),_tag_arr("=",sizeof(char),2),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_designator2doc,(*de).f1));Cyc_PP_cat(_tag_arr(_tmp13D,sizeof(struct
Cyc_PP_Doc*),2));});}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,
struct Cyc_List_List*es){return Cyc_PP_group(_tag_arr("",sizeof(char),1),_tag_arr("",
sizeof(char),1),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(void*c){void*_tmp13E=c;void*
_tmp13F;char _tmp140;void*_tmp141;short _tmp142;void*_tmp143;int _tmp144;void*
_tmp145;int _tmp146;void*_tmp147;int _tmp148;void*_tmp149;long long _tmp14A;struct
_tagged_arr _tmp14B;struct _tagged_arr _tmp14C;_LL1E9: if(_tmp13E <= (void*)1?1:*((
int*)_tmp13E)!= 0)goto _LL1EB;_tmp13F=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmp13E)->f1;_tmp140=((struct Cyc_Absyn_Char_c_struct*)_tmp13E)->f2;_LL1EA: return
Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp14E;_tmp14E.tag=
0;_tmp14E.f1=(struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp140);{void*_tmp14D[1]={&
_tmp14E};Cyc_Std_aprintf(_tag_arr("'%s'",sizeof(char),5),_tag_arr(_tmp14D,
sizeof(void*),1));}}));_LL1EB: if(_tmp13E <= (void*)1?1:*((int*)_tmp13E)!= 1)goto
_LL1ED;_tmp141=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp13E)->f1;_tmp142=((
struct Cyc_Absyn_Short_c_struct*)_tmp13E)->f2;_LL1EC: return Cyc_PP_text((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp150;_tmp150.tag=1;_tmp150.f1=(int)((
unsigned int)((int)_tmp142));{void*_tmp14F[1]={& _tmp150};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(char),3),_tag_arr(_tmp14F,sizeof(void*),1));}}));_LL1ED: if(
_tmp13E <= (void*)1?1:*((int*)_tmp13E)!= 2)goto _LL1EF;_tmp143=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp13E)->f1;if((int)_tmp143 != 2)goto _LL1EF;_tmp144=((struct Cyc_Absyn_Int_c_struct*)
_tmp13E)->f2;_LL1EE: _tmp146=_tmp144;goto _LL1F0;_LL1EF: if(_tmp13E <= (void*)1?1:*((
int*)_tmp13E)!= 2)goto _LL1F1;_tmp145=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp13E)->f1;if((int)_tmp145 != 0)goto _LL1F1;_tmp146=((struct Cyc_Absyn_Int_c_struct*)
_tmp13E)->f2;_LL1F0: return Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp152;_tmp152.tag=1;_tmp152.f1=(int)((unsigned int)_tmp146);{void*_tmp151[1]={&
_tmp152};Cyc_Std_aprintf(_tag_arr("%d",sizeof(char),3),_tag_arr(_tmp151,sizeof(
void*),1));}}));_LL1F1: if(_tmp13E <= (void*)1?1:*((int*)_tmp13E)!= 2)goto _LL1F3;
_tmp147=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp13E)->f1;if((int)_tmp147 != 1)
goto _LL1F3;_tmp148=((struct Cyc_Absyn_Int_c_struct*)_tmp13E)->f2;_LL1F2: return Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp154;_tmp154.tag=1;_tmp154.f1=(
unsigned int)_tmp148;{void*_tmp153[1]={& _tmp154};Cyc_Std_aprintf(_tag_arr("%u",
sizeof(char),3),_tag_arr(_tmp153,sizeof(void*),1));}}));_LL1F3: if(_tmp13E <= (
void*)1?1:*((int*)_tmp13E)!= 3)goto _LL1F5;_tmp149=(void*)((struct Cyc_Absyn_LongLong_c_struct*)
_tmp13E)->f1;_tmp14A=((struct Cyc_Absyn_LongLong_c_struct*)_tmp13E)->f2;_LL1F4:
return Cyc_PP_text(_tag_arr("<<FIX LONG LONG CONSTANT>>",sizeof(char),27));_LL1F5:
if(_tmp13E <= (void*)1?1:*((int*)_tmp13E)!= 4)goto _LL1F7;_tmp14B=((struct Cyc_Absyn_Float_c_struct*)
_tmp13E)->f1;_LL1F6: return Cyc_PP_text(_tmp14B);_LL1F7: if((int)_tmp13E != 0)goto
_LL1F9;_LL1F8: return Cyc_PP_text(_tag_arr("NULL",sizeof(char),5));_LL1F9: if(
_tmp13E <= (void*)1?1:*((int*)_tmp13E)!= 5)goto _LL1E8;_tmp14C=((struct Cyc_Absyn_String_c_struct*)
_tmp13E)->f1;_LL1FA: return({struct Cyc_PP_Doc*_tmp155[3];_tmp155[2]=Cyc_PP_text(
_tag_arr("\"",sizeof(char),2));_tmp155[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(
_tmp14C));_tmp155[0]=Cyc_PP_text(_tag_arr("\"",sizeof(char),2));Cyc_PP_cat(
_tag_arr(_tmp155,sizeof(struct Cyc_PP_Doc*),3));});_LL1E8:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(
int inprec,void*p,struct Cyc_List_List*es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(
p);if(p == (void*)19){if(es == 0?1: es->tl != 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp156=_cycalloc(sizeof(*_tmp156));_tmp156[0]=({struct Cyc_Core_Failure_struct
_tmp157;_tmp157.tag=Cyc_Core_Failure;_tmp157.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp159;_tmp159.tag=0;_tmp159.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp158[1]={& _tmp159};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc Size: %s with bad args",
sizeof(char),44),_tag_arr(_tmp158,sizeof(void*),1));}});_tmp157;});_tmp156;}));{
struct Cyc_PP_Doc*_tmp15A=Cyc_Absynpp_exp2doc_prec(inprec,(struct Cyc_Absyn_Exp*)
es->hd);return({struct Cyc_PP_Doc*_tmp15B[2];_tmp15B[1]=Cyc_PP_text(_tag_arr(".size",
sizeof(char),6));_tmp15B[0]=_tmp15A;Cyc_PP_cat(_tag_arr(_tmp15B,sizeof(struct Cyc_PP_Doc*),
2));});}}else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,
inprec,es);if(ds == 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp15C=
_cycalloc(sizeof(*_tmp15C));_tmp15C[0]=({struct Cyc_Core_Failure_struct _tmp15D;
_tmp15D.tag=Cyc_Core_Failure;_tmp15D.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp15F;_tmp15F.tag=0;_tmp15F.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp15E[1]={& _tmp15F};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc: %s with no args",
sizeof(char),38),_tag_arr(_tmp15E,sizeof(void*),1));}});_tmp15D;});_tmp15C;}));
else{if(ds->tl == 0)return({struct Cyc_PP_Doc*_tmp160[3];_tmp160[2]=(struct Cyc_PP_Doc*)
ds->hd;_tmp160[1]=Cyc_PP_text(_tag_arr(" ",sizeof(char),2));_tmp160[0]=ps;Cyc_PP_cat(
_tag_arr(_tmp160,sizeof(struct Cyc_PP_Doc*),3));});else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0)(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161[0]=({struct Cyc_Core_Failure_struct
_tmp162;_tmp162.tag=Cyc_Core_Failure;_tmp162.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp164;_tmp164.tag=0;_tmp164.f1=(struct _tagged_arr)Cyc_PP_string_of_doc(ps,72);{
void*_tmp163[1]={& _tmp164};Cyc_Std_aprintf(_tag_arr("Absynpp::primapp2doc: %s with more than 2 args",
sizeof(char),47),_tag_arr(_tmp163,sizeof(void*),1));}});_tmp162;});_tmp161;}));
else{return({struct Cyc_PP_Doc*_tmp165[5];_tmp165[4]=(struct Cyc_PP_Doc*)((struct
Cyc_List_List*)_check_null(ds->tl))->hd;_tmp165[3]=Cyc_PP_text(_tag_arr(" ",
sizeof(char),2));_tmp165[2]=ps;_tmp165[1]=Cyc_PP_text(_tag_arr(" ",sizeof(char),
2));_tmp165[0]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_arr(_tmp165,sizeof(
struct Cyc_PP_Doc*),5));});}}}}}struct _tagged_arr Cyc_Absynpp_prim2str(void*p){
void*_tmp166=p;_LL1FC: if((int)_tmp166 != 0)goto _LL1FE;_LL1FD: return _tag_arr("+",
sizeof(char),2);_LL1FE: if((int)_tmp166 != 1)goto _LL200;_LL1FF: return _tag_arr("*",
sizeof(char),2);_LL200: if((int)_tmp166 != 2)goto _LL202;_LL201: return _tag_arr("-",
sizeof(char),2);_LL202: if((int)_tmp166 != 3)goto _LL204;_LL203: return _tag_arr("/",
sizeof(char),2);_LL204: if((int)_tmp166 != 4)goto _LL206;_LL205: return _tag_arr("%",
sizeof(char),2);_LL206: if((int)_tmp166 != 5)goto _LL208;_LL207: return _tag_arr("==",
sizeof(char),3);_LL208: if((int)_tmp166 != 6)goto _LL20A;_LL209: return _tag_arr("!=",
sizeof(char),3);_LL20A: if((int)_tmp166 != 7)goto _LL20C;_LL20B: return _tag_arr(">",
sizeof(char),2);_LL20C: if((int)_tmp166 != 8)goto _LL20E;_LL20D: return _tag_arr("<",
sizeof(char),2);_LL20E: if((int)_tmp166 != 9)goto _LL210;_LL20F: return _tag_arr(">=",
sizeof(char),3);_LL210: if((int)_tmp166 != 10)goto _LL212;_LL211: return _tag_arr("<=",
sizeof(char),3);_LL212: if((int)_tmp166 != 11)goto _LL214;_LL213: return _tag_arr("!",
sizeof(char),2);_LL214: if((int)_tmp166 != 12)goto _LL216;_LL215: return _tag_arr("~",
sizeof(char),2);_LL216: if((int)_tmp166 != 13)goto _LL218;_LL217: return _tag_arr("&",
sizeof(char),2);_LL218: if((int)_tmp166 != 14)goto _LL21A;_LL219: return _tag_arr("|",
sizeof(char),2);_LL21A: if((int)_tmp166 != 15)goto _LL21C;_LL21B: return _tag_arr("^",
sizeof(char),2);_LL21C: if((int)_tmp166 != 16)goto _LL21E;_LL21D: return _tag_arr("<<",
sizeof(char),3);_LL21E: if((int)_tmp166 != 17)goto _LL220;_LL21F: return _tag_arr(">>",
sizeof(char),3);_LL220: if((int)_tmp166 != 18)goto _LL222;_LL221: return _tag_arr(">>>",
sizeof(char),4);_LL222: if((int)_tmp166 != 19)goto _LL1FB;_LL223: return _tag_arr("size",
sizeof(char),5);_LL1FB:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p){return Cyc_PP_text(
Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp167=(void*)s->r;_LL225: if(_tmp167 <= (void*)1?1:*((int*)_tmp167)!= 12)
goto _LL227;_LL226: return 1;_LL227:;_LL228: return 0;_LL224:;}struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st){struct Cyc_PP_Doc*s;{void*_tmp168=(void*)st->r;struct Cyc_Absyn_Exp*
_tmp169;struct Cyc_Absyn_Stmt*_tmp16A;struct Cyc_Absyn_Stmt*_tmp16B;struct Cyc_Absyn_Exp*
_tmp16C;struct Cyc_Absyn_Exp*_tmp16D;struct Cyc_Absyn_Stmt*_tmp16E;struct Cyc_Absyn_Stmt*
_tmp16F;struct _tuple2 _tmp170;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_Absyn_Stmt*
_tmp172;struct _tagged_arr*_tmp173;struct Cyc_Absyn_Exp*_tmp174;struct _tuple2
_tmp175;struct Cyc_Absyn_Exp*_tmp176;struct _tuple2 _tmp177;struct Cyc_Absyn_Exp*
_tmp178;struct Cyc_Absyn_Stmt*_tmp179;struct Cyc_Absyn_ForArrayInfo _tmp17A;struct
Cyc_List_List*_tmp17B;struct _tuple2 _tmp17C;struct Cyc_Absyn_Exp*_tmp17D;struct
_tuple2 _tmp17E;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_Absyn_Stmt*_tmp180;struct
Cyc_Absyn_Exp*_tmp181;struct Cyc_List_List*_tmp182;struct Cyc_Absyn_Exp*_tmp183;
struct Cyc_List_List*_tmp184;struct Cyc_List_List*_tmp185;struct Cyc_List_List*
_tmp186;struct Cyc_Absyn_Decl*_tmp187;struct Cyc_Absyn_Stmt*_tmp188;struct Cyc_Absyn_Stmt*
_tmp189;struct Cyc_Absyn_Stmt*_tmp18A;struct _tagged_arr*_tmp18B;struct Cyc_Absyn_Stmt*
_tmp18C;struct Cyc_Absyn_Stmt*_tmp18D;struct _tuple2 _tmp18E;struct Cyc_Absyn_Exp*
_tmp18F;struct Cyc_Absyn_Stmt*_tmp190;struct Cyc_List_List*_tmp191;struct Cyc_Absyn_Tvar*
_tmp192;struct Cyc_Absyn_Vardecl*_tmp193;int _tmp194;struct Cyc_Absyn_Stmt*_tmp195;
struct Cyc_Absyn_Exp*_tmp196;_LL22A: if((int)_tmp168 != 0)goto _LL22C;_LL22B: s=Cyc_PP_text(
_tag_arr(";",sizeof(char),2));goto _LL229;_LL22C: if(_tmp168 <= (void*)1?1:*((int*)
_tmp168)!= 0)goto _LL22E;_tmp169=((struct Cyc_Absyn_Exp_s_struct*)_tmp168)->f1;
_LL22D: s=({struct Cyc_PP_Doc*_tmp197[2];_tmp197[1]=Cyc_PP_text(_tag_arr(";",
sizeof(char),2));_tmp197[0]=Cyc_Absynpp_exp2doc(_tmp169);Cyc_PP_cat(_tag_arr(
_tmp197,sizeof(struct Cyc_PP_Doc*),2));});goto _LL229;_LL22E: if(_tmp168 <= (void*)1?
1:*((int*)_tmp168)!= 1)goto _LL230;_tmp16A=((struct Cyc_Absyn_Seq_s_struct*)
_tmp168)->f1;_tmp16B=((struct Cyc_Absyn_Seq_s_struct*)_tmp168)->f2;_LL22F: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp16A))s=({struct Cyc_PP_Doc*_tmp198[6];_tmp198[5]=
Cyc_Absynpp_is_declaration(_tmp16B)?({struct Cyc_PP_Doc*_tmp199[4];_tmp199[3]=Cyc_PP_line_doc();
_tmp199[2]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp199[1]=Cyc_PP_nest(2,
Cyc_Absynpp_stmt2doc(_tmp16B));_tmp199[0]=Cyc_PP_text(_tag_arr("{ ",sizeof(char),
3));Cyc_PP_cat(_tag_arr(_tmp199,sizeof(struct Cyc_PP_Doc*),4));}): Cyc_Absynpp_stmt2doc(
_tmp16B);_tmp198[4]=Cyc_PP_line_doc();_tmp198[3]=Cyc_PP_text(_tag_arr("}",
sizeof(char),2));_tmp198[2]=Cyc_PP_line_doc();_tmp198[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp16A));_tmp198[0]=Cyc_PP_text(_tag_arr("{ ",sizeof(char),3));Cyc_PP_cat(
_tag_arr(_tmp198,sizeof(struct Cyc_PP_Doc*),6));});else{if(Cyc_Absynpp_is_declaration(
_tmp16B))s=({struct Cyc_PP_Doc*_tmp19A[6];_tmp19A[5]=Cyc_PP_line_doc();_tmp19A[4]=
Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp19A[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp16B));_tmp19A[2]=Cyc_PP_text(_tag_arr("{ ",sizeof(char),3));_tmp19A[1]=Cyc_PP_line_doc();
_tmp19A[0]=Cyc_Absynpp_stmt2doc(_tmp16A);Cyc_PP_cat(_tag_arr(_tmp19A,sizeof(
struct Cyc_PP_Doc*),6));});else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Stmt*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,_tag_arr("",sizeof(char),1),({struct Cyc_List_List*_tmp19B=
_cycalloc(sizeof(*_tmp19B));_tmp19B->hd=_tmp16A;_tmp19B->tl=({struct Cyc_List_List*
_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->hd=_tmp16B;_tmp19C->tl=0;_tmp19C;});
_tmp19B;}));}}}else{s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,
_tag_arr("",sizeof(char),1),({struct Cyc_List_List*_tmp19D=_cycalloc(sizeof(*
_tmp19D));_tmp19D->hd=_tmp16A;_tmp19D->tl=({struct Cyc_List_List*_tmp19E=
_cycalloc(sizeof(*_tmp19E));_tmp19E->hd=_tmp16B;_tmp19E->tl=0;_tmp19E;});_tmp19D;}));}
goto _LL229;_LL230: if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 2)goto _LL232;
_tmp16C=((struct Cyc_Absyn_Return_s_struct*)_tmp168)->f1;_LL231: if(_tmp16C == 0)s=
Cyc_PP_text(_tag_arr("return;",sizeof(char),8));else{s=({struct Cyc_PP_Doc*
_tmp19F[3];_tmp19F[2]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmp19F[1]=
_tmp16C == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(_tmp16C));_tmp19F[0]=Cyc_PP_text(_tag_arr("return ",sizeof(char),8));
Cyc_PP_cat(_tag_arr(_tmp19F,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL229;_LL232:
if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 3)goto _LL234;_tmp16D=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp168)->f1;_tmp16E=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp168)->f2;_tmp16F=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp168)->f3;_LL233: {int print_else;{void*
_tmp1A0=(void*)_tmp16F->r;_LL259: if((int)_tmp1A0 != 0)goto _LL25B;_LL25A:
print_else=0;goto _LL258;_LL25B:;_LL25C: print_else=1;goto _LL258;_LL258:;}s=({
struct Cyc_PP_Doc*_tmp1A1[7];_tmp1A1[6]=print_else?({struct Cyc_PP_Doc*_tmp1A3[5];
_tmp1A3[4]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp1A3[3]=Cyc_PP_line_doc();
_tmp1A3[2]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1A4[2];_tmp1A4[1]=Cyc_Absynpp_stmt2doc(
_tmp16F);_tmp1A4[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1A4,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp1A3[1]=Cyc_PP_text(_tag_arr("else {",sizeof(char),7));
_tmp1A3[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1A3,sizeof(struct Cyc_PP_Doc*),
5));}): Cyc_PP_nil_doc();_tmp1A1[5]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp1A1[4]=Cyc_PP_line_doc();_tmp1A1[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1A2[
2];_tmp1A2[1]=Cyc_Absynpp_stmt2doc(_tmp16E);_tmp1A2[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp1A2,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1A1[2]=Cyc_PP_text(
_tag_arr(") {",sizeof(char),4));_tmp1A1[1]=Cyc_Absynpp_exp2doc(_tmp16D);_tmp1A1[
0]=Cyc_PP_text(_tag_arr("if (",sizeof(char),5));Cyc_PP_cat(_tag_arr(_tmp1A1,
sizeof(struct Cyc_PP_Doc*),7));});goto _LL229;}_LL234: if(_tmp168 <= (void*)1?1:*((
int*)_tmp168)!= 4)goto _LL236;_tmp170=((struct Cyc_Absyn_While_s_struct*)_tmp168)->f1;
_tmp171=_tmp170.f1;_tmp172=((struct Cyc_Absyn_While_s_struct*)_tmp168)->f2;_LL235:
s=({struct Cyc_PP_Doc*_tmp1A5[6];_tmp1A5[5]=Cyc_PP_text(_tag_arr("}",sizeof(char),
2));_tmp1A5[4]=Cyc_PP_line_doc();_tmp1A5[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp1A6[2];_tmp1A6[1]=Cyc_Absynpp_stmt2doc(_tmp172);_tmp1A6[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp1A6,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1A5[2]=Cyc_PP_text(
_tag_arr(") {",sizeof(char),4));_tmp1A5[1]=Cyc_Absynpp_exp2doc(_tmp171);_tmp1A5[
0]=Cyc_PP_text(_tag_arr("while (",sizeof(char),8));Cyc_PP_cat(_tag_arr(_tmp1A5,
sizeof(struct Cyc_PP_Doc*),6));});goto _LL229;_LL236: if(_tmp168 <= (void*)1?1:*((
int*)_tmp168)!= 5)goto _LL238;_LL237: s=Cyc_PP_text(_tag_arr("break;",sizeof(char),
7));goto _LL229;_LL238: if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 6)goto _LL23A;
_LL239: s=Cyc_PP_text(_tag_arr("continue;",sizeof(char),10));goto _LL229;_LL23A:
if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 7)goto _LL23C;_tmp173=((struct Cyc_Absyn_Goto_s_struct*)
_tmp168)->f1;_LL23B: s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp1A8;_tmp1A8.tag=0;_tmp1A8.f1=(struct _tagged_arr)*_tmp173;{void*_tmp1A7[1]={&
_tmp1A8};Cyc_Std_aprintf(_tag_arr("goto %s;",sizeof(char),9),_tag_arr(_tmp1A7,
sizeof(void*),1));}}));goto _LL229;_LL23C: if(_tmp168 <= (void*)1?1:*((int*)_tmp168)
!= 8)goto _LL23E;_tmp174=((struct Cyc_Absyn_For_s_struct*)_tmp168)->f1;_tmp175=((
struct Cyc_Absyn_For_s_struct*)_tmp168)->f2;_tmp176=_tmp175.f1;_tmp177=((struct
Cyc_Absyn_For_s_struct*)_tmp168)->f3;_tmp178=_tmp177.f1;_tmp179=((struct Cyc_Absyn_For_s_struct*)
_tmp168)->f4;_LL23D: s=({struct Cyc_PP_Doc*_tmp1A9[10];_tmp1A9[9]=Cyc_PP_text(
_tag_arr("}",sizeof(char),2));_tmp1A9[8]=Cyc_PP_line_doc();_tmp1A9[7]=Cyc_PP_nest(
2,({struct Cyc_PP_Doc*_tmp1AA[2];_tmp1AA[1]=Cyc_Absynpp_stmt2doc(_tmp179);_tmp1AA[
0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1AA,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp1A9[6]=Cyc_PP_text(_tag_arr(") {",sizeof(char),4));_tmp1A9[5]=Cyc_Absynpp_exp2doc(
_tmp178);_tmp1A9[4]=Cyc_PP_text(_tag_arr("; ",sizeof(char),3));_tmp1A9[3]=Cyc_Absynpp_exp2doc(
_tmp176);_tmp1A9[2]=Cyc_PP_text(_tag_arr("; ",sizeof(char),3));_tmp1A9[1]=Cyc_Absynpp_exp2doc(
_tmp174);_tmp1A9[0]=Cyc_PP_text(_tag_arr("for(",sizeof(char),5));Cyc_PP_cat(
_tag_arr(_tmp1A9,sizeof(struct Cyc_PP_Doc*),10));});goto _LL229;_LL23E: if(_tmp168
<= (void*)1?1:*((int*)_tmp168)!= 19)goto _LL240;_tmp17A=((struct Cyc_Absyn_ForArray_s_struct*)
_tmp168)->f1;_tmp17B=_tmp17A.defns;_tmp17C=_tmp17A.condition;_tmp17D=_tmp17C.f1;
_tmp17E=_tmp17A.delta;_tmp17F=_tmp17E.f1;_tmp180=_tmp17A.body;_LL23F: s=({struct
Cyc_PP_Doc*_tmp1AB[9];_tmp1AB[8]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp1AB[7]=Cyc_PP_line_doc();_tmp1AB[6]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1AC[
2];_tmp1AC[1]=Cyc_Absynpp_stmt2doc(_tmp180);_tmp1AC[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp1AC,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1AB[5]=Cyc_PP_text(
_tag_arr(") {",sizeof(char),4));_tmp1AB[4]=Cyc_Absynpp_exp2doc(_tmp17F);_tmp1AB[
3]=Cyc_PP_text(_tag_arr("; ",sizeof(char),3));_tmp1AB[2]=Cyc_Absynpp_exp2doc(
_tmp17D);_tmp1AB[1]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_vardecl2doc,
_tag_arr(" ",sizeof(char),2),_tmp17B);_tmp1AB[0]=Cyc_PP_text(_tag_arr("forarray(",
sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp1AB,sizeof(struct Cyc_PP_Doc*),9));});
goto _LL229;_LL240: if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 9)goto _LL242;
_tmp181=((struct Cyc_Absyn_Switch_s_struct*)_tmp168)->f1;_tmp182=((struct Cyc_Absyn_Switch_s_struct*)
_tmp168)->f2;_LL241: s=({struct Cyc_PP_Doc*_tmp1AD[7];_tmp1AD[6]=Cyc_PP_text(
_tag_arr("}",sizeof(char),2));_tmp1AD[5]=Cyc_PP_line_doc();_tmp1AD[4]=Cyc_Absynpp_switchclauses2doc(
_tmp182);_tmp1AD[3]=Cyc_PP_line_doc();_tmp1AD[2]=Cyc_PP_text(_tag_arr(") {",
sizeof(char),4));_tmp1AD[1]=Cyc_Absynpp_exp2doc(_tmp181);_tmp1AD[0]=Cyc_PP_text(
_tag_arr("switch (",sizeof(char),9));Cyc_PP_cat(_tag_arr(_tmp1AD,sizeof(struct
Cyc_PP_Doc*),7));});goto _LL229;_LL242: if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 
10)goto _LL244;_tmp183=((struct Cyc_Absyn_SwitchC_s_struct*)_tmp168)->f1;_tmp184=((
struct Cyc_Absyn_SwitchC_s_struct*)_tmp168)->f2;_LL243: s=({struct Cyc_PP_Doc*
_tmp1AE[7];_tmp1AE[6]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp1AE[5]=Cyc_PP_line_doc();
_tmp1AE[4]=Cyc_Absynpp_switchCclauses2doc(_tmp184);_tmp1AE[3]=Cyc_PP_line_doc();
_tmp1AE[2]=Cyc_PP_text(_tag_arr(") {",sizeof(char),4));_tmp1AE[1]=Cyc_Absynpp_exp2doc(
_tmp183);_tmp1AE[0]=Cyc_PP_text(_tag_arr("switch \"C\" (",sizeof(char),13));Cyc_PP_cat(
_tag_arr(_tmp1AE,sizeof(struct Cyc_PP_Doc*),7));});goto _LL229;_LL244: if(_tmp168 <= (
void*)1?1:*((int*)_tmp168)!= 11)goto _LL246;_tmp185=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp168)->f1;if(_tmp185 != 0)goto _LL246;_LL245: s=Cyc_PP_text(_tag_arr("fallthru;",
sizeof(char),10));goto _LL229;_LL246: if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 
11)goto _LL248;_tmp186=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp168)->f1;_LL247: s=({
struct Cyc_PP_Doc*_tmp1AF[3];_tmp1AF[2]=Cyc_PP_text(_tag_arr(");",sizeof(char),3));
_tmp1AF[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp186);_tmp1AF[0]=Cyc_PP_text(_tag_arr("fallthru(",
sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp1AF,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL229;_LL248: if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 12)goto _LL24A;
_tmp187=((struct Cyc_Absyn_Decl_s_struct*)_tmp168)->f1;_tmp188=((struct Cyc_Absyn_Decl_s_struct*)
_tmp168)->f2;_LL249: s=({struct Cyc_PP_Doc*_tmp1B0[3];_tmp1B0[2]=Cyc_Absynpp_stmt2doc(
_tmp188);_tmp1B0[1]=Cyc_PP_line_doc();_tmp1B0[0]=Cyc_Absynpp_decl2doc(_tmp187);
Cyc_PP_cat(_tag_arr(_tmp1B0,sizeof(struct Cyc_PP_Doc*),3));});goto _LL229;_LL24A:
if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 13)goto _LL24C;_tmp189=((struct Cyc_Absyn_Cut_s_struct*)
_tmp168)->f1;_LL24B: s=({struct Cyc_PP_Doc*_tmp1B1[2];_tmp1B1[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp189));_tmp1B1[0]=Cyc_PP_text(_tag_arr("cut ",sizeof(char),5));Cyc_PP_cat(
_tag_arr(_tmp1B1,sizeof(struct Cyc_PP_Doc*),2));});goto _LL229;_LL24C: if(_tmp168 <= (
void*)1?1:*((int*)_tmp168)!= 14)goto _LL24E;_tmp18A=((struct Cyc_Absyn_Splice_s_struct*)
_tmp168)->f1;_LL24D: s=({struct Cyc_PP_Doc*_tmp1B2[2];_tmp1B2[1]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp18A));_tmp1B2[0]=Cyc_PP_text(_tag_arr("splice ",sizeof(char),8));Cyc_PP_cat(
_tag_arr(_tmp1B2,sizeof(struct Cyc_PP_Doc*),2));});goto _LL229;_LL24E: if(_tmp168 <= (
void*)1?1:*((int*)_tmp168)!= 15)goto _LL250;_tmp18B=((struct Cyc_Absyn_Label_s_struct*)
_tmp168)->f1;_tmp18C=((struct Cyc_Absyn_Label_s_struct*)_tmp168)->f2;_LL24F: if(
Cyc_Absynpp_decls_first?Cyc_Absynpp_is_declaration(_tmp18C): 0)s=({struct Cyc_PP_Doc*
_tmp1B3[6];_tmp1B3[5]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp1B3[4]=Cyc_PP_line_doc();
_tmp1B3[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp18C));_tmp1B3[2]=Cyc_PP_line_doc();
_tmp1B3[1]=Cyc_PP_text(_tag_arr(": {",sizeof(char),4));_tmp1B3[0]=Cyc_PP_textptr(
_tmp18B);Cyc_PP_cat(_tag_arr(_tmp1B3,sizeof(struct Cyc_PP_Doc*),6));});else{s=({
struct Cyc_PP_Doc*_tmp1B4[3];_tmp1B4[2]=Cyc_Absynpp_stmt2doc(_tmp18C);_tmp1B4[1]=
Cyc_PP_text(_tag_arr(": ",sizeof(char),3));_tmp1B4[0]=Cyc_PP_textptr(_tmp18B);
Cyc_PP_cat(_tag_arr(_tmp1B4,sizeof(struct Cyc_PP_Doc*),3));});}goto _LL229;_LL250:
if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 16)goto _LL252;_tmp18D=((struct Cyc_Absyn_Do_s_struct*)
_tmp168)->f1;_tmp18E=((struct Cyc_Absyn_Do_s_struct*)_tmp168)->f2;_tmp18F=_tmp18E.f1;
_LL251: s=({struct Cyc_PP_Doc*_tmp1B5[7];_tmp1B5[6]=Cyc_PP_text(_tag_arr(");",
sizeof(char),3));_tmp1B5[5]=Cyc_Absynpp_exp2doc(_tmp18F);_tmp1B5[4]=Cyc_PP_text(
_tag_arr("} while (",sizeof(char),10));_tmp1B5[3]=Cyc_PP_line_doc();_tmp1B5[2]=
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp18D));_tmp1B5[1]=Cyc_PP_line_doc();
_tmp1B5[0]=Cyc_PP_text(_tag_arr("do {",sizeof(char),5));Cyc_PP_cat(_tag_arr(
_tmp1B5,sizeof(struct Cyc_PP_Doc*),7));});goto _LL229;_LL252: if(_tmp168 <= (void*)1?
1:*((int*)_tmp168)!= 17)goto _LL254;_tmp190=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp168)->f1;_tmp191=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp168)->f2;_LL253: s=({
struct Cyc_PP_Doc*_tmp1B6[9];_tmp1B6[8]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp1B6[7]=Cyc_PP_line_doc();_tmp1B6[6]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp191));_tmp1B6[5]=Cyc_PP_line_doc();_tmp1B6[4]=Cyc_PP_text(_tag_arr("} catch {",
sizeof(char),10));_tmp1B6[3]=Cyc_PP_line_doc();_tmp1B6[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp190));_tmp1B6[1]=Cyc_PP_line_doc();_tmp1B6[0]=Cyc_PP_text(_tag_arr("try {",
sizeof(char),6));Cyc_PP_cat(_tag_arr(_tmp1B6,sizeof(struct Cyc_PP_Doc*),9));});
goto _LL229;_LL254: if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 18)goto _LL256;
_tmp192=((struct Cyc_Absyn_Region_s_struct*)_tmp168)->f1;_tmp193=((struct Cyc_Absyn_Region_s_struct*)
_tmp168)->f2;_tmp194=((struct Cyc_Absyn_Region_s_struct*)_tmp168)->f3;_tmp195=((
struct Cyc_Absyn_Region_s_struct*)_tmp168)->f4;_LL255: s=({struct Cyc_PP_Doc*
_tmp1B7[11];_tmp1B7[10]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp1B7[9]=Cyc_PP_line_doc();
_tmp1B7[8]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp195));_tmp1B7[7]=Cyc_PP_line_doc();
_tmp1B7[6]=Cyc_PP_text(_tag_arr("{",sizeof(char),2));_tmp1B7[5]=Cyc_Absynpp_qvar2doc(
_tmp193->name);_tmp1B7[4]=Cyc_PP_text(_tag_arr(">",sizeof(char),2));_tmp1B7[3]=
Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp192));_tmp1B7[2]=Cyc_PP_text(_tag_arr("<",
sizeof(char),2));_tmp1B7[1]=_tmp194?Cyc_PP_text(_tag_arr("",sizeof(char),1)): Cyc_PP_text(
_tag_arr("[resetable]",sizeof(char),12));_tmp1B7[0]=Cyc_PP_text(_tag_arr("region",
sizeof(char),7));Cyc_PP_cat(_tag_arr(_tmp1B7,sizeof(struct Cyc_PP_Doc*),11));});
goto _LL229;_LL256: if(_tmp168 <= (void*)1?1:*((int*)_tmp168)!= 20)goto _LL229;
_tmp196=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp168)->f1;_LL257: s=({struct
Cyc_PP_Doc*_tmp1B8[3];_tmp1B8[2]=Cyc_PP_text(_tag_arr(");",sizeof(char),3));
_tmp1B8[1]=Cyc_Absynpp_exp2doc(_tmp196);_tmp1B8[0]=Cyc_PP_text(_tag_arr("reset_region(",
sizeof(char),14));Cyc_PP_cat(_tag_arr(_tmp1B8,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL229;_LL229:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*
p){struct Cyc_PP_Doc*s;{void*_tmp1B9=(void*)p->r;void*_tmp1BA;int _tmp1BB;char
_tmp1BC;struct _tagged_arr _tmp1BD;struct Cyc_Absyn_Vardecl*_tmp1BE;struct Cyc_List_List*
_tmp1BF;struct Cyc_Absyn_Pat*_tmp1C0;struct Cyc_Absyn_Vardecl*_tmp1C1;struct
_tuple0*_tmp1C2;struct _tuple0*_tmp1C3;struct Cyc_List_List*_tmp1C4;struct Cyc_Absyn_AggrInfo
_tmp1C5;void*_tmp1C6;struct Cyc_List_List*_tmp1C7;struct Cyc_List_List*_tmp1C8;
struct Cyc_Absyn_Enumfield*_tmp1C9;struct Cyc_Absyn_Enumfield*_tmp1CA;struct Cyc_Absyn_Tunionfield*
_tmp1CB;struct Cyc_List_List*_tmp1CC;struct Cyc_Absyn_Tunionfield*_tmp1CD;struct
Cyc_List_List*_tmp1CE;_LL25E: if((int)_tmp1B9 != 0)goto _LL260;_LL25F: s=Cyc_PP_text(
_tag_arr("_",sizeof(char),2));goto _LL25D;_LL260: if((int)_tmp1B9 != 1)goto _LL262;
_LL261: s=Cyc_PP_text(_tag_arr("NULL",sizeof(char),5));goto _LL25D;_LL262: if(
_tmp1B9 <= (void*)2?1:*((int*)_tmp1B9)!= 1)goto _LL264;_tmp1BA=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp1B9)->f1;_tmp1BB=((struct Cyc_Absyn_Int_p_struct*)_tmp1B9)->f2;_LL263: if(
_tmp1BA != (void*)1)s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp1D0;_tmp1D0.tag=1;_tmp1D0.f1=(int)((unsigned int)_tmp1BB);{void*_tmp1CF[1]={&
_tmp1D0};Cyc_Std_aprintf(_tag_arr("%d",sizeof(char),3),_tag_arr(_tmp1CF,sizeof(
void*),1));}}));else{s=Cyc_PP_text((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp1D2;_tmp1D2.tag=1;_tmp1D2.f1=(unsigned int)_tmp1BB;{void*_tmp1D1[1]={&
_tmp1D2};Cyc_Std_aprintf(_tag_arr("%u",sizeof(char),3),_tag_arr(_tmp1D1,sizeof(
void*),1));}}));}goto _LL25D;_LL264: if(_tmp1B9 <= (void*)2?1:*((int*)_tmp1B9)!= 2)
goto _LL266;_tmp1BC=((struct Cyc_Absyn_Char_p_struct*)_tmp1B9)->f1;_LL265: s=Cyc_PP_text((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp1D4;_tmp1D4.tag=0;_tmp1D4.f1=(
struct _tagged_arr)Cyc_Absynpp_char_escape(_tmp1BC);{void*_tmp1D3[1]={& _tmp1D4};
Cyc_Std_aprintf(_tag_arr("'%s'",sizeof(char),5),_tag_arr(_tmp1D3,sizeof(void*),1));}}));
goto _LL25D;_LL266: if(_tmp1B9 <= (void*)2?1:*((int*)_tmp1B9)!= 3)goto _LL268;
_tmp1BD=((struct Cyc_Absyn_Float_p_struct*)_tmp1B9)->f1;_LL267: s=Cyc_PP_text(
_tmp1BD);goto _LL25D;_LL268: if(_tmp1B9 <= (void*)2?1:*((int*)_tmp1B9)!= 0)goto
_LL26A;_tmp1BE=((struct Cyc_Absyn_Var_p_struct*)_tmp1B9)->f1;_LL269: s=Cyc_Absynpp_qvar2doc(
_tmp1BE->name);goto _LL25D;_LL26A: if(_tmp1B9 <= (void*)2?1:*((int*)_tmp1B9)!= 4)
goto _LL26C;_tmp1BF=((struct Cyc_Absyn_Tuple_p_struct*)_tmp1B9)->f1;_LL26B: s=({
struct Cyc_PP_Doc*_tmp1D5[3];_tmp1D5[2]=Cyc_PP_text(_tag_arr(")",sizeof(char),2));
_tmp1D5[1]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,
_tag_arr(",",sizeof(char),2),_tmp1BF);_tmp1D5[0]=Cyc_PP_text(_tag_arr("$(",
sizeof(char),3));Cyc_PP_cat(_tag_arr(_tmp1D5,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL25D;_LL26C: if(_tmp1B9 <= (void*)2?1:*((int*)_tmp1B9)!= 5)goto _LL26E;
_tmp1C0=((struct Cyc_Absyn_Pointer_p_struct*)_tmp1B9)->f1;_LL26D: s=({struct Cyc_PP_Doc*
_tmp1D6[2];_tmp1D6[1]=Cyc_Absynpp_pat2doc(_tmp1C0);_tmp1D6[0]=Cyc_PP_text(
_tag_arr("&",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp1D6,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL25D;_LL26E: if(_tmp1B9 <= (void*)2?1:*((int*)_tmp1B9)!= 6)goto _LL270;
_tmp1C1=((struct Cyc_Absyn_Reference_p_struct*)_tmp1B9)->f1;_LL26F: s=({struct Cyc_PP_Doc*
_tmp1D7[2];_tmp1D7[1]=Cyc_Absynpp_qvar2doc(_tmp1C1->name);_tmp1D7[0]=Cyc_PP_text(
_tag_arr("*",sizeof(char),2));Cyc_PP_cat(_tag_arr(_tmp1D7,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL25D;_LL270: if(_tmp1B9 <= (void*)2?1:*((int*)_tmp1B9)!= 11)goto
_LL272;_tmp1C2=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp1B9)->f1;_LL271: s=Cyc_Absynpp_qvar2doc(
_tmp1C2);goto _LL25D;_LL272: if(_tmp1B9 <= (void*)2?1:*((int*)_tmp1B9)!= 12)goto
_LL274;_tmp1C3=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1B9)->f1;_tmp1C4=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1B9)->f2;_LL273: s=({struct Cyc_PP_Doc*
_tmp1D8[2];_tmp1D8[1]=Cyc_PP_group(_tag_arr("(",sizeof(char),2),_tag_arr(")",
sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,
_tmp1C4));_tmp1D8[0]=Cyc_Absynpp_qvar2doc(_tmp1C3);Cyc_PP_cat(_tag_arr(_tmp1D8,
sizeof(struct Cyc_PP_Doc*),2));});goto _LL25D;_LL274: if(_tmp1B9 <= (void*)2?1:*((
int*)_tmp1B9)!= 7)goto _LL276;_tmp1C5=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1B9)->f1;
_tmp1C6=(void*)_tmp1C5.aggr_info;_tmp1C7=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp1B9)->f2;_tmp1C8=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1B9)->f3;_LL275: {
struct _tuple0*_tmp1DA;struct _tuple3 _tmp1D9=Cyc_Absyn_aggr_kinded_name(_tmp1C6);
_tmp1DA=_tmp1D9.f2;s=({struct Cyc_PP_Doc*_tmp1DB[4];_tmp1DB[3]=Cyc_PP_group(
_tag_arr("",sizeof(char),1),_tag_arr("}",sizeof(char),2),_tag_arr(",",sizeof(
char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp1C8));_tmp1DB[2]=Cyc_PP_egroup(
_tag_arr("[",sizeof(char),2),_tag_arr("]",sizeof(char),2),_tag_arr(",",sizeof(
char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tagged_arr*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(
struct _tagged_arr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_get_name,_tmp1C7)));_tmp1DB[1]=Cyc_PP_text(_tag_arr("{",sizeof(char),
2));_tmp1DB[0]=Cyc_Absynpp_qvar2doc(_tmp1DA);Cyc_PP_cat(_tag_arr(_tmp1DB,sizeof(
struct Cyc_PP_Doc*),4));});goto _LL25D;}_LL276: if(_tmp1B9 <= (void*)2?1:*((int*)
_tmp1B9)!= 9)goto _LL278;_tmp1C9=((struct Cyc_Absyn_Enum_p_struct*)_tmp1B9)->f2;
_LL277: _tmp1CA=_tmp1C9;goto _LL279;_LL278: if(_tmp1B9 <= (void*)2?1:*((int*)_tmp1B9)
!= 10)goto _LL27A;_tmp1CA=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp1B9)->f2;
_LL279: s=Cyc_Absynpp_qvar2doc(_tmp1CA->name);goto _LL25D;_LL27A: if(_tmp1B9 <= (
void*)2?1:*((int*)_tmp1B9)!= 8)goto _LL27C;_tmp1CB=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1B9)->f2;_tmp1CC=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1B9)->f3;if(_tmp1CC
!= 0)goto _LL27C;_LL27B: s=Cyc_Absynpp_qvar2doc(_tmp1CB->name);goto _LL25D;_LL27C:
if(_tmp1B9 <= (void*)2?1:*((int*)_tmp1B9)!= 8)goto _LL25D;_tmp1CD=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1B9)->f2;_tmp1CE=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1B9)->f3;_LL27D: s=({
struct Cyc_PP_Doc*_tmp1DC[2];_tmp1DC[1]=Cyc_PP_egroup(_tag_arr("(",sizeof(char),2),
_tag_arr(")",sizeof(char),2),_tag_arr(",",sizeof(char),2),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_pat2doc,_tmp1CE));_tmp1DC[0]=Cyc_Absynpp_qvar2doc(_tmp1CD->name);Cyc_PP_cat(
_tag_arr(_tmp1DC,sizeof(struct Cyc_PP_Doc*),2));});goto _LL25D;_LL25D:;}return s;}
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp){return({struct Cyc_PP_Doc*
_tmp1DD[2];_tmp1DD[1]=Cyc_Absynpp_pat2doc((*dp).f2);_tmp1DD[0]=Cyc_PP_egroup(
_tag_arr("",sizeof(char),1),_tag_arr("=",sizeof(char),2),_tag_arr("=",sizeof(
char),2),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1));Cyc_PP_cat(_tag_arr(
_tmp1DD,sizeof(struct Cyc_PP_Doc*),2));});}struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(
struct Cyc_Absyn_Switch_clause*c){if(c->where_clause == 0?(void*)(c->pattern)->r == (
void*)0: 0)return({struct Cyc_PP_Doc*_tmp1DE[2];_tmp1DE[1]=Cyc_PP_nest(2,({struct
Cyc_PP_Doc*_tmp1DF[2];_tmp1DF[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1DF[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp1DF,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1DE[0]=Cyc_PP_text(
_tag_arr("default: ",sizeof(char),10));Cyc_PP_cat(_tag_arr(_tmp1DE,sizeof(struct
Cyc_PP_Doc*),2));});else{if(c->where_clause == 0)return({struct Cyc_PP_Doc*_tmp1E0[
4];_tmp1E0[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1E1[2];_tmp1E1[1]=Cyc_Absynpp_stmt2doc(
c->body);_tmp1E1[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1E1,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp1E0[2]=Cyc_PP_text(_tag_arr(": ",sizeof(char),3));
_tmp1E0[1]=Cyc_Absynpp_pat2doc(c->pattern);_tmp1E0[0]=Cyc_PP_text(_tag_arr("case ",
sizeof(char),6));Cyc_PP_cat(_tag_arr(_tmp1E0,sizeof(struct Cyc_PP_Doc*),4));});
else{return({struct Cyc_PP_Doc*_tmp1E2[6];_tmp1E2[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*
_tmp1E3[2];_tmp1E3[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1E3[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp1E3,sizeof(struct Cyc_PP_Doc*),2));}));_tmp1E2[4]=Cyc_PP_text(
_tag_arr(": ",sizeof(char),3));_tmp1E2[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(c->where_clause));_tmp1E2[2]=Cyc_PP_text(_tag_arr(" && ",sizeof(char),
5));_tmp1E2[1]=Cyc_Absynpp_pat2doc(c->pattern);_tmp1E2[0]=Cyc_PP_text(_tag_arr("case ",
sizeof(char),6));Cyc_PP_cat(_tag_arr(_tmp1E2,sizeof(struct Cyc_PP_Doc*),6));});}}}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,
_tag_arr("",sizeof(char),1),cs);}struct Cyc_PP_Doc*Cyc_Absynpp_switchCclause2doc(
struct Cyc_Absyn_SwitchC_clause*c){struct Cyc_Absyn_SwitchC_clause _tmp1E5;struct
Cyc_Absyn_Exp*_tmp1E6;struct Cyc_Absyn_Stmt*_tmp1E7;struct Cyc_Absyn_SwitchC_clause*
_tmp1E4=c;_tmp1E5=*_tmp1E4;_tmp1E6=_tmp1E5.cnst_exp;_tmp1E7=_tmp1E5.body;if(
_tmp1E6 == 0)return({struct Cyc_PP_Doc*_tmp1E8[2];_tmp1E8[1]=Cyc_PP_nest(2,({
struct Cyc_PP_Doc*_tmp1E9[2];_tmp1E9[1]=Cyc_Absynpp_stmt2doc(c->body);_tmp1E9[0]=
Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1E9,sizeof(struct Cyc_PP_Doc*),2));}));
_tmp1E8[0]=Cyc_PP_text(_tag_arr("default: ",sizeof(char),10));Cyc_PP_cat(
_tag_arr(_tmp1E8,sizeof(struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*
_tmp1EA[4];_tmp1EA[3]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp1EB[2];_tmp1EB[1]=Cyc_Absynpp_stmt2doc(
_tmp1E7);_tmp1EB[0]=Cyc_PP_line_doc();Cyc_PP_cat(_tag_arr(_tmp1EB,sizeof(struct
Cyc_PP_Doc*),2));}));_tmp1EA[2]=Cyc_PP_text(_tag_arr(": ",sizeof(char),3));
_tmp1EA[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(_tmp1E6));
_tmp1EA[0]=Cyc_PP_text(_tag_arr("case ",sizeof(char),6));Cyc_PP_cat(_tag_arr(
_tmp1EA,sizeof(struct Cyc_PP_Doc*),4));});}}struct Cyc_PP_Doc*Cyc_Absynpp_switchCclauses2doc(
struct Cyc_List_List*cs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_SwitchC_clause*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_switchCclause2doc,_tag_arr("",sizeof(char),1),cs);}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(
f->name);else{return({struct Cyc_PP_Doc*_tmp1EC[3];_tmp1EC[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag));_tmp1EC[1]=Cyc_PP_text(_tag_arr(" = ",
sizeof(char),4));_tmp1EC[0]=Cyc_Absynpp_qvar2doc(f->name);Cyc_PP_cat(_tag_arr(
_tmp1EC,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Enumfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_enumfield2doc,_tag_arr(",",sizeof(char),2),fs);}static struct Cyc_PP_Doc*
Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){return Cyc_Absynpp_qvar2doc(v->name);}
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){return((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,_tag_arr(",",
sizeof(char),2),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd){struct Cyc_Absyn_Vardecl _tmp1EE;void*_tmp1EF;struct _tuple0*_tmp1F0;struct Cyc_Absyn_Tqual
_tmp1F1;void*_tmp1F2;struct Cyc_Absyn_Exp*_tmp1F3;struct Cyc_List_List*_tmp1F4;
struct Cyc_Absyn_Vardecl*_tmp1ED=vd;_tmp1EE=*_tmp1ED;_tmp1EF=(void*)_tmp1EE.sc;
_tmp1F0=_tmp1EE.name;_tmp1F1=_tmp1EE.tq;_tmp1F2=(void*)_tmp1EE.type;_tmp1F3=
_tmp1EE.initializer;_tmp1F4=_tmp1EE.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*
sn=Cyc_Absynpp_typedef_name2doc(_tmp1F0);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp1F4);struct Cyc_PP_Doc*beforenamedoc;if(!Cyc_Absynpp_to_VC)beforenamedoc=
attsdoc;else{void*_tmp1F5=Cyc_Tcutil_compress(_tmp1F2);struct Cyc_Absyn_FnInfo
_tmp1F6;struct Cyc_List_List*_tmp1F7;_LL27F: if(_tmp1F5 <= (void*)3?1:*((int*)
_tmp1F5)!= 8)goto _LL281;_tmp1F6=((struct Cyc_Absyn_FnType_struct*)_tmp1F5)->f1;
_tmp1F7=_tmp1F6.attributes;_LL280: beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp1F7);
goto _LL27E;_LL281:;_LL282: beforenamedoc=Cyc_PP_nil_doc();goto _LL27E;_LL27E:;}s=({
struct Cyc_PP_Doc*_tmp1F8[5];_tmp1F8[4]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));
_tmp1F8[3]=_tmp1F3 == 0?Cyc_PP_text(_tag_arr("",sizeof(char),1)):({struct Cyc_PP_Doc*
_tmp1FB[2];_tmp1FB[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(
_tmp1F3));_tmp1FB[0]=Cyc_PP_text(_tag_arr(" = ",sizeof(char),4));Cyc_PP_cat(
_tag_arr(_tmp1FB,sizeof(struct Cyc_PP_Doc*),2));});_tmp1F8[2]=Cyc_Absynpp_tqtd2doc(
_tmp1F1,_tmp1F2,({struct Cyc_Core_Opt*_tmp1F9=_cycalloc(sizeof(*_tmp1F9));_tmp1F9->v=({
struct Cyc_PP_Doc*_tmp1FA[2];_tmp1FA[1]=sn;_tmp1FA[0]=beforenamedoc;Cyc_PP_cat(
_tag_arr(_tmp1FA,sizeof(struct Cyc_PP_Doc*),2));});_tmp1F9;}));_tmp1F8[1]=Cyc_Absynpp_scope2doc(
_tmp1EF);_tmp1F8[0]=Cyc_Absynpp_to_VC?attsdoc: Cyc_PP_nil_doc();Cyc_PP_cat(
_tag_arr(_tmp1F8,sizeof(struct Cyc_PP_Doc*),5));});return s;}}struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(
struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp1FC=(void*)d->r;struct Cyc_Absyn_Fndecl*
_tmp1FD;struct Cyc_Absyn_Aggrdecl*_tmp1FE;struct Cyc_Absyn_Vardecl*_tmp1FF;struct
Cyc_Absyn_Tuniondecl*_tmp200;struct Cyc_Absyn_Tuniondecl _tmp201;void*_tmp202;
struct _tuple0*_tmp203;struct Cyc_List_List*_tmp204;struct Cyc_Core_Opt*_tmp205;int
_tmp206;struct Cyc_Absyn_Pat*_tmp207;struct Cyc_Absyn_Exp*_tmp208;struct Cyc_List_List*
_tmp209;struct Cyc_Absyn_Enumdecl*_tmp20A;struct Cyc_Absyn_Enumdecl _tmp20B;void*
_tmp20C;struct _tuple0*_tmp20D;struct Cyc_Core_Opt*_tmp20E;struct Cyc_Absyn_Typedefdecl*
_tmp20F;struct _tagged_arr*_tmp210;struct Cyc_List_List*_tmp211;struct _tuple0*
_tmp212;struct Cyc_List_List*_tmp213;struct Cyc_List_List*_tmp214;_LL284: if(*((int*)
_tmp1FC)!= 1)goto _LL286;_tmp1FD=((struct Cyc_Absyn_Fn_d_struct*)_tmp1FC)->f1;
_LL285: {void*t=(void*)({struct Cyc_Absyn_FnType_struct*_tmp21C=_cycalloc(sizeof(*
_tmp21C));_tmp21C[0]=({struct Cyc_Absyn_FnType_struct _tmp21D;_tmp21D.tag=8;
_tmp21D.f1=({struct Cyc_Absyn_FnInfo _tmp21E;_tmp21E.tvars=_tmp1FD->tvs;_tmp21E.effect=
_tmp1FD->effect;_tmp21E.ret_typ=(void*)((void*)_tmp1FD->ret_type);_tmp21E.args=((
struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple6*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp1FD->args);_tmp21E.c_varargs=_tmp1FD->c_varargs;
_tmp21E.cyc_varargs=_tmp1FD->cyc_varargs;_tmp21E.rgn_po=_tmp1FD->rgn_po;_tmp21E.attributes=
0;_tmp21E;});_tmp21D;});_tmp21C;});struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp1FD->attributes);struct Cyc_PP_Doc*inlinedoc;if(_tmp1FD->is_inline){if(Cyc_Absynpp_to_VC)
inlinedoc=Cyc_PP_text(_tag_arr("__inline ",sizeof(char),10));else{inlinedoc=Cyc_PP_text(
_tag_arr("inline ",sizeof(char),8));}}else{inlinedoc=Cyc_PP_nil_doc();}{struct
Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((void*)_tmp1FD->sc);struct Cyc_PP_Doc*
beforenamedoc=Cyc_Absynpp_to_VC?Cyc_Absynpp_callconv2doc(_tmp1FD->attributes):
attsdoc;struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp1FD->name);
struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual _tmp219;
_tmp219.q_const=0;_tmp219.q_volatile=0;_tmp219.q_restrict=0;_tmp219;}),t,({
struct Cyc_Core_Opt*_tmp21A=_cycalloc(sizeof(*_tmp21A));_tmp21A->v=({struct Cyc_PP_Doc*
_tmp21B[2];_tmp21B[1]=namedoc;_tmp21B[0]=beforenamedoc;Cyc_PP_cat(_tag_arr(
_tmp21B,sizeof(struct Cyc_PP_Doc*),2));});_tmp21A;}));struct Cyc_PP_Doc*bodydoc=({
struct Cyc_PP_Doc*_tmp217[4];_tmp217[3]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp217[2]=Cyc_PP_line_doc();_tmp217[1]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp218[
2];_tmp218[1]=Cyc_Absynpp_stmt2doc(_tmp1FD->body);_tmp218[0]=Cyc_PP_line_doc();
Cyc_PP_cat(_tag_arr(_tmp218,sizeof(struct Cyc_PP_Doc*),2));}));_tmp217[0]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));Cyc_PP_cat(_tag_arr(_tmp217,sizeof(struct Cyc_PP_Doc*),
4));});s=({struct Cyc_PP_Doc*_tmp215[4];_tmp215[3]=bodydoc;_tmp215[2]=tqtddoc;
_tmp215[1]=scopedoc;_tmp215[0]=inlinedoc;Cyc_PP_cat(_tag_arr(_tmp215,sizeof(
struct Cyc_PP_Doc*),4));});if(Cyc_Absynpp_to_VC)s=({struct Cyc_PP_Doc*_tmp216[2];
_tmp216[1]=s;_tmp216[0]=attsdoc;Cyc_PP_cat(_tag_arr(_tmp216,sizeof(struct Cyc_PP_Doc*),
2));});goto _LL283;}}_LL286: if(*((int*)_tmp1FC)!= 4)goto _LL288;_tmp1FE=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp1FC)->f1;_LL287: if(_tmp1FE->impl == 0)s=({struct Cyc_PP_Doc*
_tmp21F[5];_tmp21F[4]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmp21F[3]=Cyc_Absynpp_ktvars2doc(
_tmp1FE->tvs);_tmp21F[2]=Cyc_Absynpp_qvar2doc(_tmp1FE->name);_tmp21F[1]=Cyc_Absynpp_aggr_kind2doc((
void*)_tmp1FE->kind);_tmp21F[0]=Cyc_Absynpp_scope2doc((void*)_tmp1FE->sc);Cyc_PP_cat(
_tag_arr(_tmp21F,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp220[11];_tmp220[10]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmp220[9]=Cyc_Absynpp_atts2doc(
_tmp1FE->attributes);_tmp220[8]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));
_tmp220[7]=Cyc_PP_line_doc();_tmp220[6]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp222[
2];_tmp222[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1FE->impl))->fields);_tmp222[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp222,sizeof(struct Cyc_PP_Doc*),2));}));_tmp220[5]=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1FE->impl))->rgn_po == 0?Cyc_PP_nil_doc():({struct Cyc_PP_Doc*
_tmp221[2];_tmp221[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp1FE->impl))->rgn_po);_tmp221[0]=Cyc_PP_text(_tag_arr(":",sizeof(
char),2));Cyc_PP_cat(_tag_arr(_tmp221,sizeof(struct Cyc_PP_Doc*),2));});_tmp220[4]=
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FE->impl))->exist_vars);
_tmp220[3]=Cyc_PP_text(_tag_arr(" {",sizeof(char),3));_tmp220[2]=Cyc_Absynpp_qvar2doc(
_tmp1FE->name);_tmp220[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp1FE->kind);
_tmp220[0]=Cyc_Absynpp_scope2doc((void*)_tmp1FE->sc);Cyc_PP_cat(_tag_arr(_tmp220,
sizeof(struct Cyc_PP_Doc*),11));});}goto _LL283;_LL288: if(*((int*)_tmp1FC)!= 0)
goto _LL28A;_tmp1FF=((struct Cyc_Absyn_Var_d_struct*)_tmp1FC)->f1;_LL289: s=Cyc_Absynpp_vardecl2doc(
_tmp1FF);goto _LL283;_LL28A: if(*((int*)_tmp1FC)!= 5)goto _LL28C;_tmp200=((struct
Cyc_Absyn_Tunion_d_struct*)_tmp1FC)->f1;_tmp201=*_tmp200;_tmp202=(void*)_tmp201.sc;
_tmp203=_tmp201.name;_tmp204=_tmp201.tvs;_tmp205=_tmp201.fields;_tmp206=_tmp201.is_xtunion;
_LL28B: if(_tmp205 == 0)s=({struct Cyc_PP_Doc*_tmp223[5];_tmp223[4]=Cyc_PP_text(
_tag_arr(";",sizeof(char),2));_tmp223[3]=Cyc_Absynpp_ktvars2doc(_tmp204);_tmp223[
2]=_tmp206?Cyc_Absynpp_qvar2doc(_tmp203): Cyc_Absynpp_typedef_name2doc(_tmp203);
_tmp223[1]=_tmp206?Cyc_PP_text(_tag_arr("xtunion ",sizeof(char),9)): Cyc_PP_text(
_tag_arr("tunion ",sizeof(char),8));_tmp223[0]=Cyc_Absynpp_scope2doc(_tmp202);
Cyc_PP_cat(_tag_arr(_tmp223,sizeof(struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*
_tmp224[8];_tmp224[7]=Cyc_PP_text(_tag_arr("};",sizeof(char),3));_tmp224[6]=Cyc_PP_line_doc();
_tmp224[5]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp225[2];_tmp225[1]=Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*)_tmp205->v);_tmp225[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp225,sizeof(struct Cyc_PP_Doc*),2));}));_tmp224[4]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));_tmp224[3]=Cyc_Absynpp_ktvars2doc(_tmp204);
_tmp224[2]=_tmp206?Cyc_Absynpp_qvar2doc(_tmp203): Cyc_Absynpp_typedef_name2doc(
_tmp203);_tmp224[1]=_tmp206?Cyc_PP_text(_tag_arr("xtunion ",sizeof(char),9)): Cyc_PP_text(
_tag_arr("tunion ",sizeof(char),8));_tmp224[0]=Cyc_Absynpp_scope2doc(_tmp202);
Cyc_PP_cat(_tag_arr(_tmp224,sizeof(struct Cyc_PP_Doc*),8));});}goto _LL283;_LL28C:
if(*((int*)_tmp1FC)!= 2)goto _LL28E;_tmp207=((struct Cyc_Absyn_Let_d_struct*)
_tmp1FC)->f1;_tmp208=((struct Cyc_Absyn_Let_d_struct*)_tmp1FC)->f3;_LL28D: s=({
struct Cyc_PP_Doc*_tmp226[5];_tmp226[4]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));
_tmp226[3]=Cyc_Absynpp_exp2doc(_tmp208);_tmp226[2]=Cyc_PP_text(_tag_arr(" = ",
sizeof(char),4));_tmp226[1]=Cyc_Absynpp_pat2doc(_tmp207);_tmp226[0]=Cyc_PP_text(
_tag_arr("let ",sizeof(char),5));Cyc_PP_cat(_tag_arr(_tmp226,sizeof(struct Cyc_PP_Doc*),
5));});goto _LL283;_LL28E: if(*((int*)_tmp1FC)!= 3)goto _LL290;_tmp209=((struct Cyc_Absyn_Letv_d_struct*)
_tmp1FC)->f1;_LL28F: s=({struct Cyc_PP_Doc*_tmp227[3];_tmp227[2]=Cyc_PP_text(
_tag_arr(";",sizeof(char),2));_tmp227[1]=Cyc_Absynpp_ids2doc(_tmp209);_tmp227[0]=
Cyc_PP_text(_tag_arr("let ",sizeof(char),5));Cyc_PP_cat(_tag_arr(_tmp227,sizeof(
struct Cyc_PP_Doc*),3));});goto _LL283;_LL290: if(*((int*)_tmp1FC)!= 6)goto _LL292;
_tmp20A=((struct Cyc_Absyn_Enum_d_struct*)_tmp1FC)->f1;_tmp20B=*_tmp20A;_tmp20C=(
void*)_tmp20B.sc;_tmp20D=_tmp20B.name;_tmp20E=_tmp20B.fields;_LL291: if(_tmp20E == 
0)s=({struct Cyc_PP_Doc*_tmp228[4];_tmp228[3]=Cyc_PP_text(_tag_arr(";",sizeof(
char),2));_tmp228[2]=Cyc_Absynpp_typedef_name2doc(_tmp20D);_tmp228[1]=Cyc_PP_text(
_tag_arr("enum ",sizeof(char),6));_tmp228[0]=Cyc_Absynpp_scope2doc(_tmp20C);Cyc_PP_cat(
_tag_arr(_tmp228,sizeof(struct Cyc_PP_Doc*),4));});else{s=({struct Cyc_PP_Doc*
_tmp229[7];_tmp229[6]=Cyc_PP_text(_tag_arr("};",sizeof(char),3));_tmp229[5]=Cyc_PP_line_doc();
_tmp229[4]=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_tmp22A[2];_tmp22A[1]=Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*)_tmp20E->v);_tmp22A[0]=Cyc_PP_line_doc();Cyc_PP_cat(
_tag_arr(_tmp22A,sizeof(struct Cyc_PP_Doc*),2));}));_tmp229[3]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));_tmp229[2]=Cyc_Absynpp_qvar2doc(_tmp20D);_tmp229[
1]=Cyc_PP_text(_tag_arr("enum ",sizeof(char),6));_tmp229[0]=Cyc_Absynpp_scope2doc(
_tmp20C);Cyc_PP_cat(_tag_arr(_tmp229,sizeof(struct Cyc_PP_Doc*),7));});}goto
_LL283;_LL292: if(*((int*)_tmp1FC)!= 7)goto _LL294;_tmp20F=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp1FC)->f1;_LL293: {void*t;if(_tmp20F->defn != 0)t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp20F->defn))->v;else{t=Cyc_Absyn_new_evar(_tmp20F->kind,0);}s=({
struct Cyc_PP_Doc*_tmp22B[3];_tmp22B[2]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));
_tmp22B[1]=Cyc_Absynpp_tqtd2doc(({struct Cyc_Absyn_Tqual _tmp22C;_tmp22C.q_const=0;
_tmp22C.q_volatile=0;_tmp22C.q_restrict=0;_tmp22C;}),t,({struct Cyc_Core_Opt*
_tmp22D=_cycalloc(sizeof(*_tmp22D));_tmp22D->v=({struct Cyc_PP_Doc*_tmp22E[2];
_tmp22E[1]=Cyc_Absynpp_tvars2doc(_tmp20F->tvs);_tmp22E[0]=Cyc_Absynpp_typedef_name2doc(
_tmp20F->name);Cyc_PP_cat(_tag_arr(_tmp22E,sizeof(struct Cyc_PP_Doc*),2));});
_tmp22D;}));_tmp22B[0]=Cyc_PP_text(_tag_arr("typedef ",sizeof(char),9));Cyc_PP_cat(
_tag_arr(_tmp22B,sizeof(struct Cyc_PP_Doc*),3));});goto _LL283;}_LL294: if(*((int*)
_tmp1FC)!= 8)goto _LL296;_tmp210=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1FC)->f1;
_tmp211=((struct Cyc_Absyn_Namespace_d_struct*)_tmp1FC)->f2;_LL295: if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_add(_tmp210);s=({struct Cyc_PP_Doc*_tmp22F[7];_tmp22F[
6]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp22F[5]=Cyc_PP_line_doc();
_tmp22F[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(char),1),_tmp211);_tmp22F[3]=Cyc_PP_line_doc();_tmp22F[2]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));_tmp22F[1]=Cyc_PP_textptr(_tmp210);_tmp22F[0]=Cyc_PP_text(
_tag_arr("namespace ",sizeof(char),11));Cyc_PP_cat(_tag_arr(_tmp22F,sizeof(
struct Cyc_PP_Doc*),7));});if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL283;_LL296: if(*((int*)_tmp1FC)!= 9)goto _LL298;_tmp212=((struct Cyc_Absyn_Using_d_struct*)
_tmp1FC)->f1;_tmp213=((struct Cyc_Absyn_Using_d_struct*)_tmp1FC)->f2;_LL297: if(
Cyc_Absynpp_print_using_stmts)s=({struct Cyc_PP_Doc*_tmp230[7];_tmp230[6]=Cyc_PP_text(
_tag_arr("}",sizeof(char),2));_tmp230[5]=Cyc_PP_line_doc();_tmp230[4]=((struct
Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _tagged_arr
sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,_tag_arr("",
sizeof(char),1),_tmp213);_tmp230[3]=Cyc_PP_line_doc();_tmp230[2]=Cyc_PP_text(
_tag_arr(" {",sizeof(char),3));_tmp230[1]=Cyc_Absynpp_qvar2doc(_tmp212);_tmp230[
0]=Cyc_PP_text(_tag_arr("using ",sizeof(char),7));Cyc_PP_cat(_tag_arr(_tmp230,
sizeof(struct Cyc_PP_Doc*),7));});else{s=({struct Cyc_PP_Doc*_tmp231[7];_tmp231[6]=
Cyc_PP_text(_tag_arr("/* } */",sizeof(char),8));_tmp231[5]=Cyc_PP_line_doc();
_tmp231[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(char),1),_tmp213);_tmp231[3]=Cyc_PP_line_doc();_tmp231[2]=Cyc_PP_text(
_tag_arr(" { */",sizeof(char),6));_tmp231[1]=Cyc_Absynpp_qvar2doc(_tmp212);
_tmp231[0]=Cyc_PP_text(_tag_arr("/* using ",sizeof(char),10));Cyc_PP_cat(
_tag_arr(_tmp231,sizeof(struct Cyc_PP_Doc*),7));});}goto _LL283;_LL298: if(*((int*)
_tmp1FC)!= 10)goto _LL283;_tmp214=((struct Cyc_Absyn_ExternC_d_struct*)_tmp1FC)->f1;
_LL299: if(Cyc_Absynpp_print_externC_stmts)s=({struct Cyc_PP_Doc*_tmp232[5];
_tmp232[4]=Cyc_PP_text(_tag_arr("}",sizeof(char),2));_tmp232[3]=Cyc_PP_line_doc();
_tmp232[2]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),
struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(char),1),_tmp214);_tmp232[1]=Cyc_PP_line_doc();_tmp232[0]=Cyc_PP_text(
_tag_arr("extern \"C\" {",sizeof(char),13));Cyc_PP_cat(_tag_arr(_tmp232,sizeof(
struct Cyc_PP_Doc*),5));});else{s=({struct Cyc_PP_Doc*_tmp233[5];_tmp233[4]=Cyc_PP_text(
_tag_arr("/* } */",sizeof(char),8));_tmp233[3]=Cyc_PP_line_doc();_tmp233[2]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,
_tag_arr("",sizeof(char),1),_tmp214);_tmp233[1]=Cyc_PP_line_doc();_tmp233[0]=Cyc_PP_text(
_tag_arr("/* extern \"C\" { */",sizeof(char),19));Cyc_PP_cat(_tag_arr(_tmp233,
sizeof(struct Cyc_PP_Doc*),5));});}goto _LL283;_LL283:;}return s;}struct Cyc_PP_Doc*
Cyc_Absynpp_scope2doc(void*sc){void*_tmp234=sc;_LL29B: if((int)_tmp234 != 0)goto
_LL29D;_LL29C: return Cyc_PP_text(_tag_arr("static ",sizeof(char),8));_LL29D: if((
int)_tmp234 != 2)goto _LL29F;_LL29E: return Cyc_PP_nil_doc();_LL29F: if((int)_tmp234
!= 3)goto _LL2A1;_LL2A0: return Cyc_PP_text(_tag_arr("extern ",sizeof(char),8));
_LL2A1: if((int)_tmp234 != 4)goto _LL2A3;_LL2A2: return Cyc_PP_text(_tag_arr("extern \"C\" ",
sizeof(char),12));_LL2A3: if((int)_tmp234 != 1)goto _LL2A5;_LL2A4: return Cyc_PP_text(
_tag_arr("abstract ",sizeof(char),10));_LL2A5: if((int)_tmp234 != 5)goto _LL29A;
_LL2A6: return Cyc_PP_text(_tag_arr("register ",sizeof(char),10));_LL29A:;}int Cyc_Absynpp_exists_temp_tvar_in_effect(
void*t){void*_tmp235=t;struct Cyc_Absyn_Tvar*_tmp236;struct Cyc_List_List*_tmp237;
_LL2A8: if(_tmp235 <= (void*)3?1:*((int*)_tmp235)!= 1)goto _LL2AA;_tmp236=((struct
Cyc_Absyn_VarType_struct*)_tmp235)->f1;_LL2A9: return Cyc_Tcutil_is_temp_tvar(
_tmp236);_LL2AA: if(_tmp235 <= (void*)3?1:*((int*)_tmp235)!= 20)goto _LL2AC;_tmp237=((
struct Cyc_Absyn_JoinEff_struct*)_tmp235)->f1;_LL2AB: return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,
_tmp237);_LL2AC:;_LL2AD: return 0;_LL2A7:;}int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp238=t;struct Cyc_Core_Opt*_tmp239;struct Cyc_Core_Opt _tmp23A;void*_tmp23B;
_LL2AF: if(_tmp238 <= (void*)3?1:*((int*)_tmp238)!= 11)goto _LL2B1;_LL2B0: return 1;
_LL2B1: if(_tmp238 <= (void*)3?1:*((int*)_tmp238)!= 16)goto _LL2B3;_tmp239=((struct
Cyc_Absyn_TypedefType_struct*)_tmp238)->f4;if(_tmp239 == 0)goto _LL2B3;_tmp23A=*
_tmp239;_tmp23B=(void*)_tmp23A.v;_LL2B2: return Cyc_Absynpp_is_anon_aggrtype(
_tmp23B);_LL2B3:;_LL2B4: return 0;_LL2AE:;}struct _tuple7 Cyc_Absynpp_to_tms(struct
Cyc_Absyn_Tqual tq,void*t){void*_tmp23C=t;void*_tmp23D;struct Cyc_Absyn_Tqual
_tmp23E;struct Cyc_Absyn_Exp*_tmp23F;struct Cyc_Absyn_PtrInfo _tmp240;void*_tmp241;
void*_tmp242;struct Cyc_Absyn_Conref*_tmp243;struct Cyc_Absyn_Tqual _tmp244;struct
Cyc_Absyn_Conref*_tmp245;struct Cyc_Absyn_FnInfo _tmp246;struct Cyc_List_List*
_tmp247;struct Cyc_Core_Opt*_tmp248;void*_tmp249;struct Cyc_List_List*_tmp24A;int
_tmp24B;struct Cyc_Absyn_VarargInfo*_tmp24C;struct Cyc_List_List*_tmp24D;struct Cyc_List_List*
_tmp24E;struct Cyc_Core_Opt*_tmp24F;struct Cyc_Core_Opt*_tmp250;int _tmp251;struct
_tuple0*_tmp252;struct Cyc_List_List*_tmp253;struct Cyc_Core_Opt*_tmp254;_LL2B6:
if(_tmp23C <= (void*)3?1:*((int*)_tmp23C)!= 7)goto _LL2B8;_tmp23D=(void*)((struct
Cyc_Absyn_ArrayType_struct*)_tmp23C)->f1;_tmp23E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp23C)->f2;_tmp23F=((struct Cyc_Absyn_ArrayType_struct*)_tmp23C)->f3;_LL2B7: {
struct Cyc_Absyn_Tqual _tmp256;void*_tmp257;struct Cyc_List_List*_tmp258;struct
_tuple7 _tmp255=Cyc_Absynpp_to_tms(_tmp23E,_tmp23D);_tmp256=_tmp255.f1;_tmp257=
_tmp255.f2;_tmp258=_tmp255.f3;{void*tm;if(_tmp23F == 0)tm=(void*)0;else{tm=(void*)({
struct Cyc_Absyn_ConstArray_mod_struct*_tmp259=_cycalloc(sizeof(*_tmp259));
_tmp259[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp25A;_tmp25A.tag=0;_tmp25A.f1=(
struct Cyc_Absyn_Exp*)_check_null(_tmp23F);_tmp25A;});_tmp259;});}return({struct
_tuple7 _tmp25B;_tmp25B.f1=_tmp256;_tmp25B.f2=_tmp257;_tmp25B.f3=({struct Cyc_List_List*
_tmp25C=_cycalloc(sizeof(*_tmp25C));_tmp25C->hd=(void*)tm;_tmp25C->tl=_tmp258;
_tmp25C;});_tmp25B;});}}_LL2B8: if(_tmp23C <= (void*)3?1:*((int*)_tmp23C)!= 4)goto
_LL2BA;_tmp240=((struct Cyc_Absyn_PointerType_struct*)_tmp23C)->f1;_tmp241=(void*)
_tmp240.elt_typ;_tmp242=(void*)_tmp240.rgn_typ;_tmp243=_tmp240.nullable;_tmp244=
_tmp240.tq;_tmp245=_tmp240.bounds;_LL2B9: {struct Cyc_Absyn_Tqual _tmp25E;void*
_tmp25F;struct Cyc_List_List*_tmp260;struct _tuple7 _tmp25D=Cyc_Absynpp_to_tms(
_tmp244,_tmp241);_tmp25E=_tmp25D.f1;_tmp25F=_tmp25D.f2;_tmp260=_tmp25D.f3;{void*
ps;{struct _tuple5 _tmp262=({struct _tuple5 _tmp261;_tmp261.f1=(void*)(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmp243))->v;_tmp261.f2=(
void*)(Cyc_Absyn_compress_conref(_tmp245))->v;_tmp261;});void*_tmp263;int _tmp264;
void*_tmp265;void*_tmp266;struct Cyc_Absyn_Exp*_tmp267;void*_tmp268;int _tmp269;
void*_tmp26A;void*_tmp26B;struct Cyc_Absyn_Exp*_tmp26C;void*_tmp26D;void*_tmp26E;
_LL2C3: _tmp263=_tmp262.f1;if(_tmp263 <= (void*)1?1:*((int*)_tmp263)!= 0)goto
_LL2C5;_tmp264=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp263)->f1;if(_tmp264
!= 1)goto _LL2C5;_tmp265=_tmp262.f2;if(_tmp265 <= (void*)1?1:*((int*)_tmp265)!= 0)
goto _LL2C5;_tmp266=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp265)->f1;if(
_tmp266 <= (void*)1?1:*((int*)_tmp266)!= 0)goto _LL2C5;_tmp267=((struct Cyc_Absyn_Upper_b_struct*)
_tmp266)->f1;_LL2C4: ps=(void*)({struct Cyc_Absyn_Nullable_ps_struct*_tmp26F=
_cycalloc(sizeof(*_tmp26F));_tmp26F[0]=({struct Cyc_Absyn_Nullable_ps_struct
_tmp270;_tmp270.tag=1;_tmp270.f1=_tmp267;_tmp270;});_tmp26F;});goto _LL2C2;_LL2C5:
_tmp268=_tmp262.f1;if(_tmp268 <= (void*)1?1:*((int*)_tmp268)!= 0)goto _LL2C7;
_tmp269=(int)((struct Cyc_Absyn_Eq_constr_struct*)_tmp268)->f1;if(_tmp269 != 0)
goto _LL2C7;_tmp26A=_tmp262.f2;if(_tmp26A <= (void*)1?1:*((int*)_tmp26A)!= 0)goto
_LL2C7;_tmp26B=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp26A)->f1;if(
_tmp26B <= (void*)1?1:*((int*)_tmp26B)!= 0)goto _LL2C7;_tmp26C=((struct Cyc_Absyn_Upper_b_struct*)
_tmp26B)->f1;_LL2C6: ps=(void*)({struct Cyc_Absyn_NonNullable_ps_struct*_tmp271=
_cycalloc(sizeof(*_tmp271));_tmp271[0]=({struct Cyc_Absyn_NonNullable_ps_struct
_tmp272;_tmp272.tag=0;_tmp272.f1=_tmp26C;_tmp272;});_tmp271;});goto _LL2C2;_LL2C7:
_tmp26D=_tmp262.f2;if(_tmp26D <= (void*)1?1:*((int*)_tmp26D)!= 0)goto _LL2C9;
_tmp26E=(void*)((struct Cyc_Absyn_Eq_constr_struct*)_tmp26D)->f1;if((int)_tmp26E
!= 0)goto _LL2C9;_LL2C8: ps=(void*)0;goto _LL2C2;_LL2C9:;_LL2CA: ps=(void*)0;goto
_LL2C2;_LL2C2:;}{void*tm=(void*)({struct Cyc_Absyn_Pointer_mod_struct*_tmp275=
_cycalloc(sizeof(*_tmp275));_tmp275[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp276;_tmp276.tag=1;_tmp276.f1=(void*)ps;_tmp276.f2=(void*)_tmp242;_tmp276.f3=
tq;_tmp276;});_tmp275;});return({struct _tuple7 _tmp273;_tmp273.f1=_tmp25E;_tmp273.f2=
_tmp25F;_tmp273.f3=({struct Cyc_List_List*_tmp274=_cycalloc(sizeof(*_tmp274));
_tmp274->hd=(void*)tm;_tmp274->tl=_tmp260;_tmp274;});_tmp273;});}}}_LL2BA: if(
_tmp23C <= (void*)3?1:*((int*)_tmp23C)!= 8)goto _LL2BC;_tmp246=((struct Cyc_Absyn_FnType_struct*)
_tmp23C)->f1;_tmp247=_tmp246.tvars;_tmp248=_tmp246.effect;_tmp249=(void*)_tmp246.ret_typ;
_tmp24A=_tmp246.args;_tmp24B=_tmp246.c_varargs;_tmp24C=_tmp246.cyc_varargs;
_tmp24D=_tmp246.rgn_po;_tmp24E=_tmp246.attributes;_LL2BB: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp248 == 0?1: Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp248->v)){
_tmp248=0;_tmp247=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp247);}{struct Cyc_Absyn_Tqual _tmp278;void*_tmp279;struct Cyc_List_List*_tmp27A;
struct _tuple7 _tmp277=Cyc_Absynpp_to_tms(Cyc_Absyn_empty_tqual(),_tmp249);_tmp278=
_tmp277.f1;_tmp279=_tmp277.f2;_tmp27A=_tmp277.f3;{struct Cyc_List_List*tms=
_tmp27A;if(_tmp24E != 0?!Cyc_Absynpp_to_VC: 0)tms=({struct Cyc_List_List*_tmp27B=
_cycalloc(sizeof(*_tmp27B));_tmp27B->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp27C=_cycalloc(sizeof(*_tmp27C));_tmp27C[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp27D;_tmp27D.tag=4;_tmp27D.f1=0;_tmp27D.f2=_tmp24E;_tmp27D;});_tmp27C;}));
_tmp27B->tl=tms;_tmp27B;});tms=({struct Cyc_List_List*_tmp27E=_cycalloc(sizeof(*
_tmp27E));_tmp27E->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp27F=_cycalloc(sizeof(*_tmp27F));_tmp27F[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp280;_tmp280.tag=2;_tmp280.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp282;_tmp282.tag=1;_tmp282.f1=_tmp24A;_tmp282.f2=_tmp24B;_tmp282.f3=_tmp24C;
_tmp282.f4=_tmp248;_tmp282.f5=_tmp24D;_tmp282;});_tmp281;}));_tmp280;});_tmp27F;}));
_tmp27E->tl=tms;_tmp27E;});if(Cyc_Absynpp_to_VC)for(0;_tmp24E != 0;_tmp24E=
_tmp24E->tl){void*_tmp283=(void*)_tmp24E->hd;_LL2CC: if((int)_tmp283 != 0)goto
_LL2CE;_LL2CD: goto _LL2CF;_LL2CE: if((int)_tmp283 != 1)goto _LL2D0;_LL2CF: goto _LL2D1;
_LL2D0: if((int)_tmp283 != 2)goto _LL2D2;_LL2D1: tms=({struct Cyc_List_List*_tmp284=
_cycalloc(sizeof(*_tmp284));_tmp284->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp285=_cycalloc(sizeof(*_tmp285));_tmp285[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp286;_tmp286.tag=4;_tmp286.f1=0;_tmp286.f2=({struct Cyc_List_List*_tmp287=
_cycalloc(sizeof(*_tmp287));_tmp287->hd=(void*)((void*)_tmp24E->hd);_tmp287->tl=
0;_tmp287;});_tmp286;});_tmp285;}));_tmp284->tl=tms;_tmp284;});goto AfterAtts;
_LL2D2:;_LL2D3: goto _LL2CB;_LL2CB:;}AfterAtts: if(_tmp247 != 0)tms=({struct Cyc_List_List*
_tmp288=_cycalloc(sizeof(*_tmp288));_tmp288->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp289=_cycalloc(sizeof(*_tmp289));_tmp289[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp28A;_tmp28A.tag=3;_tmp28A.f1=_tmp247;_tmp28A.f2=0;_tmp28A.f3=1;_tmp28A;});
_tmp289;}));_tmp288->tl=tms;_tmp288;});return({struct _tuple7 _tmp28B;_tmp28B.f1=
_tmp278;_tmp28B.f2=_tmp279;_tmp28B.f3=tms;_tmp28B;});}}_LL2BC: if(_tmp23C <= (void*)
3?1:*((int*)_tmp23C)!= 0)goto _LL2BE;_tmp24F=((struct Cyc_Absyn_Evar_struct*)
_tmp23C)->f1;_tmp250=((struct Cyc_Absyn_Evar_struct*)_tmp23C)->f2;_tmp251=((
struct Cyc_Absyn_Evar_struct*)_tmp23C)->f3;_LL2BD: if(_tmp250 == 0)return({struct
_tuple7 _tmp28C;_tmp28C.f1=tq;_tmp28C.f2=t;_tmp28C.f3=0;_tmp28C;});else{return Cyc_Absynpp_to_tms(
tq,(void*)_tmp250->v);}_LL2BE: if(_tmp23C <= (void*)3?1:*((int*)_tmp23C)!= 16)goto
_LL2C0;_tmp252=((struct Cyc_Absyn_TypedefType_struct*)_tmp23C)->f1;_tmp253=((
struct Cyc_Absyn_TypedefType_struct*)_tmp23C)->f2;_tmp254=((struct Cyc_Absyn_TypedefType_struct*)
_tmp23C)->f4;_LL2BF: if((_tmp254 == 0?1: !Cyc_Absynpp_expand_typedefs)?1: Cyc_Absynpp_is_anon_aggrtype((
void*)_tmp254->v))return({struct _tuple7 _tmp28D;_tmp28D.f1=tq;_tmp28D.f2=t;
_tmp28D.f3=0;_tmp28D;});else{return Cyc_Absynpp_to_tms(tq,(void*)_tmp254->v);}
_LL2C0:;_LL2C1: return({struct _tuple7 _tmp28E;_tmp28E.f1=tq;_tmp28E.f2=t;_tmp28E.f3=
0;_tmp28E;});_LL2B5:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_Core_Opt*dopt){struct Cyc_Absyn_Tqual _tmp290;void*_tmp291;
struct Cyc_List_List*_tmp292;struct _tuple7 _tmp28F=Cyc_Absynpp_to_tms(tq,t);
_tmp290=_tmp28F.f1;_tmp291=_tmp28F.f2;_tmp292=_tmp28F.f3;_tmp292=Cyc_List_imp_rev(
_tmp292);if(_tmp292 == 0?dopt == 0: 0)return({struct Cyc_PP_Doc*_tmp293[2];_tmp293[1]=
Cyc_Absynpp_ntyp2doc(_tmp291);_tmp293[0]=Cyc_Absynpp_tqual2doc(_tmp290);Cyc_PP_cat(
_tag_arr(_tmp293,sizeof(struct Cyc_PP_Doc*),2));});else{return({struct Cyc_PP_Doc*
_tmp294[4];_tmp294[3]=Cyc_Absynpp_dtms2doc(dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)
dopt->v,_tmp292);_tmp294[2]=Cyc_PP_text(_tag_arr(" ",sizeof(char),2));_tmp294[1]=
Cyc_Absynpp_ntyp2doc(_tmp291);_tmp294[0]=Cyc_Absynpp_tqual2doc(_tmp290);Cyc_PP_cat(
_tag_arr(_tmp294,sizeof(struct Cyc_PP_Doc*),4));});}}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(
struct Cyc_Absyn_Aggrfield*f){if(f->width != 0)return({struct Cyc_PP_Doc*_tmp295[5];
_tmp295[4]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmp295[3]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp295[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(
f->width));_tmp295[1]=Cyc_PP_text(_tag_arr(":",sizeof(char),2));_tmp295[0]=Cyc_Absynpp_tqtd2doc(
f->tq,(void*)f->type,({struct Cyc_Core_Opt*_tmp296=_cycalloc(sizeof(*_tmp296));
_tmp296->v=Cyc_PP_textptr(f->name);_tmp296;}));Cyc_PP_cat(_tag_arr(_tmp295,
sizeof(struct Cyc_PP_Doc*),5));});else{return({struct Cyc_PP_Doc*_tmp297[3];
_tmp297[2]=Cyc_PP_text(_tag_arr(";",sizeof(char),2));_tmp297[1]=Cyc_Absynpp_atts2doc(
f->attributes);_tmp297[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,({struct Cyc_Core_Opt*
_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298->v=Cyc_PP_textptr(f->name);_tmp298;}));
Cyc_PP_cat(_tag_arr(_tmp297,sizeof(struct Cyc_PP_Doc*),3));});}}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _tagged_arr sep,struct
Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,_tag_arr("",sizeof(
char),1),fields);}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct Cyc_Absyn_Tunionfield*
f){return({struct Cyc_PP_Doc*_tmp299[3];_tmp299[2]=f->typs == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_args2doc(f->typs);_tmp299[1]=Cyc_Absynpp_typedef_name2doc(f->name);
_tmp299[0]=Cyc_Absynpp_scope2doc((void*)f->sc);Cyc_PP_cat(_tag_arr(_tmp299,
sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields){return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Tunionfield*),struct _tagged_arr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_tunionfield2doc,_tag_arr(",",sizeof(char),2),fields);}void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f){for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);({
void*_tmp29A[0]={};Cyc_Std_fprintf(f,_tag_arr("\n",sizeof(char),2),_tag_arr(
_tmp29A,sizeof(void*),0));});}}struct _tagged_arr Cyc_Absynpp_decllist2string(
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
