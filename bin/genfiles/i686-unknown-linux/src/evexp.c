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
 struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern unsigned char Cyc_List_List_mismatch[
18];extern unsigned char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _tagged_arr
logical_file;struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
unsigned char Cyc_Position_Nocontext[14];struct Cyc_Cstdio___abstractFILE;struct
Cyc_Std___cycFILE;extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};struct _tuple0{void*
f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
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
15];struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;
extern unsigned char Cyc_Dict_Present[12];extern unsigned char Cyc_Dict_Absent[11];
struct _tuple3{void*f1;void*f2;};struct _tuple3*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple3*Cyc_Dict_rchoose(struct
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
le;};void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct _tagged_arr ap);void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*,struct _tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_compress(void*t);unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_okay_szofarg(void*t);void*Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp*e);void*Cyc_Evexp_promote_const(void*cn){void*_tmp0=cn;void*
_tmp1;unsigned char _tmp2;void*_tmp3;short _tmp4;_LL1: if(_tmp0 <= (void*)1?1:*((int*)
_tmp0)!= 0)goto _LL3;_tmp1=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp0)->f1;
_tmp2=((struct Cyc_Absyn_Char_c_struct*)_tmp0)->f2;_LL2: return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=({struct Cyc_Absyn_Int_c_struct _tmp6;
_tmp6.tag=2;_tmp6.f1=(void*)_tmp1;_tmp6.f2=(int)_tmp2;_tmp6;});_tmp5;});_LL3: if(
_tmp0 <= (void*)1?1:*((int*)_tmp0)!= 1)goto _LL5;_tmp3=(void*)((struct Cyc_Absyn_Short_c_struct*)
_tmp0)->f1;_tmp4=((struct Cyc_Absyn_Short_c_struct*)_tmp0)->f2;_LL4: return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=({struct
Cyc_Absyn_Int_c_struct _tmp8;_tmp8.tag=2;_tmp8.f1=(void*)_tmp3;_tmp8.f2=(int)
_tmp4;_tmp8;});_tmp7;});_LL5:;_LL6: return cn;_LL0:;}unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e){void*_tmp9=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(
e));int _tmpA;_LL8: if(_tmp9 <= (void*)1?1:*((int*)_tmp9)!= 2)goto _LLA;_tmpA=((
struct Cyc_Absyn_Int_c_struct*)_tmp9)->f2;_LL9: return(unsigned int)_tmpA;_LLA:;
_LLB:({void*_tmpB[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("expecting unsigned int",
sizeof(unsigned char),23),_tag_arr(_tmpB,sizeof(void*),0));});return 0;_LL7:;}int
Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){void*_tmpC=Cyc_Evexp_promote_const(
Cyc_Evexp_eval_const_exp(e));int _tmpD;long long _tmpE;_LLD: if(_tmpC <= (void*)1?1:*((
int*)_tmpC)!= 2)goto _LLF;_tmpD=((struct Cyc_Absyn_Int_c_struct*)_tmpC)->f2;_LLE:
return _tmpD != 0;_LLF: if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 3)goto _LL11;_tmpE=((
struct Cyc_Absyn_LongLong_c_struct*)_tmpC)->f2;_LL10: return _tmpE != 0;_LL11: if((
int)_tmpC != 0)goto _LL13;_LL12: return 0;_LL13:;_LL14:({void*_tmpF[0]={};Cyc_Tcutil_terr(
e->loc,_tag_arr("expecting bool",sizeof(unsigned char),15),_tag_arr(_tmpF,
sizeof(void*),0));});return 0;_LLC:;}struct _tuple4{struct Cyc_Absyn_Tqual f1;void*
f2;};int Cyc_Evexp_okay_szofarg(void*t){void*_tmp10=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_Tvar*_tmp11;struct Cyc_Core_Opt*_tmp12;struct Cyc_Core_Opt _tmp13;void*
_tmp14;struct Cyc_Absyn_TunionFieldInfo _tmp15;void*_tmp16;struct Cyc_Absyn_Tunionfield*
_tmp17;struct Cyc_List_List*_tmp18;struct Cyc_Absyn_PtrInfo _tmp19;struct Cyc_Absyn_Conref*
_tmp1A;void*_tmp1B;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_AggrInfo _tmp1D;
void*_tmp1E;struct Cyc_List_List*_tmp1F;_LL16: if((int)_tmp10 != 0)goto _LL18;_LL17:
return 0;_LL18: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 1)goto _LL1A;_tmp11=((
struct Cyc_Absyn_VarType_struct*)_tmp10)->f1;_LL19: {void*_tmp20=Cyc_Tcutil_tvar_kind(
_tmp11);_LL49: if((int)_tmp20 != 2)goto _LL4B;_LL4A: return 1;_LL4B:;_LL4C: return 0;
_LL48:;}_LL1A: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 0)goto _LL1C;_tmp12=((
struct Cyc_Absyn_Evar_struct*)_tmp10)->f1;if(_tmp12 == 0)goto _LL1C;_tmp13=*_tmp12;
_tmp14=(void*)_tmp13.v;if((int)_tmp14 != 2)goto _LL1C;_LL1B: return 1;_LL1C: if(
_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 0)goto _LL1E;_LL1D: return 0;_LL1E: if(_tmp10
<= (void*)3?1:*((int*)_tmp10)!= 2)goto _LL20;_LL1F: return 1;_LL20: if(_tmp10 <= (
void*)3?1:*((int*)_tmp10)!= 3)goto _LL22;_tmp15=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp10)->f1;_tmp16=(void*)_tmp15.field_info;if(*((int*)_tmp16)!= 1)goto _LL22;
_tmp17=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp16)->f2;_LL21: _tmp18=
_tmp17->typs;goto _LL23;_LL22: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 9)goto
_LL24;_tmp18=((struct Cyc_Absyn_TupleType_struct*)_tmp10)->f1;_LL23: for(0;_tmp18
!= 0;_tmp18=_tmp18->tl){if(!Cyc_Evexp_okay_szofarg((*((struct _tuple4*)_tmp18->hd)).f2))
return 0;}return 1;_LL24: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 3)goto _LL26;
_LL25: return 0;_LL26: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 4)goto _LL28;_tmp19=((
struct Cyc_Absyn_PointerType_struct*)_tmp10)->f1;_tmp1A=_tmp19.bounds;_LL27: {
void*_tmp21=(void*)(Cyc_Absyn_compress_conref(_tmp1A))->v;_LL4E: if(_tmp21 <= (
void*)1?1:*((int*)_tmp21)!= 0)goto _LL50;_LL4F: return 1;_LL50:;_LL51: return 0;_LL4D:;}
_LL28: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 14)goto _LL2A;_LL29: goto _LL2B;
_LL2A: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 5)goto _LL2C;_LL2B: goto _LL2D;_LL2C:
if((int)_tmp10 != 1)goto _LL2E;_LL2D: goto _LL2F;_LL2E: if(_tmp10 <= (void*)3?1:*((int*)
_tmp10)!= 6)goto _LL30;_LL2F: return 1;_LL30: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)
!= 7)goto _LL32;_tmp1B=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp10)->f1;
_tmp1C=((struct Cyc_Absyn_ArrayType_struct*)_tmp10)->f3;_LL31: return _tmp1C != 0;
_LL32: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 8)goto _LL34;_LL33: return 0;_LL34:
if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 15)goto _LL36;_LL35: return 1;_LL36: if(
_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 10)goto _LL38;_tmp1D=((struct Cyc_Absyn_AggrType_struct*)
_tmp10)->f1;_tmp1E=(void*)_tmp1D.aggr_info;_LL37: {struct Cyc_Absyn_Aggrdecl*ad;{
void*_tmp22=_tmp1E;struct Cyc_Absyn_Aggrdecl**_tmp23;struct Cyc_Absyn_Aggrdecl*
_tmp24;_LL53: if(*((int*)_tmp22)!= 0)goto _LL55;_LL54:({void*_tmp25[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("szof on unchecked StructType",
sizeof(unsigned char),29),_tag_arr(_tmp25,sizeof(void*),0));});_LL55: if(*((int*)
_tmp22)!= 1)goto _LL52;_tmp23=((struct Cyc_Absyn_KnownAggr_struct*)_tmp22)->f1;
_tmp24=*_tmp23;_LL56: ad=_tmp24;_LL52:;}if(ad->fields == 0)return 0;_tmp1F=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ad->fields))->v;goto _LL39;}
_LL38: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 11)goto _LL3A;_tmp1F=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp10)->f2;_LL39: for(0;_tmp1F != 0;_tmp1F=_tmp1F->tl){if(!Cyc_Evexp_okay_szofarg((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp1F->hd)->type))return 0;}return 1;_LL3A: if(
_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 12)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if(
_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 13)goto _LL3E;_LL3D: return 1;_LL3E: if((int)
_tmp10 != 2)goto _LL40;_LL3F: goto _LL41;_LL40: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)
!= 18)goto _LL42;_LL41: goto _LL43;_LL42: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 
17)goto _LL44;_LL43: goto _LL45;_LL44: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 19)
goto _LL46;_LL45: return 0;_LL46: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 16)goto
_LL15;_LL47:({void*_tmp26[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr
ap))Cyc_Tcutil_impos)((struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp28;
_tmp28.tag=0;_tmp28.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp27[
1]={& _tmp28};Cyc_Std_aprintf(_tag_arr("szof typedeftype %s",sizeof(unsigned char),
20),_tag_arr(_tmp27,sizeof(void*),1));}}),_tag_arr(_tmp26,sizeof(void*),0));});
_LL15:;}void*Cyc_Evexp_eval_const_unprimop(void*p,struct Cyc_Absyn_Exp*e){void*cn=
Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(e));struct _tuple3 _tmp2A=({
struct _tuple3 _tmp29;_tmp29.f1=p;_tmp29.f2=cn;_tmp29;});void*_tmp2B;void*_tmp2C;
void*_tmp2D;void*_tmp2E;int _tmp2F;void*_tmp30;void*_tmp31;void*_tmp32;int _tmp33;
void*_tmp34;void*_tmp35;int _tmp36;void*_tmp37;void*_tmp38;_LL58: _tmp2B=_tmp2A.f1;
if((int)_tmp2B != 0)goto _LL5A;_LL59: return cn;_LL5A: _tmp2C=_tmp2A.f1;if((int)
_tmp2C != 2)goto _LL5C;_tmp2D=_tmp2A.f2;if(_tmp2D <= (void*)1?1:*((int*)_tmp2D)!= 2)
goto _LL5C;_tmp2E=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp2D)->f1;_tmp2F=((
struct Cyc_Absyn_Int_c_struct*)_tmp2D)->f2;_LL5B: return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Absyn_Int_c_struct _tmp3A;
_tmp3A.tag=2;_tmp3A.f1=(void*)((void*)0);_tmp3A.f2=- _tmp2F;_tmp3A;});_tmp39;});
_LL5C: _tmp30=_tmp2A.f1;if((int)_tmp30 != 12)goto _LL5E;_tmp31=_tmp2A.f2;if(_tmp31
<= (void*)1?1:*((int*)_tmp31)!= 2)goto _LL5E;_tmp32=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp31)->f1;_tmp33=((struct Cyc_Absyn_Int_c_struct*)_tmp31)->f2;_LL5D: return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({
struct Cyc_Absyn_Int_c_struct _tmp3C;_tmp3C.tag=2;_tmp3C.f1=(void*)((void*)1);
_tmp3C.f2=~ _tmp33;_tmp3C;});_tmp3B;});_LL5E: _tmp34=_tmp2A.f1;if((int)_tmp34 != 11)
goto _LL60;_tmp35=_tmp2A.f2;if(_tmp35 <= (void*)1?1:*((int*)_tmp35)!= 2)goto _LL60;
_tmp36=((struct Cyc_Absyn_Int_c_struct*)_tmp35)->f2;_LL5F: return(void*)({struct
Cyc_Absyn_Int_c_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_Int_c_struct
_tmp3E;_tmp3E.tag=2;_tmp3E.f1=(void*)((void*)0);_tmp3E.f2=_tmp36 == 0?1: 0;_tmp3E;});
_tmp3D;});_LL60: _tmp37=_tmp2A.f1;if((int)_tmp37 != 11)goto _LL62;_tmp38=_tmp2A.f2;
if((int)_tmp38 != 0)goto _LL62;_LL61: return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F[0]=({struct Cyc_Absyn_Int_c_struct _tmp40;
_tmp40.tag=2;_tmp40.f1=(void*)((void*)0);_tmp40.f2=1;_tmp40;});_tmp3F;});_LL62:;
_LL63:({void*_tmp41[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("bad constant expression",
sizeof(unsigned char),24),_tag_arr(_tmp41,sizeof(void*),0));});return cn;_LL57:;}
struct _tuple5{void*f1;int f2;};void*Cyc_Evexp_eval_const_binprimop(void*p,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp42=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(
e1));void*_tmp43=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(e2));void*s1;
void*s2;int i1;int i2;{void*_tmp44=_tmp42;void*_tmp45;int _tmp46;_LL65: if(_tmp44 <= (
void*)1?1:*((int*)_tmp44)!= 2)goto _LL67;_tmp45=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp44)->f1;_tmp46=((struct Cyc_Absyn_Int_c_struct*)_tmp44)->f2;_LL66: s1=_tmp45;
i1=_tmp46;goto _LL64;_LL67:;_LL68:({void*_tmp47[0]={};Cyc_Tcutil_terr(e1->loc,
_tag_arr("bad constant expression",sizeof(unsigned char),24),_tag_arr(_tmp47,
sizeof(void*),0));});return _tmp42;_LL64:;}{void*_tmp48=_tmp43;void*_tmp49;int
_tmp4A;_LL6A: if(_tmp48 <= (void*)1?1:*((int*)_tmp48)!= 2)goto _LL6C;_tmp49=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp48)->f1;_tmp4A=((struct Cyc_Absyn_Int_c_struct*)
_tmp48)->f2;_LL6B: s2=_tmp49;i2=_tmp4A;goto _LL69;_LL6C:;_LL6D:({void*_tmp4B[0]={};
Cyc_Tcutil_terr(e2->loc,_tag_arr("bad constant expression",sizeof(unsigned char),
24),_tag_arr(_tmp4B,sizeof(void*),0));});return _tmp42;_LL69:;}{void*_tmp4C=p;
_LL6F: if((int)_tmp4C != 3)goto _LL71;_LL70: goto _LL72;_LL71: if((int)_tmp4C != 4)goto
_LL73;_LL72: if(i2 == 0){({void*_tmp4D[0]={};Cyc_Tcutil_terr(e2->loc,_tag_arr("division by zero in constant expression",
sizeof(unsigned char),40),_tag_arr(_tmp4D,sizeof(void*),0));});return _tmp42;}
goto _LL6E;_LL73:;_LL74: goto _LL6E;_LL6E:;}{int has_u_arg=s1 == (void*)1?1: s2 == (
void*)1;unsigned int u1=(unsigned int)i1;unsigned int u2=(unsigned int)i2;int i3=0;
unsigned int u3=0;int b3=1;int use_i3=0;int use_u3=0;int use_b3=0;{struct _tuple5
_tmp4F=({struct _tuple5 _tmp4E;_tmp4E.f1=p;_tmp4E.f2=has_u_arg;_tmp4E;});void*
_tmp50;int _tmp51;void*_tmp52;int _tmp53;void*_tmp54;int _tmp55;void*_tmp56;int
_tmp57;void*_tmp58;int _tmp59;void*_tmp5A;int _tmp5B;void*_tmp5C;int _tmp5D;void*
_tmp5E;int _tmp5F;void*_tmp60;int _tmp61;void*_tmp62;int _tmp63;void*_tmp64;void*
_tmp65;void*_tmp66;int _tmp67;void*_tmp68;int _tmp69;void*_tmp6A;int _tmp6B;void*
_tmp6C;int _tmp6D;void*_tmp6E;int _tmp6F;void*_tmp70;int _tmp71;void*_tmp72;int
_tmp73;void*_tmp74;int _tmp75;void*_tmp76;void*_tmp77;void*_tmp78;void*_tmp79;
void*_tmp7A;void*_tmp7B;_LL76: _tmp50=_tmp4F.f1;if((int)_tmp50 != 0)goto _LL78;
_tmp51=_tmp4F.f2;if(_tmp51 != 0)goto _LL78;_LL77: i3=i1 + i2;use_i3=1;goto _LL75;
_LL78: _tmp52=_tmp4F.f1;if((int)_tmp52 != 1)goto _LL7A;_tmp53=_tmp4F.f2;if(_tmp53 != 
0)goto _LL7A;_LL79: i3=i1 * i2;use_i3=1;goto _LL75;_LL7A: _tmp54=_tmp4F.f1;if((int)
_tmp54 != 2)goto _LL7C;_tmp55=_tmp4F.f2;if(_tmp55 != 0)goto _LL7C;_LL7B: i3=i1 - i2;
use_i3=1;goto _LL75;_LL7C: _tmp56=_tmp4F.f1;if((int)_tmp56 != 3)goto _LL7E;_tmp57=
_tmp4F.f2;if(_tmp57 != 0)goto _LL7E;_LL7D: i3=i1 / i2;use_i3=1;goto _LL75;_LL7E:
_tmp58=_tmp4F.f1;if((int)_tmp58 != 4)goto _LL80;_tmp59=_tmp4F.f2;if(_tmp59 != 0)
goto _LL80;_LL7F: i3=i1 % i2;use_i3=1;goto _LL75;_LL80: _tmp5A=_tmp4F.f1;if((int)
_tmp5A != 0)goto _LL82;_tmp5B=_tmp4F.f2;if(_tmp5B != 1)goto _LL82;_LL81: u3=u1 + u2;
use_u3=1;goto _LL75;_LL82: _tmp5C=_tmp4F.f1;if((int)_tmp5C != 1)goto _LL84;_tmp5D=
_tmp4F.f2;if(_tmp5D != 1)goto _LL84;_LL83: u3=u1 * u2;use_u3=1;goto _LL75;_LL84:
_tmp5E=_tmp4F.f1;if((int)_tmp5E != 2)goto _LL86;_tmp5F=_tmp4F.f2;if(_tmp5F != 1)
goto _LL86;_LL85: u3=u1 - u2;use_u3=1;goto _LL75;_LL86: _tmp60=_tmp4F.f1;if((int)
_tmp60 != 3)goto _LL88;_tmp61=_tmp4F.f2;if(_tmp61 != 1)goto _LL88;_LL87: u3=u1 / u2;
use_u3=1;goto _LL75;_LL88: _tmp62=_tmp4F.f1;if((int)_tmp62 != 4)goto _LL8A;_tmp63=
_tmp4F.f2;if(_tmp63 != 1)goto _LL8A;_LL89: u3=u1 % u2;use_u3=1;goto _LL75;_LL8A:
_tmp64=_tmp4F.f1;if((int)_tmp64 != 5)goto _LL8C;_LL8B: b3=i1 == i2;use_b3=1;goto
_LL75;_LL8C: _tmp65=_tmp4F.f1;if((int)_tmp65 != 6)goto _LL8E;_LL8D: b3=i1 != i2;
use_b3=1;goto _LL75;_LL8E: _tmp66=_tmp4F.f1;if((int)_tmp66 != 7)goto _LL90;_tmp67=
_tmp4F.f2;if(_tmp67 != 0)goto _LL90;_LL8F: b3=i1 > i2;use_b3=1;goto _LL75;_LL90:
_tmp68=_tmp4F.f1;if((int)_tmp68 != 8)goto _LL92;_tmp69=_tmp4F.f2;if(_tmp69 != 0)
goto _LL92;_LL91: b3=i1 < i2;use_b3=1;goto _LL75;_LL92: _tmp6A=_tmp4F.f1;if((int)
_tmp6A != 9)goto _LL94;_tmp6B=_tmp4F.f2;if(_tmp6B != 0)goto _LL94;_LL93: b3=i1 >= i2;
use_b3=1;goto _LL75;_LL94: _tmp6C=_tmp4F.f1;if((int)_tmp6C != 10)goto _LL96;_tmp6D=
_tmp4F.f2;if(_tmp6D != 0)goto _LL96;_LL95: b3=i1 <= i2;use_b3=1;goto _LL75;_LL96:
_tmp6E=_tmp4F.f1;if((int)_tmp6E != 7)goto _LL98;_tmp6F=_tmp4F.f2;if(_tmp6F != 1)
goto _LL98;_LL97: b3=u1 > u2;use_b3=1;goto _LL75;_LL98: _tmp70=_tmp4F.f1;if((int)
_tmp70 != 8)goto _LL9A;_tmp71=_tmp4F.f2;if(_tmp71 != 1)goto _LL9A;_LL99: b3=u1 < u2;
use_b3=1;goto _LL75;_LL9A: _tmp72=_tmp4F.f1;if((int)_tmp72 != 9)goto _LL9C;_tmp73=
_tmp4F.f2;if(_tmp73 != 1)goto _LL9C;_LL9B: b3=u1 >= u2;use_b3=1;goto _LL75;_LL9C:
_tmp74=_tmp4F.f1;if((int)_tmp74 != 10)goto _LL9E;_tmp75=_tmp4F.f2;if(_tmp75 != 1)
goto _LL9E;_LL9D: b3=u1 <= u2;use_b3=1;goto _LL75;_LL9E: _tmp76=_tmp4F.f1;if((int)
_tmp76 != 13)goto _LLA0;_LL9F: u3=u1 & u2;use_u3=1;goto _LL75;_LLA0: _tmp77=_tmp4F.f1;
if((int)_tmp77 != 14)goto _LLA2;_LLA1: u3=u1 | u2;use_u3=1;goto _LL75;_LLA2: _tmp78=
_tmp4F.f1;if((int)_tmp78 != 15)goto _LLA4;_LLA3: u3=u1 ^ u2;use_u3=1;goto _LL75;_LLA4:
_tmp79=_tmp4F.f1;if((int)_tmp79 != 16)goto _LLA6;_LLA5: u3=u1 << u2;use_u3=1;goto
_LL75;_LLA6: _tmp7A=_tmp4F.f1;if((int)_tmp7A != 17)goto _LLA8;_LLA7: u3=u1 >> u2;
use_u3=1;goto _LL75;_LLA8: _tmp7B=_tmp4F.f1;if((int)_tmp7B != 18)goto _LLAA;_LLA9:({
void*_tmp7C[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr(">>> NOT IMPLEMENTED",sizeof(
unsigned char),20),_tag_arr(_tmp7C,sizeof(void*),0));});return _tmp42;_LLAA:;
_LLAB:({void*_tmp7D[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("bad constant expression",
sizeof(unsigned char),24),_tag_arr(_tmp7D,sizeof(void*),0));});return _tmp42;
_LL75:;}if(use_i3)return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp7E=_cycalloc(
sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Absyn_Int_c_struct _tmp7F;_tmp7F.tag=2;
_tmp7F.f1=(void*)((void*)0);_tmp7F.f2=i3;_tmp7F;});_tmp7E;});if(use_u3)return(
void*)({struct Cyc_Absyn_Int_c_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80[0]=({
struct Cyc_Absyn_Int_c_struct _tmp81;_tmp81.tag=2;_tmp81.f1=(void*)((void*)1);
_tmp81.f2=(int)u3;_tmp81;});_tmp80;});if(use_b3)return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82[0]=({struct Cyc_Absyn_Int_c_struct _tmp83;
_tmp83.tag=2;_tmp83.f1=(void*)((void*)0);_tmp83.f2=b3?1: 0;_tmp83;});_tmp82;});({
void*_tmp84[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(
_tag_arr("Evexp::eval_const_binop",sizeof(unsigned char),24),_tag_arr(_tmp84,
sizeof(void*),0));});}}void*Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){void*
cn;{void*_tmp85=(void*)e->r;void*_tmp86;struct Cyc_Absyn_Exp*_tmp87;struct Cyc_Absyn_Exp*
_tmp88;struct Cyc_Absyn_Exp*_tmp89;void*_tmp8A;struct Cyc_List_List*_tmp8B;void*
_tmp8C;struct Cyc_Absyn_Exp*_tmp8D;struct Cyc_Absyn_Enumfield*_tmp8E;struct Cyc_Absyn_Enumfield*
_tmp8F;_LLAD: if(*((int*)_tmp85)!= 0)goto _LLAF;_tmp86=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp85)->f1;_LLAE: return _tmp86;_LLAF: if(*((int*)_tmp85)!= 6)goto _LLB1;_tmp87=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp85)->f1;_tmp88=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp85)->f2;_tmp89=((struct Cyc_Absyn_Conditional_e_struct*)_tmp85)->f3;_LLB0: cn=
Cyc_Evexp_eval_const_bool_exp(_tmp87)?Cyc_Evexp_eval_const_exp(_tmp88): Cyc_Evexp_eval_const_exp(
_tmp89);goto _LLAC;_LLB1: if(*((int*)_tmp85)!= 3)goto _LLB3;_tmp8A=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmp85)->f1;_tmp8B=((struct Cyc_Absyn_Primop_e_struct*)
_tmp85)->f2;_LLB2: if(_tmp8B == 0){({void*_tmp90[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("bad static expression (no args to primop)",sizeof(unsigned char),42),
_tag_arr(_tmp90,sizeof(void*),0));});return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({struct Cyc_Absyn_Int_c_struct _tmp92;
_tmp92.tag=2;_tmp92.f1=(void*)((void*)0);_tmp92.f2=0;_tmp92;});_tmp91;});}if(
_tmp8B->tl == 0)cn=Cyc_Evexp_eval_const_unprimop(_tmp8A,(struct Cyc_Absyn_Exp*)
_tmp8B->hd);else{if(((struct Cyc_List_List*)_check_null(_tmp8B->tl))->tl != 0){({
void*_tmp93[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("bad static expression (too many args to primop)",
sizeof(unsigned char),48),_tag_arr(_tmp93,sizeof(void*),0));});return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94[0]=({
struct Cyc_Absyn_Int_c_struct _tmp95;_tmp95.tag=2;_tmp95.f1=(void*)((void*)0);
_tmp95.f2=0;_tmp95;});_tmp94;});}else{cn=Cyc_Evexp_eval_const_binprimop(_tmp8A,(
struct Cyc_Absyn_Exp*)_tmp8B->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp8B->tl))->hd);}}goto _LLAC;_LLB3: if(*((int*)_tmp85)!= 16)goto
_LLB5;_LLB4: goto _LLB6;_LLB5: if(*((int*)_tmp85)!= 17)goto _LLB7;_LLB6:({void*
_tmp96[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("sizeof(...) is not an integral constant expression in Cyclone",
sizeof(unsigned char),62),_tag_arr(_tmp96,sizeof(void*),0));});return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=({
struct Cyc_Absyn_Int_c_struct _tmp98;_tmp98.tag=2;_tmp98.f1=(void*)((void*)0);
_tmp98.f2=0;_tmp98;});_tmp97;});_LLB7: if(*((int*)_tmp85)!= 18)goto _LLB9;_LLB8:({
void*_tmp99[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("offsetof(...) is not an integral constant expression in Cyclone",
sizeof(unsigned char),64),_tag_arr(_tmp99,sizeof(void*),0));});return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A[0]=({
struct Cyc_Absyn_Int_c_struct _tmp9B;_tmp9B.tag=2;_tmp9B.f1=(void*)((void*)0);
_tmp9B.f2=0;_tmp9B;});_tmp9A;});_LLB9: if(*((int*)_tmp85)!= 13)goto _LLBB;_tmp8C=(
void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp85)->f1;_tmp8D=((struct Cyc_Absyn_Cast_e_struct*)
_tmp85)->f2;_LLBA: cn=Cyc_Evexp_eval_const_exp(_tmp8D);{struct _tuple3 _tmp9D=({
struct _tuple3 _tmp9C;_tmp9C.f1=Cyc_Tcutil_compress(_tmp8C);_tmp9C.f2=cn;_tmp9C;});
void*_tmp9E;void*_tmp9F;void*_tmpA0;void*_tmpA1;void*_tmpA2;unsigned char _tmpA3;
void*_tmpA4;void*_tmpA5;void*_tmpA6;void*_tmpA7;void*_tmpA8;short _tmpA9;void*
_tmpAA;void*_tmpAB;void*_tmpAC;void*_tmpAD;void*_tmpAE;int _tmpAF;_LLC2: _tmp9E=
_tmp9D.f1;if(_tmp9E <= (void*)3?1:*((int*)_tmp9E)!= 5)goto _LLC4;_tmp9F=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp9E)->f1;_tmpA0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp9E)->f2;_tmpA1=_tmp9D.f2;if(_tmpA1 <= (void*)1?1:*((int*)_tmpA1)!= 0)goto
_LLC4;_tmpA2=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmpA1)->f1;_tmpA3=((struct
Cyc_Absyn_Char_c_struct*)_tmpA1)->f2;_LLC3: _tmpA5=_tmp9F;_tmpA6=_tmpA0;_tmpA8=
_tmpA2;_tmpA9=(short)_tmpA3;goto _LLC5;_LLC4: _tmpA4=_tmp9D.f1;if(_tmpA4 <= (void*)
3?1:*((int*)_tmpA4)!= 5)goto _LLC6;_tmpA5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpA4)->f1;_tmpA6=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpA4)->f2;_tmpA7=
_tmp9D.f2;if(_tmpA7 <= (void*)1?1:*((int*)_tmpA7)!= 1)goto _LLC6;_tmpA8=(void*)((
struct Cyc_Absyn_Short_c_struct*)_tmpA7)->f1;_tmpA9=((struct Cyc_Absyn_Short_c_struct*)
_tmpA7)->f2;_LLC5: _tmpAB=_tmpA5;_tmpAC=_tmpA6;_tmpAE=_tmpA8;_tmpAF=(int)_tmpA9;
goto _LLC7;_LLC6: _tmpAA=_tmp9D.f1;if(_tmpAA <= (void*)3?1:*((int*)_tmpAA)!= 5)goto
_LLC8;_tmpAB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpAA)->f1;_tmpAC=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpAA)->f2;_tmpAD=_tmp9D.f2;if(_tmpAD <= (void*)
1?1:*((int*)_tmpAD)!= 2)goto _LLC8;_tmpAE=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmpAD)->f1;_tmpAF=((struct Cyc_Absyn_Int_c_struct*)_tmpAD)->f2;_LLC7: if(_tmpAB != 
_tmpAE)cn=(void*)({struct Cyc_Absyn_Int_c_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));
_tmpB0[0]=({struct Cyc_Absyn_Int_c_struct _tmpB1;_tmpB1.tag=2;_tmpB1.f1=(void*)
_tmpAB;_tmpB1.f2=_tmpAF;_tmpB1;});_tmpB0;});goto _LLC1;_LLC8:;_LLC9:({struct Cyc_Std_String_pa_struct
_tmpB3;_tmpB3.tag=0;_tmpB3.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp8C);{
void*_tmpB2[1]={& _tmpB3};Cyc_Tcutil_terr(e->loc,_tag_arr("eval_const: cannot cast to %s",
sizeof(unsigned char),30),_tag_arr(_tmpB2,sizeof(void*),1));}});return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({
struct Cyc_Absyn_Int_c_struct _tmpB5;_tmpB5.tag=2;_tmpB5.f1=(void*)((void*)0);
_tmpB5.f2=0;_tmpB5;});_tmpB4;});_LLC1:;}goto _LLAC;_LLBB: if(*((int*)_tmp85)!= 32)
goto _LLBD;_tmp8E=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp85)->f3;_LLBC: _tmp8F=
_tmp8E;goto _LLBE;_LLBD: if(*((int*)_tmp85)!= 31)goto _LLBF;_tmp8F=((struct Cyc_Absyn_Enum_e_struct*)
_tmp85)->f3;_LLBE: return Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)
_check_null(((struct Cyc_Absyn_Enumfield*)_check_null(_tmp8F))->tag));_LLBF:;
_LLC0:({void*_tmpB6[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("bad static expression",
sizeof(unsigned char),22),_tag_arr(_tmpB6,sizeof(void*),0));});return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({
struct Cyc_Absyn_Int_c_struct _tmpB8;_tmpB8.tag=2;_tmpB8.f1=(void*)((void*)0);
_tmpB8.f2=0;_tmpB8;});_tmpB7;});_LLAC:;}(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Absyn_Const_e_struct
_tmpBA;_tmpBA.tag=0;_tmpBA.f1=(void*)cn;_tmpBA;});_tmpB9;})));return cn;}
