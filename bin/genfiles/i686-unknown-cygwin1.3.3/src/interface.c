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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};void*
Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
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
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[
8];struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct _tagged_arr line;int
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
_tagged_arr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(struct
_tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();extern void*Cyc_Absyn_uchar_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern struct _tuple0*
Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;void*Cyc_Absyn_tagged_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
void*info);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_Dict;extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,void*));int Cyc_Dict_member(
struct Cyc_Dict_Dict*d,void*k);struct Cyc_Dict_Dict*Cyc_Dict_insert(struct Cyc_Dict_Dict*
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict*d,void*k);void*Cyc_Dict_fold(
void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict*d,void*accum);void Cyc_Dict_iter_c(
void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict*d);struct Cyc_Dict_Dict*
Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict*d);struct _tuple3{void*f1;void*
f2;};struct _tuple3*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*d);
struct _tuple3*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
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
ae;struct Cyc_Core_Opt*le;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();struct Cyc_Interface_I;
struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*Cyc_Interface_final();
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict*ae);struct
_tuple4{struct _tagged_arr f1;struct _tagged_arr f2;};int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple4*info);struct Cyc_Interface_I*
Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct
_tuple4*info);struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*
li,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*
linfo);void Cyc_Interface_print(struct Cyc_Interface_I*,struct Cyc_Std___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc_Std___cycFILE*);void Cyc_Interface_save(
struct Cyc_Interface_I*,struct Cyc_Std___cycFILE*);struct Cyc_Interface_I*Cyc_Interface_load(
struct Cyc_Std___cycFILE*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc_Std___cycFILE*f);
struct _tagged_arr Cyc_Absynpp_qvar2string(struct _tuple0*);void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*
te,int var_default_init,struct Cyc_List_List*ds);extern char Cyc_Lexing_Error[10];
struct Cyc_Lexing_Error_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{
void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};struct Cyc_List_List*
Cyc_Parse_parse_file(struct Cyc_Std___cycFILE*f);struct Cyc_Declaration_spec;
struct Cyc_Declarator;struct Cyc_Abstractdeclarator;int Cyc_yyparse();extern char Cyc_AbstractDeclarator_tok[
27];struct Cyc_AbstractDeclarator_tok_struct{char*tag;struct Cyc_Abstractdeclarator*
f1;};extern char Cyc_AggrFieldDeclListList_tok[30];struct Cyc_AggrFieldDeclListList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_AggrFieldDeclList_tok[26];struct
Cyc_AggrFieldDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_AttributeList_tok[
22];struct Cyc_AttributeList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_Attribute_tok[18];struct Cyc_Attribute_tok_struct{char*tag;void*f1;};
extern char Cyc_Bool_tok[13];struct Cyc_Bool_tok_struct{char*tag;int f1;};extern char
Cyc_Char_tok[13];struct Cyc_Char_tok_struct{char*tag;char f1;};extern char Cyc_DeclList_tok[
17];struct Cyc_DeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char
Cyc_DeclSpec_tok[17];struct Cyc_DeclSpec_tok_struct{char*tag;struct Cyc_Declaration_spec*
f1;};extern char Cyc_Declarator_tok[19];struct Cyc_Declarator_tok_struct{char*tag;
struct Cyc_Declarator*f1;};extern char Cyc_DesignatorList_tok[23];struct Cyc_DesignatorList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Designator_tok[19];struct Cyc_Designator_tok_struct{
char*tag;void*f1;};extern char Cyc_EnumfieldList_tok[22];struct Cyc_EnumfieldList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Enumfield_tok[18];struct Cyc_Enumfield_tok_struct{
char*tag;struct Cyc_Absyn_Enumfield*f1;};extern char Cyc_ExpList_tok[16];struct Cyc_ExpList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Exp_tok[12];struct Cyc_Exp_tok_struct{
char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_FieldPatternList_tok[25];struct
Cyc_FieldPatternList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_FieldPattern_tok[
21];struct _tuple5{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
char*tag;struct _tuple5*f1;};extern char Cyc_FnDecl_tok[15];struct Cyc_FnDecl_tok_struct{
char*tag;struct Cyc_Absyn_Fndecl*f1;};extern char Cyc_IdList_tok[15];struct Cyc_IdList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_InitDeclList_tok[21];struct Cyc_InitDeclList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_InitDecl_tok[17];struct _tuple6{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
char*tag;struct _tuple6*f1;};extern char Cyc_InitializerList_tok[24];struct Cyc_InitializerList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_Int_tok[12];struct _tuple7{void*
f1;int f2;};struct Cyc_Int_tok_struct{char*tag;struct _tuple7*f1;};extern char Cyc_Kind_tok[
13];struct Cyc_Kind_tok_struct{char*tag;void*f1;};extern char Cyc_Okay_tok[13];
extern char Cyc_ParamDeclListBool_tok[26];struct _tuple8{struct Cyc_List_List*f1;int
f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};
struct Cyc_ParamDeclListBool_tok_struct{char*tag;struct _tuple8*f1;};extern char Cyc_ParamDeclList_tok[
22];struct Cyc_ParamDeclList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern
char Cyc_ParamDecl_tok[18];struct Cyc_ParamDecl_tok_struct{char*tag;struct _tuple1*
f1;};extern char Cyc_PatternList_tok[20];struct Cyc_PatternList_tok_struct{char*tag;
struct Cyc_List_List*f1;};extern char Cyc_Pattern_tok[16];struct Cyc_Pattern_tok_struct{
char*tag;struct Cyc_Absyn_Pat*f1;};extern char Cyc_Pointer_Sort_tok[21];struct Cyc_Pointer_Sort_tok_struct{
char*tag;void*f1;};extern char Cyc_Primop_tok[15];struct Cyc_Primop_tok_struct{char*
tag;void*f1;};extern char Cyc_Primopopt_tok[18];struct Cyc_Primopopt_tok_struct{
char*tag;struct Cyc_Core_Opt*f1;};extern char Cyc_QualId_tok[15];struct Cyc_QualId_tok_struct{
char*tag;struct _tuple0*f1;};extern char Cyc_QualSpecList_tok[21];struct _tuple9{
struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_QualSpecList_tok_struct{char*tag;struct _tuple9*f1;};extern char Cyc_Rgnorder_tok[
17];struct Cyc_Rgnorder_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char
Cyc_Scope_tok[14];struct Cyc_Scope_tok_struct{char*tag;void*f1;};extern char Cyc_Short_tok[
14];struct Cyc_Short_tok_struct{char*tag;short f1;};extern char Cyc_Stmt_tok[13];
struct Cyc_Stmt_tok_struct{char*tag;struct Cyc_Absyn_Stmt*f1;};extern char Cyc_StorageClass_tok[
21];struct Cyc_StorageClass_tok_struct{char*tag;void*f1;};extern char Cyc_String_tok[
15];struct Cyc_String_tok_struct{char*tag;struct _tagged_arr f1;};extern char Cyc_Stringopt_tok[
18];struct Cyc_Stringopt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};extern char
Cyc_StructOrUnion_tok[22];struct Cyc_StructOrUnion_tok_struct{char*tag;void*f1;};
extern char Cyc_SwitchCClauseList_tok[26];struct Cyc_SwitchCClauseList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_SwitchClauseList_tok[25];struct
Cyc_SwitchClauseList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_TunionFieldList_tok[
24];struct Cyc_TunionFieldList_tok_struct{char*tag;struct Cyc_List_List*f1;};
extern char Cyc_TunionField_tok[20];struct Cyc_TunionField_tok_struct{char*tag;
struct Cyc_Absyn_Tunionfield*f1;};extern char Cyc_TypeList_tok[17];struct Cyc_TypeList_tok_struct{
char*tag;struct Cyc_List_List*f1;};extern char Cyc_TypeModifierList_tok[25];struct
Cyc_TypeModifierList_tok_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_TypeOpt_tok[
16];struct Cyc_TypeOpt_tok_struct{char*tag;struct Cyc_Core_Opt*f1;};extern char Cyc_TypeQual_tok[
17];struct Cyc_TypeQual_tok_struct{char*tag;struct Cyc_Absyn_Tqual f1;};extern char
Cyc_TypeSpecifier_tok[22];struct Cyc_TypeSpecifier_tok_struct{char*tag;void*f1;};
extern char Cyc_Type_tok[13];struct Cyc_Type_tok_struct{char*tag;void*f1;};int Cyc_yyparse();
extern char Cyc_YY1[8];struct Cyc_YY1_struct{char*tag;struct _tuple8*f1;};extern char
Cyc_YYINITIALSVAL[18];struct Cyc_Yyltype{int timestamp;int first_line;int
first_column;int last_line;int last_column;};int Cyc_Std_strptrcmp(struct
_tagged_arr*s1,struct _tagged_arr*s2);int Cyc_Std_zstrptrcmp(struct _tagged_arr*,
struct _tagged_arr*);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_tagged_arr fmt,struct _tagged_arr ap);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_function_type(
void*t);extern char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*field;};void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg1,struct _tagged_arr fmt,
struct _tagged_arr ap);struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct
Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*loc,
struct _tagged_arr*msg);struct Cyc_Absyn_Tuniondecl*Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(
struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*
loc,struct _tagged_arr*msg);struct Cyc_Tcdecl_Xtunionfielddecl*Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl*d0,struct Cyc_Tcdecl_Xtunionfielddecl*d1,struct
Cyc_Position_Segment*loc,struct _tagged_arr*msg);struct Cyc_Typerep_Int_struct{int
tag;int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int
f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple10{
unsigned int f1;struct _tagged_arr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int
tag;struct _tagged_arr*f1;unsigned int f2;struct _tagged_arr f3;};struct _tuple11{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _tagged_arr f2;};struct _tuple12{unsigned int f1;struct _tagged_arr f2;};struct
Cyc_Typerep_TUnion_struct{int tag;struct _tagged_arr f1;struct _tagged_arr f2;struct
_tagged_arr f3;};struct Cyc_Typerep_TUnionField_struct{int tag;struct _tagged_arr f1;
struct _tagged_arr f2;unsigned int f3;struct _tagged_arr f4;};struct _tuple13{struct
_tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{int tag;struct
_tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{int tag;struct
_tagged_arr*f1;int f2;struct _tagged_arr f3;};struct Cyc_Typerep_Enum_struct{int tag;
struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);struct _tuple14{struct Cyc_Dict_Dict*f1;int f2;};struct _tuple15{struct
_tagged_arr f1;int f2;};struct _tuple14 Cyc_Marshal_write_type_base(void*rep,struct
_tuple14 env,struct Cyc_Std___cycFILE*fp,void*val);void Cyc_Lex_lex_init();struct
Cyc_Interface_Ienv{struct Cyc_Dict_Dict*aggrdecls;struct Cyc_Dict_Dict*tuniondecls;
struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*typedefdecls;struct Cyc_Dict_Dict*
vardecls;struct Cyc_Dict_Dict*xtunionfielddecls;};struct Cyc_Interface_I{struct Cyc_Dict_Dict*
imports;struct Cyc_Dict_Dict*exports;};static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv(){
return({struct Cyc_Interface_Ienv*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->aggrdecls=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp0->tuniondecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);
_tmp0->enumdecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp0->typedefdecls=((struct
Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp0->vardecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp0->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp0;});}static struct Cyc_Interface_Ienv*Cyc_Interface_lazy_new_ienv(){
static struct Cyc_Interface_Ienv*ie=0;if(ie == 0)ie=(struct Cyc_Interface_Ienv*)Cyc_Interface_new_ienv();
return(struct Cyc_Interface_Ienv*)_check_null(ie);}static struct Cyc_Interface_Ienv*
Cyc_Interface_skel2i_f(void*ignored){return Cyc_Interface_new_ienv();}static
struct Cyc_Interface_I*Cyc_Interface_skel2i(struct Cyc_Dict_Dict*skel){return({
struct Cyc_Interface_I*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->imports=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Interface_Ienv*(*f)(void*),struct Cyc_Dict_Dict*d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,
skel);_tmp1->exports=((struct Cyc_Dict_Dict*(*)(struct Cyc_Interface_Ienv*(*f)(
void*),struct Cyc_Dict_Dict*d))Cyc_Dict_map)(Cyc_Interface_skel2i_f,skel);_tmp1;});}
struct Cyc_Interface_I*Cyc_Interface_empty(){return({struct Cyc_Interface_I*_tmp2=
_cycalloc(sizeof(*_tmp2));_tmp2->imports=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);
_tmp2->exports=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct Cyc_List_List*,struct
Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);_tmp2;});}struct Cyc_Interface_I*
Cyc_Interface_final(){struct Cyc_Interface_I*_tmp3=Cyc_Interface_empty();struct
Cyc_Interface_Ienv*_tmp4=Cyc_Interface_new_ienv();struct Cyc_Absyn_Tuniondecl*
_tmp5=({struct Cyc_Absyn_Tuniondecl*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15[0]=*
Cyc_Absyn_exn_tud;_tmp15;});(void*)(_tmp5->sc=(void*)((void*)2));_tmp4->tuniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl*
v))Cyc_Dict_insert)(_tmp4->tuniondecls,(*Cyc_Absyn_exn_name).f2,_tmp5);{struct
Cyc_List_List*_tmp6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;
for(0;_tmp6 != 0;_tmp6=_tmp6->tl){struct Cyc_Absyn_Tunionfield*_tmp7=({struct Cyc_Absyn_Tunionfield*
_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA[0]=*((struct Cyc_Absyn_Tunionfield*)_tmp6->hd);
_tmpA;});(void*)(_tmp7->sc=(void*)((void*)2));{struct Cyc_Tcdecl_Xtunionfielddecl*
_tmp8=({struct Cyc_Tcdecl_Xtunionfielddecl*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->base=
_tmp5;_tmp9->field=_tmp7;_tmp9;});_tmp4->xtunionfielddecls=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Tcdecl_Xtunionfielddecl*v))
Cyc_Dict_insert)(_tmp4->xtunionfielddecls,(*((struct Cyc_Absyn_Tunionfield*)_tmp6->hd)->name).f2,
_tmp8);}}}_tmp3->exports=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(_tmp3->exports,0,
_tmp4);{struct Cyc_Interface_Ienv*_tmpB=Cyc_Interface_new_ienv();struct _tuple0*
qmain=({struct _tuple0*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->f1=(void*)({
struct Cyc_Absyn_Abs_n_struct*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=({
struct Cyc_Absyn_Abs_n_struct _tmp14;_tmp14.tag=1;_tmp14.f1=0;_tmp14;});_tmp13;});
_tmp11->f2=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"main",sizeof(char),
5);_tmp11;});struct Cyc_Absyn_Vardecl*_tmpC=Cyc_Absyn_new_vardecl(qmain,Cyc_Absyn_function_typ(
0,0,Cyc_Absyn_sint_typ,({struct Cyc_List_List*_tmpD=_cycalloc(sizeof(*_tmpD));
_tmpD->hd=({struct _tuple1*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->f1=0;_tmp10->f2=
Cyc_Absyn_empty_tqual();_tmp10->f3=Cyc_Absyn_uint_typ;_tmp10;});_tmpD->tl=({
struct Cyc_List_List*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->hd=({struct _tuple1*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->f1=0;_tmpF->f2=Cyc_Absyn_empty_tqual();
_tmpF->f3=Cyc_Absyn_tagged_typ(Cyc_Absyn_tagged_typ(Cyc_Absyn_uchar_typ,(void*)2,
Cyc_Absyn_empty_tqual()),(void*)2,Cyc_Absyn_empty_tqual());_tmpF;});_tmpE->tl=0;
_tmpE;});_tmpD;}),0,0,0,0),0);(void*)(_tmpC->sc=(void*)((void*)3));_tmpB->vardecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Vardecl*
v))Cyc_Dict_insert)(_tmpB->vardecls,(*qmain).f2,_tmpC);_tmp3->imports=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*
v))Cyc_Dict_insert)(_tmp3->exports,0,_tmpB);return _tmp3;}}static void Cyc_Interface_err(
struct _tagged_arr msg){({void*_tmp16[0]={};Cyc_Tcutil_terr(0,msg,_tag_arr(_tmp16,
sizeof(void*),0));});}static void*Cyc_Interface_invalid_arg(struct _tagged_arr s){(
int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp17=_cycalloc(
sizeof(*_tmp17));_tmp17[0]=({struct Cyc_Core_Invalid_argument_struct _tmp18;_tmp18.tag=
Cyc_Core_Invalid_argument;_tmp18.f1=s;_tmp18;});_tmp17;}));}static void Cyc_Interface_fields_err(
struct _tagged_arr sc,struct _tagged_arr t,struct _tuple0*n){Cyc_Interface_err((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp1C;_tmp1C.tag=0;_tmp1C.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(n);{struct Cyc_Std_String_pa_struct
_tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _tagged_arr)t;{struct Cyc_Std_String_pa_struct
_tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _tagged_arr)sc;{void*_tmp19[3]={& _tmp1A,&
_tmp1B,& _tmp1C};Cyc_Std_aprintf(_tag_arr("fields of %s %s %s have never been defined",
sizeof(char),43),_tag_arr(_tmp19,sizeof(void*),3));}}}}));}static void Cyc_Interface_body_err(
struct _tagged_arr sc,struct _tuple0*n){Cyc_Interface_err((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp1F;_tmp1F.tag=0;_tmp1F.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(n);{struct Cyc_Std_String_pa_struct _tmp1E;_tmp1E.tag=0;
_tmp1E.f1=(struct _tagged_arr)sc;{void*_tmp1D[2]={& _tmp1E,& _tmp1F};Cyc_Std_aprintf(
_tag_arr("the body of %s function %s has never been defined",sizeof(char),50),
_tag_arr(_tmp1D,sizeof(void*),2));}}}));}static void Cyc_Interface_static_err(
struct _tagged_arr obj1,struct _tuple0*name1,struct _tagged_arr obj2,struct _tuple0*
name2){if(obj1.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr)Cyc_Interface_err((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp24;_tmp24.tag=0;_tmp24.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(name2);{struct Cyc_Std_String_pa_struct
_tmp23;_tmp23.tag=0;_tmp23.f1=(struct _tagged_arr)obj2;{struct Cyc_Std_String_pa_struct
_tmp22;_tmp22.tag=0;_tmp22.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(name1);{
struct Cyc_Std_String_pa_struct _tmp21;_tmp21.tag=0;_tmp21.f1=(struct _tagged_arr)
obj1;{void*_tmp20[4]={& _tmp21,& _tmp22,& _tmp23,& _tmp24};Cyc_Std_aprintf(_tag_arr("declaration of %s %s relies on static %s %s",
sizeof(char),44),_tag_arr(_tmp20,sizeof(void*),4));}}}}}));}static void Cyc_Interface_abstract_err(
struct _tagged_arr obj1,struct _tuple0*name1,struct _tagged_arr obj2,struct _tuple0*
name2){if(obj1.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr)Cyc_Interface_err((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp29;_tmp29.tag=0;_tmp29.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(name2);{struct Cyc_Std_String_pa_struct
_tmp28;_tmp28.tag=0;_tmp28.f1=(struct _tagged_arr)obj2;{struct Cyc_Std_String_pa_struct
_tmp27;_tmp27.tag=0;_tmp27.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(name1);{
struct Cyc_Std_String_pa_struct _tmp26;_tmp26.tag=0;_tmp26.f1=(struct _tagged_arr)
obj1;{void*_tmp25[4]={& _tmp26,& _tmp27,& _tmp28,& _tmp29};Cyc_Std_aprintf(_tag_arr("declaration of %s %s relies on fields of abstract %s %s",
sizeof(char),56),_tag_arr(_tmp25,sizeof(void*),4));}}}}}));}struct Cyc_Interface_Seen{
struct Cyc_Dict_Dict*aggrs;struct Cyc_Dict_Dict*tunions;};static struct Cyc_Interface_Seen*
Cyc_Interface_new_seen(){return({struct Cyc_Interface_Seen*_tmp2A=_cycalloc(
sizeof(*_tmp2A));_tmp2A->aggrs=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple0*,
struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp2A->tunions=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp2A;});}static int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict*ae,
struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct _tuple0*name,void*t);
static int Cyc_Interface_check_public_type_list(struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*
seen,struct _tagged_arr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*
l){int _tmp2B=1;for(0;l != 0;l=l->tl){if(!Cyc_Interface_check_public_type(ae,seen,
obj,name,f((void*)l->hd)))_tmp2B=0;}return _tmp2B;}static int Cyc_Interface_check_public_aggrdecl(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Aggrdecl*d){{
struct _handler_cons _tmp2C;_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))
_tmp2E=1;if(!_tmp2E){{int _tmp2F=((int(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))
Cyc_Dict_lookup)(seen->aggrs,d->name);_npop_handler(0);return _tmp2F;};
_pop_handler();}else{void*_tmp2D=(void*)_exn_thrown;void*_tmp31=_tmp2D;_LL1: if(
_tmp31 != Cyc_Dict_Absent)goto _LL3;_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(_tmp31);
_LL0:;}}}{int _tmp32=1;seen->aggrs=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp32);if(d->impl
!= 0){struct Cyc_List_List*_tmp33=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(d->impl))->fields;
for(0;_tmp33 != 0;_tmp33=_tmp33->tl){if(!Cyc_Interface_check_public_type(ae,seen,
_tag_arr("type",sizeof(char),5),d->name,(void*)((struct Cyc_Absyn_Aggrfield*)
_tmp33->hd)->type))_tmp32=0;}}seen->aggrs=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->aggrs,d->name,_tmp32);return
_tmp32;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_Interface_get_type1(
struct _tuple16*x){return(*x).f2;}static void*Cyc_Interface_get_type2(struct
_tuple1*x){return(*x).f3;}static int Cyc_Interface_check_public_tuniondecl(struct
Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Tuniondecl*d){{
struct _handler_cons _tmp34;_push_handler(& _tmp34);{int _tmp36=0;if(setjmp(_tmp34.handler))
_tmp36=1;if(!_tmp36){{int _tmp37=((int(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))
Cyc_Dict_lookup)(seen->tunions,d->name);_npop_handler(0);return _tmp37;};
_pop_handler();}else{void*_tmp35=(void*)_exn_thrown;void*_tmp39=_tmp35;_LL6: if(
_tmp39 != Cyc_Dict_Absent)goto _LL8;_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp39);
_LL5:;}}}{int _tmp3A=1;seen->tunions=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->tunions,d->name,_tmp3A);if(d->fields
!= 0){struct Cyc_List_List*_tmp3B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(d->fields))->v;for(0;_tmp3B != 0;_tmp3B=_tmp3B->tl){if(!((int(*)(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct
_tuple0*name,void*(*f)(struct _tuple16*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(
ae,seen,_tag_arr("[x]tunion",sizeof(char),10),d->name,Cyc_Interface_get_type1,((
struct Cyc_Absyn_Tunionfield*)_tmp3B->hd)->typs))_tmp3A=0;}}seen->tunions=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)(
seen->tunions,d->name,_tmp3A);return _tmp3A;}}static int Cyc_Interface_check_public_enumdecl(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Enumdecl*d){
return 1;}static int Cyc_Interface_check_public_typedefdecl(struct Cyc_Dict_Dict*ae,
struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*d){if(d->defn != 0)
return Cyc_Interface_check_public_type(ae,seen,(struct _tagged_arr)_tag_arr(0,0,0),
d->name,(void*)((struct Cyc_Core_Opt*)_check_null(d->defn))->v);else{return 1;}}
static int Cyc_Interface_check_public_vardecl(struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*
seen,struct Cyc_Absyn_Vardecl*d){return Cyc_Interface_check_public_type(ae,seen,
_tag_arr("variable",sizeof(char),9),d->name,(void*)d->type);}static struct Cyc_List_List*
Cyc_Interface_get_abs_ns(void*ns){void*_tmp3C=ns;struct Cyc_List_List*_tmp3D;_LLB:
if(_tmp3C <= (void*)1?1:*((int*)_tmp3C)!= 1)goto _LLD;_tmp3D=((struct Cyc_Absyn_Abs_n_struct*)
_tmp3C)->f1;_LLC: return _tmp3D;_LLD:;_LLE:((int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)(
_tag_arr("get_abs_ns",sizeof(char),11));_LLA:;}static int Cyc_Interface_check_public_type(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct
_tuple0*name,void*t){void*_tmp3E=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmp3F;void*_tmp40;void*_tmp41;struct Cyc_Core_Opt*_tmp42;struct Cyc_Core_Opt
_tmp43;void*_tmp44;struct Cyc_Absyn_FnInfo _tmp45;void*_tmp46;struct Cyc_List_List*
_tmp47;struct Cyc_Absyn_VarargInfo*_tmp48;struct Cyc_List_List*_tmp49;struct Cyc_Absyn_AggrInfo
_tmp4A;void*_tmp4B;struct Cyc_List_List*_tmp4C;struct _tuple0*_tmp4D;struct Cyc_Absyn_TunionInfo
_tmp4E;void*_tmp4F;struct Cyc_Absyn_Tuniondecl**_tmp50;struct Cyc_Absyn_Tuniondecl*
_tmp51;struct Cyc_List_List*_tmp52;struct Cyc_Absyn_TunionFieldInfo _tmp53;void*
_tmp54;struct Cyc_Absyn_Tuniondecl*_tmp55;struct Cyc_Absyn_Tunionfield*_tmp56;
struct Cyc_List_List*_tmp57;_LL10: if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 4)goto
_LL12;_tmp3F=((struct Cyc_Absyn_PointerType_struct*)_tmp3E)->f1;_tmp40=(void*)
_tmp3F.elt_typ;_LL11: _tmp41=_tmp40;goto _LL13;_LL12: if(_tmp3E <= (void*)3?1:*((int*)
_tmp3E)!= 7)goto _LL14;_tmp41=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp3E)->f1;
_LL13: _tmp44=_tmp41;goto _LL15;_LL14: if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 16)
goto _LL16;_tmp42=((struct Cyc_Absyn_TypedefType_struct*)_tmp3E)->f4;if(_tmp42 == 0)
goto _LL16;_tmp43=*_tmp42;_tmp44=(void*)_tmp43.v;_LL15: return Cyc_Interface_check_public_type(
ae,seen,obj,name,_tmp44);_LL16: if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 8)goto
_LL18;_tmp45=((struct Cyc_Absyn_FnType_struct*)_tmp3E)->f1;_tmp46=(void*)_tmp45.ret_typ;
_tmp47=_tmp45.args;_tmp48=_tmp45.cyc_varargs;_LL17: {int b=((int(*)(struct Cyc_Dict_Dict*
ae,struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct _tuple0*name,void*(*
f)(struct _tuple1*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(
ae,seen,obj,name,Cyc_Interface_get_type2,_tmp47)?Cyc_Interface_check_public_type(
ae,seen,obj,name,_tmp46): 0;if(_tmp48 != 0){void*_tmp59;struct Cyc_Absyn_VarargInfo
_tmp58=*_tmp48;_tmp59=(void*)_tmp58.type;b=Cyc_Interface_check_public_type(ae,
seen,obj,name,_tmp59);}return b;}_LL18: if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 9)
goto _LL1A;_tmp49=((struct Cyc_Absyn_TupleType_struct*)_tmp3E)->f1;_LL19: return((
int(*)(struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,
struct _tuple0*name,void*(*f)(struct _tuple16*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(
ae,seen,obj,name,Cyc_Interface_get_type1,_tmp49);_LL1A: if(_tmp3E <= (void*)3?1:*((
int*)_tmp3E)!= 10)goto _LL1C;_tmp4A=((struct Cyc_Absyn_AggrType_struct*)_tmp3E)->f1;
_tmp4B=(void*)_tmp4A.aggr_info;_tmp4C=_tmp4A.targs;_LL1B: {struct Cyc_Absyn_Aggrdecl*
_tmp5A=Cyc_Absyn_get_known_aggrdecl(_tmp4B);if((void*)_tmp5A->sc == (void*)0){Cyc_Interface_static_err(
obj,name,_tag_arr("type",sizeof(char),5),_tmp5A->name);return 0;}return Cyc_Interface_check_public_type_list(
ae,seen,obj,name,Cyc_Core_identity,_tmp4C)?Cyc_Interface_check_public_aggrdecl(
ae,seen,_tmp5A): 0;}_LL1C: if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 12)goto _LL1E;
_tmp4D=((struct Cyc_Absyn_EnumType_struct*)_tmp3E)->f1;_LL1D: {struct _tuple0
_tmp5C;void*_tmp5D;struct _tagged_arr*_tmp5E;struct _tuple0*_tmp5B=_tmp4D;_tmp5C=*
_tmp5B;_tmp5D=_tmp5C.f1;_tmp5E=_tmp5C.f2;{struct Cyc_List_List*_tmp5F=Cyc_Interface_get_abs_ns(
_tmp5D);struct Cyc_Absyn_Enumdecl*ed;{struct _handler_cons _tmp60;_push_handler(&
_tmp60);{int _tmp62=0;if(setjmp(_tmp60.handler))_tmp62=1;if(!_tmp62){{struct Cyc_Tcenv_Genv*
_tmp63=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(ae,_tmp5F);ed=*((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp63->enumdecls,_tmp5E);};_pop_handler();}
else{void*_tmp61=(void*)_exn_thrown;void*_tmp65=_tmp61;_LL25: if(_tmp65 != Cyc_Dict_Absent)
goto _LL27;_LL26:((int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)((struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp67;_tmp67.tag=0;_tmp67.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp4D);{void*_tmp66[1]={& _tmp67};Cyc_Std_aprintf(
_tag_arr("check_public_type (can't find enum %s)",sizeof(char),39),_tag_arr(
_tmp66,sizeof(void*),1));}}));_LL27:;_LL28:(void)_throw(_tmp65);_LL24:;}}}if((
void*)ed->sc == (void*)0){Cyc_Interface_static_err(obj,_tmp4D,_tag_arr("enum",
sizeof(char),5),ed->name);return 0;}return 1;}}_LL1E: if(_tmp3E <= (void*)3?1:*((int*)
_tmp3E)!= 2)goto _LL20;_tmp4E=((struct Cyc_Absyn_TunionType_struct*)_tmp3E)->f1;
_tmp4F=(void*)_tmp4E.tunion_info;if(*((int*)_tmp4F)!= 1)goto _LL20;_tmp50=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp4F)->f1;_tmp51=*_tmp50;_tmp52=_tmp4E.targs;
_LL1F: {struct _tuple0 _tmp69;void*_tmp6A;struct _tagged_arr*_tmp6B;struct _tuple0*
_tmp68=_tmp51->name;_tmp69=*_tmp68;_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;{struct Cyc_List_List*
_tmp6C=Cyc_Interface_get_abs_ns(_tmp6A);struct Cyc_Absyn_Tuniondecl*tud;{struct
_handler_cons _tmp6D;_push_handler(& _tmp6D);{int _tmp6F=0;if(setjmp(_tmp6D.handler))
_tmp6F=1;if(!_tmp6F){{struct Cyc_Tcenv_Genv*_tmp70=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp6C);tud=*((
struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp70->tuniondecls,_tmp6B);};_pop_handler();}else{void*_tmp6E=(void*)
_exn_thrown;void*_tmp72=_tmp6E;_LL2A: if(_tmp72 != Cyc_Dict_Absent)goto _LL2C;_LL2B:((
int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp74;_tmp74.tag=0;_tmp74.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmp51->name);{void*_tmp73[1]={& _tmp74};Cyc_Std_aprintf(
_tag_arr("check_public_type (can't find [x]tunion %s)",sizeof(char),44),_tag_arr(
_tmp73,sizeof(void*),1));}}));_LL2C:;_LL2D:(void)_throw(_tmp72);_LL29:;}}}if((
void*)tud->sc == (void*)0){Cyc_Interface_static_err(obj,name,_tag_arr("[x]tunion",
sizeof(char),10),tud->name);return 0;}return Cyc_Interface_check_public_type_list(
ae,seen,obj,name,Cyc_Core_identity,_tmp52)?Cyc_Interface_check_public_tuniondecl(
ae,seen,tud): 0;}}_LL20: if(_tmp3E <= (void*)3?1:*((int*)_tmp3E)!= 3)goto _LL22;
_tmp53=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3E)->f1;_tmp54=(void*)
_tmp53.field_info;if(*((int*)_tmp54)!= 1)goto _LL22;_tmp55=((struct Cyc_Absyn_KnownTunionfield_struct*)
_tmp54)->f1;_tmp56=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp54)->f2;_tmp57=
_tmp53.targs;_LL21: {struct _tuple0 _tmp76;void*_tmp77;struct _tagged_arr*_tmp78;
struct _tuple0*_tmp75=_tmp55->name;_tmp76=*_tmp75;_tmp77=_tmp76.f1;_tmp78=_tmp76.f2;{
struct Cyc_List_List*_tmp79=Cyc_Interface_get_abs_ns(_tmp77);struct Cyc_Absyn_Tuniondecl*
tud;{struct _handler_cons _tmp7A;_push_handler(& _tmp7A);{int _tmp7C=0;if(setjmp(
_tmp7A.handler))_tmp7C=1;if(!_tmp7C){{struct Cyc_Tcenv_Genv*_tmp7D=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp79);tud=*((
struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp7D->tuniondecls,_tmp78);};_pop_handler();}else{void*_tmp7B=(void*)
_exn_thrown;void*_tmp7F=_tmp7B;_LL2F: if(_tmp7F != Cyc_Dict_Absent)goto _LL31;_LL30:((
int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp81;_tmp81.tag=0;_tmp81.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmp55->name);{void*_tmp80[1]={& _tmp81};Cyc_Std_aprintf(
_tag_arr("check_public_type (can't find [x]tunion %s and search its fields)",
sizeof(char),66),_tag_arr(_tmp80,sizeof(void*),1));}}));_LL31:;_LL32:(void)
_throw(_tmp7F);_LL2E:;}}}if(tud->fields == 0)((int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp83;_tmp83.tag=0;_tmp83.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(tud->name);{void*_tmp82[1]={& _tmp83};
Cyc_Std_aprintf(_tag_arr("check_public_type ([x]tunion %s has no fields)",
sizeof(char),47),_tag_arr(_tmp82,sizeof(void*),1));}}));{struct Cyc_Absyn_Tunionfield*
tuf1=0;{struct Cyc_List_List*_tmp84=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp84 != 0;_tmp84=_tmp84->tl){if(Cyc_Std_strptrcmp((*
_tmp56->name).f2,(*((struct Cyc_Absyn_Tunionfield*)_tmp84->hd)->name).f2)== 0){
tuf1=(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)_tmp84->hd);
break;}}}if(tuf1 == 0)((int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp86;_tmp86.tag=0;_tmp86.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp56->name);{void*_tmp85[1]={& _tmp86};
Cyc_Std_aprintf(_tag_arr("check_public_type (can't find [x]tunionfield %s)",
sizeof(char),49),_tag_arr(_tmp85,sizeof(void*),1));}}));{struct Cyc_Absyn_Tunionfield*
tuf=(struct Cyc_Absyn_Tunionfield*)_check_null(tuf1);if((void*)tud->sc == (void*)0){
Cyc_Interface_static_err(obj,name,_tag_arr("[x]tunion",sizeof(char),10),tud->name);
return 0;}if((void*)tud->sc == (void*)1){Cyc_Interface_abstract_err(obj,name,
_tag_arr("[x]tunion",sizeof(char),10),tud->name);return 0;}if((void*)tuf->sc == (
void*)0){Cyc_Interface_static_err(obj,name,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp88;_tmp88.tag=0;_tmp88.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tuf->name);{
void*_tmp87[1]={& _tmp88};Cyc_Std_aprintf(_tag_arr("field %s of",sizeof(char),12),
_tag_arr(_tmp87,sizeof(void*),1));}}),tud->name);return 0;}return Cyc_Interface_check_public_type_list(
ae,seen,obj,name,Cyc_Core_identity,_tmp57)?Cyc_Interface_check_public_tuniondecl(
ae,seen,tud): 0;}}}}_LL22:;_LL23: return 1;_LLF:;}struct _tuple17{struct Cyc_Interface_Ienv*
f1;struct Cyc_Interface_Ienv*f2;int f3;struct Cyc_Dict_Dict*f4;struct Cyc_Interface_Seen*
f5;struct Cyc_Interface_I*f6;};static void Cyc_Interface_extract_aggrdecl(struct
_tuple17*env,struct _tagged_arr*x,struct Cyc_Absyn_Aggrdecl**dp){struct _tuple17
_tmp8A;struct Cyc_Interface_Ienv*_tmp8B;struct Cyc_Interface_Ienv*_tmp8C;int _tmp8D;
struct Cyc_Dict_Dict*_tmp8E;struct Cyc_Interface_Seen*_tmp8F;struct _tuple17*_tmp89=
env;_tmp8A=*_tmp89;_tmp8B=_tmp8A.f1;_tmp8C=_tmp8A.f2;_tmp8D=_tmp8A.f3;_tmp8E=
_tmp8A.f4;_tmp8F=_tmp8A.f5;{struct Cyc_Absyn_Aggrdecl*_tmp90=*dp;void*_tmp91=(
void*)_tmp90->sc;_LL34: if((int)_tmp91 != 0)goto _LL36;_LL35: if(_tmp8D?_tmp90->impl
== 0: 0)Cyc_Interface_fields_err(_tag_arr("static",sizeof(char),7),_tag_arr("struct/union",
sizeof(char),13),_tmp90->name);goto _LL33;_LL36: if((int)_tmp91 != 1)goto _LL38;
_LL37: if(_tmp90->impl == 0){if(_tmp8D)Cyc_Interface_fields_err(_tag_arr("abstract",
sizeof(char),9),_tag_arr("struct/union",sizeof(char),13),_tmp90->name);}else{
_tmp90=({struct Cyc_Absyn_Aggrdecl*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92[0]=*
_tmp90;_tmp92;});_tmp90->impl=0;}if(Cyc_Interface_check_public_aggrdecl(_tmp8E,
_tmp8F,_tmp90))_tmp8C->aggrdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp8C->aggrdecls,
x,_tmp90);goto _LL33;_LL38: if((int)_tmp91 != 2)goto _LL3A;_LL39: if(_tmp90->impl == 0){
Cyc_Interface_fields_err(_tag_arr("public",sizeof(char),7),_tag_arr("struct/union",
sizeof(char),13),_tmp90->name);_tmp90=({struct Cyc_Absyn_Aggrdecl*_tmp93=
_cycalloc(sizeof(*_tmp93));_tmp93[0]=*_tmp90;_tmp93;});(void*)(_tmp90->sc=(void*)((
void*)1));}if(Cyc_Interface_check_public_aggrdecl(_tmp8E,_tmp8F,_tmp90))_tmp8C->aggrdecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*
v))Cyc_Dict_insert)(_tmp8C->aggrdecls,x,_tmp90);goto _LL33;_LL3A: if((int)_tmp91 != 
4)goto _LL3C;_LL3B: goto _LL3D;_LL3C: if((int)_tmp91 != 3)goto _LL33;_LL3D: if(Cyc_Interface_check_public_aggrdecl(
_tmp8E,_tmp8F,_tmp90))_tmp8B->aggrdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(_tmp8B->aggrdecls,
x,_tmp90);goto _LL33;_LL33:;}}static void Cyc_Interface_extract_enumdecl(struct
_tuple17*env,struct _tagged_arr*x,struct Cyc_Absyn_Enumdecl**dp){struct _tuple17
_tmp95;struct Cyc_Interface_Ienv*_tmp96;struct Cyc_Interface_Ienv*_tmp97;int _tmp98;
struct Cyc_Dict_Dict*_tmp99;struct Cyc_Interface_Seen*_tmp9A;struct _tuple17*_tmp94=
env;_tmp95=*_tmp94;_tmp96=_tmp95.f1;_tmp97=_tmp95.f2;_tmp98=_tmp95.f3;_tmp99=
_tmp95.f4;_tmp9A=_tmp95.f5;{struct Cyc_Absyn_Enumdecl*_tmp9B=*dp;void*_tmp9C=(
void*)_tmp9B->sc;_LL3F: if((int)_tmp9C != 0)goto _LL41;_LL40: if(_tmp98?_tmp9B->fields
== 0: 0)Cyc_Interface_fields_err(_tag_arr("static",sizeof(char),7),_tag_arr("enum",
sizeof(char),5),_tmp9B->name);goto _LL3E;_LL41: if((int)_tmp9C != 1)goto _LL43;_LL42:
if(_tmp9B->fields == 0){if(_tmp98)Cyc_Interface_fields_err(_tag_arr("abstract",
sizeof(char),9),_tag_arr("enum",sizeof(char),5),_tmp9B->name);}else{_tmp9B=({
struct Cyc_Absyn_Enumdecl*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D[0]=*_tmp9B;
_tmp9D;});_tmp9B->fields=0;}if(Cyc_Interface_check_public_enumdecl(_tmp99,_tmp9A,
_tmp9B))_tmp97->enumdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp97->enumdecls,x,
_tmp9B);goto _LL3E;_LL43: if((int)_tmp9C != 2)goto _LL45;_LL44: if(_tmp9B->fields == 0){
Cyc_Interface_fields_err(_tag_arr("public",sizeof(char),7),_tag_arr("enum",
sizeof(char),5),_tmp9B->name);_tmp9B=({struct Cyc_Absyn_Enumdecl*_tmp9E=_cycalloc(
sizeof(*_tmp9E));_tmp9E[0]=*_tmp9B;_tmp9E;});(void*)(_tmp9B->sc=(void*)((void*)1));}
if(Cyc_Interface_check_public_enumdecl(_tmp99,_tmp9A,_tmp9B))_tmp97->enumdecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl*
v))Cyc_Dict_insert)(_tmp97->enumdecls,x,_tmp9B);goto _LL3E;_LL45: if((int)_tmp9C != 
4)goto _LL47;_LL46: goto _LL48;_LL47: if((int)_tmp9C != 3)goto _LL3E;_LL48: if(Cyc_Interface_check_public_enumdecl(
_tmp99,_tmp9A,_tmp9B))_tmp96->enumdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmp96->enumdecls,
x,_tmp9B);goto _LL3E;_LL3E:;}}static void Cyc_Interface_extract_xtunionfielddecl(
struct Cyc_Interface_I*i,struct Cyc_Absyn_Tuniondecl*d,struct Cyc_Absyn_Tunionfield*
f){struct Cyc_List_List*_tmp9F=0;{void*_tmpA0=(*f->name).f1;struct Cyc_List_List*
_tmpA1;struct Cyc_List_List*_tmpA2;_LL4A: if(_tmpA0 <= (void*)1?1:*((int*)_tmpA0)!= 
0)goto _LL4C;_tmpA1=((struct Cyc_Absyn_Rel_n_struct*)_tmpA0)->f1;_LL4B: _tmpA2=
_tmpA1;goto _LL4D;_LL4C: if(_tmpA0 <= (void*)1?1:*((int*)_tmpA0)!= 1)goto _LL4E;
_tmpA2=((struct Cyc_Absyn_Abs_n_struct*)_tmpA0)->f1;_LL4D: _tmp9F=_tmpA2;goto _LL49;
_LL4E:;_LL4F: goto _LL49;_LL49:;}{struct Cyc_Dict_Dict**dict;{void*_tmpA3=(void*)f->sc;
_LL51: if((int)_tmpA3 != 0)goto _LL53;_LL52: return;_LL53: if((int)_tmpA3 != 3)goto
_LL55;_LL54: dict=& i->imports;goto _LL50;_LL55: if((int)_tmpA3 != 2)goto _LL57;_LL56:
dict=& i->exports;goto _LL50;_LL57:;_LL58:((int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)(
_tag_arr("add_xtunionfielddecl",sizeof(char),21));_LL50:;}{struct Cyc_Interface_Ienv*
env;{struct _handler_cons _tmpA4;_push_handler(& _tmpA4);{int _tmpA6=0;if(setjmp(
_tmpA4.handler))_tmpA6=1;if(!_tmpA6){env=((struct Cyc_Interface_Ienv*(*)(struct
Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(*dict,_tmp9F);;
_pop_handler();}else{void*_tmpA5=(void*)_exn_thrown;void*_tmpA8=_tmpA5;_LL5A: if(
_tmpA8 != Cyc_Dict_Absent)goto _LL5C;_LL5B: env=Cyc_Interface_new_ienv();*dict=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*
v))Cyc_Dict_insert)(*dict,_tmp9F,env);goto _LL59;_LL5C:;_LL5D:(void)_throw(_tmpA8);
_LL59:;}}}env->xtunionfielddecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct Cyc_Tcdecl_Xtunionfielddecl*v))Cyc_Dict_insert)(env->xtunionfielddecls,(*
f->name).f2,({struct Cyc_Tcdecl_Xtunionfielddecl*_tmpA9=_cycalloc(sizeof(*_tmpA9));
_tmpA9->base=d;_tmpA9->field=f;_tmpA9;}));}}}static void Cyc_Interface_extract_tuniondecl(
struct _tuple17*env,struct _tagged_arr*x,struct Cyc_Absyn_Tuniondecl**dp){struct
_tuple17 _tmpAB;struct Cyc_Interface_Ienv*_tmpAC;struct Cyc_Interface_Ienv*_tmpAD;
int _tmpAE;struct Cyc_Dict_Dict*_tmpAF;struct Cyc_Interface_Seen*_tmpB0;struct Cyc_Interface_I*
_tmpB1;struct _tuple17*_tmpAA=env;_tmpAB=*_tmpAA;_tmpAC=_tmpAB.f1;_tmpAD=_tmpAB.f2;
_tmpAE=_tmpAB.f3;_tmpAF=_tmpAB.f4;_tmpB0=_tmpAB.f5;_tmpB1=_tmpAB.f6;{struct Cyc_Absyn_Tuniondecl*
_tmpB2=*dp;void*_tmpB3=(void*)_tmpB2->sc;_LL5F: if((int)_tmpB3 != 0)goto _LL61;
_LL60: if((!_tmpB2->is_xtunion?_tmpAE: 0)?_tmpB2->fields == 0: 0)Cyc_Interface_fields_err(
_tag_arr("static",sizeof(char),7),_tag_arr("tunion",sizeof(char),7),_tmpB2->name);
goto _LL5E;_LL61: if((int)_tmpB3 != 1)goto _LL63;_LL62: if(_tmpB2->fields == 0){if(!
_tmpB2->is_xtunion?_tmpAE: 0)Cyc_Interface_fields_err(_tag_arr("abstract",sizeof(
char),9),_tag_arr("tunion",sizeof(char),7),_tmpB2->name);}else{_tmpB2=({struct
Cyc_Absyn_Tuniondecl*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=*_tmpB2;_tmpB4;});
_tmpB2->fields=0;}if(Cyc_Interface_check_public_tuniondecl(_tmpAF,_tmpB0,_tmpB2))
_tmpAD->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct Cyc_Absyn_Tuniondecl*v))Cyc_Dict_insert)(_tmpAD->tuniondecls,
x,_tmpB2);goto _LL5E;_LL63: if((int)_tmpB3 != 2)goto _LL65;_LL64: _tmpB2=({struct Cyc_Absyn_Tuniondecl*
_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=*_tmpB2;_tmpB5;});if(!_tmpB2->is_xtunion?
_tmpB2->fields == 0: 0){Cyc_Interface_fields_err(_tag_arr("public",sizeof(char),7),
_tag_arr("tunion",sizeof(char),7),_tmpB2->name);(void*)(_tmpB2->sc=(void*)((void*)
1));}if(Cyc_Interface_check_public_tuniondecl(_tmpAF,_tmpB0,_tmpB2)){if(_tmpB2->is_xtunion?
_tmpB2->fields != 0: 0){struct Cyc_List_List*_tmpB6=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpB2->fields))->v;_tmpB2->fields=0;{struct Cyc_List_List*
_tmpB7=_tmpB6;for(0;_tmpB7 != 0;_tmpB7=_tmpB7->tl){Cyc_Interface_extract_xtunionfielddecl(
_tmpB1,_tmpB2,(struct Cyc_Absyn_Tunionfield*)_tmpB7->hd);}}}_tmpAD->tuniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl*
v))Cyc_Dict_insert)(_tmpAD->tuniondecls,x,_tmpB2);}goto _LL5E;_LL65: if((int)
_tmpB3 != 4)goto _LL67;_LL66:((int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)(
_tag_arr("extract_tuniondecl",sizeof(char),19));_LL67: if((int)_tmpB3 != 3)goto
_LL5E;_LL68: if(Cyc_Interface_check_public_tuniondecl(_tmpAF,_tmpB0,_tmpB2)){if(
_tmpB2->is_xtunion?_tmpB2->fields != 0: 0){_tmpB2=({struct Cyc_Absyn_Tuniondecl*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=*_tmpB2;_tmpB8;});{struct Cyc_List_List*
_tmpB9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpB2->fields))->v;
_tmpB2->fields=0;{struct Cyc_List_List*_tmpBA=_tmpB9;for(0;_tmpBA != 0;_tmpBA=
_tmpBA->tl){Cyc_Interface_extract_xtunionfielddecl(_tmpB1,_tmpB2,(struct Cyc_Absyn_Tunionfield*)
_tmpBA->hd);}}}}_tmpAC->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl*v))Cyc_Dict_insert)(_tmpAC->tuniondecls,
x,_tmpB2);}goto _LL5E;_LL5E:;}}static void Cyc_Interface_extract_typedef(struct
_tuple17*env,struct _tagged_arr*x,struct Cyc_Absyn_Typedefdecl*d){struct _tuple17
_tmpBC;struct Cyc_Interface_Ienv*_tmpBD;struct Cyc_Dict_Dict*_tmpBE;struct Cyc_Interface_Seen*
_tmpBF;struct _tuple17*_tmpBB=env;_tmpBC=*_tmpBB;_tmpBD=_tmpBC.f2;_tmpBE=_tmpBC.f4;
_tmpBF=_tmpBC.f5;if(Cyc_Interface_check_public_typedefdecl(_tmpBE,_tmpBF,d))
_tmpBD->typedefdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmpBD->typedefdecls,
x,d);}static void Cyc_Interface_extract_ordinarie(struct _tuple17*env,struct
_tagged_arr*x,struct _tuple7*v){struct _tuple17 _tmpC1;struct Cyc_Interface_Ienv*
_tmpC2;struct Cyc_Interface_Ienv*_tmpC3;int _tmpC4;struct Cyc_Dict_Dict*_tmpC5;
struct Cyc_Interface_Seen*_tmpC6;struct _tuple17*_tmpC0=env;_tmpC1=*_tmpC0;_tmpC2=
_tmpC1.f1;_tmpC3=_tmpC1.f2;_tmpC4=_tmpC1.f3;_tmpC5=_tmpC1.f4;_tmpC6=_tmpC1.f5;{
void*_tmpC7=(*v).f1;void*_tmpC8=_tmpC7;void*_tmpC9;_LL6A: if(*((int*)_tmpC8)!= 0)
goto _LL6C;_tmpC9=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmpC8)->f1;_LL6B:{void*
_tmpCA=_tmpC9;struct Cyc_Absyn_Fndecl*_tmpCB;struct Cyc_Absyn_Vardecl*_tmpCC;_LL6F:
if(_tmpCA <= (void*)1?1:*((int*)_tmpCA)!= 1)goto _LL71;_tmpCB=((struct Cyc_Absyn_Funname_b_struct*)
_tmpCA)->f1;_LL70: {struct Cyc_Absyn_Vardecl*_tmpCD=({struct Cyc_Absyn_Vardecl*
_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->sc=(void*)((void*)_tmpCB->sc);_tmpCE->name=
_tmpCB->name;_tmpCE->tq=({struct Cyc_Absyn_Tqual _tmpCF;_tmpCF.q_const=0;_tmpCF.q_volatile=
0;_tmpCF.q_restrict=0;_tmpCF;});_tmpCE->type=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpCB->cached_typ))->v);_tmpCE->initializer=0;_tmpCE->rgn=0;_tmpCE->attributes=
0;_tmpCE->escapes=0;_tmpCE;});_tmpC4=0;_tmpCC=_tmpCD;goto _LL72;}_LL71: if(_tmpCA
<= (void*)1?1:*((int*)_tmpCA)!= 0)goto _LL73;_tmpCC=((struct Cyc_Absyn_Global_b_struct*)
_tmpCA)->f1;_LL72: if(_tmpCC->initializer != 0){_tmpCC=({struct Cyc_Absyn_Vardecl*
_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=*_tmpCC;_tmpD0;});_tmpCC->initializer=
0;}{void*_tmpD1=(void*)_tmpCC->sc;_LL76: if((int)_tmpD1 != 0)goto _LL78;_LL77: if(
_tmpC4?Cyc_Tcutil_is_function_type((void*)_tmpCC->type): 0)Cyc_Interface_body_err(
_tag_arr("static",sizeof(char),7),_tmpCC->name);goto _LL75;_LL78: if((int)_tmpD1 != 
1)goto _LL7A;_LL79:((int(*)(struct _tagged_arr s))Cyc_Interface_invalid_arg)(
_tag_arr("extract_ordinarie",sizeof(char),18));_LL7A: if((int)_tmpD1 != 2)goto
_LL7C;_LL7B: if(_tmpC4?Cyc_Tcutil_is_function_type((void*)_tmpCC->type): 0)Cyc_Interface_body_err(
_tag_arr("public",sizeof(char),7),_tmpCC->name);if(Cyc_Interface_check_public_vardecl(
_tmpC5,_tmpC6,_tmpCC))_tmpC3->vardecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpC3->vardecls,
x,_tmpCC);goto _LL75;_LL7C: if((int)_tmpD1 != 4)goto _LL7E;_LL7D: goto _LL7F;_LL7E: if((
int)_tmpD1 != 3)goto _LL75;_LL7F: if(Cyc_Interface_check_public_vardecl(_tmpC5,
_tmpC6,_tmpCC))_tmpC2->vardecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpC2->vardecls,
x,_tmpCC);goto _LL75;_LL75:;}goto _LL6E;_LL73:;_LL74: goto _LL6E;_LL6E:;}goto _LL69;
_LL6C:;_LL6D: goto _LL69;_LL69:;}}struct _tuple18{struct Cyc_Interface_I*f1;int f2;
struct Cyc_Dict_Dict*f3;struct Cyc_Interface_Seen*f4;};static void Cyc_Interface_extract_f(
struct _tuple18*env_f,struct Cyc_List_List*ns,struct Cyc_Tcenv_Genv*ge){struct
_tuple18 _tmpD3;struct Cyc_Interface_I*_tmpD4;int _tmpD5;struct Cyc_Dict_Dict*_tmpD6;
struct Cyc_Interface_Seen*_tmpD7;struct _tuple18*_tmpD2=env_f;_tmpD3=*_tmpD2;
_tmpD4=_tmpD3.f1;_tmpD5=_tmpD3.f2;_tmpD6=_tmpD3.f3;_tmpD7=_tmpD3.f4;{struct
_tuple17 _tmpD8=({struct _tuple17 _tmpD9;_tmpD9.f1=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmpD4->imports,ns);
_tmpD9.f2=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmpD4->exports,ns);_tmpD9.f3=_tmpD5;_tmpD9.f4=_tmpD6;_tmpD9.f5=
_tmpD7;_tmpD9.f6=_tmpD4;_tmpD9;});((void(*)(void(*f)(struct _tuple17*,struct
_tagged_arr*,struct Cyc_Absyn_Aggrdecl**),struct _tuple17*env,struct Cyc_Dict_Dict*
d))Cyc_Dict_iter_c)(Cyc_Interface_extract_aggrdecl,& _tmpD8,ge->aggrdecls);((void(*)(
void(*f)(struct _tuple17*,struct _tagged_arr*,struct Cyc_Absyn_Tuniondecl**),struct
_tuple17*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_extract_tuniondecl,&
_tmpD8,ge->tuniondecls);((void(*)(void(*f)(struct _tuple17*,struct _tagged_arr*,
struct Cyc_Absyn_Enumdecl**),struct _tuple17*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_extract_enumdecl,& _tmpD8,ge->enumdecls);((void(*)(void(*f)(struct
_tuple17*,struct _tagged_arr*,struct Cyc_Absyn_Typedefdecl*),struct _tuple17*env,
struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_extract_typedef,& _tmpD8,ge->typedefs);((
void(*)(void(*f)(struct _tuple17*,struct _tagged_arr*,struct _tuple7*),struct
_tuple17*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,&
_tmpD8,ge->ordinaries);}}static struct Cyc_Interface_I*Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict*ae,int check_complete_defs){struct _tuple18 _tmpDA=({struct
_tuple18 _tmpDB;_tmpDB.f1=((struct Cyc_Interface_I*(*)(struct Cyc_Dict_Dict*skel))
Cyc_Interface_skel2i)(ae);_tmpDB.f2=check_complete_defs;_tmpDB.f3=ae;_tmpDB.f4=
Cyc_Interface_new_seen();_tmpDB;});((void(*)(void(*f)(struct _tuple18*,struct Cyc_List_List*,
struct Cyc_Tcenv_Genv*),struct _tuple18*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_extract_f,& _tmpDA,ae);return _tmpDA.f1;}struct Cyc_Interface_I*Cyc_Interface_extract(
struct Cyc_Dict_Dict*ae){return Cyc_Interface_gen_extract(ae,1);}inline static void
Cyc_Interface_check_err(struct _tagged_arr*msg1,struct _tagged_arr msg2){({struct
Cyc_Std_String_pa_struct _tmpDD;_tmpDD.tag=0;_tmpDD.f1=(struct _tagged_arr)msg2;{
void*_tmpDC[1]={& _tmpDD};Cyc_Tcdecl_merr(0,msg1,_tag_arr("%s",sizeof(char),3),
_tag_arr(_tmpDC,sizeof(void*),1));}});}struct _tuple19{int f1;struct Cyc_Dict_Dict*
f2;int(*f3)(void*,void*,struct _tagged_arr*);struct _tagged_arr f4;struct
_tagged_arr*f5;};static void Cyc_Interface_incl_dict_f(struct _tuple19*env,struct
_tagged_arr*x,void*y1){struct _tuple19 _tmpDF;int _tmpE0;int*_tmpE1;struct Cyc_Dict_Dict*
_tmpE2;int(*_tmpE3)(void*,void*,struct _tagged_arr*);struct _tagged_arr _tmpE4;
struct _tagged_arr*_tmpE5;struct _tuple19*_tmpDE=env;_tmpDF=*_tmpDE;_tmpE0=_tmpDF.f1;
_tmpE1=(int*)&(*_tmpDE).f1;_tmpE2=_tmpDF.f2;_tmpE3=_tmpDF.f3;_tmpE4=_tmpDF.f4;
_tmpE5=_tmpDF.f5;{struct _handler_cons _tmpE6;_push_handler(& _tmpE6);{int _tmpE8=0;
if(setjmp(_tmpE6.handler))_tmpE8=1;if(!_tmpE8){{void*_tmpE9=((void*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmpE2,x);if(!_tmpE3(y1,_tmpE9,_tmpE5))*
_tmpE1=0;};_pop_handler();}else{void*_tmpE7=(void*)_exn_thrown;void*_tmpEB=
_tmpE7;_LL81: if(_tmpEB != Cyc_Dict_Absent)goto _LL83;_LL82: Cyc_Interface_check_err(
_tmpE5,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpEE;_tmpEE.tag=0;
_tmpEE.f1=(struct _tagged_arr)*x;{struct Cyc_Std_String_pa_struct _tmpED;_tmpED.tag=
0;_tmpED.f1=(struct _tagged_arr)_tmpE4;{void*_tmpEC[2]={& _tmpED,& _tmpEE};Cyc_Std_aprintf(
_tag_arr("%s %s is missing",sizeof(char),17),_tag_arr(_tmpEC,sizeof(void*),2));}}}));*
_tmpE1=0;goto _LL80;_LL83:;_LL84:(void)_throw(_tmpEB);_LL80:;}}}}static int Cyc_Interface_incl_dict(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(void*,void*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg){struct _tuple19
_tmpEF=({struct _tuple19 _tmpF0;_tmpF0.f1=1;_tmpF0.f2=dic2;_tmpF0.f3=incl_f;_tmpF0.f4=
t;_tmpF0.f5=msg;_tmpF0;});((void(*)(void(*f)(struct _tuple19*,struct _tagged_arr*,
void*),struct _tuple19*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_incl_dict_f,&
_tmpEF,dic1);return _tmpEF.f1;}static int Cyc_Interface_incl_aggrdecl(struct Cyc_Absyn_Aggrdecl*
d0,struct Cyc_Absyn_Aggrdecl*d1,struct _tagged_arr*msg){struct Cyc_Absyn_Aggrdecl*
_tmpF1=Cyc_Tcdecl_merge_aggrdecl(d0,d1,0,msg);if(_tmpF1 == 0)return 0;if((struct
Cyc_Absyn_Aggrdecl*)d0 != _tmpF1){Cyc_Interface_check_err(msg,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmpF3;_tmpF3.tag=0;_tmpF3.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(d1->name);{void*_tmpF2[1]={& _tmpF3};Cyc_Std_aprintf(
_tag_arr("declaration of type %s discloses too much information",sizeof(char),54),
_tag_arr(_tmpF2,sizeof(void*),1));}}));return 0;}return 1;}static int Cyc_Interface_incl_tuniondecl(
struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*d1,struct _tagged_arr*
msg){struct Cyc_Absyn_Tuniondecl*_tmpF4=Cyc_Tcdecl_merge_tuniondecl(d0,d1,0,msg);
if(_tmpF4 == 0)return 0;if((struct Cyc_Absyn_Tuniondecl*)d0 != _tmpF4){Cyc_Interface_check_err(
msg,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpF6;_tmpF6.tag=0;
_tmpF6.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(d1->name);{void*_tmpF5[1]={&
_tmpF6};Cyc_Std_aprintf(_tag_arr("declaration of tunion %s discloses too much information",
sizeof(char),56),_tag_arr(_tmpF5,sizeof(void*),1));}}));return 0;}return 1;}static
int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct _tagged_arr*msg){struct Cyc_Absyn_Enumdecl*_tmpF7=Cyc_Tcdecl_merge_enumdecl(
d0,d1,0,msg);if(_tmpF7 == 0)return 0;if((struct Cyc_Absyn_Enumdecl*)d0 != _tmpF7){
Cyc_Interface_check_err(msg,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmpF9;_tmpF9.tag=0;_tmpF9.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(d1->name);{
void*_tmpF8[1]={& _tmpF9};Cyc_Std_aprintf(_tag_arr("declaration of enum %s discloses too much information",
sizeof(char),54),_tag_arr(_tmpF8,sizeof(void*),1));}}));return 0;}return 1;}static
int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*
d1,struct _tagged_arr*msg){struct Cyc_Absyn_Vardecl*_tmpFA=Cyc_Tcdecl_merge_vardecl(
d0,d1,0,msg);if(_tmpFA == 0)return 0;if((struct Cyc_Absyn_Vardecl*)d0 != _tmpFA){Cyc_Interface_check_err(
msg,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmpFC;_tmpFC.tag=0;
_tmpFC.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(d1->name);{void*_tmpFB[1]={&
_tmpFC};Cyc_Std_aprintf(_tag_arr("declaration of variable %s discloses too much information",
sizeof(char),58),_tag_arr(_tmpFB,sizeof(void*),1));}}));return 0;}return 1;}static
int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*
d1,struct _tagged_arr*msg){struct Cyc_Absyn_Typedefdecl*_tmpFD=Cyc_Tcdecl_merge_typedefdecl(
d0,d1,0,msg);if(_tmpFD == 0)return 0;if((struct Cyc_Absyn_Typedefdecl*)d0 != _tmpFD){
Cyc_Interface_check_err(msg,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmpFF;_tmpFF.tag=0;_tmpFF.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(d1->name);{
void*_tmpFE[1]={& _tmpFF};Cyc_Std_aprintf(_tag_arr("declaration of typedef %s discloses too much information",
sizeof(char),57),_tag_arr(_tmpFE,sizeof(void*),1));}}));return 0;}return 1;}static
int Cyc_Interface_incl_xtunionfielddecl(struct Cyc_Tcdecl_Xtunionfielddecl*d0,
struct Cyc_Tcdecl_Xtunionfielddecl*d1,struct _tagged_arr*msg){struct Cyc_Tcdecl_Xtunionfielddecl*
_tmp100=Cyc_Tcdecl_merge_xtunionfielddecl(d0,d1,0,msg);if(_tmp100 == 0)return 0;
if((struct Cyc_Tcdecl_Xtunionfielddecl*)d0 != _tmp100){Cyc_Interface_check_err(msg,(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp102;_tmp102.tag=0;_tmp102.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string((d1->field)->name);{void*_tmp101[1]={&
_tmp102};Cyc_Std_aprintf(_tag_arr("declaration of xtunionfield %s discloses too much information",
sizeof(char),62),_tag_arr(_tmp101,sizeof(void*),1));}}));return 0;}return 1;}
struct Cyc_Core_Opt*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict*dic1,
struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,void*(*merge_f)(void*,void*,
struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg);static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*ie1,
struct Cyc_Interface_Ienv*ie2,struct _tagged_arr*msg){int _tmp103=((int(*)(struct
Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Absyn_Aggrdecl*,
struct Cyc_Absyn_Aggrdecl*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_incl_dict)(ie1->aggrdecls,ie2->aggrdecls,Cyc_Interface_incl_aggrdecl,
_tag_arr("type",sizeof(char),5),msg);int _tmp104=((int(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Absyn_Tuniondecl*,struct Cyc_Absyn_Tuniondecl*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_incl_dict)(
ie1->tuniondecls,ie2->tuniondecls,Cyc_Interface_incl_tuniondecl,_tag_arr("tunion",
sizeof(char),7),msg);int _tmp105=((int(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,int(*incl_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct
_tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_incl_dict)(
ie1->enumdecls,ie2->enumdecls,Cyc_Interface_incl_enumdecl,_tag_arr("enum",
sizeof(char),5),msg);int _tmp106=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Typedefdecl*(*
merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,
_tag_arr("typedef",sizeof(char),8),msg)!= 0;int _tmp107=((int(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_incl_dict)(
ie1->vardecls,ie2->vardecls,Cyc_Interface_incl_vardecl,_tag_arr("variable",
sizeof(char),9),msg);int _tmp108=((int(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,int(*incl_f)(struct Cyc_Tcdecl_Xtunionfielddecl*,struct Cyc_Tcdecl_Xtunionfielddecl*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_incl_dict)(
ie1->xtunionfielddecls,ie2->xtunionfielddecls,Cyc_Interface_incl_xtunionfielddecl,
_tag_arr("xtunionfield",sizeof(char),13),msg);return((((_tmp103?_tmp104: 0)?
_tmp105: 0)?_tmp106: 0)?_tmp107: 0)?_tmp108: 0;}struct _tuple20{int f1;struct Cyc_Dict_Dict*
f2;struct _tagged_arr*f3;};static void Cyc_Interface_incl_ns_f(struct _tuple20*env,
struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie1){struct _tuple20 _tmp10A;int
_tmp10B;int*_tmp10C;struct Cyc_Dict_Dict*_tmp10D;struct _tagged_arr*_tmp10E;struct
_tuple20*_tmp109=env;_tmp10A=*_tmp109;_tmp10B=_tmp10A.f1;_tmp10C=(int*)&(*
_tmp109).f1;_tmp10D=_tmp10A.f2;_tmp10E=_tmp10A.f3;{struct Cyc_Interface_Ienv*ie2;{
struct _handler_cons _tmp10F;_push_handler(& _tmp10F);{int _tmp111=0;if(setjmp(
_tmp10F.handler))_tmp111=1;if(!_tmp111){ie2=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp10D,ns);;
_pop_handler();}else{void*_tmp110=(void*)_exn_thrown;void*_tmp113=_tmp110;_LL86:
if(_tmp113 != Cyc_Dict_Absent)goto _LL88;_LL87: ie2=Cyc_Interface_lazy_new_ienv();
goto _LL85;_LL88:;_LL89:(void)_throw(_tmp113);_LL85:;}}}if(!Cyc_Interface_incl_ienv(
ie1,ie2,_tmp10E))*_tmp10C=0;}}static int Cyc_Interface_incl_ns(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr*msg){struct _tuple20 _tmp114=({
struct _tuple20 _tmp115;_tmp115.f1=1;_tmp115.f2=dic2;_tmp115.f3=msg;_tmp115;});((
void(*)(void(*f)(struct _tuple20*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct _tuple20*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_incl_ns_f,&
_tmp114,dic1);return _tmp114.f1;}int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*
i1,struct Cyc_Interface_I*i2,struct _tuple4*info){struct _handler_cons _tmp116;
_push_handler(& _tmp116);{int _tmp118=0;if(setjmp(_tmp116.handler))_tmp118=1;if(!
_tmp118){{int _tmp119=1;struct _tagged_arr*msg=0;if(info != 0)msg=({struct
_tagged_arr*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp11D;_tmp11D.tag=0;_tmp11D.f1=(struct
_tagged_arr)(*info).f2;{struct Cyc_Std_String_pa_struct _tmp11C;_tmp11C.tag=0;
_tmp11C.f1=(struct _tagged_arr)(*info).f1;{void*_tmp11B[2]={& _tmp11C,& _tmp11D};
Cyc_Std_aprintf(_tag_arr("checking inclusion of %s exports into %s exports,",
sizeof(char),50),_tag_arr(_tmp11B,sizeof(void*),2));}}});_tmp11A;});if(!Cyc_Interface_incl_ns(
i1->exports,i2->exports,msg))_tmp119=0;if(info != 0)msg=({struct _tagged_arr*
_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E[0]=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp121;_tmp121.tag=0;_tmp121.f1=(struct _tagged_arr)(*info).f1;{struct Cyc_Std_String_pa_struct
_tmp120;_tmp120.tag=0;_tmp120.f1=(struct _tagged_arr)(*info).f2;{void*_tmp11F[2]={&
_tmp120,& _tmp121};Cyc_Std_aprintf(_tag_arr("checking inclusion of %s imports into %s imports,",
sizeof(char),50),_tag_arr(_tmp11F,sizeof(void*),2));}}});_tmp11E;});if(!Cyc_Interface_incl_ns(
i2->imports,i1->imports,msg))_tmp119=0;{int _tmp122=_tmp119;_npop_handler(0);
return _tmp122;}};_pop_handler();}else{void*_tmp117=(void*)_exn_thrown;void*
_tmp124=_tmp117;_LL8B: if(_tmp124 != Cyc_Tcdecl_Incompatible)goto _LL8D;_LL8C:
return 0;_LL8D:;_LL8E:(void)_throw(_tmp124);_LL8A:;}}}struct _tuple21{int f1;struct
Cyc_Dict_Dict*f2;struct Cyc_Dict_Dict*f3;struct Cyc_Dict_Dict*f4;void*(*f5)(void*,
void*,struct Cyc_Position_Segment*,struct _tagged_arr*);struct _tagged_arr f6;struct
_tagged_arr*f7;};void Cyc_Interface_compat_merge_dict_f(struct _tuple21*env,struct
_tagged_arr*x,void*y2){struct _tuple21 _tmp126;int _tmp127;int*_tmp128;struct Cyc_Dict_Dict*
_tmp129;struct Cyc_Dict_Dict**_tmp12A;struct Cyc_Dict_Dict*_tmp12B;struct Cyc_Dict_Dict*
_tmp12C;void*(*_tmp12D)(void*,void*,struct Cyc_Position_Segment*,struct
_tagged_arr*);struct _tagged_arr _tmp12E;struct _tagged_arr*_tmp12F;struct _tuple21*
_tmp125=env;_tmp126=*_tmp125;_tmp127=_tmp126.f1;_tmp128=(int*)&(*_tmp125).f1;
_tmp129=_tmp126.f2;_tmp12A=(struct Cyc_Dict_Dict**)&(*_tmp125).f2;_tmp12B=_tmp126.f3;
_tmp12C=_tmp126.f4;_tmp12D=_tmp126.f5;_tmp12E=_tmp126.f6;_tmp12F=_tmp126.f7;{
void*y;{struct _handler_cons _tmp130;_push_handler(& _tmp130);{int _tmp132=0;if(
setjmp(_tmp130.handler))_tmp132=1;if(!_tmp132){{void*_tmp133=((void*(*)(struct
Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp12B,x);void*_tmp134=
_tmp12D(_tmp133,y2,0,_tmp12F);if(!((unsigned int)_tmp134)){*_tmp128=0;
_npop_handler(0);return;}y=(void*)_check_null(_tmp134);};_pop_handler();}else{
void*_tmp131=(void*)_exn_thrown;void*_tmp136=_tmp131;_LL90: if(_tmp136 != Cyc_Dict_Absent)
goto _LL92;_LL91: y=y2;goto _LL8F;_LL92:;_LL93:(void)_throw(_tmp136);_LL8F:;}}}{
struct _handler_cons _tmp137;_push_handler(& _tmp137);{int _tmp139=0;if(setjmp(
_tmp137.handler))_tmp139=1;if(!_tmp139){{void*_tmp13A=((void*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp12C,x);void*_tmp13B=_tmp12D(_tmp13A,
y,0,_tmp12F);if(_tmp13B != (void*)_tmp13A){if((unsigned int)_tmp13B)Cyc_Interface_check_err(
_tmp12F,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp13E;_tmp13E.tag=0;
_tmp13E.f1=(struct _tagged_arr)*x;{struct Cyc_Std_String_pa_struct _tmp13D;_tmp13D.tag=
0;_tmp13D.f1=(struct _tagged_arr)_tmp12E;{void*_tmp13C[2]={& _tmp13D,& _tmp13E};Cyc_Std_aprintf(
_tag_arr("abstract %s %s is being imported as non-abstract",sizeof(char),49),
_tag_arr(_tmp13C,sizeof(void*),2));}}}));*_tmp128=0;}};_pop_handler();}else{void*
_tmp138=(void*)_exn_thrown;void*_tmp140=_tmp138;_LL95: if(_tmp140 != Cyc_Dict_Absent)
goto _LL97;_LL96: if(*_tmp128)*_tmp12A=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(*_tmp12A,x,y);goto _LL94;_LL97:;
_LL98:(void)_throw(_tmp140);_LL94:;}}}}}struct Cyc_Core_Opt*Cyc_Interface_compat_merge_dict(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,void*(*
merge_f)(void*,void*,struct Cyc_Position_Segment*,struct _tagged_arr*),struct
_tagged_arr t,struct _tagged_arr*msg){struct _tuple21 _tmp141=({struct _tuple21
_tmp143;_tmp143.f1=1;_tmp143.f2=dic1;_tmp143.f3=dic1;_tmp143.f4=excl;_tmp143.f5=
merge_f;_tmp143.f6=t;_tmp143.f7=msg;_tmp143;});((void(*)(void(*f)(struct _tuple21*,
struct _tagged_arr*,void*),struct _tuple21*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_compat_merge_dict_f,& _tmp141,dic2);if(_tmp141.f1)return({struct Cyc_Core_Opt*
_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->v=_tmp141.f2;_tmp142;});return 0;}
struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct Cyc_Interface_Ienv*
ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*iexcl,struct
_tagged_arr*msg){struct Cyc_Core_Opt*_tmp144=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Aggrdecl*(*
merge_f)(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->aggrdecls,ie2->aggrdecls,iexcl->aggrdecls,Cyc_Tcdecl_merge_aggrdecl,
_tag_arr("type",sizeof(char),5),msg);struct Cyc_Core_Opt*_tmp145=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,
struct Cyc_Absyn_Tuniondecl*(*merge_f)(struct Cyc_Absyn_Tuniondecl*,struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->tuniondecls,ie2->tuniondecls,
iexcl->tuniondecls,Cyc_Tcdecl_merge_tuniondecl,_tag_arr("tunion",sizeof(char),7),
msg);struct Cyc_Core_Opt*_tmp146=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Enumdecl*(*
merge_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->enumdecls,ie2->enumdecls,iexcl->enumdecls,Cyc_Tcdecl_merge_enumdecl,
_tag_arr("enum",sizeof(char),5),msg);struct Cyc_Core_Opt*_tmp147=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,
struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,ie2->typedefdecls,
iexcl->typedefdecls,Cyc_Tcdecl_merge_typedefdecl,_tag_arr("typedef",sizeof(char),
8),msg);struct Cyc_Core_Opt*_tmp148=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Vardecl*(*
merge_f)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->vardecls,ie2->vardecls,iexcl->vardecls,Cyc_Tcdecl_merge_vardecl,_tag_arr("variable",
sizeof(char),9),msg);struct Cyc_Core_Opt*_tmp149=((struct Cyc_Core_Opt*(*)(struct
Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Tcdecl_Xtunionfielddecl*(*
merge_f)(struct Cyc_Tcdecl_Xtunionfielddecl*,struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->xtunionfielddecls,ie2->xtunionfielddecls,
iexcl->xtunionfielddecls,Cyc_Tcdecl_merge_xtunionfielddecl,_tag_arr("xtunionfield",
sizeof(char),13),msg);if(((((!((unsigned int)_tmp144)?1: !((unsigned int)_tmp145))?
1: !((unsigned int)_tmp146))?1: !((unsigned int)_tmp147))?1: !((unsigned int)
_tmp148))?1: !((unsigned int)_tmp149))return 0;return({struct Cyc_Interface_Ienv*
_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->aggrdecls=(struct Cyc_Dict_Dict*)
_tmp144->v;_tmp14A->tuniondecls=(struct Cyc_Dict_Dict*)_tmp145->v;_tmp14A->enumdecls=(
struct Cyc_Dict_Dict*)_tmp146->v;_tmp14A->typedefdecls=(struct Cyc_Dict_Dict*)
_tmp147->v;_tmp14A->vardecls=(struct Cyc_Dict_Dict*)_tmp148->v;_tmp14A->xtunionfielddecls=(
struct Cyc_Dict_Dict*)_tmp149->v;_tmp14A;});}struct _tuple22{int f1;struct Cyc_Dict_Dict*
f2;struct Cyc_Dict_Dict*f3;struct Cyc_Dict_Dict*f4;struct _tagged_arr*f5;};void Cyc_Interface_compat_merge_ns_f(
struct _tuple22*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){struct
_tuple22 _tmp14C;int _tmp14D;int*_tmp14E;struct Cyc_Dict_Dict*_tmp14F;struct Cyc_Dict_Dict**
_tmp150;struct Cyc_Dict_Dict*_tmp151;struct Cyc_Dict_Dict*_tmp152;struct
_tagged_arr*_tmp153;struct _tuple22*_tmp14B=env;_tmp14C=*_tmp14B;_tmp14D=_tmp14C.f1;
_tmp14E=(int*)&(*_tmp14B).f1;_tmp14F=_tmp14C.f2;_tmp150=(struct Cyc_Dict_Dict**)&(*
_tmp14B).f2;_tmp151=_tmp14C.f3;_tmp152=_tmp14C.f4;_tmp153=_tmp14C.f5;{struct Cyc_Interface_Ienv*
ie1;struct Cyc_Interface_Ienv*iexcl;{struct _handler_cons _tmp154;_push_handler(&
_tmp154);{int _tmp156=0;if(setjmp(_tmp154.handler))_tmp156=1;if(!_tmp156){iexcl=((
struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
_tmp152,ns);;_pop_handler();}else{void*_tmp155=(void*)_exn_thrown;void*_tmp158=
_tmp155;_LL9A: if(_tmp158 != Cyc_Dict_Absent)goto _LL9C;_LL9B: iexcl=Cyc_Interface_lazy_new_ienv();
goto _LL99;_LL9C:;_LL9D:(void)_throw(_tmp158);_LL99:;}}}{struct _handler_cons
_tmp159;_push_handler(& _tmp159);{int _tmp15B=0;if(setjmp(_tmp159.handler))_tmp15B=
1;if(!_tmp15B){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(_tmp151,ns);{struct Cyc_Interface_Ienv*_tmp15C=
Cyc_Interface_compat_merge_ienv(Cyc_Interface_lazy_new_ienv(),ie1,iexcl,_tmp153);
if(_tmp15C == 0){*_tmp14E=0;_npop_handler(0);return;}ie1=(struct Cyc_Interface_Ienv*)
_check_null(_tmp15C);};_pop_handler();}else{void*_tmp15A=(void*)_exn_thrown;void*
_tmp15E=_tmp15A;_LL9F: if(_tmp15E != Cyc_Dict_Absent)goto _LLA1;_LLA0: ie1=Cyc_Interface_lazy_new_ienv();
goto _LL9E;_LLA1:;_LLA2:(void)_throw(_tmp15E);_LL9E:;}}}{struct Cyc_Interface_Ienv*
_tmp15F=Cyc_Interface_compat_merge_ienv(ie1,ie2,iexcl,_tmp153);if(_tmp15F == 0)*
_tmp14E=0;else{if(*_tmp14E)*_tmp150=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*_tmp150,ns,(
struct Cyc_Interface_Ienv*)_check_null(_tmp15F));}}}}struct Cyc_Core_Opt*Cyc_Interface_compat_merge_ns(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,
struct _tagged_arr*msg){struct Cyc_Dict_Dict*d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(
struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);
struct _tuple22 _tmp160=({struct _tuple22 _tmp162;_tmp162.f1=1;_tmp162.f2=d;_tmp162.f3=
dic1;_tmp162.f4=excl;_tmp162.f5=msg;_tmp162;});((void(*)(void(*f)(struct _tuple22*,
struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct _tuple22*env,struct Cyc_Dict_Dict*
d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_ns_f,& _tmp160,dic2);if(_tmp160.f1)
return({struct Cyc_Core_Opt*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->v=
_tmp160.f2;_tmp161;});return 0;}struct _tuple23{int f1;struct Cyc_Dict_Dict*f2;
struct Cyc_Dict_Dict*f3;struct _tagged_arr f4;struct _tagged_arr*f5;};void Cyc_Interface_disj_merge_dict_f(
struct _tuple23*env,struct _tagged_arr*x,void*y){struct _tuple23 _tmp164;int _tmp165;
int*_tmp166;struct Cyc_Dict_Dict*_tmp167;struct Cyc_Dict_Dict**_tmp168;struct Cyc_Dict_Dict*
_tmp169;struct _tagged_arr _tmp16A;struct _tagged_arr*_tmp16B;struct _tuple23*
_tmp163=env;_tmp164=*_tmp163;_tmp165=_tmp164.f1;_tmp166=(int*)&(*_tmp163).f1;
_tmp167=_tmp164.f2;_tmp168=(struct Cyc_Dict_Dict**)&(*_tmp163).f2;_tmp169=_tmp164.f3;
_tmp16A=_tmp164.f4;_tmp16B=_tmp164.f5;if(((int(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k))Cyc_Dict_member)(_tmp169,x)){Cyc_Interface_check_err(_tmp16B,(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp16E;_tmp16E.tag=0;_tmp16E.f1=(
struct _tagged_arr)*x;{struct Cyc_Std_String_pa_struct _tmp16D;_tmp16D.tag=0;
_tmp16D.f1=(struct _tagged_arr)_tmp16A;{void*_tmp16C[2]={& _tmp16D,& _tmp16E};Cyc_Std_aprintf(
_tag_arr("%s %s is exported more than once",sizeof(char),33),_tag_arr(_tmp16C,
sizeof(void*),2));}}}));*_tmp166=0;}else{if(*_tmp166)*_tmp168=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(*_tmp168,x,y);}}
struct Cyc_Core_Opt*Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict*dic1,struct
Cyc_Dict_Dict*dic2,struct _tagged_arr t,struct _tagged_arr*msg){struct _tuple23
_tmp16F=({struct _tuple23 _tmp171;_tmp171.f1=1;_tmp171.f2=dic1;_tmp171.f3=dic1;
_tmp171.f4=t;_tmp171.f5=msg;_tmp171;});((void(*)(void(*f)(struct _tuple23*,struct
_tagged_arr*,void*),struct _tuple23*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_disj_merge_dict_f,& _tmp16F,dic2);if(_tmp16F.f1)return({struct Cyc_Core_Opt*
_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->v=_tmp16F.f2;_tmp170;});return 0;}
struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*
ie1,struct Cyc_Interface_Ienv*ie2,struct _tagged_arr*msg){struct Cyc_Core_Opt*
_tmp172=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Aggrdecl*(*merge_f)(struct Cyc_Absyn_Aggrdecl*,
struct Cyc_Absyn_Aggrdecl*,struct Cyc_Position_Segment*,struct _tagged_arr*),struct
_tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->aggrdecls,
ie2->aggrdecls,((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp),Cyc_Tcdecl_merge_aggrdecl,
_tag_arr("type",sizeof(char),5),msg);struct Cyc_Core_Opt*_tmp173=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_disj_merge_dict)(ie1->tuniondecls,ie2->tuniondecls,
_tag_arr("[x]tunion",sizeof(char),10),msg);struct Cyc_Core_Opt*_tmp174=((struct
Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct
_tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_disj_merge_dict)(ie1->enumdecls,
ie2->enumdecls,_tag_arr("enum",sizeof(char),5),msg);struct Cyc_Core_Opt*_tmp175=((
struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct
Cyc_Dict_Dict*excl,struct Cyc_Absyn_Typedefdecl*(*merge_f)(struct Cyc_Absyn_Typedefdecl*,
struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,struct _tagged_arr*),
struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->typedefdecls,
ie2->typedefdecls,((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp),Cyc_Tcdecl_merge_typedefdecl,
_tag_arr("typedef",sizeof(char),8),msg);struct Cyc_Core_Opt*_tmp176=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_disj_merge_dict)(ie1->vardecls,ie2->vardecls,
_tag_arr("variable",sizeof(char),9),msg);struct Cyc_Core_Opt*_tmp177=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_disj_merge_dict)(ie1->xtunionfielddecls,ie2->xtunionfielddecls,
_tag_arr("xtunionfield",sizeof(char),13),msg);if(((((!((unsigned int)_tmp172)?1:
!((unsigned int)_tmp173))?1: !((unsigned int)_tmp174))?1: !((unsigned int)_tmp175))?
1: !((unsigned int)_tmp176))?1: !((unsigned int)_tmp177))return 0;return({struct Cyc_Interface_Ienv*
_tmp178=_cycalloc(sizeof(*_tmp178));_tmp178->aggrdecls=(struct Cyc_Dict_Dict*)
_tmp172->v;_tmp178->tuniondecls=(struct Cyc_Dict_Dict*)_tmp173->v;_tmp178->enumdecls=(
struct Cyc_Dict_Dict*)_tmp174->v;_tmp178->typedefdecls=(struct Cyc_Dict_Dict*)
_tmp175->v;_tmp178->vardecls=(struct Cyc_Dict_Dict*)_tmp176->v;_tmp178->xtunionfielddecls=(
struct Cyc_Dict_Dict*)_tmp177->v;_tmp178;});}struct _tuple24{int f1;struct Cyc_Dict_Dict*
f2;struct Cyc_Dict_Dict*f3;struct _tagged_arr*f4;};void Cyc_Interface_disj_merge_ns_f(
struct _tuple24*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){struct
_tuple24 _tmp17A;int _tmp17B;int*_tmp17C;struct Cyc_Dict_Dict*_tmp17D;struct Cyc_Dict_Dict**
_tmp17E;struct Cyc_Dict_Dict*_tmp17F;struct _tagged_arr*_tmp180;struct _tuple24*
_tmp179=env;_tmp17A=*_tmp179;_tmp17B=_tmp17A.f1;_tmp17C=(int*)&(*_tmp179).f1;
_tmp17D=_tmp17A.f2;_tmp17E=(struct Cyc_Dict_Dict**)&(*_tmp179).f2;_tmp17F=_tmp17A.f3;
_tmp180=_tmp17A.f4;{struct Cyc_Interface_Ienv*ie1;{struct _handler_cons _tmp181;
_push_handler(& _tmp181);{int _tmp183=0;if(setjmp(_tmp181.handler))_tmp183=1;if(!
_tmp183){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmp17F,ns);;_pop_handler();}else{void*_tmp182=(void*)
_exn_thrown;void*_tmp185=_tmp182;_LLA4: if(_tmp185 != Cyc_Dict_Absent)goto _LLA6;
_LLA5: ie1=Cyc_Interface_lazy_new_ienv();goto _LLA3;_LLA6:;_LLA7:(void)_throw(
_tmp185);_LLA3:;}}}{struct Cyc_Interface_Ienv*_tmp186=Cyc_Interface_disj_merge_ienv(
ie1,ie2,_tmp180);if(_tmp186 == 0)*_tmp17C=0;else{if(*_tmp17C)*_tmp17E=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*
_tmp17E,ns,(struct Cyc_Interface_Ienv*)_check_null(_tmp186));}}}}struct Cyc_Core_Opt*
Cyc_Interface_disj_merge_ns(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,
struct _tagged_arr*msg){struct _tuple24 _tmp187=({struct _tuple24 _tmp189;_tmp189.f1=
1;_tmp189.f2=dic1;_tmp189.f3=dic1;_tmp189.f4=msg;_tmp189;});((void(*)(void(*f)(
struct _tuple24*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct _tuple24*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_ns_f,&
_tmp187,dic2);if(_tmp187.f1)return({struct Cyc_Core_Opt*_tmp188=_cycalloc(sizeof(*
_tmp188));_tmp188->v=_tmp187.f2;_tmp188;});return 0;}struct Cyc_Interface_I*Cyc_Interface_merge(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple4*info){struct
_handler_cons _tmp18A;_push_handler(& _tmp18A);{int _tmp18C=0;if(setjmp(_tmp18A.handler))
_tmp18C=1;if(!_tmp18C){{struct _tagged_arr*msg=0;if(info != 0)msg=({struct
_tagged_arr*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D[0]=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp190;_tmp190.tag=0;_tmp190.f1=(struct
_tagged_arr)(*info).f2;{struct Cyc_Std_String_pa_struct _tmp18F;_tmp18F.tag=0;
_tmp18F.f1=(struct _tagged_arr)(*info).f1;{void*_tmp18E[2]={& _tmp18F,& _tmp190};
Cyc_Std_aprintf(_tag_arr("merging exports of %s and %s,",sizeof(char),30),
_tag_arr(_tmp18E,sizeof(void*),2));}}});_tmp18D;});{struct Cyc_Core_Opt*_tmp191=
Cyc_Interface_disj_merge_ns(i1->exports,i2->exports,msg);if(_tmp191 == 0){struct
Cyc_Interface_I*_tmp192=0;_npop_handler(0);return _tmp192;}if(info != 0)msg=({
struct _tagged_arr*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193[0]=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp196;_tmp196.tag=0;_tmp196.f1=(
struct _tagged_arr)(*info).f2;{struct Cyc_Std_String_pa_struct _tmp195;_tmp195.tag=
0;_tmp195.f1=(struct _tagged_arr)(*info).f1;{void*_tmp194[2]={& _tmp195,& _tmp196};
Cyc_Std_aprintf(_tag_arr("merging imports of %s and %s,",sizeof(char),30),
_tag_arr(_tmp194,sizeof(void*),2));}}});_tmp193;});{struct Cyc_Core_Opt*_tmp197=
Cyc_Interface_compat_merge_ns(i1->imports,i2->imports,(struct Cyc_Dict_Dict*)
_tmp191->v,msg);if(_tmp197 == 0){struct Cyc_Interface_I*_tmp198=0;_npop_handler(0);
return _tmp198;}{struct Cyc_Interface_I*_tmp19A=({struct Cyc_Interface_I*_tmp199=
_cycalloc(sizeof(*_tmp199));_tmp199->imports=(struct Cyc_Dict_Dict*)_tmp197->v;
_tmp199->exports=(struct Cyc_Dict_Dict*)_tmp191->v;_tmp199;});_npop_handler(0);
return _tmp19A;}}}};_pop_handler();}else{void*_tmp18B=(void*)_exn_thrown;void*
_tmp19C=_tmp18B;_LLA9: if(_tmp19C != Cyc_Tcdecl_Incompatible)goto _LLAB;_LLAA:
return 0;_LLAB:;_LLAC:(void)_throw(_tmp19C);_LLA8:;}}}struct Cyc_Interface_I*Cyc_Interface_merge_list(
struct Cyc_List_List*li,struct Cyc_List_List*linfo){if(li == 0)return(struct Cyc_Interface_I*)
Cyc_Interface_empty();{struct Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)li->hd;
struct _tagged_arr*curr_info=linfo != 0?(struct _tagged_arr*)((struct _tagged_arr*)
linfo->hd): 0;li=li->tl;if(linfo != 0)linfo=linfo->tl;for(0;li != 0;li=li->tl){
struct Cyc_Interface_I*_tmp19D=Cyc_Interface_merge(curr_i,(struct Cyc_Interface_I*)
li->hd,(curr_info != 0?linfo != 0: 0)?({struct _tuple4*_tmp1A2=_cycalloc(sizeof(*
_tmp1A2));_tmp1A2->f1=*curr_info;_tmp1A2->f2=*((struct _tagged_arr*)linfo->hd);
_tmp1A2;}): 0);if(_tmp19D == 0)return 0;curr_i=(struct Cyc_Interface_I*)_check_null(
_tmp19D);if(curr_info != 0)curr_info=linfo != 0?({struct _tagged_arr*_tmp19E=
_cycalloc(sizeof(*_tmp19E));_tmp19E[0]=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp1A1;_tmp1A1.tag=0;_tmp1A1.f1=(struct _tagged_arr)*((struct _tagged_arr*)linfo->hd);{
struct Cyc_Std_String_pa_struct _tmp1A0;_tmp1A0.tag=0;_tmp1A0.f1=(struct
_tagged_arr)*curr_info;{void*_tmp19F[2]={& _tmp1A0,& _tmp1A1};Cyc_Std_aprintf(
_tag_arr("%s+%s",sizeof(char),6),_tag_arr(_tmp19F,sizeof(void*),2));}}});_tmp19E;}):
0;if(linfo != 0)linfo=linfo->tl;}return(struct Cyc_Interface_I*)curr_i;}}struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo){if(la == 0)return(struct Cyc_Interface_I*)Cyc_Interface_empty();{
struct Cyc_Interface_I*curr_i=get((void*)la->hd);struct _tagged_arr*curr_info=
linfo != 0?(struct _tagged_arr*)((struct _tagged_arr*)linfo->hd): 0;la=la->tl;if(
linfo != 0)linfo=linfo->tl;for(0;la != 0;la=la->tl){struct Cyc_Interface_I*_tmp1A3=
Cyc_Interface_merge(curr_i,get((void*)la->hd),(curr_info != 0?linfo != 0: 0)?({
struct _tuple4*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));_tmp1A8->f1=*curr_info;_tmp1A8->f2=*((
struct _tagged_arr*)linfo->hd);_tmp1A8;}): 0);if(_tmp1A3 == 0)return 0;curr_i=(
struct Cyc_Interface_I*)_check_null(_tmp1A3);if(curr_info != 0)curr_info=linfo != 0?({
struct _tagged_arr*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4[0]=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp1A7;_tmp1A7.tag=0;_tmp1A7.f1=(
struct _tagged_arr)*((struct _tagged_arr*)linfo->hd);{struct Cyc_Std_String_pa_struct
_tmp1A6;_tmp1A6.tag=0;_tmp1A6.f1=(struct _tagged_arr)*curr_info;{void*_tmp1A5[2]={&
_tmp1A6,& _tmp1A7};Cyc_Std_aprintf(_tag_arr("%s+%s",sizeof(char),6),_tag_arr(
_tmp1A5,sizeof(void*),2));}}});_tmp1A4;}): 0;if(linfo != 0)linfo=linfo->tl;}return(
struct Cyc_Interface_I*)curr_i;}}static struct Cyc_List_List*Cyc_Interface_add_namespace(
struct Cyc_List_List*tds,struct Cyc_List_List*ns){if(ns == 0)return tds;return({
struct Cyc_List_List*_tmp1A9=_cycalloc(sizeof(*_tmp1A9));_tmp1A9->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));
_tmp1AA[0]=({struct Cyc_Absyn_Namespace_d_struct _tmp1AB;_tmp1AB.tag=8;_tmp1AB.f1=(
struct _tagged_arr*)ns->hd;_tmp1AB.f2=Cyc_Interface_add_namespace(tds,ns->tl);
_tmp1AB;});_tmp1AA;}),0);_tmp1A9->tl=0;_tmp1A9;});}static struct Cyc_List_List*Cyc_Interface_add_aggrdecl(
struct _tagged_arr*x,struct Cyc_Absyn_Aggrdecl*d,struct Cyc_List_List*tds){return({
struct Cyc_List_List*_tmp1AC=_cycalloc(sizeof(*_tmp1AC));_tmp1AC->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));
_tmp1AD[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp1AE;_tmp1AE.tag=4;_tmp1AE.f1=d;
_tmp1AE;});_tmp1AD;}),0);_tmp1AC->tl=tds;_tmp1AC;});}static struct Cyc_List_List*
Cyc_Interface_add_aggrdecl_header(struct _tagged_arr*x,struct Cyc_Absyn_Aggrdecl*d,
struct Cyc_List_List*tds){d=({struct Cyc_Absyn_Aggrdecl*_tmp1AF=_cycalloc(sizeof(*
_tmp1AF));_tmp1AF[0]=*d;_tmp1AF;});d->impl=0;if((void*)d->sc != (void*)4)(void*)(
d->sc=(void*)((void*)3));return({struct Cyc_List_List*_tmp1B0=_cycalloc(sizeof(*
_tmp1B0));_tmp1B0->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp1B1=_cycalloc(sizeof(*_tmp1B1));_tmp1B1[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp1B2;_tmp1B2.tag=4;_tmp1B2.f1=d;_tmp1B2;});_tmp1B1;}),0);_tmp1B0->tl=tds;
_tmp1B0;});}static struct Cyc_List_List*Cyc_Interface_add_tuniondecl(struct
_tagged_arr*x,struct Cyc_Absyn_Tuniondecl*d,struct Cyc_List_List*tds){return({
struct Cyc_List_List*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));_tmp1B3->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));
_tmp1B4[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp1B5;_tmp1B5.tag=5;_tmp1B5.f1=d;
_tmp1B5;});_tmp1B4;}),0);_tmp1B3->tl=tds;_tmp1B3;});}static char _tmp1B6[2]="_";
static struct _tagged_arr Cyc_Interface_us={_tmp1B6,_tmp1B6,_tmp1B6 + 2};static
struct _tagged_arr*Cyc_Interface_us_p=& Cyc_Interface_us;static struct _tuple16*Cyc_Interface_rewrite_tunionfield_type(
struct _tuple16*x){return({struct _tuple16*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));
_tmp1B7->f1=Cyc_Absyn_empty_tqual();_tmp1B7->f2=(void*)({struct Cyc_Absyn_VarType_struct*
_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8[0]=({struct Cyc_Absyn_VarType_struct
_tmp1B9;_tmp1B9.tag=1;_tmp1B9.f1=({struct Cyc_Absyn_Tvar*_tmp1BA=_cycalloc(
sizeof(*_tmp1BA));_tmp1BA->name=Cyc_Interface_us_p;_tmp1BA->identity=0;_tmp1BA->kind=(
void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp1BB=_cycalloc(sizeof(*_tmp1BB));
_tmp1BB[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp1BC;_tmp1BC.tag=0;_tmp1BC.f1=(void*)((
void*)2);_tmp1BC;});_tmp1BB;}));_tmp1BA;});_tmp1B9;});_tmp1B8;});_tmp1B7;});}
static struct Cyc_Absyn_Tunionfield*Cyc_Interface_rewrite_tunionfield(struct Cyc_Absyn_Tunionfield*
f){f=({struct Cyc_Absyn_Tunionfield*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));_tmp1BD[0]=*
f;_tmp1BD;});f->typs=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct
_tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_tunionfield_type,
f->typs);return f;}static struct Cyc_List_List*Cyc_Interface_add_tuniondecl_header(
struct _tagged_arr*x,struct Cyc_Absyn_Tuniondecl*d,struct Cyc_List_List*tds){d=({
struct Cyc_Absyn_Tuniondecl*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE[0]=*d;
_tmp1BE;});if(d->fields != 0)d->fields=({struct Cyc_Core_Opt*_tmp1BF=_cycalloc(
sizeof(*_tmp1BF));_tmp1BF->v=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tunionfield*(*
f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_tunionfield,(
struct Cyc_List_List*)(d->fields)->v);_tmp1BF;});if((void*)d->sc != (void*)4)(void*)(
d->sc=(void*)((void*)3));return({struct Cyc_List_List*_tmp1C0=_cycalloc(sizeof(*
_tmp1C0));_tmp1C0->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp1C2;_tmp1C2.tag=5;_tmp1C2.f1=d;_tmp1C2;});_tmp1C1;}),0);_tmp1C0->tl=tds;
_tmp1C0;});}static struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct
_tagged_arr*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){return({struct
Cyc_List_List*_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Enum_d_struct*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));
_tmp1C4[0]=({struct Cyc_Absyn_Enum_d_struct _tmp1C5;_tmp1C5.tag=6;_tmp1C5.f1=d;
_tmp1C5;});_tmp1C4;}),0);_tmp1C3->tl=tds;_tmp1C3;});}static struct Cyc_List_List*
Cyc_Interface_add_enumdecl_header(struct _tagged_arr*x,struct Cyc_Absyn_Enumdecl*d,
struct Cyc_List_List*tds){d=({struct Cyc_Absyn_Enumdecl*_tmp1C6=_cycalloc(sizeof(*
_tmp1C6));_tmp1C6[0]=*d;_tmp1C6;});d->fields=0;if((void*)d->sc != (void*)4)(void*)(
d->sc=(void*)((void*)3));return({struct Cyc_List_List*_tmp1C7=_cycalloc(sizeof(*
_tmp1C7));_tmp1C7->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp1C8=_cycalloc(sizeof(*_tmp1C8));_tmp1C8[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp1C9;_tmp1C9.tag=6;_tmp1C9.f1=d;_tmp1C9;});_tmp1C8;}),0);_tmp1C7->tl=tds;
_tmp1C7;});}static struct Cyc_List_List*Cyc_Interface_add_typedef(struct
_tagged_arr*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){return({
struct Cyc_List_List*_tmp1CA=_cycalloc(sizeof(*_tmp1CA));_tmp1CA->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Typedef_d_struct*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));
_tmp1CB[0]=({struct Cyc_Absyn_Typedef_d_struct _tmp1CC;_tmp1CC.tag=7;_tmp1CC.f1=d;
_tmp1CC;});_tmp1CB;}),0);_tmp1CA->tl=tds;_tmp1CA;});}static struct Cyc_List_List*
Cyc_Interface_add_vardecl(struct _tagged_arr*x,struct Cyc_Absyn_Vardecl*d,struct
Cyc_List_List*tds){return({struct Cyc_List_List*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));
_tmp1CD->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp1CE=
_cycalloc(sizeof(*_tmp1CE));_tmp1CE[0]=({struct Cyc_Absyn_Var_d_struct _tmp1CF;
_tmp1CF.tag=0;_tmp1CF.f1=d;_tmp1CF;});_tmp1CE;}),0);_tmp1CD->tl=tds;_tmp1CD;});}
static struct Cyc_List_List*Cyc_Interface_add_xtunionfielddecl(struct _tagged_arr*x,
struct Cyc_Tcdecl_Xtunionfielddecl*d,struct Cyc_List_List*tds){struct Cyc_Tcdecl_Xtunionfielddecl
_tmp1D1;struct Cyc_Absyn_Tuniondecl*_tmp1D2;struct Cyc_Absyn_Tunionfield*_tmp1D3;
struct Cyc_Tcdecl_Xtunionfielddecl*_tmp1D0=d;_tmp1D1=*_tmp1D0;_tmp1D2=_tmp1D1.base;
_tmp1D3=_tmp1D1.field;_tmp1D2=({struct Cyc_Absyn_Tuniondecl*_tmp1D4=_cycalloc(
sizeof(*_tmp1D4));_tmp1D4[0]=*_tmp1D2;_tmp1D4;});_tmp1D2->fields=({struct Cyc_Core_Opt*
_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5->v=({struct Cyc_List_List*_tmp1D6=
_cycalloc(sizeof(*_tmp1D6));_tmp1D6->hd=_tmp1D3;_tmp1D6->tl=0;_tmp1D6;});_tmp1D5;});(
void*)(_tmp1D2->sc=(void*)((void*)3));return({struct Cyc_List_List*_tmp1D7=
_cycalloc(sizeof(*_tmp1D7));_tmp1D7->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp1D9;_tmp1D9.tag=5;_tmp1D9.f1=_tmp1D2;_tmp1D9;});_tmp1D8;}),0);_tmp1D7->tl=
tds;_tmp1D7;});}static struct Cyc_List_List*Cyc_Interface_add_xtunionfielddecl_header(
struct _tagged_arr*x,struct Cyc_Tcdecl_Xtunionfielddecl*d,struct Cyc_List_List*tds){
struct Cyc_Tcdecl_Xtunionfielddecl _tmp1DB;struct Cyc_Absyn_Tuniondecl*_tmp1DC;
struct Cyc_Absyn_Tunionfield*_tmp1DD;struct Cyc_Tcdecl_Xtunionfielddecl*_tmp1DA=d;
_tmp1DB=*_tmp1DA;_tmp1DC=_tmp1DB.base;_tmp1DD=_tmp1DB.field;_tmp1DC=({struct Cyc_Absyn_Tuniondecl*
_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE[0]=*_tmp1DC;_tmp1DE;});_tmp1DD=Cyc_Interface_rewrite_tunionfield(
_tmp1DD);(void*)(_tmp1DD->sc=(void*)((void*)3));_tmp1DC->fields=({struct Cyc_Core_Opt*
_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF->v=({struct Cyc_List_List*_tmp1E0=
_cycalloc(sizeof(*_tmp1E0));_tmp1E0->hd=_tmp1DD;_tmp1E0->tl=0;_tmp1E0;});_tmp1DF;});(
void*)(_tmp1DC->sc=(void*)((void*)3));return({struct Cyc_List_List*_tmp1E1=
_cycalloc(sizeof(*_tmp1E1));_tmp1E1->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp1E2=_cycalloc(sizeof(*_tmp1E2));_tmp1E2[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp1E3;_tmp1E3.tag=5;_tmp1E3.f1=_tmp1DC;_tmp1E3;});_tmp1E2;}),0);_tmp1E1->tl=
tds;_tmp1E1;});}static void Cyc_Interface_print_ns_headers(struct Cyc_Std___cycFILE*
f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*
_tmp1E4=0;_tmp1E4=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl_header,ie->aggrdecls,
_tmp1E4);_tmp1E4=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Absyn_Tuniondecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls,_tmp1E4);_tmp1E4=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _tagged_arr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl_header,ie->enumdecls,
_tmp1E4);if(_tmp1E4 != 0){_tmp1E4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp1E4);_tmp1E4=Cyc_Interface_add_namespace(_tmp1E4,ns);Cyc_Absynpp_decllist2file(
_tmp1E4,f);}}static void Cyc_Interface_print_ns_xtunionfielddecl_headers(struct Cyc_Std___cycFILE*
f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*
_tmp1E5=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tagged_arr*,
struct Cyc_Tcdecl_Xtunionfielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*d,
struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xtunionfielddecl_header,
ie->xtunionfielddecls,0);if(_tmp1E5 != 0){_tmp1E5=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1E5);_tmp1E5=Cyc_Interface_add_namespace(
_tmp1E5,ns);Cyc_Absynpp_decllist2file(_tmp1E5,f);}}static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Std___cycFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp1E6=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(
struct _tagged_arr*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_typedef,ie->typedefdecls,
0);if(_tmp1E6 != 0){_tmp1E6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp1E6);_tmp1E6=Cyc_Interface_add_namespace(_tmp1E6,ns);Cyc_Absynpp_decllist2file(
_tmp1E6,f);}}static void Cyc_Interface_print_ns_decls(struct Cyc_Std___cycFILE*f,
struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*_tmp1E7=
0;_tmp1E7=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tagged_arr*,
struct Cyc_Absyn_Aggrdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*d,struct Cyc_List_List*
accum))Cyc_Dict_fold)(Cyc_Interface_add_aggrdecl,ie->aggrdecls,_tmp1E7);_tmp1E7=((
struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tagged_arr*,struct Cyc_Absyn_Tuniondecl*,
struct Cyc_List_List*),struct Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold)(
Cyc_Interface_add_tuniondecl,ie->tuniondecls,_tmp1E7);_tmp1E7=((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _tagged_arr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),
struct Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl,
ie->enumdecls,_tmp1E7);_tmp1E7=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(
struct _tagged_arr*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_vardecl,ie->vardecls,
_tmp1E7);_tmp1E7=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Tcdecl_Xtunionfielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls,_tmp1E7);if(_tmp1E7 != 0){_tmp1E7=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1E7);_tmp1E7=Cyc_Interface_add_namespace(
_tmp1E7,ns);Cyc_Absynpp_decllist2file(_tmp1E7,f);}}void Cyc_Interface_print(
struct Cyc_Interface_I*i,struct Cyc_Std___cycFILE*f){Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);({
void*_tmp1E8[0]={};Cyc_Std_fprintf(f,_tag_arr("/****** needed (headers) ******/\n",
sizeof(char),34),_tag_arr(_tmp1E8,sizeof(void*),0));});((void(*)(void(*f)(struct
Cyc_Std___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc_Std___cycFILE*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,f,i->imports);({
void*_tmp1E9[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** provided (headers) ******/\n",
sizeof(char),37),_tag_arr(_tmp1E9,sizeof(void*),0));});((void(*)(void(*f)(struct
Cyc_Std___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc_Std___cycFILE*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,f,i->exports);({
void*_tmp1EA[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** needed (headers of xtunionfielddecls) ******/\n",
sizeof(char),56),_tag_arr(_tmp1EA,sizeof(void*),0));});((void(*)(void(*f)(struct
Cyc_Std___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc_Std___cycFILE*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xtunionfielddecl_headers,
f,i->imports);({void*_tmp1EB[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** provided (headers of xtunionfielddecls) ******/\n",
sizeof(char),58),_tag_arr(_tmp1EB,sizeof(void*),0));});((void(*)(void(*f)(struct
Cyc_Std___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc_Std___cycFILE*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xtunionfielddecl_headers,
f,i->exports);({void*_tmp1EC[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** provided (typedefs) ******/\n",
sizeof(char),38),_tag_arr(_tmp1EC,sizeof(void*),0));});((void(*)(void(*f)(struct
Cyc_Std___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc_Std___cycFILE*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_typedefs,f,i->exports);({
void*_tmp1ED[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** needed (declarations) ******/\n",
sizeof(char),40),_tag_arr(_tmp1ED,sizeof(void*),0));});((void(*)(void(*f)(struct
Cyc_Std___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc_Std___cycFILE*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,f,i->imports);({
void*_tmp1EE[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** provided (declarations) ******/\n",
sizeof(char),42),_tag_arr(_tmp1EE,sizeof(void*),0));});((void(*)(void(*f)(struct
Cyc_Std___cycFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct Cyc_Std___cycFILE*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,f,i->exports);}
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc_Std___cycFILE*f){Cyc_Lex_lex_init();{
struct Cyc_List_List*_tmp1EF=Cyc_Parse_parse_file(f);struct Cyc_Tcenv_Tenv*_tmp1F0=
Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp1F0,0,_tmp1EF);return Cyc_Interface_gen_extract(
_tmp1F0->ae,0);}}extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep;
static struct Cyc_Typerep_ThinPtr_struct Cyc_aggrdecl_p={1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};
void*Cyc_aggrdecl_rep=(void*)& Cyc_aggrdecl_p;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tuniondecl_rep;static struct Cyc_Typerep_ThinPtr_struct Cyc_tuniondecl_p={
1,1,(void*)((void*)& Cyc_struct_Absyn_Tuniondecl_rep)};void*Cyc_tuniondecl_rep=(
void*)& Cyc_tuniondecl_p;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep;
static struct Cyc_Typerep_ThinPtr_struct Cyc_enumdecl_p={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
void*Cyc_enumdecl_rep=(void*)& Cyc_enumdecl_p;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Typedefdecl_rep;static struct Cyc_Typerep_ThinPtr_struct Cyc_typedefdecl_p={
1,1,(void*)((void*)& Cyc_struct_Absyn_Typedefdecl_rep)};void*Cyc_typedefdecl_rep=(
void*)& Cyc_typedefdecl_p;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep;
static struct Cyc_Typerep_ThinPtr_struct Cyc_vardecl_p={1,1,(void*)((void*)& Cyc_struct_Absyn_Vardecl_rep)};
void*Cyc_vardecl_rep=(void*)& Cyc_vardecl_p;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tunionfield_rep;static struct Cyc_Typerep_ThinPtr_struct Cyc_tunionfield_p={
1,1,(void*)((void*)& Cyc_struct_Absyn_Tunionfield_rep)};void*Cyc_tunionfield_rep=(
void*)& Cyc_tunionfield_p;static struct _tuple14 Cyc_Interface_write_aggrdecl_t(
struct _tuple14 env,struct Cyc_Std___cycFILE*fp,struct Cyc_Absyn_Aggrdecl*decl){
return((struct _tuple14(*)(void*rep,struct _tuple14 env,struct Cyc_Std___cycFILE*fp,
struct Cyc_Absyn_Aggrdecl*val))Cyc_Marshal_write_type_base)(Cyc_aggrdecl_rep,env,
fp,decl);}static struct _tuple14 Cyc_Interface_write_tuniondecl_t(struct _tuple14 env,
struct Cyc_Std___cycFILE*fp,struct Cyc_Absyn_Tuniondecl*decl){return((struct
_tuple14(*)(void*rep,struct _tuple14 env,struct Cyc_Std___cycFILE*fp,struct Cyc_Absyn_Tuniondecl*
val))Cyc_Marshal_write_type_base)(Cyc_tuniondecl_rep,env,fp,decl);}static struct
_tuple14 Cyc_Interface_write_enumdecl_t(struct _tuple14 env,struct Cyc_Std___cycFILE*
fp,struct Cyc_Absyn_Enumdecl*decl){return((struct _tuple14(*)(void*rep,struct
_tuple14 env,struct Cyc_Std___cycFILE*fp,struct Cyc_Absyn_Enumdecl*val))Cyc_Marshal_write_type_base)(
Cyc_enumdecl_rep,env,fp,decl);}static struct _tuple14 Cyc_Interface_write_typedefdecl_t(
struct _tuple14 env,struct Cyc_Std___cycFILE*fp,struct Cyc_Absyn_Typedefdecl*decl){
return((struct _tuple14(*)(void*rep,struct _tuple14 env,struct Cyc_Std___cycFILE*fp,
struct Cyc_Absyn_Typedefdecl*val))Cyc_Marshal_write_type_base)(Cyc_typedefdecl_rep,
env,fp,decl);}static struct _tuple14 Cyc_Interface_write_vardecl_t(struct _tuple14
env,struct Cyc_Std___cycFILE*fp,struct Cyc_Absyn_Vardecl*decl){return((struct
_tuple14(*)(void*rep,struct _tuple14 env,struct Cyc_Std___cycFILE*fp,struct Cyc_Absyn_Vardecl*
val))Cyc_Marshal_write_type_base)(Cyc_vardecl_rep,env,fp,decl);}static struct
_tuple14 Cyc_Interface_write_tunionfield_t(struct _tuple14 env,struct Cyc_Std___cycFILE*
fp,struct Cyc_Absyn_Tunionfield*decl){return((struct _tuple14(*)(void*rep,struct
_tuple14 env,struct Cyc_Std___cycFILE*fp,struct Cyc_Absyn_Tunionfield*val))Cyc_Marshal_write_type_base)(
Cyc_tunionfield_rep,env,fp,decl);}static struct _tuple14 Cyc_Interface_write_xtunionfielddecl_t(
struct _tuple14 env,struct Cyc_Std___cycFILE*fp,struct Cyc_Tcdecl_Xtunionfielddecl*
decl){env=Cyc_Interface_write_tuniondecl_t(env,fp,decl->base);env=Cyc_Interface_write_tunionfield_t(
env,fp,decl->field);return env;}void Cyc_Interface_save(struct Cyc_Interface_I*i,
struct Cyc_Std___cycFILE*f){Cyc_Interface_print(i,f);}struct Cyc_Interface_I*Cyc_Interface_load(
struct Cyc_Std___cycFILE*f){return Cyc_Interface_parse(f);}
