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
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern unsigned char Cyc_List_List_mismatch[18];extern unsigned char Cyc_List_Nth[
8];struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
line_no;int col;};extern unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];
struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern
struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);struct
Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_Absyn_Rel_n_struct{
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
15];extern struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*
x);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct Cyc_Set_Set;
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
le;};extern unsigned char Cyc_Tcutil_TypeErr[12];extern void*Cyc_Tcutil_impos(
struct _tagged_arr fmt,struct _tagged_arr ap);extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_tvar_kind(struct
Cyc_Absyn_Tvar*t);extern void*Cyc_Tcutil_compress(void*t);extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);extern int Cyc_Evexp_okay_szofarg(void*t);void*Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp*e);void*Cyc_Evexp_promote_const(void*cn){void*_tmp0=cn;
unsigned char _tmp1;void*_tmp2;short _tmp3;void*_tmp4;_LL1: if((unsigned int)_tmp0 > 
1?*((int*)_tmp0)== 0: 0){_LL8: _tmp2=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp0)->f1;
goto _LL7;_LL7: _tmp1=((struct Cyc_Absyn_Char_c_struct*)_tmp0)->f2;goto _LL2;}else{
goto _LL3;}_LL3: if((unsigned int)_tmp0 > 1?*((int*)_tmp0)== 1: 0){_LLA: _tmp4=(void*)((
struct Cyc_Absyn_Short_c_struct*)_tmp0)->f1;goto _LL9;_LL9: _tmp3=((struct Cyc_Absyn_Short_c_struct*)
_tmp0)->f2;goto _LL4;}else{goto _LL5;}_LL5: goto _LL6;_LL2: return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=({struct Cyc_Absyn_Int_c_struct _tmp6;
_tmp6.tag=2;_tmp6.f1=(void*)_tmp2;_tmp6.f2=(int)_tmp1;_tmp6;});_tmp5;});_LL4:
return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp7=_cycalloc(sizeof(*_tmp7));
_tmp7[0]=({struct Cyc_Absyn_Int_c_struct _tmp8;_tmp8.tag=2;_tmp8.f1=(void*)_tmp4;
_tmp8.f2=(int)_tmp3;_tmp8;});_tmp7;});_LL6: return cn;_LL0:;}unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e){void*_tmp9=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(
e));int _tmpA;_LLC: if((unsigned int)_tmp9 > 1?*((int*)_tmp9)== 2: 0){_LL10: _tmpA=((
struct Cyc_Absyn_Int_c_struct*)_tmp9)->f2;goto _LLD;}else{goto _LLE;}_LLE: goto _LLF;
_LLD: return(unsigned int)_tmpA;_LLF:({void*_tmpB[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("expecting unsigned int",sizeof(unsigned char),23),_tag_arr(_tmpB,
sizeof(void*),0));});return 0;_LLB:;}int Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*
e){void*_tmpC=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(e));int _tmpD;
long long _tmpE;_LL12: if((unsigned int)_tmpC > 1?*((int*)_tmpC)== 2: 0){_LL1A: _tmpD=((
struct Cyc_Absyn_Int_c_struct*)_tmpC)->f2;goto _LL13;}else{goto _LL14;}_LL14: if((
unsigned int)_tmpC > 1?*((int*)_tmpC)== 3: 0){_LL1B: _tmpE=((struct Cyc_Absyn_LongLong_c_struct*)
_tmpC)->f2;goto _LL15;}else{goto _LL16;}_LL16: if((int)_tmpC == 0){goto _LL17;}else{
goto _LL18;}_LL18: goto _LL19;_LL13: return _tmpD != 0;_LL15: return _tmpE != 0;_LL17:
return 0;_LL19:({void*_tmpF[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("expecting bool",
sizeof(unsigned char),15),_tag_arr(_tmpF,sizeof(void*),0));});return 0;_LL11:;}
struct _tuple3{struct Cyc_Absyn_Tqual f1;void*f2;};int Cyc_Evexp_okay_szofarg(void*t){
void*_tmp10=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp11;struct Cyc_Core_Opt*
_tmp12;struct Cyc_Core_Opt _tmp13;void*_tmp14;struct Cyc_Absyn_TunionFieldInfo
_tmp15;void*_tmp16;struct Cyc_Absyn_Tunionfield*_tmp17;struct Cyc_List_List*_tmp18;
struct Cyc_Absyn_PtrInfo _tmp19;struct Cyc_Absyn_Conref*_tmp1A;struct Cyc_Absyn_Exp*
_tmp1B;void*_tmp1C;struct Cyc_Absyn_AggrInfo _tmp1D;void*_tmp1E;struct Cyc_List_List*
_tmp1F;_LL1D: if((int)_tmp10 == 0){goto _LL1E;}else{goto _LL1F;}_LL1F: if((
unsigned int)_tmp10 > 3?*((int*)_tmp10)== 1: 0){_LL4F: _tmp11=((struct Cyc_Absyn_VarType_struct*)
_tmp10)->f1;goto _LL20;}else{goto _LL21;}_LL21: if((unsigned int)_tmp10 > 3?*((int*)
_tmp10)== 0: 0){_LL50: _tmp12=((struct Cyc_Absyn_Evar_struct*)_tmp10)->f1;if(_tmp12
== 0){goto _LL23;}else{_tmp13=*_tmp12;_LL51: _tmp14=(void*)_tmp13.v;if((int)_tmp14
== 2){goto _LL22;}else{goto _LL23;}}}else{goto _LL23;}_LL23: if((unsigned int)_tmp10
> 3?*((int*)_tmp10)== 0: 0){goto _LL24;}else{goto _LL25;}_LL25: if((unsigned int)
_tmp10 > 3?*((int*)_tmp10)== 2: 0){goto _LL26;}else{goto _LL27;}_LL27: if((
unsigned int)_tmp10 > 3?*((int*)_tmp10)== 3: 0){_LL52: _tmp15=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp10)->f1;_LL53: _tmp16=(void*)_tmp15.field_info;if(*((int*)_tmp16)== 1){_LL54:
_tmp17=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp16)->f2;goto _LL28;}else{
goto _LL29;}}else{goto _LL29;}_LL29: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 9:
0){_LL55: _tmp18=((struct Cyc_Absyn_TupleType_struct*)_tmp10)->f1;goto _LL2A;}else{
goto _LL2B;}_LL2B: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 3: 0){goto _LL2C;}
else{goto _LL2D;}_LL2D: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 4: 0){_LL56:
_tmp19=((struct Cyc_Absyn_PointerType_struct*)_tmp10)->f1;_LL57: _tmp1A=_tmp19.bounds;
goto _LL2E;}else{goto _LL2F;}_LL2F: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 14:
0){goto _LL30;}else{goto _LL31;}_LL31: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 
5: 0){goto _LL32;}else{goto _LL33;}_LL33: if((int)_tmp10 == 1){goto _LL34;}else{goto
_LL35;}_LL35: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 6: 0){goto _LL36;}else{
goto _LL37;}_LL37: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 7: 0){_LL59: _tmp1C=(
void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp10)->f1;goto _LL58;_LL58: _tmp1B=((
struct Cyc_Absyn_ArrayType_struct*)_tmp10)->f3;goto _LL38;}else{goto _LL39;}_LL39:
if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 8: 0){goto _LL3A;}else{goto _LL3B;}
_LL3B: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 15: 0){goto _LL3C;}else{goto
_LL3D;}_LL3D: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 10: 0){_LL5A: _tmp1D=((
struct Cyc_Absyn_AggrType_struct*)_tmp10)->f1;_LL5B: _tmp1E=(void*)_tmp1D.aggr_info;
goto _LL3E;}else{goto _LL3F;}_LL3F: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 11:
0){_LL5C: _tmp1F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp10)->f2;goto _LL40;}
else{goto _LL41;}_LL41: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 12: 0){goto
_LL42;}else{goto _LL43;}_LL43: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 13: 0){
goto _LL44;}else{goto _LL45;}_LL45: if((int)_tmp10 == 2){goto _LL46;}else{goto _LL47;}
_LL47: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 18: 0){goto _LL48;}else{goto
_LL49;}_LL49: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 17: 0){goto _LL4A;}else{
goto _LL4B;}_LL4B: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 19: 0){goto _LL4C;}
else{goto _LL4D;}_LL4D: if((unsigned int)_tmp10 > 3?*((int*)_tmp10)== 16: 0){goto
_LL4E;}else{goto _LL1C;}_LL1E: return 0;_LL20: {void*_tmp20=Cyc_Tcutil_tvar_kind(
_tmp11);_LL5E: if((int)_tmp20 == 2){goto _LL5F;}else{goto _LL60;}_LL60: goto _LL61;
_LL5F: return 1;_LL61: return 0;_LL5D:;}_LL22: return 1;_LL24: return 0;_LL26: return 1;
_LL28: _tmp18=_tmp17->typs;goto _LL2A;_LL2A: for(0;_tmp18 != 0;_tmp18=_tmp18->tl){
if(! Cyc_Evexp_okay_szofarg((*((struct _tuple3*)_tmp18->hd)).f2)){return 0;}}return
1;_LL2C: return 0;_LL2E: {void*_tmp21=(void*)(Cyc_Absyn_compress_conref(_tmp1A))->v;
_LL63: if((unsigned int)_tmp21 > 1?*((int*)_tmp21)== 0: 0){goto _LL64;}else{goto
_LL65;}_LL65: goto _LL66;_LL64: return 1;_LL66: return 0;_LL62:;}_LL30: goto _LL32;_LL32:
goto _LL34;_LL34: goto _LL36;_LL36: return 1;_LL38: return _tmp1B != 0;_LL3A: return 0;
_LL3C: return 1;_LL3E: {struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp22=_tmp1E;struct Cyc_Absyn_Aggrdecl**
_tmp23;struct Cyc_Absyn_Aggrdecl*_tmp24;_LL68: if(*((int*)_tmp22)== 0){goto _LL69;}
else{goto _LL6A;}_LL6A: if(*((int*)_tmp22)== 1){_LL6C: _tmp23=((struct Cyc_Absyn_KnownAggr_struct*)
_tmp22)->f1;_tmp24=*_tmp23;goto _LL6B;}else{goto _LL67;}_LL69:({void*_tmp25[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("szof on unchecked StructType",
sizeof(unsigned char),29),_tag_arr(_tmp25,sizeof(void*),0));});_LL6B: ad=_tmp24;
_LL67:;}if(ad->fields == 0){return 0;}_tmp1F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ad->fields))->v;goto _LL40;}_LL40: for(0;_tmp1F != 0;_tmp1F=_tmp1F->tl){
if(! Cyc_Evexp_okay_szofarg((void*)((struct Cyc_Absyn_Aggrfield*)_tmp1F->hd)->type)){
return 0;}}return 1;_LL42: goto _LL44;_LL44: return 1;_LL46: goto _LL48;_LL48: goto _LL4A;
_LL4A: goto _LL4C;_LL4C: return 0;_LL4E:({void*_tmp26[0]={};((int(*)(struct
_tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct _tagged_arr)
Cyc_Absynpp_typ2string(t);{void*_tmp27[1]={& _tmp28};Cyc_Std_aprintf(_tag_arr("szof typedeftype %s",
sizeof(unsigned char),20),_tag_arr(_tmp27,sizeof(void*),1));}}),_tag_arr(_tmp26,
sizeof(void*),0));});_LL1C:;}struct _tuple4{void*f1;void*f2;};void*Cyc_Evexp_eval_const_unprimop(
void*p,struct Cyc_Absyn_Exp*e){void*cn=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(
e));struct _tuple4 _tmp2A=({struct _tuple4 _tmp29;_tmp29.f1=p;_tmp29.f2=cn;_tmp29;});
void*_tmp2B;void*_tmp2C;int _tmp2D;void*_tmp2E;void*_tmp2F;void*_tmp30;int _tmp31;
void*_tmp32;void*_tmp33;void*_tmp34;int _tmp35;void*_tmp36;void*_tmp37;void*
_tmp38;_LL6E: _LL7A: _tmp2B=_tmp2A.f1;if((int)_tmp2B == 0){goto _LL6F;}else{goto
_LL70;}_LL70: _LL7E: _tmp2F=_tmp2A.f1;if((int)_tmp2F == 2){goto _LL7B;}else{goto
_LL72;}_LL7B: _tmp2C=_tmp2A.f2;if((unsigned int)_tmp2C > 1?*((int*)_tmp2C)== 2: 0){
_LL7D: _tmp2E=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp2C)->f1;goto _LL7C;_LL7C:
_tmp2D=((struct Cyc_Absyn_Int_c_struct*)_tmp2C)->f2;goto _LL71;}else{goto _LL72;}
_LL72: _LL82: _tmp33=_tmp2A.f1;if((int)_tmp33 == 12){goto _LL7F;}else{goto _LL74;}
_LL7F: _tmp30=_tmp2A.f2;if((unsigned int)_tmp30 > 1?*((int*)_tmp30)== 2: 0){_LL81:
_tmp32=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp30)->f1;goto _LL80;_LL80: _tmp31=((
struct Cyc_Absyn_Int_c_struct*)_tmp30)->f2;goto _LL73;}else{goto _LL74;}_LL74: _LL85:
_tmp36=_tmp2A.f1;if((int)_tmp36 == 11){goto _LL83;}else{goto _LL76;}_LL83: _tmp34=
_tmp2A.f2;if((unsigned int)_tmp34 > 1?*((int*)_tmp34)== 2: 0){_LL84: _tmp35=((
struct Cyc_Absyn_Int_c_struct*)_tmp34)->f2;goto _LL75;}else{goto _LL76;}_LL76: _LL87:
_tmp38=_tmp2A.f1;if((int)_tmp38 == 11){goto _LL86;}else{goto _LL78;}_LL86: _tmp37=
_tmp2A.f2;if((int)_tmp37 == 0){goto _LL77;}else{goto _LL78;}_LL78: goto _LL79;_LL6F:
return cn;_LL71: return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp39=_cycalloc(
sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Absyn_Int_c_struct _tmp3A;_tmp3A.tag=2;
_tmp3A.f1=(void*)((void*)0);_tmp3A.f2=- _tmp2D;_tmp3A;});_tmp39;});_LL73: return(
void*)({struct Cyc_Absyn_Int_c_struct*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({
struct Cyc_Absyn_Int_c_struct _tmp3C;_tmp3C.tag=2;_tmp3C.f1=(void*)((void*)1);
_tmp3C.f2=~ _tmp31;_tmp3C;});_tmp3B;});_LL75: return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_Int_c_struct _tmp3E;
_tmp3E.tag=2;_tmp3E.f1=(void*)((void*)0);_tmp3E.f2=_tmp35 == 0? 1: 0;_tmp3E;});
_tmp3D;});_LL77: return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp3F=_cycalloc(
sizeof(*_tmp3F));_tmp3F[0]=({struct Cyc_Absyn_Int_c_struct _tmp40;_tmp40.tag=2;
_tmp40.f1=(void*)((void*)0);_tmp40.f2=1;_tmp40;});_tmp3F;});_LL79:({void*_tmp41[
0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("bad constant expression",sizeof(
unsigned char),24),_tag_arr(_tmp41,sizeof(void*),0));});return cn;_LL6D:;}struct
_tuple5{void*f1;int f2;};void*Cyc_Evexp_eval_const_binprimop(void*p,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2){void*_tmp42=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(
e1));void*_tmp43=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(e2));void*s1;
void*s2;int i1;int i2;{void*_tmp44=_tmp42;int _tmp45;void*_tmp46;_LL89: if((
unsigned int)_tmp44 > 1?*((int*)_tmp44)== 2: 0){_LL8E: _tmp46=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp44)->f1;goto _LL8D;_LL8D: _tmp45=((struct Cyc_Absyn_Int_c_struct*)_tmp44)->f2;
goto _LL8A;}else{goto _LL8B;}_LL8B: goto _LL8C;_LL8A: s1=_tmp46;i1=_tmp45;goto _LL88;
_LL8C:({void*_tmp47[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("bad constant expression",
sizeof(unsigned char),24),_tag_arr(_tmp47,sizeof(void*),0));});return _tmp42;
_LL88:;}{void*_tmp48=_tmp43;int _tmp49;void*_tmp4A;_LL90: if((unsigned int)_tmp48 > 
1?*((int*)_tmp48)== 2: 0){_LL95: _tmp4A=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp48)->f1;goto _LL94;_LL94: _tmp49=((struct Cyc_Absyn_Int_c_struct*)_tmp48)->f2;
goto _LL91;}else{goto _LL92;}_LL92: goto _LL93;_LL91: s2=_tmp4A;i2=_tmp49;goto _LL8F;
_LL93:({void*_tmp4B[0]={};Cyc_Tcutil_terr(e2->loc,_tag_arr("bad constant expression",
sizeof(unsigned char),24),_tag_arr(_tmp4B,sizeof(void*),0));});return _tmp42;
_LL8F:;}{void*_tmp4C=p;_LL97: if((int)_tmp4C == 3){goto _LL98;}else{goto _LL99;}
_LL99: if((int)_tmp4C == 4){goto _LL9A;}else{goto _LL9B;}_LL9B: goto _LL9C;_LL98: goto
_LL9A;_LL9A: if(i2 == 0){({void*_tmp4D[0]={};Cyc_Tcutil_terr(e2->loc,_tag_arr("division by zero in constant expression",
sizeof(unsigned char),40),_tag_arr(_tmp4D,sizeof(void*),0));});return _tmp42;}
goto _LL96;_LL9C: goto _LL96;_LL96:;}{int has_u_arg=s1 == (void*)1? 1: s2 == (void*)1;
unsigned int u1=(unsigned int)i1;unsigned int u2=(unsigned int)i2;int i3=0;
unsigned int u3=0;int b3=1;int use_i3=0;int use_u3=0;int use_b3=0;{struct _tuple5
_tmp4F=({struct _tuple5 _tmp4E;_tmp4E.f1=p;_tmp4E.f2=has_u_arg;_tmp4E;});int _tmp50;
void*_tmp51;int _tmp52;void*_tmp53;int _tmp54;void*_tmp55;int _tmp56;void*_tmp57;
int _tmp58;void*_tmp59;int _tmp5A;void*_tmp5B;int _tmp5C;void*_tmp5D;int _tmp5E;void*
_tmp5F;int _tmp60;void*_tmp61;int _tmp62;void*_tmp63;void*_tmp64;void*_tmp65;int
_tmp66;void*_tmp67;int _tmp68;void*_tmp69;int _tmp6A;void*_tmp6B;int _tmp6C;void*
_tmp6D;int _tmp6E;void*_tmp6F;int _tmp70;void*_tmp71;int _tmp72;void*_tmp73;int
_tmp74;void*_tmp75;void*_tmp76;void*_tmp77;void*_tmp78;void*_tmp79;void*_tmp7A;
void*_tmp7B;_LL9E: _LLD5: _tmp51=_tmp4F.f1;if((int)_tmp51 == 0){goto _LLD4;}else{
goto _LLA0;}_LLD4: _tmp50=_tmp4F.f2;if(_tmp50 == 0){goto _LL9F;}else{goto _LLA0;}
_LLA0: _LLD7: _tmp53=_tmp4F.f1;if((int)_tmp53 == 1){goto _LLD6;}else{goto _LLA2;}
_LLD6: _tmp52=_tmp4F.f2;if(_tmp52 == 0){goto _LLA1;}else{goto _LLA2;}_LLA2: _LLD9:
_tmp55=_tmp4F.f1;if((int)_tmp55 == 2){goto _LLD8;}else{goto _LLA4;}_LLD8: _tmp54=
_tmp4F.f2;if(_tmp54 == 0){goto _LLA3;}else{goto _LLA4;}_LLA4: _LLDB: _tmp57=_tmp4F.f1;
if((int)_tmp57 == 3){goto _LLDA;}else{goto _LLA6;}_LLDA: _tmp56=_tmp4F.f2;if(_tmp56
== 0){goto _LLA5;}else{goto _LLA6;}_LLA6: _LLDD: _tmp59=_tmp4F.f1;if((int)_tmp59 == 4){
goto _LLDC;}else{goto _LLA8;}_LLDC: _tmp58=_tmp4F.f2;if(_tmp58 == 0){goto _LLA7;}
else{goto _LLA8;}_LLA8: _LLDF: _tmp5B=_tmp4F.f1;if((int)_tmp5B == 0){goto _LLDE;}
else{goto _LLAA;}_LLDE: _tmp5A=_tmp4F.f2;if(_tmp5A == 1){goto _LLA9;}else{goto _LLAA;}
_LLAA: _LLE1: _tmp5D=_tmp4F.f1;if((int)_tmp5D == 1){goto _LLE0;}else{goto _LLAC;}
_LLE0: _tmp5C=_tmp4F.f2;if(_tmp5C == 1){goto _LLAB;}else{goto _LLAC;}_LLAC: _LLE3:
_tmp5F=_tmp4F.f1;if((int)_tmp5F == 2){goto _LLE2;}else{goto _LLAE;}_LLE2: _tmp5E=
_tmp4F.f2;if(_tmp5E == 1){goto _LLAD;}else{goto _LLAE;}_LLAE: _LLE5: _tmp61=_tmp4F.f1;
if((int)_tmp61 == 3){goto _LLE4;}else{goto _LLB0;}_LLE4: _tmp60=_tmp4F.f2;if(_tmp60
== 1){goto _LLAF;}else{goto _LLB0;}_LLB0: _LLE7: _tmp63=_tmp4F.f1;if((int)_tmp63 == 4){
goto _LLE6;}else{goto _LLB2;}_LLE6: _tmp62=_tmp4F.f2;if(_tmp62 == 1){goto _LLB1;}
else{goto _LLB2;}_LLB2: _LLE8: _tmp64=_tmp4F.f1;if((int)_tmp64 == 5){goto _LLB3;}
else{goto _LLB4;}_LLB4: _LLE9: _tmp65=_tmp4F.f1;if((int)_tmp65 == 6){goto _LLB5;}
else{goto _LLB6;}_LLB6: _LLEB: _tmp67=_tmp4F.f1;if((int)_tmp67 == 7){goto _LLEA;}
else{goto _LLB8;}_LLEA: _tmp66=_tmp4F.f2;if(_tmp66 == 0){goto _LLB7;}else{goto _LLB8;}
_LLB8: _LLED: _tmp69=_tmp4F.f1;if((int)_tmp69 == 8){goto _LLEC;}else{goto _LLBA;}
_LLEC: _tmp68=_tmp4F.f2;if(_tmp68 == 0){goto _LLB9;}else{goto _LLBA;}_LLBA: _LLEF:
_tmp6B=_tmp4F.f1;if((int)_tmp6B == 9){goto _LLEE;}else{goto _LLBC;}_LLEE: _tmp6A=
_tmp4F.f2;if(_tmp6A == 0){goto _LLBB;}else{goto _LLBC;}_LLBC: _LLF1: _tmp6D=_tmp4F.f1;
if((int)_tmp6D == 10){goto _LLF0;}else{goto _LLBE;}_LLF0: _tmp6C=_tmp4F.f2;if(_tmp6C
== 0){goto _LLBD;}else{goto _LLBE;}_LLBE: _LLF3: _tmp6F=_tmp4F.f1;if((int)_tmp6F == 7){
goto _LLF2;}else{goto _LLC0;}_LLF2: _tmp6E=_tmp4F.f2;if(_tmp6E == 1){goto _LLBF;}
else{goto _LLC0;}_LLC0: _LLF5: _tmp71=_tmp4F.f1;if((int)_tmp71 == 8){goto _LLF4;}
else{goto _LLC2;}_LLF4: _tmp70=_tmp4F.f2;if(_tmp70 == 1){goto _LLC1;}else{goto _LLC2;}
_LLC2: _LLF7: _tmp73=_tmp4F.f1;if((int)_tmp73 == 9){goto _LLF6;}else{goto _LLC4;}
_LLF6: _tmp72=_tmp4F.f2;if(_tmp72 == 1){goto _LLC3;}else{goto _LLC4;}_LLC4: _LLF9:
_tmp75=_tmp4F.f1;if((int)_tmp75 == 10){goto _LLF8;}else{goto _LLC6;}_LLF8: _tmp74=
_tmp4F.f2;if(_tmp74 == 1){goto _LLC5;}else{goto _LLC6;}_LLC6: _LLFA: _tmp76=_tmp4F.f1;
if((int)_tmp76 == 13){goto _LLC7;}else{goto _LLC8;}_LLC8: _LLFB: _tmp77=_tmp4F.f1;if((
int)_tmp77 == 14){goto _LLC9;}else{goto _LLCA;}_LLCA: _LLFC: _tmp78=_tmp4F.f1;if((int)
_tmp78 == 15){goto _LLCB;}else{goto _LLCC;}_LLCC: _LLFD: _tmp79=_tmp4F.f1;if((int)
_tmp79 == 16){goto _LLCD;}else{goto _LLCE;}_LLCE: _LLFE: _tmp7A=_tmp4F.f1;if((int)
_tmp7A == 17){goto _LLCF;}else{goto _LLD0;}_LLD0: _LLFF: _tmp7B=_tmp4F.f1;if((int)
_tmp7B == 18){goto _LLD1;}else{goto _LLD2;}_LLD2: goto _LLD3;_LL9F: i3=i1 + i2;use_i3=1;
goto _LL9D;_LLA1: i3=i1 * i2;use_i3=1;goto _LL9D;_LLA3: i3=i1 - i2;use_i3=1;goto _LL9D;
_LLA5: i3=i1 / i2;use_i3=1;goto _LL9D;_LLA7: i3=i1 % i2;use_i3=1;goto _LL9D;_LLA9: u3=
u1 + u2;use_u3=1;goto _LL9D;_LLAB: u3=u1 * u2;use_u3=1;goto _LL9D;_LLAD: u3=u1 - u2;
use_u3=1;goto _LL9D;_LLAF: u3=u1 / u2;use_u3=1;goto _LL9D;_LLB1: u3=u1 % u2;use_u3=1;
goto _LL9D;_LLB3: b3=i1 == i2;use_b3=1;goto _LL9D;_LLB5: b3=i1 != i2;use_b3=1;goto
_LL9D;_LLB7: b3=i1 > i2;use_b3=1;goto _LL9D;_LLB9: b3=i1 < i2;use_b3=1;goto _LL9D;
_LLBB: b3=i1 >= i2;use_b3=1;goto _LL9D;_LLBD: b3=i1 <= i2;use_b3=1;goto _LL9D;_LLBF: b3=
u1 > u2;use_b3=1;goto _LL9D;_LLC1: b3=u1 < u2;use_b3=1;goto _LL9D;_LLC3: b3=u1 >= u2;
use_b3=1;goto _LL9D;_LLC5: b3=u1 <= u2;use_b3=1;goto _LL9D;_LLC7: u3=u1 & u2;use_u3=1;
goto _LL9D;_LLC9: u3=u1 | u2;use_u3=1;goto _LL9D;_LLCB: u3=u1 ^ u2;use_u3=1;goto _LL9D;
_LLCD: u3=u1 << u2;use_u3=1;goto _LL9D;_LLCF: u3=u1 >> u2;use_u3=1;goto _LL9D;_LLD1:({
void*_tmp7C[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr(">>> NOT IMPLEMENTED",sizeof(
unsigned char),20),_tag_arr(_tmp7C,sizeof(void*),0));});return _tmp42;_LLD3:({
void*_tmp7D[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("bad constant expression",
sizeof(unsigned char),24),_tag_arr(_tmp7D,sizeof(void*),0));});return _tmp42;
_LL9D:;}if(use_i3){return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp7E=_cycalloc(
sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Absyn_Int_c_struct _tmp7F;_tmp7F.tag=2;
_tmp7F.f1=(void*)((void*)0);_tmp7F.f2=i3;_tmp7F;});_tmp7E;});}if(use_u3){return(
void*)({struct Cyc_Absyn_Int_c_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[0]=({
struct Cyc_Absyn_Int_c_struct _tmp81;_tmp81.tag=2;_tmp81.f1=(void*)((void*)1);
_tmp81.f2=(int)u3;_tmp81;});_tmp80;});}if(use_b3){return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82[0]=({struct Cyc_Absyn_Int_c_struct _tmp83;
_tmp83.tag=2;_tmp83.f1=(void*)((void*)0);_tmp83.f2=b3? 1: 0;_tmp83;});_tmp82;});}({
void*_tmp84[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("Evexp::eval_const_binop",sizeof(unsigned char),24),_tag_arr(_tmp84,
sizeof(void*),0));});}}void*Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){void*
cn;{void*_tmp85=(void*)e->r;void*_tmp86;struct Cyc_Absyn_Exp*_tmp87;struct Cyc_Absyn_Exp*
_tmp88;struct Cyc_Absyn_Exp*_tmp89;struct Cyc_List_List*_tmp8A;void*_tmp8B;struct
Cyc_Absyn_Exp*_tmp8C;void*_tmp8D;struct Cyc_Absyn_Enumfield*_tmp8E;struct Cyc_Absyn_Enumfield*
_tmp8F;_LL101: if(*((int*)_tmp85)== 0){_LL115: _tmp86=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp85)->f1;goto _LL102;}else{goto _LL103;}_LL103: if(*((int*)_tmp85)== 6){_LL118:
_tmp89=((struct Cyc_Absyn_Conditional_e_struct*)_tmp85)->f1;goto _LL117;_LL117:
_tmp88=((struct Cyc_Absyn_Conditional_e_struct*)_tmp85)->f2;goto _LL116;_LL116:
_tmp87=((struct Cyc_Absyn_Conditional_e_struct*)_tmp85)->f3;goto _LL104;}else{goto
_LL105;}_LL105: if(*((int*)_tmp85)== 3){_LL11A: _tmp8B=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp85)->f1;goto _LL119;_LL119: _tmp8A=((struct Cyc_Absyn_Primop_e_struct*)_tmp85)->f2;
goto _LL106;}else{goto _LL107;}_LL107: if(*((int*)_tmp85)== 16){goto _LL108;}else{
goto _LL109;}_LL109: if(*((int*)_tmp85)== 17){goto _LL10A;}else{goto _LL10B;}_LL10B:
if(*((int*)_tmp85)== 18){goto _LL10C;}else{goto _LL10D;}_LL10D: if(*((int*)_tmp85)
== 13){_LL11C: _tmp8D=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp85)->f1;goto
_LL11B;_LL11B: _tmp8C=((struct Cyc_Absyn_Cast_e_struct*)_tmp85)->f2;goto _LL10E;}
else{goto _LL10F;}_LL10F: if(*((int*)_tmp85)== 32){_LL11D: _tmp8E=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp85)->f3;goto _LL110;}else{goto _LL111;}_LL111: if(*((int*)_tmp85)== 31){_LL11E:
_tmp8F=((struct Cyc_Absyn_Enum_e_struct*)_tmp85)->f3;goto _LL112;}else{goto _LL113;}
_LL113: goto _LL114;_LL102: return _tmp86;_LL104: cn=Cyc_Evexp_eval_const_bool_exp(
_tmp89)? Cyc_Evexp_eval_const_exp(_tmp88): Cyc_Evexp_eval_const_exp(_tmp87);goto
_LL100;_LL106: if(_tmp8A == 0){({void*_tmp90[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("bad static expression (no args to primop)",
sizeof(unsigned char),42),_tag_arr(_tmp90,sizeof(void*),0));});return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({
struct Cyc_Absyn_Int_c_struct _tmp92;_tmp92.tag=2;_tmp92.f1=(void*)((void*)0);
_tmp92.f2=0;_tmp92;});_tmp91;});}if(_tmp8A->tl == 0){cn=Cyc_Evexp_eval_const_unprimop(
_tmp8B,(struct Cyc_Absyn_Exp*)_tmp8A->hd);}else{if(((struct Cyc_List_List*)
_check_null(_tmp8A->tl))->tl != 0){({void*_tmp93[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("bad static expression (too many args to primop)",sizeof(unsigned char),
48),_tag_arr(_tmp93,sizeof(void*),0));});return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94[0]=({struct Cyc_Absyn_Int_c_struct _tmp95;
_tmp95.tag=2;_tmp95.f1=(void*)((void*)0);_tmp95.f2=0;_tmp95;});_tmp94;});}else{
cn=Cyc_Evexp_eval_const_binprimop(_tmp8B,(struct Cyc_Absyn_Exp*)_tmp8A->hd,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp8A->tl))->hd);}}goto
_LL100;_LL108: goto _LL10A;_LL10A:({void*_tmp96[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("sizeof(...) is not an integral constant expression in Cyclone",sizeof(
unsigned char),62),_tag_arr(_tmp96,sizeof(void*),0));});return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Absyn_Int_c_struct _tmp98;
_tmp98.tag=2;_tmp98.f1=(void*)((void*)0);_tmp98.f2=0;_tmp98;});_tmp97;});_LL10C:({
void*_tmp99[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("offsetof(...) is not an integral constant expression in Cyclone",
sizeof(unsigned char),64),_tag_arr(_tmp99,sizeof(void*),0));});return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({
struct Cyc_Absyn_Int_c_struct _tmp9B;_tmp9B.tag=2;_tmp9B.f1=(void*)((void*)0);
_tmp9B.f2=0;_tmp9B;});_tmp9A;});_LL10E: cn=Cyc_Evexp_eval_const_exp(_tmp8C);{
struct _tuple4 _tmp9D=({struct _tuple4 _tmp9C;_tmp9C.f1=Cyc_Tcutil_compress(_tmp8D);
_tmp9C.f2=cn;_tmp9C;});void*_tmp9E;unsigned char _tmp9F;void*_tmpA0;void*_tmpA1;
void*_tmpA2;void*_tmpA3;void*_tmpA4;short _tmpA5;void*_tmpA6;void*_tmpA7;void*
_tmpA8;void*_tmpA9;void*_tmpAA;int _tmpAB;void*_tmpAC;void*_tmpAD;void*_tmpAE;
void*_tmpAF;_LL120: _LL12B: _tmpA1=_tmp9D.f1;if((unsigned int)_tmpA1 > 3?*((int*)
_tmpA1)== 5: 0){_LL12D: _tmpA3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpA1)->f1;
goto _LL12C;_LL12C: _tmpA2=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpA1)->f2;
goto _LL128;}else{goto _LL122;}_LL128: _tmp9E=_tmp9D.f2;if((unsigned int)_tmp9E > 1?*((
int*)_tmp9E)== 0: 0){_LL12A: _tmpA0=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp9E)->f1;
goto _LL129;_LL129: _tmp9F=((struct Cyc_Absyn_Char_c_struct*)_tmp9E)->f2;goto _LL121;}
else{goto _LL122;}_LL122: _LL131: _tmpA7=_tmp9D.f1;if((unsigned int)_tmpA7 > 3?*((
int*)_tmpA7)== 5: 0){_LL133: _tmpA9=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpA7)->f1;goto _LL132;_LL132: _tmpA8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpA7)->f2;goto _LL12E;}else{goto _LL124;}_LL12E: _tmpA4=_tmp9D.f2;if((
unsigned int)_tmpA4 > 1?*((int*)_tmpA4)== 1: 0){_LL130: _tmpA6=(void*)((struct Cyc_Absyn_Short_c_struct*)
_tmpA4)->f1;goto _LL12F;_LL12F: _tmpA5=((struct Cyc_Absyn_Short_c_struct*)_tmpA4)->f2;
goto _LL123;}else{goto _LL124;}_LL124: _LL137: _tmpAD=_tmp9D.f1;if((unsigned int)
_tmpAD > 3?*((int*)_tmpAD)== 5: 0){_LL139: _tmpAF=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpAD)->f1;goto _LL138;_LL138: _tmpAE=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpAD)->f2;goto _LL134;}else{goto _LL126;}_LL134: _tmpAA=_tmp9D.f2;if((
unsigned int)_tmpAA > 1?*((int*)_tmpAA)== 2: 0){_LL136: _tmpAC=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmpAA)->f1;goto _LL135;_LL135: _tmpAB=((struct Cyc_Absyn_Int_c_struct*)_tmpAA)->f2;
goto _LL125;}else{goto _LL126;}_LL126: goto _LL127;_LL121: _tmpA9=_tmpA3;_tmpA8=
_tmpA2;_tmpA6=_tmpA0;_tmpA5=(short)_tmp9F;goto _LL123;_LL123: _tmpAF=_tmpA9;_tmpAE=
_tmpA8;_tmpAC=_tmpA6;_tmpAB=(int)_tmpA5;goto _LL125;_LL125: if(_tmpAF != _tmpAC){cn=(
void*)({struct Cyc_Absyn_Int_c_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({
struct Cyc_Absyn_Int_c_struct _tmpB1;_tmpB1.tag=2;_tmpB1.f1=(void*)_tmpAF;_tmpB1.f2=
_tmpAB;_tmpB1;});_tmpB0;});}goto _LL11F;_LL127:({struct Cyc_Std_String_pa_struct
_tmpB3;_tmpB3.tag=0;_tmpB3.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp8D);{
void*_tmpB2[1]={& _tmpB3};Cyc_Tcutil_terr(e->loc,_tag_arr("eval_const: cannot cast to %s",
sizeof(unsigned char),30),_tag_arr(_tmpB2,sizeof(void*),1));}});return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({
struct Cyc_Absyn_Int_c_struct _tmpB5;_tmpB5.tag=2;_tmpB5.f1=(void*)((void*)0);
_tmpB5.f2=0;_tmpB5;});_tmpB4;});_LL11F:;}goto _LL100;_LL110: _tmp8F=_tmp8E;goto
_LL112;_LL112: return Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp8F))->tag));_LL114:({void*_tmpB6[0]={};
Cyc_Tcutil_terr(e->loc,_tag_arr("bad static expression",sizeof(unsigned char),22),
_tag_arr(_tmpB6,sizeof(void*),0));});return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({struct Cyc_Absyn_Int_c_struct _tmpB8;
_tmpB8.tag=2;_tmpB8.f1=(void*)((void*)0);_tmpB8.f2=0;_tmpB8;});_tmpB7;});_LL100:;}(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmpB9=_cycalloc(
sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Absyn_Const_e_struct _tmpBA;_tmpBA.tag=0;
_tmpBA.f1=(void*)cn;_tmpBA;});_tmpB9;})));return cn;}
