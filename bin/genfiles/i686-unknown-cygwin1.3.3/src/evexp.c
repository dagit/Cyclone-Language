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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char
Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*seg;
void*kind;struct _tagged_arr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Std___cycFILE;
struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,
struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern char Cyc_Std_FileCloseError[
19];extern char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*
tag;struct _tagged_arr f1;};int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct
_tagged_arr*);struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};
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
void*);struct _tagged_arr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_Dict;extern
char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct _tuple3{void*f1;
void*f2;};struct _tuple3*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict*
d);struct _tuple3*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict*d);
struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct
Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable);int Cyc_RgnOrder_is_region_resetable(
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
ae;struct Cyc_Core_Opt*le;};void*Cyc_Tcutil_impos(struct _tagged_arr fmt,struct
_tagged_arr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _tagged_arr
fmt,struct _tagged_arr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);void*
Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_typecmp(void*,void*);struct _tuple4{
unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_okay_szofarg(void*t);
struct _tuple5{void*f1;int f2;};static struct _tuple5 Cyc_Evexp_eval_const_exp(struct
Cyc_Absyn_Exp*);static void*Cyc_Evexp_promote_const(void*cn){void*_tmp0=cn;void*
_tmp1;char _tmp2;void*_tmp3;short _tmp4;_LL1: if(_tmp0 <= (void*)1?1:*((int*)_tmp0)
!= 0)goto _LL3;_tmp1=(void*)((struct Cyc_Absyn_Char_c_struct*)_tmp0)->f1;_tmp2=((
struct Cyc_Absyn_Char_c_struct*)_tmp0)->f2;_LL2: return(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=({struct Cyc_Absyn_Int_c_struct _tmp6;
_tmp6.tag=2;_tmp6.f1=(void*)_tmp1;_tmp6.f2=(int)_tmp2;_tmp6;});_tmp5;});_LL3: if(
_tmp0 <= (void*)1?1:*((int*)_tmp0)!= 1)goto _LL5;_tmp3=(void*)((struct Cyc_Absyn_Short_c_struct*)
_tmp0)->f1;_tmp4=((struct Cyc_Absyn_Short_c_struct*)_tmp0)->f2;_LL4: return(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=({struct
Cyc_Absyn_Int_c_struct _tmp8;_tmp8.tag=2;_tmp8.f1=(void*)_tmp3;_tmp8.f2=(int)
_tmp4;_tmp8;});_tmp7;});_LL5:;_LL6: return cn;_LL0:;}struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e){void*_tmpA;int _tmpB;struct _tuple5 _tmp9=Cyc_Evexp_eval_const_exp(
e);_tmpA=_tmp9.f1;_tmpB=_tmp9.f2;if(!_tmpB)return({struct _tuple4 _tmpC;_tmpC.f1=0;
_tmpC.f2=0;_tmpC;});{void*_tmpD=Cyc_Evexp_promote_const(_tmpA);int _tmpE;_LL8: if(
_tmpD <= (void*)1?1:*((int*)_tmpD)!= 2)goto _LLA;_tmpE=((struct Cyc_Absyn_Int_c_struct*)
_tmpD)->f2;_LL9: return({struct _tuple4 _tmpF;_tmpF.f1=(unsigned int)_tmpE;_tmpF.f2=
1;_tmpF;});_LLA:;_LLB:({void*_tmp10[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("expecting unsigned int",
sizeof(char),23),_tag_arr(_tmp10,sizeof(void*),0));});return({struct _tuple4
_tmp11;_tmp11.f1=0;_tmp11.f2=1;_tmp11;});_LL7:;}}struct _tuple6{int f1;int f2;};
static struct _tuple6 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){void*
_tmp13;int _tmp14;struct _tuple5 _tmp12=Cyc_Evexp_eval_const_exp(e);_tmp13=_tmp12.f1;
_tmp14=_tmp12.f2;if(!_tmp14)return({struct _tuple6 _tmp15;_tmp15.f1=0;_tmp15.f2=0;
_tmp15;});{void*_tmp16=Cyc_Evexp_promote_const(_tmp13);int _tmp17;long long _tmp18;
_LLD: if(_tmp16 <= (void*)1?1:*((int*)_tmp16)!= 2)goto _LLF;_tmp17=((struct Cyc_Absyn_Int_c_struct*)
_tmp16)->f2;_LLE: return({struct _tuple6 _tmp19;_tmp19.f1=_tmp17 != 0;_tmp19.f2=1;
_tmp19;});_LLF: if(_tmp16 <= (void*)1?1:*((int*)_tmp16)!= 3)goto _LL11;_tmp18=((
struct Cyc_Absyn_LongLong_c_struct*)_tmp16)->f2;_LL10: return({struct _tuple6 _tmp1A;
_tmp1A.f1=_tmp18 != 0;_tmp1A.f2=1;_tmp1A;});_LL11: if((int)_tmp16 != 0)goto _LL13;
_LL12: return({struct _tuple6 _tmp1B;_tmp1B.f1=0;_tmp1B.f2=0;_tmp1B;});_LL13:;_LL14:({
void*_tmp1C[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("expecting bool",sizeof(char),
15),_tag_arr(_tmp1C,sizeof(void*),0));});return({struct _tuple6 _tmp1D;_tmp1D.f1=0;
_tmp1D.f2=0;_tmp1D;});_LLC:;}}static struct _tuple5 Cyc_Evexp_eval_const_unprimop(
void*p,struct Cyc_Absyn_Exp*e){void*_tmp1F;int _tmp20;struct _tuple5 _tmp1E=Cyc_Evexp_eval_const_exp(
e);_tmp1F=_tmp1E.f1;_tmp20=_tmp1E.f2;if(!_tmp20)return({struct _tuple5 _tmp21;
_tmp21.f1=_tmp1F;_tmp21.f2=0;_tmp21;});{struct _tuple3 _tmp23=({struct _tuple3
_tmp22;_tmp22.f1=p;_tmp22.f2=_tmp1F;_tmp22;});void*_tmp24;void*_tmp25;void*
_tmp26;void*_tmp27;int _tmp28;void*_tmp29;void*_tmp2A;void*_tmp2B;int _tmp2C;void*
_tmp2D;void*_tmp2E;int _tmp2F;void*_tmp30;void*_tmp31;_LL16: _tmp24=_tmp23.f1;if((
int)_tmp24 != 0)goto _LL18;_LL17: goto _LL15;_LL18: _tmp25=_tmp23.f1;if((int)_tmp25 != 
2)goto _LL1A;_tmp26=_tmp23.f2;if(_tmp26 <= (void*)1?1:*((int*)_tmp26)!= 2)goto
_LL1A;_tmp27=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp26)->f1;_tmp28=((struct
Cyc_Absyn_Int_c_struct*)_tmp26)->f2;_LL19: _tmp1F=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32[0]=({struct Cyc_Absyn_Int_c_struct _tmp33;
_tmp33.tag=2;_tmp33.f1=(void*)((void*)0);_tmp33.f2=- _tmp28;_tmp33;});_tmp32;});
goto _LL15;_LL1A: _tmp29=_tmp23.f1;if((int)_tmp29 != 12)goto _LL1C;_tmp2A=_tmp23.f2;
if(_tmp2A <= (void*)1?1:*((int*)_tmp2A)!= 2)goto _LL1C;_tmp2B=(void*)((struct Cyc_Absyn_Int_c_struct*)
_tmp2A)->f1;_tmp2C=((struct Cyc_Absyn_Int_c_struct*)_tmp2A)->f2;_LL1B: _tmp1F=(
void*)({struct Cyc_Absyn_Int_c_struct*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34[0]=({
struct Cyc_Absyn_Int_c_struct _tmp35;_tmp35.tag=2;_tmp35.f1=(void*)((void*)1);
_tmp35.f2=~ _tmp2C;_tmp35;});_tmp34;});goto _LL15;_LL1C: _tmp2D=_tmp23.f1;if((int)
_tmp2D != 11)goto _LL1E;_tmp2E=_tmp23.f2;if(_tmp2E <= (void*)1?1:*((int*)_tmp2E)!= 
2)goto _LL1E;_tmp2F=((struct Cyc_Absyn_Int_c_struct*)_tmp2E)->f2;_LL1D: _tmp1F=(
void*)({struct Cyc_Absyn_Int_c_struct*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=({
struct Cyc_Absyn_Int_c_struct _tmp37;_tmp37.tag=2;_tmp37.f1=(void*)((void*)0);
_tmp37.f2=_tmp2F == 0?1: 0;_tmp37;});_tmp36;});goto _LL15;_LL1E: _tmp30=_tmp23.f1;
if((int)_tmp30 != 11)goto _LL20;_tmp31=_tmp23.f2;if((int)_tmp31 != 0)goto _LL20;
_LL1F: _tmp1F=(void*)({struct Cyc_Absyn_Int_c_struct*_tmp38=_cycalloc(sizeof(*
_tmp38));_tmp38[0]=({struct Cyc_Absyn_Int_c_struct _tmp39;_tmp39.tag=2;_tmp39.f1=(
void*)((void*)0);_tmp39.f2=1;_tmp39;});_tmp38;});goto _LL15;_LL20:;_LL21:({void*
_tmp3A[0]={};Cyc_Tcutil_terr(e->loc,_tag_arr("bad constant expression",sizeof(
char),24),_tag_arr(_tmp3A,sizeof(void*),0));});goto _LL15;_LL15:;}return({struct
_tuple5 _tmp3B;_tmp3B.f1=_tmp1F;_tmp3B.f2=1;_tmp3B;});}static struct _tuple5 Cyc_Evexp_eval_const_binprimop(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){void*_tmp3D;int _tmp3E;
struct _tuple5 _tmp3C=Cyc_Evexp_eval_const_exp(e1);_tmp3D=_tmp3C.f1;_tmp3E=_tmp3C.f2;{
void*_tmp40;int _tmp41;struct _tuple5 _tmp3F=Cyc_Evexp_eval_const_exp(e2);_tmp40=
_tmp3F.f1;_tmp41=_tmp3F.f2;if(!_tmp3E?1: !_tmp41)return({struct _tuple5 _tmp42;
_tmp42.f1=_tmp3D;_tmp42.f2=0;_tmp42;});_tmp3D=Cyc_Evexp_promote_const(_tmp3D);
_tmp40=Cyc_Evexp_promote_const(_tmp40);{void*s1;void*s2;int i1;int i2;{void*_tmp43=
_tmp3D;void*_tmp44;int _tmp45;_LL23: if(_tmp43 <= (void*)1?1:*((int*)_tmp43)!= 2)
goto _LL25;_tmp44=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp43)->f1;_tmp45=((
struct Cyc_Absyn_Int_c_struct*)_tmp43)->f2;_LL24: s1=_tmp44;i1=_tmp45;goto _LL22;
_LL25:;_LL26:({void*_tmp46[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("bad constant expression",
sizeof(char),24),_tag_arr(_tmp46,sizeof(void*),0));});return({struct _tuple5
_tmp47;_tmp47.f1=_tmp3D;_tmp47.f2=1;_tmp47;});_LL22:;}{void*_tmp48=_tmp40;void*
_tmp49;int _tmp4A;_LL28: if(_tmp48 <= (void*)1?1:*((int*)_tmp48)!= 2)goto _LL2A;
_tmp49=(void*)((struct Cyc_Absyn_Int_c_struct*)_tmp48)->f1;_tmp4A=((struct Cyc_Absyn_Int_c_struct*)
_tmp48)->f2;_LL29: s2=_tmp49;i2=_tmp4A;goto _LL27;_LL2A:;_LL2B:({void*_tmp4B[0]={};
Cyc_Tcutil_terr(e2->loc,_tag_arr("bad constant expression",sizeof(char),24),
_tag_arr(_tmp4B,sizeof(void*),0));});return({struct _tuple5 _tmp4C;_tmp4C.f1=
_tmp3D;_tmp4C.f2=1;_tmp4C;});_LL27:;}{void*_tmp4D=p;_LL2D: if((int)_tmp4D != 3)
goto _LL2F;_LL2E: goto _LL30;_LL2F: if((int)_tmp4D != 4)goto _LL31;_LL30: if(i2 == 0){({
void*_tmp4E[0]={};Cyc_Tcutil_terr(e2->loc,_tag_arr("division by zero in constant expression",
sizeof(char),40),_tag_arr(_tmp4E,sizeof(void*),0));});return({struct _tuple5
_tmp4F;_tmp4F.f1=_tmp3D;_tmp4F.f2=1;_tmp4F;});}goto _LL2C;_LL31:;_LL32: goto _LL2C;
_LL2C:;}{int has_u_arg=s1 == (void*)1?1: s2 == (void*)1;unsigned int u1=(unsigned int)
i1;unsigned int u2=(unsigned int)i2;int i3=0;unsigned int u3=0;int b3=1;int use_i3=0;
int use_u3=0;int use_b3=0;{struct _tuple5 _tmp51=({struct _tuple5 _tmp50;_tmp50.f1=p;
_tmp50.f2=has_u_arg;_tmp50;});void*_tmp52;int _tmp53;void*_tmp54;int _tmp55;void*
_tmp56;int _tmp57;void*_tmp58;int _tmp59;void*_tmp5A;int _tmp5B;void*_tmp5C;int
_tmp5D;void*_tmp5E;int _tmp5F;void*_tmp60;int _tmp61;void*_tmp62;int _tmp63;void*
_tmp64;int _tmp65;void*_tmp66;void*_tmp67;void*_tmp68;int _tmp69;void*_tmp6A;int
_tmp6B;void*_tmp6C;int _tmp6D;void*_tmp6E;int _tmp6F;void*_tmp70;int _tmp71;void*
_tmp72;int _tmp73;void*_tmp74;int _tmp75;void*_tmp76;int _tmp77;void*_tmp78;void*
_tmp79;void*_tmp7A;void*_tmp7B;void*_tmp7C;void*_tmp7D;_LL34: _tmp52=_tmp51.f1;
if((int)_tmp52 != 0)goto _LL36;_tmp53=_tmp51.f2;if(_tmp53 != 0)goto _LL36;_LL35: i3=
i1 + i2;use_i3=1;goto _LL33;_LL36: _tmp54=_tmp51.f1;if((int)_tmp54 != 1)goto _LL38;
_tmp55=_tmp51.f2;if(_tmp55 != 0)goto _LL38;_LL37: i3=i1 * i2;use_i3=1;goto _LL33;
_LL38: _tmp56=_tmp51.f1;if((int)_tmp56 != 2)goto _LL3A;_tmp57=_tmp51.f2;if(_tmp57 != 
0)goto _LL3A;_LL39: i3=i1 - i2;use_i3=1;goto _LL33;_LL3A: _tmp58=_tmp51.f1;if((int)
_tmp58 != 3)goto _LL3C;_tmp59=_tmp51.f2;if(_tmp59 != 0)goto _LL3C;_LL3B: i3=i1 / i2;
use_i3=1;goto _LL33;_LL3C: _tmp5A=_tmp51.f1;if((int)_tmp5A != 4)goto _LL3E;_tmp5B=
_tmp51.f2;if(_tmp5B != 0)goto _LL3E;_LL3D: i3=i1 % i2;use_i3=1;goto _LL33;_LL3E:
_tmp5C=_tmp51.f1;if((int)_tmp5C != 0)goto _LL40;_tmp5D=_tmp51.f2;if(_tmp5D != 1)
goto _LL40;_LL3F: u3=u1 + u2;use_u3=1;goto _LL33;_LL40: _tmp5E=_tmp51.f1;if((int)
_tmp5E != 1)goto _LL42;_tmp5F=_tmp51.f2;if(_tmp5F != 1)goto _LL42;_LL41: u3=u1 * u2;
use_u3=1;goto _LL33;_LL42: _tmp60=_tmp51.f1;if((int)_tmp60 != 2)goto _LL44;_tmp61=
_tmp51.f2;if(_tmp61 != 1)goto _LL44;_LL43: u3=u1 - u2;use_u3=1;goto _LL33;_LL44:
_tmp62=_tmp51.f1;if((int)_tmp62 != 3)goto _LL46;_tmp63=_tmp51.f2;if(_tmp63 != 1)
goto _LL46;_LL45: u3=u1 / u2;use_u3=1;goto _LL33;_LL46: _tmp64=_tmp51.f1;if((int)
_tmp64 != 4)goto _LL48;_tmp65=_tmp51.f2;if(_tmp65 != 1)goto _LL48;_LL47: u3=u1 % u2;
use_u3=1;goto _LL33;_LL48: _tmp66=_tmp51.f1;if((int)_tmp66 != 5)goto _LL4A;_LL49: b3=
i1 == i2;use_b3=1;goto _LL33;_LL4A: _tmp67=_tmp51.f1;if((int)_tmp67 != 6)goto _LL4C;
_LL4B: b3=i1 != i2;use_b3=1;goto _LL33;_LL4C: _tmp68=_tmp51.f1;if((int)_tmp68 != 7)
goto _LL4E;_tmp69=_tmp51.f2;if(_tmp69 != 0)goto _LL4E;_LL4D: b3=i1 > i2;use_b3=1;goto
_LL33;_LL4E: _tmp6A=_tmp51.f1;if((int)_tmp6A != 8)goto _LL50;_tmp6B=_tmp51.f2;if(
_tmp6B != 0)goto _LL50;_LL4F: b3=i1 < i2;use_b3=1;goto _LL33;_LL50: _tmp6C=_tmp51.f1;
if((int)_tmp6C != 9)goto _LL52;_tmp6D=_tmp51.f2;if(_tmp6D != 0)goto _LL52;_LL51: b3=
i1 >= i2;use_b3=1;goto _LL33;_LL52: _tmp6E=_tmp51.f1;if((int)_tmp6E != 10)goto _LL54;
_tmp6F=_tmp51.f2;if(_tmp6F != 0)goto _LL54;_LL53: b3=i1 <= i2;use_b3=1;goto _LL33;
_LL54: _tmp70=_tmp51.f1;if((int)_tmp70 != 7)goto _LL56;_tmp71=_tmp51.f2;if(_tmp71 != 
1)goto _LL56;_LL55: b3=u1 > u2;use_b3=1;goto _LL33;_LL56: _tmp72=_tmp51.f1;if((int)
_tmp72 != 8)goto _LL58;_tmp73=_tmp51.f2;if(_tmp73 != 1)goto _LL58;_LL57: b3=u1 < u2;
use_b3=1;goto _LL33;_LL58: _tmp74=_tmp51.f1;if((int)_tmp74 != 9)goto _LL5A;_tmp75=
_tmp51.f2;if(_tmp75 != 1)goto _LL5A;_LL59: b3=u1 >= u2;use_b3=1;goto _LL33;_LL5A:
_tmp76=_tmp51.f1;if((int)_tmp76 != 10)goto _LL5C;_tmp77=_tmp51.f2;if(_tmp77 != 1)
goto _LL5C;_LL5B: b3=u1 <= u2;use_b3=1;goto _LL33;_LL5C: _tmp78=_tmp51.f1;if((int)
_tmp78 != 13)goto _LL5E;_LL5D: u3=u1 & u2;use_u3=1;goto _LL33;_LL5E: _tmp79=_tmp51.f1;
if((int)_tmp79 != 14)goto _LL60;_LL5F: u3=u1 | u2;use_u3=1;goto _LL33;_LL60: _tmp7A=
_tmp51.f1;if((int)_tmp7A != 15)goto _LL62;_LL61: u3=u1 ^ u2;use_u3=1;goto _LL33;_LL62:
_tmp7B=_tmp51.f1;if((int)_tmp7B != 16)goto _LL64;_LL63: u3=u1 << u2;use_u3=1;goto
_LL33;_LL64: _tmp7C=_tmp51.f1;if((int)_tmp7C != 17)goto _LL66;_LL65: u3=u1 >> u2;
use_u3=1;goto _LL33;_LL66: _tmp7D=_tmp51.f1;if((int)_tmp7D != 18)goto _LL68;_LL67:({
void*_tmp7E[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr(">>> NOT IMPLEMENTED",sizeof(
char),20),_tag_arr(_tmp7E,sizeof(void*),0));});goto _LL69;_LL68:;_LL69:({void*
_tmp7F[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("bad constant expression",sizeof(
char),24),_tag_arr(_tmp7F,sizeof(void*),0));});return({struct _tuple5 _tmp80;
_tmp80.f1=_tmp3D;_tmp80.f2=1;_tmp80;});_LL33:;}if(use_i3)return({struct _tuple5
_tmp81;_tmp81.f1=(void*)({struct Cyc_Absyn_Int_c_struct*_tmp82=_cycalloc(sizeof(*
_tmp82));_tmp82[0]=({struct Cyc_Absyn_Int_c_struct _tmp83;_tmp83.tag=2;_tmp83.f1=(
void*)((void*)0);_tmp83.f2=i3;_tmp83;});_tmp82;});_tmp81.f2=1;_tmp81;});if(
use_u3)return({struct _tuple5 _tmp84;_tmp84.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=({struct Cyc_Absyn_Int_c_struct _tmp86;
_tmp86.tag=2;_tmp86.f1=(void*)((void*)1);_tmp86.f2=(int)u3;_tmp86;});_tmp85;});
_tmp84.f2=1;_tmp84;});if(use_b3)return({struct _tuple5 _tmp87;_tmp87.f1=(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88[0]=({
struct Cyc_Absyn_Int_c_struct _tmp89;_tmp89.tag=2;_tmp89.f1=(void*)((void*)0);
_tmp89.f2=b3?1: 0;_tmp89;});_tmp88;});_tmp87.f2=1;_tmp87;});({void*_tmp8A[0]={};((
int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Evexp::eval_const_binop",
sizeof(char),24),_tag_arr(_tmp8A,sizeof(void*),0));});}}}}static struct _tuple5 Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp*e){struct _tuple5 ans;{void*_tmp8B=(void*)e->r;void*_tmp8C;
struct Cyc_Absyn_Exp*_tmp8D;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Absyn_Exp*_tmp8F;
void*_tmp90;struct Cyc_List_List*_tmp91;void*_tmp92;struct Cyc_Absyn_Exp*_tmp93;
struct Cyc_Absyn_Enumfield*_tmp94;struct Cyc_Absyn_Enumfield*_tmp95;_LL6B: if(*((
int*)_tmp8B)!= 0)goto _LL6D;_tmp8C=(void*)((struct Cyc_Absyn_Const_e_struct*)
_tmp8B)->f1;_LL6C: return({struct _tuple5 _tmp96;_tmp96.f1=_tmp8C;_tmp96.f2=1;
_tmp96;});_LL6D: if(*((int*)_tmp8B)!= 6)goto _LL6F;_tmp8D=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp8B)->f1;_tmp8E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp8B)->f2;_tmp8F=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp8B)->f3;_LL6E: {int _tmp98;int _tmp99;
struct _tuple6 _tmp97=Cyc_Evexp_eval_const_bool_exp(_tmp8D);_tmp98=_tmp97.f1;
_tmp99=_tmp97.f2;if(!_tmp99){Cyc_Evexp_eval_const_exp(_tmp8E);Cyc_Evexp_eval_const_exp(
_tmp8F);return({struct _tuple5 _tmp9A;_tmp9A.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({struct Cyc_Absyn_Int_c_struct _tmp9C;
_tmp9C.tag=2;_tmp9C.f1=(void*)((void*)0);_tmp9C.f2=0;_tmp9C;});_tmp9B;});_tmp9A.f2=
0;_tmp9A;});}if(_tmp98)ans=Cyc_Evexp_eval_const_exp(_tmp8E);else{ans=Cyc_Evexp_eval_const_exp(
_tmp8F);}goto _LL6A;}_LL6F: if(*((int*)_tmp8B)!= 3)goto _LL71;_tmp90=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp8B)->f1;_tmp91=((struct Cyc_Absyn_Primop_e_struct*)
_tmp8B)->f2;_LL70: if(_tmp91 == 0){({void*_tmp9D[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("bad static expression (no args to primop)",sizeof(char),42),_tag_arr(
_tmp9D,sizeof(void*),0));});return({struct _tuple5 _tmp9E;_tmp9E.f1=(void*)({
struct Cyc_Absyn_Int_c_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({
struct Cyc_Absyn_Int_c_struct _tmpA0;_tmpA0.tag=2;_tmpA0.f1=(void*)((void*)0);
_tmpA0.f2=0;_tmpA0;});_tmp9F;});_tmp9E.f2=1;_tmp9E;});}if(_tmp91->tl == 0){ans=
Cyc_Evexp_eval_const_unprimop(_tmp90,(struct Cyc_Absyn_Exp*)_tmp91->hd);goto _LL6A;}
if(((struct Cyc_List_List*)_check_null(_tmp91->tl))->tl == 0){ans=Cyc_Evexp_eval_const_binprimop(
_tmp90,(struct Cyc_Absyn_Exp*)_tmp91->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp91->tl))->hd);goto _LL6A;}({void*_tmpA1[0]={};Cyc_Tcutil_terr(e->loc,
_tag_arr("bad static expression (too many args to primop)",sizeof(char),48),
_tag_arr(_tmpA1,sizeof(void*),0));});return({struct _tuple5 _tmpA2;_tmpA2.f1=(void*)({
struct Cyc_Absyn_Int_c_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({
struct Cyc_Absyn_Int_c_struct _tmpA4;_tmpA4.tag=2;_tmpA4.f1=(void*)((void*)0);
_tmpA4.f2=0;_tmpA4;});_tmpA3;});_tmpA2.f2=1;_tmpA2;});_LL71: if(*((int*)_tmp8B)!= 
16)goto _LL73;_LL72: goto _LL74;_LL73: if(*((int*)_tmp8B)!= 17)goto _LL75;_LL74: goto
_LL76;_LL75: if(*((int*)_tmp8B)!= 18)goto _LL77;_LL76: ans=({struct _tuple5 _tmpA5;
_tmpA5.f1=(void*)({struct Cyc_Absyn_Int_c_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));
_tmpA6[0]=({struct Cyc_Absyn_Int_c_struct _tmpA7;_tmpA7.tag=2;_tmpA7.f1=(void*)((
void*)0);_tmpA7.f2=0;_tmpA7;});_tmpA6;});_tmpA5.f2=0;_tmpA5;});goto _LL6A;_LL77:
if(*((int*)_tmp8B)!= 13)goto _LL79;_tmp92=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp8B)->f1;_tmp93=((struct Cyc_Absyn_Cast_e_struct*)_tmp8B)->f2;_LL78: ans=Cyc_Evexp_eval_const_exp(
_tmp93);if(ans.f2){struct _tuple3 _tmpA9=({struct _tuple3 _tmpA8;_tmpA8.f1=Cyc_Tcutil_compress(
_tmp92);_tmpA8.f2=ans.f1;_tmpA8;});void*_tmpAA;void*_tmpAB;void*_tmpAC;void*
_tmpAD;void*_tmpAE;char _tmpAF;void*_tmpB0;void*_tmpB1;void*_tmpB2;void*_tmpB3;
void*_tmpB4;short _tmpB5;void*_tmpB6;void*_tmpB7;void*_tmpB8;void*_tmpB9;void*
_tmpBA;int _tmpBB;_LL80: _tmpAA=_tmpA9.f1;if(_tmpAA <= (void*)3?1:*((int*)_tmpAA)!= 
5)goto _LL82;_tmpAB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpAA)->f1;_tmpAC=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpAA)->f2;_tmpAD=_tmpA9.f2;if(_tmpAD <= (
void*)1?1:*((int*)_tmpAD)!= 0)goto _LL82;_tmpAE=(void*)((struct Cyc_Absyn_Char_c_struct*)
_tmpAD)->f1;_tmpAF=((struct Cyc_Absyn_Char_c_struct*)_tmpAD)->f2;_LL81: _tmpB1=
_tmpAB;_tmpB2=_tmpAC;_tmpB4=_tmpAE;_tmpB5=(short)_tmpAF;goto _LL83;_LL82: _tmpB0=
_tmpA9.f1;if(_tmpB0 <= (void*)3?1:*((int*)_tmpB0)!= 5)goto _LL84;_tmpB1=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpB0)->f1;_tmpB2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpB0)->f2;_tmpB3=_tmpA9.f2;if(_tmpB3 <= (void*)1?1:*((int*)_tmpB3)!= 1)goto
_LL84;_tmpB4=(void*)((struct Cyc_Absyn_Short_c_struct*)_tmpB3)->f1;_tmpB5=((
struct Cyc_Absyn_Short_c_struct*)_tmpB3)->f2;_LL83: _tmpB7=_tmpB1;_tmpB8=_tmpB2;
_tmpBA=_tmpB4;_tmpBB=(int)_tmpB5;goto _LL85;_LL84: _tmpB6=_tmpA9.f1;if(_tmpB6 <= (
void*)3?1:*((int*)_tmpB6)!= 5)goto _LL86;_tmpB7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpB6)->f1;_tmpB8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB6)->f2;_tmpB9=
_tmpA9.f2;if(_tmpB9 <= (void*)1?1:*((int*)_tmpB9)!= 2)goto _LL86;_tmpBA=(void*)((
struct Cyc_Absyn_Int_c_struct*)_tmpB9)->f1;_tmpBB=((struct Cyc_Absyn_Int_c_struct*)
_tmpB9)->f2;_LL85: if(_tmpB7 != _tmpBA)ans.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_Int_c_struct _tmpBD;
_tmpBD.tag=2;_tmpBD.f1=(void*)_tmpB7;_tmpBD.f2=_tmpBB;_tmpBD;});_tmpBC;});goto
_LL7F;_LL86:;_LL87:({struct Cyc_Std_String_pa_struct _tmpBF;_tmpBF.tag=0;_tmpBF.f1=(
struct _tagged_arr)Cyc_Absynpp_typ2string(_tmp92);{void*_tmpBE[1]={& _tmpBF};Cyc_Tcutil_terr(
e->loc,_tag_arr("cannot cast to %s",sizeof(char),18),_tag_arr(_tmpBE,sizeof(void*),
1));}});goto _LL7F;_LL7F:;}goto _LL6A;_LL79: if(*((int*)_tmp8B)!= 32)goto _LL7B;
_tmp94=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp8B)->f3;_LL7A: _tmp95=_tmp94;goto
_LL7C;_LL7B: if(*((int*)_tmp8B)!= 31)goto _LL7D;_tmp95=((struct Cyc_Absyn_Enum_e_struct*)
_tmp8B)->f3;_LL7C: ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp95))->tag));goto _LL6A;_LL7D:;_LL7E:({
struct Cyc_Std_String_pa_struct _tmpC1;_tmpC1.tag=0;_tmpC1.f1=(struct _tagged_arr)
Cyc_Absynpp_exp2string(e);{void*_tmpC0[1]={& _tmpC1};Cyc_Tcutil_terr(e->loc,
_tag_arr("bad static expression %s",sizeof(char),25),_tag_arr(_tmpC0,sizeof(void*),
1));}});return({struct _tuple5 _tmpC2;_tmpC2.f1=(void*)({struct Cyc_Absyn_Int_c_struct*
_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Absyn_Int_c_struct _tmpC4;
_tmpC4.tag=2;_tmpC4.f1=(void*)((void*)0);_tmpC4.f2=0;_tmpC4;});_tmpC3;});_tmpC2.f2=
1;_tmpC2;});_LL6A:;}if(ans.f2)(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Const_e_struct*
_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=({struct Cyc_Absyn_Const_e_struct
_tmpC6;_tmpC6.tag=0;_tmpC6.f1=(void*)ans.f1;_tmpC6;});_tmpC5;})));return ans;}
static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*e){void*_tmpC7=(
void*)e->r;_LL89: if(*((int*)_tmpC7)!= 0)goto _LL8B;_LL8A: return 1;_LL8B: if(*((int*)
_tmpC7)!= 6)goto _LL8D;_LL8C: return 2;_LL8D: if(*((int*)_tmpC7)!= 3)goto _LL8F;_LL8E:
return 3;_LL8F: if(*((int*)_tmpC7)!= 16)goto _LL91;_LL90: goto _LL92;_LL91: if(*((int*)
_tmpC7)!= 17)goto _LL93;_LL92: return 4;_LL93: if(*((int*)_tmpC7)!= 18)goto _LL95;
_LL94: return 5;_LL95: if(*((int*)_tmpC7)!= 13)goto _LL97;_LL96: return 6;_LL97:;_LL98:({
struct Cyc_Std_String_pa_struct _tmpC9;_tmpC9.tag=0;_tmpC9.f1=(struct _tagged_arr)
Cyc_Absynpp_exp2string(e);{void*_tmpC8[1]={& _tmpC9};Cyc_Tcutil_terr(e->loc,
_tag_arr("bad static expression %s",sizeof(char),25),_tag_arr(_tmpC8,sizeof(void*),
1));}});return 0;_LL88:;}int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){unsigned int _tmpCB;int _tmpCC;struct _tuple4 _tmpCA=Cyc_Evexp_eval_const_uint_exp(
e1);_tmpCB=_tmpCA.f1;_tmpCC=_tmpCA.f2;{unsigned int _tmpCE;int _tmpCF;struct
_tuple4 _tmpCD=Cyc_Evexp_eval_const_uint_exp(e2);_tmpCE=_tmpCD.f1;_tmpCF=_tmpCD.f2;
if(_tmpCC?_tmpCF: 0)return(int)(_tmpCB - _tmpCE);{int _tmpD0=Cyc_Evexp_const_exp_case_number(
e1);int _tmpD1=Cyc_Evexp_const_exp_case_number(e2);if(_tmpD0 != _tmpD1)return
_tmpD0 - _tmpD1;{struct _tuple3 _tmpD3=({struct _tuple3 _tmpD2;_tmpD2.f1=(void*)e1->r;
_tmpD2.f2=(void*)e2->r;_tmpD2;});void*_tmpD4;struct Cyc_Absyn_Exp*_tmpD5;struct
Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Exp*_tmpD7;void*_tmpD8;struct Cyc_Absyn_Exp*
_tmpD9;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpDB;void*_tmpDC;void*
_tmpDD;struct Cyc_List_List*_tmpDE;void*_tmpDF;void*_tmpE0;struct Cyc_List_List*
_tmpE1;void*_tmpE2;void*_tmpE3;void*_tmpE4;void*_tmpE5;void*_tmpE6;void*_tmpE7;
void*_tmpE8;struct Cyc_Absyn_Exp*_tmpE9;void*_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;
void*_tmpEC;void*_tmpED;void*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;void*_tmpF0;
struct Cyc_Absyn_Exp*_tmpF1;void*_tmpF2;void*_tmpF3;void*_tmpF4;void*_tmpF5;void*
_tmpF6;void*_tmpF7;void*_tmpF8;void*_tmpF9;struct Cyc_Absyn_Exp*_tmpFA;void*
_tmpFB;void*_tmpFC;struct Cyc_Absyn_Exp*_tmpFD;_LL9A: _tmpD4=_tmpD3.f1;if(*((int*)
_tmpD4)!= 6)goto _LL9C;_tmpD5=((struct Cyc_Absyn_Conditional_e_struct*)_tmpD4)->f1;
_tmpD6=((struct Cyc_Absyn_Conditional_e_struct*)_tmpD4)->f2;_tmpD7=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpD4)->f3;_tmpD8=_tmpD3.f2;if(*((int*)_tmpD8)!= 6)goto _LL9C;_tmpD9=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_Conditional_e_struct*)_tmpD8)->f2;_tmpDB=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpD8)->f3;_LL9B: {int _tmpFE=Cyc_Evexp_const_exp_cmp(
_tmpD5,_tmpD9);if(_tmpFE != 0)return _tmpFE;{int _tmpFF=Cyc_Evexp_const_exp_cmp(
_tmpD6,_tmpDA);if(_tmpFF != 0)return _tmpFF;return Cyc_Evexp_const_exp_cmp(_tmpD7,
_tmpDB);}}_LL9C: _tmpDC=_tmpD3.f1;if(*((int*)_tmpDC)!= 3)goto _LL9E;_tmpDD=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpDC)->f1;_tmpDE=((struct Cyc_Absyn_Primop_e_struct*)
_tmpDC)->f2;_tmpDF=_tmpD3.f2;if(*((int*)_tmpDF)!= 3)goto _LL9E;_tmpE0=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpDF)->f1;_tmpE1=((struct Cyc_Absyn_Primop_e_struct*)
_tmpDF)->f2;_LL9D: {int _tmp100=(int)_tmpDD - (int)_tmpE0;if(_tmp100 != 0)return
_tmp100;for(0;_tmpDE != 0?_tmpE1 != 0: 0;(_tmpDE=_tmpDE->tl,_tmpE1=_tmpE1->tl)){int
_tmp101=Cyc_Evexp_const_exp_cmp((struct Cyc_Absyn_Exp*)_tmpDE->hd,(struct Cyc_Absyn_Exp*)
_tmpE1->hd);if(_tmp101 != 0)return _tmp101;}return 0;}_LL9E: _tmpE2=_tmpD3.f1;if(*((
int*)_tmpE2)!= 16)goto _LLA0;_tmpE3=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpE2)->f1;_tmpE4=_tmpD3.f2;if(*((int*)_tmpE4)!= 16)goto _LLA0;_tmpE5=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpE4)->f1;_LL9F: return Cyc_Tcutil_typecmp(
_tmpE3,_tmpE5);_LLA0: _tmpE6=_tmpD3.f1;if(*((int*)_tmpE6)!= 16)goto _LLA2;_tmpE7=(
void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpE6)->f1;_tmpE8=_tmpD3.f2;if(*((
int*)_tmpE8)!= 17)goto _LLA2;_tmpE9=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpE8)->f1;
_LLA1: if(_tmpE9->topt == 0)({void*_tmp102[0]={};Cyc_Tcutil_terr(e2->loc,_tag_arr("cannot handle sizeof(exp) here -- use sizeof(type)",
sizeof(char),51),_tag_arr(_tmp102,sizeof(void*),0));});return Cyc_Tcutil_typecmp(
_tmpE7,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpE9->topt))->v);_LLA2: _tmpEA=
_tmpD3.f1;if(*((int*)_tmpEA)!= 17)goto _LLA4;_tmpEB=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpEA)->f1;_tmpEC=_tmpD3.f2;if(*((int*)_tmpEC)!= 16)goto _LLA4;_tmpED=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpEC)->f1;_LLA3: if(_tmpEB->topt == 0)({void*
_tmp103[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("cannot handle sizeof(exp) here -- use sizeof(type)",
sizeof(char),51),_tag_arr(_tmp103,sizeof(void*),0));});return Cyc_Tcutil_typecmp((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEB->topt))->v,_tmpED);_LLA4: _tmpEE=
_tmpD3.f1;if(*((int*)_tmpEE)!= 17)goto _LLA6;_tmpEF=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpEE)->f1;_tmpF0=_tmpD3.f2;if(*((int*)_tmpF0)!= 17)goto _LLA6;_tmpF1=((struct
Cyc_Absyn_Sizeofexp_e_struct*)_tmpF0)->f1;_LLA5: if(_tmpEF->topt == 0)({void*
_tmp104[0]={};Cyc_Tcutil_terr(e1->loc,_tag_arr("cannot handle sizeof(exp) here -- use sizeof(type)",
sizeof(char),51),_tag_arr(_tmp104,sizeof(void*),0));});if(_tmpF1->topt == 0)({
void*_tmp105[0]={};Cyc_Tcutil_terr(e2->loc,_tag_arr("cannot handle sizeof(exp) here -- use sizeof(type)",
sizeof(char),51),_tag_arr(_tmp105,sizeof(void*),0));});return Cyc_Tcutil_typecmp((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpEF->topt))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF1->topt))->v);_LLA6: _tmpF2=_tmpD3.f1;if(*((int*)_tmpF2)!= 18)
goto _LLA8;_tmpF3=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpF2)->f1;_tmpF4=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpF2)->f2;_tmpF5=_tmpD3.f2;if(*((
int*)_tmpF5)!= 18)goto _LLA8;_tmpF6=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmpF5)->f1;_tmpF7=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpF5)->f2;_LLA7: {
int _tmp106=Cyc_Tcutil_typecmp(_tmpF3,_tmpF6);if(_tmp106 != 0)return _tmp106;{
struct _tuple3 _tmp108=({struct _tuple3 _tmp107;_tmp107.f1=_tmpF4;_tmp107.f2=_tmpF7;
_tmp107;});void*_tmp109;void*_tmp10A;void*_tmp10B;void*_tmp10C;void*_tmp10D;
struct _tagged_arr*_tmp10E;void*_tmp10F;struct _tagged_arr*_tmp110;void*_tmp111;
unsigned int _tmp112;void*_tmp113;unsigned int _tmp114;_LLAD: _tmp109=_tmp108.f1;
if(*((int*)_tmp109)!= 0)goto _LLAF;_tmp10A=_tmp108.f2;if(*((int*)_tmp10A)!= 1)
goto _LLAF;_LLAE: return - 1;_LLAF: _tmp10B=_tmp108.f1;if(*((int*)_tmp10B)!= 1)goto
_LLB1;_tmp10C=_tmp108.f2;if(*((int*)_tmp10C)!= 0)goto _LLB1;_LLB0: return 1;_LLB1:
_tmp10D=_tmp108.f1;if(*((int*)_tmp10D)!= 0)goto _LLB3;_tmp10E=((struct Cyc_Absyn_StructField_struct*)
_tmp10D)->f1;_tmp10F=_tmp108.f2;if(*((int*)_tmp10F)!= 0)goto _LLB3;_tmp110=((
struct Cyc_Absyn_StructField_struct*)_tmp10F)->f1;_LLB2: return Cyc_Std_zstrptrcmp(
_tmp10E,_tmp110);_LLB3: _tmp111=_tmp108.f1;if(*((int*)_tmp111)!= 1)goto _LLAC;
_tmp112=((struct Cyc_Absyn_TupleIndex_struct*)_tmp111)->f1;_tmp113=_tmp108.f2;if(*((
int*)_tmp113)!= 1)goto _LLAC;_tmp114=((struct Cyc_Absyn_TupleIndex_struct*)_tmp113)->f1;
_LLB4: return(int)(_tmp112 - _tmp114);_LLAC:;}}_LLA8: _tmpF8=_tmpD3.f1;if(*((int*)
_tmpF8)!= 13)goto _LLAA;_tmpF9=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpF8)->f1;
_tmpFA=((struct Cyc_Absyn_Cast_e_struct*)_tmpF8)->f2;_tmpFB=_tmpD3.f2;if(*((int*)
_tmpFB)!= 13)goto _LLAA;_tmpFC=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpFB)->f1;
_tmpFD=((struct Cyc_Absyn_Cast_e_struct*)_tmpFB)->f2;_LLA9: {int _tmp115=Cyc_Tcutil_typecmp(
_tmpF9,_tmpFC);if(_tmp115 != 0)return _tmp115;return Cyc_Evexp_const_exp_cmp(_tmpFA,
_tmpFD);}_LLAA:;_LLAB:({void*_tmp116[0]={};((int(*)(struct _tagged_arr fmt,struct
_tagged_arr ap))Cyc_Tcutil_impos)(_tag_arr("Evexp::const_exp_cmp, unexpected case",
sizeof(char),38),_tag_arr(_tmp116,sizeof(void*),0));});_LL99:;}}}}int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){return Cyc_Evexp_const_exp_cmp(e1,
e2)== 0;}int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){unsigned int _tmp118;int _tmp119;struct _tuple4 _tmp117=Cyc_Evexp_eval_const_uint_exp(
e1);_tmp118=_tmp117.f1;_tmp119=_tmp117.f2;{unsigned int _tmp11B;int _tmp11C;struct
_tuple4 _tmp11A=Cyc_Evexp_eval_const_uint_exp(e2);_tmp11B=_tmp11A.f1;_tmp11C=
_tmp11A.f2;if(_tmp119?_tmp11C: 0)return _tmp118 <= _tmp11B;return Cyc_Evexp_same_const_exp(
e1,e2);}}struct _tuple7{struct Cyc_Absyn_Tqual f1;void*f2;};int Cyc_Evexp_okay_szofarg(
void*t){void*_tmp11D=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp11E;struct
Cyc_Core_Opt*_tmp11F;struct Cyc_Core_Opt _tmp120;void*_tmp121;struct Cyc_Absyn_TunionFieldInfo
_tmp122;void*_tmp123;struct Cyc_Absyn_Tunionfield*_tmp124;struct Cyc_List_List*
_tmp125;struct Cyc_Absyn_PtrInfo _tmp126;struct Cyc_Absyn_Conref*_tmp127;void*
_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_AggrInfo _tmp12A;void*_tmp12B;
struct Cyc_List_List*_tmp12C;struct Cyc_Absyn_Typedefdecl*_tmp12D;_LLB6: if((int)
_tmp11D != 0)goto _LLB8;_LLB7: return 0;_LLB8: if(_tmp11D <= (void*)3?1:*((int*)
_tmp11D)!= 1)goto _LLBA;_tmp11E=((struct Cyc_Absyn_VarType_struct*)_tmp11D)->f1;
_LLB9: {void*_tmp12E=Cyc_Tcutil_tvar_kind(_tmp11E);_LLED: if((int)_tmp12E != 2)
goto _LLEF;_LLEE: return 1;_LLEF:;_LLF0: return 0;_LLEC:;}_LLBA: if(_tmp11D <= (void*)3?
1:*((int*)_tmp11D)!= 0)goto _LLBC;_tmp11F=((struct Cyc_Absyn_Evar_struct*)_tmp11D)->f1;
if(_tmp11F == 0)goto _LLBC;_tmp120=*_tmp11F;_tmp121=(void*)_tmp120.v;if((int)
_tmp121 != 2)goto _LLBC;_LLBB: return 1;_LLBC: if(_tmp11D <= (void*)3?1:*((int*)
_tmp11D)!= 0)goto _LLBE;_LLBD: return 0;_LLBE: if(_tmp11D <= (void*)3?1:*((int*)
_tmp11D)!= 2)goto _LLC0;_LLBF: return 1;_LLC0: if(_tmp11D <= (void*)3?1:*((int*)
_tmp11D)!= 3)goto _LLC2;_tmp122=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp11D)->f1;
_tmp123=(void*)_tmp122.field_info;if(*((int*)_tmp123)!= 1)goto _LLC2;_tmp124=((
struct Cyc_Absyn_KnownTunionfield_struct*)_tmp123)->f2;_LLC1: _tmp125=_tmp124->typs;
goto _LLC3;_LLC2: if(_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 9)goto _LLC4;_tmp125=((
struct Cyc_Absyn_TupleType_struct*)_tmp11D)->f1;_LLC3: for(0;_tmp125 != 0;_tmp125=
_tmp125->tl){if(!Cyc_Evexp_okay_szofarg((*((struct _tuple7*)_tmp125->hd)).f2))
return 0;}return 1;_LLC4: if(_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 3)goto _LLC6;
_LLC5: return 0;_LLC6: if(_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 4)goto _LLC8;
_tmp126=((struct Cyc_Absyn_PointerType_struct*)_tmp11D)->f1;_tmp127=_tmp126.bounds;
_LLC7: {void*_tmp12F=(void*)(Cyc_Absyn_compress_conref(_tmp127))->v;_LLF2: if(
_tmp12F <= (void*)1?1:*((int*)_tmp12F)!= 0)goto _LLF4;_LLF3: return 1;_LLF4:;_LLF5:
return 0;_LLF1:;}_LLC8: if(_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 14)goto _LLCA;
_LLC9: goto _LLCB;_LLCA: if(_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 17)goto _LLCC;
_LLCB: goto _LLCD;_LLCC: if(_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 5)goto _LLCE;
_LLCD: goto _LLCF;_LLCE: if((int)_tmp11D != 1)goto _LLD0;_LLCF: goto _LLD1;_LLD0: if(
_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 6)goto _LLD2;_LLD1: return 1;_LLD2: if(
_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 7)goto _LLD4;_tmp128=(void*)((struct Cyc_Absyn_ArrayType_struct*)
_tmp11D)->f1;_tmp129=((struct Cyc_Absyn_ArrayType_struct*)_tmp11D)->f3;_LLD3:
return _tmp129 != 0;_LLD4: if(_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 8)goto _LLD6;
_LLD5: return 0;_LLD6: if(_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 15)goto _LLD8;
_LLD7: return 1;_LLD8: if(_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 10)goto _LLDA;
_tmp12A=((struct Cyc_Absyn_AggrType_struct*)_tmp11D)->f1;_tmp12B=(void*)_tmp12A.aggr_info;
_LLD9: {struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp130=_tmp12B;struct Cyc_Absyn_Aggrdecl**
_tmp131;struct Cyc_Absyn_Aggrdecl*_tmp132;_LLF7: if(*((int*)_tmp130)!= 0)goto _LLF9;
_LLF8:({void*_tmp133[0]={};((int(*)(struct _tagged_arr fmt,struct _tagged_arr ap))
Cyc_Tcutil_impos)(_tag_arr("szof on unchecked StructType",sizeof(char),29),
_tag_arr(_tmp133,sizeof(void*),0));});_LLF9: if(*((int*)_tmp130)!= 1)goto _LLF6;
_tmp131=((struct Cyc_Absyn_KnownAggr_struct*)_tmp130)->f1;_tmp132=*_tmp131;_LLFA:
ad=_tmp132;_LLF6:;}if(ad->impl == 0)return 0;_tmp12C=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;goto _LLDB;}_LLDA: if(_tmp11D <= (void*)3?1:*((int*)
_tmp11D)!= 11)goto _LLDC;_tmp12C=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp11D)->f2;
_LLDB: for(0;_tmp12C != 0;_tmp12C=_tmp12C->tl){if(!Cyc_Evexp_okay_szofarg((void*)((
struct Cyc_Absyn_Aggrfield*)_tmp12C->hd)->type))return 0;}return 1;_LLDC: if(_tmp11D
<= (void*)3?1:*((int*)_tmp11D)!= 12)goto _LLDE;_LLDD: goto _LLDF;_LLDE: if(_tmp11D <= (
void*)3?1:*((int*)_tmp11D)!= 13)goto _LLE0;_LLDF: return 1;_LLE0: if(_tmp11D <= (void*)
3?1:*((int*)_tmp11D)!= 18)goto _LLE2;_LLE1: goto _LLE3;_LLE2: if((int)_tmp11D != 2)
goto _LLE4;_LLE3: goto _LLE5;_LLE4: if(_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 20)
goto _LLE6;_LLE5: goto _LLE7;_LLE6: if(_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 19)
goto _LLE8;_LLE7: goto _LLE9;_LLE8: if(_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 21)
goto _LLEA;_LLE9: return 0;_LLEA: if(_tmp11D <= (void*)3?1:*((int*)_tmp11D)!= 16)goto
_LLB5;_tmp12D=((struct Cyc_Absyn_TypedefType_struct*)_tmp11D)->f3;_LLEB: if(
_tmp12D == 0?1: _tmp12D->kind == 0)({void*_tmp134[0]={};((int(*)(struct _tagged_arr
fmt,struct _tagged_arr ap))Cyc_Tcutil_impos)((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp136;_tmp136.tag=0;_tmp136.f1=(struct _tagged_arr)Cyc_Absynpp_typ2string(t);{
void*_tmp135[1]={& _tmp136};Cyc_Std_aprintf(_tag_arr("szof typedeftype %s",
sizeof(char),20),_tag_arr(_tmp135,sizeof(void*),1));}}),_tag_arr(_tmp134,sizeof(
void*),0));});return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp12D->kind))->v
== (void*)2;_LLB5:;}
