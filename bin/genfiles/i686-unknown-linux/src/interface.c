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
 struct Cyc_Core_Opt{void*v;};extern void*Cyc_Core_identity(void*);extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct
_tagged_arr fmt,struct _tagged_arr);extern struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern struct Cyc_List_List*Cyc_List_map(void*(*f)(
void*),struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[18];
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern
unsigned char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;
struct _tagged_arr line;int line_no;int col;};extern unsigned char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _tagged_arr source;
struct Cyc_Position_Segment*seg;void*kind;struct _tagged_arr desc;};extern
unsigned char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Rel_n_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
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
void*field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_TunionType_struct{
int tag;struct Cyc_Absyn_TunionInfo f1;};struct Cyc_Absyn_TunionFieldType_struct{int
tag;struct Cyc_Absyn_TunionFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int
tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;void*f1;struct Cyc_Absyn_Tqual f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_FnType_struct{
int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_StructType_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;struct Cyc_Absyn_Structdecl**f3;};struct Cyc_Absyn_UnionType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Uniondecl**f3;};
struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;};struct Cyc_Absyn_AnonStructType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonUnionType_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_TypedefType_struct{
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
};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_StructMember_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_StructArrow_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct _tagged_arr*f2;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Structdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Tuniondecl*f4;struct Cyc_Absyn_Tunionfield*
f5;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
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
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Struct_p_struct{int tag;
struct Cyc_Absyn_Structdecl*f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;};struct Cyc_Absyn_Tunion_p_struct{int tag;struct Cyc_Absyn_Tuniondecl*
f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;
};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct
Cyc_Absyn_UnknownFields_p_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;
struct Cyc_List_List*f3;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_SwitchC_clause{struct Cyc_Absyn_Exp*
cnst_exp;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{
void*sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};
struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*effect;void*ret_type;struct Cyc_List_List*args;int
c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;
struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*cached_typ;struct Cyc_Core_Opt*
param_vardecls;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structfield{
struct _tagged_arr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*
width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Structdecl{void*sc;struct
Cyc_Core_Opt*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_Uniondecl{void*sc;struct Cyc_Core_Opt*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{
struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_List_List*typs;struct Cyc_Position_Segment*
loc;void*sc;};struct Cyc_Absyn_Tuniondecl{void*sc;struct _tuple0*name;struct Cyc_List_List*
tvs;struct Cyc_Core_Opt*fields;int is_xtunion;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_List_List*tvs;void*defn;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*f4;int f5;};struct Cyc_Absyn_Letv_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Struct_d_struct{int tag;struct Cyc_Absyn_Structdecl*
f1;};struct Cyc_Absyn_Union_d_struct{int tag;struct Cyc_Absyn_Uniondecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _tagged_arr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{int
tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Decl{void*r;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct
Cyc_Absyn_FieldName_struct{int tag;struct _tagged_arr*f1;};extern unsigned char Cyc_Absyn_EmptyAnnot[
15];extern int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);extern int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);extern struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual();
extern void*Cyc_Absyn_uchar_t;extern void*Cyc_Absyn_uint_t;extern void*Cyc_Absyn_sint_t;
extern struct _tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;
extern void*Cyc_Absyn_tagged_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq);extern
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);
extern struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,
struct Cyc_Absyn_Exp*init);extern void*Cyc_Absyn_function_typ(struct Cyc_List_List*
tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*args,int
c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,
struct Cyc_List_List*);struct Cyc_Set_Set;extern unsigned char Cyc_Set_Absent[11];
struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern unsigned char
Cyc_Dict_Absent[11];extern struct Cyc_Dict_Dict*Cyc_Dict_empty(int(*cmp)(void*,
void*));extern int Cyc_Dict_member(struct Cyc_Dict_Dict*d,void*k);extern struct Cyc_Dict_Dict*
Cyc_Dict_insert(struct Cyc_Dict_Dict*d,void*k,void*v);extern void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict*d,void*k);extern void*Cyc_Dict_fold(void*(*f)(void*,void*,
void*),struct Cyc_Dict_Dict*d,void*accum);extern void Cyc_Dict_iter_c(void(*f)(void*,
void*,void*),void*env,struct Cyc_Dict_Dict*d);extern struct Cyc_Dict_Dict*Cyc_Dict_map(
void*(*f)(void*),struct Cyc_Dict_Dict*d);struct Cyc_Tcenv_VarRes_struct{int tag;
void*f1;};struct Cyc_Tcenv_StructRes_struct{int tag;struct Cyc_Absyn_Structdecl*f1;
};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;struct
Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct Cyc_Set_Set*
namespaces;struct Cyc_Dict_Dict*structdecls;struct Cyc_Dict_Dict*uniondecls;struct
Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct Cyc_Dict_Dict*
typedefs;struct Cyc_Dict_Dict*ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Outermost_struct{int tag;void*f1;};struct Cyc_Tcenv_Frame_struct{
int tag;void*f1;void*f2;};struct Cyc_Tcenv_Hidden_struct{int tag;void*f1;void*f2;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict*ae;struct Cyc_Core_Opt*
le;};extern struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();struct Cyc_Interface_I;extern
struct Cyc_Interface_I*Cyc_Interface_empty();extern struct Cyc_Interface_I*Cyc_Interface_final();
extern struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict*ae);struct
_tuple3{struct _tagged_arr f1;struct _tagged_arr f2;};extern int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple3*info);extern
struct Cyc_Interface_I*Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*
i2,struct _tuple3*info);extern struct Cyc_Interface_I*Cyc_Interface_merge_list(
struct Cyc_List_List*li,struct Cyc_List_List*linfo);extern struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo);extern void Cyc_Interface_print(struct Cyc_Interface_I*,
struct Cyc_Std___sFILE*);extern struct Cyc_Interface_I*Cyc_Interface_parse(struct
Cyc_Std___sFILE*);extern void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc_Std___sFILE*);
extern struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc_Std___sFILE*);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};extern void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc_Std___sFILE*f);extern struct _tagged_arr Cyc_Absynpp_qvar2string(
struct _tuple0*);extern void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,
struct Cyc_List_List*ds);extern unsigned char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*
refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};extern struct
Cyc_List_List*Cyc_Parse_parse_file(struct Cyc_Std___sFILE*f);struct Cyc_Declaration_spec;
struct Cyc_Declarator;struct Cyc_Abstractdeclarator;extern int Cyc_yyparse();extern
unsigned char Cyc_AbstractDeclarator_tok[27];struct Cyc_AbstractDeclarator_tok_struct{
unsigned char*tag;struct Cyc_Abstractdeclarator*f1;};extern unsigned char Cyc_AttributeList_tok[
22];struct Cyc_AttributeList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;
};extern unsigned char Cyc_Attribute_tok[18];struct Cyc_Attribute_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Bool_tok[13];struct Cyc_Bool_tok_struct{
unsigned char*tag;int f1;};extern unsigned char Cyc_Char_tok[13];struct Cyc_Char_tok_struct{
unsigned char*tag;unsigned char f1;};extern unsigned char Cyc_DeclList_tok[17];
struct Cyc_DeclList_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_DeclSpec_tok[17];struct Cyc_DeclSpec_tok_struct{unsigned char*
tag;struct Cyc_Declaration_spec*f1;};extern unsigned char Cyc_Declarator_tok[19];
struct Cyc_Declarator_tok_struct{unsigned char*tag;struct Cyc_Declarator*f1;};
extern unsigned char Cyc_DesignatorList_tok[23];struct Cyc_DesignatorList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Designator_tok[
19];struct Cyc_Designator_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_EnumfieldList_tok[22];struct Cyc_EnumfieldList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Enumfield_tok[
18];struct Cyc_Enumfield_tok_struct{unsigned char*tag;struct Cyc_Absyn_Enumfield*
f1;};extern unsigned char Cyc_ExpList_tok[16];struct Cyc_ExpList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Exp_tok[12];
struct Cyc_Exp_tok_struct{unsigned char*tag;struct Cyc_Absyn_Exp*f1;};extern
unsigned char Cyc_FieldPatternList_tok[25];struct Cyc_FieldPatternList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_FieldPattern_tok[
21];struct _tuple4{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_FieldPattern_tok_struct{
unsigned char*tag;struct _tuple4*f1;};extern unsigned char Cyc_FnDecl_tok[15];
struct Cyc_FnDecl_tok_struct{unsigned char*tag;struct Cyc_Absyn_Fndecl*f1;};extern
unsigned char Cyc_IdList_tok[15];struct Cyc_IdList_tok_struct{unsigned char*tag;
struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDeclList_tok[21];struct Cyc_InitDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_InitDecl_tok[
17];struct _tuple5{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_InitDecl_tok_struct{
unsigned char*tag;struct _tuple5*f1;};extern unsigned char Cyc_InitializerList_tok[
24];struct Cyc_InitializerList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_Int_tok[12];struct _tuple6{void*f1;int f2;};struct Cyc_Int_tok_struct{
unsigned char*tag;struct _tuple6*f1;};extern unsigned char Cyc_Kind_tok[13];struct
Cyc_Kind_tok_struct{unsigned char*tag;void*f1;};extern unsigned char Cyc_Okay_tok[
13];extern unsigned char Cyc_ParamDeclListBool_tok[26];struct _tuple7{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_ParamDeclListBool_tok_struct{unsigned char*tag;struct _tuple7*f1;}
;extern unsigned char Cyc_ParamDeclList_tok[22];struct Cyc_ParamDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_ParamDecl_tok[
18];struct Cyc_ParamDecl_tok_struct{unsigned char*tag;struct _tuple1*f1;};extern
unsigned char Cyc_PatternList_tok[20];struct Cyc_PatternList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_Pattern_tok[16];
struct Cyc_Pattern_tok_struct{unsigned char*tag;struct Cyc_Absyn_Pat*f1;};extern
unsigned char Cyc_Pointer_Sort_tok[21];struct Cyc_Pointer_Sort_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Primop_tok[15];struct Cyc_Primop_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Primopopt_tok[18];struct Cyc_Primopopt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_QualId_tok[15];
struct Cyc_QualId_tok_struct{unsigned char*tag;struct _tuple0*f1;};extern
unsigned char Cyc_QualSpecList_tok[21];struct _tuple8{struct Cyc_Absyn_Tqual f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_QualSpecList_tok_struct{
unsigned char*tag;struct _tuple8*f1;};extern unsigned char Cyc_Rgnorder_tok[17];
struct Cyc_Rgnorder_tok_struct{unsigned char*tag;struct Cyc_List_List*f1;};extern
unsigned char Cyc_Scope_tok[14];struct Cyc_Scope_tok_struct{unsigned char*tag;void*
f1;};extern unsigned char Cyc_Short_tok[14];struct Cyc_Short_tok_struct{
unsigned char*tag;short f1;};extern unsigned char Cyc_Stmt_tok[13];struct Cyc_Stmt_tok_struct{
unsigned char*tag;struct Cyc_Absyn_Stmt*f1;};extern unsigned char Cyc_StorageClass_tok[
21];struct Cyc_StorageClass_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_String_tok[15];struct Cyc_String_tok_struct{unsigned char*tag;
struct _tagged_arr f1;};extern unsigned char Cyc_Stringopt_tok[18];struct Cyc_Stringopt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_StructFieldDeclListList_tok[
32];struct Cyc_StructFieldDeclListList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_StructFieldDeclList_tok[28];struct Cyc_StructFieldDeclList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_StructOrUnion_tok[
22];struct Cyc_StructOrUnion_tok_struct{unsigned char*tag;void*f1;};extern
unsigned char Cyc_SwitchCClauseList_tok[26];struct Cyc_SwitchCClauseList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_SwitchClauseList_tok[
25];struct Cyc_SwitchClauseList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_TunionFieldList_tok[24];struct Cyc_TunionFieldList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_TunionField_tok[
20];struct Cyc_TunionField_tok_struct{unsigned char*tag;struct Cyc_Absyn_Tunionfield*
f1;};extern unsigned char Cyc_TypeList_tok[17];struct Cyc_TypeList_tok_struct{
unsigned char*tag;struct Cyc_List_List*f1;};extern unsigned char Cyc_TypeModifierList_tok[
25];struct Cyc_TypeModifierList_tok_struct{unsigned char*tag;struct Cyc_List_List*
f1;};extern unsigned char Cyc_TypeOpt_tok[16];struct Cyc_TypeOpt_tok_struct{
unsigned char*tag;struct Cyc_Core_Opt*f1;};extern unsigned char Cyc_TypeQual_tok[17];
struct Cyc_TypeQual_tok_struct{unsigned char*tag;struct Cyc_Absyn_Tqual f1;};extern
unsigned char Cyc_TypeSpecifier_tok[22];struct Cyc_TypeSpecifier_tok_struct{
unsigned char*tag;void*f1;};extern unsigned char Cyc_Type_tok[13];struct Cyc_Type_tok_struct{
unsigned char*tag;void*f1;};extern int Cyc_yyparse();extern unsigned char Cyc_YY1[8];
struct Cyc_YY1_struct{unsigned char*tag;struct _tuple7*f1;};extern unsigned char Cyc_YYINITIALSVAL[
18];struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;
int last_column;};extern int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct
_tagged_arr*s2);extern int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);
extern unsigned char Cyc_Tcutil_TypeErr[12];extern void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _tagged_arr fmt,struct _tagged_arr ap);extern void*Cyc_Tcutil_compress(void*t);
extern int Cyc_Tcutil_is_function_type(void*t);extern unsigned char Cyc_Tcdecl_Incompatible[
17];struct Cyc_Tcdecl_Xtunionfielddecl{struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*
field;};extern void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*loc,struct
_tagged_arr*msg1,struct _tagged_arr fmt,struct _tagged_arr ap);extern struct Cyc_Absyn_Structdecl*
Cyc_Tcdecl_merge_structdecl(struct Cyc_Absyn_Structdecl*d0,struct Cyc_Absyn_Structdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Uniondecl*
Cyc_Tcdecl_merge_uniondecl(struct Cyc_Absyn_Uniondecl*d0,struct Cyc_Absyn_Uniondecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Tuniondecl*
Cyc_Tcdecl_merge_tuniondecl(struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Enumdecl*
Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_Absyn_Typedefdecl*
Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern struct Cyc_Tcdecl_Xtunionfielddecl*
Cyc_Tcdecl_merge_xtunionfielddecl(struct Cyc_Tcdecl_Xtunionfielddecl*d0,struct Cyc_Tcdecl_Xtunionfielddecl*
d1,struct Cyc_Position_Segment*loc,struct _tagged_arr*msg);extern void Cyc_Lex_lex_init();
struct Cyc_Interface_Ienv{struct Cyc_Dict_Dict*structdecls;struct Cyc_Dict_Dict*
uniondecls;struct Cyc_Dict_Dict*tuniondecls;struct Cyc_Dict_Dict*enumdecls;struct
Cyc_Dict_Dict*typedefdecls;struct Cyc_Dict_Dict*vardecls;struct Cyc_Dict_Dict*
xtunionfielddecls;};struct Cyc_Interface_I{struct Cyc_Dict_Dict*imports;struct Cyc_Dict_Dict*
exports;};static struct Cyc_Interface_Ienv*Cyc_Interface_new_ienv(){return({struct
Cyc_Interface_Ienv*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->structdecls=((struct
Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp0->uniondecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp0->tuniondecls=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp0->enumdecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp0->typedefdecls=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp0->vardecls=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp);_tmp0->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(
Cyc_Std_zstrptrcmp);_tmp0;});}static struct Cyc_Interface_Ienv*Cyc_Interface_lazy_new_ienv(){
static struct Cyc_Interface_Ienv*ie=0;if(ie == 0){ie=(struct Cyc_Interface_Ienv*)Cyc_Interface_new_ienv();}
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
_tmp11->f2=_init_tag_arr(_cycalloc(sizeof(struct _tagged_arr)),"main",sizeof(
unsigned char),5);_tmp11;});struct Cyc_Absyn_Vardecl*_tmpC=Cyc_Absyn_new_vardecl(
qmain,Cyc_Absyn_function_typ(0,0,Cyc_Absyn_sint_t,({struct Cyc_List_List*_tmpD=
_cycalloc(sizeof(*_tmpD));_tmpD->hd=({struct _tuple1*_tmp10=_cycalloc(sizeof(*
_tmp10));_tmp10->f1=0;_tmp10->f2=Cyc_Absyn_empty_tqual();_tmp10->f3=Cyc_Absyn_uint_t;
_tmp10;});_tmpD->tl=({struct Cyc_List_List*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->hd=({
struct _tuple1*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->f1=0;_tmpF->f2=Cyc_Absyn_empty_tqual();
_tmpF->f3=Cyc_Absyn_tagged_typ(Cyc_Absyn_tagged_typ(Cyc_Absyn_uchar_t,(void*)2,
Cyc_Absyn_empty_tqual()),(void*)2,Cyc_Absyn_empty_tqual());_tmpF;});_tmpE->tl=0;
_tmpE;});_tmpD;}),0,0,0,0),0);(void*)(_tmpC->sc=(void*)((void*)3));_tmpB->vardecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Vardecl*
v))Cyc_Dict_insert)(_tmpB->vardecls,(*qmain).f2,_tmpC);_tmp3->imports=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*
v))Cyc_Dict_insert)(_tmp3->exports,0,_tmpB);return _tmp3;}}static void Cyc_Interface_err(
struct _tagged_arr msg){({void*_tmp16[0]={};Cyc_Tcutil_terr(0,msg,_tag_arr(_tmp16,
sizeof(void*),0));});}static void Cyc_Interface_fields_err(struct _tagged_arr sc,
struct _tagged_arr t,struct _tuple0*n){Cyc_Interface_err((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(n);{struct Cyc_Std_String_pa_struct _tmp19;_tmp19.tag=0;
_tmp19.f1=(struct _tagged_arr)t;{struct Cyc_Std_String_pa_struct _tmp18;_tmp18.tag=
0;_tmp18.f1=(struct _tagged_arr)sc;{void*_tmp17[3]={& _tmp18,& _tmp19,& _tmp1A};Cyc_Std_aprintf(
_tag_arr("fields of %s %s %s have never been defined",sizeof(unsigned char),43),
_tag_arr(_tmp17,sizeof(void*),3));}}}}));}static void Cyc_Interface_body_err(
struct _tagged_arr sc,struct _tuple0*n){Cyc_Interface_err((struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(n);{struct Cyc_Std_String_pa_struct _tmp1C;_tmp1C.tag=0;
_tmp1C.f1=(struct _tagged_arr)sc;{void*_tmp1B[2]={& _tmp1C,& _tmp1D};Cyc_Std_aprintf(
_tag_arr("the body of %s function %s has never been defined",sizeof(
unsigned char),50),_tag_arr(_tmp1B,sizeof(void*),2));}}}));}static void Cyc_Interface_static_err(
struct _tagged_arr obj1,struct _tuple0*name1,struct _tagged_arr obj2,struct _tuple0*
name2){if(obj1.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){Cyc_Interface_err((
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(name2);{struct Cyc_Std_String_pa_struct
_tmp21;_tmp21.tag=0;_tmp21.f1=(struct _tagged_arr)obj2;{struct Cyc_Std_String_pa_struct
_tmp20;_tmp20.tag=0;_tmp20.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(name1);{
struct Cyc_Std_String_pa_struct _tmp1F;_tmp1F.tag=0;_tmp1F.f1=(struct _tagged_arr)
obj1;{void*_tmp1E[4]={& _tmp1F,& _tmp20,& _tmp21,& _tmp22};Cyc_Std_aprintf(_tag_arr("declaration of %s %s relies on static %s %s",
sizeof(unsigned char),44),_tag_arr(_tmp1E,sizeof(void*),4));}}}}}));}}static void
Cyc_Interface_abstract_err(struct _tagged_arr obj1,struct _tuple0*name1,struct
_tagged_arr obj2,struct _tuple0*name2){if(obj1.curr != ((struct _tagged_arr)_tag_arr(
0,0,0)).curr){Cyc_Interface_err((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp27;_tmp27.tag=0;_tmp27.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(name2);{
struct Cyc_Std_String_pa_struct _tmp26;_tmp26.tag=0;_tmp26.f1=(struct _tagged_arr)
obj2;{struct Cyc_Std_String_pa_struct _tmp25;_tmp25.tag=0;_tmp25.f1=(struct
_tagged_arr)Cyc_Absynpp_qvar2string(name1);{struct Cyc_Std_String_pa_struct _tmp24;
_tmp24.tag=0;_tmp24.f1=(struct _tagged_arr)obj1;{void*_tmp23[4]={& _tmp24,& _tmp25,&
_tmp26,& _tmp27};Cyc_Std_aprintf(_tag_arr("declaration of %s %s relies on fields of abstract %s %s",
sizeof(unsigned char),56),_tag_arr(_tmp23,sizeof(void*),4));}}}}}));}}struct Cyc_Interface_Seen{
struct Cyc_Dict_Dict*structs;struct Cyc_Dict_Dict*unions;struct Cyc_Dict_Dict*
tunions;};static struct Cyc_Interface_Seen*Cyc_Interface_new_seen(){return({struct
Cyc_Interface_Seen*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->structs=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp28->unions=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))
Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp28->tunions=((struct Cyc_Dict_Dict*(*)(
int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);
_tmp28;});}static int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict*ae,
struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct _tuple0*name,void*t);
static int Cyc_Interface_check_public_type_list(struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*
seen,struct _tagged_arr obj,struct _tuple0*name,void*(*f)(void*),struct Cyc_List_List*
l){int _tmp29=1;for(0;l != 0;l=l->tl){if(! Cyc_Interface_check_public_type(ae,seen,
obj,name,f((void*)l->hd))){_tmp29=0;}}return _tmp29;}static int Cyc_Interface_check_public_structdecl(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Structdecl*
d){struct _handler_cons _tmp2A;_push_handler(& _tmp2A);{int _tmp2C=0;if(setjmp(
_tmp2A.handler)){_tmp2C=1;}if(! _tmp2C){{int _tmp2D=((int(*)(struct Cyc_Dict_Dict*d,
struct _tuple0*k))Cyc_Dict_lookup)(seen->structs,(struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(d->name))->v);_npop_handler(0);return _tmp2D;};_pop_handler();}else{
void*_tmp2B=(void*)_exn_thrown;void*_tmp2F=_tmp2B;_LL1: if(_tmp2F == Cyc_Dict_Absent){
goto _LL2;}else{goto _LL3;}_LL3: goto _LL4;_LL2: {int _tmp30=1;seen->structs=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)(
seen->structs,(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(d->name))->v,
_tmp30);if(d->fields != 0){struct Cyc_List_List*_tmp31=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(d->fields))->v;for(0;_tmp31 != 0;_tmp31=_tmp31->tl){
if(! Cyc_Interface_check_public_type(ae,seen,_tag_arr("struct",sizeof(
unsigned char),7),(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(d->name))->v,(
void*)((struct Cyc_Absyn_Structfield*)_tmp31->hd)->type)){_tmp30=0;}}}seen->structs=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)(
seen->structs,(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(d->name))->v,
_tmp30);return _tmp30;}_LL4:(void)_throw(_tmp2F);_LL0:;}}}static int Cyc_Interface_check_public_uniondecl(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Uniondecl*d){
struct _handler_cons _tmp32;_push_handler(& _tmp32);{int _tmp34=0;if(setjmp(_tmp32.handler)){
_tmp34=1;}if(! _tmp34){{int _tmp35=((int(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k))
Cyc_Dict_lookup)(seen->unions,(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(
d->name))->v);_npop_handler(0);return _tmp35;};_pop_handler();}else{void*_tmp33=(
void*)_exn_thrown;void*_tmp37=_tmp33;_LL6: if(_tmp37 == Cyc_Dict_Absent){goto _LL7;}
else{goto _LL8;}_LL8: goto _LL9;_LL7: {int _tmp38=1;seen->unions=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)(seen->unions,(
struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(d->name))->v,_tmp38);if(d->fields
!= 0){struct Cyc_List_List*_tmp39=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(d->fields))->v;for(0;_tmp39 != 0;_tmp39=_tmp39->tl){if(! Cyc_Interface_check_public_type(
ae,seen,_tag_arr("union",sizeof(unsigned char),6),(struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(d->name))->v,(void*)((struct Cyc_Absyn_Structfield*)_tmp39->hd)->type)){
_tmp38=0;}}}seen->unions=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tuple0*k,int v))Cyc_Dict_insert)(seen->unions,(struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(d->name))->v,_tmp38);return _tmp38;}_LL9:(void)_throw(_tmp37);_LL5:;}}}
struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_Interface_get_type1(
struct _tuple9*x){return(*x).f2;}static int Cyc_Interface_check_public_tuniondecl(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Tuniondecl*
d){struct _handler_cons _tmp3A;_push_handler(& _tmp3A);{int _tmp3C=0;if(setjmp(
_tmp3A.handler)){_tmp3C=1;}if(! _tmp3C){{int _tmp3D=((int(*)(struct Cyc_Dict_Dict*d,
struct _tuple0*k))Cyc_Dict_lookup)(seen->tunions,d->name);_npop_handler(0);return
_tmp3D;};_pop_handler();}else{void*_tmp3B=(void*)_exn_thrown;void*_tmp3F=_tmp3B;
_LLB: if(_tmp3F == Cyc_Dict_Absent){goto _LLC;}else{goto _LLD;}_LLD: goto _LLE;_LLC: {
int _tmp40=1;seen->tunions=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tuple0*k,int v))Cyc_Dict_insert)(seen->tunions,d->name,_tmp40);if(d->fields != 0){
struct Cyc_List_List*_tmp41=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(d->fields))->v;for(0;_tmp41 != 0;_tmp41=_tmp41->tl){if(!((int(*)(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct _tagged_arr obj,struct
_tuple0*name,void*(*f)(struct _tuple9*),struct Cyc_List_List*l))Cyc_Interface_check_public_type_list)(
ae,seen,_tag_arr("[x]tunion",sizeof(unsigned char),10),d->name,Cyc_Interface_get_type1,((
struct Cyc_Absyn_Tunionfield*)_tmp41->hd)->typs)){_tmp40=0;}}}seen->tunions=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tuple0*k,int v))Cyc_Dict_insert)(
seen->tunions,d->name,_tmp40);return _tmp40;}_LLE:(void)_throw(_tmp3F);_LLA:;}}}
static int Cyc_Interface_check_public_enumdecl(struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*
seen,struct Cyc_Absyn_Enumdecl*d){return 1;}static int Cyc_Interface_check_public_typedefdecl(
struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Typedefdecl*
d){return Cyc_Interface_check_public_type(ae,seen,(struct _tagged_arr)_tag_arr(0,0,
0),d->name,(void*)d->defn);}static int Cyc_Interface_check_public_vardecl(struct
Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*seen,struct Cyc_Absyn_Vardecl*d){return
Cyc_Interface_check_public_type(ae,seen,_tag_arr("variable",sizeof(unsigned char),
9),d->name,(void*)d->type);}static void*Cyc_Interface_get_type2(struct _tuple1*x){
return(*x).f3;}static struct Cyc_List_List*Cyc_Interface_get_abs_ns(void*ns){void*
_tmp42=ns;struct Cyc_List_List*_tmp43;_LL10: if((unsigned int)_tmp42 > 1?*((int*)
_tmp42)== 1: 0){_LL14: _tmp43=((struct Cyc_Absyn_Abs_n_struct*)_tmp42)->f1;goto
_LL11;}else{goto _LL12;}_LL12: goto _LL13;_LL11: return _tmp43;_LL13:(int)_throw((
void*)({struct Cyc_Core_Invalid_argument_struct*_tmp44=_cycalloc(sizeof(*_tmp44));
_tmp44[0]=({struct Cyc_Core_Invalid_argument_struct _tmp45;_tmp45.tag=Cyc_Core_Invalid_argument;
_tmp45.f1=_tag_arr("get_abs_ns",sizeof(unsigned char),11);_tmp45;});_tmp44;}));
_LLF:;}static int Cyc_Interface_check_public_type(struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*
seen,struct _tagged_arr obj,struct _tuple0*name,void*t){void*_tmp46=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp47;void*_tmp48;void*_tmp49;struct Cyc_Core_Opt*
_tmp4A;struct Cyc_Core_Opt _tmp4B;void*_tmp4C;struct Cyc_Absyn_FnInfo _tmp4D;struct
Cyc_Absyn_VarargInfo*_tmp4E;struct Cyc_List_List*_tmp4F;void*_tmp50;struct Cyc_List_List*
_tmp51;struct Cyc_Absyn_Structdecl**_tmp52;struct Cyc_Absyn_Structdecl*_tmp53;
struct Cyc_List_List*_tmp54;struct Cyc_Absyn_Uniondecl**_tmp55;struct Cyc_Absyn_Uniondecl*
_tmp56;struct Cyc_List_List*_tmp57;struct _tuple0*_tmp58;struct Cyc_Absyn_TunionInfo
_tmp59;struct Cyc_List_List*_tmp5A;void*_tmp5B;struct Cyc_Absyn_Tuniondecl**_tmp5C;
struct Cyc_Absyn_Tuniondecl*_tmp5D;struct Cyc_Absyn_TunionFieldInfo _tmp5E;struct
Cyc_List_List*_tmp5F;void*_tmp60;struct Cyc_Absyn_Tunionfield*_tmp61;struct Cyc_Absyn_Tuniondecl*
_tmp62;_LL16: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 4: 0){_LL2C: _tmp47=((
struct Cyc_Absyn_PointerType_struct*)_tmp46)->f1;_LL2D: _tmp48=(void*)_tmp47.elt_typ;
goto _LL17;}else{goto _LL18;}_LL18: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 7:
0){_LL2E: _tmp49=(void*)((struct Cyc_Absyn_ArrayType_struct*)_tmp46)->f1;goto _LL19;}
else{goto _LL1A;}_LL1A: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 18: 0){_LL2F:
_tmp4A=((struct Cyc_Absyn_TypedefType_struct*)_tmp46)->f3;if(_tmp4A == 0){goto
_LL1C;}else{_tmp4B=*_tmp4A;_LL30: _tmp4C=(void*)_tmp4B.v;goto _LL1B;}}else{goto
_LL1C;}_LL1C: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 8: 0){_LL31: _tmp4D=((
struct Cyc_Absyn_FnType_struct*)_tmp46)->f1;_LL34: _tmp50=(void*)_tmp4D.ret_typ;
goto _LL33;_LL33: _tmp4F=_tmp4D.args;goto _LL32;_LL32: _tmp4E=_tmp4D.cyc_varargs;
goto _LL1D;}else{goto _LL1E;}_LL1E: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 9:
0){_LL35: _tmp51=((struct Cyc_Absyn_TupleType_struct*)_tmp46)->f1;goto _LL1F;}else{
goto _LL20;}_LL20: if((unsigned int)_tmp46 > 3?*((int*)_tmp46)== 10: 0){_LL37: _tmp54=((
struct Cyc_Absyn_StructType_struct*)_tmp46)->f2;goto _LL36;_LL36: _tmp52=((struct
Cyc_Absyn_StructType_struct*)_tmp46)->f3;if(_tmp52 == 0){goto _LL22;}else{_tmp53=*
_tmp52;goto _LL21;}}else{goto _LL22;}_LL22: if((unsigned int)_tmp46 > 3?*((int*)
_tmp46)== 11: 0){_LL39: _tmp57=((struct Cyc_Absyn_UnionType_struct*)_tmp46)->f2;
goto _LL38;_LL38: _tmp55=((struct Cyc_Absyn_UnionType_struct*)_tmp46)->f3;if(_tmp55
== 0){goto _LL24;}else{_tmp56=*_tmp55;goto _LL23;}}else{goto _LL24;}_LL24: if((
unsigned int)_tmp46 > 3?*((int*)_tmp46)== 12: 0){_LL3A: _tmp58=((struct Cyc_Absyn_EnumType_struct*)
_tmp46)->f1;goto _LL25;}else{goto _LL26;}_LL26: if((unsigned int)_tmp46 > 3?*((int*)
_tmp46)== 2: 0){_LL3B: _tmp59=((struct Cyc_Absyn_TunionType_struct*)_tmp46)->f1;
_LL3D: _tmp5B=(void*)_tmp59.tunion_info;if(*((int*)_tmp5B)== 1){_LL3E: _tmp5C=((
struct Cyc_Absyn_KnownTunion_struct*)_tmp5B)->f1;_tmp5D=*_tmp5C;goto _LL3C;}else{
goto _LL28;}_LL3C: _tmp5A=_tmp59.targs;goto _LL27;}else{goto _LL28;}_LL28: if((
unsigned int)_tmp46 > 3?*((int*)_tmp46)== 3: 0){_LL3F: _tmp5E=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp46)->f1;_LL41: _tmp60=(void*)_tmp5E.field_info;if(*((int*)_tmp60)== 1){_LL43:
_tmp62=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp60)->f1;goto _LL42;_LL42:
_tmp61=((struct Cyc_Absyn_KnownTunionfield_struct*)_tmp60)->f2;goto _LL40;}else{
goto _LL2A;}_LL40: _tmp5F=_tmp5E.targs;goto _LL29;}else{goto _LL2A;}_LL2A: goto _LL2B;
_LL17: return Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp48);_LL19:
return Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp49);_LL1B: return Cyc_Interface_check_public_type(
ae,seen,obj,name,_tmp4C);_LL1D: {int b=((int(*)(struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*
seen,struct _tagged_arr obj,struct _tuple0*name,void*(*f)(struct _tuple1*),struct Cyc_List_List*
l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type2,
_tmp4F)? Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp50): 0;if(_tmp4E != 0){
void*_tmp64;struct Cyc_Absyn_VarargInfo _tmp63=*_tmp4E;_LL45: _tmp64=(void*)_tmp63.type;
goto _LL44;_LL44: b=Cyc_Interface_check_public_type(ae,seen,obj,name,_tmp64);}
return b;}_LL1F: return((int(*)(struct Cyc_Dict_Dict*ae,struct Cyc_Interface_Seen*
seen,struct _tagged_arr obj,struct _tuple0*name,void*(*f)(struct _tuple9*),struct Cyc_List_List*
l))Cyc_Interface_check_public_type_list)(ae,seen,obj,name,Cyc_Interface_get_type1,
_tmp51);_LL21: if((void*)_tmp53->sc == (void*)0){Cyc_Interface_static_err(obj,name,
_tag_arr("struct",sizeof(unsigned char),7),(struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(_tmp53->name))->v);return 0;}return Cyc_Interface_check_public_type_list(
ae,seen,obj,name,Cyc_Core_identity,_tmp54)? Cyc_Interface_check_public_structdecl(
ae,seen,_tmp53): 0;_LL23: if((void*)_tmp56->sc == (void*)0){Cyc_Interface_static_err(
obj,name,_tag_arr("union",sizeof(unsigned char),6),(struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(_tmp56->name))->v);return 0;}return Cyc_Interface_check_public_type_list(
ae,seen,obj,name,Cyc_Core_identity,_tmp57)? Cyc_Interface_check_public_uniondecl(
ae,seen,_tmp56): 0;_LL25: {struct _tuple0 _tmp66;struct _tagged_arr*_tmp67;void*
_tmp68;struct _tuple0*_tmp65=_tmp58;_tmp66=*_tmp65;_LL48: _tmp68=_tmp66.f1;goto
_LL47;_LL47: _tmp67=_tmp66.f2;goto _LL46;_LL46: {struct Cyc_List_List*_tmp69=Cyc_Interface_get_abs_ns(
_tmp68);struct Cyc_Absyn_Enumdecl*ed;{struct _handler_cons _tmp6A;_push_handler(&
_tmp6A);{int _tmp6C=0;if(setjmp(_tmp6A.handler)){_tmp6C=1;}if(! _tmp6C){{struct Cyc_Tcenv_Genv*
_tmp6D=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(ae,_tmp69);ed=*((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp6D->enumdecls,_tmp67);};_pop_handler();}
else{void*_tmp6B=(void*)_exn_thrown;void*_tmp6F=_tmp6B;_LL4A: if(_tmp6F == Cyc_Dict_Absent){
goto _LL4B;}else{goto _LL4C;}_LL4C: goto _LL4D;_LL4B:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp71;_tmp71.tag=Cyc_Core_Invalid_argument;_tmp71.f1=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmp58);{void*_tmp72[1]={& _tmp73};Cyc_Std_aprintf(
_tag_arr("check_public_type (can't find enum %s)",sizeof(unsigned char),39),
_tag_arr(_tmp72,sizeof(void*),1));}});_tmp71;});_tmp70;}));_LL4D:(void)_throw(
_tmp6F);_LL49:;}}}if((void*)ed->sc == (void*)0){Cyc_Interface_static_err(obj,
_tmp58,_tag_arr("enum",sizeof(unsigned char),5),ed->name);return 0;}return 1;}}
_LL27: {struct _tuple0 _tmp75;struct _tagged_arr*_tmp76;void*_tmp77;struct _tuple0*
_tmp74=_tmp5D->name;_tmp75=*_tmp74;_LL50: _tmp77=_tmp75.f1;goto _LL4F;_LL4F: _tmp76=
_tmp75.f2;goto _LL4E;_LL4E: {struct Cyc_List_List*_tmp78=Cyc_Interface_get_abs_ns(
_tmp77);struct Cyc_Absyn_Tuniondecl*tud;{struct _handler_cons _tmp79;_push_handler(&
_tmp79);{int _tmp7B=0;if(setjmp(_tmp79.handler)){_tmp7B=1;}if(! _tmp7B){{struct Cyc_Tcenv_Genv*
_tmp7C=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(ae,_tmp78);tud=*((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp7C->tuniondecls,_tmp76);};
_pop_handler();}else{void*_tmp7A=(void*)_exn_thrown;void*_tmp7E=_tmp7A;_LL52: if(
_tmp7E == Cyc_Dict_Absent){goto _LL53;}else{goto _LL54;}_LL54: goto _LL55;_LL53:(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp7F=_cycalloc(sizeof(*
_tmp7F));_tmp7F[0]=({struct Cyc_Core_Invalid_argument_struct _tmp80;_tmp80.tag=Cyc_Core_Invalid_argument;
_tmp80.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp82;_tmp82.tag=0;
_tmp82.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(_tmp5D->name);{void*_tmp81[
1]={& _tmp82};Cyc_Std_aprintf(_tag_arr("check_public_type (can't find [x]tunion %s)",
sizeof(unsigned char),44),_tag_arr(_tmp81,sizeof(void*),1));}});_tmp80;});_tmp7F;}));
_LL55:(void)_throw(_tmp7E);_LL51:;}}}if((void*)tud->sc == (void*)0){Cyc_Interface_static_err(
obj,name,_tag_arr("[x]tunion",sizeof(unsigned char),10),tud->name);return 0;}
return Cyc_Interface_check_public_type_list(ae,seen,obj,name,Cyc_Core_identity,
_tmp5A)? Cyc_Interface_check_public_tuniondecl(ae,seen,tud): 0;}}_LL29: {struct
_tuple0 _tmp84;struct _tagged_arr*_tmp85;void*_tmp86;struct _tuple0*_tmp83=_tmp62->name;
_tmp84=*_tmp83;_LL58: _tmp86=_tmp84.f1;goto _LL57;_LL57: _tmp85=_tmp84.f2;goto _LL56;
_LL56: {struct Cyc_List_List*_tmp87=Cyc_Interface_get_abs_ns(_tmp86);struct Cyc_Absyn_Tuniondecl*
tud;{struct _handler_cons _tmp88;_push_handler(& _tmp88);{int _tmp8A=0;if(setjmp(
_tmp88.handler)){_tmp8A=1;}if(! _tmp8A){{struct Cyc_Tcenv_Genv*_tmp8B=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ae,_tmp87);tud=*((
struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp8B->tuniondecls,_tmp85);};_pop_handler();}else{void*_tmp89=(void*)
_exn_thrown;void*_tmp8D=_tmp89;_LL5A: if(_tmp8D == Cyc_Dict_Absent){goto _LL5B;}
else{goto _LL5C;}_LL5C: goto _LL5D;_LL5B:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp8F;_tmp8F.tag=Cyc_Core_Invalid_argument;_tmp8F.f1=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp91;_tmp91.tag=0;_tmp91.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmp62->name);{void*_tmp90[1]={& _tmp91};Cyc_Std_aprintf(
_tag_arr("check_public_type (can't find [x]tunion %s and search its fields)",
sizeof(unsigned char),66),_tag_arr(_tmp90,sizeof(void*),1));}});_tmp8F;});_tmp8E;}));
_LL5D:(void)_throw(_tmp8D);_LL59:;}}}if(tud->fields == 0){(int)_throw((void*)({
struct Cyc_Core_Invalid_argument_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92[
0]=({struct Cyc_Core_Invalid_argument_struct _tmp93;_tmp93.tag=Cyc_Core_Invalid_argument;
_tmp93.f1=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp95;_tmp95.tag=0;
_tmp95.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(tud->name);{void*_tmp94[1]={&
_tmp95};Cyc_Std_aprintf(_tag_arr("check_public_type ([x]tunion %s has no fields)",
sizeof(unsigned char),47),_tag_arr(_tmp94,sizeof(void*),1));}});_tmp93;});_tmp92;}));}{
struct Cyc_Absyn_Tunionfield*tuf1=0;{struct Cyc_List_List*_tmp96=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;_tmp96 != 0;_tmp96=_tmp96->tl){
if(Cyc_Std_strptrcmp((*_tmp61->name).f2,(*((struct Cyc_Absyn_Tunionfield*)_tmp96->hd)->name).f2)
== 0){tuf1=(struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)_tmp96->hd);
break;}}}if(tuf1 == 0){(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp98;_tmp98.tag=Cyc_Core_Invalid_argument;_tmp98.f1=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp9A;_tmp9A.tag=0;_tmp9A.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(_tmp61->name);{void*_tmp99[1]={& _tmp9A};Cyc_Std_aprintf(
_tag_arr("check_public_type (can't find [x]tunionfield %s)",sizeof(unsigned char),
49),_tag_arr(_tmp99,sizeof(void*),1));}});_tmp98;});_tmp97;}));}{struct Cyc_Absyn_Tunionfield*
tuf=(struct Cyc_Absyn_Tunionfield*)_check_null(tuf1);if((void*)tud->sc == (void*)0){
Cyc_Interface_static_err(obj,name,_tag_arr("[x]tunion",sizeof(unsigned char),10),
tud->name);return 0;}if((void*)tud->sc == (void*)1){Cyc_Interface_abstract_err(obj,
name,_tag_arr("[x]tunion",sizeof(unsigned char),10),tud->name);return 0;}if((void*)
tuf->sc == (void*)0){Cyc_Interface_static_err(obj,name,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct _tagged_arr)
Cyc_Absynpp_qvar2string(tuf->name);{void*_tmp9B[1]={& _tmp9C};Cyc_Std_aprintf(
_tag_arr("field %s of",sizeof(unsigned char),12),_tag_arr(_tmp9B,sizeof(void*),1));}}),
tud->name);return 0;}return Cyc_Interface_check_public_type_list(ae,seen,obj,name,
Cyc_Core_identity,_tmp5F)? Cyc_Interface_check_public_tuniondecl(ae,seen,tud): 0;}}}}
_LL2B: return 1;_LL15:;}struct _tuple10{struct Cyc_Interface_Ienv*f1;struct Cyc_Interface_Ienv*
f2;int f3;struct Cyc_Dict_Dict*f4;struct Cyc_Interface_Seen*f5;struct Cyc_Interface_I*
f6;};static void Cyc_Interface_extract_structdecl(struct _tuple10*env,struct
_tagged_arr*x,struct Cyc_Absyn_Structdecl**dp){struct _tuple10 _tmp9E;struct Cyc_Interface_Seen*
_tmp9F;struct Cyc_Dict_Dict*_tmpA0;int _tmpA1;struct Cyc_Interface_Ienv*_tmpA2;
struct Cyc_Interface_Ienv*_tmpA3;struct _tuple10*_tmp9D=env;_tmp9E=*_tmp9D;_LL63:
_tmpA3=_tmp9E.f1;goto _LL62;_LL62: _tmpA2=_tmp9E.f2;goto _LL61;_LL61: _tmpA1=_tmp9E.f3;
goto _LL60;_LL60: _tmpA0=_tmp9E.f4;goto _LL5F;_LL5F: _tmp9F=_tmp9E.f5;goto _LL5E;
_LL5E: {struct Cyc_Absyn_Structdecl*_tmpA4=*dp;void*_tmpA5=(void*)_tmpA4->sc;
_LL65: if((int)_tmpA5 == 0){goto _LL66;}else{goto _LL67;}_LL67: if((int)_tmpA5 == 1){
goto _LL68;}else{goto _LL69;}_LL69: if((int)_tmpA5 == 2){goto _LL6A;}else{goto _LL6B;}
_LL6B: if((int)_tmpA5 == 4){goto _LL6C;}else{goto _LL6D;}_LL6D: if((int)_tmpA5 == 3){
goto _LL6E;}else{goto _LL64;}_LL66: if(_tmpA1? _tmpA4->fields == 0: 0){Cyc_Interface_fields_err(
_tag_arr("static",sizeof(unsigned char),7),_tag_arr("struct",sizeof(
unsigned char),7),(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmpA4->name))->v);}
goto _LL64;_LL68: if(_tmpA4->fields == 0){if(_tmpA1){Cyc_Interface_fields_err(
_tag_arr("abstract",sizeof(unsigned char),9),_tag_arr("struct",sizeof(
unsigned char),7),(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmpA4->name))->v);}}
else{_tmpA4=({struct Cyc_Absyn_Structdecl*_tmpA6=_cycalloc(sizeof(*_tmpA6));
_tmpA6[0]=*_tmpA4;_tmpA6;});_tmpA4->fields=0;}if(Cyc_Interface_check_public_structdecl(
_tmpA0,_tmp9F,_tmpA4)){_tmpA2->structdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Structdecl*v))Cyc_Dict_insert)(_tmpA2->structdecls,
x,_tmpA4);}goto _LL64;_LL6A: if(_tmpA4->fields == 0){Cyc_Interface_fields_err(
_tag_arr("public",sizeof(unsigned char),7),_tag_arr("struct",sizeof(
unsigned char),7),(struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(_tmpA4->name))->v);
_tmpA4=({struct Cyc_Absyn_Structdecl*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7[0]=*
_tmpA4;_tmpA7;});(void*)(_tmpA4->sc=(void*)((void*)1));}if(Cyc_Interface_check_public_structdecl(
_tmpA0,_tmp9F,_tmpA4)){_tmpA2->structdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Structdecl*v))Cyc_Dict_insert)(_tmpA2->structdecls,
x,_tmpA4);}goto _LL64;_LL6C: goto _LL6E;_LL6E: if(Cyc_Interface_check_public_structdecl(
_tmpA0,_tmp9F,_tmpA4)){_tmpA3->structdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Structdecl*v))Cyc_Dict_insert)(_tmpA3->structdecls,
x,_tmpA4);}goto _LL64;_LL64:;}}static void Cyc_Interface_extract_uniondecl(struct
_tuple10*env,struct _tagged_arr*x,struct Cyc_Absyn_Uniondecl**dp){struct _tuple10
_tmpA9;struct Cyc_Interface_Seen*_tmpAA;struct Cyc_Dict_Dict*_tmpAB;int _tmpAC;
struct Cyc_Interface_Ienv*_tmpAD;struct Cyc_Interface_Ienv*_tmpAE;struct _tuple10*
_tmpA8=env;_tmpA9=*_tmpA8;_LL74: _tmpAE=_tmpA9.f1;goto _LL73;_LL73: _tmpAD=_tmpA9.f2;
goto _LL72;_LL72: _tmpAC=_tmpA9.f3;goto _LL71;_LL71: _tmpAB=_tmpA9.f4;goto _LL70;
_LL70: _tmpAA=_tmpA9.f5;goto _LL6F;_LL6F: {struct Cyc_Absyn_Uniondecl*_tmpAF=*dp;
void*_tmpB0=(void*)_tmpAF->sc;_LL76: if((int)_tmpB0 == 0){goto _LL77;}else{goto
_LL78;}_LL78: if((int)_tmpB0 == 1){goto _LL79;}else{goto _LL7A;}_LL7A: if((int)_tmpB0
== 2){goto _LL7B;}else{goto _LL7C;}_LL7C: if((int)_tmpB0 == 4){goto _LL7D;}else{goto
_LL7E;}_LL7E: if((int)_tmpB0 == 3){goto _LL7F;}else{goto _LL75;}_LL77: if(_tmpAC?
_tmpAF->fields == 0: 0){Cyc_Interface_fields_err(_tag_arr("static",sizeof(
unsigned char),7),_tag_arr("union",sizeof(unsigned char),6),(struct _tuple0*)((
struct Cyc_Core_Opt*)_check_null(_tmpAF->name))->v);}goto _LL75;_LL79: if(_tmpAF->fields
== 0){if(_tmpAC){Cyc_Interface_fields_err(_tag_arr("abstract",sizeof(
unsigned char),9),_tag_arr("union",sizeof(unsigned char),6),(struct _tuple0*)((
struct Cyc_Core_Opt*)_check_null(_tmpAF->name))->v);}}else{_tmpAF=({struct Cyc_Absyn_Uniondecl*
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=*_tmpAF;_tmpB1;});_tmpAF->fields=0;}
if(Cyc_Interface_check_public_uniondecl(_tmpAB,_tmpAA,_tmpAF)){_tmpAD->uniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Uniondecl*
v))Cyc_Dict_insert)(_tmpAD->uniondecls,x,_tmpAF);}goto _LL75;_LL7B: if(_tmpAF->fields
== 0){Cyc_Interface_fields_err(_tag_arr("public",sizeof(unsigned char),7),
_tag_arr("union",sizeof(unsigned char),6),(struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(_tmpAF->name))->v);_tmpAF=({struct Cyc_Absyn_Uniondecl*_tmpB2=
_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=*_tmpAF;_tmpB2;});(void*)(_tmpAF->sc=(void*)((
void*)1));}if(Cyc_Interface_check_public_uniondecl(_tmpAB,_tmpAA,_tmpAF)){_tmpAD->uniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Uniondecl*
v))Cyc_Dict_insert)(_tmpAD->uniondecls,x,_tmpAF);}goto _LL75;_LL7D: goto _LL7F;
_LL7F: if(Cyc_Interface_check_public_uniondecl(_tmpAB,_tmpAA,_tmpAF)){_tmpAE->uniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Uniondecl*
v))Cyc_Dict_insert)(_tmpAE->uniondecls,x,_tmpAF);}goto _LL75;_LL75:;}}static void
Cyc_Interface_extract_enumdecl(struct _tuple10*env,struct _tagged_arr*x,struct Cyc_Absyn_Enumdecl**
dp){struct _tuple10 _tmpB4;struct Cyc_Interface_Seen*_tmpB5;struct Cyc_Dict_Dict*
_tmpB6;int _tmpB7;struct Cyc_Interface_Ienv*_tmpB8;struct Cyc_Interface_Ienv*_tmpB9;
struct _tuple10*_tmpB3=env;_tmpB4=*_tmpB3;_LL85: _tmpB9=_tmpB4.f1;goto _LL84;_LL84:
_tmpB8=_tmpB4.f2;goto _LL83;_LL83: _tmpB7=_tmpB4.f3;goto _LL82;_LL82: _tmpB6=_tmpB4.f4;
goto _LL81;_LL81: _tmpB5=_tmpB4.f5;goto _LL80;_LL80: {struct Cyc_Absyn_Enumdecl*
_tmpBA=*dp;void*_tmpBB=(void*)_tmpBA->sc;_LL87: if((int)_tmpBB == 0){goto _LL88;}
else{goto _LL89;}_LL89: if((int)_tmpBB == 1){goto _LL8A;}else{goto _LL8B;}_LL8B: if((
int)_tmpBB == 2){goto _LL8C;}else{goto _LL8D;}_LL8D: if((int)_tmpBB == 4){goto _LL8E;}
else{goto _LL8F;}_LL8F: if((int)_tmpBB == 3){goto _LL90;}else{goto _LL86;}_LL88: if(
_tmpB7? _tmpBA->fields == 0: 0){Cyc_Interface_fields_err(_tag_arr("static",sizeof(
unsigned char),7),_tag_arr("enum",sizeof(unsigned char),5),_tmpBA->name);}goto
_LL86;_LL8A: if(_tmpBA->fields == 0){if(_tmpB7){Cyc_Interface_fields_err(_tag_arr("abstract",
sizeof(unsigned char),9),_tag_arr("enum",sizeof(unsigned char),5),_tmpBA->name);}}
else{_tmpBA=({struct Cyc_Absyn_Enumdecl*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[
0]=*_tmpBA;_tmpBC;});_tmpBA->fields=0;}if(Cyc_Interface_check_public_enumdecl(
_tmpB6,_tmpB5,_tmpBA)){_tmpB8->enumdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(_tmpB8->enumdecls,
x,_tmpBA);}goto _LL86;_LL8C: if(_tmpBA->fields == 0){Cyc_Interface_fields_err(
_tag_arr("public",sizeof(unsigned char),7),_tag_arr("enum",sizeof(unsigned char),
5),_tmpBA->name);_tmpBA=({struct Cyc_Absyn_Enumdecl*_tmpBD=_cycalloc(sizeof(*
_tmpBD));_tmpBD[0]=*_tmpBA;_tmpBD;});(void*)(_tmpBA->sc=(void*)((void*)1));}if(
Cyc_Interface_check_public_enumdecl(_tmpB6,_tmpB5,_tmpBA)){_tmpB8->enumdecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl*
v))Cyc_Dict_insert)(_tmpB8->enumdecls,x,_tmpBA);}goto _LL86;_LL8E: goto _LL90;_LL90:
if(Cyc_Interface_check_public_enumdecl(_tmpB6,_tmpB5,_tmpBA)){_tmpB9->enumdecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Enumdecl*
v))Cyc_Dict_insert)(_tmpB9->enumdecls,x,_tmpBA);}goto _LL86;_LL86:;}}static void
Cyc_Interface_extract_xtunionfielddecl(struct Cyc_Interface_I*i,struct Cyc_Absyn_Tuniondecl*
d,struct Cyc_Absyn_Tunionfield*f){struct Cyc_List_List*_tmpBE=0;{void*_tmpBF=(*f->name).f1;
struct Cyc_List_List*_tmpC0;struct Cyc_List_List*_tmpC1;_LL92: if((unsigned int)
_tmpBF > 1?*((int*)_tmpBF)== 0: 0){_LL98: _tmpC0=((struct Cyc_Absyn_Rel_n_struct*)
_tmpBF)->f1;goto _LL93;}else{goto _LL94;}_LL94: if((unsigned int)_tmpBF > 1?*((int*)
_tmpBF)== 1: 0){_LL99: _tmpC1=((struct Cyc_Absyn_Abs_n_struct*)_tmpBF)->f1;goto
_LL95;}else{goto _LL96;}_LL96: goto _LL97;_LL93: _tmpC1=_tmpC0;goto _LL95;_LL95:
_tmpBE=_tmpC1;goto _LL91;_LL97: goto _LL91;_LL91:;}{struct Cyc_Dict_Dict**dict;{void*
_tmpC2=(void*)f->sc;_LL9B: if((int)_tmpC2 == 0){goto _LL9C;}else{goto _LL9D;}_LL9D:
if((int)_tmpC2 == 3){goto _LL9E;}else{goto _LL9F;}_LL9F: if((int)_tmpC2 == 2){goto
_LLA0;}else{goto _LLA1;}_LLA1: goto _LLA2;_LL9C: return;_LL9E: dict=& i->imports;goto
_LL9A;_LLA0: dict=& i->exports;goto _LL9A;_LLA2:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3[0]=({struct Cyc_Core_Invalid_argument_struct
_tmpC4;_tmpC4.tag=Cyc_Core_Invalid_argument;_tmpC4.f1=_tag_arr("add_xtunionfielddecl",
sizeof(unsigned char),21);_tmpC4;});_tmpC3;}));_LL9A:;}{struct Cyc_Interface_Ienv*
env;{struct _handler_cons _tmpC5;_push_handler(& _tmpC5);{int _tmpC7=0;if(setjmp(
_tmpC5.handler)){_tmpC7=1;}if(! _tmpC7){env=((struct Cyc_Interface_Ienv*(*)(struct
Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(*dict,_tmpBE);;
_pop_handler();}else{void*_tmpC6=(void*)_exn_thrown;void*_tmpC9=_tmpC6;_LLA4: if(
_tmpC9 == Cyc_Dict_Absent){goto _LLA5;}else{goto _LLA6;}_LLA6: goto _LLA7;_LLA5: env=
Cyc_Interface_new_ienv();*dict=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*dict,_tmpBE,
env);goto _LLA3;_LLA7:(void)_throw(_tmpC9);_LLA3:;}}}env->xtunionfielddecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Tcdecl_Xtunionfielddecl*
v))Cyc_Dict_insert)(env->xtunionfielddecls,(*f->name).f2,({struct Cyc_Tcdecl_Xtunionfielddecl*
_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->base=d;_tmpCA->field=f;_tmpCA;}));}}}
static void Cyc_Interface_extract_tuniondecl(struct _tuple10*env,struct _tagged_arr*
x,struct Cyc_Absyn_Tuniondecl**dp){struct _tuple10 _tmpCC;struct Cyc_Interface_I*
_tmpCD;struct Cyc_Interface_Seen*_tmpCE;struct Cyc_Dict_Dict*_tmpCF;int _tmpD0;
struct Cyc_Interface_Ienv*_tmpD1;struct Cyc_Interface_Ienv*_tmpD2;struct _tuple10*
_tmpCB=env;_tmpCC=*_tmpCB;_LLAE: _tmpD2=_tmpCC.f1;goto _LLAD;_LLAD: _tmpD1=_tmpCC.f2;
goto _LLAC;_LLAC: _tmpD0=_tmpCC.f3;goto _LLAB;_LLAB: _tmpCF=_tmpCC.f4;goto _LLAA;
_LLAA: _tmpCE=_tmpCC.f5;goto _LLA9;_LLA9: _tmpCD=_tmpCC.f6;goto _LLA8;_LLA8: {struct
Cyc_Absyn_Tuniondecl*_tmpD3=*dp;void*_tmpD4=(void*)_tmpD3->sc;_LLB0: if((int)
_tmpD4 == 0){goto _LLB1;}else{goto _LLB2;}_LLB2: if((int)_tmpD4 == 1){goto _LLB3;}
else{goto _LLB4;}_LLB4: if((int)_tmpD4 == 2){goto _LLB5;}else{goto _LLB6;}_LLB6: if((
int)_tmpD4 == 4){goto _LLB7;}else{goto _LLB8;}_LLB8: if((int)_tmpD4 == 3){goto _LLB9;}
else{goto _LLAF;}_LLB1: if((! _tmpD3->is_xtunion? _tmpD0: 0)? _tmpD3->fields == 0: 0){
Cyc_Interface_fields_err(_tag_arr("static",sizeof(unsigned char),7),_tag_arr("tunion",
sizeof(unsigned char),7),_tmpD3->name);}goto _LLAF;_LLB3: if(_tmpD3->fields == 0){
if(! _tmpD3->is_xtunion? _tmpD0: 0){Cyc_Interface_fields_err(_tag_arr("abstract",
sizeof(unsigned char),9),_tag_arr("tunion",sizeof(unsigned char),7),_tmpD3->name);}}
else{_tmpD3=({struct Cyc_Absyn_Tuniondecl*_tmpD5=_cycalloc(sizeof(*_tmpD5));
_tmpD5[0]=*_tmpD3;_tmpD5;});_tmpD3->fields=0;}if(Cyc_Interface_check_public_tuniondecl(
_tmpCF,_tmpCE,_tmpD3)){_tmpD1->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl*v))Cyc_Dict_insert)(_tmpD1->tuniondecls,
x,_tmpD3);}goto _LLAF;_LLB5: _tmpD3=({struct Cyc_Absyn_Tuniondecl*_tmpD6=_cycalloc(
sizeof(*_tmpD6));_tmpD6[0]=*_tmpD3;_tmpD6;});if(! _tmpD3->is_xtunion? _tmpD3->fields
== 0: 0){Cyc_Interface_fields_err(_tag_arr("public",sizeof(unsigned char),7),
_tag_arr("tunion",sizeof(unsigned char),7),_tmpD3->name);(void*)(_tmpD3->sc=(
void*)((void*)1));}if(Cyc_Interface_check_public_tuniondecl(_tmpCF,_tmpCE,_tmpD3)){
if(_tmpD3->is_xtunion? _tmpD3->fields != 0: 0){struct Cyc_List_List*_tmpD7=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpD3->fields))->v;_tmpD3->fields=
0;{struct Cyc_List_List*_tmpD8=_tmpD7;for(0;_tmpD8 != 0;_tmpD8=_tmpD8->tl){Cyc_Interface_extract_xtunionfielddecl(
_tmpCD,_tmpD3,(struct Cyc_Absyn_Tunionfield*)_tmpD8->hd);}}}_tmpD1->tuniondecls=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl*
v))Cyc_Dict_insert)(_tmpD1->tuniondecls,x,_tmpD3);}goto _LLAF;_LLB7:(int)_throw((
void*)({struct Cyc_Core_Invalid_argument_struct*_tmpD9=_cycalloc(sizeof(*_tmpD9));
_tmpD9[0]=({struct Cyc_Core_Invalid_argument_struct _tmpDA;_tmpDA.tag=Cyc_Core_Invalid_argument;
_tmpDA.f1=_tag_arr("extract_tuniondecl",sizeof(unsigned char),19);_tmpDA;});
_tmpD9;}));_LLB9: if(Cyc_Interface_check_public_tuniondecl(_tmpCF,_tmpCE,_tmpD3)){
if(_tmpD3->is_xtunion? _tmpD3->fields != 0: 0){_tmpD3=({struct Cyc_Absyn_Tuniondecl*
_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB[0]=*_tmpD3;_tmpDB;});{struct Cyc_List_List*
_tmpDC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmpD3->fields))->v;
_tmpD3->fields=0;{struct Cyc_List_List*_tmpDD=_tmpDC;for(0;_tmpDD != 0;_tmpDD=
_tmpDD->tl){Cyc_Interface_extract_xtunionfielddecl(_tmpCD,_tmpD3,(struct Cyc_Absyn_Tunionfield*)
_tmpDD->hd);}}}}_tmpD2->tuniondecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Tuniondecl*v))Cyc_Dict_insert)(_tmpD2->tuniondecls,
x,_tmpD3);}goto _LLAF;_LLAF:;}}static void Cyc_Interface_extract_typedef(struct
_tuple10*env,struct _tagged_arr*x,struct Cyc_Absyn_Typedefdecl*d){struct _tuple10
_tmpDF;struct Cyc_Interface_Seen*_tmpE0;struct Cyc_Dict_Dict*_tmpE1;struct Cyc_Interface_Ienv*
_tmpE2;struct _tuple10*_tmpDE=env;_tmpDF=*_tmpDE;_LLBD: _tmpE2=_tmpDF.f2;goto _LLBC;
_LLBC: _tmpE1=_tmpDF.f4;goto _LLBB;_LLBB: _tmpE0=_tmpDF.f5;goto _LLBA;_LLBA: if(Cyc_Interface_check_public_typedefdecl(
_tmpE1,_tmpE0,d)){_tmpE2->typedefdecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(_tmpE2->typedefdecls,
x,d);}}static void Cyc_Interface_extract_ordinarie(struct _tuple10*env,struct
_tagged_arr*x,struct _tuple6*v){struct _tuple10 _tmpE4;struct Cyc_Interface_Seen*
_tmpE5;struct Cyc_Dict_Dict*_tmpE6;int _tmpE7;struct Cyc_Interface_Ienv*_tmpE8;
struct Cyc_Interface_Ienv*_tmpE9;struct _tuple10*_tmpE3=env;_tmpE4=*_tmpE3;_LLC3:
_tmpE9=_tmpE4.f1;goto _LLC2;_LLC2: _tmpE8=_tmpE4.f2;goto _LLC1;_LLC1: _tmpE7=_tmpE4.f3;
goto _LLC0;_LLC0: _tmpE6=_tmpE4.f4;goto _LLBF;_LLBF: _tmpE5=_tmpE4.f5;goto _LLBE;
_LLBE: {void*_tmpEA=(*v).f1;void*_tmpEB=_tmpEA;void*_tmpEC;_LLC5: if(*((int*)
_tmpEB)== 0){_LLC9: _tmpEC=(void*)((struct Cyc_Tcenv_VarRes_struct*)_tmpEB)->f1;
goto _LLC6;}else{goto _LLC7;}_LLC7: goto _LLC8;_LLC6:{void*_tmpED=_tmpEC;struct Cyc_Absyn_Fndecl*
_tmpEE;struct Cyc_Absyn_Vardecl*_tmpEF;_LLCB: if((unsigned int)_tmpED > 1?*((int*)
_tmpED)== 1: 0){_LLD1: _tmpEE=((struct Cyc_Absyn_Funname_b_struct*)_tmpED)->f1;goto
_LLCC;}else{goto _LLCD;}_LLCD: if((unsigned int)_tmpED > 1?*((int*)_tmpED)== 0: 0){
_LLD2: _tmpEF=((struct Cyc_Absyn_Global_b_struct*)_tmpED)->f1;goto _LLCE;}else{goto
_LLCF;}_LLCF: goto _LLD0;_LLCC: {struct Cyc_Absyn_Vardecl*_tmpF0=({struct Cyc_Absyn_Vardecl*
_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->sc=(void*)((void*)_tmpEE->sc);_tmpF1->name=
_tmpEE->name;_tmpF1->tq=({struct Cyc_Absyn_Tqual _tmpF2;_tmpF2.q_const=0;_tmpF2.q_volatile=
0;_tmpF2.q_restrict=0;_tmpF2;});_tmpF1->type=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpEE->cached_typ))->v);_tmpF1->initializer=0;_tmpF1->rgn=0;_tmpF1->attributes=
0;_tmpF1->escapes=0;_tmpF1;});_tmpE7=0;_tmpEF=_tmpF0;goto _LLCE;}_LLCE: if(_tmpEF->initializer
!= 0){_tmpEF=({struct Cyc_Absyn_Vardecl*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[
0]=*_tmpEF;_tmpF3;});_tmpEF->initializer=0;}{void*_tmpF4=(void*)_tmpEF->sc;_LLD4:
if((int)_tmpF4 == 0){goto _LLD5;}else{goto _LLD6;}_LLD6: if((int)_tmpF4 == 1){goto
_LLD7;}else{goto _LLD8;}_LLD8: if((int)_tmpF4 == 2){goto _LLD9;}else{goto _LLDA;}
_LLDA: if((int)_tmpF4 == 4){goto _LLDB;}else{goto _LLDC;}_LLDC: if((int)_tmpF4 == 3){
goto _LLDD;}else{goto _LLD3;}_LLD5: if(_tmpE7? Cyc_Tcutil_is_function_type((void*)
_tmpEF->type): 0){Cyc_Interface_body_err(_tag_arr("static",sizeof(unsigned char),
7),_tmpEF->name);}goto _LLD3;_LLD7:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5[0]=({struct Cyc_Core_Invalid_argument_struct
_tmpF6;_tmpF6.tag=Cyc_Core_Invalid_argument;_tmpF6.f1=_tag_arr("extract_ordinarie",
sizeof(unsigned char),18);_tmpF6;});_tmpF5;}));_LLD9: if(_tmpE7? Cyc_Tcutil_is_function_type((
void*)_tmpEF->type): 0){Cyc_Interface_body_err(_tag_arr("public",sizeof(
unsigned char),7),_tmpEF->name);}if(Cyc_Interface_check_public_vardecl(_tmpE6,
_tmpE5,_tmpEF)){_tmpE8->vardecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,
struct _tagged_arr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpE8->vardecls,
x,_tmpEF);}goto _LLD3;_LLDB: goto _LLDD;_LLDD: if(Cyc_Interface_check_public_vardecl(
_tmpE6,_tmpE5,_tmpEF)){_tmpE9->vardecls=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k,struct Cyc_Absyn_Vardecl*v))Cyc_Dict_insert)(_tmpE9->vardecls,
x,_tmpEF);}goto _LLD3;_LLD3:;}goto _LLCA;_LLD0: goto _LLCA;_LLCA:;}goto _LLC4;_LLC8:
goto _LLC4;_LLC4:;}}struct _tuple11{struct Cyc_Interface_I*f1;int f2;struct Cyc_Dict_Dict*
f3;struct Cyc_Interface_Seen*f4;};static void Cyc_Interface_extract_f(struct
_tuple11*env_f,struct Cyc_List_List*ns,struct Cyc_Tcenv_Genv*ge){struct _tuple11
_tmpF8;struct Cyc_Interface_Seen*_tmpF9;struct Cyc_Dict_Dict*_tmpFA;int _tmpFB;
struct Cyc_Interface_I*_tmpFC;struct _tuple11*_tmpF7=env_f;_tmpF8=*_tmpF7;_LLE2:
_tmpFC=_tmpF8.f1;goto _LLE1;_LLE1: _tmpFB=_tmpF8.f2;goto _LLE0;_LLE0: _tmpFA=_tmpF8.f3;
goto _LLDF;_LLDF: _tmpF9=_tmpF8.f4;goto _LLDE;_LLDE: {struct _tuple10 _tmpFD=({struct
_tuple10 _tmpFE;_tmpFE.f1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmpFC->imports,ns);_tmpFE.f2=((struct
Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
_tmpFC->exports,ns);_tmpFE.f3=_tmpFB;_tmpFE.f4=_tmpFA;_tmpFE.f5=_tmpF9;_tmpFE.f6=
_tmpFC;_tmpFE;});((void(*)(void(*f)(struct _tuple10*,struct _tagged_arr*,struct Cyc_Absyn_Structdecl**),
struct _tuple10*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_extract_structdecl,&
_tmpFD,ge->structdecls);((void(*)(void(*f)(struct _tuple10*,struct _tagged_arr*,
struct Cyc_Absyn_Uniondecl**),struct _tuple10*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_extract_uniondecl,& _tmpFD,ge->uniondecls);((void(*)(void(*f)(
struct _tuple10*,struct _tagged_arr*,struct Cyc_Absyn_Tuniondecl**),struct _tuple10*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_extract_tuniondecl,&
_tmpFD,ge->tuniondecls);((void(*)(void(*f)(struct _tuple10*,struct _tagged_arr*,
struct Cyc_Absyn_Enumdecl**),struct _tuple10*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_extract_enumdecl,& _tmpFD,ge->enumdecls);((void(*)(void(*f)(struct
_tuple10*,struct _tagged_arr*,struct Cyc_Absyn_Typedefdecl*),struct _tuple10*env,
struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_extract_typedef,& _tmpFD,ge->typedefs);((
void(*)(void(*f)(struct _tuple10*,struct _tagged_arr*,struct _tuple6*),struct
_tuple10*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_extract_ordinarie,&
_tmpFD,ge->ordinaries);}}static struct Cyc_Interface_I*Cyc_Interface_gen_extract(
struct Cyc_Dict_Dict*ae,int check_complete_defs){struct _tuple11 _tmpFF=({struct
_tuple11 _tmp100;_tmp100.f1=((struct Cyc_Interface_I*(*)(struct Cyc_Dict_Dict*skel))
Cyc_Interface_skel2i)(ae);_tmp100.f2=check_complete_defs;_tmp100.f3=ae;_tmp100.f4=
Cyc_Interface_new_seen();_tmp100;});((void(*)(void(*f)(struct _tuple11*,struct Cyc_List_List*,
struct Cyc_Tcenv_Genv*),struct _tuple11*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_extract_f,& _tmpFF,ae);return _tmpFF.f1;}struct Cyc_Interface_I*Cyc_Interface_extract(
struct Cyc_Dict_Dict*ae){return Cyc_Interface_gen_extract(ae,1);}inline static void
Cyc_Interface_check_err(struct _tagged_arr*msg1,struct _tagged_arr msg2){({struct
Cyc_Std_String_pa_struct _tmp102;_tmp102.tag=0;_tmp102.f1=(struct _tagged_arr)msg2;{
void*_tmp101[1]={& _tmp102};Cyc_Tcdecl_merr(0,msg1,_tag_arr("%s",sizeof(
unsigned char),3),_tag_arr(_tmp101,sizeof(void*),1));}});}struct _tuple12{int f1;
struct Cyc_Dict_Dict*f2;int(*f3)(void*,void*,struct _tagged_arr*);struct
_tagged_arr f4;struct _tagged_arr*f5;};static void Cyc_Interface_incl_dict_f(struct
_tuple12*env,struct _tagged_arr*x,void*y1){struct _tuple12 _tmp104;struct
_tagged_arr*_tmp105;struct _tagged_arr _tmp106;int(*_tmp107)(void*,void*,struct
_tagged_arr*);struct Cyc_Dict_Dict*_tmp108;int _tmp109;int*_tmp10A;struct _tuple12*
_tmp103=env;_tmp104=*_tmp103;_LLE8: _tmp109=_tmp104.f1;_tmp10A=(int*)&(*_tmp103).f1;
goto _LLE7;_LLE7: _tmp108=_tmp104.f2;goto _LLE6;_LLE6: _tmp107=_tmp104.f3;goto _LLE5;
_LLE5: _tmp106=_tmp104.f4;goto _LLE4;_LLE4: _tmp105=_tmp104.f5;goto _LLE3;_LLE3: {
struct _handler_cons _tmp10B;_push_handler(& _tmp10B);{int _tmp10D=0;if(setjmp(
_tmp10B.handler)){_tmp10D=1;}if(! _tmp10D){{void*_tmp10E=((void*(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp108,x);if(! _tmp107(y1,_tmp10E,
_tmp105)){*_tmp10A=0;}};_pop_handler();}else{void*_tmp10C=(void*)_exn_thrown;
void*_tmp110=_tmp10C;_LLEA: if(_tmp110 == Cyc_Dict_Absent){goto _LLEB;}else{goto
_LLEC;}_LLEC: goto _LLED;_LLEB: Cyc_Interface_check_err(_tmp105,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp113;_tmp113.tag=0;_tmp113.f1=(struct
_tagged_arr)*x;{struct Cyc_Std_String_pa_struct _tmp112;_tmp112.tag=0;_tmp112.f1=(
struct _tagged_arr)_tmp106;{void*_tmp111[2]={& _tmp112,& _tmp113};Cyc_Std_aprintf(
_tag_arr("%s %s is missing",sizeof(unsigned char),17),_tag_arr(_tmp111,sizeof(
void*),2));}}}));*_tmp10A=0;goto _LLE9;_LLED:(void)_throw(_tmp110);_LLE9:;}}}}
static int Cyc_Interface_incl_dict(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,int(*incl_f)(void*,void*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg){struct _tuple12 _tmp114=({struct _tuple12 _tmp115;_tmp115.f1=1;
_tmp115.f2=dic2;_tmp115.f3=incl_f;_tmp115.f4=t;_tmp115.f5=msg;_tmp115;});((void(*)(
void(*f)(struct _tuple12*,struct _tagged_arr*,void*),struct _tuple12*env,struct Cyc_Dict_Dict*
d))Cyc_Dict_iter_c)(Cyc_Interface_incl_dict_f,& _tmp114,dic1);return _tmp114.f1;}
static int Cyc_Interface_incl_structdecl(struct Cyc_Absyn_Structdecl*d0,struct Cyc_Absyn_Structdecl*
d1,struct _tagged_arr*msg){struct Cyc_Absyn_Structdecl*_tmp116=Cyc_Tcdecl_merge_structdecl(
d0,d1,0,msg);if(_tmp116 == 0){return 0;}if((struct Cyc_Absyn_Structdecl*)d0 != 
_tmp116){Cyc_Interface_check_err(msg,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp118;_tmp118.tag=0;_tmp118.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string((
struct _tuple0*)((struct Cyc_Core_Opt*)_check_null(d1->name))->v);{void*_tmp117[1]={&
_tmp118};Cyc_Std_aprintf(_tag_arr("declaration of struct %s discloses too much information",
sizeof(unsigned char),56),_tag_arr(_tmp117,sizeof(void*),1));}}));return 0;}
return 1;}static int Cyc_Interface_incl_uniondecl(struct Cyc_Absyn_Uniondecl*d0,
struct Cyc_Absyn_Uniondecl*d1,struct _tagged_arr*msg){struct Cyc_Absyn_Uniondecl*
_tmp119=Cyc_Tcdecl_merge_uniondecl(d0,d1,0,msg);if(_tmp119 == 0){return 0;}if((
struct Cyc_Absyn_Uniondecl*)d0 != _tmp119){Cyc_Interface_check_err(msg,(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp11B;_tmp11B.tag=0;_tmp11B.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string((struct _tuple0*)((struct Cyc_Core_Opt*)
_check_null(d1->name))->v);{void*_tmp11A[1]={& _tmp11B};Cyc_Std_aprintf(_tag_arr("declaration of union %s discloses too much information",
sizeof(unsigned char),55),_tag_arr(_tmp11A,sizeof(void*),1));}}));return 0;}
return 1;}static int Cyc_Interface_incl_tuniondecl(struct Cyc_Absyn_Tuniondecl*d0,
struct Cyc_Absyn_Tuniondecl*d1,struct _tagged_arr*msg){struct Cyc_Absyn_Tuniondecl*
_tmp11C=Cyc_Tcdecl_merge_tuniondecl(d0,d1,0,msg);if(_tmp11C == 0){return 0;}if((
struct Cyc_Absyn_Tuniondecl*)d0 != _tmp11C){Cyc_Interface_check_err(msg,(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp11E;_tmp11E.tag=0;_tmp11E.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(d1->name);{void*_tmp11D[1]={& _tmp11E};
Cyc_Std_aprintf(_tag_arr("declaration of tunion %s discloses too much information",
sizeof(unsigned char),56),_tag_arr(_tmp11D,sizeof(void*),1));}}));return 0;}
return 1;}static int Cyc_Interface_incl_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct
Cyc_Absyn_Enumdecl*d1,struct _tagged_arr*msg){struct Cyc_Absyn_Enumdecl*_tmp11F=
Cyc_Tcdecl_merge_enumdecl(d0,d1,0,msg);if(_tmp11F == 0){return 0;}if((struct Cyc_Absyn_Enumdecl*)
d0 != _tmp11F){Cyc_Interface_check_err(msg,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp121;_tmp121.tag=0;_tmp121.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(d1->name);{
void*_tmp120[1]={& _tmp121};Cyc_Std_aprintf(_tag_arr("declaration of enum %s discloses too much information",
sizeof(unsigned char),54),_tag_arr(_tmp120,sizeof(void*),1));}}));return 0;}
return 1;}static int Cyc_Interface_incl_vardecl(struct Cyc_Absyn_Vardecl*d0,struct
Cyc_Absyn_Vardecl*d1,struct _tagged_arr*msg){struct Cyc_Absyn_Vardecl*_tmp122=Cyc_Tcdecl_merge_vardecl(
d0,d1,0,msg);if(_tmp122 == 0){return 0;}if((struct Cyc_Absyn_Vardecl*)d0 != _tmp122){
Cyc_Interface_check_err(msg,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp124;_tmp124.tag=0;_tmp124.f1=(struct _tagged_arr)Cyc_Absynpp_qvar2string(d1->name);{
void*_tmp123[1]={& _tmp124};Cyc_Std_aprintf(_tag_arr("declaration of variable %s discloses too much information",
sizeof(unsigned char),58),_tag_arr(_tmp123,sizeof(void*),1));}}));return 0;}
return 1;}static int Cyc_Interface_incl_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,
struct Cyc_Absyn_Typedefdecl*d1,struct _tagged_arr*msg){struct Cyc_Absyn_Typedefdecl*
_tmp125=Cyc_Tcdecl_merge_typedefdecl(d0,d1,0,msg);if(_tmp125 == 0){return 0;}if((
struct Cyc_Absyn_Typedefdecl*)d0 != _tmp125){Cyc_Interface_check_err(msg,(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp127;_tmp127.tag=0;_tmp127.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string(d1->name);{void*_tmp126[1]={& _tmp127};
Cyc_Std_aprintf(_tag_arr("declaration of typedef %s discloses too much information",
sizeof(unsigned char),57),_tag_arr(_tmp126,sizeof(void*),1));}}));return 0;}
return 1;}static int Cyc_Interface_incl_xtunionfielddecl(struct Cyc_Tcdecl_Xtunionfielddecl*
d0,struct Cyc_Tcdecl_Xtunionfielddecl*d1,struct _tagged_arr*msg){struct Cyc_Tcdecl_Xtunionfielddecl*
_tmp128=Cyc_Tcdecl_merge_xtunionfielddecl(d0,d1,0,msg);if(_tmp128 == 0){return 0;}
if((struct Cyc_Tcdecl_Xtunionfielddecl*)d0 != _tmp128){Cyc_Interface_check_err(msg,(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp12A;_tmp12A.tag=0;_tmp12A.f1=(
struct _tagged_arr)Cyc_Absynpp_qvar2string((d1->field)->name);{void*_tmp129[1]={&
_tmp12A};Cyc_Std_aprintf(_tag_arr("declaration of xtunionfield %s discloses too much information",
sizeof(unsigned char),62),_tag_arr(_tmp129,sizeof(void*),1));}}));return 0;}
return 1;}struct Cyc_Core_Opt*Cyc_Interface_compat_merge_dict(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,void*(*merge_f)(void*,
void*,struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg);static int Cyc_Interface_incl_ienv(struct Cyc_Interface_Ienv*ie1,
struct Cyc_Interface_Ienv*ie2,struct _tagged_arr*msg){int _tmp12B=((int(*)(struct
Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_incl_dict)(ie1->structdecls,ie2->structdecls,Cyc_Interface_incl_structdecl,
_tag_arr("struct",sizeof(unsigned char),7),msg);int _tmp12C=((int(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Absyn_Uniondecl*,struct Cyc_Absyn_Uniondecl*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_incl_dict)(
ie1->uniondecls,ie2->uniondecls,Cyc_Interface_incl_uniondecl,_tag_arr("union",
sizeof(unsigned char),6),msg);int _tmp12D=((int(*)(struct Cyc_Dict_Dict*dic1,
struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Absyn_Tuniondecl*,struct Cyc_Absyn_Tuniondecl*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_incl_dict)(
ie1->tuniondecls,ie2->tuniondecls,Cyc_Interface_incl_tuniondecl,_tag_arr("tunion",
sizeof(unsigned char),7),msg);int _tmp12E=((int(*)(struct Cyc_Dict_Dict*dic1,
struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_incl_dict)(
ie1->enumdecls,ie2->enumdecls,Cyc_Interface_incl_enumdecl,_tag_arr("enum",
sizeof(unsigned char),5),msg);int _tmp12F=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Typedefdecl*(*
merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_strptrcmp),Cyc_Tcdecl_merge_typedefdecl,
_tag_arr("typedef",sizeof(unsigned char),8),msg)!= 0;int _tmp130=((int(*)(struct
Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_incl_dict)(ie1->vardecls,ie2->vardecls,Cyc_Interface_incl_vardecl,
_tag_arr("variable",sizeof(unsigned char),9),msg);int _tmp131=((int(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,int(*incl_f)(struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Tcdecl_Xtunionfielddecl*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_incl_dict)(ie1->xtunionfielddecls,ie2->xtunionfielddecls,
Cyc_Interface_incl_xtunionfielddecl,_tag_arr("xtunionfield",sizeof(unsigned char),
13),msg);return(((((_tmp12B? _tmp12C: 0)? _tmp12D: 0)? _tmp12E: 0)? _tmp12F: 0)? _tmp130:
0)? _tmp131: 0;}struct _tuple13{int f1;struct Cyc_Dict_Dict*f2;struct _tagged_arr*f3;}
;static void Cyc_Interface_incl_ns_f(struct _tuple13*env,struct Cyc_List_List*ns,
struct Cyc_Interface_Ienv*ie1){struct _tuple13 _tmp133;struct _tagged_arr*_tmp134;
struct Cyc_Dict_Dict*_tmp135;int _tmp136;int*_tmp137;struct _tuple13*_tmp132=env;
_tmp133=*_tmp132;_LLF1: _tmp136=_tmp133.f1;_tmp137=(int*)&(*_tmp132).f1;goto _LLF0;
_LLF0: _tmp135=_tmp133.f2;goto _LLEF;_LLEF: _tmp134=_tmp133.f3;goto _LLEE;_LLEE: {
struct Cyc_Interface_Ienv*ie2;{struct _handler_cons _tmp138;_push_handler(& _tmp138);{
int _tmp13A=0;if(setjmp(_tmp138.handler)){_tmp13A=1;}if(! _tmp13A){ie2=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp135,ns);;
_pop_handler();}else{void*_tmp139=(void*)_exn_thrown;void*_tmp13C=_tmp139;_LLF3:
if(_tmp13C == Cyc_Dict_Absent){goto _LLF4;}else{goto _LLF5;}_LLF5: goto _LLF6;_LLF4:
ie2=Cyc_Interface_lazy_new_ienv();goto _LLF2;_LLF6:(void)_throw(_tmp13C);_LLF2:;}}}
if(! Cyc_Interface_incl_ienv(ie1,ie2,_tmp134)){*_tmp137=0;}}}static int Cyc_Interface_incl_ns(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr*msg){struct
_tuple13 _tmp13D=({struct _tuple13 _tmp13E;_tmp13E.f1=1;_tmp13E.f2=dic2;_tmp13E.f3=
msg;_tmp13E;});((void(*)(void(*f)(struct _tuple13*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct _tuple13*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_incl_ns_f,&
_tmp13D,dic1);return _tmp13D.f1;}int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*
i1,struct Cyc_Interface_I*i2,struct _tuple3*info){struct _handler_cons _tmp13F;
_push_handler(& _tmp13F);{int _tmp141=0;if(setjmp(_tmp13F.handler)){_tmp141=1;}if(
! _tmp141){{int _tmp142=1;struct _tagged_arr*msg=0;if(info != 0){msg=({struct
_tagged_arr*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143[0]=(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp146;_tmp146.tag=0;_tmp146.f1=(struct
_tagged_arr)(*info).f2;{struct Cyc_Std_String_pa_struct _tmp145;_tmp145.tag=0;
_tmp145.f1=(struct _tagged_arr)(*info).f1;{void*_tmp144[2]={& _tmp145,& _tmp146};
Cyc_Std_aprintf(_tag_arr("checking inclusion of %s exports into %s exports,",
sizeof(unsigned char),50),_tag_arr(_tmp144,sizeof(void*),2));}}});_tmp143;});}
if(! Cyc_Interface_incl_ns(i1->exports,i2->exports,msg)){_tmp142=0;}if(info != 0){
msg=({struct _tagged_arr*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147[0]=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp14A;_tmp14A.tag=0;_tmp14A.f1=(
struct _tagged_arr)(*info).f1;{struct Cyc_Std_String_pa_struct _tmp149;_tmp149.tag=
0;_tmp149.f1=(struct _tagged_arr)(*info).f2;{void*_tmp148[2]={& _tmp149,& _tmp14A};
Cyc_Std_aprintf(_tag_arr("checking inclusion of %s imports into %s imports,",
sizeof(unsigned char),50),_tag_arr(_tmp148,sizeof(void*),2));}}});_tmp147;});}
if(! Cyc_Interface_incl_ns(i2->imports,i1->imports,msg)){_tmp142=0;}{int _tmp14B=
_tmp142;_npop_handler(0);return _tmp14B;}};_pop_handler();}else{void*_tmp140=(
void*)_exn_thrown;void*_tmp14D=_tmp140;_LLF8: if(_tmp14D == Cyc_Tcdecl_Incompatible){
goto _LLF9;}else{goto _LLFA;}_LLFA: goto _LLFB;_LLF9: return 0;_LLFB:(void)_throw(
_tmp14D);_LLF7:;}}}struct _tuple14{int f1;struct Cyc_Dict_Dict*f2;struct Cyc_Dict_Dict*
f3;struct Cyc_Dict_Dict*f4;void*(*f5)(void*,void*,struct Cyc_Position_Segment*,
struct _tagged_arr*);struct _tagged_arr f6;struct _tagged_arr*f7;};void Cyc_Interface_compat_merge_dict_f(
struct _tuple14*env,struct _tagged_arr*x,void*y2){struct _tuple14 _tmp14F;struct
_tagged_arr*_tmp150;struct _tagged_arr _tmp151;void*(*_tmp152)(void*,void*,struct
Cyc_Position_Segment*,struct _tagged_arr*);struct Cyc_Dict_Dict*_tmp153;struct Cyc_Dict_Dict*
_tmp154;struct Cyc_Dict_Dict*_tmp155;struct Cyc_Dict_Dict**_tmp156;int _tmp157;int*
_tmp158;struct _tuple14*_tmp14E=env;_tmp14F=*_tmp14E;_LL103: _tmp157=_tmp14F.f1;
_tmp158=(int*)&(*_tmp14E).f1;goto _LL102;_LL102: _tmp155=_tmp14F.f2;_tmp156=(
struct Cyc_Dict_Dict**)&(*_tmp14E).f2;goto _LL101;_LL101: _tmp154=_tmp14F.f3;goto
_LL100;_LL100: _tmp153=_tmp14F.f4;goto _LLFF;_LLFF: _tmp152=_tmp14F.f5;goto _LLFE;
_LLFE: _tmp151=_tmp14F.f6;goto _LLFD;_LLFD: _tmp150=_tmp14F.f7;goto _LLFC;_LLFC: {
void*y;{struct _handler_cons _tmp159;_push_handler(& _tmp159);{int _tmp15B=0;if(
setjmp(_tmp159.handler)){_tmp15B=1;}if(! _tmp15B){{void*_tmp15C=((void*(*)(struct
Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(_tmp154,x);void*_tmp15D=
_tmp152(_tmp15C,y2,0,_tmp150);if(!((unsigned int)_tmp15D)){*_tmp158=0;
_npop_handler(0);return;}y=(void*)_check_null(_tmp15D);};_pop_handler();}else{
void*_tmp15A=(void*)_exn_thrown;void*_tmp15F=_tmp15A;_LL105: if(_tmp15F == Cyc_Dict_Absent){
goto _LL106;}else{goto _LL107;}_LL107: goto _LL108;_LL106: y=y2;goto _LL104;_LL108:(
void)_throw(_tmp15F);_LL104:;}}}{struct _handler_cons _tmp160;_push_handler(&
_tmp160);{int _tmp162=0;if(setjmp(_tmp160.handler)){_tmp162=1;}if(! _tmp162){{void*
_tmp163=((void*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k))Cyc_Dict_lookup)(
_tmp153,x);void*_tmp164=_tmp152(_tmp163,y,0,_tmp150);if(_tmp164 != (void*)_tmp163){
if((unsigned int)_tmp164){Cyc_Interface_check_err(_tmp150,(struct _tagged_arr)({
struct Cyc_Std_String_pa_struct _tmp167;_tmp167.tag=0;_tmp167.f1=(struct
_tagged_arr)*x;{struct Cyc_Std_String_pa_struct _tmp166;_tmp166.tag=0;_tmp166.f1=(
struct _tagged_arr)_tmp151;{void*_tmp165[2]={& _tmp166,& _tmp167};Cyc_Std_aprintf(
_tag_arr("abstract %s %s is being imported as non-abstract",sizeof(unsigned char),
49),_tag_arr(_tmp165,sizeof(void*),2));}}}));}*_tmp158=0;}};_pop_handler();}
else{void*_tmp161=(void*)_exn_thrown;void*_tmp169=_tmp161;_LL10A: if(_tmp169 == 
Cyc_Dict_Absent){goto _LL10B;}else{goto _LL10C;}_LL10C: goto _LL10D;_LL10B: if(*
_tmp158){*_tmp156=((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct
_tagged_arr*k,void*v))Cyc_Dict_insert)(*_tmp156,x,y);}goto _LL109;_LL10D:(void)
_throw(_tmp169);_LL109:;}}}}}struct Cyc_Core_Opt*Cyc_Interface_compat_merge_dict(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,void*(*
merge_f)(void*,void*,struct Cyc_Position_Segment*,struct _tagged_arr*),struct
_tagged_arr t,struct _tagged_arr*msg){struct _tuple14 _tmp16A=({struct _tuple14
_tmp16C;_tmp16C.f1=1;_tmp16C.f2=dic1;_tmp16C.f3=dic1;_tmp16C.f4=excl;_tmp16C.f5=
merge_f;_tmp16C.f6=t;_tmp16C.f7=msg;_tmp16C;});((void(*)(void(*f)(struct _tuple14*,
struct _tagged_arr*,void*),struct _tuple14*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_compat_merge_dict_f,& _tmp16A,dic2);if(_tmp16A.f1){return({struct
Cyc_Core_Opt*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->v=_tmp16A.f2;_tmp16B;});}
else{return 0;}}struct Cyc_Interface_Ienv*Cyc_Interface_compat_merge_ienv(struct
Cyc_Interface_Ienv*ie1,struct Cyc_Interface_Ienv*ie2,struct Cyc_Interface_Ienv*
iexcl,struct _tagged_arr*msg){struct Cyc_Core_Opt*_tmp16D=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,
struct Cyc_Absyn_Structdecl*(*merge_f)(struct Cyc_Absyn_Structdecl*,struct Cyc_Absyn_Structdecl*,
struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->structdecls,ie2->structdecls,
iexcl->structdecls,Cyc_Tcdecl_merge_structdecl,_tag_arr("struct",sizeof(
unsigned char),7),msg);struct Cyc_Core_Opt*_tmp16E=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,
struct Cyc_Absyn_Uniondecl*(*merge_f)(struct Cyc_Absyn_Uniondecl*,struct Cyc_Absyn_Uniondecl*,
struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->uniondecls,ie2->uniondecls,
iexcl->uniondecls,Cyc_Tcdecl_merge_uniondecl,_tag_arr("union",sizeof(
unsigned char),6),msg);struct Cyc_Core_Opt*_tmp16F=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,
struct Cyc_Absyn_Tuniondecl*(*merge_f)(struct Cyc_Absyn_Tuniondecl*,struct Cyc_Absyn_Tuniondecl*,
struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->tuniondecls,ie2->tuniondecls,
iexcl->tuniondecls,Cyc_Tcdecl_merge_tuniondecl,_tag_arr("tunion",sizeof(
unsigned char),7),msg);struct Cyc_Core_Opt*_tmp170=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,
struct Cyc_Absyn_Enumdecl*(*merge_f)(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,
struct Cyc_Position_Segment*,struct _tagged_arr*),struct _tagged_arr t,struct
_tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->enumdecls,ie2->enumdecls,
iexcl->enumdecls,Cyc_Tcdecl_merge_enumdecl,_tag_arr("enum",sizeof(unsigned char),
5),msg);struct Cyc_Core_Opt*_tmp171=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Typedefdecl*(*
merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,iexcl->typedefdecls,Cyc_Tcdecl_merge_typedefdecl,
_tag_arr("typedef",sizeof(unsigned char),8),msg);struct Cyc_Core_Opt*_tmp172=((
struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct
Cyc_Dict_Dict*excl,struct Cyc_Absyn_Vardecl*(*merge_f)(struct Cyc_Absyn_Vardecl*,
struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*,struct _tagged_arr*),struct
_tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->vardecls,
ie2->vardecls,iexcl->vardecls,Cyc_Tcdecl_merge_vardecl,_tag_arr("variable",
sizeof(unsigned char),9),msg);struct Cyc_Core_Opt*_tmp173=((struct Cyc_Core_Opt*(*)(
struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,
struct Cyc_Tcdecl_Xtunionfielddecl*(*merge_f)(struct Cyc_Tcdecl_Xtunionfielddecl*,
struct Cyc_Tcdecl_Xtunionfielddecl*,struct Cyc_Position_Segment*,struct _tagged_arr*),
struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->xtunionfielddecls,
ie2->xtunionfielddecls,iexcl->xtunionfielddecls,Cyc_Tcdecl_merge_xtunionfielddecl,
_tag_arr("xtunionfield",sizeof(unsigned char),13),msg);if((((((!((unsigned int)
_tmp16D)? 1: !((unsigned int)_tmp16E))? 1: !((unsigned int)_tmp16F))? 1: !((
unsigned int)_tmp170))? 1: !((unsigned int)_tmp171))? 1: !((unsigned int)_tmp172))? 1:
!((unsigned int)_tmp173)){return 0;}else{return({struct Cyc_Interface_Ienv*_tmp174=
_cycalloc(sizeof(*_tmp174));_tmp174->structdecls=(struct Cyc_Dict_Dict*)_tmp16D->v;
_tmp174->uniondecls=(struct Cyc_Dict_Dict*)_tmp16E->v;_tmp174->tuniondecls=(
struct Cyc_Dict_Dict*)_tmp16F->v;_tmp174->enumdecls=(struct Cyc_Dict_Dict*)_tmp170->v;
_tmp174->typedefdecls=(struct Cyc_Dict_Dict*)_tmp171->v;_tmp174->vardecls=(struct
Cyc_Dict_Dict*)_tmp172->v;_tmp174->xtunionfielddecls=(struct Cyc_Dict_Dict*)
_tmp173->v;_tmp174;});}}struct _tuple15{int f1;struct Cyc_Dict_Dict*f2;struct Cyc_Dict_Dict*
f3;struct Cyc_Dict_Dict*f4;struct _tagged_arr*f5;};void Cyc_Interface_compat_merge_ns_f(
struct _tuple15*env,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie2){struct
_tuple15 _tmp176;struct _tagged_arr*_tmp177;struct Cyc_Dict_Dict*_tmp178;struct Cyc_Dict_Dict*
_tmp179;struct Cyc_Dict_Dict*_tmp17A;struct Cyc_Dict_Dict**_tmp17B;int _tmp17C;int*
_tmp17D;struct _tuple15*_tmp175=env;_tmp176=*_tmp175;_LL113: _tmp17C=_tmp176.f1;
_tmp17D=(int*)&(*_tmp175).f1;goto _LL112;_LL112: _tmp17A=_tmp176.f2;_tmp17B=(
struct Cyc_Dict_Dict**)&(*_tmp175).f2;goto _LL111;_LL111: _tmp179=_tmp176.f3;goto
_LL110;_LL110: _tmp178=_tmp176.f4;goto _LL10F;_LL10F: _tmp177=_tmp176.f5;goto _LL10E;
_LL10E: {struct Cyc_Interface_Ienv*ie1;struct Cyc_Interface_Ienv*iexcl;{struct
_handler_cons _tmp17E;_push_handler(& _tmp17E);{int _tmp180=0;if(setjmp(_tmp17E.handler)){
_tmp180=1;}if(! _tmp180){iexcl=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp178,ns);;_pop_handler();}else{void*
_tmp17F=(void*)_exn_thrown;void*_tmp182=_tmp17F;_LL115: if(_tmp182 == Cyc_Dict_Absent){
goto _LL116;}else{goto _LL117;}_LL117: goto _LL118;_LL116: iexcl=Cyc_Interface_lazy_new_ienv();
goto _LL114;_LL118:(void)_throw(_tmp182);_LL114:;}}}{struct _handler_cons _tmp183;
_push_handler(& _tmp183);{int _tmp185=0;if(setjmp(_tmp183.handler)){_tmp185=1;}if(
! _tmp185){ie1=((struct Cyc_Interface_Ienv*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*
k))Cyc_Dict_lookup)(_tmp179,ns);{struct Cyc_Interface_Ienv*_tmp186=Cyc_Interface_compat_merge_ienv(
Cyc_Interface_lazy_new_ienv(),ie1,iexcl,_tmp177);if(_tmp186 == 0){*_tmp17D=0;
_npop_handler(0);return;}ie1=(struct Cyc_Interface_Ienv*)_check_null(_tmp186);};
_pop_handler();}else{void*_tmp184=(void*)_exn_thrown;void*_tmp188=_tmp184;_LL11A:
if(_tmp188 == Cyc_Dict_Absent){goto _LL11B;}else{goto _LL11C;}_LL11C: goto _LL11D;
_LL11B: ie1=Cyc_Interface_lazy_new_ienv();goto _LL119;_LL11D:(void)_throw(_tmp188);
_LL119:;}}}{struct Cyc_Interface_Ienv*_tmp189=Cyc_Interface_compat_merge_ienv(ie1,
ie2,iexcl,_tmp177);if(_tmp189 == 0){*_tmp17D=0;}else{if(*_tmp17D){*_tmp17B=((
struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*
v))Cyc_Dict_insert)(*_tmp17B,ns,(struct Cyc_Interface_Ienv*)_check_null(_tmp189));}}}}}
struct Cyc_Core_Opt*Cyc_Interface_compat_merge_ns(struct Cyc_Dict_Dict*dic1,struct
Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct _tagged_arr*msg){struct Cyc_Dict_Dict*
d=((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))
Cyc_Dict_empty)(Cyc_Absyn_varlist_cmp);struct _tuple15 _tmp18A=({struct _tuple15
_tmp18C;_tmp18C.f1=1;_tmp18C.f2=d;_tmp18C.f3=dic1;_tmp18C.f4=excl;_tmp18C.f5=msg;
_tmp18C;});((void(*)(void(*f)(struct _tuple15*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),
struct _tuple15*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_compat_merge_ns_f,&
_tmp18A,dic2);if(_tmp18A.f1){return({struct Cyc_Core_Opt*_tmp18B=_cycalloc(
sizeof(*_tmp18B));_tmp18B->v=_tmp18A.f2;_tmp18B;});}else{return 0;}}struct
_tuple16{int f1;struct Cyc_Dict_Dict*f2;struct Cyc_Dict_Dict*f3;struct _tagged_arr f4;
struct _tagged_arr*f5;};void Cyc_Interface_disj_merge_dict_f(struct _tuple16*env,
struct _tagged_arr*x,void*y){struct _tuple16 _tmp18E;struct _tagged_arr*_tmp18F;
struct _tagged_arr _tmp190;struct Cyc_Dict_Dict*_tmp191;struct Cyc_Dict_Dict*_tmp192;
struct Cyc_Dict_Dict**_tmp193;int _tmp194;int*_tmp195;struct _tuple16*_tmp18D=env;
_tmp18E=*_tmp18D;_LL123: _tmp194=_tmp18E.f1;_tmp195=(int*)&(*_tmp18D).f1;goto
_LL122;_LL122: _tmp192=_tmp18E.f2;_tmp193=(struct Cyc_Dict_Dict**)&(*_tmp18D).f2;
goto _LL121;_LL121: _tmp191=_tmp18E.f3;goto _LL120;_LL120: _tmp190=_tmp18E.f4;goto
_LL11F;_LL11F: _tmp18F=_tmp18E.f5;goto _LL11E;_LL11E: if(((int(*)(struct Cyc_Dict_Dict*
d,struct _tagged_arr*k))Cyc_Dict_member)(_tmp191,x)){Cyc_Interface_check_err(
_tmp18F,(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp198;_tmp198.tag=0;
_tmp198.f1=(struct _tagged_arr)*x;{struct Cyc_Std_String_pa_struct _tmp197;_tmp197.tag=
0;_tmp197.f1=(struct _tagged_arr)_tmp190;{void*_tmp196[2]={& _tmp197,& _tmp198};Cyc_Std_aprintf(
_tag_arr("%s %s is exported more than once",sizeof(unsigned char),33),_tag_arr(
_tmp196,sizeof(void*),2));}}}));*_tmp195=0;}else{if(*_tmp195){*_tmp193=((struct
Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*d,struct _tagged_arr*k,void*v))Cyc_Dict_insert)(*
_tmp193,x,y);}}}struct Cyc_Core_Opt*Cyc_Interface_disj_merge_dict(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr t,struct _tagged_arr*msg){struct
_tuple16 _tmp199=({struct _tuple16 _tmp19B;_tmp19B.f1=1;_tmp19B.f2=dic1;_tmp19B.f3=
dic1;_tmp19B.f4=t;_tmp19B.f5=msg;_tmp19B;});((void(*)(void(*f)(struct _tuple16*,
struct _tagged_arr*,void*),struct _tuple16*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(
Cyc_Interface_disj_merge_dict_f,& _tmp199,dic2);if(_tmp199.f1){return({struct Cyc_Core_Opt*
_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->v=_tmp199.f2;_tmp19A;});}else{
return 0;}}struct Cyc_Interface_Ienv*Cyc_Interface_disj_merge_ienv(struct Cyc_Interface_Ienv*
ie1,struct Cyc_Interface_Ienv*ie2,struct _tagged_arr*msg){struct Cyc_Core_Opt*
_tmp19C=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Structdecl*(*merge_f)(struct Cyc_Absyn_Structdecl*,
struct Cyc_Absyn_Structdecl*,struct Cyc_Position_Segment*,struct _tagged_arr*),
struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(ie1->structdecls,
ie2->structdecls,((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct _tagged_arr*,struct
_tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp),Cyc_Tcdecl_merge_structdecl,
_tag_arr("struct",sizeof(unsigned char),7),msg);struct Cyc_Core_Opt*_tmp19D=((
struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct
_tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_disj_merge_dict)(ie1->uniondecls,
ie2->uniondecls,_tag_arr("union",sizeof(unsigned char),6),msg);struct Cyc_Core_Opt*
_tmp19E=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_disj_merge_dict)(
ie1->tuniondecls,ie2->tuniondecls,_tag_arr("[x]tunion",sizeof(unsigned char),10),
msg);struct Cyc_Core_Opt*_tmp19F=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*
dic1,struct Cyc_Dict_Dict*dic2,struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_disj_merge_dict)(
ie1->enumdecls,ie2->enumdecls,_tag_arr("enum",sizeof(unsigned char),5),msg);
struct Cyc_Core_Opt*_tmp1A0=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,
struct Cyc_Dict_Dict*dic2,struct Cyc_Dict_Dict*excl,struct Cyc_Absyn_Typedefdecl*(*
merge_f)(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_Position_Segment*,
struct _tagged_arr*),struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_compat_merge_dict)(
ie1->typedefdecls,ie2->typedefdecls,((struct Cyc_Dict_Dict*(*)(int(*cmp)(struct
_tagged_arr*,struct _tagged_arr*)))Cyc_Dict_empty)(Cyc_Std_zstrptrcmp),Cyc_Tcdecl_merge_typedefdecl,
_tag_arr("typedef",sizeof(unsigned char),8),msg);struct Cyc_Core_Opt*_tmp1A1=((
struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,struct
_tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_disj_merge_dict)(ie1->vardecls,
ie2->vardecls,_tag_arr("variable",sizeof(unsigned char),9),msg);struct Cyc_Core_Opt*
_tmp1A2=((struct Cyc_Core_Opt*(*)(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*
dic2,struct _tagged_arr t,struct _tagged_arr*msg))Cyc_Interface_disj_merge_dict)(
ie1->xtunionfielddecls,ie2->xtunionfielddecls,_tag_arr("xtunionfield",sizeof(
unsigned char),13),msg);if((((((!((unsigned int)_tmp19C)? 1: !((unsigned int)
_tmp19D))? 1: !((unsigned int)_tmp19E))? 1: !((unsigned int)_tmp19F))? 1: !((
unsigned int)_tmp1A0))? 1: !((unsigned int)_tmp1A1))? 1: !((unsigned int)_tmp1A2)){
return 0;}else{return({struct Cyc_Interface_Ienv*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));
_tmp1A3->structdecls=(struct Cyc_Dict_Dict*)_tmp19C->v;_tmp1A3->uniondecls=(
struct Cyc_Dict_Dict*)_tmp19D->v;_tmp1A3->tuniondecls=(struct Cyc_Dict_Dict*)
_tmp19E->v;_tmp1A3->enumdecls=(struct Cyc_Dict_Dict*)_tmp19F->v;_tmp1A3->typedefdecls=(
struct Cyc_Dict_Dict*)_tmp1A0->v;_tmp1A3->vardecls=(struct Cyc_Dict_Dict*)_tmp1A1->v;
_tmp1A3->xtunionfielddecls=(struct Cyc_Dict_Dict*)_tmp1A2->v;_tmp1A3;});}}struct
_tuple17{int f1;struct Cyc_Dict_Dict*f2;struct Cyc_Dict_Dict*f3;struct _tagged_arr*
f4;};void Cyc_Interface_disj_merge_ns_f(struct _tuple17*env,struct Cyc_List_List*ns,
struct Cyc_Interface_Ienv*ie2){struct _tuple17 _tmp1A5;struct _tagged_arr*_tmp1A6;
struct Cyc_Dict_Dict*_tmp1A7;struct Cyc_Dict_Dict*_tmp1A8;struct Cyc_Dict_Dict**
_tmp1A9;int _tmp1AA;int*_tmp1AB;struct _tuple17*_tmp1A4=env;_tmp1A5=*_tmp1A4;
_LL128: _tmp1AA=_tmp1A5.f1;_tmp1AB=(int*)&(*_tmp1A4).f1;goto _LL127;_LL127: _tmp1A8=
_tmp1A5.f2;_tmp1A9=(struct Cyc_Dict_Dict**)&(*_tmp1A4).f2;goto _LL126;_LL126:
_tmp1A7=_tmp1A5.f3;goto _LL125;_LL125: _tmp1A6=_tmp1A5.f4;goto _LL124;_LL124: {
struct Cyc_Interface_Ienv*ie1;{struct _handler_cons _tmp1AC;_push_handler(& _tmp1AC);{
int _tmp1AE=0;if(setjmp(_tmp1AC.handler)){_tmp1AE=1;}if(! _tmp1AE){ie1=((struct Cyc_Interface_Ienv*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp1A7,ns);;
_pop_handler();}else{void*_tmp1AD=(void*)_exn_thrown;void*_tmp1B0=_tmp1AD;_LL12A:
if(_tmp1B0 == Cyc_Dict_Absent){goto _LL12B;}else{goto _LL12C;}_LL12C: goto _LL12D;
_LL12B: ie1=Cyc_Interface_lazy_new_ienv();goto _LL129;_LL12D:(void)_throw(_tmp1B0);
_LL129:;}}}{struct Cyc_Interface_Ienv*_tmp1B1=Cyc_Interface_disj_merge_ienv(ie1,
ie2,_tmp1A6);if(_tmp1B1 == 0){*_tmp1AB=0;}else{if(*_tmp1AB){*_tmp1A9=((struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict*d,struct Cyc_List_List*k,struct Cyc_Interface_Ienv*v))Cyc_Dict_insert)(*
_tmp1A9,ns,(struct Cyc_Interface_Ienv*)_check_null(_tmp1B1));}}}}}struct Cyc_Core_Opt*
Cyc_Interface_disj_merge_ns(struct Cyc_Dict_Dict*dic1,struct Cyc_Dict_Dict*dic2,
struct _tagged_arr*msg){struct _tuple17 _tmp1B2=({struct _tuple17 _tmp1B4;_tmp1B4.f1=
1;_tmp1B4.f2=dic1;_tmp1B4.f3=dic1;_tmp1B4.f4=msg;_tmp1B4;});((void(*)(void(*f)(
struct _tuple17*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct _tuple17*
env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_disj_merge_ns_f,&
_tmp1B2,dic2);if(_tmp1B2.f1){return({struct Cyc_Core_Opt*_tmp1B3=_cycalloc(
sizeof(*_tmp1B3));_tmp1B3->v=_tmp1B2.f2;_tmp1B3;});}else{return 0;}}struct Cyc_Interface_I*
Cyc_Interface_merge(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct
_tuple3*info){struct _handler_cons _tmp1B5;_push_handler(& _tmp1B5);{int _tmp1B7=0;
if(setjmp(_tmp1B5.handler)){_tmp1B7=1;}if(! _tmp1B7){{struct _tagged_arr*msg=0;if(
info != 0){msg=({struct _tagged_arr*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8[0]=(
struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp1BB;_tmp1BB.tag=0;_tmp1BB.f1=(
struct _tagged_arr)(*info).f2;{struct Cyc_Std_String_pa_struct _tmp1BA;_tmp1BA.tag=
0;_tmp1BA.f1=(struct _tagged_arr)(*info).f1;{void*_tmp1B9[2]={& _tmp1BA,& _tmp1BB};
Cyc_Std_aprintf(_tag_arr("merging exports of %s and %s,",sizeof(unsigned char),
30),_tag_arr(_tmp1B9,sizeof(void*),2));}}});_tmp1B8;});}{struct Cyc_Core_Opt*
_tmp1BC=Cyc_Interface_disj_merge_ns(i1->exports,i2->exports,msg);if(_tmp1BC == 0){
struct Cyc_Interface_I*_tmp1BD=0;_npop_handler(0);return _tmp1BD;}if(info != 0){msg=({
struct _tagged_arr*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));_tmp1BE[0]=(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp1C1;_tmp1C1.tag=0;_tmp1C1.f1=(
struct _tagged_arr)(*info).f2;{struct Cyc_Std_String_pa_struct _tmp1C0;_tmp1C0.tag=
0;_tmp1C0.f1=(struct _tagged_arr)(*info).f1;{void*_tmp1BF[2]={& _tmp1C0,& _tmp1C1};
Cyc_Std_aprintf(_tag_arr("merging imports of %s and %s,",sizeof(unsigned char),
30),_tag_arr(_tmp1BF,sizeof(void*),2));}}});_tmp1BE;});}{struct Cyc_Core_Opt*
_tmp1C2=Cyc_Interface_compat_merge_ns(i1->imports,i2->imports,(struct Cyc_Dict_Dict*)
_tmp1BC->v,msg);if(_tmp1C2 == 0){struct Cyc_Interface_I*_tmp1C3=0;_npop_handler(0);
return _tmp1C3;}{struct Cyc_Interface_I*_tmp1C5=({struct Cyc_Interface_I*_tmp1C4=
_cycalloc(sizeof(*_tmp1C4));_tmp1C4->imports=(struct Cyc_Dict_Dict*)_tmp1C2->v;
_tmp1C4->exports=(struct Cyc_Dict_Dict*)_tmp1BC->v;_tmp1C4;});_npop_handler(0);
return _tmp1C5;}}}};_pop_handler();}else{void*_tmp1B6=(void*)_exn_thrown;void*
_tmp1C7=_tmp1B6;_LL12F: if(_tmp1C7 == Cyc_Tcdecl_Incompatible){goto _LL130;}else{
goto _LL131;}_LL131: goto _LL132;_LL130: return 0;_LL132:(void)_throw(_tmp1C7);_LL12E:;}}}
struct Cyc_Interface_I*Cyc_Interface_merge_list(struct Cyc_List_List*li,struct Cyc_List_List*
linfo){if(li == 0){return(struct Cyc_Interface_I*)Cyc_Interface_empty();}{struct
Cyc_Interface_I*curr_i=(struct Cyc_Interface_I*)li->hd;struct _tagged_arr*
curr_info=linfo != 0?(struct _tagged_arr*)((struct _tagged_arr*)linfo->hd): 0;li=li->tl;
if(linfo != 0){linfo=linfo->tl;}for(0;li != 0;li=li->tl){struct Cyc_Interface_I*
_tmp1C8=Cyc_Interface_merge(curr_i,(struct Cyc_Interface_I*)li->hd,(curr_info != 0?
linfo != 0: 0)?({struct _tuple3*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->f1=*
curr_info;_tmp1CD->f2=*((struct _tagged_arr*)linfo->hd);_tmp1CD;}): 0);if(_tmp1C8
== 0){return 0;}curr_i=(struct Cyc_Interface_I*)_check_null(_tmp1C8);if(curr_info
!= 0){curr_info=linfo != 0?({struct _tagged_arr*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));
_tmp1C9[0]=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct _tmp1CC;_tmp1CC.tag=
0;_tmp1CC.f1=(struct _tagged_arr)*((struct _tagged_arr*)linfo->hd);{struct Cyc_Std_String_pa_struct
_tmp1CB;_tmp1CB.tag=0;_tmp1CB.f1=(struct _tagged_arr)*curr_info;{void*_tmp1CA[2]={&
_tmp1CB,& _tmp1CC};Cyc_Std_aprintf(_tag_arr("%s+%s",sizeof(unsigned char),6),
_tag_arr(_tmp1CA,sizeof(void*),2));}}});_tmp1C9;}): 0;}if(linfo != 0){linfo=linfo->tl;}}
return(struct Cyc_Interface_I*)curr_i;}}struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(
struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*
linfo){if(la == 0){return(struct Cyc_Interface_I*)Cyc_Interface_empty();}{struct
Cyc_Interface_I*curr_i=get((void*)la->hd);struct _tagged_arr*curr_info=linfo != 0?(
struct _tagged_arr*)((struct _tagged_arr*)linfo->hd): 0;la=la->tl;if(linfo != 0){
linfo=linfo->tl;}for(0;la != 0;la=la->tl){struct Cyc_Interface_I*_tmp1CE=Cyc_Interface_merge(
curr_i,get((void*)la->hd),(curr_info != 0? linfo != 0: 0)?({struct _tuple3*_tmp1D3=
_cycalloc(sizeof(*_tmp1D3));_tmp1D3->f1=*curr_info;_tmp1D3->f2=*((struct
_tagged_arr*)linfo->hd);_tmp1D3;}): 0);if(_tmp1CE == 0){return 0;}curr_i=(struct Cyc_Interface_I*)
_check_null(_tmp1CE);if(curr_info != 0){curr_info=linfo != 0?({struct _tagged_arr*
_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF[0]=(struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmp1D2;_tmp1D2.tag=0;_tmp1D2.f1=(struct _tagged_arr)*((struct _tagged_arr*)linfo->hd);{
struct Cyc_Std_String_pa_struct _tmp1D1;_tmp1D1.tag=0;_tmp1D1.f1=(struct
_tagged_arr)*curr_info;{void*_tmp1D0[2]={& _tmp1D1,& _tmp1D2};Cyc_Std_aprintf(
_tag_arr("%s+%s",sizeof(unsigned char),6),_tag_arr(_tmp1D0,sizeof(void*),2));}}});
_tmp1CF;}): 0;}if(linfo != 0){linfo=linfo->tl;}}return(struct Cyc_Interface_I*)
curr_i;}}static struct Cyc_List_List*Cyc_Interface_add_namespace(struct Cyc_List_List*
tds,struct Cyc_List_List*ns){if(ns == 0){return tds;}return({struct Cyc_List_List*
_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Namespace_d_struct*_tmp1D5=_cycalloc(sizeof(*_tmp1D5));_tmp1D5[0]=({
struct Cyc_Absyn_Namespace_d_struct _tmp1D6;_tmp1D6.tag=9;_tmp1D6.f1=(struct
_tagged_arr*)ns->hd;_tmp1D6.f2=Cyc_Interface_add_namespace(tds,ns->tl);_tmp1D6;});
_tmp1D5;}),0);_tmp1D4->tl=0;_tmp1D4;});}static struct Cyc_List_List*Cyc_Interface_add_structdecl(
struct _tagged_arr*x,struct Cyc_Absyn_Structdecl*d,struct Cyc_List_List*tds){return({
struct Cyc_List_List*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Struct_d_struct*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));
_tmp1D8[0]=({struct Cyc_Absyn_Struct_d_struct _tmp1D9;_tmp1D9.tag=4;_tmp1D9.f1=d;
_tmp1D9;});_tmp1D8;}),0);_tmp1D7->tl=tds;_tmp1D7;});}static struct Cyc_List_List*
Cyc_Interface_add_structdecl_header(struct _tagged_arr*x,struct Cyc_Absyn_Structdecl*
d,struct Cyc_List_List*tds){d=({struct Cyc_Absyn_Structdecl*_tmp1DA=_cycalloc(
sizeof(*_tmp1DA));_tmp1DA[0]=*d;_tmp1DA;});d->fields=0;if((void*)d->sc != (void*)
4){(void*)(d->sc=(void*)((void*)3));}return({struct Cyc_List_List*_tmp1DB=
_cycalloc(sizeof(*_tmp1DB));_tmp1DB->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Struct_d_struct*
_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC[0]=({struct Cyc_Absyn_Struct_d_struct
_tmp1DD;_tmp1DD.tag=4;_tmp1DD.f1=d;_tmp1DD;});_tmp1DC;}),0);_tmp1DB->tl=tds;
_tmp1DB;});}static struct Cyc_List_List*Cyc_Interface_add_uniondecl(struct
_tagged_arr*x,struct Cyc_Absyn_Uniondecl*d,struct Cyc_List_List*tds){return({
struct Cyc_List_List*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Union_d_struct*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));
_tmp1DF[0]=({struct Cyc_Absyn_Union_d_struct _tmp1E0;_tmp1E0.tag=5;_tmp1E0.f1=d;
_tmp1E0;});_tmp1DF;}),0);_tmp1DE->tl=tds;_tmp1DE;});}static struct Cyc_List_List*
Cyc_Interface_add_uniondecl_header(struct _tagged_arr*x,struct Cyc_Absyn_Uniondecl*
d,struct Cyc_List_List*tds){d=({struct Cyc_Absyn_Uniondecl*_tmp1E1=_cycalloc(
sizeof(*_tmp1E1));_tmp1E1[0]=*d;_tmp1E1;});d->fields=0;if((void*)d->sc != (void*)
4){(void*)(d->sc=(void*)((void*)3));}return({struct Cyc_List_List*_tmp1E2=
_cycalloc(sizeof(*_tmp1E2));_tmp1E2->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Union_d_struct*
_tmp1E3=_cycalloc(sizeof(*_tmp1E3));_tmp1E3[0]=({struct Cyc_Absyn_Union_d_struct
_tmp1E4;_tmp1E4.tag=5;_tmp1E4.f1=d;_tmp1E4;});_tmp1E3;}),0);_tmp1E2->tl=tds;
_tmp1E2;});}static struct Cyc_List_List*Cyc_Interface_add_tuniondecl(struct
_tagged_arr*x,struct Cyc_Absyn_Tuniondecl*d,struct Cyc_List_List*tds){return({
struct Cyc_List_List*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));_tmp1E5->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));
_tmp1E6[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp1E7;_tmp1E7.tag=6;_tmp1E7.f1=d;
_tmp1E7;});_tmp1E6;}),0);_tmp1E5->tl=tds;_tmp1E5;});}static unsigned char _tmp1E8[
2]="_";static struct _tagged_arr Cyc_Interface_us={_tmp1E8,_tmp1E8,_tmp1E8 + 2};
static struct _tagged_arr*Cyc_Interface_us_p=& Cyc_Interface_us;static struct _tuple9*
Cyc_Interface_rewrite_tunionfield_type(struct _tuple9*x){return({struct _tuple9*
_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->f1=Cyc_Absyn_empty_tqual();_tmp1E9->f2=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp1EA=_cycalloc(sizeof(*_tmp1EA));
_tmp1EA[0]=({struct Cyc_Absyn_VarType_struct _tmp1EB;_tmp1EB.tag=1;_tmp1EB.f1=({
struct Cyc_Absyn_Tvar*_tmp1EC=_cycalloc(sizeof(*_tmp1EC));_tmp1EC->name=Cyc_Interface_us_p;
_tmp1EC->identity=0;_tmp1EC->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*
_tmp1ED=_cycalloc(sizeof(*_tmp1ED));_tmp1ED[0]=({struct Cyc_Absyn_Eq_kb_struct
_tmp1EE;_tmp1EE.tag=0;_tmp1EE.f1=(void*)((void*)2);_tmp1EE;});_tmp1ED;}));
_tmp1EC;});_tmp1EB;});_tmp1EA;});_tmp1E9;});}static struct Cyc_Absyn_Tunionfield*
Cyc_Interface_rewrite_tunionfield(struct Cyc_Absyn_Tunionfield*f){f=({struct Cyc_Absyn_Tunionfield*
_tmp1EF=_cycalloc(sizeof(*_tmp1EF));_tmp1EF[0]=*f;_tmp1EF;});f->typs=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_tunionfield_type,
f->typs);return f;}static struct Cyc_List_List*Cyc_Interface_add_tuniondecl_header(
struct _tagged_arr*x,struct Cyc_Absyn_Tuniondecl*d,struct Cyc_List_List*tds){d=({
struct Cyc_Absyn_Tuniondecl*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));_tmp1F0[0]=*d;
_tmp1F0;});if(d->fields != 0){d->fields=({struct Cyc_Core_Opt*_tmp1F1=_cycalloc(
sizeof(*_tmp1F1));_tmp1F1->v=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tunionfield*(*
f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Interface_rewrite_tunionfield,(
struct Cyc_List_List*)(d->fields)->v);_tmp1F1;});}if((void*)d->sc != (void*)4){(
void*)(d->sc=(void*)((void*)3));}return({struct Cyc_List_List*_tmp1F2=_cycalloc(
sizeof(*_tmp1F2));_tmp1F2->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp1F3=_cycalloc(sizeof(*_tmp1F3));_tmp1F3[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp1F4;_tmp1F4.tag=6;_tmp1F4.f1=d;_tmp1F4;});_tmp1F3;}),0);_tmp1F2->tl=tds;
_tmp1F2;});}static struct Cyc_List_List*Cyc_Interface_add_enumdecl(struct
_tagged_arr*x,struct Cyc_Absyn_Enumdecl*d,struct Cyc_List_List*tds){return({struct
Cyc_List_List*_tmp1F5=_cycalloc(sizeof(*_tmp1F5));_tmp1F5->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Enum_d_struct*_tmp1F6=_cycalloc(sizeof(*_tmp1F6));
_tmp1F6[0]=({struct Cyc_Absyn_Enum_d_struct _tmp1F7;_tmp1F7.tag=7;_tmp1F7.f1=d;
_tmp1F7;});_tmp1F6;}),0);_tmp1F5->tl=tds;_tmp1F5;});}static struct Cyc_List_List*
Cyc_Interface_add_enumdecl_header(struct _tagged_arr*x,struct Cyc_Absyn_Enumdecl*d,
struct Cyc_List_List*tds){d=({struct Cyc_Absyn_Enumdecl*_tmp1F8=_cycalloc(sizeof(*
_tmp1F8));_tmp1F8[0]=*d;_tmp1F8;});d->fields=0;if((void*)d->sc != (void*)4){(void*)(
d->sc=(void*)((void*)3));}return({struct Cyc_List_List*_tmp1F9=_cycalloc(sizeof(*
_tmp1F9));_tmp1F9->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp1FA=_cycalloc(sizeof(*_tmp1FA));_tmp1FA[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp1FB;_tmp1FB.tag=7;_tmp1FB.f1=d;_tmp1FB;});_tmp1FA;}),0);_tmp1F9->tl=tds;
_tmp1F9;});}static struct Cyc_List_List*Cyc_Interface_add_typedef(struct
_tagged_arr*x,struct Cyc_Absyn_Typedefdecl*d,struct Cyc_List_List*tds){return({
struct Cyc_List_List*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));_tmp1FC->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Typedef_d_struct*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));
_tmp1FD[0]=({struct Cyc_Absyn_Typedef_d_struct _tmp1FE;_tmp1FE.tag=8;_tmp1FE.f1=d;
_tmp1FE;});_tmp1FD;}),0);_tmp1FC->tl=tds;_tmp1FC;});}static struct Cyc_List_List*
Cyc_Interface_add_vardecl(struct _tagged_arr*x,struct Cyc_Absyn_Vardecl*d,struct
Cyc_List_List*tds){return({struct Cyc_List_List*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));
_tmp1FF->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp200=
_cycalloc(sizeof(*_tmp200));_tmp200[0]=({struct Cyc_Absyn_Var_d_struct _tmp201;
_tmp201.tag=0;_tmp201.f1=d;_tmp201;});_tmp200;}),0);_tmp1FF->tl=tds;_tmp1FF;});}
static struct Cyc_List_List*Cyc_Interface_add_xtunionfielddecl(struct _tagged_arr*x,
struct Cyc_Tcdecl_Xtunionfielddecl*d,struct Cyc_List_List*tds){struct Cyc_Tcdecl_Xtunionfielddecl
_tmp203;struct Cyc_Absyn_Tunionfield*_tmp204;struct Cyc_Absyn_Tuniondecl*_tmp205;
struct Cyc_Tcdecl_Xtunionfielddecl*_tmp202=d;_tmp203=*_tmp202;_LL135: _tmp205=
_tmp203.base;goto _LL134;_LL134: _tmp204=_tmp203.field;goto _LL133;_LL133: _tmp205=({
struct Cyc_Absyn_Tuniondecl*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206[0]=*
_tmp205;_tmp206;});_tmp205->fields=({struct Cyc_Core_Opt*_tmp207=_cycalloc(
sizeof(*_tmp207));_tmp207->v=({struct Cyc_List_List*_tmp208=_cycalloc(sizeof(*
_tmp208));_tmp208->hd=_tmp204;_tmp208->tl=0;_tmp208;});_tmp207;});(void*)(
_tmp205->sc=(void*)((void*)3));return({struct Cyc_List_List*_tmp209=_cycalloc(
sizeof(*_tmp209));_tmp209->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp20B;_tmp20B.tag=6;_tmp20B.f1=_tmp205;_tmp20B;});_tmp20A;}),0);_tmp209->tl=
tds;_tmp209;});}static struct Cyc_List_List*Cyc_Interface_add_xtunionfielddecl_header(
struct _tagged_arr*x,struct Cyc_Tcdecl_Xtunionfielddecl*d,struct Cyc_List_List*tds){
struct Cyc_Tcdecl_Xtunionfielddecl _tmp20D;struct Cyc_Absyn_Tunionfield*_tmp20E;
struct Cyc_Absyn_Tuniondecl*_tmp20F;struct Cyc_Tcdecl_Xtunionfielddecl*_tmp20C=d;
_tmp20D=*_tmp20C;_LL138: _tmp20F=_tmp20D.base;goto _LL137;_LL137: _tmp20E=_tmp20D.field;
goto _LL136;_LL136: _tmp20F=({struct Cyc_Absyn_Tuniondecl*_tmp210=_cycalloc(sizeof(*
_tmp210));_tmp210[0]=*_tmp20F;_tmp210;});_tmp20E=Cyc_Interface_rewrite_tunionfield(
_tmp20E);(void*)(_tmp20E->sc=(void*)((void*)3));_tmp20F->fields=({struct Cyc_Core_Opt*
_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->v=({struct Cyc_List_List*_tmp212=
_cycalloc(sizeof(*_tmp212));_tmp212->hd=_tmp20E;_tmp212->tl=0;_tmp212;});_tmp211;});(
void*)(_tmp20F->sc=(void*)((void*)3));return({struct Cyc_List_List*_tmp213=
_cycalloc(sizeof(*_tmp213));_tmp213->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*
_tmp214=_cycalloc(sizeof(*_tmp214));_tmp214[0]=({struct Cyc_Absyn_Tunion_d_struct
_tmp215;_tmp215.tag=6;_tmp215.f1=_tmp20F;_tmp215;});_tmp214;}),0);_tmp213->tl=
tds;_tmp213;});}static void Cyc_Interface_print_ns_headers(struct Cyc_Std___sFILE*f,
struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*_tmp216=
0;_tmp216=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _tagged_arr*,
struct Cyc_Absyn_Structdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*d,struct
Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_structdecl_header,ie->structdecls,
_tmp216);_tmp216=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Absyn_Uniondecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_uniondecl_header,
ie->uniondecls,_tmp216);_tmp216=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _tagged_arr*,struct Cyc_Absyn_Tuniondecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_tuniondecl_header,
ie->tuniondecls,_tmp216);_tmp216=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _tagged_arr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl_header,ie->enumdecls,
_tmp216);if(_tmp216 != 0){_tmp216=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp216);_tmp216=Cyc_Interface_add_namespace(_tmp216,ns);Cyc_Absynpp_decllist2file(
_tmp216,f);}}static void Cyc_Interface_print_ns_xtunionfielddecl_headers(struct Cyc_Std___sFILE*
f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){struct Cyc_List_List*
_tmp217=0;_tmp217=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Tcdecl_Xtunionfielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xtunionfielddecl_header,
ie->xtunionfielddecls,_tmp217);if(_tmp217 != 0){_tmp217=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp217);_tmp217=Cyc_Interface_add_namespace(
_tmp217,ns);Cyc_Absynpp_decllist2file(_tmp217,f);}}static void Cyc_Interface_print_ns_typedefs(
struct Cyc_Std___sFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp218=0;_tmp218=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _tagged_arr*,struct Cyc_Absyn_Typedefdecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_typedef,
ie->typedefdecls,_tmp218);if(_tmp218 != 0){_tmp218=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp218);_tmp218=Cyc_Interface_add_namespace(
_tmp218,ns);Cyc_Absynpp_decllist2file(_tmp218,f);}}static void Cyc_Interface_print_ns_decls(
struct Cyc_Std___sFILE*f,struct Cyc_List_List*ns,struct Cyc_Interface_Ienv*ie){
struct Cyc_List_List*_tmp219=0;_tmp219=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _tagged_arr*,struct Cyc_Absyn_Structdecl*,struct Cyc_List_List*),struct
Cyc_Dict_Dict*d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_structdecl,
ie->structdecls,_tmp219);_tmp219=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*
f)(struct _tagged_arr*,struct Cyc_Absyn_Uniondecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_uniondecl,ie->uniondecls,
_tmp219);_tmp219=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Absyn_Tuniondecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_tuniondecl,ie->tuniondecls,
_tmp219);_tmp219=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Absyn_Enumdecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_enumdecl,ie->enumdecls,
_tmp219);_tmp219=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_vardecl,ie->vardecls,
_tmp219);_tmp219=((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct
_tagged_arr*,struct Cyc_Tcdecl_Xtunionfielddecl*,struct Cyc_List_List*),struct Cyc_Dict_Dict*
d,struct Cyc_List_List*accum))Cyc_Dict_fold)(Cyc_Interface_add_xtunionfielddecl,
ie->xtunionfielddecls,_tmp219);if(_tmp219 != 0){_tmp219=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp219);_tmp219=Cyc_Interface_add_namespace(
_tmp219,ns);Cyc_Absynpp_decllist2file(_tmp219,f);}}void Cyc_Interface_print(
struct Cyc_Interface_I*i,struct Cyc_Std___sFILE*f){Cyc_Absynpp_set_params(& Cyc_Absynpp_cyci_params_r);({
void*_tmp21A[0]={};Cyc_Std_fprintf(f,_tag_arr("/****** needed (headers) ******/\n",
sizeof(unsigned char),34),_tag_arr(_tmp21A,sizeof(void*),0));});((void(*)(void(*
f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
Cyc_Std___sFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,
f,i->imports);({void*_tmp21B[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** provided (headers) ******/\n",
sizeof(unsigned char),37),_tag_arr(_tmp21B,sizeof(void*),0));});((void(*)(void(*
f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
Cyc_Std___sFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_headers,
f,i->exports);({void*_tmp21C[0]={};Cyc_Std_fprintf(f,_tag_arr("/****** needed (headers of xtunionfielddecls) ******/\n",
sizeof(unsigned char),55),_tag_arr(_tmp21C,sizeof(void*),0));});((void(*)(void(*
f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
Cyc_Std___sFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xtunionfielddecl_headers,
f,i->imports);({void*_tmp21D[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** provided (headers of xtunionfielddecls) ******/\n",
sizeof(unsigned char),58),_tag_arr(_tmp21D,sizeof(void*),0));});((void(*)(void(*
f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
Cyc_Std___sFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_xtunionfielddecl_headers,
f,i->exports);({void*_tmp21E[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** provided (typedefs) ******/\n",
sizeof(unsigned char),38),_tag_arr(_tmp21E,sizeof(void*),0));});((void(*)(void(*
f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
Cyc_Std___sFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_typedefs,
f,i->exports);({void*_tmp21F[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** needed (declarations) ******/\n",
sizeof(unsigned char),40),_tag_arr(_tmp21F,sizeof(void*),0));});((void(*)(void(*
f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
Cyc_Std___sFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,
f,i->imports);({void*_tmp220[0]={};Cyc_Std_fprintf(f,_tag_arr("\n/****** provided (declarations) ******/\n",
sizeof(unsigned char),42),_tag_arr(_tmp220,sizeof(void*),0));});((void(*)(void(*
f)(struct Cyc_Std___sFILE*,struct Cyc_List_List*,struct Cyc_Interface_Ienv*),struct
Cyc_Std___sFILE*env,struct Cyc_Dict_Dict*d))Cyc_Dict_iter_c)(Cyc_Interface_print_ns_decls,
f,i->exports);}struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc_Std___sFILE*f){
Cyc_Lex_lex_init();{struct Cyc_List_List*_tmp221=Cyc_Parse_parse_file(f);struct
Cyc_Tcenv_Tenv*_tmp222=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp222,0,_tmp221);return
Cyc_Interface_gen_extract(_tmp222->ae,0);}}void Cyc_Interface_save(struct Cyc_Interface_I*
i,struct Cyc_Std___sFILE*f){Cyc_Interface_print(i,f);}struct Cyc_Interface_I*Cyc_Interface_load(
struct Cyc_Std___sFILE*f){return Cyc_Interface_parse(f);}
