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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{char*
tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char
Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};struct
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _tagged_arr Cyc_Absynpp_typ2string(
void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
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
_tmp1;char _tmp2;void*_tmp3;short _tmp4;_LL1: if(_tmp0 <= (void*)1?1:*((int*)_tmp0)
!= 0)goto _LL3;_tmp1=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp0)->f1;_tmp2=((
struct Cyc_Absyn_Char_c_struct*)_tmp0)->f2;_LL2: return(void*)({struct Cyc_Absyn_Int_c_struct*
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
sizeof(char),23),_tag_arr(_tmpB,sizeof(void*),0));});return 0;_LL7:;}int Cyc_Evexp_eval_const_bool_exp(
struct Cyc_Absyn_Exp*e){void*_tmpC=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(
e));int _tmpD;long long _tmpE;_LLD: if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 2)goto
_LLF;_tmpD=((struct Cyc_Absyn_Int_c_struct*)_tmpC)->f2;_LLE: return _tmpD != 0;_LLF:
if(_tmpC <= (void*)1?1:*((int*)_tmpC)!= 3)goto _LL11;_tmpE=((struct Cyc_Absyn_LongLong_c_struct*)
_tmpC)->f2;_LL10: return _tmpE != 0;_LL11: if((int)_tmpC != 0)goto _LL13;_LL12: return 0;
_LL13:;_LL14:({void*_tmpF[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("expecting bool",
sizeof(char),15),_tag_arr(_tmpF,sizeof(void*),0));});return 0;_LLC:;}struct
_tuple4{struct Cyc_Absyn_Tqual f1;void*f2;};int Cyc_Evexp_okay_szofarg(void*t){void*
_tmp10=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp11;struct Cyc_Core_Opt*
_tmp12;struct Cyc_Core_Opt _tmp13;void*_tmp14;struct Cyc_Absyn_TunionFieldInfo
_tmp15;void*_tmp16;struct Cyc_Absyn_Tunionfield*_tmp17;struct Cyc_List_List*_tmp18;
struct Cyc_Absyn_PtrInfo _tmp19;struct Cyc_Absyn_Conref*_tmp1A;void*_tmp1B;struct
Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_AggrInfo _tmp1D;void*_tmp1E;struct Cyc_List_List*
_tmp1F;struct Cyc_Absyn_Typedefdecl*_tmp20;_LL16: if((int)_tmp10 != 0)goto _LL18;
_LL17: return 0;_LL18: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 1)goto _LL1A;_tmp11=((
struct Cyc_Absyn_VarType_struct*)_tmp10)->f1;_LL19: {void*_tmp21=Cyc_Tcutil_tvar_kind(
_tmp11);_LL4D: if((int)_tmp21 != 2)goto _LL4F;_LL4E: return 1;_LL4F:;_LL50: return 0;
_LL4C:;}_LL1A: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 0)goto _LL1C;_tmp12=((
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
void*_tmp22=(void*)(Cyc_Absyn_compress_conref(_tmp1A))->v;_LL52: if(_tmp22 <= (
void*)1?1:*((int*)_tmp22)!= 0)goto _LL54;_LL53: return 1;_LL54:;_LL55: return 0;_LL51:;}
_LL28: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 14)goto _LL2A;_LL29: goto _LL2B;
_LL2A: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 17)goto _LL2C;_LL2B: goto _LL2D;
_LL2C: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 5)goto _LL2E;_LL2D: goto _LL2F;_LL2E:
if((int)_tmp10 != 1)goto _LL30;_LL2F: goto _LL31;_LL30: if(_tmp10 <= (void*)3?1:*((int*)
_tmp10)!= 6)goto _LL32;_LL31: return 1;_LL32: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)
!= 7)goto _LL34;_tmp1B=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp10)->f1;
_tmp1C=((struct Cyc_Absyn_ArrayType_struct*)_tmp10)->f3;_LL33: return _tmp1C != 0;
_LL34: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 8)goto _LL36;_LL35: return 0;_LL36:
if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 15)goto _LL38;_LL37: return 1;_LL38: if(
_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 10)goto _LL3A;_tmp1D=((struct Cyc_Absyn_AggrType_struct*)
_tmp10)->f1;_tmp1E=(void*)_tmp1D.aggr_info;_LL39: {struct Cyc_Absyn_Aggrdecl*ad;{
void*_tmp23=_tmp1E;struct Cyc_Absyn_Aggrdecl**_tmp24;struct Cyc_Absyn_Aggrdecl*
_tmp25;_LL57: if(*((int*)_tmp23)!= 0)goto _LL59;_LL58:({void*_tmp26[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("szof on unchecked StructType",
sizeof(char),29),_tag_arr(_tmp26,sizeof(void*),0));});_LL59: if(*((int*)_tmp23)!= 
1)goto _LL56;_tmp24=((struct Cyc_Absyn_KnownAggr_struct*)_tmp23)->f1;_tmp25=*
_tmp24;_LL5A: ad=_tmp25;_LL56:;}if(ad->impl == 0)return 0;_tmp1F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;goto _LL3B;}_LL3A: if(_tmp10 <= (void*)3?1:*((int*)
_tmp10)!= 11)goto _LL3C;_tmp1F=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp10)->f2;
_LL3B: for(0;_tmp1F != 0;_tmp1F=_tmp1F->tl){if(!Cyc_Evexp_okay_szofarg((void*)((
struct Cyc_Absyn_Aggrfield*)_tmp1F->hd)->type))return 0;}return 1;_LL3C: if(_tmp10 <= (
void*)3?1:*((int*)_tmp10)!= 12)goto _LL3E;_LL3D: goto _LL3F;_LL3E: if(_tmp10 <= (void*)
3?1:*((int*)_tmp10)!= 13)goto _LL40;_LL3F: return 1;_LL40: if(_tmp10 <= (void*)3?1:*((
int*)_tmp10)!= 18)goto _LL42;_LL41: goto _LL43;_LL42: if((int)_tmp10 != 2)goto _LL44;
_LL43: goto _LL45;_LL44: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 20)goto _LL46;
_LL45: goto _LL47;_LL46: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 19)goto _LL48;
_LL47: goto _LL49;_LL48: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 21)goto _LL4A;
_LL49: return 0;_LL4A: if(_tmp10 <= (void*)3?1:*((int*)_tmp10)!= 16)goto _LL15;_tmp20=((
struct Cyc_Absyn_TypedefType_struct*)_tmp10)->f3;_LL4B: if(_tmp20 == 0?1: _tmp20->kind
== 0)({void*_tmp27[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp29;_tmp29.tag=0;_tmp29.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(t);{void*_tmp28[1]={& _tmp29};Cyc_Std_aprintf(
_tag_arr("szof typedeftype %s",sizeof(char),20),_tag_arr(_tmp28,sizeof(void*),1));}}),
_tag_arr(_tmp27,sizeof(void*),0));});else{void*_tmp2A=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp20->kind))->v;_LL5C: if((int)_tmp2A != 2)goto _LL5E;_LL5D: return 1;
_LL5E:;_LL5F: return 0;_LL5B:;}_LL15:;}void*Cyc_Evexp_eval_const_unprimop(void*p,
struct Cyc_Absyn_Exp*e){void*cn=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(
e));struct _tuple3 _tmp2C=({struct _tuple3 _tmp2B;_tmp2B.f1=p;_tmp2B.f2=cn;_tmp2B;});
void*_tmp2D;void*_tmp2E;void*_tmp2F;void*_tmp30;int _tmp31;void*_tmp32;void*
_tmp33;void*_tmp34;int _tmp35;void*_tmp36;void*_tmp37;int _tmp38;void*_tmp39;void*
_tmp3A;_LL61: _tmp2D=_tmp2C.f1;if((int)_tmp2D != 0)goto _LL63;_LL62: return cn;_LL63:
_tmp2E=_tmp2C.f1;if((int)_tmp2E != 2)goto _LL65;_tmp2F=_tmp2C.f2;if(_tmp2F <= (void*)
1?1:*((int*)_tmp2F)!= 2)goto _LL65;_tmp30=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp2F)->f1;_tmp31=((struct Cyc_Absyn_Int_c_struct*)_tmp2F)->f2;_LL64: return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B[0]=({
struct Cyc_Absyn_Int_c_struct _tmp3C;_tmp3C.tag=2;_tmp3C.f1=(void*)((void*)0);
_tmp3C.f2=- _tmp31;_tmp3C;});_tmp3B;});_LL65: _tmp32=_tmp2C.f1;if((int)_tmp32 != 12)
goto _LL67;_tmp33=_tmp2C.f2;if(_tmp33 <= (void*)1?1:*((int*)_tmp33)!= 2)goto _LL67;
_tmp34=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp33)->f1;_tmp35=((struct Cyc_Absyn_Int_c_struct*)
_tmp33)->f2;_LL66: return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp3D=_cycalloc(
sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_Int_c_struct _tmp3E;_tmp3E.tag=2;
_tmp3E.f1=(void*)((void*)1);_tmp3E.f2=~ _tmp35;_tmp3E;});_tmp3D;});_LL67: _tmp36=
_tmp2C.f1;if((int)_tmp36 != 11)goto _LL69;_tmp37=_tmp2C.f2;if(_tmp37 <= (void*)1?1:*((
int*)_tmp37)!= 2)goto _LL69;_tmp38=((struct Cyc_Absyn_Int_c_struct*)_tmp37)->f2;
_LL68: return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp3F=_cycalloc(sizeof(*
_tmp3F));_tmp3F[0]=({struct Cyc_Absyn_Int_c_struct _tmp40;_tmp40.tag=2;_tmp40.f1=(
void*)((void*)0);_tmp40.f2=_tmp38 == 0?1: 0;_tmp40;});_tmp3F;});_LL69: _tmp39=
_tmp2C.f1;if((int)_tmp39 != 11)goto _LL6B;_tmp3A=_tmp2C.f2;if((int)_tmp3A != 0)goto
_LL6B;_LL6A: return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp41=_cycalloc(sizeof(*
_tmp41));_tmp41[0]=({struct Cyc_Absyn_Int_c_struct _tmp42;_tmp42.tag=2;_tmp42.f1=(
void*)((void*)0);_tmp42.f2=1;_tmp42;});_tmp41;});_LL6B:;_LL6C:({void*_tmp43[0]={};
Cyc_Tcutil_terr(e->loc,_tag_arr("bad constant expression",sizeof(char),24),
_tag_arr(_tmp43,sizeof(void*),0));});return cn;_LL60:;}struct _tuple5{void*f1;int
f2;};void*Cyc_Evexp_eval_const_binprimop(void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){void*_tmp44=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(e1));void*
_tmp45=Cyc_Evexp_promote_const(Cyc_Evexp_eval_const_exp(e2));void*s1;void*s2;int
i1;int i2;{void*_tmp46=_tmp44;void*_tmp47;int _tmp48;_LL6E: if(_tmp46 <= (void*)1?1:*((
int*)_tmp46)!= 2)goto _LL70;_tmp47=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp46)->f1;
_tmp48=((struct Cyc_Absyn_Int_c_struct*)_tmp46)->f2;_LL6F: s1=_tmp47;i1=_tmp48;
goto _LL6D;_LL70:;_LL71:({void*_tmp49[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("bad constant expression",
sizeof(char),24),_tag_arr(_tmp49,sizeof(void*),0));});return _tmp44;_LL6D:;}{void*
_tmp4A=_tmp45;void*_tmp4B;int _tmp4C;_LL73: if(_tmp4A <= (void*)1?1:*((int*)_tmp4A)
!= 2)goto _LL75;_tmp4B=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp4A)->f1;_tmp4C=((
struct Cyc_Absyn_Int_c_struct*)_tmp4A)->f2;_LL74: s2=_tmp4B;i2=_tmp4C;goto _LL72;
_LL75:;_LL76:({void*_tmp4D[0]={};Cyc_Tcutil_terr(e2->loc,_tag_arr("bad constant expression",
sizeof(char),24),_tag_arr(_tmp4D,sizeof(void*),0));});return _tmp44;_LL72:;}{void*
_tmp4E=p;_LL78: if((int)_tmp4E != 3)goto _LL7A;_LL79: goto _LL7B;_LL7A: if((int)_tmp4E
!= 4)goto _LL7C;_LL7B: if(i2 == 0){({void*_tmp4F[0]={};Cyc_Tcutil_terr(e2->loc,
_tag_arr("division by zero in constant expression",sizeof(char),40),_tag_arr(
_tmp4F,sizeof(void*),0));});return _tmp44;}goto _LL77;_LL7C:;_LL7D: goto _LL77;_LL77:;}{
int has_u_arg=s1 == (void*)1?1: s2 == (void*)1;unsigned int u1=(unsigned int)i1;
unsigned int u2=(unsigned int)i2;int i3=0;unsigned int u3=0;int b3=1;int use_i3=0;int
use_u3=0;int use_b3=0;{struct _tuple5 _tmp51=({struct _tuple5 _tmp50;_tmp50.f1=p;
_tmp50.f2=has_u_arg;_tmp50;});void*_tmp52;int _tmp53;void*_tmp54;int _tmp55;void*
_tmp56;int _tmp57;void*_tmp58;int _tmp59;void*_tmp5A;int _tmp5B;void*_tmp5C;int
_tmp5D;void*_tmp5E;int _tmp5F;void*_tmp60;int _tmp61;void*_tmp62;int _tmp63;void*
_tmp64;int _tmp65;void*_tmp66;void*_tmp67;void*_tmp68;int _tmp69;void*_tmp6A;int
_tmp6B;void*_tmp6C;int _tmp6D;void*_tmp6E;int _tmp6F;void*_tmp70;int _tmp71;void*
_tmp72;int _tmp73;void*_tmp74;int _tmp75;void*_tmp76;int _tmp77;void*_tmp78;void*
_tmp79;void*_tmp7A;void*_tmp7B;void*_tmp7C;void*_tmp7D;_LL7F: _tmp52=_tmp51.f1;
if((int)_tmp52 != 0)goto _LL81;_tmp53=_tmp51.f2;if(_tmp53 != 0)goto _LL81;_LL80: i3=
i1 + i2;use_i3=1;goto _LL7E;_LL81: _tmp54=_tmp51.f1;if((int)_tmp54 != 1)goto _LL83;
_tmp55=_tmp51.f2;if(_tmp55 != 0)goto _LL83;_LL82: i3=i1 * i2;use_i3=1;goto _LL7E;
_LL83: _tmp56=_tmp51.f1;if((int)_tmp56 != 2)goto _LL85;_tmp57=_tmp51.f2;if(_tmp57 != 
0)goto _LL85;_LL84: i3=i1 - i2;use_i3=1;goto _LL7E;_LL85: _tmp58=_tmp51.f1;if((int)
_tmp58 != 3)goto _LL87;_tmp59=_tmp51.f2;if(_tmp59 != 0)goto _LL87;_LL86: i3=i1 / i2;
use_i3=1;goto _LL7E;_LL87: _tmp5A=_tmp51.f1;if((int)_tmp5A != 4)goto _LL89;_tmp5B=
_tmp51.f2;if(_tmp5B != 0)goto _LL89;_LL88: i3=i1 % i2;use_i3=1;goto _LL7E;_LL89:
_tmp5C=_tmp51.f1;if((int)_tmp5C != 0)goto _LL8B;_tmp5D=_tmp51.f2;if(_tmp5D != 1)
goto _LL8B;_LL8A: u3=u1 + u2;use_u3=1;goto _LL7E;_LL8B: _tmp5E=_tmp51.f1;if((int)
_tmp5E != 1)goto _LL8D;_tmp5F=_tmp51.f2;if(_tmp5F != 1)goto _LL8D;_LL8C: u3=u1 * u2;
use_u3=1;goto _LL7E;_LL8D: _tmp60=_tmp51.f1;if((int)_tmp60 != 2)goto _LL8F;_tmp61=
_tmp51.f2;if(_tmp61 != 1)goto _LL8F;_LL8E: u3=u1 - u2;use_u3=1;goto _LL7E;_LL8F:
_tmp62=_tmp51.f1;if((int)_tmp62 != 3)goto _LL91;_tmp63=_tmp51.f2;if(_tmp63 != 1)
goto _LL91;_LL90: u3=u1 / u2;use_u3=1;goto _LL7E;_LL91: _tmp64=_tmp51.f1;if((int)
_tmp64 != 4)goto _LL93;_tmp65=_tmp51.f2;if(_tmp65 != 1)goto _LL93;_LL92: u3=u1 % u2;
use_u3=1;goto _LL7E;_LL93: _tmp66=_tmp51.f1;if((int)_tmp66 != 5)goto _LL95;_LL94: b3=
i1 == i2;use_b3=1;goto _LL7E;_LL95: _tmp67=_tmp51.f1;if((int)_tmp67 != 6)goto _LL97;
_LL96: b3=i1 != i2;use_b3=1;goto _LL7E;_LL97: _tmp68=_tmp51.f1;if((int)_tmp68 != 7)
goto _LL99;_tmp69=_tmp51.f2;if(_tmp69 != 0)goto _LL99;_LL98: b3=i1 > i2;use_b3=1;goto
_LL7E;_LL99: _tmp6A=_tmp51.f1;if((int)_tmp6A != 8)goto _LL9B;_tmp6B=_tmp51.f2;if(
_tmp6B != 0)goto _LL9B;_LL9A: b3=i1 < i2;use_b3=1;goto _LL7E;_LL9B: _tmp6C=_tmp51.f1;
if((int)_tmp6C != 9)goto _LL9D;_tmp6D=_tmp51.f2;if(_tmp6D != 0)goto _LL9D;_LL9C: b3=
i1 >= i2;use_b3=1;goto _LL7E;_LL9D: _tmp6E=_tmp51.f1;if((int)_tmp6E != 10)goto _LL9F;
_tmp6F=_tmp51.f2;if(_tmp6F != 0)goto _LL9F;_LL9E: b3=i1 <= i2;use_b3=1;goto _LL7E;
_LL9F: _tmp70=_tmp51.f1;if((int)_tmp70 != 7)goto _LLA1;_tmp71=_tmp51.f2;if(_tmp71 != 
1)goto _LLA1;_LLA0: b3=u1 > u2;use_b3=1;goto _LL7E;_LLA1: _tmp72=_tmp51.f1;if((int)
_tmp72 != 8)goto _LLA3;_tmp73=_tmp51.f2;if(_tmp73 != 1)goto _LLA3;_LLA2: b3=u1 < u2;
use_b3=1;goto _LL7E;_LLA3: _tmp74=_tmp51.f1;if((int)_tmp74 != 9)goto _LLA5;_tmp75=
_tmp51.f2;if(_tmp75 != 1)goto _LLA5;_LLA4: b3=u1 >= u2;use_b3=1;goto _LL7E;_LLA5:
_tmp76=_tmp51.f1;if((int)_tmp76 != 10)goto _LLA7;_tmp77=_tmp51.f2;if(_tmp77 != 1)
goto _LLA7;_LLA6: b3=u1 <= u2;use_b3=1;goto _LL7E;_LLA7: _tmp78=_tmp51.f1;if((int)
_tmp78 != 13)goto _LLA9;_LLA8: u3=u1 & u2;use_u3=1;goto _LL7E;_LLA9: _tmp79=_tmp51.f1;
if((int)_tmp79 != 14)goto _LLAB;_LLAA: u3=u1 | u2;use_u3=1;goto _LL7E;_LLAB: _tmp7A=
_tmp51.f1;if((int)_tmp7A != 15)goto _LLAD;_LLAC: u3=u1 ^ u2;use_u3=1;goto _LL7E;_LLAD:
_tmp7B=_tmp51.f1;if((int)_tmp7B != 16)goto _LLAF;_LLAE: u3=u1 << u2;use_u3=1;goto
_LL7E;_LLAF: _tmp7C=_tmp51.f1;if((int)_tmp7C != 17)goto _LLB1;_LLB0: u3=u1 >> u2;
use_u3=1;goto _LL7E;_LLB1: _tmp7D=_tmp51.f1;if((int)_tmp7D != 18)goto _LLB3;_LLB2:({
void*_tmp7E[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr(">>> NOT IMPLEMENTED",sizeof(
char),20),_tag_arr(_tmp7E,sizeof(void*),0));});return _tmp44;_LLB3:;_LLB4:({void*
_tmp7F[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("bad constant expression",sizeof(
char),24),_tag_arr(_tmp7F,sizeof(void*),0));});return _tmp44;_LL7E:;}if(use_i3)
return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp80=_cycalloc(sizeof(*_tmp80));
_tmp80[0]=({struct Cyc_Absyn_Int_c_struct _tmp81;_tmp81.tag=2;_tmp81.f1=(void*)((
void*)0);_tmp81.f2=i3;_tmp81;});_tmp80;});if(use_u3)return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82[0]=({struct Cyc_Absyn_Int_c_struct _tmp83;
_tmp83.tag=2;_tmp83.f1=(void*)((void*)1);_tmp83.f2=(int)u3;_tmp83;});_tmp82;});
if(use_b3)return(void*)({struct Cyc_Absyn_Int_c_struct*_tmp84=_cycalloc(sizeof(*
_tmp84));_tmp84[0]=({struct Cyc_Absyn_Int_c_struct _tmp85;_tmp85.tag=2;_tmp85.f1=(
void*)((void*)0);_tmp85.f2=b3?1: 0;_tmp85;});_tmp84;});({void*_tmp86[0]={};((int(*)(
struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Evexp::eval_const_binop",
sizeof(char),24),_tag_arr(_tmp86,sizeof(void*),0));});}}void*Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp*e){void*cn;{void*_tmp87=(void*)e->r;void*_tmp88;struct Cyc_Absyn_Exp*
_tmp89;struct Cyc_Absyn_Exp*_tmp8A;struct Cyc_Absyn_Exp*_tmp8B;void*_tmp8C;struct
Cyc_List_List*_tmp8D;void*_tmp8E;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Enumfield*
_tmp90;struct Cyc_Absyn_Enumfield*_tmp91;_LLB6: if(*((int*)_tmp87)!= 0)goto _LLB8;
_tmp88=(void*)((struct Cyc_Absyn_Const_e_struct*)_tmp87)->f1;_LLB7: return _tmp88;
_LLB8: if(*((int*)_tmp87)!= 6)goto _LLBA;_tmp89=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp87)->f1;_tmp8A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp87)->f2;_tmp8B=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp87)->f3;_LLB9: cn=Cyc_Evexp_eval_const_bool_exp(
_tmp89)?Cyc_Evexp_eval_const_exp(_tmp8A): Cyc_Evexp_eval_const_exp(_tmp8B);goto
_LLB5;_LLBA: if(*((int*)_tmp87)!= 3)goto _LLBC;_tmp8C=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp87)->f1;_tmp8D=((struct Cyc_Absyn_Primop_e_struct*)_tmp87)->f2;_LLBB: if(
_tmp8D == 0){({void*_tmp92[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("bad static expression (no args to primop)",
sizeof(char),42),_tag_arr(_tmp92,sizeof(void*),0));});return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=({struct Cyc_Absyn_Int_c_struct _tmp94;
_tmp94.tag=2;_tmp94.f1=(void*)((void*)0);_tmp94.f2=0;_tmp94;});_tmp93;});}if(
_tmp8D->tl == 0)cn=Cyc_Evexp_eval_const_unprimop(_tmp8C,(struct Cyc_Absyn_Exp*)
_tmp8D->hd);else{if(((struct Cyc_List_List*)_check_null(_tmp8D->tl))->tl != 0){({
void*_tmp95[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("bad static expression (too many args to primop)",
sizeof(char),48),_tag_arr(_tmp95,sizeof(void*),0));});return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Absyn_Int_c_struct _tmp97;
_tmp97.tag=2;_tmp97.f1=(void*)((void*)0);_tmp97.f2=0;_tmp97;});_tmp96;});}else{
cn=Cyc_Evexp_eval_const_binprimop(_tmp8C,(struct Cyc_Absyn_Exp*)_tmp8D->hd,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp8D->tl))->hd);}}goto
_LLB5;_LLBC: if(*((int*)_tmp87)!= 16)goto _LLBE;_LLBD: goto _LLBF;_LLBE: if(*((int*)
_tmp87)!= 17)goto _LLC0;_LLBF:({void*_tmp98[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("sizeof(...) is not an integral constant expression in Cyclone",
sizeof(char),62),_tag_arr(_tmp98,sizeof(void*),0));});return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Absyn_Int_c_struct _tmp9A;
_tmp9A.tag=2;_tmp9A.f1=(void*)((void*)0);_tmp9A.f2=0;_tmp9A;});_tmp99;});_LLC0:
if(*((int*)_tmp87)!= 18)goto _LLC2;_LLC1:({void*_tmp9B[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("offsetof(...) is not an integral constant expression in Cyclone",
sizeof(char),64),_tag_arr(_tmp9B,sizeof(void*),0));});return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({struct Cyc_Absyn_Int_c_struct _tmp9D;
_tmp9D.tag=2;_tmp9D.f1=(void*)((void*)0);_tmp9D.f2=0;_tmp9D;});_tmp9C;});_LLC2:
if(*((int*)_tmp87)!= 13)goto _LLC4;_tmp8E=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp87)->f1;_tmp8F=((struct Cyc_Absyn_Cast_e_struct*)_tmp87)->f2;_LLC3: cn=Cyc_Evexp_eval_const_exp(
_tmp8F);{struct _tuple3 _tmp9F=({struct _tuple3 _tmp9E;_tmp9E.f1=Cyc_Tcutil_compress(
_tmp8E);_tmp9E.f2=cn;_tmp9E;});void*_tmpA0;void*_tmpA1;void*_tmpA2;void*_tmpA3;
void*_tmpA4;char _tmpA5;void*_tmpA6;void*_tmpA7;void*_tmpA8;void*_tmpA9;void*
_tmpAA;short _tmpAB;void*_tmpAC;void*_tmpAD;void*_tmpAE;void*_tmpAF;void*_tmpB0;
int _tmpB1;_LLCB: _tmpA0=_tmp9F.f1;if(_tmpA0 <= (void*)3?1:*((int*)_tmpA0)!= 5)goto
_LLCD;_tmpA1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpA0)->f1;_tmpA2=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpA0)->f2;_tmpA3=_tmp9F.f2;if(_tmpA3 <= (void*)
1?1:*((int*)_tmpA3)!= 0)goto _LLCD;_tmpA4=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmpA3)->f1;_tmpA5=((struct Cyc_Absyn_Char_c_struct*)_tmpA3)->f2;_LLCC: _tmpA7=
_tmpA1;_tmpA8=_tmpA2;_tmpAA=_tmpA4;_tmpAB=(short)_tmpA5;goto _LLCE;_LLCD: _tmpA6=
_tmp9F.f1;if(_tmpA6 <= (void*)3?1:*((int*)_tmpA6)!= 5)goto _LLCF;_tmpA7=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpA6)->f1;_tmpA8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpA6)->f2;_tmpA9=_tmp9F.f2;if(_tmpA9 <= (void*)1?1:*((int*)_tmpA9)!= 1)goto
_LLCF;_tmpAA=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmpA9)->f1;_tmpAB=((
struct Cyc_Absyn_Short_c_struct*)_tmpA9)->f2;_LLCE: _tmpAD=_tmpA7;_tmpAE=_tmpA8;
_tmpB0=_tmpAA;_tmpB1=(int)_tmpAB;goto _LLD0;_LLCF: _tmpAC=_tmp9F.f1;if(_tmpAC <= (
void*)3?1:*((int*)_tmpAC)!= 5)goto _LLD1;_tmpAD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpAC)->f1;_tmpAE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpAC)->f2;_tmpAF=
_tmp9F.f2;if(_tmpAF <= (void*)1?1:*((int*)_tmpAF)!= 2)goto _LLD1;_tmpB0=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmpAF)->f1;_tmpB1=((struct Cyc_Absyn_Int_c_struct*)
_tmpAF)->f2;_LLD0: if(_tmpAD != _tmpB0)cn=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_Int_c_struct _tmpB3;
_tmpB3.tag=2;_tmpB3.f1=(void*)_tmpAD;_tmpB3.f2=_tmpB1;_tmpB3;});_tmpB2;});goto
_LLCA;_LLD1:;_LLD2:({struct Cyc_Std_String_pa_struct _tmpB5;_tmpB5.tag=0;_tmpB5.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp8E);{void*_tmpB4[1]={& _tmpB5};Cyc_Tcutil_terr(
e->loc,_tag_arr("eval_const: cannot cast to %s",sizeof(char),30),_tag_arr(_tmpB4,
sizeof(void*),1));}});return(void*)({struct Cyc_Absyn_Int_c_struct*_tmpB6=
_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_Int_c_struct _tmpB7;_tmpB7.tag=
2;_tmpB7.f1=(void*)((void*)0);_tmpB7.f2=0;_tmpB7;});_tmpB6;});_LLCA:;}goto _LLB5;
_LLC4: if(*((int*)_tmp87)!= 32)goto _LLC6;_tmp90=((struct Cyc_Absyn_AnonEnum_e_struct*)
_tmp87)->f3;_LLC5: _tmp91=_tmp90;goto _LLC7;_LLC6: if(*((int*)_tmp87)!= 31)goto
_LLC8;_tmp91=((struct Cyc_Absyn_Enum_e_struct*)_tmp87)->f3;_LLC7: return Cyc_Evexp_eval_const_exp((
struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)_check_null(_tmp91))->tag));
_LLC8:;_LLC9:({void*_tmpB8[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("bad static expression",
sizeof(char),22),_tag_arr(_tmpB8,sizeof(void*),0));});return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({struct Cyc_Absyn_Int_c_struct _tmpBA;
_tmpBA.tag=2;_tmpBA.f1=(void*)((void*)0);_tmpBA.f2=0;_tmpBA;});_tmpB9;});_LLB5:;}(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*_tmpBB=_cycalloc(
sizeof(*_tmpBB));_tmpBB[0]=({struct Cyc_Absyn_Const_e_struct _tmpBC;_tmpBC.tag=0;
_tmpBC.f1=(void*)cn;_tmpBC;});_tmpBB;})));return cn;}
