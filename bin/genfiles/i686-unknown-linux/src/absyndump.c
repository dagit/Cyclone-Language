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
 struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*
Cyc_Std_stdout;extern int Cyc_Std_fputc(int __c,struct Cyc_Std___cycFILE*__stream);
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
extern int Cyc_Std_file_string_write(struct Cyc_Std___cycFILE*fd,struct _tagged_arr
src,int src_offset,int max_count);struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,
struct _tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(
struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct
Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;}
;struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern int
Cyc_List_length(struct Cyc_List_List*x);extern struct Cyc_List_List*Cyc_List_map(
void*(*f)(void*),struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[
18];extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern unsigned char Cyc_List_Nth[8];extern int Cyc_List_exists(int(*pred)(void*),
struct Cyc_List_List*x);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
unsigned char Cyc_Position_Nocontext[14];extern struct _tagged_arr Cyc_Position_get_line_directive(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple0{
void*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
q_const: 1;int q_volatile: 1;int q_restrict: 1;};struct Cyc_Absyn_Conref{void*v;};
struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
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
15];extern void*Cyc_Absyn_compress_kb(void*);extern struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);extern
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
extern struct _tagged_arr Cyc_Absyn_attribute2string(void*);struct _tuple3{void*f1;
struct _tuple0*f2;};extern struct _tuple3 Cyc_Absyn_aggr_kinded_name(void*);struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
extern int Cyc_Absynpp_is_anon_aggrtype(void*t);extern struct _tagged_arr*Cyc_Absynpp_cyc_stringptr;
extern int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);extern struct _tagged_arr Cyc_Absynpp_char_escape(
unsigned char);extern struct _tagged_arr Cyc_Absynpp_string_escape(struct
_tagged_arr);extern struct _tagged_arr Cyc_Absynpp_prim2str(void*p);extern int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);struct _tuple4{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};extern struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple4*arg);
struct _tuple5{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};extern
struct _tuple5 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual tq,void*t);extern
unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[
12];extern unsigned char Cyc_Dict_Absent[11];struct Cyc_Tcenv_VarRes_struct{int tag;
void*f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};
struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*
f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict*
aggrdecls;struct Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct
Cyc_Dict_Dict*typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*
availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;};struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{int tag;void*f1;void*f2;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*
le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_compress(
void*t);static int Cyc_Absyndump_expand_typedefs;static int Cyc_Absyndump_qvar_to_Cids;
static int Cyc_Absyndump_add_cyc_prefix;static int Cyc_Absyndump_generate_line_directives;
static int Cyc_Absyndump_to_VC;void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*
fs){Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;Cyc_Absyndump_qvar_to_Cids=
fs->qvar_to_Cids;Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absyndump_to_VC=
fs->to_VC;Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_set_params(fs);}void Cyc_Absyndump_dumptyp(void*);void Cyc_Absyndump_dumpntyp(
void*t);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumpexp_prec(
int,struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);void
Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*);void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*);void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(
void*),void*a);void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(
void*),void*);void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);void
Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);void Cyc_Absyndump_dumploc(
struct Cyc_Position_Segment*);struct Cyc_Std___cycFILE**Cyc_Absyndump_dump_file=&
Cyc_Std_stdout;void Cyc_Absyndump_ignore(void*x){return;}static unsigned int Cyc_Absyndump_pos=
0;static unsigned char Cyc_Absyndump_prev_char='x';int Cyc_Absyndump_need_space_before(){
switch(Cyc_Absyndump_prev_char){case '{': _LL0: goto _LL1;case '}': _LL1: goto _LL2;case
'(': _LL2: goto _LL3;case ')': _LL3: goto _LL4;case '[': _LL4: goto _LL5;case ']': _LL5: goto
_LL6;case ';': _LL6: goto _LL7;case ',': _LL7: goto _LL8;case '=': _LL8: goto _LL9;case ' ':
_LL9: goto _LLA;case '\n': _LLA: goto _LLB;case '*': _LLB: return 0;default: _LLC: return 1;}}
void Cyc_Absyndump_dump(struct _tagged_arr s){int sz=(int)_get_arr_size(s,sizeof(
unsigned char));if(!((int)*((const unsigned char*)_check_unknown_subscript(s,
sizeof(unsigned char),sz - 1)))){-- sz;}Cyc_Absyndump_pos +=sz;if(Cyc_Absyndump_pos
> 80){Cyc_Absyndump_pos=(unsigned int)sz;Cyc_Std_fputc((int)'\n',*Cyc_Absyndump_dump_file);}
else{if(Cyc_Absyndump_need_space_before()){Cyc_Std_fputc((int)' ',*Cyc_Absyndump_dump_file);}}
Cyc_Std_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);if(sz >= 1){Cyc_Absyndump_prev_char=*((
const unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),sz - 1));}}
void Cyc_Absyndump_dump_nospace(struct _tagged_arr s){int sz=(int)_get_arr_size(s,
sizeof(unsigned char));if(!((int)*((const unsigned char*)_check_unknown_subscript(
s,sizeof(unsigned char),sz - 1)))){-- sz;}Cyc_Absyndump_pos +=sz;Cyc_Std_file_string_write(*
Cyc_Absyndump_dump_file,s,0,sz);if(sz >= 1){Cyc_Absyndump_prev_char=*((const
unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),sz - 1));}}void
Cyc_Absyndump_dump_char(int c){++ Cyc_Absyndump_pos;Cyc_Std_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(unsigned char)c;}void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*
loc){if(loc == 0){return;}if(! Cyc_Absyndump_generate_line_directives){return;}{
struct _tagged_arr _tmp0=Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(
_tmp0);}}void Cyc_Absyndump_dump_str(struct _tagged_arr*s){Cyc_Absyndump_dump(*s);}
void Cyc_Absyndump_dump_semi(){Cyc_Absyndump_dump_char((int)';');}void Cyc_Absyndump_dump_sep(
void(*f)(void*),struct Cyc_List_List*l,struct _tagged_arr sep){if(l == 0){return;}
for(0;l->tl != 0;l=l->tl){f((void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f((
void*)l->hd);}void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct
Cyc_List_List*l,struct _tagged_arr sep){if(l == 0){return;}for(0;l->tl != 0;l=l->tl){
f(env,(void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f(env,(void*)l->hd);}void
Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep(
f,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(void(*f)(
void*,void*),void*env,struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep_c(
f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_egroup(void(*f)(
void*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep){if(l != 0){Cyc_Absyndump_group(f,l,start,end,sep);}}void Cyc_Absyndump_dumpqvar(
struct _tuple0*v){struct Cyc_List_List*_tmp1=0;{void*_tmp2=(*v).f1;struct Cyc_List_List*
_tmp3;struct Cyc_List_List*_tmp4;_LLF: if((int)_tmp2 == 0){goto _LL10;}else{goto
_LL11;}_LL11: if((unsigned int)_tmp2 > 1?*((int*)_tmp2)== 0: 0){_LL15: _tmp3=((
struct Cyc_Absyn_Rel_n_struct*)_tmp2)->f1;goto _LL12;}else{goto _LL13;}_LL13: if((
unsigned int)_tmp2 > 1?*((int*)_tmp2)== 1: 0){_LL16: _tmp4=((struct Cyc_Absyn_Abs_n_struct*)
_tmp2)->f1;goto _LL14;}else{goto _LLE;}_LL10: _tmp3=0;goto _LL12;_LL12: _tmp1=_tmp3;
goto _LLE;_LL14: _tmp1=(Cyc_Absyndump_qvar_to_Cids? Cyc_Absyndump_add_cyc_prefix: 0)?({
struct Cyc_List_List*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->hd=Cyc_Absynpp_cyc_stringptr;
_tmp5->tl=_tmp4;_tmp5;}): _tmp4;goto _LLE;_LLE:;}if(_tmp1 != 0){Cyc_Absyndump_dump_str((
struct _tagged_arr*)_tmp1->hd);for(_tmp1=_tmp1->tl;_tmp1 != 0;_tmp1=_tmp1->tl){if(
Cyc_Absyndump_qvar_to_Cids){Cyc_Absyndump_dump_char((int)'_');}else{Cyc_Absyndump_dump_nospace(
_tag_arr("::",sizeof(unsigned char),3));}Cyc_Absyndump_dump_nospace(*((struct
_tagged_arr*)_tmp1->hd));}if(Cyc_Absyndump_qvar_to_Cids){Cyc_Absyndump_dump_nospace(
_tag_arr("_",sizeof(unsigned char),2));}else{Cyc_Absyndump_dump_nospace(_tag_arr("::",
sizeof(unsigned char),3));}Cyc_Absyndump_dump_nospace(*(*v).f2);}else{Cyc_Absyndump_dump_str((*
v).f2);}}void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict){Cyc_Absyndump_dump(
_tag_arr("restrict",sizeof(unsigned char),9));}if(tq.q_volatile){Cyc_Absyndump_dump(
_tag_arr("volatile",sizeof(unsigned char),9));}if(tq.q_const){Cyc_Absyndump_dump(
_tag_arr("const",sizeof(unsigned char),6));}}void Cyc_Absyndump_dumpscope(void*sc){
void*_tmp6=sc;_LL18: if((int)_tmp6 == 0){goto _LL19;}else{goto _LL1A;}_LL1A: if((int)
_tmp6 == 2){goto _LL1B;}else{goto _LL1C;}_LL1C: if((int)_tmp6 == 3){goto _LL1D;}else{
goto _LL1E;}_LL1E: if((int)_tmp6 == 4){goto _LL1F;}else{goto _LL20;}_LL20: if((int)
_tmp6 == 1){goto _LL21;}else{goto _LL17;}_LL19: Cyc_Absyndump_dump(_tag_arr("static",
sizeof(unsigned char),7));return;_LL1B: return;_LL1D: Cyc_Absyndump_dump(_tag_arr("extern",
sizeof(unsigned char),7));return;_LL1F: Cyc_Absyndump_dump(_tag_arr("extern \"C\"",
sizeof(unsigned char),11));return;_LL21: Cyc_Absyndump_dump(_tag_arr("abstract",
sizeof(unsigned char),9));return;_LL17:;}void Cyc_Absyndump_dumpkind(void*k){void*
_tmp7=k;_LL23: if((int)_tmp7 == 0){goto _LL24;}else{goto _LL25;}_LL25: if((int)_tmp7
== 1){goto _LL26;}else{goto _LL27;}_LL27: if((int)_tmp7 == 2){goto _LL28;}else{goto
_LL29;}_LL29: if((int)_tmp7 == 3){goto _LL2A;}else{goto _LL2B;}_LL2B: if((int)_tmp7 == 
4){goto _LL2C;}else{goto _LL22;}_LL24: Cyc_Absyndump_dump(_tag_arr("A",sizeof(
unsigned char),2));return;_LL26: Cyc_Absyndump_dump(_tag_arr("M",sizeof(
unsigned char),2));return;_LL28: Cyc_Absyndump_dump(_tag_arr("B",sizeof(
unsigned char),2));return;_LL2A: Cyc_Absyndump_dump(_tag_arr("R",sizeof(
unsigned char),2));return;_LL2C: Cyc_Absyndump_dump(_tag_arr("E",sizeof(
unsigned char),2));return;_LL22:;}void Cyc_Absyndump_dumpaggr_kind(void*k){void*
_tmp8=k;_LL2E: if((int)_tmp8 == 0){goto _LL2F;}else{goto _LL30;}_LL30: if((int)_tmp8
== 1){goto _LL31;}else{goto _LL2D;}_LL2F: Cyc_Absyndump_dump(_tag_arr("struct",
sizeof(unsigned char),7));return;_LL31: Cyc_Absyndump_dump(_tag_arr("union",
sizeof(unsigned char),6));return;_LL2D:;}void Cyc_Absyndump_dumptps(struct Cyc_List_List*
ts){Cyc_Absyndump_egroup(Cyc_Absyndump_dumptyp,ts,_tag_arr("<",sizeof(
unsigned char),2),_tag_arr(">",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2));}void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);}void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);{void*_tmp9=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpA;void*
_tmpB;_LL33: if(*((int*)_tmp9)== 0){_LL3B: _tmpA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp9)->f1;if((int)_tmpA == 2){goto _LL34;}else{goto _LL35;}}else{goto _LL35;}_LL35:
if(*((int*)_tmp9)== 2){goto _LL36;}else{goto _LL37;}_LL37: if(*((int*)_tmp9)== 1){
goto _LL38;}else{goto _LL39;}_LL39: if(*((int*)_tmp9)== 0){_LL3C: _tmpB=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp9)->f1;goto _LL3A;}else{goto _LL32;}_LL34: goto
_LL36;_LL36: goto _LL38;_LL38: Cyc_Absyndump_dump(_tag_arr("::?",sizeof(
unsigned char),4));goto _LL32;_LL3A: Cyc_Absyndump_dump(_tag_arr("::",sizeof(
unsigned char),3));Cyc_Absyndump_dumpkind(_tmpB);goto _LL32;_LL32:;}}void Cyc_Absyndump_dumptvars(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,tvs,_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,tvs,_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}struct _tuple6{
struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(struct _tuple6*pr){((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple6*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumparg,ts,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dump_callconv(
struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmpC=(void*)atts->hd;
_LL3E: if((int)_tmpC == 0){goto _LL3F;}else{goto _LL40;}_LL40: if((int)_tmpC == 1){
goto _LL41;}else{goto _LL42;}_LL42: if((int)_tmpC == 2){goto _LL43;}else{goto _LL44;}
_LL44: goto _LL45;_LL3F: Cyc_Absyndump_dump(_tag_arr("_stdcall",sizeof(
unsigned char),9));return;_LL41: Cyc_Absyndump_dump(_tag_arr("_cdecl",sizeof(
unsigned char),7));return;_LL43: Cyc_Absyndump_dump(_tag_arr("_fastcall",sizeof(
unsigned char),10));return;_LL45: goto _LL3D;_LL3D:;}}void Cyc_Absyndump_dump_noncallconv(
struct Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;
atts2 != 0;atts2=atts2->tl){void*_tmpD=(void*)((struct Cyc_List_List*)_check_null(
atts))->hd;_LL47: if((int)_tmpD == 0){goto _LL48;}else{goto _LL49;}_LL49: if((int)
_tmpD == 1){goto _LL4A;}else{goto _LL4B;}_LL4B: if((int)_tmpD == 2){goto _LL4C;}else{
goto _LL4D;}_LL4D: goto _LL4E;_LL48: goto _LL4A;_LL4A: goto _LL4C;_LL4C: goto _LL46;_LL4E:
hasatt=1;goto _LL46;_LL46:;}}if(! hasatt){return;}Cyc_Absyndump_dump(_tag_arr("__declspec(",
sizeof(unsigned char),12));for(0;atts != 0;atts=atts->tl){void*_tmpE=(void*)atts->hd;
_LL50: if((int)_tmpE == 0){goto _LL51;}else{goto _LL52;}_LL52: if((int)_tmpE == 1){
goto _LL53;}else{goto _LL54;}_LL54: if((int)_tmpE == 2){goto _LL55;}else{goto _LL56;}
_LL56: goto _LL57;_LL51: goto _LL53;_LL53: goto _LL55;_LL55: goto _LL4F;_LL57: Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL4F;_LL4F:;}Cyc_Absyndump_dump_char((
int)')');}void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){if(atts == 0){
return;}if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dump_noncallconv(atts);return;}Cyc_Absyndump_dump(
_tag_arr(" __attribute__((",sizeof(unsigned char),17));for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));if(atts->tl != 0){
Cyc_Absyndump_dump(_tag_arr(",",sizeof(unsigned char),2));}}Cyc_Absyndump_dump(
_tag_arr(")) ",sizeof(unsigned char),4));}int Cyc_Absyndump_next_is_pointer(
struct Cyc_List_List*tms){if(tms == 0){return 0;}{void*_tmpF=(void*)tms->hd;_LL59:
if((unsigned int)_tmpF > 1?*((int*)_tmpF)== 1: 0){goto _LL5A;}else{goto _LL5B;}_LL5B:
goto _LL5C;_LL5A: return 1;_LL5C: return 0;_LL58:;}}static void Cyc_Absyndump_dumprgn(
void*t){void*_tmp10=Cyc_Tcutil_compress(t);_LL5E: if((int)_tmp10 == 2){goto _LL5F;}
else{goto _LL60;}_LL60: goto _LL61;_LL5F: Cyc_Absyndump_dump(_tag_arr("`H",sizeof(
unsigned char),3));goto _LL5D;_LL61: Cyc_Absyndump_dumpntyp(t);goto _LL5D;_LL5D:;}
struct _tuple7{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};static struct
_tuple7 Cyc_Absyndump_effects_split(void*t){struct Cyc_List_List*rgions=0;struct
Cyc_List_List*effects=0;{void*_tmp11=Cyc_Tcutil_compress(t);void*_tmp12;struct
Cyc_List_List*_tmp13;_LL63: if((unsigned int)_tmp11 > 3?*((int*)_tmp11)== 17: 0){
_LL69: _tmp12=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp11)->f1;goto _LL64;}
else{goto _LL65;}_LL65: if((unsigned int)_tmp11 > 3?*((int*)_tmp11)== 18: 0){_LL6A:
_tmp13=((struct Cyc_Absyn_JoinEff_struct*)_tmp11)->f1;goto _LL66;}else{goto _LL67;}
_LL67: goto _LL68;_LL64: rgions=({struct Cyc_List_List*_tmp14=_cycalloc(sizeof(*
_tmp14));_tmp14->hd=(void*)_tmp12;_tmp14->tl=rgions;_tmp14;});goto _LL62;_LL66:
for(0;_tmp13 != 0;_tmp13=_tmp13->tl){struct Cyc_List_List*_tmp16;struct Cyc_List_List*
_tmp17;struct _tuple7 _tmp15=Cyc_Absyndump_effects_split((void*)_tmp13->hd);_LL6D:
_tmp17=_tmp15.f1;goto _LL6C;_LL6C: _tmp16=_tmp15.f2;goto _LL6B;_LL6B: rgions=Cyc_List_imp_append(
_tmp17,rgions);effects=Cyc_List_imp_append(_tmp16,effects);}goto _LL62;_LL68:
effects=({struct Cyc_List_List*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->hd=(void*)
t;_tmp18->tl=effects;_tmp18;});goto _LL62;_LL62:;}return({struct _tuple7 _tmp19;
_tmp19.f1=rgions;_tmp19.f2=effects;_tmp19;});}static void Cyc_Absyndump_dumpeff(
void*t){struct Cyc_List_List*_tmp1B;struct Cyc_List_List*_tmp1C;struct _tuple7
_tmp1A=Cyc_Absyndump_effects_split(t);_LL70: _tmp1C=_tmp1A.f1;goto _LL6F;_LL6F:
_tmp1B=_tmp1A.f2;goto _LL6E;_LL6E: _tmp1C=Cyc_List_imp_rev(_tmp1C);_tmp1B=Cyc_List_imp_rev(
_tmp1B);for(0;_tmp1B != 0;_tmp1B=_tmp1B->tl){Cyc_Absyndump_dumpntyp((void*)_tmp1B->hd);
Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;
_tmp1C != 0;_tmp1C=_tmp1C->tl){Cyc_Absyndump_dumprgn((void*)_tmp1C->hd);if(_tmp1C->tl
!= 0){Cyc_Absyndump_dump_char((int)',');}}Cyc_Absyndump_dump_char((int)'}');}
void Cyc_Absyndump_dumpntyp(void*t){void*_tmp1D=t;struct Cyc_Absyn_Tvar*_tmp1E;int
_tmp1F;struct Cyc_Core_Opt*_tmp20;struct Cyc_Core_Opt*_tmp21;int _tmp22;struct Cyc_Core_Opt*
_tmp23;struct Cyc_Core_Opt _tmp24;void*_tmp25;struct Cyc_Core_Opt*_tmp26;struct Cyc_Absyn_TunionInfo
_tmp27;void*_tmp28;struct Cyc_List_List*_tmp29;void*_tmp2A;struct Cyc_Absyn_TunionFieldInfo
_tmp2B;struct Cyc_List_List*_tmp2C;void*_tmp2D;void*_tmp2E;void*_tmp2F;void*
_tmp30;void*_tmp31;void*_tmp32;void*_tmp33;void*_tmp34;void*_tmp35;void*_tmp36;
void*_tmp37;void*_tmp38;void*_tmp39;void*_tmp3A;void*_tmp3B;void*_tmp3C;void*
_tmp3D;void*_tmp3E;void*_tmp3F;void*_tmp40;void*_tmp41;int _tmp42;struct Cyc_List_List*
_tmp43;struct Cyc_Absyn_AggrInfo _tmp44;struct Cyc_List_List*_tmp45;void*_tmp46;
struct Cyc_List_List*_tmp47;void*_tmp48;struct _tuple0*_tmp49;struct Cyc_List_List*
_tmp4A;struct Cyc_Core_Opt*_tmp4B;struct Cyc_List_List*_tmp4C;struct _tuple0*_tmp4D;
void*_tmp4E;void*_tmp4F;_LL72: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 7: 0){
goto _LL73;}else{goto _LL74;}_LL74: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 8:
0){goto _LL75;}else{goto _LL76;}_LL76: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 
4: 0){goto _LL77;}else{goto _LL78;}_LL78: if((int)_tmp1D == 0){goto _LL79;}else{goto
_LL7A;}_LL7A: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 1: 0){_LLB4: _tmp1E=((
struct Cyc_Absyn_VarType_struct*)_tmp1D)->f1;goto _LL7B;}else{goto _LL7C;}_LL7C: if((
unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 0: 0){_LLB7: _tmp21=((struct Cyc_Absyn_Evar_struct*)
_tmp1D)->f1;goto _LLB6;_LLB6: _tmp20=((struct Cyc_Absyn_Evar_struct*)_tmp1D)->f2;
if(_tmp20 == 0){goto _LLB5;}else{goto _LL7E;}_LLB5: _tmp1F=((struct Cyc_Absyn_Evar_struct*)
_tmp1D)->f3;goto _LL7D;}else{goto _LL7E;}_LL7E: if((unsigned int)_tmp1D > 3?*((int*)
_tmp1D)== 0: 0){_LLBB: _tmp26=((struct Cyc_Absyn_Evar_struct*)_tmp1D)->f1;goto _LLB9;
_LLB9: _tmp23=((struct Cyc_Absyn_Evar_struct*)_tmp1D)->f2;if(_tmp23 == 0){goto _LL80;}
else{_tmp24=*_tmp23;_LLBA: _tmp25=(void*)_tmp24.v;goto _LLB8;}_LLB8: _tmp22=((
struct Cyc_Absyn_Evar_struct*)_tmp1D)->f3;goto _LL7F;}else{goto _LL80;}_LL80: if((
unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 2: 0){_LLBC: _tmp27=((struct Cyc_Absyn_TunionType_struct*)
_tmp1D)->f1;_LLBF: _tmp2A=(void*)_tmp27.tunion_info;goto _LLBE;_LLBE: _tmp29=_tmp27.targs;
goto _LLBD;_LLBD: _tmp28=(void*)_tmp27.rgn;goto _LL81;}else{goto _LL82;}_LL82: if((
unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 3: 0){_LLC0: _tmp2B=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1D)->f1;_LLC2: _tmp2D=(void*)_tmp2B.field_info;goto _LLC1;_LLC1: _tmp2C=_tmp2B.targs;
goto _LL83;}else{goto _LL84;}_LL84: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 5:
0){_LLC4: _tmp2F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)
_tmp2F == 0){goto _LLC3;}else{goto _LL86;}_LLC3: _tmp2E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp2E == 2){goto _LL85;}else{goto _LL86;}}else{goto _LL86;}
_LL86: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 5: 0){_LLC6: _tmp31=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp31 == 0){goto _LLC5;}else{
goto _LL88;}_LLC5: _tmp30=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp30 == 0){goto _LL87;}else{goto _LL88;}}else{goto _LL88;}_LL88: if((
unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 5: 0){_LLC8: _tmp33=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f1;if((int)_tmp33 == 1){goto _LLC7;}else{goto _LL8A;}_LLC7: _tmp32=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((int)_tmp32 == 0){goto _LL89;}else{
goto _LL8A;}}else{goto _LL8A;}_LL8A: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 5:
0){_LLCA: _tmp35=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)
_tmp35 == 0){goto _LLC9;}else{goto _LL8C;}_LLC9: _tmp34=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp34 == 1){goto _LL8B;}else{goto _LL8C;}}else{goto _LL8C;}
_LL8C: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 5: 0){_LLCC: _tmp37=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp37 == 1){goto _LLCB;}else{
goto _LL8E;}_LLCB: _tmp36=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp36 == 1){goto _LL8D;}else{goto _LL8E;}}else{goto _LL8E;}_LL8E: if((
unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 5: 0){_LLCE: _tmp39=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f1;if((int)_tmp39 == 1){goto _LLCD;}else{goto _LL90;}_LLCD: _tmp38=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((int)_tmp38 == 2){goto _LL8F;}else{
goto _LL90;}}else{goto _LL90;}_LL90: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 5:
0){_LLD1: _tmp3B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)
_tmp3B == 0){goto _LLD0;}else{goto _LL92;}_LLD0: _tmp3A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp3A == 3){goto _LLCF;}else{goto _LL92;}}else{goto _LL92;}
_LLCF: if(Cyc_Absyndump_to_VC){goto _LL91;}else{goto _LL92;}_LL92: if((unsigned int)
_tmp1D > 3?*((int*)_tmp1D)== 5: 0){_LLD3: _tmp3D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f1;if((int)_tmp3D == 0){goto _LLD2;}else{goto _LL94;}_LLD2: _tmp3C=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((int)_tmp3C == 3){goto _LL93;}else{
goto _LL94;}}else{goto _LL94;}_LL94: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 5:
0){_LLD6: _tmp3F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)
_tmp3F == 1){goto _LLD5;}else{goto _LL96;}_LLD5: _tmp3E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp3E == 3){goto _LLD4;}else{goto _LL96;}}else{goto _LL96;}
_LLD4: if(Cyc_Absyndump_to_VC){goto _LL95;}else{goto _LL96;}_LL96: if((unsigned int)
_tmp1D > 3?*((int*)_tmp1D)== 5: 0){_LLD8: _tmp41=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f1;if((int)_tmp41 == 1){goto _LLD7;}else{goto _LL98;}_LLD7: _tmp40=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((int)_tmp40 == 3){goto _LL97;}else{
goto _LL98;}}else{goto _LL98;}_LL98: if((int)_tmp1D == 1){goto _LL99;}else{goto _LL9A;}
_LL9A: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 6: 0){_LLD9: _tmp42=((struct Cyc_Absyn_DoubleType_struct*)
_tmp1D)->f1;goto _LL9B;}else{goto _LL9C;}_LL9C: if((unsigned int)_tmp1D > 3?*((int*)
_tmp1D)== 9: 0){_LLDA: _tmp43=((struct Cyc_Absyn_TupleType_struct*)_tmp1D)->f1;goto
_LL9D;}else{goto _LL9E;}_LL9E: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 10: 0){
_LLDB: _tmp44=((struct Cyc_Absyn_AggrType_struct*)_tmp1D)->f1;_LLDD: _tmp46=(void*)
_tmp44.aggr_info;goto _LLDC;_LLDC: _tmp45=_tmp44.targs;goto _LL9F;}else{goto _LLA0;}
_LLA0: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 11: 0){_LLDF: _tmp48=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp1D)->f1;goto _LLDE;_LLDE: _tmp47=((struct
Cyc_Absyn_AnonAggrType_struct*)_tmp1D)->f2;goto _LLA1;}else{goto _LLA2;}_LLA2: if((
unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 12: 0){_LLE0: _tmp49=((struct Cyc_Absyn_EnumType_struct*)
_tmp1D)->f1;goto _LLA3;}else{goto _LLA4;}_LLA4: if((unsigned int)_tmp1D > 3?*((int*)
_tmp1D)== 13: 0){_LLE1: _tmp4A=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp1D)->f1;
goto _LLA5;}else{goto _LLA6;}_LLA6: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 16:
0){_LLE4: _tmp4D=((struct Cyc_Absyn_TypedefType_struct*)_tmp1D)->f1;goto _LLE3;
_LLE3: _tmp4C=((struct Cyc_Absyn_TypedefType_struct*)_tmp1D)->f2;goto _LLE2;_LLE2:
_tmp4B=((struct Cyc_Absyn_TypedefType_struct*)_tmp1D)->f3;goto _LLA7;}else{goto
_LLA8;}_LLA8: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 14: 0){_LLE5: _tmp4E=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp1D)->f1;goto _LLA9;}else{goto _LLAA;}
_LLAA: if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 15: 0){_LLE6: _tmp4F=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp1D)->f1;goto _LLAB;}else{goto _LLAC;}
_LLAC: if((int)_tmp1D == 2){goto _LLAD;}else{goto _LLAE;}_LLAE: if((unsigned int)
_tmp1D > 3?*((int*)_tmp1D)== 17: 0){goto _LLAF;}else{goto _LLB0;}_LLB0: if((
unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 19: 0){goto _LLB1;}else{goto _LLB2;}_LLB2:
if((unsigned int)_tmp1D > 3?*((int*)_tmp1D)== 18: 0){goto _LLB3;}else{goto _LL71;}
_LL73: goto _LL75;_LL75: goto _LL77;_LL77: return;_LL79: Cyc_Absyndump_dump(_tag_arr("void",
sizeof(unsigned char),5));return;_LL7B: Cyc_Absyndump_dump_str(_tmp1E->name);
return;_LL7D: Cyc_Absyndump_dump(_tag_arr("%",sizeof(unsigned char),2));if(_tmp21
== 0){Cyc_Absyndump_dump(_tag_arr("?",sizeof(unsigned char),2));}else{Cyc_Absyndump_dumpkind((
void*)_tmp21->v);}Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp51;_tmp51.tag=1;_tmp51.f1=(int)((unsigned int)_tmp1F);{void*_tmp50[1]={&
_tmp51};Cyc_Std_aprintf(_tag_arr("(%d)",sizeof(unsigned char),5),_tag_arr(_tmp50,
sizeof(void*),1));}}));return;_LL7F: Cyc_Absyndump_dumpntyp(_tmp25);return;_LL81:{
void*_tmp52=_tmp2A;struct Cyc_Absyn_UnknownTunionInfo _tmp53;int _tmp54;struct
_tuple0*_tmp55;struct Cyc_Absyn_Tuniondecl**_tmp56;struct Cyc_Absyn_Tuniondecl*
_tmp57;struct Cyc_Absyn_Tuniondecl _tmp58;int _tmp59;struct _tuple0*_tmp5A;_LLE8: if(*((
int*)_tmp52)== 0){_LLEC: _tmp53=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp52)->f1;
_LLEE: _tmp55=_tmp53.name;goto _LLED;_LLED: _tmp54=_tmp53.is_xtunion;goto _LLE9;}
else{goto _LLEA;}_LLEA: if(*((int*)_tmp52)== 1){_LLEF: _tmp56=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp52)->f1;_tmp57=*_tmp56;_tmp58=*_tmp57;_LLF1: _tmp5A=_tmp58.name;goto _LLF0;
_LLF0: _tmp59=_tmp58.is_xtunion;goto _LLEB;}else{goto _LLE7;}_LLE9: _tmp5A=_tmp55;
_tmp59=_tmp54;goto _LLEB;_LLEB: if(_tmp59){Cyc_Absyndump_dump(_tag_arr("xtunion ",
sizeof(unsigned char),9));}else{Cyc_Absyndump_dump(_tag_arr("tunion ",sizeof(
unsigned char),8));}{void*_tmp5B=Cyc_Tcutil_compress(_tmp28);_LLF3: if((int)
_tmp5B == 2){goto _LLF4;}else{goto _LLF5;}_LLF5: goto _LLF6;_LLF4: goto _LLF2;_LLF6: Cyc_Absyndump_dumptyp(
_tmp28);Cyc_Absyndump_dump(_tag_arr(" ",sizeof(unsigned char),2));goto _LLF2;
_LLF2:;}Cyc_Absyndump_dumpqvar(_tmp5A);Cyc_Absyndump_dumptps(_tmp29);goto _LLE7;
_LLE7:;}goto _LL71;_LL83:{void*_tmp5C=_tmp2D;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp5D;int _tmp5E;struct _tuple0*_tmp5F;struct _tuple0*_tmp60;struct Cyc_Absyn_Tunionfield*
_tmp61;struct Cyc_Absyn_Tunionfield _tmp62;struct _tuple0*_tmp63;struct Cyc_Absyn_Tuniondecl*
_tmp64;struct Cyc_Absyn_Tuniondecl _tmp65;int _tmp66;struct _tuple0*_tmp67;_LLF8: if(*((
int*)_tmp5C)== 0){_LLFC: _tmp5D=((struct Cyc_Absyn_UnknownTunionfield_struct*)
_tmp5C)->f1;_LLFF: _tmp60=_tmp5D.tunion_name;goto _LLFE;_LLFE: _tmp5F=_tmp5D.field_name;
goto _LLFD;_LLFD: _tmp5E=_tmp5D.is_xtunion;goto _LLF9;}else{goto _LLFA;}_LLFA: if(*((
int*)_tmp5C)== 1){_LL102: _tmp64=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp5C)->f1;_tmp65=*_tmp64;_LL104: _tmp67=_tmp65.name;goto _LL103;_LL103: _tmp66=
_tmp65.is_xtunion;goto _LL100;_LL100: _tmp61=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp5C)->f2;_tmp62=*_tmp61;_LL101: _tmp63=_tmp62.name;goto _LLFB;}else{goto _LLF7;}
_LLF9: _tmp67=_tmp60;_tmp66=_tmp5E;_tmp63=_tmp5F;goto _LLFB;_LLFB: if(_tmp66){Cyc_Absyndump_dump(
_tag_arr("xtunion ",sizeof(unsigned char),9));}else{Cyc_Absyndump_dump(_tag_arr("tunion ",
sizeof(unsigned char),8));}Cyc_Absyndump_dumpqvar(_tmp67);Cyc_Absyndump_dump(
_tag_arr(".",sizeof(unsigned char),2));Cyc_Absyndump_dumpqvar(_tmp63);Cyc_Absyndump_dumptps(
_tmp2C);goto _LLF7;_LLF7:;}goto _LL71;_LL85: Cyc_Absyndump_dump(_tag_arr("int",
sizeof(unsigned char),4));return;_LL87: Cyc_Absyndump_dump(_tag_arr("signed char",
sizeof(unsigned char),12));return;_LL89: Cyc_Absyndump_dump(_tag_arr("unsigned char",
sizeof(unsigned char),14));return;_LL8B: Cyc_Absyndump_dump(_tag_arr("short",
sizeof(unsigned char),6));return;_LL8D: Cyc_Absyndump_dump(_tag_arr("unsigned short",
sizeof(unsigned char),15));return;_LL8F: Cyc_Absyndump_dump(_tag_arr("unsigned int",
sizeof(unsigned char),13));return;_LL91: Cyc_Absyndump_dump(_tag_arr("__int64",
sizeof(unsigned char),8));return;_LL93: Cyc_Absyndump_dump(_tag_arr("long long",
sizeof(unsigned char),10));return;_LL95: Cyc_Absyndump_dump(_tag_arr("unsigned __int64",
sizeof(unsigned char),17));return;_LL97: Cyc_Absyndump_dump(_tag_arr("unsigned long long",
sizeof(unsigned char),19));return;_LL99: Cyc_Absyndump_dump(_tag_arr("float",
sizeof(unsigned char),6));return;_LL9B: if(_tmp42){Cyc_Absyndump_dump(_tag_arr("long double",
sizeof(unsigned char),12));}else{Cyc_Absyndump_dump(_tag_arr("double",sizeof(
unsigned char),7));}return;_LL9D: Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(
_tmp43);return;_LL9F: {struct _tuple0*_tmp69;void*_tmp6A;struct _tuple3 _tmp68=Cyc_Absyn_aggr_kinded_name(
_tmp46);_LL107: _tmp6A=_tmp68.f1;goto _LL106;_LL106: _tmp69=_tmp68.f2;goto _LL105;
_LL105: Cyc_Absyndump_dumpaggr_kind(_tmp6A);Cyc_Absyndump_dumpqvar(_tmp69);Cyc_Absyndump_dumptps(
_tmp45);return;}_LLA1: Cyc_Absyndump_dumpaggr_kind(_tmp48);Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpaggrfields(_tmp47);Cyc_Absyndump_dump_char((int)'}');
return;_LLA3: Cyc_Absyndump_dump(_tag_arr("enum ",sizeof(unsigned char),6));Cyc_Absyndump_dumpqvar(
_tmp49);return;_LLA5: Cyc_Absyndump_dump(_tag_arr("enum {",sizeof(unsigned char),
7));Cyc_Absyndump_dumpenumfields(_tmp4A);Cyc_Absyndump_dump(_tag_arr("}",sizeof(
unsigned char),2));return;_LLA7:(Cyc_Absyndump_dumpqvar(_tmp4D),Cyc_Absyndump_dumptps(
_tmp4C));return;_LLA9: Cyc_Absyndump_dump(_tag_arr("sizeof_t<",sizeof(
unsigned char),10));Cyc_Absyndump_dumpntyp(_tmp4E);Cyc_Absyndump_dump(_tag_arr(">",
sizeof(unsigned char),2));return;_LLAB: Cyc_Absyndump_dumprgn(_tmp4F);return;
_LLAD: goto _LLAF;_LLAF: goto _LLB1;_LLB1: goto _LLB3;_LLB3: return;_LL71:;}void Cyc_Absyndump_dumpvaropt(
struct Cyc_Core_Opt*vo){if(vo != 0){Cyc_Absyndump_dump_str((struct _tagged_arr*)vo->v);}}
void Cyc_Absyndump_dumpfunarg(struct _tuple1*t){((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct Cyc_Core_Opt*),struct Cyc_Core_Opt*))Cyc_Absyndump_dumptqtd)((*
t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple8{void*f1;void*f2;}
;void Cyc_Absyndump_dump_rgncmp(struct _tuple8*cmp){struct _tuple8 _tmp6C;void*
_tmp6D;void*_tmp6E;struct _tuple8*_tmp6B=cmp;_tmp6C=*_tmp6B;_LL10A: _tmp6E=_tmp6C.f1;
goto _LL109;_LL109: _tmp6D=_tmp6C.f2;goto _LL108;_LL108: Cyc_Absyndump_dumptyp(
_tmp6E);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptyp(_tmp6D);}void Cyc_Absyndump_dump_rgnpo(
struct Cyc_List_List*rgn_po){((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*
l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,rgn_po,
_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((int)'(');for(0;args
!= 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple1*)args->hd);if((args->tl
!= 0? 1: c_varargs)? 1: cyc_varargs != 0){Cyc_Absyndump_dump_char((int)',');}}if(
c_varargs){Cyc_Absyndump_dump(_tag_arr("...",sizeof(unsigned char),4));}else{if(
cyc_varargs != 0){struct _tuple1*_tmp6F=({struct _tuple1*_tmp70=_cycalloc(sizeof(*
_tmp70));_tmp70->f1=cyc_varargs->name;_tmp70->f2=cyc_varargs->tq;_tmp70->f3=(
void*)cyc_varargs->type;_tmp70;});Cyc_Absyndump_dump(_tag_arr("...",sizeof(
unsigned char),4));if(cyc_varargs->inject){Cyc_Absyndump_dump(_tag_arr(" inject ",
sizeof(unsigned char),9));}Cyc_Absyndump_dumpfunarg(_tmp6F);}}if(effopt != 0){Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff((void*)effopt->v);}if(rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}Cyc_Absyndump_dump_char((int)')');}
void Cyc_Absyndump_dumptyp(void*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
int),int))Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp71;_tmp71.q_const=0;
_tmp71.q_volatile=0;_tmp71.q_restrict=0;_tmp71;}),t,(void(*)(int x))Cyc_Absyndump_ignore,
0);}void Cyc_Absyndump_dumpdesignator(void*d){void*_tmp72=d;struct Cyc_Absyn_Exp*
_tmp73;struct _tagged_arr*_tmp74;_LL10C: if(*((int*)_tmp72)== 0){_LL110: _tmp73=((
struct Cyc_Absyn_ArrayElement_struct*)_tmp72)->f1;goto _LL10D;}else{goto _LL10E;}
_LL10E: if(*((int*)_tmp72)== 1){_LL111: _tmp74=((struct Cyc_Absyn_FieldName_struct*)
_tmp72)->f1;goto _LL10F;}else{goto _LL10B;}_LL10D: Cyc_Absyndump_dump(_tag_arr(".[",
sizeof(unsigned char),3));Cyc_Absyndump_dumpexp(_tmp73);Cyc_Absyndump_dump_char((
int)']');goto _LL10B;_LL10F: Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp74);goto _LL10B;_LL10B:;}struct _tuple9{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};void Cyc_Absyndump_dumpde(struct _tuple9*de){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*
de).f1,_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",sizeof(unsigned char),2),
_tag_arr("=",sizeof(unsigned char),2));Cyc_Absyndump_dumpexp((*de).f2);}void Cyc_Absyndump_dumpexps_prec(
int inprec,struct Cyc_List_List*es){((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),
int env,struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,inprec,es,
_tag_arr("",sizeof(unsigned char),1),_tag_arr("",sizeof(unsigned char),1),
_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpexp_prec(int inprec,
struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= myprec){Cyc_Absyndump_dump_nospace(
_tag_arr("(",sizeof(unsigned char),2));}{void*_tmp75=(void*)e->r;void*_tmp76;
unsigned char _tmp77;void*_tmp78;void*_tmp79;short _tmp7A;void*_tmp7B;void*_tmp7C;
int _tmp7D;void*_tmp7E;void*_tmp7F;int _tmp80;void*_tmp81;void*_tmp82;long long
_tmp83;void*_tmp84;void*_tmp85;struct _tagged_arr _tmp86;void*_tmp87;void*_tmp88;
struct _tagged_arr _tmp89;struct _tuple0*_tmp8A;struct _tuple0*_tmp8B;struct Cyc_List_List*
_tmp8C;void*_tmp8D;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Core_Opt*_tmp8F;struct
Cyc_Absyn_Exp*_tmp90;void*_tmp91;struct Cyc_Absyn_Exp*_tmp92;void*_tmp93;struct
Cyc_Absyn_Exp*_tmp94;void*_tmp95;struct Cyc_Absyn_Exp*_tmp96;void*_tmp97;struct
Cyc_Absyn_Exp*_tmp98;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Exp*_tmp9A;
struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9D;
struct Cyc_List_List*_tmp9E;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_List_List*_tmpA0;
struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Exp*_tmpA3;
struct Cyc_Absyn_Exp*_tmpA4;struct Cyc_Absyn_Exp*_tmpA5;void*_tmpA6;struct Cyc_Absyn_Exp*
_tmpA7;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Exp*_tmpA9;void*_tmpAA;struct
Cyc_Absyn_Exp*_tmpAB;void*_tmpAC;struct _tagged_arr*_tmpAD;void*_tmpAE;void*
_tmpAF;unsigned int _tmpB0;void*_tmpB1;void*_tmpB2;struct Cyc_List_List*_tmpB3;
struct Cyc_Absyn_Exp*_tmpB4;struct _tagged_arr*_tmpB5;struct Cyc_Absyn_Exp*_tmpB6;
struct _tagged_arr*_tmpB7;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_Absyn_Exp*_tmpB9;
struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_List_List*_tmpBB;struct Cyc_List_List*_tmpBC;
struct _tuple1*_tmpBD;struct Cyc_List_List*_tmpBE;struct Cyc_Absyn_Exp*_tmpBF;
struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_Absyn_Vardecl*_tmpC1;struct Cyc_List_List*
_tmpC2;struct _tuple0*_tmpC3;struct Cyc_List_List*_tmpC4;struct Cyc_Absyn_Tunionfield*
_tmpC5;struct Cyc_List_List*_tmpC6;struct _tuple0*_tmpC7;struct _tuple0*_tmpC8;
struct Cyc_Absyn_MallocInfo _tmpC9;struct Cyc_Absyn_Exp*_tmpCA;void**_tmpCB;struct
Cyc_Absyn_Exp*_tmpCC;int _tmpCD;struct Cyc_List_List*_tmpCE;struct Cyc_Core_Opt*
_tmpCF;struct Cyc_Absyn_Stmt*_tmpD0;struct Cyc_Absyn_Fndecl*_tmpD1;struct Cyc_Absyn_Exp*
_tmpD2;_LL113: if(*((int*)_tmp75)== 0){_LL175: _tmp76=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp75)->f1;if((unsigned int)_tmp76 > 1?*((int*)_tmp76)== 0: 0){_LL177: _tmp78=(
void*)((struct Cyc_Absyn_Char_c_struct*)_tmp76)->f1;goto _LL176;_LL176: _tmp77=((
struct Cyc_Absyn_Char_c_struct*)_tmp76)->f2;goto _LL114;}else{goto _LL115;}}else{
goto _LL115;}_LL115: if(*((int*)_tmp75)== 0){_LL178: _tmp79=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp75)->f1;if((unsigned int)_tmp79 > 1?*((int*)_tmp79)== 1: 0){_LL17A: _tmp7B=(
void*)((struct Cyc_Absyn_Short_c_struct*)_tmp79)->f1;goto _LL179;_LL179: _tmp7A=((
struct Cyc_Absyn_Short_c_struct*)_tmp79)->f2;goto _LL116;}else{goto _LL117;}}else{
goto _LL117;}_LL117: if(*((int*)_tmp75)== 0){_LL17B: _tmp7C=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp75)->f1;if((unsigned int)_tmp7C > 1?*((int*)_tmp7C)== 2: 0){_LL17D: _tmp7E=(
void*)((struct Cyc_Absyn_Int_c_struct*)_tmp7C)->f1;if((int)_tmp7E == 0){goto _LL17C;}
else{goto _LL119;}_LL17C: _tmp7D=((struct Cyc_Absyn_Int_c_struct*)_tmp7C)->f2;goto
_LL118;}else{goto _LL119;}}else{goto _LL119;}_LL119: if(*((int*)_tmp75)== 0){_LL17E:
_tmp7F=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp75)->f1;if((unsigned int)
_tmp7F > 1?*((int*)_tmp7F)== 2: 0){_LL180: _tmp81=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp7F)->f1;if((int)_tmp81 == 1){goto _LL17F;}else{goto _LL11B;}_LL17F: _tmp80=((
struct Cyc_Absyn_Int_c_struct*)_tmp7F)->f2;goto _LL11A;}else{goto _LL11B;}}else{
goto _LL11B;}_LL11B: if(*((int*)_tmp75)== 0){_LL181: _tmp82=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp75)->f1;if((unsigned int)_tmp82 > 1?*((int*)_tmp82)== 3: 0){_LL183: _tmp84=(
void*)((struct Cyc_Absyn_LongLong_c_struct*)_tmp82)->f1;goto _LL182;_LL182: _tmp83=((
struct Cyc_Absyn_LongLong_c_struct*)_tmp82)->f2;goto _LL11C;}else{goto _LL11D;}}
else{goto _LL11D;}_LL11D: if(*((int*)_tmp75)== 0){_LL184: _tmp85=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp75)->f1;if((unsigned int)_tmp85 > 1?*((int*)_tmp85)== 4: 0){_LL185: _tmp86=((
struct Cyc_Absyn_Float_c_struct*)_tmp85)->f1;goto _LL11E;}else{goto _LL11F;}}else{
goto _LL11F;}_LL11F: if(*((int*)_tmp75)== 0){_LL186: _tmp87=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp75)->f1;if((int)_tmp87 == 0){goto _LL120;}else{goto _LL121;}}else{goto _LL121;}
_LL121: if(*((int*)_tmp75)== 0){_LL187: _tmp88=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp75)->f1;if((unsigned int)_tmp88 > 1?*((int*)_tmp88)== 5: 0){_LL188: _tmp89=((
struct Cyc_Absyn_String_c_struct*)_tmp88)->f1;goto _LL122;}else{goto _LL123;}}else{
goto _LL123;}_LL123: if(*((int*)_tmp75)== 2){_LL189: _tmp8A=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp75)->f1;goto _LL124;}else{goto _LL125;}_LL125: if(*((int*)_tmp75)== 1){_LL18A:
_tmp8B=((struct Cyc_Absyn_Var_e_struct*)_tmp75)->f1;goto _LL126;}else{goto _LL127;}
_LL127: if(*((int*)_tmp75)== 3){_LL18C: _tmp8D=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp75)->f1;goto _LL18B;_LL18B: _tmp8C=((struct Cyc_Absyn_Primop_e_struct*)_tmp75)->f2;
goto _LL128;}else{goto _LL129;}_LL129: if(*((int*)_tmp75)== 4){_LL18F: _tmp90=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp75)->f1;goto _LL18E;_LL18E: _tmp8F=((struct
Cyc_Absyn_AssignOp_e_struct*)_tmp75)->f2;goto _LL18D;_LL18D: _tmp8E=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp75)->f3;goto _LL12A;}else{goto _LL12B;}_LL12B: if(*((int*)_tmp75)== 5){_LL191:
_tmp92=((struct Cyc_Absyn_Increment_e_struct*)_tmp75)->f1;goto _LL190;_LL190:
_tmp91=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp75)->f2;if((int)_tmp91 == 
0){goto _LL12C;}else{goto _LL12D;}}else{goto _LL12D;}_LL12D: if(*((int*)_tmp75)== 5){
_LL193: _tmp94=((struct Cyc_Absyn_Increment_e_struct*)_tmp75)->f1;goto _LL192;
_LL192: _tmp93=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp75)->f2;if((int)
_tmp93 == 2){goto _LL12E;}else{goto _LL12F;}}else{goto _LL12F;}_LL12F: if(*((int*)
_tmp75)== 5){_LL195: _tmp96=((struct Cyc_Absyn_Increment_e_struct*)_tmp75)->f1;
goto _LL194;_LL194: _tmp95=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp75)->f2;
if((int)_tmp95 == 1){goto _LL130;}else{goto _LL131;}}else{goto _LL131;}_LL131: if(*((
int*)_tmp75)== 5){_LL197: _tmp98=((struct Cyc_Absyn_Increment_e_struct*)_tmp75)->f1;
goto _LL196;_LL196: _tmp97=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp75)->f2;
if((int)_tmp97 == 3){goto _LL132;}else{goto _LL133;}}else{goto _LL133;}_LL133: if(*((
int*)_tmp75)== 6){_LL19A: _tmp9B=((struct Cyc_Absyn_Conditional_e_struct*)_tmp75)->f1;
goto _LL199;_LL199: _tmp9A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp75)->f2;
goto _LL198;_LL198: _tmp99=((struct Cyc_Absyn_Conditional_e_struct*)_tmp75)->f3;
goto _LL134;}else{goto _LL135;}_LL135: if(*((int*)_tmp75)== 7){_LL19C: _tmp9D=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp75)->f1;goto _LL19B;_LL19B: _tmp9C=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp75)->f2;goto _LL136;}else{goto _LL137;}_LL137: if(*((
int*)_tmp75)== 8){_LL19E: _tmp9F=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp75)->f1;
goto _LL19D;_LL19D: _tmp9E=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp75)->f2;
goto _LL138;}else{goto _LL139;}_LL139: if(*((int*)_tmp75)== 9){_LL1A0: _tmpA1=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp75)->f1;goto _LL19F;_LL19F: _tmpA0=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp75)->f2;goto _LL13A;}else{goto _LL13B;}_LL13B: if(*((
int*)_tmp75)== 10){_LL1A1: _tmpA2=((struct Cyc_Absyn_Throw_e_struct*)_tmp75)->f1;
goto _LL13C;}else{goto _LL13D;}_LL13D: if(*((int*)_tmp75)== 11){_LL1A2: _tmpA3=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp75)->f1;goto _LL13E;}else{goto _LL13F;}
_LL13F: if(*((int*)_tmp75)== 12){_LL1A3: _tmpA4=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp75)->f1;goto _LL140;}else{goto _LL141;}_LL141: if(*((int*)_tmp75)== 13){_LL1A5:
_tmpA6=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp75)->f1;goto _LL1A4;_LL1A4:
_tmpA5=((struct Cyc_Absyn_Cast_e_struct*)_tmp75)->f2;goto _LL142;}else{goto _LL143;}
_LL143: if(*((int*)_tmp75)== 14){_LL1A6: _tmpA7=((struct Cyc_Absyn_Address_e_struct*)
_tmp75)->f1;goto _LL144;}else{goto _LL145;}_LL145: if(*((int*)_tmp75)== 15){_LL1A8:
_tmpA9=((struct Cyc_Absyn_New_e_struct*)_tmp75)->f1;goto _LL1A7;_LL1A7: _tmpA8=((
struct Cyc_Absyn_New_e_struct*)_tmp75)->f2;goto _LL146;}else{goto _LL147;}_LL147:
if(*((int*)_tmp75)== 16){_LL1A9: _tmpAA=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp75)->f1;goto _LL148;}else{goto _LL149;}_LL149: if(*((int*)_tmp75)== 17){_LL1AA:
_tmpAB=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp75)->f1;goto _LL14A;}else{goto
_LL14B;}_LL14B: if(*((int*)_tmp75)== 18){_LL1AD: _tmpAE=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp75)->f1;goto _LL1AB;_LL1AB: _tmpAC=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp75)->f2;if(*((int*)_tmpAC)== 0){_LL1AC: _tmpAD=((struct Cyc_Absyn_StructField_struct*)
_tmpAC)->f1;goto _LL14C;}else{goto _LL14D;}}else{goto _LL14D;}_LL14D: if(*((int*)
_tmp75)== 18){_LL1B0: _tmpB1=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp75)->f1;
goto _LL1AE;_LL1AE: _tmpAF=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp75)->f2;
if(*((int*)_tmpAF)== 1){_LL1AF: _tmpB0=((struct Cyc_Absyn_TupleIndex_struct*)
_tmpAF)->f1;goto _LL14E;}else{goto _LL14F;}}else{goto _LL14F;}_LL14F: if(*((int*)
_tmp75)== 19){_LL1B2: _tmpB3=((struct Cyc_Absyn_Gentyp_e_struct*)_tmp75)->f1;goto
_LL1B1;_LL1B1: _tmpB2=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp75)->f2;goto
_LL150;}else{goto _LL151;}_LL151: if(*((int*)_tmp75)== 20){_LL1B3: _tmpB4=((struct
Cyc_Absyn_Deref_e_struct*)_tmp75)->f1;goto _LL152;}else{goto _LL153;}_LL153: if(*((
int*)_tmp75)== 21){_LL1B5: _tmpB6=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp75)->f1;
goto _LL1B4;_LL1B4: _tmpB5=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp75)->f2;goto
_LL154;}else{goto _LL155;}_LL155: if(*((int*)_tmp75)== 22){_LL1B7: _tmpB8=((struct
Cyc_Absyn_AggrArrow_e_struct*)_tmp75)->f1;goto _LL1B6;_LL1B6: _tmpB7=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp75)->f2;goto _LL156;}else{goto _LL157;}_LL157: if(*((int*)_tmp75)== 23){_LL1B9:
_tmpBA=((struct Cyc_Absyn_Subscript_e_struct*)_tmp75)->f1;goto _LL1B8;_LL1B8:
_tmpB9=((struct Cyc_Absyn_Subscript_e_struct*)_tmp75)->f2;goto _LL158;}else{goto
_LL159;}_LL159: if(*((int*)_tmp75)== 24){_LL1BA: _tmpBB=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp75)->f1;goto _LL15A;}else{goto _LL15B;}_LL15B: if(*((int*)_tmp75)== 25){_LL1BC:
_tmpBD=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp75)->f1;goto _LL1BB;_LL1BB:
_tmpBC=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp75)->f2;goto _LL15C;}else{goto
_LL15D;}_LL15D: if(*((int*)_tmp75)== 26){_LL1BD: _tmpBE=((struct Cyc_Absyn_Array_e_struct*)
_tmp75)->f1;goto _LL15E;}else{goto _LL15F;}_LL15F: if(*((int*)_tmp75)== 27){_LL1C0:
_tmpC1=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp75)->f1;goto _LL1BF;_LL1BF:
_tmpC0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp75)->f2;goto _LL1BE;_LL1BE:
_tmpBF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp75)->f3;goto _LL160;}else{
goto _LL161;}_LL161: if(*((int*)_tmp75)== 28){_LL1C2: _tmpC3=((struct Cyc_Absyn_Struct_e_struct*)
_tmp75)->f1;goto _LL1C1;_LL1C1: _tmpC2=((struct Cyc_Absyn_Struct_e_struct*)_tmp75)->f2;
goto _LL162;}else{goto _LL163;}_LL163: if(*((int*)_tmp75)== 29){_LL1C3: _tmpC4=((
struct Cyc_Absyn_AnonStruct_e_struct*)_tmp75)->f2;goto _LL164;}else{goto _LL165;}
_LL165: if(*((int*)_tmp75)== 30){_LL1C5: _tmpC6=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp75)->f1;goto _LL1C4;_LL1C4: _tmpC5=((struct Cyc_Absyn_Tunion_e_struct*)_tmp75)->f3;
goto _LL166;}else{goto _LL167;}_LL167: if(*((int*)_tmp75)== 31){_LL1C6: _tmpC7=((
struct Cyc_Absyn_Enum_e_struct*)_tmp75)->f1;goto _LL168;}else{goto _LL169;}_LL169:
if(*((int*)_tmp75)== 32){_LL1C7: _tmpC8=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp75)->f1;goto _LL16A;}else{goto _LL16B;}_LL16B: if(*((int*)_tmp75)== 33){_LL1C8:
_tmpC9=((struct Cyc_Absyn_Malloc_e_struct*)_tmp75)->f1;_LL1CC: _tmpCD=_tmpC9.is_calloc;
goto _LL1CB;_LL1CB: _tmpCC=_tmpC9.rgn;goto _LL1CA;_LL1CA: _tmpCB=_tmpC9.elt_type;
goto _LL1C9;_LL1C9: _tmpCA=_tmpC9.num_elts;goto _LL16C;}else{goto _LL16D;}_LL16D: if(*((
int*)_tmp75)== 34){_LL1CE: _tmpCF=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp75)->f1;goto _LL1CD;_LL1CD: _tmpCE=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp75)->f2;goto _LL16E;}else{goto _LL16F;}_LL16F: if(*((int*)_tmp75)== 35){_LL1CF:
_tmpD0=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp75)->f1;goto _LL170;}else{goto
_LL171;}_LL171: if(*((int*)_tmp75)== 36){_LL1D0: _tmpD1=((struct Cyc_Absyn_Codegen_e_struct*)
_tmp75)->f1;goto _LL172;}else{goto _LL173;}_LL173: if(*((int*)_tmp75)== 37){_LL1D1:
_tmpD2=((struct Cyc_Absyn_Fill_e_struct*)_tmp75)->f1;goto _LL174;}else{goto _LL112;}
_LL114: Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(
_tmp77));Cyc_Absyndump_dump_char((int)'\'');goto _LL112;_LL116: Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpD4;_tmpD4.tag=1;_tmpD4.f1=(
int)((unsigned int)((int)_tmp7A));{void*_tmpD3[1]={& _tmpD4};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmpD3,sizeof(void*),1));}}));
goto _LL112;_LL118: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmpD6;_tmpD6.tag=1;_tmpD6.f1=(int)((unsigned int)_tmp7D);{void*_tmpD5[1]={&
_tmpD6};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmpD5,
sizeof(void*),1));}}));goto _LL112;_LL11A: Cyc_Absyndump_dump((struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmpD8;_tmpD8.tag=1;_tmpD8.f1=(unsigned int)_tmp80;{
void*_tmpD7[1]={& _tmpD8};Cyc_Std_aprintf(_tag_arr("%u",sizeof(unsigned char),3),
_tag_arr(_tmpD7,sizeof(void*),1));}}));goto _LL112;_LL11C: Cyc_Absyndump_dump(
_tag_arr("<<FIX LONG LONG CONSTANT>>",sizeof(unsigned char),27));goto _LL112;
_LL11E: Cyc_Absyndump_dump(_tmp86);goto _LL112;_LL120: Cyc_Absyndump_dump(_tag_arr("NULL",
sizeof(unsigned char),5));goto _LL112;_LL122: Cyc_Absyndump_dump_char((int)'"');
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp89));Cyc_Absyndump_dump_char((
int)'"');goto _LL112;_LL124: _tmp8B=_tmp8A;goto _LL126;_LL126: Cyc_Absyndump_dumpqvar(
_tmp8B);goto _LL112;_LL128: {struct _tagged_arr _tmpD9=Cyc_Absynpp_prim2str(_tmp8D);
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8C)){case 1: _LL1D2:
if(_tmp8D == (void*)19){Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp8C))->hd);Cyc_Absyndump_dump(_tag_arr(".size",
sizeof(unsigned char),6));}else{Cyc_Absyndump_dump(_tmpD9);Cyc_Absyndump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp8C))->hd);}
break;case 2: _LL1D3: Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp8C))->hd);Cyc_Absyndump_dump(_tmpD9);Cyc_Absyndump_dump_char((
int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp8C->tl))->hd);break;default: _LL1D4:(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Core_Failure_struct
_tmpDB;_tmpDB.tag=Cyc_Core_Failure;_tmpDB.f1=_tag_arr("Absyndump -- Bad number of arguments to primop",
sizeof(unsigned char),47);_tmpDB;});_tmpDA;}));}goto _LL112;}_LL12A: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp90);if(_tmp8F != 0){Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((void*)
_tmp8F->v));}Cyc_Absyndump_dump_nospace(_tag_arr("=",sizeof(unsigned char),2));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp8E);goto _LL112;_LL12C: Cyc_Absyndump_dump(
_tag_arr("++",sizeof(unsigned char),3));Cyc_Absyndump_dumpexp_prec(myprec,_tmp92);
goto _LL112;_LL12E: Cyc_Absyndump_dump(_tag_arr("--",sizeof(unsigned char),3));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp94);goto _LL112;_LL130: Cyc_Absyndump_dumpexp_prec(myprec,_tmp96);Cyc_Absyndump_dump(
_tag_arr("++",sizeof(unsigned char),3));goto _LL112;_LL132: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp98);Cyc_Absyndump_dump(_tag_arr("--",sizeof(unsigned char),3));goto
_LL112;_LL134: Cyc_Absyndump_dumpexp_prec(myprec,_tmp9B);Cyc_Absyndump_dump_char((
int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp9A);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp99);goto _LL112;_LL136: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumpexp_prec(myprec,_tmp9D);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp9C);Cyc_Absyndump_dump_char((int)')');goto
_LL112;_LL138: _tmpA1=_tmp9F;_tmpA0=_tmp9E;goto _LL13A;_LL13A: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpA1);Cyc_Absyndump_dump_nospace(_tag_arr("(",sizeof(unsigned char),2));
Cyc_Absyndump_dumpexps_prec(20,_tmpA0);Cyc_Absyndump_dump_nospace(_tag_arr(")",
sizeof(unsigned char),2));goto _LL112;_LL13C: Cyc_Absyndump_dump(_tag_arr("throw",
sizeof(unsigned char),6));Cyc_Absyndump_dumpexp_prec(myprec,_tmpA2);goto _LL112;
_LL13E: _tmpA4=_tmpA3;goto _LL140;_LL140: Cyc_Absyndump_dumpexp_prec(inprec,_tmpA4);
goto _LL112;_LL142: Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmpA6);
Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmpA5);goto
_LL112;_LL144: Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(
myprec,_tmpA7);goto _LL112;_LL146: Cyc_Absyndump_dump(_tag_arr("new ",sizeof(
unsigned char),5));Cyc_Absyndump_dumpexp_prec(myprec,_tmpA8);goto _LL112;_LL148:
Cyc_Absyndump_dump(_tag_arr("sizeof(",sizeof(unsigned char),8));Cyc_Absyndump_dumptyp(
_tmpAA);Cyc_Absyndump_dump_char((int)')');goto _LL112;_LL14A: Cyc_Absyndump_dump(
_tag_arr("sizeof(",sizeof(unsigned char),8));Cyc_Absyndump_dumpexp(_tmpAB);Cyc_Absyndump_dump_char((
int)')');goto _LL112;_LL14C: Cyc_Absyndump_dump(_tag_arr("offsetof(",sizeof(
unsigned char),10));Cyc_Absyndump_dumptyp(_tmpAE);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dump_nospace(*_tmpAD);Cyc_Absyndump_dump_char((int)')');goto _LL112;
_LL14E: Cyc_Absyndump_dump(_tag_arr("offsetof(",sizeof(unsigned char),10));Cyc_Absyndump_dumptyp(
_tmpB1);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump((struct _tagged_arr)({
struct Cyc_Std_Int_pa_struct _tmpDD;_tmpDD.tag=1;_tmpDD.f1=(int)_tmpB0;{void*
_tmpDC[1]={& _tmpDD};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),
_tag_arr(_tmpDC,sizeof(void*),1));}}));Cyc_Absyndump_dump_char((int)')');goto
_LL112;_LL150: Cyc_Absyndump_dump(_tag_arr("__gen(",sizeof(unsigned char),7));Cyc_Absyndump_dumptvars(
_tmpB3);Cyc_Absyndump_dumptyp(_tmpB2);Cyc_Absyndump_dump_char((int)')');goto
_LL112;_LL152: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(
myprec,_tmpB4);goto _LL112;_LL154: Cyc_Absyndump_dumpexp_prec(myprec,_tmpB6);Cyc_Absyndump_dump_char((
int)'.');Cyc_Absyndump_dump_nospace(*_tmpB5);goto _LL112;_LL156: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpB8);Cyc_Absyndump_dump_nospace(_tag_arr("->",sizeof(unsigned char),3));
Cyc_Absyndump_dump_nospace(*_tmpB7);goto _LL112;_LL158: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpBA);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmpB9);
Cyc_Absyndump_dump_char((int)']');goto _LL112;_LL15A: Cyc_Absyndump_dump(_tag_arr("$(",
sizeof(unsigned char),3));Cyc_Absyndump_dumpexps_prec(20,_tmpBB);Cyc_Absyndump_dump_char((
int)')');goto _LL112;_LL15C: Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp((*
_tmpBD).f3);Cyc_Absyndump_dump_char((int)')');((void(*)(void(*f)(struct _tuple9*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmpBC,_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LL112;_LL15E:((void(*)(void(*f)(struct _tuple9*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmpBE,_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LL112;_LL160: Cyc_Absyndump_dump(_tag_arr("new {for",
sizeof(unsigned char),9));Cyc_Absyndump_dump_str((*_tmpC1->name).f2);Cyc_Absyndump_dump_char((
int)'<');Cyc_Absyndump_dumpexp(_tmpC0);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(
_tmpBF);Cyc_Absyndump_dump_char((int)'}');goto _LL112;_LL162: Cyc_Absyndump_dumpqvar(
_tmpC3);((void(*)(void(*f)(struct _tuple9*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmpC2,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));goto _LL112;
_LL164:((void(*)(void(*f)(struct _tuple9*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmpC4,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));goto _LL112;
_LL166: Cyc_Absyndump_dumpqvar(_tmpC5->name);if(_tmpC6 != 0){((void(*)(void(*f)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,
_tmpC6,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));}goto _LL112;_LL168: Cyc_Absyndump_dumpqvar(
_tmpC7);goto _LL112;_LL16A: Cyc_Absyndump_dumpqvar(_tmpC8);goto _LL112;_LL16C: if(
_tmpCD){if(_tmpCC != 0){Cyc_Absyndump_dump(_tag_arr("rcalloc(",sizeof(
unsigned char),9));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(
_tmpCC));Cyc_Absyndump_dump(_tag_arr(",",sizeof(unsigned char),2));}else{Cyc_Absyndump_dump(
_tag_arr("calloc",sizeof(unsigned char),7));}Cyc_Absyndump_dumpexp(_tmpCA);Cyc_Absyndump_dump(
_tag_arr(",",sizeof(unsigned char),2));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmpCB)),0));Cyc_Absyndump_dump(_tag_arr(")",sizeof(
unsigned char),2));}else{if(_tmpCC != 0){Cyc_Absyndump_dump(_tag_arr("rmalloc(",
sizeof(unsigned char),9));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmpCC));Cyc_Absyndump_dump(_tag_arr(",",sizeof(unsigned char),2));}
else{Cyc_Absyndump_dump(_tag_arr("malloc(",sizeof(unsigned char),8));}if(_tmpCB
!= 0){Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmpCB,
0),_tmpCA,0));}else{Cyc_Absyndump_dumpexp(_tmpCA);}Cyc_Absyndump_dump(_tag_arr(")",
sizeof(unsigned char),2));}goto _LL112;_LL16E:((void(*)(void(*f)(struct _tuple9*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmpCE,_tag_arr("{",
sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LL112;_LL170: Cyc_Absyndump_dump_nospace(_tag_arr("({",
sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(_tmpD0);Cyc_Absyndump_dump_nospace(
_tag_arr("})",sizeof(unsigned char),3));goto _LL112;_LL172: Cyc_Absyndump_dump(
_tag_arr("codegen(",sizeof(unsigned char),9));Cyc_Absyndump_dumpdecl(({struct Cyc_Absyn_Decl*
_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->r=(void*)((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=({struct Cyc_Absyn_Fn_d_struct _tmpE0;
_tmpE0.tag=1;_tmpE0.f1=_tmpD1;_tmpE0;});_tmpDF;}));_tmpDE->loc=e->loc;_tmpDE;}));
Cyc_Absyndump_dump(_tag_arr(")",sizeof(unsigned char),2));goto _LL112;_LL174: Cyc_Absyndump_dump(
_tag_arr("fill(",sizeof(unsigned char),6));Cyc_Absyndump_dumpexp(_tmpD2);Cyc_Absyndump_dump(
_tag_arr(")",sizeof(unsigned char),2));goto _LL112;_LL112:;}if(inprec >= myprec){
Cyc_Absyndump_dump_char((int)')');}}void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*
e){Cyc_Absyndump_dumpexp_prec(0,e);}void Cyc_Absyndump_dumpswitchclauses(struct
Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmpE1=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmpE1->where_clause == 0?(void*)(
_tmpE1->pattern)->r == (void*)0: 0){Cyc_Absyndump_dump(_tag_arr("default:",sizeof(
unsigned char),9));}else{Cyc_Absyndump_dump(_tag_arr("case",sizeof(unsigned char),
5));Cyc_Absyndump_dumppat(_tmpE1->pattern);if(_tmpE1->where_clause != 0){Cyc_Absyndump_dump(
_tag_arr("&&",sizeof(unsigned char),3));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmpE1->where_clause));}Cyc_Absyndump_dump_nospace(_tag_arr(":",
sizeof(unsigned char),2));}Cyc_Absyndump_dumpstmt(_tmpE1->body);}}void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt*s){void*_tmpE2=(void*)s->r;struct Cyc_Absyn_Exp*_tmpE3;
struct Cyc_Absyn_Stmt*_tmpE4;struct Cyc_Absyn_Stmt*_tmpE5;struct Cyc_Absyn_Exp*
_tmpE6;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Stmt*_tmpE8;struct Cyc_Absyn_Stmt*
_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Stmt*_tmpEB;struct _tuple2
_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct _tagged_arr*_tmpEE;struct Cyc_Absyn_Stmt*
_tmpEF;struct _tuple2 _tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct _tuple2 _tmpF2;struct
Cyc_Absyn_Exp*_tmpF3;struct Cyc_Absyn_Exp*_tmpF4;struct Cyc_Absyn_ForArrayInfo
_tmpF5;struct Cyc_Absyn_Stmt*_tmpF6;struct _tuple2 _tmpF7;struct Cyc_Absyn_Exp*
_tmpF8;struct _tuple2 _tmpF9;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_List_List*_tmpFB;
struct Cyc_List_List*_tmpFC;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Stmt*
_tmpFE;struct Cyc_Absyn_Decl*_tmpFF;struct Cyc_Absyn_Stmt*_tmp100;struct
_tagged_arr*_tmp101;struct _tuple2 _tmp102;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Stmt*
_tmp104;struct Cyc_List_List*_tmp105;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_List_List*
_tmp107;struct Cyc_List_List*_tmp108;struct Cyc_List_List*_tmp109;struct Cyc_Absyn_Stmt*
_tmp10A;struct Cyc_Absyn_Stmt*_tmp10B;struct Cyc_Absyn_Vardecl*_tmp10C;struct Cyc_Absyn_Tvar*
_tmp10D;struct Cyc_Absyn_Stmt*_tmp10E;struct Cyc_Absyn_Stmt*_tmp10F;_LL1D7: if((int)
_tmpE2 == 0){goto _LL1D8;}else{goto _LL1D9;}_LL1D9: if((unsigned int)_tmpE2 > 1?*((
int*)_tmpE2)== 0: 0){_LL205: _tmpE3=((struct Cyc_Absyn_Exp_s_struct*)_tmpE2)->f1;
goto _LL1DA;}else{goto _LL1DB;}_LL1DB: if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 
1: 0){_LL207: _tmpE5=((struct Cyc_Absyn_Seq_s_struct*)_tmpE2)->f1;goto _LL206;_LL206:
_tmpE4=((struct Cyc_Absyn_Seq_s_struct*)_tmpE2)->f2;goto _LL1DC;}else{goto _LL1DD;}
_LL1DD: if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 2: 0){_LL208: _tmpE6=((struct
Cyc_Absyn_Return_s_struct*)_tmpE2)->f1;if(_tmpE6 == 0){goto _LL1DE;}else{goto
_LL1DF;}}else{goto _LL1DF;}_LL1DF: if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 2:
0){_LL209: _tmpE7=((struct Cyc_Absyn_Return_s_struct*)_tmpE2)->f1;goto _LL1E0;}
else{goto _LL1E1;}_LL1E1: if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 3: 0){_LL20C:
_tmpEA=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpE2)->f1;goto _LL20B;_LL20B:
_tmpE9=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpE2)->f2;goto _LL20A;_LL20A:
_tmpE8=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpE2)->f3;goto _LL1E2;}else{goto
_LL1E3;}_LL1E3: if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 4: 0){_LL20E: _tmpEC=((
struct Cyc_Absyn_While_s_struct*)_tmpE2)->f1;_LL20F: _tmpED=_tmpEC.f1;goto _LL20D;
_LL20D: _tmpEB=((struct Cyc_Absyn_While_s_struct*)_tmpE2)->f2;goto _LL1E4;}else{
goto _LL1E5;}_LL1E5: if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 5: 0){goto _LL1E6;}
else{goto _LL1E7;}_LL1E7: if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 6: 0){goto
_LL1E8;}else{goto _LL1E9;}_LL1E9: if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 7: 0){
_LL210: _tmpEE=((struct Cyc_Absyn_Goto_s_struct*)_tmpE2)->f1;goto _LL1EA;}else{goto
_LL1EB;}_LL1EB: if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 8: 0){_LL216: _tmpF4=((
struct Cyc_Absyn_For_s_struct*)_tmpE2)->f1;goto _LL214;_LL214: _tmpF2=((struct Cyc_Absyn_For_s_struct*)
_tmpE2)->f2;_LL215: _tmpF3=_tmpF2.f1;goto _LL212;_LL212: _tmpF0=((struct Cyc_Absyn_For_s_struct*)
_tmpE2)->f3;_LL213: _tmpF1=_tmpF0.f1;goto _LL211;_LL211: _tmpEF=((struct Cyc_Absyn_For_s_struct*)
_tmpE2)->f4;goto _LL1EC;}else{goto _LL1ED;}_LL1ED: if((unsigned int)_tmpE2 > 1?*((
int*)_tmpE2)== 19: 0){_LL217: _tmpF5=((struct Cyc_Absyn_ForArray_s_struct*)_tmpE2)->f1;
_LL21D: _tmpFB=_tmpF5.defns;goto _LL21B;_LL21B: _tmpF9=_tmpF5.condition;_LL21C:
_tmpFA=_tmpF9.f1;goto _LL219;_LL219: _tmpF7=_tmpF5.delta;_LL21A: _tmpF8=_tmpF7.f1;
goto _LL218;_LL218: _tmpF6=_tmpF5.body;goto _LL1EE;}else{goto _LL1EF;}_LL1EF: if((
unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 9: 0){_LL21F: _tmpFD=((struct Cyc_Absyn_Switch_s_struct*)
_tmpE2)->f1;goto _LL21E;_LL21E: _tmpFC=((struct Cyc_Absyn_Switch_s_struct*)_tmpE2)->f2;
goto _LL1F0;}else{goto _LL1F1;}_LL1F1: if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 
12: 0){_LL221: _tmpFF=((struct Cyc_Absyn_Decl_s_struct*)_tmpE2)->f1;goto _LL220;
_LL220: _tmpFE=((struct Cyc_Absyn_Decl_s_struct*)_tmpE2)->f2;goto _LL1F2;}else{goto
_LL1F3;}_LL1F3: if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 15: 0){_LL223: _tmp101=((
struct Cyc_Absyn_Label_s_struct*)_tmpE2)->f1;goto _LL222;_LL222: _tmp100=((struct
Cyc_Absyn_Label_s_struct*)_tmpE2)->f2;goto _LL1F4;}else{goto _LL1F5;}_LL1F5: if((
unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 16: 0){_LL226: _tmp104=((struct Cyc_Absyn_Do_s_struct*)
_tmpE2)->f1;goto _LL224;_LL224: _tmp102=((struct Cyc_Absyn_Do_s_struct*)_tmpE2)->f2;
_LL225: _tmp103=_tmp102.f1;goto _LL1F6;}else{goto _LL1F7;}_LL1F7: if((unsigned int)
_tmpE2 > 1?*((int*)_tmpE2)== 10: 0){_LL228: _tmp106=((struct Cyc_Absyn_SwitchC_s_struct*)
_tmpE2)->f1;goto _LL227;_LL227: _tmp105=((struct Cyc_Absyn_SwitchC_s_struct*)_tmpE2)->f2;
goto _LL1F8;}else{goto _LL1F9;}_LL1F9: if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 
11: 0){_LL229: _tmp107=((struct Cyc_Absyn_Fallthru_s_struct*)_tmpE2)->f1;if(_tmp107
== 0){goto _LL1FA;}else{goto _LL1FB;}}else{goto _LL1FB;}_LL1FB: if((unsigned int)
_tmpE2 > 1?*((int*)_tmpE2)== 11: 0){_LL22A: _tmp108=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmpE2)->f1;goto _LL1FC;}else{goto _LL1FD;}_LL1FD: if((unsigned int)_tmpE2 > 1?*((
int*)_tmpE2)== 17: 0){_LL22C: _tmp10A=((struct Cyc_Absyn_TryCatch_s_struct*)_tmpE2)->f1;
goto _LL22B;_LL22B: _tmp109=((struct Cyc_Absyn_TryCatch_s_struct*)_tmpE2)->f2;goto
_LL1FE;}else{goto _LL1FF;}_LL1FF: if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 18:
0){_LL22F: _tmp10D=((struct Cyc_Absyn_Region_s_struct*)_tmpE2)->f1;goto _LL22E;
_LL22E: _tmp10C=((struct Cyc_Absyn_Region_s_struct*)_tmpE2)->f2;goto _LL22D;_LL22D:
_tmp10B=((struct Cyc_Absyn_Region_s_struct*)_tmpE2)->f3;goto _LL200;}else{goto
_LL201;}_LL201: if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 13: 0){_LL230: _tmp10E=((
struct Cyc_Absyn_Cut_s_struct*)_tmpE2)->f1;goto _LL202;}else{goto _LL203;}_LL203:
if((unsigned int)_tmpE2 > 1?*((int*)_tmpE2)== 14: 0){_LL231: _tmp10F=((struct Cyc_Absyn_Splice_s_struct*)
_tmpE2)->f1;goto _LL204;}else{goto _LL1D6;}_LL1D8: Cyc_Absyndump_dump_semi();goto
_LL1D6;_LL1DA: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dumpexp(_tmpE3);Cyc_Absyndump_dump_semi();
goto _LL1D6;_LL1DC: if(Cyc_Absynpp_is_declaration(_tmpE5)){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstmt(_tmpE5);Cyc_Absyndump_dump_char((int)'}');}else{
Cyc_Absyndump_dumpstmt(_tmpE5);}if(Cyc_Absynpp_is_declaration(_tmpE4)){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstmt(_tmpE4);Cyc_Absyndump_dump_char((int)'}');}else{
Cyc_Absyndump_dumpstmt(_tmpE4);}goto _LL1D6;_LL1DE: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(_tag_arr("return;",sizeof(unsigned char),8));goto _LL1D6;
_LL1E0: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("return",
sizeof(unsigned char),7));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmpE7));Cyc_Absyndump_dump_semi();goto _LL1D6;_LL1E2: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("if(",sizeof(unsigned char),4));Cyc_Absyndump_dumpexp(
_tmpEA);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(
_tmpE9);Cyc_Absyndump_dump_char((int)'}');{void*_tmp110=(void*)_tmpE8->r;_LL233:
if((int)_tmp110 == 0){goto _LL234;}else{goto _LL235;}_LL235: goto _LL236;_LL234: goto
_LL232;_LL236: Cyc_Absyndump_dump(_tag_arr("else{",sizeof(unsigned char),6));Cyc_Absyndump_dumpstmt(
_tmpE8);Cyc_Absyndump_dump_char((int)'}');goto _LL232;_LL232:;}goto _LL1D6;_LL1E4:
Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("while(",sizeof(
unsigned char),7));Cyc_Absyndump_dumpexp(_tmpED);Cyc_Absyndump_dump_nospace(
_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(_tmpEB);Cyc_Absyndump_dump_char((
int)'}');goto _LL1D6;_LL1E6: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("break;",sizeof(unsigned char),7));goto _LL1D6;_LL1E8: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("continue;",sizeof(unsigned char),10));goto
_LL1D6;_LL1EA: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("goto",
sizeof(unsigned char),5));Cyc_Absyndump_dump_str(_tmpEE);Cyc_Absyndump_dump_semi();
goto _LL1D6;_LL1EC: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("for(",
sizeof(unsigned char),5));Cyc_Absyndump_dumpexp(_tmpF4);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmpF3);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(
_tmpF1);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(
_tmpEF);Cyc_Absyndump_dump_char((int)'}');goto _LL1D6;_LL1EE: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("forarray(",sizeof(unsigned char),10));for(0;
_tmpFB != 0;_tmpFB=_tmpFB->tl){Cyc_Absyndump_dumpvardecl((struct Cyc_Absyn_Vardecl*)
_tmpFB->hd,s->loc);}Cyc_Absyndump_dumpexp(_tmpFA);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(
_tmpF8);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(
_tmpF6);Cyc_Absyndump_dump_char((int)'}');goto _LL1D6;_LL1F0: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("switch(",sizeof(unsigned char),8));Cyc_Absyndump_dumpexp(
_tmpFD);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpswitchclauses(
_tmpFC);Cyc_Absyndump_dump_char((int)'}');goto _LL1D6;_LL1F2: Cyc_Absyndump_dumpdecl(
_tmpFF);Cyc_Absyndump_dumpstmt(_tmpFE);goto _LL1D6;_LL1F4: if(Cyc_Absynpp_is_declaration(
_tmp100)){Cyc_Absyndump_dump_str(_tmp101);Cyc_Absyndump_dump_nospace(_tag_arr(": {",
sizeof(unsigned char),4));Cyc_Absyndump_dumpstmt(_tmp100);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dump_str(_tmp101);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpstmt(_tmp100);}goto _LL1D6;_LL1F6: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(_tag_arr("do {",sizeof(unsigned char),5));Cyc_Absyndump_dumpstmt(
_tmp104);Cyc_Absyndump_dump_nospace(_tag_arr("} while (",sizeof(unsigned char),
10));Cyc_Absyndump_dumpexp(_tmp103);Cyc_Absyndump_dump_nospace(_tag_arr(");",
sizeof(unsigned char),3));goto _LL1D6;_LL1F8: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("switch \"C\" (",sizeof(unsigned char),13));Cyc_Absyndump_dumpexp(
_tmp106);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(unsigned char),3));for(
0;_tmp105 != 0;_tmp105=_tmp105->tl){struct Cyc_Absyn_SwitchC_clause _tmp112;struct
Cyc_Absyn_Stmt*_tmp113;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_SwitchC_clause*
_tmp111=(struct Cyc_Absyn_SwitchC_clause*)_tmp105->hd;_tmp112=*_tmp111;_LL239:
_tmp114=_tmp112.cnst_exp;goto _LL238;_LL238: _tmp113=_tmp112.body;goto _LL237;
_LL237: if(_tmp114 == 0){Cyc_Absyndump_dump(_tag_arr("default: ",sizeof(
unsigned char),10));}else{Cyc_Absyndump_dump(_tag_arr("case ",sizeof(
unsigned char),6));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(
_tmp114));Cyc_Absyndump_dump_char((int)':');}Cyc_Absyndump_dumpstmt(_tmp113);}
Cyc_Absyndump_dump_char((int)'}');goto _LL1D6;_LL1FA: Cyc_Absyndump_dump(_tag_arr("fallthru;",
sizeof(unsigned char),10));goto _LL1D6;_LL1FC: Cyc_Absyndump_dump(_tag_arr("fallthru(",
sizeof(unsigned char),10));Cyc_Absyndump_dumpexps_prec(20,_tmp108);Cyc_Absyndump_dump_nospace(
_tag_arr(");",sizeof(unsigned char),3));goto _LL1D6;_LL1FE: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("try",sizeof(unsigned char),4));Cyc_Absyndump_dumpstmt(
_tmp10A);Cyc_Absyndump_dump(_tag_arr("catch {",sizeof(unsigned char),8));Cyc_Absyndump_dumpswitchclauses(
_tmp109);Cyc_Absyndump_dump_char((int)'}');goto _LL1D6;_LL200: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("region<",sizeof(unsigned char),8));Cyc_Absyndump_dumptvar(
_tmp10D);Cyc_Absyndump_dump(_tag_arr("> ",sizeof(unsigned char),3));Cyc_Absyndump_dumpqvar(
_tmp10C->name);Cyc_Absyndump_dump(_tag_arr("{",sizeof(unsigned char),2));Cyc_Absyndump_dumpstmt(
_tmp10B);Cyc_Absyndump_dump(_tag_arr("}",sizeof(unsigned char),2));goto _LL1D6;
_LL202: Cyc_Absyndump_dump(_tag_arr("cut",sizeof(unsigned char),4));Cyc_Absyndump_dumpstmt(
_tmp10E);goto _LL1D6;_LL204: Cyc_Absyndump_dump(_tag_arr("splice",sizeof(
unsigned char),7));Cyc_Absyndump_dumpstmt(_tmp10F);goto _LL1D6;_LL1D6:;}struct
_tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(
struct _tuple10*dp){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*dp).f1,
_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",sizeof(unsigned char),2),
_tag_arr("=",sizeof(unsigned char),2));Cyc_Absyndump_dumppat((*dp).f2);}void Cyc_Absyndump_dumppat(
struct Cyc_Absyn_Pat*p){void*_tmp115=(void*)p->r;int _tmp116;void*_tmp117;int
_tmp118;void*_tmp119;unsigned char _tmp11A;struct _tagged_arr _tmp11B;struct Cyc_Absyn_Vardecl*
_tmp11C;struct Cyc_List_List*_tmp11D;struct Cyc_Absyn_Pat*_tmp11E;struct Cyc_Absyn_Vardecl*
_tmp11F;struct _tuple0*_tmp120;struct Cyc_List_List*_tmp121;struct _tuple0*_tmp122;
struct Cyc_List_List*_tmp123;struct Cyc_List_List*_tmp124;struct Cyc_Absyn_AggrInfo
_tmp125;void*_tmp126;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_Tunionfield*
_tmp128;struct Cyc_Absyn_Enumfield*_tmp129;struct Cyc_Absyn_Enumfield*_tmp12A;
_LL23B: if((int)_tmp115 == 0){goto _LL23C;}else{goto _LL23D;}_LL23D: if((int)_tmp115
== 1){goto _LL23E;}else{goto _LL23F;}_LL23F: if((unsigned int)_tmp115 > 2?*((int*)
_tmp115)== 1: 0){_LL25C: _tmp117=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp115)->f1;
if((int)_tmp117 == 0){goto _LL25B;}else{goto _LL241;}_LL25B: _tmp116=((struct Cyc_Absyn_Int_p_struct*)
_tmp115)->f2;goto _LL240;}else{goto _LL241;}_LL241: if((unsigned int)_tmp115 > 2?*((
int*)_tmp115)== 1: 0){_LL25E: _tmp119=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp115)->f1;if((int)_tmp119 == 1){goto _LL25D;}else{goto _LL243;}_LL25D: _tmp118=((
struct Cyc_Absyn_Int_p_struct*)_tmp115)->f2;goto _LL242;}else{goto _LL243;}_LL243:
if((unsigned int)_tmp115 > 2?*((int*)_tmp115)== 2: 0){_LL25F: _tmp11A=((struct Cyc_Absyn_Char_p_struct*)
_tmp115)->f1;goto _LL244;}else{goto _LL245;}_LL245: if((unsigned int)_tmp115 > 2?*((
int*)_tmp115)== 3: 0){_LL260: _tmp11B=((struct Cyc_Absyn_Float_p_struct*)_tmp115)->f1;
goto _LL246;}else{goto _LL247;}_LL247: if((unsigned int)_tmp115 > 2?*((int*)_tmp115)
== 0: 0){_LL261: _tmp11C=((struct Cyc_Absyn_Var_p_struct*)_tmp115)->f1;goto _LL248;}
else{goto _LL249;}_LL249: if((unsigned int)_tmp115 > 2?*((int*)_tmp115)== 4: 0){
_LL262: _tmp11D=((struct Cyc_Absyn_Tuple_p_struct*)_tmp115)->f1;goto _LL24A;}else{
goto _LL24B;}_LL24B: if((unsigned int)_tmp115 > 2?*((int*)_tmp115)== 5: 0){_LL263:
_tmp11E=((struct Cyc_Absyn_Pointer_p_struct*)_tmp115)->f1;goto _LL24C;}else{goto
_LL24D;}_LL24D: if((unsigned int)_tmp115 > 2?*((int*)_tmp115)== 6: 0){_LL264:
_tmp11F=((struct Cyc_Absyn_Reference_p_struct*)_tmp115)->f1;goto _LL24E;}else{goto
_LL24F;}_LL24F: if((unsigned int)_tmp115 > 2?*((int*)_tmp115)== 11: 0){_LL265:
_tmp120=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp115)->f1;goto _LL250;}else{goto
_LL251;}_LL251: if((unsigned int)_tmp115 > 2?*((int*)_tmp115)== 12: 0){_LL267:
_tmp122=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp115)->f1;goto _LL266;_LL266:
_tmp121=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp115)->f2;goto _LL252;}else{
goto _LL253;}_LL253: if((unsigned int)_tmp115 > 2?*((int*)_tmp115)== 7: 0){_LL26A:
_tmp125=((struct Cyc_Absyn_Aggr_p_struct*)_tmp115)->f1;_LL26B: _tmp126=(void*)
_tmp125.aggr_info;goto _LL269;_LL269: _tmp124=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp115)->f2;goto _LL268;_LL268: _tmp123=((struct Cyc_Absyn_Aggr_p_struct*)_tmp115)->f3;
goto _LL254;}else{goto _LL255;}_LL255: if((unsigned int)_tmp115 > 2?*((int*)_tmp115)
== 8: 0){_LL26D: _tmp128=((struct Cyc_Absyn_Tunion_p_struct*)_tmp115)->f2;goto
_LL26C;_LL26C: _tmp127=((struct Cyc_Absyn_Tunion_p_struct*)_tmp115)->f3;goto _LL256;}
else{goto _LL257;}_LL257: if((unsigned int)_tmp115 > 2?*((int*)_tmp115)== 9: 0){
_LL26E: _tmp129=((struct Cyc_Absyn_Enum_p_struct*)_tmp115)->f2;goto _LL258;}else{
goto _LL259;}_LL259: if((unsigned int)_tmp115 > 2?*((int*)_tmp115)== 10: 0){_LL26F:
_tmp12A=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp115)->f2;goto _LL25A;}else{goto
_LL23A;}_LL23C: Cyc_Absyndump_dump_char((int)'_');goto _LL23A;_LL23E: Cyc_Absyndump_dump(
_tag_arr("NULL",sizeof(unsigned char),5));goto _LL23A;_LL240: Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp12C;_tmp12C.tag=1;_tmp12C.f1=(
int)((unsigned int)_tmp116);{void*_tmp12B[1]={& _tmp12C};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(unsigned char),3),_tag_arr(_tmp12B,sizeof(void*),1));}}));goto _LL23A;
_LL242: Cyc_Absyndump_dump((struct _tagged_arr)({struct Cyc_Std_Int_pa_struct
_tmp12E;_tmp12E.tag=1;_tmp12E.f1=(unsigned int)_tmp118;{void*_tmp12D[1]={&
_tmp12E};Cyc_Std_aprintf(_tag_arr("%u",sizeof(unsigned char),3),_tag_arr(_tmp12D,
sizeof(void*),1));}}));goto _LL23A;_LL244: Cyc_Absyndump_dump(_tag_arr("'",sizeof(
unsigned char),2));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp11A));
Cyc_Absyndump_dump_nospace(_tag_arr("'",sizeof(unsigned char),2));goto _LL23A;
_LL246: Cyc_Absyndump_dump(_tmp11B);goto _LL23A;_LL248: Cyc_Absyndump_dumpqvar(
_tmp11C->name);goto _LL23A;_LL24A:((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct
Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp11D,_tag_arr("$(",sizeof(
unsigned char),3),_tag_arr(")",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2));goto _LL23A;_LL24C: Cyc_Absyndump_dump(_tag_arr("&",sizeof(
unsigned char),2));Cyc_Absyndump_dumppat(_tmp11E);goto _LL23A;_LL24E: Cyc_Absyndump_dump(
_tag_arr("*",sizeof(unsigned char),2));Cyc_Absyndump_dumpqvar(_tmp11F->name);
goto _LL23A;_LL250: Cyc_Absyndump_dumpqvar(_tmp120);goto _LL23A;_LL252: Cyc_Absyndump_dumpqvar(
_tmp122);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp121,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));goto _LL23A;
_LL254: {struct _tuple0*_tmp130;struct _tuple3 _tmp12F=Cyc_Absyn_aggr_kinded_name(
_tmp126);_LL271: _tmp130=_tmp12F.f2;goto _LL270;_LL270: Cyc_Absyndump_dumpqvar(
_tmp130);Cyc_Absyndump_dump_char((int)'{');((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp124,_tag_arr("[",
sizeof(unsigned char),2),_tag_arr("]",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpdp,_tmp123,_tag_arr("",sizeof(unsigned char),1),_tag_arr("}",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));goto _LL23A;}
_LL256: Cyc_Absyndump_dumpqvar(_tmp128->name);if(_tmp127 != 0){((void(*)(void(*f)(
struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
_tmp127,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));}goto _LL23A;_LL258: _tmp12A=_tmp129;
goto _LL25A;_LL25A: Cyc_Absyndump_dumpqvar(_tmp12A->name);goto _LL23A;_LL23A:;}void
Cyc_Absyndump_dumptunionfield(struct Cyc_Absyn_Tunionfield*ef){Cyc_Absyndump_dumpqvar(
ef->name);if(ef->typs != 0){Cyc_Absyndump_dumpargs(ef->typs);}}void Cyc_Absyndump_dumptunionfields(
struct Cyc_List_List*fields){((void(*)(void(*f)(struct Cyc_Absyn_Tunionfield*),
struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumptunionfield,
fields,_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpenumfield(
struct Cyc_Absyn_Enumfield*ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->tag != 0){
Cyc_Absyndump_dump(_tag_arr(" = ",sizeof(unsigned char),4));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List*fields){((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,
fields,_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpaggrfields(
struct Cyc_List_List*fields){for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Aggrfield
_tmp132;struct Cyc_List_List*_tmp133;struct Cyc_Absyn_Exp*_tmp134;void*_tmp135;
struct Cyc_Absyn_Tqual _tmp136;struct _tagged_arr*_tmp137;struct Cyc_Absyn_Aggrfield*
_tmp131=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp132=*_tmp131;_LL277: _tmp137=
_tmp132.name;goto _LL276;_LL276: _tmp136=_tmp132.tq;goto _LL275;_LL275: _tmp135=(
void*)_tmp132.type;goto _LL274;_LL274: _tmp134=_tmp132.width;goto _LL273;_LL273:
_tmp133=_tmp132.attributes;goto _LL272;_LL272:((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _tagged_arr*),struct _tagged_arr*))Cyc_Absyndump_dumptqtd)(
_tmp136,_tmp135,Cyc_Absyndump_dump_str,_tmp137);Cyc_Absyndump_dumpatts(_tmp133);
if(_tmp134 != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp((struct
Cyc_Absyn_Exp*)_check_null(_tmp134));}Cyc_Absyndump_dump_semi();}}void Cyc_Absyndump_dumptypedefname(
struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(td->name);Cyc_Absyndump_dumptvars(
td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}static void Cyc_Absyndump_dump_callconv_qvar(
struct _tuple3*pr){{void*_tmp138=(*pr).f1;_LL279: if((int)_tmp138 == 8){goto _LL27A;}
else{goto _LL27B;}_LL27B: if((int)_tmp138 == 0){goto _LL27C;}else{goto _LL27D;}_LL27D:
if((int)_tmp138 == 1){goto _LL27E;}else{goto _LL27F;}_LL27F: if((int)_tmp138 == 2){
goto _LL280;}else{goto _LL281;}_LL281: goto _LL282;_LL27A: goto _LL278;_LL27C: Cyc_Absyndump_dump(
_tag_arr("_stdcall",sizeof(unsigned char),9));goto _LL278;_LL27E: Cyc_Absyndump_dump(
_tag_arr("_cdecl",sizeof(unsigned char),7));goto _LL278;_LL280: Cyc_Absyndump_dump(
_tag_arr("_fastcall",sizeof(unsigned char),10));goto _LL278;_LL282: goto _LL278;
_LL278:;}Cyc_Absyndump_dumpqvar((*pr).f2);}static void Cyc_Absyndump_dump_callconv_fdqvar(
struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dump_callconv(fd->attributes);Cyc_Absyndump_dumpqvar(
fd->name);}static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){for(0;vds != 
0;vds=vds->tl){Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
if(vds->tl != 0){Cyc_Absyndump_dump_char((int)',');}}}void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Vardecl
_tmp13A;struct Cyc_List_List*_tmp13B;struct Cyc_Absyn_Exp*_tmp13C;void*_tmp13D;
struct Cyc_Absyn_Tqual _tmp13E;struct _tuple0*_tmp13F;void*_tmp140;struct Cyc_Absyn_Vardecl*
_tmp139=vd;_tmp13A=*_tmp139;_LL289: _tmp140=(void*)_tmp13A.sc;goto _LL288;_LL288:
_tmp13F=_tmp13A.name;goto _LL287;_LL287: _tmp13E=_tmp13A.tq;goto _LL286;_LL286:
_tmp13D=(void*)_tmp13A.type;goto _LL285;_LL285: _tmp13C=_tmp13A.initializer;goto
_LL284;_LL284: _tmp13B=_tmp13A.attributes;goto _LL283;_LL283: Cyc_Absyndump_dumploc(
loc);if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dumpatts(_tmp13B);Cyc_Absyndump_dumpscope(
_tmp140);{struct Cyc_List_List*_tmp142;void*_tmp143;struct Cyc_Absyn_Tqual _tmp144;
struct _tuple5 _tmp141=Cyc_Absynpp_to_tms(_tmp13E,_tmp13D);_LL28D: _tmp144=_tmp141.f1;
goto _LL28C;_LL28C: _tmp143=_tmp141.f2;goto _LL28B;_LL28B: _tmp142=_tmp141.f3;goto
_LL28A;_LL28A: {void*call_conv=(void*)8;{struct Cyc_List_List*tms2=_tmp142;for(0;
tms2 != 0;tms2=tms2->tl){void*_tmp145=(void*)tms2->hd;struct Cyc_List_List*_tmp146;
_LL28F: if((unsigned int)_tmp145 > 1?*((int*)_tmp145)== 4: 0){_LL293: _tmp146=((
struct Cyc_Absyn_Attributes_mod_struct*)_tmp145)->f2;goto _LL290;}else{goto _LL291;}
_LL291: goto _LL292;_LL290: for(0;_tmp146 != 0;_tmp146=_tmp146->tl){void*_tmp147=(
void*)_tmp146->hd;_LL295: if((int)_tmp147 == 0){goto _LL296;}else{goto _LL297;}
_LL297: if((int)_tmp147 == 1){goto _LL298;}else{goto _LL299;}_LL299: if((int)_tmp147
== 2){goto _LL29A;}else{goto _LL29B;}_LL29B: goto _LL29C;_LL296: call_conv=(void*)0;
goto _LL294;_LL298: call_conv=(void*)1;goto _LL294;_LL29A: call_conv=(void*)2;goto
_LL294;_LL29C: goto _LL294;_LL294:;}goto _LL28E;_LL292: goto _LL28E;_LL28E:;}}Cyc_Absyndump_dumptq(
_tmp144);Cyc_Absyndump_dumpntyp(_tmp143);{struct _tuple3 _tmp148=({struct _tuple3
_tmp149;_tmp149.f1=call_conv;_tmp149.f2=_tmp13F;_tmp149;});((void(*)(struct Cyc_List_List*
tms,void(*f)(struct _tuple3*),struct _tuple3*a))Cyc_Absyndump_dumptms)(Cyc_List_imp_rev(
_tmp142),Cyc_Absyndump_dump_callconv_qvar,& _tmp148);}}}}else{Cyc_Absyndump_dumpscope(
_tmp140);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct
_tuple0*))Cyc_Absyndump_dumptqtd)(_tmp13E,_tmp13D,Cyc_Absyndump_dumpqvar,_tmp13F);
Cyc_Absyndump_dumpatts(_tmp13B);}if(_tmp13C != 0){Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp13C));}Cyc_Absyndump_dump_semi();}
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){void*_tmp14A=(void*)d->r;
struct Cyc_Absyn_Fndecl*_tmp14B;struct Cyc_Absyn_Aggrdecl*_tmp14C;struct Cyc_Absyn_Vardecl*
_tmp14D;struct Cyc_Absyn_Tuniondecl*_tmp14E;struct Cyc_Absyn_Tuniondecl _tmp14F;int
_tmp150;struct Cyc_Core_Opt*_tmp151;struct Cyc_List_List*_tmp152;struct _tuple0*
_tmp153;void*_tmp154;struct Cyc_Absyn_Enumdecl*_tmp155;struct Cyc_Absyn_Enumdecl
_tmp156;struct Cyc_Core_Opt*_tmp157;struct _tuple0*_tmp158;void*_tmp159;struct Cyc_Absyn_Exp*
_tmp15A;struct Cyc_Absyn_Pat*_tmp15B;struct Cyc_List_List*_tmp15C;struct Cyc_Absyn_Typedefdecl*
_tmp15D;struct Cyc_List_List*_tmp15E;struct _tagged_arr*_tmp15F;struct Cyc_List_List*
_tmp160;struct _tuple0*_tmp161;struct Cyc_List_List*_tmp162;_LL29E: if(*((int*)
_tmp14A)== 1){_LL2B4: _tmp14B=((struct Cyc_Absyn_Fn_d_struct*)_tmp14A)->f1;goto
_LL29F;}else{goto _LL2A0;}_LL2A0: if(*((int*)_tmp14A)== 4){_LL2B5: _tmp14C=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp14A)->f1;goto _LL2A1;}else{goto _LL2A2;}_LL2A2: if(*((
int*)_tmp14A)== 0){_LL2B6: _tmp14D=((struct Cyc_Absyn_Var_d_struct*)_tmp14A)->f1;
goto _LL2A3;}else{goto _LL2A4;}_LL2A4: if(*((int*)_tmp14A)== 5){_LL2B7: _tmp14E=((
struct Cyc_Absyn_Tunion_d_struct*)_tmp14A)->f1;_tmp14F=*_tmp14E;_LL2BC: _tmp154=(
void*)_tmp14F.sc;goto _LL2BB;_LL2BB: _tmp153=_tmp14F.name;goto _LL2BA;_LL2BA:
_tmp152=_tmp14F.tvs;goto _LL2B9;_LL2B9: _tmp151=_tmp14F.fields;goto _LL2B8;_LL2B8:
_tmp150=_tmp14F.is_xtunion;goto _LL2A5;}else{goto _LL2A6;}_LL2A6: if(*((int*)
_tmp14A)== 6){_LL2BD: _tmp155=((struct Cyc_Absyn_Enum_d_struct*)_tmp14A)->f1;
_tmp156=*_tmp155;_LL2C0: _tmp159=(void*)_tmp156.sc;goto _LL2BF;_LL2BF: _tmp158=
_tmp156.name;goto _LL2BE;_LL2BE: _tmp157=_tmp156.fields;goto _LL2A7;}else{goto
_LL2A8;}_LL2A8: if(*((int*)_tmp14A)== 2){_LL2C2: _tmp15B=((struct Cyc_Absyn_Let_d_struct*)
_tmp14A)->f1;goto _LL2C1;_LL2C1: _tmp15A=((struct Cyc_Absyn_Let_d_struct*)_tmp14A)->f4;
goto _LL2A9;}else{goto _LL2AA;}_LL2AA: if(*((int*)_tmp14A)== 3){_LL2C3: _tmp15C=((
struct Cyc_Absyn_Letv_d_struct*)_tmp14A)->f1;goto _LL2AB;}else{goto _LL2AC;}_LL2AC:
if(*((int*)_tmp14A)== 7){_LL2C4: _tmp15D=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp14A)->f1;goto _LL2AD;}else{goto _LL2AE;}_LL2AE: if(*((int*)_tmp14A)== 8){_LL2C6:
_tmp15F=((struct Cyc_Absyn_Namespace_d_struct*)_tmp14A)->f1;goto _LL2C5;_LL2C5:
_tmp15E=((struct Cyc_Absyn_Namespace_d_struct*)_tmp14A)->f2;goto _LL2AF;}else{goto
_LL2B0;}_LL2B0: if(*((int*)_tmp14A)== 9){_LL2C8: _tmp161=((struct Cyc_Absyn_Using_d_struct*)
_tmp14A)->f1;goto _LL2C7;_LL2C7: _tmp160=((struct Cyc_Absyn_Using_d_struct*)_tmp14A)->f2;
goto _LL2B1;}else{goto _LL2B2;}_LL2B2: if(*((int*)_tmp14A)== 10){_LL2C9: _tmp162=((
struct Cyc_Absyn_ExternC_d_struct*)_tmp14A)->f1;goto _LL2B3;}else{goto _LL29D;}
_LL29F: Cyc_Absyndump_dumploc(d->loc);if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dumpatts(
_tmp14B->attributes);}if(_tmp14B->is_inline){if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dump(
_tag_arr("__inline",sizeof(unsigned char),9));}else{Cyc_Absyndump_dump(_tag_arr("inline",
sizeof(unsigned char),7));}}Cyc_Absyndump_dumpscope((void*)_tmp14B->sc);{void*t=(
void*)({struct Cyc_Absyn_FnType_struct*_tmp164=_cycalloc(sizeof(*_tmp164));
_tmp164[0]=({struct Cyc_Absyn_FnType_struct _tmp165;_tmp165.tag=8;_tmp165.f1=({
struct Cyc_Absyn_FnInfo _tmp166;_tmp166.tvars=_tmp14B->tvs;_tmp166.effect=_tmp14B->effect;
_tmp166.ret_typ=(void*)((void*)_tmp14B->ret_type);_tmp166.args=((struct Cyc_List_List*(*)(
struct _tuple1*(*f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp14B->args);_tmp166.c_varargs=_tmp14B->c_varargs;_tmp166.cyc_varargs=_tmp14B->cyc_varargs;
_tmp166.rgn_po=_tmp14B->rgn_po;_tmp166.attributes=0;_tmp166;});_tmp165;});
_tmp164;});((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp163;
_tmp163.q_const=0;_tmp163.q_volatile=0;_tmp163.q_restrict=0;_tmp163;}),t,Cyc_Absyndump_to_VC?
Cyc_Absyndump_dump_callconv_fdqvar: Cyc_Absyndump_dump_atts_qvar,_tmp14B);Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstmt(_tmp14B->body);Cyc_Absyndump_dump_char((int)'}');
goto _LL29D;}_LL2A1: Cyc_Absyndump_dumpscope((void*)_tmp14C->sc);Cyc_Absyndump_dumpaggr_kind((
void*)_tmp14C->kind);Cyc_Absyndump_dumpqvar(_tmp14C->name);Cyc_Absyndump_dumptvars(
_tmp14C->tvs);if(_tmp14C->fields == 0){Cyc_Absyndump_dump_semi();}else{Cyc_Absyndump_dump_char((
int)'{');if(_tmp14C->exist_vars != 0){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp14C->exist_vars))->v,_tag_arr("[",sizeof(
unsigned char),2),_tag_arr("]",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2));}Cyc_Absyndump_dumpaggrfields((struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp14C->fields))->v);Cyc_Absyndump_dump(_tag_arr("}",
sizeof(unsigned char),2));Cyc_Absyndump_dumpatts(_tmp14C->attributes);Cyc_Absyndump_dump(
_tag_arr(";",sizeof(unsigned char),2));}goto _LL29D;_LL2A3: Cyc_Absyndump_dumpvardecl(
_tmp14D,d->loc);goto _LL29D;_LL2A5: Cyc_Absyndump_dumpscope(_tmp154);Cyc_Absyndump_dump(
_tmp150? _tag_arr("xtunion",sizeof(unsigned char),8): _tag_arr("tunion",sizeof(
unsigned char),7));Cyc_Absyndump_dumpqvar(_tmp153);Cyc_Absyndump_dumptvars(
_tmp152);if(_tmp151 == 0){Cyc_Absyndump_dump_semi();}else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumptunionfields((struct Cyc_List_List*)_tmp151->v);Cyc_Absyndump_dump_nospace(
_tag_arr("};",sizeof(unsigned char),3));}goto _LL29D;_LL2A7: Cyc_Absyndump_dumpscope(
_tmp159);Cyc_Absyndump_dump(_tag_arr("enum ",sizeof(unsigned char),6));Cyc_Absyndump_dumpqvar(
_tmp158);if(_tmp157 == 0){Cyc_Absyndump_dump_semi();}else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp157->v);Cyc_Absyndump_dump_nospace(
_tag_arr("};",sizeof(unsigned char),3));}return;_LL2A9: Cyc_Absyndump_dump(
_tag_arr("let",sizeof(unsigned char),4));Cyc_Absyndump_dumppat(_tmp15B);Cyc_Absyndump_dump_char((
int)'=');Cyc_Absyndump_dumpexp(_tmp15A);Cyc_Absyndump_dump_semi();goto _LL29D;
_LL2AB: Cyc_Absyndump_dump(_tag_arr("let ",sizeof(unsigned char),5));Cyc_Absyndump_dumpids(
_tmp15C);Cyc_Absyndump_dump_semi();goto _LL29D;_LL2AD: if(! Cyc_Absyndump_expand_typedefs?
1: Cyc_Absynpp_is_anon_aggrtype((void*)_tmp15D->defn)){Cyc_Absyndump_dump(
_tag_arr("typedef",sizeof(unsigned char),8));((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(({
struct Cyc_Absyn_Tqual _tmp167;_tmp167.q_const=0;_tmp167.q_volatile=0;_tmp167.q_restrict=
0;_tmp167;}),(void*)_tmp15D->defn,Cyc_Absyndump_dumptypedefname,_tmp15D);Cyc_Absyndump_dump_semi();}
goto _LL29D;_LL2AF: Cyc_Absyndump_dump(_tag_arr("namespace",sizeof(unsigned char),
10));Cyc_Absyndump_dump_str(_tmp15F);Cyc_Absyndump_dump_char((int)'{');for(0;
_tmp15E != 0;_tmp15E=_tmp15E->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)
_tmp15E->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL29D;_LL2B1: Cyc_Absyndump_dump(
_tag_arr("using",sizeof(unsigned char),6));Cyc_Absyndump_dumpqvar(_tmp161);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp160 != 0;_tmp160=_tmp160->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp160->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL29D;
_LL2B3: Cyc_Absyndump_dump(_tag_arr("extern \"C\" {",sizeof(unsigned char),13));
for(0;_tmp162 != 0;_tmp162=_tmp162->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)
_tmp162->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL29D;_LL29D:;}static void
Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){unsigned int i=Cyc_Evexp_eval_const_uint_exp(
e);if(i != 1){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((
int)'}');}}void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(void*),
void*a){if(tms == 0){f(a);return;}{void*_tmp168=(void*)tms->hd;struct Cyc_Absyn_Tqual
_tmp169;void*_tmp16A;void*_tmp16B;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_Tqual
_tmp16D;void*_tmp16E;void*_tmp16F;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Tqual
_tmp171;void*_tmp172;void*_tmp173;struct Cyc_Absyn_Tqual _tmp174;void*_tmp175;
struct Cyc_Absyn_Tvar*_tmp176;void*_tmp177;struct Cyc_Absyn_Exp*_tmp178;struct Cyc_Absyn_Tqual
_tmp179;void*_tmp17A;struct Cyc_Absyn_Tvar*_tmp17B;void*_tmp17C;struct Cyc_Absyn_Exp*
_tmp17D;struct Cyc_Absyn_Tqual _tmp17E;void*_tmp17F;struct Cyc_Absyn_Tvar*_tmp180;
void*_tmp181;_LL2CB: if((unsigned int)_tmp168 > 1?*((int*)_tmp168)== 1: 0){_LL2DD:
_tmp16B=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp168)->f1;if((
unsigned int)_tmp16B > 1?*((int*)_tmp16B)== 1: 0){_LL2DE: _tmp16C=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmp16B)->f1;goto _LL2DC;}else{goto _LL2CD;}_LL2DC: _tmp16A=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp168)->f2;if((int)_tmp16A == 2){goto _LL2DB;}else{goto _LL2CD;}_LL2DB: _tmp169=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp168)->f3;goto _LL2CC;}else{goto _LL2CD;}
_LL2CD: if((unsigned int)_tmp168 > 1?*((int*)_tmp168)== 1: 0){_LL2E1: _tmp16F=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp168)->f1;if((unsigned int)_tmp16F > 1?*((
int*)_tmp16F)== 0: 0){_LL2E2: _tmp170=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmp16F)->f1;goto _LL2E0;}else{goto _LL2CF;}_LL2E0: _tmp16E=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp168)->f2;if((int)_tmp16E == 2){goto _LL2DF;}else{goto _LL2CF;}_LL2DF: _tmp16D=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp168)->f3;goto _LL2CE;}else{goto _LL2CF;}
_LL2CF: if((unsigned int)_tmp168 > 1?*((int*)_tmp168)== 1: 0){_LL2E5: _tmp173=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp168)->f1;if((int)_tmp173 == 0){goto _LL2E4;}
else{goto _LL2D1;}_LL2E4: _tmp172=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp168)->f2;if((int)_tmp172 == 2){goto _LL2E3;}else{goto _LL2D1;}_LL2E3: _tmp171=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp168)->f3;goto _LL2D0;}else{goto _LL2D1;}
_LL2D1: if((unsigned int)_tmp168 > 1?*((int*)_tmp168)== 1: 0){_LL2E9: _tmp177=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp168)->f1;if((unsigned int)_tmp177 > 1?*((
int*)_tmp177)== 1: 0){_LL2EA: _tmp178=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmp177)->f1;goto _LL2E7;}else{goto _LL2D3;}_LL2E7: _tmp175=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp168)->f2;if((unsigned int)_tmp175 > 3?*((int*)_tmp175)== 1: 0){_LL2E8: _tmp176=((
struct Cyc_Absyn_VarType_struct*)_tmp175)->f1;goto _LL2E6;}else{goto _LL2D3;}_LL2E6:
_tmp174=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp168)->f3;goto _LL2D2;}else{goto
_LL2D3;}_LL2D3: if((unsigned int)_tmp168 > 1?*((int*)_tmp168)== 1: 0){_LL2EE:
_tmp17C=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp168)->f1;if((
unsigned int)_tmp17C > 1?*((int*)_tmp17C)== 0: 0){_LL2EF: _tmp17D=((struct Cyc_Absyn_NonNullable_ps_struct*)
_tmp17C)->f1;goto _LL2EC;}else{goto _LL2D5;}_LL2EC: _tmp17A=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp168)->f2;if((unsigned int)_tmp17A > 3?*((int*)_tmp17A)== 1: 0){_LL2ED: _tmp17B=((
struct Cyc_Absyn_VarType_struct*)_tmp17A)->f1;goto _LL2EB;}else{goto _LL2D5;}_LL2EB:
_tmp179=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp168)->f3;goto _LL2D4;}else{goto
_LL2D5;}_LL2D5: if((unsigned int)_tmp168 > 1?*((int*)_tmp168)== 1: 0){_LL2F3:
_tmp181=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp168)->f1;if((int)
_tmp181 == 0){goto _LL2F1;}else{goto _LL2D7;}_LL2F1: _tmp17F=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp168)->f2;if((unsigned int)_tmp17F > 3?*((int*)_tmp17F)== 1: 0){_LL2F2: _tmp180=((
struct Cyc_Absyn_VarType_struct*)_tmp17F)->f1;goto _LL2F0;}else{goto _LL2D7;}_LL2F0:
_tmp17E=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp168)->f3;goto _LL2D6;}else{goto
_LL2D7;}_LL2D7: if((unsigned int)_tmp168 > 1?*((int*)_tmp168)== 1: 0){goto _LL2D8;}
else{goto _LL2D9;}_LL2D9: goto _LL2DA;_LL2CC: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dump_upperbound(
_tmp16C);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL2CE: Cyc_Absyndump_dump_char((
int)'@');Cyc_Absyndump_dump_upperbound(_tmp170);Cyc_Absyndump_dumptms(tms->tl,f,
a);return;_LL2D0: Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumptms(tms->tl,
f,a);return;_LL2D2: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dump_upperbound(
_tmp178);Cyc_Absyndump_dump_str(_tmp176->name);Cyc_Absyndump_dumptms(tms->tl,f,a);
return;_LL2D4: Cyc_Absyndump_dump_char((int)'@');Cyc_Absyndump_dump_upperbound(
_tmp17D);Cyc_Absyndump_dump_str(_tmp17B->name);Cyc_Absyndump_dumptms(tms->tl,f,a);
return;_LL2D6: Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dump_str(_tmp180->name);
Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL2D8:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182[0]=({struct Cyc_Core_Impossible_struct
_tmp183;_tmp183.tag=Cyc_Core_Impossible;_tmp183.f1=_tag_arr("dumptms: bad Pointer_mod",
sizeof(unsigned char),25);_tmp183;});_tmp182;}));_LL2DA: {int next_is_pointer=0;
if(tms->tl != 0){void*_tmp184=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;
_LL2F5: if((unsigned int)_tmp184 > 1?*((int*)_tmp184)== 1: 0){goto _LL2F6;}else{goto
_LL2F7;}_LL2F7: goto _LL2F8;_LL2F6: next_is_pointer=1;goto _LL2F4;_LL2F8: goto _LL2F4;
_LL2F4:;}if(next_is_pointer){Cyc_Absyndump_dump_char((int)'(');}Cyc_Absyndump_dumptms(
tms->tl,f,a);if(next_is_pointer){Cyc_Absyndump_dump_char((int)')');}{void*
_tmp185=(void*)tms->hd;struct Cyc_Absyn_Exp*_tmp186;void*_tmp187;struct Cyc_List_List*
_tmp188;struct Cyc_Core_Opt*_tmp189;struct Cyc_Absyn_VarargInfo*_tmp18A;int _tmp18B;
struct Cyc_List_List*_tmp18C;void*_tmp18D;struct Cyc_Position_Segment*_tmp18E;
struct Cyc_List_List*_tmp18F;int _tmp190;struct Cyc_Position_Segment*_tmp191;struct
Cyc_List_List*_tmp192;struct Cyc_List_List*_tmp193;void*_tmp194;void*_tmp195;
_LL2FA: if((int)_tmp185 == 0){goto _LL2FB;}else{goto _LL2FC;}_LL2FC: if((unsigned int)
_tmp185 > 1?*((int*)_tmp185)== 0: 0){_LL308: _tmp186=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp185)->f1;goto _LL2FD;}else{goto _LL2FE;}_LL2FE: if((unsigned int)_tmp185 > 1?*((
int*)_tmp185)== 2: 0){_LL309: _tmp187=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp185)->f1;if(*((int*)_tmp187)== 1){_LL30E: _tmp18C=((struct Cyc_Absyn_WithTypes_struct*)
_tmp187)->f1;goto _LL30D;_LL30D: _tmp18B=((struct Cyc_Absyn_WithTypes_struct*)
_tmp187)->f2;goto _LL30C;_LL30C: _tmp18A=((struct Cyc_Absyn_WithTypes_struct*)
_tmp187)->f3;goto _LL30B;_LL30B: _tmp189=((struct Cyc_Absyn_WithTypes_struct*)
_tmp187)->f4;goto _LL30A;_LL30A: _tmp188=((struct Cyc_Absyn_WithTypes_struct*)
_tmp187)->f5;goto _LL2FF;}else{goto _LL300;}}else{goto _LL300;}_LL300: if((
unsigned int)_tmp185 > 1?*((int*)_tmp185)== 2: 0){_LL30F: _tmp18D=(void*)((struct
Cyc_Absyn_Function_mod_struct*)_tmp185)->f1;if(*((int*)_tmp18D)== 0){_LL311:
_tmp18F=((struct Cyc_Absyn_NoTypes_struct*)_tmp18D)->f1;goto _LL310;_LL310: _tmp18E=((
struct Cyc_Absyn_NoTypes_struct*)_tmp18D)->f2;goto _LL301;}else{goto _LL302;}}else{
goto _LL302;}_LL302: if((unsigned int)_tmp185 > 1?*((int*)_tmp185)== 3: 0){_LL314:
_tmp192=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp185)->f1;goto _LL313;_LL313:
_tmp191=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp185)->f2;goto _LL312;_LL312:
_tmp190=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp185)->f3;goto _LL303;}else{
goto _LL304;}_LL304: if((unsigned int)_tmp185 > 1?*((int*)_tmp185)== 4: 0){_LL315:
_tmp193=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp185)->f2;goto _LL305;}else{
goto _LL306;}_LL306: if((unsigned int)_tmp185 > 1?*((int*)_tmp185)== 1: 0){_LL317:
_tmp195=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp185)->f1;goto _LL316;
_LL316: _tmp194=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp185)->f2;goto
_LL307;}else{goto _LL2F9;}_LL2FB: Cyc_Absyndump_dump(_tag_arr("[]",sizeof(
unsigned char),3));goto _LL2F9;_LL2FD: Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(
_tmp186);Cyc_Absyndump_dump_char((int)']');goto _LL2F9;_LL2FF: Cyc_Absyndump_dumpfunargs(
_tmp18C,_tmp18B,_tmp18A,_tmp189,_tmp188);goto _LL2F9;_LL301:((void(*)(void(*f)(
struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,
_tmp18F,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LL2F9;_LL303: if(_tmp190){Cyc_Absyndump_dumpkindedtvars(
_tmp192);}else{Cyc_Absyndump_dumptvars(_tmp192);}goto _LL2F9;_LL305: Cyc_Absyndump_dumpatts(
_tmp193);goto _LL2F9;_LL307:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196[0]=({struct Cyc_Core_Impossible_struct
_tmp197;_tmp197.tag=Cyc_Core_Impossible;_tmp197.f1=_tag_arr("dumptms",sizeof(
unsigned char),8);_tmp197;});_tmp196;}));_LL2F9:;}return;}_LL2CA:;}}void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){struct Cyc_List_List*
_tmp199;void*_tmp19A;struct Cyc_Absyn_Tqual _tmp19B;struct _tuple5 _tmp198=Cyc_Absynpp_to_tms(
tq,t);_LL31B: _tmp19B=_tmp198.f1;goto _LL31A;_LL31A: _tmp19A=_tmp198.f2;goto _LL319;
_LL319: _tmp199=_tmp198.f3;goto _LL318;_LL318: Cyc_Absyndump_dumptq(_tmp19B);Cyc_Absyndump_dumpntyp(
_tmp19A);Cyc_Absyndump_dumptms(Cyc_List_imp_rev(_tmp199),f,a);}void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=
f;for(0;tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}({
void*_tmp19C[0]={};Cyc_Std_fprintf(f,_tag_arr("\n",sizeof(unsigned char),2),
_tag_arr(_tmp19C,sizeof(void*),0));});}
