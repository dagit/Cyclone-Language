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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___abstractFILE;struct
Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stdout;int Cyc_Std_fputc(
int __c,struct Cyc_Std___cycFILE*__stream);extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};int Cyc_Std_file_string_write(struct Cyc_Std___cycFILE*
fd,struct _tagged_arr src,int src_offset,int max_count);struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};int Cyc_Std_fprintf(
struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr);struct
_tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern unsigned char Cyc_List_Nth[8];
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};extern
unsigned char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _tagged_arr source;struct Cyc_Position_Segment*seg;void*kind;struct
_tagged_arr desc;};extern unsigned char Cyc_Position_Nocontext[14];struct
_tagged_arr Cyc_Position_get_line_directive(struct Cyc_Position_Segment*loc);
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
15];void*Cyc_Absyn_compress_kb(void*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct _tagged_arr Cyc_Absyn_attribute2string(
void*);struct _tuple3{void*f1;struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(
void*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);int Cyc_Absynpp_is_anon_aggrtype(
void*t);extern struct _tagged_arr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*);struct _tagged_arr Cyc_Absynpp_char_escape(unsigned char);
struct _tagged_arr Cyc_Absynpp_string_escape(struct _tagged_arr);struct _tagged_arr
Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*
s);struct _tuple4{struct _tagged_arr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct
_tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple4*arg);struct _tuple5{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;};struct _tuple5 Cyc_Absynpp_to_tms(struct Cyc_Absyn_Tqual
tq,void*t);unsigned int Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[
11];struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern
unsigned char Cyc_Dict_Absent[11];struct _tuple6{void*f1;void*f2;};struct _tuple6*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);struct _tuple6*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict*d);struct Cyc_Tcenv_VarRes_struct{int tag;
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
le;};void*Cyc_Tcutil_compress(void*t);static int Cyc_Absyndump_expand_typedefs;
static int Cyc_Absyndump_qvar_to_Cids;static int Cyc_Absyndump_add_cyc_prefix;static
int Cyc_Absyndump_generate_line_directives;static int Cyc_Absyndump_to_VC;void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs){Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
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
_LL6;case ';': _LL6: goto _LL7;case ',': _LL7: goto _LL8;case '=': _LL8: goto _LL9;case '?':
_LL9: goto _LLA;case '!': _LLA: goto _LLB;case ' ': _LLB: goto _LLC;case '\n': _LLC: goto _LLD;
case '*': _LLD: return 0;default: _LLE: return 1;}}void Cyc_Absyndump_dump(struct
_tagged_arr s){int sz=(int)_get_arr_size(s,sizeof(unsigned char));if(!((int)*((
const unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),sz - 1))))--
sz;Cyc_Absyndump_pos +=sz;if(Cyc_Absyndump_pos > 80){Cyc_Absyndump_pos=(
unsigned int)sz;Cyc_Std_fputc((int)'\n',*Cyc_Absyndump_dump_file);}else{if(Cyc_Absyndump_need_space_before())
Cyc_Std_fputc((int)' ',*Cyc_Absyndump_dump_file);}Cyc_Std_file_string_write(*Cyc_Absyndump_dump_file,
s,0,sz);if(sz >= 1)Cyc_Absyndump_prev_char=*((const unsigned char*)
_check_unknown_subscript(s,sizeof(unsigned char),sz - 1));}void Cyc_Absyndump_dump_nospace(
struct _tagged_arr s){int sz=(int)_get_arr_size(s,sizeof(unsigned char));if(!((int)*((
const unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),sz - 1))))--
sz;Cyc_Absyndump_pos +=sz;Cyc_Std_file_string_write(*Cyc_Absyndump_dump_file,s,0,
sz);if(sz >= 1)Cyc_Absyndump_prev_char=*((const unsigned char*)
_check_unknown_subscript(s,sizeof(unsigned char),sz - 1));}void Cyc_Absyndump_dump_char(
int c){++ Cyc_Absyndump_pos;Cyc_Std_fputc(c,*Cyc_Absyndump_dump_file);Cyc_Absyndump_prev_char=(
unsigned char)c;}void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*loc){if(
loc == 0)return;if(!Cyc_Absyndump_generate_line_directives)return;{struct
_tagged_arr _tmp0=Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(_tmp0);}}
void Cyc_Absyndump_dump_str(struct _tagged_arr*s){Cyc_Absyndump_dump(*s);}void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char((int)';');}void Cyc_Absyndump_dump_sep(void(*f)(void*),
struct Cyc_List_List*l,struct _tagged_arr sep){if(l == 0)return;for(0;l->tl != 0;l=l->tl){
f((void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f((void*)l->hd);}void Cyc_Absyndump_dump_sep_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _tagged_arr sep){if(l
== 0)return;for(0;l->tl != 0;l=l->tl){f(env,(void*)l->hd);Cyc_Absyndump_dump_nospace(
sep);}f(env,(void*)l->hd);}void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep){Cyc_Absyndump_dump_nospace(
start);Cyc_Absyndump_dump_sep(f,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep){Cyc_Absyndump_dump_nospace(start);Cyc_Absyndump_dump_sep_c(
f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_egroup(void(*f)(
void*),struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep){if(l != 0)Cyc_Absyndump_group(f,l,start,end,sep);}void Cyc_Absyndump_dumpqvar(
struct _tuple0*v){struct Cyc_List_List*_tmp1=0;{void*_tmp2=(*v).f1;struct Cyc_List_List*
_tmp3;struct Cyc_List_List*_tmp4;_LL11: if((int)_tmp2 != 0)goto _LL13;_LL12: _tmp3=0;
goto _LL14;_LL13: if(_tmp2 <= (void*)1?1:*((int*)_tmp2)!= 0)goto _LL15;_tmp3=((
struct Cyc_Absyn_Rel_n_struct*)_tmp2)->f1;_LL14: _tmp1=_tmp3;goto _LL10;_LL15: if(
_tmp2 <= (void*)1?1:*((int*)_tmp2)!= 1)goto _LL10;_tmp4=((struct Cyc_Absyn_Abs_n_struct*)
_tmp2)->f1;_LL16: _tmp1=(Cyc_Absyndump_qvar_to_Cids?Cyc_Absyndump_add_cyc_prefix:
0)?({struct Cyc_List_List*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->hd=Cyc_Absynpp_cyc_stringptr;
_tmp5->tl=_tmp4;_tmp5;}): _tmp4;goto _LL10;_LL10:;}if(_tmp1 != 0){Cyc_Absyndump_dump_str((
struct _tagged_arr*)_tmp1->hd);for(_tmp1=_tmp1->tl;_tmp1 != 0;_tmp1=_tmp1->tl){if(
Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{Cyc_Absyndump_dump_nospace(
_tag_arr("::",sizeof(unsigned char),3));}Cyc_Absyndump_dump_nospace(*((struct
_tagged_arr*)_tmp1->hd));}if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_nospace(
_tag_arr("_",sizeof(unsigned char),2));else{Cyc_Absyndump_dump_nospace(_tag_arr("::",
sizeof(unsigned char),3));}Cyc_Absyndump_dump_nospace(*(*v).f2);}else{Cyc_Absyndump_dump_str((*
v).f2);}}void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict)Cyc_Absyndump_dump(
_tag_arr("restrict",sizeof(unsigned char),9));if(tq.q_volatile)Cyc_Absyndump_dump(
_tag_arr("volatile",sizeof(unsigned char),9));if(tq.q_const)Cyc_Absyndump_dump(
_tag_arr("const",sizeof(unsigned char),6));}void Cyc_Absyndump_dumpscope(void*sc){
void*_tmp6=sc;_LL18: if((int)_tmp6 != 0)goto _LL1A;_LL19: Cyc_Absyndump_dump(
_tag_arr("static",sizeof(unsigned char),7));return;_LL1A: if((int)_tmp6 != 2)goto
_LL1C;_LL1B: return;_LL1C: if((int)_tmp6 != 3)goto _LL1E;_LL1D: Cyc_Absyndump_dump(
_tag_arr("extern",sizeof(unsigned char),7));return;_LL1E: if((int)_tmp6 != 4)goto
_LL20;_LL1F: Cyc_Absyndump_dump(_tag_arr("extern \"C\"",sizeof(unsigned char),11));
return;_LL20: if((int)_tmp6 != 1)goto _LL17;_LL21: Cyc_Absyndump_dump(_tag_arr("abstract",
sizeof(unsigned char),9));return;_LL17:;}void Cyc_Absyndump_dumpkind(void*k){void*
_tmp7=k;_LL23: if((int)_tmp7 != 0)goto _LL25;_LL24: Cyc_Absyndump_dump(_tag_arr("A",
sizeof(unsigned char),2));return;_LL25: if((int)_tmp7 != 1)goto _LL27;_LL26: Cyc_Absyndump_dump(
_tag_arr("M",sizeof(unsigned char),2));return;_LL27: if((int)_tmp7 != 2)goto _LL29;
_LL28: Cyc_Absyndump_dump(_tag_arr("B",sizeof(unsigned char),2));return;_LL29: if((
int)_tmp7 != 3)goto _LL2B;_LL2A: Cyc_Absyndump_dump(_tag_arr("R",sizeof(
unsigned char),2));return;_LL2B: if((int)_tmp7 != 4)goto _LL22;_LL2C: Cyc_Absyndump_dump(
_tag_arr("E",sizeof(unsigned char),2));return;_LL22:;}void Cyc_Absyndump_dumpaggr_kind(
void*k){void*_tmp8=k;_LL2E: if((int)_tmp8 != 0)goto _LL30;_LL2F: Cyc_Absyndump_dump(
_tag_arr("struct",sizeof(unsigned char),7));return;_LL30: if((int)_tmp8 != 1)goto
_LL2D;_LL31: Cyc_Absyndump_dump(_tag_arr("union",sizeof(unsigned char),6));
return;_LL2D:;}void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){Cyc_Absyndump_egroup(
Cyc_Absyndump_dumptyp,ts,_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumptvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(tv->name);}void Cyc_Absyndump_dumpkindedtvar(
struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(tv->name);{void*_tmp9=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpA;void*_tmpB;_LL33: if(*((int*)_tmp9)!= 0)goto _LL35;
_tmpA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp9)->f1;if((int)_tmpA != 2)goto
_LL35;_LL34: goto _LL36;_LL35: if(*((int*)_tmp9)!= 2)goto _LL37;_LL36: goto _LL38;
_LL37: if(*((int*)_tmp9)!= 1)goto _LL39;_LL38: Cyc_Absyndump_dump(_tag_arr("::?",
sizeof(unsigned char),4));goto _LL32;_LL39: if(*((int*)_tmp9)!= 0)goto _LL32;_tmpB=(
void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp9)->f1;_LL3A: Cyc_Absyndump_dump(
_tag_arr("::",sizeof(unsigned char),3));Cyc_Absyndump_dumpkind(_tmpB);goto _LL32;
_LL32:;}}void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
tvs,_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",sizeof(unsigned char),2),
_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpkindedtvars(struct
Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,tvs,_tag_arr("<",sizeof(unsigned char),2),_tag_arr(">",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}struct _tuple7{
struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(struct _tuple7*pr){((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple7*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumparg,ts,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dump_callconv(
struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmpC=(void*)atts->hd;
_LL3C: if((int)_tmpC != 0)goto _LL3E;_LL3D: Cyc_Absyndump_dump(_tag_arr("_stdcall",
sizeof(unsigned char),9));return;_LL3E: if((int)_tmpC != 1)goto _LL40;_LL3F: Cyc_Absyndump_dump(
_tag_arr("_cdecl",sizeof(unsigned char),7));return;_LL40: if((int)_tmpC != 2)goto
_LL42;_LL41: Cyc_Absyndump_dump(_tag_arr("_fastcall",sizeof(unsigned char),10));
return;_LL42:;_LL43: goto _LL3B;_LL3B:;}}void Cyc_Absyndump_dump_noncallconv(struct
Cyc_List_List*atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;
atts2=atts2->tl){void*_tmpD=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;
_LL45: if((int)_tmpD != 0)goto _LL47;_LL46: goto _LL48;_LL47: if((int)_tmpD != 1)goto
_LL49;_LL48: goto _LL4A;_LL49: if((int)_tmpD != 2)goto _LL4B;_LL4A: goto _LL44;_LL4B:;
_LL4C: hasatt=1;goto _LL44;_LL44:;}}if(!hasatt)return;Cyc_Absyndump_dump(_tag_arr("__declspec(",
sizeof(unsigned char),12));for(0;atts != 0;atts=atts->tl){void*_tmpE=(void*)atts->hd;
_LL4E: if((int)_tmpE != 0)goto _LL50;_LL4F: goto _LL51;_LL50: if((int)_tmpE != 1)goto
_LL52;_LL51: goto _LL53;_LL52: if((int)_tmpE != 2)goto _LL54;_LL53: goto _LL4D;_LL54:;
_LL55: Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL4D;
_LL4D:;}Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpatts(struct Cyc_List_List*
atts){if(atts == 0)return;if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dump_noncallconv(
atts);return;}Cyc_Absyndump_dump(_tag_arr(" __attribute__((",sizeof(
unsigned char),17));for(0;atts != 0;atts=atts->tl){Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((
void*)atts->hd));if(atts->tl != 0)Cyc_Absyndump_dump(_tag_arr(",",sizeof(
unsigned char),2));}Cyc_Absyndump_dump(_tag_arr(")) ",sizeof(unsigned char),4));}
int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms){if(tms == 0)return 0;{
void*_tmpF=(void*)tms->hd;_LL57: if(_tmpF <= (void*)1?1:*((int*)_tmpF)!= 1)goto
_LL59;_LL58: return 1;_LL59:;_LL5A: return 0;_LL56:;}}static void Cyc_Absyndump_dumprgn(
void*t){void*_tmp10=Cyc_Tcutil_compress(t);_LL5C: if((int)_tmp10 != 2)goto _LL5E;
_LL5D: Cyc_Absyndump_dump(_tag_arr("`H",sizeof(unsigned char),3));goto _LL5B;_LL5E:;
_LL5F: Cyc_Absyndump_dumpntyp(t);goto _LL5B;_LL5B:;}struct _tuple8{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};static struct _tuple8 Cyc_Absyndump_effects_split(void*
t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;{void*_tmp11=Cyc_Tcutil_compress(
t);void*_tmp12;struct Cyc_List_List*_tmp13;_LL61: if(_tmp11 <= (void*)3?1:*((int*)
_tmp11)!= 17)goto _LL63;_tmp12=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp11)->f1;
_LL62: rgions=({struct Cyc_List_List*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->hd=(
void*)_tmp12;_tmp14->tl=rgions;_tmp14;});goto _LL60;_LL63: if(_tmp11 <= (void*)3?1:*((
int*)_tmp11)!= 18)goto _LL65;_tmp13=((struct Cyc_Absyn_JoinEff_struct*)_tmp11)->f1;
_LL64: for(0;_tmp13 != 0;_tmp13=_tmp13->tl){struct Cyc_List_List*_tmp16;struct Cyc_List_List*
_tmp17;struct _tuple8 _tmp15=Cyc_Absyndump_effects_split((void*)_tmp13->hd);_tmp16=
_tmp15.f1;_tmp17=_tmp15.f2;rgions=Cyc_List_imp_append(_tmp16,rgions);effects=Cyc_List_imp_append(
_tmp17,effects);}goto _LL60;_LL65:;_LL66: effects=({struct Cyc_List_List*_tmp18=
_cycalloc(sizeof(*_tmp18));_tmp18->hd=(void*)t;_tmp18->tl=effects;_tmp18;});goto
_LL60;_LL60:;}return({struct _tuple8 _tmp19;_tmp19.f1=rgions;_tmp19.f2=effects;
_tmp19;});}static void Cyc_Absyndump_dumpeff(void*t){struct Cyc_List_List*_tmp1B;
struct Cyc_List_List*_tmp1C;struct _tuple8 _tmp1A=Cyc_Absyndump_effects_split(t);
_tmp1B=_tmp1A.f1;_tmp1C=_tmp1A.f2;_tmp1B=Cyc_List_imp_rev(_tmp1B);_tmp1C=Cyc_List_imp_rev(
_tmp1C);for(0;_tmp1C != 0;_tmp1C=_tmp1C->tl){Cyc_Absyndump_dumpntyp((void*)_tmp1C->hd);
Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;
_tmp1B != 0;_tmp1B=_tmp1B->tl){Cyc_Absyndump_dumprgn((void*)_tmp1B->hd);if(_tmp1B->tl
!= 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump_char((int)'}');}void
Cyc_Absyndump_dumpntyp(void*t){void*_tmp1D=t;struct Cyc_Absyn_Tvar*_tmp1E;struct
Cyc_Core_Opt*_tmp1F;struct Cyc_Core_Opt*_tmp20;int _tmp21;struct Cyc_Core_Opt*
_tmp22;struct Cyc_Core_Opt*_tmp23;struct Cyc_Core_Opt _tmp24;void*_tmp25;int _tmp26;
struct Cyc_Absyn_TunionInfo _tmp27;void*_tmp28;struct Cyc_List_List*_tmp29;void*
_tmp2A;struct Cyc_Absyn_TunionFieldInfo _tmp2B;void*_tmp2C;struct Cyc_List_List*
_tmp2D;void*_tmp2E;void*_tmp2F;void*_tmp30;void*_tmp31;void*_tmp32;void*_tmp33;
void*_tmp34;void*_tmp35;void*_tmp36;void*_tmp37;void*_tmp38;void*_tmp39;void*
_tmp3A;void*_tmp3B;void*_tmp3C;void*_tmp3D;void*_tmp3E;void*_tmp3F;void*_tmp40;
void*_tmp41;int _tmp42;struct Cyc_List_List*_tmp43;struct Cyc_Absyn_AggrInfo _tmp44;
void*_tmp45;struct Cyc_List_List*_tmp46;void*_tmp47;struct Cyc_List_List*_tmp48;
struct _tuple0*_tmp49;struct Cyc_List_List*_tmp4A;struct _tuple0*_tmp4B;struct Cyc_List_List*
_tmp4C;struct Cyc_Core_Opt*_tmp4D;void*_tmp4E;void*_tmp4F;_LL68: if(_tmp1D <= (void*)
3?1:*((int*)_tmp1D)!= 7)goto _LL6A;_LL69: goto _LL6B;_LL6A: if(_tmp1D <= (void*)3?1:*((
int*)_tmp1D)!= 8)goto _LL6C;_LL6B: goto _LL6D;_LL6C: if(_tmp1D <= (void*)3?1:*((int*)
_tmp1D)!= 4)goto _LL6E;_LL6D: return;_LL6E: if((int)_tmp1D != 0)goto _LL70;_LL6F: Cyc_Absyndump_dump(
_tag_arr("void",sizeof(unsigned char),5));return;_LL70: if(_tmp1D <= (void*)3?1:*((
int*)_tmp1D)!= 1)goto _LL72;_tmp1E=((struct Cyc_Absyn_VarType_struct*)_tmp1D)->f1;
_LL71: Cyc_Absyndump_dump_str(_tmp1E->name);return;_LL72: if(_tmp1D <= (void*)3?1:*((
int*)_tmp1D)!= 0)goto _LL74;_tmp1F=((struct Cyc_Absyn_Evar_struct*)_tmp1D)->f1;
_tmp20=((struct Cyc_Absyn_Evar_struct*)_tmp1D)->f2;if(_tmp20 != 0)goto _LL74;_tmp21=((
struct Cyc_Absyn_Evar_struct*)_tmp1D)->f3;_LL73: Cyc_Absyndump_dump(_tag_arr("%",
sizeof(unsigned char),2));if(_tmp1F == 0)Cyc_Absyndump_dump(_tag_arr("?",sizeof(
unsigned char),2));else{Cyc_Absyndump_dumpkind((void*)_tmp1F->v);}Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp51;_tmp51.tag=1;_tmp51.f1=(
int)((unsigned int)_tmp21);{void*_tmp50[1]={& _tmp51};Cyc_Std_aprintf(_tag_arr("(%d)",
sizeof(unsigned char),5),_tag_arr(_tmp50,sizeof(void*),1));}}));return;_LL74: if(
_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 0)goto _LL76;_tmp22=((struct Cyc_Absyn_Evar_struct*)
_tmp1D)->f1;_tmp23=((struct Cyc_Absyn_Evar_struct*)_tmp1D)->f2;if(_tmp23 == 0)goto
_LL76;_tmp24=*_tmp23;_tmp25=(void*)_tmp24.v;_tmp26=((struct Cyc_Absyn_Evar_struct*)
_tmp1D)->f3;_LL75: Cyc_Absyndump_dumpntyp(_tmp25);return;_LL76: if(_tmp1D <= (void*)
3?1:*((int*)_tmp1D)!= 2)goto _LL78;_tmp27=((struct Cyc_Absyn_TunionType_struct*)
_tmp1D)->f1;_tmp28=(void*)_tmp27.tunion_info;_tmp29=_tmp27.targs;_tmp2A=(void*)
_tmp27.rgn;_LL77:{void*_tmp52=_tmp28;struct Cyc_Absyn_UnknownTunionInfo _tmp53;
struct _tuple0*_tmp54;int _tmp55;struct Cyc_Absyn_Tuniondecl**_tmp56;struct Cyc_Absyn_Tuniondecl*
_tmp57;struct Cyc_Absyn_Tuniondecl _tmp58;struct _tuple0*_tmp59;int _tmp5A;_LLAB: if(*((
int*)_tmp52)!= 0)goto _LLAD;_tmp53=((struct Cyc_Absyn_UnknownTunion_struct*)_tmp52)->f1;
_tmp54=_tmp53.name;_tmp55=_tmp53.is_xtunion;_LLAC: _tmp59=_tmp54;_tmp5A=_tmp55;
goto _LLAE;_LLAD: if(*((int*)_tmp52)!= 1)goto _LLAA;_tmp56=((struct Cyc_Absyn_KnownTunion_struct*)
_tmp52)->f1;_tmp57=*_tmp56;_tmp58=*_tmp57;_tmp59=_tmp58.name;_tmp5A=_tmp58.is_xtunion;
_LLAE: if(_tmp5A)Cyc_Absyndump_dump(_tag_arr("xtunion ",sizeof(unsigned char),9));
else{Cyc_Absyndump_dump(_tag_arr("tunion ",sizeof(unsigned char),8));}{void*
_tmp5B=Cyc_Tcutil_compress(_tmp2A);_LLB0: if((int)_tmp5B != 2)goto _LLB2;_LLB1: goto
_LLAF;_LLB2:;_LLB3: Cyc_Absyndump_dumptyp(_tmp2A);Cyc_Absyndump_dump(_tag_arr(" ",
sizeof(unsigned char),2));goto _LLAF;_LLAF:;}Cyc_Absyndump_dumpqvar(_tmp59);Cyc_Absyndump_dumptps(
_tmp29);goto _LLAA;_LLAA:;}goto _LL67;_LL78: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)
!= 3)goto _LL7A;_tmp2B=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1D)->f1;
_tmp2C=(void*)_tmp2B.field_info;_tmp2D=_tmp2B.targs;_LL79:{void*_tmp5C=_tmp2C;
struct Cyc_Absyn_UnknownTunionFieldInfo _tmp5D;struct _tuple0*_tmp5E;struct _tuple0*
_tmp5F;int _tmp60;struct Cyc_Absyn_Tuniondecl*_tmp61;struct Cyc_Absyn_Tuniondecl
_tmp62;struct _tuple0*_tmp63;int _tmp64;struct Cyc_Absyn_Tunionfield*_tmp65;struct
Cyc_Absyn_Tunionfield _tmp66;struct _tuple0*_tmp67;_LLB5: if(*((int*)_tmp5C)!= 0)
goto _LLB7;_tmp5D=((struct Cyc_Absyn_UnknownTunionfield_struct*)_tmp5C)->f1;_tmp5E=
_tmp5D.tunion_name;_tmp5F=_tmp5D.field_name;_tmp60=_tmp5D.is_xtunion;_LLB6:
_tmp63=_tmp5E;_tmp64=_tmp60;_tmp67=_tmp5F;goto _LLB8;_LLB7: if(*((int*)_tmp5C)!= 1)
goto _LLB4;_tmp61=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp5C)->f1;_tmp62=*
_tmp61;_tmp63=_tmp62.name;_tmp64=_tmp62.is_xtunion;_tmp65=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp5C)->f2;_tmp66=*_tmp65;_tmp67=_tmp66.name;_LLB8: if(_tmp64)Cyc_Absyndump_dump(
_tag_arr("xtunion ",sizeof(unsigned char),9));else{Cyc_Absyndump_dump(_tag_arr("tunion ",
sizeof(unsigned char),8));}Cyc_Absyndump_dumpqvar(_tmp63);Cyc_Absyndump_dump(
_tag_arr(".",sizeof(unsigned char),2));Cyc_Absyndump_dumpqvar(_tmp67);Cyc_Absyndump_dumptps(
_tmp2D);goto _LLB4;_LLB4:;}goto _LL67;_LL7A: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)
!= 5)goto _LL7C;_tmp2E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((
int)_tmp2E != 0)goto _LL7C;_tmp2F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;
if((int)_tmp2F != 2)goto _LL7C;_LL7B: Cyc_Absyndump_dump(_tag_arr("int",sizeof(
unsigned char),4));return;_LL7C: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto
_LL7E;_tmp30=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp30
!= 0)goto _LL7E;_tmp31=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp31 != 0)goto _LL7E;_LL7D: Cyc_Absyndump_dump(_tag_arr("signed char",sizeof(
unsigned char),12));return;_LL7E: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto
_LL80;_tmp32=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp32
!= 1)goto _LL80;_tmp33=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp33 != 0)goto _LL80;_LL7F: Cyc_Absyndump_dump(_tag_arr("unsigned char",
sizeof(unsigned char),14));return;_LL80: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 
5)goto _LL82;_tmp34=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)
_tmp34 != 0)goto _LL82;_tmp35=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;
if((int)_tmp35 != 1)goto _LL82;_LL81: Cyc_Absyndump_dump(_tag_arr("short",sizeof(
unsigned char),6));return;_LL82: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 5)goto
_LL84;_tmp36=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)_tmp36
!= 1)goto _LL84;_tmp37=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;if((
int)_tmp37 != 1)goto _LL84;_LL83: Cyc_Absyndump_dump(_tag_arr("unsigned short",
sizeof(unsigned char),15));return;_LL84: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 
5)goto _LL86;_tmp38=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)
_tmp38 != 1)goto _LL86;_tmp39=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;
if((int)_tmp39 != 2)goto _LL86;_LL85: Cyc_Absyndump_dump(_tag_arr("unsigned int",
sizeof(unsigned char),13));return;_LL86: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 
5)goto _LL88;_tmp3A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)
_tmp3A != 0)goto _LL88;_tmp3B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;
if((int)_tmp3B != 3)goto _LL88;if(!Cyc_Absyndump_to_VC)goto _LL88;_LL87: Cyc_Absyndump_dump(
_tag_arr("__int64",sizeof(unsigned char),8));return;_LL88: if(_tmp1D <= (void*)3?1:*((
int*)_tmp1D)!= 5)goto _LL8A;_tmp3C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f1;if((int)_tmp3C != 0)goto _LL8A;_tmp3D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp3D != 3)goto _LL8A;_LL89: Cyc_Absyndump_dump(_tag_arr("long long",
sizeof(unsigned char),10));return;_LL8A: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 
5)goto _LL8C;_tmp3E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f1;if((int)
_tmp3E != 1)goto _LL8C;_tmp3F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1D)->f2;
if((int)_tmp3F != 3)goto _LL8C;if(!Cyc_Absyndump_to_VC)goto _LL8C;_LL8B: Cyc_Absyndump_dump(
_tag_arr("unsigned __int64",sizeof(unsigned char),17));return;_LL8C: if(_tmp1D <= (
void*)3?1:*((int*)_tmp1D)!= 5)goto _LL8E;_tmp40=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f1;if((int)_tmp40 != 1)goto _LL8E;_tmp41=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp1D)->f2;if((int)_tmp41 != 3)goto _LL8E;_LL8D: Cyc_Absyndump_dump(_tag_arr("unsigned long long",
sizeof(unsigned char),19));return;_LL8E: if((int)_tmp1D != 1)goto _LL90;_LL8F: Cyc_Absyndump_dump(
_tag_arr("float",sizeof(unsigned char),6));return;_LL90: if(_tmp1D <= (void*)3?1:*((
int*)_tmp1D)!= 6)goto _LL92;_tmp42=((struct Cyc_Absyn_DoubleType_struct*)_tmp1D)->f1;
_LL91: if(_tmp42)Cyc_Absyndump_dump(_tag_arr("long double",sizeof(unsigned char),
12));else{Cyc_Absyndump_dump(_tag_arr("double",sizeof(unsigned char),7));}
return;_LL92: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 9)goto _LL94;_tmp43=((
struct Cyc_Absyn_TupleType_struct*)_tmp1D)->f1;_LL93: Cyc_Absyndump_dump_char((int)'$');
Cyc_Absyndump_dumpargs(_tmp43);return;_LL94: if(_tmp1D <= (void*)3?1:*((int*)
_tmp1D)!= 10)goto _LL96;_tmp44=((struct Cyc_Absyn_AggrType_struct*)_tmp1D)->f1;
_tmp45=(void*)_tmp44.aggr_info;_tmp46=_tmp44.targs;_LL95: {void*_tmp69;struct
_tuple0*_tmp6A;struct _tuple3 _tmp68=Cyc_Absyn_aggr_kinded_name(_tmp45);_tmp69=
_tmp68.f1;_tmp6A=_tmp68.f2;Cyc_Absyndump_dumpaggr_kind(_tmp69);Cyc_Absyndump_dumpqvar(
_tmp6A);Cyc_Absyndump_dumptps(_tmp46);return;}_LL96: if(_tmp1D <= (void*)3?1:*((
int*)_tmp1D)!= 11)goto _LL98;_tmp47=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1D)->f1;_tmp48=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1D)->f2;_LL97: Cyc_Absyndump_dumpaggr_kind(
_tmp47);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmp48);
Cyc_Absyndump_dump_char((int)'}');return;_LL98: if(_tmp1D <= (void*)3?1:*((int*)
_tmp1D)!= 12)goto _LL9A;_tmp49=((struct Cyc_Absyn_EnumType_struct*)_tmp1D)->f1;
_LL99: Cyc_Absyndump_dump(_tag_arr("enum ",sizeof(unsigned char),6));Cyc_Absyndump_dumpqvar(
_tmp49);return;_LL9A: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 13)goto _LL9C;
_tmp4A=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp1D)->f1;_LL9B: Cyc_Absyndump_dump(
_tag_arr("enum {",sizeof(unsigned char),7));Cyc_Absyndump_dumpenumfields(_tmp4A);
Cyc_Absyndump_dump(_tag_arr("}",sizeof(unsigned char),2));return;_LL9C: if(_tmp1D
<= (void*)3?1:*((int*)_tmp1D)!= 16)goto _LL9E;_tmp4B=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1D)->f1;_tmp4C=((struct Cyc_Absyn_TypedefType_struct*)_tmp1D)->f2;_tmp4D=((
struct Cyc_Absyn_TypedefType_struct*)_tmp1D)->f3;_LL9D:(Cyc_Absyndump_dumpqvar(
_tmp4B),Cyc_Absyndump_dumptps(_tmp4C));return;_LL9E: if(_tmp1D <= (void*)3?1:*((
int*)_tmp1D)!= 14)goto _LLA0;_tmp4E=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp1D)->f1;_LL9F: Cyc_Absyndump_dump(_tag_arr("sizeof_t<",sizeof(unsigned char),
10));Cyc_Absyndump_dumpntyp(_tmp4E);Cyc_Absyndump_dump(_tag_arr(">",sizeof(
unsigned char),2));return;_LLA0: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 15)goto
_LLA2;_tmp4F=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp1D)->f1;_LLA1:
Cyc_Absyndump_dumprgn(_tmp4F);return;_LLA2: if((int)_tmp1D != 2)goto _LLA4;_LLA3:
goto _LLA5;_LLA4: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 17)goto _LLA6;_LLA5: goto
_LLA7;_LLA6: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 19)goto _LLA8;_LLA7: goto
_LLA9;_LLA8: if(_tmp1D <= (void*)3?1:*((int*)_tmp1D)!= 18)goto _LL67;_LLA9: return;
_LL67:;}void Cyc_Absyndump_dumpvaropt(struct Cyc_Core_Opt*vo){if(vo != 0)Cyc_Absyndump_dump_str((
struct _tagged_arr*)vo->v);}void Cyc_Absyndump_dumpfunarg(struct _tuple1*t){((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Core_Opt*),struct Cyc_Core_Opt*))
Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}void
Cyc_Absyndump_dump_rgncmp(struct _tuple6*cmp){struct _tuple6 _tmp6C;void*_tmp6D;
void*_tmp6E;struct _tuple6*_tmp6B=cmp;_tmp6C=*_tmp6B;_tmp6D=_tmp6C.f1;_tmp6E=
_tmp6C.f2;Cyc_Absyndump_dumptyp(_tmp6D);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptyp(
_tmp6E);}void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){((void(*)(void(*
f)(struct _tuple6*),struct Cyc_List_List*l,struct _tagged_arr sep))Cyc_Absyndump_dump_sep)(
Cyc_Absyndump_dump_rgncmp,rgn_po,_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((
int)'(');for(0;args != 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple1*)
args->hd);if((args->tl != 0?1: c_varargs)?1: cyc_varargs != 0)Cyc_Absyndump_dump_char((
int)',');}if(c_varargs)Cyc_Absyndump_dump(_tag_arr("...",sizeof(unsigned char),4));
else{if(cyc_varargs != 0){struct _tuple1*_tmp6F=({struct _tuple1*_tmp70=_cycalloc(
sizeof(*_tmp70));_tmp70->f1=cyc_varargs->name;_tmp70->f2=cyc_varargs->tq;_tmp70->f3=(
void*)cyc_varargs->type;_tmp70;});Cyc_Absyndump_dump(_tag_arr("...",sizeof(
unsigned char),4));if(cyc_varargs->inject)Cyc_Absyndump_dump(_tag_arr(" inject ",
sizeof(unsigned char),9));Cyc_Absyndump_dumpfunarg(_tmp6F);}}if(effopt != 0){Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff((void*)effopt->v);}if(rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}Cyc_Absyndump_dump_char((int)')');}
void Cyc_Absyndump_dumptyp(void*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
int),int))Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp71;_tmp71.q_const=0;
_tmp71.q_volatile=0;_tmp71.q_restrict=0;_tmp71;}),t,(void(*)(int x))Cyc_Absyndump_ignore,
0);}void Cyc_Absyndump_dumpdesignator(void*d){void*_tmp72=d;struct Cyc_Absyn_Exp*
_tmp73;struct _tagged_arr*_tmp74;_LLBA: if(*((int*)_tmp72)!= 0)goto _LLBC;_tmp73=((
struct Cyc_Absyn_ArrayElement_struct*)_tmp72)->f1;_LLBB: Cyc_Absyndump_dump(
_tag_arr(".[",sizeof(unsigned char),3));Cyc_Absyndump_dumpexp(_tmp73);Cyc_Absyndump_dump_char((
int)']');goto _LLB9;_LLBC: if(*((int*)_tmp72)!= 1)goto _LLB9;_tmp74=((struct Cyc_Absyn_FieldName_struct*)
_tmp72)->f1;_LLBD: Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp74);goto _LLB9;_LLB9:;}struct _tuple9{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};void Cyc_Absyndump_dumpde(struct _tuple9*de){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*
de).f1,_tag_arr("",sizeof(unsigned char),1),_tag_arr("=",sizeof(unsigned char),2),
_tag_arr("=",sizeof(unsigned char),2));Cyc_Absyndump_dumpexp((*de).f2);}void Cyc_Absyndump_dumpexps_prec(
int inprec,struct Cyc_List_List*es){((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),
int env,struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,inprec,es,
_tag_arr("",sizeof(unsigned char),1),_tag_arr("",sizeof(unsigned char),1),
_tag_arr(",",sizeof(unsigned char),2));}void Cyc_Absyndump_dumpexp_prec(int inprec,
struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= myprec)Cyc_Absyndump_dump_nospace(
_tag_arr("(",sizeof(unsigned char),2));{void*_tmp75=(void*)e->r;void*_tmp76;void*
_tmp77;unsigned char _tmp78;void*_tmp79;void*_tmp7A;short _tmp7B;void*_tmp7C;void*
_tmp7D;int _tmp7E;void*_tmp7F;void*_tmp80;int _tmp81;void*_tmp82;void*_tmp83;
long long _tmp84;void*_tmp85;struct _tagged_arr _tmp86;void*_tmp87;void*_tmp88;
struct _tagged_arr _tmp89;struct _tuple0*_tmp8A;struct _tuple0*_tmp8B;void*_tmp8C;
struct Cyc_List_List*_tmp8D;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Core_Opt*_tmp8F;
struct Cyc_Absyn_Exp*_tmp90;struct Cyc_Absyn_Exp*_tmp91;void*_tmp92;struct Cyc_Absyn_Exp*
_tmp93;void*_tmp94;struct Cyc_Absyn_Exp*_tmp95;void*_tmp96;struct Cyc_Absyn_Exp*
_tmp97;void*_tmp98;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Exp*_tmp9A;struct
Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9D;
struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_List_List*_tmp9F;struct Cyc_Absyn_Exp*_tmpA0;
struct Cyc_List_List*_tmpA1;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Exp*_tmpA3;
struct Cyc_Absyn_Exp*_tmpA4;void*_tmpA5;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Exp*
_tmpA7;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_Absyn_Exp*_tmpA9;void*_tmpAA;struct
Cyc_Absyn_Exp*_tmpAB;void*_tmpAC;void*_tmpAD;struct _tagged_arr*_tmpAE;void*
_tmpAF;void*_tmpB0;unsigned int _tmpB1;struct Cyc_List_List*_tmpB2;void*_tmpB3;
struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;struct _tagged_arr*_tmpB6;
struct Cyc_Absyn_Exp*_tmpB7;struct _tagged_arr*_tmpB8;struct Cyc_Absyn_Exp*_tmpB9;
struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_List_List*_tmpBB;struct _tuple1*_tmpBC;
struct Cyc_List_List*_tmpBD;struct Cyc_List_List*_tmpBE;struct Cyc_Absyn_Vardecl*
_tmpBF;struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_Absyn_Exp*_tmpC1;struct _tuple0*
_tmpC2;struct Cyc_List_List*_tmpC3;struct Cyc_List_List*_tmpC4;struct Cyc_List_List*
_tmpC5;struct Cyc_List_List*_tmpC6;struct Cyc_Absyn_Tunionfield*_tmpC7;struct
_tuple0*_tmpC8;struct _tuple0*_tmpC9;struct Cyc_Absyn_MallocInfo _tmpCA;int _tmpCB;
struct Cyc_Absyn_Exp*_tmpCC;void**_tmpCD;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Core_Opt*
_tmpCF;struct Cyc_List_List*_tmpD0;struct Cyc_Absyn_Stmt*_tmpD1;struct Cyc_Absyn_Fndecl*
_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;_LLBF: if(*((int*)_tmp75)!= 0)goto _LLC1;_tmp76=(
void*)((struct Cyc_Absyn_Const_e_struct*)_tmp75)->f1;if(_tmp76 <= (void*)1?1:*((
int*)_tmp76)!= 0)goto _LLC1;_tmp77=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp76)->f1;
_tmp78=((struct Cyc_Absyn_Char_c_struct*)_tmp76)->f2;_LLC0: Cyc_Absyndump_dump_char((
int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp78));Cyc_Absyndump_dump_char((
int)'\'');goto _LLBE;_LLC1: if(*((int*)_tmp75)!= 0)goto _LLC3;_tmp79=(void*)((
struct Cyc_Absyn_Const_e_struct*)_tmp75)->f1;if(_tmp79 <= (void*)1?1:*((int*)
_tmp79)!= 1)goto _LLC3;_tmp7A=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmp79)->f1;
_tmp7B=((struct Cyc_Absyn_Short_c_struct*)_tmp79)->f2;_LLC2: Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpD5;_tmpD5.tag=1;_tmpD5.f1=(
int)((unsigned int)((int)_tmp7B));{void*_tmpD4[1]={& _tmpD5};Cyc_Std_aprintf(
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmpD4,sizeof(void*),1));}}));
goto _LLBE;_LLC3: if(*((int*)_tmp75)!= 0)goto _LLC5;_tmp7C=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp75)->f1;if(_tmp7C <= (void*)1?1:*((int*)_tmp7C)!= 2)goto _LLC5;_tmp7D=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp7C)->f1;if((int)_tmp7D != 0)goto _LLC5;_tmp7E=((
struct Cyc_Absyn_Int_c_struct*)_tmp7C)->f2;_LLC4: Cyc_Absyndump_dump((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpD7;_tmpD7.tag=1;_tmpD7.f1=(int)((
unsigned int)_tmp7E);{void*_tmpD6[1]={& _tmpD7};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(unsigned char),3),_tag_arr(_tmpD6,sizeof(void*),1));}}));goto _LLBE;_LLC5:
if(*((int*)_tmp75)!= 0)goto _LLC7;_tmp7F=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp75)->f1;if(_tmp7F <= (void*)1?1:*((int*)_tmp7F)!= 2)goto _LLC7;_tmp80=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmp7F)->f1;if((int)_tmp80 != 1)goto _LLC7;_tmp81=((
struct Cyc_Absyn_Int_c_struct*)_tmp7F)->f2;_LLC6: Cyc_Absyndump_dump((struct
_tagged_arr)({struct Cyc_Std_Int_pa_struct _tmpD9;_tmpD9.tag=1;_tmpD9.f1=(
unsigned int)_tmp81;{void*_tmpD8[1]={& _tmpD9};Cyc_Std_aprintf(_tag_arr("%u",
sizeof(unsigned char),3),_tag_arr(_tmpD8,sizeof(void*),1));}}));goto _LLBE;_LLC7:
if(*((int*)_tmp75)!= 0)goto _LLC9;_tmp82=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp75)->f1;if(_tmp82 <= (void*)1?1:*((int*)_tmp82)!= 3)goto _LLC9;_tmp83=(void*)((
struct Cyc_Absyn_LongLong_c_struct*)_tmp82)->f1;_tmp84=((struct Cyc_Absyn_LongLong_c_struct*)
_tmp82)->f2;_LLC8: Cyc_Absyndump_dump(_tag_arr("<<FIX LONG LONG CONSTANT>>",
sizeof(unsigned char),27));goto _LLBE;_LLC9: if(*((int*)_tmp75)!= 0)goto _LLCB;
_tmp85=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp75)->f1;if(_tmp85 <= (void*)1?
1:*((int*)_tmp85)!= 4)goto _LLCB;_tmp86=((struct Cyc_Absyn_Float_c_struct*)_tmp85)->f1;
_LLCA: Cyc_Absyndump_dump(_tmp86);goto _LLBE;_LLCB: if(*((int*)_tmp75)!= 0)goto
_LLCD;_tmp87=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp75)->f1;if((int)_tmp87
!= 0)goto _LLCD;_LLCC: Cyc_Absyndump_dump(_tag_arr("NULL",sizeof(unsigned char),5));
goto _LLBE;_LLCD: if(*((int*)_tmp75)!= 0)goto _LLCF;_tmp88=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp75)->f1;if(_tmp88 <= (void*)1?1:*((int*)_tmp88)!= 5)goto _LLCF;_tmp89=((struct
Cyc_Absyn_String_c_struct*)_tmp88)->f1;_LLCE: Cyc_Absyndump_dump_char((int)'"');
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp89));Cyc_Absyndump_dump_char((
int)'"');goto _LLBE;_LLCF: if(*((int*)_tmp75)!= 2)goto _LLD1;_tmp8A=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp75)->f1;_LLD0: _tmp8B=_tmp8A;goto _LLD2;_LLD1: if(*((int*)_tmp75)!= 1)goto _LLD3;
_tmp8B=((struct Cyc_Absyn_Var_e_struct*)_tmp75)->f1;_LLD2: Cyc_Absyndump_dumpqvar(
_tmp8B);goto _LLBE;_LLD3: if(*((int*)_tmp75)!= 3)goto _LLD5;_tmp8C=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmp75)->f1;_tmp8D=((struct Cyc_Absyn_Primop_e_struct*)
_tmp75)->f2;_LLD4: {struct _tagged_arr _tmpDA=Cyc_Absynpp_prim2str(_tmp8C);switch(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8D)){case 1: _LL121: if(_tmp8C == (
void*)19){Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp8D))->hd);Cyc_Absyndump_dump(_tag_arr(".size",sizeof(
unsigned char),6));}else{Cyc_Absyndump_dump(_tmpDA);Cyc_Absyndump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp8D))->hd);}
break;case 2: _LL122: Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp8D))->hd);Cyc_Absyndump_dump(_tmpDA);Cyc_Absyndump_dump_char((
int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp8D->tl))->hd);break;default: _LL123:(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=({struct Cyc_Core_Failure_struct
_tmpDC;_tmpDC.tag=Cyc_Core_Failure;_tmpDC.f1=_tag_arr("Absyndump -- Bad number of arguments to primop",
sizeof(unsigned char),47);_tmpDC;});_tmpDB;}));}goto _LLBE;}_LLD5: if(*((int*)
_tmp75)!= 4)goto _LLD7;_tmp8E=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp75)->f1;
_tmp8F=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp75)->f2;_tmp90=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp75)->f3;_LLD6: Cyc_Absyndump_dumpexp_prec(myprec,_tmp8E);if(_tmp8F != 0)Cyc_Absyndump_dump(
Cyc_Absynpp_prim2str((void*)_tmp8F->v));Cyc_Absyndump_dump_nospace(_tag_arr("=",
sizeof(unsigned char),2));Cyc_Absyndump_dumpexp_prec(myprec,_tmp90);goto _LLBE;
_LLD7: if(*((int*)_tmp75)!= 5)goto _LLD9;_tmp91=((struct Cyc_Absyn_Increment_e_struct*)
_tmp75)->f1;_tmp92=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp75)->f2;if((
int)_tmp92 != 0)goto _LLD9;_LLD8: Cyc_Absyndump_dump(_tag_arr("++",sizeof(
unsigned char),3));Cyc_Absyndump_dumpexp_prec(myprec,_tmp91);goto _LLBE;_LLD9: if(*((
int*)_tmp75)!= 5)goto _LLDB;_tmp93=((struct Cyc_Absyn_Increment_e_struct*)_tmp75)->f1;
_tmp94=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp75)->f2;if((int)_tmp94 != 
2)goto _LLDB;_LLDA: Cyc_Absyndump_dump(_tag_arr("--",sizeof(unsigned char),3));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp93);goto _LLBE;_LLDB: if(*((int*)_tmp75)!= 5)goto _LLDD;_tmp95=((struct
Cyc_Absyn_Increment_e_struct*)_tmp75)->f1;_tmp96=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp75)->f2;if((int)_tmp96 != 1)goto _LLDD;_LLDC: Cyc_Absyndump_dumpexp_prec(myprec,
_tmp95);Cyc_Absyndump_dump(_tag_arr("++",sizeof(unsigned char),3));goto _LLBE;
_LLDD: if(*((int*)_tmp75)!= 5)goto _LLDF;_tmp97=((struct Cyc_Absyn_Increment_e_struct*)
_tmp75)->f1;_tmp98=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp75)->f2;if((
int)_tmp98 != 3)goto _LLDF;_LLDE: Cyc_Absyndump_dumpexp_prec(myprec,_tmp97);Cyc_Absyndump_dump(
_tag_arr("--",sizeof(unsigned char),3));goto _LLBE;_LLDF: if(*((int*)_tmp75)!= 6)
goto _LLE1;_tmp99=((struct Cyc_Absyn_Conditional_e_struct*)_tmp75)->f1;_tmp9A=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp75)->f2;_tmp9B=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp75)->f3;_LLE0: Cyc_Absyndump_dumpexp_prec(myprec,_tmp99);Cyc_Absyndump_dump_char((
int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp9A);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp9B);goto _LLBE;_LLE1: if(*((int*)_tmp75)!= 7)
goto _LLE3;_tmp9C=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp75)->f1;_tmp9D=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp75)->f2;_LLE2: Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp9C);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp9D);Cyc_Absyndump_dump_char((int)')');goto _LLBE;_LLE3: if(*((int*)
_tmp75)!= 8)goto _LLE5;_tmp9E=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp75)->f1;
_tmp9F=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp75)->f2;_LLE4: _tmpA0=_tmp9E;
_tmpA1=_tmp9F;goto _LLE6;_LLE5: if(*((int*)_tmp75)!= 9)goto _LLE7;_tmpA0=((struct
Cyc_Absyn_FnCall_e_struct*)_tmp75)->f1;_tmpA1=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp75)->f2;_LLE6: Cyc_Absyndump_dumpexp_prec(myprec,_tmpA0);Cyc_Absyndump_dump_nospace(
_tag_arr("(",sizeof(unsigned char),2));Cyc_Absyndump_dumpexps_prec(20,_tmpA1);
Cyc_Absyndump_dump_nospace(_tag_arr(")",sizeof(unsigned char),2));goto _LLBE;
_LLE7: if(*((int*)_tmp75)!= 10)goto _LLE9;_tmpA2=((struct Cyc_Absyn_Throw_e_struct*)
_tmp75)->f1;_LLE8: Cyc_Absyndump_dump(_tag_arr("throw",sizeof(unsigned char),6));
Cyc_Absyndump_dumpexp_prec(myprec,_tmpA2);goto _LLBE;_LLE9: if(*((int*)_tmp75)!= 
11)goto _LLEB;_tmpA3=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp75)->f1;_LLEA:
_tmpA4=_tmpA3;goto _LLEC;_LLEB: if(*((int*)_tmp75)!= 12)goto _LLED;_tmpA4=((struct
Cyc_Absyn_Instantiate_e_struct*)_tmp75)->f1;_LLEC: Cyc_Absyndump_dumpexp_prec(
inprec,_tmpA4);goto _LLBE;_LLED: if(*((int*)_tmp75)!= 13)goto _LLEF;_tmpA5=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp75)->f1;_tmpA6=((struct Cyc_Absyn_Cast_e_struct*)
_tmp75)->f2;_LLEE: Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmpA5);
Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmpA6);goto
_LLBE;_LLEF: if(*((int*)_tmp75)!= 14)goto _LLF1;_tmpA7=((struct Cyc_Absyn_Address_e_struct*)
_tmp75)->f1;_LLF0: Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(
myprec,_tmpA7);goto _LLBE;_LLF1: if(*((int*)_tmp75)!= 15)goto _LLF3;_tmpA8=((struct
Cyc_Absyn_New_e_struct*)_tmp75)->f1;_tmpA9=((struct Cyc_Absyn_New_e_struct*)
_tmp75)->f2;_LLF2: Cyc_Absyndump_dump(_tag_arr("new ",sizeof(unsigned char),5));
Cyc_Absyndump_dumpexp_prec(myprec,_tmpA9);goto _LLBE;_LLF3: if(*((int*)_tmp75)!= 
16)goto _LLF5;_tmpAA=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp75)->f1;
_LLF4: Cyc_Absyndump_dump(_tag_arr("sizeof(",sizeof(unsigned char),8));Cyc_Absyndump_dumptyp(
_tmpAA);Cyc_Absyndump_dump_char((int)')');goto _LLBE;_LLF5: if(*((int*)_tmp75)!= 
17)goto _LLF7;_tmpAB=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp75)->f1;_LLF6: Cyc_Absyndump_dump(
_tag_arr("sizeof(",sizeof(unsigned char),8));Cyc_Absyndump_dumpexp(_tmpAB);Cyc_Absyndump_dump_char((
int)')');goto _LLBE;_LLF7: if(*((int*)_tmp75)!= 18)goto _LLF9;_tmpAC=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp75)->f1;_tmpAD=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp75)->f2;if(*((int*)_tmpAD)!= 0)goto _LLF9;_tmpAE=((struct Cyc_Absyn_StructField_struct*)
_tmpAD)->f1;_LLF8: Cyc_Absyndump_dump(_tag_arr("offsetof(",sizeof(unsigned char),
10));Cyc_Absyndump_dumptyp(_tmpAC);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*
_tmpAE);Cyc_Absyndump_dump_char((int)')');goto _LLBE;_LLF9: if(*((int*)_tmp75)!= 
18)goto _LLFB;_tmpAF=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp75)->f1;
_tmpB0=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp75)->f2;if(*((int*)_tmpB0)
!= 1)goto _LLFB;_tmpB1=((struct Cyc_Absyn_TupleIndex_struct*)_tmpB0)->f1;_LLFA: Cyc_Absyndump_dump(
_tag_arr("offsetof(",sizeof(unsigned char),10));Cyc_Absyndump_dumptyp(_tmpAF);
Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump((struct _tagged_arr)({struct
Cyc_Std_Int_pa_struct _tmpDE;_tmpDE.tag=1;_tmpDE.f1=(int)_tmpB1;{void*_tmpDD[1]={&
_tmpDE};Cyc_Std_aprintf(_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmpDD,
sizeof(void*),1));}}));Cyc_Absyndump_dump_char((int)')');goto _LLBE;_LLFB: if(*((
int*)_tmp75)!= 19)goto _LLFD;_tmpB2=((struct Cyc_Absyn_Gentyp_e_struct*)_tmp75)->f1;
_tmpB3=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp75)->f2;_LLFC: Cyc_Absyndump_dump(
_tag_arr("__gen(",sizeof(unsigned char),7));Cyc_Absyndump_dumptvars(_tmpB2);Cyc_Absyndump_dumptyp(
_tmpB3);Cyc_Absyndump_dump_char((int)')');goto _LLBE;_LLFD: if(*((int*)_tmp75)!= 
20)goto _LLFF;_tmpB4=((struct Cyc_Absyn_Deref_e_struct*)_tmp75)->f1;_LLFE: Cyc_Absyndump_dump_char((
int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmpB4);goto _LLBE;_LLFF: if(*((int*)
_tmp75)!= 21)goto _LL101;_tmpB5=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp75)->f1;
_tmpB6=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp75)->f2;_LL100: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpB5);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmpB6);goto _LLBE;_LL101: if(*((int*)_tmp75)!= 22)goto _LL103;_tmpB7=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp75)->f1;_tmpB8=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp75)->f2;_LL102: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpB7);Cyc_Absyndump_dump_nospace(_tag_arr("->",sizeof(unsigned char),3));
Cyc_Absyndump_dump_nospace(*_tmpB8);goto _LLBE;_LL103: if(*((int*)_tmp75)!= 23)
goto _LL105;_tmpB9=((struct Cyc_Absyn_Subscript_e_struct*)_tmp75)->f1;_tmpBA=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp75)->f2;_LL104: Cyc_Absyndump_dumpexp_prec(
myprec,_tmpB9);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmpBA);
Cyc_Absyndump_dump_char((int)']');goto _LLBE;_LL105: if(*((int*)_tmp75)!= 24)goto
_LL107;_tmpBB=((struct Cyc_Absyn_Tuple_e_struct*)_tmp75)->f1;_LL106: Cyc_Absyndump_dump(
_tag_arr("$(",sizeof(unsigned char),3));Cyc_Absyndump_dumpexps_prec(20,_tmpBB);
Cyc_Absyndump_dump_char((int)')');goto _LLBE;_LL107: if(*((int*)_tmp75)!= 25)goto
_LL109;_tmpBC=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp75)->f1;_tmpBD=((
struct Cyc_Absyn_CompoundLit_e_struct*)_tmp75)->f2;_LL108: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumptyp((*_tmpBC).f3);Cyc_Absyndump_dump_char((int)')');((
void(*)(void(*f)(struct _tuple9*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmpBD,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LLBE;_LL109: if(*((int*)_tmp75)!= 
26)goto _LL10B;_tmpBE=((struct Cyc_Absyn_Array_e_struct*)_tmp75)->f1;_LL10A:((void(*)(
void(*f)(struct _tuple9*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmpBE,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LLBE;_LL10B: if(*((int*)_tmp75)!= 
27)goto _LL10D;_tmpBF=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp75)->f1;
_tmpC0=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp75)->f2;_tmpC1=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp75)->f3;_LL10C: Cyc_Absyndump_dump(_tag_arr("new {for",sizeof(unsigned char),
9));Cyc_Absyndump_dump_str((*_tmpBF->name).f2);Cyc_Absyndump_dump_char((int)'<');
Cyc_Absyndump_dumpexp(_tmpC0);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(
_tmpC1);Cyc_Absyndump_dump_char((int)'}');goto _LLBE;_LL10D: if(*((int*)_tmp75)!= 
28)goto _LL10F;_tmpC2=((struct Cyc_Absyn_Struct_e_struct*)_tmp75)->f1;_tmpC3=((
struct Cyc_Absyn_Struct_e_struct*)_tmp75)->f2;_tmpC4=((struct Cyc_Absyn_Struct_e_struct*)
_tmp75)->f3;_LL10E: Cyc_Absyndump_dumpqvar(_tmpC2);Cyc_Absyndump_dump_char((int)'{');
if(_tmpC3 != 0)Cyc_Absyndump_dumptps(_tmpC3);((void(*)(void(*f)(struct _tuple9*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmpC4,_tag_arr("",
sizeof(unsigned char),1),_tag_arr("}",sizeof(unsigned char),2),_tag_arr(",",
sizeof(unsigned char),2));goto _LLBE;_LL10F: if(*((int*)_tmp75)!= 29)goto _LL111;
_tmpC5=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp75)->f2;_LL110:((void(*)(void(*
f)(struct _tuple9*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmpC5,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LLBE;_LL111: if(*((int*)_tmp75)!= 
30)goto _LL113;_tmpC6=((struct Cyc_Absyn_Tunion_e_struct*)_tmp75)->f1;_tmpC7=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp75)->f3;_LL112: Cyc_Absyndump_dumpqvar(
_tmpC7->name);if(_tmpC6 != 0)((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp,_tmpC6,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));goto _LLBE;_LL113:
if(*((int*)_tmp75)!= 31)goto _LL115;_tmpC8=((struct Cyc_Absyn_Enum_e_struct*)
_tmp75)->f1;_LL114: Cyc_Absyndump_dumpqvar(_tmpC8);goto _LLBE;_LL115: if(*((int*)
_tmp75)!= 32)goto _LL117;_tmpC9=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp75)->f1;
_LL116: Cyc_Absyndump_dumpqvar(_tmpC9);goto _LLBE;_LL117: if(*((int*)_tmp75)!= 33)
goto _LL119;_tmpCA=((struct Cyc_Absyn_Malloc_e_struct*)_tmp75)->f1;_tmpCB=_tmpCA.is_calloc;
_tmpCC=_tmpCA.rgn;_tmpCD=_tmpCA.elt_type;_tmpCE=_tmpCA.num_elts;_LL118: if(_tmpCB){
if(_tmpCC != 0){Cyc_Absyndump_dump(_tag_arr("rcalloc(",sizeof(unsigned char),9));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmpCC));Cyc_Absyndump_dump(
_tag_arr(",",sizeof(unsigned char),2));}else{Cyc_Absyndump_dump(_tag_arr("calloc",
sizeof(unsigned char),7));}Cyc_Absyndump_dumpexp(_tmpCE);Cyc_Absyndump_dump(
_tag_arr(",",sizeof(unsigned char),2));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmpCD)),0));Cyc_Absyndump_dump(_tag_arr(")",sizeof(
unsigned char),2));}else{if(_tmpCC != 0){Cyc_Absyndump_dump(_tag_arr("rmalloc(",
sizeof(unsigned char),9));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmpCC));Cyc_Absyndump_dump(_tag_arr(",",sizeof(unsigned char),2));}
else{Cyc_Absyndump_dump(_tag_arr("malloc(",sizeof(unsigned char),8));}if(_tmpCD
!= 0)Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmpCD,0),
_tmpCE,0));else{Cyc_Absyndump_dumpexp(_tmpCE);}Cyc_Absyndump_dump(_tag_arr(")",
sizeof(unsigned char),2));}goto _LLBE;_LL119: if(*((int*)_tmp75)!= 34)goto _LL11B;
_tmpCF=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp75)->f1;_tmpD0=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp75)->f2;_LL11A:((void(*)(void(*f)(struct _tuple9*),struct Cyc_List_List*l,
struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmpD0,_tag_arr("{",sizeof(unsigned char),2),_tag_arr("}",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));goto _LLBE;_LL11B:
if(*((int*)_tmp75)!= 35)goto _LL11D;_tmpD1=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp75)->f1;_LL11C: Cyc_Absyndump_dump_nospace(_tag_arr("({",sizeof(unsigned char),
3));Cyc_Absyndump_dumpstmt(_tmpD1);Cyc_Absyndump_dump_nospace(_tag_arr("})",
sizeof(unsigned char),3));goto _LLBE;_LL11D: if(*((int*)_tmp75)!= 36)goto _LL11F;
_tmpD2=((struct Cyc_Absyn_Codegen_e_struct*)_tmp75)->f1;_LL11E: Cyc_Absyndump_dump(
_tag_arr("codegen(",sizeof(unsigned char),9));Cyc_Absyndump_dumpdecl(({struct Cyc_Absyn_Decl*
_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->r=(void*)((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_Fn_d_struct _tmpE1;
_tmpE1.tag=1;_tmpE1.f1=_tmpD2;_tmpE1;});_tmpE0;}));_tmpDF->loc=e->loc;_tmpDF;}));
Cyc_Absyndump_dump(_tag_arr(")",sizeof(unsigned char),2));goto _LLBE;_LL11F: if(*((
int*)_tmp75)!= 37)goto _LLBE;_tmpD3=((struct Cyc_Absyn_Fill_e_struct*)_tmp75)->f1;
_LL120: Cyc_Absyndump_dump(_tag_arr("fill(",sizeof(unsigned char),6));Cyc_Absyndump_dumpexp(
_tmpD3);Cyc_Absyndump_dump(_tag_arr(")",sizeof(unsigned char),2));goto _LLBE;
_LLBE:;}if(inprec >= myprec)Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*e){Cyc_Absyndump_dumpexp_prec(0,e);}void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmpE2=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmpE2->where_clause == 0?(void*)(
_tmpE2->pattern)->r == (void*)0: 0)Cyc_Absyndump_dump(_tag_arr("default:",sizeof(
unsigned char),9));else{Cyc_Absyndump_dump(_tag_arr("case",sizeof(unsigned char),
5));Cyc_Absyndump_dumppat(_tmpE2->pattern);if(_tmpE2->where_clause != 0){Cyc_Absyndump_dump(
_tag_arr("&&",sizeof(unsigned char),3));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmpE2->where_clause));}Cyc_Absyndump_dump_nospace(_tag_arr(":",
sizeof(unsigned char),2));}Cyc_Absyndump_dumpstmt(_tmpE2->body);}}void Cyc_Absyndump_dumpstmt(
struct Cyc_Absyn_Stmt*s){void*_tmpE3=(void*)s->r;struct Cyc_Absyn_Exp*_tmpE4;
struct Cyc_Absyn_Stmt*_tmpE5;struct Cyc_Absyn_Stmt*_tmpE6;struct Cyc_Absyn_Exp*
_tmpE7;struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Stmt*
_tmpEA;struct Cyc_Absyn_Stmt*_tmpEB;struct _tuple2 _tmpEC;struct Cyc_Absyn_Exp*
_tmpED;struct Cyc_Absyn_Stmt*_tmpEE;struct _tagged_arr*_tmpEF;struct Cyc_Absyn_Exp*
_tmpF0;struct _tuple2 _tmpF1;struct Cyc_Absyn_Exp*_tmpF2;struct _tuple2 _tmpF3;struct
Cyc_Absyn_Exp*_tmpF4;struct Cyc_Absyn_Stmt*_tmpF5;struct Cyc_Absyn_ForArrayInfo
_tmpF6;struct Cyc_List_List*_tmpF7;struct _tuple2 _tmpF8;struct Cyc_Absyn_Exp*_tmpF9;
struct _tuple2 _tmpFA;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Stmt*_tmpFC;
struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_List_List*_tmpFE;struct Cyc_Absyn_Decl*
_tmpFF;struct Cyc_Absyn_Stmt*_tmp100;struct _tagged_arr*_tmp101;struct Cyc_Absyn_Stmt*
_tmp102;struct Cyc_Absyn_Stmt*_tmp103;struct _tuple2 _tmp104;struct Cyc_Absyn_Exp*
_tmp105;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_List_List*
_tmp108;struct Cyc_List_List*_tmp109;struct Cyc_Absyn_Stmt*_tmp10A;struct Cyc_List_List*
_tmp10B;struct Cyc_Absyn_Tvar*_tmp10C;struct Cyc_Absyn_Vardecl*_tmp10D;struct Cyc_Absyn_Stmt*
_tmp10E;struct Cyc_Absyn_Stmt*_tmp10F;struct Cyc_Absyn_Stmt*_tmp110;_LL126: if((int)
_tmpE3 != 0)goto _LL128;_LL127: Cyc_Absyndump_dump_semi();goto _LL125;_LL128: if(
_tmpE3 <= (void*)1?1:*((int*)_tmpE3)!= 0)goto _LL12A;_tmpE4=((struct Cyc_Absyn_Exp_s_struct*)
_tmpE3)->f1;_LL129: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dumpexp(_tmpE4);
Cyc_Absyndump_dump_semi();goto _LL125;_LL12A: if(_tmpE3 <= (void*)1?1:*((int*)
_tmpE3)!= 1)goto _LL12C;_tmpE5=((struct Cyc_Absyn_Seq_s_struct*)_tmpE3)->f1;_tmpE6=((
struct Cyc_Absyn_Seq_s_struct*)_tmpE3)->f2;_LL12B: if(Cyc_Absynpp_is_declaration(
_tmpE5)){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmpE5);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dumpstmt(_tmpE5);}if(Cyc_Absynpp_is_declaration(
_tmpE6)){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmpE6);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dumpstmt(_tmpE6);}goto _LL125;_LL12C: if(_tmpE3 <= (
void*)1?1:*((int*)_tmpE3)!= 2)goto _LL12E;_tmpE7=((struct Cyc_Absyn_Return_s_struct*)
_tmpE3)->f1;if(_tmpE7 != 0)goto _LL12E;_LL12D: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("return;",sizeof(unsigned char),8));goto _LL125;_LL12E: if(_tmpE3 <= (void*)
1?1:*((int*)_tmpE3)!= 2)goto _LL130;_tmpE8=((struct Cyc_Absyn_Return_s_struct*)
_tmpE3)->f1;_LL12F: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("return",
sizeof(unsigned char),7));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmpE8));Cyc_Absyndump_dump_semi();goto _LL125;_LL130: if(_tmpE3 <= (
void*)1?1:*((int*)_tmpE3)!= 3)goto _LL132;_tmpE9=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpE3)->f1;_tmpEA=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpE3)->f2;_tmpEB=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmpE3)->f3;_LL131: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(_tag_arr("if(",sizeof(unsigned char),4));Cyc_Absyndump_dumpexp(
_tmpE9);{void*_tmp111=(void*)_tmpEA->r;_LL155: if(_tmp111 <= (void*)1?1:*((int*)
_tmp111)!= 1)goto _LL157;_LL156: goto _LL158;_LL157: if(_tmp111 <= (void*)1?1:*((int*)
_tmp111)!= 12)goto _LL159;_LL158: goto _LL15A;_LL159: if(_tmp111 <= (void*)1?1:*((int*)
_tmp111)!= 3)goto _LL15B;_LL15A: goto _LL15C;_LL15B: if(_tmp111 <= (void*)1?1:*((int*)
_tmp111)!= 15)goto _LL15D;_LL15C: Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(
unsigned char),3));Cyc_Absyndump_dumpstmt(_tmpEA);Cyc_Absyndump_dump_char((int)'}');
goto _LL154;_LL15D:;_LL15E: Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(
_tmpEA);_LL154:;}{void*_tmp112=(void*)_tmpEB->r;_LL160: if((int)_tmp112 != 0)goto
_LL162;_LL161: goto _LL15F;_LL162:;_LL163: Cyc_Absyndump_dump(_tag_arr("else{",
sizeof(unsigned char),6));Cyc_Absyndump_dumpstmt(_tmpEB);Cyc_Absyndump_dump_char((
int)'}');goto _LL15F;_LL15F:;}goto _LL125;_LL132: if(_tmpE3 <= (void*)1?1:*((int*)
_tmpE3)!= 4)goto _LL134;_tmpEC=((struct Cyc_Absyn_While_s_struct*)_tmpE3)->f1;
_tmpED=_tmpEC.f1;_tmpEE=((struct Cyc_Absyn_While_s_struct*)_tmpE3)->f2;_LL133: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("while(",sizeof(unsigned char),7));Cyc_Absyndump_dumpexp(
_tmpED);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpstmt(
_tmpEE);Cyc_Absyndump_dump_char((int)'}');goto _LL125;_LL134: if(_tmpE3 <= (void*)1?
1:*((int*)_tmpE3)!= 5)goto _LL136;_LL135: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("break;",sizeof(unsigned char),7));goto _LL125;_LL136: if(_tmpE3 <= (void*)
1?1:*((int*)_tmpE3)!= 6)goto _LL138;_LL137: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(
_tag_arr("continue;",sizeof(unsigned char),10));goto _LL125;_LL138: if(_tmpE3 <= (
void*)1?1:*((int*)_tmpE3)!= 7)goto _LL13A;_tmpEF=((struct Cyc_Absyn_Goto_s_struct*)
_tmpE3)->f1;_LL139: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("goto",
sizeof(unsigned char),5));Cyc_Absyndump_dump_str(_tmpEF);Cyc_Absyndump_dump_semi();
goto _LL125;_LL13A: if(_tmpE3 <= (void*)1?1:*((int*)_tmpE3)!= 8)goto _LL13C;_tmpF0=((
struct Cyc_Absyn_For_s_struct*)_tmpE3)->f1;_tmpF1=((struct Cyc_Absyn_For_s_struct*)
_tmpE3)->f2;_tmpF2=_tmpF1.f1;_tmpF3=((struct Cyc_Absyn_For_s_struct*)_tmpE3)->f3;
_tmpF4=_tmpF3.f1;_tmpF5=((struct Cyc_Absyn_For_s_struct*)_tmpE3)->f4;_LL13B: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("for(",sizeof(unsigned char),5));Cyc_Absyndump_dumpexp(
_tmpF0);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmpF2);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmpF4);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(
unsigned char),3));Cyc_Absyndump_dumpstmt(_tmpF5);Cyc_Absyndump_dump_char((int)'}');
goto _LL125;_LL13C: if(_tmpE3 <= (void*)1?1:*((int*)_tmpE3)!= 19)goto _LL13E;_tmpF6=((
struct Cyc_Absyn_ForArray_s_struct*)_tmpE3)->f1;_tmpF7=_tmpF6.defns;_tmpF8=_tmpF6.condition;
_tmpF9=_tmpF8.f1;_tmpFA=_tmpF6.delta;_tmpFB=_tmpFA.f1;_tmpFC=_tmpF6.body;_LL13D:
Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("forarray(",sizeof(
unsigned char),10));for(0;_tmpF7 != 0;_tmpF7=_tmpF7->tl){Cyc_Absyndump_dumpvardecl((
struct Cyc_Absyn_Vardecl*)_tmpF7->hd,s->loc);}Cyc_Absyndump_dumpexp(_tmpF9);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmpFB);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(
unsigned char),3));Cyc_Absyndump_dumpstmt(_tmpFC);Cyc_Absyndump_dump_char((int)'}');
goto _LL125;_LL13E: if(_tmpE3 <= (void*)1?1:*((int*)_tmpE3)!= 9)goto _LL140;_tmpFD=((
struct Cyc_Absyn_Switch_s_struct*)_tmpE3)->f1;_tmpFE=((struct Cyc_Absyn_Switch_s_struct*)
_tmpE3)->f2;_LL13F: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("switch(",
sizeof(unsigned char),8));Cyc_Absyndump_dumpexp(_tmpFD);Cyc_Absyndump_dump_nospace(
_tag_arr("){",sizeof(unsigned char),3));Cyc_Absyndump_dumpswitchclauses(_tmpFE);
Cyc_Absyndump_dump_char((int)'}');goto _LL125;_LL140: if(_tmpE3 <= (void*)1?1:*((
int*)_tmpE3)!= 12)goto _LL142;_tmpFF=((struct Cyc_Absyn_Decl_s_struct*)_tmpE3)->f1;
_tmp100=((struct Cyc_Absyn_Decl_s_struct*)_tmpE3)->f2;_LL141: Cyc_Absyndump_dumpdecl(
_tmpFF);Cyc_Absyndump_dumpstmt(_tmp100);goto _LL125;_LL142: if(_tmpE3 <= (void*)1?1:*((
int*)_tmpE3)!= 15)goto _LL144;_tmp101=((struct Cyc_Absyn_Label_s_struct*)_tmpE3)->f1;
_tmp102=((struct Cyc_Absyn_Label_s_struct*)_tmpE3)->f2;_LL143: if(Cyc_Absynpp_is_declaration(
_tmp102)){Cyc_Absyndump_dump_str(_tmp101);Cyc_Absyndump_dump_nospace(_tag_arr(": {",
sizeof(unsigned char),4));Cyc_Absyndump_dumpstmt(_tmp102);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dump_str(_tmp101);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpstmt(_tmp102);}goto _LL125;_LL144: if(_tmpE3 <= (void*)1?1:*((int*)
_tmpE3)!= 16)goto _LL146;_tmp103=((struct Cyc_Absyn_Do_s_struct*)_tmpE3)->f1;
_tmp104=((struct Cyc_Absyn_Do_s_struct*)_tmpE3)->f2;_tmp105=_tmp104.f1;_LL145: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(_tag_arr("do{",sizeof(unsigned char),4));Cyc_Absyndump_dumpstmt(
_tmp103);Cyc_Absyndump_dump_nospace(_tag_arr("}while(",sizeof(unsigned char),8));
Cyc_Absyndump_dumpexp(_tmp105);Cyc_Absyndump_dump_nospace(_tag_arr(");",sizeof(
unsigned char),3));goto _LL125;_LL146: if(_tmpE3 <= (void*)1?1:*((int*)_tmpE3)!= 10)
goto _LL148;_tmp106=((struct Cyc_Absyn_SwitchC_s_struct*)_tmpE3)->f1;_tmp107=((
struct Cyc_Absyn_SwitchC_s_struct*)_tmpE3)->f2;_LL147: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(_tag_arr("switch \"C\"(",sizeof(unsigned char),12));Cyc_Absyndump_dumpexp(
_tmp106);Cyc_Absyndump_dump_nospace(_tag_arr("){",sizeof(unsigned char),3));for(
0;_tmp107 != 0;_tmp107=_tmp107->tl){struct Cyc_Absyn_SwitchC_clause _tmp114;struct
Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Stmt*_tmp116;struct Cyc_Absyn_SwitchC_clause*
_tmp113=(struct Cyc_Absyn_SwitchC_clause*)_tmp107->hd;_tmp114=*_tmp113;_tmp115=
_tmp114.cnst_exp;_tmp116=_tmp114.body;if(_tmp115 == 0)Cyc_Absyndump_dump(_tag_arr("default:",
sizeof(unsigned char),9));else{Cyc_Absyndump_dump(_tag_arr("case",sizeof(
unsigned char),5));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(
_tmp115));Cyc_Absyndump_dump_char((int)':');}Cyc_Absyndump_dumpstmt(_tmp116);}
Cyc_Absyndump_dump_char((int)'}');goto _LL125;_LL148: if(_tmpE3 <= (void*)1?1:*((
int*)_tmpE3)!= 11)goto _LL14A;_tmp108=((struct Cyc_Absyn_Fallthru_s_struct*)_tmpE3)->f1;
if(_tmp108 != 0)goto _LL14A;_LL149: Cyc_Absyndump_dump(_tag_arr("fallthru;",sizeof(
unsigned char),10));goto _LL125;_LL14A: if(_tmpE3 <= (void*)1?1:*((int*)_tmpE3)!= 
11)goto _LL14C;_tmp109=((struct Cyc_Absyn_Fallthru_s_struct*)_tmpE3)->f1;_LL14B:
Cyc_Absyndump_dump(_tag_arr("fallthru(",sizeof(unsigned char),10));Cyc_Absyndump_dumpexps_prec(
20,_tmp109);Cyc_Absyndump_dump_nospace(_tag_arr(");",sizeof(unsigned char),3));
goto _LL125;_LL14C: if(_tmpE3 <= (void*)1?1:*((int*)_tmpE3)!= 17)goto _LL14E;_tmp10A=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmpE3)->f1;_tmp10B=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmpE3)->f2;_LL14D: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(_tag_arr("try",
sizeof(unsigned char),4));Cyc_Absyndump_dumpstmt(_tmp10A);Cyc_Absyndump_dump(
_tag_arr("catch{",sizeof(unsigned char),7));Cyc_Absyndump_dumpswitchclauses(
_tmp10B);Cyc_Absyndump_dump_char((int)'}');goto _LL125;_LL14E: if(_tmpE3 <= (void*)
1?1:*((int*)_tmpE3)!= 18)goto _LL150;_tmp10C=((struct Cyc_Absyn_Region_s_struct*)
_tmpE3)->f1;_tmp10D=((struct Cyc_Absyn_Region_s_struct*)_tmpE3)->f2;_tmp10E=((
struct Cyc_Absyn_Region_s_struct*)_tmpE3)->f3;_LL14F: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(_tag_arr("region<",sizeof(unsigned char),8));Cyc_Absyndump_dumptvar(
_tmp10C);Cyc_Absyndump_dump(_tag_arr("> ",sizeof(unsigned char),3));Cyc_Absyndump_dumpqvar(
_tmp10D->name);Cyc_Absyndump_dump(_tag_arr("{",sizeof(unsigned char),2));Cyc_Absyndump_dumpstmt(
_tmp10E);Cyc_Absyndump_dump(_tag_arr("}",sizeof(unsigned char),2));goto _LL125;
_LL150: if(_tmpE3 <= (void*)1?1:*((int*)_tmpE3)!= 13)goto _LL152;_tmp10F=((struct
Cyc_Absyn_Cut_s_struct*)_tmpE3)->f1;_LL151: Cyc_Absyndump_dump(_tag_arr("cut",
sizeof(unsigned char),4));Cyc_Absyndump_dumpstmt(_tmp10F);goto _LL125;_LL152: if(
_tmpE3 <= (void*)1?1:*((int*)_tmpE3)!= 14)goto _LL125;_tmp110=((struct Cyc_Absyn_Splice_s_struct*)
_tmpE3)->f1;_LL153: Cyc_Absyndump_dump(_tag_arr("splice",sizeof(unsigned char),7));
Cyc_Absyndump_dumpstmt(_tmp110);goto _LL125;_LL125:;}struct _tuple10{struct Cyc_List_List*
f1;struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(struct _tuple10*dp){Cyc_Absyndump_egroup(
Cyc_Absyndump_dumpdesignator,(*dp).f1,_tag_arr("",sizeof(unsigned char),1),
_tag_arr("=",sizeof(unsigned char),2),_tag_arr("=",sizeof(unsigned char),2));Cyc_Absyndump_dumppat((*
dp).f2);}void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){void*_tmp117=(void*)p->r;
void*_tmp118;int _tmp119;void*_tmp11A;int _tmp11B;unsigned char _tmp11C;struct
_tagged_arr _tmp11D;struct Cyc_Absyn_Vardecl*_tmp11E;struct Cyc_List_List*_tmp11F;
struct Cyc_Absyn_Pat*_tmp120;struct Cyc_Absyn_Vardecl*_tmp121;struct _tuple0*
_tmp122;struct _tuple0*_tmp123;struct Cyc_List_List*_tmp124;struct Cyc_Absyn_AggrInfo
_tmp125;void*_tmp126;struct Cyc_List_List*_tmp127;struct Cyc_List_List*_tmp128;
struct Cyc_Absyn_Tunionfield*_tmp129;struct Cyc_List_List*_tmp12A;struct Cyc_Absyn_Enumfield*
_tmp12B;struct Cyc_Absyn_Enumfield*_tmp12C;_LL165: if((int)_tmp117 != 0)goto _LL167;
_LL166: Cyc_Absyndump_dump_char((int)'_');goto _LL164;_LL167: if((int)_tmp117 != 1)
goto _LL169;_LL168: Cyc_Absyndump_dump(_tag_arr("NULL",sizeof(unsigned char),5));
goto _LL164;_LL169: if(_tmp117 <= (void*)2?1:*((int*)_tmp117)!= 1)goto _LL16B;
_tmp118=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp117)->f1;if((int)_tmp118 != 0)
goto _LL16B;_tmp119=((struct Cyc_Absyn_Int_p_struct*)_tmp117)->f2;_LL16A: Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp12E;_tmp12E.tag=1;_tmp12E.f1=(
int)((unsigned int)_tmp119);{void*_tmp12D[1]={& _tmp12E};Cyc_Std_aprintf(_tag_arr("%d",
sizeof(unsigned char),3),_tag_arr(_tmp12D,sizeof(void*),1));}}));goto _LL164;
_LL16B: if(_tmp117 <= (void*)2?1:*((int*)_tmp117)!= 1)goto _LL16D;_tmp11A=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp117)->f1;if((int)_tmp11A != 1)goto _LL16D;
_tmp11B=((struct Cyc_Absyn_Int_p_struct*)_tmp117)->f2;_LL16C: Cyc_Absyndump_dump((
struct _tagged_arr)({struct Cyc_Std_Int_pa_struct _tmp130;_tmp130.tag=1;_tmp130.f1=(
unsigned int)_tmp11B;{void*_tmp12F[1]={& _tmp130};Cyc_Std_aprintf(_tag_arr("%u",
sizeof(unsigned char),3),_tag_arr(_tmp12F,sizeof(void*),1));}}));goto _LL164;
_LL16D: if(_tmp117 <= (void*)2?1:*((int*)_tmp117)!= 2)goto _LL16F;_tmp11C=((struct
Cyc_Absyn_Char_p_struct*)_tmp117)->f1;_LL16E: Cyc_Absyndump_dump(_tag_arr("'",
sizeof(unsigned char),2));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(
_tmp11C));Cyc_Absyndump_dump_nospace(_tag_arr("'",sizeof(unsigned char),2));goto
_LL164;_LL16F: if(_tmp117 <= (void*)2?1:*((int*)_tmp117)!= 3)goto _LL171;_tmp11D=((
struct Cyc_Absyn_Float_p_struct*)_tmp117)->f1;_LL170: Cyc_Absyndump_dump(_tmp11D);
goto _LL164;_LL171: if(_tmp117 <= (void*)2?1:*((int*)_tmp117)!= 0)goto _LL173;
_tmp11E=((struct Cyc_Absyn_Var_p_struct*)_tmp117)->f1;_LL172: Cyc_Absyndump_dumpqvar(
_tmp11E->name);goto _LL164;_LL173: if(_tmp117 <= (void*)2?1:*((int*)_tmp117)!= 4)
goto _LL175;_tmp11F=((struct Cyc_Absyn_Tuple_p_struct*)_tmp117)->f1;_LL174:((void(*)(
void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _tagged_arr start,
struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
_tmp11F,_tag_arr("$(",sizeof(unsigned char),3),_tag_arr(")",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LL164;_LL175: if(_tmp117 <= (void*)
2?1:*((int*)_tmp117)!= 5)goto _LL177;_tmp120=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp117)->f1;_LL176: Cyc_Absyndump_dump(_tag_arr("&",sizeof(unsigned char),2));
Cyc_Absyndump_dumppat(_tmp120);goto _LL164;_LL177: if(_tmp117 <= (void*)2?1:*((int*)
_tmp117)!= 6)goto _LL179;_tmp121=((struct Cyc_Absyn_Reference_p_struct*)_tmp117)->f1;
_LL178: Cyc_Absyndump_dump(_tag_arr("*",sizeof(unsigned char),2));Cyc_Absyndump_dumpqvar(
_tmp121->name);goto _LL164;_LL179: if(_tmp117 <= (void*)2?1:*((int*)_tmp117)!= 11)
goto _LL17B;_tmp122=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp117)->f1;_LL17A: Cyc_Absyndump_dumpqvar(
_tmp122);goto _LL164;_LL17B: if(_tmp117 <= (void*)2?1:*((int*)_tmp117)!= 12)goto
_LL17D;_tmp123=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp117)->f1;_tmp124=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp117)->f2;_LL17C: Cyc_Absyndump_dumpqvar(
_tmp123);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp124,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));goto _LL164;
_LL17D: if(_tmp117 <= (void*)2?1:*((int*)_tmp117)!= 7)goto _LL17F;_tmp125=((struct
Cyc_Absyn_Aggr_p_struct*)_tmp117)->f1;_tmp126=(void*)_tmp125.aggr_info;_tmp127=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp117)->f2;_tmp128=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp117)->f3;_LL17E: {struct _tuple0*_tmp132;struct _tuple3 _tmp131=Cyc_Absyn_aggr_kinded_name(
_tmp126);_tmp132=_tmp131.f2;Cyc_Absyndump_dumpqvar(_tmp132);Cyc_Absyndump_dump_char((
int)'{');((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,_tmp127,_tag_arr("[",sizeof(unsigned char),2),_tag_arr("]",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));((void(*)(void(*
f)(struct _tuple10*),struct Cyc_List_List*l,struct _tagged_arr start,struct
_tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp128,_tag_arr("",sizeof(unsigned char),1),_tag_arr("}",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LL164;}_LL17F: if(_tmp117 <= (void*)
2?1:*((int*)_tmp117)!= 8)goto _LL181;_tmp129=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp117)->f2;_tmp12A=((struct Cyc_Absyn_Tunion_p_struct*)_tmp117)->f3;_LL180: Cyc_Absyndump_dumpqvar(
_tmp129->name);if(_tmp12A != 0)((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
l,struct _tagged_arr start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp12A,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(",",sizeof(unsigned char),2));goto _LL164;
_LL181: if(_tmp117 <= (void*)2?1:*((int*)_tmp117)!= 9)goto _LL183;_tmp12B=((struct
Cyc_Absyn_Enum_p_struct*)_tmp117)->f2;_LL182: _tmp12C=_tmp12B;goto _LL184;_LL183:
if(_tmp117 <= (void*)2?1:*((int*)_tmp117)!= 10)goto _LL164;_tmp12C=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp117)->f2;_LL184: Cyc_Absyndump_dumpqvar(_tmp12C->name);goto _LL164;_LL164:;}
void Cyc_Absyndump_dumptunionfield(struct Cyc_Absyn_Tunionfield*ef){Cyc_Absyndump_dumpqvar(
ef->name);if(ef->typs != 0)Cyc_Absyndump_dumpargs(ef->typs);}void Cyc_Absyndump_dumptunionfields(
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
_tmp134;struct _tagged_arr*_tmp135;struct Cyc_Absyn_Tqual _tmp136;void*_tmp137;
struct Cyc_Absyn_Exp*_tmp138;struct Cyc_List_List*_tmp139;struct Cyc_Absyn_Aggrfield*
_tmp133=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp134=*_tmp133;_tmp135=_tmp134.name;
_tmp136=_tmp134.tq;_tmp137=(void*)_tmp134.type;_tmp138=_tmp134.width;_tmp139=
_tmp134.attributes;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct
_tagged_arr*),struct _tagged_arr*))Cyc_Absyndump_dumptqtd)(_tmp136,_tmp137,Cyc_Absyndump_dump_str,
_tmp135);Cyc_Absyndump_dumpatts(_tmp139);if(_tmp138 != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp138));}Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(
td->name);Cyc_Absyndump_dumptvars(td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(
struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(fd->attributes);Cyc_Absyndump_dumpqvar(
fd->name);}static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple3*pr){{void*
_tmp13A=(*pr).f1;_LL186: if((int)_tmp13A != 8)goto _LL188;_LL187: goto _LL185;_LL188:
if((int)_tmp13A != 0)goto _LL18A;_LL189: Cyc_Absyndump_dump(_tag_arr("_stdcall",
sizeof(unsigned char),9));goto _LL185;_LL18A: if((int)_tmp13A != 1)goto _LL18C;
_LL18B: Cyc_Absyndump_dump(_tag_arr("_cdecl",sizeof(unsigned char),7));goto _LL185;
_LL18C: if((int)_tmp13A != 2)goto _LL18E;_LL18D: Cyc_Absyndump_dump(_tag_arr("_fastcall",
sizeof(unsigned char),10));goto _LL185;_LL18E:;_LL18F: goto _LL185;_LL185:;}Cyc_Absyndump_dumpqvar((*
pr).f2);}static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv(fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}
static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){for(0;vds != 0;vds=vds->tl){
Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl != 0)
Cyc_Absyndump_dump_char((int)',');}}void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Vardecl _tmp13C;void*_tmp13D;
struct _tuple0*_tmp13E;struct Cyc_Absyn_Tqual _tmp13F;void*_tmp140;struct Cyc_Absyn_Exp*
_tmp141;struct Cyc_List_List*_tmp142;struct Cyc_Absyn_Vardecl*_tmp13B=vd;_tmp13C=*
_tmp13B;_tmp13D=(void*)_tmp13C.sc;_tmp13E=_tmp13C.name;_tmp13F=_tmp13C.tq;
_tmp140=(void*)_tmp13C.type;_tmp141=_tmp13C.initializer;_tmp142=_tmp13C.attributes;
Cyc_Absyndump_dumploc(loc);if(Cyc_Absyndump_to_VC){Cyc_Absyndump_dumpatts(
_tmp142);Cyc_Absyndump_dumpscope(_tmp13D);{struct Cyc_Absyn_Tqual _tmp144;void*
_tmp145;struct Cyc_List_List*_tmp146;struct _tuple5 _tmp143=Cyc_Absynpp_to_tms(
_tmp13F,_tmp140);_tmp144=_tmp143.f1;_tmp145=_tmp143.f2;_tmp146=_tmp143.f3;{void*
call_conv=(void*)8;{struct Cyc_List_List*tms2=_tmp146;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp147=(void*)tms2->hd;struct Cyc_List_List*_tmp148;_LL191: if(_tmp147 <= (
void*)1?1:*((int*)_tmp147)!= 4)goto _LL193;_tmp148=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmp147)->f2;_LL192: for(0;_tmp148 != 0;_tmp148=_tmp148->tl){void*_tmp149=(void*)
_tmp148->hd;_LL196: if((int)_tmp149 != 0)goto _LL198;_LL197: call_conv=(void*)0;goto
_LL195;_LL198: if((int)_tmp149 != 1)goto _LL19A;_LL199: call_conv=(void*)1;goto
_LL195;_LL19A: if((int)_tmp149 != 2)goto _LL19C;_LL19B: call_conv=(void*)2;goto
_LL195;_LL19C:;_LL19D: goto _LL195;_LL195:;}goto _LL190;_LL193:;_LL194: goto _LL190;
_LL190:;}}Cyc_Absyndump_dumptq(_tmp144);Cyc_Absyndump_dumpntyp(_tmp145);{struct
_tuple3 _tmp14A=({struct _tuple3 _tmp14B;_tmp14B.f1=call_conv;_tmp14B.f2=_tmp13E;
_tmp14B;});((void(*)(struct Cyc_List_List*tms,void(*f)(struct _tuple3*),struct
_tuple3*a))Cyc_Absyndump_dumptms)(Cyc_List_imp_rev(_tmp146),Cyc_Absyndump_dump_callconv_qvar,&
_tmp14A);}}}}else{if(_tmp13D == (void*)3?Cyc_Absyndump_qvar_to_Cids: 0){void*
_tmp14C=Cyc_Tcutil_compress(_tmp140);_LL19F: if(_tmp14C <= (void*)3?1:*((int*)
_tmp14C)!= 8)goto _LL1A1;_LL1A0: goto _LL19E;_LL1A1:;_LL1A2: Cyc_Absyndump_dumpscope(
_tmp13D);_LL19E:;}else{Cyc_Absyndump_dumpscope(_tmp13D);}((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp13F,
_tmp140,Cyc_Absyndump_dumpqvar,_tmp13E);Cyc_Absyndump_dumpatts(_tmp142);}if(
_tmp141 != 0){Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(_tmp141));}Cyc_Absyndump_dump_semi();}void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*d){void*_tmp14D=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp14E;
struct Cyc_Absyn_Fndecl*_tmp14F;struct Cyc_Absyn_Aggrdecl*_tmp150;struct Cyc_Absyn_Tuniondecl*
_tmp151;struct Cyc_Absyn_Tuniondecl _tmp152;void*_tmp153;struct _tuple0*_tmp154;
struct Cyc_List_List*_tmp155;struct Cyc_Core_Opt*_tmp156;int _tmp157;struct Cyc_Absyn_Enumdecl*
_tmp158;struct Cyc_Absyn_Enumdecl _tmp159;void*_tmp15A;struct _tuple0*_tmp15B;
struct Cyc_Core_Opt*_tmp15C;struct Cyc_Absyn_Pat*_tmp15D;struct Cyc_Absyn_Exp*
_tmp15E;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_Typedefdecl*_tmp160;struct
_tagged_arr*_tmp161;struct Cyc_List_List*_tmp162;struct _tuple0*_tmp163;struct Cyc_List_List*
_tmp164;struct Cyc_List_List*_tmp165;_LL1A4: if(*((int*)_tmp14D)!= 0)goto _LL1A6;
_tmp14E=((struct Cyc_Absyn_Var_d_struct*)_tmp14D)->f1;_LL1A5: Cyc_Absyndump_dumpvardecl(
_tmp14E,d->loc);goto _LL1A3;_LL1A6: if(*((int*)_tmp14D)!= 1)goto _LL1A8;_tmp14F=((
struct Cyc_Absyn_Fn_d_struct*)_tmp14D)->f1;_LL1A7: Cyc_Absyndump_dumploc(d->loc);
if(Cyc_Absyndump_to_VC)Cyc_Absyndump_dumpatts(_tmp14F->attributes);if(_tmp14F->is_inline){
if(Cyc_Absyndump_to_VC)Cyc_Absyndump_dump(_tag_arr("__inline",sizeof(
unsigned char),9));else{Cyc_Absyndump_dump(_tag_arr("inline",sizeof(
unsigned char),7));}}Cyc_Absyndump_dumpscope((void*)_tmp14F->sc);{void*t=(void*)({
struct Cyc_Absyn_FnType_struct*_tmp167=_cycalloc(sizeof(*_tmp167));_tmp167[0]=({
struct Cyc_Absyn_FnType_struct _tmp168;_tmp168.tag=8;_tmp168.f1=({struct Cyc_Absyn_FnInfo
_tmp169;_tmp169.tvars=_tmp14F->tvs;_tmp169.effect=_tmp14F->effect;_tmp169.ret_typ=(
void*)((void*)_tmp14F->ret_type);_tmp169.args=((struct Cyc_List_List*(*)(struct
_tuple1*(*f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp14F->args);_tmp169.c_varargs=_tmp14F->c_varargs;_tmp169.cyc_varargs=_tmp14F->cyc_varargs;
_tmp169.rgn_po=_tmp14F->rgn_po;_tmp169.attributes=0;_tmp169;});_tmp168;});
_tmp167;});((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual _tmp166;
_tmp166.q_const=0;_tmp166.q_volatile=0;_tmp166.q_restrict=0;_tmp166;}),t,Cyc_Absyndump_to_VC?
Cyc_Absyndump_dump_callconv_fdqvar: Cyc_Absyndump_dump_atts_qvar,_tmp14F);Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstmt(_tmp14F->body);Cyc_Absyndump_dump_char((int)'}');
goto _LL1A3;}_LL1A8: if(*((int*)_tmp14D)!= 4)goto _LL1AA;_tmp150=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp14D)->f1;_LL1A9: Cyc_Absyndump_dumpscope((void*)_tmp150->sc);Cyc_Absyndump_dumpaggr_kind((
void*)_tmp150->kind);Cyc_Absyndump_dumpqvar(_tmp150->name);Cyc_Absyndump_dumptvars(
_tmp150->tvs);if(_tmp150->fields == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');if(_tmp150->exist_vars != 0)((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct _tagged_arr start,struct _tagged_arr end,struct
_tagged_arr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp150->exist_vars))->v,_tag_arr("[",sizeof(
unsigned char),2),_tag_arr("]",sizeof(unsigned char),2),_tag_arr(",",sizeof(
unsigned char),2));Cyc_Absyndump_dumpaggrfields((struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp150->fields))->v);Cyc_Absyndump_dump(_tag_arr("}",
sizeof(unsigned char),2));Cyc_Absyndump_dumpatts(_tmp150->attributes);Cyc_Absyndump_dump(
_tag_arr(";",sizeof(unsigned char),2));}goto _LL1A3;_LL1AA: if(*((int*)_tmp14D)!= 
5)goto _LL1AC;_tmp151=((struct Cyc_Absyn_Tunion_d_struct*)_tmp14D)->f1;_tmp152=*
_tmp151;_tmp153=(void*)_tmp152.sc;_tmp154=_tmp152.name;_tmp155=_tmp152.tvs;
_tmp156=_tmp152.fields;_tmp157=_tmp152.is_xtunion;_LL1AB: Cyc_Absyndump_dumpscope(
_tmp153);Cyc_Absyndump_dump(_tmp157?_tag_arr("xtunion",sizeof(unsigned char),8):
_tag_arr("tunion",sizeof(unsigned char),7));Cyc_Absyndump_dumpqvar(_tmp154);Cyc_Absyndump_dumptvars(
_tmp155);if(_tmp156 == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumptunionfields((struct Cyc_List_List*)_tmp156->v);Cyc_Absyndump_dump_nospace(
_tag_arr("};",sizeof(unsigned char),3));}goto _LL1A3;_LL1AC: if(*((int*)_tmp14D)!= 
6)goto _LL1AE;_tmp158=((struct Cyc_Absyn_Enum_d_struct*)_tmp14D)->f1;_tmp159=*
_tmp158;_tmp15A=(void*)_tmp159.sc;_tmp15B=_tmp159.name;_tmp15C=_tmp159.fields;
_LL1AD: Cyc_Absyndump_dumpscope(_tmp15A);Cyc_Absyndump_dump(_tag_arr("enum ",
sizeof(unsigned char),6));Cyc_Absyndump_dumpqvar(_tmp15B);if(_tmp15C == 0)Cyc_Absyndump_dump_semi();
else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)
_tmp15C->v);Cyc_Absyndump_dump_nospace(_tag_arr("};",sizeof(unsigned char),3));}
return;_LL1AE: if(*((int*)_tmp14D)!= 2)goto _LL1B0;_tmp15D=((struct Cyc_Absyn_Let_d_struct*)
_tmp14D)->f1;_tmp15E=((struct Cyc_Absyn_Let_d_struct*)_tmp14D)->f4;_LL1AF: Cyc_Absyndump_dump(
_tag_arr("let",sizeof(unsigned char),4));Cyc_Absyndump_dumppat(_tmp15D);Cyc_Absyndump_dump_char((
int)'=');Cyc_Absyndump_dumpexp(_tmp15E);Cyc_Absyndump_dump_semi();goto _LL1A3;
_LL1B0: if(*((int*)_tmp14D)!= 3)goto _LL1B2;_tmp15F=((struct Cyc_Absyn_Letv_d_struct*)
_tmp14D)->f1;_LL1B1: Cyc_Absyndump_dump(_tag_arr("let ",sizeof(unsigned char),5));
Cyc_Absyndump_dumpids(_tmp15F);Cyc_Absyndump_dump_semi();goto _LL1A3;_LL1B2: if(*((
int*)_tmp14D)!= 7)goto _LL1B4;_tmp160=((struct Cyc_Absyn_Typedef_d_struct*)_tmp14D)->f1;
_LL1B3: if(!Cyc_Absyndump_expand_typedefs?1: Cyc_Absynpp_is_anon_aggrtype((void*)
_tmp160->defn)){Cyc_Absyndump_dump(_tag_arr("typedef",sizeof(unsigned char),8));((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(({struct Cyc_Absyn_Tqual
_tmp16A;_tmp16A.q_const=0;_tmp16A.q_volatile=0;_tmp16A.q_restrict=0;_tmp16A;}),(
void*)_tmp160->defn,Cyc_Absyndump_dumptypedefname,_tmp160);Cyc_Absyndump_dump_semi();}
goto _LL1A3;_LL1B4: if(*((int*)_tmp14D)!= 8)goto _LL1B6;_tmp161=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp14D)->f1;_tmp162=((struct Cyc_Absyn_Namespace_d_struct*)_tmp14D)->f2;_LL1B5:
Cyc_Absyndump_dump(_tag_arr("namespace",sizeof(unsigned char),10));Cyc_Absyndump_dump_str(
_tmp161);Cyc_Absyndump_dump_char((int)'{');for(0;_tmp162 != 0;_tmp162=_tmp162->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp162->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1A3;_LL1B6: if(*((int*)_tmp14D)!= 9)goto _LL1B8;_tmp163=((struct
Cyc_Absyn_Using_d_struct*)_tmp14D)->f1;_tmp164=((struct Cyc_Absyn_Using_d_struct*)
_tmp14D)->f2;_LL1B7: Cyc_Absyndump_dump(_tag_arr("using",sizeof(unsigned char),6));
Cyc_Absyndump_dumpqvar(_tmp163);Cyc_Absyndump_dump_char((int)'{');for(0;_tmp164
!= 0;_tmp164=_tmp164->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp164->hd);}
Cyc_Absyndump_dump_char((int)'}');goto _LL1A3;_LL1B8: if(*((int*)_tmp14D)!= 10)
goto _LL1A3;_tmp165=((struct Cyc_Absyn_ExternC_d_struct*)_tmp14D)->f1;_LL1B9: Cyc_Absyndump_dump(
_tag_arr("extern \"C\" {",sizeof(unsigned char),13));for(0;_tmp165 != 0;_tmp165=
_tmp165->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp165->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1A3;_LL1A3:;}static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*
e){unsigned int i=Cyc_Evexp_eval_const_uint_exp(e);if(i != 1){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}void Cyc_Absyndump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*
_tmp16B=(void*)tms->hd;void*_tmp16C;struct Cyc_Absyn_Exp*_tmp16D;void*_tmp16E;
struct Cyc_Absyn_Tqual _tmp16F;void*_tmp170;struct Cyc_Absyn_Exp*_tmp171;void*
_tmp172;struct Cyc_Absyn_Tqual _tmp173;void*_tmp174;void*_tmp175;struct Cyc_Absyn_Tqual
_tmp176;void*_tmp177;struct Cyc_Absyn_Exp*_tmp178;void*_tmp179;struct Cyc_Absyn_Tvar*
_tmp17A;struct Cyc_Absyn_Tqual _tmp17B;void*_tmp17C;struct Cyc_Absyn_Exp*_tmp17D;
void*_tmp17E;struct Cyc_Absyn_Tvar*_tmp17F;struct Cyc_Absyn_Tqual _tmp180;void*
_tmp181;void*_tmp182;struct Cyc_Absyn_Tvar*_tmp183;struct Cyc_Absyn_Tqual _tmp184;
_LL1BB: if(_tmp16B <= (void*)1?1:*((int*)_tmp16B)!= 1)goto _LL1BD;_tmp16C=(void*)((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp16B)->f1;if(_tmp16C <= (void*)1?1:*((int*)
_tmp16C)!= 1)goto _LL1BD;_tmp16D=((struct Cyc_Absyn_Nullable_ps_struct*)_tmp16C)->f1;
_tmp16E=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp16B)->f2;if((int)
_tmp16E != 2)goto _LL1BD;_tmp16F=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp16B)->f3;
_LL1BC: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dump_upperbound(_tmp16D);
Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1BD: if(_tmp16B <= (void*)1?1:*((int*)
_tmp16B)!= 1)goto _LL1BF;_tmp170=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16B)->f1;if(_tmp170 <= (void*)1?1:*((int*)_tmp170)!= 0)goto _LL1BF;_tmp171=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmp170)->f1;_tmp172=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16B)->f2;if((int)_tmp172 != 2)goto _LL1BF;_tmp173=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16B)->f3;_LL1BE: Cyc_Absyndump_dump_char((int)'@');Cyc_Absyndump_dump_upperbound(
_tmp171);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1BF: if(_tmp16B <= (void*)1?
1:*((int*)_tmp16B)!= 1)goto _LL1C1;_tmp174=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16B)->f1;if((int)_tmp174 != 0)goto _LL1C1;_tmp175=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16B)->f2;if((int)_tmp175 != 2)goto _LL1C1;_tmp176=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16B)->f3;_LL1C0: Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumptms(tms->tl,
f,a);return;_LL1C1: if(_tmp16B <= (void*)1?1:*((int*)_tmp16B)!= 1)goto _LL1C3;
_tmp177=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp16B)->f1;if(_tmp177 <= (
void*)1?1:*((int*)_tmp177)!= 1)goto _LL1C3;_tmp178=((struct Cyc_Absyn_Nullable_ps_struct*)
_tmp177)->f1;_tmp179=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp16B)->f2;
if(_tmp179 <= (void*)3?1:*((int*)_tmp179)!= 1)goto _LL1C3;_tmp17A=((struct Cyc_Absyn_VarType_struct*)
_tmp179)->f1;_tmp17B=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp16B)->f3;_LL1C2:
Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dump_upperbound(_tmp178);Cyc_Absyndump_dump_str(
_tmp17A->name);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL1C3: if(_tmp16B <= (
void*)1?1:*((int*)_tmp16B)!= 1)goto _LL1C5;_tmp17C=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16B)->f1;if(_tmp17C <= (void*)1?1:*((int*)_tmp17C)!= 0)goto _LL1C5;_tmp17D=((
struct Cyc_Absyn_NonNullable_ps_struct*)_tmp17C)->f1;_tmp17E=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16B)->f2;if(_tmp17E <= (void*)3?1:*((int*)_tmp17E)!= 1)goto _LL1C5;_tmp17F=((
struct Cyc_Absyn_VarType_struct*)_tmp17E)->f1;_tmp180=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp16B)->f3;_LL1C4: Cyc_Absyndump_dump_char((int)'@');Cyc_Absyndump_dump_upperbound(
_tmp17D);Cyc_Absyndump_dump_str(_tmp17F->name);Cyc_Absyndump_dumptms(tms->tl,f,a);
return;_LL1C5: if(_tmp16B <= (void*)1?1:*((int*)_tmp16B)!= 1)goto _LL1C7;_tmp181=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp16B)->f1;if((int)_tmp181 != 0)
goto _LL1C7;_tmp182=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp16B)->f2;if(
_tmp182 <= (void*)3?1:*((int*)_tmp182)!= 1)goto _LL1C7;_tmp183=((struct Cyc_Absyn_VarType_struct*)
_tmp182)->f1;_tmp184=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp16B)->f3;_LL1C6:
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dump_str(_tmp183->name);Cyc_Absyndump_dumptms(
tms->tl,f,a);return;_LL1C7: if(_tmp16B <= (void*)1?1:*((int*)_tmp16B)!= 1)goto
_LL1C9;_LL1C8:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp185=
_cycalloc(sizeof(*_tmp185));_tmp185[0]=({struct Cyc_Core_Impossible_struct _tmp186;
_tmp186.tag=Cyc_Core_Impossible;_tmp186.f1=_tag_arr("dumptms: bad Pointer_mod",
sizeof(unsigned char),25);_tmp186;});_tmp185;}));_LL1C9:;_LL1CA: {int
next_is_pointer=0;if(tms->tl != 0){void*_tmp187=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;_LL1CC: if(_tmp187 <= (void*)1?1:*((int*)_tmp187)!= 1)
goto _LL1CE;_LL1CD: next_is_pointer=1;goto _LL1CB;_LL1CE:;_LL1CF: goto _LL1CB;_LL1CB:;}
if(next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(tms->tl,
f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp188=(void*)
tms->hd;struct Cyc_Absyn_Exp*_tmp189;void*_tmp18A;struct Cyc_List_List*_tmp18B;int
_tmp18C;struct Cyc_Absyn_VarargInfo*_tmp18D;struct Cyc_Core_Opt*_tmp18E;struct Cyc_List_List*
_tmp18F;void*_tmp190;struct Cyc_List_List*_tmp191;struct Cyc_Position_Segment*
_tmp192;struct Cyc_List_List*_tmp193;struct Cyc_Position_Segment*_tmp194;int
_tmp195;struct Cyc_List_List*_tmp196;void*_tmp197;void*_tmp198;_LL1D1: if((int)
_tmp188 != 0)goto _LL1D3;_LL1D2: Cyc_Absyndump_dump(_tag_arr("[]",sizeof(
unsigned char),3));goto _LL1D0;_LL1D3: if(_tmp188 <= (void*)1?1:*((int*)_tmp188)!= 
0)goto _LL1D5;_tmp189=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp188)->f1;
_LL1D4: Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp189);Cyc_Absyndump_dump_char((
int)']');goto _LL1D0;_LL1D5: if(_tmp188 <= (void*)1?1:*((int*)_tmp188)!= 2)goto
_LL1D7;_tmp18A=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp188)->f1;if(*((
int*)_tmp18A)!= 1)goto _LL1D7;_tmp18B=((struct Cyc_Absyn_WithTypes_struct*)_tmp18A)->f1;
_tmp18C=((struct Cyc_Absyn_WithTypes_struct*)_tmp18A)->f2;_tmp18D=((struct Cyc_Absyn_WithTypes_struct*)
_tmp18A)->f3;_tmp18E=((struct Cyc_Absyn_WithTypes_struct*)_tmp18A)->f4;_tmp18F=((
struct Cyc_Absyn_WithTypes_struct*)_tmp18A)->f5;_LL1D6: Cyc_Absyndump_dumpfunargs(
_tmp18B,_tmp18C,_tmp18D,_tmp18E,_tmp18F);goto _LL1D0;_LL1D7: if(_tmp188 <= (void*)1?
1:*((int*)_tmp188)!= 2)goto _LL1D9;_tmp190=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp188)->f1;if(*((int*)_tmp190)!= 0)goto _LL1D9;_tmp191=((struct Cyc_Absyn_NoTypes_struct*)
_tmp190)->f1;_tmp192=((struct Cyc_Absyn_NoTypes_struct*)_tmp190)->f2;_LL1D8:((
void(*)(void(*f)(struct _tagged_arr*),struct Cyc_List_List*l,struct _tagged_arr
start,struct _tagged_arr end,struct _tagged_arr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,
_tmp191,_tag_arr("(",sizeof(unsigned char),2),_tag_arr(")",sizeof(unsigned char),
2),_tag_arr(",",sizeof(unsigned char),2));goto _LL1D0;_LL1D9: if(_tmp188 <= (void*)
1?1:*((int*)_tmp188)!= 3)goto _LL1DB;_tmp193=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp188)->f1;_tmp194=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp188)->f2;
_tmp195=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp188)->f3;_LL1DA: if(_tmp195)
Cyc_Absyndump_dumpkindedtvars(_tmp193);else{Cyc_Absyndump_dumptvars(_tmp193);}
goto _LL1D0;_LL1DB: if(_tmp188 <= (void*)1?1:*((int*)_tmp188)!= 4)goto _LL1DD;
_tmp196=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp188)->f2;_LL1DC: Cyc_Absyndump_dumpatts(
_tmp196);goto _LL1D0;_LL1DD: if(_tmp188 <= (void*)1?1:*((int*)_tmp188)!= 1)goto
_LL1D0;_tmp197=(void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp188)->f1;_tmp198=(
void*)((struct Cyc_Absyn_Pointer_mod_struct*)_tmp188)->f2;_LL1DE:(int)_throw((
void*)({struct Cyc_Core_Impossible_struct*_tmp199=_cycalloc(sizeof(*_tmp199));
_tmp199[0]=({struct Cyc_Core_Impossible_struct _tmp19A;_tmp19A.tag=Cyc_Core_Impossible;
_tmp19A.f1=_tag_arr("dumptms",sizeof(unsigned char),8);_tmp19A;});_tmp199;}));
_LL1D0:;}return;}_LL1BA:;}}void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,
void*t,void(*f)(void*),void*a){struct Cyc_Absyn_Tqual _tmp19C;void*_tmp19D;struct
Cyc_List_List*_tmp19E;struct _tuple5 _tmp19B=Cyc_Absynpp_to_tms(tq,t);_tmp19C=
_tmp19B.f1;_tmp19D=_tmp19B.f2;_tmp19E=_tmp19B.f3;Cyc_Absyndump_dumptq(_tmp19C);
Cyc_Absyndump_dumpntyp(_tmp19D);Cyc_Absyndump_dumptms(Cyc_List_imp_rev(_tmp19E),
f,a);}void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*
f){Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=f;for(0;tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)tdl->hd);}({void*_tmp19F[0]={};Cyc_Std_fprintf(f,_tag_arr("\n",
sizeof(unsigned char),2),_tag_arr(_tmp19F,sizeof(void*),0));});}
